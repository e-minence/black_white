//=============================================================================
/**
 *
 *  @file   wifi_earth.c
 *  @brief    ジオネット 地球儀
 *  @author   watanabe さん (DP)
 *  @author   takahasi tomoya さん（PL）
 *  @author   hosaka genya (GS)
 *  @data   2009.02.25
 *
 *  @note 金銀で追加したタッチはAボタン処理はなくしました。
 */
//=============================================================================
#include <wchar.h>
#include "gflib.h"
#include "calctool.h"
#include "system/gfl_use.h"
#include "arc_def.h"

#include "message.naix"
#include "font/font.naix"

#include "system/main.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "system/wipe.h"

#include "print/printsys.h"
#include "print/str_tool.h"
#include "print/wordset.h"
#include "app/app_keycursor.h"
#include "app/app_taskmenu.h"
#include "gamesystem/game_beacon.h"

#include "gamesystem/game_data.h"
#include "gamesystem\msgspeed.h"

#include "savedata/mystatus.h"
#include "savedata/wifihistory.h"

#include "sound/pm_sndsys.h"

#include "net_app/wifi_country.h"
#include "wifi_earth_place.naix"
#include "wifi_earth.naix"
#include "msg/msg_wifi_earth_guide.h"
#include "msg/msg_wifi_place_msg_world.h"

#include "wifi_earth_snd.h"

typedef struct Vec2DS32_tag{
  s32 x;
  s32 y;
}Vec2DS32;

#define MAKING

BOOL  WIFI_LocalAreaExistCheck(int nationID);
//============================================================================================
//  定数定義
//============================================================================================
#ifdef PM_DEBUG
//#define WIFI_ERATH_DEBUG
//#define WIFI_ERATH_DEBUG_ALL_DRAW
//#define DEBUG_RAPID_PAD ///<ポイントの上に重なってRボタンを押すとprintQueをあふれされる

#endif

//地点登録最大数
#define WORLD_PLACE_NUM_MAX (0x400) //だいたい１０００国くらいとる

//メッセージバッファサイズ
#define EARTH_STRBUF_SIZE (0x400)
#define EARTH_NAME_SIZE   (64)

//ＢＧ透明パレット
#define EARTH_NULL_PALETTE    (0x4753)

//ＢＧコントロール定数
#define EARTH_TEXT_PLANE    (GFL_BG_FRAME2_S)
#define EARTH_ICON_PLANE    (GFL_BG_FRAME2_M)
#define EARTH_BACK_S_PLANE    (GFL_BG_FRAME3_S)
#define EARTH_BACK_M_PLANE    (GFL_BG_FRAME3_M)
#define PALSIZE         (2*16)
#define DOTSIZE         (8)

// パレット転送位置
#define EARTH_BACK_PAL      (0) //４本分
#define EARTH_SYSFONT_PAL   (4)
#define EARTH_ICONWIN_PAL   (5)
#define EARTH_TALKWIN_PAL   (6)
#define EARTH_MENUWIN_PAL   (7)
#define EARTH_TOUCHFONT_PAL   (8)
#define EARTH_TASKMENU_PAL  (9) // みる・やめるメニュー用パレット

// カラー指定
#define FBMP_COL_BLACK    (1)
#define FBMP_COL_BLK_SDW  (2)
#define FBMP_COL_WHITE    (15)

// 会話ウィンドウキャラ（window.h参照）
#define EARTH_TALKWINCHR_SIZ  (TALK_WIN_CGX_SIZ)
//#define EARTH_TALKWINCHR_NUM  (512-EARTH_TALKWINCHR_SIZ)
#define EARTH_TALKWINCHR_NUM  (4)

// メニューウィンドウキャラ（window.h参照）
#define EARTH_MENUWINCHR_SIZ  (MENU_WIN_CGX_SIZ)
//#define EARTH_MENUWINCHR_NUM  (EARTH_TALKWINCHR_NUM - EARTH_MENUWINCHR_SIZ)
#define EARTH_MENUWINCHR_NUM  (EARTH_TALKWINCHR_NUM + EARTH_TALKWINCHR_SIZ)

// メッセージウィンドウ（fldbmp.h参照、ＣＧＸ以外フィールドと同じ：コメントは数値）
#define EARTH_MSG_WIN_PX    (2)
#define EARTH_MSG_WIN_PY    (19)
#define EARTH_MSG_WIN_SX    (28)//(27)
#define EARTH_MSG_WIN_SY    (4)
#define EARTH_MSG_WIN_PAL   (EARTH_SYSFONT_PAL)
//#define EARTH_MSG_WIN_CGX   (EARTH_MENUWINCHR_NUM-(EARTH_MSG_WIN_SX*EARTH_MSG_WIN_SY))
#define EARTH_MSG_WIN_CGX   (512-(EARTH_MSG_WIN_SX*EARTH_MSG_WIN_SY))

// はい/いいえウィンドウ（fldbmp.h参照、ＣＧＸ以外フィールドと同じ：コメントは数値）
#define EARTH_YESNO_WIN_PX    (24)
#define EARTH_YESNO_WIN_PY    (13)
#define EARTH_YESNO_WIN_SX    (7)
#define EARTH_YESNO_WIN_SY    (4)
#define EARTH_YESNO_WIN_PAL   (EARTH_SYSFONT_PAL)
#define EARTH_YESNO_WIN_CGX   (EARTH_MSG_WIN_CGX-(EARTH_YESNO_WIN_SX*EARTH_YESNO_WIN_SY))

// メインメニューウィンドウ
#define EARTH_MENU_WIN_PX   (18)//(19)
#define EARTH_MENU_WIN_PY   (11)
#define EARTH_MENU_WIN_SX   (13)//(12)
#define EARTH_MENU_WIN_SY   (6)
#define EARTH_MENU_WIN_PAL    (EARTH_SYSFONT_PAL)
#define EARTH_MENU_WIN_CGX    (EARTH_MSG_WIN_CGX-(EARTH_MENU_WIN_SX*EARTH_MENU_WIN_SY))

// 国リストウィンドウ
#define EARTH_PLACE_WIN_PX    (3)
#define EARTH_PLACE_WIN_PY    (2)
#define EARTH_PLACE_WIN_SX    (26)
#define EARTH_PLACE_WIN_SY    (13)//(14)
#define EARTH_PLACE_WIN_PAL   (EARTH_SYSFONT_PAL)
#define EARTH_PLACE_WIN_CGX   (EARTH_MSG_WIN_CGX-(EARTH_PLACE_WIN_SX*EARTH_PLACE_WIN_SY))

// 情報ウィンドウ
#define EARTH_INFO_WIN_PX   (2)
#define EARTH_INFO_WIN_PY   (1)
#define EARTH_INFO_WIN_SX   (27)
#define EARTH_INFO_WIN_SY   (6)
#define EARTH_INFO_WIN_PAL    (EARTH_SYSFONT_PAL)
#define EARTH_INFO_WIN_CGX    (EARTH_YESNO_WIN_CGX-(EARTH_INFO_WIN_SX*EARTH_INFO_WIN_SY))

// アイコンウィンドウ
#define EARTH_ICON_WIN_PX   (25)
#define EARTH_ICON_WIN_PY   (21)
#define EARTH_ICON_WIN_SX   (6)
#define EARTH_ICON_WIN_SY   (2)
#define EARTH_ICON_WIN_PAL    (EARTH_TOUCHFONT_PAL)
//#define EARTH_ICON_WIN_CGX    (EARTH_MENUWINCHR_NUM-(EARTH_ICON_WIN_SX*EARTH_ICON_WIN_SY))
#define EARTH_ICON_WIN_CGX    (512-(EARTH_ICON_WIN_SX*EARTH_ICON_WIN_SY))

// 「みる」ウィンドウ
#define EARTH_LOOK_WIN_PX   (1)
#define EARTH_LOOK_WIN_PY   (21)
#define EARTH_LOOK_WIN_SX   (6)
#define EARTH_LOOK_WIN_SY   (2)
#define EARTH_LOOK_WIN_PAL    (EARTH_TOUCHFONT_PAL)
#define EARTH_LOOK_WIN_CGX    (EARTH_ICON_WIN_CGX - EARTH_LOOK_WIN_SX * EARTH_LOOK_WIN_SY )

//地球モデル初期化定義
#define INIT_EARTH_TRANS_XVAL (0)
#define INIT_EARTH_TRANS_YVAL (0)
#define INIT_EARTH_TRANS_ZVAL (0)
#define INIT_EARTH_SCALE_XVAL (FX32_ONE)
#define INIT_EARTH_SCALE_YVAL (FX32_ONE)
#define INIT_EARTH_SCALE_ZVAL (FX32_ONE)
#define INIT_EARTH_ROTATE_XVAL  (0x1980)  //東京をデフォルト位置にする
#define INIT_EARTH_ROTATE_YVAL  (0xcfe0)  //東京をデフォルト位置にする
#define INIT_EARTH_ROTATE_ZVAL  (0)

//カメラ初期化定義
#define INIT_CAMERA_TARGET_XVAL (0)
#define INIT_CAMERA_TARGET_YVAL (0)
#define INIT_CAMERA_TARGET_ZVAL (0)
#define INIT_CAMERA_POS_XVAL  (0)
#define INIT_CAMERA_POS_YVAL  (0)
#define INIT_CAMERA_POS_ZVAL  (0x128000)
#define INIT_CAMERA_DISTANCE_NEAR (0x050000)
#define INIT_CAMERA_DISTANCE_FAR  (0x128000)
#define INIT_CAMERA_PERSPWAY  (0x05c1)
#define INIT_CAMERA_CLIP_NEAR (0)
#define INIT_CAMERA_CLIP_FAR  (FX32_ONE*100)
#define CAMERA_ANGLE_MIN    (-0x4000+0x200)
#define CAMERA_ANGLE_MAX    (0x4000-0x200)

//ライト初期化定義
#define USE_LIGHT_NUM     (0)
#define LIGHT_VECDEF      (FX32_ONE-1)
#define INIT_LIGHT_ANGLE_XVAL (0)
#define INIT_LIGHT_ANGLE_YVAL (0)
#define INIT_LIGHT_ANGLE_ZVAL (-LIGHT_VECDEF)

//動作処理用定数
#define CAMERA_MOVE_SPEED_NEAR  (0x20)  //カメラ近距離時上下左右移動スピード  
#define CAMERA_MOVE_SPEED_FAR (0x200) //カメラ遠距離時上下左右移動スピード
#define CAMERA_INOUT_SPEED  (0x8000)  //カメラ遠近移動スピード
#define MARK_SCALE_INCDEC (0x80)    //地点マーク遠近移動中拡縮比率
#define EARTH_LIMIT_ROTATE_XMIN (0x1300)  //日本拡大地図の縦回転リミット下限（沖縄）
#define EARTH_LIMIT_ROTATE_XMAX (0x2020)  //日本拡大地図の縦回転リミット上限（北海道稚内）
#define EARTH_LIMIT_ROTATE_YMIN (0xcc80)  //日本拡大地図の横回転リミット下限（北海道釧路）
#define EARTH_LIMIT_ROTATE_YMAX (0xd820)  //日本拡大地図の横回転リミット上限（沖縄）

// タスクメニュー指定用
#define TASKMENU_MIRU     ( 0 )   // 「みる」
#define TASKMENU_YAMERU   ( 1 )   // 「やめる」

//メイン関数シーケンスナンバー定義
enum{
  EARTHDEMO_SEQ_MODELLOAD = 0,          //モデルデータロード
  EARTHDEMO_SEQ_DISPON,             //描画ＯＮ
  EARTHDEMO_SEQ_WELCOME_MSG,            //「ようこそ」
  EARTHDEMO_SEQ_MAINMENU,             //メインメニュー設定
  EARTHDEMO_SEQ_MAINMENU_SELECT,          //メインメニュー選択モード
  EARTHDEMO_SEQ_REGISTRATIONMENU,         //登録メニュー設定
  EARTHDEMO_SEQ_REGISTRATIONMENU_SELECT,      //登録メニュー選択モード
  EARTHDEMO_SEQ_REGISTRATIONLIST_NATION,      //国別登録リスト設定
  EARTHDEMO_SEQ_REGISTRATIONLIST_NATION2,     // 2010.2.24 add by tetsu
  EARTHDEMO_SEQ_REGISTRATIONLIST_NATION_SELECT, //国別登録リスト選択モード
  EARTHDEMO_SEQ_REGISTRATIONLIST_AREA,      //地域別登録リスト設定
  EARTHDEMO_SEQ_REGISTRATIONLIST_AREA2,     // 2010.2.24 add by tetsu
  EARTHDEMO_SEQ_REGISTRATIONLIST_AREA_SELECT,   //地域別登録リスト選択モード
  EARTHDEMO_SEQ_FINAL_REGISTRATION,       //登録最終確認設定
  EARTHDEMO_SEQ_FINAL_REGISTRATION_YESNO,     //登録最終確認
  EARTHDEMO_SEQ_FINAL_REGISTRATION_SELECT,    //登録最終確認選択モード
  EARTHDEMO_SEQ_EARTH_DISPON,           //３Ｄ地球儀モードＯＮ
  EARTHDEMO_SEQ_MOVE_EARTH,           //メイン動作
  EARTHDEMO_SEQ_MOVE_CAMERA,            //カメラ遠近移動
  EARTHDEMO_SEQ_BUTTON_ANIME,       // ボタンアニメ待ち
  EARTHDEMO_SEQ_END,                //終了処理開始
  EARTHDEMO_SEQ_EXIT,               //終了
};

//メッセージ関数シーケンスナンバー定義
enum{
  MSGSET = 0,
  MSGDRAW,
  MSGWAIT,
};

//メッセージ関数動作指定定義
enum {
  A_BUTTON_WAIT = 0,
  A_BUTTON_NOWAIT,
};

//登録メニューＩＤナンバー定義
enum{
  MENU_WORLD = 0,
  MENU_REGISTRATION,
  MENU_END,
};

//地球回転モード定義
enum {
  JAPAN_MODE = 0,
  GLOBAL_MODE,
};

//カメラ距離フラグ定義
enum {
  CAMERA_FAR = 0,
  CAMERA_NEAR,
};

//３Ｄ描画フラグ定義
enum {
  DRAW3D_DISABLE = 0,
  DRAW3D_ENABLE,
  DRAW3D_BANISH,
};

//地点データ表示カラー定義（モデル選択によって色替え：wifihistory定義に沿う順番にする）
enum {
  MARK_NULL = 0,  //未通信場所
  MARK_BLUE,    //本日はじめて通信した
  MARK_YELLOW,  //通信済
  MARK_RED,   //自分の登録場所
  MARK_GREEN,   //カーソル

  MARK_ARRAY_MAX,
};


//============================================================================================
//  構造体定義
//============================================================================================
//データ設定構造体（国バイナリデータ）
typedef struct EARTH_DATA_NATION_tag
{
  u16 flag;
  s16 x;
  s16 y;
}EARTH_DATA_NATION;

//データ設定構造体（地域バイナリデータ）
typedef struct EARTH_DATA_AREA_tag
{
  s16 x;
  s16 y;
}EARTH_DATA_AREA;

//データ構造体（国地域テーブルデータ）
typedef struct EARTH_AREATABLE_tag
{
  u16   nationID;
  u16   msg_arcID;
  const char* place_dataID;
}EARTH_AREATABLE;

//リストデータ構造体（バイナリデータ→実際に使用するリスト用データに変換）
typedef struct EARTH_DEMO_LIST_DATA_tag
{
  s16 x;
  s16 y;
  MtxFx33 rotate; //相対回転行列をあらかじめ計算して保存
  u16 col;
  u16 nationID;
  u16 areaID;
}EARTH_DEMO_LIST_DATA;

//リストデータ構造体（バイナリデータ→実際に使用するリスト用データに変換）
typedef struct EARTH_DEMO_LIST_tag
{
  u32 listcount;
  EARTH_DEMO_LIST_DATA  place[WORLD_PLACE_NUM_MAX];
}EARTH_DEMO_LIST;

//文字色設定構造体
typedef struct {
  u8 l;
  u8 s;
  u8 b;
}PRINTLSB;

