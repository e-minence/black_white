//=============================================================================
/**
 *
 *	@file		gra_tool.c
 *	@brief  グラフィックツール
 *	@author	goto / hosaka genya(GSから移植)
 *  @date	  2005.11.16
 *	@data	  2009.10.31(GSから移植)
 *
 */
//=============================================================================
#include <gflib.h>

#include "system/gra_tool.h"

// =============================================================================
//
//
//	■画像変換
//
//
// =============================================================================

//=============================================================================
/**
 *	■定数定義
 */
//=============================================================================
// 2D→1D変換に使用
#define CHAR_DOT_X		(8/2)								///< 1キャラのバイトサイズ
#define CHAR_DOT_Y		(8)									///< 1キャラのバイトサイズ
#define CHAR_BYTE		(CHAR_DOT_X * CHAR_DOT_Y)			///< 1キャラバイト数

//=============================================================================
/**
 *	■プロトタイプ宣言
 */
//=============================================================================
// 1Charを2D→1D変換
static void CharCopy1_1D_from_2D(const u8* src, u8* dst, int* ofs, int* bofs, int cs, int len);

//=============================================================================
/**
 *	■外部公開関数
 */
//=============================================================================

//--------------------------------------------------------------
/**
 * @brief	rawdataを2D→1Dに変換する
 *
 * @param	pRawData			変換対象テクスチャデータ
 * @param	org_w				変換対象データ横幅
 * @param	x					変換対象矩形:x
 * @param	y					変換対象矩形:y
 * @param	w					変換対象矩形:w
 * @param	h					変換対象矩形:h
 * @param	buff				展開先バッファ
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void ChangesInto_RawData_1D_from_2D(void * pRawData, int  org_w, int x, int y, int w, int h, void* buff)
{
	u8* p_data = NULL;
	u8* p_buff = NULL;

	int size;
	int cs;
	int ofs;
	int bofs;
	int len;
	
	///< 準備
	cs     = (sizeof(u8) * 4);				///< コピーサイズ 8dot = 4Byte
	p_data = pRawData;				///< キャラデータ
	
	p_buff = (u8*)buff;						///< 展開バッファ
	
	len = (org_w * cs);				///< 長さ(元画像の幅)
	ofs = (x * cs) + (y * len);				///< 開始位置
	bofs = 0;								///< バッファの書き込み位置
	
	//全体ｺﾋﾟｰ
	{
		int i;
		int j;

		for (j = y; j < (y + h); j++){
			for (i = x; i < (x + w); i++){
				ofs = (i * cs) + (j * len * CHAR_DOT_Y);			///< 開始位置
				CharCopy1_1D_from_2D(p_data, p_buff, &ofs, &bofs, cs, len);	///< 1キャラコピー
			}
		}
	}
}


//=============================================================================
/**
 *								static関数
 */
//=============================================================================
//--------------------------------------------------------------
/**
 * @brief	2D→1Dに1キャラ変換
 *
 * @param	src			元データ
 * @param	dst			展開先
 * @param	ofs			元データのコピー開始位置
 * @param	bofs		展開先バッファの書き込み位置
 * @param	cs			コピーサイズ
 * @param	len			2Dデータの幅
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void CharCopy1_1D_from_2D(const u8* src, u8* dst, int* ofs, int* bofs, int cs, int len)
{
	int i;
	u32 dat;
	
	for (i = 0; i < 8; i++){
		memcpy(&dst[ *bofs ], &src[ *ofs ], cs);
		*ofs  += len;
		*bofs += cs;
	}
}

