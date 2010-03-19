//======================================================================
/**
 *
 * @file  event_debug_menu.c
 * @brief フィールドデバッグメニューの制御イベント
 * @author  kagaya
 * @date  2008.11.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "arc_def.h"

#include "field/field_msgbg.h"

#include "message.naix"
#include "msg/msg_d_field.h"

#include "fieldmap.h"
#include "field/zonedata.h"
#include "arc/fieldmap/zone_id.h"
#include "field/eventdata_sxy.h"

#include "event_debug_menu.h"
#include "event_mapchange.h"
#include "event_gamestart.h"

#include "net_app/irc_match/ircbattlemenu.h"
#include "event_ircbattle.h"
#include "event_wificlub.h"
#include "event_gtsnego.h"
#include "field_subscreen.h"
#include "sound/pm_sndsys.h"

#include "font/font.naix"

#include  "field/weather_no.h"
#include  "weather.h"
#include  "msg/msg_d_tomoya.h"

#include "field_debug.h"

#include "field_event_check.h"
#include "event_debug_item.h" //EVENT_DebugItemMake
#include "event_debug_numinput.h"
#include "savedata/box_savedata.h"  //デバッグアイテム生成用
#include  "item/itemsym.h"  //ITEM_DATA_MAX
#include  "item/item.h"  //ITEM_CheckEnable
#include "app/townmap.h"
#include "net_app/worldtrade.h"
#include "../ui/debug/ui_template.h"
#include "savedata/shortcut.h"
#include "event_debug_beacon.h"
#include "app/waza_oshie.h"

#include "field_buildmodel.h"

#include "field_sound.h"

#include "script.h" //SCRIPT_ChangeScript
#include "../../../resource/fldmapdata/script/debug_scr_def.h"  //SCRID_DEBUG_COMMON
#include "../../../resource/fldmapdata/script/hiden_def.h"  //SCRID_HIDEN_DIVING

#include "eventwork.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h"

#include "event_wifibattlematch.h"

#include "field/field_comm/intrude_field.h" //PALACE_MAP_LEN

//CGEAR PICTURE
#include "c_gear.naix"
#include "./c_gear/c_gear.h"
#include "./c_gear/event_cgearget.h"
#include "savedata/save_tbl.h"
#include "savedata/c_gear_picture.h"

#include "event_debug_local.h"

#include "event_debug_season_display.h" // for DEBUG_EVENT_FLDMENU_SeasonDispSelect
#include "event_debug_demo3d.h" // for DEBUG_EVENT_FLDMENU_Demo3DSelect
#include "event_debug_menu_make_egg.h"  // for DEBUG_EVENT_FLDMENU_MakeEgg

#include "debug/debug_str_conv.h" // for DEB_STR_CONV_SJIStoStrcode

#include "event_debug_wifimatch.h"
#include "event_battlerecorder.h"
#include "event_debug_mvpoke.h"
#include "field_bbd_color.h"

#include "gamesystem/pm_weather.h"

#include "debug/debug_mystery_card.h"
#include "event_wifi_bsubway.h"
#include "event_bsubway.h"
#include "savedata/battle_box_save.h"
#include "event_geonet.h"
#include "app/name_input.h"
#include "waza_tool\wazano_def.h"

FS_EXTERN_OVERLAY( d_iwasawa );

//======================================================================
//  define
//======================================================================
#define DEBUG_MENU_PANO (14)
#define DEBUG_FONT_PANO (15)

#define D_MENU_CHARSIZE_X (18)    //メニュー横幅
#define D_MENU_CHARSIZE_Y (16)    //メニュー縦幅

#define LINER_CAM_KEY_WAIT    (30)
#define LINER_CAM_KEY_COUNT    (15)

//======================================================================
//  typedef struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static FLDMENUFUNC * DEBUGFLDMENU_InitExPos(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, u16 list_pos, u16 cursor_pos );
static void DebugMenu_IineCallBack(BMPMENULIST_WORK* lw,u32 param,u8 y);
static u32 DebugMenu_GetQuickJumpIdx( u32 strID );

static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk );

static BOOL debugMenuCallProc_OpenStartComm( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_OpenStartInvasion( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_MapZoneSelect( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_OpenCommDebugMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_OpenClubMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_OpenGTSNegoMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk );


static debugMenuCallProc_MapSeasonSelect( DEBUG_MENU_EVENT_WORK *wk );
static debugMenuCallProc_SubscreenSelect( DEBUG_MENU_EVENT_WORK *wk );
static debugMenuCallProc_MusicalSelect( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_EventFlagScript( DEBUG_MENU_EVENT_WORK *now_wk );
static BOOL debugMenuCallProc_ScriptSelect( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_GameEnd( DEBUG_MENU_EVENT_WORK * wk );

static BOOL debugMenuCallProc_ControlCamera( DEBUG_MENU_EVENT_WORK *wk );


static BOOL debugMenuCallProc_MMdlList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_ForceSave( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_ControlLight( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_ControlFog( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_WeatherList( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_CaptureList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_FieldPosData( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_ControlRtcList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_Naminori( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_DebugMakePoke( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugReWritePoke( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugItem( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_DebugSecretItem( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_DebugPDWItem( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_BoxMax( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_MyItemMax( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_SetBtlBox( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_ChangeName( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_DebugSkyJump( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_ChangePlayerSex( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_WifiGts( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_GDS( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_UITemplate( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Jump( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_NumInput( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_Kairiki( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_ControlLinerCamera( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_ControlDelicateCamera( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenu_ControlShortCut( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_BeaconFriendCode( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_WifiBattleMatch( DEBUG_MENU_EVENT_WORK *wk );
static GMEVENT_RESULT debugMenuWazaOshie( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
static BOOL debugMenuCallProc_WazaOshie( DEBUG_MENU_EVENT_WORK *p_wk );


static BOOL debugMenuCallProc_UseMemoryDump( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_SeasonDisplay( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugSake( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugAtlas( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Geonet( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_BattleRecorder( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Ananukenohimo( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Anawohoru( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Teleport( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Diving( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_Demo3d( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_DebugMvPokemon( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_BBDColor( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_MakeEgg( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_EncEffList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_DebugMakeUNData( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_BSubway( DEBUG_MENU_EVENT_WORK *wk );

static BOOL debugMenuCallProc_GPowerList( DEBUG_MENU_EVENT_WORK *wk );
static GMEVENT_RESULT debugMenuGPowerListEvent(GMEVENT *event, int *seq, void *wk );
static void DEBUG_SetMenuWorkGPower(GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list,
  HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work );
static u16 DEBUG_GetGPowerMax( GAMESYS_WORK* gsys, void* cb_work );

static BOOL debugMenuCallProc_FieldSkillList( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_MakeMysteryCardList( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_MakeMysteryCardPoke( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_MakeMysteryCardItem( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_MakeMysteryCardGPower( DEBUG_MENU_EVENT_WORK *p_wk );

static BOOL debugMenuCallProc_Zukan( DEBUG_MENU_EVENT_WORK *wk );
static BOOL debugMenuCallProc_DebugZoneJump( DEBUG_MENU_EVENT_WORK *p_wk );
static BOOL debugMenuCallProc_AllMapCheck( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_RingTone( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_EventPokeCreate( DEBUG_MENU_EVENT_WORK * p_wk );

//======================================================================
//  デバッグメニューリスト
//======================================================================
//--------------------------------------------------------------
/// デバッグメニューリスト
/// データを追加する事でメニューの項目も増えます。
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuList[] =
{
  { DEBUG_FIELD_TITLE_00, (void*)BMPMENULIST_LABEL },       //○ジャンプ（移動用）
  { DEBUG_FIELD_STR38, debugMenuCallProc_DebugSkyJump },    //空を飛ぶ
  { DEBUG_FIELD_STR43, debugMenuCallProc_Jump },            //ジャンプ（ユニオン等）
  { DEBUG_FIELD_STR05, debugMenuCallProc_MapZoneSelect },   //ゾーン選択ジャンプ
  { DEBUG_FIELD_STR06, debugMenuCallProc_MapSeasonSelect},  //四季ジャンプ
  { DEBUG_FIELD_STR05_b,   debugMenuCallProc_DebugZoneJump }, //ゾーンジャンプ
  { DEBUG_FIELD_STR09, debugMenuCallProc_AllMapCheck }, //オールマップチェック

  { DEBUG_FIELD_TITLE_02, (void*)BMPMENULIST_LABEL },       //○フィールド
  { DEBUG_FIELD_STR17, debugMenuCallProc_FieldPosData },    //座標をみる
  { DEBUG_FIELD_STR02, debugMenuCallProc_ControlLinerCamera },  //カメラ簡単操作
  { DEBUG_FIELD_STR52, debugMenuCallProc_ControlDelicateCamera }, //カメラ全部操作
  { DEBUG_FIELD_EVENT_CONTROL, debugMenuCallProc_EventFlagScript }, //イベント操作
  { DEBUG_FIELD_STR03, debugMenuCallProc_ScriptSelect },    //スクリプト実行
  { DEBUG_FIELD_STR13, debugMenuCallProc_MMdlList },        //モデルリスト
  { DEBUG_FIELD_STR15, debugMenuCallProc_ControlLight },      //ライト
  { DEBUG_FIELD_STR16, debugMenuCallProc_WeatherList },       //てんき
  { DEBUG_FIELD_STR36, debugMenuCallProc_ControlFog },        //フォグ操作
  { DEBUG_FIELD_STR_SUBSCRN, debugMenuCallProc_SubscreenSelect }, //下画面操作
  { DEBUG_FIELD_FSKILL, debugMenuCallProc_FieldSkillList },  //フィールド技（波乗り、怪力等）
  { DEBUG_FIELD_MVPOKE,   debugMenuCallProc_DebugMvPokemon },     //いどうポケモン
  { DEBUG_FIELD_STR62,   debugMenuCallProc_BBDColor },            //ビルボードの色
  { DEBUG_FIELD_ENCEFF, debugMenuCallProc_EncEffList },           //エンカウントエフェクト

  { DEBUG_FIELD_TITLE_01, (void*)BMPMENULIST_LABEL },       //○システム
  { DEBUG_FIELD_NUMINPUT, debugMenuCallProc_NumInput },     //数値入力
  { DEBUG_FIELD_STR04, debugMenuCallProc_GameEnd },         //ゲーム終了
  { DEBUG_FIELD_STR60, debugMenuCallProc_ForceSave },       //強制セーブ
  { DEBUG_FIELD_STR53, debugMenuCallProc_UseMemoryDump },   //メモリ状況
  { DEBUG_FIELD_STR22, debugMenuCallProc_ControlRtcList },  //時間
  { DEBUG_FIELD_STR61, debugMenuCallProc_CaptureList },     //キャプチャ
  { DEBUG_FIELD_STR40, debugMenuCallProc_ChangePlayerSex },   //主人公性別変更

  { DEBUG_FIELD_TITLE_03, (void*)BMPMENULIST_LABEL },       //○データ作成
  { DEBUG_FIELD_STR41, debugMenuCallProc_DebugMakePoke },   //ポケモン作成
  { DEBUG_FIELD_STR65, debugMenuCallProc_DebugReWritePoke },//ポケモン書き換え
  { DEBUG_FIELD_STR32, debugMenuCallProc_DebugItem },       //アイテム作成
  { DEBUG_FIELD_STR33, debugMenuCallProc_DebugSecretItem }, //隠されアイテム作成
  { DEBUG_FIELD_STR34, debugMenuCallProc_DebugPDWItem },    //PDWアイテム作成
  { DEBUG_FIELD_STR37, debugMenuCallProc_BoxMax },          //ボックス最大
  { DEBUG_FIELD_STR39, debugMenuCallProc_MyItemMax },       //アイテム最大
  { DEBUG_FIELD_MAKE_EGG,   debugMenuCallProc_MakeEgg },          //タマゴ作成
  { DEBUG_FIELD_MAKE_UNDATA,   debugMenuCallProc_DebugMakeUNData }, //国連データ作成
  { DEBUG_FIELD_MYSTERY_00, debugMenuCallProc_MakeMysteryCardList },//ふしぎなおくりものカード作成
  { DEBUG_FIELD_STR63, debugMenuCallProc_SetBtlBox },  //不正チェックを通るポケモンを作成
  { DEBUG_FIELD_STR64, debugMenuCallProc_ChangeName },  //主人公名を再設定
  { DEBUG_FIELD_STR67, debugMenuCallProc_EventPokeCreate },  //イベントポケモン作成

  { DEBUG_FIELD_TITLE_06, (void*)BMPMENULIST_LABEL },       //○つうしん
  { DEBUG_FIELD_STR19, debugMenuCallProc_OpenClubMenu },      //WIFIクラブ
  { DEBUG_FIELD_GPOWER, debugMenuCallProc_GPowerList},      //Gパワー
  { DEBUG_FIELD_C_CHOICE00, debugMenuCallProc_OpenCommDebugMenu },  //通信開始
  { DEBUG_FIELD_STR49, debugMenuCallProc_BeaconFriendCode },  //ともだちコード配信
  { DEBUG_FIELD_STR57, debugMenuCallProc_DebugSake },             //サケ操作
  { DEBUG_FIELD_STR58, debugMenuCallProc_DebugAtlas },            //アトラス操作
  { DEBUG_FIELD_GEONET, debugMenuCallProc_Geonet },         //ジオネット呼び出し

  { DEBUG_FIELD_TITLE_04, (void*)BMPMENULIST_LABEL },       //○アプリ
  { DEBUG_FIELD_STR44, debugMenuCallProc_UITemplate },        //UIテンプレート
  { DEBUG_FIELD_DEMO3D,   debugMenuCallProc_Demo3d },             //３Dデモ

  { DEBUG_FIELD_TITLE_05, (void*)BMPMENULIST_LABEL },       //○スタッフ用
  { DEBUG_FIELD_STR47, debugMenu_ControlShortCut },           //Yボタン登録最大
  { DEBUG_FIELD_STR51  , debugMenuCallProc_OpenGTSNegoMenu }, //GTSネゴ
  { DEBUG_FIELD_STR55,   debugMenuCallProc_CGEARPictureSave },//Cギアー写真
  { DEBUG_FIELD_STR21 , debugMenuCallProc_MusicalSelect },    //ミュージカル
  { DEBUG_FIELD_STR42, debugMenuCallProc_WifiGts },           //GTS
  { DEBUG_FIELD_STR48, debugMenuCallProc_GDS },               //GDS
  { DEBUG_FIELD_STR50, debugMenuCallProc_WazaOshie },         //技教え
  { DEBUG_FIELD_STR56, debugMenuCallProc_WifiBattleMatch },   //WIFI世界対戦
  { DEBUG_FIELD_SEASON_DISPLAY, debugMenuCallProc_SeasonDisplay },//季節表示
  { DEBUG_FIELD_STR59, debugMenuCallProc_BattleRecorder },        //バトルレコーダー
  { DEBUG_FIELD_BSW_00, debugMenuCallProc_BSubway },                //バトルサブウェイ
  { DEBUG_FIELD_ZUKAN_04, debugMenuCallProc_Zukan },            //ずかん
  { DEBUG_FIELD_STR66,  debugMenuCallProc_RingTone },           //着信音
};


//------------------------------------------------------------------------
/*
 *  QUICKSJUMP
 *  上記のリストのSTRIDを書くと、STARTボタンをおしたときに、その位置までカーソルが動きます
 */
//------------------------------------------------------------------------
#if defined DEBUG_ONLY_FOR_toru_nagihashi
  #define QuickJumpStart   DEBUG_FIELD_STR56
#elif defined DEBUG_ONLY_FOR_matsumiya
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_masafumi_saitou
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_suginaka_katsunori
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_mizuguchi_mai || defined DEBUG_ONLY_FOR_mai_ando
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_murakami_naoto
  #define QuickJumpStart   DEBUG_FIELD_STR03
  #define QuickJumpSelect  DEBUG_FIELD_STR17
#elif defined DEBUG_ONLY_FOR_ohno
  #define QuickJumpStart   DEBUG_FIELD_STR19
//#elif defined DEBUG_ONLY_FOR_
//  #define QuickJumpStart
#endif

//--------------------------------------------------------------
/// メニューヘッダー
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_DebugMenuListHeader =
{
  1,    //リスト項目数
  9,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  2,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
  DebugMenu_IineCallBack,  //１行ごとのコールバック
};

//--------------------------------------------------------------
/**
 * @brief フィールドデバッグメニュー初期化データ
 */
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugMenuData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_DebugMenuList),
  DATA_DebugMenuList,
  &DATA_DebugMenuListHeader,
  1, 1, D_MENU_CHARSIZE_X, D_MENU_CHARSIZE_Y,
  NULL,
  NULL,
};

//======================================================================
//  イベント：フィールドデバッグメニュー
//======================================================================
//--------------------------------------------------------------
/**
 * フィールドデバッグメニューイベント起動
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param heapID  HEAPID
 * @param page_id デバッグメニューページ
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_DebugMenu(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  HEAPID heapID, u16 page_id )
{
  DEBUG_MENU_EVENT_WORK * dmew;
  GMEVENT * event;

  event = GMEVENT_Create(
    gsys, NULL, DebugMenuEvent, sizeof(DEBUG_MENU_EVENT_WORK));

  dmew = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( dmew, sizeof(DEBUG_MENU_EVENT_WORK) );

  dmew->gmSys = gsys;
  dmew->gmEvent = event;
  dmew->gdata = GAMESYSTEM_GetGameData( gsys );
  dmew->fieldWork = fieldWork;
  dmew->heapID = heapID;

  return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static FLDMENUFUNC * DebugMenuInitCore(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, void* cb_work, u16 list_pos, u16 cursor_pos )
{
  GAMESYS_WORK * gsys;
  FLDMENUFUNC * ret;
  GFL_MSGDATA *msgData;
  FLDMSGBG *msgBG;
  FLDMENUFUNC_LISTDATA *listdata;
  FLDMENUFUNC_HEADER menuH;
  u16 max;

  gsys = FIELDMAP_GetGameSysWork( fieldmap );
  msgBG = FIELDMAP_GetFldMsgBG( fieldmap );
  msgData = FLDMSGBG_CreateMSGDATA( msgBG, init->msg_arc_id );

  if (init->getMaxFunc) {
    max = init->getMaxFunc( gsys, cb_work );
  } else {
    max = init->max;
  }
  if (init->makeListFunc) {
    listdata = FLDMENUFUNC_CreateListData( max, heapID );
    init->makeListFunc( gsys, listdata, heapID, msgData, cb_work );
  } else {
    listdata = FLDMENUFUNC_CreateMakeListData( init->menulist, max, msgData, heapID );
  }

  menuH = *(init->menuH);
  {
    u8 sy = (max * 2 < init->sy) ? max * 2 : init->sy;
    FLDMENUFUNC_InputHeaderListSize( &menuH, max, init->px, init->py, init->sx, sy );
  }
  ret = FLDMENUFUNC_AddMenuList( msgBG, &menuH, listdata, list_pos, cursor_pos );
  GFL_MSG_Delete( msgData );
  return ret;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
FLDMENUFUNC * DEBUGFLDMENU_Init(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init )
{
  return DebugMenuInitCore( fieldmap, heapID, init, NULL, 0, 0 );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
FLDMENUFUNC * DEBUGFLDMENU_InitEx(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, void* cb_work )
{
  return DebugMenuInitCore( fieldmap, heapID, init, cb_work, 0, 0 );
}

static FLDMENUFUNC * DEBUGFLDMENU_InitExPos(
    FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, u16 list_pos, u16 cursor_pos )
{
  return DebugMenuInitCore( fieldmap, heapID, init, NULL, list_pos, cursor_pos );
}

//--------------------------------------------------------------
/**
 * イベント：フィールドデバッグメニュー
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_MENU_EVENT_WORK *work = wk;

  switch (*seq) {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugMenuData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
#ifdef QuickJumpStart
        if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
        {
          u32 idx = DebugMenu_GetQuickJumpIdx( QuickJumpStart );
          FLDMENUFUNC_DeleteMenu( work->menuFunc );
          work->menuFunc  = DEBUGFLDMENU_InitExPos( work->fieldWork, work->heapID,  &DebugMenuData, idx-4, 4 );
        }
#endif  //QuickJumpStart
#ifdef QuickJumpSelect
        if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
        {
          u32 idx = DebugMenu_GetQuickJumpIdx( QuickJumpSelect );
          FLDMENUFUNC_DeleteMenu( work->menuFunc );
          work->menuFunc  = DEBUGFLDMENU_InitExPos( work->fieldWork, work->heapID,  &DebugMenuData, idx-4, 4 );
        }
#endif  //QuickJumpSelect

        break;
      }else if( ret == FLDMENUFUNC_CANCEL ){  //キャンセル
        (*seq)++;
      }else{              //決定
        work->call_proc = (DEBUG_MENU_CALLPROC)ret;
        (*seq)++;
      }
    }
    break;
  case 2:
    {
      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( work->call_proc != NULL ){
        if( work->call_proc(work) == TRUE ){
          return( GMEVENT_RES_CONTINUE );
        }
      }

      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  １行ごとのコールバック
 *
 *  @param  lw    ワーク
 *  @param  param 引数
 *  @param  y     Y位置
 */
