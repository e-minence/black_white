/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - include -
  File:     common.h

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef	NITRO_WXC_COMMON_H_
#define	NITRO_WXC_COMMON_H_


#include <nitro.h>


/*****************************************************************************/
/* macro */

/*
 * �f�o�b�O�o�͐؂�ւ�. (�W���ł͖����ɂȂ��Ă��܂�)
 * �R�}���h���C�����炱�̃V���{����0(����)�܂���1(�L��)�Ɏw��\�ł�.
 * �ύX�̔��f�ɂ� WXC ���C�u�����̃��r���h���K�v�ł��邱�Ƃɂ����ӂ�������.
 */
#if !defined(WXC_DEBUG_OUTPUT)
#define     WXC_DEBUG_OUTPUT    0
#endif

/* �p�~�\�� */
#define     WXC_PACKET_LOG      WXC_DEBUG_LOG
#define     WXC_DRIVER_LOG      WXC_DEBUG_LOG

/* �R���p�C�����������`�F�b�N (���C�u���������Ŏg�p) */
#if defined(SDK_COMPILER_ASSERT)
#define SDK_STATIC_ASSERT  SDK_COMPILER_ASSERT
#else
#define SDK_STATIC_ASSERT(expr) \
    extern void sdk_compiler_assert ## __LINE__ ( char is[(expr) ? +1 : -1] )
#endif


/*****************************************************************************/
/* constant */

typedef enum
{
    /* WXC_GetStatus �֐��Ŏ擾�\�ȓ������ */
    WXC_STATE_END,                     /* WXC_End �֐��ɂ��I���������� */
    WXC_STATE_ENDING,                  /* WXC_End �֐��ȍ~���� �I���������s�� */
    WXC_STATE_READY,                   /* WXC_Init �֐��ȍ~���� WXC_Start �֐��ȑO */
    WXC_STATE_ACTIVE,                  /* WXC_Start �֐��ȍ~�Ń��C�����X���L���ȏ�� */

    /* �V�X�e���R�[���o�b�N�ɒʒm���������C�x���g */
    WXC_STATE_CONNECTED,               /* �q�@�ڑ� (�e�@���̂ݔ���, ������WMStartParentCallback) */
    WXC_STATE_EXCHANGE_DONE,           /* �f�[�^�������� (�����̓��[�U���w�肵����M�o�b�t�@) */
    WXC_STATE_BEACON_RECV              /* �r�[�R����M (������WXCBeaconFoundCallback�\����) */
}
WXCStateCode;

/* �e�@���[�h�̃r�[�R���ݒ� */
#define WXC_BEACON_PERIOD            90 /* [ms] */
#define WXC_PARENT_BEACON_SEND_COUNT_OUT (900 / WXC_BEACON_PERIOD)

/* ���C�u������MP����M�p�P�b�g�T�C�Y */
#define WXC_PACKET_SIZE_MIN          20
#define WXC_PACKET_SIZE_MAX          WM_SIZE_MP_DATA_MAX

/* ���ʂ��ꂿ�����ʐM�p�� GGID */
#define WXC_GGID_COMMON_BIT          0x80000000UL
#define WXC_GGID_COMMON_ANY          (u32)(WXC_GGID_COMMON_BIT | 0x00000000UL)
#define WXC_GGID_COMMON_PARENT       (u32)(WXC_GGID_COMMON_BIT | 0x00400120UL)


/*****************************************************************************/
/* declaration */

/* WXC ���C�u�����̃R�[���o�b�N�֐��v���g�^�C�v */
typedef void (*WXCCallback) (WXCStateCode stat, void *arg);

/* �eAID���Ƃ�WXC���ێ����Ă��郆�[�U��� (WXC_GetUserInfo() �Ŏ擾) */
typedef struct WXCUserInfo
{
    u16     aid;
    u8      macAddress[6];
}
WXCUserInfo;

/* MP ����M�p�P�b�g�̑���Ɏg�p����\���� */
typedef struct WXCPacketInfo
{
    /*
     * ���M�p�o�b�t�@�܂��͎�M�o�b�t�@.
     * ���M���͂��̃o�b�t�@�Ɏ��ۂ̑��M�f�[�^���i�[���ĕԂ�.
     */
    u8     *buffer;
    /*
     * ���M�\�ő�T�C�Y�܂��͎�M�f�[�^�T�C�Y.
     * ���M���͂����Ɏ��ۂ̑��M�T�C�Y���i�[���ĕԂ�.
     */
    u16     length;
    /*
     * ���ݐڑ�����Ă��� AID �r�b�g�}�b�v.
     * (�r�b�g0 �̗L���Ŏ��g���e�@���q�@�����f�\)
     * ���M���͂����Ɏ��ۂ̑��M�Ώۂ��i�[���ĕԂ�.
     */
    u16     bitmap;
}
WXCPacketInfo;

