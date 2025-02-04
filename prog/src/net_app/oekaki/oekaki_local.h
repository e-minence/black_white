//=============================================================================
/**
 * @file  oekaki_local.h
 * @brief お絵かきボード用構造体定義（内部参照用)
 *          
 * @author  Akito Mori
 * @date    2010.01.20
 */
//=============================================================================



#ifndef __OEKAKI_LOCAL_H__
#define __OEKAKI_LOCAL_H__


#include "net_app/union_app.h"
#include "app/app_taskmenu.h"

// 機能ボタン用定義
#define FUNCBUTTON_NUM  ( 7 )     // 機能ボタンの数
#define START_WORDPANEL ( 0 )   // 最初の文字入力パネルの番号（０＝ひらがな）

// 接続人数MAXは５人
#define OEKAKI_MEMBER_MAX ( 5 )


// CLACTで定義しているセルが大きすぎてサブ画面に影響がでてしまうので離してみる
#define NAMEIN_SUB_ACTOR_DISTANCE   (256*FX32_ONE)

// CellActorに処理させるリソースマネージャの種類の数（＝マルチセル・マルチセルアニメは使用しない）
#define CLACT_RESOURCE_NUM    (  4 )
#define NAMEIN_OAM_NUM      ( 14 )


// メッセージ表示後のWAIT
#define OEKAKI_MESSAGE_END_WAIT ( 60 )


// お絵かきボードの幅・高さ
#define OEKAKI_BOARD_W    ( 30 )
#define OEKAKI_BOARD_H    ( 15 )
#define OEKAKI_GRAPHI_SIZE  ( OEKAKI_BOARD_W*OEKAKI_BOARD_H*32 )

// ボタンセルアクターのペン先定義が始まる番号
#define CELL_BRUSH_NO   ( 9 )


// ブラシの大きさ
#define SMALL_BRUSH     ( 0 )
#define MIDDLE_BRUSH    ( 1 )
#define BIG_BRUSH     ( 2 )


// お絵かき禁止フラグ
#define OEKAKI_BAN_NONE   ( 0 )
#define OEKAKI_BAN_OFF    ( 1 )
#define OEKAKI_BAN_ON   ( 2 )

// メインシーケンスの遷移
enum {
  SEQ_IN = 0,
  SEQ_MAIN,
  SEQ_LEAVE,
  SEQ_OUT,
  SEQ_DISCONNECT_WAIT,
};

// サブシーケンスの遷移用
enum{
  OEKAKI_MODE_INIT  = 0, 

  OEKAKI_MODE_NEWMEMBER,
  OEKAKI_MODE_NEWMEMBER_WAIT,
  OEKAKI_MODE_NEWMEMBER_END,

  OEKAKI_MODE,

  OEKAKI_MODE_END_SELECT,
  OEKAKI_MODE_END_SELECT_WAIT,
  OEKAKI_MODE_END_SELECT_ANSWER_WAIT,
  OEKAKI_MODE_END_SELECT_PARENT_CALL,
  OEKAKI_MODE_END_SELECT_ANSWER_OK,
  OEKAKI_MODE_END_SELECT_SEND_OK,
  OEKAKI_MODE_END_SELECT_ANSWER_NG,

  OEKAKI_MODE_END_CHILD,
  OEKAKI_MODE_END_CHILD_WAIT,
  OEKAKI_MODE_END_CHILD_WAIT2,

  OEKAKI_MODE_END_SELECT_PARENT,
  OEKAKI_MODE_END_SELECT_PARENT_WAIT,
  OEKAKI_MODE_END_SELECT_PARENT_SEND_END,

  OEKAKI_MODE_FORCE_END,
  OEKAKI_MODE_FORCE_END_WAIT,
  OEKAKI_MODE_FORCE_END_SYNCHRONIZE,
  OEKAKI_MODE_FORCE_END_WAIT_NOP,

