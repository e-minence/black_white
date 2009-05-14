//==============================================================================
/**
 * @file    game_comm.c
 * @brief   �Q�[���ʐM�Ǘ�
 * @author  matsuda
 * @date    2009.05.12(��)
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/game_comm.h"

#include "field/game_beacon_search.h"
#include "field/field_comm/field_comm_func.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�V�[�P���X�ԍ�
typedef enum{
  GCSSEQ_INIT,
  GCSSEQ_INIT_WAIT,
  GCSSEQ_UPDATE,
  GCSSEQ_EXIT,
  GCSSEQ_EXIT_WAIT,
}GCSSEQ;


//==============================================================================
//  �\���̒�`
//==============================================================================
///�Q�[�����s�p���[�N�\����
typedef struct{
  u8 seq;
  u8 padding[3];
}GAME_COMM_SUB_WORK;

///�Q�[���ʐM�Ǘ����[�N�\����
typedef struct _GAME_COMM_SYS{
  GAME_COMM_NO game_comm_no;
  GAME_COMM_NO reserve_comm_game_no;   ///<�\��ʐM�Q�[���ԍ�
  GAME_COMM_SUB_WORK sub_work;
  void *app_work;                     ///<�e�A�v���P�[�V�������m�ۂ������[�N�̃|�C���^
}GAME_COMM_SYS;

///�Q�[���ʐM�֐��f�[�^�\����
typedef struct{
  void *(*init_func)(void);                   ///<����������
  BOOL (*init_wait_func)(void *app_work);     ///<�����������҂�
  void (*update_func)(void *app_work);        ///<�X�V����
  void (*exit_func)(void *app_work);          ///<�I������
  BOOL (*exit_wait_func)(void *app_work);     ///<�I�������҂�
}GAME_FUNC_TBL;


//==============================================================================
//  �f�[�^
//==============================================================================
///�ʐM�Q�[���Ǘ��e�[�u��
static const GAME_FUNC_TBL GameFuncTbl[] = {
  //GAME_COMM_NO_NULL    1origin��
  {
    NULL,       //init
    NULL,       //init_wait
    NULL,       //update
    NULL,       //exit
    NULL,       //exit_wait
  },
  
  //GAME_COMM_NO_FIELD_BEACON_SEARCH
  {
    GameBeacon_Init,       //init
    GameBeacon_InitWait,   //init_wait
    GameBeacon_Update,     //update
    GameBeacon_Exit,       //exit
    GameBeacon_ExitWait,   //exit_wait
  },
  //GAME_COMM_NO_INVASION
  {
    FIELD_COMM_FUNC_InitCommSystem,       //init
    FIELD_COMM_FUNC_InitCommSystemWait,   //init_wait
    FIELD_COMM_FUNC_UpdateSystem,         //update
    FIELD_COMM_FUNC_TermCommSystem,       //exit
    FIELD_COMM_FUNC_TermCommSystemWait,   //exit_wait
  },
};



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �Q�[���ʐM�Ǘ����[�N�m��
 *
 * @param   heap_id		
 *
 * @retval  GAME_COMM_SYS_PTR		�m�ۂ������[�N�ւ̃|�C���^
 */
//==================================================================
GAME_COMM_SYS_PTR GameCommSys_Alloc(HEAPID heap_id)
{
  GAME_COMM_SYS_PTR gcsp;
  
  gcsp = GFL_HEAP_AllocMemory(heap_id, sizeof(GAME_COMM_SYS));
  return gcsp;
}

//==================================================================
/**
 * �Q�[���ʐM�Ǘ����[�N���
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommSys_Free(GAME_COMM_SYS_PTR gcsp)
{
  GF_ASSERT(gcsp->game_comm_no == GAME_COMM_NO_NULL && gcsp->app_work == NULL);
  
  GFL_HEAP_FreeMemory(gcsp);
}

//==================================================================
/**
 * �Q�[���ʐM���C������
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommSys_Main(GAME_COMM_SYS_PTR gcsp)
{
  GAME_COMM_SUB_WORK *sub_work;
  const GAME_FUNC_TBL *func_tbl;
  
  if(gcsp == NULL || gcsp->game_comm_no == GAME_COMM_NO_NULL){
    return;
  }
  
  sub_work = &gcsp->sub_work;
  func_tbl = &GameFuncTbl[gcsp->game_comm_no];
  
  switch(sub_work->seq){
  case GCSSEQ_INIT:
    if(func_tbl->init_func != NULL){
      gcsp->app_work = func_tbl->init_func();
    }
    sub_work->seq++;
    break;
  case GCSSEQ_INIT_WAIT:
    if(func_tbl->init_wait_func == NULL || func_tbl->init_wait_func(gcsp->app_work) == TRUE){
      sub_work->seq++;
    }
    break;
  case GCSSEQ_UPDATE:
    func_tbl->update_func(gcsp->app_work);
    break;
  case GCSSEQ_EXIT:
    if(func_tbl->exit_func != NULL){
      func_tbl->exit_func(gcsp->app_work);
    }
    sub_work->seq++;
    break;
  case GCSSEQ_EXIT_WAIT:
    if(func_tbl->exit_wait_func == NULL || func_tbl->exit_wait_func(gcsp->app_work) == TRUE){
      if(gcsp->app_work != NULL){
        GFL_HEAP_FreeMemory(gcsp->app_work);
        gcsp->app_work = NULL;
      }
      gcsp->game_comm_no = GAME_COMM_NO_NULL;
      if(gcsp->reserve_comm_game_no != GAME_COMM_NO_NULL){
        GameCommSys_Boot(gcsp, gcsp->reserve_comm_game_no);
      }
    }
    break;
  }
}

//==================================================================
/**
 * �Q�[���ʐM�N��
 *
 * @param   gcsp		        �Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^
 * @param   game_comm_no		�N������ʐM�Q�[���ԍ�(GAME_COMM_NO_???)
 */
