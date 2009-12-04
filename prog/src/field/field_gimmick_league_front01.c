////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief ギミック処理(ポケモンリーグフロント上)
 * @file field_gimmick_league_front01.c
 * @author obata
 * @date 2009.12.01
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#include "fieldmap.h"
#include "field_gimmick_league_front01.h"
#include "gamesystem/game_event.h"
#include "gamesystem/gamesystem.h"
#include "savedata/gimmickwork.h"  // for GIMMICKWORK
#include "system/ica_anime.h"  // for ICA_ANIME
#include "field_gimmick_def.h"  // for FLD_GIMMICK_C09P01
#include "field/field_const.h"  // for FIELD_CONST_GRID_SIZE
#include "fld_exp_obj.h"  // for FLD_EXP_OBJ
#include "arc/league_front.naix"  // for NARC_xxxx
#include "../../../resource/fldmapdata/zonetable/zone_id.h"  // for ZONE_ID_C09P02
#include "event_mapchange.h"  // for EVENT_ChangeMapPosNoFade
#include "event_fieldmap_control.h"  // for EVENT_FieldBrightOut
#include "field_gimmick_league_front02.h"  // for EVENT_LEAGUE_FRONT_02_GIMMICK_GetLiftDownEvent


//==========================================================================================
// ■定数
//==========================================================================================
#define ARCID (ARCID_LEAGUE_FRONT_GIMMICK)  // ギミックデータのアーカイブID

// リフト座標
#define LIFT_POS_X_GRID (30)
#define LIFT_POS_Z_GRID (43)
#define LIFT_POS_X ((LIFT_POS_X_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIFT_POS_Y (2 << FX32_SHIFT)
#define LIFT_POS_Z ((LIFT_POS_Z_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)

// ギミックワークのデータインデックス
typedef enum{
  GIMMICKWORK_DATA_WORK_ADRS,  // LF01ギミック管理ワークのアドレス
  GIMMICKWORK_DATA_NUM,
  GIMMICKWORK_DATA_MAX = GIMMICKWORK_DATA_NUM - 1
} GIMMICKWORK_DATA_INDEX;


//==========================================================================================
// ■3Dリソース
//==========================================================================================
//----------
// リソース
//----------
typedef enum{
  RES_LIFT_NSBMD,          // リフトのモデル
  RES_LIFT_NSBTA_ALL,      // 四天王(全員)　　を倒した時のita
  RES_LIFT_NSBTA_KAKUTOU,  // 四天王(格闘)　　を倒した時のita
  RES_LIFT_NSBTA_AKU,      // 四天王(悪)　　　を倒した時のita
  RES_LIFT_NSBTA_GHOST,    // 四天王(ゴースト)を倒した時のita
  RES_LIFT_NSBTA_ESPER,    // 四天王(エスパー)を倒した時のita
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[RES_NUM] = 
{
  {ARCID, NARC_league_front_pl_ele_00_nsbmd,    GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_00_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_01_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_02_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_03_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_04_nsbta, GFL_G3D_UTIL_RESARC},
}; 
//------------------------
// アニメーション(リフト)
//------------------------
typedef enum{
  LIFT_ANM_ALL,     // 四天王(全員)　　を倒した時のita
  LIFT_ANM_KAKUTOU, // 四天王(格闘)　　を倒した時のita 
  LIFT_ANM_AKU,     // 四天王(悪)　　　を倒した時のita
  LIFT_ANM_GHOST,   // 四天王(ゴースト)を倒した時のita
  LIFT_ANM_ESPER,   // 四天王(エスパー)を倒した時のita
  LIFT_ANM_NUM
} LIFT_ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table_lift[LIFT_ANM_NUM] = 
{
  {RES_LIFT_NSBTA_ALL, 0},
  {RES_LIFT_NSBTA_KAKUTOU, 0},
  {RES_LIFT_NSBTA_AKU, 0},
  {RES_LIFT_NSBTA_GHOST, 0},
  {RES_LIFT_NSBTA_ESPER, 0},
}; 
//-------------
// オブジェクト
//-------------
typedef enum{
  OBJ_LIFT,  // リフト
  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[OBJ_NUM] = 
{
  {RES_LIFT_NSBMD, 0, RES_LIFT_NSBMD, anm_table_lift, LIFT_ANM_NUM},  // リフト
}; 
//----------
// ユニット
//----------
typedef enum{
  UNIT_GIMMICK,
  UNIT_NUM
} UNIT_INDEX;
static const GFL_G3D_UTIL_SETUP unit[UNIT_NUM] = 
{ 
  {res_table, RES_NUM, obj_table, OBJ_NUM},
};


//==========================================================================================
// ■ギミックワーク
//==========================================================================================
typedef struct
{ 
  HEAPID heapID;  // 使用するヒープID

} LF01WORK;


//==========================================================================================
// ■非公開関数のプロトタイプ宣言
//==========================================================================================
static void Save( FIELDMAP_WORK* fieldmap, GIMMICKWORK_DATA_INDEX idx, u32 data );
static u32 Load( FIELDMAP_WORK* fieldmap, GIMMICKWORK_DATA_INDEX idx );
static void InitGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap );
static void LoadGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap );
static void SaveGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap );


