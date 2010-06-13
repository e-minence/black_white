//======================================================================
/**
 * @file  sta_act_script.c
 * @brief �X�e�[�W �X�N���v�g����
 * @author  ariizumi
 * @data  09/03/06
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "sta_act_script.h"
#include "sta_act_script_def.h"
#include "script_table.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define
#define SCRIPT_TCB_TASK_NUM (16)

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct


//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
static BOOL STA_SCRIPT_UpdateScript( STA_SCRIPT_SYS *work , STA_SCRIPT_WORK *scriptWork );

//============================================================================================
/**
 *  VM�C�j�V�����C�U�e�[�u��
 */
//============================================================================================
static  const VM_INITIALIZER  vm_init={
  16,//BTLV_EFFVM_STACK_SIZE,       //u16 stack_size; ///<�g�p����X�^�b�N�̃T�C�Y
  8,//BTLV_EFFVM_REG_SIZE,        //u16 reg_size;   ///<�g�p���郌�W�X�^�̐�
  STA_ScriptFuncArr,      //const VMCMD_FUNC * command_table; ///<�g�p���鉼�z�}�V�����߂̊֐��e�[�u��
  SCRIPT_ENUM_MAX,      //const u32 command_max;      ///<�g�p���鉼�z�}�V�����ߒ�`�̍ő吔
};


//--------------------------------------------------------------
//  
//--------------------------------------------------------------

STA_SCRIPT_SYS* STA_SCRIPT_InitSystem( HEAPID heapId ,ACTING_WORK *actWork)
{
  u8 i;
  STA_SCRIPT_SYS *work;
  work = GFL_HEAP_AllocMemory( heapId , sizeof( STA_SCRIPT_SYS ) );
  
  work->heapId = heapId;
  work->actWork = actWork;
  work->tcbWork = GFL_HEAP_AllocMemory( work->heapId , GFL_TCB_CalcSystemWorkSize(SCRIPT_TCB_TASK_NUM) );
  work->tcbSys = GFL_TCB_Init( SCRIPT_TCB_TASK_NUM , work->tcbWork );
  work->befVCount = OS_GetVBlankCount();
  
  NNS_FND_INIT_LIST( &work->tcbList , STA_SCRIPT_TCB_OBJECT , linkObj );
  
  for( i=0;i<SCRIPT_NUM;i++ )
  {
    work->scriptWork[i] = NULL;
  }
  
  return work;
}

void STA_SCRIPT_ExitSystem( STA_SCRIPT_SYS *work )
{
  u8 i;
  for( i=0;i<SCRIPT_NUM;i++ )
  {
    if( work->scriptWork[i] != NULL )
    {
      VM_End( work->scriptWork[i]->vmHandle );
      VM_Delete( work->scriptWork[i]->vmHandle );
      if( work->scriptWork[i]->isFlag & SFB_IS_AUTO_DELETE )
      {
        GFL_HEAP_FreeMemory( work->scriptWork[i]->scriptBaseData );
      }
      GFL_HEAP_FreeMemory( work->scriptWork[i] );
      work->scriptWork[i] = NULL;
    }
  }
  
  //TCB�Ɏc���Ă��鏈��������
  {
    STA_SCRIPT_TCB_OBJECT *tcbObj = NNS_FndGetNextListObject( &work->tcbList , NULL );
    while( tcbObj != NULL )
    {
      STA_SCRIPT_TCB_OBJECT *nextTcbObj = NNS_FndGetNextListObject( &work->tcbList , tcbObj );
      STA_SCRIPT_DeleteTcbTask( work , tcbObj );
      tcbObj = nextTcbObj;
      
      MUS_TPrintf("TCB Delete!!\n");
    }
    
  }

  GFL_TCB_Exit( work->tcbSys );
  GFL_HEAP_FreeMemory( work->tcbWork );
  GFL_HEAP_FreeMemory( work );
}

void STA_SCRIPT_UpdateSystem( STA_SCRIPT_SYS *work )
{
  u8 i;
  BOOL isFinishSync = TRUE;
  const u32 nowVCount = OS_GetVBlankCount();
  u32 subVCount = nowVCount - work->befVCount;
  
  if( subVCount > 1 )
  {
    //MUS_TPrintf("Script Delay[%d]!!\n",subVCount-1);
#if PM_DEBUG
    //�R�}����`�F�b�N�p
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
    {
      subVCount = 1;
    }
#endif
  }
#if PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y )
  {
    subVCount = 3;
  }
#endif

  for( /*subVCount*/ ; subVCount> 0 ; subVCount-- )
  {
    for( i=0;i<SCRIPT_NUM;i++ )
    {
      if( work->scriptWork[i] != NULL )
      {
        const BOOL ret = STA_SCRIPT_UpdateScript( work , work->scriptWork[i] );
        if( ret == TRUE )
        {
          GFL_HEAP_FreeMemory( work->scriptWork[i] );
          work->scriptWork[i] = NULL;
        }
        else
        if( work->scriptWork[i]->isFlag & SFB_IS_TARGET_SYNC )
        {
          //�����҂��ΏۃX�N���v�g����ł������҂��ɂȂ��Ă��Ȃ�������A�E�g�I
          if( (work->scriptWork[i]->isFlag & SFB_WAIT_SYNC) == 0 )
          {
            isFinishSync = FALSE;
          }
        }
      }
    }
    //�����̉���
    if( isFinishSync == TRUE )
    {
      for( i=0;i<SCRIPT_NUM;i++ )
      {
        if( work->scriptWork[i] != NULL )
        {
          if( (work->scriptWork[i]->isFlag & SFB_IS_TARGET_SYNC) &&
            (work->scriptWork[i]->isFlag & SFB_WAIT_SYNC) )
          {
            work->scriptWork[i]->isFlag ^= SFB_WAIT_SYNC;
          }
        }
      }
    }
    GFL_TCB_Main( work->tcbSys );
  }
  work->befVCount = nowVCount;
}

