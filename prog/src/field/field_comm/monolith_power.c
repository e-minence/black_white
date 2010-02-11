//==============================================================================
/**
 * @file    monolith_power.c
 * @brief   ���m���X�F�p���[�I�����
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
#include "intrude_mission.h"
#include "intrude_comm_command.h"
#include "msg/msg_monolith.h"
#include "monolith.naix"
#include "system/bmp_winframe.h"
#include "gamesystem/msgspeed.h"
#include "message.naix"
#include "gamesystem/g_power.h"


//==============================================================================
//  �萔��`
//==============================================================================
///BMP�g�̃p���b�g�ԍ�
#define BMPWIN_FRAME_PALNO        (MONOLITH_BG_DOWN_FONT_PALNO - 1)
///BMP�g�̃L�����N�^�J�n�ʒu
#define BMPWIN_FRAME_START_CGX    (1)


//--------------------------------------------------------------
//  
//--------------------------------------------------------------
///G�p���[�K����
typedef enum{
  _USE_POWER_OK,        ///<�K�����Ă���
  _USE_POWER_SOMEMORE,  ///<���Ə����ŏK��
  _USE_POWER_NONE,      ///<�K�����Ă��Ȃ�
  
  _USE_POWER_MAX,
}_USE_POWER;

///G�p���[���p�Ɋm�ۂ���BMP��X(�L�����N�^�P��)
#define GPOWER_NAME_BMP_LEN_X     (23)
///G�p���[���p�Ɋm�ۂ���BMP��Y(�L�����N�^�P��)
#define GPOWER_NAME_BMP_LEN_Y     (2)

///1�t���[���ɕ`�悷��G�p���[���̐� (�L���[�ɒ��߂�ɂ͐�����������̂Ŏ����Ńt���[���𕪂��ĕ`��)
#define GPOWER_NAME_WRITE_NUM     (3)

///��x�ɉ�ʂɕ\�������p���[���ڐ�
#define POWER_ITEM_DISP_NUM   (7)

enum{
  SOFTPRI_BMPOAM_NAME = 10,
};

enum{
  BGPRI_BMPOAM_NAME = 2,
};

enum{
  _PANEL_LEFT_POS = 3,      ///<�p�l�����[X�ʒu(�L�����N�^�P��)
  _PANEL_RIGHT_POS = 0x1c,  ///<�p�l���E�[X�ʒu(�L�����N�^�P��)
  _PANEL_SIZE_Y = 4,        ///<�p�l��Y�T�C�Y(�L�����N�^�P��)
};

///�p���[���̃��X�g�Ԃ̕�(�h�b�g��)
#define POWER_LIST_SPACE    (_PANEL_SIZE_Y*8)

///BMPOAM�̃p���[�� X���W
#define _BMPOAM_POWER_NAME_X    (4*8)
///BMPOAM�̃p���[�� �J�nY���W�I�t�Z�b�g
#define _BMPOAM_POWER_NAME_OFFSET_Y    (8)

///�X�N���[���̌�����(����8�r�b�g����)
#define _SCROLL_SLOW_DOWN   (0x00c0)

enum{
  _TOUCH_DECIDE_RANGE = 4,    ///<����`�F�b�N�F�ŏ��Ƀ^�b�`�p�l������������Ԃ��猈��Ɣ��肷�鋖�e�̂���
  _TOUCH_DECIDE_NONE = 0xffff,  ///<����`�F�b�N�F�ŏ��̃^�b�`���炸�ꂷ�����ׁA�����������l
};

///�����ȍ���No
#define _LIST_NO_NONE     (0xffff)
///�����ȃJ�[�\���ʒu
#define _CURSOR_POS_NONE  (0xffff)

typedef enum{
  _SET_CURSOR_MODE_INIT,  ///<�������p
  _SET_CURSOR_MODE_KEY,   ///<�L�[�ő���
  _SET_CURSOR_MODE_TP,    ///<�^�b�`�p�l���ő���
}_SET_CURSOR_MODE;


//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  GFL_TCB *vintr_tcb;
  
  GFL_MSGDATA *mm_power;        ///<�p���[��gmm
  GFL_BMP_DATA *bmp_power_name[GPOWER_ID_MAX];
  GPOWER_ID use_gpower_id[GPOWER_ID_MAX];      ///<bmp_power_name�ɑΉ�����GPOWER_ID�������Ă���
  _USE_POWER use_power[GPOWER_ID_MAX];         ///<use_gpower_id�̏K����
  u8 power_list_num;        ///<�p���[���X�g��
  u8 power_list_write_num;  ///<BMP�ɏ������񂾖��O�̐�
  
  GFL_BMPWIN *bmpwin;
  STRBUF *strbuf_stream;
  PRINT_STREAM *print_stream;
  GFL_CLWK *act_cancel;   ///<�L�����Z���A�C�R���A�N�^�[�ւ̃|�C���^

  BMPOAM_ACT_PTR bmpoam_power[POWER_ITEM_DISP_NUM];
  GFL_BMP_DATA *bmpoam_power_bmp[POWER_ITEM_DISP_NUM];
  
  s32 list_y;    ///<����8�r�b�g����
  
  u32 tp_cont_frame;     ///<TRUE:�^�b�`�p�l���������ςȂ��ɂ��Ă���t���[����
  s32 speed;              ///<�X�N���[�����x(����8�r�b�g�����B�}�C�i�X�L)
  u32 tp_backup_y;        ///<1�t���[���O��TP_Y�l
  u32 tp_first_hit_y;     ///<�ŏ��Ƀ^�b�`��������Y�l
  
  BOOL scroll_update;     ///<TRUE:�X�N���[�����W�ɐ������x���ŕω���������
  
  u16 cursor_pos;         ///<�J�[�\���ʒu(����No)
  u16 backup_cursor_pos;  ///<1�O�̃J�[�\���ʒu
  u16 decide_cursor_pos;  ///<���莞�̃J�[�\���ʒu
}MONOLITH_PWSELECT_WORK;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT MonolithPowerSelectProc_Init(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithPowerSelectProc_Main(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT MonolithPowerSelectProc_End(
  GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static void _Setup_BGFrameSetting(void);
static void _Setup_BGFrameExit(void);
static void _Setup_BGGraphicLoad(MONOLITH_SETUP *setup);
static void _BmpOamCreate(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup);
static void _BmpOamDelete(MONOLITH_PWSELECT_WORK *mpw);
static void _CancelIconCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw);
static void _CancelIconDelete(MONOLITH_PWSELECT_WORK *mpw);
static void _CancelIconUpdate(MONOLITH_PWSELECT_WORK *mpw);
static void _Setup_BmpWinCreate(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup);
static void _Setup_BmpWinDelete(MONOLITH_PWSELECT_WORK *mpw);
static void _Set_MsgStream(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup, u16 msg_id);
static BOOL _Wait_MsgStream(MONOLITH_PWSELECT_WORK *mpw);
static void _Clear_MsgStream(MONOLITH_PWSELECT_WORK *mpw);
static _USE_POWER _CheckUsePower(MONOLITH_SETUP *setup, GPOWER_ID gpower_id);
static void _Setup_PowerNameBMPCreate(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup);
static void _Setup_PowerNameBMPDelete(MONOLITH_PWSELECT_WORK *mpw);
static BOOL _PowerNameBMPDraw(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup);
static BOOL _ScrollSpeedUpdate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw);
static BOOL _ScrollPosUpdate(MONOLITH_PWSELECT_WORK *mpw, s32 add_y);
static void _ScrollPos_BGReflection(MONOLITH_PWSELECT_WORK *mpw);
static void _ScrollPos_NameOamReflection(MONOLITH_PWSELECT_WORK *mpw);
static void _PowerSelect_VBlank(GFL_TCB *tcb, void *work);
static s32 _GetTouchListNo(MONOLITH_PWSELECT_WORK *mpw, u32 tp_y);
static void _SetCursorPos(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos, _SET_CURSOR_MODE mode);
static void _CursorPos_PanelScreenChange(MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos, int palno);
static void _CursorPos_PanelFocus(MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos);
static void _CursorPos_NotFocus(MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos);


//==============================================================================
//  �f�[�^
//==============================================================================
///���m���X�p���[ROC�f�[�^
const GFL_PROC_DATA MonolithAppProc_Down_PowerSelect = {
  MonolithPowerSelectProc_Init,
  MonolithPowerSelectProc_Main,
  MonolithPowerSelectProc_End,
};

///�^�b�`�͈̓e�[�u��
static const GFL_UI_TP_HITTBL TouchRect[] = {
  {////TOUCH_CANCEL �L�����Z���A�C�R��
    CANCEL_POS_Y - CANCEL_TOUCH_RANGE_HALF,
    CANCEL_POS_Y + CANCEL_TOUCH_RANGE_HALF,
    CANCEL_POS_X - CANCEL_TOUCH_RANGE_HALF,
    CANCEL_POS_X + CANCEL_TOUCH_RANGE_HALF,
  },
  { GFL_UI_TP_HIT_END, 0, 0, 0 },
};

enum{
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
static GFL_PROC_RESULT MonolithPowerSelectProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_PWSELECT_WORK *mpw = mywk;
	ARCHANDLE *hdl;
  
  switch(*seq){
  case 0:
    mpw = GFL_PROC_AllocWork(proc, sizeof(MONOLITH_PWSELECT_WORK), HEAPID_MONOLITH);
    GFL_STD_MemClear(mpw, sizeof(MONOLITH_PWSELECT_WORK));
    mpw->decide_cursor_pos = _CURSOR_POS_NONE;
    
  	mpw->mm_power = GFL_MSG_Create(
  		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_power_dat, HEAPID_MONOLITH);

    //BG
    _Setup_BGFrameSetting();
    _Setup_BGGraphicLoad(appwk->setup);
    _Setup_BmpWinCreate(mpw, appwk->setup);
    //OBJ
    _CancelIconCreate(appwk, mpw);
    _BmpOamCreate(mpw, appwk->setup);

    MonolithTool_Panel_Init(appwk);
    if(GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY){
      _SetCursorPos(appwk, mpw, 0, _SET_CURSOR_MODE_INIT);
    }
    else{
      _SetCursorPos(appwk, mpw, _CURSOR_POS_NONE, _SET_CURSOR_MODE_INIT);
    }
    
    mpw->vintr_tcb = GFUser_VIntr_CreateTCB(
      _PowerSelect_VBlank, mpw, MONOLITH_VINTR_TCB_PRI_POWER);

    OS_TPrintf("NameDraw start\n");
    _Setup_PowerNameBMPCreate(mpw, appwk->setup);
    (*seq)++;
    break;
  case 1:
    if(_PowerNameBMPDraw(mpw, appwk->setup) == TRUE){
      OS_TPrintf("NameDraw end\n");
      //���O�`��
      _ScrollPos_NameOamReflection(mpw);
      mpw->scroll_update = TRUE;
      return GFL_PROC_RES_FINISH;
    }
    break;
  }
  
  return GFL_PROC_RES_CONTINUE;
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
static GFL_PROC_RESULT MonolithPowerSelectProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_PWSELECT_WORK *mpw = mywk;
  enum{
    SEQ_INIT,
    SEQ_TOP,
    SEQ_FINISH,
  };
  
  MonolithTool_Panel_ColorUpdate(appwk, FADE_SUB_BG);
  _CancelIconUpdate(mpw);

  if(appwk->force_finish == TRUE){
    return GFL_PROC_RES_FINISH;
  }
  
  switch(*seq){
  case SEQ_INIT:
    *seq = SEQ_TOP;
    break;
    
  case SEQ_TOP:
    {
      int trg = GFL_UI_KEY_GetTrg();
      int tp_ret = GFL_UI_TP_HitTrg(TouchRect);

      if(tp_ret == TOUCH_CANCEL || (trg & PAD_BUTTON_CANCEL)){
        OS_TPrintf("�L�����Z���I��\n");
        (*seq) = SEQ_FINISH;
      }
      
      _ScrollSpeedUpdate(appwk, mpw);
      if(_ScrollPosUpdate(mpw, mpw->speed) == TRUE){
        _ScrollPos_BGReflection(mpw);
        _ScrollPos_NameOamReflection(mpw);
        mpw->scroll_update = TRUE;
      }
    }
    break;

  case SEQ_FINISH:
    if(MonolithTool_PanelColor_GetMode(appwk) != PANEL_COLORMODE_FLASH){
      appwk->next_menu_index = MONOLITH_MENU_TITLE;
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
static GFL_PROC_RESULT MonolithPowerSelectProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  MONOLITH_APP_PARENT *appwk = pwk;
	MONOLITH_PWSELECT_WORK *mpw = mywk;

  GFL_TCB_DeleteTask( mpw->vintr_tcb );
  
  if(mpw->print_stream != NULL){
    PRINTSYS_PrintStreamDelete(mpw->print_stream);
  }
  if(mpw->strbuf_stream != NULL){
    GFL_STR_DeleteBuffer(mpw->strbuf_stream);
  }

  _Setup_PowerNameBMPDelete(mpw);

  //OBJ
  _BmpOamDelete(mpw);
  _CancelIconDelete(mpw);
  //BG
  _Setup_BmpWinDelete(mpw);
  _Setup_BGFrameExit();

  GFL_MSG_Delete(mpw->mm_power);
  
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
		{//GFL_BG_FRAME1_S
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x0c000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		{//GFL_BG_FRAME2_S
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, MONO_BG_COMMON_CHARBASE, MONO_BG_COMMON_CHAR_SIZE,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( GFL_BG_FRAME1_S,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME2_S,   &bgcnt_frame[1],   GFL_BG_MODE_TEXT );

	GFL_BG_FillScreen( GFL_BG_FRAME1_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( GFL_BG_FRAME2_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_OFF);
	GFL_BG_SetVisible(GFL_BG_FRAME2_S, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * BG�t���[�����
 */
