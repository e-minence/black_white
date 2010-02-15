//==============================================================================
/**
 * @file    monolith_title.c
 * @brief   ���m���X�F�~�b�V��������
 * @author  matsuda
 * @date    2009.11.20(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "monolith_common.h"
#include "arc_def.h"
#include "monolith_tool.h"
#include "intrude_mission.h"
#include "msg/msg_monolith.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�p�l����
enum{
  TITLE_PANEL_MISSION,
  TITLE_PANEL_POWER,
  TITLE_PANEL_STATUS,
  
  TITLE_PANEL_MAX,
  TITLE_PANEL_CANCEL = TITLE_PANEL_MAX,
};

//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  PANEL_ACTOR panel[TITLE_PANEL_MAX];
  s8 cursor_pos;      ///<�J�[�\���ʒu
  s8 select_panel;    ///<�I�������p�l��(TITLE_PANEL_xxx)
  u8 mission_occ;     ///<TRUE:�~�b�V�������I���ł���
  u8 padding;
  GFL_CLWK *act_cancel;   ///<�L�����Z���A�C�R���A�N�^�[�ւ̃|�C���^
}MONOLITH_TITLE_WORK;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT MonolithTitleProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithTitleProc_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithTitleProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void _Title_PanelCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_TITLE_WORK *mtw);
static void _Title_PanelDelete(MONOLITH_TITLE_WORK *mtw);
static void _Title_PanelUpdate(MONOLITH_APP_PARENT *appwk, MONOLITH_TITLE_WORK *mtw);
static void _Title_CancelIconCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_TITLE_WORK *mtw);
static void _Title_CancelIconDelete(MONOLITH_TITLE_WORK *mtw);
static void _Title_CancelIconUpdate(MONOLITH_TITLE_WORK *mtw);


//==============================================================================
//  �f�[�^
//==============================================================================
///���m���X�p���[ROC�f�[�^
const GFL_PROC_DATA MonolithAppProc_Down_Title = {
  MonolithTitleProc_Init,
  MonolithTitleProc_Main,
  MonolithTitleProc_End,
};

//--------------------------------------------------------------
//  �p�l��
//--------------------------------------------------------------
///�p�l��Y���W
enum{
  TITLE_PANEL_Y_POWER_NOMISSION = 8*7 + 4,
  TITLE_PANEL_Y_STATUS_NOMISSION = 8*14 + 4,

  TITLE_PANEL_Y_MISSION = 8*5 + 4,
  TITLE_PANEL_Y_POWER = 8*10 + 4,
  TITLE_PANEL_Y_STATUS = 8*15 + 4,
};

///�p�l���̐�
static const u32 TitlePanelMax[] = {  // 0:�~�b�V�����Ȃ��@1:�~�b�V��������
  2,
  3,
};

///�p�l��Y���W�e�[�u��
static const u32 TitlePanelTblY[][3] = {
  {
    TITLE_PANEL_Y_POWER_NOMISSION,
    TITLE_PANEL_Y_STATUS_NOMISSION,
  },
  {
    TITLE_PANEL_Y_MISSION,
    TITLE_PANEL_Y_POWER,
    TITLE_PANEL_Y_STATUS,
  },
};

static const u32 TitlePanelSelectIndex[][4] = {
  {
    TITLE_PANEL_POWER,
    TITLE_PANEL_STATUS,
    TITLE_PANEL_CANCEL,
  },
  {
    TITLE_PANEL_MISSION,
    TITLE_PANEL_POWER,
    TITLE_PANEL_STATUS,
    TITLE_PANEL_CANCEL,
  },
};

///�p�l���^�b�`�f�[�^   ��TitlePanelSelectIndex�ƕ��т𓯂��ɂ��Ă������ƁI
static const GFL_UI_TP_HITTBL TitlePanelRect[][5] = {
  {
    {//TITLE_PANEL_Y_POWER
      TITLE_PANEL_Y_POWER_NOMISSION - PANEL_CHARSIZE_Y/2*8,
      TITLE_PANEL_Y_POWER_NOMISSION + PANEL_CHARSIZE_Y/2*8,
      PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
      PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
    },
    {//TITLE_PANEL_Y_STATUS
      TITLE_PANEL_Y_STATUS_NOMISSION - PANEL_CHARSIZE_Y/2*8,
      TITLE_PANEL_Y_STATUS_NOMISSION + PANEL_CHARSIZE_Y/2*8,
      PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
      PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
    },
    {//�L�����Z���A�C�R��
      CANCEL_POS_Y - CANCEL_TOUCH_RANGE_HALF,
      CANCEL_POS_Y + CANCEL_TOUCH_RANGE_HALF,
      CANCEL_POS_X - CANCEL_TOUCH_RANGE_HALF,
      CANCEL_POS_X + CANCEL_TOUCH_RANGE_HALF,
    },
    { GFL_UI_TP_HIT_END, 0, 0, 0 },
  },
  {
    {//TITLE_PANEL_Y_MISSION
      TITLE_PANEL_Y_MISSION - PANEL_CHARSIZE_Y/2*8,
      TITLE_PANEL_Y_MISSION + PANEL_CHARSIZE_Y/2*8,
      PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
      PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
    },
    {//TITLE_PANEL_Y_POWER
      TITLE_PANEL_Y_POWER - PANEL_CHARSIZE_Y/2*8,
      TITLE_PANEL_Y_POWER + PANEL_CHARSIZE_Y/2*8,
      PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
      PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
    },
    {//TITLE_PANEL_Y_STATUS
      TITLE_PANEL_Y_STATUS - PANEL_CHARSIZE_Y/2*8,
      TITLE_PANEL_Y_STATUS + PANEL_CHARSIZE_Y/2*8,
      PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
      PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
    },
    {//�L�����Z���A�C�R��
      CANCEL_POS_Y - CANCEL_TOUCH_RANGE_HALF,
      CANCEL_POS_Y + CANCEL_TOUCH_RANGE_HALF,
      CANCEL_POS_X - CANCEL_TOUCH_RANGE_HALF,
      CANCEL_POS_X + CANCEL_TOUCH_RANGE_HALF,
    },
    { GFL_UI_TP_HIT_END, 0, 0, 0 },
  },
};



//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT MonolithTitleProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_TITLE_WORK *mtw = mywk;
	ARCHANDLE *hdl;
  
  mtw = GFL_PROC_AllocWork(proc, sizeof(MONOLITH_TITLE_WORK), HEAPID_MONOLITH);
  GFL_STD_MemClear(mtw, sizeof(MONOLITH_TITLE_WORK));
  
  mtw->mission_occ = appwk->parent->list_occ;
  
  _Title_PanelCreate(appwk, mtw);
  _Title_CancelIconCreate(appwk, mtw);
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F���C��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT MonolithTitleProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_TITLE_WORK *mtw = mywk;
  int tp_ret;
  
  _Title_PanelUpdate(appwk, mtw);
  _Title_CancelIconUpdate(mtw);

  if(appwk->force_finish == TRUE){
    return GFL_PROC_RES_FINISH;
  }

  switch(*seq){
  case 0:
    tp_ret = GFL_UI_TP_HitTrg(TitlePanelRect[mtw->mission_occ]);
    if(tp_ret != GFL_UI_TP_HIT_NONE){
      OS_TPrintf("�^�b�`�L�� %d\n", tp_ret);
      mtw->cursor_pos = tp_ret;
      mtw->select_panel = TitlePanelSelectIndex[mtw->mission_occ][tp_ret];
      (*seq)++;
      break;
    }
    else{
      int repeat = GFL_UI_KEY_GetRepeat();
      int trg = GFL_UI_KEY_GetTrg();
      
      if(trg & PAD_BUTTON_DECIDE){
        mtw->select_panel = TitlePanelSelectIndex[mtw->mission_occ][mtw->cursor_pos];
        (*seq)++;
      }
      else if(trg & PAD_BUTTON_CANCEL){
        mtw->select_panel = TITLE_PANEL_CANCEL;
        (*seq)++;
      }
      else if(repeat & PAD_KEY_DOWN){
        mtw->cursor_pos++;
        if(mtw->cursor_pos >= TitlePanelMax[mtw->mission_occ]){
          mtw->cursor_pos = 0;
        }
        MonolithTool_PanelOBJ_Focus(appwk, mtw->panel, 
          TitlePanelMax[mtw->mission_occ], mtw->cursor_pos, FADE_SUB_OBJ);
      }
      else if(repeat & PAD_KEY_UP){
        mtw->cursor_pos--;
        if(mtw->cursor_pos < 0){
          mtw->cursor_pos = TitlePanelMax[mtw->mission_occ] - 1;
        }
        MonolithTool_PanelOBJ_Focus(
          appwk, mtw->panel, TitlePanelMax[mtw->mission_occ], 
          mtw->cursor_pos, FADE_SUB_OBJ);
      }
    }
    break;
  case 1:
    if(mtw->select_panel == TITLE_PANEL_MISSION && appwk->parent->list_occ == FALSE){
      OS_TPrintf("�~�b�V�����I����⃊�X�g������M\n");
      (*seq)--;
      break;
    }
    if(mtw->select_panel != TITLE_PANEL_CANCEL){
      MonolithTool_PanelOBJ_Flash(appwk, mtw->panel, 
        TitlePanelMax[mtw->mission_occ], mtw->cursor_pos, FADE_SUB_OBJ);
    }
    else{
      //��check�@�L�����Z���A�C�R�������炷
    }
    (*seq)++;
    break;
  case 2:
    if(mtw->select_panel != TITLE_PANEL_CANCEL){
      if(MonolithTool_PanelColor_GetMode(appwk, FADE_SUB_OBJ) != PANEL_COLORMODE_FLASH){
        appwk->next_menu_index = MONOLITH_MENU_MISSION + mtw->select_panel;
        return GFL_PROC_RES_FINISH;
      }
    }
    else{
      //��check�@�L�����Z���A�C�R���̌��҂�
      appwk->next_menu_index = MONOLITH_MENU_MISSION + mtw->select_panel;
      return GFL_PROC_RES_FINISH;
    }
    break;
  }
  
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F�I��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT MonolithTitleProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_TITLE_WORK *mtw = mywk;

  _Title_PanelDelete(mtw);
  _Title_CancelIconDelete(mtw);
  
  GFL_PROC_FreeWork(proc);
  return GFL_PROC_RES_FINISH;
}


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �p�l������
 *
 * @param   appwk		
 * @param   mtw		
 */
