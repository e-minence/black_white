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
#include "intrude_types.h"
#include "system/bmp_winframe.h"
#include "gamesystem/msgspeed.h"
#include "monolith_snd_def.h"
#include "field/eventwork.h"
#include "../../../../resource/fldmapdata/flagwork/flag_define.h" //SYS_FLAG_


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

///BMP�g�̃p���b�g�ԍ�
#define BMPWIN_FRAME_PALNO        (MONOLITH_MENUBAR_PALNO - 1)
///BMP�g�̃L�����N�^�J�n�ʒu
#define BMPWIN_FRAME_START_CGX    (1)

///�^�C�g���̕\�����[�h
enum{
  TITLE_MODE_NOMISSION,   ///<�~�b�V�����I�����o���Ȃ�
  TITLE_MODE_NORMAL,      ///<�ʏ�\��
  TITLE_MODE_NOPOWER,     ///<�p���[�I�����o���Ȃ�
};

//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  PANEL_ACTOR panel[TITLE_PANEL_MAX];
  s8 cursor_pos;      ///<�J�[�\���ʒu
  s8 select_panel;    ///<�I�������p�l��(TITLE_PANEL_xxx)
  u8 title_mode;      ///<�^�C�g���̕\�����[�h(TITLE_MODE_xxx)
  u8 no_focus;        ///<TRUE:�J�[�\���������t�H�[�J�X���Ă��Ȃ����
  MONOLITH_CANCEL_ICON cancel_icon;   ///<�L�����Z���A�C�R���A�N�^�[�ւ̃|�C���^
  GFL_BMPWIN *bmpwin;
  PRINT_STREAM *print_stream;
  STRBUF *strbuf_stream;
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
static void _Setup_BGFrameSetting(void);
static void _Setup_BGFrameExit(void);
static void _Setup_BmpWinCreate(MONOLITH_TITLE_WORK *mtw, MONOLITH_SETUP *setup);
static void _Setup_BmpWinDelete(MONOLITH_TITLE_WORK *mtw);
static void _Set_MsgStream(MONOLITH_TITLE_WORK *mtw, MONOLITH_SETUP *setup, u16 msg_id);
static BOOL _Wait_MsgStream(MONOLITH_SETUP *setup, MONOLITH_TITLE_WORK *mtw);
static void _Clear_MsgStream(MONOLITH_TITLE_WORK *mtw);


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

  TITLE_PANEL_Y_MISSION_NOPOWER = TITLE_PANEL_Y_POWER_NOMISSION,
  TITLE_PANEL_Y_STATUS_NOPOWER = TITLE_PANEL_Y_STATUS_NOMISSION,
};

