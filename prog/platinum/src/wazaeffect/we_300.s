//==============================================================================
/**
 * @file	we_300.s
 * @brief	どろあそび			300
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_DOROASOBI
	
// =============================================================================
//
//
//	■どろあそび			300
//
//
// =============================================================================
WEST_DOROASOBI:
	
	LOAD_PARTICLE_DROP	0,W_300_SPA

	SE_L		SEQ_SE_DP_W091
	FUNC_CALL	WEST_SP_WE_SSP_POKE_SCALE_UPDOWN, 8, (WE_TOOL_M1 | WE_TOOL_SSP), 100, 120, 100, 80, 100, 1, (5 << 16) | 5
	WAIT_FLAG

	ADD_PARTICLE 	0,W_300_300_MUD_UP1, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_300_300_MUD_UP2, EMTFUNC_ATTACK_POS
	
	SE_L		SEQ_SE_DP_W253
	ADD_PARTICLE 	0,W_253_253_VOICE_RING, EMTFUNC_ATTACK_POS
	FUNC_CALL	WEST_SP_WE_SSP_POKE_SCALE_UPDOWN, 8, (WE_TOOL_M1 | WE_TOOL_SSP), 100, 80, 100, 160, 100, 1, (5 << 16) | 5
	WAIT_FLAG
	
	LOOP_LABEL		2
		SE_L		SEQ_SE_DP_W253
		FUNC_CALL	WEST_SP_WE_SSP_POKE_SCALE_UPDOWN, 8, (WE_TOOL_M1 | WE_TOOL_SSP), 100, 120, 100, 80, 100, 1, (4 << 16) | 4
		WAIT_FLAG
		FUNC_CALL	WEST_SP_WE_SSP_POKE_SCALE_UPDOWN, 8, (WE_TOOL_M1 | WE_TOOL_SSP), 100, 80, 100, 160, 100, 1, (4 << 16) | 4
		SE_L		SEQ_SE_DP_W253
		WAIT_FLAG
	
	LOOP

	WAIT			20
	SE_REPEAT_L		SEQ_SE_DP_W145,4,9

	///< 何個か落下させる

	ADD_PARTICLE 	0,W_300_300_MUD_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, +6000, -2000, +4000

	WAIT			2

	ADD_PARTICLE 	0,W_300_300_MUD_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, +12000, 0, -4000

	WAIT			3

	ADD_PARTICLE 	0,W_300_300_MUD_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, -10000, -1000, -4000
	
	WAIT			2
	ADD_PARTICLE 	0,W_300_300_MUD_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, -6000, +1000, +4000

	WAIT			5
	
	ADD_PARTICLE 	0,W_300_300_MUD_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, +4000, -1000, +3000

	WAIT			2
	
	ADD_PARTICLE 	0,W_300_300_MUD_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, +2000, -1500, +3000

	WAIT			4

	ADD_PARTICLE 	0,W_300_300_MUD_DOWN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, -5000, +2000, -4000

	SE_L			SEQ_SE_DP_W145B

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
