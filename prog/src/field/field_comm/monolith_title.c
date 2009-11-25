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


//==============================================================================
//  �萔��`
//==============================================================================
///�p�l����
#define TITLE_PANEL_MAX     (5)

//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  PANEL_ACTOR panel[TITLE_PANEL_MAX];
  u32 work;
}MONOLITH_TITLE_WORK;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT MonolithTitleProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithTitleProc_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithTitleProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void _Title_PanelCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_TITLE_WORK *mtw);
static void _Title_PanelDelete(MONOLITH_TITLE_WORK *mtw);
static void _Title_PanelUpdate(MONOLITH_TITLE_WORK *mtw);


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
  TITLE_PANEL_Y_MISSION = 8*2 + 4,
  TITLE_PANEL_Y_STATUS = 8*6 + 4,
  TITLE_PANEL_Y_RECORD = 8*10 + 4,
  TITLE_PANEL_Y_POWER = 8*14 + 4,
  TITLE_PANEL_Y_CANCEL = 8*21 + 4,
};

///�p�l��Y���W�e�[�u��
static const u32 TitlePanelTblY[] = {
  TITLE_PANEL_Y_MISSION,
  TITLE_PANEL_Y_STATUS,
  TITLE_PANEL_Y_RECORD,
  TITLE_PANEL_Y_POWER,
  TITLE_PANEL_Y_CANCEL,
};

///�p�l���^�b�`�f�[�^
static const GFL_UI_TP_HITTBL TitlePanelRect[] = {
  {//TITLE_PANEL_Y_MISSION
    TITLE_PANEL_Y_MISSION - PANEL_CHARSIZE_Y/2*8,
    TITLE_PANEL_Y_MISSION + PANEL_CHARSIZE_Y/2*8,
    PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
    PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
  },
  {//TITLE_PANEL_Y_STATUS
    TITLE_PANEL_Y_STATUS - PANEL_CHARSIZE_Y/2*8,
    TITLE_PANEL_Y_STATUS + PANEL_CHARSIZE_Y/2*8,
    PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
    PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
  },
  {//TITLE_PANEL_Y_RECORD
    TITLE_PANEL_Y_RECORD - PANEL_CHARSIZE_Y/2*8,
    TITLE_PANEL_Y_RECORD + PANEL_CHARSIZE_Y/2*8,
    PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
    PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
  },
  {//TITLE_PANEL_Y_POWER
    TITLE_PANEL_Y_POWER - PANEL_CHARSIZE_Y/2*8,
    TITLE_PANEL_Y_POWER + PANEL_CHARSIZE_Y/2*8,
    PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
    PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
  },
  {//TITLE_PANEL_Y_CANCEL
    TITLE_PANEL_Y_CANCEL - PANEL_CHARSIZE_Y/2*8,
    TITLE_PANEL_Y_CANCEL + PANEL_CHARSIZE_Y/2*8,
    PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
    PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
  },
  { GFL_UI_TP_HIT_END, 0, 0, 0 },
};
SDK_ASSERT(TITLE_PANEL_MAX == NELEMS(TitlePanelTblY) && TITLE_PANEL_MAX == NELEMS(TitlePanelRect));


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
  
  _Title_PanelCreate(appwk, mtw);

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

  _Title_PanelUpdate(mtw);

  switch(*seq){
  case 0:
    return GFL_PROC_RES_FINISH;
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
  
  for(i = 0; i < PANEL_MAX; i++){
    MonolithTool_Panel_Create(appwk->setup, &mtw->panel[i], 
      COMMON_RESOURCE_INDEX_DOWN, PANEL_SIZE_SMALL, TitlePanelTblY[i], msg_mono_title_001 + i);
  }
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
  
  for(i = 0; i < PANEL_MAX; i++){
    MonolithTool_Panel_Delete(&mtw->panel[i]);
  }
}

//==================================================================
/**
 * �p�l���X�V����
 *
 * @param   mtw		
 */
//==================================================================
static void _Title_PanelUpdate(MONOLITH_TITLE_WORK *mtw)
{
  int i;
  
  for(i = 0; i < PANEL_MAX; i++){
    MonolithTool_Panel_Update(&mtw->panel[i]);
  }
}
