//============================================================================================
/**
 * @file  wifi_p2pmatch.c
 * @bfief WIFIP2Pマッチング画面
 * @author  k.ohno
 * @date  06.04.07
 */
//============================================================================================

#include <gflib.h>
#include <dwc.h>
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_wifi_lobby.h"

#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/dwc_rapfriend.h"
#include "net_app/wificlub/wifi_p2pmatch.h"
#include "wifi_p2pmatch_local.h"
#include "wifi_p2pmatch_se.h"
#include "wifi_p2pmatchroom.h"


#include "poke_tool/regulation_def.h"

#include "savedata/wifihistory.h"
#include "savedata/config.h"
#include "savedata/battle_box_save.h"
#include "savedata/etc_save.h"

#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"
#include "system/main.h"
#include "system/wipe.h"
#include "system/rtc_tool.h"
#include "system/ds_system.h"

#include "gamesystem/msgspeed.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/pokeparty.h"
#include "item/itemsym.h"
#include "system/net_err.h"
#include "net/dwc_error.h"



#include "font/font.naix"
#include "print/str_tool.h"

#include "wifip2pmatch.naix"      // グラフィックアーカイブ定義
#include "comm_command_wfp2pmf.h"  //２Ｄフィールド
#include "comm_command_wfp2pmf_func.h"  //２Ｄフィールド
#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_system.h"

#include "sound/pm_sndsys.h"  //SOUND関連
#include "sound/pm_wb_voice.h"



// 置き換える必要があるがまだない関数  @@OO
#define SND_HANDLE_FIELD (0)
#define BGM_POKECEN_VOL (0)
#define SND_SCENE_P2P (0)
#define SEQ_WIFILOBBY (0)
#define SND_SCENE_DUMMY (0)
#define BGM_WIFILOBBY_VOL (0)
#define FLAG_ARRIVE_D32R0101 (0)
#define GMDATA_ID_EMAIL (0)
#define ID_PARA_monsno (0)
#define ID_PARA_item (0)
#define NUMBER_DISPTYPE_ZERO (STR_NUM_DISP_ZERO)
#define NUMBER_CODETYPE_DEFAULT (0)
#define FBMP_COL_NULL   (0)
#define FBMP_COL_BLACK    (1)
#define FBMP_COL_BLK_SDW  (2)
#define FBMP_COL_RED    (3)
#define FBMP_COL_RED_SDW  (4)
#define FBMP_COL_GREEN    (5)
#define FBMP_COL_GRN_SDW  (6)
#define FBMP_COL_BLUE   (7)
#define FBMP_COL_BLU_SDW  (8)
#define FBMP_COL_PINK   (9)
#define FBMP_COL_PNK_SDW  (10)
#define FBMP_COL_WHITE    (15)
#define BGM_FADE_VCHAT_TIME (0)
#define BGM_WIFILOBBY_VOL (0)
#define BGM_FADEIN_START_VOL_NOW (0)
#define _BATTLE_TOWER_REC (0)   // バトルタワーの記録表示を封印

#define _OAMBUTTON (0)  //下画面のボタンは要らない
#define MYSTATUS_GETTRAINERVIEW_COMPLETION (0)   //ユニオンの姿が数字で入らないとうまく出ない

typedef struct{
  int a;
} ZUKAN_WORK;


//static void Snd_PlayerSetInitialVolume(int a,int b){}
//static void TimeWaitIconTaskDel(void* c){}
//static void Snd_DataSetByScene( int a, int b, int c ){}
//static void Snd_SceneSet( int a ){}
//static int Snd_NowBgmNoGet(void){ return 0;}
//static void Snd_PlayerSetInitialVolumeBySeqNo( int a,  int b){}
//static void* SaveData_GetEventWork(void* a){ return NULL; }
//static BOOL SysFlag_ArriveGet(void* a,int b){ return TRUE;}
//static void* SaveData_Get(void* a, int b){ return NULL; }
//static void* SaveData_GetFrontier(void* a){ return NULL; }
//static void EMAILSAVE_Init(void* a){}
//static ZUKAN_WORK* SaveData_GetZukanWork(SAVE_CONTROL_WORK* a){ return NULL; }
//static int PokeParaGet( POKEMON_PARAM* poke, int no, void* c ){return 0;}
//static BOOL ZukanWork_GetZenkokuZukanFlag(ZUKAN_WORK* pZukan){ return TRUE; }
//static void CommInfoFinalize(void){}
//static void Snd_SePlay(int a){}
//static void Snd_BgmFadeOut( int a, int b){}
//static void Snd_BgmFadeIn( int a, int b, int c){}
//static void FONTOAM_OAMDATA_Delete( void* x){}
static BOOL _playerDirectConnectStart( WIFIP2PMATCH_WORK *wk );

#ifdef BUGFIX_BTS7868_20100716
#define _TIMEOUT_MW  (60*60)  //一分
#endif

#define COMM_BRIGHTNESS_SYNC  ( 1 ) // 基本の輝度変更Sync数
#define _BMPMENULIST_FONTSIZE   (16)

// WIFI2DMAPシステムオーバーレイ
//FS_EXTERN_OVERLAY(wifi2dmap);

// WIFI　対戦AUTOﾓｰﾄﾞデバック
#ifdef _WIFI_DEBUG_TUUSHIN
extern WIFI_DEBUG_BATTLE_WK WIFI_DEBUG_BATTLE_Work;
#endif  //_WIFI_DEBUG_TUUSHIN


//-------------------------------------
/// マッチング用拡張ヒープサイズ
//=====================================
#define WIFI_P2PMATCH_EXHEAP_SIZE ( 0x10000 )


#define SE_CALL_SIGN  (SEQ_SE_SYS_101)  //SEQ_SE_SYS_101  ,SEQ_SE_W055_01
#define SE_CALL_SIGN_PLAYER  (SEPLAYER_SYS)  //SEQ_SE_SYS_101


/*********************************************************************************************
  メイン画面のCGX割り振り   2006/01/12

    ウィンドウ枠  ：  409 - 511
      会話、メニュー、地名、看板

    BMPウィンドウ１ ：  297 - 408
      会話（最大）、看板、残りボール数

    BMPウィンドウ２ ：  55 - 296
      メニュー（最大）、はい/いいえ、地名

 *********************************************************************************************/

/*********************************************************************************************
  ウィンドウ枠
 *********************************************************************************************/
// 会話ウィンドウキャラ
#define TALK_WIN_CGX_SIZE ( 18+12 )
#define TALK_WIN_CGX_NUM  ( 512 - TALK_WIN_CGX_SIZE )

// メニューウィンドウキャラ
#define MENU_WIN_CGX_SIZE ( 9 )
#define MENU_WIN_CGX_NUM  ( TALK_WIN_CGX_NUM - MENU_WIN_CGX_SIZE )
//#define MENU_WIN_PAL    ( 11 )

// 地名ウィンドウキャラ
#define PLACE_WIN_CGX_SIZE  ( 10 )
#define PLACE_WIN_CGX_NUM ( MENU_WIN_CGX_NUM - PLACE_WIN_CGX_SIZE )
//#define PLACE_WIN_PAL   ( 7 )

// 看板ウィンドウキャラ
#define BOARD_WIN_CGX_SIZE  ( 18+12 + 24 )
#define BOARD_WIN_CGX_NUM ( PLACE_WIN_CGX_NUM - BOARD_WIN_CGX_SIZE )
//#define BOARD_WIN_PAL   ( FLD_BOARD2FRAME_PAL )

/*********************************************************************************************
  BMPウィンドウ
 *********************************************************************************************/
// 会話ウィンドウ（メイン）
#define FLD_MSG_WIN_PX    ( 2 )
#define FLD_MSG_WIN_PY    ( 19 )
#define FLD_MSG_WIN_SX    ( 27 )
#define FLD_MSG_WIN_SY    ( 4 )
#define FLD_MSG_WIN_CGX   ( BOARD_WIN_CGX_NUM - ( FLD_MSG_WIN_SX * FLD_MSG_WIN_SY ) )
// はい/いいえウィンドウ（メイン）（メニューと同じ位置（メニューより小さい））
#define FLD_YESNO_WIN_PX  ( 24 )
#define FLD_YESNO_WIN_PY  ( 13 )
#define FLD_YESNO_WIN_SX  ( 6 )
#define FLD_YESNO_WIN_SY  ( 4 )
#define FLD_YESNO_WIN_CGX ( FLD_MSG_WIN_CGX - ( FLD_YESNO_WIN_SX * FLD_YESNO_WIN_SY ) )



/// ビットマップ転送関数用定義
//------------------------------------------------------------------
#define FBMP_TRANS_OFF  (0)
#define FBMP_TRANS_ON (1)
/// 文字表示転送関数用定義
//------------------------------------------------------------------
#define FBMPMSG_WAITON_SKIPOFF  (0)
#define FBMPMSG_WAITON_SKIPON (1)
#define FBMPMSG_WAITOFF     (2)

//-------------------------------------------------------------------------
/// 文字表示色定義(default) -> gflib/fntsys.hへ移動
//------------------------------------------------------------------

#define WINCLR_COL(col) (((col)<<4)|(col))

//-------------------------------------------------------------------------
/// 文字表示スピード定義(default)
//------------------------------------------------------------------
#define FBMP_MSG_SPEED_SLOW   (8)
#define FBMP_MSG_SPEED_NORMAL (4)
#define FBMP_MSG_SPEED_FAST   (1)



//============================================================================================
//  定数定義
//============================================================================================
enum {
  SEQ_IN = 0,
  SEQ_MAIN,
  SEQ_OUT,
};

enum {
  _MENU_LIST,
  _MENU_INPUT,
  _MENU_MYCODE,
  _MENU_EXIT,
};

typedef enum {
  _CLACT_UP_CUR,
  _CLACT_DOWN_CUR,
  _CLACT_LINE_CUR,
  _CLACT_VCT_STOP,
  _CLACT_VCT_MOVE,
} _OAM_ANIM_E;

typedef struct{  // スクリーン用RECT構造体
  u8 lt_x;
  u8 lt_y;
  u8 rb_x;
  u8 rb_y;
} _SCR_RECT;

#define WF_CLACT_WKNUM  ( 8 )
#define WF_CLACT_RESNUM ( 1 )
#define WF_FONTOAM_NUM  ( 1 )

#define _PRINTTASK_MAX (8)
#define _TIMING_GAME_CHECK  (13)// つながった直後
#define _TIMING_GAME_CHECK2  (14)// つながった直後
#define _TIMING_GAME_START  (15)// タイミングをそろえる
#define _TIMING_GAME_START2  (18)// タイミングをそろえる
#define _TIMING_BATTLE_END  (16)// タイミングをそろえる
#define _TIMING_POKEPARTY_END  (17)// タイミングをそろえる
#define _TIMING_VCTEND  (18)// タイミングをそろえる
#define _TIMING_VCTST  (19)// タイミングをそろえる
#define _TIMING_DISCONNECT_END  (21)// タイミングをそろえる
#define _TIMING_SECOND_MATCH  (24)// タイミングをそろえる
#define _TIMING_CEND (25)// タイミングをそろえる

#define _TIMING_DIRECTE (26)// タイミングをそろえる
#define _TIMING_DIRECTS (27)// タイミングをそろえる


#define _RECONECTING_WAIT_TIME (20)  //再接続時間


// ユーザー表示面の設定
#define WIFIP2PMATCH_PLAYER_DISP_X  ( 1 )
#define WIFIP2PMATCH_PLAYER_DISP_Y  ( 1 )
#define WIFIP2PMATCH_PLAYER_DISP_SIZX ( 28 )
#define WIFIP2PMATCH_PLAYER_DISP_SIZY ( 2 )
#define WIFIP2PMATCH_PLAYER_DISP_NAME_X ( 32 )
#define WIFIP2PMATCH_PLAYER_DISP_ST_X ( 102 )
#define WIFIP2PMATCH_PLAYER_DISP_WINSIZ (WIFIP2PMATCH_PLAYER_DISP_SIZX*WIFIP2PMATCH_PLAYER_DISP_SIZY)


//FRAME3
#define _CGX_BOTTOM ( 512 )             // 会話ウィンドウキャラ
#define _CGX_TITLE_BOTTOM (_CGX_BOTTOM - 18*3)  // タイトルウィンドウ
#define _CGX_USER_BOTTOM  (_CGX_TITLE_BOTTOM - WIFIP2PMATCH_PLAYER_DISP_WINSIZ) // ユーザーウィンドウ

// FRAME1に転送するユーザーデータ背景
#define _CGX_USET_BACK_BOTTOM ( MENU_WIN_CGX_NUM - WIFIP2PMATCH_PLAYER_DISP_WINSIZ )


// FRAME1に転送するICONデータ
#define PLAYER_DISP_ICON_PLTTOFS  (8)
#define PLAYER_DISP_ICON_PLTTOFS_SUB (8)  //9-10-11-12

#define COMM_MESFONT_PAL      ( 14 )     //  ウインドウのパレット
#define MCV_SYSFONT_PAL       ( 15 )     // システムフォントのパレット
#define COMM_MESFRAME_PAL     ( 14 )         //  ウインドウ
#define FLD_YESNO_WIN_PAL     ( 15 )  //フォントのパレット
#define MENU_WIN_PAL          ( 14 )   //ウインドウ


//MCV_SYSFONT_PAL



#define PLAYER_DISP_ICON_PLTTNUM  (4)


#define PLAYER_DISP_ICON_CG_SIZX  (12)
#define PLAYER_DISP_ICON_CG_SIZY  (4)
#define PLAYER_DISP_ICON_CGX    ( 0 )
#define PLAYER_DISP_ICON_SCRN_X ( 2 )
#define PLAYER_DISP_ICON_SCRN_Y ( 2 )
#define PLAYER_DISP_ICON_POS_X  ( 2 )
#define PLAYER_DISP_ICON_POS_Y  ( 1 )
#define PLAYER_DISP_VCTICON_POS_X ( 26 )
#define PLAYER_DISP_VCTICON_POS_Y ( 1 )
enum{
  PLAYER_DISP_ICON_IDX_NONE_NUKI = 0,  //抜き色
  PLAYER_DISP_ICON_IDX_NORMAL_ACT,
  PLAYER_DISP_ICON_IDX_NORMAL,
  PLAYER_DISP_ICON_IDX_VCT_ACT,
  PLAYER_DISP_ICON_IDX_VCT,
  PLAYER_DISP_ICON_IDX_TVT_ACT,
  PLAYER_DISP_ICON_IDX_TVT,
  PLAYER_DISP_ICON_IDX_FIGHT_ACT,
  PLAYER_DISP_ICON_IDX_FIGHT,
  PLAYER_DISP_ICON_IDX_CHANGE_ACT,
  PLAYER_DISP_ICON_IDX_CHANGE,
  PLAYER_DISP_ICON_IDX_VCTNOT,
  PLAYER_DISP_ICON_IDX_VCTBIG,
  PLAYER_DISP_ICON_IDX_UNK_ACT,      //その他不明
  PLAYER_DISP_ICON_IDX_UNK,      //その他不明
  PLAYER_DISP_ICON_IDX_NONE,  //空
  PLAYER_DISP_ICON_IDX_NUM,    //数
};


#define PLAYER_DISP_ICON_MYMODE (PLAYER_DISP_ICON_IDX_NUM*3)
#define PLAYER_DISP_ICON_TOUCHMODE (PLAYER_DISP_ICON_IDX_NUM*1)
#define PLAYER_DISP_ICON_NAMEMODE (0)
#define PLAYER_DISP_ICON_CARDMODE (PLAYER_DISP_ICON_IDX_NUM*2)


// 会話ウィンドウ FRAME2
//#define COMM_MENUFRAME_PAL    ( 11 )         //  メニューウインドウ
//#define COMM_SYSFONT_PAL    ( 13 )         //  システムフォント

#define COMM_TALK_WIN_CGX_SIZE  ( 18+12 )
//#define COMM_TALK_WIN_CGX_NUM ( 512 - COMM_TALK_WIN_CGX_SIZE)
#define COMM_TALK_WIN_CGX_NUM ( 48 )

#define COMM_MSG_WIN_PX   ( 2 )
#define COMM_MSG_WIN_PY   ( 19 )
#define COMM_MSG_WIN_SX   ( 27 )
#define COMM_MSG_WIN_SY   ( 4 )
#define COMM_MSG_WIN_PAL    ( COMM_MESFONT_PAL )
//#define COMM_MSG_WIN_CGX    ( (COMM_TALK_WIN_CGX_NUM - 73) - ( COMM_MSG_WIN_SX * COMM_MSG_WIN_SY ) )

#define COMM_SYS_WIN_PX   ( 4 )
#define COMM_SYS_WIN_PY   ( 4 )
#define COMM_SYS_WIN_SX   ( 23 )
#define COMM_SYS_WIN_SY   ( 16 )
#define COMM_SYS_WIN_PAL    ( COMM_MESFONT_PAL )
#define COMM_SYS_WIN_CGX    ( MENU_WIN_CGX_NUM - ( COMM_SYS_WIN_SX * COMM_SYS_WIN_SY ) )  // 通信システムウィンドウ転送先


#define REGWINDOW_WIN_PX   ( 2 )
#define REGWINDOW_WIN_PY   ( 5 )
#define REGWINDOW_WIN_SX   ( 26 )
#define REGWINDOW_WIN_SY   ( 16 )


// YesNoWin
#define YESNO_WIN_CGX ((COMM_SYS_WIN_CGX - (FLD_YESNO_WIN_SX*FLD_YESNO_WIN_SY)))
#define FRAME1_YESNO_WIN_CGX  ((_CGX_USET_BACK_BOTTOM - (FLD_YESNO_WIN_SX*FLD_YESNO_WIN_SY)))


// メインメッセージキャラ
#define _NUKI_FONT_PALNO  (13)
#define _COL_N_BLACK  ( GFL_FONTSYS_SetColor( 1, 2, 0 ) )   // フォントカラー：黒
#define _COL_N_WHITE  ( GFL_FONTSYS_SetColor( 15, 2, 0 ) )   // フォントカラー：白
#define _COL_N_RED      ( GFL_FONTSYS_SetColor( 3, 4, 0 ) )   // フォントカラー：青
#define _COL_N_BLUE     ( GFL_FONTSYS_SetColor( 5, 6, 0 ) )   // フォントカラー：赤
#define _COL_N_GRAY   ( GFL_FONTSYS_SetColor( 1, 2, 0 ) )    // フォントカラー：灰

// 名前表示BMP（上画面）
#define WIFIP2PMATCH_NAME_BMP_W  ( 16 )
#define WIFIP2PMATCH_NAME_BMP_H  (  2 )
#define WIFIP2PMATCH_NAME_BMP_SIZE (WIFIP2PMATCH_NAME_BMP_W * WIFIP2PMATCH_NAME_BMP_H)

// 会話ウインドウ表示位置定義
#define WIFIP2PMATCH_TALK_X   (  2 )
#define WIFIP2PMATCH_TALK_Y   (  19 )

#define WIFIP2PMATCH_TITLE_X    (   3  )
#define WIFIP2PMATCH_TITLE_Y    (   1  )
#define WIFIP2PMATCH_TITLE_W    (  26  )
#define WIFIP2PMATCH_TITLE_H    (   2  )


#define WIFIP2PMATCH_MSG_WIN_OFFSET     (1 + TALK_WIN_CGX_SIZ     + MENU_WIN_CGX_SIZ)
#define WIFIP2PMATCH_TITLE_WIN_OFFSET   ( WIFIP2PMATCH_MSG_WIN_OFFSET   + FLD_MSG_WIN_SX*FLD_MSG_WIN_SY )
#define WIFIP2PMATCH_NAME_WIN_OFFSET    ( WIFIP2PMATCH_TITLE_WIN_OFFSET + WIFIP2PMATCH_TITLE_W*WIFIP2PMATCH_TITLE_H )
#define WIFIP2PMATCH_YESNO_WIN_OFFSET   ( WIFIP2PMATCH_NAME_WIN_OFFSET  + WIFIP2PMATCH_NAME_BMP_SIZE*5 )


// VIEW面
enum{
  MCV_BTTN_FRIEND_TYPE_NONE,  // 設定していない
  MCV_BTTN_FRIEND_TYPE_RES, // 予約
  MCV_BTTN_FRIEND_TYPE_IN,    // 登録済み
  MCV_BTTN_FRIEND_TYPE_MAX, // タイプ最大値
};
#define MCV_PAL_BACK    ( 0 ) // 背景パレットの開始位置
#define MCV_PAL_FRMNO   ( 0 ) // 背景のバットパレット開始位置
#define MCV_PAL_BTTN    ( 0 ) // ボタンパレットの開始位置
enum{
  //  MCV_PAL_BACK_0 = 0,
  //MCV_PAL_BACK_NUM,

  MCV_PAL_BTTN_GIRL = 0,
  MCV_PAL_BTTN_MAN,
  MCV_PAL_BTTN_NONE,
  MCV_PAL_BTTNST_GIRL,
  MCV_PAL_BTTNST_MAN,
  MCV_PAL_BTTNST_DUMMY1,
  MCV_PAL_BTTNST_DUMMY2,
  MCV_PAL_BTTNST_DUMMY3,
  MCV_PAL_BTTN_NUM  // 今のところ余りは
    // MCV_PAL_BTTN+MCV_PAL_BTTN_NUM〜(PLAYER_DISP_ICON_PLTTOFS_SUB-1まで
};
// アイコンの転送位置
#define MCV_ICON_CGX  (0)
#define MCV_ICON_CGSIZ  (32*4)
#define MCV_ICON_PAL    (PLAYER_DISP_ICON_PLTTOFS_SUB)

#define MCV_CGX_BTTN2 (MCV_ICON_CGX+MCV_ICON_CGSIZ) // FRAME2ユーザーデータ
#define MCV_CGX_BACK  (0)// FRAME0背景
// WIN設定
#define MCV_NAMEWIN_CGX   ( 1 ) // 名前ウィンドウ開始位置
#define MCV_NAMEWIN_DEFX  ( 4 ) // 基本位置
#define MCV_NAMEWIN_DEFY  ( 1 )
#define MCV_NAMEWIN_OFSX  ( 16 )  // 位置の移動値
#define MCV_NAMEWIN_OFSY  ( 6 )
#define MCV_NAMEWIN_SIZX  ( 9 ) // サイズ
#define MCV_NAMEWIN_SIZY  ( 3 )
#define MCV_NAMEWIN_CGSIZ ( MCV_NAMEWIN_SIZX*MCV_NAMEWIN_SIZY ) // CGサイズ
#define MCV_NAMEWIN_CGALLSIZ  ((MCV_NAMEWIN_CGSIZ*WCR_MAPDATA_1BLOCKOBJNUM)+MCV_NAMEWIN_CGX)// CGそうサイズ

#define MCV_STATUSWIN_CGX ( MCV_NAMEWIN_CGALLSIZ+1 )
#define MCV_STATUSWIN_DEFX  ( 1 ) // 基本位置
#define MCV_STATUSWIN_DEFY  ( 1 )
#define MCV_STATUSWIN_VCHATX ( 12 ) // VCHAT位置
#define MCV_STATUSWIN_OFSX  ( 16 )  // 位置の移動値
#define MCV_STATUSWIN_OFSY  ( 6 )
#define MCV_STATUSWIN_SIZX  ( 2 ) // サイズ
#define MCV_STATUSWIN_SIZY  ( 3 )
#define MCV_STATUSWIN_CGSIZ ( MCV_STATUSWIN_SIZX*MCV_STATUSWIN_SIZY ) // CGサイズ

#define MCV_USERWIN_CGX   ( 1 )
#define MCV_USERWIN_X   ( 1 )
#define MCV_USERWIN_Y   ( 5 )
#define MCV_USERWIN_SIZX  ( 30 )
#define MCV_USERWIN_SIZY  ( 21-4 )

#define MCV_USERWIN_STA_X   ( 1 )
#define MCV_USERWIN_STA_Y   ( 1 )
#define MCV_USERWIN_STA_SIZX  ( 30 )
#define MCV_USERWIN_STA_SIZY  ( 2 )



#define MCV_BUTTON_SIZX ( 16 )
#define MCV_BUTTON_SIZY ( 6 )
#define MCV_BUTTON_DEFX (0)
#define MCV_BUTTON_DEFY (0)
#define MCV_BUTTON_OFSX (16)
#define MCV_BUTTON_OFSY (6)

#define MCV_BUTTON_ICON_OFS_X ( 1 )
#define MCV_BUTTON_VCTICON_OFS_X  ( 13 )
#define MCV_BUTTON_ICON_OFS_Y ( 1 )

#define MCV_BUTTON_FRAME_NUM  (16) // ボタンアニメフレーム数

enum{
  MCV_BUTTON_TYPE_MAN,
  MCV_BUTTON_TYPE_GIRL,
  MCV_BUTTON_TYPE_NONE,
};

enum{
  MCV_USERDISP_OFF, // ユーザー表示OFF
  MCV_USERDISP_INIT,  // ユーザー表示初期化
  MCV_USERDISP_ON,  // ユーザー表示ON
  MCV_USERDISP_ONEX,  // ユーザー表示強制ON
};

#define VRANTRANSFERMAN_NUM (32)  // VramTransferManagerタスク数


enum{
  MCV_USERD_BTTN_ANM_LEFT_PUSH = 0,
  MCV_USERD_BTTN_ANM_LEFT_RELEASE,
  MCV_USERD_BTTN_ANM_BACK_PUSH,
  MCV_USERD_BTTN_ANM_BACK_RELEASE,
  MCV_USERD_BTTN_ANM_RIGHT_PUSH,
  MCV_USERD_BTTN_ANM_RIGHT_RELEASE,
};

enum{
  MCV_USERD_BTTN_MODE_WAIT,
  MCV_USERD_BTTN_MODE_NML,
  MCV_USERD_BTTN_MODE_NOBACK,
};

#define MCV_USERD_BTTN_RESCONTID  ( 30 )  // リソースID
#define MCV_USERD_BTTN_BGPRI    (0)   // BG優先順位
#define MCV_USERD_BTTN_PRI      (128)   // BG優先順位
#define MCV_USERD_BTTN_Y      (172) // ボタンY位置
#define MCV_USERD_BTTN_LEFT_X   (40)  // ←
#define MCV_USERD_BTTN_BACK_X   (128) // もどる
#define MCV_USERD_BTTN_RIGHT_X    (224) // →
#define MCV_USERD_BTTN_FONT_X   ( -18 ) // もどる　文字位置
#define MCV_USERD_BTTN_FONT_Y   ( -8 )  // もどる　文字位置
#define MCV_USERD_BTTN_FONT_SIZX  ( 8 ) // もどる　文字描画範囲
#define MCV_USERD_BTTN_FONT_SIZY  ( 2 ) // もどる　文字描画範囲
#define MCV_USERD_BTTN_FONT_CGXOFS  ( 0 ) // CGXオフセット
#define MCV_USERD_BTTN_FONT_COL   ( 0 ) // カラーパレット
#define MCV_USERD_BTTN_FONT_OAMPRI  ( 0 )
#define MCV_USERD_BTTN_LEFT_SIZX  ( 56 )  // ←サイズ
#define MCV_USERD_BTTN_BACK_SIZX  ( 96 )  // もどるサイズ
#define MCV_USERD_BTTN_RIGHT_SIZX ( 56 )  // →さいず
#define MCV_USERD_BTTN_SIZY     ( 32 )  // 縦さいず
#define MCV_USERD_BTTN_LEFT_HIT_X ( 8 ) // ←あたり判定用X
#define MCV_USERD_BTTN_BACK_HIT_X ( 80 )  // ←あたり判定用X
#define MCV_USERD_BTTN_RIGHT_HIT_X  ( 192 ) // ←あたり判定用X
#define MCV_USERD_BTTN_RIGHT_HIT_Y  ( 160 ) // ←あたり判定用Y
#define MCV_USERD_BTTN_ANMPUSHOK  ( 3 ) // 押したと判断するボタンのアニメ
#define MCV_USERD_BTTN_ANMMAX   ( 5 ) // アニメ内フレーム数
#define MCV_USERD_BTTN_ANMMAX_0ORG  ( MCV_USERD_BTTN_ANMMAX-1 ) // アニメ内フレーム数

// FONTOAMY座標アニメデータ
static const s8 c_MCV_USER_BTTN_FONT_YANM[ MCV_USERD_BTTN_ANMMAX ] = {
  MCV_USERD_BTTN_FONT_Y,
  MCV_USERD_BTTN_FONT_Y - 1,
  MCV_USERD_BTTN_FONT_Y - 2,
  MCV_USERD_BTTN_FONT_Y - 2,
  MCV_USERD_BTTN_FONT_Y - 1,
};

// ボタンシーケンス　PUSHアニメ
static const u8 c_MCV_USER_BTTN_ANM_PUSH[ MCV_USERD_BTTN_NUM ] = {
  MCV_USERD_BTTN_ANM_LEFT_PUSH,
  MCV_USERD_BTTN_ANM_BACK_PUSH,
  MCV_USERD_BTTN_ANM_RIGHT_PUSH,
};

// ボタンシーケンス　RELEASEアニメ
static const u8 c_MCV_USER_BTTN_ANM_RELEASE[ MCV_USERD_BTTN_NUM ] = {
  MCV_USERD_BTTN_ANM_LEFT_RELEASE,
  MCV_USERD_BTTN_ANM_BACK_RELEASE,
  MCV_USERD_BTTN_ANM_RIGHT_RELEASE,
};

static const GFL_UI_TP_HITTBL c_MCV_USER_BTTN_OAM_Hit[ ] = {
  { // ←
    MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
    MCV_USERD_BTTN_LEFT_HIT_X, MCV_USERD_BTTN_LEFT_HIT_X+MCV_USERD_BTTN_LEFT_SIZX,
  },
  { // もどる
    MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
    MCV_USERD_BTTN_BACK_HIT_X, MCV_USERD_BTTN_BACK_HIT_X+MCV_USERD_BTTN_BACK_SIZX,
  },
  { // →
    MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
    MCV_USERD_BTTN_RIGHT_HIT_X, MCV_USERD_BTTN_RIGHT_HIT_X+MCV_USERD_BTTN_RIGHT_SIZX,
  },
  {GFL_UI_TP_HIT_END,0,0,0},     //終了データ
};
enum{
  MCV_USERD_BTTN_RET_NONE,  // 何の反応もなし
  MCV_USERD_BTTN_RET_LEFT,  // 左がおされた
  MCV_USERD_BTTN_RET_BACK,  // もどるがおされた
  MCV_USERD_BTTN_RET_RIGHT, // 右がおされた
};


static GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,       // メイン2DエンジンのBG
  GX_VRAM_BGEXTPLTT_NONE,     // メイン2DエンジンのBG拡張パレット

  GX_VRAM_SUB_BG_128_C,     // サブ2DエンジンのBG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // サブ2DエンジンのBG拡張パレット

  GX_VRAM_OBJ_128_B,        // メイン2DエンジンのOBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // メイン2DエンジンのOBJ拡張パレット

  GX_VRAM_SUB_OBJ_128_D,     // サブ2DエンジンのOBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // サブ2DエンジンのOBJ拡張パレット

  GX_VRAM_TEX_NONE,       // テクスチャイメージスロット
  GX_VRAM_TEXPLTT_NONE,     // テクスチャパレットスロット

  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K,

};

#ifdef WFP2P_DEBUG  // 人をいっぱい出す
//#define WFP2PM_MANY_OBJ
#endif


static void _debugChangeState(WIFIP2PMATCH_WORK* wk, int state, int line)
{
  OS_TPrintf("p2p: %d\n",line);
  wk->seq = state;
}

