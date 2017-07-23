#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "afx/afx.h"
#include "afx/xchain.h"
#include "global_struct.h"


int work(void *arg, xchain *rhain, xchain *whain);

int do_work(void *arg, void *r, void *w);


#endif
