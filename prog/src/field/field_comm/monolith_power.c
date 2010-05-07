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
#include "app_menu_common.naix"
#include "app/app_menu_common.h"
#include "gamesystem/game_comm.h"
#include "monolith_snd_def.h"


//==============================================================================
//  �萔��`
//==============================================================================
///BMP�g�̃p���b�g�ԍ�
#define BMPWIN_FRAME_PALNO        (MONOLITH_MENUBAR_PALNO - 1)
///BMP�g�̃L�����N�^�J�n�ʒu
#define BMPWIN_FRAME_START_CGX    (1)

enum{
  _BMPWIN_TALKWIN,    ///<��b�E�B���h�E
  _BMPWIN_TITLE,      ///<�^�C�g��
  
  _BMPWIN_MAX,
};

enum{
  _PRINTUTIL_TITLE,
  
  _PRINTUTIL_MAX,
};


//--------------------------------------------------------------
//  
//--------------------------------------------------------------
///G�p���[���p�Ɋm�ۂ���BMP��X(�L�����N�^�P��)
#define GPOWER_NAME_BMP_LEN_X     (23)
///G�p���[���p�Ɋm�ۂ���BMP��Y(�L�����N�^�P��)
#define GPOWER_NAME_BMP_LEN_Y     (2)

///1�t���[���ɕ`�悷��G�p���[���̐� (�L���[�ɒ��߂�ɂ͐�����������̂Ŏ����Ńt���[���𕪂��ĕ`��)
#define GPOWER_NAME_WRITE_NUM     (3)

///��x�ɉ�ʂɕ\�������p���[���ڐ�
#define POWER_ITEM_DISP_NUM   (7)
///�\���L�[�ňړ������鎞�ɁA�㉺�ɗ]�T���������ăX�N���[��������ׂ̃p�l����
#define POWER_ITEM_KEY_SCROLL_SPACE   (2)

enum{
  SOFTPRI_BMPOAM_NAME = 10,
};

