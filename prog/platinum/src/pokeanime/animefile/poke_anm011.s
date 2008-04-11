//==============================================================================
/**
 * @file	poke_anm011.s
 * @brief	横とびはね　スロー
 * @author	saito
 * @date	
 *
 */
//==============================================================================

	.text
	
	.include	"past.h"
	.include	"past_def.h"
	
	.global		PAST_ANIME011
	
// =============================================================================
//
//
//	横とびはね　スロー
//
//
// =============================================================================
PAST_ANIME011:
	//>飛び跳ね1回目
	//8回の処理で0x8000（180°）を処理
	CALL_MF_CURVE_DIVTIME	APPLY_SET,0,CURVE_SIN_MINUS,TARGET_DY,10,0x8000,0,8
	//8回の処理で8移動
	CALL_MF_LINE_DIVTIME	APPLY_SET,0,TARGET_DX,8,8
	//8回の処理で0x1000(22.5°)傾く
	CALL_MF_LINE_DIVTIME	APPLY_SET,0,TARGET_ROT,0x1000,8
	HOLD_CMD
	
	//>飛び跳ね2〜4回目
	START_LOOP	2

		//スプライト位置をx=8に設定 dxを0に設定
		SET_ADD_PARAM		PARAM_X,USE_VAL,8,PARAM_SET
		SET_ADD_PARAM		PARAM_DX,USE_VAL,0,PARAM_SET
		APPLY_TRANS
		//スプライト回転を0x1000にセット
		SET_ADD_PARAM		PARAM_ROT,USE_VAL,0x1000,PARAM_SET
		APPLY_AFFINE
	
		//12回の処理で0x8000（180°）を処理
		CALL_MF_CURVE_DIVTIME	APPLY_SET,0,CURVE_SIN_MINUS,TARGET_DY,10,0x8000,0,12
		//12回の処理で-16移動
		CALL_MF_LINE_DIVTIME	APPLY_SET,0,TARGET_DX,-16,12
		//12回の処理で0x1000〜-0x1000の-0x2000(45°)分傾く
		CALL_MF_LINE_DIVTIME	APPLY_ADD,0,TARGET_ROT,-0x2000,12
		HOLD_CMD

		//スプライト位置をx=-8に設定 dxを0に設定
		SET_ADD_PARAM		PARAM_X,USE_VAL,-8,PARAM_SET
		SET_ADD_PARAM		PARAM_DX,USE_VAL,0,PARAM_SET
		APPLY_TRANS
		//スプライト回転を-0x1000にセット
		SET_ADD_PARAM		PARAM_ROT,USE_VAL,-0x1000,PARAM_SET
		APPLY_AFFINE

		//12回の処理で0x8000（180°）を処理
		CALL_MF_CURVE_DIVTIME	APPLY_SET,0,CURVE_SIN_MINUS,TARGET_DY,10,0x8000,0,12
		//12回の処理で16移動
		CALL_MF_LINE_DIVTIME	APPLY_SET,0,TARGET_DX,16,12
		//12回の処理で-0x1000〜0x1000の0x2000(45°)分傾く
		CALL_MF_LINE_DIVTIME	APPLY_ADD,0,TARGET_ROT,0x2000,12
		HOLD_CMD

	END_LOOP

	//スプライト位置をx=8に設定 dxを0に設定
	SET_ADD_PARAM		PARAM_X,USE_VAL,8,PARAM_SET
	SET_ADD_PARAM		PARAM_DX,USE_VAL,0,PARAM_SET
	APPLY_TRANS
	//スプライト回転を0x1000にセット
	SET_ADD_PARAM		PARAM_ROT,USE_VAL,0x1000,PARAM_SET
	APPLY_AFFINE

	//>飛び跳ね5回目
	//12回の処理で0x8000（180°）を処理
	CALL_MF_CURVE_DIVTIME	APPLY_SET,0,CURVE_SIN_MINUS,TARGET_DY,10,0x8000,0,12
	//12回の処理で-16移動
	CALL_MF_LINE_DIVTIME	APPLY_SET,0,TARGET_DX,-16,12
	//12回の処理で0x1000〜-0x1000の-0x2000(45°)分傾く
	CALL_MF_LINE_DIVTIME	APPLY_ADD,0,TARGET_ROT,-0x2000,12
	HOLD_CMD

	//スプライト位置をx=-8に設定 dxを0に設定
	SET_ADD_PARAM		PARAM_X,USE_VAL,-8,PARAM_SET
	SET_ADD_PARAM		PARAM_DX,USE_VAL,0,PARAM_SET
	APPLY_TRANS
	//スプライト回転を-0x1000にセット
	SET_ADD_PARAM		PARAM_ROT,USE_VAL,-0x1000,PARAM_SET
	APPLY_AFFINE
	
	//>飛び跳ね6回目
	//12回の処理で0x8000（180°）を処理
	CALL_MF_CURVE_DIVTIME	APPLY_SET,0,CURVE_SIN_MINUS,TARGET_DY,10,0x8000,0,12
	//12回の処理で8移動
	CALL_MF_LINE_DIVTIME	APPLY_SET,0,TARGET_DX,8,12
	//12回の処理で0x1000(22.5°)傾く
	CALL_MF_LINE_DIVTIME	APPLY_ADD,0,TARGET_ROT,0x1000,12
	HOLD_CMD
	
	
	END_ANM
