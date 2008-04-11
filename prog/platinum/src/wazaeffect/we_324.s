//==============================================================================
/**
 * @file	we_324.s
 * @brief	シグナルビーム			324
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_SIGUNARUBIIMU
	
// =============================================================================
//
//
//	■シグナルビーム			324
//
//
// =============================================================================
WEST_SIGUNARUBIIMU:
	
	LOAD_PARTICLE_DROP		0,W_324_SPA

	PTAT_JP			PTAT	
	CONTEST_JP		WEST_CONTEST
	
	ADD_PARTICLE_SEP	0, W_324_324_SIGNAL_BEAM1, W_324_324_SIGNAL_BEAM2, W_324_324_SIGNAL_BEAM3, W_324_324_SIGNAL_BEAM4, W_324_324_SIGNAL_BEAM5, W_324_324_SIGNAL_BEAM6, EMTFUNC_SEP_POS
	
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 0, 15,  WE_TOOL_M1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 0, 15,  WE_TOOL_E1 | WE_TOOL_SSP,

	SE_REPEAT_C		SEQ_SE_DP_W062,6,2
	LOOP_LABEL		3
		
		FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5,  WE_TOOL_E1, 0, 1, WE_PAL_RED,		8
		WAIT			8
		FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5,  WE_TOOL_E1, 0, 1, WE_PAL_GREEN,	8
		WAIT			8
		
	LOOP

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

PTAT:

	ADD_PARTICLE_PTAT	0, W_324_324_SIGNAL_BEAM_PARTY1, W_324_324_SIGNAL_BEAM_PARTY2, W_324_324_SIGNAL_BEAM_PARTY1, W_324_324_SIGNAL_BEAM_PARTY2, EMTFUNC_ATTACK_POS
	
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 0, 15,  WE_TOOL_M1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 0, 15,  WE_TOOL_E1 | WE_TOOL_SSP,

	SE_REPEAT_C		SEQ_SE_DP_W062,6,2
	LOOP_LABEL		3
		
		FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5,  WE_TOOL_E1, 0, 1, WE_PAL_RED,		8
		WAIT			8
		FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5,  WE_TOOL_E1, 0, 1, WE_PAL_GREEN,	8
		WAIT			8
		
	LOOP

	WAIT_PARTICLE
	EXIT_PARTICLE	0,

	SEQEND

WEST_CONTEST:

	ADD_PARTICLE	0, W_324_324_SIGNAL_BEAM_CNT, EMTFUNC_ATTACK_POS

	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 0, 15,  WE_TOOL_M1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 0, 15,  WE_TOOL_E1 | WE_TOOL_SSP,

	SE_REPEAT_C		SEQ_SE_DP_W062,6,2
	LOOP_LABEL		3
		
		FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5,  WE_TOOL_E1, 0, 1, WE_PAL_RED,		8
		WAIT			8
		FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5,  WE_TOOL_E1, 0, 1, WE_PAL_GREEN,	8
		WAIT			8
		
	LOOP

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
