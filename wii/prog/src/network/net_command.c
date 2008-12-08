//=============================================================================
/**
 * @file	net_command.c
 * @brief	�f�[�^�𑗂邽�߂̃R�}���h���e�[�u�������Ă��܂� �����̓V�X�e����
 * @author	Katsumi Ohno
 * @date	2005.07.26
 */
//=============================================================================

#include "net_def.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"
#include "net_handle.h"

#define RecvTestFunc(val) RecvTest ## val

//==============================================================================
//  static��`
//==============================================================================

static void RecvTestB(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void RecvTestC(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void RecvTestD(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void RecvTestE(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void RecvTestF(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void RecvTestG(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void RecvTestH(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);

static void RecvTestB(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{ NET_PRINT( "CallFunc B\n" ); }
static void RecvTestC(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{ NET_PRINT( "CallFunc C\n" ); }
static void RecvTestD(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{ NET_PRINT( "CallFunc D\n" ); }
static void RecvTestE(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{ NET_PRINT( "CallFunc E\n" ); }
static void RecvTestF(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{ NET_PRINT( "CallFunc F\n" ); }
static void RecvTestG(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{ NET_PRINT( "CallFunc G\n" ); }
static void RecvTestH(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{ NET_PRINT( "CallFunc H\n" ); }


//==============================================================================
//	�e�[�u���錾
//  enum �Ɠ����Ȃ�тɂ��Ă�������
//  CALLBACK���Ă΂ꂽ���Ȃ��ꍇ��NULL�������Ă�������
//  �R�}���h�̃T�C�Y��Ԃ��֐��������Ă��炤��
//  �Œ�o�C�g�Ȃ�� GFL_NET_COMMAND_SIZE(size) �}�N���������Ă�������
//  GFL_NET_COMMAND_VARIABLE �͉σf�[�^�g�p���Ɏg���܂�
//==============================================================================
static const NetRecvFuncTable _CommPacketTbl[] = {
	{NULL,									NULL},	//�����͌�����NULL
//	{GFL_NET_StateRecvExit,					NULL},
	{RecvTestFunc(B)				,					NULL},
//	{GFL_NET_StateRecvExitStart,			NULL},
	{RecvTestFunc(C),									NULL},
	{GFL_NET_HANDLE_RecvNegotiation,		NULL},
	{GFL_NET_HANDLE_RecvNegotiationReturn,  NULL},
//	{GFL_NET_StateRecvDSMPChange,			NULL},
	{RecvTestFunc(F)						,			NULL},
//	{GFL_NET_HANDLE_RecvTimingSync,			NULL},
	{RecvTestFunc(G),									NULL},
//	{GFL_NET_HANDLE_RecvTimingSyncEnd,		NULL},
	{RecvTestFunc(H),									NULL},
	//���j���[�p
	//�ėp���M�p
};

typedef struct{
	const NetRecvFuncTable* pCommPacket;  ///< field��battle�̃R�}���h�̃e�[�u��
	int listNum;					   ///< _pCommPacket��list��
	void* pWork;					   ///< field��battle�̃��C���ɂȂ郏�[�N
	u8 bThrowOutReq[GFL_NET_MACHINE_MAX];		///< �R�}���h�������������t���O
	u8 bThrowOuted;	///< �R�}���h����ւ����I��������TRUE
} _COMM_COMMAND_WORK;

static _COMM_COMMAND_WORK* _pCommandWork = NULL;


//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̏�����
 * @param   pCommPacketLocal �Ăяo�����W���[����p�̃R�}���h�̌n
 * @param   listNum		  �R�}���h��
 * @param   pWork			�Ăяo�����W���[����p�̃��[�N�G���A
 * @retval  none
 */
//--------------------------------------------------------------

void GFL_NET_COMMAND_Init(const NetRecvFuncTable* pCommPacketLocal,int listNum,void* pWork)
{
	int i;
	
	if(!_pCommandWork){
		_pCommandWork = (_COMM_COMMAND_WORK*)GFL_HEAP_AllocClearMemory(GFL_HEAPID_SYSTEM, sizeof(_COMM_COMMAND_WORK));
	}
	_pCommandWork->pCommPacket = pCommPacketLocal;
	_pCommandWork->listNum = listNum;
	_pCommandWork->pWork = pWork;


	for(i = 0 ; i < GFL_NET_MACHINE_MAX; i++){
		_pCommandWork->bThrowOutReq[i] = FALSE;
	}
	_pCommandWork->bThrowOuted = FALSE;


}

//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̏I������
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void GFL_NET_COMMAND_Exit( void )
{
	if(_pCommandWork){
		GFL_HEAP_FreeMemory(_pCommandWork);
		_pCommandWork = NULL;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �R�}���h���󂯎��l���ǂ���
 * @param   recvID  �󂯎���ID
 * @retval  �󂯎��ꍇTRUE
 */
//--------------------------------------------------------------

BOOL GFI_NET_COMMAND_RecvCheck(int recvID)
{
	if(recvID == GFL_NET_SENDID_ALLUSER){
		return TRUE;
	}
	else if( GFL_NET_SystemGetCurrentID() == recvID ){
		return TRUE;
	}
	else if((GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE) && (recvID==GFL_NET_NETID_SERVER) ){  // ��M�҂��Ⴄ�ꍇreturn
		return TRUE;
	}
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief   �e�[�u���ɏ]�� ��M�R�[���o�b�N���Ăяo���܂�
 * @param   command		 ��M�R�}���h
 * @param   netID		   �l�b�g���[�NID
 * @param   size			��M�f�[�^�T�C�Y
 * @param   pData		   ��M�f�[�^
 * @retval  none
 */
//--------------------------------------------------------------

void GFI_NET_COMMAND_CallBack(int netID, int recvID, int command, int size, void* pData,GFL_NETHANDLE* pNetHandle)
{
	int i;
	PTRCommRecvFunc func;
	BOOL bCheck;

	if( GFI_NET_COMMAND_RecvCheck(recvID) == FALSE){
		return;
	}
	bCheck=TRUE;  // �l�S�V�G�[�V�����R�}���h�ȊO��͂��Ȃ�
	if(!GFL_NET_HANDLE_IsNegotiation(pNetHandle)){  // �l�S�V�G�[�V����������łȂ��ꍇ
		if( command >= GFL_NET_CMD_COMMAND_MAX ){
			bCheck=FALSE;
		}
	}
	if(bCheck){
		if( command < GFL_NET_CMD_COMMAND_MAX ){
			func = _CommPacketTbl[command].callbackFunc;
		}
		else{
			if((_pCommandWork==NULL) || (command > (_pCommandWork->listNum + GFL_NET_CMD_COMMAND_MAX))){
				NET_PRINT("command %d \n", command);
				OS_Panic("no command");
				GFL_NET_SystemSetError();
				return;  // �{�Ԃł̓R�}���h�Ȃ�����
			}
			func = _pCommandWork->pCommPacket[command - GFL_NET_CMD_COMMAND_MAX].callbackFunc;
		}
		if(func != NULL){
			NET_PRINT("command %d recvID %d\n", command, recvID);
			if(_pCommandWork){
				func(netID, size, pData, _pCommandWork->pWork, pNetHandle);
			}
			else{
				func(netID, size, pData, NULL, pNetHandle);
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@�������Ă��邩�ǂ����̌���
 * @param   command		 �R�}���h
 * @retval  �����Ă�Ȃ�TRUE
 */
//--------------------------------------------------------------

BOOL GFI_NET_COMMAND_CreateBuffCheck(int command)
{
	if( command < GFL_NET_CMD_COMMAND_MAX ){
		return ( _CommPacketTbl[command].getAddrFunc != NULL);
	}
	return (_pCommandWork->pCommPacket[command - GFL_NET_CMD_COMMAND_MAX].getAddrFunc != NULL);
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@�𓾂�
 * @param   command		 �R�}���h
 * @param   netID		   ���M��
 * @param   size		 �T�C�Y
 * @retval  ��M�o�b�t�@ �����Ă��Ȃ��Ȃ��NULL
 */
//--------------------------------------------------------------

void* GFI_NET_COMMAND_CreateBuffStart(int command,int netID, int size)
{
	PTRCommRecvBuffAddr func;

	NET_PRINT("�������݃o�b�t�@�擾\n");

	if( command < GFL_NET_CMD_COMMAND_MAX ){
		func = _CommPacketTbl[command].getAddrFunc;
		return func(netID, NULL, size);
	}
	else{
		func = _pCommandWork->pCommPacket[command - GFL_NET_CMD_COMMAND_MAX].getAddrFunc;
		return func(netID, _pCommandWork->pWork, size);
	}
	return NULL;
}