//ワーク構造体
typedef struct EARTH_DEMO_WORK_tag
{
  //ヒープ設定ワーク
  int heapID;

  //セーブデータポインタ
  GAMEDATA*           gamedata;
  SAVE_CONTROL_WORK*  scw;
  MYSTATUS*           mystatus;
  WIFI_HISTORY*       wifi_sv;

  //地点リストワーク
  EARTH_DEMO_LIST placelist;

  //ＢＧシステムポインタ
  GFL_BMPWIN*   listwin;
  GFL_BMPWIN*   msgwin;
  PRINT_UTIL    printUtilMsg;
  GFL_BMPWIN*   iconwin;  ///< やめるボタン
  PRINT_UTIL    printUtilIcon;
  GFL_BMPWIN*   lookwin;  ///< みるボタン @ gs追加
  PRINT_UTIL    printUtilLook;
  GFL_BMPWIN*   infowin;
  PRINT_UTIL    printUtilInfo;
  BMPMENULIST_WORK*   bmplist;
  BMP_MENULIST_DATA*  bmplistdata;
  BMPMENU_WORK*       yesnowin;
  APP_TASKMENU_RES      *TaskMenuRes;     // メニューリソース
  APP_TASKMENU_WIN_WORK *TaskMenuWork[2]; // 「みる」「やめる」メニュー
  STRBUF *menustr[2];

  //文字列コントロール
  GFL_FONT*       fontHandle;
  GFL_MSGDATA*    msg_man;
  PRINT_STREAM*   printStream;
  PRINT_UTIL      printUtil;
  PRINT_QUE*      printQue;
  PRINTLSB        printLSB;
  APP_KEYCURSOR_WORK *printCursor;
  APP_TASKMENU_ITEMWORK menuitem[2];
  GFL_TCBLSYS*    tcbl;
  WORDSET*        wordset;
  STRBUF*         msgstr;
  int             msgseq;

  //モデルロードワーク
  NNSG3dRenderObj     renderobj;      //終了時要ヒープ開放
  NNSG3dResMdl*     resmodel;
  NNSG3dResFileHeader*  resfileheader;

  NNSG3dRenderObj     mark_renderobj[5];  //終了時要ヒープ開放
  NNSG3dResMdl*     mark_resmodel[5];
  NNSG3dResFileHeader*  mark_resfileheader[5];

  //モデル設定ワーク
  VecFx32 trans;
  VecFx32 scale;
  VecFx32 rotate;

  VecFx32 mark_scale;

  //カメラステータス
  u16 camera_status;

  //動作処理ワーク
  int   Draw3Dsw;
  BOOL  fade_end_flag;
  BOOL  Japan_ROM_mode;
  u16   earth_mode;
  u16   rotate_speed;

  //タッチパネル検出保存ワーク
  int tp_result;
  int tp_seq;
  int tp_x;
  int tp_y;
  int tp_lenx;
  int tp_leny;
  int tp_count;

  //地点登録テンポラリ
  int   my_nation;
  int   my_area;
  int   my_nation_tmp;
  int   my_area_tmp;
  BOOL  my_worldopen_flag;

  int   info_mode;

#ifdef WIFI_ERATH_DEBUG
  int debug_work[8];
#endif  

  //入力状態
  int   trg;
  int   cont;
  BOOL  tptrg;
  BOOL  tpcont;
  u32   tpx;
  u32   tpy;

  //3Dインターフェース
  GFL_G3D_CAMERA*     g3Dcamera;
  GFL_G3D_LIGHTSET*   g3Dlightset;

  GFL_G3D_RES*    g3DresEarth;
  GFL_G3D_RES*    g3DresMark[MARK_ARRAY_MAX];
  GFL_G3D_RND*    g3DrndEarth;
  GFL_G3D_RND*    g3DrndMark[MARK_ARRAY_MAX];
  GFL_G3D_OBJ*    g3DobjEarth;
  GFL_G3D_OBJ*    g3DobjMark[MARK_ARRAY_MAX];

}EARTH_DEMO_WORK;

//ビットマップリスト構造体
typedef struct EARTH_BMPLIST_tag
{
  u32 msgID;
  u32 retID;
}EARTH_BMPLIST;

typedef struct {
  u8 frmNum;
  u8 px;
  u8 py;
  u8 sx;
  u8 sy;
  u8 palIdx;
  u8 cgxIdx;
}BMPWIN_DAT;

//============================================================================================
//  グローバル関数定義
//============================================================================================
void  WIFI_RegistratonInit( GAMEDATA* gamedata );
BOOL  WIFI_NationAreaNameGet(int nationID,int areaID,
                STRBUF* nation_str,STRBUF* area_str,int heapID);
BOOL  WIFI_LocalAreaExistCheck(int nationID);

//============================================================================================
//  ローカル関数定義
//============================================================================================
static void Earth_VramBankSet(void);
static void Earth_BGsysSet(HEAPID heapID);
static void Earth_BGsysRelease( void );
static void Earth_BGdataLoad( EARTH_DEMO_WORK * wk, ARCHANDLE* p_handle );
static void Earth_BGdataRelease( EARTH_DEMO_WORK * wk );

static void EarthListLoad( EARTH_DEMO_WORK * wk );
static void EarthListSet( EARTH_DEMO_WORK * wk,u32 index, s16 x,s16 y,u16 nationID,u16 areaID );
static void EarthList_NationAreaListSet( EARTH_DEMO_WORK * wk );
static int  EarthAreaTableGet(int nationID);

static void Earth_TouchPanel( EARTH_DEMO_WORK * wk );
static void Earth_TouchPanelParamGet( EARTH_DEMO_WORK * wk,
  int prevx,int prevy,int* dirx_p,int* lenx_p,int* diry_p,int* leny_p );

static BOOL Earth_StrPrint
( EARTH_DEMO_WORK * wk, PRINT_UTIL *util, GFL_BMPWIN* bmpwin, const STRBUF* strbuf, u8 x, u8 y );
static BOOL Earth_MsgPrint( EARTH_DEMO_WORK * wk,u32 msgID,int button_mode );
static void Earth_BmpListAdd( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                              const BMPMENULIST_HEADER* listheader,const EARTH_BMPLIST* list);
static void Earth_BmpListAddGmmAll( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                                    const BMPMENULIST_HEADER* listheader,u32 listarcID);
static void Earth_BmpListAddGmmEnableList( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                                    const BMPMENULIST_HEADER* listheader,u32 listarcID);

static void Earth_BmpListDel( EARTH_DEMO_WORK* wk );

static void Earth_MyPlaceInfoWinSet( EARTH_DEMO_WORK* wk );
static void Earth_MyPlaceInfoWinSet2( EARTH_DEMO_WORK* wk, int nation, int area );
static void Earth_MyPlaceInfoWinRelease( EARTH_DEMO_WORK* wk );
static void Earth_PosInfoPut( EARTH_DEMO_WORK* wk );

static void Earth_ModelLoad( EARTH_DEMO_WORK * wk, ARCHANDLE* p_handle );
static void Earth_ModelRelease( EARTH_DEMO_WORK * wk );
static void EarthDataInit( EARTH_DEMO_WORK * wk );
static void EarthCameraInit( EARTH_DEMO_WORK * wk );
static void EarthLightInit( EARTH_DEMO_WORK * wk );
static void EarthCameraStart( EARTH_DEMO_WORK * wk );

static BOOL Earth3D_Control( EARTH_DEMO_WORK * wk,int keytrg,int keycont );
static BOOL Earth3D_CameraMoveNearFar( EARTH_DEMO_WORK * wk );
static void Earth3D_Draw( EARTH_DEMO_WORK * wk );
static void EarthVecFx32_to_MtxFx33( MtxFx33* dst, VecFx32* src );
static void EarthVecFx32_to_MtxFx33_place( MtxFx33* dst, VecFx32* src );

static void WIFI_MinusRotateChange( Vec2DS32* p_rot );
static u32 WIFI_EarthGetRotateDist( const Vec2DS32* cp_earth, const Vec2DS32* cp_place );
static void Earth_TaskMenuInit( EARTH_DEMO_WORK *wk );
static void TaskMenuOff( EARTH_DEMO_WORK *wk, int id );
static void TaskMenuPut( EARTH_DEMO_WORK *wk, int id );

static BOOL _menu_anime_check( EARTH_DEMO_WORK *wk );
static void _menu_main( EARTH_DEMO_WORK *wk );
static void _menu_anime_on( EARTH_DEMO_WORK *wk, int id );
static GFL_PROC_RESULT SubSeq_Main( EARTH_DEMO_WORK *wk, int *seq );


#ifdef WIFI_ERATH_DEBUG
static void EarthDebugWinRotateInfoWrite( EARTH_DEMO_WORK * wk );
static void EarthDebugWinNameInfoWrite( EARTH_DEMO_WORK * wk );
static void EarthDebugNationMarkSet( EARTH_DEMO_WORK * wk );
static void EarthDebugAreaMarkSet( EARTH_DEMO_WORK * wk );
#endif  

//============================================================================================
//  外部関数定義
//============================================================================================
//extern void Main_SetNextProc(FSOverlayID ov_id, const PROC_DATA * proc_data);
extern u8 MsgSpeedGet(void);

//============================================================================================
//  ＰＲＯＣ定義データ
//============================================================================================
static GFL_PROC_RESULT Earth_Demo_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT Earth_Demo_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT Earth_Demo_Exit(GFL_PROC * proc, int * seq, void * pwk, void * mywk );

const GFL_PROC_DATA Earth_Demo_proc_data = {
  Earth_Demo_Init,
  Earth_Demo_Main,
  Earth_Demo_Exit,
};

//============================================================================================
//  ＢＧ定義データ
//============================================================================================
static const GFL_BG_BGCNT_HEADER Earth_Demo_BGtxt_header = {
  0,0,          //初期表示ＸＹ設定
  0x800,0,        //スクリーンバッファサイズ(0 = 使用しない)＆オフセット
  GFL_BG_SCRSIZ_256x256,  //スクリーンサイズ
  GX_BG_COLORMODE_16,   //カラーモード
  GX_BG_SCRBASE_0x7000,   //スクリーンベースブロック
  GX_BG_CHARBASE_0x00000, //キャラクタベースブロック 
  GFL_BG_CHRSIZ_128x256,  //キャラクタデータサイズ
  GX_BG_EXTPLTT_01,   //拡張パレット選択(使用しない設定のときは無視される)
  0,            //表示プライオリティー
  0,            //エリアオーバーフラグ
  0,            //
  FALSE         //モザイク設定
};

static const GFL_BG_BGCNT_HEADER Earth_Demo_Back_header = {
  0,0,          //初期表示ＸＹ設定
  0x800,0,        //スクリーンバッファサイズ(0 = 使用しない)＆オフセット
  GFL_BG_SCRSIZ_256x256,  //スクリーンサイズ
  GX_BG_COLORMODE_16,   //カラーモード
  GX_BG_SCRBASE_0x7800,   //スクリーンベースブロック
  GX_BG_CHARBASE_0x04000, //キャラクタベースブロック 
  GFL_BG_CHRSIZ_128x256,  //キャラクタデータサイズ
  GX_BG_EXTPLTT_01,   //拡張パレット選択(使用しない設定のときは無視される)
  3,            //表示プライオリティー
  0,            //エリアオーバーフラグ
  0,            //
  FALSE         //モザイク設定
};

static const BMPWIN_DAT EarthMsgWinData =
{
  EARTH_TEXT_PLANE, //ウインドウ使用フレーム
  EARTH_MSG_WIN_PX,EARTH_MSG_WIN_PY,//ウインドウ領域の左上XY座標（キャラ単位指定）
  EARTH_MSG_WIN_SX,EARTH_MSG_WIN_SY,//ウインドウ領域のXYサイズ（キャラ単位指定）
  EARTH_MSG_WIN_PAL,  //ウインドウ領域のパレットナンバー 
  EARTH_MSG_WIN_CGX,  //ウインドウキャラ領域の開始キャラクタナンバー
};

static const BMPWIN_YESNO_DAT EarthYesNoWinData =
{
  EARTH_TEXT_PLANE, //ウインドウ使用フレーム
  EARTH_YESNO_WIN_PX,EARTH_YESNO_WIN_PY,//ウインドウ領域の左上XY座標（キャラ単位指定）
  //EARTH_YESNO_WIN_SX,EARTH_YESNO_WIN_SY,//ウインドウ領域のXYサイズ（キャラ単位指定）
  EARTH_YESNO_WIN_PAL,  //ウインドウ領域のパレットナンバー 
  EARTH_YESNO_WIN_CGX,  //ウインドウキャラ領域の開始キャラクタナンバー
};

static const BMPWIN_DAT EarthMenuWinData =
{
  EARTH_TEXT_PLANE, //ウインドウ使用フレーム
  EARTH_MENU_WIN_PX,EARTH_MENU_WIN_PY,//ウインドウ領域の左上XY座標（キャラ単位指定）
  EARTH_MENU_WIN_SX,EARTH_MENU_WIN_SY,//ウインドウ領域のXYサイズ（キャラ単位指定）
  EARTH_MENU_WIN_PAL, //ウインドウ領域のパレットナンバー 
  EARTH_MENU_WIN_CGX, //ウインドウキャラ領域の開始キャラクタナンバー
};

static const BMPWIN_DAT EarthPlaceListWinData =
{
  EARTH_TEXT_PLANE, //ウインドウ使用フレーム
  EARTH_PLACE_WIN_PX,EARTH_PLACE_WIN_PY,//ウインドウ領域の左上XY座標（キャラ単位指定）
  EARTH_PLACE_WIN_SX,EARTH_PLACE_WIN_SY,//ウインドウ領域のXYサイズ（キャラ単位指定）
  EARTH_PLACE_WIN_PAL,  //ウインドウ領域のパレットナンバー 
  EARTH_PLACE_WIN_CGX,  //ウインドウキャラ領域の開始キャラクタナンバー
};

static const BMPWIN_DAT EarthInfoWinData =
{
  EARTH_TEXT_PLANE, //ウインドウ使用フレーム
  EARTH_INFO_WIN_PX,EARTH_INFO_WIN_PY,//ウインドウ領域の左上XY座標（キャラ単位指定）
  EARTH_INFO_WIN_SX,EARTH_INFO_WIN_SY,//ウインドウ領域のXYサイズ（キャラ単位指定）
  EARTH_INFO_WIN_PAL, //ウインドウ領域のパレットナンバー 
  EARTH_INFO_WIN_CGX, //ウインドウキャラ領域の開始キャラクタナンバー
};

static const BMPWIN_DAT EarthIconWinData =
{
  EARTH_ICON_PLANE, //ウインドウ使用フレーム
  EARTH_ICON_WIN_PX,EARTH_ICON_WIN_PY,//ウインドウ領域の左上XY座標（キャラ単位指定）
  EARTH_ICON_WIN_SX,EARTH_ICON_WIN_SY,//ウインドウ領域のXYサイズ（キャラ単位指定）
  EARTH_ICON_WIN_PAL, //ウインドウ領域のパレットナンバー 
  EARTH_ICON_WIN_CGX, //ウインドウキャラ領域の開始キャラクタナンバー
};

// みるボタン @ GS追加
static const BMPWIN_DAT EarthLookWinData =
{
  EARTH_ICON_PLANE, //ウインドウ使用フレーム
  EARTH_LOOK_WIN_PX,EARTH_LOOK_WIN_PY,//ウインドウ領域の左上XY座標（キャラ単位指定）
  EARTH_LOOK_WIN_SX,EARTH_LOOK_WIN_SY,//ウインドウ領域のXYサイズ（キャラ単位指定）
  EARTH_LOOK_WIN_PAL, //ウインドウ領域のパレットナンバー 
  EARTH_LOOK_WIN_CGX, //ウインドウキャラ領域の開始キャラクタナンバー
};

static const EARTH_BMPLIST MenuList[] =
{
  {mes_earth_02_05,MENU_WORLD},
  {mes_earth_02_06,MENU_REGISTRATION},
  {mes_earth_02_07,MENU_END},
};

