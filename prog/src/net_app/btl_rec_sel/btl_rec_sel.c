//============================================================================
/**
 *  @file   btl_rec_sel.c
 *  @brief  通信対戦後の録画選択画面
 *  @author Koji Kawada
 *  @data   2010.01.27
 *  @note   
 *
 *  モジュール名：BTL_REC_SEL
 */
//============================================================================
//#define OFFLINE_TEST


// インクルード
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "gamesystem/msgspeed.h"
#include "system/palanm.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "savedata/battle_rec.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "poke_tool/poke_tool.h"
#include "pokeicon/pokeicon.h"
#include "ui/ui_easy_clwk.h"
#include "net/network_define.h"
#include "item/itemsym.h"
#include "app/app_nogear_subscreen.h"
#include "system/time_icon.h"
#include "sound/pm_sndsys.h"
#include "system/net_err.h"

#include "btl_rec_sel_graphic.h"
#include "net_app/btl_rec_sel.h"


// アーカイブ
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_b_record.h"
#include "c_gear.naix"
#include "batt_rec_gra.naix"
// サウンド


// オーバーレイ
FS_EXTERN_OVERLAY(ui_common);


//=============================================================================
/**
*  定数定義
*/
//=============================================================================
#define HEAP_SIZE              (0x50000)               ///< ヒープサイズ

// BGフレーム
#define BG_FRAME_M_BACK              (GFL_BG_FRAME2_M)        // プライオリティ2
#define BG_FRAME_M_TIME              (GFL_BG_FRAME3_M)        // プライオリティ1
#define BG_FRAME_M_TEXT              (GFL_BG_FRAME1_M)        // プライオリティ0

// BGパレット
// 本数
enum
{
  BG_PAL_NUM_M_BACKDROP      = 0,  // 使用せず
  BG_PAL_NUM_M_TEXT_FONT     = 0,  // 使用せず
  BG_PAL_NUM_M_TEXT_FRAME    = 1,
  BG_PAL_NUM_M_YN            = 1,
  BG_PAL_NUM_M_BACK          = 10,  // 0を含めて数えて9がフォント用のパレット<白(1,2)>,<赤(3,4)>
                                    // 黒文字白背景は(黒11,10,10(294b)),(灰20,20,21(5694)),(白31,31,31(7fff))
  BG_PAL_NUM_M_DEFAULT_TEXT  = 1,  // デフォルトパレット(これで1本パレットを占めておかないと、タイマーアイコンの色がおかしくなってしまう)
};
// 位置
enum
{
  BG_PAL_POS_M_BACK          = 0                                                        ,  // 0
  BG_PAL_POS_M_BACKDROP      = BG_PAL_POS_M_BACK         + BG_PAL_NUM_M_BACK            ,  // 10  // 使用せず
  BG_PAL_POS_M_TEXT_FONT     = BG_PAL_POS_M_BACKDROP     + BG_PAL_NUM_M_BACKDROP        ,  // 10  // 使用せず
  BG_PAL_POS_M_TEXT_FRAME    = BG_PAL_POS_M_TEXT_FONT    + BG_PAL_NUM_M_TEXT_FONT       ,  // 10 
  BG_PAL_POS_M_YN            = BG_PAL_POS_M_TEXT_FRAME   + BG_PAL_NUM_M_TEXT_FRAME      ,  // 11  // 使用せず
  BG_PAL_POS_M_DEFAULT_TEXT  = BG_PAL_POS_M_YN           + BG_PAL_NUM_M_YN              ,  // 12  // ここから空き
  BG_PAL_POS_M_MAX           = BG_PAL_POS_M_DEFAULT_TEXT + BG_PAL_NUM_M_DEFAULT_TEXT    ,  // 13  // ここから空き

  BG_PAL_POS_M_BACK_FONT     = 9,  // BACKのパレットのここをフォントのパレットとして使う
};

// OBJパレット
// 本数
enum
{
  OBJ_PAL_NUM_M_PI           = 3,
};
// 位置
enum
{
  OBJ_PAL_POS_M_PI           = 0,
  OBJ_PAL_POS_M_MAX          = OBJ_PAL_POS_M_PI         + OBJ_PAL_NUM_M_PI           ,  // 3  // ここから空き
};
// 本数
enum
{
  OBJ_PAL_NUM_S_             = 1,
};
// 位置
enum
{
  OBJ_PAL_POS_S_MAX          = 0                                                      ,      // ここから空き
};

// ストリームテキストウィンドウのウィンドウ内の背景色
#define TEXT_WININ_BACK_COLOR        (15)

// フェード
#define FADE_IN_WAIT           (2)         ///< フェードインのスピード
#define FADE_OUT_WAIT          (2)         ///< フェードアウトのスピード

#define INSIDE_FADE_OUT_WAIT   (-2)//(0)         ///< 内部で表示を切り替える際のフェードアウトのスピード
#define INSIDE_FADE_IN_WAIT    (-2)//(0)         ///< 内部で表示を切り替える際のフェードインのスピード

typedef enum
{
  FADE_TYPE_OUTSIDE,  // 呼び出し元との接続のフェード
  FADE_TYPE_INSIDE,   // 内部で表示を切り替える際のフェード(パレットフェードを使用する)
}
FADE_TYPE;

// ポケアイコン
enum
{
  PI_POS_R,
  PI_POS_L,
  PI_POS_MAX,
};
#define PI_PARTY_NUM      (6)      // ポケアイコンの個数

// 固定テキスト
enum
{
  FIX_PRE, 
  FIX_PRE_TITLE, 
  FIX_SEL_TIME,
  FIX_TIME,
  FIX_MAX,
};

// fix timeの状態
typedef enum
{
  FT_STATE_NOT_DISP,    // 非表示中
  FT_STATE_WHITE_DISP,  // 白文字表示中(別途表示している数字も覚えておく)
  FT_STATE_RED_DISP,    // 赤文字表示中(別途表示している数字も覚えておく)
  FT_STATE_MAX,
}
FT_STATE;

// 1秒間のフレーム数
//#define FPS (60)//(30)
// GFL_UI_GetFrameRate(), GFL_UI_FRAMERATE_30, GFL_UI_FRAMERATE_60 を使用するようにした。

// 待ち時間
#define COUNT_TIME_SEC_MAX (60)  // second

// テキストを表示した後の待ち時間
#define TEXT_STREAM_WAIT_COUNT_DEFAULT (120)  // 60fpsのときのフレーム数

// 文字数
#define STRBUF_FIX_TIME_LENGTH       (  8)  // ??:??
#define STRBUF_LENGTH                (256)  // この文字数で足りるかbuflen.hで要確認

// シーケンス
enum
{
  SEQ_FADE_INIT,
  SEQ_FADE,
  SEQ_QA_INIT,
  SEQ_QA_QUES,
  SEQ_STM_INIT,
  SEQ_QA_ANS_REC,
  SEQ_QA_ANS_NOREC,
  SEQ_QA_ANS_PRE,
  SEQ_PRE_SHOW_ON,
  SEQ_PRE_SHOW_OFF,
  SEQ_SAVE_INIT,
  SEQ_SAVE_INIT2,
  SEQ_SAVE,
  SEQ_SAVEOK,
  SEQ_SAVENG,
  SEQ_WAIT_INIT,
  SEQ_WAIT_INIT2,
  SEQ_WAIT,
  SEQ_END,
};

// バトルモードごとの情報
static const u8 battle_mode_info_tbl[BATTLE_MODE_MAX][2] =  // include/savedata/battle_rec.h  BATTLE_MODE
{
  // { str_id,  0=2人  1=4人 }
                              // //コロシアム：シングル
  { msg_record_title01, 0 },  // BATTLE_MODE_COLOSSEUM_SINGLE_FREE,
  { msg_record_title02, 0 },  // BATTLE_MODE_COLOSSEUM_SINGLE_50,
  { msg_record_title03, 0 },  // BATTLE_MODE_COLOSSEUM_SINGLE_FREE_SHOOTER,
  { msg_record_title04, 0 },  // BATTLE_MODE_COLOSSEUM_SINGLE_50_SHOOTER,
                              // //コロシアム：ダブル
  { msg_record_title05, 0 },  // BATTLE_MODE_COLOSSEUM_DOUBLE_FREE,
  { msg_record_title06, 0 },  // BATTLE_MODE_COLOSSEUM_DOUBLE_50,
  { msg_record_title07, 0 },  // BATTLE_MODE_COLOSSEUM_DOUBLE_FREE_SHOOTER,
  { msg_record_title08, 0 },  // BATTLE_MODE_COLOSSEUM_DOUBLE_50_SHOOTER,
                              // //コロシアム：トリプル
  { msg_record_title09, 0 },  // BATTLE_MODE_COLOSSEUM_TRIPLE_FREE,
  { msg_record_title10, 0 },  // BATTLE_MODE_COLOSSEUM_TRIPLE_50,
  { msg_record_title11, 0 },  // BATTLE_MODE_COLOSSEUM_TRIPLE_FREE_SHOOTER,
  { msg_record_title12, 0 },  // BATTLE_MODE_COLOSSEUM_TRIPLE_50_SHOOTER,
                              // //コロシアム：ローテーション
  { msg_record_title13, 0 },  // BATTLE_MODE_COLOSSEUM_ROTATION_FREE,
  { msg_record_title14, 0 },  // BATTLE_MODE_COLOSSEUM_ROTATION_50,
  { msg_record_title15, 0 },  // BATTLE_MODE_COLOSSEUM_ROTATION_FREE_SHOOTER,
  { msg_record_title16, 0 },  // BATTLE_MODE_COLOSSEUM_ROTATION_50_SHOOTER,
                              // //コロシアム：マルチ
  { msg_record_title17, 1 },  // BATTLE_MODE_COLOSSEUM_MULTI_FREE,
  { msg_record_title18, 1 },  // BATTLE_MODE_COLOSSEUM_MULTI_50,
  { msg_record_title19, 1 },  // BATTLE_MODE_COLOSSEUM_MULTI_FREE_SHOOTER,
  { msg_record_title20, 1 },  // BATTLE_MODE_COLOSSEUM_MULTI_50_SHOOTER,
                              // //地下鉄
  { msg_record_title21, 0 },  // BATTLE_MODE_SUBWAY_SINGLE,     //WIFI DL含む
  { msg_record_title22, 0 },  // BATTLE_MODE_SUBWAY_DOUBLE,
  { msg_record_title23, 1 },  // BATTLE_MODE_SUBWAY_MULTI,      //NPC, COMM, WIFI
                              // //ランダムマッチ：フリー
  { msg_record_title24, 0 },  // BATTLE_MODE_RANDOM_FREE_SINGLE,
  { msg_record_title25, 0 },  // BATTLE_MODE_RANDOM_FREE_DOUBLE,
  { msg_record_title26, 0 },  // BATTLE_MODE_RANDOM_FREE_TRIPLE,
  { msg_record_title27, 0 },  // BATTLE_MODE_RANDOM_FREE_ROTATION,
  { msg_record_title28, 0 },  // BATTLE_MODE_RANDOM_FREE_SHOOTER,
                              // //ランダムマッチ：レーティング
  { msg_record_title29, 0 },  // BATTLE_MODE_RANDOM_RATING_SINGLE,
  { msg_record_title30, 0 },  // BATTLE_MODE_RANDOM_RATING_DOUBLE,
  { msg_record_title31, 0 },  // BATTLE_MODE_RANDOM_RATING_TRIPLE,
  { msg_record_title32, 0 },  // BATTLE_MODE_RANDOM_RATING_ROTATION,
  { msg_record_title33, 0 },  // BATTLE_MODE_RANDOM_RATING_SHOOTER,
                              // //大会
  { msg_record_title34, 0 },  // BATTLE_MODE_COMPETITION_SINGLE,
  { msg_record_title35, 0 },  // BATTLE_MODE_COMPETITION_SINGLE_SHOOTER,
  { msg_record_title36, 0 },  // BATTLE_MODE_COMPETITION_DOUBLE,
  { msg_record_title37, 0 },  // BATTLE_MODE_COMPETITION_DOUBLE_SHOOTER,
  { msg_record_title38, 0 },  // BATTLE_MODE_COMPETITION_TRIPLE,
  { msg_record_title39, 0 },  // BATTLE_MODE_COMPETITION_TRIPLE_SHOOTER,
  { msg_record_title40, 0 },  // BATTLE_MODE_COMPETITION_ROTATION,
  { msg_record_title41, 0 },  // BATTLE_MODE_COMPETITION_ROTATION_SHOOTER,
};

