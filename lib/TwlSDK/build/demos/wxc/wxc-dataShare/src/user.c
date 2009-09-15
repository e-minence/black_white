/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - wxc-dataShare
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
#include "disp.h"
#include "gmain.h"
#include "wh.h"

static int data_exchange_count = 0;

/**** �f�o�b�O�p�f�[�^�o�b�t�@ ****/
#define DATA_SIZE 1024*20
static u8 send_data[DATA_SIZE];
static u8 recv_data[DATA_SIZE];

/* �e�X�g�p�� GGID */
#define SDK_MAKEGGID_SYSTEM(num)              (0x003FFF00 | (num))
#define GGID_ORG_1                            SDK_MAKEGGID_SYSTEM(0x52)
/*---------------------------------------------------------------------------*
    �O���ϐ���`
 *---------------------------------------------------------------------------*/
extern int passby_endflg;
extern WMBssDesc bssdesc;
extern WMParentParam parentparam;
extern u8 macAddress[6];

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

    const WXCBlockDataFormat * block = (const WXCBlockDataFormat *)arg;
    u8      sum = 0;
    u8     *data = (u8 *)block->buffer;

    ++data_exchange_count;

    /* debug ��M�f�[�^�̕\�� */
    sum = CalcHash(data);

    if (sum == data[DATA_SIZE - 1])
    {
        OS_TPrintf("sum OK 0x%02X %6d %6d sec\n", sum, data_exchange_count,
                OS_TicksToSeconds(OS_GetTick()));
        OS_TPrintf("%s\n", data);
    }
    else
    {
        OS_TPrintf("sum NG 0x%02X sum 0x0%02X %6d sec", sum, data[DATA_SIZE - 1],
               OS_TicksToSeconds(OS_GetTick()));
    }
    MI_CpuClear32(data, DATA_SIZE);
    
    // ���ʐM������I������悤�ɂ���
    
    if( WXC_IsParentMode() == TRUE)
    {
        const WMParentParam *param;
        
        passby_endflg = 1;
        
        // �e�̏����擾���Ă���
        param = WXC_GetParentParam();
        MI_CpuCopyFast( param, &parentparam, sizeof(WMParentParam) );        
    }
    else
    {
        const WMBssDesc *bss;
        
        passby_endflg = 2;
        // �e��MAC�A�h���X��Get
        bss = WXC_GetParentBssDesc();
        MI_CpuCopyFast( bss, &bssdesc, sizeof(WMBssDesc) ); 
    }
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
            MI_CpuCopy16( user->macAddress, macAddress, sizeof(u8)*6 ); 
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
    u8      MacAddress[6];
    
    
    /* ���C�u����������ԏ����� */
    WXC_Init(OS_Alloc(WXC_WORK_SIZE), system_callback, 2);

    /* ���ǃf�[�^�̃Z�b�g */
    OS_GetMacAddress(MacAddress);
    (void)OS_SPrintf((char *)send_data, "data %02X:%02X:%02X:%02X:%02X:%02X",
                  MacAddress[0], MacAddress[1], MacAddress[2],
                  MacAddress[3], MacAddress[4], MacAddress[5]);

    send_data[DATA_SIZE - 1] = CalcHash(send_data);

    /* ���ǃf�[�^�̓o�^ */
    /* ����ǃf�[�^��M�p�̃o�b�t�@�A�f�[�^�T�C�Y�͎��ǃf�[�^�T�C�Y�Ɠ��� */
    WXC_RegisterCommonData(GGID_ORG_1, user_callback, send_data, DATA_SIZE, recv_data, DATA_SIZE);
    
    /* ���C�u�����̃��C�����X���N�� */
    WXC_Start();
}

