//============================================================================
/**
 *
 *@file		be_101.s
 *@brief	戦闘シーケンス（BattleEffect）
 *			101　敵のＨＰを必ず１残す（みねうちでは倒せません）
 *
 *@author	HisashiSogabe
 *@data		2006.01.24
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_101:
	CRITICAL_CHECK
	DAMAGE_CALC
	SEQ_END
