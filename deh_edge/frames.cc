//------------------------------------------------------------------------
//  FRAME Handling
//------------------------------------------------------------------------
//
//  DEH_EDGE  Copyright (C) 2004  The EDGE Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License (in COPYING.txt) for more details.
//
//------------------------------------------------------------------------
//
//  DEH_EDGE is based on:
//
//  +  DeHackEd source code, by Greg Lewis.
//  -  DOOM source code (C) 1993-1996 id Software, Inc.
//  -  Linux DOOM Hack Editor, by Sam Lantinga.
//  -  PrBoom's DEH/BEX code, by Ty Halderman, TeamTNT.
//
//------------------------------------------------------------------------

#include "i_defs.h"
#include "frames.h"

#include "info.h"
#include "system.h"
#include "things.h"
#include "wad.h"
#include "weapons.h"


#define DEBUG_RANGES  0  // must enable one in info.cpp too


const char *Frames::attack_slot[3];

int Frames::act_flags;

namespace Frames
{
	int lowest_touched;
	int highest_touched;
}


#define IS_WEAPON(group)  (islower(group))


typedef struct
{
	const char *ddf_name;

	int act_flags;

	// attacks implied by the action, often NULL.  The format is
	// "X:ATTACK_NAME" where X is 'R' for range attacks, 'C' for
	// close-combat attacks, and 'S' for spare attacks.
	const char *atk_1;
	const char *atk_2;
}
actioninfo_t;

