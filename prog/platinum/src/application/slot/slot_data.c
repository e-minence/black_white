//******************************************************************************
/**
 * 
 * @file	slot_data.c
 * @brief	ミニゲーム　スロット　データ
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "slot_local.h"

//==============================================================================
//	define
//==============================================================================

//==============================================================================
//	struct
//==============================================================================

//==============================================================================
//	プロトタイプ
//==============================================================================

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	リール絵柄配列　各列、絵柄の数は21
//	REEL_RED7			//７
//	REEL_BLACK7			//黒７
//	REEL_REPLAY			//リプレイ
//	REEL_CHERRY			//チェリー
//	REEL_BELL			//ベル
//	REEL_SUIKA			//石
//--------------------------------------------------------------
const REEL_PATTERN DATA_SlotDataReelPattern[REEL_MAX][REEL_PT_MAX] =
{
	//一列目
	{
		REEL_SUIKA,			//0  (21)
		REEL_CHERRY,			//1
		REEL_BLACK7,			//2
		REEL_RED7,			//3
		REEL_REPLAY,			//4
		REEL_BELL,			//5
		REEL_SUIKA,			//6
		REEL_CHERRY,			//7
		REEL_BLACK7,			//8
		REEL_REPLAY,			//9
		REEL_BELL,			//10
		REEL_SUIKA,			//11
		REEL_CHERRY,			//12
		REEL_BLACK7,			//13
		REEL_REPLAY,			//14
		REEL_BELL,			//15
		REEL_SUIKA,			//16
		REEL_REPLAY,			//17
		REEL_RED7,			//18
		REEL_REPLAY,			//19
		REEL_BELL,		//20	(1)
	},
	//二列目
	{
		REEL_CHERRY,			//0 (21)
		REEL_RED7,			//1
		REEL_BLACK7,			//2
		REEL_SUIKA,			//3
		REEL_BELL,			//4
		REEL_REPLAY,			//5
		REEL_CHERRY,			//6
		REEL_RED7,			//7
		REEL_SUIKA,			//8
		REEL_BELL,			//9
		REEL_REPLAY,			//10
		REEL_CHERRY,			//11
		REEL_RED7,			//12
		REEL_SUIKA,			//13
		REEL_BELL,			//14
		REEL_REPLAY,			//15
		REEL_CHERRY,			//16
		REEL_BLACK7,			//17
		REEL_SUIKA,			//18
		REEL_BELL,			//19
		REEL_REPLAY,		//20 (1)
	},
	//三列目
	{
		REEL_BELL,			//0 (21)
		REEL_REPLAY,			//1
		REEL_BLACK7,			//2
		REEL_RED7,			//3
		REEL_REPLAY,			//4
		REEL_SUIKA,			//5
		REEL_BELL,			//6
		REEL_REPLAY,			//7
		REEL_BLACK7,			//8
		REEL_CHERRY,			//9
		REEL_SUIKA,			//10
		REEL_BELL,			//11
		REEL_REPLAY,			//12
		REEL_BLACK7,			//13
		REEL_CHERRY,			//14
		REEL_SUIKA,			//15
		REEL_BELL,			//16
		REEL_REPLAY,			//17
		REEL_RED7,			//18
		REEL_CHERRY,			//19
		REEL_SUIKA,		//20 (1)
	},
};

//--------------------------------------------------------------
///	支払い　子役時
//--------------------------------------------------------------
const int DATA_SlotPayOutSmall[REEL_PATTERN_MAX] =
{
	100, 				//赤７
	100,				//黒７
	0,					//リプレイ
	2,					//チェリー
	10,					//ベル
	15,					//スイカ
};

//--------------------------------------------------------------
///	支払い　ボーナス時
//--------------------------------------------------------------
const int DATA_SlotPayOutBonus[REEL_PATTERN_MAX] =
{
	100, 				//赤７
	100,				//黒７
	15,					//リプレイ
	2,					//チェリー
	10,					//ベル
	15,					//スイカ
};

//--------------------------------------------------------------
///	基本抽選 A
//--------------------------------------------------------------
const u32 DATA_SlotBasicLotteryA[SLOT_HARD_MAX] =
{
	25,		//設定１
	25,		//設定２
	30,		//設定３
	30,		//設定４
	35,		//設定５
	35,		//設定６
};

//--------------------------------------------------------------
///	子役抽選 A { ボーナス子役確立, 子役確立 }
//--------------------------------------------------------------
const LT_SMALLDATA DATA_SlotSmallLotteryA[SLOT_HARD_MAX] =
{
	{	//設定１
		{ 1, 4 },				//チェリーボーナス、チェリー
		{ 2, 13 },				//スイカボーナス、スイカ
		{ 1, 39 },				//ベルボーナス、ベル
		{ 1, 39 },				//リプレイボーナス、リプレイ
	},
	{	//設定２
		{ 1, 4 },				//チェリーボーナス、チェリー
		{ 2, 13 },				//スイカボーナス、スイカ
		{ 1, 39 },				//ベルボーナス、ベル
		{ 1, 39 },				//リプレイボーナス、リプレイ
	},
	{	//設定３
		{ 2, 3 },				//チェリーボーナス、チェリー
		{ 4, 11 },				//スイカボーナス、スイカ
		{ 2, 38 },				//ベルボーナス、ベル
		{ 2, 38 },				//リプレイボーナス、リプレイ
	},
	{	//設定４
		{ 3, 2 },				//チェリーボーナス、チェリー
		{ 4, 11 },				//スイカボーナス、スイカ
		{ 2, 38 },				//ベルボーナス、ベル
		{ 2, 38 },				//リプレイボーナス、リプレイ
	},
	{	//設定５
		{ 4, 1 },				//チェリーボーナス、チェリー
		{ 6, 9 },				//スイカボーナス、スイカ
		{ 3, 37 },				//ベルボーナス、ベル
		{ 3, 37 },				//リプレイボーナス、リプレイ
	},
	{	//設定６
		{ 4, 1 },				//チェリーボーナス、チェリー
		{ 6, 9 },				//スイカボーナス、スイカ
		{ 3, 37 },				//ベルボーナス、ベル
		{ 3, 37 },				//リプレイボーナス、リプレイ
	},
};

//--------------------------------------------------------------
///	基本抽選Aの子役成立時のナビ発生　通常ナビが発生する確率
//--------------------------------------------------------------
const u32 DATA_LotteryNavi[SLOT_HARD_MAX] =
{
	80,	//設定１
	80,	//設定２
	80,	//設定３
	80,	//設定４
	80,	//設定５
	70,	//設定６
};

//--------------------------------------------------------------
///	継続率割り振り {確率,ヒットした継続率,継続率種類}
//--------------------------------------------------------------
const LT_CONT_BONUS DATA_LotteryContinueBonus[SLOT_HARD_MAX][CONT_BONUS_TYPE_MAX] =
{
	{	//設定１
		{ 5, CONT_GGG_50, CONT_BONUS_50 },
		{ 10, CONT_777_70, CONT_BONUS_70 },
		{ 10, CONT_GGG_90, CONT_BONUS_90 },
		{ 10, CONT_777_90, CONT_BONUS_90 },
		{ 15, CONT_GGG_70, CONT_BONUS_70 },
		{ 50, CONT_777_50, CONT_BONUS_50 },
	},
	{	//設定２
		{ 5, CONT_GGG_50, CONT_BONUS_50 },
		{ 10, CONT_777_70, CONT_BONUS_70 },
		{ 10, CONT_GGG_90, CONT_BONUS_90 },
		{ 10, CONT_777_90, CONT_BONUS_90 },
		{ 15, CONT_GGG_70, CONT_BONUS_70 },
		{ 50, CONT_777_50, CONT_BONUS_50 },
	},
	{	//設定３
		{ 5, CONT_GGG_50, CONT_BONUS_50 },
		{ 15, CONT_777_70, CONT_BONUS_70 },
		{ 10, CONT_GGG_90, CONT_BONUS_90 },
		{ 15, CONT_777_90, CONT_BONUS_90 },
		{ 15, CONT_GGG_70, CONT_BONUS_70 },
		{ 40, CONT_777_50, CONT_BONUS_50 },
	},
	{	//設定４
		{ 5, CONT_GGG_50, CONT_BONUS_50 },
		{ 15, CONT_777_70, CONT_BONUS_70 },
		{ 15, CONT_GGG_90, CONT_BONUS_90 },
		{ 15, CONT_777_90, CONT_BONUS_90 },
		{ 10, CONT_GGG_70, CONT_BONUS_70 },
		{ 40, CONT_777_50, CONT_BONUS_50 },
	},
	{	//設定５
		{ 5, CONT_GGG_50, CONT_BONUS_50 },
		{ 20, CONT_777_70, CONT_BONUS_70 },
		{ 15, CONT_GGG_90, CONT_BONUS_90 },
		{ 20, CONT_777_90, CONT_BONUS_90 },
		{ 10, CONT_GGG_70, CONT_BONUS_70 },
		{ 30, CONT_777_50, CONT_BONUS_50 },
	},
	{	//設定６
		{ 5, CONT_GGG_50, CONT_BONUS_50 },
		{ 20, CONT_777_70, CONT_BONUS_70 },
		{ 15, CONT_GGG_90, CONT_BONUS_90 },
		{ 20, CONT_777_90, CONT_BONUS_90 },
		{ 10, CONT_GGG_70, CONT_BONUS_70 },
		{ 30, CONT_777_50, CONT_BONUS_50 },
	},
};

//--------------------------------------------------------------
///	継続率種類->実継続率
//--------------------------------------------------------------
const u32 DATA_ContinueBonusTypeParam[CONT_BONUS_TYPE_MAX] =
{
	50,		//CONT_777_50
	70,		//CONT_777_70
	90,		//CONT_777_90
	50,		//CONT_GGG_50
	70,		//CONT_GGG_70
	90,		//CONT_GGG_90
};

//--------------------------------------------------------------
///	登場ポケモン選択　{ 確率, 登場ポケモン }
//	PIP_RARE	レアピッピ
//	PIP_NORMAL	ピッピ　
//	PIP_META	メタ門
//--------------------------------------------------------------
const LT_POKE_ENTRY DATA_LotteryPokeEntry[SLOT_HARD_MAX][CONT_BONUS_MAX][PIP_MAX] =
{
	{	//設定１
		{	//継続率５０％
			{ 5, PIP_RARE },{ 20, PIP_NORMAL },{ 75, PIP_META },
		},
		{	//継続率７０％
			{ 20, PIP_RARE },{ 60, PIP_NORMAL },{ 20, PIP_META },
		},
		{	//継続率９０％
			{ 75, PIP_RARE },{ 20, PIP_NORMAL },{ 5, PIP_META },
		},
	},
	{	//設定２
		{	//継続率５０％
			{ 5, PIP_RARE },{ 20, PIP_NORMAL },{ 75, PIP_META },
		},
		{	//継続率７０％
			{ 20, PIP_RARE },{ 60, PIP_NORMAL },{ 20, PIP_META },
		},
		{	//継続率９０％
			{ 75, PIP_RARE },{ 20, PIP_NORMAL },{ 5, PIP_META },
		},
	},
	{	//設定３
		{	//継続率５０％
			{ 5, PIP_RARE },{ 20, PIP_NORMAL },{ 75, PIP_META },
		},
		{	//継続率７０％
			{ 20, PIP_RARE },{ 60, PIP_NORMAL },{ 20, PIP_META },
		},
		{	//継続率９０％
			{ 75, PIP_RARE },{ 20, PIP_NORMAL },{ 5, PIP_META },
		},
	},
	{	//設定４
		{	//継続率５０％
			{ 5, PIP_RARE },{ 20, PIP_NORMAL },{ 75, PIP_META },
		},
		{	//継続率７０％
			{ 20, PIP_RARE },{ 60, PIP_NORMAL },{ 20, PIP_META },
		},
		{	//継続率９０％
			{ 75, PIP_RARE },{ 20, PIP_NORMAL },{ 5, PIP_META },
		},
	},
	{	//設定５
		{	//継続率５０％
			{ 5, PIP_RARE },{ 20, PIP_NORMAL },{ 75, PIP_META },
		},
		{	//継続率７０％
			{ 20, PIP_RARE },{ 60, PIP_NORMAL },{ 20, PIP_META },
		},
		{	//継続率９０％
			{ 75, PIP_RARE },{ 20, PIP_NORMAL },{ 5, PIP_META },
		},
	},
	{	//設定６
		{	//継続率５０％
			{ 5, PIP_RARE },{ 20, PIP_NORMAL },{ 75, PIP_META },
		},
		{	//継続率７０％
			{ 20, PIP_RARE },{ 60, PIP_NORMAL },{ 20, PIP_META },
		},
		{	//継続率９０％
			{ 75, PIP_RARE },{ 20, PIP_NORMAL },{ 5, PIP_META },
		},
	},
};

//--------------------------------------------------------------
///	基本抽選 B
//--------------------------------------------------------------
const u32 DATA_SlotBasicLotteryB[SLOT_HARD_MAX] =
{
	25,		//設定１
	25,		//設定２
	30,		//設定３
	30,		//設定４
	35,		//設定５
	35,		//設定６
};

//--------------------------------------------------------------
///	子役抽選 B
//--------------------------------------------------------------
const u32 DATA_SlotSmallLotteryB[SLOT_HARD_MAX][4] =
{
	{	//設定１
		5,		//チェリー
		15,		//スイカ
		30,		//ベル
		50,		//リプレイ
	},
	{	//設定２
		5,		//チェリー
		15,		//スイカ
		30,		//ベル
		50,		//リプレイ
	},
	{	//設定３
		5,		//チェリー
		15,		//スイカ
		30,		//ベル
		50,		//リプレイ
	},
	{	//設定４
		5,		//チェリー
		15,		//スイカ
		30,		//ベル
		50,		//リプレイ
	},
	{	//設定５
		5,		//チェリー
		15,		//スイカ
		30,		//ベル
		50,		//リプレイ
	},
	{	//設定６
		5,		//チェリー
		15,		//スイカ
		30,		//ベル
		50,		//リプレイ
	},
};

//--------------------------------------------------------------
///	月光フラッシュ抽選
//--------------------------------------------------------------
const u32 DATA_LotteryMoonLight[SLOT_HARD_MAX] =
{
	1,	//設定１
	1,	//設定２
	3,	//設定３
	3,	//設定４
	5,	//設定５
	5,	//設定６
};

//--------------------------------------------------------------
///	赤い月抽選
//--------------------------------------------------------------
const u32 DATA_LotteryMoonLightRed[SLOT_HARD_MAX] =
{
	3,	//設定１
	3,	//設定２
	5,	//設定３
	5,	//設定４
	7,	//設定５
	7,	//設定６
};

//--------------------------------------------------------------
///	継続率ダウン抽選 { 10%ダウン, 5%ダウン }
//--------------------------------------------------------------
const u32 DATA_LotteryBonusContinue[SLOT_HARD_MAX][CONT_BONUS_DOWN_MAX] =
{
	{ 25, 20 },	//設定１
	{ 20, 15 },	//設定２
	{ 15, 15 },	//設定３
	{ 15, 15 },	//設定４
	{ 10, 15 },	//設定５
	{ 5, 10 },	//設定６
};

//--------------------------------------------------------------
///	終了時継続演出 {対象となる継続率, ボール戻り, 通常, アンコール },
//--------------------------------------------------------------
const LT_CONTBONUSEND DATA_LotteryContinueBonusEnd[SLOT_HARD_MAX][CONT_BONUSEND_NUM_MAX] =
{
	{	//設定１
		{CONT_BONUSEND_100_75, 10, 10, 80 },	//〜75%
		{CONT_BONUSEND_65, 10, 30, 60 },		//〜65%
		{CONT_BONUSEND_55, 10, 40, 50 },		//〜55%
		{CONT_BONUSEND_45, 20, 50, 30 },		//〜45%
		{CONT_BONUSEND_35, 30, 50, 20 },		//〜35%
		{CONT_BONUSEND_25, 50, 40, 10 },		//〜25%
		{CONT_BONUSEND_15, 60, 30, 10 },		//〜15%
		{CONT_BONUSEND_5, 80, 10, 10 },			//〜5%
		{CONT_BONUSEND_0, 90, 5, 5 },			//〜0% 念の為　本来ここには来ない
	},
	{	//設定２
		{CONT_BONUSEND_100_75, 10, 10, 80 },	//〜75%
		{CONT_BONUSEND_65, 10, 30, 60 },		//〜65%
		{CONT_BONUSEND_55, 10, 40, 50 },		//〜55%
		{CONT_BONUSEND_45, 20, 50, 30 },		//〜45%
		{CONT_BONUSEND_35, 30, 50, 20 },		//〜35%
		{CONT_BONUSEND_25, 50, 40, 10 },		//〜25%
		{CONT_BONUSEND_15, 60, 30, 10 },		//〜15%
		{CONT_BONUSEND_5, 80, 10, 10 },			//〜5%
		{CONT_BONUSEND_0, 90, 5, 5 },			//〜0%
	},
	{	//設定３
		{CONT_BONUSEND_100_75, 10, 10, 80 },	//〜75%
		{CONT_BONUSEND_65, 10, 30, 60 },		//〜65%
		{CONT_BONUSEND_55, 10, 40, 50 },		//〜55%
		{CONT_BONUSEND_45, 20, 50, 30 },		//〜45%
		{CONT_BONUSEND_35, 30, 50, 20 },		//〜35%
		{CONT_BONUSEND_25, 50, 40, 10 },		//〜25%
		{CONT_BONUSEND_15, 60, 30, 10 },		//〜15%
		{CONT_BONUSEND_5, 80, 10, 10 },			//〜5%
		{CONT_BONUSEND_0, 90, 5, 5 },			//〜0%
	},
	{	//設定４
		{CONT_BONUSEND_100_75, 10, 10, 80 },	//〜75%
		{CONT_BONUSEND_65, 10, 30, 60 },		//〜65%
		{CONT_BONUSEND_55, 10, 40, 50 },		//〜55%
		{CONT_BONUSEND_45, 20, 50, 30 },		//〜45%
		{CONT_BONUSEND_35, 30, 50, 20 },		//〜35%
		{CONT_BONUSEND_25, 50, 40, 10 },		//〜25%
		{CONT_BONUSEND_15, 60, 30, 10 },		//〜15%
		{CONT_BONUSEND_5, 80, 10, 10 },			//〜5%
		{CONT_BONUSEND_0, 90, 5, 5 },			//〜0%
	},
	{	//設定５
		{CONT_BONUSEND_100_75, 10, 10, 80 },	//〜75%
		{CONT_BONUSEND_65, 10, 30, 60 },		//〜65%
		{CONT_BONUSEND_55, 10, 40, 50 },		//〜55%
		{CONT_BONUSEND_45, 20, 50, 30 },		//〜45%
		{CONT_BONUSEND_35, 30, 50, 20 },		//〜35%
		{CONT_BONUSEND_25, 50, 40, 10 },		//〜25%
		{CONT_BONUSEND_15, 60, 30, 10 },		//〜15%
		{CONT_BONUSEND_5, 80, 10, 10 },			//〜5%
		{CONT_BONUSEND_0, 90, 5, 5 },			//〜0%
	},
	{	//設定６
		{CONT_BONUSEND_100_75, 10, 10, 80 },	//〜75%
		{CONT_BONUSEND_65, 10, 30, 60 },		//〜65%
		{CONT_BONUSEND_55, 10, 40, 50 },		//〜55%
		{CONT_BONUSEND_45, 20, 50, 30 },		//〜45%
		{CONT_BONUSEND_35, 30, 50, 20 },		//〜35%
		{CONT_BONUSEND_25, 50, 40, 10 },		//〜25%
		{CONT_BONUSEND_15, 60, 30, 10 },		//〜15%
		{CONT_BONUSEND_5, 80, 10, 10 },			//〜5%
		{CONT_BONUSEND_0, 90, 5, 5 },			//〜0%
	},
};
