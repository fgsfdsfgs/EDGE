//----------------------------------------------------------------------------
//  EDGE2 OpenGL Rendering (Textures)
//----------------------------------------------------------------------------
// 
//  Copyright (c) 1999-2018  The EDGE2 Team.
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

#include "system/i_defs.h"

/* NOTE: texture handling now done by image system */

/* --- AJA: test for a logo --- */

static const int logo_w = 124;
static const int logo_h = 30;

static const byte logo_data[] =
{
	2,2, 1,30, 23,90, 1,58, 4,2, 1,58, 16,90, 1,58, 
	25,2, 4,90, 1,30, 6,2, 1,58, 3,90, 1,58, 4,2, 
	24,90, 8,2, 1,90, 23,254, 1,226, 4,2, 1,175, 19,254, 
	1,175, 1,90, 1,30, 16,2, 1,30, 1,142, 1,226, 7,254, 
	1,175, 1,90, 3,2, 1,226, 3,254, 1,175, 4,2, 24,254, 
	1,58, 7,2, 1,90, 24,254, 1,90, 3,2, 1,175, 22,254, 
	1,142, 13,2, 1,30, 1,142, 12,254, 1,226, 1,58, 1,90, 
	4,254, 1,175, 4,2, 24,254, 1,175, 7,2, 1,90, 24,254, 
	1,226, 3,2, 1,175, 23,254, 1,226, 1,30, 10,2, 1,58, 
	1,226, 20,254, 1,175, 4,2, 25,254, 1,58, 8,2, 1,58, 
	1,175, 8,254, 1,175, 4,90, 1,175, 1,226, 7,254, 1,90, 
	4,2, 1,90, 1,226, 7,254, 1,175, 3,90, 1,142, 1,175, 
	8,254, 1,226, 1,30, 8,2, 1,58, 9,254, 1,226, 2,175, 
	1,226, 9,254, 1,175, 5,2, 1,30, 1,90, 8,254, 1,175, 
	4,90, 2,175, 7,254, 1,175, 10,2, 1,226, 6,254, 1,90, 
	7,2, 1,90, 1,226, 5,254, 1,175, 5,2, 1,58, 7,254, 
	6,2, 1,58, 1,226, 7,254, 1,226, 7,2, 1,30, 1,226, 
	7,254, 1,175, 1,58, 4,2, 1,58, 1,226, 7,254, 1,175, 
	7,2, 1,142, 6,254, 1,175, 7,2, 1,58, 1,175, 6,254, 
	1,30, 9,2, 1,175, 6,254, 1,90, 9,2, 1,142, 5,254, 
	1,58, 5,2, 7,254, 8,2, 1,175, 7,254, 1,142, 6,2, 
	1,142, 7,254, 1,175, 8,2, 1,175, 6,254, 1,175, 7,2, 
	1,90, 6,254, 1,175, 9,2, 1,90, 1,226, 4,254, 1,142, 
	9,2, 1,175, 6,254, 1,90, 10,2, 1,90, 4,254, 1,175, 
	5,2, 7,254, 8,2, 1,30, 1,226, 7,254, 1,58, 4,2, 
	1,30, 7,254, 1,175, 9,2, 1,30, 6,254, 1,175, 7,2, 
	1,90, 6,254, 1,175, 10,2, 1,30, 1,226, 4,254, 1,30, 
	8,2, 1,175, 6,254, 1,90, 11,2, 1,90, 4,254, 1,58, 
	4,2, 7,254, 9,2, 1,90, 7,254, 1,142, 4,2, 1,142, 
	7,254, 1,58, 10,2, 1,175, 5,254, 1,175, 7,2, 1,90, 
	6,254, 1,175, 11,2, 1,30, 1,226, 3,254, 1,142, 8,2, 
	1,175, 6,254, 1,90, 4,2, 1,90, 2,175, 1,90, 4,2, 
	1,90, 3,254, 1,142, 4,2, 7,254, 10,2, 1,226, 7,254, 
	4,2, 1,226, 6,254, 1,175, 11,2, 1,175, 5,254, 1,175, 
	7,2, 1,90, 6,254, 1,175, 4,2, 1,30, 3,175, 4,2, 
	1,30, 1,226, 2,254, 1,226, 8,2, 1,175, 6,254, 1,90, 
	4,2, 1,175, 2,254, 1,175, 5,2, 1,142, 1,254, 1,226, 
	1,90, 4,2, 7,254, 10,2, 1,142, 7,254, 1,90, 2,2, 
	1,58, 7,254, 1,90, 10,2, 1,30, 7,254, 1,30, 6,2, 
	1,90, 6,254, 1,175, 4,2, 1,90, 3,254, 5,2, 1,58, 
	2,254, 1,142, 1,30, 7,2, 1,175, 6,254, 1,90, 4,2, 
	1,226, 2,254, 1,175, 6,2, 1,90, 6,2, 7,254, 10,2, 
	1,90, 7,254, 1,90, 2,2, 1,90, 7,254, 1,58, 8,2, 
	1,90, 1,175, 8,254, 1,226, 1,175, 1,90, 4,2, 1,90, 
	6,254, 1,175, 4,2, 1,142, 3,254, 6,2, 1,90, 1,30, 
	9,2, 1,175, 6,254, 1,90, 3,2, 1,90, 3,254, 1,175, 
	13,2, 7,254, 11,2, 7,254, 1,175, 2,2, 1,175, 7,254, 
	9,2, 13,254, 4,2, 1,90, 6,254, 1,175, 3,2, 1,30, 
	4,254, 17,2, 1,175, 6,254, 1,142, 2,90, 1,175, 4,254, 
	1,175, 13,2, 7,254, 11,2, 7,254, 1,175, 2,2, 1,175, 
	7,254, 9,2, 13,254, 4,2, 1,90, 6,254, 1,175, 2,90, 
	1,142, 1,226, 4,254, 17,2, 1,175, 14,254, 1,175, 13,2, 
	7,254, 11,2, 7,254, 1,175, 2,2, 1,175, 7,254, 9,2, 
	13,175, 4,2, 1,90, 15,254, 17,2, 1,175, 14,254, 1,175, 
	13,2, 7,254, 11,2, 7,254, 1,175, 2,2, 1,175, 7,254, 
	26,2, 1,90, 15,254, 17,2, 1,175, 14,254, 1,175, 13,2, 
	7,254, 11,2, 7,254, 1,142, 2,2, 1,142, 7,254, 1,58, 
	25,2, 1,90, 15,254, 17,2, 1,175, 6,254, 1,142, 3,90, 
	1,175, 3,254, 1,175, 13,2, 7,254, 10,2, 1,90, 7,254, 
	1,90, 2,2, 1,90, 7,254, 1,90, 14,2, 1,58, 1,30, 
	9,2, 1,90, 6,254, 1,175, 3,90, 1,175, 4,254, 17,2, 
	1,175, 6,254, 1,90, 4,2, 1,226, 2,254, 1,175, 5,2, 
	1,90, 7,2, 7,254, 10,2, 1,142, 7,254, 1,30, 2,2, 
	1,30, 7,254, 1,175, 13,2, 1,30, 1,226, 1,254, 1,175, 
	1,58, 7,2, 1,90, 6,254, 1,175, 4,2, 1,142, 3,254, 
	5,2, 1,90, 1,30, 10,2, 1,175, 6,254, 1,90, 4,2, 
	1,175, 2,254, 1,175, 4,2, 1,175, 1,254, 1,226, 1,90, 
	5,2, 7,254, 10,2, 1,226, 6,254, 1,175, 4,2, 1,175, 
	7,254, 13,2, 1,175, 3,254, 1,30, 7,2, 1,90, 6,254, 
	1,175, 4,2, 1,90, 3,254, 4,2, 1,90, 2,254, 1,142, 
	9,2, 1,175, 6,254, 1,90, 4,2, 1,90, 2,254, 1,175, 
	3,2, 1,175, 3,254, 1,142, 5,2, 7,254, 9,2, 1,90, 
	7,254, 1,90, 4,2, 1,90, 7,254, 1,90, 11,2, 1,90, 
	3,254, 1,226, 8,2, 1,90, 6,254, 1,175, 4,2, 1,30, 
	3,254, 3,2, 1,90, 3,254, 1,226, 9,2, 1,175, 6,254, 
	1,90, 4,2, 1,58, 2,175, 1,90, 2,2, 1,175, 4,254, 
	1,30, 5,2, 7,254, 8,2, 1,30, 1,226, 6,254, 1,226, 
	6,2, 1,226, 6,254, 1,226, 1,30, 9,2, 1,90, 4,254, 
	1,175, 8,2, 1,90, 6,254, 1,175, 5,2, 3,175, 2,2, 
	1,90, 4,254, 1,90, 9,2, 1,175, 6,254, 1,90, 8,2, 
	1,30, 1,175, 4,254, 1,142, 6,2, 7,254, 7,2, 1,30, 
	1,226, 7,254, 1,90, 6,2, 1,58, 7,254, 1,226, 1,30, 
	6,2, 1,30, 1,175, 5,254, 1,90, 8,2, 1,90, 6,254, 
	1,175, 9,2, 1,142, 4,254, 1,226, 9,2, 1,30, 7,254, 
	1,175, 6,2, 1,30, 1,142, 6,254, 1,30, 5,2, 1,90, 
	7,254, 1,58, 5,2, 1,90, 1,226, 7,254, 1,142, 8,2, 
	1,90, 7,254, 1,226, 1,90, 1,30, 2,2, 1,90, 1,142, 
	7,254, 9,2, 1,175, 6,254, 1,226, 1,30, 6,2, 1,90, 
	1,226, 5,254, 1,90, 6,2, 1,30, 1,90, 1,175, 1,226, 
	9,254, 4,175, 1,226, 7,254, 1,142, 3,2, 1,58, 1,90, 
	1,175, 9,254, 4,175, 9,254, 1,175, 10,2, 1,142, 19,254, 
	1,175, 6,2, 1,90, 1,142, 1,175, 9,254, 1,226, 4,175, 
	7,254, 1,226, 7,2, 1,90, 24,254, 1,30, 3,2, 1,175, 
	23,254, 1,90, 12,2, 1,90, 1,226, 17,254, 1,142, 6,2, 
	24,254, 1,90, 7,2, 1,90, 23,254, 1,142, 4,2, 1,175, 
	21,254, 1,175, 1,30, 14,2, 1,30, 1,142, 11,254, 1,226, 
	1,90, 3,254, 1,58, 6,2, 23,254, 1,226, 8,2, 1,90, 
	23,254, 1,30, 4,2, 1,175, 17,254, 2,175, 1,90, 1,30, 
	19,2, 1,90, 2,175, 4,254, 1,175, 1,142, 1,58, 2,2, 
	1,58, 2,90, 7,2, 23,254, 1,90, 254,2, 

	0, 0  // THE END
};

