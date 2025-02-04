//============================================================================================
/**
 * @file   trcard_sys.h
 * @bfief  トレーナーカード画面システムヘッダ
 * @author Akito Mori / Nozomu Saito (WB移植は有泉くん)
 * @date   10.03.09
 */
//============================================================================================
#ifndef _TRCARD_SYS_H_
#define _TRCARD_SYS_H_

#include "system/bmp_menu.h"
#include "system/pms_data.h"
#include "app/trainer_card.h"
#include "trcard_obj.h"
#include "trcard_snd.h"
#include "system/pms_draw.h"

#define TRC_FRAME_RATE (60)

#define TRC_SCREEN_WIDTH  (32)
//#define TR_CARD_WIN_MAX   (14)
#define TR_CARD_PARAM_MAX (7)
#define TR_CPRM_STR_MAX   (26)

#define TR_CARD_BADGE_LV_MAX  (4)   //バッジの最大レベル
#define TR_BADGE_NUM_MAX  (16)


#define CALL_NONE     ( 0 ) ///< トレーナーカード通常終了
#define CALL_PMSINPUT ( 1 ) ///< トレーナカードから簡易会話呼び出し
#define CALL_BADGE    ( 2 ) ///< トレーナーカードからバッヂ確認画面呼び出し
#define CALL_CARD     ( 3 ) ///< トレーナーカードからバッヂ確認画面呼び出し

//パレット使用リスト
enum{
  CASE_BD_PAL   = 0,
  YNWIN_PAL     = 11, //2本使う
  TALKWIN_PAL   = 13,
  SYS_FONT_PAL  = 14,
  TR_FONT_PAL   = 15,
  BACK_FONT_PAL = 15,
};


// スクロールするスコアリストの描画内容定義
enum{
  SCORE_LINE_ID=0,
  SCORE_LINE_MONEY,
  SCORE_LINE_START_DATE_1,
  SCORE_LINE_START_DATE_2,
  SCORE_LINE_FIRST_CHAMPION_1,
  SCORE_LINE_FIRST_CHAMPION_2,
  SCORE_LINE_COMM_NUM,
  SCORE_LINE_COMM_BATTLE_NUM,
  SCORE_LINE_COMM_WIN_NUM,
  SCORE_LINE_COMM_LOSE_NUM,
  SCORE_LINE_COMM_TRADE_NUM,
  SCORE_LINE_ENCOUNT_NUM_1,
  SCORE_LINE_ENCOUNT_NUM_2,
  SCORE_LINE_TRAINER_ENCOUNT_NUM_1,
  SCORE_LINE_TRAINER_ENCOUNT_NUM_2,
  SCORE_LINE_SURETIGAI_NUM,
  SCORE_LINE_FEELING_CHECK_NUM,
  SCORE_LINE_G_POWER,
  SCORE_LINE_MUSICAL_NUM,
  SCORE_LINE_POKESHIFTER_HIGH,
  SCORE_LINE_TRIALHOUSE_HIGH,
//  SCORE_LINE_TRIALHOUSE_RANK,
  SCORE_LINE_MAX,
};

//=====================================
// BMPWIN配列指定用定義
//=====================================
typedef enum{
  // 表
  TRC_BMPWIN_TR_NAME=0,     // トレーナー名
  TRC_BMPWIN_TRAINER_TYPE,// 自分で選択できるトレーナータイプ名
  TRC_BMPWIN_PERSONALITY, // 自分で選択できるせいかく名
  TRC_BMPWIN_BOOK,        // ずかん
  TRC_BMPWIN_PLAY_TIME,   // プレイ時間
  TRC_BMPWIN_PMSWORD,      // 簡易会話

  // 裏
  TRC_BMPWIN_SCORE_LIST, // スクロールできる各種情報ウインドウ

  // メッセージ
  TRC_BMPWIN_MSG,         // サイン画面ウインドウ
  TR_CARD_WIN_MAX,
}TRC_BMPWIN;

enum{
  TR_CARD_CREDIT_KAI=0,
  TR_CARD_CREDIT_YEN,
  TR_CARD_CREDIT_YYMMDD,
  TR_CARD_CREDIT_YYMMDDHHMM,
  TR_CARD_CREDIT_HIKI,
  TR_CREDIT_STR_MAX,
};


typedef struct TR_SCRUCH_SND_tag
{
  s8 OldDirX;
  s8 OldDirY;
  s8 DirX;
  s8 DirY;
  u8 Snd;
}TR_SCRUCH_SND;

// タッチパネル情報構造体
typedef struct{
  u8  x;      // 最大8回分のタッチ座標
  u8  y;      // 
  u8  brush;  // 0=ペン, 1=消しゴム
  u8  on;     // 入力ありかどうか
}TOUCH_INFO;


