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
	char *color_name[SUITSCOUNT] = {(char *)"Club", (char *)"Diamond", (char *)"Heart", (char *)"Spade", (char *)""};
	char *weight_name[WEIGHTCOUNT] = {
		(char *)"Three", (char *)"Four", (char *)"Five", (char *)"Six",
		(char *)"Seven", (char *)"Eight", (char *)"Nine", (char *)"Ten",
		(char *)"Jack", (char *)"Queen", (char *)"King", (char *)"One",
		(char *)"Two", (char *)"SJoker", (char *)"LJoker"
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

u8				Card::to_data()
{
	u8 data = 0;
	/*data = xmalloc(CARD_DATA_SIZE);
	XXNULL(data, NULL);
	this->_data = data;

	OUT8(data, this->GetCardSuit());
	OUT8(data, this->GetCardWeight());
	*/
	data = this->GetCardSuit();
	data <<= 4;
	data = (data & 0xf0) | (this->GetCardWeight() & 0x0f);
	return data;
}














