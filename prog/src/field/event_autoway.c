//////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  移動床による強制移動イベント
 * @file   event_autoway.c
 * @author obata
 * @date   2009.11.27
 *
 * @todo SEをあてる
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "event_autoway.h"

#include "field_player.h"
#include "fldmmdl.h"
#include "fldmmdl_code.h"
#include "map_attr_def.h"  // for MATTR_xxxx


//====================================================================================================
// ■定数
//====================================================================================================
#define FRAME_PER_GRID (8)                             // 1グリッドの移動にかかる時間
#define FRAME_PER_DIRCHENGE (FRAME_PER_GRID/DIR_MAX4)  // 向きの変更間隔

// 処理シーケンス
enum
{
  SEQ_START,  // イベント開始
  SEQ_MAIN,   // メイン処理
  SEQ_EXIT,   // イベント終了
};


//====================================================================================================
// ■イベントワーク
//====================================================================================================
typedef struct
{
  GAMESYS_WORK*      gsys;  // ゲームシステム
  FIELDMAP_WORK* fieldmap;  // フィールドマップ
  FIELD_PLAYER*    player;  // 自機
  MMDL*              mmdl;  // 自機の動作モデル
  u16             moveDir;  // 移動方向
  u16              rotDir;  // 自機の向き
  u32            rotCount;  // 回転カウンタ

} EVENT_WORK;


//====================================================================================================
// ■非公開関数のプロトタイプ宣言
//====================================================================================================
static void UpdateRotDir( EVENT_WORK* work );
static void UpdateMoveDir( EVENT_WORK* work, MAPATTR_VALUE attrval );
static void MoveStart( EVENT_WORK* work );
static MAPATTR_VALUE GetAttribute( EVENT_WORK* work );
static void AttributeCheck( EVENT_WORK* work );
static BOOL HitCheck( EVENT_WORK* work );