//-----------------------------------------------------------------------------
static void DebugMenu_IineCallBack(BMPMENULIST_WORK* lw,u32 param,u8 y)
{
  if(param == BMPMENULIST_LABEL){
    BmpMenuList_TmpColorChange( lw,4,0,4);
  }else{
    BmpMenuList_TmpColorChange(lw,1,0,2);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  関数の位置を取得
 *
 *  @param  void *p_adrs  サーチする関数
 *
 *  @return 関数のインデックス
 */
//-----------------------------------------------------------------------------
static u32 DebugMenu_GetQuickJumpIdx( u32 strID )
{
  int i;
  for( i = 0; i < NELEMS(DATA_DebugMenuList); i++ )
  {
    if( DATA_DebugMenuList[i].str_id == strID )
    {
      return i;
    }
  }
  GF_ASSERT_MSG( 0, "デバッグメニューのクイックジャンプが間違っています\n" );
  return 0;
}





//======================================================================
//  汎用リスト　ヘッダー
//======================================================================
const FLDMENUFUNC_HEADER DATA_DebugMenuList_Default =
{
  1,    //リスト項目数
  9,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

/// どこでもジャンプ　メニューヘッダー
const FLDMENUFUNC_HEADER DATA_DebugMenuList_ZoneSel =
{
  1,    //リスト項目数
  9,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

//======================================================================
//  デバッグメニュー呼び出し
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL debugMenuCallProc_EventFlagScript( DEBUG_MENU_EVENT_WORK *now_wk )
{
  SCRIPT_ChangeScript( now_wk->gmEvent, SCRID_DEBUG_COMMON, NULL, HEAPID_FIELDMAP );
  //GMEVENT * new_event = DEBUG_EVENT_FLDMENU_DebugScript( now_wk );
  //GMEVENT_ChangeEvent( now_wk->gmEvent, new_event );

  return TRUE;
}
//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　デバッグスクリプト
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ScriptSelect( DEBUG_MENU_EVENT_WORK *now_wk )
{
  GMEVENT * new_event = DEBUG_EVENT_FLDMENU_DebugScript( now_wk );
  GMEVENT_ChangeEvent( now_wk->gmEvent, new_event );

  return TRUE;
}

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　通信デバッグ子メニュー
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_GameEnd( DEBUG_MENU_EVENT_WORK * wk )
{
  GMEVENT * new_event = DEBUG_EVENT_GameEnd( wk->gmSys, wk->fieldWork );
  GMEVENT_ChangeEvent( wk->gmEvent, new_event );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　ジャンプ
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Jump( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT * event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
      FS_OVERLAY_ID( debug_jump ), DEBUG_EVENT_FLDMENU_JumpEasy, NULL );
  GMEVENT_ChangeEvent( wk->gmEvent, event );
  //GMEVENT_ChangeEvent( wk->gmEvent, DEBUG_EVENT_FLDMENU_JumpEasy( wk->gmSys, NULL ) );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　四季マップ間移動
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static debugMenuCallProc_MapSeasonSelect( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT * event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
      FS_OVERLAY_ID( debug_jump ), DEBUG_EVENT_FLDMENU_ChangeSeason, NULL );
  GMEVENT_ChangeEvent( wk->gmEvent, event );
  //GMEVENT_ChangeEvent( wk->gmEvent, DEBUG_EVENT_FLDMENU_ChangeSeason( wk->gmSys, NULL ) );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　どこでもジャンプ
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_MapZoneSelect( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT * event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
      FS_OVERLAY_ID( debug_jump ), DEBUG_EVENT_FLDMENU_JumpAllZone, NULL );
  GMEVENT_ChangeEvent( wk->gmEvent, event );
  //GMEVENT_ChangeEvent( wk->gmEvent, DEBUG_EVENT_FLDMENU_JumpAllZone( wk->gmSys, NULL ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief タマゴさくせい
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_MakeEgg( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*      parentEvent = wk->gmEvent;
  GAMESYS_WORK* gameSystem  = wk->gmSys;
  HEAPID        heapID      = wk->heapID;

  GMEVENT_ChangeEvent( parentEvent, DEBUG_EVENT_FLDMENU_MakeEgg( gameSystem, heapID ) );
  return TRUE;
}

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　通信デバッグ子メニュー
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_OpenCommDebugMenu( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  const HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
#if 0 //通信システム変更の為、Fix 2009.09.03(木)
  FIELD_COMM_DEBUG_WORK *work;

  GMEVENT_Change( event,
    FIELD_COMM_DEBUG_CommDebugMenu, FIELD_COMM_DEBUG_GetWorkSize() );
  work = GMEVENT_GetEventWork( event );
  FIELD_COMM_DEBUG_InitWork( heapID , gameSys , fieldWork , event , work );
#else
  GF_ASSERT(0);
#endif

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * WiFiClubメニュー呼びだし
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_OpenClubMenu( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  GMEVENT *next_event;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;

  EVENT_WiFiClubChange(gameSys, fieldWork, event);
//  CGEARGET_EVENT_Change(gameSys, event);

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * GTSNegoメニュー呼びだし
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_OpenGTSNegoMenu( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;

  EVENT_GTSNegoChange(gameSys, fieldWork, event);
  return( TRUE );
}



#if 0
//--------------------------------------------------------------
/**
 * @brief   デバッグ ポケモンをセーブデータに入れる
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
#include "savedata/dreamworld_data.h"
#include "savedata/mystatus.h"
#include "savedata/mystatus_local.h"
#include "savedata/record.h"
static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk )
{
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData(wk->gmSys);
    DREAMWORLD_SAVEDATA *pdw = SaveControl_DataPtrGet(GAMEDATA_GetSaveControlWork(gamedata), GMDATA_ID_DREAMWORLD);
    POKEPARTY *party = GAMEDATA_GetMyPokemon(gamedata);
    POKEMON_PARAM *pp = PokeParty_GetMemberPointer(party, 0);
    POKEMON_PASO_PARAM  *ppp = PP_GetPPPPointer( pp );

    OS_TPrintf("before monsno = %d, level = %d, item = %d\n", PPP_Get(ppp, ID_PARA_monsno, NULL), PPP_Get(ppp, ID_PARA_level, NULL), PPP_Get(ppp, ID_PARA_item, NULL));
    DREAMWORLD_SV_SetSleepPokemon(pdw, pp);

//    pp = DREAMWORLD_SV_GetSleepPokemon(pdw);
  //  OS_TPrintf("after monsno = %d, level = %d, item = %d\n", PPP_Get(ppp, ID_PARA_monsno, NULL), PPP_Get(ppp, ID_PARA_level, NULL), PPP_Get(ppp, ID_PARA_item, NULL));

    {//Myステータス
      MYSTATUS* pMy = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(wk->gmSys));

      OS_TPrintf("before myst gold=%d,  id=%d, sex=%d\n",pMy->gold,  pMy->id, pMy->sex);
      pMy->gold = 511;
      pMy->id = 88776655;
      pMy->sex = 0;
      OS_TPrintf("after myst gold=%d,  id=%d, sex=%d\n",pMy->gold, pMy->id, pMy->sex);
    }

    {//レコード
      long* rec = (long*)SaveData_GetRecord(GAMEDATA_GetSaveControlWork(gamedata));

      OS_TPrintf("before record capture=%d, fishing=%d\n", rec[RECID_CAPTURE_POKE], rec[RECID_FISHING_SUCCESS]);
      rec[RECID_CAPTURE_POKE] = 7896;
      rec[RECID_FISHING_SUCCESS] = 3;
      OS_TPrintf("after record capture=%d, fishing=%d\n", rec[RECID_CAPTURE_POKE], rec[RECID_FISHING_SUCCESS]);
    }
  }

 // GMEVENT_ChangeEvent( wk->gmEvent, DEBUG_EVENT_FLDMENU_JumpEasy( wk->gmSys, wk->heapID ) );
  return( FALSE );
}


//--------------------------------------------------------------
/**
 * @brief     CGearの絵をROMから読み出しセーブ領域に入れる
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval    BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
#endif
#if 0
static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk )
{
  u16 crc=0;
  ARCHANDLE* p_handle;
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gameSys));

  NNSG2dCharacterData* charData;
  NNSG2dPaletteData* palData;
  void* pArc;
  u8* pCGearWork = GFL_HEAP_AllocMemory(HEAPID_FIELDMAP,SAVESIZE_EXTRA_CGEAR_PICTURE);
  CGEAR_PICTURE_SAVEDATA* pPic=(CGEAR_PICTURE_SAVEDATA*)pCGearWork;

  SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE, HEAPID_FIELDMAP,
                             pCGearWork,SAVESIZE_EXTRA_CGEAR_PICTURE);

  p_handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, HEAPID_FIELDMAP );
  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_decal_NCGR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedBGCharacterData( pArc, &charData ) ){
    GFL_STD_MemCopy(charData->pRawData, pPic->picture, CGEAR_DECAL_SIZE_MAX);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_decal_NCLR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedPaletteData( pArc, &palData ) ){
    GFL_STD_MemCopy(palData->pRawData, pPic->palette, CGEAR_PICTURTE_PAL_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_decal_NCLR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedPaletteData( pArc, &palData ) ){
    GFL_STD_MemCopy(palData->pRawData, pPic->palette, CGEAR_PICTURTE_PAL_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);


  GFL_ARC_CloseDataHandle( p_handle );

  crc = GFL_STD_CrcCalc( pPic, CGEAR_PICTURTE_ALL_SIZE );

  SaveControl_Extra_SaveAsyncInit(pSave,SAVE_EXTRA_ID_CGEAR_PICUTRE);
  while(1){
    if(SAVE_RESULT_OK==SaveControl_Extra_SaveAsyncMain(pSave,SAVE_EXTRA_ID_CGEAR_PICUTRE)){
      break;
    }
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
  }
  SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE);
  GFL_HEAP_FreeMemory(pCGearWork);
  CGEAR_SV_SetCGearONOFF(CGEAR_SV_GetCGearSaveData(pSave), TRUE);  //CGEAR表示有効
  CGEAR_SV_SetCGearPictureONOFF(CGEAR_SV_GetCGearSaveData(pSave),TRUE);  //CGEARデカール有効
  CGEAR_SV_SetCGearPictureCRC(CGEAR_SV_GetCGearSaveData(pSave),crc);  //CGEARデカール有効

  return( FALSE );
}
#endif

#if 1    //図鑑テスト

#include "include/savedata/zukan_wp_savedata.h"
// セーブデータ
typedef struct  {
	// カスタムグラフィックキャラ
	u8	customChar[ZUKANWP_SAVEDATA_CHAR_SIZE];
	// カスタムグラフィックパレット
	u16	customPalette[ZUKANWP_SAVEDATA_PAL_SIZE];
	// データ有無フラグ
	BOOL	flg;
} TESTZUKAN_DATA;



static BOOL debugMenuCallProc_CGEARPictureSave( DEBUG_MENU_EVENT_WORK *wk )
{
  int size,i;
  u16 crc=0;
  ARCHANDLE* p_handle;
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gameSys));
  NNSG2dCharacterData* charData;
  NNSG2dPaletteData* palData;
  NNSG2dScreenData* pScrData;
  void* pArc;
  u8* pCGearWork = GFL_HEAP_AllocMemory(HEAPID_FIELDMAP,SAVESIZE_EXTRA_ZUKAN_WALLPAPER);
  TESTZUKAN_DATA* pPic=(TESTZUKAN_DATA*)pCGearWork;

  pPic->flg=TRUE;

  p_handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, HEAPID_FIELDMAP );

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_zukantest_NCGR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedBGCharacterData( pArc, &charData ) ){
    GFL_STD_MemCopy(charData->pRawData, pPic->customChar, ZUKANWP_SAVEDATA_CHAR_SIZE);
  }
  GFL_HEAP_FreeMemory(pArc);

  pArc = GFL_ARCHDL_UTIL_Load( p_handle, NARC_c_gear_zukantest_NCLR, FALSE, HEAPID_FIELDMAP);
  if( NNS_G2dGetUnpackedPaletteData( pArc, &palData ) ){
    for(i=0;i<16;i++){
      GFL_STD_MemCopy(palData->pRawData, &pPic->customPalette[32*i], 32);
    }
  }
  GFL_HEAP_FreeMemory(pArc);

  GFL_ARC_CloseDataHandle( p_handle );

  size = ZUKANWP_SAVEDATA_CHAR_SIZE+ZUKANWP_SAVEDATA_PAL_SIZE*2;
  crc = GFL_STD_CrcCalc( pPic, size );

  {
    int i;
    u8* pU8 = (u8*)pPic;
    for(i=0;i<size;i++){
      OS_TPrintf("0x%x ",pU8[i]);
      if((i%16)==15){
        OS_TPrintf("\n");
      }
    }
  }
  
  OS_TPrintf("-----%x \n",crc);

  GFL_HEAP_FreeMemory(pCGearWork);

  return( FALSE );
}
#endif


//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し 数値入力
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_NumInput( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GMEVENT *event;

  event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
    FS_OVERLAY_ID( d_numinput ), DEBUG_EVENT_FLDMENU_NumInput, wk );

  GMEVENT_ChangeEvent( wk->gmEvent, event );

  return( TRUE );
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
typedef struct {
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  FLDMENUFUNC *menuFunc;
  FIELD_SUBSCREEN_WORK * subscreen;
}DEBUG_MENU_EVENT_SUBSCRN_SELECT_WORK, DMESSWORK;

//--------------------------------------------------------------
//  proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuSubscreenSelectEvent(
    GMEVENT *event, int *seq, void *wk );
static void setupTouchCameraSubscreen(DMESSWORK * dmess);
static void setupSoundViewerSubscreen(DMESSWORK * dmess);
static void setupNormalSubscreen(DMESSWORK * dmess);
static void setupTopMenuSubscreen(DMESSWORK * dmess);
static void setupUnionSubscreen(DMESSWORK * dmess);
static void setupDebugLightSubscreen(DMESSWORK * dmess);


//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　四季マップ間移動
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static debugMenuCallProc_SubscreenSelect( DEBUG_MENU_EVENT_WORK *wk )
{
  HEAPID heapID = wk->heapID;
  GMEVENT *event = wk->gmEvent;
  GAMESYS_WORK *gsys = wk->gmSys;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DMESSWORK *work;

    GMEVENT_Change( event,
      debugMenuSubscreenSelectEvent, sizeof(DMESSWORK) );
    work = GMEVENT_GetEventWork( event );
    GFL_STD_MemClear( work, sizeof(DMESSWORK) );

    work->gmSys = gsys;
    work->gmEvent = event;
    work->heapID = heapID;
    work->fieldWork = fieldWork;
    work->subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
    return( TRUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_SubcreenMenuList[FIELD_SUBSCREEN_MODE_MAX] =
{
  { DEBUG_FIELD_STR_SUBSCRN01, (void*)setupTouchCameraSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN02, (void*)setupSoundViewerSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN03, (void*)setupNormalSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN04, (void*)setupTopMenuSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN01, (void*)setupDebugLightSubscreen },
  { DEBUG_FIELD_STR_SUBSCRN05, (void*)setupUnionSubscreen },
};

static const DEBUG_MENU_INITIALIZER DebugSubscreenSelectData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_SubcreenMenuList),
  DATA_SubcreenMenuList,
  &DATA_DebugMenuList_Default, //流用
  1, 1, 16, 17,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * イベント：した画面ジャンプ
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuSubscreenSelectEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DMESSWORK *work = wk;

  switch( *seq )
  {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugSubscreenSelectData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }

      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){  //キャンセル
        return( GMEVENT_RES_FINISH );
      }else{
        typedef void (* CHANGE_FUNC)(DMESSWORK*);
        CHANGE_FUNC func = (CHANGE_FUNC)ret;
        func(work);
        //FIELD_SUBSCREEN_ChangeForce(FIELDMAP_GetFieldSubscreenWork(work->fieldWork), ret);
        return( GMEVENT_RES_FINISH );
      }
    }
    break;
  }

  return GMEVENT_RES_CONTINUE ;
}

//--------------------------------------------------------------
/**
 * @brief サブスクリーンのカメラ操作に同期させる
 *
 * @todo  カメラをバインドしてから、どのように切り離すか？を確定させる
 */
//--------------------------------------------------------------
static void setupTouchCameraSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
  {
    void * inner_work;
    FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(dmess->fieldWork);
    inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(dmess->subscreen);
    FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_CAMERA_POS, dmess->heapID);
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupSoundViewerSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupNormalSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_NORMAL);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupTopMenuSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_TOPMENU);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupUnionSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_UNION);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupDebugLightSubscreen(DMESSWORK * dmess)
{
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_LIGHT);
}


//--------------------------------------------------------------
//  ミュージカル系
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(musical);
#include "musical/musical_local.h"
#include "musical/musical_define.h"
#include "musical/musical_system.h"
#include "musical/musical_dressup_sys.h"
#include "musical/musical_stage_sys.h"
#include "poke_tool/poke_tool.h"  //ドレスアップ仮データ用
#include "poke_tool/monsno_def.h" //ドレスアップ仮データ用
#include "event_fieldmap_control.h"
typedef struct {
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  GMEVENT *newEvent;
  FIELDMAP_WORK *fieldWork;
  FLDMENUFUNC *menuFunc;

  POKEMON_PARAM *pokePara;
  MUSICAL_POKE_PARAM *musPoke;
  DRESSUP_INIT_WORK *dupInitWork;
  u8  menuRet;
}DEBUG_MENU_EVENT_MUSICAL_SELECT_WORK, DEB_MENU_MUS_WORK;
static GMEVENT_RESULT debugMenuMusicalSelectEvent( GMEVENT *event, int *seq, void *wk );

static void setupMusicalDressup(DEB_MENU_MUS_WORK * work);
static void setupMusicarShowPart(DEB_MENU_MUS_WORK * work);

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　ミュージカル呼び出し
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static debugMenuCallProc_MusicalSelect( DEBUG_MENU_EVENT_WORK *wk )
{
  HEAPID heapID = wk->heapID;
  GMEVENT *event = wk->gmEvent;
  GAMESYS_WORK *gsys = wk->gmSys;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEB_MENU_MUS_WORK *work;

    GMEVENT_Change( event,
      debugMenuMusicalSelectEvent, sizeof(DEB_MENU_MUS_WORK) );
    work = GMEVENT_GetEventWork( event );
    GFL_STD_MemClear( work, sizeof(DEB_MENU_MUS_WORK) );

    work->gmSys = gsys;
    work->gmEvent = event;
    work->heapID = heapID;
    work->fieldWork = fieldWork;
    work->dupInitWork = NULL;
    work->musPoke = NULL;
    work->pokePara = NULL;
    return( TRUE );
}

//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_MusicalMenuList[3] =
{
  { DEBUG_FIELD_STR_MUSICAL1, (void*)setupMusicalDressup },
  { DEBUG_FIELD_STR_MUSICAL2, (void*)setupMusicarShowPart },
};

static const DEBUG_MENU_INITIALIZER DebugMusicalSelectData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_MusicalMenuList),
  DATA_MusicalMenuList,
  &DATA_DebugMenuList_Default, //流用
  1, 1, 16, 17,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * イベント：ミュージカルデバッグメニュー
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuMusicalSelectEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEB_MENU_MUS_WORK *work = wk;

  switch( *seq )
  {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugMusicalSelectData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }

      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){  //キャンセル
        return( GMEVENT_RES_FINISH );
      }else{
        typedef void (* CHANGE_FUNC)(DEB_MENU_MUS_WORK*);
        CHANGE_FUNC func = (CHANGE_FUNC)ret;
        func(work);
        GMEVENT_CallEvent(work->gmEvent, EVENT_FSND_PushBGM(work->gmSys, FSND_FADE_NONE));
        (*seq)++;
        return( GMEVENT_RES_CONTINUE );
      }
    }
    break;
  case 2:
    GMEVENT_CallEvent(work->gmEvent, work->newEvent);
    (*seq)++;
    break;
  case 3:
    if( work->dupInitWork != NULL )
    {
      MUSICAL_DRESSUP_DeleteInitWork( work->dupInitWork );
      work->dupInitWork = NULL;
    }
    if( work->musPoke != NULL )
    {
      GFL_HEAP_FreeMemory( work->musPoke );
      work->musPoke = NULL;
    }
    if( work->pokePara != NULL )
    {
      GFL_HEAP_FreeMemory( work->pokePara );
      work->pokePara = NULL;
    }
    (*seq)++;
    break;
  case 4:
    GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(work->gmSys, FSND_FADE_NONE, FSND_FADE_NORMAL));
    (*seq)++;
    break;
  case 5:
    return( GMEVENT_RES_FINISH );
  }

  return GMEVENT_RES_CONTINUE ;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupMusicalDressup(DEB_MENU_MUS_WORK * work)
{
  work->pokePara = PP_Create( MONSNO_PIKUSII , 20 , PTL_SETUP_POW_AUTO , GFL_HEAPID_APP|HEAPDIR_MASK );
  work->musPoke = MUSICAL_SYSTEM_InitMusPoke( work->pokePara , GFL_HEAPID_APP|HEAPDIR_MASK );
  work->dupInitWork = MUSICAL_DRESSUP_CreateInitWork( GFL_HEAPID_APP|HEAPDIR_MASK , work->musPoke , SaveControl_GetPointer() );

  work->newEvent = EVENT_FieldSubProc(work->gmSys, work->fieldWork,
        FS_OVERLAY_ID(musical), &DressUp_ProcData, work->dupInitWork );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupMusicarShowPart(DEB_MENU_MUS_WORK * work)
{
  work->newEvent = EVENT_FieldSubProc(work->gmSys, work->fieldWork,
        FS_OVERLAY_ID(musical), &MusicalStage_ProcData, NULL );
}

//======================================================================
//  デバッグメニュー　カメラ操作
//======================================================================
//--------------------------------------------------------------
/// DEBUG_CTL_LINERCAMERA_WORK 線形カメラ操作ワーク1
//--------------------------------------------------------------
typedef struct
{
  int vanish;
  GAMESYS_WORK *gsys;
  GMEVENT *event;
  HEAPID heapID;
  FIELDMAP_WORK *fieldWork;
  FLDMSGBG *pMsgBG;
  FLDMSGWIN *pMsgWin;
  STRBUF *pStrBuf;
  u16 Wait;
  u16 Count;
}DEBUG_CTL_LINERCAMERA_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlLinerCamera(
    GMEVENT *event, int *seq, void *wk );
static void DampCameraInfo(FIELD_CAMERA * cam);
static BOOL LinerCamKeyContCtl(DEBUG_CTL_LINERCAMERA_WORK *work, const fx32 inAddVal, fx32 *outVal);


//======================================================================
//  デバッグメニュー　動作モデル一覧
//======================================================================
//--------------------------------------------------------------
/// DEBUG_MMDL_LIST_EVENT_WORK 動作モデルリスト処理用ワーク
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
  MMDLSYS *fldmmdlsys;

  u16 obj_code;
  u16 res_add;
  MMDL *fmmdl;
}DEBUG_MMDLLIST_EVENT_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuMMdlListEvent(
    GMEVENT *event, int *seq, void *wk );