static const BMPMENULIST_HEADER MenuListHeader = {
  /* 表示文字データポインタ(プログラム設定) */NULL,/* カーソル移動ごとのコールバック関数 */NULL,
  /* 一列表示ごとのコールバック関数 */NULL,/* 対応ビットマップ(プログラム設定) */NULL,
  /* リスト項目数 */NELEMS(MenuList),/* 表示最大項目数 */NELEMS(MenuList),    
  /* ラベル表示Ｘ座標 */0,/* 項目表示Ｘ座標 */12,/* カーソル表示Ｘ座標 */0,/* 表示Ｙ座標 */0,
  /* 文字色 */FBMP_COL_BLACK,/* 背景色 */FBMP_COL_WHITE,/* 文字影色 */FBMP_COL_BLK_SDW,   
  /* 文字間隔Ｘ */0,/* 文字間隔Ｙ */16,/* ページスキップタイプ */BMPMENULIST_NO_SKIP,   
  /* 文字指定 */0,/*ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)*/0,           
  NULL,
  /* 文字サイズX(ドット) */0,/* 文字サイズY(ドット) */0,
  /* 表示に使用するメッセージバッファ */ NULL,
  /* 表示に使用するプリントユーティリティ */NULL,
  /* 表示に使用するプリントキュー */NULL,
  /* 表示に使用するフォントハンドル */NULL,
};

static const BMPMENULIST_HEADER PlaceListHeader = {
  /* 表示文字データポインタ(プログラム設定) */NULL,/* カーソル移動ごとのコールバック関数 */NULL,
  /* 一列表示ごとのコールバック関数 */NULL,/* 対応ビットマップ(プログラム設定) */NULL,
  /* リスト項目数(プログラム設定) */0,/* 表示最大項目数 */7,
  /* ラベル表示Ｘ座標 */0,/* 項目表示Ｘ座標 */12,/* カーソル表示Ｘ座標 */0,/* 表示Ｙ座標 */0,
  /* 文字色 */FBMP_COL_BLACK,/* 背景色 */FBMP_COL_WHITE,/* 文字影色 */FBMP_COL_BLK_SDW,   
  /* 文字間隔Ｘ */0,/* 文字間隔Ｙ */16,/* ページスキップタイプ */BMPMENULIST_LRKEY_SKIP,    
  /* 文字指定 */0,/*ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)*/0,           
  NULL,
  /* 文字サイズX(ドット) */0,/* 文字サイズY(ドット) */0,
  /* 表示に使用するメッセージバッファ */ NULL,
  /* 表示に使用するプリントユーティリティ */NULL,
  /* 表示に使用するプリントキュー */NULL,
  /* 表示に使用するフォントハンドル */NULL,
};

//============================================================================================
//
//  メイン関数
//
//============================================================================================
//============================================================================================
//  初期化プロセス
//============================================================================================
static GFL_PROC_RESULT Earth_Demo_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk) 
{
  EARTH_DEMO_WORK * wk;
  HEAPID heapID = HEAPID_WIFIEARTH;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, heapID, 0x80000 );

  //ワークエリア取得
  wk = GFL_PROC_AllocWork(proc, sizeof(EARTH_DEMO_WORK), heapID);
  GFL_STD_MemClear(wk, sizeof(EARTH_DEMO_WORK));
  wk->heapID = heapID;

  //セーブデータポインタ作成
  wk->gamedata = (GAMEDATA*)pwk;
  wk->scw = GAMEDATA_GetSaveControlWork( wk->gamedata );
  wk->mystatus = GAMEDATA_GetMyStatus( wk->gamedata );
  wk->wifi_sv = SaveData_GetWifiHistory( wk->scw );

  //日本語版かどうかの判別フラグ
  if( CasetteLanguage == LANG_JAPAN ){
    // 日本語バージョンでは日本しか登録できない
    wk->Japan_ROM_mode = TRUE;
  } else {
    // 日本語バージョン以外は最初から地球儀が全部見える
    wk->Japan_ROM_mode = FALSE;
  }
#ifdef PM_DEBUG
  // Ｌボタンをおしていると入力情報をクリアした上で世界から入力できる
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L){
    WIFI_RegistratonInit( wk->gamedata );
    wk->Japan_ROM_mode = FALSE;
  }
#endif
  //ステータス取得
  wk->my_nation = MyStatus_GetMyNation( wk->mystatus );
  wk->my_area = MyStatus_GetMyArea( wk->mystatus );
  wk->my_worldopen_flag = WIFIHISTORY_GetWorldFlag( wk->wifi_sv );
  wk->TaskMenuWork[0]   = NULL;
  wk->TaskMenuWork[1]   = NULL;

//  OS_Printf("mystatus=%08x\n", (u32)wk->mystatus);
//  OS_Printf("my_nation=%d, my_area=%d\n", wk->my_nation, wk->my_area);

  //ＶＲＡＭ設定
  Earth_VramBankSet();
  //ＢＧライブラリ設定
  Earth_BGsysSet( wk->heapID );
  //メッセージ表示システム初期化
  wk->wordset = WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, EARTH_NAME_SIZE, wk->heapID );

  //３Ｄ描画スイッチ設定
  wk->Draw3Dsw = DRAW3D_DISABLE;
  GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_SUB);

  //タスクシステム設定
  wk->tcbl = GFL_TCBL_Init(wk->heapID, wk->heapID, 32, 32);
  wk->printCursor = APP_KEYCURSOR_Create( 15, TRUE, FALSE, wk->heapID );
  wk->printQue = PRINTSYS_QUE_Create(wk->heapID);

  // 通信アイコン表示
  GFL_NET_ReloadIconTopOrBottom( TRUE, wk->heapID );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON ); 
  
  
  
  //キーシステム設定
  //GFL_UI_KEY_SetRepeatSpeed( 8/*SYS_KEYREPEAT_SPEED_DEF*/, 4/*SYS_KEYREPEAT_WAIT_DEF*/ );

#if PL_T0864_080714_FIX
  // メッセージ表示関係を設定
  MsgPrintSkipFlagSet(MSG_SKIP_ON);
  MsgPrintAutoFlagSet(MSG_AUTO_OFF);
  MsgPrintTouchPanelFlagSet(MSG_TP_OFF);
#endif

  //地域リストデータ作成
  EarthListLoad(wk);
  
  return  GFL_PROC_RES_FINISH;
}

static BOOL Earth_SearchPosInfo( EARTH_DEMO_WORK* wk, u32* outIndex );
//============================================================================================
//  メインプロセス
//============================================================================================
static GFL_PROC_RESULT Earth_Demo_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk) 
{
  EARTH_DEMO_WORK * wk  = (EARTH_DEMO_WORK*)mywk;
  GFL_PROC_RESULT sys_result = GFL_PROC_RES_CONTINUE;

  // タスクメイン
  GFL_TCBL_Main(wk->tcbl);

  // メニューのアニメ実行が行われていなかったらサブシーケンス実行可能
  sys_result = SubSeq_Main( wk, seq );
  if(wk->g3DobjEarth!=NULL){
    Earth3D_Draw(wk);   //３Ｄ描画エンジン
  }
  _menu_main(wk);     // タスクメニュー描画
  PRINTSYS_QUE_Main( wk->printQue );
  return  sys_result;
}




//----------------------------------------------------------------------------------
/**
 * @brief ボタン決定アニメ
 *
 * @param   wk    
 * @param   id    
 */
