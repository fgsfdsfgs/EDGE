//------------------------------------------------------------------------
//  Clients
//------------------------------------------------------------------------
//
//  Edge MultiPlayer Server (C) 2004  Andrew Apted
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//------------------------------------------------------------------------

#include "includes.h"

#include "autolock.h"
#include "client.h"
#include "mp_main.h"
#include "network.h"
#include "packet.h"
#include "protocol.h"
#include "ui_log.h"

//
// Client list
//
std::vector<client_c *> clients;


client_c::client_c(const client_info_t *info, const NLaddress *_addr) :
	state(ST_Browsing), name(info->name),
	game_id(-1), player_id(-1), voted(false)
{
	memcpy(&addr, _addr, sizeof(addr));
}

client_c::~client_c()
{
	/* nothing to do */
}

bool client_c::CheckAddr(const NLaddress *remote_addr) const
{
	return (nlAddrCompare(&addr, remote_addr) == NL_TRUE);
}

bool client_c::Verify(const NLaddress *remote_addr) const
{
	if (state == ST_Going || state == ST_Gone)
		return false;

	return (nlAddrCompare(&addr, remote_addr) == NL_TRUE);
}

int client_c::CompareName(const char *other) const
{
	// FIXME!!!! case insensitive
	return strcmp(name.c_str(), other);
}

void client_c::FillClientInfo(client_info_t *info) const
{
	switch (state)
	{
		case ST_Going:
		case ST_Gone:
			info->state = client_info_t::CS_NotExist;
			return;  /* no other info */

		case ST_Browsing: info->state = client_info_t::CS_Browsing; break;
		case ST_Playing:  info->state = client_info_t::CS_Playing;  break;

		case ST_Queueing: info->state = voted ? client_info_t::CS_Voted :
						  client_info_t::CS_Queueing;
			break;

		default:
			AssertFail("Bad client state %d\n", info->state);
			break;
	}

	strcpy(info->name, name.c_str());

	info->game_id = game_id;
}

bool client_c::MatchDest(int dest, int game) const
{
	SYS_ASSERT(dest < 0);

	switch (dest)
	{
		case header_proto_t::D_ALL_BROWSING:
			return (state == ST_Browsing);

		case header_proto_t::D_ALL_NOT_PLAYING:
			return (state != ST_Playing);

		case header_proto_t::D_OTHER_QUEUERS:
			return (state == ST_Queueing && game_id == game);

		case header_proto_t::D_OTHER_PLAYERS:
			return (state == ST_Playing && game_id == game);

		case header_proto_t::D_ABSOLUTELY_EVERYONE:
			return true;

		default:
			// this shouldn't happen (already been checked)
			AssertFail("Bad special dest %d\n", dest);
			break;
	}

	return false;
}

void client_c::TransmitMessage(packet_c *pk)
{
	nlSetRemoteAddr(main_socket, &addr);

	pk->Write(main_socket);

	pk->hd().ByteSwap();  // for subsequent usage
}


//------------------------------------------------------------------------

bool ClientExists(short idx)
{
	if (idx < 0 || (unsigned)idx >= clients.size())
		return false;

	return clients[idx] != NULL;
}

bool VerifyClient(short idx, const NLaddress *remote_addr)
{
	return ClientExists(idx) ? clients[idx]->Verify(remote_addr) : false;
}

bool ValidatePlayerName(const char *name)
{
	// FIXME: better check
	
	if (name[0] == 0)
		return false;
	
	return true;
}

//------------------------------------------------------------------------

void SV_send_error(packet_c *pk, short type, const char *str, ...)
{
	char buffer[2048];
	
	va_list args;

	va_start(args, str);
	vsprintf(buffer, str, args);
	va_end(args);

	buffer[error_proto_t::ERR_STR_MAX] = 0;  // limit message length

	/* data */

	pk->er_p().type = type;
	pk->er_p().param[0] = 0;
	pk->er_p().param[1] = 0;

	strcpy(pk->er_p().message, buffer);

	pk->er_p().ByteSwap();

	/* header */

	int len = strlen(buffer) + sizeof(error_proto_t);
	SYS_ASSERT(pk->CanHold(len));

	pk->SetType("Er");
	pk->hd().flags = 0;
	pk->hd().data_len = len;

	pk->Write(main_socket);
}

