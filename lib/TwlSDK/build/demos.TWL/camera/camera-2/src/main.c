/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos.TWL - camera - camera-2
  File:     main.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-15#$
  $Rev: 10750 $
  $Author: kitase_hirotake $
 *---------------------------------------------------------------------------*/

#include <twl.h>
#include <twl/camera.h>
#include "DEMOBitmap.h"

#define NDMA_NO  1           // �g�p����NDMA�ԍ�(0-3)
#define NDMA_IE  OS_IE_NDMA1  // �Ή����銄�荞��
#define WIDTH   256         // �C���[�W�̕�
#define HEIGHT  192         // �C���[�W�̍���

#define LINES_AT_ONCE   CAMERA_GET_MAX_LINES(WIDTH)     // ���̓]�����C����
#define BYTES_PER_LINE  CAMERA_GET_LINE_BYTES(WIDTH)    // �ꃉ�C���̓]���o�C�g��

static void VBlankIntr(void);
static BOOL CameraInit(void);
static void CameraDmaIntr(void);
static void CameraIntrVsync(CAMERAResult result);
static void CameraIntrError(CAMERAResult result);
static void CameraIntrReboot(CAMERAResult result);

static int lineNumber = 0;
static BOOL effectFlag = FALSE;
static BOOL switchFlag = FALSE;
static BOOL standbyFlag = FALSE;
static u16 pipeBuffer[BYTES_PER_LINE * LINES_AT_ONCE / sizeof(u16)] ATTRIBUTE_ALIGN(32);
static CAMERASelect current;


static int wp;  // �J��������f�[�^����荞�ݒ��̃o�b�t�@
static int rp;  // �Ō��VRAM�ɃR�s�[�����o�b�t�@
static BOOL wp_pending; // ��荞�݂𒆒f���� (�Ăѓ����o�b�t�@�Ɏ�荞��)
static u32 stabilizedCount;
static u16 buffer[2][WIDTH*HEIGHT] ATTRIBUTE_ALIGN(32);
static void PendingCapture(void)
{
    wp_pending = TRUE;
}

