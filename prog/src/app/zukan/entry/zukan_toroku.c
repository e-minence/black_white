//============================================================================
/**
 *  @file   zukan_toroku.c
 *  @brief  �}�ӓo�^
 *  @author Koji Kawada
 *  @data   2009.12.03
 *  @note   ui_template.c�����ɍ쐬���܂����B
 *
 *  ���W���[�����FZUKAN_TOROKU
 */
//============================================================================
// �K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "print/gf_font.h"
#include "font/font.naix"

#include "print/printsys.h"

#include "../info/zukan_info.h"
#include "zukan_nickname.h"

#include "zukan_toroku_graphic.h"
#include "app/zukan_toroku.h"

#include "arc_def.h"
#include "font/font.naix"

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define ZUKAN_TOROKU_HEAP_SIZE (0x30000)  ///< �q�[�v�T�C�Y

typedef enum
{
  ZUKAN_TOROKU_STATE_TOROKU,
  ZUKAN_TOROKU_STATE_NICKNAME,
}
ZUKAN_TOROKU_STATE;

typedef enum
{
  ZUKAN_TOROKU_TOROKU_STEP_FADE_IN,
  ZUKAN_TOROKU_TOROKU_STEP_WAIT_KEY,
}
ZUKAN_TOROKU_TOROKU_STEP;

//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// PROC���[�N
//=====================================
typedef struct
{
  HEAPID heap_id;
  ZUKAN_TOROKU_GRAPHIC_WORK* graphic;
  GFL_FONT* font;
  PRINT_QUE* print_que;

  ZUKAN_TOROKU_STATE state;
  s32 step;

  ZUKAN_INFO_WORK* zukan_info_work;
  ZUKAN_NICKNAME_WORK* zukan_nickname_work;
}
ZUKAN_TOROKU_WORK;

//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================



//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT Zukan_Toroku_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Zukan_Toroku_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Zukan_Toroku_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA ZUKAN_TOROKU_ProcData =
{
  Zukan_Toroku_ProcInit,
  Zukan_Toroku_ProcMain,
  Zukan_Toroku_ProcExit,
};

//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
ZUKAN_TOROKU_PARAM* ZUKAN_TOROKU_AllocParam( HEAPID heap_id,
                                             ZUKAN_TOROKU_LAUNCH launch )
{
  ZUKAN_TOROKU_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof(ZUKAN_TOROKU_PARAM) );
  GFL_STD_MemClear( param, sizeof(ZUKAN_TOROKU_PARAM) );
  param->launch = launch;
  return param;
}

void ZUKAN_TOROKU_FreeParam( ZUKAN_TOROKU_PARAM* param )
{
  GFL_HEAP_FreeMemory( param );
}

//-------------------------------------
/// PROC�ɓn�������̐ݒ�
//=====================================
void ZUKAN_TOROKU_SetParam( ZUKAN_TOROKU_PARAM* param,
                            ZUKAN_TOROKU_LAUNCH launch )
{
  param->launch = launch;
}

ZUKAN_TOROKU_RESULT ZUKAN_TOROKU_GetResult( ZUKAN_TOROKU_PARAM* param )
{
  return param->result; 
}

