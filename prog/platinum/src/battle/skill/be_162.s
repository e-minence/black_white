//============================================================================
/**
 *
 *@file		be_162.s
 *@brief	戦闘シーケンス（BattleEffect）
 *			162　たくわえた分だけたくさん回復する。攻撃終了後、たくわえるで上げた、ステータスを元に戻す。
 *
 *@author	HisashiSogabe
 *@data		2006.02.06
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_162:
	IF_PSP			IF_FLAG_EQ,SIDE_ATTACK,ID_PSP_wkw_takuwaeru_count,0,BE_162_NG
	PSP_VALUE_WORK	VAL_GET,SIDE_ATTACK,ID_PSP_wkw_takuwaeru_count,BUF_PARA_HP_CALC_WORK
	VALUE			VAL_SUB,BUF_PARA_HP_CALC_WORK,3
	VALUE			VAL_MUL,BUF_PARA_HP_CALC_WORK,-1
	VALUE			VAL_SET,BUF_PARA_CALC_WORK,1
	VALUE_WORK		VAL_LSH,BUF_PARA_CALC_WORK,BUF_PARA_HP_CALC_WORK
	PSP_VALUE_WORK	VAL_GET,SIDE_ATTACK,ID_PSP_hpmax,BUF_PARA_HP_CALC_WORK
	DAMAGE_DIV_WORK	BUF_PARA_HP_CALC_WORK,BUF_PARA_CALC_WORK
	PSP_VALUE		VAL_SET,SIDE_ATTACK,ID_PSP_wkw_takuwaeru_count,0
	PSP_VALUE_WORK	VAL_GET,SIDE_ATTACK,ID_PSP_wkw_takuwaeru_def_count,BUF_PARA_CALC_WORK
	PSP_VALUE_WORK	VAL_SUB_ZERO,SIDE_ATTACK,ID_PSP_abiritycnt_def,BUF_PARA_CALC_WORK
	PSP_VALUE_WORK	VAL_GET,SIDE_ATTACK,ID_PSP_wkw_takuwaeru_spedef_count,BUF_PARA_CALC_WORK
	PSP_VALUE_WORK	VAL_SUB_ZERO,SIDE_ATTACK,ID_PSP_abiritycnt_spedef,BUF_PARA_CALC_WORK
	PSP_VALUE		VAL_SET,SIDE_ATTACK,ID_PSP_wkw_takuwaeru_def_count,0
	PSP_VALUE		VAL_SET,SIDE_ATTACK,ID_PSP_wkw_takuwaeru_spedef_count,0
	//HP操作対象をAttackClientに
	VALUE_WORK		VAL_SET,BUF_PARA_CLIENT_WORK,BUF_PARA_ATTACK_CLIENT
	VALUE			VAL_SET,BUF_PARA_ADD_STATUS_DIRECT,ADD_COND2_NOMIKOMU|ADD_STATUS_WAZAKOUKA
	SEQ_END

BE_162_NG:
	ATTACK_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
	MESSAGE			NomikomuNGMsg,TAG_NONE
	SERVER_WAIT
	WAIT			MSG_WAIT
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI
	SEQ_END
