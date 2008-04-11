//==============================================================================
/**
 * @file	we_161.s
 * @brief	トライアタック			161
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_TORAIATAKKU
	
// =============================================================================
//
//
//	■トライアタック			161
//
//
// =============================================================================
WEST_TORAIATAKKU:

	LOAD_PARTICLE_DROP	0,W_161_SPA
	LOAD_PARTICLE_DROP	1,W_161_SPA
	
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	WAIT_FLAG
	
	SE_REPEAT_L		SEQ_SE_DP_W161,8,3

	ADD_PARTICLE 	0,W_161_161_TRYATK_BLUE,	EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_161_161_TRYATK_RED,		EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_161_161_TRYATK_YELLOW,	EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_161_161_TRYATK_RING,	EMTFUNC_ATTACK_POS

	ADD_PARTICLE 	0,W_161_161_TRYATK_SHOOT,	EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_L161SP, OPERATOR_AXIS_AT_161, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	WAIT			40

	SE_R			SEQ_SE_DP_W172B
	ADD_PARTICLE 	0,W_161_161_TRYATK_FIRE, EMTFUNC_DEFENCE_POS
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 1, 2,  WE_TOOL_E1 | WE_TOOL_SSP,
	WAIT			20

	SE_R			SEQ_SE_DP_W161B
	ADD_PARTICLE 	0,W_161_161_TRYATK_THUNDER, EMTFUNC_DEFENCE_POS	
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 1, 2,  WE_TOOL_E1 | WE_TOOL_SSP,
	WAIT			20

	//SE_R			SEQ_SE_DP_W145C
	SE_R			SEQ_SE_DP_W280
	ADD_PARTICLE 	0,W_161_161_TRYATK_ICE, EMTFUNC_DEFENCE_POS
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 1, 2,  WE_TOOL_E1 | WE_TOOL_SSP,
	
	WAIT_FLAG

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	EXIT_PARTICLE	1,
	
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG
	
	SEQEND
