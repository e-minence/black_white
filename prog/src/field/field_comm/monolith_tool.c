//==============================================================================
/**
 * @file    monolith_tool.c
 * @brief   モノリス画面ツール
 * @author  matsuda
 * @date    2009.11.21(土)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "arc_def.h"
#include "monolith_common.h"
#include "monolith.naix"
#include "monolith_tool.h"
#include "field/intrude_common.h"
#include "intrude_work.h"
#include "msg/msg_monolith.h"


//==============================================================================
//  定数定義
//==============================================================================
///ソフトプライオリティ
enum{
  SOFTPRI_PANEL_BMPFONT = 10,
  SOFTPRI_PANEL,
};

///アクターBGプライオリティ
enum{
  BGPRI_PANEL = 1,
};

///パネル(大)BMPFONT座標X
#define PANEL_LARGE_BMPFONT_POS_X(panel_x)   (panel_x - PANEL_LARGE_CHARSIZE_X/2*8)
///パネル(小)BMPFONT座標X
#define PANEL_SMALL_BMPFONT_POS_X(panel_x)   (panel_x - PANEL_SMALL_CHARSIZE_X/2*8)
///パネル(決定)BMPFONT座標X
#define PANEL_DECIDE_BMPFONT_POS_X(panel_x)   (panel_x - PANEL_DECIDE_CHARSIZE_X/2*8)

///「はい/いいえ」ウィンドウのX位置
#define _INFO_YESNO_X   ( 32-APP_TASKMENU_PLATE_WIDTH_YN_WIN )
#define _INFO_YESNO_Y   ( 12 )


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void _PanelColor_SetMode(MONOLITH_APP_PARENT *appwk, PANEL_COLORMODE mode, FADEREQ req);
static int _PanelColor_GetStartColorPos(FADEREQ req);
static int _PanelColor_GetChangeDataColorPos(FADEREQ req);
static int _PanelColor_GetChangeColorNum(FADEREQ req);


//==============================================================================
//  データ
//==============================================================================
//--------------------------------------------------------------
//  パネル
//--------------------------------------------------------------
///パネルアクターヘッダ
static const GFL_CLWK_DATA ActHead_Panel = {
  PANEL_POS_X,                   //pos_x
  0,                             //pos_y
  COMMON_ANMSEQ_PANEL_LARGE,     //anmseq
  SOFTPRI_PANEL,                 //softpri
  BGPRI_PANEL,                   //bgpri
};

///パネルアクターのBMPOAMヘッダ
static const BMPOAM_ACT_DATA BmpOamHead_Panel = {
  NULL,                          //bmp
  0,                             //x
  0,                             //y
  0,                             //pltt_index
  COMMON_PAL_PANEL,              //pal_offset
  SOFTPRI_PANEL_BMPFONT,         //soft_pri
  BGPRI_PANEL,                   //bg_pri
  0,                             //setSerface
  0,                             //draw_type
};

//--------------------------------------------------------------
//  パネルカラーアニメ
//--------------------------------------------------------------
//パネルカラーアニメ設定
enum{
  PANEL_COLOR_START = 2,      ///<パネルカラーアニメ開始位置
  PANEL_COLOR_END = 4,        ///<パネルカラーアニメ終端位置
  PANEL_COLOR_NUM = PANEL_COLOR_END - PANEL_COLOR_START + 1,  ///<パネルカラーアニメのカラー数
  
  PANEL_FADEDATA_PALNO_OBJ = 2,    ///<パネルフェードデータ(変更後の色)が入っているパレット位置

  PANEL_START_PALNO_BG_D = COMMON_PALBG_PANEL_FOCUS,      ///<パネルフェード開始パレットNo
  PANEL_FADEDATA_PALNO_BG_D = 4,   ///<パネルフェードデータ(変更後の色)が入っているパレット位置
  
  PANEL_COLOR_FADE_ADD_EVY = 0x0080,    ///<EVY加算値(下位8ビット小数)
  
  PANEL_COLOR_FLASH_WAIT = 3,     ///<フラッシュのウェイト
  PANEL_COLOR_FLASH_COUNT = 1,    ///<フラッシュ回数
};

#if 0 //PANEL_FADEDATA_PALNOの位置に色を入れておくように変更
///パネルカラーアニメでフォーカスされた時に向かうEVY最大到達カラー
ALIGN4 static const u16 PanelFocusColor[] = {
  0x7ff9, 0x7fd3, 0x7f10,
};
SDK_COMPILER_ASSERT(NELEMS(PanelFocusColor) == PANEL_COLOR_NUM);
#endif

//--------------------------------------------------------------
//  BMPFONT
//--------------------------------------------------------------
///モノリスBMPOAMヘッダ
static const BMPOAM_ACT_DATA BmpOamHead_Str = {
  NULL,                          //bmp
  0,                             //x
  0,                             //y
  0,                             //pltt_index
  COMMON_PAL_TOWN,               //pal_offset
  200,                           //soft_pri
  3,                             //bg_pri
  0,                             //setSerface
  0,                             //draw_type
};

///キャンセルアイコンのフラッシュウェイト
#define CANCEL_ICON_FLASH_WAIT    (5)


//==================================================================
/**
 * パネルアクター生成
 *
 * @param   setup       モノリス全画面共通設定データへのポインタ
 * @param   dest		    生成したパネルアクター代入先
 * @param   res_index		共通素材Index
 * @param   size		    パネルサイズ
 * @param   x           X座標(中心)
 * @param   y		        Y座標(中心)
 * @param   msg_id      メッセージID
 * @param   wordset       WORDSETが不必要な場合はNULL指定
 * 
 * WORDSETが必要な場合は外側であらかじめWORDSETをしてください
 */
