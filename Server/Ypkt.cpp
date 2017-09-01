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

unsigned char *  pkt_match_head(unsigned char *buf, int len, unsigned char tag)
{
	int i = 0;
	if (!buf) {
		return NULL;
	}
	for (i = 0; i < len; ++i)
	{
		if (*buf == tag) {
			return buf;
		}
		++buf;
	}
	/*
	while (1)
	{
		if (*buf == '\0')
			break;
		if (*buf == tag) {
			return buf;
		}
		++buf;
	}*/
	return NULL;
}






int	 pkt_make_client(unsigned char *buf, int len, unsigned char **out_data, int *out_len)
{
	unsigned char *data = NULL, *pack = NULL, xor_cc;

	XXNULL(buf, -1);
	XXNULL(out_data, -1);
	XXNULL(out_len, -1);

	
	pack = (unsigned char *)xmalloc((len+PKT_YS_HEADLEN+PKT_YS_ENDLEN));
	XXNULL(pack, -1);
	data = pack;
	
	OUT8(pack, PKT_YS_START_TAG);		
	OUT8(pack, 0);
	OUT8(pack, PKT_YS_FRAME_TYPE);	
	OUT16_BE(pack, (len+PKT_YS_HEADLEN+PKT_YS_ENDLEN));
	xmemcpy(pack, buf, len);
	pack += len;
	OUT8(pack, PKT_YS_END_TAG);
	
	xor_cc = pkt_build_check_sum(data, (len+PKT_YS_HEADLEN+PKT_YS_ENDLEN));
	pack = data + 1;
	OUT8(pack, xor_cc);

	*out_data = data;
	*out_len = (len+PKT_YS_HEADLEN+PKT_YS_ENDLEN);
	return 0;
}

int	 pkt_parse_data(unsigned char *buf, int len, unsigned char **out_data, int *out_len)
{
	unsigned short v = 0;
	unsigned char *data = NULL, *tdata = NULL, xor_cc;

	if (!buf || !out_data) {
		xerror("error par");
		return -1;
	}

	if (!(data = pkt_match_head(buf, len, PKT_YS_START_TAG))) {
		xerror("match head PKT_YS_START_TAG error");
		return -1;
	}
	tdata = data;
	++data;
	
	IN8(data, xor_cc);//异或校验码
	xzero((data - 1), sizeof(unsigned char));

	IN8(data, v);
	assert(v == PKT_YS_FRAME_TYPE);//类型

	IN16_BE(data, v);//大小  数据大小 = v - 6;

	if (!pkt_check_sum(tdata, v, xor_cc)) {
		xerror("match head pkt_check_sum error");
		return -1;
	}
	len = v - 6;
	tdata = (unsigned char *)xmalloc(len + 1);
	xassert(tdata);
	xzero(tdata, (len + 1));
	
	xmemcpy(tdata, data, len);
	
	*out_data = tdata;
	*out_len = len;

	return 0;
}



