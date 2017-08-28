#ifndef _YCARD_H_
#define _YCARD_H_

#include "afx.h"
#include "virtual-Game.h"
#include "YUser.h"

#define CARD_NAME_LEN 12
#define CARD_DATA_SIZE 2

class Card
{
public:
	Card(Weight weight, Suits color);
	
public:
	u8 * 			GetCardName();
	Weight 			GetCardWeight();
	Suits 			GetCardSuit();
	int				set_user(User *user);	
	User *			get_user();
	u8				to_data();
	/*
	void set_attribution(CharacterType);
	CharacterType get_attribution();
	*/
protected:
private:
	void set_CardName();
private:
	u8 			_cardName[CARD_NAME_LEN];
    Weight 		_weight;
    Suits 		_color;
    User *		_user;
	//char *		_data;
};


#endif
