//============================================================================
/**
 *  @file   manual_explain.c
 *  @brief  ゲーム内マニュアル
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  モジュール名：MANUAL_EXPLAIN
 */
//============================================================================
// インクルード
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "manual_graphic.h"
#include "manual_def.h"
#include "manual_common.h"
#include "manual_touchbar.h"
#include "manual_explain.h"

// アーカイブ
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_manual.h"
#include "msg/msg_manual_text.h"
#include "manual.naix"
#include "manual_image.naix"


// サウンド

// オーバーレイ


//=============================================================================
/**
*  定数定義
*/
//=============================================================================
// 入力状態
typedef enum
{
  INPUT_STATE_NONE,  // 何の入力アニメーションも行っていないので、入力可能。
  INPUT_STATE_END,   // もう何の入力も受け付けない。
  INPUT_STATE_TB,    // タッチバーの入力アニメーションを行っている最中なので、入力不可能。
}
INPUT_STATE;

// テキスト
enum
{
  TEXT_DUMMY,      // BG_FRAME_S_MAIN
  TEXT_TITLE,      // BG_FRAME_S_MAIN
  TEXT_EXPLAIN,    // BG_FRAME_S_MAIN
  TEXT_MAX,
};

static const u8 bmpwin_setup[TEXT_MAX][9] =
{
  // frmnum              posx  posy  sizx  sizy  palnum                dir                    x  y
  {  BG_FRAME_S_MAIN,       0,    0,    1,    1, BG_PAL_POS_S_TEXT,    GFL_BMP_CHRAREA_GET_F, 0, 0 },  // TEXT_DUMMY
  {  BG_FRAME_S_MAIN,       1,    0,   30,    2, BG_PAL_POS_S_TEXT,    GFL_BMP_CHRAREA_GET_F, 0, 1 },  // TEXT_TITLE
  {  BG_FRAME_S_MAIN,       0,    3,   32,   18, BG_PAL_POS_S_TEXT,    GFL_BMP_CHRAREA_GET_F, 4, 0 },  // TEXT_EXPLAIN
};

#define TEXT_TITLE_COLOR_L    (15)  // 文字主色
#define TEXT_TITLE_COLOR_S    ( 2)  // 文字影色
#define TEXT_TITLE_COLOR_B    ( 0)  // 文字背景色(透明)

#define TEXT_EXPLAIN_COLOR_L  ( 1)  // 文字主色
#define TEXT_EXPLAIN_COLOR_S  ( 2)  // 文字影色
#define TEXT_EXPLAIN_COLOR_B  ( 0)  // 文字背景色(透明)


//=============================================================================
/**
*  構造体宣言
*/
//=============================================================================
//-------------------------------------
/// ワーク
//=====================================
struct _MANUAL_EXPLAIN_WORK
{
  // パラメータ
  MANUAL_EXPLAIN_PARAM*       param;
  // 共通
  MANUAL_COMMON_WORK*         cmn_wk;

  // ここで作成
  u8                          page_no;  // 0<= <work->param->page_num
  BOOL                        page_change_req;  // ページを変更する必要があるときTRUE

  // テキスト
  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  PRINT_QUE*                  print_que_text[TEXT_MAX];  // 他のところで作成したもののポインタ
  BOOL                        text_trans[TEXT_MAX];      // 転送する必要がある場合TRUE

  // VBlank中TCB
  GFL_TCB*                    vblank_tcb;

  // 入力の状態
  INPUT_STATE                 input_state;
};


//=============================================================================
/**
*  ローカル関数のプロトタイプ宣言
*/
//=============================================================================
// VBlank関数
static void Manual_Explain_VBlankFunc( GFL_TCB* tcb, void* wk );

// 準備
static void Manual_Explain_Prepare( MANUAL_EXPLAIN_WORK* work );
// 転送(VBlank転送ではない)
static void Manual_Explain_Trans( MANUAL_EXPLAIN_WORK* work );

// テキスト
static void Manual_Explain_TextInit( MANUAL_EXPLAIN_WORK* work );
static void Manual_Explain_TextExit( MANUAL_EXPLAIN_WORK* work );
static void Manual_Explain_TextMain( MANUAL_EXPLAIN_WORK* work );

// ページ生成(文章とテキストの生成)
static void Manual_Explain_PageChange( MANUAL_EXPLAIN_WORK* work );


