
//============================================================================
/**
 *
 *@file		sub_186.s
 *@brief	戦闘シーケンス
 *			いかくシーケンス
 *@author	HisashiSogabe
 *@data		2006.02.22
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_186:
	WAIT				MSG_WAIT/2
	VALUE				VAL_SET,BUF_PARA_CLIENT_NO_AGI,0
	//AttackClientの退避
	VALUE_WORK			VAL_SET,BUF_PARA_PUSH_CLIENT,BUF_PARA_ATTACK_CLIENT
	VALUE_WORK			VAL_SET,BUF_PARA_ATTACK_CLIENT,BUF_PARA_CLIENT_WORK
SUB_186_LOOP:
	CLIENT_NO_GET_AGI	BUF_PARA_TSUIKA_CLIENT
	SIDE_CHECK			SIDE_ATTACK,SIDE_TSUIKA,SUB_186_NEXT
	MIGAWARI_CHECK		SIDE_TSUIKA,SUB_186_NEXT
	//HP0の時は、無視
	IF_PSP				IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_hp,0,SUB_186_NEXT
	//攻撃を1段階下げるをセット
	VALUE				VAL_SET,BUF_PARA_TSUIKA_PARA,ADD_COND2_POWDOWN
	VALUE				VAL_SET,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI
	GOSUB				SUB_SEQ_ABICNT_CALC
SUB_186_NEXT:
	VALUE				VAL_ADD,BUF_PARA_CLIENT_NO_AGI,1
	CLIENT_SET_MAX_LOOP	BUF_PARA_CLIENT_NO_AGI,SUB_186_LOOP
SUB_186_END:
	//AttackClientの復帰
	VALUE_WORK			VAL_SET,BUF_PARA_ATTACK_CLIENT,BUF_PARA_PUSH_CLIENT
	SEQ_END
