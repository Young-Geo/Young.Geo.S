#include "xchain.h"

void xchain_init (xchain *ch)
{
  memset ( ch , 0 , sizeof ( xchain ) ) ;
  ch->head = ch->tail = NULL;
  ch->buffersize = 0;
}

xchain * xchain_alloc()
{
  xchain * ch = ( xchain * ) xmalloc ( sizeof ( xchain ) ) ;
  ch->head = ch->tail = NULL;
  ch->buffersize = 0;
  
  return ch ;
}

void xchain_clear(xchain *ch)
{
  if ( !ch ) return ;
  xchain_reset ( ch ) ;
  xfree ( ch ) ;
}

void xchain_reset (xchain *ch)
{
  struct xchain_granule *b;
  
  if ( !ch ) return ;
  
  while (ch->head)
  {
    b = ch->head;
    ch->head = ch->head->next;
    
    xfree(b);
  }
  
  ch->tail = NULL;
  ch->buffersize = 0;
  
  xfree ( ch ->string ) ;
  ch ->update = 0 ;
}

void xchain_add(xchain *ch, const void *data, int len)
{
  const char *buf = (const char *)data;
  
  if (!ch || !data || len <= 0) return;
  
  ch->buffersize += len;
  
  if (ch->tail && ch->tail->buflen < BUFFER_GRANULE)
  {
    int copylen = XXMIN(len, BUFFER_GRANULE - ch->tail->buflen);
    
    memcpy(ch->tail->buf + ch->tail->buflen, buf, copylen);
    buf += copylen;
    len -= copylen;
    ch->tail->buflen += copylen;
  }
  
  while (len > 0)
  {
    int grainlen = XXMIN(len, BUFFER_GRANULE);
    struct xchain_granule *newbuf;
    
    newbuf = ( struct xchain_granule * ) xmalloc ( sizeof (struct xchain_granule) ) ;

    newbuf->bufpos = 0;
    newbuf->buflen = grainlen;
    
    memcpy(newbuf->buf, buf, grainlen);
    buf += grainlen;
    len -= grainlen;
    
    if (ch->tail)
      ch->tail->next = newbuf;
    else
      ch->head = ch->tail = newbuf;
    
    newbuf->next = NULL;
    ch->tail = newbuf;
  }
  
  ch ->update = 1 ;
}

void xchain_pre_add ( xchain *ch , const void *data, int len )
{
  struct xchain_granule * head = NULL ;
  
  if ( !ch || !data || len <= 0 ) return ;
  
  if ( ch ->buffersize <= 0 )
  {
    xchain_add(ch, data, len ) ;
    return ;
  }
  ch ->buffersize += len ;
  
  head = ch ->head ;
  if ( head ->bufpos > 0 )
  {
    int copylen = 0 ;
    
    copylen = XXMIN ( len , head ->bufpos ) ;
    
    head ->bufpos -= copylen ;
    len -= copylen ;
    
    memcpy ( head ->buf + head ->bufpos ,
            ((char *)data) + len ,
            copylen ) ;
  }
  
  while ( len > 0 )
  {
    int grainlen = 0 ;
    struct xchain_granule * newbuf = NULL ;
    
    grainlen = XXMIN ( BUFFER_GRANULE , len ) ;
    
    newbuf = ( struct xchain_granule * ) xmalloc ( sizeof (struct xchain_granule) ) ;

    newbuf->bufpos = BUFFER_GRANULE - grainlen;
    newbuf->buflen = BUFFER_GRANULE ;
    
    memcpy(newbuf->buf + newbuf ->bufpos , data , grainlen);
    data = ((char *)data) + grainlen;
    len -= grainlen;
    
    newbuf ->next = head ;
    ch ->head = head = newbuf ;
  }
  
  ch ->update = 1 ;
}

void xchain_delete(xchain *ch, int len)
{
  struct xchain_granule *tmp;
  
  if ( !ch || len <= 0 ) return ;
  
  if (ch->buffersize < len) len = ch->buffersize ;
  
  while (len > 0)
  {
    int remlen = len;
    
    assert(ch->head != NULL);
    
    if (remlen >= ch->head->buflen - ch->head->bufpos)
    {
      remlen = ch->head->buflen - ch->head->bufpos;
      tmp = ch->head;
      ch->head = tmp->next;
      xfree(tmp);
      
      if (!ch->head)
        ch->tail = NULL;
    }
    else
    {
      ch->head->bufpos += remlen;
    }
    
    ch->buffersize -= remlen;
    len -= remlen;
  }
  
  ch ->update = 1 ;
}

void xchain_get(xchain *ch, void *data, int len)
{
  struct xchain_granule *tmp;
  char *data_c = (char *)data;
  
  if ( !ch || !data || len <= 0 ) return ;
  
  tmp = ch->head;
  
  if (ch->buffersize < len) len = ch->buffersize ;
  
  while (len > 0)
  {
    int remlen = len;
    
    assert(tmp != NULL);
    if (remlen >= tmp->buflen - tmp->bufpos)
    {
      remlen = tmp->buflen - tmp->bufpos;
    }
    memcpy(data_c, tmp->buf + tmp->bufpos, remlen);
    
    tmp = tmp->next;
    len -= remlen;
    data_c += remlen;
  }
}

void xchain_2data (xchain *ch, char ** data , int * len )
{
  char * d = 0 ;
  int s = 0 ;
  
  if ( !ch || !data ) return ;
  
  *data = NULL ;
  if ( len ) *len = 0 ;
  
  s = xchain_size ( ch ) ;
  if ( !s ) return ;
  
  d = xmalloc ( s ) ;
  xchain_get ( ch , d , s ) ;
  
  *data = d ;
  if ( len ) *len = s ;
  return ;
}

char * xchain_str ( xchain *ch )
{
  int tmp = 0 ;
  
  if ( !ch ) return NULL ;
  
  if ( (!( ch ->update )) && ( ch ->string ) )
    return ch ->string ;
  
  xfree ( ch ->string ) ;
  
  xchain_2data(ch , &(ch ->string), &tmp ) ;
  return ch ->string ;
}