//=============================================================================
/**
*  外部公開関数定義
*/
//=============================================================================
// 初期化処理
MANUAL_EXPLAIN_WORK*  MANUAL_EXPLAIN_Init(
    MANUAL_EXPLAIN_PARAM*   param,
    MANUAL_COMMON_WORK*     cmn_wk
)
{
  // ワーク
  MANUAL_EXPLAIN_WORK*  work  = GFL_HEAP_AllocClearMemory( cmn_wk->heap_id, sizeof(MANUAL_EXPLAIN_WORK) );

  // パラメータ
  work->param  = param;
  {
    // out
    work->param->result   = MANUAL_EXPLAIN_RESULT_RETURN;
  }
  // 共通
  work->cmn_wk = cmn_wk;

  // ここで作成
  {
    work->page_no = 0;
    work->page_change_req = FALSE;
  }

  Manual_Explain_Prepare( work );
  Manual_Explain_Trans( work );

  Manual_Explain_TextInit( work );

  // VBlank中TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_Explain_VBlankFunc, work, 1 );

  // 入力状態
  work->input_state = INPUT_STATE_NONE;

  // マニュアルタッチバー
  MANUAL_TOUCHBAR_SetType( work->cmn_wk->mtb_wk, MANUAL_TOUCHBAR_TYPE_EXPLAIN );
  MANUAL_TOUCHBAR_SetPage( work->cmn_wk->mtb_wk, work->param->page_num, work->page_no );

  // 最初のページ
  {
    Manual_Explain_PageChange( work );
  }

  return work;
}

// 終了処理
void  MANUAL_EXPLAIN_Exit(
    MANUAL_EXPLAIN_WORK*     work
)
{
  // 画像を消去
  {
    u32  image_size = 256*192*2;
    u32  size;
    u16* buf;

    buf = GFL_HEAP_AllocClearMemory( work->cmn_wk->heap_id, image_size );
    size = image_size;

    DC_FlushRange( buf, size );
    GX_LoadBG2Bmp( buf, 0, image_size );
    GFL_HEAP_FreeMemory( buf );
  }

  // VBlank中TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // ここで作成
  Manual_Explain_TextExit( work );

  // ワーク
  GFL_HEAP_FreeMemory( work );
}

// 主処理
BOOL  MANUAL_EXPLAIN_Main(
    MANUAL_EXPLAIN_WORK*     work
)
{
  BOOL b_end = FALSE;

  // テキスト
  Manual_Explain_TextMain( work );

  // マニュアルタッチバーの入力を調べる
  if(    work->input_state == INPUT_STATE_NONE
      || work->input_state == INPUT_STATE_TB )
  {
    // マニュアルタッチバー
    MANUAL_TOUCHBAR_Main( work->cmn_wk->mtb_wk );

    if( work->input_state == INPUT_STATE_NONE )
    {
      TOUCHBAR_ICON icon = MANUAL_TOUCHBAR_GetTouch( work->cmn_wk->mtb_wk );
      if( icon != TOUCHBAR_SELECT_NONE )
      {
        work->input_state = INPUT_STATE_TB;

        if( icon == TOUCHBAR_ICON_CUR_L )
        {
          if( work->page_no > 0 )
          {
            work->page_no--;
            work->page_change_req = TRUE;
          }
        }
        else if( icon == TOUCHBAR_ICON_CUR_R )
        {
          if( work->page_no < work->param->page_num -1 )
          {
            work->page_no++;
            work->page_change_req = TRUE;
          }
        }
        if( work->page_change_req ) Manual_Explain_PageChange( work );
      }
    }
    else
    {
      TOUCHBAR_ICON icon = MANUAL_TOUCHBAR_GetTrg( work->cmn_wk->mtb_wk );
      if( icon != TOUCHBAR_SELECT_NONE )
      {
        switch( icon )
        {
        case TOUCHBAR_ICON_RETURN:
          {
            work->param->result = MANUAL_EXPLAIN_RESULT_RETURN;
            work->input_state = INPUT_STATE_END;
            b_end = TRUE;
          }
          break;
        case TOUCHBAR_ICON_CUR_L:
        case TOUCHBAR_ICON_CUR_R:
          {
            work->input_state = INPUT_STATE_NONE;
            if( work->page_change_req )
            {
              MANUAL_TOUCHBAR_SetPage( work->cmn_wk->mtb_wk, work->param->page_num, work->page_no );
              work->page_change_req = FALSE;
            }
          }
          break;
        }
      }
    }
  }

  return b_end;
}


//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in]       
 *  @param[in,out]       
 *
 *  @retval          
 */
//------------------------------------------------------------------

//-------------------------------------
/// 
//=====================================


//=============================================================================
/**
*  ローカル関数定義
*/
//=============================================================================
//-------------------------------------
/// VBlank関数
//=====================================
static void Manual_Explain_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_EXPLAIN_WORK* work = (MANUAL_EXPLAIN_WORK*)wk;
}

// 準備
static void Manual_Explain_Prepare( MANUAL_EXPLAIN_WORK* work )
{
  // サブBG
  GFL_ARCHDL_UTIL_TransVramPalette(
      work->cmn_wk->handle_system,
      NARC_manual_bg_NCLR,
      PALTYPE_SUB_BG,
      BG_PAL_POS_S_REAR * 0x20,
      BG_PAL_NUM_S_REAR * 0x20,
      work->cmn_wk->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      work->cmn_wk->handle_system,
      NARC_manual_bg_NCGR,
      BG_FRAME_S_REAR,
			0,
      0,  // 全転送
      FALSE,
      work->cmn_wk->heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      work->cmn_wk->handle_system,
      NARC_manual_bg3_NSCR,
      BG_FRAME_S_REAR,
      0,
      0,  // 全転送
      FALSE,
      work->cmn_wk->heap_id );
}
// 転送(VBlank転送ではない)
static void Manual_Explain_Trans( MANUAL_EXPLAIN_WORK* work )
{
  GFL_BG_LoadScreenReq( BG_FRAME_S_REAR );
}

