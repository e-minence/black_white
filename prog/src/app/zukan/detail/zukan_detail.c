//============================================================================
/**
 *  @file   zukan_detail.c
 *  @brief  �}�ӏڍ׉�ʋ���
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *  ���W���[�����FZUKAN_DETAIL
 */
//============================================================================
#define DEBUG_KAWADA


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "savedata/zukan_savedata.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "zukan_detail_def.h"
#include "zukan_detail_graphic.h"
#include "zukan_detail_touchbar.h"
#include "zukan_detail_headbar.h"
#include "zukan_detail_common.h"
#include "zukan_detail_procsys.h"
#include "zukan_detail_info.h"
#include "zukan_detail_map.h"
#include "zukan_detail_voice.h"
#include "zukan_detail_form.h"
#include "zukan_detail.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"

// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0x90000)               ///< �q�[�v�T�C�Y


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  HEAPID                      heap_id;
  ZUKAN_DETAIL_GRAPHIC_WORK*  graphic;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �^�b�`�o�[
  ZUKAN_DETAIL_TOUCHBAR_WORK* touchbar;
  // �^�C�g���o�[
  ZUKAN_DETAIL_HEADBAR_WORK*  headbar;
  // ���ʂ̃f�[�^���܂Ƃ߂�����
  ZKNDTL_COMMON_WORK*         cmn;

  // �}�ӏڍ׉�ʋ���PROC
  ZKNDTL_PROC*                zkndtl_proc;
  void*                       zkndtl_proc_param;
  // ���̐}�ӏڍ׉�ʋ���PROC
  ZUKAN_DETAIL_TYPE           zkndtl_proc_next_type;
}
ZUKAN_DETAIL_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Zukan_Detail_VBlankFunc( GFL_TCB* tcb, void* wk );

// ZKNDTL_PROC�̃p�����[�^��p�ӂ��AZKNDTL_PROC���쐬����
static void Zukan_Detail_CreateProc( ZUKAN_DETAIL_PARAM* param, ZUKAN_DETAIL_WORK* work );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT Zukan_Detail_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Zukan_Detail_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Zukan_Detail_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    ZUKAN_DETAIL_ProcData =
{
  Zukan_Detail_ProcInit,
  Zukan_Detail_ProcMain,
  Zukan_Detail_ProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^����
 *
 *  @param[in]       heap_id       �q�[�vID
 *  @param[in,out]   gamedata      GAMEDATA
 *
 *  @retval          ZUKAN_DETAIL_PARAM
 */
//------------------------------------------------------------------
ZUKAN_DETAIL_PARAM*  ZUKAN_DETAIL_AllocParam(
                            HEAPID               heap_id,
                            GAMEDATA*            gamedata,
                            ZUKAN_DETAIL_TYPE    type,
                            u16*                 list,
                            u16                  num,
                            u16                  no )
{
  ZUKAN_DETAIL_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof( ZUKAN_DETAIL_PARAM ) );
  ZUKAN_DETAIL_InitParam(
      param,
      gamedata,
      type,
      list,
      num,
      no );
  return param;
}

//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^���
 *
 *  @param[in,out]   param      �p�����[�^�����Ő�����������
 *
 *  @retval          
 */
//------------------------------------------------------------------
void             ZUKAN_DETAIL_FreeParam(
                            ZUKAN_DETAIL_PARAM*  param )
{
  GFL_HEAP_FreeMemory( param );
}

//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^��ݒ肷��
 *
 *  @param[in,out]   param      ZUKAN_DETAIL_PARAM
 *  @param[in,out]   gamedata   GAMEDATA
 *
 *  @retval          
 */
//------------------------------------------------------------------
void             ZUKAN_DETAIL_InitParam(
                            ZUKAN_DETAIL_PARAM*  param,
                            GAMEDATA*            gamedata,
                            ZUKAN_DETAIL_TYPE    type,
                            u16*                 list,
                            u16                  num,
                            u16                  no )
{
  param->gamedata    = gamedata;
  param->type        = type;
  param->list        = list;
  param->num         = num;
  param->no          = no;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT Zukan_Detail_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZUKAN_DETAIL_PARAM*    param    = (ZUKAN_DETAIL_PARAM*)pwk;
  ZUKAN_DETAIL_WORK*     work;

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_ProcInit\n" );
  }
#endif

  // �q�[�v
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_DETAIL, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(ZUKAN_DETAIL_WORK), HEAPID_ZUKAN_DETAIL );
    GFL_STD_MemClear( work, sizeof(ZUKAN_DETAIL_WORK) );
    
    work->heap_id       = HEAPID_ZUKAN_DETAIL;

    // �g������
    // HEAPID_ZUKAN_DETAIL         �e��ʂł悭�ς�����
    // HEAPID_ZUKAN_DETAIL_COMMON  �S��ʂŋ��ʂŎg�p�������
    // HEAPID_ZUKAN_DETAIL_SYS     �e��ʂ�1�x���߂���ς��Ȃ�����
  }

  // ���̎w����������
  param->ret = ZUKAN_DETAIL_RET_CLOSE;

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    work->graphic       = ZUKAN_DETAIL_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, work->heap_id, FALSE );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Detail_VBlankFunc, work, 1 );

  // �^�b�`�o�[
  {
    ZUKAN_SAVEDATA* zkn_sv       = GAMEDATA_GetZukanSave( param->gamedata );
    BOOL            form_version = ZUKANSAVE_GetGraphicVersionUpFlag( zkn_sv );
    work->touchbar = ZUKAN_DETAIL_TOUCHBAR_Init( work->heap_id, form_version );
  } 
  // �^�C�g���o�[
  work->headbar = ZUKAN_DETAIL_HEADBAR_Init( work->heap_id, work->font );

  // ���ʂ̃f�[�^���܂Ƃ߂�����
  work->cmn = ZKNDTL_COMMON_Init(
                  work->heap_id,
                  param->gamedata,
                  work->graphic,
                  work->font,
                  work->print_que,
                  work->touchbar,
                  work->headbar,
                  param->list,
                  param->num,
                  &(param->no) );

  // NULL������
  work->zkndtl_proc        = NULL;
  work->zkndtl_proc_param  = NULL;
  // �}�ӏڍ׉�ʋ���PROC
  //Zukan_Detail_CreateProc( proc, seq, pwk, mywk );  // ���̊֐�1��ڂ̌Ăяo�����́A�܂�mywk��work���w���Ă��Ȃ��̂ŁA�������work��n���悤�ɁI
  Zukan_Detail_CreateProc( param, work );
  // ���̐}�ӏڍ׉�ʋ���PROC
  work->zkndtl_proc_next_type = ZUKAN_DETAIL_TYPE_NONE;

  // �t�F�[�h�C��(����������)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 0, 0 );

  // �ʐM�A�C�R��
  GFL_NET_ReloadIconTopOrBottom( TRUE, work->heap_id );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT Zukan_Detail_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZUKAN_DETAIL_PARAM*    param    = (ZUKAN_DETAIL_PARAM*)pwk;
  ZUKAN_DETAIL_WORK*     work     = (ZUKAN_DETAIL_WORK*)mywk;

  // �t�F�[�h�A�E�g(�����遨��)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 16, 0 );

  // �}�ӏڍ׉�ʋ���PROC
  if( work->zkndtl_proc ) ZKNDTL_PROC_SysDeleteProc( work->zkndtl_proc );
  if( work->zkndtl_proc_param ) GFL_HEAP_FreeMemory( work->zkndtl_proc_param );
  // ���ʂ̃f�[�^���܂Ƃ߂�����
  ZKNDTL_COMMON_Exit( work->cmn );
  // �^�C�g���o�[
  ZUKAN_DETAIL_HEADBAR_Exit( work->headbar );
  // �^�b�`�o�[
  ZUKAN_DETAIL_TOUCHBAR_Exit( work->touchbar );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Clear( work->print_que );
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    ZUKAN_DETAIL_GRAPHIC_Exit( work->graphic );
  }

  // �ʐM�A�C�R��
  // �I������Ƃ��͒ʐM�A�C�R���ɑ΂��ĉ������Ȃ�

  // �q�[�v
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_ZUKAN_DETAIL );
  }

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_ProcExit\n" );
  }