#if PM_DEBUG
#define   _CHANGESTATE(wk,state) _debugChangeState(wk,state,__LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGESTATE(wk,state)  wk->seq = state
#endif //GFL_NET_DEBUG

//static int _seqBackup;

//============================================================================================
//  プロトタイプ宣言
//============================================================================================
//volume変更
static void _changeBGMVol( WIFIP2PMATCH_WORK *wk, u8 endVol);
static void _initBGMVol( WIFIP2PMATCH_WORK* wk, int status);

static WIFI_STATUS* WifiFriendMatchStatusGet( u32 idx );
static u8 WifiDwc_getFriendStatus( int idx );

static BOOL WifiP2PMatch_CommWifiBattleStart( WIFIP2PMATCH_WORK* wk, int friendno );

/*** 関数プロトタイプ ***/
static void VBlankFunc( GFL_TCB *tcb , void * work );
static void VramBankSet(void);
static void BgInit( HEAPID heapID );
static void InitMessageWork( WIFIP2PMATCH_WORK *wk );
static void FreeMessageWork( WIFIP2PMATCH_WORK *wk );
static void BgExit( WIFIP2PMATCH_WORK *wk );
static void BgGraphicSet( WIFIP2PMATCH_WORK * wk, ARCHANDLE* p_handle );
static void char_pltt_manager_init(void);
static void InitCellActor(WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle);
static void BmpWinInit(WIFIP2PMATCH_WORK *wk, GFL_PROC* proc);
static void MainMenuMsgInit(WIFIP2PMATCH_WORK *wk);
static void SetCursor_Pos( GFL_CLWK* act, int x, int y );
static void WindowSet(void);

// マッチングルーム管理用関数
static void MCRSYS_SetMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj );
static void MCRSYS_DelMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj );
static void MCRSYS_SetFriendObj( WIFIP2PMATCH_WORK* wk, u32 heapID );
static int MCRSYS_ContFiendInOut( WIFIP2PMATCH_WORK* wk );
static void MCRSYS_ContFriendStatus( WIFIP2PMATCH_WORK* wk, u32 heapID );
static MCR_MOVEOBJ* MCRSYS_GetMoveObjWork( WIFIP2PMATCH_WORK* wk, u32 friendNo );

static void WifiP2PMatchFriendListIconLoad( WIFIP2PMATCH_ICON* p_data, ARCHANDLE* p_handle, u32 heapID );
static void WifiP2PMatchFriendListIconRelease( WIFIP2PMATCH_ICON* p_data );
static void WifiP2PMatchFriendListIconWrite(  WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 icon_type, u32 col );
static void WifiP2PMatchFriendListStIconWrite( WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 status,u32 gamemode, u32 iconmode  );
static void WifiP2PMatchFriendListBmpIconWrite(  GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 icon_type, u32 col,int pal );
static void WifiP2PMatchFriendListBmpStIconWrite( GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 status,u32 gamemode,int pal );
static int WifiP2PMatchBglFrmIconPalGet( u32 frm );

static int WifiP2PMatchFriendListStart( WIFIP2PMATCH_WORK* wk,BOOL bWipe );

