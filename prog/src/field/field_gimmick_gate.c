//////////////////////////////////////////////////////////////////////////////////////////// 
/** 
 *
 * @brief  ギミック登録関数(ゲート) 
 * @file   field_gimmick_gate.c 
 * @author obata 
 * @date   2009.10.21 
 *
 * [電光掲示板の追加方法]
 * �@配置したいゾーンでギミックが動作するように, fld_gimmick_assign.xls に登録
 * �A登録したゾーンでのギミック動作関数を, field_gimmick.c に登録
 * �B配置するゾーンの電光掲示板データを作成し, 電光掲示板データ登録テーブルに追加
 *
 */ 
//////////////////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h> 
#include "field_gimmick_gate.h" 
#include "arc/arc_def.h" 
#include "arc/gate.naix"
#include "arc/message.naix"

#include "field/gimmick_obj_elboard.h"
#include "savedata/gimmickwork.h"
#include "gimmick_obj_elboard.h"
#include "elboard_zone_data.h"
#include "elboard_spnews_data.h"
#include "arc/gate.naix"
#include "field_gimmick_def.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "msg/msg_gate.h"
#include "field/zonedata.h"
#include "field/enc_pokeset.h"  // for ENCPOKE_GetGenerateZone

#include "field_task_manager.h"
#include "field_task.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"


//==========================================================================================
// ■電光掲示板データ登録テーブル
//==========================================================================================
typedef struct
{
  u16 zone_id;
  int gimmick_id;
  u32 dat_id; 
} ENTRY_DATA; 
static const ENTRY_DATA entry_table[] = 
{
  { ZONE_ID_C04R0601, FLD_GIMMICK_C04R0601, NARC_gate_elboard_zone_data_c04r0601_bin },
  { ZONE_ID_C08R0601, FLD_GIMMICK_C08R0601, NARC_gate_elboard_zone_data_c08r0601_bin },
  { ZONE_ID_R13R0201, FLD_GIMMICK_R13R0201, NARC_gate_elboard_zone_data_r13r0201_bin },
  { ZONE_ID_R02R0101, FLD_GIMMICK_R02R0101, NARC_gate_elboard_zone_data_r02r0101_bin },
  { ZONE_ID_C04R0701, FLD_GIMMICK_C04R0701, NARC_gate_elboard_zone_data_c04r0701_bin },
  { ZONE_ID_C04R0801, FLD_GIMMICK_C04R0801, NARC_gate_elboard_zone_data_c04r0801_bin },
  { ZONE_ID_C02R0701, FLD_GIMMICK_C02R0701, NARC_gate_elboard_zone_data_c02r0701_bin },
  { ZONE_ID_R14R0101, FLD_GIMMICK_R14R0101, NARC_gate_elboard_zone_data_r14r0101_bin },
  { ZONE_ID_C08R0501, FLD_GIMMICK_C08R0501, NARC_gate_elboard_zone_data_c08r0501_bin },
  { ZONE_ID_C08R0701, FLD_GIMMICK_C08R0701, NARC_gate_elboard_zone_data_c08r0701_bin },
  { ZONE_ID_H01R0101, FLD_GIMMICK_H01R0101, NARC_gate_elboard_zone_data_h01r0101_bin },
  { ZONE_ID_H01R0201, FLD_GIMMICK_H01R0201, NARC_gate_elboard_zone_data_h01r0201_bin },
  { ZONE_ID_C03R0601, FLD_GIMMICK_C03R0601, NARC_gate_elboard_zone_data_c03r0601_bin },
};

//------------------------------------------------------------------------------------------
/**
 * @brief 指定したギミックIDが登録されているかどうか判定する
 *
 * @param gmk_id 判定対象ギミックID
 *
 * @return 登録されている場合 TRUE
 */
//------------------------------------------------------------------------------------------
static BOOL IsGimmickIDEntried( int gmk_id )
{
  int i;
  for( i=0; i<NELEMS(entry_table); i++ )
  {
    if( entry_table[i].gimmick_id == gmk_id )
    {
      return TRUE;  // 発見
    }
  }
  return FALSE; // 未発見
}


//==========================================================================================
// ■定数
//========================================================================================== 
// 一度に表示する地域情報の数
#define LOCAL_INFO_NUM (3)

// 拡張オブジェクト・ユニット番号
#define EXPOBJ_UNIT_ELBOARD (1)

// 表示可能な一画面に表示可能な文字数
#define DISPLAY_SIZE (8)

// ニュース間の間隔(文字数)
#define NEWS_INTERVAL (3)

// 一度に表示するジム情報の最大数
#define GYM_NEWS_MAX_NUM (4)


//==========================================================================================
// ■3Dリソース
//==========================================================================================
// リソースインデックス
typedef enum
{
  RES_ELBOARD_NSBMD,    // 掲示板のモデル
  RES_ELBOARD_NSBTX,    // 掲示板のテクスチャ
  RES_ELBOARD_NSBTA_1,  // ニュース・スクロール・アニメーション1
  RES_ELBOARD_NSBTA_2,  // ニュース・スクロール・アニメーション2
  RES_ELBOARD_NSBTA_3,  // ニュース・スクロール・アニメーション3
  RES_ELBOARD_NSBTA_4,  // ニュース・スクロール・アニメーション4
  RES_ELBOARD_NSBTA_5,  // ニュース・スクロール・アニメーション5
  RES_ELBOARD_NSBTA_6,  // ニュース・スクロール・アニメーション6
  RES_ELBOARD_NSBTA_7,  // ニュース・スクロール・アニメーション7
  RES_MONITOR_NSBTP_1,  // モニター・テクスチャパターン・アニメーション1
  RES_MONITOR_NSBTP_2,  // モニター・テクスチャパターン・アニメーション2
  RES_MONITOR_NSBTP_3,  // モニター・テクスチャパターン・アニメーション3
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[RES_NUM] = 
{
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbtx, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_1_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_2_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_3_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_4_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_5_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_6_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_7_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv01_nsbtp, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv02_nsbtp, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv03_nsbtp, GFL_G3D_UTIL_RESARC },
};

// アニメインデックス
typedef enum
{
  ANM_ELBOARD_DATE,         // 掲示板ニュース・スクロール・日付
  ANM_ELBOARD_WEATHER,      // 掲示板ニュース・スクロール・天気
  ANM_ELBOARD_PROPAGATION,  // 掲示板ニュース・スクロール・大量発生
  ANM_ELBOARD_INFO_A,       // 掲示板ニュース・スクロール・情報A
  ANM_ELBOARD_INFO_B,       // 掲示板ニュース・スクロール・情報B
  ANM_ELBOARD_INFO_C,       // 掲示板ニュース・スクロール・情報C
  ANM_ELBOARD_CM,           // 掲示板ニュース・スクロール・一言CM
  ANM_MONITOR_1,            // モニター・テクスチャ・アニメーション1
  ANM_MONITOR_2,            // モニター・テクスチャ・アニメーション2
  ANM_MONITOR_3,            // モニター・テクスチャ・アニメーション3
  ANM_NUM
} ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table[ANM_NUM] = 
{
  // アニメリソースID, アニメデータID(リソース内部INDEX)
  { RES_ELBOARD_NSBTA_1, 0 },
  { RES_ELBOARD_NSBTA_2, 0 },
  { RES_ELBOARD_NSBTA_3, 0 },
  { RES_ELBOARD_NSBTA_4, 0 },
  { RES_ELBOARD_NSBTA_5, 0 },
  { RES_ELBOARD_NSBTA_6, 0 },
  { RES_ELBOARD_NSBTA_7, 0 },
  { RES_MONITOR_NSBTP_1, 0 },
  { RES_MONITOR_NSBTP_2, 0 },
  { RES_MONITOR_NSBTP_3, 0 },
};

