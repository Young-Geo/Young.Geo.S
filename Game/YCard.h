#ifndef _YCARD_H_
#define _YCARD_H_

#include "afx.h"
#include "virtual-Game.h"

#define CARD_NAME_LEN 12

class Card
{
public:
	Card(Weight weight, Suits color, CharacterType belongTo);
	Card(Weight weight, Suits color);
	
public:
	u8 * GetCardName();
	Weight GetCardWeight();
	Suits GetCardSuit();
	void set_attribution(CharacterType);
	CharacterType get_attribution();
	
protected:
private:
	void set_CardName();
private:
	u8 _cardName[CARD_NAME_LEN];
    Weight _weight;
    Suits _color;
    CharacterType _belongTo;
};


#endif
