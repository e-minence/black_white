//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dchar.c
 *	@brief		wifi２Ｄキャラクタ読み込みシステム
 *	@author		tomoya takahshi
 *	@data		2007.02.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "system/palanm.h"
#include "arc_def.h"
#include "wifileadingchar.naix"
#include "wifi_unionobj.naix"

#include "net_app/wifi2dmap/wifi_2dchar.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------

//-------------------------------------


#define WF_2DC_RESM_OBJ_NUM	(WF_2DC_CHARNUM+1)	// リソース数	+1は陰


// リソースタイプ
typedef enum {
  WF_2DC_RES_NML,	// 通常
  WF_2DC_RES_UNI,	// ユニオン
} WF_2DC_RESTYPE;

// アークハンドルタイプ
enum{
  WF_2DC_ARCHANDLE_NML,	// 通常
  WF_2DC_ARCHANDLE_UNI,	// ユニオン
  WF_2DC_ARCHANDLE_NUM,
};


//



#define WF_2DC_RESMAN_NUM	( 4 )	// 作成するリソースマネージュ数
#define WF_2DC_ANMRESMAN_OBJNUM	( (WF_2DC_MOVENUM*2)+WF_2DC_UNICHAR_NUM+1 )	// セルとアニメのリソースマネージャオブジェクト数	+1は陰
enum{
  WF_2DC_ANMRES_ANM_CELL,
  WF_2DC_ANMRES_ANM_ANM,
  WF_2DC_ANMRES_ANM_NOFLIP_CELL,
  WF_2DC_ANMRES_ANM_NOFLIP_ANM,
  WF_2DC_ANMRES_ANM_NUM,
};
static const u8 sc_WF_2DC_ANMRES_ANMCONTID[ WF_2DC_ANMRES_ANM_NUM ] = {
  0,0,
  1,1,
};

// 通常キャラクター
#define WF_2DC_ARC_CHARIDX	( NARC_wifileadingchar_hero_NCLR )	// キャラクタグラフィックの開始
#define WF_2DC_ARC_GETNCL(x)	(((x)*2)+WF_2DC_ARC_CHARIDX)	// パレット取得
#define WF_2DC_ARC_GETNCG(x)	(WF_2DC_ARC_GETNCL(x) + 1)		// キャラクタ取得
#define WF_2DC_ARC_ANMIDX	( NARC_wifileadingchar_hero_NCER )	// ユニオンアニメデータ
#define WF_2DC_ARC_GETCEL(x)	(WF_2DC_ARC_ANMIDX+((x)*2))	// セルdataidゲット
#define WF_2DC_ARC_GETANM(x)	(WF_2DC_ARC_GETCEL(x)+1)		// アニメdataidゲット

#define WF_2DC_HERO_PLTT_START   ( 8 ) // HEROキャラクタが使用するパレットスタート位置
#define WF_2DC_HERO_PLTT_NUM		( 2 )	// HEROキャラクタが使用するパレット本数

#define WF_2DC_HERO_KAGE_PLTT_START (10)
#define WF_2DC_HERO_KAGE_PLTT_NUM (1)


// ユニオンキャラクター
#define WF_2DC_ARC_UNICHARIDX	( NARC_wifi_unionobj_normal00_NCGR )	// キャラクタグラフィックの開始 boy1 から normal00に置き換えました nagihashi
#define WF_2DC_ARC_GETUNINCL	(NARC_wifi_unionobj_wifi_union_obj_NCLR)	// パレット取得
#define WF_2DC_ARC_GETUNINCG(x)	(WF_2DC_ARC_UNICHARIDX + x)		// キャラクタ取得
#define WF_2DC_UNIPLTT_NUM		( 8 )	// ユニオンキャラクタが使用するパレット本数
#define WF_2DC_ARC_UNIANMIDX	( NARC_wifi_unionobj_normal00_NCER )	// ユニオンアニメデータ
#define WF_2DC_ARC_GETUNICEL(x)	(WF_2DC_ARC_UNIANMIDX+((x)*2))	// セルdataidゲット
#define WF_2DC_ARC_GETUNIANM(x)	(WF_2DC_ARC_GETUNICEL(x)+1)		// アニメdataidゲット


// リソース管理ID
#define WF_2DC_ARC_CONTANMID	( 0x100 )	// アニメリソース管理ID
#define WF_2DC_ARC_CONTSHADOWID	( 0x120 )	// 陰リソース管理ID
#define WF_2DC_ARC_CONTCHARID	( 0x200 )	// キャラクタリソース管理ID
#define WF_2DC_ARC_CONTUNIONPLTTID	( 0x402 )	// ユニオンパレットリソース管理ID
// ユニオンで出てくるキャラクタの管理IDの場所なので大丈夫
#define WF_2DC_ARC_CONTUNIONANMID	(0x180)	// ユニオンアニメリソース管理ID

#define WF_2DC_ARC_TURNANMCHARSIZ			(0x800)	// 振り向きのみ時のキャラクタサイズ
#define WF_2DC_ARC_TURNANMCHARSIZ_NOFLIP	(0x800)	// 振り向きのみ時のキャラクタサイズ
#define WF_2DC_ARC_NORMALANMCHARSIZ			(0x1400)// 歩き＋振り向きのみ時のキャラクタサイズ
#define WF_2DC_ARC_NORMALANMCHARSIZ_NOTFLIP	(0x1800)// 歩き＋振り向きのみ時のキャラクタサイズ	フリップなしのキャラのとき


// 各アニメのフレーム数
#define WF_2DC_ANM_WAY_FRAME	(1)		// 向き変えアニメ
#define WF_2DC_ANM_WALK_FRAME	(WF_COMMON_WALK_FRAME)		// 歩きアニメ
#define WF_2DC_ANM_TURN_FRAME	(WF_COMMON_TURN_FRAME)		// 振り向きアニメ
#define WF_2DC_ANM_RUN_FRAME	(WF_COMMON_RUN_FRAME)		// 走りアニメ
#define WF_2DC_ANM_WALLWALK_FRAME	(WF_COMMON_WALLWALK_FRAME)	// 壁歩きアニメ
#define WF_2DC_ANM_SLOWWALK_FRAME	(WF_COMMON_SLOWWALK_FRAME)	// 壁歩きアニメ
#define WF_2DC_ANM_HIGHWALK2_FRAME	(WF_COMMON_HIGHWALK2_FRAME)	// 高速歩きアニメ
#define WF_2DC_ANM_HIGHWALK4_FRAME	(WF_COMMON_HIGHWALK4_FRAME)	// 高速歩きアニメ
#define WF_2DC_ANM_ROTA_FRAME	(0xff)	// 回転アニメ

