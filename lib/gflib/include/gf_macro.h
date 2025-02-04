//=============================================================================================
/**
 * @file	gf_macro.h
 * @brief	共通マクロ定義用ヘッダ
 * @author	GAME FREAK inc.
 * @date	2006.11.28
 */
//=============================================================================================

#ifndef	__GF_MACRO_H__
#define	__GF_MACRO_H__

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
/**
 * @brief	配列の要素数を取り出すためのマクロ
 */
//--------------------------------------------------------------------
#define	NELEMS(array)	( sizeof(array) / sizeof(array[0]) )

//--------------------------------------------------------------------
/**
 * @brief	切り上げ計算用マクロ
 * @param	value		元の値
 * @param	base		切り上げの基準値
 * @return	切り上げた値
 *
 */
//--------------------------------------------------------------------
#define	ROUNDUP(value, base)	((((value) + (base -1)) / (base)) * (base))

//--------------------------------------------------------------
/**
 * @brief   4byteアライメントマクロ
 */
//--------------------------------------------------------------
#define	ALIGN4 __attribute__((aligned(4)))


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif	/*	__GF_MACRO_H__ */
