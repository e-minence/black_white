//=============================================================================
/**
 * @file	network.h
 * @brief	�ʐM���C�u�����̊O�����J�֐�
 * @author	k.ohno
 * @date    2006.11.4
 */
//=============================================================================
#ifndef _NETWORK_H_
#define _NETWORK_H_

// define 
#define NET_NETID_ALLUSER (0xff)  ///< NetID:�S���֑��M����ꍇ
#define NET_NETID_SERVER (0xfe)   ///< NetID:�T�[�o�[�̏ꍇ���� ���0����ClientID

// typedef
typedef struct u8 GameServiceID;  ///< �Q�[���T�[�r�XID  �ʐM�̎��
typedef struct u8 ConnectID;      ///< �ڑ����邽�߂�ID  0-16 �܂�
typedef struct u8 NetID;          ///< �ʐMID  0-200 �܂�

/// �ʐM�Ǘ��\����
typedef struct _GFNetHandle GFNetHandle;

/// ���M�������󂯎��R�[���o�b�N�^
typedef CBSendEndFunc;



typedef struct{             ///< �ʐM�̏������p�\����
  GameServiceID gsid;       ///< �Q�[���T�[�r�XID  �ʐM�̎��
  u32  allocNo;             ///< alloc���邽�߂̔ԍ�
  NetRecvFuncTable *pRecvFuncTable;  ///< ��M�֐��e�[�u��
  NetBeaconGetFunc *pBeaconGetFunc;  ///< �r�[�R���f�[�^�擾�֐�
  NetErrorFunc *pErrorFunc;           ///< �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NetFatalErrorFunc *pFatalErrorFunc; ///< �ً}��~�G���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NetConnectEndFunc *pConnectEndFunc;  ///< �ʐM�ؒf���ɌĂ΂��֐�
  NetDSMPSwitchEndFunc *pSDMPSwitchEndFunc;  ///< DS<>MP�؂�ւ��������ɌĂ΂��
  u8 maxConnectNum;   ///< �ő�ڑ��l��
  u8 maxBeaconNum;    ///< �ő�r�[�R�����W��

} GFNetInitializeStruct;

//-------------------------------
//  �֐� �`�O�����J�֐��͑S�Ă����ɒ�`
//-------------------------------

//==============================================================================
/**
 * �ʐM������
 * @param[in]   NetInitializeStruct*  pNetInit  �ʐM�������\���̂̃|�C���^
 * @retval  void
 */
//==============================================================================
extern void GF_NT_Initialize(const NetInitializeStruct* pNetInit);

//==============================================================================
/**
 * �ʐM�I��
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  void
 */
//==============================================================================
extern void GF_NT_Finalize(NetHandle* pNet);

//-----�r�[�R���E�Ƃ��������֘A
//==============================================================================
/**
 * �r�[�R�����󂯎��T�[�r�X��ǉ��w��  �i�������̃T�[�r�X�̃r�[�R�����E���������Ɂj
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   GameServiceID gsid  �Q�[���T�[�r�XID  �ʐM�̎��
 * @retval  none
 */
//==============================================================================
extern void GF_NT_AddBeaconServiceID(NetHandle* pNet, GameServiceID gsid);

//==============================================================================
/**
 * ��M�r�[�R���f�[�^�𓾂�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   GameServiceID gsid  �Q�[���T�[�r�XID  �ʐM�̎��
 * @retval  �r�[�R���f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
extern void* GF_NT_GetBeaconData(NetHandle* pNet, ConnectID id);


//--------�ڑ��E�ؒf
//==============================================================================
/**
 * �q�@�ɂȂ�ڑ�����
 * @retval  NetHandle* pNet     �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern NetHandle* pNet = GF_NT_ClientConnect(void);

//==============================================================================
/**
 * �e�@�ɂȂ�҂��󂯂�
 * @return  NetHandle* pNet     �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern NetHandle* pNet = GF_NT_ServerConnect(void);

//==============================================================================
/**
 * �e�q�؂�ւ��ڑ����s��
 * @return  NetHandle* pNet     �ʐM�n���h���̃|�C���^
 */
//==============================================================================
extern NetHandle* pNet = GF_NT_SwitchConnect(void);

//==============================================================================
/**
 * ���̃n���h���̒ʐM�ԍ��𓾂�
 * @param[in,out]  NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  NetID     �ʐMID
 */
