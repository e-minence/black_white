//=============================================================================
/**
 * @file	net.c
 * @brief	GFL�ʐM���C�u�����[
 * @author	GAME FREAK Inc.
 * @date    2006.12.14
 */
//=============================================================================

#include "gflib.h"

#include "device/wh_config.h"

#include "net.h"
#include "net_def.h"
#include "device/net_wireless.h"
#include "net_system.h"
#include "net_command.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "tool/net_tool.h"


struct _GFL_NETSYS{
    GFLNetInitializeStruct aNetInit;  ///< �������\����
    GFL_NETHANDLE* pNetHandle[GFL_NET_MACHINE_MAX];

    GFL_NETWL* pNetWL;
    
    int heapID;  ///< �ʐM�̊m�ۃ������[�̏ꏊ
    
} ;


static GFL_NETSYS* _pNetSys = NULL; // �ʐM�̃������[���Ǘ�����static�ϐ�





struct _GFL_NETHANDLE{
    int netID;



};



static int _addNetHandle(GFL_NETSYS* pNet, GFL_NETHANDLE* pHandle)
{
    int i;

    for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
        if(pNet->pNetHandle[i]!=NULL){
            pNet->pNetHandle[i] = pHandle;
            return i;
        }
    }
    OS_TPanic("no handle");
    return 0;
}

static void _deleteAllNetHandle(GFL_NETSYS* pNet)
{
    int i;

    for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
        if(pNet->pNetHandle[i]!=NULL){
            GFL_HEAP_FreeMemory(pNet->pNetHandle[i]);
            pNet->pNetHandle[i] = NULL;
        }
    }
}



//==============================================================================
/**
 * @brief �ʐM������
 * @param[in]   pNetInit  �ʐM�������\���̂̃|�C���^
 * @param[in]   netID     �l�b�gID
 * @return none
 */
//==============================================================================
void GFL_NET_Initialize(const GFLNetInitializeStruct* pNetInit,int heapID)
{
    GFL_NETSYS* pNet = GFL_HEAP_AllocMemory(heapID, sizeof(GFL_NETSYS));

    GFL_STD_MemClear(pNet, sizeof(GFL_NETSYS));

    GFL_STD_MemCopy(pNetInit, &pNet->aNetInit, sizeof(GFLNetInitializeStruct));
    pNet->heapID = heapID;
    
    _pNetSys = pNet;
}

//==============================================================================
/**
 * @brief  �ʐM�I��
 * @retval  TRUE  �I�����܂���
 * @retval  FALSE �I�����܂��� ���Ԃ��󂯂Ă������Ă�ł�������
 */
//==============================================================================
BOOL GFL_NET_Finalize(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    _deleteAllNetHandle(pNet);
    GFL_HEAP_FreeMemory(pNet);
    _pNetSys = NULL;
    return TRUE;
}

//-----�r�[�R���֘A
//==============================================================================
/**
 * @brief �r�[�R�����󂯎��T�[�r�X��ǉ��w��  �i�������̃T�[�r�X�̃r�[�R�����E���������Ɂj
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   GameServiceID gsid  �Q�[���T�[�r�XID  �ʐM�̎��
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_AddBeaconServiceID(GFL_NETHANDLE* pNet, GameServiceID gsid);

//==============================================================================
/**
 * @brief ��M�r�[�R���f�[�^�𓾂�
 * @param[in,out]   pNet     �ʐM�n���h���̃|�C���^
 * @param   index  �r�[�R���o�b�t�@index
 * @return  �r�[�R���f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
extern void* GFL_NET_GetBeaconData(GFL_NETHANDLE* pNet, int index)
{
    if(index >= GFL_NET_MACHINE_MAX){
        return NULL;
    }
    return GFL_NET_WLGetUserBss(index);
    
}

//==============================================================================
/**
 * @brief ��M�r�[�R���ɑΉ�����MAC�A�h���X�𓾂�
 * @param[in,out]   pNet     �ʐM�n���h���̃|�C���^
 * @param   index  �r�[�R���o�b�t�@index
 * @return  �r�[�R���f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
extern u8* GFL_NET_GetBeaconMacAddress(GFL_NETHANDLE* pNet, int index);


//--------�ڑ��E�ؒf


//==============================================================================
/**
 * @brief �q�@�ɂȂ�ڑ�����
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
GFL_NETHANDLE* GFL_NET_ClientConnect(GFL_NETSYS* pNetSYS)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    
    GFL_NETHANDLE* pHandle = GFL_HEAP_AllocMemory(pNet->heapID, sizeof(GFL_NETHANDLE));

    _addNetHandle(pNet, pHandle);
    return pHandle;
}

//==============================================================================
/**
 * @brief �q�@�ɂȂ�w�肵���e�@�ɐڑ�����
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_ClientConnectTo(GFL_NETSYS* pNetSYS,u8* macAddress);

//==============================================================================
/**
 * @brief �e�@�ɂȂ�҂��󂯂�
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_ServerConnect(GFL_NETSYS* pNetSYS);

//==============================================================================
/**
 * @brief �e�@�ɂȂ�w�肳�ꂽ�q�@�̐ڑ���҂��󂯂�
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @param   num            �q�@�l��
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_ServerConnectTo(GFL_NETSYS* pNetSYS,const u8* macAddress, const int num);

//==============================================================================
/**
 * @brief �e�q�؂�ւ��ڑ����s��
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_SwitchConnect(GFL_NETSYS* pNetSYS);

//==============================================================================
/**
 * @brief ���̃n���h���̒ʐM�ԍ��𓾂�
 * @param[in,out]  NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  NetID     �ʐMID
 */