//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
static GFL_PROC_RESULT Zukan_Toroku_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZUKAN_TOROKU_PARAM* param = (ZUKAN_TOROKU_PARAM*)pwk;
  ZUKAN_TOROKU_WORK* work;

  // �I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_TOROKU, ZUKAN_TOROKU_HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(ZUKAN_TOROKU_WORK), HEAPID_ZUKAN_TOROKU );
    GFL_STD_MemClear( work, sizeof(ZUKAN_TOROKU_WORK) );
  }

  work->heap_id = HEAPID_ZUKAN_TOROKU;

  work->graphic = ZUKAN_TOROKU_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );

  work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );

  work->print_que = PRINTSYS_QUE_Create( work->heap_id );

  {
    work->state = ZUKAN_TOROKU_STATE_TOROKU;
    work->step = ZUKAN_TOROKU_TOROKU_STEP_FADE_IN;
  }

  work->zukan_info_work = ZUKAN_INFO_Init( work->heap_id, 0, 1, 2,
                                           ZUKAN_TOROKU_GRAPHIC_GetClunit(work->graphic),
                                           3, work->print_que );

  work->zukan_nickname_work = ZUKAN_NICKNAME_Init( work->heap_id, ZUKAN_TOROKU_GRAPHIC_GetClunit(work->graphic), work->font, work->print_que );
  
  //GX_SetMasterBrightness(0);
  //GXS_SetMasterBrightness(0);
  //GFL_FADE_Init( work->heap_id );  // prog/src/system/gfl_use.c��GFLUser_Main�ŏ�ɉ񂵂Ă���̂Ŋ��ɑ��݂��Ă���Ǝv����
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 4 );  // �^�����̒lor�^�����̒l, 0<��ʂ������Ă���l>, �P�x��1�ω�����̂Ɋ|����t���[�� );

  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT Zukan_Toroku_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZUKAN_TOROKU_PARAM* param = (ZUKAN_TOROKU_PARAM*)pwk;
  ZUKAN_TOROKU_WORK* work = (ZUKAN_TOROKU_WORK*)mywk;

  //GFL_FADE_Exit();

  ZUKAN_NICKNAME_Exit( work->zukan_nickname_work );
  ZUKAN_INFO_Exit( work->zukan_info_work );

  PRINTSYS_QUE_Delete( work->print_que );

  GFL_FONT_Delete( work->font );

  ZUKAN_TOROKU_GRAPHIC_Exit( work->graphic );

  {
    HEAPID heap_id = work->heap_id;
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( heap_id );
  }
  
  // �I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

  return GFL_PROC_RES_FINISH;
} 

static GFL_PROC_RESULT Zukan_Toroku_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZUKAN_TOROKU_PARAM* param = (ZUKAN_TOROKU_PARAM*)pwk;
  ZUKAN_TOROKU_WORK* work = (ZUKAN_TOROKU_WORK*)mywk;

  ZUKAN_NICKNAME_RESULT zukan_nickname_result;
  GFL_PROC_RESULT proc_res = GFL_PROC_RES_CONTINUE;




  ZUKAN_INFO_Main( work->zukan_info_work );
  zukan_nickname_result = ZUKAN_NICKNAME_Main( work->zukan_nickname_work );
  switch( zukan_nickname_result )
  {
  case ZUKAN_NICKNAME_RESULT_NO:
    param->result = ZUKAN_TOROKU_RESULT_END;
    proc_res = GFL_PROC_RES_FINISH;
    break;
  case ZUKAN_NICKNAME_RESULT_YES:
    param->result = ZUKAN_TOROKU_RESULT_NICKNAME;
    proc_res = GFL_PROC_RES_FINISH;
    break;
  }




  


  //GFL_FADE_Main();

  PRINTSYS_QUE_Main( work->print_que );

  // 2D�`��
  ZUKAN_TOROKU_GRAPHIC_2D_Draw( work->graphic );
  // 3D�`��
  ZUKAN_TOROKU_GRAPHIC_3D_StartDraw( work->graphic );
  ZUKAN_TOROKU_GRAPHIC_3D_EndDraw( work->graphic );

  return proc_res;
}















#if 0

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
#define ZUKAN_TOROKU_INFOWIN  // INFOWIN

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
enum
{ 
  ZUKAN_TOROKU_HEAP_SIZE = 0x30000,  ///< �q�[�v�T�C�Y
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
  PLTID_BG_BACK_M          =  0,
  PLTID_BG_POKE_M          =  1,
  PLTID_BG_TASKMENU_M      = 11,
  PLTID_BG_TOUCHBAR_M      = 13,
  PLTID_BG_INFOWIN_M       = 15,
  //�T�uBG
  PLTID_BG_BACK_S          =  0,

