//============================================================================
/**
 *  @file   zukan_info.c
 *  @brief  図鑑情報
 *  @author Koji Kawada
 *  @data   2009.12.03
 *  @note   infowin.c, touchbar.c, ui_template.cを参考にして作成しました。
 *
 *  モジュール名：ZUKAN_INFO
 */
//============================================================================
// lib
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

// インクルード
#include "print/printsys.h"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/global_msg.h"

#include "ui/ui_easy_clwk.h"

#include "system/poke2dgra.h"

#include "poke_tool/pokefoot.h"

// 鳴き声
#include "sound/pm_voice.h"

// アーカイブ
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "zukan_data.naix"
#include "zukan_gra.naix"

// メッセージ
#include "msg/msg_zkn.h"
#include "msg/msg_zkn_type.h"
#include "msg/msg_zkn_height.h"
#include "msg/msg_zkn_weight.h"
#if	PM_VERSION == VERSION_BLACK
  #include "msg/msg_zkn_comment_01.h"
#else
  #include "msg/msg_zkn_comment_00.h"
#endif

// 外国語メッセージ
#include "msg/msg_zkn_comment_e.h"
#include "msg/msg_zkn_comment_fra.h"
#include "msg/msg_zkn_comment_ger.h"
#include "msg/msg_zkn_comment_ita.h"
#include "msg/msg_zkn_comment_spa.h"
#include "msg/msg_zkn_comment_kor.h"

#include "msg/msg_zkn_height_e.h"
#include "msg/msg_zkn_height_fra.h"
#include "msg/msg_zkn_height_ger.h"
#include "msg/msg_zkn_height_ita.h"
#include "msg/msg_zkn_height_spa.h"
#include "msg/msg_zkn_height_kor.h"

#include "msg/msg_zkn_weight_e.h"
#include "msg/msg_zkn_weight_fra.h"
#include "msg/msg_zkn_weight_ger.h"
#include "msg/msg_zkn_weight_ita.h"
#include "msg/msg_zkn_weight_spa.h"
#include "msg/msg_zkn_weight_kor.h"

#include "msg/msg_zkn_type_e.h"
#include "msg/msg_zkn_type_fra.h"
#include "msg/msg_zkn_type_ger.h"
#include "msg/msg_zkn_type_ita.h"
#include "msg/msg_zkn_type_spa.h"
#include "msg/msg_zkn_type_kor.h"

#include "msg/msg_zkn_monsname_e.h"
#include "msg/msg_zkn_monsname_fra.h"
#include "msg/msg_zkn_monsname_ger.h"
#include "msg/msg_zkn_monsname_ita.h"
#include "msg/msg_zkn_monsname_spa.h"
#include "msg/msg_zkn_monsname_kor.h"

// zukan_info
#include "zukan_info.h"

//=============================================================================
/**
 *  定数定義
 */
//=============================================================================
// BGフレーム
#define ZUKAN_INFO_BG_FRAME_M_MSG    (GFL_BG_FRAME3_M)
#define ZUKAN_INFO_BG_FRAME_M_FORE   (GFL_BG_FRAME2_M)
#define ZUKAN_INFO_BG_FRAME_M_BACK   (GFL_BG_FRAME0_M)

#define ZUKAN_INFO_BG_FRAME_S_MSG    (GFL_BG_FRAME3_S)
#define ZUKAN_INFO_BG_FRAME_S_FORE   (GFL_BG_FRAME2_S)
#define ZUKAN_INFO_BG_FRAME_S_BACK   (GFL_BG_FRAME0_S)

// BGパレット
// 本数
enum
{
  ZUKAN_INFO_BG_PAL_NUM_FORE_BACK    =  3,
  ZUKAN_INFO_BG_PAL_NUM_MSG          =  1,
  ZUKAN_INFO_BG_PAL_NUM_BACK_NONE    =  2,  // 何も表示しないとき用の背景
};
// 位置
enum
{
  ZUKAN_INFO_BG_PAL_POS_FORE_BACK    = 0,
  ZUKAN_INFO_BG_PAL_POS_MSG          = ZUKAN_INFO_BG_PAL_POS_FORE_BACK   + ZUKAN_INFO_BG_PAL_NUM_FORE_BACK   ,  // 3
  ZUKAN_INFO_BG_PAL_POS_BACK_NONE    = ZUKAN_INFO_BG_PAL_POS_MSG         + ZUKAN_INFO_BG_PAL_NUM_MSG         ,  // 4
  ZUKAN_INFO_BG_PAL_POS_MAX          = ZUKAN_INFO_BG_PAL_POS_BACK_NONE   + ZUKAN_INFO_BG_PAL_NUM_BACK_NONE   ,  // 6  // ここから空き
};

// OBJパレット
// 本数
enum
{
  ZUKAN_INFO_OBJ_PAL_NUM_POKE2D    =  1,
  ZUKAN_INFO_OBJ_PAL_NUM_TYPEICON  =  3,
  ZUKAN_INFO_OBJ_PAL_NUM_POKEFOOT  =  1,
};
// 位置
enum
{
  ZUKAN_INFO_OBJ_PAL_POS_POKE2D    = 0,
  ZUKAN_INFO_OBJ_PAL_POS_TYPEICON  = ZUKAN_INFO_OBJ_PAL_POS_POKE2D   + ZUKAN_INFO_OBJ_PAL_NUM_POKE2D   ,  // 1
  ZUKAN_INFO_OBJ_PAL_POS_POKEFOOT  = ZUKAN_INFO_OBJ_PAL_POS_TYPEICON + ZUKAN_INFO_OBJ_PAL_NUM_TYPEICON ,  // 4
  ZUKAN_INFO_OBJ_PAL_POS_MAX       = ZUKAN_INFO_OBJ_PAL_POS_POKEFOOT + ZUKAN_INFO_OBJ_PAL_NUM_POKEFOOT ,  // 5  // ここから空き
};

// 色について
#define ZUKAN_INFO_COLOR_SIZE           (2)         // 1色のサイズは2バイト
#define ZUKAN_INFO_PAL_LINE_COLOR_NUM   (0x10)      // 1本の色数
#define ZUKAN_INFO_PAL_LINE_SIZE        ( ZUKAN_INFO_COLOR_SIZE * ZUKAN_INFO_PAL_LINE_COLOR_NUM )  // 1本のサイズ(32バイト)

// BGについて
#define ZUKAN_INFO_BG_CHARA_SIZE           ( 32 * 4 * GFL_BG_1CHRDATASIZ )  // バイト
#define ZUKAN_INFO_BG_SCREEN_W             (32)  // キャラ
#define ZUKAN_INFO_BG_SCREEN_H             (32)//(24)  // キャラ
#define ZUKAN_INFO_BG_SCREEN_SIZE          ( ZUKAN_INFO_BG_SCREEN_W * ZUKAN_INFO_BG_SCREEN_H * GFL_BG_1SCRDATASIZ )  // バイト

// BGのスクロール
#define ZUKAN_INFO_BACK_BG_SCROLL_X_VALUE (1)
#define ZUKAN_INFO_BACK_BG_SCROLL_WAIT    (4)  // 4フレームに1回動く  // 4より大きいと動きがカクカクしてしまう

// TCBと更新
#define ZUKAN_INFO_VBLANK_TCB_PRI               (1)
#define ZUKAN_INFO_BG_UPDATE_BIT_RESET          (0)
#define ZUKAN_INFO_BACK_BG_UPDATE_BIT_SCROLL    (1<<0)

// MSGについて
enum
{
  ZUKAN_INFO_MSG_TOROKU,
  ZUKAN_INFO_MSG_NAME,
  ZUKAN_INFO_MSG_PHYSICAL,
  ZUKAN_INFO_MSG_EXPLAIN,

  ZUKAN_INFO_MSG_MAX,
};

typedef enum
{
  ZUKAN_INFO_ALIGN_LEFT,
  ZUKAN_INFO_ALIGN_RIGHT,
  ZUKAN_INFO_ALIGN_CENTER,
}
ZUKAN_INFO_ALIGN;

#define ZUKAN_INFO_STRBUF_LEN (128)  // この文字数で足りるかbuflen.hで要確認

// ステップ
typedef enum
{
  ZUKAN_INFO_STEP_COLOR_WAIT,    // TOROKU
  ZUKAN_INFO_STEP_COLOR_WHILE,   // TOROKU
  ZUKAN_INFO_STEP_COLOR,         // TOROKU
  ZUKAN_INFO_STEP_MOVE_WAIT,     // TOROKU
  ZUKAN_INFO_STEP_MOVE,          // TOROKU
  ZUKAN_INFO_STEP_CENTER,        // NICKNAME
  ZUKAN_INFO_STEP_CENTER_STILL,  // TOROKU or NICKNAME
  ZUKAN_INFO_STEP_START_STILL,   // LIST
}
ZUKAN_INFO_STEP;

// ポケモン2Dについて
#define ZUKAN_INFO_START_POKEMON_POS_X   (8*6+2)//(8*6)  // ポケモン2Dは12chara x 12chara
                                                       // 左上のピクセル座標が(0, 24)もしくは(2,24)になるように。
#define ZUKAN_INFO_START_POKEMON_POS_Y   (8*(3+6))

#define ZUKAN_INFO_CENTER_POKEMON_POS_X  (8*16)
#define ZUKAN_INFO_CENTER_POKEMON_POS_Y  (8*(3+6))

// ポケモン2Dに乗るWND
#define ZUKAN_INFO_WND_DOWN_Y_START    (8*17)
#define ZUKAN_INFO_WND_DOWN_Y_GOAL     (8*2)
#define ZUKAN_INFO_WND_UP_Y            (8*1)
#define ZUKAN_INFO_WND_LEFT_X          (0)
#define ZUKAN_INFO_WND_RIGHT_X         (100)

#define ZUKAN_INFO_COLOR_WHILE         (15)  // ポケモン2Dに乗るWNDが動き出すまでの待ちフレーム数

// パレットアニメーション
#define ZUKAN_INFO_PALETTE_ANIME_LINE          (0x1)        // 16本中のどれをパレットアニメーションするか
#define ZUKAN_INFO_PALETTE_ANIME_LINE_S        (0x1)        // 16本中のどれがパレットアニメーションのスタート色のあるラインか
#define ZUKAN_INFO_PALETTE_ANIME_LINE_E        (0x2)        // 16本中のどれがパレットアニメーションのエンド色のあるラインか
#define ZUKAN_INFO_PALETTE_ANIME_NO            (0x1)        // 16色中のどこからパレットアニメーションの対象として開始するか
#define ZUKAN_INFO_PALETTE_ANIME_NUM           (5)          // パレットアニメーションの対象として何色あるか
#define ZUKAN_INFO_PALETTE_ANIME_VALUE         (0x400)      // cos使うので0〜0xFFFFのループ
#define ZUKAN_INFO_PALETTE_ANIME_VALUE_MAX     (0x10000)

// Yオフセット
#define ZUKAN_INFO_Y_OFFSET      (-8*3)      // 図鑑の説明画面として下のディスプレイ(メイン)に表示するとき

