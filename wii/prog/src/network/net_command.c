//=============================================================================
/**
 * @file	net_command.c
 * @brief	�f�[�^�𑗂邽�߂̃R�}���h���e�[�u�������Ă��܂� �����̓V�X�e����
 * @author	Katsumi Ohno
 * @date    2005.07.26
 */
//=============================================================================


#include "net_def.h"
#include "net_system.h"
#include "net_command.h"
#include "net_handle.h"

//==============================================================================
//  static��`
//==============================================================================

#define _COMM_TBL_MAX (4)  // �ςݏd�˂�e�[�u��

//==============================================================================
//	�e�[�u���錾
//  enum �Ɠ����Ȃ�тɂ��Ă�������
//  CALLBACK���Ă΂ꂽ���Ȃ��ꍇ��NULL�������Ă�������
//==============================================================================
static const NetRecvFuncTable _CommPacketTbl[] = {
    {NULL,                                   NULL},
    {NULL,NULL},
    {NULL,NULL},
    {GFL_NET_HANDLE_RecvNegotiation,         NULL},
    {GFL_NET_HANDLE_RecvNegotiationReturn,   NULL},
    {NULL,NULL},
    {GFL_NET_HANDLE_RecvTimingSync,          NULL},
    {GFL_NET_HANDLE_RecvTimingSyncEnd,       NULL},
    //���j���[�p
    //�ėp���M�p
#if GFL_NET_WIFI
    {GFL_NET_StateRecvWifiExit,              NULL},
#endif

};
/*
static const NetRecvFuncTable _CommPacketTbl[] = {
    {NULL,                                   NULL},
    {GFL_NET_StateRecvExit,                  NULL},
    {GFL_NET_StateRecvExitStart,             NULL},
    {GFL_NET_HANDLE_RecvNegotiation,         NULL},
    {GFL_NET_HANDLE_RecvNegotiationReturn,   NULL},
    {GFL_NET_StateRecvDSMPChange,            NULL},
    {GFL_NET_HANDLE_RecvTimingSync,          NULL},
    {GFL_NET_HANDLE_RecvTimingSyncEnd,       NULL},
    //���j���[�p
    //�ėp���M�p
#if GFL_NET_WIFI
    {GFL_NET_StateRecvWifiExit,              NULL},
#endif

};
*/
typedef struct{
    const NetRecvFuncTable* pCommPacket;  ///< field��battle�̃R�}���h�̃e�[�u��
    int listNum;                       ///< _pCommPacket��list��
    void* pWork;                       ///< field��battle�̃��C���ɂȂ郏�[�N
    int NetworkCommandHeaderNo;      ///< �e�[�u����ʔԍ�
} _NET_COMMAND_TABLE;


static _NET_COMMAND_TABLE _BaseTable = {
    _CommPacketTbl,
    GFL_NET_CMD_COMMAND_MAX,
    NULL,
    0,
};


typedef struct{
    _NET_COMMAND_TABLE commTbl[_COMM_TBL_MAX];
    u8 bThrowOutReq[GFL_NET_MACHINE_MAX];        ///< �R�}���h�������������t���O
    u8 bThrowOuted;    ///< �R�}���h����ւ����I��������TRUE
} _COMM_COMMAND_WORK;

static _COMM_COMMAND_WORK* _pCommandWork = NULL;


