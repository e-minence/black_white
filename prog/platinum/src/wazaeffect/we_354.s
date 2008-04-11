//==============================================================================
/**
 * @file	we_354.s
 * @brief	サイコブースト			354
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_SAIKOBUUSUTO
	
// =============================================================================
//
//
//	■サイコブースト			354
//
//
// =============================================================================
WEST_SAIKOBUUSUTO:
		
	LOAD_PARTICLE_DROP	0,W_354_SPA
	ADD_PARTICLE 	0,W_354_354_PSY_BALL1, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_354_354_LINE_BURN, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_354_354_PSY_BALL2, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_354_354_BALL_BURN, EMTFUNC_ATTACK_POS

	SE_L			SEQ_SE_DP_W376

	WAIT			30 * 2
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_WHITE, WEDEF_FADE_PARAM, 50
	WAIT			30 * 2
	SE_L			SEQ_SE_DP_W379
	
	WORK_CLEAR
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 	1
	HAIKEI_CHG		BG_ID_354, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_NONE
	WAIT			5
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_WHITE, WEDEF_FADE_PARAM, 10
	
	SE_WAITPLAY_R	SEQ_SE_DP_480,10
	SE_WAITPLAY_R	SEQ_SE_DP_400,15

	ADD_PARTICLE 	0,W_354_354_HITMARK1, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_354_354_HITMARK2, EMTFUNC_DEFENCE_POS
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_B_X, 0, SHAKE_B_WAIT, SHAKE_B_NUM*4,  WE_TOOL_E1 | WE_TOOL_SSP,

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WORK_CLEAR
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R,	1
	HAIKEI_RECOVER	BG_ID_354, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_NONE
	HAIKEI_CHG_WAIT
	SEQEND
