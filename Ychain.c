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




#include "Ychain.h"


static struct buffer_chain *
buffer_chain_new(unsigned int size)
{
	struct buffer_chain *chain;
	unsigned int to_alloc;

	if (size > EV_SIZE_MAX - EVBUFFER_CHAIN_SIZE)
		return (NULL);

	size += EVBUFFER_CHAIN_SIZE;

	/* get the next largest memory that can hold the buffer */
	if (size < EV_SIZE_MAX / 2) {
		to_alloc = MINBUFFER_SIZE;
		while (to_alloc < size) {
			to_alloc <<= 1;
		}
	} else {
		to_alloc = size;
	}

	/* we get everything in one chunk */
	if ((chain = Ymalloc(to_alloc)) == NULL)
		return (NULL);

	memset(chain, 0, EVBUFFER_CHAIN_SIZE);

	chain->buffer_len = to_alloc - EVBUFFER_CHAIN_SIZE;

	/* this way we can manipulate the buffer to different addresses,
	 * which is required for mmap for example.
	 */
	chain->buffer = (unsigned char *)((struct buffer_chain *)(chain) + 1);

	chain->refcnt = 1;

	return (chain);
}

static inline void
buffer_chain_free(struct buffer_chain *chain)
{
	assert(chain->refcnt > 0);
	if (--chain->refcnt > 0) {
		return;
	}


	Yfree((void *)chain);
}

Ychain_t *Ychain_new()
{
	Ychain_t *Ychain = NULL;
	Ychain = calloc(1, sizeof(Ychain_t));
	if (Ychain == NULL)
		return (NULL);

	Ychain->refcnt = 1;
	Ychain->last_with_datap = &Ychain->first;

	return (Ychain);
}


void Ychain_free(Ychain_t *Ychain)
{
	struct buffer_chain *chain, *next;

	assert(Ychain->refcnt > 0);

	if (--Ychain->refcnt > 0) { //被引用次数-1仍然没有为0不能释放
		return;
	}

	for (chain = Ychain->first; chain != NULL; chain = next) {
		next = chain->next;
		buffer_chain_free(chain);
	}
	
	//mm_free(Ychain);
	free(Ychain);
}


int Ychain_size(const Ychain_t *Ychain)
{
	return (int)(Ychain->total_len);
}


int chain_add(Ychain_t *Ychain, const void *data_in, unsigned int datlen)
{
	struct buffer_chain *chain, *tmp;
	const unsigned char *data = data_in;
	unsigned int remain, to_alloc;
	int result = -1;

	/*if (Ychain->freeze_end) {
		goto done;
	}*/
	/* Prevent buf->total_len overflow */
	if (datlen > EV_SIZE_MAX - Ychain->total_len) {
		goto done;
	}

	if (*Ychain->last_with_datap == NULL) {
		chain = Ychain->last;
	} else {
		chain = *Ychain->last_with_datap;
	}

	/* If there are no chains allocated for this buffer, allocate one
	 * big enough to hold all the data. */
	if (chain == NULL) {
		chain = buffer_chain_new(datlen);
		if (!chain)
			goto done;
		buffer_chain_insert(Ychain, chain);
	}


	/* we need to add another chain */
	to_alloc = chain->buffer_len;
	if (to_alloc <= EVBUFFER_CHAIN_MAX_AUTO_SIZE/2)
		to_alloc <<= 1;
	if (datlen > to_alloc)
		to_alloc = datlen;
	tmp = buffer_chain_new(to_alloc);
	if (tmp == NULL)
		goto done;

	if (remain) {
		memcpy(chain->buffer + chain->misalign + chain->off,
		    data, remain);
		chain->off += remain;
		Ychain->total_len += remain;
		Ychain->n_add_for_cb += remain;
	}

	data += remain;
	datlen -= remain;

	memcpy(tmp->buffer, data, datlen);
	tmp->off = datlen;
	buffer_chain_insert(Ychain, tmp);

out:
	result = 0;
done:
	return result;
}
