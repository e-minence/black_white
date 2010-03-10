//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_movepoke.c
 *	@brief  移動ポケモン　イベント
 *	@author	tomoya takahashi
 *	@date		2010.03.09
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "system/ica_anime.h"

#include "field/field_const.h"

#include "arc/arc_def.h"
#include "arc/event_movepoke.naix"


#include "fieldmap.h"
#include "fldmmdl.h"
#include "fieldmap_func.h"

#include "event_movepoke.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	アニメーションデータバッファ
//=====================================
enum
{
  ANIME_STATUS_NONE,
  ANIME_STATUS_DOING,
};


//-------------------------------------
///	アニメーションデータバッファ
//=====================================
#define EV_MOVEPOKE_ANIME_INTERVAL  (10)
#define EV_MOVEPOKE_ANIME_BUF ( 0x180 )



//-------------------------------------
///	モデル登録ヘッダー
//=====================================
static MMDL_HEADER s_MMDL_HEADER = {
  MMDL_ID_EVENTOBJ_00,
  0,
  MV_DMY,
  0,
  0,
  DIR_DOWN,
  0,0,0,0,0,
  MMDL_HEADER_POSTYPE_GRID,
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	移動ポケモン　動作ワーク
//=====================================
struct _EV_MOVEPOKE_WORK 
{
  MMDL* p_poke;
  u8 animebuf[ EV_MOVEPOKE_ANIME_BUF ];
  ICA_ANIME* p_anime;
  FLDMAPFUNC_WORK * p_task;

  u16 grid_x;
  u16 grid_z;

  u8 anime_req;
  u8 anime_no;
  u8 anime_status;
  u8 pad;

  HEAPID heapID;
};

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// 位置と回転を設定
static void MOVEPOKE_SetUpMMdl( const ICA_ANIME* cp_anime, MMDL* p_mmdl, u16 grid_ofs_x, u16 grid_ofs_z );



// タスク関数
static void MOVEPOKE_Update( FLDMAPFUNC_WORK* p_taskwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void MOVEPOKE_Draw( FLDMAPFUNC_WORK* p_taskwk, FIELDMAP_WORK* p_fieldmap, void* p_work );

//-------------------------------------
///	データ
//=====================================
static const FLDMAPFUNC_DATA sc_MAPFUNC_DATA = {
  0,
  sizeof(EV_MOVEPOKE_WORK),
  NULL,
  NULL,
  MOVEPOKE_Update,
  MOVEPOKE_Draw,
};



//----------------------------------------------------------------------------
/**
 *	@brief  移動ポケモン  ワーククリエイト
 *
 *	@param	p_fieldmap    フィールドマップ
 *	@param  objcode       オブジェコード
 *	@param  grid_x        xグリッド座標 配置
 *	@param  grid_z        zグリッド座標
 *	@param	heapID        ヒープID
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
EV_MOVEPOKE_WORK* EVENT_MOVEPOKE_Create( FIELDMAP_WORK* p_fieldmap, u32 objcode, u16 grid_x, u16 grid_z, HEAPID heapID )
{
  EV_MOVEPOKE_WORK* p_wk;
  MMDLSYS* p_fos = FIELDMAP_GetMMdlSys( p_fieldmap );
  FLDMAPFUNC_SYS * p_funcsys = FIELDMAP_GetFldmapFuncSys( p_fieldmap );
  FLDMAPFUNC_WORK* p_funcwk;

  
  p_funcwk = FLDMAPFUNC_Create( GFL_OVERLAY_BLANK_ID, p_funcsys, &sc_MAPFUNC_DATA );
  p_wk     = FLDMAPFUNC_GetFreeWork( p_funcwk );
  p_wk->p_task    = p_funcwk;
  p_wk->heapID    = heapID;

  p_wk->grid_x    = grid_x;
  p_wk->grid_z    = grid_z;

  s_MMDL_HEADER.obj_code = objcode;
  p_wk->p_poke = MMDLSYS_AddMMdl(
	  p_fos, &s_MMDL_HEADER, FIELDMAP_GetZoneID(p_fieldmap) );

  // 非表示
  MMDL_SetStatusBitVanish( p_wk->p_poke, TRUE );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  移動ポケモン　ワーク破棄
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void EVENT_MOVEPOKE_Delete( EV_MOVEPOKE_WORK* p_wk )
{
  // モデル破棄
  MMDL_Delete( p_wk->p_poke );
  p_wk->p_poke = NULL;

  // タスク破棄
  FLDMAPFUNC_Delete( p_wk->p_task );
}


//----------------------------------------------------------------------------
/**
 *	@brief  移動ポケモン　アニメーション開始
 *
 *	@param	p_wk      ワーク
 *	@param	anime_no  アニメーションナンバー 
 */
//-----------------------------------------------------------------------------
void EVENT_MOVEPOKE_StartAnime( EV_MOVEPOKE_WORK* p_wk, EV_MOVEPOKE_ANIME_TYPE anime_no )
{
  GF_ASSERT( p_wk->anime_status == ANIME_STATUS_NONE );

  // アニメーション生成
  p_wk->p_anime = ICA_ANIME_CreateStreaming( p_wk->heapID, ARCID_EV_MOVEPOKE, 
      NARC_event_movepoke_rai_kaza_01_bin + anime_no, 
      EV_MOVEPOKE_ANIME_INTERVAL, p_wk->animebuf, EV_MOVEPOKE_ANIME_BUF );

  // アニメーション開始
  p_wk->anime_status = ANIME_STATUS_DOING;

  // ポケモンの表示ON
  MMDL_SetStatusBitVanish( p_wk->p_poke, FALSE );


  MOVEPOKE_SetUpMMdl( p_wk->p_anime, p_wk->p_poke, p_wk->grid_x, p_wk->grid_z );
}

//----------------------------------------------------------------------------
/**
 *	@brief  移動ポケモン　アニメーション終了チェック
 *
 *	@param	cp_wk   ワーク
 *
 *	@retval TRUE    終了
 *	@retval FALSE   途中
 */
//-----------------------------------------------------------------------------
BOOL EVENT_MOVEPOKE_IsAnimeEnd( const EV_MOVEPOKE_WORK* cp_wk )
{
  if( cp_wk->anime_status != ANIME_STATUS_NONE ){
    return FALSE;
  }
  return TRUE;
}







//-----------------------------------------------------------------------------
/**
 *      private関数
 */
//-----------------------------------------------------------------------------
  
//----------------------------------------------------------------------------
/**
 *	@brief  アップデート
 *
 *	@param	p_taskwk        タスクワーク
 *	@param	p_fieldmap      フィールドマップ
 *	@param	p_work          ワーク
 */
//-----------------------------------------------------------------------------
static void MOVEPOKE_Update( FLDMAPFUNC_WORK* p_taskwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  EV_MOVEPOKE_WORK* p_wk = p_work;

  switch( p_wk->anime_status ){
  case ANIME_STATUS_NONE:
    break;

  case ANIME_STATUS_DOING:
    {
      BOOL result;

      // アニメーションを進めて反映
      result = ICA_ANIME_IncAnimeFrame( p_wk->p_anime, FX32_ONE );
      
      // 反映
      MOVEPOKE_SetUpMMdl( p_wk->p_anime, p_wk->p_poke, p_wk->grid_x, p_wk->grid_z );
      
      if( result == TRUE ){

        // 終了
        ICA_ANIME_Delete( p_wk->p_anime );
        p_wk->p_anime = NULL;
        p_wk->anime_status = ANIME_STATUS_NONE;
      }
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  描画
 */
//-----------------------------------------------------------------------------
static void MOVEPOKE_Draw( FLDMAPFUNC_WORK* p_taskwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
}



//----------------------------------------------------------------------------
/**
 *	@brief  モデルのセットアップ
 *
 *	@param	cp_anime  アニメーションデータ
 *	@param	p_mmdl    動作モデル
 */
//-----------------------------------------------------------------------------
static void MOVEPOKE_SetUpMMdl( const ICA_ANIME* cp_anime, MMDL* p_mmdl, u16 grid_ofs_x, u16 grid_ofs_z )
{
  VecFx32 trans;
  VecFx32 rotate;

  ICA_ANIME_GetTranslate( cp_anime, &trans );
  ICA_ANIME_GetRotate( cp_anime, &rotate );

  trans.x += GRID_TO_FX32( grid_ofs_x );
  trans.z += GRID_TO_FX32( grid_ofs_z );
  
  // 位置反映
  MMDL_SetVectorPos( p_mmdl, &trans );

  // 角度から、方向反映

}