  OEKAKI_MODE_END_PARENT_ONLY,
  OEKAKI_MODE_END_PARENT_ONLY_WAIT,

  OEKAKI_MODE_LOGOUT_CHILD,
  OEKAKI_MODE_LOGOUT_CHILD_WAIT,
  OEKAKI_MODE_LOGOUT_CHILD_CLOSE,
    
};

enum{
  CLACT_RES_M_PLTT,   // メイン画面パレット
  CLACT_RES_M_CHR,    // メイン画面キャラ
  CLACT_RES_M_CELL,   // メイン画面セル
  CLACT_RES_SYS_PLTT,   // メイン画面システムアイコンパレット
  CLACT_RES_SYS_CHR,    // メイン画面システムアイコンキャラ
  CLACT_RES_SYS_CELL,   // メイン画面システムアイコンセル
  CLACT_RES_S_PLTT,   // サブ画面パレット
  CLACT_RES_S_CHR,    // サブ画面キャラ
  CLACT_RES_S_CELL,   // サブ画面セル

  CLACT_RES_OEKAKI_MAX,
};


enum{
  OEKAKI_SYNCHRONIZE_END=200,
};

// 上下画面指定定義
#define BOTH_LCD  ( 2 )
#define MAIN_LCD  ( GFL_BG_MAIN_DISP )  // 要は０と
/*↑[GS_CONVERT_TAG]*/
#define SUB_LCD   ( GFL_BG_SUB_DISP )   // １なんですが。
/*↑[GS_CONVERT_TAG]*/

// BMPWIN指定
enum{
  BMP_NAME1_S_BG0,
  BMP_NAME2_S_BG0,
  BMP_NAME3_S_BG0,
  BMP_NAME4_S_BG0,
  BMP_NAME5_S_BG0,
  BMP_OEKAKI_MAX,
};





// タッチパネル情報構造体
// このデータが通信で送信されます
typedef struct{
  u8 x[2];  // 最大４回分のタッチ座標
  u8 y[2];  // 
  u8 color:3; // 選択中のカラー
  u8 size:3;  // サンプリング成功個数
  u8 brush:2; // 選択中のペン先
  u8 banFlag;  // 操作禁止フラグ(親機からのみ送信）
}TOUCH_INFO;

typedef struct{
  u8   x,y;
  u16  size;
}OLD_TOUCH_INFO;

typedef struct{
  u8  chara[1000];
  u32 _xor;
  u8  no;
  u8  dummy[3];
}OEKAKIG_SPLIT_DATA;

//--------------------------------------------------------------
/**
 * @brief   この構造体の中身を替えたら、必ず通信関数テーブルで
 *          CommRecordCornerEndChildを使用している箇所の送受信サイズを変更すること！！
 *      ファイル：comm_command_record.c, comm_command_oekaki.c, comm_command_field.c
 */
//--------------------------------------------------------------
typedef struct{
  u8 ridatu_id;       ///<離脱者のID
  u8 oya_share_num;   ///<親の持つshareNum
  u8 request;         ///<命令コード
  u8 ridatu_kyoka;    ///<TRUE:離脱OK、FALSE:離脱NG
}COMM_OEKAKI_END_CHILD_WORK;

//====================================================
/// データ送信リクエスト構造体
//====================================================
typedef struct{
    int command;       // 通信コマンド
    u8  id;            // 乱入者ID
    u8  etc[3];
    COMM_OEKAKI_END_CHILD_WORK trans_work;  // 子機離脱宣言用ワーク
} OEKAKI_SEND_REQUEST;


enum{
  OEKAKI_PRINT_UTIL_NAME_WIN0=0,
  OEKAKI_PRINT_UTIL_NAME_WIN1,
  OEKAKI_PRINT_UTIL_NAME_WIN2,
  OEKAKI_PRINT_UTIL_NAME_WIN3,
  OEKAKI_PRINT_UTIL_NAME_WIN4,
  OEKAKI_PRINT_UTIL_BOARD,
  OEKAKI_PRINT_UTIL_MSG,
  OEKAKI_PRINT_UTIL_MAX,
};