// �����\��
static void PutString( char *format, ... )
{
    u16             *dest = G2_GetBG1ScrPtr();
    char            temp[32+1];
    int             i;
    va_list         va;

    va_start(va, format);
    (void)OS_VSNPrintf(temp, sizeof(temp), format, va);
    va_end(va);

    for (i = 0; i < 32 && temp[i]; i++)
    {
        dest[i] = (u16)((u8)temp[i] | (1 << 12));
    }
}

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void TwlMain()
{
    // ������
    OS_Init();
    OS_InitThread();
    GX_Init();
    OS_InitTick();
    OS_InitAlarm();

    // GX��DMA���g��Ȃ� (��DMA�̓J����DMA�̎ז��ɂȂ�)
    (void)GX_SetDefaultDMA(GX_DMA_NOT_USE);

    // VRAM�N���A
    GX_SetBankForLCDC(GX_VRAM_LCDC_A);
    GX_SetBankForLCDC(GX_VRAM_LCDC_B);
    MI_CpuClearFast((void*)HW_LCDC_VRAM_A, 128 * 1024);
    MI_CpuClearFast((void*)HW_LCDC_VRAM_B, 128 * 1024);

    // �_�C���N�g�r�b�g�}�b�v�\�����[�h�������\��
    GX_SetBankForBG(GX_VRAM_BG_256_AB);         // VRAM-A,B�o���N���a�f�Ɋ��蓖��
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_4, GX_BG0_AS_2D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG1 | GX_PLANEMASK_BG3);

    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                     GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
    G2_SetBG1Priority(1);
    G2_BG1Mosaic(FALSE);

    G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x20000);
    G2_SetBG3Priority(3);
    G2_BG3Mosaic(FALSE);

    // �����̓ǂݍ���
    {
        static const GXRgb pal[16] = { GX_RGB(0, 0, 0), GX_RGB(31, 31, 31), };
        GX_LoadBG1Char(DEMOAsciiChr, 0x00000, sizeof(DEMOAsciiChr));
        GX_LoadBGPltt(pal, 0x0000, sizeof(pal));
    }
    wp = 0;
    rp = 1;
    wp_pending = TRUE;
    stabilizedCount = 0;

    // V�u�����N���荞�ݐݒ�
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableInterrupts();

    OS_WaitVBlankIntr();
    GX_DispOn();

    // �J����������
    current = CAMERA_SELECT_IN;
    (void)CameraInit();

    CAMERA_SetOutputFormat(CAMERA_OUTPUT_RGB);
    CAMERA_SetTransferLines(CAMERA_GET_MAX_LINES(WIDTH));

    // DMA���荞�ݐݒ�
    OS_SetIrqFunction(NDMA_IE, CameraDmaIntr);
    (void)OS_EnableIrqMask(NDMA_IE);

    // �J����VSYNC���荞�݃R�[���o�b�N
    CAMERA_SetVsyncCallback(CameraIntrVsync);

    // �J�����G���[���荞�݃R�[���o�b�N
    CAMERA_SetBufferErrorCallback(CameraIntrError);

    // �J�����̍ċN�������R�[���o�b�N
    CAMERA_SetRebootCallback(CameraIntrReboot);

    // DMA�X�^�[�g (�����I�Ɏ~�߂Ȃ�����i���ɗL��)
    MI_StopNDma(NDMA_NO);
    CAMERA_DmaPipeInfinity(NDMA_NO, pipeBuffer, CAMERA_GetBytesAtOnce(WIDTH), NULL, NULL);

    // �J�����X�^�[�g
    lineNumber = 0;
    CAMERA_ClearBuffer();
    CAMERA_StartCapture();
    OS_TPrintf("Camera is shooting a movie...\n");

    while (1)
    {
        u16 pad;
        u16 trg;
        static u16 old = 0xffff;

        OS_WaitVBlankIntr();

        if (wp == rp)
        {
            rp ^= 1;
            GX_LoadBG3Scr(buffer[rp], 0, BYTES_PER_LINE * HEIGHT);
            DC_InvalidateRange(buffer[rp], BYTES_PER_LINE * HEIGHT);
        }

        pad = PAD_Read();
        trg = (u16)(pad & ~old);
        old = pad;

        if (trg & PAD_BUTTON_A)
        {
            effectFlag ^= 1;
            OS_TPrintf("Effect %s\n", effectFlag ? "ON" : "OFF");
        }
        if (trg & PAD_BUTTON_X)
        {
            current = (current == CAMERA_SELECT_IN ? CAMERA_SELECT_OUT : CAMERA_SELECT_IN);
            switchFlag = TRUE;
        }
        if (trg & PAD_BUTTON_B)
        {
            standbyFlag ^= 1;

            if(standbyFlag)
            {
                switchFlag = TRUE;
            }
            else
            {
                CAMERAResult result;

                OS_TPrintf("call CAMERA_I2CActivate(%s)\n", (current == CAMERA_SELECT_IN ? "CAMERA_SELECT_IN" : "CAMERA_SELECT_OUT"));
                result = CAMERA_I2CActivateAsync(current, NULL, NULL);
                if(result == CAMERA_RESULT_FATAL_ERROR)
                    OS_Panic("CAMERA FATAL ERROR\n");
                stabilizedCount = 0;
            }
        }
    }
}

//--------------------------------------------------------------------------------
//    �u�u�����N���荞�ݏ���
//
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking VBlank interrupt
}

//--------------------------------------------------------------------------------
//    �J�����̏��������� (Init��I2C�֘A�̂�)
//
BOOL CameraInit(void)
{
    CAMERAResult result;
    result = CAMERA_Init();
    if(result == CAMERA_RESULT_FATAL_ERROR)
        OS_TPanic("CAMERA_Init was failed.");
    if(result == CAMERA_RESULT_ILLEGAL_STATUS)
        return FALSE;

    result = CAMERA_I2CActivate(current);
    if (result == CAMERA_RESULT_FATAL_ERROR)
        OS_TPanic("CAMERA_I2CActivate was failed. (%d)\n", result);
    if(result == CAMERA_RESULT_ILLEGAL_STATUS)
        return FALSE;
    stabilizedCount = 0;

    return TRUE;
}

