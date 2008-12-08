//=============================================================================
/**
 * @file	net_system.h
 * @brief	�ʐM�V�X�e��
 * @author	Katsumi Ohno
 * @date    2005.07.08
 */
//=============================================================================

#ifndef _NET_SYSTEM_H_
#define _NET_SYSTEM_H_

#include "net.h"
#include "net_def.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "net_handle.h"


extern BOOL _commSystemInit(int packetSizeMax, HEAPID heapID);
extern void GFL_NET_UpdateSystem( MPDSContext *DSContext );


// �֐��؂�o���������� funccut.rb  k.ohno 2006.12.5 
//==============================================================================
/**
 * @brief   WiFiGame�̏��������s��
 * @param   packetSizeMax �p�P�b�g�̃T�C�Y�}�b�N�X
 * @param   regulationNo  �Q�[���̎��
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemWiFiModeInit(int packetSizeMax, HEAPID heapIDSys, HEAPID heapIDWifi);
//==============================================================================
/**
 * @brief   wifi�̃Q�[�����J�n����
 * @param   target:   ���Ȃ�e�A�O�ȏ�Ȃ�Ȃ��ɂ����e�@�̔ԍ�
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
extern int GFL_NET_SystemWifiApplicationStart( int target );
//==============================================================================
/**
 * @brief   IRC�ʐM�V�X�e���̏��������s��
 * @param   bTGIDChange   �V�K�̃Q�[���̏������̏ꍇTRUE �Â��r�[�R���ł̌듮���h�����ߗp
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIrcModeInit(BOOL bChangeTGID);
//==============================================================================
/**
 * @brief   �e�@�̏��������s��
 * @param   work_area     �V�X�e���Ŏg���������[�̈�
 *                        NULL�̏ꍇ���łɏ������ς݂Ƃ��ē���
 * @param   bTGIDChange   �V�K�̃Q�[���̏������̏ꍇTRUE �Â��r�[�R���ł̌듮���h�����ߗp
 * @param   packetSizeMax �p�P�b�g�T�C�Y
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
extern BOOL GFI_NET_SystemParentModeInit(BOOL bTGIDChange, int packetSizeMax);
//==============================================================================
/**
 * @brief   �e�@�̊m�����s��
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
extern BOOL GFI_NET_SystemParentModeInitProcess(BOOL channelChange);
//==============================================================================
/**
 * @brief   �q�@�̏��������s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *                      NULL�̏ꍇ�͂��łɏ������ς݂Ƃ��Ĉ���
 * @param   regulationNo  �Q�[���̎��
 * @param   bBconInit  �r�[�R���f�[�^������������̂��ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemChildModeInit(BOOL bBconInit, int packetSizeMax);
//==============================================================================
/**
 * @brief   �q�@�̏��������s��+ �ڑ��ɂ����s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *                      NULL�̏ꍇ�͂��łɏ������ς݂Ƃ��Ĉ���
 * @param   regulationNo  �Q�[���̎��
 * @param   bBconInit  �r�[�R���f�[�^������������̂��ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemChildModeInitAndConnect(BOOL bInit,u8* pMacAddr,int packetSizeMax,_PARENTFIND_CALLBACK pCallback);
//==============================================================================
/**
 * @brief   �ʐM�ؒf���s��
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemFinalize(void);
//==============================================================================
/**
 * @brief   �q�@ index�ڑ�
 * @param   index   �e�̃��X�g��index
 * @retval  �q�@�ڑ���e�@�ɑ�������TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemChildIndexConnect(u16 index,_PARENTFIND_CALLBACK pCallback,GFL_NETHANDLE* pHandle);
//==============================================================================
/**
 * @brief   �ʐM�f�[�^�̍X�V����  �f�[�^�����W
 *    main.c   ����  vblank��ɂ����ɌĂ΂��
 * @param   none
 * @retval  �f�[�^�V�F�A�����O���������Ȃ������ꍇFALSE
 */
