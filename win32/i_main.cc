//----------------------------------------------------------------------------
//  EDGE Win32 Main Interface Functions
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
// -ACB- 1999/04/02
//
#include "..\i_defs.h"

#include "..\m_argv.h"
#include "..\e_main.h"

#include "i_sysinc.h"

HWND mainwindow;
HINSTANCE maininstance;
HACCEL accelerator;

// Not on MSVC
#ifdef __BORLANDC__

void InitSignal(void);

//
// Catcher
//
// Exception handler
//
// NOTE: it is important to reset our handler after we get
//       called. Otherwise, our handler won't get called
//       again.
//
void Catcher(int ExceptionType, int ExceptionError)
{
	char buf[20];
	int retval;
	static int ignore = 0;

	if (ignore)
	{
		InitSignal();
		return;
	}

	// make sure it is a floating point exception
	if (ExceptionType == SIGFPE)
	{
		switch (ExceptionError)                           
		{
    		case FPE_INTOVFLOW:       
    			retval = MessageBox(GetFocus(), "80x86 Interrupt on overflow !!!", "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
    
    		case FPE_INTDIV0:        
    			retval = MessageBox(GetFocus(), "Integer divide by zero!!!", "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
    
    		case FPE_INVALID:         
    			retval = MessageBox(GetFocus(), "80x87 invalid operation!!!", "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
    
    		case FPE_ZERODIVIDE:      
    			retval = MessageBox(GetFocus(), "Float divide by zero!!!", "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
    
    		case FPE_OVERFLOW:        
    			retval = MessageBox(GetFocus(), "80x87 arithmetic overflow!!!", "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
    
    		case FPE_UNDERFLOW:       
    			retval = MessageBox(GetFocus(), "80x87 arithmetic underflow!!!", "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
    
    		case FPE_INEXACT:         
    			retval = MessageBox(GetFocus(), "80x87 precision loss!!!", "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
    
    		case FPE_STACKFAULT:      
    			retval = MessageBox(GetFocus(), "80x87 stack overflow!!!", "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
    
    		case FPE_EXPLICITGEN:     
    			retval = MessageBox(GetFocus(), "SIGFPE raise()'d!!!", "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
    
    		default:
    			wsprintf(buf, "unknown exception %d\n\r", ExceptionType);
    			retval = MessageBox(GetFocus(), buf, "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
    			break;
		}
	}
	else
	{
		wsprintf(buf, "unknown exception %d\n\r", ExceptionType);
		retval = MessageBox(GetFocus(), buf, "Catcher", MB_ABORTRETRYIGNORE|MB_ICONWARNING);
	}

	if (retval == IDABORT)
		I_Error("User abort after signal error");

	if (retval == IDRETRY)
		InitSignal();

	if (retval == IDIGNORE)
	{
		InitSignal();
		ignore = 1;
	}
}

//
// InitSignal
//
// Set signal handler to be our own catcher
//
void InitSignal(void)
{
	if (signal(SIGFPE, (void (*)(int))Catcher) == SIG_ERR)
	{
		MessageBox(NULL, "Error installing signal handler", "Error", MB_OK);
		PostQuitMessage(0);
	}
}

#else
//
// InitSignal
//
// Set signal handler to be our own catcher
//
void InitSignal(void)
{
}
#endif

//
// I_InitApp
//
// Do work required for every instance of the application:
// Create the window, initialize data
//
// -ACB- 1999/01/25
//
bool I_InitApp(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	WNDCLASS wc;

	// Set up and register window class
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = I_WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NAME;
	wc.lpszClassName = NAME;
	RegisterClass(&wc);

	// Create a window
	hWnd = CreateWindow(NAME, TITLE,
		WS_OVERLAPPEDWINDOW,
		4, 4, 2, 2,
		NULL, NULL, hInstance, NULL);


	if (!hWnd)
		return false;

	ShowWindow(hWnd, SW_SHOWMINIMIZED); // -ACB- 2004/02/15 Start hidden regardless...
	UpdateWindow(hWnd);
	SetFocus(hWnd);

	memcpy(&mainwindow, &hWnd, sizeof(HWND)); 
	memcpy(&maininstance, &hInstance, sizeof(HINSTANCE));

	InitSignal();

	return true;
}

// -ACB- 2003/10/05 We need these outside the function, so we can delete them on exit
static const char **edgeargv = NULL;
static int edgeargc = 0;

//
// strtok_quote
//
// -AJA- 2003/10/11: like strtok() but handles double quotes (").
//
// This is to allow filenames with spaces.  Examples:
//
//      "C:\Program Files\Foo"
//      @"\Temp Stuff\RESPONSE.TXT"
//      My" "Wads\Good" "Time.wad
//
static char *strtok_quote(char *str, const char *sep)
{
	static char *cur_pos = NULL;

	if (str)
	{
		cur_pos = str;

		// ignore leading spaces
		while (*cur_pos && strchr(sep, *cur_pos) != NULL)
			cur_pos++;
	}

	if (*cur_pos == 0)
		return NULL;

	char *result = cur_pos;
	char *dest   = cur_pos;

	bool quoting = false;

	// look for an unquoted space, removing quotes as we go
	while (*cur_pos && (quoting || strchr(sep, *cur_pos) == NULL))
	{
		if (*cur_pos == '"')
		{
			quoting = ! quoting;
			cur_pos++;
			continue;
		}

		*dest++ = *cur_pos++;
	}

	if (dest < cur_pos)
		*dest = 0;

	// remove trailing spaces from argument
	while (*cur_pos && strchr(sep, *cur_pos) != NULL)
		*cur_pos++ = 0;
	
	return result;
}

//
// ParseParameters
//
// Parse the command line for parameters and
// give them in a form M_InitArguments
//
static void ParseParameters(void)
{
	char *s;
	char *p;
	char *cmdline;

	cmdline = GetCommandLine();

	// allow for 32 parameters
	edgeargv = new const char*[32];
	if (!edgeargv)
		I_Error("ParseParameters: FAILED ON PARAMETER POINTER MALLOC");

	edgeargc = 0;
	s = strtok_quote(cmdline, " ");
	while (s != NULL)
	{
		p = new char[strlen(s)+1];
		strcpy(p, s);
		edgeargv[edgeargc] = p;
		edgeargc++;
		s = strtok_quote(NULL, " ");

		// grow 32 elements at a time
		if ((edgeargc & 31) == 0)
		{
			const char **oldargv;

			oldargv = edgeargv;

			edgeargv = new const char*[edgeargc+32];
			if (!edgeargv)
				I_Error("ParseParameters: FAILED ON PARAMETER POINTER REALLOC");

			memcpy(edgeargv, oldargv, sizeof(char*)*edgeargc-1);
			delete [] oldargv;
		}
	}
}

//
// CleanupParameters
//
static void CleanupParameters(void)
{
	int i;

	if (edgeargc && edgeargv)
	{
		// Cleanup the remaining elements
		for (i=0; i<edgeargc; i++)
		{
			delete [] edgeargv[i];
		}
		delete [] edgeargv;
	}
}

static void I_ChangeToExeDir(const char *full_path)
{
	const char *r = strrchr(full_path, '\\');

	if (r == NULL || r == full_path)
		return;

	int length = (r - full_path) + 1;

	char *buf = new char[length];

	memcpy(buf, full_path, length);
	buf[length] = 0;

	SetCurrentDirectory(buf);

	delete[] buf;
}

//
// WinMain
//
// Life starts here....
//
int PASCAL WinMain (HINSTANCE curr, HINSTANCE prev, LPSTR cmdline, int show)
{
	HANDLE edgemap;

	// Check we've not already running
	edgemap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY, 0, 8, TITLE);
	if (edgemap)
	{
		DWORD lasterror = GetLastError();
		if (lasterror == ERROR_ALREADY_EXISTS)
		{
			MessageBox(NULL, "EDGE is already running!", TITLE, MB_ICONSTOP|MB_OK);
			return -1;
		}
	}

	// Setup our windozes bits...
	if (!I_InitApp(curr, show))
		return -1;

	// sort command line 
	ParseParameters();

	// -AJA- workaround for drag-and-drop (Windows sets our current
	//       directory to the windows directory C:\WINDOWS).
	if (edgeargc >= 1)
		I_ChangeToExeDir(edgeargv[0]);

	// Run Game....
	engine::Main(edgeargc, edgeargv);

	// Cleanup on exit
	CleanupParameters();
	return 0;
}

