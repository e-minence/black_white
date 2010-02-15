//==============================================================================
/**
 * @file    monolith_tool.c
 * @brief   ���m���X��ʃc�[��
 * @author  matsuda
 * @date    2009.11.21(�y)
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


//==============================================================================
//  �萔��`
//==============================================================================
///�\�t�g�v���C�I���e�B
enum{
  SOFTPRI_PANEL_BMPFONT = 10,
  SOFTPRI_PANEL,
};

///�A�N�^�[BG�v���C�I���e�B
enum{
  BGPRI_PANEL = 1,
};

///�p�l��(��)BMPFONT���WX
#define PANEL_LARGE_BMPFONT_POS_X(panel_x)   (panel_x - PANEL_LARGE_CHARSIZE_X/2*8)
///�p�l��(��)BMPFONT���WX
#define PANEL_SMALL_BMPFONT_POS_X(panel_x)   (panel_x - PANEL_SMALL_CHARSIZE_X/2*8)
///�p�l��(����)BMPFONT���WX
#define PANEL_DECIDE_BMPFONT_POS_X(panel_x)   (panel_x - PANEL_DECIDE_CHARSIZE_X/2*8)


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _PanelColor_SetMode(MONOLITH_APP_PARENT *appwk, PANEL_COLORMODE mode, FADEREQ req);
static int _PanelColor_GetStartColorPos(FADEREQ req);
static int _PanelColor_GetChangeDataColorPos(FADEREQ req);
static int _PanelColor_GetChangeColorNum(FADEREQ req);


//==============================================================================
//  �f�[�^
//==============================================================================
//--------------------------------------------------------------
//  �p�l��
//--------------------------------------------------------------
///�p�l���A�N�^�[�w�b�_
static const GFL_CLWK_DATA ActHead_Panel = {
  PANEL_POS_X,                   //pos_x
  0,                             //pos_y
  COMMON_ANMSEQ_PANEL_LARGE,     //anmseq
  SOFTPRI_PANEL,                 //softpri
  BGPRI_PANEL,                   //bgpri
};

///�p�l���A�N�^�[��BMPOAM�w�b�_
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
//  �p�l���J���[�A�j��
//--------------------------------------------------------------
//�p�l���J���[�A�j���ݒ�
enum{
  PANEL_COLOR_START = 2,      ///<�p�l���J���[�A�j���J�n�ʒu
  PANEL_COLOR_END = 4,        ///<�p�l���J���[�A�j���I�[�ʒu
  PANEL_COLOR_NUM = PANEL_COLOR_END - PANEL_COLOR_START + 1,  ///<�p�l���J���[�A�j���̃J���[��
  
  PANEL_FADEDATA_PALNO_OBJ = 2,    ///<�p�l���t�F�[�h�f�[�^(�ύX��̐F)�������Ă���p���b�g�ʒu

  PANEL_START_PALNO_BG_D = COMMON_PALBG_PANEL_FOCUS,      ///<�p�l���t�F�[�h�J�n�p���b�gNo
  PANEL_FADEDATA_PALNO_BG_D = 4,   ///<�p�l���t�F�[�h�f�[�^(�ύX��̐F)�������Ă���p���b�g�ʒu
  
  PANEL_COLOR_FADE_ADD_EVY = 0x0080,    ///<EVY���Z�l(����8�r�b�g����)
  
  PANEL_COLOR_FLASH_WAIT = 3,     ///<�t���b�V���̃E�F�C�g
  PANEL_COLOR_FLASH_COUNT = 1,    ///<�t���b�V����
};

#if 0 //PANEL_FADEDATA_PALNO�̈ʒu�ɐF�����Ă����悤�ɕύX
///�p�l���J���[�A�j���Ńt�H�[�J�X���ꂽ���Ɍ�����EVY�ő哞�B�J���[
ALIGN4 static const u16 PanelFocusColor[] = {
  0x7ff9, 0x7fd3, 0x7f10,
};
SDK_COMPILER_ASSERT(NELEMS(PanelFocusColor) == PANEL_COLOR_NUM);
#endif

//--------------------------------------------------------------
//  BMPFONT
//--------------------------------------------------------------
///���m���XBMPOAM�w�b�_
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


//==================================================================
/**
 * �p�l���A�N�^�[����
 *
 * @param   setup       ���m���X�S��ʋ��ʐݒ�f�[�^�ւ̃|�C���^
 * @param   dest		    ���������p�l���A�N�^�[�����
 * @param   res_index		���ʑf��Index
 * @param   size		    �p�l���T�C�Y
 * @param   x           X���W(���S)
 * @param   y		        Y���W(���S)
 * @param   msg_id      ���b�Z�[�WID
 * @param   wordset       WORDSET���s�K�v�ȏꍇ��NULL�w��
 * 
 * WORDSET���K�v�ȏꍇ�͊O���ł��炩����WORDSET�����Ă�������
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
  
  //BMP�쐬
  dest->bmp = GFL_BMP_Create(bmp_size_x, PANEL_CHARSIZE_Y, GFL_BMP_16_COLOR, HEAPID_MONOLITH);
  
  //�w�b�_�쐬
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
  
  //����
  dest->cap = GFL_CLACT_WK_Create(setup->clunit, setup->common_res[res_index].char_index, 
    setup->common_res[res_index].pltt_index, setup->common_res[res_index].cell_index, 
	  &head, defrend_type, HEAPID_MONOLITH);
  dest->bmpoam = BmpOam_ActorAdd(setup->bmpoam_sys, &bmpoam_head);
  
  //�����`��
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
  
  //�����t���O�Z�b�g
  dest->trans_flag = TRUE;
}

//==================================================================
/**
 * �p�l���A�N�^�[�폜
 *
 * @param   panel		�p�l���A�N�^�[�ւ̃|�C���^
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
 * �p�l���A�N�^�[�̕\���ݒ�
 *
 * @param   panel		
 * @param   on_off		TRUE:�\��ON�A�@FALSE:�\��OFF
 */