// オブジェクトインデックス
typedef enum
{
  OBJ_ELBOARD,  // 電光掲示板
  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[OBJ_NUM] = 
{
  // モデルリソースID, 
  // モデルデータID(リソース内部INDEX), 
  // テクスチャリソースID,
  // アニメテーブル, 
  // アニメリソース数
  { RES_ELBOARD_NSBMD, 0, RES_ELBOARD_NSBTX, anm_table, ANM_NUM },
}; 

// ユニットインデックス
typedef enum
{
  UNIT_ELBOARD, // 電光掲示板 + モニター
  UNIT_NUM
} UNIT_INDEX;
static const GFL_G3D_UTIL_SETUP unit[UNIT_NUM] =
{
  { res_table, RES_NUM, obj_table, OBJ_NUM },
};


//==========================================================================================
// ■ニュースパラメータ
//==========================================================================================
// ニュースタイプ
typedef enum {
  NEWS_TYPE_NULL,         // 空
  NEWS_TYPE_DATE,         // 日付
  NEWS_TYPE_WEATHER,      // 天気
  NEWS_TYPE_PROPAGATION,  // 大量発生
  NEWS_TYPE_INFO_A,       // 地域情報A
  NEWS_TYPE_INFO_B,       // 地域情報B
  NEWS_TYPE_INFO_C,       // 地域情報C
  NEWS_TYPE_CM,           // 一言CM 
  NEWS_TYPE_SPECIAL,      // 臨時ニュース
  NEWS_TYPE_NUM,
  NEWS_TYPE_MAX = NEWS_TYPE_NUM - 1
} NEWS_TYPE;

// ニュース登録番号
typedef enum {
  NEWS_INDEX_DATE,         // 日付
  NEWS_INDEX_WEATHER,      // 天気
  NEWS_INDEX_PROPAGATION,  // 大量発生
  NEWS_INDEX_INFO_A,       // 地域情報A
  NEWS_INDEX_INFO_B,       // 地域情報B
  NEWS_INDEX_INFO_C,       // 地域情報C
  NEWS_INDEX_CM,           // 一言CM
  NEWS_INDEX_NUM,
  NEWS_INDEX_MAX = NEWS_INDEX_NUM -1
} NEWS_INDEX; 

// スクロール・アニメーション・インデックス
static u16 news_anm_index[NEWS_INDEX_NUM] = 
{
  ANM_ELBOARD_DATE,        // 日付
  ANM_ELBOARD_WEATHER,     // 天気
  ANM_ELBOARD_PROPAGATION, // 大量発生
  ANM_ELBOARD_INFO_A,      // 地域情報A
  ANM_ELBOARD_INFO_B,      // 地域情報B
  ANM_ELBOARD_INFO_C,      // 地域情報C
  ANM_ELBOARD_CM,          // 一言CM
};
// テクスチャ名
static char* news_tex_name[NEWS_INDEX_NUM] =
{
  "gelboard_1",
  "gelboard_2",
  "gelboard_3",
  "gelboard_4",
  "gelboard_5",
  "gelboard_6",
  "gelboard_7",
};
// パレット名
static char* news_plt_name[NEWS_INDEX_NUM] =
{
  "gelboard_1_pl",
  "gelboard_2_pl",
  "gelboard_3_pl",
  "gelboard_4_pl",
  "gelboard_5_pl",
  "gelboard_6_pl",
  "gelboard_7_pl",
};
// 天気に使用するメッセージ
u32 str_id_weather[WEATHER_NO_NUM] = 
{
  msg_gate_sunny,     // 晴れ
  msg_gate_snow,      // 雪
  msg_gate_rain,      // 雨
  msg_gate_storm,     // 砂嵐
  msg_gate_spark,     // 雷雨
  msg_gate_snowstorm, // 吹雪
  msg_gate_arare,     // 霰
  msg_gate_mirage,    // 蜃気楼
};

// ジム情報の登録場所
static const NEWS_INDEX gym_news_idx[GYM_NEWS_MAX_NUM] = 
{ 
  NEWS_INDEX_PROPAGATION, 
  NEWS_INDEX_INFO_A, 
  NEWS_INDEX_INFO_B, 
  NEWS_INDEX_INFO_C
};


//==========================================================================================
// ■モニターパラメータ
//==========================================================================================
// アニメーション番号
typedef enum {
  MONITOR_ANIME_1,
  MONITOR_ANIME_2,
  MONITOR_ANIME_3,
  MONITOR_ANIME_NUM,
  MONITOR_ANIME_MAX = MONITOR_ANIME_NUM - 1
} MONITOR_ANIME_INDEX;

// 実アニメーション番号
static u16 monitor_anime[MONITOR_ANIME_NUM] = 
{ 
  ANM_MONITOR_1,
  ANM_MONITOR_2,
  ANM_MONITOR_3,
};


//==========================================================================================
// ■ニュース登録状況
//==========================================================================================
typedef struct
{
  u8                          newsNum;  // 登録したニュースの数
  NEWS_TYPE  newsType[NEWS_INDEX_NUM];  // 登録したニュース
  u32      spNewsFlag[NEWS_INDEX_NUM];  // 登録した臨時ニュースに対応するフラグ

} NEWS_ENTRY_DATA;

//------------------------------------------------------------------------------------------
/**
 * @brief 登録状況を更新する
 *
 * @param data 追加先のデータ
 * @param type 追加するニュースのタイプ
 * @param flag 臨時ニュースの場合, 対応するフラグを指定
 */
//------------------------------------------------------------------------------------------
static void AddNewsEntryData( NEWS_ENTRY_DATA* data, NEWS_TYPE type, u32 flag )
{
  // すでに最大数が登録されている
  if( NEWS_INDEX_NUM <= data->newsNum ){ return; }

  // 登録
  data->newsType[ data->newsNum ]   = type;
  data->spNewsFlag[ data->newsNum ] = flag;
  data->newsNum++;
}


//==========================================================================================
// ■ギミック セーブワーク
//==========================================================================================
typedef struct
{
  void*                gateWork;  // ギミック管理ワークへのポインタ
  u32             recoveryFrame;  // 復帰フレーム
  NEWS_ENTRY_DATA newsEntryData;  // ニュース登録状況

} SAVEWORK;


//==========================================================================================
// ■ギミック管理ワーク
//==========================================================================================
typedef struct
{ 
  HEAPID                   heapID;  // 使用するヒープID
  FIELDMAP_WORK*         fieldmap;  // フィールドマップ
  GOBJ_ELBOARD*           elboard;  // 電光掲示板管理オブジェクト
  u32               recoveryFrame;  // 復帰フレーム
  ELBOARD_ZONE_DATA*     gateData;  // ゲートデータ
  ELBOARD_SPNEWS_DATA* spNewsData;  // 臨時ニュースデータ
  u8                spNewsDataNum;  // 臨時ニュースデータ数
  NEWS_ENTRY_DATA   newsEntryData;  // ニュース登録状況

} GATEWORK;


//==========================================================================================
// ■非公開関数のプロトタイプ宣言
//==========================================================================================
static void GimmickSave( const GATEWORK* work );
static void GimmickLoad( GATEWORK* work );
static GATEWORK* CreateGateWork( FIELDMAP_WORK* fieldmap );
static void DeleteGateWork( GATEWORK* work );
static BOOL LoadGateData( GATEWORK* work, FIELDMAP_WORK* fieldmap );
static void DeleteGateData( GATEWORK* work );
static void LoadSpNewsData( GATEWORK* work );
static void DeleteSpNewsData( GATEWORK* work );
static void SetElboardPos( GFL_G3D_OBJSTATUS* status, ELBOARD_ZONE_DATA* data );
static void SetupElboardNews( GATEWORK* work );
static void SetupElboardNews_Normal( GATEWORK* work );
static void SetupElboardNews_Special( GATEWORK* work );
static const ELBOARD_SPNEWS_DATA* SearchTopNews( GATEWORK* work );
static const ELBOARD_SPNEWS_DATA* SearchGymNews( GATEWORK* work );
static const ELBOARD_SPNEWS_DATA* SearchFlagNews( GATEWORK* work, u32 flag );
static BOOL CheckSpecialNews( GATEWORK* work );
static void EntryNews( GATEWORK* work, 
                       const NEWS_PARAM* news, NEWS_TYPE type, 
                       const ELBOARD_SPNEWS_DATA* sp_data );
static void AddNews_DATE( GATEWORK* work );
static void AddNews_PROPAGATION( GATEWORK* work );
static void AddNews_WEATHER( GATEWORK* work );
static void AddNews_INFO_A( GATEWORK* work );
static void AddNews_INFO_B( GATEWORK* work );
static void AddNews_INFO_C( GATEWORK* work );
static void AddNews_CM( GATEWORK* work );
static void AddNews_SPECIAL( GATEWORK* work, const ELBOARD_SPNEWS_DATA* news );
static void AddSpNews_DIRECT( GATEWORK* work,
                              const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx );
static void AddSpNews_CHAMP( GATEWORK* work,
                             const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx );
static void AddSpNews_GYM( GATEWORK* work );


//==========================================================================================
// ■ギミック登録関数
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief 初期化関数
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  GATEWORK*                work = NULL;  // GATEギミック管理ワーク
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // 拡張オブジェクトのユニットを追加
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &unit[UNIT_ELBOARD], EXPOBJ_UNIT_ELBOARD );

  // ギミック管理ワークを作成
  work = CreateGateWork( fieldmap );

  // ギミックのセーブデータを読み込む
  GimmickLoad( work );

  // ニュースセットアップ
  {
    int i;
    GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
    GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
    GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
    int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
    SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
    NEWS_ENTRY_DATA* data = &save_buf->newsEntryData;

    // フラグ復元
    for( i=0; i<data->newsNum; i++)
    {
      EVENTWORK* evwork = GAMEDATA_GetEventWork( gdata );
      if( data->newsType[i] == NEWS_TYPE_SPECIAL )
      {
        EVENTWORK_SetEventFlag( evwork, data->spNewsFlag[i] );
      }
    }
    // ニュースをセット
    SetupElboardNews( work ); 
    GOBJ_ELBOARD_SetFrame( work->elboard, work->recoveryFrame << FX32_SHIFT );
  }

  // DEBUG:
  OBATA_Printf( "GIMMICK-GATE: setup\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 終了関数
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;

  // ギミック状態をセーブ
  GimmickSave( work );
  
  // 電光掲示板管理ワークを破棄
  if( work->elboard )
  {
    GOBJ_ELBOARD_Delete( work->elboard );
  }

  // ギミック管理ワークを破棄
  DeleteGateWork( work );

  // DEBUG:
  OBATA_Printf( "GIMMICK-GATE: end\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 動作関数
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;

  // 電光掲示板メイン処理
  GOBJ_ELBOARD_Main( work->elboard, FX32_ONE ); 

  // モニターアニメーション再生
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
    FLD_EXP_OBJ_PlayAnime( exobj_cnt );
  }

#if 0
  // TEST:
  {
    int key = GFL_UI_KEY_GetCont();
    int trg = GFL_UI_KEY_GetTrg();
    if( key & PAD_BUTTON_L )
    {
      if( trg & PAD_BUTTON_A )
      { 
        GATE_GIMMICK_Camera_LookElboard( fieldmap, 30 );
      }
      if( trg & PAD_BUTTON_B )
      { 
        GATE_GIMMICK_Camera_Reset( fieldmap, 30 );
      }
    }
  }
#endif
}


