//=============================================================================
/**
 * @file	  c_gear.c
 * @brief	  コミュニケーションギア
 * @author	ohno_katsumi@gamefreak.co.jp
 * @author	takahashi_tomoya@gamefreak.co.jp　2010.4.3〜
 * @date	  09/04/30
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "arc_def.h"
#include "c_gear.h"
#include "c_gear_pattern.h"
#include "savedata/save_tbl.h"
#include "savedata/c_gear_picture.h"
#include "system/main.h"  //HEAPID
#include "system/palanm.h"  //HEAPID

#include "sound/pm_sndsys.h"
#include "system/wipe.h"

#include "c_gear.naix"
#include "c_gear_obj_NANR_LBLDEFS.h"

#include "net/wih_dwc.h"

#include "field/field_status.h"
#include "field/field_status_local.h"

#include "gamesystem/game_beacon_accessor.h"
#include "../../field/field_comm/intrude_work.h"
#include "../../field/field_comm/intrude_main.h"

#include "../field_sound.h"

#include "../event_ircbattle.h"      //EVENT_IrcBattle
#include "../event_gsync.h"         //EVENT_GSync
#include "../event_cg_wireless.h"         //EVENT_CG_Wireless
#include "net_app/cg_help.h"  //CGHELP呼び出し
#include "../event_fieldmap_control.h"  //CGHELP呼び出し
#include "../event_research_radar.h"  //EVENT_ResearchRadar
#include "../event_subscreen.h"      //EVENT_ChangeSubScreen
#include "../event_cg_power.h"      //EVENT_ChangeSubScreen


#ifdef PM_DEBUG

#ifdef DEUBG_ONLY_FOR_tomoya_takahashi

#define DEBUG_LOCAL

#endif

#endif

#define OAM_USE_PLTT_NUM (14)

static const u16 _BLACK_COLOR[] = {
  0x0441,
  0x0423,
  0x0441,
};

static const u32 _bgpal[]=
{
  NARC_c_gear_c_gear_m_NCLR,NARC_c_gear_c_gear_f_NCLR,NARC_c_gear_c_gear_m_NCLR,
};
static const u32 _bgcgx[][3]=
{
  { NARC_c_gear_c_gear_m1_NCGR,NARC_c_gear_c_gear_f1_NCGR,NARC_c_gear_c_gear_m1_NCGR, },
  { NARC_c_gear_c_gear_m2_NCGR,NARC_c_gear_c_gear_f2_NCGR,NARC_c_gear_c_gear_m2_NCGR, },
  { NARC_c_gear_c_gear_m3_NCGR,NARC_c_gear_c_gear_f3_NCGR,NARC_c_gear_c_gear_m3_NCGR, },
  { NARC_c_gear_c_gear_m4_NCGR,NARC_c_gear_c_gear_f4_NCGR,NARC_c_gear_c_gear_m4_NCGR, },
  { NARC_c_gear_c_gear_m5_NCGR,NARC_c_gear_c_gear_f5_NCGR,NARC_c_gear_c_gear_m5_NCGR, },
};

static const u32 _cellpal[]=
{
  NARC_c_gear_c_gear_m_obj_NCLR,NARC_c_gear_c_gear_f_obj_NCLR,NARC_c_gear_c_gear_m_obj_NCLR,
};


#define _NET_DEBUG (1)  //デバッグ時は１
#define _BRIGHTNESS_SYNC (0)  // フェードのＳＹＮＣは要調整

// サウンド仮想ラベル
#define GEAR_SE_DECIDE_ (SEQ_SE_DECIDE3)
#define GEAR_SE_CANCEL_ (SEQ_SE_SYS_70)

#define MSG_COUNTDOWN_FRAMENUM (30*3)

//--------------------------------------------
// 画面構成定義
//--------------------------------------------
#define _WINDOW_MAXNUM (4)   //ウインドウのパターン数

#define _MESSAGE_BUF_NUM	( 100*2 )

#define _BUTTON_WIN_CENTERX (16)   // 真ん中
#define _BUTTON_WIN_CENTERY (13)   //
#define _BUTTON_WIN_WIDTH (22)    // ウインドウ幅
#define _BUTTON_WIN_HEIGHT (3)    // ウインドウ高さ
#define _BUTTON_WIN_PAL   (12)  // ウインドウ
#define _BUTTON_MSG_PAL   (13)  // メッセージフォント

#define	_BUTTON_WIN_CGX_SIZE   ( 18+12 )
#define	_BUTTON_WIN_CGX	( 2 )


#define	_BUTTON_FRAME_CGX		( _BUTTON_WIN_CGX + ( 23 * 16 ) )	// 通信システムウィンドウ転送先

#define	FBMP_COL_WHITE		(15)


#define POS_SCANRADAR_X  (34)
#define POS_SCANRADAR_Y  (170)

#define POS_CROSS_X ( 72 )
#define POS_CROSS_X_CENTER ( POS_CROSS_X+32 )
#define POS_CROSS_Y ( 178 )
#define POS_CROSS_Y_CENTER ( POS_CROSS_Y-8 )

#define CROSS_COLOR_MAX ( 17 )
// (u32)配列としてのインデックス
static const u32 sc_CROSS_COLOR_RES_IDX[CROSS_COLOR_MAX] = {
  1, 2, 3, 4, 5, 6, 7,
  9,10,11,12,13,14,15,
  17,18,19,
};

#define CROSS_COLOR_TRANS_POS_START ( (6*16) )


//-------------------------

#define PANEL_Y1 (6)
#define PANEL_Y2 (4)
#define PANEL_X1 (-2)

#define PANEL_HEIDHT1 (C_GEAR_PANEL_HEIGHT-1)
#define PANEL_HEIGHT2 (C_GEAR_PANEL_HEIGHT)

#define PANEL_WIDTH (C_GEAR_PANEL_WIDTH)
#define PANEL_SIZEXY (4)   //

#define _CGEAR_TYPE_PATTERN_NUM (5)


typedef enum{
  _CLACT_PLT,
  _CLACT_CHR,
  _CLACT_ANM,
} _CGEAR_CLACT_TYPE;


typedef enum{
  _CGEAR_NET_BIT_IR = (1 << CGEAR_PANELTYPE_IR),
  _CGEAR_NET_BIT_WIRELESS = (1 << CGEAR_PANELTYPE_WIRELESS),
  _CGEAR_NET_BIT_WIFI = (1 << CGEAR_PANELTYPE_WIFI),
} _CGEAR_NET_BIT;



enum{
  // パネル数
  _CGEAR_NET_CHANGEPAL_IRC = 0,
  _CGEAR_NET_CHANGEPAL_WIFI,
  _CGEAR_NET_CHANGEPAL_WIRELES,
  _CGEAR_NET_CHANGEPAL_MAX,

  
  // パレットタイプ
  _CGEAR_NET_PALTYPE_RED = 0,
  _CGEAR_NET_PALTYPE_BLUE,
  _CGEAR_NET_PALTYPE_YELLOW,
  _CGEAR_NET_PALTYPE_GREEN,

  _CGEAR_NET_PALTYPE_MAX,

  // ワイアレスカラーカウント
  _CGEAR_NET_WIRELES_TYPE_PALACE = 0,
  _CGEAR_NET_WIRELES_TYPE_UNION,
  _CGEAR_NET_WIRELES_TYPE_FUSHIGI,
  _CGEAR_NET_WIRELES_TYPE_MAX,
  
};


// パレット　通信受信アニメの数値
static const u8 _CGEAR_NET_CHANGEPAL_POSX[ _CGEAR_NET_CHANGEPAL_MAX ] = {
  0xf, 0x7, 0xb, 
};
#define _CGEAR_NET_CHANGEPAL_POSY (1)

enum{
  _CGEAR_NET_CHANGEPAL_ANM_TYPE_HIGH,
  _CGEAR_NET_CHANGEPAL_ANM_TYPE_NORMAL,

  _CGEAR_NET_CHANGEPAL_ANM_TYPE_MAX,
};
static const u16 _CGEAR_NET_CHANGEPAL_ANM_COUNT_MAX[ _CGEAR_NET_CHANGEPAL_ANM_TYPE_MAX ] = 
{
  24,
  160,  // 
};

static const u8 _CGEAR_NET_CHANGEPAL_ANM_MOD_TBL[] = {
   0, 4, 8, 
  10,14,18,
  20,22,24,26,28,
  30,31,31,31,31,31,30,
  28,26,24,22,20,
  18,14,10,
   8, 4, 0,
};
#define _CGEAR_NET_CHANGEPAL_ANM_MOD_NUM_MAX  ( NELEMS(_CGEAR_NET_CHANGEPAL_ANM_MOD_TBL) )
#define _CGEAR_NET_CHANGEPAL_ANM_MOD_MAX  ( 31 )

#define _CGEAR_NET_CHANGEPAL_ANM_HIGH_COUNT  ( 20)



#define _CGEAR_NET_CHANGEPAL_ANM_NORMAL_TOPWAIT  ( 10 )
#define _CGEAR_NET_CHANGEPAL_ANM_NORMAL_COUNT  ( 140 )
#define _CGEAR_NET_CHANGEPAL_ANM_NORMAL_MOD_MAX ( 48 ) // 通常時は彩度を落とす



//--------------------------

typedef enum{
  _SELECTANIM_NONE,
  _SELECTANIM_WAIT,
  _SELECTANIM_STANDBY,
  _SELECTANIM_ANIMING,
  _SELECTANIM_RUN,
  _SELECTANIM_END,


} _SELECTANIM_ENUM;
#define _SELECT_ANIME_WAIT (16)


//-------------------------------------
///	パネルパレット情報
//=====================================
enum {

  PANEL_PLTT_NONE = 0x6,
  PANEL_PLTT_0 = 0x5,
  PANEL_PLTT_1 = 0x4,
  PANEL_PLTT_2 = 0x3,
  PANEL_PLTT_3 = 0x2,
  PANEL_PLTT_HIGH = 0x1,

  _CGEAR_NET_CHANGEPAL_HIGH = 0,
  _CGEAR_NET_CHANGEPAL_3,
  _CGEAR_NET_CHANGEPAL_2,
  _CGEAR_NET_CHANGEPAL_1,
  _CGEAR_NET_CHANGEPAL_0,
  _CGEAR_NET_CHANGEPAL_NUM, // 5
} ;




//-------------------------------------
///	パネルのカラーアニメ
//=====================================
enum{

  // アニメタイプ
  PANEL_ANIME_TYPE_ON_OFF = 0,
  PANEL_ANIME_TYPE_ON,
  PANEL_ANIME_TYPE_OFF,
  PANEL_ANIME_TYPE_MAX,

  // カラータイプ
  PANEL_COLOR_TYPE_NONE = 0,
  PANEL_COLOR_TYPE_RED,
  PANEL_COLOR_TYPE_YELLOW,
  PANEL_COLOR_TYPE_BLUE,
  PANEL_COLOR_TYPE_BASE,
  PANEL_COLOR_TYPE_MAX,

  // アニメーション段階
  PANEL_COLOR_PATTERN_NUM = _CGEAR_NET_CHANGEPAL_NUM,

  // アニメーションフレーム
  PANEL_ANIME_DEF_FRAME = 2,
  PANEL_ANIME_MIDDLE_FRAME = 3,
  PANEL_ANIME_SLOW_FRAME = 4,
  

};
static const u8 sc_PANEL_COLOR_ANIME_END_INDEX[ PANEL_COLOR_TYPE_MAX ][ 9 ] = {
  // NONE
  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  // RED
  {  0, 1, 2, 3, 4, 3, 2, 1, 0 },
  // YELLOW
  {  0, 1, 2, 3, 4, 3, 2, 1, 0 },
  // BLUE
  {  0, 1, 2, 3, 4, 3, 2, 1, 0 },
  // BASE
  {  0, 1, 2, 3, 4, 3, 2, 1, 0 },
};
// カラーPATTERN情報
static const u8 sc_PANEL_COLOR_ANIME_TBL[ PANEL_COLOR_TYPE_MAX ][ PANEL_COLOR_PATTERN_NUM ] = 
{
  // NONE
  { PANEL_PLTT_NONE, PANEL_PLTT_0, PANEL_PLTT_1, PANEL_PLTT_2, PANEL_PLTT_3 },
  // RED
  { PANEL_PLTT_0, PANEL_PLTT_1, PANEL_PLTT_2, PANEL_PLTT_3, PANEL_PLTT_HIGH },
  // YELLOW
  { PANEL_PLTT_0, PANEL_PLTT_1, PANEL_PLTT_2, PANEL_PLTT_3, PANEL_PLTT_HIGH },
  // BLUE
  { PANEL_PLTT_0, PANEL_PLTT_1, PANEL_PLTT_2, PANEL_PLTT_3, PANEL_PLTT_HIGH },
  // BASE
  { PANEL_PLTT_0, PANEL_PLTT_1, PANEL_PLTT_2, PANEL_PLTT_3, PANEL_PLTT_HIGH },
};
// カラータイプのパネルタイプ
static const u16 sc_PANEL_TYPE_TO_COLOR[] = {
  PANEL_COLOR_TYPE_NONE,
  PANEL_COLOR_TYPE_RED,
  PANEL_COLOR_TYPE_YELLOW,
  PANEL_COLOR_TYPE_BLUE,
  PANEL_COLOR_TYPE_BASE,
  PANEL_COLOR_TYPE_BASE,
};



//-------------------------------------
///	OAMパレットフェード
//=====================================
#define OAM_PFADE_NORMAL_MSK  ( (1<<0x1) | (1<<0x2) | (1<<0x3) | (1<<0x5) | (1<<0x6) | (1<<0x7) | (1<<0x8) | (1<<0x9) | (1<<0xB) | (1<<0xC) | (1<<0xD) | (1<<0xE) | (1<<0xF) )
//-------------------------------------
///	BGパレットフェード
//=====================================
#define BG_PFADE_NORMAL_MSK  ( (1<<0x1) | (1<<0x2) | (1<<0x3) | (1<<0x4) | (1<<0x5) )


//--------------------------

typedef struct {
  int leftx;
  int lefty;
  int width;
  int height;
} _WINDOWPOS;

typedef struct {
  u8 time;
  u8 x;
  u8 y;
  u8 frame;
} _ANIM_DATA;




// 表示OAMの時間とかの最大
enum{
  _CLACT_TIME_AMPM,
  _CLACT_TIME_NUMBER_2,
  _CLACT_TIME_NUMBER_10A,
  _CLACT_TIME_COLON,
  _CLACT_TIME_NUMBER_6,
  _CLACT_TIME_NUMBER_10B,
  _CLACT_BATT,
  _CLACT_LOGO,
  _CLACT_HELP,
  _CLACT_EDITMARKON,
  _CLACT_POWER,
  _CLACT_TIMEPARTS_MAX,
};


//すれ違いよう
#define _CLACT_CROSS_MAX  (10)

// タイプ
#define _CLACT_TYPE_MAX (3)

#define POS_HELPMARK_X    (198)
#define POS_HELPMARK_Y    (POS_SCANRADAR_Y)
#define POS_EDITMARK_X    (174)
#define POS_EDITMARK_Y    (POS_SCANRADAR_Y)
#define POS_POWERMARK_X    (222)
#define POS_POWERMARK_Y    (POS_SCANRADAR_Y)

#define _SLEEP_START_TIME_WAIT (16)


// スリープ復帰アニメ
enum{
  BOOT_ANIME_SEQ_ALL_ON,
  BOOT_ANIME_SEQ_ALL_WAIT,
  BOOT_ANIME_SEQ_SMOOTHIN_MAIN,
  BOOT_ANIME_SEQ_END,
};

// スリープ復帰
const static _ANIM_DATA screenTable[]={
  { 0, 0, 0, PANEL_ANIME_DEF_FRAME},
  { 0, 0, 1, PANEL_ANIME_DEF_FRAME},
  { 0, 0, 2, PANEL_ANIME_DEF_FRAME},
  { 0, 8, 0, PANEL_ANIME_DEF_FRAME},
  { 0, 8, 1, PANEL_ANIME_DEF_FRAME},
  { 0, 8, 2, PANEL_ANIME_DEF_FRAME},

  { 1, 1, 0, PANEL_ANIME_DEF_FRAME},
  { 1, 1, 1, PANEL_ANIME_DEF_FRAME},
  { 1, 1, 2, PANEL_ANIME_DEF_FRAME},
  { 1, 1, 3, PANEL_ANIME_DEF_FRAME},
  { 1, 7, 0, PANEL_ANIME_DEF_FRAME},
  { 1, 7, 1, PANEL_ANIME_DEF_FRAME},
  { 1, 7, 2, PANEL_ANIME_DEF_FRAME},
  { 1, 7, 3, PANEL_ANIME_DEF_FRAME},

  { 2, 2, 0, PANEL_ANIME_DEF_FRAME},
  { 2, 2, 1, PANEL_ANIME_DEF_FRAME},
  { 2, 2, 2, PANEL_ANIME_DEF_FRAME},
  { 2, 6, 0, PANEL_ANIME_DEF_FRAME},
  { 2, 6, 1, PANEL_ANIME_DEF_FRAME},
  { 2, 6, 2, PANEL_ANIME_DEF_FRAME},

  { 3, 3, 0, PANEL_ANIME_DEF_FRAME},
  { 3, 3, 1, PANEL_ANIME_DEF_FRAME},
  { 3, 3, 2, PANEL_ANIME_DEF_FRAME},
  { 3, 3, 3, PANEL_ANIME_DEF_FRAME},
  { 3, 5, 0, PANEL_ANIME_DEF_FRAME},
  { 3, 5, 1, PANEL_ANIME_DEF_FRAME},
  { 3, 5, 2, PANEL_ANIME_DEF_FRAME},
  { 3, 5, 3, PANEL_ANIME_DEF_FRAME},

  { 4, 4, 0, PANEL_ANIME_DEF_FRAME},
  { 4, 4, 2, PANEL_ANIME_DEF_FRAME},
  { 4, 4, 1, PANEL_ANIME_DEF_FRAME},
};

// 起動アニメ
#define START_UP_SCREEN_ANIME_FRAME_MAX ( 126 )
#define START_UP_SCREEN_ANIME_FRAME_START (0)
#define START_UP_SCREEN_ANIME_FRAME_NEXT (56)
#define START_UP_SCREEN_ANIME_FRAME_LAST (96)
const static _ANIM_DATA StartUpScreenTable[]={
  // 最初のいっこ
  { START_UP_SCREEN_ANIME_FRAME_START, 4, 1, PANEL_ANIME_SLOW_FRAME},

  // まわりの４こ
  { START_UP_SCREEN_ANIME_FRAME_NEXT+0, 2, 1, PANEL_ANIME_MIDDLE_FRAME},
  { START_UP_SCREEN_ANIME_FRAME_NEXT+2, 6, 1, PANEL_ANIME_MIDDLE_FRAME},
  { START_UP_SCREEN_ANIME_FRAME_NEXT+4, 4, 0, PANEL_ANIME_MIDDLE_FRAME},
  { START_UP_SCREEN_ANIME_FRAME_NEXT+6, 4, 2, PANEL_ANIME_MIDDLE_FRAME},


  // 最後のいっぱい出るところ 斜め対象的に点滅する。　こまおくりでみてみてください。
  { 0+START_UP_SCREEN_ANIME_FRAME_LAST, 2, 0, PANEL_ANIME_DEF_FRAME},
  { 3+START_UP_SCREEN_ANIME_FRAME_LAST, 6, 2, PANEL_ANIME_DEF_FRAME},
  { 5+START_UP_SCREEN_ANIME_FRAME_LAST, 3, 1, PANEL_ANIME_DEF_FRAME},
  { 7+START_UP_SCREEN_ANIME_FRAME_LAST, 5, 2, PANEL_ANIME_DEF_FRAME},
  { 8+START_UP_SCREEN_ANIME_FRAME_LAST, 1, 2, PANEL_ANIME_DEF_FRAME},
  { 8+START_UP_SCREEN_ANIME_FRAME_LAST, 7, 1, PANEL_ANIME_DEF_FRAME},
  { 9+START_UP_SCREEN_ANIME_FRAME_LAST, 1, 1, PANEL_ANIME_DEF_FRAME},
  { 9+START_UP_SCREEN_ANIME_FRAME_LAST, 7, 2, PANEL_ANIME_DEF_FRAME},
  {10+START_UP_SCREEN_ANIME_FRAME_LAST, 3, 2, PANEL_ANIME_DEF_FRAME},
  {10+START_UP_SCREEN_ANIME_FRAME_LAST, 5, 1, PANEL_ANIME_DEF_FRAME},
  {11+START_UP_SCREEN_ANIME_FRAME_LAST, 2, 2, PANEL_ANIME_DEF_FRAME},
  {11+START_UP_SCREEN_ANIME_FRAME_LAST, 6, 0, PANEL_ANIME_DEF_FRAME},
  {12+START_UP_SCREEN_ANIME_FRAME_LAST, 2, 1, PANEL_ANIME_DEF_FRAME},
  {12+START_UP_SCREEN_ANIME_FRAME_LAST, 6, 1, PANEL_ANIME_DEF_FRAME},

  {13+START_UP_SCREEN_ANIME_FRAME_LAST, 3, 0, PANEL_ANIME_DEF_FRAME},
  {13+START_UP_SCREEN_ANIME_FRAME_LAST, 5, 3, PANEL_ANIME_DEF_FRAME},
  {14+START_UP_SCREEN_ANIME_FRAME_LAST, 1, 0, PANEL_ANIME_DEF_FRAME},
  {14+START_UP_SCREEN_ANIME_FRAME_LAST, 0, 0, PANEL_ANIME_DEF_FRAME},
  {15+START_UP_SCREEN_ANIME_FRAME_LAST, 7, 3, PANEL_ANIME_DEF_FRAME},
  {15+START_UP_SCREEN_ANIME_FRAME_LAST, 8, 2, PANEL_ANIME_DEF_FRAME},
  {16+START_UP_SCREEN_ANIME_FRAME_LAST, 0, 1, PANEL_ANIME_DEF_FRAME},
  {16+START_UP_SCREEN_ANIME_FRAME_LAST, 0, 2, PANEL_ANIME_DEF_FRAME},
  {17+START_UP_SCREEN_ANIME_FRAME_LAST, 8, 2, PANEL_ANIME_DEF_FRAME},
  {17+START_UP_SCREEN_ANIME_FRAME_LAST, 8, 1, PANEL_ANIME_DEF_FRAME},
  {17+START_UP_SCREEN_ANIME_FRAME_LAST, 4, 0, PANEL_ANIME_DEF_FRAME},
  {18+START_UP_SCREEN_ANIME_FRAME_LAST, 1, 3, PANEL_ANIME_DEF_FRAME},
  {18+START_UP_SCREEN_ANIME_FRAME_LAST, 3, 3, PANEL_ANIME_DEF_FRAME},
  {18+START_UP_SCREEN_ANIME_FRAME_LAST, 4, 2, PANEL_ANIME_DEF_FRAME},
  {19+START_UP_SCREEN_ANIME_FRAME_LAST, 7, 0, PANEL_ANIME_DEF_FRAME},
  {19+START_UP_SCREEN_ANIME_FRAME_LAST, 5, 0, PANEL_ANIME_DEF_FRAME},
  {19+START_UP_SCREEN_ANIME_FRAME_LAST, 4, 1, PANEL_ANIME_DEF_FRAME},
};




enum{
  TOUCH_LABEL_ALL,
  TOUCH_LABEL_EDIT,
  TOUCH_LABEL_HELP,
  TOUCH_LABEL_CROSS,
  TOUCH_LABEL_RADAR,
  TOUCH_LABEL_LOGO,
  TOUCH_LABEL_POWER,

  TOUCH_LABEL_MAX,
};

static const GFL_UI_TP_HITTBL bttndata[] = {  //上下左右
 
  {	PANEL_Y2 * 8,  PANEL_Y2 * 8 + (PANEL_SIZEXY * 8 * PANEL_HEIGHT2), 0,32*8-1 },   //タッチパネル全部
  { (POS_EDITMARK_Y-8), (POS_EDITMARK_Y+8), (POS_EDITMARK_X-8), (POS_EDITMARK_X+8) },        //こうさく
  { (POS_HELPMARK_Y-8), (POS_HELPMARK_Y+8), (POS_HELPMARK_X-8), (POS_HELPMARK_X+8) },   //HELP
  { (POS_CROSS_Y_CENTER-8), (POS_CROSS_Y_CENTER+8), POS_CROSS_X_CENTER-40, POS_CROSS_X_CENTER+40 },                //SURECHIGAI
  { (POS_SCANRADAR_Y-8), (POS_SCANRADAR_Y+8), (POS_SCANRADAR_X-8), (POS_SCANRADAR_X+8) }, //RADAR
  { 22-8, (22+8), 128-32, 128+32 },    //CGEARLOGO
  { (POS_POWERMARK_Y-8), (POS_POWERMARK_Y+8), (POS_POWERMARK_X-8), (POS_POWERMARK_X+8) },        //電源
  {GFL_UI_TP_HIT_END,0,0,0},		 //終了データ
};


//-------------------------------------
///	起動アニメーション
//=====================================
#define STARTUP_ANIME_OBJ_MAX ( 18 )      // 起動アニメーションに必要なオブジェ数
#define STARTUP_ANIME_INDEX_START ( NANR_c_gear_obj_CellAnime_logo1 )  // 各オブジェに設定するアニメーション開始インデックス
#define STARTUP_ANIME_POS_X (128)
#define STARTUP_ANIME_POS_Y (96)
#define STARTUP_OAM_ALPHA_ANIME_FRAME_MAX ( 8 )
#define STARTUP_TBL_ALPHA_TIME_WAIT ( 8 )
#define STARTUP_END_TIME_WAIT ( 8 )
enum
{
  STARTUP_SEQ_ANIME_START,
  STARTUP_SEQ_WIPE_IN,
  STARTUP_SEQ_ANIME_WAIT,
  STARTUP_SEQ_OAM_ALPHA_WAIT,
  STARTUP_SEQ_TBL_IN_WAIT,
  STARTUP_SEQ_TBL_ALPHA_WAIT,
  STARTUP_SEQ_END,

  STARTUP_SEQ_SKIP,
};



//-------------------------------------------------------------------------
///	文字表示色定義(default)	-> gflib/fntsys.hへ移動
//------------------------------------------------------------------

#define WINCLR_COL(col)	(((col)<<4)|(col))



//--------------------------------------------
// 内部ワーク
//--------------------------------------------


#define GEAR_MAIN_FRAME   (GFL_BG_FRAME2_S)
#define GEAR_BMPWIN_FRAME   (GFL_BG_FRAME1_S)
#define GEAR_BUTTON_FRAME   (GFL_BG_FRAME0_S)
#define GEAR_FB_MESSAGE   (GFL_BG_FRAME3_S)

#define CGEAR_CLACT_BG_PRI  ( 2 )


//-------------------------------------
///	セルACTORユーザー連打ラー
//=====================================
enum {
  CGEAR_REND_SUB,

  CGEAR_REND_MAX,
} ;
static const GFL_REND_SURFACE_INIT sc_REND_SURFACE_INIT[ CGEAR_REND_MAX ] = {
  {
    0, 0,
    256, 192,
    CLSYS_DRAW_SUB,
    CLSYS_REND_CULLING_TYPE_NONE,
  },
};


typedef void (StateFunc)(C_GEAR_WORK* pState);
typedef BOOL (TouchFunc)(int no, C_GEAR_WORK* pState);


//-------------------------------------
///	演出待ちイベントワーク
//=====================================
typedef struct {
  C_GEAR_WORK* pWork;
  u32 createEvent;
} EV_CGEAR_EFFECTWAIT;

//-------------------------------------
///	パネル　マーク　アニメ
//=====================================
typedef struct {
  u16 anime_on:1;
  u16 count:15;
  u16 one_frame;
  u8  color;
  u8  end_anime_index;
  u8  x;
  u8  y;
} PANEL_MARK_ANIME;

//-------------------------------------
///	パレット演出コントロール
//=====================================
typedef struct{
  u16  plt_count;
  u8  plt_anmtype;
  u8  wireles_count;
  u32 last_bit;
  u32 plt_beacon;
} PANEL_PLT_ANIME;

//-------------------------------------
///	ボタンパレットフェード処理
//=====================================
typedef struct {
  u32 anime_on;
  u32 msk;
  GFL_CLWK* dummy;
  PALETTE_FADE_PTR  p_fade_ptr;
  GFL_TCBSYS* p_fade_tcbsys;
} BUTTON_PAL_FADE;


struct _C_GEAR_WORK {
  StateFunc* state;      ///< ハンドルのプログラム状態
  TouchFunc* touch;
  int selectType;   // 接続タイプ
  HEAPID heapID;
  GFL_BUTTON_MAN* pButton;
  GFL_ARCUTIL_TRANSINFO subchar;
  FIELD_SUBSCREEN_WORK* subscreen;
  GAMESYS_WORK* pGameSys;
  CGEAR_SAVEDATA* pCGSV;
  ARCHANDLE* handle;  // アーカイブハンドル
  u32 objRes[3];  //CLACTリソース

  GFL_CLUNIT *cellUnit;
  GFL_CLSYS_REND* userRend;
  GFL_CLWK  *cellSelect[C_GEAR_PANEL_WIDTH * C_GEAR_PANEL_HEIGHT];
  GFL_CLWK  *cellCursor[_CLACT_TIMEPARTS_MAX];
  GFL_CLWK  *cellType[_CLACT_TYPE_MAX];
  GFL_CLWK  *cellCross;
  GFL_CLWK  *cellCrossBase;
  GFL_CLWK  *cellRadar;
  GFL_CLWK  *cellStartUp[STARTUP_ANIME_OBJ_MAX];
  u32 crossColor[CROSS_COLOR_MAX]; //すれ違いカラー

  GFL_CLWK  *cellMove;

  STARTUP_ENDCALLBACK* pCall;
  void* pWork;

  GFL_TCBSYS* pfade_tcbsys;
  GFL_TCB*                    vblank_tcb;
  void* pfade_tcbsys_wk;
  PALETTE_FADE_PTR            pfade_ptr;
  
  int createEvent;

  BUTTON_PAL_FADE button_palfade;

  u16 palBase[_CGEAR_NET_CHANGEPAL_MAX][_CGEAR_NET_CHANGEPAL_NUM];
  u16 palTrans[_CGEAR_NET_CHANGEPAL_MAX][_CGEAR_NET_CHANGEPAL_NUM];
  u16 palChangeType[ _CGEAR_NET_CHANGEPAL_MAX ];
  u16 palChangeCol[_CGEAR_NET_PALTYPE_MAX][_CGEAR_NET_CHANGEPAL_NUM];
  u16 tpx;
  u16 tpy;

  // パネル
  PANEL_MARK_ANIME panel_mark[C_GEAR_PANEL_WIDTH][C_GEAR_PANEL_HEIGHT];

  u8 typeAnim[C_GEAR_PANEL_WIDTH][C_GEAR_PANEL_HEIGHT];
  u8 select_type;
  u8 select_count;

  // 選択カーソル
  u8 select_cursor;

  // パレットアニメーション
  PANEL_PLT_ANIME plt_anime;

  u8 GetOpenTrg;
  u8 beacon_bit;   //ビーコンbit
  u8 touchx;    //タッチされた場所
  u8 touchy;    //タッチされた場所
  u8 bAction;
  u8 cellMoveCreateCount;
  u8 cellMoveType;
  u8 state_seq;
  u8 startCounter;
  u8 bPanelEdit;
  u8 bgno;
  u8 sex;
  u8 power_flag;  // 電源がはいっているか？
  u8 use_skip;    // スキップ許可

  u8 sleep_mode;  // イベント中のスリープモード
  u8 high_sleep_mode;  // Highスリープモード
  u8 sleep_color_req;

  u8 tvt_snd;    // 
};



//-----------------------------------------------
//static 定義
//-----------------------------------------------
static void _changeState(C_GEAR_WORK* pWork,StateFunc* state);
static void _changeStateDebug(C_GEAR_WORK* pWork,StateFunc* state, int line);
static void _buttonWindowCreate(int num,int* pMsgBuff,C_GEAR_WORK* pWork);
static void _modeSelectMenuInit(C_GEAR_WORK* pWork);
static void _modeSelectMenuWait(C_GEAR_WORK* pWork);  // 通常の待機
static void _modeSelectMenuWait0(C_GEAR_WORK* pWork);  // 初期入手の演出
static void _modeSelectMenuWait1(C_GEAR_WORK* pWork);  // スリープ復帰の演出
static void _modeSelectMenuWait2(C_GEAR_WORK* pWork);  // 復帰・初期　共通の演出
static void _gearXY2PanelScreen(int x,int y, int* px, int* py);
static BOOL _modeSelectWaitSubFadeEnd( const C_GEAR_WORK* cpWork );


static void _modeSetSavePanelType( C_GEAR_WORK* pWork, CGEAR_SAVEDATA* pSV,int x, int y, CGEAR_PANELTYPE_ENUM type );

// CGEARセーブ情報の取得
static CGEAR_PANELTYPE_ENUM _cgearSave_GetPanelType(const C_GEAR_WORK* cpWork,int x, int y);
static BOOL _cgearSave_IsPanelTypeIcon(const C_GEAR_WORK* cpWork,int x, int y);
static BOOL _cgearSave_IsPanelTypeLast(const C_GEAR_WORK* cpWork,int x, int y, CGEAR_PANELTYPE_ENUM type );

// スキップチェック
static BOOL _IsEffectSkip( const C_GEAR_WORK* cpWork );


// サブBGのセットアップ
static void _createSubBg(C_GEAR_WORK* pWork);
static void _setUpSubAlpha( C_GEAR_WORK* pWork );

// スクリーン操作
static void _gearPanelBgScreenMake(C_GEAR_WORK* pWork,int xs,int ys, CGEAR_PANELTYPE_ENUM type);
static void _gearBootInitScreen(C_GEAR_WORK* pWork);
static BOOL _gearBootMain(C_GEAR_WORK* pWork);
static BOOL _gearStartUpMain(C_GEAR_WORK* pWork);

static void _gearStartUpAllOff(C_GEAR_WORK* pWork);

static void _timeAnimation(C_GEAR_WORK* pWork);
static void _typeAnimation(C_GEAR_WORK* pWork);
static void _editMarkONOFF(C_GEAR_WORK* pWork,BOOL bOn);
static void _gearArcCreate(C_GEAR_WORK* pWork, int sex, u32 bgno, BOOL pal_trans);
static void _arcGearRelease(C_GEAR_WORK* pWork);
static void _gearObjResCreate(C_GEAR_WORK* pWork, int sex);
static void _gearObjCreate(C_GEAR_WORK* pWork);
static void _gearCrossObjCreate(C_GEAR_WORK* pWork);

// OAM操作
static void _gearMarkObjDrawEnable(C_GEAR_WORK* pWork,BOOL bFlg);
static void _gearAllObjDrawEnabel(C_GEAR_WORK* pWork,BOOL bFlg);
static void _gearStartUpObjDrawEnabel(C_GEAR_WORK* pWork,BOOL bFlg);
static void _gearStartStartUpObjAnime(C_GEAR_WORK* pWork);
static void _gearEndStartUpObjAnime(C_GEAR_WORK* pWork);
static BOOL _gearIsEndStartUpObjAnime(const C_GEAR_WORK* cpWork);




// パネルアニメーション処理
static void _PanelPaletteAnimeInit( C_GEAR_WORK* pWork );
static void _PanelPaletteAnimeInitSleep( C_GEAR_WORK* pWork );
static void _PaletteSetColType( C_GEAR_WORK* pWork, int panel,int type );
static void _PaletteMake(C_GEAR_WORK* pWork,const u16* pltt,int type);
static void _PanelPaletteChange(C_GEAR_WORK* pWork);
static void _PanelPaletteChangeHigh(C_GEAR_WORK* pWork, int change_panel );
static void _PanelPaletteChangeNormal(C_GEAR_WORK* pWork, int change_panel);
static void _PanelPaletteChangeCore( C_GEAR_WORK* pWork, int change_panel, int mod, int mod_max );
static BOOL _PanelPaletteIsEnd(const C_GEAR_WORK* cpWork );
static void _PanelPaletteUpdate( C_GEAR_WORK* pWork );
static void _PanelPalette_EXEnd( C_GEAR_WORK* pWork );
static u32 _PanelPaletteColorSetUp( C_GEAR_WORK* pWork );
static BOOL _PanelPaletteIsBeaconChange( GAME_COMM_SYS_PTR game_comm, u32 last_bit, u32 bit, u32* change_beacon );


static void _PanelMarkAnimeSysInit( C_GEAR_WORK* pWork );
static void _PanelMarkAnimeSysMain( C_GEAR_WORK* pWork );
static BOOL _PanelMarkAnimeSysIsAnime( const C_GEAR_WORK* cpWork );
static void _PanelMarkAnimeSysAllStop( C_GEAR_WORK* pWork );

static void _PanelMarkAnimeInit( PANEL_MARK_ANIME* p_mark, int x, int y );
static void _PanelMarkAnimeSetOff( PANEL_MARK_ANIME* p_mark, C_GEAR_WORK* pWork, CGEAR_PANELTYPE_ENUM panel_type );
static void _PanelMarkAnimeStart( PANEL_MARK_ANIME* p_mark, C_GEAR_WORK* pWork, u8 color_type, u8 anime_type, CGEAR_PANELTYPE_ENUM panel_type, u16 frame );
static void _PanelMarkAnimeStop( PANEL_MARK_ANIME* p_mark);
static void _PanelMarkAnimeMain( PANEL_MARK_ANIME* p_mark, const C_GEAR_WORK* cp_work );
static void _PanelMarkAnimeWriteScreen( const PANEL_MARK_ANIME* cp_mark, u32 anime_index );
static BOOL _PanelMarkAnimeIsAnime( const PANEL_MARK_ANIME* cp_mark );

static int _PanelMark_GetPatternIndex( const C_GEAR_WORK* cpWork, u32 col_type, int x, int y );

// 選択アニメ
static void _modeSelectAnimInit(C_GEAR_WORK* pWork);
static BOOL _modeSelectAnimWait(C_GEAR_WORK* pWork);
static void _selectAnimInit(C_GEAR_WORK* pWork,int x,int y);

// カーソル選択アニメ
static BOOL _CursorSelectAnimeWait( C_GEAR_WORK* pWork );
static BOOL _PalAnimeSelectAnimeWait( C_GEAR_WORK* pWork );

// イベント待ち
static void _modeEventWait( C_GEAR_WORK* pWork );

// スリープモードの管理
static void SleepMode_Start( C_GEAR_WORK* pWork );
static void SleepMode_HighSleepStart( C_GEAR_WORK* pWork );
static void SleepMode_End( C_GEAR_WORK* pWork );
static void SleepMode_ColorUpdate( C_GEAR_WORK* pWork );
static BOOL SleepMode_IsSleep( const C_GEAR_WORK* cpWork );

// パレットフェード
static void _PFadeSetDefaultPal( C_GEAR_WORK* pWork, BOOL comm );
static void _PFadeSetBlack( C_GEAR_WORK* pWork );
static void _PFadeSetSleepBlack( C_GEAR_WORK* pWork, BOOL on_flag );
static void _PFadeToBlack( C_GEAR_WORK* pWork );
static void _PFadeFromBlack( C_GEAR_WORK* pWork );
static BOOL _PFadeIsFade( const C_GEAR_WORK* cpWork );

// パレットフェード　ボタンアニメ
static void _BUTTONPAL_Init( C_GEAR_WORK* pWork, BUTTON_PAL_FADE* p_fwk );
static void _BUTTONPAL_Exit( BUTTON_PAL_FADE* p_fwk );
static void _BUTTONPAL_Start( BUTTON_PAL_FADE* p_fwk, u32 msk );
static BOOL _BUTTONPAL_Update( BUTTON_PAL_FADE* p_fwk );


// CGEAR演出イベント
static GMEVENT* EVENT_ButtonEffectWaitCall( C_GEAR_WORK* pWork, u32 createEvent );
static GMEVENT_RESULT EVENT_ButtonEffectWait( GMEVENT *event, int *seq, void *wk );

#ifdef _NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG


//------------------------------------------------------------------------------
/**
 * @brief   チップを置いて良いかどうか
 * @retval  置いていいときTRUE
 */