  //���C��OBJ
  PLTID_OBJ_TOUCHBAR_M     =  0,  // 3�{�g�p
  PLTID_OBJ_TYPEICON_M     =  3,  // 3�{�g�p
  PLTID_OBJ_OAM_MAPMODEL_M =  6,  // 1�{�g�p
  PLTID_OBJ_POKEICON_M     =  7,  // 3�{�g�p
  PLTID_OBJ_POKEITEM_M     = 10,  // 1�{�g�p
  PLTID_OBJ_ITEMICON_M     = 11,
  PLTID_OBJ_POKE_M         = 12,
  PLTID_OBJ_BALLICON_M     = 13,  // 1�{�g�p
  PLTID_OBJ_TOWNMAP_M      = 14,
  //�T�uOBJ
  PLTID_OBJ_PMS_DRAW       =  0,  // 5�{�g�p
};

//=============================================================================
/**
 *  �\���̒�`
 */
//=============================================================================
//--------------------------------------------------------------
/// BG�Ǘ����[�N
//==============================================================
typedef struct 
{
  int dummy;
}
ZUKAN_TOROKU_BG_WORK;

//--------------------------------------------------------------
/// ���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID                   heapID;

  ZUKAN_TOROKU_BG_WORK     wk_bg;

  // �`��ݒ�
  UI_TEMPLATE_GRAPHIC_WORK *graphic;

  // �t�H���g
  GFL_FONT                 *font;

  // �v�����g�L���[
  PRINT_QUE                *print_que;
  GFL_MSGDATA              *msg;
}
ZUKAN_TOROKU_MAIN_WORK;

//=============================================================================
/**
 *  �f�[�^��`
 */
//=============================================================================

//=============================================================================
/**
 *  �v���g�^�C�v�錾
 */
