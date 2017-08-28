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
	
	xsprintf((char *)this->name, "%s%s%s", user_1->get_username(), user_2->get_username(), user_3->get_username());

	this->cards = new deque<Card *>();
	
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
	this->data2client();

	this->_user_1->set_status(IN_GAME);
	this->_user_2->set_status(IN_GAME);
	this->_user_3->set_status(IN_GAME);
	return 0;
}

int Game::stop()
{
	return 0;
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
	deque<Card*> newque;
	deque<Card*>::iterator it;
	
    if (CARDCOUNT != this->get_car_count()) {
		xerror("Cardcount error");
		return -1;
    }
	
	for (it = this->cards->begin(); it != this->cards->end(); ++it)
	{
		srand((unsigned)time(NULL));
		newque.insert((newque.begin() + (rand() % CARDCOUNT)), (*it));
	}

    this->cards->clear();

    for (it = newque.begin(); it != newque.end(); ++it)
	{
		this->cards->push_back(*it);
	}
	
    newque.clear();

	return 0;
}

int		Game::deal()
{
	int i = 0;
	Card *card = NULL;

	if (!this->cards || !this->_user_1 || !this->_user_2 || !this->_user_3) {
		xerror("deal error");
		return -1;
	}
	
	for (i = 0; i < (CARDCOUNT - LAST_CARD); ++i)
	{
		card = (*this->cards)[i];
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

int 	Game::data2client()
{	
	deque<Card*>::iterator it;
	Card *card = NULL;
	u8 *d1 = NULL, *d2 = NULL, *d3 = NULL;
	XXNULL(this->cards, -1);	
	XXNULL(this->_user_1, -1);
	XXNULL(this->_user_2, -1);
	XXNULL(this->_user_3, -1);

	d1 = (u8 *)xmalloc(UDATA);	
	d2 = (u8 *)xmalloc(UDATA);
	d3 = (u8 *)xmalloc(UDATA);
	
	XXNULL(d1, -1);
	XXNULL(d2, -1);
	XXNULL(d3, -1);
	
	for (it = this->cards->begin(); it != this->cards->end(); ++it)
	{
		card = (*it);
		switch ((unsigned int)card->get_user())
		{
			case (unsigned int)this->_user_1:
				OUT8(d1, card->to_data());
			break;
			
			case (unsigned int)this->_user_2:				
				OUT8(d2, card->to_data());
			break;

			case (unsigned int)this->_user_3:				
				OUT8(d3, card->to_data());
			break;
		}
	}
	
	this->_user_1->send_client(d1, UDATA);
	this->_user_2->send_client(d2, UDATA);
	this->_user_3->send_client(d3, UDATA);

	xfree(d1);
	xfree(d2);
	xfree(d3);
	return 0;
}

