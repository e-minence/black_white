//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		tr2dgra.c
 *	@brief	トレーナーOBJ,BGグラフィック（ナギーが作ったpoke2dgra.cを丸パクリ）
 *	@author	sogabe
 *	@data		2010.02.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#define	TRGRA_LZ	// トレーナーグラフィック圧縮有効定義

//ライブラリ
#include <gflib.h>

//アーカイブ
#include "arc_def.h"
#include "pokegra/pokegra_wb.naix"

//リソース
#include "system/pokegra.h"
#include "app/app_menu_common.h"

//外部参照
#include "system/tr2dgra.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================

#define TR_DIR_FRONT  ( 0 )   //基本正面しか取らない

//=============================================================================
/**
 *					プロトタイプ
 */
//=============================================================================
static void* TR2DGRA_LoadCharacter( NNSG2dCharacterData **ncg_data, int trtype, HEAPID heapID );
//=============================================================================
/**
 *					外部公開
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	NNSG2dCharacterData **ncg_data
 *	@param	trtype
 *	@param	heapID 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void* TR2DGRA_LoadCharacter( NNSG2dCharacterData **ncg_data, int trtype, HEAPID heapID )
{	
	u32 cgr;
	void *p_buf;

	//リソース受け取り
	cgr	= TRGRA_GetCgrArcIndex( trtype, TR_DIR_FRONT );
	//リソースはOBJとして作っているので、LoadOBJじゃないと読み込めない
#ifdef	TRGRA_LZ
	p_buf = GFL_ARC_UTIL_LoadOBJCharacter( TRGRA_GetArcID( TR_DIR_FRONT ), cgr, TRUE, ncg_data, heapID );
#else	// TRGRA_LZ
	p_buf = GFL_ARC_UTIL_LoadOBJCharacter( TRGRA_GetArcID( TR_DIR_FRONT ), cgr, FALSE, ncg_data, heapID );
#endif	// TRGRA_LZ

	return p_buf;
}


//=============================================================================
/**
 *					BG関係
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG貼り付け用に、キャラ、ﾊﾟﾚｯﾄ読み込み
 *
 *	@param	trtype		トレーナータイプ
 *	@param	frm				読込先フレーム
 *	@param	chr_offs	キャラオフセット
 *	@param	plt_offs	パレットオフセット(0〜15)
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
void TR2DGRA_BG_TransResource( int trtype, u32 frm, u32 chr_offs, u32 plt_offs, HEAPID heapID )
{
	PALTYPE	paltype;
	ARCID		arc;
	ARCDATID	plt;

	//パレット読込先取得
	if( frm < GFL_BG_FRAME0_S )
	{	
		paltype	= PALTYPE_MAIN_BG;
	}
	else
	{	
		paltype	= PALTYPE_SUB_BG;
	}
	
	//リソース受け取り
	{	
		arc	= TRGRA_GetArcID( TR_DIR_FRONT );
		plt	= TRGRA_GetPalArcIndex( trtype, TR_DIR_FRONT );
	}

	//リソース読み込み
	{	
		ARCHANDLE *p_handle;
		void *p_buff;
		NNSG2dCharacterData *ncg_data;

		p_handle	= GFL_ARC_OpenDataHandle( arc, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, plt,
				paltype, plt_offs*0x20, TRGRA_TRAINER_PLT_SIZE, heapID );

		p_buff	= TR2DGRA_LoadCharacter( &ncg_data, trtype, heapID );
		GFL_BG_LoadCharacter( frm, ncg_data->pRawData, TRGRA_TRAINER_CHARA_SIZE, chr_offs );

		GFL_ARC_CloseDataHandle( p_handle );

		GFL_HEAP_FreeMemory( p_buff );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG貼り付け用に、キャラ、ﾊﾟﾚｯﾄ読み込みエリアマネージャ版
 *
 *	@param	trtype		トレーナータイプ
 *	@param	frm				読込先フレーム
 *	@param	chr_offs	キャラオフセット
 *	@param	plt				パレットオフセット(0〜15)
 *	@param	heapID		ヒープID
 *
 *
 */
