/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - demos - UnitTours/2D_CharBg_256BMP
  File:     main.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that use 256 color BITMAP BG:
// 
// This sample draw picture on the display with 256 color palette BITMAP BG.
// 
// 
// USAGE:
//   UP, DOWN, LEFT, RIGHT      : Slide the BG
//   A, B                       : Scaling the BG
//   L, R                       : Rotate the BG
//   SELECT + [UP, DOWN, LEFT, RIGHT] : Change the center position of rotation
//   SELECT                     : Switch ON/OFF that set BG area over mode, 
//                                loop or transparent.
//   
// HOWTO:
// 1. Transfer the palette data by GX_LoadBGPltt().
// 2. Transfer the screen data by GX_LoadBGxScr().
// 3. Set 256 color BITMAP mode by G2_SetBGxControl256Bmp().
//---------------------------------------------------------------------------


#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif
#include "DEMO.h"
#include "data.h"
//---------------------------------------------------------------------------
//  �T���F
//  �@�@�@�u�u�����N���荞�ݏ���
//  �����F
//  �@�@�@�u�u�����N���荞�݂��s��ꂽ���Ƃ��m�F���邽�߂̃t���O�𗧂Ă܂��B
//
//        ���ʏ������iDEMOInitCommon()�j�ŁA
//        �E�h�q�p���荞�݂̑I��(OS_SetIrqMask())�B
//        �E�h�q�p���荞�݂��s�����̊֐��̓o�^(OS_SetIRQFunction())�B
//        �E�h�q�p���荞�݂̋��̐ݒ�(OS_EnableIrq())�B
//        ���s���邱�ƂŁA���̊֐����u�u�����N�̂Ƃ��ɌĂ΂�܂��B
//---------------------------------------------------------------------------
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // VBlank ���荞�݂̊m�F�t���O��ݒ�
}