#endif

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT Zukan_Detail_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZUKAN_DETAIL_PARAM*    param    = (ZUKAN_DETAIL_PARAM*)pwk;
  ZUKAN_DETAIL_WORK*     work     = (ZUKAN_DETAIL_WORK*)mywk;

  BOOL create_proc_instant = FALSE;

  if( ZKNDTL_PROC_SysMain(work->zkndtl_proc, work->cmn) )
  {
    if( work->zkndtl_proc_next_type != ZUKAN_DETAIL_TYPE_NONE )
    {
      param->type = work->zkndtl_proc_next_type;
      Zukan_Detail_CreateProc( param, work );
      work->zkndtl_proc_next_type = ZUKAN_DETAIL_TYPE_NONE;
  
      create_proc_instant = TRUE;
    }
    else
    {
      return GFL_PROC_RES_FINISH;
    }
  }

  if( !create_proc_instant )
  {
    ZKNDTL_COMMAND cmd;
    cmd = ZUKAN_DETAIL_TOUCHBAR_GetTrg( work->touchbar );
    if( cmd == ZKNDTL_CMD_NONE )  // Trg��Touch���D�悳����
    {
      cmd = ZUKAN_DETAIL_TOUCHBAR_GetTouch( work->touchbar );
    }
    ZKNDTL_PROC_SysCommand(work->zkndtl_proc, work->cmn, cmd);
    
    switch( cmd )
    {
    case ZKNDTL_CMD_CLOSE:   param->ret                      = ZUKAN_DETAIL_RET_CLOSE;     break;
    case ZKNDTL_CMD_RETURN:  param->ret                      = ZUKAN_DETAIL_RET_RETURN;    break;
    case ZKNDTL_CMD_INFO:    work->zkndtl_proc_next_type     = ZUKAN_DETAIL_TYPE_INFO;     break;
    case ZKNDTL_CMD_MAP:     work->zkndtl_proc_next_type     = ZUKAN_DETAIL_TYPE_MAP;      break;
    case ZKNDTL_CMD_VOICE:   work->zkndtl_proc_next_type     = ZUKAN_DETAIL_TYPE_VOICE;    break;
    case ZKNDTL_CMD_FORM:    work->zkndtl_proc_next_type     = ZUKAN_DETAIL_TYPE_FORM;     break;
    }
  }
  
  ZUKAN_DETAIL_TOUCHBAR_Main( work->touchbar );
  ZUKAN_DETAIL_HEADBAR_Main( work->headbar );
  
  PRINTSYS_QUE_Main( work->print_que );

  // 2D�`��
  ZUKAN_DETAIL_GRAPHIC_2D_Draw( work->graphic );

  // 3D�`��
  ZUKAN_DETAIL_GRAPHIC_3D_StartDraw( work->graphic );
  if( !create_proc_instant )
  {
    ZKNDTL_PROC_SysDraw3D(work->zkndtl_proc, work->cmn);
  }
  ZUKAN_DETAIL_GRAPHIC_3D_EndDraw( work->graphic );

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Zukan_Detail_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_DETAIL_WORK* work = (ZUKAN_DETAIL_WORK*)wk;
}