// 友達データの強制表示処理
static void WifiP2PMatch_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID );
static void WifiP2PMatch_UserDispOff( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void WifiP2PMatch_UserDispOff_Target( WIFIP2PMATCH_WORK *wk, u32 target_friend, u32 heapID );
static void WifiP2PMatch_UserDispOff_Any( WIFIP2PMATCH_WORK *wk, u32 heapID );

static void WifiP2PMatch_UserDispOn_MyAcces( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID );
static void WifiP2PMatch_UserDispOff_MyAcces( WIFIP2PMATCH_WORK *wk, u32 heapID );

// 友達データビューアー
static BOOL MCVSys_MoveCheck( const WIFIP2PMATCH_WORK *wk );
static void MCVSys_Init( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID );
static void MCVSys_Exit( WIFIP2PMATCH_WORK *wk );
static u32 MCVSys_Updata( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UpdataBttn( WIFIP2PMATCH_WORK *wk );
static BOOL MCVSys_UserDispEndCheck( WIFIP2PMATCH_WORK *wk );
static u32  MCVSys_UserDispGetFriend( const WIFIP2PMATCH_WORK* cp_wk );
static void MCVSys_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID );
static void MCVSys_UserDispOff( WIFIP2PMATCH_WORK *wk );
static void MCVSys_UserDispPageChange( WIFIP2PMATCH_WORK *wk, u32 oambttn_ret );
static void MCVSys_BttnSet( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 type );
static void MCVSys_BttnDel( WIFIP2PMATCH_WORK *wk, u32 friendNo );
static u32 MCVSys_BttnTypeGet( const WIFIP2PMATCH_WORK *wk, u32 friendNo );
static void MCVSys_ReWrite( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_BttnAllWriteReq( WIFIP2PMATCH_WORK *wk );

static void MCVSys_BttnCallBack( u32 bttnid, u32 event, void* p_work );
static void MCVSys_BttnAnmMan( WIFIP2PMATCH_WORK *wk );
static void MCVSys_GraphicSet( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID );
static void MCVSys_GraphicDel( WIFIP2PMATCH_WORK *wk );
static void MCVSys_GraphicScrnCGOfsChange( NNSG2dScreenData* p_scrn, u8 cgofs );
static void MCVSys_BackDraw( WIFIP2PMATCH_WORK *wk );
static void MCVSys_BttnDraw( WIFIP2PMATCH_WORK *wk );
static void MCVSys_UserDispDraw_Renew( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDraw( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType00( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispFrontiorNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 factoryid, u32 friendno, u32 x, u32 y );
static void MCVSys_UserDispFrontiorTitleStrGet( WIFIP2PMATCH_WORK *wk, STRBUF* p_str, u32 factoryid, u32 friendno );
static void MCVSys_UserDispNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 num, u32 x, u32 y );
static void MCVSys_BttnWrite( WIFIP2PMATCH_WORK *wk, u8 cx, u8 cy, u8 type, u8 frame );
static u32 MCVSys_StatusMsgIdGet( u32 status,u32 gamemode, int* col );
static void MCVSys_BttnWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN* p_bmp, u32 friendNo, u32 frame, u32 area_id );
static void MCVSys_BttnStatusWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN** p_stbmp, u32 friendNo, u32 frame, u32 x, u32 y );
static void MCVSys_OamBttnOn( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOnNoBack( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOff( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnObjAnmStart( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no );
static BOOL MCVSys_OamBttnObjAnmMain( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no, u32 push_bttn, u32 event );
static void MCVSys_FriendNameSet( WIFIP2PMATCH_WORK* p_wk, int friendno );

static void BmpWinDelete( WIFIP2PMATCH_WORK *wk );
static void _systemMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno );
//static void WifiP2PMatchMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno , BOOL bSystem);
static void EndMessageWindowOff( WIFIP2PMATCH_WORK *wk );
static void WifiP2PMatchDataSendRecv( WIFIP2PMATCH_WORK *wk );
static void SequenceChange_MesWait( WIFIP2PMATCH_WORK *wk, int next );
static void _myStatusChange(WIFIP2PMATCH_WORK *wk, int status,int gamemode);
static void _myStatusChange_not_send(WIFIP2PMATCH_WORK *wk, int status,int gamemode);
static u32 _WifiMyStatusGet( WIFIP2PMATCH_WORK * p_wk, WIFI_STATUS* p_status );
static u32 _WifiMyGameModeGet( WIFIP2PMATCH_WORK * p_wk, WIFI_STATUS* p_status );
static BOOL _regulationCheck(WIFIP2PMATCH_WORK * wk);
static void MCRSYS_AllDelMoveObjWork( WIFIP2PMATCH_WORK* wk );


static void WifiP2P_SetLobbyBgm( void );



static void _errorDisp(WIFIP2PMATCH_WORK* wk);
static void _windelandSEcall(WIFIP2PMATCH_WORK *wk);

static void FriendRequestWaitOn( WIFIP2PMATCH_WORK* wk, BOOL msg_on );
static void FriendRequestWaitOff( WIFIP2PMATCH_WORK* wk );
static BOOL FriendRequestWaitFlagGet( const WIFIP2PMATCH_WORK* cp_wk );


// FuncTableからシーケンス遷移で呼ばれる関数
static int WifiP2PMatch_MainInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _normalConnectYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _normalConnectWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _differMachineInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _differMachineNext( WIFIP2PMATCH_WORK *wk, int seq );
static int _differMachineOneMore( WIFIP2PMATCH_WORK *wk, int seq );
static int _firstYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _retryInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _retryYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _retryWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _retry( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_ReConnectingWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_ConnectingInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_Connecting( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FirstSaving( WIFIP2PMATCH_WORK *wk, int seq );
static int _firstConnectEndMsg( WIFIP2PMATCH_WORK *wk, int seq );
static int _firstConnectEndMsgWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FriendListInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FriendListInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FriendListMain( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnect( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectEndYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectEndWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTDisconnect(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_BattleDisconnect(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_Disconnect(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_BattleConnectInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BattleConnectWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BattleConnect( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_MainReturn( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectMenuInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentRestart( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSubSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectRelInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectRelYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectRelWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuLoop( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelYesNoVCT( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelWaitVCT( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuSend( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuCheck( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMyStatusWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _personalDataInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _personalDataWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _personalDataEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitExiting( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int _nextBattleYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _nextBattleWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _vchatNegoCheck( WIFIP2PMATCH_WORK *wk, int seq );
static int _vchatNegoWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _playerDirectInit( WIFIP2PMATCH_WORK *wk, int seq );
static int MessageEndReturnList( WIFIP2PMATCH_WORK *wk, int seq );
static int ReturnList( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectMenuInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuSendD( WIFIP2PMATCH_WORK *wk, int seq );
static int _DirectConnectWait( WIFIP2PMATCH_WORK *wk, int seq  );
static int _DirectConnectWait2( WIFIP2PMATCH_WORK *wk, int seq  );
static int WifiP2PMatch_Disconnect2(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_VCTDisconnectSend1(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_VCTDisconnectSend2(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_VCTDisconnectSend3(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_FriendListMain_MW( WIFIP2PMATCH_WORK *wk, int seq );

static int WifiP2PMatch_ReconectingWaitPre(WIFIP2PMATCH_WORK *wk, int seq);


static BOOL _myVChatStatusToggle(WIFIP2PMATCH_WORK *wk);
static BOOL _myVChatStatusToggleOrg(WIFIP2PMATCH_WORK *wk);
static BOOL _myVChatStatusOrgSet(WIFIP2PMATCH_WORK *wk);
static int _vchatToggleWait( WIFIP2PMATCH_WORK *wk, int seq );
static BOOL _tradeNumCheck(WIFIP2PMATCH_WORK * wk);
static int _childModeConnect( WIFIP2PMATCH_WORK *wk, int seq );


static int WifiP2PMatch_EndWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CheckAndEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CancelEnableWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CancelEnableWaitDP( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FirstSaving2( WIFIP2PMATCH_WORK *wk, int seq );
static int _callGameInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectMain( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTDisconnectSendEnd(WIFIP2PMATCH_WORK *wk, int seq);
static int _modeTVT1YesNo( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT1Wait( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT2YesNo( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT2Wait( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT3YesNo( WIFIP2PMATCH_WORK* wk, int seq );
static int _modeTVT3Wait( WIFIP2PMATCH_WORK* wk, int seq );
static int _playerDirectInit0Next( WIFIP2PMATCH_WORK *wk, int seq );
static void _FriendFloorInit( WIFIP2PMATCH_WORK *wk );
static int _wifip2pmatch_mode_friendlist_mw_wait( WIFIP2PMATCH_WORK *wk, int seq );


#include "wifi_p2pmatch_message.c"
#include "wifi_p2pmatch_direct.c"
#include "wifi_p2pmatch_machine.c"



static void* _getMyUserData(void* pWork)  //DWCUserData
{
  WIFIP2PMATCH_WORK *wk = (WIFIP2PMATCH_WORK*)pWork;
  return WifiList_GetMyUserInfo(GAMEDATA_GetWiFiList(wk->pGameData));
}

static void* _getFriendData(void* pWork)  //DWCFriendData
{
  WIFIP2PMATCH_WORK *wk = (WIFIP2PMATCH_WORK*)pWork;
  return WifiList_GetDwcDataPtr(GAMEDATA_GetWiFiList(wk->pGameData),0);
}

static void _deleteFriendList(int deletedIndex,int srcIndex, void* pWork)
{
  WIFIP2PMATCH_WORK *wk = (WIFIP2PMATCH_WORK*)pWork;

  WifiList_DataMarge(GAMEDATA_GetWiFiList(wk->pGameData), deletedIndex, srcIndex);
}


// レコードコーナーメインシーケンス用関数配列定義
static int (*FuncTable[])(WIFIP2PMATCH_WORK *wk, int seq)={
  WifiP2PMatch_MainInit,  // WIFIP2PMATCH_MODE_INIT  = 0,
  _retryInit,        //WIFIP2PMATCH_RETRY_INIT
  _retryYesNo,        //WIFIP2PMATCH_RETRY_YESNO
  _retryWait,         //WIFIP2PMATCH_RETRY_WAIT
  _retry,             //WIFIP2PMATCH_RETRY
  WifiP2PMatch_ConnectingInit,        // WIFIP2PMATCH_CONNECTING_INIT,
  WifiP2PMatch_Connecting,            // WIFIP2PMATCH_CONNECTING,
  _firstConnectEndMsg,                 //WIFIP2PMATCH_FIRST_ENDMSG
  _firstConnectEndMsgWait,    //WIFIP2PMATCH_FIRST_ENDMSG_WAIT
  WifiP2PMatch_FriendListInit,        // WIFIP2PMATCH_FRIENDLIST_INIT,
  WifiP2PMatch_FriendListInit2, //WIFIP2PMATCH_FRIENDLIST_INIT2
  WifiP2PMatch_FriendListMain,            // WIFIP2PMATCH_MODE_FRIENDLIST
  _vchatToggleWait,                   // WIFIP2PMATCH_VCHATWIN_WAIT
  WifiP2PMatch_VCTConnectInit2,        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2
  WifiP2PMatch_VCTConnectInit,        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT
  WifiP2PMatch_VCTConnectWait,        // WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT
  WifiP2PMatch_VCTConnect,        // WIFIP2PMATCH_MODE_VCT_CONNECT
  WifiP2PMatch_VCTConnectEndYesNo,   // WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO
  WifiP2PMatch_VCTConnectEndWait,   // WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT
  WifiP2PMatch_VCTDisconnect,   // WIFIP2PMATCH_MODE_VCT_DISCONNECT
  WifiP2PMatch_BattleDisconnect,   //WIFIP2PMATCH_MODE_BATTLE_DISCONNECT
  WifiP2PMatch_Disconnect,  //WIFIP2PMATCH_MODE_DISCONNECT
  WifiP2PMatch_BattleConnectInit,     // WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT
  WifiP2PMatch_BattleConnectWait,     // WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT
  WifiP2PMatch_BattleConnect,        // WIFIP2PMATCH_MODE_BATTLE_CONNECT
  WifiP2PMatch_MainReturn,            // WIFIP2PMATCH_MODE_MAIN_MENU
  WifiP2PMatch_EndWait,         // WIFIP2PMATCH_MODE_END_WAIT,
  WifiP2PMatch_CheckAndEnd,          //  WIFIP2PMATCH_MODE_CHECK_AND_END
  _parentModeSelectMenuInit,  // WIFIP2PMATCH_MODE_SELECT_INIT
  _parentModeSelectMenuWait,  // WIFIP2PMATCH_MODE_SELECT_WAIT
  _parentModeSubSelectMenuWait, // WIFIP2PMATCH_MODE_SUBBATTLE_WAIT
  _parentModeSelectRelInit,  //WIFIP2PMATCH_MODE_SELECT_REL_INIT
  _parentModeSelectRelYesNo, //WIFIP2PMATCH_MODE_SELECT_REL_YESNO
  _parentModeSelectRelWait,  //WIFIP2PMATCH_MODE_SELECT_REL_WAIT
  _childModeMatchMenuInit,   //WIFIP2PMATCH_MODE_MATCH_INIT
  _childModeMatchMenuInit2,   //WIFIP2PMATCH_MODE_MATCH_INIT2
  _childModeMatchMenuWait,   //WIFIP2PMATCH_MODE_MATCH_WAIT
  _childModeMatchMenuLoop, //WIFIP2PMATCH_MODE_MATCH_LOOP
  WifiP2PMatch_BCancelYesNo, //WIFIP2PMATCH_MODE_BCANCEL_YESNO
  WifiP2PMatch_BCancelWait,    //WIFIP2PMATCH_MODE_BCANCEL_WAIT
  _parentModeCallMenuInit,//WIFIP2PMATCH_MODE_CALL_INIT
  _parentModeCallMenuYesNo,//WIFIP2PMATCH_MODE_CALL_YESNO
  _parentModeCallMenuSend, //WIFIP2PMATCH_MODE_CALL_SEND
  _parentModeCallMenuCheck, //WIFIP2PMATCH_MODE_CALL_CHECK
  _parentModeCallMyStatusWait, //WIFIP2PMATCH_MODE_MYSTATUS_WAIT   //51
  _parentModeCallMenuWait,//WIFIP2PMATCH_MODE_CALL_WAIT
  _personalDataInit,    //WIFIP2PMATCH_MODE_PERSONAL_INIT
  _personalDataWait,    //WIFIP2PMATCH_MODE_PERSONAL_WAIT
  _personalDataEnd,    //WIFIP2PMATCH_MODE_PERSONAL_END
  _exitYesNo,       //WIFIP2PMATCH_MODE_EXIT_YESNO
  _exitWait,        //WIFIP2PMATCH_MODE_EXIT_WAIT
  _exitExiting,        //WIFIP2PMATCH_MODE_EXITING
  _exitEnd,        //WIFIP2PMATCH_MODE_EXIT_END
  _nextBattleYesNo,  //WIFIP2PMATCH_NEXTBATTLE_YESNO
  _nextBattleWait,  //WIFIP2PMATCH_NEXTBATTLE_WAIT
  _vchatNegoCheck, //WIFIP2PMATCH_MODE_VCHAT_NEGO
  _vchatNegoWait, //WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT
  WifiP2PMatch_ReConnectingWait, //WIFIP2PMATCH_RECONECTING_WAIT
  WifiP2PMatch_BCancelYesNoVCT, //WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT
  WifiP2PMatch_BCancelWaitVCT,    //WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT
  WifiP2PMatch_FirstSaving, //WIFIP2PMATCH_FIRST_SAVING
  WifiP2PMatch_CancelEnableWait, //WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT
  WifiP2PMatch_FirstSaving2, //WIFIP2PMATCH_FIRST_SAVING2
  _callGameInit,  //WIFIP2PMATCH_MODE_CALLGAME_INIT
  _playerDirectInit, //WIFIP2PMATCH_MODE_PLAYERDIRECT_INIT
  _playerDirectInit1, //  WIFIP2PMATCH_PLAYERDIRECT_INIT1,
  _playerDirectInit2, //  WIFIP2PMATCH_PLAYERDIRECT_INIT2,
  _playerDirectInit3, //  WIFIP2PMATCH_PLAYERDIRECT_INIT3,
  _playerDirectInit5, //  WIFIP2PMATCH_PLAYERDIRECT_INIT5,
  _playerDirectInit6, //  WIFIP2PMATCH_PLAYERDIRECT_INIT6,
  _playerDirectInit7, //  WIFIP2PMATCH_PLAYERDIRECT_INIT7,
  _playerDirectReturn, //WIFIP2PMATCH_PLAYERDIRECT_RETURN
  _playerDirectWait, //  WIFIP2PMATCH_PLAYERDIRECT_WAIT,
  _playerDirectWaitSt, //  WIFIP2PMATCH_PLAYERDIRECT_WAIT_ST,
  _playerDirectEnd, //  WIFIP2PMATCH_PLAYERDIRECT_END,    //80
  _playerDirectVCT, //  WIFIP2PMATCH_PLAYERDIRECT_VCT,
  _playerDirectTVT, //  WIFIP2PMATCH_PLAYERDIRECT_TVT,
  _playerDirectTrade, //  WIFIP2PMATCH_PLAYERDIRECT_TRADE,
  _playerDirectBattle1, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE1,
  _playerDirectSub1, // WIFIP2PMATCH_PLAYERDIRECT_SUB1
  _playerDirectSub2, // WIFIP2PMATCH_PLAYERDIRECT_SUB2
  _playerDirectSubStart, //WIFIP2PMATCH_PLAYERDIRECT_SUBSTART
  _playerDirectSubstartCall, //WIFIP2PMATCH_PLAYERDIRECT_SUBSTARTCALL
  _playerDirectEndCall, // WIFIP2PMATCH_PLAYERDIRECT_ENDCALL
  _playerDirectBattle2,//WIFIP2PMATCH_PLAYERDIRECT_BATTLE2
  _playerDirectBattleMode,  //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE,
  _playerDirectBattleMode2,  //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE2,
  _playerDirectBattleRule,  // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE,   //ルール
  _playerDirectBattleRule2,  // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE2,   //ルール
  _playerDirectBattleShooter,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER,  //シューター
  _playerDirectBattleShooter2,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER2,  //シューター
  _playerDirectBattleDecide,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE,  //決定
  _playerDirectBattleWatch,  // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH,  //ルールを見る
  _playerDirectBattleWatch2,  // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH2,  //ルールを見る
  _playerDirectBattleGO,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO
  _playerDirectBattleGO1,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO1
  _playerDirectBattleGO2,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO2
  _playerDirectBattleGO3,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO3
  _playerDirectBattleGO4,  //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO4
  _playerDirectNoregParent, //WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT  /レギュレーションが違う
  _playerDirectBattleStart, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START
  _playerDirectBattleStart2, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START2
  _playerDirectBattleStart3, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START3
  _playerDirectBattleStart4, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START4
  _playerDirectBattleStart5, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START5
  _playerDirectBattleStart6, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START6
  _playerDirectBattleStart7, //  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START7

  _playerDirectBattleNoregSelectTemoti, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_TEMOTI
  _playerDirectBattleNoregSelectBBox,   //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_BBOX
  _playerDirectBattleNoregSelectWait,   //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_WAIT

  _playerDirectFailed, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED
  _playerDirectFailed2, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED2
  _playerDirectFailed3, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED3
  MessageEndReturnList, //WIFIP2PMATCH_MESSAGEEND_RETURNLIST
  ReturnList,         //WIFIP2PMATCH_RETURNLIST
  _playerMachineInit1,//WIFIP2PMATCH_PLAYERMACHINE_INIT1
  _playerMachineNoregParent, //WIFIP2PMATCH_PLAYERMACHINE_NOREG_PARENT
  _playerMachineBattleDecide, //WIFIP2PMATCH_PLAYERMACHINE_BATTLE_DECIDE
  _childModeConnect, //WIFIP2PMATCH_MODE_CHILD_CONNECT
  _playerDirectNoregParent1, //WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT1
  _playerDirectNoregParent2, //WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT2
  _playerDirectInit1Next, //WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT1
  _playerDirectBattleGO_12, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_12
  _playerDirectBattleGO_13, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_13
  _playerDirectBattleGO_14, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_14
  _playerDirectVCTChange1,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE1
  _playerDirectVCTChange2,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE2
  _playerDirectVCTChange3,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE3
  _playerDirectVCTChange4,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE4
  _playerDirectVCTChange5,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE5
  _playerDirectVCTChange6,  //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE6
  _parentModeSelectMenuInit2, //WIFIP2PMATCH_MODE_SELECT_INIT2
  _playerDirectEnd2, //WIFIP2PMATCH_PLAYERDIRECT_END2,
  _playerDirectEndCall2, //WIFIP2PMATCH_PLAYERDIRECT_ENDCALL2,
  _playerDirectVCTChangeE, //WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE_E
  _parentModeCallMenuSendD, //WIFIP2PMATCH_MODE_CALL_CHECK_D
  _DirectConnectWait,//WIFIP2PMATCH_MODE_CONNECTWAIT,
  _DirectConnectWait2,//  WIFIP2PMATCH_MODE_CONNECTWAIT2,
  _playerDirectWaitSendCommand, // WIFIP2PMATCH_PLAYERDIRECT_WAIT_COMMAND
  _playerDirectWaitSendCommand2, // WIFIP2PMATCH_PLAYERDIRECT_WAIT_COMMAND2
  _playerDirectBattleGo3KeyWait, // WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO3_KEYWAIT
  _playerDirectEndKeyWait,  //WIFIP2PMATCH_PLAYERDIRECT_END_KEYWAIT
  _playerDirectEnd3, //WIFIP2PMATCH_PLAYERDIRECT_END3
  _playerDirectSub3, //WIFIP2PMATCH_PLAYERDIRECT_SUB3
  _playerDirectSubFailed, //WIFIP2PMATCH_PLAYERDIRECT_SUB_FAILED
  WifiP2PMatch_Disconnect2, //WIFIP2PMATCH_MODE_DISCONNECT2
  WifiP2PMatch_VCTDisconnectSend1,  //WIFIP2PMATCH_VCTEND_COMMSEND1
  WifiP2PMatch_VCTDisconnectSend2,  //WIFIP2PMATCH_VCTEND_COMMSEND2
  WifiP2PMatch_VCTDisconnectSend3, //WIFIP2PMATCH_VCTEND_COMMSEND3
  WifiP2PMatch_VCTConnectMain, //WIFIP2PMATCH_MODE_VCT_CONNECT_MAIN
  _playerDirectSub23, //  WIFIP2PMATCH_PLAYERDIRECT_SUB23
  _playerMachineTalkEnd, //  WIFIP2PMATCH_PLAYERMACHINE_TALKEND,
  _playerDirectBattleStart42, //WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START42
  _playerDirectEndNext,  ///WIFIP2PMATCH_PLAYERDIRECT_END_NEXT
  WifiP2PMatch_VCTDisconnectSendEnd,//WIFIP2PMATCH_VCTEND_COMMSEND2_ENDCHK
  _playerDirectCancelEnd, //WIFIP2PMATCH_PLAYERDIRECT_CANCELEND
  _playerDirectCancelEndNext, //WIFIP2PMATCH_PLAYERDIRECT_CANCELEND_NEXT
  _playerDirectSub45, //WIFIP2PMATCH_PLAYERDIRECT_SUB45
  WifiP2PMatch_FriendListMain_MW, //WIFIP2PMATCH_MODE_FRIENDLIST_MW
  WifiP2PMatch_ReconectingWaitPre, //WIFIP2PMATCH_RECONECTING_WAIT_PRE
  _modeTVT1YesNo,    //WIFIP2PMATCH_MODE_TVTMESSAGE1_YESNO
  _modeTVT1Wait,  //WIFIP2PMATCH_MODE_TVTMESSAGE1_WAIT
  _modeTVT2YesNo,    //  WIFIP2PMATCH_MODE_TVTMESSAGE2_YESNO
  _modeTVT2Wait,  // WIFIP2PMATCH_MODE_TVTMESSAGE2_WAIT
  _modeTVT3YesNo,    //  WIFIP2PMATCH_MODE_TVTMESSAGE3_YESNO
  _modeTVT3Wait,  // WIFIP2PMATCH_MODE_TVTMESSAGE3_WAIT
  _modeTVT4YesNo,    //  WIFIP2PMATCH_MODE_TVTMESSAGE4_YESNO
  _modeTVT4Wait,  // WIFIP2PMATCH_MODE_TVTMESSAGE4_WAIT
  _playerDirectEndChild, //WIFIP2PMATCH_PLAYERDIRECT_END_CHILD
  _playerDirectEndChildNext,//WIFIP2PMATCH_PLAYERDIRECT_END_CHILD_NEXT
  _playerDirectInit2Next,//WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT2
  _playerDirectInit3Next,//WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT3
  _playerDirectInit0Next, //WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT0
  _wifip2pmatch_mode_friendlist_mw_wait, //WIFIP2PMATCH_MODE_FRIENDLIST_MW_WAIT

};



#define _MAXNUM   (2)         // 最大接続人数
#define _MAXSIZE  (80)        // 最大送信バイト数
#define _BCON_GET_NUM (16)    // 最大ビーコン収集数


static GFLNetInitializeStruct aGFLNetInit = {
  NULL,  // 受信関数テーブル
  0, // 受信テーブル要素数
  NULL,    ///< ハードで接続した時に呼ばれる
  NULL,    ///< ネゴシエーション完了時にコール
  NULL,   // ユーザー同士が交換するデータのポインタ取得関数
  NULL,   // ユーザー同士が交換するデータのサイズ取得関数
  NULL,  // ビーコンデータ取得関数
  NULL,  // ビーコンデータサイズ取得関数
  NULL,  // ビーコンのサービスを比較して繋いで良いかどうか判断する
  NULL,            // 普通のエラーが起こった場合 通信終了
  NULL,  // 通信不能なエラーが起こった場合呼ばれる 切断するしかない
  NULL,  // 通信切断時に呼ばれる関数
  NULL,  // オート接続で親になった場合
  NULL,     ///< wifi接続時に自分のデータをセーブする必要がある場合に呼ばれる関数
  NULL, ///< wifi接続時にフレンドコードの入れ替えを行う必要がある場合呼ばれる関数
  _deleteFriendList,  ///< wifiフレンドリスト削除コールバック
  _getFriendData,   ///< DWC形式の友達リスト
  _getMyUserData,  ///< DWCのユーザデータ（自分のデータ）
  GFL_NET_DWC_HEAPSIZE,   ///< DWCへのHEAPサイズ
  TRUE,        ///< デバック用サーバにつなぐかどうか
  SYACHI_NETWORK_GGID,  //ggid
  GFL_HEAPID_APP,  //元になるheapid
  HEAPID_NETWORK,  //通信用にcreateされるHEAPID
  HEAPID_WIFI,  //wifi用にcreateされるHEAPID
  HEAPID_NETWORK,  //IRC用にcreateされるHEAPID
  GFL_WICON_POSX,GFL_WICON_POSY,        // 通信アイコンXY位置
  _MAXNUM,     // 最大接続人数
  _MAXSIZE,  //最大送信バイト数
  _BCON_GET_NUM,    // 最大ビーコン収集数
  TRUE,     // CRC計算
  FALSE,     // MP通信＝親子型通信モードかどうか
  GFL_NET_TYPE_WIFI,  //通信種別
  TRUE,     // 親が再度初期化した場合、つながらないようにする場合TRUE
  WB_NET_WIFICLUB,  //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // 赤外線タイムアウト時間
#endif
  0,//MP親最大サイズ 512まで
  0,//dummy
};

#if PM_DEBUG
static WIFIP2PMATCH_WORK* pDebugWork=NULL;
#endif


static void _GFL_NET_InitAndStruct(WIFIP2PMATCH_WORK *wk,BOOL bInit)
{

  if( OS_IsRunOnTwl() ){//DSIならVCT用のメモリが大きくとられる為、領域を多くしないといけない
    aGFLNetInit.heapSize = GFL_NET_DWCLOBBY_HEAPSIZE;
  }
  else{
    aGFLNetInit.heapSize = GFL_NET_DWC_HEAPSIZE;
  }
  if(bInit){
    GFL_NET_Init(&aGFLNetInit, NULL, wk);
  }
  else{
    GFL_NET_ChangeInitStruct(&aGFLNetInit);
  }
}

static const u8 ViewButtonFrame_y[ 4 ] = {
  8, 7, 5, 7
  };



// 自分ステータスを取得する
static u32 _WifiMyStatusGet( WIFIP2PMATCH_WORK * p_wk, WIFI_STATUS* p_status )
{
  return WIFI_STATUS_GetWifiStatus(p_status);
}
// 自分ゲームモードを取得する
static u32 _WifiMyGameModeGet( WIFIP2PMATCH_WORK * p_wk, WIFI_STATUS* p_status )
{
  return WIFI_STATUS_GetGameMode(p_status);
}

static BOOL _modeIsBattleStatus(int gamemode)
{
  switch(gamemode){
  case WIFI_GAME_BATTLE_SINGLE_ALL:   // シングルLv50対戦募集中
  case WIFI_GAME_BATTLE_SINGLE_FLAT:      // シングルバトル
  case WIFI_GAME_BATTLE_DOUBLE_ALL:      // ダブル
  case WIFI_GAME_BATTLE_DOUBLE_FLAT:      // ダブル
  case WIFI_GAME_BATTLE_TRIPLE_ALL:      // トリプル
  case WIFI_GAME_BATTLE_TRIPLE_FLAT:      // トリプル
  case WIFI_GAME_BATTLE_ROTATION_ALL:      // ローテーション
  case WIFI_GAME_BATTLE_ROTATION_FLAT:      // ローテーション
    return TRUE;
  }
  return FALSE;
}

//接続可能モードかどうか
static BOOL _modeIsConnectStatus(int gamemode)
{
  if((gamemode < WIFI_GAME_UNKNOWN)  && (gamemode > WIFI_GAME_UNIONMATCH)){
    return TRUE;
  }
  return FALSE;
}



// ステートがBATTLEWAITかどうか
static BOOL _modeBattleWait(int status,int gamemode)
{
  if(_modeIsBattleStatus(gamemode)){
    if(status == WIFI_STATUS_RECRUIT){
      return TRUE;
    }
  }
  return FALSE;
}

// ステートがWAITかどうか
static BOOL
_modeWait(int status)
{
  if(status==WIFI_STATUS_RECRUIT){
    return TRUE;
  }
  return FALSE;
}

// ステートがBATTLEかどうか
static BOOL _modeBattle(int status,int gamemode)
{
  if(_modeIsBattleStatus(gamemode)){
    if(status == WIFI_STATUS_PLAYING){
      return TRUE;
    }
  }
  return FALSE;
}

static BOOL _is2pokeMode(int status,int gamemode)
{
  if(gamemode > WIFI_GAME_LOGIN_WAIT){
    if(gamemode < WIFI_GAME_UNKNOWN){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンの数が交換に適しているかどうか
 * @param status
 * @return  none
 */
//--------------------------------------------------------------------------------------------

static BOOL _tradeNumCheck(WIFIP2PMATCH_WORK * wk)
{
  BOX_MANAGER * pManager = GAMEDATA_GetBoxManager(wk->pGameData);

  /*
  if(GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT){
    return FALSE;
  }
   */
  if(PokeParty_GetPokeCountNotEgg(GAMEDATA_GetMyPokemon(wk->pGameData)) < 2 ){
//  if((PokeParty_GetPokeCount(GAMEDATA_GetMyPokemon(wk->pGameData))==1) && ( BOXDAT_GetPokeExistCountTotal(pManager)==0 )){
    return FALSE;
  }
  return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * レギュレーションでバトルできるかどうか
 * @param status
 * @return  none
 */
//--------------------------------------------------------------------------------------------

static BOOL _regulationCheck(WIFIP2PMATCH_WORK * wk)
{
  u32 fail_bit;

  if(POKE_REG_OK!=_CheckRegulation_Temoti(wk->pRegulation, wk->pGameData, &fail_bit )){
    if(POKE_REG_OK!=_CheckRegulation_BBox(wk->pRegulation, wk->bb_party, &fail_bit )){
      // 選ぶ事ができない
      return FALSE;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * ステータスにあわせて通信ステートも変更する
 * @param status
 * @return  none
 */
//--------------------------------------------------------------------------------------------

static void _commStateChange(WIFIP2PMATCH_WORK * wk,int status,int gamemode)
{
  if(status == WIFI_STATUS_WAIT){
    GFL_NET_SetWifiBothNet(FALSE);
    _GFL_NET_InitAndStruct(wk,FALSE);
  }
  else if(_modeBattle(status, gamemode)){
    GFL_NET_SetWifiBothNet(TRUE); // wifiの通信を非同期から同期に
  }

  switch(status){
  case WIFI_STATUS_RECRUIT:    // 募集中  = 誰でも繋ぐ事が可能
  case WIFI_STATUS_PLAY_AND_RECRUIT:      // 他の人と接続中でさらに募集中
    GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),TRUE);  //接続許可
    break;
  case WIFI_STATUS_CALL:       // 他の人に接続しようとしている
  case WIFI_STATUS_WAIT:   // 待機中
  case WIFI_STATUS_NONE:   // 何も無い	NONEのときは出現もしません
  case WIFI_STATUS_PLAYING:      // 他の人と接続中
  case WIFI_STATUS_UNKNOWN:   // 新たに作ったらこの番号以上になる
  default:
    GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),TRUE);  //接続不可
    break;
  }
  OS_TPrintf("status %d gamemode %d\n",status,gamemode);


}

//============================================================================================
//  プロセス関数
//============================================================================================

static void _graphicInit(WIFIP2PMATCH_WORK * wk)
{
  ARCHANDLE* p_handle;

  // sys_VBlankFuncChange( NULL, NULL );  // VBlankセット
  //    sys_HBlankIntrStop(); //HBlank割り込み停止
  // Vblank期間中のBG転送終了
  if(wk->vblankFunc){
    GFL_TCB_DeleteTask( wk->vblankFunc );
  }

  GFL_DISP_GX_SetVisibleControlDirect(0);   //全BG&OBJの表示OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
#if WB_FIX
  sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );
#endif

  p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, HEAPID_WIFIP2PMATCH );


  // 文字列マネージャー生成
  wk->WordSet    = WORDSET_Create( HEAPID_WIFIP2PMATCH );
  wk->MsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_lobby_dat, HEAPID_WIFIP2PMATCH );
  wk->SysMsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_system_dat, HEAPID_WIFIP2PMATCH );
  wk->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_WIFIP2PMATCH );
  wk->font_handle_num = GFL_FONT_Create( ARCID_FONT, NARC_font_num_gftr,  GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIP2PMATCH );


  GFL_DISP_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

  // VRAM バンク設定
  VramBankSet();

  // BGLレジスタ設定
  BgInit(HEAPID_WIFIP2PMATCH );
  GFL_BMPWIN_Init( HEAPID_WIFIP2PMATCH );



  //BGグラフィックセット
  BgGraphicSet( wk, p_handle );

  // VBlank関数セット
  //    sys_VBlankFuncChange( VBlankFunc, wk );


  wk->vblankFunc = GFUser_VIntr_CreateTCB( VBlankFunc , wk , 1 );


  // OBJキャラ、パレットマネージャー初期化
  char_pltt_manager_init();

  // CellActorシステム初期化
  InitCellActor(wk, p_handle);

  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON ); //メイン画面OBJ面ＯＮ

  GFL_ARC_CloseDataHandle( p_handle );
}

//--------------------------------------------------------------------------------------------
/**
 * グラフィックにかかわる部分の終了処理
 * @param wk  WIFIP2PMATCH_WORK
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void _graphicEnd(WIFIP2PMATCH_WORK* wk)
{
  int i;

  if(wk->vblankFunc){
    GFL_TCB_DeleteTask( wk->vblankFunc );
  }
  _TrainerOAMFree( wk );
  _TouchResExit(wk);
  _UnderScreenReload( wk );


  // マッチングルーム破棄
  if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
    MCRSYS_AllDelMoveObjWork(wk);
  }
  // ビューアー破棄
  if( MCVSys_MoveCheck( wk ) == TRUE ){
    MCVSys_Exit( wk );
  }

  // アイコン破棄
  WifiP2PMatchFriendListIconRelease( &wk->icon );

  // セルアクターセット破棄
  GFL_CLACT_USERREND_Delete(wk->renddata);
  GFL_CLACT_UNIT_Delete(wk->clactSet);

  //OAMレンダラー破棄
  GFL_CLACT_SYS_Delete();//

  // BMPウィンドウ開放
  BmpWinDelete( wk );
  BgExit(wk);

  GFL_BMPWIN_Exit();
  // BGL削除
  GFL_BG_Exit( );


  // メッセージマネージャー・ワードセットマネージャー解放
  GFL_MSG_Delete( wk->SysMsgManager );
  GFL_MSG_Delete( wk->MsgManager );
  WORDSET_Delete( wk->WordSet );

  GFL_FONT_Delete(wk->font_handle_num);
  GFL_FONT_Delete(wk->fontHandle);
}


//--------------------------------------------------------------------------------------------
/**
 * VBlank関数
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB *tcb , void * work )
{
  WIFIP2PMATCH_WORK* wk = work;
  GFL_CLACT_SYS_VBlankFunc();

}

//--------------------------------------------------------------------------------------------
/**
 * VRAM設定
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VramBankSet(void)
{
  GFL_DISP_SetBank( &_defVBTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * BG設定
 *
 * @param ini   BGLデータ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgInit(HEAPID heapID )
{
  int i;

  GFL_BG_Init(heapID);
  // BG SYSTEM
  {
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  // 背景面
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000,
      0x10000,
      GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME0_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );


  }

  // メイン画面1
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000,
      0x8000,
      GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    //    GFL_BG_FillCharacter( GFL_BG_FRAME1_M, 0x00, 1, 0 );
    GFL_BG_ClearFrame( GFL_BG_FRAME1_M);
    GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );
  }

  // リストなど
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000,
      0x8000,
      GX_BG_EXTPLTT_23,
      2, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_FillCharacter( GFL_BG_FRAME2_M, 0x00, 1, 0 );
    GFL_BG_ClearFrame( GFL_BG_FRAME2_M);
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
  }

#if 1
  // いろいろに使う
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x18000,
      0x8000,
      GX_BG_EXTPLTT_23,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME3_M);
    GFL_BG_LoadScreenReq( GFL_BG_FRAME3_M );
  }
#endif

  // サブ画面テキスト面
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000,
      0x8000,
      GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME0_S );
  }
  // ユーザー状態表示面
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000,
      0x8000,
      GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME1_S );
  }
  // ボタンorユーザーデータ時の背景
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000,
      0x8000,
      GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME2_S );
  }
  // ユーザーデータテキスト
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000,
      0x8000,
      GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME3_S );
  }

  for(i=0;i <GFL_BG_FRAME3_S;i++){
    GFL_BG_ClearCharacter( i );
  }

  GFL_BG_SetPriority(GFL_BG_FRAME0_M , 3);  //はいけい
  GFL_BG_SetPriority(GFL_BG_FRAME1_M , 1);   // yesno win　ユーザーデータの背景
  GFL_BG_SetPriority(GFL_BG_FRAME3_M , 1);  // メッセージ
  GFL_BG_SetPriority(GFL_BG_FRAME2_M , 0);   // menuリスト

  G2_BlendNone();
  G2S_BlendNone();
}

static void WindowSet(void)
{
  GX_SetVisibleWnd( GX_WNDMASK_NONE );
  GXS_SetVisibleWnd( GX_WNDMASK_NONE );
  /*
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
  G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_NONE, 1 );
  G2_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, 1 );
  G2_SetWnd0Position( 0, 255, 0, 192 );

  GXS_SetVisibleWnd( GX_WNDMASK_W0 );
  G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_NONE, 1 );
  G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, 1 );
  G2S_SetWnd0Position( 0, 255, 0, 192 );
   */
}

//----------------------------------------------------------------------------
/**
 *  @brief  生成した動作オブジェクトワークを設定する
 *
 *  @param  wk    ワーク
 *  @param  p_obj オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static void MCRSYS_SetMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj )
{
  int i;

  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
    if( wk->p_matchMoveObj[i] == NULL ){
      wk->p_matchMoveObj[i] = p_obj;
      return ;
    }
  }
  GF_ASSERT( 0 );
}
//----------------------------------------------------------------------------
/**
 *  @brief  引数のOBJが入っているワークをNULLにする
 *
 *  @param  wk    ワーク
 *  @param  p_obj NULLを入れてほしいワークに入っているポインタ
 */
//-----------------------------------------------------------------------------
static void MCRSYS_DelMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj )
{
  int i;

  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
    if( wk->p_matchMoveObj[i] == p_obj ){
      wk->p_matchMoveObj[i] = NULL;
      return ;
    }
  }
  GF_ASSERT( 0 );
}


//----------------------------------------------------------------------------
/**
 *  @brief  部屋全部初期化＋ワーク消去
 *
 *  @param  wk    ワーク
 *  @param  p_obj NULLを入れてほしいワークに入っているポインタ
 */
//-----------------------------------------------------------------------------
static void MCRSYS_AllDelMoveObjWork( WIFIP2PMATCH_WORK* wk )
{
  int i;

  WIFI_MCR_Dest( &wk->matchroom );
  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ )
  {
    wk->p_matchMoveObj[i] = NULL;
    MCVSys_BttnDel( wk, i+1 );

  }
  MI_CpuClear8(wk->matchStatusBackup, sizeof(wk->matchStatusBackup));
}

//----------------------------------------------------------------------------
/**
 *  @brief  friendNOのオブジェクトワークを取得する
 *
 *  @param  wk      ワーク
 *  @param  friendNo  friendNO
 *
 *  @return オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static MCR_MOVEOBJ* MCRSYS_GetMoveObjWork( WIFIP2PMATCH_WORK* wk, u32 friendNo )
{
  int i;

  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
    if( wk->p_matchMoveObj[i] ){
      if( WIFI_MCR_GetFriendNo( wk->p_matchMoveObj[i] ) == friendNo ){
        return wk->p_matchMoveObj[i];
      }
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  アイコンデータ読込み
 *
 *  @param  p_data    データ格納先
 *  @param  p_handle  ハンドル
 *  @param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListIconLoad( WIFIP2PMATCH_ICON* p_data, ARCHANDLE* p_handle, u32 heapID )
{
  // パレット転送
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCLR,
                                    PALTYPE_MAIN_BG, PLAYER_DISP_ICON_PLTTOFS*32,
                                    PLAYER_DISP_ICON_PLTTNUM*32, heapID );

  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCLR,
                                    PALTYPE_SUB_BG, PLAYER_DISP_ICON_PLTTOFS_SUB*32,
                                    PLAYER_DISP_ICON_PLTTNUM*32, heapID );

  // キャラクタデータ転送 先頭からとらないといけない

  p_data->wf_match_all_iconcgx1m =
    GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle,
                                                 NARC_wifip2pmatch_wf_match_all_icon_NCGR,
                                                 GFL_BG_FRAME1_M, 0, 0, heapID);

  p_data->wf_match_all_iconcgx2s =
    GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle,
                                                 NARC_wifip2pmatch_wf_match_all_icon_NCGR,
                                                 GFL_BG_FRAME2_S, 0, 0, heapID);

  // さらにキャラクタデータを保存しておく
  if( p_data->p_charbuff == NULL ){
    p_data->p_charbuff = GFL_ARCHDL_UTIL_LoadOBJCharacter( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCGR,
                                                           FALSE, &p_data->p_char, heapID);
  }

  // スクリーンデータ読込み
  // リトライ時にスクリーンデータがある場合は読み込まない
  if( p_data->p_buff == NULL ){
    p_data->p_buff = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NSCR,
                                                 FALSE, &p_data->p_scrn, heapID);
  }

}

//----------------------------------------------------------------------------
/**
 *  @brief  Iconデータはき
 *
 *  @param  p_data  ワーク
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListIconRelease( WIFIP2PMATCH_ICON* p_data )
{

  if( p_data->p_buff != NULL ){
    GFL_HEAP_FreeMemory( p_data->p_buff );
    p_data->p_buff = NULL;
  }
  if( p_data->p_charbuff != NULL ){
    GFL_HEAP_FreeMemory( p_data->p_charbuff );
    p_data->p_charbuff = NULL;
  }

  //GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_M, PLAYER_DISP_ICON_CGX, p_data->wf_match_all_iconcgx1m);
  //GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S, PLAYER_DISP_ICON_CGX,  p_data->wf_match_all_iconcgx2s);

  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_M,
                           GFL_ARCUTIL_TRANSINFO_GetPos(p_data->wf_match_all_iconcgx1m),
                           GFL_ARCUTIL_TRANSINFO_GetSize(p_data->wf_match_all_iconcgx1m));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,
                           GFL_ARCUTIL_TRANSINFO_GetPos(p_data->wf_match_all_iconcgx2s),
                           GFL_ARCUTIL_TRANSINFO_GetSize(p_data->wf_match_all_iconcgx2s));



}

//----------------------------------------------------------------------------
/**
 *  @brief  アイコンを書き込む
 *
 *  @param  p_data    データ
 *  @param  frm     フレーム
 *  @param  cx      キャラクタｘ座標
 *  @param  cy      キャラクタｙ座標
 *  @param  icon_type アイコンタイプ
 *  @param  col     0=灰 1=赤
 */
//-----------------------------------------------------------------------------


static void WifiP2PMatchFriendListIconWrite(WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 icon_type, u32 col )
{
  int pal;
  // 書き込み
  int i,j;
  int datax,datay;
  int width;
  u16* p_scrndata = (u16*)p_data->p_scrn->rawData;

  pal = WifiP2PMatchBglFrmIconPalGet( frm );
  datax = PLAYER_DISP_ICON_SCRN_X*(icon_type % PLAYER_DISP_ICON_IDX_NUM);
  datay = 2 * (icon_type / PLAYER_DISP_ICON_IDX_NUM);
  width = p_data->p_scrn->screenWidth/8;

  for(i = 0;i < PLAYER_DISP_ICON_SCRN_Y;i++){
    for(j = 0;j < PLAYER_DISP_ICON_SCRN_X;j++){
      GFL_BG_ScrSetDirect( frm, j+cx,i+cy,  ((p_scrndata[ width*(datay+i) + datax+j ] + ( pal << 12 ))));
    }
  }
  // 転送
  GFL_BG_LoadScreenV_Req( frm );
}



static u8 _gamemode2icon(  u32 gamemode,u32 status )
{
  u8 scrn_idx = PLAYER_DISP_ICON_IDX_NONE;

  switch(gamemode){
  case WIFI_GAME_VCT:
    if(status == WIFI_STATUS_RECRUIT){
      scrn_idx = PLAYER_DISP_ICON_IDX_VCT;
    }
    else{
      scrn_idx = PLAYER_DISP_ICON_IDX_VCT_ACT;
    }
    break;
  case WIFI_GAME_TVT:      // TVT中
    if(status == WIFI_STATUS_RECRUIT){
      scrn_idx = PLAYER_DISP_ICON_IDX_TVT;
    }
    else{
      scrn_idx = PLAYER_DISP_ICON_IDX_TVT_ACT;
    }
    break;
  case WIFI_GAME_TRADE:
    if(status == WIFI_STATUS_RECRUIT){
      scrn_idx = PLAYER_DISP_ICON_IDX_CHANGE;
    }
    else{
      scrn_idx = PLAYER_DISP_ICON_IDX_CHANGE_ACT;
    }
    break;
  case WIFI_GAME_LOGIN_WAIT:
    scrn_idx = PLAYER_DISP_ICON_IDX_NORMAL;
    break;
  case WIFI_GAME_NONE:
    scrn_idx = PLAYER_DISP_ICON_IDX_NONE;
    break;
  case WIFI_GAME_UNIONMATCH:
    scrn_idx = PLAYER_DISP_ICON_IDX_NORMAL_ACT;
    break;
  default:
    if(_modeIsBattleStatus(gamemode)){
      if(status == WIFI_STATUS_RECRUIT){
        scrn_idx = PLAYER_DISP_ICON_IDX_FIGHT;
      }
      else{
        scrn_idx = PLAYER_DISP_ICON_IDX_FIGHT_ACT;
      }
    }
    else if(gamemode >= WIFI_GAME_UNKNOWN){    // 新たに作ったらこの番号以上になる
      scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
    }
    break;
  }
  return scrn_idx;
}


//----------------------------------------------------------------------------
/**
 *  @brief  Icon書き込み
 *
 *  @param  p_data    データ
 *  @param  frm     フレーム
 *  @param  cx      キャラクタｘ座標
 *  @param  cy      キャラクタｙ座標
 *  @param  status    状態
 *  @param  vctIcon   ボイスチャットONOFFフラグ
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListStIconWrite(  WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 status,u32 gamemode, u32 iconmode )
{
  u8 col=0;
  u8 scrn_idx=0;

  scrn_idx = _gamemode2icon(gamemode, status);
  //  if(status == WIFI_STATUS_RECRUIT){
  //   col = 1;
  //  }

  // 書き込み
  WifiP2PMatchFriendListIconWrite( p_data, frm, cx, cy, scrn_idx + iconmode, col );
}



//----------------------------------------------------------------------------
/**
 *  @brief  ビットマップにアイコンデータを書き込む
 *
 *  @param  p_bmp   ビットマップ
 *  @param  p_data    アイコンデータ
 *  @param  x     書き込みｘ座標
 *  @param  y     書き込みｙ座標
 *  @param  status    状態
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListBmpStIconWrite( GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 status,u32 gamemode,int pal )
{
  u8 col=0;
  u8 scrn_idx=0;

  //  if(status == WIFI_STATUS_RECRUIT){
  //  col = 1;
  //  }
  scrn_idx = _gamemode2icon(gamemode, status);

  WifiP2PMatchFriendListBmpIconWrite( p_bmp, p_data, x, y, scrn_idx, col,pal );
}

//----------------------------------------------------------------------------
/**
 *  @brief  frmのアイコンパレットナンバーを返す
 *
 *  @param  frm   BGL　FRAME
 *
 *  @return パレットNO
 */
//-----------------------------------------------------------------------------
static int WifiP2PMatchBglFrmIconPalGet( u32 frm )
{
  if( frm < GFL_BG_FRAME0_S ){
    return PLAYER_DISP_ICON_PLTTOFS;
  }
  return PLAYER_DISP_ICON_PLTTOFS_SUB;
}

//----------------------------------------------------------------------------
/**
 *  @brief  フレンドリスト開始宣言
 */
//-----------------------------------------------------------------------------
static int WifiP2PMatchFriendListStart( WIFIP2PMATCH_WORK *wk,BOOL bWipe )
{
  if(bWipe){
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );
  }

  // 部屋のBGM設定
  WIFI_STATUS_SetVChatStatus(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetUseVChat(wk->pMatch, wk->pParentWork->vchatMain);
  WifiP2P_SetLobbyBgm();

  return WIFIP2PMATCH_FRIENDLIST_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * BG解放
 *
 * @param ini   BGLデータ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( WIFIP2PMATCH_WORK *wk )
{
  int i;

  // for(i=0;i<GFL_BG_FRAME3_S;i++){
  //   GFL_BG_FreeBGControl( i );
  // }
  return;
  if(wk->talkwin_m2){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(wk->talkwin_m2),
                             GFL_ARCUTIL_TRANSINFO_GetSize(wk->talkwin_m2));
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2),
                             GFL_ARCUTIL_TRANSINFO_GetSize(wk->menuwin_m2));
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m1),
                             GFL_ARCUTIL_TRANSINFO_GetSize(wk->menuwin_m1));
    wk->talkwin_m2=0;
    wk->menuwin_m2=0;
    wk->menuwin_m1=0;
  }

}


//--------------------------------------------------------------------------------------------
/**
 * グラフィックデータセット
 *
 * @param wk    ポケモンリスト画面のワーク
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------


static void BgGraphicSet( WIFIP2PMATCH_WORK * wk, ARCHANDLE* p_handle )
{
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 |
                                 GX_PLANEMASK_BG3, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_ON );

  // アイコンデータ読込み
  WifiP2PMatchFriendListIconLoad(  &wk->icon, p_handle, HEAPID_WIFIP2PMATCH );


  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , MCV_SYSFONT_PAL*0x20, 16*2, HEAPID_WIFIP2PMATCH );

  {
    wk->talkwin_m2 = BmpWinFrame_GraphicSetAreaMan(
      GFL_BG_FRAME2_M,  COMM_MESFRAME_PAL,  MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH );
    wk->menuwin_m2 = BmpWinFrame_GraphicSetAreaMan(
      GFL_BG_FRAME2_M,  MENU_WIN_PAL, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH );
    wk->menuwin_m1 = BmpWinFrame_GraphicSetAreaMan(
      GFL_BG_FRAME1_M,  MENU_WIN_PAL, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH );
  }

}


//** CharManager PlttManager用 **//
#define WIFIP2PMATCH_CHAR_CONT_NUM        (20)
#define WIFIP2PMATCH_CHAR_VRAMTRANS_MAIN_SIZE   (128*1024)
#define WIFIP2PMATCH_CHAR_VRAMTRANS_SUB_SIZE    (16*1024)
#define WIFIP2PMATCH_PLTT_CONT_NUM        (20)

//-------------------------------------
//
//  キャラクタマネージャー
//  パレットマネージャーの初期化
//
//=====================================
static void char_pltt_manager_init(void)
{
#if 0
  // キャラクタマネージャー初期化
  {
    CHAR_MANAGER_MAKE cm = {
      WIFIP2PMATCH_CHAR_CONT_NUM,
      WIFIP2PMATCH_CHAR_VRAMTRANS_MAIN_SIZE,
      WIFIP2PMATCH_CHAR_VRAMTRANS_SUB_SIZE,
      HEAPID_WIFIP2PMATCH
      };
    InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_32K );
  }
  // パレットマネージャー初期化
  InitPlttManager(WIFIP2PMATCH_PLTT_CONT_NUM, HEAPID_WIFIP2PMATCH);

  // 読み込み開始位置を初期化
  CharLoadStartAll();
  PlttLoadStartAll();
  //通信アイコン用にキャラ＆パレット制限
  CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
  CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
#endif
}


#define _CLSYS_RESOUCE_MAX		(100)

//--------------------------------------------------------------
///	セルアクター　初期化データ
//--------------------------------------------------------------
static const GFL_CLSYS_INIT fldmapdata_CLSYS_Init =
{
  0, 0,
  0, 512,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL, //通信アイコン部分
  0,
  _CLSYS_RESOUCE_MAX,
  _CLSYS_RESOUCE_MAX,
  _CLSYS_RESOUCE_MAX,
  _CLSYS_RESOUCE_MAX,
  16, 16,
};

//------------------------------------------------------------------
/**
 * セルアクター初期化
 *
 * @param   wk    レーダー構造体のポインタ
 *
 * @retval  none
 */
//------------------------------------------------------------------

//-------------------------------------
///	独自レンダラー作成用
/// サーフェースデータ構造体
//=====================================
const static GFL_REND_SURFACE_INIT sini[] ={
  {
    0,0,			// サーフェース左上ｘ座標			// サーフェース左上ｙ座標
    256,				// サーフェース幅
    192,				// サーフェース高さ
    CLSYS_DRAW_MAIN,	// サーフェースタイプ(CLSYS_DRAW_TYPE)
    CLSYS_REND_CULLING_TYPE_NORMAL,       // カリングタイプ
  },
  {
    0,256,			// サーフェース左上ｘ座標			// サーフェース左上ｙ座標
    256,				// サーフェース幅
    192,				// サーフェース高さ
    CLSYS_DRAW_SUB,	// サーフェースタイプ(CLSYS_DRAW_TYPE)
    CLSYS_REND_CULLING_TYPE_NORMAL,       // カリングタイプ
  }};

static void InitCellActor(WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle)
{
  int i;

 // const u8 CELL_MAX = 17*4 + 6 + 9*4;
  const u8 CELL_MAX = 100;   //パソコン４　キャラ３２＊２＋２ 下画面2+ポケモン６＊２ = ８４

  GFL_CLACT_SYS_Create( &fldmapdata_CLSYS_Init , &_defVBTbl, HEAPID_WIFIP2PMATCH );
  wk->clactSet  = GFL_CLACT_UNIT_Create( CELL_MAX , 0, HEAPID_WIFIP2PMATCH );
  //GFL_CLACT_UNIT_SetDefaultRend( wk->clactSet );

  {
    wk->renddata =  GFL_CLACT_USERREND_Create( sini, NELEMS(sini), HEAPID_WIFIP2PMATCH );
    GFL_CLACT_UNIT_SetUserRend(wk->clactSet, wk->renddata);
  }

}

#define TRAINER_NAME_POS_X    ( 24 )
#define TRAINER_NAME_POS_Y    ( 32 )
#define TRAINER_NAME_POS_SPAN ( 32 )

#define TRAINER_NAME_WIN_X    (  3 )
#define TRAINER1_NAME_WIN_Y   (  6 )
#define TRAINER2_NAME_WIN_Y   (  7 )


// はい・いいえBMP（下画面）
#define YESNO_WIN_FRAME_CHAR  ( 1 + TALK_WIN_CGX_SIZ )
#define YESNO_CHARA_OFFSET    ( 1 + TALK_WIN_CGX_SIZ + MENU_WIN_CGX_SIZ + FLD_MSG_WIN_SX*FLD_MSG_WIN_SY )
#define YESNO_WINDOW_X      ( 22 )
#define YESNO_WINDOW_Y1     (  7 )
#define YESNO_WINDOW_Y2     ( 13 )
#define YESNO_CHARA_W     (  8 )
#define YESNO_CHARA_H     (  4 )

/* static const NET_BMPWIN_DAT TouchYesNoBmpDat[2]={
 *     {
 *         GFL_BG_FRAME0_M, YESNO_WINDOW_X, YESNO_WINDOW_Y1,
 *         YESNO_CHARA_W, YESNO_CHARA_H, 13, YESNO_CHARA_OFFSET
 *         },
 *     {
 *         GFL_BG_FRAME0_M, YESNO_WINDOW_X, YESNO_WINDOW_Y2,
 *         YESNO_CHARA_W, YESNO_CHARA_H, 13, YESNO_CHARA_OFFSET+YESNO_CHARA_W*YESNO_CHARA_H
 *
 *         }
 * };
 */



//------------------------------------------------------------------
/**
 * $brief   開始時のメッセージ   WIFIP2PMATCH_MODE_INIT
 * @param   wk
 * @param   seq
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_MainInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 status,gamemode;

  if(wk->pParentWork->seq != WIFI_GAME_NONE){
    _initBGMVol( wk, WIFI_STATUS_PLAYING);

    WifiList_SetLastPlayDate( wk->pList, GFL_NET_DWC_GetFriendIndex());  //もどってきたら日付更新
    if(((wk->state == WIFIP2PMATCH_STATE_TALK) || (wk->state == WIFIP2PMATCH_STATE_RECV))
       && (!GFL_NET_DWC_IsDisconnect())){  //話しかけ接続時
      //ここでVCT切断
      DWCRAP_StopVChat();
      _FriendFloorInit( wk );
      wk->friendNo = wk->pParentWork->friendNo;
      _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_UNIONMATCH);
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_SECOND_MATCH, WB_NET_WIFICLUB);
      _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT0);
    }
    else{  //相手と切断
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_VCTEND, WB_NET_WIFICLUB);
      _CHANGESTATE(wk, WIFIP2PMATCH_VCTEND_COMMSEND2);
    }
  }
  else
  {        // 接続がまだ
    _CHANGESTATE(wk, WIFIP2PMATCH_FRIENDLIST_INIT);
    _initBGMVol( wk, WIFI_STATUS_WAIT);
  }

  //BTS2447 対処 nagihashi
  //ライブキャスター等でここに戻ってくると再度PlayBGMをしてしまうので、
  //同じ曲の時は鳴らさないようにする。
  if( PMSND_GetNextBGMsoundNo() != SEQ_BGM_POKECEN )
  {
    PMSND_PlayBGM(SEQ_BGM_POKECEN);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   ログイン接続中  WIFIP2PMATCH_CONNECTING_INIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_ConnectingInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  GFL_BG_SetPriority(GFL_BG_FRAME0_M , 3);  //はいけい
  GFL_BG_SetPriority(GFL_BG_FRAME1_M , 1);   // yesno win
  GFL_BG_SetPriority(GFL_BG_FRAME3_M , 1);  // メッセージ
  GFL_BG_SetPriority(GFL_BG_FRAME2_M , 0);   //
  _CHANGESTATE(wk,WIFIP2PMATCH_CONNECTING);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   _sendMatchStatus ステータスを送る
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _sendMatchStatus(WIFIP2PMATCH_WORK *wk)
{
  GFL_NET_DWC_SetMyInfo((const char *)wk->pMatch, sizeof(WIFI_STATUS));
}

//------------------------------------------------------------------
/**
 * $brief   MyMatchStatus作成
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static void _makeMyMatchStatus(WIFIP2PMATCH_WORK* wk, u32 status, u32 gamemode)
{
  MYSTATUS* pMyStatus = GAMEDATA_GetMyStatus(wk->pGameData);

  WIFI_STATUS_SetPMVersion(wk->pMatch, PM_VERSION);
  WIFI_STATUS_SetPMLang(wk->pMatch, PM_LANG);
  WIFI_STATUS_SetSex(wk->pMatch, MyStatus_GetMySex(pMyStatus));
  WIFI_STATUS_SetTrainerView(wk->pMatch,MyStatus_GetTrainerView(pMyStatus));
  _myStatusChange_not_send(wk, status, gamemode); // BGM状態などを調整
  WIFI_STATUS_SetMyNation(wk->pMatch, MyStatus_GetMyNation(pMyStatus));
  WIFI_STATUS_SetMyArea(wk->pMatch, MyStatus_GetMyArea(pMyStatus));
  WIFI_STATUS_SetVChatStatus(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetUseVChat(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetPlayerID(wk->pMatch, MyStatus_GetID(pMyStatus));
  WIFI_STATUS_SetGameSyncID(wk->pMatch, MyStatus_GetProfileID(pMyStatus));
  
  _sendMatchStatus(wk);

}

//------------------------------------------------------------------
/**
 * $brief   フレンドのstatus読み込み
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _readFriendMatchStatus(WIFIP2PMATCH_WORK* wk)
{
  int i,num = 0;
  WIFI_STATUS* p_status;
  u32 status;

  //    MI_CpuFill8(wk->index2No, 0, sizeof(WIFIP2PMATCH_MEMBER_MAX));
  for(i = 0;i < WIFIP2PMATCH_MEMBER_MAX;i++){
    wk->index2No[i] = 0;

    // その人は自分の友達か？
    if( wk->friend_num > i ){
      // このチェックを少し進化させる
      // 最新情報を受信して初めて登録される
      if(DWC_STATUS_OFFLINE != WifiDwc_getFriendStatus(i)){
        p_status = WifiFriendMatchStatusGet( i );
        status = _WifiMyStatusGet( wk, p_status );

        if( (status != WIFI_STATUS_NONE) &&
            (status < WIFI_STATUS_UNKNOWN) ){
          //          GF_ASSERT(p_status->trainer_view != PLHERO);
          wk->index2No[num] = i+1;
          num++;
        }else{
          if( MCVSys_BttnTypeGet( wk, i+1 ) != MCV_BTTN_FRIEND_TYPE_NONE ){
            MCVSys_BttnDel( wk, i+1 );
          }
        }
      }
    }
  }
  return num;
}

//------------------------------------------------------------------
/**
 * $brief   フレンドのstatus検査　こちらの状態と変わったら変わった数を返す
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _checkUserDataMatchStatus(WIFIP2PMATCH_WORK* wk)
{
  int i,num = 0;
  BOOL back_up_wait;
  BOOL now_wait;
  MCR_MOVEOBJ* p_obj;
  WIFI_STATUS* p_status;
  u8 status;
  u8 gamemode;

  for(i = 0;i < WIFIP2PMATCH_MEMBER_MAX;i++){

    p_status = WifiFriendMatchStatusGet( i );
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = _WifiMyGameModeGet(wk, p_status);
    if(DWC_STATUS_OFFLINE == WifiDwc_getFriendStatus( i )){
      status = WIFI_GAME_NONE;
    }
    if((wk->matchStatusBackup[i]  != status) ||
       (wk->matchGamemodeBackup[i]  != gamemode) ||
       (wk->matchVchatBackup[i]  != WIFI_STATUS_GetVChatStatus(p_status)) ){

      // オブジェクトワーク
      p_obj = MCRSYS_GetMoveObjWork( wk, i+1 );

      // そのオブジェが登録され、新しい状態が描画オブジェに反映されるまでは
      // バックアップステータスを更新しない
      if( p_obj ){

        // ステータスが待ち状態になったらジャンプさせる
        // 待ち状態から普通に戻ったら通常に戻す
        back_up_wait = _modeWait( wk->matchStatusBackup[i] );
        now_wait = _modeWait( status );
        if( (back_up_wait == TRUE) && (now_wait == FALSE) ){
          //NPCの通常動作に戻す
          WIFI_MCR_NpcMoveSet( &wk->matchroom, p_obj,  MCR_NPC_MOVE_NORMAL );
        }else if( (back_up_wait == FALSE) && (now_wait == TRUE) ){

          //NPCをジャンプ動作にする
          WIFI_MCR_NpcMoveSet( &wk->matchroom, p_obj,  MCR_NPC_MOVE_JUMP );
        }
        {   //大事な情報セット
          WifiList_SetFriendInfo(wk->pList, i, WIFILIST_FRIEND_UNION_GRA, WIFI_STATUS_GetTrainerView(p_status));
          WifiList_SetFriendInfo(wk->pList, i, WIFILIST_FRIEND_SEX, WIFI_STATUS_GetSex(p_status));
          WifiList_SetFriendInfo(wk->pList, i, WIFILIST_GAMESYNC_ID, WIFI_STATUS_GetGameSyncID(p_status));
          WifiList_SetFriendInfo(wk->pList, i, WIFILIST_FRIEND_ID, WIFI_STATUS_GetPlayerID(p_status));
        }
      }
      wk->matchStatusBackup[i] = status;
      wk->matchGamemodeBackup[i] = gamemode;
      wk->matchVchatBackup[i] = WIFI_STATUS_GetVChatStatus(p_status);
      num++;
    }
  }
  return num;
}

//------------------------------------------------------------------
/**
 * $brief   エラー表示
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------


static void _errorDisp(WIFIP2PMATCH_WORK* wk)
{
#if 0
  GFL_NETSTATE_DWCERROR* pErr = GFL_NET_StateGetWifiError();
  int msgno,no = pErr->errorCode;
  int type =  GFL_NET_DWC_ErrorType(pErr->errorCode, pErr->errorType);

  if((type == 11) || (no == ERRORCODE_0)){
    msgno = dwc_error_0015;
    type = 11;
  }
  else if(no == ERRORCODE_HEAP){
    msgno = dwc_error_0014;
    type = 12;
  }
  else{
    if( type >= 0 ){
      if(DS_SYSTEM_IsRunOnTwl()){
        msgno = twlerror[type];
      }
      else{
        msgno = nitroerror[type];
      }
    }else{
      msgno = dwc_error_0012;
    }
  }
  OS_TPrintf("エラーメッセージ %d \n",msgno);
  EndMessageWindowOff(wk);
  WORDSET_RegisterNumber(wk->WordSet, 0, no,
                         5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);

  // エラーメッセージを表示しておくシンク数
  wk->timer = 30;

  // 上画面下の自分情報文字列をクリアする
  _systemMessagePrint(wk, msgno);

  switch(type){
  case 1:
  case 4:
  case 5:
  case 11:
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_INIT);
    break;
  case 6:
  case 7:
  case 8:
  case 9:
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_INIT);
    break;
  case 10:
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_INIT);
    break;
  case 0:
  case 2:
  case 3:
  default:
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CHECK_AND_END);
    break;
  }
#endif
}

//------------------------------------------------------------------
/**
 * $brief   接続をリトライする  WIFIP2PMATCH_RETRY_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _retryInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( wk->timer > 0 ){
    wk->timer --;
    return seq;
  }

  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_052, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_YESNO);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   接続をリトライする  WIFIP2PMATCH_RETRY_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _retryYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( WifiP2PMatchMessageEndCheck(wk) ){

    _yenowinCreateM2( wk );

    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   接続をリトライする  WIFIP2PMATCH_RETRY_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _retryWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret;

  ret = _bmpMenu_YesNoSelectMain(wk);

  if(ret == BMPMENU_NULL)
  {  // まだ選択中
    return seq;
  }
  else if(ret == 0)
  { // はいを選択した場合
    //GFL_NET_StateWifiLogout();
    GFL_NET_Exit(NULL);
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    _CHANGESTATE(wk,WIFIP2PMATCH_RETRY);
  }
  else
  {  // いいえを選択した場合
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT );  // VCHATの状態を元に戻すため
    //GFL_NET_StateWifiLogout();
    GFL_NET_Exit(NULL);
    //@@OO     CommInfoFinalize();   //Info初期化
    wk->endSeq = WIFI_GAME_NONE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
  }
  EndMessageWindowOff(wk);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   接続をリトライ  WIFIP2PMATCH_RETRAY
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _retry( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  ARCHANDLE* p_handle;

  if(!GFL_NET_IsResetEnable())
  {

    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE )
    {

      // ブラックアウト
      //      WIPE_SetBrightness( WIPE_DISP_MAIN, WIPE_FADE_BLACK );
      //      WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );
      //      GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,16,16,0);

      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
                      WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );

      MCRSYS_AllDelMoveObjWork(wk);

      // OAMバッファクリーン
      GFL_CLACT_SYS_ClearAllOamBuffer();
    }
    // ビューアー破棄
    if( MCVSys_MoveCheck( wk ) == TRUE ){
      MCVSys_Exit( wk );

    }

    // ビットマップOFF
    // ユーザーデータ
    wk->MyInfoWinBack = _BmpWinDel(wk->MyInfoWinBack);

    //接続開始
    GFL_BG_ClearFrame( GFL_BG_FRAME3_M);

    // グラフィックリセット
    p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, HEAPID_WIFIP2PMATCH );
    BgGraphicSet( wk, p_handle );
    GFL_ARC_CloseDataHandle( p_handle );



    // ブライトネス状態リセっト
    // MCR画面破棄時に暗くした物を解除
    wk->brightness_reset = TRUE;
    //    WIPE_ResetBrightness( WIPE_DISP_MAIN );
    //    WIPE_ResetBrightness( WIPE_DISP_SUB );

    if( wk->menulist){
      GFL_BMPWIN_Delete(wk->ListWin);
      BmpMenuList_Exit(wk->lw, NULL, NULL);
      wk->lw = NULL;
      BmpMenuWork_ListDelete( wk->menulist );
      wk->menulist=NULL;
    }

    //        wk->friendMatchReadCount;
    MI_CpuClear8(wk->index2No, sizeof(wk->index2No));
    MI_CpuClear8(wk->index2NoBackUp, sizeof(wk->index2NoBackUp));
    MI_CpuClear8(wk->matchStatusBackup, sizeof(wk->matchStatusBackup));
    MI_CpuClear8(wk->matchGamemodeBackup, sizeof(wk->matchGamemodeBackup));
    MI_CpuClear8(wk->matchVchatBackup, sizeof(wk->matchVchatBackup));

#if 0
    for(i=0;i < _OAM_NUM;i++){
      CLACT_SetDrawFlag( wk->MainActWork[i], 0 );
    }
#endif
    MainMenuMsgInit(wk);
    _GFL_NET_InitAndStruct(wk,FALSE);
    //    GFL_NET_Init(&aGFLNetInit, NULL, wk);
    GFL_NET_StateWifiEnterLogin();
    //        wk->pMatch = GFL_NET_StateWifiEnterLogin(wk->pSaveData,sizeof(TEST_MATCH_WORK));
    WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_CONNECTING_INIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ログイン接続中  WIFIP2PMATCH_CONNECTING
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_Connecting( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if( GFL_NET_DWC_GetSaving()) {
    _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_SAVING);
    WifiP2PMatchMessagePrint(wk, dwc_message_0015, TRUE);
  }

  if(GFL_NET_StateIsWifiLoginState()){
    //OS_TPrintf("WIFI接続完了\n");
    if( wk->bInitMessage ){  // 初回接続時にはセーブシーケンスへ
      //            SaveData_SaveParts(wk->pSaveData, SVBLK_ID_NORMAL);  //セーブ中
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG);
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
    }
  }
  else if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(wk);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   初回セーブ処理
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FirstSaving( WIFIP2PMATCH_WORK *wk, int seq )

{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if( GFL_NET_DWC_GetSaving()) {
    GAMEDATA_SaveAsyncStart(wk->pGameData);
    _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_SAVING2);
    return seq;
  }
  if(GFL_NET_StateIsWifiLoginState()){
    //OS_TPrintf("WIFI接続完了\n");
    if( wk->bInitMessage ){
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG);
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
    }
  }
  else if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(wk);
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   初回セーブ処理  WIFIP2PMATCH_FIRST_SAVING2
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FirstSaving2( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_NET_DWC_GetSaving()){
    SAVE_RESULT result = GAMEDATA_SaveAsyncMain(wk->pGameData);
    if (result != SAVE_RESULT_CONTINUE && result != SAVE_RESULT_LAST) {
      GFL_NET_DWC_ResetSaving();
    }
    else{
      return seq;
    }
  }
  if(GFL_NET_StateIsWifiLoginState()){
    //OS_TPrintf("WIFI接続完了\n");
    if( wk->bInitMessage ){
      _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG);
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
    }
  }
  else if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(wk);
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   切断＋VCT待ちに移行するまで待つ  WIFIP2PMATCH_RECONECTING_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_ReConnectingWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;

  if(wk->timer > 0){
    wk->timer--;
    return seq;
  }

  if(wk->rpm){
    PokeRegulation_DeletePrintMsg(wk->rpm);
    wk->rpm = NULL;
  }

  
  if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(wk);
  }
  else if(GFL_NET_StateIsWifiLoginMatchState()){
    //OS_TPrintf("WIFI接続完了\n");
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
      MCRSYS_AllDelMoveObjWork(wk);
    }
    _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
  }
  else if((GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_DISCONNECTING) || GFL_NET_StateIsWifiDisconnect() || !GFL_NET_IsConnectMember(GFL_NET_NETID_SERVER)){
    GFL_NET_StateWifiMatchEnd(TRUE);
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
      MCRSYS_AllDelMoveObjWork(wk);
    }
    _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
  }
  return seq;
}




//------------------------------------------------------------------
/**
 * $brief   最初接続完了  WIFIP2PMATCH_FIRST_ENDMSG
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _firstConnectEndMsg( WIFIP2PMATCH_WORK *wk, int seq )
{
  EndMessageWindowOff(wk);
  if(DS_SYSTEM_IsRunOnTwl()){
    _systemMessagePrint(wk, dwctwl_message_0004);
  }
  else{
    _systemMessagePrint(wk, dwc_message_0004);
  }
  _CHANGESTATE(wk,WIFIP2PMATCH_FIRST_ENDMSG_WAIT);
  //#if AFTER_MASTER_070410_WIFIAPP_N22_EUR_FIX
  wk->bInitMessage = FALSE;
  //#endif //AFTER_MASTER_070410_WIFIAPP_N22_EUR_FIX
  return seq;

}

//WIFIP2PMATCH_FIRST_ENDMSG_WAIT
static int _firstConnectEndMsgWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    if( 0==WifiList_GetFriendDataNum(wk->pList) ){  //フレンドがいない
      WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXITING);
      wk->timer = 1;
    }
    else{
      _makeMyMatchStatus(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
      _readFriendMatchStatus( wk );
      {
        _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
      }
      wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
    }
  }
  return seq;

}

//------------------------------------------------------------------
/**
 * $brief   フレンドリスト表示初期化
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------

static void _userDataDisp(WIFIP2PMATCH_WORK* wk)
{
  if(wk->MyInfoWin){
    GFL_BMPWIN_Delete(wk->MyInfoWin);
  }

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*MCV_SYSFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  wk->MyInfoWinBack = _BmpWinDel(wk->MyInfoWinBack);

  wk->MyInfoWin=GFL_BMPWIN_Create(
    GFL_BG_FRAME3_M,
    WIFIP2PMATCH_PLAYER_DISP_X, WIFIP2PMATCH_PLAYER_DISP_Y,
    WIFIP2PMATCH_PLAYER_DISP_SIZX, WIFIP2PMATCH_PLAYER_DISP_SIZY,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );


  wk->MyInfoWinBack=GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    WIFIP2PMATCH_PLAYER_DISP_X, WIFIP2PMATCH_PLAYER_DISP_Y,
    WIFIP2PMATCH_PLAYER_DISP_SIZX, WIFIP2PMATCH_PLAYER_DISP_SIZY,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );

  // 初期化
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MyInfoWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWin);
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MyInfoWinBack), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWinBack);


  // システムウィンドウ設定
  //  BmpMenuWinWrite( wk->MyInfoWinBack, WINDOW_TRANS_ON, MENU_WIN_CGX_NUM, MENU_WIN_PAL );
  //    GFL_BMPWIN_MakeFrameScreen(wk->MyInfoWinBack, MENU_WIN_CGX_NUM, MENU_WIN_PAL);
  //BmpWinFrame_CgxSet(GFL_BG_FRAME1_M,COMM_TALK_WIN_CGX_NUM, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);
  BmpWinFrame_Write( wk->MyInfoWinBack, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFRAME_PAL );
}

// ダミーオブジェクトを登録しまくる
static void DEBUG_DummyObjIn( WIFIP2PMATCH_WORK *wk, int num )
{
  int i;
  MCR_MOVEOBJ* p_moveobj;

  for( i=0; i<num; i++ ){
    // 自分を出す
    p_moveobj = WIFI_MCR_SetNpc( &wk->matchroom, i%16, i+1 );

    // みんな飛び跳ねる
    WIFI_MCR_NpcMoveSet( &wk->matchroom, p_moveobj, MCR_NPC_MOVE_JUMP );

    MCRSYS_SetMoveObjWork( wk, p_moveobj ); // 登録したワークを保存しておく

  }
}

//------------------------------------------------------------------
/**
 * $brief   マッチングしてよいかどうかの判定
 * @param   friendIndex判定する友人のindex
 * @retval  マッチングokならTRUE
 */
//------------------------------------------------------------------

static BOOL WIFIP2PModeCheck( int friendIndex ,void* pWork)
{
  u32 mySt,myGamemode;
  u32 targetSt,targetGamemode;
  WIFIP2PMATCH_WORK *wk = pWork;
  WIFI_STATUS* pMatch = wk->pMatch;
  WIFI_STATUS* p_status = WifiFriendMatchStatusGet( friendIndex);

  mySt = WIFI_STATUS_GetWifiStatus(pMatch);
  targetSt = WIFI_STATUS_GetWifiStatus(p_status);

  myGamemode = WIFI_STATUS_GetGameMode(pMatch);
  targetGamemode = WIFI_STATUS_GetGameMode(p_status);

  if(myGamemode == targetGamemode){
    return TRUE;
  }
  return FALSE;
}



//------------------------------------------------------------------
/**
 * $brief   フレンドリスト表示初期化   WIFIP2PMATCH_FRIENDLIST_INIT
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FriendListInit( WIFIP2PMATCH_WORK *wk, int seq )
{

  // エラーチェック
  if( GFL_NET_StateIsWifiError() == FALSE ){

    // 切断検査
    if(!GFL_NET_StateIsWifiLoginMatchState()){
      return seq;
    }
  }
  _CHANGESTATE(wk,WIFIP2PMATCH_FRIENDLIST_INIT2);
  return seq;
}





//------------------------------------------------------------------
/**
 * $brief   部屋の初期化
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void _FriendFloorInit( WIFIP2PMATCH_WORK *wk )
{
  int i,x;
  int num = 0;
  ARCHANDLE* p_handle;
  MCR_MOVEOBJ* p_moveobj;
  int obj_code;

  // コールバックを設定
  GFL_NET_DWC_SetConnectModeCheckCallback( WIFIP2PModeCheck );

  // ここから先は通信エラーをシステムウィンドウで出す

  p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, HEAPID_WIFIP2PMATCH );

  _myVChatStatusOrgSet(wk);

  _windelandSEcall(wk);

  MI_CpuClear8(wk->index2NoBackUp, sizeof(wk->index2NoBackUp));


  if(wk->menulist){
    GFL_BMPWIN_Delete(wk->ListWin);
    BmpMenuList_Exit(wk->lw, NULL, NULL);
    wk->lw = NULL;
    BmpMenuWork_ListDelete( wk->menulist );
    wk->menulist = NULL;
  }
  if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){

    if( MyStatus_GetMySex( GAMEDATA_GetMyStatus(wk->pGameData) ) == PM_MALE ){
      obj_code = WB_HERO;
    }else{
      obj_code = WB_HEROINE;
    }

    // マッチングルーム初期化
    wk->friend_num = WifiList_GetFriendDataLastIdx( wk->pList );
    WIFI_MCR_Init( &wk->matchroom, HEAPID_WIFIP2PMATCH, p_handle,
                   obj_code, wk->friend_num, ARCID_WIFIP2PMATCH,wk->clactSet, wk->renddata );

    p_moveobj = WIFI_MCR_SetPlayer( &wk->matchroom, obj_code );

    MCRSYS_SetMoveObjWork( wk, p_moveobj ); // 登録したワークを保存しておく

  }
  // ビューアー初期化
  if( MCVSys_MoveCheck( wk ) == FALSE ){
    MCVSys_Init( wk, p_handle, HEAPID_WIFIP2PMATCH );
  }

  //MACアドレスをリセットしておく
  WIFI_STATUS_ResetVChatMac(wk->pMatch);
  wk->DirectMacSet = 0;

  // 今の状態を書き込む
  MCVSys_ReWrite( wk, HEAPID_WIFIP2PMATCH );


  // ユーザーデータ表示
  EndMessageWindowOff(wk);
  _readFriendMatchStatus(wk);

  // ユーザーデータのBMP作成
  _userDataDisp(wk);

  _GFL_NET_InitAndStruct(wk,FALSE);  //メモリ初期化

  wk->preConnect = -1;

  // 080605 tomoya BTS:249の対処のため
  // 親の通信リセット処理後直ぐに接続してしまうのを回避
  // （接続されてもVCHATの状態のまま、FriendListシーケンスに移行させる）
  //  mydwc_ResetNewPlayer(); // ちなみにmydwc_returnLobby内でも初期化してる
#if 0
  CLACT_SetDrawFlag( wk->MainActWork[_CLACT_LINE_CUR], 1 );
#endif

  GFL_ARC_CloseDataHandle( p_handle );

  // ワイプイン
  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );

}



//------------------------------------------------------------------
/**
 * $brief   フレンドリスト表示初期化   WIFIP2PMATCH_FRIENDLIST_INIT2
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FriendListInit2( WIFIP2PMATCH_WORK *wk, int seq )
{

  _FriendFloorInit(wk);
  _myStatusChange(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
  _commStateChange(wk,WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);

  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  FriendRequestWaitOff(wk);      // 主人公の動作を許可 状態をNONEにもどす
  GFL_NET_SetAutoErrorCheck(FALSE);
  GFL_NET_SetNoChildErrorCheck(FALSE);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   誰かが接続してきた場合
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _checkParentConnect(WIFIP2PMATCH_WORK *wk)
{
  if( GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED ){
    return TRUE;
  }
  return FALSE;
}

static BOOL _checkParentNewPlayer( WIFIP2PMATCH_WORK *wk )
{
  if( (GFL_NET_DWC_IsNewPlayer() != -1) ){
    return TRUE;
  }
  return FALSE;
}

#ifdef BUGFIX_BTS7868_20100716
//------------------------------------------------------------------
/**
 * $brief   誰かが接続してきた場合
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _checkParentErrorConnect(WIFIP2PMATCH_WORK *wk)
{
  switch(GFL_NET_StateGetWifiStatus()){
  case GFL_NET_STATE_TIMEOUT:
  case GFL_NET_STATE_DISCONNECTING:
  case GFL_NET_STATE_FAIL:
    return TRUE;
  default:
    return FALSE;
  }
  return FALSE;
}
#endif

//----------------------------------------------------------------------------
/**
 *  @brief  友達の出入りを管理する
 *
 *  @param  wk  ワーク
 *
 *  @retval 今の友達の数
 */
//-----------------------------------------------------------------------------
static int MCRSYS_ContFiendInOut( WIFIP2PMATCH_WORK* wk )
{
  int friend_num;
  int i, j;
  MCR_MOVEOBJ* p_obj;
  BOOL match;
  WIFI_STATUS* p_status;
  BOOL in_flag;
  BOOL out_flag;

  // 最新版の友達数と、友達ナンバー配列を作成
  friend_num = _readFriendMatchStatus( wk );
  //  wk->friendMatchReadCount = friend_num;


  in_flag = FALSE;
  out_flag = FALSE;

  // バックアップと比較する
  if( GFL_STD_MemComp( wk->index2No, wk->index2NoBackUp, sizeof(wk->index2NoBackUp) ) != 0 ){

    // 人が変わった
    // バックアップ側にいて、最新版にいないやつはOUT
    // 最新版にいて、バックアップ版にいないやつはIN
    for( i=0; i<WIFIP2PMATCH_MEMBER_MAX; i++ ){

      // OUTチェック
      match = FALSE;
      for( j=0; j<WIFIP2PMATCH_MEMBER_MAX; j++ ){
        if( wk->index2No[j] == wk->index2NoBackUp[i] ){
          match = TRUE;
          break;
        }
      }
      // いなかったので削除
      // 一気に32人入ってきたとき0がいないという風になってしまうので
      // ０じゃないかもチェック
      if( (match == FALSE) && (wk->index2NoBackUp[i] != 0) ){
        p_obj = MCRSYS_GetMoveObjWork( wk, wk->index2NoBackUp[i] );
        if( p_obj != NULL ){
          WIFI_MCR_DelPeopleReq( &wk->matchroom, p_obj );
          MCRSYS_DelMoveObjWork( wk, p_obj );
          MCVSys_BttnDel( wk, wk->index2NoBackUp[i] );
          WifiP2PMatch_UserDispOff_Target( wk, wk->index2NoBackUp[i], HEAPID_WIFIP2PMATCH );  // その人がした画面に詳細表示されているなら消す
          out_flag = TRUE;
        }
        else{
          MCVSys_BttnDel( wk, wk->index2NoBackUp[i] );
        }
      }

      // INチェック
      match = FALSE;
      for( j=0; j<WIFIP2PMATCH_MEMBER_MAX; j++ ){
        if( wk->index2No[i] == wk->index2NoBackUp[j] ){
          match = TRUE;
          break;
        }
      }
      // 前はいなかったので追加
      if( (match == FALSE) && (wk->index2No[i] != 0) ){
        p_status = WifiFriendMatchStatusGet( wk->index2No[i]-1 );

        p_obj = WIFI_MCR_SetNpc( &wk->matchroom,
                                 WIFI_STATUS_GetTrainerView(p_status), wk->index2No[i] );


        // 登録できたかチェック
        if( p_obj ){

          MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_IN );

          MCRSYS_SetMoveObjWork( wk, p_obj );

          in_flag = TRUE;
        }else{

          // オブジェが登録できなかったので予約登録
          // （これだとボタンを押すことが出来ない）
          MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_RES );
   //       wk->index2No[i] = 0;  // 入ってきたのはなかったことにする

        }
      }
    }

    // 最新版に変更
    // memcpy( wk->index2NoBackUp, wk->index2No, sizeof(u8)*WIFIP2PMATCH_MEMBER_MAX );
    GFL_STD_MemCopy(  wk->index2No, wk->index2NoBackUp,sizeof(wk->index2NoBackUp) );


    if( in_flag || out_flag){
      // ボタン全描画リクエスト
      MCVSys_BttnAllWriteReq( wk );
    }
    //出入りSE
    if( in_flag == TRUE ){
      PMSND_PlaySE( SEQ_SE_FLD_05 );
    }else if( out_flag == TRUE ){
      PMSND_PlaySE( SEQ_SE_FLD_05 );
    }

  }


  return friend_num;
}




//----------------------------------------------------------------------------
/**
 *  @brief  友達の出入りを管理する   OBJ登録していないお友達検査
 *
 *  @param  wk  ワーク
 *
 *  @retval 今の友達の数
 */
//-----------------------------------------------------------------------------
static void MCRSYS_ContFiendInOut2( WIFIP2PMATCH_WORK* wk )
{
  int friend_num;
  int i, j;
  MCR_MOVEOBJ* p_obj;
  BOOL match;
  WIFI_STATUS* p_status;
  BOOL in_flag;

  in_flag = FALSE;
  for( i=0; i<WIFIP2PMATCH_MEMBER_MAX; i++ ){
    if(wk->index2No[i] != 0){ 
      p_obj = MCRSYS_GetMoveObjWork( wk, wk->index2No[i] );
      if(p_obj){
        continue;
      }
      p_status = WifiFriendMatchStatusGet( wk->index2No[i]-1 );
      
      p_obj = WIFI_MCR_SetNpc( &wk->matchroom,
                                 WIFI_STATUS_GetTrainerView(p_status), wk->index2No[i] );
      // 登録できたかチェック
      if( p_obj ){
        MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_IN );
        MCRSYS_SetMoveObjWork( wk, p_obj );

        wk->matchStatusBackup[i] = 0;
        wk->matchGamemodeBackup[i] = 0;
        wk->matchVchatBackup[i] = 0;

        in_flag = TRUE;
      }
    }
  }
  //出入りSE
  if( in_flag == TRUE ){
    PMSND_PlaySE( SEQ_SE_FLD_05 );
  }
}




//----------------------------------------------------------------------------
/**
 *  @brief  友達の状態変化を表示
 *
 *  @param  wk  ワーク
 */
//-----------------------------------------------------------------------------
static void MCRSYS_ContFriendStatus( WIFIP2PMATCH_WORK* wk, u32 heapID )
{
  int i;
  u32 friendID;
  u32 type;
  BOOL vchat;
  int change_num;

  // 状態変更数を取得（今までと動きを一緒にするためにこうしておく）
  change_num = _checkUserDataMatchStatus( wk );

  // 皆の状態変化を登録する
  if( change_num > 0 ){
    if( wk->view.user_disp != MCV_USERDISP_OFF ){
      MCVSys_UserDispDraw_Renew( wk, heapID );
    }else{
      wk->view.bttn_allchg = TRUE;
      MCVSys_BttnDraw( wk );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達オブジェクトの登録破棄管理
 *
 *  @param  wk  ワーク
 */
//-----------------------------------------------------------------------------
static void MCRSYS_SetFriendObj( WIFIP2PMATCH_WORK* wk, u32 heapID )
{
  // 友達ナンバーが変わっていたらいなくなった友達を消し、増えた友達を登録
  MCRSYS_ContFiendInOut( wk );

  // 状態が変わったら何かを変える
  MCRSYS_ContFriendStatus( wk, heapID );

  //邪魔されたお友達検査
  MCRSYS_ContFiendInOut2( wk );
}

//------------------------------------------------------------------
/**
 * $brief   影の表示を元に戻す
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void MCRSYS_ReloadShadow( WIFIP2PMATCH_WORK* wk )
{
  int i;

  for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
    if( wk->p_matchMoveObj[i] ){
      WIFI_MCR_ShadowOn(&wk->matchroom, wk->p_matchMoveObj[i]);  //影を出す
    }
  }
}

//------------------------------------------------------------------
/**
 * $brief   フレンドリスト表示中処理 WIFIP2PMATCH_MODE_FRIENDLIST
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_FriendListMain( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret = MCR_RET_NONE;
  int checkMatch;
  u32 check_friend;
  MCR_MOVEOBJ* p_obj;
  u32 status,gamemode;

  wk->vchatrev = 0;  //マシンでの暫定VCTフラグリセット

  // ワイプ終了待ち
  if( !WIPE_SYS_EndCheck() ){
    return seq;
  }

  // エラーチェック
  if( GFL_NET_StateIsWifiError()){
    //        wk->localTime=0;
    _errorDisp(wk);
    return seq;
  }

#ifdef BUGFIX_BTS7868_20100716
  if(wk->preConnect != -1){  //誰かとつながった状態でタイムアウトスタート
    wk->localTime++;
    if(wk->localTime >  _TIMEOUT_MW){ //一分
      GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_DISCONNECT );
      return seq;
    }
  }
#endif
  
  // ボイスチャットONOFF繰り返しでここの画面ではまる為
  // えら状態になったらDISCONNECT処理に遷移
  if( GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_DISCONNECTING ){
    // この処理は、_parentModeCallMenuInitからもってきました
    // 適合しない=>切断処理へ
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    _myStatusChange(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
    GFL_NET_StateWifiMatchEnd(TRUE);
    return seq;
  }

  // 友達が現れたら出す
  MCRSYS_SetFriendObj( wk, HEAPID_WIFIP2PMATCH );
  MCVSys_UpdataBttn( wk );


  // パソコンアニメが動いていたら終了
  WIFI_MCR_PCAnmOff( &wk->matchroom );

  // ボイスチャットのONOFFはプレイヤーが動けるときにだけ設定できる
  if( WIFI_MCR_PlayerMovePauseGet( &wk->matchroom ) == FALSE ){
    // ボイスチャット
    if(GFL_NET_DWC_IsNewPlayer() == -1){
      if(PAD_BUTTON_X & GFL_UI_KEY_GetTrg()){
        if(_myVChatStatusToggleOrg(wk)){
          WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_054, FALSE);
        }
        else{
          WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_055, FALSE);
        }
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        _userDataInfoDisp(wk);
        _CHANGESTATE(wk,WIFIP2PMATCH_VCHATWIN_WAIT);
        WifiP2PMatch_UserDispOff_Any(wk, HEAPID_WIFIP2PMATCH); // した画面初期化
        return seq;
      }
#if PM_DEBUG
      if(PAD_BUTTON_Y & GFL_UI_KEY_GetTrg()){
        if(WIFI_STATUS_GetGameMode(wk->pMatch) != WIFI_GAME_UNKNOWN){
          _myStatusChange(wk, WIFI_STATUS_RECRUIT,WIFI_GAME_UNKNOWN);
          WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_074, FALSE);
        }
        else{
          _myStatusChange(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
          WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_075, FALSE);
        }
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        _userDataInfoDisp(wk);
        _CHANGESTATE(wk,WIFIP2PMATCH_VCHATWIN_WAIT);
        WifiP2PMatch_UserDispOff_Any(wk, HEAPID_WIFIP2PMATCH); // した画面初期化
        return seq;
      }
#endif
    }

    // した画面も動かない
    check_friend = MCVSys_Updata( wk, HEAPID_WIFIP2PMATCH );  // した画面も動かす
    p_obj = MCRSYS_GetMoveObjWork( wk, check_friend );
    if( check_friend != 0 ){
      if( p_obj ){
        WIFI_MCR_CursorOn( &wk->matchroom, p_obj );
      }else{
        MCRSYS_ReloadShadow(wk);
        WIFI_MCR_CursorOff( &wk->matchroom );
      }
    }else{
      MCRSYS_ReloadShadow(wk);
      WIFI_MCR_CursorOff( &wk->matchroom );
    }

  }


  //  if( wk->DirectModeNo!=0){
  //    if(!WIFI_STATUS_IsVChatMac(wk->pMatch, WifiFriendMatchStatusGet( wk->DirectModeNo-1 ))){
  //      wk->DirectModeNo=0;
  //}
  //  }
  {
    int j;
    for(j = 0; j < WIFIP2PMATCH_MEMBER_MAX ; j++){
      int n = GFUser_GetPublicRand0(WIFIP2PMATCH_MEMBER_MAX);  //いつも若い順になら無いように乱数検査
      if(WIFI_STATUS_IsVChatMac(wk->pMatch, WifiFriendMatchStatusGet( n ))){
        u8 callcount = WIFI_STATUS_GetCallCounter(WifiFriendMatchStatusGet( n ));
        OS_TPrintf("呼び出し %d %d\n",callcount,wk->pParentWork->matchno[n] );
        if(callcount != wk->pParentWork->matchno[n] ){
          //自分が呼び出されているので、接続開始 状態を取得
          status = _WifiMyStatusGet( wk, WifiFriendMatchStatusGet( n ) );
          gamemode = _WifiMyGameModeGet( wk, WifiFriendMatchStatusGet( n ) );
          wk->friendNo = n + 1;
          //OS_TPrintf("ダイレクト番号セット%d\n", wk->friendNo);
          PMSND_PlaySE_byPlayerID( SE_CALL_SIGN,SE_CALL_SIGN_PLAYER );
          _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALLGAME_INIT);
          return seq;
        }
      }
    }
  }


  // 友達からこちらに接続してきたときの処理
  checkMatch = _checkParentConnect(wk);
  if( (0 !=  checkMatch) && (wk->preConnect != -1) ){ // 接続してきた
    OS_TPrintf("接続 %d\n",wk->DirectModeNo);
    PMSND_PlaySE_byPlayerID( SE_CALL_SIGN,SE_CALL_SIGN_PLAYER );
    GFL_NET_SetAutoErrorCheck(TRUE);
    GFL_NET_SetNoChildErrorCheck(TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_INIT);
    return seq;
  }
  if((wk->preConnect == -1) && (GFL_NET_DWC_IsNewPlayer() != -1)){  // 通常のコネクト開始
    wk->preConnect = GFL_NET_DWC_IsNewPlayer();
    _friendNameExpand(wk, wk->preConnect);
    if(wk->DirectMacSet==0){
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_043, FALSE);
    }

    // つながった人のデータ表示
    //    WifiP2PMatch_UserDispOn( wk, wk->preConnect+1, HEAPID_WIFIP2PMATCH );

    wk->localTime = 0;
  }
  else if((wk->preConnect != -1) && (GFL_NET_DWC_IsNewPlayer() == -1)){ // 自分的には新しくコネクトしてきたのに、実際はコネクトしてきていなかったとき？

    // ボイスチャット設定
    GFL_NET_DWC_SetVChat(FALSE);// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
    WIFI_STATUS_SetUseVChat(wk->pMatch, WIFI_STATUS_GetVChatStatus(wk->pMatch));
    _friendNameExpand(wk, wk->preConnect);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    GFL_NET_StateWifiMatchEnd(TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }
#ifdef BUGFIX_BTS7868_20100716
  checkMatch = _checkParentErrorConnect(wk);
  if( (0 !=  checkMatch) && (wk->preConnect != -1) ){ // 接続してるのにおかしい
    _friendNameExpand(wk, GFL_NET_DWC_GetDisconnectNewPlayer());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }
#endif

  // VCHAT　ON　状態なのに、新しいコネクトが-1ならVCHATをOFFにする
  // 080615 tomoya
  if( (GFL_NET_DWC_IsVChat() == TRUE) && (GFL_NET_DWC_IsNewPlayer() == -1) ){
    GFL_NET_DWC_SetVChat(FALSE);// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
    WIFI_STATUS_SetUseVChat(wk->pMatch, WIFI_STATUS_GetVChatStatus(wk->pMatch));
  }


  // 状態を取得
  status = _WifiMyStatusGet( wk, wk->pMatch );

  // 誰も自分に接続してきていないときだけリストを動かせる
  if(wk->preConnect == -1){

    // CANCELボタンでも待機状態をクリア
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
      if(_modeWait(status)){  // 待ち状態のとき
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
        WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // した画面初期化
        return seq;
      }
    }
    ret = WIFI_MCR_Main( &wk->matchroom );
    WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン
  }
  switch(ret){
  case MCR_RET_NONE:
    return seq;
  case MCR_RET_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    if(_modeWait(status)){  // 待ち状態のとき
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
    }
    else{ // それか終了チェックへ
      wk->endSeq = WIFI_GAME_NONE;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_YESNO);
      WifiP2PMatchMessagePrint(wk, dwc_message_0010, TRUE);
      //        wk->localTime=0;
      ret = BMPMENULIST_CANCEL;
    }
    WifiP2PMatch_UserDispOff_Any(wk, HEAPID_WIFIP2PMATCH); // した画面初期化
    return seq;

  case MCR_RET_MYSELECT:   //パソコンに話しかける
    {  // 募集の行で選択したとき
      if(status == WIFI_STATUS_WAIT){
        wk->state = WIFIP2PMATCH_STATE_MACHINE;  //NOTダイレクト
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        WIFI_MCR_PCAnmStart( &wk->matchroom );  // pcアニメ開始
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT);
        WifiP2PMatch_UserDispOff_Any(wk, HEAPID_WIFIP2PMATCH); // した画面初期化
        wk->DirectMacSet=0;
        return seq;
      }
    }
    break;
  case MCR_RET_SELECT:   //相手に話しかける
    if(WIFI_STATUS_WAIT==status){  // 人の名前ー＞マッチングへ
      //相手の状態を確保 以後この状態をみるように変更 相手の動作をリアルタイムに追わない
      int friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
      PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
      GFL_STD_MemCopy(WifiFriendMatchStatusGet( friendNo - 1 ), &wk->targetStatus, sizeof(WIFI_STATUS));
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT);
      WifiP2PMatch_UserDispOff_Any(wk, HEAPID_WIFIP2PMATCH); // した画面初期化
      return seq;
    }
    break;
  default:
    GF_ASSERT(0);
    break;
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   フレンドリスト表示中処理 WIFIP2PMATCH_MODE_FRIENDLIST_MW
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_FriendListMain_MW( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret = MCR_RET_NONE;
  int checkMatch;
  u32 check_friend;
  MCR_MOVEOBJ* p_obj;
  u32 status,gamemode;


#ifdef BUGFIX_BTS7868_20100716
  if(wk->preConnect != -1){  //誰かとつながった状態でタイムアウトスタート
    wk->localTime++;
    if(wk->localTime >  _TIMEOUT_MW){ //一分
      GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_DISCONNECT );
      return seq;
    }
  }
#endif
  
  //BTS3567、BTS3795-bの対処 Saito
  //メッセージがある場合、メッセージを早送りするために読んでいます
  WifiP2PMatchMessageEndCheck(wk);

  wk->vchatrev = 0;  //マシンでの暫定VCTフラグリセット

  // 友達が現れたら出す
  MCRSYS_SetFriendObj( wk, HEAPID_WIFIP2PMATCH );
  MCVSys_UpdataBttn( wk );

  // パソコンアニメが動いていたら終了
  WIFI_MCR_PCAnmOff( &wk->matchroom );

  // 友達からこちらに接続してきたときの処理
  checkMatch = _checkParentConnect(wk);
  if( (0 !=  checkMatch) && (wk->preConnect != -1) ){ // 接続してきた
    OS_TPrintf("接続 %d\n",wk->DirectModeNo);
    PMSND_PlaySE_byPlayerID( SE_CALL_SIGN,SE_CALL_SIGN_PLAYER );
    GFL_NET_SetAutoErrorCheck(TRUE);
    GFL_NET_SetNoChildErrorCheck(TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_INIT);
    return seq;
  }
#ifdef BUGFIX_BTS7868_20100716
  checkMatch = _checkParentErrorConnect(wk);
  if( (0 !=  checkMatch) && (wk->preConnect != -1) ){ // 接続してるのにおかしい
    _friendNameExpand(wk, GFL_NET_DWC_GetDisconnectNewPlayer());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }
#endif
  
  if((wk->preConnect == -1) && (GFL_NET_DWC_IsNewPlayer() != -1)){  // 通常のコネクト開始
    wk->preConnect = GFL_NET_DWC_IsNewPlayer();
    _friendNameExpand(wk, wk->preConnect);
    if(wk->DirectMacSet==0){
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_043, FALSE);
    }
    wk->localTime = 0;
  }
  else if(GFL_NET_DWC_GetDisconnectNewPlayer() != -1){
    _friendNameExpand(wk, GFL_NET_DWC_GetDisconnectNewPlayer());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }


  // 状態を取得
  status = _WifiMyStatusGet( wk, wk->pMatch );


  
  // 誰も自分に接続してきていないときだけリストを動かせる
  if(wk->preConnect == -1){

    // CANCELボタンでも待機状態をクリア
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
      if(_modeWait(status)){  // 待ち状態のとき
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
        WifiP2PMatch_UserDispOff_Any(wk, HEAPID_WIFIP2PMATCH); // した画面初期化
        return seq;
      }
    }
    ret = WIFI_MCR_Main( &wk->matchroom );
    WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン
  }
  switch(ret){
  case MCR_RET_NONE:
    return seq;
  case MCR_RET_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    if(_modeWait(status)){  // 待ち状態のとき
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_INIT);
    }
    else{ // それか終了チェックへ
      wk->endSeq = WIFI_GAME_NONE;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_YESNO);
      WifiP2PMatchMessagePrint(wk, dwc_message_0010, TRUE);
      //        wk->localTime=0;
      ret = BMPMENULIST_CANCEL;
    }
    WifiP2PMatch_UserDispOff_Any(wk, HEAPID_WIFIP2PMATCH); // した画面初期化
    return seq;

  case MCR_RET_MYSELECT:   //パソコンに話しかける
    {  // 募集の行で選択したとき
      if(status == WIFI_STATUS_WAIT){
        wk->state = WIFIP2PMATCH_STATE_MACHINE;  //NOTダイレクト
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        WIFI_MCR_PCAnmStart( &wk->matchroom );  // pcアニメ開始
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT);
        WifiP2PMatch_UserDispOff_Any(wk, HEAPID_WIFIP2PMATCH); // した画面初期化
        return seq;
      }
    }
    break;
  case MCR_RET_SELECT:   //相手に話しかける
    if(WIFI_STATUS_WAIT==status){  // 人の名前ー＞マッチングへ
      //相手の状態を確保 以後この状態をみるように変更 相手の動作をリアルタイムに追わない
      int friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
      PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
      GFL_STD_MemCopy(WifiFriendMatchStatusGet( friendNo - 1 ), &wk->targetStatus, sizeof(WIFI_STATUS));
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT);
      WifiP2PMatch_UserDispOff_Any(wk, HEAPID_WIFIP2PMATCH); // した画面初期化
      return seq;
    }
    break;
  default:
    GF_ASSERT(0);
    break;
  }
  return seq;
}




//------------------------------------------------------------------
/**
 * $brief   話しかけられた場合の確認画面に   WIFIP2PMATCH_MODE_CALLGAME_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _callGameInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  // だれかが話しかけてきた
  int n = wk->friendNo -1;

  if(PMSND_CheckPlaySE_byPlayerID( SE_CALL_SIGN_PLAYER )){
    return seq;
  }
  {
    u8 callcount = WIFI_STATUS_GetCallCounter(WifiFriendMatchStatusGet( n ));
    wk->pParentWork->matchno[n] = callcount;   //前回マッチングした時のno
  }
  
  if(n>=0 && n < WIFIP2PMATCH_MEMBER_MAX){
    if( WifiP2PMatch_CommWifiBattleStart( wk, n ) ){
      wk->cancelEnableTimer = _CANCELENABLE_TIMER;

      _commStateChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_UNIONMATCH);
      _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_UNIONMATCH);  // 接続中になる

      WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );  // した画面初期化

      _friendNameExpand(wk, n);

      wk->cancelEnableTimer = _CANCELENABLE_TIMER;
      WifiP2PMatchMessagePrintDirect(wk,msg_wifilobby_082, FALSE);
      WifiP2PMatchMessage_TimeIconStart(wk);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_LOOP);

//      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_MATCH_LOOP_MSG);
//      {
//        u8 callcount = WIFI_STATUS_GetCallCounter(WifiFriendMatchStatusGet( n ));
//        wk->pParentWork->matchno[n] = callcount;   //前回マッチングした時のno
//      }
      wk->DirectModeNo = wk->friendNo;
      return seq;
    }
  }
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST); //もとに戻る

  return seq;

}

//------------------------------------------------------------------
/**
 * $brief   VCTONOFFウインドウを閉じる  WIFIP2PMATCH_VCHATWIN_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatToggleWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_UI_KEY_GetTrg()){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

    FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
    EndMessageWindowOff(wk);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続開始
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectInit2( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2
{
  WIFI_STATUS* p_status;
  int vchat;
  int status;

  // VChatフラグを取得
  p_status = WifiFriendMatchStatusGet( wk->friendNo - 1 );
  vchat = WIFI_STATUS_GetVChatStatus(p_status);
  status  = _WifiMyStatusGet( wk, p_status );

  wk->cancelEnableTimer--;
  if(wk->cancelEnableTimer < 0  ){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT);
  }
#if PLFIX_T869
  // VCHATフラグがOFFになっていたら通信切断
  else if(vchat == FALSE){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
#endif
  // 相手の状態が通常状態でない場合切断
  else if( (status != WIFI_STATUS_WAIT) && (status != WIFI_STATUS_RECRUIT) ){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED){  // つながった
    //    _timeWaitIconDel(wk);   timeWait内でMsgWinを破棄しているということはメッセージ終了でもOK↓
    EndMessageWindowOff(wk);
    // VCT接続開始 + 接続MACは消去
    WIFI_STATUS_ResetVChatMac(wk->pMatch);
    wk->DirectMacSet = 0;
    _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_VCT);  // VCT中になる
    wk->VChatModeOff = FALSE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
    //WifiList_SetLastPlayDate( wk->pList, wk->friendNo - 1); // 最後に遊んだ日付は、VCTがつながったときに設定する
    _changeBGMVol( wk, 0 );

    WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_1015, FALSE);
  }

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続開始
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectInit( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT
{

  // 080703 tomoya takahashi
  // やはりはまるときはあるようなので、
  //　すぐに次にいくように修正する
  _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
  WifiP2PMatchMessagePrint(wk, msg_wifilobby_011, FALSE);
  _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_VCT);  // VCT中になる
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT);

  //WifiList_SetLastPlayDate( wk->pList, GFL_NET_DWC_GetFriendIndex()); // 最後に遊んだ日付は、VCTがつながったときに設定する

#if 0
  if(GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED){  // つながった
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_011, FALSE);
    _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_VCT);  // VCT中になる
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT);
  }
#endif

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続まち WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectWait( WIFIP2PMATCH_WORK *wk, int seq )        //
{

  WIFI_STATUS* p_status;
  int vchat;
  int status;

  if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
    return seq;
  }

  // 080624　追加
  // 通信リセットされるとindexが0より小さくなる
  // そしたら切断
  if( GFL_NET_DWC_GetFriendIndex() < 0 ){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }

  if( WifiP2PMatchMessageEndCheck(wk) ){
    if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
      EndMessageWindowOff(wk);
      WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_1015, FALSE);

      _myStatusChange(wk, WIFI_STATUS_PLAYING, WIFI_GAME_VCT);  // VCT中になる

      _changeBGMVol( wk, 0 );
      wk->VChatModeOff = FALSE;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
    }
    else{
      WifiP2PMatch_VCTConnect(wk,seq);
    }
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   VCT接続中  WIFIP2PMATCH_MODE_VCT_CONNECT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnect( WIFIP2PMATCH_WORK *wk, int seq )
{
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_VCTST, WB_NET_WIFICLUB);
  //BTS6463
  WifiList_SetLastPlayDate( wk->pList, GFL_NET_DWC_GetFriendIndex());
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT_MAIN);
  return seq;
}


static void _VctIconReloadFunc( WIFIP2PMATCH_WORK *wk )
{
  // アイコン書き換え
  if(GFL_NET_DWC_IsSendVoiceAndInc()){
    WifiP2PMatchFriendListIconWrite(   &wk->icon, GFL_BG_FRAME1_M,
                                       PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y, PLAYER_DISP_ICON_IDX_VCTBIG +PLAYER_DISP_ICON_MYMODE, 0 );
  }
  else{
    WifiP2PMatchFriendListIconWrite(   &wk->icon, GFL_BG_FRAME1_M,
                                       PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y, PLAYER_DISP_ICON_IDX_VCT_ACT + PLAYER_DISP_ICON_MYMODE, 0 );
  }
}


//------------------------------------------------------------------
/**
 * $brief   VCT接続中  WIFIP2PMATCH_MODE_VCT_CONNECT_MAIN
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectMain( WIFIP2PMATCH_WORK *wk, int seq )
{
  WIFI_STATUS* p_status;
  int status,gamemode;

  // VChatフラグを取得
  p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
  status  = _WifiMyStatusGet( wk, p_status );
  gamemode  = _WifiMyGameModeGet( wk, p_status );

  // 友達が現れたら出す
  MCRSYS_SetFriendObj( wk, HEAPID_WIFIP2PMATCH );
  MCVSys_UpdataBttn( wk );

  if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_017, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO);
  }
  else if(wk->VChatModeOff){
    EndMessageWindowOff(wk);
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
    wk->timer = 60;
    _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND3);
  }
  else{
    _VctIconReloadFunc(wk);   // アイコン書き換え
  }

  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   VCT接続終了待ち  WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO
 * @param   wk
 * @retval  seq
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectEndYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  _VctIconReloadFunc(wk);   // アイコン書き換え

  if( WifiP2PMatchMessageEndCheck(wk)){
    if(wk->VChatModeOff){
      EndMessageWindowOff(wk);
      _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
      wk->timer = 60;
      _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND3);
    }
    else{      // はいいいえウインドウを出す
      _yenowinCreateM2(wk);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT);
    }
  }

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続終了YESNO待ち  WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT
 * @param   wk
 * @retval  seq
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectEndWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret;

  _VctIconReloadFunc(wk);   // アイコン書き換え

  ret = _bmpMenu_YesNoSelectMain(wk);

  if(ret == BMPMENU_NULL){  // まだ選択中
    if(wk->VChatModeOff){
      EndMessageWindowOff(wk);
      BmpMenu_YesNoMenuExit( wk->pYesNoWork );
      wk->pYesNoWork = NULL;
      _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
      wk->timer = 60;
      _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND3);
    }
  }
  else if(ret == 0)
  { // はいを選択した場合
    _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND1);
  }
  else
  {  // いいえを選択した場合
    WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_1015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
  }
  return seq;
}


//WIFIP2PMATCH_VCTEND_COMMSEND1
static int WifiP2PMatch_VCTDisconnectSend1(WIFIP2PMATCH_WORK *wk, int seq)
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_VCHATOFF, 0, NULL)){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_VCTEND, WB_NET_WIFICLUB);
    _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND2);
  }
  return seq;
}


//WIFIP2PMATCH_VCTEND_COMMSEND2_ENDCHK
//VCTライブラリの終了待ち
static int WifiP2PMatch_VCTDisconnectSendEnd(WIFIP2PMATCH_WORK *wk, int seq)
{
  if(DWCRAP_IsVChat()==FALSE){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_SECOND_MATCH, WB_NET_WIFICLUB);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT0);
  }
  return seq;
}


//WIFIP2PMATCH_VCTEND_COMMSEND2
static int WifiP2PMatch_VCTDisconnectSend2(WIFIP2PMATCH_WORK *wk, int seq)
{
  wk->VChatModeOff = FALSE;

  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_VCTEND, WB_NET_WIFICLUB) || (GFL_NET_HANDLE_GetNumNegotiation()!=2 )){
    if((wk->state == WIFIP2PMATCH_STATE_TALK ) || (wk->state == WIFIP2PMATCH_STATE_RECV )){
      //ここでVCT切断
      DWCRAP_StopVChat();
      _changeBGMVol( wk, _VOL_DEFAULT );
      EndMessageWindowOff(wk);
      _myStatusChange(wk, WIFI_STATUS_CALL,WIFI_GAME_UNIONMATCH);
      _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND2_ENDCHK);
    }
    else{
      _myStatusChange(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
      GFL_NET_SetAutoErrorCheck(FALSE);
      GFL_NET_SetNoChildErrorCheck(FALSE);
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_DISCONNECT_END, WB_NET_WIFICLUB);
      _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT_PRE);
    }
  }
  return seq;
}



//WIFIP2PMATCH_RECONECTING_WAIT_PRE
static int WifiP2PMatch_ReconectingWaitPre(WIFIP2PMATCH_WORK *wk, int seq)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_DISCONNECT_END, WB_NET_WIFICLUB)
     || (GFL_NET_HANDLE_GetNumNegotiation()!=2 )){
    GFL_NET_StateWifiMatchEnd(TRUE);
    wk->preConnect = -1;
    wk->timer = _RECONECTING_WAIT_TIME;
    EndMessageWindowOff(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
  }
  return seq;
}



//WIFIP2PMATCH_VCTEND_COMMSEND3
static int WifiP2PMatch_VCTDisconnectSend3(WIFIP2PMATCH_WORK *wk, int seq)
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  wk->timer--;
  if(wk->timer < 0){
    //   if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_VCHATOFF, 0, NULL)){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_VCTEND, WB_NET_WIFICLUB);
    _CHANGESTATE(wk,WIFIP2PMATCH_VCTEND_COMMSEND2);
    // }
  }
  return seq;
}

//WIFIP2PMATCH_MODE_VCT_DISCONNECT
static int WifiP2PMatch_VCTDisconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
  u32 status;

  if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
    return seq;
  }


  // ユーザーデータOFF
  //  WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );

  status = _WifiMyStatusGet( wk, wk->pMatch );

  if(status != WIFI_STATUS_WAIT){
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
  }
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    wk->timer = 30;
    return seq;
  }
  wk->timer--;
  if((GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)) || (wk->timer==0)){
    EndMessageWindowOff(wk);
    GFL_NET_StateWifiMatchEnd(TRUE);

    // 通信状態を元に戻す
    //GFL_NET_StateChangeWiFiLogin();
    //    GFL_NET_ChangeInitStruct(&aGFLNetInit);
    _GFL_NET_InitAndStruct(wk,FALSE);

    //        if(wk->menulist==NULL){
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){
      wk->timer = _RECONECTING_WAIT_TIME;
      _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
    }
    else{

      // VCHATを元に戻す
      _myVChatStatusOrgSet( wk );
      _userDataInfoDisp(wk);

      wk->preConnect = -1;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
#ifdef BUGFIX_BTS7749_20100713
      WIFI_MCR_NpcPauseOff( &wk->matchroom, MCRSYS_GetMoveObjWork( wk, WIFI_MCR_PlayerSelect( &wk->matchroom ) ) );
#endif
      FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
    }
  }
  return seq;
}

//WIFIP2PMATCH_MODE_BATTLE_DISCONNECT
static int WifiP2PMatch_BattleDisconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    EndMessageWindowOff(wk);
    _myStatusChange(wk,  WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    GFL_NET_StateWifiMatchEnd(TRUE);
    wk->timer = _RECONECTING_WAIT_TIME;
    _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
  }
  return seq;
}




//WIFIP2PMATCH_MODE_DISCONNECT2
static int WifiP2PMatch_Disconnect2(WIFIP2PMATCH_WORK *wk, int seq)
{
  wk->timer++;
  if(wk->timer <= (60*4)){
    return seq;
  }

  // 話しかけている友達ナンバー取得
  WIFI_MCR_NpcPauseOff( &wk->matchroom, MCRSYS_GetMoveObjWork( wk, WIFI_MCR_PlayerSelect( &wk->matchroom ) ) );

  EndMessageWindowOff(wk);
  _myStatusChange(wk,  WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);

  GFL_NET_StateWifiMatchEnd(TRUE);
  // 通信状態を元に戻す
  _GFL_NET_InitAndStruct(wk,FALSE);

  if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){
    _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
  }
  else{
    // VCHATをオリジナルに戻す
    _myVChatStatusOrgSet( wk );
    _userDataInfoDisp(wk);

    wk->preConnect = -1;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
  }
  return seq;
}




//WIFIP2PMATCH_MODE_DISCONNECT
static int WifiP2PMatch_Disconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
  // ユーザーデータOFF
  //  WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );
  //




  // エラーチェック
  if( GFL_NET_StateIsWifiError() ){
    _errorDisp(wk);
    return seq;
  }


  if( !WifiP2PMatchMessageEndCheck(wk) ){
    wk->timer = 30;
    return seq;
  }
  wk->timer--;
  if((GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)) || (wk->timer==0)){


    // 話しかけている友達ナンバー取得
    WIFI_MCR_NpcPauseOff( &wk->matchroom, MCRSYS_GetMoveObjWork( wk, WIFI_MCR_PlayerSelect( &wk->matchroom ) ) );

    EndMessageWindowOff(wk);
    _myStatusChange(wk,  WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);

    GFL_NET_StateWifiMatchEnd(TRUE);
    // 通信状態を元に戻す
    //GFL_NET_StateChangeWiFiLogin();
    //    GFL_NET_ChangeInitStruct(&aGFLNetInit);
    _GFL_NET_InitAndStruct(wk,FALSE);

    //        if(wk->menulist==NULL){
    if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){
      _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
    }
    else{

      // VCHATをオリジナルに戻す
      _myVChatStatusOrgSet( wk );
      _userDataInfoDisp(wk);

      wk->preConnect = -1;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

      FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
    }
  }
  return seq;
}





static int WifiP2PMatch_BattleConnectInit( WIFIP2PMATCH_WORK *wk, int seq )    // WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT
{
  return seq;
}

static int WifiP2PMatch_BattleConnectWait( WIFIP2PMATCH_WORK *wk, int seq )     // WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT
{
  return seq;
}

static int WifiP2PMatch_BattleConnect( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_BATTLE_CONNECT
{
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   カーソル位置を変更する
 *
 * @param   act   アクターのポインタ
 * @param   x
 * @param   y
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void SetCursor_Pos( GFL_CLWK* act, int x, int y )
{
  GFL_CLACTPOS clp;

  clp.x = x;
  clp.y = y;
  GFL_CLACT_WK_SetWldPos(act ,&clp);

}

//------------------------------------------------------------------
/**
 * $brief   ボタンを押すと終了  WIFIP2PMATCH_MODE_CHECK_AND_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_CheckAndEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( wk->timer > 0 ){
    wk->timer --;
    return seq;
  }

  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){


    WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXITING);
    wk->timer = 1;

    /*
        wk->endSeq = WIFI_P2PMATCH_END;
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
        GFL_NET_StateWifiLogout();
//*/
    }
    return seq;
}



//------------------------------------------------------------------
/**
 * $brief   募集の取り消し WIFIP2PMATCH_MODE_SELECT_REL_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeSelectRelInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン
  WifiP2PMatchMessagePrint(wk, msg_wifilobby_007, FALSE);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_YESNO);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   募集の取り消し  WIFIP2PMATCH_MODE_SELECT_REL_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _parentModeSelectRelYesNo( WIFIP2PMATCH_WORK* wk, int seq )
{
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン
  if( WifiP2PMatchMessageEndCheck(wk) ){
    // はいいいえウインドウを出す
    _yenowinCreateM2(wk);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_REL_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   募集の取り消し  WIFIP2PMATCH_MODE_SELECT_REL_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _parentModeSelectRelWait( WIFIP2PMATCH_WORK* wk, int seq )
{
  int i;
  int ret;
  ret = _bmpMenu_YesNoSelectMain(wk);

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン

  if((wk->preConnect == -1) && (GFL_NET_DWC_IsNewPlayer() != -1)){  // 接続があった
    // すでにYesNoSelectMainで解放されてなければ
    if( ret == BMPMENU_NULL ){
      BmpMenu_YesNoMenuExit( wk->pYesNoWork );
      wk->pYesNoWork = NULL;
    }
    ret = 1;  // いいえに変更
  }
  if(ret == BMPMENU_NULL){  // まだ選択中
    return seq;
  }else if(ret == 0){ // はいを選択した場合
    //書き込み
    _myStatusChange(wk,  WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    GFL_NET_StateWifiMatchEnd(TRUE);
    // 通信状態を元に戻す
    _GFL_NET_InitAndStruct(wk,FALSE);

    FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    
  }
  else{  // いいえを選択した場合

    // 主人公動作停止を再度表示
//    FriendRequestWaitOff( wk );  //BTS6584
    //    FriendRequestWaitOn( wk, TRUE );
    //メッセージ交換だけ行う
    WifiP2PMatchMessagePrintDirect( wk, msg_wifilobby_142, FALSE );

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
  }

  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   待機状態になる為の選択メニュー WIFIP2PMATCH_MODE_SELECT_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeSelectMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  _ParentModeSelectMenu(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT2);
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン
  return seq;
}

//WIFIP2PMATCH_MODE_SELECT_INIT2
static int _parentModeSelectMenuInit2( WIFIP2PMATCH_WORK *wk, int seq )
{
  WifiP2PMatchMessagePrint(wk, msg_wifilobby_006, FALSE);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_WAIT);
  return seq;
}



static void _windelandSEcall(WIFIP2PMATCH_WORK *wk)
{
  if(wk->SubListWin){
    wk->SubListWin = _BmpWinDel(wk->SubListWin);
    BmpMenuList_Exit(wk->sublw, NULL,  &wk->singleCur[_MENUTYPE_GAME]);
    BmpMenuWork_ListDelete( wk->submenulist );
    EndMessageWindowOff(wk);
  }
}


//------------------------------------------------------------------
/**
 * $brief   待機状態になる為の選択メニュー WIFIP2PMATCH_MODE_SELECT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret;
  int num = PokeParty_GetBattlePokeNum(wk->pMyPoke);

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  ret = BmpMenuList_Main(wk->sublw);

  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    _windelandSEcall(wk);
    FriendRequestWaitOff(wk);      // 主人公の動作を許可 状態をNONEにもどす
    return seq;
    break;
  case WIFI_GAME_BATTLE_SINGLE_ALL:
    _windelandSEcall(wk);
    wk->battleMode = wk->pParentWork->battleBoard.battleModeSelect;
    wk->battleRule = wk->pParentWork->battleBoard.battleRuleSelect;
    wk->pParentWork->shooter = wk->pParentWork->battleBoard.shooterSelect;
    _battleCustomSelectMenu(wk,TRUE);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE2);
    return seq;
  case WIFI_GAME_VCT:
    if( !wk->pParentWork->vchatMain ){
      return seq;
    }
    else{
      _windelandSEcall(wk);
      WifiP2PMatch_CommWifiBattleStart( wk, -1 );
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
//      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW0);
    }
    break;
  case WIFI_GAME_TRADE:
    if(!_tradeNumCheck(wk)){
      wk->SubListWin = _BmpWinDel(wk->SubListWin);
      BmpMenuList_Exit(wk->sublw, NULL,  &wk->singleCur[_MENUTYPE_GAME]);
      BmpMenuWork_ListDelete( wk->submenulist );
      EndMessageWindowOff(wk);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_1013, FALSE);
      _CHANGESTATE(wk, WIFIP2PMATCH_MESSAGEEND_RETURNLIST);
      return seq;
    }
    else{
      _windelandSEcall(wk);
      WifiP2PMatch_CommWifiBattleStart( wk, -1 );
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
//      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW0);
    }
    break;
  case WIFI_GAME_TVT:
    if( !wk->pParentWork->vchatMain ){   //TVT選択不可
      return seq;
    }
    else{
      if((FALSE == DS_SYSTEM_IsRestrictPhotoExchange()) && DS_SYSTEM_IsRunOnTwl()){  //許可
        _windelandSEcall(wk);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_1046, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_TVTMESSAGE1_YESNO);
        return seq;
      }
      else{
        _windelandSEcall(wk);
        WifiP2PMatch_CommWifiBattleStart( wk, -1 );
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
//        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW0);
      }
    }
    break;
  }
  FriendRequestWaitOn( wk, TRUE );       // 動作停止させる
#ifdef BUGFIX_BTS7736_20100712
  wk->timer = 0;
#else
  wk->timer = 60 * 3;
#endif
  wk->GameModeKeep = ret;
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW_WAIT);

//  _myStatusChange(wk, WIFI_STATUS_RECRUIT, ret );  //状態を変更
  return seq;
}


//

//------------------------------------------------------------------
/**
 * $brief   状態を変更するのに間を空けて、飛ばないようにする BTS7685
    WIFIP2PMATCH_MODE_FRIENDLIST_MW_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _wifip2pmatch_mode_friendlist_mw_wait( WIFIP2PMATCH_WORK *wk, int seq )
{
  wk->timer--;
  if(wk->timer<0){
    _myStatusChange(wk, WIFI_STATUS_RECRUIT, wk->GameModeKeep );  //状態を変更
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   TVT確認メッセージ  WIFIP2PMATCH_MODE_TVTMESSAGE1_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _modeTVT1YesNo( WIFIP2PMATCH_WORK* wk, int seq )
{
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン
  if( WifiP2PMatchMessageEndCheck(wk) ){
    // はいいいえウインドウを出す
    _yenowinCreateM2(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_TVTMESSAGE1_WAIT);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   TVT確認メッセージ  WIFIP2PMATCH_MODE_TVTMESSAGE1_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _modeTVT1Wait( WIFIP2PMATCH_WORK* wk, int seq )
{
  int i;
  int ret;
  ret = _bmpMenu_YesNoSelectMain(wk);

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン

  if(ret == BMPMENU_NULL){  // まだ選択中
    return seq;
  }else if(ret == 0){ // はいを選択した場合
    WifiP2PMatch_CommWifiBattleStart( wk, -1 );
    _myStatusChange(wk, WIFI_STATUS_RECRUIT, WIFI_GAME_TVT );
    FriendRequestWaitOn( wk, TRUE );       // 動作停止させる
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
  }
  else{  // いいえを選択した場合
    FriendRequestWaitOff(wk);      // 主人公の動作を許可 状態をNONEにもどす
    EndMessageWindowOff(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   TVT確認メッセージ  WIFIP2PMATCH_MODE_TVTMESSAGE2_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _modeTVT2YesNo( WIFIP2PMATCH_WORK* wk, int seq )
{
  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン
  if( WifiP2PMatchMessageEndCheck(wk) ){
    // はいいいえウインドウを出す
    _yenowinCreateM2(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_TVTMESSAGE2_WAIT);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   TVT確認メッセージ  WIFIP2PMATCH_MODE_TVTMESSAGE2_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _modeTVT2Wait( WIFIP2PMATCH_WORK* wk, int seq )
{
  int i;
  int ret;
  WIFI_STATUS* p_status=&wk->targetStatus;
  int friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  MCR_MOVEOBJ* p_npc;

  ret = _bmpMenu_YesNoSelectMain(wk);
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン

  if(ret == BMPMENU_NULL){  // まだ選択中
    return seq;
  }else if(ret == 0){ // はいを選択した場合
    wk->state = WIFIP2PMATCH_STATE_MACHINE_RECV;
    if(WIFI_STATUS_GetVChatStatus(p_status) == WIFI_STATUS_GetVChatStatus(wk->pMatch)){
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_CHILD_CONNECT);
    }
    else{
      if(WIFI_STATUS_GetVChatStatus(wk->pMatch)){//自分がON
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_070, FALSE);
      }
      else{
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_069, FALSE);
      }
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_VCHAT_NEGO);
    }
  }
  else{  // いいえを選択した場合
    EndMessageWindowOff(wk);
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    wk->preConnect = -1;
    FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

    // NPCを元に戻す
    if( p_npc != NULL ){
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    }


  }
  return seq;
}




//------------------------------------------------------------------
/**
 * $brief   リストに戻る  WIFIP2PMATCH_MESSAGEEND_RETURNLIST
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int MessageEndReturnList( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if(GFL_UI_KEY_GetTrg()){
    EndMessageWindowOff(wk);
    GFL_NET_StateWifiMatchEnd(TRUE);
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    wk->preConnect = -1;
    FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   リストに戻る  WIFIP2PMATCH_RETURNLIST
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int ReturnList( WIFIP2PMATCH_WORK *wk, int seq )
{
  EndMessageWindowOff(wk);
  GFL_NET_StateWifiMatchEnd(TRUE);
  _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
  wk->preConnect = -1;
  FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   待機状態になる為の選択メニュー WIFIP2PMATCH_MODE_SUBBATTLE_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeSubSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret;

  WIFI_MCR_PCAnmMain( &wk->matchroom ); // パソコンアニメメイン

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  ret = BmpMenuList_Main(wk->sublw);
  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_SELECT_INIT);
    break;
  default:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    _myStatusChange(wk, WIFI_STATUS_RECRUIT ,ret);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    EndMessageWindowOff(wk);
    break;
  }


  //  {
  if( wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST ){  // 何か選択したときのみ、CANCELはだめ
    BOOL msg_on = TRUE;
    WifiP2PMatch_CommWifiBattleStart( wk, -1 );
    // 動作停止させる
    FriendRequestWaitOn( wk, msg_on );
  }



  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, &wk->singleCur[wk->bSingle]);
  BmpMenuWork_ListDelete( wk->submenulist );
  return seq;

}

//------------------------------------------------------------------
/**
 * $brief   マッチング申し込み WIFIP2PMATCH_MODE_MATCH_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _childModeMatchMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  int gmmno,gmmidx=0;
  u16 friendNo,status,gamemode;
  WIFI_STATUS* p_status = &wk->targetStatus;
  MCR_MOVEOBJ* p_player;
  MCR_MOVEOBJ* p_npc;
  u32 way;

  GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),FALSE);  //接続禁止

  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

  //NPCを自分の方向に向ける
  p_player = MCRSYS_GetMoveObjWork( wk, 0 );
  GF_ASSERT( p_player );

  way = WIFI_MCR_GetRetWay( p_player );
  WIFI_MCR_NpcPauseOn( &wk->matchroom, p_npc, way );

  status = _WifiMyStatusGet( wk, p_status );
  gamemode = _WifiMyGameModeGet( wk, p_status );

  _friendNameExpand(wk, friendNo - 1);

  if(WIFI_GAME_TRADE == gamemode){
    gmmidx = 1;
  }
  else if(WIFI_GAME_TVT == gamemode){
    gmmidx = 2;
  }
  else if(WIFI_GAME_VCT == gamemode){
    gmmidx = 3;
  }
  else if(_modeIsBattleStatus(gamemode)){
    gmmidx = 4;
  }

  switch(status){
  case WIFI_STATUS_NONE:
    gmmno = msg_wifilobby_077;
    break;
  case WIFI_STATUS_WAIT:   // 待機中
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    _playerDirectConnectStart(wk);
    //    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT2);
    return seq;
  case WIFI_STATUS_RECRUIT:    // 募集中
    {
      u32 msgbuff[]={msg_wifilobby_077, msg_wifilobby_004,msg_wifilobby_080,msg_wifilobby_078,msg_wifilobby_003};
      gmmno = msgbuff[gmmidx];
    }
    break;
  case WIFI_STATUS_PLAYING:      // 他の人と接続中
    {
      u32 msgbuff[]={msg_wifilobby_077, msg_wifilobby_049,msg_wifilobby_079,msg_wifilobby_050,msg_wifilobby_048};
      gmmno = msgbuff[gmmidx];
    }
    break;
  case WIFI_STATUS_PLAY_AND_RECRUIT:      // 他の人と接続中でさらに募集中
    gmmno = msg_wifilobby_077;
    break;
  default://WIFI_STATUS_UNKNOWN
    gmmno = msg_wifilobby_077;
    break;
  }

  WifiP2PMatchMessagePrint(wk, gmmno, FALSE);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_INIT2);
  return seq;
}

//----------------------------------------------------------------------------
/**
 *  @brief  メッセージの終了を見て、リストを出します WIFIP2PMATCH_MODE_MATCH_INIT2
 */
//-----------------------------------------------------------------------------
static int _childModeMatchMenuInit2( WIFIP2PMATCH_WORK *wk, int seq )
{
  MCR_MOVEOBJ* p_npc;
  u32 checkMatch;
  int friendNo;
  u32 status,gamemode;
  WIFI_STATUS* p_status = &wk->targetStatus;


  // エラーチェック
  if( GFL_NET_StateIsWifiError() ){
    _errorDisp(wk);
    return seq;
  }

  // 話しかけている友達ナンバー取得
  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );


  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  {
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = _WifiMyGameModeGet( wk, p_status );

    switch(status){
    case WIFI_STATUS_WAIT:   //相手指定で呼びかけるモードにいく
      _ChildModeMatchMenuDisp(wk);
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_PLAYERDIRECT_INIT);
      break;
    case WIFI_STATUS_RECRUIT:
    case WIFI_STATUS_PLAY_AND_RECRUIT:
      if(_modeIsConnectStatus(gamemode)){
        _ChildModeMatchMenuDisp(wk);
        _CHANGESTATE(wk, WIFIP2PMATCH_MODE_MATCH_WAIT);
        break;
      }
      //break throw
    case WIFI_STATUS_PLAYING:
    default:
      //メッセージ出して終了
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
      _CHANGESTATE(wk,WIFIP2PMATCH_MESSAGEEND_RETURNLIST);
      break;
    }
  }
  return seq;
}

static BOOL _playerDirectConnectStart( WIFIP2PMATCH_WORK *wk )
{
  int friendNo,message = 0,vchat,fst;
  u32 ret;
  int status,gamemode;
  u16 mainCursor;
  int checkMatch;
  MCR_MOVEOBJ* p_npc;
  WIFI_STATUS* p_status;

  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );

  wk->friendNo = friendNo;
  //OS_TPrintf("ダイレクト番号セット%d\n", friendNo);

  if(  friendNo != 0 ){
    int num = PokeParty_GetBattlePokeNum(wk->pMyPoke);

    p_status = WifiFriendMatchStatusGet( friendNo - 1 );
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = WIFI_GAME_UNIONMATCH;
    vchat = WIFI_STATUS_GetVChatStatus(p_status);

    if(WIFI_STATUS_UNKNOWN == status){
      return FALSE;
    }


    {
      //OS_TPrintf( "wifi接続先 %d %d\n", friendNo - 1,status );
      if( WifiP2PMatch_CommWifiBattleStart( wk, - 1 ) ){
        WIFI_STATUS_SetVChatMac(wk->pMatch, WifiFriendMatchStatusGet( friendNo - 1 ));

        wk->cancelEnableTimer = _CANCELENABLE_TIMER;
        _commStateChange(wk,WIFI_STATUS_CALL, gamemode);
        _myStatusChange(wk, WIFI_STATUS_CALL, gamemode);  // 呼びかけ待機中になる
        _friendNameExpand(wk, friendNo - 1);
        wk->DirectMacSet = friendNo;
        WifiP2PMatchMessagePrintDirect(wk,msg_wifilobby_014, FALSE);
        WifiP2PMatchMessage_TimeIconStart(wk);
        _CHANGESTATE(wk,  WIFIP2PMATCH_MODE_CONNECTWAIT);
        message = 1;
      }else{
        _friendNameExpand(wk, friendNo - 1);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
        message = 1;
      }
    }
  }
  return TRUE;
}


//WIFIP2PMATCH_MODE_CONNECTWAIT
static int _DirectConnectWait( WIFIP2PMATCH_WORK *wk, int seq  )
{
  MCR_MOVEOBJ* p_npc;

  //BTS3795-aの対処 Saito
  //メッセージを早送りするために読んでいます
  WifiP2PMatchMessageEndCheck(wk);

  if(GFL_NET_DWC_IsNewPlayer()!=-1){
#if 0 //def BUGFIX_BTS7751_20100713
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
#else
    _CHANGESTATE(wk,  WIFIP2PMATCH_MODE_FRIENDLIST);
#endif
  }
  if(DWC_STATUS_OFFLINE == WifiDwc_getFriendStatus(wk->friendNo-1)){  //BTS6212 BTS6409
    _friendNameExpand(wk, wk->friendNo - 1);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }

  wk->timer++;

  if( (wk->timer % (15*60))==0 ){  //カウントアップ
    WIFI_STATUS_SetVChatMac(wk->pMatch, WifiFriendMatchStatusGet( wk->friendNo-1 ));
    _myStatusChange(wk, WIFI_STATUS_CALL, WIFI_GAME_UNIONMATCH);  // 呼びかけ待機中になる
  }
#ifdef BUGFIX_BTS7749_20100713
  if(wk->timer > (60*60)){  //繰り返してもつながらない場合切ってしまう
    _friendNameExpand(wk, wk->friendNo - 1);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }
#endif

  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_068, FALSE);
    _yenowinCreateM2(wk);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CONNECTWAIT2);
  }
  return seq;
}

//WIFIP2PMATCH_MODE_CONNECTWAIT2
static int _DirectConnectWait2( WIFIP2PMATCH_WORK *wk, int seq  )
{
  int ret;
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  ret = _bmpMenu_YesNoSelectMain(wk);
  if(ret == BMPMENU_NULL){  // まだ選択中
    return seq;
  }else if(ret == 0){ // はいを選択した場合
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_071, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
  }
  else{  // いいえを選択した場合
    EndMessageWindowOff(wk);
    WifiP2PMatchMessagePrint(wk,msg_wifilobby_014, FALSE);
    WifiP2PMatchMessage_TimeIconStart(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CONNECTWAIT);
  }
  return seq;
}


//----------------------------------------------------------------------------
/**
 *  @brief  相手を指定して接続 WIFIP2PMATCH_MODE_PLAYERDIRECT_INIT
 */
//-----------------------------------------------------------------------------
static int _playerDirectInit( WIFIP2PMATCH_WORK *wk, int seq )
{


  u32 ret;
  int status,gamemode;
  int friendNo,message = 0,vchat,fst;
  u16 mainCursor;
  int checkMatch;
  MCR_MOVEOBJ* p_npc;
  WIFI_STATUS* p_status;


  ret = BmpMenuList_Main(wk->sublw);

  // 話しかけている友達ナンバー取得
  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

  // エーラーチェック
  if(GFL_NET_StateIsWifiError()){
    wk->SubListWin = _BmpWinDel(wk->SubListWin);
    BmpMenuList_Exit(wk->sublw, NULL, NULL);
    BmpMenuWork_ListDelete( wk->submenulist );

    if( p_npc != NULL ){
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    }

    _errorDisp(wk);
    return seq;
  }


  switch(ret){
  case BMPMENULIST_NULL:

    // 相手がいなくなったり
    // 相手のステータスが違うものに変わったら、
    // 表示を消して元に戻る
    if( p_npc == NULL ){
      BmpMenuList_Exit( wk->sublw, NULL, NULL );
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
      message = 1;
      break;
    }else{
      p_status = WifiFriendMatchStatusGet( friendNo - 1 );
      status = _WifiMyStatusGet( wk, p_status );
      vchat = WIFI_STATUS_GetVChatStatus(p_status);
      gamemode = _WifiMyGameModeGet( wk, p_status );

      // 状態がかわったり
      // 4人募集のゲームじゃないのに、VCHATフラグが変わったら切断
      if((_WifiMyGameModeGet( wk, &wk->targetStatus ) != gamemode) ||
         ( (WIFI_STATUS_GetVChatStatus( &wk->targetStatus ) != vchat)) ){

        _friendNameExpand(wk, friendNo - 1);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
        message = 1;
        break;
      }
    }
    return seq;

  case BMPMENULIST_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_CANCEL1);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    break;
  default:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    if(ret == _CONNECTING){  // MACをビーコンで流し、親になる
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
      _playerDirectConnectStart(wk);
    }
    else if(ret == _INFOVIEW){
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_PERSONAL_INIT);
    }
    break;
  }


  // 後始末

  if(message==0){
    EndMessageWindowOff(wk);
  }
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, NULL);
  BmpMenuWork_ListDelete( wk->submenulist );

  // NPCを元に戻す
  if( p_npc != NULL ){
    WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
  }

  // 戻るだけなら人の情報を消す
  if( wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST ){

    // その人の情報を消す
    //    WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );

  }else{

    // 繋がりにいくとき

  }

  return seq;

}




//----------------------------------------------------------------------------
/**
 *  @brief  ロビー用のBGMを設定する
 */
//-----------------------------------------------------------------------------
static void WifiP2P_SetLobbyBgm( void )
{
  u32 bgm;

  // ボイスチャットなしにする
  GFL_NET_DWC_SetVChat(FALSE);    // ボイスチャットとBGM音量の関係を整理 tomoya takahashi

}

//------------------------------------------------------------------
/**
 * $brief   つなぎに行く選択メニュー WIFIP2PMATCH_MODE_MATCH_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _childModeMatchMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret;
  int status,gamemode;
  int friendNo,message = 0,vchat,fst;
  u16 mainCursor;
  int checkMatch;
  MCR_MOVEOBJ* p_npc;
  WIFI_STATUS* p_status=&wk->targetStatus;

  ret = BmpMenuList_Main(wk->sublw);

  checkMatch = _checkParentNewPlayer(wk);
  if( 0 !=  checkMatch ){ // 接続してきた
    ret = BMPMENULIST_CANCEL;
  }

  // 話しかけている友達ナンバー取得
  friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  wk->friendNo = friendNo;
  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

  // エラーチェック
  if(GFL_NET_StateIsWifiError()){
    wk->SubListWin = _BmpWinDel(wk->SubListWin);
    BmpMenuList_Exit(wk->sublw, NULL, NULL);
    BmpMenuWork_ListDelete( wk->submenulist );

    if( p_npc != NULL ){
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    }

    _errorDisp(wk);
    return seq;
  }
  if(ret != BMPMENULIST_NULL){
    EndMessageWindowOff(wk);
    wk->SubListWin = _BmpWinDel(wk->SubListWin);
    BmpMenuList_Exit(wk->sublw, NULL, NULL);
    BmpMenuWork_ListDelete( wk->submenulist );
  }
  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    PMSND_PlaySystemSE(SEQ_SE_CANCEL1);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    break;
  default:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    gamemode = _WifiMyGameModeGet( wk, p_status );
    if(gamemode == WIFI_GAME_TVT){
      if((FALSE == DS_SYSTEM_IsRestrictPhotoExchange()) && DS_SYSTEM_IsRunOnTwl()){  //許可
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_1046, FALSE);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_TVTMESSAGE2_YESNO);
        return seq;
      }
    }
    if(gamemode == WIFI_GAME_TRADE){
      if(!_tradeNumCheck(wk)){
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_1013, FALSE);
        _CHANGESTATE(wk, WIFIP2PMATCH_MESSAGEEND_RETURNLIST);
        break;
      }
    }
    if(_modeIsBattleStatus(gamemode)){
      wk->battleMode = (gamemode - WIFI_GAME_BATTLE_SINGLE_ALL) / 2;
      wk->battleRule = (gamemode - WIFI_GAME_BATTLE_SINGLE_ALL) % 2;
      {
        u32 regulation = _createRegulation(wk);
        if(!_regulationCheck(wk)){
          WifiP2PMatchMessagePrint(wk, msg_wifilobby_100, FALSE);
          _CHANGESTATE(wk, WIFIP2PMATCH_MESSAGEEND_RETURNLIST);
        }
      }
    }
    wk->state = WIFIP2PMATCH_STATE_MACHINE_RECV;
    if(WIFI_STATUS_GetVChatStatus(p_status) == WIFI_STATUS_GetVChatStatus(wk->pMatch)){
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_CHILD_CONNECT);
    }
    else{
      if(WIFI_STATUS_GetVChatStatus(wk->pMatch)){//自分がON
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_070, FALSE);
      }
      else{
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_069, FALSE);
      }
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_VCHAT_NEGO);
    }
    break;
  }
  // NPCを元に戻す
  if( p_npc != NULL ){
    WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
  }
  return seq;
}






//------------------------------------------------------------------
/**
 * $brief   子機がつなぎに行く WIFIP2PMATCH_MODE_CHILD_CONNECT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _childModeConnect( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 ret;
  int status,gamemode;
  int message = 0,vchat,fst;
  u16 mainCursor;
  int checkMatch;
  MCR_MOVEOBJ* p_npc;
  WIFI_STATUS* p_status=&wk->targetStatus;
  int friendNo = wk->friendNo;
  int num = PokeParty_GetBattlePokeNum(wk->pMyPoke);


  if(  wk->friendNo == 0 ){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_071, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
  }

  {
    p_status = WifiFriendMatchStatusGet( friendNo - 1 );
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = _WifiMyGameModeGet( wk, p_status );
    vchat = WIFI_STATUS_GetVChatStatus(p_status);
    if(
      (_WifiMyStatusGet( wk, &wk->targetStatus) != status) ||
      (_WifiMyGameModeGet( wk, &wk->targetStatus) != gamemode) ||
      (WIFI_STATUS_GetVChatStatus( &wk->targetStatus) != vchat) ||
      (WIFI_STATUS_UNKNOWN == status)
      ){
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
      return seq;
    }
  }

  //自分が子機になって相手に接続
  fst = WifiDwc_getFriendStatus(friendNo - 1);

  {
    //OS_TPrintf( "wifi接続先 %d %d\n", friendNo - 1,status );
    if( WifiP2PMatch_CommWifiBattleStart( wk, friendNo - 1 ) ){

      wk->cancelEnableTimer = _CANCELENABLE_TIMER;
      _commStateChange(wk,WIFI_STATUS_CALL, gamemode);
      _myStatusChange(wk, WIFI_STATUS_CALL, gamemode);  // 接続待機中になる
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrintDirect(wk,msg_wifilobby_014, FALSE);
      WifiP2PMatchMessage_TimeIconStart(wk);
      wk->friendNo = friendNo;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MATCH_LOOP);
    }
    else{
      _friendNameExpand(wk, friendNo - 1);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
    }
  }
  return seq;
}


//WIFIP2PMATCH_MODE_MATCH_LOOP
static int _childModeMatchMenuLoop( WIFIP2PMATCH_WORK *wk, int seq )
{
  int status;

  //BTS2302の対処 naigihashi
  //メッセージを早送りするために読んでいます
  //メッセージを描画していなくてもここに来る場合があります
  WifiP2PMatchMessageEndCheck(wk);

  wk->cancelEnableTimer--;
  if(wk->cancelEnableTimer < 0  ){
    MCR_MOVEOBJ* p_npc;                    //BTS7577 
    int friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
    p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );
    // NPCを元に戻す
    if( p_npc != NULL ){
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    }
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT);

  }
  else if(GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_FAIL ){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_DWC_GetStepMatchResult() == GFL_NET_STATE_FAIL){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_012, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if((GFL_NET_DWC_GetStepMatchResult() == GFL_NET_STATE_DISCONNECTING) || (GFL_NET_StateIsWifiDisconnect())){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateGetWifiStatus()==GFL_NET_STATE_MATCHED){  // 相手に接続した

    WIFI_STATUS_ResetVChatMac(wk->pMatch);
    _sendMatchStatus(wk);
    // ２〜４人募集でないとき
    status = _WifiMyStatusGet( wk, wk->pMatch );
    {

      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
      //    _timeWaitIconDel(wk);   timeWait内でMsgWinを破棄しているということはメッセージ終了でもOK↓
      EndMessageWindowOff(wk);
      //    CommInfoInitialize(wk->pSaveData,NULL);   //Info初期化   //@@OO
      CommCommandWFP2PMF_MatchStartInitialize(wk);
      wk->timer = 30;

      GFL_NET_SetAutoErrorCheck(TRUE);
      GFL_NET_SetNoChildErrorCheck(TRUE);
    }
  }
  return seq;
}

//-------------------------------------VCTのキャンセル機能を急遽実装
//-------------------------------------VCTのキャンセル機能を急遽取り外し

//------------------------------------------------------------------
/**
 * $brief   Bキャンセル  WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelYesNoVCT( WIFIP2PMATCH_WORK *wk, int seq )
{
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   Bキャンセル  WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelWaitVCT( WIFIP2PMATCH_WORK *wk, int seq )
{
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   DWC切断 WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_CancelEnableWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  _myVChatStatusOrgSet(wk);
  _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
  GFL_NET_StateWifiMatchEnd(TRUE);
  wk->preConnect = -1;
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
  EndMessageWindowOff(wk);
  return seq;
}


//------------------------------------------

//------------------------------------------------------------------
/**
 * $brief   Bキャンセル  WIFIP2PMATCH_MODE_BCANCEL_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelYesNo( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT
{
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   Bキャンセル  WIFIP2PMATCH_MODE_BCANCEL_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelWait( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT
{
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   親機受付まち。子機から応募があったことを知らせる WIFIP2PMATCH_MODE_CALL_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeCallMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  int mySt;
  int targetSt;
  WIFI_STATUS* p_status;
  int myvchat;


  if(PMSND_CheckPlaySE_byPlayerID( SE_CALL_SIGN_PLAYER )){
    return seq;
  }
  if(  DWC_STATUS_OFFLINE == WifiDwc_getFriendStatus(GFL_NET_DWC_GetFriendIndex())){  //社内で発見された
    _friendNameExpand(wk, wk->preConnect);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    return seq;
  }
  p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
  mySt = _WifiMyStatusGet( wk, wk->pMatch );
  targetSt = _WifiMyStatusGet( wk,p_status );
  myvchat = WIFI_STATUS_GetVChatStatus( wk->pMatch );
  wk->friendNo = GFL_NET_DWC_GetFriendIndex()+1;
  //OS_TPrintf("ダイレクト%d\n", wk->friendNo);
  if(wk->DirectMacSet==0){
    _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_008, FALSE);
  }
  CommCommandWFP2PMF_MatchStartInitialize(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
  wk->timer = 30;
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   コネクションはろうとしている期間中のエラー表示
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static BOOL _connectingErrFunc(WIFIP2PMATCH_WORK *wk)
{
  if(GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_TIMEOUT){
    _friendNameExpand(wk, wk->preConnect);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(GFL_NET_StateIsWifiError()){
    _errorDisp(wk);
  }
  else{
    return FALSE;
  }
  wk->bRetryBattle = FALSE;
  return TRUE;
}

//------------------------------------------------------------------
/**
 * $brief   つながるべきステート確認  WIFIP2PMATCH_MODE_CALL_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMenuYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  //BTS2302の対処 naigihashi
  //メッセージを早送りするために読んでいます
  ////メッセージを描画していなくてもここに来る場合があります
  WifiP2PMatchMessageEndCheck(wk);

  if((GFL_NET_StateGetWifiStatus() >= GFL_NET_STATE_DISCONNECTING) || GFL_NET_StateIsWifiDisconnect() ){

    if(wk->bRetryBattle){
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_065, FALSE);
    }
    else{
      _friendNameExpand(wk, wk->preConnect);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
    }
    wk->bRetryBattle = FALSE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
  }
  else if(_connectingErrFunc(wk)){
  }
  else{

    if(wk->timer > 1){
      wk->timer--;
    }

    if(wk->timer==0){
      if(GFL_NET_HANDLE_IsNegotiation(GFL_NET_HANDLE_GetCurrentHandle())){
        GFL_STD_MemClear(&wk->matchGameMode,sizeof(wk->matchGameMode));
        GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_CHECK2,WB_NET_WIFICLUB);
        wk->bRetryBattle = FALSE;
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_066, FALSE);
        WifiP2PMatchMessage_TimeIconStart(wk);
        _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_SEND);
      }
    }
    else{
      if(wk->timer == 1){
        if(GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())!=0){
          if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
            wk->timer = 0;
          }
        }
        else if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
          if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
            wk->timer = 0;
          }
        }
      }
    }
  }
  return seq;
}




//------------------------------------------------------------------
/**
 * $brief   MySTATUSを受信 CNM_WFP2PMF_MYSTATUS
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvMyStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;
  MYSTATUS* pTargetSt;
  const u16 *pRecvData = pData;


  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
  }
  //０，１，２，３が通信用に入れても良い
  pTargetSt = GAMEDATA_GetMyStatusPlayer(wk->pGameData,netID);  //netIDで代入
  MyStatus_Copy(pData,pTargetSt);

  {
    ETC_SAVE_WORK * pETC = SaveData_GetEtc( wk->pSaveData );
    EtcSave_SetAcquaintance(pETC, MyStatus_GetID(pTargetSt));
  }
}



//------------------------------------------------------------------
/**
 * $brief   PokePartyを受信 CNM_WFP2PMF_POPEPARTY
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvPokeParty(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{

  //すでに受信は完了

}

//------------------------------------------------------------------
/**
 * $brief   PokePartyを受信 CNM_WFP2PMF_POPEPARTY
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
void WifiP2PMatchRecvVctOff(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
  }
  wk->VChatModeOff=TRUE;
}


//------------------------------------------------------------------
/**
 * $brief   PokePartyの受信ワークを返す CNM_WFP2PMF_POPEPARTY
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

u8* WifiP2PMatchGetPokePartyWork(int netID, void* pWk, int size)
{
  WIFIP2PMATCH_WORK *wk = pWk;
  return (void*)wk->pParentWork->pPokeParty[netID];
}

//------------------------------------------------------------------
/**
 * $brief   直接接続時のフローコマンドを受信 CNM_WFP2PMF_DIRECT_COMMAND
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvDirectMode(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;
  MYSTATUS* pTargetSt;
  const u8 *pChangeStateNo = pData;


  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
  }

  if(wk->seq == WIFIP2PMATCH_PLAYERDIRECT_WAIT){
    OS_TPrintf("_CHANGESTATE %d\n",pChangeStateNo[0]);
    wk->nextSeq = pChangeStateNo[0];
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_DIRECTE, WB_NET_WIFICLUB );
  }
  else{
    GF_ASSERT(0);  // 不正コマンド受信は切断
    _CHANGESTATE(wk, WIFIP2PMATCH_MODE_DISCONNECT);
  }
  //  wk->matchGameMode[netID] = pRecvData[0];
}

//------------------------------------------------------------------
/**
 * $brief   レギュレーションを受信 CNM_WFP2PMF_DIRECT_COMMAND CNM_WFP2PMF_REGLATION
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvBattleRegulation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;
  MYSTATUS* pTargetSt;
  const u32 *pRegNo = pData;


  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
  }
  //  wk->matchRegulation = pRegNo[0];
  _convertRegulation(wk,pRegNo[0]);


}

typedef struct{
  u16 gamemode;
  u16 vct;
  u32 regcode;
} _WFP2PMF_STATUS;



//------------------------------------------------------------------
/**
 * $brief   つながるべきステートを受信 CNM_WFP2PMF_STATUS
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

void WifiP2PMatchRecvGameStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  WIFIP2PMATCH_WORK *wk = pWork;
  const _WFP2PMF_STATUS *pRecvData = pData;

  wk->matchGameMode[netID] = pRecvData->gamemode;
  wk->pParentWork->VCTOn[netID] = pRecvData->vct;
  _convertRegulation(wk,pRecvData->regcode);
}

//
//------------------------------------------------------------------
/**
 * $brief   つながるべきステートを送信  WIFIP2PMATCH_MODE_CALL_SEND
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeCallMenuSend( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(_connectingErrFunc(wk)){
  }
  else if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_GAME_CHECK2, WB_NET_WIFICLUB)){
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_MYSTATUS, MyStatus_GetWorkSize(), GAMEDATA_GetMyStatus(wk->pGameData))){
      EndMessageWindowOff(wk);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_CHECK_D);
    }
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   つながるべきステートを送信  WIFIP2PMATCH_MODE_CALL_CHECK_D
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeCallMenuSendD( WIFIP2PMATCH_WORK *wk, int seq )
{
  _WFP2PMF_STATUS regStatus;
  u16 status = _WifiMyStatusGet( wk, wk->pMatch );
  u32* pData;

  regStatus.gamemode = _WifiMyGameModeGet( wk, wk->pMatch );
  
  if(wk->vchatrev){  //マシンでの暫定VCTフラグ
    regStatus.vct = wk->vchatrev-1;
    wk->vchatrev=0;
  }
  else{
    regStatus.vct = wk->pParentWork->vchatMain;
  }
  regStatus.regcode = _createRegulationType(wk,REG_CREATE_TYPE_SELECT);
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_STATUS, sizeof(regStatus), &regStatus)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_CHECK);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   つながるべきステート解析  WIFIP2PMATCH_MODE_CALL_CHECK
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMenuCheck( WIFIP2PMATCH_WORK *wk, int seq )
{
  int id=0;

  if(GFL_NET_IsParentMachine()){  //相手側のIDをみる
    id = 1;
  }
  if(_connectingErrFunc(wk)){
  }
  else if(WIFI_GAME_NONE != wk->matchGameMode[id]){  // 送られてきた
    u16 gamemode = _WifiMyGameModeGet( wk, wk->pMatch );
    if(wk->matchGameMode[id] == gamemode){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_START,WB_NET_WIFICLUB);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_MYSTATUS_WAIT);
    }
    else{  // 異なるステートを選択した場合
      _friendNameExpand(wk, GFL_NET_DWC_GetFriendIndex());
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_DISCONNECT);
    }
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   タイミングコマンド待ち WIFIP2PMATCH_MODE_MYSTATUS_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMyStatusWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int status;

  if(_connectingErrFunc(wk)){
  }
  else if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_GAME_START, WB_NET_WIFICLUB)){
    GFL_NET_SetAutoErrorCheck(TRUE);
    GFL_NET_SetNoChildErrorCheck(TRUE);

    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,_TIMING_GAME_START2,WB_NET_WIFICLUB);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_WAIT);
  }
  return seq;
}


static BOOL _parent_MsgEndCheck( WIFIP2PMATCH_WORK *wk )
{
  if(WifiP2PMatchMessageEndCheck(wk)){
    return TRUE;
  }

  if( !wk->MsgWin ){
    return TRUE;
  }

  return FALSE;
}

//------------------------------------------------------------------
/**
 * $brief   タイミングコマンド待ち WIFIP2PMATCH_MODE_CALL_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 status,gamemode;
  int checkFriend[GFL_NET_MACHINE_MAX];
  WIFI_STATUS* p_status;

  if(_connectingErrFunc(wk)){
  }
  else if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_GAME_START2,WB_NET_WIFICLUB)
          && (_parent_MsgEndCheck( wk ) == TRUE) ){   // メッセージの終了も待つように変更 08.06.01  tomoya
    GFL_NET_DWC_FriendAutoInputCheck( WifiList_GetDwcDataPtr(GAMEDATA_GetWiFiList(wk->pGameData), 0));

    EndMessageWindowOff(wk);

    p_status = WifiFriendMatchStatusGet( GFL_NET_DWC_GetFriendIndex() );
    status = _WifiMyStatusGet( wk, p_status );
    gamemode = _WifiMyGameModeGet( wk, p_status );

    if(gamemode == WIFI_GAME_UNIONMATCH){ //繋ぐだけ。繋いだ後に各ゲーム選択
      _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_INIT1);
    }
    else{
      /// 検査後にゲームに飛ぶ
      status = WIFI_STATUS_PLAYING;
      _myStatusChange(wk, status,gamemode);  // 接続中になる
      _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERMACHINE_INIT1);
    }
  }
  return seq;
}



//------------------------------------------------------------------
/**
 * $brief   メインメニューの戻る際の初期化
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_MainReturn( WIFIP2PMATCH_WORK *wk, int seq )
{
  GFL_BG_ClearFrame(  GFL_BG_FRAME3_M);
  wk->mainCur = 0;
  _CHANGESTATE(wk,WifiP2PMatchFriendListStart(wk,FALSE));
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   個人データ表示初期化 WIFIP2PMATCH_MODE_PERSONAL_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _personalDataInit( WIFIP2PMATCH_WORK *wk, int seq )
{
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_PERSONAL_WAIT);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   個人データ表示 WIFIP2PMATCH_MODE_PERSONAL_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _personalDataWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if( 0 !=  _checkParentConnect(wk)){ // 接続してきた
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_PERSONAL_END);
  }
  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_PERSONAL_END);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   個人データ表示おわり WIFIP2PMATCH_MODE_PERSONAL_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _personalDataEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i,x;
  int num = 0;

  EndMessageWindowOff(wk);


  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   終了確認メッセージ  WIFIP2PMATCH_MODE_EXIT_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _exitYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( WifiP2PMatchMessageEndCheck(wk) ){
    // はいいいえウインドウを出す
    _yenowinCreateM2(wk);

    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_WAIT);
  }


  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   終了確認メッセージ  WIFIP2PMATCH_MODE_EXIT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _exitWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret;

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  
  ret = _bmpMenu_YesNoSelectMain(wk);

  if(ret == BMPMENU_NULL){  // まだ選択中
    return seq;
  }else if(ret == 0){ // はいを選択した場合
    wk->pYesNoWork=NULL;
    WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXITING);
    wk->timer = 1;
  }
  else{  // いいえを選択した場合
    wk->pYesNoWork=NULL;
    EndMessageWindowOff(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
    FriendRequestWaitOff( wk );      // 主人公の動作を許可 状態をNONEにもどす
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   終了確認メッセージ  WIFIP2PMATCH_MODE_EXITING
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _exitExiting( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  if(wk->timer == 1){
    wk->timer = 0;
    GFL_NET_Exit(NULL);
  }
  if(!GFL_NET_IsInit()){
    //OS_TPrintf("切断した時にフレンドコードを詰める\n");
    WifiList_FormUpData(wk->pList);  // 切断した時にフレンドコードを詰める
    WifiP2PMatchMessagePrint(wk, dwc_message_0012, TRUE);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_EXIT_END);
    wk->timer = 30;
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   終了完了  WIFIP2PMATCH_MODE_EXIT_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------


static int _exitEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  wk->timer--;
  if(wk->timer==0){
    wk->endSeq = WIFI_GAME_NONE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_END_WAIT);
    EndMessageWindowOff(wk);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   もう一度対戦するか聞く WIFIP2PMATCH_NEXTBATTLE_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _nextBattleYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_BATTLE_END,WB_NET_WIFICLUB)==FALSE){
    return seq;
  }
  // 通信同期中に電源を切られたら、ずっと同期待ちしてしまうので、通信同期後にオートエラーチェックを
  // はずす
  GFL_NET_SetAutoErrorCheck(FALSE);
  GFL_NET_SetNoChildErrorCheck(TRUE);

  if( WifiP2PMatchMessageEndCheck(wk) ){
    _yenowinCreateM2(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_NEXTBATTLE_WAIT);
  }

  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   もう一度対戦するか聞く WIFIP2PMATCH_NEXTBATTLE_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _nextBattleWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret;

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }

  ret = _bmpMenu_YesNoSelectMain(wk);
  if(ret == BMPMENU_NULL){  // まだ選択中
    return seq;
  }else if(ret == 0){ // はいを選択した場合
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_073, FALSE);
    WifiP2PMatchMessage_TimeIconStart(wk);
    wk->bRetryBattle = TRUE;
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_CALL_YESNO);
    wk->timer = 30;
  }
  else{  // いいえを選択した場合
    EndMessageWindowOff(wk);
    GFL_NET_StateWifiMatchEnd(TRUE);
    wk->timer = _RECONECTING_WAIT_TIME;
    _CHANGESTATE(wk,WIFIP2PMATCH_RECONECTING_WAIT);
  }
  //        wk->pMatch = GFL_NET_StateGetMatchWork();  //@@
  //      wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
  return seq;
}


//------------------------------------------------------------------
/**
 * $brief   VCAHTを変更してもいいかどうか聞く WIFIP2PMATCH_MODE_VCHAT_NEGO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatNegoCheck( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( WifiP2PMatchMessageEndCheck(wk) ){
    _yenowinCreateM2(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCAHTを変更してもいいかどうか聞く WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatNegoWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  int i;
  int ret,status;
  int friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
  MCR_MOVEOBJ* p_npc;

  p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );
  
  GFL_FONTSYS_SetDefaultColor();
  ret = _bmpMenu_YesNoSelectMain(wk);
  if(ret == BMPMENU_NULL){  // まだ選択中
    return seq;
  }else if(ret == 0){ // はいを選択した場合
    // 接続開始
    //VCT反転
    WIFI_STATUS_SetVChatStatus(wk->pMatch, 1-WIFI_STATUS_GetVChatStatus(wk->pMatch));
    WIFI_STATUS_SetUseVChat(wk->pMatch, WIFI_STATUS_GetVChatStatus(wk->pMatch));
    wk->vchatrev = (1 - wk->pParentWork->vchatMain) + 1;  //反転フラグを格納
    _CHANGESTATE(wk, WIFIP2PMATCH_MODE_CHILD_CONNECT);
  }
  else{
    wk->state = WIFIP2PMATCH_STATE_NONE;
    _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
    wk->preConnect = -1;
    // NPCを元に戻す
    if( p_npc != NULL ){
      WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
    }
    // VCHAT元に戻す
    _myVChatStatusOrgSet( wk );
    _userDataInfoDisp(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);
  }
  EndMessageWindowOff(wk);
  return seq;
}


// WIFIP2PMATCH_MODE_END_WAIT
//------------------------------------------------------------------
/**
 * $brief   終了
 *
 * @param   wk
 * @param   seq
 *
 * @retval  int
 */
//------------------------------------------------------------------
static int  WifiP2PMatch_EndWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(!GFL_NET_IsInit()){
    //        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
    //                      COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
    //  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
    //                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );
    //    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT,
    //                              0,16,COMM_BRIGHTNESS_SYNC);
    seq = SEQ_OUT;            //終了シーケンスへ
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * $brief
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _myStatusChange(WIFIP2PMATCH_WORK *wk, int status,int gamemode)
{
  if(wk->pMatch==NULL){  //0707
    return;
  }
  if((status == WIFI_STATUS_WAIT) && (gamemode==WIFI_GAME_LOGIN_WAIT)){
    WIFI_STATUS_ResetVChatMac(wk->pMatch);
  }
  if((status == WIFI_STATUS_PLAYING) && (gamemode<WIFI_GAME_UNIONMATCH)){
    //バトルや対戦以外でも更新する為にここにした
    WifiList_SetLastPlayDate( wk->pList, GFL_NET_DWC_GetFriendIndex());
  }
  _myStatusChange_not_send( wk, status, gamemode );
  _sendMatchStatus(wk);
}

//----------------------------------------------------------------------------
/**
 *  @brief  自分のステータス設定    ただ送信はしない
 *
 *  @param  wk
 *  @param  status
 */
//-----------------------------------------------------------------------------
static void _myStatusChange_not_send(WIFIP2PMATCH_WORK *wk, int status,int gamemode)
{
  int org_status,org_mode;

  if(wk->pMatch==NULL){  //0707
    return;
  }

  org_status = _WifiMyStatusGet( wk, wk->pMatch );
  org_mode = _WifiMyGameModeGet(wk, wk->pMatch);

  if((org_status != status) || (org_mode != gamemode)){
    _commStateChange(wk,status,gamemode);

    WIFI_STATUS_SetWifiStatus(wk->pMatch,status);
    WIFI_STATUS_SetGameMode(wk->pMatch,gamemode);
    if(gamemode == WIFI_GAME_TVT && status == WIFI_STATUS_PLAYING){
    }
    else if(status == WIFI_STATUS_PLAYING ){
      // ボリュームを落す
      if(gamemode != WIFI_GAME_UNIONMATCH){
     //   _changeBGMVol( wk, _VOL_DEFAULT/3 );
      }
    }
    else{
      //  ボリュームを戻す
      _changeBGMVol( wk, _VOL_DEFAULT );
    }
  }
  _userDataInfoDisp(wk);
}

//------------------------------------------------------------------
/**
 * $brief   VCHATフラグの切り替え
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
#if 0
static BOOL _myVChatStatusToggle(WIFIP2PMATCH_WORK *wk)
{
  WIFI_STATUS_SetVChatStatus(wk->pMatch, 1 - WIFI_STATUS_GetVChatStatus( wk->pMatch ));
  WIFI_STATUS_SetUseVChat(wk->pMatch, WIFI_STATUS_GetVChatStatus(wk->pMatch));
  _sendMatchStatus(wk);
  return WIFI_STATUS_GetVChatStatus( wk->pMatch );
}
#endif
//------------------------------------------------------------------
/**
 * $brief   VCHATフラグの切り替え
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _myVChatStatusToggleOrg(WIFIP2PMATCH_WORK *wk)
{
  //本物をひっくり返す
  wk->pParentWork->vchatMain = 1 - wk->pParentWork->vchatMain;
  WIFI_STATUS_SetVChatStatus(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetUseVChat(wk->pMatch, wk->pParentWork->vchatMain);
  _sendMatchStatus(wk);
  return WIFI_STATUS_GetVChatStatus( wk->pMatch );
}

//------------------------------------------------------------------
/**
 * $brief   VCHATフラグをオリジナルにもどす  ==.> VCHATステータスをオリジナルに戻す
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _myVChatStatusOrgSet(WIFIP2PMATCH_WORK *wk)
{
  _changeBGMVol( wk, _VOL_DEFAULT );

  WIFI_STATUS_SetVChatStatus(wk->pMatch, wk->pParentWork->vchatMain);
  WIFI_STATUS_SetUseVChat(wk->pMatch, wk->pParentWork->vchatMain);
  _sendMatchStatus(wk);

  return WIFI_STATUS_GetVChatStatus( wk->pMatch );
}


//----------------------------------------------------------------------------
/**
 *  @brief  ユーザーデータ強制表示処理
 *
 *  @param  wk      システムワーク
 *  @param  friendNo  友達番号
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
  MCR_MOVEOBJ* p_obj;

  MCVSys_UserDispOn( wk, friendNo, heapID );
  p_obj = MCRSYS_GetMoveObjWork( wk, friendNo );
  if( p_obj ){
    WIFI_MCR_CursorOn( &wk->matchroom, p_obj );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ユーザーデータ強制表示処理終了
 *
 *  @param  wk      システムワーク
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff( WIFIP2PMATCH_WORK *wk, u32 heapID )
{

  _TrainerOAMFree( wk );
  _TouchResExit(wk);
  _UnderScreenReload( wk );

  MCVSys_UserDispOff( wk );
  MCRSYS_ReloadShadow(wk);
  WIFI_MCR_CursorOff( &wk->matchroom );

  // 再描画
  MCVSys_ReWrite( wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  表示中の人なら強制的に表示終了
 *
 *  @param  wk        システムワーク
 *  @param  target_friend ターゲットの人物のインデックス
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff_Target( WIFIP2PMATCH_WORK *wk, u32 target_friend, u32 heapID )
{
  if( MCVSys_UserDispGetFriend( wk ) == target_friend ){
    WifiP2PMatch_UserDispOff( wk, heapID );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  誰かを表示しているならば強制的に表示終了
 *
 *  @param  wk        システムワーク
 *  @param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff_Any( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  if( wk->view.user_disp != MCV_USERDISP_OFF ){
    WifiP2PMatch_UserDispOff( wk, heapID );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  自分でアクセスしたときのユーザーデータ強制表示処理
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOn_MyAcces( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
  MCVSys_UserDispOn( wk, friendNo, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  自分でアクセスしたときのユーザーデータ強制表示処理終了
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff_MyAcces( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  MCVSys_UserDispOff( wk );

  // 再描画
  MCVSys_ReWrite( wk, HEAPID_WIFIP2PMATCH );
}


//----------------------------------------------------------------------------
/**
 *  @brief  動作しているかチェック
 *
 *  @param  wk  システムワーク
 *
 *  @retval TRUE  動作中
 *  @retval FALSE 非動作中
 */
//-----------------------------------------------------------------------------
static BOOL MCVSys_MoveCheck( const WIFIP2PMATCH_WORK *wk )
{
  if( wk->view.p_bttnman == NULL ){
    return FALSE;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達データビューアー初期化
 *
 *  @param  wk      システムワーク
 *  @param  p_handle  ハンドル
 *  @param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void MCVSys_Init( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
  static const GFL_UI_TP_HITTBL bttndata[ WCR_MAPDATA_1BLOCKOBJNUM+1 ] = {
    { 0,    47,   0,    119 },
    { 48,   95,   0,    119 },
    { 96,   143,  0,    119 },
    { 144,  191,  0,    119 },

    { 0,    47,   128,  255 },
    { 48,   95,   128,  255 },
    { 96,   143,  128,  255 },
    { 144,  191,  128,  255 },
    {GFL_UI_TP_HIT_END,0,0,0},     //終了データ
  };

  GFL_STD_MemFill( &wk->view, 0, sizeof(WIFIP2PMATCH_VIEW) );

  // 表示設定
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );

  // ボタンデータ初期化
  wk->view.p_bttnman = GFL_BMN_Create( bttndata, MCVSys_BttnCallBack, wk, heapID );

  // ワードセット初期化
  wk->view.p_wordset = WORDSET_Create( heapID );

  // グラフィックデータ読込み
  MCVSys_GraphicSet( wk, p_handle, heapID );


  // 更新
  wk->view.bttn_allchg = TRUE;
  MCVSys_BackDraw( wk );
  MCVSys_BttnDraw( wk );
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達データビューアー破棄
 *
 *  @param  wk      システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_Exit( WIFIP2PMATCH_WORK *wk )
{
  if( MCVSys_MoveCheck( wk ) == FALSE ){
    return;
  }

  // グラフィックはき
  MCVSys_GraphicDel( wk );

  // ボタンマネージャ破棄
  GFL_BMN_Delete( wk->view.p_bttnman );
  wk->view.p_bttnman = NULL;

  // ワードセット破棄
  WORDSET_Delete( wk->view.p_wordset );
  wk->view.p_wordset = NULL;

  // BG２、３は非表示にしておく
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達データビューアーアップデート
 *
 *  @param  wk      システムワーク
 *
 *  @retval 押されている友達番号  + 1
 *  @retval 0 押されていない
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_Updata( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  u32 map_param, map_param2;
  u32 oambttn_ret;
  BOOL userd_end;

  // 現在フレーム設定
  map_param = WIFI_MCR_GetPlayerOnMapParam( &wk->matchroom );

  // オブジェクトの位置だと下を見ないとちゃんとしたフレームの位置を取得できない
  map_param2 = WIFI_MCR_GetPlayerOnUnderMapParam( &wk->matchroom );
  if( (map_param2 >= MCR_MAPPM_MAP00) &&
      (map_param2 <= MCR_MAPPM_MAP03) ){
    map_param = map_param2;
  }

  if( (map_param >= MCR_MAPPM_MAP00) &&
      (map_param <= MCR_MAPPM_MAP03) ){
    if( wk->view.frame_no != (map_param - MCR_MAPPM_MAP00) ){
      wk->view.frame_no = (map_param - MCR_MAPPM_MAP00);

      // 背景カラー変更
      MCVSys_BackDraw( wk );

      // ボタンも書き換える
      wk->view.button_on = TRUE;
      wk->view.bttn_allchg = TRUE;  // ボタン強制変更
    }
  }


  // ユーザーデータ表示処理
  if( wk->view.user_disp == MCV_USERDISP_INIT ){
    wk->view.user_disp = MCV_USERDISP_ON;
    MCVSys_UserDispDraw( wk, heapID );
  }



  if( wk->view.user_disp == MCV_USERDISP_OFF ){

    _TrainerOAMFree( wk );
    _TouchResExit(wk);
    _UnderScreenReload( wk );


    // ボタンメイン
    GFL_BMN_Main( wk->view.p_bttnman );

    // ボタンアニメ処理
    MCVSys_BttnAnmMan( wk );

    // 表示メイン
    if( wk->view.button_on == TRUE ){
      MCVSys_BttnDraw( wk );
      wk->view.button_on = FALSE;
    }
  }

  // 歩いたり、タッチパネルに触れていたらユーザー表示をOFFする
  if( (wk->view.user_disp == MCV_USERDISP_ON) ||
      (wk->view.user_disp == MCV_USERDISP_ONEX) ){

    // した画面OAMボタンメイン
    //oambttn_ret = MCVSys_OamBttnMain( wk );

    //  USERD終了チェック
    userd_end = MCVSys_UserDispEndCheck( wk );

    // なにかキーを押すか、「もどる」を押したら終了する
    if( userd_end == TRUE ){
 //     wk->view.bttn_chg_friendNo = wk->view.touch_friendNo; // この友達のボタンを更新してもらう
      wk->view.touch_friendNo = 0;
      wk->view.touch_frame = 0;
      wk->view.button_on = TRUE;
      wk->view.bttn_allchg = TRUE;

      WifiP2PMatch_UserDispOff( wk, heapID );
      wk->view.user_disp = MCV_USERDISP_OFF;
      wk->view.bttn_allchg = TRUE;
      
    }
  }

  return wk->view.touch_friendNo;
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達データビューアーアップデート  ボタンの表示のみ
 *
 *  @param  wk  システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_UpdataBttn( WIFIP2PMATCH_WORK *wk )
{
  if( wk->view.user_disp == MCV_USERDISP_OFF ){

    // 表示メイン
    if( wk->view.button_on == TRUE ){
      MCVSys_BttnDraw( wk );
      wk->view.button_on = FALSE;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ユーザーディスプレイを終了するかチェック
 *
 *  @param  wk      ワーク
 *  @param  oambttn_ret OAMボタンメイン戻り値
 *
 *  @retval TRUE  終了
 *  @retval FALSE しない
 */
//-----------------------------------------------------------------------------
static BOOL MCVSys_UserDispEndCheck( WIFIP2PMATCH_WORK *wk  )
{
  // 強制表示しているので終了しない
  if( (wk->view.user_disp == MCV_USERDISP_ONEX) ){
    return FALSE;
  }

  if(wk->pTouchWork){
    TOUCHBAR_Main(wk->pTouchWork);
    switch( TOUCHBAR_GetTrg(wk->pTouchWork )){
    case TOUCHBAR_ICON_RETURN:
      return TRUE;
    }
  }
  //  移動はcontボタンはトリガー
  if( (GFL_UI_KEY_GetCont() & (PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_KEY_UP|PAD_KEY_DOWN)) ||
      (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_X))
      ){

    //  if( (GFL_UI_KEY_GetCont() & (PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_KEY_UP|PAD_KEY_DOWN)) ){

    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  今詳細表示している人の友達IDXを取得
 *
 *  @param  cp_wk ワーク
 *
 *  @return 友達インデックス  (0なら表示してない)
 */
//-----------------------------------------------------------------------------
static u32  MCVSys_UserDispGetFriend( const WIFIP2PMATCH_WORK* cp_wk )
{
  return cp_wk->view.touch_friendNo;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ユーザーデータ表示
 *
 *  @param  wk      ワーク
 *  @param  friendNo  友達番号
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
  if( MCVSys_MoveCheck(wk) == TRUE ){
    wk->view.touch_friendNo = friendNo;
    wk->view.touch_frame  = 2;
    wk->view.user_disp    = MCV_USERDISP_ONEX;
    //    wk->view.user_dispno = WF_USERDISPTYPE_NRML;
    MCVSys_UserDispDraw( wk, heapID );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ユーザーデータOFF
 *
 *  @param  wk      ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispOff( WIFIP2PMATCH_WORK *wk )
{
  if( MCVSys_MoveCheck(wk) == TRUE ){
    wk->view.touch_friendNo = 0;
    wk->view.touch_frame = 0;
    wk->view.user_disp    = MCV_USERDISP_OFF;
    wk->view.button_on    = TRUE;
    wk->view.bttn_allchg = TRUE;
    //    MCVSys_OamBttnOff( wk );  // ボタンOFF
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ボタンイベントコールバック
 *
 *  @param  bttnid    ボタンID
 *  @param  event   イベント種類
 *  @param  p_work    ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  WIFIP2PMATCH_WORK *wk = p_work;
  u32 friendNo;

  // 友達番号取得
  friendNo = (wk->view.frame_no * WCR_MAPDATA_1BLOCKOBJNUM) + bttnid;
  friendNo ++;  // 自分が０だから

  // 動作中かチェック
  if( wk->view.bttnfriendNo[ friendNo-1 ] != MCV_BTTN_FRIEND_TYPE_IN ){
    // 動作していないので何もしない
    return ;
  }
  // すでにボタン動作中なので反応しない
  if( wk->view.touch_friendNo != 0 ){
    return ;
  }

  switch( event ){
  case GFL_BMN_EVENT_TOUCH:   ///< 触れた瞬間
    wk->view.touch_friendNo = friendNo;
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    break;
  default:
    break;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	ボタンアニメメイン
 *
 *	@param	wk		システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnAnmMan( WIFIP2PMATCH_WORK *wk )
{
  static const u8 BttnAnmFrame[ MCV_BUTTON_FRAME_NUM ] = {
    1, 1,1,1, 2,2,2,2,
    1, 1,1,1, 2,2,2,2,
  };

  // 動作しているかチェック
  if( wk->view.touch_friendNo == 0 ){
    return ;
  }

  // 動作
  wk->view.touch_frame = BttnAnmFrame[wk->view.button_count];
  wk->view.button_count ++;
  wk->view.button_on = TRUE;
  // 終了チェック
  if( wk->view.button_count >= MCV_BUTTON_FRAME_NUM ){
    wk->view.button_count = 0;
    wk->view.touch_frame = 0;
    wk->view.user_disp = MCV_USERDISP_INIT;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックデータ設定
 *
 *  @param  wk      システムワーク
 *  @param  p_handle  アーカイブハンドル
 *  @param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicSet( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
  int i, j;
  int x, y;

  // BG設定
  // パレット転送
  //  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_btm_NCLR,  // 背景用
  //                                  PALTYPE_SUB_BG, MCV_PAL_BACK*32, MCV_PAL_BACK_NUM*32, heapID );
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NCLR, // ボタン用
                                    PALTYPE_SUB_BG, MCV_PAL_BTTN*32, MCV_PAL_BTTN_NUM*32, heapID );

  // キャラクタ転送
  wk->bgchrSubBack = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_wifip2pmatch_wf_match_btm_NCGR,
                                                                  GFL_BG_FRAME0_S, 0, FALSE, heapID );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NCGR,
                                        GFL_BG_FRAME2_S, MCV_CGX_BTTN2, 0, FALSE, heapID );

  // スクリーン読込みor転送
  // 背景はキャラクタ位置がずれているのでスクリーンデータを書き換える
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, NARC_wifip2pmatch_wf_match_btm_NSCR,
                                          GFL_BG_FRAME0_S, 0,  GFL_ARCUTIL_TRANSINFO_GetPos(wk->bgchrSubBack),
                                          0, FALSE, heapID );

  // ボタンスクリーン読込み
  wk->view.p_bttnbuff = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NSCR, FALSE, &wk->view.p_bttnscrn, heapID );
  MCVSys_GraphicScrnCGOfsChange( wk->view.p_bttnscrn, MCV_CGX_BTTN2 );

  // ユーザースクリーン読込み
  for( i=0; i < 1; i++ ){
    wk->view.p_userbuff[i] = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_result00_NSCR+i, FALSE, &wk->view.p_userscrn[i], heapID );
    MCVSys_GraphicScrnCGOfsChange( wk->view.p_userscrn[i], MCV_CGX_BTTN2 );
  }

  // ダミースクリーン読み込み
  //  wk->view.p_useretcbuff = GFL_ARCHDL_UTIL_LoadScreen( p_handle, NARC_wifip2pmatch_wf_match_btm_etc_NSCR, FALSE, &wk->view.p_useretcscrn, heapID );
  //  MCVSys_GraphicScrnCGOfsChange( wk->view.p_useretcscrn, MCV_CGX_BTTN2 );



  // フォントパレット読込み
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                MCV_SYSFONT_PAL*32, 0x20, heapID);
  //  TalkFontPaletteLoad( PALTYPE_SUB_BG, MCV_SYSFONT_PAL*32, heapID );
  //GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , 13*0x20, 16*2, heapID );

  // ビットマップ作成
  for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){
    x = i/4;  // 配置は2*4
    y = i%4;
    //    GFL_BG_BmpWinInit( wk->view.nameWin[i] );
    wk->view.nameWin[i] = GFL_BMPWIN_Create(
      GFL_BG_FRAME3_S,
      MCV_NAMEWIN_DEFX+(MCV_NAMEWIN_OFSX*x),
      MCV_NAMEWIN_DEFY+(MCV_NAMEWIN_OFSY*y),
      MCV_NAMEWIN_SIZX, MCV_NAMEWIN_SIZY,
      MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );
    // 透明にして展開
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.nameWin[i]), 0 );
    GFL_BMPWIN_TransVramCharacter( wk->view.nameWin[i] );

    // 状態面書き込み先
    for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
      //  GFL_BG_BmpWinInit( wk->view.statusWin[i][j] );
      wk->view.statusWin[i][j] = GFL_BMPWIN_Create(
        GFL_BG_FRAME1_S,
        MCV_STATUSWIN_DEFX+(MCV_STATUSWIN_OFSX*x)+(j*MCV_STATUSWIN_VCHATX),
        MCV_STATUSWIN_DEFY+(MCV_STATUSWIN_OFSY*y),
        MCV_STATUSWIN_SIZX, MCV_STATUSWIN_SIZY,
        MCV_SYSFONT_PAL,
        GFL_BMP_CHRAREA_GET_B );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.statusWin[i][j]), 0 );
      GFL_BMPWIN_TransVramCharacter( wk->view.statusWin[i][j] );
    }

  }

  wk->view.userWinStatus = GFL_BMPWIN_Create(
    GFL_BG_FRAME3_S,
    MCV_USERWIN_STA_X, MCV_USERWIN_STA_Y,
    MCV_USERWIN_STA_SIZX, MCV_USERWIN_STA_SIZY,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );
  // 透明にして展開
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.userWinStatus), 0 );
  GFL_BMPWIN_MakeScreen(wk->view.userWinStatus);
  GFL_BMPWIN_TransVramCharacter( wk->view.userWinStatus );

  wk->view.userWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME3_S,
    MCV_USERWIN_X, MCV_USERWIN_Y,
    MCV_USERWIN_SIZX, MCV_USERWIN_SIZY,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B );
  // 透明にして展開
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.userWin), 0 );
  GFL_BMPWIN_MakeScreen(wk->view.userWin);
  GFL_BMPWIN_TransVramCharacter( wk->view.userWin );

  GFL_BG_LoadScreenReq(GFL_BG_FRAME0_S);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME3_S);

}

//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックデータ破棄
 *
 *  @param  wk  システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicDel( WIFIP2PMATCH_WORK *wk )
{
  int i, j;


  // ビットマップ破棄
  for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){
    GFL_BMPWIN_Delete( wk->view.nameWin[i] );
    for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
      GFL_BMPWIN_Delete( wk->view.statusWin[i][j] );
    }
  }
  GFL_BMPWIN_Delete( wk->view.userWinStatus );
  GFL_BMPWIN_Delete( wk->view.userWin );

  // ボタンスクリーン破棄
  GFL_HEAP_FreeMemory( wk->view.p_bttnbuff );

  // ユーザーウインドウ
  for( i=0; i<1; i++ ){
    GFL_HEAP_FreeMemory( wk->view.p_userbuff[i] );
  }

  // ダミースクリーン破棄
  //  GFL_HEAP_FreeMemory( wk->view.p_useretcbuff );
}

//----------------------------------------------------------------------------
/**
 *  @brief  SCRNのキャラクタNoを転送先のアドレス分足す
 *
 *  @param  p_scrn  スクリーンデータ
 *  @param  cgofs キャラクタオフセット（キャラクタ単位）
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicScrnCGOfsChange( NNSG2dScreenData* p_scrn, u8 cgofs )
{
  u16* p_scrndata;
  int i, j;
  int siz_x, siz_y;

  p_scrndata = (u16*)p_scrn->rawData;
  siz_x = p_scrn->screenWidth/8;
  siz_y = p_scrn->screenHeight/8;

  for( i=0; i<siz_y; i++ ){
    for( j=0; j<siz_x; j++ ){
      p_scrndata[ (i*siz_x)+j ] += cgofs;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ボタン設定
 *
 *  @param  wk      システムワーク
 *  @param  friendNo  友達番号
 *  @param  type    設定タイプ
 *
 *  type
 *    MCV_BTTN_FRIEND_TYPE_RES, // 予約
 *    MCV_BTTN_FRIEND_TYPE_IN,  // 登録済み
 *
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnSet( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 type )
{
  GF_ASSERT( friendNo > 0 );
  GF_ASSERT( type > MCV_BTTN_FRIEND_TYPE_NONE );
  GF_ASSERT( type < MCV_BTTN_FRIEND_TYPE_MAX );
  wk->view.bttnfriendNo[ friendNo-1 ] = type;
  wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ボタンからはずす
 *
 *  @param  wk      システムワーク
 *  @param  friendNo  友達番号
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnDel( WIFIP2PMATCH_WORK *wk, u32 friendNo )
{
  GF_ASSERT( friendNo > 0 );
  wk->view.bttnfriendNo[ friendNo-1 ] = MCV_BTTN_FRIEND_TYPE_NONE;
  wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ボタンの表示タイプを取得する
 *
 *  @param  wk      ワーク
 *  @param  friendNo  友達number
 *
 *  @return 表示タイプ
 *  type
 *    MCV_BTTN_FRIEND_TYPE_RES, // 予約
 *    MCV_BTTN_FRIEND_TYPE_IN,  // 登録済み
 *    MCV_BTTN_FRIEND_TYPE_NONE,  // なし
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_BttnTypeGet( const WIFIP2PMATCH_WORK *wk, u32 friendNo )
{
  GF_ASSERT( friendNo > 0 );
  return wk->view.bttnfriendNo[ friendNo-1 ];
}

//----------------------------------------------------------------------------
/**
 *  @brief  強制書き直し
 *
 *  @param  wk      システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_ReWrite( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  if( wk->view.user_disp != MCV_USERDISP_OFF ){
    MCVSys_UserDispDraw( wk, heapID );
  }else{
    wk->view.bttn_allchg = TRUE;
    MCVSys_BttnDraw( wk );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ボタンをすべて書くリクエストを出す
 *
 *  @param  wk
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnAllWriteReq( WIFIP2PMATCH_WORK *wk )
{
  wk->view.bttn_allchg = TRUE;
  wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  背景描画
 *
 *  @param  wk  システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_BackDraw( WIFIP2PMATCH_WORK *wk )
{
  // 背景のカラーを変える
  //  GFL_BG_ChangeScreenPalette(   GFL_BG_FRAME0_S, 0, 0,
  //                              32, 24, wk->view.frame_no+MCV_PAL_FRMNO );

  //  GFL_BG_LoadScreenV_Req(   GFL_BG_FRAME0_S );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ボタン表示
 *
 *  @param  wk  システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnDraw( WIFIP2PMATCH_WORK *wk )
{
  int i, j;
  int sex;
  int bttn_type;
  int x, y;
  int friend_no;
  int frame;
  BOOL write_change_masterflag; // 強制的にすべてのボタンを書き換える
  BOOL write_change_localflag;  // ここのボタンごとの書き換えチェック

  // 全書き換えチェック
  if( wk->view.bttn_allchg == TRUE ){
    wk->view.bttn_allchg = FALSE;
    write_change_masterflag = TRUE;

    // スクリーンクリア
    //    GFL_BG_ScrFill( GFL_BG_FRAME2_S, 0, 0, 0, 32, 24, 0 );
    GFL_BG_ClearFrame( GFL_BG_FRAME3_S);
   // GFL_BG_ClearScreenCodeVReq(GFL_BG_FRAME2_S);
/*    {
      ARCHANDLE* p_handle;
      p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, HEAPID_WIFIP2PMATCH );
      GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, NARC_wifip2pmatch_wf_match_btm_NSCR,
                                              GFL_BG_FRAME0_S, 0,  GFL_ARCUTIL_TRANSINFO_GetPos(wk->bgchrSubBack),
                                              0, FALSE, HEAPID_WIFIP2PMATCH );
      GFL_ARC_CloseDataHandle(p_handle);
    }*/

  }else{
    write_change_masterflag = FALSE;
  }

  for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){

    friend_no = (wk->view.frame_no*WCR_MAPDATA_1BLOCKOBJNUM) + i + 1;
    // 表示タイプ設定
    if( wk->view.bttnfriendNo[friend_no - 1] != MCV_BTTN_FRIEND_TYPE_NONE ){
      sex = WifiList_GetFriendInfo( wk->pList, friend_no - 1, WIFILIST_FRIEND_SEX );
      if(sex == PM_MALE){
        bttn_type = MCV_BUTTON_TYPE_MAN;
      }else{
        bttn_type = MCV_BUTTON_TYPE_GIRL;
      }
    }else{
      bttn_type = MCV_BUTTON_TYPE_NONE;
    }

    // ボタン表示座標
    x = i/4;
    y = i%4;

    // 押されているボタンかチェック
    if( friend_no == wk->view.touch_friendNo ){
      frame = wk->view.touch_frame;
    }else{
      frame = 0;
    }


    // ボタンの形が変わっているかチェック
    if( (friend_no == wk->view.touch_friendNo) ||
        (friend_no == wk->view.bttn_chg_friendNo) ){
      write_change_localflag = TRUE;
    }else{
      write_change_localflag = FALSE;
    }

    // 書き換えるかチェック
    if( (write_change_masterflag == TRUE) ||
        (write_change_localflag == TRUE) ){

      // ボタン
      MCVSys_BttnWrite( wk,
                        MCV_BUTTON_DEFX+(MCV_BUTTON_OFSX*x), MCV_BUTTON_DEFY+(MCV_BUTTON_OFSY*y),
                        bttn_type, frame );

      // 名前表示
      if( bttn_type != MCV_BUTTON_TYPE_NONE ){

        // 名前の表示
        MCVSys_BttnWinDraw( wk, wk->view.nameWin[i], friend_no, frame, i );
        GFL_BMPWIN_MakeScreen(wk->view.nameWin[i]);

        MCVSys_BttnStatusWinDraw( wk, wk->view.statusWin[i], friend_no, frame,
                                  MCV_BUTTON_DEFX+(MCV_BUTTON_OFSX*x), MCV_BUTTON_DEFY+(MCV_BUTTON_OFSY*y) );
      }else{

        // 透明にする
        GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.nameWin[i]), 0 );
        GFL_BMPWIN_TransVramCharacter( wk->view.nameWin[i] );
        for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
          GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.statusWin[i][j]), 0 );
          GFL_BMPWIN_TransVramCharacter( wk->view.statusWin[i][j] );
        }
      }
    }
  }

  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);
  GFL_BG_LoadScreenV_Req(  GFL_BG_FRAME2_S );

  // メッセージ面の表示設定
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
}