//==================================================================
static void _Title_PanelCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_TITLE_WORK *mtw)
{
  int i;
  
  MonolithTool_Panel_Init(appwk);

  for(i = 0; i < TitlePanelMax[mtw->mission_occ]; i++){
    if(mtw->mission_occ == TRUE){
      MonolithTool_PanelOBJ_Create(appwk->setup, &mtw->panel[i], 
        COMMON_RESOURCE_INDEX_DOWN, PANEL_SIZE_SMALL, 
        PANEL_POS_X, TitlePanelTblY[mtw->mission_occ][i], 
        msg_mono_title_001 + i, NULL);
    }
    else{
      MonolithTool_PanelOBJ_Create(appwk->setup, &mtw->panel[i], 
        COMMON_RESOURCE_INDEX_DOWN, PANEL_SIZE_SMALL, 
        PANEL_POS_X, TitlePanelTblY[mtw->mission_occ][i], 
        msg_mono_title_002 + i, NULL);
    }
  }
  
  MonolithTool_PanelOBJ_Focus(appwk, mtw->panel, TitlePanelMax[mtw->mission_occ], 
    0, FADE_SUB_OBJ);
}

//==================================================================
/**
 * �p�l���폜
 *
 * @param   mtw		
 */
//==================================================================
static void _Title_PanelDelete(MONOLITH_TITLE_WORK *mtw)
{
  int i;
  
  for(i = 0; i < TitlePanelMax[mtw->mission_occ]; i++){
    MonolithTool_PanelOBJ_Delete(&mtw->panel[i]);
  }
}

