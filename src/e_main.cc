//---------------------------------------------------------------------------
//  EDGE Main Init + Program Loop Code
//----------------------------------------------------------------------------
// 
//  Copyright (c) 1999-2005  The EDGE Team.
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
//----------------------------------------------------------------------------
//
//  Based on the DOOM source code, released by Id Software under the
//  following copyright:
//
//    Copyright (C) 1993-1996 by id Software, Inc.
//
//----------------------------------------------------------------------------
//
// DESCRIPTION:
//      EDGE main program (engine::Main),
//      game loop (engine::Loop) and startup functions.
//
// -MH- 1998/07/02 "shootupdown" --> "true3dgameplay"
// -MH- 1998/08/19 added up/down movement variables
//

#include "i_defs.h"
#include "e_main.h"

#include "am_map.h"
#include "con_defs.h"
#include "con_main.h"
#include "dm_defs.h"
#include "dm_state.h"
#include "dstrings.h"
#include "e_demo.h"
#include "e_input.h"
#include "f_finale.h"
#include "g_game.h"
#include "gui_main.h"
#include "gui_gui.h"
#include "hu_stuff.h"
#include "l_glbsp.h"
#include "m_argv.h"
#include "m_bbox.h"
#include "m_misc.h"
#include "m_menu.h"
#include "n_network.h"
#include "p_setup.h"
#include "p_spec.h"
#include "r_local.h"
#include "rad_trig.h"
#include "r_layers.h"
#include "r_vbinit.h"
#include "r2_defs.h"
#include "rgl_defs.h"
#include "rgl_wipe.h"
#include "s_sound.h"
#include "st_stuff.h"
#include "sv_chunk.h"
#include "sv_main.h"
#include "v_colour.h"
#include "v_ctx.h"
#include "v_res.h"
#include "w_image.h"
#include "w_textur.h"
#include "w_wad.h"
#include "wi_stuff.h"
#include "z_zone.h"

#include "errorcodes.h"
#include "version.h"

#include "epi/asserts.h"
#include "epi/errors.h"
#include "epi/files.h"
#include "epi/filesystem.h"
#include "epi/path.h"
#include "epi/strings.h"
#include "epi/utility.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define DEFAULT_LANGUAGE  "ENGLISH"

#define E_TITLE  "EDGE v" EDGEVERSTR

bool singletics = false;  // debug flag to cancel adaptiveness

int maketic;

// -ES- 2000/02/13 Takes screenshot every screenshot_rate tics.
// Must be used in conjunction with singletics.
static int screenshot_rate;

// For screenies...
bool m_screenshot_required = false;
bool need_save_screenshot  = false;

FILE *logfile = NULL;
FILE *debugfile = NULL;

gameflags_t default_gameflags =
{
	false,  // nomonsters
	false,  // fastparm

	false,  // res_respawn setting
	false,  // respawn
	false,  // item respawn

	false,  // true 3d gameplay
	MENU_GRAV_NORMAL, // gravity
	false,  // more blood

	true,   // jump
	true,   // crouch
	true,   // mlook
	AA_ON,  // autoaim
     
	true,   // cheats
     
	true,   // have_extra
	false,  // limit_zoom
	false,  // shadows
	false,  // halos

	false,    // sector_compat
	true,     // kicking
	true      // weapon_switch
};

// -KM- 1998/12/16 These flags hold everything needed about a level
// -KM- 1999/01/29 Added autoaim flag.
// -AJA- 2000/02/02: Removed initialisation (it *should* be setup at
//       level start).

gameflags_t level_flags;

// -KM- 1998/12/16 These flags are the users prefs and are copied to
//   gameflags when a new level is started.
// -AJA- 2000/02/02: Removed initialisation (done in code using
//       `default_gameflags').

gameflags_t global_flags;

skill_t startskill;
char *startmap;  // FIXME: make static

int startplayers = 1;
int startbots = 0;

bool autostart;
bool advance_title;

int newnmrespawn = 0;

bool rotatemap = false;
bool showstats = false;
bool swapstereo = false;
bool mus_pause_stop = false;
bool infight = false;
bool png_scrshots = false;

bool external_ddf = false;
bool strict_errors = false;
bool lax_errors = false;
bool hom_detect = false;
bool no_warnings = false;
bool no_obsoletes = false;
bool autoquickload = false;

// FIXME!! Strbox this lot...
epi::strent_c iwad_base;

epi::strent_c cache_dir;
epi::strent_c ddf_dir;
epi::strent_c game_dir;
epi::strent_c home_dir;
epi::strent_c save_dir;
epi::strent_c shot_dir;

int crosshair = 0;

layer_t *backbg_layer = NULL;
layer_t *conplayer_layer = NULL;
layer_t *pause_layer = NULL;

//
// EVENT HANDLING
//
// Events are asynchronous inputs generally generated by the game user.
// Events can be discarded if no responder claims them
//
event_t events[MAXEVENTS];
int eventhead;
int eventtail;

// ===================Internal====================

class startup_progress_c
{
private:
	int perc;
	int g_step, g_size, g_total;  // global progress
	int l_step, l_total;   // local progress

public:
	startup_progress_c() :
		perc(-1),  // force update on initial setting
		g_step(0), g_size(0), g_total(0),
		l_step(0), l_total(0) { }

	~startup_progress_c() { }

	void recomputePercent()
	{
		int pp = (100 * g_step + (100 * g_size * l_step / l_total)) / g_total;

		DEV_ASSERT2(0 <= pp && pp <= 100);

		if (pp != perc)
		{
			perc = pp; drawIt();
		}
	}

	void drawIt(int glbsp_perc = -1)  // a bit kludgy...
	{
		RGL_DrawProgress(perc, glbsp_perc);
	}

	void setGlobal(int step, int size, int total)
	{
		g_step  = step;
		g_size  = size;
		g_total = total;
	}

	void setLocal(int step, int total)
	{
		l_step  = step;
		l_total = total;
	}
};

static startup_progress_c s_progress;

