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