typedef struct TR_CARD_WORK_tag
{
  HEAPID heapId;
  GFL_BMPWIN  *win[TR_CARD_WIN_MAX];      // BMPウィンドウデータ（通常）

  TRCARD_CALL_PARAM* tcp; ///<カード呼び出しパラメータ
  TR_CARD_DATA *TrCardData;
  u8 TrSignData[SIGN_SIZE_X*SIGN_SIZE_Y*64];  //転送するので４バイト境界の位置に置くこと
  void *TrArcData;              //トレーナーグラフィックアーカイブデータ
  NNSG2dCharacterData* TrCharData;      //トレーナーキャラデータポインタ
  BOOL is_back;
  BOOL brushOK;
  
  fx32 CardScaleX;
  fx32 CardScaleY;
  fx32 CoverScaleY;
  int  RotateCount;
  int  CardCenterX;
  int  CardCenterY;
  int  CardOffsetX;
  int  CardOffsetY;
  u16  ScaleSide;     // カードの左右どっちを拡大しているか？( 0:左 1:右）
  u16  ScaleMode;     // 拡大縮小モード状態

//  int touch;
  int  key_mode;
  BOOL rc_cover;
  int  BeforeX;
  int  BeforeY;
  TR_CARD_OBJ_WORK ObjWork;
//  int RevSpeed;
  int  Scroll;
  BOOL ButtonPushFlg;
  SND_BADGE_WORK SndBadgeWork;
  TR_SCRUCH_SND  ScruchSnd;
  
  void             *TrScrnArcData;
  NNSG2dScreenData *ScrnData;
  void             *pScrnBCase;
  NNSG2dScreenData *pSBCase;
  void             *pPalBuf;
  NNSG2dPaletteData *pPalData;
  int               blink_count;  // 
  
//  TOUCH_SW_SYS *ynbtn_wk;   // YesNo選択ボタンワーク
  GFL_TCB      *vblankFunc;
  GFL_TCBLSYS  *vblankTcblSys;

  GFL_FONT     *fontHandle;
  GFL_MSGDATA  *msgMan;
  WORDSET      *wordset;
  
  STRBUF  *PlayTimeBuf;   //プレイ時間用文字列バッファ
  STRBUF  *SecBuf;        //プレイ時間コロン用文字列バッファ
  STRBUF  *TmpBuf;        //テンポラリ文字列
  STRBUF  *DigitBuf;      //カード数値パラメータ展開用文字列バッファ
  STRBUF  *ExpBuf[3];     //<説明メッセージ文字列バッファ
  STRBUF  *SignBuf[2];    //サイン説明メッセージ文字列バッファ
  STRBUF  *CPrmBuf[TR_CPRM_STR_MAX];  //カードパラメータ展開用文字列バッファ
  STRBUF  *CreditBuf[TR_CREDIT_STR_MAX];  // カードパラメータ展開用(回数単位表示用）文字列バッファ

  PMS_DATA      pms;          ///< 簡易会話
  PMS_DRAW_WORK *PmsDrawWork; ///< 簡易会話描画ワーク

  u8 badge[TR_BADGE_NUM_MAX];
  u8 IsOpen;
  u8 Counter;
  u8 sub_seq;
  
  u8 SignAnimeWait;   // トレーナーサインアニメ用ワーク
  u8 SignAnimePat;    // トレーナーサインアニメのパターン指定（SIGN_ALL,SIGN_LEFT,SIGN_RIGHT)
  u8 SecCount;        // 秒表示のためのカウンタ
  u8 isClear:1;       // 殿堂入りしているか否か？
  u8 isComm:1;        // 通信中か否か？
  u8 aff_req:1;       // アフィン変換実行リクエスト
  u8 drug_f:1;        // ドラッグフラグ
  u8 Dummy:5;         // パディング
  
  PRINT_QUE     *printQue;
  PRINT_STREAM  *printHandle;
  u8  win_type;  ///<ウィンドウタイプ
  u8  msg_spd;   ///<メッセージスピード
  u8  scrl_ct;   ///<背景スクロールカウンタ
  u32 tp_x;
  u32 tp_y;
  u8  draw_seq;

  s16 scrol_point;      // スコア表示スクロールポイント
  s16 old_scrol_point;  // スクロールポイント過去位置
  s16 touch_sy;         // タッチスタートY座標
  s16 scrol_start;      // スクロールスタート位置

  TOUCH_INFO      MyTouchResult;  
  TOUCH_INFO      AllTouchResult; // 通信で取得したサンプリング結果（このデータで描画する
  TOUCH_INFO      OldTouch;       // 前回からのポイント履歴
  u8  pen;                        // サインの色(0:黒 1:白）
  u8  card_list_col;              // カード裏面のリスト描画色を黄色にする（0:黒 1:黄色）
  u8  scroll_se_wait;             // 裏面スクロールのSE用ウェイト
  u8  sign_se_wait;

  u8  score_list[SCORE_LINE_MAX]; ///< 表示する項目を並べた配列
  u8  score_max;                  ///< 現在表示できる項目の最大数
  u8  b_touch_scroll; ///< 裏面スクロール開始フラグ
  u8  b_touch_sign;   ///< 裏面サイン開始フラグ

}TR_CARD_WORK;

extern GFL_PROC_RESULT TrCardProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT TrCardProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT TrCardProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

#endif //_TRCARD_SYS_H_