// 外国語図鑑
#define FOREIGN_MONSNO_MAX  (493)  // 0はポケモンではないがデータあり、1がフシギダネ、493がアルセウスまで存在する、イッシュから追加された494から存在しない
#define FOREIGN_MONSNO_GIRATHINA_FORM  (FOREIGN_MONSNO_MAX +1)  // ギラティナだけ別フォルム用のテキストがある(487はフォルム0番、494はフォルム1番のデータ)

static const u16 narc_msg_name[ZUKAN_INFO_LANG_MAX] =
{
  NARC_message_zkn_monsname_e_dat,
  NARC_message_zkn_monsname_fra_dat,
  NARC_message_zkn_monsname_ger_dat,
  NARC_message_zkn_monsname_ita_dat,
  NARC_message_zkn_monsname_spa_dat,
  NARC_message_zkn_monsname_kor_dat,
};
static const u16 narc_msg_type[ZUKAN_INFO_LANG_MAX] =
{
  NARC_message_zkn_type_e_dat,
  NARC_message_zkn_type_fra_dat,
  NARC_message_zkn_type_ger_dat,
  NARC_message_zkn_type_ita_dat,
  NARC_message_zkn_type_spa_dat,
  NARC_message_zkn_type_kor_dat,
};
static const u16 narc_msg_height[ZUKAN_INFO_LANG_MAX] =
{
  NARC_message_zkn_height_e_dat,
  NARC_message_zkn_height_fra_dat,
  NARC_message_zkn_height_ger_dat,
  NARC_message_zkn_height_ita_dat,
  NARC_message_zkn_height_spa_dat,
  NARC_message_zkn_height_kor_dat,
};
static const u16 narc_msg_weight[ZUKAN_INFO_LANG_MAX] =
{
  NARC_message_zkn_weight_e_dat,
  NARC_message_zkn_weight_fra_dat,
  NARC_message_zkn_weight_ger_dat,
  NARC_message_zkn_weight_ita_dat,
  NARC_message_zkn_weight_spa_dat,
  NARC_message_zkn_weight_kor_dat,
};
static const u16 narc_msg_explain[ZUKAN_INFO_LANG_MAX] =
{
  NARC_message_zkn_comment_e_dat,
  NARC_message_zkn_comment_fra_dat,
  NARC_message_zkn_comment_ger_dat,
  NARC_message_zkn_comment_ita_dat,
  NARC_message_zkn_comment_spa_dat,
  NARC_message_zkn_comment_kor_dat,
};
static const u16 str_id_cap_height[ZUKAN_INFO_LANG_MAX] =
{
  ZKN_CAP_HEIGHT_E,
  ZKN_CAP_HEIGHT_FRA,
  ZKN_CAP_HEIGHT_GER,
  ZKN_CAP_HEIGHT_ITA,
  ZKN_CAP_HEIGHT_SPA,
  ZKN_CAP_HEIGHT_KOR,
};
static const u16 str_id_cap_weight[ZUKAN_INFO_LANG_MAX] =
{
  ZKN_CAP_WEIGHT_E,
  ZKN_CAP_WEIGHT_FRA,
  ZKN_CAP_WEIGHT_GER,
  ZKN_CAP_WEIGHT_ITA,
  ZKN_CAP_WEIGHT_SPA,
  ZKN_CAP_WEIGHT_KOR,
};

// 外国語のタイプアイコン
typedef struct
{
  u8 tate;  // 6個なので0<= <6
  u8 yoko;  // 4個なので0<= <4
}
TYPEICON_POS;
static const TYPEICON_POS typeicon_pos_tbl[POKETYPE_MAX] =  // include/poke_tool/poketype_def.h
{
  // { tate, yoko }
  { 0, 0 },  // POKETYPE_NORMAL 
  { 2, 1 },  // POKETYPE_KAKUTOU
  { 2, 0 },  // POKETYPE_HIKOU  
  { 3, 0 },  // POKETYPE_DOKU   
  { 1, 2 },  // POKETYPE_JIMEN  
  { 1, 1 },  // POKETYPE_IWA    
  { 2, 3 },  // POKETYPE_MUSHI  
  { 2, 2 },  // POKETYPE_GHOST  
  { 3, 2 },  // POKETYPE_HAGANE 
  { 0, 1 },  // POKETYPE_HONOO  
  { 0, 2 },  // POKETYPE_MIZU   
  { 0, 3 },  // POKETYPE_KUSA   
  { 1, 0 },  // POKETYPE_DENKI  
  { 3, 1 },  // POKETYPE_ESPER  
  { 1, 3 },  // POKETYPE_KOORI  
  { 4, 0 },  // POKETYPE_DRAGON 
  { 3, 3 },  // POKETYPE_AKU    
/*
  { 4, 1 },  // ???    
  { 4, 2 },  // COOL    
  { 4, 3 },  // BEAUTY
  { 5, 0 },  // CUTE
  { 5, 1 },  // SMART    
  { 5, 2 },  // TOUGH    
*/
};

static const u16 typeicon_ncg_data_id_tbl[ZUKAN_INFO_LANG_MAX] =
{
  NARC_zukan_gra_info_st_type_eng_NCGR,
  NARC_zukan_gra_info_st_type_fra_NCGR,
  NARC_zukan_gra_info_st_type_ger_NCGR,
  NARC_zukan_gra_info_st_type_ita_NCGR,
  NARC_zukan_gra_info_st_type_spa_NCGR,
  NARC_zukan_gra_info_st_type_kor_NCGR,
};


//=============================================================================
/**
 *  構造体宣言
 */
//=============================================================================
struct _ZUKAN_INFO_WORK
{
  // 引数
  HEAPID                  heap_id;         // 他のところのを覚えているだけで生成や破棄はしない。
  BOOL                    zenkoku_flag;
  BOOL                    get_flag;
  u16                     monsno;
  u16                     formno;          // 0<=formno<POKETOOL_GetPersonalParam( monsno, 0, POKEPER_ID_form_max )
  u16                     sex;             // PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN
  u16                     rare;            // 0(レアじゃない)か1(レア)  // PP_CheckRare( pp ) 
  u32                     personal_rnd;    // ?  // personal_rndはmonsno==MONSNO_PATTIIRUのときは必須
  ZUKAN_INFO_LAUNCH       launch;
  ZUKAN_INFO_DISP         disp;
  u8                      bg_priority;
  GFL_CLUNIT*             clunit;          // 他のところのを覚えているだけで生成や破棄はしない。
  GFL_FONT*               font;            // 他のところのを覚えているだけで生成や破棄はしない。
  PRINT_QUE*              print_que;       // 他のところのを覚えているだけで生成や破棄はしない。

  // BG
  // fore = setumei, back = base
  GFL_ARCUTIL_TRANSINFO     fore_bg_chara_info;    ///< foreのBGキャラ領域
  GFL_ARCUTIL_TRANSINFO     back_bg_chara_info;    ///< backのBGキャラ領域
  u8                        fore_bg_update;        ///< foreのBG更新ビットフラグ
  u8                        back_bg_update;        ///< backのBG更新ビットフラグ

  // 何も表示しないとき用の背景
  GFL_ARCUTIL_TRANSINFO     back_none_bg_chara_info;    // 何も表示しないとき用の背景のBGキャラ領域
  BOOL                      back_none_bg_need_free;     // 読み込んでいたらTRUE
  BOOL                      back_none_bg_display_mode;  // 何も表示しないとき用の背景のみを表示しているモードのときTRUE

  PALTYPE    bg_paltype;
  u8         msg_bg_frame;
  u8         fore_bg_frame;
  u8         back_bg_frame;

  // TCB
  GFL_TCB*     vblank_tcb;      ///< VBlank中のTCB

  // ステップやタイミング
  ZUKAN_INFO_STEP      step;
  BOOL                 move_flag;
  BOOL                 cry_flag;
  u32                  color_while_count;
  u32                  back_bg_scroll_wait_count;

  // WND
  int wnd_down_y;  // ウィンドウの右下Y座標

  //MSG系
  GFL_BMPWIN*  bmpwin[ZUKAN_INFO_MSG_MAX];

  // モンスターボールのマーク
  GFL_BMPWIN*  ball_yes_bmpwin;
  GFL_BMPWIN*  ball_no_bmpwin;

  // ポケモン2D
  u32          ncg_poke2d;
  u32          ncl_poke2d;
  u32          nce_poke2d;
  GFL_CLWK*    clwk_poke2d;

  // タイプアイコン
  u32       typeicon_cg_idx[2];
  u32       typeicon_cl_idx;
  u32       typeicon_cean_idx;
  GFL_CLWK* typeicon_clwk[2];

  // ポケモンの足跡
  UI_EASY_CLWK_RES clres_pokefoot;
  GFL_CLWK* clwk_pokefoot;

  // パレットアニメーション
  u16 anm_cnt;
  u16 trans_buf[ZUKAN_INFO_PALETTE_ANIME_NUM];  // 型のサイズはZUKAN_INFO_COLOR_SIZE
  u16 palette_anime_s[ZUKAN_INFO_PALETTE_ANIME_NUM];
  u16 palette_anime_e[ZUKAN_INFO_PALETTE_ANIME_NUM];

  // Yオフセット
  s32 y_offset;
};

//=============================================================================
/**
*  ローカル関数のプロトタイプ宣言
*/
//=============================================================================
static void Zukan_Info_VBlankFunc( GFL_TCB* tcb, void* wk );

// Message
static void Zukan_Info_DrawStr( HEAPID heap_id, GFL_BMPWIN* bmpwin, GFL_MSGDATA* msgdata, PRINT_QUE* print_que, GFL_FONT* font,
                                u32 str_id, u16 x, u16 y, u16 color, ZUKAN_INFO_ALIGN align, WORDSET* wordset );
static void Zukan_Info_CreateMessage( ZUKAN_INFO_WORK* work );
static void Zukan_Info_CreateForeignMessage( ZUKAN_INFO_WORK* work, ZUKAN_INFO_LANG lang );
static void Zukan_Info_DeleteMessage( ZUKAN_INFO_WORK* work );

// モンスターボールのマーク
static void Zukan_Info_CreateBall( ZUKAN_INFO_WORK* work );
static void Zukan_Info_DeleteBall( ZUKAN_INFO_WORK* work );
static void Zukan_Info_TransBall_VBlank( ZUKAN_INFO_WORK* work );

//ポケモン2D
static void Zukan_Info_Poke2dLoadResourceObj( ZUKAN_INFO_WORK* work );
static void Zukan_Info_Poke2dUnloadResourceObj( ZUKAN_INFO_WORK* work );
static void Zukan_Info_Poke2dCreateCLWK( ZUKAN_INFO_WORK* work, u16 pos_x, u16 pos_y );
static void Zukan_Info_Poke2dDeleteCLWK( ZUKAN_INFO_WORK* work );

// タイプアイコン
static void Zukan_Info_CreateMultiLangTypeicon( ZUKAN_INFO_WORK* work, ZUKAN_INFO_LANG lang );
static void Zukan_Info_CreateTypeicon( ZUKAN_INFO_WORK* work, PokeType type1, PokeType type2 );
static void Zukan_Info_CreateForeignTypeicon( ZUKAN_INFO_WORK* work, PokeType type1, PokeType type2, ZUKAN_INFO_LANG lang );
static void Zukan_Info_DeleteTypeicon( ZUKAN_INFO_WORK* work );

