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
WEST_Hataku:

	LOAD_PARTICLE_DROP	0,W_451_SPA

	PTAT_JP			PTAT	
	CONTEST_JP		WEST_CONTEST
	
	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	WAIT_FLAG
	
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE01, EMTFUNC_ATTACK_POS
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE02, EMTFUNC_ATTACK_POS
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE03, EMTFUNC_ATTACK_POS
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE04, EMTFUNC_ATTACK_POS
	
	SE_REPEAT_L			SEQ_SE_DP_206,4,5

	WAIT				80
	SE_FLOW_LR			SEQ_SE_DP_291
	
	ADD_PARTICLE_SEP	0, W_451_451_CHARGE_BEAM01, W_451_451_CHARGE_BEAM02, W_451_451_CHARGE_BEAM03, W_451_451_CHARGE_BEAM04, W_451_451_CHARGE_BEAM05, W_451_451_CHARGE_BEAM06, EMTFUNC_SEP_POS
	
	WAIT				5
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM*2,  WE_TOOL_E1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_YELLOW, WEDEF_FADE_PARAM, 10
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG	

	SEQEND

PTAT:

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	WAIT_FLAG
	
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE01, EMTFUNC_ATTACK_POS
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE02, EMTFUNC_ATTACK_POS
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE03, EMTFUNC_ATTACK_POS
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE04, EMTFUNC_ATTACK_POS
	
	SE_REPEAT_L			SEQ_SE_DP_206,4,5
	WAIT				80
	SE_FLOW_LR			SEQ_SE_DP_291
	
	ADD_PARTICLE_PTAT	0, W_451_451_CHARGE_BEAM_PARTY1, W_451_451_CHARGE_BEAM_PARTY2, W_451_451_CHARGE_BEAM_PARTY1, W_451_451_CHARGE_BEAM_PARTY2, EMTFUNC_ATTACK_POS
	
	WAIT				5
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM*2,  WE_TOOL_E1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_YELLOW, WEDEF_FADE_PARAM, 10
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG	

	SEQEND

WEST_CONTEST:

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 0, 12, WE_PAL_BLACK,
	WAIT_FLAG

	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE01, EMTFUNC_ATTACK_POS
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE02, EMTFUNC_ATTACK_POS
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE03, EMTFUNC_ATTACK_POS
	ADD_PARTICLE		0, W_451_451_DENKI_CHARGE04, EMTFUNC_ATTACK_POS
	
	SE_REPEAT_L			SEQ_SE_DP_206,4,5
	WAIT				80
	SE_FLOW_LR			SEQ_SE_DP_291
	
	ADD_PARTICLE		0, W_451_451_CHARGE_BEAM_CNT, EMTFUNC_DUMMY
	
	WAIT				5
	FUNC_CALL		WEST_SP_WT_SHAKE, 5, SHAKE_M_X, 0, SHAKE_M_WAIT, SHAKE_M_NUM*2,  WE_TOOL_E1 | WE_TOOL_SSP,
	FUNC_CALL		WEST_SP_WE_SSP_POKE_PAL_FADE, 6,  WE_TOOL_E1, 0, 1, WE_PAL_YELLOW, WEDEF_FADE_PARAM, 10
	
	WAIT_PARTICLE
	EXIT_PARTICLE	0,

	FUNC_CALL		WEST_SP_WE_HAIKEI_PAL_FADE, 5, 0, 1, 12, 0, WE_PAL_BLACK,
	WAIT_FLAG	

	SEQEND
