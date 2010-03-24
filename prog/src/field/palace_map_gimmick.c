//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		palace_map_gimmick.c
 *	@brief  パレスマップ　ギミック
 *	@author	tomoya takahashi
 *	@date		2010.03.20
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "fieldmap.h"
#include "palace_map_gimmick.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"
#include "p_tree.naix"
#include "palace_effect.naix"
#include "field/field_comm/intrude_field.h"
#include "field/field_comm/intrude_work.h"

#include "eventdata_local.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


// 拡張オブジェクト・ユニット番号
#define EXPOBJ_UNIT_IDX (0)
// ギミックワークのアサインID
#define GIMMICK_WORK_ASSIGN_ID (0)


//-------------------------------------
///	ギミックワーク
//=====================================
// リソースインデックス
enum {
  MAP_RES_CUBE,           // 
  MAP_RES_CUBE_ANIME,     // 
  
  MAP_RES_NUM
} ;
static const GFL_G3D_UTIL_RES map_res_tbl[ MAP_RES_NUM ] = 
{
  { ARCID_PALACE_EFFECT, NARC_palace_effect_block_nsbmd, GFL_G3D_UTIL_RESARC },     // 
  { ARCID_PALACE_EFFECT, NARC_palace_effect_block_nsbma, GFL_G3D_UTIL_RESARC },     // 
};

// アニメーションインデックス
enum{
  MAP_ANM_CUBE_ANIME,

  MAP_ANM_CUBE_NUM,
} ;
static const GFL_G3D_UTIL_ANM map_res_cube_anm_tbl[ MAP_ANM_CUBE_NUM ] = 
{
  { MAP_RES_CUBE_ANIME, 0 },
};

// オブジェクトインデックス
enum {
  MAP_OBJ_CUBE,

  MAP_OBJ_NUM
} ;
static const GFL_G3D_UTIL_OBJ map_obj_table[ MAP_OBJ_NUM ] = 
{
  // モデルリソースID, 
  // モデルデータID(リソース内部INDEX), 
  // テクスチャリソースID,
  // アニメテーブル, 
  // アニメリソース数
  
  //白
  { MAP_OBJ_CUBE, 0, MAP_RES_CUBE, map_res_cube_anm_tbl, MAP_ANM_CUBE_NUM },
};

static const GFL_G3D_UTIL_SETUP map_setup = { map_res_tbl, MAP_RES_NUM, map_obj_table, MAP_OBJ_NUM };





//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	ギミックワーク
//=====================================
typedef struct {
  BOOL on;

  GFL_G3D_OBJSTATUS* objstatus;
  
} PALACE_MAP_GMK_WORK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static BOOL IsHit( PALACE_MAP_GMK_WORK* wk, const FIELD_PLAYER* player );




//----------------------------------------------------------------------------
/**
 *	@brief  パレス　進入先の街でのギミックをセットアップ
 *
 *	@param	fieldWork   フィールドワーク
 */
//-----------------------------------------------------------------------------
void PALACE_MAP_GMK_Setup(FIELDMAP_WORK *fieldWork)
{
  PALACE_MAP_GMK_WORK* wk;  //
  HEAPID                heapID = FIELDMAP_GetHeapID( fieldWork );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //汎用ワーク確保
  wk = GMK_TMP_WK_AllocWork
      (fieldWork, GIMMICK_WORK_ASSIGN_ID, heapID, sizeof(PALACE_MAP_GMK_WORK));
  // 拡張オブジェクトのユニットを追加
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &map_setup, EXPOBJ_UNIT_IDX );

  // 表示OFF
  FLD_EXP_OBJ_SetVanish( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, TRUE );

  // OBJSTATUS取得
  wk->objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE );

  // アニメOFF
  {
    EXP_OBJ_ANM_CNT_PTR anime = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, 
        EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, MAP_ANM_CUBE_ANIME );
    FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, MAP_ANM_CUBE_ANIME, TRUE );
    FLD_EXP_OBJ_ChgAnmStopFlg( anime, TRUE );
    FLD_EXP_OBJ_ChgAnmLoopFlg( anime, FALSE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  パレス　進入先の街でのギミックを終了
 *
 *	@param	fieldWork   フィールドワーク
 */
//-----------------------------------------------------------------------------
void PALACE_MAP_GMK_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //ユニット破棄
  FLD_EXP_OBJ_DelUnit( exobj_cnt, EXPOBJ_UNIT_IDX );  
  //汎用ワーク解放
  GMK_TMP_WK_FreeWork(fieldWork, GIMMICK_WORK_ASSIGN_ID);
}