//----------------------------------------------------------------------------------
static void _menu_anime_on( EARTH_DEMO_WORK *wk, int id )
{
  // ボタンが存在しているならボタン決定アニメ発動
  if(wk->TaskMenuWork[id]!=NULL){
    if(id==TASKMENU_MIRU){
      APP_TASKMENU_WIN_SetActive( wk->TaskMenuWork[TASKMENU_MIRU], TRUE );
    }else if(id==TASKMENU_YAMERU){
      APP_TASKMENU_WIN_SetDecide( wk->TaskMenuWork[TASKMENU_YAMERU], TRUE );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief タスクメニューメイン処理
 *
 * @param   wk    
 *
 * @retval  static    
 */
//----------------------------------------------------------------------------------
static void _menu_main( EARTH_DEMO_WORK *wk )
{
  if(wk->TaskMenuWork[0]!=NULL){
    APP_TASKMENU_WIN_Update( wk->TaskMenuWork[0] );
  }
  if(wk->TaskMenuWork[1]!=NULL){
    APP_TASKMENU_WIN_Update( wk->TaskMenuWork[1] );
  }

}

static const GFL_UI_TP_HITTBL touch_move_table[]={
  {   0, 167,   0, 239, },
  { 168, 191,  72, 183, },
  {GFL_UI_TP_HIT_END,0,0,0},    // 終了データ
};
#ifdef DEBUG_RAPID_PAD
static int debug_flag=0;
static void _debug_pad( EARTH_DEMO_WORK *wk )
{
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_R){
    debug_flag ^=1;
    if(debug_flag==0){
      wk->trg  = PAD_BUTTON_X;
    }else{
      wk->cont = PAD_KEY_LEFT;
    }
  }else{
    debug_flag=0;
  }
}
#endif

//----------------------------------------------------------------------------------
/**
 * @brief サブシーケンス制御
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  GFL_PROC_RESULT   
 */
//----------------------------------------------------------------------------------
static GFL_PROC_RESULT SubSeq_Main( EARTH_DEMO_WORK *wk, int *seq )
{
  ARCHANDLE* p_handle;
  GFL_PROC_RESULT sys_result = GFL_PROC_RES_CONTINUE;

  wk->trg = GFL_UI_KEY_GetTrg(); 
  wk->cont = GFL_UI_KEY_GetCont();

  if(GFL_UI_TP_HitCont( touch_move_table )!=GFL_UI_TP_HIT_NONE){
    wk->tpcont = GFL_UI_TP_GetPointCont(&wk->tpx, &wk->tpy); 
  }else{
    wk->tpcont = FALSE;
  }
  if(wk->tpcont == TRUE){
    wk->tptrg = GFL_UI_TP_GetTrg(); 
  } else {
    wk->tptrg = GFL_UI_TP_GetPointTrg(&wk->tpx, &wk->tpy); 
  }

  switch(*seq){
  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_MODELLOAD://モデルデータロード
    //メッセージデータ読み込み
    wk->msg_man = GFL_MSG_Create
        (GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_earth_guide_dat, wk->heapID);

    p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFI_EARTH, wk->heapID );

    Earth_ModelLoad(wk, p_handle);  //モデルロードメイン
    Earth_BGdataLoad(wk, p_handle); //ＢＧデータロード

    GFL_ARC_CloseDataHandle( p_handle );
    //輝度変更セット（ＩＮ）
    wk->fade_end_flag = FALSE;
    WIPE_SYS_Start( WIPE_PATTERN_WMS,WIPE_TYPE_FADEIN,WIPE_TYPE_FADEIN,
            WIPE_FADE_BLACK,WIPE_DEF_DIV,WIPE_DEF_SYNC,wk->heapID);

    GFL_BG_SetVisible( EARTH_TEXT_PLANE, VISIBLE_ON );
    GFL_BG_SetVisible( EARTH_ICON_PLANE, VISIBLE_ON );
    GFL_BG_SetVisible( EARTH_BACK_S_PLANE, VISIBLE_ON );
    GFL_BG_SetVisible( EARTH_BACK_M_PLANE, VISIBLE_ON );

    *seq = EARTHDEMO_SEQ_DISPON;
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_DISPON:  //描画ＯＮ
    if(WIPE_SYS_EndCheck() == TRUE){  //輝度変更待ち
      *seq = EARTHDEMO_SEQ_WELCOME_MSG;
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_WELCOME_MSG: //「ようこそ」
    if(Earth_MsgPrint(wk,mes_earth_01_01,A_BUTTON_NOWAIT) == TRUE){
      //住んでいる場所入力済？
      if(wk->my_nation == 0){
        //未登録
        *seq = EARTHDEMO_SEQ_MAINMENU;  //メインメニュー設定へ
      }else{
        //登録済
        *seq = EARTHDEMO_SEQ_EARTH_DISPON;  //３Ｄ地球儀モードＯＮへ
      }
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_MAINMENU:  //メインメニュー設定

    if(Earth_MsgPrint(wk,mes_earth_01_02,A_BUTTON_NOWAIT) == TRUE){
      //選択リスト表示
      Earth_BmpListAdd(wk, &EarthMenuWinData, &MenuListHeader, MenuList);
      *seq = EARTHDEMO_SEQ_MAINMENU_SELECT; //選択モードへ
    }
    break;

  case EARTHDEMO_SEQ_MAINMENU_SELECT: //メインメニュー選択モード
    {
      u32 list_result;
      // printQueに残っている場合はBMPLISTのメインを回さない
      if(PRINTSYS_QUE_IsFinished( wk->printQue )==FALSE){
        break;
      }

      list_result = BmpMenuList_Main(wk->bmplist);

      if(list_result == BMPMENULIST_NULL){
        break;
      }
      Earth_BmpListDel(wk);//選択リスト削除処理


      switch(list_result){
      default:
      case MENU_WORLD:  //「いちらん」
        *seq = EARTHDEMO_SEQ_EARTH_DISPON;  //３Ｄ地球儀モードＯＮへ
        PMSND_PlaySE( WIFIEARTH_SND_SELECT );
        break;
      case MENU_REGISTRATION: //「とうろく」
        *seq = EARTHDEMO_SEQ_REGISTRATIONMENU;  //登録メニュー設定へ
        PMSND_PlaySE( WIFIEARTH_SND_SELECT );
        break;
      case BMPMENULIST_CANCEL:
      case MENU_END:  //「やめる」
        *seq = EARTHDEMO_SEQ_END; //終了処理へ
        PMSND_PlaySE( WIFIEARTH_SND_YAMERU );
        break;
      }
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_REGISTRATIONMENU:  //登録メニュー設定

    if(Earth_MsgPrint(wk,mes_earth_01_03,A_BUTTON_NOWAIT) == TRUE){
      //はい/いいえウィンドウ表示
      wk->yesnowin = BmpMenu_YesNoSelectInit
        (&EarthYesNoWinData, EARTH_MENUWINCHR_NUM,EARTH_MENUWIN_PAL, 0, wk->heapID);
      *seq = EARTHDEMO_SEQ_REGISTRATIONMENU_SELECT; //選択モードへ
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONMENU_SELECT: //登録メニュー選択モード
    {
      u32 list_result = BmpMenu_YesNoSelectMain(wk->yesnowin);

      switch(list_result){
      case 0:   //「はい」
        if(wk->Japan_ROM_mode == TRUE){
          wk->my_nation_tmp = WIFI_COUNTRY_GetJapanID(); //日本語版限定処理（国入力スキップ）
          *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_AREA;    //地域別登録リスト選択へ
        }else{
          *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_NATION;  //国別登録リスト選択へ
        }
        break;
      case BMPMENU_CANCEL:  //「いいえ」
        *seq = EARTHDEMO_SEQ_MAINMENU;
        break;
      }
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_NATION: //国別登録リスト設定
    wk->my_nation_tmp = 0;//登録情報テンポラリ初期化
//    Earth_BmpListAddGmmAll
//      (wk, &EarthPlaceListWinData, &PlaceListHeader, NARC_message_wifi_place_msg_world_dat);
      Earth_BmpListAddGmmEnableList
        (wk, &EarthPlaceListWinData, &PlaceListHeader, NARC_message_wifi_place_msg_world_dat);

    *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_NATION2;  //国別登録リスト選択へ
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_NATION2:
    if(Earth_MsgPrint(wk,mes_earth_01_04,A_BUTTON_NOWAIT) == TRUE){
      *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_NATION_SELECT;  //国別登録リスト選択へ
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_NATION_SELECT:  //国別登録リスト選択モード
    {
      int list_result;
      // printQueに残っている場合はBMPLISTのメインを回さない
      if(PRINTSYS_QUE_IsFinished( wk->printQue )==FALSE){
        break;
      }
      list_result = BmpMenuList_Main(wk->bmplist);
#ifdef WIFI_ERATH_DEBUG
      EarthDebugNationMarkSet(wk);
#endif
      if(list_result == BMPMENULIST_NULL){
        break;
      }
      Earth_BmpListDel(wk);//選択リスト削除処理

      switch(list_result){
      default:
        {
          wk->my_nation_tmp = list_result;  //国登録

          if(WIFI_LocalAreaExistCheck(wk->my_nation_tmp) == TRUE){
            //地域が存在する場合
            *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_AREA; //地域別登録リスト選択へ
          }else{
            //地域が存在しない場所の登録
            wk->my_area_tmp = 0;        //地域登録（なし）
            *seq = EARTHDEMO_SEQ_FINAL_REGISTRATION;  //国地域登録最終確認へ
          }
        }
        PMSND_PlaySE( WIFIEARTH_SND_SELECT );
        break;
      case BMPMENULIST_CANCEL:  //「いいえ」キャンセル
        //*seq = EARTHDEMO_SEQ_REGISTRATIONMENU;  //登録メニューへ
        *seq = EARTHDEMO_SEQ_MAINMENU;
        PMSND_PlaySE( WIFIEARTH_SND_YAMERU );
        break;
      }
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_AREA: //地域別登録リスト設定
    wk->my_area_tmp = 0;//登録情報テンポラリ初期化
    Earth_BmpListAddGmmAll(wk, &EarthPlaceListWinData,&PlaceListHeader,
          WIFI_COUNTRY_CountryCodeToPlaceMsgDataID(wk->my_nation_tmp));
    *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_AREA2;
    break;
  case EARTHDEMO_SEQ_REGISTRATIONLIST_AREA2:  //地域別登録リスト設定
    if(Earth_MsgPrint(wk,mes_earth_01_05,A_BUTTON_NOWAIT) == TRUE){
      *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_AREA_SELECT;  //地域別登録リスト選択へ
    }
    break;

  case EARTHDEMO_SEQ_REGISTRATIONLIST_AREA_SELECT:  //地域別登録リスト選択モード
    {
      int list_result;
      // printQueに残っている場合はBMPLISTのメインを回さない
      if(PRINTSYS_QUE_IsFinished( wk->printQue )==FALSE){
        break;
      }
      list_result = BmpMenuList_Main(wk->bmplist);
#ifdef WIFI_ERATH_DEBUG
      EarthDebugAreaMarkSet(wk);
#endif
      if(list_result == BMPMENULIST_NULL){
        break;
      }
      Earth_BmpListDel(wk);//選択リスト削除処理


      switch(list_result){
      default:
        wk->my_area_tmp = list_result;      //地域登録
        *seq = EARTHDEMO_SEQ_FINAL_REGISTRATION;  //国地域登録最終確認へ
        PMSND_PlaySE( WIFIEARTH_SND_SELECT );
        break;
      case BMPMENULIST_CANCEL://キャンセル
        PMSND_PlaySE( WIFIEARTH_SND_YAMERU );
        if(wk->Japan_ROM_mode == TRUE){
          //*seq = EARTHDEMO_SEQ_REGISTRATIONMENU;  //登録メニューへ(日本版の場合)
          *seq = EARTHDEMO_SEQ_MAINMENU;
        } else {
          *seq = EARTHDEMO_SEQ_REGISTRATIONLIST_NATION; //国別登録リスト設定へ
        }
      }
    }
    break;

  case EARTHDEMO_SEQ_FINAL_REGISTRATION:  //登録最終確認設定
    Earth_MyPlaceInfoWinSet2( wk, wk->my_nation_tmp, wk->my_area_tmp );//登録地名表示
    *seq = EARTHDEMO_SEQ_FINAL_REGISTRATION_YESNO;  //登録最終確認へ
    break;

  case EARTHDEMO_SEQ_FINAL_REGISTRATION_YESNO:  //登録最終確認

    if(Earth_MsgPrint(wk,mes_earth_01_06,A_BUTTON_NOWAIT) == TRUE){
      //はい/いいえウィンドウ表示
      wk->yesnowin = BmpMenu_YesNoSelectInit
        (&EarthYesNoWinData, EARTH_MENUWINCHR_NUM,EARTH_MENUWIN_PAL, 0, wk->heapID);
      *seq = EARTHDEMO_SEQ_FINAL_REGISTRATION_SELECT; //登録最終確認選択モードへ
    }
    break;

  case EARTHDEMO_SEQ_FINAL_REGISTRATION_SELECT: //登録最終確認選択モード
    {
      u32 list_result = BmpMenu_YesNoSelectMain(wk->yesnowin);

      switch(list_result){
      case 0:   //「はい」
        Earth_MyPlaceInfoWinRelease( wk );
        //登録データセーブ
        WIFIHISTORY_SetMyNationArea
          (wk->wifi_sv, wk->mystatus, wk->my_nation_tmp, wk->my_area_tmp);
        // ビーコンデータにも登録
        GAMEBEACON_SendDataUpdate_NationArea( wk->my_nation_tmp, wk->my_area_tmp );

        wk->my_nation = wk->my_nation_tmp;
        wk->my_area = wk->my_area_tmp;

        *seq = EARTHDEMO_SEQ_EARTH_DISPON;  //３Ｄ地球儀モードＯＮへ
        break;
      case BMPMENU_CANCEL:  //「いいえ」
        Earth_MyPlaceInfoWinRelease( wk );
        *seq = EARTHDEMO_SEQ_MAINMENU;
        break;
      }
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_EARTH_DISPON:  //３Ｄ地球儀モードＯＮ
    //日本語版？
    if((wk->Japan_ROM_mode == TRUE)&&(wk->my_worldopen_flag == FALSE)){
      //日本語版で海外のモードが開いていない場合は日本限定マップ
      wk->earth_mode = JAPAN_MODE;
    }else{
      //その他、世界マップ
      wk->earth_mode = GLOBAL_MODE;
    }
#ifdef WIFI_ERATH_DEBUG_ALL_DRAW
    wk->earth_mode = GLOBAL_MODE; // DEBUG
#endif
    EarthDataInit(wk);                //地球データ初期化
    EarthList_NationAreaListSet(wk);  //登録地域データ設定
    EarthCameraStart(wk);             //カメラ開始設定

    //メッセージ画面クリア
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );
    GFL_BMPWIN_TransVramCharacter(wk->msgwin);
    //「やめる」アイコンＯＮ
    TaskMenuPut( wk, 1 );
//    GFL_BMPWIN_MakeScreen(wk->iconwin);
//    BmpWinFrame_Write(wk->iconwin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM,EARTH_MENUWIN_PAL);

    //住んでいる場所入力済？
    if(wk->my_nation != 0){
      Earth_MyPlaceInfoWinSet( wk );//登録地名表示
    }
    Earth_PosInfoPut( wk );
    wk->info_mode = 0;

    wk->Draw3Dsw = DRAW3D_ENABLE;//３Ｄ描画スイッチ設定ＯＮ

    *seq = EARTHDEMO_SEQ_MOVE_EARTH;
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_MOVE_EARTH:  //地球回転
    {
      u32 minindex; // ダミー
      u16 camera_status_backup = wk->camera_status;

      if(PRINTSYS_QUE_IsFinished( wk->printQue )==FALSE){
        break;
      }

      //タッチパネルコントロール
      Earth_TouchPanel(wk);

      if( Earth_SearchPosInfo(wk, &minindex ) ){
        // 「みる」アイコンＯＮ
        TaskMenuPut( wk, 0 );
      }else{
        //「みる」アイコンＯＦＦ
        TaskMenuOff( wk, 0 );
      }

      //終了判定
      if((wk->trg & PAD_BUTTON_B)||(wk->tp_result & PAD_BUTTON_B)){
        //「やめる」アイコンＯＦＦ
        TaskMenuOff( wk, TASKMENU_MIRU );
        _menu_anime_on( wk, TASKMENU_YAMERU );
        PMSND_PlaySE( WIFIEARTH_SND_YAMERU );

        //メッセージ画面クリア
        GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );
        GFL_BMPWIN_TransVramCharacter(wk->msgwin);
        *seq = EARTHDEMO_SEQ_BUTTON_ANIME;
      }else{

// ポイントの上に重なってRボタンを押すとprintQueをあふれされる操作（対策済み）
#ifdef DEBUG_RAPID_PAD
        _debug_pad(wk);
#endif

        // 「みる」機能
        if( ((wk->trg & PAD_BUTTON_X)||(wk->tp_result & PAD_BUTTON_X))&&(wk->info_mode == 0) ){
          wk->info_mode = 1;
          _menu_anime_on( wk, TASKMENU_MIRU );
          Earth_PosInfoPut( wk );

          // info_modeには Earth_PosInfoPut の検索結果が入っている
          if( wk->info_mode == 1 ){
            PMSND_PlaySE( WIFIEARTH_SND_XSELECT );
          }
          break;
        }

        // 抜け判定
        if((wk->trg & (PAD_BUTTON_X|PAD_BUTTON_A|PAD_BUTTON_B)||(wk->tp_result & PAD_BUTTON_X))
            &&(wk->info_mode == 1)){
          wk->info_mode = 0;
          Earth_PosInfoPut( wk );
          break;
        }

        {
          BOOL move_flag;
          //地球回転コントロール
          move_flag = Earth3D_Control(wk, wk->trg, wk->cont);

          if( (move_flag == TRUE)&&(wk->info_mode == 1) ){
            wk->info_mode = 0;
            Earth_PosInfoPut( wk );
          }
        }
        //カメラ遠近移動判定
        if(camera_status_backup != wk->camera_status){
          *seq = EARTHDEMO_SEQ_MOVE_CAMERA;
          if( wk->camera_status == CAMERA_FAR ){
            PMSND_PlaySE( WIFIEARTH_SND_ZOMEOUT );
          }else{
            PMSND_PlaySE( WIFIEARTH_SND_ZOMEIN );
          }
        }
      }
    }

#ifdef WIFI_ERATH_DEBUG
    EarthDebugWinRotateInfoWrite(wk);//デバッグ情報表示
#endif
    break;

  case EARTHDEMO_SEQ_BUTTON_ANIME:
    if(APP_TASKMENU_WIN_IsFinish( wk->TaskMenuWork[1]) ){
      //住んでいる場所入力済？
      if(wk->my_nation == 0){
        //メニュー画面へ
        wk->Draw3Dsw = DRAW3D_BANISH;//３Ｄ画面消去→スイッチ設定ＯＦＦ
        *seq = EARTHDEMO_SEQ_MAINMENU;
      }else{
        Earth_MyPlaceInfoWinRelease( wk );
        *seq = EARTHDEMO_SEQ_END;//終了
      }
      TaskMenuOff( wk, TASKMENU_YAMERU );
    }
    break;

  case EARTHDEMO_SEQ_MOVE_CAMERA: //カメラ距離移動
    {
      BOOL end_flag = Earth3D_CameraMoveNearFar(wk);

      if(end_flag == TRUE){
        *seq = EARTHDEMO_SEQ_MOVE_EARTH;
      }
    }
    break;

  //-----------------------------------------------------------
  case EARTHDEMO_SEQ_END:   //終了処理
    wk->fade_end_flag = FALSE;//輝度変更待ちフラグリセット
    WIPE_SYS_Start( WIPE_PATTERN_WMS,WIPE_TYPE_FADEOUT,WIPE_TYPE_FADEOUT,
            WIPE_FADE_BLACK,WIPE_DEF_DIV,WIPE_DEF_SYNC,wk->heapID);
    *seq = EARTHDEMO_SEQ_EXIT;
    break;

  case EARTHDEMO_SEQ_EXIT:  //終了

    if(WIPE_SYS_EndCheck() == TRUE){  //輝度変更待ち
      wk->Draw3Dsw = DRAW3D_ENABLE;//３Ｄ描画スイッチ設定ＯＦＦ
      //ＢＧデータ破棄
      Earth_BGdataRelease(wk);
      //モデルデータ破棄
      Earth_ModelRelease(wk);
      //メッセージデータ破棄
      GFL_MSG_Delete(wk->msg_man);

      (*seq) = 0;
      sys_result = GFL_PROC_RES_FINISH; //終了フラグ
    }
    break;
  }

  PRINT_UTIL_Trans( &wk->printUtil, wk->printQue );
  PRINT_UTIL_Trans( &wk->printUtilInfo, wk->printQue );
  PRINT_UTIL_Trans( &wk->printUtilLook, wk->printQue );
  PRINT_UTIL_Trans( &wk->printUtilIcon, wk->printQue );
  PRINT_UTIL_Trans( &wk->printUtilMsg, wk->printQue );

  return sys_result;
}

//============================================================================================
//  終了プロセス
//============================================================================================
static GFL_PROC_RESULT Earth_Demo_Exit(GFL_PROC * proc, int * seq, void * pwk, void * mywk) 
{
  EARTH_DEMO_WORK * wk  = (EARTH_DEMO_WORK*)mywk;
  int heapID = wk->heapID;

  PRINTSYS_QUE_Clear( wk->printQue ); 
  PRINTSYS_QUE_Delete(wk->printQue);

  //ＢＧ表示停止
  GFL_BG_SetVisible( EARTH_TEXT_PLANE, VISIBLE_OFF );
  GFL_BG_SetVisible( EARTH_ICON_PLANE, VISIBLE_OFF );
  GFL_BG_SetVisible( EARTH_BACK_S_PLANE, VISIBLE_OFF );
  GFL_BG_SetVisible( EARTH_BACK_M_PLANE, VISIBLE_OFF );
  GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);

  APP_KEYCURSOR_Delete(wk->printCursor);
  GFL_TCBL_Exit(wk->tcbl);

  //各種内部確保ワーク開放
  WORDSET_Delete( wk->wordset );
  Earth_BGsysRelease();

  //ワーク開放
  GFL_PROC_FreeWork( proc );

  //ヒープ開放
  GFL_HEAP_DeleteHeap( heapID );

  return  GFL_PROC_RES_FINISH;
}

//============================================================================================
//
//  ローカル関数（メインシステム関連）
//
//============================================================================================
//----------------------------------
//ＶＲＡＭ設定
//----------------------------------
static void Earth_VramBankSet(void)
{
  GFL_DISP_VRAM vramSetTable = {
    GX_VRAM_BG_128_C,                 // メイン2DエンジンのBG
    GX_VRAM_BGEXTPLTT_NONE,           // メイン2DエンジンのBG拡張パレット
    GX_VRAM_SUB_BG_32_H,              // サブ2DエンジンのBG
    GX_VRAM_SUB_BGEXTPLTT_NONE,       // サブ2DエンジンのBG拡張パレット
    GX_VRAM_OBJ_16_F,                 // メイン2DエンジンのOBJ
    GX_VRAM_OBJEXTPLTT_NONE,          // メイン2DエンジンのOBJ拡張パレット
    GX_VRAM_SUB_OBJ_16_I,             // サブ2DエンジンのOBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,      // サブ2DエンジンのOBJ拡張パレット
    GX_VRAM_TEX_01_AB,                // テクスチャイメージスロット
    GX_VRAM_TEXPLTT_0123_E,           // テクスチャパレットスロット
    GX_OBJVRAMMODE_CHAR_1D_32K,       // メインOBJマッピングモード
    GX_OBJVRAMMODE_CHAR_1D_32K,       // サブOBJマッピングモード
  };
  // VRAM全クリア
  GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
  MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
  (void)GX_DisableBankForLCDC();

  MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
  MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

  MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
  MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

  GFL_DISP_SetBank( &vramSetTable );
}

//----------------------------------
//ＢＧシステム設定
//----------------------------------
static void Earth_BGsysSet( HEAPID heapID )
{
  GFL_BG_SYS_HEADER bgsysHeader = {
    GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
  };

  //ＢＧシステム起動
  GFL_BG_Init(heapID);
  //ＢＧモード設定
  GFL_BG_SetBGMode(&bgsysHeader);

  //３Ｄシステム起動
  GFL_G3D_Init( GFL_G3D_VMANLNK,
                GFL_G3D_TEX384K,
                GFL_G3D_VMANLNK,
                GFL_G3D_PLT32K,
                0,
                heapID,
                NULL );
  GFL_BG_SetBGControl3D(1);

  //ビットマップウインドウ起動
  GFL_BMPWIN_Init(heapID);

  GFL_DISP_SetDispOn();
}

static void Earth_BGsysRelease( void )
{
  GFL_BMPWIN_Exit();
  GFL_G3D_Exit();
  GFL_BG_Exit();
}


//============================================================================================
//
//  ローカル関数（地域データ取得関連）
//
//============================================================================================
//----------------------------------
//地点リストロード
//----------------------------------
static void EarthListLoad( EARTH_DEMO_WORK * wk )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFI_EARTH_PLACE, wk->heapID );

  //地点リスト総数初期化
  wk->placelist.listcount = 0;

  {//地点マーク回転初期化（国データバイナリデータロード）
    void* filep;
    EARTH_DATA_NATION* listp;
    u32 size;
    int i,listcount;

    filep = GFL_ARCHDL_UTIL_LoadEx
      ( p_handle, NARC_wifi_earth_place_place_pos_wrd_dat, FALSE, wk->heapID, &size );

    listp = (EARTH_DATA_NATION*)filep;  //ファイル読み込み用に変換
    listcount = size/6;       //地点数取得（データ長：１地点につき６バイト）

    listp++;  //1originのため読み飛ばし
    for(i=1;i<listcount;i++){
      if(listp->flag != 2){ //2の場合は地域リストが存在する
//        OS_Printf("pre y=%d,", listp->y);
        EarthListSet(wk,wk->placelist.listcount,listp->x,listp->y,i,0);
        wk->placelist.listcount++;
      }
      listp++;
    }
    GFL_HEAP_FreeMemory(filep);
  }
  {//地点マーク回転初期化（地域データバイナリデータロード）
    void* filep;
    EARTH_DATA_AREA* listp;
    u32 size, data_id;
    int i,datLen,index,listcount;

    index = 1;  //1orgin
    datLen = WIFI_COUNTRY_GetDataLen();

    while(index < datLen){

      data_id = WIFI_COUNTRY_DataIndexToPlaceDataID( index );
      filep = GFL_ARCHDL_UTIL_LoadEx( p_handle, data_id, FALSE, wk->heapID, &size );

      listp = (EARTH_DATA_AREA*)filep;  //ファイル読み込み用に変換
      listcount = size/4;   //地点数取得（データ長：１地点につき４バイト）

      listp++;  //1originのため読み飛ばし
      for(i=1;i<listcount;i++){//1orgin
        EarthListSet(wk,wk->placelist.listcount,listp->x,listp->y,
                WIFI_COUNTRY_DataIndexToCountryCode(index),i);
        wk->placelist.listcount++;
        listp++;
      }
      GFL_HEAP_FreeMemory(filep);
      index++;
    }
  }
  GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------
//リスト登録
//----------------------------------
static void EarthListSet( EARTH_DEMO_WORK * wk,u32 index,s16 x,s16 y,u16 nationID,u16 areaID )
{
  MtxFx33 rotMtx = {FX32_ONE,0,0,0,FX32_ONE,0,0,0,FX32_ONE};
  VecFx32 rotVec;
//  OS_Printf(" func in y=%d\n", y);

  wk->placelist.place[index].x = x; //Ｘ回転オフセット取得
  wk->placelist.place[index].y = y; //Ｙ回転オフセット取得
  
  rotVec.x = x;
  rotVec.y = y;
  rotVec.z = INIT_EARTH_ROTATE_ZVAL;
  EarthVecFx32_to_MtxFx33_place(&rotMtx,&rotVec); //初期位置からの相対座標計算
  wk->placelist.place[index].rotate = rotMtx;

  //マーク色設定(wifihistory定義に沿うこと)
  wk->placelist.place[index].col = WIFIHISTORY_GetStat(wk->wifi_sv,nationID,areaID);

#ifdef WIFI_ERATH_DEBUG_ALL_DRAW
  wk->placelist.place[index].col = WIFIHIST_STAT_EXIST; // DEBUG
#endif

  wk->placelist.place[index].nationID = nationID; //該当国ＩＤ
  wk->placelist.place[index].areaID = areaID;   //該当地域ＩＤ
}

//----------------------------------
//登録地域情報セット
//----------------------------------
static void EarthList_NationAreaListSet( EARTH_DEMO_WORK * wk )
{
  int i;

  for(i=0;i<wk->placelist.listcount;i++){
    if((wk->placelist.place[i].nationID == wk->my_nation) //該当国ＩＤ
      &&(wk->placelist.place[i].areaID == wk->my_area)){  //該当地域ＩＤ

      wk->placelist.place[i].col = MARK_RED;  //マーク色設定
      wk->rotate.x  = wk->placelist.place[i].x; //地球儀連動
      wk->rotate.y  = wk->placelist.place[i].y; //地球儀連動
    }
  }
}

//----------------------------------
//ローカルエリア存在チェック
//----------------------------------
static int  EarthAreaTableGet(int nationID)
{
  return WIFI_COUNTRY_CountryCodeToDataIndex( nationID );
}

// 「みる」「やめる」座標
#define MENU_BT0_X  (  0 )
#define MENU_BT1_X  ( 23 )
#define MENU_BT_Y   ( 21 )
#define MENU_BT_W   (  9 )
#define MENU_BT_H   (  3 )


// 「みる」「やめる」ボタン範囲定義
static const GFL_UI_TP_HITTBL touch_tbl[]={
  { MENU_BT_Y*8,(MENU_BT_Y+MENU_BT_H)*8-1, MENU_BT0_X*8, (MENU_BT0_X+MENU_BT_W)*8-1 },
  { MENU_BT_Y*8,(MENU_BT_Y+MENU_BT_H)*8-1, MENU_BT1_X*8, (MENU_BT1_X+MENU_BT_W)*8-1 },
  {GFL_UI_TP_HIT_END, 0, 0, 0 },
};

//============================================================================================
//
//  ローカル関数（タッチパネル関連）
//
// ■GS変更点 09.02.26 : by hosaka genya
// ・処理の流れを全面的に改造
// ・ズームイン／アウトの「アソビ」を実装
//  (1SYNCのタッチ移動量が一定数以下の時のみ、ズームイン／アウトするようにした)
//============================================================================================
static void Earth_TouchPanel( EARTH_DEMO_WORK * wk )
{
  u32 ret;
  enum {
    ZOOM_LEN_LIMIT = 4, ///< ズームイン／アウトのアソビ
  };

  int dirx,lenx,diry,leny;
  int button_area = 0;  ///< グラフィックボタンの範囲

  // 初期化
  wk->tp_result = 0;
  
  //------------------------------------------------------------------------------
  // Touch Graphic Button Check
  //------------------------------------------------------------------------------
  ret = GFL_UI_TP_HitTrg( touch_tbl );
  if(ret==0){
    button_area = PAD_BUTTON_X;
  }else if(ret==1){
    button_area = PAD_BUTTON_B;
  }
/*
  if( (wk->tpx >= ((EARTH_ICON_WIN_PX-1) * DOTSIZE))&&
    (wk->tpx <= ((EARTH_ICON_WIN_PX+1 + EARTH_ICON_WIN_SX) * DOTSIZE))&&
    (wk->tpy >= ((EARTH_ICON_WIN_PY-1) * DOTSIZE))&&
    (wk->tpy <= ((EARTH_ICON_WIN_PY + EARTH_ICON_WIN_SY) * DOTSIZE))) {
    // 「やめる」ボタン
    button_area = PAD_BUTTON_B;
  } else if( (wk->tpx >= ((EARTH_LOOK_WIN_PX-1) * DOTSIZE))&&
    (wk->tpx <= ((EARTH_LOOK_WIN_PX+1 + EARTH_LOOK_WIN_SX) * DOTSIZE))&&
    (wk->tpy >= ((EARTH_LOOK_WIN_PY-1) * DOTSIZE))&&
    (wk->tpy <= ((EARTH_LOOK_WIN_PY + EARTH_LOOK_WIN_SY) * DOTSIZE))) {
    //「みる」ボタン
    button_area = PAD_BUTTON_X;
  }
*/
  //------------------------------------------------------------------------------
  // Touch Trg
  //------------------------------------------------------------------------------
  if(wk->tptrg){
    if( button_area ) {
      // 既に結果が出ていた = ボタン範囲
      // ボタンを押したことにして抜ける
      wk->tp_result = button_area;
      return;
    } else {
      // そのほかの場合、状況をリセット
      wk->tp_seq = 0;
      wk->tp_lenx = 0;
      wk->tp_leny = 0;
      wk->tp_count = 0;
      wk->tp_result = 0;
      //初回の検出位置を保存
      wk->tp_x = wk->tpx;
      wk->tp_y = wk->tpy;
      wk->tp_count = 4;
    }
  }
  
  //------------------------------------------------------------------------------
  // Touch Cont
  //------------------------------------------------------------------------------
  if(wk->tpcont){
    switch(wk->tp_seq){
    case 0:
      //最初のカウントはトリガー認識用に無視
      if(!wk->tp_count){
        wk->tp_seq++;
      }else{
        wk->tp_count--;
      }
    case 1:
      // ボタンの範囲だったら地球儀タッチ処理を飛ばす
      if( button_area ){ break; }

      Earth_TouchPanelParamGet(wk, wk->tp_x,wk->tp_y,&dirx,&lenx,&diry,&leny);
      wk->tp_result = dirx | diry;
      wk->tp_lenx = lenx;
      wk->tp_leny = leny;
      wk->tp_x = wk->tpx;
      wk->tp_y = wk->tpy;
      break;
    }
  //------------------------------------------------------------------------------
  // Touch None
  //------------------------------------------------------------------------------
  }else{
    if( !button_area && wk->tp_count && wk->tp_lenx < ZOOM_LEN_LIMIT && wk->tp_leny < ZOOM_LEN_LIMIT ) {
      // ズームイン／アウト
      wk->tp_result = PAD_BUTTON_A;
    }
    wk->tp_seq = 0;
    wk->tp_lenx = 0;
    wk->tp_leny = 0;
    wk->tp_count = 0;
  }
}

static void Earth_TouchPanelParamGet( EARTH_DEMO_WORK * wk,
  int prevx,int prevy,int* dirx_p,int* lenx_p,int* diry_p,int* leny_p )
{
  int x_dir = 0;
  int y_dir = 0;
  int x_len = 0;
  int y_len = 0;

  //Ｘ方向＆移動幅取得
  if(wk->tpx != 0xffff){
    x_len = wk->tpx - prevx;
    if(x_len < 0){
      x_len ^= -1;
      x_dir = PAD_KEY_RIGHT;
    }else{
      if(x_len > 0){
        x_dir = PAD_KEY_LEFT;
      }
    }
  }
  x_len &= 0x3f;  //リミッター
  *dirx_p = x_dir;
  *lenx_p = x_len;

  //Ｙ方向＆移動幅取得
  if(wk->tpy != 0xffff){
    y_len = wk->tpy - prevy;
    if(y_len < 0){
      y_len ^= -1;
      y_dir = PAD_KEY_DOWN;
    }else{
      if(y_len > 0){
        y_dir = PAD_KEY_UP;
      }
    }
  }
  y_len &= 0x3f;  //リミッター
  *diry_p = y_dir;
  *leny_p = y_len;
}


//============================================================================================
//
//  ローカル関数（２Ｄ関連）
//
//============================================================================================
//----------------------------------
//ＢＭＰＷＩＮ作成
//----------------------------------
static GFL_BMPWIN* _createBmpWin(const BMPWIN_DAT* wDat, BOOL makeScreen)
{
  GFL_BMPWIN* bmpwin;

  //ウインドウ作成
  bmpwin = GFL_BMPWIN_Create( wDat->frmNum, 
                              wDat->px, wDat->py, 
                              wDat->sx, wDat->sy, 
                              wDat->palIdx, GFL_BMP_CHRAREA_GET_B);
  //ウインドウクリア
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), FBMP_COL_WHITE );
  if(makeScreen == TRUE){
    GFL_BMPWIN_MakeScreen(bmpwin);
  }
  return bmpwin;
}

//----------------------------------
//ＢＧ設定
//----------------------------------
static void Earth_BGdataLoad( EARTH_DEMO_WORK * wk, ARCHANDLE* p_handle )
{
  //フォントハンドル作成
  wk->fontHandle = GFL_FONT_Create( ARCID_FONT,
                                    NARC_font_large_gftr,
                                    GFL_FONT_LOADTYPE_FILE,
                                    FALSE,
                                    wk->heapID);

  //文字色設定バックアップ
  GFL_FONTSYS_GetColor(&wk->printLSB.l, &wk->printLSB.s, &wk->printLSB.b);
    
  //文字色設定
  GFL_FONTSYS_SetColor(FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_WHITE);
  //--------サブＢＧ面-------------------------------------------
  //テキストＢＧ面コントロール設定
  GFL_BG_SetBGControl(EARTH_TEXT_PLANE,&Earth_Demo_BGtxt_header,GFL_BG_MODE_TEXT);
  GFL_BG_ClearScreen(EARTH_TEXT_PLANE);//テキストＢＧ面クリア

  //背景ＢＧ面コントロール設定
  GFL_BG_SetBGControl(EARTH_BACK_S_PLANE,&Earth_Demo_Back_header,GFL_BG_MODE_TEXT);

  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, 
                                        NARC_wifi_earth_earth_bg_NCGR,
                                        EARTH_BACK_S_PLANE,
                                        0,
                                        0,
                                        FALSE,
                                        wk->heapID);
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle,
                                    NARC_wifi_earth_earth_bg_NCLR,
                                    PALTYPE_SUB_BG,
                                    EARTH_BACK_PAL*PALSIZE,
                                    PALSIZE*4,
                                    wk->heapID);
  GFL_ARCHDL_UTIL_TransVramScreen(p_handle,
                                  NARC_wifi_earth_earth_bg_NSCR,
                                  EARTH_BACK_S_PLANE,
                                  0,
                                  0,
                                  FALSE,
                                  wk->heapID);
  //メッセージウインドウキャラ＆パレット読み込み（ウインドウ外側）
  BmpWinFrame_GraphicSet
    (EARTH_TEXT_PLANE, EARTH_TALKWINCHR_NUM, EARTH_TALKWIN_PAL, MENU_TYPE_SYSTEM, wk->heapID);
  BmpWinFrame_GraphicSet
    (EARTH_TEXT_PLANE, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL, MENU_TYPE_SYSTEM, wk->heapID);

  //フォントパレット読み込み
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, 
                                NARC_font_default_nclr,
                                PALTYPE_SUB_BG,
                                EARTH_SYSFONT_PAL*PALSIZE,
                                PALSIZE,
                                wk->heapID);

  //NULLキャラ＆パレット設定
  GFL_BG_SetClearCharacter( EARTH_TEXT_PLANE, 32, 0, wk->heapID );
  GFL_BG_SetBackGroundColor( EARTH_TEXT_PLANE,EARTH_NULL_PALETTE );

  //メッセージウインドウビットマップ作成（ウインドウ内側）
  wk->msgwin = _createBmpWin( &EarthMsgWinData, TRUE );
  BmpWinFrame_Write(wk->msgwin, WINDOW_TRANS_ON, EARTH_TALKWINCHR_NUM,EARTH_TALKWIN_PAL);
  GFL_BMPWIN_TransVramCharacter(wk->msgwin);

  //メッセージシステム初期化
  wk->msgseq = MSGSET;

  //--------メインＢＧ面------------------------------------------
  //アイコンＢＧ面コントロール設定
  GFL_BG_SetBGControl(EARTH_ICON_PLANE,&Earth_Demo_BGtxt_header,GFL_BG_MODE_TEXT);
  GFL_BG_ClearScreen(EARTH_ICON_PLANE);//アイコンＢＧ面クリア

  // ３Ｄ面プライオリティ設定
  GFL_BG_SetBGControl3D( 1 );     // ボタンよりは下に

  //背景ＢＧ面コントロール設定
  GFL_BG_SetBGControl(EARTH_BACK_M_PLANE,&Earth_Demo_Back_header,GFL_BG_MODE_TEXT);

  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, 
                                        NARC_wifi_earth_earth_bg_NCGR,
                                        EARTH_BACK_M_PLANE,
                                        0,
                                        0,
                                        FALSE,
                                        wk->heapID);
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle,
                                    NARC_wifi_earth_earth_bg_NCLR,
                                    PALTYPE_MAIN_BG,
                                    EARTH_BACK_PAL*PALSIZE,
                                    PALSIZE*4,
                                    wk->heapID);
  GFL_ARCHDL_UTIL_TransVramScreen(p_handle,
                                  NARC_wifi_earth_earth_bg_NSCR,
                                  EARTH_BACK_M_PLANE,
                                  0,
                                  0,
                                  FALSE,
                                  wk->heapID);
  //メニューウインドウキャラ＆パレット読み込み（ウインドウ外側）
  BmpWinFrame_GraphicSet
    (EARTH_ICON_PLANE, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL, 0, wk->heapID);

  //フォントパレット読み込み
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, 
                                NARC_font_default_nclr,
                                PALTYPE_MAIN_BG,
                                EARTH_SYSFONT_PAL*PALSIZE,
                                PALSIZE,
                                wk->heapID);
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, 
                                NARC_font_default_nclr,
                                PALTYPE_MAIN_BG,
                                EARTH_TOUCHFONT_PAL*PALSIZE,
                                PALSIZE,
                                wk->heapID);

  //NULLキャラ＆パレット設定
  GFL_BG_SetClearCharacter( EARTH_ICON_PLANE, 32, 0, wk->heapID );
  GFL_BG_SetBackGroundColor( EARTH_ICON_PLANE,0x0000 );
  {
    
#if 0
    STRBUF* temp_str;

    //メッセージバッファの生成
    temp_str= GFL_STR_CreateBuffer(16, wk->heapID);

    //メッセージウインドウビットマップ作成（ウインドウ内側）
    wk->iconwin = _createBmpWin( &EarthIconWinData, FALSE );
    //文字列の取得（やめる）
    GFL_MSG_GetString(wk->msg_man, mes_earth_03_03, temp_str);
    //文字列の表示
    Earth_StrPrint(wk, &wk->printUtilIcon, wk->iconwin, temp_str, 4, 0);  
    GFL_BMPWIN_TransVramCharacter(wk->iconwin);
    
    //メッセージウインドウビットマップ作成（ウインドウ内側）
    wk->lookwin = _createBmpWin( &EarthLookWinData, FALSE );
    //文字列の取得（みる）
    GFL_MSG_GetString(wk->msg_man, mes_earth_02_08, temp_str);
    //文字列の表示
    Earth_StrPrint(wk, &wk->printUtilLook, wk->lookwin, temp_str, 0, 0);
    GFL_BMPWIN_TransVramCharacter(wk->lookwin);

    //メッセージバッファの開放
    GFL_STR_DeleteBuffer(temp_str);

#endif
    
    // タスクメニューの登録
    Earth_TaskMenuInit( wk );
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief 画面下２つのボタンをTASKMENUシステムで描画
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void Earth_TaskMenuInit( EARTH_DEMO_WORK *wk )
{
  
  //文字列の取得（みる・やめる）
  wk->menustr[0] = GFL_MSG_CreateString(wk->msg_man, mes_earth_02_08);
  wk->menustr[1] = GFL_MSG_CreateString(wk->msg_man, mes_earth_03_03 );
  // タスクメンニューリソース読み込み
  wk->TaskMenuRes = APP_TASKMENU_RES_Create( EARTH_ICON_PLANE, 
                                             EARTH_TASKMENU_PAL, 
                                             wk->fontHandle, 
                                             wk->printQue, 
                                             wk->heapID );
  // 「みる」メニュー項目設定
  wk->menuitem[0].str      = wk->menustr[0];
  wk->menuitem[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  wk->menuitem[0].type     = APP_TASKMENU_WIN_TYPE_NORMAL;
  // 「もどる」
  wk->menuitem[1].str      = wk->menustr[1];
  wk->menuitem[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  wk->menuitem[1].type     = APP_TASKMENU_WIN_TYPE_RETURN;

}


static const int pos_tbl[][3] = {
  { MENU_BT0_X ,MENU_BT_Y, MENU_BT_W },
  { MENU_BT1_X ,MENU_BT_Y, MENU_BT_W },
};

//----------------------------------------------------------------------------------
/**
 * @brief タスクメニュー表示ON
 *
 * @param   wk    
 * @param   id    
 */
//----------------------------------------------------------------------------------
static void TaskMenuPut( EARTH_DEMO_WORK *wk, int id )
{
  // 登録                                             
  if(wk->TaskMenuWork[id]==NULL){
    wk->TaskMenuWork[id] =  APP_TASKMENU_WIN_Create( 
                                wk->TaskMenuRes, &wk->menuitem[id],  
                                pos_tbl[id][0], 
                                pos_tbl[id][1],
                                pos_tbl[id][2], wk->heapID );
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief タスクメニュー表示OFF
 *
 * @param   wk    
 * @param   id    
 */
//----------------------------------------------------------------------------------
static void TaskMenuOff( EARTH_DEMO_WORK *wk, int id )
{
  // 削除
  if(wk->TaskMenuWork[id]!=NULL){
    APP_TASKMENU_WIN_Delete( wk->TaskMenuWork[id] );
    GFL_BG_LoadScreenV_Req( EARTH_ICON_PLANE );
    wk->TaskMenuWork[id] = NULL;
  }
}



static void Earth_BGdataRelease( EARTH_DEMO_WORK * wk )
{
  // タスクメニューを解放
  TaskMenuOff( wk, TASKMENU_MIRU );
  TaskMenuOff( wk, TASKMENU_YAMERU );
  APP_TASKMENU_RES_Delete( wk->TaskMenuRes );
  GFL_STR_DeleteBuffer( wk->menustr[0]);
  GFL_STR_DeleteBuffer( wk->menustr[1]);
  wk->TaskMenuWork[0]   = NULL;
  wk->TaskMenuWork[1]   = NULL;

  // BMPWIN周りを解放
//  GFL_BMPWIN_Delete(wk->iconwin);
//  GFL_BMPWIN_Delete(wk->lookwin);
  GFL_BMPWIN_Delete(wk->msgwin);

  GFL_BG_FreeBGControl(EARTH_ICON_PLANE);
  GFL_BG_FreeBGControl(EARTH_TEXT_PLANE);
  GFL_BG_FreeBGControl(EARTH_BACK_M_PLANE);
  GFL_BG_FreeBGControl(EARTH_BACK_S_PLANE);

  GFL_FONT_Delete(wk->fontHandle);
  //文字色設定復帰
  GFL_FONTSYS_SetColor(wk->printLSB.l, wk->printLSB.s, wk->printLSB.b);
}

//----------------------------------
//文字列表示
//----------------------------------
static BOOL Earth_StrPrint
( EARTH_DEMO_WORK * wk, PRINT_UTIL *util, GFL_BMPWIN* bmpwin, const STRBUF* strbuf, u8 x, u8 y )
{
  //プリントキューハンドル作成
  PRINT_UTIL_Setup( util, bmpwin);

  //文字列の表示
  PRINT_UTIL_Print( util, wk->printQue, x, y, strbuf, wk->fontHandle);

  return TRUE;
}

//----------------------------------
//メッセージ表示
//----------------------------------
static BOOL Earth_MsgPrint( EARTH_DEMO_WORK * wk,u32 msgID,int button_mode )
{
  BOOL result = FALSE;

  switch(wk->msgseq){
  case MSGSET:
    //領域クリア
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );

    //文字列バッファの作成
    wk->msgstr = GFL_STR_CreateBuffer(EARTH_STRBUF_SIZE, wk->heapID); 

    //文字列の取得
    GFL_MSG_GetString(wk->msg_man, msgID, wk->msgstr);

    //文字列の表示
    wk->printStream = PRINTSYS_PrintStream( wk->msgwin,             // GFL_BMPWIN
                                            0, 0,                   // u16 x, y
                                            wk->msgstr,             // STRBUF*
                                            wk->fontHandle,         // GFL_FONT*
                                            MSGSPEED_GetWait(),     // int
                                            wk->tcbl,               // GFL_TCBLSYS*
                                            0,                      // u32 tcbpri
                                            wk->heapID,             // HEAPID
                                            FBMP_COL_WHITE );       // u16 clrCol
    wk->msgseq = MSGDRAW;
    break;

  case MSGDRAW:
    {
      PRINTSTREAM_STATE state = PRINTSYS_PrintStreamGetState( wk->printStream );
      APP_KEYCURSOR_Main( wk->printCursor, wk->printStream, wk->msgwin );
      switch(state){
      case PRINTSTREAM_STATE_RUNNING:///< 処理実行中（文字列が流れている）
        if(GFL_UI_KEY_GetCont()&(PAD_BUTTON_DECIDE)){
          PRINTSYS_PrintStreamShortWait( wk->printStream, 0 );
        }
        break;
      case PRINTSTREAM_STATE_PAUSE:  ///< 一時停止中（ページ切り替え待ち等）
        if(wk->trg & PAD_BUTTON_A || wk->trg & PAD_BUTTON_B){
          // 文字列送り処理
          PRINTSYS_PrintStreamReleasePause(wk->printStream);
          PMSND_PlaySE( WIFIEARTH_MOVE_CURSOR );
        }
        break;
      case PRINTSTREAM_STATE_DONE:   ///< 文字列終端まで表示完了
        wk->msgseq = MSGWAIT;
        break;
      }
    }
    break;

  case MSGWAIT:
    //終了
    if((button_mode != A_BUTTON_WAIT)||(wk->trg & PAD_BUTTON_A)){
      GFL_STR_DeleteBuffer(wk->msgstr);
      PRINTSYS_PrintStreamDelete(wk->printStream);

      wk->msgseq = MSGSET;
      result = TRUE;
    }
    break;
  }
  return result;
}

//----------------------------------
//リスト表示用コールバック
//----------------------------------
static void Earth_BmpListMoveSeCall(BMPMENULIST_WORK * wk,u32 param,u8 mode)
{
  if( mode == 0 ){//初期化時は鳴らさない
    PMSND_PlaySE( WIFIEARTH_MOVE_CURSOR );
  }
}

//----------------------------------
//リスト表示:BmpList設定参照
//----------------------------------
static void Earth_BmpListAdd( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                              const BMPMENULIST_HEADER* listheader,const EARTH_BMPLIST* list)
{
  BMPMENULIST_HEADER  listheader_tmp;
  int i;

  //メニュービットマップ追加
  wk->listwin = _createBmpWin( windata, TRUE );
  //メニューリスト用文字列バッファ作成
  wk->bmplistdata = BmpMenuWork_ListCreate(listheader->count,wk->heapID);
  //メニューリスト用文字列バッファ取得
  for( i=0; i<listheader->count; i++ ){
    BmpMenuWork_ListAddArchiveString
      (wk->bmplistdata, wk->msg_man, list[i].msgID, list[i].retID, wk->heapID);
  }
  //プリントUTILハンドル作成
  PRINT_UTIL_Setup(&wk->printUtil, wk->listwin);

  //メニュービットマップリストヘッダ作成
  listheader_tmp = *listheader;
  listheader_tmp.list = wk->bmplistdata;
  listheader_tmp.win  = wk->listwin;
  listheader_tmp.call_back = Earth_BmpListMoveSeCall;
  listheader_tmp.font_size_x = 12;
  listheader_tmp.font_size_y = GFL_FONT_GetLineHeight( wk->fontHandle );
  listheader_tmp.msgdata = wk->msg_man;
  listheader_tmp.print_util = &wk->printUtil;
  listheader_tmp.print_que = wk->printQue;
  listheader_tmp.font_handle = wk->fontHandle;

  //メニュービットマップリスト作成
  wk->bmplist = BmpMenuList_Set(&listheader_tmp, 0, 0, wk->heapID);
  BmpMenuList_SetCursorBmp(wk->bmplist, wk->heapID);

  //ウインドウ描画
  BmpWinFrame_Write(wk->listwin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL);
  GFL_BMPWIN_TransVramCharacter(wk->listwin);
}


static void _set_bmplist( EARTH_DEMO_WORK *wk, const BMPMENULIST_HEADER *listheader, int listcount )
{
  BMPMENULIST_HEADER  listheader_tmp;

  //プリントキューハンドル作成
  PRINT_UTIL_Setup(&wk->printUtil, wk->listwin);

  //メニュービットマップリストヘッダ作成
  listheader_tmp = *listheader;
  listheader_tmp.list = wk->bmplistdata;
  listheader_tmp.count = listcount; //1originのため補正
  listheader_tmp.win  = wk->listwin;
  listheader_tmp.call_back = Earth_BmpListMoveSeCall;
  listheader_tmp.font_size_x = 12;
  listheader_tmp.font_size_y = GFL_FONT_GetLineHeight( wk->fontHandle );
  listheader_tmp.print_util = &wk->printUtil;
  listheader_tmp.print_que = wk->printQue;
  listheader_tmp.font_handle = wk->fontHandle;

  //メニュービットマップリスト作成
  wk->bmplist = BmpMenuList_Set(&listheader_tmp, 0, 0, wk->heapID);
  BmpMenuList_SetCursorBmp(wk->bmplist, wk->heapID);

  //ウインドウ描画
  BmpWinFrame_Write(wk->listwin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL);
  GFL_BMPWIN_TransVramCharacter(wk->listwin);

}
//----------------------------------
//リスト表示２:gmmファイル一括、リスト選択返り値はリストの順番と同じ(1orgin)
//----------------------------------
static void Earth_BmpListAddGmmAll( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                                    const BMPMENULIST_HEADER* listheader,u32 listarcID)
{
  GFL_MSGDATA* msg_man;
  u32 listcount;
  int i;

  //メニュービットマップ追加
  wk->listwin = _createBmpWin( windata, TRUE );
  //メッセージマネージャ作成
  msg_man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, listarcID, wk->heapID);
  //メッセージ総数取得
  listcount = GFL_MSG_GetStrCount(msg_man);
  //メニューリスト用文字列バッファ作成
  wk->bmplistdata = BmpMenuWork_ListCreate(listcount,wk->heapID);

  //メニューリスト用文字列バッファ取得
  for( i=1; i<listcount; i++ ){ //1オリジンのため
    BmpMenuWork_ListAddArchiveString(wk->bmplistdata, msg_man, i, i, wk->heapID);
  }
  //メッセージデータ破棄
  GFL_MSG_Delete(msg_man);

  _set_bmplist(wk, listheader, listcount-1);
}


#include "sort_list.cdat"   // EnableCountryList

//----------------------------------
//リスト表示３:gmmファイルから登録可能国だけリストに入れる
//----------------------------------
static void Earth_BmpListAddGmmEnableList( EARTH_DEMO_WORK * wk, const BMPWIN_DAT* windata,
                                    const BMPMENULIST_HEADER* listheader,u32 listarcID)
{
  GFL_MSGDATA* msg_man;
  int i;

  //メニュービットマップ追加
  wk->listwin = _createBmpWin( windata, TRUE );
  //メッセージマネージャ作成
  msg_man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, listarcID, wk->heapID);

  //メニューリスト用文字列バッファ作成
  wk->bmplistdata = BmpMenuWork_ListCreate(COUNTRY_ENABLE_MAX,wk->heapID);

  //メニューリスト用文字列バッファ取得
  for( i=0; i<COUNTRY_ENABLE_MAX; i++ ){ //1オリジンのため
    BmpMenuWork_ListAddArchiveString(wk->bmplistdata, msg_man, 
                                     EnableCountryList[i], 
                                     EnableCountryList[i], wk->heapID);
  }
  //メッセージデータ破棄
  GFL_MSG_Delete(msg_man);

  _set_bmplist(wk, listheader, COUNTRY_ENABLE_MAX);
}

//----------------------------------
//リスト削除
//----------------------------------
static void Earth_BmpListDel( EARTH_DEMO_WORK* wk )
{
  //選択リスト削除処理
  BmpWinFrame_Clear( wk->listwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_Delete(wk->listwin);

  BmpMenuList_Exit(wk->bmplist,NULL,NULL);
  BmpMenuWork_ListDelete(wk->bmplistdata);

}

//----------------------------------
//登録地名表示ウインドウ
//----------------------------------
static void Earth_MyPlaceInfoWinSet( EARTH_DEMO_WORK* wk )
{
  STRBUF* msgstr = GFL_STR_CreateBuffer(EARTH_STRBUF_SIZE, wk->heapID); 
  STRBUF* msgtmp = GFL_STR_CreateBuffer(EARTH_STRBUF_SIZE, wk->heapID); 

  wk->infowin = _createBmpWin(&EarthInfoWinData, TRUE );

  WORDSET_RegisterCountryName( wk->wordset, 0, wk->my_nation );
  WORDSET_RegisterLocalPlaceName( wk->wordset, 1, wk->my_nation, wk->my_area );

  //文字列の取得
  GFL_MSG_GetString(wk->msg_man, mes_earth_03_01, msgtmp);
  WORDSET_ExpandStr(wk->wordset, msgstr, msgtmp);

  //文字列の表示
  Earth_StrPrint(wk, &wk->printUtilInfo, wk->infowin, msgstr, 0, 0);

  GFL_STR_DeleteBuffer( msgtmp );
  GFL_STR_DeleteBuffer( msgstr );

  BmpWinFrame_Write(wk->infowin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL);
  GFL_BMPWIN_TransVramCharacter(wk->infowin);
}

static void Earth_MyPlaceInfoWinSet2( EARTH_DEMO_WORK* wk, int nation, int area )
{
  STRBUF* str1 = GFL_STR_CreateBuffer( EARTH_NAME_SIZE, wk->heapID );
  STRBUF* str2 = GFL_STR_CreateBuffer( EARTH_NAME_SIZE, wk->heapID );

  wk->infowin = _createBmpWin(&EarthInfoWinData, TRUE );

  WIFI_NationAreaNameGet(nation, area, str1, str2, wk->heapID);
  if( area != 0 ){
    Earth_StrPrint(wk, &wk->printUtilInfo, wk->infowin, str2, 0, 16);
  }
  Earth_StrPrint(wk, &wk->printUtilInfo, wk->infowin, str1, 0, 0);
  GFL_STR_DeleteBuffer(str2);
  GFL_STR_DeleteBuffer(str1);

  BmpWinFrame_Write(wk->infowin, WINDOW_TRANS_ON, EARTH_MENUWINCHR_NUM, EARTH_MENUWIN_PAL);
//  GFL_BMPWIN_TransVramCharacter( wk->infowin );
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->infowin );
}

static void Earth_MyPlaceInfoWinRelease( EARTH_DEMO_WORK* wk )
{
  BmpWinFrame_Clear( wk->infowin, WINDOW_TRANS_ON );
  GFL_BMPWIN_Delete(wk->infowin);
}

// 一番近い地点を表示するように変更 tomoya 08.04.03
//-----------------------------------------------------------------------------
/**
 *  @brief  一番近い地点を検索
 *
 *  @param  EARTH_DEMO_WORK* wk 
 *
 *  @retval 見つかったか否か
 *
 *  @note tomoyaさんが 08.04.03に作ったものをhosakaが関数化 09.03.19
 */
//-----------------------------------------------------------------------------
static BOOL Earth_SearchPosInfo( EARTH_DEMO_WORK* wk, u32* outIndex )
{  
  int   i;
  BOOL  search_result = FALSE;
  s16   minx = (s16)(wk->rotate.x - 0x80);
  s16   maxx = (s16)(wk->rotate.x + 0x80);
  s32   miny = (s16)(wk->rotate.y - 0x80);
  s32   maxy = (s16)(wk->rotate.y + 0x80);
  s32   miny2;
  s32   maxy2;
  u32   mindist   = 0x80*2;
  u32   minindex  = wk->placelist.listcount;
  u32   dist;
  Vec2DS32 rot_a, rot_b;

  rot_a.x = wk->rotate.x;
  rot_a.y = wk->rotate.y;
  WIFI_MinusRotateChange( &rot_a );

  // Yは途中で符号が変わるので２パターンの範囲を持つ
  if( MATH_ABS( maxy - miny ) > (0x80*2) )
  {
    miny2 = (s16)wk->rotate.y;
    maxy2 = (s16)wk->rotate.y;
    if( miny2 > 0 )
    {
      miny = -0x10000 + miny2;
      maxy = -0x10000 + maxy2;
    }
    else
    {
      miny = miny2 + 0x10000;
      maxy = maxy2 + 0x10000;
    }
    
    miny2 -= 0x80;
    maxy2 += 0x80;
    miny  -= 0x80;
    maxy  += 0x80;
  }
  else
  {
    miny2 = miny;
    maxy2 = maxy;
  }

//  OS_TPrintf( "minx=%d maxx=%d miny=%d maxy=%d miny2=%d maxy2=%d\n", 
//                minx, maxx, miny, maxy, miny2, maxy2 );

  for(i=0;i<wk->placelist.listcount;i++){
    if( (wk->placelist.place[i].x > minx)&&(wk->placelist.place[i].x < maxx) && 
      ( (wk->placelist.place[i].y > miny)&&(wk->placelist.place[i].y < maxy) || (wk->placelist.place[i].y > miny2)&&(wk->placelist.place[i].y < maxy2) ) &&
       (wk->placelist.place[i].col != MARK_NULL) ){

      rot_b.x = wk->placelist.place[i].x;
      rot_b.y = wk->placelist.place[i].y;
      WIFI_MinusRotateChange( &rot_b );

      dist =WIFI_EarthGetRotateDist( &rot_a, &rot_b );
      if( dist < mindist ){
        mindist   = dist;
        minindex  = i;
      }
    }
  }
  
  *outIndex = minindex;

  if( minindex != wk->placelist.listcount ){
    search_result = TRUE;
  }

  return search_result;
}


//----------------------------------
//地名表示
//----------------------------------
static void Earth_PosInfoPut( EARTH_DEMO_WORK* wk )
{
  if( wk->info_mode == 0 ){
    {
      STRBUF* msgstr = GFL_STR_CreateBuffer(EARTH_STRBUF_SIZE, wk->heapID); 

      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );
      GFL_MSG_GetString(wk->msg_man, mes_earth_03_02, msgstr);
      Earth_StrPrint(wk, &wk->printUtilMsg, wk->msgwin, msgstr, 0, 0);
      GFL_STR_DeleteBuffer( msgstr );
    }
  } else {
    u32 minindex;

    //カーソル位置地名表示  
    if( Earth_SearchPosInfo( wk, &minindex ) ) {
      STRBUF* str1 = GFL_STR_CreateBuffer(EARTH_NAME_SIZE, wk->heapID );
      STRBUF* str2 = GFL_STR_CreateBuffer(EARTH_NAME_SIZE, wk->heapID );

      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->msgwin), FBMP_COL_WHITE );
      WIFI_NationAreaNameGet( wk->placelist.place[minindex].nationID,
                  wk->placelist.place[minindex].areaID,
                  str1,str2,wk->heapID);
      if( wk->placelist.place[minindex].areaID != 0 ){
        Earth_StrPrint(wk, &wk->printUtilMsg, wk->msgwin, str2, 0, 16);
      }
      Earth_StrPrint(wk, &wk->printUtilMsg, wk->msgwin, str1, 0, 0);
      GFL_STR_DeleteBuffer(str2);
      GFL_STR_DeleteBuffer(str1);

      // さしている位置を地域の位置にする
      wk->rotate.x = wk->placelist.place[minindex].x;
      wk->rotate.y = wk->placelist.place[minindex].y;
    }else{
      wk->info_mode = 0;
    }
  }
  GFL_BMPWIN_TransVramCharacter(wk->msgwin);
}

