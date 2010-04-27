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
#include "buflen.h"

#include "field/game_beacon_search.h"
#include "message.naix"
#include "msg/msg_invasion.h"
#include "net_app/union/union_main.h"
#include "net/wih_dwc.h"
#include "field/fieldmap_proc.h"
#include "field/intrude_comm.h"
#include "system/net_err.h"
#include "system/main.h"
#include "field/zonedata.h"
#include "musical/musical_comm_field.h"
#include "field/intrude_common.h"


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
  GCSSEQ_FINISH,
}GCSSEQ;

///�ʐM�ő�l��
#define COMM_PLAYER_MAX   (FIELD_COMM_MEMBER_MAX)

///�C���t�H���[�V�������b�Z�[�W�L���[��
#define COMM_INFO_QUE_MAX     (6)


//==============================================================================
//  �\���̒�`
//==============================================================================
//--------------------------------------------------------------
//  �X�e�[�^�X
//--------------------------------------------------------------
///�e�v���C���[�̒ʐM�X�e�[�^�X
typedef struct{
  u16 zone_id;        ///<���݂���]�[��ID
  u16 old_zone_id;    ///<�O�܂ł����]�[��ID
  u8 pm_version;      ///<�v���C���[���g��PM_VERSION
  u8 invasion_netid;  ///<�N����ROM
  u8 old_invasion_netid;  ///<�O�܂ł����N����ROM
  u8 padding;
}GAME_COMM_PLAYER_STATUS;

//--------------------------------------------------------------
//  �C���t�H���[�V����
//--------------------------------------------------------------
///�C���t�H���[�V�����̃��b�Z�[�W�L���[�\����
typedef struct{
  u16 message_id;
  u8 net_id;
  u8 use:1;           ///<TRUE:�g�p�� FALSE:���g�p
  u8 sex:1;
  u8 target_sex:1;
  u8      :5;
  STRBUF *name_strbuf;         ///<���b�Z�[�W�̎�v���C���[��
  STRBUF *target_name_strbuf;  ///<���b�Z�[�W�̎�v���C���[�̃^�[�Q�b�g�ɂ��ꂽ�v���C���[��
}GAME_COMM_INFO_QUE;

///�C���t�H���[�V�����\����
typedef struct{
  GAME_COMM_INFO_QUE msg_que[COMM_INFO_QUE_MAX];
  u8 now_pos;       ///<���܂��Ă���L���[�̊J�n�ʒu
  u8 space_pos;     ///<�󂫃L���[�̊J�n�ʒu
  u8 padding[2];
}GAME_COMM_INFO;

//--------------------------------------------------------------
//  �V�X�e��
//--------------------------------------------------------------
///�Q�[�����s�p���[�N�\����
typedef struct{
  int func_seq;
  u8 seq;
  u8 padding[3];
}GAME_COMM_SUB_WORK;

///�Q�[���ʐM�Ǘ����[�N�\����
typedef struct _GAME_COMM_SYS{
  GAMEDATA *gamedata;                 ///<�Q�[���f�[�^�ւ̃|�C���^
  GAME_COMM_STATUS comm_status;         ///<�ʐM�X�e�[�^�X
  GAME_COMM_NO game_comm_no;
  GAME_COMM_NO last_comm_no;            ///<�Ō�Ɏ��s���Ă���GAME_COMM_NO
  GAME_COMM_LAST_STATUS last_status;    ///<�Ō�Ɏ��s���Ă���GAME_COMM_NO�̏I�����
  GAME_COMM_SUB_WORK sub_work;
  void *parent_work;                  ///<�Ăяo�����Ɉ����n���|�C���^
  void *app_work;                     ///<�e�A�v���P�[�V�������m�ۂ������[�N�̃|�C���^
  void *exitcallback_parentwork;      ///<�I�����ɌĂяo���R�[���o�b�N�֐��֓n��ParentWork
  GAMECOMM_EXITCALLBACK_FUNC exitcallback_func; ///<�I�����ɌĂяo���R�[���o�b�N�֐��ւ̃|�C���^
  GAME_COMM_PLAYER_STATUS player_status[COMM_PLAYER_MAX]; ///<�ʐM�v���C���[�X�e�[�^�X
  GAME_COMM_INFO info;                ///<�C���t�H���[�V�������[�N
}GAME_COMM_SYS;

