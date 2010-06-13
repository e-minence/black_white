//======================================================================
/**
 * @file  dressup_system.h
 * @brief �~���[�W�J���̃h���X�A�b�vProc
 * @author  ariizumi
 * @data  09/02/10
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "musical/musical_system.h"
#include "musical/musical_dressup_sys.h"
#include "musical/musical_shot_sys.h"
#include "dup_local_def.h"
#include "dup_fitting.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//  define
//======================================================================
FS_EXTERN_OVERLAY(dressup);

//======================================================================
//  enum
//======================================================================
enum  //���C���V�[�P���X
{
  DUP_SEQ_INIT_FITTING, //�h���X�A�b�v��������
  DUP_SEQ_LOOP_FITTING,
  DUP_SEQ_TERM_FITTING,
};

//======================================================================
//  typedef struct
//======================================================================

typedef struct
{
  FITTING_INIT_WORK *fitInitWork;
  FITTING_WORK *fitWork;
  FITTING_RETURN  fitRet;
  DRESSUP_INIT_WORK *initWork;
}DRESSUP_LOCAL_WORK;

//======================================================================
//  proto
//======================================================================

static GFL_PROC_RESULT DressUpProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT DressUpProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT DressUpProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA DressUp_ProcData =
{
  DressUpProc_Init,
  DressUpProc_Main,
  DressUpProc_Term
};

//--------------------------------------------------------------
//  
//--------------------------------------------------------------

static GFL_PROC_RESULT DressUpProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  DRESSUP_LOCAL_WORK *work;
  DRESSUP_INIT_WORK *initWork = pwk;
  *seq = DUP_SEQ_INIT_FITTING;
  GFL_OVERLAY_Load(FS_OVERLAY_ID(dressup));

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MUSICAL_DRESSUP, 0x40000 );

  work = GFL_PROC_AllocWork( proc, sizeof(DRESSUP_LOCAL_WORK), HEAPID_MUSICAL_DRESSUP );
  work->fitInitWork = GFL_HEAP_AllocMemory( HEAPID_MUSICAL_DRESSUP , sizeof(FITTING_INIT_WORK) );

#if PM_DEBUG
  if( pwk == NULL )
  {
    GFL_OVERLAY_Load(FS_OVERLAY_ID(musical_shot));
    {
      POKEMON_PARAM *pokePara = PP_Create( MONSNO_NIDOKINGU , 20 , PTL_SETUP_POW_AUTO , HEAPID_MUSICAL_DRESSUP );
      MUSICAL_POKE_PARAM *musPoke = MUSICAL_SYSTEM_InitMusPoke( pokePara , HEAPID_MUSICAL_DRESSUP );
      initWork = MUSICAL_DRESSUP_CreateInitWork( HEAPID_MUSICAL_DRESSUP , musPoke , SaveControl_GetPointer(/*�f�o�b�O�p*/) );
    }
  }
#endif
  
  work->initWork = initWork;

  work->fitInitWork->commWork = initWork->commWork;
  work->fitInitWork->musPoke = initWork->musPoke;
  work->fitInitWork->mus_save = initWork->mus_save;
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT DressUpProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  DRESSUP_LOCAL_WORK *work = mywk;
#if PM_DEBUG
  if( pwk == NULL )
  {
    GFL_HEAP_FreeMemory( work->initWork->musPoke->pokePara );
    GFL_HEAP_FreeMemory( work->initWork->musPoke );
    GFL_HEAP_FreeMemory( work->initWork );
    GFL_OVERLAY_Unload(FS_OVERLAY_ID(musical_shot));
  }
#endif
  GFL_HEAP_FreeMemory( work->fitInitWork );
  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( HEAPID_MUSICAL_DRESSUP );

  GFL_OVERLAY_Unload(FS_OVERLAY_ID(dressup));

  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT DressUpProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  //��ʑJ�ڂ��Ȃ��̂�Fitting���Ŋ������邽�ߖ��ʂȏ����H
  DRESSUP_LOCAL_WORK *work = mywk;
  switch( *seq )
  {
  case DUP_SEQ_INIT_FITTING:
    work->fitWork = DUP_FIT_InitFitting(work->fitInitWork,HEAPID_MUSICAL_DRESSUP);
    *seq = DUP_SEQ_LOOP_FITTING;
    MUS_TPrintf("HEAP[%x]\n",GFL_HEAP_GetHeapFreeSize( GFL_HEAPID_APP ));
    if( work->initWork->commWork != NULL )
    {
      if( MUS_COMM_GetMode( work->initWork->commWork ) == MCM_PARENT )
      {
        //�ȃf�[�^�z�M
        MUS_COMM_Start_SendStrmData( work->initWork->commWork );
      }
    }
    break;
    
  case DUP_SEQ_LOOP_FITTING:
    {
      FITTING_RETURN ret = DUP_FIT_LoopFitting(work->fitWork);
      if( ret != FIT_RET_CONTINUE )
      {
        work->fitRet = ret;
        *seq = DUP_SEQ_TERM_FITTING;
      }
    }
    break;

  case DUP_SEQ_TERM_FITTING:
    DUP_FIT_TermFitting(work->fitWork);
    return GFL_PROC_RES_FINISH;
    break;
  }
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
//  InitWork������
//--------------------------------------------------------------
DRESSUP_INIT_WORK* MUSICAL_DRESSUP_CreateInitWork( HEAPID heapId , MUSICAL_POKE_PARAM *musPoke , SAVE_CONTROL_WORK *saveCtrl )
{
  DRESSUP_INIT_WORK* initWork;
  initWork = GFL_HEAP_AllocMemory( heapId , sizeof(DRESSUP_INIT_WORK));
  initWork->commWork = NULL;
  initWork->mus_save = MUSICAL_SAVE_GetMusicalSave(saveCtrl);
  initWork->musPoke = musPoke;

  return initWork;
}

//--------------------------------------------------------------
//  InitWork�J��
//--------------------------------------------------------------
void MUSICAL_DRESSUP_DeleteInitWork( DRESSUP_INIT_WORK* initWork )
{
  GFL_HEAP_FreeMemory( initWork );
}
