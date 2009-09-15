/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CAMERA - demos - simpleShoot-1
  File:     main.c

  Copyright 2003-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <twl.h>
#include "DEMO.h"
#include <twl/camera.h>
#include <twl/dsp.h>
#include <twl/dsp/common/g711.h>

#include    "snd_data.h"

#define NDMA_NO      1           // �g�p����NDMA�ԍ�(0-3)
#define WIDTH       256         // �C���[�W�̕�
#define HEIGHT      192         // �C���[�W�̍���

#define LINES_AT_ONCE   CAMERA_GET_MAX_LINES(WIDTH)     // ���̓]�����C����
#define BYTES_PER_LINE  CAMERA_GET_LINE_BYTES(WIDTH)    // �ꃉ�C���̓]���o�C�g��

void VBlankIntr(void);
static BOOL CameraInit(void);
static void CameraIntrVsync(CAMERAResult result);
static void CameraIntrError(CAMERAResult result);
static void CameraIntrReboot(CAMERAResult result);
static void CameraDmaRecvIntr(void* arg);

static int wp;  // �J��������f�[�^����荞�ݒ��̃o�b�t�@
static int rp;  // �Ō��VRAM�ɃR�s�[�����o�b�t�@
static BOOL wp_pending; // ��荞�݂𒆒f���� (�Ăѓ����o�b�t�@�Ɏ�荞��)
static u32 stabilizedCount;
static u16 buffer[2][WIDTH*HEIGHT] ATTRIBUTE_ALIGN(32);

static BOOL switchFlag;

#define STRM_BUF_SIZE 1024*64

static char StrmBuf[STRM_BUF_SIZE] ATTRIBUTE_ALIGN(32);
static u32 len;

// �t�@�C����
static const char filename1[] = "shutter_sound_32730.wav";

static void DebugReport(void)
{
    OS_TPrintf("\nCapture to No.%d\tDisplay from No.%d\n", wp, rp);
}
static void PendingCapture(void)
{
    wp_pending = TRUE;
}

// �B�e�����摜���i�[����o�b�t�@
static u16 shoot_buffer[WIDTH*HEIGHT] ATTRIBUTE_ALIGN(32);
static int shoot_flag = 0;

static CAMERASelect current_camera = CAMERA_SELECT_IN;

//--------------------------------------------------------------------------------
//    �u�u�����N���荞�ݏ���
//
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
    if (wp == rp)
    {
        rp ^= 1;
        MI_CpuCopyFast(buffer[rp], (void*)HW_LCDC_VRAM_A, BYTES_PER_LINE * HEIGHT);
        DC_InvalidateRange(buffer[rp], BYTES_PER_LINE * HEIGHT);
    }
    if(shoot_flag == 1)
    {
        MI_CpuCopyFast(buffer[rp], (void*)shoot_buffer, BYTES_PER_LINE * HEIGHT);
        shoot_flag = 2;
    }
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

    result = CAMERA_I2CEffect(current_camera, CAMERA_EFFECT_NONE);
    if (result != CAMERA_RESULT_SUCCESS)
    {
        OS_TPrintf("CAMERA_I2CEffect was failed. (%d)\n", result);
    }

    result = CAMERA_I2CActivate(current_camera);
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
#define PRINT_RATE  32
void CameraIntrError(CAMERAResult result)
{
#pragma unused(result)
    OS_TPrintf("Error was occurred.\n");
    // �J������~����
    CAMERA_StopCapture();
    MI_StopNDma(NDMA_NO);
    CAMERA_ClearBuffer();
    wp_pending = TRUE;      // ����������t���[�����g�p����
    CAMERA_DmaRecvAsync(NDMA_NO, buffer[wp], CAMERA_GetBytesAtOnce(WIDTH), CAMERA_GET_FRAME_BYTES(WIDTH, HEIGHT), CameraDmaRecvIntr, NULL);
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
        current_camera = (current_camera == CAMERA_SELECT_IN ? CAMERA_SELECT_OUT : CAMERA_SELECT_IN);
        result = CAMERA_I2CActivate(current_camera);
        if(result == CAMERA_RESULT_FATAL_ERROR)
            OS_Panic("CAMERA FATAL ERROR\n");
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
        OS_TPrintf(".");
        if (MI_IsNDmaBusy(NDMA_NO)) // �摜�̓]�����I����Ă��邩�`�F�b�N
        {
            OS_TPrintf("DMA was not done until VBlank.\n");
            MI_StopNDma(NDMA_NO);
        }
        // ���̃t���[���̃L���v�`�����J�n����
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
        CAMERA_DmaRecvAsync(NDMA_NO, buffer[wp], CAMERA_GetBytesAtOnce(WIDTH), CAMERA_GET_FRAME_BYTES(WIDTH, HEIGHT), CameraDmaRecvIntr, NULL);
    }

    {
        static OSTick begin = 0;
        static int count = 0;
        if (begin == 0)
        {
            begin = OS_GetTick();
        }
        else if (++count == PRINT_RATE)
        {
            OSTick uspf = OS_TicksToMicroSeconds(OS_GetTick() - begin) / count;
            int mfps = (int)(1000000000LL / uspf);
            OS_TPrintf("%2d.%03d fps\n", mfps / 1000, mfps % 1000);
            count = 0;
            begin = OS_GetTick();
        }
    }
}

