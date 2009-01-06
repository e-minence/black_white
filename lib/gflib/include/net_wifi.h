#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
/**
 * @file	net_wifi.h
 * @brief	WIFI�ʐM���C�u�����̊O�����J�֐�
 * @author	k.ohno
 * @date    2007.2.28
 */
//=============================================================================
#ifndef __NET_WIFI_H__
#define __NET_WIFI_H__


//==============================================================================
/**
 * @brief   Wi-Fi�ڑ������� ���������ɌĂяo���Ă����K�v������
 * @param   heapID     �������̈�
 * @param   errorFunc  �G���[�\���֐�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WifiStart( int heapID, NetErrorFunc errorFunc );

//==============================================================================
/**
 * @brief   Wi-Fi�ݒ��ʂ��Ăяo��
 * @param   heapID  �������̈�
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WifiUtility( int heapID );

//==============================================================================
/**
 * �����_���}�b�`���s���֐�
 * @target   ���c�����ŃQ�[�����J�ÁB�O�ȏ�c�ڑ����ɍs������́A�t�����h���X�g��̈ʒu
 * @retval  ���c�����B�O�c���s�B
 */
//==============================================================================
extern int GFL_NET_DWC_StartMatch( u8* keyStr,int numEntry, BOOL bParent, u32 timelimit );

//==============================================================================
/**
 * aid��Ԃ��܂��B�ڑ�����܂ł�MYDWC_NONE_AID��Ԃ��܂��B
 * @retval  aid�B�������ڑ��O��MYDWC_NONE_AID
 */
//==============================================================================
extern u32 GFL_NET_DWC_GetAid(void);
#define MYDWC_NONE_AID (0xffffffff)

//==============================================================================
/**
 * �Q�[����W�E�Q�����s���֐��B
 * @target   ���c�����ŃQ�[�����J�ÁB�O�ȏ�c�ڑ����ɍs������́A�t�����h���X�g��̈ʒu
 * @retval  ���c�����B�O�c���s�B
 */
//==============================================================================
extern int GFL_NET_DWC_StartGame( int target,int maxnum, BOOL bVCT );
//==============================================================================
/**
 * �ڑ������m�������ɌĂ΂��֐����Z�b�g����
 * @param 	pFunc  �ڑ����ɌĂ΂��֐�
 * @param 	pWork  ���[�N�G���A
 * @retval  none
 */
//==============================================================================
// �ڑ��R�[���o�b�N�^
//  �F�B���w��̏ꍇaid�͎�����aid
//  �F�B�w��̏ꍇaid�͑���̂�aid
typedef void (*MYDWCConnectFunc) (u16 aid,void* pWork);
extern void GFL_NET_DWC_SetConnectCallback( MYDWCConnectFunc pFunc, void* pWork );
//==============================================================================
/**
 * @brief   �]����Ԓ��̐ڑ�����Ԃ�
 * @param   none
 * @retval  ��
 */
//==============================================================================
extern int GFL_NET_DWC_AnybodyEvalNum(void);
//==============================================================================
/**
 * �ؒf�����m�������ɌĂ΂��֐����Z�b�g����
 * @param 	pFunc  �ؒf���ɌĂ΂��֐�
 * @param 	pWork  ���[�N�G���A
 * @retval  none
 */
//==============================================================================
// �ؒf�R�[���o�b�N�^
typedef void (*MYDWCDisconnectFunc) (u16 aid,void* pWork);
extern void GFL_NET_DWC_SetDisconnectCallback( MYDWCDisconnectFunc pFunc, void* pWork );

//==============================================================================
/**
 * DWC�ʐM�����X�V���s��
 * @param none
 * @retval 0�c����, ���c�G���[����
 */
//==============================================================================
extern void GFL_NET_DWC_StartVChat(int heapID);
//==============================================================================
/**
 * @brief   �{�C�X�`���b�g���~���܂�
 * @param   void
 * @retval  void
 */
//==============================================================================
extern void GFL_NET_DWC_StopVChat(void);

