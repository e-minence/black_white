//=============================================================================
/**
 *
 *  @file   demo3d.c
 *  @brief  3D�f���Đ��A�v��
 *  @author genya hosaka
 *  @data   2009.11.27
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

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

//�^�b�`�o�[
#include "ui/touchbar.h"

//INFOWIN
#include "infowin/infowin.h"

//�`��ݒ�
#include "demo3d_graphic.h"

//�A�[�J�C�u
#include "arc_def.h"

//�O�����J
#include "demo/demo3d.h"

#include "message.naix"
#include "townmap_gra.naix"

#include "demo3d_engine.h"

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
FS_EXTERN_OVERLAY(ui_common);

#define CHECK_KEY_TRG( key ) ( ( GFL_UI_KEY_GetTrg() & (key) ) == (key) )

//=============================================================================
/**
 *                �萔��`
 */
//=============================================================================
enum
{ 
  DEMO3D_HEAP_SIZE = 0x110000,  ///< �q�[�v�T�C�Y
};

//-------------------------------------
/// �t���[��
//=====================================
enum
{ 
  BG_FRAME_BAR_M  = GFL_BG_FRAME1_M,
  BG_FRAME_POKE_M = GFL_BG_FRAME2_M,
  BG_FRAME_BACK_M = GFL_BG_FRAME3_M,
  BG_FRAME_BACK_S = GFL_BG_FRAME2_S,
  BG_FRAME_TEXT_S = GFL_BG_FRAME0_S, 
};
//-------------------------------------
/// �p���b�g
//=====================================
enum
{ 
  //���C��BG
  PLTID_BG_BACK_M       = 0,
  PLTID_BG_POKE_M       = 1,
  PLTID_BG_TASKMENU_M   = 11,
  PLTID_BG_TOUCHBAR_M   = 13,
  PLTID_BG_INFOWIN_M    = 15,
  //�T�uBG
  PLTID_BG_BACK_S       = 0,

  //���C��OBJ
  PLTID_OBJ_TOUCHBAR_M  = 0, // 3�{�g�p
  PLTID_OBJ_TOWNMAP_M = 14,   

  //�T�uOBJ
  PLTID_OBJ_COMMON_S = 0,
};

//=============================================================================
/**
 *                �\���̒�`
 */
//=============================================================================

// �s���S�^
typedef struct _APP_EXCEPTION_WORK APP_EXCEPTION_WORK;

//--------------------------------------------------------------
/// ��O���� �֐���`
//==============================================================
typedef void (*APP_EXCEPTION_FUNC)( APP_EXCEPTION_WORK* wk );

//--------------------------------------------------------------
/// ��O���� �֐��e�[�u��
//==============================================================
typedef struct {
  APP_EXCEPTION_FUNC Init;
  APP_EXCEPTION_FUNC Main;
  APP_EXCEPTION_FUNC End;
} APP_EXCEPTION_FUNC_SET;

//--------------------------------------------------------------
/// �A�v����O����
//==============================================================
struct _APP_EXCEPTION_WORK {
  //[IN]
  HEAPID heapID;
  const DEMO3D_GRAPHIC_WORK* graphic;
  const DEMO3D_ENGINE_WORK* engine;

  //[PRIVATE]
  const APP_EXCEPTION_FUNC_SET* p_funcset;
  void *userwork;
  int  key_skip;  // �L�[�X�L�b�v

  
};

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
static int _key_check( DEMO3D_MAIN_WORK *wk );

//-----------------------------------------------------------
// �V���D�����
//-----------------------------------------------------------
#define C_CRUISER_POS_FLASH_SYNC (100) ///< �X�V�Ԋu