// アニメのシーケンス数
#define WF_2DC_ANM_ROTA			(4)
#define WF_2DC_ANM_WALK			(5)		// 歩きアニメのシーケンス数
#define WF_2DC_ANM_RUN			(9)		// 走りアニメのシーケンス数
#define WF_2DC_ANM_FRAME		(2*FX32_ONE)	// 16 fure
#define WF_2DC_ANM_SLOWFRAME (FX32_ONE)
#define WF_2DC_ANM_HIGH2 (8*FX32_ONE)			//
#define WF_2DC_ANM_HIGH4 (6*FX32_ONE)


// かげ
#define WF_2DC_SHADOW_MAT_OFS_X		( 8 )
#define WF_2DC_SHADOW_MAT_OFS_Y		( 14 )

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	キャラクタリソースデータ
//=====================================
typedef struct {
  WF_2DC_RESTYPE type;

  u32 resid[2];	//CG PLリソースオブジェ

  u32					drawtype;	// 転送先
  WF_2DC_MOVETYPE		movetype;	// 動作タイプ
} WF_2DCRES;


//-------------------------------------
///	アニメリソースデータ
//=====================================
typedef struct {
  void* resobj[WF_2DC_ANMRES_ANM_NUM];	// CE ANリソースオブジェ
  u32 resid;
} WF_2DCANMRES;


//-------------------------------------
/// 2Dキャラクタワーク
//=====================================
struct _WF_2DCWK {
  const WF_2DCRES* cp_res;	// リソースデータ
  GFL_CLWK*	p_clwk;		// 人物
  GFL_CLWK*	p_shadow;	// 陰
  WF_2DC_ANMTYPE	patanmtype;
  WF_COMMON_WAY	patanmway;
  s16				patanmframe;
  s16				pad_0;
};

//-------------------------------------
///	陰リソース
//=====================================
typedef struct {
  u32 resid[4];	//影リソース
} WF_2DCSH_RES;


//-------------------------------------
///	2Dキャラクタ管理システム
//=====================================
struct _WF_2DCSYS {
  GFL_CLUNIT* p_unit;
  PALETTE_FADE_PTR p_pfd;
  WF_2DCWK*		p_wk;
  u32 objnum;
  WF_2DCRES		chrres[ WF_2DC_CHARNUM ];
  WF_2DCANMRES	anmres[ WF_2DC_CHARNUM ];
  WF_2DCSH_RES	shadowres;
  u32				shadow_pri;		// 陰のソフト優先順位開始位置
  u32 unionplttid;
  ARCHANDLE*	p_handle[WF_2DC_ARCHANDLE_NUM];
  u32 CELLANIMIndex[WF_2DC_MOVENUM][WF_2DC_ANMRES_ANM_NUM];
  u32 hero_no;
} ;


//-------------------------------------
///	グラフィックファイルとキャラクタコードをリンク
//するデータの構造体
//=====================================
typedef struct {
  u16 code;
  u16 wf2dc_char;
} WF_2DC_CHARDATA;


//-----------------------------------------------------------------------------
/**
 *					データ部
 */
//-----------------------------------------------------------------------------


/// 各アニメの終了フレーム数
static const u8 WF_2DC_AnmFrame[WF_2DC_ANMNUM] = {
  WF_2DC_ANM_WAY_FRAME,
  WF_2DC_ANM_ROTA_FRAME,
  WF_2DC_ANM_WALK_FRAME,
  WF_2DC_ANM_TURN_FRAME,
  WF_2DC_ANM_RUN_FRAME,
  WF_2DC_ANM_WALLWALK_FRAME,
  WF_2DC_ANM_SLOWWALK_FRAME,
  WF_2DC_ANM_HIGHWALK2_FRAME,
  WF_2DC_ANM_HIGHWALK4_FRAME,
  0,
};

//----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
 */
//-----------------------------------------------------------------------------

//-------------------------------------
/// アニメリソース関連
//=====================================
static void WF_2DC_AnmResLoad( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype, u32 heap );
static void WF_2DC_AnmResDel( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype );

static u32 WF_2DC_CharNoGet( u32 view_type );
static BOOL WF_2DC_AnmModeLinkCheck( u32 view_type, WF_2DC_MOVETYPE movetype );
static u32 WF_2DC_AnmResContIdGet( WF_2DC_MOVETYPE movetype, u32 res_type, BOOL flip );

