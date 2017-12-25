#include "xtime.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static struct timeval  xtv ;
void xtimer_start ()
{
  gettimeofday ( &xtv , NULL ) ;
}
float xtimer_end ( char * string )
{
  struct timeval  xtv2 ;
  int sec , usec ;
  float gap ;
  
  gettimeofday ( &xtv2 , NULL ) ;
  sec = (int)(xtv2.tv_sec - xtv.tv_sec) ;
  usec = (int)(xtv2.tv_usec - xtv.tv_usec) ;
  if ( usec < 0 )
  {
    sec -- ;
    usec += 1000 * 1000 ;
  }
  
  gap = (float)(sec + ((float)usec / 1000000)) ;
  printf("%s : %f second\n" , string ? string : "time used" , gap) ;
  
  return gap ;
}
int xgetminsecond ()
{
  struct timeval timev ;
  
  gettimeofday(&timev, NULL) ;
  timev.tv_sec %= 24 * 3600 ;//24小时重置一次时间戳
  return (int)((timev.tv_sec * 1000) + (timev.tv_usec / 1000)) ;
}
int xgetimestr ( char time_str[BUFFER_SIZE] , int exact , int nice )
{
  time_t timep ;
  int ret = 0 ;
  
  memset ( time_str , 0, BUFFER_SIZE ) ;
  time ( &timep) ;
  if ( exact )
  {
    if ( nice )
      ret = (int)strftime ( time_str , BUFFER_SIZE - 1 ,
                           "%Y-%m-%d %H:%M:%S", localtime(&timep) ) ;
    else
      ret = (int)strftime ( time_str , BUFFER_SIZE - 1 ,
                           "%Y-%m-%d-%H-%M-%S", localtime(&timep) ) ;
  }
  else
    ret = (int)strftime ( time_str , BUFFER_SIZE - 1 ,
                         "%Y-%m-%d", localtime(&timep) ) ;
  
  XXNULL ( ret , 0 ) ;
  return 1 ;
}
#ifdef WIN32
int
gettimeofday(struct timeval *tp, void *tzp)
{
  time_t clock;
  struct tm tm;
  SYSTEMTIME wtm;
  
  if ( !tp ) return -1 ;
  
  GetLocalTime(&wtm);
  tm.tm_year     = wtm.wYear - 1900;
  tm.tm_mon     = wtm.wMonth - 1;
  tm.tm_mday     = wtm.wDay;
  tm.tm_hour     = wtm.wHour;
  tm.tm_min     = wtm.wMinute;
  tm.tm_sec     = wtm.wSecond;
  tm. tm_isdst    = -1;
  clock = mktime(&tm);
  tp->tv_sec = (long)clock;
  tp->tv_usec = wtm.wMilliseconds * 1000;
  
  return (0);
}
#endif

static char randombuffer[129] ;
char * xgen_random ()
{
  static struct timeval tv_old ;
  struct timeval tv ;
  
  do
    gettimeofday (&tv,null) ;
  while ( tv_old.tv_sec == tv.tv_sec && tv_old.tv_usec == tv.tv_usec  ) ;
  
  tv_old = tv ;

  sprintf(randombuffer, "%d%d" , (int)tv.tv_sec, (int)tv.tv_usec) ;
  return randombuffer ;
}
void xsleep( int ms_time )
{
#ifndef WIN32
  int ret = 0 ;
  struct timeval tv ;
  
  tv.tv_sec = ms_time / 1000 ;
  tv.tv_usec = ( ms_time % 1000 ) * 1000 ;
  
__select_call:
  ret = select ( 1 , NULL , NULL , NULL , &tv ) ;
  if ( ret < 0 )
  {
    if ( EINTR == errno ) goto __select_call ;
  }
  
#else
  Sleep ( ms_time ) ;
#endif
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
