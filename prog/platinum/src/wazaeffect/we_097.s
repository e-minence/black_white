//==============================================================================
/**
 * @file	we_097.s
 * @brief	こうそくいどう			97
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_KOUSOKUIDOU
	
// =============================================================================
//
//
//	■こうそくいどう			97
//
//
// =============================================================================
#define WE098_SHAKE_W_X	(2)		// ふり幅
#define WE098_SHAKE_NUM	(1)		// ふる回数
#define WE098_SHAKE_SYNC (1)	// シンク数
WEST_KOUSOKUIDOU:

	LOAD_PARTICLE_DROP	0, W_097_SPA
	
	SIDE_JP		0, SIDE_MINE, SIDE_ENEMY
	
	SEQEND

SIDE_MINE:

	POKEOAM_RES_INIT

	POKEOAM_RES_LOAD	0,
	POKEOAM_RES_LOAD	1,

	// 描画開始
	POKEOAM_DROP	WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_0, WEDEF_POKE_RES_0,
	POKEOAM_DROP	WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_1, WEDEF_POKE_RES_1,

	PT_DROP_EX		WEDEF_DROP_M2, WEDEF_DROP_M2,

	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_HI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	HAIKEI_CHG		BG_ID_097, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_MOVE
	SE_C			SEQ_SE_PL_W097
	
	ADD_PARTICLE 	0,W_097_097_LINE01, EMTFUNC_DUMMY
	
	HAIKEI_CHG_WAIT

/// -------------- 仮 --------------------------------
	// ポケモンOAM準備
	//SE_C			SEQ_SE_DP_KAZE
	
	WAIT			19
	//SE_L			SEQ_SE_DP_W104
	//SE_REPEAT_L		SEQ_SE_DP_W104,4,2
	WAIT			1
	
	FUNC_CALL		WEST_SP_WE_098, 0,0,0,0,0,0,0,0,0

	WAIT_FLAG	///<TCB待ち
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_1
	POKEOAM_RES_FREE
	PT_DROP_RESET_EX
/// -------------- 仮 --------------------------------

	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_HI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	HAIKEI_RECOVER	BG_ID_097, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND


SIDE_ENEMY:

	POKEOAM_RES_INIT

	POKEOAM_RES_LOAD	0,
	POKEOAM_RES_LOAD	1,

	// 描画開始
	POKEOAM_DROP	WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_0, WEDEF_POKE_RES_0,
	POKEOAM_DROP	WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_1, WEDEF_POKE_RES_1,
	PT_DROP_EX		WEDEF_DROP_M2, WEDEF_DROP_M2,

	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_HI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	HAIKEI_CHG		BG_ID_097, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_MOVE
	SE_C			SEQ_SE_PL_W097
	
	ADD_PARTICLE 	0,W_097_097_LINE02, EMTFUNC_DUMMY
	
	HAIKEI_CHG_WAIT

/// -------------- 仮 --------------------------------
	// ポケモンOAM準備
	//SE_C			SEQ_SE_DP_KAZE
	
	WAIT			19
	//SE_L			SEQ_SE_DP_W104
	//SE_REPEAT_L		SEQ_SE_DP_W104,4,2
	WAIT			1
	
	FUNC_CALL		WEST_SP_WE_098, 0,0,0,0,0,0,0,0,0

	WAIT_FLAG	///<TCB待ち
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_1
	POKEOAM_RES_FREE
	PT_DROP_RESET_EX
/// -------------- 仮 --------------------------------

	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_HI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	HAIKEI_RECOVER	BG_ID_097, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP
	HAIKEI_CHG_WAIT
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
