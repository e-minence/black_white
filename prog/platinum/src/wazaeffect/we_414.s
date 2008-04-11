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
#define OFS_X1	(-32)
#define OFS_Y1	(-10)
#define OFS_X2	(0)
#define OFS_Y2	(-10)
#define OFS_X3	(+32)
#define OFS_Y3	(-10)
WEST_Hataku:
	LOAD_PARTICLE_DROP	0,W_414_SPA

FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_RED_BROWN,
WAIT_FLAG

	SEPLAY_PAN		SEQ_SE_DP_W053, WAZA_SE_PAN_DF
		
	///< 溜め
	ADD_PARTICLE 	0,W_414_414_FIRE_BURN1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X1), PT_LCD_PTP_CHG(OFS_Y1), 0

	ADD_PARTICLE 	0,W_414_414_FIRE_BURN2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X1), PT_LCD_PTP_CHG(OFS_Y1), 0

	ADD_PARTICLE 	0,W_414_414_BALL_BURN1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X1), PT_LCD_PTP_CHG(OFS_Y1), 0
	
	ADD_PARTICLE 	0,W_414_414_BALL_BURN2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X1), PT_LCD_PTP_CHG(OFS_Y1), 0
	
	ADD_PARTICLE 	0,W_414_414_BALL_BURN3, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X1), PT_LCD_PTP_CHG(OFS_Y1), 0
	
	FUNC_CALL		WEST_SP_BG_SHAKE, 5, 0, 5, 0, 5, 0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_RED_BROWN, WEDEF_FADE_PARAM, 0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,
	WAIT			20
	
	ADD_PARTICLE 	0,W_414_414_FIRE_BURN1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X2), PT_LCD_PTP_CHG(OFS_Y2), 0

	ADD_PARTICLE 	0,W_414_414_FIRE_BURN2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X2), PT_LCD_PTP_CHG(OFS_Y2), 0

	ADD_PARTICLE 	0,W_414_414_BALL_BURN1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X2), PT_LCD_PTP_CHG(OFS_Y2), 0
	
	ADD_PARTICLE 	0,W_414_414_BALL_BURN2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X2), PT_LCD_PTP_CHG(OFS_Y2), 0
	
	ADD_PARTICLE 	0,W_414_414_BALL_BURN3, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X2), PT_LCD_PTP_CHG(OFS_Y2), 0

	FUNC_CALL		WEST_SP_BG_SHAKE, 5, 0, 5, 0, 5, 0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_RED_BROWN, WEDEF_FADE_PARAM, 0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,
	WAIT			20
	
	ADD_PARTICLE 	0,W_414_414_FIRE_BURN1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X3), PT_LCD_PTP_CHG(OFS_Y3), 0

	ADD_PARTICLE 	0,W_414_414_FIRE_BURN2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X3), PT_LCD_PTP_CHG(OFS_Y3), 0

	ADD_PARTICLE 	0,W_414_414_BALL_BURN1, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X3), PT_LCD_PTP_CHG(OFS_Y3), 0
	
	ADD_PARTICLE 	0,W_414_414_BALL_BURN2, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X3), PT_LCD_PTP_CHG(OFS_Y3), 0
	
	ADD_PARTICLE 	0,W_414_414_BALL_BURN3, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_EP_OFS, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE
	EX_DATA			4, OPERATOR_EX_REVERCE_OFF, PT_LCD_PTP_CHG(OFS_X3), PT_LCD_PTP_CHG(OFS_Y3), 0
	
	FUNC_CALL		WEST_SP_BG_SHAKE, 5, 0, 5, 0, 5, 0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_RED_BROWN, WEDEF_FADE_PARAM, 0
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM,  WE_TOOL_E1 | WE_TOOL_SSP,
	WAIT_FLAG

	WAIT_PARTICLE
	EXIT_PARTICLE	0,


FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_RED_BROWN,
WAIT_FLAG
	
	SEQEND