//--------------------------------------------------------------
static void _Setup_BGFrameExit(void)
{
	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_OFF);
	GFL_BG_SetVisible(GFL_BG_FRAME2_S, VISIBLE_OFF);
  GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
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
	GFL_ARCHDL_UTIL_TransVramScreen(setup->hdl, NARC_monolith_mono_bgd_power_lz_NSCR, 
		GFL_BG_FRAME2_S, 0, 0, TRUE, HEAPID_MONOLITH);

	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
}

//--------------------------------------------------------------
/**
 * BMPWIN�쐬
 *
 * @param   mpw		
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_BmpWinCreate(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup)
{
  GFL_BMP_DATA *bmp;

  BmpWinFrame_CgxSet( GFL_BG_FRAME1_S, BMPWIN_FRAME_START_CGX, MENU_TYPE_SYSTEM, HEAPID_MONOLITH );
  PaletteWorkSetEx_Arc(setup->pfd, ARCID_FLDMAP_WINFRAME, BmpWinFrame_WinPalArcGet(), 
    HEAPID_MONOLITH, FADE_SUB_BG, 0x20, BMPWIN_FRAME_PALNO * 16, 0);
  
	mpw->bmpwin = GFL_BMPWIN_Create( GFL_BG_FRAME1_S,
		1,19,30,4, MONOLITH_BG_DOWN_FONT_PALNO, GFL_BMP_CHRAREA_GET_B );

	bmp = GFL_BMPWIN_GetBmp( mpw->bmpwin );
	
	GFL_BMP_Clear( bmp, 0xff );
//	GFL_BMPWIN_TransVramCharacter( mpw->bmpwin );
	GFL_BMPWIN_MakeScreen( mpw->bmpwin );
//	GFL_BG_LoadScreenReq( GFL_BG_FRAME1_S );
	
	BmpWinFrame_Write( mpw->bmpwin, WINDOW_TRANS_ON, BMPWIN_FRAME_START_CGX, BMPWIN_FRAME_PALNO );
}

//--------------------------------------------------------------
/**
 * BMPWIN�폜
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _Setup_BmpWinDelete(MONOLITH_PWSELECT_WORK *mpw)
{
  GFL_BMPWIN_Delete(mpw->bmpwin);
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�o�͊J�n
 *
 * @param   mpw		
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Set_MsgStream(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup, u16 msg_id)
{
  GF_ASSERT(mpw->print_stream == NULL);
  
  GFL_FONTSYS_SetColor( 1, 2, 15 );

  mpw->strbuf_stream = GFL_MSG_CreateString(setup->mm_monolith, msg_id);
  
  mpw->print_stream = PRINTSYS_PrintStream(
    mpw->bmpwin, 0, 0, mpw->strbuf_stream, setup->font_handle,
    MSGSPEED_GetWait(), setup->tcbl_sys, 10, HEAPID_MONOLITH, 0xf);

	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�o�͊����҂�
 *
 * @param   mpw		
 *
 * @retval  BOOL		TRUE:�o�͊���
 */