// ポケモンの足跡
static void Zukan_Info_CreatePokefoot( ZUKAN_INFO_WORK* work, u32 monsno );
static void Zukan_Info_DeletePokefoot( ZUKAN_INFO_WORK* work );

// ポケモン2D以外をまとめて
static void Zukan_Info_CreateOthers( ZUKAN_INFO_WORK* work );

// パレットアニメーション
static void Zukan_Info_UpdatePaletteAnime( ZUKAN_INFO_WORK* work );

// 全OBJの表示/非表示を設定する
void ZUKAN_INFO_SetDrawEnableAllObj( ZUKAN_INFO_WORK* work, BOOL on_off );


//=============================================================================
/**
 *  外部公開関数定義
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief     初期化処理
 *
 *  @param[in] a_heap_id         ヒープID
 *  @param[in] a_pp              ポケモンパラメータ
 *  @param[in] a_zenkoku_flag    全国図鑑になっていたらTRUE
 *  @param[in] a_get_flag        ゲットしていたらTRUE
 *  @param[in] a_launch          起動方法
 *  @param[in] a_disp            表示面
 *  @param[in] a_bg_priority     0か1(0のときは012,1のときは123というプライオリティになる)
 *  @param[in] a_clunit          セルアクターユニット
 *  @param[in] a_font            フォント
 *  @param[in] a_print_que       プリントキュー
 *
 *  @retval    生成したワーク
 */
//-----------------------------------------------------------------------------
ZUKAN_INFO_WORK* ZUKAN_INFO_Init(
                     HEAPID                   a_heap_id,
                     const POKEMON_PARAM*     a_pp,
                     BOOL                     a_zenkoku_flag,
                     BOOL                     a_get_flag,
                     ZUKAN_INFO_LAUNCH        a_launch,
                     ZUKAN_INFO_DISP          a_disp,
                     u8                       a_bg_priority,
                     GFL_CLUNIT*              a_clunit,
                     GFL_FONT*                a_font,
                     PRINT_QUE*               a_print_que )
{
  //BOOL fast         = PP_FastModeOn( a_pp );  // 高速化モードはconstはずさないといけないので、止めた
  u16  monsno       = (u16)PP_Get( a_pp, ID_PARA_monsno, NULL );
  u16  formno       = (u16)PP_Get( a_pp, ID_PARA_form_no, NULL );
  u16  sex          = PP_GetSex( a_pp );
  u16  rare         = PP_CheckRare( a_pp );
  u32  personal_rnd = (u16)PP_Get( a_pp, ID_PARA_personal_rnd, NULL );
  //if( fast ) PP_FastModeOff( a_pp, fast );

  return ZUKAN_INFO_InitFromMonsno(
             a_heap_id,
             monsno,
             formno,
             sex,
             rare,
             personal_rnd,
             a_zenkoku_flag,
             a_get_flag,
             a_launch,
             a_disp,
             a_bg_priority,
             a_clunit,
             a_font,
             a_print_que );
}

