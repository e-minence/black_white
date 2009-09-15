/*---------------------------------------------------------------------------*
  Project:  TwlSDK - DSP - demos - yuvToRgb
  File:     main.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-08#$
  $Rev: 9562 $
  $Author: kitase_hirotake $
*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
    �J�������� YUV422 �`���Ŏ擾�����摜���ADSP ��p����
    RGB555 �`���ɕϊ����ĉ�ʂɕ\������f���ł��B
    
    START �{�^���ŏI�����܂��B
 ----------------------------------------------------------------------------*/

#include <twl.h>
#include <twl/dsp.h>

#include <twl/camera.h>

#include <DEMO.h>
#include <twl/dsp/common/graphics.h>

/* �ݒ� */
#define DMA_NO_FOR_FS          3    // FS_Init �p
#define CAMERA_NEW_DMA_NO      1    // CAMERA�Ŏg�p����New DMA�ԍ�

#define CAM_WIDTH   256            // �J�������擾����摜�̕�
#define CAM_HEIGHT  192            // �J�������擾����摜�̍���

#define LINES_AT_ONCE   CAMERA_GET_MAX_LINES(CAM_WIDTH)     // ���̓]�����C����
#define BYTES_PER_LINE  CAMERA_GET_LINE_BYTES(CAM_WIDTH)    // �ꃉ�C���̓]���o�C�g��

/*---------------------------------------------------------------------------*
 �v���g�^�C�v�錾
*---------------------------------------------------------------------------*/
void VBlankIntr(void);
static void CameraIntrVsync(CAMERAResult result);
static void CameraIntrError(CAMERAResult result);
static void CameraIntrReboot(CAMERAResult result);
static void InitializeGraphics(void);
static void InitializeCamera(void);
static void WriteScreenBuffer(u16 *data, u32 width, u32 height, u16 *scr);
static void ConvertCallbackFunc(void);

/*---------------------------------------------------------------------------*
 �����ϐ���`
*---------------------------------------------------------------------------*/
// ���C���E�T�u��ʂ̃X�N���[���o�b�t�@
// �J�������擾�����摜�A�y��DSP �ɂ��ϊ����ʂ��i�[����o�b�t�@
static u16 TmpBuf[2][CAM_WIDTH * CAM_HEIGHT] ATTRIBUTE_ALIGN(32);

static BOOL StartRequest = FALSE;
static OSTick StartTick;       // DSP �ɂ�鏈�����Ԃ��v�����邽�߂̕ϐ�

static int wp;                  // �J��������f�[�^����荞�ݒ��̃o�b�t�@
static int rp;                  // �Ō��VRAM�ɃR�s�[�����o�b�t�@
static BOOL wp_pending;         // ��荞�݂𒆒f���� (�Ăѓ����o�b�t�@�Ɏ�荞��)

static BOOL IsConvertNow = FALSE;      // YUV -> RGB �ϊ����s���Ă��邩�ǂ���

/*---------------------------------------------------------------------------*
 Name:         TwlMain

 Description:  �������y�у��C�����[�v�B

 Arguments:    None.

 Returns:      None.
*---------------------------------------------------------------------------*/
void TwlMain(void)
{
    FSFile file;
    
    DEMOInitCommon();
    OS_InitThread();
    OS_InitTick();
    OS_InitAlarm();

    DEMOInitVRAM();
    InitializeGraphics();
    InitializeCamera();

    DEMOStartDisplay();
    
    // GX��DMA���g��Ȃ� (��DMA�̓J����DMA�̎ז��ɂȂ�)
    (void)GX_SetDefaultDMA(GX_DMA_NOT_USE);

    // �ŏ���ROM�w�b�_�ɏ]���� WRAM �������Ɋ��蓖�Ă��Ă���\��������̂ŁA�N���A����
    (void)MI_FreeWram_B( MI_WRAM_ARM9 );
    (void)MI_CancelWram_B( MI_WRAM_ARM9 );
    (void)MI_FreeWram_C( MI_WRAM_ARM9 );
    (void)MI_CancelWram_C( MI_WRAM_ARM9 );
    
    FS_Init(DMA_NO_FOR_FS);

    (void)OS_EnableInterrupts();

    // �O���t�B�b�N�X�R���|�[�l���g�̃��[�h
    DSP_OpenStaticComponentGraphics(&file);
    if(!DSP_LoadGraphics(&file, 0xFF, 0xFF))
    {
        OS_TPanic("failed to load graphics DSP-component! (lack of WRAM-B/C)");
    }
    
    // �J�����X�^�[�g
    wp = 0;
    rp = 1;
    wp_pending = TRUE;
    StartRequest = TRUE;
    CameraIntrVsync(CAMERA_RESULT_SUCCESS);
    OS_TPrintf("Camera is shooting a movie...\n");
    OS_TPrintf("Press A Button.\n");
    
    while (1)
    {
        DEMOReadKey();
        
        if (DEMO_IS_TRIG( PAD_BUTTON_START ))
        {
            break;    // �I��
        }
        
        if (wp == rp && !IsConvertNow)
        {
            rp ^= 1;
            DC_FlushRange(TmpBuf[rp], BYTES_PER_LINE * CAM_HEIGHT);
            GX_LoadBG3Scr(TmpBuf[rp], 0, BYTES_PER_LINE * CAM_HEIGHT);
        }
        
        OS_WaitVBlankIntr();           // Waiting the end of VBlank interrupt
    }
    
    OS_TPrintf("demo end.\n");

    // �O���t�B�b�N�X�R���|�[�l���g�̃A�����[�h
    DSP_UnloadGraphics();
    OS_Terminate();
}