//--------------------------------------------------------------
static BOOL _Wait_MsgStream(MONOLITH_PWSELECT_WORK *mpw)
{
  if(PRINTSYS_PrintStreamGetState( mpw->print_stream ) == PRINTSTREAM_STATE_DONE ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�N���A
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _Clear_MsgStream(MONOLITH_PWSELECT_WORK *mpw)
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( mpw->bmpwin );

  GF_ASSERT(mpw->print_stream != NULL);
  PRINTSYS_PrintStreamDelete(mpw->print_stream);
  mpw->print_stream = NULL;
  
  GFL_STR_DeleteBuffer(mpw->strbuf_stream);
  mpw->strbuf_stream = NULL;
  
	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_OFF);
  GFL_BMP_Clear(bmp, 0xff);
	GFL_BMPWIN_TransVramCharacter( mpw->bmpwin );

  GFL_FONTSYS_SetColor( 
    MONOLITH_FONT_DEFCOLOR_LETTER, MONOLITH_FONT_DEFCOLOR_SHADOW, MONOLITH_FONT_DEFCOLOR_BACK );
}

//--------------------------------------------------------------
/**
 * �p���[�̏K���󋵂𒲂ׂ�
 *
 * @param   setup		
 * @param   gpower_id		G�p���[ID
 *
 * @retval  _USE_POWER		�K����
 */
