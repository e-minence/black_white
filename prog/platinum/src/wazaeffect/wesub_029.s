//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	none
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
	.text
	
	.include	"west.h"

	.global		WEST_ANGER

#define ITYAMON_RES_NO		(0)
#define ITYAMON_RES_NUM		(2)
#define ITYAMON_ACT_NUM		(7)

WEST_ANGER:
	
	///<初期化処理　読み込み処理
	CATS_RES_INIT		ITYAMON_RES_NO, ITYAMON_ACT_NUM, ITYAMON_RES_NUM, ITYAMON_RES_NUM, ITYAMON_RES_NUM, ITYAMON_RES_NUM, 0, 0

	CATS_CAHR_RES_LOAD	ITYAMON_RES_NO, EFFECT_207_NCGR_BIN
	CATS_PLTT_RES_LOAD	ITYAMON_RES_NO, EFFECT_207_NCLR, 1
	CATS_CELL_RES_LOAD	ITYAMON_RES_NO, EFFECT_207_NCER_BIN
	CATS_CELL_ANM_RES_LOAD	ITYAMON_RES_NO, EFFECT_207_NANR_BIN

	SE_REPEAT_R			SEQ_SE_DP_W207B,10,2

	CATS_ACT_ADD		ITYAMON_RES_NO, WEST_CSP_WE_207_SUB, EFFECT_207_NCGR_BIN, EFFECT_207_NCLR, EFFECT_207_NCER_BIN, EFFECT_207_NANR_BIN, 0, 0, 0,0,

	WAIT	16
	WAIT_FLAG

	///<破棄処理
	CATS_RES_FREE		ITYAMON_RES_NO
	
	SEQEND