//==========================================================================================
// ■ギミック登録関数
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief 初期化関数
 *
 * @param fieldmap ギミック動作フィールドマップ
 */
//------------------------------------------------------------------------------------------
void LEAGUE_FRONT_01_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  LF01WORK* work;
  HEAPID heap_id;

  // DEBUG:
  OBATA_Printf( "GIMMICK-LF01: setup\n" ); 

  // ギミック管理ワーク作成
  heap_id      = FIELDMAP_GetHeapID( fieldmap );
  work         = (LF01WORK*)GFL_HEAP_AllocMemory( heap_id, sizeof(LF01WORK) );
  work->heapID = heap_id; 

  // ギミック初期化
  InitGimmick( work, fieldmap );
  LoadGimmick( work, fieldmap ); 

  // LF01ギミック管理ワークのアドレスを保存
  Save( fieldmap, GIMMICKWORK_DATA_WORK_ADRS, (u32)work );

}

//------------------------------------------------------------------------------------------
/**
 * @brief 終了関数
 *
 * @param fieldmap ギミック動作フィールドマップ
 */
//------------------------------------------------------------------------------------------
void LEAGUE_FRONT_01_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  LF01WORK* work;

  // セーブ
  work = (LF01WORK*)Load( fieldmap, GIMMICKWORK_DATA_WORK_ADRS );
  SaveGimmick( work, fieldmap );

  // ギミック管理ワーク破棄
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OBATA_Printf( "GIMMICK-LF01: end\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 動作関数
 *
 * @param fieldmap ギミック動作フィールドマップ
 */
//------------------------------------------------------------------------------------------
void LEAGUE_FRONT_01_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  // 拡張オブジェクトのアニメ再生
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cnt;
    exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
    FLD_EXP_OBJ_PlayAnime( exobj_cnt );
  } 
}


//==========================================================================================
// ■ 非公開関数
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief ギミックワークへのセーブ処理
 *
 * @param fieldmap フィールドマップ
 * @param idx      格納先インデックス
 * @param data     格納する実データ
 */
//------------------------------------------------------------------------------------------
static void Save( FIELDMAP_WORK* fieldmap, GIMMICKWORK_DATA_INDEX idx, u32 data )
{
  GAMESYS_WORK*   gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*      gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK* gmkwork = GAMEDATA_GetGimmickWork( gdata );
  u32*       save_work = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_C09P01 );

  // ギミックワークに保存
  save_work[idx] = data;

  // DEBUG:
  OBATA_Printf( "GIMMICK-FL01: save[%d] = %d\n", idx, data );
}

//------------------------------------------------------------------------------------------
/**
 * @brief ギミックワークからのロード処理
 *
 * @param fieldmap フィールドマップ
 * @param idx      取得データのインデックス
 *
 * @return 指定したインデックスに格納した値
 */
//------------------------------------------------------------------------------------------
static u32 Load( FIELDMAP_WORK* fieldmap, GIMMICKWORK_DATA_INDEX idx )
{
  GAMESYS_WORK*   gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*      gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK* gmkwork = GAMEDATA_GetGimmickWork( gdata );
  u32*       save_work = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_C09P01 );
  u32              val = 0;

  // ギミックワークから取得
  val = save_work[idx];

  // DEBUG:
  OBATA_Printf( "GIMMICK-FL01: load[%d] = %d\n", idx, val );

  return val;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ギミックを初期化する
 *
 * @param work     初期化対象ワーク
 * @param fieldmap 依存するフィールドマップ
 */
//------------------------------------------------------------------------------------------
static void InitGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;

  // 拡張オブジェクトのユニットを追加
  exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &unit[UNIT_GIMMICK], UNIT_GIMMICK );

  // リフトのアニメーション初期化
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_ALL,     TRUE );
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_KAKUTOU, FALSE );
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_AKU,     FALSE );
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_GHOST,   FALSE );
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_ESPER,   FALSE );

  // リフトの座標を初期化
  {
    GFL_G3D_OBJSTATUS* objstatus;
    objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT );
    VEC_Set( &objstatus->trans, LIFT_POS_X, LIFT_POS_Y, LIFT_POS_Z );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief ギミックの状態を復帰する
 *
 * @param work     復帰対象ギミックの管理ワーク
 * @param fieldmap 依存するフィールドマップ
 */
