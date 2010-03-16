//==============================================================================
/**
 * @file    monolith_palacemap.c
 * @brief   ���m���X�F�p���X�}�b�v
 * @author  matsuda
 * @date    2009.11.25(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "monolith_common.h"
#include "arc_def.h"
#include "monolith_tool.h"
#include "msg/msg_monolith.h"
#include "intrude_work.h"
#include "monolith.naix"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  MONOLITH_BMPSTR bmpstr_title;
  GFL_CLWK *act_town[INTRUDE_TOWN_MAX]; ///<�X�A�C�R���A�N�^�[�ւ̃|�C���^
}MONOLITH_PALACEMAP_WORK;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT MonolithPalaceMapProc_Init(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithPalaceMapProc_Main(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithPalaceMapProc_End(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void _Setup_BGFrameSetting(void);
static void _Setup_BGFrameExit(void);
static void _Setup_BGGraphicLoad(MONOLITH_SETUP *setup);
static void _TownIcon_AllCreate(MONOLITH_PALACEMAP_WORK *mpw, MONOLITH_APP_PARENT *appwk);
static void _TownIcon_AllDelete(MONOLITH_PALACEMAP_WORK *mpw);
static void _TownIcon_AllUpdate(MONOLITH_PALACEMAP_WORK *mpw, MONOLITH_APP_PARENT *appwk);


//==============================================================================
//  �f�[�^
//==============================================================================
///���m���X�p���[ROC�f�[�^
const GFL_PROC_DATA MonolithAppProc_Up_PalaceMap = {
  MonolithPalaceMapProc_Init,
  MonolithPalaceMapProc_Main,
  MonolithPalaceMapProc_End,
};

//--------------------------------------------------------------
//  �X�A�C�R��
//--------------------------------------------------------------
///�X�A�C�R���\�����W�e�[�u��
static const GFL_POINT TownIconPosTbl[] = {
  {11*8, 6*8},
  {16*8, 6*8},
  {21*8, 6*8},
  
  {8*8, 10*8},
  {0x18*8, 10*8},
  
  {11*8, 14*8},
  {16*8, 14*8},
  {21*8, 14*8},
};
SDK_COMPILER_ASSERT(NELEMS(TownIconPosTbl) == INTRUDE_TOWN_MAX);




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
static GFL_PROC_RESULT MonolithPalaceMapProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_PALACEMAP_WORK *mpw = mywk;
	ARCHANDLE *hdl;
  
  mpw = GFL_PROC_AllocWork(proc, sizeof(MONOLITH_PALACEMAP_WORK), HEAPID_MONOLITH);
  GFL_STD_MemClear(mpw, sizeof(MONOLITH_PALACEMAP_WORK));
  
  //BG
  _Setup_BGFrameSetting();
  _Setup_BGGraphicLoad(appwk->setup);
  //OBJ
  {
    MYSTATUS *myst = MonolithTool_GetMystatus(appwk);

    STRBUF *strbuf = 	GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, HEAPID_MONOLITH);

  	GFL_STR_SetStringCodeOrderLength(
  	  strbuf, MyStatus_GetMyName(myst), PERSON_NAME_SIZE + EOM_SIZE);
    WORDSET_RegisterWord(appwk->setup->wordset, 0, strbuf, MyStatus_GetMySex(myst), TRUE, PM_LANG);

    GFL_STR_DeleteBuffer(strbuf);
  }
  MonolithTool_Bmpoam_Create(appwk->setup, &mpw->bmpstr_title, COMMON_RESOURCE_INDEX_UP, 
    128, 12, 30, 2, msg_mono_title_000, appwk->setup->wordset);
  _TownIcon_AllCreate(mpw, appwk);
  
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
static GFL_PROC_RESULT MonolithPalaceMapProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_PALACEMAP_WORK *mpw = mywk;
  int tp_ret;
  
  if(appwk->up_proc_finish == TRUE || appwk->force_finish == TRUE){
    return GFL_PROC_RES_FINISH;
  }
  
  MonolithTool_Bmpoam_TransUpdate(appwk->setup, &mpw->bmpstr_title);
  _TownIcon_AllUpdate(mpw, appwk);

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
static GFL_PROC_RESULT MonolithPalaceMapProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_PALACEMAP_WORK *mpw = mywk;
  
  //OBJ
  _TownIcon_AllDelete(mpw);
  MonolithTool_Bmpoam_Delete(&mpw->bmpstr_title);
  //BG
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
		{//GFL_BG_FRAME2_M
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, MONO_BG_COMMON_CHARBASE, MONO_BG_COMMON_CHAR_SIZE,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( GFL_BG_FRAME2_M,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );

	GFL_BG_FillScreen( GFL_BG_FRAME2_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_SetVisible(GFL_BG_FRAME2_M, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * BG�t���[�����
 */
//--------------------------------------------------------------
static void _Setup_BGFrameExit(void)
{
	GFL_BG_SetVisible(GFL_BG_FRAME2_M, VISIBLE_OFF);
  GFL_BG_FreeBGControl(GFL_BG_FRAME2_M);
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
	GFL_ARCHDL_UTIL_TransVramScreen(setup->hdl, NARC_monolith_mono_bgu_map_lz_NSCR, 
		GFL_BG_FRAME2_M, 0, 0, TRUE, HEAPID_MONOLITH);

	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
}

//--------------------------------------------------------------
/**
 * @brief   �X�A�C�R���A�N�^�[����
 */
//--------------------------------------------------------------
static void _TownIcon_AllCreate(MONOLITH_PALACEMAP_WORK *mpw, MONOLITH_APP_PARENT *appwk)
{
  int i;
  OCCUPY_INFO *occupy;
  
  occupy = MonolithTool_GetOccupyInfo(appwk);
  
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    mpw->act_town[i] = MonolithTool_TownIcon_Create(
      appwk->setup, occupy, i, &TownIconPosTbl[i], COMMON_RESOURCE_INDEX_UP);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �X�A�C�R���A�N�^�[�폜
 */
//--------------------------------------------------------------
static void _TownIcon_AllDelete(MONOLITH_PALACEMAP_WORK *mpw)
{
  int i;
  
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    MonolithTool_TownIcon_Delete(mpw->act_town[i]);
  }
}

//--------------------------------------------------------------
/**
 * �X�A�C�R���S�X�V
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _TownIcon_AllUpdate(MONOLITH_PALACEMAP_WORK *mpw, MONOLITH_APP_PARENT *appwk)
{
  int i;
  OCCUPY_INFO *occupy;
  
  occupy = MonolithTool_GetOccupyInfo(appwk);
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    MonolithTool_TownIcon_Update(mpw->act_town[i], occupy, i);
  }
}