//--------------------------------------------------------------------------------
//    DMA���荞�ݏ���
//
#define FPS_SAMPLES 4
void CameraDmaIntr(void)
{
    static BOOL effect = FALSE;
    CAMERAResult result;

    OS_SetIrqCheckFlag(NDMA_IE);
    OS_SetIrqFunction(NDMA_IE, CameraDmaIntr);

    // �K�v�ȏ��������ăt���[���o�b�t�@�ɃR�s�[����
    if (effect) // �l�K�|�W���]����
    {
        int i;
        u32 *src = (u32*)pipeBuffer;
        u32 *dest = (u32*)buffer[wp] + lineNumber * WIDTH / 2;
        for (i = 0; i < WIDTH * LINES_AT_ONCE / 2; i++)
        {
            dest[i] = ~src[i] | 0x80008000;
        }
    }
    else
    {
        MI_CpuCopyFast(pipeBuffer, (u16*)buffer[wp] + lineNumber * WIDTH, sizeof(pipeBuffer));
    }
    DC_InvalidateRange(pipeBuffer, sizeof(pipeBuffer));

    lineNumber += LINES_AT_ONCE;
    if (lineNumber >= HEIGHT)
    {
        static OSTick begin = 0;
        static int uspf[FPS_SAMPLES] = { 0 };
        static int count = 0;
        int i;
        int sum = 0;
        OSTick end = OS_GetTick();
        if (begin)  // ���������
        {
            uspf[count] = (int)OS_TicksToMicroSeconds(end - begin);
            count = (count + 1) % FPS_SAMPLES;
        }
        begin = end;
        // ���ϒl�̎Z��
        for (i = 0; i < FPS_SAMPLES; i++)
        {
            if (uspf[i] == 0)  break;
            sum +=  uspf[i];
        }
        if (sum)
        {
            int mfps = (int)(1000000000LL * i / sum);
            PutString("%2d.%03d fps", mfps / 1000, mfps % 1000);
        }

        if (switchFlag)
        {
            if (standbyFlag)
            {
                OS_TPrintf("call CAMERA_I2CActivate(CAMERA_SELECT_NONE)\n");
                result = CAMERA_I2CActivate(CAMERA_SELECT_NONE);
                if(result == CAMERA_RESULT_FATAL_ERROR)
                    OS_Panic("CAMERA FATAL ERROR\n");
            }
            else
            {
                OS_TPrintf("call CAMERA_I2CActivate(%s)\n", (current == CAMERA_SELECT_IN ? "CAMERA_SELECT_IN" : "CAMERA_SELECT_OUT"));
                result = CAMERA_I2CActivateAsync(current, NULL, NULL);
                if(result == CAMERA_RESULT_FATAL_ERROR)
                    OS_Panic("CAMERA FATAL ERROR\n");
                stabilizedCount = 0;
            }
            switchFlag = FALSE;
        }
        effect = effectFlag;

        // �o�b�t�@�̐؂�ւ�
        if (wp_pending)
        {
            wp_pending = FALSE;
        }
        else
        {
            // �J���������肷��܂ŁA�L���v�`�����ʂ͉�ʂɕ\�����Ȃ�
            // ���̃f���łُ͈�ȐF��������邽�߂ɍŒ���҂K�v�̂��� 4 �J�����t���[���҂��Ă��邪�A
            // �����I�o�����肷��܂ő҂������ꍇ�́A���t�@�����X�ɂ���悤�ɉ����� 14�A���O�� 30 �t���[���҂K�v������܂��B
            if(stabilizedCount > 4)
            {
                wp ^= 1;
            }
        }

        lineNumber = 0;
    }
}

void CameraIntrVsync(CAMERAResult result)
{
#pragma unused(result)
    if(stabilizedCount <= 30)
        stabilizedCount++;
}

//--------------------------------------------------------------------------------
//    �J�������荞�ݏ��� (�G���[����Vsync���ɔ���)
//
void CameraIntrError(CAMERAResult result)
{
#pragma unused(result)
    OS_TPrintf("Error was occurred.\n");

    // �J������~
    CAMERA_StopCapture();
    lineNumber = 0;
    wp_pending = TRUE;      // ����������t���[�����g�p����
    // �J�����ĊJ
    CAMERA_ClearBuffer();
    CAMERA_StartCapture();
}

void CameraIntrReboot(CAMERAResult result)
{
    if(result == CAMERA_RESULT_FATAL_ERROR)
    {
        return; // �J�������ċN���������s���Ă����A���邱�Ƃ��ł��Ȃ�����
    }
    CameraIntrError(result); // DMA�̓���������Ă��邩������Ȃ��̂Ŏd�؂蒼��
}
