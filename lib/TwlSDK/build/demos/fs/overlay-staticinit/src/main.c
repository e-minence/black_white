/*---------------------------------------------------------------------------*
  Project:  TWLSDK - demos - FS - overlay-staticinit
  File:     main.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-14 #$
  $Rev: 9314 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "DEMO.h"

/* �e���[�h�̃C���^�t�F�[�X */
#include "mode.h"


/*---------------------------------------------------------------------------*/
/* constants */

// �����T���v�����O�p�萔
#define     SAMPLING_FREQUENCE      4
#define     SAMPLING_BUFSIZE        (SAMPLING_FREQUENCE + 1)
#define     SAMPLING_START_VCOUNT   0


/*---------------------------------------------------------------------------*/
/* variables */

// �e���[�h�̃C���^�t�F�[�X
BOOL    (*UpdateFrame) (int frame_count, const InputData * input, int player_count,
                        int own_player_id);
void    (*DrawFrame) (int frame);
void    (*EndFrame) (FSOverlayID *p_next_mode);

// �^�b�`�p�l���p�����T���v�����O�o�b�t�@
static TPData tp_auto_buf[SAMPLING_BUFSIZE];


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         InitApp

  Description:  �A�v���P�[�V������{�����̏�����

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitApp(void)
{
    OS_Init();
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    FS_Init(FS_DMA_NOT_USE);

    // �^�b�`�p�l���̏�����
    {
        TPCalibrateParam calibrate;
        TP_Init();
        if (TP_GetUserInfo(&calibrate))
        {
            TP_SetCalibrateParam(&calibrate);
        }
        (void)TP_RequestAutoSamplingStart(SAMPLING_START_VCOUNT, SAMPLING_FREQUENCE, tp_auto_buf,
                                          SAMPLING_BUFSIZE);
    }

    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayBitmap();
    DEMOHookConsole();

    // ���C���f�B�X�v���C�Ń^�b�`�p�l�����g�p���邽�߂�GX�ݒ��؂�ւ���B
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 0, 1));
    DEMOSetBitmapGroundColor(DEMO_RGB_CLEAR);
    DEMOStartDisplay();
}

/*---------------------------------------------------------------------------*
  Name:         GetInput

  Description:  ���݂̓��͏�Ԃ��擾

  Arguments:    input             �擾�������̊i�[��

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void GetInput(InputData * input)
{
    if (input)
    {
        const u16 hold_bak = input->hold_bits;
        u16     hold_bits;
        // �^�b�`�p�l�������X�V
        const TPData *const cur_tp = tp_auto_buf + TP_GetLatestIndexInAuto();
        if (cur_tp->touch == TP_TOUCH_OFF)
        {
            input->tp.touch = TP_TOUCH_OFF;
        }
        else if (cur_tp->validity == TP_VALIDITY_VALID)
        {
            TP_GetCalibratedPoint(&input->tp, cur_tp);
        }
        // �p�b�h���(����у^�b�`�y���ڐG�r�b�g)���X�V
        hold_bits = (u16)(PAD_Read() | (input->tp.touch ? PAD_BUTTON_TP : 0));
        input->push_bits = (u16)(~hold_bak & hold_bits);
        input->hold_bits = hold_bits;
        input->release_bits = (u16)(hold_bak & ~hold_bits);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �A�v���P�[�V�����̃��C���G���g���|�C���g

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    // �A�v���P�[�V�����t���[�����[�N�Ɖ�ʑJ�ڏ�Ԃ����������܂�
    int         player_count = 1;
    int         own_player_id;
    InputData   input[1];
    FSOverlayID cur_mode;
    FSOverlayID prev_mode;
    FS_EXTERN_OVERLAY(top_menu);

    InitApp();
    cur_mode = FS_OVERLAY_ID(top_menu);
    prev_mode = FS_OVERLAY_ID(top_menu);
    UpdateFrame = NULL;
    DrawFrame = NULL;
    EndFrame = NULL;
    MI_CpuClear8(input, sizeof(input));
    player_count = 1;
    own_player_id = 0;

    // �I�[�o�[���C�Ƃ��Ċe���[�h���\����, �������J��Ԃ��J�ڂ��܂�
    for (;;)
    {
        // ���݂̃��[�h���J�n���܂��B
        // ���[�h�����ƂƂ��� NitroStaticInit() �Ŏ�������������
        // �K�v�ȃC���^�t�F�[�X�����Ȑݒ肳��܂��B
        // ���[�h�̏I��������, ���݂̃��[�h���g�� UpdateFrame �ŕԂ��܂��B
        {
            int     frame = 0;
            if (!FS_LoadOverlay(MI_PROCESSOR_ARM9, cur_mode))
            {
                OS_TPanic("failed to load specified mode(%08X)", cur_mode);
            }
            GetInput(&input[own_player_id]);
            for (;;)
            {
                GetInput(&input[own_player_id]);
                if (!UpdateFrame(frame, input, player_count, own_player_id))
                {
                    break;
                }
                DrawFrame(frame);
                DEMO_DrawFlip();
                OS_WaitVBlankIntr();
                ++frame;
            }
        }
        // ���݂̃��[�h���I����, ���̃��[�h�ֈڍs���܂��B
        // ���݂̃��[�h���ڍs��𖾎��I�ɐݒ肵�Ȃ������ꍇ
        // �O��̃��[�h�֖߂邱�ƂɂȂ�܂��B
        {
            FSOverlayID next_mode = prev_mode;
            EndFrame(&next_mode);
            if (!FS_UnloadOverlay(MI_PROCESSOR_ARM9, cur_mode))
            {
                OS_TPanic("failed to unload specified mode(%08X)", cur_mode);
            }
            prev_mode = cur_mode;
            cur_mode = next_mode;
        }
    }

}