ZUKAN_INFO_WORK* ZUKAN_INFO_InitFromMonsno(
                            HEAPID                 a_heap_id,
                            u16                    a_monsno,
                            u16                    a_formno,        // 0<=formno<POKETOOL_GetPersonalParam( monsno, 0, POKEPER_ID_form_max )
                            u16                    a_sex,           // PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN
                            u16                    a_rare,          // 0(レアじゃない)か1(レア)  // PP_CheckRare( pp ) 
                            u32                    a_personal_rnd,  // ?  // personal_rndはmonsno==MONSNO_PATTIIRUのときは必須
                            BOOL                   a_zenkoku_flag,
                            BOOL                   a_get_flag,
                            ZUKAN_INFO_LAUNCH      a_launch,
                            ZUKAN_INFO_DISP        a_disp,
                            u8                     a_bg_priority,
                            GFL_CLUNIT*            a_clunit,
                            GFL_FONT*              a_font,
                            PRINT_QUE*             a_print_que )
{
  ZUKAN_INFO_WORK* work;

  // ワーク生成、初期化
  {
    u32 size = sizeof(ZUKAN_INFO_WORK);
    work = GFL_HEAP_AllocMemory( a_heap_id, size );
    GFL_STD_MemClear( work, size );
  }

  // 引数
  {
    work->heap_id             = a_heap_id;
    work->monsno              = a_monsno;
    work->formno              = a_formno;
    work->sex                 = a_sex;   
    work->rare                = a_rare; 
    work->personal_rnd        = a_personal_rnd;
    work->zenkoku_flag        = a_zenkoku_flag;
    work->get_flag            = a_get_flag;
    work->launch              = a_launch;
    work->disp                = a_disp;
    work->bg_priority         = a_bg_priority;
    work->clunit              = a_clunit;
    work->font                = a_font;
    work->print_que           = a_print_que;
  }

  // 初期化
  {
    // BG
    if( work->disp == ZUKAN_INFO_DISP_M )
    {
      work->bg_paltype    = PALTYPE_MAIN_BG;
      work->msg_bg_frame  = ZUKAN_INFO_BG_FRAME_M_MSG; 
      work->fore_bg_frame = ZUKAN_INFO_BG_FRAME_M_FORE; 
      work->back_bg_frame = ZUKAN_INFO_BG_FRAME_M_BACK; 
    }
    else
    {
      work->bg_paltype    = PALTYPE_SUB_BG;
      work->msg_bg_frame  = ZUKAN_INFO_BG_FRAME_S_MSG; 
      work->fore_bg_frame = ZUKAN_INFO_BG_FRAME_S_FORE; 
      work->back_bg_frame = ZUKAN_INFO_BG_FRAME_S_BACK; 
    }
    GFL_BG_SetPriority( work->msg_bg_frame, work->bg_priority );  // 最前面
    GFL_BG_SetPriority( work->fore_bg_frame, work->bg_priority +1 );
    GFL_BG_SetPriority( work->back_bg_frame, work->bg_priority +2 );

    work->fore_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
    work->back_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
 
    // ステップやタイミング
    // 起動方法
    switch( work->launch )
    {
    case ZUKAN_INFO_LAUNCH_TOROKU:
      {
        work->step = ZUKAN_INFO_STEP_COLOR_WAIT;
      }
      break;
    case ZUKAN_INFO_LAUNCH_NICKNAME:
      {
        work->step = ZUKAN_INFO_STEP_CENTER;
      }
      break;
    case ZUKAN_INFO_LAUNCH_LIST:
      {
        work->step = ZUKAN_INFO_STEP_START_STILL;
      }
      break;
    }
    work->move_flag                 = FALSE;
    work->cry_flag                  = FALSE;
    work->color_while_count         = 0;
    work->back_bg_scroll_wait_count = 0;

    // パレットアニメーション
    work->anm_cnt = 0;
        
    // Yオフセット
    if( work->launch == ZUKAN_INFO_LAUNCH_LIST && work->disp == ZUKAN_INFO_DISP_M )
    {
      work->y_offset = ZUKAN_INFO_Y_OFFSET;
    }
    else
    {
      work->y_offset = 0;
    }
  }

  // リソース読み込み
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );
    GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_zukan_gra_info_zukan_bgu_NCLR, work->bg_paltype,
                                      ZUKAN_INFO_BG_PAL_POS_FORE_BACK * ZUKAN_INFO_PAL_LINE_SIZE,
                                      ZUKAN_INFO_BG_PAL_NUM_FORE_BACK * ZUKAN_INFO_PAL_LINE_SIZE,
                                      work->heap_id );

    // fore
    work->fore_bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle, NARC_zukan_gra_info_zukan_bgu_NCGR,
                                                                            work->fore_bg_frame,
                                                                            ZUKAN_INFO_BG_CHARA_SIZE, FALSE,
                                                                            work->heap_id );
    GF_ASSERT_MSG( work->fore_bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_INFO : BGキャラ領域が足りませんでした。\n" );
    
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_zukan_gra_info_setumei_bgu_NSCR, work->fore_bg_frame,
                                     GFL_ARCUTIL_TRANSINFO_GetPos( work->fore_bg_chara_info ),
                                     ZUKAN_INFO_BG_SCREEN_SIZE, FALSE, work->heap_id );

    // back
    work->back_bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle, NARC_zukan_gra_info_zukan_bgu_NCGR,
                                                                            work->back_bg_frame,
                                                                            ZUKAN_INFO_BG_CHARA_SIZE, FALSE, work->heap_id );
    GF_ASSERT_MSG( work->back_bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_INFO : BGキャラ領域が足りませんでした。\n" );
        
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_zukan_gra_info_base_bgu_NSCR, work->back_bg_frame,
                                     GFL_ARCUTIL_TRANSINFO_GetPos( work->back_bg_chara_info ),
                                     ZUKAN_INFO_BG_SCREEN_SIZE, FALSE, work->heap_id );
    
    GFL_ARC_CloseDataHandle( handle );
  }

  // 背景の加工
  {
    // 初期化
    work->back_none_bg_need_free = FALSE;
    work->back_none_bg_display_mode = FALSE;
    
    if( work->launch == ZUKAN_INFO_LAUNCH_LIST && work->disp == ZUKAN_INFO_DISP_S )
    {
      // ポケモンずかん　とうろく　かんりょう！
      // の場所を消す
      GFL_BG_FillScreen( work->fore_bg_frame, 0, 0, 0, 32, 3, GFL_BG_SCRWRT_PALIN );

      // 背景を追加する
      {
        // 何も表示しないとき用の背景
        ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );

        GFL_ARCHDL_UTIL_TransVramPaletteEx(
            handle,
            NARC_zukan_gra_info_info_bgu_NCLR,
            work->bg_paltype,
            0 * 0x20,
            ZUKAN_INFO_BG_PAL_POS_BACK_NONE * 0x20,
            ZUKAN_INFO_BG_PAL_NUM_BACK_NONE * 0x20,
            work->heap_id );

        work->back_none_bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
                                            handle,
                                            NARC_zukan_gra_info_info_bgu_NCGR,
                                            work->back_bg_frame,
                                            32 * 8 * GFL_BG_1CHRDATASIZ,
                                            FALSE,
                                            work->heap_id );
        GF_ASSERT_MSG( work->back_none_bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_INFO : BGキャラ領域が足りませんでした。\n" );

        work->back_none_bg_need_free = TRUE;

        GFL_ARCHDL_UTIL_TransVramScreenCharOfs(
            handle,
            NARC_zukan_gra_info_infobase_bgu_NSCR,
            work->back_bg_frame,
            32 * 32,
            GFL_ARCUTIL_TRANSINFO_GetPos( work->back_none_bg_chara_info ),
            32 * 32 * GFL_BG_1SCRDATASIZ,
            FALSE,
            work->heap_id );

        GFL_BG_ChangeScreenPalette( work->back_bg_frame, 0,    32, 32, 21, ZUKAN_INFO_BG_PAL_POS_BACK_NONE    );
        GFL_BG_ChangeScreenPalette( work->back_bg_frame, 0, 32+21, 32,  3, ZUKAN_INFO_BG_PAL_POS_BACK_NONE +1 );

        GFL_ARC_CloseDataHandle( handle );
      }
    }
  }

  // パレットアニメーション
  {
    NNSG2dPaletteData* pal_data;
    u16* pal_raw;
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );
    u32 size = GFL_ARC_GetDataSizeByHandle( handle, NARC_zukan_gra_info_zukan_bgu_NCLR );  // BGのパレット
    void* pal_src = GFL_HEAP_AllocMemory( work->heap_id, size );
    GFL_ARC_LoadDataByHandle( handle, NARC_zukan_gra_info_zukan_bgu_NCLR, pal_src );
    NNS_G2dGetUnpackedPaletteData( pal_src, &pal_data );
    pal_raw = (u16*)(pal_data->pRawData);
    {
      u8 i;
      for( i=0; i<ZUKAN_INFO_PALETTE_ANIME_NUM; i++ )
      {
        work->palette_anime_s[i] = pal_raw[ ZUKAN_INFO_PAL_LINE_COLOR_NUM*ZUKAN_INFO_PALETTE_ANIME_LINE_S + ZUKAN_INFO_PALETTE_ANIME_NO + i];
        work->palette_anime_e[i] = pal_raw[ ZUKAN_INFO_PAL_LINE_COLOR_NUM*ZUKAN_INFO_PALETTE_ANIME_LINE_E + ZUKAN_INFO_PALETTE_ANIME_NO + i];
      }
    }
    GFL_HEAP_FreeMemory( pal_src );
    GFL_ARC_CloseDataHandle( handle );
  }

  // モンスターボールのマーク
  Zukan_Info_CreateBall( work );
  Zukan_Info_TransBall_VBlank( work );

  // TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Info_VBlankFunc, work, ZUKAN_INFO_VBLANK_TCB_PRI );

  // ポケモン2D以外
  if(    work->launch == ZUKAN_INFO_LAUNCH_TOROKU
      || work->launch == ZUKAN_INFO_LAUNCH_LIST )
  {
    Zukan_Info_CreateOthers( work );
  }

  //ポケモン2D
  {
    u16 pos_x, pos_y;
    switch(work->launch)
    {
    case ZUKAN_INFO_LAUNCH_TOROKU:
    case ZUKAN_INFO_LAUNCH_LIST:
      {
        pos_x = ZUKAN_INFO_START_POKEMON_POS_X;
        pos_y = ZUKAN_INFO_START_POKEMON_POS_Y;
      }
      break;
    case ZUKAN_INFO_LAUNCH_NICKNAME:
      {
        pos_x = ZUKAN_INFO_CENTER_POKEMON_POS_X;
        pos_y = ZUKAN_INFO_CENTER_POKEMON_POS_Y;
      }
      break;
    }
    Zukan_Info_Poke2dLoadResourceObj( work );
    Zukan_Info_Poke2dCreateCLWK( work, pos_x, (u16)( pos_y + work->y_offset ) );
  }

  // WND
  if( work->launch == ZUKAN_INFO_LAUNCH_TOROKU )
  {
    work->wnd_down_y = ZUKAN_INFO_WND_DOWN_Y_START;
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
    G2_SetWnd0Position( ZUKAN_INFO_WND_LEFT_X, ZUKAN_INFO_WND_UP_Y, ZUKAN_INFO_WND_RIGHT_X, work->wnd_down_y );
    G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
                           TRUE );
    G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
                           FALSE );
    G2_SetBlendBrightness( GX_BLEND_PLANEMASK_OBJ,
                           -16 );
  }

  // BGのYオフセット
  GFL_BG_SetScroll( work->fore_bg_frame, GFL_BG_SCROLL_Y_SET, - work->y_offset );
  GFL_BG_SetScroll( work->back_bg_frame, GFL_BG_SCROLL_Y_SET, - work->y_offset );
  GFL_BG_SetScroll( work->msg_bg_frame, GFL_BG_SCROLL_Y_SET, - work->y_offset );

  // BG
  GFL_BG_LoadScreenReq( work->fore_bg_frame );
  GFL_BG_LoadScreenReq( work->back_bg_frame );

  return work;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         終了処理 
 *
 *  @param[in,out] work  ZUKAN_INFO_Initで生成したワーク
 *
 *  @retval        なし 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Exit( ZUKAN_INFO_WORK* work )
{
  // 何も表示しないとき用の背景
  {
    // 何も表示しないとき用の背景から通常の表示にしておく
    ZUKAN_INFO_DisplayNormal( work );
    work->back_none_bg_display_mode = FALSE;
  }

  // ポケモン2D
  Zukan_Info_Poke2dDeleteCLWK( work );
  Zukan_Info_Poke2dUnloadResourceObj( work );

  // ポケモン2D以外
  if( work->launch == ZUKAN_INFO_LAUNCH_LIST )  // TOROKUの場合は既に破棄されている
  {
    ZUKAN_INFO_DeleteOthers( work );
  }

  // TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // モンスターボールのマーク
  Zukan_Info_DeleteBall( work );

  // 読み込んだリソースの破棄
  {
    // 何も表示しないとき用の背景
    if( work->back_none_bg_need_free )
    {
      GFL_BG_FreeCharacterArea( work->back_bg_frame, GFL_ARCUTIL_TRANSINFO_GetPos( work->back_none_bg_chara_info ),
                                GFL_ARCUTIL_TRANSINFO_GetSize( work->back_none_bg_chara_info ) );
      work->back_none_bg_need_free = FALSE;
    }

    // back
    GFL_BG_FreeCharacterArea( work->back_bg_frame, GFL_ARCUTIL_TRANSINFO_GetPos( work->back_bg_chara_info ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->back_bg_chara_info ) );
      
    // fore
    GFL_BG_FreeCharacterArea( work->fore_bg_frame, GFL_ARCUTIL_TRANSINFO_GetPos( work->fore_bg_chara_info ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->fore_bg_chara_info ) );
  }

  // ワーク破棄
  {
    GFL_HEAP_FreeMemory( work );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief         主処理 
 *
 *  @param[in,out] work  ZUKAN_INFO_Initで生成したワーク
 *
 *  @retval        なし 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Main( ZUKAN_INFO_WORK* work )
{
  // パレットアニメーション
  Zukan_Info_UpdatePaletteAnime( work );

  // BGスクロール
  {
    work->back_bg_scroll_wait_count++;
    if( work->back_bg_scroll_wait_count == ZUKAN_INFO_BACK_BG_SCROLL_WAIT )
    {
      work->back_bg_update |= ZUKAN_INFO_BACK_BG_UPDATE_BIT_SCROLL;
      work->back_bg_scroll_wait_count = 0;
    }
  }

  // ステップ
  switch(work->step)
  {
  case ZUKAN_INFO_STEP_COLOR_WAIT:
    {
    }
    break;
  case ZUKAN_INFO_STEP_COLOR_WHILE:
    {
      work->color_while_count++;
      if( work->color_while_count == ZUKAN_INFO_COLOR_WHILE )
      {
        work->step = ZUKAN_INFO_STEP_COLOR;
      }
    }
    break;
  case ZUKAN_INFO_STEP_COLOR:
    {
      work->wnd_down_y--;
      G2_SetWnd0Position( ZUKAN_INFO_WND_LEFT_X, ZUKAN_INFO_WND_UP_Y, ZUKAN_INFO_WND_RIGHT_X, work->wnd_down_y );
      if( work->wnd_down_y == ZUKAN_INFO_WND_DOWN_Y_GOAL )
      {
        GX_SetVisibleWnd( GX_WNDMASK_NONE );
        G2_SetBlendBrightness( GX_BLEND_PLANEMASK_OBJ,
                               0 );
        PMVOICE_Play( work->monsno, work->formno, 64, FALSE, 0, 0, FALSE, 0 );
        work->step = ZUKAN_INFO_STEP_MOVE_WAIT;
      }
    }
    break;
  case ZUKAN_INFO_STEP_MOVE_WAIT:
    {
      if(work->move_flag)
      {
        work->step = ZUKAN_INFO_STEP_MOVE;
      }
    }
    break;
  case ZUKAN_INFO_STEP_MOVE:
    {
      // work->y_offsetを使用するときは、ここには来ない

      BOOL goal_x = FALSE;
      BOOL goal_y = FALSE;

      // ポケモン2D
      GFL_CLACTPOS pos;
      GFL_CLACT_WK_GetWldPos( work->clwk_poke2d, &pos );
      pos.x += 1;
      pos.y += 1;
      if( pos.x > ZUKAN_INFO_CENTER_POKEMON_POS_X )
      {
        pos.x = ZUKAN_INFO_CENTER_POKEMON_POS_X;
        goal_x = TRUE;
      }
      if( pos.y > ZUKAN_INFO_CENTER_POKEMON_POS_Y )
      {
        pos.y = ZUKAN_INFO_CENTER_POKEMON_POS_Y;
        goal_y = TRUE;
      }
      GFL_CLACT_WK_SetWldPos( work->clwk_poke2d, &pos );
            
      if(goal_x && goal_y)
      {
        work->step = ZUKAN_INFO_STEP_CENTER_STILL;
      }
    }
    break;
  case ZUKAN_INFO_STEP_CENTER:
    {
      if(work->cry_flag)
      {
        PMVOICE_Play( work->monsno, work->formno, 64, FALSE, 0, 0, FALSE, 0 );
        work->step = ZUKAN_INFO_STEP_CENTER_STILL;
      }
    }
    break;
  case ZUKAN_INFO_STEP_CENTER_STILL:
    {
    }
    break;
  case ZUKAN_INFO_STEP_START_STILL:
    {
    }
    break;
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief         ポケモン2D以外のものを破棄する 
 *
 *  @param[in,out] work  ZUKAN_INFO_Initで生成したワーク
 *
 *  @retval        なし 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_DeleteOthers( ZUKAN_INFO_WORK* work )
{
  if( work->get_flag )
  {
    // ポケモンの足跡
    Zukan_Info_DeletePokefoot( work );
    // タイプアイコン
    Zukan_Info_DeleteTypeicon( work ); 
  } 
  // Message
  Zukan_Info_DeleteMessage( work );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         ポケモン2Dの移動開始許可フラグを立てる 
 *
 *  @param[in,out] work  ZUKAN_INFO_Initで生成したワーク
 *
 *  @retval        なし
 *
 *  @note          中央へ移動できる段階になったら移動しなさいよ、というフラグを立てるだけなので、直ちに移動するとは限らない。
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Move( ZUKAN_INFO_WORK* work )
{
  work->move_flag = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         ポケモンの鳴き声フラグを立てる
 *
 *  @param[in,out] work  ZUKAN_INFO_Initで生成したワーク
 *
 *  @retval        なし 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Cry( ZUKAN_INFO_WORK* work )
{
  work->cry_flag = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief     ポケモン2Dが移動中か否か 
 *
 *  @param[in] work  ZUKAN_INFO_Initで生成したワーク
 *
 *  @retval    移動を開始していたらTRUEを返す。移動を開始していないときはFALSEを返す。移動を終了していたらFALSEを返す。 
 */
//-----------------------------------------------------------------------------
BOOL ZUKAN_INFO_IsMove( ZUKAN_INFO_WORK* work )
{
  BOOL ret = FALSE;
  if( work->step == ZUKAN_INFO_STEP_MOVE ) ret = TRUE;
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *  @brief     ポケモン2Dが中央でじっとしているか否か
 *
 *  @param[in] work  ZUKAN_INFO_Initで生成したワーク
 *
 *  @retval    中央で静止していたらTRUE 
 */
//-----------------------------------------------------------------------------
BOOL ZUKAN_INFO_IsCenterStill( ZUKAN_INFO_WORK* work )
{
  BOOL ret = FALSE;
  if( work->step == ZUKAN_INFO_STEP_CENTER_STILL ) ret = TRUE;
  return ret;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         処理を開始させる
 *
 *  @param[in,out] work  ZUKAN_INFO_Initで生成したワーク
 *
 *  @retval        なし 
 */
//-----------------------------------------------------------------------------
void ZUKAN_INFO_Start( ZUKAN_INFO_WORK* work )
{
  if( work->step == ZUKAN_INFO_STEP_COLOR_WAIT )
  {
    work->step = ZUKAN_INFO_STEP_COLOR_WHILE;
  }
}

//-------------------------------------
/// ポケモンを変更する
//=====================================
void ZUKAN_INFO_ChangePoke(
                ZUKAN_INFO_WORK* work,
                u16              monsno,
                u16              formno,
                u16              sex,
                u16              rare,
                u32              personal_rnd,
                BOOL             get_flag )
{
  // 前のを削除する
  // ポケモン2D
  Zukan_Info_Poke2dDeleteCLWK( work );
  Zukan_Info_Poke2dUnloadResourceObj( work );
  // ポケモン2D以外
  ZUKAN_INFO_DeleteOthers( work );

  // 次のを生成する
  work->monsno         = monsno;
  work->formno         = formno;
  work->sex            = sex;   
  work->rare           = rare; 
  work->personal_rnd   = personal_rnd;
  work->get_flag       = get_flag;

  // モンスターボールのマーク
  Zukan_Info_TransBall_VBlank( work );

  // ポケモン2D以外
  Zukan_Info_CreateOthers( work );
  //ポケモン2D
  {
    u16 pos_x, pos_y;
    switch(work->launch)
    {
    case ZUKAN_INFO_LAUNCH_TOROKU:
    case ZUKAN_INFO_LAUNCH_LIST:
      {
        pos_x = ZUKAN_INFO_START_POKEMON_POS_X;
        pos_y = ZUKAN_INFO_START_POKEMON_POS_Y;
      }
      break;
    case ZUKAN_INFO_LAUNCH_NICKNAME:
      {
        pos_x = ZUKAN_INFO_CENTER_POKEMON_POS_X;
        pos_y = ZUKAN_INFO_CENTER_POKEMON_POS_Y;
      }
      break;
    }
    Zukan_Info_Poke2dLoadResourceObj( work );
    Zukan_Info_Poke2dCreateCLWK( work, pos_x, (u16)( pos_y + work->y_offset ) );
  }

  // 全OBJの表示/非表示を設定する
  ZUKAN_INFO_SetDrawEnableAllObj( work, !work->back_none_bg_display_mode );
}

//-------------------------------------
/// 言語を変更する
//=====================================
void ZUKAN_INFO_ChangeLang( ZUKAN_INFO_WORK* work,
                ZUKAN_INFO_LANG lang )
{
  // 前のを削除する
  if( work->get_flag )
  {
    Zukan_Info_DeleteTypeicon( work );
  }
  Zukan_Info_DeleteMessage( work );
  
  // 次のを生成する
  if( lang == ZUKAN_INFO_LANG_NONE || work->monsno > FOREIGN_MONSNO_MAX )
  {
    Zukan_Info_CreateMessage( work );
    if( work->get_flag )
    {
      Zukan_Info_CreateMultiLangTypeicon( work, lang );
    }
  }
  else
  {
    Zukan_Info_CreateForeignMessage( work, lang );
    if( work->get_flag )
    {
      Zukan_Info_CreateMultiLangTypeicon( work, lang );
    }
  }
}

//-------------------------------------
/// ポケモンと言語を変更する
//=====================================
void ZUKAN_INFO_ChangePokeAndLang(
                ZUKAN_INFO_WORK* work,
                u16              monsno,
                u16              formno,
                u16              sex,
                u16              rare,
                u32              personal_rnd,
                BOOL             get_flag,
                ZUKAN_INFO_LANG  lang )
{
  ZUKAN_INFO_ChangePoke( work, monsno, formno, sex, rare, personal_rnd, get_flag );
  ZUKAN_INFO_ChangeLang( work, lang );
}

//-------------------------------------
/// 何も表示しないとき用の背景のみを表示する
//=====================================
void ZUKAN_INFO_DisplayBackNone( ZUKAN_INFO_WORK* work )
{
  u8 i;
  
  if( work->back_none_bg_display_mode ) return;  // 既に、何も表示しないとき用の背景のみを表示している

  work->back_none_bg_display_mode = TRUE;

  // BG
  GFL_BG_SetVisible( work->msg_bg_frame,  VISIBLE_OFF );
  GFL_BG_SetVisible( work->fore_bg_frame, VISIBLE_OFF );

  GFL_BG_SetScrollReq( work->back_bg_frame, GFL_BG_SCROLL_Y_SET, -32*8 );

  // 全OBJの表示/非表示を設定する
  ZUKAN_INFO_SetDrawEnableAllObj( work, !work->back_none_bg_display_mode );
}

//-------------------------------------
/// 通常の表示にする
//=====================================
void ZUKAN_INFO_DisplayNormal( ZUKAN_INFO_WORK* work )
{
  u8 i;
  
  if( !(work->back_none_bg_display_mode) ) return;  // 既に、通常の表示

  work->back_none_bg_display_mode = FALSE;
  
  // BG
  GFL_BG_SetVisible( work->msg_bg_frame,  VISIBLE_ON );
  GFL_BG_SetVisible( work->fore_bg_frame, VISIBLE_ON );

  GFL_BG_SetScrollReq( work->back_bg_frame, GFL_BG_SCROLL_Y_SET, - work->y_offset );

  // 全OBJの表示/非表示を設定する
  ZUKAN_INFO_SetDrawEnableAllObj( work, !work->back_none_bg_display_mode );
}


//=============================================================================
/**
*  ローカル関数定義
*/
//=============================================================================
//-------------------------------------
/// VBlank中に呼び出される関数
//=====================================
static void Zukan_Info_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_INFO_WORK* work = (ZUKAN_INFO_WORK*)wk;

  if( work->back_bg_update & ZUKAN_INFO_BACK_BG_UPDATE_BIT_SCROLL )
  {
    GFL_BG_SetScrollReq( work->back_bg_frame, GFL_BG_SCROLL_X_INC, ZUKAN_INFO_BACK_BG_SCROLL_X_VALUE );
  }

  work->fore_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
  work->back_bg_update = ZUKAN_INFO_BG_UPDATE_BIT_RESET;
}

//-------------------------------------
/// 文字を書く
//=====================================
static void Zukan_Info_DrawStr( HEAPID heap_id, GFL_BMPWIN* bmpwin, GFL_MSGDATA* msgdata, PRINT_QUE* print_que, GFL_FONT* font,
                                u32 str_id, u16 x, u16 y, u16 color, ZUKAN_INFO_ALIGN align, WORDSET* wordset )
{
  // LEFTのときは(x,y)が左上になるように書く
  // RIGHTのときは(x,y)が右上になるように書く
  // CENTERのときは(bmpwinの左右中央になる位置,y)になるように書く(xは使用しない)

  GFL_BMP_DATA* bmp_data = GFL_BMPWIN_GetBmp( bmpwin );
  u16 ax;
  STRBUF* strbuf;
  u16 str_width;
  u16 bmp_width;

  if( wordset )
  {
    STRBUF* src_strbuf = GFL_MSG_CreateString( msgdata, str_id );
    strbuf = GFL_STR_CreateBuffer( ZUKAN_INFO_STRBUF_LEN, heap_id );
    WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
    GFL_STR_DeleteBuffer( src_strbuf );
  }
  else
  {
    strbuf = GFL_MSG_CreateString( msgdata, str_id );
  }

  switch( align )
  {
  case ZUKAN_INFO_ALIGN_LEFT:
    {
      ax = x;
    }
    break;
  case ZUKAN_INFO_ALIGN_RIGHT:
    {
      str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, font, 0 ) );
      ax = x - str_width;
    }
    break;
  case ZUKAN_INFO_ALIGN_CENTER:
    {
      str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, font, 0 ) );
      bmp_width = GFL_BMP_GetSizeX( bmp_data );
      ax = ( bmp_width - str_width ) / 2;
    }
    break;
  }
 
  PRINTSYS_PrintQueColor( print_que, bmp_data, 
                          ax, y, strbuf, font, color );
        
  GFL_STR_DeleteBuffer( strbuf );
}

//-------------------------------------
/// BGに文字を書く
//=====================================
static void Zukan_Info_CreateMessage( ZUKAN_INFO_WORK* work )
{
  u16 monsno_formno_pos;

  GFL_MSGDATA* msgdata_common;
  GFL_MSGDATA* msgdata_kind;
  GFL_MSGDATA* msgdata_height;
  GFL_MSGDATA* msgdata_weight;
  GFL_MSGDATA* msgdata_explain;

  PALTYPE paltype = (work->disp==ZUKAN_INFO_DISP_M)?(PALTYPE_MAIN_BG):(PALTYPE_SUB_BG);

  msgdata_common = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_dat, work->heap_id );
  msgdata_kind = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_type_dat, work->heap_id );
  msgdata_height = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_height_dat, work->heap_id );
  msgdata_weight = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_weight_dat, work->heap_id );
#if	PM_VERSION == VERSION_BLACK
  msgdata_explain = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_comment_01_dat, work->heap_id );
