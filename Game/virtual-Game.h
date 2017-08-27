#ifndef __VIRTUAL_GAME_H_
#define __VIRTUAL_GAME_H_

#include "xlog.h"
#include "afx.h"

#define SUITSCOUNT 5
#define WEIGHTCOUNT 15
#define CARDCOUNT 54
#define CHARACTERCOUNT 3
#define LAST_CARD 3

/// <summary>
///	角色
/// </summary>
typedef enum _CharacterType {
    Library = 0,
    FarmerOne,
    FarmerTwo,
    Landlord,
    Desk
} CharacterType;

enum USER_TAG {
	U1,
	U2,
	U3
};

/// <summary>
/// 花色
/// </summary>
typedef enum _Suits {
    Club = 0,
    Diamond,
    Heart,
    Spade,
    None
} Suits;

/// <summary>
/// 卡牌权值
/// </summary>
typedef enum _Weight {
    Three = 0,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King,
    One,
    Two,
    SJoker,
    LJoker,
} Weight;

/// <summary>
/// 出牌类型
/// </summary>
typedef enum _CardsType {
    //未知类型
    none_type = 0,
    //王炸
    JokerBoom,
    //炸弹
    Boom,
    //三个不带
    OnlyThree,
    //三个带一
    ThreeAndOne,
    //三个带二
    ThreeAndTwo,
    //顺子 五张或更多的连续单牌
    Straight,
    //双顺 三对或更多的连续对牌
    DoubleStraight,
    //三顺 二个或更多的连续三张牌
    TripleStraight,
    //对子
    Double,
    //单个
    Single
} CardsType;


class virtual_Game {
public:
	virtual_Game(){}
	~virtual_Game(){}
public:
	virtual int display() = 0;
	virtual int stop() = 0;
	virtual u8 *get_name() = 0;
};

#endif
