/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - include -
  File:     driver.h

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

#ifndef	NITRO_WXC_DRIVER_H_
#define	NITRO_WXC_DRIVER_H_

#include    <nitro.h>


/*****************************************************************************/
/* constant */

/* ���C�����X��{�ݒ� */
#define     WXC_DEFAULT_PORT             4
#define     WXC_DEFAULT_PORT_PRIO        2

/* �ȑO�� 150[ms] �����������݂̓r�[�R���Ԋu(90[ms]) + 20[ms] �� */
#define     WXC_SCAN_TIME_MAX            (WXC_BEACON_PERIOD + 20)

#define     WXC_MAX_RATIO                100

/* ���C�����X�h���C�o����̃C�x���g�R�[���o�b�N��� */
typedef enum
{
    /* ���C�����X�I�� (�����͏�� NULL) */
    WXC_DRIVER_EVENT_STATE_END,
    /* STOP �X�e�[�g�ɑJ�ڊ��� (�����͏�� NULL) */
    WXC_DRIVER_EVENT_STATE_IDLE,
    /* IDLE �X�e�[�g�ɑJ�ڊ��� (�����͏�� NULL) */
    WXC_DRIVER_EVENT_STATE_STOP,
    /* MP_PARENT �X�e�[�g�ɑJ�ڊ��� (�����͏�� NULL) */
    WXC_DRIVER_EVENT_STATE_PARENT,
    /* MP_CHILD �X�e�[�g�ɑJ�ڊ���  (�����͏�� NULL) */
    WXC_DRIVER_EVENT_STATE_CHILD,
    /* �r�[�R���X�V�^�C�~���O (������ WMParentParam �|�C���^) */
    WXC_DRIVER_EVENT_BEACON_SEND,
    /* �r�[�R�����o�^�C�~���O (������ WMBssDesc �|�C���^) */
    WXC_DRIVER_EVENT_BEACON_RECV,
    /* MP �p�P�b�g���M (������ WXCPacketInfo �|�C���^) */
    WXC_DRIVER_EVENT_DATA_SEND,
    /* MP �p�P�b�g��M (������ const WXCPacketInfo �|�C���^) */
    WXC_DRIVER_EVENT_DATA_RECV,
    /* �ڑ��O�̒ʒm (������ WMBssDesc �|�C���^) */
    WXC_DRIVER_EVENT_CONNECTING,
    /* �ڑ��̌��m (������) */
    WXC_DRIVER_EVENT_CONNECTED,
    /* �ؒf�̌��m (������ WMStartParentCallback �|�C���^) */
    WXC_DRIVER_EVENT_DISCONNECTED,

    WXC_DRIVER_EVENT_MAX
}
WXCDriverEvent;

/* �h���C�o�̓������ */
typedef enum WXCDriverState
{
    WXC_DRIVER_STATE_END,              /* �������O (driver = NULL) */
    WXC_DRIVER_STATE_BUSY,             /* ��ԑJ�ڒ� */
    WXC_DRIVER_STATE_STOP,             /* STOP �X�e�[�g�ň��蒆 */
    WXC_DRIVER_STATE_IDLE,             /* IDLE �X�e�[�g�ň��蒆 */
    WXC_DRIVER_STATE_PARENT,           /* MP_PARENT �X�e�[�g�ň��蒆 */
    WXC_DRIVER_STATE_CHILD,            /* MP_CHILD �X�e�[�g�ň��蒆 */
    WXC_DRIVER_STATE_ERROR             /* �������A��i�̖����G���[ */
}
WXCDriverState;


/*****************************************************************************/
/* declaration */

/*---------------------------------------------------------------------------*
  Name:         WXCDriverEventFunc

  Description:  ���C�����X�h���C�o����̃C�x���g�R�[���o�b�N�v���g�^�C�v

  Arguments:    event         �ʒm����C�x���g
                arg           �C�x���g���Ƃɒ�`���ꂽ����

  Returns:      �C�x���g���Ƃɒ�`���ꂽ u32 �̃C�x���g���ʒl.
 *---------------------------------------------------------------------------*/
typedef u32 (*WXCDriverEventFunc) (WXCDriverEvent event, void *arg);


/* WXC���C�u�����������C�����X�h���C�o�\���� */
typedef struct WXCDriverWork
{

    /* WM �������[�N */
    u8      wm_work[WM_SYSTEM_BUF_SIZE] ATTRIBUTE_ALIGN(32);
    u8      mp_send_work[WM_SIZE_MP_PARENT_SEND_BUFFER(WM_SIZE_MP_DATA_MAX, FALSE)]
        ATTRIBUTE_ALIGN(32);
    u8     
        mp_recv_work[WM_SIZE_MP_PARENT_RECEIVE_BUFFER(WM_SIZE_MP_DATA_MAX, WM_NUM_MAX_CHILD, FALSE)]
        ATTRIBUTE_ALIGN(32);
    u8      current_send_buf[WM_SIZE_MP_DATA_MAX] ATTRIBUTE_ALIGN(32);
    u16     wm_dma;                    /* WM �p DMA �`�����l�� */
    u16     current_channel;           /* ���݂̃`�����l��(Measure/Start) */
    u16     own_aid;                   /* ���g�� AID */
    u16     peer_bitmap;               /* �ڑ�����̃r�b�g�}�b�v */
    u16     send_size_max;             /* MP ���M�T�C�Y */
    u16     recv_size_max;             /* MP ���M�T�C�Y */
    BOOL    send_busy;                 /* �O��� MP �����҂��@ */

    /* ��ԑJ�ڏ�� */
    WXCDriverState state;
    WXCDriverState target;
    WXCDriverEventFunc callback;

    /* �e�@������ */
    WMParentParam *parent_param;
    BOOL    need_measure_channel;
    int     measure_ratio_min;

    /* �q�@������ */
    int     scan_found_num;
    u8      padding1[20];
    WMBssDesc target_bss[1] ATTRIBUTE_ALIGN(32);
    u8      scan_buf[WM_SIZE_SCAN_EX_BUF] ATTRIBUTE_ALIGN(32);
    WMScanExParam scan_param[1] ATTRIBUTE_ALIGN(32);
    u8      ssid[24];
    u8      padding2[4];

}
WXCDriverWork;


