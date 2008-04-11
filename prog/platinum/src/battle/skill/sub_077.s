
//============================================================================
/**
 *
 *@file		sub_077.s
 *@brief	戦闘シーケンス
 *			いたみわけ効果発動シーケンス
 *@author	HisashiSogabe
 *@data		2006.01.23
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_077:
//みがわりを出されているときは、失敗する
	MIGAWARI_CHECK	SIDE_DEFENCE,Umakukimaran

	GOSUB			SUB_SEQ_WAZA_OUT_EFF

//お互いのHPを足して２で割る
	PSP_VALUE_WORK	VAL_GET,SIDE_ATTACK,ID_PSP_hp,BUF_PARA_CALC_WORK
	PSP_VALUE_WORK	VAL_GET,SIDE_DEFENCE,ID_PSP_hp,BUF_PARA_HP_CALC_WORK
	VALUE_WORK		VAL_ADD,BUF_PARA_CALC_WORK,BUF_PARA_HP_CALC_WORK
	VALUE			VAL_DIV,BUF_PARA_CALC_WORK,2

	//ダメージエフェクトで点滅しないフラグを立てる
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_BLINK
	//HP操作対象をAttackClientに
	VALUE_WORK		VAL_SET,BUF_PARA_CLIENT_WORK,BUF_PARA_ATTACK_CLIENT
	//AttackClientのダメージ量を計算
	PSP_VALUE_WORK	VAL_GET,SIDE_ATTACK,ID_PSP_hp,BUF_PARA_HP_CALC_WORK
	VALUE_WORK		VAL_SUB,BUF_PARA_HP_CALC_WORK,BUF_PARA_CALC_WORK
	VALUE			VAL_MUL,BUF_PARA_HP_CALC_WORK,-1
	GOSUB			SUB_SEQ_HP_CALC

	//ダメージエフェクトで点滅しないフラグを立てる
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_BLINK
	//HP操作対象をDefenceClientに
	VALUE_WORK		VAL_SET,BUF_PARA_CLIENT_WORK,BUF_PARA_DEFENCE_CLIENT
	//DefenceClientのダメージ量を計算
	PSP_VALUE_WORK	VAL_GET,SIDE_DEFENCE,ID_PSP_hp,BUF_PARA_HP_CALC_WORK
	VALUE_WORK		VAL_SUB,BUF_PARA_HP_CALC_WORK,BUF_PARA_CALC_WORK
	VALUE			VAL_MUL,BUF_PARA_HP_CALC_WORK,-1
	GOSUB			SUB_SEQ_HP_CALC

	MESSAGE			ItamiwakeMsg,TAG_NONE
	SERVER_WAIT
	WAIT			MSG_WAIT
	SEQ_END

Umakukimaran:
	VALUE		VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_UMAKUKIMARAN
	SEQ_END
