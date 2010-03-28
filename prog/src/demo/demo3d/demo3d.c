//=============================================================================
/**
 *
 *  @file   demo3d.c
 *  @brief  3D�f���Đ��A�v��
 *  @author genya hosak -> miyuki iwasawa
 *  @data   2009.11.27 -> 2010.02.18
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/wipe.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

// ������֘A
#include "font/font.naix"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" //PRINT_QUE

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

#include "field/field_light_status.h"
#include "field/weather_no.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "arc/fieldmap/zone_id.h"

//�O�����J
#include "message.naix"

#include "demo3d_local.h"
#include "demo3d_graphic.h"
#include "demo3d_engine.h"
#include "demo3d_exp.h"
#include "demo3d_data.h"

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *                �萔��`
 */
//=============================================================================
enum
{ 
  DEMO3D_HEAP_SIZE = 0x130000,  ///< �q�[�v�T�C�Y
};

//=============================================================================
/**
 *                �\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
/// BG�Ǘ����[�N
//==============================================================
typedef struct 
{
  int dummy;
} DEMO3D_BG_WORK;

//--------------------------------------------------------------
/// ���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;

  DEMO3D_PARAM*         param;

  DEMO3D_BG_WORK        wk_bg;

  //�`��ݒ�
  DEMO3D_GRAPHIC_WORK *graphic;

  //�t�H���g
  GFL_FONT                  *font;

  //�v�����g�L���[
  PRINT_QUE                 *print_que;
  GFL_MSGDATA               *msg;

  // �f���G���W��
  DEMO3D_ENGINE_WORK*   engine;

  // �A�v����O�����G���W��
  APP_EXCEPTION_WORK*   expection;
  

} DEMO3D_MAIN_WORK;


//=============================================================================
/**
 *              �f�[�^��`
 */
//=============================================================================


//=============================================================================
/**
 *              �v���g�^�C�v�錾
 */
//=============================================================================
//-------------------------------------
/// PROC
//=====================================
static GFL_PROC_RESULT Demo3DProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT Demo3DProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT Demo3DProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

//-----------------------------------------------------------
// �A�v����O����
//-----------------------------------------------------------
static APP_EXCEPTION_WORK* APP_EXCEPTION_Create( DEMO3D_ID demo_id, DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_ENGINE_WORK* engine, HEAPID heapID );
static void APP_EXCEPTION_Delete( APP_EXCEPTION_WORK* wk );
static void APP_EXCEPTION_Main( APP_EXCEPTION_WORK* wk );

static BOOL sub_FadeInOutReq( u8 demo_id, u8 wipe, HEAPID heapID );
static int _key_check( DEMO3D_MAIN_WORK *wk );

//=============================================================================
/**
 *                �O�����J
 */