//====================================================================================================
// ■イベント処理関数
//====================================================================================================
static GMEVENT_RESULT AutoMove( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  // イベント開始
  case SEQ_START:
    // 足元が移動床でなければ終了
    {
      MAPATTR_VALUE attrval = GetAttribute( work );
      if( (attrval != MATTR_MOVEF_LEFT) && (attrval != MATTR_MOVEF_RIGHT) &&
          (attrval != MATTR_MOVEF_UP)   && (attrval != MATTR_MOVEF_DOWN) )
      { 
        *seq = SEQ_EXIT;
        break;
      }
    }
    *seq = SEQ_MAIN;
    break;
  // メイン処理
  case SEQ_MAIN:
    // 回転
    UpdateRotDir( work );
    // 移動終了時
    if( MMDL_CheckEndAcmd(work->mmdl) ) // if(移動終了)
    {
      // アトリビュートチェック
      AttributeCheck( work );
      // 衝突チェック
      if( HitCheck(work) )  // if(衝突)
      { 
        *seq = SEQ_EXIT; 
        break;
      }
      // 移動開始
      MoveStart( work );
    }
    break;
  // イベント終了
  case SEQ_EXIT:
    // 変更したステータスビットを元に戻す
    MMDL_OffStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
    // 進行方向を向く
    MMDL_SetDirDisp( work->mmdl, work->moveDir );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//====================================================================================================
// ■外部公開関数
//====================================================================================================

//----------------------------------------------------------------------------------------------------
/**
 * @brief 移動床イベントを作成する
 *
 * @param parent   親となるイベント
 * @param gsys     ゲームシステム
 * @param fieldmap フィールドマップ
 *
 * @return 移動床イベント
 */
//----------------------------------------------------------------------------------------------------
GMEVENT* EVENT_PlayerMoveOnAutoWay( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // 生成
  event = GMEVENT_Create( gsys, parent, AutoMove, sizeof(EVENT_WORK) );
  // 初期化
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->fieldmap = fieldmap;
  work->player   = FIELDMAP_GetFieldPlayer( fieldmap );
  work->mmdl     = FIELD_PLAYER_GetMMdl( work->player );
  work->moveDir  = DIR_LEFT;
  work->rotDir   = MMDL_GetDirDisp( work->mmdl );
  work->rotCount = 0;
  return event;
} 


//====================================================================================================
// ■非公開関数のプロトタイプ宣言
//====================================================================================================

//----------------------------------------------------------------------------------------------------
/**
 * @brief 自機の向きを更新する
 *
 * @param work    イベントワーク
 */
//----------------------------------------------------------------------------------------------------
static void UpdateRotDir( EVENT_WORK* work )
{ 
  if( ++work->rotCount % FRAME_PER_DIRCHENGE == 0 )
  {
    // 更新
    work->rotDir = (work->rotDir + 1) % DIR_MAX4;
    // 動作モデルに反映
    MMDL_OffStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
    MMDL_SetDirDisp( work->mmdl, work->rotDir );
    MMDL_OnStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief 移動方向を更新する
 *
 * @param work    イベントワーク
 * @param attrval 足元のアトリビュート
 */
//----------------------------------------------------------------------------------------------------
static void UpdateMoveDir( EVENT_WORK* work, MAPATTR_VALUE attrval )
{
  switch( attrval ) 
  {
  case MATTR_MOVEF_RIGHT: work->moveDir = DIR_RIGHT; break;
  case MATTR_MOVEF_LEFT:  work->moveDir = DIR_LEFT;  break;
  case MATTR_MOVEF_UP:    work->moveDir = DIR_UP;    break;
  case MATTR_MOVEF_DOWN:  work->moveDir = DIR_DOWN;  break;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief 移動を開始する
 *
 * @param work イベントワーク
 */
//----------------------------------------------------------------------------------------------------
static void MoveStart( EVENT_WORK* work )
{ 
  switch( work->moveDir )
  {
  case DIR_LEFT:  MMDL_SetAcmd( work->mmdl, AC_WALK_L_8F ); break;
  case DIR_RIGHT: MMDL_SetAcmd( work->mmdl, AC_WALK_R_8F ); break;
  case DIR_UP:    MMDL_SetAcmd( work->mmdl, AC_WALK_U_8F ); break;
  case DIR_DOWN:  MMDL_SetAcmd( work->mmdl, AC_WALK_D_8F ); break;
  default:        return;
  }

  // 移動アニメーションによる向きの変更を一時停止
  MMDL_OnStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief 足元のアトリビュートを取得する
 *
 * @param work イベントワーク
 *
 * @return 主人公の足元のアトリビュート
 */
//----------------------------------------------------------------------------------------------------
static MAPATTR_VALUE GetAttribute( EVENT_WORK* work )
{
  VecFx32 pos;
  MAPATTR attr;
  MAPATTR_VALUE attrval;
  FLDMAPPER* mapper;

  // 足元のアトリビュートを取得
  FIELD_PLAYER_GetPos( work->player, &pos );
  mapper  = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
  attr    = MAPATTR_GetAttribute( mapper, &pos );
  attrval = MAPATTR_GetAttrValue( attr );
  return attrval;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief アトリビュートチェック
 *
 * @param work イベントワーク
 */
//----------------------------------------------------------------------------------------------------
static void AttributeCheck( EVENT_WORK* work )
{
  MAPATTR_VALUE attrval;

  // 足元のアトリビュートを取得
  attrval = GetAttribute( work );

  // 移動方向を更新
  UpdateMoveDir( work, attrval );
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief 衝突チェック
 *
 * @param work イベントワーク
 *
 * @return 衝突したら TRUE, そうでなければ FALSE
 */
//----------------------------------------------------------------------------------------------------
static BOOL HitCheck( EVENT_WORK* work )
{
  // 進行方向のグリッドをチェック
  u32 hitbit = MMDL_HitCheckMoveDir( work->mmdl, work->moveDir );
  return (hitbit != MMDL_MOVEHITBIT_NON);
}