//==========================================================================================
// ■ 掲示板
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板に平常通りのニュースを設定する
 *
 * @param fieldmap フィールドマップ
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Elboard_SetupNormalNews( FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;

  // 平常ニュースを構成
  SetupElboardNews_Normal( work );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板に指定したニュースを追加する
 *
 * @param fieldmap フィールドマップ
 * @param str_id   追加するニュースのメッセージ番号
 * @param wordset  指定メッセージに展開するワードセット
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Elboard_AddSpecialNews( FIELDMAP_WORK* fieldmap, 
                                          u32 str_id, WORDSET* wordset )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;

  // 電光掲示板が登録されていない場所で呼ばれたら何もしない
  if( IsGimmickIDEntried( gmk_id ) != TRUE ){ return; }

  // ニュースを追加
  {
    int index;
    NEWS_PARAM news;

    index = GOBJ_ELBOARD_GetNewsNum( work->elboard );
    if( MAX_NEWS_NUM <= index )
    {
      OBATA_Printf( "==========================================\n" );
      OBATA_Printf( "すでに最大数のニュースが設定されています。\n" );
      OBATA_Printf( "==========================================\n" );
      return;
    }
    news.animeIndex = news_anm_index[index];
    news.texName    = news_tex_name[index];
    news.pltName    = news_plt_name[index];
    news.msgArcID   = ARCID_MESSAGE;
    news.msgDatID   = NARC_message_gate_dat;
    news.msgStrID   = str_id;
    news.wordset    = wordset;
    GOBJ_ELBOARD_AddNews( work->elboard, &news );
  }

}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板の状態を復帰させる(復帰ポイントは自動的に記憶)
 *
 * @param fieldmap フィールドマップ
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Elboard_Recovery( FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;
  NEWS_ENTRY_DATA* data = &save_buf->newsEntryData;

  // 電光掲示板が登録されていない場所で呼ばれたら何もしない
  if( IsGimmickIDEntried( gmk_id ) != TRUE )
  {
    return;
  } 

  // フラグ復元
  for( i=0; i<data->newsNum; i++)
  {
    EVENTWORK* evwork = GAMEDATA_GetEventWork( gdata );
    if( data->newsType[i] == NEWS_TYPE_SPECIAL )
    {
      EVENTWORK_SetEventFlag( evwork, data->spNewsFlag[i] );
    }
  }
  // 再セットアップ
  SetupElboardNews( work );
#if 0
  // ニュース復帰
  for( i=0; i<data->newsNum; i++ )
  {
    switch( data->newsType[i] )
    {
    default:                    break;
    case NEWS_TYPE_NULL:        break;
    case NEWS_TYPE_DATE:        AddNews_DATE(work);         break;
    case NEWS_TYPE_WEATHER:     AddNews_WEATHER(work);      break;
    case NEWS_TYPE_PROPAGATION: AddNews_PROPAGATION(work);  break;
    case NEWS_TYPE_INFO_A:      AddNews_INFO_A(work);       break;
    case NEWS_TYPE_INFO_B:      AddNews_INFO_B(work);       break;
    case NEWS_TYPE_INFO_C:      AddNews_INFO_C(work);       break;
    case NEWS_TYPE_CM:          AddNews_CM(work);           break;
    case NEWS_TYPE_SPECIAL:     // 臨時ニュース
                                {
                                  u32 flag;
                                  const ELBOARD_SPNEWS_DATA* news;
                                  flag = data->spNewsFlag[i];
                                  news = SearchFlagNews( work, flag );
                                  AddNews_SPECIAL( work, news );
                                }
                                break;
    }
  } 
#endif
  // 掲示板復帰処理
  GOBJ_ELBOARD_SetFrame( work->elboard, work->recoveryFrame << FX32_SHIFT );
}


