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
//	■ミラクル愛
//
//
// =============================================================================
WEST_Hataku:

	LOAD_PARTICLE_DROP	0,W_357_SPA

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	WAIT_FLAG
	
	ADD_PARTICLE 	0,W_357_357_EYE_CIRCLE, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_LSP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	ADD_PARTICLE 	0,W_357_357_EYE_DOUKOU, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_LSP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	ADD_PARTICLE 	0,W_357_357_EYE_SIROME, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_LSP, OPERATOR_AXIS_AT, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	ADD_PARTICLE 	0,W_357_357_EYE_LINE,	EMTFUNC_DEFENCE_POS
	
	WAIT			5
	SE_L			SEQ_SE_DP_183
	WAIT			25
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_WHITE, WEDEF_FADE_PARAM, 25

	SE_R			SEQ_SE_DP_210

	WAIT_PARTICLE
	EXIT_PARTICLE	0,

FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
WAIT_FLAG
	
	SEQEND
