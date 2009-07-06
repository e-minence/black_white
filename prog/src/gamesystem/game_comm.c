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
#include "field/field_comm/field_comm_func.h"
#include "fieldmap/zone_id.h"
#include "message.naix"
#include "msg/msg_invasion.h"
#include "net_app/union/union_main.h"


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

///�ʐM�ő�l��
#define COMM_PLAYER_MAX   (4)

///�C���t�H���[�V�������b�Z�[�W�L���[��
#define COMM_INFO_QUE_MAX     (24)


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
  u16 same_count;      ///<�����X�e�[�^�X�������Ă�����
  u8 invasion_netid;  ///<�N����ROM
  u8 padding;
}GAME_COMM_PLAYER_STATUS;

//--------------------------------------------------------------
//  �C���t�H���[�V����
//--------------------------------------------------------------
///�C���t�H���[�V�����̃��b�Z�[�W�L���[�\����
typedef struct{
  u16 message_id;
  u8 net_id;
  u8 use;           ///<TRUE:�g�p�� FALSE:���g�p
}GAME_COMM_INFO_QUE;

///�C���t�H���[�V�����\����
typedef struct{
  GAME_COMM_INFO_QUE msg_que[COMM_INFO_QUE_MAX];
  u8 now_pos;       ///<���܂��Ă���L���[�̊J�n�ʒu
  u8 space_pos;     ///<�󂫃L���[�̊J�n�ʒu
  u8 padding[2];
  
  STRBUF *name_strbuf[COMM_INFO_QUE_MAX];
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
  GAME_COMM_NO reserve_comm_game_no;   ///<�\��ʐM�Q�[���ԍ�
  GAME_COMM_SUB_WORK sub_work;
  void *parent_work;                  ///<�Ăяo�����Ɉ����n���|�C���^
  void *app_work;                     ///<�e�A�v���P�[�V�������m�ۂ������[�N�̃|�C���^
  void *reserve_parent_work;          ///<�Ăяo�����Ɉ����n���|�C���^
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
  //GAME_COMM_NO_UNION
  {
    UnionComm_Init,       //init
    UnionComm_InitWait,   //init_wait
    UnionComm_Update,     //update
    UnionComm_Exit,       //exit
    UnionComm_ExitWait,   //exit_wait
  },
};

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void GameCommSub_SeqSet(GAME_COMM_SUB_WORK *sub_work, u8 seq);
static void GameCommInfo_SetQue(GAME_COMM_SYS_PTR gcsp, int comm_net_id, u32 message_id);


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
  int i;
  
  gcsp = GFL_HEAP_AllocClearMemory(heap_id, sizeof(GAME_COMM_SYS));
  gcsp->gamedata = gamedata;
  
  comm_info = &gcsp->info;
  for(i = 0; i < COMM_PLAYER_MAX; i++){
    comm_info->name_strbuf[i] = GFL_STR_CreateBuffer(BUFLEN_PERSON_NAME, heap_id);
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
  
  for(i = 0; i < COMM_PLAYER_MAX; i++){
    GFL_STR_DeleteBuffer(comm_info->name_strbuf[i]);
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
      gcsp->app_work = NULL;
      gcsp->game_comm_no = GAME_COMM_NO_NULL;
      gcsp->comm_status = GAME_COMM_STATUS_NULL;
      if(gcsp->reserve_comm_game_no != GAME_COMM_NO_NULL){
        GameCommSys_Boot(gcsp, gcsp->reserve_comm_game_no, gcsp->reserve_parent_work);
        gcsp->reserve_comm_game_no = GAME_COMM_NO_NULL;
        gcsp->reserve_parent_work = NULL;
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
 * @param   parent_work     �����n���|�C���^
 */
//==================================================================
void GameCommSys_Boot(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no, void *parent_work)
{
  GF_ASSERT(gcsp->game_comm_no == GAME_COMM_NO_NULL);
  
  gcsp->game_comm_no = game_comm_no;
  gcsp->parent_work = parent_work;
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
  GameCommSub_SeqSet(&gcsp->sub_work, GCSSEQ_EXIT);
}

//==================================================================
/**
 * �ʐM�Q�[���؂�ւ����N�G�X�g
 *
 * @param   gcsp		
 * @param   game_comm_no		�؂�ւ���A�N������ʐM�Q�[���ԍ�(GAME_COMM_NO_???)
 * @param   parent_work     �����n���|�C���^
 */
//==================================================================
void GameCommSys_ChangeReq(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no, void *parent_work)
{
  GF_ASSERT(gcsp->sub_work.seq == GCSSEQ_UPDATE);
  
  GameCommSub_SeqSet(&gcsp->sub_work, GCSSEQ_EXIT);
  gcsp->reserve_comm_game_no = game_comm_no;
  gcsp->reserve_parent_work = parent_work;
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
      //FIXME �������r�[�R�����̃`�F�b�N��
      u8 i;
      gcsp->comm_status = GAME_COMM_STATUS_NULL;
      for( i=0;i<4;i++ )
      {
        if( GFL_NET_GetBeaconData( i ) != NULL )
        {
          gcsp->comm_status = GAME_COMM_STATUS_WIRELESS;
        }
      }
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
 */
//==================================================================
void GameCommStatus_SetPlayerStatus(GAME_COMM_SYS_PTR gcsp, int comm_net_id, ZONEID zone_id, u8 invasion_netid)
{
  GAME_COMM_PLAYER_STATUS *player_status = &gcsp->player_status[comm_net_id];
  ZONEID old, now;
  
  old = player_status->old_zone_id;
  now = player_status->zone_id;
  
  if(player_status->zone_id == zone_id && player_status->invasion_netid == invasion_netid){
    return;
  }
  //��check�@�b�菈���@�ʐM�m�����K�����O�������������Ă���󋵂ł͂Ȃ�����
  {
    MYSTATUS *myst = GAMEDATA_GetMyStatusPlayer(gcsp->gamedata, comm_net_id);
    if(MyStatus_CheckNameClear(myst) == TRUE){
      OS_TPrintf("INFO:���O���Ȃ� net_id = %d\n", comm_net_id);
      return;
    }
  }
  
  if(player_status->zone_id != zone_id){
    player_status->old_zone_id = player_status->zone_id;
    player_status->zone_id = zone_id;
    player_status->same_count = 0;
  }
  else if(zone_id == ZONE_ID_PALACETEST){
    player_status->same_count++;
  }
  player_status->invasion_netid = invasion_netid;
  
  //���b�Z�[�W�쐬
  if(zone_id == ZONE_ID_PALACETEST){
    if(player_status->same_count == 0){
      GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test01_01 + comm_net_id);
      GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test03_01 + comm_net_id);
      OS_TPrintf("INFO:�p���X�ɂ��܂� net_id = %d\n", comm_net_id);
    }
    else if(player_status->same_count > 60 * 10){
      GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test01_01 + comm_net_id);
      GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test03_01 + comm_net_id);
      player_status->same_count = 0;
      OS_TPrintf("INFO:�p�����ăp���X�ɂ��܂� net_id = %d\n", comm_net_id);
    }
  }
  else if(player_status->old_zone_id == ZONE_ID_PALACETEST && comm_net_id != invasion_netid
      && zone_id != ZONE_ID_PALACETEST && invasion_netid == GFL_NET_SystemGetCurrentID()){
    GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test07_01 + comm_net_id);
    OS_TPrintf("INFO:�X�ɐN�����Ă����I net_id = %d\n", comm_net_id);
  }
  else if(old == 0 && now == 0){
    GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test09_01);
    OS_TPrintf("INFO:�ʐM�ڑ������I net_id = %d\n", comm_net_id);
  }
}