//==========================================================================================
// ■カメラ
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief カメラを電光掲示板に向ける
 *
 * @param fieldmap フィールドマップ
 * @param frame    カメラの動作に要するフレーム数
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Camera_LookElboard( FIELDMAP_WORK* fieldmap, u16 frame )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;
  fx32 val_len;
  u16 val_pitch, val_yaw;
  VecFx32 val_target;

  // 電光掲示板データを取得
  if( !work->gateData )
  { // 取得失敗
    OBATA_Printf( "==========================================\n" );
    OBATA_Printf( "GIMMICK-GATE: 電光掲示板データの取得に失敗\n" );
    OBATA_Printf( "==========================================\n" );
    return;
  }
  // 電光掲示板の向きでカメラの回転を決定
  switch( work->gateData->dir )
  {
  case DIR_DOWN:
    val_pitch = 0x0ee5;
    val_yaw   = 0;
    val_len   = 0x0086 << FX32_SHIFT;
    VEC_Set( &val_target, 0, 0x001b<<FX32_SHIFT, 0xfff94000 );
    break;
  case DIR_RIGHT: 
    val_pitch = 0x1ad3;
    val_yaw   = 0x3f5d;
    val_len   = 0x0058 << FX32_SHIFT;
    VEC_Set( &val_target, 0xfff5d000, 0x001a<<FX32_SHIFT, 0xfffff000 );
    break;
  case DIR_UP:
  case DIR_LEFT:
  default:
    OBATA_Printf( "==================================\n" );
    OBATA_Printf( "GIMMICK-GATE: 掲示板の向きが未対応\n" );
    OBATA_Printf( "==================================\n" );
    return;
  } 
  // タスク登録
  {
    FIELD_TASK_MAN* man;
    FIELD_TASK* zoom;
    FIELD_TASK* pitch;
    FIELD_TASK* yaw;
    FIELD_TASK* target;
    zoom   = FIELD_TASK_CameraLinearZoom( fieldmap, frame, val_len );
    pitch  = FIELD_TASK_CameraRot_Pitch( fieldmap, frame, val_pitch );
    yaw    = FIELD_TASK_CameraRot_Yaw( fieldmap, frame, val_yaw );
    target = FIELD_TASK_CameraTargetOffset( fieldmap, frame, &val_target );
    man = FIELDMAP_GetTaskManager( fieldmap );
    FIELD_TASK_MAN_AddTask( man, zoom, NULL );
    FIELD_TASK_MAN_AddTask( man, pitch, NULL );
    FIELD_TASK_MAN_AddTask( man, yaw, NULL );
    FIELD_TASK_MAN_AddTask( man, target, NULL );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief カメラを元に戻す
 *
 * @param fieldmap フィールドマップ
 * @param frame    カメラの動作に要するフレーム数
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Camera_Reset( FIELDMAP_WORK* fieldmap, u16 frame )
{
  fx32 val_len;
  u16 val_pitch, val_yaw;
  VecFx32 val_target = {0, 0, 0};
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );
  FLD_CAMERA_PARAM def_param;
  // デフォルトパラメータ取得
  FIELD_CAMERA_GetInitialParameter( camera, &def_param );
  val_len   = def_param.Distance << FX32_SHIFT;
  val_pitch = def_param.Angle.x;
  val_yaw   = def_param.Angle.y;
  // タスク登録
 {
    FIELD_TASK_MAN* man;
    FIELD_TASK* zoom;
    FIELD_TASK* pitch;
    FIELD_TASK* yaw;
    FIELD_TASK* target;
    zoom   = FIELD_TASK_CameraLinearZoom( fieldmap, frame, val_len );
    pitch  = FIELD_TASK_CameraRot_Pitch( fieldmap, frame, val_pitch );
    yaw    = FIELD_TASK_CameraRot_Yaw( fieldmap, frame, val_yaw );
    target = FIELD_TASK_CameraTargetOffset( fieldmap, frame, &val_target );
    man = FIELDMAP_GetTaskManager( fieldmap );
    FIELD_TASK_MAN_AddTask( man, zoom, NULL );
    FIELD_TASK_MAN_AddTask( man, pitch, NULL );
    FIELD_TASK_MAN_AddTask( man, yaw, NULL );
    FIELD_TASK_MAN_AddTask( man, target, NULL );
  }
}