//--------------------------------------------------------------
/// ���W�e�[�u��
//==============================================================
static const GFL_CLACTPOS g_c_cruiser_postbl[] = 
{
  { 13*8,  17*8, },   //0F
  { 13*8,  17*8-4, },
  { 13*8,  16*8, },
  { 13*8,  16*8-4, },
  { 13*8,  15*8, },
  { 13*8,  15*8-4, },
  { 13*8,  14*8, },
  { 13*8,  14*8-4, },
  { 13*8,  13*8, },
  { 13*8,  13*8-4, },
  { 13*8,  12*8, },   //1000F �X�J�C�A���[�u���b�W
  { 13*8,  12*8-4, },
  { 13*8,  11*8, },
  { 13*8,  11*8-4, },
  { 13*8,  10*8, },
  { 13*8,  10*8-4, },
  { 13*8,  9*8, },
  { 13*8,  9*8-4, },
  { 13*8,  8*8, },
  { 13*8,  8*8-4, },
  { 13*8,  7*8, },    //2000F
  { 13*8,  7*8-4, },
  { 13*8,  6*8, },
  { 13*8,  6*8-4, },
  { 13*8,  5*8, },    //2400F �X�^�[���C��
  { 14*8,  5*8-4, },
  { 15*8,  5*8-4, },
  { 16*8,  5*8, },    //2700F �܂�Ԃ��n�_
  { 16*8,  6*8-4, },
  { 16*8,  6*8, },
  { 16*8,  7*8-4, },    //3000F
  { 16*8,  7*8, },
  { 16*8,  8*8-4, },
  { 16*8,  8*8, },
  { 16*8,  9*8-4, },
  { 16*8,  9*8, },
  { 16*8,  10*8-4, },
  { 16*8,  10*8, },
  { 16*8,  11*8-4, },
  { 16*8,  11*8, },
  { 16*8,  12*8-4, },   //4000F �X�J�C�A���[�u���b�W
  { 16*8,  12*8, },
  { 16*8,  13*8-4, },
  { 16*8,  13*8, },
  { 16*8,  14*8-4, },
  { 16*8-4,  14*8, },
  { 15*8,  15*8-4, },
  { 15*8-4,  15*8, },
  { 14*8,  16*8-4, },
  { 14*8-4,  16*8, },
  { 13*8,  17*8-4, },   //5000F
  { 13*8-4,  17*8, },
  { 12*8,  17*8, },
  { 12*8-4,  17*8, },
};

//--------------------------------------------------------------
/// �V���D�p���[�N(��O�����̃��[�U�[���[�N)
//==============================================================
typedef struct {
  u8 pos_id;
  u8 padding[3];
  UI_EASY_CLWK_RES clwk_res;
  GFL_CLWK* clwk_marker;
} EXP_C_CRUISER_WORK;

static void EXP_C_CRUISER_Init( APP_EXCEPTION_WORK* wk );
static void EXP_C_CRUISER_Main( APP_EXCEPTION_WORK* wk );
static void EXP_C_CRUISER_End( APP_EXCEPTION_WORK* wk );

