//=============================================================================
/**
 *
 *	@file		pms_select.c
 *	@brief  �o�^�ς݊ȈՉ�b�I�����
 *	@author	hosaka genya
 *	@data		2009.10.20
 *
 */
//=============================================================================

//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "savedata/save_control.h"

// �ȈՉ�b�\���V�X�e��
#include "system/pms_draw.h"

//FONT
#include "print/gf_font.h"
#include "font/font.naix"

#include "system/brightness.h"

//PRINT_QUE
#include "print/printsys.h"
#include "print/wordset.h"

//�`��ݒ�
#include "pms_select_graphic.h"

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

//�^�b�`�o�[
#include "ui/touchbar.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "pmsi.naix"
#include "message.naix"
#include "msg/msg_pms_input.h"

//SE
#include "sound/pm_sndsys.h"
#include "pms_select_sound_def.h"

// ���͉��
#include "app/pms_input.h"

//�O�����J
#include "app/pms_select.h"

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
#define PMS_SELECT_TOUCHBAR
#define PMS_SELECT_TASKMENU
#define PMS_SELECT_PMSDRAW       // �ȈՉ�b�\��

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

enum
{ 
  PMS_SELECT_HEAP_SIZE = 0x1c000,  ///< �q�[�v�T�C�Y

  SELECT_ID_NULL = 255,  // �v���[�g�̂ǂ���I��ł��Ȃ��Ƃ�
};

//--------------------------------------------------------------
///	OBJ
//==============================================================
enum
{
  // �X�N���[���o�[�̍���
  LIST_BAR_STD_PX = 8 * 30,
  LIST_BAR_STD_PY = 8 *  1,

  // �o�[�A�C�R��
  LIST_BAR_ANM_IDX = 16,  // �o�[�A�C�R���̃A�j��IDX
  LIST_BAR_SY = 10,  // �o�[�A�C�R���̒���
  LIST_BAR_SY_CENTER = LIST_BAR_SY / 2,

  // �X�N���[���o�[�̃^�b�`�\�͈�
  LIST_BAR_TOUCH_MIN = LIST_BAR_STD_PY,     // LIST_BAR_TOUCH_MIN<= <=LIST_BAR_TOUCH_MAX  // ������MAX��=�܂݂܂�
  LIST_BAR_TOUCH_MAX = GX_LCD_SIZE_Y - 8*4 -1,

  // �o�[�A�C�R���̔z�u�\�͈�
  LIST_BAR_AREA_MIN  = LIST_BAR_TOUCH_MIN +3 + LIST_BAR_SY_CENTER,  // LIST_BAR_AREA_MIN<= <=LIST_BAR_AREA_MAX  // ������MAX��=�܂݂܂�
  LIST_BAR_AREA_MAX  = LIST_BAR_TOUCH_MAX -2 - LIST_BAR_SY_CENTER,
  LIST_BAR_AREA_SIZE = LIST_BAR_AREA_MAX - LIST_BAR_AREA_MIN +1,  // MAX���܂ނ̂�+1���Ă���

  // �o�[�A�C�R�����s�K�v�ȂƂ��̃v���[�g�̌�
  LIST_BAR_ENABLE_LISTNUM = 3,  
};

//--------------------------------------------------------------
///	BG
//==============================================================
enum
{
  // �ȈՉ�b��\������v���[�g
  PLATE_BG_SX = 29,  // �P�ʃL����
  PLATE_BG_SY = 6,   
  PLATE_BG_START_PY = 1,
  PLATE_BG_PLTT_OFS_BASE = 0x2,
};

//--------------------------------------------------------------
///	�V�[�P���X
//==============================================================
// ���C���̃V�[�P���X
enum
{
  // ����������
  SEQ_INIT                    = 0,

  // �v���[�g�I��
  SEQ_PLATE_SELECT_INIT,
  SEQ_PLATE_SELECT_MAIN,
  SEQ_PLATE_SELECT_END,

  // �^�X�N���j���[�I��
  SEQ_CMD_SELECT_INIT,
  SEQ_CMD_SELECT_MAIN,
  SEQ_CMD_SELECT_END,

  // �ȈՉ�b���͉��PROC�̌Ăяo�������A
  SEQ_CALL_EDIT_FADE_OUT_START,
  SEQ_CALL_EDIT_FADE_OUT_WAIT,
  SEQ_CALL_EDIT_INIT,
  SEQ_CALL_EDIT_MAIN,
  SEQ_CALL_EDIT_END,
  SEQ_CALL_EDIT_FADE_IN_START,
  SEQ_CALL_EDIT_FADE_IN_WAIT,

  // �I������
  SEQ_EXIT,
};

//--------------------------------------------------------------
///	SceneFunc
//==============================================================
// �v���[�g�I��
static BOOL ScenePlateSelect_Init( int* seq, void* work );
static BOOL ScenePlateSelect_Main( int* seq, void* work );
static BOOL ScenePlateSelect_End( int* seq, void* work );
// �R�}���h�I��
static BOOL SceneCmdSelect_Init( int* seq, void* work );
static BOOL SceneCmdSelect_Main( int* seq, void* work );
static BOOL SceneCmdSelect_End( int* seq, void* work );
// ���͉�ʌĂяo�������A
static BOOL SceneCallEdit_Main( int* seq, void* work );
static BOOL SceneCallEdit_End( int* seq, void* work );

//--------------------------------------------------------------
///	SceneID
//==============================================================

//--------------------------------------------------------------
///	SceneTable
//==============================================================

//--------------------------------------------------------------
///	�^�X�N���j���[ID
//==============================================================
enum
{ 
  TASKMENU_ID_DECIDE,
  TASKMENU_ID_CALL_EDIT,
  TASKMENU_ID_CANCEL,
};

//-------------------------------------
///	�t���[��
//=====================================
enum
{	
  // MAIN
	BG_FRAME_MENU_M	  = GFL_BG_FRAME0_M,  // �^�X�N���j���[
	BG_FRAME_BAR_M	  = GFL_BG_FRAME1_M,  // �^�b�`�o�[(�����̂���)
	BG_FRAME_TEXT_M	  = GFL_BG_FRAME2_M,  // �v���[�g
	BG_FRAME_BACK_M	  = GFL_BG_FRAME3_M,  // �w�i
  // SUB
	BG_FRAME_BACK_S	= GFL_BG_FRAME0_S,  // �w�i

  // �^�X�N���j���[���o�Ă���Ƃ��ɁA���̌����Â�����
  BG_MENU_BRIGHT = -8,
  BG_MENU_MASK_PLANE = (PLANEMASK_BG1|PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_OBJ),
  BG_MENU_MASK_DISP = (MASK_MAIN_DISPLAY),
};
//-------------------------------------
///	�p���b�g
//=====================================
// �p���b�g�ʒu
enum
{	
	//���C��BG
	PLTID_BG_COMMON_M			= 0,
  PLTID_BG_MSG_M        = 8,
	PLTID_BG_TASKMENU_M		= 12,
	PLTID_BG_TOUCHBAR_M		= 14,

	//�T�uBG
	PLTID_BG_BACK_S				=	0,

	//���C��OBJ
	PLTID_OBJ_COMMON_M    = 0, // 5�{�g�p
  PLTID_OBJ_TOUCHBAR_M	= 5, // 3�{�g�p
  PLTID_OBJ_PMS_DRAW    = 8, // 5�{�g�p 
	//�T�uOBJ
};

// �p���b�g��
enum
{
	//���C��BG
	PLTNUM_BG_COMMON_M			= 8,
  PLTNUM_BG_MSG_M        = 1,
	PLTNUM_BG_TASKMENU_M		= 0,  // ?�{
	PLTNUM_BG_TOUCHBAR_M		= 0,  // ?�{

	//�T�uBG
	PLTNUM_BG_BACK_S				=	0,  // �S�]��

	//���C��OBJ
	PLTNUM_OBJ_COMMON_M    = 5,
  PLTNUM_OBJ_TOUCHBAR_M	= 3,
  PLTNUM_OBJ_PMS_DRAW    = 5, 
	//�T�uOBJ
};

// �����p���b�g���̐F
enum
{
  MSG_M_COL_L                =  1,
  MSG_M_COL_S                =  2,
  MSG_M_COL_B_SELECTED       = 14,
  MSG_M_COL_B_NOT_SEL        = 13,
};

