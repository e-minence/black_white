//==============================================================================
/**
 * @file    monolith_mission_select.c
 * @brief   ���m���X�F�~�b�V�����󂯂�
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
#include "monolith.naix"


//==============================================================================
//  �萔��`
//==============================================================================
///�p�l���u�󂯂�vY���W
#define PANEL_RECEIVE_Y   (0x11*8)

//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  GFL_CLWK *act_town[INTRUDE_TOWN_MAX]; ///<�X�A�C�R���A�N�^�[�ւ̃|�C���^
  GFL_CLWK *act_cancel;   ///<�L�����Z���A�C�R���A�N�^�[�ւ̃|�C���^
  PANEL_ACTOR panel;
}MONOLITH_MSSELECT_WORK;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT MonolithMissionSelectProc_Init(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithMissionSelectProc_Main(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithMissionSelectProc_End(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void _Setup_BGFrameSetting(void);
static void _Setup_BGFrameExit(void);
static void _Setup_BGGraphicLoad(MONOLITH_SETUP *setup);
static void _TownIcon_AllCreate(MONOLITH_MSSELECT_WORK *mmw, MONOLITH_APP_PARENT *appwk);
static void _TownIcon_AllDelete(MONOLITH_MSSELECT_WORK *mmw);
static void _TownIcon_AllUpdate(MONOLITH_MSSELECT_WORK *mmw, MONOLITH_APP_PARENT *appwk);
static void _Msselect_PanelCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_MSSELECT_WORK *mmw);
static void _Msselect_PanelDelete(MONOLITH_MSSELECT_WORK *mmw);
static void _Msselect_PanelUpdate(MONOLITH_APP_PARENT *appwk, MONOLITH_MSSELECT_WORK *mmw);
static void _Msselect_CancelIconCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_MSSELECT_WORK *mmw);
static void _Msselect_CancelIconDelete(MONOLITH_MSSELECT_WORK *mmw);
static void _Msselect_CancelIconUpdate(MONOLITH_MSSELECT_WORK *mmw);


//==============================================================================
//  �f�[�^
//==============================================================================
///���m���X�p���[ROC�f�[�^
const GFL_PROC_DATA MonolithAppProc_Down_MissionSelect = {
  MonolithMissionSelectProc_Init,
  MonolithMissionSelectProc_Main,
  MonolithMissionSelectProc_End,
};

//--------------------------------------------------------------
//  �X�A�C�R��
//--------------------------------------------------------------
///�X�A�C�R���\�����W
enum{
  TOWN_0_X = 11*8,
  TOWN_0_Y = 3*8,
  TOWN_1_X = 16*8,
  TOWN_1_Y = 3*8,
  TOWN_2_X = 21*8,
  TOWN_2_Y = 3*8,
  
  TOWN_3_X = 8*8,
  TOWN_3_Y = 7*8,
  TOWN_4_X = 0x18*8,
  TOWN_4_Y = 7*8,
  
  TOWN_5_X = 11*8,
  TOWN_5_Y = 11*8,
  TOWN_6_X = 16*8,
  TOWN_6_Y = 11*8,
  TOWN_7_X = 21*8,
  TOWN_7_Y = 11*8,
};

///�X�A�C�R���\�����W�e�[�u��
static const GFL_POINT TownIconPosTbl[] = {
  {TOWN_0_X, TOWN_0_Y},
  {TOWN_1_X, TOWN_1_Y},
  {TOWN_2_X, TOWN_2_Y},
  
  {TOWN_3_X, TOWN_3_Y},
  {TOWN_4_X, TOWN_4_Y},
  
  {TOWN_5_X, TOWN_5_Y},
  {TOWN_6_X, TOWN_6_Y},
  {TOWN_7_X, TOWN_7_Y},
};
SDK_COMPILER_ASSERT(NELEMS(TownIconPosTbl) == INTRUDE_TOWN_MAX);

///�X�A�C�R���^�b�`�͈́F���a
#define TOWN_TOUCH_RANGE  (8)

///�X�A�C�R���^�b�`�͈̓e�[�u��
static const GFL_UI_TP_HITTBL TownTouchRect[] = {
  {//TOUCH_TOWN0
    TOWN_0_Y - TOWN_TOUCH_RANGE,
    TOWN_0_Y + TOWN_TOUCH_RANGE,
    TOWN_0_X - TOWN_TOUCH_RANGE,
    TOWN_0_X + TOWN_TOUCH_RANGE,
  },
  {//TOUCH_TOWN1
    TOWN_1_Y - TOWN_TOUCH_RANGE,
    TOWN_1_Y + TOWN_TOUCH_RANGE,
    TOWN_1_X - TOWN_TOUCH_RANGE,
    TOWN_1_X + TOWN_TOUCH_RANGE,
  },
  {//TOUCH_TOWN2
    TOWN_2_Y - TOWN_TOUCH_RANGE,
    TOWN_2_Y + TOWN_TOUCH_RANGE,
    TOWN_2_X - TOWN_TOUCH_RANGE,
    TOWN_2_X + TOWN_TOUCH_RANGE,
  },
  {//TOUCH_TOWN3
    TOWN_3_Y - TOWN_TOUCH_RANGE,
    TOWN_3_Y + TOWN_TOUCH_RANGE,
    TOWN_3_X - TOWN_TOUCH_RANGE,
    TOWN_3_X + TOWN_TOUCH_RANGE,
  },
  {//TOUCH_TOWN4
    TOWN_4_Y - TOWN_TOUCH_RANGE,
    TOWN_4_Y + TOWN_TOUCH_RANGE,
    TOWN_4_X - TOWN_TOUCH_RANGE,
    TOWN_4_X + TOWN_TOUCH_RANGE,
  },
  {//TOUCH_TOWN5
    TOWN_5_Y - TOWN_TOUCH_RANGE,
    TOWN_5_Y + TOWN_TOUCH_RANGE,
    TOWN_5_X - TOWN_TOUCH_RANGE,
    TOWN_5_X + TOWN_TOUCH_RANGE,
  },
  {//TOUCH_TOWN6
    TOWN_6_Y - TOWN_TOUCH_RANGE,
    TOWN_6_Y + TOWN_TOUCH_RANGE,
    TOWN_6_X - TOWN_TOUCH_RANGE,
    TOWN_6_X + TOWN_TOUCH_RANGE,
  },
  {//TOUCH_TOWN7
    TOWN_7_Y - TOWN_TOUCH_RANGE,
    TOWN_7_Y + TOWN_TOUCH_RANGE,
    TOWN_7_X - TOWN_TOUCH_RANGE,
    TOWN_7_X + TOWN_TOUCH_RANGE,
  },
  {////TOUCH_RECEIVE �p�l���F�~�b�V�������󂯂�
    PANEL_RECEIVE_Y - PANEL_CHARSIZE_Y/2*8,
    PANEL_RECEIVE_Y + PANEL_CHARSIZE_Y/2*8,
    PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
    PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
  },
  {////TOUCH_CANCEL �L�����Z���A�C�R��
    CANCEL_POS_Y - CANCEL_TOUCH_RANGE_HALF,
    CANCEL_POS_Y + CANCEL_TOUCH_RANGE_HALF,
    CANCEL_POS_X - CANCEL_TOUCH_RANGE_HALF,
    CANCEL_POS_X + CANCEL_TOUCH_RANGE_HALF,
  },
  { GFL_UI_TP_HIT_END, 0, 0, 0 },
};
SDK_COMPILER_ASSERT(NELEMS(TownTouchRect) == INTRUDE_TOWN_MAX + 3);

enum{
  TOUCH_TOWN0,
  TOUCH_TOWN1,
  TOUCH_TOWN2,
  TOUCH_TOWN3,
  TOUCH_TOWN4,
  TOUCH_TOWN5,
  TOUCH_TOWN6,
  TOUCH_TOWN7,
  TOUCH_RECEIVE,
  TOUCH_CANCEL,
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
static GFL_PROC_RESULT MonolithMissionSelectProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_MSSELECT_WORK *mmw = mywk;
	ARCHANDLE *hdl;
  
  mmw = GFL_PROC_AllocWork(proc, sizeof(MONOLITH_MSSELECT_WORK), HEAPID_MONOLITH);
  GFL_STD_MemClear(mmw, sizeof(MONOLITH_MSSELECT_WORK));
  
  //BG
  _Setup_BGFrameSetting();
  _Setup_BGGraphicLoad(appwk->setup);
  //OBJ
  _TownIcon_AllCreate(mmw, appwk);
  _Msselect_PanelCreate(appwk, mmw);
  _Msselect_CancelIconCreate(appwk, mmw);

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
static GFL_PROC_RESULT MonolithMissionSelectProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_MSSELECT_WORK *mmw = mywk;
  int tp_ret;
  
  _TownIcon_AllUpdate(mmw, appwk);
  _Msselect_PanelUpdate(appwk, mmw);
  _Msselect_CancelIconUpdate(mmw);

  switch(*seq){
  case 0:
    tp_ret = GFL_UI_TP_HitTrg(TownTouchRect);
    if(tp_ret >= TOUCH_TOWN0 && tp_ret < TOUCH_TOWN0 + INTRUDE_TOWN_MAX){
      OS_TPrintf("�X�I�� %d\n", tp_ret);
      appwk->common->mission_select_town = tp_ret;
    }
    else if(tp_ret == TOUCH_RECEIVE){
      OS_TPrintf("�u�󂯂�v�I��\n");
    }
    else if(tp_ret == TOUCH_CANCEL){
      OS_TPrintf("�L�����Z���I��\n");
    }
    
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
static GFL_PROC_RESULT MonolithMissionSelectProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_MSSELECT_WORK *mmw = mywk;
  
  //OBJ
  _Msselect_CancelIconDelete(mmw);
  _Msselect_PanelDelete(mmw);
  _TownIcon_AllDelete(mmw);
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
		{//GFL_BG_FRAME2_S
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, MONO_BG_COMMON_CHARBASE, MONO_BG_COMMON_CHAR_SIZE,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( GFL_BG_FRAME2_S,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );

	GFL_BG_FillScreen( GFL_BG_FRAME2_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_SetVisible(GFL_BG_FRAME2_S, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * BG�t���[�����
 */
