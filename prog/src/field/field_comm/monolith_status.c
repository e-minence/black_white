//==============================================================================
/**
 * @file    monolith_status.c
 * @brief   ���m���X�F��Ԃ�����
 * @author  matsuda
 * @date    2009.12.04(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "monolith_common.h"
#include "arc_def.h"
#include "monolith_tool.h"
#include "msg/msg_monolith.h"
#include "mission_types.h"
#include "intrude_mission.h"
#include "intrude_work.h"

#include "monolith.naix"


//==============================================================================
//  �萔��`
//==============================================================================
///BMPWIN
enum{
  BMPWIN_LEVEL,           ///<���m���X���x��
  BMPWIN_NICKNAME,        ///<���m���X�̒ʏ�
  BMPWIN_POWER_BALANCE,   ///<�p���[�o�����X
  BMPWIN_USE_POWER,       ///<�g����p���[�̐�
  BMPWIN_TIME,            ///<�؍ݎ���
  
  BMPWIN_MAX,
};

///BMPWIN�̃T�C�Y��`�Ȃ�
enum{
  BMPWIN_POS_LEVEL_X = 2,
  BMPWIN_POS_LEVEL_Y = 4,
  BMPWIN_POS_LEVEL_SIZE_X = 32 - BMPWIN_POS_LEVEL_X,
  BMPWIN_POS_LEVEL_SIZE_Y = 2,
  
  BMPWIN_POS_NICKNAME_X = 2,
  BMPWIN_POS_NICKNAME_Y = BMPWIN_POS_LEVEL_Y + 2,
  BMPWIN_POS_NICKNAME_SIZE_X = 32 - BMPWIN_POS_NICKNAME_X,
  BMPWIN_POS_NICKNAME_SIZE_Y = 2,
  
  BMPWIN_POS_POWER_BALANCE_X = 4,
  BMPWIN_POS_POWER_BALANCE_Y = BMPWIN_POS_NICKNAME_Y + 4,
  BMPWIN_POS_POWER_BALANCE_SIZE_X = 32 - BMPWIN_POS_POWER_BALANCE_X,
  BMPWIN_POS_POWER_BALANCE_SIZE_Y = 4,
  
  BMPWIN_POS_USE_POWER_X = BMPWIN_POS_POWER_BALANCE_X,
  BMPWIN_POS_USE_POWER_Y = 15,
  BMPWIN_POS_USE_POWER_SIZE_X = 32 - BMPWIN_POS_USE_POWER_X,
  BMPWIN_POS_USE_POWER_SIZE_Y = 2,
  
  BMPWIN_POS_TIME_X = BMPWIN_POS_POWER_BALANCE_X,
  BMPWIN_POS_TIME_Y = BMPWIN_POS_USE_POWER_Y + 2,
  BMPWIN_POS_TIME_SIZE_X = 32 - BMPWIN_POS_TIME_X,
  BMPWIN_POS_TIME_SIZE_Y = 2,
};

//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  GFL_BMPWIN *bmpwin[BMPWIN_MAX];
  PRINT_UTIL print_util[BMPWIN_MAX];
  PANEL_ACTOR panel_title;
  GFL_CLWK *act_cancel;
}MONOLITH_STATUS_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT MonolithStatusProc_Init(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithStatusProc_Main(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithStatusProc_End(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void _Setup_BGFrameSetting(void);
static void _Setup_BGFrameExit(void);
static void _Setup_BGGraphicLoad(MONOLITH_SETUP *setup);
static void _Setup_BmpWin_Create(MONOLITH_SETUP *setup, MONOLITH_STATUS_WORK *msw);
static void _Setup_BmpWin_Exit(MONOLITH_STATUS_WORK *msw);
static void _Setup_TitlePanel_Create(MONOLITH_APP_PARENT *appwk, MONOLITH_STATUS_WORK *msw);
static void _Setup_TitlePanel_Delete(MONOLITH_STATUS_WORK *msw);
static void _Write_Status(MONOLITH_APP_PARENT *appwk, MONOLITH_SETUP *setup, MONOLITH_STATUS_WORK *msw);
static void _Status_CancelIconCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_STATUS_WORK *msw);
static void _Status_CancelIconDelete(MONOLITH_STATUS_WORK *msw);
static void _Status_CancelIconUpdate(MONOLITH_STATUS_WORK *msw);


//==============================================================================
//  �f�[�^
//==============================================================================
///���m���X�p���[ROC�f�[�^
const GFL_PROC_DATA MonolithAppProc_Down_Status = {
  MonolithStatusProc_Init,
  MonolithStatusProc_Main,
  MonolithStatusProc_End,
};

///�^�b�`�͈̓e�[�u��
static const GFL_UI_TP_HITTBL StatusTouchRect[] = {
  {
    CANCEL_POS_Y - CANCEL_TOUCH_RANGE_HALF,
    CANCEL_POS_Y + CANCEL_TOUCH_RANGE_HALF,
    CANCEL_POS_X - CANCEL_TOUCH_RANGE_HALF,
    CANCEL_POS_X + CANCEL_TOUCH_RANGE_HALF,
  },
  { GFL_UI_TP_HIT_END, 0, 0, 0 },
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
static GFL_PROC_RESULT MonolithStatusProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_STATUS_WORK *msw = mywk;
	ARCHANDLE *hdl;
  
  msw = GFL_PROC_AllocWork(proc, sizeof(MONOLITH_STATUS_WORK), HEAPID_MONOLITH);
  GFL_STD_MemClear(msw, sizeof(MONOLITH_STATUS_WORK));
  
  //BG
  _Setup_BGFrameSetting();
  _Setup_BGGraphicLoad(appwk->setup);
  _Setup_BmpWin_Create(appwk->setup, msw);
  //OBJ
  _Setup_TitlePanel_Create(appwk, msw);
  _Status_CancelIconCreate(appwk, msw);
  
  //���b�Z�[�W�`��
  _Write_Status(appwk, appwk->setup, msw);
  
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
static GFL_PROC_RESULT MonolithStatusProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_STATUS_WORK *msw = mywk;
  int tp_ret;
  int i;

  if(appwk->up_proc_finish == TRUE || appwk->force_finish == TRUE){
    return GFL_PROC_RES_FINISH;
  }
  
  for(i = 0; i < BMPWIN_MAX; i++){
    PRINT_UTIL_Trans(&msw->print_util[i], appwk->setup->printQue);
  }
  MonolithTool_Panel_TransUpdate(appwk->setup, &msw->panel_title);
  _Status_CancelIconUpdate(msw);
  
  switch(*seq){
  case 0:
    {
      int trg = GFL_UI_KEY_GetTrg();
      int tp_ret = GFL_UI_TP_HitTrg(StatusTouchRect);

      if(tp_ret != GFL_UI_TP_HIT_NONE || (trg & PAD_BUTTON_CANCEL)){
        OS_TPrintf("�L�����Z���I��\n");
        (*seq)++;
      }
    }
    break;
  case 1:
    appwk->next_menu_index = MONOLITH_MENU_TITLE;
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
static GFL_PROC_RESULT MonolithStatusProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_STATUS_WORK *msw = mywk;
  
  //OBJ
  _Status_CancelIconDelete(msw);
  _Setup_TitlePanel_Delete(msw);
  
  //BG
  _Setup_BmpWin_Exit(msw);
  _Setup_BGFrameExit();
  
  GFL_PROC_FreeWork(proc);
  return GFL_PROC_RES_FINISH;
}


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 */
//--------------------------------------------------------------
static void _Setup_BGFrameSetting(void)
{
	static const GFL_BG_BGCNT_HEADER bgcnt_frame[] = {
		{//GFL_BG_FRAME0_S
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( GFL_BG_FRAME0_S,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );

	GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_SetVisible(GFL_BG_FRAME0_S, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * BG�t���[�����
 */
//--------------------------------------------------------------
static void _Setup_BGFrameExit(void)
{
	GFL_BG_SetVisible(GFL_BG_FRAME0_S, VISIBLE_OFF);
  GFL_BG_FreeBGControl(GFL_BG_FRAME0_S);
}

//--------------------------------------------------------------
/**
 * @brief   BG�O���t�B�b�N��VRAM�֓]��
 *
 * @param   hdl		�A�[�J�C�u�n���h��
 */
//--------------------------------------------------------------
static void _Setup_BGGraphicLoad(MONOLITH_SETUP *setup)
{
	GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
}

//--------------------------------------------------------------
/**
 * BMPWIN�쐬
 *
 * @param   setup		
 * @param   msw		
 */
//--------------------------------------------------------------
static void _Setup_BmpWin_Create(MONOLITH_SETUP *setup, MONOLITH_STATUS_WORK *msw)
{
  int i;
  
  //���m���X���x��
  msw->bmpwin[BMPWIN_LEVEL] = GFL_BMPWIN_Create(GFL_BG_FRAME0_S, 
    BMPWIN_POS_LEVEL_X, BMPWIN_POS_LEVEL_Y, 
    BMPWIN_POS_LEVEL_SIZE_X, BMPWIN_POS_LEVEL_SIZE_Y, 
    MONOLITH_BG_DOWN_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
  GFL_BMPWIN_TransVramCharacter( msw->bmpwin[BMPWIN_LEVEL] );
  GFL_BMPWIN_MakeScreen( msw->bmpwin[BMPWIN_LEVEL] );

  //���m���X�̒ʏ�
  msw->bmpwin[BMPWIN_NICKNAME] = GFL_BMPWIN_Create(GFL_BG_FRAME0_S, 
    BMPWIN_POS_NICKNAME_X, BMPWIN_POS_NICKNAME_Y, 
    BMPWIN_POS_NICKNAME_SIZE_X, BMPWIN_POS_NICKNAME_SIZE_Y, 
    MONOLITH_BG_DOWN_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
  GFL_BMPWIN_TransVramCharacter( msw->bmpwin[BMPWIN_NICKNAME] );
  GFL_BMPWIN_MakeScreen( msw->bmpwin[BMPWIN_NICKNAME] );

  //�p���[�o�����X
  msw->bmpwin[BMPWIN_POWER_BALANCE] = GFL_BMPWIN_Create(GFL_BG_FRAME0_S, 
    BMPWIN_POS_POWER_BALANCE_X, BMPWIN_POS_POWER_BALANCE_Y, 
    BMPWIN_POS_POWER_BALANCE_SIZE_X, BMPWIN_POS_POWER_BALANCE_SIZE_Y, 
    MONOLITH_BG_DOWN_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
  GFL_BMPWIN_TransVramCharacter( msw->bmpwin[BMPWIN_POWER_BALANCE] );
  GFL_BMPWIN_MakeScreen( msw->bmpwin[BMPWIN_POWER_BALANCE] );

  //�g����p���[�̐�
  msw->bmpwin[BMPWIN_USE_POWER] = GFL_BMPWIN_Create(GFL_BG_FRAME0_S, 
    BMPWIN_POS_USE_POWER_X, BMPWIN_POS_USE_POWER_Y, 
    BMPWIN_POS_USE_POWER_SIZE_X, BMPWIN_POS_USE_POWER_SIZE_Y, 
    MONOLITH_BG_DOWN_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
  GFL_BMPWIN_TransVramCharacter( msw->bmpwin[BMPWIN_USE_POWER] );
  GFL_BMPWIN_MakeScreen( msw->bmpwin[BMPWIN_USE_POWER] );

  //�؍ݎ���
  msw->bmpwin[BMPWIN_TIME] = GFL_BMPWIN_Create(GFL_BG_FRAME0_S, 
    BMPWIN_POS_TIME_X, BMPWIN_POS_TIME_Y, 
    BMPWIN_POS_TIME_SIZE_X, BMPWIN_POS_TIME_SIZE_Y, 
    MONOLITH_BG_DOWN_FONT_PALNO, GFL_BMP_CHRAREA_GET_B);
  GFL_BMPWIN_TransVramCharacter( msw->bmpwin[BMPWIN_TIME] );
  GFL_BMPWIN_MakeScreen( msw->bmpwin[BMPWIN_TIME] );

  for(i = 0; i < BMPWIN_MAX; i++){
    PRINT_UTIL_Setup(&msw->print_util[i], msw->bmpwin[i]);
  }
  
	GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
}

//--------------------------------------------------------------
/**
 * BMPWIN�폜
 *
 * @param   msw		
 */
//--------------------------------------------------------------
static void _Setup_BmpWin_Exit(MONOLITH_STATUS_WORK *msw)
{
  int i;
  
  for(i = 0; i < BMPWIN_MAX; i++){
    GFL_BMPWIN_Delete(msw->bmpwin[i]);
  }
}

//--------------------------------------------------------------
/**
 * �^�C�g���p�l���쐬
 *
 * @param   appwk		
 * @param   msw		
 */
//--------------------------------------------------------------
static void _Setup_TitlePanel_Create(MONOLITH_APP_PARENT *appwk, MONOLITH_STATUS_WORK *msw)
{
  MYSTATUS *myst = Intrude_GetMyStatus(appwk->parent->intcomm, appwk->parent->palace_area);

  STRBUF *strbuf = 	GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, HEAPID_MONOLITH);

	GFL_STR_SetStringCodeOrderLength(
	  strbuf, MyStatus_GetMyName(myst), PERSON_NAME_SIZE + EOM_SIZE);
  WORDSET_RegisterWord(appwk->setup->wordset, 0, strbuf, MyStatus_GetMySex(myst), TRUE, PM_LANG);

  GFL_STR_DeleteBuffer(strbuf);

  MonolithTool_Panel_Create(appwk->setup, &msw->panel_title, 
    COMMON_RESOURCE_INDEX_DOWN, PANEL_SIZE_SMALL, 12, msg_mono_title_000, appwk->setup->wordset);
}

//--------------------------------------------------------------
/**
 * �^�C�g���p�l���폜
 *
 * @param   msw		
 */
//--------------------------------------------------------------
static void _Setup_TitlePanel_Delete(MONOLITH_STATUS_WORK *msw)
{
  MonolithTool_Panel_Delete(&msw->panel_title);
}

//--------------------------------------------------------------
/**
 * �~�b�V���������`��
 *
 * @param   appwk		
 * @param   setup		
 * @param   msw		
 * @param   select_town		
 */
//--------------------------------------------------------------
static void _Write_Status(MONOLITH_APP_PARENT *appwk, MONOLITH_SETUP *setup, MONOLITH_STATUS_WORK *msw)
{
  STRBUF *strbuf, *expand_strbuf;
  const OCCUPY_INFO *occupy;
  
  occupy = Intrude_GetOccupyInfo(appwk->parent->intcomm, appwk->parent->palace_area);
  
  strbuf = GFL_STR_CreateBuffer(256, HEAPID_MONOLITH);
  expand_strbuf = GFL_STR_CreateBuffer( 256, HEAPID_MONOLITH );

  //���m���X���x��
  GFL_MSG_GetString(setup->mm_monolith, msg_mono_st_000, strbuf);
  WORDSET_RegisterNumber(setup->wordset, 0, occupy->intrude_level + 1, 3, 
    STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT); // level + 1 = �\����1origin��
  WORDSET_ExpandStr( setup->wordset, expand_strbuf, strbuf );
  PRINT_UTIL_Print(&msw->print_util[BMPWIN_LEVEL], setup->printQue, 
    0, 0, expand_strbuf, setup->font_handle);
  
  //�ʏ�
  GFL_MSG_GetString(setup->mm_monolith, msg_mono_st_001, strbuf);
  PRINT_UTIL_Print(&msw->print_util[BMPWIN_NICKNAME], setup->printQue, 
    0, 0, strbuf, setup->font_handle);
  
  //�p���[�o�����X
  GFL_MSG_GetString(setup->mm_monolith, msg_mono_st_002, strbuf);
  PRINT_UTIL_Print(&msw->print_util[BMPWIN_POWER_BALANCE], setup->printQue, 
    0, 8, strbuf, setup->font_handle);
  //�p���[�o�����X�FWHITE
  GFL_MSG_GetString(setup->mm_monolith, msg_mono_st_003, strbuf);
  WORDSET_RegisterNumber(setup->wordset, 0, 560, 4, 
    STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( setup->wordset, expand_strbuf, strbuf );
  PRINT_UTIL_Print(&msw->print_util[BMPWIN_POWER_BALANCE], setup->printQue, 
    14*8, 0, expand_strbuf, setup->font_handle);
  //�p���[�o�����X�FBLACK
  GFL_MSG_GetString(setup->mm_monolith, msg_mono_st_004, strbuf);
  WORDSET_RegisterNumber(setup->wordset, 0, 70, 4, 
    STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( setup->wordset, expand_strbuf, strbuf );
  PRINT_UTIL_Print(&msw->print_util[BMPWIN_POWER_BALANCE], setup->printQue, 
    14*8, 16, expand_strbuf, setup->font_handle);
  
  //�g����p���[�̐�
  GFL_MSG_GetString(setup->mm_monolith, msg_mono_st_005, strbuf);
  WORDSET_RegisterNumber(setup->wordset, 0, 99, 3, 
    STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( setup->wordset, expand_strbuf, strbuf );
  PRINT_UTIL_Print(&msw->print_util[BMPWIN_USE_POWER], setup->printQue, 
    0, 0, expand_strbuf, setup->font_handle);
  
  //�؍ݎ���
  GFL_MSG_GetString(setup->mm_monolith, msg_mono_st_006, strbuf);
  WORDSET_RegisterNumber(setup->wordset, 0, 99, 3, 
    STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_RegisterNumber(setup->wordset, 1, 99, 2, 
    STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( setup->wordset, expand_strbuf, strbuf );
  PRINT_UTIL_Print(&msw->print_util[BMPWIN_TIME], setup->printQue, 
    0, 0, expand_strbuf, setup->font_handle);
  
  GFL_STR_DeleteBuffer(strbuf);
  GFL_STR_DeleteBuffer(expand_strbuf);
}

//==================================================================
/**
 * �L�����Z���A�C�R������
 *
 * @param   appwk		
 * @param   msw		
 */
//==================================================================
static void _Status_CancelIconCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_STATUS_WORK *msw)
{
  msw->act_cancel = MonolithTool_CancelIcon_Create(appwk->setup);
}

//==================================================================
/**
 * �L�����Z���A�C�R���폜
 *
 * @param   msw		
 */
//==================================================================
static void _Status_CancelIconDelete(MONOLITH_STATUS_WORK *msw)
{
  MonolithTool_CancelIcon_Delete(msw->act_cancel);
}

//==================================================================
/**
 * �L�����Z���A�C�R���X�V����
 *
 * @param   msw		
 */
//==================================================================
static void _Status_CancelIconUpdate(MONOLITH_STATUS_WORK *msw)
{
  MonolithTool_CancelIcon_Update(msw->act_cancel);
}
