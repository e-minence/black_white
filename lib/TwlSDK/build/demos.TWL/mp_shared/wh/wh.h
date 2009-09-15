/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - demos.Twl
  File:     wh.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-20#$
  $Rev: 9005 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/


#ifndef __WMHIGH_H__
#define __WMHIGH_H__

#include "default/wh_config.h"

/* NITRO-SDK3.1RC ����ǉ�. (����ȑO�͏��1) */
#if !defined(WH_MP_FREQUENCY)
#define WH_MP_FREQUENCY   1
#endif

// WM_Initialize���g�p���ď���������ꍇ��OFF
// WM_Init, WM_Enable, WM_PowerOn���ʂɎg�p���čׂ������䂷��K�v������ꍇ�ɂ�ON�ɐݒ肷��B
// #define WH_USE_DETAILED_INITIALIZE

enum
{
    WH_SYSSTATE_STOP,                  // �������
    WH_SYSSTATE_IDLE,                  // �ҋ@��
    WH_SYSSTATE_SCANNING,              // �X�L������
    WH_SYSSTATE_BUSY,                  // �ڑ���ƒ�
    WH_SYSSTATE_CONNECTED,             // �ڑ������i���̏�ԂŒʐM�\�j
    WH_SYSSTATE_DATASHARING,           // data-sharing�L���Őڑ�����
    WH_SYSSTATE_KEYSHARING,            // key-sharing�L���Őڑ�����
    WH_SYSSTATE_MEASURECHANNEL,        // �`�����l���̓d�g�g�p�����`�F�b�N
    WH_SYSSTATE_CONNECT_FAIL,          // �e�@�ւ̐ڑ����s
    WH_SYSSTATE_ERROR,                 // �G���[����
    WH_SYSSTATE_FATAL,                 // FATAL�G���[����
    WH_SYSSTATE_NUM
};

enum
{
    WH_CONNECTMODE_MP_PARENT,          // �e�@ MP �ڑ����[�h
    WH_CONNECTMODE_MP_CHILD,           // �q�@ MP �ڑ����[�h
    WH_CONNECTMODE_KS_PARENT,          // �e�@ key-sharing �ڑ����[�h
    WH_CONNECTMODE_KS_CHILD,           // �q�@ key-sharing �ڑ����[�h
    WH_CONNECTMODE_DS_PARENT,          // �e�@ data-sharing �ڑ����[�h
    WH_CONNECTMODE_DS_CHILD,           // �q�@ data-sharing �ڑ����[�h
    WH_CONNECTMODE_NUM
};

enum
{
    // ���O�̃G���[�R�[�h
    WH_ERRCODE_DISCONNECTED = WM_ERRCODE_MAX,   // �e����ؒf���ꂽ
    WH_ERRCODE_PARENT_NOT_FOUND,       // �e�����Ȃ�
    WH_ERRCODE_NO_RADIO,               // �����g�p�s��
    WH_ERRCODE_LOST_PARENT,            // �e����������
    WH_ERRCODE_NOMORE_CHANNEL,         // ���ׂẴ`�����l���̒������I����
    WH_ERRCODE_MAX
};

typedef void (*WHStartScanCallbackFunc) (WMBssDesc *bssDesc);

/* �e�@��M�o�b�t�@�̃T�C�Y */
#define WH_PARENT_RECV_BUFFER_SIZE  WM_SIZE_MP_PARENT_RECEIVE_BUFFER( WH_CHILD_MAX_SIZE, WH_CHILD_MAX, FALSE )
/* �e�@���M�o�b�t�@�̃T�C�Y */
#define WH_PARENT_SEND_BUFFER_SIZE  WM_SIZE_MP_PARENT_SEND_BUFFER( WH_PARENT_MAX_SIZE, FALSE )

/* �q�@��M�o�b�t�@�̃T�C�Y */
#define WH_CHILD_RECV_BUFFER_SIZE   WM_SIZE_MP_CHILD_RECEIVE_BUFFER( WH_PARENT_MAX_SIZE, FALSE )
/* �q�@���M�o�b�t�@�̃T�C�Y */
#define WH_CHILD_SEND_BUFFER_SIZE   WM_SIZE_MP_CHILD_SEND_BUFFER( WH_CHILD_MAX_SIZE, FALSE )

/* SDK �̃T���v���f���p�ɗ\�񂳂ꂽ GGID ���`����}�N�� */
#define SDK_MAKEGGID_SYSTEM(num)    (0x003FFF00 | (num))

/* ���ڑ�, �܂��͐e�@�������Ȃ���Ԃ������r�b�g�}�b�v�l */
#define WH_BITMAP_EMPTY           1

// ���M�R�[���o�b�N�^ (for data-sharing model)
typedef void (*WHSendCallbackFunc) (BOOL result);

// �ڑ�������֐��^ (for multiboot model)
typedef BOOL (*WHJudgeAcceptFunc) (WMStartParentCallback *);

// ��M�R�[���o�b�N�^
typedef void (*WHReceiverFunc) (u16 aid, u16 *data, u16 size);

// WEP Key �����֐�
typedef u16 (*WHParentWEPKeyGeneratorFunc) (u16 *wepkey, const WMParentParam *parentParam);
typedef u16 (*WHChildWEPKeyGeneratorFunc) (u16 *wepkey, const WMBssDesc *bssDesc);



/**************************************************************************
 * �ȉ��́AWH �̊e��ݒ�l��ύX����֐��ł��B
 **************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WH_SetGgid

  Description:  �Q�[���O���[�vID��ݒ肵�܂��B
                �e�@�̐ڑ��O�ɌĂяo���܂��B

  Arguments:    ggid    �ݒ肷��Q�[���O���[�vID.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetGgid(u32 ggid);

/*---------------------------------------------------------------------------*
  Name:         WH_SetSsid

  Description:  �q�@�ڑ����Ɏw�肷��SSID��ݒ肵�܂��B
                �q�@����̐ڑ��O�ɌĂяo���܂��B

  Arguments:    ssid    �ݒ肷��SSID���i�[���ꂽ�o�b�t�@�B
                length  �ݒ肷��SSID�̃f�[�^���B
                        WM_SIZE_CHILD_SSID(24�o�C�g)�ɖ����Ȃ��ꍇ��
                        �㑱����]����0�Ŗ��߁A����ꍇ�ɂ͐؂�̂Ă܂��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetSsid(const void *ssid, u32 length);

/*---------------------------------------------------------------------------*
  Name:         WH_SetUserGameInfo

  Description:  ���[�U��`�̐e�@����ݒ肵�܂��B
                �e�@�̐ڑ��O�ɌĂяo���܂��B

  Arguments:    userGameInfo  ���[�U��`�̐e�@���ւ̃|�C���^
                length        ���[�U��`�̐e�@���̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetUserGameInfo(u16 *userGameInfo, u16 length);

/*---------------------------------------------------------------------------*
  Name:         WH_SetDebugOutput

  Description:  �f�o�b�O������o�͗p�̊֐���ݒ肵�܂��B

  Arguments:    func    �ݒ肷��f�o�b�O������o�͗p�̊֐�.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WH_SetDebugOutput(void (*func) (const char *, ...));

/*---------------------------------------------------------------------------*
  Name:         WH_SetParentWEPKeyGenerator

  Description:  WEP Key �𐶐�����֐���ݒ肵�܂��B
                ���̊֐����Ăяo���ƁA�ڑ����̔F�؂� WEP ���g���܂��B
                �Q�[���A�v���P�[�V�������Ƃ̃A���S���Y����
                �ڑ��O�ɐe�q�œ���̒l��ݒ肵�܂��B
                ���̊֐��͐e�@�p�ł��B

  Arguments:    func    WEP Key �𐶐�����֐��ւ̃|�C���^
                        NULL ���w�肷��� WEP Key ���g�p���Ȃ�

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetParentWEPKeyGenerator(WHParentWEPKeyGeneratorFunc func);

/*---------------------------------------------------------------------------*
  Name:         WH_SetChildWEPKeyGenerator

  Description:  WEP Key �𐶐�����֐���ݒ肵�܂��B
                ���̊֐����Ăяo���ƁA�ڑ����̔F�؂� WEP ���g���܂��B
                �Q�[���A�v���P�[�V�������Ƃ̃A���S���Y����
                �ڑ��O�ɐe�q�œ���̒l��ݒ肵�܂��B
                ���̊֐��͎q�@�p�ł��B

  Arguments:    func    WEP Key �𐶐�����֐��ւ̃|�C���^
                        NULL ���w�肷��� WEP Key ���g�p���Ȃ�

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetChildWEPKeyGenerator(WHChildWEPKeyGeneratorFunc func);

/*---------------------------------------------------------------------------*
  Name:         WH_SetIndCallback

  Description:  WH_Initialize �֐��ŌĂяo���Ă��� WM_SetIndCallback �֐��Ɏw�肷��
                �R�[���o�b�N�֐����w�肵�܂��B
                ���̊֐��� WH_Initialize �֐��̑O�ɌĂԂ悤�ɂ��Ă��������B
                ���̊֐��ŃR�[���o�b�N�֐����w�肵�Ă��Ȃ��ꍇ�̓f�t�H���g��
                WH_IndicateHandler ���R�[���o�b�N�ɐݒ肳��܂��B

  Arguments:    callback    WM_SetIndCallback �Ŏw�肷�� Indication �ʒm�p�R�[���o�b�N�ł��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WH_SetIndCallback(WMCallbackFunc callback);

/**************************************************************************
 * �ȉ��́A�჌�C�� WM ���C�u�����̏�Ԃ��擾���郉�b�p�[�֐��ł��B
 **************************************************************************/

/* ----------------------------------------------------------------------
  Name:        WH_GetLinkLevel
  Description: �d�g�̎�M���x���擾���܂��B
  Arguments:   none.
  Returns:     WMLinkLevel �̐��l��Ԃ��܂��B
  ---------------------------------------------------------------------- */
extern int WH_GetLinkLevel(void);

/* ----------------------------------------------------------------------
   Name:        WH_GetAllowedChannel
   Description: �ڑ��Ɏg�p�o����`�����l���̃r�b�g�p�^�[�����擾���܂��B
   Arguments:   none.
   Returns:     channel pattern
   ---------------------------------------------------------------------- */
extern u16 WH_GetAllowedChannel(void);


/**************************************************************************
 * �ȉ��́AWH �̏�Ԃ��擾����֐��ł��B
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_GetBitmap
   Description: �ڑ���Ԃ������r�b�g�p�^�[�����擾���܂��B
   Arguments:   none.
   Returns:     bitmap pattern
   ---------------------------------------------------------------------- */
extern u16 WH_GetBitmap(void);

/* ----------------------------------------------------------------------
   Name:        WH_GetSystemState
   Description: WH �̓�����Ԃ��擾���܂��B
   Arguments:   none.
   Returns:     ������ԁiWH_SYSSTATE_XXXX�j�B
   ---------------------------------------------------------------------- */
extern int WH_GetSystemState(void);

/* ----------------------------------------------------------------------
   Name:        WH_GetConnectMode
   Description: �ڑ������擾���܂��B
   Arguments:   none.
   Returns:     �ڑ����iWH_CONNECTMODE_XX_XXXX�j�B
   ---------------------------------------------------------------------- */
extern int WH_GetConnectMode(void);

/* ----------------------------------------------------------------------
   Name:        WH_GetLastError
   Description: �ł��ŋ߂ɋN�����G���[�̃R�[�h���擾���܂��B
   Arguments:   none.
   Returns:     �G���[�R�[�h�B
   ---------------------------------------------------------------------- */
extern int WH_GetLastError(void);

/*---------------------------------------------------------------------------*
  Name:         WH_PrintBssDesc

  Description:  WMBssDesc �\���̂̃����o���f�o�b�O�o�͂���B

  Arguments:    info    �f�o�b�O�o�͂�����BssDesc�ւ̃|�C���^.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_PrintBssDesc(WMBssDesc *info);


/**************************************************************************
 * �ȉ��́A�`�����l���Ɋւ��鏈�����s���֐��ł��B
 **************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WH_StartMeasureChannel

  Description:  �g�p�\�ȑS�`�����l���œd�g�g�p���̒������J�n���܂��B
                ��������������ƁA�g�p�����Œ�̃`�����l��������Ōv�Z��
                ��Ԃ� WH_SYSSTATE_MEASURECHANNEL �Ɉڍs���܂��B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL WH_StartMeasureChannel(void);

/*---------------------------------------------------------------------------*
  Name:         WH_GetMeasureChannel

  Description:  ���p�\�Ȓ������Ԏg�p���̒Ⴂ�`�����l����Ԃ��܂��B
                WH_MeasureChannel�̓��삪������WH_SYSSTATE_MEASURECHANNEL���
                �ɂȂ��Ă���K�v������܂��B
                ���̊֐����R�[�������ƈ�Ԏg�p���̒Ⴂ�`�����l����Ԃ�
                WH_SYSSTATE_IDLE��ԂɑJ�ڂ��܂��B
                
  Arguments:    None.

  Returns:      �����Ƃ��g�p���̒Ⴂ���p�\�ȃ`�����l���ԍ�.
 *---------------------------------------------------------------------------*/
extern u16 WH_GetMeasureChannel(void);


/**************************************************************************
 * �ȉ��́A���������������ĒʐM�\��Ԃ܂őJ�ڂ���֐��ł��B
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_Initialize
   Description: ��������Ƃ��s���A�������V�[�P���X���J�n���܂��B
   Arguments:   None.
   Returns:     �V�[�P���X�J�n�ɐ�������ΐ^�B
   ---------------------------------------------------------------------- */