//==================================================================
void MonolithTool_PanelOBJ_SetEnable(PANEL_ACTOR *panel, BOOL on_off)
{
  GFL_CLACT_WK_SetDrawEnable( panel->cap, on_off );
  BmpOam_ActorSetDrawEnable( panel->bmpoam, on_off );
}

//==================================================================
/**
 * �p�l���A�N�^�[�X�V����
 *
 * @param   setup   ���m���X�S��ʋ��ʐݒ�f�[�^�ւ̃|�C���^
 * @param   panel		�p�l���A�N�^�[�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:BMP�]�����s����
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
 * �p�l���ɑ΂��ăt�H�[�J�X��ݒ肷��
 *
 * @param   appwk
 * @param   panel[]		  �p�l���A�N�^�[�̔z��ւ̃|�C���^
 * @param   panel_max		�p�l���A�N�^�[�z��̗v�f��
 * @param   focus_no		�t�H�[�J�X�����p�l���ԍ�(�����t�H�[�J�X���Ȃ��ꍇ��PANEL_NO_FOCUS)
 * @param   req         FADE_SUB_OBJ��
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
 * �p�l���ɑ΂��ăt���b�V�����s��
 *
 * @param   appwk
 * @param   panel[]		  �p�l���A�N�^�[�̔z��ւ̃|�C���^
 * @param   panel_max		�p�l���A�N�^�[�z��̗v�f��
 * @param   focus_no		�t���b�V������p�l���ԍ�(�����t�H�[�J�X���Ȃ��ꍇ��PANEL_NO_FOCUS)
 * @param   req         FADE_SUB_OBJ��
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
 * �p�l��BG�ɑ΂��ăt�H�[�J�X��ݒ肷��
 *
 * @param   appwk
 * @param   focus_use		TRUE:�t�H�[�J�X�A�j���J�n�B�@FALSE:�t�H�[�J�X�A�j����~
 * @param   req         FADE_SUB_OBJ��
 *
 * PanelOBJ�ƈႢ�t�H�[�J�X�Ώۂ̃X�N���[���͊O���œƎ��ɕҏW����K�v������
 * ���̊֐��ōs���̂̓t�H�[�J�X�A�j���̊J�n�ƒ�~�̐���̂�
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
 * �p�l��BG�ɑ΂��ăt���b�V�����s��
 *
 * @param   appwk
 * @param   req         FADE_SUB_OBJ��
 *
 * PanelOBJ�ƈႢ�t�H�[�J�X�Ώۂ̃X�N���[���͊O���œƎ��ɕҏW����K�v������
 * ���̊֐��ōs���̂̓t���b�V���A�j���̊J�n�X�C�b�`�����邾��
 */
