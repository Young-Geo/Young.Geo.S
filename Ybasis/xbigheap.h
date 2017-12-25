#ifndef __H_XBIGHEAP_H__
#define __H_XBIGHEAP_H__
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include "xarray.h"
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
typedef struct _xbigheap_v
{
  int weight , idx ;
  void * value ;
} xbigheap_v ;
typedef struct _xbigheap_t
{
  int size ;
  xarray * heap ;
} xbigheap_t ;
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
xbigheap_t * xbigheap_alloc () ;
void xbigheap_free ( xbigheap_t ** pbig ) ;
////////////////////////////////////////////////////////////////////////////
void * xbigheap_push ( xbigheap_t * big , void * value , int weight ) ;
int  xbigheap_pop  ( xbigheap_t * big , void ** pvalue , int * pweight ) ;
void xbigheap_update ( xbigheap_t * big , void * idx , int weight ) ;
#define xbigheap_get(BIG,REF) (((xbigheap_v *)REF) ->value)
#define xbigheap_isempty(BIG) ((BIG) ->size <= 1)
#define xbigheap_notempty(BIG) ((BIG) ->size > 1)
#define xbigheap_size(BIG) ((BIG) ->size)
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#endif