void E_ProgressMessage(const char *message)
{
	// FIXME: show message near progress bar
	I_Printf("%s", message);
}

void E_LocalProgress(int step, int total)
{
	s_progress.setLocal(step, total);
	s_progress.recomputePercent();
}

void E_GlobalProgress(int step, int size, int total)
{
	s_progress.setGlobal(step, size, total);

	E_LocalProgress(0, 1);  // recomputes the percentage
}

void E_NodeMessage(const char *message)
{
	// FIXME: show message
}

void E_NodeProgress(int perc)
{
	s_progress.drawIt(perc);
}

//
// SetGlobalVars
//
// -ACB- 1999/09/20 Created. Sets Global Stuff.
//
static void SetGlobalVars(void)
{
	int p;
	const char *s;

	// Screen Resolution Check...
	s = M_GetParm("-width");
	if (s)
		SCREENWIDTH = atoi(s);

	s = M_GetParm("-height");
	if (s)
		SCREENHEIGHT = atoi(s);

	p = M_CheckParm("-res");
	if (p && p + 2 < M_GetArgCount())
	{
		SCREENWIDTH  = atoi(M_GetArgument(p + 1));
		SCREENHEIGHT = atoi(M_GetArgument(p + 2));
	}

	// Bits per pixel check....
	s = M_GetParm("-bpp");
	if (s)
		SCREENBITS = atoi(s) * 8;

	// restrict depth to allowable values
	if (SCREENBITS < 15)
		SCREENBITS = 15;
	else if (SCREENBITS > 16)
		SCREENBITS = 32;

	M_CheckBooleanParm("windowed",   &SCREENWINDOW, false);
	M_CheckBooleanParm("fullscreen", &SCREENWINDOW, true);

	// deathmatch check...
	if (M_CheckParm("-altdeath"))
	{
		deathmatch = 2;
	}
	else
	{
		p = M_CheckParm("-deathmatch");

		if (p)
		{
			deathmatch = 1;

			if (p + 1 < M_GetArgCount())
				deathmatch = atoi(M_GetArgument(p + 1));

			if (!deathmatch)
				deathmatch = 1;
		}
	}

	// sprite kludge (TrueBSP)
	p = M_CheckParm("-spritekludge");
	if (p)
	{
		if (p + 1 < M_GetArgCount())
			sprite_kludge = atoi(M_GetArgument(p + 1));

		if (!sprite_kludge)
			sprite_kludge = 1;
	}

	// speed for mouse look
	s = M_GetParm("-vspeed");
	if (s)
	{
		mlookspeed = atoi(s) / 64;
		if (mlookspeed >= 20)
			mlookspeed = 19;
	}

	// -AJA- 1999/10/18: Reworked these with M_CheckBooleanParm
	M_CheckBooleanParm("rotatemap", &rotatemap, false);
	M_CheckBooleanParm("invertmouse", &invertmouse, false);
	M_CheckBooleanParm("showstats", &showstats, false);
	M_CheckBooleanParm("hom", &hom_detect, false);
	M_CheckBooleanParm("sound", &nosound, true);
	M_CheckBooleanParm("music", &nomusic, true);
	M_CheckBooleanParm("cdmusic", &nocdmusic, true);
	M_CheckBooleanParm("itemrespawn", &global_flags.itemrespawn, false);
	M_CheckBooleanParm("mlook", &global_flags.mlook, false);
	M_CheckBooleanParm("monsters", &global_flags.nomonsters, true);
	M_CheckBooleanParm("fast", &global_flags.fastparm, false);
	M_CheckBooleanParm("extras", &global_flags.have_extra, false);
	M_CheckBooleanParm("shadows", &global_flags.shadows, false);
	M_CheckBooleanParm("halos", &global_flags.halos, false);
	M_CheckBooleanParm("kick", &global_flags.kicking, false);
	M_CheckBooleanParm("singletics", &singletics, false);
	M_CheckBooleanParm("infight", &infight, false);
	M_CheckBooleanParm("true3d", &global_flags.true3dgameplay, false);
	M_CheckBooleanParm("blood", &global_flags.more_blood, false);
	M_CheckBooleanParm("cheats", &global_flags.cheats, false);
	M_CheckBooleanParm("jumping", &global_flags.jump, false);
	M_CheckBooleanParm("crouching", &global_flags.crouch, false);
	M_CheckBooleanParm("weaponswitch", &global_flags.weapon_switch, false);
	M_CheckBooleanParm("autoload", &autoquickload, false);

	if (M_CheckParm("dlights"))
		use_dlights = 1;
	else if (M_CheckParm("nodlights"))
		use_dlights = 0;

	if (M_CheckParm("-ecompat"))
		global_flags.sector_compat = true;

	if (!global_flags.respawn)
	{
		if (M_CheckParm("-newnmrespawn"))
		{
			global_flags.res_respawn = true;
			global_flags.respawn = true;
		}
		else if (M_CheckParm("-respawn"))
		{
			global_flags.respawn = true;
		}
	}

	// check for strict and no-warning options
	M_CheckBooleanParm("strict", &strict_errors, false);
	M_CheckBooleanParm("warn", &no_warnings, true);
	M_CheckBooleanParm("obsolete", &no_obsoletes, true);
	M_CheckBooleanParm("lax", &lax_errors, false);
}

//
// SetLanguage
//
void SetLanguage(void)
{
	const char *want_lang = M_GetParm("-lang");

	if (! want_lang)
		want_lang = config_language.GetString();  // m_misc

	if (want_lang)
	{
		if (language.Select(want_lang))
			return;

		I_Error("Invalid language: '%s'\n", want_lang);
	}

	if (language.IsValid())
		return;

	if (language.Select(DEFAULT_LANGUAGE))
		return;

	if (language.Select(0))
		return;

	I_Error("Unable to select any language!");
}

