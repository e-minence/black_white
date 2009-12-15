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
#include "intrude_mission.h"


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size);
static void _IntrudeRecv_Shutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MemberNum(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_ProfileReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_Profile(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_DeleteProfile(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_PlayerStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_Talk(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_TalkAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_TalkCancel(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_BingoIntrusion(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_BingoIntrusionAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_ReqBingoIntrusionParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_BingoIntrusionParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionListReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionList(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionEntryAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _MissionOrderConfirm(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionAchieve(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionAchieveAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_OccupyInfo(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_TargetTiming(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_PlayerSupport(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_WfbcReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_Wfbc(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


//==============================================================================
//  �f�[�^
//==============================================================================
///���j�I����M�R�}���h�e�[�u��   ��INTRUDE_CMD_???�ƕ��т𓯂��ɂ��Ă������ƁI�I
const NetRecvFuncTable Intrude_CommPacketTbl[] = {
  {_IntrudeRecv_Shutdown, NULL},               //INTRUDE_CMD_SHUTDOWN
  {_IntrudeRecv_MemberNum, NULL},              //INTRUDE_CMD_MEMBER_NUM
  {_IntrudeRecv_ProfileReq, NULL},             //INTRUDE_CMD_PROFILE_REQ
  {_IntrudeRecv_Profile, NULL},                //INTRUDE_CMD_PROFILE
  {_IntrudeRecv_DeleteProfile, NULL},          //INTRUDE_CMD_DELETE_PROFILE
  {_IntrudeRecv_PlayerStatus, NULL},           //INTRUDE_CMD_PLAYER_STATUS
  {_IntrudeRecv_Talk, NULL},                   //INTRUDE_CMD_TALK
  {_IntrudeRecv_TalkAnswer, NULL},             //INTRUDE_CMD_TALK_ANSWER
  {_IntrudeRecv_TalkCancel, NULL},             //INTRUDE_CMD_TALK_CANCEL
  {_IntrudeRecv_BingoIntrusion, NULL},         //INTRUDE_CMD_BINGO_INTRUSION
  {_IntrudeRecv_BingoIntrusionAnswer, NULL},   //INTRUDE_CMD_BINGO_INTRUSION_ANSWER
  {_IntrudeRecv_ReqBingoIntrusionParam, NULL}, //INTRUDE_CMD_REQ_BINGO_INTRUSION_PARAM
  {_IntrudeRecv_BingoIntrusionParam, NULL},    //INTRUDE_CMD_BINGO_INTRUSION_PARAM
  {_IntrudeRecv_MissionListReq, NULL},         //INTRUDE_CMD_MISSION_LIST_REQ
  {_IntrudeRecv_MissionList, NULL},            //INTRUDE_CMD_MISSION_LIST
  {_MissionOrderConfirm, NULL},                //INTRUDE_CMD_MISSION_ORDER_CONFIRM
  {_IntrudeRecv_MissionEntryAnswer, NULL},     //INTRUDE_CMD_MISSION_ENTRY_ANSWER
  {_IntrudeRecv_MissionReq, NULL},             //INTRUDE_CMD_MISSION_REQ
  {_IntrudeRecv_MissionData, NULL},            //INTRUDE_CMD_MISSION_DATA
  {_IntrudeRecv_MissionAchieve, NULL},         //INTRUDE_CMD_MISSION_ACHIEVE
  {_IntrudeRecv_MissionAchieveAnswer, NULL},   //INTRUDE_CMD_MISSION_ACHIEVE_ANSWER
  {_IntrudeRecv_MissionResult, NULL},          //INTRUDE_CMD_MISSION_RESULT
  {_IntrudeRecv_OccupyInfo, NULL},             //INTRUDE_CMD_OCCUPY_INFO
  {_IntrudeRecv_TargetTiming, NULL},           //INTRUDE_CMD_TARGET_TIMING
  {_IntrudeRecv_PlayerSupport, NULL},          //INTRUDE_CMD_PLAYER_SUPPORT
  {_IntrudeRecv_WfbcReq, NULL},                //INTRUDE_CMD_WFBC_REQ
  {_IntrudeRecv_Wfbc, NULL},                   //INTRUDE_CMD_WFBC
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

//--------------------------------------------------------------
/**
 * �����ȊO�̃v���C���[�����݂��Ă��邩���ׂ�
 *
 * @param   none		
 *
 * @retval  BOOL		TRUE:���݂��Ă���B�@FALSE:�N�����Ȃ�
 * 
 * ���E�ȂǂŐe��l�ɂȂ����ꍇ�A�L���[�ɒ��܂����o�b�t�@���������ꂸ�A�L���[�����Ă��܂��̂�
 * ���M�R�}���h�����s���Ȃ��悤�Ƀ`�F�b�N����֐�
 */
//--------------------------------------------------------------
static BOOL _OtherPlayerExistence(void)
{
  return Intrude_OtherPlayerExistence();
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
  OS_TPrintf("�ؒf�R�}���h��M netID=%d\n", netID);
}

//==================================================================
/**
 * �f�[�^���M�F�ؒf
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_Shutdown(INTRUDE_COMM_SYS_PTR intcomm)
{
  OS_TPrintf("SEND:�ؒf�R�}���h\n");
  
  if(_OtherPlayerExistence() == FALSE){
    //���ɒN�����Ȃ��̂Ŏ����Ŏ�M���߂��Ăяo��
    OS_TPrintf("�����Őؒf�R�[���o�b�N�𒼐ڌĂяo��\n");
    _IntrudeRecv_Shutdown(
      GFL_NET_SystemGetCurrentID(), 0, NULL, intcomm, GFL_NET_HANDLE_GetCurrentHandle());
    return TRUE;
  }
  
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), INTRUDE_CMD_SHUTDOWN, 0, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�N���Q���l��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MemberNum(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const u8 *member_num = pData;
  
	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return;   //�����̃f�[�^�͎󂯎��Ȃ�
  }
  intcomm->member_num = *member_num;
  OS_TPrintf("member_num��M %d\n", *member_num);
}

//==================================================================
/**
 * �f�[�^���M�F�N���Q���l�� (�e�@��p����)
 *
 * @param   intcomm		
 * @param   member_num		�Q���l��
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MemberNum(INTRUDE_COMM_SYS_PTR intcomm, u8 member_num)
{
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }
  
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, 
    INTRUDE_CMD_MEMBER_NUM, sizeof(member_num), &member_num, TRUE, FALSE, FALSE);
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
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

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
  
  if(netID >= intcomm->member_num){
    //�Ǘ����Ă���l�������傫��ID���痈�����͎󂯎��Ȃ��B
    //��ɊǗ��l������M���Ă���łȂ��ƃ}�b�v�̌q���肪�g��ł��Ȃ��B����ĕ\���ʒu���s���ɂȂ�
    OS_TPrintf("PROFILE RECV �Ǘ��l�������傫��ID�ׁ̈A���� net_id=%d, member_num=%d\n", netID, intcomm->member_num);
    return;
  }
  Intrude_SetProfile(intcomm, netID, recv_profile);
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
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_PROFILE, sizeof(INTRUDE_PROFILE), &intcomm->send_profile);
  if(ret == TRUE){
    intcomm->profile_req = FALSE;
    OS_TPrintf("�����v���t�B�[�����M\n");
    if(GFL_NET_IsParentMachine() == TRUE){
      //�e�̏ꍇ�A�v���t�B�[����v������Ă����Ƃ������͓r���Q���҂�����̂�
      //���݂̎Q���l���A�~�b�V���������M����
      intcomm->member_send_req = TRUE;
      MISSION_Set_DataSendReq(&intcomm->mission);
    }
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
 * @brief   �R�}���h��M�F���E�҂̃v���t�B�[�����폜
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_DeleteProfile(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const int *leave_netid = pData;
  
  if(intcomm->recv_profile & (1 << (*leave_netid))){
    intcomm->recv_profile ^= 1 << (*leave_netid);
    CommPlayer_Del(intcomm->cps, *leave_netid);
  }
  OS_TPrintf("Receive:���E�҂̃v���t�B�[���폜 ���E�҂�NetID = %d\n", *leave_netid);
}

//==================================================================
/**
 * �f�[�^���M�F���E�҂̃v���t�B�[�����폜(�e�@��p����)
 * @param   leave_netid   ���E�҂�NetID
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_DeleteProfile(INTRUDE_COMM_SYS_PTR intcomm, int leave_netid)
{
  if(_OtherPlayerExistence() == FALSE){
    _IntrudeRecv_DeleteProfile(GFL_NET_SystemGetCurrentID(), 
      sizeof(leave_netid), &leave_netid, intcomm, GFL_NET_HANDLE_GetCurrentHandle());
    return TRUE;
  }

  GF_ASSERT(GFL_NET_IsParentMachine() == TRUE);
  OS_TPrintf("Send:���E�҂̃v���t�B�[���폜 ���E�҂�NetID = %d\n", leave_netid);
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), INTRUDE_CMD_DELETE_PROFILE, sizeof(leave_netid), &leave_netid);
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
  
	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){ //�����̃f�[�^�͎󂯎��Ȃ�
    //�O���[�X�P�[�����ׁ̈A���ꂾ���Z�b�g
    GameCommStatus_SetPlayerStatus(intcomm->game_comm, netID, recv_data->zone_id,
      recv_data->palace_area);
    return;
  }
  if((intcomm->recv_profile & (1 << netID)) == 0){  //�v���t�B�[������M�͎󂯎��Ȃ�
    return;   
  }
  Intrude_SetPlayerStatus(intcomm, netID, recv_data);
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
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

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
  const INTRUDE_TALK_FIRST_ATTACK *first_attack = pData;
  
  OS_TPrintf("�b��������ꂽ��M�@net_id=%d, talk_type=%d\n", netID, first_attack->talk_type);
  if(Intrude_SetTalkReq(intcomm, netID) == TRUE){
    intcomm->recv_talk_first_attack = *first_attack;
  }
}

//==================================================================
/**
 * �f�[�^���M�F�b��������
 * @param   send_net_id		�b���������NetID
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_Talk(INTRUDE_COMM_SYS_PTR intcomm, int send_net_id)
{
  INTRUDE_TALK_FIRST_ATTACK first_attack;
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }
  
  GFL_STD_MemClear(&first_attack, sizeof(INTRUDE_TALK_FIRST_ATTACK));
  if(MISSION_CheckMissionTargetNetID(&intcomm->mission, send_net_id) == TRUE){
    first_attack.talk_type = INTRUDE_TALK_TYPE_MISSION;
    first_attack.mdata = *(MISSION_GetRecvData(&intcomm->mission));
  }
  else{
    first_attack.talk_type = INTRUDE_TALK_TYPE_NORMAL;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_TALK, sizeof(INTRUDE_TALK_FIRST_ATTACK), &first_attack, FALSE, FALSE, FALSE);
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
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_TALK_ANSWER, sizeof(INTRUDE_TALK_STATUS), &answer, FALSE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�b���������L�����Z��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_TalkCancel(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  
  OS_TPrintf("�b�������L�����Z����M�@net_id=%d\n", netID);
  Intrude_SetTalkCancel(intcomm, netID);
}

//==================================================================
/**
 * �f�[�^���M�F�b���������L�����Z��
 * @param   send_net_id		�b���������NetID
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_TalkCancel(int send_net_id)
{
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_TALK_CANCEL, 0, NULL, FALSE, FALSE, FALSE);
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
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_BINGO_INTRUSION, 0, NULL, FALSE, FALSE, FALSE);
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
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_BINGO_INTRUSION_ANSWER, sizeof(BINGO_INTRUSION_ANSWER), 
    &answer, FALSE, FALSE, FALSE);
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
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_REQ_BINGO_INTRUSION_PARAM, 0, NULL, FALSE, FALSE, FALSE);
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
  
  GFL_STD_MemCopy(inpara, &bingo->intrusion_param, sizeof(BINGO_INTRUSION_PARAM));
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
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    INTRUDE_CMD_BINGO_INTRUSION_PARAM, sizeof(BINGO_INTRUSION_PARAM), &bingo->intrusion_param, 
    FALSE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�~�b�V�����I����⃊�X�g�v��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MissionListReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const u32 *palace_area = pData;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  OS_TPrintf("RECIEVE�F�~�b�V�������X�g�v�� net_id=%d\n", netID);
  MISSION_MissionList_Create(intcomm, &intcomm->mission, netID, *palace_area);
  MISSION_Set_ListSendReq(&intcomm->mission, *palace_area);
}

//==================================================================
/**
 * �f�[�^���M�F�~�b�V�����I����⃊�X�g�v��
 *
 * @param   intcomm         
 * @param   monolith_type		MONOLITH_TYPE_???
 * @param   zone_id		      �~�j���m���X������]�[��ID
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionListReq(INTRUDE_COMM_SYS_PTR intcomm, u32 palace_area)
{
  MISSION_REQ req;
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  OS_TPrintf("SEND:�~�b�V�������X�g�v��\n");
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_LIST_REQ, sizeof(palace_area), &palace_area);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�~�b�V�����I����⃊�X�g
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MissionList(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const MISSION_CHOICE_LIST *mlist = pData;
  
  MISSION_SetMissionList(&intcomm->mission, mlist);
  OS_TPrintf("RECEIVE: �~�b�V�������X�g palace_area = %d\n", mlist->md[0].palace_area);
}

//==================================================================
/**
 * �f�[�^���M�F�~�b�V�����I����⃊�X�g
 *
 * @param   intcomm         
 * @param   mission         �~�b�V�����f�[�^
 * @param   palace_area     �ǂ̃p���X�G���A�̌�⃊�X�g�Ȃ̂�
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionList(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission, int palace_area)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }
  
  ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_LIST, sizeof(MISSION_CHOICE_LIST), &mission->list[palace_area]);
  if(ret == TRUE){
    OS_TPrintf("SEND�F�~�b�V�������X�g palace_area=%d\n", palace_area);
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�~�b�V�����󒍂��܂�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _MissionOrderConfirm(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const MISSION_DATA *mdata = pData;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  OS_TPrintf("��M�F�~�b�V�����󒍂��܂� net_id=%d\n", netID);
  MISSION_SetEntryNew(intcomm, &intcomm->mission, mdata, netID);
}

//==================================================================
/**
 * �f�[�^���M�F�~�b�V�����󒍂��܂�
 *
 * @param   intcomm         
 * @param   mdata           �󒍂��悤�Ƃ��Ă���~�b�V�����f�[�^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionOrderConfirm(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_DATA *mdata)
{
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  MISSION_ClearRecvEntryAnswer(&intcomm->mission);

  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_ORDER_CONFIRM, sizeof(MISSION_DATA), mdata);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�u�~�b�V�����󒍂��܂��v�̕Ԏ�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MissionEntryAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const MISSION_ENTRY_ANSWER *entry_answer = pData;
  
  OS_TPrintf("��M�F�u�~�b�V�����󒍂��܂��v�̌��� net_id=%d\n", netID);
  MISSION_SetRecvEntryAnswer(&intcomm->mission, entry_answer);
}

//==================================================================
/**
 * �f�[�^���M�F�u�~�b�V�����󒍂��܂��v�̕Ԏ�
 *
 * @param   intcomm		
 * @param   entry_answer		
 * @param   send_netid		
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionEntryAnswer(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_ENTRY_ANSWER *entry_answer, int send_netid)
{
  if(_OtherPlayerExistence() == FALSE){
    return TRUE;  //���肪���Ȃ��ꍇ�͑��M�ς݂ɂ��Ă��܂�
  }
  if(GFL_NET_IsConnectMember(send_netid) == FALSE){
    return TRUE;  //���肪���Ȃ��ꍇ�͑��M�ς݂ɂ��Ă��܂�
  }
  
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_netid, 
    INTRUDE_CMD_MISSION_ENTRY_ANSWER, sizeof(MISSION_ENTRY_ANSWER), entry_answer, 
    FALSE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�~�b�V�����v��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MissionReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const MISSION_REQ *req = pData;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  OS_TPrintf("��M�F�~�b�V�����v�� net_id=%d\n", netID);
  MISSION_SetEntry(intcomm, &intcomm->mission, req, netID);
  //�v�����ꂽ����ɂ͍��̃~�b�V������Ԃ��Ă�����B������̓r��������������Ȃ��̂ŁB
  MISSION_Set_DataSendReq(&intcomm->mission);
}

//==================================================================
/**
 * �f�[�^���M�F�~�b�V�����v��
 *
 * @param   intcomm         
 * @param   monolith_type		MONOLITH_TYPE_???
 * @param   zone_id		      �~�j���m���X������]�[��ID
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionReq(INTRUDE_COMM_SYS_PTR intcomm, int monolith_type, u16 zone_id)
{
  MISSION_REQ req;
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  req.monolith_type = monolith_type;
  req.zone_id = zone_id;
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_REQ, sizeof(MISSION_REQ), &req);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�~�b�V�����f�[�^
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MissionData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const MISSION_DATA *mdata = pData;
  
  MISSION_SetMissionData(&intcomm->mission, mdata);
}

//==================================================================
/**
 * �f�[�^���M�F�~�b�V�����f�[�^
 *
 * @param   intcomm         
 * @param   mission         �~�b�V�����f�[�^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionData(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }
  
  ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_DATA, sizeof(MISSION_DATA), &mission->data);
  if(ret == TRUE){
    OS_TPrintf("���M�F�~�b�V�����f�[�^ \n");
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�~�b�V�����B����e�ɓ`����
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MissionAchieve(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const MISSION_DATA *mdata = pData;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  OS_TPrintf("��M�F�~�b�V�����B�� netID=%d\n", netID);
  MISSION_EntryAchieve(&intcomm->mission, mdata, netID);
}

//==================================================================
/**
 * �f�[�^���M�F�~�b�V�����B����e�ɓ`����
 *
 * @param   intcomm         
 * @param   mission         �~�b�V�����f�[�^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionAchieve(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }
  
  MISSION_ClearAchieveAnswer(&intcomm->mission);
  ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_ACHIEVE, sizeof(MISSION_DATA), &mission->data);
  if(ret == TRUE){
    OS_TPrintf("���M�F�~�b�V�����B�� \n");
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�~�b�V�����B���񍐂̕Ԏ�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MissionAchieveAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const MISSION_ACHIEVE *achieve = pData;
  
  OS_TPrintf("��M�F�~�b�V�����B���񍐂̕Ԏ� netID=%d\n", netID);
  MISSION_SetParentAchieve(&intcomm->mission, *achieve);
}

//==================================================================
/**
 * �f�[�^���M�F�~�b�V�����B���񍐂̕Ԏ�
 *
 * @param   intcomm         
 * @param   mission         �~�b�V�����f�[�^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionAchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_ACHIEVE achieve, int send_netid)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return TRUE;  //���肪���Ȃ��ꍇ�͑��M�ς݂ɂ��Ă��܂�
  }
  if(GFL_NET_IsConnectMember(send_netid) == FALSE){
    return TRUE;  //���肪���Ȃ��ꍇ�͑��M�ς݂ɂ��Ă��܂�
  }
  
  ret = GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_netid, 
    INTRUDE_CMD_MISSION_ACHIEVE_ANSWER, sizeof(MISSION_ACHIEVE), &achieve, 
    FALSE, FALSE, FALSE);
  if(ret == TRUE){
    OS_TPrintf("���M�F�~�b�V�����B���񍐂̕Ԏ� achieve=%d, send_netid=%d\n", achieve, send_netid);
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�~�b�V��������
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MissionResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const MISSION_RESULT *mresult = pData;
  
  if((intcomm->recv_profile & (1 << netID)) == 0){
    OS_TPrintf("��M�F�~�b�V�������ʁF�v���t�B�[������M�ׁ̈A�󂯎��Ȃ� recv_profile=%d\n", 
      intcomm->recv_profile);
    return;
  }
  
  OS_TPrintf("��M�F�~�b�V��������\n");
  MISSION_SetResult(&intcomm->mission, mresult);
}

//==================================================================
/**
 * �f�[�^���M�F�~�b�V��������
 *
 * @param   intcomm         
 * @param   mission         �~�b�V�����f�[�^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionResult(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission)
{
  BOOL ret;
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_RESULT, sizeof(MISSION_RESULT), &mission->result);
  if(ret == TRUE){
    OS_TPrintf("���M�F�~�b�V��������\n");
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�苒���
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_OccupyInfo(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const OCCUPY_INFO *cp_occupy = pData;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  OCCUPY_INFO *dest_occupy;
  
  if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){ //�����̃f�[�^�͎󂯎��Ȃ�
    return;
  }
  if((intcomm->recv_profile & (1 << netID)) == 0){
    OS_TPrintf("��M�F�苒���F�v���t�B�[������M�ׁ̈A�󂯎��Ȃ� recv_profile=%d\n", 
      intcomm->recv_profile);
    return;
  }
  
  OS_TPrintf("��M�F�苒��� net_id = %d\n", netID);
  dest_occupy = GAMEDATA_GetOccupyInfo(gamedata, netID);
  GFL_STD_MemCopy(cp_occupy, dest_occupy, sizeof(OCCUPY_INFO));
  
  if(netID == intcomm->intrude_status_mine.palace_area){
    intcomm->area_occupy_update = TRUE;
  }
}

//==================================================================
/**
 * �f�[�^���M�F�苒���
 *
 * @param   intcomm         
 * @param   mission         �~�b�V�����f�[�^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_OccupyInfo(INTRUDE_COMM_SYS_PTR intcomm)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  const OCCUPY_INFO *occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  BOOL ret;
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_OCCUPY_INFO, sizeof(OCCUPY_INFO), occupy);
  if(ret == TRUE){
    OS_TPrintf("���M�F�苒���\n");
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F����w��̃^�C�~���O�R�}���h
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_TargetTiming(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const u8 *timing_no = pData;
  
  if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){ //�����̃f�[�^�͎󂯎��Ȃ�
    return;
  }
  
  OS_TPrintf("��M�F����w��^�C�~���O�R�}���h netID=%d, timing_no=%d\n", netID, *timing_no);
  GF_ASSERT_MSG(intcomm->recv_target_timing_no == 0, 
    "before %d, new %d\n", intcomm->recv_target_timing_no, *timing_no);
  
  intcomm->recv_target_timing_no = *timing_no;
  intcomm->recv_target_timing_netid = netID;
}

//==================================================================
/**
 * �f�[�^���M�F����w��̃^�C�~���O�R�}���h
 *
 * @param   intcomm		
 * @param   timing_no		�^�C�~���O�ԍ�
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_TargetTiming(INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid, u8 timing_no)
{
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_netid, INTRUDE_CMD_TARGET_TIMING, sizeof(timing_no), &timing_no, FALSE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�ʐM������T�|�[�g
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_PlayerSupport(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const SUPPORT_TYPE *support_type = pData;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  COMM_PLAYER_SUPPORT *cps = GAMEDATA_GetCommPlayerSupportPtr(gamedata);
  
  if((intcomm->recv_profile & (1 << netID)) == 0){
    OS_TPrintf("��M�F�v���C���[�T�|�[�g�F�v���t�B�[������M�ׁ̈A�󂯎��Ȃ� netID=%d\n", netID);
    return;
  }
  
  OS_TPrintf("��M�F�v���C���[�T�|�[�g netID=%d, type=%d\n", netID, *support_type);
  COMM_PLAYER_SUPPORT_SetParam(cps, *support_type, GAMEDATA_GetMyStatusPlayer(gamedata, netID));
}

//==================================================================
/**
 * �f�[�^���M�F�ʐM������T�|�[�g
 *
 * @param   intcomm		
 * @param   send_netid    �T�|�[�g����
 * @param   support_type  �T�|�[�g�^�C�v(SUPPORT_TYPE_???)
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_PlayerSupport(INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid, SUPPORT_TYPE support_type)
{
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_netid, INTRUDE_CMD_PLAYER_SUPPORT, sizeof(support_type), &support_type, 
    FALSE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�FWFBC�p�����[�^�v��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_WfbcReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  
  OS_TPrintf("RECIEVE�FWFBC�v�� net_id=%d\n", netID);
  intcomm->wfbc_req |= 1 << netID;
}

//==================================================================
/**
 * �f�[�^���M�FWFBC�p�����[�^�v��
 *
 * @param   intcomm		
 * @param   send_netid		�v�������NetID
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_WfbcReq(INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid)
{
  BOOL ret;
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  OS_TPrintf("SEND:WFBC�v��  send_netid=%d\n", send_netid);
  
  ret = GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_netid, INTRUDE_CMD_WFBC_REQ, 0, NULL, 
    FALSE, FALSE, FALSE);
  if(ret == TRUE){
    intcomm->wfbc_recv = FALSE;
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�FWFBC�p�����[�^
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_Wfbc(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const FIELD_WFBC_CORE *wfbc_core = pData;
  FIELD_WFBC_CORE *dest_wfbc;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  
  dest_wfbc = GAMEDATA_GetWFBCCoreData( gamedata, GAMEDATA_WFBC_ID_COMM );
  GFL_STD_MemCopy(wfbc_core, dest_wfbc, sizeof(FIELD_WFBC_CORE));
  
  GF_ASSERT(intcomm->wfbc_recv == FALSE);
  intcomm->wfbc_recv = TRUE;
  OS_TPrintf("RECEIVE: WFBC�p�����[�^ netID = %d\n", netID);
}

//==================================================================
/**
 * �f�[�^���M�FWFBC�p�����[�^
 *
 * @param   intcomm		
 * @param   send_netid_bit		���M��(Bit�w��)
 * @param   wfbc_core		      WFBC�p�����[�^�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_Wfbc(INTRUDE_COMM_SYS_PTR intcomm, u32 send_netid_bit, const FIELD_WFBC_CORE *wfbc_core)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }
  
  ret = GFL_NET_SendDataExBit(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_netid_bit, INTRUDE_CMD_WFBC, sizeof(FIELD_WFBC_CORE), wfbc_core,
    FALSE, FALSE, FALSE);
  if(ret == TRUE){
    OS_TPrintf("SEND�FWFBC�p�����[�^ send_netid_bit = %d\n", send_netid_bit);
  }
  return ret;
}
