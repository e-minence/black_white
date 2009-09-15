/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos.TWL - DSP - g711
  File:     main.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

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
#include <twl/dsp.h>
#include <twl/dsp/common/g711.h>

#include <DEMO.h>


/*---------------------------------------------------------------------------*/
/* functions */

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
    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 0, 1));
    DEMOSetBitmapGroundColor(DEMO_RGB_CLEAR);
    DEMOStartDisplay();

    // DSP�R���|�[�l���g�����[�h�B
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
        DSP_OpenStaticComponentG711(file);
        if (!DSP_LoadG711(file, slotB, slotC))
        {
            OS_TPanic("failed to load G.711 DSP-component! (lack of WRAM-B/C)");
        }
    }

    // G.711���������������E�����ł��邱�Ƃ��m�F�B
    {
        enum { sample_wave_max = 512 * 1024 };
        static u8   encbuf[sample_wave_max] ATTRIBUTE_ALIGN(4);
        static u8   srcbuf[sample_wave_max] ATTRIBUTE_ALIGN(4);
        static u8   dstbuf[sample_wave_max] ATTRIBUTE_ALIGN(4);
        u32         length;
        // �܂��ϊ��Ώۂ�PCM�g�`��p�ӂ���B
        {
            FSFile  file[1];
            u32     chunk;
            FS_InitFile(file);
            if (!FS_OpenFileEx(file, "rom:/fanfare.32.wav", FS_FILEMODE_R) ||
                !FS_SeekFile(file, 0x24, FS_SEEK_SET) ||
                (FS_ReadFile(file, &chunk, 4) != 4) ||
                (chunk != (('d'<<0)|('a'<<8)|('t'<<16) |('a'<<24))) ||
                (FS_ReadFile(file, &length, 4) != 4) ||
                (FS_ReadFile(file, srcbuf, (s32)length) != length))
            {
                OS_TPanic("cannot prepare sample waveform!");
            }
            (void)FS_CloseFile(file);
            DC_StoreRange(srcbuf, length);
        }
        // �P�ʂ��T���v�����֓���B
        length /= sizeof(u16);
        // ����ꂽPCM�g�`��A-law�`���ŕ������B
        DSP_EncodeG711(encbuf, srcbuf, length, DSP_AUDIO_CODEC_MODE_G711_ALAW);
        // ���̏�Ŋ�����҂B
        DSP_WaitForG711();
        // ����ꂽA-law�`����PCM�g�`�ɕ����B
        DSP_DecodeG711(dstbuf, encbuf, length, DSP_AUDIO_CODEC_MODE_G711_ALAW);
        // ���̏�Ŋ�����҂B
        DSP_WaitForG711();

        // G.711�Ƃ͒��ڊ֌W�Ȃ����AA�{�^������������T�E���h���Đ������Ă݂�B
        (void)SNDEX_SetDSPMixRate(0);
        for (;;)
        {
            DEMOReadKey();
            if (DEMO_IS_TRIG(PAD_BUTTON_A))
            {
                DSP_PlaySound(srcbuf, length*2, FALSE); // length �̓T���v�����Ȃ̂ŁA�o�C�g�P�ʂɏC��
            }
            DEMO_DrawFlip();
            OS_WaitVBlankIntr();
        }
    }

    // �s�v�ɂȂ��DSP�R���|�[�l���g���A�����[�h�B
    DSP_UnloadG711();

    // �����P���ȃT���v���Ȃ̂ŉ�ʕ\�����ׂ����̂͂Ȃ��B
    OS_Terminate();
}
