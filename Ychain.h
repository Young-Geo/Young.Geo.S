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










#ifndef __YCHAIN__H_
#define __YCHAIN__H_


#define EVBUFFER_CHAIN_SIZE sizeof(struct buffer_chain)
#define EV_SIZE_MAX YUINT_MAX
#define EVBUFFER_CHAIN_MAX_AUTO_SIZE 4096



struct buffer_chain {
	struct buffer_chain *next;

	unsigned int buffer_len;

	ev_misalign_t misalign;
	unsigned int off;
	unsigned flags;
	int refcnt;
	unsigned char *buffer;
};


typedef struct _Ychain_t {
	struct buffer_chain *first;
	struct buffer_chain *last;

	struct buffer_chain **last_with_datap;

	unsigned int total_len;
	
	int refcnt;
	
} Ychain_t;




Ychain_t *Ychain_new(void);

void Ychain_free(Ychain_t *Ychain);

#define Ychain_size(Ychain) ((Ychain)->total_len)
int Ychain_size(const Ychain_t *Ychain);

















#endif
