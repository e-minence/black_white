//==============================================================================
/**
 * @file	we_245.s
 * @brief	しんそく			245
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_SINSOKU
	
// =============================================================================
//
//
//	■しんそく			245
//
//
// =============================================================================

#define LOOP_NUM	(3)
#define MOVE_VAL_X	(8)
#define MOVE_VAL_RX	(LOOP_NUM * MOVE_VAL_X * -1)

WEST_SINSOKU:
	LOAD_PARTICLE_DROP	1,W_001_SPA
	LOAD_PARTICLE_DROP	0,W_245_SPA
	
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 1
	HAIKEI_CHG		BG_ID_097, WEDEF_HAIKEI_MODE_FADE | WEDEF_HAIKEI_SBIT_MOVE
	
	ADD_PARTICLE 	0,W_245_245_LINE_WHITE,		EMTFUNC_DEFENCE_POS

	HAIKEI_CHG_WAIT
	
	WAIT			1
	FUNC_CALL		WEST_SP_WE_T03, 2, 5, 0,

	SE_L			SEQ_SE_DP_W013B,2,2
	FUNC_CALL		WEST_SP_WE_100, 0,0,0,0,0,0,0,0,0
	ADD_PARTICLE 	0,W_245_245_KEMURI_WHITE,	EMTFUNC_ATTACK_POS
	WAIT_FLAG
	
	///< 途切れるので呼んでみる
	ADD_PARTICLE 	0,W_245_245_LINE_WHITE, EMTFUNC_DEFENCE_POS

	LOOP_LABEL		LOOP_NUM
		
		SE_R			SEQ_SE_DP_030
		ADD_PARTICLE 	1,W_001_001_HIT_MARK, EMTFUNC_DEFENCE_POS
		FUNC_CALL		WEST_SP_WE_T04, 3, 1, MOVE_VAL_X,  WE_TOOL_E1 | WE_TOOL_SSP,
		WAIT_FLAG
		FUNC_CALL		WEST_SP_WT_SHAKE, 5, 1, 0, 1, 4,  WE_TOOL_E1 | WE_TOOL_SSP,
		WAIT_FLAG
	
	LOOP
	
	SE_REPEAT_L		SEQ_SE_DP_W104,2,2
	FUNC_CALL		WEST_SP_WE_T05, 3, 4, MOVE_VAL_RX, WE_TOOL_E1 | WE_TOOL_SSP
	WAIT_FLAG
	
	FUNC_CALL		WEST_SP_WE_T03, 2, 5, 0,
	WAIT_FLAG
	CATS_RES_FREE	0

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	WORK_SET		WEDEF_GP_INDEX_SPEED_X,  WEDEF_HAIKEI_SPEED_X
	WORK_SET		WEDEF_GP_INDEX_SPEED_Y,  WEDEF_HAIKEI_SPEED_Y
	WORK_SET		WEDEF_GP_INDEX_SPEED_R,  1
	WORK_SET		WEDEF_GP_INDEX_SCREEN_R, 1
	HAIKEI_RECOVER	BG_ID_097, WEDEF_HAIKEI_RC_MODE_FADE | WEDEF_HAIKEI_SBIT_STOP

	HAIKEI_CHG_WAIT

	SEQEND
