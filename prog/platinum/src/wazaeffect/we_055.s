//==============================================================================
/**
 * @file	we_055.s
 * @brief	みずでっぽう			55
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_MIZUDEPPOU
	
// =============================================================================
//
//
//	■みずでっぽう			55
//
//
// =============================================================================
WEST_MIZUDEPPOU:
	
	LOAD_PARTICLE_DROP	0,W_055_SPA
	
	PTAT_JP			PTAT
	CONTEST_JP		WEST_CONTEST
	
	ADD_PARTICLE_SEP 0, W_055_055_WATER_BEAM, W_055_055_WATER_BEAM02, W_055_055_WATER_BEAM03, W_055_055_WATER_BEAM04, W_055_055_WATER_BEAM06, W_055_055_WATER_BEAM05, EMTFUNC_SEP_POS

	//SE_R			SEQ_SE_DP_W145
	SE_FLOW_LR		SEQ_SE_DP_W055
	
	WAIT			10

	ADD_PARTICLE 	0,W_055_055_KEMURI_HIT1, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_055_055_KEMURI_HIT2, EMTFUNC_DEFENCE_POS
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,

	//SE_R			SEQ_SE_DP_W152
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WAIT_FLAG
	
	SEQEND

///< 2vs2
PTAT:
	
	ADD_PARTICLE_PTAT	0, W_055_055_WATER_BEAM_PARTY1, W_055_055_WATER_BEAM_PARTY2, W_055_055_WATER_BEAM_PARTY1, W_055_055_WATER_BEAM_PARTY2, EMTFUNC_ATTACK_POS

	//SE_R			SEQ_SE_DP_W145
	SE_FLOW_LR		SEQ_SE_DP_W055
	
	WAIT			10

	ADD_PARTICLE 	0,W_055_055_KEMURI_HIT1, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_055_055_KEMURI_HIT2, EMTFUNC_DEFENCE_POS
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,

	//SE_R			SEQ_SE_DP_W152
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WAIT_FLAG

	SEQEND

///< コンテスト
WEST_CONTEST:

	ADD_PARTICLE	0, W_055_055_WATER_BEAM_CNT, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_ON, +PT_LCD_PTP_CHG(58), -PT_LCD_PTP_CHG(44), 0

	//SE_R			SEQ_SE_DP_W145
	SE_FLOW_LR		SEQ_SE_DP_W055
	
	WAIT			10

	ADD_PARTICLE 	0,W_055_055_KEMURI_HIT1, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_055_055_KEMURI_HIT2, EMTFUNC_DEFENCE_POS
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,

	//SE_R			SEQ_SE_DP_W152
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WAIT_FLAG
	
	SEQEND
