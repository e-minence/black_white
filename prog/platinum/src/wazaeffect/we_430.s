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

	LOAD_PARTICLE_DROP	0,W_430_SPA

FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
WAIT_FLAG

	SE_REPEAT_L		SEQ_SE_DP_W082,3,10
		
	ADD_PARTICLE 	0,W_430_430_LIGHT_RGB, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_430_430_LIGHT_BIG1,EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_430_430_KIRA_BURN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_430_430_LIGHT_BIG2,EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_430_430_RING_SPIN1,EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_430_430_RING_SPIN2,EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_EP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	WAIT			90
	SE_REPEAT_R		SEQ_SE_DP_161,2,2

	ADD_PARTICLE 	0,W_430_430_BALL_BEAM1,EMTFUNC_ATTACK_POS
	FUNC_CALL		WEST_SP_EMIT_STRAIGHT, 6, 0, 0, 0, 0, 10, 64,
	
	WAIT			10
	SE_R			SEQ_SE_DP_400
	
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,
	ADD_PARTICLE 	0,W_430_430_HIT_MARK, EMTFUNC_DEFENCE_POS

	WAIT_PARTICLE
	EXIT_PARTICLE	0,

FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
WAIT_FLAG
	
	SEQEND

