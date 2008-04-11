
//============================================================================
/**
 *
 *@file		sub_003.s
 *@brief	戦闘シーケンス
 *			逃げるシーケンス
 *@author	HisashiSogabe
 *@data		2005.07.29
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_003:
	//特性しぜんかいふくをもっていたら、状態異常を直す
	SIZENKAIHUKU_CHECK	SIDE_MINE_1,SUB_003_MINE2
	PSP_VALUE			VAL_SET,SIDE_MINE_1,ID_PSP_condition,0
SUB_003_MINE2:
	SIZENKAIHUKU_CHECK	SIDE_MINE_2,SUB_003_START
	PSP_VALUE			VAL_SET,SIDE_MINE_2,ID_PSP_condition,0
SUB_003_START:

	SE_PLAY			SIDE_ATTACK,BSE_ESCAPE
	IF				IF_FLAG_BIT,BUF_PARA_FIGHT_TYPE,FIGHT_TYPE_TOWER,TowerEscape
	//通信対戦は、特性、装備効果をみない
	IF				IF_FLAG_BIT,BUF_PARA_FIGHT_TYPE,FIGHT_TYPE_SIO,SioEscape
	//特性にげあしかチェック
	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_ATTACK,TOKUSYU_NIGEASI,Nigeashi
	//装備効果でかならず逃げれるかチェック
	SOUBI_CHECK		SOUBI_HAVE,SIDE_ATTACK,SOUBI_KANARAZUNIGERARERU,Kemuridama
Command:
	MESSAGE			EscapeMsg,TAG_NONE
	BRANCH			SUB_003_NEXT

Nigeashi:
	MESSAGE			TokuseiEscapeMsg,TAG_NICK_TOKU,SIDE_ATTACK,SIDE_ATTACK
	BRANCH			SUB_003_NEXT

Kemuridama:
	MESSAGE			ItemEscapeMsg,TAG_NICK_ITEM,SIDE_ATTACK,SIDE_ATTACK

SUB_003_NEXT:
	SERVER_WAIT
	WAIT			MSG_WAIT
	FADE_OUT
	SERVER_WAIT
	VALUE			VAL_BIT,BUF_PARA_WIN_LOSE_FLAG,FIGHT_ESCAPE
	SEQ_END

TowerEscape:
	GIVEUP_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
	FADE_OUT
	SERVER_WAIT
	VALUE			VAL_NBIT,BUF_PARA_WIN_LOSE_FLAG,FIGHT_ESCAPE_SELECT_PAUSE
	SEQ_END

SioEscape:
	ESCAPE_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
	FADE_OUT
	SERVER_WAIT
	VALUE			VAL_NBIT,BUF_PARA_WIN_LOSE_FLAG,FIGHT_ESCAPE_SELECT_PAUSE
	SEQ_END

