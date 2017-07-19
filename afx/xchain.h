#ifndef __H_XCHAIN_H__
#define __H_XCHAIN_H__

#include "afx.h"

#define BUFFER_GRANULE  512

struct xchain_granule
{
  struct xchain_granule *next;
  int buflen, bufpos;
  char buf[BUFFER_GRANULE];
} ;

typedef struct bufchain_tag
{
  struct xchain_granule *head, *tail;
  int buffersize;		       /* current amount of buffered data */
  
  char * string ;
  int update ;
} xchain;

void xchain_init (xchain *ch) ;
xchain * xchain_alloc() ;
void xchain_clear(xchain *ch) ;
void xchain_reset(xchain *ch) ;
#define xchain_size(ch) ((ch)->buffersize)
void xchain_add(xchain *ch, const void *data, int len) ;
void xchain_pre_add ( xchain *ch , const void *data, int len ) ;
void xchain_delete(xchain *ch, int len) ;
void xchain_get (xchain *ch, void *data, int len) ;
void xchain_2data (xchain *ch, char ** data , int * len ) ;
char * xchain_str ( xchain *ch ) ;

#endif