// パレットフェード
#define PF_TCBSYS_TASK_MAX  (2)
#define PF_BG_M_NUM   (14)  // 0<= <14  // 何かに使うかもしれない最後のパレット以外全て
#define PF_OBJ_M_NUM  (14)  // 0<= <14  // 通信アイコン以外全て
typedef enum
{
  PF_REQ_NONE,
  PF_REQ_FADE_IN,
  PF_REQ_FADE_OUT,
}
PF_REQ;
typedef enum
{
  PF_STATE_BLACK,
  PF_STATE_FADE_IN,
  PF_STATE_COLOR,
  PF_STATE_FADE_OUT,
}
PF_STATE;


//=============================================================================
/**
*  構造体宣言
*/
//=============================================================================
// ポケアイコン
typedef struct
{
  u32                monsno;  // 0のときなし
  u32                formno;
  u32                sex;
  BOOL               egg;
  UI_EASY_CLWK_RES   res;
  GFL_CLWK*          clwk;
  u8                 x;
  u8                 y;
  u8                 anim;
  u8                 soft_pri;
}
PI_DATA;

//-------------------------------------
/// PROC ワーク
//=====================================
typedef struct
{
  // グラフィック、フォントなど
  HEAPID                      heap_id;
  BTL_REC_SEL_GRAPHIC_WORK*   graphic;
  GFL_FONT*                   font;

  PRINT_QUE*                  print_que;               // text_winin_bmpwin
  PRINT_QUE*                  print_que_fix_pre;       // FIX_PRE, FIX_PRE_TITLE
  PRINT_QUE*                  print_que_fix_sel_time;  // FIX_SEL_TIME
  PRINT_QUE*                  print_que_fix_time;      // FIX_TIME

  // シーケンス処理用 
  int                         fade_next_seq;
  int                         fade_mode;
  int                         fade_start_evy;
  int                         fade_end_evy;
  int                         fade_wait;
  FADE_TYPE                   fade_type;

  int                         qa_next_seq;
  u32                         qa_str_id;
  BOOL                        qa_non;
 
  int                         stm_next_seq;
  u32                         stm_str_id;

  int                         next_seq;

  // VBlank中TCB
  GFL_TCB*                    vblank_tcb;

  // バトルレコーダーの有無
  BOOL                        b_battle_recorder;  // TRUEのときバトルレコーダーを所持している
  BOOL                        b_end_immediately;  // 直ちに終了させるときTRUE

  // メッセージ
  GFL_MSGDATA*                msgdata_rec;

  // ストリームテキストウィンドウ
  PRINT_STREAM*               text_stream;
  BOOL                        text_stream_need_input;  // ストリームテキスト表示が完了したときに入力を待って終了する場合TRUE
  u32                         text_stream_wait_count;  // ストリームテキスト表示が完了したときにカウントダウンして0になるまで待つ
                                                       // 60fps時のフレーム数を入れておけばいい
  GFL_TCBLSYS*                text_tcblsys;
  GFL_BMPWIN*                 text_winin_bmpwin;
  GFL_BMPWIN*                 text_dummy_bmpwin;      ///< 0番のキャラクターを1x1でつくっておく
  GFL_ARCUTIL_TRANSINFO       text_winfrm_tinfo;
  STRBUF*                     text_strbuf;
  BOOL                        text_winin_trans;  // text_winin_bmpwinの転送が済んでいたらTRUE

  // はい・いいえウィンドウ
  BMPWIN_YESNO_DAT            yn_dat;
  BMPMENU_WORK*               yn_wk;

  // 固定テキスト
  GFL_BMPWIN*                 fix_bmpwin[FIX_MAX];
  BOOL                        fix_finish[FIX_MAX];  // 転送済みのときTRUE(転送するものがないときもTRUE)、つまり転送しなければならないときのみFALSE。
  u32                         fix_frame;
  u32                         fix_wait_count;
  BOOL                        fix_pause;   // TRUEのとき一時停止中
  BOOL                        fix_timeup;  // TRUEのとき時間切れ
  // fix timeの状態
  FT_STATE                    ft_state;
  u16                         ft_sec;  // 表示している数字

  // ポケアイコン
  PI_DATA                     pi_data[PI_POS_MAX][PI_PARTY_NUM];

  // バトルモードから表示するテキストID、ポケアイコンの並べ方を決める
  u32                         battle_mode_str_id;
  BOOL                        battle_mode_arrange_two;

  // BattleRec BATTLE_REC
  BATTLE_REC_SAVEDATA*        battle_rec_savedata;
  BATTLE_MODE                 battle_rec_mode;
  int                         battle_rec_new_mode;
  u16                         battle_rec_new_work0;
  u16                         battle_rec_new_work1;
  SAVE_RESULT                 battle_rec_new_save_result;

  // BG Main
  GFL_ARCUTIL_TRANSINFO       bg_m_tinfo;

  // パレットフェード
  UI_EASY_CLWK_RES            pf_dummy_pi_res;
  GFL_CLWK*                   pf_dummy_pi_clwk;
  GFL_TCBSYS*                 pf_tcbsys;
  void*                       pf_tcbsys_wk;
  PALETTE_FADE_PTR            pf_ptr;
  PF_REQ                      pf_req;
  PF_STATE                    pf_state;

  // タイマーアイコン
  TIMEICON_WORK*              timeicon_wk;
}
BTL_REC_SEL_WORK;


//=============================================================================
/**
*  ローカル関数のプロトタイプ宣言
*/
//=============================================================================
// シーケンス処理用
static void Btl_Rec_Sel_ChangeSeqFade( int* seq,
                BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, int mode, int start_evy, int end_evy, int wait,
                FADE_TYPE type );
static void Btl_Rec_Sel_ChangeSeqQa( int* seq,
                BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, u32 str_id, BOOL non );
static void Btl_Rec_Sel_NoChangeSeqQa( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, u32 str_id, BOOL non );
static void Btl_Rec_Sel_ChangeSeqStm( int* seq,
                BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, u32 str_id );
static void Btl_Rec_Sel_NoChangeSeqStm( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, u32 str_id );

// VBlank関数
static void Btl_Rec_Sel_VBlankFunc( GFL_TCB* tcb, void* wk );

