//=============================================================================
/**
 * @file	wifi_p2pmatch_local.h
 * @brief	WIFICLUBローカル定義
 * @author	k.ohno
 * @date    2006.4.5
 */
//=============================================================================


#pragma once


#include <gflib.h>
#include "net_app/wificlub/wifi_p2pmatch.h"

#include "wifi_status.h"
#include "savedata/wifilist.h"
#include "savedata/config.h"

#include "print/wordset.h"

#include "system/bmp_oam.h"
#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"

#include "net_app/connect_anm.h"
#include "app/app_keycursor.h"

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_regulation.h"

#include "wifi_p2pmatchroom.h"
#include "ui/touchbar.h"
#include "system/time_icon.h"
#include "app/app_printsys_common.h"


#define WIFIP2PMATCH_MEMBER_MAX  (WIFILIST_FRIEND_MAX)
#define WIFIP2PMATCH_DISP_MEMBER_MAX  (5)

// 機能ボタン用定義
#define FUNCBUTTON_NUM	( 7 ) 		// 機能ボタンの数
#define START_WORDPANEL ( 0 )		// 最初の文字入力パネルの番号（０＝ひらがな）

// CLACTで定義しているセルが大きすぎてサブ画面に影響がでてしまうので離してみる
#define NAMEIN_SUB_ACTOR_DISTANCE 	(256)

// CellActorに処理させるリソースマネージャの種類の数（＝マルチセル・マルチセルアニメは使用しない）
#define CLACT_RESOURCE_NUM		(  4 )
#define _OAM_NUM			( 5 )

#define FRIENDCODE_MAXLEN     (12)

// マッチングルームにはいてくる人の数（自分も入れて）
#define MATCHROOM_IN_NPCNUM	(32)
#define MATCHROOM_IN_OBJNUM	(MATCHROOM_IN_NPCNUM+1)

// メッセージ表示後のWAIT
#define WIFIP2PMATCH_CORNER_MESSAGE_END_WAIT	( 60 )


//上がめんぽけもんアイコンパレット
#define _OBJPLT_POKEICON_OFFSET (11*32)

