#else
  msgdata_explain = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_comment_00_dat, work->heap_id );
#endif

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, paltype,
                                 ZUKAN_INFO_BG_PAL_POS_MSG * ZUKAN_INFO_PAL_LINE_SIZE,
                                 ZUKAN_INFO_BG_PAL_NUM_MSG * ZUKAN_INFO_PAL_LINE_SIZE,
                                 work->heap_id );
  
  // 次のフォルムデータの位置を参照できるリスト
  {
    u32 size;
    u16 next_form_pos_num;
    u16* next_form_pos_list = GFL_ARC_UTIL_LoadEx( ARCID_ZUKAN_DATA, NARC_zukan_data_zkn_next_form_pos_dat, FALSE, work->heap_id, &size );
    next_form_pos_num = (u16)( size / sizeof(u16) );
    GF_ASSERT_MSG( next_form_pos_num > MONSNO_END+1+1, "ZUKAN_INFO : 次のフォルムのデータの位置を参照できる列の個数が足りません。\n" );

    // 次のフォルムデータの位置を参照できるリストを利用して、フォルムデータの位置を得る
    {
      u16 pos = work->monsno;
      {
        u16 formno_count = 0;
        while( formno_count != work->formno )
        {
          pos = next_form_pos_list[pos];
          if( pos == 0 ) break;
          formno_count++;
        }
        GF_ASSERT_MSG( pos > 0, "ZUKAN_INFO : フォルム番号が異常です。\n" );
      }
      monsno_formno_pos = pos;
    }

    GFL_HEAP_FreeMemory( next_form_pos_list );
  }

  {
    s32 i;

    // キャラクター単位でX座標、Y座標、Xサイズ、Yサイズを指定する
    const u8 pos_siz[ZUKAN_INFO_MSG_MAX][4] =
    {
      {  2,  0, 28,  3 },
      { 16,  4, 15,  5 },
      { 18, 12, 13,  5 },
      {  2, 17, 28,  7 },
    };

    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      // bmpwinのビットマップを透明色で塗り潰しておいたほうがいい？→GFL_BMP_Clearすることにした
      work->bmpwin[i] = GFL_BMPWIN_Create( work->msg_bg_frame,
                                           pos_siz[i][0], pos_siz[i][1], pos_siz[i][2], pos_siz[i][3],
                                           ZUKAN_INFO_BG_PAL_POS_MSG, GFL_BMP_CHRAREA_GET_B );
      // クリア
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->bmpwin[i] ), 0 );
    }

    if( work->launch == ZUKAN_INFO_LAUNCH_LIST && work->disp == ZUKAN_INFO_DISP_S )
    {
      // ポケモンずかん　とうろく　かんりょう！
      // の場所に何も書かないようにする
    }
    else
    {
      // ポケモンずかん　とうろく　かんりょう！
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_TOROKU], msgdata_common,
                          work->print_que, work->font,
                          ZKN_POKEGET_00, 0, 5, PRINTSYS_LSB_Make(0xF,2,0), ZUKAN_INFO_ALIGN_CENTER, NULL );
    }

    // 025(ポケモンの番号)
    {
      u16 disp_no = work->monsno;
      if( !(work->zenkoku_flag) )
      {
        u16* chihou_no_list = POKE_PERSONAL_GetZenkokuToChihouArray( work->heap_id, NULL );
        disp_no = chihou_no_list[work->monsno];
        GFL_HEAP_FreeMemory( chihou_no_list );
      }
      if( disp_no != 0 )  // 番号判明
      {
        WORDSET* wordset = WORDSET_Create( work->heap_id );
        WORDSET_RegisterNumber( wordset, 0, disp_no, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
        Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_common,
                            work->print_que, work->font,
                            ZNK_POKELIST_17, 8, 5, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, wordset );
        WORDSET_Delete( wordset );
      }
      else  // 番号不明
      {
        Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_common,
                            work->print_que, work->font,
                            ZKN_NO_UNKNOWN, 8, 5, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );
      }
    }
    
    // 例：ピカチュウ
    {
      STRBUF* strbuf;
      strbuf = GFL_MSG_CreateString( GlobalMsg_PokeName, work->monsno );
      PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp( work->bmpwin[ZUKAN_INFO_MSG_NAME] ),
                              48, 5, strbuf, work->font, PRINTSYS_LSB_Make(1,2,0) );  
      GFL_STR_DeleteBuffer( strbuf );
    }

    // 例：ねずみポケモン
    {
      u32 disp_id = ZKN_TYPE_000;  // 種族不明
      if( work->get_flag )
      {
        disp_id = ZKN_TYPE_000 + work->monsno;  // 種族判明
      }
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_kind, work->print_que, work->font,
                          disp_id, 8, 22, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );
    }
    
    // たかさ
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_common, work->print_que, work->font,
                        ZNK_POKELIST_19, 0, 4, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    
    // おもさ
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_common, work->print_que, work->font,
                        ZNK_POKELIST_20, 0, 20, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    
    // ？？？.？m  // 左上指定なら(40, 4)  // mとkgで小数点が揃うように
    {
      u32 disp_id = ZKN_HEIGHT_000_000;  // たかさ不明
      if( work->get_flag )
      {
        disp_id = ZKN_HEIGHT_000_000 + monsno_formno_pos;  // たかさ判明
      }
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_height, work->print_que, work->font,
                          disp_id, 93, 4, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_RIGHT, NULL );
    }

    // ？？？.？kg  // 左上指定なら(40,20)
    { 
      u32 disp_id = ZKN_WEIGHT_000_000;  // おもさ不明
      if( work->get_flag )
      {
        disp_id = ZKN_WEIGHT_000_000 + monsno_formno_pos;  // おもさ判明
      }
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_weight, work->print_que, work->font,
                          disp_id, 100, 20, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_RIGHT, NULL );
    }

    // 例：かたい　きのみも　でんげきで
    {
      u32 disp_id = 0;  // テキスト不明    // ZKN_COMMENT_00_000_000  // ZKN_COMMENT_01_000_000
      if( work->get_flag )
      {
        disp_id = 0 + monsno_formno_pos;  // テキスト判明
      }
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_EXPLAIN], msgdata_explain, work->print_que, work->font,
                          disp_id, 4, 5, PRINTSYS_LSB_Make(0xF,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    }

    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->bmpwin[i] );
    }
  }

  GFL_MSG_Delete(msgdata_explain);
  GFL_MSG_Delete(msgdata_weight);
  GFL_MSG_Delete(msgdata_height);
  GFL_MSG_Delete(msgdata_kind);
  GFL_MSG_Delete(msgdata_common);
}

