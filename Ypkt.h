#ifndef __Y_PKT_H_
#define __Y_PKT_H_

#include "afx/afx.h"

#define PKT_YS_START_TAG 0xA5
#define PKT_YS_END_TAG 0x5A
#define PKT_YS_FRAME_TYPE 0xB5




unsigned char pkt_build_check_sum ( unsigned char * buf , int size);
int pkt_check_sum ( unsigned char * buf , int size , unsigned char checksum );
int pkt_build_byte_tag(unsigned char * buf, unsigned char tag);
unsigned char *  pkt_build_short_tag(unsigned char * buf, unsigned short tag);
unsigned char *  pkt_match_head(unsigned char *buf, unsigned char tag);



#endif
