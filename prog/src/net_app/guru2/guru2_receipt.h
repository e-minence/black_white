//=============================================================================
/**
 * @file    guru2_receipt.h
 * @brief   ぐるぐる交換　受付
 * @author  Akito Mori(移植） / kagaya 
 * @data    2010.01.20
 */
//=============================================================================
#ifndef GURU2_RECEIPT_H
#define GURU2_RECEIPT_H

#include "guru2_local.h"

//機能ボタン用定義
#define FUNCBUTTON_NUM  ( 7 )     // 機能ボタンの数
#define START_WORDPANEL ( 0 )   // 最初の文字入力パネルの番号（０＝ひらがな）

// 接続人数MAXは５人
#define RECORD_CORNER_MEMBER_MAX  ( 5 )

// CLACTで定義しているセルが大きすぎてサブ画面に影響がでてしまうので離してみる
#define NAMEIN_SUB_ACTOR_DISTANCE   (256*FX32_ONE)

// CellActorに処理させるリソースマネージャの種類の数（＝マルチセル・マルチセルアニメは使用しない）
#define CLACT_RESOURCE_NUM  (  4 )
#define NAMEIN_OAM_NUM      ( 14 )
#define PLAYER_OAM_NUM      (  5 )

// メッセージ表示後のWAIT
#define RECORD_CORNER_MESSAGE_END_WAIT  ( 60 )

#define ERRNUM_TIMEOUT  (10*30)  //PL:0632 err_numが変化しないためシーケンスが進まないバグパッチ修正

// 文字パネルの遷移用
enum{
  RECORD_MODE_INIT  = 0, 
  RECORD_MODE_NEWMEMBER,
  RECORD_MODE_NEWMEMBER_END,

  RECORD_MODE,

  RECORD_MODE_END_SELECT,
  RECORD_MODE_END_SELECT_WAIT,
  RECORD_MODE_END_SELECT_ANSWER_WAIT,
  RECORD_MODE_END_SELECT_ANSWER_OK,
  RECORD_MODE_END_SELECT_ANSWER_NG,
  RECORD_MODE_END_CHILD,
  RECORD_MODE_END_CHILD_WAIT,
  RECORD_MODE_END_SELECT_PARENT,
  RECORD_MODE_END_SELECT_PARENT_WAIT,
  RECORD_MODE_FORCE_END,
  RECORD_MODE_FORCE_END_MES_WAIT,
  RECORD_MODE_FORCE_END_WAIT,
  RECORD_MODE_FORCE_END_SYNCHRONIZE,
  RECORD_MODE_END_PARENT_ONLY,
  RECORD_MODE_END_PARENT_ONLY_WAIT,
  RECORD_MODE_LOGOUT_CHILD,
  RECORD_MODE_LOGOUT_CHILD_WAIT,
  RECORD_MODE_LOGOUT_CHILD_CLOSE,
  RECORD_MODE_START_SELECT,
  RECORD_MODE_START_SELECT_WAIT,
  RECORD_MODE_START_RECORD_COMMAND,
  RECORD_MODE_RECORD_SEND_DATA,
  RECORD_MODE_MESSAGE_WAIT,
  RECORD_MODE_GURU2_POKESEL_START,
};

#if 0
enum{
  RECORD_SYNCHRONIZE_END=201,
};
#endif

// 上下画面指定定義
#define RES_NUM ( 3 )
#define MAIN_LCD  ( GF_BGL_MAIN_DISP )  // 要は０と
#define SUB_LCD   ( GF_BGL_SUB_DISP )   // １なんですが。
#define CHARA_RES ( 2 )


// シーケンス定義
enum{
  RECORD_EXIST_NO=0,
  RECORD_EXIST_APPEAR_REQ,
  RECORD_EXIST_APPEAR,
  RECORD_EXIST_BYE_REQ,
};

enum{
  GURU2_CLACT_OBJ0_RES_PLTT,   // ユニオンキャラOBJ_PLTT0
  GURU2_CLACT_OBJ1_RES_PLTT,   // ユニオンキャラOBJ_PLTT1
  GURU2_CLACT_OBJ2_RES_PLTT,   // ユニオンキャラOBJ_PLTT2
  GURU2_CLACT_OBJ3_RES_PLTT,   // ユニオンキャラOBJ_PLTT3
  GURU2_CLACT_OBJ4_RES_PLTT,   // ユニオンキャラOBJ_PLTT4
  GURU2_CLACT_OBJ0_RES_CHR,    // ユニオンキャラOBJ_CHARA0
  GURU2_CLACT_OBJ1_RES_CHR,    // ユニオンキャラOBJ_CHARA1
  GURU2_CLACT_OBJ2_RES_CHR,    // ユニオンキャラOBJ_CHARA2
  GURU2_CLACT_OBJ3_RES_CHR,    // ユニオンキャラOBJ_CHARA3
  GURU2_CLACT_OBJ4_RES_CHR,    // ユニオンキャラOBJ_CHARA4
  GURU2_CLACT_RES_CELL,   // ユニオンキャラOBJ_CELL(主人公のアニメだけど）
  GURU2_CLACT_RES_NUM,
};

typedef struct{
  int sw;
  int seq;
  int wait;
  void *paldata;
  NNSG2dPaletteData *palbuf;
}VTRANS_PAL_WORK;

//-------------------------------------
/// 080626 tomoya
//  トレーナID保存領域
//=====================================
typedef union {
  u64   check;
  struct{
    u32   tr_id;
    u32   in;
  }data;
} TR_ID_DATA;

