//==============================================================================
/**
 * @file    gds_battle_mode.h
 * @brief   戦闘録画モードのbit定義
 * @author  matsuda
 * @date    2010.03.21(日)
 */
//==============================================================================
#pragma once


//==============================================================================
//  定数定義
//==============================================================================
//--------------------------------------------------------------
//  バトルモード
//--------------------------------------------------------------
// 「シングル」「ダブル」「トリプル」「ローテーション」「マルチ」
#define BATTLEMODE_TYPE_BIT 0

//「通信」か「サブウェイ」か。
//通信・サブウェイ、どちらのランキングに含めるかは、こちらを利用します。
#define BATTLEMODE_COMMUNICATION_BIT 3

//「通信」の場合は通信のタイプ、「サブウェイ」の場合は０
#define BATTLEMODE_COMMMODE_BIT 4

// 「ランダムマッチ」の場合、「フリー」か「レーティング戦」か
#define BATTLEMODE_RATING_BIT 6

// シューターが有か無しか
#define BATTLEMODE_SHOOTER_BIT 7

// レベルレギュレーション（フリー、スタンダード、レベル５０）
#define BATTLEMODE_REGULATION_BIT 8

typedef enum{
	BATTLEMODE_TYPE_SINGLE = 0,
	BATTLEMODE_TYPE_DOUBLE,
	BATTLEMODE_TYPE_TRIPLE,
	BATTLEMODE_TYPE_ROTATION,
	BATTLEMODE_TYPE_MULTI
} BATTLEMODE_TYPE;

typedef enum{
	BATTLEMODE_COMMUNICATION_SUBWAY = 0,
	BATTLEMODE_COMMUNICATION_COMMUNICATION = 1
} BATTLEMODE_COMMUNICATION;

typedef enum{
	BATTLEMODE_COMMMODE_SUBWAY = 0,
	BATTLEMODE_COMMMODE_COLOSSEUM,
	BATTLEMODE_COMMMODE_RANDOM,
	BATTLEMODE_COMMMODE_CHAMPIONSHIP
} BATTLEMODE_COMMMODE;

typedef enum {
	BATTLEMODE_RATING_FREE = 0,
	BATTLEMODE_RATING_RATING
} BATTLEMODE_RATING;

typedef enum {
	BATTLEMODE_SHOOTER_OFF = 0,
	BATTLEMODE_SHOOTER_ON
} BATTLEMODE_SHOOTER;

typedef enum {
	BATTLEMODE_REGULATION_FREE = 0,
	BATTLEMODE_REGULATION_STANDARD,
	BATTLEMODE_REGULATION_LEVEL50
} BATTLEMODE_REGULATION;

#define BATTLEMODE_TYPE_MASK 0x7
#define BATTLEMODE_COMMUNICATION_MASK 0x8

//「通信」の場合は通信のタイプ、「サブウェイ」の場合は０
#define BATTLEMODE_COMMMODE_MASK 0x30

// 「ランダムマッチ」の場合、「フリー」か「レーティング戦」か
#define BATTLEMODE_RATING_MASK 0x40

// シューターが有か無しか
#define BATTLEMODE_SHOOTER_MASK 0x80

// レベルレギュレーション（フリー、スタンダード、レベル５０）
#define BATTLEMODE_REGULATION_MASK 0x300


//--------------------------------------------------------------
//  検索条件”指定なし”の定義
//--------------------------------------------------------------
///ポケモン指定無し
#define BATTLE_REC_SEARCH_MONSNO_NONE     (0xffff)
///国コード指定無し
#define BATTLE_REC_SEARCH_COUNTRY_CODE_NONE   (0xff)
///地方コード指定無し
#define BATTLE_REC_SEARCH_LOCAL_CODE_NONE   (0xff)



//--------------------------------------------------------------
//  
//--------------------------------------------------------------
//BATTLEMODE_SEARCH_NO_COLOSSEUM_SINGLE_NOSHOOTER,    ///<コロシアム シングル シューター無し
#define BATTLEMODE_BIT_COLOSSEUM_SINGLE_NOSHOOTER    ((BATTLEMODE_TYPE_SINGLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_OFF << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_SINGLE_NOSHOOTER    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)


