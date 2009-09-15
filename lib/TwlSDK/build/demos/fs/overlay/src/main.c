/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos - FS - overlay
  File:     main.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "DEMO.h"
#include "func.h"


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         LoadOverlayHigh

  Description:  �����Ƃ��ȒP�Ȏ菇�ŃI�[�o�[���C�����[�h���܂��B
                �ʏ�͂��̏����ŏ[���ł��B

  Arguments:    id : �I�[�o�[���CID

  Returns:      ���[�h�����̐���
 *---------------------------------------------------------------------------*/
static BOOL LoadOverlayHigh(MIProcessor target, FSOverlayID id)
{
    return FS_LoadOverlay(target, id);
}

/*---------------------------------------------------------------------------*
  Name:         LoadOverlayMiddle

  Description:  ���璷�Ȏ菇�ŃI�[�o�[���C�����[�h���܂��B
                �I�[�o�[���C���̎擾�����Ǝ��ۂ̃��W���[���ǂݍ��ݏ�����
                �������čs���K�v������ꍇ�͂��̂悤�ȃV�[�P���X�ɂȂ�܂��B

  Arguments:    id : �I�[�o�[���CID

  Returns:      ���[�h�����̐���
 *---------------------------------------------------------------------------*/
static BOOL LoadOverlayMiddle(MIProcessor target, FSOverlayID id)
{
    BOOL            retval = FALSE;
    FSOverlayInfo   info;
    if (FS_LoadOverlayInfo(&info, target, id))
    {
        if (FS_LoadOverlayImage(&info))
        {
            FS_StartOverlay(&info);
            retval = TRUE;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         LoadOverlayLow

  Description:  �����Ƃ��ᐅ����API�݂̂�g�ݍ��킹�ăI�[�o�[���C�����[�h���܂��B
                WFS���C�u�������g�p���ă��C�����X�o�R�ŃI�[�o�[���C���擾����ȂǁA
                �S�̂̐i�����A�v���P�[�V�������ŏڍׂɊǗ��������ꍇ��
                ���̃V�[�P���X�����Q�l���������B

  Arguments:    id : �I�[�o�[���CID

  Returns:      ���[�h�����̐���
 *---------------------------------------------------------------------------*/
static BOOL LoadOverlayLow(MIProcessor target, FSOverlayID id)
{
    BOOL            retval = FALSE;
    FSOverlayInfo   info;
    if (FS_LoadOverlayInfo(&info, target, id))
    {
        FSFile  file;
        FS_InitFile(&file);
        (void)FS_LoadOverlayImageAsync(&info, &file);
        (void)FS_WaitAsync(&file);
        (void)FS_CloseFile(&file);
        FS_StartOverlay(&info);
        retval = TRUE;
    }
    return retval;
}

// High,Middle,Low������̃��[�h�����������ł��B
// ���g�p�ɂȂ�V�[�����l�����ꂽ��œK�؂ȕ��@�����I�����������B
#define	LoadOverlay(id)	LoadOverlayLow(MI_PROCESSOR_ARM9, id)

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �A�v���P�[�V�������C���G���g��

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    OS_Init();
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayBitmap();
    DEMOHookConsole();

    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 0, 1));
    DEMOSetBitmapGroundColor(DEMO_RGB_CLEAR);
    DEMOStartDisplay();

    FS_Init(FS_DMA_NOT_USE);

    OS_TPrintf("--------------------------------\n"
               "overlay sample.\n");

    {
        // LSF�t�@�C���ɋL�q�����I�[�o�[���CID�͖����I�ɐ錾����K�v������܂��B
        // (���ۂ̃V���{���̓����N����LCF�e���v���[�g�t�@�C���������������܂�)
        FS_EXTERN_OVERLAY(main_overlay_1);
        FS_EXTERN_OVERLAY(main_overlay_2);
        FS_EXTERN_OVERLAY(main_overlay_3);

        /* *INDENT-OFF* */
        static struct
        {
            FSOverlayID     id;
            const char     *name;
        }
        list[] =
        {
            { FS_OVERLAY_ID(main_overlay_1), "main_overlay_1", },
            { FS_OVERLAY_ID(main_overlay_2), "main_overlay_2", },
            { FS_OVERLAY_ID(main_overlay_3), "main_overlay_3", },
        };
        static const int    overlay_max = sizeof(list) / sizeof(*list);
        /* *INDENT-ON* */

        int     i;
        int     loaded_overlay = -1;
        int     current = 0;

        // �e�I�[�o�[���C��̊֐��̃A�h���X��\�����܂��B
        // ���ɁAfunc_1��func_2�������A�h���X�ł���_�ɒ��ڂ��Ă��������B
        OS_TPrintf("func_1() : addr = 0x%08X.\n", func_1);
        OS_TPrintf("func_2() : addr = 0x%08X.\n", func_2);
        OS_TPrintf("func_3() : addr = 0x%08X.\n", func_3);

        // �I�[�o�[���C��I�����A���[�h���܂��B
        for (;;)
        {
            DEMOReadKey();
            // A�{�^���Ō��ݑI�����Ă���I�[�o�[���C�����[�h/�A�����[�h���܂��B
            if (DEMO_IS_TRIG(PAD_BUTTON_A))
            {
                if (loaded_overlay == -1)
                {
                    (void)LoadOverlay(list[current].id);
                    loaded_overlay = current;
                }
                else
                {
                    (void)FS_UnloadOverlay(MI_PROCESSOR_ARM9, list[loaded_overlay].id);
                    loaded_overlay = -1;
                }
            }
            // B�{�^���Ō��݂̃I�[�o�[���C��ɂ���֐���1��Ăяo���܂��B
            if (DEMO_IS_TRIG(PAD_BUTTON_B))
            {
                // �����̃I�[�o�[���C�̗̈悪�������鎞�A
                // FS���C�u�����͍Ō�Ƀ��[�h���ꂽ���̈ȊO�̓����ۏ؂��܂���B
                // �I�[�o�[���C��̊֐����g�p����ۂ͂��̓_�ɂ����ӂ��������B
                if (loaded_overlay == 0)
                {
                    func_1();
                }
                else if (loaded_overlay == 1)
                {
                    func_2();
                }
                else if (loaded_overlay == 2)
                {
                    func_3();
                }
            }
            // �㉺�L�[�ŃJ�[�\�����ړ����܂��B
            if (DEMO_IS_TRIG(PAD_KEY_DOWN))
            {
                if (++current >= overlay_max)
                {
                    current -= overlay_max;
                }
            }
            else if (DEMO_IS_TRIG(PAD_KEY_UP))
            {
                if (--current < 0)
                {
                    current += overlay_max;
                }
            }
            // ��ʕ\��
            {
                int     ox = 10;
                int     oy = 60;
                DEMOFillRect(0, 0, GX_LCD_SIZE_X, GX_LCD_SIZE_Y, DEMO_RGB_CLEAR);
                DEMOSetBitmapTextColor(GX_RGBA(0, 31, 0, 1));
                DEMODrawText(10, 10, "UP&DOWN :select overlay");
                DEMODrawText(10, 20, "A button:load/unload overlay");
                DEMODrawText(10, 30, "B button:call function");
                DEMOSetBitmapTextColor(GX_RGBA(31, 31, 31, 1));
                DEMODrawFrame(ox, oy, 240, 10 + overlay_max * 10, GX_RGBA( 0, 31, 0, 1));
                for (i = 0; i < overlay_max; ++i)
                {
                    BOOL    focus = (i == current);
                    BOOL    loaded = (list[i].id == loaded_overlay);
                    DEMOSetBitmapTextColor((GXRgb)(focus ? GX_RGBA(31, 31, 0, 1) : GX_RGBA(31, 31, 31, 1)));
                    DEMODrawText(ox + 10, oy + 5 + i * 10, "%s%s %s",
                                 focus ? ">" : " ", list[i].name, loaded ? "(loaded)" : "");
                }
            }
            DEMO_DrawFlip();
            OS_WaitVBlankIntr();
        }
    }

    OS_Terminate();
}
