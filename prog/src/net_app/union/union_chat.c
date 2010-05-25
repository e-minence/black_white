//==============================================================================
/**
 * @file    union_chat.c
 * @brief   �`���b�g����
 * @author  matsuda
 * @date    2009.08.26(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "net_app/union/union_main.h"
#include "print\str_tool.h"
#include "union_local.h"
#include "union_chat.h"
#include "union_tool.h"
#include "savedata/etc_save.h"


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static BOOL UnionChat_CheckSameLog(UNION_CHAT_LOG *log, const u8 *mac_address, u16 rand);



//==================================================================
/**
 * �`���b�g�o�^
 *
 * @param   unisub		
 * @param   bpc		          ���������v���C���[(�����̔����̏ꍇ��NULL�w��)
 * @param   mine_pmsdata		�����̔���(�����ł͂Ȃ��l�̔����̏ꍇ��NULL�w��)
 * @param   mine_chat_type	�����̔����̃`���b�g�^�C�v(�����ł͂Ȃ��l�̔����̏ꍇ�͖���)
 */
//==================================================================
void UnionChat_AddChat(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *bpc, const PMS_DATA *mine_pmsdata, UNION_CHAT_TYPE mine_chat_type)
{
  UNION_CHAT_LOG *log = &unisys->chat_log;
  UNION_CHAT_DATA *dest;
  STRCODE *name;
  
  GF_ASSERT(bpc != NULL || mine_pmsdata != NULL);
  
  //�����̔����͓���`�F�b�N��ʂ��Ȃ��B���l�̔����͓���`�F�b�N������
  if(bpc != NULL && UnionChat_CheckSameLog(log, bpc->mac_address, bpc->beacon.pms_rand) == FALSE){
    return;
  }
  
  //���O�Ǘ��f�[�^�Ƀf�[�^���
  dest = UnionChat_GetSpaceBuffer(log);
  if(bpc == NULL){
    MYSTATUS *myst = unisys->uniparent->mystatus;
    MyStatus_CopyNameStrCode(myst, dest->name, PERSON_NAME_SIZE + EOM_SIZE);
    GF_ASSERT(mine_pmsdata != NULL || (mine_pmsdata==NULL && mine_chat_type != UNION_CHAT_TYPE_NORMAL));
    dest->pmsdata = *mine_pmsdata;
    dest->chat_type = mine_chat_type;
    dest->rand = 0;
    OS_GetMacAddress(dest->mac_address);
    dest->sex = MyStatus_GetMySex(myst);
  }
  else{
    STRTOOL_Copy(bpc->beacon.name, dest->name, PERSON_NAME_SIZE + EOM_SIZE);
    dest->pmsdata = bpc->beacon.pmsdata;
    dest->chat_type = bpc->beacon.chat_type;
    dest->rand = bpc->beacon.pms_rand;
    GFL_STD_MemCopy(bpc->mac_address, dest->mac_address, 6);
    dest->sex = bpc->beacon.sex;
    {//�F�B�`�F�b�N
      GAMEDATA *gamedata = GAMESYSTEM_GetGameData(unisys->uniparent->gsys);
      WIFI_LIST* wifilist = GAMEDATA_GetWiFiList(gamedata);
      ETC_SAVE_WORK *etc_save = SaveData_GetEtc( GAMEDATA_GetSaveControlWork(gamedata) );
      
      if(UnionTool_CheckDwcFriend(wifilist, &bpc->beacon) == TRUE){
        dest->friend_type = UNION_CHAT_FRIEND_TYPE_FRIEND;
      }
      else if(EtcSave_CheckAcquaintance(etc_save, bpc->beacon.trainer_id) == TRUE){
        dest->friend_type = UNION_CHAT_FRIEND_TYPE_ACQUAINTANCE;
      }
    }
  }
  
  //���݌��Ă���\���ʒu���I�[�ł���Ȃ�A�\���ʒu���ǉ����ꂽ�`���b�g���ɍ��킹�Ă��炵�Ă���
  if(log->chat_log_count == log->chat_view_no){
    log->chat_view_no++;
  }
  log->chat_log_count++;
  //�\���ʒu�����O�ۑ��������ꂽ�ꏊ�����Ă���Ȃ�A���̕��\���ʒu��i�߂�
  if(log->chat_log_count - log->chat_view_no >= UNION_CHAT_LOG_MAX){
    log->chat_view_no = log->chat_log_count - UNION_CHAT_LOG_MAX + 1;
  }
}