static u16 * DEBUG_GetOBJCodeStrBuf( HEAPID heapID, u16 code );
static void DEBUG_SetMenuWorkMMdlList(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work );

/// 動作モデルリスト メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_MMdlList =
{
  1,    //リスト項目数
  10,   //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

static const DEBUG_MENU_INITIALIZER DebugMMdlListData = {
  NARC_message_d_field_dat,
  OBJCODETOTAL,
  NULL,
  &DATA_DebugMenuList_MMdlList,
  1, 1, 11, 16,
  DEBUG_SetMenuWorkMMdlList,
  NULL,
};

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　動作モデル一覧
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_MMdlList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_MMDLLIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuMMdlListEvent, sizeof(DEBUG_MMDLLIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_MMDLLIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;

  {
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    work->fldmmdlsys = GAMEDATA_GetMMdlSys( gdata );
  }

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：動作モデル一覧
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuMMdlListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_MMDLLIST_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugMMdlListData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }

      if( ret == FLDMENUFUNC_CANCEL || ret == NONDRAW ){ //キャンセル
        FLDMENUFUNC_DeleteMenu( work->menuFunc );
        return( GMEVENT_RES_FINISH );
      }

      work->obj_code = ret;
#if 0 //要らない
      work->res_add = MMDL_BLACTCONT_AddOBJCodeRes(
          work->fldmmdlsys, work->obj_code );
#endif
      {
        //VecFx32 pos;
        MMDL *jiki;
        MMDL_HEADER head = {
          0,  ///<識別ID
          0,  ///<表示するOBJコード
          MV_DOWN, ///<動作コード
          0,  ///<イベントタイプ
          0,  ///<イベントフラグ
          0,  ///<イベントID
          0,  ///<指定方向
          0,  ///<指定パラメタ 0
          0,  ///<指定パラメタ 1
          0,  ///<指定パラメタ 2
          4,  ///<X方向移動制限
          4,  ///<Z方向移動制限
          0,  ///<グリッドX
          0,  ///<グリッドZ
          0,  ///<Y値 fx32型
        };
        MMDL_HEADER_GRIDPOS *gridpos;


        jiki = MMDLSYS_SearchOBJID(
          work->fldmmdlsys, MMDL_ID_PLAYER );

        head.id = 250;
        MMDLHEADER_SetGridPos( &head,
            MMDL_GetGridPosX( jiki ) + 2,
            MMDL_GetGridPosZ( jiki ),
            MMDL_GetVectorPosY( jiki ) );
        head.obj_code = work->obj_code;
        work->fmmdl = MMDLSYS_AddMMdl(
          work->fldmmdlsys, &head, 0 );
      }

      (*seq)++;
      break;
    case 2:
      {
        int key_trg = GFL_UI_KEY_GetTrg();
        u8  move = 0xFF;
        MMDL_UpdateMoveProc( work->fmmdl );

        if( (key_trg & PAD_BUTTON_B) ){
          MMDL_Delete( work->fmmdl );

          if( work->res_add == TRUE ){
            MMDL_BLACTCONT_DeleteOBJCodeRes(
                work->fldmmdlsys, work->obj_code );
          }

          (*seq) = 1;
          return GMEVENT_RES_CONTINUE;
        }

        if( key_trg & PAD_BUTTON_X ){
          move = MV_RT2;
        }else if( (key_trg & PAD_KEY_UP )){
          move = MV_UP;
        }else if( key_trg & PAD_KEY_DOWN ){
          move = MV_DOWN;
        }else if( key_trg & PAD_KEY_LEFT ){
          move = MV_LEFT;
        }else if( key_trg & PAD_KEY_RIGHT ){
          move = MV_RIGHT;
        }
        if( move != 0xFF ){
          MMDL_ChangeMoveCode( work->fmmdl, move );
        }

      }
      break;
    }
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * 動作モデルOBJコード->STRCODE
 * @param heapID  文字列バッファ確保用ヒープID
 * @param code  文字列を取得したいOBJコード
 * @retval  u16*  文字列が格納されたu16*(開放が必要
 */
//--------------------------------------------------------------
static u16 * DEBUG_GetOBJCodeStrBuf( HEAPID heapID, u16 code )
{
  int i;
  u16 utf16,utf16_eom;
  u16 *pStrBuf;
  u8 *name8;

  pStrBuf = GFL_HEAP_AllocClearMemory( heapID,
      sizeof(u16)*DEBUG_OBJCODE_STR_LENGTH );
  name8 = DEBUG_MMDL_GetOBJCodeString( code, heapID );
  //DEBUG_ConvertAsciiToUTF16( name8, DEBUG_OBJCODE_STR_LENGTH, pStrBuf );
  DEB_STR_CONV_SjisToStrcode( (const char *)name8, pStrBuf, DEBUG_OBJCODE_STR_LENGTH );
  GFL_HEAP_FreeMemory( name8 );

  return( pStrBuf );
}

//--------------------------------------------------------------
/**
 * 動作モデルリスト用BMP_MENULIST_DATAセット
 * @param list  セット先BMP_MENULIST_DATA
 * @param heapID  文字列バッファ確保用HEAPID
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkMMdlList(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  u16 *str;
  int id,max;
  STRBUF *strBuf = GFL_STR_CreateBuffer( DEBUG_OBJCODE_STR_LENGTH, heapID );

  for( id = OBJCODESTART_BBD; id < OBJCODEEND_BBD; id++ ){
    GFL_STR_ClearBuffer( strBuf );
    str = DEBUG_GetOBJCodeStrBuf( heapID, id );
    GFL_STR_SetStringCode( strBuf, str );
    GFL_HEAP_FreeMemory( str );
    FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
  }

  for( id = OBJCODESTART_TPOKE; id < OBJCODEEND_TPOKE; id++ ){
    GFL_STR_ClearBuffer( strBuf );
    str = DEBUG_GetOBJCodeStrBuf( heapID, id );
    GFL_STR_SetStringCode( strBuf, str );
    GFL_HEAP_FreeMemory( str );
    FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
  }

  for( id = OBJCODESTART_MDL; id < OBJCODEEND_MDL; id++ ){
    GFL_STR_ClearBuffer( strBuf );
    str = DEBUG_GetOBJCodeStrBuf( heapID, id );
    GFL_STR_SetStringCode( strBuf, str );
    GFL_HEAP_FreeMemory( str );
    FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
  }

  GFL_HEAP_FreeMemory( strBuf );
}

//======================================================================
//  デバッグメニュー　ライト操作
//======================================================================

//--------------------------------------------------------------
/// DEBUG_CTLIGHT_WORK ライト操作ワーク
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK *gsys;
  GMEVENT *event;
  HEAPID heapID;
  FIELDMAP_WORK *fieldWork;

  GFL_BMPWIN* p_win;
}DEBUG_CTLLIGHT_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlLight(
    GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　カメラ操作
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ControlLight( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_CTLLIGHT_WORK *work;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;

  GMEVENT_Change( event, debugMenuControlLight, sizeof(DEBUG_CTLLIGHT_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CTLLIGHT_WORK) );

  work->gsys = gsys;
  work->event = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}


//--------------------------------------------------------------
/**
 * イベント：カメラ操作
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlLight(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_CTLLIGHT_WORK *work = wk;
  FIELD_LIGHT* p_light;

  // ライト取得
  p_light = FIELDMAP_GetFieldLight( work->fieldWork );

  switch( (*seq) ){
  case 0:
    // ライト管理開始
    FIELD_LIGHT_DEBUG_Init( p_light, work->heapID );

    // インフォーバーの非表示
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(work->fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // ビットマップウィンドウ初期化
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // パレット情報を転送
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, work->heapID );

      // ビットマップウィンドウを作成
      work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( work->p_win );
      GFL_BMPWIN_TransVramCharacter( work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // ウィンドウ
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, work->heapID );
      BmpWinFrame_Write( work->p_win, TRUE, 1, 15 );
    }

    FIELD_LIGHT_DEBUG_PrintData( p_light, work->p_win );

    (*seq)++;
  case 1:
    // ライト管理メイン
    FIELD_LIGHT_DEBUG_Control( p_light );
    FIELD_LIGHT_DEBUG_PrintData( p_light, work->p_win );

    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
      (*seq)++;
    }
    break;
  case 2:
    // ライト管理終了
    FIELD_LIGHT_DEBUG_Exit( p_light );

    // ビットマップウィンドウ破棄
    {
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
      GFL_BMPWIN_Delete( work->p_win );
    }

    // インフォーバーの表示
    FIELDMAP_SetFieldSubscreenWork(work->fieldWork,
        FIELD_SUBSCREEN_Init( work->heapID, work->fieldWork, FIELD_SUBSCREEN_NORMAL ));

    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}


//======================================================================
//  デバッグメニュー　フォグ操作
//======================================================================

//--------------------------------------------------------------
/// DEBUG_CTLFOG_WORK フォグ操作ワーク
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK *gsys;
  GMEVENT *event;
  HEAPID heapID;
  FIELDMAP_WORK *fieldWork;

  GFL_BMPWIN* p_win;

}DEBUG_CTLFOG_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlFog(
    GMEVENT *event, int *seq, void *wk );

static BOOL debugMenuCallProc_ControlFog( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_CTLFOG_WORK *work;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;

  GMEVENT_Change( event, debugMenuControlFog, sizeof(DEBUG_CTLFOG_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CTLFOG_WORK) );

  work->gsys = gsys;
  work->event = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}


//--------------------------------------------------------------
/**
 * イベント：ふぉぐ操作
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlFog(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_CTLFOG_WORK *work = wk;
  FIELD_FOG_WORK* p_fog;

  // フォグ取得
  p_fog = FIELDMAP_GetFieldFog( work->fieldWork );

  switch( (*seq) ){
  case 0:

    // インフォーバーの非表示
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(work->fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // ビットマップウィンドウ初期化
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // パレット情報を転送
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, work->heapID );

      // ビットマップウィンドウを作成
      work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( work->p_win );
      GFL_BMPWIN_TransVramCharacter( work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // ウィンドウ
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, work->heapID );
      BmpWinFrame_Write( work->p_win, TRUE, 1, 15 );
    }

    FIELD_FOG_DEBUG_Init( p_fog, work->heapID );


    (*seq)++;
  case 1:
    // フォグ管理メイン
    FIELD_FOG_DEBUG_Control( p_fog );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
    FIELD_FOG_DEBUG_PrintData( p_fog, work->p_win );
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
      (*seq)++;
    }
    break;
  case 2:

    // ビットマップウィンドウ破棄
    {
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
      GFL_BMPWIN_Delete( work->p_win );
    }

    FIELD_FOG_DEBUG_Exit( p_fog );


    // インフォーバーの表示
    FIELDMAP_SetFieldSubscreenWork(work->fieldWork,
        FIELD_SUBSCREEN_Init( work->heapID, work->fieldWork, FIELD_SUBSCREEN_NORMAL ));

    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}


//======================================================================
//  デバッグメニュー　てんきリスト
//======================================================================
//--------------------------------------------------------------
/// DEBUG_WEATERLIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DEBUG_WEATERLIST_EVENT_WORK;


//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuWeatherListEvent(
    GMEVENT *event, int *seq, void *work );

///テストカメラリスト メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_WeatherList =
{
  1,    //リスト項目数
  6,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

///てんきメニューリスト
static const FLDMENUFUNC_LIST DATA_WeatherMenuList[] =
{
  { D_TOMOYA_WEATEHR00, (void*)WEATHER_NO_SUNNY },
  { D_TOMOYA_WEATEHR01, (void*)WEATHER_NO_SNOW },
  { D_TOMOYA_WEATEHR02, (void*)WEATHER_NO_RAIN },
  { D_TOMOYA_WEATEHR03, (void*)WEATHER_NO_STORM },
  { D_TOMOYA_WEATEHR05, (void*)WEATHER_NO_SPARK },
  { D_TOMOYA_WEATEHR06, (void*)WEATHER_NO_SNOWSTORM },
  { D_TOMOYA_WEATEHR07, (void*)WEATHER_NO_ARARE },
  { D_TOMOYA_WEATEHR09, (void*)WEATHER_NO_RAIKAMI },
  { D_TOMOYA_WEATEHR10, (void*)WEATHER_NO_KAZAKAMI },
  { D_TOMOYA_WEATEHR11, (void*)WEATHER_NO_DIAMONDDUST },
  { D_TOMOYA_WEATEHR12, (void*)WEATHER_NO_MIST },
  { D_TOMOYA_WEATEHR13, (void*)WEATHER_NO_EVENING_RAIN },
  { D_TOMOYA_WEATEHR08, (void*)WEATHER_NO_MIRAGE },
};
#define DEBUG_WEATHERLIST_LIST_MAX  ( NELEMS(DATA_WeatherMenuList) )

static const DEBUG_MENU_INITIALIZER DebugWeatherMenuListData = {
  NARC_message_d_tomoya_dat,
  NELEMS(DATA_WeatherMenuList),
  DATA_WeatherMenuList,
  &DATA_DebugMenuList_WeatherList,
  1, 1, 12, 11,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　天気リスト
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_WeatherList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_WEATERLIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuWeatherListEvent, sizeof(DEBUG_WEATERLIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_WEATERLIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：てんきリスト
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuWeatherListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_WEATERLIST_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugWeatherMenuListData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }

      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret != FLDMENUFUNC_CANCEL ){  //決定
        GAMEDATA* gdata = GAMESYSTEM_GetGameData( work->gmSys );
        FIELD_WEATHER_Change( FIELDMAP_GetFieldWeather( work->fieldWork ), ret );
        GAMEDATA_SetWeatherNo( gdata, ret );
      }

      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  デバッグメニュー　位置情報
//======================================================================
//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　フィールド位置情報
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_FieldPosData( DEBUG_MENU_EVENT_WORK *wk )
{
  FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( wk->fieldWork );
  FIELD_DEBUG_SetPosPrint( debug );
  return( FALSE );
}

//======================================================================
/// デバックメニュー　時間
//======================================================================
//--------------------------------------------------------------
/// DEBUG_CONTROL_TIME_LIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DEBUG_CONTROL_TIME_LIST_EVENT_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlTimeListEvent(
    GMEVENT *event, int *seq, void *work );


//--------------------------------------------------------------
/// data
//--------------------------------------------------------------
/// 時間管理タイプ数
enum{
  CONT_TIME_TYPE_NORMAL,
  CONT_TIME_TYPE_8HOUR,
  CONT_TIME_TYPE_12HOUR,
  CONT_TIME_TYPE_18HOUR,
  CONT_TIME_TYPE_22HOUR,
  CONT_TIME_TYPE_2HOUR,
  CONT_TIME_TYPE_600RATE,
  CONT_TIME_TYPE_60RATE,

  CONT_TIME_TYPE_NUM,
} ;


///テストカメラリスト メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_ContTimeList =
{
  1,    //リスト項目数
  CONT_TIME_TYPE_NUM,   //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

///テストカメラメニューリスト
static const FLDMENUFUNC_LIST DATA_ControlTimeMenuList[CONT_TIME_TYPE_NUM] =
{
  { DEBUG_FIELD_STR30, (void*)CONT_TIME_TYPE_NORMAL,  },
  { DEBUG_FIELD_STR23, (void*)CONT_TIME_TYPE_8HOUR,   },
  { DEBUG_FIELD_STR24, (void*)CONT_TIME_TYPE_12HOUR,  },
  { DEBUG_FIELD_STR25, (void*)CONT_TIME_TYPE_18HOUR,  },
  { DEBUG_FIELD_STR26, (void*)CONT_TIME_TYPE_22HOUR,  },
  { DEBUG_FIELD_STR27, (void*)CONT_TIME_TYPE_2HOUR,   },
  { DEBUG_FIELD_STR28, (void*)CONT_TIME_TYPE_600RATE, },
  { DEBUG_FIELD_STR29, (void*)CONT_TIME_TYPE_60RATE,  },
};

static const DEBUG_MENU_INITIALIZER DebugControlTimeMenuListData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_ControlTimeMenuList),
  DATA_ControlTimeMenuList,
  &DATA_DebugMenuList_ContTimeList,
  1, 1, 9, 13,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　テストカメラリスト
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ControlRtcList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_CONTROL_TIME_LIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuControlTimeListEvent, sizeof(DEBUG_CONTROL_TIME_LIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CONTROL_TIME_LIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：時間操作リスト
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlTimeListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_CONTROL_TIME_LIST_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugControlTimeMenuListData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }

      if( ret != FLDMENUFUNC_CANCEL ){  //決定
        switch( ret ){
        case CONT_TIME_TYPE_NORMAL:
          GFL_RTC_DEBUG_StopFakeTime();
          break;
        case CONT_TIME_TYPE_8HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 8,0 );
        case CONT_TIME_TYPE_12HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 12,0 );
          break;
        case CONT_TIME_TYPE_18HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 18,0 );
          break;
        case CONT_TIME_TYPE_22HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 22,0 );
          break;
        case CONT_TIME_TYPE_2HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 2,0 );
          break;
        case CONT_TIME_TYPE_600RATE:
          GFL_RTC_DEBUG_StartFakeTime( 600 );
          break;
        case CONT_TIME_TYPE_60RATE:
          GFL_RTC_DEBUG_StartFakeTime( 60 );
          break;
        default:
          break;
        }
      }else{

        FLDMENUFUNC_DeleteMenu( work->menuFunc );


        // オワリ
        return( GMEVENT_RES_FINISH );
      }
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  デバッグメニュー　波乗り
//======================================================================
//--------------------------------------------------------------
/**
 * 波乗りイベント呼び出し
 * @param wk DEBUG_MENU_EVENT_WORK*
 * @retval BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Naminori( DEBUG_MENU_EVENT_WORK *wk )
{
  if( FIELDMAP_GetMapControlType(wk->fieldWork) != FLDMAP_CTRLTYPE_GRID )
  {
    // レールのとき
    FIELD_PLAYER* p_fld_player = FIELDMAP_GetFieldPlayer( wk->fieldWork );
    if( FIELD_PLAYER_GetMoveForm( p_fld_player ) != PLAYER_MOVE_FORM_SWIM ){
      FIELD_PLAYER_SetNaminori( p_fld_player );
    }else{
      FIELD_PLAYER_SetNaminoriEnd( p_fld_player );
    }
    return( FALSE );
  }

  FIELD_EVENT_ChangeNaminoriStart( wk->gmEvent, wk->gmSys, wk->fieldWork );
  return( TRUE );
}

//======================================================================
//  デバッグメニュー ポケモン作成
//======================================================================
#include "debug/debug_makepoke.h"
FS_EXTERN_OVERLAY(debug_makepoke);
static GMEVENT_RESULT debugMenuMakePoke( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//-------------------------------------
/// デバッグポケモン作成用ワーク
//=====================================
typedef struct
{
  HEAPID heapID;
  GAMESYS_WORK    *p_gamesys;
  GMEVENT         *p_event;
  FIELDMAP_WORK *p_field;
  PROCPARAM_DEBUG_MAKEPOKE p_mp_work;
  POKEMON_PARAM *pp;
} DEBUG_MAKEPOKE_EVENT_WORK;

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン作成
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ワーク
 *
 *  @return TRUEイベント継続  FALSE終了
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugMakePoke( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_MAKEPOKE_EVENT_WORK *p_mp_work;

  //イヴェント
  GMEVENT_Change( p_event, debugMenuMakePoke, sizeof(DEBUG_MAKEPOKE_EVENT_WORK) );
  p_mp_work = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_mp_work, sizeof(DEBUG_MAKEPOKE_EVENT_WORK) );

  //ワーク設定
  p_mp_work->p_gamesys  = p_gamesys;
  p_mp_work->p_event    = p_event;
  p_mp_work->p_field    = p_field;
  p_mp_work->heapID     = heapID;
  p_mp_work->pp = PP_Create( 1,1,PTL_SETUP_ID_AUTO,p_mp_work->heapID );
  p_mp_work->p_mp_work.dst = p_mp_work->pp;
  p_mp_work->p_mp_work.oyaStatus = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(p_gamesys) );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  デバッグポケモン作成イベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuMakePoke( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
  };

  DEBUG_MAKEPOKE_EVENT_WORK *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(debug_makepoke), &ProcData_DebugMakePoke, &p_wk->p_mp_work ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    if( p_wk->pp != NULL )
    {
      GAMEDATA *gmData = GAMESYSTEM_GetGameData(p_wk->p_gamesys);
      POKEPARTY *party = GAMEDATA_GetMyPokemon(gmData);

      {
        u16 oyaName[6] = {L'で',L'ば',L'ぐ',L'ぽ',L'け',0xFFFF};
        PP_Put( p_wk->pp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
        PP_Put( p_wk->pp , ID_PARA_oyasex , PTL_SEX_MALE );
      }
      //手持ちに空きがあれば入れる
      if( PokeParty_GetPokeCount( party ) < 6 )
      {
        PokeParty_Add( party , p_wk->pp );
      }

      GFL_HEAP_FreeMemory( p_wk->pp );
    }
    return GMEVENT_RES_FINISH;
    break;
  }

  return GMEVENT_RES_CONTINUE ;
}

static GMEVENT_RESULT debugMenuReWritePoke( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン書き換え
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ワーク
 *
 *  @return TRUEイベント継続  FALSE終了
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugReWritePoke( DEBUG_MENU_EVENT_WORK *p_wk )
{ 
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_MAKEPOKE_EVENT_WORK *p_mp_work;

  //イヴェント
  GMEVENT_Change( p_event, debugMenuReWritePoke, sizeof(DEBUG_MAKEPOKE_EVENT_WORK) );
  p_mp_work = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_mp_work, sizeof(DEBUG_MAKEPOKE_EVENT_WORK) );

  //ワーク設定
  p_mp_work->p_gamesys  = p_gamesys;
  p_mp_work->p_event    = p_event;
  p_mp_work->p_field    = p_field;
  p_mp_work->heapID     = heapID;
  {
    POKEPARTY *p_party  = GAMEDATA_GetMyPokemon( GAMESYSTEM_GetGameData(p_gamesys) );
    p_mp_work->pp =  PokeParty_GetMemberPointer( p_party, 0 );
  }
  p_mp_work->p_mp_work.dst  = p_mp_work->pp;
  p_mp_work->p_mp_work.oyaStatus = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(p_gamesys) );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  デバッグポケモン書き換えイベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuReWritePoke( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
  };

  DEBUG_MAKEPOKE_EVENT_WORK *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(debug_makepoke), &ProcData_DebugMakePoke, &p_wk->p_mp_work ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}


//======================================================================
//  デバッグメニュー アイテム
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   デバッグアイテム作成
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_DebugItem( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  GMEVENT * child_event;

  child_event = GMEVENT_CreateOverlayEventCall(
      gameSys, FS_OVERLAY_ID( debug_item ), EVENT_DebugItemMake, NULL );
  //child_event = EVENT_DebugItemMake(gameSys, wk->heapID);
  GMEVENT_ChangeEvent( event, child_event );

  return( TRUE );
}
//--------------------------------------------------------------
/**
 * @brief   デバッグ隠されアイテム作成
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_DebugSecretItem( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  GMEVENT * child_event;

  child_event = GMEVENT_CreateOverlayEventCall(
      gameSys, FS_OVERLAY_ID( debug_item ), EVENT_DebugSecretItemMake, NULL );
  GMEVENT_ChangeEvent( event, child_event );

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * @brief   デバッグPDWアイテム作成
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_DebugPDWItem( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  GMEVENT * child_event;

  child_event = GMEVENT_CreateOverlayEventCall(
      gameSys, FS_OVERLAY_ID( debug_item ), EVENT_DebugPDWItemMake, NULL );
  GMEVENT_ChangeEvent( event, child_event );

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンボックスにポケモンでいっぱいにする
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------

static BOOL debugMenuCallProc_BoxMax( DEBUG_MENU_EVENT_WORK *wk )
{
#if 1
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  MYSTATUS *myStatus;
  POKEMON_PARAM *pp;
  POKEMON_PASO_PARAM  *ppp;
  const STRCODE *name;

  myStatus = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gameSys));
  name = MyStatus_GetMyName( myStatus );

  pp = PP_Create(100, 100, 123456, HEAPID_FIELDMAP);

  {
    int i,j,k=1;
    BOX_MANAGER* pBox = GAMEDATA_GetBoxManager(GAMESYSTEM_GetGameData(gameSys));

    if(GFL_UI_KEY_GetCont() & PAD_BUTTON_L){
      k=BOX_MAX_TRAY;
    }
    for(i=0;i < k;i++){
      for(j=0;j < 30;j++){
//        int monsno =GFUser_GetPublicRand(MONSNO_END-1)+1;
//        int monsno = i+10;
        int monsno = GFUser_GetPublicRand(300)+1;
        OS_TPrintf("%d  %d %d作成\n",monsno, i, j);
        PP_Setup(pp,  monsno , 100, MyStatus_GetID( myStatus ));

        ppp = (POKEMON_PASO_PARAM  *)PP_GetPPPPointerConst( pp );
        PPP_Put( ppp , ID_PARA_oyaname_raw , (u32)name );
        PPP_Put( ppp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );

        BOXDAT_PutPokemonBox(pBox, i, ppp);
      }
    }
  }

  GFL_HEAP_FreeMemory(pp);
#endif
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * @brief   バッグにアイテムを限界までいれる
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------

static BOOL debugMenuCallProc_MyItemMax( DEBUG_MENU_EVENT_WORK *wk )
{
  u32 i;
  MYITEM_PTR myitem = GAMEDATA_GetMyItem(GAMESYSTEM_GetGameData(wk->gmSys));

  for( i = 0; i < ITEM_DATA_MAX; i++ ){
    if( ITEM_CheckEnable( i ) ){
      MYITEM_AddItem(myitem , i, 2, GFL_HEAPID_APP );
    }
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * @brief   不正チェックを通るポケモンをバトルBOXへセットする
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_SetBtlBox( DEBUG_MENU_EVENT_WORK *wk )
{
#if 1
  GAMESYS_WORK  *gameSys  = wk->gmSys;
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gameSys));
  MYSTATUS *myStatus;
  POKEMON_PASO_PARAM  *ppp;
  const STRCODE *name;
  POKEPARTY *party = PokeParty_AllocPartyWork( HEAPID_FIELDMAP );
  BATTLE_BOX_SAVE* btlbox = BATTLE_BOX_SAVE_GetBattleBoxSave( sv );
  myStatus = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gameSys));
  name = MyStatus_GetMyName( myStatus );
  BATTLE_BOX_SAVE_InitWork(btlbox);



  { 
    int i,j;

    static const u16 monsno_tbl1[] =
    { 
      MONSNO_KORATTA,
      MONSNO_OTATI,
      MONSNO_POTIENA,
      MONSNO_BIPPA,
      MONSNO_PHI,
      MONSNO_ZIGUZAGUMA,
    };
    static const u16 waza_tbl1[] =
    { 
      WAZANO_TAIATARI,
      WAZANO_HIKKAKU,
      WAZANO_TAIATARI,
      WAZANO_TAIATARI,
      WAZANO_HATAKU,
      WAZANO_TAIATARI,
    };
    static const u16 monsno_tbl2[] =
    { 
      MONSNO_KAIRIKII,
      MONSNO_OKORIZARU,
      MONSNO_GOUKAZARU,
      MONSNO_HARITEYAMA,
      MONSNO_HERAKUROSU,
      MONSNO_RUKARIO,
    };
    static const u16 waza_tbl2[] =
    { 
      WAZANO_BAKURETUPANTI,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
      WAZANO_INFAITO,
    };
    u16 poke_name[6] =
      L"デバグポケ";

    const u16 *p_monsno;
    const u16 *p_waza;
    u32 pow = 0;

    poke_name[ 5 ]  = 0xFFFF;


    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
    { 
      p_monsno  = monsno_tbl1;
      p_waza    =waza_tbl1;
      pow       =0;
    }
    else
    { 
      p_monsno  = monsno_tbl2;
      p_waza    =waza_tbl2;
      pow       =252;
    }


    for(i=0;i < PokeParty_GetPokeCountMax(party);i++){
      POKEMON_PARAM *p_temp = PP_Create( p_monsno[ i ], 100, 0, HEAPID_PROC ); 
      PP_Put( p_temp, ID_PARA_waza1, p_waza[ i ] );
      PP_Put( p_temp, ID_PARA_waza2, WAZANO_NULL );
      PP_Put( p_temp, ID_PARA_waza3, WAZANO_NULL );
      PP_Put( p_temp, ID_PARA_waza4, WAZANO_NULL );
      PP_Put( p_temp, ID_PARA_oyaname_raw, (u32)poke_name );
      PP_Put( p_temp, ID_PARA_pow_exp, pow );

      PP_Renew( p_temp );
      PokeParty_Add( party, p_temp );

      GFL_HEAP_FreeMemory( p_temp );
    }
    BATTLE_BOX_SAVE_SetPokeParty( btlbox, party );

  }
  
  GFL_HEAP_FreeMemory(party);
#endif
  return( FALSE );
}
//======================================================================
//  デバッグメニュー 主人公名前設定
//======================================================================
FS_EXTERN_OVERLAY(namein);
//-------------------------------------
/// デバッグ主人公名変更
//=====================================
typedef struct
{
  GAMESYS_WORK    *p_gamesys;
  GMEVENT         *p_event;
  FIELDMAP_WORK *p_field;
  NAMEIN_PARAM   *p_param;
  STRBUF          *p_default_str;
} DEBUG_CHANGENAME_EVENT_WORK;
//-------------------------------------
///   PROTOTYPE
//=====================================
static GMEVENT_RESULT debugEvnetChangeName( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//--------------------------------------------------------------
/**
 * @brief   主人公名前を再設定する
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ChangeName( DEBUG_MENU_EVENT_WORK *p_wk )
{ 
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  DEBUG_CHANGENAME_EVENT_WORK  *p_ev_wk;
  GAMEDATA  *p_gamedata = GAMESYSTEM_GetGameData(p_gamesys);
  MYSTATUS  *p_mystatus = GAMEDATA_GetMyStatus( p_gamedata );
  MISC      *p_misc     = SaveData_GetMisc( GAMEDATA_GetSaveControlWork(p_gamedata) );

  //イヴェント
  GMEVENT_Change( p_event, debugEvnetChangeName, sizeof(DEBUG_CHANGENAME_EVENT_WORK) );
  p_ev_wk = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_ev_wk, sizeof(DEBUG_CHANGENAME_EVENT_WORK) );

  //ワーク設定
  p_ev_wk->p_gamesys  = p_gamesys;
  p_ev_wk->p_event    = p_event;
  p_ev_wk->p_field    = p_field;

  //デフォルト名
  p_ev_wk->p_default_str  = MyStatus_CreateNameString(p_mystatus, HEAPID_PROC);

  GFL_OVERLAY_Load(FS_OVERLAY_ID(namein) );
  //名前入力ワーク設定
  p_ev_wk->p_param  = NAMEIN_AllocParam( HEAPID_PROC, NAMEIN_MYNAME, MyStatus_GetMySex(p_mystatus), 0, NAMEIN_PERSON_LENGTH, p_ev_wk->p_default_str, p_misc );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  主人公名前再設定イベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugEvnetChangeName( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
    SEQ_EXIT,
  };

  DEBUG_CHANGENAME_EVENT_WORK  *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        NO_OVERLAY_ID, &NameInputProcData, p_wk->p_param ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    if( !NAMEIN_IsCancel( p_wk->p_param ) )
    { 
      MYSTATUS      *p_mystatus  = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(p_wk->p_gamesys) );
      NAMEIN_CopyStr( p_wk->p_param, p_wk->p_default_str );
      MyStatus_SetMyNameFromString(p_mystatus, p_wk->p_default_str );
    }

    *p_seq  = SEQ_EXIT;
    break;


  case SEQ_EXIT:
    NAMEIN_FreeParam( p_wk->p_param );
    GFL_STR_DeleteBuffer( p_wk->p_default_str );

    GFL_OVERLAY_Unload( FS_OVERLAY_ID(namein) );
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}



//======================================================================
//  デバッグメニュー そらを飛ぶ
//======================================================================
FS_EXTERN_OVERLAY(townmap);
//-------------------------------------
/// デバッグ空を飛ぶ用ワーク
//=====================================
typedef struct
{
  GAMESYS_WORK    *p_gamesys;
  GMEVENT         *p_event;
  FIELDMAP_WORK *p_field;
  TOWNMAP_PARAM   *p_param;
} DEBUG_SKYJUMP_EVENT_WORK;
//-------------------------------------
///   PROTOTYPE
//=====================================
static GMEVENT_RESULT debugMenuSkyJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *  @brief  デバッグ空を飛ぶ
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ワーク
 *
 *  @return TRUEイベント継続  FALSE終了
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugSkyJump( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  DEBUG_SKYJUMP_EVENT_WORK  *p_sky;

  //イヴェント
  GMEVENT_Change( p_event, debugMenuSkyJump, sizeof(DEBUG_SKYJUMP_EVENT_WORK) );
  p_sky = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_sky, sizeof(DEBUG_SKYJUMP_EVENT_WORK) );

  //ワーク設定
  p_sky->p_gamesys  = p_gamesys;
  p_sky->p_event    = p_event;
  p_sky->p_field    = p_field;

  //タウンマップワーク設定
  p_sky->p_param  = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(TOWNMAP_PARAM) );
  GFL_STD_MemClear( p_sky->p_param, sizeof(TOWNMAP_PARAM) );
  p_sky->p_param->mode      = TOWNMAP_MODE_DEBUGSKY;
  {
    GAMEDATA    *p_gamedata;
    PLAYER_WORK *p_player;
    p_gamedata  = GAMESYSTEM_GetGameData( p_sky->p_gamesys );
    p_player    = GAMEDATA_GetMyPlayerWork( p_gamedata );
    p_sky->p_param->zoneID    = PLAYERWORK_getZoneID(p_player);
    p_sky->p_param->escapeID  = GAMEDATA_GetEscapeLocation( p_gamedata )->zone_id;
    p_sky->p_param->p_gamesys = p_gamesys;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  デバッグ空を飛ぶイベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuSkyJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
    SEQ_CHANGE_MAP,
    SEQ_EXIT,
  };

  DEBUG_SKYJUMP_EVENT_WORK  *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(townmap), &TownMap_ProcData, p_wk->p_param ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    if( p_wk->p_param->select == TOWNMAP_SELECT_SKY )
    {
      *p_seq  = SEQ_CHANGE_MAP;
    }
    else
    {
      *p_seq  = SEQ_EXIT;
    }
    break;

  case SEQ_CHANGE_MAP:
    NAGI_Printf("ゾーンID %d",p_wk->p_param->zoneID);
    GMEVENT_CallEvent( p_wk->p_event, DEBUG_EVENT_QuickChangeMapDefaultPos( p_wk->p_gamesys, p_wk->p_field, p_wk->p_param->zoneID ) );
    *p_seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    if( p_wk->p_param )
    {
      GFL_HEAP_FreeMemory(p_wk->p_param);
    }
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}

//======================================================================
//  デバッグメニュー　自機男女切り替え
//======================================================================
//--------------------------------------------------------------
/**
 * デバッグメニュー　自機男女切り替え
 * @param wk DEBUG_MENU_EVENT_WORK
 * @retval  BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ChangePlayerSex( DEBUG_MENU_EVENT_WORK *wk )
{
  int sex;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldMap = wk->fieldWork;
  u16 zone_id = FIELDMAP_GetZoneID( fieldMap );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( wk->gdata );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( wk->gdata );
  MYSTATUS *s_mystatus = SaveData_GetMyStatus( save );

  sex = MyStatus_GetMySex( mystatus );
  sex ^= 1;
  MyStatus_SetMySex( mystatus, sex );
  MyStatus_SetMySex( s_mystatus, sex );

  {
    GMEVENT * mapchange_event;
    mapchange_event = DEBUG_EVENT_ChangeMapDefaultPos( gsys, fieldMap, zone_id );
    GMEVENT_ChangeEvent( event, mapchange_event );
  }
  return( TRUE );
}

//======================================================================
//  デバッグメニュー　GTS画面へ
//======================================================================
FS_EXTERN_OVERLAY(worldtrade);
#include "net/dwc_rapcommon.h"
//-------------------------------------
/// デバッグGTS用ワーク
//=====================================
typedef struct
{
  GAMESYS_WORK        *p_gamesys;
  GMEVENT             *p_event;
  FIELDMAP_WORK     *p_field;
  EVENT_WIFICLUB_WORK wifi;
  WORLDTRADE_PARAM    gts;
} DEBUG_WIFIGTS_EVENT_WORK;
static GMEVENT_RESULT debugMenuWifiGts( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *  @brief  GTS画面へいくイベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_WifiGts( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  GAMEDATA *p_gamedata      = GAMESYSTEM_GetGameData(p_gamesys);
  DEBUG_WIFIGTS_EVENT_WORK  *p_gts;


    //イヴェント
    GMEVENT_Change( p_event, debugMenuWifiGts, sizeof(DEBUG_WIFIGTS_EVENT_WORK) );
    p_gts = GMEVENT_GetEventWork( p_event );
    GFL_STD_MemClear( p_gts, sizeof(DEBUG_WIFIGTS_EVENT_WORK) );


    //ワーク設定
    p_gts->p_gamesys  = p_gamesys;
    p_gts->p_event    = p_event;
    p_gts->p_field    = p_field;

    //WiFiClubワーク設定
    p_gts->wifi.event     = p_event;
    p_gts->wifi.gsys      = p_gamesys;
    p_gts->wifi.fieldmap  = p_field;
    p_gts->wifi.pWork     = NULL;
    p_gts->wifi.isEndProc = TRUE;
    p_gts->wifi.selectType  = 0;

    //GTSワーク設定
    p_gts->gts.savedata         = GAMEDATA_GetSaveControlWork(p_gamedata );
    p_gts->gts.worldtrade_data  = SaveData_GetWorldTradeData(p_gts->gts.savedata);
    p_gts->gts.systemdata       = SaveData_GetSystemData(p_gts->gts.savedata);
    p_gts->gts.myparty          = SaveData_GetTemotiPokemon(p_gts->gts.savedata);
    p_gts->gts.mybox            = GAMEDATA_GetBoxManager(p_gamedata);
    p_gts->gts.zukanwork        = GAMEDATA_GetZukanSave( p_gamedata );
    p_gts->gts.wifilist         = GAMEDATA_GetWiFiList(p_gamedata);
    p_gts->gts.wifihistory      = SaveData_GetWifiHistory(p_gts->gts.savedata);
    p_gts->gts.mystatus         = SaveData_GetMyStatus(p_gts->gts.savedata);
    p_gts->gts.config           = SaveData_GetConfig(p_gts->gts.savedata);
    p_gts->gts.record           = SaveData_GetRecord(p_gts->gts.savedata);
    p_gts->gts.myitem           = NULL;
    p_gts->gts.gamesys          = p_gamesys;

    p_gts->gts.zukanmode        = 0;
    p_gts->gts.profileId        = WifiList_GetMyGSID( p_gts->gts.wifilist );
    p_gts->gts.contestflag      = FALSE;
    p_gts->gts.connect          = 0;

    OS_Printf( "GTS Start\n" );
    return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  デバッグGTS接続用イベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuWifiGts( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_WIFI,
    SEQ_PROC_END,
    SEQ_CALL_GTS,
    SEQ_EXIT,
  };

  DEBUG_WIFIGTS_EVENT_WORK  *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_WIFI:
    if(GAME_COMM_NO_NULL!= GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(p_wk->wifi.gsys))){
      GameCommSys_ExitReq(GAMESYSTEM_GetGameCommSysPtr(p_wk->wifi.gsys));
    }
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    if(GAME_COMM_NO_NULL == GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(p_wk->wifi.gsys)))
    {
      *p_seq  = SEQ_CALL_GTS;
    }
    break;

  case SEQ_CALL_GTS:
     GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(worldtrade), &WorldTrade_ProcData, &p_wk->gts ) );
    *p_seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}

//======================================================================
//  デバッグメニュー　GDS接続
//======================================================================
#include "net_app/gds_main.h"
FS_EXTERN_OVERLAY(gds_comm);
//-------------------------------------
/// デバッグGDS用ワーク
//=====================================
typedef struct
{
  GAMESYS_WORK        *gsys;
  FIELDMAP_WORK     *fieldWork;
} DEBUG_GDS_EVENT_WORK;
static GMEVENT_RESULT debugMenuGDS( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *  @brief  GTS画面へいくイベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_GDS( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK *gsys = p_wk->gmSys;
  FIELDMAP_WORK *fieldWork = p_wk->fieldWork;
  GAMEDATA *p_gamedata      = GAMESYSTEM_GetGameData(gsys);
  GMEVENT       *p_event    = p_wk->gmEvent;
  DEBUG_GDS_EVENT_WORK *p_gds;

  if( WifiList_CheckMyGSID(GAMEDATA_GetWiFiList(p_gamedata) ) )
  {
    //イヴェント
    GMEVENT_Change( p_event, debugMenuGDS, sizeof(DEBUG_GDS_EVENT_WORK) );
    p_gds = GMEVENT_GetEventWork( p_event );
    p_gds->gsys = gsys;
    p_gds->fieldWork = fieldWork;
  }
  else
  {
    OS_Printf( "GameSpyIDが不正なので、GTSを開始しなかった\n" );
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  デバッグGTS接続用イベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuGDS( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  DEBUG_GDS_EVENT_WORK  *p_gds  = p_wk_adrs;

  switch(*p_seq )
  {
  case 0:
    {
      GDSPROC_PARAM *gds_param;

      gds_param = GFL_HEAP_AllocClearMemory(HEAPID_PROC, sizeof(GDSPROC_PARAM));
      gds_param->gamedata = GAMESYSTEM_GetGameData(p_gds->gsys);
      gds_param->savedata = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(p_gds->gsys));
      gds_param->connect = 0;
      gds_param->gds_mode = 0;

      GMEVENT_CallEvent( p_event, EVENT_FieldSubProc( p_gds->gsys, p_gds->fieldWork,
          FS_OVERLAY_ID(gds_comm), &GdsMainProcData, gds_param ) );
      (*p_seq)++;
    }
    break;

  case 1:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  デバッグメニュー UIテンプレートへ
//======================================================================
FS_EXTERN_OVERLAY(ui_debug);
//-------------------------------------
/// デバッグUIテンプレート用ワーク
//=====================================
typedef struct
{
  GAMESYS_WORK        *p_gamesys;
  GMEVENT             *p_event;
  FIELDMAP_WORK       *p_field;
  UI_TEMPLATE_PARAM   param;
} DEBUG_UITEMPLATE_EVENT_WORK;
static GMEVENT_RESULT debugMenuUITemplate( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *  @brief  UIテンプレート接続
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_UITemplate( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field    = p_wk->fieldWork;
  DEBUG_UITEMPLATE_EVENT_WORK *p_param;

  //イヴェント
  GMEVENT_Change( p_event, debugMenuUITemplate, sizeof(DEBUG_UITEMPLATE_EVENT_WORK) );
  p_param = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_param, sizeof(DEBUG_UITEMPLATE_EVENT_WORK) );

  p_param->p_gamesys  = p_gamesys;
  p_param->param.gamesys  = p_gamesys;
  p_param->p_event    = p_event;
  p_param->p_field    = p_field;

  OS_Printf( "UIテンプレート Start\n" );

  return TRUE;

}
//----------------------------------------------------------------------------
/**
 *  @brief  デバッグGTS接続用イベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuUITemplate( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_EXIT,
  };

  DEBUG_UITEMPLATE_EVENT_WORK *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
      GMEVENT_CallEvent( p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(ui_debug), &UITemplateProcData, &p_wk->param ) );
    *p_seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}

//======================================================================
//  怪力
//======================================================================
//--------------------------------------------------------------
/**
 * 怪力実行
 * @param wk DEBUG_MENU_EVENT_WORK*
 * @retval BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Kairiki( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  EVENTWORK *evwork = GAMEDATA_GetEventWork( wk->gdata );
  EVENTWORK_SetEventFlag( evwork, SYS_FLAG_KAIRIKI );
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　線形カメラ操作
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ControlLinerCamera( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_CTL_LINERCAMERA_WORK *work;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;

  GMEVENT_Change( event, debugMenuControlLinerCamera, sizeof(DEBUG_CTL_LINERCAMERA_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CTL_LINERCAMERA_WORK) );

  work->gsys = gsys;
  work->event = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;

  {
    FIELD_SUBSCREEN_WORK * subscreen;


    // カメラ操作は下画面で行う
    subscreen = FIELDMAP_GetFieldSubscreenWork(work->fieldWork);
    FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
    {
      void * inner_work;
      FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(work->fieldWork);
      inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(subscreen);
      FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_CAMERA_POS, heapID);

      //カメラのバインドを切る
      FIELD_CAMERA_FreeTarget(cam);
    }
  }
  // レールカメラ反映の停止
  {
    FLDNOGRID_MAPPER* mapper;

    if( FIELDMAP_GetBaseSystemType( work->fieldWork ) == FLDMAP_BASESYS_RAIL )
    {
      mapper = FIELDMAP_GetFldNoGridMapper( work->fieldWork );
      FLDNOGRID_MAPPER_SetRailCameraActive( mapper, FALSE );
    }
  }

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：線形カメラ操作
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlLinerCamera(
    GMEVENT *event, int *seq, void *wk )
{
  VecFx32 add = {0,0,0};
  BOOL shift = FALSE;

  DEBUG_CTL_LINERCAMERA_WORK *work = wk;
  FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(work->fieldWork);


  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
    FIELD_CAMERA_DEBUG_ReleaseSubScreen( cam );

    // レールカメラ反映の再開
    {
      FLDNOGRID_MAPPER* mapper;

      if( FIELDMAP_GetBaseSystemType( work->fieldWork ) == FLDMAP_BASESYS_RAIL )
      {
        mapper = FIELDMAP_GetFldNoGridMapper( work->fieldWork );
        FLDNOGRID_MAPPER_SetRailCameraActive( mapper, TRUE );
      }
    }
    //バインド復帰
    FIELD_CAMERA_BindDefaultTarget(cam);
    return( GMEVENT_RES_FINISH );
  }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG ){
    DampCameraInfo(cam);
  }else if( GFL_UI_KEY_GetCont() & PAD_KEY_UP ){             //奥
    shift = LinerCamKeyContCtl(work, -FX32_ONE, &add.z);
  }else if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN ){             //手前
    shift = LinerCamKeyContCtl(work, FX32_ONE, &add.z);
  }else if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT ){             //左
    shift = LinerCamKeyContCtl(work, -FX32_ONE, &add.x);
  }else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT ){             //右
    shift = LinerCamKeyContCtl(work, FX32_ONE, &add.x);
  }else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y ){             //上
    shift = LinerCamKeyContCtl(work, FX32_ONE, &add.y);
  }else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){             //下
    shift = LinerCamKeyContCtl(work, -FX32_ONE, &add.y);
  }else{
    work->Wait = 0;
    work->Count = 0;
  }
  if(shift){
    VecFx32 pos;
    FIELD_CAMERA_GetTargetPos( cam, &pos );
    VEC_Add( &pos, &add, &pos );
    FIELD_CAMERA_SetTargetPos( cam, &pos );
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * イベント：線形カメラ操作キー入力
 * @param work      カメラワーク
 * @param inAddval  気入力結果の際の加算値
 * @param outVal    格納バッファ
 * @retval  BOOL    加算したらTRUE
 */
//--------------------------------------------------------------
static BOOL LinerCamKeyContCtl(DEBUG_CTL_LINERCAMERA_WORK *work, const fx32 inAddVal, fx32 *outVal)
{
  if (work->Wait >= LINER_CAM_KEY_WAIT){
    *outVal = inAddVal;
    return TRUE;
  }else{
    work->Wait++;
    if (work->Count>0){
      work->Count--;
    }else{
      *outVal = inAddVal;
      work->Count = LINER_CAM_KEY_COUNT;
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * イベント：線形カメラ操作 情報ダンプ
 * @param cam   フィールドカメラポインタ
 * @retval  none
 */
//--------------------------------------------------------------
static void DampCameraInfo(FIELD_CAMERA * cam)
{
  u16 pitch;
  u16 yaw;
  fx32 len;
  VecFx32 target;
  u16 fovy;
  VecFx32 shift;

  OS_Printf("--DUMP_FIELD_CAMERA_PARAM--\n");

  pitch = FIELD_CAMERA_GetAnglePitch(cam);
  yaw = FIELD_CAMERA_GetAngleYaw(cam);
  len = FIELD_CAMERA_GetAngleLen(cam);
  fovy = FIELD_CAMERA_GetFovy(cam);
  FIELD_CAMERA_GetTargetOffset( cam, &shift );
  FIELD_CAMERA_GetTargetPos( cam, &target );

  OS_Printf("%d,%d,%d,%d,%d,%d\n", pitch, yaw, len, target.x, target.y, target.z );
#ifdef DEBUG_ONLY_FOR_saitou
  OS_Printf("%d,%d,%d,%d\n", fovy, shift.x, shift.y, shift.z );
#endif
}

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　Yボタン登録操作
 * @param wk  DEBUG_MENU_EVENT_WORK*  ワーク
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenu_ControlShortCut( DEBUG_MENU_EVENT_WORK *wk )
{
  int i;
  GAMESYS_WORK *gsys = wk->gmSys;
  SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork(wk->gdata);
  SHORTCUT *p_shortcut_sv = SaveData_GetShortCut( p_sv );
  for( i = 0; i < SHORTCUT_ID_MAX; i++ )
  {
    SHORTCUT_SetRegister( p_shortcut_sv, i, TRUE );
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ビーコンでの友達コード配信
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ワーク
 *
 *  @return TRUEイベント継続
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_BeaconFriendCode( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT * new_event;
  new_event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
      FS_OVERLAY_ID( debug_beacon ), EVENT_DebugBeacon, wk->fieldWork );
  //new_event = EVENT_DebugBeacon( wk->gmSys, wk->fieldWork );
  GMEVENT_ChangeEvent( wk->gmEvent, new_event );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  WiFi世界対戦へ接続
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ワーク
 *
 *  @return TRUEイベント継続
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_WifiBattleMatch( DEBUG_MENU_EVENT_WORK *wk )
{
  WIFIBATTLEMATCH_MODE mode = WIFIBATTLEMATCH_MODE_RANDOM;
  WIFIBATTLEMATCH_BTLRULE btl_rule  = WIFIBATTLEMATCH_BTLRULE_SINGLE;
#ifdef PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L && GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    btl_rule  = WIFIBATTLEMATCH_BTLRULE_ROTATE;
  }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    btl_rule  = WIFIBATTLEMATCH_BTLRULE_DOUBLE;
  }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    btl_rule  = WIFIBATTLEMATCH_BTLRULE_TRIPLE;
  }
#endif

  GMEVENT_ChangeEvent( wk->gmEvent, EVENT_WifiBattleMatch( wk->gmSys, wk->fieldWork, DEBUG_MODE(mode), WIFIBATTLEMATCH_POKE_BTLBOX, btl_rule ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  サケサーバーデバッグモードへ
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ワーク
 *
 *  @return TRUEでイベンﾄ継続
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugSake( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GMEVENT_ChangeEvent( p_wk->gmEvent, EVENT_DEBUG_WifiMatch( p_wk->gmSys, p_wk->fieldWork, DEBUG_WIFIBATTLEMATCH_MODE_SAKE ) );
  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  アトラスサーバーデバッグモードへ
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ワーク
 *
 *  @return TRUEでイベンﾄ継続
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugAtlas( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GMEVENT_ChangeEvent( p_wk->gmEvent, EVENT_DEBUG_WifiMatch( p_wk->gmSys, p_wk->fieldWork, DEBUG_WIFIBATTLEMATCH_MODE_ATLAS ) );
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief   デバッグメニュー：ジオネット呼び出し
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Geonet( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT_ChangeEvent( p_wk->gmEvent, EVENT_Geonet( p_wk->gmSys ) );
  return TRUE;
}
//======================================================================
//  バトルレコーダー
//======================================================================
#include "net_app/battle_recorder.h"
FS_EXTERN_OVERLAY( battle_recorder );

static GMEVENT_RESULT debugBattleRecorder( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//-------------------------------------
/// バトルレコーダーPROCイベントワーク
//=====================================
typedef struct
{
  HEAPID heapID;
  GAMESYS_WORK    *p_gamesys;
  GMEVENT         *p_event;
  FIELDMAP_WORK   *p_field;
  BATTLERECORDER_PARAM  param;
} DEBUG_BR_EVENT_WORK;

//----------------------------------------------------------------------------
/**
 *  @brief  バトルレコーダーへ
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ワーク
 *
 *  @return TRUEでイベンﾄ継続
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_BattleRecorder( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_BR_EVENT_WORK *p_param;

  //イヴェント
  GMEVENT_Change( p_event, debugBattleRecorder, sizeof(DEBUG_BR_EVENT_WORK) );
  p_param = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_param, sizeof(DEBUG_BR_EVENT_WORK) );

  //ワーク設定
  p_param->p_gamesys  = p_gamesys;
  p_param->p_event    = p_event;
  p_param->p_field    = p_field;
  p_param->heapID     = heapID;
  p_param->param.p_gamedata = GAMESYSTEM_GetGameData(p_gamesys);
  p_param->param.mode       = BR_MODE_BROWSE;

  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    p_param->param.mode  = BR_MODE_GLOBAL_BV;
  }
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    p_param->param.mode  = BR_MODE_GLOBAL_MUSICAL;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  バトルレコーダー呼び出しイベント
 *
 *  @param  GMEVENT *p_event  イベント
 *  @param  *p_seq      シーケンス
 *  @param  *p_wk_adrs  ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugBattleRecorder( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
  };

  DEBUG_BR_EVENT_WORK *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( p_wk->p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
        FS_OVERLAY_ID(battle_recorder), &BattleRecorder_ProcData, &p_wk->param ) );
    *p_seq  = SEQ_PROC_END;
    break;

  case SEQ_PROC_END:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}

//-----------------------------------------------------------------------------
/**
 *    グラフィッカー用
 *      カメラ　こまかく設定
 */
//-----------------------------------------------------------------------------
//-------------------------------------
/// デバッグカメラ　こまかく設定用ワーク
//=====================================
typedef struct
{
  FIELDMAP_WORK*  p_field;
  FIELD_CAMERA*   p_camera;
  HEAPID heapID;

  BOOL draw_help;
  BOOL param_change;

  GFL_BMPWIN* p_win;
} DEBUG_DELICATE_CAMERA_EVENT_WORK;
static GMEVENT_RESULT debugMenuDelicateCamera( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );

//----------------------------------------------------------------------------
/**
 *  @brief  起動
 *
 *  @param  wk
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_ControlDelicateCamera( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_DELICATE_CAMERA_EVENT_WORK* p_work;
  GMEVENT* p_event = wk->gmEvent;
  FIELDMAP_WORK* p_fieldWork = wk->fieldWork;
  HEAPID heapID = wk->heapID;

  GMEVENT_Change( p_event, debugMenuDelicateCamera, sizeof(DEBUG_DELICATE_CAMERA_EVENT_WORK) );
  p_work = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_work, sizeof(DEBUG_DELICATE_CAMERA_EVENT_WORK) );

  p_work->p_field   = p_fieldWork;
  p_work->p_camera  = FIELDMAP_GetFieldCamera( p_fieldWork );
  p_work->heapID    = heapID;

  FIELD_CAMERA_DEBUG_InitControl( p_work->p_camera, heapID );

  // レールカメラ反映の停止
  {
    FLDNOGRID_MAPPER* mapper;

    if( FIELDMAP_GetBaseSystemType( p_fieldWork ) == FLDMAP_BASESYS_RAIL )
    {
      mapper = FIELDMAP_GetFldNoGridMapper( p_fieldWork );
      FLDNOGRID_MAPPER_SetRailCameraActive( mapper, FALSE );
    }
  }


  // 表示面の作成
  {
    const FLDMAPPER* cp_mapper = FIELDMAP_GetFieldG3Dmapper( p_fieldWork );
    fx32 map_size_x, map_size_z;

    FLDMAPPER_GetSize( cp_mapper, &map_size_x, &map_size_z );

    // インフォーバーの非表示
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(p_fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // ビットマップウィンドウ初期化
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // パレット情報を転送
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, heapID );

      // ビットマップウィンドウを作成
      p_work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( p_work->p_win );
      GFL_BMPWIN_TransVramCharacter( p_work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // ウィンドウ
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, heapID );
      BmpWinFrame_Write( p_work->p_win, TRUE, 1, 15 );

      FIELD_CAMERA_DEBUG_DrawInfo( p_work->p_camera, p_work->p_win, map_size_x, map_size_z );
    }
  }

  return( TRUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  カメラ　詳細操作　メイン
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuDelicateCamera( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  DEBUG_DELICATE_CAMERA_EVENT_WORK* p_work = p_wk_adrs;
  BOOL result;
  int trg = GFL_UI_KEY_GetTrg();
  int cont = GFL_UI_KEY_GetCont();
  int repeat = GFL_UI_KEY_GetRepeat();
  const FLDMAPPER* cp_mapper = FIELDMAP_GetFieldG3Dmapper( p_work->p_field );
  fx32 map_size_x, map_size_z;

  FLDMAPPER_GetSize( cp_mapper, &map_size_x, &map_size_z );

  // SELECT終了
  if( trg & PAD_BUTTON_SELECT )
  {
    // ビットマップウィンドウ破棄
    {
      BmpWinFrame_Clear( p_work->p_win, TRUE );
      GFL_BMPWIN_Delete( p_work->p_win );
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
    }

    FIELD_CAMERA_DEBUG_ExitControl( p_work->p_camera );

    // インフォーバーの表示
    FIELDMAP_SetFieldSubscreenWork(p_work->p_field,
        FIELD_SUBSCREEN_Init( p_work->heapID, p_work->p_field, FIELD_SUBSCREEN_NORMAL ));
    return ( GMEVENT_RES_FINISH );
  }

  // DEBUGで、スクリプト操作用の表示を出す。
  // バッファリングモードも変わってしまいますが・・・。
  if( trg & PAD_BUTTON_DEBUG )
  {
    VecFx32 offset;
    VecFx32 target;
    u16 pitch;
    u16 yaw;
    fx32 len;

    // 線形カメラ用表示パラメータをカメラに設定
    FIELD_CAMERA_GetTargetOffset( p_work->p_camera, &offset );
    FIELD_CAMERA_GetTargetPos( p_work->p_camera, &target );
    pitch = FIELD_CAMERA_GetAnglePitch( p_work->p_camera );
    yaw   = FIELD_CAMERA_GetAngleYaw( p_work->p_camera );
    len   = FIELD_CAMERA_GetAngleLen( p_work->p_camera );
    FIELD_CAMERA_DEBUG_SetUpLinerCameraInfoData( p_work->p_camera );

    // 平行移動も含めた座標を返す。
    DampCameraInfo( p_work->p_camera );

    // 元に戻す
    FIELD_CAMERA_SetTargetPos( p_work->p_camera, &target );
    FIELD_CAMERA_SetTargetOffset( p_work->p_camera, &offset );
    FIELD_CAMERA_SetAnglePitch( p_work->p_camera, pitch );
    FIELD_CAMERA_SetAngleYaw( p_work->p_camera, yaw );
    FIELD_CAMERA_SetAngleLen( p_work->p_camera, len );
  }


  // カメラデバック入力
  result = FIELD_CAMERA_DEBUG_Control( p_work->p_camera, trg, cont, repeat );

  // タッチでヘルプ表示切替
  if( GFL_UI_TP_GetTrg() )
  {
    p_work->draw_help ^= 1;
    result = TRUE;
  }

  // ヘルプ表示
  if( result )
  {
    p_work->param_change = TRUE;
  }
  // 描画が重いので、操作が完了してから描画更新
  else if( p_work->param_change )
  {
    p_work->param_change = FALSE;
    if( p_work->draw_help )
    {
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_work->p_win ), 0xf );
      FIELD_CAMERA_DEBUG_DrawControlHelp( p_work->p_camera, p_work->p_win );
    }
    else
    {
      // カメラ情報を更新
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_work->p_win ), 0xf );
      FIELD_CAMERA_DEBUG_DrawInfo( p_work->p_camera, p_work->p_win, map_size_x, map_size_z );
    }
  }


  return( GMEVENT_RES_CONTINUE );
}




//======================================================================
//  技思い出し画面テスト呼び出し
//======================================================================
FS_EXTERN_OVERLAY(waza_oshie);
//-------------------------------------
/// 技思い出し画面テスト用ワーク
//=====================================
typedef struct
{
  GAMESYS_WORK        *p_gamesys;
  GMEVENT             *p_event;
  FIELDMAP_WORK       *p_field;
  WAZAOSHIE_DATA      param;
} DEBUG_WAZAOSHIE_EVENT_WORK;
static GMEVENT_RESULT debugMenuWazaOshie( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );

//----------------------------------------------------------------------------
/**
 *  @brief  技思い出し画面
 *
 *  @param  DEBUG_MENU_EVENT_WORK *p_wk   ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_WazaOshie( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *p_gamesys  = p_wk->gmSys;
  GMEVENT       *p_event    = p_wk->gmEvent;
  FIELDMAP_WORK *p_field    = p_wk->fieldWork;
  DEBUG_WAZAOSHIE_EVENT_WORK  *p_param;
  GAMEDATA *gmData = GAMESYSTEM_GetGameData(p_gamesys);
  POKEPARTY *party = GAMEDATA_GetMyPokemon(gmData);
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gmData);

  // イベント起動
  GMEVENT_Change( p_event, debugMenuWazaOshie, sizeof(DEBUG_WAZAOSHIE_EVENT_WORK) );
  p_param = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_param, sizeof(DEBUG_WAZAOSHIE_EVENT_WORK) );

  p_param->p_gamesys     = p_gamesys;
  p_param->p_event       = p_event;
  p_param->p_field       = p_field;

//  p_param->param.gsys    = p_gamesys;
  p_param->param.pp   = PokeParty_GetMemberPointer( party, 0 );
  p_param->param.myst = SaveData_GetMyStatus(sv);   // 自分データ
  p_param->param.cfg  = SaveData_GetConfig(sv);     // コンフィグデータ
  p_param->param.gsys = p_gamesys;
  p_param->param.waza_tbl   = WAZAOSHIE_GetRemaindWaza( p_param->param.pp, HEAPID_WORLD );

  OS_Printf( "技教え Start\n" );

  return TRUE;

}
//----------------------------------------------------------------------------
/**
 *  @brief  デバッグ技思い出し画面接続用イベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuWazaOshie( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_EXIT,
  };

  DEBUG_WAZAOSHIE_EVENT_WORK  *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:

    GMEVENT_CallEvent( p_event, EVENT_FieldSubProc( p_wk->p_gamesys, p_wk->p_field,
                                                    FS_OVERLAY_ID(waza_oshie), &WazaOshieProcData, &p_wk->param ) );

    *p_seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}


//======================================================================
//  使用メモリの表示（主要な）
//======================================================================
//-------------------------------------
/// 使用メモリの表示（主要な）
//=====================================
typedef struct
{
  FIELDMAP_WORK       *p_field;
  GFL_BMPWIN* p_win;
  HEAPID heapID;

  // MESSAGEデータ
  WORDSET*    p_debug_wordset;
  GFL_FONT*   p_debug_font;
  GFL_MSGDATA*  p_debug_msgdata;
  STRBUF*     p_debug_strbuff;
  STRBUF*     p_debug_strbuff_tmp;
} DEBUG_USEMEMORY_EVENT_WORK;
static GMEVENT_RESULT debugMenuUseMemoryDump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
static void debugMenuWriteUseMemoryDump( DEBUG_USEMEMORY_EVENT_WORK* p_wk );


//----------------------------------------------------------------------------
/**
 *  @brief  メモリ使用情報表示
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_UseMemoryDump( DEBUG_MENU_EVENT_WORK *p_wk )
{
  DEBUG_USEMEMORY_EVENT_WORK* p_work;
  GMEVENT* p_event = p_wk->gmEvent;
  FIELDMAP_WORK* p_fieldWork = p_wk->fieldWork;
  HEAPID heapID = p_wk->heapID;

  GMEVENT_Change( p_event, debugMenuUseMemoryDump, sizeof(DEBUG_USEMEMORY_EVENT_WORK) );
  p_work = GMEVENT_GetEventWork( p_event );
  GFL_STD_MemClear( p_work, sizeof(DEBUG_USEMEMORY_EVENT_WORK) );

  p_work->p_field   = p_fieldWork;
  p_work->heapID    = heapID;

  // MESSAGE情報読み込み
  {
    // ワードセット作成
    p_work->p_debug_wordset = WORDSET_Create( heapID );
    p_work->p_debug_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_field_dat, heapID );

    p_work->p_debug_strbuff    = GFL_STR_CreateBuffer( 256, heapID );
    p_work->p_debug_strbuff_tmp  = GFL_STR_CreateBuffer( 256, heapID );

    // フォントデータ
    p_work->p_debug_font = GFL_FONT_Create(
      ARCID_FONT, NARC_font_large_gftr,
      GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
  }


  // 表示面の作成
  {
    // インフォーバーの非表示
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(p_fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // ビットマップウィンドウ初期化
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // パレット情報を転送
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, heapID );

      // ビットマップウィンドウを作成
      p_work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( p_work->p_win );
      GFL_BMPWIN_TransVramCharacter( p_work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // ウィンドウ
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, heapID );
      BmpWinFrame_Write( p_work->p_win, TRUE, 1, 15 );
    }

    // 描画処理
    {
      debugMenuWriteUseMemoryDump( p_work );
    }
  }

  return( TRUE );
}


//----------------------------------------------------------------------------
/**
 *  @brief  メモリーダンプメイン
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuUseMemoryDump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  DEBUG_USEMEMORY_EVENT_WORK* p_work = p_wk_adrs;

  // 表示OFF
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    // 全破棄
    {
      BmpWinFrame_Clear( p_work->p_win, TRUE );
      GFL_BMPWIN_Delete( p_work->p_win );
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
    }

    // MESSAGE情報破棄
    {
      // フォントデータ
      GFL_FONT_Delete( p_work->p_debug_font );
      p_work->p_debug_font = NULL;

      GFL_MSG_Delete( p_work->p_debug_msgdata );
      p_work->p_debug_msgdata = NULL;

      WORDSET_Delete( p_work->p_debug_wordset );
      p_work->p_debug_wordset = NULL;

      GFL_STR_DeleteBuffer( p_work->p_debug_strbuff );
      p_work->p_debug_strbuff = NULL;
      GFL_STR_DeleteBuffer( p_work->p_debug_strbuff_tmp );
      p_work->p_debug_strbuff_tmp = NULL;
    }
    // インフォーバーの表示
    FIELDMAP_SetFieldSubscreenWork(p_work->p_field,
        FIELD_SUBSCREEN_Init( p_work->heapID, p_work->p_field, FIELD_SUBSCREEN_NORMAL ));

    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  メモリーダンプ表示
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void debugMenuWriteUseMemoryDump( DEBUG_USEMEMORY_EVENT_WORK* p_wk )
{
  /* 1段目 */
  // 配置オブジェクト
#ifdef BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 1, FIELD_BMODEL_MAN_GetUseResourceMemorySize(), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

#else // BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 1, 0, 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

#endif // BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

  // 動作オブジェ
#ifdef DEBUG_MMDL_RESOURCE_MEMORY_SIZE

  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 3, DEBUG_MMDL_GetUseResourceMemorySize(), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

#else // DEBUG_MMDL_RESOURCE_MEMORY_SIZE

  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 3, 0, 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

#endif  // DEBUG_MMDL_RESOURCE_MEMORY_SIZE

  // FIELDヒープの残り
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 4, GFI_HEAP_GetHeapFreeSize(p_wk->heapID), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 5, GFI_HEAP_GetHeapAllocatableSize(p_wk->heapID), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

  // 表示
  GFL_MSG_GetString( p_wk->p_debug_msgdata, DEBUG_FIELD_STR54, p_wk->p_debug_strbuff_tmp );

  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_wk->p_win ), 0, 0, p_wk->p_debug_strbuff, p_wk->p_debug_font );
  


  /* 2段目 */
  // HEAPID FIELD_PRBUF
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 0, GFI_HEAP_GetHeapFreeSize(HEAPID_FIELD_PRBUF), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 1, GFI_HEAP_GetHeapAllocatableSize(HEAPID_FIELD_PRBUF), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  // HEAPID SUBSCREEN
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 2, GFI_HEAP_GetHeapFreeSize(HEAPID_FIELD_SUBSCREEN), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterHexNumber( p_wk->p_debug_wordset, 3, GFI_HEAP_GetHeapAllocatableSize(HEAPID_FIELD_SUBSCREEN), 8, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );

  
  // 表示
  GFL_MSG_GetString( p_wk->p_debug_msgdata, DEBUG_FIELD_STR68, p_wk->p_debug_strbuff_tmp );

  WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_wk->p_win ), 96, 0, p_wk->p_debug_strbuff, p_wk->p_debug_font );

  

  GFL_BMPWIN_TransVramCharacter( p_wk->p_win );
}



//----------------------------------------------------------------------------
/**
 *  @brief  季節表示
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_SeasonDisplay( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*         parent = wk->gmEvent;
  GAMESYS_WORK*      gsys = wk->gmSys;
  FIELDMAP_WORK* fieldmap = wk->fieldWork;
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );

  GMEVENT_ChangeEvent( parent, DEBUG_EVENT_FLDMENU_SeasonDispSelect( gsys, heap_id ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  あなぬけのヒモ
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Ananukenohimo( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*      parent = wk->gmEvent;
  GAMESYS_WORK* gsys = wk->gmSys;
  FIELDMAP_WORK* fieldmap = wk->fieldWork;

  GMEVENT_ChangeEvent( parent, EVENT_ChangeMapByAnanukenohimo( fieldmap, gsys ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  あなをほる
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Anawohoru( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*      parent = wk->gmEvent;
  GAMESYS_WORK* gsys = wk->gmSys;

  GMEVENT_ChangeEvent( parent, EVENT_ChangeMapByAnawohoru( gsys ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  テレポート
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Teleport( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT*      parent = wk->gmEvent;
  GAMESYS_WORK* gsys = wk->gmSys;

  GMEVENT_ChangeEvent( parent, EVENT_ChangeMapByTeleport( gsys ) );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  フィールド技：ダイビング
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Diving( DEBUG_MENU_EVENT_WORK *wk )
{
  if( FIELDMAP_GetMapControlType(wk->fieldWork) != FLDMAP_CTRLTYPE_GRID )
  {
    FIELD_PLAYER* p_fld_player = FIELDMAP_GetFieldPlayer( wk->fieldWork );

    // レール上ではこっち
    FIELD_PLAYER_SetRequest( p_fld_player, FIELD_PLAYER_REQBIT_DIVING );
    return FALSE;
  }
  SCRIPT_ChangeScript( wk->gmEvent, SCRID_HIDEN_DIVING_MENU, NULL, HEAPID_FIELDMAP );
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief 3Dデモ再生
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_Demo3d( DEBUG_MENU_EVENT_WORK* wk )
{
  GMEVENT*         parent = wk->gmEvent;
  GAMESYS_WORK*      gsys = wk->gmSys;
  FIELDMAP_WORK* fieldmap = wk->fieldWork;
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );

  GMEVENT_ChangeEvent( parent, DEBUG_EVENT_FLDMENU_Demo3DSelect( gsys, heap_id ) );

  return TRUE;
}

//--------------------------------------------------------------
/**
 * 強制セーブ
 *
 * @param   wk
 *
 * @retval  BOOL    TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ForceSave( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  GMEVENT *event;
  FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
  GFL_MSGDATA *msgdata;
  FLDMSGWIN *fldmsgwin;

  msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_field_dat, heapID );

  fldmsgwin = FLDMSGWIN_Add(msgBG, msgdata, 1, 19, 30, 4);

  FLDMSGWIN_Print( fldmsgwin, 0, 0, DEBUG_FIELD_STR60_message );
  FLDMSGBG_AllPrint(msgBG);

  //セーブ
  GAMEDATA_Save(GAMESYSTEM_GetGameData(gsys));

  FLDMSGWIN_Delete(fldmsgwin);
  GFL_MSG_Delete(msgdata);

  return( FALSE );
}

//--------------------------------------------------------------
/**
 * 着信音
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_RingTone( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GAMESYS_WORK * gsys = p_wk->gmSys;
  GAMEDATA* gamedata  = GAMESYSTEM_GetGameData( gsys );
  FIELD_SOUND * fsnd  = GAMEDATA_GetFieldSound( gamedata );
  
  if ( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE3 ) == FALSE ) {
    FSND_RequestTVTRingTone( fsnd );
  } else {
    FSND_StopTVTRingTone( fsnd );
  }
  return FALSE;
}

//======================================================================
//  デバッグメニュー　キャプチャ
//======================================================================
//--------------------------------------------------------------
/// DEBUG_CAPTURELIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
  int select_mode;
}DEBUG_CAPTURELIST_EVENT_WORK;


//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuCaptureListEvent(
    GMEVENT *event, int *seq, void *work );

enum{
  CAPTURE_MODE_NORMAL,
  CAPTURE_MODE_PLAYER_OFF,
  CAPTURE_MODE_NPC_OFF,
};

///テストカメラリスト メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_CaptureList =
{
  1,    //リスト項目数
  3,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

///てんきメニューリスト
static const FLDMENUFUNC_LIST DATA_CaptureMenuList[] =
{
  { DEBUG_FIELD_STR61_000, (void*)CAPTURE_MODE_NORMAL },
  { DEBUG_FIELD_STR61_001, (void*)CAPTURE_MODE_PLAYER_OFF },
  { DEBUG_FIELD_STR61_002, (void*)CAPTURE_MODE_NPC_OFF },
};

static const DEBUG_MENU_INITIALIZER DebugCaptureMenuListData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_CaptureMenuList),
  DATA_CaptureMenuList,
  &DATA_DebugMenuList_CaptureList,
  1, 1, 12, 11,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　天気リスト
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_CaptureList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_CAPTURELIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuCaptureListEvent, sizeof(DEBUG_CAPTURELIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_CAPTURELIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：てんきリスト
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuCaptureListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_CAPTURELIST_EVENT_WORK *work = wk;
  MMDL *fmmdl;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugCaptureMenuListData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }

      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){  //決定
        return GMEVENT_RES_FINISH;
      }

      work->select_mode = ret;
      (*seq)++;
    }
    break;
  case 2:
    switch(work->select_mode){
    case CAPTURE_MODE_NORMAL:
      break;
    case CAPTURE_MODE_PLAYER_OFF:
      {
        FIELD_PLAYER *player = FIELDMAP_GetFieldPlayer( work->fieldWork );
        fmmdl = FIELD_PLAYER_GetMMdl( player );
        MMDL_SetStatusBitVanish( fmmdl, TRUE );
      }
      break;
    case CAPTURE_MODE_NPC_OFF:
      {
        u32 no = 0;
        BOOL ret;
        MMDLSYS *fos = FIELDMAP_GetMMdlSys( work->fieldWork );

        do{
          ret = MMDLSYS_SearchUseMMdl(fos, &fmmdl, &no );
          if(ret == TRUE){
            MMDL_SetStatusBitVanish( fmmdl, TRUE );
          }
        }while(ret == TRUE);
      }
      break;
    }
    (*seq)++;
    break;
  case 3:
    MI_CpuClearFast( (void*)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE );
    GX_SetCapture(
        GX_CAPTURE_SIZE_256x192,      // キャプチャサイズ
        GX_CAPTURE_MODE_A,      // キャプチャモード
        GX_CAPTURE_SRCA_3D,       // キャプチャブレンドA
        GX_CAPTURE_SRCB_VRAM_0x00000,       // キャプチャブレンドB
        GX_CAPTURE_DEST_VRAM_D_0x00000,     // 転送Vram
        0,            // ブレンド係数A
        0);           // ブレンド係数B

    //LCDCにメモリマッピング
    GX_SetBankForLCDC(GX_VRAM_LCDC_D);
  //  GX_SetGraphicsMode(GX_DISPMODE_VRAM_C, GX_BGMODE_0, GX_BG0_AS_3D);

    return GMEVENT_RES_FINISH;
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * @brief   移動ポケモンデバッグ
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_DebugMvPokemon( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event;

  event = GMEVENT_CreateOverlayEventCall( wk->gmSys,
    FS_OVERLAY_ID( d_iwasawa ), EVENT_DebugMovePoke, wk );

  GMEVENT_ChangeEvent( wk->gmEvent, event );

  return( TRUE );
}



//-----------------------------------------------------------------------------
// ビルボードカラー　の調整
//-----------------------------------------------------------------------------

//--------------------------------------------------------------
/// DEBUG_CTLIGHT_WORK ライト操作ワーク
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK *gsys;
  GMEVENT *event;
  HEAPID heapID;
  FIELDMAP_WORK *fieldWork;

  GFL_BMPWIN* p_win;
}DEBUG_BBDCOLOR_WORK;

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuControlBbdColor(
    GMEVENT *event, int *seq, void *wk );

//----------------------------------------------------------------------------
/**
 *  @brief  ビルボードカラーを指定
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_BBDColor( DEBUG_MENU_EVENT_WORK *wk )
{
  DEBUG_BBDCOLOR_WORK *work;
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;

  GMEVENT_Change( event, debugMenuControlBbdColor, sizeof(DEBUG_BBDCOLOR_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_BBDCOLOR_WORK) );

  work->gsys = gsys;
  work->event = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}


static GMEVENT_RESULT debugMenuControlBbdColor(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_BBDCOLOR_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    {
      HEAPID heapID = FIELDMAP_GetHeapID( work->fieldWork );
      FLD_BBD_COLOR* p_color = FLD_BBD_COLOR_Create( heapID );
      GFL_BBDACT_SYS* p_bbd_act = FIELDMAP_GetBbdActSys( work->fieldWork );
      GFL_BBD_SYS* p_bbd_sys = GFL_BBDACT_GetBBDSystem( p_bbd_act );
      AREADATA* p_areadata = FIELDMAP_GetAreaData( work->fieldWork );

      FLD_BBD_COLOR_Load( p_color, AREADATA_GetBBDColor(p_areadata) );

      // モデル管理開始
      FLD_BBD_COLOR_DEBUG_Init( p_bbd_sys, p_color, work->heapID );

      FLD_BBD_COLOR_Delete( p_color );
    }

    // インフォーバーの非表示
    FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(work->fieldWork));
    GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

    // ビットマップウィンドウ初期化
    {
      static const GFL_BG_BGCNT_HEADER header_sub3 = {
        0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
        GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
      };

      GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
      GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
      GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

      // パレット情報を転送
      GFL_ARC_UTIL_TransVramPalette(
        ARCID_FONT, NARC_font_default_nclr,
        PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, work->heapID );

      // ビットマップウィンドウを作成
      work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
        1, 1, 30, 22,
        FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
      GFL_BMPWIN_MakeScreen( work->p_win );
      GFL_BMPWIN_TransVramCharacter( work->p_win );
      GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );

      // ウィンドウ
      BmpWinFrame_GraphicSet( FIELD_SUBSCREEN_BGPLANE, 1, 15, 0, work->heapID );
      BmpWinFrame_Write( work->p_win, TRUE, 1, 15 );
    }

    FLD_BBD_COLOR_DEBUG_PrintData( work->p_win );

    (*seq)++;
  case 1:
    // ライト管理メイン
    FLD_BBD_COLOR_DEBUG_Control();
    FLD_BBD_COLOR_DEBUG_PrintData( work->p_win );

    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
      (*seq)++;
    }
    break;
  case 2:
    FLD_BBD_COLOR_DEBUG_Exit();

    // ビットマップウィンドウ破棄
    {
      GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
      GFL_BMPWIN_Delete( work->p_win );
    }

    // インフォーバーの表示
    FIELDMAP_SetFieldSubscreenWork(work->fieldWork,
        FIELD_SUBSCREEN_Init( work->heapID, work->fieldWork, FIELD_SUBSCREEN_NORMAL ));

    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}


//======================================================================
//  デバッグメニュー　エンカウントエフェクトリスト
//======================================================================
//--------------------------------------------------------------
/// DEBUG_ENCEFF_LIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DEBUG_ENCEFF_LIST_EVENT_WORK;

///リスト最大
#define ENCEFFLISTMAX (35)

///リスト メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_EncEffList =
{
  1,    //リスト項目数
  6,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

///メニューリスト
static const FLDMENUFUNC_LIST DATA_EncEffMenuList[ENCEFFLISTMAX] =
{
  { DEBUG_FIELD_ENCEFF01, (void*)0 },
  { DEBUG_FIELD_ENCEFF02, (void*)1 },
  { DEBUG_FIELD_ENCEFF03, (void*)2 },
  { DEBUG_FIELD_ENCEFF04, (void*)3 },
  { DEBUG_FIELD_ENCEFF05, (void*)4 },
  { DEBUG_FIELD_ENCEFF06, (void*)5 },
  { DEBUG_FIELD_ENCEFF07, (void*)6 },
  { DEBUG_FIELD_ENCEFF08, (void*)7 },
  { DEBUG_FIELD_ENCEFF09, (void*)8 },
  { DEBUG_FIELD_ENCEFF10, (void*)9 },
  { DEBUG_FIELD_ENCEFF11, (void*)10 },
  { DEBUG_FIELD_ENCEFF12, (void*)11 },
  { DEBUG_FIELD_ENCEFF13, (void*)12 },
  { DEBUG_FIELD_ENCEFF14, (void*)13 },
  { DEBUG_FIELD_ENCEFF15, (void*)14 },
  { DEBUG_FIELD_ENCEFF16, (void*)15 },
  { DEBUG_FIELD_ENCEFF17, (void*)16 },
  { DEBUG_FIELD_ENCEFF18, (void*)17 },
  { DEBUG_FIELD_ENCEFF19, (void*)18 },
  { DEBUG_FIELD_ENCEFF20, (void*)19 },
  { DEBUG_FIELD_ENCEFF21, (void*)20 },
  { DEBUG_FIELD_ENCEFF22, (void*)21 },
  { DEBUG_FIELD_ENCEFF23, (void*)22 },
  { DEBUG_FIELD_ENCEFF24, (void*)23 },
  { DEBUG_FIELD_ENCEFF25, (void*)24 },
  { DEBUG_FIELD_ENCEFF26, (void*)25 },
  { DEBUG_FIELD_ENCEFF27, (void*)26 },
  { DEBUG_FIELD_ENCEFF28, (void*)27 },
  { DEBUG_FIELD_ENCEFF30, (void*)28 },
  { DEBUG_FIELD_ENCEFF31, (void*)29 },
  { DEBUG_FIELD_ENCEFF29, (void*)30 },
  { DEBUG_FIELD_ENCEFF32, (void*)31 },
  { DEBUG_FIELD_ENCEFF33, (void*)32 },
  { DEBUG_FIELD_ENCEFF34, (void*)33 },
  { DEBUG_FIELD_ENCEFF35, (void*)34 },
};

static const DEBUG_MENU_INITIALIZER DebugEncEffMenuListData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_EncEffMenuList),
  DATA_EncEffMenuList,
  &DATA_DebugMenuList_EncEffList, //流用
  1, 1, 16, 11,
  NULL,
  NULL
};


//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuEncEffListEvent( GMEVENT *event, int *seq, void *wk );


//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　エンカウントエフェクト
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_EncEffList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_ENCEFF_LIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuEncEffListEvent, sizeof(DEBUG_ENCEFF_LIST_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_ENCEFF_LIST_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：エンカウントエフェクトリスト
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuEncEffListEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_ENCEFF_LIST_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugEncEffMenuListData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }

      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret != FLDMENUFUNC_CANCEL ) //決定
      {
        //エンカウントエフェクトコール
        OS_Printf("enceff= %d\n",ret);
        ENCEFF_SetEncEff(FIELDMAP_GetEncEffCntPtr(work->fieldWork), event, ret);
        (*seq)++;
        return( GMEVENT_RES_CONTINUE );
      }
      else return( GMEVENT_RES_FINISH );
    }
    break;
  case 2:
    {
      ENCEFF_CNT_PTR cnt_ptr = FIELDMAP_GetEncEffCntPtr(work->fieldWork);
      ENCEFF_FreeUserWork(cnt_ptr);
      //オーバーレイアンロード
      ENCEFF_UnloadEffOverlay(cnt_ptr);
    }

    GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, -1 );
    (*seq)++;
    break;
  case 3:
    if( GFL_FADE_CheckFade() == FALSE ){
      return( GMEVENT_RES_FINISH );
    }
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  デバッグメニュー 国連データ作成
//======================================================================
#include "debug/debug_make_undata.h"
FS_EXTERN_OVERLAY(debug_make_undata);
static GMEVENT_RESULT debugMenuMakeUNData( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//-------------------------------------
/// デバッグ国連データ作成用ワーク
//=====================================
typedef struct
{
  HEAPID HeapID;
  GAMESYS_WORK    *gsys;
  GMEVENT         *Event;
  FIELDMAP_WORK *FieldWork;
  PROCPARAM_DEBUG_MAKE_UNDATA p_work;
} DEBUG_MAKE_UNDATA_EVENT_WORK;

//----------------------------------------------------------------------------
/**
 *  @brief  国連データ作成
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ワーク
 *
 *  @return TRUEイベント継続  FALSE終了
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugMakeUNData( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *gsys  = p_wk->gmSys;
  GMEVENT       *event    = p_wk->gmEvent;
  FIELDMAP_WORK *fieldWork  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_MAKE_UNDATA_EVENT_WORK *evt_work;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gsys));

  //イベントチェンジ
  GMEVENT_Change( event, debugMenuMakeUNData, sizeof(DEBUG_MAKE_UNDATA_EVENT_WORK) );
  evt_work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( evt_work, sizeof(DEBUG_MAKE_UNDATA_EVENT_WORK) );

  //ワーク設定
  evt_work->gsys = gsys;
  evt_work->Event = event;
  evt_work->FieldWork = fieldWork;
  evt_work->HeapID = heapID;
  evt_work->p_work.wh = SaveData_GetWifiHistory(pSave);

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  デバッグ国連データ作成イベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuMakeUNData( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_END,
  };

  DEBUG_MAKE_UNDATA_EVENT_WORK *evt_work = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( evt_work->Event, EVENT_FieldSubProc( evt_work->gsys, evt_work->FieldWork,
        FS_OVERLAY_ID(debug_make_undata), &ProcData_DebugMakeUNData, &evt_work->p_work ) );
    *p_seq  = SEQ_PROC_END;
    break;
  case SEQ_PROC_END:
    return GMEVENT_RES_FINISH;
    break;
  }

  return GMEVENT_RES_CONTINUE ;
}

//======================================================================
//  デバッグメニュー　バトルサブウェイ
//======================================================================
//--------------------------------------------------------------
/// DEBUG_BSUBWAY_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
}DEBUG_BSUBWAY_EVENT_WORK;

static const FLDMENUFUNC_HEADER DATA_DebugMenuList_BSubway =
{
  1,    //リスト項目数
  6,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

enum
{
  DEBUG_BSWAY_DEMO_HOME = 0xfff0,
  DEBUG_BSWAY_DEMO_HOME_LAST,

  DEBUG_BSWAY_SCDATA_VIEW,
  DEBUG_BSWAY_WIFI_GAMEDATA_DOWNLOAD,
  DEBUG_BSWAY_WIFI_RIREKI_DOWNLOAD,
  DEBUG_BSWAY_WIFI_UPLOAD,
};

static const FLDMENUFUNC_LIST DATA_BSubwayMenuList[] =
{
  { DEBUG_FIELD_BSW_01, (void*)ZONE_ID_C04R0102 },
  { DEBUG_FIELD_BSW_02, (void*)ZONE_ID_C04R0104 },
  { DEBUG_FIELD_BSW_03, (void*)ZONE_ID_C04R0106 },
  { DEBUG_FIELD_BSW_04, (void*)ZONE_ID_C04R0108 },
  { DEBUG_FIELD_BSW_05, (void*)ZONE_ID_C04R0103 },
  { DEBUG_FIELD_BSW_06, (void*)ZONE_ID_C04R0105 },
  { DEBUG_FIELD_BSW_07, (void*)ZONE_ID_C04R0107 },
  { DEBUG_FIELD_BSW_08, (void*)ZONE_ID_C04R0110 },
  { DEBUG_FIELD_BSW_09, (void*)ZONE_ID_C04R0111 },
  { DEBUG_FIELD_BSW_09, (void*)DEBUG_BSWAY_DEMO_HOME },
  { DEBUG_FIELD_BSW_09, (void*)DEBUG_BSWAY_DEMO_HOME_LAST },
  { DEBUG_FIELD_BSW_10, (void*)DEBUG_BSWAY_SCDATA_VIEW },
  { DEBUG_FIELD_WIFI_BSW_01, (void*)DEBUG_BSWAY_WIFI_GAMEDATA_DOWNLOAD },
  { DEBUG_FIELD_WIFI_BSW_02, (void*)DEBUG_BSWAY_WIFI_RIREKI_DOWNLOAD },
  { DEBUG_FIELD_WIFI_BSW_03, (void*)DEBUG_BSWAY_WIFI_UPLOAD },
};

#define DEBUG_BSUBWAY_LIST_MAX ( NELEMS(DATA_BSubwayMenuList) )

static const DEBUG_MENU_INITIALIZER DebugBSubwayMenuData = {
  NARC_message_d_field_dat,
  DEBUG_BSUBWAY_LIST_MAX,
  DATA_BSubwayMenuList,
  &DATA_DebugMenuList_BSubway,
  1, 1, 15, 12,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * イベント：バトルサブウェイデバッグメニュー
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuBSubwayEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_BSUBWAY_EVENT_WORK  *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init(
        work->fieldWork, work->heapID,  &DebugBSubwayMenuData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      GMEVENT* next_event;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }

      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){  //キャンセル
        return( GMEVENT_RES_FINISH );
      }

      switch( ret ){
      case DEBUG_BSWAY_DEMO_HOME:
        ret = ZONE_ID_C04R0111;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, ret );
        break;
      case DEBUG_BSWAY_DEMO_HOME_LAST:
        ret = ZONE_ID_C04R0111;
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, ret );
        break;
      default:
        next_event = DEBUG_EVENT_QuickChangeMapDefaultPos(
          work->gmSys, work->fieldWork, ret );
        break;

      // 歴代リーダー表示
      case DEBUG_BSWAY_SCDATA_VIEW:  
        next_event = BSUBWAY_EVENT_CallLeaderBoard( work->gmSys );
        break;


      // WiFi系イベントの起動
      case DEBUG_BSWAY_WIFI_GAMEDATA_DOWNLOAD:  // ゲームデータダウンロード
        next_event = WIFI_BSUBWAY_EVENT_Start( work->gmSys,
            WIFI_BSUBWAY_MODE_GAMEDATA_DOWNLOAD );
        break;

      case DEBUG_BSWAY_WIFI_RIREKI_DOWNLOAD:  // 履歴ダウンロード
        next_event = WIFI_BSUBWAY_EVENT_Start( work->gmSys,
            WIFI_BSUBWAY_MODE_SUCCESSDATA_DOWNLOAD );
        break;

      case DEBUG_BSWAY_WIFI_UPLOAD: // ゲームデータアップロード
        next_event = WIFI_BSUBWAY_EVENT_Start( work->gmSys,
            WIFI_BSUBWAY_MODE_SCORE_UPLOAD );
        break;
      }

      GMEVENT_CallEvent( event, next_event );
      (*seq)++;
    }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_BSubway( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_WEATERLIST_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuBSubwayEvent, sizeof(DEBUG_BSUBWAY_EVENT_WORK) );
  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_BSUBWAY_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//======================================================================
//  デバッグメニュー　Gパワー
//======================================================================
#include "gamesystem/g_power.h"
#include "msg/msg_d_matsu.h"
//--------------------------------------------------------------
/// DEBUG_GPOWER_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;

  POWER_CONV_DATA *powerdata;
}DEBUG_GPOWER_EVENT_WORK;


//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuGPowerListEvent(
    GMEVENT *event, int *seq, void *work );

///テストカメラリスト メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_GPowerList =
{
  1,    //リスト項目数
  9,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  16,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

static const DEBUG_MENU_INITIALIZER DebugGPowerMenuListData = {
  NARC_message_power_dat,
  NULL,
  NULL,
  &DATA_DebugMenuList_GPowerList,
  1, 1, 24, 19,
  DEBUG_SetMenuWorkGPower,
  DEBUG_GetGPowerMax,
};

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　天気リスト
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_GPowerList( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_GPOWER_EVENT_WORK *work;

  GMEVENT_Change( event,
    debugMenuGPowerListEvent, sizeof(DEBUG_GPOWER_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_GPOWER_EVENT_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：Gパワーリスト
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuGPowerListEvent(GMEVENT *event, int *seq, void *wk )
{
  DEBUG_GPOWER_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugGPowerMenuListData );
    work->powerdata = GPOWER_PowerData_LoadAlloc(work->heapID);
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }
      if(ret == FLDMENUFUNC_CANCEL){
        (*seq)++;
        break;
      }

      if(ret < GPOWER_ID_MAX){
        GPOWER_Set_OccurID(ret, work->powerdata, FALSE);
      }
      else{
        GPOWER_Clear_AllPower();
      }
    }
    break;
  case 2:
    FLDMENUFUNC_DeleteMenu( work->menuFunc );
    GPOWER_PowerData_Unload(work->powerdata);
    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * Gパワー用BMP_MENULIST_DATAセット
 * @param list  セット先BMP_MENULIST_DATA
 * @param heapID  文字列バッファ確保用HEAPID
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkGPower(GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list,
  HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  int id;
  STRBUF *strBuf = GFL_STR_CreateBuffer( 64, heapID );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMSGBG * msgBG = FIELDMAP_GetFldMsgBG( fieldmap );
  GFL_MSGDATA * pMsgDataDMatsu = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_d_matsu_dat );

  for( id = 0; id < GPOWER_ID_MAX; id++ ){
    GFL_MSG_GetString( msgData,  id, strBuf );
    FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
  }
  GFL_MSG_GetString(pMsgDataDMatsu, DM_MSG_POWER_RESET, strBuf);
  FLDMENUFUNC_AddStringListData( list, strBuf, GPOWER_ID_MAX, heapID );

  GFL_MSG_Delete( pMsgDataDMatsu );
  GFL_HEAP_FreeMemory( strBuf );
}

//--------------------------------------------------------------
/**
 * @brief Gパワー用リスト最大値取得関数
 * @param fieldmap
 * @return  マップ最大数
 */
//--------------------------------------------------------------
static u16 DEBUG_GetGPowerMax( GAMESYS_WORK* gsys, void* cb_work )
{
  return GPOWER_ID_MAX + 1; // +1 = 「全てリセット」
}

//======================================================================
//  デバッグメニュー　フィールドわざ
//======================================================================
//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuFieldSkillListEvent(GMEVENT *event, int *seq, void *wk );

static const FLDMENUFUNC_LIST DATA_SubFieldSkillList[] =
{
  { DEBUG_FIELD_STR31, debugMenuCallProc_Naminori },              //波乗り
  { DEBUG_FIELD_STR45, debugMenuCallProc_Kairiki },               //怪力
  { DEBUG_FIELD_ANANUKENOHIMO, debugMenuCallProc_Ananukenohimo }, //穴ヌケの紐
  { DEBUG_FIELD_ANAWOHORU, debugMenuCallProc_Anawohoru },         //穴を掘る
  { DEBUG_FIELD_TELEPORT, debugMenuCallProc_Teleport },           //テレポート
  { DEBUG_FIELD_FSKILL_01, debugMenuCallProc_Diving },            //ダイビング
};

static const DEBUG_MENU_INITIALIZER DebugSubFieldSkillListSelectData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_SubFieldSkillList),
  DATA_SubFieldSkillList,
  &DATA_DebugMenuList_Default, //流用
  1, 1, 16, 17,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　フィールド技リスト
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_FieldSkillList( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  DEBUG_MENU_EVENT_WORK   temp  = *wk;
  DEBUG_MENU_EVENT_WORK   *work;

  GMEVENT_Change( event,
    debugMenuFieldSkillListEvent, sizeof(DEBUG_MENU_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_MENU_EVENT_WORK) );

  *work  = temp;
  work->call_proc = NULL;

  work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugSubFieldSkillListSelectData );

  return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：フィールド技リスト
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuFieldSkillListEvent(GMEVENT *event, int *seq, void *wk )
{
  DEBUG_MENU_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }else if( ret == FLDMENUFUNC_CANCEL ){  //キャンセル
        (*seq)++;
      }else if( ret != FLDMENUFUNC_CANCEL ){  //決定
        work->call_proc = (DEBUG_MENU_CALLPROC)ret;
        (*seq)++;
      }
    }
    break;

  case 2:
    {
      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( work->call_proc != NULL ){
        if( work->call_proc(work) == TRUE ){
          return( GMEVENT_RES_CONTINUE );
        }
      }

      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  デバッグメニュー　ふしぎなおくりもの作成
//======================================================================
//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
#define MYSTERY_DLDATA_DEBUG_PRINT
static const FLDMENUFUNC_LIST DATA_SubMysteryCardMakeList[] =
{
  { DEBUG_FIELD_MYSTERY_01, debugMenuCallProc_MakeMysteryCardPoke },              //ポケモン作成
  { DEBUG_FIELD_MYSTERY_02, debugMenuCallProc_MakeMysteryCardItem },               //道具作成
  { DEBUG_FIELD_MYSTERY_03, debugMenuCallProc_MakeMysteryCardGPower },              //Gパワー作成
};

static const DEBUG_MENU_INITIALIZER DebugSubMysteryCardMakeData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_SubMysteryCardMakeList),
  DATA_SubMysteryCardMakeList,
  &DATA_DebugMenuList_Default, //流用
  1, 1, 16, 17,
  NULL,
  NULL
};


static BOOL debugMenuCallProc_MakeMysteryCardList( DEBUG_MENU_EVENT_WORK *wk )
{
  GMEVENT *event = wk->gmEvent;
  DEBUG_MENU_EVENT_WORK   temp  = *wk;
  DEBUG_MENU_EVENT_WORK   *work;

  //イベント流用
  GMEVENT_Change( event,
    debugMenuFieldSkillListEvent, sizeof(DEBUG_MENU_EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_MENU_EVENT_WORK) );

  *work  = temp;
  work->call_proc = NULL;

  work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugSubMysteryCardMakeData );

  return( TRUE );
}