//--------------------------------------------------------------
static void _Setup_BGFrameExit(void)
{
	GFL_BG_SetVisible(GFL_BG_FRAME2_S, VISIBLE_OFF);
  GFL_BG_FreeBGControl(GFL_BG_FRAME2_S);
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
	GFL_ARCHDL_UTIL_TransVramScreen(setup->hdl, NARC_monolith_mono_bgd_map_lz_NSCR, 
		GFL_BG_FRAME2_S, 0, 0, TRUE, HEAPID_MONOLITH);

	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
}

//--------------------------------------------------------------
/**
 * @brief   �X�A�C�R���A�N�^�[����
 */
//--------------------------------------------------------------
static void _TownIcon_AllCreate(MONOLITH_MSSELECT_WORK *mmw, MONOLITH_APP_PARENT *appwk)
{
  int i;
  OCCUPY_INFO *occupy;
  
  occupy = MonolithTool_GetOccupyInfo(appwk);
  
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    mmw->act_town[i] = MonolithTool_TownIcon_Create(
      appwk->setup, occupy, i, &TownIconPosTbl[i], COMMON_RESOURCE_INDEX_UP);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �X�A�C�R���A�N�^�[�폜
 */
//--------------------------------------------------------------
static void _TownIcon_AllDelete(MONOLITH_MSSELECT_WORK *mmw)
{
  int i;
  
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    MonolithTool_TownIcon_Delete(mmw->act_town[i]);
  }
}

//--------------------------------------------------------------
/**
 * �X�A�C�R���S�X�V
 *
 * @param   mmw		
 */
//--------------------------------------------------------------
static void _TownIcon_AllUpdate(MONOLITH_MSSELECT_WORK *mmw, MONOLITH_APP_PARENT *appwk)
{
  int i;
  OCCUPY_INFO *occupy;
  
  occupy = MonolithTool_GetOccupyInfo(appwk);
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    MonolithTool_TownIcon_Update(mmw->act_town[i], occupy, i);
  }
}