//-----------------------------------------------------------------------------
GFL_ARCUTIL_TRANSINFO TR2DGRA_BG_TransResourceByAreaMan( int trtype, u32 frm, u32 plt_offs, HEAPID heapID )
{	
	PALTYPE	paltype;
	ARCID		arc;
	ARCDATID	plt;
	GFL_ARCUTIL_TRANSINFO info;

	//パレット読込先取得
	if( frm < GFL_BG_FRAME0_S )
	{	
		paltype	= PALTYPE_MAIN_BG;
	}
	else
	{	
		paltype	= PALTYPE_SUB_BG;
	}
	
	//リソース受け取り
	{	
		arc	= TRGRA_GetArcID( TR_DIR_FRONT );
		plt	= TRGRA_GetPalArcIndex( trtype, TR_DIR_FRONT );
	}

	//リソース読み込み
	{	
		ARCHANDLE *p_handle;
		u32 pos;
		void *p_buff;
		NNSG2dCharacterData *ncg_data;

		p_handle	= GFL_ARC_OpenDataHandle( arc, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, plt,
				paltype, plt_offs, TRGRA_TRAINER_PLT_SIZE, heapID );

		p_buff	= TR2DGRA_LoadCharacter( &ncg_data, trtype, heapID );
		pos = GFL_BG_LoadCharacterAreaMan( frm, ncg_data->pRawData, TRGRA_TRAINER_CHARA_SIZE );

		if(  pos == AREAMAN_POS_NOTFOUND )
		{	
			info	= GFL_ARCUTIL_TRANSINFO_FAIL;
		}
		else
		{	
			info	= GFL_ARCUTIL_TRANSINFO_Make(pos, POKEGRA_POKEMON_CHARA_SIZE);
		}

		GFL_ARC_CloseDataHandle( p_handle );
	}

	return info;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンに書き込み
 *
 *	@param	u32 frm		フレーム
 *	@param	chr_offs	キャラオフセット
 *	@param	pal_offs	パレットオフセット
 *	@param	x					書き込み開始位置X
 *	@param	y					書き込み開始位置Y
 *
 */
//-----------------------------------------------------------------------------
void TR2DGRA_BG_WriteScreen( u32 frm, u32 chr_offs, u32 pal_offs, u16 x, u16 y )
{	
	int xx, yy;
	int cnt;

	cnt	= 0;
	//OAMの8x8,2x4,2x4,4x2,4x2,2x2の順に並んでいる
	for( yy = 0; yy < 8; yy++ ){
		for( xx = 0; xx < 8; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx, y + yy, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 4; yy++ ){
		for( xx = 0; xx < 2; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx + 8, y + yy, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 4; yy++ ){
		for( xx = 0; xx < 2; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx + 8, y + yy + 4, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 2; yy++ ){
		for( xx = 0; xx < 4; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx, y + yy + 8, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 2; yy++ ){
		for( xx = 0; xx < 4; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx + 4, y + yy + 8, 1, 1, pal_offs );
			cnt++;
		}
	}

	for( yy = 0; yy < 2; yy++ ){
		for( xx = 0; xx < 2; xx++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + xx + 8, y + yy + 8, 1, 1, pal_offs );
			cnt++;
		}
	}
}

//=============================================================================
/**
 *					OBJ関係
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ハンドルオープン
 *	@param	HEAPID	ヒープID
 *	@return	ハンドル
 */
//-----------------------------------------------------------------------------
ARCHANDLE *TR2DGRA_OpenHandle( HEAPID heapID )
{	
	return GFL_ARC_OpenDataHandle( TRGRA_GetArcID( TR_DIR_FRONT ), heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーのパレット読み込み
 *
 *	@param	ARCHANDLE *p_handle			ハンドル
 *	@param	trtype									トレーナータイプ
 *	@param	vramType	              読み込みタイプ
 *	@param	byteOffs	              パレット読み込みオフセット
 *	@param	heapID		              ヒープID 
 *
 *	@return	登録ID
 */
//-----------------------------------------------------------------------------
u32 TR2DGRA_OBJ_PLTT_Register( ARCHANDLE *p_handle, int trtype, CLSYS_DRAW_TYPE vramType, u16 byteOffs, HEAPID heapID )
{	
	u32 plt;
	plt	= TRGRA_GetPalArcIndex( trtype, TR_DIR_FRONT );

	//読み込み
	return GFL_CLGRP_PLTT_RegisterEx( p_handle, plt, vramType, byteOffs, 0, TRGRA_TRAINER_PLT_NUM, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーキャラクター読み込み
 *
 *	@param	p_handle								アークハンドル
 *	@param	trtype									モンスター番号
 *	@param	vramType								読み込みタイプ
 *	@param	heapID									ヒープID
 *
 *	@return	登録番号
 */
//-----------------------------------------------------------------------------
u32 TR2DGRA_OBJ_CGR_Register( ARCHANDLE *p_handle, int trtype, CLSYS_DRAW_TYPE vramType, HEAPID heapID )
{	
	u32 cgr;
	cgr	= TRGRA_GetCgrArcIndex( trtype, TR_DIR_FRONT );
	//読み込み
#ifdef	TRGRA_LZ
	return GFL_CLGRP_CGR_Register( p_handle, cgr, TRUE, vramType, heapID );
#else	// TRGRA_LZ
	return GFL_CLGRP_CGR_Register( p_handle, cgr, FALSE, vramType, heapID );
#endif	// TRGRA_LZ
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーセル読み込み
 *
 *	@param	trtype		トレーナータイプ
 *	@param	mapping		マッピングモード
 *	@param	vramType	読み込みタイプ
 *	@param	heapID		ヒープID
 *
 *	@return
 */
//-----------------------------------------------------------------------------
u32 TR2DGRA_OBJ_CELLANM_Register( int trtype, APP_COMMON_MAPPING mapping, CLSYS_DRAW_TYPE vramType, HEAPID heapID )
{	
	ARCHANDLE *p_handle;
	u32 cel, anm;
	u32 ret;

  {	
    cel	= APP_COMMON_GetTrDummyCellArcIdx( mapping );
    anm	= APP_COMMON_GetTrDummyAnimeArcIdx( mapping );
  }

  p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );
  ret	= GFL_CLGRP_CELLANIM_Register( p_handle, cel, anm, heapID );
  GFL_ARC_CloseDataHandle( p_handle );

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーパレット&キャラクター再読み込み
 *
 *	@param	p_handle								アークハンドル
 *	@param	trtype									モンスター番号
 *	@param	vramType								読み込みタイプ
 *	@param	heapID									ヒープID
 *
 *	@return	登録番号
 */
//-----------------------------------------------------------------------------
void TR2DGRA_OBJ_RES_Replace( ARCHANDLE *p_handle, int trtype, int cgr_idx, int pltt_idx, HEAPID heapID )
{	
  {
    u32 pltt;
    u32* pltt_buf;
    NNSG2dPaletteData* pltt_res;

	  pltt	= TRGRA_GetPalArcIndex( trtype, TR_DIR_FRONT );
    pltt_buf = GFL_ARC_LoadDataAllocByHandle( p_handle, pltt, heapID );
    NNS_G2dGetUnpackedPaletteData( pltt_buf, &pltt_res );
    GFL_CLGRP_PLTT_Replace( pltt_idx, pltt_res, TRGRA_TRAINER_PLT_NUM );

    GFL_HEAP_FreeMemory( pltt_buf );
  }
  {
    NNSG2dCharacterData* cgr_res;
    u32* cgr_buf;
	  u32 cgr	= TRGRA_GetCgrArcIndex( trtype, TR_DIR_FRONT );

#ifdef	TRGRA_LZ
		cgr_buf = GFL_ARCHDL_UTIL_LoadOBJCharacter( p_handle, cgr, TRUE, &cgr_res, heapID );
#else	// TRGRA_LZ
    cgr_buf = GFL_ARC_LoadDataAllocByHandle( p_handle, cgr, heapID );
    NNS_G2dGetUnpackedCharacterData( cgr_buf, &cgr_res );
#endif	// TRGRA_LZ

    GFL_CLGRP_CGR_Replace( cgr_idx, cgr_res );
    
    GFL_HEAP_FreeMemory( cgr_buf );
  }
}

