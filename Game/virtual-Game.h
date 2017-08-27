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
///	��ɫ
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
/// ��ɫ
/// </summary>
typedef enum _Suits {
    Club = 0,
    Diamond,
    Heart,
    Spade,
    None
} Suits;

/// <summary>
/// ����Ȩֵ
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
/// ��������
/// </summary>
typedef enum _CardsType {
    //δ֪����
    none_type = 0,
    //��ը
    JokerBoom,
    //ը��
    Boom,
    //��������
    OnlyThree,
    //������һ
    ThreeAndOne,
    //��������
    ThreeAndTwo,
    //˳�� ���Ż�������������
    Straight,
    //˫˳ ���Ի�������������
    DoubleStraight,
    //��˳ ��������������������
    TripleStraight,
    //����
    Double,
    //����
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