///�Q�[���ʐM�֐��f�[�^�\����
typedef struct{
  void *(*init_func)(int *seq, void *pwk);                         ///<����������
  BOOL (*init_wait_func)(int *seq, void *pwk, void *app_work);     ///<�����������҂�
  void (*update_func)(int *seq, void *pwk, void *app_work);        ///<�X�V����
  BOOL (*exit_func)(int *seq, void *pwk, void *app_work);          ///<�I������
  BOOL (*exit_wait_func)(int *seq, void *pwk, void *app_work);     ///<�I�������҂�
  
  void (*field_create)(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork);  ///<Field�쐬���ɌĂ΂��R�[���o�b�N
  void (*field_delete)(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork);  ///<Field�폜���ɌĂ΂��R�[���o�b�N
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
    NULL,       //field_create
    NULL,       //field_delete
  },
  
  //GAME_COMM_NO_FIELD_BEACON_SEARCH
  {
    GameBeacon_Init,       //init
    GameBeacon_InitWait,   //init_wait
    GameBeacon_Update,     //update
    GameBeacon_Exit,       //exit
    GameBeacon_ExitWait,   //exit_wait
    NULL,       //field_create
    NULL,       //field_delete
  },
  //GAME_COMM_NO_INVASION
  {
    IntrudeComm_InitCommSystem,       //init
    IntrudeComm_InitCommSystemWait,   //init_wait
    IntrudeComm_UpdateSystem,         //update
    IntrudeComm_TermCommSystem,       //exit
    IntrudeComm_TermCommSystemWait,   //exit_wait
    IntrudeComm_FieldCreate,          //field_create
    IntrudeComm_FieldDelete,          //field_delete
  },
  //GAME_COMM_NO_UNION
  {
    UnionComm_Init,       //init
    UnionComm_InitWait,   //init_wait
    UnionComm_Update,     //update
    UnionComm_Exit,       //exit
    UnionComm_ExitWait,   //exit_wait
    UnionMain_Callback_FieldCreate,       //field_create
    UnionMain_Callback_FieldDelete,       //field_delete
  },
  {
    MUS_COMM_InitGameComm,       //init
    NULL,       //init_wait
    MUS_COMM_UpdateGameComm,     //update
    MUS_COMM_ExitGameComm,       //exit
    MUS_COMM_ExitWaitGameComm,   //exit_wait
    NULL,       //field_create
    NULL,       //field_delete
  },
  //GAME_COMM_NO_DEBUG_SCANONLY
  {
    GameBeacon_Init,       //init
    GameBeacon_InitWait,   //init_wait
    GameBeacon_Update,     //update
    GameBeacon_Exit,       //exit
    GameBeacon_ExitWait,   //exit_wait
    NULL,       //field_create
    NULL,       //field_delete
  },
};

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void GameCommSub_SeqSet(GAME_COMM_SUB_WORK *sub_work, u8 seq);
static void GameCommInfo_SetQue(GAME_COMM_SYS_PTR gcsp, int comm_net_id, int target_net_id, u32 message_id);


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
 * @param   gamedata     �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  GAME_COMM_SYS_PTR		�m�ۂ������[�N�ւ̃|�C���^
 */
//==================================================================
GAME_COMM_SYS_PTR GameCommSys_Alloc(HEAPID heap_id, GAMEDATA *gamedata)
{
  GAME_COMM_SYS_PTR gcsp;
  GAME_COMM_INFO *comm_info;
  const MYSTATUS *myst;
  int i;
  
  gcsp = GFL_HEAP_AllocClearMemory(heap_id, sizeof(GAME_COMM_SYS));
  gcsp->gamedata = gamedata;
  
  myst = GAMEDATA_GetMyStatus(gamedata);
  comm_info = &gcsp->info;
  for(i = 0; i < COMM_INFO_QUE_MAX; i++){
    comm_info->msg_que[i].name_strbuf = GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, heap_id);
    comm_info->msg_que[i].target_name_strbuf = GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, heap_id);
    //������΂Ȃ��悤�Ɏ����̃f�[�^�Ŗ��߂Ă���
    MyStatus_CopyNameString(myst, comm_info->msg_que[i].name_strbuf);
    MyStatus_CopyNameString(myst, comm_info->msg_que[i].target_name_strbuf);
  }
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
  GAME_COMM_INFO *comm_info = &gcsp->info;
  int i;
  
  GF_ASSERT(gcsp->game_comm_no == GAME_COMM_NO_NULL && gcsp->app_work == NULL);
  
  for(i = 0; i < COMM_INFO_QUE_MAX; i++){
    GFL_STR_DeleteBuffer(comm_info->msg_que[i].name_strbuf);
    GFL_STR_DeleteBuffer(comm_info->msg_que[i].target_name_strbuf);
  }
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
      gcsp->app_work = func_tbl->init_func(&sub_work->func_seq, gcsp->parent_work);
    }
    GameCommSub_SeqSet(sub_work, GCSSEQ_INIT_WAIT);
    break;
  case GCSSEQ_INIT_WAIT:
    if(func_tbl->init_wait_func == NULL || func_tbl->init_wait_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work) == TRUE){
      //��check�@���󃏃C�����X�����Ȃ��̂ŁA�Ƃ肠����������WIRELESS����
      gcsp->comm_status = GAME_COMM_STATUS_WIRELESS;
      GameCommSub_SeqSet(sub_work, GCSSEQ_UPDATE);
    }
    break;
  case GCSSEQ_UPDATE:
    func_tbl->update_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work);
    break;
  case GCSSEQ_EXIT:
    if(func_tbl->exit_func != NULL){
      if(func_tbl->exit_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work) == TRUE){
        GameCommSub_SeqSet(sub_work, GCSSEQ_EXIT_WAIT);
      }
    }
    else{
      GameCommSub_SeqSet(sub_work, GCSSEQ_EXIT_WAIT);
    }
    break;
  case GCSSEQ_EXIT_WAIT:
    if(func_tbl->exit_wait_func == NULL || func_tbl->exit_wait_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work) == TRUE){
      GameCommSub_SeqSet(sub_work, GCSSEQ_FINISH);
    }
    else{
      break;
    }
    //break;
  case GCSSEQ_FINISH:
    gcsp->app_work = NULL;
    gcsp->game_comm_no = GAME_COMM_NO_NULL;
    gcsp->comm_status = GAME_COMM_STATUS_NULL;
    {//�I���R�[���o�b�N
      GAMECOMM_EXITCALLBACK_FUNC exit_callback = gcsp->exitcallback_func;
      void *exit_parentwork = gcsp->exitcallback_parentwork;
      gcsp->exitcallback_func = NULL;
      gcsp->exitcallback_parentwork = NULL;
      if(exit_callback != NULL){
        exit_callback(exit_parentwork);
      }
    }
    break;
  }
}