//
// SpecialWadVerify
//
static void SpecialWadVerify(void)
{
	int lump;

	// the WAD version
	int wad_ver;
	int wad_ver_frac;

	// the backward compatibility of the WAD
	int wad_bc;
	int wad_bc_frac;

	// The sub-version of the WAD (100% compatible with other WADs within the same wad_version)
	int wad_sub_ver;
	const void *data;
	const char *s;

	lump = W_CheckNumForName("EDGEVER");
	if (lump < 0)
		I_Error("EDGEVER lump not found. Get EDGE.WAD at http://edge.sourceforge.net/");

	data = W_CacheLumpNum(lump);
	s = (const char*)data;

	wad_ver = atoi(s);
	while (isdigit(*s))
		s++;

	s++;
	wad_ver_frac = atoi(s);
	while (isdigit(*s))
		s++;

	s++;
	wad_sub_ver = atoi(s);
	while (*s != '\n')
		s++;

	while (!isdigit(*s))
		s++;

	wad_bc = atoi(s);
	while (isdigit(*s))
		s++;

	s++;
	wad_bc_frac = atoi(s);

	W_DoneWithLump(data);

	if (wad_ver * 1024 + wad_ver_frac < EDGE_WAD_VERSION * 1024 + EDGE_WAD_VERSION_FRAC)
	{
		I_Error("EDGE.WAD version %d.%d found, version %d.%d is required.\n"
				"Get it at http://edge.sourceforge.net/", wad_ver, wad_ver_frac,
				EDGE_WAD_VERSION, EDGE_WAD_VERSION_FRAC);
	}

	if (wad_bc * 1024 + wad_bc_frac > EDGE_WAD_VERSION * 1024 + EDGE_WAD_VERSION_FRAC)
	{
		I_Error("EDGE.WAD version %d.%d required, found too new version %d.%d\n"
				"which is not backward compatible enough. Get an older EDGE.WAD or\n"
				"a newer EDGE version at http://edge.sourceforge.net/",
				EDGE_WAD_VERSION, EDGE_WAD_VERSION_FRAC, wad_ver, wad_ver_frac);
	}

	if (wad_ver > EDGE_WAD_VERSION)
	{
		I_Warning("EDGE.WAD version %d.%d required, found newer version %d.%d.\n"
				  "This version of EDGE is probably out-of-date, newer versions are\n"
				  "available at http://edge.sourceforge.net/",
				  EDGE_WAD_VERSION, EDGE_WAD_VERSION_FRAC, wad_ver, wad_ver_frac);
	}
	else if (EDGE_WAD_SUB_VERSION > wad_sub_ver)
	{
		I_Warning("Slightly out-of-date EDGE.WAD (v%d.%d.%d) found,\n"
				  "%d.%d.%d is recommended. Get it at http://edge.sourceforge.net/",
				  wad_ver, wad_ver_frac, wad_sub_ver,
				  EDGE_WAD_VERSION, EDGE_WAD_VERSION_FRAC, EDGE_WAD_SUB_VERSION);
	}

	I_Printf("EDGE.WAD version %d.%d.%d found.\n", wad_ver, wad_ver_frac, wad_sub_ver);
}

//
// ShowNotice
//
static void ShowNotice(void)
{
	I_Printf("%s", language["Notice"]);

}

static void DoSystemStartup(void)
{
	// startup the system now
	W_InitImages();
	GUI_MainInit();

	L_WriteDebug("- System startup begun.\n");

	I_SystemStartup();

	// -ES- 1998/09/11 Use R_ChangeResolution to enter gfx mode
	int idx = scrmodelist.FindNearest(SCREENWIDTH, 
                                      SCREENHEIGHT, 
                                      SCREENBITS, 
                                      SCREENWINDOW);

	if (idx < 0)
        I_Error("DoSystemStartup: No available resolutions"); // Must be valid

	R_ChangeResolution(idx);

	// -KM- 1998/09/27 Change res now, so music doesn't start before
	// screen.  Reset clock too.
	L_WriteDebug("- Changing Resolution...\n");

	R_ExecuteChangeResolution();

	RGL_Init();

	L_WriteDebug("- System startup done.\n");
}

// ===============End of Internals================


//
// E_PostEvent
//
// Called by the I/O functions when input is detected
//
void E_PostEvent(event_t * ev)
{
	events[eventhead] = *ev;
	eventhead = (eventhead + 1) % MAXEVENTS;

#ifdef DEBUG_KEY_EV  //!!!!
if (ev->type == ev_keydown || ev->type == ev_keyup)
{
	L_WriteDebug("EVENT @ %08x %d %s\n",
		I_ReadMicroSeconds()/1000,
		ev->value.key,
		(ev->type == ev_keyup) ? "DOWN" : "up");
}
#endif
}

//
// E_ProcessEvents
//
// Send all the events of the given timestamp down the responder chain
//
void E_ProcessEvents(void)
{
	event_t *ev;

	for (; eventtail != eventhead; eventtail = (eventtail + 1) % MAXEVENTS)
	{
		ev = &events[eventtail];
		if (chat_on && HU_Responder(ev))
			continue;  // let chat eat the event first of all

		if (GUI_MainResponder(ev))
			continue;  // GUI ate the event

		if (M_Responder(ev))
			continue;  // menu ate the event

		G_Responder(ev);  // let game eat it, nobody else wanted it
	}
}

static void M_DisplayPause(void)
{
	static const image_t *pause_image = NULL;

	if (! pause_image)
		pause_image = W_ImageLookup("M_PAUSE");

	// make sure image is centered horizontally

	float w = FROM_320(IM_WIDTH(pause_image));
	float h = FROM_200(IM_HEIGHT(pause_image));

	float x = FROM_320(160) - w / 2;
	float y = FROM_200(10);

    RGL_DrawImage(x, y, w, h, pause_image, 0, 0,
                   IM_RIGHT(pause_image), IM_BOTTOM(pause_image), NULL, 1.0f);
}

//
// E_Display
//
// Draw current display, possibly wiping it from the previous
//
// -ACB- 1998/07/27 Removed doublebufferflag check (unneeded).  

// wipegamestate can be set to GS_NOTHING to force a wipe on the next draw