//--------------------------------------------------------------
static _USE_POWER _CheckUsePower(MONOLITH_SETUP *setup, GPOWER_ID gpower_id)
{
  //��check�@�Ƃ肠���������_��
  return GFUser_GetPublicRand0(_USE_POWER_MAX);
}

//--------------------------------------------------------------
/**
 * G�p���[��BMP�쐬
 *
 * @param   mpw		
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_PowerNameBMPCreate(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup)
{
  GPOWER_ID gpower_id;
  _USE_POWER use_power;
  int use_count = 0;
  
  for(gpower_id = 0; gpower_id < GPOWER_ID_MAX; gpower_id++){
    use_power = _CheckUsePower(setup, gpower_id);
    if(use_power == _USE_POWER_NONE){
      continue;
    }
    
    mpw->use_gpower_id[use_count] = gpower_id;
    mpw->use_power[use_count] = use_power;
    mpw->bmp_power_name[use_count] = GFL_BMP_Create( 
      GPOWER_NAME_BMP_LEN_X, GPOWER_NAME_BMP_LEN_Y, GFL_BMP_16_COLOR, HEAPID_MONOLITH );
    
    use_count++;
  }
  
  mpw->power_list_num = use_count;
}

//--------------------------------------------------------------
/**
 * G�p���[��BMP�폜
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _Setup_PowerNameBMPDelete(MONOLITH_PWSELECT_WORK *mpw)
{
  int i;
  
  for(i = 0; i < mpw->power_list_num; i++){
    GFL_BMP_Delete(mpw->bmp_power_name[i]);
  }
}

//--------------------------------------------------------------
/**
 * G�p���[����BMP�`��
 *
 * @param   mpw		  
 *
 * @retval  BOOL		TRUE:�S�Ă�G�p���[���̕`�悪��������
 * @retval  BOOL    FALSE:�܂��r��
 *
 * TRUE���Ԃ�܂Ŗ��t���[���Ăё����Ă�������
 */
