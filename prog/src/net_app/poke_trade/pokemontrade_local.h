//=============================================================================
/**
 * @file	  pokemontrade_local.h
 * @brief	  ポケモン交換 ローカル共有定義
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/08/28
 */
//=============================================================================

#pragma once

#if PM_DEBUG
#define _TRADE_DEBUG (1)
#else
#define _TRADE_DEBUG (0)
#endif

#include "../../field/event_ircbattle.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"

#include "pokemontrade_snd.h"
#include "sound/pm_sndsys.h"
#include "net/network_define.h"

#include "print/wordset.h"

#include "savedata/box_savedata.h"  //デバッグアイテム生成用

#include "savedata/mail_util.h"
#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "system/time_icon.h"
#include "app/app_taskmenu.h"
#include "pokemontrade_anim.h"
#include "system/ica_anime.h"
#include "ui/ui_easy_clwk.h"
//タッチバー
#include "ui/touchbar.h"
#include "savedata/mail_util.h"
#include "progval.h"

#include "app/app_printsys_common.h"
#include "net/net_save.h"
#include "net/nhttp_rap.h"
#include "pm_define.h"
#include "savedata/wifihistory.h"
#include "app/app_keycursor.h"


#define FIRST_NATUKIDO  (70)		///交換されたポケモンに入れるなつき度

///3Dモデルのタイプ
typedef enum
{
  REEL_PANEL_OBJECT,
  TRADE01_OBJECT,
  TRADEIR_OBJECT,
  TRADEUP_OBJECT,
  TRADEMIDDLE_OBJECT,
  TRADEDOWN_OBJECT,
  OBJECT_MODEL_MAX,
} D3_OBJECT_MODEL;


typedef enum
{
  SETUP_TRADE_BG_MODE_NORMAL,
  SETUP_TRADE_BG_MODE_DEMO,
} SETUP_TRADE_BG_MODE;


typedef enum
{
  _DEMO_TYPE_ALL,
  _DEMO_TYPE_UP,
  _DEMO_TYPE_MIDDLE,
  _DEMO_TYPE_DOWN,
}_DEMO_TYPE_;



typedef enum
{
  _CHANGERAND = 43,    ///< ポケモン交換タイミング揺らぎ数
  _TIMING_ENDNO=12,
  _TIMING_TRADEDEMO_START,
  _TIMING_SAVEST,
  _TIMING_SAVELAST,
  _TIMING_SAVEEND,
  _TIMING_ANIMEEND,
  _TIMING_POKECOLOR,
  _TIMING_RETURN,     //あいてが交換したがっていて、相手がやめたい時は同期をとって元のシーケンスに戻る
  _TIMING_RETURN2,
  _TIMING_EVIL,
  _TIMING_FIRSTBOTH,
  _TIMING_UNDATA,
  _TIMING_SHINKA_AFTER,

  POKETRADE_FACTOR_TIMING_A,   ///->初期状態POKETRADE_FACTOR_NONEにするタイミング
  POKETRADE_FACTOR_TIMING_B,   ///->POKETRADE_FACTOR_SINGLECHANGE かPOKETRADE_FACTOR_TRI_SELECTか POKETRADE_FACTOR_ENDのタイミング
  POKETRADE_FACTOR_TIMING_C,   //  POKETRADE_FACTOR_SINGLE_OK  かPOKETRADE_FACTOR_SINGLE_NGか  POKETRADE_FACTOR_EGG
  POKETRADE_FACTOR_TIMING_D,  ///->POKETRADE_FACTOR_TRI_DECIDEかPOKETRADE_FACTOR_TRI_ENDのタイミング
  POKETRADE_FACTOR_TIMING_E,  ///->POKETRADE_FACTOR_TRI_LASTかPOKETRADE_FACTOR_TRI_BACKのタイミング
  POKETRADE_FACTOR_TIMING_F,  ///->POKETRADE_FACTOR_TRI_DECIDEからPOKETRADE_FACTOR_TRI_LASTへのまち
  POKETRADE_FACTOR_TIMING_G,  ///不正検査
} NET_TIMING_ENUM;


typedef enum
{
  POKETRADE_FACTOR_NONE,            //まだ選んで無い
  POKETRADE_FACTOR_SINGLECHANGE,    //一体選択みせあい
  POKETRADE_FACTOR_SINGLE_OK,    //一体選択OK
  POKETRADE_FACTOR_SINGLE_NG,    //一体選択NG
  POKETRADE_FACTOR_END,            //終わり選択
  POKETRADE_FACTOR_EGG,        // たまごと最後の手持ち交換
  POKETRADE_FACTOR_MAIL,      //メールボックスがいっぱい
  POKETRADE_FACTOR_ILLEGAL,  // 不正ポケモン
  POKETRADE_FACTOR_ILLEGAL_FRIEND,  // 相手が不正ポケモン
  POKETRADE_FACTOR_WAZA,      //技がまずい
  POKETRADE_FACTOR_TRI_SELECT,   //  ３体選択にすすむ

  POKETRADE_FACTOR_TRI_DECIDE,   //  ３たい交換する
  POKETRADE_FACTOR_TRI_END,     //  三対選択後やめた

  POKETRADE_FACTOR_TRI_LAST,   //こうかん
  POKETRADE_FACTOR_TRI_BACK,   //もう一度3体選択に

}POKETRADE_FACTOR_ENUM;





#define BOX_VERTICAL_NUM (5)
#define BOX_HORIZONTAL_NUM (6)

#define HAND_VERTICAL_NUM (3)
#define HAND_HORIZONTAL_NUM (2)


//#define BOX_MONS_NUM (30)


#define _BRIGHTNESS_SYNC (2)  // フェードのＳＹＮＣは要調整



//パレットの定義はあっても良いと
#define _FONT_PARAM_LETTER_BLUE (0x5)
#define _FONT_PARAM_SHADOW_BLUE (0x6)
#define _FONT_PARAM_LETTER_RED (0x3)
#define _FONT_PARAM_SHADOW_RED (0x4)
#define	FBMP_COL_WHITE		(15)


#define PLTID_OBJ_TYPEICON_M (8) // //3本
#define PLTID_OBJ_BALLICON_M (12)  // 1本使用
#define PLTID_OBJ_POKESTATE_M (13) //
#define PLTID_OBJ_POKERUS_M (0) //
#define PLTID_OBJ_DEMO_M (7)
#define PLTID_OBJ_POKEITEM_M (6)
#define PLTID_OBJ_BALLICON_M_L (5)  // 1本使用
#define _OBJPLT_GTS_POKEICON_OFFSET_M (0)


//#define _OBJPLT_POKEICON_GRAY_OFFSET (0) ///< グレー用ポケモンアイコンパレット
//#define _OBJPLT_POKEICON_GRAY  (3)  //ポケモンアイコングレイ 3本