static void Zukan_Info_CreateForeignMessage( ZUKAN_INFO_WORK* work, ZUKAN_INFO_LANG lang )
{
  u16 monsno_formno_pos;

  GFL_MSGDATA* msgdata_common;
  GFL_MSGDATA* msgdata_kind;
  GFL_MSGDATA* msgdata_height;
  GFL_MSGDATA* msgdata_weight;
  GFL_MSGDATA* msgdata_explain;
  GFL_MSGDATA* msgdata_name;

  PALTYPE paltype = (work->disp==ZUKAN_INFO_DISP_M)?(PALTYPE_MAIN_BG):(PALTYPE_SUB_BG);

  msgdata_common = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_zkn_dat, work->heap_id );
  msgdata_kind = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, narc_msg_type[lang], work->heap_id );
  msgdata_height = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, narc_msg_height[lang], work->heap_id );
  msgdata_weight = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, narc_msg_weight[lang], work->heap_id );
  msgdata_explain = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, narc_msg_explain[lang], work->heap_id );
  msgdata_name   = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, narc_msg_name[lang], work->heap_id );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, paltype,
                                 ZUKAN_INFO_BG_PAL_POS_MSG * ZUKAN_INFO_PAL_LINE_SIZE,
                                 ZUKAN_INFO_BG_PAL_NUM_MSG * ZUKAN_INFO_PAL_LINE_SIZE,
                                 work->heap_id );
  
  // 次のフォルムデータの位置を参照できるリスト
  {
    monsno_formno_pos = work->monsno;
    if( work->monsno == MONSNO_GIRATHINA  && work->formno != 0 )
    {
      monsno_formno_pos = FOREIGN_MONSNO_GIRATHINA_FORM;
    }
  }

  {
    s32 i;

    // キャラクター単位でX座標、Y座標、Xサイズ、Yサイズを指定する
    const u8 pos_siz[ZUKAN_INFO_MSG_MAX][4] =
    {
      {  2,  0, 28,  3 },
      { 16,  4, 15,  5 },
      { 18, 12, 13,  5 },
      {  2, 17, 28,  7 },
    };

    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      // bmpwinのビットマップを透明色で塗り潰しておいたほうがいい？→GFL_BMP_Clearすることにした
      work->bmpwin[i] = GFL_BMPWIN_Create( work->msg_bg_frame,
                                           pos_siz[i][0], pos_siz[i][1], pos_siz[i][2], pos_siz[i][3],
                                           ZUKAN_INFO_BG_PAL_POS_MSG, GFL_BMP_CHRAREA_GET_B );
      // クリア
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->bmpwin[i] ), 0 );
    }

    if( work->launch == ZUKAN_INFO_LAUNCH_LIST && work->disp == ZUKAN_INFO_DISP_S )
    {
      // ポケモンずかん　とうろく　かんりょう！
      // の場所に何も書かないようにする
    }
    else
    {
      // ポケモンずかん　とうろく　かんりょう！
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_TOROKU], msgdata_common,
                          work->print_que, work->font,
                          ZKN_POKEGET_00, 0, 5, PRINTSYS_LSB_Make(0xF,2,0), ZUKAN_INFO_ALIGN_CENTER, NULL );
    }

    // 025(ポケモンの番号)
    {
      u16 disp_no = work->monsno;
      if( !(work->zenkoku_flag) )
      {
        u16* chihou_no_list = POKE_PERSONAL_GetZenkokuToChihouArray( work->heap_id, NULL );
        disp_no = chihou_no_list[work->monsno];
        GFL_HEAP_FreeMemory( chihou_no_list );
      }
      if( disp_no != 0 )  // 番号判明
      {
        WORDSET* wordset = WORDSET_Create( work->heap_id );
        WORDSET_RegisterNumber( wordset, 0, disp_no, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
        Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_common,
                            work->print_que, work->font,
                            ZNK_POKELIST_17, 8, 5, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, wordset );
        WORDSET_Delete( wordset );
      }
      else  // 番号不明
      {
        Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_common,
                            work->print_que, work->font,
                            ZKN_NO_UNKNOWN, 8, 5, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );
      }
    }
    
    // 例：ピカチュウ
    {
      u32 disp_id = 0 + monsno_formno_pos;  // 名前判明
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_name, work->print_que, work->font,
                          disp_id, 48, 5, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    }

    // 例：ねずみポケモン
    {
      u32 disp_id = 0;  // 種族不明
      if( work->get_flag )
      {
        disp_id = 0 + monsno_formno_pos;  // 種族判明
      }
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_NAME], msgdata_kind, work->print_que, work->font,
                          disp_id, 8, 22, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );
    }

    // たかさ
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_common, work->print_que, work->font,
                        str_id_cap_height[lang], 0, 4, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    
    // おもさ
    Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_common, work->print_que, work->font,
                        str_id_cap_weight[lang], 0, 20, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    
    // ？？？.？m  // 左上指定なら(40, 4)  // mとkgで小数点が揃うように
    {
      u32 disp_id = 0;  // たかさ不明
      if( work->get_flag )
      {
        disp_id = 0 + monsno_formno_pos;  // たかさ判明
      }
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_height, work->print_que, work->font,
                          disp_id, 93, 4, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_RIGHT, NULL );
    }

    // ？？？.？kg  // 左上指定なら(40,20)
    { 
      u32 disp_id = 0;  // おもさ不明
      if( work->get_flag )
      {
        disp_id = 0 + monsno_formno_pos;  // おもさ判明
      }
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_PHYSICAL], msgdata_weight, work->print_que, work->font,
                          disp_id, 100, 20, PRINTSYS_LSB_Make(1,2,0), ZUKAN_INFO_ALIGN_RIGHT, NULL );
    }

    // 例：かたい　きのみも　でんげきで
    {
      u32 disp_id = 0;  // テキスト不明    // ZKN_COMMENT_00_000_000  // ZKN_COMMENT_01_000_000
      if( work->get_flag )
      {
        disp_id = 0 + monsno_formno_pos;  // テキスト判明
      }
      Zukan_Info_DrawStr( work->heap_id, work->bmpwin[ZUKAN_INFO_MSG_EXPLAIN], msgdata_explain, work->print_que, work->font,
                          disp_id, 4, 5, PRINTSYS_LSB_Make(0xF,2,0), ZUKAN_INFO_ALIGN_LEFT, NULL );      
    }

    for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->bmpwin[i] );
    }
  }

  GFL_MSG_Delete(msgdata_name);
  GFL_MSG_Delete(msgdata_explain);
  GFL_MSG_Delete(msgdata_weight);
  GFL_MSG_Delete(msgdata_height);
  GFL_MSG_Delete(msgdata_kind);
  GFL_MSG_Delete(msgdata_common);
}

