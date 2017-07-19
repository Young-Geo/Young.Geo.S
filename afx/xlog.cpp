#include "xlog.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/* log */
static char xlogfile[ BUFFER_SIZE ] ;
static int xlogfilelen = 0 ;
static int xlevel = 0 ;

int xlog_printstdout = 1 ;

void getself ( char * buffer , int size )
{
  int ret = 0 ;
  
  if ( !buffer || size <= 0 ) return ;
  memset ( buffer , 0 , size ) ;
  
#ifdef WIN32
  ret = GetModuleFileName ( NULL , buffer , size - 1 ) ;
#else
  #ifdef MACOSX
  ret = 0 ;
  #else
  ret = readlink ("/proc/self/exe", buffer, size - 1 ) ;
  #endif
#endif
  
  if ( ret <= 0 )
  {
    strncpy ( buffer , "./exe" , size - 1 ) ;
  }
    
  return ;
}
void getworkspace ( char * buffer , int size )
{
  char * p = NULL ;
  
  getself ( buffer , size - 5 ) ;
  p = strrchr ( buffer , '/' ) ;
  if ( !p )
  {
    p = strrchr ( buffer , '\\' ) ;
  }
  if ( p ) *p = 0 ;
}
void loginit ( int level , char * logfile )
{
  xlevel = level ;
  
  if ( logfile && logfile[0] )
  {
    strcpy ( xlogfile , logfile ) ;
  }
  else
  {
    getworkspace ( xlogfile , BUFFER_SIZE - 5 ) ;
    strcat ( xlogfile , "/run.log" ) ;
  }
  xlogfilelen = xstrlen ( xlogfile ) ;
}
char msg_buf[XLOGBUFFERSIZE] ;
void logwrite(int level, const char* fmt, ...)//print log
{
  int prefixlen = 0 ;
  char time[BUFFER_SIZE] ;
  int i = 0 ;
  
  FILE * pfile = NULL ;
  va_list ap;
  
  if ( !(level & xlevel) )
  {
    if ( !xlog_printstdout ) return ;
  }
  if (!fmt || !fmt[0] ) return ;
  
  if ( !xlogfilelen )
  {
    strcpy ( xlogfile , "/run.log" ) ;
    xlogfilelen = xstrlen ( xlogfile ) ;
  }
  
  pfile = fopen ( xlogfile , "a+" ) ;
  if ( !pfile )
  {
    if ( !xlog_printstdout ) return ;
  }
  xgetimestr ( time , 1 , 1 ) ;
  
  i = 0 ;
  if ( level & XVERR )
  {
    strcpy ( msg_buf + i , "[Err" ) ;
    i += 4 ;//4 is [Err's len
  }
  if ( level & XVLOG )
  {
    if ( i ) msg_buf[ i++ ] = '|' ;
    else msg_buf[ i++ ] = '[' ;
    
    strcpy( msg_buf + i , "Log" ) ;
    i += 3 ;//3 is Log's len
  }
  if ( level & XVDBG )
  {
    if ( i ) msg_buf[ i++ ] = '|' ;
    else msg_buf[ i++ ] = '[' ;
    
    strcpy( msg_buf + i , "Dbg" ) ;
    i += 3 ;//3 is Dbg's len
  }
  msg_buf[ i++ ] = ']' ;
  msg_buf[ i++ ] = '[' ;

  strcpy( msg_buf + i , time ) ;
  i += 19 ;//19 is time's len
  
  msg_buf[ i++ ] = ']' ;
  msg_buf[ i++ ] = ':' ;
  
  prefixlen = i ;
  va_start(ap, fmt) ;
  vsnprintf ( msg_buf + prefixlen , XLOGBUFFERSIZE - prefixlen - 2 , fmt , ap ) ;
  va_end(ap) ;
  
  i = xstrlen(msg_buf) ;
  if ( msg_buf[i - 1] != '\n' )
  {
    msg_buf[i ++] ='\n' ;
    msg_buf[i] = 0 ;
  }
  
  if ( pfile )
  {
    fwrite ( msg_buf , 1 , i , pfile ) ;
    fclose ( pfile ) ;
  }
  if ( xlog_printstdout )
  {  
    printf ( "%s" , msg_buf ) ;
  }
  return ;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
