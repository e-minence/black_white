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


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size);
static void _ColosseumRecv_Shutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_MainMenuListResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_MainMenuListResultAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _ColosseumRecv_TrainerCardParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


//==============================================================================
//  �f�[�^
//==============================================================================
///���j�I����M�R�}���h�e�[�u��   ��COLOSSEUM_CMD_???�ƕ��т𓯂��ɂ��Ă������ƁI�I
const NetRecvFuncTable Colosseum_CommPacketTbl[] = {
  {_ColosseumRecv_Shutdown, NULL},                //COLOSSEUM_CMD_SHUTDOWN
  {_ColosseumRecv_MainMenuListResult, NULL},      //COLOSSEUM_CMD_MAINMENU_LIST_RESULT
  {_ColosseumRecv_MainMenuListResultAnswer, NULL},      //COLOSSEUM_CMD_MAINMENU_LIST_RESULT_ANSWER
  {_ColosseumRecv_TrainerCardParam, _RecvHugeBuffer},        //COLOSSEUM_CMD_TRAINERCARD_PARAM
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
static void _ColosseumRecv_MainMenuListResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
#if 0
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const u32 *select = pData;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //�����̃f�[�^�Ȃ̂Ŗ���
  }
  
  situ->mycomm.mainmenu_select = *select;
#endif
}

//==================================================================
/**
 * �f�[�^���M�F���C�����j���[�̑I������
 * @param   select_list		�I������(UNION_MSG_MENU_SELECT_???)
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_MainMenuListResult(u32 select_list)
{
  return 0;
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
static void _ColosseumRecv_MainMenuListResultAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
#if 0
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const BOOL *yes_no = pData;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //�����̃f�[�^�Ȃ̂Ŗ���
  }
  
  situ->mycomm.mainmenu_yesno_result = *yes_no;
#endif
}

//==================================================================
/**
 * �f�[�^���M�F���C�����j���[�̑I�����ʂ̕Ԏ�
 * @param   yes_no    TRUE:�͂��@FALSE:������
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_MainMenuListResultAnswer(BOOL yes_no)
{
  return 0;
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
static void _ColosseumRecv_TrainerCardParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
#if 0
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //�����̃f�[�^�Ȃ̂Ŗ���
  }
  
  OS_TPrintf("COmmCOmmand �J�[�h��M netID = %d\n", netID);
  situ->mycomm.trcard.target_card_receive = TRUE;
  GFL_STD_MemCopy(pData, situ->mycomm.trcard.target_card, size);
#endif
}

//==================================================================
/**
 * �f�[�^���M�F�g���[�i�[�J�[�h���
 * @param   yes_no    TRUE:�͂��@FALSE:������
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL ColosseumSend_TrainerCardParam(UNION_SYSTEM_PTR unisys)
{
  return 0;
}
