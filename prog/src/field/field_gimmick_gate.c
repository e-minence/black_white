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

#include "field/gimmick_obj_elboard.h"
#include "field_gimmick_def.h"
#include "field/zonedata.h"
#include "field/enc_pokeset.h"  // for ENCPOKE_GetGenerateZone
#include "gamesystem/pm_season.h" // for PMSEASON_xxxx

#include "savedata/gimmickwork.h"
#include "savedata/misc.h" // for MISC_xxxx
#include "savedata/save_tbl.h"
#include "savedata/dendou_save.h"
#include "savedata/config.h"

#include "gmk_tmp_wk.h"
#include "gimmick_obj_elboard.h"
#include "elboard_zone_data.h"
#include "elboard_spnews_data.h" 
#include "field_task_manager.h"
#include "field_task.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"
#include "move_pokemon.h"

#include "arc/arc_def.h" 
#include "arc/gate.naix"
#include "arc/message.naix" 
#include "msg/msg_gate.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h"


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

// ギミックワークのアサインID
#define GIMMICK_WORK_ASSIGN_ID (0)


//==========================================================================================
// ■3Dリソース
//==========================================================================================
// リソースインデックス
typedef enum {
  RES_ELBOARD_NSBMD,        // 掲示板のモデル
  RES_ELBOARD_NSBTX,        // 掲示板のテクスチャ
  RES_ELBOARD_NSBTA_1,      // ニュース・スクロール・アニメーション1
  RES_ELBOARD_NSBTA_2,      // ニュース・スクロール・アニメーション2
  RES_ELBOARD_NSBTA_3,      // ニュース・スクロール・アニメーション3
  RES_ELBOARD_NSBTA_4,      // ニュース・スクロール・アニメーション4
  RES_ELBOARD_NSBTA_5,      // ニュース・スクロール・アニメーション5
  RES_ELBOARD_NSBTA_6,      // ニュース・スクロール・アニメーション6
  RES_ELBOARD_NSBTA_7,      // ニュース・スクロール・アニメーション7
  RES_ELBOARD_NSBTA_1_FAST, // ニュース・スクロール・アニメーション1 ( 速 )
  RES_ELBOARD_NSBTA_2_FAST, // ニュース・スクロール・アニメーション2 ( 速 )
  RES_ELBOARD_NSBTA_3_FAST, // ニュース・スクロール・アニメーション3 ( 速 )
  RES_ELBOARD_NSBTA_4_FAST, // ニュース・スクロール・アニメーション4 ( 速 )
  RES_ELBOARD_NSBTA_5_FAST, // ニュース・スクロール・アニメーション5 ( 速 )
  RES_ELBOARD_NSBTA_6_FAST, // ニュース・スクロール・アニメーション6 ( 速 )
  RES_ELBOARD_NSBTA_7_FAST, // ニュース・スクロール・アニメーション7 ( 速 )
  RES_MONITOR_NSBTP_BROKEN, // モニター・テクスチャパターン・アニメーション ( モニタ情報<故障> )
  RES_MONITOR_NSBTP_C01,    // モニター・テクスチャパターン・アニメーション ( モニタ情報C01 )
  RES_MONITOR_NSBTP_C02,    // モニター・テクスチャパターン・アニメーション ( モニタ情報C02 )
  RES_MONITOR_NSBTP_C03,    // モニター・テクスチャパターン・アニメーション ( モニタ情報C03 )
  RES_MONITOR_NSBTP_C04,    // モニター・テクスチャパターン・アニメーション ( モニタ情報C04 )
  RES_MONITOR_NSBTP_C05,    // モニター・テクスチャパターン・アニメーション ( モニタ情報C05 )
  RES_MONITOR_NSBTP_C08W,   // モニター・テクスチャパターン・アニメーション ( モニタ情報C08W )
  RES_MONITOR_NSBTP_C08B,   // モニター・テクスチャパターン・アニメーション ( モニタ情報C08B )
  RES_MONITOR_NSBTP_TG,     // モニター・テクスチャパターン・アニメーション ( モニタ情報TG )
  RES_MONITOR_NSBTP_ST,     // モニター・テクスチャパターン・アニメーション ( モニタ情報ST )
  RES_MONITOR_NSBTP_WF,     // モニター・テクスチャパターン・アニメーション ( モニタ情報WF )
  RES_MONITOR_NSBTP_BC,     // モニター・テクスチャパターン・アニメーション ( モニタ情報BC )
  RES_MONITOR_NSBTP_D03,    // モニター・テクスチャパターン・アニメーション ( モニタ情報D03 )
  RES_NUM                   // 総数
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[ RES_NUM ] = 
{
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbmd, GFL_G3D_UTIL_RESARC },        // 掲示板のモデル
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbtx, GFL_G3D_UTIL_RESARC },        // 掲示板のテクスチャ
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_1_nsbta, GFL_G3D_UTIL_RESARC },      // ニュース・スクロール・アニメーション1
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_2_nsbta, GFL_G3D_UTIL_RESARC },      // ニュース・スクロール・アニメーション2
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_3_nsbta, GFL_G3D_UTIL_RESARC },      // ニュース・スクロール・アニメーション3
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_4_nsbta, GFL_G3D_UTIL_RESARC },      // ニュース・スクロール・アニメーション4
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_5_nsbta, GFL_G3D_UTIL_RESARC },      // ニュース・スクロール・アニメーション5
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_6_nsbta, GFL_G3D_UTIL_RESARC },      // ニュース・スクロール・アニメーション6
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_7_nsbta, GFL_G3D_UTIL_RESARC },      // ニュース・スクロール・アニメーション7
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_1_nsbta, GFL_G3D_UTIL_RESARC }, // ニュース・スクロール・アニメーション1 ( 速 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_2_nsbta, GFL_G3D_UTIL_RESARC }, // ニュース・スクロール・アニメーション2 ( 速 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_3_nsbta, GFL_G3D_UTIL_RESARC }, // ニュース・スクロール・アニメーション3 ( 速 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_4_nsbta, GFL_G3D_UTIL_RESARC }, // ニュース・スクロール・アニメーション4 ( 速 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_5_nsbta, GFL_G3D_UTIL_RESARC }, // ニュース・スクロール・アニメーション5 ( 速 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_6_nsbta, GFL_G3D_UTIL_RESARC }, // ニュース・スクロール・アニメーション6 ( 速 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_fast_7_nsbta, GFL_G3D_UTIL_RESARC }, // ニュース・スクロール・アニメーション7 ( 速 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv00_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報<故障> )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv01_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報C01 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv02_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報C02 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv03_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報C03 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv04_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報C04 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv05_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報C05 )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv08_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報C08W )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv09_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報C08B )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv10_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報TG )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv11_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報ST )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv12_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報WF )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv13_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報BC )
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv14_nsbtp, GFL_G3D_UTIL_RESARC },   // モニター・テクスチャパターン・アニメーション ( モニタ情報D03 )
};

