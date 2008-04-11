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

	LOAD_PARTICLE_DROP	0,W_383_SPA

	SE_R			SEQ_SE_DP_SHUSHU
	
	CONTEST_JP		CONTEST

	ADD_PARTICLE 	0,W_383_383_MANE_BALL1, EMTFUNC_DEFENCE_POS
	
	ADD_PARTICLE 	0,W_383_383_MANE_BALL2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_SP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_383_383_MANE_BALL3, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_SP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_383_383_MANE_BALL4, EMTFUNC_ATTACK_POS

	WAIT			35
	SE_FLOW_RL		SEQ_SE_DP_W383
	WAIT			10
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_BROWN, WEDEF_FADE_PARAM, 10
	
	//SE_L			SEQ_SE_DP_211

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

CONTEST:

	ADD_PARTICLE 	0,W_383_383_MANE_BALL1, EMTFUNC_DEFENCE_POS
	
	ADD_PARTICLE 	0,W_383_383_MANE_BALL2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_SP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_383_383_MANE_BALL3, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_AT, OPERATOR_POS_SP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	ADD_PARTICLE 	0,W_383_383_MANE_BALL4, EMTFUNC_ATTACK_POS

	WAIT			35
	SE_FLOW_RL		SEQ_SE_DP_W383
	WAIT			10
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_M1, 0, 1, WE_PAL_BROWN, WEDEF_FADE_PARAM, 10
	
	//SE_L			SEQ_SE_DP_211

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND