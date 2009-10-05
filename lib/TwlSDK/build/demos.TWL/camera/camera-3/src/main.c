/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos.TWL - camera - camera-3
  File:     main.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

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

#define NDMA_NO      1           // �g�p����NDMA�ԍ�(0-3)
#define WIDTH       256         // �C���[�W�̕�
#define HEIGHT      192         // �C���[�W�̍���

#define LINES_AT_ONCE   CAMERA_GET_MAX_LINES(WIDTH)     // ���̓]�����C����
#define BYTES_PER_LINE  CAMERA_GET_LINE_BYTES(WIDTH)    // �ꃉ�C���̓]���o�C�g��

static void VBlankIntr(void);
static BOOL CameraInit(void);
static void CameraIntrVsync(CAMERAResult result);
static void CameraIntrError(CAMERAResult result);
static void CameraIntrReboot(CAMERAResult result);
static void CameraDmaRecvIntr(void* arg);

static u32 stabilizedCount;

static BOOL switchFlag;

/////
//  �`������\����FPS���傫���O��ł̃g���v���o�b�t�@�V�[�P���X
/////

static int wp;  // �J��������f�[�^����荞�ݒ��̃o�b�t�@
static int rp;  // �\�����̃o�b�t�@
static BOOL wp_pending; // ��荞�݂𒆒f���� (�Ăѓ����o�b�t�@�Ɏ�荞��)
static GXBGBmpScrBase dispSrc[] =
{
    GX_BG_BMPSCRBASE_0x20000,
    GX_BG_BMPSCRBASE_0x40000,
    GX_BG_BMPSCRBASE_0x60000,
};
static void* dispAddr[] =
{
    (void*)(HW_BG_VRAM + 0x20000),
    (void*)(HW_BG_VRAM + 0x40000),
    (void*)(HW_BG_VRAM + 0x60000),
};
static CAMERASelect current;
static void DebugReport(void)
{
    const char* const str[] =
    {
        "HW_BG_VRAM + 0x20000",
        "HW_BG_VRAM + 0x40000",
        "HW_BG_VRAM + 0x60000"
    };
    OSIntrMode enabled = OS_DisableInterrupts();
    int wp_bak = wp;
    int rp_bak = rp;
    (void)OS_RestoreInterrupts(enabled);
    OS_TPrintf("\nCapture to %s\tDisplay from %s\n", str[wp_bak], str[rp_bak]);
}
static GXBGBmpScrBase GetNextDispSrc(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    GXBGBmpScrBase base;
    int next = (rp + 1) % 3;
    if (next != wp) // ���o�b�t�@���������ݒ��łȂ�
    {
        rp = next;
    }
    base = dispSrc[rp];
    (void)OS_RestoreInterrupts(enabled);
    //DebugReport();
    return base;
}
static void* GetNextCaptureAddr(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    void* addr;
    int next = (wp + 1) % 3;
    if (wp_pending)         // �����o�b�t�@�Ɏ�荞�݂�����
    {
        wp_pending = FALSE;
    }
    else if (next != rp)    // ���o�b�t�@���ǂݍ��ݒ��łȂ�
    {
        // �J���������肷��܂ŁA�L���v�`�����ʂ͉�ʂɕ\�����Ȃ�
        // ���̃f���łُ͈�ȐF��������邽�߂ɍŒ���҂K�v�̂��� 4 �J�����t���[���҂��Ă��邪�A
        // �����I�o�����肷��܂ő҂������ꍇ�́A���t�@�����X�ɂ���悤�ɉ����� 14�A���O�� 30 �t���[���҂K�v������܂��B
        if(stabilizedCount > 4)
        {
            wp = next;
        }
    }
    else    // ��荞�ނׂ��o�b�t�@���Ȃ� (fps����݂Ă��蓾�Ȃ�)
    {
        OS_TPrintf("ERROR: there is no unused frame to capture.\n");
    }
    addr = dispAddr[wp];
    (void)OS_RestoreInterrupts(enabled);
    //DebugReport();
    return addr;
}
static void PendingCapture(void)
{
    wp_pending = TRUE;
}

