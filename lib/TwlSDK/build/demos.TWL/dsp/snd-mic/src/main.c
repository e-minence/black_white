/*---------------------------------------------------------------------------*
  Project:  TwlSDK - DSP - demos.TWL - snd-mic
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


#include <twl.h>
#include <twl/dsp.h>
#include <twl/dsp/common/g711.h>

#include <DEMO.h>


/*---------------------------------------------------------------------------*/
/* constants */

#define TEST_BUFFER_SIZE    (1024 * 32)
#define WAV_BUFFER_SIZE     (1024 * 512)


/*---------------------------------------------------------------------------*/
/* variables */

// �T�E���h�Đ��e�X�g�p�̉����o�b�t�@�B
static u8           gWavData[WAV_BUFFER_SIZE] ATTRIBUTE_ALIGN(4);
static u32          gWavSize;

// �}�C�N�T���v�����O�e�X�g�p�̃����O�o�b�t�@�B
static u8           gDspData[TEST_BUFFER_SIZE] ATTRIBUTE_ALIGN(2);

// DSP�Ƃ̔�r�p�Ɏ��s����MIC���C�u�����̃T���v�����O������B
static MICAutoParam gMicAutoParam;
static u8           gMicData[TEST_BUFFER_SIZE] ATTRIBUTE_ALIGN(HW_CACHE_LINE_SIZE);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         NormalizeRingBuffer

  Description:  �����O�o�b�t�@����ŐV��HW_LCD_HEIGHT�T���v���������擾�B

  Arguments:    dst      : �T���v���i�[��B
                type     : �T���v����ʂ�\��MICSamplingType�^�B
                buffer   : �T���v���̊i�[���ꂽ�����O�o�b�t�@�擪�A�h���X�B
                position : �Ō�̃T���v�����O�ʒu�B(�o�C�g�P��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void NormalizeRingBuffer(void *dst, MICSamplingType type, const void *buffer, int position)
{
    const u8   *src = (const u8 *)buffer;
    u8         *tmp = (u8 *)dst;
    int         smp = 1;
    switch (type)
    {
    case MIC_SAMPLING_TYPE_8BIT:
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
        smp = sizeof(u8);
        break;
    case MIC_SAMPLING_TYPE_12BIT:
    case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
    case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
        smp = sizeof(u16);
        break;
    }
    {
        int     max = TEST_BUFFER_SIZE / smp;
        int     ofs = (position / smp - (HW_LCD_HEIGHT - 1)) & (max - 1);
        int     pos = 0;
        while (pos < HW_LCD_HEIGHT)
        {
            int     n = MATH_MIN(HW_LCD_HEIGHT - pos, max - ofs);
            MI_CpuCopy8(&src[ofs * smp], &tmp[pos * smp], (u32)(n * smp));
            ofs = (ofs + n) & (max - 1);
            pos += n;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DrawWaveGraph

  Description:  �O�p�|���S���Ŕg�`��`��

  Arguments:    type   : �T���v����ʂ�\��MICSamplingType�^�B
                color  : �g�`�̐F�B
                buffer : �w�肵���T���v����ʂŊi�[���ꂽ�g�`�z��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DrawWaveGraph(MICSamplingType type, GXRgb color, const void *buffer)
{
    int     i;
    int     bits = 8;
    BOOL    isSigned = FALSE;
    switch (type)
    {
    default:
    case MIC_SAMPLING_TYPE_8BIT:
        bits = 8, isSigned = FALSE;
        break;
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
        bits = 8, isSigned = TRUE;
        break;
    case MIC_SAMPLING_TYPE_12BIT:
    case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
        bits = 16, isSigned = FALSE;
        break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
    case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
        bits = 16, isSigned = TRUE;
        break;
    }
    for (i = 0; i < HW_LCD_HEIGHT - 1; ++i)
    {
        u8      cur8 = (u8)((bits == 8) ? ((const u8*)buffer)[i] : (((const u16*)buffer)[i] >> 8));
        u8      nxt8 = (u8)((bits == 8) ? ((const u8*)buffer)[i + 1] : (((const u16*)buffer)[i + 1] >> 8));
        int     cur = isSigned ? (((s8)cur8) + 128) : cur8;
        int     nxt = isSigned ? (((s8)nxt8) + 128) : nxt8;
        fx16    fsx = (fx16)(((cur - 128) * 0x1000) / 128);
        fx16    fsy = (fx16)(((96 - i) * 0x1000) / 96);
        fx16    fex = (fx16)(((nxt - 128) * 0x1000) / 128);
        fx16    fey = (fx16)(((96 - i + 1) * 0x1000) / 96);
        G3_Begin(GX_BEGIN_TRIANGLES);
        {
            G3_Color(color);
            G3_Vtx(fsx, fsy, 0);
            G3_Color(color);
            G3_Vtx(fex, fey, 0);
            G3_Color(color);
            G3_Vtx(fsx, fsy, 1);
        }
        G3_End();
    }
}

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void TwlMain(void)
{
    // OS�̏������B
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    FS_Init(FS_DMA_NOT_USE);
    SNDEX_Init();

    // ��ʕ\���̏������B
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayBitmap();
    DEMOHookConsole();
    G3X_InitMtxStack();
    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 0, 1));
    DEMOSetBitmapGroundColor(DEMO_RGB_CLEAR);
    DEMOStartDisplay();

    OS_TPrintf("A button: start DSP sampling\n");
    OS_TPrintf("B button: stop DSP sampling\n");
    OS_TPrintf("X button: pause rendering\n");
    OS_TPrintf("Y button: play oneshot sound\n");
    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 31, 1));
    DEMODrawText(0, 170, "--- DSP");
    DEMOSetBitmapTextColor(GX_RGBA(31, 31,  0, 1));
    DEMODrawText(0, 180, "--- MIC");
    DEMOSetBitmapTextColor(GX_RGBA( 0, 31,  0, 1));
    DEMODrawText(220,   5, "OLD");
    DEMODrawText(220, 180, "NEW");

    // �}�C�N���W���[���̓d����ON�B
    (void)PM_SetAmp(PM_AMP_ON);

    // MIC���C�u�����̎����T���v�����O���J�n�B
    {
        MIC_Init();
        gMicAutoParam.type = MIC_SAMPLING_TYPE_12BIT;
        gMicAutoParam.buffer = (void *)gMicData;
        gMicAutoParam.size = TEST_BUFFER_SIZE;
        gMicAutoParam.rate = MIC_SAMPLING_RATE_32730;
        gMicAutoParam.loop_enable = TRUE;
        gMicAutoParam.full_callback = NULL;
        for (;;)
        {
            MICResult   result = MIC_StartLimitedSampling(&gMicAutoParam);
            if (result == MIC_RESULT_SUCCESS)
            {
                break;
            }
            else if ((result == MIC_RESULT_BUSY) || (result == MIC_RESULT_SEND_ERROR))
            {
                OS_Sleep(1);
            }
            else if (result == MIC_RESULT_ILLEGAL_STATUS)
            {
                OS_TWarning("Already started sampling.\n");
                break;
            }
            else if (result == MIC_RESULT_ILLEGAL_PARAMETER)
            {
                OS_TWarning("Illegal parameter to start automatic sampling.\n");
                break;
            }
            else
            {
                OS_TPanic("MIC_StartLimtedSampling() replied fatal error (%d).\n", result);
            }
        }
    }

    // DSP�R���|�[�l���g�����[�h�B
    // (�ǂ̃R���|�[�l���g�ł��悢�̂ł����ł�G.711���g�p)
    {
        (void)MI_FreeWram_B(MI_WRAM_ARM9);
        (void)MI_CancelWram_B(MI_WRAM_ARM9);
        (void)MI_FreeWram_C(MI_WRAM_ARM9);
        (void)MI_CancelWram_C(MI_WRAM_ARM9);
        {
            FSFile  file[1];
            DSP_OpenStaticComponentG711(file);
            if (!DSP_LoadG711(file, 0xFF, 0xFF))
            {
                OS_TPanic("can't allocate WRAM Slot");
            }
        }
    }

    // �T�E���h�Đ��e�X�g�p��PCM�g�`�������B
    {
        FSFile  file[1];
        u32     chunk;
        FS_InitFile(file);
        if (!FS_OpenFileEx(file, "rom:/fanfare.32.wav", FS_FILEMODE_R) ||
            !FS_SeekFile(file, 0x24, FS_SEEK_SET) ||
            (FS_ReadFile(file, &chunk, 4) != 4) ||
            (chunk != (('d'<<0)|('a'<<8)|('t'<<16) |('a'<<24))) ||
            (FS_ReadFile(file, &gWavSize, 4) != 4) ||
            (FS_ReadFile(file, gWavData, (s32)gWavSize) != gWavSize))
        {
            OS_TPanic("cannot prepare sample waveform!");
        }
        (void)FS_CloseFile(file);
        DC_StoreRange(gWavData, gWavSize);
    }

    // DSP�T�E���h��100%�ɐݒ�B
    (void)SNDEX_SetDSPMixRate(0);

    // ���C�����[�v�B
    for (;;)
    {
        static BOOL pauseRendering = FALSE;

        DEMOReadKey();

        // A/B�{�^����DSP�T���v�����O���J�n/��~�B
        if (DEMO_IS_TRIG(PAD_BUTTON_A))
        {
            DSP_StartSampling(gDspData, sizeof(gDspData));
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_B))
        {
            DSP_StopSampling();
        }
        // X�{�^���Ŕg�`�`����ꎞ��~�B
        if (DEMO_IS_TRIG(PAD_BUTTON_X))
        {
            pauseRendering = !pauseRendering;
        }
        // Y�{�^���ŃT�E���h���Đ��B
        if (DEMO_IS_TRIG(PAD_BUTTON_Y))
        {
            DSP_PlaySound(gWavData, gWavSize, FALSE);
        }

        // ��r�̂��߁AMIC��DSP�̃T���v�����O�ʒu���Ȃ�ׂ������Ɏ擾����B
        {
            const u8   *dsp, *mic;

            // DSP�����̃T���v�����O�o�b�t�@�ƒ���I�ɓ�������K�v�����邪
            // ���̏�����CPU�ōs���邽�߃L���b�V���̖������͕s�v�B
            DSP_SyncSamplingBuffer();
            dsp = (const u8 *)DSP_GetLastSamplingAddress();
            mic = (const u8 *)MIC_GetLastSamplingAddress();
            // MIC���C�u������ARM7�����ڃ��C���������ɏ������ނ���
            // �����I�ȓ����͕s�v�����L���b�V���̖��������K�v�B
            DC_InvalidateRange(gMicData, sizeof(gMicData));

            // ���҂̍ŐV�g�`��`��B
            if (!pauseRendering)
            {
                G3X_Reset();
                G3_Identity();
                G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

                {
                    u8          tmp[HW_LCD_HEIGHT * sizeof(u16)];
                    const u8   *src = gDspData;
                    MICSamplingType type = MIC_SAMPLING_TYPE_SIGNED_12BIT;
                    NormalizeRingBuffer(tmp, type, src, dsp - src);
                    DrawWaveGraph(type, GX_RGB(31, 31, 31), tmp);
                }

                if (mic != NULL)
                {
                    u8          tmp[HW_LCD_HEIGHT * sizeof(u16)];
                    const u8   *src = gMicData;
                    MICSamplingType type = gMicAutoParam.type;
                    NormalizeRingBuffer(tmp, type, src, mic - src);
                    DrawWaveGraph(type, GX_RGB(31, 31,  0), tmp);
                }

                G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
            }
        }

        DEMO_DrawFlip();
        OS_WaitVBlankIntr();
    }

    OS_Terminate();
}