//============================================================================================
//  構造体定義
//============================================================================================
struct GURU2RC_WORK{
  int proc_seq;
  int seq_no;                       //シーケンス
  int end_next_flag;
  GURU2PROC_WORK *g2p;
  GURU2COMM_WORK *g2c;
  
  BOOL        wipe_end;             // ワイプ処理待ち
  VTRANS_PAL_WORK   palwork;        // パレットアニメ用ワーク
  GFL_TCB     *trans_tcb;
  GFL_TCB     *vintr_tcb;           // Vblank処理
  ARCHANDLE   *union_handle;        // ユニオンOBJリソースのファイルハンドル
  
  GFL_FONT    *font;
  WORDSET     *WordSet;             // メッセージ展開用ワークマネージャー
  GFL_MSGDATA *MsgManager;          // 名前入力メッセージデータマネージャー
  STRBUF      *TrainerName[RECORD_CORNER_MEMBER_MAX];   // 名前
  STRBUF      *EndString;           // 文字列「やめる」
  STRBUF      *TalkString;          // 会話メッセージ用
  STRBUF      *TitleString;         // タイトルメッセージ用
  int       MsgIndex;               // 終了検出用ワーク
  
  GFL_CLUNIT  *clUnit;               // セルアクターセット
  u32         resObjTbl[GURU2_CLACT_RES_NUM];
  GFL_CLWK    *MainActWork[NAMEIN_OAM_NUM];   // セルアクターワークポインタ配列

  GFL_BMPWIN    *TrainerNameWin;     // お絵かき画面用BMPウインドウ
  GFL_BMPWIN    *MsgWin;             // 会話ウインドウ
  GFL_BMPWIN    *EndWin;             // やめる
  GFL_BMPWIN    *TitleWin;           // 「レコードコーナー　ぼしゅうちゅう！」など
  GFL_BMPWIN    *YesNoWin[2];        // はい・いいえウインドウのポインタ
  BMPMENU_WORK  *YesNoMenuWork;
  PRINT_QUE     *printQue;
  PRINT_STREAM  *printStream;
  GFL_TCBLSYS   *pMsgTcblSys;       // メッセージ表示用タスクシステム
  PRINT_UTIL    printUtil;

  void*                 FieldObjCharaBuf[2];  // 人物OBJキャラファイルデータ
  NNSG2dCharacterData*  FieldObjCharaData[2]; // 人物OBJキャラデータの実ポインタ        
  void*                 UnionObjPalBuf;       // 人物OBJパレットァイルデータ
  NNSG2dPaletteData*    UnionObjPalData;      // 人物OBJパレットファイルデータ

  int           seq;                  // 現在の文字入力状態（入力OK/アニメ中）など
  int           nextseq;
  int           mode;                 // 現在最前面の文字パネル
  int           wait;
  u8            ConnectCheck[8][2];
  const MYSTATUS *TrainerStatus[RECORD_CORNER_MEMBER_MAX][2];
  int           TrainerReq[RECORD_CORNER_MEMBER_MAX];

  int           saveseq_work;         // セーブシーケンス管理用ワーク

  u16           CursorPal;
  u16           etc_1;
  u8            brush_color;
  u8            brush;

  GURU2COMM_RC_SENDDATA send_data;
  GURU2COMM_RC_SENDDATA recv_data[RECORD_CORNER_MEMBER_MAX];
  u8            record_execute;             // TRUE:レコード混ぜ中
  
  // ----親だけが必要なワーク
  volatile int  connectBackup;              // 接続人数監視用
  int           beacon_flag;
  u32           ridatu_bit;               // 離脱しようとしている子のBit
  u8            status_end;               // TRUE:終了シーケンスへ移行
  s8            limit_mode;               // 接続制限状態
  u8            start_num;                // レコード混ぜ開始人数
  u8            record_send;              // TRUE:レコード送信した
  u32           force_end_bit;            // 通信終了宣言時のメンバー構成bit
  // 子機が必要なワーク
    int err_num_timeout;   ///PL:0632 err_numが変化しないためシーケンスが進まないバグパッチ修正
  s32           err_num;                // 通信終了をみるためのワーク
  s16           ridatu_wait;      // 離脱用のウェイト

  //------080626  tomoya 修正 トレーナの登場と退場チェック用のトレーナID保存領域
  TR_ID_DATA    trainer_id[RECORD_CORNER_MEMBER_MAX][2];

#ifdef PM_DEBUG
  int           frame;                  //
  int           framenum[9][2];             //
#endif
};

//==============================================================================
//  extern
//==============================================================================
extern void Guru2Rc_MainSeqForceChange( GURU2RC_WORK *wk, int seq, u8 id  );
extern void Guru2Rc_MainSeqCheckChange( GURU2RC_WORK *wk, int seq, u8 id  );

#if 0
extern void CommCommandRecordInitialize(void* pWork);
extern void RecordCorner_MainSeqCheckChange( RECORD_WORK *wk, int seq, u8 id );
extern void RecordCorner_MainSeqForceChange( RECORD_WORK *wk, int seq, u8 id  );
extern void RecordCornerTitleChange( RECORD_WORK *wk );
extern int RecordCorner_MyStatusGetNum(void);

extern void MakeSendData(SAVEDATA * sv, RECORD_DATA * send_rec);
extern void MixReceiveData(SAVEDATA * sv, const RECORD_DATA * record);
#endif

#endif

