//==============================================================================
/**
 * @file    union_comm_command.c
 * @brief   �R���V�A���F�ʐM�R�}���h
 * @author  matsuda
 * @date    2009.07.14(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "union_types.h"
#include "union_local.h"
#include "savedata/mystatus.h"
#include "net_app/union/union_main.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "colosseum_comm_command.h"
#include "colosseum.h"


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size);
static void _ColosseumRecv_Shutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_BasicStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_TrainerCard(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_PosPackage(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_StandingPositionConfirm(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_AnswerStandingPosition(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_Pokeparty(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_StandingPos(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_Leave(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


//==============================================================================
//  �f�[�^
//==============================================================================
///���j�I����M�R�}���h�e�[�u��   ��COLOSSEUM_CMD_???�ƕ��т𓯂��ɂ��Ă������ƁI�I
const NetRecvFuncTable Colosseum_CommPacketTbl[] = {
  {_ColosseumRecv_Shutdown, NULL},                  //COLOSSEUM_CMD_SHUTDOWN
  {_ColosseumRecv_BasicStatus, _RecvHugeBuffer},    //COLOSSEUM_CMD_BASIC_STATUS
  {_ColosseumRecv_TrainerCard, _RecvHugeBuffer},    //COLOSSEUM_CMD_TRAINERCARD
  {_ColosseumRecv_PosPackage, _RecvHugeBuffer},     //COLOSSEUM_CMD_POS_PACKAGE
  {_ColosseumRecv_StandingPositionConfirm, NULL},   //COLOSSEUM_CMD_STANDPOS_CONFIRM
  {_ColosseumRecv_AnswerStandingPosition, NULL},    //COLOSSEUM_CMD_ANSWER_STANDPOS
  {_ColosseumRecv_Pokeparty, _RecvHugeBuffer},      //COLOSSEUM_CMD_POKEPARTY
  {_ColosseumRecv_StandingPos, NULL},               //COLOSSEUM_CMD_STANDING_POS
  {_ColosseumRecv_Leave, NULL},                     //COLOSSEUM_CMD_LEAVE
};
SDK_COMPILER_ASSERT(NELEMS(Colosseum_CommPacketTbl) == COLOSSEUM_CMD_NUM);


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �R���V�A���p�̒ʐM�R�}���h�e�[�u����ǉ�����
 *
 * @param   unisys		
 */
//==================================================================
void Colosseum_AddCommandTable(UNION_SYSTEM_PTR unisys)
{
  GFL_NET_AddCommandTable(
    GFL_NET_CMD_COLOSSEUM, Colosseum_CommPacketTbl, NELEMS(Colosseum_CommPacketTbl), unisys);
}

//==================================================================
/**
 * �R���V�A���p�̒ʐM�R�}���h�e�[�u�����폜����
 */
//==================================================================
void Colosseum_DelCommandTable(void)
{
  GFL_NET_DelCommandTable(GFL_NET_CMD_COLOSSEUM);
}


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
  UNION_SYSTEM_PTR unisys = pWork;
  GF_ASSERT_MSG(size <= UNION_HUGE_RECEIVE_BUF_SIZE, "size=%x, recv_size=%x\n", size, UNION_HUGE_RECEIVE_BUF_SIZE);
	return unisys->huge_receive_buf[netID];
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
static void _ColosseumRecv_Shutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  ;
}