gamestate_e wipegamestate = GS_DEMOSCREEN;
wipetype_e wipe_method = WIPE_Melt;
int wipe_reverse = 0;

static bool wipe_gl_active = false;

void E_Display(void)
{
	static bool fullscreen = false;

	// for wiping
	bool wipe;

	if (nodrawers)
		return;  // for comparative timing / profiling

	// -ES-  1998/08/20: Resolution Change Check
	// -ACB- 2005/03/06: Now using the new res index
	if (newres_idx >= 0)
		R_ExecuteChangeResolution();

	// Start the frame - should we need to.
	I_StartFrame();

	// change the view size if needed
	if (setsizeneeded)
		R_ExecuteSetViewSize();

	// -AJA- 1999/07/03: removed PLAYPAL reference.
	if (gamestate != GS_LEVEL)
	{
		V_SetPalette(PALETTE_NORMAL, 0);
	}

	// -AJA- 1999/08/02: Make sure palette/gamma is OK. This also should
	//       fix (finally !) the "gamma too late on walls" bug.
	V_ColourNewFrame();

	// save the current screen if about to wipe
	if (gamestate != wipegamestate)
	{
		wipe = true;
		wipe_gl_active = true;
		RGL_InitWipe(wipe_reverse, wipe_method);
	}
	else
		wipe = false;

	if (gamestate == GS_LEVEL)
		HU_Erase();

	// do buffered drawing
	switch (gamestate)
	{
		case GS_LEVEL:
			if (automapactive == 2)
				AM_Drawer();

			ST_Drawer();

			fullscreen = viewwindowheight == SCREENHEIGHT;
			break;

		case GS_INTERMISSION:
			WI_Drawer();
			break;

		case GS_FINALE:
			F_Drawer();
			break;

		case GS_DEMOSCREEN:
			E_PageDrawer();
			break;

		case GS_NOTHING:
			break;
	}

	if (need_save_screenshot)
	{
		R_Render();
		M_MakeSaveScreenShot();

		need_save_screenshot = false;
	}

	// draw the view directly
	if (gamestate == GS_LEVEL && automapactive != 2)
	{
		R_Render();

		if (automapactive != 2)
			AM_Drawer();
	}

	if (gamestate == GS_LEVEL)
	{
		ST_Drawer();
		HU_Drawer();
		RAD_Drawer();
		M_DisplayAir();
	}

	wipegamestate = gamestate;

	if (paused)
		M_DisplayPause();

	// menus go directly to the screen
	M_Drawer();  // menu is drawn even on top of everything (except console)
	M_DisplayDisk();

	N_NetUpdate();  // send out any new accumulation

	if (m_screenshot_required)
	{
		m_screenshot_required = false;
		M_ScreenShot(true);
	}
	else if (screenshot_rate && gamestate == GS_LEVEL)
	{
		DEV_ASSERT2(singletics);

		if (leveltime % screenshot_rate == 0)
			M_ScreenShot(false);
	}

	// draw console _after_ doing screenshots
	GUI_MainDrawer();

	// normal update
	if (!wipe && !wipe_gl_active)
	{
		RGL_DrawBeta();
		I_FinishFrame();  // page flip or blit buffer
		return;
	}

	// -AJA- Wipe code for GL.  Sorry for all this ugliness, but it just
	//       didn't fit into the existing wipe framework.
	//
	if (RGL_DoWipe())
	{
		RGL_StopWipe();
		wipe_gl_active = false;
	}

	M_Drawer();  // menu is drawn even on top of wipes

	RGL_DrawBeta();
	I_FinishFrame();  // page flip or blit buffer
}

//
//  DEMO LOOP
//
int demosequence;
static int demo_num;
static int page_map;
static int page_pic;
static int pagetic;
static const image_t *page_image = NULL;

//
// E_PageTicker
//
// Handles timing for warped projection
//
void E_PageTicker(void)
{
	if (--pagetic < 0)
		E_AdvanceDemo();
}

#define NOPAGE_COLOUR  0x303030

//
// E_PageDrawer
//
void E_PageDrawer(void)
{
	if (page_image)
		RGL_Image(0, 0, SCREENWIDTH, SCREENHEIGHT, page_image);
	else
		RGL_SolidBox(0, 0, SCREENWIDTH, SCREENHEIGHT, NOPAGE_COLOUR, 1);
}

//
// E_AdvanceDemo
//
// Called after each demo or intro demosequence finishes
//
void E_AdvanceDemo(void)
{
	advance_title = true;
}

static void TitleNextPicture(void)
{
	int count;
	gamedef_c *g;
  
	// prevent an infinite loop
	for (count=0; count < 200; count++)
	{
		g = gamedefs[page_map];

		if (page_pic >= g->titlepics.GetSize())
		{
			page_map = (page_map + 1) % gamedefs.GetSize();
			page_pic = 0;
			continue;
		}

		// ignore non-existing episodes.  Doesn't include title-only ones
		// like [EDGE].
		if (page_pic == 0 && g->firstmap && g->firstmap[0] &&
			W_CheckNumForName(g->firstmap) == -1)
		{
			page_map = (page_map + 1) % gamedefs.GetSize();
			continue;
		}

		// ignore non-existing images
		if (W_CheckNumForName(g->titlepics[page_pic]) < 0)
		{
			page_pic += 1;
			continue;
		}

		// found one !!

		if (page_pic == 0 && g->titlemusic > 0)
			S_ChangeMusic(g->titlemusic, false);

		page_image = W_ImageLookup(g->titlepics[page_pic]);
		page_pic += 1;
    
		return;
	}
}