//---------------------------------------------------------------------------
//  �T���F
//        �A�t�B���g���E256�F�r�b�g�}�b�v�a�f��\��
//  �����F
//�@�@�@�@256�F�r�b�g�}�b�v�a�f���A�A�t�B���ϊ��i��]�g��k���j���s���ĕ\����
//      ��B
//
//        1. BGMODE��3�Ԃ�BG��3���g�p����B
//        2. G2_SetBG3Control256Bmp()�Őݒ���s���A�w��̏ꏊ�փL�����N�^�f�[�^
//         ��]������B
//        3. �X�N���[���f�[�^���w��̏ꏊ�֓]������B
//  ����F
//        PLUS_KEY          : BG�ʂ̃I�t�Z�b�g�𑀍삷��B
//        SELECT +  PLUS_KEY: ��]�E�g�k�̒��S�ʒu�𑀍삷��B
//        BUTTON_A,B        : �g�k����B
//        BUTTON_L,R        : ��]����B
//        SELECT            : �G���A�I�[�o�[�����̃X�C�b�`
//        START             : �ݒ�l������������B
//---------------------------------------------------------------------------
#ifdef SDK_TWL
void TwlMain(void)
#else
void NitroMain(void)
#endif
{
    int     trg = 0;
    int     offset_H = 0;
    int     offset_V = 30;
    fx32    scale = FX32_ONE;
    s16     x_0 = 0, y_0 = 0;
    s16     x_1 = 0, y_1 = 0;
    u16     rotate = 0;

    //---------------------
    // ������
    //---------------------
    DEMOInitCommon();
    DEMOInitVRAM();

    /* �a�f�̐ݒ� */
    GX_SetBankForBG(GX_VRAM_BG_256_AB); // VRAM-A,B�o���N���a�f�Ɋ��蓖��

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS,    // �Q�c�^�R�c���[�h
                       GX_BGMODE_3,    // BGMODE 3
                       GX_BG0_AS_2D);  // �a�f�O�ɂQ�c�\��

    GX_SetBGScrOffset(GX_BGSCROFFSET_0x00000);  // �X�N���[���I�t�Z�b�g�l��ݒ�
    GX_SetBGCharOffset(GX_BGCHAROFFSET_0x10000);        // �L�����N�^�I�t�Z�b�g��ݒ�

    GX_SetVisiblePlane(GX_PLANEMASK_BG3);       // �a�f�R��\���I��
    G2_SetBG3Priority(0);              // BG3�̗D�揇�ʂ��g�b�v�ɐݒ�
    G2_BG3Mosaic(FALSE);               // BG3�Ƀ��U�C�N���ʂ��s��Ȃ�

    /* �p���b�g�f�[�^�[�̓ǂݍ��݁i�a�f�p���b�g�f�[�^��VRAM�ɓ]���j */
    GX_LoadBGPltt((const void *)bitmapBG_256color_Palette,      // �]�����̃|�C���^ 
                  0,                   // �I�t�Z�b�g
                  PALETTE_SIZE);       // �T�C�Y

    /* �a�f�R�Ƀr�b�g�}�b�v�f�[�^��]�� */
    GX_LoadBG3Bmp(bitmapBG_256color_Texel, 0, SCREEN_SIZE);

    DEMOStartDisplay();

    //---------------------
    //  ���C�����[�v
    //---------------------
    while (1)
    {
        MtxFx22 mtx;

        /* �p�b�h���̓ǂݍ��݂Ƒ��� */
        DEMOReadKey();
#ifdef SDK_AUTOTEST                    // code for auto test
        {
            const EXTKeys keys[8] =
                { {PAD_BUTTON_A, 10}, {PAD_BUTTON_L, 10}, {PAD_KEY_UP | PAD_KEY_RIGHT, 20}, {0,
                                                                                             0}
            };
            EXT_AutoKeys(keys, &gKeyWork.press, &gKeyWork.trigger);
        }
#endif

        if (DEMO_IS_PRESS(PAD_BUTTON_SELECT))
        {
            if (DEMO_IS_PRESS(PAD_KEY_UP))
            {
                y_0 -= 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_DOWN))
            {
                y_0 += 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
            {
                x_0 += 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_LEFT))
            {
                x_0 -= 2;
            }
        }
        else
        {
            if (DEMO_IS_PRESS(PAD_KEY_UP))
            {
                offset_V += 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_DOWN))
            {
                offset_V -= 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
            {
                offset_H -= 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_LEFT))
            {
                offset_H += 2;
            }
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_A))
        {
            scale += 2 << (FX32_SHIFT - 8);
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_B))
        {
            scale -= 2 << (FX32_SHIFT - 8);
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_L))
        {
            rotate -= 256;
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_R))
        {
            rotate += 256;
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_SELECT))
        {
            trg = (trg + 1) & 0x01;
            OS_Printf("area_over=%d\n", trg);
        }

        if (DEMO_IS_TRIG(PAD_BUTTON_START))
        {
            offset_H = 0;
            offset_V = 0;
            x_0 = 32;
            y_0 = 32;
            scale = 1 << FX32_SHIFT;
            rotate = 0;
        }

        /* ��ʃT�C�Y256x256�h�b�g�A256�F�r�b�g�}�b�vB�ɂa�f�R��ݒ� */
        {
            GXBGAreaOver area_over = (trg ? GX_BG_AREAOVER_REPEAT : GX_BG_AREAOVER_XLU);

            // 256�F�r�b�g�}�b�v��BG��BG3��ݒ�
            // �@��ʃT�C�Y              �F��ʃT�C�Y256x256�h�b�g
            //   �G���A�I�[�o�[����      �Farea_over�ɂ���Č���
            //�@ �X�N���[���x�[�X�u���b�N�F0x00000
            G2_SetBG3Control256Bmp(GX_BG_SCRSIZE_256BMP_256x256,
                                   area_over, GX_BG_BMPSCRBASE_0x00000);
        }

        /* �A�t�B���ϊ��s��̐��� */
        {
            fx16    sinVal = FX_SinIdx(rotate);
            fx16    cosVal = FX_CosIdx(rotate);
            fx32    rScale = FX_Inv(scale);

            mtx._00 = (fx32)((cosVal * rScale) >> FX32_SHIFT);
            mtx._01 = (fx32)((sinVal * rScale) >> FX32_SHIFT);
            mtx._10 = -(fx32)((sinVal * rScale) >> FX32_SHIFT);
            mtx._11 = (fx32)((cosVal * rScale) >> FX32_SHIFT);
        }

#ifdef SDK_AUTOTEST                    // code for auto test
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0xCB08F058);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        /* VBlank�҂� */
        OS_WaitVBlankIntr();

        /* BG3���ʂɓK�p�����A�t�B���ϊ���ݒ� */
        G2_SetBG3Affine(&mtx,          // �ϊ��s��
                        x_0,           // ��]���S(X)���W
                        y_0,           // ��]���S(Y)���W
                        offset_H,      // ��]�O���W(X)
                        offset_V);     // ��]�O���W(Y)
    }
}
