//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		poke2dgra.c
 *	@brief	ポケモンOBJ,BGグラフィック
 *	@author	Toru=Nagihashi
 *	@data		2009.10.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//リソース
#include "system/pokegra.h"
#include "app/app_menu_common.h"

//外部参照
#include "system/poke2dgra.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================

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
 *	@param	POKEMON_PASO_PARAM* ppp
 *	@param	dir
 *	@param	heapID 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
void* POKE2DGRA_LoadCharacter( NNSG2dCharacterData **ncg_data, const POKEMON_PASO_PARAM* ppp, int dir, HEAPID heapID )
{	
	u32 cgr;
	void *p_buf;

	//リソース受け取り
	{	
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );
		cgr	= POKEGRA_GetCgrArcIndex( mons_no, form_no, sex, rare, dir );
	}
	//リソースはOBJとして作っているので、LoadOBJじゃないと読み込めない
	p_buf = GFL_ARC_UTIL_LoadOBJCharacter( POKEGRA_GetArcID(), cgr, FALSE, ncg_data, heapID );

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
 *	@param	const POKEMON_PASO_PARAM* ppp	ポケモンパーソナル
 *	@param	dir														絵の方向
 *	@param	frm														読込先フレーム
 *	@param	chr_offs											キャラオフセット
 *	@param	plt_offs											パレットオフセット(0〜15)
 *	@param	heapID												ヒープID
 */