static const actioninfo_t action_info[NUMACTIONS] =
{
    { "NOTHING", 0, NULL, NULL },     // A_NULL
    { "LIGHT0", 0, NULL, NULL },      // A_Light0
    { "READY", 0, NULL, NULL },       // A_WeaponReady
    { "LOWER", 0, NULL, NULL },       // A_Lower
    { "RAISE", 0, NULL, NULL },       // A_Raise
    { "SHOOT", 0, "C:PLAYER_PUNCH", NULL },       // A_Punch
    { "REFIRE", 0, NULL, NULL },      // A_ReFire
    { "SHOOT", 0, "R:PLAYER_PISTOL", NULL },       // A_FirePistol
    { "LIGHT1", 0, NULL, NULL },      // A_Light1
    { "SHOOT", 0, "R:PLAYER_SHOTGUN", NULL },       // A_FireShotgun
    { "LIGHT2", 0, NULL, NULL },      // A_Light2
    { "SHOOT", 0, "R:PLAYER_SHOTGUN2", NULL },       // A_FireShotgun2
    { "CHECKRELOAD", 0, NULL, NULL }, // A_CheckReload
    { "PLAYSOUND(DBOPN)", 0, NULL, NULL },  // A_OpenShotgun2
    { "PLAYSOUND(DBLOAD)", 0, NULL, NULL }, // A_LoadShotgun2
    { "PLAYSOUND(DBCLS)", 0, NULL, NULL },  // A_CloseShotgun2
    { "SHOOT", 0, "R:PLAYER_CHAINGUN", NULL },      // A_FireCGun
    { "FLASH", 0, NULL, NULL },      // A_GunFlash
    { "SHOOT", 0, "R:PLAYER_MISSILE", NULL },      // A_FireMissile
    { "SHOOT", 0, "C:PLAYER_SAW", NULL },      // A_Saw
    { "SHOOT", 0, "R:PLAYER_PLASMA", NULL },      // A_FirePlasma
    { "PLAYSOUND(BFG)", 0, NULL, NULL },      // A_BFGsound
    { "SHOOT", 0, "R:PLAYER_BFG9000", NULL },      // A_FireBFG
    { "SPARE_ATTACK", 0, NULL, NULL },      // A_BFGSpray
    { "EXPLOSIONDAMAGE", AF_EXPLODE, NULL, NULL },      // A_Explode
    { "MAKEPAINSOUND", 0, NULL, NULL },      // A_Pain
    { "PLAYER_SCREAM", 0, NULL, NULL },      // A_PlayerScream
    { "MAKEDEAD", 0, NULL, NULL },      // A_Fall
    { "MAKEOVERKILLSOUND", 0, NULL, NULL },      // A_XScream
    { "LOOKOUT", 0, NULL, NULL },      // A_Look
    { "CHASE", 0, NULL, NULL },      // A_Chase
    { "FACETARGET", 0, NULL, NULL },      // A_FaceTarget
    { "RANGE_ATTACK", 0, "R:FORMER_HUMAN_PISTOL", NULL },      // A_PosAttack
    { "MAKEDEATHSOUND", 0, NULL, NULL },      // A_Scream
    { "RANGE_ATTACK", 0, "R:FORMER_HUMAN_SHOTGUN", NULL },      // A_SPosAttack
    { "RESCHASE", 0, NULL, NULL },      // A_VileChase
    { "RANGEATTEMPTSND", 0, NULL, NULL },      // A_VileStart
    { "RANGE_ATTACK", 0, "R:ARCHVILE_FIRE", NULL },      // A_VileTarget
    { "EFFECTTRACKER", 0, NULL, NULL },      // A_VileAttack
    { "TRACKERSTART", 0, NULL, NULL },      // A_StartFire
    { "TRACKERFOLLOW", 0, NULL, NULL },      // A_Fire
    { "TRACKERACTIVE", 0, NULL, NULL },      // A_FireCrackle
    { "RANDOM_TRACER", 0, NULL, NULL },      // A_Tracer
    { "CLOSEATTEMPTSND", 0, NULL, NULL },      // A_SkelWhoosh
    { "CLOSE_ATTACK", 0, "C:REVENANT_CLOSECOMBAT", NULL },      // A_SkelFist
    { "RANGE_ATTACK", 0, "R:REVENANT_MISSILE", NULL },      // A_SkelMissile
    { "RANGEATTEMPTSND", 0, NULL, NULL },      // A_FatRaise
    { "RESET_SPREADER", 0, NULL, NULL },      // A_FatAttack1
    { "RANGE_ATTACK", 0, "R:MANCUBUS_FIREBALL", NULL },      // A_FatAttack2
    { "RANGE_ATTACK", 0, "R:MANCUBUS_FIREBALL", NULL },      // A_FatAttack3
    { "NOTHING", 0, NULL, NULL },      // A_BossDeath
    { "RANGE_ATTACK", 0, "R:FORMER_HUMAN_CHAINGUN", NULL },      // A_CPosAttack
    { "REFIRE_CHECK", 0, NULL, NULL },      // A_CPosRefire
    { "COMBOATTACK", 0, "R:IMP_FIREBALL", "C:IMP_CLOSECOMBAT" }, // A_TroopAttack
    { "CLOSE_ATTACK", 0, "C:DEMON_CLOSECOMBAT", NULL },      // A_SargAttack
    { "COMBOATTACK", 0, "R:CACO_FIREBALL", "C:CACO_CLOSECOMBAT" }, // A_HeadAttack
    { "COMBOATTACK", 0, "R:BARON_FIREBALL", "C:BARON_CLOSECOMBAT" }, // A_BruisAttack
    { "RANGE_ATTACK", 0, "R:SKULL_ASSAULT", NULL },      // A_SkullAttack
    { "WALKSOUND_CHASE", 0, NULL, NULL },      // A_Metal
    { "REFIRE_CHECK", 0, NULL, NULL },      // A_SpidRefire
    { "WALKSOUND_CHASE", 0, NULL, NULL },      // A_BabyMetal
    { "RANGE_ATTACK", 0, "R:ARACHNOTRON_PLASMA", NULL },      // A_BspiAttack
    { "PLAYSOUND(HOOF)", 0, NULL, NULL },      // A_Hoof
    { "RANGE_ATTACK", 0, "R:CYBERDEMON_MISSILE", NULL },      // A_CyberAttack
    { "RANGE_ATTACK", 0, "R:ELEMENTAL_SPAWNER", NULL },      // A_PainAttack
    { "SPARE_ATTACK", 0, "S:ELEMENTAL_DEATHSPAWN", NULL },      // A_PainDie
    { "NOTHING", 0, NULL, NULL },      // A_KeenDie
    { "MAKEPAINSOUND", 0, NULL, NULL },      // A_BrainPain
    { "BRAINSCREAM", 0, NULL, NULL },      // A_BrainScream
    { "BRAINDIE", 0, NULL, NULL },      // A_BrainDie
    { "NOTHING", 0, NULL, NULL },      // A_BrainAwake
    { "BRAINSPIT", 0, "R:BRAIN_CUBE", NULL },      // A_BrainSpit
    { "MAKEACTIVESOUND", 0, NULL, NULL },      // A_SpawnSound
    { "CUBETRACER", 0, NULL, NULL },      // A_SpawnFly
    { "BRAINMISSILEEXPLODE", 0, NULL, NULL },     // A_BrainExplode
    { "CUBESPAWN", 0, NULL, NULL }      // A_CubeSpawn (NEW)
};