void* STA_SCRIPT_SetScript( STA_SCRIPT_SYS *work , void *scriptData , const BOOL isTrgSync )
{
  u8 i;
  for( i=0;i<SCRIPT_NUM;i++ )
  {
    if( work->scriptWork[i] == NULL )
    {
      break;
    }
  }

  GF_ASSERT_MSG( i<SCRIPT_NUM , "Stage script is full!!\n" );
  
  work->scriptWork[i] = GFL_HEAP_AllocMemory( work->heapId , sizeof( STA_SCRIPT_WORK ) );
  work->scriptWork[i]->scriptData = scriptData;
  work->scriptWork[i]->scriptBaseData = NULL;
  work->scriptWork[i]->waitCnt = 0;
  work->scriptWork[i]->frame = 0;
  work->scriptWork[i]->isFlag = 0;
  work->scriptWork[i]->trgPokeFlg = 0;
  work->scriptWork[i]->lineNo = i;
  if( isTrgSync == TRUE )
  {
    work->scriptWork[i]->isFlag |= SFB_IS_TARGET_SYNC;
  }

  work->scriptWork[i]->sysWork = work;
  work->scriptWork[i]->vmHandle = VM_Create( work->heapId , &vm_init );
  VM_Init( work->scriptWork[i]->vmHandle , (void*)work->scriptWork[i] );
  VM_Start( work->scriptWork[i]->vmHandle , work->scriptWork[i]->scriptData );
  
  return work->scriptWork[i];
}

//�A�s�[���X�N���v�g�E�T�u�X�N���v�g�ǂݍ���
void STA_SCRIPT_SetSubScript( STA_SCRIPT_SYS *work , void *scriptBaseData , const u8 scriptNo , const u8 targetPokeBit )
{
  STA_SCRIPT_WORK *scriptWork;
  const u32 *headData = (u32*)scriptBaseData;
  
  
  scriptWork = STA_SCRIPT_SetScript( work , (u8*)scriptBaseData+headData[scriptNo] , FALSE );
  scriptWork->scriptBaseData = scriptBaseData;
  scriptWork->isFlag |= SFB_IS_AUTO_DELETE;
  scriptWork->trgPokeFlg = targetPokeBit;

}

static BOOL STA_SCRIPT_UpdateScript( STA_SCRIPT_SYS *work , STA_SCRIPT_WORK *scriptWork )
{
  
  if( scriptWork->waitCnt > 0 )
  {
    scriptWork->waitCnt--;
  }

  if( scriptWork->waitCnt == 0 &&
    ( scriptWork->isFlag & SFB_WAIT_SYNC ) == 0)
  {
    VM_Control( scriptWork->vmHandle );
    if( scriptWork->isFlag & SFB_IS_FINISH )
    {
      VM_End( scriptWork->vmHandle );
      VM_Delete( scriptWork->vmHandle );
      if( scriptWork->isFlag & SFB_IS_AUTO_DELETE )
      {
        GFL_HEAP_FreeMemory( scriptWork->scriptBaseData );
      }
      return TRUE;
    }
  }
  scriptWork->frame++;
  return FALSE;
}

const u8 STA_SCRIPT_GetRunningScriptNum( STA_SCRIPT_SYS *work )
{
  u8 i;
  u8 num = 0;
  for( i=0;i<SCRIPT_NUM;i++ )
  {
    if( work->scriptWork[i] != NULL )
    {
      num++;
    }
  }
  return num; 
}

//--------------------------------------------------------------
//  TCB�ɒǉ�
//--------------------------------------------------------------
STA_SCRIPT_TCB_OBJECT* STA_SCRIPT_CreateTcbTask( STA_SCRIPT_SYS *work , GFL_TCB_FUNC *func , void* tcbWork , u32 pri )
{
  STA_SCRIPT_TCB_OBJECT *tcbObj = GFL_HEAP_AllocMemory( work->heapId , sizeof(STA_SCRIPT_TCB_OBJECT) );
  tcbObj->tcbTask = GFL_TCB_AddTask( work->tcbSys , func , tcbWork , pri );
  if( tcbObj->tcbTask != NULL )
  {
    tcbObj->tcbWork = tcbWork;
    
    NNS_FndAppendListObject( &work->tcbList , tcbObj );
    return tcbObj;
  }
  else
  {
    MUS_TPrintf("*******************************************************\n");
    MUS_TPrintf("*******************************************************\n");
    MUS_TPrintf("TCB�o�^���I�[�o�[! �����ɓ��������̂����炵�Ă��������B\n");
    MUS_TPrintf("*******************************************************\n");
    MUS_TPrintf("*******************************************************\n");
    GFL_HEAP_FreeMemory( tcbObj );
    return NULL;
  }
}

//--------------------------------------------------------------
//  TCB����폜
//--------------------------------------------------------------
void STA_SCRIPT_DeleteTcbTask( STA_SCRIPT_SYS *work , STA_SCRIPT_TCB_OBJECT *tcbObj )
{
  GFL_HEAP_FreeMemory( tcbObj->tcbWork );
  GFL_TCB_DeleteTask( tcbObj->tcbTask );

  NNS_FndRemoveListObject( &work->tcbList , tcbObj );

  GFL_HEAP_FreeMemory( tcbObj );
}