static const int init_w = 216;
static const int init_h = 27;

static const byte init_data[] =
{
	200,0, 200,0, 200,0, 213,0, 1,28, 3,85, 1,28, 
	211,0, 1,226, 4,255, 1,198, 1,28, 209,0, 4,85, 1,141, 
	1,255, 1,226, 1,28, 213,0, 1,113, 1,255, 1,113, 213,0, 
	1,28, 1,255, 1,170, 48,0, 1,85, 8,170, 1,141, 3,0, 
	1,28, 4,170, 1,85, 2,0, 1,141, 3,170, 1,141, 2,0, 
	1,28, 10,170, 1,85, 6,0, 1,85, 1,198, 2,255, 1,198, 
	1,141, 1,56, 4,0, 1,85, 10,170, 1,28, 3,0, 1,28, 
	1,170, 2,255, 1,226, 1,141, 1,28, 1,0, 2,170, 1,141, 
	3,0, 1,141, 9,170, 1,85, 3,0, 1,85, 10,170, 1,28, 
	2,0, 1,56, 1,255, 1,113, 1,170, 1,198, 2,255, 1,198, 
	1,141, 1,28, 5,0, 1,141, 9,170, 1,141, 2,0, 1,28, 
	4,170, 1,85, 2,0, 1,141, 3,170, 1,141, 6,0, 2,85, 
	1,56, 2,0, 1,255, 1,170, 7,0, 1,85, 1,226, 1,255, 
	1,198, 1,28, 9,0, 1,28, 1,198, 1,255, 1,226, 1,85, 
	10,0, 1,141, 2,255, 1,141, 8,0, 1,141, 8,255, 1,226, 
	3,0, 1,56, 4,255, 1,141, 2,0, 1,226, 3,255, 1,226, 
	2,0, 1,56, 10,255, 1,141, 5,0, 1,85, 1,255, 1,226, 
	2,170, 3,255, 1,170, 3,0, 1,141, 10,255, 1,56, 2,0, 
	1,28, 1,226, 1,255, 1,226, 1,170, 1,198, 2,255, 1,170, 
	2,255, 1,226, 3,0, 1,226, 9,255, 1,141, 3,0, 1,141, 
	10,255, 1,56, 2,0, 1,85, 3,255, 1,226, 2,170, 1,226, 
	1,255, 1,226, 1,28, 4,0, 1,226, 9,255, 1,226, 2,0, 
	1,56, 4,255, 1,141, 2,0, 1,226, 3,255, 1,226, 4,0, 
	1,113, 4,255, 1,226, 1,85, 1,255, 1,170, 7,0, 1,226, 
	3,255, 1,141, 9,0, 1,141, 3,255, 1,226, 9,0, 1,56, 
	4,255, 1,56, 11,0, 1,170, 1,255, 9,0, 1,170, 1,255, 
	5,0, 1,85, 1,255, 1,85, 8,0, 1,170, 1,255, 10,0, 
	1,226, 1,255, 1,28, 3,0, 1,85, 2,198, 8,0, 1,255, 
	1,170, 7,0, 1,141, 1,255, 1,170, 3,0, 1,28, 1,141, 
	2,255, 1,85, 8,0, 1,170, 1,255, 13,0, 1,255, 1,170, 
	7,0, 1,85, 1,255, 1,226, 1,56, 4,0, 1,85, 1,255, 
	1,113, 8,0, 1,85, 1,255, 1,85, 8,0, 1,170, 1,255, 
	5,0, 1,85, 1,255, 1,85, 4,0, 1,141, 2,255, 1,141, 
	1,85, 1,113, 1,198, 2,255, 1,170, 7,0, 1,226, 3,255, 
	1,141, 9,0, 1,141, 3,255, 1,226, 9,0, 1,56, 4,255, 
	1,56, 11,0, 1,170, 1,255, 9,0, 1,170, 1,255, 5,0, 
	1,85, 1,255, 1,85, 8,0, 1,170, 1,255, 10,0, 1,255, 
	1,170, 15,0, 1,255, 1,170, 7,0, 1,170, 1,255, 1,85, 
	5,0, 1,141, 1,255, 1,85, 8,0, 1,170, 1,255, 13,0, 
	1,255, 1,170, 7,0, 1,56, 1,255, 1,28, 6,0, 1,255, 
	1,170, 8,0, 1,85, 1,255, 1,85, 8,0, 1,170, 1,255, 
	5,0, 1,85, 1,255, 1,85, 3,0, 1,28, 2,255, 1,28, 
	4,0, 1,170, 1,255, 1,170, 7,0, 1,85, 1,226, 1,255, 
	1,198, 1,28, 9,0, 1,28, 1,198, 1,255, 1,226, 1,85, 
	10,0, 1,141, 2,255, 1,141, 12,0, 1,170, 1,255, 9,0, 
	1,170, 1,255, 5,0, 1,85, 1,255, 1,85, 8,0, 1,170, 
	1,255, 10,0, 1,255, 1,170, 15,0, 1,255, 1,170, 7,0, 
	1,141, 1,255, 1,170, 5,0, 1,85, 1,255, 1,85, 8,0, 
	1,170, 1,255, 13,0, 1,255, 1,170, 11,0, 1,56, 3,85, 
	1,170, 1,255, 1,141, 8,0, 1,85, 1,255, 1,85, 8,0, 
	1,170, 1,255, 5,0, 1,85, 1,255, 1,85, 3,0, 1,113, 
	1,255, 1,141, 5,0, 1,28, 1,255, 1,170, 52,0, 1,170, 
	1,255, 9,0, 1,170, 1,255, 5,0, 1,85, 1,255, 1,85, 
	8,0, 1,170, 1,255, 10,0, 1,255, 1,170, 15,0, 1,255, 
	1,170, 7,0, 1,28, 1,226, 1,255, 1,226, 1,141, 2,85, 
	1,113, 1,198, 1,255, 1,85, 8,0, 1,170, 1,255, 13,0, 
	1,255, 1,170, 8,0, 1,28, 1,170, 6,255, 1,226, 1,28, 
	8,0, 1,85, 1,255, 1,85, 8,0, 1,170, 1,255, 5,0, 
	1,85, 1,255, 1,85, 3,0, 1,170, 1,255, 1,85, 6,0, 
	1,255, 1,170, 52,0, 1,170, 1,255, 9,0, 1,170, 1,255, 
	5,0, 1,85, 1,255, 1,85, 8,0, 1,170, 1,255, 10,0, 
	1,255, 1,170, 15,0, 1,255, 1,170, 8,0, 1,28, 1,198, 
	7,255, 1,85, 8,0, 1,170, 1,255, 13,0, 1,255, 1,170, 
	8,0, 1,226, 2,255, 1,226, 3,170, 1,113, 10,0, 1,85, 
	1,255, 1,85, 8,0, 1,170, 1,255, 5,0, 1,85, 1,255, 
	1,85, 3,0, 1,170, 1,255, 1,85, 6,0, 1,255, 1,170, 
	52,0, 1,170, 1,255, 9,0, 1,170, 1,255, 1,28, 4,0, 
	1,85, 1,255, 1,85, 8,0, 1,170, 1,255, 10,0, 1,255, 
	1,170, 15,0, 1,255, 1,170, 11,0, 3,85, 1,56, 1,85, 
	1,255, 1,85, 8,0, 1,170, 1,255, 13,0, 1,255, 1,170, 
	7,0, 1,85, 1,255, 1,170, 5,0, 1,28, 1,56, 9,0, 
	1,85, 1,255, 1,85, 8,0, 1,170, 1,255, 1,28, 4,0, 
	1,85, 1,255, 1,85, 3,0, 1,85, 1,255, 1,198, 5,0, 
	1,56, 1,255, 1,170, 52,0, 1,170, 1,255, 9,0, 1,170, 
	1,255, 1,170, 4,0, 1,198, 1,255, 1,28, 8,0, 1,170, 
	1,255, 7,0, 1,56, 2,85, 1,255, 1,198, 5,85, 1,28, 
	9,0, 1,255, 1,170, 9,0, 1,85, 1,28, 4,0, 1,170, 
	1,255, 1,56, 8,0, 1,170, 1,255, 13,0, 1,255, 1,170, 
	7,0, 1,85, 1,255, 1,113, 4,0, 1,28, 1,226, 1,255, 
	9,0, 1,85, 1,255, 1,85, 8,0, 1,170, 1,255, 1,170, 
	4,0, 1,198, 1,255, 1,28, 4,0, 1,226, 1,255, 1,113, 
	3,0, 1,28, 1,226, 1,255, 1,170, 52,0, 1,170, 1,255, 
	7,0, 1,28, 1,170, 1,226, 2,255, 1,198, 1,113, 1,85, 
	1,170, 1,255, 1,198, 6,0, 1,141, 2,170, 1,226, 1,255, 
	7,0, 10,255, 1,170, 5,0, 1,28, 3,170, 1,255, 1,170, 
	8,0, 1,85, 2,255, 1,170, 2,85, 1,170, 1,255, 1,226, 
	9,0, 1,170, 1,255, 9,0, 1,28, 3,170, 1,255, 1,170, 
	8,0, 1,226, 1,255, 1,141, 2,85, 1,141, 3,255, 6,0, 
	1,85, 2,170, 1,198, 1,255, 1,85, 6,0, 1,28, 1,170, 
	1,226, 2,255, 1,198, 1,113, 1,85, 1,170, 1,255, 1,198, 
	5,0, 1,56, 1,226, 1,255, 1,226, 1,170, 1,198, 1,255, 
	1,226, 1,255, 1,226, 1,170, 1,28, 50,0, 1,170, 1,255, 
	7,0, 1,56, 3,255, 1,56, 1,226, 3,255, 1,198, 1,28, 
	6,0, 1,226, 4,255, 7,0, 1,56, 2,85, 1,255, 1,198, 
	5,85, 1,28, 5,0, 1,56, 4,255, 1,170, 9,0, 1,141, 
	5,255, 1,226, 1,28, 9,0, 1,170, 1,255, 9,0, 1,56, 
	4,255, 1,170, 8,0, 1,28, 1,198, 4,255, 2,226, 1,255, 
	6,0, 1,141, 4,255, 1,85, 6,0, 1,56, 3,255, 1,56, 
	1,226, 3,255, 1,198, 1,28, 6,0, 1,28, 1,141, 1,226, 
	1,255, 1,226, 1,141, 1,28, 3,255, 1,56, 50,0, 1,170, 
	1,255, 13,0, 1,56, 1,85, 1,56, 23,0, 1,255, 1,170, 
	28,0, 3,85, 1,56, 11,0, 1,170, 1,255, 25,0, 1,28, 
	2,85, 1,56, 1,0, 1,28, 1,56, 24,0, 1,56, 1,85, 
	1,56, 65,0, 1,28, 3,85, 1,198, 1,255, 3,85, 1,56, 
	35,0, 1,255, 1,170, 43,0, 1,170, 1,255, 124,0, 1,170, 
	9,255, 23,0, 1,141, 1,170, 1,28, 9,0, 1,255, 1,170, 
	14,0, 1,28, 1,170, 1,141, 26,0, 1,170, 1,255, 12,0, 
	1,28, 1,170, 1,141, 26,0, 1,85, 1,170, 1,85, 80,0, 
	1,28, 8,85, 1,56, 23,0, 2,255, 1,85, 9,0, 1,56, 
	1,28, 14,0, 1,85, 2,255, 22,0, 1,56, 5,255, 12,0, 
	1,85, 2,255, 26,0, 1,170, 1,255, 1,170, 113,0, 2,255, 
	1,85, 25,0, 1,85, 2,255, 22,0, 1,28, 5,170, 12,0, 
	1,85, 2,255, 26,0, 1,170, 1,255, 1,170, 113,0, 1,56, 
	1,85, 27,0, 1,85, 1,56, 41,0, 1,85, 1,56, 26,0, 
	1,28, 1,85, 1,28, 253,0, 254,0, 

	0,0  // THE END
};