//=============================================================================
const GFL_PROC_DATA Demo3DProcData = 
{
  Demo3DProc_Init,
  Demo3DProc_Main,
  Demo3DProc_Exit,
};
//=============================================================================
/**
 *                PROC
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief  PROC ����������
 *
 *  @param  GFL_PROC *proc�v���Z�X�V�X�e��
 *  @param  *seq          �V�[�P���X
 *  @param  *pwk          DEMO3D_PARAM
 *  @param  *mywk         PROC���[�N
 *
 *  @retval �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Demo3DProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
  DEMO3D_MAIN_WORK *wk;
  DEMO3D_PARAM *param;

  //�I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
  
  //�����擾
  param = pwk;

  GF_ASSERT( param->demo_id != DEMO3D_ID_NULL && param->demo_id < DEMO3D_ID_MAX );

  //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_DEMO3D, DEMO3D_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(DEMO3D_MAIN_WORK), HEAPID_DEMO3D );
  GFL_STD_MemClear( wk, sizeof(DEMO3D_MAIN_WORK) );

  // ������
  wk->heapID      = HEAPID_DEMO3D;
  wk->param       = param;

  //�`��ݒ菉����
  wk->graphic = DEMO3D_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, param->demo_id, wk->heapID );

  //�t�H���g�쐬
  wk->font      = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
                        GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

  //���b�Z�[�W
  wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
      NARC_message_mictest_dat, wk->heapID );

  //PRINT_QUE�쐬
  wk->print_que   = PRINTSYS_QUE_Create( wk->heapID );

  // BG/OBJ���\���ɂ��Ă���
  GFL_BG_SetVisible( BG_FRAME_BACK_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_TEXT_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_BACK_M, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_POKE_M, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_BAR_M,  VISIBLE_OFF );
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );

  //3D ������
  wk->engine = Demo3D_ENGINE_Init( wk->graphic, param, wk->heapID );
 
  // �f�����̗�O�����G���W��������
  wk->expection = APP_EXCEPTION_Create( param->demo_id, wk->graphic, wk->engine, wk->heapID );

  // �t�F�[�h�C�� ���N�G�X�g
  sub_FadeInOutReq( param->demo_id, WIPE_TYPE_FADEIN, wk->heapID );

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  PROC �I������
 *
 *  @param  GFL_PROC *proc�v���Z�X�V�X�e��
 *  @param  *seq          �V�[�P���X
 *  @param  *pwk          DEMO3D_PARAM
 *  @param  *mywk         PROC���[�N
 *
 *  @retval �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Demo3DProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  DEMO3D_MAIN_WORK* wk = mywk;
  
  switch( *seq ){
  case 0:
    // �t�F�[�h�A�E�g ���N�G�X�g
    (*seq) = 1+sub_FadeInOutReq( wk->param->demo_id, WIPE_TYPE_FADEOUT, wk->heapID );
    return GFL_PROC_RES_CONTINUE;
  case 1:
    if( WIPE_SYS_EndCheck() == FALSE ){
      return GFL_PROC_RES_CONTINUE;
    }
    break;
  default:
    break;
  }

  // ��O�����G���W�� �I������
  APP_EXCEPTION_Delete( wk->expection );

  //���b�Z�[�W�j��
  GFL_MSG_Delete( wk->msg );

  //PRINT_QUE
  PRINTSYS_QUE_Delete( wk->print_que );

  //FONT
  GFL_FONT_Delete( wk->font );
  
  //3D �j��
  Demo3D_ENGINE_Exit( wk->engine );

  //�`��ݒ�j��
  DEMO3D_GRAPHIC_Exit( wk->graphic );

  //PROC�p���������
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heapID );

  //�I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  PROC �又��
 *
 *  @param  GFL_PROC *proc�v���Z�X�V�X�e��
 *  @param  *seq          �V�[�P���X
 *  @param  *pwk          DEMO3D_PARAM
 *  @param  *mywk         PROC���[�N
 *
 *  @retval �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Demo3DProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  DEMO3D_MAIN_WORK* wk = mywk;
  BOOL is_end;

#if 0 
  // �t�F�[�h���͏������Ȃ�
  if( WIPE_SYS_EndCheck() == FALSE ){
  {
    return GFL_PROC_RES_CONTINUE;
  }
#endif

  // ��O�����G���W�� �又��
  APP_EXCEPTION_Main( wk->expection );

  //PRINT_QUE
  PRINTSYS_QUE_Main( wk->print_que );

  //2D�`��
  DEMO3D_GRAPHIC_2D_Draw( wk->graphic );

  //3D�`��
  is_end = Demo3D_ENGINE_Main( wk->engine );

  // ���[�v���o���L�[�I���L���̎��ɂ̓L�[�ł��I��
  if(_key_check(wk)){
    // [OUT] �t���[���l��ݒ�
    wk->param->end_frame  = DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT; 
    wk->param->result     = DEMO3D_RESULT_USER_END;
    DEMO3D_ENGINE_SetEndResult( wk->engine, wk->param->result );
    return GFL_PROC_RES_FINISH;
  }
  else if( is_end )
  {
    // [OUT] �t���[���l��ݒ�
    wk->param->end_frame  = DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT; 
    wk->param->result     = DEMO3D_RESULT_FINISH;
    DEMO3D_ENGINE_SetEndResult( wk->engine, wk->param->result );
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}


//=============================================================================
/**
 *                static�֐�
 */