//下は通信アイコン出さないからもう一本可能
#define _TOUCHBAR_OBJ_PALPOS  (0)  //タッチバーのパレット
#define _OBJPLT_COMMON_OFFSET (0)   //アプリケーションメニュ
#define _OBJPLT_COMMON  (TOUCHBAR_OBJ_PLT_NUM)  //サブ画面OBJパレット下画面バー 3本

#define PLTID_OBJ_POKERUS_S (0xb) //
#define PLTID_OBJ_POKEITEM_S (0xc) //
#define PLTID_OBJ_BALLICON_S (0xd)  // 1本使用
#define PLTID_OBJ_POKERUS_FACE_S (0xe) //顔マーク

#define _OBJPLT_POKEICON_OFFSET (3*0x20)
#define _OBJPLT_POKEICON  (3)  //ポケモンアイコン 3本
#define _OBJPLT_BOX_OFFSET (_OBJPLT_POKEICON_OFFSET+_OBJPLT_POKEICON*32)
#define _OBJPLT_BOX  (6)  //サブ画面OBJパレット基本  5本
#define _PALETTE_OAMMSG (0x0e) //メッセージOAM用


//PLTID_OBJ_POKEITEM_S

//--- TOUCH2
//--- POKE5
//------  SCROLLb



#define _POKEMON_MAIN_FRIENDGIVEMSG_PAL (5)  //メイン画面、説明文のパレット
#define _POKEMON_MAIN_FRIENDGIVEMSG_PAL_RED (12)  //メイン画面、説明文のパレット
#define _POKEMON_MAIN_FRIENDGIVEMSG_PAL_BLUE (14)  //メイン画面、説明文のパレット

#define _TRADE_BG_PALLETE_NUM (6)  //交換BGのパレットの本数   0-4
#define _TOUCHBAR_BG_PALPOS (7)     //タッチバーの色     TOUCHBAR_BG_PLT_NUM （１）本

#define _SUBLIST_NORMAL_PAL   (9)   //サブメニューの通常パレット ef0
#define _STATUS_MSG_PAL   (11)  // メッセージフォント

#define _MAINBG_APP_MSG_PAL (13)
//メッセージ系は上下共通 BG
#define _BUTTON_MSG_PAL   (14)  // メッセージフォント
#define _BUTTON_WIN_PAL   (15)  // ウインドウ


//#define PLIST_RENDER_MAIN (1)


#define _POKE_PRJORTH_Y_COEFFICIENT (12)
#define _POKE_PRJORTH_X_COEFFICIENT (16)


#define _POKE_PRJORTH_TOP   (FX32_ONE*_POKE_PRJORTH_Y_COEFFICIENT)    ///< PRJORTH	→top	  :nearクリップ面上辺のY座標
#define _POKE_PRJORTH_RIGHT (FX32_ONE*_POKE_PRJORTH_X_COEFFICIENT)  ///< PRJORTH	→right	  :nearクリップ面右辺のX座標

#define _MCSS_POS_X(x) (x * FX32_ONE)
#define _MCSS_POS_Y(y) (y * FX32_ONE)
#define _MCSS_POS_Z(z) (z * FX32_ONE)

#define PSTATUS_MCSS_POS_X1 _MCSS_POS_X(-55)            //自分最初の位置X
#define PSTATUS_MCSS_POS_X2 _MCSS_POS_X(55)             //相手最初の位置X
#define PSTATUS_MCSS_POS_Y  _MCSS_POS_Y(-28)            //最初の位置Yは共通



#define PSTATUS_MCSS_POS_MYZ   (0)
#define PSTATUS_MCSS_POS_YOUZ  (0)



#define YESNO_CHARA_OFFSET	(21 * 20 )
#define YESNO_CHARA_W		( 8 )
#define YESNO_CHARA_H		( 4 )

#define _BUTTON_WIN_WIDTH (22)    // ウインドウ幅
#define _BUTTON_WIN_HEIGHT (5)    // ウインドウ高さ


#define CONTROL_PANEL_Y (TOUCHBAR_ICON_Y+12)


#define CHANGEBUTTON_X (72)
#define CHANGEBUTTON_Y (CONTROL_PANEL_Y)
#define CHANGEBUTTON_WIDTH  (5*8)
#define CHANGEBUTTON_HEIGHT (3*8)
#define WINCLR_COL(col)	(((col)<<4)|(col))




#define YESBUTTON_X (200)
#define YESBUTTON_Y (160)
#define NOBUTTON_X (200)
#define NOBUTTON_Y (180)

#define _SUBMENU_LISTMAX (8)

#define _LING_LINENO_MAX (12)  //リングバッファ最大
//#define _G3D_MDL_MAX (1)  //モデルの数

/// ドット単位で位置を管理  8*20がBOX 8*12がてもち BOX_MAX_TRAY => 2880+96=2976

//#define TOTAL_DOT_MAX  ((BOX_MAX_TRAY * 20 * 8) + (12 * 8))

#define _STARTDOT_OFFSET (-80)

//GTS交換でステータスを見せる秒数
#define _SLIDE_END_TIME (120)


typedef void (StateFunc)(POKEMON_TRADE_WORK* pState);


// キャンセルを GTSNEGO_CANCEL_DISCONNECT_NUM やったら強制切断 GTSNEGO
#define GTSNEGO_CANCEL_DISCONNECT_NUM (3)


typedef enum
{
  POKEMONTORADE_SEQ_WAIT,  //待機中
  POKEMONTORADE_SEQ_MISERUOK,  //こうかんしたがっている
  POKEMONTORADE_SEQ_CANCEL, //やめたがっている
  POKEMONTORADE_SEQ_NONE,
}POKEMONTORADE_SEQ_ENUM;



typedef enum
{
  PLT_POKEICON,   //ARCID_POKEICON
  PAL_SCROLLBAR, //ARCID_POKETRADE
  PAL_SCROLLBAR_UP, //ARCID_POKETRADE
  PLT_COMMON,
  PLT_POKECREATE,
 // PLT_POKEICON_GRAY,
  PLT_GTS_POKEICON,
  PLT_RESOURCE_MAX,

  CHAR_SCROLLBAR = PLT_RESOURCE_MAX,    //ARCID_POKETRADE
  CHAR_SCROLLBAR_UP,    //ARCID_POKETRADE
  CHAR_SELECT_POKEICON1, //選択中ポケモンアイコン１
  CHAR_SELECT_POKEICON2, //選択中ポケモンアイコン１
  CHAR_COMMON,
  CHAR_POKECREATE,
  CHAR_RESOURCE_MAX,

  ANM_POKEICON = CHAR_RESOURCE_MAX,  //ARCID_POKEICON
  ANM_SCROLLBAR, //ARCID_POKETRADE
  ANM_SCROLLBAR_UP, //ARCID_POKETRADE
  ANM_COMMON,
  ANM_POKECREATE,
  ANM_GTS_POKEICON,
  ANM_RESOURCE_MAX,

  CEL_RESOURCE_MAX=ANM_RESOURCE_MAX,
} _CELL_RESOURCE_TYPE;