static const int glbsp_w = 100;
static const int glbsp_h = 36;

static const byte glbsp_data[] =
{
	6,0, 5,85, 91,0, 1,28, 1,141, 1,226, 7,255, 1,226,
	1,141, 1,28, 86,0, 1,85, 5,255, 1,170, 3,85, 1,170,
	3,255, 1,141, 84,0, 1,85, 4,255, 1,226, 1,56, 5,0,
	1,56, 3,255, 1,170, 83,0, 1,226, 4,255, 1,113, 7,0,
	1,141, 3,255, 1,113, 82,0, 5,255, 8,0, 1,85, 3,255,
	1,198, 82,0, 1,198, 4,255, 8,0, 1,113, 4,255, 82,0,
	1,28, 1,226, 3,255, 1,141, 6,0, 1,85, 5,255, 84,0,
	1,113, 1,170, 3,255, 4,170, 6,255, 1,226, 84,0, 1,56,
	1,198, 13,255, 1,141, 1,0, 10,255, 1,0, 1,170, 14,255,
	1,198, 1,170, 1,113, 1,56, 9,0, 1,56, 1,113, 1,170,
	1,226, 4,255, 1,170, 1,141, 1,56, 6,0, 1,170, 11,255,
	1,85, 13,0, 1,85, 14,255, 1,226, 1,28, 1,0, 1,170,
	1,198, 6,255, 1,226, 1,170, 1,0, 1,113, 1,170, 1,226,
	7,255, 1,198, 2,170, 1,226, 5,255, 1,198, 1,28, 5,0,
	1,56, 1,198, 4,255, 1,226, 2,170, 1,226, 3,255, 1,226,
	1,56, 4,0, 1,56, 1,113, 1,170, 6,255, 1,226, 1,141,
	1,85, 1,28, 13,0, 1,226, 13,255, 1,198, 1,28, 4,0,
	1,198, 4,255, 1,198, 6,0, 1,198, 5,255, 1,113, 4,0,
	1,113, 6,255, 1,56, 3,0, 1,28, 4,255, 1,198, 1,56,
	4,0, 1,113, 4,255, 1,85, 6,0, 1,198, 5,255, 1,28,
	16,0, 5,255, 1,226, 6,170, 1,113, 1,56, 6,0, 1,170,
	4,255, 1,170, 6,0, 1,170, 5,255, 6,0, 1,113, 5,255,
	1,226, 3,0, 1,85, 3,255, 1,170, 7,0, 1,113, 4,255,
	1,56, 5,0, 1,170, 5,255, 17,0, 1,198, 3,255, 1,170,
	15,0, 1,170, 4,255, 1,170, 6,0, 1,170, 5,255, 7,0,
	6,255, 1,85, 2,0, 1,85, 2,255, 1,170, 8,0, 1,28,
	4,255, 1,198, 5,0, 1,170, 5,255, 17,0, 1,28, 1,226,
	2,255, 1,141, 15,0, 1,170, 4,255, 1,170, 6,0, 1,170,
	5,255, 7,0, 1,170, 5,255, 1,170, 2,0, 1,85, 2,255,
	1,28, 9,0, 5,255, 1,28, 4,0, 1,170, 5,255, 19,0,
	1,113, 1,198, 1,255, 1,198, 3,170, 1,141, 1,85, 1,28,
	8,0, 1,170, 4,255, 1,170, 6,0, 1,170, 5,255, 7,0,
	1,170, 5,255, 1,170, 2,0, 1,113, 1,255, 1,113, 9,0,
	1,28, 5,255, 1,85, 4,0, 1,170, 5,255, 18,0, 1,56,
	1,198, 9,255, 1,141, 1,28, 6,0, 1,170, 4,255, 1,170,
	6,0, 1,170, 5,255, 7,0, 1,170, 5,255, 1,170, 2,0,
	1,170, 1,255, 1,28, 9,0, 1,141, 5,255, 1,85, 4,0,
	1,170, 5,255, 17,0, 1,85, 5,255, 1,85, 1,0, 1,56,
	1,226, 3,255, 1,226, 1,28, 5,0, 1,170, 4,255, 1,170,
	6,0, 1,170, 5,255, 7,0, 1,226, 5,255, 1,113, 2,0,
	2,56, 9,0, 1,85, 6,255, 1,85, 4,0, 1,170, 5,255,
	16,0, 1,28, 5,255, 1,141, 3,0, 1,113, 4,255, 1,198,
	5,0, 1,170, 4,255, 1,170, 6,0, 1,170, 5,255, 6,0,
	1,56, 6,255, 1,28, 11,0, 1,28, 1,170, 7,255, 5,0,
	1,170, 5,255, 16,0, 1,113, 5,255, 1,28, 3,0, 1,28,
	5,255, 1,56, 4,0, 1,170, 4,255, 1,170, 6,0, 1,170,
	5,255, 6,0, 1,226, 5,255, 1,141, 10,0, 1,56, 1,170,
	8,255, 1,141, 5,0, 1,170, 10,255, 2,170, 1,85, 8,0,
	1,170, 5,255, 5,0, 5,255, 1,85, 4,0, 1,170, 4,255,
	1,170, 6,0, 1,170, 5,255, 4,0, 1,56, 1,226, 5,255,
	1,170, 9,0, 1,113, 1,226, 9,255, 1,226, 1,28, 5,0,
	1,170, 5,255, 1,226, 2,170, 1,226, 4,255, 1,226, 1,113,
	6,0, 1,170, 5,255, 5,0, 5,255, 1,85, 4,0, 1,170,
	4,255, 1,170, 6,0, 1,170, 5,255, 3,170, 1,226, 5,255,
	1,198, 1,56, 8,0, 1,56, 1,226, 10,255, 1,226, 1,28,
	6,0, 1,170, 5,255, 1,28, 3,0, 1,85, 5,255, 1,170,
	5,0, 1,85, 5,255, 1,28, 3,0, 1,28, 5,255, 5,0,
	1,170, 4,255, 1,170, 6,0, 1,170, 13,255, 1,85, 9,0,
	1,85, 11,255, 1,141, 1,28, 7,0, 1,170, 5,255, 5,0,
	1,85, 5,255, 1,141, 5,0, 1,226, 4,255, 1,113, 3,0,
	1,113, 4,255, 1,170, 1,85, 4,0, 1,170, 4,255, 1,170,
	6,0, 1,170, 5,255, 3,0, 1,28, 1,113, 1,226, 3,255,
	1,226, 1,85, 6,0, 1,56, 10,255, 1,141, 1,28, 9,0,
	1,170, 5,255, 6,0, 1,198, 5,255, 1,28, 4,0, 1,28,
	1,226, 4,255, 1,85, 1,0, 1,56, 8,255, 3,0, 1,170,
	4,255, 1,170, 6,0, 1,170, 5,255, 5,0, 1,28, 1,226,
	4,255, 1,85, 5,0, 1,198, 7,255, 1,226, 1,113, 12,0,
	1,170, 5,255, 6,0, 1,141, 5,255, 1,85, 5,0, 1,28,
	1,170, 9,255, 1,170, 1,198, 3,255, 1,56, 2,0, 1,170,
	4,255, 1,170, 6,0, 1,170, 5,255, 6,0, 1,113, 5,255,
	1,28, 3,0, 1,56, 6,255, 1,226, 1,113, 14,0, 1,170,
	5,255, 6,0, 1,85, 5,255, 1,170, 7,0, 1,28, 1,85,
	1,141, 3,170, 1,141, 1,85, 3,0, 1,28, 1,113, 1,198,
	1,85, 2,0, 1,170, 4,255, 1,170, 6,0, 1,170, 5,255,
	6,0, 1,28, 5,255, 1,141, 3,0, 1,85, 5,255, 1,226,
	1,28, 7,0, 2,56, 6,0, 1,170, 5,255, 6,0, 1,85,
	5,255, 1,170, 24,0, 1,170, 4,255, 1,170, 6,0, 1,170,
	5,255, 7,0, 5,255, 1,170, 3,0, 1,85, 5,255, 1,85,
	8,0, 1,198, 1,170, 6,0, 1,170, 5,255, 6,0, 1,85,
	5,255, 1,170, 24,0, 1,170, 4,255, 1,170, 6,0, 1,170,
	5,255, 7,0, 5,255, 1,170, 3,0, 1,28, 5,255, 8,0,
	1,56, 1,255, 1,170, 6,0, 1,170, 5,255, 6,0, 1,141,
	5,255, 1,113, 24,0, 1,170, 4,255, 1,170, 6,0, 1,170,
	5,255, 6,0, 1,56, 5,255, 1,141, 4,0, 1,226, 4,255,
	8,0, 1,170, 1,255, 1,170, 6,0, 1,170, 5,255, 6,0,
	1,198, 5,255, 1,56, 24,0, 1,170, 4,255, 1,170, 6,0,
	1,170, 5,255, 6,0, 1,198, 5,255, 1,56, 4,0, 1,85,
	4,255, 1,113, 6,0, 1,85, 2,255, 1,170, 6,0, 1,170,
	5,255, 5,0, 1,85, 5,255, 1,198, 25,0, 1,170, 4,255,
	1,170, 6,0, 1,170, 5,255, 1,28, 3,0, 1,28, 1,170,
	5,255, 1,141, 6,0, 1,170, 4,255, 1,85, 4,0, 1,113,
	3,255, 1,170, 6,0, 1,198, 5,255, 1,28, 3,0, 1,85,
	1,226, 4,255, 1,226, 1,28, 25,0, 1,198, 4,255, 1,170,
	3,0, 1,56, 1,85, 1,170, 6,255, 1,226, 2,170, 1,198,
	6,255, 1,113, 8,0, 1,85, 1,226, 3,255, 1,226, 2,170,
	5,255, 1,113, 3,0, 1,56, 1,85, 1,170, 6,255, 1,226,
	2,170, 1,226, 5,255, 1,198, 1,28, 24,0, 1,113, 1,198,
	5,255, 1,170, 3,0, 1,170, 14,255, 1,198, 1,170, 1,85,
	1,28, 11,0, 1,85, 1,170, 1,198, 4,255, 1,198, 1,170,
	1,85, 5,0, 1,170, 14,255, 1,170, 1,141, 1,56, 26,0,
	1,113, 6,255, 1,170, 95,0, 1,28, 2,85, 1,170, 1,113,
	73,0,

	0,0  // THE END
};