//==============================================================================
//  �C���t�H���[�V�������b�Z�[�W
//==============================================================================
//--------------------------------------------------------------
/**
 * �C���t�H���[�V�������b�Z�[�W���L���[�ɒ��߂�
 *
 * @param   gcsp		      
 * @param   comm_net_id		
 * @param   message_id		
 */
//--------------------------------------------------------------
static void GameCommInfo_SetQue(GAME_COMM_SYS_PTR gcsp, int comm_net_id, u32 message_id)
{
  GAME_COMM_INFO *comm_info = &gcsp->info;
  GAME_COMM_INFO_QUE *que;
  
  if(comm_info->msg_que[comm_info->space_pos].use == TRUE){
    //���b�Z�[�W�̗D�揇�ʂ����܂��Ă��Ȃ����͏㏑��(�Â��L���[�͍ŐV�̃L���[�ŏ㏑�������
    que = &comm_info->msg_que[comm_info->space_pos];
    comm_info->now_pos++;   //�ŌẪL���[����\�������悤��now_pos�̈ʒu�����ɂ��炷
  }
  else{ //�󂢂Ă���L���[�Ȃ̂ł��̂܂ܑ��
    que = &comm_info->msg_que[comm_info->space_pos];
  }

  GFL_STD_MemClear(que, sizeof(GAME_COMM_INFO_QUE));
  que->net_id = comm_net_id;
  que->message_id = message_id;
  que->use = TRUE;

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
  MYSTATUS *myst;
  
  if(que->use == FALSE){
    return FALSE;
  }
  
  //�L���[��GAME_COMM_INFO_MESSAGE�^�ɕϊ����đ��
  GFL_STD_MemClear(dest_msg, sizeof(GAME_COMM_INFO_MESSAGE));
  myst = GAMEDATA_GetMyStatusPlayer(gcsp->gamedata, que->net_id);
  MyStatus_CopyNameString(myst, comm_info->name_strbuf[que->net_id]);
  dest_msg->name[0] = comm_info->name_strbuf[que->net_id];
  dest_msg->wordset_no[0] = que->net_id;
  dest_msg->message_id = que->message_id;
  
  //�L���[�̎Q�ƈʒu��i�߂�
  que->use = FALSE;
  comm_info->now_pos++;
  if(comm_info->now_pos >= COMM_INFO_QUE_MAX){
    comm_info->now_pos = 0;
  }
  
  return TRUE;
}

