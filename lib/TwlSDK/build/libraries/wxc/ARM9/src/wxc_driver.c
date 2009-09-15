/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - libraries -
  File:     wxc_driver.c

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-16#$
  $Rev: 9661 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <nitro/wxc/common.h>
#include <nitro/wxc/driver.h>


/*****************************************************************************/
/* declaration */

/* �G���[�`�F�b�N�֐��Q */
static void WXCi_RecoverWmApiFailure(WXCDriverWork *, WMApiid id, WMErrCode err);
static BOOL WXCi_CheckWmApiResult(WXCDriverWork * driver, WMApiid id, WMErrCode err);
static BOOL WXCi_CheckWmCallbackResult(WXCDriverWork *, void *arg);
static void WXCi_ErrorQuit(WXCDriverWork *);

/*
 * ���C�����X�X�e�[�g�Ԃ̘A���J�ڃv���V�[�W��.
 * �ŏ��Ɉ��� NULL �ŌĂяo��, ���X�Ɠ����֐��փR�[���o�b�N����.
 * �ڕW�̏�ԂɒB������C�x���g�ʒm�R�[���o�b�N����.
 *
 * ���l:
 *   �����̊֐��͍ŏI�I�ɃX���b�h����e�Ղɂ���悤�݌v����Ă���̂�,
 *   �����e�i���X�̍ۂɂ����֐����̌X�� WM �֐��Ăяo���𕪊�����
 *   ���� StateIn- StateOut- �ɕ�����悤�ȕύX�����ׂ��łȂ�.
 */
static void WXCi_InitProc(void *arg);  /* (end) -> STOP             */
static void WXCi_StartProc(void *arg); /* STOP -> IDLE              */
static void WXCi_StopProc(void *arg);  /* IDLE -> STOP              */
static void WXCi_EndProc(void *arg);   /* STOP  -> (end)            */
static void WXCi_ResetProc(void *arg); /* (any) -> IDLE             */
static void WXCi_StartParentProc(void *arg);    /* IDLE  -> MP_PARENT        */
static void WXCi_StartChildProc(void *arg);     /* IDLE  -> MP_CHILD         */
static void WXCi_ScanProc(void *arg);  /* IDLE -> SCAN -> IDLE      */
static void WXCi_MeasureProc(void *arg);        /* IDLE -> (measure) -> IDLE */

/* ��ԑJ�ڐ��� */
static void WXCi_OnStateChanged(WXCDriverWork *, WXCDriverState state, void *arg);

/* ���̑��̃R�[���o�b�N����уC���W�P�[�g */
static void WXCi_IndicateCallback(void *arg);
static void WXCi_PortCallback(void *arg);
static void WXCi_MPSendCallback(void *arg);
static void WXCi_ParentIndicate(void *arg);
static void WXCi_ChildIndicate(void *arg);

/*****************************************************************************/
/* constant */

/* SDK3.0RC�ȍ~�ł́A���g�̐ؒf�ł��C���W�P�[�g����������悤�ύX���ꂽ */
#define VERSION_TO_INT(major, minor, relstep) \
    (((major) << 24) | ((minor) << 16) | ((relstep) << 0))
#if VERSION_TO_INT(SDK_VERSION_MAJOR, SDK_VERSION_MINOR, SDK_VERSION_RELSTEP) < VERSION_TO_INT(3, 0, 20100)
#define WM_STATECODE_DISCONNECTED_FROM_MYSELF ((WMStateCode)26)
#endif


/*****************************************************************************/
/* variable */

/*
 * ���C�����X�h���C�o�̃��[�N������.
 * �E����� WM �R�[���o�b�N���ł̂� this �Ƃ��Ďg�p����ړI�ŗp�ӂ���Ă���.
 *   (�ʏ�, �\�Ȍ��� WXCDriverWork �������ɂƂ�Ăяo���Ώۂ𖾎����邱��)
 * �E�O���[�o���|�C���^�𒼐ڎQ�Ƃ���Ɣ�volatile�����ɂȂ��������������,
 *   ���̃|�C���^�𑽗p����֐����ł͈�U���[�J���ϐ��ɂ����������ǂ�.
 */