//-------------------------------------
/// BGに書いた文字を破棄する
//=====================================
static void Zukan_Info_DeleteMessage( ZUKAN_INFO_WORK* work )
{
  s32 i;
  for( i=0; i<ZUKAN_INFO_MSG_MAX; i++ )
  {
    GFL_BMPWIN_Delete( work->bmpwin[i] );
  }
}

//-------------------------------------
/// モンスターボールのマーク
//=====================================
static void Zukan_Info_CreateBall( ZUKAN_INFO_WORK* work )
{
  GFL_BMP_DATA* bmp_data;
  bmp_data = GFL_BMP_LoadCharacter( ARCID_ZUKAN_GRA, NARC_zukan_gra_info_zukan_bgu_NCGR, 0, work->heap_id );
  
  // モンスターボールのマークあり
  work->ball_yes_bmpwin = GFL_BMPWIN_Create( work->fore_bg_frame, 12, 4, 4, 3, 0x0, GFL_BMP_CHRAREA_GET_F );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_yes_bmpwin), 8*1, 8*0, 0, 0, 8*4, 8*3, 0x0 );

  // モンスターボールのマークなし
  work->ball_no_bmpwin = GFL_BMPWIN_Create( work->fore_bg_frame, 12, 4, 4, 3, 0x0, GFL_BMP_CHRAREA_GET_F );
/*
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*26, 8*0,   0,   0, 8*4, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*11, 8*1,   0, 8*1, 8*4, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*18, 8*1, 8*1, 8*2, 8*2, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8* 1, 8*2, 8*0, 8*2, 8*1, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8* 4, 8*2, 8*3, 8*2, 8*1, 8*1, 0x0 );
*/
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x15, 8*0x01,   0,   0, 8*4, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x1F, 8*0x01,   0, 8*1, 8*1, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x05, 8*0x02, 8*1, 8*1, 8*3, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x10, 8*0x00, 8*0, 8*2, 8*1, 8*1, 0x0 );
  GFL_BMP_Print( bmp_data, GFL_BMPWIN_GetBmp(work->ball_no_bmpwin), 8*0x0B, 8*0x02, 8*1, 8*2, 8*3, 8*1, 0x0 );

  GFL_BMP_Delete( bmp_data );
}
static void Zukan_Info_DeleteBall( ZUKAN_INFO_WORK* work )
{
  GFL_BMPWIN_Delete( work->ball_yes_bmpwin );
  GFL_BMPWIN_Delete( work->ball_no_bmpwin );
}
static void Zukan_Info_TransBall_VBlank( ZUKAN_INFO_WORK* work )
{
  // モンスターボールのマークを付ける
  if( work->get_flag )
  {
    GFL_BMPWIN_MakeTransWindow( work->ball_yes_bmpwin );
  }
  else
  {
    GFL_BMPWIN_MakeTransWindow( work->ball_no_bmpwin );
  }
}

//-------------------------------------
/// ポケモン2Dのリソースを読み込む
//=====================================
static void Zukan_Info_Poke2dLoadResourceObj( ZUKAN_INFO_WORK* work )
{
  ARCHANDLE*            handle;

  CLSYS_DRAW_TYPE draw_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DRAW_MAIN):(CLSYS_DRAW_SUB);

  // ハンドル
  handle = POKE2DGRA_OpenHandle( work->heap_id );
  // リソース読みこみ
  work->ncg_poke2d = POKE2DGRA_OBJ_CGR_Register(
                         handle,
                         work->monsno, work->formno, work->sex, work->rare,
                         POKEGRA_DIR_FRONT, FALSE,
                         work->personal_rnd,
                         draw_type, work->heap_id );
  work->ncl_poke2d = POKE2DGRA_OBJ_PLTT_Register(
                         handle,
                         work->monsno, work->formno, work->sex, work->rare,
                         POKEGRA_DIR_FRONT, FALSE,
                         draw_type,
                         ZUKAN_INFO_OBJ_PAL_POS_POKE2D * ZUKAN_INFO_PAL_LINE_SIZE, work->heap_id );
  work->nce_poke2d = POKE2DGRA_OBJ_CELLANM_Register(
                         work->monsno, work->formno, work->sex, work->rare,
                         POKEGRA_DIR_FRONT, FALSE,
                         APP_COMMON_MAPPING_128K,
                         draw_type, work->heap_id );
  GFL_ARC_CloseDataHandle( handle );
}

//-------------------------------------
/// ポケモン2Dのリソース破棄
//=====================================
static void Zukan_Info_Poke2dUnloadResourceObj( ZUKAN_INFO_WORK* work )
{
  //リソース破棄
  GFL_CLGRP_PLTT_Release( work->ncl_poke2d );
  GFL_CLGRP_CGR_Release( work->ncg_poke2d );
  GFL_CLGRP_CELLANIM_Release( work->nce_poke2d );
}

//-------------------------------------
/// ポケモン2DのOBJを生成する
//=====================================
static void Zukan_Info_Poke2dCreateCLWK( ZUKAN_INFO_WORK* work, u16 pos_x, u16 pos_y )
{
  GFL_CLWK_DATA cldata;

  CLSYS_DEFREND_TYPE defrend_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DEFREND_MAIN):(CLSYS_DEFREND_SUB);

  GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
  cldata.pos_x = pos_x;
  cldata.pos_y = pos_y;
  work->clwk_poke2d = GFL_CLACT_WK_Create( work->clunit, 
                                           work->ncg_poke2d,
                                           work->ncl_poke2d,
                                           work->nce_poke2d,
                                           &cldata, 
                                           defrend_type, work->heap_id );
  
  GFL_CLACT_WK_SetSoftPri( work->clwk_poke2d, 0 );  // 手前 > ポケモン2D > 足跡 > 属性アイコン > 奥
  
  if( work->launch == ZUKAN_INFO_LAUNCH_LIST )
  {
    GFL_CLACT_WK_SetObjMode( work->clwk_poke2d, GX_OAM_MODE_XLU );  // BGとともにこのOBJも暗くしたいので
  }
}

//-------------------------------------
/// ポケモン2DのOBJを破棄する
//=====================================
static void Zukan_Info_Poke2dDeleteCLWK( ZUKAN_INFO_WORK* work )
{
  GFL_CLACT_WK_Remove( work->clwk_poke2d );
}

//-------------------------------------
/// タイプアイコンOBJを生成する
//=====================================
static void Zukan_Info_CreateMultiLangTypeicon( ZUKAN_INFO_WORK* work, ZUKAN_INFO_LANG lang )
{
  // タイプアイコン

    //PokeType type1 = (PokeType)( PP_Get( work->pp, ID_PARA_type1, NULL ) );
    //PokeType type2 = (PokeType)( PP_Get( work->pp, ID_PARA_type2, NULL ) );
    
    POKEMON_PERSONAL_DATA* ppd = POKE_PERSONAL_OpenHandle( work->monsno, work->formno, work->heap_id );
    PokeType type1 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_type1 );
    PokeType type2 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_type2 );
    POKE_PERSONAL_CloseHandle( ppd );

    if( type1 == type2 )
    {
      type2 = POKETYPE_NULL;
    }

  if( lang == ZUKAN_INFO_LANG_NONE )  // 日本語
  {
    Zukan_Info_CreateTypeicon( work, type1, type2 );
  }
  else
  {
    Zukan_Info_CreateForeignTypeicon( work, type1, type2, lang );
  }
}
static void Zukan_Info_CreateTypeicon( ZUKAN_INFO_WORK* work, PokeType type1, PokeType type2 )
{
  s32 i;
  PokeType type[2];
  GFL_CLWK_DATA data[2] = { 0 };
  
  CLSYS_DRAW_TYPE draw_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DRAW_MAIN):(CLSYS_DRAW_SUB);
  CLSYS_DEFREND_TYPE defrend_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DEFREND_MAIN):(CLSYS_DEFREND_SUB);

  type[0] = type1;
  type[1] = type2;

  data[0].pos_x = 8*21;
  data[0].pos_y = 8*11 -4 + work->y_offset;
  data[1].pos_x = 8*26;
  data[1].pos_y = 8*11 -4 + work->y_offset;
  
  // リソース読み込み
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), work->heap_id );

    for( i=0; i<2; i++ )
    {
      work->typeicon_cg_idx[i] = GFL_CLGRP_REGISTER_FAILED;
      if( type[i] == POKETYPE_NULL )
      {
        continue;
      }
      work->typeicon_cg_idx[i] = GFL_CLGRP_CGR_Register( handle,
                                                         APP_COMMON_GetPokeTypeCharArcIdx(type[i]),
                                                         FALSE, draw_type, work->heap_id );
    }

    work->typeicon_cl_idx = GFL_CLGRP_PLTT_RegisterEx( handle,
                                                       APP_COMMON_GetPokeTypePltArcIdx(),
                                                       draw_type,
                                                       ZUKAN_INFO_OBJ_PAL_POS_TYPEICON * ZUKAN_INFO_PAL_LINE_SIZE,
                                                       0, ZUKAN_INFO_OBJ_PAL_NUM_TYPEICON, work->heap_id );

    work->typeicon_cean_idx = GFL_CLGRP_CELLANIM_Register( handle,
                                                           APP_COMMON_GetPokeTypeCellArcIdx( APP_COMMON_MAPPING_128K ),
                                                           APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K ),
                                                           work->heap_id );
  
    GFL_ARC_CloseDataHandle( handle );
  }

  // CLWK作成
  {
    for( i=0; i<2; i++ )
    {
      if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
      {
        continue;
      }
      work->typeicon_clwk[i] = GFL_CLACT_WK_Create( work->clunit,
                                                    work->typeicon_cg_idx[i],
                                                    work->typeicon_cl_idx,
                                                    work->typeicon_cean_idx,
                                                    &(data[i]),
                                                    defrend_type, work->heap_id );
      GFL_CLACT_WK_SetPlttOffs( work->typeicon_clwk[i], APP_COMMON_GetPokeTypePltOffset(type[i]),
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
      GFL_CLACT_WK_SetSoftPri( work->typeicon_clwk[i], 2 );  // 手前 > ポケモン2D > 足跡 > 属性アイコン > 奥
      GFL_CLACT_WK_SetObjMode( work->typeicon_clwk[i], GX_OAM_MODE_XLU );  // BGとともにこのOBJも暗くしたいので
    }
  }
}

