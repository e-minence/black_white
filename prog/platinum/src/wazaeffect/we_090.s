//==============================================================================
/**
 * @file	we_090.s
 * @brief	じわれ			90
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_ZIWARE
	
// =============================================================================
//
//
//	■じわれ			90
//
//
// =============================================================================
#define W090_RES_NO			(0)
#define W090_RES_NUM		(2)
#define W090_ACT_NUM		(1)

WEST_ZIWARE:

	LOAD_PARTICLE_DROP	0,W_090_SPA
	
	CATS_RES_INIT			W090_RES_NO, W090_ACT_NUM, W090_RES_NUM, W090_RES_NUM, W090_RES_NUM, W090_RES_NUM, 0, 0

	CATS_CAHR_RES_LOAD		W090_RES_NO, EFFECT_090_NCGR_BIN
	CATS_PLTT_RES_LOAD		W090_RES_NO, EFFECT_090_NCLR, 1
	CATS_CELL_RES_LOAD		W090_RES_NO, EFFECT_090_NCER_BIN
	CATS_CELL_ANM_RES_LOAD	W090_RES_NO, EFFECT_090_NANR_BIN
	
	///< パーティクル
	ADD_PARTICLE 	0,W_090_090_JIWARE_SOTNE01, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_090_090_JIWARE_SOTNE02, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_090_090_JIWARE_SOTNE03, EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_090_090_JIWARE_SOTNE04, EMTFUNC_DEFENCE_POS

	///< 背景切り替え
	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE,  0
	HAIKEI_CHG		BG_ID_090, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_NONE
	HAIKEI_CHG_WAIT
		
	///< ゆれ
	SE_C			SEQ_SE_DP_W090
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_B_X, 0, SHAKE_M_WAIT, 30,  WE_TOOL_E1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_BG_SHAKE, 5, 8, 0, 0, 31, 0
	
	CATS_ACT_ADD	W090_RES_NO, WEST_CSP_WE_090, EFFECT_090_NCGR_BIN, EFFECT_090_NCLR, EFFECT_090_NCER_BIN, EFFECT_090_NANR_BIN, 0, 0, 0

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WAIT_FLAG

	WORK_SET		WEDEF_GP_INDEX_FADE_TYPE,  0
	HAIKEI_RECOVER	BG_ID_090, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_NONE
	HAIKEI_CHG_WAIT
	
	SEQEND
