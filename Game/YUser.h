#ifndef _YUSER_H_
#define _YUSER_H_

#include "xlog.h"
#include "afx.h"

#define USERNAME_LEN 12
#define PASSWORD_LEN 32


enum USER_STATUS{
	OFF_LINE,
	ON_LINE,
	READY,
	IN_GAME
};

class User
{
public:
	User();
	User(u8 *username, u8 *password);
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
	//username,password
	u32 id;
	u8 username[12];
	u8 password[32];
	u8 status;
	p_g _thread_entity;
	USER_STATUS _user_status;
};


















#endif