// 文字パネルの遷移用
enum{
  WIFIP2PMATCH_MODE_INIT  = 0,
  WIFIP2PMATCH_RETRY_INIT,
  WIFIP2PMATCH_RETRY_YESNO,
  WIFIP2PMATCH_RETRY_WAIT,
  WIFIP2PMATCH_RETRY,
  WIFIP2PMATCH_CONNECTING_INIT,
  WIFIP2PMATCH_CONNECTING,
  WIFIP2PMATCH_FIRST_ENDMSG,
  WIFIP2PMATCH_FIRST_ENDMSG_WAIT,
  WIFIP2PMATCH_FRIENDLIST_INIT,
  WIFIP2PMATCH_FRIENDLIST_INIT2,
  WIFIP2PMATCH_MODE_FRIENDLIST,
  WIFIP2PMATCH_VCHATWIN_WAIT,           
  WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2,
  WIFIP2PMATCH_MODE_VCT_CONNECT_INIT,
  WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT,
  WIFIP2PMATCH_MODE_VCT_CONNECT,
  WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO,
  WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT,
  WIFIP2PMATCH_MODE_VCT_DISCONNECT,
  WIFIP2PMATCH_MODE_BATTLE_DISCONNECT,
  WIFIP2PMATCH_MODE_DISCONNECT,
  WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT, 
  WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT,
  WIFIP2PMATCH_MODE_BATTLE_CONNECT,
  WIFIP2PMATCH_MODE_MAIN_MENU,
  WIFIP2PMATCH_MODE_END_WAIT,
  WIFIP2PMATCH_MODE_CHECK_AND_END,
  WIFIP2PMATCH_MODE_SELECT_INIT,
  WIFIP2PMATCH_MODE_SELECT_WAIT,
  WIFIP2PMATCH_MODE_SUBBATTLE_WAIT,
  WIFIP2PMATCH_MODE_SELECT_REL_INIT,
  WIFIP2PMATCH_MODE_SELECT_REL_YESNO,   
  WIFIP2PMATCH_MODE_SELECT_REL_WAIT,
  WIFIP2PMATCH_MODE_MATCH_INIT,
  WIFIP2PMATCH_MODE_MATCH_INIT2,
  WIFIP2PMATCH_MODE_MATCH_WAIT,
  WIFIP2PMATCH_MODE_MATCH_LOOP,
  WIFIP2PMATCH_MODE_BCANCEL_YESNO,
  WIFIP2PMATCH_MODE_BCANCEL_WAIT,
  WIFIP2PMATCH_MODE_CALL_INIT,
  WIFIP2PMATCH_MODE_CALL_YESNO,
  WIFIP2PMATCH_MODE_CALL_SEND,
  WIFIP2PMATCH_MODE_CALL_CHECK,     
  WIFIP2PMATCH_MODE_MYSTATUS_WAIT,
  WIFIP2PMATCH_MODE_CALL_WAIT,
  WIFIP2PMATCH_MODE_PERSONAL_INIT,
  WIFIP2PMATCH_MODE_PERSONAL_WAIT,
  WIFIP2PMATCH_MODE_PERSONAL_END,
  WIFIP2PMATCH_MODE_EXIT_YESNO,
  WIFIP2PMATCH_MODE_EXIT_WAIT,
  WIFIP2PMATCH_MODE_EXITING,
  WIFIP2PMATCH_MODE_EXIT_END,
  WIFIP2PMATCH_NEXTBATTLE_YESNO,     
  WIFIP2PMATCH_NEXTBATTLE_WAIT,
  WIFIP2PMATCH_MODE_VCHAT_NEGO,
  WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT,
  WIFIP2PMATCH_RECONECTING_WAIT,
  WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT,
  WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT,
  WIFIP2PMATCH_FIRST_SAVING,
  WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT,
  WIFIP2PMATCH_FIRST_SAVING2,   
  WIFIP2PMATCH_MODE_CALLGAME_INIT,
  WIFIP2PMATCH_MODE_PLAYERDIRECT_INIT,
  WIFIP2PMATCH_PLAYERDIRECT_INIT1,
  WIFIP2PMATCH_PLAYERDIRECT_INIT2,
  WIFIP2PMATCH_PLAYERDIRECT_INIT3,
  WIFIP2PMATCH_PLAYERDIRECT_INIT5,
  WIFIP2PMATCH_PLAYERDIRECT_INIT6,
  WIFIP2PMATCH_PLAYERDIRECT_INIT7,
  WIFIP2PMATCH_PLAYERDIRECT_RETURN,
  WIFIP2PMATCH_PLAYERDIRECT_WAIT,
  WIFIP2PMATCH_PLAYERDIRECT_WAIT_ST,
  WIFIP2PMATCH_PLAYERDIRECT_END,  
  WIFIP2PMATCH_PLAYERDIRECT_VCT,
  WIFIP2PMATCH_PLAYERDIRECT_TVT,
  WIFIP2PMATCH_PLAYERDIRECT_TRADE,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE1,
  WIFIP2PMATCH_PLAYERDIRECT_SUB1,
  WIFIP2PMATCH_PLAYERDIRECT_SUB2,
  WIFIP2PMATCH_PLAYERDIRECT_SUBSTART,
  WIFIP2PMATCH_PLAYERDIRECT_SUBSTARTCALL,
  WIFIP2PMATCH_PLAYERDIRECT_ENDCALL,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE,   //ルール
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE2,   //ルール
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER,  //シューター
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER2,  //シューター
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE,  //決定
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH,  //ルールを見る 
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH2,  //ルールを見る 
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO1,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO3,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO4,
  WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START3,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START4,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START5,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START6,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START7,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_TEMOTI,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_BBOX,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_WAIT,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED3,
  WIFIP2PMATCH_MESSAGEEND_RETURNLIST,
  WIFIP2PMATCH_RETURNLIST,
  WIFIP2PMATCH_PLAYERMACHINE_INIT1,
  WIFIP2PMATCH_PLAYERMACHINE_NOREG_PARENT,
  WIFIP2PMATCH_PLAYERMACHINE_BATTLE_DECIDE,
  WIFIP2PMATCH_MODE_CHILD_CONNECT,
  WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT1,
  WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT2,
  WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT1,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_12,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_13,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_14,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE1,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE2,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE3,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE4,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE5,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE6,
  WIFIP2PMATCH_MODE_SELECT_INIT2,
  WIFIP2PMATCH_PLAYERDIRECT_END2,
  WIFIP2PMATCH_PLAYERDIRECT_ENDCALL2,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE_E,
  WIFIP2PMATCH_MODE_CALL_CHECK_D,
  WIFIP2PMATCH_MODE_CONNECTWAIT,
  WIFIP2PMATCH_MODE_CONNECTWAIT2,
  WIFIP2PMATCH_PLAYERDIRECT_WAIT_COMMAND,
  WIFIP2PMATCH_PLAYERDIRECT_WAIT_COMMAND2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO3_KEYWAIT,
  WIFIP2PMATCH_PLAYERDIRECT_END_KEYWAIT,
  WIFIP2PMATCH_PLAYERDIRECT_END3,
  WIFIP2PMATCH_PLAYERDIRECT_SUB3,
  WIFIP2PMATCH_PLAYERDIRECT_SUB_FAILED,
  WIFIP2PMATCH_MODE_DISCONNECT2,
  WIFIP2PMATCH_VCTEND_COMMSEND1,
  WIFIP2PMATCH_VCTEND_COMMSEND2,
  WIFIP2PMATCH_VCTEND_COMMSEND3,
  WIFIP2PMATCH_MODE_VCT_CONNECT_MAIN,
  WIFIP2PMATCH_PLAYERDIRECT_SUB23,
  WIFIP2PMATCH_PLAYERMACHINE_TALKEND,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START42,
  WIFIP2PMATCH_PLAYERDIRECT_END_NEXT,
  WIFIP2PMATCH_VCTEND_COMMSEND2_ENDCHK,
  WIFIP2PMATCH_PLAYERDIRECT_CANCELEND,
  WIFIP2PMATCH_PLAYERDIRECT_CANCELEND_NEXT,
  WIFIP2PMATCH_PLAYERDIRECT_SUB45,
  WIFIP2PMATCH_MODE_FRIENDLIST_MW,
  WIFIP2PMATCH_RECONECTING_WAIT_PRE,
  WIFIP2PMATCH_MODE_TVTMESSAGE1_YESNO,
  WIFIP2PMATCH_MODE_TVTMESSAGE1_WAIT,
  WIFIP2PMATCH_MODE_TVTMESSAGE2_YESNO,
  WIFIP2PMATCH_MODE_TVTMESSAGE2_WAIT,
  WIFIP2PMATCH_MODE_TVTMESSAGE3_YESNO,
  WIFIP2PMATCH_MODE_TVTMESSAGE3_WAIT,
  WIFIP2PMATCH_MODE_TVTMESSAGE4_YESNO,
  WIFIP2PMATCH_MODE_TVTMESSAGE4_WAIT,
  WIFIP2PMATCH_PLAYERDIRECT_END_CHILD,
  WIFIP2PMATCH_PLAYERDIRECT_END_CHILD_NEXT,
  WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT2,
  WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT3,
  WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT0,  //256個超えてはいけない
  WIFIP2PMATCH_MODE_FRIENDLIST_MW_WAIT,
};