// アニメインデックス
typedef enum {
  ANM_ELBOARD_DATE,         // 掲示板ニュース・スクロール・日付
  ANM_ELBOARD_WEATHER,      // 掲示板ニュース・スクロール・天気
  ANM_ELBOARD_PROPAGATION,  // 掲示板ニュース・スクロール・大量発生
  ANM_ELBOARD_INFO_A,       // 掲示板ニュース・スクロール・情報A
  ANM_ELBOARD_INFO_B,       // 掲示板ニュース・スクロール・情報B
  ANM_ELBOARD_INFO_C,       // 掲示板ニュース・スクロール・情報C
  ANM_ELBOARD_CM,           // 掲示板ニュース・スクロール・一言CM
  ANM_MONITOR_C01,          // モニター・テクスチャ・アニメーション ( モニタ情報C01 )
  ANM_MONITOR_C02,          // モニター・テクスチャ・アニメーション ( モニタ情報C02 )
  ANM_MONITOR_C03,          // モニター・テクスチャ・アニメーション ( モニタ情報C03 )
  ANM_MONITOR_C04,          // モニター・テクスチャ・アニメーション ( モニタ情報C04 )
  ANM_MONITOR_C05,          // モニター・テクスチャ・アニメーション ( モニタ情報C05 )
  ANM_MONITOR_C08W,         // モニター・テクスチャ・アニメーション ( モニタ情報C08W )
  ANM_MONITOR_C08B,         // モニター・テクスチャ・アニメーション ( モニタ情報C08B )
  ANM_MONITOR_TG,           // モニター・テクスチャ・アニメーション ( モニタ情報TG )
  ANM_MONITOR_ST,           // モニター・テクスチャ・アニメーション ( モニタ情報ST )
  ANM_MONITOR_WF,           // モニター・テクスチャ・アニメーション ( モニタ情報WF )
  ANM_MONITOR_BC,           // モニター・テクスチャ・アニメーション ( モニタ情報BC )
  ANM_MONITOR_D03,          // モニター・テクスチャ・アニメーション ( モニタ情報D03 )
  ANM_MONITOR_BROKEN,       // モニター・テクスチャ・アニメーション ( モニタ情報<故障> )
  ANM_NUM                   // 総数
} ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table[ ANM_NUM ] = 
{
  // アニメリソースID, アニメデータID(リソース内部INDEX)
  { RES_ELBOARD_NSBTA_1, 0 },     // 掲示板ニュース・スクロール・日付
  { RES_ELBOARD_NSBTA_2, 0 },     // 掲示板ニュース・スクロール・天気
  { RES_ELBOARD_NSBTA_3, 0 },     // 掲示板ニュース・スクロール・大量発生
  { RES_ELBOARD_NSBTA_4, 0 },     // 掲示板ニュース・スクロール・情報A
  { RES_ELBOARD_NSBTA_5, 0 },     // 掲示板ニュース・スクロール・情報B
  { RES_ELBOARD_NSBTA_6, 0 },     // 掲示板ニュース・スクロール・情報C
  { RES_ELBOARD_NSBTA_7, 0 },     // 掲示板ニュース・スクロール・一言CM
  { RES_MONITOR_NSBTP_C01, 0 },   // モニター・テクスチャ・アニメーション ( モニタ情報C01 )
  { RES_MONITOR_NSBTP_C02, 0 },   // モニター・テクスチャ・アニメーション ( モニタ情報C02 )
  { RES_MONITOR_NSBTP_C03, 0 },   // モニター・テクスチャ・アニメーション ( モニタ情報C03 )
  { RES_MONITOR_NSBTP_C04, 0 },   // モニター・テクスチャ・アニメーション ( モニタ情報C04 )
  { RES_MONITOR_NSBTP_C05, 0 },   // モニター・テクスチャ・アニメーション ( モニタ情報C05 )
  { RES_MONITOR_NSBTP_C08W, 0 },  // モニター・テクスチャ・アニメーション ( モニタ情報C08W )
  { RES_MONITOR_NSBTP_C08B, 0 },  // モニター・テクスチャ・アニメーション ( モニタ情報C08B )
  { RES_MONITOR_NSBTP_TG, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報TG )
  { RES_MONITOR_NSBTP_ST, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報ST )
  { RES_MONITOR_NSBTP_WF, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報WF )
  { RES_MONITOR_NSBTP_BC, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報BC )
  { RES_MONITOR_NSBTP_D03, 0 },   // モニター・テクスチャ・アニメーション ( モニタ情報D03 )
  { RES_MONITOR_NSBTP_BROKEN, 0 },// モニター・テクスチャ・アニメーション ( モニタ情報<故障> )
};
static const GFL_G3D_UTIL_ANM anm_table_fast[ ANM_NUM ] = 
{
  // アニメリソースID, アニメデータID(リソース内部INDEX)
  { RES_ELBOARD_NSBTA_1_FAST, 0 }, // 掲示板ニュース・スクロール・日付 ( 速 )
  { RES_ELBOARD_NSBTA_2_FAST, 0 }, // 掲示板ニュース・スクロール・天気 ( 速 )
  { RES_ELBOARD_NSBTA_3_FAST, 0 }, // 掲示板ニュース・スクロール・大量発生 ( 速 )
  { RES_ELBOARD_NSBTA_4_FAST, 0 }, // 掲示板ニュース・スクロール・情報A ( 速 )
  { RES_ELBOARD_NSBTA_5_FAST, 0 }, // 掲示板ニュース・スクロール・情報B ( 速 )
  { RES_ELBOARD_NSBTA_6_FAST, 0 }, // 掲示板ニュース・スクロール・情報C ( 速 )
  { RES_ELBOARD_NSBTA_7_FAST, 0 }, // 掲示板ニュース・スクロール・一言CM ( 速 )
  { RES_MONITOR_NSBTP_C01, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報C01 )
  { RES_MONITOR_NSBTP_C02, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報C02 )
  { RES_MONITOR_NSBTP_C03, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報C03 )
  { RES_MONITOR_NSBTP_C04, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報C04 )
  { RES_MONITOR_NSBTP_C05, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報C05 )
  { RES_MONITOR_NSBTP_C08W, 0 },   // モニター・テクスチャ・アニメーション ( モニタ情報C08W )
  { RES_MONITOR_NSBTP_C08B, 0 },   // モニター・テクスチャ・アニメーション ( モニタ情報C08B )
  { RES_MONITOR_NSBTP_TG, 0 },     // モニター・テクスチャ・アニメーション ( モニタ情報TG )
  { RES_MONITOR_NSBTP_ST, 0 },     // モニター・テクスチャ・アニメーション ( モニタ情報ST )
  { RES_MONITOR_NSBTP_WF, 0 },     // モニター・テクスチャ・アニメーション ( モニタ情報WF )
  { RES_MONITOR_NSBTP_BC, 0 },     // モニター・テクスチャ・アニメーション ( モニタ情報BC )
  { RES_MONITOR_NSBTP_D03, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報D03 )
  { RES_MONITOR_NSBTP_BROKEN, 0 },    // モニター・テクスチャ・アニメーション ( モニタ情報<故障> )
};

