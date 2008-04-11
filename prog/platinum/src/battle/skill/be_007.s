//============================================================================
/**
 *
 *@file		be_007.s
 *@brief	戦闘シーケンス（BattleEffect）
 *			007　使用した後、戦闘不能になる
 *
 *@author	HisashiSogabe
 *@data		2005.12.05
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_007:
	//特性しめりけがいる時は、じばく失敗
	KATAYABURI_TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_ALL,TOKUSYU_SIMERIKE,TokusyuNoJibaku

	//自爆の初期化処理をした後は、JibakuStartへジャンプ
	IF				IF_FLAG_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_JIBAKU_MASK,JibakuStart
//自爆の初期化処理
	VALUE			VAL_SET,BUF_PARA_CALC_WORK,SERVER_STATUS_FLAG_JIBAKU
	VALUE_WORK		VAL_LSH,BUF_PARA_CALC_WORK,BUF_PARA_ATTACK_CLIENT
	VALUE_WORK		VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,BUF_PARA_CALC_WORK
	PSP_VALUE		VAL_SET,SIDE_ATTACK,ID_PSP_hp,0
	//ゲージを0にする処理
	VALUE			VAL_SET,BUF_PARA_HP_CALC_WORK,JIBAKU_HP
	HP_GAUGE_CALC	SIDE_ATTACK
	SERVER_WAIT
	GOSUB			SUB_SEQ_WAZA_OUT_EFF
JibakuStart:	
	CRITICAL_CHECK
	DAMAGE_CALC
	SEQ_END
TokusyuNoJibaku:
	ATTACK_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
	MESSAGE			ShimerikeM2MMsg,TAG_NICK_TOKU_NICK_WAZA,SIDE_TOKUSEI,SIDE_TOKUSEI,SIDE_ATTACK,SIDE_ATTACK
	SERVER_WAIT
	WAIT			MSG_WAIT
	//複数体チェックをしないフラグを立てる
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_NO_DOUBLE_CHECK
	//技の起動に失敗フラグを立てる
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI
	SEQ_END