// テキスト
static void Manual_Explain_TextInit( MANUAL_EXPLAIN_WORK* work )
{
  // bmpwin
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_bmpwin[i] = GFL_BMPWIN_Create(
                                     bmpwin_setup[i][0],
                                     bmpwin_setup[i][1], bmpwin_setup[i][2], bmpwin_setup[i][3], bmpwin_setup[i][4],
                                     bmpwin_setup[i][5], bmpwin_setup[i][6] );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[i]), 0 );
    GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );
    work->text_trans[i] = FALSE;
  }

  // print_que
  work->print_que_text[TEXT_DUMMY]   = work->cmn_wk->print_que_title;
  work->print_que_text[TEXT_TITLE]   = work->cmn_wk->print_que_title;
  work->print_que_text[TEXT_EXPLAIN] = work->cmn_wk->print_que_main;

  // title
  {
    STRBUF* strbuf = GFL_MSG_CreateString( work->cmn_wk->msgdata_main, work->param->title_str_id );
    PRINTSYS_PrintQueColor(
        work->print_que_text[TEXT_TITLE],
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_TITLE]),
        bmpwin_setup[TEXT_TITLE][7], bmpwin_setup[TEXT_TITLE][8],
        strbuf,
        work->cmn_wk->font,
        PRINTSYS_LSB_Make(TEXT_TITLE_COLOR_L,TEXT_TITLE_COLOR_S,TEXT_TITLE_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
    work->text_trans[TEXT_TITLE] = TRUE;
    // 既に済んでいるかもしれないので、Mainを1度呼んでおく
    Manual_Explain_TextMain( work );
  }
}
static void Manual_Explain_TextExit( MANUAL_EXPLAIN_WORK* work )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_trans[i] = FALSE;
    PRINTSYS_QUE_Clear( work->print_que_text[i] );
    GFL_BMPWIN_ClearScreen( work->text_bmpwin[i] );  // このBGフレームのスクリーンは次の画面で全画面が書かれるわけではないので、
                                                     // ゴミを残さないようにしておく。APP_TASKMENU_WIN_Deleteを参考にした。
    GFL_BMPWIN_Delete( work->text_bmpwin[i] );
  }
}
static void Manual_Explain_TextMain( MANUAL_EXPLAIN_WORK* work )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    if( work->text_trans[i] )
    {
      if( !PRINTSYS_QUE_IsExistTarget( work->print_que_text[i], GFL_BMPWIN_GetBmp(work->text_bmpwin[i]) ) )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
        work->text_trans[i] = FALSE;
      }
    }
  }
}

// ページ生成(文章とテキストの生成)
static void Manual_Explain_PageChange( MANUAL_EXPLAIN_WORK* work )
{
  // work->page_noは既にこれから生成するページ番号になっている 

  u32  image_size = 256*192*2;
  u32  size;
  u16* buf;


  // テキスト

  // 今まで表示していたものをクリアする
  PRINTSYS_QUE_Clear( work->print_que_text[TEXT_EXPLAIN] );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_EXPLAIN]), 0 );

  {
    STRBUF* strbuf = GFL_MSG_CreateString( work->cmn_wk->msgdata_main, work->param->page[work->page_no].str_id );
    PRINTSYS_PrintQueColor(
        work->print_que_text[TEXT_EXPLAIN],
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_EXPLAIN]),
        bmpwin_setup[TEXT_EXPLAIN][7], bmpwin_setup[TEXT_EXPLAIN][8],
        strbuf,
        work->cmn_wk->font,
        PRINTSYS_LSB_Make(TEXT_EXPLAIN_COLOR_L,TEXT_EXPLAIN_COLOR_S,TEXT_EXPLAIN_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
    work->text_trans[TEXT_EXPLAIN] = TRUE;
    // 既に済んでいるかもしれないので、Mainを1度呼んでおく
    Manual_Explain_TextMain( work );
  }


  // 画像

  if( work->param->page[work->page_no].image == MANUAL_EXPLAIN_NO_IMAGE )
  {
    buf = GFL_HEAP_AllocClearMemory( work->cmn_wk->heap_id, image_size );
    size = image_size;
  }
  else
  {
    buf = GFL_ARCHDL_UTIL_LoadEx( work->cmn_wk->handle_explain, work->param->page[work->page_no].image, TRUE, work->cmn_wk->heap_id, &size );
  }
  DC_FlushRange( buf, size );
  GX_LoadBG2Bmp( buf, 0, image_size );
  GFL_HEAP_FreeMemory( buf );
}