///�p�l���̐�
static const u32 TitlePanelMax[] = {  // 0:�~�b�V�����Ȃ��@1:�~�b�V��������@2:���~�b�V����
  2,
  3,
  2,
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
  {
    TITLE_PANEL_Y_MISSION_NOPOWER,
    TITLE_PANEL_Y_STATUS_NOPOWER,
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
  {
    TITLE_PANEL_MISSION,
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
  {
    {//TITLE_PANEL_Y_MISSION
      TITLE_PANEL_Y_MISSION_NOPOWER - PANEL_CHARSIZE_Y/2*8,
      TITLE_PANEL_Y_MISSION_NOPOWER + PANEL_CHARSIZE_Y/2*8,
      PANEL_POS_X - PANEL_SMALL_CHARSIZE_X/2*8,
      PANEL_POS_X + PANEL_SMALL_CHARSIZE_X/2*8,
    },
    {//TITLE_PANEL_Y_STATUS
      TITLE_PANEL_Y_STATUS_NOPOWER - PANEL_CHARSIZE_Y/2*8,
      TITLE_PANEL_Y_STATUS_NOPOWER + PANEL_CHARSIZE_Y/2*8,
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
	GAMEDATA *gamedata = GAMESYSTEM_GetGameData(appwk->parent->gsys);
  
  mtw = GFL_PROC_AllocWork(proc, sizeof(MONOLITH_TITLE_WORK), HEAPID_MONOLITH);
  GFL_STD_MemClear(mtw, sizeof(MONOLITH_TITLE_WORK));
  
  if(EVENTWORK_CheckEventFlag(
      GAMEDATA_GetEventWork(gamedata), SYS_FLAG_PALACE_MISSION_CLEAR) == FALSE){
    mtw->title_mode = TITLE_MODE_NOPOWER;
  }
  else if(appwk->parent->list_occ == TRUE){
    mtw->title_mode = TITLE_MODE_NORMAL;
  }
  else{
    mtw->title_mode = TITLE_MODE_NOMISSION;
  }
  
  _Setup_BGFrameSetting();
  _Setup_BmpWinCreate(mtw, appwk->setup);

  _Title_PanelCreate(appwk, mtw);
  _Title_CancelIconCreate(appwk, mtw);
  
	GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
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
  enum{
    _SEQ_MAIN,
    _SEQ_DECIDE,
    _SEQ_DECIDE_WAIT,
    _SEQ_STREAM_WAIT,
  };
  
  _Title_PanelUpdate(appwk, mtw);
  _Title_CancelIconUpdate(mtw);

  if(appwk->force_finish == TRUE){
    return GFL_PROC_RES_FINISH;
  }

  switch(*seq){
  case _SEQ_MAIN:
    tp_ret = GFL_UI_TP_HitTrg(TitlePanelRect[mtw->title_mode]);
    if(tp_ret != GFL_UI_TP_HIT_NONE){
      OS_TPrintf("�^�b�`�L�� %d\n", tp_ret);
      mtw->cursor_pos = tp_ret;
      mtw->select_panel = TitlePanelSelectIndex[mtw->title_mode][tp_ret];
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
      mtw->no_focus = FALSE;
      (*seq)++;
      break;
    }
    else{
      int repeat = GFL_UI_KEY_GetRepeat();
      int trg = GFL_UI_KEY_GetTrg();
      
      if(trg > 0 && mtw->no_focus == TRUE){
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        mtw->no_focus = FALSE;
        MonolithTool_PanelOBJ_Focus(appwk, mtw->panel, TitlePanelMax[mtw->title_mode], 
          mtw->cursor_pos, FADE_SUB_OBJ);
        PMSND_PlaySE(MONOLITH_SE_SELECT);
      }
      else if(trg & PAD_BUTTON_DECIDE){
        mtw->select_panel = TitlePanelSelectIndex[mtw->title_mode][mtw->cursor_pos];
        (*seq)++;
      }
      else if(trg & PAD_BUTTON_CANCEL){
        mtw->select_panel = TITLE_PANEL_CANCEL;
        (*seq)++;
      }
      else if(repeat & PAD_KEY_DOWN){
        mtw->cursor_pos++;
        if(mtw->cursor_pos >= TitlePanelMax[mtw->title_mode]){
          mtw->cursor_pos = 0;
        }
        MonolithTool_PanelOBJ_Focus(appwk, mtw->panel, 
          TitlePanelMax[mtw->title_mode], mtw->cursor_pos, FADE_SUB_OBJ);
        PMSND_PlaySE(MONOLITH_SE_SELECT);
      }
      else if(repeat & PAD_KEY_UP){
        mtw->cursor_pos--;
        if(mtw->cursor_pos < 0){
          mtw->cursor_pos = TitlePanelMax[mtw->title_mode] - 1;
        }
        MonolithTool_PanelOBJ_Focus(
          appwk, mtw->panel, TitlePanelMax[mtw->title_mode], 
          mtw->cursor_pos, FADE_SUB_OBJ);
        PMSND_PlaySE(MONOLITH_SE_SELECT);
      }
    }
    break;
  case _SEQ_DECIDE:
    if(mtw->select_panel == TITLE_PANEL_MISSION){
      if(appwk->parent->list_occ == FALSE){
        OS_TPrintf("�~�b�V�����I����⃊�X�g������M\n");
        GF_ASSERT(0);
        (*seq)--;
        break;
      }
      else if(appwk->parent->intcomm != NULL 
          && MISSION_RecvCheck(&appwk->parent->intcomm->mission) == TRUE){  //�~�b�V�����󒍍ς�
        PMSND_PlaySE(MONOLITH_SE_DECIDE);
        _Set_MsgStream(mtw, appwk->setup, msg_mono_title_004);
        (*seq) = _SEQ_STREAM_WAIT;
        break;
      }
    }
    if(mtw->select_panel != TITLE_PANEL_CANCEL){
      MonolithTool_PanelOBJ_Flash(appwk, mtw->panel, 
        TitlePanelMax[mtw->title_mode], mtw->cursor_pos, FADE_SUB_OBJ);
      PMSND_PlaySE(MONOLITH_SE_DECIDE);
    }
    else{
      MonolithTool_CancelIcon_FlashReq(&mtw->cancel_icon);
      PMSND_PlaySE(MONOLITH_SE_CANCEL);
    }
    (*seq)++;
    break;
  case _SEQ_DECIDE_WAIT:
    if(mtw->select_panel != TITLE_PANEL_CANCEL){
      if(MonolithTool_PanelColor_GetMode(appwk, FADE_SUB_OBJ) != PANEL_COLORMODE_FLASH){
        appwk->next_menu_index = MONOLITH_MENU_MISSION + mtw->select_panel;
        return GFL_PROC_RES_FINISH;
      }
    }
    else{
      if(MonolithTool_CancelIcon_FlashCheck(&mtw->cancel_icon) == FALSE){
        appwk->next_menu_index = MONOLITH_MENU_MISSION + mtw->select_panel;
        return GFL_PROC_RES_FINISH;
      }
    }
    break;
  
  case _SEQ_STREAM_WAIT:
    if(_Wait_MsgStream(appwk->setup, mtw) == TRUE){
      if(GFL_UI_TP_GetTrg() || (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
        _Clear_MsgStream(mtw);
        PMSND_PlaySE(MONOLITH_SE_MSG);
        *seq = _SEQ_MAIN;
      }
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

  if(PRINTSYS_QUE_IsFinished(appwk->setup->printQue) == FALSE){
    return GFL_PROC_RES_CONTINUE;
  }

  GFL_DISP_GXS_SetVisibleControlDirect(GX_PLANEMASK_BG3);

  if(mtw->print_stream != NULL){
    PRINTSYS_PrintStreamDelete(mtw->print_stream);
  }
  if(mtw->strbuf_stream != NULL){
    GFL_STR_DeleteBuffer(mtw->strbuf_stream);
  }

  _Title_PanelDelete(mtw);
  _Title_CancelIconDelete(mtw);

  _Setup_BmpWinDelete(mtw);
  _Setup_BGFrameExit();
  
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

  for(i = 0; i < TitlePanelMax[mtw->title_mode]; i++){
    if(mtw->title_mode == TITLE_MODE_NORMAL){
      MonolithTool_PanelOBJ_Create(appwk->setup, &mtw->panel[i], 
        COMMON_RESOURCE_INDEX_DOWN, PANEL_SIZE_SMALL, 
        PANEL_POS_X, TitlePanelTblY[mtw->title_mode][i], 
        msg_mono_title_001 + i, NULL);
    }
    else if(mtw->title_mode == TITLE_MODE_NOMISSION){
      MonolithTool_PanelOBJ_Create(appwk->setup, &mtw->panel[i], 
        COMMON_RESOURCE_INDEX_DOWN, PANEL_SIZE_SMALL, 
        PANEL_POS_X, TitlePanelTblY[mtw->title_mode][i], 
        msg_mono_title_002 + i, NULL);
    }
    else if(mtw->title_mode == TITLE_MODE_NOPOWER){
      u32 msg_id = i == 0 ? msg_mono_title_001 : msg_mono_title_003;
      MonolithTool_PanelOBJ_Create(appwk->setup, &mtw->panel[i], 
        COMMON_RESOURCE_INDEX_DOWN, PANEL_SIZE_SMALL, 
        PANEL_POS_X, TitlePanelTblY[mtw->title_mode][i], 
        msg_id, NULL);
    }
  }
  
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_KEY ){
    MonolithTool_PanelOBJ_Focus(appwk, mtw->panel, TitlePanelMax[mtw->title_mode], 
      0, FADE_SUB_OBJ);
    mtw->no_focus = FALSE;
  }
  else{
    mtw->no_focus = TRUE;
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
  
  for(i = 0; i < TitlePanelMax[mtw->title_mode]; i++){
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
  for(i = 0; i < TitlePanelMax[mtw->title_mode]; i++){
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
  MonolithTool_CancelIcon_Create(appwk->setup, &mtw->cancel_icon);
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
  MonolithTool_CancelIcon_Delete(&mtw->cancel_icon);
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
  MonolithTool_CancelIcon_Update(&mtw->cancel_icon);
}

//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 */
//--------------------------------------------------------------
static void _Setup_BGFrameSetting(void)
{
	static const GFL_BG_BGCNT_HEADER bgcnt_frame[] = {
		{//GFL_BG_FRAME1_S
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x0c000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( GFL_BG_FRAME1_S,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );

	GFL_BG_FillScreen( GFL_BG_FRAME1_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_OFF);
}

//--------------------------------------------------------------
/**
 * BG�t���[�����
 */
//--------------------------------------------------------------
static void _Setup_BGFrameExit(void)
{
	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_OFF);
  GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
}

//--------------------------------------------------------------
/**
 * BMPWIN�쐬
 *
 * @param   mtw		
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_BmpWinCreate(MONOLITH_TITLE_WORK *mtw, MONOLITH_SETUP *setup)
{
  GFL_BMP_DATA *bmp;

  BmpWinFrame_CgxSet( GFL_BG_FRAME1_S, BMPWIN_FRAME_START_CGX, MENU_TYPE_SYSTEM, HEAPID_MONOLITH );
  PaletteWorkSetEx_Arc(setup->pfd, ARCID_FLDMAP_WINFRAME, 
    BmpWinFrame_WinPalArcGet(MENU_TYPE_SYSTEM), 
    HEAPID_MONOLITH, FADE_SUB_BG, 0x20, BMPWIN_FRAME_PALNO * 16, 0);
  
	mtw->bmpwin = GFL_BMPWIN_Create( GFL_BG_FRAME1_S,
		1,19,30,4, MONOLITH_BG_DOWN_FONT_PALNO, GFL_BMP_CHRAREA_GET_B );

	bmp = GFL_BMPWIN_GetBmp( mtw->bmpwin );
	
	GFL_BMP_Clear( bmp, 0xff );
//	GFL_BMPWIN_TransVramCharacter( mtw->bmpwin );
	GFL_BMPWIN_MakeScreen( mtw->bmpwin );
//	GFL_BG_LoadScreenReq( GFL_BG_FRAME1_S );
	
	BmpWinFrame_Write( mtw->bmpwin, WINDOW_TRANS_ON, BMPWIN_FRAME_START_CGX, BMPWIN_FRAME_PALNO );
}

//--------------------------------------------------------------
/**
 * BMPWIN�폜
 *
 * @param   mtw		
 */
//--------------------------------------------------------------
static void _Setup_BmpWinDelete(MONOLITH_TITLE_WORK *mtw)
{
  GFL_BMPWIN_Delete(mtw->bmpwin);
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�o�͊J�n
 *
 * @param   mtw		
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Set_MsgStream(MONOLITH_TITLE_WORK *mtw, MONOLITH_SETUP *setup, u16 msg_id)
{
  GF_ASSERT(mtw->print_stream == NULL);
  
  GFL_FONTSYS_SetColor( 1, 2, 15 );

  mtw->strbuf_stream = GFL_MSG_CreateString(setup->mm_monolith, msg_id);
  
  mtw->print_stream = PRINTSYS_PrintStream(
    mtw->bmpwin, 0, 0, mtw->strbuf_stream, setup->font_handle,
    MSGSPEED_GetWait(), setup->tcbl_sys, 10, HEAPID_MONOLITH, 0xf);

	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�o�͊����҂�
 *
 * @param   mtw		
 *
 * @retval  BOOL		TRUE:�o�͊���
 */
//--------------------------------------------------------------
static BOOL _Wait_MsgStream(MONOLITH_SETUP *setup, MONOLITH_TITLE_WORK *mtw)
{
  return APP_PRINTSYS_COMMON_PrintStreamFunc( &setup->app_printsys, mtw->print_stream );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�N���A
 *
 * @param   mtw		
 */
//--------------------------------------------------------------
static void _Clear_MsgStream(MONOLITH_TITLE_WORK *mtw)
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( mtw->bmpwin );

  GF_ASSERT(mtw->print_stream != NULL);
  PRINTSYS_PrintStreamDelete(mtw->print_stream);
  mtw->print_stream = NULL;
  
  GFL_STR_DeleteBuffer(mtw->strbuf_stream);
  mtw->strbuf_stream = NULL;
  
	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_OFF);
  GFL_BMP_Clear(bmp, 0xff);
	GFL_BMPWIN_TransVramCharacter( mtw->bmpwin );

  GFL_FONTSYS_SetColor( 
    MONOLITH_FONT_DEFCOLOR_LETTER, MONOLITH_FONT_DEFCOLOR_SHADOW, MONOLITH_FONT_DEFCOLOR_BACK );
}
