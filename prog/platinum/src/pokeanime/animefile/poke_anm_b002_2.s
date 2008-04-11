//==============================================================================
/**
 * @file	poke_anm_b_002_2.s
 * @brief	ポケモンアニメ 横揺れ
 * @author	saito
 * @date	
 *
 */
//==============================================================================

	.text
	
	.include	"past.h"
	.include	"past_def.h"
	
	.global		PAST_ANIME_B_002_2
	
// =============================================================================
//
//
//	背面	横揺れ
//
//
// =============================================================================
PAST_ANIME_B002_2:
	//20回のループで1.5周（0x18000）分を処理
	CALL_MF_CURVE_DIVTIME	APPLY_SET,0,CURVE_SIN,TARGET_DX,6,0x18000,0,20
	HOLD_CMD
	END_ANM