//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̏�����
 * @param   cokind          �R�}���h�̌n�ԍ� NetStartCommand_e
 * @param   pCommPacketLocal �Ăяo�����W���[����p�̃R�}���h�̌n
 * @param   listNum          �R�}���h��
 * @param   pWork            �Ăяo�����W���[����p�̃��[�N�G���A
 * @retval  none
 */
//--------------------------------------------------------------

void GFL_NET_COMMAND_Init(const int cokind,const NetRecvFuncTable* pCommPacketLocal,int listNum,void* pWork)
{
    int i;
    
    if(!_pCommandWork){
        _pCommandWork = (_COMM_COMMAND_WORK*)GFL_HEAP_AllocClearMemory(GFL_HEAPID_SYSTEM, sizeof(_COMM_COMMAND_WORK));
    }

    GFL_NET_AddCommandTable(cokind, pCommPacketLocal, listNum, pWork);
    _BaseTable.pWork = pWork;

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
        GFL_STD_MemClear(_pCommandWork, sizeof(_COMM_COMMAND_WORK));
        GFL_HEAP_FreeMemory(_pCommandWork);
        _pCommandWork = NULL;
    }
}

//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̒ǉ�
 * @param   cokind          �R�}���h�̌n�ԍ� NetStartCommand_e
 * @param   pCommPacketLocal �Ăяo�����W���[����p�̃R�}���h�̌n�e�[�u��
 * @param   listNum          �R�}���h��
 * @param   pWork            �Ăяo�����W���[����p�̃��[�N�G���A
 * @retval  none
 */
//--------------------------------------------------------------
void GFL_NET_AddCommandTable(const int cokind,const NetRecvFuncTable* pCommPacketLocal,const int listNum,void* pWork)
{
    int i;

    for(i = 0 ; i < _COMM_TBL_MAX ; i++){
        if(_pCommandWork->commTbl[i].NetworkCommandHeaderNo == 0){
            _pCommandWork->commTbl[i].NetworkCommandHeaderNo = cokind;
            _pCommandWork->commTbl[i].pCommPacket = pCommPacketLocal;
            _pCommandWork->commTbl[i].listNum = listNum;
            _pCommandWork->commTbl[i].pWork = pWork;
            break;
        }
    }
}

//--------------------------------------------------------------
/**
 * @brief   �R�}���h�e�[�u���̍폜
 * @param   cokind          �R�}���h�̌n�ԍ� NetStartCommand_e
 * @retval  none
 */
//--------------------------------------------------------------
void GFL_NET_DelCommandTable(const int cokind)
{
    int i;

    for(i = 0 ; i < _COMM_TBL_MAX ; i++){
        if(_pCommandWork->commTbl[i].NetworkCommandHeaderNo == cokind){
            GFL_STD_MemClear(&_pCommandWork->commTbl[i], sizeof(_NET_COMMAND_TABLE));
            break;
        }
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
 * @brief   �R�}���h�e�[�u����������
 * @param   command          �R�}���h
 * @retval  _NET_COMMAND_TABLE
 */
//--------------------------------------------------------------
static _NET_COMMAND_TABLE* _getCommandTable(int command)
{
    int i;
    int cokind = command&0xff00;
    int co = command&0xff;

    if(cokind==0){
        return &_BaseTable;
    }
    for(i = 0 ; i < _COMM_TBL_MAX ; i++){
        if(_pCommandWork->commTbl[i].NetworkCommandHeaderNo == cokind){
            NET_PRINT("_NET_COMMAND_TABLE %d %d\n",i,co);
            GF_ASSERT(_pCommandWork->commTbl[i].listNum > co);
            return &_pCommandWork->commTbl[i];
        }
    }
    GF_ASSERT(0);
    return NULL;
}

//--------------------------------------------------------------
/**
 * @brief   �R�}���h�֐���Ԃ�
 * @param   command          �R�}���h
 * @retval  _NET_COMMAND_TABLE
 */
//--------------------------------------------------------------
static void* _getCommandFunc(int command)
{
    _NET_COMMAND_TABLE* pTbl = _getCommandTable(command);

    if(pTbl){
        return pTbl->pCommPacket[(command&0xff)].callbackFunc;
    }
    return NULL;
}

//--------------------------------------------------------------
/**
 * @brief   �R�}���hwork��Ԃ�
 * @param   command          �R�}���h
 * @retval  _NET_COMMAND_TABLE
 */
//--------------------------------------------------------------
static void* _getCommandWork(int command)
{
    _NET_COMMAND_TABLE* pTbl = _getCommandTable(command);

    if(pTbl){
        return pTbl->pWork;
    }
    return NULL;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@�𓾂�֐���Ԃ�
 * @param   command          �R�}���h
 * @retval  _NET_COMMAND_TABLE
 */
//--------------------------------------------------------------
static void* _getCommandRecvAddrFunc(int command)
{
    _NET_COMMAND_TABLE* pTbl = _getCommandTable(command);

    if(pTbl){
        return pTbl->pCommPacket[(command&0xff)].getAddrFunc;
    }
    return NULL;
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
        func = (PTRCommRecvFunc)_getCommandFunc(command);
        if(func==NULL){
            GFL_NET_SystemSetError();
            return;  // �{�Ԃł̓R�}���h�Ȃ�����
        }
        NET_PRINT("command %d recvID %d\n", command, recvID);
        func(netID, size, pData, _getCommandWork(command), pNetHandle);
    }
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
    return ( _getCommandRecvAddrFunc(command) != NULL);
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

    NET_PRINT("�������݃o�b�t�@�擾\n");
    func = (PTRCommRecvBuffAddr)_getCommandRecvAddrFunc(command);
    if( func ){
        return func(netID, _getCommandWork(command), size);
    }
    return NULL;
}

