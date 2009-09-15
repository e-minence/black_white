/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - relayStation-2
  File:     user.c

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-11-15#$
  $Rev: 2414 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#include <nitro/wxc.h>
#include "user.h"
#include "dispfunc.h"


/*****************************************************************************/
/* variable */

/* ���p���������[���[�h�Ȃ� TRUE */
BOOL    station_is_relay;

/* �����p�f�[�^�o�b�t�@ */
u8      send_data[DATA_SIZE];
static u8 recv_data[DATA_SIZE];

/* ��L���M�f�[�^�̌��̏��L�� */
u8      send_data_owner[6];

/* ���������� */
static int data_exchange_count = 0;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         CalcHash

  Description:  �ȈՃ`�F�b�N�p�̃n�b�V���v�Z

  Arguments:    src           �v�Z�Ώۂ̃o�b�t�@

  Returns:      �v�Z�l.
 *---------------------------------------------------------------------------*/
static u8 CalcHash(const u8 *src)
{
    int     i;
    u8      sum = 0;
    for (i = 0; i < DATA_SIZE - 1; i++)
    {
        sum ^= src[i];
    }
    return sum;
}

/*---------------------------------------------------------------------------*
  Name:         user_callback

  Description:  �f�[�^���������R�[���o�b�N

  Arguments:    stat          �ʒm�X�e�[�^�X. (��� WXC_STATE_EXCHANGE_DONE)
                arg           �ʒm����. (��M�f�[�^�o�b�t�@)

  Returns:      �v�Z�l.
 *---------------------------------------------------------------------------*/
static void user_callback(WXCStateCode stat, void *arg)
{
#pragma unused(stat)

    /* �����ɂ͎�M�f�[�^�̏�񂪗^������ */
    const WXCBlockDataFormat *recv = (const WXCBlockDataFormat *)arg;

    ++data_exchange_count;

    /*
     * ���p���������[���[�h�Ȃ�, ���M���ׂ��f�[�^��
     * ���������܎�M�����f�[�^�ōēo�^����.
     */
    if (station_is_relay)
    {
        /* ��M����� MAC �A�h���X��ۑ� */
        const WXCUserInfo *info = WXC_GetUserInfo((u16)((WXC_GetOwnAid() == 0) ? 1 : 0));
        MI_CpuCopy8(info->macAddress, send_data_owner, 6);
        /* �����p�f�[�^���ēo�^ */
        MI_CpuCopy8(recv->buffer, send_data, sizeof(send_data));
        MI_CpuClear32(recv->buffer, DATA_SIZE);
        WXC_SetInitialData(GGID_ORG_1, send_data, DATA_SIZE, recv_data, DATA_SIZE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         system_callback

  Description:  WXC ���C�u�����̃V�X�e���R�[���o�b�N

  Arguments:    stat          �ʒm�X�e�[�^�X.
                arg           �ʒm����. (��M�f�[�^�o�b�t�@)

  Returns:      �v�Z�l.
 *---------------------------------------------------------------------------*/
static void system_callback(WXCStateCode state, void *arg)
{
    switch (state)
    {
    case WXC_STATE_READY:
        /*
         * WXC_Init �֐��Ăяo���̓������甭��.
         * arg �͏�� NULL.
         */
        break;

    case WXC_STATE_ACTIVE:
        /*
         * WXC_Start �֐��Ăяo���̓������甭��.
         * arg �͏�� NULL.
         */
        break;

    case WXC_STATE_ENDING:
        /*
         * WXC_End �֐��Ăяo���̓������甭��.
         * arg �͏�� NULL.
         */
        break;

    case WXC_STATE_END:
        /*
         * WXC_End �֐��ɂ��I�������������ɔ���.
         * arg �� WXC_Init �֐��Ŋ��蓖�Ă��������[�N������.
         * ���̎��_�Ń��[�N�������̓��[�U�ɉ������Ă��܂�.
         */
        {
            void   *system_work = arg;
            OS_Free(system_work);
        }
        break;

    case WXC_STATE_EXCHANGE_DONE:
        /*
         * �f�[�^��������. (����͔������܂���)
         */
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         User_Init

  Description:  WXC ���C�u�����Ɋւ��郆�[�U������������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void User_Init(void)
{
    /* WXC���C�u����������ԏ����� */
    WXC_Init(OS_Alloc(WXC_WORK_SIZE), system_callback, 2);

    /* ���p�����[�h�ł͎q�@�ɂ����Ȃ�Ȃ� */
    WXC_SetChildMode(TRUE);

    /*
     * �����f�[�^�u���b�N�̓o�^.
     * ����ȍ~�̓����[���[�h�Ŏ�M�����ꍇ�ȊO
     * �o�^�f�[�^���X�V����邱�Ƃ͖���.
     */
    OS_GetMacAddress(send_data_owner);
    (void)OS_SPrintf((char *)send_data, "data %02X:%02X:%02X:%02X:%02X:%02X",
                     send_data_owner[0], send_data_owner[1], send_data_owner[2],
                     send_data_owner[3], send_data_owner[4], send_data_owner[5]);
    send_data[DATA_SIZE - 1] = CalcHash(send_data);
    WXC_RegisterCommonData(GGID_ORG_1, user_callback, send_data, DATA_SIZE, recv_data, DATA_SIZE);

    /* WXC���C�u�����̃��C�����X���N�� */
    WXC_Start();
}
