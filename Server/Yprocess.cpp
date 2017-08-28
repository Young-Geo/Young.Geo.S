#include "Yprocess.h"



int work(void *arg, xchain *rchain, xchain *wchain)
{
	return 0;
}

p_g login(thread_entity_t *thread_entity, void *client_buf, u8 *username, u8 *password)
{
	char sql[1024] = { 0 };
	int id, solo_f, solo_w, solo_s, lv, money_d, money_z;
	unsigned long user_len = 0, pass_len = 0;
	if (!thread_entity || !username || !password) {
		xerror("login error NULL\n");
		return NULL;
	}

	xstrcpy(sql, "SELECT ID, SOLO_FAIL, SOLO_W, SOLO_S, LV, MONEY_D, MONEY_Z FROM user WHERE USERNAME = ? and PASSWORD = ? ");
	if (!thread_entity->coc.Prepare(sql)) {
		xerror("prepare error %s", thread_entity->coc.GetErrorMessage());
		return NULL;
	}
	user_len = xstrlen((char *)username);
	pass_len = xstrlen((char *)password);
	thread_entity->coc.BindString(1, (char *)username, &user_len);//绑定输入结果
	thread_entity->coc.BindString(2, (char *)password, &pass_len);
	thread_entity->coc.BindFinish();

	thread_entity->coc.DefineInt(1, &id);//绑定输出结果
	thread_entity->coc.DefineInt(2, &solo_f);	
	thread_entity->coc.DefineInt(3, &solo_w);
	thread_entity->coc.DefineInt(4, &solo_s);
	thread_entity->coc.DefineInt(5, &lv);
	thread_entity->coc.DefineInt(6, &money_d);
	thread_entity->coc.DefineInt(7, &money_z);
	thread_entity->coc.DefineFinish();
	
	if (!thread_entity->coc.ExecQuery()) {
		xerror("select data error %s", thread_entity->coc.GetErrorMessage());
		return NULL;
	}
	if (!thread_entity->coc.Fetch()) {
		xerror("select data error %s", thread_entity->coc.GetErrorMessage());
		return NULL;
	}
	//登录处理数据查询
	//if ()
	//登录成功
	User *user = new User(id, username, password, money_z, money_d, solo_w, solo_f, solo_s, lv, (p_g)thread_entity);
	if (!user) {
		xerror("new User error");
  		return NULL;
	}
	user->set_clientbuf((p_g)client_buf);
	pthread_mutex_lock(&thread_entity->mutex_users);
	xlist_add(thread_entity->users, (const char *)user->get_username(), XLIST_CPP, (char *)user);			
	pthread_mutex_unlock(&thread_entity->mutex_users);

	//查询其他信息物品等
	return user->todata();
}

int rigister(thread_entity_t *thread_entity, u8 *username, u8 *password)
{
	char sql[1024] = { 0 };
	unsigned long user_len = 0, pass_len = 0;
	int id = 0;
	
	if (!thread_entity || !username || !password) {
		xerror("register error NULL\n");
		return -1;
	}

	xstrcpy(sql, "INSERT INTO user(USERNAME, PASSWORD) VALUES(?, ?)");
	if (!thread_entity->coc.Prepare(sql)) {
		xerror("prepare error %s", thread_entity->coc.GetErrorMessage());
		return -1;
	}
	
	user_len = xstrlen((char *)username);
	pass_len = xstrlen((char *)password);
	
	thread_entity->coc.BindString(1, (char *)username, &user_len);	
	thread_entity->coc.BindString(2, (char *)password, &pass_len);
	thread_entity->coc.BindFinish();
	if (!thread_entity->coc.Execute()) {
		xerror("insert data error %s", thread_entity->coc.GetErrorMessage());
		return -1;
	}

	/*
	User *user = new User(id, username, password, (p_g)thread_entity);
	if (!user) {
		xerror("new User error");
  		return -1;
	}

	
	pthread_mutex_lock(&thread_entity->mutex_users);
	xlist_add(thread_entity->users, (const char *)user->get_username(), XLIST_STRING, (char *)user);			
	pthread_mutex_unlock(&thread_entity->mutex_users);
	*/
	return 0;
}


