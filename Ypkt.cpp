#include "Ypkt.h"



int pkt_check_sum(unsigned char * buf , int size , unsigned char checksum )
{
  int i ;
  unsigned char sum = 0 ;
  
  for ( i = 0 ; i < size ; i ++ )
  {
    sum ^= buf[i] ;
  }
  
  if ( checksum != sum )
  {
  	return 0;
  }
  return 1 ;
}


unsigned char pkt_build_check_sum(unsigned char * buf , int size )
{
  int i ;
  unsigned char sum = 0 ;
  
  for ( i = 0 ; i < size ; i ++ )
  {
    sum ^= buf[i] ;
  }
  
  return sum ;
}

unsigned char *  pkt_build_byte_tag(unsigned char * buf, unsigned char tag)
{
	if (!buf) {
		return NULL;
	}
	*buf = tag;
	return ++buf;
}

unsigned char *  pkt_build_short_tag(unsigned char * buf, unsigned short tag)
{
	if (!buf) {
		return NULL;
	}
	*((unsigned short *)buf) = tag;
	return (buf += 2);
}

unsigned char *  pkt_match_head(unsigned char *buf, unsigned char tag)
{
	int i = 0;
	if (!buf) {
		return NULL;
	}
	while (1)
	{
		if (*buf == '\0')
			break;
		if (*buf == tag) {
			return buf;
		}
		++buf;
	}
	return NULL;
}
