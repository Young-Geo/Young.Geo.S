#ifndef _YUSER_H_
#define _YUSER_H_

#include "xlog.h"
#include "afx.h"
#include "YRes.h"
#include "xlist.h"
#include "virtual-Game.h"
#include "Ypkt.h"
#include "global_struct.h"



#define USERNAME_LEN 12
#define PASSWORD_LEN 33
#define USER_DATA_SIZE 25
#define U_ID_INIT 100000


enum USER_STATUS {
	OFF_LINE,
	ON_LINE,
	READY,
	IN_GAME,
	IN_GAME_OFF
};

class User
{
public:
	User();
	User(u32 id, u8 *username, u8 *password, p_g thread_entity);
	User(u32 id, u8 *username, u8 *password, u32 money_z, u32 money_d, u32 solo_w, u32 solo_f, u16 solo_s, u16 lv, p_g thread_entity);
	virtual ~User();
public:
	u8 *			get_username();
	int				set_username(u8 username[USERNAME_LEN]);
	u8 *			get_password();
	int				set_password(u8 password[PASSWORD_LEN]);
	p_g 			get_thread();
	int  			set_thread(p_g thread_entity);
	USER_STATUS 	get_status();
	void			set_status(USER_STATUS user_status);
	u8 *			todata();
	int				ress_init();
	int				add_res(Res *res);
	int				set_chara(CharacterType chara);
	CharacterType   get_chara();
	//int				send_client(u8 * data, int len);
	int	&			Card_Count();
	//void			set_clientbuf(p_g buf);	
	long	&		Time();
	void			Destory();
	void			set_carddata(u8 *);
	u8*				get_carddata();
	void			addlandlord(u8 *);
protected:
	
private:
	u32 _id;
	u8 _username[USERNAME_LEN];
	u8 _password[PASSWORD_LEN];
	u8 _status;
	p_g _thread_entity;
	USER_STATUS _user_status;
private:
	u32 _money_z, _money_d;//钻和豆子
	u32 _solo_w, _solo_f;//赢和失败的局数
	u16 _solo_s;//连胜
	u16 _lv;//级数
private:
	xlist *			_res_s;
	u8 *			_data;
	CharacterType 	_chara;
	p_g				_client_buf;
	int				_chad_count;
	#define CARD_DATA_MAXSIZE 20
	#define LANDLORD_SIZE 3
	u8 				_card_data[CARD_DATA_SIZE];
private:	
	long now;
};


















#endif