int ready_start(xlist **users, u8 inx, xlist *readys, xlist *games, u8 *username)
{
	User *user = NULL, *user_1 = NULL, *user_2 = NULL;
	int i = 0, size = 0;
	xlist *t_list = NULL;
	Game *game = NULL;
	u8 game_name[USERNAME_LEN * 3];

	if (!users || !readys || !games || !username) {
		xerror("ready_start error NULL\n");
		return -1;
	}
	
	user = (User *)xlist_getv(users[inx], (char *)username);//分线程找，，后续需要沟通
	if (!user) {
		xerror("no find user\n");
		return -1;
	}

	if (xlist_size(readys) < 2) {
		xmessage("%s add readys \n", username);
		xlist_add(readys, (char *)username, XLIST_PTR, (char *)user);
		return 0;
	}
	
	user_1 = (User *)xlist_index(readys, 0)->value;
	user_2 = (User *)xlist_index(readys, 1)->value;
	if (!user_1 || !user_2) {
		xmessage("%s add readys \n", username);
		xlist_add(readys, (char *)username, XLIST_PTR, (char *)user);
		return 0;
	}
	xlist_delete(readys, (char *)user_1->get_username());	
	xlist_delete(readys, (char *)user_2->get_username());

	game = new Game(user, user_1, user_2);
	if (!game) {
		xerror("new Game error\n");
		return -1;
	}
	xlist_add(games, (char *)game_name, XLIST_STRING, (char *)game);
	return 0;
}


int do_work(void *arg, void *U_buf, void *r, void *w)
{
	thread_entity_t *thread_entity = NULL;
	xchain *rchain = NULL, *wchain = NULL;
	unsigned char buf[1024], *data = NULL;
	unsigned short type = 0;
	u8 username[USERNAME_LEN], password[PASSWORD_LEN];
	u8 inx = 0;
	int len = 0;


	assert(arg);
	assert(r);
	assert(w);


	thread_entity = (thread_entity_t *)arg;
	rchain = (xchain *)r, wchain = (xchain *)w;

	xchain_get(rchain, (void *)&type, 2);
	xchain_delete(rchain, 2);
	
	switch (type)//各种业务处理
	{
		case LOGIN:
			{
				#define REC_LEN  3
				u8 rec[REC_LEN + USER_DATA_SIZE] = {0}, *buf = NULL, *u_buf = NULL;
				u8 rec_inx = 0;
				
				xchain_get(rchain, (void *)username, USERNAME_LEN);	
				xchain_delete(rchain, USERNAME_LEN);
				xchain_get(rchain, (void *)password, PASSWORD_LEN);
				xchain_delete(rchain, PASSWORD_LEN);

				if (!(u_buf = (u8 *)login(thread_entity, U_buf, username, password))) {
					xerror("login error\n");					
					rec_inx = 0;
				} else {
					rec_inx = thread_entity->inx + 1;
				}
				xmessage("login username = %s, password = %s\n", username, password);
				buf = rec;
				OUT16_LE(buf, REC_LOGIN);
				OUT8(buf, rec_inx);
				if (rec_inx) {
					xmemcpy(buf, u_buf, USER_DATA_SIZE);
					buf += USER_DATA_SIZE;
				}
				xchain_add(wchain, (void *)rec, (buf-rec));
			}
		break;

		case MATCH://就绪状态，应该先找同种状态的人看看能不能匹配游戏
			{
				xchain_get(rchain, (void *)username, USERNAME_LEN);	
				xchain_get(rchain, (void *)&inx, 1);
				if (ready_start(thread_entity->master->arr_users, inx, thread_entity->master->readys, thread_entity->master->games, username)) {
					xerror("ready_start error\n");
				}
			}
		break;

		case REGISTER:
			{
				unsigned char rec[REC_LEN] = {0}, *buf = NULL;
				u8 rec_inx = 0;
				
				xchain_get(rchain, (void *)username, USERNAME_LEN);	
				xchain_delete(rchain, USERNAME_LEN);
				xchain_get(rchain, (void *)password, PASSWORD_LEN);
				xchain_delete(rchain, PASSWORD_LEN);

				if (rigister(thread_entity, username, password)) {
					xerror("register error\n");					
					rec_inx = 0;
				} else {
					rec_inx = thread_entity->inx + 1;
				}
				xmessage("register username = %s, password = %s\n", username, password);
				buf = rec;
				OUT16_LE(buf, REC_REGISTER);	
				OUT8(buf, rec_inx);
				xchain_add(wchain, (void *)rec, (buf-rec));
			}
		break;
		
		default:
			
		break;
	}
	
	return 0;
}
























