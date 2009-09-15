/*---------------------------------------------------------------------------*
  Project:  TwlSDK - DSP - demos - scaling-1
  File:     main.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

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
 640x480 �̉摜�f�[�^���A�w�肵���{���A�̈�ɂ��Ċg��i�k���j���s���f���ł��B
 
 �摜�̊g��i�k���j�̍ہA��ԕ��@�Ƃ��ĂR�̃��[�h���I���ł��܂��B
 
 �E������@
     - A : ��ԃ��[�h�̕ύX
           Nearest Neighbor, Bilinear, Bicubic, Nearest Neighbor... �̏��ɑJ��
           
     - UP, DOWN, RIGHT, LEFT : �̈�̈ړ�
           �g��i�k���j�������s���̈���ړ����܂��B
           640x480 �̉摜����͂ݏo���悤�Ȉړ��͂ł��܂���B
           
     - START : �f���̏I��
     
 �E���̑�
     - �{����ς������Ƃ��́A���̕��͂̉��̒萔 SCALING_FACTOR_X, SCALING_FACTOR_Y ��
       �ύX���čăr���h���ĉ������B
     
     - DSP �̏����̈�̃T�C�Y��ς������Ƃ��́AAREA_WIDTH, AREA_HEIGHT ��
       �ύX���čăr���h���ĉ������B
       
 ----------------------------------------------------------------------------*/

#include <twl.h>
#include <twl/dsp.h>

#include <DEMO.h>
#include <twl/dsp/common/graphics.h>

/* �ݒ� */
#define RUN_ASYNC     0            // �g�k������񓯊��Ŏ��s���邩�ǂ���
#define DMA_NO_FOR_FS 1            // FS_Init �p

#define AREA_WIDTH   185            // �g�k������������̈�� width
#define AREA_HEIGHT  140            // �g�k������������̈�� height

#define SCALING_FACTOR_X 1.4f      // X �����̔{���i�����_��3�ʂ܂ŗL��, ����� 31 �܂Łj
#define SCALING_FACTOR_Y 1.4f      // Y �����̔{���i�����_��3�ʂ܂ŗL��, ����� 31 �܂Łj

#define OUTPUT_WIDTH  DSP_CALC_SCALING_SIZE(AREA_WIDTH, SCALING_FACTOR_X)  // ������̉𑜓x, f32 �^�ɂ��덷���l��
#define OUTPUT_HEIGHT DSP_CALC_SCALING_SIZE(AREA_HEIGHT, SCALING_FACTOR_Y)

/*---------------------------------------------------------------------------*
 �摜�f�[�^ (640x480)
 *---------------------------------------------------------------------------*/
extern const u8 _binary_output_dat[];
extern const u8 _binary_output_dat_end[];

#define DATA_WIDTH    640
#define DATA_HEIGHT   480

/*---------------------------------------------------------------------------*
 �v���g�^�C�v�錾
*---------------------------------------------------------------------------*/
void VBlankIntr(void);

static void ExecScaling(void);
static void InitializeGraphics(void);
static void WriteScreenBuffer(u16 *data, u32 width, u32 height, u16 *scr);
static void ScalingCallbackFunc(void);

/*---------------------------------------------------------------------------*
 �����ϐ���`
*---------------------------------------------------------------------------*/
// ���C���E�T�u��ʂ̃X�N���[���o�b�t�@
static u16 ScrBuf[HW_LCD_WIDTH * HW_LCD_HEIGHT] ATTRIBUTE_ALIGN(32);
// DSP �ɂ��ϊ����ʂ��i�[����o�b�t�@
static u16 TmpBuf[OUTPUT_WIDTH * OUTPUT_HEIGHT] ATTRIBUTE_ALIGN(32);

static OSTick StartTick;       // DSP �ɂ�鏈�����Ԃ��v�����邽�߂̕ϐ�
static BOOL IsDspProcessing;   // DSP �����������s�����ǂ����i�񓯊��ŏ��������s���Ɏg�p�j

static u16 AreaX = 0;          // �g�k������������̈�̍���� X ���W
static u16 AreaY = 0;          // �g�k������������̈�̍���� Y ���W

// ��ԃ��[�h
static u16 ModeNames[3] = {
    DSP_GRAPHICS_SCALING_MODE_N_NEIGHBOR,
    DSP_GRAPHICS_SCALING_MODE_BILINEAR,
    DSP_GRAPHICS_SCALING_MODE_BICUBIC
    };

static u8 ModeNameStrings[3][24] = {
        "Nearest Neighbor",
        "Bilinear",
        "Bicubic"
        };