typedef enum
{
  CELL_CUR_SELECT,
  CELL_CUR_MOJI,
  CELL_CUR_SCROLLBAR,
  CELL_CUR_POKE_BASEPANEL,
  CELL_CUR_POKE_SELECT,
  CELL_CUR_POKE_PLAYER,
  CELL_CUR_POKE_FRIEND,
  CHAR_LEFTPAGE_MARK,
  CHAR_RETURNPAGE_MARK,
  CELL_CUR_POKE_KEY,
  CELL_DISP_NUM
} _CELL_DISP_TYPE;



// セットアップ番号
enum
{
  SETUP_INDEX_BALL,
  SETUP_INDEX_ROUND,
  SETUP_INDEX_MAX
};


// 顔噴出し
enum{
  GTS_ERUPTED_BUTTON_BASE,
  GTS_ERUPTED_BUTTON_MARK,
  GTS_ERUPTED_BUTTON_NUM,
};

// 顔マークボタン
enum{
  GTS_FACE_BUTTON_GOOD,
  GTS_FACE_BUTTON_BAD,
  GTS_FACE_BUTTON_HART,
  GTS_FACE_BUTTON_ACK,
  GTS_FACE_BUTTON_NUM,
};

// ６体見せ合いの時の文章
enum{
  GTS_SEL6MSG_MY_NAME,
  GTS_SEL6MSG_MY_POKE1,
  GTS_SEL6MSG_MY_POKE2,
  GTS_SEL6MSG_MY_POKE3,
  GTS_SEL6MSG_FRIEND_NAME,
  GTS_SEL6MSG_FRIEND_POKE1,
  GTS_SEL6MSG_FRIEND_POKE2,
  GTS_SEL6MSG_FRIEND_POKE3,
  GTS_SEL6MSG_NUM,
};

typedef enum{
  PAL_SEL6MSG,
  ANM_SEL6MSG,
} OAMRESOURCE_SEL6MSG;

// ポケモンステータスを自分と相手を切り替える際のCELLの位置
#define _POKEMON_SELECT1_CELLX  (128-32)
#define _POKEMON_SELECT1_CELLY  (12)
#define _POKEMON_SELECT2_CELLX  (128+32)
#define _POKEMON_SELECT2_CELLY  (12)

// 日本語検索文字MAX
#define JAPANESE_SEARCH_INDEX_MAX (44)

//GTSで交換するポケモンMAX
#define GTS_NEGO_POKESLT_MAX  (3)
#define GTS_NEGO_UPSTATUSMSG_MAX (6)
#define GTS_PLAYER_WORK_NUM  (2)


#define GTSFACEICON_POSX (8)
#define GTSFACEICON_POSY (144)
#define GTSFACEICON_HEIGHT (24)
#define GTSFACEICON_WIDTH (24)

#define _POKEMARK_MAX  (8)

/// @brief 3Dモデルカラーフェード
typedef struct
{
  GFL_G3D_RES* g3DRES;             //パレットフェード  対象3Dモデルリソースへのポインタ
  void*        pData_dst;          //パレットフェード  転送用ワーク
  u8            pal_fade_flag;      //パレットフェード  起動フラグ
  u8            pal_fade_count;     //パレットフェード  対象リソース数
  u8            pal_fade_start_evy; //パレットフェード　START_EVY値
  u8            pal_fade_end_evy;		//パレットフェード　END_EVY値
  u8            pal_fade_wait;			//パレットフェード　wait値
  u8            pal_fade_wait_tmp;	//パレットフェード　wait_tmp値
  u16           pal_fade_rgb;				//パレットフェード　end_evy時のrgb値
}_EFFTOOL_PAL_FADE_WORK;

/// @brief 2D面フェード
typedef struct
{
  s8   pal_fade_time;			//パレットフェード トータル時間
  s8   pal_fade_nowcount;	//パレットフェード 現在の進行時間
  s8   pal_start;
  s8   pal_end;
}_D2_PAL_FADE_WORK;



/// @brief ポケモン座標移動
typedef struct
{
  MCSS_WORK* pMcss;  ///< 動かす物体
  int   time;			   ///< トータル時間
  int   nowcount;	   ///<  現在の進行時間
  float   percent;   ///< 加速する計算
  float   add;   ///< 変化率
  u16 sins;
  u16 wave;
  u16 waveNum;
  VecFx32  start;      ///< 開始位置
  VecFx32  end;        ///< 終わり位置
  VecFx32* MoveTbl;  ///< テーブル移動の際の座標
} _POKEMCSS_MOVE_WORK;


typedef enum
{  // ボールアイコンタイプ
  UI_BALL_SUBSTATUS,     ///< サブステータス用
  UI_BALL_MYSTATUS,      ///< メイン画面自分の
  UI_BALL_FRIENDSTATUS,  ///< メイン画面相手の

  UI_BALL_NUM
} _UI_BALLICON_TYPE;

typedef struct
{
  //ポケモンボックスマーク・ステート（レア、ポケルス顔）アイコン
  UI_EASY_CLWK_RES      res;
  BOOL load;
  GFL_CLWK                  *clwk[_POKEMARK_MAX];
} _STATE_ICON_WORK;

typedef struct
{
  // ポケアイコン用アイテムアイコン
  UI_EASY_CLWK_RES      clres_poke_item;
  GFL_CLWK                  *clwk_poke_item;
} _ITEMMARK_ICON_WORK;


typedef struct
{
  // ボールアイコン
  UI_EASY_CLWK_RES      clres_ball;
  GFL_CLWK              *clwk_ball;
} _BALL_ICON_WORK;


typedef struct
{
  // タイプアイコン
  UI_EASY_CLWK_RES      clres_type_icon;
  GFL_CLWK                  *clwk_type_icon;
} _TYPE_ICON_WORK;


#include "pokemontrade_demo_local.h"


typedef struct
{
  u16 bAreaOver;   //一回でもエリア外に出たかどうか
} PENMOVE_WORK;


/// 巻き戻しようバックアップ構造体
typedef struct
{
  MAIL_BLOCK* pMail;     
  RECORD* pRecord;
  WIFI_HISTORY* pWifiHis;
  ZUKAN_SAVEDATA* pZukan;
  WIFI_NEGOTIATION_SAVEDATA* pNego;
  POKEPARTY* pPokeParty;
  BOX_TRAY_DATA* pBoxTray;
  PLAYTIME_SAVE_DATE date;
  BOOL bVoice;
} SAVEREV_BACKUP;

typedef struct
{
  u16 monsno;
  u8 formno;
  u8 sex;
} POKE_EASY_INFO;



typedef enum{
  _TEMPBUFF_ID0,
  _TEMPBUFF_ID1,
  _TEMPBUFF_SEND,
  _TEMPBUFF_MAX,
} _TEMPBUFF_ENUM;