/*****************************************************************************/
/* function */

#ifdef  __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         WXC_InitDriver

  Description:  ���C�����X������������ IDLE �ֈڍs�J�n����.

  Arguments:    driver        �������[�N�������o�b�t�@�Ƃ��Ďg�p����
                              WXCDriverWork �\���̂ւ̃|�C���^
                              32�o�C�g�A���C�������g����Ă���K�v������
                pp            �e�@�ݒ�
                func          �C�x���g�ʒm�R�[���o�b�N
                dma           ���C�����X�Ɋ��蓖�Ă� DMA �`�����l��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_InitDriver(WXCDriverWork * driver, WMParentParam *pp, WXCDriverEventFunc func, u32 dma);

/*---------------------------------------------------------------------------*
  Name:         WXC_SetDriverTarget

  Description:  ����̏�Ԃ�ڕW�ɑJ�ڂ��J�n����.

  Arguments:    driver        WXCDriverWork �\����
                target        �J�ڐ�̃X�e�[�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_SetDriverTarget(WXCDriverWork * driver, WXCDriverState target);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetDriverState

  Description:  ���݂̑J�ڏ�Ԃ��擾����.

  Arguments:    None.

  Returns:      ���݂̑J�ڏ��.
 *---------------------------------------------------------------------------*/
static inline WXCDriverState WXC_GetDriverState(const WXCDriverWork * driver)
{
    return driver->state;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_SetDriverSsid

  Description:  �ڑ����� SSID ��ݒ肷��.

  Arguments:    driver        WXCDriverWork �\����.
                buffer        �ݒ肷SSID�f�[�^.
                length        SSID�f�[�^��.
                              WM_SIZE_CHILD_SSID �ȉ��ł���K�v������.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_SetDriverSsid(WXCDriverWork * driver, const void *buffer, u32 length);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetDriverTarget

  Description:  ���݂̖ڕW�J�ڐ��Ԃ��擾����.

  Arguments:    None.

  Returns:      ���݂̖ڕW�J�ڐ���.
 *---------------------------------------------------------------------------*/
static inline WXCDriverState WXC_GetDriverTarget(const WXCDriverWork * driver)
{
    return driver->target;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_IsParentMode

  Description:  ���݂̃��C�����X��Ԃ��e�@���[�h������.
                WXC_STATE_ACTIVE �̏�ԂɌ���L��.

  Arguments:    None.

  Returns:      ���C�����X��Ԃ��e�@���[�h�Ȃ� TRUE, �q�@���[�h�Ȃ� FALSE.
 *---------------------------------------------------------------------------*/
static inline BOOL WXCi_IsParentMode(const WXCDriverWork * driver)
{
    return (driver->state == WXC_DRIVER_STATE_PARENT);
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_GetParentBssDesc

  Description:  �ڑ���� WMBssDesc �\���̂��擾. (�q�@�̏ꍇ�̂ݗL��)

  Arguments:    None.

  Returns:      �ڑ���� WMBssDesc �\����
 *---------------------------------------------------------------------------*/
static inline const WMBssDesc *WXCi_GetParentBssDesc(const WXCDriverWork * driver)
{
    return driver->target_bss;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_Stop

  Description:  ���C�����X��Ԃ� STOP �ֈڍs�J�n����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WXCi_Stop(WXCDriverWork * driver)
{
    WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_STOP);
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_StartParent

  Description:  ���C�����X��Ԃ� IDLE ���� MP_PARENT �ֈڍs�J�n����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WXCi_StartParent(WXCDriverWork * driver)
{
    WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_PARENT);
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_StartChild

  Description:  ���C�����X��Ԃ� IDLE ���� MP_CHILD �ֈڍs�J�n����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WXCi_StartChild(WXCDriverWork * driver)
{
    WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_CHILD);
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_Reset

  Description:  ���C�����X��Ԃ����Z�b�g���� IDLE �ֈڍs�J�n����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WXCi_Reset(WXCDriverWork * driver)
{
    WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_IDLE);
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_End

  Description:  ���C�����X���I������.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WXCi_End(WXCDriverWork * driver)
{
    WXC_SetDriverTarget(driver, WXC_DRIVER_STATE_END);
}


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif


#endif /* NITRO_WXC_DRIVER_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
