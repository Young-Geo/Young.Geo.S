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