struct _POKEMON_TRADE_WORK{
  NHTTP_RAP_WORK* pNHTTP;
  PENMOVE_WORK aPanWork;
  POKEMONTRADE_PARAM* pParentWork;
  POKEMONTRADE_DEMO_WORK* pPokemonTradeDemo;
  u8 FriendPokemonCol[2][BOX_POKESET_MAX + TEMOTI_POKEMAX];         ///< 相手のポケモンBOXにあるポケモン色
  GFL_BMPWIN* StatusWin[2];     ///< ステータス表示
  GFL_BMPWIN* StatusWin1[2];     ///< ステータス表示
  GFL_BMPWIN* StatusWin2[2];     ///< ステータス表示
  GFL_BMPWIN* GTSInfoWindow;     ///< GTSネゴシエーション説明
  GFL_BMPWIN* GTSInfoWindow2;     ///< GTSネゴシエーション説明
  POKEMON_PARAM* recvPoke[2];  ///< 受け取ったポケモンを格納する場所
  BOOL bRecvPokeBox[2];    /// 手持ちかボックスか
  StateFunc* state;      ///< ハンドルのプログラム状態
  TIMEICON_WORK* pTimeIcon;
  HEAPID heapID;
  void* pTexBoard[5];///< ボードテクスチャー
  SAVEREV_BACKUP aBackup;
  
  int BOX_TRAY_MAX;
  int TRADEBOX_LINEMAX;  ///< ボックスのマックスが決まらないので縦の列が変数
  int _SRCMAX;    ///< ボックスのマックスが決まらないのでスクリーンの数
  int _DOTMAX;     ///< ボックスのマックスが決まらないので全横ドット
//  int _DOT_START_POS;  ///< ボックスのマックスが決まらないので始まる場所の変数
//#define TOTAL_DOT_MAX  ((BOX_MAX_TRAY * 20 * 8) + (12 * 8))

  GFL_BMPWIN* MessageWin;  ///< メッセージ系表示

  GFL_ARCUTIL_TRANSINFO bgchar;

  APP_TASKMENU_WIN_WORK* pAppWin;
  APP_TASKMENU_WORK* pAppTask;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
  APP_TASKMENU_RES* pAppTaskRes;
  GFL_BMPWIN* MyInfoWin;            // ステータス表示
  GFL_BMPWIN* BoxNameWin[BOX_MAX_TRAY+1];            // ボックス名表示
  GFL_BMPWIN* mesWin;               // 会話ウインドウ用
  GFL_MSGDATA *pMsgData;  //
  WORDSET *pWordSet;								// メッセージ展開用ワークマネージャー
  GFL_FONT* pFontHandle;
  PRINT_STREAM* pStream;
  STRBUF* pStrBuf;
  STRBUF* pExStrBuf;
  STRBUF* pMessageStrBuf;
  STRBUF* pMessageStrBufEx;
  PRINT_QUE*            SysMsgQue;
  GFL_TCBLSYS *pMsgTcblSys;
  GFL_TCBLSYS *pTimerTcblSys;
  APP_KEYCURSOR_WORK* pKeyCursor;

  GFL_BMPWIN* SerchMojiWin[JAPANESE_SEARCH_INDEX_MAX];  //頭文字検索のウインドウ用
  int selectMoji;                  //頭文字検索で選んだ文字

  GFL_G3D_RES* pG3dRes;
  GFL_G3D_OBJ* pG3dObj;
  GFL_G3D_OBJSTATUS*	pStatus;
  GFL_G3D_CAMERA		*pCamera;
  _EFFTOOL_PAL_FADE_WORK* pModelFade;
  _D2_PAL_FADE_WORK* pD2Fade;

  //  G3D_MDL_WORK			mdl[_G3D_MDL_MAX];
  //    BMPWINFRAME_AREAMANAGER_POS aPos;
  //3D
  int objCount;

  GFL_G3D_UTIL* g3dUtil;
  u16 unitIndex;


  BOX_MANAGER* pBox;
  GAMEDATA* pGameData;
//  MAIL_BLOCK* pMailBlock;
  MYSTATUS* pMy;
  MYSTATUS* pFriend;
  POKEPARTY* pMyParty;
  GFL_G3D_CAMERA    *camera;

  MCSS_SYS_WORK *mcssSys;
  MCSS_WORK     *pokeMcss[4];
  BOOL     mcssStop[2];
  _POKEMCSS_MOVE_WORK* pMoveMcss[4];

  GFL_ARCUTIL_TRANSINFO subchar;
  GFL_ARCUTIL_TRANSINFO subchar1;
  GFL_ARCUTIL_TRANSINFO subchar2;
  GFL_ARCUTIL_TRANSINFO subcharMain;
  u32 cellRes[CEL_RESOURCE_MAX];

  //	GAMESYS_WORK* pGameSys;
  _BALL_ICON_WORK aBallIcon[UI_BALL_NUM];
  _TYPE_ICON_WORK aTypeIcon[4];  //ダブルバッファ
  _ITEMMARK_ICON_WORK aItemMark;
  _ITEMMARK_ICON_WORK aPokerusMark;
  _ITEMMARK_ICON_WORK aPokerusFaceMark;   //下画面用顔マーク
  _STATE_ICON_WORK aPokeMark;

  TOUCHBAR_WORK* pTouchWork;

  GFL_CLUNIT	*cellUnit;
  GFL_TCB *g3dVintr; //3D用vIntrTaskハンドル

//  u32 pokeIconNcgResOrg[BOX_POKESET_MAX+ TEMOTI_POKEMAX];
   