//=============================================================================
//-------------------------------------
/// PROC
//=====================================
static GFL_PROC_RESULT Zukan_Toroku_ProcInit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT Zukan_Toroku_ProcMain( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT Zukan_Toroku_ProcExit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

//-------------------------------------
/// �ėp�������[�e�B���e�B
//=====================================
static void Zukan_Toroku_BG_LoadResource( ZUKAN_TOROKU_BG_WORK *wk, HEAPID heapID );

//=============================================================================
/**
 *  �O�����J
 */
//=============================================================================
const GFL_PROC_DATA ZUKAN_TOROKU_ProcData =
{
  Zukan_Toroku_ProcInit,
  Zukan_Toroku_ProcMain,
  Zukan_Toroku_ProcExit,
};

//=============================================================================
/**
 *  PROC
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief     PROC ����������
 *
 *  @param[in] proc �v���Z�X�V�X�e��
 *  @param[in] seq  �V�[�P���X
 *  @param[in] pwk  ZUKAN_TOROKU_PARAM
 *  @param[in] mywk PROC���[�N
 *
 *  @retval    �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Zukan_Toroku_ProcInit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
  ZUKAN_TOROKU_MAIN_WORK *wk;
  ZUKAN_TOROKU_PARAM     *param;

  // �I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

  // �����擾
  param = pwk;

  // �q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_TOROKU, ZUKAN_TOROKU_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(ZUKAN_TOROKU_MAIN_WORK), HEAPID_ZUKAN_TOROKU );
  GFL_STD_MemClear( wk, sizeof(ZUKAN_TOROKU_MAIN_WORK) );

  // ������
  wk->heapID = HEAPID_ZUKAN_TOROKU;

  // �`��ݒ菉����
  wk->graphic = UI_TEMPLATE_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );

  // �t�H���g�쐬
  wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
  GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

  // ���b�Z�[�W
  wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
  NARC_message_mictest_dat, wk->heapID );

  // PRINT_QUE�쐬
  wk->print_que = PRINTSYS_QUE_Create( wk->heapID );

  // BG���\�[�X�ǂݍ���
  Ui_Test_BG_LoadResource( &wk->wk_bg, wk->heapID );

  // @todo �t�F�[�h�V�[�P���X���Ȃ��̂�
  GX_SetMasterBrightness(0);
  GXS_SetMasterBrightness(0);

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *  @brief     PROC �I������
 *
 *  @param[in] proc �v���Z�X�V�X�e��
 *  @param[in] seq  �V�[�P���X
 *  @param[in] pwk  ZUKAN_TOROKU_PARAM
 *  @param[in] mywk PROC���[�N
 *
 *  @retval    �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Zukan_Toroku_ProcExit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  ZUKAN_TOROKU_MAIN_WORK* wk = mywk;

  // ���b�Z�[�W�j��
  GFL_MSG_Delete( wk->msg );

  // PRINT_QUE
  PRINTSYS_QUE_Delete( wk->print_que );

  // FONT
  GFL_FONT_Delete( wk->font );

  // �`��ݒ�j��
  UI_TEMPLATE_GRAPHIC_Exit( wk->graphic );

  // PROC�p���������
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heapID );

  // �I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *  @brief     PROC �又��
 *
 *  @param[in] proc �v���Z�X�V�X�e��
 *  @param[in] seq  �V�[�P���X
 *  @param[in] pwk  ZUKAN_TOROKU_PARAM
 *  @param[in] mywk PROC���[�N
 *
 *  @retval    �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Zukan_Toroku_ProcMain( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  ZUKAN_TOROKU_MAIN_WORK* wk = mywk;

  // �f�o�b�O�{�^���ŃA�v���I��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    return GFL_PROC_RES_FINISH;
  }

  // PRINT_QUE
  PRINTSYS_QUE_Main( wk->print_que );

  // 2D�`��
  UI_TEMPLATE_GRAPHIC_2D_Draw( wk->graphic );

  // 3D�`��
  UI_TEMPLATE_GRAPHIC_3D_StartDraw( wk->graphic );

  UI_TEMPLATE_GRAPHIC_3D_EndDraw( wk->graphic );

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
 *  static�֐�
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief     BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *  @param[in] wk BG�Ǘ����[�N
 *  @param[in] heapID  �q�[�vID 
 *
 *  @retval    none
 */
//-----------------------------------------------------------------------------
static void Ui_Test_BG_LoadResource( ZUKAN_TOROKU_BG_WORK* wk, HEAPID heapID )
{
  // @TODO �Ƃ肠�����}�C�N�e�X�g�̃��\�[�X
  ARCHANDLE *handle;

  handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_TOROKU, heapID );

  // �㉺��ʂa�f�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_ui_test_back_bg_down_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x20, heapID );
  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_ui_test_back_bg_up_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x20, heapID );

  // ----- ����� -----
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_ui_test_back_bg_down_NCGR,
                                        BG_FRAME_BACK_S, 0, 0, 0, heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_ui_test_back_bg_down_NSCR,
                                   BG_FRAME_BACK_S, 0, 0, 0, heapID );

  // ----- ���� -----
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_ui_test_back_bg_up_NCGR,
                                        BG_FRAME_BACK_M, 0, 0, 0, heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_ui_test_back_bg_up_NSCR,
                                   BG_FRAME_BACK_M, 0, 0, 0, heapID );

  GFL_ARC_CloseDataHandle( handle );
}

#if 0 // tab���܂��C�����Ă��Ȃ�
#ifdef ZUKAN_TOROKU_INFOWIN
//=============================================================================
/**
 *		INFOWIN
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	INFOWIN������
 *
 *	@param	GAMESYS_WORK *gamesys	�Q�[���V�X�e��
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heapID )
{	
	GAME_COMM_SYS_PTR comm;
	comm	= GAMESYSTEM_GetGameCommSysPtr(gamesys);
	INFOWIN_Init( BG_FRAME_BAR_M, PLTID_BG_INFOWIN_M, comm, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	INFOWIN�j��
 *
 *	@param	void 
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Exit( void )
{	
	INFOWIN_Exit();
}
//----------------------------------------------------------------------------
/**
 *	@brief	INFWIN���C������
 *
 *	@param	void 
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Main( void )
{	
	INFOWIN_Update();
}
#endif //ZUKAN_TOROKU_INFOWIN
#endif

#endif