//-------------------------------------
/// ZKNDTL_PROC�̃p�����[�^��p�ӂ��AZKNDTL_PROC���쐬����
//=====================================
static void Zukan_Detail_CreateProc( ZUKAN_DETAIL_PARAM* param, ZUKAN_DETAIL_WORK* work )
{
  const ZKNDTL_PROC_DATA* zkndtl_proc_data;

  // �O�̂��폜����
  if( work->zkndtl_proc ) ZKNDTL_PROC_SysDeleteProc( work->zkndtl_proc );
  if( work->zkndtl_proc_param ) GFL_HEAP_FreeMemory( work->zkndtl_proc_param );

  // �ǂ̉�ʂ�\�����邩
  switch( param->type )
  {
  case ZUKAN_DETAIL_TYPE_INFO:
    {
      ZUKAN_DETAIL_INFO_PARAM* zkndtl_proc_param = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(ZUKAN_DETAIL_INFO_PARAM) );
      ZUKAN_DETAIL_INFO_InitParam( zkndtl_proc_param, work->heap_id );
      work->zkndtl_proc_param = zkndtl_proc_param;
      zkndtl_proc_data = &ZUKAN_DETAIL_INFO_ProcData;
    }
    break;
  case ZUKAN_DETAIL_TYPE_MAP:
    {
      ZUKAN_DETAIL_MAP_PARAM* zkndtl_proc_param = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(ZUKAN_DETAIL_MAP_PARAM) );
      ZUKAN_DETAIL_MAP_InitParam( zkndtl_proc_param, work->heap_id );
      work->zkndtl_proc_param = zkndtl_proc_param;
      zkndtl_proc_data = &ZUKAN_DETAIL_MAP_ProcData;
    }
    break;
  case ZUKAN_DETAIL_TYPE_VOICE:
    {
      ZUKAN_DETAIL_VOICE_PARAM* zkndtl_proc_param = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(ZUKAN_DETAIL_VOICE_PARAM) );
      ZUKAN_DETAIL_VOICE_InitParam( zkndtl_proc_param, work->heap_id ); 
      work->zkndtl_proc_param = zkndtl_proc_param;
      zkndtl_proc_data = &ZUKAN_DETAIL_VOICE_ProcData;
    }
    break;
  case ZUKAN_DETAIL_TYPE_FORM:
    {
      ZUKAN_DETAIL_FORM_PARAM* zkndtl_proc_param = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(ZUKAN_DETAIL_FORM_PARAM) );
      ZUKAN_DETAIL_FORM_InitParam( zkndtl_proc_param, work->heap_id );
      work->zkndtl_proc_param = zkndtl_proc_param;
      zkndtl_proc_data = &ZUKAN_DETAIL_FORM_ProcData;
    }
    break;
  }

  // �}�ӏڍ׉�ʋ���PROC
  work->zkndtl_proc = ZKNDTL_PROC_SysCreateProc( zkndtl_proc_data, work->zkndtl_proc_param, work->heap_id );
}

