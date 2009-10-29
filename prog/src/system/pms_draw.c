//=============================================================================
/**
 *
 *	@file		pms_draw.c
 *	@brief  簡易会話表示システム（簡易会話+デコメ管理）
 *	@author	hosaka genya
 *	@data		2009.10.14
 *
 */
//=============================================================================
#include <gflib.h>
#include "print/printsys.h"

#include "system/pms_draw.h"

//アーカイブ
#include "arc_def.h"
#include "pmsi.naix"

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================

//--------------------------------------------------------------
///	要素ごとの管理データ
//==============================================================
typedef struct {
  BOOL          b_useflag;
  PRINT_UTIL    print_util;
  GFL_CLWK*     clwk[PMS_WORD_MAX];
  BOOL          b_clwk_deco[PMS_WORD_MAX]; ///< CLWKデコメ判定フラグ
} PMS_DRAW_UNIT;

//--------------------------------------------------------------
///	OBJリソース管理ワーク
//==============================================================
typedef struct {
  GFL_CLUNIT* clwk_unit;
  CLSYS_DRAW_TYPE vram_type;
  u32  obj_ncl;
  u32  obj_ncg;
  u32  obj_nce;
} PMS_DRAW_OBJ;

//--------------------------------------------------------------
///	メインワーク
//==============================================================
struct _PMS_DRAW_WORK {
  // [IN]
  HEAPID      heap_id;
  GFL_FONT*   font;
  PRINT_QUE*  print_que;
  // [PRIVATE]
  PMS_DRAW_UNIT*  unit;
  PMS_DRAW_OBJ wk_obj;
  BOOL    b_print_end;  ///< プリント終了フラグ
  u8      unit_num;
  u8      padding[3];
};

//=============================================================================
/**
 *							プロトタイプ宣言
 */
//=============================================================================
static u32 _obj_get_ncer( CLSYS_DRAW_TYPE vram_type );
static u32 _obj_get_nanr( CLSYS_DRAW_TYPE vram_type );
static void _obj_loadres( PMS_DRAW_OBJ* obj, u8 pltt_ofs, HEAPID heap_id );
static void _obj_unloadres( PMS_DRAW_OBJ* obj );
static GFL_CLWK* _obj_create( PMS_DRAW_OBJ* obj, HEAPID heap_id );
static void _obj_set_deco( GFL_CLWK* act, GFL_BMPWIN* win, u8 width, u8 line, PMS_DECO_ID deco_id, GFL_POINT* offset );
static void _unit_init( PMS_DRAW_UNIT* unit, PMS_DRAW_OBJ* obj, HEAPID heap_id );
static void _unit_exit( PMS_DRAW_UNIT* unit );
static BOOL _unit_main( PMS_DRAW_UNIT* unit, PRINT_QUE* que );
static void _unit_print( PMS_DRAW_UNIT* unit, PRINT_QUE* print_que, GFL_FONT* font, GFL_BMPWIN* win, PMS_DATA* pms, GFL_POINT* offset, HEAPID heap_id );
static void _unit_clear( PMS_DRAW_UNIT* unit );
static CLSYS_DRAW_TYPE BGFrameToVramType( u8 frame );

//=============================================================================
/**
 *								外部公開関数
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  簡易会話表示システム 初期化
 *
 *	@param	GFL_CLUNIT* clunit  アクターユニット
 *	@param  CLSYS_DRAW_TYPE vram_type 上下画面フラグ
 *	@param	que   プリントキュー
 *	@param  font  フォント
 *	@param	obj_pltt_ofs パレットオフセット
 *	@param	id_max  管理する表示ユニット数
 *	@param	heap_id ヒープID
 *
 *	@retval ワーク
 */