//
// This cycles through the demo sequences.
// -KM- 1998/12/16 Fixed for DDF.
//
static void E_DoAdvanceTitle(void)
{
	advance_title = false;
	usergame = false;     // no save or end game here
	paused = false;
	gameaction = ga_nothing;

	demosequence = (demosequence + 1) % 2;  // - Kester

	switch (demosequence)  // - Kester
	{
		case 0:  // Title Picture
		{
			gamestate = GS_DEMOSCREEN;

			TitleNextPicture();

			pagetic = gamedefs[page_map]->titletics;
			break;
		}

		default:  // Play Demo
		{
			char buffer[9];

			sprintf(buffer, "DEMO%x", demo_num++);

			if (W_CheckNumForName(buffer) < 0)
			{
				demo_num = 1;
				sprintf(buffer, "DEMO1");
			}

			// -AJA- FIXME: demos in lumps not yet supported
			// G_DeferredPlayDemo(buffer);

			break;
		}
	}
}

//
// E_StartTitle
//
void E_StartTitle(void)
{
	gameaction = ga_nothing;
	demosequence = 1;
	demo_num = 1;

	// force pic overflow -> first available titlepic
	page_map = gamedefs.GetSize() - 1;
	page_pic = 999;
 
	E_AdvanceDemo();
}

//
// InitDirectories
//
// Detects which directories to search for DDFs, WADs and other files in.
//
// -ES- 2000/01/01 Written.
//
void InitDirectories(void)
{
    epi::string_c path;

	const char *s = M_GetParm("-home");
    if (s)
        home_dir.Set(s);

	// Get the Home Directory from environment if set
    if (home_dir.IsEmpty())
    {
        s = getenv("HOME");
        if (s)
        {
            path = epi::path::Join(s, EDGEHOMESUBDIR); 

            const char *test_dir = path.GetString();
			if (!epi::the_filesystem->IsDir(test_dir))
			{
                epi::the_filesystem->MakeDir(test_dir);

                // Check whether the directory was created
                if (!epi::the_filesystem->IsDir(test_dir))
                    test_dir = NULL; // Make invalid since it clearly isn't
			}

            if (test_dir)
                home_dir.Set(test_dir);
        }
    }

    if (home_dir.IsEmpty())
        home_dir.Set("."); // Default to current directory

	// Get the Game Directory from parameter.
	s = M_GetParm("-game");
	if (s)
    {
        game_dir.Set(s);
	}
	else
	{
        game_dir.Set(".");
	}

	// add parameter file "gamedir/parms" if it exists.
    path = epi::path::Join(game_dir.GetString(), "parms");

	if (epi::the_filesystem->Access(path.GetString(), epi::file_c::ACCESS_READ))
	{
		// Insert it right after the game parameter
		M_ApplyResponseFile(path.GetString(), M_CheckParm("-game") + 2);
	}

	s = M_GetParm("-ddf");
	if (s)
	{
		external_ddf = true;

		ddf_dir.Set(s);
	} 
	else
	{
		ddf_dir.Set(game_dir.GetString());
	}

	// config file
	s = M_GetParm("-config");
	if (s)
	{
	    path.Empty();
		M_ComposeFileName(path, home_dir.GetString(), s);
		cfgfile.Set(path.GetString());
	}
	else
    {
        path = epi::path::Join(home_dir.GetString(), EDGECONFIGFILE);
		cfgfile.Set(path.GetString());
	}
	
	// cache directory
    path = epi::path::Join(home_dir.GetString(), CACHEDIR);

    if (!epi::the_filesystem->IsDir(path.GetString()))
        epi::the_filesystem->MakeDir(path.GetString());

    cache_dir.Set(path.GetString());

	// savegame directory
    path = epi::path::Join(home_dir.GetString(), SAVEGAMEDIR);
	
    if (!epi::the_filesystem->IsDir(path.GetString()))
        epi::the_filesystem->MakeDir(path.GetString());

    save_dir.Set(path.GetString());

	// screenshot directory
    path = epi::path::Join(home_dir.GetString(), SCRNSHOTDIR);

    if (!epi::the_filesystem->IsDir(path.GetString()))
        epi::the_filesystem->MakeDir(path.GetString());

    shot_dir.Set(path.GetString());
}

//
// CheckExternal
//
// Checks if DDF files exist in the DDF directory, and if so then
// enables "external-ddf mode", which prevents the DDF lumps within
// EDGE.WAD from being parsed.
//
// -AJA- 2000/05/23: written.
//
#define EXTERN_FILE  "things.ddf"

void CheckExternal(void)
{
    epi::string_c test_filename;
  
	// too simplistic ?

	test_filename = epi::path::Join(game_dir.GetString(), EXTERN_FILE);

	if (epi::the_filesystem->Access(test_filename.GetString(), epi::file_c::ACCESS_READ))
		external_ddf = true;
}

//
// E_IdentifyVersion
//
// Adds an IWAD and EDGE.WAD. -ES-  2000/01/01 Rewritten.
//
const char *wadname[] = { "doom2", "doom", "plutonia", "tnt", "freedoom", NULL };