// �F�ύX
enum
{
  COL_CHANGE_0,    // �v���[�g�̘g0
  COL_CHANGE_1,    // �v���[�g�̘g1
  COL_CHANGE_2,    // �v���[�g�̘g2
  COL_CHANGE_3,    // �����̔w�i�FSELECTED
  COL_CHANGE_4,    // �����̔w�i�FNOT_SEL
  COL_CHANGE_MAX,
};

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
//--------------------------------------------------------------
///	BG�Ǘ����[�N
//==============================================================
typedef struct 
{
  GFL_ARCUTIL_TRANSINFO m_back_chr_pos;
  void*             plateScrnWork;
  NNSG2dScreenData* platescrnData;
  u16   transAnmCnt;
  GXRgb transCol[COL_CHANGE_MAX];
} PMS_SELECT_BG_WORK;

#ifdef PMS_SELECT_PMSDRAW
#define PMS_SELECT_PMSDRAW_NUM (4) ///< �ȈՉ�b�̌�
#endif // PMS_SELECT_PMSDRAW 

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  //[IN]
  PMS_SELECT_PARAM* out_param;

  //[PRIVATE]
  HEAPID heapID;

  GFL_CLWK*               clwk_bar;
  UI_EASY_CLWK_RES        clwkres;

  SAVE_CONTROL_WORK*      save_ctrl;
  PMSW_SAVEDATA*          pmsw_save;

	PMS_SELECT_BG_WORK			wk_bg;

	//�`��ݒ�
	PMS_SELECT_GRAPHIC_WORK	*graphic;

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;
#endif //PMS_SELECT_TOUCHBAR

	//�t�H���g
	GFL_FONT									*font;

	//�v�����g�L���[
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;

#ifdef PMS_SELECT_TASKMENU
	//�^�X�N���j���[
	APP_TASKMENU_RES					*menu_res;
	APP_TASKMENU_WORK					*menu;
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_PMSDRAW
  GFL_BMPWIN*               pms_win[ PMS_SELECT_PMSDRAW_NUM ];
  BOOL                      pms_win_tranReq[ PMS_SELECT_PMSDRAW_NUM ];
  PMS_DRAW_WORK*            pms_draw;
#endif //PMS_SELECT_PMSDRAW

  void*                     subproc_work;   ///< �T�uPROC�p���[�N�ۑ��̈�

  u8    select_id;        ///< �I������ID  // SELECT_ID_NULL�̂Ƃ������I�΂�Ă��Ȃ�
  u8    list_head_id;     ///< ���X�g�̐擪ID
  u8    list_max;         ///< ���X�g�̍��ڐ�  // �u�����炵���@���b�Z�[�W���@��������v�͏�������
  u8    list_max_add;     // �u�����炵���@���b�Z�[�W���@��������v����������  // = list_max or = list_max+1  // ���ꂪ���X�g�̑���
  u8    b_listbar : 1;    ///< ���X�g�o�[���^�b�`���Ă��邩�t���O
  u8    padding_bit : 7;
  
  int  sub_seq;  // 1�K�w���̃V�[�P���X 
  int  sub_res;  // 1�K�w���̌���

  BOOL b_touch;  // �^�b�`����̂Ƃ�TRUE�A�L�[����̂Ƃ�FALSE 

  // ���[�J��PROC�V�X�e��
  GFL_PROCSYS*  local_procsys;
} PMS_SELECT_MAIN_WORK;


//=============================================================================
/**
 *							�f�[�^��`
 */
//=============================================================================


//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT PMSSelectProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT PMSSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT PMSSelectProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
//-------------------------------------
///	�ėp�������[�e�B���e�B
//=====================================
static void PMSSelect_GRAPHIC_Load( PMS_SELECT_MAIN_WORK* wk );
static void PMSSelect_GRAPHIC_UnLoad( PMS_SELECT_MAIN_WORK* wk );
static void PMSSelect_BG_LoadResource( PMS_SELECT_BG_WORK* wk, HEAPID heapID );
static void PMSSelect_BG_UnLoadResource( PMS_SELECT_BG_WORK* wk );
static void PMSSelect_BG_PlateTrans( PMS_SELECT_BG_WORK* wk, u8 view_pos_id, u32 sentence_type, BOOL is_select );
static void PMSSelect_BG_PlateMain( PMS_SELECT_BG_WORK* wk );
static void PMSSelect_BG_PlateAnimeReset( PMS_SELECT_BG_WORK* wk );
static void PMSSelect_BG_PlateFutterTrans( PMS_SELECT_BG_WORK* wk, u32 sentence_type );

#ifdef PMS_SELECT_TOUCHBAR
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static void PMSSelect_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
//�ȉ��J�X�^���{�^���g�p�T���v���p
static TOUCHBAR_WORK * PMSSelect_TOUCHBAR_Init( PMS_SELECT_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID );
static void PMSSelect_TOUCHBAR_Exit( PMS_SELECT_MAIN_WORK *wk );
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
//-------------------------------------
///	���X�g�V�X�e���͂��A������
//=====================================
static APP_TASKMENU_WORK * PMSSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID );
static void PMSSelect_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void PMSSelect_TASKMENU_Main( APP_TASKMENU_WORK *menu );
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_PMSDRAW
//-------------------------------------
///	�ȈՉ�b�\��
//=====================================
static void PMSSelect_PMSDRAW_Init( PMS_SELECT_MAIN_WORK* wk );
static void PMSSelect_PMSDRAW_Exit( PMS_SELECT_MAIN_WORK* wk );
static void PMSSelect_PMSDRAW_Proc( PMS_SELECT_MAIN_WORK* wk );
#endif // PMS_SELECT_PMSDRAW

static void PLATE_CNT_Setup( PMS_SELECT_MAIN_WORK* wk );
static void PLATE_CNT_UpdateAll( PMS_SELECT_MAIN_WORK* wk );
static void PLATE_CNT_UpdateCore( PMS_SELECT_MAIN_WORK* wk, BOOL is_check_print_end );
static BOOL PLATE_CNT_Main( PMS_SELECT_MAIN_WORK* wk );
static void PLATE_CNT_Trans( PMS_SELECT_MAIN_WORK* wk );
static BOOL PLATE_UNIT_Trans( PRINT_QUE* print_que, GFL_BMPWIN* win, BOOL *transReq );
static void PLATE_UNIT_DrawLastMessage( PMS_SELECT_MAIN_WORK* wk, u8 view_pos_id, u8 data_id, BOOL is_select );
static void PLATE_UNIT_DrawNormal( PMS_SELECT_MAIN_WORK* wk, u8 view_pos_id, u8 data_id, BOOL is_select );

static int TOUCH_GetListPos( void );


//=============================================================================
/**
 *								�O�����J
 */
//=============================================================================
const GFL_PROC_DATA ProcData_PMSSelect = 
{
	PMSSelectProc_Init,
	PMSSelectProc_Main,
	PMSSelectProc_Exit,
};