// ストリームテキストウィンドウ
static void Btl_Rec_Sel_TextInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_TextExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_TextMain( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_TextShowWinFrm( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_TextShowOffWinFrm( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_TextClearWinIn( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_TextShowOffWinIn( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );

static void Btl_Rec_Sel_TextStartStream( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                                     u32 str_id, BOOL text_stream_need_input );

static BOOL Btl_Rec_Sel_TextWaitStream( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static BOOL Btl_Rec_Sel_TextWaitStreamOne( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );

static void Btl_Rec_Sel_TextStartPrint( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,  // ストリームじゃないテキストウィンドウ
                                     u32 str_id );
static BOOL Btl_Rec_Sel_TextWaitPrint( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );  // ストリームじゃないテキストウィンドウ

// はい・いいえウィンドウ
static void Btl_Rec_Sel_YnInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_YnExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_YnStartSel(  BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );

// 固定テキスト
static void Btl_Rec_Sel_FixInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_FixExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_FixMain( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_FixMainTrans( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_FixShowOnPre( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_FixShowOffPre( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_FixStartTime( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work, u16 sec );
static void Btl_Rec_Sel_FixEndTime( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_FixUpdateTime( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work, u16 sec );

// ポケアイコン
static void Btl_Rec_Sel_PiInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static void Btl_Rec_Sel_PiExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
static GFL_CLWK* CreatePokeicon( GFL_CLUNIT* clunit, HEAPID heap_id, CLSYS_DRAW_TYPE draw_type, u8 pltt_line,
                     u32 monsno, u32 formno, u32 sex, BOOL egg, UI_EASY_CLWK_RES* res, u8 x, u8 y, u8 anim, u8 bg_pri,
                     u8 soft_pri );
static void DeletePokeicon( UI_EASY_CLWK_RES* res, GFL_CLWK* clwk );

// バトルモードから表示するテキストID、ポケアイコンの並べ方を決める
static void Btl_Rec_Sel_DecideFromBattleMode( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );

// BG Main
void Btl_Rec_Sel_BgMInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_BgMExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_BgMCreateNon( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_BgMCreateVs2( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_BgMCreateVs4( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_BgMClear( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
  
// BG Sub
void Btl_Rec_Sel_BgSInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_BgSExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );

// パレットフェード
void Btl_Rec_Sel_PfCreateDummyPokeicon( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_PfDeleteDummyPokeicon( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_PfInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_PfExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_PfMain( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work );
void Btl_Rec_Sel_PfVBlankFunc( BTL_REC_SEL_WORK* work );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT Btl_Rec_Sel_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Btl_Rec_Sel_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Btl_Rec_Sel_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    BTL_REC_SEL_ProcData =
{
  Btl_Rec_Sel_ProcInit,
  Btl_Rec_Sel_ProcMain,
  Btl_Rec_Sel_ProcExit,
};


//=============================================================================
/**
*  外部公開関数定義
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief       PROC パラメータ生成
 *
 *  @param[in]   heap_id       ヒープID
 *  @param[in]   gamedata      GAMEDATA  // 性別判定、バトルレコーダーの有無判定、セーブデータ取得
 *  @param[in]   b_rec         サーバーバージョンを比較した結果、録画セーブ可能のときTRUE
 *  @param[in]   b_sync        同期を取る必要があるときTRUE
 *  @param[in]   b_correct     正しいデータのときTRUE、不正データのときFALSE
 *  @param[in]   battle_mode   BATTLE_MODE_xxx
 *  @param[in]   fight_count   連勝数
 *
 *  @retval      BTL_REC_SEL_PARAM
 */
//------------------------------------------------------------------
BTL_REC_SEL_PARAM*  BTL_REC_SEL_AllocParam(
                            HEAPID           heap_id,
                            GAMEDATA*        gamedata,
                            BOOL             b_rec,
                            BOOL             b_sync,
                            BOOL             b_correct,
                            int              battle_mode,
                            int              fight_count )
{
  BTL_REC_SEL_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof( BTL_REC_SEL_PARAM ) );
  BTL_REC_SEL_InitParam( param, gamedata, b_rec, b_sync, b_correct, battle_mode, fight_count );
  return param;
}

//------------------------------------------------------------------
/**
 *  @brief           PROC パラメータ解放
 *
 *  @param[in,out]   param      BTL_REC_SEL_AllocParamで生成したもの
 *
 *  @retval          
 */
//------------------------------------------------------------------
void             BTL_REC_SEL_FreeParam(
                            BTL_REC_SEL_PARAM*  param )
{
  GFL_HEAP_FreeMemory( param );
}

//------------------------------------------------------------------
/**
 *  @brief           PROC パラメータを設定する
 *
 *  @param[in,out]   param         BTL_REC_SEL_PARAM
 *  @param[in]       gamedata      GAMEDATA  // 性別判定、バトルレコーダーの有無判定、セーブデータ取得
 *  @param[in]       b_rec         サーバーバージョンを比較した結果、録画セーブ可能のときTRUE
 *  @param[in]       b_sync        同期を取る必要があるときTRUE
 *  @param[in]       b_correct     正しいデータのときTRUE、不正データのときFALSE
 *  @param[in]       battle_mode   BATTLE_MODE_xxx
 *  @param[in]       fight_count   連勝数
 *
 *  @retval          
 */
//------------------------------------------------------------------
void             BTL_REC_SEL_InitParam(
                            BTL_REC_SEL_PARAM*  param,
                            GAMEDATA*           gamedata,
                            BOOL                b_rec,
                            BOOL                b_sync,
                            BOOL                b_correct,
                            int                 battle_mode,
                            int                 fight_count )
{
  param->gamedata    = gamedata;
  param->b_rec       = b_rec;
  param->b_sync      = b_sync;
  param->b_correct   = b_correct;
  param->battle_mode = battle_mode;
  param->fight_count = fight_count;
}


//=============================================================================
/**
*  ローカル関数定義(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC 初期化処理
//=====================================
static GFL_PROC_RESULT Btl_Rec_Sel_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  BTL_REC_SEL_PARAM*    param    = (BTL_REC_SEL_PARAM*)pwk;
  BTL_REC_SEL_WORK*     work;

  // オーバーレイ読み込み
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

  // ヒープ
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BTL_REC_SEL, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(BTL_REC_SEL_WORK), HEAPID_BTL_REC_SEL );
    GFL_STD_MemClear( work, sizeof(BTL_REC_SEL_WORK) );
    
    work->heap_id       = HEAPID_BTL_REC_SEL;
  }

  // 何よりも先に行う初期化
  {
    // 非表示
    u8 i;
    for(i=GFL_BG_FRAME0_M; i<=GFL_BG_FRAME3_S; i++)
    {
      GFL_BG_SetVisible( i, VISIBLE_OFF );
    }
  } 

  // グラフィック、フォントなど
  {
    work->graphic                = BTL_REC_SEL_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font                   = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    
    work->print_que              = PRINTSYS_QUE_Create( work->heap_id );
    work->print_que_fix_pre      = PRINTSYS_QUE_Create( work->heap_id );
    work->print_que_fix_sel_time = PRINTSYS_QUE_Create( work->heap_id );
    work->print_que_fix_time     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // バトルレコーダーの有無
  { 
    MYITEM_PTR  myitem_ptr  = GAMEDATA_GetMyItem( param->gamedata );
    work->b_battle_recorder = MYITEM_CheckItem( myitem_ptr, ITEM_BATORUREKOODAA, 1, work->heap_id );  // TRUEのときバトルレコーダーを所持している
    work->b_end_immediately = FALSE;  // 直ちに終了させるときTRUE
  }

  // VBlank中TCB
  {
    work->vblank_tcb = GFUser_VIntr_CreateTCB( Btl_Rec_Sel_VBlankFunc, work, 1 );
  }

  // メッセージ
  work->msgdata_rec     = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_b_record_dat, work->heap_id );

  // BG Main
  Btl_Rec_Sel_BgMInit( param, work ); 
  Btl_Rec_Sel_BgMClear( param, work );
  // BG Sub
  Btl_Rec_Sel_BgSInit( param, work ); 

  // ストリームテキストウィンドウ
  Btl_Rec_Sel_TextInit( param, work );
  Btl_Rec_Sel_TextShowWinFrm( param, work );
  // はい・いいえウィンドウ
  Btl_Rec_Sel_YnInit( param, work );
  // 固定テキスト
  Btl_Rec_Sel_FixInit( param, work );

  // プライオリティ、表示、背景色など
  {
    GFL_BG_SetPriority( BG_FRAME_M_BACK              , 2 );
    GFL_BG_SetPriority( BG_FRAME_M_TIME              , 1 );
    GFL_BG_SetPriority( BG_FRAME_M_TEXT              , 0 );  // 最前面

    GFL_BG_SetVisible( BG_FRAME_M_BACK               , VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_TIME               , VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_TEXT               , VISIBLE_ON );
  
    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x0000 );
    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0x0000 );
  }

  // フェードイン(黒16→見える0)する前に画面を黒にしておく
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 16, 0 );

  // シーケンス処理用
  if( work->b_battle_recorder )
  {
    if( param->b_correct )
    {
      if( param->b_rec )
      {
        Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_QA_INIT,
            GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT,
            FADE_TYPE_OUTSIDE );
        Btl_Rec_Sel_NoChangeSeqQa( param, work, SEQ_QA_ANS_REC, msg_record_01_01, TRUE );
        Btl_Rec_Sel_FixStartTime( param, work, COUNT_TIME_SEC_MAX );
        if( param->b_sync ) Btl_Rec_Sel_BgMCreateNon( param, work );
        work->fix_pause = TRUE;
      }
      else
      {
        Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_WAIT_INIT,
            GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT,
            FADE_TYPE_OUTSIDE );
      }
    }
    else
    {
      {
        Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_WAIT_INIT,
            GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT,
            FADE_TYPE_OUTSIDE );
      }
    }
  }
  else
  {
    {
      if( param->b_sync )
      {
        Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_WAIT_INIT,
            GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT,
            FADE_TYPE_OUTSIDE );
      }
      else
      {
        work->b_end_immediately = TRUE;
      }
    }
  }

  // パレットフェード
  Btl_Rec_Sel_PfCreateDummyPokeicon( param, work );
  Btl_Rec_Sel_PfInit( param, work );
  Btl_Rec_Sel_PfDeleteDummyPokeicon( param, work );

  // 通信アイコン
  GFL_NET_ReloadIconTopOrBottom( TRUE, work->heap_id );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC 終了処理
//=====================================
static GFL_PROC_RESULT Btl_Rec_Sel_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  BTL_REC_SEL_PARAM*    param    = (BTL_REC_SEL_PARAM*)pwk;
  BTL_REC_SEL_WORK*     work     = (BTL_REC_SEL_WORK*)mywk;

  // 通信アイコン
  // 終了するときは通信アイコンに対して何もしない

  // パレットフェード
  Btl_Rec_Sel_PfExit( param, work );

  // 固定テキスト
  Btl_Rec_Sel_FixExit( param, work );
  // はい・いいえウィンドウ
  Btl_Rec_Sel_YnExit( param, work );
  // ストリームテキストウィンドウ
  Btl_Rec_Sel_TextExit( param, work );

  // BG Sub
  Btl_Rec_Sel_BgSExit( param, work ); 
  // BG Main
  Btl_Rec_Sel_BgMExit( param, work ); 

  // メッセージ
  GFL_MSG_Delete( work->msgdata_rec );

  // VBlank中TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // グラフィック、フォントなど
  {
    PRINTSYS_QUE_Clear( work->print_que_fix_time );
    PRINTSYS_QUE_Delete( work->print_que_fix_time );
    PRINTSYS_QUE_Clear( work->print_que_fix_sel_time );
    PRINTSYS_QUE_Delete( work->print_que_fix_sel_time );
    PRINTSYS_QUE_Clear( work->print_que_fix_pre );
    PRINTSYS_QUE_Delete( work->print_que_fix_pre );
    PRINTSYS_QUE_Clear( work->print_que );  // これをやっておかないと、PRINTSYS_QUE_Delete時にque->runningJobが残っていてエラーになることがある。
    PRINTSYS_QUE_Delete( work->print_que );

    GFL_FONT_Delete( work->font );
    BTL_REC_SEL_GRAPHIC_Exit( work->graphic );
  }

  // ヒープ
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_BTL_REC_SEL );
  }

	// オーバーレイ破棄
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC 主処理
//=====================================
static GFL_PROC_RESULT Btl_Rec_Sel_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  BTL_REC_SEL_PARAM*    param    = (BTL_REC_SEL_PARAM*)pwk;
  BTL_REC_SEL_WORK*     work     = (BTL_REC_SEL_WORK*)mywk;

  SAVE_CONTROL_WORK* sv = GAMEDATA_GetSaveControlWork( param->gamedata );

  int key_trg = GFL_UI_KEY_GetTrg();

  switch(*seq)
  {
  case SEQ_FADE_INIT:
    {
      // 特別処理
      if( work->b_end_immediately )  // 直ちに終了させるときTRUE
      {
        (*seq) = SEQ_END;
        break;
      }

      // 内部フェード
      if( work->fade_type == FADE_TYPE_INSIDE )
      {
        if( work->fade_start_evy == 0 )  // フェードアウト(見える0→黒16)
        {
          work->pf_req = PF_REQ_FADE_OUT;
        }
        else  // フェードイン(黒16→見える0)
        {
          work->pf_req = PF_REQ_FADE_IN;
        }
      }
      // 外部フェード
      else
      {
        // フェードイン(黒16→見える0) / フェードアウト(見える0→黒16)
        GFL_FADE_SetMasterBrightReq( work->fade_mode, work->fade_start_evy, work->fade_end_evy, work->fade_wait );
      } 
      (*seq) = SEQ_FADE;
    }
    break;
  case SEQ_FADE:
    {
      BOOL b_finish = FALSE;
      // 内部フェード
      if( work->fade_type == FADE_TYPE_INSIDE )
      {
        if( work->pf_state == PF_STATE_BLACK || work->pf_state == PF_STATE_COLOR )
        {
          b_finish = TRUE;
        }
      }
      // 外部フェード
      else
      {
        if( !GFL_FADE_CheckFade() )
        {
          b_finish = TRUE;
        }
      }
      if( b_finish )
      {
        work->fix_pause = FALSE;
        (*seq) = work->fade_next_seq;
      }
    }
    break;
  case SEQ_QA_INIT:
    {
      Btl_Rec_Sel_TextStartStream( param, work, work->qa_str_id, FALSE );
      (*seq) = SEQ_QA_QUES;
    }
    break;
  case SEQ_QA_QUES:
    {
      if( Btl_Rec_Sel_TextWaitStream( param, work ) )
      {
        Btl_Rec_Sel_YnStartSel( param, work );
        (*seq) = work->qa_next_seq;
      }
    }
    break;
  case SEQ_STM_INIT:
    {
      Btl_Rec_Sel_TextStartStream( param, work, work->stm_str_id, FALSE );
      (*seq) = work->stm_next_seq;
    }
    break;
  case SEQ_QA_ANS_REC:
    {
      u32 ret = BmpMenu_YesNoSelectMain( work->yn_wk );
      if( ret != BMPMENU_NULL )
      {
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        if( ret == 0 )
        {
          LOAD_RESULT result;
#ifdef OFFLINE_TEST
          BOOL btl_data = TRUE; 
#else
          BOOL btl_data = BattleRec_DataOccCheck( sv, work->heap_id, &result, LOADDATA_MYREC ); 
          // resultがRECLOAD_RESULT_OKならTRUEが返ってくるようだ
#endif

          if( btl_data )  // 録画データがある場合
          {
            work->fix_pause = TRUE;
            Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_PRE_SHOW_ON,
                GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, INSIDE_FADE_OUT_WAIT,
                FADE_TYPE_INSIDE );
          }
          else
          {
            Btl_Rec_Sel_FixEndTime( param, work );
            (*seq) = SEQ_SAVE_INIT;
          }
        }
        else
        {
          Btl_Rec_Sel_ChangeSeqQa( seq, param, work, SEQ_QA_ANS_NOREC, msg_record_02_01, TRUE );
        }
      }
      else if( work->fix_timeup )
      {
        BmpMenu_YesNoMenuExit( work->yn_wk );
        (*seq) = SEQ_WAIT_INIT;
      }
    }
    break;
  case SEQ_QA_ANS_NOREC:
    {
      u32 ret = BmpMenu_YesNoSelectMain( work->yn_wk );
      if( ret != BMPMENU_NULL )
      {
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        if( ret == 0 )
        {
          Btl_Rec_Sel_FixEndTime( param, work );
          (*seq) = SEQ_WAIT_INIT;
        } 
        else
        {
          Btl_Rec_Sel_ChangeSeqQa( seq, param, work, SEQ_QA_ANS_REC, msg_record_01_01, TRUE );
        }
      }
      else if( work->fix_timeup )
      {
        BmpMenu_YesNoMenuExit( work->yn_wk );
        (*seq) = SEQ_WAIT_INIT;
      }
    }
    break;
  case SEQ_QA_ANS_PRE:
    {
      u32 ret = BmpMenu_YesNoSelectMain( work->yn_wk );
      if( ret != BMPMENU_NULL )
      {
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        if( ret == 0 )
        {
          work->next_seq = SEQ_SAVE_INIT;
        }
        else
        {
          work->next_seq = SEQ_WAIT_INIT;
        }
        Btl_Rec_Sel_FixEndTime( param, work );
        Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_PRE_SHOW_OFF,
           GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, INSIDE_FADE_OUT_WAIT,
           FADE_TYPE_INSIDE );
      }
      else if( work->fix_timeup )
      {
        BmpMenu_YesNoMenuExit( work->yn_wk );
        work->next_seq = SEQ_WAIT_INIT;
        Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_PRE_SHOW_OFF,
           GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, INSIDE_FADE_OUT_WAIT,
           FADE_TYPE_INSIDE );
      }
    }
    break;
  case SEQ_PRE_SHOW_ON:
    {
#ifdef OFFLINE_TEST
      u8 i, j;
 
      // サンプルデータ作成
      {
        for( j=0; j<PI_POS_MAX; j++ )
        {
          for( i=0; i<PI_PARTY_NUM; i++ )
          {
            work->pi_data[j][i].monsno = PI_PARTY_NUM * j + i + 1;
            work->pi_data[j][i].formno = 0;
            work->pi_data[j][i].sex    = 0;
          }
        }
        work->battle_rec_mode = BATTLE_MODE_COLOSSEUM_SINGLE_FREE;
        //work->battle_rec_mode = BATTLE_MODE_COLOSSEUM_MULTI_FREE;
      }

      Btl_Rec_Sel_BgMClear( param, work );  // 前の背景を消しておく
      Btl_Rec_Sel_TextClearWinIn( param, work );

      Btl_Rec_Sel_DecideFromBattleMode( param, work );
      Btl_Rec_Sel_FixShowOnPre( param, work );
      Btl_Rec_Sel_PiInit( param, work );
      if( work->battle_mode_arrange_two )
        Btl_Rec_Sel_BgMCreateVs2( param, work );
      else
        Btl_Rec_Sel_BgMCreateVs4( param, work );

      Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_QA_INIT,
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, INSIDE_FADE_IN_WAIT,
          FADE_TYPE_INSIDE );
      Btl_Rec_Sel_NoChangeSeqQa( param, work, SEQ_QA_ANS_PRE, msg_record_03_01, FALSE );
#else
      // ここで録画データを表示
      LOAD_RESULT result;
      work->battle_rec_savedata = BattleRec_LoadAlloc( sv, work->heap_id, &result, LOADDATA_MYREC );
      // 必ずTRUEが返ってくる 

      Btl_Rec_Sel_BgMClear( param, work );  // 前の背景を消しておく
      Btl_Rec_Sel_TextClearWinIn( param, work );

      if( result == RECLOAD_RESULT_OK )
      {
        BATTLE_REC_HEADER_PTR head = BattleRec_HeaderPtrGetWork( work->battle_rec_savedata );
        u64 hp;
        int i;
#ifdef BUGFIX_BTS7753_100713
        // 使用ポケモンを設定する前にwork->battle_mode_arrange_twoを設定しておかねばならないので、
        // 先に録画施設の取得、何人対戦かを明らかにしておく。
        hp = RecHeader_ParamGet( head, RECHEAD_IDX_MODE, 0 );
        work->battle_rec_mode = (int)hp;

        Btl_Rec_Sel_DecideFromBattleMode( param, work );
#endif  // BUGFIX_BTS7753_100713
        for( i=0; i<HEADER_MONSNO_MAX; i++ )
        {
          u32 monsno;
          u32 formno = 0;
          u32 sex = 0;
          hp = RecHeader_ParamGet( head, RECHEAD_IDX_MONSNO, i );
          monsno = (u32)hp;
          if( monsno != 0 )
          {
            hp = RecHeader_ParamGet( head, RECHEAD_IDX_FORM_NO, i );
            formno = (u32)hp;
            hp = RecHeader_ParamGet( head, RECHEAD_IDX_GENDER, i );
            sex = (u32)hp;
          }
#ifdef BUGFIX_BTS7753_100713
          if( work->battle_mode_arrange_two )  // 2人対戦
          {
            //         相手
            //      vs
            // 自分

            // データの並び
            //  0 1 2 3 4 5 6 7 8 91011 
            // 自自自自自自相相相相相相
           
            // work->pi_dataの並び
            // work->pi_data[0][]が右上、work->pi_data[1][]が左下
            // work->pi_data[][b]のbが小さいものが左、bが大きいものが右

            if(i<6)  // 自分
            {
              work->pi_data[1][i%PI_PARTY_NUM].monsno = monsno;
              work->pi_data[1][i%PI_PARTY_NUM].formno = formno;
              work->pi_data[1][i%PI_PARTY_NUM].sex    = sex;
            }
            else  // 相手
            {
              work->pi_data[0][i%PI_PARTY_NUM].monsno = monsno;
              work->pi_data[0][i%PI_PARTY_NUM].formno = formno;
              work->pi_data[0][i%PI_PARTY_NUM].sex    = sex;
            }
          }
          else  // 4人対戦
          {
            //         相手一
            //         相手二
            //      vs
            // 自分
            // 味方

            // データの並び
            //  0 1 2 3 4 5 6 7 8 91011
            // 自自自一一一味味味二二二
            
            // work->pi_dataの並び
            // work->pi_data[0][]が右上、work->pi_data[1][]が左下
            // work->pi_data[][b]のbが小さいものが左、bが大きいものが右
            // work->pi_data[][b]のbが0から2が2段の上段、bが3から5が2段の下段
           
            int a, b;
            if(i<3)  // 自分
            {
              a = 1;
              b = i;
            }
            else if(i<6)  // 相手一
            {
              a = 0;
              b = i -3;
            }
            else if(i<9)  // 味方
            {
              a = 1;
              b = i -3;
            }
            else  // 相手二
            {
              a = 0;
              b = i -6;
            }
            work->pi_data[a][b].monsno = monsno;
            work->pi_data[a][b].formno = formno;
            work->pi_data[a][b].sex    = sex;
          }
#else  // BUGFIX_BTS7753_100713
          work->pi_data[i/PI_PARTY_NUM][i%PI_PARTY_NUM].monsno = monsno;
          work->pi_data[i/PI_PARTY_NUM][i%PI_PARTY_NUM].formno = formno;
          work->pi_data[i/PI_PARTY_NUM][i%PI_PARTY_NUM].sex    = sex;
#endif  // BUGFIX_BTS7753_100713
        }
#ifndef BUGFIX_BTS7753_100713
        hp = RecHeader_ParamGet( head, RECHEAD_IDX_MODE, i );
        work->battle_rec_mode = (int)hp;

        Btl_Rec_Sel_DecideFromBattleMode( param, work );
#endif  // BUGFIX_BTS7753_100713
        Btl_Rec_Sel_FixShowOnPre( param, work );
        Btl_Rec_Sel_PiInit( param, work );
        if( work->battle_mode_arrange_two )
          Btl_Rec_Sel_BgMCreateVs2( param, work );
        else
          Btl_Rec_Sel_BgMCreateVs4( param, work );

        Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_QA_INIT,
            GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, INSIDE_FADE_IN_WAIT,
            FADE_TYPE_INSIDE );
        Btl_Rec_Sel_NoChangeSeqQa( param, work, SEQ_QA_ANS_PRE, msg_record_03_01, FALSE );
      }
      else
      {
        Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_SAVE_INIT,
            GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, INSIDE_FADE_IN_WAIT,
            FADE_TYPE_INSIDE );
      }

      BattleRec_ExitWork( work->battle_rec_savedata );
#endif
    }
    break;
  case SEQ_PRE_SHOW_OFF:
    {
      // ここで録画データを非表示
      Btl_Rec_Sel_FixShowOffPre( param, work );
      Btl_Rec_Sel_PiExit( param, work );
      Btl_Rec_Sel_BgMClear( param, work );
      
      Btl_Rec_Sel_ChangeSeqFade( seq, param, work, work->next_seq,
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, INSIDE_FADE_IN_WAIT,
          FADE_TYPE_INSIDE );
      
      Btl_Rec_Sel_TextClearWinIn( param, work );
    
      // ウィンドウがもう不要の場合は消しておく
      if(    work->next_seq == SEQ_WAIT_INIT
          && work->b_battle_recorder
          && param->b_rec
          && (!(param->b_sync)) )
      {
        Btl_Rec_Sel_TextShowOffWinIn( param, work );
        Btl_Rec_Sel_TextShowOffWinFrm( param, work );
      }

      // フェードインする前に時間表示も消しておく(タイムアップしたときの0表示が長く残っているので、ここで消しておく必要がある)
      Btl_Rec_Sel_FixEndTime( param, work );
    }
    break;

  case SEQ_SAVE_INIT:
    {
      Btl_Rec_Sel_TextStartPrint( param, work, msg_record_04_01 );

      (*seq) = SEQ_SAVE_INIT2;
    }
    break;
  case SEQ_SAVE_INIT2:
    {
      if( Btl_Rec_Sel_TextWaitPrint( param, work ) )
      {
        // タイマーアイコン
        work->timeicon_wk = TIMEICON_Create(
														    GFUser_VIntr_GetTCBSYS(),
                                work->text_winin_bmpwin,
                                TEXT_WININ_BACK_COLOR,
                                TIMEICON_DEFAULT_WAIT,
                                work->heap_id );
        // ここでセーブ開始
        work->battle_rec_new_mode = 0;
        work->battle_rec_new_work0 = 0;
        work->battle_rec_new_work1 = 0;
        work->battle_rec_new_save_result = SAVE_RESULT_CONTINUE;

        (*seq) = SEQ_SAVE;
      }
    }
    break;
  case SEQ_SAVE:
    {
#ifdef OFFLINE_TEST
      work->battle_rec_new_save_result = SAVE_RESULT_OK;
#else
      if( work->battle_rec_new_save_result != SAVE_RESULT_OK && work->battle_rec_new_save_result != SAVE_RESULT_NG )
      {
        work->battle_rec_new_save_result = BattleRec_Save( param->gamedata, work->heap_id,
            param->battle_mode, param->fight_count, LOADDATA_MYREC,  // fight_countはバトルサブウェイ以外では使わないので0でいい
            &(work->battle_rec_new_work0), &(work->battle_rec_new_work1) );
      }
#endif
      if( work->battle_rec_new_save_result == SAVE_RESULT_OK || work->battle_rec_new_save_result == SAVE_RESULT_NG )
      {
        //録画をセーブするとメモリ上のデータを暗号化してしまうので
        //連続で使えるように復号化しておく
        BattleRec_DataDecoded();

        // タイマーアイコン
        TIMEICON_Exit( work->timeicon_wk );
        
        if( work->battle_rec_new_save_result == SAVE_RESULT_OK )  // セーブ正常終了
        {
			    PMSND_PlaySE( SEQ_SE_SAVE );
          Btl_Rec_Sel_ChangeSeqStm( seq, param, work, SEQ_SAVEOK, msg_record_05_01 );
          work->text_stream_wait_count = TEXT_STREAM_WAIT_COUNT_DEFAULT;
        }
        else if( work->battle_rec_new_save_result == SAVE_RESULT_NG )  // セーブ異常終了
        {
          Btl_Rec_Sel_ChangeSeqStm( seq, param, work, SEQ_SAVENG, msg_record_06_01 );
          work->text_stream_wait_count = TEXT_STREAM_WAIT_COUNT_DEFAULT;
        }
      }
    }
    break;

  case SEQ_SAVEOK:
    {
      if( Btl_Rec_Sel_TextWaitStream( param, work ) )
        (*seq) = SEQ_WAIT_INIT;
    }
    break;
  case SEQ_SAVENG:
    {
      if( Btl_Rec_Sel_TextWaitStream( param, work ) )
        (*seq) = SEQ_WAIT_INIT;
    }
    break;

  case SEQ_WAIT_INIT:
    {
      u32 str_id;
      if( work->b_battle_recorder )
      {
        if( param->b_correct )
        {
          if( param->b_rec )
          {
            if( param->b_sync )
            {
              str_id = msg_record_07_01;
            }
            else
            {
              // 下画面のフェードだけして、他は何もせずに終了
              Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_END,
                  GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, FADE_OUT_WAIT,
                  FADE_TYPE_OUTSIDE );
              break;
            }
          }
          else
          {
            if( param->b_sync )
            {
              str_id = msg_record_07_02;
            }
            else
            {
              str_id = msg_record_07_03;
            }
          }
        }
        else
        {
          {
            {
              str_id = msg_record_07_04;
            }
          }
        }
      }
      else
      {
        {
          if( param->b_sync )
          {
            str_id = msg_record_07_01;
          }
          else
          {
            // ここには来ないようにBtl_Rec_Sel_ProcInitで判定している
            // 何も表示せずに終了
            (*seq) = SEQ_END;
            break;
          }
        }
      }

      if( param->b_sync )
      {
        Btl_Rec_Sel_TextStartPrint( param, work, str_id );
      }
      else
      {
        Btl_Rec_Sel_TextStartStream( param, work, str_id, TRUE );
      }
      (*seq) = SEQ_WAIT_INIT2;
    }
    break;
  case SEQ_WAIT_INIT2:
    {
      BOOL b_ok = FALSE;
      if( param->b_sync )
      {
        if( Btl_Rec_Sel_TextWaitPrint( param, work ) ) b_ok = TRUE;
      }
      else
      {
        if( Btl_Rec_Sel_TextWaitStream( param, work ) ) b_ok = TRUE;
      }

      if( b_ok )
      {
        if( param->b_sync )
        {
#ifndef OFFLINE_TEST
          GFL_NETHANDLE* nethandle = GFL_NET_HANDLE_GetCurrentHandle();
          GFL_NET_HANDLE_TimeSyncStart( nethandle, BTL_REC_SEL_NET_TIMING_SYNC_NO, WB_NET_BTL_REC_SEL );
#endif

          // タイマーアイコン
          work->timeicon_wk = TIMEICON_Create(
	  													    GFUser_VIntr_GetTCBSYS(),
                                  work->text_winin_bmpwin,
                                  TEXT_WININ_BACK_COLOR,
                                  TIMEICON_DEFAULT_WAIT,
                                  work->heap_id );
        }
        (*seq) = SEQ_WAIT;
      }
    }
    break;
  case SEQ_WAIT:
    {
      {
        BOOL ret = TRUE;
        if( param->b_sync )
        {
#ifndef OFFLINE_TEST
          // エラー確認
          if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
          {
            // エラーが発生したので、終了させる
            ret = TRUE;
          }
          else
          {
            GFL_NETHANDLE* nethandle = GFL_NET_HANDLE_GetCurrentHandle();
            ret = GFL_NET_HANDLE_IsTimeSync( nethandle, BTL_REC_SEL_NET_TIMING_SYNC_NO, WB_NET_BTL_REC_SEL );
          }
#endif
          if( ret )
          {
            // タイマーアイコン
            TIMEICON_Exit( work->timeicon_wk );
          }
        }
        if( ret )  // 相手待ち終了
        {
          Btl_Rec_Sel_ChangeSeqFade( seq, param, work, SEQ_END,
              GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, FADE_OUT_WAIT,
              FADE_TYPE_OUTSIDE );
        }
      }
    }
    break;

  case SEQ_END:
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  Btl_Rec_Sel_TextMain( param, work );
  Btl_Rec_Sel_FixMain( param, work );
  Btl_Rec_Sel_FixMainTrans( param, work );

  // パレットフェード
  Btl_Rec_Sel_PfMain( param, work );

  PRINTSYS_QUE_Main( work->print_que );
  PRINTSYS_QUE_Main( work->print_que_fix_pre );
  PRINTSYS_QUE_Main( work->print_que_fix_sel_time );
  PRINTSYS_QUE_Main( work->print_que_fix_time );

  // 2D描画
  BTL_REC_SEL_GRAPHIC_2D_Draw( work->graphic );
  // 3D描画
  BTL_REC_SEL_GRAPHIC_3D_StartDraw( work->graphic );
  // ここに挿入 
  BTL_REC_SEL_GRAPHIC_3D_EndDraw( work->graphic );

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
*  ローカル関数定義
*/
//=============================================================================
//-------------------------------------
/// シーケンス処理用
//=====================================
static void Btl_Rec_Sel_ChangeSeqFade( int* seq,
                BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, int mode, int start_evy, int end_evy, int wait,
                FADE_TYPE type )
{
  (*seq) = SEQ_FADE_INIT;
  work->fade_next_seq    = next_seq;
  work->fade_mode        = mode;
  work->fade_start_evy   = start_evy;
  work->fade_end_evy     = end_evy;
  work->fade_wait        = wait;
  work->fade_type        = type;
}

static void Btl_Rec_Sel_ChangeSeqQa( int* seq,
                BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, u32 str_id, BOOL non )
{
  (*seq) = SEQ_QA_INIT;
  Btl_Rec_Sel_NoChangeSeqQa( param, work, next_seq, str_id, non );
}
static void Btl_Rec_Sel_NoChangeSeqQa( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, u32 str_id, BOOL non )
{
  work->qa_next_seq    = next_seq;
  work->qa_str_id      = str_id;
  work->qa_non         = non;
}

static void Btl_Rec_Sel_ChangeSeqStm( int* seq,
                BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, u32 str_id )
{
  (*seq) = SEQ_STM_INIT;
  Btl_Rec_Sel_NoChangeSeqStm( param, work, next_seq, str_id );
}
static void Btl_Rec_Sel_NoChangeSeqStm( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                int next_seq, u32 str_id )
{
  work->stm_next_seq     = next_seq;
  work->stm_str_id       = str_id;
}

//-------------------------------------
/// VBlank関数
//=====================================
static void Btl_Rec_Sel_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  BTL_REC_SEL_WORK* work = (BTL_REC_SEL_WORK*)wk;

  // パレットフェード
  Btl_Rec_Sel_PfVBlankFunc( work );
}

//-------------------------------------
/// TEXT
//=====================================
static void Btl_Rec_Sel_TextInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  // パレット
//  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
//      BG_PAL_POS_M_TEXT_FONT * 0x20, BG_PAL_NUM_M_TEXT_FONT * 0x20, work->heap_id );

  // パレット
  // BACKのパレットのここをフォントのパレットとして使う
  {
    // パレットの作成＆転送
    u16 num = 3;
    u16 siz = sizeof(u16) * num;
    u16 ofs = BG_PAL_POS_M_BACK_FONT * 0x20 + 0x20 - 2 * num;
    u16* pal = GFL_HEAP_AllocClearMemory( work->heap_id, siz );
    pal[0x00] = 0x294b;  // 13  // 黒
    pal[0x01] = 0x5694;  // 14  // 灰
    pal[0x02] = 0x7fff;  // 15  // 白 
    GFL_BG_LoadPalette( BG_FRAME_M_BACK, pal, siz, ofs );
    GFL_HEAP_FreeMemory( pal );
  }

  // デフォルトのパレット
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
      BG_PAL_POS_M_DEFAULT_TEXT * 0x20, BG_PAL_NUM_M_DEFAULT_TEXT * 0x20, work->heap_id );

  // BGフレームのスクリーンの空いている箇所に何も表示がされないようにしておく
  work->text_dummy_bmpwin = GFL_BMPWIN_Create( BG_FRAME_M_TEXT, 0, 0, 1, 1,
                                BG_PAL_POS_M_BACK_FONT, GFL_BMP_CHRAREA_GET_F );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_dummy_bmpwin), 0 );
  GFL_BMPWIN_TransVramCharacter(work->text_dummy_bmpwin);

  // ウィンドウ内
  work->text_winin_bmpwin = GFL_BMPWIN_Create( BG_FRAME_M_TEXT, 1, 19, 30, 4,
                                 BG_PAL_POS_M_DEFAULT_TEXT, GFL_BMP_CHRAREA_GET_F );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_winin_bmpwin), TEXT_WININ_BACK_COLOR );
  GFL_BMPWIN_TransVramCharacter(work->text_winin_bmpwin);
    
  // ウィンドウ枠
  work->text_winfrm_tinfo = BmpWinFrame_GraphicSetAreaMan( BG_FRAME_M_TEXT,
                                BG_PAL_POS_M_TEXT_FRAME,
                                MENU_TYPE_SYSTEM, work->heap_id );
  
  // ウィンドウ内、ウィンドウ枠どちらの表示もまだ行わないので、初期化ではここまで。

  // TCBL、フォントカラー、転送など
  work->text_tcblsys = GFL_TCBL_Init( work->heap_id, work->heap_id, 1, 0 );
  //GFL_FONTSYS_SetColor( 13, 14, TEXT_WININ_BACK_COLOR );  // BG_PAL_POS_M_BACK_FONTを使うとき
  GFL_FONTSYS_SetColor( 1, 2, TEXT_WININ_BACK_COLOR );  // BG_PAL_POS_M_DEFAULT_TEXTを使うとき
  GFL_BG_LoadScreenV_Req( BG_FRAME_M_TEXT );

  // NULL初期化
  work->text_stream            = NULL;
  work->text_stream_need_input = FALSE;
  work->text_stream_wait_count = 0;
  work->text_strbuf            = NULL;
}
static void Btl_Rec_Sel_TextExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  if( work->text_stream ) PRINTSYS_PrintStreamDelete( work->text_stream );
  if( work->text_strbuf ) GFL_STR_DeleteBuffer( work->text_strbuf );
  GFL_TCBL_Exit( work->text_tcblsys );
  GFL_BG_FreeCharacterArea( BG_FRAME_M_TEXT,
      GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
      GFL_ARCUTIL_TRANSINFO_GetSize(work->text_winfrm_tinfo) );
  GFL_BMPWIN_Delete( work->text_winin_bmpwin );
  GFL_BMPWIN_Delete( work->text_dummy_bmpwin );
}
static void Btl_Rec_Sel_TextMain( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  GFL_TCBL_Main( work->text_tcblsys );
  if( GFL_UI_GetFrameRate() == GFL_UI_FRAMERATE_30 ) GFL_TCBL_Main( work->text_tcblsys );
}
static void Btl_Rec_Sel_TextShowWinFrm( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  BmpWinFrame_Write( work->text_winin_bmpwin, WINDOW_TRANS_ON_V,
      GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
      BG_PAL_POS_M_TEXT_FRAME );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_winin_bmpwin );
}
static void Btl_Rec_Sel_TextShowOffWinFrm( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  BmpWinFrame_Clear( work->text_winin_bmpwin, WINDOW_TRANS_ON_V );
}
static void Btl_Rec_Sel_TextClearWinIn( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_winin_bmpwin), TEXT_WININ_BACK_COLOR );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_winin_bmpwin );
}
static void Btl_Rec_Sel_TextShowOffWinIn( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_winin_bmpwin), 0 );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_winin_bmpwin );
}
static void Btl_Rec_Sel_TextStartStream( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                                     u32 str_id, BOOL text_stream_need_input )
{
  // 一旦消去
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_winin_bmpwin), TEXT_WININ_BACK_COLOR );
  
  // 前のを消す
  if( work->text_stream ) PRINTSYS_PrintStreamDelete( work->text_stream );  // この関数でタスクも削除してくれるので、同時に動くタスクは1つで済む
  if( work->text_strbuf ) GFL_STR_DeleteBuffer( work->text_strbuf );

  // 文字列作成
  {
    work->text_strbuf  = GFL_MSG_CreateString( work->msgdata_rec, str_id );
  }
  
  // ストリーム開始
  work->text_stream = PRINTSYS_PrintStream(
                          work->text_winin_bmpwin,
                          0, 0,
                          work->text_strbuf,
                          work->font, MSGSPEED_GetWait(),
                          work->text_tcblsys, 2,
                          work->heap_id,
                          TEXT_WININ_BACK_COLOR );

  work->text_stream_need_input = text_stream_need_input;
}