//------------------------------------------------------------------------

typedef struct
{
	int obj_num;  // thing or weapon
	int start1, end1;
	int start2, end2;
}
staterange_t;

static const staterange_t thing_range[] =
{
	// Things...
    { MT_PLAYER, S_PLAY, S_PLAY_XDIE9, -1,-1 },
    { MT_POSSESSED, S_POSS_STND, S_POSS_RAISE4, -1,-1 },
    { MT_SHOTGUY, S_SPOS_STND, S_SPOS_RAISE5, -1,-1 },
    { MT_VILE, S_VILE_STND, S_VILE_DIE10, -1,-1 },
    { MT_UNDEAD, S_SKEL_STND, S_SKEL_RAISE6, -1,-1 },
    { MT_SMOKE, S_SMOKE1, S_SMOKE5, -1,-1 },
    { MT_FATSO, S_FATT_STND, S_FATT_RAISE8, -1,-1 },
    { MT_CHAINGUY, S_CPOS_STND, S_CPOS_RAISE7, -1,-1 },
    { MT_TROOP, S_TROO_STND, S_TROO_RAISE5, -1,-1 },
    { MT_SERGEANT, S_SARG_STND, S_SARG_RAISE6, -1,-1 },
    { MT_SHADOWS, S_SARG_STND, S_SARG_RAISE6, -1,-1 },
    { MT_HEAD, S_HEAD_STND, S_HEAD_RAISE6, -1,-1 },
    { MT_BRUISER, S_BOSS_STND, S_BOSS_RAISE7, -1,-1 },
    { MT_KNIGHT, S_BOS2_STND, S_BOS2_RAISE7, -1,-1 },
    { MT_SKULL, S_SKULL_STND, S_SKULL_DIE6, -1,-1 },
    { MT_SPIDER, S_SPID_STND, S_SPID_DIE11, -1,-1 },
    { MT_BABY, S_BSPI_STND, S_BSPI_RAISE7, -1,-1 },
    { MT_CYBORG, S_CYBER_STND, S_CYBER_DIE10, -1,-1 },
    { MT_PAIN, S_PAIN_STND, S_PAIN_RAISE6, -1,-1 },
    { MT_WOLFSS, S_SSWV_STND, S_SSWV_RAISE5, -1,-1 },
    { MT_KEEN, S_KEENSTND, S_KEENPAIN2, -1,-1 },
    { MT_BOSSBRAIN, S_BRAIN, S_BRAIN_DIE4, -1,-1 },
    { MT_BOSSSPIT, S_BRAINEYE, S_BRAINEYE1, -1,-1 },
    { MT_BARREL, S_BAR1, S_BEXP5, -1,-1 },
    { MT_PUFF, S_PUFF1, S_PUFF4, -1,-1 },
    { MT_BLOOD, S_BLOOD1, S_BLOOD3, -1,-1 },
    { MT_TFOG, S_TFOG, S_TFOG10, -1,-1 },
    { MT_IFOG, S_IFOG, S_IFOG5, -1,-1 },
    { MT_TELEPORTMAN, S_TFOG, S_TFOG10, -1,-1 },

    { MT_MISC0, S_ARM1, S_ARM1A, -1,-1 },
    { MT_MISC1, S_ARM2, S_ARM2A, -1,-1 },
    { MT_MISC2, S_BON1, S_BON1E, -1,-1 },
    { MT_MISC3, S_BON2, S_BON2E, -1,-1 },
    { MT_MISC4, S_BKEY, S_BKEY2, -1,-1 },
    { MT_MISC5, S_RKEY, S_RKEY2, -1,-1 },
    { MT_MISC6, S_YKEY, S_YKEY2, -1,-1 },
    { MT_MISC7, S_YSKULL, S_YSKULL2, -1,-1 },
    { MT_MISC8, S_RSKULL, S_RSKULL2, -1,-1 },
    { MT_MISC9, S_BSKULL, S_BSKULL2, -1,-1 },
    { MT_MISC10, S_STIM, S_STIM, -1,-1 },
    { MT_MISC11, S_MEDI, S_MEDI, -1,-1 },
    { MT_MISC12, S_SOUL, S_SOUL6, -1,-1 },
    { MT_INV, S_PINV, S_PINV4, -1,-1 },
    { MT_MISC13, S_PSTR, S_PSTR, -1,-1 },
    { MT_INS, S_PINS, S_PINS4, -1,-1 },
    { MT_MISC14, S_SUIT, S_SUIT, -1,-1 },
    { MT_MISC15, S_PMAP, S_PMAP6, -1,-1 },
    { MT_MISC16, S_PVIS, S_PVIS2, -1,-1 },
    { MT_MEGA, S_MEGA, S_MEGA4, -1,-1 },
    { MT_CLIP, S_CLIP, S_CLIP, -1,-1 },
    { MT_MISC17, S_AMMO, S_AMMO, -1,-1 },
    { MT_MISC18, S_ROCK, S_ROCK, -1,-1 },
    { MT_MISC19, S_BROK, S_BROK, -1,-1 },
    { MT_MISC20, S_CELL, S_CELL, -1,-1 },
    { MT_MISC21, S_CELP, S_CELP, -1,-1 },
    { MT_MISC22, S_SHEL, S_SHEL, -1,-1 },
    { MT_MISC23, S_SBOX, S_SBOX, -1,-1 },
    { MT_MISC24, S_BPAK, S_BPAK, -1,-1 },
    { MT_MISC25, S_BFUG, S_BFUG, -1,-1 },
    { MT_CHAINGUN, S_MGUN, S_MGUN, -1,-1 },
    { MT_MISC26, S_CSAW, S_CSAW, -1,-1 },
    { MT_MISC27, S_LAUN, S_LAUN, -1,-1 },
    { MT_MISC28, S_PLAS, S_PLAS, -1,-1 },
    { MT_SHOTGUN, S_SHOT, S_SHOT, -1,-1 },
    { MT_SUPERSHOTGUN, S_SHOT2, S_SHOT2, -1,-1 },
    { MT_MISC29, S_TECHLAMP, S_TECHLAMP4, -1,-1 },
    { MT_MISC30, S_TECH2LAMP, S_TECH2LAMP4, -1,-1 },
    { MT_MISC31, S_COLU, S_COLU, -1,-1 },
    { MT_MISC32, S_TALLGRNCOL, S_TALLGRNCOL, -1,-1 },
    { MT_MISC33, S_SHRTGRNCOL, S_SHRTGRNCOL, -1,-1 },
    { MT_MISC34, S_TALLREDCOL, S_TALLREDCOL, -1,-1 },
    { MT_MISC35, S_SHRTREDCOL, S_SHRTREDCOL, -1,-1 },
    { MT_MISC36, S_SKULLCOL, S_SKULLCOL, -1,-1 },
    { MT_MISC37, S_HEARTCOL, S_HEARTCOL2, -1,-1 },
    { MT_MISC38, S_EVILEYE, S_EVILEYE4, -1,-1 },
    { MT_MISC39, S_FLOATSKULL, S_FLOATSKULL3, -1,-1 },
    { MT_MISC40, S_TORCHTREE, S_TORCHTREE, -1,-1 },
    { MT_MISC41, S_BLUETORCH, S_BLUETORCH4, -1,-1 },
    { MT_MISC42, S_GREENTORCH, S_GREENTORCH4, -1,-1 },
    { MT_MISC43, S_REDTORCH, S_REDTORCH4, -1,-1 },
    { MT_MISC44, S_BTORCHSHRT, S_BTORCHSHRT4, -1,-1 },
    { MT_MISC45, S_GTORCHSHRT, S_GTORCHSHRT4, -1,-1 },
    { MT_MISC46, S_RTORCHSHRT, S_RTORCHSHRT4, -1,-1 },
    { MT_MISC47, S_STALAGTITE, S_STALAGTITE, -1,-1 },
    { MT_MISC48, S_TECHPILLAR, S_TECHPILLAR, -1,-1 },
    { MT_MISC49, S_CANDLESTIK, S_CANDLESTIK, -1,-1 },
    { MT_MISC50, S_CANDELABRA, S_CANDELABRA, -1,-1 },
    { MT_MISC51, S_BLOODYTWITCH, S_BLOODYTWITCH4, -1,-1 },
    { MT_MISC60, S_BLOODYTWITCH, S_BLOODYTWITCH4, -1,-1 },
    { MT_MISC52, S_MEAT2, S_MEAT2, -1,-1 },
    { MT_MISC53, S_MEAT3, S_MEAT3, -1,-1 },
    { MT_MISC54, S_MEAT4, S_MEAT4, -1,-1 },
    { MT_MISC55, S_MEAT5, S_MEAT5, -1,-1 },
    { MT_MISC56, S_MEAT2, S_MEAT2, -1,-1 },
    { MT_MISC57, S_MEAT4, S_MEAT4, -1,-1 },
    { MT_MISC58, S_MEAT3, S_MEAT3, -1,-1 },
    { MT_MISC59, S_MEAT5, S_MEAT5, -1,-1 },
    { MT_MISC61, S_HEAD_DIE6, S_HEAD_DIE6, -1,-1 },
    { MT_MISC62, S_PLAY_DIE7, S_PLAY_DIE7, -1,-1 },
    { MT_MISC63, S_POSS_DIE5, S_POSS_DIE5, -1,-1 },
    { MT_MISC64, S_SARG_DIE6, S_SARG_DIE6, -1,-1 },
    { MT_MISC65, S_SKULL_DIE6, S_SKULL_DIE6, -1,-1 },
    { MT_MISC66, S_TROO_DIE5, S_TROO_DIE5, -1,-1 },
    { MT_MISC67, S_SPOS_DIE5, S_SPOS_DIE5, -1,-1 },
    { MT_MISC68, S_PLAY_XDIE9, S_PLAY_XDIE9, -1,-1 },
    { MT_MISC69, S_PLAY_XDIE9, S_PLAY_XDIE9, -1,-1 },
    { MT_MISC70, S_HEADSONSTICK, S_HEADSONSTICK, -1,-1 },
    { MT_MISC71, S_GIBS, S_GIBS, -1,-1 },
    { MT_MISC72, S_HEADONASTICK, S_HEADONASTICK, -1,-1 },
    { MT_MISC73, S_HEADCANDLES, S_HEADCANDLES2, -1,-1 },
    { MT_MISC74, S_DEADSTICK, S_DEADSTICK, -1,-1 },
    { MT_MISC75, S_LIVESTICK, S_LIVESTICK2, -1,-1 },
    { MT_MISC76, S_BIGTREE, S_BIGTREE, -1,-1 },
    { MT_MISC77, S_BBAR1, S_BBAR3, -1,-1 },
    { MT_MISC78, S_HANGNOGUTS, S_HANGNOGUTS, -1,-1 },
    { MT_MISC79, S_HANGBNOBRAIN, S_HANGBNOBRAIN, -1,-1 },
    { MT_MISC80, S_HANGTLOOKDN, S_HANGTLOOKDN, -1,-1 },
    { MT_MISC81, S_HANGTSKULL, S_HANGTSKULL, -1,-1 },
    { MT_MISC82, S_HANGTLOOKUP, S_HANGTLOOKUP, -1,-1 },
    { MT_MISC83, S_HANGTNOBRAIN, S_HANGTNOBRAIN, -1,-1 },
    { MT_MISC84, S_COLONGIBS, S_COLONGIBS, -1,-1 },
    { MT_MISC85, S_SMALLPOOL, S_SMALLPOOL, -1,-1 },
    { MT_MISC86, S_BRAINSTEM, S_BRAINSTEM, -1,-1 },

	/* BRAIN_DEATH_MISSILE : S_BRAINEXPLODE1, S_BRAINEXPLODE3 */

	// Attacks...
    { MT_FIRE, S_FIRE1, S_FIRE30, -1,-1 },
    { MT_TRACER, S_TRACER, S_TRACEEXP3, -1,-1 },
    { MT_FATSHOT, S_FATSHOT1, S_FATSHOTX3, -1,-1 },
    { MT_BRUISERSHOT, S_BRBALL1, S_BRBALLX3, -1,-1 },
    { MT_SPAWNSHOT, S_SPAWN1, S_SPAWNFIRE8, -1,-1 },
    { MT_TROOPSHOT, S_TBALL1, S_TBALLX3, -1,-1 },
    { MT_HEADSHOT, S_RBALL1, S_RBALLX3, -1,-1 },
    { MT_ARACHPLAZ, S_ARACH_PLAZ, S_ARACH_PLEX5, -1,-1 },
    { MT_ROCKET, S_ROCKET, S_ROCKET, S_EXPLODE1, S_EXPLODE3 },
    { MT_PLASMA, S_PLASBALL, S_PLASEXP5, -1,-1 },
    { MT_BFG, S_BFGSHOT, S_BFGLAND6, -1,-1 },
    { MT_EXTRABFG, S_BFGEXP, S_BFGEXP4, -1,-1 },

    { -1, -1,-1, -1, -1 }  // End sentinel
};