//=============================================================================
/**
 *								PROC
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC ����������
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					PMS_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT PMSSelectProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	PMS_SELECT_MAIN_WORK *wk;
	PMS_SELECT_PARAM *param;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

  {
       // �^���Âɂ��Ă���
      GX_SetMasterBrightness(-16);
      GXS_SetMasterBrightness(-16);
  }

  // ���x�ݒ�
  //GFL_UI_KEY_SetRepeatSpeed( 4, 8 );
	
	//�����擾
	param	= pwk;

	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UI_DEBUG, PMS_SELECT_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(PMS_SELECT_MAIN_WORK), HEAPID_UI_DEBUG );
  GFL_STD_MemClear( wk, sizeof(PMS_SELECT_MAIN_WORK) );

  // ������
  wk->heapID    = HEAPID_UI_DEBUG;
  wk->save_ctrl = param->save_ctrl;
  wk->pmsw_save = SaveData_GetPMSW( param->save_ctrl );
  wk->out_param = param;
	
	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_pms_input_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

  // �O���t�B�b�N���[�h
  PMSSelect_GRAPHIC_Load( wk );

  // ���[�J��PROC�V�X�e�����쐬
  wk->local_procsys = GFL_PROC_LOCAL_boot( wk->heapID );

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �I������
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					PMS_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT PMSSelectProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	PMS_SELECT_MAIN_WORK* wk = mywk;

  // ���[�J��PROC�V�X�e����j��
  GFL_PROC_LOCAL_Exit( wk->local_procsys ); 

  PMSSelect_GRAPHIC_UnLoad( wk );

	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );

  // ���ɂ��ǂ�
  //GFL_UI_KEY_SetRepeatSpeed( 8, 15 );

	//PROC�p���������
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heapID );
	
  //�I�[�o�[���C�j��
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

  return GFL_PROC_RES_FINISH;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �又��
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					PMS_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static void ChangeSeq( int* seq, PMS_SELECT_MAIN_WORK* wk, int s );
static void ChangeSeq( int* seq, PMS_SELECT_MAIN_WORK* wk, int s )
{
  *seq = s;
  wk->sub_seq = 0;
  // wk->sub_res�͕ύX���Ă͂Ȃ�Ȃ��A1�V�[�P���X��Ŏg�����Ƃ�����̂ŁB
}
static GFL_PROC_RESULT PMSSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	PMS_SELECT_MAIN_WORK* wk = mywk;

  // ���[�J��PROC�̍X�V����
  GFL_PROC_MAIN_STATUS  local_proc_status   =  GFL_PROC_LOCAL_Main( wk->local_procsys );
  if( local_proc_status == GFL_PROC_MAIN_VALID ) return GFL_PROC_RES_CONTINUE;

  switch(*seq)
  {
  case SEQ_INIT:
    {
      // ���񔻒�
      PMS_DATA* data;
      data = PMSW_GetDataEntry( wk->pmsw_save, 0 );
      if( PMSDAT_IsComplete( data, wk->heapID ) == FALSE )
      {
        wk->select_id = 0;  // 0�Ԗڂ̃v���[�g�ɊȈՉ�b�𓖂Ă͂߂�悤���ꂩ��ҏW����
        ChangeSeq( seq, wk, SEQ_CALL_EDIT_INIT ); // �����Ȃ�ҏW��ʂ�
       
        HOSAKA_Printf("����Ȃ̂Œ��ڕҏW��ʂ�\n");
      }
      else
      {
        //@TODO	�t�F�[�h�V�[�P���X���Ȃ��̂�
        GX_SetMasterBrightness(0);
        GXS_SetMasterBrightness(0);
        
        ChangeSeq( seq, wk, SEQ_PLATE_SELECT_INIT );
      }
    }
    break;
  case SEQ_PLATE_SELECT_INIT:
    {
      if( ScenePlateSelect_Init( &(wk->sub_seq), wk ) )
      {
        ChangeSeq( seq, wk, SEQ_PLATE_SELECT_MAIN );
      }
    }
    break;
  case SEQ_PLATE_SELECT_MAIN:
    {
      if( ScenePlateSelect_Main( &(wk->sub_seq), wk ) )
      {
        ChangeSeq( seq, wk, wk->sub_res );
      }
    }
    break;
  case SEQ_PLATE_SELECT_END:
    {
      if( ScenePlateSelect_End( &(wk->sub_seq), wk ) )
      {
        ChangeSeq( seq, wk, SEQ_EXIT );
      }
    }
    break;
  case SEQ_CMD_SELECT_INIT:
    {
      if( SceneCmdSelect_Init( &(wk->sub_seq), wk ) )
      {
        ChangeSeq( seq, wk, SEQ_CMD_SELECT_MAIN );
      }
    }
    break;
  case SEQ_CMD_SELECT_MAIN:
    {
      if( SceneCmdSelect_Main( &(wk->sub_seq), wk ) )
      {
        ChangeSeq( seq, wk, SEQ_CMD_SELECT_END );
      }
    }
    break;
  case SEQ_CMD_SELECT_END:
    {
      if( SceneCmdSelect_End( &(wk->sub_seq), wk ) )
      {
        ChangeSeq( seq, wk, wk->sub_res );  // SceneCmdSelect_Main��wk->sub_res�����̂܂܎c���Ă����A�����ŗ��p����
      }
    }
    break;
  case SEQ_CALL_EDIT_FADE_OUT_START:
    {
       // �t�F�[�h�C��(�����遨��)
      GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0);
      ChangeSeq( seq, wk, SEQ_CALL_EDIT_FADE_OUT_WAIT );
    }
    break;
  case SEQ_CALL_EDIT_FADE_OUT_WAIT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        ChangeSeq( seq, wk, SEQ_CALL_EDIT_INIT );
      }
    }
    break;
  case SEQ_CALL_EDIT_INIT:
    {
      // �O���t�B�b�N�A�����[�h
      PMSSelect_GRAPHIC_UnLoad( wk );
      GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

      ChangeSeq( seq, wk, SEQ_CALL_EDIT_MAIN );
      
      HOSAKA_Printf("call!\n");
      
      return GFL_PROC_RES_CONTINUE;
    }
    break;
  case SEQ_CALL_EDIT_MAIN:
    {
      if( SceneCallEdit_Main( &(wk->sub_seq), wk ) )
      {
        ChangeSeq( seq, wk, SEQ_CALL_EDIT_END );
      }
      
      return GFL_PROC_RES_CONTINUE;
    }
    break;
  case SEQ_CALL_EDIT_END:
    {
      // ���[�J��PROC���I������̂�҂�  // ����Main�̍ŏ���GFL_PROC_MAIN_VALID�Ȃ�return���Ă���̂ŁA�����ł͔��肵�Ȃ��Ă��悢���O�̂���
      if( local_proc_status != GFL_PROC_MAIN_VALID )
      {
        if( SceneCallEdit_End( &(wk->sub_seq), wk ) )
  	    {
          HOSAKA_Printf("return! \n");
        
          GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
          // �O���t�B�b�N�����[�h
          PMSSelect_GRAPHIC_Load( wk );
      
          ChangeSeq( seq, wk, SEQ_CALL_EDIT_FADE_IN_START );
        }
      }
      
      return GFL_PROC_RES_CONTINUE;
    }
    break;
  case SEQ_CALL_EDIT_FADE_IN_START:
    {
      // �t�F�[�h�C��(����������)
      GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0);
      ChangeSeq( seq, wk, SEQ_CALL_EDIT_FADE_IN_WAIT );
    }
    break;
  case SEQ_CALL_EDIT_FADE_IN_WAIT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        ChangeSeq( seq, wk, SEQ_PLATE_SELECT_INIT );
      }
    }
    break;
  case SEQ_EXIT:
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  // PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );
  
  // PLATE�]��
  PLATE_CNT_Trans( wk );

#ifdef PMS_SELECT_PMSDRAW
  PMSSelect_PMSDRAW_Proc( wk );
#endif //PMS_SELECT_PMSDRAW

	// 2D�`��
	PMS_SELECT_GRAPHIC_2D_Draw( wk->graphic );

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �O���t�B�b�N ���[�h
 *	
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void PMSSelect_GRAPHIC_Load( PMS_SELECT_MAIN_WORK* wk )
{ 
  // �������
  wk->select_id = SELECT_ID_NULL;  // �L�[���͂ł����֗��Ă����ꍇ�A�ŏ��ɉ�����I��ł������ق����悳����
  wk->list_head_id = 0;  // �ȈՉ�b�ҏW����߂��Ă����ꍇ�A�v���[�g�̌����Ă�����̂��A�ҏW���Ă������̂ɂ��Ă������ق����悳����
  wk->b_listbar = FALSE;
  wk->b_touch = TRUE;    // �L�[���͂ł����֗��Ă����ꍇ�A�����K�؂ɂ��Ȃ���΂Ȃ�Ȃ�

	//�`��ݒ菉����
	wk->graphic	= PMS_SELECT_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );
  
  // OBJ���X�g�o�[ �A�C�R���ǂݍ���
  {
    UI_EASY_CLWK_RES_PARAM prm;
		GFL_CLUNIT	*clunit	= PMS_SELECT_GRAPHIC_GetClunit( wk->graphic );

    prm.draw_type = CLSYS_DRAW_MAIN;
    prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
    prm.arc_id    = ARCID_PMSI_GRAPHIC;
    prm.pltt_id   = NARC_pmsi_pms_obj_main_NCLR;
    prm.ncg_id    = NARC_pmsi_pms2_obj_main_NCGR;
    prm.cell_id   = NARC_pmsi_pms2_obj_main_NCER;
    prm.anm_id    = NARC_pmsi_pms2_obj_main_NANR;
    prm.pltt_line = PLTID_OBJ_COMMON_M;
    prm.pltt_src_ofs  = 0;
    prm.pltt_src_num  = PLTNUM_OBJ_COMMON_M;

    UI_EASY_CLWK_LoadResource( &wk->clwkres, &prm, clunit, wk->heapID );

    wk->clwk_bar = UI_EASY_CLWK_CreateCLWK( &wk->clwkres, clunit, LIST_BAR_STD_PX, LIST_BAR_AREA_MIN, LIST_BAR_ANM_IDX ,wk->heapID );

    // �^�X�N���j���[�̉�
    GFL_CLACT_WK_SetBgPri( wk->clwk_bar, 1 );

    // �A�j������ 
    GFL_CLACT_WK_SetAutoAnmFlag( wk->clwk_bar, TRUE );
  }

	//BG���\�[�X�ǂݍ���
	PMSSelect_BG_LoadResource( &wk->wk_bg, wk->heapID );

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= PMS_SELECT_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= PMSSelect_TOUCHBAR_Init( wk, clunit, wk->heapID );
	}
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
	//TASKMENU���\�[�X�ǂݍ��݁�������
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_MENU_M, PLTID_BG_TASKMENU_M, wk->font, wk->print_que, wk->heapID );
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_PMSDRAW  
  PMSSelect_PMSDRAW_Init( wk );
#endif //PMS_SELECT_PMSDRAW

}

//-----------------------------------------------------------------------------
/**
 *	@brief  �O���t�B�b�N �A�����[�h
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void PMSSelect_GRAPHIC_UnLoad( PMS_SELECT_MAIN_WORK* wk )
{
  UI_EASY_CLWK_UnLoadResource( &wk->clwkres );

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[
	PMSSelect_TOUCHBAR_Exit( wk );
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
	//TASKMENU�V�X�e�������\�[�X�j��
	APP_TASKMENU_RES_Delete( wk->menu_res );	
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_PMSDRAW
  PMSSelect_PMSDRAW_Exit( wk );
#endif //PMS_SELECT_PMSDRAW
	
  //BG���\�[�X�j��
	PMSSelect_BG_UnLoadResource( &wk->wk_bg );

	//�`��ݒ�j��
	PMS_SELECT_GRAPHIC_Exit( wk->graphic );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *	@param	PMS_SELECT_BG_WORK* wk BG�Ǘ����[�N
 *	@param	heapID  �q�[�vID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_LoadResource( PMS_SELECT_BG_WORK* wk, HEAPID heapID )
{
	ARCHANDLE	*handle;
	
  handle	= GFL_ARC_OpenDataHandle( ARCID_PMSI_GRAPHIC, heapID );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_pmsi_pms2_bg_main_NCLR, PALTYPE_MAIN_BG, 0x20*PLTID_BG_COMMON_M, 0x20*PLTNUM_BG_COMMON_M, heapID );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_pmsi_pms2_bg_main_NCLR, PALTYPE_SUB_BG,  0x20*PLTID_BG_BACK_S, 0x20*PLTNUM_BG_BACK_S, heapID );
	
  // ��b�t�H���g�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0x20*PLTID_BG_MSG_M, 0x20*PLTNUM_BG_MSG_M, heapID );
  {
    // ����Ȃ��F�������Ă���
    BOOL result;

    wk->transCol[COL_CHANGE_3] = GX_RGB(31, 31, 31);
    result = NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        PLTID_BG_MSG_M * 32 + MSG_M_COL_B_SELECTED * 2 ,
                                        &(wk->transCol[COL_CHANGE_3]) , 2 );
    GF_ASSERT( result );

    wk->transCol[COL_CHANGE_4] = GX_RGB(31, 31, 31);
    result = NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        PLTID_BG_MSG_M * 32 + MSG_M_COL_B_NOT_SEL * 2 ,
                                        &(wk->transCol[COL_CHANGE_4]) , 2 );
    GF_ASSERT( result );
  } 

  //	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_pmsi_pms2_bg_back_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_pmsi_pms2_bg_back_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );

	//	----- ����� -----

  // �w�i
	wk->m_back_chr_pos = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(	handle, NARC_pmsi_pms2_bg_back_NCGR, BG_FRAME_BACK_M, 
            0, 0, heapID );

	GF_ASSERT( wk->m_back_chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL );

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs( handle, NARC_pmsi_pms2_bg_back01_NSCR, BG_FRAME_BACK_M,
        0, GFL_ARCUTIL_TRANSINFO_GetPos( wk->m_back_chr_pos ), 0, 0, heapID );

  // �v���[�g
  GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_pmsi_pms2_bg_main_NCGR,
						BG_FRAME_TEXT_M, 0, 0, 0, heapID );

  wk->plateScrnWork = GFL_ARCHDL_UTIL_LoadScreen( handle, NARC_pmsi_pms2_bg_main1_NSCR, FALSE,
                      &wk->platescrnData, heapID );

	GFL_ARC_CloseDataHandle( handle );

  // ���j���[��BG�ɂ��o�[�����[�h���Ă���
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BG�J��
 *
 *	@param	PMS_SELECT_BG_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_UnLoadResource( PMS_SELECT_BG_WORK* wk )
{
  // �w��BG�J��
	GFL_BG_FreeCharacterArea( BG_FRAME_BACK_M,
			GFL_ARCUTIL_TRANSINFO_GetPos(wk->m_back_chr_pos),
			GFL_ARCUTIL_TRANSINFO_GetSize(wk->m_back_chr_pos));

  GFL_HEAP_FreeMemory( wk->plateScrnWork );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BG �v���[�g�]��
 *
 *	@param	PMS_SELECT_BG_WORK* wk 
 *	@param	view_pos_id
 *	@param	sentence_type
 *	@param	is_select 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_PlateTrans( PMS_SELECT_BG_WORK* wk, u8 view_pos_id, u32 sentence_type, BOOL is_select )
{
  int i;
  int sx = wk->platescrnData->screenWidth/8;
  int sy = wk->platescrnData->screenHeight/8;

  u16* buff = (u16*)&wk->platescrnData->rawData;

  // �I��F
  if( is_select )
  {
    sentence_type = PMS_TYPE_MAX;
  }

  // �p���b�g�w��
  for( i=0; i<(sx*sy); i++ )
  {
    // �X�N���[���f�[�^�̏��4bit�Ńp���b�g�w��
    buff[i] = ( buff[i] & 0xFFF ) + 0x1000 * ( sentence_type + PLATE_BG_PLTT_OFS_BASE );
  }

  //@TODO �g�����]������Ηǂ��B
  GFL_BG_WriteScreenExpand( BG_FRAME_TEXT_M, 
    0, view_pos_id * PLATE_BG_SY + 1,
    sx, PLATE_BG_SY,
    &wk->platescrnData->rawData,
    0,  0 * PLATE_BG_SY,
    sx, sy );

  GFL_BG_LoadScreenV_Req( BG_FRAME_TEXT_M );
}

//�v���[�g�̃A�j���Bsin�g���̂�0�`0xFFFF�̃��[�v
#define PLATE_PLTT_ANIME_VALUE (0x400)

typedef struct
{
  u16 pal_line_no;        // 16�{���邤���̉��{�ڂ�
  u16 col_pos_in_line;    // 1�{�ɂ���16�F�̂����̉��F�ڂ�
  u16 col_s;
  u16 col_e;
}
PAL_ANIM_SET;

static const PAL_ANIM_SET pal_anim_set[3] =
{
  {  0x7,  0x5, GX_RGB( 0,20,31), GX_RGB(15,31,31) },
  {  0x7,  0x9, GX_RGB( 4,11,20), GX_RGB( 2,25,31) },
  {  0x7,  0xA, GX_RGB(10,15,26), GX_RGB( 5,29,31) },
};

// BG�A�j��
static void _UpdatePalletAnime( u16 *anmCnt , u16 *transBuf , const PAL_ANIM_SET* anim_set, u8 anim_set_size )
{
  //�v���[�g�A�j��
  if( *anmCnt + PLATE_PLTT_ANIME_VALUE >= 0x10000 )
  {
    *anmCnt = *anmCnt + PLATE_PLTT_ANIME_VALUE-0x10000;
  }
  else
  {
    *anmCnt += PLATE_PLTT_ANIME_VALUE;
  }
  {
    //1�`0�ɕϊ�
    const fx16 cos = (FX_CosIdx(*anmCnt)+FX16_ONE)/2;

    {
      u8 i;
      for( i=0; i<anim_set_size; i++ )
      {
        const u8 r_s = ( anim_set[i].col_s & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
        const u8 r_e = ( anim_set[i].col_e & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
        const u8 g_s = ( anim_set[i].col_s & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
        const u8 g_e = ( anim_set[i].col_e & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
        const u8 b_s = ( anim_set[i].col_s & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
        const u8 b_e = ( anim_set[i].col_e & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

        const u8 r = r_s + (((r_e - r_s)*cos)>>FX16_SHIFT);
        const u8 g = g_s + (((g_e - g_s)*cos)>>FX16_SHIFT);
        const u8 b = b_s + (((b_e - b_s)*cos)>>FX16_SHIFT);
      
        BOOL result;
        transBuf[i] = GX_RGB(r, g, b);
        result = NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        anim_set[i].pal_line_no * 32 + anim_set[i].col_pos_in_line * 2 ,
                                        &(transBuf[i]) , 2 );
        GF_ASSERT( result == TRUE );
      }
    }
  }
}


//-----------------------------------------------------------------------------
/**
 *	@brief  BG�v���[�g �又��
 *
 *	@param	PMS_SELECT_BG_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_PlateMain( PMS_SELECT_BG_WORK* wk )
{
  _UpdatePalletAnime( &wk->transAnmCnt, wk->transCol, pal_anim_set, NELEMS(pal_anim_set) );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BG�A�j���[�V�����J�E���^�����Z�b�g
 *
 *	@param	PMS_SELECT_BG_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_PlateAnimeReset( PMS_SELECT_BG_WORK* wk )
{
  wk->transAnmCnt = 0;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�̉��g��BG�ŏ㕔�ɓ]�����N�G�X�g
 *
 *	@param	PMS_SELECT_BG_WORK* wk
 *	@param	sentence_type 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_PlateFutterTrans( PMS_SELECT_BG_WORK* wk, u32 sentence_type )
{
  int i;
  int sx = wk->platescrnData->screenWidth/8;
  int sy = wk->platescrnData->screenHeight/8;

  u16* buff = (u16*)&wk->platescrnData->rawData;

  //@TODO �X�N���[���o�b�t�@�S�̂�u��������K�v�͂Ȃ�

  // �p���b�g�w��
  for( i=0; i<(sx*sy); i++ )
  {
    // �X�N���[���f�[�^�̏��4bit�Ńp���b�g�w��
    buff[i] = ( buff[i] & 0xFFF ) + 0x1000 * ( sentence_type + PLATE_BG_PLTT_OFS_BASE );
  }
  
  GFL_BG_WriteScreenExpand( BG_FRAME_TEXT_M, 
    0, 0,
    sx, 1,
    &wk->platescrnData->rawData,
    0,  PLATE_BG_SY-1,
    sx, sy );

  GFL_BG_LoadScreenV_Req( BG_FRAME_TEXT_M );
}

#ifdef PMS_SELECT_TOUCHBAR
//=============================================================================
/**
 *	TOUCHBAR
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR������
 *
 * @param  wk										���[�N
 *	@param	GFL_CLUNIT *clunit	CLUNIT
 *	@param	heapID							�q�[�vID
 *
 *	@return	TOUCHBAR_WORK
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK * PMSSelect_TOUCHBAR_Init( PMS_SELECT_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID )
{
  TOUCHBAR_WORK* touchbar_work;
	
  //�^�b�`�o�[�̐ݒ�
	
  //�A�C�R���̐ݒ�
	//�������
	TOUCHBAR_SETUP	touchbar_setup = {0};

	TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
	{	
		{	
			TOUCHBAR_ICON_RETURN,
			{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
		},
	};

	//�ݒ�\����
	//�����قǂ̑����{���\�[�X���������
	touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
	touchbar_setup.p_unit		= clunit;										//OBJ�ǂݍ��݂̂��߂�CLUNIT
	touchbar_setup.bar_frm	= BG_FRAME_BAR_M;						//BG�ǂݍ��݂̂��߂�BG��
	touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;			//BG��گ�
	touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;			//OBJ��گ�
	touchbar_setup.mapping	= APP_COMMON_MAPPING_64K;	//�}�b�s���O���[�h

	touchbar_work = TOUCHBAR_Init( &touchbar_setup, heapID );

  // �^�X�N���j���[�̉�
  TOUCHBAR_SetBgPriority( touchbar_work, TOUCHBAR_ICON_RETURN, 1 );
  
	return touchbar_work;
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR�j��	�J�X�^���{�^����
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void PMSSelect_TOUCHBAR_Exit( PMS_SELECT_MAIN_WORK *wk )
{	
	//�^�b�`�o�[�j��
	TOUCHBAR_Exit( wk->touchbar );
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR���C������
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void PMSSelect_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Main( touchbar );
}
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̏�����
 *
 *	@param	menu_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * PMSSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID )
{	
	int i;
	APP_TASKMENU_INITWORK	init;
	APP_TASKMENU_ITEMWORK	item[3];
	APP_TASKMENU_WORK			*menu;	

  static u8 strtbl[] = {
    str_decide,
    str_edit,
    str_cancel,
  };

	//���̐ݒ�
	for( i = 0; i < NELEMS(item); i++ )
	{	
		item[i].str	      = GFL_MSG_CreateString( msg, strtbl[i] );	//������
		item[i].msgColor	= APP_TASKMENU_ITEM_MSGCOLOR;	//�����F
		item[i].type			= APP_TASKMENU_WIN_TYPE_NORMAL;	//���̎��
	}
	
  // �u���ǂ�v�L��
  item[2].type  = APP_TASKMENU_WIN_TYPE_RETURN,	//���ǂ�L�������������i�����j������Ȃ�

	//������
	init.heapId		= heapID;
	init.itemNum	= NELEMS(item);
	init.itemWork = item;
	init.posType	= ATPT_RIGHT_DOWN;
	init.charPosX	= 32;
	init.charPosY = 24;
	init.w				= APP_TASKMENU_PLATE_WIDTH;
	init.h				= APP_TASKMENU_PLATE_HEIGHT;

	menu	= APP_TASKMENU_OpenMenu( &init, menu_res );

	//��������
	for( i = 0; i < NELEMS(item); i++ )
	{	
		GFL_STR_DeleteBuffer( item[i].str );
	}

	return menu;

}
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̔j��
 *
 *	@param	APP_TASKMENU_WORK *menu	���[�N
 */
//-----------------------------------------------------------------------------
static void PMSSelect_TASKMENU_Exit( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_CloseMenu( menu );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̃��C������
 *
 *	@param	APP_TASKMENU_WORK *menu	���[�N
 */
//-----------------------------------------------------------------------------
static void PMSSelect_TASKMENU_Main( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_UpdateMenu( menu );
}
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_PMSDRAW

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� ����������
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_PMSDRAW_Init( PMS_SELECT_MAIN_WORK* wk )
{
  int i;
	GFL_CLUNIT	*clunit;
  
  clunit = PMS_SELECT_GRAPHIC_GetClunit( wk->graphic );

  wk->pms_draw  = PMS_DRAW_Init( clunit, CLSYS_DRAW_MAIN, wk->print_que, wk->font, 
      PLTID_OBJ_PMS_DRAW, PMS_SELECT_PMSDRAW_NUM ,wk->heapID );

  HOSAKA_Printf("list_head_id=%d select_id=%d \n", wk->list_head_id, wk->select_id );
  
  // ���X�g�̐��𑪒�
  wk->list_max = 0;
  for( i=0; i<PMS_DATA_ENTRY_MAX; i++ )
  {
    PMS_DATA* pms;
    
    pms = PMSW_GetDataEntry( wk->pmsw_save, i );
      
    if( PMSDAT_IsComplete( pms, wk->heapID ) == FALSE )
    {
      HOSAKA_Printf( "list_max=%d \n", wk->list_max );
      break;
    }
    else
    {
      wk->list_max++; // �u�����炵���@���b�Z�[�W���@��������v�͏�������
    }
  }

  // �u�����炵���@���b�Z�[�W���@��������v���������� 
  {
    wk->list_max_add = wk->list_max;
    if( wk->list_max < PMS_DATA_ENTRY_MAX ) wk->list_max_add++;  // �u�����炵���@���b�Z�[�W���@��������v����������
  }

  // �o�[���o���ݒ�
  {
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_bar, ( wk->list_max_add > LIST_BAR_ENABLE_LISTNUM ) );
  }

  // PMS�\���pBMPWIN����
  for( i=0; i<PMS_SELECT_PMSDRAW_NUM; i++ )
  {
    wk->pms_win[i] = GFL_BMPWIN_Create(
        BG_FRAME_TEXT_M,					// �a�f�t���[��
        2, 2 + 6 * i,			      	// �\�����W(�L�����P��)
        25, 4,    							  // �\���T�C�Y
        PLTID_BG_MSG_M,					// �p���b�g
        GFL_BMP_CHRAREA_GET_B );	// �L�����擾����
  }

  // �ȈՉ�b�`�� ������
  PLATE_CNT_Setup( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� �㏈��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_PMSDRAW_Exit( PMS_SELECT_MAIN_WORK* wk )
{
  PMS_DRAW_Exit( wk->pms_draw );
  {
    int i;
    for( i=0; i<PMS_SELECT_PMSDRAW_NUM; i++ )
    {
      GFL_BMPWIN_Delete( wk->pms_win[i] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� �又��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_PMSDRAW_Proc( PMS_SELECT_MAIN_WORK* wk )
{
  PMS_DRAW_Main( wk->pms_draw );
}

#endif // PMS_SELECT_PMSDRAW

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�Ǘ� �S�v���[�g���j�b�g��������
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PLATE_CNT_Setup( PMS_SELECT_MAIN_WORK* wk )
{
  PLATE_CNT_UpdateCore( wk, FALSE );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�Ǘ� �S�v���[�g���j�b�g���X�V
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void PLATE_CNT_UpdateAll( PMS_SELECT_MAIN_WORK* wk )
{
  PLATE_CNT_UpdateCore( wk, TRUE );
  PMSSelect_BG_PlateAnimeReset( &wk->wk_bg );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�Ǘ� �S�v���[�g���j�b�g���X�V �R�A
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PLATE_CNT_UpdateCore( PMS_SELECT_MAIN_WORK* wk, BOOL is_check_print_end )
{ 
  int i;
    
  // �܂������]�����I�����Ă��Ȃ����̂�����Ƃ��͍X�V���Ȃ�
  if( is_check_print_end && PMS_DRAW_IsPrintEnd( wk->pms_draw ) == FALSE )
  {
    return;
  }

  // ��[�X�N���[�����N���A
  GFL_BG_ClearScreenCodeVReq( BG_FRAME_TEXT_M, 0 );

  // ���X�g���擪�łȂ���΍ŏ㕔�Ƀt�b�^��\��
  if( wk->list_head_id > 0 )
  {
    PMS_DATA* pms;
    u8 futter_id;
    u32 sentence_type;
    
    futter_id = wk->list_head_id - 1;
    pms = PMSW_GetDataEntry( wk->pmsw_save, futter_id );
    sentence_type = PMSDAT_GetSentenceType( pms );

    PMSSelect_BG_PlateFutterTrans( &wk->wk_bg, sentence_type );
  }

  // �ȈՉ�b��S�ĕ`��
  for( i=0; (wk->list_head_id+i) < wk->list_max && i < PMS_SELECT_PMSDRAW_NUM; i++ )
  {
    u8 data_id = wk->list_head_id + i;
    BOOL is_select = ( data_id == wk->select_id );
    
    {
      // �ȈՉ�b�`��
      PLATE_UNIT_DrawNormal( wk, i, data_id, is_select );
    }
  }
  // �u�����炵���@���b�Z�[�W���@��������v������
  {
    if( wk->list_max < PMS_DATA_ENTRY_MAX && i < PMS_SELECT_PMSDRAW_NUM )
    {
      u8 data_id = wk->list_head_id + i;
      BOOL is_select = ( data_id == wk->select_id );

      // �u�����炵���@���b�Z�[�W���@��������v
      PLATE_UNIT_DrawLastMessage( wk, i, NULL, is_select );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �L�[�ړ����o�[�A�C�R���̈ʒu�ύX
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void LIST_BAR_SetPosFromKey( PMS_SELECT_MAIN_WORK* wk )
{
  // wk->list_head_id���������l�ɍX�V����Ă���A���̊֐����Ăяo�����

  int moveable_num;  // ��ʂ̍ŏ㕔�ɂȂ邱�Ƃ��ł���v���[�g�̌�
  int pos_id;
  s16 clpos;
  int fx_elem_size;
 
  // �o�[���o�����Ă��Ȃ��ꍇ�͏����Ȃ�
  if( GFL_CLACT_WK_GetDrawEnable( wk->clwk_bar ) == FALSE )
  {
    return;
  }

  moveable_num = wk->list_max_add - LIST_BAR_ENABLE_LISTNUM +1;  // �o�[�A�C�R�����o�Ă���Ȃ�A�����͕K�����ɂȂ�
  pos_id = wk->list_head_id;
  
  // MIN
  if( pos_id == 0 )
  {
    clpos = LIST_BAR_AREA_MIN;
  }
  // MAX
  else if( pos_id == moveable_num -1 )
  {
    clpos = LIST_BAR_AREA_MAX;
  }
  // OTHER
  else
  {
    // �^�b�`���W����CLWK�̍��W���Z�o
    //clpos = LIST_BAR_AREA_MIN + LIST_BAR_AREA_SIZE / moveable_num * wk->list_head_id + LIST_BAR_AREA_SIZE / moveable_num / 2;  // �����ɕ\�����Ă���
    clpos = LIST_BAR_AREA_MIN +  ( LIST_BAR_AREA_SIZE * wk->list_head_id ) / moveable_num + LIST_BAR_AREA_SIZE / ( moveable_num * 2 );  // �v�Z���������ւ��Ă���
   
    if( clpos > LIST_BAR_AREA_MAX ) clpos = LIST_BAR_AREA_MAX;
  }

  GFL_CLACT_WK_SetWldTypePos( wk->clwk_bar, clpos, CLSYS_MAT_Y );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`�ړ��ɂ�郊�X�g�̈ʒu�ύX
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL LIST_BAR_SetPosFromTouch( PMS_SELECT_MAIN_WORK* wk )
{
  u32 tx, ty;

  // �o�[���o�����Ă��Ȃ��ꍇ�͏����Ȃ�
  if( GFL_CLACT_WK_GetDrawEnable( wk->clwk_bar ) == FALSE )
  {
    return FALSE;
  }

  // �g���K�Ŕ͈͂��擾���A����ȍ~�͔͈͂����Ȃ�
  if( GFL_UI_TP_GetPointTrg( &tx, &ty ) == FALSE )
  {
    return FALSE;
  }

  if( tx >= LIST_BAR_STD_PX && tx < GX_LCD_SIZE_X )
  { 
    if( ty <= LIST_BAR_TOUCH_MAX && ty >= LIST_BAR_TOUCH_MIN ) 
    {
      wk->b_listbar = TRUE;
      return TRUE;
    }
  }
  
  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	u32 py
 *	@param	list_max_add
 *	@param	out_list_head_id
 *	@param	out_pos_dot 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void LIST_BAR_GetPosDataByTouch( u32 py, int list_max_add, u16* out_list_head_id, s16* out_pos_dot )
{
  int moveable_num;  // ��ʂ̍ŏ㕔�ɂȂ邱�Ƃ��ł���v���[�g�̌�
  int i;

  moveable_num = list_max_add - LIST_BAR_ENABLE_LISTNUM +1;  // �o�[�A�C�R�����o�Ă���Ȃ�A�����͕K�����ɂȂ�

  for( i=0; i<moveable_num; i++ )
  {
    int min = LIST_BAR_AREA_MIN + ( LIST_BAR_AREA_SIZE * i ) / moveable_num;  // min<= <max
    int max = LIST_BAR_AREA_MIN + ( LIST_BAR_AREA_SIZE * (i+1) ) / moveable_num;  // ������max�́A���̃v���[�g�̈ʒu�ɂȂ�̂ŁA=���܂܂Ȃ�
    if( i==0 )
    {
      if( py < max )
      {
        *out_list_head_id = i;
        break;
      }
    }
    else if( i==moveable_num -1 )
    {
      if( py >= min )
      {
        *out_list_head_id = i;
        break;
      }
    }
    else 
    {
      if( min<=py && py<max )
      {
        *out_list_head_id = i;
        break;
      }
    }
  }

  {
    u32 py_dot = MATH_CLAMP( py, LIST_BAR_AREA_MIN, LIST_BAR_AREA_MAX );
    *out_pos_dot = py_dot;
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL LIST_BAR_CheckTouching( PMS_SELECT_MAIN_WORK* wk )
{
  u32 tx, ty;
  u16 head_id;
  s16 clpos;

  GF_ASSERT( wk->b_listbar );

  // �p���^�b�`�`�F�b�N���^�b�`���W�擾
  if( GFL_UI_TP_GetPointCont( &tx, &ty ) == FALSE )
  {
    wk->b_listbar = FALSE;
    return FALSE;
  }

  LIST_BAR_GetPosDataByTouch( ty, wk->list_max_add, &head_id, &clpos );
  GFL_CLACT_WK_SetWldTypePos( wk->clwk_bar, clpos, CLSYS_MAT_Y );  // �E�[�̃X�N���[���o�[�̃o�[�A�C�R���̈ʒu�̓^�b�`�ɒǐ����ď�ɍX�V���Ă���

  // �I��ID�ɕύX���������ꍇ��OBJ���ړ�
  if( head_id != wk->list_head_id )
  {
    // �o��
    wk->list_head_id = head_id;

    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g���� �又��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL PLATE_CNT_Main( PMS_SELECT_MAIN_WORK* wk )
{
  BOOL b_decide = FALSE;
  BOOL b_input = TRUE;

 if( b_input )
 {
  // �Z���N�g������ԂŃL�[���͂��ꂽ��
  if( GFL_UI_KEY_GetTrg() && wk->select_id == SELECT_ID_NULL )
  {
    wk->select_id = wk->list_head_id; //�擪��I��
    wk->b_touch = FALSE;

    PLATE_CNT_UpdateAll( wk );

    b_input = FALSE;
  }
 }
 
 if( b_input )
 {
  // �L�[����
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
  {
    b_decide = TRUE;
    b_input = FALSE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )
  {
    if( wk->select_id > 0 )
    {
      wk->select_id--;

      // ���X�g�̐擪��I�����Ă����ꍇ�̓��X�g���ƈړ�
      if( wk->select_id+1 == wk->list_head_id )
      {
        wk->list_head_id--;

        GF_ASSERT( wk->list_head_id >= 0 );
      }

      LIST_BAR_SetPosFromKey( wk );

      //@TODO �d���悤�Ȃ�A
      //1:�y�[�W�S�̂��X�V����Ȃ�����UpdateAll���X�N���[���̃J���[��u�����鏈���ɂ���
      //2:�ړ��̎��̓R�s�[���g���B
      PLATE_CNT_UpdateAll( wk );

      GFL_SOUND_PlaySE( SE_MOVE_CURSOR );
      
      HOSAKA_Printf("list_head_id=%d select_id=%d \n", wk->list_head_id, wk->select_id );
    }
    else
    {
      // �u�u�[���ĉ���炷�Ȃ炱����
    }
    b_input = FALSE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )
  {
    if( wk->select_id < wk->list_max_add-1 )
    {
      wk->select_id++;

      // ���X�g�̖�����I�����Ă����ꍇ�̓��X�g���ƈړ�
      if( ( wk->select_id == wk->list_head_id + PMS_SELECT_PMSDRAW_NUM - 1 ) )
      {
        wk->list_head_id++;

        GF_ASSERT( wk->list_head_id >= 0 );
      }

      LIST_BAR_SetPosFromKey( wk );
        
      PLATE_CNT_UpdateAll( wk );

      GFL_SOUND_PlaySE( SE_MOVE_CURSOR );
     
      HOSAKA_Printf("list_head_id=%d select_id=%d \n", wk->list_head_id, wk->select_id );
    }
    else
    {
      // �u�u�[���ĉ���炷�Ȃ炱����
    }
    b_input = FALSE;
  }
 }

 if( b_input )
 {
  // �^�b�`����
  {
    // �E�[�̃X���C�h�o�[�̃o�[�A�C�R���𑀍삵�Ă���Œ��Ȃ�
    if( wk->b_listbar )
    {
      if( LIST_BAR_CheckTouching( wk ) )
      {
        // �ړ����������ꍇ�͍X�V
        PLATE_CNT_UpdateAll( wk );
      }
      b_input = FALSE;
    }
    // �E�[�̃X���C�h�o�[�̃o�[�A�C�R���𑀍삵�Ă��Ȃ��Ȃ�
    else
    {
      int list_pos = TOUCH_GetListPos();

      // �^�b�`�����Ƃ���Ƀv���[�g�����݂��Ă��邩
      if( list_pos != -1 )
      {
        if( list_pos > wk->list_max_add -1 )
        {
          // �^�b�`�����Ƃ���Ƀv���[�g�͑��݂��Ȃ��̂ŁAlist_pos��-1�ɏ��������Ă���
          list_pos = -1;
        }
      }

      // �^�b�`�����v���[�g��I��
      if( list_pos != -1 )
      {
        wk->b_touch = TRUE;

        wk->select_id =  wk->list_head_id + list_pos;

        HOSAKA_Printf("list_pos = %d, select_id = %d \n", list_pos, wk->select_id );
      
        PLATE_CNT_UpdateAll( wk );
        
        b_input = FALSE;
        b_decide = TRUE;
      }
      // �o�[���^�b�`�Ńv���[�g�ړ�
      else
      {
        // ���X�g�o�[�����삵�Ă��Ȃ���ΊJ�n����
        if( wk->b_listbar == FALSE )
        {
          LIST_BAR_SetPosFromTouch( wk );
        }
      
        // �E�[�̃X���C�h�o�[�̃o�[�A�C�R���𑀍삵�n�߂��Ȃ�
        if( wk->b_listbar == TRUE )
        {
          wk->b_touch = TRUE;
          
          if( LIST_BAR_CheckTouching( wk ) || wk->select_id != SELECT_ID_NULL )  // �ŏ��̃^�b�`�ʒu���`�F�b�N����
          {
            wk->select_id = SELECT_ID_NULL; ///< �E�[�̃X���C�h�o�[�̃o�[�A�C�R�����^�b�`�����̂ŁA�I���𖳌���
            
            // �ړ����������ꍇ�A�܂��́A�I������Ă������̂����������ꂽ�ꍇ�A�͍X�V
            PLATE_CNT_UpdateAll( wk );
          }
          b_input = FALSE;
        }
      }
    }
  }
 }

 return b_decide;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �S�v���[�g��]��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PLATE_CNT_Trans( PMS_SELECT_MAIN_WORK* wk )
{
  int i;

  for( i=0; i < PMS_SELECT_PMSDRAW_NUM; i++ )
  {
    PLATE_UNIT_Trans( wk->print_que, wk->pms_win[ i ], &wk->pms_win_tranReq[ i ] );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�]��
 *
 *	@param	PRINT_QUE* print_que
 *	@param	win
 *	@param	transReq 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL PLATE_UNIT_Trans( PRINT_QUE* print_que, GFL_BMPWIN* win, BOOL *transReq )
{ 
  GFL_BMP_DATA* bmp = GFL_BMPWIN_GetBmp( win );

  if( *transReq == TRUE && !PRINTSYS_QUE_IsExistTarget( print_que, bmp ) )
  {
    GFL_BMPWIN_MakeTransWindow( win );
//    GFL_BMPWIN_TransVramCharacter( win );
    *transReq = FALSE;
    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �����炵���@���b�Z�[�W���@��������
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk
 *	@param	view_pos_id
 *	@param	data_id
 *	@param	is_select 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PLATE_UNIT_DrawLastMessage( PMS_SELECT_MAIN_WORK* wk, u8 view_pos_id, u8 data_id, BOOL is_select )
{
  PRINTSYS_LSB color;
  STRBUF* buf;

  GF_ASSERT( view_pos_id < PMS_SELECT_PMSDRAW_NUM );
  
  // �ȈՉ�b��\�����Ȃ�Ώ���
  if( PMS_DRAW_IsPrinting( wk->pms_draw, view_pos_id ) )
  {
    PMS_DRAW_Clear( wk->pms_draw, view_pos_id, FALSE );
  }

  buf = GFL_MSG_CreateString( wk->msg, pms_input01_01 );
  
  // �v���[�g�`��
  PMSSelect_BG_PlateTrans( &wk->wk_bg, view_pos_id, 0, is_select );
  
  // �I����Ԃɂ���ĕ����`��G���A�̃J���[���w��
  if( is_select )
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->pms_win[ view_pos_id ] ), MSG_M_COL_B_SELECTED );
    color = PRINTSYS_LSB_Make( MSG_M_COL_L, MSG_M_COL_S, MSG_M_COL_B_SELECTED );
  }
  else
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->pms_win[ view_pos_id ] ), MSG_M_COL_B_NOT_SEL );
    color = PRINTSYS_LSB_Make( MSG_M_COL_L, MSG_M_COL_S, MSG_M_COL_B_NOT_SEL );
  }

  PRINTSYS_PrintQueColor( wk->print_que, GFL_BMPWIN_GetBmp(wk->pms_win[ view_pos_id ]), 0, 0, buf, wk->font, color );

  GFL_STR_DeleteBuffer( buf );

  wk->pms_win_tranReq[ view_pos_id ] = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�`��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk
 *	@param	view_pos_id
 *	@param	data_id 
 *
 *	@retval 
 */
//-----------------------------------------------------------------------------
static void PLATE_UNIT_DrawNormal( PMS_SELECT_MAIN_WORK* wk, u8 view_pos_id, u8 data_id, BOOL is_select )
{
  PMS_DATA* pms;
  
  GF_ASSERT( view_pos_id < PMS_SELECT_PMSDRAW_NUM );

  // �\�����Ȃ�Ώ���
  if( PMS_DRAW_IsPrinting( wk->pms_draw, view_pos_id ) )
  {
    PMS_DRAW_Clear( wk->pms_draw, view_pos_id, FALSE );
  }
     
  pms = PMSW_GetDataEntry( wk->pmsw_save, data_id );

  // �p���b�g�ɂ���ăv���[�g�̔w�i�F��ύX
  {
    u32 sentence_type = PMSDAT_GetSentenceType( pms );
    PMSSelect_BG_PlateTrans( &wk->wk_bg, view_pos_id, sentence_type, is_select );
  }

  // �I����Ԃɂ���ĕ����`��G���A�̐F�w��
  if( is_select )
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->pms_win[ view_pos_id ] ), MSG_M_COL_B_SELECTED );
    PMS_DRAW_SetPrintColor( wk->pms_draw, PRINTSYS_LSB_Make( MSG_M_COL_L, MSG_M_COL_S, MSG_M_COL_B_SELECTED ) );
    PMS_DRAW_SetNullColorPallet( wk->pms_draw, MSG_M_COL_B_SELECTED );
  }
  else
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->pms_win[ view_pos_id ] ), MSG_M_COL_B_NOT_SEL );
    PMS_DRAW_SetPrintColor( wk->pms_draw, PRINTSYS_LSB_Make( MSG_M_COL_L, MSG_M_COL_S, MSG_M_COL_B_NOT_SEL ) );
    PMS_DRAW_SetNullColorPallet( wk->pms_draw, MSG_M_COL_B_NOT_SEL );
  }

  //  �ȈՉ�b�\��
  if( PMSDAT_IsComplete( pms, wk->heapID ) == TRUE )
  {
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[ view_pos_id ], pms, view_pos_id );
    wk->pms_win_tranReq[ view_pos_id ] = TRUE;
  }
  
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�͈̔͂��^�b�`���Ă��邩�H
 *
 *	@param	void
 *
 *	@retval 0�ȏ�̂Ƃ��^�b�`���Ă��郊�X�g�����Ԗ�(��ʂɌ����Ă��郊�X�g�̒��ł̔ԍ��Ȃ̂ŁA�ʂ��ԍ��ł͂Ȃ�)���Ԃ��A���̂Ƃ��^�b�`���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static int TOUCH_GetListPos( void )
{
  u32 px, py;

  if( GFL_UI_TP_GetPointTrg( &px, &py ) )
  {
    if( px < GX_LCD_SIZE_X - (8*3) && py >= PLATE_BG_START_PY * 8 )
    {
        // �L�����ɕϊ�
        py /= 8;
        // �擪�܂ł��炵
        py -= PLATE_BG_START_PY;
        // ���X�g�̏ꏊ���擾
        py /= PLATE_BG_SY;

        // �^�b�`�\�Ȃ̂�3����
        if( py < PMS_SELECT_PMSDRAW_NUM-1 )
        {
          return py;
        }
    }
  }

  return -1;
}

//=============================================================================
// SCENE
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�I�� ������
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL ScenePlateSelect_Init( int* seq, void* work )
{
  PMS_SELECT_MAIN_WORK* wk = work;

  // �S�ĕ\��
  TOUCHBAR_SetVisibleAll( wk->touchbar, TRUE );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�I���又��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL ScenePlateSelect_Main( int* seq , void* work )
{ 
  PMS_SELECT_MAIN_WORK* wk = work;
  TOUCHBAR_ICON result;
  BOOL b_decide;

  PMSSelect_BG_PlateMain( &wk->wk_bg );

	//�^�b�`�o�[���C������
	PMSSelect_TOUCHBAR_Main( wk->touchbar );

  // �^�b�`�o�[���͏�Ԏ擾
  result = TOUCHBAR_GetTrg( wk->touchbar );

  // �^�b�`�o�[���͔���
  switch( result )
  {
  case TOUCHBAR_ICON_RETURN :
    // �L�����Z���t���OON
    wk->out_param->out_cancel_flag  = TRUE;
    wk->out_param->out_pms_data     = NULL;
    wk->sub_res = SEQ_PLATE_SELECT_END;
    return TRUE;

  // @TODO ���E�ƍ��ڃA�C�R��

  case TOUCHBAR_ICON_CUTSOM1 :
  case TOUCHBAR_SELECT_NONE :
    break;

  default : GF_ASSERT(0);
  }

  // �v���[�g����
  b_decide = PLATE_CNT_Main( wk );
    
  // ���j���[���J��
  if( b_decide )
  {
    PMS_DATA* pms;

    GFL_SOUND_PlaySE( SE_DECIDE );
    
    pms = PMSW_GetDataEntry( wk->pmsw_save, wk->select_id );

    if( PMSDAT_IsComplete( pms, wk->heapID ) )
    {
      wk->sub_res = SEQ_CMD_SELECT_INIT;
    }
    else
    {
      // �I��̈悪�V�K�Ȃ璼�ڃW�����v
      wk->sub_res = SEQ_CALL_EDIT_FADE_OUT_START;
    }

    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�I�� �㏈��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL ScenePlateSelect_End( int* seq, void* work )
{
  PMS_SELECT_MAIN_WORK* wk = work;
  
  // �S�ď���
  TOUCHBAR_SetVisibleAll( wk->touchbar, FALSE );

  return TRUE;
}
  
//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�ɑ΂���R�}���h�I�� ������
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCmdSelect_Init( int* seq, void* work )
{
  PMS_SELECT_MAIN_WORK* wk = work;

  wk->menu = PMSSelect_TASKMENU_Init( wk->menu_res, wk->msg, wk->heapID );

  SetBrightness( BG_MENU_BRIGHT, BG_MENU_MASK_PLANE, BG_MENU_MASK_DISP );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�ɑ΂���R�}���h�I�� �又��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCmdSelect_Main( int* seq, void* work )
{
  PMS_SELECT_MAIN_WORK* wk = work;
	
  //�^�X�N���j���[���C������
	PMSSelect_TASKMENU_Main( wk->menu );

  if( APP_TASKMENU_IsFinish( wk->menu ) )
  {
    u8 pos = APP_TASKMENU_GetCursorPos( wk->menu );

    switch( pos )
    {
    case TASKMENU_ID_DECIDE :
      // �����Ă� �� �I��
      wk->sub_res = SEQ_PLATE_SELECT_END;
      // �I�����ꂽPMS_DATA��ۑ�
      wk->out_param->out_pms_data     = PMSW_GetDataEntry( wk->pmsw_save, wk->select_id );
      wk->out_param->out_cancel_flag  = FALSE;
      break;
    case TASKMENU_ID_CALL_EDIT :
      // �ւ񂵂イ �� ���͉�ʌĂяo��
      wk->sub_res = SEQ_CALL_EDIT_FADE_OUT_START;
      break;
    case TASKMENU_ID_CANCEL :
      // �L�����Z�� �� �I���ɖ߂�
      wk->sub_res = SEQ_PLATE_SELECT_MAIN;
      break;
    default : GF_ASSERT(0);
    }

    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�ɑ΂���R�}���h�I�� �㏈��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCmdSelect_End( int* seq, void* work )
{ 
  // wk->sub_res��SceneCmdSelect_Main�̌��ʂ���ύX�����Ɏc���Ă���

  PMS_SELECT_MAIN_WORK* wk = work;
  
  SetBrightness( 0, BG_MENU_MASK_PLANE, BG_MENU_MASK_DISP );

	PMSSelect_TASKMENU_Exit( wk->menu );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ւ񂵂イ���ȈՉ�b����
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCallEdit_Main( int* seq, void* work )
{
  PMSI_PARAM* pmsi;
  PMS_SELECT_MAIN_WORK* wk = work;

  switch( *seq )
  {
  case 0:
    (*seq)++; 
    break;
  case 1:
    {
#if PM_DEBUG
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
      {
        PMS_DATA data;
        PMSDAT_Init( &data, 0 );
        pmsi = PMSI_PARAM_Create(PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, wk->save_ctrl, wk->heapID );
      }
      else
#endif
      {
        PMS_DATA* data;

        pmsi = PMSI_PARAM_Create(PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, wk->save_ctrl, wk->heapID );
        data = PMSW_GetDataEntry( wk->pmsw_save, wk->select_id );

        PMSI_PARAM_SetInitializeDataSentence( pmsi, data );
      }

      // PROC�ؑ� ���͉�ʌĂяo��  // ���[�J��PROC�Ăяo��
      GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &ProcData_PMSInput, pmsi );
      wk->subproc_work = pmsi;

      return TRUE;
    }
    break;

  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ҏW��ʏI��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCallEdit_End( int* seq, void* work )
{ 
  PMS_SELECT_MAIN_WORK* wk = work;

  switch( *seq )
  {
  case 0:

    if( PMSI_PARAM_CheckModified( wk->subproc_work ) )
    {
      PMS_DATA pms;

      // �ҏW�������b�Z�[�W���Z�[�u
      PMSI_PARAM_GetInputDataSentence( wk->subproc_work, &pms );

      PMSW_SetDataEntry( wk->pmsw_save, wk->select_id, &pms );
    }

    // �p�����[�^�폜
    PMSI_PARAM_Delete( wk->subproc_work );

    (*seq)++;
    
    break;

  case 1:
    {
      return TRUE;
    }
    break;

  default : GF_ASSERT(0);
  }

  return FALSE;
}


