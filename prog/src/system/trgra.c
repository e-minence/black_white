//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		trgra.c
 *	@brief	トレーナーグラフィック素材の取得
 *	@author	HisashiSogabe
 *	@data		2010.01.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//アーカイブ
#include "arc_def.h"
#include "pokegra/pokegra_wb.naix"

//ポケツール
#include "poke_tool/monsno_def.h"
#include "poke_tool/poke_tool.h"

//外部参照
#include "system/trgra.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//トレーナー一体を構成するMCSS用ファイルの構成
enum{
	TRGRA_NCGR,
	TRGRA_NCBR,
	TRGRA_NCER,
	TRGRA_NANR,
	TRGRA_NMCR,
	TRGRA_NMAR,
	TRGRA_NCEC,
	TRGRA_NCLR,

	TRGRA_FILE_MAX,			//トレーナー一体を構成するMCSS用ファイルの総数
};

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================

//=============================================================================
/**
 *		リソース取得関数
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	POKEGRAのアーカイブID
 *		
 *	@return	アーカイブID
 */
//-----------------------------------------------------------------------------
ARCID TRGRA_GetArcID( void )
{	
	return ARCID_TRFGRA;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  トレーナータイプ
 *
 *	@return	キャラアーカイブインデックス
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetCgrArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//トレーナータイプからファイルのオフセットを計算

	return file_start + TRGRA_NCGR;
}

//----------------------------------------------------------------------------
/**
 *	@brief	
 *
 *  @param	tr_type  トレーナータイプ
 *
 *	@return	キャラビットマップアーカイブインデックス
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetCbrArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//トレーナータイプからファイルのオフセットを計算

	return file_start + TRGRA_NCBR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  トレーナータイプ
 *
 *	@return	ﾊﾟﾚｯﾄアーカイブインデックス
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetPalArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//トレーナータイプからファイルのオフセットを計算

	return file_start + TRGRA_NCLR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  トレーナータイプ
 *
 *	@return	セルアーカイブインデックス
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetCelArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//トレーナータイプからファイルのオフセットを計算

	return file_start + TRGRA_NCER;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  トレーナータイプ
 *
 *	@return	セルアニメアーカイブインデックス
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetAnmArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//トレーナータイプからファイルのオフセットを計算

	return file_start + TRGRA_NANR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  トレーナータイプ
 *
 *	@return	マルチセルアーカイブインデックス
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetMCelArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//トレーナータイプからファイルのオフセットを計算

	return file_start + TRGRA_NMCR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  トレーナータイプ
 *
 *	@return	マルチセルアニメアーカイブインデックス
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetMAnmArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//トレーナータイプからファイルのオフセットを計算

	return file_start + TRGRA_NMCR;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *  @param	tr_type  トレーナータイプ
 *
 *	@return	アーカイブインデックス
 */
//-----------------------------------------------------------------------------
ARCDATID TRGRA_GetNcecArcIndex( int tr_type )
{	
	int	file_start = TRGRA_FILE_MAX * tr_type;	//トレーナータイプからファイルのオフセットを計算

	return file_start + TRGRA_NCEC;
}

