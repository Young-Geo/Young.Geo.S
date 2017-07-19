#include "Ybasis.h"
#include "argv.h"

#if 0
#ifdef  __cplusplus
extern "C" {
#endif
#endif

#ifndef __YCODE__H_
#define __YCODE__H_


#define ARGV_T 1



typedef void YCode;
typedef unsigned char YCodetype;

int Ycode(IN YCode *code, YCodetype type, OUT unsigned char **buf)
{
	int ret = 0;
	if (!code || type > YUCHAR_MAX) {
		fprintf(stderr, "Ycode parameter error \n");
		return -1;
	}
	switch (type)
	{
	case ARGV_T:
		ret = argv2buf((argv_t *)code, buf);
		break;
	default:
		assert(0);
		break;
	}
	return ret;
}

int Ydecode(OUT YCode *code, YCodetype type, IN unsigned char *buf)
{
	int ret = 0;
	if (!code || type > YUCHAR_MAX) {
		fprintf(stderr, "Ycode parameter error \n");
		return -1;
	}
	switch (type)
	{
	case ARGV_T:
		ret = buf2argv((argv_t *)code, buf);
		break;
	default:
		assert(0);
		break;
	}
	return ret;
}

#endif

#if 0
#ifdef  __cplusplus
}
#endif
#endif