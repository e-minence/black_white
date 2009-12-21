//==============================================================================
/**
 * @file    game_beacon.c
 * @brief   �Q�[�����̃r�[�R�����Ǘ��V�X�e��
 * @author  matsuda
 * @date    2009.12.13(��)
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/game_data.h"
#include "gamesystem/game_beacon.h"
#include "msg/msg_beacon_status.h"
#include "savedata/encount_sv.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�r�[�R������
#define LIFE_TIME     (15 * 60)

///�s���ԍ�
typedef enum{
  GAMEBEACON_ACTION_NULL,                 ///<�f�[�^����
  GAMEBEACON_ACTION_APPEAL,               ///<�u�������ɂ��܂��v
  GAMEBEACON_ACTION_CONGRATULATIONS,      ///<�u���߂łƂ��I�v
  GAMEBEACON_ACTION_POKE_EVOLUTION,       ///<�|�P�����i��
  GAMEBEACON_ACTION_POKE_LVUP,            ///<�|�P�������x���A�b�v
  GAMEBEACON_ACTION_POKE_GET,             ///<�|�P�����ߊl
  GAMEBEACON_ACTION_UNION_IN,             ///<���j�I�����[������
  GAMEBEACON_ACTION_UNION_OUT,            ///<���j�I�����[���ގ�
  GAMEBEACON_ACTION_ENCOUNT_DOWN,         ///<�G���J�E���g���_�E��
  
  GAMEBEACON_ACTION_MAX,
}GAMEBEACON_ACTION;


//==============================================================================
//  �\���̒�`
//==============================================================================
///1���̃��O�f�[�^
typedef struct{
  GAMEBEACON_INFO info;
}GAMEBEACON_LOG;

///���M�r�[�R���Ǘ�
typedef struct{
  GAMEBEACON_INFO info;               ///<���M�r�[�R��
  s16 life;                           ///<���M����
  u16 padding;
}GAMEBEACON_SEND_MANAGER;

///�Q�[���r�[�R���Ǘ��V�X�e���̃|�C���^
typedef struct{
  GAMEDATA *gamedata;
  GAMEBEACON_SEND_MANAGER send;       ///<���M�r�[�R���Ǘ�
  GAMEBEACON_LOG log[GAMEBEACON_LOG_MAX]; ///<���O(�`�F�[���o�b�t�@)
  u32 log_count;                      ///<���O�S�̂̎�M�������J�E���g
  s8 start_log;                       ///<���O�̃`�F�[���J�n�ʒu
  s8 end_log;                         ///<���O�̃`�F�[���I�[�ʒu
  s8 log_num;                         ///<���O����
  u8 padding;
}GAMEBEACON_SYSTEM;


//==============================================================================
//  ���[�J���ϐ�
//==============================================================================
///�Q�[���r�[�R���Ǘ��V�X�e���̃|�C���^
static GAMEBEACON_SYSTEM * GameBeaconSys = NULL;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void SendBeacon_Update(GAMEBEACON_SEND_MANAGER *send);
static void BeaconInfo_Set(GAMEBEACON_SYSTEM *bsys, const GAMEBEACON_INFO *info);
static GAMEBEACON_INFO * BeaconInfo_Get(GAMEBEACON_SYSTEM *bsys, int log_no);
static void SendBeacon_Init(GAMEBEACON_SEND_MANAGER *send, GAMEDATA * gamedata);
static void SendBeacon_SetCommon(GAMEBEACON_SEND_MANAGER *send);


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �Q�[���r�[�R���Ǘ��V�X�e���̍쐬
 */
//==================================================================
void GAMEBEACON_Init(HEAPID heap_id)
{
  GF_ASSERT(GameBeaconSys == NULL);
  
  GameBeaconSys = GFL_HEAP_AllocClearMemory(heap_id, sizeof(GAMEBEACON_SYSTEM));
  GameBeaconSys->end_log = -1;
}

//==================================================================
/**
 * �Q�[���r�[�R���Ǘ��V�X�e���̔j��
 */
//==================================================================
void GAMEBEACON_Exit(void)
{
  GF_ASSERT(GameBeaconSys != NULL);
  
  GFL_HEAP_FreeMemory(GameBeaconSys);
}

//==================================================================
/**
 * �Q�[���r�[�R���Ǘ��V�X�e���̍X�V����
 */