extern BOOL WH_Initialize(void);


/*---------------------------------------------------------------------------*
  Name:         WH_TurnOnPictoCatch

  Description:  �s�N�g�`���b�g�T�[�`�@�\��L���ɂ���B
                WH_StartScan�ɂăX�L�������ɁA�s�N�g�`���b�g�𔭌������ꍇ�ɂ�
                �R�[���o�b�N�֐����Ă΂��悤�ɂȂ�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_TurnOnPictoCatch(void);

/*---------------------------------------------------------------------------*
  Name:         WH_TurnOffPictoCatch

  Description:  �s�N�g�`���b�g�T�[�`�@�\�𖳌��ɂ���B
                WH_StartScan�ɂăX�L�������ɁA�s�N�g�`���b�g�𔭌������ꍇ�ł�
                ���������悤�ɂȂ�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_TurnOffPictoCatch(void);

/*---------------------------------------------------------------------------*
  Name:         WH_StartScan

  Description:  �e�@�̃r�[�R�����擾����֐�

  Arguments:    callback - �e�@�������ɕԂ��R�[���o�b�N��ݒ肷��B
                
                macAddr  - �ڑ�����e�@��MAC�A�h���X���w��
                           0xFFFFFF�Ȃ�΂��ׂĂ̐e�@����������B
                           
                channel  - �e����������`�����l�����w��
                           0�Ȃ�΂��ׂẴ`�����l������������B

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL WH_StartScan(WHStartScanCallbackFunc callback, const u8 *macAddr, u16 channel);

/*---------------------------------------------------------------------------*
  Name:         WH_EndScan

  Description:  �e�@�̃r�[�R�����擾����֐�

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL WH_EndScan(void);

/* ----------------------------------------------------------------------
  Name:        WH_ParentConnect
  Description: �ڑ��V�[�P���X���J�n���܂��B
  Arguments:   mode    - WH_CONNECTMODE_MP_PARENT �Ȃ�ΐe�@�Ƃ���MP�J�n�B
                         WH_CONNECTMODE_DS_PARENT �Ȃ�ΐe�@�Ƃ���DataSharing�J�n�B
                         WH_CONNECTMODE_KS_PARENT �Ȃ�ΐe�@�Ƃ���KeySharing�J�n�B
               tgid    - �e�@�ʐMtgid
               channel - �e�@�ʐMchannel
  Returns:     �ڑ��V�[�P���X�J�n�ɐ�������ΐ^�B
  ---------------------------------------------------------------------- */
extern BOOL WH_ParentConnect(int mode, u16 tgid, u16 channel);

/* ----------------------------------------------------------------------
  Name:        WH_ChildConnect
  Description: �ڑ��V�[�P���X���J�n���܂��B
  Arguments:   mode -    WH_CONNECTMODE_MP_CHILD �Ȃ�Ύq�@�Ƃ���MP�J�n�B
                         WH_CONNECTMODE_DS_CHILD �Ȃ�Ύq�@�Ƃ���DataSharing�J�n�B
                         WH_CONNECTMODE_KS_CHILD �Ȃ�Ύq�@�Ƃ���KeySharing�J�n�B
               bssDesc - �ڑ�����e�@��bssDesc
               
  Returns:     �ڑ��V�[�P���X�J�n�ɐ�������ΐ^�B
  ---------------------------------------------------------------------- */
extern BOOL WH_ChildConnect(int mode, WMBssDesc *bssDesc);

/* ----------------------------------------------------------------------
   Name:        WH_ChildConnectAuto
   Description: �q�@�ڑ��V�[�P���X���J�n���܂��B
                �������AWH_ParentConnect �� WH_ChildConnect �Ŏw�肷��
                �e��ݒ������̎��������ɔC���܂��B
   Arguments:   mode    - WH_CONNECTMODE_MP_CHILD �Ȃ�Ύq�@�Ƃ���MP�J�n�B
                          WH_CONNECTMODE_DS_CHILD �Ȃ�Ύq�@�Ƃ���DataSharing�J�n�B
                          WH_CONNECTMODE_KS_CHILD �Ȃ�Ύq�@�Ƃ���KeySharing�J�n�B

                macAddr - �ڑ�����e�@��MAC�A�h���X���w��
                          0xFFFFFF�Ȃ�΂��ׂĂ̐e�@����������B
                          
                channel - �e����������`�����l�����w��
                          0�Ȃ�΂��ׂẴ`�����l������������B

  Returns:     �ڑ��V�[�P���X�J�n�ɐ�������ΐ^�B
   ---------------------------------------------------------------------- */
