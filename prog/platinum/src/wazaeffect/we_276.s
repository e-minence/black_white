//==============================================================================
/**
 * @file	we_276.s
 * @brief	ばかぢから			276
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_BAKADIKARA
	
// =============================================================================
//
//
//	■ばかぢから			276
//
//
// =============================================================================
WEST_BAKADIKARA:

	LOAD_PARTICLE_DROP	0,W_276_SPA

	SE_L			SEQ_SE_DP_W025

	POKEOAM_RES_INIT
	POKEOAM_RES_LOAD	0
	
	POKEOAM_DROP		WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_0, WEDEF_POKE_RES_0,
//	POKEOAM_DROP		WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_1, WEDEF_POKE_RES_0,

	FUNC_CALL			WEST_SP_WE_T08, 2, 0, WEDEF_DROP_M1,
	ADD_PARTICLE 	0,W_276_276_LINE_UP1, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_276_276_LINE_UP2, EMTFUNC_ATTACK_POS

	WAIT_FLAG
	WAIT			30
	WAIT			10
	
	WORK_CLEAR
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 	1
	HAIKEI_CHG		BG_ID_276, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_NONE
	WAIT			5
	
	SE_R			SEQ_SE_DP_W025B
	FUNC_CALL		WEST_SP_WE_T10,	4, 2, +16, -8, WE_TOOL_M1 | WE_TOOL_SSP
	WAIT			2
		
	ADD_PARTICLE 	0,W_276_276_BALL_BURN, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_276_276_HITMARK,   EMTFUNC_DEFENCE_POS

	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 8, 0, 1, 4,  WE_TOOL_E1 | WE_TOOL_SSP,

	FUNC_CALL		WEST_SP_WE_T10,	4, 2, -16, +8, WE_TOOL_M1 | WE_TOOL_SSP
	WAIT_FLAG


	WAIT_PARTICLE
	EXIT_PARTICLE	0,

	WORK_CLEAR
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R,	1
	HAIKEI_RECOVER	BG_ID_276, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_NONE
	HAIKEI_CHG_WAIT
	
	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_1
	
	SEQEND