//==============================================================================
extern BOOL GFL_NET_SystemUpdateData(void);
//==============================================================================
/**
 * @brief   �ʐM�o�b�t�@���N���A�[����
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemReset(void);
//==============================================================================
/**
 * @brief   DS���[�h�ŒʐM�o�b�t�@���N���A�[����
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemResetDS(void);
//==============================================================================
/**
 * @brief   �ʐM�o�b�t�@���N���A�[����+�r�[�R���̏��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemResetBattleChild(void);
//==============================================================================
/**
 * @brief   �ʐM����M�������ɌĂ΂��R�[���o�b�N
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemRecvCallback(u16 aid, u16 *data, u16 size);
//==============================================================================
/**
 * �ʐM����M�������ɌĂ΂��R�[���o�b�N
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemRecvParentCallback(u16 aid, u16 *data, u16 size);
//==============================================================================
/**
 * @brief   �q�@���̃A�N�V�������s��ꂽ���Ƃ�ݒ�
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemActionCommandSet(void);
//==============================================================================
/**
 * �q�@���M���\�b�h
 * �e�@���f�[�^���q�@�S���ɑ��M����͕̂ʊ֐�
 * @param   command    comm_sharing.h�ɒ�`�������x��
 * @param   data       ���M�������f�[�^ �Ȃ�����NULL
 * @param   size       ���M��    �R�}���h�����̏ꍇ0
 * @param   bFast       ���M��    �R�}���h�����̏ꍇ0
 * @param   myID       ���M����l�̃n���h��no
 * @param   sendID     ���鑊���ID   �S���Ȃ�0xff
 * @param   bSendBuffLock ���M�o�b�t�@���Œ肵�đ�e�ʃf�[�^�𑗐M����
 * @retval  ���M�L���[�ɓ��������ǂ���
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendData(int command, const void* data, int size, BOOL bFast, int myID, int sendID, BOOL bSendBuffLock);
//==============================================================================
/**
 * ���M�o�b�t�@�c��e��
 * @retval  �T�C�Y
 */
//==============================================================================
extern int GFL_NET_SystemGetSendRestSize(void);
//==============================================================================
/**
 * �T�[�o���̑��M�o�b�t�@�c��e��
 * @retval  �T�C�Y
 */
//==============================================================================
extern int GFL_NET_SystemGetSendRestSize_ServerSide(void);
//==============================================================================
/**
 * �ʐM�\��ԂȂ̂��ǂ�����Ԃ� �������R�}���h�ɂ��l�S�V�G�[�V�������܂��̏��
 * @param   �e�q�@��netID
 * @retval  TRUE  �ʐM�\    FALSE �ʐM�ؒf
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsConnect(u16 netID);
//==============================================================================
/**
 * �ʐM�\��Ԃ̐l����Ԃ�
 * @param   none
 * @retval  �ڑ��l��
 */
//==============================================================================
extern int GFL_NET_SystemGetConnectNum(void);
//==============================================================================
/**
 * ���������Ă��邩�ǂ�����Ԃ�
 * @param   none
 * @retval  �������I����Ă�����TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsInitialize(void);
//==============================================================================
/**
 * �T�[�o�[������q�@�ɑ���ꍇ ���M�L���[�ւ̒ǉ�
 * @param   command    comm_sharing.h�ɒ�`�������x��
 * @param   sendNetID
 * @param   data       ���M�������f�[�^ �Ȃ�����NULL
 *                     ���̃f�[�^�͐ÓI�łȂ���΂Ȃ�Ȃ�  �o�b�t�@�ɗ��߂Ȃ�����
 * @param   byte       ���M��    �R�}���h�����̏ꍇ0
 * @retval  ���M�L���[�ɓ��������ǂ���
 */
//==============================================================================
//extern BOOL GFL_NET_SystemSetSendQueue_ServerSide(int command, const void* data, int size);
//==============================================================================
/**
 * �N���C�A���g������e�@�ɑ���ꍇ ���M�L���[�ւ̒ǉ�
 * @param   command    comm_sharing.h�ɒ�`�������x��
 * @param   data       ���M�������f�[�^ �Ȃ�����NULL
 *                     ���̃f�[�^�͐ÓI�łȂ���΂Ȃ�Ȃ�  �o�b�t�@�ɗ��߂Ȃ�����
 * @param   byte       ���M��    �R�}���h�����̏ꍇ0
 * @retval  ���M�L���[�ɓ��������ǂ���
 */
//==============================================================================
extern BOOL GFL_NET_SystemSetSendQueue(int command, const void* data, int size);
//==============================================================================
/**
 * DS�ʐMMP�ʐM�̐؂�ւ�  GFL_NET_CMD_DSMP_CHANGE
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================
extern void GFL_NET_SystemRecvDSMPChange(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * DS�ʐMMP�ʐM�̐؂�ւ�
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================
extern void GFL_NET_SystemRecvDSMPChangeReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * DS�ʐMMP�ʐM�̐؂�ւ� �I������ GFL_NET_CMD_DSMP_CHANGE_END
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================
extern void GFL_NET_SystemRecvDSMPChangeEnd(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * �����̋@��ID��Ԃ�
 * @param   
 * @retval  �����̋@��ID  �Ȃ����Ă��Ȃ��ꍇCOMM_PARENT_ID
 */