static void Zukan_Info_CreateForeignTypeicon( ZUKAN_INFO_WORK* work, PokeType type1, PokeType type2, ZUKAN_INFO_LANG lang )
{
  s32 i;
  PokeType type[2];
  GFL_CLWK_DATA data[2] = { 0 };
  
  CLSYS_DRAW_TYPE draw_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DRAW_MAIN):(CLSYS_DRAW_SUB);
  CLSYS_DEFREND_TYPE defrend_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DEFREND_MAIN):(CLSYS_DEFREND_SUB);

  type[0] = type1;
  type[1] = type2;

  data[0].pos_x = 8*21;
  data[0].pos_y = 8*11 -4 + work->y_offset;
  data[1].pos_x = 8*26;
  data[1].pos_y = 8*11 -4 + work->y_offset;
  
  // リソース読み込み
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), work->heap_id );

    // BG用キャラとしてバイナリ変換してあるキャラデータを読み込む
    NNSG2dCharacterData* chara_data;
    void* buf = GFL_ARC_UTIL_LoadBGCharacter( ARCID_ZUKAN_GRA, typeicon_ncg_data_id_tbl[lang], FALSE, &chara_data, work->heap_id );
    // g2dcvtrマニュアル
    // 6.1 ncg（キャラクタデータ）ファイルの変換
    // ncg単独変換を行う場合は、ncgファイルがBG用キャラクタデータであることを-bg オプションで明示してください。入力されたncgファイルが常に２Dマッピングモードであるものとして、変換を実行します。
    u8* chara_raw_data = chara_data->pRawData;  // 32キャラx32キャラ

    for( i=0; i<2; i++ )
    {
      work->typeicon_cg_idx[i] = GFL_CLGRP_REGISTER_FAILED;
      if( type[i] == POKETYPE_NULL )
      {
        continue;
      }
      work->typeicon_cg_idx[i] = GFL_CLGRP_CGR_Register( handle,
                                                         APP_COMMON_GetPokeTypeCharArcIdx(type[i]),
                                                         FALSE, draw_type, work->heap_id );
      {
        u8 yoko = typeicon_pos_tbl[ type[i] ].yoko;
        u8 tate = typeicon_pos_tbl[ type[i] ].tate;
        GFL_CLGRP_CGR_ReplaceEx( work->typeicon_cg_idx[i], &chara_raw_data[32*32*2 *tate + 32*4 *yoko], 32*4, 0, draw_type );  // 上段
        GFL_CLGRP_CGR_ReplaceEx( work->typeicon_cg_idx[i], &chara_raw_data[32*32*2 *tate + 32*32 + 32*4 *yoko], 32*4, 32*4, draw_type );  // 下段
      }
    }

    work->typeicon_cl_idx = GFL_CLGRP_PLTT_RegisterEx( handle,
                                                       APP_COMMON_GetPokeTypePltArcIdx(),
                                                       draw_type,
                                                       ZUKAN_INFO_OBJ_PAL_POS_TYPEICON * ZUKAN_INFO_PAL_LINE_SIZE,
                                                       0, ZUKAN_INFO_OBJ_PAL_NUM_TYPEICON, work->heap_id );

    work->typeicon_cean_idx = GFL_CLGRP_CELLANIM_Register( handle,
                                                           APP_COMMON_GetPokeTypeCellArcIdx( APP_COMMON_MAPPING_128K ),
                                                           APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K ),
                                                           work->heap_id );
  
    GFL_ARC_CloseDataHandle( handle );

    GFL_HEAP_FreeMemory( buf );
  }

  // CLWK作成
  {
    for( i=0; i<2; i++ )
    {
      if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
      {
        continue;
      }
      work->typeicon_clwk[i] = GFL_CLACT_WK_Create( work->clunit,
                                                    work->typeicon_cg_idx[i],
                                                    work->typeicon_cl_idx,
                                                    work->typeicon_cean_idx,
                                                    &(data[i]),
                                                    defrend_type, work->heap_id );
      GFL_CLACT_WK_SetPlttOffs( work->typeicon_clwk[i], APP_COMMON_GetPokeTypePltOffset(type[i]),
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
      GFL_CLACT_WK_SetSoftPri( work->typeicon_clwk[i], 2 );  // 手前 > ポケモン2D > 足跡 > 属性アイコン > 奥
      GFL_CLACT_WK_SetObjMode( work->typeicon_clwk[i], GX_OAM_MODE_XLU );  // BGとともにこのOBJも暗くしたいので
    }
  }
}


//-------------------------------------
/// タイプアイコンOBJを破棄する
//=====================================
static void Zukan_Info_DeleteTypeicon( ZUKAN_INFO_WORK* work )
{
  s32 i=0;

  // CLWK破棄
  for( i=0; i<2; i++ )
  {
    if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
    {
      continue;
    }
    GFL_CLACT_WK_Remove( work->typeicon_clwk[i] );
  }

  // リソース破棄
  GFL_CLGRP_PLTT_Release( work->typeicon_cl_idx );
  for( i=0; i<2; i++ )
  {
    if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
    {
      continue;
    }
    GFL_CLGRP_CGR_Release( work->typeicon_cg_idx[i] );
  }
  GFL_CLGRP_CELLANIM_Release( work->typeicon_cean_idx );
}

//-------------------------------------
/// ポケモンの足跡OBJを生成する
//=====================================
static void Zukan_Info_CreatePokefoot( ZUKAN_INFO_WORK* work, u32 monsno )
{
  UI_EASY_CLWK_RES_PARAM prm;
  CLSYS_DRAW_TYPE draw_type = (work->disp==ZUKAN_INFO_DISP_M)?(CLSYS_DRAW_MAIN):(CLSYS_DRAW_SUB);

  if( monsno > MONSNO_ARUSEUSU ) monsno = 1;  // 開発中だけの処理

  prm.draw_type = draw_type;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NCGR;  // NCLR非圧縮、NCGR圧縮、NCER非圧縮、NANR非圧縮
  prm.arc_id    = PokeFootArcFileGet();
  prm.pltt_id   = PokeFootPlttDataIdxGet();
  prm.ncg_id    = PokeFootCharDataIdxGet((int)monsno);
  prm.cell_id   = PokeFootCellDataIdxGet();
  prm.anm_id    = PokeFootCellAnmDataIdxGet();
  prm.pltt_line = ZUKAN_INFO_OBJ_PAL_POS_POKEFOOT;
  prm.pltt_src_ofs = 0;
  prm.pltt_src_num = ZUKAN_INFO_OBJ_PAL_NUM_POKEFOOT;
  
  UI_EASY_CLWK_LoadResource( &work->clres_pokefoot, &prm, work->clunit, work->heap_id );

  work->clwk_pokefoot = UI_EASY_CLWK_CreateCLWK( &work->clres_pokefoot, work->clunit, 8*15, (u8)( 8*11 + work->y_offset ), 0, work->heap_id );
  GFL_CLACT_WK_SetSoftPri( work->clwk_pokefoot, 1 );  // 手前 > ポケモン2D > 足跡 > 属性アイコン > 奥
  GFL_CLACT_WK_SetObjMode( work->clwk_pokefoot, GX_OAM_MODE_XLU );  // BGとともにこのOBJも暗くしたいので
}

//-------------------------------------
/// ポケモンの足跡OBJを破棄する
//=====================================
static void Zukan_Info_DeletePokefoot( ZUKAN_INFO_WORK* work )
{
  //CLWK破棄
  GFL_CLACT_WK_Remove( work->clwk_pokefoot );
  //リソース破棄
  UI_EASY_CLWK_UnLoadResource( &work->clres_pokefoot );
}

//-------------------------------------
/// ポケモン2D以外のものを生成する
//=====================================
static void Zukan_Info_CreateOthers( ZUKAN_INFO_WORK* work )
{
  // Message
  Zukan_Info_CreateMessage( work );

  if( work->get_flag )
  {
    // タイプアイコン
    Zukan_Info_CreateMultiLangTypeicon( work, ZUKAN_INFO_LANG_NONE );

    // ポケモンの足跡
    {
      Zukan_Info_CreatePokefoot( work, work->monsno );
    }
  }
}

//-------------------------------------
/// パレットアニメーションの更新
//=====================================
static void Zukan_Info_UpdatePaletteAnime( ZUKAN_INFO_WORK* work )
{
  // アニメーションカウントを進める
  if( work->anm_cnt + ZUKAN_INFO_PALETTE_ANIME_VALUE >= ZUKAN_INFO_PALETTE_ANIME_VALUE_MAX )
  {
    work->anm_cnt = work->anm_cnt + ZUKAN_INFO_PALETTE_ANIME_VALUE - ZUKAN_INFO_PALETTE_ANIME_VALUE_MAX;
  }
  else
  {
    work->anm_cnt += ZUKAN_INFO_PALETTE_ANIME_VALUE;
  }

  // パレットの色を更新する
  {
#define ZUKAN_INFO_GET_R(color) ( ( (color) & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT )
#define ZUKAN_INFO_GET_G(color) ( ( (color) & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT )
#define ZUKAN_INFO_GET_B(color) ( ( (color) & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT )

    // 0〜1の値に変換
    const fx16 cos = ( FX_CosIdx(work->anm_cnt) + FX16_ONE ) / 2;
    u8 i;
    for( i=0; i<ZUKAN_INFO_PALETTE_ANIME_NUM; i++ )
    {
      const u8 rs = ZUKAN_INFO_GET_R( work->palette_anime_s[i] );
      const u8 gs = ZUKAN_INFO_GET_G( work->palette_anime_s[i] );
      const u8 bs = ZUKAN_INFO_GET_B( work->palette_anime_s[i] );
      const u8 re = ZUKAN_INFO_GET_R( work->palette_anime_e[i] );
      const u8 ge = ZUKAN_INFO_GET_G( work->palette_anime_e[i] );
      const u8 be = ZUKAN_INFO_GET_B( work->palette_anime_e[i] );
      const u8 r = rs + ( ((re-rs)*cos) >> FX16_SHIFT );
      const u8 g = gs + ( ((ge-gs)*cos) >> FX16_SHIFT );
      const u8 b = bs + ( ((be-bs)*cos) >> FX16_SHIFT );
      work->trans_buf[i] = GX_RGB(r, g, b);
    }

    {
      NNS_GFD_DST_TYPE dst_type =(work->disp==ZUKAN_INFO_DISP_M)?(NNS_GFD_DST_2D_BG_PLTT_MAIN):(NNS_GFD_DST_2D_BG_PLTT_SUB); 
      NNS_GfdRegisterNewVramTransferTask( dst_type,
          ZUKAN_INFO_PALETTE_ANIME_LINE * ZUKAN_INFO_PAL_LINE_SIZE + ZUKAN_INFO_PALETTE_ANIME_NO * ZUKAN_INFO_COLOR_SIZE,
          work->trans_buf, ZUKAN_INFO_COLOR_SIZE * ZUKAN_INFO_PALETTE_ANIME_NUM );
    }

#undef ZUKAN_INFO_GET_R
#undef ZUKAN_INFO_GET_G
#undef ZUKAN_INFO_GET_B
  }
}

//-------------------------------------
/// 全OBJの表示/非表示を設定する
//=====================================
void ZUKAN_INFO_SetDrawEnableAllObj( ZUKAN_INFO_WORK* work, BOOL on_off )
{
  u8 i;

  // ポケモン2D
  GFL_CLACT_WK_SetDrawEnable( work->clwk_poke2d, on_off );
 
  if( work->get_flag )
  {
    // タイプアイコン
    GFL_CLACT_WK_SetDrawEnable( work->clwk_pokefoot, on_off );
 
    // ポケモンの足跡
    for( i=0; i<2; i++ )
    {
      if( work->typeicon_cg_idx[i] == GFL_CLGRP_REGISTER_FAILED )
      {
        continue;
      }
      GFL_CLACT_WK_SetDrawEnable( work->typeicon_clwk[i], on_off );
    }
  }
}