//------------------------------------------------------------------------------

static BOOL _isSetChip(int x,int y)
{
  if((x % 2) == 0){  // 偶数なら短い列の方   yは=かy-1が隣になる
    if(y < (C_GEAR_PANEL_HEIGHT-1)){
      return TRUE;
    }
  }
  else{
    if(y < (C_GEAR_PANEL_HEIGHT)){
      return TRUE;
    }
  }
  return FALSE;
}



//------------------------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(C_GEAR_WORK* pWork,StateFunc state)
{
  pWork->state = state;
  pWork->startCounter = 0;
  pWork->state_seq = 0;
}

//------------------------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(C_GEAR_WORK* pWork,StateFunc state, int line)
{
  NET_PRINT("cg: %d\n",line);
  _changeState(pWork, state);
}
#endif


//------------------------------------------------------------------------------
/**
 * @brief   与えられたxyから周囲の座標を返す
 * @param   x,y          探す場所
 * @param   xbuff,ybuff  探した場所をバッファする
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _getHexAround(int x,int y, int* xbuff,int* ybuff)
{
  if((x % 2) == 0){  // 偶数なら短い列の方   yは=かy-1が隣になる
    xbuff[0]=x;   ybuff[0]=y-1;
    xbuff[1]=x;   ybuff[1]=y+1;
    xbuff[2]=x+1; ybuff[2]=y;
    xbuff[3]=x+1; ybuff[3]=y-1;
    xbuff[4]=x-1; ybuff[4]=y;
    xbuff[5]=x-1; ybuff[5]=y-1;
  }
  else{   // 奇数なら長い列の方   yは=かy+1が隣になる
    xbuff[0]=x;   ybuff[0]=y-1;
    xbuff[1]=x;   ybuff[1]=y+1;
    xbuff[2]=x+1; ybuff[2]=y;
    xbuff[3]=x+1; ybuff[3]=y+1;
    xbuff[4]=x-1; ybuff[4]=y;
    xbuff[5]=x-1; ybuff[5]=y+1;
  }
}



static void _selectAnimInit(C_GEAR_WORK* pWork,int x,int y)
{
  GFL_CLWK_DATA cellInitData;
  int scrx,scry,i;

  _gearXY2PanelScreen(x, y, &scrx, &scry);
  cellInitData.pos_x = scrx * 8+16;
  cellInitData.pos_y = scry * 8+16;
  cellInitData.anmseq = NANR_c_gear_obj_CellAnime01 + (pWork->select_type - CGEAR_PANELTYPE_IR);
  cellInitData.softpri = 0;
  cellInitData.bgpri = CGEAR_CLACT_BG_PRI;
  i = x + y * C_GEAR_PANEL_WIDTH;
  pWork->cellSelect[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                              pWork->objRes[_CLACT_CHR],
                                              pWork->objRes[_CLACT_PLT],
                                              pWork->objRes[_CLACT_ANM],
                                              &cellInitData ,
                                              CGEAR_REND_SUB,
                                              pWork->heapID );
  GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellSelect[i], TRUE );
  GFL_CLACT_WK_SetDrawEnable( pWork->cellSelect[i], TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  アニメ完了待ち
 *
 *	@param	C_GEAR_WORK* pWork
 *	@param	x
 *	@param	y 
 *
 *	@retval TRUE  完了
 *	@retval FALSE 途中
 */
