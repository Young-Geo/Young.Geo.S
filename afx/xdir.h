//////////////////////////////////////////////////////////////////////////
#ifndef __H_XDIR_H__
#define __H_XDIR_H__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/* include */
#include "afx.h"
/* macro */
#ifndef WIN32
#include <dirent.h>
#include <sys/stat.h>
#endif

typedef struct _XDIR
{
#ifndef WIN32
  DIR * dir ;
#else
  HANDLE dir ;
#endif
  char path [ BUFFER_SIZE ] ;
  char filename [ BUFFER_SIZE ] ;
  int isdir ;
} XDIR ;

int changeGapchar_Win2Linux ( char * path1 , int len1 , char * path2 , int size2 , int * rlen2 ) ;
void catGapchar ( char * path ) ;
XDIR * xdir_open ( char * path ) ;
char * xdir_find ( XDIR * dir ) ;
void xdir_close ( XDIR ** dir ) ;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////