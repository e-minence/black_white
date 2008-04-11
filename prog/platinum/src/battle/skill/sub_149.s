
//============================================================================
/**
 *
 *@file		sub_149.s
 *@brief	戦闘シーケンス
 *			コスモパワーシーケンス
 *@author	HisashiSogabe
 *@data		2006.02.09
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_149:
	IF_PSP			IF_FLAG_NE,SIDE_TSUIKA,ID_PSP_abiritycnt_def,12,SUB_149_NEXT
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_abiritycnt_spedef,12,NoUpMsg
SUB_149_NEXT:
	GOSUB			SUB_SEQ_WAZA_OUT_EFF
	//アサートメッセージ非表示フラグをON
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_DIRECT_MSG
	//これ以降は、ATTACK_MESSAGEとWAZA_EFFECTは出ないようにする
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_ATTACK_MSG|SERVER_STATUS_FLAG_NO_WAZA_EFFECT
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_ABICNT_EFF_CHECK
	//防御を1段階上げるをセット
	VALUE			VAL_SET,BUF_PARA_TSUIKA_PARA,ADD_COND2_DEFUP
	GOSUB			SUB_SEQ_ABICNT_CALC
	//特防を1段階上げるをセット
	VALUE			VAL_SET,BUF_PARA_TSUIKA_PARA,ADD_COND2_SPEDEFUP
	GOSUB			SUB_SEQ_ABICNT_CALC
	VALUE			VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_ABICNT_EFF_FLAG
	VALUE			VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_ABICNT_EFF_CHECK
	SEQ_END

NoUpMsg:
	ATTACK_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
	MESSAGE			NouryokuNoUpMineMsg,TAG_NICK,SIDE_TSUIKA
	SERVER_WAIT
	WAIT			MSG_WAIT
	//VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI_RENZOKU_CHECK
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI
	SEQ_END