//==================================================================
void GAMEBEACON_Update(void)
{
  GAMEBEACON_SYSTEM *bsys = GameBeaconSys;
  
  if(GameBeaconSys == NULL){
    return;
  }

  SendBeacon_Update(&bsys->send);
}

//==================================================================
/**
 * �Q�[���r�[�R���̏����ݒ���s���܂�
 *
 * @param   gamedata		�Q�[���f�[�^�ւ̃|�C���^
 *
 * gamedata�Ɏ����̖��O�Ȃǂ�ǂݍ��񂾌�ɁA�R�[������悤�ɂ��Ă��������B
 */
//==================================================================
void GAMEBEACON_Setting(GAMEDATA *gamedata)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;

  GameBeaconSys->gamedata = gamedata;
  SendBeacon_Init(send, gamedata);
}

//==================================================================
/**
 * �Q�[���r�[�R�����M�f�[�^���R�s�[
 *
 * @param   info		�R�s�[��ւ̃|�C���^
 */
//==================================================================
void GAMEBEACON_SendDataCopy(GAMEBEACON_INFO *info)
{
  GFL_STD_MemCopy(&GameBeaconSys->send, info, sizeof(GAMEBEACON_INFO));
}

//--------------------------------------------------------------
/**
 * ���M�r�[�R���X�V����
 *
 * @param   send		
 */
//--------------------------------------------------------------
static void SendBeacon_Update(GAMEBEACON_SEND_MANAGER *send)
{
  if(send->info.action_no != GAMEBEACON_ACTION_APPEAL){
    send->life++;
    if(send->life > LIFE_TIME){
      send->life = 0;
      send->info.action_no = GAMEBEACON_ACTION_APPEAL;
    }
  }
}

//--------------------------------------------------------------
/**
 * ���O�ɐV�����r�[�R�������Z�b�g����
 *
 * @param   bsys		
 * @param   info		�Z�b�g����r�[�R�����
 */
//--------------------------------------------------------------
static void BeaconInfo_Set(GAMEBEACON_SYSTEM *bsys, const GAMEBEACON_INFO *info)
{
  GAMEBEACON_LOG *setlog;
  int before_end_log;
  
  MATSUDA_Printf("GameBeaconSet chain_no = %d\n", bsys->end_log);

  before_end_log = bsys->end_log;
  bsys->end_log++;
  if(bsys->end_log >= GAMEBEACON_LOG_MAX){
    bsys->end_log = 0;
  }
  setlog = &bsys->log[bsys->end_log];

  if(bsys->log_num > 0 && bsys->end_log == bsys->start_log){
    bsys->start_log++;
    if(bsys->start_log >= GAMEBEACON_LOG_MAX){
      bsys->start_log = 0;
    }
  }
  
  GFL_STD_MemClear(setlog, sizeof(GAMEBEACON_LOG));
  setlog->info = *info;
  if(bsys->log_num < GAMEBEACON_LOG_MAX){
    bsys->log_num++;
  }
}

//==================================================================
/**
 * ��M�r�[�R�����Z�b�g
 *
 * @param   info		��M�����r�[�R���f�[�^�ւ̃|�C���^
 * 
 * @retval  TRUE:�Z�b�g�����@�@FALSE:���O�ɓ����f�[�^����M�ς�
 */
//==================================================================
BOOL GAMEBEACON_SetRecvBeacon(const GAMEBEACON_INFO *info)
{
  GAMEBEACON_SYSTEM *bsys = GameBeaconSys;
  int i;
  
//  OS_TPrintf("SetRecv action_NO = %d\n", info->action_no);
  if(info->action_no == GAMEBEACON_ACTION_NULL || info->action_no >= GAMEBEACON_ACTION_MAX){
    return FALSE;
  }
  
  if(bsys->start_log <= bsys->end_log){
    for(i = bsys->start_log; i <= bsys->end_log; i++){
      if(bsys->log[i].info.trainer_id == info->trainer_id){
        if(info->action_no == GAMEBEACON_ACTION_APPEAL 
            || bsys->log[i].info.send_counter == info->send_counter){
//        OS_TPrintf("���Ɏ�M�ς�\n");
          return FALSE; //���O�ɓ����f�[�^����M�ς�
        }
      }
    }
  }
  
  BeaconInfo_Set(bsys, info);
  bsys->log_count++;
  OS_TPrintf("�Z�b�g���� %d����\n", bsys->log_count);
  
  //���O�̃p���[��񔽉f�@��check ������x�p���[�����܂��Ă�����t�H�[�}�b�g������
  switch(info->action_no){
  case GAMEBEACON_ACTION_ENCOUNT_DOWN:
    {
      SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(bsys->gamedata);
      ENC_SV_PTR evc_sv = EncDataSave_GetSaveDataPtr(sv_ctrl);
      EncDataSave_SetSprayCnt(evc_sv, 100);
    }
    break;
  }
  
  return TRUE;
}