//==================================================================
/**
 * �p�l���X�V����
 *
 * @param   mtw		
 */
//==================================================================
static void _Title_PanelUpdate(MONOLITH_APP_PARENT *appwk, MONOLITH_TITLE_WORK *mtw)
{
  int i;
  
  MonolithTool_Panel_ColorUpdate(appwk, FADE_SUB_OBJ);
  for(i = 0; i < TitlePanelMax[mtw->mission_occ]; i++){
    MonolithTool_PanelOBJ_TransUpdate(appwk->setup, &mtw->panel[i]);
  }
}

//==================================================================
/**
 * �L�����Z���A�C�R������
 *
 * @param   appwk		
 * @param   mtw		
 */
//==================================================================
static void _Title_CancelIconCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_TITLE_WORK *mtw)
{
  mtw->act_cancel = MonolithTool_CancelIcon_Create(appwk->setup);
}

//==================================================================
/**
 * �L�����Z���A�C�R���폜
 *
 * @param   mtw		
 */
//==================================================================
static void _Title_CancelIconDelete(MONOLITH_TITLE_WORK *mtw)
{
  MonolithTool_CancelIcon_Delete(mtw->act_cancel);
}

//==================================================================
/**
 * �L�����Z���A�C�R���X�V����
 *
 * @param   mtw		
 */
//==================================================================
static void _Title_CancelIconUpdate(MONOLITH_TITLE_WORK *mtw)
{
  MonolithTool_CancelIcon_Update(mtw->act_cancel);
}
