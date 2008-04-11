//==============================================================================
/**
 * @file	we_326.s
 * @brief	じんつうりき			326
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_ZINTUURIKI
	
// =============================================================================
//
//
//	■じんつうりき			326
//
//
// =============================================================================
// 光る
#define ZINTUURIKI_PAL_FADE_AT	(WE_TOOL_M1)
#define ZINTUURIKI_PAL_FADE_NUM	(2)
#define ZINTUURIKI_PAL_RGB		(WE_PAL_BLUE)
#define ZINTUURIKI_PAL_FADE_MAX	(10)

// OAM拡縮
#define ZINTUURIKI_OAM_AT			(0)
#define ZINTUURIKI_OAM_ALPHA		(8)
#define ZINTUURIKI_OAM_SCALE_S		(10)
#define ZINTUURIKI_OAM_SCALE_E		(15)
#define ZINTUURIKI_OAM_SCALE_D		(10)
#define ZINTUURIKI_OAM_SCALE_NUM	(1)
#define ZINTUURIKI_OAM_SYNC			((9<<16) | 9)

WEST_ZINTUURIKI:
	LOAD_PARTICLE_DROP	0,W_326_SPA

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	WAIT_FLAG

	SE_FLOW_LR			SEQ_SE_DP_W020
	
	///< パーティクル
	ADD_PARTICLE 	0,W_326_326_RING_BURN, EMTFUNC_FIELD_OPERATOR
	EX_DATA			OPERATOR_DEF_DATA_NUM, OPERATOR_PRI_NONE, OPERATOR_TARGET_DF, OPERATOR_POS_SP, OPERATOR_AXIS_NONE, OPERATOR_FLD_NONE, OPERATOR_CAMERA_NONE

	// ポケモンBG準備
	POKEBG_DROP		1,	WEDEF_POKE_AUTO_OFF

	// ポケモンOAM準備
	POKEOAM_RES_INIT

	POKEOAM_RES_LOAD	0,


	
	//攻撃ポケモン光る
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5, ZINTUURIKI_PAL_FADE_AT, 0, ZINTUURIKI_PAL_FADE_NUM, ZINTUURIKI_PAL_RGB, ZINTUURIKI_PAL_FADE_MAX, 0,0,0
	FUNC_CALL		WEST_SP_WE_326, 0,0

	LOOP_LABEL	3
		SE_FLOW_LR			SEQ_SE_DP_SHUSHU
		WAIT 16
	LOOP

	
	WAIT	(ZINTUURIKI_PAL_FADE_MAX*4)
	SE_L			SEQ_SE_DP_W043

	// 描画開始
	POKEOAM_DROP		WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_0, WEDEF_POKE_RES_0,
	PT_DROP_EX		WEDEF_DROP_M2, WEDEF_DROP_M2,

	//攻撃ポケモンOAM拡縮
	FUNC_CALL		WEST_SP_WE_CAP_POKE_SCALE_UPDOWN,  8, ZINTUURIKI_OAM_AT, ZINTUURIKI_OAM_ALPHA, ZINTUURIKI_OAM_SCALE_S, ZINTUURIKI_OAM_SCALE_E, ZINTUURIKI_OAM_SCALE_D, ZINTUURIKI_OAM_SCALE_NUM, ZINTUURIKI_OAM_SYNC, 0,

	WAIT_FLAG	///<TCB待ち

	// ポケモンOAM破棄
	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	PT_DROP_RESET_EX

	// ポケモンBG　リセット
	POKEBG_DROP_RESET 0

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG
		
	SEQEND