static BOOL debugMenuCallProc_MakeMysteryCardPoke( DEBUG_MENU_EVENT_WORK *p_wk )
{
  DOWNLOAD_GIFT_DATA  dl_data;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(p_wk->gdata);
  MYSTERY_DATA *p_mystery_sv  = SaveData_GetMysteryData( pSave);
  GIFT_PACK_DATA  data;
  int i;

  GFL_STD_MemClear( &dl_data, sizeof(DOWNLOAD_GIFT_DATA) );

  for( i = 1; i < 2048; i++ )
  {
    if( !MYSTERYDATA_IsEventRecvFlag(p_mystery_sv, i) )
    {
      DEBUG_MYSTERY_SetGiftPokeData( &data );
      DEBUG_MYSTERY_SetGiftCommonData( &data, i, FALSE ); 
      MYSTERYDATA_SetCardData( p_mystery_sv, &data );

      DEBUG_MYSTERY_SetDownLoadData( &dl_data, 0xFFFFFFFF, LANG_JAPAN );
      dl_data.data  = data;
#ifdef MYSTERY_DLDATA_DEBUG_PRINT
      {
        int j;
        const u8 * cp_data  = (const u8*)&dl_data;
        for( j = 0; j < sizeof(DOWNLOAD_GIFT_DATA); j++ )
        { 
          OS_TFPrintf( 3, "0x%x ", cp_data[j] );
          if( j % 0x10 == 0xF )
          { 
            OS_TFPrintf( 3, "\n" );
          }
        }
      }
#endif
      OS_TPrintf( "ふしぎなカードをセットしました イベントID=[%d]\n", i );
      break;
    }
  }


  return FALSE;
}
static BOOL debugMenuCallProc_MakeMysteryCardItem( DEBUG_MENU_EVENT_WORK *p_wk )
{
  DOWNLOAD_GIFT_DATA  dl_data;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(p_wk->gdata);
  MYSTERY_DATA *p_mystery_sv  = SaveData_GetMysteryData( pSave);
  GIFT_PACK_DATA  data;
  int i;

  GFL_STD_MemClear( &dl_data, sizeof(DOWNLOAD_GIFT_DATA) );
  for( i = 1; i < 2048; i++ )
  {
    if( !MYSTERYDATA_IsEventRecvFlag(p_mystery_sv, i) )
    {
      DEBUG_MYSTERY_SetGiftItemData( &data );
      DEBUG_MYSTERY_SetGiftCommonData( &data, i, FALSE ); 
      MYSTERYDATA_SetCardData( p_mystery_sv, &data );

      DEBUG_MYSTERY_SetDownLoadData( &dl_data, 0xFFFFFFFF, LANG_JAPAN );
      dl_data.data  = data;
#ifdef MYSTERY_DLDATA_DEBUG_PRINT
      {
        int j;
        const u8 * cp_data  = (const u8*)&dl_data;
        for( j = 0; j < sizeof(DOWNLOAD_GIFT_DATA); j++ )
        { 
          OS_TFPrintf( 3, "0x%x ", cp_data[j] );
          if( j % 0x10 == 0xF )
          { 
            OS_TFPrintf( 3, "\n" );
          }
        }
      }
#endif
      OS_TPrintf( "ふしぎなカードをセットしました イベントID=[%d]\n", i );
      break;
    }
  }

  return FALSE;
}
static BOOL debugMenuCallProc_MakeMysteryCardGPower( DEBUG_MENU_EVENT_WORK *p_wk )
{
  DOWNLOAD_GIFT_DATA  dl_data;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(p_wk->gdata);
  MYSTERY_DATA *p_mystery_sv  = SaveData_GetMysteryData( pSave);
  GIFT_PACK_DATA  data;
  int i;

  GFL_STD_MemClear( &dl_data, sizeof(DOWNLOAD_GIFT_DATA) );
  for( i = 1; i < 2048; i++ )
  {
    if( !MYSTERYDATA_IsEventRecvFlag(p_mystery_sv, i) )
    {
      DEBUG_MYSTERY_SetGiftGPowerData( &data );
      DEBUG_MYSTERY_SetGiftCommonData( &data, i, FALSE ); 
      MYSTERYDATA_SetCardData( p_mystery_sv, &data );

      DEBUG_MYSTERY_SetDownLoadData( &dl_data, 0xFFFFFFFF, LANG_JAPAN );
      dl_data.data  = data;
#ifdef MYSTERY_DLDATA_DEBUG_PRINT
      {
        int j;
        const u8 * cp_data  = (const u8*)&dl_data;
        for( j = 0; j < sizeof(DOWNLOAD_GIFT_DATA); j++ )
        { 
          OS_TFPrintf( 3, "0x%x ", cp_data[j] );
          if( j % 0x10 == 0xF )
          { 
            OS_TFPrintf( 3, "\n" );
          }
        }
      }
#endif
      OS_TPrintf( "ふしぎなカードをセットしました イベントID=[%d]\n", i );
      break;
    }
  }

  return FALSE;
}