//=============================================================================

//----------------------------------------------------------------------------------
/*
 *  @brief  �t�F�[�h�C��/�A�E�g���N�G�X�g
 */
//----------------------------------------------------------------------------------
static BOOL sub_FadeInOutReq( u8 demo_id, u8 wipe, HEAPID heapID )
{
  u8  type, sync;
  u16 color;

  Demo3D_DATA_GetFadeParam( demo_id, (wipe == WIPE_TYPE_FADEOUT), &type, &sync );
  
  if( type == DEMO3D_FADE_BLACK ){
    color = WIPE_FADE_BLACK;
  }else{
    color = WIPE_FADE_WHITE;
  }
  if( sync > 0 ){
    WIPE_SYS_Start( WIPE_PATTERN_WMS, wipe, wipe, color, sync, 1, heapID );
    return FALSE;
  }
  WIPE_SetBrightnessFadeOut( color );
  return TRUE;
}

//----------------------------------------------------------------------------------
/**
 * @brief �L�[�`�F�b�N�ikey_skip��TRUE�������f���I���ł���)
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _key_check( DEMO3D_MAIN_WORK *wk )
{
  if( WIPE_SYS_EndCheck() == FALSE ){
    return FALSE;
  }
  if( wk->expection->key_skip )
  {
    if( GFL_UI_KEY_GetTrg() & 
        (  PAD_BUTTON_DECIDE 
        | PAD_BUTTON_CANCEL 
        | PAD_BUTTON_X 
        | PAD_BUTTON_Y
        | PAD_KEY_UP 
        | PAD_KEY_DOWN 
        | PAD_KEY_LEFT 
        | PAD_KEY_RIGHT )
    )
    {
      return TRUE;
    }
        
  }

#ifdef PM_DEBUG
  if( (GFL_UI_KEY_GetCont() &PAD_BUTTON_X) && (GFL_UI_KEY_GetCont() &PAD_BUTTON_Y)){
    return TRUE;
  }
#endif
  return FALSE;
}

//-----------------------------------------------------------------------------
// �A�v�����Ƃ̗�O�\��
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�v����O�����G���W�� ����
 *
 *  @param  DEMO3D_ID demo_id
 *  @param  heapID 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static APP_EXCEPTION_WORK* APP_EXCEPTION_Create( DEMO3D_ID demo_id, DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_ENGINE_WORK* engine, HEAPID heapID )
{
  APP_EXCEPTION_WORK* wk;

  GF_ASSERT( graphic );
  GF_ASSERT( engine );
  
  // ���C�����[�N �A���P�[�g
  wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(APP_EXCEPTION_WORK) );

  // �����o������
  wk->graphic = graphic;
  wk->engine = engine;
  wk->heapID = heapID;

  wk->p_funcset = NULL;
  wk->key_skip  = FALSE;

  // �V���D
  switch( demo_id )
  {
  case DEMO3D_ID_C_CRUISER :
    wk->p_funcset = &DATA_c_exp_funcset_c_cruiser;
    wk->key_skip  = TRUE;                     // �L�[�X�L�b�v�L��
    break;
  default : 
    ;
  }

  // ����������
  if( wk->p_funcset )
  {
    wk->p_funcset->Init( wk );
  }

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�v����O�����G���W�� �폜
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void APP_EXCEPTION_Delete( APP_EXCEPTION_WORK* wk )
{
  GF_ASSERT(wk);

  // �I������
  if( wk->p_funcset )
  {
    wk->p_funcset->End( wk );
  }

  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�v����O�����G���W�� �又��
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void APP_EXCEPTION_Main( APP_EXCEPTION_WORK* wk )
{
  GF_ASSERT(wk);

  // �又��
  if( wk->p_funcset )
  {
    wk->p_funcset->Main( wk );
  }
}