static const staterange_t weapon_range[] =
{
	// Weapons...
    { wp_fist, S_PUNCH, S_PUNCH5, -1,-1 },
    { wp_chainsaw, S_SAW, S_SAW3, -1,-1 },
    { wp_pistol, S_PISTOL, S_PISTOLFLASH, S_LIGHTDONE, S_LIGHTDONE },
    { wp_shotgun, S_SGUN, S_SGUNFLASH2, S_LIGHTDONE, S_LIGHTDONE },
    { wp_chaingun, S_CHAIN, S_CHAINFLASH2, S_LIGHTDONE, S_LIGHTDONE },
    { wp_missile, S_MISSILE, S_MISSILEFLASH4, S_LIGHTDONE, S_LIGHTDONE },
    { wp_plasma, S_PLASMA, S_PLASMAFLASH2, S_LIGHTDONE, S_LIGHTDONE },
    { wp_bfg, S_BFG, S_BFGFLASH2, S_LIGHTDONE, S_LIGHTDONE },
    { wp_supershotgun, S_DSGUN, S_DSGUNFLASH2, S_LIGHTDONE, S_LIGHTDONE },

    { -1, -1,-1, -1, -1 }  // End sentinel
};

namespace Frames
{
	void MarkStateUsers(int state)
	{
		if (state == S_NULL)
			return;

		if (state <= S_LAST_WEAPON_STATE)
		{
			for (int w = 0; weapon_range[w].obj_num >= 0; w++)
			{
				const staterange_t *R = weapon_range + w;

				if ((R->start1 <= state && state <= R->end1) ||
				    (R->start2 <= state && state <= R->end2))
				{
					weapon_modified[R->obj_num] = true;
				}
			}

			return;
		}

		// check things.

		for (int t = 0; thing_range[t].obj_num >= 0; t++)
		{
			const staterange_t *R = thing_range + t;

			if ((R->start1 <= state && state <= R->end1) ||
				(R->start2 <= state && state <= R->end2))
			{
				mobj_modified[R->obj_num] = true;
			}
		}
	}
}