//======================================================================
//
//    マップ全チェック
//
//======================================================================

static GMEVENT * EVENT_DEBUG_AllMapCheck( GAMESYS_WORK * gsys, BOOL up_to_flag );
//--------------------------------------------------------------
/// デバッグメニュー：全マップチェック
//--------------------------------------------------------------
static BOOL debugMenuCallProc_AllMapCheck( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT * new_event;
  new_event = EVENT_DEBUG_AllMapCheck( p_wk->gmSys, FALSE );
  GMEVENT_ChangeEvent( p_wk->gmEvent, new_event );
  return TRUE;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  int check_zone_id;
  BOOL up_to_flag;
  int wait;
  GAMESYS_WORK * gsys;
}ALL_MAP_CHECK_WORK;

//--------------------------------------------------------------
//--------------------------------------------------------------
static const u16 ng_zone_table[] = {
  ZONE_ID_UNION,
  ZONE_ID_PALACETEST,
  ZONE_ID_CLOSSEUM,
  ZONE_ID_CLOSSEUM02,
  ZONE_ID_BCWFTEST,
  ZONE_ID_D10,
  ZONE_ID_PLD10,
};

//#include "arc/fieldmap/map_matrix.naix"
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL checkNGZoneID( zone_id )
{
  int i;
#if 0
    if ( ZONEDATA_GetMatrixID(zone_id) == NARC_map_matrix_palacefield_mat_bin )
    {
      continue;
    }
#endif
  for ( i = 0; i < NELEMS(ng_zone_table); i++ )
  {
    if (ng_zone_table[i] == zone_id )
    {
      return TRUE;
    }
  }
  return FALSE;
}
static int getNextZoneID( ALL_MAP_CHECK_WORK* amcw )
{
  int zone_id = amcw->check_zone_id;
  while (TRUE)
  {
    if ( amcw->up_to_flag )
    {
      zone_id ++;
      if ( zone_id >= ZONE_ID_MAX ) break;
    } else {
      zone_id --;
      if ( zone_id < 0 ) break;
    }
    if ( checkNGZoneID( zone_id ) == TRUE ) continue;
    break;
  }
  amcw->check_zone_id = zone_id;
  return zone_id;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT_RESULT allMapCheckEvent( GMEVENT * event, int *seq, void * wk )
{
  ALL_MAP_CHECK_WORK * amcw = wk;
  FIELDMAP_WORK * fieldmap;
  u16 zone_id;

  enum {
    SEQ_INIT,
    SEQ_SEEK_ID,
    SEQ_WAIT
  };
  switch ( *seq )
  {
  case SEQ_INIT:
    if ( amcw->up_to_flag )
    {
      amcw->check_zone_id = -1;
    } else {
      amcw->check_zone_id = ZONE_ID_MAX;
    }
    *seq = SEQ_SEEK_ID;
    /* FALL THROUGH */

  case SEQ_SEEK_ID:
    zone_id = getNextZoneID( amcw );
    if ( zone_id >= ZONE_ID_MAX || zone_id < 0 )
    {
      return GMEVENT_RES_FINISH;
    }
    {
      char buf[ZONEDATA_NAME_LENGTH];
      ZONEDATA_DEBUG_GetZoneName(buf, zone_id);
      OS_TPrintf( "\t[[ALL MAP CHECKING... :%s]]!!\n", buf );
    }
    fieldmap = GAMESYSTEM_GetFieldMapWork( amcw->gsys );
    GMEVENT_CallEvent( event,
        DEBUG_EVENT_ChangeMapDefaultPos( amcw->gsys, fieldmap, zone_id ) );
    *seq = SEQ_WAIT;
    break;

  case SEQ_WAIT:
    amcw->wait ++;
    if ( amcw->wait > 120 )
    {
      amcw->wait = 0;
      *seq = SEQ_SEEK_ID;
    }
    break;
  }

  return GMEVENT_RES_CONTINUE;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * EVENT_DEBUG_AllMapCheck( GAMESYS_WORK * gsys, BOOL up_to_flag )
{
  GMEVENT * new_event;
  ALL_MAP_CHECK_WORK * amcw;
  new_event = GMEVENT_Create( gsys, NULL, allMapCheckEvent, sizeof(ALL_MAP_CHECK_WORK) );
  amcw = GMEVENT_GetEventWork( new_event );
  amcw->up_to_flag = up_to_flag;
  amcw->gsys = gsys;
  return new_event;
}

//======================================================================
//  デバッグメニュー ゾーンジャンプ
//======================================================================
#include "debug/debug_zone_jump.h"
FS_EXTERN_OVERLAY(debug_zone_jump);
static GMEVENT_RESULT debugMenuZoneJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//-------------------------------------
/// デバッグ国連データ作成用ワーク
//=====================================
typedef struct
{
  HEAPID HeapID;
  GAMESYS_WORK    *gsys;
  GMEVENT         *Event;
  FIELDMAP_WORK *FieldWork;
  PROCPARAM_DEBUG_ZONE_JUMP p_work;
} DEBUG_ZONE_JUMP_EVENT_WORK;

//----------------------------------------------------------------------------
/**
 *  @brief  ゾーンジャンプ
 *
 *  @param  DEBUG_MENU_EVENT_WORK *wk   ワーク
 *
 *  @return TRUEイベント継続  FALSE終了
 */
//-----------------------------------------------------------------------------
static BOOL debugMenuCallProc_DebugZoneJump( DEBUG_MENU_EVENT_WORK *p_wk )
{
  GAMESYS_WORK  *gsys  = p_wk->gmSys;
  GMEVENT       *event    = p_wk->gmEvent;
  FIELDMAP_WORK *fieldWork  = p_wk->fieldWork;
  HEAPID heapID = HEAPID_PROC;
  DEBUG_ZONE_JUMP_EVENT_WORK *evt_work;
  SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gsys));

  //イベントチェンジ
  GMEVENT_Change( event, debugMenuZoneJump, sizeof(DEBUG_ZONE_JUMP_EVENT_WORK) );
  evt_work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( evt_work, sizeof(DEBUG_ZONE_JUMP_EVENT_WORK) );

  //ワーク設定
  evt_work->gsys = gsys;
  evt_work->Event = event;
  evt_work->FieldWork = fieldWork;
  evt_work->HeapID = heapID;

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  デバッグゾーンジャンプイベント
 *
 *  @param  GMEVENT *event  GMEVENT
 *  @param  *seq            シーケンス
 *  @param  *work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuZoneJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_CALL_PROC,
    SEQ_PROC_RESULT,
    SEQ_PROC_END,
  };

  DEBUG_ZONE_JUMP_EVENT_WORK *evt_work = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_CALL_PROC:
    GMEVENT_CallEvent( evt_work->Event, EVENT_FieldSubProc( evt_work->gsys, evt_work->FieldWork,
        FS_OVERLAY_ID(debug_zone_jump), &ProcData_DebugZoneJump, &evt_work->p_work ) );
    *p_seq  = SEQ_PROC_RESULT;
    break;
  case SEQ_PROC_RESULT:
    if ( evt_work->p_work.Ret )
    {
      GMEVENT * mapchange_event;
      OS_Printf("ゾーンID　%d にジャンプ\n", evt_work->p_work.ZoneID);
      mapchange_event = DEBUG_EVENT_QuickChangeMapDefaultPos( evt_work->gsys, evt_work->FieldWork, evt_work->p_work.ZoneID );
      GMEVENT_ChangeEvent( p_event, mapchange_event );
    }
    else *p_seq  = SEQ_PROC_END;
    break;
  case SEQ_PROC_END:
    return GMEVENT_RES_FINISH;
    break;
  }

  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  デバッグメニュー　図鑑