static const int build_w = 264;
static const int build_h = 19;

static const byte build_data[] =
{
	96,0, 1,113, 3,255, 1,170, 1,56, 200,0, 58,0, 1,56,
	1,113, 1,85, 1,113, 1,226, 1,255, 1,85, 200,0, 61,0,
	1,28, 1,255, 1,198, 200,0, 62,0, 1,170, 1,255, 161,0,
	1,141, 7,170, 1,113, 1,28, 6,0, 1,113, 2,170, 1,113,
	1,28, 1,113, 2,170, 1,28, 2,0, 1,28, 9,170, 1,85,
	3,0, 1,141, 8,170, 1,85, 5,0, 1,85, 2,170, 1,141,
	1,56, 1,0, 2,170, 1,141, 2,0, 1,28, 9,170, 1,85,
	2,0, 1,85, 3,170, 1,85, 2,0, 1,141, 3,170, 1,28,
	5,0, 2,85, 1,28, 1,0, 1,170, 1,255, 19,0, 1,28,
	1,141, 1,198, 3,255, 1,170, 1,141, 1,28, 2,0, 1,141,
	10,170, 1,85, 15,0, 1,113, 4,170, 1,28, 2,0, 1,113,
	1,226, 1,56, 5,0, 1,56, 1,113, 2,170, 1,113, 1,56,
	7,0, 1,56, 1,141, 2,170, 1,85, 1,0, 1,113, 2,170,
	1,28, 4,0, 1,85, 3,170, 1,113, 1,56, 5,0, 1,85,
	1,170, 1,85, 4,170, 1,85, 8,0, 1,28, 1,141, 1,170,
	1,56, 10,0, 1,113, 1,170, 1,113, 10,0, 1,56, 1,170,
	1,141, 1,28, 3,0, 1,141, 1,198, 1,255, 1,198, 3,170,
	1,198, 2,255, 1,85, 4,0, 1,170, 1,255, 1,226, 1,170,
	1,198, 2,226, 1,255, 1,170, 1,28, 2,0, 1,28, 3,170,
	1,198, 1,255, 1,198, 3,170, 1,85, 3,0, 1,141, 3,170,
	1,255, 1,226, 3,170, 1,85, 3,0, 1,28, 1,226, 1,255,
	1,226, 1,170, 1,198, 1,255, 1,170, 1,255, 1,226, 1,141,
	2,0, 1,28, 3,170, 1,198, 1,255, 1,198, 3,170, 1,85,
	2,0, 1,85, 1,198, 1,255, 1,198, 1,85, 2,0, 1,141,
	1,226, 1,255, 1,170, 1,28, 3,0, 1,113, 4,255, 2,198,
	1,255, 18,0, 1,85, 1,226, 1,255, 1,198, 2,85, 1,113,
	1,198, 1,255, 1,170, 2,0, 1,141, 1,170, 1,198, 1,255,
	1,198, 5,170, 1,255, 1,170, 15,0, 1,113, 1,226, 1,255,
	2,170, 1,28, 1,0, 1,28, 2,255, 1,85, 4,0, 1,170,
	2,255, 2,170, 2,255, 1,170, 5,0, 1,170, 2,255, 2,170,
	1,255, 1,226, 1,198, 1,255, 1,170, 1,28, 2,0, 1,85,
	1,226, 1,255, 1,198, 2,170, 2,255, 1,226, 1,85, 3,0,
	1,170, 3,255, 2,170, 1,198, 1,255, 1,198, 7,0, 1,226,
	3,255, 1,56, 8,0, 1,141, 3,255, 1,141, 8,0, 1,56,
	3,255, 1,226, 4,0, 1,85, 1,255, 1,85, 4,0, 1,56,
	1,255, 1,226, 3,0, 1,28, 1,255, 1,198, 3,0, 1,85,
	2,255, 8,0, 1,85, 1,255, 1,85, 11,0, 1,255, 1,170,
	7,0, 1,226, 1,255, 1,113, 3,0, 1,85, 2,255, 1,170,
	7,0, 1,85, 1,255, 1,85, 7,0, 1,85, 1,255, 1,85,
	4,0, 1,170, 1,255, 4,0, 1,85, 1,255, 1,226, 1,56,
	1,0, 1,56, 1,198, 2,255, 17,0, 1,28, 1,226, 1,255,
	1,85, 5,0, 1,255, 1,170, 4,0, 1,85, 1,255, 1,85,
	5,0, 1,255, 1,170, 16,0, 1,170, 1,255, 4,0, 1,141,
	2,255, 1,85, 3,0, 1,170, 1,255, 1,170, 4,0, 1,170,
	1,255, 1,170, 3,0, 1,141, 1,255, 1,170, 1,28, 2,0,
	1,28, 1,226, 2,255, 3,0, 1,28, 1,255, 1,226, 1,56,
	4,0, 1,113, 1,198, 1,226, 3,0, 1,170, 1,255, 1,141,
	4,0, 1,85, 1,255, 1,113, 6,0, 4,255, 1,85, 8,0,
	1,170, 3,255, 1,170, 8,0, 1,85, 4,255, 4,0, 1,85,
	1,255, 1,85, 5,0, 1,170, 1,255, 1,85, 2,0, 1,85,
	1,255, 1,85, 4,0, 1,198, 1,255, 8,0, 1,85, 1,255,
	1,85, 11,0, 1,255, 1,170, 6,0, 1,85, 1,255, 1,198,
	5,0, 1,113, 1,255, 1,170, 7,0, 1,85, 1,255, 1,85,
	7,0, 1,85, 1,255, 1,85, 4,0, 1,170, 1,255, 4,0,
	1,226, 1,255, 1,28, 3,0, 1,28, 2,255, 17,0, 1,170,
	1,255, 1,113, 6,0, 1,255, 1,170, 4,0, 1,85, 1,255,
	1,85, 5,0, 1,255, 1,170, 16,0, 1,170, 1,255, 3,0,
	1,28, 1,255, 1,198, 1,255, 1,85, 2,0, 1,56, 1,255,
	1,198, 6,0, 1,198, 1,255, 1,56, 2,0, 2,255, 1,28,
	4,0, 1,28, 2,255, 3,0, 1,170, 1,255, 1,113, 11,0,
	1,141, 1,198, 6,0, 1,255, 1,170, 6,0, 1,85, 1,226,
	1,255, 1,141, 9,0, 1,28, 1,198, 1,255, 1,198, 1,28,
	9,0, 1,141, 1,255, 1,226, 1,85, 4,0, 1,85, 1,255,
	1,85, 5,0, 1,170, 1,255, 1,85, 2,0, 1,85, 1,255,
	1,85, 4,0, 1,170, 1,255, 8,0, 1,85, 1,255, 1,85,
	11,0, 1,255, 1,170, 6,0, 1,170, 1,255, 1,85, 6,0,
	1,255, 1,170, 7,0, 1,85, 1,255, 1,85, 7,0, 1,85,
	1,255, 1,85, 4,0, 1,170, 1,255, 3,0, 1,56, 1,255,
	1,198, 5,0, 1,170, 1,255, 17,0, 2,255, 3,0, 1,28,
	3,85, 1,255, 1,198, 1,28, 3,0, 1,85, 1,255, 1,85,
	5,0, 1,255, 1,170, 16,0, 1,170, 1,255, 3,0, 2,198,
	1,85, 1,255, 1,85, 2,0, 1,141, 1,255, 1,113, 6,0,
	1,113, 1,255, 1,141, 1,0, 1,85, 1,255, 1,170, 6,0,
	1,170, 1,255, 3,0, 2,255, 15,0, 1,56, 3,85, 1,198,
	1,255, 1,141, 41,0, 1,85, 1,255, 1,85, 4,0, 1,28,
	2,255, 1,28, 2,0, 1,85, 1,255, 1,85, 4,0, 1,170,
	1,255, 8,0, 1,85, 1,255, 1,85, 11,0, 1,255, 1,170,
	6,0, 1,170, 1,255, 1,85, 6,0, 1,255, 1,170, 7,0,
	1,85, 1,255, 1,85, 7,0, 1,85, 1,255, 1,85, 4,0,
	1,170, 1,255, 3,0, 1,85, 1,255, 1,170, 5,0, 1,170,
	1,255, 16,0, 1,85, 1,255, 1,170, 3,0, 1,85, 5,255,
	1,141, 3,0, 1,85, 1,255, 1,85, 5,0, 1,226, 1,141,
	16,0, 1,170, 1,255, 2,0, 1,56, 1,255, 1,56, 1,85,
	1,255, 1,85, 2,0, 1,170, 1,255, 1,85, 6,0, 1,85,
	1,255, 1,170, 1,0, 1,85, 1,255, 1,170, 6,0, 1,170,
	1,255, 3,0, 10,255, 1,226, 4,0, 1,141, 6,255, 1,226,
	1,28, 41,0, 1,85, 1,255, 1,198, 3,170, 1,198, 1,255,
	1,226, 1,85, 3,0, 1,85, 1,255, 1,85, 4,0, 1,170,
	1,255, 8,0, 1,85, 1,255, 1,85, 11,0, 1,255, 1,170,
	6,0, 1,141, 1,255, 1,141, 5,0, 1,28, 1,255, 1,170,
	7,0, 1,85, 1,255, 1,85, 7,0, 1,85, 1,255, 1,85,
	4,0, 1,170, 1,255, 3,0, 1,56, 1,255, 1,198, 5,0,
	1,170, 1,255, 16,0, 1,85, 1,255, 1,170, 13,0, 1,85,
	1,255, 1,85, 23,0, 1,170, 1,255, 2,0, 2,198, 1,0,
	1,85, 1,255, 1,85, 2,0, 1,141, 1,255, 1,113, 6,0,
	1,113, 1,255, 1,141, 1,0, 1,56, 1,255, 1,226, 6,0,
	1,198, 1,255, 3,0, 1,226, 1,255, 1,113, 6,85, 2,255,
	3,0, 1,113, 1,255, 1,226, 2,170, 1,141, 1,85, 1,56,
	43,0, 1,85, 1,255, 1,198, 3,170, 2,255, 1,141, 4,0,
	1,85, 1,255, 1,85, 4,0, 1,170, 1,255, 8,0, 1,85,
	1,255, 1,85, 11,0, 1,255, 1,170, 6,0, 1,56, 1,255,
	1,226, 5,0, 1,141, 1,255, 1,170, 7,0, 1,85, 1,255,
	1,85, 7,0, 1,85, 1,255, 1,141, 4,0, 1,198, 1,255,
	4,0, 1,226, 1,255, 1,28, 3,0, 1,28, 2,255, 16,0,
	1,85, 1,255, 1,170, 13,0, 1,85, 1,255, 1,85, 23,0,
	1,170, 1,255, 1,0, 1,56, 1,255, 1,56, 1,0, 1,85,
	1,255, 1,85, 2,0, 1,56, 1,255, 1,226, 6,0, 1,226,
	1,255, 1,56, 2,0, 1,226, 1,255, 1,56, 4,0, 1,56,
	2,255, 3,0, 1,141, 1,255, 1,113, 5,0, 1,113, 1,255,
	1,170, 3,0, 1,170, 1,255, 1,28, 4,0, 1,28, 1,141,
	42,0, 1,85, 1,255, 1,85, 4,0, 1,198, 1,255, 1,113,
	3,0, 1,85, 1,255, 1,85, 4,0, 1,170, 1,255, 8,0,
	1,85, 1,255, 1,85, 11,0, 1,255, 1,170, 7,0, 1,141,
	1,255, 1,198, 1,56, 1,0, 1,28, 1,141, 2,255, 1,170,
	7,0, 1,85, 1,255, 1,85, 7,0, 1,85, 2,255, 1,85,
	2,0, 1,56, 1,255, 1,170, 4,0, 1,85, 1,255, 1,226,
	1,56, 1,0, 1,56, 1,198, 2,255, 16,0, 1,28, 1,255,
	1,226, 13,0, 1,85, 1,255, 1,85, 23,0, 1,170, 1,255,
	1,0, 2,198, 2,0, 1,85, 1,255, 1,85, 3,0, 1,170,
	1,255, 1,170, 4,0, 1,170, 1,255, 1,170, 3,0, 1,56,
	1,255, 1,226, 1,113, 2,0, 1,113, 1,226, 2,255, 3,0,
	1,28, 1,226, 1,255, 1,85, 3,0, 1,28, 1,226, 1,255,
	1,56, 3,0, 1,141, 1,255, 1,85, 3,0, 1,85, 1,226,
	1,255, 42,0, 1,85, 1,255, 1,85, 4,0, 1,85, 1,255,
	1,170, 2,0, 1,141, 2,255, 1,85, 2,0, 1,226, 3,255,
	6,0, 4,255, 1,85, 11,0, 1,255, 1,170, 8,0, 1,113,
	1,226, 4,255, 1,113, 1,255, 1,170, 5,0, 4,255, 1,85,
	6,0, 1,141, 2,255, 1,198, 1,255, 1,198, 1,170, 2,255,
	1,28, 5,0, 1,85, 1,226, 3,255, 2,198, 2,255, 1,56,
	15,0, 1,198, 1,255, 1,56, 6,0, 1,141, 1,85, 4,0,
	1,85, 1,255, 1,85, 23,0, 1,170, 1,255, 1,113, 1,255,
	1,56, 2,0, 1,85, 1,255, 1,85, 4,0, 1,170, 2,255,
	2,170, 2,255, 1,170, 5,0, 1,56, 1,198, 4,255, 1,170,
	1,198, 1,255, 4,0, 1,28, 1,226, 1,255, 1,226, 1,170,
	1,198, 1,255, 1,226, 1,85, 4,0, 1,28, 1,226, 1,255,
	1,198, 1,170, 1,198, 3,255, 42,0, 1,85, 1,255, 1,85,
	4,0, 1,113, 1,255, 1,141, 2,0, 1,28, 2,85, 1,28,
	2,0, 1,56, 2,85, 1,56, 6,0, 4,85, 1,28, 11,0,
	1,255, 1,170, 10,0, 1,56, 2,85, 2,0, 1,255, 1,170,
	5,0, 4,85, 1,28, 6,0, 1,28, 2,85, 1,28, 1,56,
	1,141, 1,170, 1,141, 1,28, 8,0, 2,85, 1,28, 1,0,
	1,56, 2,85, 16,0, 1,56, 1,255, 1,198, 5,0, 1,113,
	1,255, 1,170, 4,0, 1,85, 1,255, 1,85, 23,0, 1,170,
	1,255, 1,226, 1,141, 3,0, 1,85, 1,255, 1,85, 5,0,
	1,56, 1,113, 2,170, 1,113, 1,56, 8,0, 1,28, 2,85,
	1,28, 1,0, 1,170, 1,255, 6,0, 1,85, 1,141, 2,170,
	1,85, 8,0, 1,85, 3,170, 1,85, 1,113, 1,141, 41,0,
	1,56, 1,141, 1,255, 1,141, 3,85, 1,141, 1,255, 1,226,
	1,28, 34,0, 1,255, 1,170, 15,0, 1,255, 1,170, 57,0,
	1,141, 1,255, 1,198, 1,56, 2,0, 1,113, 2,255, 1,170,
	2,0, 1,56, 1,85, 1,141, 1,255, 1,141, 2,85, 20,0,
	1,85, 1,198, 2,255, 1,28, 1,0, 1,28, 1,85, 1,141,
	1,255, 1,141, 1,28, 23,0, 1,170, 1,255, 67,0, 1,226,
	6,255, 1,226, 1,141, 1,28, 21,0, 1,85, 1,170, 1,85,
	11,0, 1,255, 1,170, 15,0, 1,255, 1,170, 7,0, 1,85,
	1,170, 1,85, 48,0, 1,85, 1,226, 4,255, 1,226, 1,255,
	1,170, 2,0, 1,226, 6,255, 1,56, 18,0, 1,56, 3,255,
	1,141, 2,0, 1,141, 4,255, 1,141, 23,0, 1,170, 1,255,
	98,0, 1,170, 1,255, 1,170, 7,0, 1,85, 4,255, 1,170,
	13,0, 1,226, 2,255, 1,170, 7,0, 1,170, 1,255, 1,170,
	50,0, 1,56, 2,85, 1,28, 1,0, 1,56, 63,0, 1,141,
	3,255, 98,0, 1,141, 1,255, 1,141, 7,0, 1,28, 4,85,
	1,56, 13,0, 1,56, 2,85, 1,56, 7,0, 1,141, 1,255,
	1,141, 119,0, 1,28, 3,85, 67,0,

	0,0  // THE END
};