// オブジェクトインデックス
typedef enum {
  OBJ_ELBOARD, // 電光掲示板
  OBJ_NUM      // 総数
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[ OBJ_NUM ] = 
{
  // モデルリソースID, 
  // モデルデータID(リソース内部INDEX), 
  // テクスチャリソースID,
  // アニメテーブル, 
  // アニメリソース数
  { RES_ELBOARD_NSBMD, 0, RES_ELBOARD_NSBTX, anm_table, ANM_NUM }, // 電光掲示板
}; 
static const GFL_G3D_UTIL_OBJ obj_table_fast[ OBJ_NUM ] = 
{
  // モデルリソースID, 
  // モデルデータID(リソース内部INDEX), 
  // テクスチャリソースID,
  // アニメテーブル, 
  // アニメリソース数
  { RES_ELBOARD_NSBMD, 0, RES_ELBOARD_NSBTX, anm_table_fast, ANM_NUM }, // 電光掲示板 ( 速 )
}; 

// ユニットインデックス
typedef enum {
  UNIT_ELBOARD_NORMAL, // 電光掲示板 + モニター
  UNIT_ELBOARD_FAST,   // 電光掲示板(速) + モニター
  UNIT_NUM             // 総数
} UNIT_INDEX;
static const GFL_G3D_UTIL_SETUP unit[ UNIT_NUM ] =
{
  { res_table, RES_NUM, obj_table, OBJ_NUM }, // 電光掲示板 + モニター
  { res_table, RES_NUM, obj_table_fast, OBJ_NUM }, // 電光掲示板(速) + モニター
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
  NEWS_TYPE_CHAMPION,     // チャンピオン情報
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
u32 str_id_weather[ WEATHER_NO_NUM ] = 
{
  msg_gate_sunny,        // 晴れ
  msg_gate_snow,         // 雪
  msg_gate_rain,         // 雨
  msg_gate_storm,        // 砂嵐
  msg_gate_snowstorm,    // 吹雪
  msg_gate_arare,        // あられ
  msg_gate_raikami,      // ライカミ
  msg_gate_kazakami,     // カザカミ
  msg_gate_diamond_dust, // ダイアモンドダスト
  msg_gate_mist,         // 霰
  msg_gate_mist,         // パレスWhite用 霧
  msg_gate_mist,         // パレスBlack用 霧
  msg_gate_storm_high,   // 砂嵐強
  msg_gate_mist,         // パレスWhite用 霧 行ったことない所用
  msg_gate_mist,         // パレスBlack用 霧 行ったことない所用
};

// ジム情報の登録場所
static const NEWS_INDEX gym_news_idx[ GYM_NEWS_MAX_NUM ] = 
{ 
  NEWS_INDEX_PROPAGATION, 
  NEWS_INDEX_INFO_A, 
  NEWS_INDEX_INFO_B, 
  NEWS_INDEX_INFO_C
};

// レア天気一覧
static const u8 RareWeatherNo[] = 
{
  WEATHER_NO_RAIKAMI,
  WEATHER_NO_KAZAKAMI,
  WEATHER_NO_DIAMONDDUST,
};

// レア天気のチェック対象ゾーン一覧
static const u16 RareWeatherCheckZoneID[] = 
{
  ZONE_ID_C07,
};


//==========================================================================================
// ■モニターパラメータ
//==========================================================================================
// アニメーション番号
typedef enum {
  MONITOR_ANIME_C01,    // ( モニタ情報C01 )
  MONITOR_ANIME_C02,    // ( モニタ情報C02 )
  MONITOR_ANIME_C03,    // ( モニタ情報C03 )
  MONITOR_ANIME_C04,    // ( モニタ情報C04 )
  MONITOR_ANIME_C05,    // ( モニタ情報C05 )
  MONITOR_ANIME_C08W,   // ( モニタ情報C08W )
  MONITOR_ANIME_C08B,   // ( モニタ情報C08B )
  MONITOR_ANIME_TG,     // ( モニタ情報TG )
  MONITOR_ANIME_ST,     // ( モニタ情報ST )
  MONITOR_ANIME_WF,     // ( モニタ情報WF )
  MONITOR_ANIME_BC,     // ( モニタ情報BC )
  MONITOR_ANIME_D03,    // ( モニタ情報D03 )
  MONITOR_ANIME_BROKEN, // ( モニタ情報<故障> )
  MONITOR_ANIME_NUM,    // 総数
  MONITOR_ANIME_MAX = MONITOR_ANIME_NUM - 1
} MONITOR_ANIME_INDEX;

// 実アニメーション番号
static u16 monitor_anime[ MONITOR_ANIME_NUM ] = 
{ 
  ANM_MONITOR_C01,    // ( モニタ情報C01 )
  ANM_MONITOR_C02,    // ( モニタ情報C02 )
  ANM_MONITOR_C03,    // ( モニタ情報C03 )
  ANM_MONITOR_C04,    // ( モニタ情報C04 )
  ANM_MONITOR_C05,    // ( モニタ情報C05 )
  ANM_MONITOR_C08W,   // ( モニタ情報C08W )
  ANM_MONITOR_C08B,   // ( モニタ情報C08B )
  ANM_MONITOR_TG,     // ( モニタ情報TG )
  ANM_MONITOR_ST,     // ( モニタ情報ST )
  ANM_MONITOR_WF,     // ( モニタ情報WF )
  ANM_MONITOR_BC,     // ( モニタ情報BC )
  ANM_MONITOR_D03,    // ( モニタ情報D03 )
  ANM_MONITOR_BROKEN, // ( モニタ情報<故障> )
};


//==========================================================================================
// ■天気ニュースの生成パラメータ
//==========================================================================================
typedef struct 
{
  u16 zoneID[ WEATHER_ZONE_NUM ]; // ゾーンID
  u8  weatherNo[ WEATHER_ZONE_NUM ]; // ゾーンの天気

} WEATHER_NEWS_PARAM; 

//==========================================================================================
// ■ニュース登録状況
//==========================================================================================
typedef struct
{
  u8        newsNum;                      // 登録したニュースの数
  NEWS_TYPE newsType[ NEWS_INDEX_NUM ];   // 登録したニュース
  u32       spNewsFlag[ NEWS_INDEX_NUM ]; // 登録した臨時ニュースに対応するフラグ

} NEWS_ENTRY_DATA;

//==========================================================================================
// ■ギミック セーブワーク
//==========================================================================================
typedef struct
{
  BOOL            firstSetupFlag;   // 初回セットアップが完了しているかどうか
  u32             recoveryFrame;    // 復帰フレーム
  u16             champNewsMinutes; // ゲートに入った時のチャンプニュース残り時間
  NEWS_ENTRY_DATA newsEntryData;    // ニュース登録状況

} SAVEWORK; 

//==========================================================================================
// ■ギミック管理ワーク
//==========================================================================================
typedef struct
{ 
  HEAPID         heapID;     // 使用するヒープID
  FIELDMAP_WORK* fieldmap;   // フィールドマップ
  GAMEDATA*      gameData;   // ゲームデータ
  GAMESYS_WORK*  gameSystem; // ゲームシステム

  BOOL firstSetupFlag; // １回目のセットアップが完了しているかどうか

  GOBJ_ELBOARD*        elboard;        // 電光掲示板管理オブジェクト
  u32                  recoveryFrame;  // 復帰フレーム
  ELBOARD_ZONE_DATA*   gateData;       // ゲートデータ
  ELBOARD_SPNEWS_DATA* spNewsData;     // 臨時ニュースデータ
  u8                   spNewsDataNum;  // 臨時ニュースデータ数
  NEWS_ENTRY_DATA      newsEntryData;  // ニュース登録状況
  u16                  champNewsMinutes; // ゲートに入った時のチャンプニュース残り時間

} GATEWORK;


//==========================================================================================
// ■関数インデックス
//==========================================================================================
// セーブワークアクセス
SAVEWORK* GetGimmickSaveWork( FIELDMAP_WORK* fieldmap ); // ギミックのセーブワークを取得する
// ギミックの保存と復帰
static void SaveGimmick( const GATEWORK* work ); // ギミック状態をセーブする
static void LoadGimmick( GATEWORK* work ); // ギミック状態をロードする
static void RecoverChampNewsTimer( GATEWORK* work ); // チャンピオンニュースの残り表示時間を復帰する
static void RecoverSpNewsFlags( GATEWORK* work ); // 臨時ニュースのフラグ状態を復帰する
static void RecoverElboardStatus( GATEWORK* work ); // 電光掲示板の状態を復帰する
// ギミック管理ワークの生成・破棄
static GATEWORK* CreateGateWork( FIELDMAP_WORK* fieldmap ); // ギミック管理ワークを生成する
static void DeleteGateWork( GATEWORK* work ); // ギミック管理ワークを破棄する
static void FirstSetupGateWork( GATEWORK* work ); // ギミック管理ワークの初回セットアップを行う
// ゲートデータ
static void LoadGateData( GATEWORK* work ); // ゲートデータを読み込む
static void DeleteGateData( GATEWORK* work ); // ゲートデータを破棄する
// モニター
static void StartMonitorAnime( GATEWORK* work ); // モニターのアニメーションを開始する
// 電光掲示板
static void SetElboardPos( GATEWORK* work ); // 電光掲示板を配置する
// 天気ニュース
static int GetValidWeatherNewsNum( WEATHER_NEWS_PARAM* param ); // 有効な天気情報の数を取得する
static void InitWeatherNewsParam( WEATHER_NEWS_PARAM* param ); // 天気ニュースパラメータを初期化する
static void GetWeatherNewsParam( const GATEWORK* work, WEATHER_NEWS_PARAM* dest ); // 表示すべき天気ニュースを取得する
static void GetRareWeather( const GATEWORK* work, WEATHER_NEWS_PARAM* dest ); // 珍しい天気ニュースを取得する
static void GetMovePokeWeather( const GATEWORK* work, WEATHER_NEWS_PARAM* dest ); // 移動ポケモンに関する天気ニュースを取得する
// チャンピオンニュース
static BOOL CheckChampionNews( const GATEWORK* work ); // チャンピオンニュースがあるかどうかを判定する
static BOOL CheckChampionDataExist( const GATEWORK* work ); // チャンピオンデータが存在するかどうかを判定する
static BOOL CheckFirstClearDataExist( const GATEWORK* work ); //「はじめてのクリア」データが存在するかどうかを判定する
static BOOL CheckDendouDataExist( const GATEWORK* work ); //「殿堂入り」データが存在するかどうかを判定する
static void GetChampMonsNo_byDendouData( const GATEWORK* work, int* destMonsNo, int* destMonsNum ); //「殿堂入り」データのモンスター番号を取得する
static void GetChampMonsNo_byFirstClear( const GATEWORK* work, int* destMonsNo, int* destMonsNum ); //「はじめてクリア」データのモンスター番号を取得する
// 臨時ニュース
static void LoadSpNewsData( GATEWORK* work ); // 臨時ニュースデータを読み込む
static void DeleteSpNewsData( GATEWORK* work ); // 臨時ニュースデータを破棄する
static const ELBOARD_SPNEWS_DATA* SearchTopNews( const GATEWORK* work ); // 最も優先順位の高い臨時ニュースを検索する
static const ELBOARD_SPNEWS_DATA* SearchGymNews( const GATEWORK* work ); // 最も優先順位の高いジムニュースを検索する
static const ELBOARD_SPNEWS_DATA* SearchFlagNews( const GATEWORK* work, u32 flag ); // 指定したフラグに対応する臨時ニュースを検索する
static BOOL CheckSpecialNews( const GATEWORK* work ); // 臨時ニュースがあるかどうかをチェックする
// ニュース登録
static void SetupElboardNews( GATEWORK* work ); // 掲示板で表示するニュースをセットアップする
static void SetupElboardNews_Normal( GATEWORK* work ); // 掲示板で表示するニュースを平常ニュースで構成する
static void SetupElboardNews_Champion( GATEWORK* work ); // 掲示板で表示するニュースをチャンピオンニュースで構成する
static void SetupElboardNews_Special( GATEWORK* work ); // 掲示板で表示するニュースを臨時ニュースで構成する
static void EntryNews( GATEWORK* work, const NEWS_PARAM* news, NEWS_TYPE type, const ELBOARD_SPNEWS_DATA* sp_data ); // ニュースを登録する
static void AddNewsEntryData( GATEWORK* work, NEWS_TYPE newsType, u32 spNewsFlag ); // 登録状況を更新する
// 平常ニュース追加
static void AddNews_DATE( GATEWORK* work ); // 日付情報を追加する
static void AddNews_WEATHER( GATEWORK* work ); // 天気情報を追加する
static void AddNews_PROPAGATION( GATEWORK* work ); // 大量発生情報を追加する
static void AddNews_INFO_A( GATEWORK* work ); // 地域情報Aを追加する
static void AddNews_INFO_B( GATEWORK* work ); // 地域情報Bを追加する
static void AddNews_INFO_C( GATEWORK* work ); // 地域情報Cを追加する
static void AddNews_CM( GATEWORK* work ); // 一言CMを追加する
// チャンピオンニュース追加
static void AddNews_CHAMPION( GATEWORK* work ); // チャンピオン情報を追加する
// 臨時ニュース追加
static void AddNews_SPECIAL( GATEWORK* work ); // 臨時情報を追加する
static void AddSpNews_DIRECT( GATEWORK* work, const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx ); // 臨時情報 ( メッセージそのまま ) を追加する
static void AddSpNews_GYM( GATEWORK* work ); // 臨時情報 ( ジム情報 ) を追加する
// デバッグ
#ifdef PM_DEBUG
static void DebugPrint_SAVEWORK( const SAVEWORK* save ); // セーブワークを出力する
#endif


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
  GATEWORK* work = NULL;  // GATEギミック管理ワーク
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // 拡張オブジェクトのユニットを追加
  {
    GAMESYS_WORK* gameSystem;
    GAMEDATA* gameData;
    SAVE_CONTROL_WORK* save;
    MOJIMODE mojiMode;
    UNIT_INDEX index;
    CONFIG* config;
    
    gameSystem = FIELDMAP_GetGameSysWork( fieldmap );
    gameData = GAMESYSTEM_GetGameData( gameSystem );
    save = GAMEDATA_GetSaveControlWork( gameData );
    config = SaveData_GetConfig( save );
    mojiMode = CONFIG_GetMojiMode( config );
    switch( mojiMode ) {
    case MOJIMODE_KANJI:    index = UNIT_ELBOARD_NORMAL; break;
    case MOJIMODE_HIRAGANA: index = UNIT_ELBOARD_FAST;   break;
    default: GF_ASSERT(0);
    }
    FLD_EXP_OBJ_AddUnitByHandle( exobj_cnt, &unit[ index ], EXPOBJ_UNIT_ELBOARD );
  }

  // ギミック管理ワークを作成
  work = CreateGateWork( fieldmap );

  // 各種データ読み込み
  LoadGateData( work );   // ゲート
  LoadSpNewsData( work ); // 臨時ニュース
  LoadGimmick( work );    // ギミックのセーブデータ

  // 初回セットアップ
  if( work->firstSetupFlag == FALSE ) { 
    FirstSetupGateWork( work ); 
  }
  else {
    RecoverChampNewsTimer( work ); // チャンピオンニュースの残り時間復帰
    RecoverSpNewsFlags( work );    // フラグ復帰
  }

  // ニュースをセットアップ
  SetupElboardNews( work );

  // 掲示板・モニターをセットアップ
  RecoverElboardStatus( work );  // 掲示板の状態復帰
  SetElboardPos( work ); // 電光掲示板を配置
  StartMonitorAnime( work ); // モニター・アニメーション開始
}

//------------------------------------------------------------------------------------------
/**
 * @brief 終了関数
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  GATEWORK* work = (GATEWORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );

  // ギミック状態をセーブ
  SaveGimmick( work );
  
  // 電光掲示板管理ワークを破棄
  if( work->elboard ){ GOBJ_ELBOARD_Delete( work->elboard ); }

  // ギミック管理ワークを破棄
  DeleteGateWork( work );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 動作関数
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  GATEWORK* work = (GATEWORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );
  static fx32 animeSpeed = FX32_ONE;

  // 電光掲示板メイン処理
  GOBJ_ELBOARD_Main( work->elboard, animeSpeed );

  // モニターアニメーション再生
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
    FLD_EXP_OBJ_PlayAnime( exobj_cnt );
  }
}


//------------------------------------------------------------------------------------------
/**
 * @brief 電光掲示板の向きを取得する
 *
 * @param fieldmap
 *
 * @return 電光掲示板の向き( DIR_xxxx )
 */
//------------------------------------------------------------------------------------------
u8 GATE_GIMMICK_GetElboardDir( FIELDMAP_WORK* fieldmap )
{ 
  GATEWORK* work = (GATEWORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );

  return work->gateData->dir;
}


//==========================================================================================
// ■ 非公開関数
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief ギミックのセーブワークを取得する
 *
 * @param fieldmap フィールドマップ
 *
 * @return ギミックのローカルセーブデータ
 */
//------------------------------------------------------------------------------------------
SAVEWORK* GetGimmickSaveWork( FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*   gsys;
  GAMEDATA*      gdata;
  GIMMICKWORK* gmkwork;
  int           gmk_id;
  SAVEWORK*  save_work;

  GF_ASSERT( fieldmap );

  // ギミックのセーブワークを取得
  gsys      = FIELDMAP_GetGameSysWork( fieldmap );
  gdata     = GAMESYSTEM_GetGameData( gsys );
  gmkwork   = GAMEDATA_GetGimmickWork( gdata );
  gmk_id    = GIMMICKWORK_GetAssignID( gmkwork );
  save_work = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id ); 
  return save_work;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ギミック状態を保存する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void SaveGimmick( const GATEWORK* work )
{
  SAVEWORK* saveWork;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->elboard );

  // ギミック状態を保存
  saveWork = GetGimmickSaveWork( work->fieldmap );
  saveWork->firstSetupFlag   = work->firstSetupFlag;
  saveWork->recoveryFrame    = GOBJ_ELBOARD_GetFrame( work->elboard ) >> FX32_SHIFT; 
  saveWork->champNewsMinutes = work->champNewsMinutes;
  saveWork->newsEntryData    = work->newsEntryData;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ギミックのセーブデータを読み込む
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void LoadGimmick( GATEWORK* work )
{
  SAVEWORK* saveWork;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );

  // セーブデータ取得
  saveWork = GetGimmickSaveWork( work->fieldmap );
  work->firstSetupFlag   = saveWork->firstSetupFlag;
  work->recoveryFrame    = saveWork->recoveryFrame;  // 掲示板の復帰ポイント
  work->champNewsMinutes = saveWork->champNewsMinutes;
}