//--------------------------------------------------------------
/**
 * ���O����r�[�R�������擾����
 *
 * @param   bsys		
 * @param   log_no  �ŐV�̃��O����k���ĉ����ڂ̃��O���擾��������
 *                  (0���ƍŐV�F0�`GAMEBEACON_LOG_MAX-1)
 *
 * @retval  GAMEBEACON_INFO *		NULL�̏ꍇ�̓f�[�^����
 */
//--------------------------------------------------------------
static GAMEBEACON_INFO * BeaconInfo_Get(GAMEBEACON_SYSTEM *bsys, int log_no)
{
  int log_pos;
  
  if(bsys->log_num == 0 || bsys->log_num - log_no <= 0){
    return NULL;  //�f�[�^����
  }
  
  log_pos = bsys->end_log - log_no;
  if(log_pos < 0){
    log_pos = GAMEBEACON_LOG_MAX + log_pos;
  }
  return &bsys->log[log_pos].info;
}

//==================================================================
/**
 * ���O����r�[�R�������擾����
 *
 * @param   log_no  �ŐV�̃��O����k���ĉ����ڂ̃��O���擾��������
 *                  (0���ƍŐV�F0�`GAMEBEACON_LOG_MAX-1)
 *
 * @retval  GAMEBEACON_INFO *		NULL�̏ꍇ�̓f�[�^����
 */
//==================================================================
const GAMEBEACON_INFO * GAMEBEACON_Get_BeaconLog(int log_no)
{
  return BeaconInfo_Get(GameBeaconSys, log_no);
}

//==================================================================
/**
 * ���݂܂ł̃g�[�^����M���O�������擾
 *
 * @retval  u32		
 */
//==================================================================
u32 GAMEBEACON_Get_LogCount(void)
{
  return GameBeaconSys->log_count;
}

//==================================================================
/**
 * ���O����r�[�R�����̂��C�ɓ���̐F���擾����
 *
 * @param   dest_buf		�F�̑����ւ̃|�C���^
 * @param   log_no      �ŐV�̃��O����k���ĉ����ڂ̃��O���擾��������
 *                      (0���ƍŐV�F0�`GAMEBEACON_LOG_MAX-1)
 *
 * @retval  BOOL		TRUE:�r�[�R����񂪑��݂��Ă���
 * @retval  BOOL		FALSE:�r�[�R����񂪖������߁A�F�̎擾���o���Ȃ�
 */
//==================================================================
BOOL GAMEBEACON_Get_FavoriteColor(GXRgb *dest_buf, int log_no)
{
  GAMEBEACON_INFO *info = BeaconInfo_Get(GameBeaconSys, log_no);
  if(info != NULL){
    *dest_buf = info->favorite_color;
    return TRUE;
  }
  return FALSE;
}


//--------------------------------------------------------------
/**
 * ���M�r�[�R���ɏ����f�[�^���Z�b�g
 *
 * @param   send		
 * @param   gamedata		
 */
//--------------------------------------------------------------
static void SendBeacon_Init(GAMEBEACON_SEND_MANAGER *send, GAMEDATA * gamedata)
{
  GAMEBEACON_INFO *info = &send->info;
  MYSTATUS *myst = GAMEDATA_GetMyStatus(gamedata);

  info->action_no = GAMEBEACON_ACTION_APPEAL;
  MyStatus_CopyNameStrCode( myst, info->name, BUFLEN_PERSON_NAME );
  info->trainer_id = MyStatus_GetID(myst);
  info->favorite_color = *(OS_GetFavoriteColorTable());
}

//--------------------------------------------------------------
/**
 * ���M�r�[�R�����Z�b�g�������̋��ʏ���
 *
 * @param   send		
 * @param   info		
 */
//--------------------------------------------------------------
static void SendBeacon_SetCommon(GAMEBEACON_SEND_MANAGER *send)
{
  send->info.send_counter++;
  send->life = 0;
}