#define OEKAKI_FIFO_MAX   ( 100 )


// 親機・子機共通の送信FIFO構造体
// 親機は5人分格納するが、子機は[0]に自分の情報を格納するだけ
typedef struct{
  TOUCH_INFO        fifo[OEKAKI_FIFO_MAX][5]; // 送信失敗のリトライ用のFIFO
  int               start;    // 送信スタートポイント
  int               end;      // タッチパネル情報を格納する位置
}TOUCH_FIFO;

//============================================================================================
//  構造体定義
//============================================================================================

struct OEKAKI_WORK{
  BOOL      wipe_end;
//  OEKAKI_PARAM  *param;
  PICTURE_PARENT_WORK *param;
  GFL_TCB       *vblankTcb;

  GFL_FONT    *font;
  WORDSET     *WordSet;                   // メッセージ展開用ワークマネージャー
  GFL_MSGDATA *MsgManager;            // 名前入力メッセージデータマネージャー
  STRBUF      *TrainerName[OEKAKI_MEMBER_MAX];    // 名前
  STRBUF      *EndString;                 // 文字列「やめる」
  STRBUF      *TalkString;                // 会話メッセージ用
  int       MsgIndex;                     // 終了検出用ワーク

  GFL_CLUNIT              *clUnit;      // セルアクターセット
  u32                     resObjTbl[ CLACT_RES_OEKAKI_MAX ];
//  CLACT_U_EASYRENDER_DATA renddata;       // 簡易レンダーデータ
//  CLACT_U_RES_MANAGER_PTR resMan[CLACT_RESOURCE_NUM];       // リソースマネージャ
//  CLACT_U_RES_OBJ_PTR   resObjTbl[BOTH_LCD][CLACT_RESOURCE_NUM];// リソースオブジェテーブル
//  CLACT_HEADER      clActHeader_m;        // セルアクターヘッダー
//  CLACT_HEADER      clActHeader_s;        // セルアクターヘッダー
  GFL_CLWK      *MainActWork[NAMEIN_OAM_NUM];        // セルアクターワークポインタ配列
  GFL_CLWK      *SubActWork[NAMEIN_OAM_NUM];       // セルアクターワークポインタ配列
  GFL_CLWK      *ButtonActWork[12];  // ボタンアクターポインタ
  GFL_CLWK      *EndIconActWork;     // 「×」APP共通アイコン

  GFL_BMPWIN    *TrainerNameWin[BMP_OEKAKI_MAX];     // お絵かき画面用BMPウインドウ
  GFL_BMPWIN    *OekakiBoard;
  GFL_BMPWIN    *MsgWin;                // 会話ウインドウ
  GFL_BMPWIN    *EndWin;                // やめる
  GFL_BMPWIN    *YesNoWin[2];           // はい・いいえウインドウのポインタ
  PRINT_UTIL    printUtil[OEKAKI_PRINT_UTIL_MAX];
  PRINT_QUE     *printQueName;          // 上画面名前リスト用のプリントキュー
  PRINT_QUE     *printQue;
  PRINT_STREAM  *printStream;           // 
  GFL_TCBLSYS   *pMsgTcblSys;           // メッセージ表示用タスクシステム

  // APPMENU関連
  APP_TASKMENU_RES   *app_res;            // はい・いいえメニューリソース
  APP_TASKMENU_WORK  *app_menuwork;       // はい・いいえメニューワーク
  APP_TASKMENU_ITEMWORK yn_menuitem[2];   // はい・いいえメニューアイテム

  // サブシーケンス制御
  int           proc_seq;                 // MainProcのシーケンスを監視するためだけにある
                                          // （サブシーケンスは書き換えない）
  int           seq;                      // 現在のサブシーケンス状態
  int           next_seq;                 // 次のシーケンスは何か？
  int           mode;                     // 現在最前面の文字パネル
  int           wait;
  int           connectBackup;            // 直前の接続人数
  int           shareNum;                 // 画像送信済みの人数
  int           shareBit;                 // 画像送信済みの人数(ビット)
  int           newMemberId;              // 乱入してきた人のID
  