//------------------------------------------------------------------------

void Frames::Startup(void)
{
	memset(state_dyn, 0, sizeof(state_dyn));
}

void Frames::ResetAll(void)
{
	for (int i = 0; i < NUMSTATES; i++)
	{
		state_dyn[i].group = state_dyn[i].gr_idx = 0;
	}

	attack_slot[0] = attack_slot[1] = attack_slot[2] = NULL;

	act_flags = 0;

	lowest_touched  = 99999;
	highest_touched = S_NULL;
}

int Frames::BeginGroup(int first, char group)
{
	if (first == S_NULL)
		return 0;

	state_dyn[first].group  = group;
	state_dyn[first].gr_idx = 1;

	return 1;
}

void Frames::SpreadGroups(void)
{
	bool changes;

	do
	{
		changes = false;

		for (int i = 0; i < NUMSTATES; i++)
		{
			if (state_dyn[i].group == 0)
				continue;

			int next = states[i].nextstate;

			if (next == S_NULL)
				continue;

			if (state_dyn[next].group != 0)
				continue;

			state_dyn[next].group  = state_dyn[i].group;
			state_dyn[next].gr_idx = state_dyn[i].gr_idx + 1;
		}
	}
	while (changes);
}

bool Frames::CheckSpawnRemove(int first)
{
	assert(first != S_NULL);

	for (;;)
	{
		if (state_dyn[first].group != 'S')
			break;

		if (states[first].tics < 0)  // hibernation
			break;

		int prev_idx = state_dyn[first].gr_idx;

		first = states[first].nextstate;

		if (first == S_NULL)
			return true;

		int next_idx = state_dyn[first].gr_idx;

		if (next_idx != (prev_idx + 1))
			break;
	}

	return false;
}