//==================================================================
/**
 * �r�[�R�����̓��e��WORDSET����
 *
 * @param   info		        �Ώۂ̃r�[�R�����ւ̃|�C���^
 * @param   wordset		
 * @param   temp_heap_id		�����Ńe���|�����Ƃ��Ďg�p����q�[�vID
 */
//==================================================================
void GAMEBEACON_Wordset(const GAMEBEACON_INFO *info, WORDSET *wordset, HEAPID temp_heap_id)
{
  STRBUF *name_strbuf = GFL_STR_CreateBuffer(BUFLEN_PERSON_NAME, temp_heap_id);
  STRBUF *nickname_strbuf = GFL_STR_CreateBuffer(BUFLEN_POKEMON_NAME, temp_heap_id);

	GFL_STR_SetStringCodeOrderLength(name_strbuf, info->name, BUFLEN_PERSON_NAME);
  WORDSET_RegisterWord(wordset, 0, name_strbuf, 0, TRUE, PM_LANG);

  switch(info->action_no){
  case GAMEBEACON_ACTION_POKE_EVOLUTION:       ///<�|�P�����i��
  case GAMEBEACON_ACTION_POKE_LVUP:            ///<�|�P�������x���A�b�v
  case GAMEBEACON_ACTION_POKE_GET:             ///<�|�P�����ߊl
  	GFL_STR_SetStringCodeOrderLength(name_strbuf, info->nickname, BUFLEN_POKEMON_NAME);
    WORDSET_RegisterWord(wordset, 1, name_strbuf, 0, TRUE, PM_LANG);
    break;
  }

  GFL_STR_DeleteBuffer(name_strbuf);
  GFL_STR_DeleteBuffer(nickname_strbuf);
}

//==================================================================
/**
 * �r�[�R����񂩂�\�����郁�b�Z�[�WID���擾����
 *
 * @param   info		�Ώۂ̃r�[�R�����ւ̃|�C���^
 */
//==================================================================
u32 GAMEBEACON_GetMsgID(const GAMEBEACON_INFO *info)
{
  if(info->action_no == GAMEBEACON_ACTION_NULL || info->action_no >= GAMEBEACON_ACTION_MAX){
    GF_ASSERT_MSG(0, "action_no = %d\n", info->action_no);
    return msg_beacon_001;
  }
  return msg_beacon_001 + info->action_no - GAMEBEACON_ACTION_APPEAL;
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�u���߂łƂ��I�v
 */
//==================================================================
void GAMEBEACON_Set_Congratulations(void)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  
  send->info.action_no = GAMEBEACON_ACTION_CONGRATULATIONS;
  SendBeacon_SetCommon(send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�����i��
 *
 * @param   nickname		�Ώۃ|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_PokemonEvolution(const STRBUF *nickname)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  
  send->info.action_no = GAMEBEACON_ACTION_POKE_EVOLUTION;
  GFL_STR_GetStringCode(nickname, send->info.nickname, BUFLEN_POKEMON_NAME);
  SendBeacon_SetCommon(send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�������x���A�b�v
 *
 * @param   nickname		�Ώۃ|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_PokemonLevelUp(const STRBUF *nickname)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  
  send->info.action_no = GAMEBEACON_ACTION_POKE_LVUP;
  GFL_STR_GetStringCode(nickname, send->info.nickname, BUFLEN_POKEMON_NAME);
  SendBeacon_SetCommon(send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�����ߊl
 *
 * @param   nickname		�Ώۃ|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_PokemonGet(const STRBUF *nickname)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  
  send->info.action_no = GAMEBEACON_ACTION_POKE_GET;
  GFL_STR_GetStringCode(nickname, send->info.nickname, BUFLEN_POKEMON_NAME);
  SendBeacon_SetCommon(send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���j�I�����[������
 */
//==================================================================
void GAMEBEACON_Set_UnionIn(void)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  
  send->info.action_no = GAMEBEACON_ACTION_UNION_IN;
  SendBeacon_SetCommon(send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���j�I�����[���ގ�
 */
//==================================================================
void GAMEBEACON_Set_UnionOut(void)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  
  send->info.action_no = GAMEBEACON_ACTION_UNION_OUT;
  SendBeacon_SetCommon(send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�G���J�E���g���_�E��
 */
//==================================================================
void GAMEBEACON_Set_EncountDown(void)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  
  send->info.action_no = GAMEBEACON_ACTION_ENCOUNT_DOWN;
  SendBeacon_SetCommon(send);
}
