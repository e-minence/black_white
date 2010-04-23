//===============================================================================
/*
 *  @file event_debug_livecomm.c
 *  @brief  すれ違い通信デバッグメニューイベント
 *  @author Miyuki Iwasawa
 *  @date   10.04.19
 */
//===============================================================================

#ifdef PM_DEBUG

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "arc_def.h"
#include "font/font.naix"

#include "field/field_msgbg.h"
#include "fieldmap.h"
#include "field/zonedata.h"

#include "sound/pm_sndsys.h"

#include "field_sound.h"
#include "field/eventwork.h"

#include "event_debug_local.h"
#include "debug/debug_str_conv.h" // for DEB_STR_CONV_SjisToStrcode
#include "print/wordset.h"  //WORDSET
#include "net/net_whpipe.h"
#include "app/townmap_util.h"

#include "message.naix"
#include "msg/msg_d_numinput.h"
#include "msg/msg_d_field.h"
#include "msg/msg_d_livecomm.h"

#include "arc/fieldmap/zone_id.h"
#include "arc/fieldmap/debug_symbol.naix"  //DEBUG_SYM_
#include "arc/fieldmap/debug_list.h"  //DEBUG_SCR_

#include "poke_tool/monsno_def.h"
#include "tr_tool/trno_def.h"
#include "item/itemsym.h"
#include "waza_tool/wazano_def.h"

#include "savedata/intrude_save.h"
#include "savedata/playtime.h"

#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_types.h"
#include "field/beacon_view.h"
#include "field/field_comm/beacon_view_local.h"
#include "event_debug_livecomm.h"

//======================================================================
//
//定数定義
//
//======================================================================
typedef enum{
  SEQ_MENU_INIT,
  SEQ_MENU_MAIN,
  SEQ_EXIT,
}SEQ_ID;

enum{
 MENU_BEACON_REQ,
 MENU_STACK_CHECK,
 MENU_SURETIGAI_NUM,
 MENU_THANKS_NUM,
 MENU_STACK_CLEAR,
 MENU_MEMBER_CLEAR,
 MENU_COMM_BUF_CLEAR,
 MENU_EXIT,
};

/////////////////////////////////////////////
///パレット
#define PANO_FONT (14)
#define FBMP_COL_WHITE  (15)
#define FCOL_WIN_BASE (15)
#define FCOL_WIN_W  ( PRINTSYS_MACRO_LSB(1,2,FCOL_WIN_BASE) ) ///<BG白抜き

#define BUFLEN_BEACON_MSG (8*2*2+EOM_SIZE)

/////////////////////////////////////////////
///スタックチェック
#define SCHECK_LINE_MAX (4)
#define SCHECK_LINE_OY  (16*3)
#define SCHECK_PAGE_MAX (8)


/////////////////////////////////////////////
///ビーコンリクエスト
#define BREQ_LINE_MAX (3+FAKE_DATA_MAX)
#define BREQ_WIN_MAIN_SX  (31)

#define BREQ_FWAZA_MAX  (9)

typedef enum{
 TR_PAT_MINE,
 TR_PAT_FAKE,
 TR_PAT_RND,
 TR_PAT_MAX,
}TR_PATTERN;

typedef enum{
  FAKE_ID,
  FAKE_NAME,
  FAKE_ZONE,
  FAKE_SEX,
  FAKE_VERSION,
  FAKE_LANG,
  FAKE_DATA_MAX,
}FAKE_DATA_ID;

typedef enum{
  BPRM_WSET_NONE,
  BPRM_WSET_TRNAME,
  BPRM_WSET_TRNAME_EX,
  BPRM_WSET_MONSNAME,
  BPRM_WSET_ITEMNAME,
  BPRM_WSET_FWAZA,
  BPRM_WSET_GPOWER,
  BPRM_WSET_ACTION,
  BPRM_WSET_BEACON_REQ_TRAINER,
  BPRM_WSET_SEX,
  BPRM_WSET_VERSION,
  BPRM_WSET_LANG,
  BPRM_WSET_ZONE,
}BPRM_WSET_TYPE;

typedef enum{
 BEACON_PSET_DEFAULT,     //デフォルト(トレーナー名)  
 BEACON_PSET_TR_ID,      //対戦相手名
 BEACON_PSET_MONSNO,    //ポケモン種族名
 BEACON_PSET_NICKNAME,    //ポケモンニックネーム
 BEACON_PSET_ITEM,        //アイテム名
 BEACON_PSET_PTIME,       //プレイタイム
 BEACON_PSET_SURETIGAI_CT,//すれ違い回数
 BEACON_PSET_THANKS,      //御礼回数
 BEACON_PSET_WAZA,        //技名
 BEACON_PSET_VICTORY,     //サブウェイ挑戦中の連勝数1-7
 BEACON_PSET_TH_RANK,     //トライアルハウスランク
 BEACON_PSET_GPOWER,      //Gパワー名
 BEACON_PSET_ID,          //プレイヤーID
 BEACON_PSET_FREEWORD,    //フリーワード8文字
 BEACON_PSET_MAX,
}BEACON_PSET_TYPE;

typedef enum{
 BEACON_ARG_NONE,
 BEACON_ARG_U8,
 BEACON_ARG_U16,
 BEACON_ARG_U32,
 BEACON_ARG_STR,
 BEACON_ARG_U16_STR,
 BEACON_ARG_GDATA_U32,
 BEACON_ARG_FWORD,
 BEACON_ARG_GPOWER,
}BEACON_ARG_TYPE;

typedef void (*BEACON_SET_FUNC_NONE)(void);
typedef void (*BEACON_SET_FUNC_U8)(u8);
typedef void (*BEACON_SET_FUNC_U16)(u16);
typedef void (*BEACON_SET_FUNC_U32)(u32);
typedef void (*BEACON_SET_FUNC_STR)(const STRBUF*);
typedef void (*BEACON_SET_FUNC_U16_STR)(u16,const STRBUF*);
typedef void (*BEACON_SET_FUNC_GDATA_U32)(GAMEDATA* gdata,u32);
typedef void (*BEACON_SET_FUNC_GDATA_STR)(GAMEDATA* gdata, const STRBUF*);
typedef void (*BEACON_SET_FUNC_GPOWER)(GPOWER_ID);

//======================================================================
//
//構造体定義
//
//======================================================================

typedef struct _D_BEACON_PARAM{
  void* func;
  BEACON_PSET_TYPE  prm_type; //パラメータタイプ
  BEACON_ARG_TYPE   arg_type; //引数の型タイプ
}D_BEACON_PARAM;

typedef struct _D_FAKE_PRM_SET{
  BPRM_WSET_TYPE  wset_type;
  int keta;
  int min,max;
}D_FAKE_PRM_SET;

typedef struct _D_BEACON_PRM_SET{
  int min,max;   //パラメータのmix,max
  BPRM_WSET_TYPE wset_type;
}D_BEACON_PRM_SET;

typedef struct _D_BEACON_REQ{
  TR_PATTERN  trainer;  ///トレーナーのパターン
  u32         tr_id;
  GAMEBEACON_ACTION action;

  u16 tr_no;
  u16 mons_no;
  u16 item_no;
  u16 ptime_h;  //プレイ時間(時)
}D_BEACON_REQ;