/* WXC_STATE_BEACON_RECV �R�[���o�b�N�̈��� */
typedef struct WXCBeaconFoundCallback
{
    /*
     * �w��̃v���g�R���ɍ��v����t�H�[�}�b�g�ł���� TRUE,
     * �����łȂ��ꍇ�ɂ� FALSE �Ƃ��ė^������.
     * ���̃r�[�R���̑��M���֐ڑ�����ꍇ�͂��̃����o�� TRUE �ɂ��ĕԂ�.
     */
    BOOL    matched;
    /*
     * ���񌟏o���ꂽ�r�[�R��.
     */
    const WMBssDesc *bssdesc;
}
WXCBeaconFoundCallback;


/*****************************************************************************/
/* function */

#ifdef  __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
    WXC ���[�e�B���e�B
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
  Name:         WXC_DEBUG_LOG

  Description:  WXC ���C�u���������̃f�o�b�O�o�͊֐�.
                ���̊֐��� SDK_WEAK_SYMBOL �ŏC������Ă���,
                �W���ł� OS_TPrintf �Ɠ����ȏ����ɂȂ�܂�.
                �A�v���P�[�V�����ŗL�̃f�o�b�O�V�X�e���Ȃǂ�
                ���̓����֐����I�[�o�[���C�h���邱�Ƃ��ł��܂�.

  Arguments:    format        �f�o�b�O�o�͓��e����������������
                ...           �ψ���

  Returns:      None.
 *---------------------------------------------------------------------------*/
#if (WXC_DEBUG_OUTPUT == 1)
void    WXC_DEBUG_LOG(const char *format, ...);
#else
#define     WXC_DEBUG_LOG(...) ((void)0)
#endif

/*---------------------------------------------------------------------------*
  Name:         WXC_GetWmApiName

  Description:  WM �֐��̖��O��������擾

  Arguments:    id          WM �֐��̎�ނ����� WMApiid �񋓒l

  Returns:      WM �֐��̖��O������.
 *---------------------------------------------------------------------------*/
const char *WXC_GetWmApiName(WMApiid id);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetWmErrorName

  Description:  WM �G���[�R�[�h�̖��O��������擾

  Arguments:    err         WM �G���[�R�[�h������ WMErrCode �񋓒l

  Returns:      WM �G���[�R�[�h�̖��O������.
 *---------------------------------------------------------------------------*/
const char *WXC_GetWmErrorName(WMErrCode err);

/*---------------------------------------------------------------------------*
  Name:         WXC_CheckWmApiResult

  Description:  WM �֐��̌Ăяo���Ԃ�l�𔻒肵,
                �G���[�ł���Ώڍׂ��f�o�b�O�o��.

  Arguments:    id          WM �֐��̎��
                err         �֐�����Ԃ��ꂽ�G���[�R�[�h

  Returns:      �����ȕԂ�l�ł���� TRUE, �����łȂ���� FALSE.
 *---------------------------------------------------------------------------*/
BOOL    WXC_CheckWmApiResult(WMApiid id, WMErrCode err);

/*---------------------------------------------------------------------------*
  Name:         WXC_CheckWmCallbackResult

  Description:  WM �R�[���o�b�N�̕Ԃ�l�𔻒肵,
                �G���[�ł���Ώڍׂ��f�o�b�O�o��.

  Arguments:    arg         WM �֐��̃R�[���o�b�N�֕Ԃ��ꂽ����

  Returns:      �����Ȍ��ʒl�ł���� TRUE, �����łȂ���� FALSE.
 *---------------------------------------------------------------------------*/
BOOL    WXC_CheckWmCallbackResult(void *arg);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetNextAllowedChannel

  Description:  �w�肳�ꂽ�`�����l���̎��Ɏg�p�\�ȃ`�����l�����擾.
                �ŏ�ʃ`�����l���̎��͍ŉ��ʃ`�����l���փ��[�v����.
                0 ���w�肷��΍ŉ��ʂ̃`�����l�����擾.

  Arguments:    ch            ����̃`�����l���ʒu (1����14, ���邢��0)

  Returns:      ���Ɏg�p�\�ȃ`�����l��. (1����14)
 *---------------------------------------------------------------------------*/
u16     WXC_GetNextAllowedChannel(int ch);

/*---------------------------------------------------------------------------*
  Name:         WXC_IsCommonGgid

  Description:  �w�肳�ꂽGGID�����ʂ��ꂿ�����ʐM�v���g�R��GGID������.

  Arguments:    gid          ���肷��GGID

  Returns:      ���ʂ��ꂿ�����ʐM�v���g�R��GGID�ł����TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE
BOOL    WXC_IsCommonGgid(u32 ggid)
{
    return ((ggid & WXC_GGID_COMMON_BIT) != 0);
}


#ifdef  __cplusplus
}       /* extern "C" */
#endif


#endif /* NITRO_WXC_COMMON_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