//==================================================================
/**
 * �t�B�[���h�}�b�v�쐬���Ɏ��s����R�[���o�b�N
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommSys_Callback_FieldCreate(GAME_COMM_SYS_PTR gcsp, void *fieldWork)
{
  if(gcsp->sub_work.seq == GCSSEQ_FINISH){
    return;
  }
  
  if(GameFuncTbl[gcsp->game_comm_no].field_create != NULL){
    GameFuncTbl[gcsp->game_comm_no].field_create(gcsp->parent_work, gcsp->app_work, fieldWork);
  }
}

//==================================================================
/**
 * �t�B�[���h�}�b�v�폜���ɌĂ΂��R�[���o�b�N
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommSys_Callback_FieldDelete(GAME_COMM_SYS_PTR gcsp, void *fieldWork)
{
  if(gcsp->sub_work.seq == GCSSEQ_FINISH){
    return;
  }

  if(GameFuncTbl[gcsp->game_comm_no].field_delete != NULL){
    GameFuncTbl[gcsp->game_comm_no].field_delete(gcsp->parent_work, gcsp->app_work, fieldWork);
  }
}

//==================================================================
/**
 * �Q�[���ʐM�N��
 *
 * @param   gcsp		        �Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^
 * @param   game_comm_no		�N������ʐM�Q�[���ԍ�(GAME_COMM_NO_???)
 * @param   parent_work     �����n���|�C���^
 */
//==================================================================
void GameCommSys_Boot(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no, void *parent_work)
{
  GF_ASSERT(gcsp->game_comm_no == GAME_COMM_NO_NULL);
  
  gcsp->game_comm_no = game_comm_no;
  gcsp->last_comm_no = game_comm_no;
  gcsp->last_status = GAME_COMM_LAST_STATUS_NULL;
  gcsp->parent_work = parent_work;
  gcsp->exitcallback_func = NULL;
  gcsp->exitcallback_parentwork = NULL;
  GFL_STD_MemClear(&gcsp->sub_work, sizeof(GAME_COMM_SUB_WORK));
  GameCommStatus_InitPlayerStatus(gcsp);
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
  GameCommSys_ExitReqCallback(gcsp, NULL, NULL);
}

