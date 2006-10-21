//----------------------------------------------------------------------------
//  EDGE Main
//----------------------------------------------------------------------------
// 
//  Copyright (c) 1999-2006  The EDGE Team.
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

#include "i_defs.h"
#include "i_sdlinc.h"  // needed for proper SDL main linkage

#ifdef WIN32
#include "w32_sysinc.h"  // for <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dm_defs.h"
#include "m_argv.h"
#include "e_main.h"

#include "epi/strings.h"

// Application active?
int app_state = APP_STATE_ACTIVE;

extern "C" {

int main(int argc, char *argv[])
{
    I_SetupSignalHandlers(); 

    I_CheckAlreadyRunning();

    // -ACB- 2005/11/26 We don't do on LINUX since we assume the 
    //                  executable is globally installed
#ifndef LINUX
    // -AJA- change current dir to match executable
    I_ChangeToExeDir(argv[0]);
#endif

	// Run EDGE. it never returns
	engine::Main(argc, (const char **) argv);

	return 0;
}

} // extern "C"


void I_Loop(void)
{
	for (;;)
	{
		// We always do this once here, although the engine may makes in own
		// calls to keep on top of the event processing
		I_ControlGetEvents(); 
		
		if (app_state & APP_STATE_ACTIVE)
			engine::Tick();
			
		if (app_state & APP_STATE_PENDING_QUIT) // Engine may have set this 
			break;
	}
}
