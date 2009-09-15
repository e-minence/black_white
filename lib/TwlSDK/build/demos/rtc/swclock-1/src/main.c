/*---------------------------------------------------------------------------*
  Project:  TwlSDK - RTC - demos - swclock-1
  File:     main.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-11-02#$
  $Rev: 2048 $
  $Author: tokunaga_eiji $
 *---------------------------------------------------------------------------*/

#ifdef SDK_TWL
#include <twl.h>
#include <twl/rtc.h>
#else
#include <nitro.h>
#include <nitro/rtc.h>
#endif

#include    "font.h"


/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
#define     KEY_REPEAT_START    25     // �L�[���s�[�g�J�n�܂ł̃t���[����
#define     KEY_REPEAT_SPAN     10     // �L�[���s�[�g�̊Ԋu�t���[����


/*---------------------------------------------------------------------------*
    ������萔��`
 *---------------------------------------------------------------------------*/
// �j��
const char *StrWeek[7] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
// �L�[���͏��
typedef struct KeyInformation
{
    u16     cnt;                       // �����H���͒l
    u16     trg;                       // �����g���K����
    u16     up;                        // �����g���K����
    u16     rep;                       // �����ێ����s�[�g����

}
KeyInformation;

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void);
static void AlarmIntrCallback(void);

static void KeyRead(KeyInformation * pKey);
static void ClearString(void);
static void PrintString(s16 x, s16 y, u8 palette, char *text, ...);
static void ColorString(s16 x, s16 y, s16 length, u8 palette);


/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static u16 gScreen[32 * 32];           // ���z�X�N���[��
static KeyInformation gKey;            // �L�[����

static RTCDate   gRtcDate;             // RTC ����擾������t
static RTCTime   gRtcTime;             // RTC ����擾���鎞��
static RTCDate   gSWClockDate;         // �\�t�g�E�F�A�N���b�N����擾������t
static RTCTimeEx gSWClockTimeEx;       // �\�t�g�E�F�A�N���b�N����擾���鎞��

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    // �e�평����
    OS_Init();
    FX_Init();
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    // �\���ݒ菉����
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();
    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);

    // ������\���p�ɂQ�c�\���ݒ�
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf800,      // SCR �x�[�X�u���b�N 31
                     GX_BG_CHARBASE_0x00000,    // CHR �x�[�X�u���b�N 0
                     GX_BG_EXTPLTT_01);
    G2_SetBG0Priority(0);
    G2_BG0Mosaic(FALSE);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    GX_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));
    MI_CpuFillFast((void *)gScreen, 0, sizeof(gScreen));
    DC_FlushRange(gScreen, sizeof(gScreen));
    /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    // �����ݐݒ�
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();


    //****************************************************************
    // RTC������
    RTC_Init();
    // �`�b�N������
    OS_InitTick();
    // �\�t�g�E�F�A�N���b�N������
    (void)RTC_InitSWClock();
    //****************************************************************

    // LCD�\���J�n
    GX_DispOn();
    GXS_DispOn();

    // �f�o�b�O������o��
    OS_Printf("ARM9: SWClock demo started.\n");

    // �L�[���͏��擾�̋�Ăяo��(IPL �ł� A �{�^�������΍�)
    KeyRead(&gKey);

    // ���C�����[�v
    while (TRUE)
    {
        // �L�[���͏��擾
        KeyRead(&gKey);

        // �X�N���[���N���A
        ClearString();

        //****************************************************************
        // RTC ������t�E������ǂݏo��
        (void)RTC_GetDateTime(&gRtcDate, &gRtcTime);
        // �\�t�g�E�F�A�N���b�N������t�E������ǂݏo��
        (void)RTC_GetDateTimeExFromSWClock(&gSWClockDate, &gSWClockTimeEx);
        //****************************************************************

        // RTC ����擾�������t��\��
        PrintString(1, 3, 0xf, "RTCDate:");
        PrintString(10, 3, 0xf, "%04d/%02d/%02d/%s",
                    gRtcDate.year + 2000, gRtcDate.month, gRtcDate.day, StrWeek[gRtcDate.week]);
        // �\�t�g�E�F�A�N���b�N����擾�������t��\��
        PrintString(1, 5, 0xf, "SWCDate:");
        PrintString(10, 5, 0xf, "%04d/%02d/%02d/%s",
                    gSWClockDate.year + 2000, gSWClockDate.month, gSWClockDate.day, StrWeek[gSWClockDate.week]);

        // RTC ����擾����������\��
        PrintString(1, 8, 0xf, "RTCTime:");
        PrintString(10, 8, 0xf, "%02d:%02d:%02d", gRtcTime.hour, gRtcTime.minute, gRtcTime.second);
        // �\�t�g�E�F�A�N���b�N����擾����������\��
        PrintString(1, 10, 0xf, "SWCTime:");        
        PrintString(10, 10, 0xf, "%02d:%02d:%02d:%03d", gSWClockTimeEx.hour, gSWClockTimeEx.minute,
                    gSWClockTimeEx.second, gSWClockTimeEx.millisecond);

        // �\�t�g�E�F�A�N���b�N����擾���� 2000�N1��1��0��00������̑��`�b�N�l��\��
        PrintString(1, 13, 0xf, "SWCTick:");
        PrintString(10, 13, 0xf, "%llu", RTC_GetSWClockTick() );        
        
        // �{�^�������������\��
        PrintString(1, 19, 0x3, "START > Synclonize SWC with RTC.");
        PrintString(1, 20, 0x3, "A     > Go to sleep.");
        PrintString(1, 21, 0x3, "B     > Wake up from sleep.");


        // �{�^�����͑���
        // START �L�[
        if ((gKey.trg) & PAD_BUTTON_START)
        {
            OS_TPrintf("Synchronizing SWC with RTC.\n");
            (void)RTC_SyncSWClock();
        }

        // �` �{�^��
        if (gKey.trg & PAD_BUTTON_A)
        {
            OS_TPrintf("Going to sleep... Press B button to wake up.\n");
            PM_GoSleepMode( PM_TRIGGER_KEY, PM_PAD_LOGIC_AND, PAD_BUTTON_B );
        }

        // �u�u�����N�҂�
        OS_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃x�N�g���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    // ���z�X�N���[����VRAM�ɔ��f
    DC_FlushRange(gScreen, sizeof(gScreen));
    /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    // IRQ �`�F�b�N�t���O���Z�b�g
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  �L�[���͏���ҏW����B
                �����g���K�A�����g���K�A�����p�����s�[�g�����o�B

  Arguments:    pKey  - �ҏW����L�[���͏��\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void KeyRead(KeyInformation * pKey)
{
    static u16 repeat_count[12];
    int     i;
    u16     r;

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
                    repeat_count[i] = KEY_REPEAT_START - KEY_REPEAT_SPAN;
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
    pKey->cnt = r;                     // �����H�L�[����
}

/*---------------------------------------------------------------------------*
  Name:         ClearString

  Description:  ���z�X�N���[�����N���A����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ClearString(void)
{
    MI_CpuClearFast((void *)gScreen, sizeof(gScreen));
}

/*---------------------------------------------------------------------------*
  Name:         PrintString

  Description:  ���z�X�N���[���ɕ������z�u����B�������32�����܂ŁB

  Arguments:    x       - ������̐擪��z�u���� x ���W( �~ 8 �h�b�g )�B
                y       - ������̐擪��z�u���� y ���W( �~ 8 �h�b�g )�B
                palette - �����̐F���p���b�g�ԍ��Ŏw��B
                text    - �z�u���镶����B�I�[������NULL�B
                ...     - ���z�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintString(s16 x, s16 y, u8 palette, char *text, ...)
{
    va_list vlist;
    char    temp[32 + 2];
    s32     i;

    va_start(vlist, text);
    (void)vsnprintf(temp, 33, text, vlist);
    va_end(vlist);

    *(u16 *)(&temp[32]) = 0x0000;
    for (i = 0;; i++)
    {
        if (temp[i] == 0x00)
        {
            break;
        }
        gScreen[((y * 32) + x + i) % (32 * 32)] = (u16)((palette << 12) | temp[i]);
    }
}

/*---------------------------------------------------------------------------*
  Name:         ColorString

  Description:  ���z�X�N���[���ɔz�u����������̐F��ύX����B

  Arguments:    x       - �F�ύX���J�n���� x ���W( �~ 8 �h�b�g )�B
                y       - �F�ύX���J�n���� y ���W( �~ 8 �h�b�g )�B
                length  - �A�����ĐF�ύX���镶�����B
                palette - �����̐F���p���b�g�ԍ��Ŏw��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ColorString(s16 x, s16 y, s16 length, u8 palette)
{
    s32     i;
    u16     temp;
    s32     index;

    if (length < 0)
        return;

    for (i = 0; i < length; i++)
    {
        index = ((y * 32) + x + i) % (32 * 32);
        temp = gScreen[index];
        temp &= 0x0fff;
        temp |= (palette << 12);
        gScreen[index] = temp;
    }
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