static void IdentifyVersion(void)
{
	// Check -iwad parameter, find out if it is the IWADs directory
    epi::string_c iwad_par;
    epi::string_c iwad_file;
    epi::string_c iwad_dir;

    iwad_par.Set(M_GetParm("-iwad"));

    if (!iwad_par.IsEmpty())
    {
        if (epi::the_filesystem->IsDir(iwad_par.GetString()))
        {
            iwad_dir.Set(iwad_par.GetString());
            iwad_par.Empty(); // Discard 
        }
    }   

    // If we haven't yet set the IWAD directory, then we check
    // the DOOMWADDIR environment variable
    if (iwad_dir.IsEmpty())
    {
        const char *s = getenv("DOOMWADDIR");

        if (s && epi::the_filesystem->IsDir(s))
            iwad_dir.Set(s);
    }

    // Should the IWAD directory not be set by now, then we
    // use our standby option of the current directory.
    if (iwad_dir.IsEmpty())
        iwad_dir.Set(".");

    // Should the IWAD Parameter not be empty then it means
    // that one was given which is not a directory. Therefore
    // we assume it to be a name
    if (!iwad_par.IsEmpty())
    {
        epi::string_c fn;

        fn = iwad_par;
        
        // Is it missing the extension?
        epi::string_c ext = epi::path::GetExtension(iwad_par.GetString());
        if (ext.CompareNoCase(EDGEWADEXT))
        {
            // Add one
            fn.AddString("." EDGEWADEXT);
        }

        // If no directory given use the IWAD directory
        epi::string_c dir = epi::path::GetDir(fn.GetString());
        if (!dir.GetLength())
            iwad_file = epi::path::Join(iwad_dir.GetString(), fn.GetString()); 
        else
            iwad_file = fn;

        if (!epi::the_filesystem->Access(iwad_file.GetString(), epi::file_c::ACCESS_READ))
        {
			I_Error("IdentifyVersion: Unable to add specified '%s'", fn.GetString());
        }
    }
    else
    {
        const char *location;
        epi::string_c fn;
        int max = 1;

        if (iwad_dir.Compare(game_dir.GetString())) 
        {
            // IWAD directory & game directory differ 
            // therefore do a second loop which will
            // mean we check both.
            max++;
        } 

		bool done = false;
		for (int i = 0; i < max && !done; i++)
		{
			location = (i == 0 ? iwad_dir.GetString() : game_dir.GetString());

			//
			// go through the available wad names constructing an access
			// name for each, adding the file if they exist.
			//
			// -ACB- 2000/06/08 Quit after we found a file - don't load
			//                  more than one IWAD
			//
			for (int w_idx=0; wadname[w_idx]; w_idx++)
			{
				fn = epi::path::Join(location, wadname[w_idx]);
                fn.AddString("." EDGEWADEXT);

				if (epi::the_filesystem->Access(fn.GetString(), epi::file_c::ACCESS_READ))
				{
                    iwad_file = fn;
					done = true;
					break;
				}
			}
		}
    }

	if (iwad_file.IsEmpty())
		I_Error("IdentifyVersion: No IWADS found!\n");

    W_AddRawFilename(iwad_file.GetString(), FLKIND_IWad);

    iwad_file.ToUpper(); // Make uppercase
    
    iwad_par = epi::path::GetBasename(iwad_file.GetString());
    iwad_base.Set(iwad_par.GetString());

	L_WriteDebug("IWAD BASE = [%s]\n", iwad_base.GetString());

    // Emulate this behaviour?

    // Look for the required wad in the IWADs dir and then the gamedir
    epi::string_c reqwad_filename;

    reqwad_filename = epi::path::Join(iwad_dir.GetString(), REQUIREDWAD "." EDGEWADEXT);
    if (!epi::the_filesystem->Access(reqwad_filename.GetString(), epi::file_c::ACCESS_READ))
    {
        reqwad_filename = epi::path::Join(game_dir.GetString(), REQUIREDWAD "." EDGEWADEXT);
        if (!epi::the_filesystem->Access(reqwad_filename.GetString(), epi::file_c::ACCESS_READ))
        {
            I_Error("IdentifyVersion: Could not find required %s.%s!\n", 
                    REQUIREDWAD, EDGEWADEXT);
        }
    }

    W_AddRawFilename(reqwad_filename.GetString(), FLKIND_EWad);
}

static void CheckTurbo(void)
{
	int turbo_scale = 100;

	int p = M_CheckParm("-turbo");

	if (p)
	{
		if (p + 1 < M_GetArgCount())
			turbo_scale = atoi(M_GetArgument(p + 1));
		else
			turbo_scale = 200;

		if (turbo_scale < 10)  turbo_scale = 10;
		if (turbo_scale > 400) turbo_scale = 400;

		CON_MessageLDF("TurboScale", turbo_scale);
	}

	E_SetTurboScale(turbo_scale);
}

static void CheckSkillEtc(void)
{
	// get skill / episode / map from parms
	startskill = sk_medium;
	autostart = false;

	// -KM- 1999/01/29 Use correct skill: 1 is easiest, not 0
	const char *ps = M_GetParm("-skill");
	if (ps)
	{
		startskill = (skill_t)(atoi(ps) - 1);
		autostart = true;
	}

	ps = M_GetParm("-warp");
	if (ps)
	{
		startmap = Z_StrDup(ps);
		autostart = true;
	}
	else
	{
		startmap = Z_StrDup("MAP01"); // MUNDO HACK!!!!
	}

	ps = M_GetParm("-screenshot");
	if (ps)
	{
		screenshot_rate = atoi(ps);
		singletics = true;
	}

	ps = M_GetParm("-players");
	if (ps)
	{
		startplayers = atoi(ps);
		if (startplayers > 1)
			netgame = true;
	}

	ps = M_GetParm("-bots");
	if (ps)
	{
		startbots = atoi(ps);
	}

	// force a net game
	if (M_CheckParm("-netgame") > 0)
	{
		netgame = true;
	}
}

static void ShowDateAndVersion(void)
{
	time_t cur_time;
	char timebuf[100];

	time(&cur_time);
	strftime(timebuf, 99, "%I:%M %p on %d/%b/%Y", localtime(&cur_time));

	L_WriteLog("[Log file created at %s]\n\n", timebuf);
	L_WriteDebug("[Debug file created at %s]\n\n", timebuf);

	// 23-6-98 KM Changed to hex to allow versions such as 0.65a etc
	I_Printf("EDGE v" EDGEVERSTR " compiled on " __DATE__ " at " __TIME__ "\n");
	I_Printf("EDGE homepage is at http://edge.sourceforge.net/\n");
	I_Printf("EDGE is based on DOOM by id Software http://www.idsoftware.com/\n");
}

