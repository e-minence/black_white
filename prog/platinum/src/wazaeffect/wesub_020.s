//==============================================================================
/**
 * @file	we_258.s
 * @brief	あられ			258
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_ARARE
	
// =============================================================================
//
//
//	■あられ			258
//
//
// =============================================================================
WEST_ARARE:
	
	LOAD_PARTICLE_DROP	0,W_258_SPA

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK
	WAIT_FLAG	

	SE_REPEAT_C		SEQ_SE_DP_W258,3,7

	ADD_PARTICLE 	0,W_258_258_ICE_SHOWER, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_258_258_ICE_SHOWER2, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_258_258_ICE_KIRA, EMTFUNC_ATTACK_POS

	WAIT_PARTICLE
	EXIT_PARTICLE	0,

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK
	WAIT_FLAG
	
	SEQEND