static BOOL Btl_Rec_Sel_TextWaitStream( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  // 30フレーム/秒のときは60フレーム/秒に比べて文字送りが遅いので、2回呼び30フレームでも60フレームと同じ速度を実現する
  BOOL ret;
  ret = Btl_Rec_Sel_TextWaitStreamOne( param, work );
  if( !ret )
  {
    if( GFL_UI_GetFrameRate() == GFL_UI_FRAMERATE_30 )
    {
      ret = Btl_Rec_Sel_TextWaitStreamOne( param, work );
    }
  }
  return ret;
}
static BOOL Btl_Rec_Sel_TextWaitStreamOne( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  BOOL ret = FALSE;

  switch( PRINTSYS_PrintStreamGetState( work->text_stream ) )
  {
  case PRINTSTREAM_STATE_RUNNING:
    if( ( GFL_UI_KEY_GetCont() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetCont() )
    {
      PRINTSYS_PrintStreamShortWait( work->text_stream, 0 );

      if( GFL_UI_KEY_GetCont() & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
      {
        // タッチorキー
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      }
      else
      {
        // タッチorキー
        GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
      }
    }
    break;
  case PRINTSTREAM_STATE_PAUSE:
    if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetTrg() )
    { 
      PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
      PRINTSYS_PrintStreamReleasePause( work->text_stream );

      // メッセージ送りキーカーソルアイコンAPP_KEYCURSOR_WORKが必要なメッセージはないので、用意していない。

      if( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
      {
        // タッチorキー
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      }
      else
      {
        // タッチorキー
        GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
      }
    }
    break;
  case PRINTSTREAM_STATE_DONE:
    {
      if( work->text_stream_wait_count > 0 )
      {
        (work->text_stream_wait_count)--;
        // 30fpsか60fpsかは気にしなくてよい。30フレームのときはこの関数の呼び出し元がこの関数を2回呼んでくれるから。
      }
      else if( work->text_stream_need_input )
      {
        if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetTrg() )
        {
          // メッセージ送りキーカーソルアイコンAPP_KEYCURSOR_WORKが出ているわけではないので、SEも鳴らさなくていいかな
          
          if( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
          {
            // タッチorキー
            GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
          }
          else
          {
            // タッチorキー
            GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
          }

          work->text_stream_need_input = FALSE;
          ret = TRUE;
        }
      }
      else
      {
        ret = TRUE;
      }
    }
    break;
  }
  
  return ret;
}

static void Btl_Rec_Sel_TextStartPrint( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work,
                                     u32 str_id )
{
  // 一旦消去
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_winin_bmpwin), TEXT_WININ_BACK_COLOR );
  
  // 前のを消す
  if( work->text_stream ) PRINTSYS_PrintStreamDelete( work->text_stream );  // この関数でタスクも削除してくれるので、同時に動くタスクは1つで済む
  work->text_stream = NULL;
  if( work->text_strbuf ) GFL_STR_DeleteBuffer( work->text_strbuf );

  // 文字列作成
  {
    work->text_strbuf  = GFL_MSG_CreateString( work->msgdata_rec, str_id );
  }

  // フラグ
  work->text_winin_trans = FALSE;

  // 描画リクエスト
  PRINTSYS_PrintQue(
      work->print_que,
      GFL_BMPWIN_GetBmp(work->text_winin_bmpwin),
      0, 0,
      work->text_strbuf,
      work->font );

  // 既に済んでいるかもしれないので1度呼んでおく
  Btl_Rec_Sel_TextWaitPrint( param, work );
}
static BOOL Btl_Rec_Sel_TextWaitPrint( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  if( !work->text_winin_trans )
  {
    if( !PRINTSYS_QUE_IsExistTarget( work->print_que, GFL_BMPWIN_GetBmp(work->text_winin_bmpwin) ) )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->text_winin_bmpwin );
      work->text_winin_trans = TRUE;
    }
  }
  return work->text_winin_trans;
}