static const int beta_w = 176;
static const int beta_h = 22;

static const byte beta_data[] =
{
	125,0, 1,56, 1,85, 1,56, 30,0, 1,28, 1,141,
	3,170, 1,113, 1,56, 13,0, 1,113, 11,255, 1,198,
	1,141, 1,28, 4,0, 1,56, 1,226, 14,255, 1,85,
	5,0, 1,198, 11,255, 1,113, 3,0, 1,198, 5,255,
	1,226, 1,85, 3,0, 1,198, 6,255, 1,198, 1,28,
	22,0, 1,141, 11,255, 1,141, 10,0, 1,170, 3,255,
	1,170, 9,0, 1,85, 1,226, 11,255, 1,226, 6,0,
	1,170, 6,255, 1,226, 1,113, 11,0, 15,255, 1,85,
	3,0, 1,170, 15,255, 1,170, 4,0, 1,85, 13,255,
	2,0, 1,85, 7,255, 1,170, 2,0, 1,85, 8,255,
	1,85, 22,0, 13,255, 9,0, 1,113, 5,255, 1,113,
	8,0, 1,170, 13,255, 1,28, 5,0, 1,85, 8,255,
	1,198, 1,28, 9,0, 1,198, 15,255, 1,56, 2,0,
	1,113, 15,255, 1,170, 4,0, 1,56, 12,255, 1,226,
	2,0, 1,28, 7,255, 1,141, 2,0, 1,28, 8,255,
	1,28, 22,0, 1,226, 11,255, 1,226, 9,0, 1,170,
	5,255, 1,170, 8,0, 1,113, 13,255, 1,85, 8,0,
	1,56, 1,85, 1,141, 4,255, 1,226, 1,28, 9,0,
	1,85, 4,255, 5,85, 1,198, 4,255, 1,170, 3,0,
	1,56, 1,198, 3,255, 1,141, 6,85, 1,198, 2,255,
	1,170, 5,0, 1,28, 3,85, 1,198, 3,255, 1,141,
	3,85, 4,0, 1,28, 1,85, 3,255, 1,141, 1,85,
	4,0, 1,28, 1,85, 1,226, 3,255, 1,141, 1,28,
	24,0, 4,85, 3,255, 1,198, 3,85, 10,0, 1,141,
	5,255, 1,141, 9,0, 1,170, 3,255, 1,226, 1,113,
	4,85, 3,255, 1,85, 11,0, 1,28, 1,226, 3,255,
	1,198, 10,0, 4,255, 6,0, 1,170, 4,255, 4,0,
	1,170, 3,255, 1,85, 6,0, 1,170, 2,255, 1,170,
	9,0, 1,170, 3,255, 1,85, 9,0, 1,198, 2,255,
	1,170, 6,0, 1,28, 3,255, 1,226, 30,0, 3,255,
	1,170, 13,0, 1,28, 1,226, 3,255, 1,226, 1,28,
	10,0, 1,170, 3,255, 1,226, 1,28, 3,0, 1,113,
	1,255, 1,226, 1,28, 12,0, 1,28, 1,226, 3,255,
	1,113, 9,0, 4,255, 6,0, 1,85, 4,255, 4,0,
	1,170, 3,255, 1,85, 6,0, 1,170, 2,255, 1,170,
	9,0, 1,170, 3,255, 1,85, 9,0, 1,113, 3,255,
	6,170, 1,198, 3,255, 1,141, 30,0, 3,255, 1,170,
	14,0, 1,28, 1,141, 1,170, 1,141, 1,28, 12,0,
	1,170, 3,255, 1,226, 1,28, 19,0, 1,113, 3,255,
	1,226, 9,0, 4,255, 6,0, 1,113, 3,255, 1,226,
	4,0, 1,170, 3,255, 1,85, 2,0, 1,198, 1,255,
	1,113, 1,0, 1,85, 2,255, 1,85, 9,0, 1,170,
	3,255, 1,85, 9,0, 1,28, 13,255, 1,56, 30,0,
	3,255, 1,170, 32,0, 1,170, 3,255, 1,226, 1,56,
	19,0, 1,226, 3,255, 1,85, 8,0, 4,255, 4,0,
	1,28, 1,113, 4,255, 1,113, 4,0, 1,170, 3,255,
	1,85, 1,0, 1,85, 3,255, 14,0, 1,170, 3,255,
	1,85, 10,0, 1,198, 11,255, 1,226, 31,0, 3,255,
	1,170, 33,0, 1,170, 4,255, 1,85, 12,0, 1,113,
	1,226, 2,255, 1,170, 1,56, 1,141, 3,255, 1,141,
	8,0, 13,255, 1,170, 5,0, 1,170, 9,255, 14,0,
	1,170, 3,255, 1,85, 10,0, 1,85, 3,255, 4,85,
	1,198, 3,255, 1,113, 31,0, 3,255, 1,170, 34,0,
	1,170, 4,255, 1,85, 9,0, 1,28, 1,226, 6,255,
	1,170, 3,255, 1,170, 8,0, 12,255, 1,56, 6,0,
	1,170, 9,255, 8,0, 1,28, 1,85, 4,0, 1,170,
	3,255, 1,85, 3,0, 1,28, 1,85, 1,28, 5,0,
	3,255, 1,85, 3,0, 1,226, 3,255, 1,28, 31,0,
	3,255, 1,170, 35,0, 1,170, 4,255, 1,56, 8,0,
	1,170, 12,255, 8,0, 12,255, 1,170, 6,0, 1,170,
	9,255, 7,0, 1,28, 2,255, 1,170, 3,0, 1,170,
	3,255, 1,85, 3,0, 1,226, 1,255, 1,198, 5,0,
	1,170, 2,255, 1,170, 2,0, 1,56, 3,255, 1,198,
	32,0, 3,255, 1,170, 36,0, 1,226, 3,255, 1,226,
	7,0, 1,56, 4,255, 1,198, 1,85, 1,113, 1,226,
	5,255, 8,0, 4,255, 3,0, 1,28, 1,113, 4,255,
	1,85, 5,0, 1,170, 3,255, 1,85, 1,0, 1,85,
	3,255, 1,0, 1,28, 2,141, 1,28, 2,0, 1,85,
	3,255, 3,0, 1,170, 3,255, 1,85, 2,0, 1,85,
	3,255, 5,0, 1,85, 3,255, 1,28, 1,0, 1,141,
	3,255, 1,113, 32,0, 3,255, 1,170, 36,0, 1,28,
	4,255, 1,113, 6,0, 1,170, 3,255, 1,170, 3,0,
	1,28, 1,226, 4,255, 8,0, 4,255, 5,0, 1,198,
	3,255, 1,170, 5,0, 1,170, 3,255, 1,85, 1,0,
	1,28, 1,226, 1,255, 1,141, 1,0, 1,141, 2,255,
	1,141, 2,0, 1,85, 3,255, 3,0, 1,170, 3,255,
	1,85, 2,0, 1,85, 3,255, 6,0, 3,255, 1,113,
	1,0, 1,226, 3,255, 33,0, 3,255, 1,170, 37,0,
	1,170, 3,255, 1,170, 6,0, 4,255, 1,56, 4,0,
	1,141, 4,255, 8,0, 4,255, 5,0, 1,170, 3,255,
	1,170, 5,0, 1,170, 3,255, 1,85, 6,0, 1,170,
	2,255, 1,170, 2,0, 1,85, 3,255, 3,0, 1,170,
	3,255, 1,85, 2,0, 1,85, 3,255, 6,0, 1,141,
	2,255, 1,198, 1,56, 3,255, 1,170, 33,0, 3,255,
	1,170, 37,0, 1,85, 4,255, 6,0, 4,255, 5,0,
	1,85, 3,255, 1,198, 8,0, 4,255, 4,0, 1,85,
	4,255, 1,141, 5,0, 1,170, 3,255, 1,85, 6,0,
	1,170, 2,255, 1,170, 2,0, 1,85, 3,255, 3,0,
	1,170, 3,255, 1,85, 2,0, 1,85, 3,255, 6,0,
	1,56, 3,255, 1,170, 3,255, 1,85, 33,0, 3,255,
	1,170, 29,0, 1,141, 1,255, 1,170, 5,0, 1,85,
	4,255, 6,0, 4,255, 5,0, 1,85, 3,255, 1,170,
	6,0, 1,113, 14,255, 1,56, 3,0, 1,56, 1,226,
	14,255, 1,170, 2,0, 1,85, 17,255, 4,0, 1,198,
	8,255, 1,226, 34,0, 3,255, 1,170, 29,0, 3,255,
	1,56, 4,0, 1,141, 3,255, 1,226, 6,0, 1,198,
	3,255, 1,85, 4,0, 1,170, 3,255, 1,85, 6,0,
	14,255, 1,170, 4,0, 1,170, 15,255, 1,170, 2,0,
	1,56, 16,255, 1,226, 3,0, 1,85, 9,255, 1,141,
	29,0, 1,28, 1,226, 1,255, 1,170, 1,113, 3,255,
	1,170, 29,0, 3,255, 1,226, 1,85, 2,0, 1,113,
	4,255, 1,170, 6,0, 1,113, 3,255, 1,226, 1,28,
	2,0, 1,85, 3,255, 1,226, 7,0, 1,226, 11,255,
	1,198, 1,85, 5,0, 1,113, 15,255, 1,141, 3,0,
	16,255, 1,170, 3,0, 1,28, 9,255, 1,56, 29,0,
	1,85, 7,255, 1,170, 29,0, 1,198, 11,255, 1,56,
	7,0, 1,226, 4,255, 1,170, 1,198, 4,255, 1,85,
	8,0, 10,85, 9,0, 1,56, 14,85, 4,0, 1,56,
	15,85, 1,28, 4,0, 1,28, 7,85, 1,56, 31,0,
	1,198, 6,255, 1,170, 29,0, 1,28, 1,226, 9,255,
	1,85, 8,0, 1,28, 1,226, 8,255, 1,170, 109,0,
	1,28, 1,85, 1,170, 1,226, 2,255, 1,141, 31,0,
	1,113, 1,198, 5,255, 1,198, 1,85, 10,0, 1,28,
	1,141, 5,255, 1,226, 1,85, 114,0, 1,28, 1,85,
	1,28, 34,0, 3,85, 1,28, 15,0, 1,56, 2,85,
	1,56, 9,0,

	0,0  // THE END
};

