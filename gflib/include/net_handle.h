//=============================================================================
/**
 * @file	net_handle.h
 * @brief	�ʐM�n���h���̊֐�
 * @author  GAME FREAK Inc. k.ohno
 * @date    2008.2.26
 */
//=============================================================================
#ifndef __NET_HANDLE_H__
#define __NET_HANDLE_H__


///<  @brief  �l�b�g���[�N�n���h��
struct _GFL_NETHANDLE{
    void* pInfomation;       ///<���[�U�[���m����������f�[�^�̃|�C���^
    int infomationSize;      ///< ���[�U�[���m����������f�[�^�̃T�C�Y
    u8 timing[GFL_NET_HANDLE_MAX];       ///< �q�@����̎󂯎��L�^
    u16 negotiationBit;  ///< ���̐ڑ����������ǂ��� 0�Ȃ狖��
    u8 timingRecv;       ///< �ʐM�Ŏ󂯎�����ԍ�
    u8 timingSyncEnd;    ///< �����R�}���h�p
    u8 timingSendFlg;      ///< ���������ǂ���
    u8 timingSyncMy;     ///< ������������NO
    u8 negotiationType;        ///< �ڑ����Ă���n���h���̏��
    u8 negotiationSendFlg;      ///< ���������ǂ���
    u8 serviceNo;              ///< �ʐM�T�[�r�X�ԍ�
    u8 version;                ///< �ʐM����̒ʐM�o�[�W�����ԍ�
};


/// @brief  �ʐM�c�[�����[�N
typedef struct _NET_TOOLSYS_t NET_TOOLSYS;


typedef enum {
    _NEG_TYPE_FIRST,
    _NEG_TYPE_SEND_BEFORE,
    _NEG_TYPE_SEND,
    _NEG_TYPE_SEND_AFTER,
    _NEG_TYPE_RECV,
    _NEG_TYPE_CONNECT,       ///< �ڑ���e���F�؂����ꍇ CONNECT�ɂȂ� ���̃R�}���h��͒ʐM�\�ɂȂ�
    _NEG_TYPE_END,           ///< �ؒf���

} _NEGOTIATION_TYPE_E;


//==============================================================================
/**
 * @brief       netHandle�̔ԍ��𓾂�
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @param       pHandle   �ʐM�n���h��
 * @return      �ǉ������ԍ�
 */
//==============================================================================
extern int GFL_NET_HANDLE_GetNetHandleID(GFL_NETHANDLE* pHandle);

//==============================================================================
/**
 * @brief  �^�C�~���O�R�}���h����M����   GFL_NET_CMD_TIMING_SYNC
 * @param[in]   netID  �ʐM���M��ID
 * @param[in]   size   ��M�f�[�^�T�C�Y
 * @param[in]   pData  ��M�f�[�^�|�C���^
 * @param[in,out]   pWork  �g�p�҂��K�v��work�̃|�C���^
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @retval      none
 */
//==============================================================================
extern void GFL_NET_HANDLE_RecvTimingSync(const int netID, const int size, const void* pData,
                                       void* pWork, GFL_NETHANDLE* pNet);
//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���hEND����M����   GFL_NET_CMD_TIMING_SYNC_END
 * @param[in]   netID  �ʐM���M��ID
 * @param[in]   size   ��M�f�[�^�T�C�Y
 * @param[in]   pData  ��M�f�[�^�|�C���^
 * @param[in,out]   pWork  �g�p�҂��K�v��work�̃|�C���^
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @retval      none
 */
//==============================================================================
extern void GFL_NET_HANDLE_RecvTimingSyncEnd(const int netID, const int size, const void* pData,
                                          void* pWork, GFL_NETHANDLE* pNet);
//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @param   no   �^�C�~���O��肽���ԍ�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_HANDLE_TimingSyncStart(GFL_NETHANDLE* pNet, const u8 no);
//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param   pNet   �l�b�g���[�N�n���h��
 * @param   no     ��������ԍ�
 * @retval  TRUE   �͂��Ă���
 * @retval  FALSE  �͂��Ă��Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_HANDLE_IsTimingSync(const GFL_NETHANDLE* pNet, const u8 no);
//==============================================================================
/**
 * @brief   �n���h�������̃��C��
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_HANDLE_MainProc(void);
//==============================================================================
/**
 * @brief   DSMP���[�h�ύX��Main�֐�
 * @param   pNet  �ʐM�n���h��
 * @return  noen
 */
//==============================================================================
extern void GFL_NET_TOOL_TransmissonMain(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief       �����̃l�S�V�G�[�V����������ł��邩�ǂ���
 * @param[in]   pHandle   �ʐM�n���h��
 * @return      ����ł���ꍇTRUE   �܂��̏ꍇFALSE
 */
//==============================================================================
extern BOOL GFL_NET_HANDLE_IsNegotiation(GFL_NETHANDLE* pHandle);

//==============================================================================
/**
 * @brief       �l�S�V�G�[�V�������������Ă���̂����l��
 * @return      ����ł���l�� 
 */
//==============================================================================
extern int GFL_NET_HANDLE_GetNumNegotiation(void);
//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�R�[���o�b�N GFL_NET_CMD_NEGOTIATION
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_HANDLE_RecvNegotiation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�R�[���o�b�N GFL_NET_CMD_NEGOTIATION_RETURN
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_HANDLE_RecvNegotiationReturn(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   �����̃}�V���̃n���h�����A��
 * @retval  GFL_NETHANDLE
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_HANDLE_GetCurrentHandle(void);
//==============================================================================
/**
 * @brief       �l�S�V�G�[�V�����J�n��e�ɑ��M����
 * @return      ���M�ɐ���������TRUE
 */
//==============================================================================
extern BOOL GFL_NET_HANDLE_RequestNegotiation(void);

#endif //__NET_HANDLE_H__