//--------------------------------------------------------------
static BOOL _PowerNameBMPDraw(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup)
{
  int i, count = 0;
  STRBUF *str;
  static const PRINTSYS_LSB print_color[] = {
    PRINTSYS_MACRO_LSB(0xf, 2, 0),
    PRINTSYS_MACRO_LSB(2, 1, 0),
  };
  
  str = GFL_STR_CreateBuffer(32, HEAPID_MONOLITH);
  for(i = mpw->power_list_write_num; i < mpw->power_list_num; i++){
    GF_ASSERT(mpw->use_power[i] < NELEMS(print_color));
    GFL_MSG_GetString(mpw->mm_power, mpw->use_gpower_id[i], str);
    PRINTSYS_PrintColor( mpw->bmp_power_name[i], 0, 0, str, 
      setup->font_handle, print_color[mpw->use_power[i]] );
    
    count++;
    if(count >= GPOWER_NAME_WRITE_NUM){
      break;
    }
  }
  GFL_STR_DeleteBuffer(str);
  
  mpw->power_list_write_num = i;
  if(mpw->power_list_write_num < mpw->power_list_num){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * BMPOAM���쐬
 *
 * @param   mpw		
 * @param   setup		
 */
//--------------------------------------------------------------
static void _BmpOamCreate(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup)
{
  int i;
  BMPOAM_ACT_DATA head = {
    NULL,
    _BMPOAM_POWER_NAME_X,
    0,
    0,
    0,
    SOFTPRI_BMPOAM_NAME,
    BGPRI_BMPOAM_NAME,
    CLSYS_DEFREND_SUB,
    CLSYS_DRAW_SUB,
  };
  
  head.pltt_index = setup->common_res[COMMON_RESOURCE_INDEX_DOWN].pltt_bmpfont_index;
  for(i = 0; i < POWER_ITEM_DISP_NUM; i++){
    mpw->bmpoam_power_bmp[i] = GFL_BMP_Create( 
      GPOWER_NAME_BMP_LEN_X, GPOWER_NAME_BMP_LEN_Y, GFL_BMP_16_COLOR, HEAPID_MONOLITH );
    head.bmp = mpw->bmpoam_power_bmp[i];
    mpw->bmpoam_power[i] = BmpOam_ActorAdd(setup->bmpoam_sys, &head);
  }
}

//--------------------------------------------------------------
/**
 * BMPOAM���폜
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _BmpOamDelete(MONOLITH_PWSELECT_WORK *mpw)
{
  int i;
  
  for(i = 0; i < POWER_ITEM_DISP_NUM; i++){
    BmpOam_ActorDel(mpw->bmpoam_power[i]);
    GFL_BMP_Delete(mpw->bmpoam_power_bmp[i]);
  }
}

//==================================================================
/**
 * �L�����Z���A�C�R������
 *
 * @param   appwk		
 * @param   mpw		
 */
//==================================================================
static void _CancelIconCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw)
{
  mpw->act_cancel = MonolithTool_CancelIcon_Create(appwk->setup);
}

//==================================================================
/**
 * �L�����Z���A�C�R���폜
 *
 * @param   mpw		
 */
//==================================================================
static void _CancelIconDelete(MONOLITH_PWSELECT_WORK *mpw)
{
  MonolithTool_CancelIcon_Delete(mpw->act_cancel);
}

//==================================================================
/**
 * �L�����Z���A�C�R���X�V����
 *
 * @param   mpw		
 */
//==================================================================
static void _CancelIconUpdate(MONOLITH_PWSELECT_WORK *mpw)
{
  MonolithTool_CancelIcon_Update(mpw->act_cancel);
}

//--------------------------------------------------------------
/**
 * �X�N���[�����x���Z�o����
 *
 * @param   mpw		
 *
 * @retval  BOOL		TRUE:����L�[���������@FALSE:����L�[�������Ă��Ȃ�
 */
//--------------------------------------------------------------
static BOOL _ScrollSpeedUpdate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw)
{
  BOOL tp_cont;
  u32 tp_x, tp_y;
  BOOL decide_on = FALSE;
  u16 cursor_pos = _CURSOR_POS_NONE;
  
  tp_cont = GFL_UI_TP_GetPointCont( &tp_x, &tp_y );
  if(mpw->tp_cont_frame == 0 && (tp_x < 24 || tp_x > 224)){
    tp_cont = FALSE;
  }
  
  if(mpw->tp_cont_frame == 0){
    //�����͂Ői��ł���ꍇ�ɐV���ȓ��͂�����Β�~����
    if(mpw->speed != 0){
      if(tp_cont == TRUE 
          ||(GFL_UI_KEY_GetTrg() & (PAD_KEY_UP|PAD_KEY_DOWN|PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL))){
        mpw->speed = 0;
      }
    }

    if(tp_cont == TRUE){
      mpw->tp_cont_frame = 1;
      mpw->tp_backup_y = tp_y;
      mpw->tp_first_hit_y = tp_y;
      _SetCursorPos(appwk, mpw, _CURSOR_POS_NONE, _SET_CURSOR_MODE_INIT);
    }
    else if(mpw->speed != 0){ //����
      if(mpw->speed > 0){
        mpw->speed -= _SCROLL_SLOW_DOWN;
        if(mpw->speed < 0){
          mpw->speed = 0;
        }
      }
      else{
        mpw->speed += _SCROLL_SLOW_DOWN;
        if(mpw->speed > 0){
          mpw->speed = 0;
        }
      }
    }
  }
  else{ //�^�b�`�p�l���������ςȂ��̏ꍇ�̑���
    if(tp_cont == FALSE){
      mpw->tp_cont_frame = 0;
      if(mpw->tp_first_hit_y != _TOUCH_DECIDE_NONE){
        if(MATH_ABS((s32)mpw->tp_backup_y - (s32)mpw->tp_first_hit_y) < _TOUCH_DECIDE_RANGE){
          s32 list_no = _GetTouchListNo(mpw, mpw->tp_backup_y);
          if(list_no != _LIST_NO_NONE){
            decide_on = TRUE;
            cursor_pos = list_no;
          }
        }
      }
    }
    else{
      mpw->speed = (mpw->tp_backup_y - (s32)tp_y) * 0x100;  //�}�C�i�X�L�̈�SHIFT�ł͂Ȃ��|���Z
      mpw->tp_backup_y = tp_y;
      mpw->tp_cont_frame++;
      if(mpw->tp_first_hit_y != _TOUCH_DECIDE_NONE){
        if(MATH_ABS((s32)tp_y - (s32)mpw->tp_first_hit_y) > _TOUCH_DECIDE_RANGE){
          mpw->tp_first_hit_y = _TOUCH_DECIDE_NONE;
        }
      }
    }
  }
  
  if(decide_on == TRUE){
    mpw->speed = 0;
    _SetCursorPos(appwk, mpw, cursor_pos, _SET_CURSOR_MODE_TP);
  }
  
  return decide_on;
}

