#ifndef __VIRTUAL_GAME_H_
#define __VIRTUAL_GAME_H_

#include "xlog.h"
#include "afx.h"

#define SUITSCOUNT 4
#define WEIGHTCOUNT 15

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

/*
/// <summary>
/// ���
/// </summary>
enum Identity {
    Farmer,
    Landlord
};*/

/// <summary>
/// ��������
/// </summary>
enum CardsType {
    //δ֪����
    none_type,
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
};


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