//------------------------------------------------------------------------------------------
/**
 * @brief チャンピオンニュースの残り表示時間を復帰する
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void RecoverChampNewsTimer( GATEWORK* work )
{
  SAVE_CONTROL_WORK* save;
  MISC* misc;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gameData );

  save = GAMEDATA_GetSaveControlWork( work->gameData );
  misc = SaveData_GetMisc( save ); 
  MISC_SetChampNewsMinutes( misc, work->champNewsMinutes );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 表示していた臨時ニュースに対応するフラグを復帰する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void RecoverSpNewsFlags( GATEWORK* work )
{
  int i;
  EVENTWORK* evwork;
  SAVEWORK* saveWork;
  NEWS_ENTRY_DATA* entry_data;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gameData );
  GF_ASSERT( work->gameSystem );

  evwork     = GAMEDATA_GetEventWork( work->gameData );
  saveWork   = GetGimmickSaveWork( work->fieldmap );
  entry_data = &saveWork->newsEntryData;

  // フラグ復元
  for( i=0; i<entry_data->newsNum; i++)
  {
    if( entry_data->newsType[i] == NEWS_TYPE_SPECIAL ) {
      EVENTWORK_SetEventFlag( evwork, entry_data->spNewsFlag[i] );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板の状態を復帰する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void RecoverElboardStatus( GATEWORK* work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->elboard );

  // 掲示板のアニメフレーム復帰
  GOBJ_ELBOARD_SetFrame( work->elboard, work->recoveryFrame << FX32_SHIFT );
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
  GATEWORK* work;
  HEAPID heapID;
  FLD_EXP_OBJ_CNT_PTR exObjCnt;

  GF_ASSERT( fieldmap );

  heapID   = FIELDMAP_GetHeapID( fieldmap );
  exObjCnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // ギミック管理ワークを生成
  work = (GATEWORK*)GMK_TMP_WK_AllocWork( fieldmap, 
                                          GIMMICK_WORK_ASSIGN_ID, 
                                          heapID, sizeof(GATEWORK) );
  // ワークを初期化
  GFL_STD_MemClear( work, sizeof(GATEWORK) );
  work->heapID         = heapID;
  work->fieldmap       = fieldmap;
  work->gameSystem     = FIELDMAP_GetGameSysWork( fieldmap );
  work->gameData       = GAMESYSTEM_GetGameData( work->gameSystem );
  work->firstSetupFlag = FALSE;

  // 電光掲示板管理ワークを作成
  {
    ELBOARD_PARAM param;
    param.heapID       = heapID;
    param.maxNewsNum   = NEWS_INDEX_NUM;
    param.dispSize     = DISPLAY_SIZE;
    param.newsInterval = NEWS_INTERVAL;
    param.g3dObj       = FLD_EXP_OBJ_GetUnitObj( exObjCnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
    work->elboard      = GOBJ_ELBOARD_Create( &param );
  } 
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
  if( work ) {
    DeleteGateData( work );      // ゲートデータ
    DeleteSpNewsData( work );    // 臨時ニュースデータ
    GMK_TMP_WK_FreeWork( work->fieldmap, GIMMICK_WORK_ASSIGN_ID ); // 本体
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief ギミック管理ワークの初回セットアップを行う
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void FirstSetupGateWork( GATEWORK* work )
{
  const SAVE_CONTROL_WORK* save;
  const MISC* misc;

  GF_ASSERT( work );
  GF_ASSERT( work->gameData );
  GF_ASSERT( work->firstSetupFlag == FALSE );

  if( work->firstSetupFlag == FALSE ) {
    // チャンピオンニュースの残り時間を取得
    save = GAMEDATA_GetSaveControlWorkConst( work->gameData );
    misc = SaveData_GetMiscConst( save ); 
    work->champNewsMinutes = MISC_GetChampNewsMinutes( misc );

    // 初回セットアップ完了
    work->firstSetupFlag = TRUE;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief ゾーンに応じた電光掲示板データを取得する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void LoadGateData( GATEWORK* work )
{
  int datID;
  u16 zoneID;
  u32 dataNum;
  ELBOARD_ZONE_DATA data;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );

  // すでに読み込まれている
  if( work->gateData ){ return; }

  // ゾーンIDを取得
  zoneID = FIELDMAP_GetZoneID( work->fieldmap );

  // ワークを確保
  work->gateData = GFL_HEAP_AllocMemory( work->heapID, sizeof(ELBOARD_ZONE_DATA) );
  dataNum        = GFL_ARC_GetDataFileCnt( ARCID_ELBOARD_ZONE );

  // 電光掲示板データ配列から, 該当するデータを検索
  for( datID=0; datID < dataNum; datID++ )
  {
    // 読み込み
    ELBOARD_ZONE_DATA_Load( &data, ARCID_ELBOARD_ZONE, datID ); 

    // 発見( ゾーン一致 and バージョン一致 )
    if( data.zoneID == zoneID ) {
      if( (data.version == 0) || (data.version == GetVersion()) ) {
        *(work->gateData) = data;
        return;
      }
    }
  } 

  // 現在のゾーンに対応する電光掲示板データが登録されていない場合
  OS_Printf( "error: 電光掲示板データが登録されていません。\n" );
  GF_ASSERT(0);
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
 * @brief モニターのアニメーションを開始する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void StartMonitorAnime( GATEWORK* work )
{ 
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  
  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gateData );

  exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );

  // モニター・アニメーション開始
  {
    int anime_idx;
    anime_idx = monitor_anime[ work->gateData->monitorAnimeIndex ];
    FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD, anime_idx, TRUE ); 
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 電光掲示板を配置する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void SetElboardPos( GATEWORK* work )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  GFL_G3D_OBJSTATUS* status;
  ELBOARD_ZONE_DATA* gate_data;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gateData );

  exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
  status    = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
  gate_data = work->gateData;

  // 座標を設定
  status->trans.x = gate_data->x << FX32_SHIFT;
  status->trans.y = gate_data->y << FX32_SHIFT;
  status->trans.z = gate_data->z << FX32_SHIFT;

  // 向きを設定
  {
    u16 rot;
    switch( gate_data->dir ) {
    case DIR_DOWN:  rot = 0;    break;
    case DIR_RIGHT: rot = 90;   break;
    case DIR_UP:    rot = 180;  break;
    case DIR_LEFT:  rot = 270;  break;
    default:        rot = 0;    break;
    }
    rot *= 182;  // 65536÷360 = 182.044...
    GFL_CALC3D_MTX_CreateRot( 0, rot, 0, &status->rotate );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 登録状況を更新する
 *
 * @param work
 * @param newsType   追加するニュースのタイプ
 * @param spNewsFlag 臨時ニュースの場合, 対応するフラグを指定
 */