typedef struct _DMENU_LIVE_COMM{
  HEAPID  heapID;
  HEAPID  tmpHeapID;

  GAMESYS_WORK* gsys;
  GAMEDATA* gdata;
  FIELDMAP_WORK* fieldmap;

  SAVE_CONTROL_WORK* save;
  INTRUDE_SAVE_WORK* int_sv;
  MYITEM_PTR item_sv;
  MISC* misc_sv;
  MYSTATUS* my_status;

  BEACON_VIEW*    view_wk;
  BEACON_STATUS* b_status;
  GAMEBEACON_INFO_TBL*  infoStack;  //スタックワーク
  GAMEBEACON_INFO_TBL*  infoLog;    //ログテーブル
 
  GFL_FONT* fontHandle;
	GFL_MSGDATA* msgman;						//メッセージマネージャー
  WORDSET* wset;
  STRBUF *str_tmp;
  STRBUF *str_expand;
  const char* pAllZoneName;
 
  FLDMENUFUNC *menuFunc;

  int               beacon_prm[BEACON_PSET_MAX];
  int               fake_prm[TR_PAT_MAX][FAKE_DATA_MAX];
  STRBUF*           beacon_nickname;
  STRBUF*           beacon_fword;

  GAMEBEACON_INFO*  tmpInfo;
  u16               tmpTime;
  BMP_WIN           win_ninput;
}DMENU_LIVE_COMM;

typedef int (*NINPUT_GET_FUNC)( DMENU_LIVE_COMM* wk, int param );
typedef void (*NINPUT_SET_FUNC)( DMENU_LIVE_COMM* wk, int param, int value );

/// 数値入力：パラメータ
typedef struct {
  int min;  ///<入力最小値
  int max;  ///<入力最大値
  int* ret_wk;
  ///値を取得するための関数
  NINPUT_SET_FUNC set_func;
  NINPUT_GET_FUNC get_func;
}NINPUT_PARAM;

//======================================================================
//
//データ定義
//
//======================================================================

static const FLDMENUFUNC_LIST DATA_DebugLiveCommMenuList[] =
{
  { dlc_menu_01, (void*)MENU_BEACON_REQ },
  { dlc_menu_02, (void*)MENU_STACK_CHECK },
  { dlc_menu_03, (void*)MENU_SURETIGAI_NUM },
  { dlc_menu_04, (void*)MENU_THANKS_NUM },
  { dlc_menu_05, (void*)MENU_STACK_CLEAR },
  { dlc_menu_06, (void*)MENU_MEMBER_CLEAR },
  { dlc_menu_07, (void*)MENU_COMM_BUF_CLEAR },
  { dlc_menu_08, (void*)MENU_EXIT },
};

static const DEBUG_MENU_INITIALIZER DebugLiveCommMenuData = {
  NARC_message_d_livecomm_dat,
  NELEMS(DATA_DebugLiveCommMenuList),
  DATA_DebugLiveCommMenuList,
  &DATA_DebugMenuList_Default, //流用
  1, 1, 24, 17,
  NULL,
  NULL
};


/*
 *  @brief  GAMEBEACON_ACTION型の並びと同一である必要があります
 */