//==================================================================
void MonolithTool_PanelBG_Flash(MONOLITH_APP_PARENT *appwk, FADEREQ req)
{
  _PanelColor_SetMode(appwk, PANEL_COLORMODE_FLASH, req);
}

//==================================================================
/**
 * �p�l���V�X�e��������
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
 * �p�l���J���[�A�j���F���[�h�擾
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
 * �p�l���J���[�A�j���F�X�V����
 *
 * @param   appwk		
 */
//==================================================================
void MonolithTool_Panel_ColorUpdate(MONOLITH_APP_PARENT *appwk, FADEREQ req)
{
  PANEL_COLOR_CONTROL *pcc = &appwk->tool.panel_color[req];
  int start_colpos, change_colpos, color_num;
  
  GF_ASSERT(req < PANEL_CONTROL_MAX);
  
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
 * �p�l���J���[�A�j���F���[�h�ݒ�
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
  
  //�J���[������
  SoftFadePfd(appwk->setup->pfd, req, _PanelColor_GetStartColorPos(req), 
    _PanelColor_GetChangeColorNum(req), 0, 0x0000);
}

//--------------------------------------------------------------
/**
 * �p�l���t�F�[�h�J�n�J���[�ʒu���擾����
 * @param   req		
 * @retval  int		�J���[�ʒu
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
 * �p�l���t�F�[�h�̕ω���̐F�������Ă���J���[�ʒu���擾����
 * @param   req		
 * @retval  int		�ω���̃J���[�������Ă���J���[�ʒu
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
 * �p�l���t�F�[�h����J���[�����擾����
 * @param   req		
 * @retval  int		�J���[��
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
 * BMPOAM�A�N�^�[����
 *
 * @param   setup         ���m���X�S��ʋ��ʐݒ�f�[�^�ւ̃|�C���^
 * @param   dest		      ��������BMPOAM�A�N�^�[�����
 * @param   res_index		  ���ʑf��Index
 * @param   act_x		      BMPOAM�A�N�^�[�\�����WX(���S���W)
 * @param   act_y		      BMPOAM�A�N�^�[�\�����WY(���S���W)
 * @param   bmp_size_x		BMP�T�C�YX(�L�����N�^�P��)
 * @param   bmp_size_y		BMP�T�C�YY(�L�����N�^�P��)
 * @param   msg_id		    ���b�Z�[�WID
 * @param   wordset       WORDSET���s�K�v�ȏꍇ��NULL�w��
 * 
 * WORDSET���K�v�ȏꍇ�͊O���ł��炩����WORDSET�����Ă�������
 */
//==================================================================
void MonolithTool_Bmpoam_Create(MONOLITH_SETUP *setup, MONOLITH_BMPSTR *dest, COMMON_RESOURCE_INDEX res_index, int act_x, int act_y, int bmp_size_x, int bmp_size_y, u32 msg_id, WORDSET *wordset)
{
  CLSYS_DEFREND_TYPE defrend_type;
  BMPOAM_ACT_DATA bmpoam_head = BmpOamHead_Str;
  STRBUF *strbuf;
  
  //BMP�쐬
  dest->bmp = GFL_BMP_Create(bmp_size_x, bmp_size_y, GFL_BMP_16_COLOR, HEAPID_MONOLITH);
  
  //�w�b�_�쐬
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
  
  //����
  dest->bmpoam = BmpOam_ActorAdd(setup->bmpoam_sys, &bmpoam_head);
  
  //�����`��
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
  
  //�����t���O�Z�b�g
  dest->trans_flag = TRUE;
}

//==================================================================
/**
 * BMPOAM�A�N�^�[�폜
 *
 * @param   panel		�p�l���A�N�^�[�ւ̃|�C���^
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
 * BMPOAM�A�N�^�[�X�V����
 *
 * @param   setup   ���m���X�S��ʋ��ʐݒ�f�[�^�ւ̃|�C���^
 * @param   bmpstr		�p�l���A�N�^�[�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:BMP�]�����s����
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
 * BMPOAM�A�N�^�[BG�v���C�I���e�B�ݒ�
 *
 * @param   setup		
 * @param   bmpstr		
 * @param   bg_pri		BG�v���C�I���e�B
 */
//==================================================================
void MonolithTool_Bmpoam_BGPriSet(MONOLITH_SETUP *setup, MONOLITH_BMPSTR *bmpstr, int bg_pri)
{
  BmpOam_ActorSetBgPriority( bmpstr->bmpoam, bg_pri );
}

//==================================================================
/**
 * ���݃v���C���[������p���X�G���A�̐苒�����擾����
 *
 * @param   appwk		
 *
 * @retval  OCCUPY_INFO *		�苒���ւ̃|�C���^
 */
//==================================================================
OCCUPY_INFO * MonolithTool_GetOccupyInfo(MONOLITH_APP_PARENT *appwk)
{
  return Intrude_GetOccupyInfo(appwk->parent->intcomm, appwk->parent->palace_area);
}

//==================================================================
/**
 * �X�A�C�R���A�N�^�[���쐬
 *
 * @param   setup		    
 * @param   occupy		  �苒���ւ̃|�C���^
 * @param   town_no		  �XNo
 * @param   pos		      �\�����W
 * @param   res_index		COMMON_RESOURCE_INDEX
 *
 * @retval  GFL_CLWK *		
 */
//==================================================================
GFL_CLWK * MonolithTool_TownIcon_Create(MONOLITH_SETUP *setup, const OCCUPY_INFO *occupy, int town_no, const GFL_POINT *pos, COMMON_RESOURCE_INDEX res_index)
{
  GFL_CLWK *cap;
  u16 setSerface;
  GFL_CLWK_DATA ActHead_Town = {  //�X�A�C�R���̃A�N�^�[�w�b�_
    0, 0, //pos_x, pos_y
    COMMON_ANMSEQ_BLACK_TOWN, //anmseq
    0, 2, //softpri, bgpri
  };
  
  setSerface = (res_index == COMMON_RESOURCE_INDEX_UP) ? CLSYS_DEFREND_MAIN : CLSYS_DEFREND_SUB;
  
  ActHead_Town.pos_x = pos->x;
  ActHead_Town.pos_y = pos->y;
  if(occupy->town.town_occupy[town_no] == OCCUPY_TOWN_WHITE){
    ActHead_Town.anmseq = COMMON_ANMSEQ_WHITE_TOWN;
  }
  
  cap = GFL_CLACT_WK_Create( setup->clunit, 
    setup->common_res[res_index].char_index, 
    setup->common_res[res_index].pltt_index, 
    setup->common_res[res_index].cell_index, 
    &ActHead_Town, setSerface, HEAPID_MONOLITH );

  if(occupy->town.town_occupy[town_no] == OCCUPY_TOWN_NEUTRALITY){
    GFL_CLACT_WK_SetDrawEnable( cap, FALSE );  //�����Ȃ̂Ŕ�\��
  }
  GFL_CLACT_WK_SetAutoAnmFlag( cap, TRUE );
  
  return cap;
}

//==================================================================
/**
 * �X�A�C�R���A�N�^�[�폜
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
 * �X�A�C�R���A�N�^�[�X�V
 *
 * @param   cap		
 * @param   occupy		
 * @param   town_no		
 */
//==================================================================
void MonolithTool_TownIcon_Update(GFL_CLWK *cap, const OCCUPY_INFO *occupy, int town_no)
{
  int anmseq;
  
  if(occupy->town.town_occupy[town_no] == OCCUPY_TOWN_WHITE){
    anmseq = COMMON_ANMSEQ_WHITE_TOWN;
  }
  else if(occupy->town.town_occupy[town_no] == OCCUPY_TOWN_BLACK){
    anmseq = COMMON_ANMSEQ_BLACK_TOWN;
  }
  else{
    GFL_CLACT_WK_SetDrawEnable( cap, FALSE );  //�����Ȃ̂Ŕ�\��
    return;
  }

  GFL_CLACT_WK_SetDrawEnable( cap, TRUE );
  GFL_CLACT_WK_SetAnmSeqDiff( cap, anmseq );
}

//==================================================================
/**
 * �L�����Z���A�C�R���A�N�^�[���쐬
 *
 * @param   setup		    
 *
 * @retval  GFL_CLWK *		
 */
//==================================================================
GFL_CLWK * MonolithTool_CancelIcon_Create(MONOLITH_SETUP *setup)
{
  GFL_CLWK *cap;
  GFL_CLWK_DATA ActHead_Cancel = {  //�L�����Z���A�C�R���̃A�N�^�[�w�b�_
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
  
  return cap;
}

//==================================================================
/**
 * �L�����Z���A�C�R���A�N�^�[�폜
 *
 * @param   cap		
 */
//==================================================================
void MonolithTool_CancelIcon_Delete(GFL_CLWK *cap)
{
  GFL_CLACT_WK_Remove(cap);
}

//==================================================================
/**
 * �L�����Z���A�C�R���A�N�^�[�X�V
 *
 * @param   cap		
 */
//==================================================================
void MonolithTool_CancelIcon_Update(GFL_CLWK *cap)
{
  return;
}

//==================================================================
/**
 * ���A�N�^�[���쐬
 *
 * @param   setup		    
 *
 * @retval  GFL_CLWK *		
 */
//==================================================================
GFL_CLWK * MonolithTool_Arrow_Create(MONOLITH_SETUP *setup, int x, int y, int anmseq)
{
  GFL_CLWK *cap;
  GFL_CLWK_DATA ActHead_Arrow = {  //���A�N�^�[�w�b�_
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
 * ���A�N�^�[�폜
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
 * ���A�N�^�[�X�V
 *
 * @param   cap		
 */
//==================================================================
void MonolithTool_ArrowIcon_Update(GFL_CLWK *cap)
{
  return;
}


#if 0
//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�`��֘A�̐ݒ�
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
	GFL_BG_LoadScreenReq(FRAME_MSG_S);	//MakeScreen�����̂𔽉f

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
 * @brief   �~�A�N�^�[�쐬
 *
 * @param   cs		
 * @param   hdl		�A�[�J�C�u�n���h��
 */
//--------------------------------------------------------------
static void CCLocal_CircleActor_Create(COMPATI_SYS *cs, ARCHANDLE *hdl)
{
	int i;
	
	//���\�[�X�o�^
	cs->cgr_id[CHARID_CIRCLE] = GFL_CLGRP_CGR_Register(
		hdl, NARC_compati_check_cc_circle_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_MONOLITH);
	cs->cell_id[CELLID_CIRCLE] = GFL_CLGRP_CELLANIM_Register(
		hdl, NARC_compati_check_cc_circle_NCER, NARC_compati_check_cc_circle_NANR, HEAPID_MONOLITH);
	//�p���b�g���\�[�X�o�^(�������~�����F��ς���̂ŉ~�̐����o�^)
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		cs->pltt_id[PLTTID_CIRCLE + i] = GFL_CLGRP_PLTT_RegisterEx(hdl, 
			NARC_compati_check_cc_circle_NCLR,
			CLSYS_DRAW_MAIN, 0x20 * i, 0, 1, HEAPID_MONOLITH);
	}
	
	//�A�N�^�[�o�^
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
 * @brief   �~�A�N�^�[�j��
 *
 * @param   cs		
 */
//--------------------------------------------------------------
static void CCLocal_CircleActor_Delete(COMPATI_SYS *cs)
{
	int i;
	
	//�A�N�^�[�폜
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		GFL_CLACT_WK_Remove(cs->circle_cap[i]);
	}
	
	//���\�[�X���
	GFL_CLGRP_CGR_Release(cs->cgr_id[CHARID_CIRCLE]);
	GFL_CLGRP_CELLANIM_Release(cs->cell_id[CELLID_CIRCLE]);
	for(i = 0; i < CC_CIRCLE_MAX; i++){
		GFL_CLGRP_PLTT_Release(cs->pltt_id[PLTTID_CIRCLE + i]);
	}
}
#endif

