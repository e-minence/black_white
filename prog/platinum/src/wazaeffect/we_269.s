//==============================================================================
/**
 * @file	we_269.s
 * @brief	ちょうはつ			269
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_TYOUHATU
	
// =============================================================================
//
//
//	■ちょうはつ			269
//
//
// =============================================================================
#define W269_OAM_MAX	(1)
#define ITYAMON_RES_NO		(0)
#define ITYAMON_RES_NUM		(2)
#define ITYAMON_ACT_NUM		(7)
WEST_TYOUHATU:

	LOAD_PARTICLE_DROP	0,W_269_SPA
	
	SE_C			SEQ_SE_DP_W118
	SE_WAITPLAY_C	SEQ_SE_DP_W039,30
	SE_WAITPLAY_C	SEQ_SE_DP_W039,47

	ADD_PARTICLE 	0,W_269_269_BOMB1, EMTFUNC_DUMMY
	ADD_PARTICLE 	0,W_269_269_BOMB2, EMTFUNC_DUMMY

	CATS_RES_INIT			0, W269_OAM_MAX, 1,1,1,1,0,0
	CATS_CAHR_RES_LOAD		0, EFFECT_269_NCGR_BIN
	CATS_PLTT_RES_LOAD		0, EFFECT_269_NCLR, 1
	CATS_CELL_RES_LOAD		0, EFFECT_269_NCER_BIN
	CATS_CELL_ANM_RES_LOAD	0, EFFECT_269_NANR_BIN
	CATS_ACT_ADD			0, WEST_CSP_WE_269,  EFFECT_269_NCGR_BIN, EFFECT_269_NCLR, EFFECT_269_NCER_BIN, EFFECT_269_NANR_BIN, 0, 0, 1, W269_OAM_MAX
	WAIT_FLAG
	CATS_RES_FREE			0

	/// おこらせる。
	
	///<初期化処理　読み込み処理
	CATS_RES_INIT		ITYAMON_RES_NO, ITYAMON_ACT_NUM, ITYAMON_RES_NUM, ITYAMON_RES_NUM, ITYAMON_RES_NUM, ITYAMON_RES_NUM, 0, 0

	CATS_CAHR_RES_LOAD	ITYAMON_RES_NO, EFFECT_207_NCGR_BIN
	CATS_PLTT_RES_LOAD	ITYAMON_RES_NO, EFFECT_207_NCLR, 1
	CATS_CELL_RES_LOAD	ITYAMON_RES_NO, EFFECT_207_NCER_BIN
	CATS_CELL_ANM_RES_LOAD	ITYAMON_RES_NO, EFFECT_207_NANR_BIN
	
	CATS_ACT_ADD		ITYAMON_RES_NO, WEST_CSP_WE_207_SUB, EFFECT_207_NCGR_BIN, EFFECT_207_NCLR, EFFECT_207_NCER_BIN, EFFECT_207_NANR_BIN, 0, 0, 0,0,
	POKE_FADE_EZ	WE_PAL_RED, WE_TOOL_E1, 1

	WAIT	4
	//SE_REPEAT_R		SEQ_SE_DP_050,10,2
	SE_REPEAT_R			SEQ_SE_DP_W207B,10,2
	WAIT	10
	WAIT_FLAG

	///<破棄処理
	CATS_RES_FREE		ITYAMON_RES_NO

	WAIT_PARTICLE
	EXIT_PARTICLE	0,
	
	SEQEND