//==================================================================
/**
 * �I�����N�G�X�g(���S�I�����ɌĂ΂��R�[���o�b�N�t��)
 *
 * @param   gcsp		
 * @param   callback_func		�ʐM���I�����ɌĂ΂��R�[���o�b�N�֐��ւ̃|�C���^
 * @param   parent_work		  �R�[���o�b�N�֐��Ăяo�����Ɉ����Ƃ��ēn�����|�C���^
 */
//==================================================================
void GameCommSys_ExitReqCallback(GAME_COMM_SYS_PTR gcsp, GAMECOMM_EXITCALLBACK_FUNC callback_func, void *parent_work)
{
  GF_ASSERT(gcsp->sub_work.seq == GCSSEQ_UPDATE);
  
  OS_TPrintf("GameCommSys_ExitReq\n");
  GameCommSub_SeqSet(&gcsp->sub_work, GCSSEQ_EXIT);
  gcsp->exitcallback_func = callback_func;
  gcsp->exitcallback_parentwork = parent_work;
}

//==================================================================
/**
 * �ʐM�X�e�[�^�X���擾
 *
 * @param   gcsp		
 *
 * @retval  GAME_COMM_STATUS	�ʐM�X�e�[�^�X
 */
//==================================================================
GAME_COMM_STATUS GameCommSys_GetCommStatus(GAME_COMM_SYS_PTR gcsp)
{
  const GFLNetInitializeStruct *aNetStruct;
  
  if(GFL_NET_IsInit() == FALSE){
    return GAME_COMM_STATUS_NULL;
  }
  
  aNetStruct = GFL_NET_GetNETInitStruct();
	switch(aNetStruct->bNetType){
  case GFL_NET_TYPE_WIRELESS:		///<���C�����X�ʐM
  case GFL_NET_TYPE_IRC_WIRELESS:	///<�ԊO���ʐM�Ń}�b�`���O��A���C�����X�ʐM�ֈڍs
    gcsp->comm_status = GAME_COMM_STATUS_WIRELESS;
    break;

  case GFL_NET_TYPE_WIRELESS_SCANONLY:	///<���C�����X�ʐM(�X�L������p�E�d�������v��_��)
    {
      gcsp->comm_status = WIH_DWC_GetAllBeaconType(NULL);  //@todo
    }
    break;
  case GFL_NET_TYPE_WIFI:			///<WIFI�ʐM
  case GFL_NET_TYPE_WIFI_LOBBY:	///<WIFI�L��ʐM
    gcsp->comm_status = GAME_COMM_STATUS_WIFI;
    break;
  case GFL_NET_TYPE_IRC:			///<�ԊO���ʐM
    gcsp->comm_status = GAME_COMM_STATUS_IRC;
    break;
  default:
    GF_ASSERT(0);
    break;
  }
  return gcsp->comm_status;
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
 * @retval      FALSE:�ʏ�̍X�V���������s���Ă��� or GAME_COMM_NO_NULL ���
 */
//==================================================================
BOOL GameCommSys_CheckSystemWaiting(GAME_COMM_SYS_PTR gcsp)
{
  if(gcsp->game_comm_no != GAME_COMM_NO_NULL && gcsp->sub_work.seq != GCSSEQ_UPDATE){
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

//==================================================================
/**
 * GAMEDATA�ւ̃|�C���^���擾����
 *
 * @param   gcsp		
 *
 * @retval  GAMEDATA *		�Q�[���f�[�^�ւ̃|�C���^
 */
//==================================================================
GAMEDATA * GameCommSys_GetGameData(GAME_COMM_SYS_PTR gcsp)
{
  return gcsp->gamedata;
}

//==================================================================
/**
 * �Ō�Ɏ��s���Ă����ʐM�Q�[���ԍ����擾����
 *
 * @param   gcsp		
 *
 * @retval  GAME_COMM_NO		�Ō�Ɏ��s���Ă����Q�[���ԍ�
 */
//==================================================================
GAME_COMM_NO GameCommSys_GetLastCommNo(GAME_COMM_SYS_PTR gcsp)
{
  return gcsp->last_comm_no;
}

//==================================================================
/**
 * �Ō�Ɏ��s���Ă���GAME_COMM_NO�̏I����Ԃ��Z�b�g
 *
 * @param   gcsp		
 * @param   last_status		
 */
//==================================================================
void GameCommSys_SetLastStatus(GAME_COMM_SYS_PTR gcsp, GAME_COMM_LAST_STATUS last_status)
{
  GF_ASSERT_MSG(gcsp->last_status == GAME_COMM_LAST_STATUS_NULL, "lastst %d, %d", gcsp->last_status, last_status);
  gcsp->last_status = last_status;
}

//==================================================================
/**
 * �Ō�Ɏ��s���Ă���GAME_COMM_NO�̏I����Ԃ��Z�b�g
 *
 * @param   gcsp		
 * @param   last_status		
 */
//==================================================================
void GameCommSys_ClearLastStatus(GAME_COMM_SYS_PTR gcsp)
{
  gcsp->last_status = GAME_COMM_LAST_STATUS_NULL;
}

//==================================================================
/**
 * �Ō�Ɏ��s���Ă���GAME_COMM_NO�̏I����Ԃ��擾
 *
 * @param   gcsp		
 *
 * @retval  GAME_COMM_LAST_STATUS		
 */
//==================================================================
GAME_COMM_LAST_STATUS GameCommSys_GetLastStatus(GAME_COMM_SYS_PTR gcsp)
{
  return gcsp->last_status;
}


//--------------------------------------------------------------
/**
 * �T�u���[�N�̃V�[�P���X�ԍ����Z�b�g����
 *
 * @param   sub_work		
 * @param   seq		
 */
//--------------------------------------------------------------
static void GameCommSub_SeqSet(GAME_COMM_SUB_WORK *sub_work, u8 seq)
{
  sub_work->seq = seq;
  sub_work->func_seq = 0;
}


//==============================================================================
//  �v���C���[�X�e�[�^�X
//==============================================================================
//==================================================================
/**
 * �ʐM�v���C���[�X�e�[�^�X���Z�b�g����
 *
 * @param   gcsp		  
 * @param   comm_net_id		    ���̃X�e�[�^�X�Ώۂ�netID
 * @param   zone_id		        comm_net_id�̃v���C���[��������]�[��ID
 * @param   invasion_netid		comm_net_id�̃v���C���[���N�����Ă���ROM��netID
 * @param   pm_version        comm_net_id�̃v���C���[��PM_VERSION
 * @param   first_status      TRUE:���߂ăZ�b�g����X�e�[�^�X�@FALSE:2��ڈȍ~
 */
//==================================================================
void GameCommStatus_SetPlayerStatus(GAME_COMM_SYS_PTR gcsp, int comm_net_id, ZONEID zone_id, u8 invasion_netid, int pm_version, BOOL first_status)
{
  GAME_COMM_PLAYER_STATUS *player_status = &gcsp->player_status[comm_net_id];
  ZONEID old, now;
  
  old = player_status->old_zone_id;
  now = player_status->zone_id;
  
  player_status->pm_version = pm_version;
  
  if(player_status->zone_id == zone_id && player_status->invasion_netid == invasion_netid){
    return;
  }
  
  //���b�Z�[�W�쐬
  if(player_status->zone_id != zone_id){
    player_status->old_zone_id = player_status->zone_id;
    player_status->zone_id = zone_id;
  }
  if(player_status->invasion_netid != invasion_netid){
    player_status->old_invasion_netid = player_status->invasion_netid;
    player_status->invasion_netid = invasion_netid;
    if(first_status == FALSE){
      GameCommInfo_MessageEntry_IntrudePalace(gcsp, comm_net_id, invasion_netid);
    }
  }
}

//==================================================================
/**
 * �ʐM�v���C���[�X�e�[�^�X����N����NetID���擾
 *
 * @param   gcsp		
 * @param   comm_net_id		
 *
 * @retval  u8		�N����NetID
 */
//==================================================================
u8 GameCommStatus_GetPlayerStatus_InvasionNetID(GAME_COMM_SYS_PTR gcsp, int comm_net_id)
{
  GAME_COMM_PLAYER_STATUS *player_status = &gcsp->player_status[comm_net_id];
  return player_status->invasion_netid;
}

//==================================================================
/**
 * �ʐM�v���C���[�X�e�[�^�X����ROM�o�[�W�������擾
 *
 * @param   gcsp		
 * @param   comm_net_id		
 *
 * @retval  u8		ROM�o�[�W����
 */
//==================================================================
u8 GameCommStatus_GetPlayerStatus_RomVersion(GAME_COMM_SYS_PTR gcsp, int comm_net_id)
{
  GAME_COMM_PLAYER_STATUS *player_status = &gcsp->player_status[comm_net_id];
  return player_status->pm_version;
}

//==================================================================
/**
 * �ʐM�v���C���[�X�e�[�^�X��������
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommStatus_InitPlayerStatus(GAME_COMM_SYS_PTR gcsp)
{
  GFL_STD_MemClear(gcsp->player_status, sizeof(GAME_COMM_PLAYER_STATUS) * COMM_PLAYER_MAX);
}


//==============================================================================
//  �C���t�H���[�V�������b�Z�[�W
//==============================================================================
//--------------------------------------------------------------
/**
 * �C���t�H���[�V�������b�Z�[�W���L���[�ɒ��߂�
 *
 * @param   gcsp		      
 * @param   comm_net_id		  ���b�Z�[�W�̎�ƂȂ�l����NetID
 * @param   target_net_id		���b�Z�[�W�̎�̃^�[�Q�b�g�ƂȂ�l����NetID
 * @param   message_id		
 */
//--------------------------------------------------------------
static void GameCommInfo_SetQue(GAME_COMM_SYS_PTR gcsp, int comm_net_id, int target_net_id, u32 message_id)
{
  GAME_COMM_INFO *comm_info = &gcsp->info;
  GAME_COMM_INFO_QUE *que;
  const MYSTATUS *myst, *target_myst;

  if(comm_net_id == GAMEDATA_GetIntrudeMyID(gcsp->gamedata)){
    myst = GAMEDATA_GetMyStatus(gcsp->gamedata);
  }
  else{
    myst = GAMEDATA_GetMyStatusPlayer(gcsp->gamedata, comm_net_id);
  }
  if(target_net_id == GAMEDATA_GetIntrudeMyID(gcsp->gamedata)){
    target_myst = GAMEDATA_GetMyStatus(gcsp->gamedata);
  }
  else{
    target_myst = GAMEDATA_GetMyStatusPlayer(gcsp->gamedata, target_net_id);
  }
  
  //��check�@�b�菈���@�ʐM�m�����K�����O�������������Ă���󋵂ł͂Ȃ�����
  if(MyStatus_CheckNameClear(myst) == TRUE || MyStatus_CheckNameClear(target_myst) == TRUE){
    return;
  }

  if(comm_info->msg_que[comm_info->space_pos].use == TRUE){
    //���b�Z�[�W�̗D�揇�ʂ����܂��Ă��Ȃ����͏㏑��(�Â��L���[�͍ŐV�̃L���[�ŏ㏑�������
    que = &comm_info->msg_que[comm_info->space_pos];
    comm_info->now_pos++;   //�ŌẪL���[����\�������悤��now_pos�̈ʒu�����ɂ��炷
  }
  else{ //�󂢂Ă���L���[�Ȃ̂ł��̂܂ܑ��
    que = &comm_info->msg_que[comm_info->space_pos];
  }

  que->net_id = comm_net_id;
  que->message_id = message_id;
  que->use = TRUE;
  
  MyStatus_CopyNameString(myst, que->name_strbuf);
  que->sex = MyStatus_GetMySex(myst);
  MyStatus_CopyNameString(target_myst, que->target_name_strbuf);
  que->target_sex = MyStatus_GetMySex(target_myst);
  
  comm_info->space_pos++;
  if(comm_info->space_pos >= COMM_INFO_QUE_MAX){
    comm_info->space_pos = 0;
  }
  if(comm_info->now_pos >= COMM_INFO_QUE_MAX){
    comm_info->now_pos = 0;
  }
}

//==================================================================
/**
 * �C���t�H���[�V�������b�Z�[�W���擾����
 *
 * @param   gcsp		    
 * @param   dest_msg		���b�Z�[�W�����
 *
 * @retval  BOOL		TRUE:�f�[�^����
 * @retval  BOOL		FALSE:�f�[�^�Ȃ�
 */
//==================================================================
BOOL GameCommInfo_GetMessage(GAME_COMM_SYS_PTR gcsp, GAME_COMM_INFO_MESSAGE *dest_msg)
{
  GAME_COMM_INFO *comm_info = &gcsp->info;
  GAME_COMM_INFO_QUE *que = &comm_info->msg_que[comm_info->now_pos];
  int i;
  
  if(que->use == FALSE){
    return FALSE;
  }
  
  //�L���[��GAME_COMM_INFO_MESSAGE�^�ɕϊ����đ��
  GFL_STD_MemClear(dest_msg, sizeof(GAME_COMM_INFO_MESSAGE));

  dest_msg->name[0] = que->name_strbuf;
  dest_msg->wordset_no[0] = 0;
  dest_msg->wordset_sex[0] = que->sex;
  dest_msg->name[1] = que->target_name_strbuf;
  dest_msg->wordset_no[1] = 1;
  dest_msg->wordset_sex[1] = que->target_sex;

  dest_msg->message_id = que->message_id;
  
  //�L���[�̎Q�ƈʒu��i�߂�
  que->use = FALSE;
  comm_info->now_pos++;
  if(comm_info->now_pos >= COMM_INFO_QUE_MAX){
    comm_info->now_pos = 0;
  }
  
  return TRUE;
}

//==================================================================
/**
 * �C���t�H���[�V�������b�Z�[�W�o�^�F�uxxxx�v�́uxxxx�v�̃p���X����p���[���������
 *
 * @param   gcsp		
 * @param   player_netid		���NetID
 * @param   target_netid		�^�[�Q�b�g��NetID
 */
//==================================================================
void GameCommInfo_MessageEntry_GetPower(GAME_COMM_SYS_PTR gcsp, int player_netid, int target_netid)
{
  u16 msg_id = (player_netid == target_netid) ? msg_invasion_info_010 : msg_invasion_info_009;
  GameCommInfo_SetQue(gcsp, player_netid, target_netid, msg_id);
}

//==================================================================
/**
 * �C���t�H���[�V�������b�Z�[�W�o�^�F�uxxxx�v���uxxxx�v�̃p���X�ɐN�����܂���
 *
 * @param   gcsp		
 * @param   player_netid		���NetID
 * @param   target_netid		�^�[�Q�b�g��NetID
 */
//==================================================================
void GameCommInfo_MessageEntry_IntrudePalace(GAME_COMM_SYS_PTR gcsp, int player_netid, int target_netid)
{
  u16 msg_id = (player_netid == target_netid) ? msg_invasion_info_001 : msg_invasion_info_000;
  GameCommInfo_SetQue(gcsp, player_netid, target_netid, msg_id);
}

//==================================================================
/**
 * �C���t�H���[�V�������b�Z�[�W�o�^�F�uxxxx�v�̃p���X�ƂȂ���܂���
 *
 * @param   gcsp		
 * @param   player_netid		���NetID
 */
//==================================================================
void GameCommInfo_MessageEntry_PalaceConnect(GAME_COMM_SYS_PTR gcsp, int player_netid)
{
  GameCommInfo_SetQue(gcsp, player_netid, player_netid, msg_invasion_info_002);
}

//==================================================================
/**
 * �C���t�H���[�V�������b�Z�[�W�o�^�F�uxxxx�v�͋A���Ă����܂���
 *
 * @param   gcsp		
 * @param   player_netid		���NetID
 */
//==================================================================
void GameCommInfo_MessageEntry_Leave(GAME_COMM_SYS_PTR gcsp, int player_netid)
{
  GameCommInfo_SetQue(gcsp, player_netid, player_netid, msg_invasion_info_003);
}

//==================================================================
/**
 * �C���t�H���[�V�������b�Z�[�W�o�^�F�~�b�V��������
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommInfo_MessageEntry_MissionSuccess(GAME_COMM_SYS_PTR gcsp)
{
  GameCommInfo_SetQue(gcsp, GAMEDATA_GetIntrudeMyID(gcsp->gamedata), 
    GAMEDATA_GetIntrudeMyID(gcsp->gamedata), msg_invasion_info_003);
}

//==================================================================
/**
 * �C���t�H���[�V�������b�Z�[�W�o�^�F�~�b�V�������s
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommInfo_MessageEntry_MissionFail(GAME_COMM_SYS_PTR gcsp)
{
  GameCommInfo_SetQue(gcsp, GAMEDATA_GetIntrudeMyID(gcsp->gamedata), 
    GAMEDATA_GetIntrudeMyID(gcsp->gamedata), msg_invasion_info_003);
}