//======================================================================

typedef struct {
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FIELDMAP_WORK *fieldWork;
  FLDMENUFUNC *menuFunc;
  ZUKAN_SAVEDATA * sv;
}DEBUG_ZUKAN_WORK;

///リスト メニューヘッダー
static const FLDMENUFUNC_HEADER ZukanMenuHeader =
{
  1,    //リスト項目数
  8,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

///メニューリスト
static const FLDMENUFUNC_LIST ZukanMenuList[] =
{
  { DEBUG_FIELD_ZUKAN_01, (void*)0 },   // 全国捕獲
  { DEBUG_FIELD_ZUKAN_07, (void*)1 },   // 全国見た
  { DEBUG_FIELD_ZUKAN_10, (void*)6 },   // 地方捕獲
  { DEBUG_FIELD_ZUKAN_11, (void*)7 },   // 地方見た
  { DEBUG_FIELD_ZUKAN_09, (void*)2 },   // ランダム
  { DEBUG_FIELD_ZUKAN_03, (void*)3 },   // フォルム
  { DEBUG_FIELD_ZUKAN_05, (void*)4 },   // 全国フラグ
  { DEBUG_FIELD_ZUKAN_06, (void*)5 },   // バージョンアップ
};

static const DEBUG_MENU_INITIALIZER DebugMenuZukanImitializer = {
  NARC_message_d_field_dat,
  NELEMS(ZukanMenuList),
  ZukanMenuList,
  &ZukanMenuHeader,
  1, 1, 16, 16,
  NULL,
  NULL
};

//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuZukanEvent( GMEVENT *event, int *seq, void *wk );
static void SetZukanDataOne( DEBUG_ZUKAN_WORK * wk, u16 mons, u16 form, u32 mode );


//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　図鑑
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Zukan( DEBUG_MENU_EVENT_WORK *wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_ZUKAN_WORK *work;

  GMEVENT_Change( event, debugMenuZukanEvent, sizeof(DEBUG_ZUKAN_WORK) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_ZUKAN_WORK) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->heapID = heapID;
  work->fieldWork = fieldWork;
  work->sv = GAMEDATA_GetZukanSave( GAMESYSTEM_GetGameData(wk->gmSys) );

  return TRUE;
}

//--------------------------------------------------------------
/**
 * イベント：図鑑
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuZukanEvent( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_ZUKAN_WORK *work = wk;

  switch( *seq ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID, &DebugMenuZukanImitializer );
    (*seq)++;
    break;

  case 1:

    switch( FLDMENUFUNC_ProcMenu(work->menuFunc) ){
    case FLDMENUFUNC_NULL:    // 操作無し
      break;

    case FLDMENUFUNC_CANCEL:  // キャンセル
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case 0:     // 全国捕獲
      {
        u32 i;
        for( i=1; i<=MONSNO_END; i++ ){
          SetZukanDataOne( wk, i, 0, 0 );
        }
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case 1:     // 全国見た
      {
        u32 i;
        for( i=1; i<=MONSNO_END; i++ ){
          SetZukanDataOne( wk, i, 0, 1 );
        }
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case 2:     // ランダム
      {
        u32 rand;
        u32 max;
        u32 i;
        for( i=1; i<=MONSNO_END; i++ ){
          rand = GFL_STD_MtRand( 3 );
          if( rand == 2 ){ continue; }
          max = ZUKANSAVE_GetFormMax( i );
          if( max != 0 ){
            SetZukanDataOne( wk, i, GFL_STD_MtRand(max), rand );
          }else{
            SetZukanDataOne( wk, i, 0, rand );
          }
        }
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case 3:     // フォルム
      {
        u32 max;
        u16 i, j;
        for( i=1; i<=MONSNO_END; i++ ){
          max = ZUKANSAVE_GetFormMax( i );
          if( max != 0 ){
            for( j=0; j<max; j++ ){
              SetZukanDataOne( wk, i, j, 0 );
            }
          }
        }
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case 4:     // 全国フラグ
      ZUKANSAVE_SetZenkokuZukanFlag( work->sv );
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case 5:     // バージョンアップ
      ZUKANSAVE_SetGraphicVersionUpFlag( work->sv );
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case 6:     // 地方捕獲
      {
				u16 * buf;
        u32 i;
				buf = POKE_PERSONAL_GetZenkokuToChihouArray( work->heapID, NULL );
        for( i=1; i<=MONSNO_END; i++ ){
					if( buf[i] != 0 ){
						SetZukanDataOne( wk, i, 0, 0 );
					}
        }
				GFL_HEAP_FreeMemory( buf );
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;

    case 7:     // 地方見た
      {
				u16 * buf;
        u32 i;
				buf = POKE_PERSONAL_GetZenkokuToChihouArray( work->heapID, NULL );
        for( i=1; i<=MONSNO_END; i++ ){
					if( buf[i] != 0 ){
	          SetZukanDataOne( wk, i, 0, 1 );
					}
        }
				GFL_HEAP_FreeMemory( buf );
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      return GMEVENT_RES_FINISH;
    }
    break;
  }

  return GMEVENT_RES_CONTINUE;
}

static void SetZukanDataOne( DEBUG_ZUKAN_WORK * wk, u16 mons, u16 form, u32 mode )
{
  POKEMON_PARAM * pp = PP_Create( mons, 50, 0, wk->heapID );

  if( mode == 0 ){
    ZUKANSAVE_SetPokeGet( wk->sv, pp );
  }else{
    ZUKANSAVE_SetPokeSee( wk->sv, pp );
  }

  GFL_HEAP_FreeMemory( pp );
}


//--------------------------------------------------------------
//イベントポケモン作成
//--------------------------------------------------------------
#include "poke_tool/poke_memo.h"
static GMEVENT_RESULT debugMenuEventpokeCreate( GMEVENT *event, int *seq, void *wk );


static const FLDMENUFUNC_LIST DATA_EventPokeCreate[] =
{
  { DEBUG_FIELD_EVEPOKE01, (void*)0 },
  { DEBUG_FIELD_EVEPOKE02, (void*)1 },
  { DEBUG_FIELD_EVEPOKE03, (void*)2 },
  { DEBUG_FIELD_EVEPOKE04, (void*)3 },
  { DEBUG_FIELD_EVEPOKE05, (void*)4 },
  { DEBUG_FIELD_EVEPOKE06, (void*)5 },
  { DEBUG_FIELD_EVEPOKE07, (void*)6 },
  { DEBUG_FIELD_EVEPOKE08, (void*)7 },
  { DEBUG_FIELD_EVEPOKE09, (void*)8 },
  { DEBUG_FIELD_EVEPOKE10, (void*)9 },
  { DEBUG_FIELD_EVEPOKE11, (void*)10 },
};

static const DEBUG_MENU_INITIALIZER DebugEventPokeCreateMenu = {
  NARC_message_d_field_dat,
  NELEMS(DATA_EventPokeCreate),
  DATA_EventPokeCreate,
  &DATA_DebugMenuList_Default, //流用
  1, 1, 16, 17,
  NULL,
  NULL
};

typedef struct
{
  HEAPID heapId;
  GAMESYS_WORK *gmSys;
  GMEVENT *gmEvent;
  FLDMENUFUNC *menuFunc;
  FIELDMAP_WORK *fieldWork;
}DEBUG_EVENT_POKE_CREATE;

static BOOL debugMenuCallProc_EventPokeCreate( DEBUG_MENU_EVENT_WORK * wk )
{
  GAMESYS_WORK *gsys = wk->gmSys;
  GMEVENT *event = wk->gmEvent;
  HEAPID heapID = wk->heapID;
  FIELDMAP_WORK *fieldWork = wk->fieldWork;
  DEBUG_EVENT_POKE_CREATE *work;

  GMEVENT_Change( event, debugMenuEventpokeCreate, sizeof(DEBUG_EVENT_POKE_CREATE) );

  work = GMEVENT_GetEventWork( event );
  GFL_STD_MemClear( work, sizeof(DEBUG_EVENT_POKE_CREATE) );

  work->gmSys = gsys;
  work->gmEvent = event;
  work->fieldWork = fieldWork;
  work->heapId = heapID;

  return TRUE;
}
static GMEVENT_RESULT debugMenuEventpokeCreate( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_EVENT_POKE_CREATE *work = wk;

  switch( *seq ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapId, &DebugEventPokeCreateMenu );
    (*seq)++;
    break;

  case 1:
    {
      const u32 ret = FLDMENUFUNC_ProcMenu(work->menuFunc);
      GAMEDATA *gmData = GAMESYSTEM_GetGameData(work->gmSys);
      POKEPARTY *party = GAMEDATA_GetMyPokemon(gmData);
      BOX_MANAGER *boxData = GAMEDATA_GetBoxManager(gmData);
      
      if( ret == FLDMENUFUNC_NULL )
      {
        return GMEVENT_RES_CONTINUE;
      }
      else
      if( ret == FLDMENUFUNC_CANCEL )
      {
        FLDMENUFUNC_DeleteMenu( work->menuFunc );
        return GMEVENT_RES_FINISH;
      }
      else
      {
        POKEMON_PARAM *pp;
        //ポケモン作成
        switch( ret )
        {
        case 0: //10えいがセレビィ
          pp = PP_Create( MONSNO_SEREBHI , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_get_place , POKE_MEMO_PLACE_SEREBIXI_BEFORE );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 1: //10えいがエンテイ
          pp = PP_Create( MONSNO_ENTEI , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_get_place , POKE_MEMO_PLACE_ENRAISUI_BEFORE );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 2: //10えいがライコウ
          pp = PP_Create( MONSNO_RAIKOU , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_get_place , POKE_MEMO_PLACE_ENRAISUI_BEFORE );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 3: //10えいがスイクン
          pp = PP_Create( MONSNO_SUIKUN , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_get_place , POKE_MEMO_PLACE_ENRAISUI_BEFORE );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 4: //10えいがセレビィ(後
          pp = PP_Create( MONSNO_SEREBHI , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_get_place , POKE_MEMO_PLACE_SEREBIXI_AFTER );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 5: //10えいがエンテイ(後
          pp = PP_Create( MONSNO_ENTEI , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_get_place , POKE_MEMO_PLACE_ENRAISUI_AFTER );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 6: //10えいがライコウ(後
          pp = PP_Create( MONSNO_RAIKOU , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_get_place , POKE_MEMO_PLACE_ENRAISUI_AFTER );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 7: //10えいがスイクン(後
          pp = PP_Create( MONSNO_SUIKUN , 50 , PTL_SETUP_RND_RARE , work->heapId );
          PP_Put( pp , ID_PARA_get_place , POKE_MEMO_PLACE_ENRAISUI_AFTER );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 8: //メロディア
          pp = PP_Create( MONSNO_MERODHIA , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 9: //ダルタニス
          pp = PP_Create( MONSNO_DARUTANISU , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        case 10: //はいふシェイミ
          pp = PP_Create( MONSNO_SHEIMI , 50 , PTL_SETUP_RND_AUTO , work->heapId );
          PP_Put( pp , ID_PARA_event_get_flag , TRUE );
          break;
        }
        {
          u16 oyaName[] = {L'イ',L'ベ',L'ン',L'ト',L'ぽ',L'け',0xFFFF};
          PP_Put( pp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
          PP_Put( pp , ID_PARA_oyasex , PTL_SEX_MALE );
        }
        //手持ちに空きがあれば入れる
        if( PokeParty_GetPokeCount( party ) < 6 )
        {
          PokeParty_Add( party , pp );
        }
        else
        {
          BOXDAT_PutPokemon( boxData , PP_GetPPPPointer( pp ) );
        }
        GFL_HEAP_FreeMemory( pp );
      }
    }
  }

  return GMEVENT_RES_CONTINUE;
}