namespace Frames
{
	void UpdateAttackSlots(const char *act_name, const char *atk)
	{
		if (! atk)
			return;

		assert(strlen(atk) >= 3);
		assert(atk[1] == ':');

		if (atk[0] == 'S')
		{
			attack_slot[SPARE] = atk + 2;
			return;
		}

		// try to use spare attack if normal attack is already uses, in
		// case some mods give a monster two different attacks.

		assert(atk[0] == 'R' || atk[0] == 'C');

		int N = (atk[0] == 'R') ? RANGE : COMBAT;

		if (attack_slot[N])
		{
			if (strcmp(attack_slot[N], atk + 2) == 0)
				return;  // no problem, already contains this attack

			N = SPARE;

			if (attack_slot[N])
			{
				// XXX should show the problem thing
				// FIXME: maybe disable the action ??
				PrintWarn("No free attack slots for action %s.\n", act_name);
				return;
			}
		}

		attack_slot[N] = atk + 2;
	}
}

void Frames::OutputState(int cur)
{
	assert(cur > 0);

	state_t *st = states + cur;

	if (cur <= S_LAST_WEAPON_STATE)
		act_flags |= AF_WEAPON_ST;
	else
		act_flags |= AF_THING_ST;

	if (cur != S_LIGHTDONE)
	{
		if (cur < lowest_touched)  lowest_touched  = cur;
		if (cur > highest_touched) highest_touched = cur;
	}

	const char *act_name = action_info[st->action].ddf_name;

	WAD::Printf("    %s:%c:%d:%s:%s",
		sprnames[st->sprite], 65 + ((int) st->frame & 63), (int) st->tics,
		(st->frame >= 32768) ? "BRIGHT" : "NORMAL", act_name);

	act_flags |= action_info[st->action].act_flags;

	UpdateAttackSlots(act_name, action_info[st->action].atk_1);
	UpdateAttackSlots(act_name, action_info[st->action].atk_2);
}

