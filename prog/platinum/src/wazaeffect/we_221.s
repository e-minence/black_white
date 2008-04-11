//==============================================================================
/**
 * @file	we_221.s
 * @brief	せいなるほのお			221
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_SEINARUHONOO
	
// =============================================================================
//
//
//	■せいなるほのお			221
//
//
// =============================================================================
WEST_SEINARUHONOO:
	
	LOAD_PARTICLE_DROP	0,W_221_SPA
	WAIT	1
		
	WORK_CLEAR
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 1
	HAIKEI_CHG		BG_ID_221, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_MOVE
	HAIKEI_CHG_WAIT

	ADD_PARTICLE 	0,W_221_221_SEIHONOO_FIRE1, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_221_221_SEIHONOO_FIRE2,	EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_221_221_SEIHONOO_RING,	EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_221_221_SEIHONOO_BALL,	EMTFUNC_DEFENCE_POS

	SE_R			SEQ_SE_DP_W221B
	WAIT			40
	SE_R			SEQ_SE_DP_W053
	WAIT			10
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_RED, 10, 0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_S_X, 0, SHAKE_S_WAIT, SHAKE_S_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,

	WAIT_PARTICLE
	EXIT_PARTICLE	0,

	SE_STOP			SEQ_SE_DP_W053

	
	WORK_CLEAR
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 1
	HAIKEI_RECOVER	BG_ID_200, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT
			
	SEQEND