//============================================================================================
//
//  ローカル関数（３Ｄ関連）
//
//============================================================================================
typedef struct {
  u16 idx;
  u16 datID;
}MARK_DATA;
static const MARK_DATA mark_array_data[] = {
  { MARK_RED,     NARC_wifi_earth_earth_mark_r_nsbmd },
  { MARK_GREEN,   NARC_wifi_earth_earth_mark_g_nsbmd },
  { MARK_BLUE,    NARC_wifi_earth_earth_mark_b_nsbmd },
  { MARK_YELLOW,  NARC_wifi_earth_earth_mark_y_nsbmd },
};
//----------------------------------
//３Ｄデータロード関数＆３Ｄデータ開放関数
//----------------------------------
static void Earth_ModelLoad( EARTH_DEMO_WORK * wk, ARCHANDLE* p_handle )
{
  u32 arcID = ARCID_WIFI_EARTH;
  int i;

  //地球モデル
  wk->g3DresEarth = GFL_G3D_CreateResourceArc(arcID, NARC_wifi_earth_wifi_earth_nsbmd);
  GFL_G3D_TransVramTexture( wk->g3DresEarth );
  wk->g3DrndEarth = GFL_G3D_RENDER_Create(wk->g3DresEarth, 0, wk->g3DresEarth); 
  wk->g3DobjEarth = GFL_G3D_OBJECT_Create(wk->g3DrndEarth, NULL, 0); 

  //地点マークモデル
  for(i=0; i<NELEMS(mark_array_data); i++){
    u16 p = mark_array_data[i].idx;
    wk->g3DresMark[p] = GFL_G3D_CreateResourceArc(arcID, mark_array_data[i].datID);
    GFL_G3D_TransVramTexture( wk->g3DresMark[p] );
    wk->g3DrndMark[p] = GFL_G3D_RENDER_Create(wk->g3DresMark[p], 0, wk->g3DresMark[p]); 
    wk->g3DobjMark[p] = GFL_G3D_OBJECT_Create(wk->g3DrndMark[p], NULL, 0); 
  }

  // カメラ・ライトセットアップ
  EarthCameraInit( wk );
  EarthLightInit( wk );
}