//==============================================================================
extern u16 GFL_NET_SystemGetCurrentID(void);
//==============================================================================
/**
 * WH���C�u�����Ł@�ʐM��Ԃ�BIT���m�F
 * @param   none
 * @retval  �ڑ����킩��BIT�z��
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsChildsConnecting(void);
//==============================================================================
/**
 * �G���[��Ԃ��ǂ���
 * @param   none
 * @retval  �G���[�̎�TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsError(void);
//==============================================================================
/**
 * ��l�ʐM���[�h��ݒ�
 * @param   bAlone    ��l�ʐM���[�h
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetAloneMode(BOOL bAlone);
//==============================================================================
/**
 * ��l�ʐM���[�h���ǂ������擾
 * @param   none
 * @retval  ��l�ʐM���[�h�̏ꍇTRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemGetAloneMode(void);
//==============================================================================
/**
 * �����I���R�}���h��M
 * @param   netID  �ʐMID
 * @param   size   �T�C�Y
 * @param   pData  �f�[�^
 * @param   pWork  ���[�N
 * @return  none
 */
//==============================================================================
extern void GFL_NET_SystemRecvAutoExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * �f�o�b�O�p�Ƀ_���v��\������
 * @param   adr           �\���������A�h���X
 * @param   length        ����
 * @param   pInfoStr      �\�����������b�Z�[�W
 * @retval  �T�[�r�X�ԍ�
 */
//==============================================================================
extern void GFL_NET_SystemDump_Debug(u8* adr, int length, char* pInfoStr);
//==============================================================================
/**
 * ����̃R�}���h�𑗐M���I�������ǂ����𒲂ׂ� �N���C�A���g��
 * @param   command ���ׂ�R�}���h
 * @retval  �R�}���h���݂�����TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsSendCommand(int command, int myID);
//==============================================================================
/**
 * �L���[������ۂ��ǂ��� �T�[�o�[��
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsEmptyQueue_ServerSize(void);
//==============================================================================
/**
 * �L���[������ۂ��ǂ���
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsEmptyQueue(void);
//==============================================================================
/**
 * wifi�ڑ��������ǂ�����ݒ肷��
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//==============================================================================
extern void GFL_NET_SystemSetWifiConnect(BOOL bConnect);
//==============================================================================
/**
 * �퓬�ɓ���O�̓G�����̗����ʒu��ݒ�
 * @param   no   �����Ă����ʒu�̔ԍ��ɒ���������
 * @param   netID   �ʐM��ID
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetStandNo(int no,int netID);
//==============================================================================
/**
 * �퓬�ɓ���O�̓G�����̗����ʒu�𓾂�
 * @param   netID �ʐM��ID
 * @retval  �����Ă����ʒu�̔ԍ��ɒ���������  0-3  0,2 vs 1,3
 */
//==============================================================================
extern int GFL_NET_SystemGetStandNo(int netID);
//==============================================================================
/**
 * VCHAT��Ԃ��ǂ���
 * @param   none
 * @retval  VCHAT��ԂȂ�TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsVChat(void);
//==============================================================================
/**
 * WIFI�ʐM�𓯊��ʐM���邩�A�񓯊��ʐM���邩�̐؂�ւ����s��
 * @param   TRUE ���� FALSE �񓯊�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetWifiBothNet(BOOL bFlg);
//==============================================================================
/**
 * �G���[�ɂ���ꍇTRUE
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetError(void);
//==============================================================================
/**
 * �ʐM���~�߂�
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemShutdown(void);
//==============================================================================
/**
 * �T�[�o�[���̒ʐM�L���[�����Z�b�g����
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemResetQueue_Server(void);
//==============================================================================
/**
 * �ʐM�̎�M���~�߂�t���O���Z�b�g
 * @param   bFlg  TRUE�Ŏ~�߂�  FALSE�ŋ���
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemRecvStop(BOOL bFlg);
//==============================================================================
/**
 * @brief   �L�[�V�F�A�����O�@�\�ŃL�[�𓾂�
 * @retval  TRUE  �f�[�^�V�F�A�����O����
 * @retval  FALSE �f�[�^�V�F�A�����O���s
 */
//==============================================================================

extern BOOL GFL_NET_SystemGetKey(int no, u16* key);
//==============================================================================
/**
 * @brief   �w�肳�ꂽID�̃n���h����Ԃ�
 * @param   NetID  �l�b�g�h�c
 * @retval  GFL_NETHANDLE
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_SystemGetHandle(int NetID);

//extern void _commRecvCallback(u16 aid, u16 *data, u16 size);
//extern BOOL _commSystemInit(int packetSizeMax, HEAPID heapID);



typedef struct{
    int dataPoint; // ��M�o�b�t�@�\�񂪂���R�}���h�̃X�^�[�g�ʒu
    u8* pRecvBuff; // ��M�o�b�t�@�\�񂪂���R�}���h�̃X�^�[�g�ʒu
    u16 realSize;   /// �ʐM��ɂ��������̃p�P�b�g�̃T�C�Y
    u16 tblSize;   /// �e�[�u����̓o�^�T�C�Y
    u8 valCommand;  /// �R�}���h
    u8 sendID;      /// ���M��ID
    u8 recvBit;      /// ��MID
} _RECV_COMMAND_PACK;

typedef struct{
    /// ----------------------------�q�@�p���e�@�pBUFF
    u8* sSendBuf;          ///<  �q�@�̑��M�p�o�b�t�@
    u8* sSendBufRing;  ///<  �q�@�̑��M�����O�o�b�t�@
    u8* sSendServerBuf;          ///<  �e�@�̑��M�p�o�b�t�@
    u8* sSendServerBufRing;
    u8* pServerRecvBufRing;       ///< �e�@����M�o�b�t�@
    u8* pRecvBufRing;             ///< �q�@���󂯎��o�b�t�@
    u8* pTmpBuff;                 ///< ��M�󂯓n���̂��߂̈ꎞ�o�b�t�@�|�C���^
    //----ringbuff manager
    RingBuffWork sendRing;
    RingBuffWork recvRing;                      ///< �q�@�̎�M�����O�o�b�t�@
    RingBuffWork sendServerRing;
    RingBuffWork recvServerRing[GFL_NET_MACHINE_MAX];
    ///---quemanager �֘A
    SEND_QUEUE_MANAGER sendQueueMgr;
    SEND_QUEUE_MANAGER sendQueueMgrServer;
    ///---��M�֘A
    _RECV_COMMAND_PACK recvCommServer[GFL_NET_MACHINE_MAX];
    _RECV_COMMAND_PACK recvCommClient;
    
    //---------  �����֘A
    BOOL bWifiSendRecv;   // WIFI�̏ꍇ��������鎞�ƂƂ�Ȃ��Ƃ����K�v�Ȃ̂� �؂蕪����
    volatile int countSendRecv;   // ��������{�󂯎������| ��
    volatile int countSendRecvServer[GFL_NET_MACHINE_MAX];   // ��������{�󂯎������| ��

    //-------
    int packetSizeMax;
    u16 bitmap;   // �ڑ����Ă���@���BIT�Ǘ�
    
    //-------------------
//    NET_TOOLSYS* pTool;  ///< netTOOL�̃��[�N
//    GFL_NETHANDLE* pNetHandle[GFL_NET_MACHINE_MAX];
  //  u16 key[GFL_NET_MACHINE_MAX];
//    UI_KEYSYS* pKey[GFL_NET_MACHINE_MAX];       ///<  �L�[�V�F�A�����O�|�C���^
//    u8 device;   ///< �f�o�C�X�؂�ւ� wifi<>wi

#if _COUNT_TEST
    u8 DSCount; // �R�}���h�̏��Ԃ����������ǂ����m�F�p
    u8 DSCountRecv[GFL_NET_MACHINE_MAX];
#endif
//    u8 recvDSCatchFlg[GFL_NET_MACHINE_MAX];  // �ʐM������������Ƃ��L�� DS�����p

    u8 bFirstCatch[GFL_NET_MACHINE_MAX];  // �R�}���h���͂��߂Ă���������p
    u8 bFirstCatchP2C;    ///< �ŏ��̃f�[�^����M�����^�C�~���O��TRUE�ɂȂ�t���O

    u8 bNextSendData;  ///
    u8 bNextSendDataServer;  ///
    u8 bAlone;    // ��l�ŒʐM�ł���悤�ɂ��郂�[�h�̎�TRUE
    u8 bWifiConnect; //WIFI�ʐM�\�ɂȂ�����TRUE
    u8 bResetState;
    u8 bError;  // �����s�\�Ȏ���TRUE
    u8 bShutDown;
    u8 bDSSendOK;   //DS�ʐM�Ŏ��𑗐M���Ă悢�ꍇTRUE �O��G���[�őO�̂𑗂�ꍇFALSE
} _COMM_WORK_SYSTEM;


#endif // _NET_SYSTEM_H_