// �����\�� (1�s����)
static u16 text_buffer[ 32 ] ATTRIBUTE_ALIGN(32);
static void PutString( char *format, ... )
{
    u16             *dest = text_buffer;
    char            temp[32+1];
    int             i;
    va_list         va;

    va_start(va, format);
    (void)OS_VSNPrintf(temp, sizeof(temp), format, va);
    va_end(va);

    MI_CpuClearFast(text_buffer, sizeof(text_buffer));
    for (i = 0; i < 32 && temp[i]; i++)
    {
        dest[i] = (u16)((u8)temp[i] | (1 << 12));
    }
    DC_StoreRange(text_buffer, sizeof(text_buffer));
}

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void TwlMain()
{
    CAMERAResult result;

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
    GX_SetBankForLCDC(GX_VRAM_LCDC_C);
    GX_SetBankForLCDC(GX_VRAM_LCDC_D);
    MI_CpuClearFast((void*)HW_LCDC_VRAM_A, 128 * 1024);
    MI_CpuClearFast((void*)HW_LCDC_VRAM_B, 128 * 1024);
    MI_CpuClearFast((void*)HW_LCDC_VRAM_C, 128 * 1024);
    MI_CpuClearFast((void*)HW_LCDC_VRAM_D, 128 * 1024);

    // �_�C���N�g�r�b�g�}�b�v�\�����[�h�������\��
    GX_SetBankForBG(GX_VRAM_BG_512_ABCD);         // VRAM-A,B,C,D�o���N���a�f�Ɋ��蓖��
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_4, GX_BG0_AS_2D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG1 | GX_PLANEMASK_BG3);

    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                     GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
    G2_SetBG1Priority(1);
    G2_BG1Mosaic(FALSE);

    wp = 0;
    rp = 2;
    wp_pending = TRUE;
    stabilizedCount = 0;
    switchFlag = FALSE;
    G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GetNextDispSrc());
    G2_SetBG3Priority(3);
    G2_BG3Mosaic(FALSE);

    // �����̓ǂݍ���
    {
        static const GXRgb pal[16] = { GX_RGB(0, 0, 0), GX_RGB(31, 31, 31), };
        GX_LoadBG1Char(DEMOAsciiChr, 0x00000, sizeof(DEMOAsciiChr));
        GX_LoadBGPltt(pal, 0x0000, sizeof(pal));
    }

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

    // DMA���荞�ݐݒ�
    (void)OS_EnableIrqMask(OS_IE_NDMA1);

    // �J����VSYNC���荞�݃R�[���o�b�N
    CAMERA_SetVsyncCallback(CameraIntrVsync);

    // �J�����G���[���荞�݃R�[���o�b�N
    CAMERA_SetBufferErrorCallback(CameraIntrError);

    // �J�����̍ċN�������R�[���o�b�N
    CAMERA_SetRebootCallback(CameraIntrReboot);

    CAMERA_SetOutputFormat(CAMERA_OUTPUT_RGB);
    CAMERA_SetTransferLines(CAMERA_GET_MAX_LINES(WIDTH));

    // �L���v�`���X�^�[�g
    CAMERA_DmaRecvAsync(NDMA_NO, GetNextCaptureAddr(), CAMERA_GetBytesAtOnce(WIDTH), CAMERA_GET_FRAME_BYTES(WIDTH, HEIGHT), CameraDmaRecvIntr, NULL);
    CAMERA_ClearBuffer();
    CAMERA_StartCapture();
    OS_TPrintf("Camera is shooting a movie...\n");

    while (1)
    {
        u16 pad;
        u16 trg;
        static u16 old = 0xffff;
        static BOOL standby = FALSE;

        OS_WaitVBlankIntr();

        pad = PAD_Read();
        trg = (u16)(pad & ~old);
        old = pad;

        if (trg & PAD_BUTTON_A)
        {
            DebugReport();
        }
        if (trg & PAD_BUTTON_X)
        {
            switchFlag = TRUE;
        }
        if (trg & PAD_BUTTON_B)
        {
            standby ^= 1;
            if (standby)
            {
                // StopCapture �����O�� Activate �� NONE �ɂ���ƁA���� Activate ON �ɂ���܂� IsBusy �� TRUE �ƂȂ鋰�ꂪ����
                CAMERA_StopCapture();
                while(CAMERA_IsBusy())
                {
                }
                MI_StopNDma(NDMA_NO);
                result = CAMERA_I2CActivate(CAMERA_SELECT_NONE);
                if(result == CAMERA_RESULT_FATAL_ERROR)
                    OS_Panic("CAMERA FATAL ERROR\n");
            }
            else
            {
                result = CAMERA_I2CActivate(current);
                if(result == CAMERA_RESULT_FATAL_ERROR)
                    OS_Panic("CAMERA FATAL ERROR\n");
                stabilizedCount = 0;
                CAMERA_DmaRecvAsync(NDMA_NO, GetNextCaptureAddr(), CAMERA_GetBytesAtOnce(WIDTH), CAMERA_GET_FRAME_BYTES(WIDTH, HEIGHT), CameraDmaRecvIntr, NULL);
                CAMERA_ClearBuffer();
                CAMERA_StartCapture();
            }
            OS_TPrintf("%s\n", result == CAMERA_RESULT_SUCCESS ? "SUCCESS" : "FAILED");
        }
    }
}