static void Earth_ModelRelease( EARTH_DEMO_WORK * wk )
{
  int i;

  GFL_G3D_CAMERA_Delete( wk->g3Dcamera );
  GFL_G3D_LIGHT_Delete( wk->g3Dlightset );

  GFL_G3D_OBJECT_Delete( wk->g3DobjEarth ); 
  GFL_G3D_RENDER_Delete( wk->g3DrndEarth ); 
  GFL_G3D_FreeVramTexture( wk->g3DresEarth );
  GFL_G3D_DeleteResource( wk->g3DresEarth );

  for(i=0; i<NELEMS(mark_array_data); i++){
    u16 p = mark_array_data[i].idx;
    GFL_G3D_OBJECT_Delete( wk->g3DobjMark[p] ); 
    GFL_G3D_RENDER_Delete( wk->g3DrndMark[p] ); 
    GFL_G3D_FreeVramTexture( wk->g3DresMark[p] );
    GFL_G3D_DeleteResource( wk->g3DresMark[p] );
  }
  
  wk->g3DobjEarth = NULL;
}

//----------------------------------
//モデル関連データ初期化（地点データロードあり）
//----------------------------------
static void EarthDataInit( EARTH_DEMO_WORK * wk )
{
  {//地球の座標初期化（地点マークと共有）
    
    wk->trans.x = INIT_EARTH_TRANS_XVAL; 
    wk->trans.y = INIT_EARTH_TRANS_YVAL; 
    wk->trans.z = INIT_EARTH_TRANS_ZVAL; 
  }
  {//地球のスケール初期化
    wk->scale.x = INIT_EARTH_SCALE_XVAL;
    wk->scale.y = INIT_EARTH_SCALE_YVAL;
    wk->scale.z = INIT_EARTH_SCALE_ZVAL;
  }
  {//地球の回転初期化
    wk->rotate.x  = INIT_EARTH_ROTATE_XVAL;
    wk->rotate.y  = INIT_EARTH_ROTATE_YVAL;
    wk->rotate.z  = INIT_EARTH_ROTATE_ZVAL;
  }
  {//地点マークスケール初期化
    wk->mark_scale.x  = INIT_EARTH_SCALE_XVAL;
    wk->mark_scale.y  = INIT_EARTH_SCALE_YVAL;
    wk->mark_scale.z  = INIT_EARTH_SCALE_ZVAL;
  }
}