static WXCDriverWork *work;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         WXCi_ErrorQuit

  Description:  �G���[�����o�����ۂ̃��Z�b�g����.

  Arguments:    driver        WXCDriverWork �\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_ErrorQuit(WXCDriverWork * driver)
{
    /* �����Ă��ُ�I�������ɂȂ�̂� BUSY �𖳎����� */
    if (driver->state == WXC_DRIVER_STATE_BUSY)
    {
        driver->state = driver->target;
    }
    WXCi_End(driver);
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_RecoverWmApiFailure

  Description:  WM �֐��̃G���[�����ԉ񕜂̎��s

  Arguments:    driver        WXCDriverWork �\����
                id            WM �֐��̎��
                err           �֐�����Ԃ��ꂽ�G���[�R�[�h

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_RecoverWmApiFailure(WXCDriverWork * driver, WMApiid id, WMErrCode err)
{
    /* �O�̂��߂����ł��������� */
    if (err == WM_ERRCODE_SUCCESS)
    {
        return;
    }

    switch (id)
    {
    default:
        /* WXC �Ŏg�p���Ă��Ȃ��͂��̊֐� */
        OS_TWarning("WXC library error (unknown WM API : %d)\n", id);
        WXCi_ErrorQuit(driver);
        break;

    case WM_APIID_ENABLE:
    case WM_APIID_POWER_ON:
    case WM_APIID_POWER_OFF:
    case WM_APIID_DISABLE:
    case WM_APIID_RESET:
        /* �������ɂ��玸�s�����ꍇ�͏I�� */
        WXCi_ErrorQuit(driver);
        break;

    case WM_APIID_MEASURE_CHANNEL:
    case WM_APIID_SET_P_PARAM:
    case WM_APIID_START_PARENT:
    case WM_APIID_START_MP:
    case WM_APIID_START_SCAN_EX:
    case WM_APIID_END_SCAN:
    case WM_APIID_DISCONNECT:
        /* ��v�ȏ�ԑJ�ڊ֐��ŗ\�����ʎ��s�����������ꍇ�͏I�� */
        WXCi_ErrorQuit(driver);
        break;

    case WM_APIID_START_CONNECT:
        /* StartConnect �������̐ڑ����s�͂��肦��̂ōēx IDLE �� */
        if ((err == WM_ERRCODE_FAILED) ||
            (err == WM_ERRCODE_NO_ENTRY) || (err == WM_ERRCODE_OVER_MAX_ENTRY))
        {
            /* ���̈�u���� BUSY ���������� */
            driver->state = WXC_DRIVER_STATE_CHILD;
            WXCi_Reset(driver);
        }
        /* ����ȊO�͒v���I�G���[�Ɣ��f���ďI�� */
        else
        {
            WXCi_ErrorQuit(driver);
        }
        break;

    case WM_APIID_INDICATION:
        /* FIFO_ERROR, FLASH_ERROR ���̒v���I�Ȓʒm����M�����ꍇ�͏I�� */
        WXCi_ErrorQuit(driver);
        break;

    case WM_APIID_SET_MP_DATA:
    case WM_APIID_PORT_SEND:
        /*
         * SetMPData �����s����P�[�X:
         *   ILLEGAL_STATE,INVALID_PARAM,FIFO_ERROR ... ���.
         *   NO_CHILD ... �Ăяo����.
         *   SEND_QUEUE_FULL,SEND_FAILED ... �R�[���o�b�N��.
         * �Ď��s�ɂ���ĉ���������Ȃ̂Ŗ�������.
         */
        break;

    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_CheckWmApiResult

  Description:  WM �֐��̌Ăяo���Ԃ�l�𔻒�

  Arguments:    driver        WXCDriverWork �\����
                id            WM �֐��̎��
                err           �֐�����Ԃ��ꂽ�G���[�R�[�h

  Returns:      WM_ERRCODE_SUCCESS �Ȃ� TRUE,
                �����łȂ���΃G���[�o�͂��� FALSE.
 *---------------------------------------------------------------------------*/
static BOOL WXCi_CheckWmApiResult(WXCDriverWork * driver, WMApiid id, WMErrCode err)
{
    BOOL    ret = WXC_CheckWmApiResult(id, err);
    /* �G���[�������ꍇ�� API ���Ƃ̕��A���� */
    if (!ret)
    {
        WXCi_RecoverWmApiFailure(driver, id, err);
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_CheckWmCallbackResult

  Description:  WM �R�[���o�b�N�̕Ԃ�l�𔻒�

  Arguments:    driver        WXCDriverWork �\����
                arg           �֐�����Ԃ��ꂽ����

  Returns:      WM_ERRCODE_SUCCESS �Ȃ� TRUE,
                �����łȂ���΃G���[�o�͂��� FALSE.
 *---------------------------------------------------------------------------*/
static BOOL WXCi_CheckWmCallbackResult(WXCDriverWork * driver, void *arg)
{
    BOOL    ret = WXC_CheckWmCallbackResult(arg);
    /* �G���[�������ꍇ�� API ���Ƃ̕��A���� */
    if (!ret)
    {
        const WMCallback *cb = (const WMCallback *)arg;
        WXCi_RecoverWmApiFailure(driver, (WMApiid)cb->apiid, (WMErrCode)cb->errcode);
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_CallDriverEvent

  Description:  ���C�����X�h���C�o�C�x���g��ʒm

  Arguments:    driver        WXCDriverWork �\����
                event         �ʒm����C�x���g
                arg           �C�x���g���Ƃɒ�`���ꂽ����

  Returns:      �C�x���g���Ƃɒ�`���ꂽ u32 �̃C�x���g���ʒl.
 *---------------------------------------------------------------------------*/
static inline u32 WXCi_CallDriverEvent(WXCDriverWork * driver, WXCDriverEvent event, void *arg)
{
    u32     result = 0;
    if (driver->callback)
    {
        result = (*driver->callback) (event, arg);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_CallSendEvent

  Description:  �f�[�^���M�^�C�~���O�C�x���g�̃R�[���o�b�N�֐��B

  Arguments:    driver        WXCDriverWork �\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_CallSendEvent(WXCDriverWork * driver)
{
    if (!driver->send_busy)
    {
        /*
         * ���M�p�P�b�g�o�b�t�@���̐ݒ�.
         * TODO:
         * ����� MP �ʐM�m������1�񂾂��ێ������ق��������I?
         * ���� wxc_api.c �ɑS����������������.
         */
        const u16 max_length = (driver->own_aid == 0) ?
            driver->parent_param->parentMaxSize : driver->target_bss->gameInfo.childMaxSize;
        WXCPacketInfo packet;
        packet.bitmap = driver->peer_bitmap;
        packet.length = max_length;
        packet.buffer = driver->current_send_buf;
        /* ���M�f�[�^�����[�U�ɐݒ肳���� */
        (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_DATA_SEND, &packet);
        /* �f�[�^���M */
        if (packet.length <= max_length)
        {
            WMErrCode ret;
            ret = WM_SetMPDataToPort(WXCi_MPSendCallback,
                                     (u16 *)packet.buffer, packet.length,
                                     packet.bitmap, WXC_DEFAULT_PORT, WXC_DEFAULT_PORT_PRIO);
            driver->send_busy = WXCi_CheckWmApiResult(driver, WM_APIID_SET_MP_DATA, ret);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_OnStateChanged

  Description:  �w��̃X�e�[�g�Ɉ��肵�����̏���

  Arguments:    driver        WXCDriverWork �\����
                state         �J�ڂ��m�肵���X�e�[�g
                arg           �C�x���g���Ƃ̈���

  Returns:      �C�x���g���Ƃɒ�`���ꂽ u32 �̃C�x���g���ʒl.
 *---------------------------------------------------------------------------*/
static void WXCi_OnStateChanged(WXCDriverWork * driver, WXCDriverState state, void *arg)
{
    driver->state = state;

    /* ���݂̏�Ԃ��J�ڑΏۂł���΃��[�U�ɒʒm */
    if (driver->target == state)
    {
        switch (state)
        {
        case WXC_DRIVER_STATE_END:
            (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_END, NULL);
            break;

        case WXC_DRIVER_STATE_STOP:
            (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_STOP, NULL);
            break;

        case WXC_DRIVER_STATE_IDLE:
            (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_IDLE, NULL);
            break;

        case WXC_DRIVER_STATE_PARENT:
            driver->send_busy = FALSE;
            (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_PARENT, NULL);
            break;

        case WXC_DRIVER_STATE_CHILD:
            driver->send_busy = FALSE;
            (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_STATE_CHILD, NULL);
            /* ���̏�Őe�Ƃ̐ڑ���ʒm */
            driver->peer_bitmap |= (u16)(1 << 0);
            (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_CONNECTED, arg);
            WXCi_CallSendEvent(driver);
            break;
        }
    }

    /*
     * �����łȂ���Όp���J��.
     * +-----+      +------+       +------+                     +--------+
     * |     > Init |      > Start >      > Measure/StartParent > PARENT |
     * |     |      |      |       |      <               Reset <        |
     * |     |      |      |       |      |                     +--------+
     * | END |      | STOP |       | IDLE > Scan/StartChild     > CHILD  |
     * |     |      |      |       |      <               Reset <        |
     * |     |      |      <  Stop <      |                     +--------+
     * |     <  End <      <   End <      |
     * +-----+      +------+       +------+
     */
    else
    {
        switch (state)
        {

        case WXC_DRIVER_STATE_END:
            WXCi_InitProc(NULL);
            break;

        case WXC_DRIVER_STATE_STOP:
            switch (driver->target)
            {
            case WXC_DRIVER_STATE_END:
                WXCi_EndProc(NULL);
                break;
            case WXC_DRIVER_STATE_IDLE:
            case WXC_DRIVER_STATE_PARENT:
            case WXC_DRIVER_STATE_CHILD:
                WXCi_StartProc(NULL);
                break;
            }
            break;

        case WXC_DRIVER_STATE_IDLE:
            switch (driver->target)
            {
            case WXC_DRIVER_STATE_END:
            case WXC_DRIVER_STATE_STOP:
                WXCi_StopProc(NULL);
                break;
            case WXC_DRIVER_STATE_PARENT:
                driver->need_measure_channel = TRUE;
                if (driver->need_measure_channel)
                {
                    WXCi_MeasureProc(NULL);
                }
                break;
            case WXC_DRIVER_STATE_CHILD:
                WXCi_ScanProc(NULL);
                break;
            }
            break;

        case WXC_DRIVER_STATE_PARENT:
        case WXC_DRIVER_STATE_CHILD:
            WXCi_ResetProc(NULL);
            break;

        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_MPSendCallback

  Description:  WM_SetMPData�ɑ΂���R�[���o�b�N�֐��B

  Arguments:    arg - �R�[���o�b�N�\���̂ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_MPSendCallback(void *arg)
{
    WXCDriverWork *const driver = work;

    (void)WXCi_CheckWmCallbackResult(driver, arg);
    driver->send_busy = FALSE;
    if (driver->peer_bitmap != 0)
    {
        WXCi_CallSendEvent(driver);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_IndicateCallback

  Description:  Indicate�������ɌĂяo�����R�[���o�b�N�֐��B

  Arguments:    arg - �R�[���o�b�N�\���̂ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_IndicateCallback(void *arg)
{
    /* TODO: ����� CheckError �ɓ����ł��Ȃ���? */
    WXCDriverWork *const driver = work;
    WMIndCallback *cb = (WMIndCallback *)arg;
    if (cb->errcode == WM_ERRCODE_FIFO_ERROR)
    {
        WXC_DRIVER_LOG("WM_ERRCODE_FIFO_ERROR Indication!\n");
        /* �����s�\�ȃG���[ */
        driver->target = WXC_DRIVER_STATE_ERROR;
        driver->state = WXC_DRIVER_STATE_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_PortCallback

  Description:  port �ɑ΂����M�ʒm

  Arguments:    arg - �R�[���o�b�N�\���̂ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_PortCallback(void *arg)
{
    WXCDriverWork *const driver = work;

    if (WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;
        switch (cb->state)
        {
        case WM_STATECODE_PORT_RECV:
            {
                WXCPacketInfo packet;
                packet.bitmap = (u16)(1 << cb->aid);
                packet.length = cb->length;
                packet.buffer = (u8 *)cb->data;
                (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_DATA_RECV, &packet);
            }
            break;
        case WM_STATECODE_CONNECTED:
            break;
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        case WM_STATECODE_DISCONNECTED:
            WXC_DRIVER_LOG("disconnected(%02X-=%02X)\n", driver->peer_bitmap, (1 << cb->aid));
            driver->peer_bitmap &= (u16)~(1 << cb->aid);
            (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_DISCONNECTED,
                                       (void *)(1 << cb->aid));
            break;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_InitProc

  Description:  READY -> STOP �ւ̘A���J��

  Arguments:    arg           �R�[���o�b�N���� (�Ăяo������ NULL ���w��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_InitProc(void *arg)
{
    WXCDriverWork *const driver = work;
    WMCallback *cb = (WMCallback *)arg;

    if (!arg || WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMErrCode wmResult = WM_ERRCODE_SUCCESS;
        /* (outof) -> STOP */
        if (!arg)
        {
            /* ���� MeasureChannel �͋N������1��̂ݍs�� */
            driver->need_measure_channel = TRUE;
            driver->state = WXC_DRIVER_STATE_BUSY;
            wmResult = WM_Init(driver->wm_work, driver->wm_dma);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_INITIALIZE, wmResult);
            wmResult = WM_Enable(WXCi_InitProc);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_ENABLE, wmResult);
        }
        /* ���� */
        else if (cb->apiid == WM_APIID_ENABLE)
        {
            /* �e��ʒm����M */
            wmResult = WM_SetIndCallback(WXCi_IndicateCallback);
            if (WXCi_CheckWmApiResult(driver, WM_APIID_INDICATION, wmResult))
            {
                /* �|�[�g�R�[���o�b�N���w�� */
                wmResult = WM_SetPortCallback(WXC_DEFAULT_PORT, WXCi_PortCallback, NULL);
                if (WXCi_CheckWmApiResult(driver, WM_APIID_PORT_SEND, wmResult))
                {
                    WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_STOP, NULL);
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_StartProc

  Description:  STOP -> IDLE �ւ̘A���J��

  Arguments:    arg           �R�[���o�b�N���� (�Ăяo������ NULL ���w��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_StartProc(void *arg)
{
    WXCDriverWork *const driver = work;
    WMCallback *cb = (WMCallback *)arg;

    if (!arg || WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMErrCode wmResult = WM_ERRCODE_SUCCESS;
        /* STOP -> IDLE */
        if (!arg)
        {
            driver->state = WXC_DRIVER_STATE_BUSY;
            wmResult = WM_PowerOn(WXCi_StartProc);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_POWER_ON, wmResult);
        }
        /* ���� */
        else if (cb->apiid == WM_APIID_POWER_ON)
        {
            WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_IDLE, NULL);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_StopProc

  Description:  IDLE -> STOP �ւ̘A���J��

  Arguments:    arg           �R�[���o�b�N���� (�Ăяo������ NULL ���w��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_StopProc(void *arg)
{
    WXCDriverWork *const driver = work;
    WMCallback *cb = (WMCallback *)arg;

    if (!arg || WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMErrCode wmResult = WM_ERRCODE_SUCCESS;
        /* IDLE -> STOP */
        if (!arg)
        {
            driver->state = WXC_DRIVER_STATE_BUSY;
            wmResult = WM_PowerOff(WXCi_StopProc);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_POWER_OFF, wmResult);
        }
        /* ���� */
        else if (cb->apiid == WM_APIID_POWER_OFF)
        {
            WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_STOP, NULL);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_EndProc

  Description:  STOP -> READY �ւ̘A���J��

  Arguments:    arg           �R�[���o�b�N���� (�Ăяo������ NULL ���w��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_EndProc(void *arg)
{
    WXCDriverWork *const driver = work;
    WMCallback *cb = (WMCallback *)arg;

    if (!arg || WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMErrCode wmResult = WM_ERRCODE_SUCCESS;
        /* STOP -> READY */
        if (!arg)
        {
            driver->state = WXC_DRIVER_STATE_BUSY;
            wmResult = WM_Disable(WXCi_EndProc);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_DISABLE, wmResult);
        }
        /* ���� */
        else if (cb->apiid == WM_APIID_DISABLE)
        {
            /* ���C�����X�̊��S�ȉ�������ʒm */
            wmResult = WM_Finish();
            if (WXCi_CheckWmApiResult(driver, WM_APIID_END, wmResult))
            {
                work = NULL;
                WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_END, NULL);
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_ResetProc

  Description:  (any) -> IDLE �ւ̘A���J��

  Arguments:    arg           �R�[���o�b�N���� (�Ăяo������ NULL ���w��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_ResetProc(void *arg)
{
    WXCDriverWork *const driver = work;
    WMCallback *cb = (WMCallback *)arg;

    if (!arg || WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMErrCode wmResult = WM_ERRCODE_SUCCESS;
        /* (any) -> IDLE */
        if (!arg)
        {
            driver->state = WXC_DRIVER_STATE_BUSY;
            wmResult = WM_Reset(WXCi_ResetProc);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_RESET, wmResult);
        }
        /* ���� */
        else if (cb->apiid == WM_APIID_RESET)
        {
            driver->own_aid = 0;
            WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_IDLE, NULL);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_ParentIndicate

  Description:  �e�@�� StartParent �C���W�P�[�g

  Arguments:    arg           �R�[���o�b�N����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_ParentIndicate(void *arg)
{
    WXCDriverWork *const driver = work;
    WMStartParentCallback *cb = (WMStartParentCallback *)arg;

    /* �P�Ȃ��ԑJ�ڂ̏ꍇ�͖������ăV�[�P���X�𑱂��� */
    if (cb->state == WM_STATECODE_PARENT_START)
    {
        WXCi_StartParentProc(arg);
    }
    /* ����ȊO�̓C���W�P�[�g�Ȃ̂� wxc_api.c �փt�b�N������ */
    else if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        switch (cb->state)
        {
        case WM_STATECODE_PARENT_START:
            break;
        case WM_STATECODE_CONNECTED:
            {
                BOOL    mp_start = (driver->peer_bitmap == 0);
                WXC_DRIVER_LOG("connected(%02X+=%02X)\n", driver->peer_bitmap, (1 << cb->aid));
                driver->peer_bitmap |= (u16)(1 << cb->aid);
                (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_CONNECTED, cb);
                /* �ŏ��̎q�@�Ȃ瑗�M�^�C�~���O�ʒm */
                if (mp_start)
                {
                    WXCi_CallSendEvent(driver);
                }
            }
            break;
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        case WM_STATECODE_DISCONNECTED:
            /* �ؒf�ʒm������ PortCallback �ňꌳ�����ꂽ */
            break;
        case WM_STATECODE_BEACON_SENT:
            (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_BEACON_SEND, driver->parent_param);
            break;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_StartParentProc

  Description:  IDLE -> PARENT -> MP_PARENT �ւ̘A���J��

  Arguments:    arg           �R�[���o�b�N���� (�Ăяo������ NULL ���w��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_StartParentProc(void *arg)
{
    WXCDriverWork *const driver = work;
    WMCallback *cb = (WMCallback *)arg;

    if (!arg || WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMErrCode wmResult = WM_ERRCODE_SUCCESS;
        /* IDLE -> IDLE (WM_SetParentParameter) */
        if (!arg)
        {
            driver->state = WXC_DRIVER_STATE_BUSY;
            /* �`�����l����N�������X�V */
            driver->parent_param->channel = driver->current_channel;
            driver->parent_param->tgid = WM_GetNextTgid();
            WXC_DRIVER_LOG("start parent. (%2dch, TGID=%02X, GGID=%04X)\n",
                           driver->current_channel, driver->parent_param->tgid,
                           driver->parent_param->ggid);
            wmResult = WM_SetParentParameter(WXCi_StartParentProc, driver->parent_param);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_SET_P_PARAM, wmResult);
        }
        /* IDLE -> PARENT */
        else if (cb->apiid == WM_APIID_SET_P_PARAM)
        {
            /*
             * �C���W�P�[�g�̓s����, �R�[���o�b�N��
             * WXCi_ParentIndicate() �։I�񂷂�.
             */
            wmResult = WM_StartParent(WXCi_ParentIndicate);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_PARENT, wmResult);
        }
        /* PARENT -> MP_PARENT */
        else if (cb->apiid == WM_APIID_START_PARENT)
        {
            /*
             * �����ł͏�� WXCi_ParentIndicate() ����
             * WM_STATECODE_PARENT_START �̂ݒʒm�����.
             */
            driver->own_aid = 0;
            driver->peer_bitmap = 0;
            wmResult = WM_StartMP(WXCi_StartParentProc,
                                  (u16 *)driver->mp_recv_work, driver->recv_size_max,
                                  (u16 *)driver->mp_send_work, driver->send_size_max,
                                  (u16)(driver->parent_param->CS_Flag ? 0 : 1));
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_MP, wmResult);
        }
        /* ���� */
        else if (cb->apiid == WM_APIID_START_MP)
        {
            WMStartMPCallback *cb = (WMStartMPCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_MP_START:
                WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_PARENT, NULL);
                break;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_ChildIndicate

  Description:  �q�@�� StartConnect �C���W�P�[�g

  Arguments:    arg           �R�[���o�b�N����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_ChildIndicate(void *arg)
{
    WXCDriverWork *const driver = work;

    if (WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMStartConnectCallback *cb = (WMStartConnectCallback *)arg;
        switch (cb->state)
        {
        case WM_STATECODE_CONNECT_START:
        case WM_STATECODE_BEACON_LOST:
            break;

        case WM_STATECODE_CONNECTED:
            /* ��ԑJ�ڒ��̏ꍇ�͖������ăV�[�P���X�𑱂��� */
            if (driver->state != WXC_DRIVER_STATE_CHILD)
            {
                WXCi_StartChildProc(arg);
            }
            break;

        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        case WM_STATECODE_DISCONNECTED:
            /* �J�ڒ��łȂ���΂����Ń��Z�b�g */
            if (driver->state != WXC_DRIVER_STATE_BUSY)
            {
                driver->target = WXC_DRIVER_STATE_PARENT;
                WXCi_ResetProc(NULL);
            }
            else
            {
                driver->target = WXC_DRIVER_STATE_IDLE;
            }
            break;

        default:
            WXCi_ErrorQuit(driver);
            break;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_StartChildProc

  Description:  IDLE -> CHILD -> MP_CHILD �ւ̘A���J��

  Arguments:    arg           �R�[���o�b�N���� (�Ăяo������ NULL ���w��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_StartChildProc(void *arg)
{
    WXCDriverWork *const driver = work;
    WMCallback *cb = (WMCallback *)arg;

    if (!arg || WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMErrCode wmResult = WM_ERRCODE_SUCCESS;
        /* IDLE -> CHILD */
        if (!arg)
        {
            /*
             * �ڑ��O��SSID���Ȃǂ�ݒ肷�邽�߂̃t�b�N�Ăяo��.
             */
            u8      ssid_bak[WM_SIZE_BSSID];
            MI_CpuCopy8(driver->target_bss->ssid, ssid_bak, sizeof(ssid_bak));
            (void)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_CONNECTING, driver->target_bss);
            MI_CpuCopy8(driver->target_bss->ssid + 8, driver->ssid, WM_SIZE_CHILD_SSID);
            MI_CpuCopy8(ssid_bak, driver->target_bss->ssid, sizeof(ssid_bak));
            /*
             * �C���W�P�[�g�̓s����, �R�[���o�b�N��
             * WXCi_ChildIndicate() �։I�񂷂�.
             */
            driver->state = WXC_DRIVER_STATE_BUSY;
            wmResult = WM_StartConnect(WXCi_ChildIndicate, driver->target_bss, driver->ssid);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_CONNECT, wmResult);
        }
        /* CHILD -> MP_CHILD */
        else if (cb->apiid == WM_APIID_START_CONNECT)
        {
            WMStartConnectCallback *cb = (WMStartConnectCallback *)arg;
            /*
             * �����ł͏�� WXCi_ChildIndicate() ����
             * WM_STATECODE_CONNECTED �̂ݒʒm�����.
             */
            driver->own_aid = cb->aid;
            wmResult = WM_StartMP(WXCi_StartChildProc,
                                  (u16 *)driver->mp_recv_work, driver->recv_size_max,
                                  (u16 *)driver->mp_send_work, driver->send_size_max,
                                  (u16)(driver->parent_param->CS_Flag ? 0 : 1));
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_MP, wmResult);
        }
        /* ���� */
        else if (cb->apiid == WM_APIID_START_MP)
        {
            WMStartMPCallback *cb = (WMStartMPCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_MP_START:
                WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_CHILD, cb);
                break;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_MeasureProc

  Description:  IDLE -> (measure) -> IDLE �ւ̘A���J��

  Arguments:    arg           �R�[���o�b�N���� (�Ăяo������ NULL ���w��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_MeasureProc(void *arg)
{
    WXCDriverWork *const driver = work;
    WMMeasureChannelCallback *cb = (WMMeasureChannelCallback *)arg;
    u16     channel = 0;

    /* �v���l�̏����� */
    if (!arg)
    {
        driver->state = WXC_DRIVER_STATE_BUSY;
        driver->current_channel = 0;
        driver->measure_ratio_min = WXC_MAX_RATIO + 1;
    }
    else if (WXCi_CheckWmCallbackResult(driver, cb))
        /* �v�������R�[���o�b�N */
    {
        channel = cb->channel;
        /* ���g�p���̒Ⴂ�`�����l�����擾 (�����l 101% �Ȃ̂Ő擪�͕K���I��) */
        if (driver->measure_ratio_min > cb->ccaBusyRatio)
        {
            driver->measure_ratio_min = cb->ccaBusyRatio;
            driver->current_channel = channel;
        }
        /* �S�`�����l���v������ */
        if (channel == (32 - MATH_CountLeadingZeros(WM_GetAllowedChannel())))
        {
            driver->need_measure_channel = FALSE;
            /* ���݂��Ȃ� PARENT ���Ώۂł���ΐڑ��ֈڍs */
            //WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_IDLE, NULL);
            WXCi_StartParentProc(NULL);
            return;
        }
    }
    /* �v���Ɏ��s (�G���[�I����) */
    else
    {
        driver->need_measure_channel = FALSE;
    }

    /* �S�Ẵ`�����l�������ʂ��珇�Ɍv�� */
    if (driver->need_measure_channel)
    {
        /* 1�t���[���Ɉ��ʐM���Ă���d�g���E���邾���̊Ԋu(ms) */
        const u16 WH_MEASURE_TIME = 30;
        /* �L�����A�Z���X��ED�l�̘_���a */
        const u16 WH_MEASURE_CS_OR_ED = 3;
        /* �����f�[�^�ɂ��o���I�ɗL���Ǝv���邨����ED臒l */
        const u16 WH_MEASURE_ED_THRESHOLD = 17;
        WMErrCode ret;

        channel = WXC_GetNextAllowedChannel(channel);
        ret = WM_MeasureChannel(WXCi_MeasureProc,
                                WH_MEASURE_CS_OR_ED, WH_MEASURE_ED_THRESHOLD,
                                channel, WH_MEASURE_TIME);
        (void)WXCi_CheckWmApiResult(driver, WM_APIID_MEASURE_CHANNEL, ret);
    }

}

/*---------------------------------------------------------------------------*
  Name:         WXCi_ScanProc

  Description:  IDLE -> SCAN -> IDLE �ւ̘A���J��

  Arguments:    arg           �R�[���o�b�N���� (�Ăяo������ NULL ���w��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_ScanProc(void *arg)
{
    WXCDriverWork *const driver = work;
    WMCallback *cb = (WMCallback *)arg;
    
    static u16 scan_channelList;

    if (!arg || WXCi_CheckWmCallbackResult(driver, arg))
    {
        WMErrCode wmResult = WM_ERRCODE_SUCCESS;

        /* IDLE -> SCAN */
        if (!arg)
        {
            driver->state = WXC_DRIVER_STATE_BUSY;
            driver->scan_found_num = 0;
            /* �u���[�h�L���X�g�A�h���X�őS�`�����l����T�� */
            driver->scan_param->scanBuf = (WMBssDesc *)driver->scan_buf;
            driver->scan_param->scanBufSize = WM_SIZE_SCAN_EX_BUF;
            
            /* �T������`�����l���� 1 �`�����l�����ɂ��� */
            scan_channelList = WM_GetAllowedChannel();
            driver->scan_param->channelList = (u16)MATH_GetLeastSignificantBit(scan_channelList);
            scan_channelList = (u16)(scan_channelList ^ MATH_GetLeastSignificantBit(scan_channelList));
            
            driver->scan_param->maxChannelTime = WXC_SCAN_TIME_MAX;
            MI_CpuFill8(driver->scan_param->bssid, 0xFF, sizeof(driver->scan_param->bssid));
            driver->scan_param->scanType = WM_SCANTYPE_PASSIVE;
            driver->scan_param->ssidLength = 0;
            MI_CpuFill8(driver->scan_param->ssid, 0xFF, sizeof(driver->scan_param->ssid));
            wmResult = WM_StartScanEx(WXCi_ScanProc, driver->scan_param);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_SCAN_EX, wmResult);
        }
        /* SCAN -> IDLE */
        else if (cb->apiid == WM_APIID_START_SCAN_EX)
        {
            WMStartScanExCallback *cb = (WMStartScanExCallback *)arg;
            /* �r�[�R�������o�����Εێ� */
            if (cb->state == WM_STATECODE_PARENT_FOUND)
            {
                DC_InvalidateRange(driver->scan_buf, WM_SIZE_SCAN_EX_BUF);
                driver->scan_found_num = cb->bssDescCount;
            }
            wmResult = WM_EndScan(WXCi_ScanProc);
            (void)WXCi_CheckWmApiResult(driver, WM_APIID_END_SCAN, wmResult);
        }
        /* ���� */
        else if (cb->apiid == WM_APIID_END_SCAN)
        {
            BOOL    ret = FALSE;
            /* �Ώۂ��܂� CHILD �ł����BssDesc �̔��� */
            if (driver->target == WXC_DRIVER_STATE_CHILD)
            {
                int     i;
                const u8 *scan_buf = driver->scan_buf;

                WXC_DRIVER_LOG("found:%d beacons\n", driver->scan_found_num);
                for (i = 0; i < driver->scan_found_num; ++i)
                {
                    const WMBssDesc *p_desc = (const WMBssDesc *)scan_buf;
                    const int len = p_desc->length * 2;
                    BOOL    is_valid;
                    is_valid = WM_IsValidGameBeacon(p_desc);
                    WXC_DRIVER_LOG("   GGID=%08X(%2dch:%3dBYTE)\n",
                                   is_valid ? p_desc->gameInfo.ggid : 0xFFFFFFFF,
                                   p_desc->channel, len);
                    if (is_valid)
                    {
                        /* �e BssDesc ���ƂɃR�[���o�b�N */
                        ret =
                            (BOOL)WXCi_CallDriverEvent(driver, WXC_DRIVER_EVENT_BEACON_RECV,
                                                       (void *)p_desc);
                        if (ret)
                        {
                            WXC_DRIVER_LOG("     -> matched!\n");
                            MI_CpuCopy8(p_desc, driver->target_bss, sizeof(WMBssDesc));
                            break;
                        }
                    }
                    scan_buf += MATH_ROUNDUP(len, 4);
                }
                /* �܂��Ώۂ����܂��Ă��炸�A�T�����Ă��Ȃ��`�����l��������Ȃ�΁A�T������ */
                if((ret == FALSE)&&(MATH_GetLeastSignificantBit(scan_channelList) != 0))
                {
                    driver->scan_found_num = 0;
                    driver->scan_param->channelList = (u16)MATH_GetLeastSignificantBit(scan_channelList);
                    scan_channelList = (u16)(scan_channelList ^ MATH_GetLeastSignificantBit(scan_channelList));
                    wmResult = WM_StartScanEx(WXCi_ScanProc, driver->scan_param);
                    (void)WXCi_CheckWmApiResult(driver, WM_APIID_START_SCAN_EX, wmResult);
                    return;
                }
            }
            /* �Ώۂ�����ΐڑ��J�n */
            if (ret)
            {
                WXCi_StartChildProc(NULL);
            }
            /* ���Ȃ���Ύ��̋N�����[�h��I�� */
            else
            {
                if (driver->target == WXC_DRIVER_STATE_CHILD)
                {
                    driver->target = WXC_DRIVER_STATE_IDLE;
                }
                WXCi_OnStateChanged(driver, WXC_DRIVER_STATE_IDLE, NULL);
            }
        }
    }
}

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
void WXC_InitDriver(WXCDriverWork * driver, WMParentParam *pp, WXCDriverEventFunc func, u32 dma)
{
    /* ���荞�݃n���h���̊O�ŏ��� TGID ���擾 */
    {
        OSIntrMode bak_cpsr = OS_EnableInterrupts();
        (void)WM_GetNextTgid();
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
    /* ���[�N�ϐ������� */
    work = driver;
    MI_CpuClear32(driver, sizeof(WXCDriverWork));
    driver->own_aid = 0;
    driver->send_busy = TRUE;
    driver->callback = func;
    driver->wm_dma = (u16)dma;
    driver->send_size_max = (u16)sizeof(driver->mp_send_work);
    driver->recv_size_max = (u16)sizeof(driver->mp_recv_work);
    driver->state = WXC_DRIVER_STATE_END;
    driver->parent_param = pp;
    driver->parent_param->entryFlag = 1;
    driver->parent_param->beaconPeriod = WXC_BEACON_PERIOD;
    driver->parent_param->channel = 1;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_SetDriverTarget

  Description:  ����̏�Ԃ�ڕW�ɑJ�ڂ��J�n����.

  Arguments:    driver        WXCDriverWork �\����
                target        �J�ڐ�̃X�e�[�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_SetDriverTarget(WXCDriverWork * driver, WXCDriverState target)
{
    driver->target = target;
    /* �Ⴄ��ԂɈ��肵�Ă���΂����őJ�ڂ��J�n */
    if ((driver->state != WXC_DRIVER_STATE_BUSY) && (driver->state != driver->target))
    {
        WXCi_OnStateChanged(driver, driver->state, NULL);
    }
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
void WXC_SetDriverSsid(WXCDriverWork * driver, const void *buffer, u32 length)
{
    length = (u32)MATH_MIN(length, WM_SIZE_CHILD_SSID);
    MI_CpuCopy8(buffer, driver->ssid, length);
    MI_CpuFill8(driver->ssid + length, 0x00, (u32)(WM_SIZE_CHILD_SSID - length));
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
