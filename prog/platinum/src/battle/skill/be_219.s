//============================================================================
/**
 *
 *@file		be_219.s
 *@brief	戦闘シーケンス（BattleEffect）
 *			219　相手の素早さより自分の素早さが遅い場合、その差があるほど、
 *				 相手にダメージを与える。威力：１＋25×（敵の素早さ÷自分の素早さ）。威力最大150まで。
 *
 *@author	HisashiSogabe
 *@data		2006.02.14
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

BE_219:
	GIROBALL
	CRITICAL_CHECK
	DAMAGE_CALC
	SEQ_END