//==================================================================
/**
 * �f�[�^���M�F�ؒf
 * @param   select_list		�I������(UNION_MSG_MENU_SELECT_???)
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_Shutdown(COMM_PLAYER_PACKAGE *pos_package)
{
  GF_ASSERT(0); //���쐬
  return 0;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F��{���
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _ColosseumRecv_BasicStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //�����̃f�[�^�Ȃ̂Ŗ���
  }
  if(clsys == NULL || clsys->cps == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  OS_TPrintf("�R���V�A���F��{����M�Fnet_id = %d\n", netID);
  GFL_STD_MemCopy(pData, &clsys->basic_status[netID], size);
}

//==================================================================
/**
 * �f�[�^���M�F���W�p�b�P�[�W
 * @param   select_list		�I������(UNION_MSG_MENU_SELECT_???)
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_BasicStatus(COLOSSEUM_BASIC_STATUS *basic_status)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, 
    COLOSSEUM_CMD_BASIC_STATUS, sizeof(COLOSSEUM_BASIC_STATUS), 
    basic_status, TRUE, FALSE, TRUE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�g���[�i�[�J�[�h
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _ColosseumRecv_TrainerCard(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  COMM_PLAYER_SYS_PTR cps;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //�����̃f�[�^�Ȃ̂Ŗ���
  }
  if(clsys == NULL || clsys->cps == NULL || clsys->recvbuf.tr_card[netID] == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  OS_TPrintf("�R���V�A���F�g���[�i�[�J�[�h��M net_id = %d\n", netID);
  GFL_STD_MemCopy(pData, clsys->recvbuf.tr_card[netID], size);
  clsys->recvbuf.tr_card_occ[netID] = TRUE;
}

//==================================================================
/**
 * �f�[�^���M�F���W�p�b�P�[�W
 * @param   select_list		�I������(UNION_MSG_MENU_SELECT_???)
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_TrainerCard(TR_CARD_DATA *send_card)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, 
    COLOSSEUM_CMD_TRAINERCARD, sizeof(TR_CARD_DATA), send_card, TRUE, FALSE, TRUE);
}


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F���W�p�b�P�[�W
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _ColosseumRecv_PosPackage(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //�����̃f�[�^�Ȃ̂Ŗ���
  }
  if(clsys == NULL || clsys->cps == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  Colosseum_SetCommPlayerPos(clsys, netID, pData);
}

//==================================================================
/**
 * �f�[�^���M�F���W�p�b�P�[�W
 * @param   select_list		�I������(UNION_MSG_MENU_SELECT_???)
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_PosPackage(COMM_PLAYER_PACKAGE *pos_package)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, 
    COLOSSEUM_CMD_POS_PACKAGE, sizeof(COMM_PLAYER_PACKAGE), 
    pos_package, FALSE, TRUE, TRUE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�e�Ɏ����̍��̗����ʒu���g���Ă����Ȃ����m�F����
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _ColosseumRecv_StandingPositionConfirm(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  COMM_PLAYER_SYS_PTR cps;
  const u8 *stand_pos = pData;

  if(clsys == NULL || clsys->cps == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  cps = clsys->cps;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return; //�e�@�ȊO�ɂ͑��M���Ă��Ȃ��͂������ǂ����ł��ꉞ�`�F�b�N
  }
  
  OS_TPrintf("��M�F�����ʒu�m�F net_id=%d, stand_pos=%d\n", netID, *stand_pos);
  Colosseum_Parent_SetStandingPosition(clsys, netID, *stand_pos);
}

//==================================================================
/**
 * �f�[�^���M�F�e�Ɏ����̍��̗����ʒu���g���Ă����Ȃ����m�F����
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_StandingPositionConfirm(COLOSSEUM_SYSTEM_PTR clsys)
{
  u8 stand_pos;
  
  stand_pos = Colosseum_Mine_GetStandingPostion(clsys);

  //�e�@�ɂ������M
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_NO_PARENTMACHINE, 
    COLOSSEUM_CMD_STANDPOS_CONFIRM, sizeof(stand_pos), 
    &stand_pos, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�����ʒu�g�p���ʂ̕Ԏ�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _ColosseumRecv_AnswerStandingPosition(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  COMM_PLAYER_SYS_PTR cps;
  const BOOL *result = pData;

  if(clsys == NULL || clsys->cps == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  cps = clsys->cps;
  
  OS_TPrintf("��M�F�����ʒu�̕Ԏ� result=%d\n", *result);
  Colosseum_Mine_SetAnswerStandingPosition(clsys, *result);
}

//==================================================================
/**
 * �f�[�^���M�F�����ʒu�g�p���ʂ̕Ԏ��𑗐M
 *
 * @param   clsys		
 * @param   send_net_id		���M���netID
 * @param   result		    ����
 *
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_AnswerStandingPosition(COLOSSEUM_SYSTEM_PTR clsys, int send_net_id, BOOL result)
{
  u8 stand_pos;
  
  stand_pos = Colosseum_Mine_GetStandingPostion(clsys);

  //�Ώێ҂ɂ������M
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_net_id, 
    COLOSSEUM_CMD_ANSWER_STANDPOS, sizeof(BOOL), &result, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�FPOKEPARTY
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _ColosseumRecv_Pokeparty(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;

  if(clsys == NULL || clsys->recvbuf.pokeparty[netID] == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  OS_TPrintf("�R���V�A���FPOKEPARTY��M�Fnet_id = %d\n", netID);
  GFL_STD_MemCopy(pData, clsys->recvbuf.pokeparty[netID], size);
  clsys->recvbuf.pokeparty_occ[netID] = TRUE;
}

//==================================================================
/**
 * �f�[�^���M�FPOKEPARTY
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_Pokeparty(POKEPARTY *pokeparty)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, 
    COLOSSEUM_CMD_POKEPARTY, sizeof(PokeParty_GetWorkSize()), 
    pokeparty, TRUE, FALSE, TRUE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�FPOKEPARTY
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _ColosseumRecv_StandingPos(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;

  if(clsys == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  OS_TPrintf("�R���V�A���F�����ʒu��M�Fnet_id = %d\n", netID);
  GFL_STD_MemCopy(pData, clsys->recvbuf.stand_position, size);
  clsys->recvbuf.stand_position_occ = TRUE;
}

//==================================================================
/**
 * �f�[�^���M�FPOKEPARTY
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_StandingPos(u8 *standing_pos)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    COLOSSEUM_CMD_STANDING_POS, sizeof(u8) * COLOSSEUM_MEMBER_MAX, standing_pos);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�ޏo
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _ColosseumRecv_Leave(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;

  if(clsys == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  OS_TPrintf("�R���V�A���F�ޏo��M�Fnet_id = %d\n", netID);
  clsys->recvbuf.leave[netID] = TRUE;
}

//==================================================================
/**
 * �f�[�^���M�F�ޏo
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_Leave(void)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), COLOSSEUM_CMD_LEAVE, 0, NULL);
}

