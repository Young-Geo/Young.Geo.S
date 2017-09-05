#include "YGame.h"



Game::Game(User * user_1,User * user_2,User * user_3)
{

	if (!user_1 || !user_2 || !user_3) {
		xerror("make Game error");
		return;
	}
	
	this->_user_1 = user_1;
	this->_user_2 = user_2;
	this->_user_3 = user_3;

	this->_user_1->set_status(READY);
	this->_user_2->set_status(READY);
	this->_user_3->set_status(READY);

	xsprintf((char *)this->name, "%s%s%s", user_1->get_username(), user_2->get_username(), user_3->get_username());
	
	this->cards = new deque<Card *>;
	
	if (!this->cards) {
		xerror("new vector error");
		return;
	}
	if (create_card()) {
		xerror("create_card error");
		this->destory();
		return;
	}
}

Game::~Game()
{
	this->destory();
}

int 	Game::display()
{

	if (!this->_user_1 || !this->_user_2 || !this->_user_3) {
		xerror("error user NULL\n");
		return -1;
	}
	//通知发牌等一切事物
	//改变用户相应状态

	if (this->Shuffle()) {
		//error
		return -1;
	}
	if (this->deal()) {
		//error
		return -1;
	}
	
	this->_user_1->set_status(IN_GAME);
	this->_user_2->set_status(IN_GAME);
	this->_user_3->set_status(IN_GAME);

	return 0;
}

void Game::stop()
{
	if (this->_user_1->get_status() == OFF_LINE || this->_user_1->get_status() == IN_GAME_OFF || \
		this->_user_2->get_status() == OFF_LINE || this->_user_2->get_status() == IN_GAME_OFF || \
		this->_user_3->get_status() == OFF_LINE || this->_user_3->get_status() == IN_GAME_OFF) {
		
		this->destory();
		//((global_t *)this->master)->;
		xlist_delete(((global_t *)this->master)->games, (char *)this->get_name());
		//直接销毁游戏
		delete this;
		return;
	}

	//如果都在则更改状态等待下次开始
	this->_user_1->set_status(READY);
	this->_user_2->set_status(READY);
	this->_user_3->set_status(READY);
}

u8 * 	Game::get_name()
{
	return this->name;
}

int Game::create_card()
{
	int color, value;
	Weight w;
	Suits s;
	Card *card = NULL;

	if (!this->cards) {
		xerror("cards NULL");
		return -1;
	}
	
    for (color = 0; color < 4; color++)
    {
        for (value = 0; value < 13; value++)
        {
            w = (Weight)value;
            s = (Suits)color;
            card = new Card(w, s);
			if (NULL == card)
				return -1;
            this->cards->push_back(card);
        }
	}
	
	card = new Card(SJoker, None);
	if (NULL == card)
		return -1;
	this->cards->push_back(card);
	
	card = new Card(LJoker, None);
	if (NULL == card)
		return -1;
	this->cards->push_back(card);
        //创建大小joker
	return 0;
}

void Game::destory()
{
	int i = 0;
	Card *card = NULL;
	if (this->cards) {
		while (!this->cards->empty())
		{
			card = this->cards->front();
			if (!card)
				continue;
			delete card;
			this->cards->pop_front();
		}
		delete this->cards;
		this->cards = NULL;
	}
}

int Game::get_car_count()
{
	if (!this->cards)
		return -1;
	return this->cards->size();
}

int Game::Shuffle()
{
	deque<Card*> newque(CARDCOUNT);
	deque<Card*>::iterator it;
	int ran = 0;
	
    if (CARDCOUNT != this->get_car_count()) {
		xerror("Cardcount error");
		return -1;
    }

	
	xmessage("shuffle newque count %d", newque.size());
	
    newque.clear();
	for (it = this->cards->begin(); it != this->cards->end(); ++it)
	{
		srand((unsigned)time(NULL));
		ran = rand() % CARDCOUNT;
		xmessage("rand %d", ran);
		//newque.insert((newque.begin() + (rand() % CARDCOUNT)), (*it));
		newque.insert((newque.begin() + ran), (*it));
	}

	xmessage("shuffle newque count %d", newque.size());

    this->cards->clear();
	
	xmessage("shuffle cards count %d", this->cards->size());

    for (it = newque.begin(); it != newque.end(); ++it)
	{
		this->cards->push_back(*it);
	}
	xmessage("shuffle cards count %d", this->cards->size());
	
    newque.clear();

	return 0;
}

int		Game::deal()
{
	int i = 0;
	Card *card = NULL;
	
	if (!this->cards || !this->_user_1 || !this->_user_2 || !this->_user_3) {
		xerror("deal NULL error");
		return -1;
	}
	xmessage("deal card count = %d", this->cards->size());

	for (i = 0; i < (CARDCOUNT - LAST_CARD); ++i)
	{
		card = (*this->cards)[i];
		//card = this->cards->at(i);
		if (!card) {
			xerror("deal error");
			return -1;
		}
		
		switch ((i % CHARACTERCOUNT))
		{
			case U1:
				card->set_user(this->_user_1);
			break;

			case U2:
				card->set_user(this->_user_2);
			break;
			
			case U3:
				card->set_user(this->_user_3);
			break;

			default:
				//error
			break;
		}
	}

	return 0;
}

#define UDATA 17

int 	Game::data2user()
{	
	deque<Card*>::iterator it;
	User *user= NULL;
	Card *card = NULL;
	u8 dd1[UDATA] = { 0 }, dd2[UDATA] = { 0 }, dd3[UDATA] = { 0 };
	u8 *d1 = NULL, *d2 = NULL, *d3 = NULL;
	
	XXNULL(this->cards, -1);
	XXNULL(this->_user_1, -1);
	XXNULL(this->_user_2, -1);
	XXNULL(this->_user_3, -1);

	d1 = dd1;
	d2 = dd2;
	d3 = dd3;
	
	for (it = this->cards->begin(); it != this->cards->end(); ++it)
	{
		card = (*it);
		xassert(card);
		user = card->get_user();
		xassert(user);

		if (user == this->_user_1) {
			OUT8(d1, card->to_data());
		}
		
		if (user == this->_user_2) {
			OUT8(d2, card->to_data());
		}
		
		if (user == this->_user_2) {
			OUT8(d3, card->to_data());
		}
	}
	
	this->_user_1->set_carddata(dd1);
	this->_user_2->set_carddata(dd2);
	this->_user_3->set_carddata(dd3);

	return 0;
}


User *	Game::get_user_by_name(unsigned char *username)
{
	if (!this->_user_1 || !_user_2 || !_user_3) {
		xerror("error usre null");
		return NULL;
	}

	if (ZERO == xstrncmp((char *)this->_user_1->get_username(), (char *)username, USERNAME_LEN))
		return this->_user_1;
	if (ZERO == xstrncmp((char *)this->_user_2->get_username(), (char *)username, USERNAME_LEN))
		return this->_user_2;
	if (ZERO == xstrncmp((char *)this->_user_3->get_username(), (char *)username, USERNAME_LEN))
		return this->_user_3;
}
