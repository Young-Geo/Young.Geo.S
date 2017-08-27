#include "YUser.h"


User::User()
{
	this->_user_status = ON_LINE;
	this->ress_init();
}

User::User(u32 id, u8 *username, u8 *password, p_g thread_entity)
{
	if (!username || !password) {
		xerror("User username password NULL\n");
		return;
	}

	xmemcpy(this->_username, username, xstrlen((char *)username));
	xmemcpy(this->_password, password, xstrlen((char *)password));

	this->_id = id;
	this->_money_z = ZERO;
	this->_money_d = ZERO;
	this->_solo_w = ZERO;
	this->_solo_f = ZERO;
	this->_solo_s = ZERO;
	this->_lv = ZERO;
	this->_user_status = ON_LINE;
	this->_thread_entity = thread_entity;
	
	this->ress_init();
	
	return;
}

User::User(u32 id, u8 *username, u8 *password, u32 money_z, u32 money_d, u32 solo_w, u32 solo_f, u16 solo_s, u16 lv, p_g thread_entity)
{
	if (!username || !password || id < U_ID_INIT) {
		xerror("new User username password NULL\n");
		return;
	}
	
	xmemcpy(this->_username, username, xstrlen((char *)username));
	xmemcpy(this->_password, password, xstrlen((char *)password));
	
	this->_id = id;
	this->_money_z = money_z;
	this->_money_d = money_d;
	this->_solo_w = solo_w;
	this->_solo_f = solo_f;
	this->_solo_s = solo_s;
	this->_lv = lv;
	this->_user_status = ON_LINE;
	this->_thread_entity = thread_entity;
	
	this->ress_init();

}

User::~User()
{
	if (this->_res_s)
		xlist_clean(&this->_res_s);
	if (this->_data)
		delete this->_data;
}



u8 *			User::get_username()
{
	return this->_username;
}

int 			User::set_username(u8 *username)
{

	if (!username) {
		xerror("set_username username NULL\n");
		return -1;
	}

	xmemcpy(this->_username, username, strlen((char *)username));
	return 0;
}

u8 *			User::get_password()
{
	return this->_password;
}
int 			User::set_password(u8 *password)
{
	if (!password) {
		xerror("set_password password NULL\n");
		return -1;
	}

	xmemcpy(this->_password, password, strlen((char *)password));
	return 0;
}


p_g User::get_thread()
{
	return this->_thread_entity;
}
int  User::set_thread(p_g thread_entity)
{
	if (!thread_entity) {
		xerror("User set_thread error \n");
		return -1;
	}
	this->_thread_entity = thread_entity;
	return 0;
}

USER_STATUS 	User::get_status()
{
	return this->_user_status;
}
void			User::set_status(USER_STATUS user_status)
{
	this->_user_status = user_status;
}

u8 *			User::todata()
{
	u8 *data = NULL, *out_data = NULL;
	data = (u8 *)xmalloc(USER_DATA_SIZE);
	XXNULL(data, NULL);
	this->_data = out_data = data;
	
	if (this->_id < U_ID_INIT || ZERO == this->_user_status) {
		xerror("user todata error");
		return NULL;
	}
	
	OUT32_LE(data, this->_id);
	OUT8(data, this->_user_status);
	OUT32_LE(data, this->_money_z);
	OUT32_LE(data, this->_money_d);
	OUT32_LE(data, this->_solo_w);
	OUT32_LE(data, this->_solo_f);
	OUT16_LE(data, this->_solo_s);
	OUT16_LE(data, this->_lv);
	
	return out_data;
}

int 			User::ress_init()
{
	this->_res_s = xlist_init();
	if (!this->_res_s) {
		xerror("res_s init error");
		return -1;
	}
	return 0;
}

int 			User::add_res(Res * res)
{
	if (!res || !this->_res_s) {
		xerror("add res error");
		return -1;
	}
	xlist_add(this->_res_s, (char *)res->get_res_name(), XLIST_CPP, (char *)res);
	return 0;
}

int 			User::set_chara(CharacterType chara)
{
	this->_chara = chara;
}

CharacterType	User::get_chara()
{
	return this->_chara;
}