// ���łɁA�����f�[�^�����X�g��ɂ���B���̏ꍇ�͉�������K�v�Ȃ��B
#define DWCFRIEND_INLIST 0
// ���łɁA�����f�[�^�����X�g��ɂ��邪�A�㏑�����邱�Ƃ��]�܂����ꍇ�B
#define DWCFRIEND_OVERWRITE 1
// �����f�[�^�̓��X�g��ɂȂ��B
#define DWCFRIEND_NODATA 2
// �f�[�^���������Ȃ��B
#define DWCFRIEND_INVALID 3

//==============================================================================
/**
 * ���łɓ����l���o�^����Ă��Ȃ����B
 * @param   index �����f�[�^�����������Ƃ��́A���̏ꏊ�B
 * @param         ������Ȃ��ꍇ�͋󂢂Ă���Ƃ����Ԃ�
 * @param         �ǂ����󂢂Ă��Ȃ��ꍇ�́A-1 
 * @retval  DWCFRIEND_INLIST �c ���łɁA�����f�[�^�����X�g��ɂ���B���̏ꍇ�͉�������K�v�Ȃ��B
 * @retval  DWCFRIEND_OVERWRITE �c ���łɁA�����f�[�^�����X�g��ɂ��邪�A�㏑�����邱�Ƃ��]�܂����ꍇ�B
 * @retval  DWCFRIEND_NODATA �c �����f�[�^�̓��X�g��ɂȂ��B
 * @retval  DWCFRIEND_INVALID �c �󂯎����DWCFriendData���ُ�B
 */
//==============================================================================
extern int GFL_NET_DWC_CheckFriendByToken(DWCFriendData *data, int *index);

//==============================================================================
/**
 * �G���[�R�[�h��ێ�����
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_StateSetWifiError(int code, int type, int ret);

typedef struct{
  int errorCode;
  int errorType;
  int errorRet;
} GFL_NETSTATE_DWCERROR;

//==============================================================================
/**
 * �G���[�R�[�h�������o��
 * @param   none
 * @retval  none
 */
//==============================================================================
extern GFL_NETSTATE_DWCERROR* GFL_NET_StateGetWifiError(void);



/// WiFi�Ŏg��Heap�̃T�C�Y
#define MYDWC_HEAPSIZE (0x30000)

// ��M�R�[���o�b�N�^�BWHReceiverFunc�Ɠ��`
typedef void (*MYDWCReceiverFunc) (u16 aid, u16 *data, u16 size);

//�ڑ����� �t�����h�R�[�h���X�g�C���f�b�N�X
typedef BOOL (*MYDWCConnectModeCheckFunc) (int index);

#define DWCRAP_STARTGAME_OK (0)
#define DWCRAP_STARTGAME_NOTSTATE  (-1)
#define DWCRAP_STARTGAME_RETRY  (-2)
#define DWCRAP_STARTGAME_FAILED  (-3)
#define DWCRAP_STARTGAME_FIRSTSAVE (-4)

#define STEPMATCH_CONTINUE 0
#define STEPMATCH_SUCCESS  (DWC_ERROR_NUM)
#define STEPMATCH_CANCEL (STEPMATCH_SUCCESS+1)
#define STEPMATCH_FAIL (STEPMATCH_SUCCESS+2)
#define STEPMATCH_TIMEOUT (STEPMATCH_SUCCESS+3)
#define STEPMATCH_DISCONNECT (STEPMATCH_SUCCESS+4)
#define ERRORCODE_0 (STEPMATCH_SUCCESS+5)
#define ERRORCODE_HEAP (STEPMATCH_SUCCESS+6)
#define STEPMATCH_CONNECT (STEPMATCH_SUCCESS+7)


#if GFL_NET_WIFI //wifi
// �������牺�̓f�o�C�X���t�A�N�Z�X����ׂɕK�v�Ȃ��̂Ȃ̂Ŏg�p���Ȃ��ł�������
extern void GFI_NET_NetWifiSaveUserDataFunc(void);
extern void GFI_NET_NetWifiMargeFrinedDataFunc(int deletedIndex,int srcIndex);
extern DWCUserData* GFI_NET_GetMyDWCUserData(void);
extern DWCFriendData* GFI_NET_GetMyDWCFriendData(void);
extern int GFI_NET_GetFriendNumMax(void);

#endif //GFL_NET_WIFI


#endif //__NET_WIFI_H__
#ifdef __cplusplus
} /* extern "C" */
#endif