//==================================================================
void MonolithTool_PanelOBJ_Create(MONOLITH_SETUP *setup, PANEL_ACTOR *dest, COMMON_RESOURCE_INDEX res_index, PANEL_SIZE size, int x, int y, u32 msg_id, WORDSET *wordset)
{
  CLSYS_DEFREND_TYPE defrend_type;
  GFL_CLWK_DATA head = ActHead_Panel;
  BMPOAM_ACT_DATA bmpoam_head = BmpOamHead_Panel;
  int bmp_size_x;
  STRBUF *strbuf;
  
  switch(size){
  case PANEL_SIZE_LARGE:
    bmp_size_x = PANEL_LARGE_CHARSIZE_X;
    head.anmseq = COMMON_ANMSEQ_PANEL_LARGE;
    bmpoam_head.x = PANEL_LARGE_BMPFONT_POS_X(x);
    break;
  case PANEL_SIZE_SMALL:
    bmp_size_x = PANEL_SMALL_CHARSIZE_X;
    head.anmseq = COMMON_ANMSEQ_PANEL_SMALL;
    bmpoam_head.x = PANEL_SMALL_BMPFONT_POS_X(x);
    break;
  case PANEL_SIZE_DECIDE:
  default:
    bmp_size_x = PANEL_DECIDE_CHARSIZE_X;
    head.anmseq = COMMON_ANMSEQ_PANEL_DECIDE;
    bmpoam_head.x = PANEL_DECIDE_BMPFONT_POS_X(x);
    break;
  }
  
  //BMP作成
  dest->bmp = GFL_BMP_Create(bmp_size_x, PANEL_CHARSIZE_Y, GFL_BMP_16_COLOR, HEAPID_MONOLITH);
  
  //ヘッダ作成
  head.pos_x = x;
  head.pos_y = y;
  bmpoam_head.y = y - 8;
  bmpoam_head.bmp = dest->bmp;
  bmpoam_head.pltt_index = setup->common_res[res_index].pltt_bmpfont_index;
  if(res_index == COMMON_RESOURCE_INDEX_UP){
    defrend_type = CLSYS_DEFREND_MAIN;
    bmpoam_head.draw_type = CLSYS_DRAW_MAIN;
  }
  else{
    defrend_type = CLSYS_DEFREND_SUB;
    bmpoam_head.draw_type = CLSYS_DRAW_SUB;
  }
  bmpoam_head.setSerface = defrend_type;
  
  //生成
  dest->cap = GFL_CLACT_WK_Create(setup->clunit, setup->common_res[res_index].char_index, 
    setup->common_res[res_index].pltt_index, setup->common_res[res_index].cell_index, 
	  &head, defrend_type, HEAPID_MONOLITH);
  dest->bmpoam = BmpOam_ActorAdd(setup->bmpoam_sys, &bmpoam_head);
  
  //文字描画
  strbuf = GFL_MSG_CreateString( setup->mm_monolith, msg_id );
  if(wordset != NULL){
    STRBUF *expand_str = GFL_STR_CreateBuffer(128, HEAPID_MONOLITH);
    WORDSET_ExpandStr( wordset, expand_str, strbuf );
    PRINTSYS_PrintQue( setup->printQue, dest->bmp, 0, 0, expand_str, setup->font_handle );
    GFL_STR_DeleteBuffer(expand_str);
  }
  else{
    PRINTSYS_PrintQue( setup->printQue, dest->bmp, 0, 0, strbuf, setup->font_handle );
  }
  GFL_STR_DeleteBuffer(strbuf);
  
  //内部フラグセット
  dest->trans_flag = TRUE;
}

//==================================================================
/**
 * パネルアクター削除
 *
 * @param   panel		パネルアクターへのポインタ
 */
//==================================================================
void MonolithTool_PanelOBJ_Delete(PANEL_ACTOR *panel)
{
  GFL_CLACT_WK_Remove(panel->cap);
  BmpOam_ActorDel(panel->bmpoam);
  GFL_BMP_Delete(panel->bmp);
  panel->cap = NULL;
  panel->bmpoam = NULL;
  panel->bmp = NULL;
}

//==================================================================
/**
 * パネルアクターの表示設定
 *
 * @param   panel		
 * @param   on_off		TRUE:表示ON、　FALSE:表示OFF
 */
//==================================================================
void MonolithTool_PanelOBJ_SetEnable(PANEL_ACTOR *panel, BOOL on_off)
{
  GFL_CLACT_WK_SetDrawEnable( panel->cap, on_off );
  BmpOam_ActorSetDrawEnable( panel->bmpoam, on_off );
}

//==================================================================
/**
 * パネルアクター更新処理
 *
 * @param   setup   モノリス全画面共通設定データへのポインタ
 * @param   panel		パネルアクターへのポインタ
 *
 * @retval  BOOL		TRUE:BMP転送を行った
 */
