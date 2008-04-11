
//============================================================================
/**
 *
 *@file		sub_088.s
 *@brief	戦闘シーケンス
 *			いやしのすずシーケンス
 *@author	HisashiSogabe
 *@data		2006.01.25
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_088:
	IYASINOSUZU
	IF			IF_FLAG_EQ,BUF_PARA_WAZA_NO_NOW,WAZANO_AROMASERAPII,AromaMessage
	MESSAGE		IyashinosuzuMsg,TAG_NONE
	BRANCH		SUB_088_NEXT
AromaMessage:
	MESSAGE		AromaMsg,TAG_NONE
SUB_088_NEXT:
	SERVER_WAIT
	WAIT		MSG_WAIT
	IF			IF_FLAG_EQ,BUF_PARA_WAZA_NO_NOW,WAZANO_AROMASERAPII,SUB_088_END
	//ぼうおんで防いだ系メッセージ
	IF			IF_FLAG_NBIT,BUF_PARA_CALC_WORK,BOUON_SIDE_1,BouonSide2
	MESSAGE		BouonMineMsg,TAG_NICK_TOKU_WAZA,SIDE_ATTACK,SIDE_ATTACK,SIDE_WORK
	SERVER_WAIT
	WAIT		MSG_WAIT
BouonSide2:
	IF			IF_FLAG_NBIT,BUF_PARA_CALC_WORK,BOUON_SIDE_2,SUB_088_END
	MESSAGE		BouonMineMsg,TAG_NICK_TOKU_WAZA,SIDE_ATTACK_PAIR,SIDE_ATTACK_PAIR,SIDE_WORK
	SERVER_WAIT
	WAIT		MSG_WAIT
SUB_088_END:
	IF			IF_FLAG_BIT,BUF_PARA_CALC_WORK,NOSET_SIDE_1,NoSetSide1
	STATUS_SET	SIDE_ATTACK,STATUS_NORMAL
NoSetSide1:
	IF			IF_FLAG_BIT,BUF_PARA_CALC_WORK,NOSET_SIDE_2,NoSetSide2
	STATUS_SET	SIDE_ATTACK_PAIR,STATUS_NORMAL
NoSetSide2:
	SEQ_END