//-------------------------------------
/// キャラ・パレットリソース関連
//=====================================
static void WF_2DC_CharResLoad( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap, WF_2DC_RESTYPE restype );
static void WF_2DC_CharResLoadNml( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
static void WF_2DC_CharResLoadUni( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
static void WF_2DC_CharResDel( WF_2DCSYS* p_sys, u32 char_no );
static void WF_2DC_CharResDelNml( WF_2DCSYS* p_sys, u32 char_no );
static void WF_2DC_CharResDelUni( WF_2DCSYS* p_sys, u32 char_no );
static BOOL WF_2DC_CharResCheck( const WF_2DCSYS* cp_sys, u32 char_no );
static u32 WF_2DC_CharResDrawTypeGet( const WF_2DCRES* cp_res );
static WF_2DC_MOVETYPE WF_2DC_CharResMoveTypeGet( const WF_2DCRES* cp_res );


//-------------------------------------
///	陰リソース関連
//=====================================
static void WF_2DC_ShResLoad( WF_2DCSYS* p_sys, u32 draw_type, u32 heap );
static void WF_2DC_ShResDel( WF_2DCSYS* p_sys );
static BOOL WF_2DC_ShResCheck( const WF_2DCSH_RES* cp_shadowres );



//-------------------------------------
/// ユニオンリソース関連
//=====================================
static void WF_2DC_UniCharPlttResLoad( WF_2DCSYS* p_sys, u32 draw_type,int palstartpos, u32 heap );
static void WF_2DC_UniCharPlttResDel( WF_2DCSYS* p_sys );
static void WF_2DC_UniCharAnmResLoad( WF_2DCSYS* p_sys, u32 heap );
static void WF_2DC_UniCharAnmResDel( WF_2DCSYS* p_sys );

//-------------------------------------
/// オブジェワーク操作関連
//=====================================
static WF_2DCWK* WF_2DC_WkCleanGet( WF_2DCSYS* p_sys );
static BOOL WF_2DC_WkCleanCheck( const WF_2DCWK* cp_wk );
static void WF_2DC_WkPatAnmUpdate( WF_2DCWK* p_wk );


//-------------------------------------
/// アニメ管理関連
//=====================================
static void WF_2DC_WkPatAnmWayInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmRotaInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmTurnInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmRunInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmWallWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmSlowWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmHighWalk2Init( WF_2DCWK* p_wk, WF_COMMON_WAY way );
static void WF_2DC_WkPatAnmHighWalk4Init( WF_2DCWK* p_wk, WF_COMMON_WAY way );

static void WF_2DC_WkPatAnmRotaMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmWalkMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmTurnMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmRunMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmWallWalkMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmSlowWalkMain( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmHighWalk2Main( WF_2DCWK* p_wk );
static void WF_2DC_WkPatAnmHighWalk4Main( WF_2DCWK* p_wk );

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI　2D描画システム作成
 *
 *	@param	p_clset		使用するアクターセット
 *	@param	pfd			パレットフェードポインタ
 *	@param	objnum		オブジェクト数
 *	@param	heap		ヒープ
 *
 *	@return	作成したシステムポインタ
 */
//-----------------------------------------------------------------------------
WF_2DCSYS* WF_2DC_SysInit( GFL_CLUNIT* p_unit, PALETTE_FADE_PTR p_pfd, u32 objnum, u32 heap )
{
  WF_2DCSYS* p_sys;
  int i;

  // メモリ確保
  p_sys = GFL_HEAP_AllocClearMemory( heap, sizeof(WF_2DCSYS) );

  // アクターセットは上からもらう
  p_sys->p_unit = p_unit;

  // パレットフェードシステム
  p_sys->p_pfd = p_pfd;

  // アーカイブハンドルオープン
  p_sys->p_handle[WF_2DC_ARCHANDLE_NML] = GFL_ARC_OpenDataHandle( ARCID_WIFILEADING, heap );
  p_sys->p_handle[WF_2DC_ARCHANDLE_UNI] = GFL_ARC_OpenDataHandle( ARCID_WIFIUNIONCHAR, heap );


  //    NNS_G2dInitImagePaletteProxy( p_sys->pltProxy[screen] );
  //  GFL_ARC_UTIL_TransVramPaletteMakeProxy( arcfile , arcpal ,
  //                                        hwscreen , 0 , wk->heapid , &wk->clact.pltProxy[screen] );


  // リソースマネージャ初期化
  //	for( i=0; i<2; i++ ){	// CG、PLは、キャラクタ分
  //		p_sys->p_res_man[i] = CLACT_U_ResManagerInit( WF_2DC_RESM_OBJ_NUM, i, heap );
  //	}
  //	for( i=0; i<2; i++ ){	// CE,ANはアニメタイプ分
  //		p_sys->p_res_man[i+2] = CLACT_U_ResManagerInit( WF_2DC_ANMRESMAN_OBJNUM, i+2, heap );
  //	}

  //リソースIDの初期化
  for( i=0; i<WF_2DC_CHARNUM; i++ ){
    p_sys->chrres[i].resid[0] = GFL_CLGRP_REGISTER_FAILED;
  }
  p_sys->shadowres.resid[0] = GFL_CLGRP_REGISTER_FAILED;
  p_sys->unionplttid = GFL_CLGRP_REGISTER_FAILED;

  // アニメーションリソース読み込み
  //	for( i=0; i<WF_2DC_MOVENUM; i++ ){
  WF_2DC_AnmResLoad( p_sys, 0, heap );
  //	}

  // アクターワーク作成
  p_sys->objnum = objnum;
  p_sys->p_wk = GFL_HEAP_AllocClearMemory( heap, sizeof(WF_2DCWK)*p_sys->objnum );

  return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI　2D描画システム破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_SysExit( WF_2DCSYS* p_sys )
{
  int i;

  // ワーク破棄
  for( i=0; i<p_sys->objnum; i++ ){
    if( WF_2DC_WkCleanCheck( &p_sys->p_wk[i] ) == FALSE ){
      WF_2DC_WkDel( &p_sys->p_wk[i] );
    }
  }

  // アニメーションリソース破棄
  //	for( i=0; i<WF_2DC_MOVENUM; i++ ){
  WF_2DC_AnmResDel( p_sys, 0 );
  //	}

  // キャラクタリソース破棄
  WF_2DC_AllResDel( p_sys );

  // アーカイブハンドルクローズ
  //	GFL_ARC_CloseDataHandle( p_sys->p_handle[WF_2DC_ARCHANDLE_NML] );
  //	GFL_ARC_CloseDataHandle( p_sys->p_handle[WF_2DC_ARCHANDLE_UNI] );

  GFL_ARC_CloseDataHandle( p_sys->p_handle[WF_2DC_ARCHANDLE_NML] );
  GFL_ARC_CloseDataHandle( p_sys->p_handle[WF_2DC_ARCHANDLE_UNI] );

  // リソースマネージャ破棄
  //	for( i=0; i<WF_2DC_RESMAN_NUM; i++ ){
  //		CLACT_U_ResManagerDelete( p_sys->p_res_man[i] );
  //	}

  // ワーク破棄
  GFL_HEAP_FreeMemory( p_sys->p_wk );
  GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	リソース登録
 *
 *	@param	p_sys			システムワーク
 *	@param	view_type		見た目（FIELDOBJCODE）
 *	@param	draw_type		転送先
 *	@param	movetype		動作タイプ
 *	@param	heap			ヒープ
 *
 *	draw_type
	mainに登録：NNS_G2D_VRAM_TYPE_2DMAIN
	subに登録 ：NNS_G2D_VRAM_TYPE_2DSUB
	両方に登録：NNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
void WF_2DC_ResSet( WF_2DCSYS* p_sys, u32 view_type, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
  u32 char_no;
  BOOL result;

  // 読み込み可能かチェック
  result = WF_2DC_AnmModeLinkCheck( view_type, movetype );
  GF_ASSERT( result );

  // 見た目と性別からキャラクター番号取得
  char_no = WF_2DC_CharNoGet( view_type );

  // 読み込み
  WF_2DC_CharResLoad( p_sys, char_no, draw_type, movetype, heap, WF_2DC_RES_NML );
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース破棄
 *
 *	@param	p_sys		システムワーク
 *	@param	view_type	見た目FIELDOBJCODE
 *
 */
//-----------------------------------------------------------------------------
void WF_2DC_ResDel( WF_2DCSYS* p_sys, u32 view_type )
{
  u32 char_no;

  char_no = WF_2DC_CharNoGet( view_type );

  // リソース破棄
  WF_2DC_CharResDel( p_sys, char_no );
}

//----------------------------------------------------------------------------
/**
 *	@brief	読み込んでいるかチェック
 *
 *	@param	cp_sys		システムワーク
 *	@param	view_type	見た目（FIELDOBJCODE）
 *
 *	@retval	TRUE	読み込んである
 *	@retval	FALSE	読み込んでない
 */
//-----------------------------------------------------------------------------
BOOL WF_2DC_ResCheck( const WF_2DCSYS* cp_sys, u32 view_type )
{
  u32 char_no;

  // 見た目と性別からキャラクター番号取得
  char_no = WF_2DC_CharNoGet( view_type );
  return WF_2DC_CharResCheck( cp_sys, char_no );
}

//----------------------------------------------------------------------------
/**
 *	@brief	全リソースの破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_AllResDel( WF_2DCSYS* p_sys )
{
  int i;

  // ユニオンリソースの破棄
  //	if( p_sys->p_unionpltt != NULL ){
  if( p_sys->unionplttid !=  GFL_CLGRP_REGISTER_FAILED){
    WF_2DC_UnionResDel( p_sys );
  }

  // その他のリソースはき
  for( i=0; i<WF_2DC_CHARNUM; i++ ){
    if( WF_2DC_CharResCheck( p_sys, i ) == TRUE ){
      // リソース破棄
      WF_2DC_CharResDel( p_sys, i );
    }
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタの登録
 *
 *	@param	p_sys			システムワーク
 *	@param	draw_type		転送先
 *	@param	movetype		動作タイプ
 *  @param  palstartpos   パレット開始位置
 *	@param	heap			ヒープ
 *
 *	draw_type
	mainに登録：NNS_G2D_VRAM_TYPE_2DMAIN
	subに登録 ：NNS_G2D_VRAM_TYPE_2DSUB
	両方に登録：NNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
void WF_2DC_UnionResSet( WF_2DCSYS* p_sys, u32 draw_type, WF_2DC_MOVETYPE movetype,int palstartpos, u32 heap )
{
  int i;

  // 動作可能かチェック
  // 主人公以外は、走り以外受け付けていない
  GF_ASSERT( movetype != WF_2DC_MOVERUN );


  // ユニオンアニメリソース読込み
  WF_2DC_UniCharPlttResLoad( p_sys, draw_type, palstartpos, heap );

  // アニメリソース登録
  WF_2DC_UniCharAnmResLoad( p_sys, heap );

  for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){
    // 読み込み
    WF_2DC_CharResLoad( p_sys, i, draw_type, movetype, heap, WF_2DC_RES_UNI );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタのリソースはき
 *
 *	@param	p_sys			システムワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_UnionResDel( WF_2DCSYS* p_sys )
{
  int i;

  // ユニオンパレット破棄
  WF_2DC_UniCharPlttResDel( p_sys );

  // アニメリソース破棄
  WF_2DC_UniCharAnmResDel( p_sys );

  for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){
    // キャラクタはき
    WF_2DC_CharResDel( p_sys, i );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	陰のリソースを読み込む
 *
 *	@param	p_sys			システムワーク
 *	@param	draw_type		転送先
 *	@param	shadow_pri		陰の表示優先順位
 *	@param	heap			ヒープID
 *
 *	draw_type
	mainに登録：NNS_G2D_VRAM_TYPE_2DMAIN
	subに登録 ：NNS_G2D_VRAM_TYPE_2DSUB
	両方に登録：NNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
void WF_2DC_ShadowResSet( WF_2DCSYS* p_sys, u32 draw_type, u32 shadow_pri, u32 heap )
{
  WF_2DC_ShResLoad( p_sys, draw_type, heap );
  p_sys->shadow_pri = shadow_pri;
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰のリソースを破棄する
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_ShadowResDel( WF_2DCSYS* p_sys )
{
  WF_2DC_ShResDel( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタアクター登録
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_data		登録データ
 *	@param	view_type	FIELDOBJCODE
 *	@param	movetype	動作タイプ
 *	@param	heap		ヒープ
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WF_2DCWK* WF_2DC_WkAdd( WF_2DCSYS* p_sys, const WF_2DC_WKDATA* cp_data, u32 view_type, u32 heap )
{
  WF_2DCWK* p_wk;
  //	GFL_CLWK_DATA add;
  GFL_CLWK_DATA	cellInitData;
  u32 char_no;

  // 空のワーク取得
  p_wk = WF_2DC_WkCleanGet( p_sys );

  // キャラクタナンバー取得
  char_no = WF_2DC_CharNoGet( view_type );

  // 読み込み済みチェック
  GF_ASSERT( WF_2DC_CharResCheck( p_sys, char_no ) == TRUE );


  cellInitData.pos_x = cp_data->x;
  cellInitData.pos_y = cp_data->y;
  cellInitData.anmseq = 0;
  cellInitData.softpri = cp_data->pri;
  cellInitData.bgpri = cp_data->bgpri;



  // 表示エリア
  if( (p_sys->chrres[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_MAX) ||
      (p_sys->chrres[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_2DMAIN) ){
    //		add.DrawArea = NNS_G2D_VRAM_TYPE_2DMAIN;
  }else{
    //		add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
  }

  // アクター登録
  p_wk->p_clwk = GFL_CLACT_WK_Create(p_sys->p_unit,
                                     p_sys->chrres[ char_no ].resid[ 0 ],
                                     p_sys->chrres[ char_no ].resid[ 1 ],
                                     p_sys->anmres[char_no].resid,
                                     &cellInitData, CLSYS_DEFREND_MAIN, heap );

  //	CLACT_BGPriorityChg( p_wk->p_clwk, cp_data->bgpri );
  GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, cp_data->bgpri );

  // 動作モードにより初期アニメを変更
  if( p_sys->chrres[ char_no ].movetype == WF_2DC_MOVENORMAL ){
    // した歩き
    GFL_CLACT_WK_SetAnmIndex(p_wk->p_clwk, WF_2DC_ANM_WALK+WF_COMMON_BOTTOM);
  }else{
    // 下向き
    GFL_CLACT_WK_SetAnmIndex( p_wk->p_clwk, WF_COMMON_BOTTOM );
  }

  // アニメデータ初期化
  p_wk->patanmtype = WF_2DC_ANMNONE;
  p_wk->patanmframe = 0;
  p_wk->patanmway = WF_COMMON_BOTTOM;

  // リソース保存
  p_wk->cp_res = &p_sys->chrres[ char_no ];

  // 陰ワークの作成
  if( WF_2DC_ShResCheck( &p_sys->shadowres ) ){


    cellInitData.pos_x = cp_data->x;
    cellInitData.pos_y = cp_data->y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = p_sys->shadow_pri;
    cellInitData.bgpri = 0;


    // 表示エリア
    if( (p_sys->chrres[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_MAX) ||
        (p_sys->chrres[ char_no ].drawtype == NNS_G2D_VRAM_TYPE_2DMAIN) ){
      //			add.DrawArea = NNS_G2D_VRAM_TYPE_2DMAIN;
    }else{
      //			add.DrawArea = NNS_G2D_VRAM_TYPE_2DSUB;
    }


    p_wk->p_shadow = GFL_CLACT_WK_Create(p_sys->p_unit,
                                         p_sys->shadowres.resid[0],
                                         p_sys->shadowres.resid[1],
                                         p_sys->shadowres.resid[2],
                                         &cellInitData,
                                         CLSYS_DEFREND_MAIN,heap );

    GFL_CLACT_WK_SetBgPri( p_wk->p_shadow, cp_data->bgpri );
  }else{
    p_wk->p_shadow = NULL;
  }

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アクター破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkDel( WF_2DCWK* p_wk )
{

  if( p_wk->p_shadow != NULL ){
    GFL_CLACT_WK_Remove( p_wk->p_shadow );
  }
  GFL_CLACT_WK_Remove( p_wk->p_clwk );
  GFL_STD_MemClear( p_wk, sizeof(WF_2DCWK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターワークポインタの取得
 *
 *	@param	p_wk	ワーク
 *
 *	@return	セルアクターワーク
 */
//-----------------------------------------------------------------------------
GFL_CLWK* WF_2DC_WkClWkGet( WF_2DCWK* p_wk )
{
  return p_wk->p_clwk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターワークポインタの取得
 *
 *	@param	p_wk	ワーク
 *
 *	@return	セルアクターワーク
 */
//-----------------------------------------------------------------------------
const GFL_CLWK* WF_2DC_WkConstClWkGet( const WF_2DCWK* cp_wk )
{
  return cp_wk->p_clwk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標の設定
 *
 *	@param	p_wk	ワーク
 *	@param	x		ｘ座標
 *	@param	y		ｙ座標
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y )
{
  GFL_CLACTPOS	mat;

  mat.x = x;
  mat.y = y;

  GFL_CLACT_WK_SetWldPos( p_wk->p_clwk, &mat );


  if( p_wk->p_shadow ){

    mat.x += WF_2DC_SHADOW_MAT_OFS_X;
    mat.y += WF_2DC_SHADOW_MAT_OFS_Y;
    GFL_CLACT_WK_SetWldPos( p_wk->p_shadow, &mat );

  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標取得
 *
 *	@param	p_wk	ワーク
 *	@param	x_y		フラグ
 *
 *	x_y
	WF_2DC_GET_X,
	WF_2DC_GET_Y
 *
 *
 *	@return	フラグの座標の値
 */
//-----------------------------------------------------------------------------
s16 WF_2DC_WkMatrixGet( WF_2DCWK* p_wk, WF_2DC_MAT x_y )
{
  GFL_CLACTPOS p_pos;

  GFL_CLACT_WK_GetWldPos(p_wk->p_clwk, &p_pos);

  if( x_y == WF_2DC_GET_X ){
    return p_pos.x;
  }
  else{
    return p_pos.y;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示優先順位を設定
 *
 *	@param	p_wk	ワーク
 *	@param	pri		表示優先順位
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkDrawPriSet( WF_2DCWK* p_wk, u16 pri )
{
  GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, pri );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示優先順位を取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	表示優先順位
 */
//-----------------------------------------------------------------------------
u16 WF_2DC_WkDrawPriGet( const WF_2DCWK* cp_wk )
{
  return GFL_CLACT_WK_GetSoftPri( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションを進める
 *
 *	@param	p_wk	ワーク
 *	@param	frame	フレーム
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkAnmAddFrame( WF_2DCWK* p_wk, fx32 frame )
{
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, frame );
}

//----------------------------------------------------------------------------
/**
 *	@brie	表示非表示設定
 *
 *	@param	p_wk	ワーク
 *	@param	flag	フラグ
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkDrawFlagSet( WF_2DCWK* p_wk, BOOL flag )
{
  GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, flag );
  if( p_wk->p_shadow ){
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_shadow, flag );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brie	影のみ表示非表示設定
 *
 *	@param	p_wk	ワーク
 *	@param	flag	フラグ
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkShadowDrawFlagSet( WF_2DCWK* p_wk, BOOL flag )
{
  if( p_wk->p_shadow ){
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_shadow, flag );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示非表示取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	表示
 *	@retval	FALSE	非表示
 */
//-----------------------------------------------------------------------------
BOOL WF_2DC_WkDrawFlagGet( const WF_2DCWK* cp_wk )
{
  return GFL_CLACT_WK_GetDrawEnable( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作タイプを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	動作タイプ
 */
//-----------------------------------------------------------------------------
WF_2DC_MOVETYPE WF_2DC_WkMoveTypeGet( const WF_2DCWK* cp_wk )
{
  return cp_wk->cp_res->movetype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションタイプを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	アニメーションタイプ
 */
//-----------------------------------------------------------------------------
WF_2DC_ANMTYPE WF_2DC_WkAnmTypeGet( const WF_2DCWK* cp_wk )
{
  return cp_wk->patanmtype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションフレーム設定
 *
 *	@param	p_wk		ワーク
 *	@param	frame		フレーム
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkAnmFrameSet( WF_2DCWK* p_wk, u16 frame )
{
  GFL_CLACT_WK_SetAnmIndex( p_wk->p_clwk, frame );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションフレーム取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	フレームインデックス
 */
//-----------------------------------------------------------------------------
u16 WF_2DC_WkAnmFrameGet( const WF_2DCWK* cp_wk )
{
  return GFL_CLACT_WK_GetAnmIndex( cp_wk->p_clwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰の位置を操作する
 *
 *	@param	p_wk	ワーク
 *	@param	x		ｘ座標
 *	@param	y		ｙ座標
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkShadowMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y )
{
  /*	VecFx32	mat;
	if( p_wk->p_shadow != NULL ){

		mat.x = x+WF_2DC_SHADOW_MAT_OFS_X<<FX32_SHIFT;
		mat.y = y+WF_2DC_SHADOW_MAT_OFS_Y<<FX32_SHIFT;
		mat.z = 0;
		CLACT_SetMatrix( p_wk->p_shadow, &mat );
	}
   */
  GFL_CLACTPOS pos;

  if( p_wk->p_shadow != NULL ){
    pos.x = x+WF_2DC_SHADOW_MAT_OFS_X;
    pos.y = y+WF_2DC_SHADOW_MAT_OFS_Y;
    GFL_CLACT_WK_SetWldPos(p_wk->p_shadow,&pos );
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief	Patternアニメ開始
 *
 *	@param	p_wk		ワーク
 *	@param	anmtype		アニメタイプ
 *	@param	anmway		アニメ方向
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkPatAnmStart( WF_2DCWK* p_wk, WF_2DC_ANMTYPE anmtype, WF_COMMON_WAY anmway )
{
  static void (* const pFunc[])( WF_2DCWK*, WF_COMMON_WAY ) = {
    WF_2DC_WkPatAnmWayInit,
    WF_2DC_WkPatAnmRotaInit,
    WF_2DC_WkPatAnmWalkInit,
    WF_2DC_WkPatAnmTurnInit,
    WF_2DC_WkPatAnmRunInit,
    WF_2DC_WkPatAnmWallWalkInit,
    WF_2DC_WkPatAnmSlowWalkInit,
    WF_2DC_WkPatAnmHighWalk2Init,
    WF_2DC_WkPatAnmHighWalk4Init,
    NULL
    };

  GF_ASSERT( anmtype < WF_2DC_ANMNUM );
  GF_ASSERT( anmway < WF_COMMON_WAYNUM );

  p_wk->patanmtype = anmtype;
  p_wk->patanmframe = 0;

  if( pFunc[ anmtype ] != NULL ){
    pFunc[ anmtype ]( p_wk, anmway );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	Patternアニメ終了チェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL WF_2DC_WkPatAnmEndCheck( const WF_2DCWK* cp_wk )
{
  if( WF_2DC_AnmFrame[ cp_wk->patanmtype ] == 0xff ){	// ループ設定
    return FALSE;
  }

  //終了チェック
  if( WF_2DC_AnmFrame[ cp_wk->patanmtype ] <= cp_wk->patanmframe ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Patternアニメを進める
 *
 *	@param	p_wk		ワーク
 *	@param	speed		スピード
 */
//-----------------------------------------------------------------------------
void WF_2DC_WkPatAnmAddFrame( WF_2DCWK* p_wk )
{
  BOOL updata = FALSE;

  if( WF_2DC_AnmFrame[ p_wk->patanmtype ] == 0xff ){	// ループ設定
    updata = TRUE;
  }else{

    if( WF_2DC_AnmFrame[ p_wk->patanmtype ] >= p_wk->patanmframe + 1 ){
      updata = TRUE;
    }
  }

  if( updata == TRUE ){
    WF_2DC_WkPatAnmUpdate( p_wk );	// アニメ更新
    p_wk->patanmframe ++;
  }
}


//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションリソース読み込み
 *
 *	@param	p_sys			システムワーク
 *	@param	movetype		動作タイプ
 *	@param	heap			ヒープ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_AnmResLoad( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype, u32 heap )
{
  int i;


  for(i = WF_2DC_UNICHAR_NUM; i < WF_2DC_CHARNUM; i++ ){
    p_sys->anmres[i].resid = GFL_CLGRP_CELLANIM_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                         WF_2DC_ARC_GETCEL(i-WB_HERO),
                                                         WF_2DC_ARC_GETANM(i-WB_HERO),
                                                         heap);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションリソース破棄
 *
 *	@param	p_sys			システムワーク
 *	@param	movetype		動作タイプ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_AnmResDel( WF_2DCSYS* p_sys, WF_2DC_MOVETYPE movetype )
{
  int i;

  for(i = WF_2DC_UNICHAR_NUM; i < WF_2DC_CHARNUM; i++ ){
    GFL_CLGRP_CELLANIM_Release(p_sys->anmres[i].resid);
    p_sys->anmres[i].resid = 0;
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief	見た目からキャラクタNOを取得する
 *
 *	@param	view_type	見た目（FIELDOBJCODE）
 *
 *	@return	キャラクタNO
 */
//-----------------------------------------------------------------------------
static u32 WF_2DC_CharNoGet( u32 view_type )
{
  return view_type;
}

//----------------------------------------------------------------------------
/**
 *	@brief	指定された人物に動作タイプを指定できるかチェック
 *
 *	@param	view_type		見た目
 *	@param	movetype		動作タイプ
 *
 *	@retval	TRUE	指定できる
 *	@retval	FALSE	指定できない
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_AnmModeLinkCheck( u32 view_type, WF_2DC_MOVETYPE movetype )
{
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース読み込み
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 *	@param	draw_type	転送先
 *	@param	movetype	動作タイプ
 *	@param	heap		ヒープ
 *	@param	restype		リソースタイプ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResLoad( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap, WF_2DC_RESTYPE restype )
{
  // 読み込み済みでないことをチェック
  //	GF_ASSERT( p_sys->res[ char_no ].resobj[0] == NULL );

  if( restype == WF_2DC_RES_NML ){
    WF_2DC_CharResLoadNml( p_sys, char_no, draw_type, movetype, heap );
  }else{
    WF_2DC_CharResLoadUni( p_sys, char_no, draw_type, movetype, heap );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクターリソース読み込み通常時
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 *	@param	draw_type	転送先
 *	@param	movetype	動作タイプ
 *	@param	heap		ヒープ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResLoadNml( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
  NNSG2dCharacterData* p_char;
  BOOL result;
  BOOL flip;	// フリップを使用するキャラクタか
  u32 cell_contid, anm_contid;


  p_sys->chrres[ char_no ].resid[ 0 ] = GFL_CLGRP_CGR_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                               WF_2DC_ARC_GETNCG( char_no-WB_HERO ),
                                                               FALSE , draw_type, heap);

  p_sys->chrres[ char_no ].resid[ 1 ] = GFL_CLGRP_PLTT_RegisterEx(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                                  WF_2DC_ARC_GETNCL( char_no-WB_HERO ),
                                                                  draw_type, WF_2DC_HERO_PLTT_START*0x20, 0, WF_2DC_HERO_PLTT_NUM, heap);



  flip = TRUE;

  // 管理パラメータ設定
  p_sys->chrres[ char_no ].drawtype = draw_type;
  p_sys->chrres[ char_no ].movetype = movetype;
  p_sys->chrres[ char_no ].type = WF_2DC_RES_NML;

}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクターリソース読み込み	ユニオン時
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 *	@param	draw_type	転送先
 *	@param	movetype	動作タイプ
 *	@param	heap		ヒープ
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResLoadUni( WF_2DCSYS* p_sys, u32 char_no, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap )
{
  NNSG2dCharacterData* p_char;
  BOOL result;
  u32 union_idx;


  p_sys->chrres[ char_no ].resid[ 0 ] = GFL_CLGRP_CGR_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
                                                               WF_2DC_ARC_GETUNINCG( char_no ),
                                                               FALSE , draw_type, heap);


  p_sys->chrres[ char_no ].resid[ 1 ] = p_sys->unionplttid;


  // 管理パラメータ設定
  p_sys->chrres[ char_no ].drawtype = draw_type;
  p_sys->chrres[ char_no ].movetype = movetype;
  p_sys->chrres[ char_no ].type = WF_2DC_RES_UNI;

}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース破棄
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResDel( WF_2DCSYS* p_sys, u32 char_no )
{

  if( p_sys->chrres[ char_no ].type == WF_2DC_RES_NML ){
    WF_2DC_CharResDelNml( p_sys, char_no );
  }else{
    WF_2DC_CharResDelUni( p_sys, char_no );
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース破棄	通常時
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResDelNml( WF_2DCSYS* p_sys, u32 char_no )
{

  GFL_CLGRP_CGR_Release(p_sys->chrres[ char_no ].resid[0]);
  GFL_CLGRP_PLTT_Release(p_sys->chrres[ char_no ].resid[1]);
  p_sys->chrres[ char_no ].resid[ 0 ] = GFL_CLGRP_REGISTER_FAILED;
  p_sys->chrres[ char_no ].resid[ 1 ] = GFL_CLGRP_REGISTER_FAILED;

}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース破棄	ユニオン時
 *
 *	@param	p_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 */
//-----------------------------------------------------------------------------
static void WF_2DC_CharResDelUni( WF_2DCSYS* p_sys, u32 char_no )
{
  GFL_CLGRP_CGR_Release(p_sys->chrres[ char_no ].resid[0]);
  p_sys->chrres[ char_no ].resid[ 0 ] = GFL_CLGRP_REGISTER_FAILED;

}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソースが読み込まれているかチェック
 *
 *	@param	cp_sys		システムワーク
 *	@param	char_no		キャラクタ番号
 *
 *	@retval	TRUE	読み込み完了
 *	@retval	FALSE	読み込んでいない
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_CharResCheck( const WF_2DCSYS* cp_sys, u32 char_no )
{
  if( cp_sys->chrres[ char_no ].resid[ 0 ] == GFL_CLGRP_REGISTER_FAILED ){
    return FALSE;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画タイプの取得
 *
 *	@param	cp_res	リソースワーク
 *
 *	@return	描画タイプ取得
 */
//-----------------------------------------------------------------------------
static u32 WF_2DC_CharResDrawTypeGet( const WF_2DCRES* cp_res )
{
  return cp_res->drawtype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作タイプ取得
 *
 *	@param	cp_res	リソースワーク
 *
 *	@return	動作タイプ
 */
//-----------------------------------------------------------------------------
static WF_2DC_MOVETYPE WF_2DC_CharResMoveTypeGet( const WF_2DCRES* cp_res )
{
  return cp_res->movetype;
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰リソースを読み込む
 *
 *	@param	p_sys		システムワーク
 *	@param	draw_type	表示画面
 *	@param	heap		ヒープID
 */
//-----------------------------------------------------------------------------
static void WF_2DC_ShResLoad( WF_2DCSYS* p_sys, u32 draw_type, u32 heap )
{
  int i;
  BOOL result;
  BOOL hero_load;
  //	u32 hero_no;
  //	GF_ASSERT( p_sys->shadowres.resobj[0] == NULL );

  // 主人公が読み込まれているかチェックする

  hero_load = FALSE;
  if( WF_2DC_CharResCheck( p_sys, WB_HERO ) == TRUE ){
    p_sys->hero_no		= WB_HERO;
    hero_load	= TRUE;
  }
  else if( WF_2DC_CharResCheck( p_sys, WB_HEROINE ) == TRUE ){
    p_sys->hero_no		= WB_HEROINE;
    hero_load	= TRUE;
  }

  GF_ASSERT_MSG( hero_load == TRUE, "shodow load miss  hero or heroine not load" );


  p_sys->shadowres.resid[2] = GFL_CLGRP_CELLANIM_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                          NARC_wifileadingchar_hero_ine_kage_NCER,
                                                          NARC_wifileadingchar_hero_ine_kage_NANR,
                                                          heap);

  p_sys->shadowres.resid[0] = GFL_CLGRP_CGR_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                     NARC_wifileadingchar_hero_ine_kage_NCGR,
                                                     FALSE , draw_type, heap);

  p_sys->shadowres.resid[1] = GFL_CLGRP_PLTT_RegisterEx(p_sys->p_handle[WF_2DC_ARCHANDLE_NML],
                                                        NARC_wifileadingchar_hero_ine_kage_NCLR,
                                                        draw_type, WF_2DC_HERO_KAGE_PLTT_START*0x20, 0, WF_2DC_HERO_KAGE_PLTT_NUM,heap);
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰リソースの破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_ShResDel( WF_2DCSYS* p_sys )
{
  int i;

  if( WF_2DC_ShResCheck( &p_sys->shadowres ) == TRUE ){

    // キャラクタリソース破棄
    GFL_CLGRP_CGR_Release(p_sys->shadowres.resid[0]);
    p_sys->shadowres.resid[ 0 ] = GFL_CLGRP_REGISTER_FAILED;
    GFL_CLGRP_PLTT_Release(p_sys->shadowres.resid[1]);
    p_sys->shadowres.resid[ 1 ] = GFL_CLGRP_REGISTER_FAILED;
    GFL_CLGRP_CELLANIM_Release(p_sys->shadowres.resid[2]);
    p_sys->shadowres.resid[ 2 ] = GFL_CLGRP_REGISTER_FAILED;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰リソースが読み込まれているかチェック
 *
 *	@param	cp_shadowres	ワーク
 *
 *	@retval	TRUE	リソース読み込み済み
 *	@retval	FALSE	リソースが読み込まれていない
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_ShResCheck( const WF_2DCSH_RES* cp_shadowres )
{

  //	if( cp_shadowres->resobj[0] != NULL ){
  if( cp_shadowres->resid[0] != GFL_CLGRP_REGISTER_FAILED ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタ用パレット読込み
 *
 *	@param	p_sys		システムワーク
 *	@param	draw_type	描画面
 *	@param	heap
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharPlttResLoad( WF_2DCSYS* p_sys, u32 draw_type, int offset, u32 heap )
{
  BOOL result;

  p_sys->unionplttid = GFL_CLGRP_PLTT_RegisterEx(p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
                                                 WF_2DC_ARC_GETUNINCL,
                                                 draw_type, offset, 0, WF_2DC_UNIPLTT_NUM,heap);

}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタ用パレット破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharPlttResDel( WF_2DCSYS* p_sys )
{
  //	GF_ASSERT( p_sys->p_unionpltt != NULL );

  GFL_CLGRP_PLTT_Release(p_sys->unionplttid);
  p_sys->unionplttid = GFL_CLGRP_REGISTER_FAILED;


  // ユニオンパレット破棄
  //	CLACT_U_PlttManagerDelete( p_sys->p_unionpltt );
  //	CLACT_U_ResManagerResDelete( p_sys->p_res_man[1], p_sys->p_unionpltt );
  //	p_sys->p_unionpltt = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタ用アニメリソース読込み
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharAnmResLoad( WF_2DCSYS* p_sys, u32 heap )
{
  int i;

  GF_ASSERT( p_sys->anmres[ 0 ].resobj[0] == NULL );

  for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){

    p_sys->anmres[i].resid = GFL_CLGRP_CELLANIM_Register(p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
                                                         WF_2DC_ARC_GETUNICEL(i),
                                                         WF_2DC_ARC_GETUNIANM(i),
                                                         heap);


    /*
		p_sys->unionres[i].resobj[ 0 ] =
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				p_sys->p_res_man[2], p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
				WF_2DC_ARC_GETUNICEL(i), FALSE,
				WF_2DC_ARC_CONTUNIONANMID+i, CLACT_U_CELL_RES, heap );

		p_sys->unionres[i].resobj[ 1 ] =
			CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
				p_sys->p_res_man[3], p_sys->p_handle[WF_2DC_ARCHANDLE_UNI],
				WF_2DC_ARC_GETUNIANM(i), FALSE,
				WF_2DC_ARC_CONTUNIONANMID+i, CLACT_U_CELLANM_RES, heap );
     */
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタ用アニメリソース破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_UniCharAnmResDel( WF_2DCSYS* p_sys )
{
  int i;

  //	GF_ASSERT( p_sys->unionres[ 0 ].resobj[0] != NULL );

  for( i=0; i<WF_2DC_UNICHAR_NUM; i++ ){
    GFL_CLGRP_CELLANIM_Release(p_sys->anmres[i].resid);
    p_sys->anmres[i].resid = GFL_CLGRP_REGISTER_FAILED;


    //		CLACT_U_ResManagerResDelete( p_sys->p_res_man[2],
    //					p_sys->unionres[i].resobj[ 0 ] );

    //		CLACT_U_ResManagerResDelete( p_sys->p_res_man[3],
    //					p_sys->unionres[i].resobj[ 1 ] );

    //		p_sys->unionres[ i ].resobj[0] = NULL;
    //		p_sys->unionres[ i ].resobj[1] = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	空のワークを取得
 *
 *	@param	p_sys	ワーク
 *
 *	@return	空のワーク
 */
//-----------------------------------------------------------------------------
static WF_2DCWK* WF_2DC_WkCleanGet( WF_2DCSYS* p_sys )
{
  int i;

  for( i=0; i<p_sys->objnum; i++ ){
    if( WF_2DC_WkCleanCheck( &p_sys->p_wk[i] ) == TRUE ){
      return &p_sys->p_wk[i];
    }
  }
  GF_ASSERT( 0 );
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Patternアニメ更新
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmUpdate( WF_2DCWK* p_wk )
{
  static void (* const pFunc[])( WF_2DCWK*  ) = {
    NULL,
    WF_2DC_WkPatAnmRotaMain,
    WF_2DC_WkPatAnmWalkMain,
    WF_2DC_WkPatAnmTurnMain,
    WF_2DC_WkPatAnmRunMain,
    WF_2DC_WkPatAnmWallWalkMain,
    WF_2DC_WkPatAnmSlowWalkMain,
    WF_2DC_WkPatAnmHighWalk2Main,
    WF_2DC_WkPatAnmHighWalk4Main,
    NULL
    };
  if( pFunc[p_wk->patanmtype] != NULL ){
    pFunc[p_wk->patanmtype]( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	方向変えアニメ初期化
 *
 *	@param	p_wk	ワーク
 *	@param	way		方向
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWayInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	回転アニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRotaInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_ROTA );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_ROTA );
}

//----------------------------------------------------------------------------
/**
 *	@brief	歩きアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	振り向きアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmTurnInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );

  // 後で設定することで２フレーム目で方向が変わる
  p_wk->patanmway = way;
}

//----------------------------------------------------------------------------
/**
 *	@brief	走りアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRunInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_RUN+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_RUN+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	壁方向歩きアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWallWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゆっくり歩きアニメ初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmSlowWalkInit( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	高速アニメ２フレーム
 *
 *	@param	p_wk		ワーク
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk2Init( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	高速アニメ４フレーム
 *
 *	@param	p_wk		ワーク
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk4Init( WF_2DCWK* p_wk, WF_COMMON_WAY way )
{
  p_wk->patanmway = way;
  //	CLACT_AnmChgCheck( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  GFL_CLACT_WK_SetAnmSeqDiff( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
}

//----------------------------------------------------------------------------
/**
 *	@brief	回転アニメ
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRotaMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_FRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_FRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	歩きアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWalkMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_FRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_FRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	振り向きアニメ
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmTurnMain( WF_2DCWK* p_wk )
{
  if( p_wk->patanmframe == 0 ){
    //	CLACT_AnmFrameSet( p_wk->p_clwk, 1 );
    GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, 1 );
  }else{
    //	CLACT_AnmChg( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
    GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, WF_2DC_ANM_WALK+p_wk->patanmway );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	走りアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmRunMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_FRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_FRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	壁方向歩きアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmWallWalkMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゆっくり歩きアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmSlowWalkMain( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_SLOWFRAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	高速２フレアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk2Main( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_HIGH2 );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_HIGH2 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	高速４フレアニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WF_2DC_WkPatAnmHighWalk4Main( WF_2DCWK* p_wk )
{
  //	CLACT_AnmFrameChg( p_wk->p_clwk, WF_2DC_ANM_HIGH4 );
  GFL_CLACT_WK_AddAnmFrame( p_wk->p_clwk, WF_2DC_ANM_HIGH4 );
}


//----------------------------------------------------------------------------
/**
 *	@brief	ワークが使われているかチェック
 *
 *	@param cp_wk	ワーク
 *
 *	@retval		TRUE　		未使用
 *	@retval		FALSE		使用中
 */
//-----------------------------------------------------------------------------
static BOOL WF_2DC_WkCleanCheck( const WF_2DCWK* cp_wk )
{
  if( cp_wk->p_clwk == NULL ){
    return TRUE;
  }
  return FALSE;
}