//----------------------------------
//カメラ初期化
//----------------------------------
static const VecFx32  target_pos = { 
  INIT_CAMERA_TARGET_XVAL, INIT_CAMERA_TARGET_YVAL, INIT_CAMERA_TARGET_ZVAL 
};
static const VecFx32  camera_pos = {
  INIT_CAMERA_POS_XVAL, INIT_CAMERA_POS_YVAL, INIT_CAMERA_POS_ZVAL 
};

static void EarthCameraInit( EARTH_DEMO_WORK * wk )
{
  wk->g3Dcamera = GFL_G3D_CAMERA_CreateDefault( &camera_pos, &target_pos, wk->heapID );
  GFL_G3D_CAMERA_Switching(wk->g3Dcamera);
}

static void EarthCameraStart( EARTH_DEMO_WORK * wk )
{
  fx32 near = INIT_CAMERA_CLIP_NEAR;
  fx32 far = INIT_CAMERA_CLIP_FAR;

  GFL_G3D_CAMERA_SetPos( wk->g3Dcamera, &camera_pos );
  GFL_G3D_CAMERA_SetTarget( wk->g3Dcamera, &target_pos );
  GFL_G3D_CAMERA_SetfovySin( wk->g3Dcamera, FX_SinIdx( INIT_CAMERA_PERSPWAY ) );
  GFL_G3D_CAMERA_SetfovyCos( wk->g3Dcamera, FX_CosIdx( INIT_CAMERA_PERSPWAY ) );
  GFL_G3D_CAMERA_SetNear( wk->g3Dcamera, &near );
  GFL_G3D_CAMERA_SetFar( wk->g3Dcamera, &far );

  if(wk->earth_mode == JAPAN_MODE){
    //カメラ距離フラグ初期化（開始時は近距離）
    wk->camera_status = CAMERA_NEAR;
  }else{
    //カメラ距離フラグ初期化（開始時は遠距離）
    wk->camera_status = CAMERA_FAR;
  }
  while(1){
    //カメラ設定
    if(Earth3D_CameraMoveNearFar(wk) == TRUE){
      break;
    }
  }
}

//----------------------------------
//ライト初期化
//----------------------------------
static const GFL_G3D_LIGHT_DATA lightTbl[] = {
  { USE_LIGHT_NUM, 
    {{INIT_LIGHT_ANGLE_XVAL, INIT_LIGHT_ANGLE_YVAL, INIT_LIGHT_ANGLE_ZVAL}, GX_RGB(31,31,31) } 
  },
};
static const GFL_G3D_LIGHTSET_SETUP lightSetup = { lightTbl, NELEMS(lightTbl) };

static void EarthLightInit( EARTH_DEMO_WORK * wk )
{
  wk->g3Dlightset = GFL_G3D_LIGHT_Create( &lightSetup, wk->heapID );
  GFL_G3D_LIGHT_Switching(wk->g3Dlightset);
}

//----------------------------------
//３Ｄ回転計算
//----------------------------------
static void  EarthVecFx32_to_MtxFx33( MtxFx33* dst, VecFx32* src )
{
  MtxFx33 tmp;

  MTX_RotY33( dst,FX_SinIdx((u16)src->y),FX_CosIdx((u16)src->y));

  MTX_RotX33( &tmp,FX_SinIdx((u16)src->x),FX_CosIdx((u16)src->x));
  MTX_Concat33(dst,&tmp,dst);

  MTX_RotZ33( &tmp,FX_SinIdx((u16)src->z),FX_CosIdx((u16)src->z));
  MTX_Concat33(dst,&tmp,dst);
}

//----------------------------------
//３Ｄ回転計算(相対)
//----------------------------------
static void  EarthVecFx32_to_MtxFx33_place( MtxFx33* dst, VecFx32* src )
{
  MtxFx33 tmp;

  MTX_RotY33( dst,FX_SinIdx((u16)src->x),FX_CosIdx((u16)src->x));

  MTX_RotX33( &tmp,FX_SinIdx((u16)-src->y),FX_CosIdx((u16)-src->y));
  MTX_Concat33(dst,&tmp,dst);

  MTX_RotZ33( &tmp,FX_CosIdx((u16)src->z),FX_SinIdx((u16)src->z));
  MTX_Concat33(dst,&tmp,dst);
}