//-------------------------------------
/// はい・いいえウィンドウ
//=====================================
static void Btl_Rec_Sel_YnInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  // この関数はBtl_Rec_Sel_TextInitが済んで
  // BG_PAL_POS_M_TEXT_FONTが設定されている状態で呼ぶこと
  // GFL_FONTSYS_SetColorの設定も必要かもしれない

  u32 size = GFL_BG_1CHRDATASIZ * ( 7*4 +12 );  // system/bmp_menu.cのBmpMenu_YesNoSelectInitを参考にしたサイズ + 少し大きめに
  u32 pos = GFL_BG_AllocCharacterArea( BG_FRAME_M_TEXT, size, GFL_BG_CHRAREA_GET_B );
  GF_ASSERT_MSG( pos != AREAMAN_POS_NOTFOUND, "BTL_REC_SEL : BGキャラ領域が足りませんでした。\n" );
  GFL_BG_FreeCharacterArea( BG_FRAME_M_TEXT, pos, size );

  //カーソルの色がデフォルトフォントパレットになっていないと都合が悪いので
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
      BG_PAL_POS_M_YN * 0x20, BG_PAL_NUM_M_YN * 0x20, work->heap_id );
  // BACKのパレットのここをフォントのパレットとして使う
  // に合わせる
  {
    // パレットの作成＆転送
    u16 num = 3;
    u16 siz = sizeof(u16) * num;
    u16 ofs = BG_PAL_POS_M_YN * 0x20 + 0x20 - 2 * num;
    u16* pal = GFL_HEAP_AllocClearMemory( work->heap_id, siz );
    pal[0x00] = 0x294b;  // 13  // 黒
    pal[0x01] = 0x5694;  // 14  // 灰
    pal[0x02] = 0x7fff;  // 15  // 白 
    GFL_BG_LoadPalette( BG_FRAME_M_BACK, pal, siz, ofs );
    GFL_HEAP_FreeMemory( pal );
  }

  work->yn_dat.frmnum    = BG_FRAME_M_TEXT;
  work->yn_dat.pos_x     = 24;
  work->yn_dat.pos_y     = 13;  // 14ではメッセージウィンドウにかかる
  work->yn_dat.palnum    = BG_PAL_POS_M_YN;//BG_PAL_POS_M_BACK_FONT;//カーソルの色がデフォルトフォントパレットになっていないと都合が悪いので
  work->yn_dat.chrnum    = pos;  // 使われていないようだ
}
static void Btl_Rec_Sel_YnExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  // BmpMenu_YesNoSelectMainが解放してくれるので、何もしなくてよい
}
static void Btl_Rec_Sel_YnStartSel(  BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  // この関数はBtl_Rec_Sel_TextInitが済んで
  // work->text_winfrm_tinfoが設定されている状態で呼ぶこと

  work->yn_wk = BmpMenu_YesNoSelectInit(
                    &work->yn_dat,
                    GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
                    BG_PAL_POS_M_TEXT_FRAME,
                    0,
                    work->heap_id );
}