//--------------------------------------------------------------
/**
 * �`���b�g���O�Ǘ��f�[�^����󂫃o�b�t�@���擾����
 *
 * @param   log		�`���b�g���O�Ǘ��f�[�^�ւ̃|�C���^
 *
 * @retval  UNION_CHAT_DATA *		�󂫃o�b�t�@�ւ̃|�C���^
 */
//--------------------------------------------------------------
UNION_CHAT_DATA * UnionChat_GetSpaceBuffer(UNION_CHAT_LOG *log)
{
  UNION_CHAT_DATA *buffer;
  
  if(log->chat_log_count > 0 && log->end_no == log->start_no){
    log->start_no++;
    if(log->start_no >= UNION_CHAT_LOG_MAX){
      log->start_no = 0;
    }
  }

  buffer = &log->chat[log->end_no];
  
  log->end_no++;
  if(log->end_no >= UNION_CHAT_LOG_MAX){
    log->end_no = 0;
  }
  
  return buffer;
}

//--------------------------------------------------------------
/**
 * �`���b�g���O�Ǘ��f�[�^����w�胍�O�ԍ��̃f�[�^��ǂݎ��
 *
 * @param   log		    �`���b�g���O�Ǘ��f�[�^�ւ̃|�C���^
 * @param   log_no		���O�ԍ�
 *
 * @retval  UNION_CHAT_DATA *		�`���b�g�f�[�^�ւ̃|�C���^(�f�[�^�������ꍇ��NULL)
 */
//--------------------------------------------------------------
UNION_CHAT_DATA * UnionChat_GetReadBuffer(UNION_CHAT_LOG *log, u32 log_no)
{
  u32 buffer_pos;
  static const u8 invalid_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  
  buffer_pos = log_no % UNION_CHAT_LOG_MAX;  //(log->start_no + log_no) % UNION_CHAT_LOG_MAX;
  if(GFL_STD_MemComp(log->chat[buffer_pos].mac_address, invalid_mac, 6) == 0){
    return NULL;
  }
  OS_TPrintf("aaa buffer_pos = %d, start=%d, log_no=%d\n", buffer_pos, log->start_no, log_no);
  return &log->chat[buffer_pos];
}

//--------------------------------------------------------------
/**
 * �`���b�g�f�[�^�������l�����Ō�ɔ��������`���b�g�f�[�^�Ɠ������̂ł͂Ȃ������ׂ�
 *
 * @param   log		        ���O�Ǘ��f�[�^�ւ̃|�C���^
 * @param   mac_address		�����҂�MacAddress
 * @param   rand  	      �����҂̃����_���R�[�h
 *
 * @retval  BOOL		TRUE:���������ł͂Ȃ��B�@FALSE:���������ł���
 */
//--------------------------------------------------------------
static BOOL UnionChat_CheckSameLog(UNION_CHAT_LOG *log, const u8 *mac_address, u16 rand)
{
  int log_no;
  UNION_CHAT_DATA *check_chat;
  
  for(log_no = UNION_CHAT_LOG_MAX - 1; log_no > -1; log_no--){
    check_chat = UnionChat_GetReadBuffer(log, log_no);
    if(check_chat != NULL){
      if(GFL_STD_MemComp(check_chat->mac_address, mac_address, 6) == 0){
        if(check_chat->rand != rand){
          return TRUE;  //�����l���̒��O�̃��O���Ⴄrand�R�[�h�ׁ̈A�V�K�̔���
        }
        return FALSE;
      }
    }
  }
  return TRUE;  //���̐l���̔��������O�ɑ��݂��Ȃ�
}

