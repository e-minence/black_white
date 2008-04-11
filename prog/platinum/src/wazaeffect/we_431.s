//==============================================================================
/**
 * @file	we_001.s
 * @brief	はたく
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_Hataku
	
// =============================================================================
//
//
//	■はたく
//
//
// =============================================================================
WEST_Hataku:

	LOAD_PARTICLE_DROP	0,W_431_SPA

	SE_L			SEQ_SE_DP_W088
	
	
	LOOP_LABEL		3
	
		FUNC_CALL		WEST_SP_WE_T10, 4,  4, 0,  +8, WE_TOOL_M1 | WE_TOOL_SSP
		
		
		ADD_PARTICLE 	0,W_431_431_STONE_BURN, EMTFUNC_FIELD_OPERATOR
		EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
		EX_DATA			4, OPERATOR_EX_REVERCE_OFF, 0, PT_LCD_PTP_CHG(-20), 0
		
		WAIT		4
		
		FUNC_CALL		WEST_SP_WE_T10, 4,  4, 0,  -8, WE_TOOL_M1 | WE_TOOL_SSP
		
		WAIT		4

	LOOP

	///< BEAM
	FUNC_CALL		WEST_SP_WE_T10,	4, 4, -16, +8, WE_TOOL_M1 | WE_TOOL_SSP
	SE_L			SEQ_SE_DP_W036
	WAIT_FLAG
	
	WAIT			10
	
	FUNC_CALL		WEST_SP_WE_T10,	4, 4, +32, -16, WE_TOOL_M1 | WE_TOOL_SSP
	WAIT_FLAG
	
	SE_R			SEQ_SE_DP_W085

	ADD_PARTICLE 	0,W_431_431_HIT_MARK, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_431_431_BALL_BURN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_431_431_HIT_MEMURI, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_B_X, 0, SHAKE_B_WAIT, SHAKE_B_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,

	FUNC_CALL		WEST_SP_WE_T10,	4, 4, -16, +8, WE_TOOL_M1 | WE_TOOL_SSP
	WAIT_FLAG
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

