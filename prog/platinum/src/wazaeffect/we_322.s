//==============================================================================
/**
 * @file	we_322.s
 * @brief	コスモパワー			322
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_KOSUMOPAWAA
	
// =============================================================================
//
//
//	■コスモパワー			322
//
//
// =============================================================================
#define WE322_FADE_WAIT		(60)

WEST_KOSUMOPAWAA:
	
	LOAD_PARTICLE_DROP	0,W_322_SPA
	
	PTAT_JP			PTAT	
	
	///< ポケモン暗く
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6, WE_TOOL_M2, 0, 1, WE_PAL_BLACK, 16, WE322_FADE_WAIT
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6, WE_TOOL_E1, 0, 1, WE_PAL_BLACK, 16, WE322_FADE_WAIT
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6, WE_TOOL_E2, 0, 1, WE_PAL_BLACK, 16, WE322_FADE_WAIT
	WAIT			10
	SE_C			SEQ_SE_DP_W322
	
	///< 背景切り替え
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE, 0
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  0
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  1
	HAIKEI_CHG		BG_ID_322, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_MOVE
	HAIKEI_CHG_WAIT

	ADD_PARTICLE 	0,W_322_322_COSMO_UP1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_322_322_COSMO_UP2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_322_322_RAND_BIG, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, 0, PT_LCD_PTP_CHG(-10), 0

	ADD_PARTICLE 	0,W_322_322_KIRA_BURN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WAIT_FLAG

	///< 背景切り替え待ち
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE, 0
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  0
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  1
	HAIKEI_RECOVER	BG_ID_322, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT

	SEQEND

PTAT:
	
	SIDE_JP		0, SIDE_MINE, SIDE_ENEMY
	
	SEQEND

SIDE_MINE:
	///< ポケモン暗く
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6, WE_TOOL_M2, 0, 1, WE_PAL_BLACK, 16, WE322_FADE_WAIT
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6, WE_TOOL_B, 0, 1, WE_PAL_BLACK, 16, WE322_FADE_WAIT
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6, WE_TOOL_D, 0, 1, WE_PAL_BLACK, 16, WE322_FADE_WAIT
	WAIT			10
	SE_C			SEQ_SE_DP_W322
	
	///< 背景切り替え
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE, 0
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  0
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  1
	HAIKEI_CHG		BG_ID_322, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_MOVE
	HAIKEI_CHG_WAIT

	ADD_PARTICLE 	0,W_322_322_COSMO_UP1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_322_322_COSMO_UP2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_322_322_RAND_BIG, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, 0, PT_LCD_PTP_CHG(-10), 0

	ADD_PARTICLE 	0,W_322_322_KIRA_BURN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WAIT_FLAG

	///< 背景切り替え待ち
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE, 0
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  0
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  1
	HAIKEI_RECOVER	BG_ID_322, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT
	
	SEQEND

SIDE_ENEMY:
	///< ポケモン暗く
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6, WE_TOOL_M2, 0, 1, WE_PAL_BLACK, 16, WE322_FADE_WAIT
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6, WE_TOOL_A, 0, 1, WE_PAL_BLACK, 16, WE322_FADE_WAIT
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6, WE_TOOL_C, 0, 1, WE_PAL_BLACK, 16, WE322_FADE_WAIT
	WAIT			10
	SE_C			SEQ_SE_DP_W322
	
	///< 背景切り替え
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE, 0
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  0
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  1
	HAIKEI_CHG		BG_ID_322, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_MOVE
	HAIKEI_CHG_WAIT

	ADD_PARTICLE 	0,W_322_322_COSMO_UP1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_322_322_COSMO_UP2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	
	ADD_PARTICLE 	0,W_322_322_RAND_BIG, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, 0, PT_LCD_PTP_CHG(-10), 0

	ADD_PARTICLE 	0,W_322_322_KIRA_BURN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WAIT_FLAG

	///< 背景切り替え待ち
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE, 0
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  0
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  1
	HAIKEI_RECOVER	BG_ID_322, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT
	
	SEQEND