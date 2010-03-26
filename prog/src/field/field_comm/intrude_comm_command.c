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
static void _IntrudeRecv_MissionStart(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionAchieve(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionAchieveAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_MissionResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_OccupyInfo(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_TargetTiming(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_PlayerSupport(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_SecretItem(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_WfbcReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_Wfbc(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_WfbcNpcAns(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_WfbcNpcReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_SymbolDataReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_SymbolData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _IntrudeRecv_SymbolDataChange(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


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
  {_IntrudeRecv_MissionList, _RecvHugeBuffer}, //INTRUDE_CMD_MISSION_LIST
  {_MissionOrderConfirm, NULL},                //INTRUDE_CMD_MISSION_ORDER_CONFIRM
  {_IntrudeRecv_MissionEntryAnswer, NULL},     //INTRUDE_CMD_MISSION_ENTRY_ANSWER
  {_IntrudeRecv_MissionReq, NULL},             //INTRUDE_CMD_MISSION_REQ
  {_IntrudeRecv_MissionData, NULL},            //INTRUDE_CMD_MISSION_DATA
  {_IntrudeRecv_MissionStart, NULL},           //INTRUDE_CMD_MISSION_START
  {_IntrudeRecv_MissionAchieve, NULL},         //INTRUDE_CMD_MISSION_ACHIEVE
  {_IntrudeRecv_MissionAchieveAnswer, NULL},   //INTRUDE_CMD_MISSION_ACHIEVE_ANSWER
  {_IntrudeRecv_MissionResult, NULL},          //INTRUDE_CMD_MISSION_RESULT
  {_IntrudeRecv_OccupyInfo, NULL},             //INTRUDE_CMD_OCCUPY_INFO
  {_IntrudeRecv_TargetTiming, NULL},           //INTRUDE_CMD_TARGET_TIMING
  {_IntrudeRecv_PlayerSupport, NULL},          //INTRUDE_CMD_PLAYER_SUPPORT
  {_IntrudeRecv_SecretItem, NULL},             //INTRUDE_CMD_SECRET_ITEM
  {_IntrudeRecv_WfbcReq, NULL},                //INTRUDE_CMD_WFBC_REQ
  {_IntrudeRecv_Wfbc, NULL},                   //INTRUDE_CMD_WFBC
  {_IntrudeRecv_WfbcNpcAns, NULL},             //INTRUDE_CMD_WFBC_NPC_ANS
  {_IntrudeRecv_WfbcNpcReq, NULL},             //INTRUDE_CMD_WFBC_NPC_REQ
  {_IntrudeRecv_SymbolDataReq, NULL},          //INTRUDE_CMD_SYMBOL_DATA_REQ
  {_IntrudeRecv_SymbolData, NULL},             //INTRUDE_CMD_SYMBOL_DATA
  {_IntrudeRecv_SymbolDataChange, NULL},       //INTRUDE_CMD_SYMBOL_DATA_CHANGE
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
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  GF_ASSERT(netID == 0);  //�e����HugeBuffer�����p�ӂ��Ă��Ȃ����߁A����f�[�^���M�͐e����
  GF_ASSERT_MSG(size <= INTRUDE_HUGE_RECEIVE_BUF_SIZE, "size=%x, recv_size=%x\n", 
    size, INTRUDE_HUGE_RECEIVE_BUF_SIZE);
	return intcomm->huge_receive_buf;
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
BOOL IntrudeSend_Talk(INTRUDE_COMM_SYS_PTR intcomm, int send_net_id, const MISSION_DATA *mdata, INTRUDE_TALK_TYPE intrude_talk_type)
{
  INTRUDE_TALK_FIRST_ATTACK first_attack;
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }
  
  GFL_STD_MemClear(&first_attack, sizeof(INTRUDE_TALK_FIRST_ATTACK));
  first_attack.talk_type = intrude_talk_type;
  first_attack.mdata = *mdata;

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
  
  OS_TPrintf("�b�������̕Ԏ����M answer=%d\n", answer);
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
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  NetID list_netid;   //�~�b�V�������X�g���L�҂�NetID
  
  list_netid = mlist->target_info.net_id;
  MISSION_SetMissionList(&intcomm->mission, mlist, list_netid);

  if(list_netid != GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){//�����̃f�[�^�Ŗ����Ȃ�苒�����Z�b�g
    OCCUPY_INFO *dest_occupy = GAMEDATA_GetOccupyInfo(gamedata, list_netid);
    GFL_STD_MemCopy(&mlist->occupy, dest_occupy, sizeof(OCCUPY_INFO));
  }
  
  OS_TPrintf("RECEIVE: �~�b�V�������X�g list_netid = %d\n", list_netid);
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
  
  ret = GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER,
    INTRUDE_CMD_MISSION_LIST, sizeof(MISSION_CHOICE_LIST), &mission->list[palace_area],
    FALSE, FALSE, TRUE);
  if(ret == TRUE){
    OS_TPrintf("SEND�F�~�b�V�������X�g palace_area=%d, size=%d\n", 
      palace_area, sizeof(MISSION_CHOICE_LIST));
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
  const MISSION_ENTRY_REQ *entry_req = pData;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  OS_TPrintf("��M�F�~�b�V�����󒍂��܂� net_id=%d\n", netID);
  MISSION_SetEntryNew(intcomm, &intcomm->mission, entry_req, netID);
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
BOOL IntrudeSend_MissionOrderConfirm(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_ENTRY_REQ *entry_req)
{
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  MISSION_ClearRecvEntryAnswer(&intcomm->mission);

  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_ORDER_CONFIRM, sizeof(MISSION_ENTRY_REQ), entry_req);
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
  
  OS_TPrintf("Recv�F�u�~�b�V�����󒍂��܂��v�̌��� net_id=%d\n", netID);
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
  
  OS_TPrintf("SEND�F�u�~�b�V�����󒍂��܂��v�̕Ԏ�\n");
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
#if 0 //�폜�\�� 2010.01.29(��) 
  MISSION_REQ req;
  
  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }

  req.monolith_type = monolith_type;
  req.zone_id = zone_id;
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_REQ, sizeof(MISSION_REQ), &req);
#else
  GF_ASSERT(0);
  return FALSE;
#endif
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
  BOOL new_mission;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  ZONEID zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(gamedata));
  
#if 0 //�G���g���[���ɂ������߁A�\�ł��󂯎��悤�ɂ����@2010.02.02(��)
  if(ZONEDATA_IsPalaceField(zone_id) == FALSE && ZONEDATA_IsPalace(zone_id) == FALSE){
    OS_TPrintf("�~�b�V������M�F�\�t�B�[���h�ɂ��邽�߁A�󂯎��Ȃ�\n");
    return;
  }
#endif

  new_mission = MISSION_SetMissionData(intcomm, &intcomm->mission, mdata);
  if(new_mission == TRUE){  //�A����M�ŏ㏑������Ȃ��悤�ɒ��ڑ���͂��Ȃ�
    intcomm->new_mission_recv = TRUE;
    GameCommInfo_MessageEntry_Mission(intcomm->game_comm, mdata->accept_netid);
  }
  
  OS_TPrintf("�~�b�V������M�Fnew_mission_recv = %d\n", new_mission);
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
 * @brief   �R�}���h��M�F�~�b�V�����J�n
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_MissionStart(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const MISSION_DATA *mdata = pData;
  const MISSION_DATA *my_recv_data = MISSION_GetRecvData(&intcomm->mission);
  
  if(my_recv_data == NULL){
    OS_TPrintf("RECV:�~�b�V�����J�n�F�~�b�V��������M�̈ז���\n");
    return;
  }
  if(GFL_STD_MemComp(my_recv_data, mdata, sizeof(MISSION_DATA)) != 0){
    OS_TPrintf("RECV:�~�b�V�����J�n�F�������Ă���~�b�V�����f�[�^�ƈႤ�̂Ŗ���\n");
    return;
  }
  
  MISSION_RecvMissionStart(&intcomm->mission);
  OS_TPrintf("RECV:�~�b�V�����J�n\n");
}

//==================================================================
/**
 * �f�[�^���M�F�~�b�V�����J�n
 *
 * @param   intcomm         
 * @param   mission         �~�b�V�����f�[�^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_MissionStart(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }
  
  ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_MISSION_START, sizeof(MISSION_DATA), &mission->data);
  if(ret == TRUE){
    OS_TPrintf("���M�F�~�b�V�����J�n \n");
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
    MISSION_SetMissionComplete(&intcomm->mission);
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
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  OCCUPY_INFO *occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  
  if((intcomm->recv_profile & (1 << netID)) == 0){
    OS_TPrintf("��M�F�~�b�V�������ʁF�v���t�B�[������M�ׁ̈A�󂯎��Ȃ� recv_profile=%d\n", 
      intcomm->recv_profile);
    return;
  }
  
  //�������~�b�V�����̃^�[�Q�b�g������
  if(mresult->mission_data.target_info.net_id 
      == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    if(mresult->mission_fail == TRUE){  //�~�b�V�������s�̏ꍇ�̓��X�g����蒼��
      MISSION_LIST_Create_Type(occupy, mresult->mission_data.cdata.type);
      intcomm->send_occupy = TRUE;
    }
    else{ //�B���҂�����̂ŁA�^�[�Q�b�g�����������̐苒�����N���A�ς݂ɂ���
      MISSION_SetMissionClear(gamedata, mresult);
      intcomm->send_occupy = TRUE;
    }
  }

  //�������B���҂̏ꍇ�̓��x���A�b�v
  if(MISSION_GetResultPoint(intcomm, &intcomm->mission) > 0){
    if(mresult->mission_data.monolith_type == MONOLITH_TYPE_BLACK){
      OccupyInfo_LevelUpBlack(occupy, MISSION_ACHIEVE_ADD_LEVEL);
    }
    else{
      OccupyInfo_LevelUpWhite(occupy, MISSION_ACHIEVE_ADD_LEVEL);
    }
    intcomm->send_occupy = TRUE;
  }

  //�S�苒�B���Ȃ�΃~�b�V�������X�g���쐬���Ȃ���
  if(MISSION_LIST_Create_Complete(occupy) == TRUE){
    intcomm->send_occupy = TRUE;
  }
  
  if(MISSION_RecvCheck(&intcomm->mission) == FALSE 
      || MISSION_GetMissionEntry(&intcomm->mission) == FALSE){
    OS_TPrintf("��M�F�~�b�V�������ʁF�~�b�V�����ɎQ�����Ă��Ȃ��ׁA���ʂ��󂯎��Ȃ�\n");
    return;
  }

  OS_TPrintf("��M�F�~�b�V��������\n");
  MISSION_SetResult(intcomm, &intcomm->mission, mresult);
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
  {
    FIELD_STATUS *fldstatus = GAMEDATA_GetFieldStatus(gamedata);
    if(FIELD_STATUS_GetProcAction( fldstatus ) == PROC_ACTION_BATTLE){
      SUPPORT_TYPE now_support = COMM_PLAYER_SUPPORT_GetSupportType(cps);
      if(now_support == SUPPORT_TYPE_NULL || now_support == SUPPORT_TYPE_USED){
        COMM_PLAYER_SUPPORT_SetParam(
          cps, *support_type, GAMEDATA_GetMyStatusPlayer(gamedata, netID));
      }
    }
  }
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
 * @brief   �R�}���h��M�F�B���A�C�e��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_SecretItem(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const INTRUDE_SECRET_ITEM_SAVE *itemdata = pData;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  
  if((intcomm->recv_profile & (1 << netID)) == 0){
    OS_TPrintf("��M�F�B���A�C�e���F�v���t�B�[������M�ׁ̈A�󂯎��Ȃ� netID=%d\n", netID);
    return;
  }
  
  OS_TPrintf("��M�F�B���A�C�e�� netID=%d\n", netID);
  {
    INTRUDE_SAVE_WORK *intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork(gamedata) );
    ISC_SAVE_SetItem(intsave, itemdata);
  }
}

//==================================================================
/**
 * �f�[�^���M�F�B���A�C�e��
 *
 * @param   send_netid    �T�|�[�g����
 * @param   support_type  �T�|�[�g�^�C�v(SUPPORT_TYPE_???)
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_SecretItem(NetID send_netid, const INTRUDE_SECRET_ITEM_SAVE *itemdata)
{
  if(_OtherPlayerExistence() == FALSE){
    return TRUE;
  }

  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_netid, INTRUDE_CMD_SECRET_ITEM, sizeof(INTRUDE_SECRET_ITEM_SAVE), itemdata, 
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
  GAMEDATA_SetUpPalaceWFBCCoreData( gamedata, wfbc_core );
  
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

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�FWFBC�p�����[�^�FFIELD_WFBC_COMM_NPC_ANS
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_WfbcNpcAns(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const FIELD_WFBC_COMM_NPC_ANS *npc_ans = pData;
  
  FIELD_WFBC_COMM_DATA_SetRecvCommAnsData(&intcomm->wfbc_comm_data, npc_ans);
  OS_TPrintf("RECEIVE: WFBC�p�����[�^NPC Ans netID = %d\n", netID);
}

//==================================================================
/**
 * �f�[�^���M�FWFBC�p�����[�^�FFIELD_WFBC_COMM_NPC_ANS
 *
 * @param   intcomm		
 * @param   send_netid		    ���M��
 * @param   wfbc_core		      WFBC�p�����[�^�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_WfbcNpcAns(const FIELD_WFBC_COMM_NPC_ANS *npc_ans, NetID send_netid)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return TRUE;
  }
  
  ret = GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_netid, INTRUDE_CMD_WFBC_NPC_ANS, sizeof(FIELD_WFBC_COMM_NPC_ANS), npc_ans, 
    FALSE, FALSE, FALSE);
  if(ret == TRUE){
    OS_TPrintf("SEND�FWFBC�p�����[�^:NPC_ANS send_netid = %d\n", send_netid);
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�FWFBC�p�����[�^�FFIELD_WFBC_COMM_NPC_REQ
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_WfbcNpcReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const FIELD_WFBC_COMM_NPC_REQ *npc_req = pData;
  
  FIELD_WFBC_COMM_DATA_SetRecvCommReqData(&intcomm->wfbc_comm_data, netID, npc_req);
  OS_TPrintf("RECEIVE: WFBC�p�����[�^NPC Req netID = %d\n", netID);
}

//==================================================================
/**
 * �f�[�^���M�FWFBC�p�����[�^�FFIELD_WFBC_COMM_NPC_REQ
 *
 * @param   intcomm		
 * @param   send_netid    		���M��
 * @param   wfbc_core		      WFBC�p�����[�^�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_WfbcNpcReq(const FIELD_WFBC_COMM_NPC_REQ *npc_req, NetID send_netid)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return TRUE;
  }
  
  ret = GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_netid, INTRUDE_CMD_WFBC_NPC_REQ, sizeof(FIELD_WFBC_COMM_NPC_REQ), npc_req, 
    FALSE, FALSE, FALSE);
  if(ret == TRUE){
    OS_TPrintf("SEND�FWFBC�p�����[�^:NPC_REQ\n");
  }
  return ret;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�V���{���G���J�E���g�f�[�^��v��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_SymbolDataReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const SYMBOL_DATA_REQ *p_sdr = pData;
  
  intcomm->req_symbol_data[netID] = *p_sdr;
  MATSUDA_Printf("RECV: symbol_req netID = %d\n", netID);
}

//==================================================================
/**
 * �f�[�^���M�F�V���{���G���J�E���g�f�[�^��v��
 *
 * @param   intcomm		
 * @param   send_netid    		���M��
 * @param   wfbc_core		      WFBC�p�����[�^�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_SymbolDataReq(INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid, const SYMBOL_DATA_REQ *p_sdr)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return FALSE;
  }
  
  MATSUDA_Printf("SEND: symbol_req\n");
  
  intcomm->recv_symbol_flag = FALSE;
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_netid, INTRUDE_CMD_SYMBOL_DATA_REQ, sizeof(SYMBOL_DATA_REQ), p_sdr, 
    FALSE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�V���{���G���J�E���g�f�[�^
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_SymbolData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const INTRUDE_SYMBOL_WORK *p_symboldata = pData;
  
  intcomm->intrude_symbol = *p_symboldata;
  intcomm->recv_symbol_flag = TRUE;
  intcomm->recv_symbol_change_flag = FALSE;
  MATSUDA_Printf("RECV: symbol_data netID = %d\n", netID);
}

//==================================================================
/**
 * �f�[�^���M�F�V���{���G���J�E���g�f�[�^
 *
 * @param   intcomm		
 * @param   send_netid_bit		���M��
 * @param   wfbc_core		      WFBC�p�����[�^�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_SymbolData(INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid)
{
  if(_OtherPlayerExistence() == FALSE){
    return TRUE;
  }
  
  MATSUDA_Printf("SEND: symbol_data net_id\n", send_netid);
  
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_netid, INTRUDE_CMD_SYMBOL_DATA, sizeof(INTRUDE_SYMBOL_WORK), 
    &intcomm->intrude_send_symbol, FALSE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�����̃V���{���G���J�E���g�f�[�^���ύX��������m�点��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _IntrudeRecv_SymbolDataChange(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  INTRUDE_COMM_SYS_PTR intcomm = pWork;
  const SYMBOL_DATA_CHANGE *p_sdc = pData;
  const INTRUDE_SYMBOL_WORK *now_symbol = &intcomm->intrude_symbol;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  
  if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){ //�����̃f�[�^�͎󂯎��Ȃ�
    return;
  }

  MATSUDA_Printf("RECV: symbol_change net_id=%d ", netID);
  if(intcomm->intrude_status_mine.palace_area != netID){
//  if(now_symbol->net_id != netID){// || now_symbol->symbol_map_id != p_sdc->symbol_map_id){
    MATSUDA_Printf("�N����ƈႤNetID�̈ז���\n");
    return;
  }
  if(ZONEDATA_IsBingo( PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(gamedata)) ) == FALSE){
    MATSUDA_Printf("�r���S�}�b�v�ɂ��Ȃ��̂Ŗ���\n");
    return;
  }
  MATSUDA_Printf("\n");
  intcomm->recv_symbol_change_flag = TRUE;
}

//==================================================================
/**
 * �f�[�^���M�F�����̃V���{���G���J�E���g�f�[�^���ύX��������m�点��
 *
 * @param   intcomm		
 * @param   send_netid_bit		���M��(Bit�w��)
 * @param   wfbc_core		      WFBC�p�����[�^�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL IntrudeSend_SymbolDataChange(const SYMBOL_DATA_CHANGE *p_sdc)
{
  BOOL ret;

  if(_OtherPlayerExistence() == FALSE){
    return TRUE;
  }
  
  MATSUDA_Printf("SEND: symbol_change\n");
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    INTRUDE_CMD_SYMBOL_DATA_CHANGE, sizeof(SYMBOL_DATA_CHANGE), p_sdc);
}