//----------------------------------------------------------------------------
/**
 *	@brief  パレス　進入先の街でのギミック動作
 *
 *	@param	fieldWork   フィールドワーク
 */
//-----------------------------------------------------------------------------
void PALACE_MAP_GMK_Move(FIELDMAP_WORK *fieldWork)
{
  PALACE_MAP_GMK_WORK* wk;  //
  EXP_OBJ_ANM_CNT_PTR anime;
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldWork );
  VecFx32 pos;
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  EVENTDATA_SYSTEM* evsys = GAMEDATA_GetEventData( gdata );

  // ワーク取得
  wk = GMK_TMP_WK_GetWork( fieldWork, GIMMICK_WORK_ASSIGN_ID );

  // アニメーションコントローラー取得
  anime = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, 
      EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, MAP_ANM_CUBE_ANIME );
  
  // 表示キューブOFF
  if( wk->on ){

    FLD_EXP_OBJ_PlayAnime( exobj_cnt );
    
    if( FLD_EXP_OBJ_ChkAnmEnd(anime) ){
      // OFF
      FLD_EXP_OBJ_SetVanish( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, TRUE );
      wk->on = FALSE;
      //TOMOYA_Printf( "cube off\n" );
    }
  }

  // 位置チェック
  // 自機がヒット状態の場合、
  // 目の前にダミーイベントがあったら、
  // キューブを表示する。
  if(  IsHit(wk, player) ){
    // 位置チェック
    FIELD_PLAYER_GetDirPos( player, FIELD_PLAYER_GetDir(player), &pos );
    pos.x -= MMDL_VEC_X_GRID_OFFS_FX32;
    pos.z -= MMDL_VEC_Z_GRID_OFFS_FX32;

    if( EVENTDATA_SYS_CheckPosDummyEvent( evsys, &pos ) ){
      wk->on        = TRUE;
      

      // 表示ON
      FLD_EXP_OBJ_SetVanish( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, FALSE );
      FLD_EXP_OBJ_SetObjAnmFrm( exobj_cnt, 
        EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, MAP_ANM_CUBE_ANIME, 0 );
      FLD_EXP_OBJ_ChgAnmStopFlg( anime, FALSE );

      wk->objstatus->trans = pos;

      //TOMOYA_Printf( "cube on x[%d] y[%d] z[%d]\n", FX_Whole(pos.x), FX_Whole(pos.y), FX_Whole(pos.z) );
    }
  }
}




//-----------------------------------------------------------------------------
/**
 *      private関数
 */
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  壁ヒット中のアクションコマンドかチェック
 *
 *	@param	acmd  アクションコマンド
 *
 *	@retval TRUE    ヒット中
 *	@retval FALSE   そのた動作中
 */
//-----------------------------------------------------------------------------
static BOOL IsHit( PALACE_MAP_GMK_WORK* wk, const FIELD_PLAYER* player )
{
  int i;
  u32 move_value = FIELD_PLAYER_GetMoveValue( player );
  u32 move_status = FIELD_PLAYER_GetMoveState( player );

  //TOMOYA_Printf( "move_value [%d] move_status [%d]\n", move_value, move_status );
  
  if( (move_status == PLAYER_MOVE_STATE_END) && (move_value == PLAYER_MOVE_VALUE_STOP) ){

    return TRUE;
  }
  return FALSE;
}