//-----------------------------------------------------------------------------
static BOOL _selectAnimeWait( const C_GEAR_WORK* cpWork,int x,int y )
{
  int i;

  i = x + y * C_GEAR_PANEL_WIDTH;
  if( cpWork->cellSelect[i] ){
    if( GFL_CLACT_WK_CheckAnmActive( cpWork->cellSelect[i] ) ){
      return FALSE;
    }
  }
  return TRUE;
}


static BOOL _modeSelectAnimWait(C_GEAR_WORK* pWork)
{
  int x;
  int y,i;
  int xbuff[6];
  int ybuff[6];

  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH ; x++)
    {
      // 終了チェック
      if( pWork->typeAnim[x][y] == _SELECTANIM_RUN ){
        if( _selectAnimeWait( pWork, x, y ) ){
          pWork->typeAnim[x][y] = _SELECTANIM_END;
        }
      }
      
      // 開始チェック
      if(pWork->typeAnim[x][y] == _SELECTANIM_STANDBY)
      {
        _selectAnimInit(pWork, x,y);
        pWork->typeAnim[x][y] = _SELECTANIM_ANIMING;
      }
    }
  }


  // イベントへジャンプ
  pWork->select_count ++;
  if( pWork->select_count >= _SELECT_ANIME_WAIT ){
    return TRUE;
  }


  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH ; x++)
    {
      // 
      // 広がっていく
      if(pWork->typeAnim[x][y] == _SELECTANIM_ANIMING)
      {
        pWork->typeAnim[x][y] = _SELECTANIM_RUN;

        
        // 自分の周り６つを探す。
        _getHexAround( x,y, xbuff, ybuff );
      
        for( i=0; i<6; i++ ){
          // 範囲内？チェック
          if( (xbuff[i] >= 0) && (xbuff[i] < C_GEAR_PANEL_WIDTH) &&
              (ybuff[i] >= 0) && (ybuff[i] < C_GEAR_PANEL_HEIGHT) ){
            // 設定
            if( pWork->typeAnim[ xbuff[i] ][ ybuff[i] ] == _SELECTANIM_WAIT ){
              pWork->typeAnim[ xbuff[i] ][ ybuff[i] ] = _SELECTANIM_STANDBY;
            }else if( pWork->typeAnim[ xbuff[i] ][ ybuff[i] ] == _SELECTANIM_NONE ){
              pWork->typeAnim[ xbuff[i] ][ ybuff[i] ] = _SELECTANIM_ANIMING;
            }
          }
        }
      }
    }
  }

  return FALSE;
}



//------------------------------------------------------------------------------
/**
 * @brief   選択されたのでアニメを行う
 * @param   x,y          探す場所
 * @param   xbuff,ybuff  探した場所をバッファする
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeSelectAnimInit(C_GEAR_WORK* pWork)
{
  int x,y;
  CGEAR_PANELTYPE_ENUM type;

  type = _cgearSave_GetPanelType(pWork,pWork->touchx,pWork->touchy);
  pWork->select_type = type;
  pWork->select_count = 0;

  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH ; x++)
    {
      if(_cgearSave_GetPanelType(pWork,x,y) == type){
        pWork->typeAnim[x][y] = _SELECTANIM_WAIT;
      }
    }
  }
  pWork->typeAnim[pWork->touchx][pWork->touchy] = _SELECTANIM_STANDBY;
  _modeSelectAnimWait(pWork);
}

//------------------------------------------------------------------------------
/**
 * @brief   タイプに変換
 * @retval  none
 */
//------------------------------------------------------------------------------
static int getTypeToTouchPos(C_GEAR_WORK* pWork,int touchx,int touchy,int *pxp, int* pyp)
{
  int xp,yp;
  int type = CGEAR_PANELTYPE_NONE;

  {  // ギアのタイプ分析
    int ypos[2] = {PANEL_Y1,PANEL_Y2};
    int yloop[2] = {PANEL_HEIDHT1,PANEL_HEIGHT2};
    touchx = touchx / 8;
    touchy = touchy / 8;
    xp = (touchx - PANEL_X1) / PANEL_SIZEXY;
    if(touchy - ypos[xp % 2] < 0){
      return CGEAR_PANELTYPE_MAX;
    }
    yp = (touchy - ypos[xp % 2]) / PANEL_SIZEXY;

    if((xp < C_GEAR_PANEL_WIDTH) && (yp < yloop[ xp % 2 ]))
    {
      type = _cgearSave_GetPanelType(pWork,xp,yp);
    }
  }
  *pxp=xp;
  *pyp=yp;
  return type;
}


//----------------------------------------------------------------------------
/**
 *	@brief  パネルのパレットアニメ処理　初期化
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static void _PanelPaletteAnimeInit( C_GEAR_WORK* pWork )
{
  u32 bit; 
  u32 pltt_beacon;

  bit = _PanelPaletteColorSetUp( pWork );
  pWork->plt_anime.last_bit = bit;  // 光らないようにする。
  if( _PanelPaletteIsBeaconChange( GAMESYSTEM_GetGameCommSysPtr(pWork->pGameSys), pWork->plt_anime.last_bit, bit, &pltt_beacon ) ){
    
    pWork->plt_anime.plt_beacon = pltt_beacon;
    pWork->plt_anime.plt_anmtype = _CGEAR_NET_CHANGEPAL_ANM_TYPE_HIGH;
  }else{
    pWork->plt_anime.plt_beacon = pWork->beacon_bit;
    pWork->plt_anime.plt_anmtype = _CGEAR_NET_CHANGEPAL_ANM_TYPE_NORMAL;
  }

  pWork->plt_anime.last_bit = bit;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パネルのパレットアニメ処理　初期化  スリープ復帰用
 */
//-----------------------------------------------------------------------------
static void _PanelPaletteAnimeInitSleep( C_GEAR_WORK* pWork )
{
  u32 bit; 
  u32 pltt_beacon;

  bit = _PanelPaletteColorSetUp( pWork );
  pWork->plt_anime.last_bit = 0;  // 光るようにする。
  if( _PanelPaletteIsBeaconChange( GAMESYSTEM_GetGameCommSysPtr(pWork->pGameSys), pWork->plt_anime.last_bit, bit, &pltt_beacon ) ){
    
    pWork->plt_anime.plt_beacon = pltt_beacon;
    pWork->plt_anime.plt_anmtype = _CGEAR_NET_CHANGEPAL_ANM_TYPE_HIGH;
  }else{
    pWork->plt_anime.plt_beacon = pWork->beacon_bit;
    pWork->plt_anime.plt_anmtype = _CGEAR_NET_CHANGEPAL_ANM_TYPE_NORMAL;
  }

  pWork->plt_anime.last_bit = bit;
}


//----------------------------------------------------------------------------
/**
 *	@brief  フェードカラーのセットアップ
 *
 *	@param	pWork   ワーク
 */
//-----------------------------------------------------------------------------
static u32 _PanelPaletteColorSetUp( C_GEAR_WORK* pWork )
{
  GAME_COMM_SYS_PTR pGC = GAMESYSTEM_GetGameCommSysPtr(pWork->pGameSys);
  u32 bit = 0;

  pWork->beacon_bit=0;
  pWork->plt_anime.plt_count=0;

  // 次のアニメーション確定処理
  if(Intrude_CheckPalaceConnect(pGC)){
    pWork->beacon_bit |= _CGEAR_NET_BIT_WIRELESS;
    _PaletteSetColType( pWork, _CGEAR_NET_CHANGEPAL_WIRELES, _CGEAR_NET_PALTYPE_GREEN );
  }

  if(!pWork->beacon_bit){
    bit = WIH_DWC_GetAllBeaconTypeBit( GAMEDATA_GetWiFiList(GAMESYSTEM_GetGameData(pWork->pGameSys)) );

    
    /*if(bit & GAME_COMM_SBIT_IRC_ALL){
      pWork->beacon_bit |= _CGEAR_NET_BIT_IR;
    }*/
    if(bit & GAME_COMM_SBIT_WIRELESS_ALL){
      pWork->beacon_bit |= _CGEAR_NET_BIT_WIRELESS;
    }
    if(bit & GAME_COMM_SBIT_WIFI_ALL){
      pWork->beacon_bit |= _CGEAR_NET_BIT_WIFI;
    }

    // IRCカラー
    if(bit & GAME_COMM_STATUS_BIT_IRC){
      _PaletteSetColType( pWork, _CGEAR_NET_CHANGEPAL_IRC, _CGEAR_NET_PALTYPE_RED );
    }
    
    // WIFIカラー
    if(bit & GAME_COMM_STATUS_BIT_WIFI){      // 登録済み
      _PaletteSetColType( pWork, _CGEAR_NET_CHANGEPAL_WIFI, _CGEAR_NET_PALTYPE_GREEN );
    }
    else if(bit & GAME_COMM_STATUS_BIT_WIFI_FREE){  // かぎなし
      _PaletteSetColType( pWork, _CGEAR_NET_CHANGEPAL_WIFI, _CGEAR_NET_PALTYPE_YELLOW );
    }
    else if(bit & GAME_COMM_STATUS_BIT_WIFI_ZONE){  // 任天堂ゾーン　など　公式
      _PaletteSetColType( pWork, _CGEAR_NET_CHANGEPAL_WIFI, _CGEAR_NET_PALTYPE_BLUE );
    }
    else if(bit & GAME_COMM_STATUS_BIT_WIFI_LOCK){  // かぎあり
      _PaletteSetColType( pWork, _CGEAR_NET_CHANGEPAL_WIFI, _CGEAR_NET_PALTYPE_RED );
    }

    // Wirelesカラー
    if(bit & GAME_COMM_STATUS_BIT_WIRELESS_TR){ // トランシーバーは最優先
      _PaletteSetColType( pWork, _CGEAR_NET_CHANGEPAL_WIRELES, _CGEAR_NET_PALTYPE_YELLOW );
    }
    else 
    {
      // トランシーバー以外は順番に
      static const u32 WIRELES_COUNT_MASK[ _CGEAR_NET_WIRELES_TYPE_MAX ] = 
      {
        GAME_COMM_STATUS_BIT_WIRELESS_UN, 
        GAME_COMM_STATUS_BIT_WIRELESS_FU, 
      };
      static const u32 WIRELES_COUNT_COLOR[ _CGEAR_NET_WIRELES_TYPE_MAX ] = 
      {
        _CGEAR_NET_PALTYPE_BLUE, 
        _CGEAR_NET_PALTYPE_RED, 
      };
      int i;

      for( i=0; i<_CGEAR_NET_WIRELES_TYPE_MAX; i++ ){
        pWork->plt_anime.wireles_count = (pWork->plt_anime.wireles_count + 1) % _CGEAR_NET_WIRELES_TYPE_MAX;
        if( bit & WIRELES_COUNT_MASK[ pWork->plt_anime.wireles_count ] ){
          _PaletteSetColType( pWork, _CGEAR_NET_CHANGEPAL_WIRELES, WIRELES_COUNT_COLOR[ pWork->plt_anime.wireles_count ] );
          break;
        }
      }
    }
  }

  return bit;
}

//----------------------------------------------------------------------------
/**
 *	@brief  beaconとbeaconをチェックして、変わった情報を設定
 *
 *	@param	last_bit
 *	@param	bit
 */