//-------------------------------------
/// 固定テキスト
//=====================================
static void Btl_Rec_Sel_FixInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  // この関数はBtl_Rec_Sel_TextInitが済んで
  // BG_PAL_POS_M_TEXT_FONTが設定されている状態で呼ぶこと
  // GFL_FONTSYS_SetColorの設定も必要かもしれない

  typedef struct
  {
    u8 frmnum;
    u8 posx;
    u8 posy;
    u8 sizx;
    u8 sizy;
    u8 dir;
  }
  FIX_DATA;
  FIX_DATA fix_data[FIX_MAX] =
  {
    { BG_FRAME_M_TEXT,  2,  1, 13, 2, GFL_BMP_CHRAREA_GET_F },
    { BG_FRAME_M_TEXT,  3,  3, 26, 4, GFL_BMP_CHRAREA_GET_F },
    { BG_FRAME_M_TIME,  2, 15, 11, 2, GFL_BMP_CHRAREA_GET_B },  // ダミーのキャラをつくるのが面倒なので後ろから確保
    { BG_FRAME_M_TIME, 13, 15,  5, 3, GFL_BMP_CHRAREA_GET_B },//{ 14, 15,  2, 3 },
  };
  u8 i;

  for( i=0; i<FIX_MAX; i++ )
  {
    work->fix_bmpwin[i] = GFL_BMPWIN_Create( fix_data[i].frmnum,
                        fix_data[i].posx, fix_data[i].posy, fix_data[i].sizx, fix_data[i].sizy,
                        BG_PAL_POS_M_BACK_FONT, fix_data[i].dir );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->fix_bmpwin[i]), 0 );
    work->fix_finish[i] = TRUE;  // 転送するものがないのでTRUE
  }

  // fix timeの状態
  {
    work->ft_state = FT_STATE_NOT_DISP;
    work->ft_sec   = 0;
  }
}
static void Btl_Rec_Sel_FixExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  u8 i;
  for( i=0; i<FIX_MAX; i++ )
  {
    GFL_BMPWIN_Delete( work->fix_bmpwin[i] );
  }
}
static void Btl_Rec_Sel_FixMain( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  u8 i;
  
  u32 l_fps;
  l_fps = ( GFL_UI_GetFrameRate() == GFL_UI_FRAMERATE_30 ) ? (30) : (60);
  
  if( !(param->b_sync) ) return;

  if( work->fix_pause ) return;

  if( work->fix_frame >= l_fps || work->fix_wait_count > 0 )
  {
    // 残り時間表示を更新する
    {
      u32 sec = work->fix_frame / l_fps;
      Btl_Rec_Sel_FixUpdateTime(param, work, sec);
    }

    // 時間を進める
    if( work->fix_frame >= l_fps )
    {
      work->fix_frame--;
      if( work->fix_frame == (l_fps -1) )
      {
        // 時間切れ
        work->fix_timeup = TRUE;
      }
    }
    else
    {
      work->fix_wait_count--;
      if( work->fix_wait_count == 0 )
      {
        // SELECT TIME ??を非表示にする
        Btl_Rec_Sel_FixEndTime( param, work );
      }
    }
  }
}
static void Btl_Rec_Sel_FixMainTrans( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  u8 i;

  for( i=FIX_PRE; i<=FIX_PRE_TITLE; i++ )
  {
    if( !(work->fix_finish[i]) )
    {
      if( !PRINTSYS_QUE_IsExistTarget( work->print_que_fix_pre, GFL_BMPWIN_GetBmp(work->fix_bmpwin[i]) ) )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->fix_bmpwin[i] );
        work->fix_finish[i] = TRUE;
      }
    }
  }

  i=FIX_SEL_TIME;
  {
    if( !(work->fix_finish[i]) )
    {
      if( !PRINTSYS_QUE_IsExistTarget( work->print_que_fix_sel_time, GFL_BMPWIN_GetBmp(work->fix_bmpwin[i]) ) )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->fix_bmpwin[i] );
        work->fix_finish[i] = TRUE;
      }
    }
  }

  i=FIX_TIME;
  {
    if( !(work->fix_finish[i]) )
    {
      if( !PRINTSYS_QUE_IsExistTarget( work->print_que_fix_time, GFL_BMPWIN_GetBmp(work->fix_bmpwin[i]) ) )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->fix_bmpwin[i] );
        work->fix_finish[i] = TRUE;
      }
    }
  }
}