//--------------------------------------------------------------------------------
//    �u�u�����N���荞�ݏ���
//
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking VBlank interrupt
}

// �J�������荞��
void CameraIntrError(CAMERAResult result)
{
#pragma unused(result)
    OS_TPrintf("Error was occurred.\n");
    // ��~����
    CAMERA_StopCapture();           // �J������~
    CAMERA_ClearBuffer();           // �N���A
    MI_StopNDma(CAMERA_NEW_DMA_NO); // DMA��~
    wp_pending = TRUE;              // ����������t���[�����g�p����
    StartRequest = TRUE;            // �J�����ĊJ�v��
}

void CameraIntrReboot(CAMERAResult result)
{
    if(result == CAMERA_RESULT_FATAL_ERROR)
    {
        return; // �J�������ċN���������s���Ă����A���邱�Ƃ��ł��Ȃ�����
    }
    // �J�����X�^�[�g
    CAMERA_ClearBuffer();
    MI_StopNDma(CAMERA_NEW_DMA_NO);
    wp_pending = TRUE;              // ����������t���[�����g�p����
    StartRequest = TRUE;            // �J�����ĊJ�v��
}


void CameraIntrVsync(CAMERAResult result)
{
#pragma unused(result)
    // �ȍ~��Vsync���̏���
    if (StartRequest)
    {
        CAMERA_ClearBuffer();
        CAMERA_StartCapture();
        StartRequest = FALSE;
    }

    if (CAMERA_IsBusy() == FALSE)   // done to execute stop command?
    {
    }
    else
    {
        if (MI_IsNDmaBusy(CAMERA_NEW_DMA_NO))    // NOT done to capture last frame?
        {
            OS_TPrintf("DMA was not done until VBlank.\n");
            MI_StopNDma(CAMERA_NEW_DMA_NO);  // DMA��~
        }
        // start to capture for next frame
        if (wp_pending)
        {
            wp_pending = FALSE;
        }
        else
        {
            // �X�V�o�b�t�@�ύX
            wp ^= 1;
            IsConvertNow = TRUE;
            StartTick = OS_GetTick();
            (void)DSP_ConvertYuvToRgbAsync(TmpBuf[rp ^1], TmpBuf[rp ^1], CAM_WIDTH * CAM_HEIGHT * sizeof(u16), ConvertCallbackFunc);
        }

        CAMERA_DmaRecvAsync(CAMERA_NEW_DMA_NO, TmpBuf[wp], CAMERA_GetBytesAtOnce(CAM_WIDTH), CAMERA_GET_FRAME_BYTES(CAM_WIDTH, CAM_HEIGHT), NULL, NULL);
    }
}

static void InitializeGraphics()
{
    // VRAM ���蓖��
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BG0_AS_2D);
    GX_SetVisiblePlane( GX_PLANEMASK_BG3 );
    GXS_SetGraphicsMode( GX_BGMODE_4 );
    GXS_SetVisiblePlane( GX_PLANEMASK_BG3 );
    
    GX_SetBGScrOffset(GX_BGSCROFFSET_0x00000);  // �X�N���[���I�t�Z�b�g�l��ݒ�
    GX_SetBGCharOffset(GX_BGCHAROFFSET_0x20000);  // �L�����N�^�x�[�X�I�t�Z�b�g�l��ݒ�
    
    G2_BlendNone();
    G2S_BlendNone();
    GX_Power2DSub(TRUE);    // �T�u2D�O���t�B�b�N�G���W�����I�t
    
    // main-BG
    // BG3: scr 96KB
    {
        G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
        G2_SetBG3Priority(2);
        G2_BG3Mosaic(FALSE);
    }
    
    // sub-BG
    // BG3: scr 96KB
    {
        G2S_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
        G2S_SetBG3Priority(2);
        G2S_BG3Mosaic(FALSE);
    }
}

static void InitializeCamera(void)
{
    CAMERAResult result;
    
    result = CAMERA_Init();
    if(result == CAMERA_RESULT_FATAL_ERROR)
        OS_TPanic("CAMERA_Init was failed.");

    result = CAMERA_I2CActivate(CAMERA_SELECT_IN);
    if (result == CAMERA_RESULT_FATAL_ERROR)
        OS_TPanic("CAMERA_I2CActivate was failed. (%d)\n", result);

    // �J����VSYNC���荞�݃R�[���o�b�N
    CAMERA_SetVsyncCallback(CameraIntrVsync);

    // �J�����G���[���荞�݃R�[���o�b�N
    CAMERA_SetBufferErrorCallback(CameraIntrError);
    
    // �J�����̍ċN�������R�[���o�b�N
    CAMERA_SetRebootCallback(CameraIntrReboot);

    CAMERA_SetOutputFormat(CAMERA_OUTPUT_YUV);
    CAMERA_SetTransferLines(CAMERA_GET_MAX_LINES(CAM_WIDTH));
}

/* YUV -> RGB �̃R���o�[�g�I�����ɌĂ΂��R�[���o�b�N�֐� */
static void ConvertCallbackFunc(void)
{
    OS_TPrintf("[Async]time: %d microsec.\n", OS_TicksToMicroSeconds(OS_GetTick() - StartTick));
    IsConvertNow = FALSE;
}

