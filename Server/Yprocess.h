#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "afx.h"
#include "xchain.h"
#include "xlist.h"
#include "global_struct.h"
#include "YGame.h"
#include "YUser.h"
#include "YS.h"

enum Business_type {
	LOGIN = 1,
	MATCH,
	REC_LOGIN,
	REGISTER,
	REC_REGISTER,
	REC_MATCH
};

int work(void *arg, xchain *rhain, xchain *whain);

int do_work(void *arg, void *r, void *w);

#endif