static void SetupLogAndDebugFiles(void)
{
	// -AJA- 2003/11/08 The log file gets all CON_Printfs, I_Printfs,
	//                  I_Warnings and I_Errors.
	if (! M_CheckParm("-nolog"))
	{
        epi::string_c logfn = epi::path::Join(home_dir.GetString(), EDGELOGFILE);

		logfile = fopen(logfn.GetString(), "w");

		if (!logfile)
			I_Error("[engine::Startup] Unable to create log file");
	}
	else
	{
		logfile = NULL;
	}
	
	//
	// -ACB- 1998/09/06 Only used for debugging.
	//                  Moved here to setup debug file for DDF Parsing...
	//
	// -ES- 1999/08/01 Debugfiles can now be used without -DDEVELOPERS, and
	//                 then logs all the CON_Printfs, I_Printfs and I_Errors.
	//
	// -ACB- 1999/10/02 Don't print to console, since we don't have a console yet.
	//
	int p = M_CheckParm("-debug");
	if (p)
	{
        epi::string_c debugfn = epi::path::Join(home_dir.GetString(), "debug.txt");

		debugfile = fopen(debugfn.GetString(), "w");

		if (!debugfile)
			I_Error("[engine::Startup] Unable to create debugfile");
	}
	else
	{
		debugfile = NULL;
	}
}

static void AddSingleCmdLineFile(const char *name)
{
    epi::string_c ext = epi::path::GetExtension(name);
	int kind = FLKIND_Lump;

    ext.ToLower(); // Presume it is in lowercase

	if (!ext.Compare("edm"))
	{
		singledemo = true;
		G_DeferredPlayDemo(name);
		return;
	}

	// no need to check for GWA (shouldn't be added manually)

	if (!ext.Compare("wad"))
		kind = FLKIND_PWad;
	else if (!ext.Compare("hwa"))
		kind = FLKIND_HWad;
	else if (!ext.Compare("scr"))
		kind = FLKIND_Script;
	else if (!ext.Compare("ddf") || !ext.Compare("ldf"))
		kind = FLKIND_DDF;
	else if (!ext.Compare("deh") || !ext.Compare("bex"))
		kind = FLKIND_Deh;

	epi::string_c fn;

	M_ComposeFileName(fn, game_dir.GetString(), name);
	W_AddRawFilename(fn.GetString(), kind);
}

static void AddCommandLineFiles(void)
{
    epi::string_c ext;
	epi::string_c fn;

	// first handle "loose" files (arguments before the first option)

	int p;
	const char *ps;

	for (p = 1; p < M_GetArgCount() && '-' != (ps = M_GetArgument(p))[0]; p++)
	{
		AddSingleCmdLineFile(ps);
	}

	// next handle the -file option (we allow multiple uses)

	p = M_CheckNextParm("-file", 0);
	
	while (p)
	{
		// the parms after p are wadfile/lump names,
		// go until end of parms or another '-' preceded parm

		for (p++; p < M_GetArgCount() && '-' != (ps = M_GetArgument(p))[0]; p++)
		{
			AddSingleCmdLineFile(ps);
		}

		p = M_CheckNextParm("-file", p-1);
	}

	// scripts....

	p = M_CheckNextParm("-script", 0);
	
	while (p)
	{
		// the parms after p are script filenames,
		// go until end of parms or another '-' preceded parm

		for (p++; p < M_GetArgCount() && '-' != (ps = M_GetArgument(p))[0]; p++)
		{
            ext = epi::path::GetExtension(ps);
            ext.ToLower();

			// sanity check...
			if (!ext.Compare("wad") || 
                !ext.Compare("gwa") ||
			    !ext.Compare("hwa") ||
                !ext.Compare("ddf") ||
			    !ext.Compare("deh") ||
			    !ext.Compare("bex"))
			{
				I_Error("Illegal filename for -script: %s\n", ps);
			}

			M_ComposeFileName(fn, game_dir.GetString(), ps);
			W_AddRawFilename(fn.GetString(), FLKIND_Script);
		}

		p = M_CheckNextParm("-script", p-1);
	}


	// finally handle the -deh option(s)

	p = M_CheckNextParm("-deh", 0);
	
	while (p)
	{
		// the parms after p are Dehacked/BEX filenames,
		// go until end of parms or another '-' preceded parm

		for (p++; p < M_GetArgCount() && '-' != (ps = M_GetArgument(p))[0]; p++)
		{
            ext = epi::path::GetExtension(ps);
            ext.ToLower();

			// sanity check...
			if (!ext.Compare("wad") || 
                !ext.Compare("gwa") ||
			    !ext.Compare("hwa") ||
                !ext.Compare("ddf") ||
			    !ext.Compare("scr"))
			{
				I_Error("Illegal filename for -deh: %s\n", ps);
			}

			M_ComposeFileName(fn, game_dir.GetString(), ps);
			W_AddRawFilename(fn.GetString(), FLKIND_Deh);
		}

		p = M_CheckNextParm("-deh", p-1);
	}
}


//
// E_EngineShutdown
//
void E_EngineShutdown(void)
{
	if (demorecording)
		G_FinishDemo();

	S_StopMusic();  // <--- FIXME: sound::StopMusic()?

    sound::Shutdown();

	N_QuitNetGame();
}

typedef struct
{
	int prog_time;  // rough indication of progress time
	void (*function)(void);
}
startuporder_t;

startuporder_t startcode[] =
{
	{  1, CheckExternal,       },
	{  1, DDF_Init,            },
	{  1, IdentifyVersion,     },
	{  1, AddCommandLineFiles, },
	{  1, CheckTurbo,          },
	{  1, CheckSkillEtc,       },
	{  1, RAD_Init,            },
	{  4, W_InitMultipleFiles, },
	{  1, V_InitPalette,       },
	{  2, HU_Init,             },
	{  3, R_InitFlats,         },
	{ 10, W_InitTextures,      },
	{  1, GUI_ConInit,         },
	{  1, SpecialWadVerify,    },
	{  1, GUI_MouseInit,       },
	{  1, M_InitMiscConVars,   },
	{ 20, W_ReadDDF,           },
	{  1, DDF_CleanUp,         },
	{  1, SetLanguage,         },
	{  1, ShowNotice,          },
	{  1, SV_MainInit,         },
	{ 15, W_ImageCreateUser,   },
	{ 20, R_InitSprites,       },
	{  1, M_Init,              },
	{  3, R_Init,              },
	{  1, P_Init,              },
	{  1, P_MapInit,           },
	{  1, P_InitSwitchList,    },
	{  1, R_InitPicAnims,      },
	{  1, sound::Init,         },
	{  1, N_InitNetwork,       },
	{  2, ST_Init,             },
	{  0, NULL,                }
};