static const APP_EXCEPTION_FUNC_SET c_exp_funcset_c_cruiser = 
{
  EXP_C_CRUISER_Init,
  EXP_C_CRUISER_Main,
  EXP_C_CRUISER_End,
};

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

  //3D ������
  wk->engine = Demo3D_ENGINE_Init( wk->graphic, param->demo_id, param->start_frame, wk->heapID );

  // BG/OBJ���\���ɂ��Ă���
  GFL_BG_SetVisible( BG_FRAME_BACK_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_TEXT_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_BACK_M, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_POKE_M, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_BAR_M,  VISIBLE_OFF );
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
  
  // �t�F�[�h�C�� ���N�G�X�g
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 2 );

  // �f�����̗�O�����G���W��������
  wk->expection = APP_EXCEPTION_Create( param->demo_id, wk->graphic, wk->engine, wk->heapID );


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
  
  if( *seq == 0 )
  {
    // �t�F�[�h�A�E�g ���N�G�X�g
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 2 );
    (*seq)++;
    return GFL_PROC_RES_CONTINUE;
  }
  else if( GFL_FADE_CheckFade() == TRUE )
  {
    // �t�F�[�h���͏����ɓ���Ȃ�
    return GFL_PROC_RES_CONTINUE;
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

  // �t�F�[�h���͏������Ȃ�
  if( GFL_FADE_CheckFade() == TRUE )
  {
    return GFL_PROC_RES_CONTINUE;
  }

  // ��O�����G���W�� �又��
  APP_EXCEPTION_Main( wk->expection );

  //PRINT_QUE
  PRINTSYS_QUE_Main( wk->print_que );

  //2D�`��
  DEMO3D_GRAPHIC_2D_Draw( wk->graphic );

  //3D�`��
  is_end = Demo3D_ENGINE_Main( wk->engine );

  // ���[�v���o���L�[�I���L���̎��ɂ̓L�[�ł��I��
  if( is_end || _key_check(wk))
  {
    // [OUT] �t���[���l��ݒ�
    wk->param->end_frame  = DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT; 
    wk->param->result     = DEMO3D_RESULT_FINISH;
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
  if( wk->expection->key_skip )
  {
    if( GFL_UI_KEY_GetTrg() & 
          PAD_BUTTON_DECIDE 
        | PAD_BUTTON_CANCEL 
        | PAD_BUTTON_X 
        | PAD_BUTTON_Y
        | PAD_KEY_UP 
        | PAD_KEY_DOWN 
        | PAD_KEY_LEFT 
        | PAD_KEY_RIGHT 
    )
    {
      return TRUE;
    }
        
  }
  
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
    wk->p_funcset = &c_exp_funcset_c_cruiser; // ����ʓo�^
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




//-----------------------------------------------------------------------------
// �V���D �����
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *  @brief  BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *  @param  DEMO3D_BG_WORK* wk BG�Ǘ����[�N
 *  @param  heapID  �q�[�vID 
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void _C_CRUISER_LoadBG( HEAPID heapID )
{
  ARCHANDLE *handle;

  handle  = GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heapID );

  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_townmap_gra_tmap_ship_NCLR,
      PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0, heapID );
  
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_townmap_gra_tmap_ship_NCGR,
            BG_FRAME_BACK_S, 0, 0, 0, heapID );
  GFL_ARCHDL_UTIL_TransVramScreen(  handle, NARC_townmap_gra_tmap_ship_NSCR,
            BG_FRAME_BACK_S, 0, 0, 0, heapID ); 

  GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ��O���� �V���D ������
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void EXP_C_CRUISER_Init( APP_EXCEPTION_WORK* wk )
{
  EXP_C_CRUISER_WORK* uwk;

  wk->userwork = GFL_HEAP_AllocClearMemory( wk->heapID, sizeof(EXP_C_CRUISER_WORK) );
  
  uwk = wk->userwork;

  _C_CRUISER_LoadBG( wk->heapID );

  {
    GFL_CLUNIT* clunit;
    UI_EASY_CLWK_RES_PARAM prm;

    clunit  = DEMO3D_GRAPHIC_GetClunit( wk->graphic );

    prm.draw_type = CLSYS_DRAW_SUB;
    prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
    prm.arc_id    = ARCID_TOWNMAP_GRAPHIC;
    prm.pltt_id   = NARC_townmap_gra_tmap_ship_obj_NCLR;
    prm.ncg_id    = NARC_townmap_gra_tmap_ship_obj_NCGR;
    prm.cell_id   = NARC_townmap_gra_tmap_ship_obj_NCER;
    prm.anm_id    = NARC_townmap_gra_tmap_ship_obj_NANR;
    prm.pltt_line = PLTID_OBJ_COMMON_S;
    prm.pltt_src_ofs = 0;
    prm.pltt_src_num = 1;

    UI_EASY_CLWK_LoadResource( &uwk->clwk_res, &prm, clunit, wk->heapID );

    uwk->clwk_marker = UI_EASY_CLWK_CreateCLWK( &uwk->clwk_res, clunit, 40, 40, 0, wk->heapID );
      
    GFL_CLACT_WK_SetDrawEnable( uwk->clwk_marker, TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( uwk->clwk_marker, TRUE );

  }
  
  GFL_BG_SetVisible( BG_FRAME_BACK_S, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ��O���� �V���D �又��
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void EXP_C_CRUISER_Main( APP_EXCEPTION_WORK* wk )
{
  int frame;
  EXP_C_CRUISER_WORK* uwk = wk->userwork;
  
  frame = DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT;
  if( uwk->pos_id < NELEMS(g_c_cruiser_postbl) )

  {
    if( uwk->pos_id * C_CRUISER_POS_FLASH_SYNC <= frame )
    {
      // ���W�X�V
      GFL_CLACT_WK_SetPos( uwk->clwk_marker, &g_c_cruiser_postbl[ uwk->pos_id ], CLSYS_DRAW_SUB );
      GFL_CLACT_WK_SetAnmFrame( uwk->clwk_marker, 0 );

      OS_TPrintf("frame=%d marker pos_id=%d x=%d, y=%d\n", 
          frame, uwk->pos_id, 
          g_c_cruiser_postbl[uwk->pos_id].x,
          g_c_cruiser_postbl[uwk->pos_id].y );

      uwk->pos_id++;

      // �I������
      if( uwk->pos_id == NELEMS(g_c_cruiser_postbl) )
      {
        // ��\����
        GFL_CLACT_WK_SetDrawEnable( uwk->clwk_marker, FALSE );
        OS_TPrintf("marker visible off \n");
      }
    }
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief  ��O���� �V���D �������
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void EXP_C_CRUISER_End( APP_EXCEPTION_WORK* wk )
{
  EXP_C_CRUISER_WORK* uwk = wk->userwork;

  // OBJ���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( &uwk->clwk_res );

  // ���[�U�[���[�N���
  GFL_HEAP_FreeMemory( wk->userwork );
}

