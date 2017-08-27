#include "YCard.h"

Card::Card(Weight weight, Suits color)
{
	if (weight < Three || weight > LJoker || color < Club || color > None) {
		return;
	}
	
	this->_weight = weight;
	this->_color = color;
	
	this->set_CardName();
}

u8 * Card::GetCardName()
{
	if (ZERO == this->_cardName[ZERO])
		return NULL;
	return this->_cardName;
}

void Card::set_CardName()
{
	char *color_name[SUITSCOUNT] = {"Club", "Diamond", "Heart", "Spade", ""};
	char *weight_name[WEIGHTCOUNT] = {
		"Three", "Four", "Five", "Six",
		"Seven", "Eight", "Nine", "Ten",
		"Jack", "Queen", "King", "One"
		"Two", "SJoker", "LJoker"
	};
	
	xzero(this->_cardName, CARD_NAME_LEN);
	xstrcpy((char *)this->_cardName, color_name[this->_color]);
	xstrcat((char *)this->_cardName, weight_name[this->_weight]);
}

Weight Card::GetCardWeight()
{
	return this->_weight;
}

Suits Card::GetCardSuit()
{
	return this->_color;
}

/*
void Card::set_attribution(CharacterType belongTo)
{
	this->_belongTo = belongTo;
}

CharacterType Card::get_attribution()
{
	return this->_belongTo;
}
*/

int 			Card::set_user(User *user)
{
	this->_user = user;
}
User *			Card::get_user()
{
	return this->_user;
}
