//----------------------------------------------------------------------------
/**
 *  @brief  ファクトリーデータ表示
 *
 *  @param  wk      ワーク
 *  @param  strid   文字ID
 *  @param  factoryid ファクトリーデータID
 *  @param  friendno  友達番号
 *  @param  x     表示X
 *  @param  y     表示Y
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispFrontiorNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 factoryid, u32 friendno, u32 x, u32 y )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  フロンティア　タイトルメッセージ取得
 *
 *  @param  wk      ワーク
 *  @param  p_str   文字列格納先
 *  @param  factoryid ファクトリー　セーブデータID
 *  @param  friendno  友達ナンバー
 *
 *  factoryid
 *    MCV_FRONTIOR_TOWOR,
 *    MCV_FRONTIOR_FACTORY,
 *    MCV_FRONTIOR_FACTORY100,
 *    MCV_FRONTIOR_CASTLE,
 *    MCV_FRONTIOR_STAGE,
 *    MCV_FRONTIOR_ROULETTE,
 *
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispFrontiorTitleStrGet( WIFIP2PMATCH_WORK *wk, STRBUF* p_str, u32 factoryid, u32 friendno )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  数字を表示する処理
 *
 *  @param  wk    ワーク
 *  @param  strid メッセージID
 *  @param  num   数字
 *  @param  x   ｘドット座標
 *  @param  y   ｙドット座標
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 num, u32 x, u32 y )
{
  WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
  GFL_MSG_GetString(  wk->MsgManager, strid, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin),
                  x,  y,
                  wk->TitleString, wk->fontHandle);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ボタン描画
 *
 *  @param  wk    システムワーク
 *  @param  cx    ｘキャラ座標
 *  @param  cy    ｙキャラ座標
 *  @param  type  ボタンタイプ
 *  @param  frame フレーム
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnWrite( WIFIP2PMATCH_WORK *wk, u8 cx, u8 cy, u8 type, u8 frame )
{
  u16 reed_x, reed_y;
  u16* p_buff;


#if 1

  if(type == MCV_BUTTON_TYPE_NONE){
    reed_x = MCV_BUTTON_SIZX * 0;
    reed_y = MCV_BUTTON_SIZY * 3;
  }
  else{
    reed_x = MCV_BUTTON_SIZX * type;
    reed_y = MCV_BUTTON_SIZY * frame;
  }

  GFL_BG_WriteScreenExpand( GFL_BG_FRAME2_S,
                            cx, cy, MCV_BUTTON_SIZX, MCV_BUTTON_SIZY,
                            wk->view.p_bttnscrn->rawData,
                            reed_x, reed_y,
                            32, 24 );


#else
  // frame 3のときは1を表示する
  if( frame == 3 ){
    frame = 1;
  }

  if( (frame < 2) ){
    reed_x = MCV_BUTTON_SIZX * type;
    reed_y = MCV_BUTTON_SIZY * frame;

    GFL_BG_WriteScreenExpand( GFL_BG_FRAME2_S,
                              cx, cy, MCV_BUTTON_SIZX, MCV_BUTTON_SIZY,
                              wk->view.p_bttnscrn->rawData,
                              reed_x, reed_y,
                              32, 18 );
  }else{
    reed_x = 0;
    reed_y = MCV_BUTTON_SIZY * type;
    p_buff = (u16*)wk->view.p_bttnscrn->rawData;

    GFL_BG_WriteScreenExpand( GFL_BG_FRAME2_S,
                              cx, cy, MCV_BUTTON_SIZX, MCV_BUTTON_SIZY,
                              &p_buff[ 32*18 ],
                              reed_x, reed_y,
                              16, 18 );
  }

  // パレットカラーを合わせる
  //  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S, cx, cy,
  //                            MCV_BUTTON_SIZX, MCV_BUTTON_SIZY, type+MCV_PAL_BTTN );
#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief  ボタン上のトレーナー名の表示
 *
 *  @param  wk      システムワーク
 *  @param  p_bmp   ビットマップ
 *  @param  friendNo  友達ナンバー
 *  @param  frame   ボタンフレーム
 *  @param  area_id   配置ID
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN* p_bmp, u32 friendNo, u32 frame, u32 area_id )
{
  int sex;
  int col;
  int y;

  // 画面クリーン
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_bmp), 0 );

  //y座標を取得
  y = 8;//ViewButtonFrame_y[ frame ];

  _COL_N_WHITE;
  MCVSys_FriendNameSet(wk, friendNo-1);
  GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_033, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_bmp),
                  2,  y,
                  wk->TitleString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter( p_bmp );
}

//----------------------------------------------------------------------------
/**
 *  @brief  状態ウィンドウの描画
 *
 *  @param  wk      しすてむワーク
 *  @param  p_stbmp   状態ビットマップ
 *  @param  friendNo  友達番号
 *  @param  frame   ボタンフレーム
 *  @param  area_id   配置ID
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnStatusWinDraw( WIFIP2PMATCH_WORK *wk, GFL_BMPWIN** p_stbmp, u32 friendNo, u32 frame, u32 x, u32 y )
{
  int i;
  int vct_icon;
  WIFI_STATUS* p_status;
  u32 status,gamemode,mode;

  if(frame==0){
    mode = PLAYER_DISP_ICON_NAMEMODE;
  }
  else{
    mode = PLAYER_DISP_ICON_TOUCHMODE;
  }
  p_status = WifiFriendMatchStatusGet( friendNo - 1 );
  status = _WifiMyStatusGet( wk, p_status );
  gamemode = _WifiMyGameModeGet( wk, p_status );

  WifiP2PMatchFriendListStIconWrite( &wk->icon, GFL_BG_FRAME2_S,
                                     x+2, y+2,
                                     status,gamemode, mode);

  if( WIFI_STATUS_GetUseVChat(p_status) ){
    vct_icon = PLAYER_DISP_ICON_IDX_NONE;
  }else{
    vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
  }
  WifiP2PMatchFriendListIconWrite(  &wk->icon, GFL_BG_FRAME2_S,
                                    x+12, y+2,
                                    vct_icon+mode, 0 );

  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);

}


//----------------------------------------------------------------------------
/**
 *  @brief  Vierのほうのワードセットに人の名前を設定
 *
 *  @param  p_wk      ワーク
 *  @param  friendno    友達ナンバー
 */
