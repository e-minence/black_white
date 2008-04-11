//==============================================================================
/**
 * @file	we_248.s
 * @brief	みらいよち			248
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_MIRAIYOTI
	
// =============================================================================
//
//
//	■みらいよち			248
//
//
// =============================================================================
WEST_MIRAIYOTI:
	
	TURN_CHK		WEST_248_TURN_1, WEST_248_TURN_2
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND

#define WE093_SHAKE_MOVE	(2)	// 横揺れ幅
#define WE093_SHAKE_SYNC	(1)	
#define WE093_SHAKE_ATNUM	(2)
// 光る
#define WE093_PAL_FADE_AT	(WE_TOOL_M1)
#define WE093_PAL_FADE_DF	(WE_TOOL_E1)
#define WE093_PAL_FADE_NUM	(1)
#define WE093_PAL_RGB		(WE_PAL_WHITE)
#define WE093_PAL_FADE_MAX	(4)

WEST_248_TURN_1:

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	WAIT_FLAG
	

	// ポケモンOAM準備
	POKEOAM_RES_INIT

	POKEOAM_RES_LOAD	0,

	// 描画開始
	POKEOAM_DROP	WEDEF_DROP_M1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_0, WEDEF_POKE_RES_0,

	FUNC_CALL		WEST_SP_WT_SHAKE, 5, WE093_SHAKE_MOVE, 0,WE093_SHAKE_SYNC, WE093_SHAKE_ATNUM, WE_TOOL_M1 | WE_TOOL_SSP, 0, 0
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5, WE093_PAL_FADE_AT, 0, WE093_PAL_FADE_NUM, WE093_PAL_RGB, WE093_PAL_FADE_MAX, 0,0,0

	WAIT	10
	SE_C		SEQ_SE_DP_W060
	
	WAIT_FLAG	///<TCB待ち

	// ポケモンOAM破棄
	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG

	SEQEND

WEST_248_TURN_2:

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	WAIT_FLAG
	

	// ポケモンOAM準備
	POKEOAM_RES_INIT

	POKEOAM_RES_LOAD	0,

	// 描画開始
	POKEOAM_DROP	WEDEF_DROP_E1, WEDEF_POKE_AUTO_OFF, WEDEF_POKE_CAP_ID_0, WEDEF_POKE_RES_0,
	PT_DROP_EX		WEDEF_DROP_E2, WEDEF_DROP_E2,

//	FUNC_CALL		WEST_SP_WT_SHAKE, 5, WE093_SHAKE_MOVE, 0,WE093_SHAKE_SYNC, WE093_SHAKE_ATNUM, WE_TOOL_M1 | WE_TOOL_SSP, 0, 0
//	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5, WE093_PAL_FADE_AT, 0, WE093_PAL_FADE_NUM, WE093_PAL_RGB, WE093_PAL_FADE_MAX, 0,0,0

	SE_C			SEQ_SE_DP_W060
	WAIT	10
	
	FUNC_CALL		WEST_SP_WE_093, 0,0,0,0,0,0,0,0,0
	SE_R			SEQ_SE_DP_480

	WAIT_FLAG	///<TCB待ち

	// ポケモンOAM破棄
	POKEOAM_RES_FREE
	POKEOAM_DROP_RESET	WEDEF_POKE_CAP_ID_0
	PT_DROP_RESET_EX


	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG

	SEQEND
