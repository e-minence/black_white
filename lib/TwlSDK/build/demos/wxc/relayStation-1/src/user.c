/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - relayStation-1
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
#include "common.h"
#include "dispfunc.h"


/*****************************************************************************/
/* constant */

/* �e�X�g�p����M�f�[�^�T�C�Y */
#define DATA_SIZE (1024 * 20)

/* ���p���őΉ����� GGDI �ꗗ */
static u32 ggid_list[] =
{
    GGID_ORG_1,
    GGID_ORG_2,
	0,
} ;


/*****************************************************************************/
/* variable */

/* ���������� */
static int data_exchange_count = 0;

/* ����M�p�f�[�^�o�b�t�@ */
static u8 send_data[2][DATA_SIZE];
static u8 recv_data[2][DATA_SIZE];


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
  Name:         CreateData

  Description:  ���M�p�f�[�^���쐬.

  Arguments:    buf           �f�[�^�i�[��.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CreateData(void *buf)
{
	*(OSTick*)buf = OS_GetTick();
	OS_GetMacAddress((u8*)buf + sizeof(OSTick));
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

    const WXCBlockDataFormat * block = (const WXCBlockDataFormat *)arg;
    u8     *recv = (u8 *)block->buffer;

    ++data_exchange_count;

    /* ��M�f�[�^�̕\�� */
	{
	    const u8 *dst_mac = recv + sizeof(OSTick);
		u8      sum = CalcHash(recv);
		if (sum == recv[DATA_SIZE - 1])
		{
			BgPrintf((s16)1, (s16)5, WHITE, "sum OK 0x%02X %6d %6d sec", sum, data_exchange_count,
					OS_TicksToSeconds(OS_GetTick()));
			BgPrintf((s16)1, (s16)6, WHITE, "  (from %02X:%02X:%02X:%02X:%02X:%02X)\n\n",
						dst_mac[0], dst_mac[1], dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5]);
		}
		else
		{
			BgPrintf((s16)1, (s16)5, WHITE, "sum NG 0x%02X %6d %6d sec", sum, data_exchange_count,
					OS_TicksToSeconds(OS_GetTick()));
		}
	}

    /* ����̌����p�f�[�^�o�b�t�@�ݒ� */
    {
		/* �Ή����� GGID ���ƂɓƗ����ăo�b�t�@���Ǘ� */
		const u32 ggid = WXC_GetCurrentGgid();
		int index = 0;
		for ( index = 0 ; (ggid_list[index]|WXC_GGID_COMMON_BIT) != ggid ; ++index )
		{
		}
		/*
		 * �����M�����f�[�^�ɂ���,
		 * �E����̑��M�Ɏg����(���p���[�h)
		 * �E�����œǂݎ̂Ă邩(�z�M���[�h)
		 * �I������.
		 */
		if (keep_flg)
		{
			CreateData(send_data[index]);
		}
		else
		{
		    MI_CpuCopy8(recv, send_data[index], sizeof(send_data[index]));
		}
		/*
		 * �f�[�^�̍Đݒ�.
		 * WXC_RegisterCommonData() �̑��M�f�[�^�� NULL ���w�肵�Ă���
		 * CONNECTED �̒ʒm�i�K�� WXC_AddData() ������@�Ɠ���.
		 */
		send_data[index][DATA_SIZE - 2] = (u8)ggid;
		send_data[index][DATA_SIZE - 1] = CalcHash(send_data[index]);
		MI_CpuClear32(recv, DATA_SIZE);
		WXC_SetInitialData(ggid, send_data[index], DATA_SIZE, recv_data[index], DATA_SIZE);
    }

}


/*---------------------------------------------------------------------------*
  Name:         system_callback

  Description:  WXC ���C�u�����̃V�X�e���R�[���o�b�N

  Arguments:    stat          �ʒm�X�e�[�^�X. (��� WXC_STATE_EXCHANGE_DONE)
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
            void *system_work = arg;
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


void User_Init(void)
{
	int	index;

    /* ���C�u����������ԏ����� */
    WXC_Init(OS_Alloc(WXC_WORK_SIZE), system_callback, 2);

    /* ���p�����[�h�ł͎q�@�ɂ����Ȃ�Ȃ� */
    WXC_SetChildMode(TRUE);

    /* �e GGID ���Ƃ̏���f�[�^�u���b�N�o�^ */
    for ( index = 0 ; ggid_list[index] ; ++index )
    {
        if(passby_ggid[index])
        {
            CreateData(send_data[index]);
            send_data[index][DATA_SIZE - 2] = (u8)ggid_list[index];
            send_data[index][DATA_SIZE - 1] = CalcHash(send_data[index]);
            WXC_RegisterCommonData(ggid_list[index], user_callback, send_data[index], DATA_SIZE, recv_data[index], DATA_SIZE);
        }
    }

    /* ���C�u�����̃��C�����X���N�� */
    WXC_Start();
}
