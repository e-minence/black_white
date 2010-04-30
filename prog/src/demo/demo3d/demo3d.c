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
#include "system/brightness.h"

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
#include "sound/wb_sound_data.sadl"
#include "sound/pm_sndsys.h" // for SEQ_SE_XXX

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
  
  u32 oldVCount;
  u32 delayVCount;
  u32 delayVCountTotal;
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

#ifdef PM_DEBUG
static void debug_FldmmdlAnmCodeCheck( const GAMESYS_WORK* gsys );
#endif  //PM_DEBUG

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
  DEMO3D_PARAM *param = pwk;
  
  //�I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

  GF_ASSERT( param->demo_id != DEMO3D_ID_NULL && param->demo_id < DEMO3D_ID_MAX );
#ifdef PM_DEBUG
  debug_FldmmdlAnmCodeCheck( param->gsys );
#endif

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

  //�u���C�g�l�X�������r���I�������ꍇ�΍�̃��Z�b�g
  BrightnessChgReset(MASK_DOUBLE_DISPLAY);
#if 1
  //�T�E���h�������X�g�b�v
  PMSND_StopSE_byPlayerID( PLAYER_SE_SYS );
  PMSND_StopSE_byPlayerID( PLAYER_SE_1 );
  PMSND_StopSE_byPlayerID( PLAYER_SE_2 );
  PMSND_StopSE_byPlayerID( PLAYER_SE_PSG );
#endif
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
  GF_ASSERT_MSG( GFL_HEAP_CheckHeapSafe( HEAPID_DEMO3D ) == TRUE,"Demo3D HeapError!\n" );

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

  // ��O�����G���W�� �又��
  APP_EXCEPTION_Main( wk->expection );

  //PRINT_QUE
  PRINTSYS_QUE_Main( wk->print_que );

  //2D�`��
  DEMO3D_GRAPHIC_2D_Draw( wk->graphic );

  //3D�`��
  is_end = Demo3D_ENGINE_Main( wk->engine, wk->delayVCount );
 
  { //�������������o
    u32 nowVCount = OS_GetVBlankCount();
    u32 subVCount = nowVCount - wk->oldVCount;
  
    wk->delayVCount = 0;
    if( wk->oldVCount > 0 && subVCount > 1 )
    {
      wk->delayVCount = (subVCount-1);
      OS_TPrintf("%d %d\n",subVCount-1, DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT);
    }
    wk->oldVCount = nowVCount;
    wk->delayVCountTotal += wk->delayVCount;
  }

  switch( *seq ){
  case 0:
    // ���[�v���o���L�[�I���L���̎��ɂ̓L�[�ł��I��
    if(_key_check(wk) || is_end ){
      // [OUT] �t���[���l��ݒ�
      wk->param->end_frame  = DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT; 
      wk->param->result     = DEMO3D_RESULT_USER_END + is_end;
      DEMO3D_ENGINE_SetEndResult( wk->engine, wk->param->result );
     
//      MI_CpuClear32( (void*)0x05000200, 4);
      #ifdef PM_DEBUG
        OS_TPrintf("# Demo3D EndFrame = [%d] ���������t���[���� [%d]!!\n",wk->param->end_frame, wk->delayVCountTotal);
      #else
        #ifdef DEBUG_ONLY_FOR_iwasawa
        GF_ASSERT_MSG((!wk->delayVCountTotal),"AnmDelay %d please push(L+R+X+Y)\n",wk->delayVCountTotal);
        #endif
      #endif
    
      // �t�F�[�h�A�E�g ���N�G�X�g
      if( sub_FadeInOutReq( wk->param->demo_id, WIPE_TYPE_FADEOUT, wk->heapID )){
        return GFL_PROC_RES_FINISH;
      }
      (*seq)++;
    }
    break;
  case 1:
    OS_TPrintf("# Demo3D EndOutFrame = [%d] \n",DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT );
    if( WIPE_SYS_EndCheck() == TRUE ){
      return GFL_PROC_RES_FINISH;
    }
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
  WIPE_SetBrightnessFadeOut( color );
  if( sync > 0 ){
    u8 spd = GFL_FADE_GetFadeSpeed();
    WIPE_SYS_Start( WIPE_PATTERN_WMS, wipe, wipe, color, sync*spd, 1, heapID );
    return FALSE;
  }
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
  if( wk->param->result != DEMO3D_RESULT_NULL || WIPE_SYS_EndCheck() == FALSE ){
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

#ifdef PM_DEBUG
#include "arc/fieldmap/fldmmdl_objcode.h"
#include "field/fldmmdl_list_symbol.h"
#include "field/fldmmdl.h"
/*
 *  @brief  ���p���Ă���fldmmdl�̃A�j��ID���Ademo3d���c�����Ă���^�C�v��
 *        �@��v���Ă��邩�ǂ������`�F�b�N
 */
static void debug_FldmmdlAnmCodeCheck( const GAMESYS_WORK* gsys )
{
  int i;
  const OBJCODE_PARAM* obj_prm;
  const MMDLSYS *mmdlsys = GAMEDATA_GetMMdlSys( GAMESYSTEM_GetGameData( (GAMESYS_WORK*)gsys ));

  static const u16 checkTbl[][2] = {
    HERO,     ANMID_HERO,
    HEROINE,  ANMID_HERO,
		BOSS,     ANMID_BOSS_N,
		DANCER,   ANMID_BLACT_FLIP,
		TRAINERM, ANMID_BLACT_FLIP,
    WAITRESS, ANMID_BLACT_FLIP,
    BOY4,     ANMID_BLACT_NONFLIP,
    GIRL2,    ANMID_BLACT_FLIP,
    MOUNTMAN, ANMID_BLACT_FLIP,
    OL,       ANMID_BLACT_NONFLIP,
    BABYGIRL2,ANMID_BLACT_FLIP,
  };

  for(i = 0;i < NELEMS(checkTbl);i++){
    obj_prm = MMDLSYS_GetOBJCodeParam( mmdlsys, checkTbl[i][0] );
    if( obj_prm->anm_id != checkTbl[i][1]){
      GF_ASSERT_MSG(0,"Demo3D Fldmmdl anmType err idx=%d -> %d \n",i, checkTbl[i][0]);
    }
  }
}

#endif  //PM_DEBUG