//----------------------------------
//地球操作関数
//----------------------------------
static BOOL Earth3D_Control( EARTH_DEMO_WORK * wk,int keytrg,int keycont )
{
  u16 rotate_speed_x;
  u16 rotate_speed_y;
  s16 rotate_x;
  s16 rotate_y;
  BOOL result = FALSE;

  rotate_x = wk->rotate.x;
  rotate_y = wk->rotate.y;

  

  //カメラ遠近移動判定（世界地球儀モードのみ）
  if((keytrg & PAD_BUTTON_A)||(wk->tp_result & PAD_BUTTON_A)){
    if(wk->earth_mode == GLOBAL_MODE){
      if(wk->camera_status == CAMERA_FAR){
        wk->camera_status = CAMERA_NEAR;
      }else{
        wk->camera_status = CAMERA_FAR;
      }
    }
    result = TRUE;
    return result;
  }
  //カメラ移動スピード初期設定
  // カメラ移動スピードを少し早く変更 tomoya 08.04.03
  if(wk->camera_status == CAMERA_FAR){
    //遠距離
    if((wk->tp_lenx)||(wk->tp_leny)){ //タッチパネル入力あり
      rotate_speed_x = CAMERA_MOVE_SPEED_FAR/6 * wk->tp_lenx;
      rotate_speed_y = CAMERA_MOVE_SPEED_FAR/6 * wk->tp_leny;
    }else{
      rotate_speed_x = CAMERA_MOVE_SPEED_FAR;
      rotate_speed_y = CAMERA_MOVE_SPEED_FAR;
    }
  }else{
    //近距離
    if((wk->tp_lenx)||(wk->tp_leny)){ //タッチパネル入力あり
      rotate_speed_x = CAMERA_MOVE_SPEED_NEAR/3 * wk->tp_lenx;
      rotate_speed_y = CAMERA_MOVE_SPEED_NEAR/3 * wk->tp_leny;
    }else{
      rotate_speed_x = CAMERA_MOVE_SPEED_NEAR;
      rotate_speed_y = CAMERA_MOVE_SPEED_NEAR;
    }
  }
  //カメラ上下左右移動判定
  if((keycont & PAD_KEY_LEFT)||(wk->tp_result & PAD_KEY_LEFT)){
    if(wk->earth_mode == GLOBAL_MODE){
      wk->rotate.y += rotate_speed_x;
    }else{
      //if((u16)wk->rotate.y < EARTH_LIMIT_ROTATE_YMAX){
      if(rotate_y < (s16)EARTH_LIMIT_ROTATE_YMAX){
        wk->rotate.y += rotate_speed_x;
      }
    }
    result = TRUE;
  }
  if((keycont & PAD_KEY_RIGHT)||(wk->tp_result & PAD_KEY_RIGHT)){
    if(wk->earth_mode == GLOBAL_MODE){
      wk->rotate.y -= rotate_speed_x;
    }else{
      //if((u16)wk->rotate.y > EARTH_LIMIT_ROTATE_YMIN){
      if(rotate_y > (s16)EARTH_LIMIT_ROTATE_YMIN){
        wk->rotate.y -= rotate_speed_x;
      }
    }
    result = TRUE;
  }
  if((keycont & PAD_KEY_UP)||(wk->tp_result & PAD_KEY_UP)){
    if(wk->earth_mode == GLOBAL_MODE){
      //if(((u16)(wk->rotate.x + rotate_speed_y)) < CAMERA_ANGLE_MAX){
      if((rotate_x + rotate_speed_y) < CAMERA_ANGLE_MAX){
        wk->rotate.x += rotate_speed_y;
      }else{
        wk->rotate.x = CAMERA_ANGLE_MAX;
      }
    }else{
      //if((u16)wk->rotate.x < EARTH_LIMIT_ROTATE_XMAX){
      if(rotate_x < (s16)EARTH_LIMIT_ROTATE_XMAX){
        wk->rotate.x += rotate_speed_y;
      }
    }
    result = TRUE;
  }
  if((keycont & PAD_KEY_DOWN)||(wk->tp_result & PAD_KEY_DOWN)){
    if(wk->earth_mode == GLOBAL_MODE){
      //if(((u16)(wk->rotate.x - rotate_speed_y)) > CAMERA_ANGLE_MIN){
      if((rotate_x - rotate_speed_y) > CAMERA_ANGLE_MIN){
        wk->rotate.x -= rotate_speed_y;
      }else{
        wk->rotate.x = CAMERA_ANGLE_MIN;
      }
    }else{
      //if((u16)wk->rotate.x > EARTH_LIMIT_ROTATE_XMIN){
      if(rotate_x > (s16)EARTH_LIMIT_ROTATE_XMIN){
        wk->rotate.x -= rotate_speed_y;
      }
    }
    result = TRUE;
  }

#ifdef PM_DEBUG
  if(result){
    OS_TPrintf("rot_x=0x%x, rot_y=0x%x\n", wk->rotate.x, wk->rotate.y);
  }
#endif
  return result;
}

//----------------------------------
//カメラ遠近移動関数
//----------------------------------
static BOOL Earth3D_CameraMoveNearFar( EARTH_DEMO_WORK * wk )
{
  VecFx32 camPos, camTarget, vec;
  fx32 distance;
  BOOL result = FALSE;

  GFL_G3D_CAMERA_GetPos( wk->g3Dcamera, &camPos );
  GFL_G3D_CAMERA_GetTarget( wk->g3Dcamera, &camTarget );
  VEC_Subtract(&camPos, &camTarget, &vec);
  distance = VEC_Mag(&vec);   // 距離取得
  VEC_Normalize(&vec, &vec);  // 方向単位ベクトル取得

  switch(wk->camera_status){

  case CAMERA_NEAR://近づく
    if(distance > (INIT_CAMERA_DISTANCE_NEAR + CAMERA_INOUT_SPEED)){
      distance -= CAMERA_INOUT_SPEED;
      wk->mark_scale.x -= MARK_SCALE_INCDEC;
      wk->mark_scale.y = wk->mark_scale.x;
    }else{
      distance = INIT_CAMERA_DISTANCE_NEAR;
      result = TRUE;
    }
    break;

  case CAMERA_FAR://遠ざかる
    if(distance < (INIT_CAMERA_DISTANCE_FAR - CAMERA_INOUT_SPEED)){
      distance += CAMERA_INOUT_SPEED;
      wk->mark_scale.x += MARK_SCALE_INCDEC;
      wk->mark_scale.y = wk->mark_scale.x;
    }else{
      distance = INIT_CAMERA_DISTANCE_FAR;
      result = TRUE;
    }
    break;
  }
  VEC_MultAdd(distance, &vec, &camTarget, &camPos); 
  GFL_G3D_CAMERA_SetPos( wk->g3Dcamera, &camPos );

  return result;
}

//----------------------------------
//３Ｄ描画関数
//----------------------------------
static void Earth3D_Draw( EARTH_DEMO_WORK * wk )
{
  MtxFx33 rotate_world = {FX32_ONE,0,0,0,FX32_ONE,0,0,0,FX32_ONE};

  //描画フラグ判定
  switch(wk->Draw3Dsw){

  case DRAW3D_DISABLE:
    break;

  case DRAW3D_BANISH:
    GFL_G3D_DRAW_Start();
    GFL_G3D_DRAW_End();
    wk->Draw3Dsw = DRAW3D_DISABLE;
    break;

  case DRAW3D_ENABLE:
    GFL_G3D_LIGHT_Switching(wk->g3Dlightset);
    GFL_G3D_CAMERA_Switching(wk->g3Dcamera);

    GFL_G3D_DRAW_Start();
    GFL_G3D_DRAW_SetLookAt();
    {
      GFL_G3D_OBJSTATUS statusEarth;
      statusEarth.trans = wk->trans;
      MTX_Identity33( &statusEarth.rotate );

      //中心マーク描画（基準点のため回転なし）
      statusEarth.scale = wk->mark_scale;
      GFL_G3D_DRAW_DrawObject( wk->g3DobjMark[MARK_GREEN], &statusEarth);
      
      //地球描画
      statusEarth.scale = wk->scale;
      EarthVecFx32_to_MtxFx33(&statusEarth.rotate, &wk->rotate); //グローバル回転行列取得
      GFL_G3D_DRAW_DrawObject( wk->g3DobjEarth, &statusEarth);
  
      //地点マーク描画
      {
        GFL_G3D_OBJSTATUS statusMark;
        int i;

        statusMark.trans = wk->trans;
        statusMark.scale = wk->mark_scale;

        for(i=0;i<wk->placelist.listcount;i++){
          MTX_Concat33(&wk->placelist.place[i].rotate, &statusEarth.rotate, &statusMark.rotate);
  
          if(wk->placelist.place[i].col != MARK_NULL){
            GFL_G3D_DRAW_DrawObject( wk->g3DobjMark[wk->placelist.place[i].col], &statusMark);
          }
        }
      }
    }
    GFL_G3D_DRAW_End();
    break;
  }
}
  
//============================================================================================
//
//  グローバル関数（地域データ取得関連）
//
//============================================================================================
//============================================================================================
/**
 *  地域データ初期化（セーブワークの初期化）
 *
 * @param 
 * @retval
 */
//============================================================================================
void  WIFI_RegistratonInit( GAMEDATA* gamedata )
{
  SAVE_CONTROL_WORK*  scw = GAMEDATA_GetSaveControlWork( gamedata );
  MYSTATUS*           mystatus = GAMEDATA_GetMyStatus( gamedata );
  WIFI_HISTORY*       wh = SaveData_GetWifiHistory( scw );

  WIFIHISTORY_SetMyNationArea(wh, mystatus, 0, 0);
  WIFIHISTORY_SetWorldFlag(wh, FALSE);
}

//============================================================================================
/**
 *  国名取得
 *
 * @param[in] nationID  国ＩＤ
 * @param[in] areaID    地域ＩＤ
 * @param[in] nation_str  国名格納バッファポインタ
 * @param[in] area_str  地域名格納バッファポインタ
 * @param[in] heapID    テンポラリヒープＩＤ
 *
 * @retval  FALSE:地域がない国　TRUE:地域がある国
 */
//============================================================================================
BOOL  WIFI_NationAreaNameGet
      (int nationID,int areaID, STRBUF* nation_str,STRBUF* area_str,int heapID)
{
  GFL_MSGDATA*  msg_man;
  int areaIndex = EarthAreaTableGet(nationID);  //地域データテーブルＩＮＤＥＸ取得
  int datID;
  BOOL result;

  datID = NARC_message_wifi_place_msg_world_dat;
  msg_man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, datID, heapID);
  GFL_MSG_GetString(msg_man, nationID, nation_str);//国名取得
  GFL_MSG_Delete(msg_man);

  if(areaIndex == 0){
    //地域が存在しない国の場合「なし」という文字列を一応セット
    areaIndex = 1;
    areaID = 0;
    result = FALSE;
  }else{
    result = TRUE;
  }

  datID = WIFI_COUNTRY_DataIndexToPlaceMsgDataID(areaIndex);
  msg_man = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, datID, heapID); 
  GFL_MSG_GetString(msg_man, areaID, area_str);//地域名取得
  GFL_MSG_Delete(msg_man);

  return result;
}

//============================================================================================
/**
 *  ローカルエリア存在チェック
 *
 * @param[in] nationID  国ＩＤ
 *
 * @retval  FALSE:地域なし　TRUE:地域あり
 */
//============================================================================================
BOOL  WIFI_LocalAreaExistCheck(int nationID)
{
  if(EarthAreaTableGet(nationID)){
    return TRUE;
  }else{
    return FALSE;
  }
}




//----------------------------------------------------------------------------
/**
 *  @brief  マイナスの値の回転角度や３６０度以上の回転角度をなくす
 *
 *  @param  p_rot 回転ベクトル
 */
//-----------------------------------------------------------------------------
static void WIFI_MinusRotateChange( Vec2DS32* p_rot )
{
  if( p_rot->x >= 0 ){
    p_rot->x = p_rot->x % 0xffff;
  }else{
    p_rot->x = p_rot->x + ( 0xffff* ((MATH_ABS( p_rot->x )/0xffff)+1) );
  }
  if( p_rot->y >= 0 ){
    p_rot->y = p_rot->y % 0xffff;
  }else{
    p_rot->y = p_rot->y + ( 0xffff* ((MATH_ABS( p_rot->y )/0xffff)+1) );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  回転ベクトルの距離を求める
 *
 *  @param  cp_earth    地球儀の回転ベクトル
 *  @param  cp_place    地域の回転ベクトル
 *
 *  @return 距離（u32単位）
 */
//-----------------------------------------------------------------------------
static u32 WIFI_EarthGetRotateDist( const Vec2DS32* cp_earth, const Vec2DS32* cp_place )
{
  s32 dif_x, dif_y;
  u32 dist;

  // 距離が近いほうを選ぶ
  dif_x = MATH_ABS(cp_earth->x - cp_place->x);
  dif_y = MATH_ABS(cp_earth->y - cp_place->y);

  if( dif_x > GFL_CALC_RotKey(180) ){
    dif_x = 0xffff - dif_x;
  }
  if( dif_y > GFL_CALC_RotKey(180) ){
    dif_y = 0xffff - dif_y;
  }
  dist = FX_Sqrt( ((dif_x*dif_x)+(dif_y*dif_y)) << FX32_SHIFT ) >> FX32_SHIFT;

  return dist;
}


//----------------------------------
// デバッグ
//----------------------------------
#ifdef WIFI_ERATH_DEBUG
static void EarthDebugWinRotateInfoWrite( EARTH_DEMO_WORK * wk )
    
{
  STRBUF* str1 = GFL_STR_CreateBuffer(10, HEAPID_BASE_DEBUG);
  STRBUF* str2 = GFL_STR_CreateBuffer(10, HEAPID_BASE_DEBUG);

  STRBUF_SetHexNumber( str1, (u16)wk->rotate.x, 4, 
            NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
  STRBUF_SetHexNumber( str2, (u16)wk->rotate.y, 4, 
            NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
  Earth_StrPrint(wk, &wk->printUtilMsg, wk->msgwin, str1, 0, 0);
  Earth_StrPrint(wk, &wk->printUtilMsg, wk->msgwin, str2, 0, 16);

  GFL_STR_DeleteBuffer(str2);
  GFL_STR_DeleteBuffer(str1);
}

static void EarthDebugWinNameInfoWrite( EARTH_DEMO_WORK * wk )
    
{
  STRBUF  *str1,*str2;
  int i;

  if(!wk->debug_work[0]){
    for(i=0;i<wk->placelist.listcount;i++){
      if((wk->placelist.place[i].x == (s16)wk->rotate.x)
        &&(wk->placelist.place[i].y == (s16)wk->rotate.y)){
  
        str1 = GFL_STR_CreateBuffer(EARTH_NAME_SIZE, HEAPID_BASE_DEBUG);
        str2 = GFL_STR_CreateBuffer(EARTH_NAME_SIZE, HEAPID_BASE_DEBUG);

        WIFI_NationAreaNameGet( wk->placelist.place[i].nationID,
                    wk->placelist.place[i].areaID,
                    str1,str2,wk->heapID);
        Earth_StrPrint(wk, &wk->printUtilMsg, wk->msgwin, str1, 16*4, 0);
        Earth_StrPrint(wk, &wk->printUtilMsg, wk->msgwin, str2, 16*4, 16);

        GFL_STR_DeleteBuffer(str2);
        GFL_STR_DeleteBuffer(str1);

        wk->debug_work[0] = 1;
      }
    }
  }else{
    str1 = GFL_STR_CreateBuffer(10, HEAPID_BASE_DEBUG);
    str2 = GFL_STR_CreateBuffer(10, HEAPID_BASE_DEBUG);

    GF_BGL_BmpWinFill(&wk->msgwin,FBMP_COL_WHITE,0,0,
              EARTH_MSG_WIN_SX*DOTSIZE,EARTH_MSG_WIN_SY*DOTSIZE);
    STRBUF_SetHexNumber( str1, (u16)wk->rotate.x, 4, 
              NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
    STRBUF_SetHexNumber( str2, (u16)wk->rotate.y, 4, 
              NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
    Earth_StrPrint(wk, &wk->printUtilMsg, wk->msgwin, str1, 0, 0);
    Earth_StrPrint(wk, &wk->printUtilMsg, wk->msgwin, str2, 0, 16);

    GFL_STR_DeleteBuffer(str2);
    GFL_STR_DeleteBuffer(str1);

    wk->debug_work[0] = 0;
  }
}

static void EarthDebugNationMarkSet( EARTH_DEMO_WORK * wk )
{
  u16 pattern_flag = WIFIHIST_STAT_NODATA;
  u16 cursor_pos;
  int nationID;

  if(wk->trg & PAD_BUTTON_SELECT){
    pattern_flag = WIFIHIST_STAT_NEW;
  }
  if(wk->trg & PAD_BUTTON_SELECT){
    pattern_flag = WIFIHIST_STAT_EXIST;
  }
  if(pattern_flag == WIFIHIST_STAT_NODATA){
    return;
  }
  BmpListDirectPosGet(wk->bmplist,&cursor_pos);

  nationID = cursor_pos+1;  //1origin

  if(WIFI_LocalAreaExistCheck(nationID) == FALSE){
    //地域が存在しない場合のみ設定
    WIFIHISTORY_SetStat(wk->wifi_sv,nationID,0,pattern_flag);
  }
  EarthListLoad(wk);  //reload
}

static void EarthDebugAreaMarkSet( EARTH_DEMO_WORK * wk )
{
  u16 pattern_flag = WIFIHIST_STAT_NODATA;
  u16 cursor_pos;
  int areaID;

  if(wk->trg & PAD_BUTTON_SELECT){
    pattern_flag = WIFIHIST_STAT_NEW;
  }
  if(wk->trg & PAD_BUTTON_SELECT){
    pattern_flag = WIFIHIST_STAT_EXIST;
  }
  if(pattern_flag == WIFIHIST_STAT_NODATA){
    return;
  }
  BmpListDirectPosGet(wk->bmplist,&cursor_pos);

  areaID = cursor_pos+1;  //1origin

  WIFIHISTORY_SetStat(wk->wifi_sv,wk->my_nation_tmp,areaID,pattern_flag);
  EarthListLoad(wk);  //reload
}

#endif




















