//------------------------------------------------------------------------
//  UTILITIES
//------------------------------------------------------------------------
// 
//  DEH_EDGE  Copyright (C) 2004-2005  The EDGE Team
// 
//  This program is under the GNU General Public License.
//  It comes WITHOUT ANY WARRANTY of any kind.
//  See COPYING.txt for the full details.
//
//------------------------------------------------------------------------

#ifndef __UTIL_HDR__
#define __UTIL_HDR__

#include <stdio.h>

/// `CA- 6.5.2016: quick hacks to change these in Visual Studio (less warnings). 
#ifdef _MSC_VER
#define strdup _strdup
#define stricmp _stricmp
#define strnicmp _strnicmp
#endif


namespace Deh_Edge
{

// file utilities
bool FileExists(const char *filename);
bool CheckExtension(const char *filename, const char *ext);
const char *ReplaceExtension(const char *filename, const char *ext);
const char *FileBaseName(const char *filename);
bool FileIsBinary(FILE *fp);

// string utilities
int StrCaseCmp(const char *A, const char *B);
int StrCaseCmpPartial(const char *A, const char *B);
void StrMaxCopy(char *dest, const char *src, int max);
const char *StrUpper(const char *name);
char *StringNew(int length);
char *StringDup(const char *orig);

}  // Deh_Edge

#endif /* __UTIL_HDR__ */