//BATTLEMODE_SEARCH_NO_COLOSSEUM_SINGLE_SHOOTER,    ///<コロシアム シングル シューターあり
#define BATTLEMODE_BIT_COLOSSEUM_SINGLE_SHOOTER   ((BATTLEMODE_TYPE_SINGLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_ON << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_SINGLE_SHOOTER    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_COLOSSEUM_DOUBLE_NOSHOOTER,    ///<コロシアム ダブル シューター無し
#define BATTLEMODE_BIT_COLOSSEUM_DOUBLE_NOSHOOTER    ((BATTLEMODE_TYPE_DOUBLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_OFF << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_DOUBLE_NOSHOOTER   (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_COLOSSEUM_DOUBLE_SHOOTER,      ///<コロシアム ダブル シューターあり
#define BATTLEMODE_BIT_COLOSSEUM_DOUBLE_SHOOTER    ((BATTLEMODE_TYPE_DOUBLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_ON << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_DOUBLE_SHOOTER    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_COLOSSEUM_TRIPLE_NOSHOOTER,    ///<コロシアム トリプル シューター無し
#define BATTLEMODE_BIT_COLOSSEUM_TRIPLE_NOSHOOTER    ((BATTLEMODE_TYPE_TRIPLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_OFF << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_TRIPLE_NOSHOOTER    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_COLOSSEUM_TRIPLE_SHOOTER,      ///<コロシアム トリプル シューターあり
#define BATTLEMODE_BIT_COLOSSEUM_TRIPLE_SHOOTER    ((BATTLEMODE_TYPE_TRIPLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_ON << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_TRIPLE_SHOOTER   (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_COLOSSEUM_ROTATION_NOSHOOTER,    ///<コロシアム ローテーション シューター無し
#define BATTLEMODE_BIT_COLOSSEUM_ROTATION_NOSHOOTER    ((BATTLEMODE_TYPE_ROTATION << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_OFF << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_ROTATION_NOSHOOTER    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_COLOSSEUM_ROTATION_SHOOTER,      ///<コロシアム ローテーション シューターあり
#define BATTLEMODE_BIT_COLOSSEUM_ROTATION_SHOOTER    ((BATTLEMODE_TYPE_ROTATION << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_ON << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_ROTATION_SHOOTER    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_COLOSSEUM_MULTI_NOSHOOTER,    ///<コロシアム マルチ シューター無し
#define BATTLEMODE_BIT_COLOSSEUM_MULTI_NOSHOOTER    ((BATTLEMODE_TYPE_MULTI << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_OFF << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_MULTI_NOSHOOTER    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_COLOSSEUM_MULTI_SHOOTER,      ///<コロシアム マルチ シューターあり
#define BATTLEMODE_BIT_COLOSSEUM_MULTI_SHOOTER    ((BATTLEMODE_TYPE_MULTI << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_COLOSSEUM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_ON << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_COLOSSEUM_MULTI_SHOOTER    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_SUBWAY_SINGLE,                 ///<地下鉄 シングル
#define BATTLEMODE_BIT_SUBWAY_SINGLE    ((BATTLEMODE_TYPE_SINGLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_SUBWAY << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_SUBWAY << BATTLEMODE_COMMMODE_BIT))
#define BATTLEMODE_MASK_SUBWAY_SINGLE    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK)

//BATTLEMODE_SEARCH_NO_SUBWAY_DOUBLE,                 ///<地下鉄 ダブル
#define BATTLEMODE_BIT_SUBWAY_DOUBLE    ((BATTLEMODE_TYPE_DOUBLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_SUBWAY << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_SUBWAY << BATTLEMODE_COMMMODE_BIT))
#define BATTLEMODE_MASK_SUBWAY_DOUBLE    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK)

//BATTLEMODE_SEARCH_NO_SUBWAY_MULTI,                  ///<地下鉄 マルチ
#define BATTLEMODE_BIT_SUBWAY_MULTI    ((BATTLEMODE_TYPE_MULTI << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_SUBWAY << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_SUBWAY << BATTLEMODE_COMMMODE_BIT))
#define BATTLEMODE_MASK_SUBWAY_MULTI    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK)

//BATTLEMODE_SEARCH_NO_RANDOM_SINGLE,                 ///<ランダムマッチ シングル
#define BATTLEMODE_BIT_RANDOM_SINGLE    ((BATTLEMODE_TYPE_SINGLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_RANDOM << BATTLEMODE_COMMMODE_BIT))
#define BATTLEMODE_MASK_RANDOM_SINGLE    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK)

//BATTLEMODE_SEARCH_NO_RANDOM_DOUBLE,                 ///<ランダムマッチ ダブル
#define BATTLEMODE_BIT_RANDOM_DOUBLE    ((BATTLEMODE_TYPE_DOUBLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_RANDOM << BATTLEMODE_COMMMODE_BIT))
#define BATTLEMODE_MASK_RANDOM_DOUBLE    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK)

//BATTLEMODE_SEARCH_NO_RANDOM_TRIPLE,                 ///<ランダムマッチ トリプル
#define BATTLEMODE_BIT_RANDOM_TRIPLE    ((BATTLEMODE_TYPE_TRIPLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_RANDOM << BATTLEMODE_COMMMODE_BIT))
#define BATTLEMODE_MASK_RANDOM_TRIPLE    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK)

//BATTLEMODE_SEARCH_NO_RANDOM_ROTATION,               ///<ランダムマッチ ローテーション
#define BATTLEMODE_BIT_RANDOM_ROTATION    ((BATTLEMODE_TYPE_ROTATION << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_RANDOM << BATTLEMODE_COMMMODE_BIT))
#define BATTLEMODE_MASK_RANDOM_ROTATION    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK)

//BATTLEMODE_SEARCH_NO_RANDOM_SHOOTER,                ///<ランダムマッチ シューターバトル
#define BATTLEMODE_BIT_RANDOM_SHOOTER    ((BATTLEMODE_TYPE_TRIPLE << BATTLEMODE_TYPE_BIT) | (BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_RANDOM << BATTLEMODE_COMMMODE_BIT) | (BATTLEMODE_SHOOTER_ON << BATTLEMODE_SHOOTER_BIT))
#define BATTLEMODE_MASK_RANDOM_SHOOTER    (BATTLEMODE_TYPE_MASK | BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK | BATTLEMODE_SHOOTER_MASK)

//BATTLEMODE_SEARCH_NO_CONTEST,                       ///<バトル大会
#define BATTLEMODE_BIT_CONTEST    ((BATTLEMODE_COMMUNICATION_COMMUNICATION << BATTLEMODE_COMMUNICATION_BIT) | (BATTLEMODE_COMMMODE_CHAMPIONSHIP << BATTLEMODE_COMMMODE_BIT))
#define BATTLEMODE_MASK_CONTEST    (BATTLEMODE_COMMUNICATION_MASK | BATTLEMODE_COMMMODE_MASK)

//BATTLEMODE_SEARCH_NO_ALL,                       ///<全て(施設指定無し)
#define BATTLEMODE_BIT_ALL        (0)
#define BATTLEMODE_MASK_ALL       (0)

