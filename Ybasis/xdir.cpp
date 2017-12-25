#include "xdir.h"
///////////////////////////////////////////path///////////////////////////////////////////////////////
int changeGapchar_Win2Linux ( char * path1 , int len1 , char * path2 , int size2 , int * rlen2 )
{
  int i = 0 ;
  int len = 0 ;
  
  if ( !path1 || !path1[0] || len1 <= 0 || !path2 || size2 <= 0 ) return 0 ;
  
  len = xstrlen ( path1 ) ;
  len1 = len < len1 ? len : len1 ;
  
  if ( size2 < len1 + 1 ) return -1 ;
  
  for ( i = 0 ; i < len1 ; i ++ )
    path2 [ i ] = path1 [ i ] == '\\' ? '/' : path1 [ i ] ;
  
  path2 [ i ] = 0 ;
  if ( rlen2 ) *rlen2 = i ;
  
  return 1 ;
}
void catGapchar ( char * path )
{
  int i = 0 ;
  
  i = xstrlen ( path ) ;
  if ( ( path [ i - 1 ] != '/' ) && ( path [ i - 1 ] != '\\' ) )
  {
    path [ i ++ ] = '/' ;
    path [ i ++ ] = 0 ;
  }
}

XDIR * xdir_open ( char * path )
{
  XDIR * ret = NULL ;
  
  int len = 0 ;
  char xpath [ BUFFER_SIZE ] ;
  
#ifndef WIN32
#else
  WIN32_FIND_DATA FindFileData;
#endif
  
  len = xstrlen ( path ) ;
  if ( len <= 0 || len >= ( BUFFER_SIZE / 2 ) ) return NULL ;
  
  changeGapchar_Win2Linux ( path , xstrlen ( path ) , xpath , BUFFER_SIZE - 1 , &len ) ;
  catGapchar ( xpath ) ;
  
  ret = ( XDIR * ) xmalloc ( sizeof (XDIR) ) ;
  strncpy ( ret ->path , xpath , BUFFER_SIZE - 1 ) ;
  
#ifndef WIN32
  ret ->dir = opendir ( xpath ) ;
  if ( !ret ->dir )
  {
    xfree ( ret ) ;
    return NULL ;
  }
#else
  strncat ( xpath , "*.*" , BUFFER_SIZE - 1 ) ;
  
  ret ->dir = FindFirstFile ( xpath , &FindFileData ) ;
  if ( INVALID_HANDLE_VALUE == ret ->dir )
  {
    xfree ( ret ) ;
    return NULL ;
  }
  
  changeGapchar_Win2Linux ( FindFileData.cFileName , xstrlen ( FindFileData.cFileName ) ,
                           ret ->filename , BUFFER_SIZE - 1 , &len ) ;
  
  if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
  {
    catGapchar ( ret ->filename ) ;
    ret ->isdir = 1 ;
  }
#endif
  
  return ret ;
}

char * xdir_find ( XDIR * dir )
{
  char * ret = NULL ;
  
#ifndef WIN32
  struct dirent * file = NULL ;
  struct stat statbuf ;
  char filename2 [ BUFFER_SIZE * 2 ] ;
#else
  WIN32_FIND_DATA FindFileData;
#endif
  
  XXNULL ( dir , ret ) ;
  XXNULL ( dir ->dir , ret	 ) ;
  
  dir ->isdir = 0 ;
  memset ( dir ->filename , 0 , BUFFER_SIZE ) ;
  
#ifndef WIN32
  file = readdir ( dir ->dir ) ;
  if ( !file )
  {
    return ret ;
  }
  
  strncpy ( dir ->filename , file ->d_name , BUFFER_SIZE - 1 ) ;
  
  strncpy ( filename2 , dir ->path , BUFFER_SIZE - 1 ) ;
  strncat ( filename2 , file ->d_name , BUFFER_SIZE - 1 ) ;
  
  lstat ( filename2 , &statbuf ) ;
  if ( S_ISDIR( statbuf.st_mode ) )
  {
    catGapchar ( dir ->filename ) ;
    dir ->isdir = 1 ;
  }
#else
  if ( !FindNextFile( dir ->dir , &FindFileData ) )
  {
    return ret ;
  }
  
  strncpy ( dir ->filename , FindFileData.cFileName , BUFFER_SIZE - 1 ) ;
  if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
  {
    catGapchar ( dir ->filename ) ;
    dir ->isdir = 1 ;
  }
#endif
  
  ret = dir ->filename ;
  return ret ;
}
void xdir_close ( XDIR ** dir )
{
  XNULL ( *dir ) ;
  XNULL ( (*dir) ->dir ) ;
  
#ifndef WIN32
  closedir ( (*dir) ->dir ) ;
#else
  FindClose ( (*dir) ->dir ) ;
#endif
  xfree ( *dir ) ;
  *dir = NULL ;
}
