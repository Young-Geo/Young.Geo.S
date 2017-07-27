#include "Yprocess.h"



int work(void *arg, xchain *rchain, xchain *wchain)
{
	return 0;
}

int login(thread_entity_t *thread_entity, u8 *username, u8 *password)
{
	if (!thread_entity || !username || !password) {
		xerror("login error NULL\n");
		return -1;
	}
	//登录处理数据查询
	//if ()
	//登录成功
	User *user = new User(username, password, (p_g)thread_entity);
	user->set_status(ON_LINE);
	user->set_thread(thread_entity);
	pthread_mutex_lock(&thread_entity->mutex_users);
	xlist_add(thread_entity->users, (const char *)user->get_username(), XLIST_STRING, (char *)user);			
	pthread_mutex_unlock(&thread_entity->mutex_users);
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

	sprintf((char *)game_name, "%s%s%s", user->get_username(), user_1->get_username(), user_2->get_username());
	game = new Game(user, user_1, user_2, game_name);
	if (!game) {
		xerror("new Game error\n");
		return -1;
	}
	xlist_add(games, (char *)game_name, XLIST_STRING, (char *)game);
	return 0;
}


int do_work(void *arg, void *r, void *w)
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
				#define REC_LEN 9
				unsigned char rec[REC_LEN] = {0}, *buf = NULL, roc;
				u8 rec_inx = 0;
				
				xchain_get(rchain, (void *)username, USERNAME_LEN);	
				xchain_delete(rchain, USERNAME_LEN);
				xchain_get(rchain, (void *)password, PASSWORD_LEN);
				xchain_delete(rchain, PASSWORD_LEN);

				if (login(thread_entity, username, password)) {
					xerror("login error\n");					
					rec_inx = 0;
				} else {
					rec_inx = thread_entity->inx + 1;
				}
				xmessage("login ok username = %s, password = %s\n", username, password);
				buf = rec;
				pkt_build_byte_tag(buf, PKT_YS_START_TAG);	
				pkt_build_byte_tag(buf, 0);				
				pkt_build_byte_tag(buf, PKT_YS_FRAME_TYPE);				
				pkt_build_short_tag(buf, REC_LEN);				
				pkt_build_short_tag(buf, REC_LOGIN);
				pkt_build_byte_tag(buf, (unsigned char)rec_inx);				
				pkt_build_byte_tag(buf, PKT_YS_END_TAG);
				roc = pkt_build_check_sum(rec, REC_LEN);
				buf = &rec[1];
				pkt_build_byte_tag(buf, roc);	
				xchain_add(wchain, (void *)rec, REC_LEN);
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

		case 'S':
			{
				
			}
		break;
		
		default:
			
		break;
	}
	
	return 0;
}
























