
//============================================================================
/**
 *
 *@file		sub_085.s
 *@brief	戦闘シーケンス
 *			どろぼうシーケンス
 *@author	HisashiSogabe
 *@data		2006.01.25
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_085:
	DOROBOU			SUB_085_END,SUB_085_TOKUSEI
	VALUE			VAL_SET,BUF_PARA_WAZA_EFF_CNT,1
	VALUE			VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_WAZA_EFFECT
	WAZA_EFFECT		SIDE_ATTACK
	SERVER_WAIT
	MESSAGE			DorobouM2MMsg,TAG_NICK_NICK_ITEM,SIDE_ATTACK,SIDE_DEFENCE,SIDE_DEFENCE
	SERVER_WAIT
	WAIT			MSG_WAIT
	PSP_VALUE_WORK	VAL_GET,SIDE_DEFENCE,ID_PSP_item,BUF_PARA_TEMP_WORK
	PSP_VALUE_WORK	VAL_SET,SIDE_ATTACK,ID_PSP_item,BUF_PARA_TEMP_WORK
	PSP_VALUE		VAL_SET,SIDE_DEFENCE,ID_PSP_item,0
	BRANCH			SUB_085_END
SUB_085_TOKUSEI:
	MESSAGE			TokuseiNoWazaMineMsg,TAG_NICK_TOKU_WAZA,SIDE_DEFENCE,SIDE_DEFENCE,SIDE_ATTACK
	SERVER_WAIT
	WAIT			MSG_WAIT
SUB_085_END:
	SEQ_END
