/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos.TWL - snd - ignoreHWVolumeDemo
  File:     main.c

  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-02-09#$
  $Rev: 10003 $
  $Author: nishimoto_takashi $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    A sample that make a sound on setup time.

    USAGE:
        UP , DOWN    : Switch item to control.
        LEFT , RIGHT : Set config related to selected item.
        A            : Set SND Alarm.

 *---------------------------------------------------------------------------*/

#include    <twl.h>
#include    "font.h"
#include    "snd_data.h"

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
#define     INDEX_MAX   5

#define     TIMER_MIN   1
#define     TIMER_MAX  10

/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static u16      screen[32 * 32] ATTRIBUTE_ALIGN(HW_CACHE_LINE_SIZE);
static s32      index   =   0;

static SNDEXMute        mute     =   SNDEX_MUTE_OFF;
static u8               rate     =   0;
static u8               volume   =   0;
static u8               alarmVol =   0;
static u8               timer    =   5;

static BOOL             isSetAlarm           = FALSE;
static BOOL             isCalledAlarmHandler = FALSE;
static BOOL             isPlayingSound       = FALSE;

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static BOOL     CheckResult     (SNDEXResult result);
static void     UpdateScreen    (void);
static void     VBlankIntr      (void);
static void     PrintString     (s16 x, s16 y, u8 palette, char *text, ...);
static void     VolumeSwitchCallback(SNDEXResult result, void* arg);
static void     AlarmHandler    (void* arg);

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.

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

    /* �T�E���h�g���@�\������ */
    SNDEX_Init();
    (void)CheckResult(SNDEX_GetMute(&mute));
    (void)CheckResult(SNDEX_GetDSPMixRate(&rate));
    (void)CheckResult(SNDEX_GetVolume(&volume));
    UpdateScreen();
    
    /* ���ʒ����{�^���������̃R�[���o�b�N�֐���ݒ� */
    SNDEX_SetVolumeSwitchCallback(VolumeSwitchCallback, NULL);

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
            }
            if (keyTrg & PAD_KEY_DOWN)
            {
                index   =   (index + 1) % INDEX_MAX;
            }
            if (keyTrg & PAD_KEY_RIGHT)
            {
                switch (index)
                {
                case 0: // Mute control
                    if (mute == SNDEX_MUTE_OFF)
                    {
                        if (CheckResult(SNDEX_SetMute(SNDEX_MUTE_ON)) == TRUE)
                        {
                            mute    =   SNDEX_MUTE_ON;
                        }
                    }
                    break;
                case 1: // DSP mix rate control
                    if (rate < SNDEX_DSP_MIX_RATE_MAX)
                    {
                        if (CheckResult(SNDEX_SetDSPMixRate((u8)(rate + 1))) == TRUE)
                        {
                            rate ++;
                        }
                    }
                    break;
                case 2: // Volume control
                    if (volume < SNDEX_VOLUME_MAX)
                    {
                        if (CheckResult(SNDEX_SetVolume((u8)(volume + 1))) == TRUE)
                        {
                            volume ++;
                        }
                    }
                    break;
                case 3: // Alarm Volume control
                    if (alarmVol < SNDEX_VOLUME_MAX)
                    {
                        alarmVol++;
                    }
                    break;
                case 4: // Tiemr for Alarm
                    if (timer < TIMER_MAX)
                    {
                        timer++;
                    }
                    break;
                }
            }
            if (keyTrg & PAD_KEY_LEFT)
            {
                switch (index)
                {
                case 0: // Mute control
                    if (mute == SNDEX_MUTE_ON)
                    {
                        if (CheckResult(SNDEX_SetMute(SNDEX_MUTE_OFF)) == TRUE)
                        {
                            mute    =   SNDEX_MUTE_OFF;
                        }
                    }
                    break;
                case 1: // DSP mix rate control
                    if (rate > SNDEX_DSP_MIX_RATE_MIN)
                    {
                        if (CheckResult(SNDEX_SetDSPMixRate((u8)(rate - 1))) == TRUE)
                        {
                            rate --;
                        }
                    }
                    break;
                case 2: // Volume control
                    if (volume > SNDEX_VOLUME_MIN)
                    {
                        if (CheckResult(SNDEX_SetVolume((u8)(volume - 1))) == TRUE)
                        {
                            volume --;
                        }
                    }
                    break;
                case 3: // Alarm Volume control
                    if (alarmVol > SNDEX_VOLUME_MIN)
                    {
                        alarmVol--;
                    }
                    break;
                case 4: // Timer for Alarm
                    if (timer > TIMER_MIN)
                    {
                        timer--;
                    }
                    break;
                }
            }
            if (keyTrg & PAD_BUTTON_A)
            {
                /* ��莞�Ԍ�ɃT�E���h����悤�ɂ��� or �T�E���h�Đ����̓T�E���h�̒�~ */
                if (isPlayingSound)
                {
                    u32 tag;
                    // �T�E���h���~�i�����̏I����҂j
                    SND_StopSeq(0);
                    tag = SND_GetCurrentCommandTag();
                    (void)SND_FlushCommand(SND_COMMAND_BLOCK | SND_COMMAND_IMMEDIATE);
                    SND_WaitForCommandProc(tag);
                    
                    // �������̉������S�ɒ�~�����܂ő҂�
                    // SNDEX_ResetIgnoreHWVolume() ���s���̉��ʕύX���e�����Ă��܂�����
                    // �ȉ��Ŏw�肵�Ă���҂����Ԃ͕K�������œK�l�ł͂Ȃ�
                    OS_SpinWait(67 * 1000 * 700); //�� 700ms
                    
                    // ���ʃ��Z�b�g�E��������܂Ń��g���C
                    while (SNDEX_ResetIgnoreHWVolume() != SNDEX_RESULT_SUCCESS) {}
                    
                    OS_TPrintf("stop sequence.\n");
                    isPlayingSound = FALSE;
                }
                else if (!isSetAlarm)
                {
                    SND_SetupAlarm(0, (u32)OS_SecondsToTicks(timer), 0, AlarmHandler, NULL);
                    SND_StartTimer(0, 0, 1, 0);
                    isSetAlarm = TRUE;
                    OS_TPrintf("set alarm. (%d sec)\n", timer);
                }
            }
            
            if (isCalledAlarmHandler)
            {
                // ���ʕύX�E��������܂Ń��g���C����
                while (SNDEX_SetIgnoreHWVolume(alarmVol) != SNDEX_RESULT_SUCCESS) {}
                
                // ���ʐݒ��A�}���ȉ��ʕύX�ɂ��|�b�v�����y�����邽�߂�
                // ���ʂ��Ȃ��炩�Ȃɕω������Ă���A���̒x���͍ő�Ŗ� 32msec �����邱�Ƃ���E�G�C�g������
                OS_SpinWait(67 * 1000 * 32);  // �� 32ms

                SND_StartSeq(0, sound_seq_data, 0, (SNDBankData*)sound_bank_data);
                OS_TPrintf("start sequence.\n");
                isCalledAlarmHandler = FALSE;
                isPlayingSound = TRUE;
            }
        
            /* �T�E���h���C������ */
            while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
            {
            }
            (void)SND_FlushCommand(SND_COMMAND_NOBLOCK);
            
            /* �{�����[���̕ύX�� volume �l��Ǐ]������ */
            if (!isCalledAlarmHandler && !isPlayingSound)
            {
                (void)CheckResult(SNDEX_GetVolume(&volume));
            }
            
            /* ��ʍX�V */
            UpdateScreen();

            /* V �u�����N�҂� */
            OS_WaitVBlankIntr();
        }
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
    PrintString(2,  1, 0xf, "Mute         : %d/1 [ %s ]", mute, ((mute == SNDEX_MUTE_OFF) ? "OFF" : "ON"));
    PrintString(2,  3, 0xf, "DSP mix      : %d/8", rate);
    PrintString(2,  5, 0xf, "Volume       : %d/%d", volume, (u8)SNDEX_VOLUME_MAX);

    PrintString(2,  7, 0xf, "Alarm Volume : %d/%d", alarmVol, (u8)SNDEX_VOLUME_MAX);
    PrintString(2,  9, 0xf, "Timer        : %d/%d sec", timer, (u8)TIMER_MAX);
    
    PrintString(0, 13, 0xe, "- [A]        : set / stop Alarm");
    PrintString(0, 15, 0xe, "- [Volume Button] : print log");
    
    if (isSetAlarm)
    {
        PrintString(2, 19, 0x1, "set Alarm...");
    }
    else if (isPlayingSound)
    {
        PrintString(2, 19, 0x1,  "playing sound...");
        PrintString(10, 20, 0x1, "press A to stop.");
    }

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

/*---------------------------------------------------------------------------*
  Name:         VolumeSwitchCallback

  Description:  TWL�{�̂̉��ʒ����{�^���������Ɏ��s�����R�[���o�b�N�֐�

  Arguments:    result  -   ���ʒ����{�^����������ɂ�錋�ʁB
                arg     -   ���̊֐����R�[���o�b�N�֐��Ƃ��ČĂ΂ꂽ�ۂɓn���������B
                

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
static void
VolumeSwitchCallback (SNDEXResult result, void* arg)
{
#pragma unused( arg )
#pragma unused( result )

    /* �R�[���o�b�N�������ASNDEX ���C�u�����̊֐��͎g�p�ł��܂���B*
     * SNDEX_RESULT_EXCLUSIVE ���Ԃ�܂��B                         */
    OS_TPrintf("volume switch pressed.\n");
}

static void
AlarmHandler (void* arg)
{
#pragma unused( arg )
    
    OS_TPrintf("alarm handler called.\n");
    isSetAlarm  = FALSE;
    isCalledAlarmHandler = TRUE;
}