enum{
	WIFIP2PMATCH_SYNCHRONIZE_END=201,
};

// 上下画面指定定義
#define BOTH_LCD	( 2 )
#define MAIN_LCD	( GF_BGL_MAIN_DISP )	// 要は０と
#define SUB_LCD		( GF_BGL_SUB_DISP )		// １なんですが。


// BMPWIN指定
enum{
	BMP_NAME1_S_BG0,
	BMP_NAME2_S_BG0,
	BMP_NAME3_S_BG0,
	BMP_NAME4_S_BG0,
	BMP_NAME5_S_BG0,
	BMP_WIFIP2PMATCH_MAX,
};

///レギュレーション：ノーマル色
#define REG_NORMAL_COLOR      PRINTSYS_MACRO_LSB(1, 2, 0)
///レギュレーション：NG色
#define REG_FAIL_COLOR        PRINTSYS_MACRO_LSB(4, 2, 0)


#define MSG_WHITE_COLOR      PRINTSYS_MACRO_LSB(15, 2, 0)


typedef enum{
  REGWIN_TYPE_RULE,        ///<ルールを見る
  REGWIN_TYPE_RULE_SHORT,
  REGWIN_TYPE_NG_TEMOTI,   ///<手持ちNGチェック
  REGWIN_TYPE_NG_BBOX,     ///<バトルボックスNGチェック
} REGWIN_TYPE;