//------------------------------------------------------------------------------------------
static void LoadGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap )
{
} 

//------------------------------------------------------------------------------------------
/**
 * @brief ギミックの状態を保存する
 *
 * @param work     保存対象ギミックの管理ワーク
 * @param fieldmap 依存するフィールドマップ
 */
//------------------------------------------------------------------------------------------
static void SaveGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap )
{
}


//==========================================================================================
// ■ イベント
//==========================================================================================
typedef struct
{
  GAMESYS_WORK* gsys;
  FIELDMAP_WORK* fieldmap;
  ICA_ANIME* liftAnime;  // リフトの移動アニメーション

} LIFTDOWN_EVENTWORK;

//------------------------------------------------------------------------------------------
/**
 * @brief リフト降下イベント処理
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT LiftDownEvent( GMEVENT* event, int* seq, void* wk )
{
  LIFTDOWN_EVENTWORK* work = (LIFTDOWN_EVENTWORK*)wk;

  switch( *seq )
  {
  // アニメ開始
  case 0:
    {
      HEAPID heap_id;
      heap_id = FIELDMAP_GetHeapID( work->fieldmap );
      work->liftAnime = ICA_ANIME_CreateAlloc( heap_id, ARCID, 
                                               NARC_league_front_pl_ele_00_ica_bin );
    }
    ++(*seq);
    break;
  // アニメ終了待ち
  case 1:
    // 自機, リフトの座標を更新
    {
      VecFx32 trans, pos;
      FIELD_PLAYER* player;
      GFL_G3D_OBJSTATUS* lift_status;
      FLD_EXP_OBJ_CNT_PTR exobj_cnt;
      // リフト
      ICA_ANIME_GetTranslate( work->liftAnime, &trans );
      exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
      lift_status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT );
      lift_status->trans.y = trans.y;
      // 自機
      player = FIELDMAP_GetFieldPlayer( work->fieldmap );
      FIELD_PLAYER_GetPos( player, &pos );
      pos.y = trans.y;
      FIELD_PLAYER_SetPos( player, &pos );
    }
    // アニメーション更新
    if( ICA_ANIME_IncAnimeFrame( work->liftAnime, FX32_ONE ) )  // if(アニメ終了)
    { 
      ICA_ANIME_Delete( work->liftAnime );
      ++(*seq); 
    }
    break;
  // フェードアウト
  case 2:
    {
      GMEVENT* new_event;
      new_event = EVENT_FieldFadeOut( work->gsys, work->fieldmap,
                                      FIELD_FADE_BLACK, 
                                      FIELD_FADE_WAIT );
      GMEVENT_CallEvent( event, new_event );
    }
    ++(*seq);
    break;
  // マップチェンジ
  case 3:
    {
      GMEVENT* new_event;
      VecFx32 pos;
      VEC_Set( &pos, 0, 0, 0 );
      new_event = EVENT_ChangeMapPosNoFade( work->gsys, work->fieldmap, 
                                            ZONE_ID_C09P02, &pos, DIR_DOWN );
      GMEVENT_CallEvent( event, new_event );
    }
    ++(*seq);
    break;
  // 次のイベントへ切り替え
  case 4:
    {
      GMEVENT* next_event;
      next_event = LEAGUE_FRONT_02_GIMMICK_GetLiftDownEvent( work->gsys, work->fieldmap );
      GMEVENT_CallEvent( event, next_event );
    }
    ++(*seq);
    break;
  // 終了
  case 5:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief リフト降下イベントを作成する
 * 
 * @param gsys     ゲームシステム
 * @param fieldmap フィールドマップ
 *
 * @return リフト降下イベント
 */
//------------------------------------------------------------------------------------------
GMEVENT* LEAGUE_FRONT_01_GIMMICK_GetLiftDownEvent( GAMESYS_WORK* gsys, 
                                                   FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  LIFTDOWN_EVENTWORK* evwork;
  LF01WORK* gmkwork = (LF01WORK*)Load( fieldmap, GIMMICKWORK_DATA_WORK_ADRS );

  // 生成
  event = GMEVENT_Create( gsys, NULL, LiftDownEvent, sizeof(LIFTDOWN_EVENTWORK) );
  // 初期化
  evwork = GMEVENT_GetEventWork( event );
  evwork->gsys      = gsys;
  evwork->fieldmap  = fieldmap;
  evwork->liftAnime = NULL;
  return event;
}
