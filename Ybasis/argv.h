#include "Ybasis.h"

#if 0
#ifdef  __cplusplus
extern "C" {
#endif
#endif

#ifndef __YARGV__H_
#define __YARGV__H_
#define ARGV_COUNT 1024
typedef struct _argv_t
{
	u_16 type, count;
	int argv[ARGV_COUNT];
	u_32 datasize;
	u_8 *data;
} argv_t;

int argv2buf(IN argv_t *argv, OUT unsigned char **buf)
{
	int i;
	u_32 argv_count = 0;
	u_8 *data = NULL, *temp;
	if (!argv || !buf || !argv->count) {
		fprintf(stderr, "argv2buf parameter error\n");
		return -1;
	}
	argv_count = sizeof(u_16) + sizeof(u_16);
	argv_count += sizeof(int) * argv->count;
	argv_count += sizeof(u_32);
	argv_count += argv->datasize;
	data = (u_8 *)malloc(sizeof(u_8) * argv_count);
	if (!data) {
		fprintf(stderr, "argv2buf malloc error\n");
		return argv_count;
	}
	temp = data;
	OUT16(data, argv->type);
	OUT16(data, argv->count);
	for (i = 0; i < argv->count; ++i)
	{
		OUT32(data, argv->argv[i]);
	}
	OUT32(data, argv->datasize);
	if (argv->datasize > 0)
		memcpy(data, argv->data, argv->datasize);
	*buf = temp;
	return argv_count;
}

int buf2argv(OUT argv_t *argv, IN unsigned char *buf)
{
	int ret = 0, i;
	u_16 v16;
	u_32 v32;
	unsigned char *temp = NULL;
	if (!argv || !buf) {
		fprintf(stderr, "argv2buf parameter error\n");
		return -1;
	}
	temp = buf;

	IN16(temp, v16);
	argv->type = v16;
	IN16(temp, v16);
	if (!v16) {
		fprintf(stderr, "argv2buf parsing error\n");
		return -1;
	}
	argv->count = v16;
	for (i = 0; i < argv->count; ++i)
	{
		IN32(temp, argv->argv[i]);
	}
	IN32(temp, v32);
	if (v32 > 0) {
		argv->data = (u_8 *)malloc(sizeof(u_8) * v32 + 1);
		if (!argv->data)
			return -1;
		memcpy(argv->data, temp, v32);
		argv->datasize = v32;
	}
	return 0;
}


#endif

#if 0
#ifdef  __cplusplus
}
#endif
#endif