//u16 singleCur[_MENUTYPE_MAX];			// バトルタイプ選択メニューカーソルの種類
typedef enum{
  _MENUTYPE_BATTLE_CUSTOM,
  _MENUTYPE_BATTLE_MODE,
  _MENUTYPE_BATTLE_RULE,
  _MENUTYPE_BATTLE_SHOOTER,
  _MENUTYPE_GAME,
  _MENUTYPE_POKEPARTY,
  _MENUTYPE_MAX
} _MENUTYPE_;


#define _CANCELENABLE_TIMER (60*60)   // キャンセルになる為のタイマー60min


typedef enum{
	WF_VIEW_STATUS,
	WF_VIEW_VCHAT,
	WF_VIEW_STATUS_NUM,
} WF_VIEW_STATUS_e;

// ユーザ表示ボタン数
typedef enum{
	MCV_USERD_BTTN_LEFT = 0,
	MCV_USERD_BTTN_BACK,
	MCV_USERD_BTTN_RIGHT,
	MCV_USERD_BTTN_NUM,
} MCV_USERD_BTTN_e;

//プレイヤーの状況
typedef enum
{
  WIFIP2PMATCH_STATE_NONE,     //何も行っていない
  WIFIP2PMATCH_STATE_MACHINE,  //マシーンにて募集
  WIFIP2PMATCH_STATE_MACHINE_RECV,  //募集に参加した
  WIFIP2PMATCH_STATE_TALK,     //話しかけた（親）
  WIFIP2PMATCH_STATE_RECV,     //話しかけられた（子）
} WIFIP2PMATCH_STATE;

//レギュレーション作成タイプ
typedef enum{
  REG_CREATE_TYPE_DECIDE, ///<決定しているルールから作る
  REG_CREATE_TYPE_SELECT, ///<選んでいるルールから作る
}REG_CREATE_TYPE;

//-------------------------------------
///	Iconグラフィック
//=====================================
typedef struct {
	void* p_buff;
	NNSG2dScreenData* p_scrn;

	void* p_charbuff;
	NNSG2dCharacterData* p_char;
  u32 wf_match_all_iconcgx1m;
  u32 wf_match_all_iconcgx2s;

} WIFIP2PMATCH_ICON;


typedef struct {
  u8 bgmVol;        ///< BGMボリューム変更
  u8 bgmVolStart;    ///< BGMボリューム変更値
  u8 bgmVolEnd;    ///< BGMボリューム最後の値
  u8 bgmVolCount;    ///< BGMボリューム変更カウント
} WIFIP2PMATCH_VOL;


//-------------------------------------
///	データビューアーデータ
//=====================================
typedef struct {
	// ワードセット
	WORDSET*	p_wordset;			// メッセージ展開用ワークマネージャー
	// ボタングラフィック
	void* p_bttnbuff;
	NNSG2dScreenData* p_bttnscrn;

	void* p_userbuff[1];
	NNSG2dScreenData* p_userscrn[1];

	void* p_useretcbuff;
//	NNSG2dScreenData* p_useretcscrn;
	
	GFL_BUTTON_MAN* p_bttnman;	// ボタン管理システム
	u8 bttnfriendNo[ MATCHROOM_IN_NPCNUM ];
	u8 frame_no;	// 今表示している床のナンバー
	u8 touch_friendNo;	// 触れている友達番号+1
	u8 touch_frame;	// フレーム数
	u8 user_disp;		// ユーザーデータ表示ONOFF
	s8 user_dispno;	// ユーザーDISP内容
	u8 button_on;		// ボタン表示更新
	u8 button_count;	// ボタンフレームカウンタ
	u8 bttn_chg_friendNo;	// ボタンを更新してほしい友達番号
	BOOL bttn_allchg;	// ボタンすべてを更新するか 

	// 表示ビットマップ面
	GFL_BMPWIN*	  nameWin[ WCR_MAPDATA_1BLOCKOBJNUM ];
	GFL_BMPWIN*	  statusWin[ WCR_MAPDATA_1BLOCKOBJNUM ][ WF_VIEW_STATUS_NUM ];
	GFL_BMPWIN*	  userWinStatus;
	GFL_BMPWIN*	  userWin;

	// ボタン
	u32	button_res[ 4 ];
  u32 btnCGRid;
  u32 btnCLRid;
  u32 btnCERid;

//	CLACT_WORK_PTR button_act[MCV_USERD_BTTN_NUM];
   GFL_CLWK* button_act[MCV_USERD_BTTN_NUM];
#if 1
//  CHAR_MANAGER_ALLOCDATA back_fontoam_cg;
  BMPOAM_SYS_PTR back_fontoam;	// もどる用FONTOAM
  BMPOAM_ACT_PTR BmpOamAct;
  GFL_BMP_DATA*  BmpOamBmp;
#endif
  u32 buttonact_on;			// ボタン動作モード
	u32 touch_button;
	u32 touch_button_event;
	GFL_BUTTON_MAN* p_oambttnman;	// ボタン管理システム
} WIFIP2PMATCH_VIEW;