//-----------------------------------------------------------------------------
static BOOL _PanelPaletteIsBeaconChange( GAME_COMM_SYS_PTR game_comm, u32 last_bit, u32 bit, u32* change_beacon )
{
  BOOL ret = FALSE;
  
  *change_beacon = 0;

  // 進入最優先
  if( (last_bit != 0) && Intrude_CheckPalaceConnect( game_comm ) ){
    *change_beacon |= _CGEAR_NET_BIT_WIRELESS;
    return TRUE;
  }
  
  /*
  // IRC
  if( (bit & GAME_COMM_STATUS_BIT_IRC) != (last_bit & GAME_COMM_STATUS_BIT_IRC) ){
    *change_beacon |= _CGEAR_NET_BIT_IR;
    ret = TRUE;
  }
  */

  // WIFI
  {
    if( (last_bit & GAME_COMM_STATUS_BIT_WIFI) != (bit & GAME_COMM_STATUS_BIT_WIFI) ){      // 登録済み
      *change_beacon |= _CGEAR_NET_BIT_WIFI;
      ret = TRUE;
    }
    
    if( ((last_bit & GAME_COMM_STATUS_BIT_WIFI) == 0) && // １つ前が消えている状態で・・・
        ((bit & GAME_COMM_STATUS_BIT_WIFI) == 0) && ((*change_beacon & _CGEAR_NET_BIT_WIFI)==0) ){
      
      if( (last_bit & GAME_COMM_STATUS_BIT_WIFI_FREE) != (bit & GAME_COMM_STATUS_BIT_WIFI_FREE) ){ // かぎなし
        *change_beacon |= _CGEAR_NET_BIT_WIFI;
        ret = TRUE;
      }
    }

    if( ((last_bit & GAME_COMM_STATUS_BIT_WIFI_FREE) == 0) && // １つ前が消えている状態で・・・
        ((bit & GAME_COMM_STATUS_BIT_WIFI_FREE) == 0) && ((*change_beacon & _CGEAR_NET_BIT_WIFI)==0)){
      
      if( (last_bit & GAME_COMM_STATUS_BIT_WIFI_ZONE) != (bit & GAME_COMM_STATUS_BIT_WIFI_ZONE) ){ // 任天堂ゾーン　など　公式
        *change_beacon |= _CGEAR_NET_BIT_WIFI;
        ret = TRUE;
      }
    }

    if( ((last_bit & GAME_COMM_STATUS_BIT_WIFI_ZONE) == 0) && // １つ前が消えている状態で・・・
        ((bit & GAME_COMM_STATUS_BIT_WIFI_ZONE) == 0) && ((*change_beacon & _CGEAR_NET_BIT_WIFI)==0)){
      
      if( (last_bit & GAME_COMM_STATUS_BIT_WIFI_LOCK) != (bit & GAME_COMM_STATUS_BIT_WIFI_LOCK) ){ // かぎあり
        *change_beacon |= _CGEAR_NET_BIT_WIFI;
        ret = TRUE;
      }
    }
  }

  // Wireless
  {
    // まずはトランシーバーなら必ず
    if( (bit & GAME_COMM_STATUS_BIT_WIRELESS_TR) ){
      *change_beacon |= _CGEAR_NET_BIT_WIRELESS;
      ret = TRUE;
    }
    if( ((last_bit & GAME_COMM_STATUS_BIT_WIRELESS_TR) == 0) && // １つ前が消えている状態で・・・
        ((bit & GAME_COMM_STATUS_BIT_WIRELESS_TR) == 0) && ((*change_beacon & _CGEAR_NET_BIT_WIRELESS)==0) ){

      if( (bit & (GAME_COMM_STATUS_BIT_WIRELESS|GAME_COMM_STATUS_BIT_WIRELESS_UN|GAME_COMM_STATUS_BIT_WIRELESS_FU)) != 
          (last_bit & (GAME_COMM_STATUS_BIT_WIRELESS|GAME_COMM_STATUS_BIT_WIRELESS_UN|GAME_COMM_STATUS_BIT_WIRELESS_FU)) ){
        *change_beacon |= _CGEAR_NET_BIT_WIRELESS;
        ret = TRUE;
      }
    }
  }

  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  パネルのパレット変更　アップデート処理
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static void _PanelPaletteUpdate( C_GEAR_WORK* pWork )
{
  {
    pWork->plt_anime.plt_count++;
    
    if( _PanelPaletteIsEnd( pWork ) )
    {
      u32 bit;
      u32 plt_beacon;
      
      bit = _PanelPaletteColorSetUp( pWork );
      if( _PanelPaletteIsBeaconChange( GAMESYSTEM_GetGameCommSysPtr(pWork->pGameSys), pWork->plt_anime.last_bit, bit, &plt_beacon ) ){
        pWork->plt_anime.plt_beacon = plt_beacon;
        pWork->plt_anime.plt_anmtype = _CGEAR_NET_CHANGEPAL_ANM_TYPE_HIGH;
      }else{
        pWork->plt_anime.plt_beacon = pWork->beacon_bit;
        pWork->plt_anime.plt_anmtype = _CGEAR_NET_CHANGEPAL_ANM_TYPE_NORMAL;
      }
      pWork->plt_anime.last_bit = bit;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  強制的に終了にする。
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static void _PanelPalette_EXEnd( C_GEAR_WORK* pWork )
{
  int  i;
  for(i = 0 ; i < _CGEAR_NET_CHANGEPAL_MAX; i++)
  {
    _PanelPaletteChangeCore( pWork, i, 0, _CGEAR_NET_CHANGEPAL_ANM_NORMAL_MOD_MAX );
  }
  pWork->plt_anime.plt_count = _CGEAR_NET_CHANGEPAL_ANM_COUNT_MAX[pWork->plt_anime.plt_anmtype] - 1; // 一度リセットカウントを入れるため
}


//------------------------------------------------------------------------------
/**
 * @brief   通信を拾ったら、パレットで点滅させる
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _PanelPaletteChange(C_GEAR_WORK* pWork)
{
  u8 bittype[_CGEAR_NET_CHANGEPAL_MAX]={_CGEAR_NET_BIT_IR,_CGEAR_NET_BIT_WIFI,_CGEAR_NET_BIT_WIRELESS};

  int i;

  for(i = 0 ; i < _CGEAR_NET_CHANGEPAL_MAX; i++)
  {
    if( (pWork->plt_anime.plt_beacon & bittype[ i ]) )
    {

      if( pWork->plt_anime.plt_anmtype == _CGEAR_NET_CHANGEPAL_ANM_TYPE_HIGH ){
        _PanelPaletteChangeHigh( pWork, i );
      }else{
        _PanelPaletteChangeNormal( pWork, i );
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  パネルの色変え処理　HIGH
 */
//-----------------------------------------------------------------------------
static void _PanelPaletteChangeHigh(C_GEAR_WORK* pWork, int change_panel)
{
  int mod;
  int index;
  
  if( pWork->plt_anime.plt_count > _CGEAR_NET_CHANGEPAL_ANM_HIGH_COUNT ){
    return ;
  }

  mod = ((pWork->plt_anime.plt_count) % (_CGEAR_NET_CHANGEPAL_ANM_HIGH_COUNT/2));

  index = (mod * _CGEAR_NET_CHANGEPAL_ANM_MOD_NUM_MAX) / (_CGEAR_NET_CHANGEPAL_ANM_HIGH_COUNT/2);
  
  mod = _CGEAR_NET_CHANGEPAL_ANM_MOD_TBL[ index ];
  _PanelPaletteChangeCore( pWork, change_panel, mod, _CGEAR_NET_CHANGEPAL_ANM_MOD_MAX );
}

//----------------------------------------------------------------------------
/**
 *	@brief  パネルの色変え処理　NORMAL
 */
//-----------------------------------------------------------------------------
static void _PanelPaletteChangeNormal(C_GEAR_WORK* pWork, int change_panel)
{
  int mod;
  int index;

  if( pWork->plt_anime.plt_count > (_CGEAR_NET_CHANGEPAL_ANM_NORMAL_TOPWAIT+_CGEAR_NET_CHANGEPAL_ANM_NORMAL_COUNT) ){
    return ;
  }

  if( pWork->plt_anime.plt_count < _CGEAR_NET_CHANGEPAL_ANM_NORMAL_TOPWAIT  ){
    return ;
  }

  mod = ((pWork->plt_anime.plt_count-_CGEAR_NET_CHANGEPAL_ANM_NORMAL_TOPWAIT) % (_CGEAR_NET_CHANGEPAL_ANM_NORMAL_COUNT));

  index = (mod * _CGEAR_NET_CHANGEPAL_ANM_MOD_NUM_MAX) / _CGEAR_NET_CHANGEPAL_ANM_NORMAL_COUNT;
  
  mod = _CGEAR_NET_CHANGEPAL_ANM_MOD_TBL[ index ];
  _PanelPaletteChangeCore( pWork, change_panel, mod, _CGEAR_NET_CHANGEPAL_ANM_NORMAL_MOD_MAX );
}

static void _PanelPaletteChangeCore( C_GEAR_WORK* pWork, int change_panel, int mod, int mod_max )
{
  int pal;

  for(pal = 0; pal < _CGEAR_NET_CHANGEPAL_NUM; pal++)
  {

    int add,rgb,base;
    int shift;
    pWork->palTrans[change_panel][pal] = 0;
    for(rgb = 0; rgb < 3; rgb++){
      shift = rgb * 5;
      base = (pWork->palBase[change_panel][pal] >> shift) & 0x1f;
      add = ((pWork->palChangeCol[ pWork->palChangeType[change_panel] ][pal] >> shift) & 0x1f) - base;
      pWork->palTrans[change_panel][pal] |= ((((add * mod) / mod_max) + base) & 0x1f)<<(shift);
    }

    // BGPalette
    PaletteWorkSet( pWork->pfade_ptr, &pWork->palTrans[change_panel][pal], FADE_SUB_BG, (16*(pal+_CGEAR_NET_CHANGEPAL_POSY) + _CGEAR_NET_CHANGEPAL_POSX[change_panel]), 2 );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  色かえ動作の完了待ち
 */
//-----------------------------------------------------------------------------
static BOOL _PanelPaletteIsEnd(const C_GEAR_WORK* cpWork )
{
  if( cpWork->plt_anime.plt_count >= _CGEAR_NET_CHANGEPAL_ANM_COUNT_MAX[cpWork->plt_anime.plt_anmtype] ){
    return TRUE;
  }
  return FALSE;
}





static void _PaletteMake(C_GEAR_WORK* pWork,const u16* pltt,int type)
{
  int x=type,y;

  for(y = 0 ; y < _CGEAR_NET_CHANGEPAL_NUM; y++){
    pWork->palChangeCol[x][y]= pltt[ (x*16) + 0xb+y ];
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  パネルのカラータイプを設定
 *
 */
//-----------------------------------------------------------------------------
static void _PaletteSetColType( C_GEAR_WORK* pWork, int panel,int type )
{
  GF_ASSERT( type < _CGEAR_NET_PALTYPE_MAX );
  GF_ASSERT( panel < _CGEAR_NET_CHANGEPAL_NUM );
  pWork->palChangeType[ panel ] = type;
}



//----------------------------------------------------------------------------
/**
 *	@brief  パネルマークアニメシステム　初期化
 *
 *	@param	pWork   
 */
//-----------------------------------------------------------------------------
static void _PanelMarkAnimeSysInit( C_GEAR_WORK* pWork )
{
  int i, j;
  for( i=0; i<C_GEAR_PANEL_WIDTH; i++ ){
    for( j=0; j<C_GEAR_PANEL_HEIGHT; j++ ){
      _PanelMarkAnimeInit( &pWork->panel_mark[i][j], i, j );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  パネルマーク　アニメシステム　メイン
 */
//-----------------------------------------------------------------------------
static void _PanelMarkAnimeSysMain( C_GEAR_WORK* pWork )
{
  int i, j;
  for( i=0; i<C_GEAR_PANEL_WIDTH; i++ ){
    for( j=0; j<C_GEAR_PANEL_HEIGHT; j++ ){
      _PanelMarkAnimeMain( &pWork->panel_mark[i][j], pWork );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  パネルマーク　アニメシステム　メイン
 *
 *	@retval TRUE  アニメ中
 *	@retval FALSE  アニメ完了
 */
//-----------------------------------------------------------------------------
static BOOL _PanelMarkAnimeSysIsAnime( const C_GEAR_WORK* cpWork )
{
  int i, j;
  BOOL result = FALSE;
  for( i=0; i<C_GEAR_PANEL_WIDTH; i++ ){
    for( j=0; j<C_GEAR_PANEL_HEIGHT; j++ ){
      if( _PanelMarkAnimeIsAnime( &cpWork->panel_mark[i][j] ) ){
        result = TRUE;
        break;
      }
    }
  }

  return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief  停止処理
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static void _PanelMarkAnimeSysAllStop( C_GEAR_WORK* pWork )
{
  int i, j;
  for( i=0; i<C_GEAR_PANEL_WIDTH; i++ ){
    for( j=0; j<C_GEAR_PANEL_HEIGHT; j++ ){
      _PanelMarkAnimeStop( &pWork->panel_mark[i][j] );
    }
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  パネルマークのスクリーンパレットアニメーション  初期化
 *
 *	@param	p_mark      マークワーク
 *	@param  x           インデックスｘ
 *	@param  y           インデックスｙ
 */
//-----------------------------------------------------------------------------
static void _PanelMarkAnimeInit( PANEL_MARK_ANIME* p_mark, int x, int y )
{
  GFL_STD_MemClear( p_mark, sizeof(PANEL_MARK_ANIME) );

  p_mark->x = x;
  p_mark->y = y;
}

//----------------------------------------------------------------------------
/**
 *	@brief  OFF状態の設定
 */
//-----------------------------------------------------------------------------
static void _PanelMarkAnimeSetOff( PANEL_MARK_ANIME* p_mark, C_GEAR_WORK* pWork, CGEAR_PANELTYPE_ENUM panel_type )
{
  GF_ASSERT( p_mark->anime_on == FALSE );
  
  // color用のパネルを書き込み。
  p_mark->color = PANEL_COLOR_TYPE_NONE;
  _gearPanelBgScreenMake(pWork, p_mark->x, p_mark->y, panel_type);
  _PanelMarkAnimeWriteScreen( p_mark, 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  パネルマークのスクリーンパレットアニメーション　開始
 *
 *	@param	p_mark        マークワーク
 *	@param	color_type    カラータイプ
 *	@param  panel_type    パネルタイプ
 *	@param	anime_type    アニメタイプ
 */
//-----------------------------------------------------------------------------
static void _PanelMarkAnimeStart( PANEL_MARK_ANIME* p_mark, C_GEAR_WORK* pWork, u8 color_type, u8 anime_type, CGEAR_PANELTYPE_ENUM panel_type, u16 frame )
{
  u32 set_index = 0;
  
  GF_ASSERT( color_type < PANEL_COLOR_TYPE_MAX );
  GF_ASSERT( anime_type < PANEL_ANIME_TYPE_MAX );
  
  p_mark->count       = 0;
  p_mark->color       = color_type;
  p_mark->anime_on    = TRUE;
  p_mark->one_frame   = frame;

  if( anime_type == PANEL_ANIME_TYPE_ON_OFF ){
    p_mark->end_anime_index  = 0;  // アニメーションインデックスの終了値
  }else if( anime_type == PANEL_ANIME_TYPE_ON ){
    p_mark->end_anime_index  = PANEL_COLOR_PATTERN_NUM-1;  // アニメーションインデックスの終了値
  }else if( anime_type == PANEL_ANIME_TYPE_OFF ){
    // 0, 1, 2, 3, 4, 3, 2, 1, 0
    p_mark->end_anime_index  = _PanelMark_GetPatternIndex( pWork, color_type, p_mark->x, p_mark->y );  // アニメーションインデックスの終了値
    set_index                = PANEL_COLOR_PATTERN_NUM-1;
    p_mark->count       = (frame * PANEL_COLOR_PATTERN_NUM);
  }

  // color用のパネルを書き込み。
  _gearPanelBgScreenMake(pWork, p_mark->x, p_mark->y, panel_type);
  _PanelMarkAnimeWriteScreen( p_mark, set_index );
}

//----------------------------------------------------------------------------
/**
 *	@brief  途中でも停止
 *
 *	@param	p_mark
 */
//-----------------------------------------------------------------------------
static void _PanelMarkAnimeStop( PANEL_MARK_ANIME* p_mark)
{
  p_mark->anime_on    = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パネルマークのスクリーンパレットアニメーション　メイン
 *
 *	@param	p_mark      マークワーク
 *	@param	cp_work     ワーク
 */
//-----------------------------------------------------------------------------
static void _PanelMarkAnimeMain( PANEL_MARK_ANIME* p_mark, const C_GEAR_WORK* cp_work )
{
  u32 anime_index;
  BOOL screen_write;
  
  if( p_mark->anime_on ){

    p_mark->count ++;

    anime_index = p_mark->count / p_mark->one_frame;
    screen_write = FALSE;

    
    // アニメインデックスがPANEL_COLOR_PATTERN_NUMまでは、同じ
    if( (anime_index >= PANEL_COLOR_PATTERN_NUM) ){
      anime_index = (PANEL_COLOR_PATTERN_NUM-1) - (anime_index % PANEL_COLOR_PATTERN_NUM);

      // 終了チェック
      if( anime_index == p_mark->end_anime_index ){
        p_mark->anime_on = FALSE;
      }
    }

    if( (anime_index < PANEL_COLOR_PATTERN_NUM) && 
        ((p_mark->count % p_mark->one_frame) == 0) ){
      //スクリーン書き換え
      screen_write = TRUE;
    }

    if( screen_write ){
      _PanelMarkAnimeWriteScreen( p_mark, anime_index );
    }
  }
  
  // 
}

//----------------------------------------------------------------------------
/**
 *	@brief  スクリーンのパレット切り替え
 */
//-----------------------------------------------------------------------------
static void _PanelMarkAnimeWriteScreen( const PANEL_MARK_ANIME* cp_mark, u32 anime_index )
{
  int scrn_x, scrn_y;
  int scrn_size_x;
  u32 pal_index;

  pal_index    = sc_PANEL_COLOR_ANIME_TBL[ cp_mark->color ][ anime_index ];

  _gearXY2PanelScreen(cp_mark->x, cp_mark->y, &scrn_x, &scrn_y );

  scrn_size_x = PANEL_SIZEXY;
  if( scrn_x < 0 ){
    scrn_size_x = PANEL_SIZEXY + scrn_x;
    scrn_x = 0;
  }else if( (scrn_x + PANEL_SIZEXY) > 32 ){
    scrn_size_x = 32 - scrn_x;
  }
  
  GFL_BG_ChangeScreenPalette( GEAR_BUTTON_FRAME, 
      scrn_x, scrn_y, scrn_size_x, PANEL_SIZEXY, pal_index );

  GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );
}


//----------------------------------------------------------------------------
/**
 *	@brief  アニメーション中チェック
 *
 *	@param	cp_mark 
 */
//-----------------------------------------------------------------------------
static BOOL _PanelMarkAnimeIsAnime( const PANEL_MARK_ANIME* cp_mark )
{
  return cp_mark->anime_on;
}

//----------------------------------------------------------------------------
/**
 *	@brief  終点カラーを取得
 *
 *	@param	cpWork      ワーク
 *	@param	col_type    カラータイプ
 *	@param  x           xインデックス
 *	@param  y           yインデックス
 */
//-----------------------------------------------------------------------------
static int _PanelMark_GetPatternIndex( const C_GEAR_WORK* cpWork, u32 col_type, int x, int y )
{
  if( _cgearSave_IsPanelTypeIcon( cpWork, x, y ) ){
    return sc_PANEL_COLOR_ANIME_END_INDEX[ col_type ][ 4 ];
  }

  return sc_PANEL_COLOR_ANIME_END_INDEX[ col_type ][ x ];
}




//------------------------------------------------------------------------------
/**
 * @brief   ギアｘｙをスクリーンに変換
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _gearXY2PanelScreen(int x,int y, int* px, int* py)
{
  int ypos[2] = {PANEL_Y1,PANEL_Y2};
  *px = PANEL_X1 + x * PANEL_SIZEXY;
  *py = ypos[ x % 2 ] + y * PANEL_SIZEXY;
}


//----------------------------------------------------------------------------
/**
 *	@brief  サブ画面フェード完了まち
 *
 *	@param	cpWork 
 *
 *	@retval TRUE  フェード完了
 *	@retval FALSE  フェード途中
 */
//-----------------------------------------------------------------------------
static BOOL _modeSelectWaitSubFadeEnd( const C_GEAR_WORK* cpWork )
{
  if( WIPE_SYS_EndCheck() == FALSE ){
    return FALSE;
  }

  // 輝度チェック
  if( GXS_GetMasterBrightness() != 0 ){
    return FALSE;
  }

  {
    FIELDMAP_WORK* p_fieldmap = GAMESYSTEM_GetFieldMapWork( cpWork->pGameSys );
    if( FIELDMAP_CheckSeasonDispFlag(p_fieldmap) ){
      return FALSE;
    }
  }

  return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   パネルの数を取得
 * @retval  none
 */
//------------------------------------------------------------------------------

static int _gearPanelTypeNum(C_GEAR_WORK* pWork, CGEAR_PANELTYPE_ENUM type)
{
  int x,y,i;

  if(type == CGEAR_PANELTYPE_BASE){
    return 1;
  }
  i=0;
  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH; x++)
    {
      if(_cgearSave_GetPanelType(pWork,x,y)==type)
      {
        i++;
      }
    }
  }
  return i;
}


//----------------------------------------------------------------------------
/**
 *	@brief  パレットフェード基準となる　フェード終了値を設定
 *
 *	@param	pWork
 *	@param	comm 
 */
//-----------------------------------------------------------------------------
static void _PFadeSetDefaultPal( C_GEAR_WORK* pWork, BOOL comm )
{
  int i, j;
  u16* p_buff;
  u16* p_trans;
  if( !comm ){
    // 基準値を暗めにする。
    p_buff = PaletteWorkDefaultWorkGet( pWork->pfade_ptr, FADE_SUB_OBJ );
    p_trans = PaletteWorkTransWorkGet( pWork->pfade_ptr, FADE_SUB_OBJ );
    for( i=0; i<OAM_USE_PLTT_NUM; i++ ){
      if( (1<<i) & OAM_PFADE_NORMAL_MSK ){
        // 16こかえる
        SoftFade( &p_buff[ 16*i ], &p_buff[ 16*i ], 16, 8, _BLACK_COLOR[pWork->sex] );
        GFL_STD_MemCopy( &p_buff[ 16*i ], &p_trans[ 16*i ], 2*16 );
      }
    }

    p_buff = PaletteWorkDefaultWorkGet( pWork->pfade_ptr, FADE_SUB_BG );
    p_trans = PaletteWorkTransWorkGet( pWork->pfade_ptr, FADE_SUB_BG );
    for( i=0; i<OAM_USE_PLTT_NUM; i++ ){
      if( (1<<i) & BG_PFADE_NORMAL_MSK ){
        // 16こかえる
        SoftFade( &p_buff[ 16*i ], &p_buff[ 16*i ], 16, 8, _BLACK_COLOR[pWork->sex] );
        GFL_STD_MemCopy( &p_buff[ 16*i ], &p_trans[ 16*i ], 2*16 );
      }
    }
  }
}

static void _PFadeSetBlack( C_GEAR_WORK* pWork )
{
  // 黒く
  PaletteFadeReq(
    pWork->pfade_ptr, PF_BIT_SUB_OBJ, 0xffff,   -120, 0, 16, _BLACK_COLOR[pWork->sex], pWork->pfade_tcbsys
    );
}

//----------------------------------------------------------------------------
/**
 *	@brief  スリープブラック
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static void _PFadeSetSleepBlack( C_GEAR_WORK* pWork, BOOL on_flag )
{
  int evy;
  if(on_flag){
    evy = 8;
  }else{
    evy = 0;
  }
  // 黒く
  PaletteFadeReq(
    pWork->pfade_ptr, PF_BIT_SUB_OBJ, 0xffff,   -120, 0, evy, _BLACK_COLOR[pWork->sex], pWork->pfade_tcbsys
    );

  // 黒く
  PaletteFadeReq(
    pWork->pfade_ptr, PF_BIT_SUB_BG, BG_PFADE_NORMAL_MSK,   -120, 0, evy, _BLACK_COLOR[pWork->sex], pWork->pfade_tcbsys
    );
}

static void _PFadeToBlack( C_GEAR_WORK* pWork )
{
  // 黒く
  PaletteFadeReq(
    pWork->pfade_ptr, PF_BIT_SUB_OBJ, 0xffff,  1, 0, 16, _BLACK_COLOR[pWork->sex], pWork->pfade_tcbsys
    );
}

static void _PFadeFromBlack( C_GEAR_WORK* pWork )
{
  // 黒く
  PaletteFadeReq(
    pWork->pfade_ptr, PF_BIT_SUB_OBJ, 0xffff,  1, 16, 0, _BLACK_COLOR[pWork->sex], pWork->pfade_tcbsys
    );
}

static BOOL _PFadeIsFade( const C_GEAR_WORK* cpWork )
{
  return PaletteFadeCheck( cpWork->pfade_ptr );
}



static void _gearBootInitScreen(C_GEAR_WORK* pWork)
{
  int i;
  int x , y;
  int yloop[2] = {PANEL_HEIDHT1,PANEL_HEIGHT2};

  for(x = 0; x < PANEL_WIDTH; x++){   // XはPANEL_WIDTH回
    for(y = 0; y < yloop[ x % 2]; y++){ //Yは xの％２でyloopの繰り返し
      _gearPanelBgScreenMake(pWork, x, y, CGEAR_PANELTYPE_NONE);
    }
  }
  GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );
}



//----------------------------------------------------------------------------
/**
 *	@brief  スリープ復帰アニメ
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL _gearBootMain(C_GEAR_WORK* pWork)
{
  int i;
  int x , y;
  int yloop[2] = {PANEL_HEIDHT1,PANEL_HEIGHT2};

  switch( pWork->state_seq ){
  case BOOT_ANIME_SEQ_ALL_ON:
    // まず全体を点等
    for(x = 0; x < PANEL_WIDTH; x++){   // XはPANEL_WIDTH回
      for(y = 0; y < yloop[ x % 2]; y++){ //Yは xの％２でyloopの繰り返し
        _PanelMarkAnimeStart( &pWork->panel_mark[ x ][ y ], pWork, PANEL_COLOR_TYPE_BASE, PANEL_ANIME_TYPE_ON, CGEAR_PANELTYPE_BASE, PANEL_ANIME_DEF_FRAME );
      }
    }
    pWork->state_seq ++;
    break;

  case BOOT_ANIME_SEQ_ALL_WAIT:
    if( _PanelMarkAnimeSysIsAnime( pWork ) == FALSE ){

      pWork->state_seq ++;
    }
    break;

  case BOOT_ANIME_SEQ_SMOOTHIN_MAIN:
    {
      u32 panel_type;
      for(i=0;i < elementof(screenTable);i++){
        if(screenTable[i].time == pWork->startCounter){
          panel_type = _cgearSave_GetPanelType( pWork, screenTable[i].x, screenTable[i].y );
          _PanelMarkAnimeStart( &pWork->panel_mark[ screenTable[i].x ][ screenTable[i].y ], 
              pWork, sc_PANEL_TYPE_TO_COLOR[ panel_type ], PANEL_ANIME_TYPE_OFF, panel_type, screenTable[i].frame );
        }
      }

      if( pWork->startCounter >= screenTable[ NELEMS(screenTable)-1 ].time ){
        _PFadeFromBlack(pWork);
        _gearMarkObjDrawEnable(pWork,TRUE);
        pWork->state_seq ++;
      }

      // ずっとカウントアップ
      pWork->startCounter++;
    }
    break;
    
  case BOOT_ANIME_SEQ_END:
    if( (_PanelMarkAnimeSysIsAnime( pWork ) == FALSE) && (_PFadeIsFade( pWork ) == FALSE) ){
      return TRUE;
    }
    break;

  default:
    break;
  }


  // パネルアニメのシステムメイン
  _PanelMarkAnimeSysMain( pWork );

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  スタートアップ　スクリーンアニメ　メイン
 *
 *	@param	pWork
 */
//-----------------------------------------------------------------------------
static BOOL _gearStartUpMain(C_GEAR_WORK* pWork)
{
  int i;


  if( START_UP_SCREEN_ANIME_FRAME_MAX >= pWork->startCounter ){
    // 徐々に枠が増えていく
    for(i=0;i < elementof(StartUpScreenTable);i++){
      if(StartUpScreenTable[i].time == pWork->startCounter) {
        _PanelMarkAnimeStart( &pWork->panel_mark[ StartUpScreenTable[i].x ][ StartUpScreenTable[i].y ], pWork, PANEL_COLOR_TYPE_BASE, PANEL_ANIME_TYPE_ON_OFF, CGEAR_PANELTYPE_BASE, StartUpScreenTable[i].frame );
        
        //_gearPanelBgScreenMake(pWork, StartUpScreenTable[i].x, StartUpScreenTable[i].y, 
        //    CGEAR_PANELTYPE_BOOT, FALSE);
      }
    }
    pWork->startCounter++;
  }else{
    if( _PanelMarkAnimeSysIsAnime( pWork ) == FALSE ){
      return TRUE;
    }
  }



  // パネルアニメのシステムメイン
  _PanelMarkAnimeSysMain( pWork );

  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  点灯させたテーブルを消していく。
 *
 *	@param	pWork
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void _gearStartUpAllOff(C_GEAR_WORK* pWork)
{
  int x, y;
  int yloop[2] = {PANEL_HEIDHT1,PANEL_HEIGHT2};
  
  // まず全体を点等
  for(x = 0; x < C_GEAR_PANEL_WIDTH; x++){   // XはPANEL_WIDTH回
    for(y = 0; y < yloop[ x % 2]; y++){ //Yは xの％２でyloopの繰り返し
      _PanelMarkAnimeSetOff( &pWork->panel_mark[ x ][ y ], pWork, CGEAR_PANELTYPE_BASE );
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   パネルタイプをスクリーンに書き込む
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _gearPanelBgScreenMake(C_GEAR_WORK* pWork,int xs,int ys, CGEAR_PANELTYPE_ENUM type)
{
  int ypos[2] = {PANEL_Y1,PANEL_Y2};
  int x,y,i,j;
  int typepos[] = {0x18, 0x0c,0x10,0x14,0x18,0x1c};
  u16* pScrAddr = GFL_BG_GetScreenBufferAdrs(GEAR_BUTTON_FRAME );
  int xscr;
  int yscr;
  int col_index;
  int col_type;

  _gearXY2PanelScreen(xs,ys,&xscr,&yscr);


  // パネルタイプとxインデックスからパレットナンバーを取得
  col_type = sc_PANEL_TYPE_TO_COLOR[ type ];
  col_index = _PanelMark_GetPatternIndex( pWork, col_type, xs, ys );

  for(y = yscr, i = 0; i < PANEL_SIZEXY; y++, i++){
    for(x = xscr, j = 0; j < PANEL_SIZEXY; x++, j++){
      if((x >= 0) && (x < 32)){
        int charpos = typepos[type] + i * 0x20 + j;
        int scr = x + (y * 32);

        
        pScrAddr[scr] = (sc_PANEL_COLOR_ANIME_TBL[col_type][ col_index ]<<12) | charpos;
        //		NET_PRINT("x%d y%d  %d \n",x,y,palpos[type] + charpos);
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ギアセーブデータをみて、文字を出す良い場所を返す
 * @retval  見つからなかったらFALSE
 */
//------------------------------------------------------------------------------

static BOOL _gearGetTypeBestPosition(C_GEAR_WORK* pWork,CGEAR_PANELTYPE_ENUM type, int* px, int* py)
{
  int x,y;
  int first_x, first_y;
  BOOL first_data;

  first_x = 0;
  first_y = 0;
  first_data = FALSE;

  for(y = 0; y < C_GEAR_PANEL_HEIGHT; y++)
  {
    for(x = 0; x < C_GEAR_PANEL_WIDTH; x++)
    {
      if(_cgearSave_GetPanelType(pWork,x,y) == type)
      {
        // 最初にあった位置を覚えておく。
        if( first_data == FALSE ){
          first_x = x;
          first_y = y;
          first_data = TRUE;
        }

        // 
        if( _cgearSave_IsPanelTypeIcon(pWork,x,y) ){ // アイコンが出る場所
          _gearXY2PanelScreen(x,y,px,py);
          return TRUE;
        }

        // 
        if( _cgearSave_IsPanelTypeLast(pWork,x,y,type) ){ // アイコンが出る場所
          first_x = x;
          first_y = y;
          first_data = TRUE;
        }
      }
    }
  }
  
  if( first_data ){
    // もしなかったら、最初に見つかった位置にする。
    _gearXY2PanelScreen(first_x,first_y,px,py);
    CGEAR_SV_SetPanelType( pWork->pCGSV, first_x, first_y, type, TRUE, FALSE );
    _gearPanelBgScreenMake(pWork, first_x, first_y, type );
    GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );
    return TRUE;
  }

  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ギアのパネルをセーブデータにしたがって作る
 * @retval  none
 */
//------------------------------------------------------------------------------


static void _gearPanelBgCreate(C_GEAR_WORK* pWork)
{
  int x , y;
  int yloop[2] = {PANEL_HEIDHT1,PANEL_HEIGHT2};
  int type;

  for(x = 0; x < PANEL_WIDTH; x++){   // XはPANEL_WIDTH回
    for(y = 0; y < yloop[ x % 2]; y++){ //Yは xの％２でyloopの繰り返し

      _gearPanelBgScreenMake(pWork, x, y, _cgearSave_GetPanelType(pWork,x,y));
    }
  }
  GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );
}


//------------------------------------------------------------------------------
/**
 * @brief   ギアのARCを読み込み
 * @retval  none
 */
//------------------------------------------------------------------------------



//static u32 _objpal[]={NARC_c_gear_c_gear_obj_NCLR,NARC_c_gear_c_gear2_obj_NCLR,NARC_c_gear_c_gear_obj_NCLR};
//static u32 _objcgx[]={NARC_c_gear_c_gear_obj_NCGR,NARC_c_gear_c_gear2_obj_NCGR,NARC_c_gear_c_gear_obj_NCGR};

static void _gearArcCreate(C_GEAR_WORK* pWork, int sex, u32 bgno, BOOL pal_trans)
{
  u32 scrno=0;

  if( pal_trans ){

    GFL_ARCHDL_UTIL_TransVramPalette( pWork->handle, _bgpal[ sex ],
                                      PALTYPE_SUB_BG, 0, 0,  HEAPID_FIELD_SUBSCREEN);


    {
      int x,y, i;
      void* buff;
      NNSG2dPaletteData* pltt;
      u16* pltt_data;

      buff = GFL_ARCHDL_UTIL_LoadPalette( pWork->handle, _bgpal[ sex ], &pltt, GFL_HEAP_LOWID(HEAPID_FIELD_SUBSCREEN) );

      pltt_data = pltt->pRawData;
      for(y = 0 ; y < _CGEAR_NET_CHANGEPAL_NUM; y++){
        for(x = 0 ; x < _CGEAR_NET_CHANGEPAL_MAX; x++){
          pWork->palBase[x][y] = pltt_data[(16*(_CGEAR_NET_CHANGEPAL_POSY+y)) + _CGEAR_NET_CHANGEPAL_POSX[x] ];
        }
      }
      GFL_HEAP_FreeMemory( buff );

      buff = GFL_ARCHDL_UTIL_LoadPalette( pWork->handle, NARC_c_gear_c_gear_obj_ani_NCLR, &pltt, GFL_HEAP_LOWID(HEAPID_FIELD_SUBSCREEN) );

      for( i=0; i<_CGEAR_NET_PALTYPE_MAX; i++ ){
        _PaletteMake(pWork,pltt->pRawData,i);
      }

      GFL_HEAP_FreeMemory( buff );
   
    }
  }




  // サブ画面BGキャラ転送
  pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( pWork->handle, _bgcgx[bgno][sex],
                                                                GEAR_MAIN_FRAME, 0, FALSE, HEAPID_FIELD_SUBSCREEN);

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(pWork->handle,
                                         NARC_c_gear_c_gear01_NSCR,
                                         GEAR_MAIN_FRAME, 0,
                                         GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                         HEAPID_FIELD_SUBSCREEN);



}



static void _gearObjResCreate(C_GEAR_WORK* pWork, int sex)
{
  u32 scrno=0;



  pWork->objRes[_CLACT_PLT] = GFL_CLGRP_PLTT_Register( pWork->handle,
                                                       _cellpal[sex],
                                                       CLSYS_DRAW_SUB , 0 , HEAPID_FIELD_SUBSCREEN );
  pWork->objRes[_CLACT_CHR] = GFL_CLGRP_CGR_Register( pWork->handle,
                                                      NARC_c_gear_c_gear_obj_NCGR ,
                                                      FALSE , CLSYS_DRAW_SUB , HEAPID_FIELD_SUBSCREEN );

  pWork->objRes[_CLACT_ANM] = GFL_CLGRP_CELLANIM_Register( pWork->handle,
                                                           NARC_c_gear_c_gear_obj_NCER ,
                                                           NARC_c_gear_c_gear_obj_NANR ,
                                                           pWork->heapID );


  // ＣｒｏｓｓＯｂｊカラー取得
  {
    void * p_buff;
    NNSG2dPaletteData* p_pltt;
    int i;
    u32* p_pltdata;

    p_buff = GFL_ARCHDL_UTIL_LoadPalette( pWork->handle, NARC_c_gear_c_gear_obj_favorit_NCLR, &p_pltt, pWork->heapID );


    // 色を保存
    p_pltdata = (u32*)p_pltt->pRawData;
    for( i=0; i<CROSS_COLOR_MAX; i++ ){
      pWork->crossColor[i] = p_pltdata[ sc_CROSS_COLOR_RES_IDX[i] ];
    }

    GFL_HEAP_FreeMemory( p_buff );
  }


  GFL_NET_WirelessIconEasy_HoldLCD(FALSE, pWork->heapID);
  GFL_NET_ChangeIconPosition(240-22,14);
  GFL_NET_SetIconForcePosition(240-22,14);
  GFL_NET_ReloadIcon();
}





static void _gearDecalScreenArcCreate(C_GEAR_WORK* pWork,BOOL bDecal)
{
  u32 scrno=0;

  if(bDecal){
    scrno = NARC_c_gear_c_gear00_NSCR;
  }
  else{
    scrno = NARC_c_gear_c_gear00_nodecal_NSCR;
  }

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(pWork->handle,
                                         scrno,
                                         GEAR_BMPWIN_FRAME, 0,
                                         GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                         HEAPID_FIELD_SUBSCREEN);
}


//------------------------------------------------------------------------------
/**
 * @brief   サブ画面の設定
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _createSubBg(C_GEAR_WORK* pWork)
{
  int i = 0;
  for(i = GFL_BG_FRAME0_S;i <= GFL_BG_FRAME3_S ; i++)
  {
    GFL_BG_SetVisible( i, VISIBLE_OFF );
  }
  {
    int frame = GEAR_MAIN_FRAME;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_SetPriority( frame, 3 );
    //  GFL_BG_FillCharacter( frame, 0x00, 1, 0 );

    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GEAR_BMPWIN_FRAME;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_SetPriority( frame, 1 );

    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }

  {
    int frame = GEAR_BUTTON_FRAME;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_SetPriority( frame, 2 );

    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }

  // アルファ値設定
  _setUpSubAlpha( pWork );
}

//----------------------------------------------------------------------------
/**
 *	@brief  サブ面ALPHA設定
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static void _setUpSubAlpha( C_GEAR_WORK* pWork )
{
  G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_OBJ, 9, 16 );
}



//------------------------------------------------------------------------------
/**
 * @brief   受け取った数のウインドウを等間隔に作る 幅は3char
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _buttonWindowCreate(int num,int* pMsgBuff,C_GEAR_WORK* pWork)
{
}


//----------------------------------------------------------------------------
/**
 *	@brief	ボタンイベントコールバック  GFL_BMN_EVENT_TOUCHのみ
 *
 *	@param	bttnid		ボタンID
 *	@param	event		イベント種類
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------

static void _touchFunction(C_GEAR_WORK *pWork, int bttnid)
{
  u32 touchx,touchy;

  // 起動してないなら、電源いがい触れない。
  if( pWork->power_flag == FALSE ){
    if( (bttnid != TOUCH_LABEL_POWER) &&
        (bttnid != TOUCH_LABEL_ALL) ){
      return ;
    }
  }

  switch(bttnid){
  case TOUCH_LABEL_ALL:
    if(GFL_UI_TP_GetPointCont(&touchx,&touchy)){
      pWork->tpx = touchx;
      pWork->tpy = touchy;
    }
    pWork->cellMoveCreateCount = 0;
    break;
  case TOUCH_LABEL_EDIT:
    PMSND_PlaySE( SEQ_SE_MSCL_07 );
    pWork->bPanelEdit = pWork->bPanelEdit ^ 1;
    _editMarkONOFF(pWork, pWork->bPanelEdit);
    break;
  case TOUCH_LABEL_HELP:
    pWork->createEvent = FIELD_SUBSCREEN_ACTION_CGEAR_HELP;
    break;
  case TOUCH_LABEL_CROSS:
    pWork->createEvent = FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW;
    break;
  case TOUCH_LABEL_RADAR:
    pWork->createEvent = FIELD_SUBSCREEN_ACTION_SCANRADAR;
    break;

  case TOUCH_LABEL_LOGO:
    PMSND_PlaySE( SEQ_SE_MSCL_07 );

    GFL_BG_FreeCharacterArea(GEAR_MAIN_FRAME,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar),
                             GFL_ARCUTIL_TRANSINFO_GetSize(pWork->subchar));

//    _arcGearRelease(pWork);

    pWork->bgno++;
    pWork->bgno = pWork->bgno % _CGEAR_TYPE_PATTERN_NUM;

    CGEAR_SV_SetCGearType( pWork->pCGSV, pWork->bgno );

    _gearArcCreate(pWork, pWork->sex, pWork->bgno, FALSE);  //ARC読み込み BG&OBJ
    _gearPanelBgCreate(pWork);	// パネル作成
  //  _gearObjCreate(pWork); //CLACT設定
    //_gearCrossObjCreate(pWork);
    _gearMarkObjDrawEnable(pWork,TRUE);

    break;

  case TOUCH_LABEL_POWER:
    pWork->createEvent = FIELD_SUBSCREEN_ACTION_CGEAR_POWER;
    break;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	ボタンイベントコールバック
 *
 *	@param	bttnid		ボタンID
 *	@param	event		イベント種類
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  C_GEAR_WORK *pWork = p_work;
  u32 friendNo;
  u32 touchx,touchy;
  int xp,yp;
  int type = CGEAR_PANELTYPE_NONE;

  if( SleepMode_IsSleep( pWork ) ){
    return;
  }

  switch( event ){
  case GFL_BMN_EVENT_TOUCH:
    _touchFunction(pWork,bttnid);
    break;
  case GFL_BMN_EVENT_HOLD:
    if(pWork->bPanelEdit){
      if(GFL_UI_TP_GetPointCont(&touchx,&touchy)){
        pWork->tpx=touchx;
        pWork->tpy=touchy;
      }
      if(pWork->cellMove){
        GFL_CLACTPOS pos;
        pos.x = pWork->tpx;  // OBJ表示の為の補正値
        pos.y = pWork->tpy;
        GFL_CLACT_WK_SetPos(pWork->cellMove, &pos, CGEAR_REND_SUB);
      }
      else if(pWork->cellMoveCreateCount > 20){
        GFL_CLWK_DATA cellInitData;
        int type = getTypeToTouchPos(pWork,touchx,touchy,&xp,&yp);
        if(type != CGEAR_PANELTYPE_MAX){
          pWork->cellMoveType = type;
        }
        if(pWork->cellMoveType != CGEAR_PANELTYPE_NONE)
        {

          //セルの生成
          cellInitData.pos_x = pWork->tpx;
          cellInitData.pos_y = pWork->tpy;
          cellInitData.anmseq = NANR_c_gear_obj_CellAnime01 + pWork->cellMoveType - 1;
          cellInitData.softpri = 0;
          cellInitData.bgpri = CGEAR_CLACT_BG_PRI;
          pWork->cellMove = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                 pWork->objRes[_CLACT_CHR],
                                                 pWork->objRes[_CLACT_PLT],
                                                 pWork->objRes[_CLACT_ANM],
                                                 &cellInitData ,
                                                 CGEAR_REND_SUB,
                                                 pWork->heapID );
          GFL_CLACT_WK_SetDrawEnable( pWork->cellMove, TRUE );
          GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellMove, TRUE );
        }
      }
      else{
        pWork->cellMoveCreateCount++;
      }
    }
    break;

  case GFL_BMN_EVENT_RELEASE:		///< 離された瞬間

    touchx=pWork->tpx;
    touchy=pWork->tpy;
    pWork->tpx=0;
    pWork->tpy=0;
    type = getTypeToTouchPos(pWork,touchx,touchy,&xp,&yp);  // ギアのタイプ分析
    if(type == CGEAR_PANELTYPE_MAX){
      return;
    }

    if(pWork->cellMove){
      GFL_CLACT_WK_Remove( pWork->cellMove );
      pWork->cellMove=NULL;

      if((type == CGEAR_PANELTYPE_BASE) ||(  _gearPanelTypeNum(pWork,type) > 1 && _isSetChip(xp,yp)))  //下にあるタイプが一個以上ある場合
      {
        type = pWork->cellMoveType;
        _modeSetSavePanelType(pWork, pWork->pCGSV,xp,yp,type);
        GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );
      }
      return;
    }


    if(pWork->bPanelEdit)  ///< パネルタイプを変更
    {
      if(((_gearPanelTypeNum(pWork,type) > 1 ) || (type == CGEAR_PANELTYPE_BASE))&& _isSetChip(xp,yp))
      {
        type = (type+1) % CGEAR_PANELTYPE_MAX;
        _modeSetSavePanelType(pWork, pWork->pCGSV,xp,yp,type);
        GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );
        PMSND_PlaySE( SEQ_SE_MSCL_07 );
      }
    }
    else{    ///< ギアメニューを変更
      pWork->touchx = xp;
      pWork->touchy = yp;

      if( !SleepMode_IsSleep( pWork ) ){  //通信ONなら
        u32 type = _cgearSave_GetPanelType(pWork,pWork->touchx,pWork->touchy);
        
        switch( type ){
        case CGEAR_PANELTYPE_IR:
          pWork->createEvent = FIELD_SUBSCREEN_ACTION_IRC;
          break;
        case CGEAR_PANELTYPE_WIFI:
          pWork->createEvent = FIELD_SUBSCREEN_ACTION_GSYNC;
          break;
        case CGEAR_PANELTYPE_WIRELESS:
          pWork->createEvent = FIELD_SUBSCREEN_ACTION_WIRELESS;
          break;
        }
      }
    }
    break;

  default:
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   エディットモードのONOFF
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _editMarkONOFF(C_GEAR_WORK* pWork,BOOL bOn)
{
  if(bOn){
    GFL_CLACT_WK_SetAnmIndex( pWork->cellCursor[_CLACT_EDITMARKON], 0 );
    GFL_CLACT_WK_SetAutoAnmSpeed( pWork->cellCursor[_CLACT_EDITMARKON], 2*FX32_ONE );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellCursor[_CLACT_EDITMARKON], TRUE );
  }
  else{
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellCursor[_CLACT_EDITMARKON], FALSE );
    GFL_CLACT_WK_SetAnmIndex( pWork->cellCursor[_CLACT_EDITMARKON], 1 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   OBJの初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _gearObjCreate(C_GEAR_WORK* pWork)
{
  int i;
  GFL_CLWK_DATA cellInitData;
  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);


  for(i=0;i < _CLACT_TIMEPARTS_MAX ;i++)
  {
    static u8 anmbuff[]=
    {
      NANR_c_gear_obj_CellAnime_ampm,
      NANR_c_gear_obj_CellAnime_NO2,
      NANR_c_gear_obj_CellAnime_NO10a,
      NANR_c_gear_obj_CellAnime_colon,
      NANR_c_gear_obj_CellAnime_NO6,
      NANR_c_gear_obj_CellAnime_NO10b,
      NANR_c_gear_obj_CellAnime_batt1,
      NANR_c_gear_obj_CellAnime_loro_all,
      NANR_c_gear_obj_CellAnime_help,
      NANR_c_gear_obj_CellAnime_cus_on,
      NANR_c_gear_obj_CellAnime_on_off,
      };
    static u8 xbuff[]=
    {
      63,      42-10,      48-10,      52-10,      57-10,      63-10,
      178,      128,
      POS_HELPMARK_X,      POS_EDITMARK_X,  POS_POWERMARK_X,  };
    static u8 ybuff[]=
    {
      22,   22,
      22,   22,
      22,   22,
      22,   22,
      POS_HELPMARK_Y,
      POS_EDITMARK_Y,
      POS_POWERMARK_Y,
    };

    //セルの生成
    cellInitData.pos_x = xbuff[i];
    cellInitData.pos_y = ybuff[i];
    cellInitData.anmseq = anmbuff[i];
    if(NANR_c_gear_obj_CellAnime_batt1==cellInitData.anmseq){
      if( OS_IsRunOnTwl() ){//DSIなら
        cellInitData.anmseq = NANR_c_gear_obj_CellAnime_batt2;
      }
    }

    cellInitData.softpri = 0;
    cellInitData.bgpri = CGEAR_CLACT_BG_PRI;
    //↑矢印
    pWork->cellCursor[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                pWork->objRes[_CLACT_CHR],
                                                pWork->objRes[_CLACT_PLT],
                                                pWork->objRes[_CLACT_ANM],
                                                &cellInitData ,
                                                CGEAR_REND_SUB,
                                                pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCursor[i], FALSE );
  }
  _editMarkONOFF(pWork,FALSE);

  for(i=0;i < _CLACT_TYPE_MAX ;i++)
  {
    static int anmbuff[]=
    {
      NANR_c_gear_obj_CellAnime_IR,
      NANR_c_gear_obj_CellAnime_WIRELESS,
      NANR_c_gear_obj_CellAnime_WIFI,
    };

    //セルの生成

    cellInitData.pos_x = 8;
    cellInitData.pos_y = 8;
    cellInitData.anmseq = anmbuff[i];
    cellInitData.softpri = 0;
    cellInitData.bgpri = CGEAR_CLACT_BG_PRI;
    //↑矢印
    pWork->cellType[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                              pWork->objRes[_CLACT_CHR],
                                              pWork->objRes[_CLACT_PLT],
                                              pWork->objRes[_CLACT_ANM],
                                              &cellInitData ,
                                              CGEAR_REND_SUB,
                                              pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellType[i], FALSE );
  }
  _timeAnimation(pWork);
  _typeAnimation(pWork);
  
}


//------------------------------------------------------------------------------
/**
 * @brief   マーカーの表示許可
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _gearMarkObjDrawEnable(C_GEAR_WORK* pWork,BOOL bFlg)
{
  int i;
  for(i=0;i < _CLACT_TYPE_MAX ;i++)
  {
    GFL_CLACT_WK_SetDrawEnable( pWork->cellType[i], bFlg );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  全OAMの表示許可
 *
 *	@param	pWork
 *	@param	bFlg
 */
//-----------------------------------------------------------------------------
static void _gearAllObjDrawEnabel(C_GEAR_WORK* pWork,BOOL bFlg)
{
  int i;

  // 選択時アニメ用
  for(i=0;i < C_GEAR_PANEL_WIDTH * C_GEAR_PANEL_HEIGHT;i++)
  {
    if( pWork->cellSelect[i] ){
      GFL_CLACT_WK_SetDrawEnable( pWork->cellSelect[i], bFlg );
    }
  }
  
  // タイプ表示用
  for(i=0;i < _CLACT_TYPE_MAX ;i++)
  {
    if( pWork->cellType[i] && pWork->power_flag ){
      GFL_CLACT_WK_SetDrawEnable( pWork->cellType[i], bFlg );
    }
  }

  // カーソル
  for(i=0;i < _CLACT_TIMEPARTS_MAX;i++)
  {
    if( pWork->cellCursor[i] ){
      if( ( (i == _CLACT_HELP) || (i == _CLACT_EDITMARKON) ) ){
        if( pWork->power_flag ){
          GFL_CLACT_WK_SetDrawEnable( pWork->cellCursor[i], bFlg );
        }
      }else{
        GFL_CLACT_WK_SetDrawEnable( pWork->cellCursor[i], bFlg );
      }
    }
  }


  // すれ違いよう
  if( pWork->cellCross && pWork->power_flag ){
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCross, bFlg );
  }
  if( pWork->cellCrossBase && pWork->power_flag ){
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCrossBase, bFlg );
  }

  // レーダー
  if( pWork->cellRadar && pWork->power_flag ){
    GFL_CLACT_WK_SetDrawEnable( pWork->cellRadar, bFlg );
  }
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  起動アニメーションの表示　ON・OFF
 */
//-----------------------------------------------------------------------------
static void _gearStartUpObjDrawEnabel(C_GEAR_WORK* pWork,BOOL bFlg)
{
  int i;

  // 起動時アニメ用
  for(i=0;i < STARTUP_ANIME_OBJ_MAX;i++)
  {
    GFL_CLACT_WK_SetDrawEnable( pWork->cellStartUp[i], bFlg );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  起動アニメーションの開始
 */
//-----------------------------------------------------------------------------
static void _gearStartStartUpObjAnime(C_GEAR_WORK* pWork)
{
  int i;
  GFL_CLWK_DATA cellInitData;

  // オートアニメーション開始
  for(i=0;i < STARTUP_ANIME_OBJ_MAX;i++)
  {

    //セルの生成
    cellInitData.pos_x = STARTUP_ANIME_POS_X;
    cellInitData.pos_y = STARTUP_ANIME_POS_Y;
    cellInitData.anmseq = STARTUP_ANIME_INDEX_START + i;
    cellInitData.softpri = 0;
    cellInitData.bgpri = CGEAR_CLACT_BG_PRI;
    //↑矢印
    pWork->cellStartUp[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                              pWork->objRes[_CLACT_CHR],
                                              pWork->objRes[_CLACT_PLT],
                                              pWork->objRes[_CLACT_ANM],
                                              &cellInitData ,
                                              CGEAR_REND_SUB,
                                              pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellStartUp[i], FALSE );
    GFL_CLACT_WK_SetAutoAnmSpeed( pWork->cellStartUp[i], FX32_ONE*2 );


    GFL_CLACT_WK_ResetAnm( pWork->cellStartUp[i] );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellStartUp[i], TRUE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  起動アニメーションの終了
 */
//-----------------------------------------------------------------------------
static void _gearEndStartUpObjAnime(C_GEAR_WORK* pWork)
{
  int i;

  if( pWork->cellStartUp[0] ){
    // オートアニメーション開始
    for(i=0;i < STARTUP_ANIME_OBJ_MAX;i++)
    {
      GFL_CLACT_WK_Remove( pWork->cellStartUp[i] );
      pWork->cellStartUp[i] =NULL;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  起動アニメーションの終了待ち
 *	@retval TRUE    完了
 *	@retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL _gearIsEndStartUpObjAnime(const C_GEAR_WORK* cpWork)
{
  int i;

  // すべてのアニメーションの完了を待つ
  for(i=0;i < STARTUP_ANIME_OBJ_MAX;i++)
  {
    if( GFL_CLACT_WK_CheckAnmActive( cpWork->cellStartUp[i] ) == TRUE ){
      return FALSE;
    }
  }
  return TRUE;
}



//------------------------------------------------------------------------------
/**
 * @brief   すれ違いOBJの初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _gearCrossObjCreate(C_GEAR_WORK* pWork)
{
  int i;
  GFL_CLWK_DATA cellInitData;

  {

    //セルの生成

    cellInitData.pos_x = POS_CROSS_X_CENTER;
    cellInitData.pos_y = POS_CROSS_Y_CENTER;
    cellInitData.anmseq = NANR_c_gear_obj_CellAnime_surechigai;
    cellInitData.softpri = 0;
    cellInitData.bgpri = CGEAR_CLACT_BG_PRI;
    pWork->cellCross = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                               pWork->objRes[_CLACT_CHR],
                                               pWork->objRes[_CLACT_PLT],
                                               pWork->objRes[_CLACT_ANM],
                                               &cellInitData ,
                                               CGEAR_REND_SUB,
                                               pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCross, FALSE );
  }
  {
    cellInitData.pos_x = POS_CROSS_X_CENTER;
    cellInitData.pos_y = POS_CROSS_Y_CENTER;
    cellInitData.anmseq = NANR_c_gear_obj_CellAnime_surechigai_waku;
    cellInitData.softpri = 20;
    cellInitData.bgpri = CGEAR_CLACT_BG_PRI;
    pWork->cellCrossBase = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                               pWork->objRes[_CLACT_CHR],
                                               pWork->objRes[_CLACT_PLT],
                                               pWork->objRes[_CLACT_ANM],
                                               &cellInitData ,
                                               CGEAR_REND_SUB,
                                               pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellCrossBase, FALSE );
    
  }

  {
    //セルの生成

    cellInitData.pos_x = POS_SCANRADAR_X;
    cellInitData.pos_y = POS_SCANRADAR_Y;
    cellInitData.anmseq = NANR_c_gear_obj_CellAnime_radar;
    cellInitData.softpri = 0;
    cellInitData.bgpri = CGEAR_CLACT_BG_PRI;
    pWork->cellRadar = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                            pWork->objRes[_CLACT_CHR],
                                            pWork->objRes[_CLACT_PLT],
                                            pWork->objRes[_CLACT_ANM],
                                            &cellInitData ,
                                            CGEAR_REND_SUB,
                                            pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellRadar, FALSE );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   すれ違いOBJの実行
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _gearCrossObjMain(C_GEAR_WORK* pWork)
{
  const GAMEBEACON_INFO *beacon_info;
  u8 col;
  int i;
  s32 log_count;
  int put_count = 0; 

  if( !pWork->power_flag ){
    return ;
  }

  log_count = GAMEBEACON_Get_LogCount();

  log_count --; // 0オリジン
  for(i=0;i < GAMEBEACON_INFO_TBL_MAX;i++)
  {
    // 10個表示したら終わり
    if( put_count >= _CLACT_CROSS_MAX ){
      break;
    }
    
    if( log_count < 0 ){
      // 何もなしの色を設定
      PaletteWorkSet( pWork->pfade_ptr, &pWork->crossColor[ 16 ], FADE_SUB_OBJ, CROSS_COLOR_TRANS_POS_START + (sc_CROSS_COLOR_RES_IDX[ put_count ]*2), 4 );
      put_count ++;
      
    }else{
    
      beacon_info = GAMEBEACON_Get_BeaconLog(log_count);
      if(beacon_info != NULL){
        col = GAMEBEACON_Get_FavoriteColorIndex(beacon_info);
        // colの色を設定
        PaletteWorkSet( pWork->pfade_ptr, &pWork->crossColor[ col ], FADE_SUB_OBJ, CROSS_COLOR_TRANS_POS_START + (sc_CROSS_COLOR_RES_IDX[ put_count ]*2), 4 );
        put_count ++;
      }

      log_count --;
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   すれ違いOBJの開放
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _gearCrossObjDelete(C_GEAR_WORK* pWork)
{
  int i;

  if(pWork->cellCross){
    GFL_CLACT_WK_Remove(pWork->cellCross);
    pWork->cellCross = NULL;
  }
  if(pWork->cellCrossBase){
    GFL_CLACT_WK_Remove(pWork->cellCrossBase);
  }
  if(pWork->cellRadar){
    GFL_CLACT_WK_Remove(  pWork->cellRadar);
    pWork->cellRadar=NULL;
  }

}



//------------------------------------------------------------------------------
/**
 * @brief   モードセレクト全体の初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
//#define _NUKI_FONT_PALNO  (13)

static void _modeInit(C_GEAR_WORK* pWork,BOOL bBoot)
{
  MYSTATUS* pMy = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(pWork->pGameSys) );

  pWork->sex = MyStatus_GetMySex(pMy);
  pWork->bgno = CGEAR_SV_GetCGearType(pWork->pCGSV);

  //セル系システムの作成
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 57+_CLACT_TIMEPARTS_MAX, 0 , pWork->heapID );

  // ユーザーレンダラー設定
  pWork->userRend = GFL_CLACT_USERREND_Create( sc_REND_SURFACE_INIT, CGEAR_REND_MAX, pWork->heapID );
  GFL_CLACT_UNIT_SetUserRend( pWork->cellUnit, pWork->userRend );
  
  _gearArcCreate(pWork, pWork->sex, pWork->bgno, TRUE);  //ARC読み込み BG&OBJ
  _gearObjResCreate(pWork, pWork->sex);
  _gearObjCreate(pWork); //CLACT設定
  _gearCrossObjCreate(pWork);
  pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );

  if(bBoot){
    _gearBootInitScreen(pWork);
    _gearAllObjDrawEnabel(pWork, FALSE);
  }
  else{
    _gearPanelBgCreate(pWork);	// パネル作成
    _gearAllObjDrawEnabel(pWork, TRUE);
    _gearMarkObjDrawEnable(pWork,TRUE);
  }

}






static void _arcGearRelease(C_GEAR_WORK* pWork)
{

  _gearCrossObjDelete(pWork);

  if(pWork->cellMove){
    GFL_CLACT_WK_Remove( pWork->cellMove );
    pWork->cellMove=NULL;
  }
  {
    int i;
    for(i = 0;i < _CLACT_TIMEPARTS_MAX; i++){
      if(pWork->cellCursor[i]){
        GFL_CLACT_WK_Remove( pWork->cellCursor[i] );
        pWork->cellCursor[i] = NULL;
      }
    }
    for(i = 0;i < _CLACT_TYPE_MAX; i++){
      if( pWork->cellType[i]){
        GFL_CLACT_WK_Remove( pWork->cellType[i] );
        pWork->cellType[i] = NULL;
      }
    }
    for(i = 0;i < C_GEAR_PANEL_WIDTH * C_GEAR_PANEL_HEIGHT; i++){
      if(pWork->cellSelect[i]){
        GFL_CLACT_WK_Remove( pWork->cellSelect[i] );
        pWork->cellSelect[i] = NULL;
      }
    }
  }
  GFL_CLGRP_CELLANIM_Release( pWork->objRes[_CLACT_ANM] );
  GFL_CLGRP_CGR_Release( pWork->objRes[_CLACT_CHR] );
  GFL_CLGRP_PLTT_Release( pWork->objRes[_CLACT_PLT] );


  GFL_BG_FreeCharacterArea(GEAR_MAIN_FRAME,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->subchar));


}


static void _workEnd(C_GEAR_WORK* pWork)
{
  if(pWork->pButton){
    GFL_BMN_Delete(pWork->pButton);
  }
  _arcGearRelease(pWork);

  GFL_CLACT_USERREND_Delete( pWork->userRend );
  GFL_CLACT_UNIT_Delete( pWork->cellUnit );


  GFL_BG_FreeBGControl(GEAR_BUTTON_FRAME);
  GFL_BG_FreeBGControl(GEAR_BMPWIN_FRAME);
  GFL_BG_FreeBGControl(GEAR_MAIN_FRAME);


  GFL_BG_SetVisible( GEAR_BUTTON_FRAME, VISIBLE_OFF );
  GFL_BG_SetVisible( GEAR_BMPWIN_FRAME, VISIBLE_OFF );
  GFL_BG_SetVisible( GEAR_MAIN_FRAME, VISIBLE_OFF );

}


//------------------------------------------------------------------------------
/**
 * @brief   時間アニメーション
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _timeAnimation(C_GEAR_WORK* pWork)
{
  RTCTime time;

  GFL_RTC_GetTime( &time );

  {  //AMPM
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_ampm];
    int num = time.hour / 12;
    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }
  {  //時10
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_NO2];
    int num = (time.hour % 12) / 10;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
    if(num==0){
      GFL_CLACT_WK_SetDrawEnable(cp_wk,FALSE);
    }
    else{
      GFL_CLACT_WK_SetDrawEnable(cp_wk,TRUE);
    }
  }
  {  //時1
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_NO10a];
    int num = (time.hour % 12) % 10;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }

  {  //ころん
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_colon];
    int num = time.second % 2;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }


  {//秒１０
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_NO6];
    int num = time.minute / 10;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }
  {//秒１
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_NO10b];
    int num = time.minute % 10;

    if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
      GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
    }
  }
  {
    GFL_CLWK* cp_wk = pWork->cellCursor[NANR_c_gear_obj_CellAnime_batt1];
    if( OS_IsRunOnTwl() ){//DSIなら
      if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  GFL_UI_GetBattLevel()){
        GFL_CLACT_WK_SetAnmIndex(cp_wk, GFL_UI_GetBattLevel());
      }
    }
    else{
      int num = (GFL_UI_GetBattLevel() == GFL_UI_BATTLEVEL_HI ? 1 : 0);
      if(GFL_CLACT_WK_GetAnmIndex(cp_wk) !=  num){
        GFL_CLACT_WK_SetAnmIndex(cp_wk,num);
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タイプのアニメーション 位置調整
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _typeAnimation(C_GEAR_WORK* pWork)
{
  int i;

  for(i=0;i < _CLACT_TYPE_MAX ;i++)
  {
    int x,y;
    GFL_CLACTPOS pos;
    if( _gearGetTypeBestPosition(pWork, CGEAR_PANELTYPE_IR+i, &x, &y) ){
      x *= 8;
      y *= 8;
      pos.x = x+24-6-2;  // OBJ表示の為の補正値
      pos.y = y+6+6+3;
      GFL_CLACT_WK_SetPos(pWork->cellType[i], &pos, CGEAR_REND_SUB);

      // X位置をプライオリティに。
      GFL_CLACT_WK_SetSoftPri( pWork->cellType[i], x/8 );

    }else{

      pos.x = -64;  // OBJ表示の為の補正値
      pos.y = 0;
      GFL_CLACT_WK_SetPos(pWork->cellType[i], &pos, CGEAR_REND_SUB);

    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   モードセレクト画面待機　（通常ループ）
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait(C_GEAR_WORK* pWork)
{
  // 画面が出るまで待つ
  if( _modeSelectWaitSubFadeEnd( pWork ) == FALSE ){
    return ;
  }
  GFL_BMN_Main( pWork->pButton );
  
  // スリープカラー反映
  SleepMode_ColorUpdate( pWork );
  
  _timeAnimation(pWork);

  // スリープ中は停止
  if( !SleepMode_IsSleep( pWork ) ){
    if( pWork->power_flag ){
      _PanelPaletteUpdate( pWork ); 
      _PanelPaletteChange(pWork);

      _gearCrossObjMain(pWork);
    }
  }else{

    // CGEAR以外でイベントが発生
    // ボタンのリクエストクリア
    pWork->createEvent = FIELD_SUBSCREEN_ACTION_NONE;
  }
  
 


#if 0
  ///インフォメーションメッセージ構造体(キューを取得する時、この構造体に変換して取得する)
  typedef struct{
    STRBUF *name[INFO_WORDSET_MAX];     ///<未使用の場合はNULLが入っています
    u8 wordset_no[INFO_WORDSET_MAX];
    u16 message_id;
    u8 padding[2];
  }GAME_COMM_INFO_MESSAGE;
#endif

}
//------------------------------------------------------------------------------
/**
 * @brief   スリープ復帰　画面　待機１
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait2(C_GEAR_WORK* pWork)
{
  
  if(_gearBootMain(pWork) == FALSE){
    if(pWork->pCall){
      pWork->pCall(pWork->pWork);
    }
    pWork->pCall=NULL;
    pWork->pWork=NULL;
    return;
  }

  _PanelPaletteAnimeInitSleep( pWork );
  // 通常待機へ遷移
  _CHANGE_STATE(pWork, _modeSelectMenuWait);
}


//------------------------------------------------------------------------------
/**
 * @brief   初期起動　画面　待機
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait0(C_GEAR_WORK* pWork)
{

  switch(pWork->state_seq){
  case STARTUP_SEQ_ANIME_START:

    // 何もないテーブルを書き込み
    _gearBootInitScreen( pWork ); // 1回表示するため。

    // 全OAM　非表示
    _gearAllObjDrawEnabel( pWork, FALSE );

    pWork->state_seq ++;
    break;

  case STARTUP_SEQ_WIPE_IN:

    if( _modeSelectWaitSubFadeEnd( pWork ) == FALSE ){
      break;
    }


    // 起動アニメだけ表示
    _gearStartStartUpObjAnime( pWork );
    _gearStartUpObjDrawEnabel( pWork, TRUE );
    pWork->state_seq ++;
    break;

  // アニメ待ち
  case STARTUP_SEQ_ANIME_WAIT:

    
    if( _gearIsEndStartUpObjAnime( pWork ) == FALSE ){
      break;
    }

    // ブラックアウト開始
    _PFadeToBlack( pWork );

    pWork->startCounter = 0;
    pWork->state_seq ++;
    break;

  // ALPHAアニメウエイト
  case STARTUP_SEQ_OAM_ALPHA_WAIT:

    // スキップ
    if( _IsEffectSkip(pWork) ){
      pWork->state_seq = STARTUP_SEQ_SKIP;
      break;
    }

    if( pWork->startCounter < STARTUP_OAM_ALPHA_ANIME_FRAME_MAX ){
      pWork->startCounter ++;
      break;
    }
    
    _gearStartUpObjDrawEnabel( pWork, FALSE );
    _gearEndStartUpObjAnime( pWork );
    // OAMブラックアウト
    _PFadeSetBlack(pWork);

    pWork->state_seq++;
    pWork->startCounter = 0;
    break;

  // テーブルが順に出てくるアニメ
  case STARTUP_SEQ_TBL_IN_WAIT:

    // スキップ
    if( _IsEffectSkip(pWork) ){
      pWork->state_seq = STARTUP_SEQ_SKIP;
      break;
    }

    if( _gearStartUpMain( pWork ) == TRUE ){

      pWork->startCounter = 0;
      pWork->state_seq++;
    }
    break;

  case STARTUP_SEQ_TBL_ALPHA_WAIT:

    // スキップ
    if( _IsEffectSkip(pWork) ){
      pWork->state_seq = STARTUP_SEQ_SKIP;
      break;
    }
    
    pWork->startCounter ++;
    if( pWork->startCounter >= STARTUP_TBL_ALPHA_TIME_WAIT ){
      // 最終フレームで全消し
      _gearStartUpAllOff( pWork );
      pWork->startCounter = 0;
      pWork->state_seq++;
    }
    break;

  case STARTUP_SEQ_END:

    pWork->startCounter ++;
    if( pWork->startCounter < STARTUP_END_TIME_WAIT ){
      break;
    }
    
    _gearAllObjDrawEnabel( pWork, TRUE );
    _gearMarkObjDrawEnable(pWork,FALSE);
    _CHANGE_STATE(pWork, _modeSelectMenuWait2);
    break;



  case STARTUP_SEQ_SKIP:
    _gearEndStartUpObjAnime( pWork );
    // OAMブラックアウト
    _PFadeSetBlack(pWork);

    // アニメストップ
    _PanelMarkAnimeSysAllStop( pWork );

    // 全消し
    _gearStartUpAllOff( pWork );

    pWork->startCounter = STARTUP_END_TIME_WAIT;

    pWork->state_seq = STARTUP_SEQ_END;
    break;

  default:
    break;
  }
  
}

//------------------------------------------------------------------------------
/**
 * @brief   スリープ復帰　タイムウエイト
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait1(C_GEAR_WORK* pWork)
{

  if(pWork->startCounter==0){
    _PFadeSetBlack(pWork);
    _gearAllObjDrawEnabel( pWork, TRUE );
    _gearMarkObjDrawEnable(pWork,FALSE);

    // 枠部分を表示
    _gearBootInitScreen( pWork ); // 1回表示するため。

  }else if(pWork->startCounter==1){
    WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
  }

  pWork->startCounter++;
  if( pWork->startCounter >= _SLEEP_START_TIME_WAIT ){
    pWork->startCounter = 0;
    _CHANGE_STATE(pWork, _modeSelectMenuWait2);
  }
}




static BOOL _loadExData(C_GEAR_WORK* pWork,GAMESYS_WORK* pGameSys)
{
  BOOL ret=FALSE;

  {
    int i;
    u8* pCGearWork = GFL_HEAP_AllocMemory(HEAPID_FIELD_SUBSCREEN,SAVESIZE_EXTRA_CGEAR_PICTURE);
    SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(pGameSys));


    if(LOAD_RESULT_OK== SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE, HEAPID_FIELD_SUBSCREEN,
                                                   pCGearWork,SAVESIZE_EXTRA_CGEAR_PICTURE)){
      CGEAR_PICTURE_SAVEDATA* pPic = (CGEAR_PICTURE_SAVEDATA*)pCGearWork;

      {
        u16 crc = GFL_STD_CrcCalc( pPic, CGEAR_PICTURTE_CHAR_SIZE+CGEAR_PICTURTE_PAL_SIZE+CGEAR_PICTURTE_SCR_SIZE );
        u16 crc2 = CGEAR_SV_GetCGearPictureCRC(CGEAR_SV_GetCGearSaveData(pSave));
        if(crc == crc2){
          if(CGEAR_PICTURE_SAVE_IsPalette(pPic)){
            ret = TRUE;
            for(i=0;i<CGEAR_DECAL_SIZEY;i++){
              GFL_BG_LoadCharacter(GEAR_MAIN_FRAME,&pCGearWork[CGEAR_DECAL_SIZEX * 32 * i],CGEAR_DECAL_SIZEX * 32, (5 + i)*32);
            }
            GFL_BG_LoadPalette(GEAR_MAIN_FRAME,pPic->palette,CGEAR_PICTURTE_PAL_SIZE, 32*0x0a );
          }
          GFL_BG_LoadScreen(GEAR_MAIN_FRAME,pPic->scr,CGEAR_PICTURTE_SCR_SIZE, 0 );
          GFL_BG_LoadScreenReq(GEAR_MAIN_FRAME);
        }
#if PM_DEBUG
        else{
          OS_TPrintf("CRCDIFF chk%x pict%x\n",crc,crc2);
        }
#endif
      }
    }
    SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE);
    GFL_HEAP_FreeMemory(pCGearWork);
  }
  return ret;
}



///< スリープ起動時に呼ばれる関数
static void _SLEEPGO_FUNC(void* pWk)
{
  C_GEAR_WORK* pWork = pWk;

  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
}

///< スリープ復帰時に呼ばれる関数
static void _SLEEPRELEASE_FUNC(void* pWk)
{
  C_GEAR_WORK* pWork = pWk;

  pWork->GetOpenTrg=TRUE;
}

//-------------------------------------
/// VBlank関数
//=====================================
static void _VBlankFunc( GFL_TCB* tcb, void* wk )
{
  C_GEAR_WORK* pWork = (C_GEAR_WORK*)wk;

  // 白く飛ばす演出のためのパレットフェード
  if( pWork->pfade_ptr ) PaletteFadeTrans( pWork->pfade_ptr );
}

//------------------------------------------------------------------------------
/**
 * @brief   スタート
 * @retval  none
 */
//------------------------------------------------------------------------------
C_GEAR_WORK* CGEAR_Init( CGEAR_SAVEDATA* pCGSV,FIELD_SUBSCREEN_WORK* pSub,GAMESYS_WORK* pGameSys, BOOL power_effect )
{
  C_GEAR_WORK *pWork = NULL;
  BOOL ret = FALSE;

  //GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_CGEAR, 0x8000 );

  // OS_TPrintf("zzzz start field_heap = %x\n", GFL_HEAP_GetHeapFreeSize(HEAPID_FIELD_SUBSCREEN));

  pWork = GFL_HEAP_AllocClearMemory( HEAPID_FIELD_SUBSCREEN, sizeof( C_GEAR_WORK ) );
  pWork->heapID = HEAPID_FIELD_SUBSCREEN;
  pWork->pCGSV = pCGSV;
  pWork->subscreen = pSub;
  pWork->pGameSys = pGameSys;
  pWork->bAction = TRUE;
  pWork->power_flag = GAMESYSTEM_GetAlwaysNetFlag( pWork->pGameSys );
  pWork->use_skip = TRUE;

  pWork->handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, HEAPID_FIELD_SUBSCREEN );

  pWork->pfade_tcbsys_wk = GFL_HEAP_AllocClearMemory( HEAPID_FIELD_SUBSCREEN, GFL_TCB_CalcSystemWorkSize(4) );
  pWork->pfade_tcbsys = GFL_TCB_Init( 4, pWork->pfade_tcbsys_wk );

  if( power_effect ){
    _CHANGE_STATE(pWork,_modeSelectMenuWait0);
  }else{
    _PanelPaletteAnimeInit( pWork );
    _CHANGE_STATE(pWork,_modeSelectMenuWait);
  }

  _createSubBg(pWork);   //BGVRAM設定
  _modeInit(pWork, power_effect);
  if(CGEAR_SV_GetCGearPictureONOFF(pWork->pCGSV)){
    ret = _loadExData(pWork,pGameSys);  //デカール読み込み
  }
  _gearDecalScreenArcCreate(pWork,ret);

  // パネルアニメのシステムクリア
  _PanelMarkAnimeSysInit( pWork );


  pWork->pfade_ptr = PaletteFadeInit( HEAPID_FIELD_SUBSCREEN );
  PaletteTrans_AutoSet( pWork->pfade_ptr, TRUE );
  PaletteFadeWorkAllocSet( pWork->pfade_ptr, FADE_SUB_OBJ, 0x20*OAM_USE_PLTT_NUM, HEAPID_FIELD_SUBSCREEN );
  PaletteFadeWorkAllocSet( pWork->pfade_ptr, FADE_SUB_BG, 0x20*OAM_USE_PLTT_NUM, HEAPID_FIELD_SUBSCREEN );
  // 現在VRAMにあるパレットを壊さないように、VRAMからパレット内容をコピーする
  PaletteWorkSet_VramCopy( pWork->pfade_ptr, FADE_SUB_OBJ, 0, 0x20*OAM_USE_PLTT_NUM );
  PaletteWorkSet_VramCopy( pWork->pfade_ptr, FADE_SUB_BG, 0, 0x20*OAM_USE_PLTT_NUM );
  _PFadeSetDefaultPal( pWork, pWork->power_flag );
  PaletteFadeTrans( pWork->pfade_ptr );

  {
    pWork->vblank_tcb = GFUser_VIntr_CreateTCB( _VBlankFunc, pWork, 1 );
  }


  GFL_UI_SleepGoSetFunc(&_SLEEPGO_FUNC,  pWork);
  GFL_UI_SleepReleaseSetFunc(&_SLEEPRELEASE_FUNC,  pWork);


  // ボタンパレットアニメシステム初期化
  _BUTTONPAL_Init( pWork, &pWork->button_palfade );

  
  // CrossObj初期化
  _gearCrossObjMain( pWork );


  // 通信OFF時は強制スリープ
  if( (GFL_NET_IsInit() ==FALSE) && pWork->power_flag ){
    // スリープ
    SleepMode_HighSleepStart( pWork );
  }


  //  _PFadeToBlack(pWork);
  //  OS_TPrintf("zzzz start field_heap = %x\n", GFL_HEAP_GetHeapFreeSize(HEAPID_FIELD_SUBSCREEN));

  return pWork;
}




//------------------------------------------------------------------------------
/**
 * @brief   起動画面スタート
 * @retval  none
 */
//------------------------------------------------------------------------------
C_GEAR_WORK* CGEAR_FirstInit( CGEAR_SAVEDATA* pCGSV,FIELD_SUBSCREEN_WORK* pSub,GAMESYS_WORK* pGameSys, STARTUP_ENDCALLBACK* pCall,void* pWork2, BOOL power_effect )
{
  C_GEAR_WORK* pWork = CGEAR_Init( pCGSV, pSub, pGameSys, power_effect);
  
  
  pWork->pCall = pCall;
  pWork->pWork = pWork2;

  if( power_effect ){
    _CHANGE_STATE(pWork,_modeSelectMenuWait0);
    // スキップ不可能
    pWork->use_skip = FALSE;
  }else{
    if(pWork->pCall){
      pWork->pCall(pWork->pWork);
    }
    pWork->pCall=NULL;
    pWork->pWork=NULL;
  }


  // 初期配置情報を設定
  CGEAR_PATTERN_SetUp( pCGSV, pWork->handle, GFUser_GetPublicRand(CGEAR_PATTERN_MAX), 
      HEAPID_FIELD_SUBSCREEN );

  // タイプの表示をリセット
  _typeAnimation(pWork);
    
  return pWork;
}



//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------




void CGEAR_Main( C_GEAR_WORK* pWork,BOOL bAction )
{
  if(pWork->bAction != bAction){

    // イベント中は、画面を暗く
    if( bAction == FALSE ){
      SleepMode_Start( pWork );
    }else{
      SleepMode_End( pWork );
    }

    pWork->bAction = bAction;
  }

  // スリープ時は強制停止
  if( SleepMode_IsSleep( pWork ) ){
    FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( GAMESYSTEM_GetGameData(pWork->pGameSys) );
    FSND_StopTVTRingTone( fsnd );
    pWork->tvt_snd = FALSE;
  }

  if( !SleepMode_IsSleep( pWork ) )
  {
    if( !Intrude_CheckPalaceConnect(GAMESYSTEM_GetGameCommSysPtr(pWork->pGameSys)) ){ // 進入チェック
      // トランシーバー反応処理
      u32 bit = WIH_DWC_GetAllBeaconTypeBit(GAMEDATA_GetWiFiList(GAMESYSTEM_GetGameData(pWork->pGameSys)));
      FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( GAMESYSTEM_GetGameData(pWork->pGameSys) );

      // トランシーバー再生処理
      if(bit & GAME_COMM_STATUS_BIT_WIRELESS_TR){ // トランシーバー
        if( pWork->tvt_snd == FALSE ){
          //OS_TPrintf("よびだし\n");
          pWork->tvt_snd = TRUE;
          // 強制的にアニメーションをリセット
          _PanelPalette_EXEnd( pWork );
        }
        FSND_RequestTVTRingTone( fsnd);

      // トランシーバー停止処理
      }else if(!(bit & GAME_COMM_STATUS_BIT_WIRELESS_TR)){

        FSND_StopTVTRingTone( fsnd );
        pWork->tvt_snd = FALSE;
      }
    }
  }
  if( pWork->GetOpenTrg ){
    pWork->GetOpenTrg=FALSE;
    if( SleepMode_IsSleep( pWork ) == FALSE ){
      _CHANGE_STATE(pWork,_modeSelectMenuWait1);
    }else{
      // スリープ中は簡単に終わらす。
      WIPE_ResetBrightness(WIPE_DISP_SUB);
    }
  }
  {
    StateFunc* state = pWork->state;
    if(state != NULL){
      state(pWork);
    }
  }
  if( pWork->pfade_tcbsys ) GFL_TCB_Main( pWork->pfade_tcbsys );


}


//------------------------------------------------------------------------------
/**
 * @brief   CGEAR_ActionCallback
 * @retval  none
 */
//------------------------------------------------------------------------------
void CGEAR_ActionCallback( C_GEAR_WORK* pWork , FIELD_SUBSCREEN_ACTION actionno)
{
}



//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
void CGEAR_Exit( C_GEAR_WORK* pWork )
{
  _BUTTONPAL_Exit( &pWork->button_palfade );
  
  GFL_UI_SleepGoSetFunc(NULL,  NULL);
  GFL_UI_SleepReleaseSetFunc(NULL,  NULL);

  GFL_NET_ChangeIconPosition(GFL_WICON_POSX,GFL_WICON_POSY);
  GFL_NET_ResetIconForcePosition();
  GFL_NET_ReloadIcon();

  // パレットフェード
  PaletteFadeWorkAllocFree( pWork->pfade_ptr, FADE_SUB_OBJ );
  PaletteFadeWorkAllocFree( pWork->pfade_ptr, FADE_SUB_BG );
  PaletteFadeFree( pWork->pfade_ptr );
  // タスク
  GFL_TCB_Exit( pWork->pfade_tcbsys );
  GFL_HEAP_FreeMemory( pWork->pfade_tcbsys_wk );
  GFL_TCB_DeleteTask( pWork->vblank_tcb );
  {
    FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( GAMESYSTEM_GetGameData(pWork->pGameSys) );
    FSND_StopTVTRingTone( fsnd );
  }

  GFL_ARC_CloseDataHandle( pWork->handle );

  _workEnd(pWork);
  G2S_BlendNone();
  GFL_HEAP_FreeMemory(pWork);

}


//------------------------------------------------------------------------------
/**
 * @brief   EventCheck
 * @retval  none
 */
//------------------------------------------------------------------------------

GMEVENT* CGEAR_EventCheck(C_GEAR_WORK* pWork, BOOL bEvReqOK, FIELD_SUBSCREEN_WORK* pSub )
{
  GMEVENT* event=NULL;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(pWork->pGameSys);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(pWork->pGameSys);

  if(bEvReqOK == FALSE || fieldWork == NULL){
    return NULL;
  }

  switch(pWork->createEvent){
  case FIELD_SUBSCREEN_ACTION_GSYNC:
  case FIELD_SUBSCREEN_ACTION_IRC:
  case FIELD_SUBSCREEN_ACTION_WIRELESS:
    PMSND_PlaySystemSE( GEAR_SE_DECIDE_ );
    event = EVENT_ButtonEffectWaitCall( pWork, pWork->createEvent );
    break;
    
  case FIELD_SUBSCREEN_ACTION_SCANRADAR:
  case FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW:
  case FIELD_SUBSCREEN_ACTION_CGEAR_HELP:
  case FIELD_SUBSCREEN_ACTION_CGEAR_POWER:
    PMSND_PlaySE( SEQ_SE_MSCL_07 );
    event = EVENT_ButtonEffectWaitCall( pWork, pWork->createEvent );
    break;
  }
  if( event ){
    pWork->createEvent = FIELD_SUBSCREEN_ACTION_NONE;
    // イベント待ちにする。
    _CHANGE_STATE(pWork,_modeEventWait);
  }
  return event;
}


//----------------------------------------------------------------------------
/**
 *	@brief  パネルタイプを設定  （設定した場所をICONの場所にする。）
 *
 *	@param	pSV
 *	@param	x
 *	@param	y
 *	@param	type 
 */
//-----------------------------------------------------------------------------
static void _modeSetSavePanelType( C_GEAR_WORK* pWork, CGEAR_SAVEDATA* pSV,int x, int y, CGEAR_PANELTYPE_ENUM type )
{
  int i, j;

  for( i=0; i<C_GEAR_PANEL_HEIGHT; i++ ){
    for( j=0; j<C_GEAR_PANEL_WIDTH; j++ ){
      if( _cgearSave_GetPanelType( pWork, j, i ) == type ){

        if( _cgearSave_IsPanelTypeIcon( pWork, j, i ) ){
          // OFFにする。
          CGEAR_SV_SetPanelType( pSV, j, i, type, FALSE, TRUE );
          _gearPanelBgScreenMake(pWork, j, i, type);  // 色を普通の色に
        }else if( _cgearSave_IsPanelTypeLast( pWork, j, i, type ) ){
          // LastをOFFにする。
          CGEAR_SV_SetPanelType( pSV, j, i, type, FALSE, FALSE );
        }
        
      }
    }
  }

  // ONにする。
  CGEAR_SV_SetPanelType( pSV, x, y, type, TRUE, FALSE );
  _gearPanelBgScreenMake(pWork, x, y, type);  // つよい色に


  GFL_BG_LoadScreenReq( GEAR_BUTTON_FRAME );

  // タイプの表示をリセット
  _typeAnimation(pWork);
}




//----------------------------------------------------------------------------
/**
 *	@brief  Ｃｇｅａｒセーブ　パネルタイプの取得
 *
 *	@param	cpWork
 *	@param	x   
 *	@param	y
 *
 *	@return パネルタイプ
 */
//-----------------------------------------------------------------------------
static CGEAR_PANELTYPE_ENUM _cgearSave_GetPanelType(const C_GEAR_WORK* cpWork,int x, int y)
{
  if( cpWork->power_flag ){
    return CGEAR_SV_GetPanelType( cpWork->pCGSV, x, y );
  }

  return CGEAR_PANELTYPE_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  アイコンの出るところかチェック
 *
 *	@param	cpWork
 *	@param	x
 *	@param	y
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL _cgearSave_IsPanelTypeIcon(const C_GEAR_WORK* cpWork,int x, int y)
{
  if( cpWork->power_flag ){
    return CGEAR_SV_IsPanelTypeIcon( cpWork->pCGSV, x, y );
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  1つ前にアイコンがあった場所か？チェック
 *
 *	@param	cpWork
 *	@param	x
 *	@param	y
 *	@param	type 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL _cgearSave_IsPanelTypeLast(const C_GEAR_WORK* cpWork,int x, int y, CGEAR_PANELTYPE_ENUM type )
{
  if( cpWork->power_flag ){
    return CGEAR_SV_IsPanelTypeLast( cpWork->pCGSV, x, y, type );
  }
  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  スキップチェック
 *
 *	@param	const C_GEAR_WORK* cpWork 
 *
 *	@retval TRUE  スキップ
 */
//-----------------------------------------------------------------------------
static BOOL _IsEffectSkip( const C_GEAR_WORK* cpWork )
{
  if( cpWork->use_skip ){
    if( GFL_UI_TP_GetTrg() ){
      return TRUE;
    }
  }
  return FALSE;
}




//----------------------------------------------------------------------------
/**
 *	@brief  カーソル選択　待ち　アニメーション
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static BOOL _CursorSelectAnimeWait( C_GEAR_WORK* pWork )
{
  switch( pWork->state_seq ){
  case 0:
    // アニメーション開始
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellCursor[ pWork->select_cursor ], TRUE );
    GFL_CLACT_WK_SetAutoAnmSpeed( pWork->cellCursor[ pWork->select_cursor ], FX32_ONE*2 );
    pWork->state_seq ++;
    break;

  case 1:
    if( GFL_CLACT_WK_CheckAnmActive( pWork->cellCursor[ pWork->select_cursor ] ) == FALSE ){

      return TRUE;
    }
    break;

  default:
    break;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パレットアニメでの　選択アニメーション
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static BOOL _PalAnimeSelectAnimeWait( C_GEAR_WORK* pWork )
{
  switch( pWork->state_seq ){
  case 0:
    switch( pWork->select_cursor ){
    case TOUCH_LABEL_CROSS:
      _BUTTONPAL_Start( &pWork->button_palfade, (1<<6) | (1<<7) | (1<<8) );
      break;
    case TOUCH_LABEL_RADAR:
      _BUTTONPAL_Start( &pWork->button_palfade, (1<<9) );
      break;
    default:
      // 選択カーソル不明
      GF_ASSERT(0);
      break;
    }
    pWork->state_seq ++;
    break;

  case 1:
    if( _BUTTONPAL_Update( &pWork->button_palfade ) ){

      return TRUE;
    }
    break;

  default:
    break;
  }
  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  イベント待ち
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static void _modeEventWait( C_GEAR_WORK* pWork )
{
}




//----------------------------------------------------------------------------
/**
 *	@brief  スリープモード開始
 *
 *	@param	pWork   ワーク
 */
//-----------------------------------------------------------------------------
static void SleepMode_Start( C_GEAR_WORK* pWork )
{
  if( pWork->high_sleep_mode == FALSE ){
    if( pWork->sleep_mode == FALSE ){
      pWork->sleep_mode = TRUE;
      pWork->sleep_color_req = TRUE;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  スリープモード開始
 *
 *	@param	pWork   ワーク
 */
//-----------------------------------------------------------------------------
static void SleepMode_HighSleepStart( C_GEAR_WORK* pWork )
{
  if( pWork->sleep_mode == FALSE ){
    pWork->high_sleep_mode = TRUE;
    pWork->sleep_mode = TRUE;
    pWork->sleep_color_req = TRUE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  スリープモード停止
 *
 *	@param	pWork ワーク
 */
//-----------------------------------------------------------------------------
static void SleepMode_End( C_GEAR_WORK* pWork )
{
  if( pWork->high_sleep_mode == FALSE ){
    if( pWork->sleep_mode == TRUE ){
      pWork->sleep_mode = FALSE;
      pWork->sleep_color_req = TRUE;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  色変換処理
 *
 *	@param	pWork 
 */
//-----------------------------------------------------------------------------
static void SleepMode_ColorUpdate( C_GEAR_WORK* pWork )
{
  if( pWork->sleep_color_req ){
    _PFadeSetSleepBlack( pWork, pWork->sleep_mode );
    pWork->sleep_color_req = FALSE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  スリープモードチェック
 *
 *	@param	cpWork 
 */
//-----------------------------------------------------------------------------
static BOOL SleepMode_IsSleep( const C_GEAR_WORK* cpWork )
{
  return cpWork->sleep_mode;

}



//----------------------------------------------------------------------------
/**
 *	@brief  ボタン　パレット　アニメ  初期化
 *
 *	@param	pWork   ワーク
 *	@param	p_fwk   ワーク
 */
//-----------------------------------------------------------------------------
static void _BUTTONPAL_Init( C_GEAR_WORK* pWork, BUTTON_PAL_FADE* p_fwk )
{
  // ダミーのACTOR作成
  {
    GFL_CLWK_DATA cellInitData;

    cellInitData.pos_x = 0;
    cellInitData.pos_y = 0;
    cellInitData.anmseq = NANR_c_gear_obj_CellAnime_help;
    cellInitData.softpri = 0;
    cellInitData.bgpri = CGEAR_CLACT_BG_PRI;

    p_fwk->dummy = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                              pWork->objRes[_CLACT_CHR],
                                              pWork->objRes[_CLACT_PLT],
                                              pWork->objRes[_CLACT_ANM],
                                              &cellInitData ,
                                              CGEAR_REND_SUB,
                                              pWork->heapID );

    // 表示OFF
    GFL_CLACT_WK_SetDrawEnable( p_fwk->dummy, FALSE );
  }

  // パレットフェード
  p_fwk->p_fade_ptr     = pWork->pfade_ptr;
  p_fwk->p_fade_tcbsys  = pWork->pfade_tcbsys;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ボタン　パレット　アニメ  破棄処理
 *
 *	@param	p_fwk 
 */
//-----------------------------------------------------------------------------
static void _BUTTONPAL_Exit( BUTTON_PAL_FADE* p_fwk )
{
  GFL_CLACT_WK_Remove( p_fwk->dummy );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ボタン　パレット　アニメ  開始
 *
 *	@param	p_fwk   ワーク
 *	@param	msk     パレットマスク
 */
//-----------------------------------------------------------------------------
static void _BUTTONPAL_Start( BUTTON_PAL_FADE* p_fwk, u32 bit )
{

  GFL_CLACT_WK_ResetAnm( p_fwk->dummy );
  GFL_CLACT_WK_SetAutoAnmFlag( p_fwk->dummy, TRUE );
  GFL_CLACT_WK_SetAutoAnmSpeed( p_fwk->dummy, 2<<FX32_SHIFT );

  p_fwk->anime_on = TRUE;

  p_fwk->msk = bit;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ボタン　パレット　アニメ　更新
 *
 *	@param	p_fwk   ワーク
 *
 *	@retval TRUE    完了
 *	@retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL _BUTTONPAL_Update( BUTTON_PAL_FADE* p_fwk )
{
  u32 frame_index;
  static const u32 sc_INDEX_Color[] = {
    0,
    9,
    0,
    9,
    0,
    9,
  };
  
  if( p_fwk->anime_on == FALSE ){
    return TRUE;
  }


  if( GFL_CLACT_WK_CheckAnmActive( p_fwk->dummy ) == FALSE ){
    p_fwk->anime_on = FALSE;
    return TRUE;
  }

  frame_index = GFL_CLACT_WK_GetAnmIndex( p_fwk->dummy );
  // パレットカラーを調整
  PaletteFadeReq(
    p_fwk->p_fade_ptr, PF_BIT_SUB_OBJ, p_fwk->msk,   -120, 0, sc_INDEX_Color[frame_index], 0x0, p_fwk->p_fade_tcbsys
    );
  return FALSE;
}




// CGEAR演出イベント
//----------------------------------------------------------------------------
/**
 *	@brief  CGEAR演出待ちイベント生成
 */
//-----------------------------------------------------------------------------
static GMEVENT* EVENT_ButtonEffectWaitCall( C_GEAR_WORK* pWork, u32 createEvent )
{
  EV_CGEAR_EFFECTWAIT* p_wk;
  GMEVENT * event;
  
  event = GMEVENT_Create(pWork->pGameSys, NULL, EVENT_ButtonEffectWait, sizeof(EV_CGEAR_EFFECTWAIT));
  
  p_wk = GMEVENT_GetEventWork(event);
  
  p_wk->pWork = pWork;
  p_wk->createEvent = createEvent;

  // Effect初期化
  switch(p_wk->createEvent){
  case FIELD_SUBSCREEN_ACTION_GSYNC:
  case FIELD_SUBSCREEN_ACTION_IRC:
  case FIELD_SUBSCREEN_ACTION_WIRELESS:
    _modeSelectAnimInit( p_wk->pWork );
    break;
  case FIELD_SUBSCREEN_ACTION_SCANRADAR:
    pWork->select_cursor = TOUCH_LABEL_RADAR; 
    break;

  case FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW:
    pWork->select_cursor = TOUCH_LABEL_CROSS; 
    break;

  case FIELD_SUBSCREEN_ACTION_CGEAR_HELP:
    pWork->select_cursor = _CLACT_HELP;
    break;
  case FIELD_SUBSCREEN_ACTION_CGEAR_POWER:
    pWork->select_cursor = _CLACT_POWER;
    break;
  }
  
  return event;
}

static GMEVENT_RESULT EVENT_ButtonEffectWait( GMEVENT *event, int *seq, void *wk )
{
  EV_CGEAR_EFFECTWAIT* p_wk = wk;
  C_GEAR_WORK* pWork = p_wk->pWork;
  BOOL result = FALSE;
  GMEVENT* p_cevent;
  FIELDMAP_WORK* fieldWork = GAMESYSTEM_GetFieldMapWork( pWork->pGameSys );

  // Effect完了待ち
  switch(p_wk->createEvent){
  case FIELD_SUBSCREEN_ACTION_GSYNC:
  case FIELD_SUBSCREEN_ACTION_IRC:
  case FIELD_SUBSCREEN_ACTION_WIRELESS:
    result = _modeSelectAnimWait( p_wk->pWork );
    break;
  case FIELD_SUBSCREEN_ACTION_SCANRADAR:
  case FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW:
    result = _PalAnimeSelectAnimeWait( p_wk->pWork );
    break;

  case FIELD_SUBSCREEN_ACTION_CGEAR_HELP:
  case FIELD_SUBSCREEN_ACTION_CGEAR_POWER:
    result = _CursorSelectAnimeWait( p_wk->pWork );
    break;
  }

  if( result == FALSE ){
    return GMEVENT_RES_CONTINUE;
  }


  // アニメ完了後別イベントへ
  switch(p_wk->createEvent){
  case FIELD_SUBSCREEN_ACTION_GSYNC:
    p_cevent = EVENT_GSync(pWork->pGameSys, fieldWork, NULL, TRUE);
    break;
  case FIELD_SUBSCREEN_ACTION_IRC:
    p_cevent = EVENT_IrcBattle(pWork->pGameSys, fieldWork, NULL, TRUE);
    break;
  case FIELD_SUBSCREEN_ACTION_WIRELESS:
    p_cevent= EVENT_CG_Wireless(pWork->pGameSys, fieldWork, NULL, TRUE);
    break;
  case FIELD_SUBSCREEN_ACTION_SCANRADAR:
    p_cevent= EVENT_ResearchRadar( pWork->pGameSys, fieldWork );
    break;
  case FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW:
    p_cevent= EVENT_ChangeSubScreen(pWork->pGameSys, fieldWork, FIELD_SUBSCREEN_BEACON_VIEW);
    break;
  case FIELD_SUBSCREEN_ACTION_CGEAR_HELP:
    {
      CG_HELP_INIT_WORK *initWork = GFL_HEAP_AllocClearMemory( HEAPID_PROC,sizeof(CG_HELP_INIT_WORK) );
      initWork->myStatus = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(pWork->pGameSys));
      p_cevent= EVENT_FieldSubProc_Callback(pWork->pGameSys, fieldWork, FS_OVERLAY_ID(cg_help),&CGearHelp_ProcData,initWork,NULL,initWork);
    }
    break;


  case FIELD_SUBSCREEN_ACTION_CGEAR_POWER:
    p_cevent= EVENT_CGearPower(pWork->pGameSys);
    break;
  }

  // イベント変更
  GMEVENT_ChangeEvent( event, p_cevent );
 

  return GMEVENT_RES_CONTINUE;
}


