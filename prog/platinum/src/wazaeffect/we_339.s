//==============================================================================
/**
 * @file	we_339.s
 * @brief	ビルドアップ			339
 * @author	goto
 * @date	2005.07.13(水)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
	
	.text
	
	.include	"west.h"

	.global		WEST_BIRUDOAPPU
	
// =============================================================================
//
//
//	■ビルドアップ			339
//
//
// =============================================================================
WEST_BIRUDOAPPU:

	SE_L			SEQ_SE_DP_W207
	SE_WAITPLAY_L	SEQ_SE_DP_W207D,20

	FUNC_CALL	WEST_SP_WE_339,	1, 0
	
	WAIT_FLAG
	
	SEQEND