struct _WIFIP2PMATCH_WORK{
	GFL_TCB					*vblankFunc;
  WIFI_LIST* pList;				// セーブデータ内のユーザーとフレンドデータ
  WIFI_STATUS* pMatch;		// サーバーと送受信する自分の状態
//  TEST_MATCH_WORK* pMatch;		// サーバーと送受信する自分の状態

  int friendMatchReadCount;		// ビーコンを受信した友達数
  u8 index2No[WIFIP2PMATCH_MEMBER_MAX];	// フレンドナンバー配列
  u8 index2NoBackUp[WIFIP2PMATCH_MEMBER_MAX];	// フレンドナンバー配列バックアップ
  u8 matchStatusBackup[WIFIP2PMATCH_MEMBER_MAX];// 友達ビーコンデータが変わったかﾁｪｯｸ用
  u8 matchGamemodeBackup[WIFIP2PMATCH_MEMBER_MAX];// 友達ビーコンデータが変わったかﾁｪｯｸ用
  u8 matchVchatBackup[WIFIP2PMATCH_MEMBER_MAX];// 友達ビーコンデータが変わったかﾁｪｯｸ用
//  NAMEIN_PARAM*		nameinParam;
  
  // メインリスト用ワーク
  BMP_MENULIST_DATA*   menulist;
  BMPMENULIST_WORK* lw;		// BMPメニューワーク
  
  BMP_MENULIST_DATA*   submenulist;
  BMPMENULIST_WORK* sublw;		// BMPメニューワーク
//  GFL_BG_INI		*bgl;									// GF_BGL_INI
  SAVE_CONTROL_WORK*  pSaveData;
  GAMEDATA* pGameData;
  POKEPARTY* pMyPoke;
  POKEPARTY* bb_party;
  void* pEmail;
  void* pFrontier;
  CONFIG* pConfig;
  WORDSET			*WordSet;								// メッセージ展開用ワークマネージャー
  GFL_MSGDATA *MsgManager;							// 名前入力メッセージデータマネージャー
  GFL_MSGDATA *SysMsgManager;  //
  GFL_FONT 			*fontHandle;
  GFL_FONT 			*font_handle_num;
 // STRBUF			*TrainerName[WIFIP2PMATCH_MEMBER_MAX];		// 名前
//  STRBUF			*MsgString;								// メッセージ
  STRBUF*         pExpStrBuf;
  STRBUF			*TalkString;							// 会話メッセージ用
  STRBUF			*TitleString;							// タイトルメッセージ用
//  STRBUF			*MenuString[4];							// メニューメッセージ用
  STRBUF*         pTemp;        // 入力登録時の一時バッファ
  TIMEICON_WORK* pTimeIcon;
  
  int				MsgIndex;								// 終了検出用ワーク
  BMPMENU_WORK* pYesNoWork;
  void* timeWaitWork;			// タイムウエイトアイコンワーク

  GFL_CLUNIT* clactSet;								// セルアクターセット
  GFL_CLSYS_REND*          renddata;						// 簡易レンダーデータ

  u32 cellRes[12];
  u32 cellResPal;
  u32 cellResAnm;
  GFL_CLWK* pokemonIcon[12];
  
  GFL_CLWK* pTrgra;   //トレーナーの大きい絵
  u32 trRes[3];       //トレーナーのリソース