//----------------------------------------------------------------------------

static byte *Unpacker(const byte *src, int w, int h)
{
	byte *buffer = new byte[w * h];
	byte *dest = buffer;

	for (; *src; src += 2)
	{
		for (int j=0; j < src[0]; j++)
			*dest++ = src[1];
	}

	SYS_ASSERT((dest - buffer) == (w * h));

	return buffer;
}

const byte *RGL_LogoImage(int *w, int *h)
{
	static byte *unpacked_logo = NULL;

	if (! unpacked_logo)
		unpacked_logo = Unpacker(logo_data, logo_w, logo_h);

	*w = logo_w;
	*h = logo_h;

	return unpacked_logo;
}

const byte *RGL_InitImage(int *w, int *h)
{
	static byte *unpacked_logo = NULL;

	if (! unpacked_logo)
		unpacked_logo = Unpacker(init_data, init_w, init_h);

	*w = init_w;
	*h = init_h;

	return unpacked_logo;
}

const byte *RGL_GlbspImage(int *w, int *h)
{
	static byte *unpacked_logo = NULL;

	if (! unpacked_logo)
		unpacked_logo = Unpacker(glbsp_data, glbsp_w, glbsp_h);

	*w = glbsp_w;
	*h = glbsp_h;

	return unpacked_logo;
}

const byte *RGL_BuildImage(int *w, int *h)
{
	static byte *unpacked_logo = NULL;

	if (! unpacked_logo)
		unpacked_logo = Unpacker(build_data, build_w, build_h);

	*w = build_w;
	*h = build_h;

	return unpacked_logo;
}

const byte *RGL_BetaImage(int *w, int *h)
{
	static byte *unpacked_logo = NULL;

	if (! unpacked_logo)
		unpacked_logo = Unpacker(beta_data, beta_w, beta_h);

	*w = beta_w;
	*h = beta_h;

	return unpacked_logo;
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
