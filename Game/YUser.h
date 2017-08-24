#ifndef _YUSER_H_
#define _YUSER_H_

#include "xlog.h"
#include "afx.h"

#define USERNAME_LEN 12
#define PASSWORD_LEN 32


enum USER_STATUS {
	OFF_LINE,
	ON_LINE,
	READY,
	IN_GAME
};

class User
{
public:
	User();
	User(u8 *username, u8 *password, p_g thread_entity);
	~User();
public:
	u8 *			get_username();
	int				set_username(u8 username[12]);
	u8 *			get_password();
	int				set_password(u8 password[32]);
	p_g 			get_thread();
	int  			set_thread(p_g thread_entity);
	USER_STATUS 	get_status();
	void			set_status(USER_STATUS user_status);
protected:
	
private:
	u32 id;
	u8 username[12];
	u8 password[32];
	u8 status;
	p_g _thread_entity;
	USER_STATUS _user_status;
private:
	u32 money_z, money_d;//钻和豆子
	u32 solo_w, solo_f;//赢和失败的局数
	u16 solo_s;//连胜
	u16 lv;//级数
};


















#endif