static void Btl_Rec_Sel_FixShowOnPre( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  STRBUF* strbuf;
  GFL_BMP_DATA* bmp_data;
  u8 i;

  // 前回の記録
  {
    strbuf = GFL_MSG_CreateString( work->msgdata_rec, msg_record_10_01 );
    bmp_data = GFL_BMPWIN_GetBmp( work->fix_bmpwin[FIX_PRE] );
    PRINTSYS_PrintQueColor( work->print_que_fix_pre, bmp_data, 4, 1, strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );
    GFL_STR_DeleteBuffer( strbuf );
    work->fix_finish[FIX_PRE] = FALSE;
  }

  // バトルモード
  // 例：コロシアム　シングル
  // 　　せいげんなし
  {
    strbuf = GFL_MSG_CreateString( work->msgdata_rec, work->battle_mode_str_id );
    bmp_data = GFL_BMPWIN_GetBmp( work->fix_bmpwin[FIX_PRE_TITLE] );
    PRINTSYS_PrintQueColor( work->print_que_fix_pre, bmp_data, 4, 1, strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );
    GFL_STR_DeleteBuffer( strbuf );
    work->fix_finish[FIX_PRE_TITLE] = FALSE;
  }

  // 既に済んでいるかもしれないので1度呼んでおく
  Btl_Rec_Sel_FixMainTrans( param, work );
}
static void Btl_Rec_Sel_FixShowOffPre( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  u8 i;
  for( i=FIX_PRE; i<=FIX_PRE_TITLE; i++ )
  {
    PRINTSYS_QUE_Clear( work->print_que_fix_pre );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->fix_bmpwin[i]), 0 );
    GFL_BMPWIN_MakeTransWindow_VBlank( work->fix_bmpwin[i] );
    work->fix_finish[i] = TRUE;
  }
}

static void Btl_Rec_Sel_FixStartTime( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work, u16 sec )
{
  u32 l_fps;
  l_fps = ( GFL_UI_GetFrameRate() == GFL_UI_FRAMERATE_30 ) ? (30) : (60);

  work->fix_frame = sec * l_fps + (l_fps-1);
  work->fix_wait_count = l_fps /2;
  work->fix_pause = FALSE;
  work->fix_timeup = FALSE;

  work->ft_state = FT_STATE_NOT_DISP;
  work->ft_sec = 0;

  Btl_Rec_Sel_FixUpdateTime( param, work, sec );
}
static void Btl_Rec_Sel_FixEndTime( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  u8 i;

  work->fix_frame = 0;
  work->fix_wait_count = 0;

  i=FIX_SEL_TIME;
  {
    PRINTSYS_QUE_Clear( work->print_que_fix_sel_time );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->fix_bmpwin[i]), 0 );
    GFL_BMPWIN_MakeTransWindow_VBlank( work->fix_bmpwin[i] );
    work->fix_finish[i] = TRUE;
  }

  i=FIX_TIME;
  {
    PRINTSYS_QUE_Clear( work->print_que_fix_time );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->fix_bmpwin[i]), 0 );
    GFL_BMPWIN_MakeTransWindow_VBlank( work->fix_bmpwin[i] );
    work->fix_finish[i] = TRUE;
  }

  if( work->qa_non )  // non背景(SELECT TIMEを表示する枠しかない背景)の場合は、文字を消すのと同時に背景のクリアも行う
  {                   // non背景でないときはフェードしてから背景クリアをしなければならないので、専用のクリアに任せる
    Btl_Rec_Sel_BgMClear( param, work );
  }

  work->ft_state = FT_STATE_NOT_DISP;
  work->ft_sec = 0;
}
static void Btl_Rec_Sel_FixUpdateTime( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work, u16 sec )
{
  STRBUF* strbuf;
  GFL_BMP_DATA* bmp_data;
  PRINTSYS_LSB color;
  u8 i;
  FT_STATE  ft_state_curr;

  if( !(param->b_sync) ) return;

  if( sec > 10 )
  {
    color = PRINTSYS_LSB_Make(1,2,0);
    ft_state_curr = FT_STATE_WHITE_DISP;
  }
  else
  {
    //color = PRINTSYS_LSB_Make(4,3,0);
    color = PRINTSYS_LSB_Make(3,4,0);
    ft_state_curr = FT_STATE_RED_DISP;
  }

  // 一旦消去
  if( ft_state_curr != work->ft_state )
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->fix_bmpwin[FIX_SEL_TIME]), 0 );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->fix_bmpwin[FIX_TIME]), 0 );
  }
  else if( sec != work->ft_sec )
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->fix_bmpwin[FIX_TIME]), 0 );
  }

  // SELECT TIME
  if( ft_state_curr != work->ft_state )
  {
    PRINTSYS_QUE_Clear( work->print_que_fix_sel_time );
    strbuf = GFL_MSG_CreateString( work->msgdata_rec, msg_record_time01 );
    bmp_data = GFL_BMPWIN_GetBmp( work->fix_bmpwin[FIX_SEL_TIME] );
    PRINTSYS_PrintQueColor( work->print_que_fix_sel_time, bmp_data, 4, 1, strbuf, work->font, color );
    work->fix_finish[FIX_SEL_TIME] = FALSE;
    GFL_STR_DeleteBuffer( strbuf );
  }

  // ??:??
  if( ft_state_curr != work->ft_state || sec != work->ft_sec )
  {
    WORDSET* wordset = WORDSET_Create( work->heap_id );
    STRBUF* src_strbuf = GFL_MSG_CreateString( work->msgdata_rec, msg_record_time03 );
    strbuf = GFL_STR_CreateBuffer( STRBUF_FIX_TIME_LENGTH, work->heap_id );
   
    PRINTSYS_QUE_Clear( work->print_que_fix_time );

    WORDSET_RegisterNumber( wordset, 1, sec, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
   
    bmp_data = GFL_BMPWIN_GetBmp( work->fix_bmpwin[FIX_TIME] );
    PRINTSYS_PrintQueColor( work->print_que_fix_time, bmp_data, 2, 2, strbuf, work->font, color );
    work->fix_finish[FIX_TIME] = FALSE;

    GFL_STR_DeleteBuffer( strbuf );
    GFL_STR_DeleteBuffer( src_strbuf );
    WORDSET_Delete( wordset );
  }

  work->ft_state = ft_state_curr;
  work->ft_sec = sec;

  // 既に済んでいるかもしれないので1度呼んでおく
  Btl_Rec_Sel_FixMainTrans( param, work );
}

//-------------------------------------
/// ポケアイコン
//=====================================
static void Btl_Rec_Sel_PiInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  GFL_CLUNIT* clunit = BTL_REC_SEL_GRAPHIC_GetClunit( work->graphic );
  u8 i, j;
 
  // データ作成
  {
    for( j=0; j<PI_POS_MAX; j++ )
    {
      for( i=0; i<PI_PARTY_NUM; i++ )
      {
        //work->pi_data[j][i].monsno = PI_PARTY_NUM * j + i;
        //work->pi_data[j][i].formno = 0;
        work->pi_data[j][i].egg    = 0;
        work->pi_data[j][i].anim   = 1;
      }
    }
  }

  // 位置
  {
    for( j=0; j<PI_POS_MAX; j++ )
    {
      for( i=0; i<PI_PARTY_NUM; i++ )
      {
        if( work->battle_mode_arrange_two )  // 2人対戦
        {
          if( j == PI_POS_R )
          {
            work->pi_data[j][i].x        = 11*8+12 + 3*8*i;
            work->pi_data[j][i].y        =  8*8;
            work->pi_data[j][i].soft_pri = PI_PARTY_NUM - i;  // 左から6,5,4,3,2,1となるように
          }
          else
          {
            work->pi_data[j][i].x        =  3*8+12 + 3*8*i;
            work->pi_data[j][i].y        = 14*8;
            work->pi_data[j][i].soft_pri = PI_PARTY_NUM - i;  // 左から6,5,4,3,2,1となるように
          }
        }
        else  // 4人対戦
        {
          if( j == PI_POS_R )
          {
            if( i < 3 )
            {
              work->pi_data[j][i].x        = 20*8+12 + 3*8*i;
              work->pi_data[j][i].y        =  8*8;
              work->pi_data[j][i].soft_pri = PI_PARTY_NUM - i;  // 左から6,5,4となるように
            }
            else
            {
              work->pi_data[j][i].x        = 20*8+12 + 3*8*(i-3);
              work->pi_data[j][i].y        = 11*8;
              work->pi_data[j][i].soft_pri = PI_PARTY_NUM - i;  // 左から3,2,1となるように
            }
          }
          else
          {
            if( i< 3 )
            {
              work->pi_data[j][i].x        =  3*8+12 + 3*8*i;
              work->pi_data[j][i].y        = 11*8;
              work->pi_data[j][i].soft_pri = PI_PARTY_NUM - i;  // 左から6,5,4となるように
            }
            else
            {
              work->pi_data[j][i].x        =  3*8+12 + 3*8*(i-3);
              work->pi_data[j][i].y        = 14*8;
              work->pi_data[j][i].soft_pri = PI_PARTY_NUM - i;  // 左から3,2,1となるように
            }
          }
        }
      }
    }
  }

  // 生成
  for( j=0; j<PI_POS_MAX; j++ )
  {
    for( i=0; i<PI_PARTY_NUM; i++ )
    {
      if( work->pi_data[j][i].monsno != 0 )
      {
        work->pi_data[j][i].clwk = CreatePokeicon(
                                       clunit, work->heap_id, CLSYS_DRAW_MAIN, OBJ_PAL_POS_M_PI,
                                       work->pi_data[j][i].monsno, work->pi_data[j][i].formno, work->pi_data[j][i].sex,
                                       work->pi_data[j][i].egg, &(work->pi_data[j][i].res),
                                       work->pi_data[j][i].x, work->pi_data[j][i].y, work->pi_data[j][i].anim, 1,
                                       work->pi_data[j][i].soft_pri );
      }
    }
  }
}
static void Btl_Rec_Sel_PiExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  u8 i, j;
  for( j=0; j<PI_POS_MAX; j++ )
  {
    for( i=0; i<PI_PARTY_NUM; i++ )
    {
      if( work->pi_data[j][i].monsno != 0 )
      {
        DeletePokeicon( &(work->pi_data[j][i].res), work->pi_data[j][i].clwk );
      }
    }
  }
}
static GFL_CLWK* CreatePokeicon( GFL_CLUNIT* clunit, HEAPID heap_id, CLSYS_DRAW_TYPE draw_type, u8 pltt_line,
                     u32 monsno, u32 formno, u32 sex, BOOL egg, UI_EASY_CLWK_RES* res, u8 x, u8 y, u8 anim, u8 bg_pri,
                     u8 soft_pri )
{
  GFL_CLWK* clwk;

  UI_EASY_CLWK_RES_PARAM res_param;

  res_param.draw_type       = draw_type;
  res_param.comp_flg        = UI_EASY_CLWK_RES_COMP_NCLR;
  res_param.arc_id          = ARCID_POKEICON;
  res_param.pltt_id         = POKEICON_GetPalArcIndex();
  res_param.ncg_id          = POKEICON_GetCgxArcIndexByMonsNumber( monsno, formno, sex, egg );
  res_param.cell_id         = POKEICON_GetCellArcIndex(); 
  res_param.anm_id          = POKEICON_GetAnmArcIndex();
  res_param.pltt_line       = pltt_line;
  res_param.pltt_src_ofs    = 0;
  res_param.pltt_src_num    = 3;
    
  UI_EASY_CLWK_LoadResource( res, &res_param, clunit, heap_id );

  // アニメシーケンスで指定( 0=瀕死, 1=HP最大, 2=HP緑, 3=HP黄, 4=HP赤, 5=状態異常 )
  clwk = UI_EASY_CLWK_CreateCLWK( res, clunit, x, y, anim, heap_id );

  // 上にメッセージウィンドウが出ることもあるので、BGプライオリティを変更しておく
  GFL_CLACT_WK_SetBgPri( clwk, bg_pri );
  
  // 上にアイテムアイコンを描画するので優先度を下げておく
  GFL_CLACT_WK_SetSoftPri( clwk, soft_pri );
  // オートアニメ ON
  GFL_CLACT_WK_SetAutoAnmFlag( clwk, TRUE );

  {
    u8 pal_num = POKEICON_GetPalNum( monsno, formno, sex, egg );
    GFL_CLACT_WK_SetPlttOffs( clwk, pal_num, CLWK_PLTTOFFS_MODE_OAM_COLOR );
  }

  return clwk;
}
static void DeletePokeicon( UI_EASY_CLWK_RES* res, GFL_CLWK* clwk )
{
  GFL_CLACT_WK_Remove( clwk );
  UI_EASY_CLWK_UnLoadResource( res );
}

