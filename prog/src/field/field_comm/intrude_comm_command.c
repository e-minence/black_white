//==============================================================================
/**
 * @file    intrude_comm_command.c
 * @brief   �N���ʐM�R�}���h
 * @author  matsuda
 * @date    2009.09.03(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "net/network_define.h"
#include "intrude_types.h"
#include "intrude_comm_command.h"
#include "field/zonedata.h"
#include "bingo_system.h"
#include "intrude_main.h"


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size);
static void _IntrudeRecv_Shutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_ProfileReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_Profile(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_PlayerStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_Talk(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_TalkAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_BingoIntrusion(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_BingoIntrusionAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_ReqBingoIntrusionParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_BingoIntrusionParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


//==============================================================================
//  �f�[�^
//==============================================================================
///���j�I����M�R�}���h�e�[�u��   ��INTRUDE_CMD_???�ƕ��т𓯂��ɂ��Ă������ƁI�I
const NetRecvFuncTable Intrude_CommPacketTbl[] = {
  {_IntrudeRecv_Shutdown, NULL},               //INTRUDE_CMD_SHUTDOWN
  {_IntrudeRecv_ProfileReq, NULL},             //INTRUDE_CMD_PROFILE_REQ
  {_IntrudeRecv_Profile, NULL},                //INTRUDE_CMD_PROFILE
  {_IntrudeRecv_PlayerStatus, NULL},           //INTRUDE_CMD_PLAYER_STATUS
  {_IntrudeRecv_Talk, NULL},                   //INTRUDE_CMD_TALK
  {_IntrudeRecv_TalkAnswer, NULL},             //INTRUDE_CMD_TALK_ANSWER
  {_IntrudeRecv_BingoIntrusion, NULL},         //INTRUDE_CMD_BINGO_INTRUSION
  {_IntrudeRecv_BingoIntrusionAnswer, NULL},   //INTRUDE_CMD_BINGO_INTRUSION_ANSWER
  {_IntrudeRecv_ReqBingoIntrusionParam, NULL}, //INTRUDE_CMD_REQ_BINGO_INTRUSION_PARAM
  {_IntrudeRecv_BingoIntrusionParam, NULL},    //INTRUDE_CMD_BINGO_INTRUSION_PARAM
};
SDK_COMPILER_ASSERT(NELEMS(Intrude_CommPacketTbl) == INTRUDE_CMD_NUM);



//==============================================================================
//
//  
//
//==============================================================================

//--------------------------------------------------------------
/**
 * ����f�[�^�p��M�o�b�t�@�擾
 *
 * @param   netID		
 * @param   pWork		
 * @param   size		
 *
 * @retval  u8 *		
 */
//--------------------------------------------------------------
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size)
{
#if 0 //��check�@�܂�����f�[�^�p�ӂ��Ă��Ȃ�
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  GF_ASSERT_MSG(size <= UNION_HUGE_RECEIVE_BUF_SIZE, "size=%x, recv_size=%x\n", size, UNION_HUGE_RECEIVE_BUF_SIZE);
	return intcomm->huge_receive_buf[netID];
#else
  GF_ASSERT(0);
  return NULL;
#endif
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�ؒf
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_Shutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  
  intcomm->exit_recv = TRUE;
  OS_TPrintf("�ؒf�R�}���h��M\n");
}

