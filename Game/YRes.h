#ifndef _Y_RES_H_
#define _Y_RES_H_

#include "afx.h"

#define RES_NAMELEN 12

class Res
{
public:
	Res(){}
public:
	u8 *		get_res_name();
protected:
	
private:
	u8 _res_name[RES_NAMELEN];
	u32 _res_money;
	u32 _res_num;
	u32 _res_id;
};


#endif
