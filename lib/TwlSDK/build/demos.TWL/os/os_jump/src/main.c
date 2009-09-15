/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos.TWL - os - os_jump
  File:     main.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-08#$
  $Rev: 8901 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/

#include <twl.h>
#include <stdlib.h>

#include "screen.h"

/*---------------------------------------------------------------------------*
    �萔 ��`
 *---------------------------------------------------------------------------*/
#define KEY_REPEAT_START    25  // �L�[���s�[�g�J�n�܂ł̃t���[����
#define KEY_REPEAT_SPAN     10  // �L�[���s�[�g�̊Ԋu�t���[����

/*---------------------------------------------------------------------------*
    �\���� ��`
 *---------------------------------------------------------------------------*/

// �L�[���͏��
typedef struct KeyInfo
{
    u16 cnt;    // �����H���͒l
    u16 trg;    // �����g���K����
    u16 up;     // �����g���K����
    u16 rep;    // �����ێ����s�[�g����
} KeyInfo;

// �L�[����
static KeyInfo  gKey;

/*---------------------------------------------------------------------------*
   Prototype
 *---------------------------------------------------------------------------*/

static void VBlankIntr(void);
static void InitInterrupts(void);
static void InitHeap(void);

static void ReadKey(KeyInfo* pKey);

/*---------------------------------------------------------------------------*/

void TwlMain(void)
{
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    InitHeap();
    InitScreen();
    InitInterrupts();

    GX_DispOn();
    GXS_DispOn();

    ClearScreen();

    // �L�[���͏��擾�̋�Ăяo��(IPL �ł� A �{�^�������΍�)
    ReadKey(&gKey);

    while(TRUE)
    {
        // �L�[���͏��擾
        ReadKey(&gKey);

        // ���C����ʃN���A
        ClearScreen();
        
        PutMainScreen(0, 0, 0xff, "OS_IsRebooted = %s", OS_IsRebooted()? "TRUE":"FALSE");
        
        PutSubScreen(0, 0, 0xff, "APPLICATION JUMP DEMO");
        PutSubScreen(0, 1, 0xff, "  A: OS_JumpToSystemMenu");
        PutSubScreen(0, 2, 0xff, "  B: OS_RebootSystem");
        PutSubScreen(0, 3, 0xff, "  X: OS_JumpToEULAViewer");
        PutSubScreen(0, 4, 0xff, "  Y: OS_JumpToInternetSetting");
        PutSubScreen(0, 5, 0xff, "  R: OS_JumpToWirelessSetting");
        
        if (gKey.trg & PAD_BUTTON_A)
        {
            if (FALSE == OS_JumpToSystemMenu() )
            {
                //�W�����v�Ɏ��s
                OS_Panic("OS_JumpToSystemMenu() is Failed");
            }
        }

        if (gKey.trg & PAD_BUTTON_B)
        {
            if (FALSE == OS_RebootSystem() )
            {
                //�W�����v�Ɏ��s
                OS_Panic("OS_RebootSystem() is Failed");
            }
        }

        if (gKey.trg & PAD_BUTTON_X)
        {
            if (FALSE == OS_JumpToEULAViewer() )
            {
                //�W�����v�Ɏ��s
                OS_Panic("OS_JumpToEULAViewer() is Failed");
            }
        }

        if (gKey.trg & PAD_BUTTON_Y)
        {
            if (FALSE == OS_JumpToInternetSetting() )
            {
                //�W�����v�Ɏ��s
                OS_Panic("OS_JumpToInternetSetting() is Failed");
            }
        }
        if (gKey.trg & PAD_BUTTON_R)
        {
            if (FALSE == OS_JumpToWirelessSetting() )
            {
                //�W�����v�Ɏ��s
                OS_Panic("OS_JumpToWirelessSetting() is Failed");
            }
        }
        // �u�u�����N�҂� ( �X���b�h�Ή� )
        OS_WaitVBlankIntr();
    }

    OS_Terminate();
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃n���h���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    // �e�L�X�g�\�����X�V
    UpdateScreen();

    // IRQ �`�F�b�N�t���O���Z�b�g
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         InitInterrupts

  Description:  ���荞�ݐݒ������������B
                V �u�����N���荞�݂������A���荞�݃n���h����ݒ肷��B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitInterrupts(void)
{
    // V �u�����N���荞�ݐݒ�
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);

    // ���荞�݋���
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
}

/*---------------------------------------------------------------------------*
  Name:         InitHeap

  Description:  ���C����������̃A���[�i�ɂă����������ăV�X�e��������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitHeap(void)
{
    void*           tempLo;
    OSHeapHandle    hh;

    // ���C����������̃A���[�i�Ƀq�[�v���ЂƂ쐬
    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        // �q�[�v�쐬�Ɏ��s�����ꍇ�ُ͈�I��
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}

/*---------------------------------------------------------------------------*
  Name:         ReadKey

  Description:  �L�[���͏����擾���A���͏��\���̂�ҏW����B
                �����g���K�A�����g���K�A�����p�����s�[�g�g���K �����o����B

  Arguments:    pKey  - �ҏW����L�[���͏��\���̂��w�肷��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ReadKey(KeyInfo* pKey)
{
    static u16  repeat_count[12];
    int         i;
    u16         r;

    r = PAD_Read();
    pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < 12; i++)
    {
        if (r & (0x0001 << i))
        {
            if (!(pKey->cnt & (0x0001 << i)))
            {
                pKey->trg |= (0x0001 << i);     // �����g���K
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // �����p�����s�[�g
                    repeat_count[i] = (u16) (KEY_REPEAT_START - KEY_REPEAT_SPAN);
                }
                else
                {
                    repeat_count[i]++;
                }
            }
        }
        else
        {
            if (pKey->cnt & (0x0001 << i))
            {
                pKey->up |= (0x0001 << i);      // �����g���K
            }
        }
    }

    pKey->cnt = r;  // �����H�L�[����
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
