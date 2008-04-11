//==============================================================================
/**
 * @file	we_209.s
 * @brief	スパーク			209
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_SUPAAKU
	
// =============================================================================
//
//
//	■スパーク			209
//
//
// =============================================================================
#define W206_BG_PAL_FADE		(0)
#define W206_BG_PAL_FADE_WAIT	(0)
#define W206_BG_PAL_FADE_COL	(WE_PAL_YELLOW)

#define W206_SSP_PAL_FADE_AT	(WE_TOOL_M1)
#define W206_SSP_PAL_FADE_NUM	(1)
#define W206_SSP_PAL_RGB		(W206_BG_PAL_FADE_COL)
#define W206_SSP_PAL_FADE_MAX	(10)
#define MOVE_VAL_X1		(WE_MOVE_X_VAL_DEF)

WEST_SUPAAKU:
	
	LOAD_PARTICLE_DROP	0,W_209_SPA
	ADD_PARTICLE 	0,W_209_209_SPARK_THANDER1, EMTFUNC_ATTACK_POS
	ADD_PARTICLE 	0,W_209_209_SPARK_THANDER2, EMTFUNC_ATTACK_POS

	SE_L			SEQ_SE_DP_W209

	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5, WE_TOOL_M1, 1, W206_SSP_PAL_FADE_NUM, W206_SSP_PAL_RGB, W206_SSP_PAL_FADE_MAX, 0,0,0
	LOOP_LABEL		3	
		//SE_L		SEQ_SE_DP_W085B

		FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, W206_BG_PAL_FADE, W206_BG_PAL_FADE_WAIT, 0, 8, W206_BG_PAL_FADE_COL,
		WAIT_FLAG
		FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, W206_BG_PAL_FADE, W206_BG_PAL_FADE_WAIT, 8, 0, W206_BG_PAL_FADE_COL,
		WAIT_FLAG
	LOOP
	
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 5, WE_TOOL_E1, 1, W206_SSP_PAL_FADE_NUM, W206_SSP_PAL_RGB, W206_SSP_PAL_FADE_MAX, 0,0,0
	WAIT			5
	ADD_PARTICLE 	0,W_209_209_SPARK_HIT2,		EMTFUNC_DEFENCE_POS
	ADD_PARTICLE 	0,W_209_209_SPARK_HIT,		EMTFUNC_DEFENCE_POS
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_S_X, 0, SHAKE_S_WAIT, SHAKE_S_NUM*3,  WE_TOOL_E1 | WE_TOOL_SSP,
	SE_R			SEQ_SE_DP_W086
	WAIT			1
	FUNC_CALL		WEST_SP_WE_T05, 3, 3, +MOVE_VAL_X1, WE_TOOL_M1 | WE_TOOL_SSP
	WAIT			5
	FUNC_CALL		WEST_SP_WE_T05, 3, 3, -MOVE_VAL_X1, WE_TOOL_M1 | WE_TOOL_SSP
	
	WAIT_FLAG
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