// The engine namespace
namespace engine
{
	// Local Prototypes
	void Startup();
	void Shutdown(void);

    //
    // AutoStart
    //
	void AutoStart()
	{
		newgame_params_c params;

		params.skill = startskill;	
		params.deathmatch = deathmatch;	

		params.map = G_LookupMap(startmap);

		if (! params.map)
			I_Error("-warp: no such level '%s'\n", startmap);

		params.game = gamedefs.Lookup(params.map->episode_name);
		if (! params.game)
			I_Error("-warp: no gamedef for level '%s'\n", startmap);

		params.random_seed = I_PureRandom();

		params.SinglePlayer(startbots);

		if (! G_DeferredInitNew(params, true /* compat_check */))
			I_Error("-warp: cannot init level '%s'\n", startmap);
	}

	//
	// Startup
	//
	void Startup()
	{
		int p;
		const char *ps;

		// Version check ?
		if (M_CheckParm("-version"))
		{
			// -AJA- using I_Error here, since I_Printf crashes this early on
			I_Error("\nEDGE version is " EDGEVERSTR "\n");
		}

		// -AJA- 2000/02/02: initialise global gameflags to defaults
		global_flags = default_gameflags;

		InitDirectories();

		SetupLogAndDebugFiles();

		// -ACB- 1999/09/20 defines to be used?
		CON_InitConsole(79, 25, false);  // AJA: FIXME: init later (in startcode[])

		ShowDateAndVersion();

		M_LoadDefaults();
		SetGlobalVars();

		DoSystemStartup();

		I_PutTitle(E_TITLE); // Needs to be done once the system is up and running

		// RGL_FontStartup();

		E_GlobalProgress(0, 0, 1);

		int total=0;
		int cur=0;

		for (p=0; startcode[p].function != NULL; p++)
			total += startcode[p].prog_time;

		// Cycle through all the startup functions
		for (p=0; startcode[p].function != NULL; p++)
		{
			E_GlobalProgress(cur, startcode[p].prog_time, total);

			startcode[p].function();

			cur += startcode[p].prog_time;
		}

		E_GlobalProgress(100, 0, 100);

		CON_SetVisible(vs_notvisible);

		// start the appropriate game based on parms
		ps = M_GetParm("-record");
		if (ps)
		{
			G_RecordDemo(ps);
			autostart = true;
		}

		ps = M_GetParm("-playdemo");
		if (ps)
		{
			// quit after one demo
			singledemo = true;
			G_DeferredPlayDemo(ps);
		}

		ps = M_GetParm("-timedemo");
		if (ps)
		{
			G_DeferredTimeDemo(ps);
		}

		ps = M_GetParm("-loadgame");
		if (ps)
		{
			G_DeferredLoadGame(atoi(ps));
		}

		L_WriteDebug("- Startup: ready to play.\n");

		if (gameaction != ga_loadgame && gameaction != ga_playdemo)
		{
			if (netgame)
				N_InitiateNetGame();
			else if (autostart)
				AutoStart();
			else
				E_StartTitle();  // start up intro loop
		}

		Z_Free(startmap);
	}

	//
	// Main
	//
	// -ACB- 1998/08/10 Removed all reference to a gamemap, episode and mission
	//                  Used LanguageLookup() for lang specifics.
	//
	// -ACB- 1998/09/06 Removed all the unused code that no longer has
	//                  relevance.    
	//
	// -ACB- 1999/09/04 Removed statcopy parm check - UNUSED
	//
	// -ACB- 2004/05/31 Moved into a namespace, the c++ revolution begins....
	//
	void Main(int argc, const char **argv)
	{
		// Start the EPI Interface 
		epi::Init();

		// Start memory allocation system at the very start (SCHEDULED FOR REMOVAL)
		Z_Init();

		// Implemented here - since we need to bring the memory manager up first
		// -ACB- 2004/05/31
		M_InitArguments(argc, argv);

		try
		{
			// Startup function will throw an error if something goes wrong
			Startup();

			L_WriteDebug("- Entering game loop...\n");

			// -ACB- 1999/09/24 Call System Specific Looping function. Some
			//                  systems don't loop forever.
			I_Loop();
		}
		catch(epi::error_c err)
		{
			I_Error("%s\n", err.GetInfo());
		}
		catch(...)
		{
			I_Error("Unexpected internal failure occurred !\n");
		}

		Shutdown();    // Shutdown whatever at this point

		// Kill the epi interface
		epi::Shutdown();
	}

	//
	// Idle
	//
	// Called when this application has lost focus (i.e. an ALT+TAB event)
	//
	void Idle(void)
	{
		E_ReleaseAllKeys();
	}
	
	//
	// Tick
	//
	// This Function is called by I_Loop for a single loop in the
	// system.
	//
	// -ACB- 1999/09/24 Written
	// -ACB- 2004/05/31 Namespace'd
	//
	void Tick(void)
	{
		// -ES- 1998/09/11 It's a good idea to frequently check the heap
#ifdef DEVELOPERS
		//Z_CheckHeap();
//		L_WriteDebug("[0] Mem size: %ld\n", epi::the_mem_manager->GetAllocatedSize());
#endif

		bool fresh_game_tic;

		int counts = N_TryRunTics(&fresh_game_tic);

		DEV_ASSERT2(counts > 0);

		for (; counts > 0; counts--)  // run the tics
		{
			if (advance_title)
				E_DoAdvanceTitle();

			GUI_MainTicker();
			M_Ticker();

			G_Ticker(fresh_game_tic);

			sound::Ticker(); 
			S_MusicTicker(); // -ACB- 1999/11/13 Improved music update routines

			N_NetUpdate();  // check for new console commands
		}

		// Update display, next frame, with current state.
		E_Display();
	}

	//
	// Shutdown
	//
	void Shutdown()
	{
		/* ... */
	}
};

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