static const D_BEACON_PARAM DATA_DebugBeaconParam[GAMEBEACON_ACTION_MAX] = {
  ///<GAMEBEACON_ACTION_NULL  ///<データ無し 0
  { NULL,                                 BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE, },

  ///<XXさんをサーチしました！ 1
  { NULL,                                 BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE, }, 
  ///<BATTLE_WILD_POKE_START 野生のポケモンと対戦を開始しました！2
  { GAMEBEACON_Set_BattleWildPokeStart,   BEACON_PSET_MONSNO,	  BEACON_ARG_U16,	},
  ///<BATTLE_WILD_POKE_VICTORY 野生のポケモンに勝利しました！3
  { GAMEBEACON_Set_BattleWildPokeVictory, BEACON_PSET_MONSNO,	  BEACON_ARG_U16,	},
  ///<BATTLE_SP_POKE_START 特別なポケモンと対戦を開始しました！ 4
  { GAMEBEACON_Set_BattleSpPokeStart,     BEACON_PSET_MONSNO,	  BEACON_ARG_U16,	},
  ///<BATTLE_SP_POKE_VICTORY 特別なポケモンに勝利しました！5
  { GAMEBEACON_Set_BattleSpPokeVictory,   BEACON_PSET_MONSNO,	  BEACON_ARG_U16,	},
  ///<BATTLE_TRAINER_START トレーナーと対戦を開始しました！6
  { GAMEBEACON_Set_BattleTrainerStart,    BEACON_PSET_TR_ID,	    BEACON_ARG_U16,	},
  ///<BATTLE_TRAINER_VICTORY トレーナーに勝利しました！7
  { GAMEBEACON_Set_BattleTrainerVictory,  BEACON_PSET_TR_ID,	    BEACON_ARG_U16,	},
  ///<BATTLE_LEADER_START  ジムリーダーと対戦を開始しました！ 8
  { GAMEBEACON_Set_BattleLeaderStart,     BEACON_PSET_DEFAULT,	  BEACON_ARG_U16,	},
  ///<BATTLE_LEADER_VICTORY ジムリーダーに勝利しました！9
  { GAMEBEACON_Set_BattleLeaderVictory,   BEACON_PSET_DEFAULT,	  BEACON_ARG_U16,	},
  ///<BATTLE_BIGFOUR_START  四天王と対戦を開始しました！10
  { GAMEBEACON_Set_BattleSpTrainerStart,  BEACON_PSET_DEFAULT,	  BEACON_ARG_U16,	},
  ///<BATTLE_BIGFOUR_VICTORY 四天王に勝利しました！11
  { GAMEBEACON_Set_BattleSpTrainerVictory,BEACON_PSET_DEFAULT,	  BEACON_ARG_U16,	},
  ///<BATTLE_CHAMPION_START チャンピオンと対戦を開始しました！12
  { GAMEBEACON_Set_BattleSpTrainerStart,  BEACON_PSET_DEFAULT,	  BEACON_ARG_U16,	},
  ///<BATTLE_CHAMPION_VICTORY チャンピオンに勝利しました！13
  { GAMEBEACON_Set_BattleSpTrainerVictory,BEACON_PSET_DEFAULT,	  BEACON_ARG_U16,	},
  ///<POKE_GET ポケモン捕獲 14
  { GAMEBEACON_Set_WildPokemonGet,        BEACON_PSET_MONSNO,	  BEACON_ARG_U16,	},
  ///<SP_POKE_GET 特別なポケモン捕獲 15
  { GAMEBEACON_Set_SpecialPokemonGet,     BEACON_PSET_MONSNO,	  BEACON_ARG_U16,	},
  ///<POKE_LVUP ポケモンレベルアップ 16
  { GAMEBEACON_Set_PokemonLevelUp,        BEACON_PSET_NICKNAME,	  BEACON_ARG_STR,	},
  ///<POKE_EVOLUTION ポケモン進化 17
  { GAMEBEACON_Set_PokemonEvolution,      BEACON_PSET_MONSNO, 	  BEACON_ARG_U16_STR,	},
  ///<GPOWER Gパワー発動 18
  { GAMEBEACON_Set_GPower,                BEACON_PSET_GPOWER,	  BEACON_ARG_GPOWER,	},
  ///<SP_ITEM_GET 貴重品ゲット 19
  { GAMEBEACON_Set_SpItemGet,             BEACON_PSET_ITEM,	      BEACON_ARG_U16,	},
  ///<PLAYTIME  一定のプレイ時間を越えた 20
  { GAMEBEACON_Set_PlayTime,              BEACON_PSET_PTIME,	    BEACON_ARG_U32,	},
  ///<ZUKAN_COMPLETE 図鑑完成 21
  { GAMEBEACON_Set_ZukanComplete,         BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<THANKYOU_OVER お礼を受けた回数が規定数を超えた 22
  { GAMEBEACON_Set_ThankyouOver,          BEACON_PSET_THANKS,	    BEACON_ARG_U32,	},
  ///<UNION_IN ユニオンルームに入った 23
  { GAMEBEACON_Set_UnionIn,               BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<THANKYOU ありがとう！24
  { GAMEBEACON_Set_Thankyou,              BEACON_PSET_ID,	        BEACON_ARG_GDATA_U32,	},
  ///<DISTRIBUTION_POKE ポケモン配布中 25
  { GAMEBEACON_Set_DistributionPoke,      BEACON_PSET_MONSNO,	    BEACON_ARG_U16,	},
  ///<DISTRIBUTION_ITEM アイテム配布中 26
  { GAMEBEACON_Set_DistributionItem,      BEACON_PSET_ITEM,	      BEACON_ARG_U16,	},
  ///<DISTRIBUTION_ETC  その他配布中 27
  { GAMEBEACON_Set_DistributionEtc,       BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<CRITICAL_HIT 急所に攻撃を当てた 28
  { GAMEBEACON_Set_CriticalHit,           BEACON_PSET_NICKNAME,	  BEACON_ARG_STR,	},
  ///<CRITICAL_DAMAGE 急所に攻撃を受けた 29
  { GAMEBEACON_Set_CriticalDamage,        BEACON_PSET_NICKNAME,	  BEACON_ARG_STR,	},
  ///<ESCAPE 戦闘から逃げ出した 30
  { GAMEBEACON_Set_Escape,                BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<HP_LITTLE HPが残り少ない 31
  { GAMEBEACON_Set_HPLittle,              BEACON_PSET_NICKNAME,	  BEACON_ARG_STR,	},
  ///<PP_LITTLE PPが残り少ない 32
  { GAMEBEACON_Set_PPLittle,              BEACON_PSET_NICKNAME,	  BEACON_ARG_STR,	},
  ///<DYING 先頭のポケモンが瀕死 33
  { GAMEBEACON_Set_Dying,                 BEACON_PSET_NICKNAME,	  BEACON_ARG_STR,	},
  ///<STATE_IS_ABNORMAL 先頭のポケモンが状態異常 34
  { GAMEBEACON_Set_StateIsAbnormal,       BEACON_PSET_NICKNAME,	  BEACON_ARG_STR,	},
  ///<USE_ITEM アイテムを使用 35
  { GAMEBEACON_Set_UseItem,               BEACON_PSET_ITEM,	      BEACON_ARG_U16,	},
  ///<FIELD_SKILL フィールド技を使用 36
  { GAMEBEACON_Set_FieldSkill,            BEACON_PSET_WAZA,	      BEACON_ARG_U16,	},
  ///<SODATEYA_EGG 育て屋から卵を引き取った 37
  { GAMEBEACON_Set_SodateyaEgg,           BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<EGG_HATCH タマゴが孵化した 38
  { GAMEBEACON_Set_EggHatch,              BEACON_PSET_MONSNO,	    BEACON_ARG_U16,	},
  ///<SHOPING 買い物中 39
  { GAMEBEACON_Set_Shoping,               BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<SUBWAY バトルサブウェイ挑戦中 40
  { GAMEBEACON_Set_Subway,                BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<SUBWAY_STRAIGHT_VICTORIES バトルサブウェイ連勝中 41
  { GAMEBEACON_Set_SubwayStraightVictories, BEACON_PSET_VICTORY,  BEACON_ARG_U32,	},
  ///<SUBWAY_TROPHY_GET バトルサブウェイトロフィーを貰った 42
  { GAMEBEACON_Set_SubwayTrophyGet,       BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<TRIALHOUSE トライアルハウスに挑戦中 43
  { GAMEBEACON_Set_TrialHouse,            BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<TRIALHOUSE_RANK トライアルハウスでランク確定 44
  { GAMEBEACON_Set_TrialHouseRank,        BEACON_PSET_TH_RANK,	  BEACON_ARG_U8,	},
  ///<FERRIS_WHEEL 観覧車に乗った 45
  { GAMEBEACON_Set_FerrisWheel,           BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<POKESHIFTER ポケシフターに入った 46
  { GAMEBEACON_Set_PokeShifter,           BEACON_PSET_DEFAULT,	  BEACON_ARG_NONE,	},
  ///<MUSICAL ミュージカル挑戦中 47
  { GAMEBEACON_Set_Musical,               BEACON_PSET_NICKNAME,	  BEACON_ARG_NONE,	},
  ///<OTHER_GPOWER_USE 他人のGパワーを使用 48
  { GAMEBEACON_Set_OtherGPowerUse,        BEACON_PSET_GPOWER,	    BEACON_ARG_GPOWER,	},
  ///<FREEWORD 一言メッセージ 49
  { GAMEBEACON_Set_FreeWord,              BEACON_PSET_FREEWORD,	  BEACON_ARG_FWORD,	},
  ///<DISTRIBUTION_GPOWER Gパワー配布中 50 
  { GAMEBEACON_Set_DistributionGPower,    BEACON_PSET_GPOWER, 	  BEACON_ARG_GPOWER,	},
};

static const  D_BEACON_PRM_SET DATA_DebugBeaconParamSet[BEACON_PSET_MAX] = {
 { -1, -1, BPRM_WSET_NONE,}, //BEACON_PSET_DEFAULT, デフォルト(トレーナー名)  
 { 1, TRID_MAX-1, BPRM_WSET_TRNAME }, //BEACON_PSET_TR_ID,      対戦相手名
 { MONSNO_HUSIGIDANE, MONSNO_END, BPRM_WSET_MONSNAME }, //BEACON_PSET_MONSNO,    ポケモン種族名
 { -1, -1, BPRM_WSET_NONE }, //BEACON_PSET_NICKNAME,    ポケモンニックネーム
 { ITEM_MASUTAABOORU, ITEM_DATA_MAX, BPRM_WSET_ITEMNAME }, //BEACON_PSET_ITEM,        アイテム名
 { 0, 999, BPRM_WSET_NONE }, //BEACON_PSET_PTIME,       プレイタイム
 { 0, CROSS_COMM_SURETIGAI_COUNT_MAX, BPRM_WSET_NONE }, //BEACON_PSET_SURETIGAI_CT, すれ違い回数
 { 0, CROSS_COMM_THANKS_RECV_COUNT_MAX, BPRM_WSET_NONE }, //BEACON_PSET_THANKS, 御礼回数
 { 0, BREQ_FWAZA_MAX, BPRM_WSET_FWAZA }, //BEACON_PSET_WAZA,        技名
 { 0, 6, BPRM_WSET_NONE }, //BEACON_PSET_VICTORY,     サブウェイ挑戦中の連勝数1-7
 { 0, 5, BPRM_WSET_NONE }, //BEACON_PSET_TH_RANK,     トライアルハウスランク
 { 0, GPOWER_ID_CAPTURE_MAX, BPRM_WSET_GPOWER }, //BEACON_PSET_GPOWER,      Gパワー名
 { 0, 65535, BPRM_WSET_NONE }, //BEACON_PSET_ID,     プレイヤーID
 { -1, -1, BPRM_WSET_NONE }, //BEACON_PSET_FREEWORD,    フリーワード8文字
};

static const D_FAKE_PRM_SET DATA_FakeParamSet[] = {
  { BPRM_WSET_NONE, 5, 0, 0xFFFF,}, //FAKE_ID,
  { BPRM_WSET_TRNAME_EX, 3, 0, TRID_MAX-1,}, //FAKE_NAME,
  { BPRM_WSET_ZONE, 3, 0, ZONE_ID_MAX-1,}, //FAKE_ZONE,
  { BPRM_WSET_SEX, 1, PM_MALE, PM_FEMALE,}, //FAKE_SEX,
  { BPRM_WSET_VERSION, 2, VERSION_WHITE, VERSION_BLACK+1,}, //FAKE_VERSION,
  { BPRM_WSET_LANG, 1, LANG_JAPAN, LANG_KOREA,}, //FAKE_LANG,
};

////////////////////////////////////////////////
//フィールド技リスト
static const u16 DATA_FieldWazaTbl[BREQ_FWAZA_MAX] = {
  WAZANO_IAIGIRI,
  WAZANO_SORAWOTOBU,
  WAZANO_NAMINORI,
  WAZANO_KAIRIKI,
  WAZANO_ANAWOHORU,
  WAZANO_TEREPOOTO,
  WAZANO_TAKINOBORI,
  WAZANO_HURASSYU,
  WAZANO_DAIBINGU,
};

//======================================================================
//
//プロトタイプ
//
//======================================================================

static GMEVENT_RESULT event_LiveCommMain( GMEVENT * event, int *seq, void * work);
static void dmenu_WorkInit( DMENU_LIVE_COMM* wk );
static void dmenu_WorkRelease( DMENU_LIVE_COMM* wk );

static void sub_BmpWinAdd( BMP_WIN* bmpwin, u8 px, u8 py, u8 sx, u8 sy );
static void sub_BmpWinDel( BMP_WIN* bmpwin );
static void sub_GetMsgToBuf( DMENU_LIVE_COMM* wk, u8 msg_id );
static inline void sub_FreeWordset( WORDSET* wset, u8 idx, STRBUF* str );
static void sub_BeaconParamWordset( DMENU_LIVE_COMM* wk, u8 idx, int value, BPRM_WSET_TYPE type );

static GMEVENT* event_CreateEventNumInput( DMENU_LIVE_COMM* wk, int key, int min, int max, NINPUT_SET_FUNC set_func, NINPUT_GET_FUNC get_func );
static GMEVENT* event_CreateEventNumInputRetWork( DMENU_LIVE_COMM* wk, int key, int min, int max, int* ret_wk, BPRM_WSET_TYPE wset_type );
static GMEVENT* event_CreateEventStackCheck( GAMESYS_WORK* gsys, DMENU_LIVE_COMM* wk );
static GMEVENT* event_CreateEventBeaconReq( GAMESYS_WORK* gsys, DMENU_LIVE_COMM* wk );

static int ninput_GetMiscCount( DMENU_LIVE_COMM* wk, int param );
static void ninput_SetMiscCount( DMENU_LIVE_COMM* wk, int param, int value );

//======================================================================
//
//関数定義
//
//======================================================================
//--------------------------------------------------------------
/**
 * すれ違い通信デバッグメニューイベント生成
 * @param wk  DEBUG_MENU_EVENT_WORK*
 */
//--------------------------------------------------------------
GMEVENT* DEBUG_EVENT_LiveComm( GAMESYS_WORK* gsys, void* parent_work )
{
  DMENU_LIVE_COMM* wk;
  GMEVENT* event;

  event = GMEVENT_Create( gsys, NULL, event_LiveCommMain, sizeof(DMENU_LIVE_COMM) );

  wk = GMEVENT_GetEventWork( event );
  wk->gsys = gsys; 
  wk->gdata = GAMESYSTEM_GetGameData( gsys ); 
  wk->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  wk->save = GAMEDATA_GetSaveControlWork( wk->gdata ); 
  wk->int_sv = SaveData_GetIntrude( wk->save );
  wk->item_sv = GAMEDATA_GetMyItem( wk->gdata );
  wk->misc_sv = (MISC*)SaveData_GetMiscConst( wk->save );
  wk->my_status = GAMEDATA_GetMyStatus( wk->gdata );

  wk->view_wk = (BEACON_VIEW*)parent_work;

  wk->heapID = FIELDMAP_GetHeapID( wk->fieldmap );
  wk->tmpHeapID = GFL_HEAP_LOWID( wk->heapID );
  
  dmenu_WorkInit( wk );

  return event;
}

//-----------------------------------------------------------------
/*
 *  @brief  すれ違いデバッグメニューワーク初期化
 */
//-----------------------------------------------------------------
static void dmenu_WorkInit( DMENU_LIVE_COMM* wk )
{
  int i;

  wk->fontHandle = FLDMSGBG_GetFontHandle( FIELDMAP_GetFldMsgBG( wk->fieldmap ) );

  wk->wset = WORDSET_Create( wk->heapID );
  wk->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
      ARCID_MESSAGE, NARC_message_d_livecomm_dat, wk->heapID );
  
  wk->str_tmp = GFL_STR_CreateBuffer( BUFLEN_TMP_MSG, wk->heapID );
  wk->str_expand = GFL_STR_CreateBuffer( BUFLEN_TMP_MSG, wk->heapID );

  wk->pAllZoneName = ZONEDATA_GetAllZoneName( wk->heapID );

  wk->beacon_nickname = GFL_STR_CreateBuffer( BUFLEN_BEACON_MSG, wk->heapID );
  wk->beacon_fword = GFL_STR_CreateBuffer( BUFLEN_BEACON_MSG, wk->heapID );

  //すれ違いデータ初期化
  {
    POKEPARTY * party = GAMEDATA_GetMyPokemon( wk->gdata );
    u32 party_max = PokeParty_GetPokeCount( party );
    u8 party_top = PokeParty_GetMemberTopIdxNotEgg( party );
    POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, PokeParty_GetMemberTopIdxNotEgg( party ) );

    //ニックネーム初期値
    PP_Get( pp, ID_PARA_nickname, wk->beacon_nickname );

    //フリーワード初期値
    GFL_STR_SetStringCodeOrderLength( wk->beacon_fword,
      MISC_CrossComm_GetSelfIntroduction( wk->misc_sv ), 8+EOM_SIZE );

    for(i = 0;i < BEACON_PSET_MAX;i++){
      const D_BEACON_PRM_SET* prm_set = &DATA_DebugBeaconParamSet[i];
      if( prm_set->min >= 0 ){
        wk->beacon_prm[ i ] = prm_set->min;
      }
    }
  }
  //フェイクデータ初期化
  {
    int* fake = &wk->fake_prm[TR_PAT_MINE][0];
    
    fake[FAKE_ID] = MyStatus_GetID_Low( wk->my_status );
    fake[FAKE_NAME] = 1;
    fake[FAKE_ZONE] = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork( wk->gdata ));
    fake[FAKE_SEX] = MyStatus_GetMySex( wk->my_status );
    fake[FAKE_VERSION] = PM_VERSION;
    fake[FAKE_LANG] = PM_LANG;

    MI_CpuCopy8( fake, &(wk->fake_prm[TR_PAT_FAKE][0]),sizeof(int)*FAKE_DATA_MAX);
    MI_CpuCopy8( fake, &(wk->fake_prm[TR_PAT_RND][0]),sizeof(int)*FAKE_DATA_MAX);
  }
  wk->tmpInfo = GAMEBEACON_Alloc( wk->heapID );
}

//-----------------------------------------------------------------
/*
 *  @brief  すれ違いデバッグメニューワーク解放
 */
//-----------------------------------------------------------------
static void dmenu_WorkRelease( DMENU_LIVE_COMM* wk )
{
  GFL_HEAP_FreeMemory( wk->tmpInfo );
  
  GFL_STR_DeleteBuffer( wk->beacon_fword );
  GFL_STR_DeleteBuffer( wk->beacon_nickname);
  
  GFL_HEAP_FreeMemory( (void*)wk->pAllZoneName );

  GFL_STR_DeleteBuffer( wk->str_expand );
  GFL_STR_DeleteBuffer( wk->str_tmp );

  GFL_MSG_Delete( wk->msgman );
  WORDSET_Delete( wk->wset );

  //すれ違い通信バッファを元に戻す
  GAMEBEACON_Setting( wk->gdata );
}

//--------------------------------------------------------------
/*
 *  @brief  すれ違いデバッグメニューメインイベント
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_LiveCommMain( GMEVENT * event, int *seq, void * work)
{
  GAMESYS_WORK * gsys = GMEVENT_GetGameSysWork(event);
  DMENU_LIVE_COMM* wk = (DMENU_LIVE_COMM*)work;

  switch(*seq)
  {
  case SEQ_MENU_INIT:
    wk->menuFunc = DEBUGFLDMENU_Init( wk->fieldmap, wk->heapID,  &DebugLiveCommMenuData );
    (*seq)++;
    break;
  case SEQ_MENU_MAIN:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( wk->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }

      FLDMENUFUNC_DeleteMenu( wk->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL || ret == MENU_EXIT ){  //キャンセル
        *seq = SEQ_EXIT; 
        break;
      }
      switch( ret ){
      case MENU_BEACON_REQ:
        GMEVENT_CallEvent( event, event_CreateEventBeaconReq( wk->gsys, wk ) );
        break;
      case MENU_STACK_CHECK:
        GMEVENT_CallEvent( event, event_CreateEventStackCheck( wk->gsys, wk ) );
        break;
      case MENU_SURETIGAI_NUM:
        GMEVENT_CallEvent( event, event_CreateEventNumInput( wk, 0, wk->view_wk->ctrl.max, CROSS_COMM_SURETIGAI_COUNT_MAX,
            ninput_SetMiscCount, ninput_GetMiscCount) );
        break;
      case MENU_THANKS_NUM:
        GMEVENT_CallEvent( event, event_CreateEventNumInput( wk, 1, 0, CROSS_COMM_THANKS_RECV_COUNT_MAX,
            ninput_SetMiscCount, ninput_GetMiscCount) );
        break;
      case MENU_STACK_CLEAR:
        GAMEBEACON_InfoTbl_Clear( wk->view_wk->infoStack );
        break;
      case MENU_MEMBER_CLEAR:
        DEBUG_BEACON_VIEW_MemberListClear( wk->view_wk );
        break;
      case MENU_COMM_BUF_CLEAR:
        DEBUG_RecvBeaconBufferClear();
        break;
      }
      *seq = SEQ_MENU_INIT; 
    }
    break;
  case SEQ_EXIT:
  default:
    dmenu_WorkRelease( wk );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//-----------------------------------------------------------------
/*
 *  @brief  ウィンドウ追加 
 */
//-----------------------------------------------------------------
static void sub_BmpWinAdd( BMP_WIN* bmpwin, u8 px, u8 py, u8 sx, u8 sy )
{
  bmpwin->win = GFL_BMPWIN_Create( FLDBG_MFRM_MSG,
		px, py, sx, sy, PANO_FONT, GFL_BMP_CHRAREA_GET_B );

  bmpwin->bmp = GFL_BMPWIN_GetBmp( bmpwin->win );
  bmpwin->frm = GFL_BMPWIN_GetFrame( bmpwin->win );
  
  GFL_BMP_Clear( bmpwin->bmp, FCOL_WIN_BASE );
	GFL_BMPWIN_MakeScreen( bmpwin->win );
	GFL_BMPWIN_TransVramCharacter( bmpwin->win );
	GFL_BG_LoadScreenV_Req( bmpwin->frm );
}

//-----------------------------------------------------------------
/*
 *  @brief  ウィンドウ削除
 */
//-----------------------------------------------------------------
static void sub_BmpWinDel( BMP_WIN* bmpwin )
{
  GFL_BMPWIN_ClearScreen( bmpwin->win );
	GFL_BG_LoadScreenV_Req( bmpwin->frm );

  GFL_BMPWIN_Delete( bmpwin->win );
}

//----------------------------------------------------------
/*
 *  @brief  文字列整形
 *
 *  指定メッセージを wk->str_expandにつめる
 */
//----------------------------------------------------------
static void sub_GetMsgToBuf( DMENU_LIVE_COMM* wk, u8 msg_id )
{
  GFL_MSG_GetString( wk->msgman, msg_id, wk->str_tmp);
  WORDSET_ExpandStr( wk->wset, wk->str_expand, wk->str_tmp );
}

//----------------------------------------------------------
/*
 *  @brief  フリーワード展開
 */
//----------------------------------------------------------
static inline void sub_FreeWordset( WORDSET* wset, u8 idx, STRBUF* str )
{
  WORDSET_RegisterWord( wset, idx, str, PM_NEUTRAL, TRUE, PM_LANG );
}

//----------------------------------------------------------
/*
 *  ゾーンID文字列展開
 */
//----------------------------------------------------------
static void sub_GetZoneIDStr( const char * allName, u32 zoneID, STRBUF * strBuf )
{
  u16 pStrCode[64];
  
  DEB_STR_CONV_SjisToStrcode( &allName[ ZONEDATA_NAME_LENGTH * zoneID ], pStrCode, 64 );
  GFL_STR_SetStringCode( strBuf, pStrCode );
}

//----------------------------------------------------------
/*
 *  @brief  指定パラメータワード展開
 */
//----------------------------------------------------------
static void sub_BeaconParamWordset( DMENU_LIVE_COMM* wk, u8 idx, int value, BPRM_WSET_TYPE type )
{
  switch( type ){
  case BPRM_WSET_TRNAME:
    WORDSET_RegisterTrainerName( wk->wset, idx, value );
    return;
  case BPRM_WSET_TRNAME_EX:
    if( value == 0){
      MyStatus_CopyNameString( wk->my_status, wk->str_tmp );
    }else{
      WORDSET_RegisterTrainerName( wk->wset, idx, value );
      return;
    }
    break;
  case BPRM_WSET_MONSNAME:
    WORDSET_RegisterPokeMonsNameNo( wk->wset, idx, value );
    return;
  case BPRM_WSET_ITEMNAME:
    WORDSET_RegisterItemName( wk->wset, idx, value );
    return;
  case BPRM_WSET_FWAZA:
    WORDSET_RegisterWazaName( wk->wset, idx, DATA_FieldWazaTbl[value]);
    return;
  case BPRM_WSET_GPOWER:
    WORDSET_RegisterGPowerName( wk->wset, idx, value );
    return;
  case BPRM_WSET_ACTION:
    GFL_MSG_GetString( wk->msgman, dlc_action_00+value, wk->str_tmp );
    break;
  case BPRM_WSET_BEACON_REQ_TRAINER:
    GFL_MSG_GetString( wk->msgman, dlc_beacon_req_trainer01+value, wk->str_tmp );
    break;
  case BPRM_WSET_SEX:
    GFL_MSG_GetString( wk->msgman, dlc_sex_male+value, wk->str_tmp );
    break;
  case BPRM_WSET_VERSION:
    GFL_MSG_GetString( wk->msgman, dlc_version_black+(value-VERSION_BLACK), wk->str_tmp );
    break;
  case BPRM_WSET_LANG:
    GFL_MSG_GetString( wk->msgman, dlc_lang_01+(value-LANG_JAPAN), wk->str_tmp );
    break;
  case BPRM_WSET_ZONE:
    WORDSET_RegisterPlaceName( wk->wset, 7, ZONEDATA_GetPlaceNameID(value) );
    sub_GetMsgToBuf( wk, dlc_tmp_buf );
    sub_GetZoneIDStr( wk->pAllZoneName, value, wk->str_tmp );
    GFL_STR_AddString( wk->str_tmp, wk->str_expand );
    break;
  }
  sub_FreeWordset( wk->wset, idx, wk->str_tmp );
}

//----------------------------------------------------------
/*
 *  @brief  STRBUFからSTRCODE[]にコピー
 */
//----------------------------------------------------------
static void sub_GetStringCode( STRBUF* buf, STRCODE* code, int len )
{
  STRCODE code_eom = GFL_STR_GetEOMCode();
  const STRCODE *src_code;
  int i;
 
  src_code = GFL_STR_GetStringCodePointer(buf);
  for(i = 0; i < len; i++){
    code[i] = src_code[i];
    if(src_code[i] == code_eom){
      break;
    }
  }
}

//----------------------------------------------------------
/*
 *  @brief  GAMEBEACON_INFOにパーソナルデータを展開
 */
//----------------------------------------------------------
static void sub_BeaconInfoSet( DMENU_LIVE_COMM* wk, TR_PATTERN pat )
{
  int i;
  GAMEBEACON_INFO* info = DEBUG_SendBeaconInfo_GetPtr();
  int* fake = &wk->fake_prm[pat][0]; 

  GAMEBEACON_Setting( wk->gdata );
  switch(pat){
  case TR_PAT_MINE:
    return;
  case TR_PAT_FAKE:
    break;
  case TR_PAT_RND:
    for(i = 0;i < FAKE_DATA_MAX;i++){
      int range;
      const D_FAKE_PRM_SET* f_prm = &DATA_FakeParamSet[i];

      range = f_prm->max-f_prm->min;
      fake[i] = (GFUser_GetPublicRand0( 0xFFFFFFFF )%range)+f_prm->min;
    }
    break;
  }
  //id
  info->trainer_id = (u16)fake[FAKE_ID];
  //name
  sub_BeaconParamWordset( wk, 7, fake[FAKE_NAME], BPRM_WSET_TRNAME_EX );
  sub_GetMsgToBuf( wk, dlc_tmp_buf );
  sub_GetStringCode( wk->str_expand, info->name, PERSON_NAME_SIZE);
  //zone
  info->zone_id = fake[FAKE_ZONE];
  info->townmap_root_zone_id = TOWNMAP_UTIL_GetRootZoneID( wk->gdata, info->zone_id);
  //sex
  info->sex = fake[FAKE_SEX];
  //pm_version
  info->pm_version = fake[FAKE_VERSION];
  //language
  info->language = fake[FAKE_LANG];
}

//===================================================================================
//数値入力イベント
//===================================================================================
typedef struct _EVWK_NINPUT{
  DMENU_LIVE_COMM* dlc_wk;

  u8    decide_f;
  int   key;
  int   value;

  BPRM_WSET_TYPE wset_type;

  NINPUT_PARAM  prm;
  BMP_WIN win;
}EVWK_NINPUT;

static GMEVENT_RESULT event_NumInputMain( GMEVENT * event, int *seq, void * work);
static void ninput_PrintNumWin( DMENU_LIVE_COMM* wk, EVWK_NINPUT* evwk, BMP_WIN* bmpwin, int num );

//--------------------------------------------------------------
/**
 * 数値入力イベント生成コア
 * @param wk  DEBUG_MENU_EVENT_WORK*
 */
//--------------------------------------------------------------
static GMEVENT* event_CreateEventNumInputCore( DMENU_LIVE_COMM* wk,
    int key, int min, int max, NINPUT_SET_FUNC set_func, NINPUT_GET_FUNC get_func,
    int* ret_wk, BPRM_WSET_TYPE wset_type )
{
  EVWK_NINPUT* evwk;
  GMEVENT* event;

  event = GMEVENT_Create( wk->gsys, NULL, event_NumInputMain, sizeof(EVWK_NINPUT) );

  evwk = GMEVENT_GetEventWork( event );
  evwk->dlc_wk = wk; 

  evwk->prm.min = min;
  evwk->prm.max = max;
  evwk->prm.ret_wk = ret_wk;
  evwk->prm.set_func = set_func;
  evwk->prm.get_func = get_func;

  evwk->wset_type = wset_type;
  evwk->key = key;

  if( ret_wk != NULL ){
    evwk->value = *ret_wk;
  }else{
    evwk->value = (get_func)( wk, key );
  }
  return event;
}

//--------------------------------------------------------------
/**
 * 数値入力イベント生成
 * @param wk  DEBUG_MENU_EVENT_WORK*
 */
//--------------------------------------------------------------
static GMEVENT* event_CreateEventNumInput( DMENU_LIVE_COMM* wk, int key, int min, int max, NINPUT_SET_FUNC set_func, NINPUT_GET_FUNC get_func )
{
  return event_CreateEventNumInputCore( wk,
            key, min, max, set_func, get_func, NULL, BPRM_WSET_NONE );
}
static GMEVENT* event_CreateEventNumInputRetWork( DMENU_LIVE_COMM* wk, int key, int min, int max, int* ret_wk, BPRM_WSET_TYPE wset_type )
{
  return event_CreateEventNumInputCore( wk,
            key, min, max, NULL, NULL, ret_wk, wset_type );
}

//--------------------------------------------------------------
/*
 *  @brief  数値入力イベント
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_NumInputMain( GMEVENT * event, int *seq, void * work)
{
  EVWK_NINPUT* evwk = (EVWK_NINPUT*)work;
  DMENU_LIVE_COMM* wk = evwk->dlc_wk;
  NINPUT_PARAM* prm = &evwk->prm;

  switch(*seq)
  {
  case 0:
    sub_BmpWinAdd( &evwk->win, 1, 24-2, 30, 2 );
    ninput_PrintNumWin( wk, evwk, &evwk->win, evwk->value );
    (*seq)++;
    break;
  case 1:
    {
      int trg = GFL_UI_KEY_GetTrg();
      int cont = GFL_UI_KEY_GetCont();
      int diff;
      int before, after;
      
      if( trg & PAD_BUTTON_B ){
        (*seq)++;
        break;
      }
      if( trg & PAD_BUTTON_A ){
        if( prm->ret_wk != NULL ){
          *prm->ret_wk = evwk->value;
        }else{
          (prm->set_func)( wk, evwk->key, evwk->value );
        }
        (*seq)++;
        break;
      }
      after = before = evwk->value;
      if( evwk->value < prm->min || evwk->value > prm->max ){
        break;
      }
      diff = 0;
      if (trg & PAD_KEY_UP){
        diff = 1;
      } else if (trg & PAD_KEY_DOWN) {
        diff = -1;
      } else if (trg & PAD_KEY_LEFT){
        diff = -10;
      } else if (trg & PAD_KEY_RIGHT){
        diff = 10;
      }
      if( cont & PAD_BUTTON_R ){
        diff *= 10;
      }else if( cont & PAD_BUTTON_L ){
        diff *= 100;
      }
      if(diff == 0){
        break;
      }
      if( (diff < 0) && ( (after-prm->min) < (diff*-1))){
        if( before > prm->min ){
          after = prm->min; //いったんmin
        }else{
          after = prm->max; //回り込み
        }
      }else if( (diff > 0) && ((prm->max-after) < diff) ){
        if( before < prm->max ){
          after = prm->max; //いったんmax
        }else{
          after = prm->min; //回り込み
        }
      }else{
        after = before+diff;
      }
      if (after != before ) {
        ninput_PrintNumWin( wk, evwk, &evwk->win, after );
        evwk->value = after;
      }
    }
    break;
  default:
    sub_BmpWinDel( &evwk->win );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/// 数値入力ウィンドウ：表示更新
//--------------------------------------------------------------
static void ninput_PrintNumWin( DMENU_LIVE_COMM* wk, EVWK_NINPUT* evwk, BMP_WIN* bmpwin, int num )
{
  WORDSET_RegisterNumber(wk->wset, 0, num, 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

  if( evwk->wset_type == BPRM_WSET_NONE ){
    sub_GetMsgToBuf( wk, dlc_num_input01 );
  }else{
    sub_BeaconParamWordset( wk, 1, num, evwk->wset_type );
    sub_GetMsgToBuf( wk, dlc_num_input02 );
  }

  GFL_BMP_Clear( bmpwin->bmp, FCOL_WIN_BASE );
	PRINTSYS_PrintColor( bmpwin->bmp, 0, 0, wk->str_expand, wk->fontHandle, FCOL_WIN_W );

  GFL_BMPWIN_TransVramCharacter( bmpwin->win );  //transfer characters
	GFL_BG_LoadScreenV_Req( bmpwin->frm );  //transfer screen
}

//===================================================================================
//スタックチェックイベント
//===================================================================================
typedef struct _EVWK_STACK_CHECK{
  DMENU_LIVE_COMM* dlc_wk;

  u8      page;
  BMP_WIN win;
}EVWK_STACK_CHECK;

static GMEVENT_RESULT event_StackCheckMain( GMEVENT * event, int *seq, void * work);
static void scheck_ParamDraw( DMENU_LIVE_COMM* wk, EVWK_STACK_CHECK* evwk, u8 idx, u8 line );
static void scheck_PageDraw( DMENU_LIVE_COMM* wk, EVWK_STACK_CHECK* evwk, u8 page );

//--------------------------------------------------------------
/**
 * スタックチェックイベント生成
 * @param wk  DEBUG_MENU_EVENT_WORK*
 */
//--------------------------------------------------------------
static GMEVENT* event_CreateEventStackCheck( GAMESYS_WORK* gsys, DMENU_LIVE_COMM* wk )
{
  EVWK_STACK_CHECK* evwk;
  GMEVENT* event;

  event = GMEVENT_Create( gsys, NULL, event_StackCheckMain, sizeof(EVWK_STACK_CHECK) );

  evwk = GMEVENT_GetEventWork( event );
  evwk->dlc_wk = wk; 

  return event;
}

//--------------------------------------------------------------
/*
 *  @brief  スタックチェックイベント
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_StackCheckMain( GMEVENT * event, int *seq, void * work)
{
  EVWK_STACK_CHECK* evwk = (EVWK_STACK_CHECK*)work;
  DMENU_LIVE_COMM* wk = evwk->dlc_wk;

  switch(*seq)
  {
  case 0:
    sub_BmpWinAdd( &evwk->win, 0, 0, 32, 24 );
    scheck_PageDraw( wk, evwk, evwk->page );
    (*seq)++;
    break;
  case 1:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & PAD_BUTTON_B){
        (*seq)++;
      }else if( trg & PAD_KEY_LEFT ){
        evwk->page = (evwk->page+(SCHECK_PAGE_MAX-1))%SCHECK_PAGE_MAX;
        scheck_PageDraw( wk, evwk, evwk->page );
      }else if( trg & PAD_KEY_RIGHT ){
        evwk->page = (evwk->page+1)%SCHECK_PAGE_MAX;
        scheck_PageDraw( wk, evwk, evwk->page );
      }
    }
    break;
  default:
    sub_BmpWinDel( &evwk->win );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/*
 *  @brief  スタック情報書き出し
 */
//--------------------------------------------------------------
static void scheck_ParamDraw( DMENU_LIVE_COMM* wk, EVWK_STACK_CHECK* evwk, u8 idx, u8 line )
{
  u8 msg_id;
  GAMEBEACON_ACTION action;

  WORDSET_RegisterNumber( wk->wset, 0, idx, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

  if( GAMEBEACON_InfoTbl_GetBeacon( wk->view_wk->infoStack, wk->tmpInfo, &wk->tmpTime, idx)){
    msg_id = dlc_stack_check_param01;
  
    //プレイヤー名取得
    GAMEBEACON_Get_PlayerNameToBuf( wk->tmpInfo, wk->str_tmp );
    sub_FreeWordset( wk->wset, 1, wk->str_tmp );

    //トレーナーID
    WORDSET_RegisterNumber( wk->wset, 2, GAMEBEACON_Get_TrainerID( wk->tmpInfo ), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

    //アクション
    action = GAMEBEACON_Get_Action_ActionNo( wk->tmpInfo );
    WORDSET_RegisterNumber( wk->wset, 3, action, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
    GFL_MSG_GetString( wk->msgman, dlc_action_00+action, wk->str_tmp);
    sub_FreeWordset( wk->wset, 4, wk->str_tmp );

  }else{
    msg_id = dlc_stack_check_param02;
  }
  sub_GetMsgToBuf( wk, msg_id );
	PRINTSYS_PrintColor( evwk->win.bmp, 0, line*SCHECK_LINE_OY, wk->str_expand, wk->fontHandle, FCOL_WIN_W );
}

//--------------------------------------------------------------
/*
 *  @brief  スタック情報書き出し
 */
//--------------------------------------------------------------
static void scheck_PageDraw( DMENU_LIVE_COMM* wk, EVWK_STACK_CHECK* evwk, u8 page )
{
  int i,idx;
  GFL_BMP_Clear( evwk->win.bmp, FCOL_WIN_BASE );

  idx = page*SCHECK_LINE_MAX;
  for(i = 0;i < SCHECK_LINE_MAX;i++){
    if( idx+i >= BS_LOG_MAX ){
      break;
    }
    scheck_ParamDraw( wk, evwk, idx+i, i );
  }
	GFL_BMPWIN_TransVramCharacter( evwk->win.win );
	GFL_BG_LoadScreenV_Req( evwk->win.frm );
}

//===================================================================================
//ビーコン送信ユーティリティイベント
//===================================================================================
typedef struct _EVWK_BEACON_REQ{
  DMENU_LIVE_COMM* dlc_wk;

  u8      line;
  int     type;
  int     action;

  const D_BEACON_PARAM* b_prm;
  const D_BEACON_PRM_SET* b_prm_set;

  BMP_WIN win_main;
  BMP_WIN win_line;
}EVWK_BEACON_REQ;

static GMEVENT_RESULT event_BeaconReqMain( GMEVENT * event, int *seq, void * work);
static void breq_PrintCursor( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk, u8 line );
static void breq_PrintLine( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk, u8 line, STRBUF* str );
static void breq_DrawBeaconType( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk );
static void breq_DrawAction( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk );
static void breq_DrawFake( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk );
static void breq_BeaconSend( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk );

//--------------------------------------------------------------
/**
 * ビーコン送信イベント生成
 * @param wk  DEBUG_MENU_EVENT_WORK*
 */
//--------------------------------------------------------------
static GMEVENT* event_CreateEventBeaconReq( GAMESYS_WORK* gsys, DMENU_LIVE_COMM* wk )
{
  EVWK_BEACON_REQ* evwk;
  GMEVENT* event;

  event = GMEVENT_Create( gsys, NULL, event_BeaconReqMain, sizeof(EVWK_BEACON_REQ) );

  evwk = GMEVENT_GetEventWork( event );
  evwk->dlc_wk = wk; 

  evwk->action = GAMEBEACON_ACTION_BATTLE_WILD_POKE_START;
  return event;
}

//--------------------------------------------------------------
/*
 *  @brief  ビーコン送信イベント
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_BeaconReqMain( GMEVENT * event, int *seq, void * work)
{
  EVWK_BEACON_REQ* evwk = (EVWK_BEACON_REQ*)work;
  DMENU_LIVE_COMM* wk = evwk->dlc_wk;

  switch(*seq)
  {
  case 0:
    sub_BmpWinAdd( &evwk->win_main, 1, 0, 31, 22 );
    sub_BmpWinAdd( &evwk->win_line, 0, 0, 1, 22 );
    breq_PrintCursor( wk, evwk, evwk->line );
    (*seq)++;
    break;
  case 1:
    evwk->b_prm = &DATA_DebugBeaconParam[ evwk->action ];
    evwk->b_prm_set = &DATA_DebugBeaconParamSet[ evwk->b_prm->prm_type ];
    breq_DrawBeaconType( wk, evwk );
    breq_DrawAction( wk, evwk );
    breq_DrawFake( wk, evwk );
    (*seq)++;
    break;
  case 2:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & PAD_BUTTON_B){
        (*seq) = 0xFF;
        break;
      }else if( trg & PAD_KEY_UP ){
        evwk->line = (evwk->line+(BREQ_LINE_MAX-1))%BREQ_LINE_MAX;
        breq_PrintCursor( wk, evwk, evwk->line );
        break;
      }else if( trg & PAD_KEY_DOWN ){
        evwk->line = (evwk->line+1)%BREQ_LINE_MAX;
        breq_PrintCursor( wk, evwk, evwk->line );
        break;
      }else if( trg & PAD_BUTTON_START ){
        (*seq)++;
        break;
      }

      if( (trg & PAD_BUTTON_A) == 0 ){
        break;
      }
      switch( evwk->line ){
      case 0:
        GMEVENT_CallEvent( event, event_CreateEventNumInputRetWork( wk, 0, 0, 2,
            &evwk->type, BPRM_WSET_BEACON_REQ_TRAINER));
        break;
      case 1:
        GMEVENT_CallEvent( event, event_CreateEventNumInputRetWork( wk, 0, 
            GAMEBEACON_ACTION_BATTLE_WILD_POKE_START, GAMEBEACON_ACTION_MAX-1,
            &evwk->action, BPRM_WSET_ACTION ));
        break;
      case 2:
        if( evwk->b_prm_set->min >= 0 ){
          GMEVENT_CallEvent( event, event_CreateEventNumInputRetWork( wk, 0, 
            evwk->b_prm_set->min, evwk->b_prm_set->max,
            &wk->beacon_prm[evwk->b_prm->prm_type], evwk->b_prm_set->wset_type ));
        }
      default:
        {
          u8 fake_id = evwk->line-3;
          const D_FAKE_PRM_SET* f_prm = &DATA_FakeParamSet[fake_id];
          GMEVENT_CallEvent( event, event_CreateEventNumInputRetWork( wk, 0, 
            f_prm->min, f_prm->max, &(wk->fake_prm[TR_PAT_FAKE][fake_id]),f_prm->wset_type ));
        }
        break;
      }
      (*seq) = 1;
    }
    break;
  case 3:
    breq_BeaconSend( wk, evwk );
    (*seq) = 1;
    break;
  default:
    sub_BmpWinDel( &evwk->win_line );
    sub_BmpWinDel( &evwk->win_main );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

static void breq_PrintCursor( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk, u8 line )
{
  GFL_BMP_Clear( evwk->win_line.bmp, FCOL_WIN_BASE );
  GFL_MSG_GetString( wk->msgman, dlc_cursor, wk->str_tmp );
	PRINTSYS_PrintColor( evwk->win_line.bmp, 0, line*16, wk->str_tmp, wk->fontHandle, FCOL_WIN_W );
	GFL_BMPWIN_TransVramCharacter( evwk->win_line.win );
}

static void breq_PrintLine( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk, u8 line, STRBUF* str )
{
  GFL_BMP_Fill( evwk->win_main.bmp, 0, line*16, BREQ_WIN_MAIN_SX*8, 16, FCOL_WIN_BASE );
	PRINTSYS_PrintColor( evwk->win_main.bmp, 0, line*16, str, wk->fontHandle, FCOL_WIN_W );
	GFL_BMPWIN_TransVramCharacter( evwk->win_main.win );
}

static void breq_DrawBeaconType( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk )
{
  GFL_MSG_GetString( wk->msgman, dlc_beacon_req_trainer01+evwk->type, wk->str_tmp );
  breq_PrintLine( wk, evwk, 0, wk->str_tmp );
}

static void breq_DrawAction( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk )
{
  int value;

  WORDSET_RegisterNumber( wk->wset, 0, evwk->action, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  sub_BeaconParamWordset( wk, 1, evwk->action, BPRM_WSET_ACTION );

  sub_GetMsgToBuf( wk, dlc_beacon_req_action );
  breq_PrintLine( wk, evwk, 1, wk->str_expand );

  //パラメータプリント
  value = wk->beacon_prm[evwk->b_prm->prm_type];

  WORDSET_RegisterNumber( wk->wset, 0, value,  5, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  sub_FreeWordset( wk->wset, 2, wk->beacon_nickname );
  sub_FreeWordset( wk->wset, 3, wk->beacon_fword );

  sub_BeaconParamWordset( wk, 1, value, evwk->b_prm_set->wset_type );
  sub_GetMsgToBuf( wk, dlc_beacon_req_param01+evwk->b_prm->prm_type );
  breq_PrintLine( wk, evwk, 2, wk->str_expand );
}

static void breq_DrawFakeLine( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk, FAKE_DATA_ID fake_id )
{
  u8 line = 3+fake_id;
  int* mine = &wk->fake_prm[ TR_PAT_MINE ][0];
  int* fake = &wk->fake_prm[ TR_PAT_FAKE ][0];
  const D_FAKE_PRM_SET* f_prm = &DATA_FakeParamSet[fake_id];

  WORDSET_RegisterNumber(wk->wset, 0, fake[fake_id], f_prm->keta, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterNumber(wk->wset, 1, mine[fake_id], f_prm->keta, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  sub_BeaconParamWordset( wk, 2, fake[fake_id], f_prm->wset_type );
  sub_BeaconParamWordset( wk, 3, mine[fake_id], f_prm->wset_type );
  
  sub_GetMsgToBuf( wk, dlc_fake_tr_id+fake_id );
  breq_PrintLine( wk, evwk, line, wk->str_expand );
}

static void breq_DrawFake( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk )
{
  int i;

  for(i = 0;i < FAKE_DATA_MAX;i++){
    breq_DrawFakeLine( wk, evwk, i );
  }
}

static void breq_BeaconSend( DMENU_LIVE_COMM* wk, EVWK_BEACON_REQ* evwk )
{
  int value = -1;

  sub_BeaconInfoSet( wk, evwk->type );

  if( evwk->b_prm_set->max >= 0 ){
    value = wk->beacon_prm[ evwk->b_prm->prm_type ];
  }
  switch( evwk->b_prm->arg_type ) {
  case	BEACON_ARG_NONE:
    ((BEACON_SET_FUNC_NONE)evwk->b_prm->func)();
    break;
  case	BEACON_ARG_U8:
    GF_ASSERT( value >= 0 );
    ((BEACON_SET_FUNC_U8)evwk->b_prm->func)( (u8)value );
    break;
  case	BEACON_ARG_U16:
    GF_ASSERT( value >= 0 );
    ((BEACON_SET_FUNC_U16)evwk->b_prm->func)( (u16)value );
    break;
  case	BEACON_ARG_U32:
    GF_ASSERT( value >= 0 );
    ((BEACON_SET_FUNC_U32)evwk->b_prm->func)( (u32)value );
    break;
  case	BEACON_ARG_STR:
    ((BEACON_SET_FUNC_STR)evwk->b_prm->func)( wk->beacon_nickname );
    break;
  case	BEACON_ARG_U16_STR:
    GF_ASSERT( value >= 0 );
    ((BEACON_SET_FUNC_U16_STR)evwk->b_prm->func)( (u16)value, wk->beacon_nickname );
    break;
  case	BEACON_ARG_GDATA_U32:
    GF_ASSERT( value >= 0 );
    ((BEACON_SET_FUNC_GDATA_U32)evwk->b_prm->func)( wk->gdata, (u32)value );
    break;
  case	BEACON_ARG_FWORD:
    ((BEACON_SET_FUNC_STR)evwk->b_prm->func)( wk->beacon_fword );
    break;
  case	BEACON_ARG_GPOWER:
    GF_ASSERT( value >= 0 );
    ((BEACON_SET_FUNC_GPOWER)evwk->b_prm->func)( (GPOWER_ID)value );
    break;
  }
}

//===========================================================================
//数値入力ゲッター＆セッター
//===========================================================================
//--------------------------------------------------------------
/*
 *  @brief  Misc関係　ゲット 
 */
//--------------------------------------------------------------
static int ninput_GetMiscCount( DMENU_LIVE_COMM* wk, int param )
{
  switch(param){
  case 0:
    return MISC_CrossComm_GetSuretigaiCount( wk->misc_sv );
  case 1:
    return MISC_CrossComm_GetThanksRecvCount( wk->misc_sv );
  }
  return 0;
}

//--------------------------------------------------------------
/*
 *  @brief  Misc関係　セット 
 */
//--------------------------------------------------------------
static void ninput_SetMiscCount( DMENU_LIVE_COMM* wk, int param, int value )
{
  switch(param){
  case 0:
    MISC_CrossComm_SetSuretigaiCount( wk->misc_sv, value );
    DEBUG_BEACON_VIEW_SuretigaiCountSet( wk->view_wk, value );
  case 1:
    MISC_CrossComm_SetThanksRecvCount( wk->misc_sv, value );
  }
}









#endif  //PM_DEBUG