  u32 pokeIconNcgRes[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  GFL_CLWK* markIcon2;
  GFL_CLWK* pokeIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  GFL_CLWK* markIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
//  GFL_CLWK* searchIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u16 pokeIconNo[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u8 pokeIconForm[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u8 pokeIconSex[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u8 pokeIconLine[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  GFL_CLACTPOS pokeIconMark[_LING_LINENO_MAX][BOX_VERTICAL_NUM];  //検索している物
  u8 pokeIconMarkFlg[_LING_LINENO_MAX][BOX_VERTICAL_NUM];

  NET_SAVE_WORK* pNetSave;
  GFL_CLWK* curIcon[CELL_DISP_NUM];
  GFL_CLWK* mojiIcon;

  int windowNum;
  int anmCount;
  int saveStep;
  BOOL bTouch;
  BOOL IsIrc;
  u32 connect_bit;
  BOOL connect_ok;
  BOOL receive_ok;
  u32 receive_result_param;
  u32 receive_first_param;

  int nowBoxno;  //いまのボックス
  int x;
  u32 y;
  BOOL bUpVec;

//  GFL_CLWK* pSelectCLWK;   ///<選んでるポケモンCLACT   こちらは何を選択しているかに使う
  GFL_CLWK* pCatchCLWK;   ///<つかんでるポケモンCLACT  こちらはタッチ時に移動するために使う
  GFL_CLACTPOS aCatchOldPos;  //つかんでるポケモンの前の位置
  GFL_CLACTPOS aDifferencePos;  //つかんでるポケモンの誤差位置
  GFL_CLACTPOS aVecPos;  //ベクトルを作る為にとっておく前の位置
  BOOL bStatusDisp;

  int MainObjCursorLine;   //OBJカーソルライン
  int MainObjCursorIndex;  //OBJカーソルインデックス

  int workPokeIndex;       // マウス操作でにぎったポケモン
  int workBoxno;           // マウス操作でにぎったポケモン
  int selectIndex;  //候補のポケモンIndex   上に表示しているポケモン
  int selectBoxno;  //候補のポケモンBox     上に表示しているポケモン
  int importanceIndex;  //候補のポケモンIndex   上に表示しているポケモン
  int importanceBoxno;  //候補のポケモンBox     上に表示しているポケモン

  int underSelectIndex;  //まだ相手に見せてないポケモンIndex ステータス表示のポケモン
  int underSelectBoxno;  //まだ相手に見せてないポケモンBox   ステータス表示のポケモン
  int pokemonsetCall;
  int pokemonselectnoGTS;

  //GTS用
  int GTSSelectIndex[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];  //候補のポケモンIndex
  int GTSSelectBoxno[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];  //候補のポケモンBox

  GFL_CLWK* pokeSelectSixCur;
  POKEMON_PARAM* GTSSelectPP[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];
  BOOL bGTSSelectPokeBox[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];  //ボックスか手持ちか
  GFL_CLWK* pokeIconGTS[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];
  u32 pokeIconNcgResGTS[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];
  GFL_BMPWIN* TriStatusWin[ GTS_PLAYER_WORK_NUM*2 ];            // ボックス名表示
  int GTStype[GTS_PLAYER_WORK_NUM];  //
  int GTSlv[GTS_PLAYER_WORK_NUM];  //
  BOOL bGTSSelect[GTS_PLAYER_WORK_NUM];  //決定したかどうか

  BOOL pokemonThreeSet;  //相手側のポケモンが３体セットされた
  
  POKEMON_PARAM* TempBuffer[_TEMPBUFF_MAX]; //ポケモン受信用バッファ+送信用バッファ

  BOOL bParent;
  BOOL bTouchReset;

  short xspeed;
  short speed;   ///< スクロール慣性速度
  short BoxScrollNum;   ///< ドット単位で位置を管理  8*20がBOX 8*12がてもち BOX_MAX_TRAY => 2880+96=2976
  short FriendBoxScrollNum;   ///< 上記の相手側の値

  short oldLine; //現在の描画Line 更新トリガ

  BOOL bgscrollRenew;
  BOOL touchON;
  int bgscroll;
  int ringLineNo;
  int demoBGM;
  u16 flipflg;
  u16 bEvCheck;
  int modelno;  ///< 表示しているモデルの番号
  int pokemonselectno;  ///< 自分と相手のポケモン選択中のどちらを指しているか

  void* pVramOBJ;
  void* pVramBG;
  GFL_CLWK* eruptedButtonGTS[GTS_ERUPTED_BUTTON_NUM * 2];  //顔噴出し
  GFL_CLWK* faceButtonGTS[GTS_FACE_BUTTON_NUM];  //顔マークボタン
  GFL_CLWK* select6Msg[GTS_SEL6MSG_NUM];  //文章
  u32 select6CellRes[2]; //pal anm
  GFL_BMP_DATA* listBmp[GTS_SEL6MSG_NUM];
  u32 listRes[GTS_SEL6MSG_NUM];
  u16 palette3d[16];
  u16 palTrans[16];
  u16* scrTray;
  u16* scrTemoti;
  u16* scrTrayMark;
  u16* scrTemotiMark;
  u8* pCharMem;
  GFL_CLACTPOS AddPos;
  POKEMONTRADE_TYPE type;
  PROGVAL_PEZIER_WORK aCutMullRom;
  APP_PRINTSYS_COMMON_WORK trgWork;  //キーの制御を行うワーク
  s16 SuckedCount;
  s16 countNEGOSlideScroll;
  u8 changeFactor[2];
  u8 SlideWindowTimer[2];
  u8 timerErupted[2]; //顔文字消去タイマー
  u8 evilCheck[2];
  u8 pokemonGTSSeq[2];  //メッセージの表示切替
  u8 pokemonGTSStateMode;  //画面的にpokemonGTSSeqを受け入れてよい場合TRUE
  u8 BGClearFlg;
  u8 DemoBGClearFlg;
  u8 bByebyeMessageEach;  //送り先IDと送信ポケモン一致
  u8 bEncountMessageEach; //受け取りIDと受信ポケモン一致
  u8 bByebyeNoJump;
  u8 bEncountNoJump;
  u8 NegoWaitTime;  //NEGOを遅らせるタイマー
  u8 friendBoxNum;  //ともだちのボックス番号
  u8 statusModeOn;  //
  u8 friendMailBoxFULL;  //ともだちはメールボックス満杯かどうか
  u8 mojiIconEnd;  //文字点滅アイコンが終わったかどうか
  u8 zenkoku_flag;
  u8 bBackupStart;  //セーブのスタート このフラグが立ってたらエラー復帰不可能
  u8 pushSound;
  u8 vtask;   //20100603 add saito  dummyがあったので差し替えました。
  u8 triCancel;  //キャンセルを GTSNEGO_CANCEL_DISCONNECT_NUM やったら強制切断 GTSNEGO
  u8 pokemonSendDisableFlg:4;  //ポケモンを相手に送信して良いかどうか
  u8 pokemonEnableSendFlg:4;  //ポケモンを受け取ったという事を相手に送信する
  POKE_EASY_INFO pokeInfo[6+BOX_MAX_TRAY*BOX_MAX_COLUMN*BOX_MAX_RAW];
} ;


#define UNIT_NULL		(0xffff)

extern void IRC_POKMEONTRADE_ChangeFinish(POKEMON_TRADE_WORK* pWork);


extern void IRC_POKETRADE_GraphicInitMainDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_GraphicInitSubDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_GraphicInitSubDispSys(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_MainGraphicExit(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SubGraphicExit(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_CommonCellInit(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_GraphicExit(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SubStatusInit(POKEMON_TRADE_WORK* pWork,int pokeposx, int type);
extern void IRC_POKETRADE_SubStatusEnd(POKEMON_TRADE_WORK* pWork);


extern BOOL POKEMONTRADEPROC_IsTriSelect(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADEPROC_IsNetworkMode(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_PROC_FadeoutStart(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_TOUCHBAR_SetReturnIconActiveTRUE(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_TrayDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_InitBoxIcon( BOX_MANAGER* boxData ,POKEMON_TRADE_WORK* pWork , const BOOL isTransVBlank );
extern void IRC_POKETRADE_AllDeletePokeIconResource(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_G3dDraw(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetSubdispGraphicDemo(POKEMON_TRADE_WORK* pWork,int type);
extern void IRC_POKETRADE_ResetSubdispGraphicDemo(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_GraphicFreeVram(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetMainDispGraphic(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ResetSubDispGraphic(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetSubVram(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetMainVram(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_TouchStateGTS(POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE3D_3DReelPaletteFade(POKEMON_TRADE_WORK *pWork);
extern void IRC_POKETRADE_3DGraphicSetUp( POKEMON_TRADE_WORK* pWork );
extern void IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE type);
extern void IRC_POKETRADE_CreatePokeIconResource(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_InitBoxCursor(POKEMON_TRADE_WORK* pWork);
extern POKEMON_PARAM* IRC_POKEMONTRADE_GetRecvPP(POKEMON_TRADE_WORK *pWork, int index);
extern int POKETRADE_boxScrollNum2Line(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_MainObjCursorDisp(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADEDEMO_Init( POKEMON_TRADE_WORK* pWork );
extern void IRC_POKETRADEDEMO_Main( POKEMON_TRADE_WORK* pWork );
extern void IRC_POKETRADEDEMO_End( POKEMON_TRADE_WORK* pWork );
//extern GFL_CLWK* IRC_POKETRADE_GetCLACT( POKEMON_TRADE_WORK* pWork , int x, int y, int* trayno, int* pokeindex);
extern GFL_CLWK* IRC_POKETRADE_GetCLACT( POKEMON_TRADE_WORK* pWork , int x, int y, int* trayno, int* pokeindex, int* outline, int* outindex, int* outRingLine);

extern void IRC_POKETRADEDEMO_SetModel( POKEMON_TRADE_WORK* pWork, int modelno);
extern void IRC_POKETRADEDEMO_RemoveModel( POKEMON_TRADE_WORK* pWork);
extern void POKE_MAIN_Pokemonset(POKEMON_TRADE_WORK *pWork, int side, POKEMON_PARAM* pp );
extern void POKETRE_MAIN_ChangePokemonSendDataNetwork(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKMEONTRADE_STEP_ChangeDemo_PokeMove(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_IRCDEMO_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_SAVE_Init(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_SAVE_TimingStart(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_EVOLUTION_TimingStart(POKEMON_TRADE_WORK* pWork);

extern void IRCPOKETRADE_PokeDeleteMcss( POKEMON_TRADE_WORK *pWork,int no  );
extern void IRCPOKETRADE_PokeCreateMcss( POKEMON_TRADE_WORK *pWork ,int no, int bFront, const POKEMON_PARAM *pp, BOOL bRev );
extern void IRCPOKETRADE_PokeCreateMcssGTS( POKEMON_TRADE_WORK *pWork ,int no, int bFront, const POKEMON_PARAM *pp, BOOL bRev,BOOL bGTS );
extern POKEMON_PASO_PARAM* IRCPOKEMONTRADE_GetPokeDataAddress(BOX_MANAGER* boxData , int trayNo, int index,POKEMON_TRADE_WORK* pWork);
extern POKE_EASY_INFO* IRCPOKEMONTRADE_GetPokeInfo(int trayNo, int index, POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_SetSubStatusIcon(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetMainStatusBG(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ResetMainStatusBG(POKEMON_TRADE_WORK* pWork);
extern void IRCPOKEMONTRADE_ResetPokemonStatusMessage(POKEMON_TRADE_WORK* pWork, int side);
extern void IRC_POKETRADE_InitSubMojiBG(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_EndSubMojiBG(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SendVramBoxNameChar(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SendScreenBoxNameChar(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ResetBoxNameWindow(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ItemIconDisp(POKEMON_TRADE_WORK* pWork,int side, POKEMON_PARAM* pp);

extern void IRC_POKETRADE_ItemIconReset(_ITEMMARK_ICON_WORK* pIM);

extern void IRC_POKETRADE_PokerusIconDisp(POKEMON_TRADE_WORK* pWork,int side,int bMain, POKEMON_PARAM* pp);
extern void IRC_POKETRADE_PokeStatusIconDisp(POKEMON_TRADE_WORK* pWork, POKEMON_PARAM* pp, BOOL bEgg);
extern void IRC_POKETRADE_PokeStatusIconReset(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetCursorXY(POKEMON_TRADE_WORK* pWork);
extern BOOL POKETRADE_IsMainCursorDispIn(POKEMON_TRADE_WORK* pWork,int* line);
extern int POKETRADE_Line2RingLineIconGet(int line);
extern void POKETRADE_TOUCHBAR_Init(POKEMON_TRADE_WORK* pWork);

extern BOOL POKEMONTRADE_IsPokeLanguageMark(int monsno,int moji);
extern void IRC_POKETRADE_StatusWindowMessagePaletteTrans(POKEMON_TRADE_WORK* pWork, int palno, int palType);
extern void IRC_POKETRADE_GraphicInitSubDispStatusDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_GraphicEndSubDispStatusDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_EndIconResource(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_DEMOCLACT_Create(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_CLACT_Create(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE2D_IconGray(POKEMON_TRADE_WORK* pWork, GFL_CLWK* pCL ,BOOL bGray);
extern void IRC_POKETRADE_PosChangeSubStatusIcon(POKEMON_TRADE_WORK* pWork,int sel,BOOL bReset);
extern void POKEMONTRADE_VisibleFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo, BOOL bVisible);
extern void POKE_GTS_VisibleFaceIcon(POKEMON_TRADE_WORK* pWork,BOOL bVisible);
extern BOOL POKE_GTS_BanPokeCheck(POKEMON_TRADE_WORK* pWork, POKEMON_PASO_PARAM* ppp);
extern void POKETRADE_2D_GTSPokemonIconChangePosUp(POKEMON_TRADE_WORK* pWork,int side_up, int no_up);

extern void POKEMON_TRADE_MaskCommon(POKEMON_TRADE_WORK* pWork);
extern BOOL POKE_GTS_IsMyIn(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_WindowOpenXY(POKEMON_TRADE_WORK* pWork,BOOL bFast,int x,int y,int xm,int ym);
extern int POKEMONTRADE_GetFriendBoxNum(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_NEGOBG_Select6Create(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_NEGOBG_Select6Scroll(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_ResetPokemonBallIcon(POKEMON_TRADE_WORK *pWork);

extern void POKEMONTRADE_NEGOBG_SlideInit(POKEMON_TRADE_WORK* pWork,int side, POKEMON_PARAM* pp);
extern void POKEMONTRADE_NEGOBG_SlideMessage(POKEMON_TRADE_WORK *pWork, int side,POKEMON_PARAM* pp);
extern void POKEMONTRADE_NEGOBG_SlideMessageDel(POKEMON_TRADE_WORK *pWork,int side);
extern void POKEMONTRADE_NEGO_SlideInit(POKEMON_TRADE_WORK* pWork,int side,POKEMON_PARAM* pp);
extern void POKEMONTRADE_NEGO_SlideMain(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_2D_GTSPokemonIconVisible(POKEMON_TRADE_WORK* pWork,int side, BOOL onoff);
extern void POKEMONTRADE_changeTimingDemoStart(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_IsEggAndLastBattlePokemonChange(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_WindowTimeIconStart(POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE_DEMOBGMChange(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKMEONTRADE_changeState(POKEMON_TRADE_WORK* pWork,StateFunc* state);
extern BOOL POKEMONTRADE_IsMailBoxFull(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_IsMailPokemon(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRAE_EndCancelState(POKEMON_TRADE_WORK* pWork);

extern void IRCPOKETRADE_PokeCreateMcssNormal( POKEMON_TRADE_WORK *pWork ,int no, int bFront, const POKEMON_PARAM *pp, BOOL bRev );
extern POKEMON_PARAM* POKEMONTRADE_CreatePokeData(BOX_MANAGER* boxData , int trayNo, int index,POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_IsIllegalPokemon(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_IsIllegalPokemonFriend(POKEMON_TRADE_WORK* pWork);
extern BOOL POKE_GTS_CreateCancelButton(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_DeleteCancelButton(POKEMON_TRADE_WORK* pWork, BOOL bar,BOOL change);
extern void POKEMON_TRADE_PaletteFade(POKEMON_TRADE_WORK* pWork, BOOL bFade, int type, int paletteBIT);

extern void POKEMONTRADE2D_SetVisibleMainVBlank(POKEMON_TRADE_WORK* pWork, u32 frame );
extern void POKEMONTRADE2D_SetVisibleSubVBlank(POKEMON_TRADE_WORK* pWork, u32 frame );

#if _TRADE_DEBUG

extern void IRC_POKMEONTRADE_changeStateDebug(POKEMON_TRADE_WORK* pWork,StateFunc* state, int line);

#define   _CHANGE_STATE(pWork, state)  IRC_POKMEONTRADE_changeStateDebug(pWork ,state, __LINE__)

#else  //_NET_DEBUG


#define   _CHANGE_STATE(pWork, state)  IRC_POKMEONTRADE_changeState(pWork ,state)

#endif //_NET_DEBUG


#define _TEMOTITRAY_SCR_MAX (12)
#define _BOXTRAY_SCR_MAX (20)
//#define _SRCMAX ((BOX_MAX_TRAY*_BOXTRAY_SCR_MAX)+_TEMOTITRAY_SCR_MAX)

#define _TEMOTITRAY_MAX (8*_TEMOTITRAY_SCR_MAX)
#define _BOXTRAY_MAX (8*_BOXTRAY_SCR_MAX)

//#define _DOTMAX ((BOX_MAX_TRAY*_BOXTRAY_MAX)+_TEMOTITRAY_MAX)

//#define _DOT_START_POS (_DOTMAX - 80)


extern int IRC_TRADE_LINE2TRAY(int lineno,POKEMON_TRADE_WORK* pWork);
extern int IRC_TRADE_LINE2POKEINDEX(int lineno,int verticalindex);


// 通常のフォントカラー
#define	FBMP_COL_NULL		(0)
#define	FBMP_COL_BLACK		(1)
#define	FBMP_COL_BLK_SDW	(2)
#define	FBMP_COL_RED		(3)
#define	FBMP_COL_RED_SDW	(4)
#define	FBMP_COL_GREEN		(7)
#define	FBMP_COL_GRN_SDW	(8)
#define	FBMP_COL_BLUE		(5)
#define	FBMP_COL_BLUE_SDW	(6)
#define	FBMP_COL_PINK		(9)
#define	FBMP_COL_PNK_SDW	(10)
#define	FBMP_COL_WHITE_SDW		(2)
#define	FBMP_COL_WHITE		(15)



#define _CLACT_SOFTPRI_POKESEL  (11)
#define _CLACT_SOFTPRI_POKESEL_BAR  (12)
#define _CLACT_SOFTPRI_CATCHPOKE  (1)
#define _CLACT_SOFTPRI_SCROLL_BAR  (14)
#define _CLACT_SOFTPRI_SELECT (15)
#define _CLACT_SOFTPRI_POKELIST  (16)
#define _CLACT_SOFTPRI_MARK (50)

// 顔をだす時間
#define _TIMER_ERUPTED_NUM (60)


///通信コマンド
typedef enum {
  _NETCMD_CHANGEFACTOR = GFL_NET_CMD_IRCTRADE,
  _NETCMD_SELECT_POKEMON,
  _NETCMD_CHANGE_POKEMON,
  _NETCMD_POKEMONSELECT_GTS,
  _NETCMD_LOOKATPOKE,
  _NETCMD_END,
  _NETCMD_CANCEL_POKEMON,
  _NETCMD_THREE_SELECT1,
  _NETCMD_THREE_SELECT2,
  _NETCMD_THREE_SELECT3,
  _NETCMD_THREE_SELECT_END,
  _NETCMD_THREE_SELECT_CANCEL,
  _NETCMD_SCROLLBAR,
  _NETCMD_FACEICON,
  _NETCMD_POKEMONCOLOR,
  _NETCMD_GTSSEQNO,
  _NETCMD_FRIENDBOXNUM,
  _NETCMD_EVILCHECK,
  _NETCMD_UN,
  _NETCMD_THREE_SELECT1_BOX,
  _NETCMD_THREE_SELECT2_BOX,
  _NETCMD_THREE_SELECT3_BOX,
  _NETCMD_SELECT_POKEMON_BOX,
  _NETCMD_SELECT_RETURN,
} _POKEMON_TRADE_SENDCMD;


extern BOOL POKE_GTS_PokemonsetAndSendData(POKEMON_TRADE_WORK* pWork,int index,int boxno);
extern void POKE_GTS_CreatePokeIconResource(POKEMON_TRADE_WORK* pWork,int mainsub);
extern void POKE_GTS_StatusMessageDisp(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_SelectStatusMessageDisp(POKEMON_TRADE_WORK* pWork, int side, BOOL bSelected, BOOL bForce);
extern void POKE_GTS_SelectStatusMessageDelete(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_EndWork(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_InitWork(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_Select6Init(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_Select6MessageInit(POKEMON_TRADE_WORK* pWork);
extern int POKE_GTS_IsSelect(POKEMON_TRADE_WORK* pWork,int boxno,int index);
extern BOOL POKE_GTS_IsFullMode(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADEPROC_IsTimeWaitSelect(POKEMON_TRADE_WORK* pWork);
extern int POKEMONTRADE_NEGO_CheckGTSSetPokemon(POKEMON_TRADE_WORK* pWork,int side,int index,int boxno);




//メッセージ関連
extern void POKETRADE_MESSAGE_WindowOpen(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_WindowOpenCustom(POKEMON_TRADE_WORK* pWork,BOOL bFast, BOOL bUnderPos);
extern void POKETRADE_MESSAGE_WindowClose(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_WindowClear(POKEMON_TRADE_WORK* pWork);
extern BOOL POKETRADE_MESSAGE_EndCheck(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_HeapInit(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_HeapEnd(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_AppMenuOpenCustom(POKEMON_TRADE_WORK* pWork, int *menustr,int num, int x,int y);
extern void POKETRADE_MESSAGE_AppMenuOpen(POKEMON_TRADE_WORK* pWork, int *menustr,int num);
extern void POKETRADE_MESSAGE_AppMenuClose(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_ChangePokemonMyStDisp(POKEMON_TRADE_WORK* pWork,int pageno,POKEMON_PARAM* pp);
extern void POKETRADE_MESSAGE_ChangePokemonStatusDisp(POKEMON_TRADE_WORK* pWork,POKEMON_PARAM* pp, int mcssno, int change);
extern void POKETRADE_MESSAGE_SetPokemonStatusMessage(POKEMON_TRADE_WORK *pWork, int side,POKEMON_PARAM* pp);
extern void POKETRADE_MESSAGE_ResetPokemonMyStDisp(POKEMON_TRADE_WORK* pWork,BOOL bClear);
extern void POKETRADE_MESSAGE_ResetPokemonStatusMessage(POKEMON_TRADE_WORK *pWork, const BOOL win_mask_off); //20100624 mod Saito BTS6424
extern void POKETRADE_MESSAGE_CreatePokemonParamDisp(POKEMON_TRADE_WORK* pWork, POKEMON_PARAM* pp);
extern void POKETRADE_MESSAGE_SixStateDisp(POKEMON_TRADE_WORK* pWork,int frame);
extern void POKETRADE_MESSAGE_ChangeStreamType(POKEMON_TRADE_WORK* pWork,int type);

//ポケモン２Ｄ
extern void POKETRADE_2D_GTSPokemonIconSet(POKEMON_TRADE_WORK* pWork, int side,int no, POKEMON_PARAM* pp, int hilow, BOOL bDisp);
extern void POKETRADE_2D_GTSPokemonIconReset(POKEMON_TRADE_WORK* pWork,int side, int no);


extern void POKEMONTRADE_StartFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo);
extern void POKEMONTRADE_RemoveFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo);


//ポケモンプロセス ir union wifi用
extern void POKETRADE_PROC_PokemonStatusStart(POKEMON_TRADE_WORK* pWork);
extern void POKE_TRADE_PROC_TouchStateCommon(POKEMON_TRADE_WORK* pWork);

//ポケモンネゴシエーション用
extern void POKETRADE_NEGO_Select6keywait(POKEMON_TRADE_WORK* pWork);
extern int POKETRADE_NEGO_IsSelect(POKEMON_TRADE_WORK* pWork,int line , int height);
extern BOOL POKETRADE_NEGO_IsStatusLookAt(POKEMON_TRADE_WORK* pWork,int line , int height);

extern void POKETRADE_TOUCHBAR_Init(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_RemoveCoreResource(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_ReleasePokeIconResource(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_SixStateDelete(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_DirectAddPokemon(POKEMON_TRADE_WORK* pWork,int index,const POKEMON_PARAM* pp);
extern void POKE_GTS_DeletePokemonDirect(POKEMON_TRADE_WORK* pWork,int side,int index);
extern void POKETRADE_2D_GTSPokemonIconResetAll(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_DeletePokemonState(POKEMON_TRADE_WORK* pWork);

extern void POKETRADE_2D_ObjTrayDispInit(POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE_StartCatched(POKEMON_TRADE_WORK* pWork,int line, int pos,int x,int y,POKEMON_PASO_PARAM* ppp);
extern void POKEMONTRADE_StartSucked(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_SuckedMain(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_RemovePokemonCursor(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_2D_AlphaSet(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE2D_IconAllGray(POKEMON_TRADE_WORK* pWork,BOOL bGray);
extern void  POKEMONTRADE_PokeIconCgxLoad(POKEMON_TRADE_WORK* pWork, int count );


extern void POKE_GTS_InitFaceIcon(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_DeleteFaceIcon(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_FaceIconFunc(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_FaceIconDeleteFunc(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_TouchFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo);
extern void POKEMONTRADE_StartEruptedGTS(POKEMON_TRADE_WORK* pWork, int faceNo, int index);
extern void POKEMONTRADE_RemoveEruptedGTS(POKEMON_TRADE_WORK* pWork,int index);

extern void POKE_GTS_DeleteEruptedIcon(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_InitEruptedIcon(POKEMON_TRADE_WORK* pWork,int faceNo, int index);

extern void POKE_GTS_InitEruptedIconResource(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ReleaseMainObj(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_InitMainObj(POKEMON_TRADE_WORK* pWork);
extern BOOL PokemonTrade_SetMyPokeColor( POKEMON_TRADE_WORK* pWork,int index  );
extern void IRC_POKETRADE3D_SetColorTex( POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE_StartPokeSelectSixButton(POKEMON_TRADE_WORK* pWork,int index);
extern void POKEMONTRADE_RemovePokeSelectSixButton(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_IsInPokemonRecvPoke(POKEMON_PARAM* pp);
extern void POKEMONTRADE_CreatePokeSelectMessage(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_RemovePokeSelectMessage(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_MessageOAMWriteVram(POKEMON_TRADE_WORK* pWork);
extern BOOL IRC_POKEMONTRADE_GetRecvBoxFlg(POKEMON_TRADE_WORK *pWork, int index);
extern void IRC_POKEMONTRADE_SetRecvBoxFlg(POKEMON_TRADE_WORK *pWork, int index, BOOL bFlg);


extern void POKMEONTRADE_DEMO_GTSUP_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_DEMO_GTSDOWN_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_DEMO_GTSMID_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_DEMO_GTS_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_StartMojiSelect(POKEMON_TRADE_WORK* pWork,int x,int y);
extern void POKEMONTRADE_EndMojiSelect(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_CheckMojiSelect(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE2D_ChangePokemonPalette(POKEMON_TRADE_WORK* pWork, BOOL bGray);
extern void POKEMONTRADE_McssAnmStop( u32 data, fx32 currentFrame );
extern void POKEMONTRADE_CancelCall(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_McssMoveDel(_POKEMCSS_MOVE_WORK* pPoke);



//ボタン作成
extern APP_TASKMENU_WIN_WORK* POKEMONTRADE_MESSAGE_CancelButtonStart(POKEMON_TRADE_WORK* pWork,int msgno);
extern void POKEMONTRADE_MESSAGE_CancelButtonDelete(POKEMON_TRADE_WORK* pWork,int bar,int change);
extern BOOL POKEMONTRADE_MESSAGE_ButtonCheck(POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE_pokeMoveFunc(_POKEMCSS_MOVE_WORK* pMove,POKEMON_TRADE_WORK* pWork);
extern _POKEMCSS_MOVE_WORK* POKEMONTRADE_pokeTblMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, VecFx32* pTbl, HEAPID heapID);
extern _POKEMCSS_MOVE_WORK* POKEMONTRADE_pokeMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, HEAPID heapID);
extern void POKEMONTRADE_pokeMoveRenew(_POKEMCSS_MOVE_WORK* pPoke,int time, const VecFx32* pPos);
extern void POKETRADE_SE_irSeStrat(int count);
extern void POKETRADE_SE_gtsSeStrat(int count);