//==================================================================
/**
 * �f�[�^���M�F�ؒf
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_Shutdown(void)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), INTRUDE_CMD_SHUTDOWN, 0, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�v���t�B�[����v��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_ProfileReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  
  intcomm->profile_req = TRUE;
  OS_TPrintf("�v���t�B�[���v���R�}���h��M net_id=%d\n", netID);
}

//==================================================================
/**
 * �f�[�^���M�F�v���t�B�[����v��
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_ProfileReq(void)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), INTRUDE_CMD_PROFILE_REQ, 0, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�v���t�B�[��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_Profile(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const INTRUDE_PROFILE *recv_profile = pData;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  MYSTATUS *myst;
  OCCUPY_INFO *occupy;
  
  myst = GAMEDATA_GetMyStatusPlayer(gamedata, netID);
  MyStatus_Copy(&recv_profile->mystatus, myst);

  occupy = GAMEDATA_GetOccupyInfo(gamedata, netID);
  GFL_STD_MemCopy(&recv_profile->occupy, occupy, sizeof(OCCUPY_INFO));
  
  _IntrudeRecv_PlayerStatus(netID, size, &recv_profile->status, pWork, pNetHandle);
  
  intcomm->recv_profile |= 1 << netID;
  OS_TPrintf("�v���t�B�[����M�@net_id=%d, recv_bit=%d\n", netID, intcomm->recv_profile);
}

//==================================================================
/**
 * �f�[�^���M�F�����̃v���t�B�[��
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_Profile(INTRUDE_COMM_SYS_PTR intcomm)
{
  BOOL ret;
  
  ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_PROFILE, sizeof(INTRUDE_PROFILE), &intcomm->send_profile);
  if(ret == TRUE){
    intcomm->profile_req = FALSE;
    OS_TPrintf("�����v���t�B�[�����M\n");
  }
  else{
    OS_TPrintf("�����v���t�B�[�����M���s\n");
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F���ݒl���
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_PlayerStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const INTRUDE_STATUS *recv_data = pData;
  INTRUDE_STATUS *target_status;
  int mission_no;
  
	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return;   //�����̃f�[�^�͎󂯎��Ȃ�
  }
  target_status = &intcomm->intrude_status[netID];
  GFL_STD_MemCopy(recv_data, target_status, sizeof(INTRUDE_STATUS));

  //�v���C���[�X�e�[�^�X�Ƀf�[�^�Z�b�g�@��game_comm�Ɏ������Ă���̂�ς��邩��
  if(netID == 0){
    mission_no = target_status->mission_no;
  }
  else{
    mission_no = GameCommStatus_GetPlayerStatus_MissionNo(intcomm->game_comm, 0);
    //mission_no = GameCommStatus_GetPlayerStatus_MissionNo(game_comm, GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()));
  }
  GameCommStatus_SetPlayerStatus(intcomm->game_comm, netID, target_status->zone_id,
    target_status->palace_area, target_status->zone_id);

  {//�]�[���Ⴂ�ɂ��A�N�^�[�\���E��\���ݒ�
    PLAYER_WORK *my_player = GAMEDATA_GetMyPlayerWork(GameCommSys_GetGameData(intcomm->game_comm));
    ZONEID my_zone_id = PLAYERWORK_getZoneID( my_player );
    
    if(target_status->zone_id != my_zone_id){
      target_status->player_pack.vanish = TRUE;   //�Ⴄ�]�[���ɂ���̂Ŕ�\��
    }
    else if(ZONEDATA_IsPalace(target_status->zone_id) == FALSE
        && target_status->palace_area != intcomm->intrude_status_mine.palace_area){
      //�ʏ�t�B�[���h�̓����]�[���ɋ��Ă��A�N����ROM���Ⴄ�Ȃ��\��
      target_status->player_pack.vanish = TRUE;
    }
    else{
      target_status->player_pack.vanish = FALSE;
    }
  }
}

//==================================================================
/**
 * �f�[�^���M�F���ݒl���
 *
 * @param   intcomm		
 * @param   gamedata		
 * @param   send_status		���M�f�[�^�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_PlayerStatus(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_STATUS *send_status)
{
  BOOL ret;
  
  ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_PLAYER_STATUS, sizeof(INTRUDE_STATUS), send_status);
  if(ret == TRUE){
    intcomm->send_status = FALSE;
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�b��������
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_Talk(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  
  OS_TPrintf("�b��������ꂽ��M�@net_id=%d\n", netID);
  Intrude_SetTalkReq(intcomm, netID);
}

//==================================================================
/**
 * �f�[�^���M�F�b��������
 * @param   send_net_id		�b���������NetID
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_Talk(int send_net_id)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_TALK, 0, NULL, FALSE, FALSE, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�b��������ꂽ�̂Ŏ����̏󋵂�Ԏ��Ƃ��ĕԂ�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_TalkAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const INTRUDE_TALK_STATUS *answer = pData;
  
  OS_TPrintf("�b�������̕Ԏ���M�@net_id=%d, answer=%d\n", netID, *answer);
  Intrude_SetTalkAnswer(intcomm, netID, *answer);
}

//==================================================================
/**
 * �f�[�^���M�F�b��������ꂽ�̂Ŏ����̏󋵂�Ԏ��Ƃ��ĕԂ�
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_TalkAnswer(INTRUDE_COMM_SYS_PTR intcomm, int send_net_id, INTRUDE_TALK_STATUS answer)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_TALK_ANSWER, sizeof(INTRUDE_TALK_STATUS), &answer, FALSE, FALSE, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�r���S�o�g���������v��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_BingoIntrusion(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  BINGO_SYSTEM *bingo = Bingo_GetBingoSystemWork(intcomm);
  
  OS_TPrintf("�������v���R�}���h��M net_id=%d\n", netID);
  Bingo_Req_IntrusionPlayer(intcomm, bingo, netID);
}

//==================================================================
/**
 * �f�[�^���M�F�r���S�o�g���������v��
 * @param   send_net_id		���M��̃l�b�gID
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_BingoIntrusion(int send_net_id)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_BINGO_INTRUSION, 0, NULL, FALSE, FALSE, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�r���S�o�g���������v���̕Ԏ�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_BingoIntrusionAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  BINGO_SYSTEM *bingo = Bingo_GetBingoSystemWork(intcomm);
  const BINGO_INTRUSION_ANSWER *answer = pData;
  
  bingo->intrusion_recv_answer = *answer;
  OS_TPrintf("�����̕Ԏ���M answer=%d\n", *answer);
}

//==================================================================
/**
 * �f�[�^���M�F�r���S�o�g���������v���̕Ԏ�
 *
 * @param   send_net_id		���M��̃l�b�gID
 * @param   answer		    �Ԏ�
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_BingoIntrusionAnswer(int send_net_id, BINGO_INTRUSION_ANSWER answer)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_BINGO_INTRUSION_ANSWER, sizeof(BINGO_INTRUSION_ANSWER), 
    &answer, FALSE, FALSE, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�r���S�o�g�������p�̃p�����[�^�v��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_ReqBingoIntrusionParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  BINGO_SYSTEM *bingo = Bingo_GetBingoSystemWork(intcomm);
  
  bingo->param_req_bit |= 1 << netID;
  OS_TPrintf("�r���S�o�g�������p�����[�^�v����M net_id = %d\n", netID);
}

//==================================================================
/**
 * �f�[�^���M�F�r���S�o�g�������p�̃p�����[�^�v��
 * @param   send_net_id		���M��̃l�b�gID
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_ReqBingoBattleIntrusionParam(int send_net_id)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_REQ_BINGO_INTRUSION_PARAM, 0, NULL, FALSE, FALSE, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�r���S�o�g�������p�p�����[�^
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_BingoIntrusionParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  BINGO_SYSTEM *bingo = Bingo_GetBingoSystemWork(intcomm);
  const BINGO_INTRUSION_PARAM *inpara = pData;
  
  GFL_STD_MemCopy(inpara, &bingo->intrusion_param, size);
  OS_TPrintf("�r���S�o�g�������p�����[�^��M net_id = %d\n", netID);
}

//==================================================================
/**
 * �f�[�^���M�F�r���S�o�g�������p�̃p�����[�^���M
 * @param   send_net_id		���M��̃l�b�gID
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_BingoBattleIntrusionParam(BINGO_SYSTEM *bingo, int send_net_id)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_BINGO_INTRUSION_PARAM, sizeof(BINGO_INTRUSION_PARAM), &bingo->intrusion_param, 
    FALSE, FALSE, NULL);
}