//==============================================================================
extern NetID GFL_NET_GetNetID(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief ���݂̐ڑ��l���𓾂�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  int  �ڑ���
 */
//==============================================================================
extern int GFL_NET_GetConnectNum(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief ID�̒ʐM�Ɛڑ����Ă��邩�ǂ�����Ԃ�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   NetID id            �l�b�gID
 * @retval  BOOL  �ڑ����Ă�����TRUE
 */
//==============================================================================
extern BOOL GFL_NET_IsConnectMember(GFL_NETHANDLE* pNet,NetID id);

//==============================================================================
/**
 * @brief �ʐM�ؒf����
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  none
 */
//==============================================================================
//
extern void GFL_NET_Disconnect(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief �ڑ������ǂ���
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  none
 */
//==============================================================================
extern BOOL GFL_NET_IsConnect(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief �ڑ����ύX
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   u8 num     �ύX��
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SetClientConnectNum(GFL_NETHANDLE* pNet,u8 num);

//==============================================================================
/**
 * @brief �V�K�ڑ��֎~������ݒ�
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   BOOL bEnable     TRUE=�ڑ����� FALSE=�֎~
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SetClientConnect(GFL_NETHANDLE* pNet,BOOL bEnable);


//--------���M

//==============================================================================
/**
 * @brief ���M�J�n���q�@�ɓ`����i�e�@��p�֐��j
 *        ���̊֐��ȍ~�ݒ肵�����M�f�[�^���q�@�Ŏg�p�����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   NetID id            �q�@ID
 * @return  none
 */
//==============================================================================
extern void GFL_NET_SendEnableCommand(GFL_NETHANDLE* pNet,const NetID id);

//==============================================================================
/**
 * @brief ���M�������肽���ǂ�������������i�q�@��p�֐��j
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @retval  TRUE   ����
 * @retval  FALSE  �֎~
 */
//==============================================================================
extern BOOL GFL_NET_IsSendEnable(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief ���M�J�n
 *        �S���ɖ������ő��M  ���M�T�C�Y�͏��������̃e�[�u����������o��
 *        �f�[�^�͕K���R�s�[����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   u16 sendCommand                ���M����R�}���h
 * @param[in]   void* data                     ���M�f�[�^�|�C���^
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_SendData(GFL_NETHANDLE* pNet,const u16 sendCommand,const void* data);

//==============================================================================
/**
 * @brief ���M�J�n
 * @param[in,out]  pNet  �ʐM�n���h��
 * @param[in]   sendID                     ���M���� �S���֑��M����ꍇ NET_SENDID_ALLUSER
 * @param[in]   sendCommand                ���M����R�}���h
 * @param[in]   pCBSendEndFunc  ���M������������R�[���o�b�N�֐��̓o�^
 * @param[in]   size                       ���M�f�[�^�T�C�Y
 * @param[in]   data                     ���M�f�[�^�|�C���^
 * @param[in]   bDataCopy                 �f�[�^���R�s�[����ꍇTRUE
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u16 sendCommand,
                    const CBSendEndFunc* pCBSendEndFunc,const u32 size,
                    const void* data, const BOOL bDataCopy);

//==============================================================================
/**
 * @brief ���M�f�[�^���������ǂ���
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @retval  TRUE   �����ꍇ
 * @retval  FALSE  �c���Ă���ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_IsEmptySendData(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief   ���t���[�����肽���f�[�^��o�^����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param   pGet      �f�[�^�擾�֐�
 * @param   pRecv     ��M�֐�
 * @param   size      �T�C�Y
 * @return  none
 */
//==============================================================================
extern void GFL_NET_SetEveryTimeSendData(GFL_NETHANDLE* pNet, CBGetEveryTimeData* pGet, CBRecvEveryTimeData* pRecv,const int size);



//--------���̑��A�c�[����
//==============================================================================
/**
 * @brief �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   u8 no   �^�C�~���O��肽���ԍ�
 * @return      none
 */
//==============================================================================
extern void GFL_NET_TimingSyncStart(GFL_NETHANDLE* pNet, const u8 no);
//==============================================================================
/**
 * @brief �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   no   �͂��ԍ�
 * @retval  TRUE    �^�C�~���O�����v
 * @retval  FALSE   �^�C�~���O��������Ă��Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_IsTimingSync(GFL_NETHANDLE* pNet, const u8 no);


//==============================================================================
/**
 * @brief    �ʐM�Ǘ��\���̂𓾂�  �i�ʐM�����g�p �O���Ɍ��J����Ȃ�GFL_NET_Initialize�̖߂�l�ɂ���ق��������j
 * @return   �l�b�g�V�X�e���\���̃|�C���^
 */
//==============================================================================
GFL_NETSYS* _GFL_NET_GetNETSYS(void)
{
    return _pNetSys;
}

//==============================================================================
/**
 * @brief    WL�ʐM�Ǘ��\���̂𓾂�  �i�ʐM�����g�p ���荞�ݗp�j
 * @return   �l�b�g�V�X�e���\���̃|�C���^
 */
//==============================================================================

GFL_NETWL* _GFL_NET_GetNETWL(void)
{
    return _pNetSys->pNetWL;
}

