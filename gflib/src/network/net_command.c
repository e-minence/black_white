//=============================================================================
/**
 * @file	net_command.c
 * @brief	�f�[�^�𑗂邽�߂̃R�}���h���e�[�u�������Ă��܂� �����̓V�X�e����
 * @author	Katsumi Ohno
 * @date    2005.07.26
 */
//=============================================================================

#include "gflib.h"
#include "net.h"
#include "net_def.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"
#include "tool/net_tool.h"

//==============================================================================
//  static��`
//==============================================================================
static void _commCommandRecvThrowOut(const int netID, const int size, const void* pData, void* pWork,GFL_NETHANDLE* pNetHandle);
static void _commCommandRecvThrowOutReq(const int netID, const int size, const void* pData, void* pWork,GFL_NETHANDLE* pNetHandle);
static void _commCommandRecvThrowOutEnd(const int netID, const int size, const void* pData, void* pWork,GFL_NETHANDLE* pNetHandle);


//==============================================================================
//	�e�[�u���錾
//  comm_shar.h �� enum �Ɠ����Ȃ�тɂ��Ă�������
//  CALLBACK���Ă΂ꂽ���Ȃ��ꍇ��NULL�������Ă�������
//  �R�}���h�̃T�C�Y��Ԃ��֐��������Ă��炦��ƒʐM���y���Ȃ�܂�
//  _getZero�̓T�C�Y�Ȃ���Ԃ��܂��B_getVariable�͉σf�[�^�g�p���Ɏg���܂�
//==============================================================================
static const NetRecvFuncTable _CommPacketTbl[] = {
    {NULL,                        GFL_NET_COMMAND_SIZE( 0 ), NULL},
    {CommRecvExit,                GFL_NET_COMMAND_SIZE( 0 ), NULL},
    {GFL_NET_SystemRecvAutoExit,            GFL_NET_COMMAND_SIZE( 0 ), NULL},
    {CommRecvNegotiation,         CommRecvGetNegotiationSize, NULL},
    {CommRecvNegotiationReturn,   CommRecvGetNegotiationSize, NULL},
    {GFL_NET_SystemRecvDSMPChange,          GFL_NET_COMMAND_SIZE( 1 ), NULL},
    {GFL_NET_SystemRecvDSMPChangeReq,       GFL_NET_COMMAND_SIZE( 1 ), NULL},
    {GFL_NET_SystemRecvDSMPChangeEnd,       GFL_NET_COMMAND_SIZE( 1 ), NULL},
    {_commCommandRecvThrowOut,    GFL_NET_COMMAND_SIZE( 0 ), NULL},
    {_commCommandRecvThrowOutReq, GFL_NET_COMMAND_SIZE( 0 ), NULL},
    {_commCommandRecvThrowOutEnd, GFL_NET_COMMAND_SIZE( 0 ), NULL},
    {GFL_NET_ToolRecvTimingSync,          GFL_NET_COMMAND_SIZE( 1 ), NULL},
    {GFL_NET_ToolRecvTimingSyncEnd,       GFL_NET_COMMAND_SIZE( 1 ), NULL},
};

typedef struct{
    const NetRecvFuncTable* pCommPacket;  ///< field��battle�̃R�}���h�̃e�[�u��
    int listNum;                       ///< _pCommPacket��list��
    void* pWork;                       ///< field��battle�̃��C���ɂȂ郏�[�N
    u8 bThrowOutReq[GFL_NET_MACHINE_MAX];        ///< �R�}���h�������������t���O
    u8 bThrowOuted;    ///< �R�}���h����ւ����I��������TRUE
} _COMM_COMMAND_WORK;

static _COMM_COMMAND_WORK* _pCommandWork = NULL;