//-----------------------------------------------------------------------------
void POKE2DGRA_BG_TransResource( const POKEMON_PASO_PARAM* ppp, int dir, u32 frm, u32 chr_offs, u32 plt_offs, HEAPID heapID )
{
	PALTYPE	paltype;
	ARCID		arc;
	ARCDATID	cgr, plt;

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
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );

		arc	= POKEGRA_GetArcID();
		cgr	= POKEGRA_GetCgrArcIndex( mons_no, form_no, sex, rare, dir );
		plt	= POKEGRA_GetPalArcIndex( mons_no, form_no, sex, rare, dir );
	}

	//リソース読み込み
	{	
		ARCHANDLE *p_handle;
		void *p_buff;
		NNSG2dCharacterData *ncg_data;

		p_handle	= GFL_ARC_OpenDataHandle( arc, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, plt,
				paltype, plt_offs*0x20, POKEGRA_POKEMON_PLT_SIZE, heapID );

		p_buff	= POKE2DGRA_LoadCharacter( &ncg_data, ppp, dir, heapID );
		GFL_BG_LoadCharacter( frm, ncg_data->pRawData, POKEGRA_POKEMON_CHARA_SIZE, chr_offs );

		GFL_ARC_CloseDataHandle( p_handle );

		GFL_HEAP_FreeMemory( p_buff );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG貼り付け用に、キャラ、ﾊﾟﾚｯﾄ読み込みエリアマネージャ版
 *
 *	@param	const POKEMON_PASO_PARAM* ppp	ポケモンパーソナル
 *	@param	dir														絵の方向
 *	@param	frm														読込先フレーム
 *	@param	chr_offs											キャラオフセット
 *	@param	plt														パレットオフセット(0〜15)
 *	@param	heapID												ヒープID
 *
 *
 */
//-----------------------------------------------------------------------------
GFL_ARCUTIL_TRANSINFO POKE2DGRA_BG_TransResourceByAreaMan( const POKEMON_PASO_PARAM* ppp, int dir, u32 frm, u32 plt_offs, HEAPID heapID )
{	
	PALTYPE	paltype;
	ARCID		arc;
	ARCDATID	cgr, plt;
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
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );

		arc	= POKEGRA_GetArcID();
		cgr	= POKEGRA_GetCgrArcIndex( mons_no, form_no, sex, rare, dir );
		plt	= POKEGRA_GetPalArcIndex( mons_no, form_no, sex, rare, dir );
	}

	//リソース読み込み
	{	
		ARCHANDLE *p_handle;
		u32 pos;
		void *p_buff;
		NNSG2dCharacterData *ncg_data;

		p_handle	= GFL_ARC_OpenDataHandle( arc, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, plt,
				paltype, plt_offs, POKEGRA_POKEMON_PLT_SIZE, heapID );

		p_buff	= POKE2DGRA_LoadCharacter( &ncg_data, ppp, dir, heapID );
		pos = GFL_BG_LoadCharacterAreaMan( frm, ncg_data->pRawData, POKEGRA_POKEMON_CHARA_SIZE );

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
void POKE2DGRA_BG_WriteScreen( u32 frm, u32 chr_offs, u32 pal_offs, u16 x, u16 y )
{	
	int i, j;
	int cnt;

	cnt	= 0;
	//OAMの8x8,4x8,8x4,4x4の順に並んでいる
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 8; j++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + j, y + i, 1, 1, pal_offs );
			cnt++;
		}
	}
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 4; j++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + j + 8, y + i, 1, 1, pal_offs );
			cnt++;
		}
	}
	for( i = 0; i < 4; i++ ){
		for( j = 0; j < 8; j++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + j, y + i + 8, 1, 1, pal_offs );
			cnt++;
		}
	}
	for( i = 0; i < 4; i++ ){
		for( j = 0; j < 4; j++ ){
			GFL_BG_FillScreen( frm, chr_offs + cnt, x + j + 8, y + i + 8, 1, 1, pal_offs );
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
ARCHANDLE *POKE2DGRA_OpenHandle( HEAPID heapID )
{	
	return GFL_ARC_OpenDataHandle( POKEGRA_GetArcID(), heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ポケモンのパレット読み込み
 *
 *	@param	ARCHANDLE *p_handle			ハンドル
 *	@param	POKEMON_PASO_PARAM* ppp	ポケモンパーソナルパラメータ
 *	@param	dir				ポケモンの方向
 *	@param	vramType	読み込みタイプ
 *	@param	byteOffs	パレット読み込みオフセット
 *	@param	heapID		ヒープID 
 *
 *	@return	登録ID
 */
//-----------------------------------------------------------------------------
u32 POKE2DGRA_OBJ_PLTT_Register( ARCHANDLE *p_handle, const POKEMON_PASO_PARAM* ppp, int dir, CLSYS_DRAW_TYPE vramType, u16 byteOffs, HEAPID heapID )
{	
	u32 plt;
	//リソース受け取り
	{	
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );

		plt	= POKEGRA_GetPalArcIndex( mons_no, form_no, sex, rare, dir );
	}

	//読み込み
	return GFL_CLGRP_PLTT_RegisterEx( p_handle, plt, vramType, byteOffs, 0, POKEGRA_POKEMON_PLT_NUM, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ポケモンキャラクター読み込み
 *
 *	@param	ARCHANDLE *p_handle			ハンドル
 *	@param	POKEMON_PASO_PARAM* ppp	ポケモンパーソナルパラメータ
 *	@param	dir											ポケモンの方向
 *	@param	vramType								読み込みタイプ
 *	@param	heapID									ヒープID
 *
 *	@return	登録番号
 */
//-----------------------------------------------------------------------------
u32 POKE2DGRA_OBJ_CGR_Register( ARCHANDLE *p_handle, const POKEMON_PASO_PARAM* ppp, int dir, CLSYS_DRAW_TYPE vramType, HEAPID heapID )
{	
	u32 cgr;
	//リソース受け取り
	{	
		int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
		int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
		int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
		int	rare	= PPP_CheckRare( ppp );

		cgr	= POKEGRA_GetCgrArcIndex( mons_no, form_no, sex, rare, dir );
	}

	//読み込み
	return GFL_CLGRP_CGR_Register( p_handle, cgr, FALSE, vramType, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ポケモンセル読み込み
 *
 *	@param	const POKEMON_PASO_PARAM* ppp	ポケモンパーソナルパラメータ
 *	@param	dir														ポケモンの方向
 *	@param	mapping												マッピングモード
 *	@param	vramType											読み込みタイプ
 *	@param	heapID												ヒープID
 *
 *	@return
 */
//-----------------------------------------------------------------------------
extern u32 POKE2DGRA_OBJ_CELLANM_Register( const POKEMON_PASO_PARAM* ppp, int dir, APP_COMMON_MAPPING mapping, CLSYS_DRAW_TYPE vramType, HEAPID heapID )
{	
	ARCHANDLE *p_handle;
	u32 cel, anm;
	u32 ret;

	{	
		cel	= APP_COMMON_GetDummyCellArcIdx( mapping );
		anm	= APP_COMMON_GetDummyAnimeArcIdx( mapping );
	}

	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );
	ret	= GFL_CLGRP_CELLANIM_Register( p_handle, cel, anm, heapID );
	GFL_ARC_CloseDataHandle( p_handle );

	return ret;
}