const char *Frames::GroupToName(char group, bool use_spawn)
{
	switch (group)
	{
		case 'S': return use_spawn ? "SPAWN" : "IDLE";
		case 'E': return "CHASE";
		case 'L': return "MELEE";
		case 'M': return "MISSILE";
		case 'P': return "PAIN";
		case 'D': return "DEATH";
		case 'X': return "OVERKILL";
		case 'R': return "RESPAWN";
		case 'H': return "RESURRECT";

		// weapons
		case 'u': return "UP";
		case 'd': return "DOWN";
		case 'r': return "READY";
		case 'a': return "ATTACK";
		case 'f': return "FLASH";

		default:
			InternalError("GroupToName: BAD GROUP '%c'\n", group);
	}		

	return NULL;
}

void Frames::OutputGroup(int first, char group)
{
	if (first == S_NULL)
		return;

	assert(state_dyn[first].group != 0);

	bool use_spawn = (group == 'S') && CheckSpawnRemove(first);

	// this allows group sharing (especially MELEE and MISSILE)
	char first_group = state_dyn[first].group;

	WAD::Printf("\n");
	WAD::Printf("STATES(%s) =\n", GroupToName(group, use_spawn));

	int cur = first;

	for (;;)
	{
		OutputState(cur);

		if (states[cur].tics < 0)  // go into hibernation
		{
			WAD::Printf(";\n");
			return;
		}

		int prev_idx = state_dyn[cur].gr_idx;

		cur = states[cur].nextstate;

		if (cur == S_NULL)
		{
			WAD::Printf(",#REMOVE;\n");
			return;
		}

		char next_gr = state_dyn[cur].group;
		int next_idx = state_dyn[cur].gr_idx;

		assert(next_gr != 0);

		// check if finished and DON'T need a redirector
		if (cur == first && (group == 'S' || group == 'E' ||
			group == 'u' || group == 'd' || group == 'r'))
		{
			WAD::Printf(";\n");
			return;
		}

		if (next_gr == first_group && next_idx == (prev_idx + 1))
		{
			WAD::Printf(",\n");
			continue;
		}

		// we must be finished, and we need redirection

		if (next_idx == 1)
			WAD::Printf(",#%s;\n", GroupToName(next_gr, use_spawn));
		else
			WAD::Printf(",#%s:%d;\n", GroupToName(next_gr, use_spawn), next_idx);

		return;
	}
}


//------- DEBUGGING ------------------------------------------------------

#if (DEBUG_RANGES)
extern const char *state_names[NUMSTATES];
#endif

void Frames::DebugRange(const char *kind, const char *entry)
{
#if (DEBUG_RANGES)
	fprintf(stderr, "%s [%s] : ", kind, entry);
	
	if (highest_touched == S_NULL)
	{
		fprintf(stderr, "empty\n");
	}
	else
	{
		assert(0 < lowest_touched);
		assert(lowest_touched <= highest_touched);
		assert(highest_touched < NUMSTATES);

		fprintf(stderr, "%s -> %s\n", state_names[lowest_touched],
			state_names[highest_touched]);
	}
#endif
}
