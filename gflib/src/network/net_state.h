//=============================================================================
/**
 * @file	net_state.h
 * @brief	�ʐM��Ԃ��Ǘ�����T�[�r�X  �ʐM�̏�ʂɂ���
 *          �X���b�h�̂ЂƂƂ��ē����A�����̒ʐM��Ԃ⑼�̋@���
 *          �J�n��I�����Ǘ�����
 *          communication�����݂̂��Ĉړ�
 * @author	k.ohno
 * @date    2006.01.12
 */
//=============================================================================

#ifndef __NET_STATE_H__
#define __NET_STATE_H__

// �֐��؂�o���������� funccut.rb  k.ohno 2006.12.5 
//==============================================================================
/**
 * @brief   �ʐM�f�o�C�X������������
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateDeviceInitialize(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   �}�b�N�A�h���X���w�肵�Ďq�@�ڑ��J�n
 * @param   connectIndex �ڑ�����e�@��Index
 * @param   bAlloc       �������[�̊m��
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateConnectMacAddress(GFL_NETHANDLE* pNetHandle,BOOL bInit);
//==============================================================================
/**
 * @brief   �q�@�J�n  �r�[�R���̎��W�ɓ���
 * @param   connectIndex �ڑ�����e�@��Index
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateBeaconScan(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̏���
 * @param
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateMainProc(GFL_NETHANDLE* pHandle);
//==============================================================================
/**
 * @brief   �e�Ƃ��Ă̒ʐM�����J�n
 * @param   pNetHandle  �n���h��
 * @param   heapID      HEAPID
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateCreateParent(GFL_NETHANDLE* pNetHandle,HEAPID heapID);
extern void GFL_NET_StateConnectParent(GFL_NETHANDLE* pNetHandle,HEAPID heapID);
//==============================================================================
/**
 * @brief   �e�q�����؂�ւ��ʐM�����J�n
 * @param   pNetHandle  �n���h��
 * @param   heapID      HEAPID
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateChangeoverConnect(GFL_NETHANDLE* pNetHandle,HEAPID heapID);
//==============================================================================
/**
 * @brief   �I���R�}���h �q�@���e�@�ɂ�߂�悤�ɑ��M  �S���̎q�@�ɑ���Ԃ�GFL_NET_CMD_EXIT_REQ
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateRecvExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   �I���R�}���h   �q�@���e�@�ɂ�߂�悤�ɑ��M �Ԃ��肫�� GFL_NET_CMD_EXIT
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateRecvExitStart(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateRecvNegotiation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION_RETURN
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateRecvNegotiationReturn(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   �ʐM�����I���葱���J�n
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateExit(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   DSMP���[�h�ύX��Main�֐�
 * @param   pNet  �ʐM�n���h��
 * @return  noen
 */
//==============================================================================
extern void GFL_NET_StateTransmissonMain(GFL_NETHANDLE* pNet);
//==============================================================================
/**
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ�  GFL_NET_CMD_DSMP_CHANGE
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================
extern void GFL_NET_StateRecvDSMPChange(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ�
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================
extern void GFL_NET_StateRecvDSMPChangeReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ� �I������ GFL_NET_CMD_DSMP_CHANGE_END
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================
extern void GFL_NET_StateRecvDSMPChangeEnd(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   WIFI�w��ڑ����J�n����
 * @param   pNetHandle   GFL_NETHANDLE
 * @param   target    �Ȃ��ɂ����e�@�̔ԍ�
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
extern int GFL_NET_StateStartWifiPeerMatch( GFL_NETHANDLE* pNetHandle, int target );
//==============================================================================
/**
 * @brief   WIFI�����_���}�b�`���J�n����
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
extern int GFL_NET_StateStartWifiRandomMatch( GFL_NETHANDLE* pNetHandle );
//==============================================================================
/**
 * @brief   �e�Ƃ��Ă̒ʐM�����J�n
 * @param   pNetHandle  �n���h��
 * @param   heapID      HEAPID
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateConnectWifiParent(GFL_NETHANDLE* pNetHandle,HEAPID heapID);
//==============================================================================
/**
 * @brief   �}�b�`���O�������������ǂ����𔻒�
 * @param   pNetHandle   GFL_NETHANDLE
 * @param   target: ���Ȃ�e�A�O�ȏ�Ȃ�Ȃ��ɂ����e�@�̔ԍ�
 * @retval  1. �����@�@0. �ڑ���   2. �G���[��L�����Z���Œ��f
 */