void TwlMain(void)
{
    int     vram_slot = 0, count = 0;
    CAMERAResult result;

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, and initialize VRAM.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();

    // GX��DMA���g��Ȃ� (��DMA�̓J����DMA�̎ז��ɂȂ�)
    (void)GX_SetDefaultDMA(GX_DMA_NOT_USE);

    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);

    GXS_SetGraphicsMode(GX_BGMODE_3);

    GXS_SetVisiblePlane(GX_PLANEMASK_BG3);
    G2S_SetBG3Priority(0);
    G2S_BG3Mosaic(FALSE);

    G2S_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);

    MI_CpuClearFast(shoot_buffer, WIDTH*HEIGHT*sizeof(u16));
    GXS_LoadBG3Bmp(shoot_buffer, 0, WIDTH*HEIGHT*sizeof(u16));

    GXS_DispOn();

    // ������
    OS_InitThread();
    OS_InitTick();
    OS_InitAlarm();
    MI_InitNDmaConfig();
    FS_Init( FS_DMA_NOT_USE );

    // V�u�����N���荞�ݐݒ�
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableInterrupts();

    // VRAM�\�����[�h
    GX_SetBankForLCDC(GX_VRAM_LCDC_A);
    MI_CpuClearFast((void*)HW_LCDC_VRAM_A, BYTES_PER_LINE * HEIGHT);
    wp = 0;
    rp = 1;
    wp_pending = TRUE;
    stabilizedCount = 0;
    switchFlag = FALSE;
    GX_SetGraphicsMode(GX_DISPMODE_VRAM_A, GX_BGMODE_0, GX_BG0_AS_2D);
    OS_WaitVBlankIntr();
    GX_DispOn();

    // �J����������
    (void)CameraInit();

    // DMA���荞�ݐݒ�
    (void)OS_EnableIrqMask(OS_IE_NDMA1);

    // �J����VSYNC���荞�݃R�[���o�b�N
    CAMERA_SetVsyncCallback(CameraIntrVsync);

    // �J�����G���[���荞�݃R�[���o�b�N
    CAMERA_SetBufferErrorCallback(CameraIntrError);

    // �J�����̍ċN�������R�[���o�b�N
    CAMERA_SetRebootCallback(CameraIntrReboot);

    CAMERA_SetTrimmingParams(0, 0, WIDTH, HEIGHT);
    CAMERA_SetTrimming(TRUE);
    CAMERA_SetOutputFormat(CAMERA_OUTPUT_RGB);
    CAMERA_SetTransferLines(CAMERA_GET_MAX_LINES(WIDTH));

    // �L���v�`���X�^�[�g
    CAMERA_DmaRecvAsync(NDMA_NO, buffer[wp], CAMERA_GetBytesAtOnce(WIDTH), CAMERA_GET_FRAME_BYTES(WIDTH, HEIGHT), CameraDmaRecvIntr, NULL);
    CAMERA_ClearBuffer();
    CAMERA_StartCapture(); // �J����VSYNC���ɌĂяo���Ă���̂ŁA������ CAMERA_IsBusy() �� TRUE ��Ԃ��悤�ɂȂ�
    OS_TPrintf("Camera is shooting a movie...\n");

    DEMOStartDisplay();

    // DSP�V���b�^�[���̏�����
    {
        FSFile  file[1];
        int     slotB;
        int     slotC;
        // ���̃T���v���ł͑S�Ă�WRAM��DSP�̂��߂Ɋm�ۂ��Ă��邪
        // ���ۂɂ͕K�v�ȃX���b�g����������΂悢�B
        (void)MI_FreeWram_B(MI_WRAM_ARM9);
        (void)MI_CancelWram_B(MI_WRAM_ARM9);
        (void)MI_FreeWram_C(MI_WRAM_ARM9);
        (void)MI_CancelWram_C(MI_WRAM_ARM9);
        slotB = 0xFF;
        slotC = 0xFF;
        // ���̃T���v���ł͊ȈՉ��̂��ߐÓI��������̃t�@�C�����g�p����B
        FS_InitFile(file);
        DSPi_OpenStaticComponentG711Core(file);
        if (!DSP_LoadG711(file, slotB, slotC))
        {
            OS_TPanic("failed to load G.711 DSP-component! (lack of WRAM-B/C)");
        }
        (void)FS_CloseFile(file);

        // �T�E���h�g���@�\������
        SNDEX_Init();

        // �V���b�^�[���̃��[�h
        {
            FSFile file[1];
            SNDEXFrequency freq;

            if(SNDEX_GetI2SFrequency(&freq) != SNDEX_RESULT_SUCCESS)
            {
                OS_Panic("failed SNDEX_GetI2SFrequency");
            }
            // I2S������g���̏����l��32.73kHz�̂͂��ł���
            if(freq != SNDEX_FREQUENCY_32730)
            {
                OS_Panic("default value is SNDEX_FREQUENCY_32730");
            }

            FS_InitFile(file);
            // 32.73kHz�̃V���b�^�[�������[�h����
            if( !FS_OpenFileEx(file, filename1, FS_FILEMODE_R) )
            {
                OS_Panic("failed FS_OpenFileEx");
            }
            len = FS_GetFileLength(file);
            OS_TPrintf("len = %d\n", len);
            if( FS_ReadFile(file, StrmBuf, (s32)len) == -1)
            {
                OS_Panic("failed FS_ReadFile");
            }
            (void)FS_CloseFile(file);
            DC_StoreRange(StrmBuf, len);
        }
    }

    /* �T�E���h������ */
    SND_Init();
    SND_AssignWaveArc((SNDBankData*)sound_bank_data, 0, (SNDWaveArc*)sound_wave_data);
    SND_StartSeq(0, sound_seq_data, 0, (SNDBankData*)sound_bank_data);

    while (1)
    {
        // �p�b�h���̓ǂݍ��݂Ƒ���
        DEMOReadKey();

        if (PAD_DetectFold() == TRUE)
        {
            // �V���b�^�[���Đ����Ȃ�΁A�����܂ő҂�
            while(DSP_IsShutterSoundPlaying())
            {
                OS_Sleep(100);
            }
            // �X���[�v����O�� DSP ���~������
            DSP_UnloadG711();

            // �J������~����
            CAMERA_StopCapture();
            MI_StopNDma(NDMA_NO);
            result = CAMERA_I2CActivate(CAMERA_SELECT_NONE);
            if(result == CAMERA_RESULT_FATAL_ERROR)
                OS_Panic("CAMERA FATAL ERROR\n");
            wp_pending = TRUE;      // ����������t���[�����g�p����
            CAMERA_DmaRecvAsync(NDMA_NO, buffer[wp], CAMERA_GetBytesAtOnce(WIDTH), CAMERA_GET_FRAME_BYTES(WIDTH, HEIGHT), CameraDmaRecvIntr, NULL);
            CAMERA_ClearBuffer();
            CAMERA_StartCapture();

            PM_GoSleepMode(PM_TRIGGER_COVER_OPEN | PM_TRIGGER_CARD, 0, 0);

            // �X���[�v���A��� DSP ���ĊJ����
            {
                FSFile  file[1];
                int     slotB;
                int     slotC;

                slotB = 0xFF;
                slotC = 0xFF;
                // ���̃T���v���ł͊ȈՉ��̂��ߐÓI��������̃t�@�C�����g�p����B
                FS_InitFile(file);
                DSPi_OpenStaticComponentG711Core(file);
                if (!DSP_LoadG711(file, slotB, slotC))
                {
                    OS_TPanic("failed to load G.711 DSP-component! (lack of WRAM-B/C)");
                }
                (void)FS_CloseFile(file);
            }
            // �J�������X�^���o�C��ԂɂȂ��Ă���̂ŁA�A�N�e�B�u�ɂ���
            result = CAMERA_I2CActivate(current_camera);
            if(result == CAMERA_RESULT_FATAL_ERROR)
                OS_Panic("CAMERA FATAL ERROR\n");
            stabilizedCount = 0;
        }

        if(DEMO_IS_TRIG(PAD_BUTTON_A) && (shoot_flag == 0))
        {
            while(DSP_PlayShutterSound(StrmBuf, len) == FALSE)
            {
            }
            // �Î~��B�e���͊O���J���������v���ꎞ�������܂�
            if(current_camera == CAMERA_SELECT_OUT)
            {
                if(CAMERA_SwitchOffLED() != CAMERA_RESULT_SUCCESS)
                    OS_TPanic("CAMERA_SwitchOffLED was failed.\n");
            }
            OS_Sleep(200);
            shoot_flag = 1;
        }

        if(DEMO_IS_TRIG(PAD_BUTTON_X))
        {
            switchFlag = TRUE;
        }

        OS_WaitVBlankIntr();

        if(shoot_flag == 2)
        {
            GXS_LoadBG3Bmp((void*)((int)shoot_buffer), 0, BYTES_PER_LINE * HEIGHT);
            shoot_flag = 0;
        }

        // �T�E���h���C������
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
        {
        }
        (void)SND_FlushCommand(SND_COMMAND_NOBLOCK);
    }
}
