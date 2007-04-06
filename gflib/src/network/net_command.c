//=============================================================================
/**
 * @file	net_command.c
 * @brief	�f�[�^�𑗂邽�߂̃R�}���h���e�[�u�������Ă��܂� �����̓V�X�e����
 * @author	Katsumi Ohno
 * @date    2005.07.26
 */
//=============================================================================

#include "gflib.h"

#include "net_def.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"
#include "tool/net_tool.h"

//==============================================================================
//  static��`
//==============================================================================


//==============================================================================
//	�e�[�u���錾
//  comm_shar.h �� enum �Ɠ����Ȃ�тɂ��Ă�������
//  CALLBACK���Ă΂ꂽ���Ȃ��ꍇ��NULL�������Ă�������
//  �R�}���h�̃T�C�Y��Ԃ��֐��������Ă��炤���A�Œ�o�C�g�Ȃ��
//  GFL_NET_COMMAND_SIZE(size) �}�N���������Ă��炤�ƁA
//  �T�C�Y��ʐM�ő���Ȃ��̂ŏ����y���Ȃ�܂�
//  GFL_NET_COMMAND_VARIABLE �͉σf�[�^�g�p���Ɏg���܂�
//==============================================================================
static const NetRecvFuncTable _CommPacketTbl[] = {
    {NULL,                        GFL_NET_COMMAND_SIZE( 0 ), NULL},
    {GFL_NET_StateRecvExit,                GFL_NET_COMMAND_SIZE( 0 ), NULL},
    {GFL_NET_StateRecvExitStart,            GFL_NET_COMMAND_SIZE( 0 ), NULL},
    {GFL_NET_StateRecvNegotiation,         GFL_NET_COMMAND_SIZE( 1 ), NULL},
    {GFL_NET_StateRecvNegotiationReturn,   GFL_NET_COMMAND_SIZE( 2 ), NULL},
    {GFL_NET_StateRecvDSMPChange,          GFL_NET_COMMAND_SIZE( 1 ), NULL},
    {GFL_NET_StateRecvDSMPChangeReq,       GFL_NET_COMMAND_SIZE( 1 ), NULL},
    {GFL_NET_StateRecvDSMPChangeEnd,       GFL_NET_COMMAND_SIZE( 1 ), NULL},
    {GFL_NET_TOOL_RecvTimingSync,          GFL_NET_COMMAND_SIZE( 1 ), NULL},
    {GFL_NET_TOOL_RecvTimingSyncEnd,       GFL_NET_COMMAND_SIZE( 1 ), NULL},
#if GFL_NET_WIFI
    {GFL_NET_StateRecvWifiExit,            GFL_NET_COMMAND_SIZE( 1 ), NULL},
#endif
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

void GFL_NET_COMMAND_Init(const NetRecvFuncTable* pCommPacketLocal,int listNum,void* pWork)
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

void GFL_NET_COMMAND_Exit( void )
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

void GFI_NET_COMMAND_CallBack(int netID, int recvID, int command, int size, void* pData)
{
    int i;
    PTRCommRecvFunc func;
    BOOL bCheck;
    GFL_NETHANDLE* pNetHandle;

    NET_PRINT("--- %d \n", netID);

    for(i = 0;i < GFL_NET_HANDLE_MAX;i++){
        pNetHandle = GFL_NET_GetNetHandle(i);
        if(!pNetHandle){
            continue;
        }
        if((recvID != GFL_NET_SENDID_ALLUSER) && (i != recvID)){  // ��M�҂��Ⴄ�ꍇcontinue
            continue;
        }
        bCheck=TRUE;  // �l�S�V�G�[�V�����R�}���h�ȊO��͂��Ȃ�
        if(!GFL_NET_IsNegotiation(pNetHandle)){  // �l�S�V�G�[�V����������łȂ��ꍇ
            if(!((command == GFL_NET_CMD_NEGOTIATION) || (command == GFL_NET_CMD_NEGOTIATION_RETURN))){
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
                NET_PRINT("command %d recvID %d\n", command,i);
                if(_pCommandWork){
                    func(netID, size, pData, _pCommandWork->pWork, pNetHandle);
                }
                else{
                    func(netID, size, pData, NULL, pNetHandle);
                }
            }
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

int GFI_NET_COMMAND_GetPacketSize(int command)
{
    int size = 0;
    PTRCommRecvSizeFunc func=NULL;
    u32 num;

    if( command < GFL_NET_CMD_COMMAND_MAX ){
        func = _CommPacketTbl[command].getSizeFunc;
    }
    else{
        if((_pCommandWork==NULL) || (command > (_pCommandWork->listNum + GFL_NET_CMD_COMMAND_MAX))){
            NET_PRINT("command %d \n", command);
            OS_Panic("no command");
            GFL_NET_SystemSetError();
        }
        else{
            func = _pCommandWork->pCommPacket[command - GFL_NET_CMD_COMMAND_MAX].getSizeFunc;
        }
    }
    if(func != NULL){
        num = (int)func;
        if((_GFL_NET_COMMAND_SIZE_H & num) == _GFL_NET_COMMAND_SIZE_H){
            size = num - _GFL_NET_COMMAND_SIZE_H;
        }
        else{
            size = func();
        }
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
 * @param   command         �R�}���h
 * @param   netID           ���M��
 * @param   size         �T�C�Y
 * @retval  ��M�o�b�t�@ �����Ă��Ȃ��Ȃ��NULL
 */
//--------------------------------------------------------------

void* GFI_NET_COMMAND_CreateBuffStart(int command,int netID, int size)
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