//------------------------------------------------------------------------------------------
static void AddNewsEntryData( GATEWORK* work, NEWS_TYPE newsType, u32 spNewsFlag )
{
  NEWS_ENTRY_DATA* entryData = &(work->newsEntryData);
  int              entryNum  = entryData->newsNum;

  // すでに最大数が登録されている
  if( NEWS_INDEX_NUM <= entryNum ){ return; }

  // 登録
  entryData->newsType[ entryNum ]   = newsType;
  entryData->spNewsFlag[ entryNum ] = spNewsFlag;
  entryData->newsNum                = entryNum + 1;
}

//------------------------------------------------------------------------------------------
/**
 * @brief チャンピオンニュースの有無を調べる
 *
 * @param work
 * 
 * @return チャンピオン情報がある場合 TRUE, そうでなければ FALSE
 */
//------------------------------------------------------------------------------------------
static BOOL CheckChampionNews( const GATEWORK* work )
{ 
  //「はじめてクリア」or「殿堂入り」データが存在する
  if( (CheckFirstClearDataExist( work ) == TRUE) ||
      (CheckDendouDataExist( work ) == TRUE) )
  {
    // 表示の残り時間がある
    if( 0 < work->champNewsMinutes ) { 
      return TRUE; 
    }
  }

  return FALSE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief 「はじめてクリア」データが存在するかどうかを判定する
 *
 * @param work
 *
 * @return 「はじめてクリア」のデータが存在する場合 TRUE
 *          そうでない場合 FALSE
 */
//------------------------------------------------------------------------------------------ 
static BOOL CheckFirstClearDataExist( const GATEWORK* work )
{
  EVENTWORK* evwork;

  evwork  = GAMEDATA_GetEventWork( work->gameData );

  // シナリオクリアフラグ立っている
  if( EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_GAME_CLEAR ) == TRUE ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 「殿堂入り」データが存在するかどうかを判定する
 *
 * @param work
 *
 * @return 「殿堂入り」のデータが存在する場合 TRUE
 *          そうでない場合 FALSE
 */
//------------------------------------------------------------------------------------------ 
static BOOL CheckDendouDataExist( const GATEWORK* work )
{
  EVENTWORK* evwork;

  evwork  = GAMEDATA_GetEventWork( work->gameData );

  // チャンピオンに勝利フラグが立っている
  if( EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_CHAMPION_WIN ) == TRUE ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 「殿堂入り」データのモンスター番号を取得する
 *
 * @param work
 * @param destMonsNo  モンスター番号を格納する配列
 * @param destMonsNum 取得したモンスター番号の数の格納先
 */
//------------------------------------------------------------------------------------------
static void GetChampMonsNo_byDendouData( const GATEWORK* work, int* destMonsNo, int* destMonsNum )
{
  SAVE_CONTROL_WORK* save;
  LOAD_RESULT result;
  DENDOU_SAVEDATA* dendouData;
  int i;

  // データが存在しない
  GF_ASSERT( CheckDendouDataExist( work ) == TRUE );

  *destMonsNum = 0;

  // 外部データのロード
  save = GAMEDATA_GetSaveControlWork( work->gameData );
  result = SaveControl_Extra_Load( save, SAVE_EXTRA_ID_DENDOU, work->heapID );

  // 読み込みに成功
  if( (result == LOAD_RESULT_OK) || (result == LOAD_RESULT_NG) )
  {
    // 殿堂データを取得
    dendouData = 
      SaveControl_Extra_DataPtrGet( save, SAVE_EXTRA_ID_DENDOU, EXGMDATA_ID_DENDOU );

    // レコードがある
    if( 0 < DendouData_GetRecordCount(dendouData) ) {

      // モンスター数を取得
      *destMonsNum = DendouData_GetPokemonCount( dendouData, 0 );

      // モンスター番号を取得
      for( i=0; i<*destMonsNum; i++ )
      {
        DENDOU_POKEMON_DATA pokeData;
        pokeData.nickname = GFL_STR_CreateBuffer( 64, work->heapID );
        pokeData.oyaname = GFL_STR_CreateBuffer( 64, work->heapID );
        DendouData_GetPokemonData( dendouData, 0, i, &pokeData );
        destMonsNo[i] = pokeData.monsno;
        GFL_STR_DeleteBuffer( pokeData.nickname );
        GFL_STR_DeleteBuffer( pokeData.oyaname );
      }
    } 
    // レコードがない
    else {
      GF_ASSERT(0);
    }
  }
  // 読み込みに失敗
  else {
    GF_ASSERT(0);
  }

  // セーブデータ解放
  SaveControl_Extra_Unload( save, SAVE_EXTRA_ID_DENDOU );
}

//------------------------------------------------------------------------------------------
/**
 * @brief「はじめてクリア」データのモンスター番号を取得する
 *
 * @param work
 * @param destMonsNo  モンスター番号を格納する配列
 * @param destMonsNum 取得したモンスター番号の数の格納先
 */
//------------------------------------------------------------------------------------------
static void GetChampMonsNo_byFirstClear( const GATEWORK* work, int* destMonsNo, int* destMonsNum )
{
  SAVE_CONTROL_WORK* save;
  LOAD_RESULT result;
  DENDOU_RECORD* record;
  int i;

  // データが存在しない
  GF_ASSERT( CheckFirstClearDataExist(work) == TRUE );

  record = GAMEDATA_GetDendouRecord( work->gameData );
  *destMonsNum = DendouRecord_GetPokemonCount( record );

  for( i=0; i<*destMonsNum; i++ )
  {
    DENDOU_POKEMON_DATA pokeData;
    pokeData.nickname = GFL_STR_CreateBuffer( 64, work->heapID );
    pokeData.oyaname = GFL_STR_CreateBuffer( 64, work->heapID );
    DendouRecord_GetPokemonData( record, i, &pokeData );
    destMonsNo[i] = pokeData.monsno;
    GFL_STR_DeleteBuffer( pokeData.nickname );
    GFL_STR_DeleteBuffer( pokeData.oyaname );
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
  GF_ASSERT( work );

  // すでに読み込まれている
  if( work->spNewsData ){ return; }

  // 臨時ニュースデータを読み込む
  {
    int data_idx;
    int data_num;

    // データ数取得
    data_num = GFL_ARC_GetDataFileCnt( ARCID_ELBOARD_SPNEWS );

    // 各データを取得
    work->spNewsData = GFL_HEAP_AllocMemory( work->heapID, 
                                             sizeof(ELBOARD_SPNEWS_DATA) * data_num );

    for( data_idx=0; data_idx < data_num; data_idx++ )
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
  GF_ASSERT( work );

  if( work->spNewsData ) {
    GFL_HEAP_FreeMemory( work->spNewsData );
    work->spNewsData = NULL;
    work->spNewsDataNum = 0;
  }
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
static const ELBOARD_SPNEWS_DATA* SearchTopNews( const GATEWORK* work )
{
  int i;
  u32        zone_id;
  EVENTWORK* evwork;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gameData );
  GF_ASSERT( work->gameSystem );

  zone_id = FIELDMAP_GetZoneID( work->fieldmap );
  evwork  = GAMEDATA_GetEventWork( work->gameData );

  // データを持っていない
  if( !work->spNewsData ) { return NULL; }

  // 臨時ニュースデータを検索
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    BOOL flag_hit = EVENTWORK_CheckEventFlag( evwork, work->spNewsData[i].flag );
    BOOL zone_hit = ELBOARD_SPNEWS_DATA_CheckZoneHit( &work->spNewsData[i], zone_id );
    BOOL version_hit = ELBOARD_SPNEWS_DATA_CheckVersionHit( &work->spNewsData[i] );

    // バージョン一致 and フラグON and ゾーン一致
    if( version_hit && flag_hit && zone_hit ) { return &work->spNewsData[i]; }
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
static const ELBOARD_SPNEWS_DATA* SearchGymNews( const GATEWORK* work )
{
  int i;
  u32        zoneID;
  EVENTWORK* evwork;

  GF_ASSERT( work );
  GF_ASSERT( work->fieldmap );
  GF_ASSERT( work->gameData );

  zoneID = FIELDMAP_GetZoneID( work->fieldmap );
  evwork = GAMEDATA_GetEventWork( work->gameData );

  // データを持っていない
  if( work->spNewsData == NULL ) { return NULL; }

  // 臨時ニュースデータを検索
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    BOOL flag_hit = EVENTWORK_CheckEventFlag( evwork, work->spNewsData[i].flag );
    BOOL zone_hit = ELBOARD_SPNEWS_DATA_CheckZoneHit( &work->spNewsData[i], zoneID );

    // フラグON and ゾーン一致のジム情報を発見
    if( flag_hit && zone_hit ) {
      if( work->spNewsData[i].newsType == SPNEWS_TYPE_GYM ) {
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
static const ELBOARD_SPNEWS_DATA* SearchFlagNews( const GATEWORK* work, u32 flag )
{
  int i;

  GF_ASSERT( work );

  // データを持っていない
  if( !work->spNewsData ){ return NULL; }

  // 臨時ニュースデータを検索
  for( i=0; i < work->spNewsDataNum; i++ )
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
static BOOL CheckSpecialNews( const GATEWORK* work )
{
  return (SearchTopNews( work ) != NULL);
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
 *  �Bフラグ操作 ( 臨時ニュースの場合 )
 */
//------------------------------------------------------------------------------------------
static void EntryNews( GATEWORK* work, 
                       const NEWS_PARAM* news, NEWS_TYPE type, 
                       const ELBOARD_SPNEWS_DATA* sp_data )
{
  GF_ASSERT( work );

  // ニュースを追加
  GOBJ_ELBOARD_AddNews( work->elboard, news );

  // 登録状況を更新
  if( type == NEWS_TYPE_SPECIAL ) { 
    AddNewsEntryData( work, type, sp_data->flag );
  }
  else {
    AddNewsEntryData( work, type, 0 );
  }

  // フラグ操作
  // 臨時ニュースを追加した場合, そのニュースに設定されたフラグ操作を行う.
  if( type == NEWS_TYPE_SPECIAL ) {
    // フラグを落とす
    if( sp_data->flagControl == FLAG_CONTROL_RESET ) { 
      EVENTWORK* evwork = GAMEDATA_GetEventWork( work->gameData ); 
      EVENTWORK_ResetEventFlag( evwork, sp_data->flag );
    }
  }
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
  GF_ASSERT( work );

  // チャンピオンニュースあり
  if( CheckChampionNews( work ) == TRUE ) {
    SetupElboardNews_Champion( work ); 
  }
  // 臨時ニュースあり
  else if( CheckSpecialNews( work ) ) {
    SetupElboardNews_Special( work ); 
  }
  // 臨時ニュースなし
  else {
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
  // すでに他のニュースが登録されている場合
  if( GOBJ_ELBOARD_GetNewsNum(work->elboard) != 0 ) {
    OS_Printf( "すでに他のニュースが設定されています。\n" );
    GF_ASSERT(0);
    return;
  }

  if( !work->gateData ){ return; } // データを持っていない
  if( !work->gateData->newsDispValidFlag ) { return; } // ニュース表示フラグがOFF

  // ニュースを追加
  AddNews_PROPAGATION( work ); // 大量発生
  AddNews_DATE( work );        // 日付
  AddNews_WEATHER( work );     // 天気
  AddNews_INFO_A( work );      // 地域情報A
  AddNews_INFO_B( work );      // 地域情報B
  AddNews_INFO_C( work );      // 地域情報C
  AddNews_CM( work );          // 一言CM
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板に表示する情報を チャンピオンニュースで構成する
 *
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews_Champion( GATEWORK* work )
{
  // すでに他のニュースが登録されている場合
  if( GOBJ_ELBOARD_GetNewsNum(work->elboard) != 0 ) {
    OS_Printf( "すでに他のニュースが設定されています。\n" );
    GF_ASSERT(0);
    return;
  }

  if( !work->gateData ){ return; } // データを持っていない
  if( !work->gateData->newsDispValidFlag ) { return; } // ニュース表示フラグがOFF

  // ニュースを追加
  AddNews_PROPAGATION( work ); // 大量発生
  AddNews_DATE( work );        // 日付
  AddNews_WEATHER( work );     // 天気
  AddNews_CHAMPION( work );    // チャンピオン情報
  AddNews_CM( work );          // 一言CM
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
  // すでに他のニュースが登録されている場合
  if( GOBJ_ELBOARD_GetNewsNum(work->elboard) != 0 ) {
    OS_Printf( "すでに他のニュースが設定されています。\n" );
    GF_ASSERT(0);
    return;
  }

  if( !work->spNewsData ){ return; } // データを持っていない
  if( !work->gateData ){ return; } // データを持っていない
  if( !work->gateData->newsDispValidFlag ) { return; } // ニュース表示フラグがOFF

  // ニュースを追加
  AddNews_PROPAGATION( work ); // 大量発生
  AddNews_DATE( work );        // 日付
  AddNews_WEATHER( work );     // 天気
  AddNews_SPECIAL( work );     // 臨時ニュース
  AddNews_CM( work );          // 一言CM
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
  u32 msgStrID;
  NEWS_PARAM news;
  WORDSET* wordset;
  WEATHER_NEWS_PARAM newsParam;

  // 天気ニュースパラメータを取得
  GetWeatherNewsParam( work, &newsParam );

  // ワードセットを生成
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
      u16 zoneID;
      u8 weatherNo;

      // ゾーン指定が無効値なら表示しない
      if( newsParam.zoneID[i] == ZONE_ID_MAX ) { continue; }

      zoneID = newsParam.zoneID[i];
      weatherNo = newsParam.weatherNo[i];

      // 地名を取得
      strbuf_zone = 
        GFL_MSG_CreateString( msg_place_name, ZONEDATA_GetPlaceNameID( zoneID ) );
      // 天気を取得
      strbuf_weather = 
        GFL_MSG_CreateString( msg_gate, str_id_weather[ weatherNo ] );

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

    // メッセージデータのハンドルを閉じる
    GFL_MSG_Delete( msg_gate );
    GFL_MSG_Delete( msg_place_name );
  }

  // 表示に使用するメッセージIDを選択
  switch( GetValidWeatherNewsNum( &newsParam ) ) {
  case 1: msgStrID = msg_gate_weather_set_1; break;
  case 2: msgStrID = msg_gate_weather_set_2; break;
  case 3: msgStrID = msg_gate_weather_set_3; break;
  case 4: msgStrID = msg_gate_weather_set_4; break;
  default: GF_ASSERT(0);
  }

  // ニュースパラメータを作成
  news.animeIndex = news_anm_index[ NEWS_INDEX_WEATHER ];
  news.texName    = news_tex_name[ NEWS_INDEX_WEATHER ];
  news.pltName    = news_plt_name[ NEWS_INDEX_WEATHER ];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = msgStrID;
  news.wordset    = wordset;

  // ニュース登録
  EntryNews( work, &news, NEWS_TYPE_WEATHER, NULL ); 

  // ワードセット破棄
  WORDSET_Delete( wordset ); 
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
  NEWS_PARAM news;
  WORDSET* wordset;
  u16 zoneID;

  // 大量発生が起きているゾーンを取得
  zoneID = ENCPOKE_GetGenerateZone( work->gameData ); 

  // 大量発生が起きていない
  if( zoneID == 0xFFFF ){ return; }

  // R08 は冬にエンカウントできない
  if( (zoneID == ZONE_ID_R08) && 
      (GAMEDATA_GetSeasonID(work->gameData) == PMSEASON_WINTER) ) {
    return;
  }

  // ワードセット作成
  wordset = WORDSET_Create( work->heapID );

  // ワードセットに地名をセット
  {
    GFL_MSGDATA* msg_place_name;
    int str_id;
    STRBUF* zone_name;

    msg_place_name = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
                                     ARCID_MESSAGE, NARC_message_place_name_dat, work->heapID ); 
    str_id         = ZONEDATA_GetPlaceNameID( zoneID );
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

  // ニュース登録
  EntryNews( work, &news, NEWS_TYPE_PROPAGATION, NULL ); 

  // ワードセット破棄
  WORDSET_Delete( wordset );
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
  switch( date.week ) {
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

  // ニュース登録
  EntryNews( work, &news, NEWS_TYPE_INFO_A, NULL ); 
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
  switch( date.week ) {
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

  // ニュース登録
  EntryNews( work, &news, NEWS_TYPE_INFO_B, NULL ); 
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
  switch( date.week ) {
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

  // ニュース登録
  EntryNews( work, &news, NEWS_TYPE_INFO_C, NULL ); 
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
  switch( date.week ) {
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
  
  // ニュース登録
  EntryNews( work, &news, NEWS_TYPE_CM, NULL ); 
} 

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のチャンピオンニュースを追加する
 *
 * @param work     ギミック管理ワーク
 * @param news_idx ニュースの追加場所を指定
 */
//------------------------------------------------------------------------------------------
static void AddNews_CHAMPION( GATEWORK* work )
{
  HEAPID heap_id; 
  NEWS_PARAM news;
  WORDSET* wordset;
  int monsno[6];
  int monsnum;
  int i;
  u32 strID;

  // ワードセット作成
  heap_id = GOBJ_ELBOARD_GetHeapID( work->elboard );
  wordset = WORDSET_Create( heap_id );

  //「殿堂入り」データがあれば, 参照する
  if( CheckDendouDataExist( work ) ) {
    GetChampMonsNo_byDendouData( work, monsno, &monsnum );
  }
  //「殿堂入り」データがなければ,「はじめてクリア」データを参照する
  else {
    GetChampMonsNo_byFirstClear( work, monsno, &monsnum );
  }

  // 使用するメッセージを選択
  switch( monsnum ) {
  case 1: strID = msg_gate_champ_1; break;
  case 2: strID = msg_gate_champ_2; break;
  case 3: strID = msg_gate_champ_3; break;
  case 4: strID = msg_gate_champ_4; break;
  case 5: strID = msg_gate_champ_5; break;
  case 6: strID = msg_gate_champ_6; break;
  default: GF_ASSERT(0);
  }

  // ポケモン名をワードセットに登録
  for( i=0; i<monsnum; i++ )
  {
    WORDSET_RegisterPokeMonsNameNo( wordset, i, monsno[i] );
  } 

  // プレイヤー名をワードセットに登録
  {
    MYSTATUS* mystatus;
    mystatus = GAMEDATA_GetMyStatus( work->gameData );
    WORDSET_RegisterPlayerName( wordset, 6, mystatus );
  }

  // ニュースパラメータを作成
  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_A];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_A];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_A];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = strID;
  news.wordset    = wordset;

  // ニュース登録
  EntryNews( work, &news, NEWS_TYPE_CHAMPION, NULL ); 

  // ワードセット破棄
  WORDSET_Delete( wordset );
}

//------------------------------------------------------------------------------------------
/**
 * @brief 掲示板のニュースを追加する(臨時ニュース)
 * 
 * @param work ギミック管理ワーク
 */
//------------------------------------------------------------------------------------------
static void AddNews_SPECIAL( GATEWORK* work )
{ 
  const ELBOARD_SPNEWS_DATA* news;

  // 臨時ニュースを取得
  news = SearchTopNews( work );

  GF_ASSERT( news ) // 臨時ニュースがない

  // ニュースを追加
  switch( news->newsType ) {
  case SPNEWS_TYPE_DIRECT: AddSpNews_DIRECT( work, news, NEWS_INDEX_INFO_A );  break;
  case SPNEWS_TYPE_GYM:    AddSpNews_GYM( work );                              break;
  default: GF_ASSERT(0);
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

  news.animeIndex = news_anm_index[ news_idx ];
  news.texName    = news_tex_name[ news_idx ];
  news.pltName    = news_plt_name[ news_idx ];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = spnews->msgID;
  news.wordset    = NULL;
  
  // ニュース登録
  EntryNews( work, &news, NEWS_TYPE_SPECIAL, spnews ); 
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
  while( news && count < GYM_NEWS_MAX_NUM )
  {
    // ニュースを追加
    AddSpNews_DIRECT( work, news, gym_news_idx[count++] );

    // 次のジムニュースを取得
    news = SearchGymNews( work );
  } 
}

//------------------------------------------------------------------------------------------
/**
 * @brief 有効な天気情報の数を取得する
 *
 * @param param 天気ニュースパラメータ
 *
 * @return 指定した天気ニュースパラメータに含まれる, 有効な天気情報の数
 */
//------------------------------------------------------------------------------------------
static int GetValidWeatherNewsNum( WEATHER_NEWS_PARAM* param )
{
  int i;
  int num = 0;

  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    // 有効なデータを発見
    if( (param->zoneID[i] != ZONE_ID_MAX) &&
        (param->weatherNo[i] != WEATHER_NO_NONE) ) { num++; }
  }

  return num;
}

//------------------------------------------------------------------------------------------
/**
 * @brief 天気ニュースパラメータを初期化する
 *
 * @param param 初期化するパラメータ
 */
//------------------------------------------------------------------------------------------
static void InitWeatherNewsParam( WEATHER_NEWS_PARAM* param )
{
  int i;

  // 無効値で初期化する
  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    param->zoneID[i]    = ZONE_ID_MAX;
    param->weatherNo[i] = WEATHER_NO_NONE;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 表示すべき天気ニュースを取得する
 *
 * @param work
 * @param dest 取得した情報の格納先 
 */
//------------------------------------------------------------------------------------------
static void GetWeatherNewsParam( const GATEWORK* work, WEATHER_NEWS_PARAM* dest )
{
  WEATHER_NEWS_PARAM rareWeather;
  WEATHER_NEWS_PARAM movePokeWeather;
  int num = 0;
  int i;

  InitWeatherNewsParam( dest ); // 初期化
  GetRareWeather( work, &rareWeather ); // レア天気を取得
  GetMovePokeWeather( work, &movePokeWeather ); // 移動ポケモン天気を取得

  // 取得した移動ポケモン天気を登録
  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    // 無効データを発見
    if( (movePokeWeather.zoneID[i] == ZONE_ID_MAX) ||
        (movePokeWeather.weatherNo[i] == WEATHER_NO_NONE) ) { break; }

    // 特殊天気の同時多発数が多すぎる
    if( SP_WEATHER_MAX_NUM <= num ) {
      GF_ASSERT(0);
      break;
    }

    // 登録
    dest->zoneID[num]    = movePokeWeather.zoneID[i];
    dest->weatherNo[num] = movePokeWeather.weatherNo[i];
    num++;
  }

  // 取得した特殊天気を登録
  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    // 無効データを発見
    if( (rareWeather.zoneID[i] == ZONE_ID_MAX) ||
        (rareWeather.weatherNo[i] == WEATHER_NO_NONE) ) { break; }

    // 特殊天気の同時多発数が多すぎる
    if( SP_WEATHER_MAX_NUM <= num ) {
      GF_ASSERT(0);
      break;
    }

    // 登録
    dest->zoneID[num]    = rareWeather.zoneID[i];
    dest->weatherNo[num] = rareWeather.weatherNo[i];
    num++;
  }

  // 特殊天気がない場合は, 通常の天気を登録する
  if( num == 0 ) {
    dest->zoneID[0] = work->gateData->zoneID_weather_1;
    dest->zoneID[1] = work->gateData->zoneID_weather_2;
    dest->zoneID[2] = work->gateData->zoneID_weather_3;
    dest->zoneID[3] = work->gateData->zoneID_weather_4;
  }
  for( i=0; i<WEATHER_ZONE_NUM; i++ )
  {
    dest->weatherNo[i] = FIELDMAP_GetZoneWeatherID( work->fieldmap, dest->zoneID[i] );
  } 
}

//------------------------------------------------------------------------------------------
/**
 * @brief 珍しい天気ニュースを取得する
 *
 * @param work
 * @param dest 取得した情報の格納先
 */
//------------------------------------------------------------------------------------------
static void GetRareWeather( const GATEWORK* work, WEATHER_NEWS_PARAM* dest )
{
  int i, j;
  int num; 

  // データの格納先をクリア
  InitWeatherNewsParam( dest );

  // チェック対象ゾーンについて, レア天気を探す
  num = 0;
  for( i=0; i<NELEMS(RareWeatherCheckZoneID); i++ )
  {
    // 今日の天気を取得
    u16 zoneID = RareWeatherCheckZoneID[i];
    u8 weather = FIELDMAP_GetZoneWeatherID( work->fieldmap, zoneID );

    // レア天気かどうかを判定
    for( j=0; j<NELEMS(RareWeatherNo); j++ )
    {
      // レア天気を発見
      if( weather == RareWeatherNo[j] ) {
        // レア天気の同時多発数が多すぎる
        if( WEATHER_ZONE_NUM <= num ) {
          GF_ASSERT(0);
          break;
        }
        // 天気ニュースパラメータに追加
        dest->zoneID[ num ] = zoneID;
        dest->weatherNo[ num ] = weather;
        num++;
      }
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief 移動ポケモンに関する天気ニュースを取得する
 *
 * @param work
 * @param dest 取得した情報の格納先
 */
//------------------------------------------------------------------------------------------
static void GetMovePokeWeather( const GATEWORK* work, WEATHER_NEWS_PARAM* dest )
{
  SAVE_CONTROL_WORK* save;
  ENC_SV_PTR encSave;
  int movePokeID;
  u16 zoneID;
  u8 weatherNo;

  save    = GAMEDATA_GetSaveControlWork( work->gameData );
  encSave = EncDataSave_GetSaveDataPtr( save );

  // データの格納先をクリア
  InitWeatherNewsParam( dest );

  // 移動ポケモンがいない
  if( MP_CheckMovePoke( encSave ) == FALSE ) { return; }

  // 移動ポケモンIDを決定
  switch( GetVersion() ) {
  case VERSION_WHITE: movePokeID = MOVE_POKE_RAIKAMI;  break;
  case VERSION_BLACK: movePokeID = MOVE_POKE_KAZAKAMI; break;
  default: GF_ASSERT(0);
  }

  // 天気を決定
  switch( movePokeID ) { 
  case MOVE_POKE_RAIKAMI:  weatherNo = WEATHER_NO_RAIKAMI; break;
  case MOVE_POKE_KAZAKAMI: weatherNo = WEATHER_NO_KAZAKAMI; break;
  default: GF_ASSERT(0);
  }

  // 移動ポケモンがいるゾーンIDを取得
  zoneID = MP_GetMovePokeZoneID( encSave, movePokeID ); 

  // 移動ポケモンが隠れているので無効
  if( zoneID == MVPOKE_ZONE_NULL ) { return; }

  // 天気ニュースデータに追加
  dest->zoneID[0]    = zoneID;
  dest->weatherNo[0] = weatherNo;
}

#ifdef PM_DEBUG
//------------------------------------------------------------------------------------------
/**
 * @brief セーブワークを出力する
 *
 * @param save
 */
//------------------------------------------------------------------------------------------
static void DebugPrint_SAVEWORK( const SAVEWORK* save )
{
  int i;
  OBATA_Printf( "GIMMICK-GATE: gimmick load\n" );
  OBATA_Printf( "-recoveryFrame = %d\n", save->recoveryFrame );
  OBATA_Printf( "-newsEntryData.newsNum = %d\n",  save->newsEntryData.newsNum );
  for( i=0; i<NEWS_INDEX_NUM; i++ )
  {
    OBATA_Printf( "-newsEntryData.newsType[%d] = ", i );
    switch( save->newsEntryData.newsType[i] ) {
    case NEWS_TYPE_NULL:        OBATA_Printf( "NULL\n" );        break;
    case NEWS_TYPE_DATE:        OBATA_Printf( "DATE\n" );        break;
    case NEWS_TYPE_WEATHER:     OBATA_Printf( "WEATHER\n" );     break;
    case NEWS_TYPE_PROPAGATION: OBATA_Printf( "PROPAGATION\n" ); break;
    case NEWS_TYPE_INFO_A:      OBATA_Printf( "INFO_A\n" );      break;
    case NEWS_TYPE_INFO_B:      OBATA_Printf( "INFO_B\n" );      break;
    case NEWS_TYPE_INFO_C:      OBATA_Printf( "INFO_C\n" );      break;
    case NEWS_TYPE_CM:          OBATA_Printf( "CM\n" );          break;
    case NEWS_TYPE_SPECIAL:     OBATA_Printf( "SPECIAL\n" );     break;
    case NEWS_TYPE_CHAMPION:    OBATA_Printf( "CHAMPION\n" );    break;
    default:                    OBATA_Printf( "UNKNOWN\n" );     break;
    }
  }
  for( i=0; i<NEWS_INDEX_NUM; i++ )
  {
    OBATA_Printf( "-newsEntryData.spNewsFlag[%d] = %d\n", 
                  i, save->newsEntryData.spNewsFlag[i] );
  }
}
#endif