//==================================================================
BOOL MonolithTool_PanelOBJ_TransUpdate(MONOLITH_SETUP *setup, PANEL_ACTOR *panel)
{
  if( panel->trans_flag == TRUE 
      && PRINTSYS_QUE_IsExistTarget(setup->printQue, panel->bmp) == FALSE ){
    BmpOam_ActorBmpTrans(panel->bmpoam);
    panel->trans_flag = FALSE;
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * パネルに対してフォーカスを設定する
 *
 * @param   appwk
 * @param   panel[]		  パネルアクターの配列へのポインタ
 * @param   panel_max		パネルアクター配列の要素数
 * @param   focus_no		フォーカスされるパネル番号(何もフォーカスしない場合はPANEL_NO_FOCUS)
 * @param   req         FADE_SUB_OBJ等
 */
//==================================================================
void MonolithTool_PanelOBJ_Focus(MONOLITH_APP_PARENT *appwk, PANEL_ACTOR panel[], int panel_max, int focus_no, FADEREQ req)
{
  int i;
  
  for(i = 0; i < panel_max; i++){
    if(i == focus_no){
      GFL_CLACT_WK_SetPlttOffs(panel[i].cap, COMMON_PAL_PANEL_FOCUS, CLWK_PLTTOFFS_MODE_PLTT_TOP);
      _PanelColor_SetMode(appwk, PANEL_COLORMODE_FOCUS, req);
    }
    else{
      GFL_CLACT_WK_SetPlttOffs(panel[i].cap, COMMON_PAL_PANEL, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    }
  }
  
  if(focus_no == PANEL_NO_FOCUS){
    _PanelColor_SetMode(appwk, PANEL_COLORMODE_NONE, req);
  }
}

//==================================================================
/**
 * パネルに対してフラッシュを行う
 *
 * @param   appwk
 * @param   panel[]		  パネルアクターの配列へのポインタ
 * @param   panel_max		パネルアクター配列の要素数
 * @param   focus_no		フラッシュするパネル番号(何もフォーカスしない場合はPANEL_NO_FOCUS)
 * @param   req         FADE_SUB_OBJ等
 */
//==================================================================
void MonolithTool_PanelOBJ_Flash(MONOLITH_APP_PARENT *appwk, PANEL_ACTOR panel[], int panel_max, int focus_no, FADEREQ req)
{
  int i;
  
  for(i = 0; i < panel_max; i++){
    if(i == focus_no){
      GFL_CLACT_WK_SetPlttOffs(panel[i].cap, COMMON_PAL_PANEL_FOCUS, CLWK_PLTTOFFS_MODE_PLTT_TOP);
      _PanelColor_SetMode(appwk, PANEL_COLORMODE_FLASH, req);
    }
    else{
      GFL_CLACT_WK_SetPlttOffs(panel[i].cap, COMMON_PAL_PANEL, CLWK_PLTTOFFS_MODE_PLTT_TOP);
    }
  }
}

//==================================================================
/**
 * パネルBGに対してフォーカスを設定する
 *
 * @param   appwk
 * @param   focus_use		TRUE:フォーカスアニメ開始。　FALSE:フォーカスアニメ停止
 * @param   req         FADE_SUB_OBJ等
 *
 * PanelOBJと違いフォーカス対象のスクリーンは外側で独自に編集する必要がある
 * この関数で行うのはフォーカスアニメの開始と停止の制御のみ
 */
//==================================================================
void MonolithTool_PanelBG_Focus(MONOLITH_APP_PARENT *appwk, BOOL focus_use, FADEREQ req)
{
  if(focus_use == TRUE){
    _PanelColor_SetMode(appwk, PANEL_COLORMODE_FOCUS, req);
  }
  else{
    _PanelColor_SetMode(appwk, PANEL_COLORMODE_NONE, req);
  }
}

//==================================================================
/**
 * パネルBGに対してフラッシュを行う
 *
 * @param   appwk
 * @param   req         FADE_SUB_OBJ等
 *
 * PanelOBJと違いフォーカス対象のスクリーンは外側で独自に編集する必要がある
 * この関数で行うのはフラッシュアニメの開始スイッチを入れるだけ
 */
//==================================================================
void MonolithTool_PanelBG_Flash(MONOLITH_APP_PARENT *appwk, FADEREQ req)
{
  _PanelColor_SetMode(appwk, PANEL_COLORMODE_FLASH, req);
}

//==================================================================
/**
 * パネルシステム初期化
 *
 * @param   appwk		
 */
//==================================================================
void MonolithTool_Panel_Init(MONOLITH_APP_PARENT *appwk)
{
  int i;
  for(i = 0; i < PANEL_CONTROL_MAX; i++){
    GFL_STD_MemClear(&appwk->tool.panel_color[i], sizeof(PANEL_COLOR_CONTROL));
  }
}

//==================================================================
/**
 * パネルカラーアニメ：モード取得
 *
 * @param   appwk		
 * @retval  PANEL_COLORMODE		
 */
//==================================================================
PANEL_COLORMODE MonolithTool_PanelColor_GetMode(MONOLITH_APP_PARENT *appwk, FADEREQ req)
{
  return appwk->tool.panel_color[req].mode;
}

//==================================================================
/**
 * パネルカラーアニメ：更新処理
 *
 * @param   appwk		
 */
//==================================================================
void MonolithTool_Panel_ColorUpdate(MONOLITH_APP_PARENT *appwk, FADEREQ req)
{
  PANEL_COLOR_CONTROL *pcc = &appwk->tool.panel_color[req];
  int start_colpos, change_colpos, color_num;
  
  GF_ASSERT_HEAVY(req < PANEL_CONTROL_MAX);
  
  start_colpos = _PanelColor_GetStartColorPos(req);
  change_colpos = _PanelColor_GetChangeDataColorPos(req);
  color_num = _PanelColor_GetChangeColorNum(req);
  
  switch(pcc->mode){
  case PANEL_COLORMODE_FOCUS:
    {
      int i;
      s32 evy;
      
      evy = pcc->evy;
      if(pcc->add_dir == 0){
        evy += PANEL_COLOR_FADE_ADD_EVY;
        if(evy >= (17 << 8)){
          evy = 16 << 8;
          pcc->add_dir ^= 1;
        }
      }
      else{
        evy -= PANEL_COLOR_FADE_ADD_EVY;
        if(evy <= 0){
          evy = 0x100;
          pcc->add_dir ^= 1;
        }
      }
      pcc->evy = evy;
      evy >>= 8;
      for(i = 0; i < color_num; i++){
        SoftFadePfd(appwk->setup->pfd, req, start_colpos + i,
          1, evy, 
          PaletteWork_ColorGet(appwk->setup->pfd, req, FADEBUF_SRC, 
          change_colpos + i));
      }
    }
    break;
  case PANEL_COLORMODE_FLASH:
    {
      int i;
      
      if(pcc->wait == 0){
        pcc->wait = PANEL_COLOR_FLASH_WAIT;
        if(pcc->evy == 0){
          pcc->evy = 16 << 8;
        }
        else{
          pcc->evy = 0;
          pcc->count++;
          if(pcc->count > PANEL_COLOR_FLASH_COUNT){
            _PanelColor_SetMode(appwk, PANEL_COLORMODE_NONE, req);
          }
        }
        for(i = 0; i < color_num; i++){
          SoftFadePfd(appwk->setup->pfd, req, start_colpos + i,
            1, pcc->evy >> 8, 
            PaletteWork_ColorGet(appwk->setup->pfd, req, FADEBUF_SRC, 
            change_colpos + i));
        }
      }
      else{
        pcc->wait--;
      }
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * パネルカラーアニメ：モード設定
 *
 * @param   appwk		
 * @param   mode		
 * @param   req		
 */
//--------------------------------------------------------------
static void _PanelColor_SetMode(MONOLITH_APP_PARENT *appwk, PANEL_COLORMODE mode, FADEREQ req)
{
  PANEL_COLOR_CONTROL *pcc = &appwk->tool.panel_color[req];
  
  GFL_STD_MemClear(pcc, sizeof(PANEL_COLOR_CONTROL));
  pcc->mode = mode;
  
  //カラー初期化
  SoftFadePfd(appwk->setup->pfd, req, _PanelColor_GetStartColorPos(req), 
    _PanelColor_GetChangeColorNum(req), 0, 0x0000);
}

//--------------------------------------------------------------
/**
 * パネルフェード開始カラー位置を取得する
 * @param   req		
 * @retval  int		カラー位置
 */
//--------------------------------------------------------------
static int _PanelColor_GetStartColorPos(FADEREQ req)
{
  if(req == FADE_MAIN_BG || req == FADE_SUB_BG){
    return PANEL_START_PALNO_BG_D * 16 + PANEL_COLOR_START;
  }
  else{
    return COMMON_PAL_PANEL_FOCUS * 16 + PANEL_COLOR_START;
  }
}

//--------------------------------------------------------------
/**
 * パネルフェードの変化先の色が入っているカラー位置を取得する
 * @param   req		
 * @retval  int		変化先のカラーが入っているカラー位置
 */
//--------------------------------------------------------------
static int _PanelColor_GetChangeDataColorPos(FADEREQ req)
{
  if(req == FADE_MAIN_BG || req == FADE_SUB_BG){
    return PANEL_FADEDATA_PALNO_BG_D * 16 + PANEL_COLOR_START;
  }
  else{
    return PANEL_FADEDATA_PALNO_OBJ * 16 + PANEL_COLOR_START;
  }
}

//--------------------------------------------------------------
/**
 * パネルフェードするカラー数を取得する
 * @param   req		
 * @retval  int		カラー数
 */
//--------------------------------------------------------------
static int _PanelColor_GetChangeColorNum(FADEREQ req)
{
  if(req == FADE_MAIN_BG || req == FADE_SUB_BG){
    return PANEL_COLOR_NUM;
  }
  else{
    return PANEL_COLOR_NUM;
  }
}



//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * BMPOAMアクター生成
 *
 * @param   setup         モノリス全画面共通設定データへのポインタ
 * @param   dest		      生成したBMPOAMアクター代入先
 * @param   res_index		  共通素材Index
 * @param   act_x		      BMPOAMアクター表示座標X(中心座標)
 * @param   act_y		      BMPOAMアクター表示座標Y(中心座標)
 * @param   bmp_size_x		BMPサイズX(キャラクタ単位)
 * @param   bmp_size_y		BMPサイズY(キャラクタ単位)
 * @param   msg_id		    メッセージID
 * @param   wordset       WORDSETが不必要な場合はNULL指定
 * 
 * WORDSETが必要な場合は外側であらかじめWORDSETをしてください
 */
//==================================================================
void MonolithTool_Bmpoam_Create(MONOLITH_SETUP *setup, MONOLITH_BMPSTR *dest, COMMON_RESOURCE_INDEX res_index, int act_x, int act_y, int bmp_size_x, int bmp_size_y, u32 msg_id, WORDSET *wordset)
{
  CLSYS_DEFREND_TYPE defrend_type;
  BMPOAM_ACT_DATA bmpoam_head = BmpOamHead_Str;
  STRBUF *strbuf;
  
  //BMP作成
  dest->bmp = GFL_BMP_Create(bmp_size_x, bmp_size_y, GFL_BMP_16_COLOR, HEAPID_MONOLITH);
  
  //ヘッダ作成
  bmpoam_head.x = act_x - bmp_size_x/2*8;
  bmpoam_head.y = act_y - bmp_size_y/2*8;
  bmpoam_head.bmp = dest->bmp;
  bmpoam_head.pltt_index = setup->common_res[res_index].pltt_bmpfont_index;
  if(res_index == COMMON_RESOURCE_INDEX_UP){
    bmpoam_head.draw_type = CLSYS_DRAW_MAIN;
    bmpoam_head.setSerface = CLSYS_DEFREND_MAIN;
  }
  else{
    bmpoam_head.draw_type = CLSYS_DRAW_SUB;
    bmpoam_head.setSerface = CLSYS_DEFREND_SUB;
  }
  
  //生成
  dest->bmpoam = BmpOam_ActorAdd(setup->bmpoam_sys, &bmpoam_head);
  
  //文字描画
  strbuf = GFL_MSG_CreateString( setup->mm_monolith, msg_id );
  if(wordset != NULL){
    STRBUF *expand_str = GFL_STR_CreateBuffer(128, HEAPID_MONOLITH);
    WORDSET_ExpandStr(wordset, expand_str, strbuf );
    PRINTSYS_PrintQue( setup->printQue, dest->bmp, 0, 0, expand_str, setup->font_handle );
    GFL_STR_DeleteBuffer(expand_str);
  }
  else{
    PRINTSYS_PrintQue( setup->printQue, dest->bmp, 0, 0, strbuf, setup->font_handle );
  }
  GFL_STR_DeleteBuffer(strbuf);
  
  //内部フラグセット
  dest->trans_flag = TRUE;
}

//==================================================================
/**
 * BMPOAMアクター削除
 *
 * @param   panel		パネルアクターへのポインタ
 */
//==================================================================
void MonolithTool_Bmpoam_Delete(MONOLITH_BMPSTR *bmpstr)
{
  BmpOam_ActorDel(bmpstr->bmpoam);
  GFL_BMP_Delete(bmpstr->bmp);
  bmpstr->bmpoam = NULL;
  bmpstr->bmp = NULL;
}

//==================================================================
/**
 * BMPOAMアクター更新処理
 *
 * @param   setup   モノリス全画面共通設定データへのポインタ
 * @param   bmpstr		パネルアクターへのポインタ
 *
 * @retval  BOOL		TRUE:BMP転送を行った
 */
//==================================================================
BOOL MonolithTool_Bmpoam_TransUpdate(MONOLITH_SETUP *setup, MONOLITH_BMPSTR *bmpstr)
{
  if( bmpstr->trans_flag == TRUE 
      && PRINTSYS_QUE_IsExistTarget(setup->printQue, bmpstr->bmp) == FALSE ){
    BmpOam_ActorBmpTrans(bmpstr->bmpoam);
    bmpstr->trans_flag = FALSE;
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * BMPOAMアクターBGプライオリティ設定
 *
 * @param   setup		
 * @param   bmpstr		
 * @param   bg_pri		BGプライオリティ
 */
//==================================================================
void MonolithTool_Bmpoam_BGPriSet(MONOLITH_SETUP *setup, MONOLITH_BMPSTR *bmpstr, int bg_pri)
{
  BmpOam_ActorSetBgPriority( bmpstr->bmpoam, bg_pri );
}

//==================================================================
/**
 * 現在プレイヤーがいるパレスエリアのMYSTATUSを取得する
 *
 * @param   appwk		
 *
 * @retval  OCCUPY_INFO *		占拠情報へのポインタ
 */
//==================================================================
MYSTATUS * MonolithTool_GetMystatus(MONOLITH_APP_PARENT *appwk)
{
  return Intrude_GetMyStatus(GAMESYSTEM_GetGameData(appwk->parent->gsys), appwk->parent->palace_area);
}

//==================================================================
/**
 * 現在プレイヤーがいるパレスエリアの占拠情報を取得する
 *
 * @param   appwk		
 *
 * @retval  OCCUPY_INFO *		占拠情報へのポインタ
 */
//==================================================================
OCCUPY_INFO * MonolithTool_GetOccupyInfo(MONOLITH_APP_PARENT *appwk)
{
  return Intrude_GetOccupyInfo(GAMESYSTEM_GetGameData(appwk->parent->gsys), appwk->parent->palace_area);
}

//==================================================================
/**
 * 街アイコンアクターを作成
 *
 * @param   setup		    
 * @param   occupy		  占拠情報へのポインタ
 * @param   town_no		  街No
 * @param   pos		      表示座標
 * @param   res_index		COMMON_RESOURCE_INDEX
 *
 * @retval  GFL_CLWK *		
 */
//==================================================================
GFL_CLWK * MonolithTool_TownIcon_Create(MONOLITH_SETUP *setup, const OCCUPY_INFO *occupy, int town_no, const GFL_POINT *pos, COMMON_RESOURCE_INDEX res_index)
{
  GFL_CLWK *cap;
  u16 setSerface;
  GFL_CLWK_DATA ActHead_Town = {  //街アイコンのアクターヘッダ
    0, 0, //pos_x, pos_y
    COMMON_ANMSEQ_BLACK_TOWN, //anmseq
    10, 2, //softpri, bgpri
  };
  
  setSerface = (res_index == COMMON_RESOURCE_INDEX_UP) ? CLSYS_DEFREND_MAIN : CLSYS_DEFREND_SUB;
  
  ActHead_Town.pos_x = pos->x;
  ActHead_Town.pos_y = pos->y;
  if(occupy->mlst.mission_clear[town_no] == MISSION_CLEAR_WHITE){
    ActHead_Town.anmseq = COMMON_ANMSEQ_WHITE_TOWN;
  }
  
  cap = GFL_CLACT_WK_Create( setup->clunit, 
    setup->common_res[res_index].char_index, 
    setup->common_res[res_index].pltt_index, 
    setup->common_res[res_index].cell_index, 
    &ActHead_Town, setSerface, HEAPID_MONOLITH );

  if(occupy->mlst.mission_clear[town_no] == MISSION_CLEAR_NONE){
    GFL_CLACT_WK_SetDrawEnable( cap, FALSE );  //中立なので非表示
  }
  GFL_CLACT_WK_SetAutoAnmFlag( cap, TRUE );
  
  return cap;
}

//==================================================================
/**
 * 街アイコンアクター削除
 *
 * @param   cap		
 */
//==================================================================
void MonolithTool_TownIcon_Delete(GFL_CLWK *cap)
{
  GFL_CLACT_WK_Remove(cap);
}

//==================================================================
/**
 * 街アイコンアクター更新
 *
 * @param   cap		
 * @param   occupy		
 * @param   town_no		
 */
//==================================================================
void MonolithTool_TownIcon_Update(GFL_CLWK *cap, const OCCUPY_INFO *occupy, int town_no)
{
  int anmseq;
  
  if(occupy->mlst.mission_clear[town_no] == MISSION_CLEAR_WHITE){
    anmseq = COMMON_ANMSEQ_WHITE_TOWN;
  }
  else if(occupy->mlst.mission_clear[town_no] == MISSION_CLEAR_BLACK){
    anmseq = COMMON_ANMSEQ_BLACK_TOWN;
  }
  else{
    GFL_CLACT_WK_SetDrawEnable( cap, FALSE );  //中立なので非表示
    return;
  }

  GFL_CLACT_WK_SetDrawEnable( cap, TRUE );
  GFL_CLACT_WK_SetAnmSeqDiff( cap, anmseq );
}

//==================================================================
/**
 * 街アイコンカーソルアクターを作成
 *
 * @param   setup		    
 * @param   pos		      表示座標
 * @param   res_index		COMMON_RESOURCE_INDEX
 *
 * @retval  GFL_CLWK *		
 */
//==================================================================
GFL_CLWK * MonolithTool_TownCursor_Create(MONOLITH_SETUP *setup, const GFL_POINT *pos, COMMON_RESOURCE_INDEX res_index)
{
  GFL_CLWK *cap;
  u16 setSerface;
  GFL_CLWK_DATA ActHead_TownCursor = {  //街アイコンカーソルのアクターヘッダ
    0, 0, //pos_x, pos_y
    COMMON_ANMSEQ_TOWN_CURSOR, //anmseq
    0, 2, //softpri, bgpri
  };
  
  setSerface = (res_index == COMMON_RESOURCE_INDEX_UP) ? CLSYS_DEFREND_MAIN : CLSYS_DEFREND_SUB;
  
  ActHead_TownCursor.pos_x = pos->x;
  ActHead_TownCursor.pos_y = pos->y;
  
  cap = GFL_CLACT_WK_Create( setup->clunit, 
    setup->common_res[res_index].char_index, 
    setup->common_res[res_index].pltt_index, 
    setup->common_res[res_index].cell_index, 
    &ActHead_TownCursor, setSerface, HEAPID_MONOLITH );

  return cap;
}

//==================================================================
/**
 * 街アイコンカーソルアクター削除
 *
 * @param   cap		
 */
//==================================================================
void MonolithTool_TownCursor_Delete(GFL_CLWK *cap)
{
  GFL_CLACT_WK_Remove(cap);
}

//==================================================================
/**
 * キャンセルアイコンアクターを作成
 *
 * @param   setup		    
 *
 * @retval  GFL_CLWK *		
 */
//==================================================================
void MonolithTool_CancelIcon_Create(MONOLITH_SETUP *setup, MONOLITH_CANCEL_ICON *cancel)
{
  GFL_CLWK *cap;
  GFL_CLWK_DATA ActHead_Cancel = {  //キャンセルアイコンのアクターヘッダ
    CANCEL_POS_X, CANCEL_POS_Y, //pos_x, pos_y
    COMMON_ANMSEQ_RETURN, //anmseq
    0, 2, //softpri, bgpri
  };
  
  cap = GFL_CLACT_WK_Create( setup->clunit, 
    setup->common_res[COMMON_RESOURCE_INDEX_DOWN].char_index, 
    setup->common_res[COMMON_RESOURCE_INDEX_DOWN].pltt_index, 
    setup->common_res[COMMON_RESOURCE_INDEX_DOWN].cell_index, 
    &ActHead_Cancel, CLSYS_DEFREND_SUB, HEAPID_MONOLITH );

  GFL_CLACT_WK_SetAutoAnmFlag( cap, TRUE );
  
  cancel->cap = cap;
}

//==================================================================
/**
 * キャンセルアイコンアクター削除
 *
 * @param   cap		
 */
//==================================================================
void MonolithTool_CancelIcon_Delete(MONOLITH_CANCEL_ICON *cancel)
{
  GFL_CLACT_WK_Remove(cancel->cap);
}

//==================================================================
/**
 * キャンセルアイコンアクター更新
 *
 * @param   cap		
 */
//==================================================================
void MonolithTool_CancelIcon_Update(MONOLITH_CANCEL_ICON *cancel)
{
  if(cancel->flash_enable == TRUE){
    if(cancel->wait % CANCEL_ICON_FLASH_WAIT == 0){
      GFL_CLACT_WK_SetPlttOffs(cancel->cap, COMMON_PAL_RETURN + cancel->flash_anm, 
        CLWK_PLTTOFFS_MODE_PLTT_TOP);
      cancel->flash_anm ^= 1;
      if(cancel->wait / CANCEL_ICON_FLASH_WAIT == 3){
        cancel->flash_enable = FALSE;
      }
    }
    cancel->wait++;
  }
}

//==================================================================
/**
 * キャンセルアイコンアクター：フラッシュリクエスト
 *
 * @param   cancel		
 */
//==================================================================
void MonolithTool_CancelIcon_FlashReq(MONOLITH_CANCEL_ICON *cancel)
{
  cancel->flash_enable = TRUE;
  cancel->wait = 0;
  cancel->flash_anm = 1;
}

//==================================================================
/**
 * キャンセルアイコンアクター：フラッシュ中かどうかを調べる
 * @param   cancel		
 * @retval  BOOL		TRUE:フラッシュ中　FALSE:フラッシュしていない
 */
//==================================================================
BOOL MonolithTool_CancelIcon_FlashCheck(MONOLITH_CANCEL_ICON *cancel)
{
  return cancel->flash_enable;
}

//==================================================================
/**
 * 矢印アクターを作成
 *
 * @param   setup		    
 *
 * @retval  GFL_CLWK *		
 */
//==================================================================
GFL_CLWK * MonolithTool_Arrow_Create(MONOLITH_SETUP *setup, int x, int y, int anmseq)
{
  GFL_CLWK *cap;
  GFL_CLWK_DATA ActHead_Arrow = {  //矢印アクターヘッダ
    0, 0, //pos_x, pos_y
    0, //anmseq
    0, 1, //softpri, bgpri
  };
  
  ActHead_Arrow.pos_x = x;
  ActHead_Arrow.pos_y = y;
  ActHead_Arrow.anmseq = anmseq;
  cap = GFL_CLACT_WK_Create( setup->clunit, 
    setup->common_res[COMMON_RESOURCE_INDEX_DOWN].char_index, 
    setup->common_res[COMMON_RESOURCE_INDEX_DOWN].pltt_index, 
    setup->common_res[COMMON_RESOURCE_INDEX_DOWN].cell_index, 
    &ActHead_Arrow, CLSYS_DEFREND_SUB, HEAPID_MONOLITH );

  GFL_CLACT_WK_SetAutoAnmFlag( cap, TRUE );
  
  return cap;
}

//==================================================================
/**
 * 矢印アクター削除
 *
 * @param   cap		
 */
//==================================================================
void MonolithTool_ArrowIcon_Delete(GFL_CLWK *cap)
{
  GFL_CLACT_WK_Remove(cap);
}

//==================================================================
/**
 * 矢印アクター更新
 *
 * @param   cap		
 */
//==================================================================
void MonolithTool_ArrowIcon_Update(GFL_CLWK *cap)
{
  return;
}

//==================================================================
/**
 * メニューシステム：リソース読み込み
 *
 * @param   setup		
 * @param   frame_no		          フレーム番号
 * @param   heap_id		            ヒープID
 *
 * @retval  APP_TASKMENU_RES *		
 */
//==================================================================
APP_TASKMENU_RES * MonolithTool_TaskMenuCreate(MONOLITH_SETUP *setup, int frame_no, HEAPID heap_id)
{
  APP_TASKMENU_RES *app_res;
  
  app_res = APP_TASKMENU_RES_Create(frame_no, MONOLITH_BG_YESNO_PALNO, 
    setup->font_handle, setup->printQue, heap_id);
  return app_res;
}

//==================================================================
/**
 * メニューシステム：リソース破棄
 *
 * @param   app_task_res		
 */
//==================================================================
void MonolithTool_TaskMenuDelete(APP_TASKMENU_RES *app_task_res)
{
  APP_TASKMENU_RES_Delete(app_task_res);
}

//==================================================================
/**
 * メニューシステム：「はい/いいえ」ウィンドウセットアップ
 *
 * @param   setup		
 * @param   app_task_res		
 * @param   heap_id		
 *
 * @retval  APP_TASKMENU_WORK *		
 */
//==================================================================
APP_TASKMENU_WORK * MonolithTool_TaskMenu_YesNoInit(MONOLITH_SETUP *setup, APP_TASKMENU_RES *app_task_res, HEAPID heap_id)
{
  APP_TASKMENU_ITEMWORK itemWork[2];
  APP_TASKMENU_INITWORK initWork;
  APP_TASKMENU_WORK *app_menu;
  
  itemWork[0].str = GFL_MSG_CreateString( setup->mm_monolith , msg_mono_pow_006 );  //はい
  itemWork[1].str = GFL_MSG_CreateString( setup->mm_monolith , msg_mono_pow_007 );  //いいえ
  itemWork[0].msgColor = PRINTSYS_LSB_Make(0xe, 0xf, 0);
  itemWork[1].msgColor = PRINTSYS_LSB_Make(0xe, 0xf, 0);
  itemWork[0].type = APP_TASKMENU_WIN_TYPE_NORMAL;
  itemWork[1].type = APP_TASKMENU_WIN_TYPE_NORMAL;

  initWork.heapId = heap_id;
  initWork.itemNum = 2;
  initWork.itemWork = itemWork;
  initWork.posType = ATPT_LEFT_UP;
  initWork.charPosX = _INFO_YESNO_X;
  initWork.charPosY = _INFO_YESNO_Y;
  initWork.w = APP_TASKMENU_PLATE_WIDTH_YN_WIN;
  initWork.h = APP_TASKMENU_PLATE_HEIGHT_YN_WIN;

  app_menu = APP_TASKMENU_OpenMenu( &initWork, app_task_res );
  
  GFL_STR_DeleteBuffer( itemWork[0].str );
  GFL_STR_DeleteBuffer( itemWork[1].str );
  
  return app_menu;
}

//==================================================================
/**
 * 「はい/いいえ」ウィンドウ破棄
 *
 * @param   app_menu		
 */
//==================================================================
void MonolithTool_TaskMenu_YesNoExit(APP_TASKMENU_WORK *app_menu)
{
  APP_TASKMENU_CloseMenu( app_menu );
}

//==================================================================
/**
 * 「はい/いいえ」ウィンドウ更新処理
 *
 * @param   app_menu		
 * @param   ret_yesno		戻り値がTRUEの場合、選択結果が入る(TRUE:はい、FALSE:いいえ)
 *
 * @retval  BOOL		TRUE:選択終了　FALSE:選択中
 */
//==================================================================
BOOL MonolithTool_TaskMenu_Update(MONOLITH_SETUP *setup, int frame_no, APP_TASKMENU_WORK *app_menu, BOOL *ret_yesno)
{
  APP_TASKMENU_UpdateMenu( app_menu );
  if(APP_TASKMENU_IsFinish( app_menu ) == TRUE){
    if(APP_TASKMENU_GetCursorPos( app_menu ) == 0){
      *ret_yesno = TRUE;
    }
    else{
      *ret_yesno = FALSE;
    }
    return TRUE;
  }
  return FALSE;
}


///バランスゲージ描画用の設定値
enum{
  BALANCE_GAUGE_CHARA_MAX = 30,     ///<バランスゲージの横長キャラ数
  BALANCE_GAUGE_DOTTO_MAX = BALANCE_GAUGE_CHARA_MAX * 8,    ///<バランスゲージのドット数
  BALANCE_SCRN_START_X = 1,
  
  BALANCE_UP_SCRN_START_Y = 0x14,
  BALANCE_DOWN_SCRN_START_Y = 0xe,
  
  BALANCE_SCRN_BLACK_CHARNO_START = 1,  ///<ブラックゲージのキャラクタNo開始位置
  BALANCE_SCRN_MAXWHITE_CHARNO = 9,     ///<ホワイトゲージ(MAX)のキャラクタNo開始位置
  ONECHARA_DOT = 8,   ///<1つのキャラクタのドット数
  BALANCE_GAUGE_PALNO = 1,          ///<バランスゲージのパレット番号
};

//--------------------------------------------------------------
/**
 * バランスゲージのスクリーンを書き直す
 *
 * @param   appwk		
 */
//--------------------------------------------------------------
void MonolithTool_BalanceGaugeRewrite(MONOLITH_APP_PARENT *appwk, BALANCE_GAUGE_DISP up_down, int frame_no)
{
  const OCCUPY_INFO *occupy = MonolithTool_GetOccupyInfo(appwk);
  int lv_w, lv_b, lv_total;
  int dot_w, dot_b;
  int pos;
  u16 *scrnbuf;
  int start_y;
  
  if(up_down == BALANCE_GAUGE_UP){
    start_y = BALANCE_UP_SCRN_START_Y;
  }
  else{
    start_y = BALANCE_DOWN_SCRN_START_Y;
  }
  
  lv_w = occupy->white_level;
  lv_b = occupy->black_level;
  lv_total = lv_w + lv_b;
  
  if(lv_w == lv_b){ //初期値がレベル0同士の場合があるので
    dot_w = BALANCE_GAUGE_DOTTO_MAX / 2;
    dot_b = BALANCE_GAUGE_DOTTO_MAX - dot_w;
  }
  else{
    dot_w = BALANCE_GAUGE_DOTTO_MAX * lv_w / lv_total;
    if(lv_b > 0){ //小数切捨てによって出た部分がレベル0のブラックに加算されないようにチェック
      dot_b = BALANCE_GAUGE_DOTTO_MAX - dot_w;
    }
    else{
      dot_b = 0;
      dot_w = BALANCE_GAUGE_DOTTO_MAX;
    }
  }
  //レベルが1以上あるなら計算上ドットが0になっていも1ドットは入れる
  if(lv_w > 0 && dot_w == 0){
    dot_w++;
    dot_b--;
  }
  else if(lv_b > 0 && dot_b == 0){
    dot_b++;
    dot_w--;
  }
  
  scrnbuf = GFL_HEAP_AllocClearMemory(HEAPID_MONOLITH, BALANCE_GAUGE_CHARA_MAX * sizeof(u16));
  
  //黒のゲージを描画
  pos = 0;
  while(dot_b > 0){
    if(dot_b >= ONECHARA_DOT){
      scrnbuf[pos] = BALANCE_SCRN_BLACK_CHARNO_START;
      dot_b -= ONECHARA_DOT;
    }
    else{
      scrnbuf[pos] = ONECHARA_DOT - dot_b + BALANCE_SCRN_BLACK_CHARNO_START;
      dot_b = 0;
    }
    pos++;
  }
  //残りを白MAXで埋める
  for( ; pos < BALANCE_GAUGE_CHARA_MAX; pos++){
    scrnbuf[pos] = BALANCE_SCRN_MAXWHITE_CHARNO;
  }
  //パレットNoをスクリーンに入れる
  for(pos = 0; pos < BALANCE_GAUGE_CHARA_MAX; pos++){
    scrnbuf[pos] |= BALANCE_GAUGE_PALNO << 12;
  }
  
  //スクリーン描画
  GFL_BG_WriteScreen( frame_no, scrnbuf, 
    BALANCE_SCRN_START_X, start_y, BALANCE_GAUGE_CHARA_MAX, 1);
  
  GFL_HEAP_FreeMemory(scrnbuf);
}


#if 0
//--------------------------------------------------------------
/**
 * @brief   メッセージ描画関連の設定
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_MessageSetting(COMPATI_SYS *cs)
{
	int i;

	cs->drawwin[CCBMPWIN_TITLE].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 8, 2, 16, 2, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	cs->drawwin[CCBMPWIN_POINT].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 14, 8, 6, 2, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	cs->drawwin[CCBMPWIN_TALK].win = GFL_BMPWIN_Create(
		FRAME_MSG_S, 1, 18, 30, 4, CC_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
	for(i = 0; i < CCBMPWIN_MAX; i++){
		GF_ASSERT(cs->drawwin[i].win != NULL);
		cs->drawwin[i].bmp = GFL_BMPWIN_GetBmp(cs->drawwin[i].win);
		GFL_BMP_Clear( cs->drawwin[i].bmp, 0xff );
		GFL_BMPWIN_MakeScreen( cs->drawwin[i].win );
		GFL_BMPWIN_TransVramCharacter( cs->drawwin[i].win );
		PRINT_UTIL_Setup( cs->drawwin[i].printUtil, cs->drawwin[i].win );
	}
	GFL_BG_LoadScreenReq(FRAME_MSG_S);	//MakeScreenしたのを反映

	cs->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_MONOLITH );

//		PRINTSYS_Init( HEAPID_MONOLITH );
	cs->printQue = PRINTSYS_QUE_Create( HEAPID_MONOLITH );

	cs->mm = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_compati_check_dat, HEAPID_MONOLITH);
	for(i = 0; i < CCBMPWIN_MAX; i++){
		cs->strbuf_win[i] = GFL_STR_CreateBuffer( 64, HEAPID_MONOLITH );
	}
	cs->strbuf_point = GFL_MSG_CreateString(cs->mm, COMPATI_STR_009);
}

//--------------------------------------------------------------
/**
 * @brief   円アクター作成
 *
 * @param   cs		
 * @param   hdl		アーカイブハンドル
 */
//--------------------------------------------------------------
static void CCLocal_CircleActor_Create(COMPATI_SYS *cs, ARCHANDLE *hdl)
{
	int i;
	
	//リソース登録
	cs->cgr_id[CHARID_CIRCLE] = GFL_CLGRP_CGR_Register(
		hdl, NARC_compati_check_cc_circle_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_MONOLITH);
	cs->cell_id[CELLID_CIRCLE] = GFL_CLGRP_CELLANIM_Register(
		hdl, NARC_compati_check_cc_circle_NCER, NARC_compati_check_cc_circle_NANR, HEAPID_MONOLITH);
	//パレットリソース登録(押した円だけ色を変えるので円の数分登録)
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		cs->pltt_id[PLTTID_CIRCLE + i] = GFL_CLGRP_PLTT_RegisterEx(hdl, 
			NARC_compati_check_cc_circle_NCLR,
			CLSYS_DRAW_MAIN, 0x20 * i, 0, 1, HEAPID_MONOLITH);
	}
	
	//アクター登録
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		cs->circle_cap[i] = GFL_CLACT_WK_Create(cs->clunit, cs->cgr_id[CHARID_CIRCLE], 
			cs->pltt_id[PLTTID_CIRCLE + i], cs->cell_id[CELLID_CIRCLE], 
			&CircleObjParam, CLSYS_DEFREND_MAIN, HEAPID_MONOLITH);
		GFL_CLACT_WK_AddAnmFrame(cs->circle_cap[i], FX32_ONE);
		GFL_CLACT_WK_SetDrawEnable(cs->circle_cap[i], FALSE);
	}
}

//--------------------------------------------------------------
/**
 * @brief   円アクター破棄
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_CircleActor_Delete(COMPATI_SYS *cs)
{
	int i;
	
	//アクター削除
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		GFL_CLACT_WK_Remove(cs->circle_cap[i]);
	}
	
	//リソース解放
	GFL_CLGRP_CGR_Release(cs->cgr_id[CHARID_CIRCLE]);
	GFL_CLGRP_CELLANIM_Release(cs->cell_id[CELLID_CIRCLE]);
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		GFL_CLGRP_PLTT_Release(cs->pltt_id[PLTTID_CIRCLE + i]);
	}
}
#endif