//--------------------------------------------------------------
/**
 * �J�[�\���ʒu�ύX����
 *
 * @param   mpw		
 * @param   cursor_pos		�J�[�\���ʒu(���ڔԍ�) �����I���ɂ���ɂ� _CURSOR_POS_NONE
 */
//--------------------------------------------------------------
static void _SetCursorPos(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos, _SET_CURSOR_MODE mode)
{
  //�̂̃J�[�\���ʒu��BG�X�N���[�����t�F�[�h�����̃J���[No�ɕύX����
  _CursorPos_NotFocus(mpw, mpw->cursor_pos);
  
  //���̃J�[�\���ʒu��BG�X�N���[�����t�F�[�h�L�̃J���[No�ɕύX����
  _CursorPos_PanelFocus(mpw, cursor_pos);
  
  if(cursor_pos == _CURSOR_POS_NONE){
    MonolithTool_PanelBG_Focus(appwk, FALSE, FADE_SUB_BG);
  }
  else if((mode == _SET_CURSOR_MODE_TP 
      && (cursor_pos == mpw->cursor_pos || cursor_pos == mpw->backup_cursor_pos))
      || (mode == _SET_CURSOR_MODE_KEY && cursor_pos == mpw->cursor_pos)){
    mpw->decide_cursor_pos = cursor_pos;
    MonolithTool_PanelBG_Flash(appwk, FADE_SUB_BG);
  }
  else{
    MonolithTool_PanelBG_Focus(appwk, TRUE, FADE_SUB_BG);
  }
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);

  mpw->backup_cursor_pos = mpw->cursor_pos;
  mpw->cursor_pos = cursor_pos;
}