//-------------------------------------
/// バトルモードから表示するテキストID、ポケアイコンの並べ方を決める
//=====================================
static void Btl_Rec_Sel_DecideFromBattleMode( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  u8 m = work->battle_rec_mode;
  if( m >= BATTLE_MODE_MAX ) m = 0;

  // テキストID
  work->battle_mode_str_id = battle_mode_info_tbl[m][0];

  // ポケアイコンの並べ方
  if( battle_mode_info_tbl[m][1] == 0 )
  {
    work->battle_mode_arrange_two = TRUE;
  }
  else
  {
    work->battle_mode_arrange_two = FALSE;
  }
}

//-------------------------------------
/// BG Main
//=====================================
void Btl_Rec_Sel_BgMInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_BATT_REC_GRA, work->heap_id );

  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_batt_rec_gra_batt_rec_browse_bg_NCLR, PALTYPE_MAIN_BG,
                                    BG_PAL_POS_M_BACK * 0x20, BG_PAL_NUM_M_BACK * 0x20, work->heap_id );
  work->bg_m_tinfo = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle,
                            NARC_batt_rec_gra_batt_rec_data_NCGR,
                            BG_FRAME_M_BACK,
                            0,
                            FALSE, work->heap_id );
  GF_ASSERT_MSG( work->bg_m_tinfo != GFL_ARCUTIL_TRANSINFO_FAIL, "BTL_REC_SEL : BGキャラ領域が足りませんでした。\n" );

  GFL_ARC_CloseDataHandle( handle );
}
void Btl_Rec_Sel_BgMExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  GFL_BG_FreeCharacterArea( BG_FRAME_M_BACK,
                            GFL_ARCUTIL_TRANSINFO_GetPos( work->bg_m_tinfo ),
                            GFL_ARCUTIL_TRANSINFO_GetSize( work->bg_m_tinfo ) );
}
void Btl_Rec_Sel_BgMCreateNon( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  //GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_batt_rec_gra_batt_rec_non_NSCR,
  //                                 BG_FRAME_M_BACK,
  //                                 GFL_ARCUTIL_TRANSINFO_GetPos( work->bg_m_tinfo ),
  //                                 0,//32*32*GFL_BG_1SCRDATASIZ,
  //                                 FALSE, work->heap_id );

  GFL_ARC_UTIL_TransVramScreen( ARCID_BATT_REC_GRA,
                                NARC_batt_rec_gra_batt_rec_non_NSCR,
                                BG_FRAME_M_BACK,
                                GFL_ARCUTIL_TRANSINFO_GetPos( work->bg_m_tinfo ),
                                0,
                                FALSE,
                                work->heap_id );
      
  //GFL_BG_SetScroll( BG_FRAME_M_BACK, GFL_BG_SCROLL_X_SET, 0 );
  //GFL_BG_SetScroll( BG_FRAME_M_BACK, GFL_BG_SCROLL_Y_SET, 0 );

  //GFL_BG_LoadScreenReq( BG_FRAME_M_BACK );
  GFL_BG_LoadScreenV_Req( BG_FRAME_M_BACK );

  GFL_BG_SetScrollReq( BG_FRAME_M_TIME, GFL_BG_SCROLL_Y_SET, 0 );
}
void Btl_Rec_Sel_BgMCreateVs2( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  GFL_ARC_UTIL_TransVramScreen( ARCID_BATT_REC_GRA,
                                NARC_batt_rec_gra_batt_rec_vs2_NSCR,
                                BG_FRAME_M_BACK,
                                GFL_ARCUTIL_TRANSINFO_GetPos( work->bg_m_tinfo ),
                                0,
                                FALSE,
                                work->heap_id );
 
  GFL_BG_LoadScreenV_Req( BG_FRAME_M_BACK );
 
  GFL_BG_SetScroll( BG_FRAME_M_TIME, GFL_BG_SCROLL_Y_SET, -8 );
}
void Btl_Rec_Sel_BgMCreateVs4( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  GFL_ARC_UTIL_TransVramScreen( ARCID_BATT_REC_GRA,
                                NARC_batt_rec_gra_batt_rec_vs4_NSCR,
                                BG_FRAME_M_BACK,
                                GFL_ARCUTIL_TRANSINFO_GetPos( work->bg_m_tinfo ),
                                0,
                                FALSE,
                                work->heap_id );
 
  GFL_BG_LoadScreenV_Req( BG_FRAME_M_BACK );
  
  GFL_BG_SetScroll( BG_FRAME_M_TIME, GFL_BG_SCROLL_Y_SET, -8 );
}
void Btl_Rec_Sel_BgMClear( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  GFL_BG_ClearScreenCodeVReq( BG_FRAME_M_BACK, 0 );
}
  
//-------------------------------------
/// BG Sub
//=====================================
void Btl_Rec_Sel_BgSInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  MYSTATUS*   mystatus    = GAMEDATA_GetMyStatus( param->gamedata );
  u8          sex         = (u8)MyStatus_GetMySex(mystatus);
  
  APP_NOGEAR_SUBSCREEN_Init();
  APP_NOGEAR_SUBSCREEN_Trans( work->heap_id, sex );
}
void Btl_Rec_Sel_BgSExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  APP_NOGEAR_SUBSCREEN_Exit();
}

//-------------------------------------
/// パレットフェード
//=====================================
void Btl_Rec_Sel_PfCreateDummyPokeicon( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  GFL_CLUNIT* clunit = BTL_REC_SEL_GRAPHIC_GetClunit( work->graphic );
  
  work->pf_dummy_pi_clwk = CreatePokeicon(
                               clunit, work->heap_id, CLSYS_DRAW_MAIN, OBJ_PAL_POS_M_PI,
                               1, 0, 0,
                               FALSE, &(work->pf_dummy_pi_res),
                               0, 0, 0, 1,
                               1 );
}
void Btl_Rec_Sel_PfDeleteDummyPokeicon( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  DeletePokeicon( &(work->pf_dummy_pi_res), work->pf_dummy_pi_clwk );
}
void Btl_Rec_Sel_PfInit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  // タスク
  work->pf_tcbsys_wk    = GFL_HEAP_AllocClearMemory( work->heap_id, GFL_TCB_CalcSystemWorkSize(PF_TCBSYS_TASK_MAX) );
  work->pf_tcbsys       = GFL_TCB_Init( PF_TCBSYS_TASK_MAX, work->pf_tcbsys_wk );

  // パレットフェード
  work->pf_ptr = PaletteFadeInit( work->heap_id );
  PaletteTrans_AutoSet( work->pf_ptr, TRUE );
  PaletteFadeWorkAllocSet( work->pf_ptr, FADE_MAIN_BG, PF_BG_M_NUM*0x20, work->heap_id );
  PaletteFadeWorkAllocSet( work->pf_ptr, FADE_MAIN_OBJ, PF_OBJ_M_NUM*0x20, work->heap_id );

  // 現在VRAMにあるパレットを壊さないように、VRAMからパレット内容をコピーする
  PaletteWorkSet_VramCopy( work->pf_ptr, FADE_MAIN_BG, 0, PF_BG_M_NUM*0x20 );
  PaletteWorkSet_VramCopy( work->pf_ptr, FADE_MAIN_OBJ, 0, PF_OBJ_M_NUM*0x20 );

  // 初期化
  work->pf_state = PF_STATE_COLOR;
  work->pf_req = PF_REQ_NONE;
}
void Btl_Rec_Sel_PfExit( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  // パレットフェード
  PaletteFadeWorkAllocFree( work->pf_ptr, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( work->pf_ptr, FADE_MAIN_OBJ );
  PaletteFadeFree( work->pf_ptr );

  // タスク
  GFL_TCB_Exit( work->pf_tcbsys );
  GFL_HEAP_FreeMemory( work->pf_tcbsys_wk );
}
void Btl_Rec_Sel_PfMain( BTL_REC_SEL_PARAM* param, BTL_REC_SEL_WORK* work )
{
  // タスク
  GFL_TCB_Main( work->pf_tcbsys );

  // パレットフェード
  {
    u8 i;
    u16 req_bit;

    if( PaletteFadeCheck(work->pf_ptr) == 0 )
    {
      // 状態
      switch( work->pf_state )
      {
      case PF_STATE_FADE_IN:
        {
          work->pf_state = PF_STATE_COLOR;
        }
        break;
      case PF_STATE_FADE_OUT:
        {
          work->pf_state = PF_STATE_BLACK;
        }
        break;
      }

      // 要求
      switch( work->pf_req )
      {
      case PF_REQ_FADE_IN:
        {
          if( work->pf_state != PF_STATE_FADE_IN )
          {
            // ブラック→カラー

            req_bit = 0;
            for( i=0; i<PF_BG_M_NUM; i++ ) req_bit |= 1<<i;
            PaletteFadeReq(
              work->pf_ptr,
              PF_BIT_MAIN_BG,
              req_bit,
              INSIDE_FADE_IN_WAIT,
              16, 0, 0x0000,
              work->pf_tcbsys
            );

            req_bit = 0;
            for( i=0; i<PF_OBJ_M_NUM; i++ ) req_bit |= 1<<i;
            PaletteFadeReq(
              work->pf_ptr,
              PF_BIT_MAIN_OBJ,
              req_bit,
              INSIDE_FADE_IN_WAIT,
              16, 0, 0x0000,
              work->pf_tcbsys
            );

            work->pf_state = PF_STATE_FADE_IN;
            work->pf_req = PF_REQ_NONE;
          }
        }
        break;
      case PF_REQ_FADE_OUT:
        {
          if( work->pf_state != PF_STATE_FADE_OUT )
          {
            // カラー→ブラック
            
            req_bit = 0;
            for( i=0; i<PF_BG_M_NUM; i++ ) req_bit |= 1<<i;
            PaletteFadeReq(
              work->pf_ptr,
              PF_BIT_MAIN_BG,
              req_bit,
              INSIDE_FADE_OUT_WAIT,
              0, 16, 0x0000,
              work->pf_tcbsys
            );

            req_bit = 0;
            for( i=0; i<PF_OBJ_M_NUM; i++ ) req_bit |= 1<<i;
            PaletteFadeReq(
              work->pf_ptr,
              PF_BIT_MAIN_OBJ,
              req_bit,
              INSIDE_FADE_OUT_WAIT,
              0, 16, 0x0000,
              work->pf_tcbsys
            );

            work->pf_state = PF_STATE_FADE_OUT;
            work->pf_req = PF_REQ_NONE;
          }
        }
        break;
      }
    }
  }
}
void Btl_Rec_Sel_PfVBlankFunc( BTL_REC_SEL_WORK* work )
{
  PaletteFadeTrans( work->pf_ptr );
}