//-----------------------------------------------------------------------------
PMS_DRAW_WORK* PMS_DRAW_Init( GFL_CLUNIT* clunit, CLSYS_DRAW_TYPE vram_type, PRINT_QUE* que, GFL_FONT* font, u8 obj_pltt_ofs, u8 id_max, HEAPID heap_id )
{
  int i;
  PMS_DRAW_WORK* wk;

  // 両画面はサポート対象外
  GF_ASSERT( vram_type != CLSYS_DRAW_MAX );

  // ヒープ生成
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(PMS_DRAW_WORK) );
  GFL_STD_MemClear( wk, sizeof(PMS_DRAW_WORK) );

  wk->heap_id     = heap_id;
  wk->print_que   = que;
  wk->font        = font;
  wk->unit_num    = id_max;

  // ユニット初期化
  wk->unit = GFL_HEAP_AllocClearMemory( heap_id, sizeof(PMS_DRAW_UNIT) *id_max );
  GFL_STD_MemClear( wk->unit, sizeof(PMS_DRAW_UNIT) * id_max );

  // OBJリソース管理ワーク初期化
  wk->wk_obj.clwk_unit = clunit;
  wk->wk_obj.vram_type = vram_type;

  // OBJリソースロード
  _obj_loadres( &wk->wk_obj, obj_pltt_ofs, wk->heap_id );

  // 表示ユニット初期化
  for( i=0; i<wk->unit_num; i++ )
  {
    _unit_init( &wk->unit[i], &wk->wk_obj, wk->heap_id );
  }

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  簡易会話表示システム 主処理
 *
 *	@param	PMS_DRAW_WORK* wk ワーク
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Main( PMS_DRAW_WORK* wk )
{
  int i;

  // 何こともなければ転送終了
  wk->b_print_end = TRUE;
  
  for( i=0; i<wk->unit_num; i++ )
  {
    if( _unit_main( &wk->unit[i], wk->print_que ) )
    {
      // 一個でも転送が終わってないものがあったら転送終了フラグをOFF
      wk->b_print_end = FALSE;
    }
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  簡易会話表示システム 開放処理
 *
 *	@param	PMS_DRAW_WORK* wk ワーク
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Exit( PMS_DRAW_WORK* wk )
{
  int i;

  // 表示ユニットワーク終了処理
  for( i=0; i<wk->unit_num; i++ )
  {
    _unit_exit( &wk->unit[i] );
  }
    
  // 表示ユニットワークを一括フリー
  GFL_HEAP_FreeMemory( wk->unit );

  _obj_unloadres( &wk->wk_obj );

  // ヒープ開放
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  指定IDに簡易会話を表示
 *
 *	@param	PMS_DRAW_WORK* wk ワーク
 *	@param	win 表示するBMPWIN
 *	@param	pms 表示する簡易会話データ
 *	@param	id 表示ユニット管理ID
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Print( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, PMS_DATA* pms, u8 id )
{ 
  GFL_POINT offset = {0};
  PMS_DRAW_PrintOffset( wk, win, pms, id, &offset );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  指定IDに簡易会話を表示(表示オフセット指定版)
 *
 *	@param	PMS_DRAW_WORK* wk ワーク
 *	@param	win 表示するBMPWIN
 *	@param	pms 表示する簡易会話データ
 *	@param	id 表示ユニット管理ID
 *	@param  offset 表示オフセット（ドット単位）
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_PrintOffset( PMS_DRAW_WORK* wk, GFL_BMPWIN* win, PMS_DATA* pms, u8 id, GFL_POINT* offset )
{ 
  PMS_DRAW_UNIT* unit;

  GF_ASSERT( wk && win && pms );
  GF_ASSERT( id < wk->unit_num );

  HOSAKA_Printf("print id=%d \n", id);

  _unit_print( &wk->unit[id], wk->print_que, wk->font, win, pms, offset, wk->heap_id );
    
  // Mainを通るまでは転送されない
  wk->b_print_end = FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  指定IDの表示終了チェック
 *
 *	@param	PMS_DRAW_WORK* wk ワーク
 *
 *	@retval	TRUE:プリント終了
 */
//-----------------------------------------------------------------------------
BOOL PMS_DRAW_IsPrintEnd( PMS_DRAW_WORK* wk )
{
  return wk->b_print_end;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  指定IDの表示クリア
 *
 *	@param	PMS_DRAW_WORK* wk ワーク
 *	@param	id 表示ユニット管理ID
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Clear( PMS_DRAW_WORK* wk, u8 id )
{ 
  PMS_DRAW_UNIT* unit;

  GF_ASSERT( wk );
  GF_ASSERT( id < wk->unit_num );
  
  _unit_clear( &wk->unit[id] );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  指定表示ユニットが表示中かどうかを返す
 *
 *	@param	PMS_DRAW_WORK* wk ワーク
 *	@param	id 表示ユニット管理ID
 *
 *	@retval TRUE:表示中 FALSE:クリアされている
 */
//-----------------------------------------------------------------------------
BOOL PMS_DRAW_IsPrinting( PMS_DRAW_WORK* wk, u8 id )
{
  GF_ASSERT( wk );
  GF_ASSERT( id < wk->unit_num );

  return wk->unit[id].b_useflag;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  表示ユニットを上書き(双方プリント済みのものに限る)
 *
 *	@param	PMS_DRAW_WORK* wk ワーク
 *	@param	id_src 入れ替え元
 *	@param	id_dst 入れ替え先
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMS_DRAW_Copy( PMS_DRAW_WORK* wk, u8 id_src, u8 id_dst )
{
  int i;
  PMS_DRAW_UNIT* src;
  PMS_DRAW_UNIT* dst;
  GFL_POINT pos_ofs;
  
  GF_ASSERT( wk );
  GF_ASSERT( id_src < wk->unit_num );
  GF_ASSERT( id_dst < wk->unit_num );

  src = &wk->unit[id_src];
  dst = &wk->unit[id_dst];

  // OBJ追随のため、BMPWINの移動量を測定しておく
  pos_ofs.x = GFL_BMPWIN_GetPosX( dst->print_util.win ) - GFL_BMPWIN_GetPosX( src->print_util.win );
  pos_ofs.y = GFL_BMPWIN_GetPosY( dst->print_util.win ) - GFL_BMPWIN_GetPosY( src->print_util.win );

  // BMPWINの表示を上書き転送
  GFL_BMP_Copy( GFL_BMPWIN_GetBmp(src->print_util.win), GFL_BMPWIN_GetBmp(dst->print_util.win) );
  GFL_BMPWIN_MakeTransWindow_VBlank( dst->print_util.win );

  // OBJ上書き
  for( i=0; i<PMS_WORD_MAX; i++ )
  {
    u8 srcf;
    CLSYS_DRAW_TYPE draw_type;
    GFL_CLACTPOS pos;

    srcf = GFL_BMPWIN_GetFrame( src->print_util.win );
    draw_type = BGFrameToVramType( srcf );

    // 元のOBJ座標を取得し、移動量を足しこむ
    GFL_CLACT_WK_GetPos( src->clwk[i], &pos, draw_type );
//  HOSAKA_Printf("newpos=%d %d diff=%d %d \n",pos.x, pos.y, pos_ofs.x, pos_ofs.y);
    pos.x += pos_ofs.x * 8;
    pos.y += pos_ofs.y * 8;
  
    // 座標
    GFL_CLACT_WK_SetPos( dst->clwk[i], &pos, draw_type );
    // アニメ
    GFL_CLACT_WK_SetAnmSeq( dst->clwk[i], GFL_CLACT_WK_GetAnmSeq( src->clwk[i] ) );
    // 表示フラグ
    GFL_CLACT_WK_SetDrawEnable( dst->clwk[i], src->b_clwk_deco[i] );
  }

  // メンバコピー
  // @TODO CLACTとBMPWINを保持しておいて構造体を丸ごとコピーするほうが保守性が高い。
  dst->print_util.transReq = src->print_util.transReq;
  dst->b_useflag = src->b_useflag;
  for( i=0; i<PMS_WORD_MAX; i++ )
  {
    dst->b_clwk_deco[i] = src->b_clwk_deco[i];
  }

}

//=============================================================================
/**
 *								static関数
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  VRAMモードからセルの取得リソースを判定
 *
 *	@param	CLSYS_DRAW_TYPE vram_type 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u32 _obj_get_ncer( CLSYS_DRAW_TYPE vram_type )
{
	GXOBJVRamModeChar vrammode;
  
  if( vram_type == CLSYS_DRAW_MAIN )
  {
    vrammode = GX_GetOBJVRamModeChar();
  }
  else
  {
    vrammode = GXS_GetOBJVRamModeChar();
  }

  switch(vrammode){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		return NARC_pmsi_pms2_obj_dekome_32k_NCER;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		return NARC_pmsi_pms2_obj_dekome_64k_NCER;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
//		return NARC_pmsi_pms2_obj_dekome_128k_NCER;
	default:
		GF_ASSERT(0);	//非対応のマッピングモード
	}
  
  return NARC_pmsi_pms2_obj_dekome_128k_NCER;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  VRAMモードからセルアニメの取得リソースを判定
 *
 *	@param	CLSYS_DRAW_TYPE vram_type 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u32 _obj_get_nanr( CLSYS_DRAW_TYPE vram_type )
{
	GXOBJVRamModeChar vrammode;
  
  if( vram_type == CLSYS_DRAW_MAIN )
  {
    vrammode = GX_GetOBJVRamModeChar();
  }
  else
  {
    vrammode = GXS_GetOBJVRamModeChar();
  }
	
  switch(vrammode){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		return NARC_pmsi_pms2_obj_dekome_32k_NANR;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		return NARC_pmsi_pms2_obj_dekome_64k_NANR;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
//		return NARC_pmsi_pms2_obj_dekome_128k_NANR;
	default:
		GF_ASSERT(0);	//非対応のマッピングモード
	}
  
  return NARC_pmsi_pms2_obj_dekome_128k_NANR;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  OBJ リソース初期化
 *
 *	@param	PMS_DRAW_OBJ* obj 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _obj_loadres( PMS_DRAW_OBJ* obj, u8 pltt_ofs, HEAPID heap_id )
{
  ARCHANDLE* handle;
  u32 res_ncer = _obj_get_ncer( obj->vram_type );
  u32 res_nanr = _obj_get_nanr( obj->vram_type );

  // ハンドルオープン
  handle	= GFL_ARC_OpenDataHandle( ARCID_PMSI_GRAPHIC, heap_id );

	//リソース読みこみ
	obj->obj_ncl	= GFL_CLGRP_PLTT_RegisterEx( handle, NARC_pmsi_pms2_obj_dekome_NCLR, obj->vram_type, 0x20*pltt_ofs, 0, PMS_DRAW_OBJ_PLTT_NUM, heap_id );
  obj->obj_ncg = GFL_CLGRP_CGR_Register( handle, NARC_pmsi_pms2_obj_dekome_NCGR, FALSE, obj->vram_type, heap_id );
  obj->obj_nce = GFL_CLGRP_CELLANIM_Register( handle, res_ncer, res_nanr, heap_id );

  GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  OBJ リソース開放
 *
 *	@param	PMS_DRAW_OBJ* obj 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _obj_unloadres( PMS_DRAW_OBJ* obj )
{
	GFL_CLGRP_PLTT_Release( obj->obj_ncl );
	GFL_CLGRP_CGR_Release( obj->obj_ncg );
	GFL_CLGRP_CELLANIM_Release( obj->obj_nce );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  アクター生成
 *
 *	@param	PMS_DRAW_OBJ* obj 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* _obj_create( PMS_DRAW_OBJ* obj, HEAPID heap_id )
{
  GFL_CLWK* act;
  GFL_CLWK_DATA cldata = {0};
  
//  act = GFL_CLACT_WK_CreateAffine( obj->clwk_unit, 
  act = GFL_CLACT_WK_Create( obj->clwk_unit, 
      obj->obj_ncg, 
      obj->obj_ncl,
      obj->obj_nce,
      &cldata,
      obj->vram_type,
      heap_id
      );

  GFL_CLACT_WK_SetDrawEnable( act, FALSE );
  GFL_CLACT_WK_SetAutoAnmFlag( act, TRUE );
  
  // 30FPSで動作していた場合はセルアニメ倍速
  if( GFL_UI_GetFrameRate() == GFL_UI_FRAMERATE_30 )
  {
    GFL_CLACT_WK_SetAutoAnmSpeed( act, FX32_ONE * 2 );
  }

  return act;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  デコメアクター表示
 *
 *	@param	GFL_CLWK* act
 *	@param	win
 *	@param	deco_id 
 *	@param	offset 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _obj_set_deco( GFL_CLWK* act, GFL_BMPWIN* win, u8 width, u8 line, PMS_DECO_ID deco_id, GFL_POINT* offset )
{
  u8 frame;
  GFL_CLACTPOS pos;

  GF_ASSERT( deco_id > PMS_DECOID_NULL && deco_id < PMS_DECOID_MAX );
  
  frame = GFL_BMPWIN_GetFrame( win );
  pos.x = GFL_BMPWIN_GetPosX( win ) * 8 + width + offset->x;
  pos.y = GFL_BMPWIN_GetPosY( win ) * 8 + line * 16 + offset->y;

  HOSAKA_Printf("pos=%d,%d line=%d width=%d \n",pos.x, pos.y, line, width);

  GFL_CLACT_WK_SetPos( act, &pos, BGFrameToVramType( frame ) );
  GFL_CLACT_WK_SetBgPri( act, GFL_BG_GetPriority( frame ) );
  GFL_CLACT_WK_SetAnmSeq( act, deco_id-1 ); // アニメは0オリジン
}

//-----------------------------------------------------------------------------
/**
 *	@brief  表示ユニット 初期化
 *
 *	@param	PMS_DRAW_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _unit_init( PMS_DRAW_UNIT* unit, PMS_DRAW_OBJ* obj, HEAPID heap_id )
{
  int i;

  for( i=0; i<PMS_WORD_MAX; i++ )
  {
    // アクター生成
    unit->clwk[i] = _obj_create( obj, heap_id );
  }
  
  unit->b_useflag = FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  表示ユニット 削除
 *
 *	@param	PMS_DRAW_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _unit_exit( PMS_DRAW_UNIT* unit )
{
  int i;

  for( i=0; i<PMS_WORD_MAX; i++ )
  {
    // アクター解放
    GFL_CLACT_WK_Remove( unit->clwk[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  表示ユニット 主処理
 *
 *	@param	PMS_DRAW_UNIT* unit
 *	@param	que 
 *
 *  @retval BOOL	まだ転送が終わっていない場合はTRUE／それ以外FALSE
 */
//-----------------------------------------------------------------------------
static BOOL _unit_main( PMS_DRAW_UNIT* unit, PRINT_QUE* que )
{
  BOOL doing;

  doing = !PRINT_UTIL_Trans( &unit->print_util, que );

  // 表示が終了していればデコメアクター表示
  if( doing == FALSE )
  {
    int i;
    for( i=0; i<PMS_WORD_MAX; i++ )
    {
      if( unit->b_clwk_deco[i] )
      {
        GFL_CLACT_WK_SetDrawEnable( unit->clwk[i], TRUE );
      }
    }
  }

  return doing;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  表示ユニット プリント
 *
 *	@param	PMS_DRAW_UNIT* unit
 *	@param	print_que
 *	@param	font
 *	@param	win
 *	@param	pms
 *	@param  offset
 *	@param	heap_id 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _unit_print( PMS_DRAW_UNIT* unit, PRINT_QUE* print_que, GFL_FONT* font, GFL_BMPWIN* win, PMS_DATA* pms, GFL_POINT* offset, HEAPID heap_id )
{
  STRBUF* buf;

  GF_ASSERT_MSG( unit->b_useflag == FALSE, "PMS_DRAW_Print 二重呼出!" );

  // プリント設定初期化
  PRINT_UTIL_Setup( &unit->print_util, win );

  // バッファ展開前の文字列
  buf = PMSDAT_GetSourceString( pms, heap_id );

  {
    int i;

    for( i=0; i<PMS_WORD_MAX; i++ )
    {
      // デコメ判定
      if( PMSDAT_IsDecoID( pms, i ) )
      {
        PMS_DECO_ID deco_id;
        u8 width, line;
      
        deco_id = PMSDAT_GetWordNumber( pms, i );

        HOSAKA_Printf("[%d]deco_id=%d\n",i, deco_id);
        
        width = PRINTSYS_GetTagWidth( buf, i, font, 0 );
        line  = PRINTSYS_GetTagLine( buf, i );

        // デコメアクター設定
        _obj_set_deco( unit->clwk[i], win, width, line, deco_id, offset );

        unit->b_clwk_deco[i] = TRUE;
      }
    }
  }
  
  // @TODO 一度開放しているのであまり効率が良くない。
  // 負荷がかかるようであれば、PMSDAT_ToStringの中でしていることをこちらに持ってくる。
  // もしくはPMSDATの方で後からレジストする関数を作る。
  GFL_STR_DeleteBuffer( buf );
  
  // プリントリクエスト
  buf = PMSDAT_ToString( pms, heap_id );
  PRINT_UTIL_Print( &unit->print_util, print_que, offset->x, offset->y, buf, font );
  GFL_STR_DeleteBuffer( buf );
      
  // 転送
	GFL_BMPWIN_MakeScreen( unit->print_util.win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(unit->print_util.win) );

  unit->b_useflag = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  表示ユニット クリア
 *
 *	@param	PMS_DRAW_UNIT* unit 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _unit_clear( PMS_DRAW_UNIT* unit )
{
  GF_ASSERT( unit->b_useflag == TRUE );

  // キャラ・スクリーンをクリアして転送
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(unit->print_util.win), 0 );
	GFL_BMPWIN_TransVramCharacter( unit->print_util.win );
	GFL_BMPWIN_ClearTransWindow( unit->print_util.win );

  // 画像非表示
  {
    int i;
    for( i=0; i<PMS_WORD_MAX; i++ )
    {
      GFL_CLACT_WK_SetDrawEnable( unit->clwk[i], FALSE );
      unit->b_clwk_deco[i] = FALSE;
    }
  }

  unit->b_useflag = FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BGフレームからOBJ用VRAMTYPEを取得
 *
 *	@param	u8 frame 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static CLSYS_DRAW_TYPE BGFrameToVramType( u8 frame )
{
  GF_ASSERT( frame <= GFL_BG_FRAME3_S );

  if( frame >= GFL_BG_FRAME0_S )
  {
    return CLSYS_DRAW_SUB;
  }

  return CLSYS_DRAW_MAIN;
}