  GFL_BMPWIN*			MsgWin;									// 会話ウインドウ
  GFL_BMPWIN*			MyInfoWin;								// 自分の状態表示
  GFL_BMPWIN*			MyInfoWinBack;								// タイトル
  GFL_BMPWIN*			SysMsgWin;								// システムウィンドウで描画するもの	えらーや、DWCルールのメッセージ
  GFL_BMPWIN*			ListWin;									// フレンドリスト
  GFL_BMPWIN*			SubListWin;									// 募集するタイプなどを描画するリスト
  GFL_BMPWIN*			MyWin;									// 友達の個人情報表示
  PRINT_UTIL            SysMsgPrintUtil;    // システムウインドウPrintUtil
  PRINT_QUE*            SysMenuQue;
  PRINT_QUE*            SysMsgQue;
  PRINT_STREAM* pStream;
	GFL_TCBLSYS *pMsgTcblSys;

   int cancelEnableTimer;   // キャンセル許可になる為のタイマー
  int localTime;
  int seq;									// 現在の文字入力状態（入力OK/アニメ中）など
  int       endSeq;
  int						nextseq;
  int initSeq;
  int						mode;									// 現在最前面の文字パネル
  int						timer;			// 待ち時間や、フラグなどに使用
  MYSTATUS                *pMyStatus;		// 自分のステータス
  BOOL bInitMessage;		// 初回接続か
  GFL_PROC*		subProc;
  int preConnect;			// 新しく来た友達(-1=なし)
  u16 matchGameMode[WIFIP2PMATCH_MEMBER_MAX];   ///<CNM_WFP2PMF_STATUS
  u16 singleCur[_MENUTYPE_MAX];			// バトルタイプ選択メニューカーソル
  u16 bSingle;				// SINGLEバトル　ダブルバトルスイッチ
  u16 friendNo;			// 今つながっている友達ナンバー
  u16 bRetryBattle;
  u16 GameModeKeep;

  WIFIP2PMATCH_PROC_PARAM* pParentWork;
  WIFI_STATUS targetStatus;  //接続しようとしている人のステータス
  APP_PRINTSYS_COMMON_WORK trgWork;  //キーの制御を行うワーク

	WIFI_MATCHROOM matchroom;	// マッチングルームワーク
	MCR_MOVEOBJ*	p_matchMoveObj[ MATCHROOM_IN_OBJNUM ];
	WIFIP2PMATCH_ICON icon;	// アイコングラフィック
	WIFIP2PMATCH_VIEW view;	// 友達データビューアー
  u32 bgchrSubButton;
  u32 bgchrSubBack;
  TOUCHBAR_WORK* pTouchWork;

  APP_KEYCURSOR_WORK* pKeyCursor;
  
	BOOL friend_request_wait;	// 友達募集中にTRUEになるフラグ
	
	CONNECT_BG_PALANM cbp;		// Wifi接続画面のBGパレットアニメ制御構造体

	u16 brightness_reset;	// _retryでマスター輝度をVBlankでリセットするため
	u16 friend_num;			// P2Pmatch画面初期化タイミングの友達数
 	u16 directmode;			// 直接会話中のゲームモード

  u32 talkwin_m2;
  u32 menuwin_m2;
  u32 menuwin_m1;
  u32 matchRegulation;  //通信で貰う相手側の選択したレギュレーション
	WIFIP2PMATCH_VOL aVol;
  REGULATION_PRINT_MSG* rpm;  // レギュレーションプリントメッセージ
  REGULATION* pRegulation;
  u16 battleMode;  //バトルのモード
  u16 battleRule;  //バトルのルール
  u8 DirectModeNo;    // ダイレクトにはなしかけられている場合、話してきたやつのNO
  u8 DirectMacSet;  //話しかける為のMACをセットした人
  u8 keepGameMode;
  u8 command;   ///< direct進行の為のコマンド
  u8 backupCursor;
  u8 VChatModeOff;  //VCTOFFが呼ばれた
  u8 vchatrev;
  u8 nextSeq;
  u8     mainCur;
  u8     subCur;

  WIFIP2PMATCH_STATE state; //プレイヤーの状況
};

//通信関連の関数
extern void WifiP2PMatchRecvGameStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvMyStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvDirectMode(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvBattleRegulation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvPokeParty(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvVctOff(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);

extern u8* WifiP2PMatchGetPokePartyWork(int netID, void* pWk, int size);
extern void WifiP2PMatchMessage_TimeIconStart(WIFIP2PMATCH_WORK* wk);
extern void WIFIP2PMatch_pokePartyMenuDelete( WIFIP2PMATCH_WORK* wk );