//-----------------------------------------------------------------------------
static void MCVSys_FriendNameSet( WIFIP2PMATCH_WORK* p_wk, int friendno )
{
  if(friendno != -1){
    int sex = WifiList_GetFriendInfo(p_wk->pList, friendno, WIFILIST_FRIEND_SEX);
    WifiList_GetFriendName(p_wk->pList, friendno, p_wk->pExpStrBuf);
    WORDSET_RegisterWord( p_wk->view.p_wordset, 0, p_wk->pExpStrBuf, sex, TRUE, PM_LANG);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達マッチデータのステータス取得
 *
 *  @param  wk      システムワーク
 *  @param  idx     インデックス
 */
//-----------------------------------------------------------------------------
static WIFI_STATUS* WifiFriendMatchStatusGet( u32 idx )
{
  GF_ASSERT( idx < WIFIP2PMATCH_MEMBER_MAX );

  return (WIFI_STATUS*)GFL_NET_DWC_GetFriendInfo( idx );
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達の状態を取得する
 *
 *  @param  idx   インデックス
 */
//-----------------------------------------------------------------------------

static u8 WifiDwc_getFriendStatus( int idx )
{
  return GFL_NET_DWC_getFriendStatus(idx);
}

//----------------------------------------------------------------------------
/**
 *  @brief  P2P通信ゲームを開始する。
 *
 *  @param  friendno  ターゲット番号  （-1なら親）
 *  @param  status    ステータス
 *
 *  @return
 */
//-----------------------------------------------------------------------------
static BOOL WifiP2PMatch_CommWifiBattleStart( WIFIP2PMATCH_WORK* wk, int friendno )
{
  // 接続する前に４人募集で送られてくる可能性のある
  // コマンドを設定する
  CommCommandWFP2PMF_MatchStartInitialize(wk);

  return GFL_NET_StateStartWifiPeerMatch( friendno );
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達募集中　設定ON
 *
 *  @param  wk  ワーク
 */
//-----------------------------------------------------------------------------
static void FriendRequestWaitOn( WIFIP2PMATCH_WORK* wk, BOOL msg_on )
{
  if( wk->friend_request_wait == FALSE ){
    // メッセージをだして、プレイヤー動作を停止する
    wk->friend_request_wait = TRUE;
    WIFI_MCR_PlayerMovePause( &wk->matchroom, TRUE );

    if( msg_on == TRUE ){
      WifiP2PMatchMessagePrintDirect( wk, msg_wifilobby_142, FALSE );
  //    WifiP2PMatchMessagePrint( wk, msg_wifilobby_142, FALSE );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達募集中　設定OFF
 *
 *  @param  wk  ワーク
 */
//-----------------------------------------------------------------------------
static void FriendRequestWaitOff( WIFIP2PMATCH_WORK* wk )
{


  if( wk->friend_request_wait == TRUE ){
    // メッセージを消して、プレイヤー動作を開始する
    wk->friend_request_wait = FALSE;
    EndMessageWindowOff( wk );
    WIFI_MCR_PlayerMovePause( &wk->matchroom, FALSE );
    wk->state = WIFIP2PMATCH_STATE_NONE;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  友達募集中フラグ取得
 *
 *  @param  cp_wk ワーク
 *
 *  @retval TRUE  募集中
 *  @retval FALSE ぼしゅうしていない
 */
//-----------------------------------------------------------------------------
static BOOL FriendRequestWaitFlagGet( const WIFIP2PMATCH_WORK* cp_wk )
{
  return cp_wk->friend_request_wait;
}



static BOOL _funcBGMVol(WIFIP2PMATCH_WORK* wk )
{

  //  wk->aVol.bgmVolStart = wk->aVol.bgmVol;
  //  wk->aVol.bgmVolCount = _VOL_TRACK_FRAME;
  //  wk->aVol.bgmVolEnd = endVol;
  int volnum;

  if(wk->aVol.bgmVolCount!=0){
    wk->aVol.bgmVolCount--;
    if(wk->aVol.bgmVolCount!=0){
      int sub = wk->aVol.bgmVolEnd;
      sub = sub - wk->aVol.bgmVolStart;
      if(sub < 0){
        sub = - sub;
        sub = (sub / _VOL_TRACK_FRAME) * wk->aVol.bgmVolCount;
        wk->aVol.bgmVol = wk->aVol.bgmVolEnd + sub;
      }
      else{
        sub = (sub / _VOL_TRACK_FRAME) * wk->aVol.bgmVolCount;
        wk->aVol.bgmVol = wk->aVol.bgmVolEnd - sub;
      }
      //OS_TPrintf("VOL変更 %d %d \n",wk->aVol.bgmVol, sub);
      PMSND_ChangeBGMVolume( _VOL_TRACK_ALL, wk->aVol.bgmVol );
      return FALSE;
    }
    else{
      //OS_TPrintf("VOLおわり %d\n",wk->aVol.bgmVolEnd);
      wk->aVol.bgmVol = wk->aVol.bgmVolEnd;
      PMSND_ChangeBGMVolume( _VOL_TRACK_ALL, wk->aVol.bgmVolEnd );
    }
  }
  return TRUE;
}


static void _changeBGMVol( WIFIP2PMATCH_WORK* wk, u8 endVol)
{

  if(wk->aVol.bgmVolEnd == endVol){
    //OS_TPrintf("ボリュームそのまま %d %d\n",endVol);
    return;
  }
  //OS_TPrintf("ボリューム変更 %d %d \n",wk->aVol.bgmVolEnd, endVol);
  wk->aVol.bgmVolStart = wk->aVol.bgmVol;
  wk->aVol.bgmVolCount = _VOL_TRACK_FRAME;
  wk->aVol.bgmVolEnd = endVol;
  //  PMSND_SetMaxVolume( endVol );

  PMSND_AllPlayerVolumeSet( endVol, PMSND_MASKPL_ALL);
  PMV_SetMasterVolume(endVol);

}


static void _initBGMVol( WIFIP2PMATCH_WORK* wk, int status)
{

  if(status == WIFI_STATUS_PLAYING){
    wk->aVol.bgmVolEnd = _VOL_DEFAULT/3;
  }
  else{
    wk->aVol.bgmVolEnd = _VOL_DEFAULT;
  }
  wk->aVol.bgmVolStart = wk->aVol.bgmVolEnd;
  //OS_TPrintf("ボリューム初期化 %d\n",wk->aVol.bgmVolEnd);
}

//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：初期化
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT WifiP2PMatchProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFIP2PMATCH_WORK * wk = mywk;
  WIFIP2PMATCH_PROC_PARAM* pParentWork = pwk;
  u32 result;


  GFL_HEAP_CreateHeapLo( GFL_HEAPID_APP, HEAPID_WIFIP2PMATCH, 0xa4000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(WIFIP2PMATCH_WORK), HEAPID_WIFIP2PMATCH );
  MI_CpuFill8( wk, 0, sizeof(WIFIP2PMATCH_WORK) );
  GFL_NET_ChangeWork(wk);

  // Vram転送マネージャ作成
#if WB_FIX
  initVramTransferManagerHeap( VRANTRANSFERMAN_NUM, HEAPID_WIFIP2PMATCH );
#endif
#if PM_DEBUG
  pDebugWork = wk;
#endif
  
  //        wk->MsgIndex = _PRINTTASK_MAX;
  wk->pMatch = pParentWork->pMatch;

  wk->pSaveData = pParentWork->pSaveData;
  wk->pGameData = pParentWork->pGameData;

  wk->bb_party = _BBox_PokePartyAlloc(wk->pGameData);
  
  wk->pMyPoke = GAMEDATA_GetMyPokemon(wk->pGameData);
  wk->pList = GAMEDATA_GetWiFiList(wk->pGameData);
  wk->pConfig = SaveData_GetConfig(pParentWork->pSaveData);
  wk->initSeq = pParentWork->seq;    // P2PかDPWか

  wk->state = pParentWork->bTalk;


  if(pParentWork->seq == WIFI_GAME_ERROR){
    pParentWork->seq = WIFI_GAME_NONE;
    _initBGMVol( wk, WIFI_STATUS_PLAYING);
  }
  else if(pParentWork->seq != WIFI_GAME_NONE){
    _initBGMVol( wk, WIFI_STATUS_PLAYING);
  }
  else
  {        // 接続がまだ
    _initBGMVol( wk, WIFI_STATUS_WAIT);
  }

  wk->endSeq = WIFI_GAME_NONE;
  wk->preConnect = -1;
  wk->pParentWork = pParentWork;

  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );

  // ワーク初期化
  InitMessageWork( wk );
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_INIT);

  // グラフィック初期化
  _graphicInit(wk);

  if(pParentWork->seq == WIFI_GAME_NONE){
    _makeMyMatchStatus(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
  }

  if(GFL_NET_IsInit()){
    CommCommandWFP2PMF_MatchStartInitialize(wk);
    GFL_NET_ReloadIconTopOrBottom(TRUE, HEAPID_WIFIP2PMATCH);
  }

  return GFL_PROC_RES_FINISH;
}




//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：メイン
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------

static GFL_PROC_RESULT WifiP2PMatchProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFIP2PMATCH_WORK * wk  = mywk;

  if(GFL_NET_IsInit()){
    switch(GFL_NET_DWC_ERROR_ReqErrorDisp(TRUE,TRUE)){
    case GFL_NET_DWC_ERROR_RESULT_TIMEOUT:
      WIFIP2PMatch_pokePartyMenuDelete(wk);
      BmpWinDelete(wk);
      GFL_NET_SetAutoErrorCheck(FALSE);
      GFL_NET_SetNoChildErrorCheck(FALSE);
//      GFL_NET_StateWifiMatchEnd(TRUE);  // マッチングを切る
      _myStatusChange(wk, WIFI_STATUS_WAIT, WIFI_GAME_LOGIN_WAIT);
      wk->timer = _RECONECTING_WAIT_TIME;
      _CHANGESTATE(wk, WIFIP2PMATCH_RECONECTING_WAIT);
      return GFL_PROC_RES_CONTINUE;
    case GFL_NET_DWC_ERROR_RESULT_RETURN_PROC:
      WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
      WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
      wk->endSeq = WIFI_GAME_ERROR;
      return GFL_PROC_RES_FINISH;
    }
  }

  if( NetErr_App_CheckError() == NET_ERR_CHECK_NONE ){
    if(FuncTable[wk->seq]!=NULL){
      *seq = (*FuncTable[wk->seq])( wk, *seq );
      if(*seq == SEQ_OUT){
        WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
                        WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , HEAPID_WIFIP2PMATCH );
        return GFL_PROC_RES_FINISH;
      }
    }
  }
  if(wk->clactSet){
    GFL_CLACT_SYS_Main();
  }

//#ifdef DEBUG_ONLY_FOR_toru_nagihashi
#if 0
  {
    static const char *sc_print_tbl[] =
    {
      "なし",
      "マシーン募集",
      "募集に参加",
      "はなしかけた",
      "られた",
    };
    OS_TPrintf( "状態:%s\n", sc_print_tbl[ wk->state ] );
  }
#endif

  if(wk->SysMsgQue){
    u8 defL, defS, defB;
    GFL_FONTSYS_GetColor( &defL, &defS, &defB );
    GFL_FONTSYS_SetColor(FBMP_COL_BLACK, FBMP_COL_BLK_SDW, 15);
    GFL_TCBL_Main( wk->pMsgTcblSys );
    GFL_FONTSYS_SetColor( defL, defS, defB );
    PRINTSYS_QUE_Main(wk->SysMsgQue);
  }
  if(wk->SysMenuQue){
    PRINTSYS_QUE_Main(wk->SysMenuQue);
  }
  _funcBGMVol(wk);

  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：終了
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT WifiP2PMatchProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFIP2PMATCH_WORK  *wk    = mywk;
  WIFIP2PMATCH_PROC_PARAM* pParentWork = pwk;


  if(!_funcBGMVol(wk)){
    return GFL_PROC_RES_CONTINUE;
  }
  if( !WIPE_SYS_EndCheck() ){
    return GFL_PROC_RES_CONTINUE;
  }
  if(GFL_NET_IsInit()){
    CommCommandWFP2PMF_MatchStartFinalize(wk);
  }
  pParentWork->bTalk =  wk->state;

  if(wk->pRegulation){
    Regulation_Copy(wk->pRegulation, pParentWork->pRegulation);
    GFL_HEAP_FreeMemory(wk->pRegulation);
    wk->pRegulation=NULL;
  }

  WIFIP2PMatch_pokePartyMenuDelete(wk);
  _graphicEnd(wk);

  pParentWork->friendNo = wk->friendNo;
  //OS_TPrintf("ともだちのばんごうは%d\n", wk->friendNo);
  pParentWork->seq = wk->endSeq;

  if(GFL_NET_IsInit()){
    pParentWork->targetID = GFL_NET_DWC_GetFriendIndex();
  }

  _BBox_PokePartyFree(wk->bb_party);

  
  // ワーク解放
  FreeMessageWork( wk );

  GFL_PROC_FreeWork( proc );        // GFL_PROCワーク開放


#if WB_TEMP_FIX
  // VramTransferマネージャ破棄
  DellVramTransferManager();
#endif

  GFL_HEAP_DeleteHeap( HEAPID_WIFIP2PMATCH );


  return GFL_PROC_RES_FINISH;
}



// プロセス定義データ
const GFL_PROC_DATA WifiP2PMatchProcData = {
  WifiP2PMatchProc_Init,
  WifiP2PMatchProc_Main,
  WifiP2PMatchProc_End,
};