//--------------------------------------------------------------
/**
 * �J�[�\���ʒu�̃p�l���̃X�N���[�����w��p���b�gNo�ɕύX����
 *
 * @param   cursor_pos		
 * @param   palno		
 */
//--------------------------------------------------------------
static void _CursorPos_PanelScreenChange(MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos, int palno)
{
  s32 list_top_no, list_space_pos, scr_y, item_offset, target_bg_y;
  
  if(cursor_pos == _CURSOR_POS_NONE){
    return;
  }
  
  //�擪�̍��ڔԍ�
  list_top_no = (mpw->list_y >> 8) / POWER_LIST_SPACE;
  list_space_pos = (mpw->list_y >> 8) % POWER_LIST_SPACE;
  scr_y = (mpw->list_y >> 8) % (256-192);
  item_offset = cursor_pos - list_top_no;
  
  if(item_offset < 0 || item_offset >= list_top_no + POWER_ITEM_DISP_NUM){
    return; //��ʊO
  }
  
  target_bg_y = (scr_y + POWER_LIST_SPACE * item_offset - list_space_pos ) % 256;
  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S, _PANEL_LEFT_POS, target_bg_y/8, 
    _PANEL_RIGHT_POS - _PANEL_LEFT_POS, _PANEL_SIZE_Y, palno );
}

//--------------------------------------------------------------
/**
 * �J�[�\���ʒu�̃p�l�����t�H�[�J�X��Ԃɂ���
 *
 * @param   cursor_pos		
 */
//--------------------------------------------------------------
static void _CursorPos_PanelFocus(MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos)
{
  _CursorPos_PanelScreenChange(mpw, cursor_pos, COMMON_PALBG_PANEL_FOCUS);
}

