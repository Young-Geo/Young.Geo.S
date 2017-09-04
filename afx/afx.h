#ifndef __H_AFX_H__
#define __H_AFX_H__
////////////////////////////////////////////////////////////////////////////
#define LANGUAGE_CPP
#define LINUX
////////////////////////////////////////////////////////////////////////////
#ifndef LINUX
#define MACOSX
#else
#undef MACOSX
#endif


////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
typedef __int64 int64_t
#else
#include <stdint.h>
#endif
////////////////////////////////////////////////////////////////////////////
#define ZERO 0
////////////////////////////////////////////////////////////////////////////
#define Kbyte(x) (1024 * (x))
#define Mbyte(x) (1024 * 1024 * (x))
#define Gbyte(x) (1024 * 1024 * 1024 * (x))
#define Tbyte(x) (1024 * 1024 * 1024 * 1024 * (x))

#define Second(x) (x)
#define Minute(x) (x * 60)
#define Hour(x) (x * 3600)
#define Day(x) (x * 3600 * 24)
////////////////////////////////////////////////////////////////////////////
#define MINBUFFER_SIZE  (256)
#define BUFFER_SIZE     (1024)

#define XXMIN(x,y)  ( (x) < (y) ? (x) : (y) )
#define XXMAX(x,y)  ( (x) > (y) ? (x) : (y) )
#define XXSWAP(x,y) ( (x) ^= (y) ^= (x) ^= (y) )
#define XXSWAP_TMP(x,y,type) {type tmp = y; y = x; x = tmp;}
#define POW2(x) ((x) * (x))
#define P2I(PV) ((int)(((char *)PV) - ((char *)0)))
#define I2P(IV) ((void *)(((char *)0) + IV))

#define XM_PI 3.14159265358979323846264338327950288f
#define XM_2PI 6.28318530717958647692528676655900576f
#define XSQRT2 1.414213562373095f
#define XSQRT3 1.732050807568877f
#define XSQRT5 2.23606797749979f

#define XINT_MAX        2147483647    /* maximum (signed) int value */
#define XFLT_MAX          0x1.fffffep127f
#define XFLT_MIN          0x1.0p-126f
#define XFLT_EPSILON      0x1.0p-23f
#define XDBL_MAX          0x1.fffffffffffffp1023
#define XDBL_MIN          0x1.0p-1022
#define XDBL_EPSILON      0x1.0p-52

#define RADIAN2ANGLE(x) ( (57.29577951308232f) * ((float)x) )
#define ANGLE2RADIAN(x) ( (0.017453292519943f) * ((float)x) )

#define XNULL(x) if (!(x)) return ;
#define XXNULL(x,y) if (!(x)) return y ;

////////////////////////////////////////////////////////////////////////////
#define NEXT_BE(p,v)  ((v) = ((v) << 8) + *((p) ++))
#define SKIP(p,len)   (p += len)
#define IN8(p,v)      ((v) = *((p) ++))
#define IN16_BE(p,v)  {(v) = *((p) ++); NEXT_BE((p),(v));}
#define IN32_BE(p,v)  {IN16_BE((p),(v)); NEXT_BE((p),(v)); NEXT_BE((p),(v));}
#define OUT8(p,v)     (*((p) ++) = (v))
#define OUT16_BE(p,v) {*((p) ++) = ((v) >> 8) & 0xff; *((p) ++) = (v) & 0xff;}
#define OUT32_BE(p,v) {OUT16_BE((p),((v) >> 16) & 0xffff); OUT16_BE((p),(v) & 0xffff); }
#define IN16_LE(p,v)  {(v) = *((p) ++); (v) += *((p) ++) << 8;}
#define IN32_LE(p,v)  {IN16_LE((p),(v)) ; (v) += *((p) ++) << 16; (v) += *((p) ++) << 24;}
#define OUT16_LE(p,v) {*((p) ++) = (v) & 0xff; *((p) ++) = ((v) >> 8) & 0xff; }
#define OUT32_LE(p,v) {OUT16_LE((p), (v) & 0xffff); OUT16_LE((p), ((v) >> 16) & 0xffff); }
////////////////////////////////////////////////////////////////////////////
typedef unsigned char   u8 ;
typedef unsigned short  u16 ;
typedef unsigned int    u32 ;
typedef void * 			p_g;

////////////////////////////////////////////////////////////////////////////
void * saferealloc( void * oldmen , unsigned int size) ;
void * safemalloc(unsigned int size) ;
void safefree(void ** ptr) ;
////////////////////////////////////////////////////////////////////////////
#define xfree(p) safefree((void **)&(p))
#define xmalloc(x) (char*)safemalloc((unsigned int)(x))
#define xrealloc(oldp,x) (char*)saferealloc((oldp), (unsigned int)(x))
#define null NULL
////////////////////////////////////////////////////////////////////////////
#define xzero(buf, count) memset((buf), 0, (count))
#define xmemset(buf, v, count) memset((buf), (v), (count))
////////////////////////////////////////////////////////////////////////////
float stov ( char * s ) ;
int hexstring2hex ( char * s ) ;
////////////////////////////////////////////////////////////////////////////
void hexdump(unsigned char *p, unsigned int len , char * tofile , char * flag ) ;
////////////////////////////////////////////////////////////////////////////
#if INTERFACE_MODE
#else
int xfd_can_read ( int fd , int sec , int usec ) ;
#endif
////////////////////////////////////////////////////////////////////////////
float xsqrt ( float v ) ;
unsigned int xstr_hash (const char * v) ;
#define xstrlen(S) ((int)strlen(S))
#define xstrcpy(dest, src) strcpy((dest), (src))
#define xstrcat(dest, src) strcat((dest), (src))
#define xsprintf(str, format, ...) sprintf((str), (format), ##__VA_ARGS__)
#define xprintf(format, ...) printf((format), ##__VA_ARGS__);
char * xstrdup ( const char * str ) ;
char * xstrndup ( char * str , int n ) ;
void * xmemdup ( void * mem , int n ) ;
int xmemmem ( char * buffer , int len1 ,  char * buf , int len2 ) ;
#define xmemcpy(dest, src, n) memcpy((dest), (src), (n))
#define xmemcmp(dest, src, n) memcmp((dest), (src), (n))
int xstrcmp ( const char * a , const char * b ) ;
#define xstrncmp(a, b, c) strncmp((a), (b), (c))
//unsigned int xgetminsecond () ;
int intpercentcalc ( int min , int max , int current , int limit ) ;
int file_size ( char * filename ) ;
int file_content ( char ** data , int * size , char * filename ) ;
////////////////////////////////////////////////////////////////////////////
int sigignore(int sig);
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#endif
