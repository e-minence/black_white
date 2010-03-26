//==============================================================================
/**
 * @file    comm_entry_menu_comm_command.c
 * @brief   �Q����W���j���[����F�ʐM�e�[�u��
 * @author  matsuda
 * @date    2010.01.11(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "savedata/mystatus.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "net_app/comm_entry_menu.h"
#include "comm_entry_menu_comm_command.h"
#include "savedata/mystatus_local.h"



//==============================================================================
//  �\���̒�`
//==============================================================================
///�e�փG���g���[�ʒm����Ƃ��̑��M�f�[�^
typedef struct{
  MYSTATUS mystatus;
  u8 mac_address[6];
  u8 force_entry;
  u8 padding;
}CEM_SEND_ENTRY;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static u8 * _RecvMemberInfo(int netID, void* pWork, int size);
static void _CemRecv_Entry(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _CemRecv_EntryOK(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _CemRecv_EntryNG(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _CemRecv_GameStart(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _CemRecv_GameCancel(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _CemRecv_MemberInfo(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


//==============================================================================
//  �f�[�^
//==============================================================================
///���j�I����M�R�}���h�e�[�u��   ��ENTRYMENU_CMD_???�ƕ��т𓯂��ɂ��Ă������ƁI�I
const NetRecvFuncTable CommEntryMenu_CommPacketTbl[] = {
  {_CemRecv_Entry, NULL},               //ENTRYMENU_CMD_ENTRY
  {_CemRecv_EntryOK, NULL},             //ENTRYMENU_CMD_ENTRY_OK
  {_CemRecv_EntryNG, NULL},             //ENTRYMENU_CMD_ENTRY_NG
  {_CemRecv_GameStart, NULL},           //ENTRYMENU_CMD_GAME_START
  {_CemRecv_GameCancel, NULL},          //ENTRYMENU_CMD_GAME_CANCEL
  {_CemRecv_MemberInfo, _RecvMemberInfo}, //ENTRYMENU_CMD_MEMBER_INFO
};
SDK_COMPILER_ASSERT(NELEMS(CommEntryMenu_CommPacketTbl) == ENTRYMENU_CMD_NUM);



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �Q����W���j���[�p�̒ʐM�R�}���h�e�[�u����ǉ�����
 *
 * @param   em		
 */
//==================================================================
void CommEntryMenu_AddCommandTable(COMM_ENTRY_MENU_PTR em)
{
  GFL_NET_AddCommandTable(GFL_NET_CMD_COMM_ENTRY_MENU, CommEntryMenu_CommPacketTbl, 
    NELEMS(CommEntryMenu_CommPacketTbl), em);
}

//==================================================================
/**
 * �Q����W���j���[�p�̒ʐM�R�}���h�e�[�u�����폜����
 */
//==================================================================
void CommEntryMenu_DelCommandTable(void)
{
  GFL_NET_DelCommandTable(GFL_NET_CMD_COMM_ENTRY_MENU);
}


//--------------------------------------------------------------
/**
 * �����o�[����M�o�b�t�@�擾�֐�
 *
 * @param   netID		
 * @param   pWork		
 * @param   size		
 *
 * @retval  u8 *		
 */