//==================================================================
void GameCommSys_Boot(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no)
{
  GF_ASSERT(gcsp->game_comm_no == GAME_COMM_NO_NULL);
  
  gcsp->game_comm_no = game_comm_no;
  GFL_STD_MemClear(&gcsp->sub_work, sizeof(GAME_COMM_SUB_WORK));
}

//==================================================================
/**
 * �I�����N�G�X�g
 *
 * @param   gcsp		
 *
 * �I���̊m�F��GameCommSys_BootCheck�֐����g�p���Ă�������
 */
//==================================================================
void GameCommSys_ExitReq(GAME_COMM_SYS_PTR gcsp)
{
  GF_ASSERT(gcsp->sub_work.seq == GCSSEQ_UPDATE);
  
  OS_TPrintf("GameCommSys_ExitReq\n");
  gcsp->sub_work.seq = GCSSEQ_EXIT;
}

//==================================================================
/**
 * �ʐM�Q�[���؂�ւ����N�G�X�g
 *
 * @param   gcsp		
 * @param   game_comm_no		�؂�ւ���A�N������ʐM�Q�[���ԍ�(GAME_COMM_NO_???)
 */
//==================================================================
void GameCommSys_ChangeReq(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no)
{
  GF_ASSERT(gcsp->sub_work.seq == GCSSEQ_UPDATE);
  
  gcsp->sub_work.seq = GCSSEQ_EXIT;
  gcsp->reserve_comm_game_no = game_comm_no;
}

//==================================================================
/**
 * �N���m�F
 *
 * @param   gcsp		
 *
 * @retval  GAME_COMM_NO  �N�����Ă���ʐM�Q�[���ԍ�(�����N�����Ă��Ȃ����GAME_COMM_NO_NULL)
 */
//==================================================================
GAME_COMM_NO GameCommSys_BootCheck(GAME_COMM_SYS_PTR gcsp)
{
  if(gcsp->game_comm_no != GAME_COMM_NO_NULL){
    return gcsp->game_comm_no;
  }
  if(gcsp->reserve_comm_game_no != GAME_COMM_NO_NULL){
    return gcsp->reserve_comm_game_no;
  }
  return GAME_COMM_NO_NULL;
}

//==================================================================
/**
 * �ʐM�Q�[���ԍ����擾����
 *
 * @param   gcsp		
 *
 * @retval  GAME_COMM_NO		
 */
//==================================================================
GAME_COMM_NO GameCommSys_GetCommGameNo(GAME_COMM_SYS_PTR gcsp)
{
  return gcsp->game_comm_no;
}

//==================================================================
/**
 * �ʐM�Q�[�����V�X�e���̑҂���Ԃ����ׂ�
 *
 * @param   gcsp		
 *
 * @retval  		TRUE:�������҂��A�I���҂��A���̃V�X�e���n�̑҂���ԂɂȂ��Ă���
 * @retval      FALSE:�ʏ�̍X�V���������s���Ă�����
 */
//==================================================================
BOOL GameCommSys_CheckSystemWaiting(GAME_COMM_SYS_PTR gcsp)
{
  if(gcsp->game_comm_no == GAME_COMM_NO_NULL || gcsp->sub_work.seq != GCSSEQ_UPDATE){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �A�v���P�[�V�������[�N�̃|�C���^���擾����
 *
 * @param   gcsp		  
 *
 * @retval  void *		�A�v���P�[�V�������[�N�ւ̃|�C���^
 */
//==================================================================
void *GameCommSys_GetAppWork(GAME_COMM_SYS_PTR gcsp)
{
  return gcsp->app_work;
}
