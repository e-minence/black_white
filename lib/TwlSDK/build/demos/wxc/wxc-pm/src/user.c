/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - wxc-pm
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

/* �e�X�g�p�� GGID */
#define SDK_MAKEGGID_SYSTEM(num)              (0x003FFF00 | (num))
#define GGID_ORG_1                            SDK_MAKEGGID_SYSTEM(0x53)
#define GGID_ORG_2                            SDK_MAKEGGID_SYSTEM(0x54)

/* �e�X�g�p����M�f�[�^�T�C�Y */
#define DATA_SIZE (1024 * 20)


/*****************************************************************************/
/* variable */

/* ���������� */
static int data_exchange_count = 0;

/* �e�X�g�p�f�[�^�o�b�t�@ */
static u8 send_data[DATA_SIZE];
static u8 recv_data[DATA_SIZE];

static u8 send_data2[DATA_SIZE];
static u8 recv_data2[DATA_SIZE];

extern int passby_endflg;
extern BOOL passby_ggid[MENU_MAX];     // ���ꂿ�����ʐM������GGID

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
        BgPrintf((s16)1, (s16)5, WHITE, "sum OK 0x%02X %6d %6d sec", sum, data_exchange_count,
                OS_TicksToSeconds(OS_GetTick()));
        BgPrintf((s16)1, (s16)6, WHITE, "%s", data);
    }
    else
    {
        BgPrintf((s16)1, (s16)5, WHITE, "sum NG 0x%02X %6d %6d sec", sum, data_exchange_count,
                OS_TicksToSeconds(OS_GetTick()));
    }
    MI_CpuClear32(data, DATA_SIZE);
    
    // ���ʐM������I������悤�ɂ���
    
    if( WXC_IsParentMode() == TRUE)
    {
        passby_endflg = 1;
    }
    else
    {
        passby_endflg = 2;
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
    /* ���C�u����������ԏ����� */
    WXC_Init(OS_Alloc(WXC_WORK_SIZE), system_callback, 2);

    /*
     * �f�[�^�u���b�N�̓o�^.
     * ����, ����M�f�[�^�T�C�Y�͎����Ƃ������Ƃ���.
     */
    // �t���O�������Ă�Γo�^
    if(passby_ggid[0] == TRUE)
    {
        (void)OS_SPrintf((char *)send_data, "---------------------");
        send_data[DATA_SIZE - 2] = (u8)GGID_ORG_1;
        send_data[DATA_SIZE - 1] = CalcHash(send_data);
        WXC_RegisterCommonData(GGID_ORG_1, user_callback, send_data, DATA_SIZE, recv_data, DATA_SIZE);
    }
    
    /*
     * �e�X�g�Ƃ��Ă���1�f�[�^�u���b�N�o�^.
     * ���d�l�ł͊e�X�̃f�[�^�u���b�N���������őI����,
     * ���̂Ƃ��ɓ���GGID�����q�@�ƃf�[�^��������.
     */
    // �t���O�������Ă�Γo�^
    
    if(passby_ggid[1] == TRUE)
    {
        (void)OS_SPrintf((char *)send_data2, "/////////////////////");
        send_data2[DATA_SIZE - 2] = (u8)GGID_ORG_2;
        send_data2[DATA_SIZE - 1] = CalcHash(send_data2);
        WXC_RegisterCommonData(GGID_ORG_2, user_callback, send_data2, DATA_SIZE, recv_data2, DATA_SIZE);
    }
    
    /* ���C�u�����̃��C�����X���N�� */
    WXC_Start();
}
