/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos.TWL - snd - IIRFilterdemo
  File:     main.c

  Copyright 2008-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    A sample that use SNDEX library.

    USAGE:
        UP , DOWN    : Switch item to control.
        LEFT , RIGHT : Set config related to selected item.

    HOWTO:
        1. Initialize SNDEX library.
        2. Call API to set/get each config.
 *---------------------------------------------------------------------------*/

#include    <twl.h>
#include    "font.h"
#include    "snd_data.h"

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
#define     INDEX_MAX   9

#define     BAND_FOR_IIR_RIGHT       1
#define     BAND_FOR_IIR_LEFT       -1
#define     BAND_FOR_IIR_A          10
#define     BAND_FOR_IIR_B         -10
#define     BAND_FOR_IIR_X         100
#define     BAND_FOR_IIR_Y        -100
#define     BAND_FOR_IIR_R        1000
#define     BAND_FOR_IIR_L       -1000
#define     BAND_FOR_IIR_START   10000
#define     BAND_FOR_IIR_SELECT -10000

/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static u16      screen[32 * 32] ATTRIBUTE_ALIGN(HW_CACHE_LINE_SIZE);

static SNDEXIirFilterParam  FilterParam;
static SNDEXIirFilterTarget FilterTarget;

static const char* FilterTargetStrings[] =
{
    "ADC_1", "ADC_2", "ADC_3", "ADC_4", "ADC_5", "DAC_LEFT_1", "DAC_LEFT_2", "DAC_LEFT_3", "DAC_LEFT_4", "DAC_LEFT_5",
    "DAC_RIGHT_1", "DAC_RIGHT_2", "DAC_RIGHT_3", "DAC_RIGHT_4", "DAC_RIGHT_5", "DAC_BOTH_1", "DAC_BOTH_2", "DAC_BOTH_3",
    "DAC_BOTH_4", "DAC_BOTH_5"
};

static s32      index     =   0;
static SNDEXMute        mute    =   SNDEX_MUTE_OFF;
static u8               volume  =   0;

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/