//--------------------------------------------------------------
/**
 * �J�[�\���ʒu�̃p�l�����t�H�[�J�X������Ԃɂ���
 *
 * @param   cursor_pos		
 */
//--------------------------------------------------------------
static void _CursorPos_NotFocus(MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos)
{
  _CursorPos_PanelScreenChange(mpw, cursor_pos, COMMON_PALBG_PANEL);
}

//--------------------------------------------------------------
/**
 * ���X�g���W�X�V����
 *
 * @param   mpw		
 * @param   add_y		����8�r�b�g����(�}�C�i�X�L)
 * 
 * @retval  TRUE:���W�̕ω����������x���Ŕ�������
 */
//--------------------------------------------------------------
static BOOL _ScrollPosUpdate(MONOLITH_PWSELECT_WORK *mpw, s32 add_y)
{
  s32 max_y, backup_y;
  
  backup_y = mpw->list_y;
  
  max_y = mpw->power_list_num * POWER_LIST_SPACE;
  if(max_y <= 192){
    return FALSE; //�X�N���[��������K�v����
  }
  
  mpw->list_y += add_y;
  if(mpw->list_y < 0){
    mpw->list_y = 0;
  }
  else if(mpw->list_y > ((max_y - 192) << 8)){
    mpw->list_y = (max_y - 192) << 8;
  }
  
  if((mpw->list_y >> 8) != (backup_y >> 8)){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���X�g���W��BG�̍��W�֔��f����
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _ScrollPos_BGReflection(MONOLITH_PWSELECT_WORK *mpw)
{
  s32 scr_y;
  
  scr_y = (mpw->list_y >> 8) % (256-192);
  GFL_BG_SetScrollReq( GFL_BG_FRAME2_S, GFL_BG_SCROLL_Y_SET, scr_y );
}

//--------------------------------------------------------------
/**
 * ���X�g���W���p���[��BMPOAM�֔��f����
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _ScrollPos_NameOamReflection(MONOLITH_PWSELECT_WORK *mpw)
{
  s32 list_space_pos;
  s32 oam_top_y, list_top_no;
  int i;
  
  //���X�g���W����擪�ɂȂ�OAM��Y���W���Z�o����
  list_space_pos = (mpw->list_y >> 8) % POWER_LIST_SPACE;
  oam_top_y = _BMPOAM_POWER_NAME_OFFSET_Y - list_space_pos;
  
  //���X�g���W����擪�ɂȂ鍀�ڔԍ����Z�o����
  list_top_no = (mpw->list_y >> 8) / POWER_LIST_SPACE;
  
  //�擪���珇�ɖ��OBMP���㏑���R�s�[����
  for(i = 0; i < POWER_ITEM_DISP_NUM; i++){
    BmpOam_ActorSetPos(mpw->bmpoam_power[i], 
      _BMPOAM_POWER_NAME_X, oam_top_y + POWER_LIST_SPACE * i);
    if(mpw->bmp_power_name[list_top_no + i] != NULL){
      GFL_BMP_Copy( mpw->bmp_power_name[list_top_no + i], mpw->bmpoam_power_bmp[i] );
    }
    //�]����V�u�����N�ōs��
  }
}

//--------------------------------------------------------------
/**
 * �p���[�I����ʁFV�u�����NTCB
 *
 * @param   tcb		
 * @param   work		
 */
//--------------------------------------------------------------
static void _PowerSelect_VBlank(GFL_TCB *tcb, void *work)
{
  MONOLITH_PWSELECT_WORK *mpw = work;
  int i;
  
  if(mpw->scroll_update == TRUE){
    for(i = 0; i < POWER_ITEM_DISP_NUM; i++){
      BmpOam_ActorBmpTrans(mpw->bmpoam_power[i]);
    }
    mpw->scroll_update = FALSE;
  }
}

//--------------------------------------------------------------
/**
 * �^�b�`���W�ƃ��X�g���W����^�b�`��������No���擾����
 *
 * @param   mpw		
 * @param   tp_y		�^�b�`���WY
 *
 * @retval  s32		����No(�����ȏꍇ��_LIST_NO_NONE)
 */
//--------------------------------------------------------------
static s32 _GetTouchListNo(MONOLITH_PWSELECT_WORK *mpw, u32 tp_y)
{
  s32 list_no;
  
  list_no = ((mpw->list_y >> 8) + tp_y) / POWER_LIST_SPACE;
  if(list_no >= mpw->power_list_num){
    return _LIST_NO_NONE;
  }
  return list_no;
}
