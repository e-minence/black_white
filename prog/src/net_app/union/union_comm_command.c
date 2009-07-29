//==============================================================================
/**
 * @file    union_comm_command.c
 * @brief   ���j�I�����[���F�ʐM�R�}���h
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
#include "union_comm_command.h"
#include "union_types.h"
#include "union_local.h"
#include "colosseum.h"


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size);
static void _UnionRecv_Shutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MainMenuListResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MainMenuListResultAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_TrainerCardParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_ColosseumEntryStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_ColosseumEntryAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_ColosseumEntryAllReady(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


//==============================================================================
//  �f�[�^
//==============================================================================
///���j�I����M�R�}���h�e�[�u��   ��UNION_CMD_???�ƕ��т𓯂��ɂ��Ă������ƁI�I
const NetRecvFuncTable Union_CommPacketTbl[] = {
  {_UnionRecv_Shutdown, NULL},                //UNION_CMD_SHUTDOWN
  {_UnionRecv_MainMenuListResult, NULL},      //UNION_CMD_MAINMENU_LIST_RESULT
  {_UnionRecv_MainMenuListResultAnswer, NULL},      //UNION_CMD_MAINMENU_LIST_RESULT_ANSWER
  {_UnionRecv_TrainerCardParam, _RecvHugeBuffer},        //UNION_CMD_TRAINERCARD_PARAM
  {_UnionRecv_ColosseumEntryStatus, _RecvHugeBuffer},   //UNION_CMD_COLOSSEUM_ENTRY
  {_UnionRecv_ColosseumEntryAnswer, _RecvHugeBuffer},   //UNION_CMD_COLOSSEUM_ENTRY_ANSWER
  {_UnionRecv_ColosseumEntryAllReady, NULL},            //UNION_CMD_COLOSSEUM_ENTRY_ALL_READY
};
SDK_COMPILER_ASSERT(NELEMS(Union_CommPacketTbl) == UNION_CMD_NUM);



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
  UNION_SYSTEM_PTR unisys = pWork;
  GF_ASSERT_MSG(size <= UNION_HUGE_RECEIVE_BUF_SIZE, "size=%x, recv_size=%x\n", size, UNION_HUGE_RECEIVE_BUF_SIZE);
	return unisys->huge_receive_buf[netID];
}

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
static void _UnionRecv_Shutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F���C�����j���[�̑I������
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MainMenuListResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const u32 *select = pData;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //�����̃f�[�^�Ȃ̂Ŗ���
  }
  
  situ->mycomm.mainmenu_select = *select;
}

//==================================================================
/**
 * �f�[�^���M�F���C�����j���[�̑I������
 * @param   select_list		�I������(UNION_MSG_MENU_SELECT_???)
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL UnionSend_MainMenuListResult(u32 select_list)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), UNION_CMD_MAINMENU_LIST_RESULT, sizeof(u32), &select_list);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F���C�����j���[�̑I�����ʂ̕Ԏ�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MainMenuListResultAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const BOOL *yes_no = pData;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //�����̃f�[�^�Ȃ̂Ŗ���
  }
  
  situ->mycomm.mainmenu_yesno_result = *yes_no;
}

//==================================================================
/**
 * �f�[�^���M�F���C�����j���[�̑I�����ʂ̕Ԏ�
 * @param   yes_no    TRUE:�͂��@FALSE:������
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL UnionSend_MainMenuListResultAnswer(BOOL yes_no)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), UNION_CMD_MAINMENU_LIST_RESULT_ANSWER, sizeof(BOOL), &yes_no);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�g���[�i�[�J�[�h���
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_TrainerCardParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //�����̃f�[�^�Ȃ̂Ŗ���
  }
  
  OS_TPrintf("COmmCOmmand �J�[�h��M netID = %d\n", netID);
  situ->mycomm.trcard.target_card_receive = TRUE;
  GFL_STD_MemCopy(pData, situ->mycomm.trcard.target_card, size);
}

//==================================================================
/**
 * �f�[�^���M�F�g���[�i�[�J�[�h���
 * @param   yes_no    TRUE:�͂��@FALSE:������
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL UnionSend_TrainerCardParam(UNION_SYSTEM_PTR unisys)
{
  GF_ASSERT(unisys->my_situation.mycomm.trcard.my_card != NULL);
  
  OS_TPrintf("TR_CARD_DATA = %x\n", sizeof(TR_CARD_DATA));
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, 
    UNION_CMD_TRAINERCARD_PARAM, sizeof(TR_CARD_DATA), 
    unisys->my_situation.mycomm.trcard.my_card, TRUE, FALSE, TRUE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�R���V�A���F�G���g���[���
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_ColosseumEntryStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  const COLOSSEUM_BASIC_STATUS *p_basic = pData;
  
  if(clsys == NULL || clsys->cps == NULL || clsys->entry_menu == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  OS_TPrintf("�R���V�A���F�G���g���[����M�Fnet_id = %d\n", netID);
  CommEntryMenu_Entry(clsys->entry_menu, netID, p_basic->name, p_basic->id, p_basic->sex, p_basic->force_entry);
}

//==================================================================
/**
 * �f�[�^���M�F�R���V�A���F�G���g���[���
 * @param   select_list		�I������(UNION_MSG_MENU_SELECT_???)
 * @param   parent_only   TRUE:�e�݂̂ɑ��M�B�@FALSE:�S���ɑ��M
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL UnionSend_ColosseumEntryStatus(COLOSSEUM_BASIC_STATUS *basic_status)
{
  OS_TPrintf("�R���V�A���F�G���g���[��񑗐M\n");
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_NO_PARENTMACHINE, 
    UNION_CMD_COLOSSEUM_ENTRY, sizeof(COLOSSEUM_BASIC_STATUS), 
    basic_status, TRUE, FALSE, TRUE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�R���V�A���F�G���g���[����
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_ColosseumEntryAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  const COMM_ENTRY_ANSWER *answer = pData;
  
  if(clsys == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  OS_TPrintf("�R���V�A���F�G���g���[���ʎ�M�Fanswer = %d\n", *answer);
  clsys->mine.entry_answer = *answer;
}

//==================================================================
/**
 * �f�[�^���M�F�R���V�A���F�G���g���[����
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL UnionSend_ColosseumEntryAnswer(int send_netid, COMM_ENTRY_ANSWER answer)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_netid, UNION_CMD_COLOSSEUM_ENTRY_ANSWER, sizeof(COMM_ENTRY_ANSWER), &answer, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�R���V�A���F�G���g���[�S��������
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_ColosseumEntryAllReady(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  const COMM_ENTRY_ANSWER *answer = pData;
  
  if(clsys == NULL){
    GF_ASSERT(0);
    return; //�������o���Ă��Ȃ��̂Ŏ󂯎��Ȃ�
  }
  
  OS_TPrintf("�R���V�A���F�G���g���[�S����������M\n");
  clsys->entry_all_ready = TRUE;
}

//==================================================================
/**
 * �f�[�^���M�F�R���V�A���F�G���g���[�S��������
 * @param   
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL UnionSend_ColosseumEntryAllReady(void)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), UNION_CMD_COLOSSEUM_ENTRY_ALL_READY, 0, NULL);
}

