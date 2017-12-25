 //  Created by 胡兵兵 on 16/4/1.
//  Copyright © 2016年 xxx_orgnization. All rights reserved.
//

#include "xarray.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static xarray_pos * xarrayp_alloc ( int sizeofv )
{
  xarray_pos * arrp ;
  
  arrp = (xarray_pos *) xmalloc(sizeof(xarray_pos)) ;
  arrp ->sizeofv = sizeofv ;
  
  return arrp ;
}
static void xarrayp_free ( xarray_pos ** parrp )
{
  xarray_pos * arrp ;
  
  int i ;
  char * cell ;
  
  if ( !parrp )return ;
  arrp = *parrp ;
  
  if ( !arrp ) return ;
  *parrp = NULL ;
  
  for ( i = 0 ; i < arrp ->array_count ; i ++ )
  {
    cell = *(char **)(arrp ->array + (i * 8)) ;
    if ( !cell ) break ;
    
    xfree(cell) ;
  }
  xfree(arrp ->array) ;
  xfree(arrp) ;
}
////////////////////////////////////////////////////////////////////////////
void xarrayp_set ( xarray_pos* arrp , int i , void * v )
{
  int idx , j , needsize ;
  char * cell ;
  
  idx = i >> CELL_SIZE_2n ;
  needsize = (idx + 1) * 8 ;
  
  if ( needsize > (int)arrp ->array_size )
  {
    needsize = ((needsize + CELL_SIZE - 1) >> CELL_SIZE_2n) << CELL_SIZE_2n ;
    arrp ->array_size = needsize ;
    arrp ->array = xrealloc(arrp ->array, arrp ->array_size) ;
  }
  while ( arrp ->array_count <= idx )
  {
    *(char **)(arrp ->array + arrp ->array_count * 8) =
    xmalloc(CELL_SIZE * arrp ->sizeofv) ;
    arrp ->array_count ++ ;
  }
  
  cell = *(char **)(arrp ->array + idx * 8) ;
  j = i % CELL_SIZE ;
  
  switch (arrp ->sizeofv)
  {
    case 1:
      *(cell + j) = *(char *)v ;
      break;
    case 2:
      *(short *)(cell + (j << 1)) = *(short *)v ;
      break ;
    case 4:
      *(int *)(cell + (j << 2)) = *(int *)v ;
      break;
    case 8:
      *(void **)(cell + (j << 3)) = *(void **)v ;
      break ;
    default:
      memcpy(cell + (j * arrp ->sizeofv), v, arrp ->sizeofv) ;
      break;
  }
  
  if ( i >= (int)arrp ->size )
  {
    arrp ->size = i + 1 ;
  }
}
int  xarrayp_get ( xarray_pos* arrp , int i , void * v )
{
  int idx , j ;
  char * cell ;
  
  if ( i >= (int)arrp ->size )
  {
    return 0 ;
  }
  
  idx = i >> CELL_SIZE_2n ;
  j = i % CELL_SIZE ;
  
  assert(arrp ->array_count > idx) ;
  cell = *(char **)(arrp ->array + idx * 8) ;
  
  switch (arrp ->sizeofv)
  {
    case 1:
      *(char *)v = *(cell + j) ;
      break;
    case 2:
      *(short *)v = *(short *)(cell + (j << 1)) ;
      break ;
    case 4:
      *(int *)v = *(int *)(cell + (j << 2)) ;
      break;
    case 8:
      *(void **)v = *(void **)(cell + (j << 3)) ;
      break ;
    default:
      memcpy(v, cell + (j * arrp ->sizeofv), arrp ->sizeofv) ;
      break;
  }
  return 1 ;
}
////////////////////////////////////////////////////////////////////////////
xarray * xarray_alloc ( int sizeofv )
{
  xarray * arr ;
  
  arr = (xarray *)xmalloc(sizeof(xarray)) ;
  arr ->positive = xarrayp_alloc(sizeofv) ;
  arr ->nagetive = xarrayp_alloc(sizeofv) ;
  
  return arr ;
}
void xarray_free ( xarray ** parr )
{
  xarray * arr ;
  
  if ( !parr ) return ;
  arr = *parr ;
  
  if ( !arr ) return ;
  *parr = NULL ;
  
  xarrayp_free(&arr ->positive) ;
  xarrayp_free(&arr ->nagetive) ;
  xfree(arr) ;
}
void xarray_left_right ( xarray * arr , int * left , int * right )
{
  *left = - (xarrayp_size(arr ->nagetive)) ;
  *right = xarrayp_size(arr ->positive) ;
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
xarray2 * xarray2_alloc ( int sizeofv )
{
  xarray2 * arr2 ;
  
  arr2 = (xarray2 *)xmalloc(sizeof(xarray2)) ;
  arr2 ->arr2 = xarray_alloc(8) ;
  arr2 ->sizeofv = sizeofv ;
  
  return arr2 ;
}
void xarray2_free_arrp ( xarray_pos * arrp )
{
  int i , j ;
  char * cell ;
  xarray * arr ;
  
  for ( i = 0 ; i < arrp ->array_count ; i ++ )
  {
    cell = *(char **)(arrp ->array + (i * 8)) ;
    if ( !cell ) break ;
    
    for ( j = 0 ; j < CELL_SIZE ; j ++ )
    {
      arr = *((xarray **)(cell + j * 8)) ;
      if ( !arr ) continue ;
      
      xarray_free(&arr) ;
    }
    xfree(cell) ;
  }
  xfree(arrp ->array) ;
  xfree(arrp) ;
}
void xarray2_free ( xarray2 ** parr2 )
{
  xarray2 * arr2 ;
  
  if ( !parr2 )return ;
  arr2 = *parr2 ;
  
  if ( !arr2 )return ;
  *parr2 = NULL ;
  
  xarray2_free_arrp(arr2 ->arr2 ->positive) ;
  xarray2_free_arrp(arr2 ->arr2 ->nagetive) ;
  xfree(arr2 ->arr2) ;
  xfree(arr2) ;
}
////////////////////////////////////////////////////////////////////////////
void xarray2_set ( xarray2 * arr2 , int i , int j , void * v )
{
  xarray * arr ;
  int ret ;
  
  ret = xarray_get(arr2 ->arr2, i, (char *)&arr) ;
  if ( !ret || !arr )
  {
    arr = xarray_alloc(arr2 ->sizeofv) ;
    xarray_set(arr2 ->arr2, i, (char *)&arr) ;
  }
  
  xarray_set(arr, j, v) ;
}
int xarray2_get ( xarray2 * arr2 , int i , int j , void * v )
{
  xarray * arr ;
  int ret ;
  
  ret = xarray_get(arr2 ->arr2, i, (char *)&arr) ;
  if ( !ret || !arr )
  {
    return 0 ;
  }
  
  return xarray_get(arr, j, v) ;
}
void xarray2_left_right ( xarray2 * arr2 , int * left , int * right )
{
  xarray_left_right(arr2 ->arr2, left, right) ;
}
void xarray2_bottom_top ( xarray2 * arr2 , int i , int * bottom , int * top )
{
  xarray * arr ;
  int ret ;
  
  ret = xarray_get(arr2 ->arr2, i, (char *)&arr) ;
  if ( !ret || !arr )
  {
    *bottom = 0 ;
    *top = 0 ;
  }
  else
  {
    xarray_left_right(arr, bottom, top) ;
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
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
