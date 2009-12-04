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
  BMPWIN_POS_POWER_BALANCE_SIZE_Y = 5,
  
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
  u8 view_town;                         ///<�~�b�V����������\�����̊X�ԍ�
  u8 padding[3];
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
static void _Write_Status(MONOLITH_APP_PARENT *appwk, MONOLITH_SETUP *setup, MONOLITH_STATUS_WORK *msw, int select_town);


//==============================================================================
//  �f�[�^
//==============================================================================
///���m���X�p���[ROC�f�[�^
const GFL_PROC_DATA MonolithAppProc_Down_Status = {
  MonolithStatusProc_Init,
  MonolithStatusProc_Main,
  MonolithStatusProc_End,
};

///�X�ԍ�����~�b�V�����^�C�v���擾����e�[�u��
static const u32 TownNo_to_Type[] = {
  MISSION_TYPE_PERSONALITY, ///<���i
  MISSION_TYPE_VICTORY,     ///<����(LV)
  MISSION_TYPE_OCCUR,       ///<����(�G���J�E���g)
  MISSION_TYPE_SKILL,       ///<�Z
  MISSION_TYPE_SIZE,        ///<�傫��
  MISSION_TYPE_BASIC,       ///<��b
  MISSION_TYPE_ITEM,        ///<����
  MISSION_TYPE_ATTRIBUTE,   ///<����
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
  
  msw->view_town = 0xff;  //����̕`���ʂ��ׂɑ��݂��Ȃ��X�ԍ���ݒ�
  
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
  int print_finish_count = 0;

  if(appwk->up_proc_finish == TRUE || appwk->force_finish == TRUE){
    return GFL_PROC_RES_FINISH;
  }
  
  for(i = 0; i < BMPWIN_MAX; i++){
    if(PRINT_UTIL_Trans(&msw->print_util[i], appwk->setup->printQue) == TRUE){
      print_finish_count++;
    }
  }
  if(print_finish_count == BMPWIN_MAX){
    if(msw->view_town != appwk->common->mission_select_town){
      _Write_Status(appwk, appwk->setup, msw, appwk->common->mission_select_town);
      msw->view_town = appwk->common->mission_select_town;
    }
  }
  
  switch(*seq){
  case 0:
    return GFL_PROC_RES_CONTINUE;
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
 * �~�b�V���������`��
 *
 * @param   appwk		
 * @param   setup		
 * @param   msw		
 * @param   select_town		
 */
//--------------------------------------------------------------
static void _Write_Status(MONOLITH_APP_PARENT *appwk, MONOLITH_SETUP *setup, MONOLITH_STATUS_WORK *msw, int select_town)
{
  STRBUF *str_type, *str_explain, *expand_explain;
  u32 explain_msgid;
  const MISSION_DATA *mdata;
  
  expand_explain = GFL_STR_CreateBuffer( 256, HEAPID_MONOLITH );

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(msw->bmpwin[BMPWIN_TYPE]), 0x0000);
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(msw->bmpwin[BMPWIN_EXPLAIN]), 0x0000);

  mdata = &appwk->parent->list.md[TownNo_to_Type[select_town]];
  explain_msgid = mdata->cdata.msg_id_contents_monolith;
  
  str_type = GFL_MSG_CreateString(setup->mm_monolith, 
    msg_mono_mistype_000 + TownNo_to_Type[select_town]);
  str_explain = GFL_MSG_CreateString(setup->mm_mission_mono, explain_msgid);
  
  MISSIONDATA_Wordset(appwk->parent->intcomm, mdata, setup->wordset, HEAPID_MONOLITH);
  WORDSET_ExpandStr(setup->wordset, expand_explain, str_explain );

  PRINT_UTIL_Print(&msw->print_util[BMPWIN_TYPE], setup->printQue, 
    0, 4, str_type, setup->font_handle);
  PRINT_UTIL_Print(&msw->print_util[BMPWIN_EXPLAIN], setup->printQue, 
    0, 0, expand_explain, setup->font_handle);
  
  GFL_STR_DeleteBuffer(str_type);
  GFL_STR_DeleteBuffer(str_explain);
  GFL_STR_DeleteBuffer(expand_explain);
}