extern BOOL WH_ChildConnectAuto(int mode, const u8 *macAddr, u16 channel);

/*---------------------------------------------------------------------------*
  Name:         WH_SetJudgeAcceptFunc

  Description:  �q�@�̐ڑ��󂯓���𔻒肷�邽�߂̊֐����Z�b�g���܂��B

  Arguments:    �q�@�̐ڑ�����֐���ݒ�.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetJudgeAcceptFunc(WHJudgeAcceptFunc func);


/**************************************************************************
 * �ȉ��́AWH_DATA_PORT �|�[�g���g�p���钼�ړI�� MP �ʐM�̊֐��ł��B
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_SetReceiver
   Description: WH_DATA_PORT �|�[�g�Ƀf�[�^��M�R�[���o�b�N��ݒ肵�܂��B
   Arguments:   proc - �f�[�^��M�R�[���o�b�N
   Returns:     none.
   ---------------------------------------------------------------------- */
extern void WH_SetReceiver(WHReceiverFunc proc);

/* ----------------------------------------------------------------------
   Name:        WH_SendData
   Description: WH_DATA_PORT �|�[�g�Ƀf�[�^���M���J�n���܂��B
               �iMP�ʐM�p�B�f�[�^�V�F�A�����O���ȂǂɌĂԕK�v�͂���܂���j
   Arguments:   size - �f�[�^�T�C�Y
   Returns:     ���M�J�n�ɐ�������ΐ^�B
   ---------------------------------------------------------------------- */
extern BOOL WH_SendData(void *data, u16 datasize, WHSendCallbackFunc callback);


/**************************************************************************
 * �ȉ��́A�f�[�^�V�F�A�����O�ʐM�𐧌䂷��֐��ł��B
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_GetKeySet
   Description: ���L�L�[�f�[�^��ǂݏo���܂��B
   Arguments:   keyset - �f�[�^�i�[��w��
   Returns:     ��������ΐ^�B
   ---------------------------------------------------------------------- */
extern BOOL WH_GetKeySet(WMKeySet *keyset);

/* ----------------------------------------------------------------------
   Name:        WH_GetSharedDataAdr
  Description: �w��� aid �����}�V�����瓾���f�[�^�̃A�h���X��
                ���L�f�[�^�̃A�h���X����v�Z���擾���܂��B
   Arguments:   aid - �}�V���̎w��
   Returns:     ���s���� NULL �B
   ---------------------------------------------------------------------- */
extern u16 *WH_GetSharedDataAdr(u16 aid);

/* ----------------------------------------------------------------------
   Name:        WH_StepDS
   Description: �f�[�^�V�F�A�����O�̓�����1�i�߂܂��B
                ���t���[���ʐM����Ȃ�A���̊֐������t���[���ĂԕK�v��
                ����܂��B
   Arguments:   data - ���M����f�[�^
   Returns:     ��������ΐ^�B
   ---------------------------------------------------------------------- */
extern BOOL WH_StepDS(void *data);


/**************************************************************************
 * �ȉ��́A�ʐM���I�����ď�������Ԃ܂őJ�ڂ�����֐��ł��B
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_Reset
   Description: ���Z�b�g�V�[�P���X���J�n���܂��B
                ���̊֐����ĂԂƁA���̏�Ԃɍ\�킸���Z�b�g���܂��B
        �G���[����̋������A�p�ł��B
   Arguments:   none.
   Returns:     �����J�n�ɐ�������ΐ^�B
   ---------------------------------------------------------------------- */
extern void WH_Reset(void);

/* ----------------------------------------------------------------------
   Name:        WH_Finalize
   Description: �㏈���E�I���V�[�P���X���J�n���܂��B
                ���̊֐����ĂԂƁA���̏�Ԃ����ēK�؂ȏI���V�[�P���X��
                ���s���܂��B
                �ʏ�̏I�������ɂ́iWH_Reset�ł͂Ȃ��j���̊֐����g�p���܂��B
   Arguments:   None.
   Returns:     None.
   ---------------------------------------------------------------------- */
extern void WH_Finalize(void);

/*---------------------------------------------------------------------------*
  Name:         WH_End

  Description:  �����ʐM���I������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL WH_End(void);

/*---------------------------------------------------------------------------*
  Name:         WH_GetCurrentAid

  Description:  ���݂̎�����AID���擾���܂��B
                �q�@�͐ڑ��E�ؒf���ɕω�����\��������܂��B

  Arguments:    None.

  Returns:      AID�̒l
 *---------------------------------------------------------------------------*/
extern u16 WH_GetCurrentAid(void);

#endif