//==============================================================================
extern NetID GF_NT_GetMyNetID(NetHandle* pNet);

//==============================================================================
/**
 * ���݂̐ڑ��l���𓾂�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  int  �ڑ���
 */
//==============================================================================
extern int GF_NT_GetConnectNum(NetHandle* pNet);

//==============================================================================
/**
 * ID�̒ʐM�Ɛڑ����Ă��邩�ǂ�����Ԃ�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   NetID id            �l�b�gID
 * @retval  BOOL  �ڑ����Ă�����TRUE
 */
//==============================================================================
extern BOOL GF_NT_IsConnectMember(NetHandle* pNet,NetID id);

//==============================================================================
/**
 * �ʐM�ؒf����
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  none
 */
//==============================================================================
//
extern void GF_NT_Disconnect(NetHandle* pNet);

//==============================================================================
/**
 * �ڑ������ǂ���
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  none
 */
//==============================================================================
extern BOOL GF_NT_IsConnect(NetHandle* pNet);

//==============================================================================
/**
 * �ڑ����ύX
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   u8 num     �ύX��
 * @retval  none
 */
//==============================================================================
extern void GF_NT_SetClientConnectNum(NetHandle* pNet,u8 num);

//==============================================================================
/**
 * �V�K�ڑ��֎~������ݒ�
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   BOOL bEnable     TRUE=�ڑ����� FALSE=�֎~
 * @retval  none
 */
//==============================================================================
extern void GF_NT_SetClientConnect(NetHandle* pNet,BOOL bEnable);


//--------���M

//==============================================================================
/**
 * ���M�J�n
 * @brief �S���ɖ������ő��M  ���M�T�C�Y�͏��������̃e�[�u����������o��
 *        �f�[�^�͕K���R�s�[����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   u16 sendCommand                ���M����R�}���h
 * @param[in]   void* data                     ���M�f�[�^�|�C���^
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
extern BOOL GF_NT_SendData(NetHandle* pNet,const u16 sendCommand,const void* data);

//==============================================================================
/**
 * ���M�J�n
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   u16 sendCommand                ���M����R�}���h
 * @param[in]   NetID sendID                     ���M���� �S���֑��M����ꍇ NET_SENDID_ALLUSER
 * @param[in]   CBSendEndFunc* pCBSendEndFunc  ���M������������R�[���o�b�N�֐��̓o�^
 * @param[in]   u32 size                       ���M�f�[�^�T�C�Y
 * @param[in]   void* data                     ���M�f�[�^�|�C���^
 * @param[in]   BOOL bDataCopy                 �f�[�^���R�s�[����ꍇTRUE
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
extern BOOL GF_NT_SendData(NetHandle* pNet,const NetID sendID,
                    const CBSendEndFunc* pCBSendEndFunc,const u32 size,
                    const void* data, const BOOL bDataCopy);

//==============================================================================
/**
 * ���M�f�[�^���������ǂ���
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @retval  TRUE   �����ꍇ
 * @retval  FALSE  �c���Ă���ꍇ
 */
//==============================================================================
extern BOOL GF_NT_IsEmptySendData(NetHandle* pNet);

//--------����
// 
//==============================================================================
/**
 * �����ʐM�ɐ؂�ւ�����e�q�ʐM�ɐ؂�ւ����肷��
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   BOOL bDSSwitch  TRUE=DS�ɐ؂�ւ��� FALSE=MP�ɐ؂�ւ���
 * @return  none
 */
//==============================================================================
extern void GF_NT_DSMPSwitch(NetHandle* pNet, const BOOL bDSSwitch);


//--------���̑��A�c�[����
//==============================================================================
/**
 * �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   u8 no   �^�C�~���O��肽���ԍ�
 * @return      none
 */
//==============================================================================
extern void GF_NT_TimingSyncStart(NetHandle* pNet, const u8 no);
//==============================================================================
/**
 * �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   no   �͂��ԍ�
 * @retval  TRUE    �^�C�~���O�����v
 * @retval  FALSE   �^�C�~���O��������Ă��Ȃ�
 */
//==============================================================================
extern BOOL GF_NT_IsTimingSync(NetHandle* pNet, const u8 no);


#endif //_NETWORK_H_

