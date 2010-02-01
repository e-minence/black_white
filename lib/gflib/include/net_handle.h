#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
/**
 * @file	net_handle.h
 * @brief	�ʐM�n���h���̊֐�
 * @author  GAME FREAK Inc. k.ohno
 * @date    2008.2.26
 */
//=============================================================================
#pragma once

typedef struct _GFL_NETHANDLE2 GFL_NETHANDLE2;


typedef void (FUNC2)(GFL_NETHANDLE2* handle);


///<  @brief  �l�b�g���[�N�n���h��
struct _GFL_NETHANDLE2{
  void* pInfomation;       ///<���[�U�[���m����������f�[�^�̃|�C���^
  FUNC2* pCallback;
};



typedef struct{
	u8 child_netid;				    ///< �ؒf����ID
  u8 bSend;  ///���������ǂ���
} NEGOTIATION_RESET_DATA;

///<  @brief  �l�b�g���[�N�n���h��
struct _GFL_NETHANDLE{
  void* pInfomation;       ///<���[�U�[���m����������f�[�^�̃|�C���^
  int infomationSize;      ///< ���[�U�[���m����������f�[�^�̃T�C�Y
  void* negoBuff[GFL_NET_HANDLE_MAX];       ///< �q�@����̎󂯎��L�^
  NEGOTIATION_RESET_DATA aResetBuff[GFL_NET_HANDLE_MAX];
  u16 negotiationBit;  ///< ���̐ڑ����������ǂ��� 0�Ȃ狖��
  u16 negotiationReturn;  ///< ���̐ڑ����������ǂ��� 0�Ȃ狖��
  u16 timingSyncMy;     ///< ������������NO
  u16 timingRecv;       ///< �ʐM�Ŏ󂯎�����ԍ�
  u16 timingSyncEnd;    ///< �����R�}���h�p
  u16 timingR[(GFL_NET_HANDLE_MAX / 4) * 4];       ///< �q�@����̎󂯎��L�^padding
  u8 timingSendFlg;      ///< ���������ǂ���
  u8 negotiationType;        ///< �ڑ����Ă���n���h���̏��
  u8 negotiationSendFlg;      ///< ���������ǂ���
  u8 serviceNo;              ///< �ʐM�T�[�r�X�ԍ�
  u8 version;                ///< �ʐM����̒ʐM�o�[�W�����ԍ�
  u8 bSendInfomation;   ///< �C���t�H���[�V�����f�[�^�𑗂�t���O
  u8 dummy[2];
};


/// @brief  �ʐM�c�[�����[�N
typedef struct _NET_TOOLSYS_t NET_TOOLSYS;


typedef enum {
  GFL_NET_NEG_TYPE_FIRST,
  GFL_NET_NEG_TYPE_SEND_BEFORE,
  GFL_NET_NEG_TYPE_SEND,
  GFL_NET_NEG_TYPE_SEND_AFTER,
  GFL_NET_NEG_TYPE_RECV,
  GFL_NET_NEG_TYPE_CONNECT,       ///< �ڑ���e���F�؂����ꍇ CONNECT�ɂȂ� ���̃R�}���h��͒ʐM�\�ɂȂ�
  GFL_NET_NEG_TYPE_END,           ///< �ؒf���
  GFL_NET_NEG_TYPE_TYPE_ERROR,           ///< �G���[�ł���
  GFL_NET_NEG_TYPE_NONE,

} GFL_NET_NEGOTIATION_TYPE_E;

//==============================================================================
/**
 * @brief   �ʐM�n���h��������������
 * @param   netID    �ʐMID
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_InitHandle(int netID);

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
 * @brief   �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @param   no   �^�C�~���O��肽���ԍ�
 * @param   networkDefId   ����ID
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_HANDLE_TimeSyncStart(GFL_NETHANDLE* pNet, const u8 no, const u8 networkDefId);

//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param   pNet   �l�b�g���[�N�n���h��
 * @param   no     ��������ԍ�
 * @param   networkDefId   ����ID
 * @retval  TRUE   �͂��Ă���
 * @retval  FALSE  �͂��Ă��Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_HANDLE_IsTimeSync(const GFL_NETHANDLE* pNet, const u8 no, const u8 networkDefId);

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
 * @brief       �l�S�V�G�[�V�����J�n��e�ɑ��M����
 * @return      ���M�ɐ���������TRUE
 */
//==============================================================================
extern BOOL GFL_NET_HANDLE_RequestNegotiation(void);
//==============================================================================
/**
 * @brief       id�ɉ�����netHandle���o��
 * @param       netID    id
 * @return      netHandle
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_GetNetHandle(int netID);
//==============================================================================
/**
 * @brief       �ڑ��n���h��������  �ʐM�����ŒʐM�ؒf���Ɏg��
 * @param       netID  �������T�[�o�͎̂󂯕t���Ȃ�
 * @return      none
 */
//==============================================================================
extern void GFI_NET_HANDLE_Delete(int netID);
//==============================================================================
/**
 * @brief   �C���t�H���[�V�����p�R�[���o�b�N GFL_NET_CMD_INFOMATION
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_HANDLE_RecvInfomation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


//-------------------------------------------------------------------------
/**
 * @brief   GFL_NETHANDLE�̃C���t�H���[�V�����𓾂�
 * @param   GFL_NETHANDLE
 * @retval  INFOMATION�f�[�^
 */
//-------------------------------------------------------------------------
extern void* GFL_NET_HANDLE_GetInfomationData(const GFL_NETHANDLE* pNetHandle);

//-------------------------------------------------------------------------
/**
 * @brief   GFL_NETHANDLE�̃C���t�H���[�V�����T�C�Y�𓾂�
 * @param   GFL_NETHANDLE
 * @retval  INFOMATION�T�C�Y
 */
//-------------------------------------------------------------------------
extern int GFL_NET_HANDLE_GetInfomationDataSize(const GFL_NETHANDLE* pNetHandle);
//------------------------------------------------------------------------------
/**
 * @brief       ���݂̃l�S�V�G�[�V�������ʂ�����
 * @param[in]   pHandle   �ʐM�n���h��
 * @return      GFL_NET_NEGOTIATION_TYPE_E
 */
//------------------------------------------------------------------------------
extern GFL_NET_NEGOTIATION_TYPE_E GFL_NET_HANDLE_GetNegotiationType(GFL_NETHANDLE* pHandle);

//==============================================================================
/**
 * @brief       �l�S�V�G�[�V�������������鎖���q�@�ɑ��M����
 * @param[in]   �ʐM��������ID
 * @return      none
 */
//==============================================================================
extern void GFL_NET_HANDLE_RequestResetNegotiation(int netID);

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p���Z�b�g�R�[���o�b�N GFL_NET_CMD_NEGOTIATION_RESET
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_HANDLE_RecvNegotiationReset(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


#ifdef __cplusplus
} /* extern "C" */
#endif