//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̏�����
 * @param   pCommPacketLocal �Ăяo�����W���[����p�̃R�}���h�̌n
 * @param   listNum          �R�}���h��
 * @param   pWork            �Ăяo�����W���[����p�̃��[�N�G���A
 * @retval  none
 */
//--------------------------------------------------------------

void GFL_NET_CommandInitialize(const NetRecvFuncTable* pCommPacketLocal,int listNum,void* pWork)
{
    int i;
    
    if(!_pCommandWork){
        _pCommandWork = GFL_HEAP_AllocMemory(GFL_HEAPID_SYSTEM, sizeof(_COMM_COMMAND_WORK));
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

void GFL_NET_CommandFinalize( void )
{
    if(_pCommandWork){
        GFL_HEAP_FreeMemory(_pCommandWork);
        _pCommandWork = NULL;
    }
}

//--------------------------------------------------------------
/**
 * @brief   �e�[�u���ɏ]�� ��M�R�[���o�b�N���Ăяo���܂�
 * @param   command         ��M�R�}���h
 * @param   netID           �l�b�g���[�NID
 * @param   size            ��M�f�[�^�T�C�Y
 * @param   pData           ��M�f�[�^
 * @retval  none
 */
//--------------------------------------------------------------

void GFL_NET_CommandCallBack(int netID, int command, int size, void* pData)
{
    PTRCommRecvFunc func;

    if( command < GFL_NET_CMD_COMMAND_MAX ){
        func = _CommPacketTbl[command].callbackFunc;
    }
    else{
        if((_pCommandWork==NULL) || (command > (_pCommandWork->listNum + GFL_NET_CMD_COMMAND_MAX))){
            OHNO_PRINT("command %d \n", command);
            OS_Panic("no command");
            GFL_NET_SystemSetError();
            return;  // �{�Ԃł̓R�}���h�Ȃ�����
        }
        func = _pCommandWork->pCommPacket[command - GFL_NET_CMD_COMMAND_MAX].callbackFunc;
    }
    if(func != NULL){
        if(_pCommandWork){
            func(netID, size, pData, _pCommandWork->pWork, GFL_NET_SystemGetHandle(netID));
        }
        else{
            func(netID, size, pData, NULL, GFL_NET_SystemGetHandle(netID));
        }
    }
}


//--------------------------------------------------------------
/**
 * @brief   ��`���������R�}���h�̃T�C�Y��Ԃ��܂�
 * @param   command         �R�}���h
 * @retval  �f�[�^�̃T�C�Y   �ςȂ� COMM_VARIABLE_SIZE��Ԃ��܂�
 */
//--------------------------------------------------------------

int GFL_NET_CommandGetPacketSize(int command)
{
    int size = 0;
    PTRCommRecvSizeFunc func=NULL;

    if( command < GFL_NET_CMD_COMMAND_MAX ){
        func = _CommPacketTbl[command].getSizeFunc;
    }
    else{
        if((_pCommandWork==NULL) || (command > (_pCommandWork->listNum + GFL_NET_CMD_COMMAND_MAX))){
            OHNO_PRINT("command %d \n", command);
            OS_Panic("no command");
            GFL_NET_SystemSetError();
        }
        else{
            func = _pCommandWork->pCommPacket[command - GFL_NET_CMD_COMMAND_MAX].getSizeFunc;
        }
    }
    if(func != NULL){
        size = func();
    }
    return size;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@�������Ă��邩�ǂ����̌���
 * @param   command         �R�}���h
 * @retval  �����Ă�Ȃ�TRUE
 */
//--------------------------------------------------------------

BOOL GFL_NET_CommandCreateBuffCheck(int command)
{
    if( command < GFL_NET_CMD_COMMAND_MAX ){
        return ( _CommPacketTbl[command].getAddrFunc != NULL);
    }
    return (_pCommandWork->pCommPacket[command - GFL_NET_CMD_COMMAND_MAX].getAddrFunc != NULL);
}

//--------------------------------------------------------------
/**
 * @brief   
 * @param   command         �R�}���h
 * @retval  �����Ă�Ȃ�TRUE
 */
//--------------------------------------------------------------

void* GFL_NET_CommandCreateBuffStart(int command,int netID, int size)
{
    PTRCommRecvBuffAddr func;

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

//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̔p��
 * @param   none
 * @retval  ��t����������TRUE
 */
//--------------------------------------------------------------

BOOL GFL_NET_CommandThrowOut(void)
{
    if(_pCommandWork==NULL){
        OS_TPanic("no init");
    }
    _pCommandWork->bThrowOuted = FALSE;
    return GFL_NET_SystemSendFixData(GFL_NET_CMD_THROWOUT);
}


//==============================================================================
/**
 * �R�}���h�p���R�[���o�b�N
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _commCommandRecvThrowOut(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    if(GFL_NET_SystemGetCurrentID() != COMM_PARENT_ID){
        return;
    }
//    OHNO_PRINT("CommRecvDSMPChange ��M\n");
    // �S���ɐ؂�ւ��M���𑗂�
    _pCommandWork->bThrowOutReq[netID] = TRUE;
    for(i = 0 ; i < GFL_NET_MACHINE_MAX; i++){
        if(!GFL_NET_SystemIsConnect(i)){
            continue;
        }
        if(!_pCommandWork->bThrowOutReq[i]){
            return;
        }
    }
    GFL_NET_SystemSendData_ServerSide(GFL_NET_CMD_THROWOUT_REQ, NULL, 0);
}

//==============================================================================
/**
 * �R�}���h�p�����s�R�[���o�b�N
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _commCommandRecvThrowOutReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    _pCommandWork->pCommPacket = NULL;
    _pCommandWork->listNum = 0;
    _pCommandWork->pWork = NULL;
    _pCommandWork->bThrowOuted = TRUE;

    GFL_NET_SystemSendFixSizeData(GFL_NET_CMD_THROWOUT_END,pData);
}

//==============================================================================
/**
 * �R�}���h�p�������R�[���o�b�N
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _commCommandRecvThrowOutEnd(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    if(GFL_NET_SystemGetCurrentID() != COMM_PARENT_ID){
        return;
    }
    _pCommandWork->bThrowOutReq[netID] = FALSE;
}

//==============================================================================
/**
 * �R�}���h�����ł������ǂ������m�F����
 * @retval  TRUE ��������
 * @retval  FALSE �������Ă��Ȃ�
 */
//==============================================================================

BOOL GFL_NET_CommandIsThrowOuted(void)
{
    if(_pCommandWork){
        return _pCommandWork->bThrowOuted;
    }
    return FALSE;
}

