
//============================================================================
/**
 *
 *@file		sub_096.s
 *@brief	戦闘シーケンス
 *			のろい（ノーマル）追加効果シーケンス
 *@author	HisashiSogabe
 *@data		2006.01.27
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_096:
	//素早さを1段階下げるをセット
	VALUE		VAL_SET,BUF_PARA_TSUIKA_PARA,ADD_COND2_AGIDOWN
	GOSUB		SUB_SEQ_ABICNT_CALC
	//これ以降は、ATTACK_MESSAGEとWAZA_EFFECTは出ないようにする
	VALUE		VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_ATTACK_MSG|SERVER_STATUS_FLAG_NO_WAZA_EFFECT
	VALUE		VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_ABICNT_EFF_CHECK
	//攻撃を1段階上げるをセット
	VALUE		VAL_SET,BUF_PARA_TSUIKA_PARA,ADD_COND2_POWUP
	GOSUB		SUB_SEQ_ABICNT_CALC
	//防御を1段階上げるをセット
	VALUE		VAL_SET,BUF_PARA_TSUIKA_PARA,ADD_COND2_DEFUP
	GOSUB		SUB_SEQ_ABICNT_CALC
	VALUE		VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_ABICNT_EFF_FLAG
	VALUE		VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG2,SERVER_STATUS_FLAG2_ABICNT_EFF_CHECK
	SEQ_END