//==============================================================================
extern int GFL_NET_StateWifiIsMatched(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   WIFI�Ń}�b�`���O��؂�ꍇ�̏���
 * @param   bAuto  �������狭���ؒf����Ƃ���FALSE �ڑ��󋵂ɂ��킹��ꍇTRUE
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateWifiMatchEnd(BOOL bAuto, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   WIFI�I���R�}���h   �q�@���e�@�ɂ�߂�悤�ɑ��M �Ԃ��肫�� CS_WIFI_EXIT
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateRecvWifiExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   WIFI�Ń}�b�`���O��؂�ꍇ�̏��� �ؒf�R�}���h�𑗐M
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateSendWifiMatchEnd(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   Wifi�ؒf�R�}���h���󂯎�����ꍇTRUE
 * @param   none
 * @retval  WifiLogout�Ɉڍs�����ꍇTRUE
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiDisconnect(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   WifiLogin�Ɉڍs�������ǂ����̊m�F
 * @param   none
 * @retval  WifiLogin�Ɉڍs�����ꍇTRUE
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiLoginState(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   WifiLoginMatch�Ɉڍs�������ǂ����̊m�F
 * @param   none
 * @retval  WifiLoginMatch�Ɉڍs�����ꍇTRUE
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiLoginMatchState(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   WifiError�̏ꍇ���̔ԍ���Ԃ�  �G���[�ɂȂ��Ă邩�ǂ������m�F���Ă�������o������
 * @param   none
 * @retval  Error�ԍ�
 */
//==============================================================================
extern int GFL_NET_StateGetWifiErrorNo(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   WIFI�Ń��O�A�E�g���s���ꍇ�̏�
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateWifiLogout(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   WifiError���ǂ���
 * @param   none
 * @retval  Error
 */
//==============================================================================
extern BOOL GFL_NET_StateIsWifiError(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   wifi�}�b�`���O��Ԃփ��O�C������
 * @param   pNetHandle  GFL_NETHANDLE
 * @param   netHeapID   netLib�Ŋm�ێg�p����ID
 * @param   wifiHeapID  NitroWifi&NitroDWC&VCT�Ŋm�ۂ��郁����
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateWifiEnterLogin(GFL_NETHANDLE* pNetHandle, HEAPID netHeapID, HEAPID wifiHeapID);
//--------------------------errfunc------------------------------------------------

//==============================================================================
/**
 * @brief   �����I�ɃG���[���o���s�����ǂ���
 *          (TRUE�̏ꍇ�G���[���݂���ƃu���[�X�N���[���{�ċN���ɂȂ�)
 * @param   bAuto  TRUE�Ō����J�n FALSE�ŃG���[�������s��Ȃ�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_STATE_SetAutoErrorCheck(GFL_NETHANDLE* pNetHandle,BOOL bAuto);
//==============================================================================
/**
 * @brief   �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ肷��
 * @param   bFlg    �ؒf=�G���[�ɂ���
 * @param   bAuto  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_STATE_SetNoChildErrorCheck(GFL_NETHANDLE* pNetHandle,BOOL bFlg);


#define _ERR_DEFINE  (0)
#if _ERR_DEFINE

//==============================================================================
/**
 * @brief   ���������G���[�̌�����ʐM���������邩�ǂ�����ݒ�
 * @param   bFlg  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================
extern BOOL CommStateGetErrorCheck(GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * @brief   �G���[��Ԃɓ���
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void CommSetErrorReset(GFL_NETHANDLE* pNetHandle,u8 type);
//==============================================================================
/**
 * @brief   ���Z�b�g�G���[��ԂɂȂ������ǂ����m�F����
 * @param   none
 * @retval  ���Z�b�g�G���[��ԂȂ�TRUE
 */
//==============================================================================
extern u8 CommIsResetError(GFL_NETHANDLE* pNetHandle);
//--------------------------------------------------------------
/**
 * @brief   fatal�G���[�p�֐�  ���̊֐������甲�����Ȃ�
 * @param   no   �G���[�ԍ�
 * @retval  none
 */
//--------------------------------------------------------------
extern void CommFatalErrorFunc(int no);
//--------------------------------------------------------------
/**
 * @brief   �G���[�����m���� �ʐM�G���[�p�E�C���h�E���o��
 * @param   heapID    �������[�m�ۂ���HEAPID
 * @param   bgl       GF_BGL_INI
 * @retval  none
 */
//--------------------------------------------------------------
//extern void CommErrorCheck(int heapID, GF_BGL_INI* bgl);
//--------------------------------------------------------------
/**
 * @brief   �G���[�����m���� �ʐM�G���[�p�E�C���h�E���o��
 * @param   heapID    �������[�m�ۂ���HEAPID
 * @param   bgl       GF_BGL_INI
 * @retval  none
 */
//--------------------------------------------------------------
extern void CommErrorDispCheck(GFL_NETHANDLE* pNetHandle,int heapID);
//--------------------------------------------------------------
/**
 * @brief   ���Z�b�g�ł����Ԃ��ǂ���
 * @param   none
 * @retval  TRUE �Ȃ烊�Z�b�g
 */
//--------------------------------------------------------------
extern BOOL CommStateIsResetEnd(void);
//==============================================================================
/**
 * @brief   �G���[�ɂ���ꍇ���̊֐����ĂԂƃG���[�ɂȂ�܂�
 * @param   �G���[���
 * @retval  �󂯕t�����ꍇTRUE
 */
//==============================================================================
extern BOOL CommStateSetError(GFL_NETHANDLE* pNetHandle,int no);


#define COMM_ERROR_RESET_SAVEPOINT  (1)
#define COMM_ERROR_RESET_TITLE  (2)
#define COMM_ERROR_RESET_GTS  (3)
#define COMM_ERROR_RESET_OTHER (4)


#define COMM_ERRORTYPE_ARESET (0)     // A�{�^���Ń��Z�b�g
#define COMM_ERRORTYPE_POWEROFF (1)   // �d����؂�Ȃ��Ƃ����Ȃ�
#define COMM_ERRORTYPE_TITLE (2)     // A�{�^���Ń��Z�b�g �^�C�g���߂�
#define COMM_ERRORTYPE_GTS (3)     // A�{�^���Ń��Z�b�g �Z�[�u�|�C���g�֖߂�

#endif  //_ERR_DEFINE

#endif //__NET_STATE_H__