//--------------------------------------------------------------------------------
//    �u�u�����N���荞�ݏ���
//
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
    G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GetNextDispSrc());
    GX_LoadBG1Scr(text_buffer, 0, sizeof(text_buffer));
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
//    �J�������荞�ݏ��� (�G���[����Vsync���̗����Ŕ���)
//
#define FPS_SAMPLES 4
void CameraIntrError(CAMERAResult result)
{
#pragma unused(result)
    OS_TPrintf("Error was occurred.\n");
    // �J������~����
    CAMERA_StopCapture();
    MI_StopNDma(NDMA_NO);
    CAMERA_ClearBuffer();
    PendingCapture();       // ����������t���[�����g�p����
    CAMERA_DmaRecvAsync(NDMA_NO, GetNextCaptureAddr(), CAMERA_GetBytesAtOnce(WIDTH), CAMERA_GET_FRAME_BYTES(WIDTH, HEIGHT), CameraDmaRecvIntr, NULL);
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

void CameraIntrVsync(CAMERAResult result)
{
#pragma unused(result)
    if(stabilizedCount <= 30)
        stabilizedCount++;
    if(switchFlag)
    {
        current = (current == CAMERA_SELECT_IN ? CAMERA_SELECT_OUT : CAMERA_SELECT_IN);
        OS_TPrintf("call CAMERA_I2CActivate(%s)... ", (current == CAMERA_SELECT_IN ? "CAMERA_SELECT_IN" : "CAMERA_SELECT_OUT"));
        result = CAMERA_I2CActivate(current);
        if(result == CAMERA_RESULT_FATAL_ERROR)
            OS_Panic("CAMERA FATAL ERROR\n");
        OS_TPrintf("%s\n", result == CAMERA_RESULT_SUCCESS ? "SUCCESS" : "FAILED");
        stabilizedCount = 0;
        switchFlag = FALSE;
    }
}

void CameraDmaRecvIntr(void* arg)
{
#pragma unused(arg)
    MI_StopNDma(NDMA_NO);

    if (CAMERA_IsBusy() == TRUE)
    {
        if (MI_IsNDmaBusy(NDMA_NO)) // �摜�̓]�����I����Ă��邩�`�F�b�N
        {
            OS_TPrintf("DMA was not done until VBlank.\n");
            MI_StopNDma(NDMA_NO);
        }
        // ���̃t���[���̃L���v�`�����J�n����
        CAMERA_DmaRecvAsync(NDMA_NO, GetNextCaptureAddr(), CAMERA_GetBytesAtOnce(WIDTH), CAMERA_GET_FRAME_BYTES(WIDTH, HEIGHT), CameraDmaRecvIntr, NULL);
    }

    // �t���[�����[�g�̕\��
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
    }
}
