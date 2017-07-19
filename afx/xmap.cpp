 //  Created by 胡兵兵 on 16/4/1.
//  Copyright © 2016年 xxx_orgnization. All rights reserved.
//

#include "xmap.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
xmap * xmap_alloc ()
{
  xmap * map = (xmap *)xmalloc(sizeof(xmap)) ;
  map ->map = xarray2_alloc(8) ;
  return map ;
}
void xmap_free ( xmap ** pmap )
{
  xmap * map ;
  int i , j , l , r , b , t ;
  void * p ;
  
  if ( !pmap ) return ;
  map = *pmap ;
  
  if ( !map )return ;
  *pmap = NULL ;
  
  xarray2_left_right(map ->map, &l, &r) ;
  for ( i = l ; i <= r ; i ++ )
  {
    xarray2_bottom_top(map ->map, i, &b, &t) ;
    for ( j = b ; j <= t ; j ++ )
    {
      if ( xarray2_get(map ->map, i, j, &p) && p )
      {
        xfree(p) ;
      }
    }
  }
  
  xarray2_free(&map ->map) ;
  xfree(map);
}
////////////////////////////////////////////////////////////////////////////
void xmap_set ( xmap * map , int i , int j , void * v )
{
  static int ibase , jbase , tempi , tempj ;

  if ( i < 0 )
  {
    i = -i ;
    tempi = (i + SECTION_SIZE_1) >> SECTION_SIZE_2n ;
    
    ibase = - tempi ;
    i -= ((tempi - 1) << SECTION_SIZE_2n) + 1 ;
  }
  else
  {
    ibase = i >> SECTION_SIZE_2n ;
    i -= ibase << SECTION_SIZE_2n ;
  }
  if ( j < 0 )
  {
    j = -j ;
    tempj = (j + SECTION_SIZE_1) >> SECTION_SIZE_2n ;
    
    jbase = - tempj ;
    j -= ((tempj - 1) << SECTION_SIZE_2n) + 1 ;
  }
  else
  {
    jbase = j >> SECTION_SIZE_2n ;
    j -= jbase << SECTION_SIZE_2n ;
  }
  assert(i < SECTION_SIZE && j < SECTION_SIZE && i >= 0 && j >= 0) ;
  
  if ( (ibase != map ->i) || (jbase != map ->j) || (!map ->p) )
  {
    map ->i = ibase ;
    map ->j = jbase ;
    
    if ( !xarray2_get(map ->map, ibase, jbase, &map ->p) || !map ->p)
    {
      map ->p = (void **)xmalloc( SECTION_SIZE * SECTION_SIZE * 8) ;
      xarray2_set(map ->map, ibase, jbase, (char *)&map ->p) ;
    }
  }
  map ->p[(i << SECTION_SIZE_2n) + j] = *(void **)v ;
}
int  xmap_get ( xmap * map , int i , int j , void * v )
{
  static int ibase , jbase , tempi , tempj ;
  
  if ( i < 0 )
  {
    i = -i ;
    tempi = (i + SECTION_SIZE_1) >> SECTION_SIZE_2n ;
    
    ibase = - tempi ;
    i -= ((tempi - 1) << SECTION_SIZE_2n) + 1 ;
  }
  else
  {
    ibase = i >> SECTION_SIZE_2n ;
    i -= ibase << SECTION_SIZE_2n ;
  }
  if ( j < 0 )
  {
    j = -j ;
    tempj = (j + SECTION_SIZE_1) >> SECTION_SIZE_2n ;
    
    jbase = - tempj ;
    j -= ((tempj - 1) << SECTION_SIZE_2n) + 1 ;
  }
  else
  {
    jbase = j >> SECTION_SIZE_2n ;
    j -= jbase << SECTION_SIZE_2n ;
  }
  assert(i < SECTION_SIZE && j < SECTION_SIZE && i >= 0 && j >= 0) ;
  
  if ( (ibase != map ->i) || (jbase != map ->j) || (!map ->p) )
  {
    if ( !xarray2_get(map ->map, ibase, jbase, &map ->p) || !map ->p)
    {
      return 0 ;
    }
    map ->i = ibase ;
    map ->j = jbase ;
  }
  *(void **)v = map ->p[(i << SECTION_SIZE_2n) + j] ;
  return 1 ;
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
int xmap2buffer (xmap * map , int left , int bottom , int right , int top ,
                  unsigned char (*pv2gray)(char exist , void * pv) ,
                  unsigned char ** buffer , int * width , int * height )
{
  int w , h , x , y ;
  unsigned char * buf , gray , * l ;
  
  char exist = 0 ;
  void * pv ;
  
  w = right - left + 1 ;
  h = top - bottom + 1 ;
  if ( w <= 0 || h <= 0 || w > 65535 || h > 65535 )
  {
    return 0 ;
  }
  
  buf = (unsigned char *)xmalloc(w * h) ;
  for ( y = top ; y >= bottom ; y -- )
  {
    l = buf + (y - top) * w ;
    for ( x = left ; x <= right ; x ++ )
    {
      if ( !xmap_get(map, x, y, &pv) )
      {
        exist = 0 ;
      }
      if ( pv2gray )
      {
        gray = pv2gray(exist, pv) ;
      }
      else
      {
        gray = exist ? (pv ? 255 : 0) : 0 ;
      }
      if ( !gray )
      {
        l[x - left] = gray ;
      }
    }
  }
  
  *width = w ; *height = h ; *buffer = buf ;
  return 1 ;
}
////////////////////////////////////////////////////////////////////////////
int buffer2xmap (unsigned char * buffer , int width , int height ,
                 void * (*gray2pv)(unsigned char gray) ,
                 xmap * map , int left , int top , int * right , int * bottom )
{
  unsigned char * line , v ;
  int x , y , l ;
  void * pv ;
  
  for ( y = 0 ; y < height ; y ++ )
  {
    line = buffer + y * width ;
    l = top - y ;
    for ( x = 0 ; x < width ; x ++ )
    {
      v = line[x] ;
      if ( gray2pv )
      {
        pv = gray2pv(v) ;
      }
      else
      {
        pv = v ? (I2P(1)) : NULL ;
      }
      xmap_set(map, x + left, l, &pv) ;
    }
  }
  *right = left + width - 1 ;
  *bottom = top - height + 1 ;
  return 1 ;
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