//==================================================================
/**
 * �p�l������
 *
 * @param   appwk		
 * @param   mmw		
 */
//==================================================================
static void _Msselect_PanelCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_MSSELECT_WORK *mmw)
{
  MonolithTool_Panel_Create(appwk->setup, &mmw->panel, 
    COMMON_RESOURCE_INDEX_DOWN, PANEL_SIZE_SMALL, PANEL_RECEIVE_Y, msg_mono_mis_000);
  MonolithTool_Panel_Focus(appwk, &mmw->panel, 1, 0, FADE_SUB_OBJ);
}

//==================================================================
/**
 * �p�l���폜
 *
 * @param   mmw		
 */
//==================================================================
static void _Msselect_PanelDelete(MONOLITH_MSSELECT_WORK *mmw)
{
  MonolithTool_Panel_Delete(&mmw->panel);
}

//==================================================================
/**
 * �p�l���X�V����
 *
 * @param   mmw		
 */
//==================================================================
static void _Msselect_PanelUpdate(MONOLITH_APP_PARENT *appwk, MONOLITH_MSSELECT_WORK *mmw)
{
  MonolithTool_Panel_ColorUpdate(appwk, FADE_SUB_OBJ);
  MonolithTool_Panel_TransUpdate(appwk->setup, &mmw->panel);
}

//==================================================================
/**
 * �L�����Z���A�C�R������
 *
 * @param   appwk		
 * @param   mmw		
 */
//==================================================================
static void _Msselect_CancelIconCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_MSSELECT_WORK *mmw)
{
  mmw->act_cancel = MonolithTool_CancelIcon_Create(appwk->setup);
}

//==================================================================
/**
 * �L�����Z���A�C�R���폜
 *
 * @param   mmw		
 */
//==================================================================
static void _Msselect_CancelIconDelete(MONOLITH_MSSELECT_WORK *mmw)
{
  MonolithTool_CancelIcon_Delete(mmw->act_cancel);
}

//==================================================================
/**
 * �L�����Z���A�C�R���X�V����
 *
 * @param   mmw		
 */
//==================================================================
static void _Msselect_CancelIconUpdate(MONOLITH_MSSELECT_WORK *mmw)
{
  MonolithTool_CancelIcon_Update(mmw->act_cancel);
}