//--------------------------------------------------------------
static u8 * _RecvMemberInfo(int netID, void* pWork, int size)
{
  COMM_ENTRY_MENU_PTR em = pWork;
	
	return (u8*)(CommEntryMenu_GetMemberInfo(em));
}


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�e�փG���g���[��ʒm
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _CemRecv_Entry(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  COMM_ENTRY_MENU_PTR em = pWork;
  const CEM_SEND_ENTRY *recv_entry = pData;
  
  OS_TPrintf("�G���g���[��M�Fnet_id=%d\n", netID);
  CommEntryMenu_Entry(
    em, netID, &recv_entry->mystatus, recv_entry->force_entry, recv_entry->mac_address);
}

//==================================================================
/**
 * �f�[�^���M�F�e�փG���g���[��ʒm
 * @param   mystatus      MYSTATUS
 * @param   force_entry   TRUE:�����Q��   FALSE:�e�ɎQ�����Ă��������f���Ă��炤
 * @param   BOOL    TRUE:MP�ʐM�@FALSE:DS�ʐM
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL CemSend_Entry(const MYSTATUS *myst, BOOL force_entry, BOOL comm_mp)
{
  CEM_SEND_ENTRY send_entry;
  
  GFL_STD_MemClear(&send_entry, sizeof(CEM_SEND_ENTRY));
  MyStatus_Copy(myst, &send_entry.mystatus);
  OS_GetMacAddress(send_entry.mac_address);
  send_entry.force_entry = force_entry;
  
  if(comm_mp == TRUE){
    return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, 
      ENTRYMENU_CMD_ENTRY, sizeof(CEM_SEND_ENTRY), &send_entry, FALSE, FALSE, FALSE);
  }
  else{
    return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_NO_PARENTMACHINE, 
      ENTRYMENU_CMD_ENTRY, sizeof(CEM_SEND_ENTRY), &send_entry, FALSE, FALSE, FALSE);
  }
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�G���g���[OK
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _CemRecv_EntryOK(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  COMM_ENTRY_MENU_PTR em = pWork;
  const MYSTATUS *myst = pData;
  
  OS_TPrintf("Recv : EntryOK = %d\n", netID);
  CommEntryMenu_SetEntryAnswer(em, ENTRY_PARENT_ANSWER_OK, myst);
}

//==================================================================
/**
 * �f�[�^���M�F�G���g���[OK(�e�@�p)
 * @param   send_id   ���M��
 * @param   BOOL    TRUE:MP�ʐM�@FALSE:DS�ʐM
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL CemSend_EntryOK(NetID send_id, BOOL comm_mp, const MYSTATUS *myst)
{
  OS_TPrintf("Send : EntryOK net_id=%d\n", send_id);
  if(comm_mp == TRUE){
    return GFL_NET_SendDataEx(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER), send_id, 
      ENTRYMENU_CMD_ENTRY_OK, MyStatus_GetWorkSize(), myst, FALSE, FALSE, FALSE);
  }
  else{
    return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_id, 
      ENTRYMENU_CMD_ENTRY_OK, MyStatus_GetWorkSize(), myst, FALSE, FALSE, FALSE);
  }
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�G���g���[NG
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _CemRecv_EntryNG(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  COMM_ENTRY_MENU_PTR em = pWork;
  const MYSTATUS *myst = pData;
  
  CommEntryMenu_SetEntryAnswer(em, ENTRY_PARENT_ANSWER_NG, myst);
}

//==================================================================
/**
 * �f�[�^���M�F�G���g���[NG(�e�@�p)
 * @param   send_id   ���M��
 * @param   BOOL    TRUE:MP�ʐM�@FALSE:DS�ʐM
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL CemSend_EntryNG(NetID send_id, BOOL comm_mp, const MYSTATUS *myst)
{
  if(comm_mp == TRUE){
    return GFL_NET_SendDataEx(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER), send_id, 
      ENTRYMENU_CMD_ENTRY_NG, MyStatus_GetWorkSize(), myst, FALSE, FALSE, FALSE);
  }
  else{
    return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_id, 
      ENTRYMENU_CMD_ENTRY_NG, MyStatus_GetWorkSize(), myst, FALSE, FALSE, FALSE);
  }
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�Q�[���J�n��ʒm
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _CemRecv_GameStart(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  COMM_ENTRY_MENU_PTR em = pWork;

  CommEntryMenu_SetGameStart(em);
}

//==================================================================
/**
 * �f�[�^���M�F�Q�[���J�n��ʒm(�e�@�p)
 * @param   BOOL    TRUE:MP�ʐM�@FALSE:DS�ʐM
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL CemSend_GameStart(BOOL comm_mp)
{
  if(comm_mp == TRUE){
    return GFL_NET_SendData(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER), 
      ENTRYMENU_CMD_GAME_START, 0, NULL);
  }
  else{
    return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
      ENTRYMENU_CMD_GAME_START, 0, NULL);
  }
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�Q�[�����~��ʒm
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _CemRecv_GameCancel(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  COMM_ENTRY_MENU_PTR em = pWork;

  CommEntryMenu_SetGameCancel(em);
}

//==================================================================
/**
 * �f�[�^���M�F�Q�[�����~��ʒm(�e�@�p)
 * @param   BOOL    TRUE:MP�ʐM�@FALSE:DS�ʐM
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL CemSend_GameCancel(BOOL comm_mp)
{
  if(comm_mp == TRUE){
    return GFL_NET_SendData(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER), 
      ENTRYMENU_CMD_GAME_CANCEL, 0, NULL);
  }
  else{
    return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
      ENTRYMENU_CMD_GAME_CANCEL, 0, NULL);
  }
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �R�}���h��M�F�Q���ҏ��
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _CemRecv_MemberInfo(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  COMM_ENTRY_MENU_PTR em = pWork;
  const ENTRYMENU_MEMBER_INFO *recv_member_info = pData;

  OS_TPrintf("Recv:�Q���ҏ��\n");
  
  if(GFL_NET_IsParentMachine() == TRUE){
    CommEntyrMenu_MemberInfoReserveUpdate(em);
    return; //�e�@�͎󂯎��Ȃ�
  }
  
  //����f�[�^��M�o�b�t�@��_RecvMemberInfo�Œ��ڑ����̃A�h���X��n���Ă���̂ŃR�s�[�̕K�v��
  //�������A��M��������m�点��ׁA�Z�b�g�֐����Ăяo��
  CommEntryMenu_RecvMemberInfo(em, recv_member_info);
}

//==================================================================
/**
 * �f�[�^���M�F�Q���ҏ��(�e�@�p)
 * @param   member_info   
 * @param   BOOL    TRUE:MP�ʐM�@FALSE:DS�ʐM
 * @retval  BOOL		TRUE:���M�����B�@FALSE:���s
 */
//==================================================================
BOOL CemSend_MemberInfo(const ENTRYMENU_MEMBER_INFO *member_info, u8 send_bit, BOOL comm_mp)
{
  OS_TPrintf("SEND:�Q���ҏ�� send_bit = %d\n", send_bit);
  if(comm_mp == TRUE){
    return GFL_NET_SendDataExBit(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER), send_bit, 
      ENTRYMENU_CMD_MEMBER_INFO, CommEntryMenu_GetMemberInfoSize(), member_info, 
      FALSE, FALSE, TRUE);
  }
  else{
    return GFL_NET_SendDataExBit(GFL_NET_HANDLE_GetCurrentHandle(), send_bit, 
      ENTRYMENU_CMD_MEMBER_INFO, CommEntryMenu_GetMemberInfoSize(), member_info, 
      FALSE, FALSE, TRUE);
  }
}

