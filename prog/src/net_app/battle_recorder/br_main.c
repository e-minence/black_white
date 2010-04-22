//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   battle_recorder.c
 *  @brief  �o�g�����R�[�_�[���C��
 *  @author Toru=Nagihashi
 *  @data   2009.11.09
 *
 *  ���̃v���Z�X�́A�e�A�v���P�[�V�����v���Z�X���q��������
 *  �A�v���P�[�V�����Ԃ̏��̂��Ƃ�����邽�߂ɑ��݂���B
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�v���Z�X
#include "gamesystem/btl_setup.h"
#include "battle/battle.h"
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"

//�����̃��W���[��
#include "br_core.h"
#include "br_data.h"
#include "br_snd.h"

//�O�����J
#include "net_app/battle_recorder.h"

//-------------------------------------
/// �R�A�v���Z�X
//=====================================
FS_EXTERN_OVERLAY( battle_recorder_core );

//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================
//-------------------------------------
/// �f�o�b�O
//=====================================
#ifdef PM_DEBUG
static int s_debug_flag = 0;
#endif //PM_DEBUG

//=============================================================================
/**
 *          �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// �T�u�v���b�N�ړ�
//=====================================
typedef void *(*SUBPROC_ALLOC_FUNCTION)( HEAPID heapID, void *p_wk_adrs, u32 pre_procID );
typedef void (*SUBPROC_FREE_FUNCTION)( void *p_param, void *p_wk_adrs );
typedef struct
{
  FSOverlayID             ov_id;
  const GFL_PROC_DATA     *cp_procdata;
  SUBPROC_ALLOC_FUNCTION  alloc_func;
  SUBPROC_FREE_FUNCTION   free_func;
} SUBPROC_DATA;
typedef struct {
  GFL_PROCSYS     *p_procsys;
  u32             seq;
  void            *p_proc_param;
  SUBPROC_DATA    *p_data;

  HEAPID          heapID;
  void            *p_wk_adrs;
  const SUBPROC_DATA      *cp_procdata_tbl;

  u32             pre_procID;
  u32             next_procID;
  u32             now_procID;
} SUBPROC_WORK;

//-------------------------------------
/// ���C�����[�N
//=====================================
typedef struct
{
  //�T�u�v���Z�X�V�X�e��
  SUBPROC_WORK          subproc;

  //�풓�ɂ����f�[�^  �icore�͐퓬�ɂ����Ɖ�������̂Ŏc���Ă����������̂͂����Ɂj
  BR_DATA               data;

  //����
  BATTLERECORDER_PARAM  *p_param;
} BR_SYS_WORK;

//=============================================================================
/**
 *          �v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
/// BR���C���v���Z�X
//=====================================
static GFL_PROC_RESULT BR_SYS_PROC_Init
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_SYS_PROC_Exit
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_SYS_PROC_Main
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
/// �T�u�v���Z�X
//=====================================
static void SUBPROC_Init( SUBPROC_WORK *p_wk, const SUBPROC_DATA *cp_procdata_tbl, void *p_wk_adrs, HEAPID heapID );
static BOOL SUBPROC_Main( SUBPROC_WORK *p_wk );
static void SUBPROC_Exit( SUBPROC_WORK *p_wk );
static void SUBPROC_CallProc( SUBPROC_WORK *p_wk, u32 procID );

//-------------------------------------
/// �T�u�v���Z�X�p�����̉���A�j���֐�
//=====================================
//�o�g�����R�[�_�[�R�A
static void *BR_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs, u32 pre_procID );
static void BR_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//�퓬
static void *BR_BATTLE_AllocParam( HEAPID heapID, void *p_wk_adrs, u32 pre_procID );
static void BR_BATTLE_FreeParam( void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *          �f�[�^
*/
//=============================================================================
//-------------------------------------
/// �T�u�v���Z�X�ړ��f�[�^
//=====================================
typedef enum
{
  SUBPROCID_CORE,
  SUBPROCID_BTLREC,

  SUBPROCID_MAX
} SUBPROC_ID;
static const SUBPROC_DATA sc_subproc_data[SUBPROCID_MAX]  =
{
  //SUBPROCID_CORE
  {
    FS_OVERLAY_ID(battle_recorder_core),
    &BR_CORE_ProcData,
    BR_CORE_AllocParam,
    BR_CORE_FreeParam,
  },
  //SUBPROCID_BTLREC
  {
    FS_OVERLAY_ID(battle),
    &BtlProcData,
    BR_BATTLE_AllocParam,
    BR_BATTLE_FreeParam,
  },
};

//=============================================================================
/**
 *          �O���Q��
*/
//=============================================================================
//-------------------------------------
/// �o�g�����R�[�_�[�v���Z�X
//=====================================
const GFL_PROC_DATA BattleRecorder_ProcData =
{
  BR_SYS_PROC_Init,
  BR_SYS_PROC_Main,
  BR_SYS_PROC_Exit,
};

//=============================================================================
/**
 *          BR���C���v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g�����R�[�_�[PROC�ǂݑւ��pPROC  ������
 *
 *  @param  GFL_PROC *p_proc  �v���Z�X
 *  @param  *p_seq            �V�[�P���X
 *  @param  *p_param          �e���[�N
 *  @param  *p_work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_SYS_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  BR_SYS_WORK *p_wk;

  //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_RECORDER_SYS, 0x12000 );

  //�v���Z�X���[�N�쐬
  p_wk  = GFL_PROC_AllocWork( p_proc, sizeof(BR_SYS_WORK), HEAPID_BATTLE_RECORDER_SYS );
  GFL_STD_MemClear( p_wk, sizeof(BR_SYS_WORK) );
  p_wk->p_param   = p_param_adrs;

  //�^��f�[�^�J�n
  BattleRec_Init( HEAPID_BATTLE_RECORDER_SYS );

  //���W���[���쐬
  SUBPROC_Init( &p_wk->subproc, sc_subproc_data, p_wk, HEAPID_BATTLE_RECORDER_SYS );
  SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );

#ifdef PM_DEBUG
  if( p_wk->p_param->p_gamedata== NULL )
  {
    p_wk->p_param->p_gamedata = GAMEDATA_Create( HEAPID_BATTLE_RECORDER_SYS );
    s_debug_flag  = TRUE;
  }
#endif

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g�����R�[�_�[PROC�ǂݑւ��pPROC  �j��
 *
 *  @param  GFL_PROC *p_proc  �v���Z�X
 *  @param  *p_seq            �V�[�P���X
 *  @param  *p_param          �e���[�N
 *  @param  *p_work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_SYS_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  BR_SYS_WORK *p_wk = p_wk_adrs;

#ifdef PM_DEBUG
  if( s_debug_flag )
  {
    GAMEDATA_Delete( p_wk->p_param->p_gamedata );
    s_debug_flag  = FALSE;
  }
#endif

  //�풓�f�[�^�ŃA���b�N���Ă������̂�j��
  if( p_wk->data.rec_saveinfo.is_check )
  { 
    int i;
    for( i = 0; i < BR_SAVEDATA_NUM; i++ )
    { 
      if( p_wk->data.rec_saveinfo.p_name[i] )
      { 
        GFL_STR_DeleteBuffer( p_wk->data.rec_saveinfo.p_name[i] );
      }
    }
  }

  //���W���[���j��
  SUBPROC_Exit( &p_wk->subproc );

  //�^��f�[�^�I��
  BattleRec_Exit();

  //�v���Z�X���[�N�j��
  GFL_PROC_FreeWork( p_proc );

  //�q�[�v�j��
  GFL_HEAP_DeleteHeap( HEAPID_BATTLE_RECORDER_SYS );

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g�����R�[�_�[PROC�ǂݑւ��pPROC  ���C������
 *
 *  @param  GFL_PROC *p_proc  �v���Z�X
 *  @param  *p_seq            �V�[�P���X
 *  @param  *p_param          �e���[�N
 *  @param  *p_work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_SYS_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  {
    BR_SYS_SEQ_INIT,
    BR_SYS_SEQ_MAIN,
    BR_SYS_SEQ_EXIT,
  };

  BR_SYS_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  {
  case BR_SYS_SEQ_INIT:
    *p_seq  = BR_SYS_SEQ_MAIN;
    break;

  case BR_SYS_SEQ_MAIN:
    {
      if( SUBPROC_Main( &p_wk->subproc ) )
      {
        *p_seq  = BR_SYS_SEQ_EXIT;
      }
    }
    break;

  case BR_SYS_SEQ_EXIT:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *      SUBPROC�V�X�e��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  SUBPROC�V�X�e�� ������
 *
 *  @param  SUBPROC_WORK *p_wk  ���[�N
 *  @param  cp_procdata_tbl     �v���Z�X�ڑ��e�[�u��
 *  @param  void *p_wk_adrs     �A���b�N�֐��Ɖ���֐��ɓn�����[�N
 *  @param  heapID              �V�X�e���\�z�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_Init( SUBPROC_WORK *p_wk, const SUBPROC_DATA *cp_procdata_tbl, void *p_wk_adrs, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
  p_wk->p_procsys       = GFL_PROC_LOCAL_boot( heapID );
  p_wk->p_wk_adrs       = p_wk_adrs;
  p_wk->cp_procdata_tbl = cp_procdata_tbl;
  p_wk->heapID          = heapID;
}

//----------------------------------------------------------------------------
/**
 *  @brief  SUBPROC�V�X�e�� ���C������
 *
 *  @param  SUBPROC_WORK *p_wk  ���[�N
 *
 *  @retval TRUE�Ȃ�ΏI��  FALSE�Ȃ��PROC�����݂���
 */
//-----------------------------------------------------------------------------
static BOOL SUBPROC_Main( SUBPROC_WORK *p_wk )
{
  enum
  {
    SEQ_INIT,
    SEQ_ALLOC_PARAM,
    SEQ_MAIN,
    SEQ_FREE_PARAM,
    SEQ_END,
  };

  switch( p_wk->seq )
  {
  case SEQ_INIT:
    p_wk->pre_procID  = p_wk->now_procID;
    p_wk->now_procID  = p_wk->next_procID;
    p_wk->seq = SEQ_ALLOC_PARAM;
    break;

  case SEQ_ALLOC_PARAM:
    //�v���Z�X�����쐬�֐�������ΌĂяo��
    if( p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func )
    {
      p_wk->p_proc_param  = p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func(
          p_wk->heapID, p_wk->p_wk_adrs, p_wk->pre_procID );
    }
    else
    {
      p_wk->p_proc_param  = NULL;
    }

    //�v���b�N�Ăяo��
    GFL_PROC_LOCAL_CallProc( p_wk->p_procsys, p_wk->cp_procdata_tbl[  p_wk->now_procID ].ov_id,
          p_wk->cp_procdata_tbl[  p_wk->now_procID ].cp_procdata, p_wk->p_proc_param );

    p_wk->seq = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    {
      GFL_PROC_MAIN_STATUS result;
      result  = GFL_PROC_LOCAL_Main( p_wk->p_procsys );
      if( GFL_PROC_MAIN_NULL == result )
      {
        p_wk->seq = SEQ_FREE_PARAM;
      }
    }
    break;

  case SEQ_FREE_PARAM:
    //�v���Z�X�����j���֐��Ăяo��
    if( p_wk->cp_procdata_tbl[  p_wk->now_procID ].free_func )
    {
      p_wk->cp_procdata_tbl[  p_wk->now_procID ].free_func( p_wk->p_proc_param, p_wk->p_wk_adrs );
      p_wk->p_proc_param  = NULL;
    }

    //�������̃v���Z�X������ΌĂяo��
    //�Ȃ���ΏI��
    if( p_wk->now_procID  != p_wk->next_procID )
    {
      p_wk->seq = SEQ_INIT;
    }
    else
    {
      p_wk->seq = SEQ_END;
    }
    break;

  case SEQ_END:
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  SUBPROC�V�X�e�� �j��
 *
 *  @param  SUBPROC_WORK *p_wk  ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_Exit( SUBPROC_WORK *p_wk )
{
  GF_ASSERT( p_wk->p_proc_param == NULL );

  GFL_PROC_LOCAL_Exit( p_wk->p_procsys );
  GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  SUBPROC�V�X�e�� �v���Z�X���N�G�X�g
 *
 *  @param  SUBPROC_WORK *p_wk  ���[�N
 *  @param  proc_id             �Ăԃv���Z�XID
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_CallProc( SUBPROC_WORK *p_wk, u32 procID )
{
  p_wk->next_procID = procID;
}

//=============================================================================
/**
 *    �T�u�v���Z�X�p�쐬�j��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g�����R�[�_�[�R�A�̈���  �쐬
 *
 *  @param  HEAPID heapID     �q�[�vID
 *  @param  *p_wk_adrs        ���[�N
 *  @param  pre_procID
 *
 *  @return ����
 */
//-----------------------------------------------------------------------------
static void *BR_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs, u32 pre_procID )
{
  BR_CORE_PARAM *p_param;

  BR_SYS_WORK   *p_wk = p_wk_adrs;

  p_param = GFL_HEAP_AllocMemory( heapID, sizeof(BR_CORE_PARAM) );
  GFL_STD_MemClear( p_param, sizeof(BR_CORE_PARAM) );
  p_param->p_param  = p_wk->p_param;
  p_param->p_data   = &p_wk->data;

  if( pre_procID == SUBPROCID_BTLREC )
  {
    p_param->mode = BR_CORE_MODE_RETURN;
  }
  else
  {
    p_param->mode = BR_CORE_MODE_INIT;
  }

  return p_param;;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g�����R�[�_�[�R�A�̈���  �j��
 *
 *  @param  void *p_param_adrs        ����
 *  @param  *p_wk_adrs                ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{
  BR_CORE_PARAM *p_param  = p_param_adrs;
  BR_SYS_WORK   *p_wk     = p_wk_adrs;

  switch( p_param->ret )
  {
  case BR_CORE_RETURN_BTLREC:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_BTLREC );
    break;

  case BR_CORE_RETURN_FINISH:
    break;
  }

  GFL_HEAP_FreeMemory( p_param );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���̈���  �j��
 *
 *  @param  void *p_param_adrs        ����
 *  @param  *p_wk_adrs                ���[�N
 */
//-----------------------------------------------------------------------------
static void *BR_BATTLE_AllocParam( HEAPID heapID, void *p_wk_adrs, u32 pre_procID )
{
  BR_SYS_WORK   *p_wk     = p_wk_adrs;
  BATTLE_SETUP_PARAM  *p_param;
  GAMEDATA            *p_gamedata;

  p_param = GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_SETUP_PARAM) );
  GFL_STD_MemClear( p_param, sizeof(BATTLE_SETUP_PARAM) );
  p_gamedata  = p_wk->p_param->p_gamedata;

  BTL_SETUP_InitForRecordPlay( p_param, p_gamedata, heapID );

  BattleRec_LoadToolModule();
  BattleRec_RestoreSetupParam( p_param, heapID );
  BattleRec_UnloadToolModule();

  PMSND_PushBGM();
  PMSND_PlayBGM( p_param->musicDefault );

  return p_param;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���̈���  �j��
 *
 *  @param  void *p_param_adrs        ����
 *  @param  *p_wk_adrs                ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_BATTLE_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{
  BR_SYS_WORK         *p_wk     = p_wk_adrs;
  BATTLE_SETUP_PARAM  *p_param  = p_param_adrs;

  PMSND_PopBGM();

  p_wk->data.is_recplay_finish = p_param->recPlayCompleteFlag;

  GFL_HEAP_FreeMemory( p_param->playerStatus[ BTL_CLIENT_PLAYER ] );  //�v���C���[��MySatus�͊J������Ȃ��̂�
  BTL_SETUP_QuitForRecordPlay( p_param );
  GFL_HEAP_FreeMemory( p_param );

  SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
}

