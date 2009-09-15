/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - unregister-1
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
#include "font.h"
#include "text.h"


/*****************************************************************************/
/* constant */

/* �e�X�g�p�� GGID */
#define SDK_MAKEGGID_SYSTEM(num)              (0x003FFF00 | (num))
#define GGID_ORG_1                            SDK_MAKEGGID_SYSTEM(0x55)

/* �e�X�g�p����M�f�[�^�T�C�Y */
#define DATA_SIZE (1024 * 20)


/*****************************************************************************/
/* variable */

/* ���������� */
static int data_exchange_count = 0;

/* �e�X�g�p�f�[�^�o�b�t�@ */
static u8 send_data[DATA_SIZE];
static u8 recv_data[DATA_SIZE];


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
    u8      sum = 0;
    u8     *data = (u8 *)block->buffer;

	SDK_ASSERT(stat == WXC_STATE_EXCHANGE_DONE);
    ++data_exchange_count;

    /* debug ��M�f�[�^�̕\�� */
    sum = CalcHash(data);

    if (sum == data[DATA_SIZE - 1])
    {
        mprintf("sum OK 0x%02X %6d %6d sec\n", sum, data_exchange_count,
                OS_TicksToSeconds(OS_GetTick()));

		/* ��M�����f�[�^�������̂��̂��đ������� */
		{
			u8 mac[6];
			const u8 *dst_mac = data + sizeof(OSTick);
			int i;

			OS_GetMacAddress(mac);
			for ( i = 0 ; (i < sizeof(mac)) && (dst_mac[i] == mac[i]) ; ++i )
			{
			}
			/* �������ȑO�ɑ������f�[�^�Ȃ�, �ēx�f�[�^���� */
			if(i >= sizeof(mac))
			{
				mprintf("  (own data before %6d sec)\n",
					OS_TicksToSeconds(OS_GetTick() - *(OSTick*)data));
				CreateData(send_data);
			}
			/* �V��������̃f�[�^�Ȃ炻�̂܂ܕԐM */
			else
			{
				mprintf("  (from %02X:%02X:%02X:%02X:%02X:%02X)\n\n",
					dst_mac[0], dst_mac[1], dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5]);
				MI_CpuCopy8(data, send_data, sizeof(send_data));
			}
			/*
			 * �f�[�^�� Unregister ���čēx Register ���邽�߂Ɉꎞ��~.
			 * ���݂̃��C�u�����ł� WXC_SetInitialData() ���g�p���邱�Ƃɂ����
			 * ���C�����X���ꎞ��~�����Ƃ����̏�œ����̏������\����,
			 * �{�T���v���� WXC_UnregisterData() �̎g�p���@���������̂Ȃ̂�
			 * �����Ă��̕��@���̗p���Ă���.
			 */
			WXC_Stop();
		}
    }
    else
    {
        mprintf("sum NG 0x%02X sum 0x0%02X %6d sec\n", sum, data[DATA_SIZE - 1],
                OS_TicksToSeconds(OS_GetTick()));
    }

    MI_CpuClear32(data, DATA_SIZE);
}

/*---------------------------------------------------------------------------*
  Name:         system_callback

  Description:  WXC ���C�u�����̃V�X�e���R�[���o�b�N

  Arguments:    stat          �ʒm�X�e�[�^�X.
                arg           �ʒm����. (�ʒm�X�e�[�^�X�ɂ���ĈӖ����قȂ�)

  Returns:      �v�Z�l.
 *---------------------------------------------------------------------------*/
static void system_callback(WXCStateCode state, void *arg)
{
	switch (state)
	{
	case WXC_STATE_READY:
		/*
		 * WXC_Init �֐��Ăяo���̓�������A�܂���WXC_Stop�֐��̊������_�Ŕ���.
		 * arg �͏�� NULL.
		 */

		/* �����̓o�^�f�[�^������΂����Ŕj�� */
		if (data_exchange_count > 0)
		{
			WXC_UnregisterData(GGID_ORG_1);
		}
		/* ����̌����p�f�[�^��V���ɓo�^ */
		send_data[DATA_SIZE - 2] = (u8)GGID_ORG_1;
		send_data[DATA_SIZE - 1] = CalcHash(send_data);
		WXC_RegisterCommonData(GGID_ORG_1, user_callback, send_data, DATA_SIZE, recv_data, DATA_SIZE);
		/* ���C�u�����̃��C�����X���N�� */
		WXC_Start();
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

	case WXC_STATE_CONNECTED:
		/*
		 * �V�K�̐ڑ������������ꍇ�ɔ���.
		 * arg �͐ڑ��Ώۂ����� WXCUserInfo �\���̂ւ̃|�C���^.
		 */
		{
			const WXCUserInfo * user = (const WXCUserInfo *)arg;
			OS_TPrintf("connected(%2d):%02X:%02X:%02X:%02X:%02X:%02X\n",
				user->aid,
				user->macAddress[0], user->macAddress[1], user->macAddress[2],
				user->macAddress[3], user->macAddress[4], user->macAddress[5]);
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
    /* ����̌����p�f�[�^�������� */
    data_exchange_count = 0;
    CreateData(send_data);
    /* ���C�u����������ԏ����� */
    WXC_Init(OS_Alloc(WXC_WORK_SIZE), system_callback, 2);
}