//==========================================================================================
// ■ 非公開関数
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief ギミック状態を保存する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void GimmickSave( const GATEWORK* work )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*       gwork = (GATEWORK*)save_buf->gateWork;

  // 掲示板の動作フレーム数を記憶
  save_buf->gateWork      = (void*)work;
  save_buf->recoveryFrame = GOBJ_ELBOARD_GetFrame( gwork->elboard ) >> FX32_SHIFT; 
  save_buf->newsEntryData = gwork->newsEntryData;

  // DEBUG:
  {
    int i;
    OBATA_Printf( "GIMMICK-GATE: gimmick save\n" );
    OBATA_Printf( "-recoveryFrame = %d\n", save_buf->recoveryFrame );
    OBATA_Printf( "-newsEntryData.newsNum = %d\n",  save_buf->newsEntryData.newsNum );
    for( i=0; i<NEWS_INDEX_NUM; i++ )
    {
      OBATA_Printf( "-newsEntryData.newsType[%d] = ", i );
      switch( save_buf->newsEntryData.newsType[i] )
      {
      case NEWS_TYPE_NULL:        OBATA_Printf( "NULL\n" );        break;
      case NEWS_TYPE_DATE:        OBATA_Printf( "DATE\n" );        break;
      case NEWS_TYPE_WEATHER:     OBATA_Printf( "WEATHER\n" );     break;
      case NEWS_TYPE_PROPAGATION: OBATA_Printf( "PROPAGATION\n" ); break;
      case NEWS_TYPE_INFO_A:      OBATA_Printf( "INFO_A\n" );      break;
      case NEWS_TYPE_INFO_B:      OBATA_Printf( "INFO_B\n" );      break;
      case NEWS_TYPE_INFO_C:      OBATA_Printf( "INFO_C\n" );      break;
      case NEWS_TYPE_CM:          OBATA_Printf( "CM\n" );          break;
      case NEWS_TYPE_SPECIAL:     OBATA_Printf( "SPECIAL\n" );     break;
      default:                    OBATA_Printf( "UNKNOWN\n" );     break;
      }
    }
    for( i=0; i<NEWS_INDEX_NUM; i++ )
    {
      OBATA_Printf( "-newsEntryData.spNewsFlag[%d] = %d\n",
                    i, save_buf->newsEntryData.spNewsFlag[i] );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief ギミックのセーブデータを読み込む
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void GimmickLoad( GATEWORK* work )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = NULL;

  // セーブデータ取得
  save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  work->recoveryFrame = save_buf->recoveryFrame;  // 掲示板の復帰ポイント

  // ギミック管理ワークのアドレスを記憶
  save_buf->gateWork = work; 

  // DEBUG:
  {
    int i;
    OBATA_Printf( "GIMMICK-GATE: gimmick load\n" );
    OBATA_Printf( "-recoveryFrame = %d\n", save_buf->recoveryFrame );
    OBATA_Printf( "-newsEntryData.newsNum = %d\n",  save_buf->newsEntryData.newsNum );
    for( i=0; i<NEWS_INDEX_NUM; i++ )
    {
      OBATA_Printf( "-newsEntryData.newsType[%d] = ", i );
      switch( save_buf->newsEntryData.newsType[i] )
      {
      case NEWS_TYPE_NULL:        OBATA_Printf( "NULL\n" );        break;
      case NEWS_TYPE_DATE:        OBATA_Printf( "DATE\n" );        break;
      case NEWS_TYPE_WEATHER:     OBATA_Printf( "WEATHER\n" );     break;
      case NEWS_TYPE_PROPAGATION: OBATA_Printf( "PROPAGATION\n" ); break;
      case NEWS_TYPE_INFO_A:      OBATA_Printf( "INFO_A\n" );      break;
      case NEWS_TYPE_INFO_B:      OBATA_Printf( "INFO_B\n" );      break;
      case NEWS_TYPE_INFO_C:      OBATA_Printf( "INFO_C\n" );      break;
      case NEWS_TYPE_CM:          OBATA_Printf( "CM\n" );          break;
      case NEWS_TYPE_SPECIAL:     OBATA_Printf( "SPECIAL\n" );     break;
      default:                    OBATA_Printf( "UNKNOWN\n" );     break;
      }
    }
    for( i=0; i<NEWS_INDEX_NUM; i++ )
    {
      OBATA_Printf( "-newsEntryData.spNewsFlag[%d] = %d\n",
                    i, save_buf->newsEntryData.spNewsFlag[i] );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief ギミック管理ワークを作成する
 *
 * @param fieldmap 依存するフィールドマップ
 *
 * @return 作成したギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static GATEWORK* CreateGateWork( FIELDMAP_WORK* fieldmap )
{
  GATEWORK*                work = NULL;
  HEAPID                heap_id = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // ギミック管理ワークを作成
  work = (GATEWORK*)GFL_HEAP_AllocMemory( heap_id, sizeof(GATEWORK) );
  GFL_STD_MemClear( work, sizeof(GATEWORK) );

  // 初期化
  work->heapID   = heap_id;
  work->fieldmap = fieldmap;

  // 電光掲示板管理ワークを作成
  {
    ELBOARD_PARAM param;
    param.heapID       = heap_id;
    param.maxNewsNum   = NEWS_INDEX_NUM;
    param.dispSize     = DISPLAY_SIZE;
    param.newsInterval = NEWS_INTERVAL;
    param.g3dObj       = FLD_EXP_OBJ_GetUnitObj( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
    work->elboard      = GOBJ_ELBOARD_Create( &param );
  } 

  // 電光掲示板データを取得
  if( !LoadGateData( work, fieldmap ) )
  {
    // データが取得できなかったら, 以降の初期化を中断
    return work;
  }
  // 臨時ニュースデータを取得
  LoadSpNewsData( work );

  // 電光掲示板を配置
  { 
    GFL_G3D_OBJSTATUS* status = 
      FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
    SetElboardPos( status, work->gateData );
  } 

  // モニター・アニメーション開始
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD, 
                           monitor_anime[work->gateData->monitorAnimeIndex], TRUE ); 

  return work;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ギミック管理ワークを破棄する
 *
 * @param work 破棄するワーク
 */
//------------------------------------------------------------------------------------------
static void DeleteGateWork( GATEWORK* work )
{
  DeleteGateData( work );      // ゲートデータ
  DeleteSpNewsData( work );    // 臨時ニュースデータ
  GFL_HEAP_FreeMemory( work ); // 本体
}

//------------------------------------------------------------------------------------------
/**
 * @brief ゾーンに応じた電光掲示板データを取得する
 *
 * @param work     読み込んだデータを保持するワーク
 * @param fieldmap フィールドマップ
 *
 * @return データが正しく読み込めたら TRUE
 */
//------------------------------------------------------------------------------------------
static BOOL LoadGateData( GATEWORK* work, FIELDMAP_WORK* fieldmap )
{
  int i;
  u16 zone_id;

  // すでに読み込まれている
  if( work->gateData ){ return FALSE; }

  // ゾーンIDを取得
  zone_id = FIELDMAP_GetZoneID( fieldmap );

  // 電光掲示板データテーブルから, 該当するデータを検索
  for( i=0; i<NELEMS(entry_table); i++ )
  {
    if( entry_table[i].zone_id == zone_id )
    {
      // 電光掲示板データを読み込む
      work->gateData = GFL_HEAP_AllocMemory( work->heapID, sizeof(ELBOARD_ZONE_DATA) );
      ELBOARD_ZONE_DATA_Load( work->gateData, ARCID_GATE_GIMMICK, entry_table[i].dat_id );
      return TRUE;
    }
  } 

  // 現在のゾーンに対応する電光掲示板データが登録されていない場合
  OBATA_Printf( "=============================================\n" );
  OBATA_Printf( "error: 電光掲示板データが登録されていません。\n" );
  OBATA_Printf( "=============================================\n" );
  return FALSE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ゲートデータを破棄する
 *
 * @param work 破棄するゲートデータを保持するワーク
 */
//------------------------------------------------------------------------------------------
static void DeleteGateData( GATEWORK* work )
{
  if( work->gateData )
  {
    GFL_HEAP_FreeMemory( work->gateData );
    work->gateData = NULL;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 臨時ニュースデータを読み込む
 *
 * @param work 読み込んだデータを保持するワーク
 */
//------------------------------------------------------------------------------------------
static void LoadSpNewsData( GATEWORK* work )
{

  // すでに読み込まれている
  if( work->spNewsData ){ return; }

  // 全データを読み込み
  {
    int data_idx;
    int data_num;

    // データ数取得
    data_num = GFL_ARC_GetDataFileCnt( ARCID_ELBOARD_SPNEWS );

    // 各データを取得
    work->spNewsData = GFL_HEAP_AllocMemory( work->heapID, 
                                             sizeof(ELBOARD_SPNEWS_DATA) * data_num );
    for( data_idx=0; data_idx<data_num; data_idx++ )
    {
      ELBOARD_SPNEWS_DATA_Load( &work->spNewsData[data_idx], ARCID_ELBOARD_SPNEWS, data_idx );
    }
    work->spNewsDataNum = data_num;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 臨時ニュースデータを破棄する
 *
 * @param 破棄するデータを保持するワーク
 */
//------------------------------------------------------------------------------------------
static void DeleteSpNewsData( GATEWORK* work )
{
  if( work->spNewsData )
  {
    GFL_HEAP_FreeMemory( work->spNewsData );
    work->spNewsData = NULL;
    work->spNewsDataNum = 0;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 電光掲示板を配置する
 *
 * @param status 電光掲示板のオブジェステータス
 * @param data   電光掲示板データ
 */
//------------------------------------------------------------------------------------------
static void SetElboardPos( GFL_G3D_OBJSTATUS* status, ELBOARD_ZONE_DATA* data )
{
  u16 rot;

  // 座標を設定
  status->trans.x = data->x << FX32_SHIFT;
  status->trans.y = data->y << FX32_SHIFT;
  status->trans.z = data->z << FX32_SHIFT;

  // 向きを設定
  switch( data->dir )
  {
  case DIR_DOWN:  rot = 0;    break;
  case DIR_RIGHT: rot = 90;   break;
  case DIR_UP:    rot = 180;  break;
  case DIR_LEFT:  rot = 270;  break;
  default:        rot = 0;    break;
  }
  rot *= 182;  // 65536÷360 = 182.044...
  GFL_CALC3D_MTX_CreateRot( 0, rot, 0, &status->rotate );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 電光掲示板 ニュースセットアップ
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews( GATEWORK* work )
{
  if( CheckSpecialNews(work) )  // if(臨時ニュース有)
  { // 臨時ニュース
    SetupElboardNews_Special( work ); 
  }
  else
  { // 平常ニュース
    SetupElboardNews_Normal( work );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板に表示する情報を 平常ニュースで構成する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews_Normal( GATEWORK* work )
{
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );

  // すでに他のニュースが登録されている場合
  {
    int num = GOBJ_ELBOARD_GetNewsNum( work->elboard );
    if( num != 0 )
    {
      OBATA_Printf( "======================================\n" );
      OBATA_Printf( "すでに他のニュースが設定されています。\n" );
      OBATA_Printf( "======================================\n" );
      return;
    }
  } 
  // データを持っていない
  if( !work->gateData ){ return; }

  // ニュースを追加
  AddNews_DATE( work );         // 日付
  AddNews_WEATHER( work );      // 天気
  AddNews_PROPAGATION( work );  // 大量発生
  AddNews_INFO_A( work );       // 地域情報A
  AddNews_INFO_B( work );       // 地域情報B
  AddNews_INFO_C( work );       // 地域情報C
  AddNews_CM( work );           // 一言CM
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板に表示する情報を 臨時ニュースで構成する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews_Special( GATEWORK* work )
{ 
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );

  // すでに他のニュースが登録されている場合
  {
    int num = GOBJ_ELBOARD_GetNewsNum( work->elboard );
    if( num != 0 )
    {
      OBATA_Printf( "======================================\n" );
      OBATA_Printf( "すでに他のニュースが設定されています。\n" );
      OBATA_Printf( "======================================\n" );
      return;
    }
  } 
  // データを持っていない
  if( !work->spNewsData ){ return; }

  // ニュースを追加
  AddNews_DATE( work );         // 日付
  AddNews_WEATHER( work );      // 天気
  AddNews_PROPAGATION( work );  // 大量発生
  { // 臨時ニュース
    const ELBOARD_SPNEWS_DATA* news;
    news = SearchTopNews( work );
    AddNews_SPECIAL( work, news );
  }
  AddNews_CM( work );           // 一言CM
}

//------------------------------------------------------------------------------------------
/**
 * @brief 表示すべきニュースの中で, 最も優先順位の高い臨時ニュースを検索する
 *
 * @param work ギミック管理ワーク
 *
 * @return 最も優先順位の高い, 表示すべき臨時ニュース
 *         表示すべきニュースがない場合 NULL
 */
//------------------------------------------------------------------------------------------
static const ELBOARD_SPNEWS_DATA* SearchTopNews( GATEWORK* work )
{
  int i;
  u32        zone_id = FIELDMAP_GetZoneID( work->fieldmap );
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
  EVENTWORK*  evwork = GAMEDATA_GetEventWork( gdata );

  // データを持っていない
  if( !work->spNewsData ){ return NULL; }

  // 臨時ニュースデータを検索
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    BOOL flag_hit = EVENTWORK_CheckEventFlag( evwork, work->spNewsData[i].flag );
    BOOL zone_hit = ELBOARD_SPNEWS_DATA_CheckZoneHit( &work->spNewsData[i], zone_id );
    if( flag_hit && zone_hit )  // if(フラグON && ゾーン一致)
    {
      return &work->spNewsData[i];
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------------------
/**
 * @brief 表示すべきニュースの中で, 最も優先順位の高い臨時ジムニュースを検索する
 *
 * @param work ギミック管理ワーク
 *
 * @return 最も優先順位の高い, 表示すべき臨時ニュース
 *         表示すべきニュースがない場合 NULL
 */
//------------------------------------------------------------------------------------------
static const ELBOARD_SPNEWS_DATA* SearchGymNews( GATEWORK* work )
{
  int i;
  u32        zone_id = FIELDMAP_GetZoneID( work->fieldmap );
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
  EVENTWORK*  evwork = GAMEDATA_GetEventWork( gdata );

  // データを持っていない
  if( !work->spNewsData ){ return NULL; }

  // 臨時ニュースデータを検索
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    BOOL flag_hit = EVENTWORK_CheckEventFlag( evwork, work->spNewsData[i].flag );
    BOOL zone_hit = ELBOARD_SPNEWS_DATA_CheckZoneHit( &work->spNewsData[i], zone_id );
    if( flag_hit && zone_hit )  // if(フラグON && ゾーン一致)
    {
      if( work->spNewsData[i].newsType == SPNEWS_TYPE_GYM )  // if(ジムニュース)
      {
        return &work->spNewsData[i];
      }
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------------------
/**
 * @brief 指定したフラグに対応するニュースを検索する
 *
 * @param work ギミック管理ワーク
 * @param flag 検索するフラグ
 *
 * @return 指定したフラグに対応するニュース
 *         ニュースがない場合 NULL
 */
//------------------------------------------------------------------------------------------
static const ELBOARD_SPNEWS_DATA* SearchFlagNews( GATEWORK* work, u32 flag )
{
  int i;

  // データを持っていない
  if( !work->spNewsData ){ return NULL; }

  // 臨時ニュースデータを検索
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    if( work->spNewsData[i].flag == flag )
    {
      return &work->spNewsData[i];
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------------------
/**
 * @brief 臨時ニュースの有無を調べる
 *
 * @param work ギミック管理ワーク
 *
 * @return 表示すべき臨時ニュースがある場合 TRUE, そうでなければ FALSE
 */
//------------------------------------------------------------------------------------------
static BOOL CheckSpecialNews( GATEWORK* work )
{
  return (SearchTopNews(work) != NULL);
}

//------------------------------------------------------------------------------------------
/**
 * @brief ニュースを登録する
 *
 * @param work    ギミック管理ワーク
 * @param news    登録するニュースパラメータ
 * @param type    登録するニュースのタイプ
 * @param sp_data 登録する臨時ニュースのデータ( 平常ニュースの場合はNULL )
 *
 * ※ニュースの追加は, 必ずこの関数を介して行う。
 *  �@掲示板へニュースを追加
 *  �Aニュース登録状況の更新
 *  �Bフラグ操作(臨時ニュースの場合)
 */
//------------------------------------------------------------------------------------------
static void EntryNews( GATEWORK* work, 
                       const NEWS_PARAM* news, NEWS_TYPE type, 
                       const ELBOARD_SPNEWS_DATA* sp_data )
{
  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, news );

  // 登録状況を更新
  {
    u32 flag = 0;
    if( type == NEWS_TYPE_SPECIAL ){ flag = sp_data->flag; }
    AddNewsEntryData( &work->newsEntryData, type, flag );
  }

  // フラグ操作
  // 臨時ニュースを追加した場合, そのニュースに設定されたフラグ操作を行う.
  if( type == NEWS_TYPE_SPECIAL )
  {
    // フラグを落とす
    if( sp_data->flagControl == FLAG_CONTROL_RESET )
    { 
      GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
      GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
      EVENTWORK*  evwork = GAMEDATA_GetEventWork( gdata ); 
      EVENTWORK_ResetEventFlag( evwork, sp_data->flag );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のニュースを追加する(日付)
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void AddNews_DATE( GATEWORK* work )
{
  NEWS_PARAM news;
  WORDSET* wordset;
  RTCDate date;

  // ワードセット作成
  wordset = WORDSET_Create( work->heapID );
  RTC_GetDate( &date );
  WORDSET_RegisterNumber( wordset, 0, date.month, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterNumber( wordset, 1, date.day,   2, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );

  // ニュースパラメータを作成
  news.animeIndex = news_anm_index[NEWS_INDEX_DATE];
  news.texName    = news_tex_name[NEWS_INDEX_DATE];
  news.pltName    = news_plt_name[NEWS_INDEX_DATE];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = work->gateData->msgID_date;
  news.wordset    = wordset;

  // ニュース登録
  EntryNews( work, &news, NEWS_TYPE_DATE, NULL ); 

  // ワードセット破棄
  WORDSET_Delete( wordset );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のニュースを追加する(天気)
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void AddNews_WEATHER( GATEWORK* work )
{
  int i;
  NEWS_PARAM news;
  WORDSET* wordset;
  u32 zone_id[WEATHER_ZONE_NUM];

  // 表示するゾーンリストを作成
  zone_id[0] = work->gateData->zoneID_weather_1;
  zone_id[1] = work->gateData->zoneID_weather_2;
  zone_id[2] = work->gateData->zoneID_weather_3;
  zone_id[3] = work->gateData->zoneID_weather_4;

  // ワードセット作成
  wordset = WORDSET_CreateEx( WEATHER_ZONE_NUM, 256, work->heapID );

  // ワードセット登録処理
  {
    GFL_MSGDATA* msg_place_name;
    GFL_MSGDATA* msg_gate;

    // メッセージデータ ハンドルオープン
    msg_place_name = GFL_MSG_Create( 
        GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_place_name_dat, work->heapID ); 
    msg_gate = GFL_MSG_Create( 
        GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_gate_dat, work->heapID ); 

    // 各ゾーンの地名＋天気を登録
    for( i=0; i<WEATHER_ZONE_NUM; i++ )
    {
      STRBUF* strbuf_zone;    // 地名
      STRBUF* strbuf_weather; // 天気
      STRBUF* strbuf_set;     // 地名＋天気

      // ゾーンが無効値なら表示しない
      if( zone_id[i] == ZONE_ID_MAX ) 
      {
        continue;
      }
      // 地名を取得
      {
        int str_id = ZONEDATA_GetPlaceNameID( zone_id[i] );
        strbuf_zone = GFL_MSG_CreateString( msg_place_name, str_id );
      }
      // 天気を取得
      {
        int weather = FIELDMAP_GetZoneWeatherID( work->fieldmap, zone_id[i] );
        strbuf_weather = GFL_MSG_CreateString( msg_gate, str_id_weather[weather] );
      }
      // 地名＋天気のセットを作成
      {
        WORDSET* wordset_expand = WORDSET_CreateEx( 2, 256, work->heapID );
        STRBUF* strbuf_expand = GFL_MSG_CreateString( msg_gate, msg_gate_weather );
        strbuf_set = GFL_STR_CreateBuffer( 64, work->heapID );
        WORDSET_RegisterWord( wordset_expand, 0, strbuf_zone, 0, TRUE, 0 );
        WORDSET_RegisterWord( wordset_expand, 1, strbuf_weather, 0, TRUE, 0 );
        WORDSET_ExpandStr( wordset_expand, strbuf_set, strbuf_expand );
        GFL_STR_DeleteBuffer( strbuf_expand );
        WORDSET_Delete( wordset_expand );
      }
      // 作成した文字列をワードセットに登録
      WORDSET_RegisterWord( wordset, i, strbuf_set, 0, TRUE, 0 );
      // 後始末
      GFL_STR_DeleteBuffer( strbuf_zone );
      GFL_STR_DeleteBuffer( strbuf_weather );
      GFL_STR_DeleteBuffer( strbuf_set );
    }

    // メッセージデータ ハンドルクローズ
    GFL_MSG_Delete( msg_gate );
    GFL_MSG_Delete( msg_place_name );
  }

  // ニュースパラメータを作成
  news.animeIndex = news_anm_index[NEWS_INDEX_WEATHER];
  news.texName    = news_tex_name[NEWS_INDEX_WEATHER];
  news.pltName    = news_plt_name[NEWS_INDEX_WEATHER];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = work->gateData->msgID_weather;
  news.wordset    = wordset;

  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // ワードセット破棄
  WORDSET_Delete( wordset );

  // 登録状況を更新
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_WEATHER, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のニュースを追加する(大量発生)
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void AddNews_PROPAGATION( GATEWORK* work )
{
  GAMESYS_WORK* gsys;
  GAMEDATA* gdata;
  NEWS_PARAM news;
  WORDSET* wordset;
  u16 zone_id;

  // 大量発生が起きているゾーンを取得
  gsys    = FIELDMAP_GetGameSysWork( work->fieldmap );
  gdata   = GAMESYSTEM_GetGameData( gsys );
  zone_id = ENCPOKE_GetGenerateZone( gdata ); 
  // 大量発生が起きていない
  if( zone_id == 0xFFFF ){ return; }

  // ワードセット作成
  wordset = WORDSET_Create( work->heapID );

  // ワードセットに地名をセット
  {
    GFL_MSGDATA* msg_place_name;
    int str_id;
    STRBUF* zone_name;

    msg_place_name = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
                                     ARCID_MESSAGE, NARC_message_place_name_dat, work->heapID ); 
    str_id         = ZONEDATA_GetPlaceNameID( zone_id );
    zone_name      = GFL_MSG_CreateString( msg_place_name, str_id );
    WORDSET_RegisterWord( wordset, 0, zone_name, 0, TRUE, 0 );
    GFL_STR_DeleteBuffer( zone_name );
    GFL_MSG_Delete( msg_place_name );
  }

  // ニュースパラメータを作成
  news.animeIndex = news_anm_index[NEWS_INDEX_PROPAGATION];
  news.texName    = news_tex_name[NEWS_INDEX_PROPAGATION];
  news.pltName    = news_plt_name[NEWS_INDEX_PROPAGATION];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = work->gateData->msgID_propagation;
  news.wordset    = wordset;

  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // ワードセット破棄
  WORDSET_Delete( wordset );

  // 登録状況を更新
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_PROPAGATION, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のニュースを追加する(地域情報A)
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO_A( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // 日時データを取得
  RTC_GetDate( &date ); 

  // 曜日に応じた情報を選択
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_infoG;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_infoD;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_infoA;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_infoE;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_infoB;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_infoF;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_infoC;  break;
  }

  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_A];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_A];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_A];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;

  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // 登録状況を更新
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_INFO_A, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のニュースを追加する(地域情報B)
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO_B( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // 日時データを取得
  RTC_GetDate( &date ); 

  // 曜日に応じた情報を選択
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_infoH;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_infoE;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_infoB;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_infoF;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_infoC;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_infoG;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_infoD;  break;
  }

  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_B];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_B];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_B];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;

  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // 登録状況を更新
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_INFO_B, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のニュースを追加する(地域情報C)
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO_C( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // 日時データを取得
  RTC_GetDate( &date ); 

  // 曜日に応じた情報を選択
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_infoI;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_infoF;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_infoC;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_infoG;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_infoD;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_infoH;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_infoE;  break;
  }

  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_C];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_C];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_C];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;

  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // 登録状況を更新
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_INFO_C, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のニュースを追加する(一言CM)
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void AddNews_CM( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // 日時データを取得
  RTC_GetDate( &date );

  // 曜日に応じたCMを選択
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_cmSun;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_cmMon;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_cmTue;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_cmWed;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_cmThu;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_cmFri;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_cmSat;  break;
  }
  news.animeIndex = news_anm_index[NEWS_INDEX_CM];
  news.texName    = news_tex_name[NEWS_INDEX_CM];
  news.pltName    = news_plt_name[NEWS_INDEX_CM];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;
  
  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // 登録状況を更新
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_CM, 0 );
} 

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のニュースを追加する(臨時ニュース)
 * 
 * @param work   ギミック管理ワーク
 * @param news   追加する臨時ニュースデータ
 */
//------------------------------------------------------------------------------------------
static void AddNews_SPECIAL( GATEWORK* work, const ELBOARD_SPNEWS_DATA* news )
{ 
  // ニュースを追加
  switch( news->newsType )
  {
  case SPNEWS_TYPE_DIRECT: AddSpNews_DIRECT( work, news, NEWS_INDEX_INFO_A );  break;
  case SPNEWS_TYPE_CHAMP:  AddSpNews_CHAMP( work, news, NEWS_INDEX_INFO_A );   break;
  case SPNEWS_TYPE_GYM:    AddSpNews_GYM( work );                              break;
  }
} 

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板の臨時ニュースを追加する(メッセージそのまま)
 *
 * @param work     ギミック管理ワーク
 * @param spnews   臨時ニュースデータ
 * @param news_idx ニュースの追加場所を指定
 */
//------------------------------------------------------------------------------------------
static void AddSpNews_DIRECT( GATEWORK* work, 
                              const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx )
{
  NEWS_PARAM news;

  news.animeIndex = news_anm_index[news_idx];
  news.texName    = news_tex_name[news_idx];
  news.pltName    = news_plt_name[news_idx];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = spnews->msgID;
  news.wordset    = NULL;
  
  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // フラグ操作
  if( spnews->flagControl == FLAG_CONTROL_RESET )
  { // フラグを落とす
    GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
    GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
    EVENTWORK*  evwork = GAMEDATA_GetEventWork( gdata ); 
    EVENTWORK_ResetEventFlag( evwork, spnews->flag );
  }

  // 登録状況を更新
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_SPECIAL, spnews->flag );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板の臨時ニュースを追加する(チャンピオン情報)
 *
 * @param work     ギミック管理ワーク
 * @param spnews   臨時ニュースデータ
 * @param news_idx ニュースの追加場所を指定
 *
 * @todo ポケモン名を正しくセットする
 */
//------------------------------------------------------------------------------------------
static void AddSpNews_CHAMP( GATEWORK* work,
                             const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx )
{
  HEAPID heap_id; 
  NEWS_PARAM news;
  WORDSET* wordset;

  // ワードセット作成
  heap_id = GOBJ_ELBOARD_GetHeapID( work->elboard );
  wordset = WORDSET_Create( heap_id );

  // ポケモン名をセット
  WORDSET_RegisterPokeMonsNameNo( wordset, 0, 1 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 1, 2 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 2, 3 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 3, 4 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 4, 5 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 5, 6 );

  // ニュースパラメータを作成
  news.animeIndex = news_anm_index[news_idx];
  news.texName    = news_tex_name[news_idx];
  news.pltName    = news_plt_name[news_idx];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = spnews->msgID;
  news.wordset    = wordset;

  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // ワードセット破棄
  WORDSET_Delete( wordset );

  // フラグ操作
  if( spnews->flagControl == FLAG_CONTROL_RESET )
  { // フラグを落とす
    GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
    GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
    EVENTWORK*  evwork = GAMEDATA_GetEventWork( gdata ); 
    EVENTWORK_ResetEventFlag( evwork, spnews->flag );
  }

  // 登録状況を更新
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_SPECIAL, spnews->flag );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板の臨時ニュースを追加する(ジム情報)
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void AddSpNews_GYM( GATEWORK* work )
{
  // ニュース追加場所
  const ELBOARD_SPNEWS_DATA* news;
  int count = 0;

  // 全てのジムニュースを表示する
  news = SearchGymNews( work ); 
  while( news && count<NELEMS(gym_news_idx) )
  {
    // ニュースを追加
    AddSpNews_DIRECT( work, news, gym_news_idx[count++] );

    // 次のジムニュースを取得
    news = SearchGymNews( work );
  } 
}
