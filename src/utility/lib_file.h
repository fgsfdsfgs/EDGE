//------------------------------------------------------------------------
//  File Utilities
//------------------------------------------------------------------------
//
//  Eureka DOOM Editor
//
//  Copyright (C) 2006-2012 Andrew Apted
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

#ifndef __LIB_FILE_H__
#define __LIB_FILE_H__

#include <functional>

#ifdef WIN32
#define DIR_SEP_CH   '\\'
#define DIR_SEP_STR  "\\"
#else
#define DIR_SEP_CH   '/'
#define DIR_SEP_STR  "/"
#endif

// filename functions
bool HasExtension(const char *filename);
bool MatchExtension(const char *filename, const char *ext);
SString ReplaceExtension(const char *filename, const char *ext);
const char *FindBaseName(const char *filename);
bool FilenameIsBare(const char *filename);
SString FilenameReposition(const char *filename, const char *othername);
SString FilenameGetPath(const char *filename);

// file utilities
bool FileExists(const char *filename);
bool FileCopy(const char *src_name, const char *dest_name);
bool FileRename(const char *old_name, const char *new_name);
bool FileDelete(const char *filename);
bool FileChangeDir(const char *dir_name);
bool FileMakeDir(const char *dir_name);

u8_t *FileLoad(const char *filename, int *length);
void FileFree(u8_t *mem);

// miscellanous
SString GetExecutablePath(const char *argv0);

//------------------------------------------------------------------------

// directory functions
bool PathIsDirectory(const char *path);

enum scan_flags_e
{
	SCAN_F_IsDir    = (1 << 0),
	SCAN_F_Hidden   = (1 << 1),
	SCAN_F_ReadOnly = (1 << 2),
};

enum scan_error_e
{
	SCAN_ERROR = -1,  // general catch-all

	SCAN_ERR_NoExist  = -2,  // could not find given path
	SCAN_ERR_NotDir   = -3,  // path was not a directory
};

typedef void (* directory_iter_f)(const char *name, int flags, void *priv_dat);

int ScanDirectory(const char *path, directory_iter_f func, void *priv_dat);
int ScanDirectory(const char *path, const std::function<void(const char *, int)> &func);
// scan the directory with the given path and call the given
// function (passing the private data pointer to it) for each
// entry in the directory.  Returns the total number of entries,
// or a negative value on error (SCAN_ERR_xx value).

#endif /* __LIB_FILE_H__ */

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