enum{
  BGPRI_BMPOAM_HAVE_POINT = 1,
  BGPRI_BMPOAM_NAME = 3,
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

enum{
  _DUMMY_PANEL_UP_NUM = 1,    ///<��ɓ���_�~�[�p�l����
  _DUMMY_PANEL_DOWN_NUM = 1,  ///<���ɓ���_�~�[�p�l����
  _DUMMY_PANEL_NUM = 2,       ///<�㉺�ɓ���_�~�[�p�l����
};
///�^�C�g���o�[�̒�������p�l�����J�n����悤�ɂ���ׂ̃��X�gY����l
#define _LIST_Y_TOP_OFFSET         (8 << 8)
///���j���[�o�[�̐^��Ńp�l������~����悤�ɂ���ׂ̃��X�gY�����l
#define _LIST_Y_BOTTOM_OFFSET      (8 << 8)
///�^�C�g���o�[��Y��
#define _TITLE_BAR_Y_LEN          (8*3)
///���j���[�o�[��Y��
#define _MENU_BAR_Y_LEN           (8*3)

///�p�l���u���炤�vX���W
#define _PANEL_DECIDE_X   (64)
///�p�l���u���炤�vY���W
#define _PANEL_DECIDE_Y   (192 - _MENU_BAR_Y_LEN/2)

enum{
  _SCROLL_ARROW_X = 128,
  _SCROLL_ARROW_UP_Y = _TITLE_BAR_Y_LEN + 4,
  _SCROLL_ARROW_DOWN_Y = 192 - _MENU_BAR_Y_LEN - 4,
};


//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V����������ʐ��䃏�[�N
typedef struct{
  GFL_TCB *vintr_tcb;
  
  u32 alloc_menubar_pos;
  
  PANEL_ACTOR panel;
  GFL_CLWK *act_arrow_up;       ///<����
  GFL_CLWK *act_arrow_down;     ///<�����
  
  GFL_MSGDATA *mm_power;        ///<�p���[��gmm
  GFL_BMP_DATA *bmp_power_name[GPOWER_ID_MAX + _DUMMY_PANEL_NUM];
  GPOWER_ID use_gpower_id[GPOWER_ID_MAX + _DUMMY_PANEL_NUM];      ///<bmp_power_name�ɑΉ�����GPOWER_ID�������Ă���
  MONO_USE_POWER use_power[GPOWER_ID_MAX + _DUMMY_PANEL_NUM];         ///<use_gpower_id�̏K����
  u8 power_list_num;        ///<�p���[���X�g��
  u8 power_list_write_num;  ///<BMP�ɏ������񂾖��O�̐�
  
  GFL_BMPWIN *bmpwin[_BMPWIN_MAX];
  PRINT_UTIL print_util[_PRINTUTIL_MAX];
  STRBUF *strbuf_stream;
  PRINT_STREAM *print_stream;
  MONOLITH_CANCEL_ICON cancel_icon;   ///<�L�����Z���A�C�R���A�N�^�[�ւ̃|�C���^

  BMPOAM_ACT_PTR bmpoam_power[POWER_ITEM_DISP_NUM];
  GFL_BMP_DATA *bmpoam_power_bmp[POWER_ITEM_DISP_NUM];
  
  s32 list_y;    ///<����8�r�b�g����
  
  u32 tp_cont_frame;     ///<TRUE:�^�b�`�p�l���������ςȂ��ɂ��Ă���t���[����
  s32 speed;              ///<�X�N���[�����x(����8�r�b�g�����B�}�C�i�X�L)
  s32 one_speed;          ///<�X�N���[�����x(����8�r�b�g�����B�}�C�i�X�L) 1�t���[���ŏ���
  u32 tp_backup_y;        ///<1�t���[���O��TP_Y�l
  u32 tp_first_hit_y;     ///<�ŏ��Ƀ^�b�`��������Y�l
  
  BOOL scroll_update;     ///<TRUE:�X�N���[�����W�ɐ������x���ŕω���������
  
  s32 cursor_pos;         ///<�J�[�\���ʒu(����No)
  s32 backup_cursor_pos;  ///<1�O�̃J�[�\���ʒu
  s32 backup_tp_decide_pos; ///<�^�b�`�p�l�����蔻��p��1�O�̑I�����Ă����J�[�\���ʒu
  s32 decide_cursor_pos;  ///<���莞�̃J�[�\���ʒu
  BOOL cursor_pos_update;   ///<TRUE:�J�[�\���ʒu�ɍX�V��������
  _SET_CURSOR_MODE cursor_mode; ///<�J�[�\�����L�[�ƃ^�b�`�p�l���ǂ���ł���������
  
  APP_TASKMENU_RES *app_menu_res;
  APP_TASKMENU_WORK *app_menu_work;
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
static void _Setup_BGGraphicLoad(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup);
static void _Setup_BGGraphicUnload(MONOLITH_PWSELECT_WORK *mpw);
static void _BmpOamCreate(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup);
static void _BmpOamDelete(MONOLITH_PWSELECT_WORK *mpw);
static void _DecidePanelCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw);
static void _DecidePanelDelete(MONOLITH_PWSELECT_WORK *mpw);
static void _DecidePanelUpdate(MONOLITH_SETUP *setup, MONOLITH_PWSELECT_WORK *mpw);
static void _CancelIconCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw);
static void _CancelIconDelete(MONOLITH_PWSELECT_WORK *mpw);
static void _CancelIconUpdate(MONOLITH_PWSELECT_WORK *mpw);
static void _ScrollArrowCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw);
static void _ScrollArrowDelete(MONOLITH_PWSELECT_WORK *mpw);
static void _ScrollArrowUpdate(MONOLITH_PWSELECT_WORK *mpw);
static void _Setup_BmpWinCreate(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup);
static void _Setup_BmpWinDelete(MONOLITH_PWSELECT_WORK *mpw);
static void _Set_TitlePrint(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw);
static void _Set_MsgStream(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup, u16 msg_id);
static void _Set_MsgStreamExpand(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup, u16 msg_id);
static BOOL _Wait_MsgStream(MONOLITH_SETUP *setup, MONOLITH_PWSELECT_WORK *mpw);
static void _Clear_MsgStream(MONOLITH_PWSELECT_WORK *mpw);
static void _Setup_ScreenClear(MONOLITH_PWSELECT_WORK *mpw);
static void _Setup_PowerNameBMPCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup);
static void _Setup_PowerNameBMPDelete(MONOLITH_PWSELECT_WORK *mpw);
static BOOL _PowerNameBMPDraw(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup);
static BOOL _ScrollSpeedUpdate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw);
static void _ScrollBeforeUpdate(MONOLITH_PWSELECT_WORK *mpw);
static BOOL _SetCursorPos(MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos, _SET_CURSOR_MODE mode);
static void _ScrollAfterUpdate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw);
static BOOL _ScrollPosUpdate(MONOLITH_PWSELECT_WORK *mpw, s32 add_y);
static void _ScrollPos_BGReflection(MONOLITH_PWSELECT_WORK *mpw);
static void _ScrollPos_NameOamReflection(MONOLITH_PWSELECT_WORK *mpw);
static void _PowerSelect_VBlank(GFL_TCB *tcb, void *work);
static s32 _GetTouchListNo(MONOLITH_PWSELECT_WORK *mpw, u32 tp_y);
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
    mpw->list_y = _LIST_Y_TOP_OFFSET;
    mpw->decide_cursor_pos = _CURSOR_POS_NONE;
    mpw->backup_cursor_pos = _CURSOR_POS_NONE;
    mpw->backup_tp_decide_pos = _CURSOR_POS_NONE;

    MonolithTool_Panel_Init(appwk);
    
  	mpw->mm_power = GFL_MSG_Create(
  		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_power_dat, HEAPID_MONOLITH);

    //BG
    _Setup_BGFrameSetting();
    _Setup_BGGraphicLoad(mpw, appwk->setup);
    _Setup_BmpWinCreate(mpw, appwk->setup);
    _ScrollPos_BGReflection(mpw);
    //OBJ
    _CancelIconCreate(appwk, mpw);
    _BmpOamCreate(mpw, appwk->setup);
    _DecidePanelCreate(appwk, mpw);
    _ScrollArrowCreate(appwk, mpw);

    _Set_TitlePrint(appwk, mpw);

    mpw->app_menu_res = MonolithTool_TaskMenuCreate(
      appwk->setup, GFL_BG_FRAME0_S, HEAPID_MONOLITH);

    appwk->common->power_select_no = GPOWER_ID_NULL;
    
    mpw->vintr_tcb = GFUser_VIntr_CreateTCB(
      _PowerSelect_VBlank, mpw, MONOLITH_VINTR_TCB_PRI_POWER);

    OS_TPrintf("NameDraw start\n");
    _Setup_PowerNameBMPCreate(appwk, mpw, appwk->setup);
    _Setup_ScreenClear(mpw);
    (*seq)++;
    break;
  case 1:
    if(_PowerNameBMPDraw(mpw, appwk->setup) == TRUE){
      OS_TPrintf("NameDraw end\n");
      //���O�`��
      _ScrollPos_NameOamReflection(mpw);
      mpw->scroll_update = TRUE;

      if(GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY){
        _SetCursorPos(mpw, _DUMMY_PANEL_UP_NUM, _SET_CURSOR_MODE_INIT);
      }
      else{
        _SetCursorPos(mpw, _CURSOR_POS_NONE, _SET_CURSOR_MODE_INIT);
      }
      (*seq)++;
    }
    break;
  case 2:
    {
      int i;
      BOOL no_trans = FALSE;

      for(i = 0; i < _PRINTUTIL_MAX; i++){
        if(PRINT_UTIL_Trans(&mpw->print_util[i], appwk->setup->printQue) == FALSE){
          no_trans = TRUE;
        }
      }
      if(no_trans == FALSE){
      	GFL_BG_SetVisible(GFL_BG_FRAME0_S, VISIBLE_ON);
      	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_ON);
      	GFL_BG_SetVisible(GFL_BG_FRAME2_S, VISIBLE_ON);
      	GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
        return GFL_PROC_RES_FINISH;
      }
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
  int i;
  enum{
    SEQ_INIT,
    SEQ_FIRST_STREAM,
    SEQ_FIRST_STREAM_WAIT,
    SEQ_FIRST_STREAM_TP_RELEASE,
    SEQ_TOP,
    SEQ_YESNO_STREAM,
    SEQ_YESNO_STREAM_WAIT,
    SEQ_YESNO_SELECT_WAIT,
    SEQ_DECIDE_STREAM,
    SEQ_DECIDE_SEND,
    SEQ_DECIDE_STREAM_WAIT,
    SEQ_DECIDE_ME_FADEOUT_WAIT,
    SEQ_DECIDE_ME_PLAY,
    SEQ_DECIDE_ME_PLAY_WAIT,
    SEQ_DECIDE_ME_FADEIN_WAIT,
    SEQ_DECIDE_STREAM_KEY_WAIT,
    SEQ_TP_RELEASE_WAIT,
    SEQ_FINISH,
  };
  
  MonolithTool_Panel_ColorUpdate(appwk, FADE_SUB_BG);
  MonolithTool_Panel_ColorUpdate(appwk, FADE_SUB_OBJ);
  _CancelIconUpdate(mpw);
  _DecidePanelUpdate(appwk->setup, mpw);
  for(i = 0; i < _PRINTUTIL_MAX; i++){
    PRINT_UTIL_Trans(&mpw->print_util[i], appwk->setup->printQue);
  }

  if(appwk->force_finish == TRUE && (*seq) == SEQ_TOP){
    return GFL_PROC_RES_FINISH;
  }
  
  switch(*seq){
  case SEQ_INIT:
    *seq = SEQ_FIRST_STREAM;
    break;

  case SEQ_FIRST_STREAM:
    if(PRINTSYS_QUE_IsFinished( appwk->setup->printQue ) == TRUE){  //��check�@Que�̃p���b�g�J���[�����̂Ŏb��Ώ��@�t�H���g�̃J���[�w�肪�ʂɏo����悤�ɂȂ�Ύ������
      _Set_MsgStream(mpw, appwk->setup, msg_mono_pow_014);
      (*seq)++;
    }
    break;
  case SEQ_FIRST_STREAM_WAIT:
    if(_Wait_MsgStream(appwk->setup, mpw) == TRUE){
      if(GFL_UI_TP_GetTrg() || (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
        _Clear_MsgStream(mpw);
        PMSND_PlaySE(MONOLITH_SE_MSG);
        if(GFL_UI_TP_GetTrg() == FALSE){
          _SetCursorPos(mpw, _DUMMY_PANEL_UP_NUM, _SET_CURSOR_MODE_INIT);
        }
        else{
          _SetCursorPos(mpw, _CURSOR_POS_NONE, _SET_CURSOR_MODE_INIT);
        }
        (*seq)++;
      }
    }
    break;
  case SEQ_FIRST_STREAM_TP_RELEASE: //��������Ɏg�p�����L�[�ƃ^�b�`�����𗣂��̂�҂�
    if(GFL_UI_TP_GetCont() == FALSE 
        && (GFL_UI_KEY_GetCont() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL)) == 0){
      *seq = SEQ_TOP;
    }
    break;
    
  case SEQ_TOP:
    {
      int trg = GFL_UI_KEY_GetTrg();
      int tp_ret = GFL_UI_TP_HitTrg(TouchRect);

      if(tp_ret == TOUCH_CANCEL || (trg & PAD_BUTTON_CANCEL)){
        OS_TPrintf("�L�����Z���I��\n");
        MonolithTool_CancelIcon_FlashReq(&mpw->cancel_icon);
        if(tp_ret == TOUCH_CANCEL){
          GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
        }
        else{
          GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        }
        PMSND_PlaySE(MONOLITH_SE_CANCEL);
        (*seq) = SEQ_FINISH;
        break;
      }
      
      _ScrollSpeedUpdate(appwk, mpw);
      _ScrollBeforeUpdate(mpw);
      if(_ScrollPosUpdate(mpw, mpw->speed + mpw->one_speed) == TRUE){
        _ScrollPos_BGReflection(mpw);
        _ScrollPos_NameOamReflection(mpw);
        mpw->scroll_update = TRUE;
      }
      _ScrollAfterUpdate(appwk, mpw);
    }

    if(mpw->cursor_pos == _CURSOR_POS_NONE){
      appwk->common->power_select_no = GPOWER_ID_NULL;
      MonolithTool_PanelOBJ_SetEnable(&mpw->panel, FALSE);
    }
    else{
      appwk->common->power_select_no = mpw->use_gpower_id[mpw->cursor_pos];
      appwk->common->power_mono_use = mpw->use_power[mpw->cursor_pos];
      if(mpw->use_power[mpw->cursor_pos] == MONO_USE_POWER_OK){
        MonolithTool_PanelOBJ_SetEnable(&mpw->panel, TRUE);
      }
      else{
        MonolithTool_PanelOBJ_SetEnable(&mpw->panel, FALSE);
      }
    }
    
    if(mpw->decide_cursor_pos != _CURSOR_POS_NONE){
      PMSND_PlaySE( MONOLITH_SE_DECIDE );
      *seq = SEQ_YESNO_STREAM;
    }
    break;
  
  case SEQ_YESNO_STREAM:
    if(MonolithTool_PanelColor_GetMode(appwk, FADE_SUB_BG) != PANEL_COLORMODE_FLASH
        && MonolithTool_PanelColor_GetMode(appwk, FADE_SUB_OBJ) != PANEL_COLORMODE_FLASH){
      _Set_MsgStream(mpw, appwk->setup, msg_mono_pow_008);
      (*seq)++;
    }
    break;
  case SEQ_YESNO_STREAM_WAIT:
    if(_Wait_MsgStream(appwk->setup, mpw) == TRUE){
      GF_ASSERT(mpw->app_menu_work == NULL);
      mpw->app_menu_work = MonolithTool_TaskMenu_YesNoInit(
        appwk->setup, mpw->app_menu_res, HEAPID_MONOLITH);
      (*seq)++;
    }
    break;
  case SEQ_YESNO_SELECT_WAIT:
    {
      BOOL ret_yesno;
      if(MonolithTool_TaskMenu_Update(appwk->setup, GFL_BG_FRAME0_S, 
          mpw->app_menu_work, &ret_yesno) == TRUE){
        MonolithTool_TaskMenu_YesNoExit(mpw->app_menu_work);
        mpw->app_menu_work = NULL;
        _Clear_MsgStream(mpw);
        if(ret_yesno == TRUE){
          *seq = SEQ_DECIDE_STREAM;
        }
        else{
          mpw->decide_cursor_pos = _CURSOR_POS_NONE;
          MonolithTool_PanelBG_Focus(appwk, TRUE, FADE_SUB_BG);
          MonolithTool_PanelOBJ_Focus(appwk, &mpw->panel, 1, PANEL_NO_FOCUS, FADE_SUB_OBJ);
          *seq = SEQ_TOP;
        }
      }
    }
    break;
    
  case SEQ_DECIDE_STREAM:
    if(MonolithTool_PanelColor_GetMode(appwk, FADE_SUB_BG) != PANEL_COLORMODE_FLASH
        && MonolithTool_PanelColor_GetMode(appwk, FADE_SUB_OBJ) != PANEL_COLORMODE_FLASH){
      WORDSET_RegisterGPowerName( 
        appwk->setup->wordset, 0, mpw->use_gpower_id[mpw->decide_cursor_pos] );
      _Set_MsgStreamExpand(mpw, appwk->setup, msg_mono_pow_011);
      *seq = SEQ_DECIDE_SEND;
    }
    break;
  case SEQ_DECIDE_SEND:
    if(appwk->parent->intcomm != NULL){
      if(IntrudeSend_GPowerEquip(appwk->parent->palace_area) == TRUE){
        *seq = SEQ_DECIDE_STREAM_WAIT;
      }
    }
    else{
      *seq = SEQ_DECIDE_STREAM_WAIT;
    }
    break;
  case SEQ_DECIDE_STREAM_WAIT:
    if(_Wait_MsgStream(appwk->setup, mpw) == TRUE){
      PMSND_FadeOutBGM (PMSND_FADE_FAST);
      (*seq)++;
    }
    break;
  case SEQ_DECIDE_ME_FADEOUT_WAIT:
    if(PMSND_CheckFadeOnBGM() == FALSE){
      PMSND_PauseBGM(TRUE);
      PMSND_PushBGM();
      (*seq)++;
    }
    break;
  case SEQ_DECIDE_ME_PLAY:
    PMSND_PlayBGM( MONOLITH_ME_GPOWER_EQP );
    (*seq)++;
    break;
  case SEQ_DECIDE_ME_PLAY_WAIT:
    if( PMSND_CheckPlayBGM() == FALSE ){
      PMSND_PopBGM();
      PMSND_PauseBGM(FALSE);
      PMSND_FadeInBGM (PMSND_FADE_FAST);
      (*seq)++;
    }
    break;
  case SEQ_DECIDE_ME_FADEIN_WAIT:
    if(PMSND_CheckFadeOnBGM() == FALSE){
      *seq = SEQ_DECIDE_STREAM_KEY_WAIT;
    }
    break;
  case SEQ_DECIDE_STREAM_KEY_WAIT:
    if(GFL_UI_TP_GetTrg() || (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
      _Clear_MsgStream(mpw);
      {
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData(appwk->parent->gsys);
        INTRUDE_SAVE_WORK *intsave = SaveData_GetIntrude(GAMEDATA_GetSaveControlWork(gamedata));
        ISC_SAVE_SetGPowerID(intsave, mpw->use_gpower_id[mpw->decide_cursor_pos]);
      }
      {
        GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(appwk->parent->gsys);
        GameCommInfo_MessageEntry_GetPower(game_comm, 
          GAMEDATA_GetIntrudeMyID(GAMESYSTEM_GetGameData(appwk->parent->gsys)), 
          appwk->parent->palace_area);
      }
      appwk->common->power_eqp_update = TRUE;
      mpw->decide_cursor_pos = _CURSOR_POS_NONE;
      MonolithTool_PanelBG_Focus(appwk, TRUE, FADE_SUB_BG);
      MonolithTool_PanelOBJ_Focus(appwk, &mpw->panel, 1, PANEL_NO_FOCUS, FADE_SUB_OBJ);
      if(GFL_UI_TP_GetTrg()){
        *seq = SEQ_TP_RELEASE_WAIT;
      }
      else{
        *seq = SEQ_FINISH;
      }
    }
    break;
  case SEQ_TP_RELEASE_WAIT:
    if(GFL_UI_TP_GetCont() == 0){
      *seq = SEQ_FINISH;
    }
    break;
    
  case SEQ_FINISH:
    if(MonolithTool_PanelColor_GetMode(appwk, FADE_SUB_BG) != PANEL_COLORMODE_FLASH
        && MonolithTool_PanelColor_GetMode(appwk, FADE_SUB_OBJ) != PANEL_COLORMODE_FLASH
        && MonolithTool_CancelIcon_FlashCheck(&mpw->cancel_icon) == FALSE){
      appwk->next_menu_index = MONOLITH_MENU_TITLE;
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  _ScrollArrowUpdate(mpw);
  
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

  GFL_DISP_GXS_SetVisibleControlDirect(GX_PLANEMASK_BG3);

  GFL_TCB_DeleteTask( mpw->vintr_tcb );
  
  if(mpw->print_stream != NULL){
    PRINTSYS_PrintStreamDelete(mpw->print_stream);
  }
  if(mpw->strbuf_stream != NULL){
    GFL_STR_DeleteBuffer(mpw->strbuf_stream);
  }

  _Setup_PowerNameBMPDelete(mpw);

  if(mpw->app_menu_work != NULL){
    MonolithTool_TaskMenu_YesNoExit(mpw->app_menu_work);
  }
  MonolithTool_TaskMenuDelete(mpw->app_menu_res);

  //OBJ
  _DecidePanelDelete(mpw);
  _BmpOamDelete(mpw);
  _CancelIconDelete(mpw);
  _ScrollArrowDelete(mpw);
  //BG
  _Setup_BGGraphicUnload(mpw);
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
		{//GFL_BG_FRAME0_S
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x0c000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		{//GFL_BG_FRAME1_S
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x14000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
		{//GFL_BG_FRAME2_S
			0, 0, MONO_BG_COMMON_SCR_SIZE, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, MONO_BG_COMMON_CHARBASE, MONO_BG_COMMON_CHAR_SIZE,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( GFL_BG_FRAME0_S,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME1_S,   &bgcnt_frame[1],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME2_S,   &bgcnt_frame[2],   GFL_BG_MODE_TEXT );

	GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( GFL_BG_FRAME1_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( GFL_BG_FRAME2_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
}

//--------------------------------------------------------------
/**
 * BG�t���[�����
 */
//--------------------------------------------------------------
static void _Setup_BGFrameExit(void)
{
	GFL_BG_SetVisible(GFL_BG_FRAME0_S, VISIBLE_OFF);
	GFL_BG_SetVisible(GFL_BG_FRAME1_S, VISIBLE_OFF);
	GFL_BG_SetVisible(GFL_BG_FRAME2_S, VISIBLE_OFF);
  GFL_BG_FreeBGControl(GFL_BG_FRAME0_S);
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
static void _Setup_BGGraphicLoad(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup)
{
	GFL_ARCHDL_UTIL_TransVramScreen(setup->hdl, NARC_monolith_mono_bgd_power_lz_NSCR, 
		GFL_BG_FRAME2_S, 0, 0, TRUE, HEAPID_MONOLITH);
	
	GFL_ARCHDL_UTIL_TransVramBgCharacter(setup->hdl, NARC_monolith_mono_bgd_lz_NCGR, 
		GFL_BG_FRAME1_S, 0, 0, TRUE, HEAPID_MONOLITH);  //�L�����x�[�X�𕪂��Ă���̂�
	GFL_ARCHDL_UTIL_TransVramScreen(setup->hdl, NARC_monolith_mono_bgd_title_lz_NSCR, 
		GFL_BG_FRAME1_S, 0, 0, TRUE, HEAPID_MONOLITH);

	//���j���[�o�[  ���Ƀp�l����ʂ��ׁAcommon�����łȂ�����BG�ʂɂ����j���[�o�[���Z�b�g
  {
    ARCHANDLE *app_handle = GFL_ARC_OpenDataHandle(ARCID_APP_MENU_COMMON, HEAPID_MONOLITH);
  
    mpw->alloc_menubar_pos = GFL_BG_AllocCharacterArea(
      GFL_BG_FRAME1_S, MENUBAR_CGX_SIZE, GFL_BG_CHRAREA_GET_B );
    GFL_ARCHDL_UTIL_TransVramBgCharacter(
      app_handle, NARC_app_menu_common_menu_bar_NCGR, GFL_BG_FRAME1_S, mpw->alloc_menubar_pos, 
      MENUBAR_CGX_SIZE, FALSE, HEAPID_MONOLITH);
    
    APP_COMMON_MenuBarScrn_Fusion(app_handle, GFL_BG_FRAME1_S, HEAPID_MONOLITH, 
      mpw->alloc_menubar_pos, MONOLITH_MENUBAR_PALNO);
  
  	GFL_ARC_CloseDataHandle(app_handle);
  	GFL_BG_LoadScreenReq( GFL_BG_FRAME1_S );
  }
	
	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
}

//--------------------------------------------------------------
/**
 * BG�O���t�B�b�N���
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _Setup_BGGraphicUnload(MONOLITH_PWSELECT_WORK *mpw)
{
  GFL_BG_FreeCharacterArea( GFL_BG_FRAME1_S, mpw->alloc_menubar_pos, MENUBAR_CGX_SIZE );
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

  BmpWinFrame_CgxSet( GFL_BG_FRAME0_S, BMPWIN_FRAME_START_CGX, MENU_TYPE_SYSTEM, HEAPID_MONOLITH );
  PaletteWorkSetEx_Arc(setup->pfd, ARCID_FLDMAP_WINFRAME, BmpWinFrame_WinPalArcGet(MENU_TYPE_SYSTEM), 
    HEAPID_MONOLITH, FADE_SUB_BG, 0x20, BMPWIN_FRAME_PALNO * 16, 0);
  
	mpw->bmpwin[_BMPWIN_TALKWIN] = GFL_BMPWIN_Create( GFL_BG_FRAME0_S,
		1,19,30,4, MONOLITH_BG_DOWN_FONT_PALNO, GFL_BMP_CHRAREA_GET_B );

	bmp = GFL_BMPWIN_GetBmp( mpw->bmpwin[_BMPWIN_TALKWIN] );
	
	GFL_BMP_Clear( bmp, 0xff );
//	GFL_BMPWIN_TransVramCharacter( mpw->bmpwin[_BMPWIN_TALKWIN] );
//	GFL_BMPWIN_MakeScreen( mpw->bmpwin[_BMPWIN_TALKWIN] );
//	GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	
	mpw->bmpwin[_BMPWIN_TITLE] = GFL_BMPWIN_Create( GFL_BG_FRAME0_S,
		0,0,32,3, MONOLITH_BG_DOWN_FONT_PALNO, GFL_BMP_CHRAREA_GET_B );
	GFL_BMPWIN_MakeScreen( mpw->bmpwin[_BMPWIN_TITLE] );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	PRINT_UTIL_Setup(&mpw->print_util[_PRINTUTIL_TITLE], mpw->bmpwin[_BMPWIN_TITLE]);
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
  int i;
  for(i = 0; i < _BMPWIN_MAX; i++){
    GFL_BMPWIN_Delete(mpw->bmpwin[i]);
  }
}

//--------------------------------------------------------------
/**
 * �^�C�g���`��
 *
 * @param   appwk		
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _Set_TitlePrint(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw)
{
  STRBUF *str_msg;
  
  str_msg = GFL_MSG_CreateString(appwk->setup->mm_monolith, msg_mono_pow_010);

  PRINT_UTIL_Print(&mpw->print_util[_PRINTUTIL_TITLE], appwk->setup->printQue, 
    24, 4, str_msg, appwk->setup->font_handle);
  
  GFL_STR_DeleteBuffer(str_msg);
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
    mpw->bmpwin[_BMPWIN_TALKWIN], 0, 0, mpw->strbuf_stream, setup->font_handle,
    MSGSPEED_GetWait(), setup->tcbl_sys, 10, HEAPID_MONOLITH, 0xf);

	GFL_BMPWIN_MakeScreen( mpw->bmpwin[_BMPWIN_TALKWIN] );
	BmpWinFrame_Write( mpw->bmpwin[_BMPWIN_TALKWIN], 
	  WINDOW_TRANS_ON_V, BMPWIN_FRAME_START_CGX, BMPWIN_FRAME_PALNO );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�o�͊J�n(WORDSET���ꂽ���̂�EXPAND���ďo��)
 *
 * @param   mpw		
 * @param   setup		
 * @param   u16     msg_id
 *
 * WORDSET�p�̒P��͊O���ŃZ�b�g���Ă�������
 */
//--------------------------------------------------------------
static void _Set_MsgStreamExpand(MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup, u16 msg_id)
{
  STRBUF *str_temp;
  
  GF_ASSERT(mpw->print_stream == NULL);
  
  GFL_FONTSYS_SetColor( 1, 2, 15 );

  str_temp = GFL_MSG_CreateString(setup->mm_monolith, msg_id);
  mpw->strbuf_stream = GFL_STR_CreateBuffer(256, HEAPID_MONOLITH);
  
  WORDSET_ExpandStr( setup->wordset, mpw->strbuf_stream, str_temp );
  GFL_STR_DeleteBuffer(str_temp);
  
  mpw->print_stream = PRINTSYS_PrintStream(
    mpw->bmpwin[_BMPWIN_TALKWIN], 0, 0, mpw->strbuf_stream, setup->font_handle,
    MSGSPEED_GetWait(), setup->tcbl_sys, 10, HEAPID_MONOLITH, 0xf);

	GFL_BMPWIN_MakeScreen( mpw->bmpwin[_BMPWIN_TALKWIN] );
	BmpWinFrame_Write( mpw->bmpwin[_BMPWIN_TALKWIN], 
	  WINDOW_TRANS_ON_V, BMPWIN_FRAME_START_CGX, BMPWIN_FRAME_PALNO );
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
static BOOL _Wait_MsgStream(MONOLITH_SETUP *setup, MONOLITH_PWSELECT_WORK *mpw)
{
  return APP_PRINTSYS_COMMON_PrintStreamFunc( &setup->app_printsys, mpw->print_stream );
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
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( mpw->bmpwin[_BMPWIN_TALKWIN] );

  GF_ASSERT(mpw->print_stream != NULL);
  PRINTSYS_PrintStreamDelete(mpw->print_stream);
  mpw->print_stream = NULL;
  
  GFL_STR_DeleteBuffer(mpw->strbuf_stream);
  mpw->strbuf_stream = NULL;

  BmpWinFrame_Clear( mpw->bmpwin[_BMPWIN_TALKWIN], WINDOW_TRANS_ON );
  
  GFL_BMP_Clear(bmp, 0xff);
	GFL_BMPWIN_TransVramCharacter( mpw->bmpwin[_BMPWIN_TALKWIN] );

  GFL_FONTSYS_SetColor( 
    MONOLITH_FONT_DEFCOLOR_LETTER, MONOLITH_FONT_DEFCOLOR_SHADOW, MONOLITH_FONT_DEFCOLOR_BACK );
}

//--------------------------------------------------------------
/**
 * ���X�g����ʓ��Ɏ��܂��Ă���ꍇ�A�s�K�v�ȃX�N���[�����N���A����
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _Setup_ScreenClear(MONOLITH_PWSELECT_WORK *mpw)
{
  int start_y;
  
  if(mpw->power_list_num - _DUMMY_PANEL_NUM >= POWER_ITEM_DISP_NUM){
    return;
  }
  
  start_y = (mpw->power_list_num - _DUMMY_PANEL_DOWN_NUM) * POWER_LIST_SPACE / 8;
  GFL_BG_FillScreen( GFL_BG_FRAME2_S, 0, 0, start_y, 32, 32 - start_y, GFL_BG_SCRWRT_PALIN );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );
}

//--------------------------------------------------------------
/**
 * G�p���[��BMP�쐬
 *
 * @param   mpw		
 * @param   setup		
 */
//--------------------------------------------------------------
static void _Setup_PowerNameBMPCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw, MONOLITH_SETUP *setup)
{
  GPOWER_ID gpower_id;
  MONO_USE_POWER use_power;
  int use_count = 0;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(appwk->parent->gsys);
  const OCCUPY_INFO *occupy = MonolithTool_GetOccupyInfo(appwk);
  
  mpw->use_gpower_id[use_count] = 0;  //��ʏ㕔�̌����Ȃ��p�l��
  mpw->use_power[use_count] = MONO_USE_POWER_SOMEMORE;
  mpw->bmp_power_name[use_count] = GFL_BMP_Create( 
    GPOWER_NAME_BMP_LEN_X, GPOWER_NAME_BMP_LEN_Y, GFL_BMP_16_COLOR, HEAPID_MONOLITH );
  use_count++;
  for(gpower_id = 0; gpower_id < GPOWER_ID_MAX; gpower_id++){
    use_power = MonolithTool_CheckUsePower(setup, gpower_id, occupy, &appwk->parent->monolith_status);
    if(use_power == MONO_USE_POWER_NONE){
      continue;
    }
    
    mpw->use_gpower_id[use_count] = gpower_id;
    mpw->use_power[use_count] = use_power;
    mpw->bmp_power_name[use_count] = GFL_BMP_Create( 
      GPOWER_NAME_BMP_LEN_X, GPOWER_NAME_BMP_LEN_Y, GFL_BMP_16_COLOR, HEAPID_MONOLITH );
    
    use_count++;
  }
  mpw->use_gpower_id[use_count] = 0;  //��ʉ����̌����Ȃ��p�l��
  mpw->use_power[use_count] = MONO_USE_POWER_SOMEMORE;
  mpw->bmp_power_name[use_count] = GFL_BMP_Create( 
    GPOWER_NAME_BMP_LEN_X, GPOWER_NAME_BMP_LEN_Y, GFL_BMP_16_COLOR, HEAPID_MONOLITH );
  use_count++;

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
    if(i >= _DUMMY_PANEL_UP_NUM && i < mpw->power_list_num - _DUMMY_PANEL_DOWN_NUM){
      GF_ASSERT(mpw->use_power[i] < NELEMS(print_color));
      GFL_MSG_GetString(mpw->mm_power, mpw->use_gpower_id[i], str);
      PRINTSYS_PrintColor( mpw->bmp_power_name[i], 0, 0, str, 
        setup->font_handle, print_color[mpw->use_power[i]] );
    }
    
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
 * �u���炤�v�p�l������
 *
 * @param   appwk		
 * @param   mpw		
 */
//==================================================================
static void _DecidePanelCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw)
{
  MonolithTool_PanelOBJ_Create(appwk->setup, &mpw->panel, 
    COMMON_RESOURCE_INDEX_DOWN, PANEL_SIZE_DECIDE, 
    _PANEL_DECIDE_X, _PANEL_DECIDE_Y, msg_mono_pow_012, NULL);
//  MonolithTool_PanelOBJ_Focus(appwk, &mpw->panel, 1, 0, FADE_SUB_OBJ);
}

//==================================================================
/**
 * �u���炤�v�p�l���폜
 *
 * @param   mpw		
 */
//==================================================================
static void _DecidePanelDelete(MONOLITH_PWSELECT_WORK *mpw)
{
  MonolithTool_PanelOBJ_Delete(&mpw->panel);
}

//==================================================================
/**
 * �u���炤�v�p�l���X�V����
 *
 * @param   mpw		
 */
//==================================================================
static void _DecidePanelUpdate(MONOLITH_SETUP *setup, MONOLITH_PWSELECT_WORK *mpw)
{
  MonolithTool_PanelOBJ_TransUpdate(setup, &mpw->panel);
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
  MonolithTool_CancelIcon_Create(appwk->setup, &mpw->cancel_icon);
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
  MonolithTool_CancelIcon_Delete(&mpw->cancel_icon);
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
  MonolithTool_CancelIcon_Update(&mpw->cancel_icon);
}

//==================================================================
/**
 * ���㉺�A�N�^�[����
 *
 * @param   appwk		
 * @param   mpw		
 */
//==================================================================
static void _ScrollArrowCreate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw)
{
  mpw->act_arrow_up = MonolithTool_Arrow_Create(
    appwk->setup, _SCROLL_ARROW_X, _SCROLL_ARROW_UP_Y, COMMON_ANMSEQ_ARROW_UP);
  mpw->act_arrow_down = MonolithTool_Arrow_Create(
    appwk->setup, _SCROLL_ARROW_X, _SCROLL_ARROW_DOWN_Y, COMMON_ANMSEQ_ARROW_DOWN);
  GFL_CLACT_WK_SetDrawEnable( mpw->act_arrow_up, FALSE );
  GFL_CLACT_WK_SetDrawEnable( mpw->act_arrow_down, FALSE );
}

//==================================================================
/**
 * ���㉺�A�N�^�[�폜
 *
 * @param   mpw		
 */
//==================================================================
static void _ScrollArrowDelete(MONOLITH_PWSELECT_WORK *mpw)
{
  MonolithTool_ArrowIcon_Delete(mpw->act_arrow_up);
  MonolithTool_ArrowIcon_Delete(mpw->act_arrow_down);
}

//==================================================================
/**
 * ���㉺�A�N�^�[�X�V����
 *
 * @param   mpw		
 */
//==================================================================
static void _ScrollArrowUpdate(MONOLITH_PWSELECT_WORK *mpw)
{
  if(mpw->list_y <= _LIST_Y_TOP_OFFSET){
    GFL_CLACT_WK_SetDrawEnable( mpw->act_arrow_up, FALSE );
  }
  else{
    GFL_CLACT_WK_SetDrawEnable( mpw->act_arrow_up, TRUE );
  }
//  if((mpw->list_y >> 8) >= POWER_LIST_SPACE * mpw->power_list_num - _MENU_BAR_Y_LEN - 192){
  if((mpw->list_y >> 8) >= mpw->power_list_num * POWER_LIST_SPACE - (_LIST_Y_BOTTOM_OFFSET >> 8) - 192){
    GFL_CLACT_WK_SetDrawEnable( mpw->act_arrow_down, FALSE );
  }
  else{
    GFL_CLACT_WK_SetDrawEnable( mpw->act_arrow_down, TRUE );
  }
  
  MonolithTool_ArrowIcon_Update(mpw->act_arrow_up);
  MonolithTool_ArrowIcon_Update(mpw->act_arrow_down);
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
  s32 cursor_pos = _CURSOR_POS_NONE;
  
  mpw->one_speed = 0;
  
  tp_cont = GFL_UI_TP_GetPointCont( &tp_x, &tp_y );
  if(mpw->tp_cont_frame == 0 && (tp_x < 24 || tp_x > 224)){
    tp_cont = FALSE;
  }
  
  if(GFL_UI_KEY_GetCont() & (PAD_KEY_UP|PAD_KEY_DOWN|PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL)){
    s32 list_top_no = ((mpw->list_y >> 8) + _TITLE_BAR_Y_LEN) / POWER_LIST_SPACE;
    s32 list_space_pos = ((mpw->list_y >> 8) + _TITLE_BAR_Y_LEN) % POWER_LIST_SPACE;
    //�����͂Ői��ł���ꍇ�ɐV���ȓ��͂�����Β�~����
    mpw->speed = 0;
    mpw->tp_cont_frame = 0;
    tp_cont = FALSE;
    if(mpw->cursor_pos == _CURSOR_POS_NONE){
      s32 set_pos = list_top_no;
      if(list_space_pos != 0){
        set_pos++;
      }
      if(set_pos < _DUMMY_PANEL_UP_NUM){
        set_pos = _DUMMY_PANEL_UP_NUM;
      }
      if(set_pos >= mpw->power_list_num - 1 - _DUMMY_PANEL_DOWN_NUM){
        _SetCursorPos(mpw, mpw->power_list_num - 1 - _DUMMY_PANEL_DOWN_NUM, _SET_CURSOR_MODE_KEY);
      }
      else{
        _SetCursorPos(mpw, set_pos, _SET_CURSOR_MODE_KEY);
      }
      PMSND_PlaySE(MONOLITH_SE_SELECT);
      return FALSE;
    }
    else{
      if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
        _SetCursorPos(mpw, mpw->cursor_pos, _SET_CURSOR_MODE_KEY);
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        PMSND_PlaySE(MONOLITH_SE_DECIDE);
        return TRUE;
      }
      else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
        s32 check_topno = list_top_no;
        if(list_space_pos != 0){
          check_topno++;
        }
        if(_SetCursorPos(mpw, mpw->cursor_pos - 1, _SET_CURSOR_MODE_KEY) == TRUE){
          PMSND_PlaySE(MONOLITH_SE_SELECT);
        }
        if(mpw->cursor_pos < check_topno){
          mpw->one_speed = -(POWER_LIST_SPACE << 8);
        }
        return FALSE;
      }
      else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
        s32 check_bottomno = ((mpw->list_y>>8) + 192-_MENU_BAR_Y_LEN) / POWER_LIST_SPACE;
        s32 bottom_list_space = ((mpw->list_y>>8) + 192-_MENU_BAR_Y_LEN) % POWER_LIST_SPACE;
        if(bottom_list_space != 0){
          //check_bottomno--;
        }
        if(_SetCursorPos(mpw, mpw->cursor_pos + 1, _SET_CURSOR_MODE_KEY) == TRUE){
          PMSND_PlaySE(MONOLITH_SE_SELECT);
        }
        if(mpw->cursor_pos >= check_bottomno){
          mpw->one_speed = POWER_LIST_SPACE << 8;
        }
        return FALSE;
      }
    }
  }
  
  if(mpw->tp_cont_frame == 0){
  #if 0
    //�����͂Ői��ł���ꍇ�ɐV���ȓ��͂�����Β�~����
    if(mpw->speed != 0){
      if(tp_cont == TRUE 
          ||(GFL_UI_KEY_GetTrg() & (PAD_KEY_UP|PAD_KEY_DOWN|PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL))){
        mpw->speed = 0;
      }
    }
  #endif

    if(tp_cont == TRUE){
      if(tp_y > _TITLE_BAR_Y_LEN && tp_y < 192-_MENU_BAR_Y_LEN){
        mpw->tp_cont_frame = 1;
        mpw->tp_backup_y = tp_y;
        mpw->tp_first_hit_y = tp_y;
        _SetCursorPos(mpw, _CURSOR_POS_NONE, _SET_CURSOR_MODE_INIT);
      }
      //�u���炤�v�^�b�`�`�F�b�N
      else if(GFL_UI_TP_GetTrg() == TRUE && mpw->cursor_pos != _CURSOR_POS_NONE
          && tp_x >= _PANEL_DECIDE_X-PANEL_DECIDE_CHARSIZE_X/2*8 
          && tp_x <= _PANEL_DECIDE_X+PANEL_DECIDE_CHARSIZE_X/2*8 
          && tp_y >= _PANEL_DECIDE_Y-PANEL_CHARSIZE_Y/2*8
          && tp_y <= _PANEL_DECIDE_Y+PANEL_CHARSIZE_Y/2*8){
        decide_on = TRUE;
        cursor_pos = mpw->cursor_pos;
        GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
      }
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
            PMSND_PlaySE(MONOLITH_SE_SELECT);
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
    _SetCursorPos(mpw, cursor_pos, _SET_CURSOR_MODE_TP);
  }
  
  return decide_on;
}

//--------------------------------------------------------------
/**
 * �J�[�\���ʒu�ύX����
 *
 * @param   mpw		
 * @param   cursor_pos		�J�[�\���ʒu(���ڔԍ�) �����I���ɂ���ɂ� _CURSOR_POS_NONE
 *
 * @retval  TRUE:�J�[�\���ʒu��ύX�����@FALSE:�J�[�\���ʒu��ύX���Ȃ�����
 */
//--------------------------------------------------------------
static BOOL _SetCursorPos(MONOLITH_PWSELECT_WORK *mpw, s32 cursor_pos, _SET_CURSOR_MODE mode)
{
  if(cursor_pos != _CURSOR_POS_NONE 
      && (cursor_pos >= mpw->power_list_num - _DUMMY_PANEL_DOWN_NUM 
      || cursor_pos < _DUMMY_PANEL_UP_NUM)){
    return FALSE;
  }

  mpw->backup_cursor_pos = mpw->cursor_pos;
#if 0 //�_�u���^�b�`�Ō���͖����ɂ���
  if(mpw->cursor_pos != _CURSOR_POS_NONE){
    mpw->backup_tp_decide_pos = mpw->cursor_pos;
  }
#else
  mpw->backup_tp_decide_pos = mpw->cursor_pos;
#endif
  mpw->cursor_pos = cursor_pos;
  mpw->cursor_pos_update = TRUE;
  mpw->cursor_mode = mode;
  return TRUE;
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
 * BG�X�N���[���J�n�O�Ɏ��s����K�v�����鏈��
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _ScrollBeforeUpdate(MONOLITH_PWSELECT_WORK *mpw)
{
  if(mpw->cursor_pos_update == TRUE){
    //�̂̃J�[�\���ʒu��BG�X�N���[�����t�F�[�h�����̃J���[No�ɕύX����
    _CursorPos_NotFocus(mpw, mpw->backup_cursor_pos);
  }
}

//--------------------------------------------------------------
/**
 * BG�X�N���[����Ɏ��s����K�v�����鏈��
 *
 * @param   mpw		
 */
//--------------------------------------------------------------
static void _ScrollAfterUpdate(MONOLITH_APP_PARENT *appwk, MONOLITH_PWSELECT_WORK *mpw)
{
  if(mpw->cursor_pos_update == TRUE){
    //���̃J�[�\���ʒu��BG�X�N���[�����t�F�[�h�L�̃J���[No�ɕύX����
    _CursorPos_PanelFocus(mpw, mpw->cursor_pos);
  
    if(mpw->cursor_pos == _CURSOR_POS_NONE){
      MonolithTool_PanelBG_Focus(appwk, FALSE, FADE_SUB_BG);
    }
    else if((mpw->cursor_mode == _SET_CURSOR_MODE_TP 
            && mpw->cursor_pos == mpw->backup_tp_decide_pos)
        || (mpw->cursor_mode == _SET_CURSOR_MODE_KEY 
            && mpw->cursor_pos == mpw->backup_cursor_pos)){
      if(mpw->use_power[mpw->cursor_pos] == MONO_USE_POWER_OK){
        mpw->decide_cursor_pos = mpw->cursor_pos;
        if(mpw->cursor_mode == _SET_CURSOR_MODE_KEY){
          MonolithTool_PanelBG_Flash(appwk, FADE_SUB_BG);
        }
        else{
          MonolithTool_PanelOBJ_Flash(appwk, &mpw->panel, 1, 0, FADE_SUB_OBJ);
        }
      }
      else{
        if(mpw->cursor_mode == _SET_CURSOR_MODE_TP){
          MonolithTool_PanelBG_Focus(appwk, TRUE, FADE_SUB_BG);
        }
      }
    }
    else{
      MonolithTool_PanelBG_Focus(appwk, TRUE, FADE_SUB_BG);
    }
    GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);
    
    mpw->cursor_pos_update = FALSE;
  }
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
  
  max_y = mpw->power_list_num * POWER_LIST_SPACE - (_LIST_Y_BOTTOM_OFFSET >> 8);
  if(max_y <= 192){
    return FALSE; //�X�N���[��������K�v����
  }
  
  mpw->list_y += add_y;
  if(mpw->list_y < _LIST_Y_TOP_OFFSET){
    mpw->list_y = _LIST_Y_TOP_OFFSET;
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