void PK_connect_to_server(packet_c *pk, NLaddress *remote_addr)
{
	// FIXME: check if too many games

	int client_id;

	// check if not already connected
	// Note: don't need to lock here, since other thread never modifies
	//       the data (only copies it).

	for (client_id = 0; (unsigned)client_id < clients.size(); client_id++)
	{
		client_c *CL = clients[client_id];

		if (! CL)
			continue;

		if (CL->CheckAddr(remote_addr))
		{
			SV_send_error(pk, error_proto_t::ERR_AlreadyConnected, "Already connected !");
			LogPrintf(2, "Client %d was already connected.\n", client_id);
			return;
		}
	}

	// determine client ID#
	for (client_id = 0; (unsigned)client_id < clients.size(); client_id++)
	{
		if (clients[client_id] == NULL)
			break;
	}

	connect_proto_t& con = pk->cs_p();

	// FIXME: check data_len

	con.ByteSwap();
	con.info.name[client_info_t::NAME_LEN-1] = 0;  // ensure NUL-terminated

	if (! ValidatePlayerName(con.info.name))
	{
		SV_send_error(pk, error_proto_t::ERR_Unknown, "Invalid name !");
		LogPrintf(1, "Client %d tried to connect with invalid name.\n", client_id);
		return;
	}

	// check if name already used
	for (int c2 = 0; (unsigned)c2 < clients.size(); c2++)
	{
		client_c *CL = clients[c2];

		if (! CL || c2 == client_id)
			continue;

		if (CL->CompareName(con.info.name) == 0)
		{
			SV_send_error(pk, error_proto_t::ERR_Unknown, "Name already in use !");
			LogPrintf(2, "Client %d tried to connect, name was already used.\n", client_id);
			return;
		}
	}

	// LOCK STRUCTURES
	{
		autolock_c LOCK(&global_lock);

		client_c *CL = new client_c(&con.info, remote_addr);

		SYS_ASSERT((unsigned)client_id <= clients.size());

		if ((unsigned)client_id == clients.size())
			clients.push_back(CL);
		else
			clients[client_id] = CL;
	}
	// NOW UNLOCKED

	// successful!

	con.info.game_id = -1;
	con.info.state = client_info_t::CS_Browsing;

	con.ByteSwap();

	pk->SetType("Cs");

	pk->hd().source = client_id;
	pk->hd().flags = 0;
	pk->hd().data_len = 0;
	pk->hd().game = -1;

	pk->Write(main_socket);

	LogPrintf(0, "New client %d: name '%s' addr %s\n", client_id, "Player",
		GetAddrName(remote_addr));
}

void PK_leave_server(packet_c *pk)
{
	client_c *CL = clients[pk->hd().source];

	// client actually removed during timeout handling
	CL->state = client_c::ST_Going;
	LogPrintf(0, "Client %d has disconnected.\n", pk->hd().source);

	// FIXME: !!! remove from queue/game

	pk->SetType("Ls");

	pk->hd().flags = 0;
	pk->hd().data_len = 0;
	pk->hd().game = -1;

	pk->Write(main_socket);
}

void PK_broadcast_discovery(packet_c *pk)
{
	// very simple: just send it back!
	// (client will get our address and port)

	pk->SetType("Bd");

	pk->Write(main_socket);
}

void PK_keep_alive(packet_c *pk)
{
	client_c *CL = clients[pk->hd().source];

	CL->alive_millies = 0;
}

void PK_query_client(packet_c *pk)
{
	query_client_proto_t& qc = pk->qc_p();

	qc.ByteSwap(false);

	// FIXME: check data_len

	short cur_id = qc.first_id;
	byte  total  = qc.count;

	if (total == 0)
		return;

	// prepare packet header
	pk->SetType("Qc");

	pk->hd().flags = 0;
	pk->hd().data_len = sizeof(query_client_proto_t) +
		(qc.count - 1) * sizeof(client_info_t);

	while (total > 0)
	{
		qc.first_id = cur_id;
		qc.count = MIN(total, query_client_proto_t::CLIENT_FIT);
		
		total -= qc.count;

		for (byte i = 0; i < qc.count; i++)
		{
			if (! ClientExists(qc.first_id + i))
			{
				qc.info[i].state = client_info_t::CS_NotExist;
				continue;
			}

			client_c *CL = clients[qc.first_id + i];
			SYS_NULL_CHECK(CL);

			CL->FillClientInfo(qc.info + i);
		}

		qc.ByteSwap(true);

		pk->Write(main_socket);

		pk->hd().ByteSwap();  // FIXME: rebuild header each time
	}
}

void PK_message(packet_c *pk)
{
	int client_id = pk->hd().source;
	int dest_id   = pk->hd().dest;

	int game_id = clients[client_id]->game_id;

	pk->SetType("Ms");
	pk->hd().game = game_id;

	if (dest_id >= 0 && ClientExists(dest_id))
	{
		client_c *CL = clients[dest_id];
		CL->TransmitMessage(pk);
		return;
	}

	switch (dest_id)
	{
		case header_proto_t::D_ALL_BROWSING:
		case header_proto_t::D_ALL_NOT_PLAYING:
		case header_proto_t::D_OTHER_QUEUERS:
		case header_proto_t::D_OTHER_PLAYERS:
		case header_proto_t::D_ABSOLUTELY_EVERYONE:
		{
			for (int c = 0; (unsigned)c < clients.size(); c++)
			{
				client_c *CL = clients[c];
				
				if (! CL)
					continue;

				// Fixme: have a "not me too" flag
				// if (c == client_id && ! me_too) continue;

				// handle special destinations
				if (dest_id < 0 && ! CL->MatchDest(dest_id, game_id))
					continue;

				CL->TransmitMessage(pk);
			}
		}
		break;

		default:
			// FIXME: log and/or error packet
			break;
	}
}

