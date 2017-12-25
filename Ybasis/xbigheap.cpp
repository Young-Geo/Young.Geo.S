////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include "xbigheap.h"
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
xbigheap_t * xbigheap_alloc ()
{
  xbigheap_t * big ;
  xbigheap_v * v  = NULL ;
  
  big = (xbigheap_t *)xmalloc(sizeof(xbigheap_t)) ;
  big ->heap = xarray_alloc(8) ;
  
  xarray_set(big ->heap, 0, &v) ;
  big ->size ++ ;
  
  return big ;
}
void xbigheap_free ( xbigheap_t ** pbig )
{
  xbigheap_t * big ;
  xbigheap_v * bigv ;
  int i , ret ;
  
  if ( !pbig ) return ;
  big = *pbig ;
  
  if ( !big )return ;
  *pbig = NULL ;
  
  for ( i = 0 ; i < big ->size ; i ++ )
  {
    ret = xarray_get(big ->heap, i, &bigv) ;
    if ( !ret || !bigv )
    {
      continue ;
    }
    xfree(bigv ->value) ;
    xfree(bigv) ;
  }
  xarray_free(&big ->heap) ;
  xfree(big) ;
}
////////////////////////////////////////////////////////////////////////////
static void xbigheap_exchange ( xbigheap_t * big , int i , int j )
{
  xbigheap_v * vi , * vj ;
  
  xarray_get(big ->heap, i, &vi) ;
  xarray_get(big ->heap, j, &vj) ;
  xarray_set(big ->heap, i, &vj) ;
  xarray_set(big ->heap, j, &vi) ;
  vj ->idx = i ;
  vi ->idx = j ;
}
static int xbigheap_weight_get ( xbigheap_t * big , int i )
{
  xbigheap_v * v = NULL ;
  
  xarray_get(big ->heap, i, &v) ;
  return v ->weight ;
}
static void xbigheap_swim(xbigheap_t * big , int i)
{
  int weight_parent , weight ;
  while (i > 1)
  {
    weight_parent = xbigheap_weight_get ( big , i / 2 ) ;
    weight = xbigheap_weight_get ( big , i ) ;
    
    if ( weight <= weight_parent )
    {
      break ;
    }
    
    xbigheap_exchange(big, i / 2 , i) ;
    i /= 2 ;
  }
}
static void xbigheap_sink( xbigheap_t * big ,int i)
{
  int max_idx , max , left , right , weight , weight_left , weight_right , weight_max ;
  
  max_idx = big ->size - 1 ;
  while( (i << 1) < max_idx )
  {
    left = i << 1 ; right = left + 1 ;
    
    weight = xbigheap_weight_get(big, i) ;
    weight_left = xbigheap_weight_get(big, left ) ;
    weight_right = xbigheap_weight_get(big, right ) ;
    
    if(weight_left < weight_right)
    {
      max = right ;
      weight_max = weight_right ;
    }
    else
    {
      max = left ;
      weight_max = weight_left ;
    }
    
    if ( weight >= weight_max )
    {
      break ;
    }
    
    xbigheap_exchange(big, i, max) ;
    i = max ;
  }
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void xbigheap_print ( xbigheap_t * big )
{
  int i ;
  xbigheap_v * heapv ;
  
  for ( i = 1 ; i < big ->size ; i ++ )
  {
    xarray_get(big ->heap, i, &heapv) ;
    assert(heapv) ;
    xmessage("xbigheap[%d] : %d(%d)\n" , i , heapv ->weight , heapv ->idx) ;
  }
}
void * xbigheap_push ( xbigheap_t * big , void * value , int weight )
{
  xbigheap_v * heapv ;
  
  heapv = (xbigheap_v *)xmalloc(sizeof(xbigheap_v)) ;
  heapv ->weight = weight ;
  heapv ->value = value ;
  
  heapv ->idx = big ->size ;
  xarray_set(big ->heap, heapv ->idx, &heapv) ;
  big ->size ++ ;
  
  xbigheap_swim(big, heapv ->idx) ;
  return heapv ;
}
int xbigheap_pop  ( xbigheap_t * big , void ** pvalue , int * pweight )
{
  xbigheap_v * heapv ;
  int idx ;
  
  if ( big ->size <= 1 )
  {
    return 0 ;
  }
  
  xarray_get(big ->heap, 1, &heapv) ; assert(heapv ->idx == 1) ;
  if ( pvalue ) *pvalue  = heapv ->value ;
  if ( pweight) *pweight = heapv ->weight ;
  
  xfree(heapv) ;
  big ->size -- ;
  
  if ( big ->size > 1 )
  {
    idx = big ->size ;
    xarray_get(big ->heap, idx, &heapv) ; assert(heapv ->idx == idx) ;
    heapv ->idx = 1 ;
    xarray_set(big ->heap, 1, &heapv) ;
    
    xbigheap_sink(big, 1) ;
  }
  return 1 ;
}
void xbigheap_update ( xbigheap_t * big , void * idx , int weight )
{
  xbigheap_v * heapv = NULL , * heapidx = (xbigheap_v *)idx ;
  int up ;
  
  xarray_get(big ->heap, heapidx ->idx, &heapv) ; assert(heapv == idx) ;
  if ( weight == heapv ->weight )
  {
    return ;
  }
  else if ( weight > heapv ->weight )
  {
    up = 1 ;
  }
  else
  {
    up = 0 ;
  }
  heapv ->weight = weight ;
  
  if ( up )
  {
    xbigheap_swim(big, heapv ->idx) ;
  }
  else
  {
    xbigheap_sink(big, heapv ->idx) ;
  }
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////