  u8            ConnectCheck[8][2];
  const MYSTATUS *TrainerStatus[8][2];

  u8            SendBoardGraphic[0x4000];
  u16           CursorPal;
  u8            brush_color;
  u8            brush;

  TOUCH_INFO        MyTouchResult;                      // 自分のサンプリング結果（これは送信するだけ
  TOUCH_INFO        AllTouchResult[OEKAKI_MEMBER_MAX];  // 通信で取得したサンプリング結果（このデータで描画する
  OLD_TOUCH_INFO    OldTouch[OEKAKI_MEMBER_MAX];        // 前回からのポイント履歴
  TP_ONE_DATA       oldTpData;            // 前回のタッチデータ

  u8            *lz_buf;                   // 圧縮画像格納領域
  int           send_num;
  u8            canvas_buf[OEKAKI_GRAPHI_SIZE];
  OEKAKIG_SPLIT_DATA    send_buf;
  OEKAKIG_SPLIT_DATA    split_temp[OEKAKI_MEMBER_MAX];
  OEKAKI_SEND_REQUEST   send_req;

  s32           err_num;                  // 通信終了をみるためのワーク
  u32           ridatu_bit;               // 離脱しようとしている子のBit
  u16           oya_share_num;            // 親が持っているshareNumをもらう
  s16           ridatu_wait;              // 離脱用のウェイト
  u16           status_end;               // TRUE:終了シーケンスへ移行
  u16           force_end;                // TRUE:強制終了モード中

  TOUCH_FIFO    TouchFifo;                // タッチ情報送信FIFO
  
  // ----親だけが必要なワーク

  TOUCH_INFO    ParentTouchResult[5];     // 子機から受信した頂点情報を送信するためのバッファ
  int           banFlag;
  int           yesno_flag;               // 現在「はい・いいえ」画面を呼び出し中
  int           firstChild;               // 一番最初にやってきたときに子機から乱入宣言されても無視
  int           ireagalJoin;              // お絵かき終了時に乱入されたか
  int           bookJoin;                 // 乱入予約フラグ
  int           joinBit;                  // 乱入者ビット
#ifdef PM_DEBUG
  int           frame;                    // 
  int           framenum[9][2];           // 
#endif
};


enum{
  COEC_REQ_RIDATU_CHECK,  ///<離脱確認
  COEC_REQ_RIDATU_EXE,    ///<離脱実行
};


extern void OekakiBoardCommSendPokeData(int netID, POKEPARTY *party, int no);
extern void OekakiBoardCommSend(int netID, int command, int pos);
extern void OekakiBoard_MainSeqCheckChange( OEKAKI_WORK *wk, int seq, u8 id );
extern void OekakiBoard_MainSeqForceChange( OEKAKI_WORK *wk, int seq, u8 id  );
extern void Oekaki_SendDataRequest( OEKAKI_WORK *wk, int command, int id );

extern void OekakiTouchFifo_Init( TOUCH_FIFO *touchFifo );
extern void OekakiTouchFifo_AddEnd( TOUCH_FIFO *touchFifo );
extern void OekakiTouchFifo_AddStart( TOUCH_FIFO *touchFifo );
extern void OekakiTouchFifo_Set( TOUCH_INFO *myResult, TOUCH_FIFO *touchFifo, int id );
extern void OekakiTouchFifo_Get( TOUCH_INFO *myResult, TOUCH_FIFO *touchFifo );
extern void OekakiTouchFifo_GetParent( TOUCH_INFO *myResult, TOUCH_FIFO *touchFifo );
extern void OekakiTouchFifo_AddEndParent( TOUCH_FIFO *touchFifo );


#endif
