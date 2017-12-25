#include "afx.h"
////////////////////////////////////////////////////////////////////////////
#define MEM_LEAK_TEST 0
#if MEM_LEAK_TEST
static int memorycount = 0 ;
#endif
////////////////////////////////////////////////////////////////////////////
void * saferealloc( void * oldmen , unsigned int size)
{
  void *p = NULL ;
  unsigned int tmp ;
  
  if (size >= XINT_MAX )p = NULL;
  else
  {
    tmp = size % (sizeof(void*)) ;
    tmp = tmp ? ( 2 * ( sizeof ( void * ) ) - tmp ) : ( sizeof ( void * ) ) ;
    size += tmp ;
    p = realloc ( oldmen , size ) ;
    
#if MEM_LEAK_TEST
    if ( !oldmen )
    {
      memorycount ++ ;
      printf("memcount : %05d\n" , memorycount) ;
    }
#endif
    
    memset (((unsigned char *)p) + (size - tmp) , 0 , tmp) ;
  }
  return p ;
}
void * safemalloc( unsigned int size )
{
  void * p = NULL ;
  unsigned int tmp ;
  
  if ( size >= XINT_MAX ) p = NULL ;
  else
  {
    tmp = size % ( sizeof ( void * ) ) ;
    size += tmp ? ( 2 * ( sizeof ( void * ) ) - tmp ) : ( sizeof ( void * ) ) ;
    p = malloc ( size ) ;

#if MEM_LEAK_TEST
    memorycount ++ ;
    printf("memcount : %05d\n" , memorycount) ;
#endif

    if ( p ) memset ( p , 0 , size ) ;
  }
  return p;
}
////////////////////////////////////////////////////////////////////////////
void safefree ( void ** ptr )
{
  if ( (!ptr) || (!(*ptr)) ) return ;
  free ( *ptr ) ;
  
#if MEM_LEAK_TEST
  memorycount -- ;
  printf("memcount : %05d\n" , memorycount) ;
#endif
  
  *ptr = NULL ;
}
////////////////////////////////////////////////////////////////////////////
float stov ( char * s )
{
  int nagative = 0 ;
  float v = 0 , dev = 10 ;
  char c ;
  
  if ( *s == '-' )
  {
    s ++ ;
    nagative = 1 ;
  }
  while ((c = *s) != 0)
  {
    if ( c == '.' )
    {
      s ++ ;
      break ;
    }
    if ( c < '0' || c > '9' )
    {
      break ;
    }
    
    v *= 10 ;
    v += c - '0' ;
    
    s ++ ;
  }
  while ((c = *s) != 0)
  {
    if ( c < '0' || c > '9' )
    {
      break ;
    }
    
    v += (c - '0') / dev ;
    dev *= 10 ;
    
    s ++ ;
  }
  if ( nagative )
  {
    v = -v ;
  }
  return v ;
}
int hexstring2hex ( char * s )
{
  char * hex = NULL , c ;
  int active = 0 , count = 0 ;
  
  for ( ; (c = * s) != 0 ; s ++ )
  {
    if (c >= '0' && c <= '9')
    {
      c -= '0' ;
    }
    else if (c >= 'A' && c <= 'F')
    {
      c = 10 + c - 'A' ;
    }
    else if (c >= 'a' && c <= 'f')
    {
      c = 10 + c - 'a' ;
    }
    else
    {
      if ( active )
      {
        active = 0 ;
        hex ++ ;
      }
      continue ;
    }
    
    if ( !active )
    {
      active = 1 ;
    }
    
    if ( !hex )
    {
      hex = s ;
    }
    
    if ( active == 1 )
    {
      *hex = c ;
      count ++ ;
    }
    else
    {
      *hex *= 16 ;
      *hex += c ;
    }
    
    active ++ ;
    if ( active > 2 )
    {
      hex ++ ;
      active = 0 ;
    }
  }
  
  return count ;
}
////////////////////////////////////////////////////////////////////////////
void hexdump(unsigned char *p, unsigned int len , char * tofile , char * flag )
{
  unsigned char *line = p;
  unsigned int i, thisline, offset = 0;
  
  char lb[BUFFER_SIZE] ;
  int lbi = 0 ;
  FILE * pfile = NULL ;
  
  if ( tofile ) pfile = fopen ( tofile , "a+" ) ;
  
  sprintf ( lb , "---------------------------------------------------------------------[%d]" , len ) ;
  if ( flag ) strcat ( lb , flag ) ;
  strcat ( lb , "\n" ) ;
  if ( pfile ) fwrite ( lb , sizeof ( char ) , (int)strlen ( lb ) , pfile ) ;
  else printf ( "%s" , lb ) ;
  
  while (offset < len)
  {
    memset ( lb, 0, sizeof(lb) ) ;
    lbi = 0 ;
    
    sprintf(lb + lbi ,"%04x ", offset);
    lbi += strlen ( lb + lbi ) ;
    
    thisline = len - offset;
    if (thisline > 16)
      thisline = 16;
    
    for (i = 0; i < thisline; i++)
    {
      sprintf(lb + lbi ,"%02x ", line[i]);
      lbi += strlen ( lb + lbi ) ;
    }
    
    for (; i < 16; i++)
    {
      sprintf(lb + lbi ,"   ");
      lbi += strlen ( lb + lbi ) ;
    }
    
    for (i = 0; i < thisline; i++)
    {
      sprintf(lb + lbi ,"%c", (line[i] >= 0x20 && line[i] < 0x7f) ? line[i] : '.');
      lbi += strlen ( lb + lbi ) ;
    }
    
    strcat ( lb , "\n" ) ;
    if ( pfile ) fwrite ( lb , sizeof ( char ) , strlen ( lb ) , pfile ) ;
    else printf ("%s" , lb ) ;
    
    offset += thisline;
    line += thisline;
  }
  
  if ( pfile ) fclose ( pfile ) ;
}
////////////////////////////////////////////////////////////////////////////
#if INTERFACE_MODE
#else
int xfd_can_read ( int fd , int sec , int usec )
{
  fd_set rset ;
  struct timeval tv ;
  int ret = 0 ;
  
  if ( fd < 0 ) return 0 ;
  
  tv.tv_sec = sec ;
  tv.tv_usec = usec ;
  
  FD_ZERO ( &rset ) ;
  FD_SET ( fd , &rset ) ;
  
__select_call:
  ret = select ( fd + 1 , &rset , NULL , NULL , &tv ) ;
  if ( ret < 0 )
  {
    if ( errno == EINTR ) goto __select_call ;
    return 0 ;
  }
  if ( ret == 0 ) return 0 ;
  if ( FD_ISSET ( fd , &rset) ) return 1 ;
  else return 0 ;
}
#endif
////////////////////////////////////////////////////////////////////////////
float xsqrt( float v )
{
  static int i ;
  static float x2, y ;
  static const float threehalfs = 1.5f ;
  
  x2 = v * 0.5f ;
  y  = v ;
  i  = * ( int * ) &y ;
  i  = 0x5f375a86 - ( i >> 1 ) ;
  y  = * ( float * ) &i ;
  y  = y * ( threehalfs - ( x2 * y * y ) ) ;
  y  = y * ( threehalfs - ( x2 * y * y ) ) ;
  return v * y ;
}
unsigned int xstr_hash (const char * v)
{
  const char * p ;
  unsigned int h = 5381 ;
  
  if ( !v || !v[0] )
  {
    return 0 ;
  }
  
  for ( p = v; *p != '\0'; p ++ )
  {
    h = (h << 5) + h + (*p) ;
  }
  
  return h ;
}
char * xstrdup ( const char * str )
{
  char * ret  = NULL ;
  
  if ( !str ) return NULL ;
  
  ret =  xmalloc ( (unsigned int)strlen(str) );
  strcpy ( ret , str ) ;
  
  return ret ;
}
char * xstrndup ( char * str , int n )
{
  char * ret  = NULL ;
  
  if ( !str ) return NULL ;

  ret =  xmalloc ( n + 1 );
  strncpy ( ret , str , n ) ;
  
  return ret ;
}
void * xmemdup ( void * mem , int n )
{
  void * ret  = NULL ;
  
  if ( !mem ) return NULL ;
  
  ret =  xmalloc ( n + 1 );
  memcpy ( ret , mem , n ) ;
  
  return ret ;
}
int xmemmem ( char * buffer , int len1 ,  char * buf , int len2 )
{
  int i , j ;
  
  if ( len1 < len2 )
  {
    return -1 ;
  }
  
  len1 -= len2 - 1 ;
  for ( i = 0 ; i < len1 ; i ++ )
  {
    for ( j = 0 ; j < len2 ; j ++ )
    {
      if ( buffer[i + j] != buf[j] )
      {
        break ;
      }
    }
    if ( j >= len2 )
    {
      break ;
    }
  }
  if ( i >= len1 )
  {
    return -1 ;
  }
  return i ;
}
int xstrcmp ( const char * a , const char * b )
{
  if ( !a )
  {
    if ( !b )
    {
      return 0 ;
    }
    else
    {
      return -1 ;
    }
  }
  else
  {
    if ( !b )
    {
      return 1 ;
    }
    return strcmp(a, b) ;
  }
}
int intpercentcalc ( int min , int max , int current , int limit )
{
  float percent ;
  int ret ;
  
  if ( current >= limit || current <= 0 )
  {
    return max ;
  }
  
  percent = ((float)current) / ((float)limit) ;
  ret = (int)(percent * max) ;
  if ( ret < min )
  {
    ret = min ;
  }
  return ret ;
}
int file_size ( char * filename )
{
  FILE * pFile = NULL ;
  int lSize = 0 ;
  
  pFile = fopen (filename, "rb" );
  if ( !pFile) return 0;
  
  if ( !fseek (pFile , 0 , SEEK_END) ) lSize = (int)ftell (pFile);
  
  fclose(pFile);
  
  return lSize;
}
int file_content ( char ** data , int * size , char * filename )
{
  int statu = 0 ;
  FILE * pfile = NULL ;
  char * filedata = NULL ;
  int filedatasize = 0 ;
  
  if ( !data || !size || !filename ) return 0 ;
  
  //get size
  filedatasize = file_size ( filename ) ;
  if ( !filedatasize ) return 0 ;
  
  //malloc memory
  filedata = xmalloc ( filedatasize ) ;
  if ( !filedata ) return 0 ;
  
  //open file
  pfile = fopen ( filename , "rb" ) ;
  if ( !pfile ) goto __finish ;
  
  //read file data
  statu = (int)fread ( filedata , sizeof(char) , filedatasize , pfile ) ;
  if ( !statu ) goto __finish ;
  
  *data = filedata ;
  *size = filedatasize ;
__finish:
  if ( pfile ) fclose ( pfile ) ;
  if ( !statu ) xfree ( filedata ) ;
  
  return statu ;
}

int sigignore(int sig)
{
    struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN, sa.sa_flags = 0;
    if (sigemptyset(&sa.sa_mask) == -1 || sigaction(sig, &sa, 0) == -1) {
        return -1;
    }
    return 0;
}