static void     UpdateValue     (u16* target, s16 value);
static BOOL     CheckResult     (SNDEXResult result);
static void     UpdateScreen    (void);
static void     VBlankIntr      (void);
static void     PrintString     (s16 x, s16 y, u8 palette, char *text, ...);

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.s

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
TwlMain (void)
{
    /* ������ */
    OS_Init();
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    /* �\���ݒ菉���� */
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();
    MI_CpuFillFast((void*)HW_OAM, GX_LCD_SIZE_Y, HW_OAM_SIZE);
    MI_CpuClearFast((void*)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void*)HW_DB_OAM, GX_LCD_SIZE_Y, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void*)HW_DB_PLTT, HW_DB_PLTT_SIZE);
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256,
            GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xf800,      // SCR �x�[�X�u���b�N 31
            GX_BG_CHARBASE_0x00000,    // CHR �x�[�X�u���b�N 0
            GX_BG_EXTPLTT_01);
    G2_SetBG0Priority(0);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    GX_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));
    MI_CpuClearFast((void*)screen, sizeof(screen));
    DC_FlushRange(screen, sizeof(screen));
    GX_LoadBG0Scr(screen, 0, sizeof(screen));

    /* �����ݐݒ� */
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    /* �T�E���h������ */
    SND_Init();
    SND_AssignWaveArc((SNDBankData*)sound_bank_data, 0, (SNDWaveArc*)sound_wave_data);
    SND_StartSeq(0, sound_seq_data, 0, (SNDBankData*)sound_bank_data);

    /* �T�E���h�g���@�\������ */
    SNDEX_Init();
    (void)CheckResult(SNDEX_GetMute(&mute));
    (void)CheckResult(SNDEX_GetVolume(&volume));
    
    /* �t�B���^�p�����[�^������ */
    FilterParam.n0 = 0;
    FilterParam.n1 = 0;
    FilterParam.n2 = 0;
    FilterParam.d1 = 0;
    FilterParam.d2 = 0;
    FilterTarget   = SNDEX_IIR_FILTER_DAC_LEFT_1;
    UpdateScreen();

    /* LCD �\���J�n */
    GX_DispOn();
    GXS_DispOn();

    {
        u16     keyOld  =   PAD_Read();
        u16     keyTrg;
        u16     keyNow;
    
        /* ���C�����[�v */
        while (TRUE)
        {
            /* �L�[���͏��擾 */
            keyNow  =   PAD_Read();
            keyTrg  =   (u16)((keyOld ^ keyNow) & keyNow);
            keyOld  =   keyNow;
        
            /* ���͂ɉ����Ċe�푀�� */
            if (keyTrg & PAD_KEY_UP)
            {
                index   =   (index + INDEX_MAX - 1) % INDEX_MAX;
                UpdateScreen();
            }
            if (keyTrg & PAD_KEY_DOWN)
            {
                index   =   (index + 1) % INDEX_MAX;
                UpdateScreen();
            }
            
            /* �I���ʒu�ɉ����ĕ��� */
            if (index < 5)
            {
                if (keyTrg & PAD_KEY_LEFT)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_LEFT);
                }
                else if (keyTrg & PAD_KEY_RIGHT)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_RIGHT);
                }
                else if (keyTrg & PAD_BUTTON_A)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_A);
                }
                else if (keyTrg & PAD_BUTTON_B)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_B);
                }
                else if (keyTrg & PAD_BUTTON_X)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_X);
                }
                else if (keyTrg & PAD_BUTTON_Y)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_Y);
                }
                else if (keyTrg & PAD_BUTTON_L)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_L);
                }
                else if (keyTrg & PAD_BUTTON_R)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_R);
                }
                else if (keyTrg & PAD_BUTTON_START)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_START);
                }
                else if (keyTrg & PAD_BUTTON_SELECT)
                {
                    UpdateValue((u16*)((u32)&FilterParam + index * sizeof(u16)), BAND_FOR_IIR_SELECT);
                }
            }
            else
            {
                if (keyTrg & PAD_KEY_RIGHT)
                {
                    switch (index)
                    {
                    case 5: // Filter Target
                        if ( FilterTarget + 1 >= SNDEX_IIR_FILTER_TARGET_MAX )
                        {
                            FilterTarget = SNDEX_IIR_FILTER_DAC_LEFT_1;
                        }
                        else
                        {
                            FilterTarget++;
                        }
                        UpdateScreen();
                        break;
                    case 6: // Mute control
                        if (mute == SNDEX_MUTE_OFF)
                        {
                            if (CheckResult(SNDEX_SetMute(SNDEX_MUTE_ON)) == TRUE)
                            {
                                mute    =   SNDEX_MUTE_ON;
                                UpdateScreen();
                            }
                        }
                        break;
                    case 7: // Volume control
                        if (volume < SNDEX_VOLUME_MAX)
                        {
                            if (CheckResult(SNDEX_SetVolume((u8)(volume + 1))) == TRUE)
                            {
                                volume ++;
                                UpdateScreen();
                            }
                        }
                        break;
                    }
                }
                if (keyTrg & PAD_KEY_LEFT)
                {
                    switch (index)
                    {
                    case 5: // Filter Target
                        if ( FilterTarget == SNDEX_IIR_FILTER_DAC_LEFT_1 )
                        {
                            FilterTarget = (SNDEXIirFilterTarget)(SNDEX_IIR_FILTER_TARGET_MAX - 1);
                        }
                        else
                        {
                            FilterTarget--;
                        }
                        UpdateScreen();
                        break;
                    case 6: // Mute control
                        if (mute == SNDEX_MUTE_ON)
                        {
                            if (CheckResult(SNDEX_SetMute(SNDEX_MUTE_OFF)) == TRUE)
                            {
                                mute    =   SNDEX_MUTE_OFF;
                                UpdateScreen();
                            }
                        }
                        break;
                    case 7: // Volume control
                        if (volume > SNDEX_VOLUME_MIN)
                        {
                            if (CheckResult(SNDEX_SetVolume((u8)(volume - 1))) == TRUE)
                            {
                                volume --;
                                UpdateScreen();
                            }
                        }
                        break;
                    }
                }
                if (keyTrg & PAD_BUTTON_A)
                {
                    switch (index)
                    {
                    case 8:
                        if (!CheckResult(SNDEX_SetIirFilter((SNDEXIirFilterTarget)FilterTarget, &FilterParam)) == TRUE)
                        {
                                OS_Warning("SNDEX_SetIirFilter() failed.\n");
                        }
                        break;
                    }
                }
            }
        
            /* �T�E���h���C������ */
            while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
            {
            }
            (void)SND_FlushCommand(SND_COMMAND_NOBLOCK);
        
            /* V �u�����N�҂� */
            OS_WaitVBlankIntr();
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         UpdateValue

  Description:  IIR �t�B���^�ɐݒ肷��p�����[�^�l�̑���

  Arguments:    target - �l�𑝌�������Ώےl
                value  - �����l

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
static void UpdateValue(u16* target, s16 value)
{
    if ((s32)(*(target) + value) >= 0 && (u32)(*(target) + value) <= 0xffff)
    {
        *target = (u16)(*(target) + value);
        UpdateScreen();
    }
}

/*---------------------------------------------------------------------------*
  Name:         CheckResult

  Description:  SNDEX ���C�u������ API �Ăяo�����ʂ��m�F����B

  Arguments:    result  -   API ���Ԃ��ꂽ�߂�l���w�肷��B

  Returns:      BOOL    -   API �Ăяo�������������ꍇ�� TRUE ��Ԃ��B
                            ���炩�̗��R�Ŏ��s�����ꍇ�� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
CheckResult (SNDEXResult result)
{
    switch (result)
    {
    case SNDEX_RESULT_SUCCESS:
        return TRUE;
    case SNDEX_RESULT_BEFORE_INIT:
        /* ���C�u�����������O�B
            �e API ���g�p����O�ɁASNDEX_Init() �֐����Ăяo���K�v������܂��B */
        OS_TWarning("Not initialized.\n");
        break;
    case SNDEX_RESULT_INVALID_PARAM:
        /* �s���ȃp�����[�^�B
            API �ɗ^���Ă���������������ĉ������B */
        OS_TWarning("Invalid parameter.\n");
        break;
    case SNDEX_RESULT_EXCLUSIVE:
        /* �r�����䒆�B
            �񓯊��^ API ��A���ŌĂяo������A�����X���b�h����񓯊���
            SNDEX ���C�u�������g�p�����肵�Ȃ��ŉ������B */
        OS_TWarning("Overlapped requests.\n");
        break;
    case SNDEX_RESULT_ILLEGAL_STATE:
        /* ��Ԉُ�B
            �n�[�h�E�F�A�� TWL �łȂ��ꍇ�ASNDEX ���C�u�����͎g�p�ł��܂���B
            ��O�n���h�������瓯���^�̊֐��͌Ăяo���܂���B
            CODEC �� DS ���[�h�ɂ��Ă���ꍇ�́A�����̋����o�͏�Ԃɐؑւ����܂���B
            CODEC �� DS ���[�h�ɂ��Ă���ꍇ�A�����̋����o�͏�Ԃł���ꍇ�A
            �}�C�N�̎����T���v�����O���ł���ꍇ�́AI2S ���g����ύX�ł��܂���B */
        OS_TWarning("In unavailable state.\n");
        break;
    case SNDEX_RESULT_PXI_SEND_ERROR:
        /* PXI ���M�G���[�B
            ARM9 ���� ARM7 �ւ̃f�[�^���M�L���[���O�a���Ă��܂��BARM7 ���L���[����
            �f�[�^����������܂ł��΂炭�҂��Ă��烊�g���C���ĉ������B */
        OS_TWarning("PXI queue full.\n");
        break;
    case SNDEX_RESULT_DEVICE_ERROR:
        /* �f�o�C�X����Ɏ��s�B
            ���ʂ̎擾�A�ύX�� CPU �O���f�o�C�X (�}�C�R��) �ւ̃A�N�Z�X�𔺂��܂����A
            ���̃A�N�Z�X�Ɏ��s���Ă��܂��B���񃊃g���C���ďǏ󂪉��P���Ȃ��ꍇ�́A
            �}�C�R���̖\����n�[�h�E�F�A�̌̏�ȂǁA�\�t�g�I�ɂ͕����s�\�Ȍ�����
            ����\�����������߁A"����" �Ƃ݂Ȃ��ď�����i�߂ĉ������B */
        OS_TWarning("Micro controller unusual.\n");
        return TRUE;
    case SNDEX_RESULT_FATAL_ERROR:
    default:
        /* FATAL �G���[�B
            ���C�u�����̃��W�b�N��͔��������܂���B���C�u�����̓�����ԊǗ��𖳎�����
            ARM7 �ɒ��� PXI �R�}���h�𔭍s�����ꍇ��A�������j��ɂ���ē�����ԊǗ���
            �s�������������ꍇ�A�z��O�� ARM7 �R���|�[�l���g�Ƒg�ݍ��킹���ꍇ�Ȃǂ�
            ��������\��������܂��B */
        OS_TWarning("Fatal error.\n");
        break;
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         UpdateScreen

  Description:  ��ʕ\�����e���X�V����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
UpdateScreen (void)
{
    /* ���z�X�N���[�����N���A */
    MI_CpuClearFast((void*)screen, sizeof(screen));

    /* ���z�X�N���[����ɕ������ҏW */
    PrintString(2,  1, 0xf, "n0 : %d", FilterParam.n0);
    PrintString(2,  3, 0xf, "n1 : %d", FilterParam.n1);
    PrintString(2,  5, 0xf, "n2 : %d", FilterParam.n2);
    PrintString(2,  7, 0xf, "d1 : %d", FilterParam.d1);
    PrintString(2,  9, 0xf, "d2 : %d", FilterParam.d2);
    PrintString(2, 11, 0xf, "Target : %s", FilterTargetStrings[FilterTarget]);
    PrintString(2, 13 ,0xf, "Mute     : %d/1 [ %s ]", mute, ((mute == SNDEX_MUTE_OFF) ? "OFF" : "ON"));
    PrintString(2, 15, 0xf, "Volume   : %d/%d", volume, SNDEX_VOLUME_MAX);
    PrintString(2, 17, 0xf, "Set IIR Filter");
    
    PrintString(1, 19, 0x8, "LEFT RIGHT   +- 1");
    PrintString(1, 20, 0x8, "A B          +- 10");
    PrintString(1, 21, 0x8, "X Y          +- 100");
    PrintString(1, 22, 0x8, "L R          +- 1000");
    PrintString(1, 23, 0x8, "START SELECT +- 10000");

    /* �I�����ڂ̕\���F��ύX */
    PrintString(0, (s16)(index * 2 + 1), 0xf, ">");
    {
        s32     i;
        s32     j;
    
        for (i = 0; i < 32; i ++)
        {
            j   =   ((index * 2 + 1) * 32) + i;
            screen[j]   &=  (u16)(~(0xf << 12));
            screen[j]   |=  (u16)(0x4 << 12);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃x�N�g���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
VBlankIntr (void)
{
    /* ���z�X�N���[���� VRAM �ɔ��f */
    DC_FlushRange(screen, sizeof(screen));
    GX_LoadBG0Scr(screen, 0, sizeof(screen));

    /* IRQ �`�F�b�N�t���O���Z�b�g */
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
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
static void
PrintString (s16 x, s16 y, u8 palette, char *text, ...)
{
    va_list vlist;
    char    temp[32 + 2];
    s32     i;

    va_start(vlist, text);
    (void)vsnprintf(temp, 33, text, vlist);
    va_end(vlist);

    *((u16*)(&temp[32]))    =   0x0000;
    for (i = 0; ; i++)
    {
        if (temp[i] == 0x00)
        {
            break;
        }
        screen[((y * 32) + x + i) % (32 * 32)] = (u16)((palette << 12) | temp[i]);
    }
}