static u32 DspMode = 0;

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
    OS_InitAlarm();             // DSP_Scaling* �̓����Ŋ֐����g�p����ꍇ�͕K�{�i������ OS_Sleep ���g�p���Ă��邽�߁j

    DEMOInitVRAM();
    InitializeGraphics();

    DEMOStartDisplay();

    // �ŏ���ROM�w�b�_�ɏ]���� WRAM �������Ɋ��蓖�Ă��Ă���\��������̂ŁA�N���A����
    (void)MI_FreeWram_B( MI_WRAM_ARM9 );
    (void)MI_CancelWram_B( MI_WRAM_ARM9 );
    (void)MI_FreeWram_C( MI_WRAM_ARM9 );
    (void)MI_CancelWram_C( MI_WRAM_ARM9 );
    
    FS_Init(DMA_NO_FOR_FS);

    (void)OS_EnableInterrupts();

    IsDspProcessing = FALSE;
    
    // �X�N���[���o�b�t�@���N���A
    MI_CpuClear8(ScrBuf, sizeof(ScrBuf));

    // �O���t�B�b�N�X�R���|�[�l���g�̃��[�h
    DSP_OpenStaticComponentGraphics(&file);
    if(!DSP_LoadGraphics(&file, 0xFF, 0xFF))
    {
        OS_TPanic("failed to load graphics DSP-component! (lack of WRAM-B/C)");
    }
    
    // ������s
    ExecScaling();
    
    while (1)
    {
        DEMOReadKey();
        
        if (DEMO_IS_TRIG( PAD_BUTTON_START ))
        {
            break;    // �I��
        }
        
        // �����̑Ώۗ̈�̈ړ�
        if (DEMO_IS_PRESS( PAD_KEY_RIGHT ))
        {
            AreaX += 5;
            
            if (AreaX >= DATA_WIDTH - AREA_WIDTH - 1)
            {
                AreaX = DATA_WIDTH - AREA_WIDTH - 1;
            }
            ExecScaling();
        }
        else if (DEMO_IS_PRESS( PAD_KEY_LEFT ))
        {
            if (AreaX != 0)
            {
                if (AreaX <= 5)
                {
                    AreaX = 0;
                }
                else
                {
                    AreaX -= 5;
                }
            
                ExecScaling();
            }
        }
        
        if (DEMO_IS_PRESS( PAD_KEY_UP ))
        {
            if (AreaY != 0)
            {
                if (AreaY <= 5)
                {
                    AreaY = 0;
                }
                else
                {
                    AreaY -= 5;
                }
            
                ExecScaling();
            }
        }
        else if (DEMO_IS_PRESS( PAD_KEY_DOWN ))
        {
            AreaY += 5;
            if (AreaY >= DATA_HEIGHT - AREA_HEIGHT - 1)
            {
                AreaY = DATA_HEIGHT - AREA_HEIGHT - 1;
            }
            ExecScaling();
        }
        
        // ��ԃ��[�h�̕ύX
        if (DEMO_IS_TRIG( PAD_BUTTON_A ))
        {
            DspMode++;
            if (DspMode >= 3)
            {
                DspMode = 0;
            }
            
            ExecScaling();
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

/*--------------------------------------------------------------------------------
    DSP ��p���āA�ݒ肵���I�v�V�����ɂ��摜�f�[�^�̊g�k���s��
 ---------------------------------------------------------------------------------*/
static void ExecScaling()
{
    // ���s
#if RUN_ASYNC

	if ( !IsDspProcessing )
	{
		StartTick = OS_GetTick();

	    DSP_ScalingAsyncEx(_binary_output_dat, TmpBuf, DATA_WIDTH, DATA_HEIGHT,
	                       SCALING_FACTOR_X, SCALING_FACTOR_Y, ModeNames[DspMode], AreaX, AreaY, AREA_WIDTH, AREA_HEIGHT, ScalingCallbackFunc);

	    IsDspProcessing = TRUE;
	}

#else
	StartTick = OS_GetTick();
    (void)DSP_ScalingEx(_binary_output_dat, TmpBuf, DATA_WIDTH, DATA_HEIGHT,
                  SCALING_FACTOR_X, SCALING_FACTOR_Y, ModeNames[DspMode], AreaX, AreaY, AREA_WIDTH, AREA_HEIGHT);
    OS_TPrintf("mode: %s, time: %d microsec.\n", ModeNameStrings[DspMode], OS_TicksToMicroSeconds(OS_GetTick() - StartTick));

    // �X�N���[���ɕ\������悤�Ƀf�[�^�𒲐�
    WriteScreenBuffer(TmpBuf, OUTPUT_WIDTH, OUTPUT_HEIGHT, ScrBuf);
    
    // �X�N���[���o�b�t�@�̃L���b�V���̔j��
    DC_FlushAll();

    // �������ʂ� VRAM �փ��[�h
    GX_LoadBG3Bmp(ScrBuf, 0, HW_LCD_WIDTH * HW_LCD_HEIGHT * sizeof(u16));
#endif

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

/* data �̓��e�� 256x192 �̃X�N���[���o�b�t�@�֏������� */
static void WriteScreenBuffer(u16 *data, u32 width, u32 height, u16 *scr)
{
    int i;
    u32 lp_count;
    u32 tmp_linesize;
    
    // scrbuf �̍쐬
    if( height > HW_LCD_HEIGHT )
    {
        lp_count = HW_LCD_HEIGHT;
    }
    else
    {
        lp_count = height;
    }
    
    // 256x256 �� BMP �Ƃ��ĉ���ʂɕ\������s������T�C�Y���l������K�v������
    // 1���C�����R�s�[
    if( width > HW_LCD_WIDTH)
    {
        tmp_linesize = HW_LCD_WIDTH * sizeof(u16);
    }
    else
    {
        tmp_linesize = width * sizeof(u16);
    }
    
    for ( i=0; i < lp_count; i++ )
    {
        MI_CpuCopy( data + width * i, scr + HW_LCD_WIDTH * i, tmp_linesize );
    }
}

/* �g�k�����̏I�����ɌĂ΂��R�[���o�b�N�֐� */
static void ScalingCallbackFunc(void)
{
    OS_TPrintf("[Async]mode: %s, time: %d microsec.\n", ModeNameStrings[DspMode], OS_TicksToMicroSeconds(OS_GetTick() - StartTick));

	    // �X�N���[���ɕ\������悤�Ƀf�[�^�𒲐�
    WriteScreenBuffer(TmpBuf, OUTPUT_WIDTH, OUTPUT_HEIGHT, ScrBuf);
	    
    // �X�N���[���o�b�t�@�̃L���b�V���̔j��
    DC_FlushAll();

    // �������ʂ� VRAM �փ��[�h
    GX_LoadBG3Bmp(ScrBuf, 0, HW_LCD_WIDTH * HW_LCD_HEIGHT * sizeof(u16));

    IsDspProcessing = FALSE;
}

