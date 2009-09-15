/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos - spi - pmAmpGain
  File:     main.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-05-01#$
  $Rev: 5860 $
  $Author: terui $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    A sample that controls the amplifier gain for microphone input.

    USAGE:
        UP, DOWN : Control amplifier gain.

    HOWTO:
        1. Initialize MIC library.
        2. Start auto sampling of MIC by default configuration.
        3. When you change amplifier gain, you don't have to stop sampling.
 *---------------------------------------------------------------------------*/

#ifdef  SDK_TWL
#include    <twl.h>
#else
#include    <nitro.h>
#endif

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
#define     TEST_BUFFER_SIZE    (4 * 1024)   // 4K bytes
#define     RETRY_MAX_COUNT     8

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void     StepUpAmpGain(void);
static void     StepDownAmpGain(void);
static u32      GetDefaultMicSamplingRate(void);
#ifdef  SDK_TWL
static SNDEXFrequency   GetI2SFrequency(void);
#endif
static void     StartMicSampling(const MICAutoParam* param);

static void     SetDrawData(void *address, MICSamplingType type);

static void     VBlankIntr(void);
static void     Init3D(void);
static void     Draw3D(void);
static void     DrawLine(s16 sx, s16 sy, s16 ex, s16 ey);

/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static MICAutoParam gMicAutoParam;
static u8           gDrawData[192];
static u8           gMicData[TEST_BUFFER_SIZE] ATTRIBUTE_ALIGN(HW_CACHE_LINE_SIZE);

/*---------------------------------------------------------------------------*
  Name:         NitroMain/TwlMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef  SDK_TWL
void TwlMain(void)
#else
void NitroMain(void)
#endif
{
    /* �e�평���� */
    OS_Init();
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    /* �\���ݒ菉���� */
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();
    MI_CpuFillFast((void *)HW_OAM, GX_LCD_SIZE_Y, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, GX_LCD_SIZE_Y, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);

    /* 3D�֘A������ */
    Init3D();

    /* �����ݐݒ� */
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    /* �֘A���C�u���������� */
    {
        (void)PM_SetAmp(PM_AMP_ON);
        (void)PM_SetAmpGain(PM_AMPGAIN_80);
    
        MIC_Init();
        gMicAutoParam.type          =   MIC_SAMPLING_TYPE_8BIT;
        gMicAutoParam.buffer        =   (void *)gMicData;
        gMicAutoParam.size          =   TEST_BUFFER_SIZE;
        gMicAutoParam.loop_enable   =   TRUE;
        gMicAutoParam.full_callback =   NULL;
        gMicAutoParam.rate          =   GetDefaultMicSamplingRate();
        StartMicSampling((const MICAutoParam *)&gMicAutoParam);
    }

    /* �����ϐ������� */
    MI_CpuFill8(gDrawData, 0x80, sizeof(gDrawData));

    /* LCD�\���J�n */
    GX_DispOn();
    GXS_DispOn();

    /* �f�o�b�O������o�� */
    OS_Printf("pmAmpGain demo started.\n");
    OS_Printf("   up/down    -> change amplifier gain\n");
    OS_Printf("\n");

    /* �L�[���͏��擾�̋�Ăяo��(IPL �ł� A �{�^�������΍�) */
    (void)PAD_Read();

    {
        u16     keyOld  =   0;
        u16     keyTrg;
        u16     keyNow;
    
        /* ���C�����[�v */
        while (TRUE)
        {
            /* �L�[���͏��擾 */
            keyNow  =   PAD_Read();
            keyTrg  =   (u16)((keyOld ^ keyNow) & keyNow);
            keyOld  =   keyNow;
        
            /* �A���v������ύX */
            if (keyTrg & PAD_KEY_UP)
            {
                StepUpAmpGain();
            }
            if (keyTrg & PAD_KEY_DOWN)
            {
                StepDownAmpGain();
            }
        
            /* �g�`�`�� */
            SetDrawData(MIC_GetLastSamplingAddress(), gMicAutoParam.type);
            Draw3D();
        
            /* �u�u�����N�҂� */
            OS_WaitVBlankIntr();
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         StepDownAmpGain

  Description:  �A���v���������i�K�グ��B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
StepUpAmpGain(void)
{
    PMAmpGain   gain;

    if (PM_GetAmpGain(&gain) == PM_RESULT_SUCCESS)
    {
        switch (gain)
        {
        case PM_AMPGAIN_20:
            gain    =   PM_AMPGAIN_40;
            break;
        case PM_AMPGAIN_40:
            gain    =   PM_AMPGAIN_80;
            break;
        case PM_AMPGAIN_80:
            gain    =   PM_AMPGAIN_160;
            break;
        default:
            return;
        }
        if (PM_SetAmpGain(gain) == PM_RESULT_SUCCESS)
        {
            OS_Printf("Amplifier gain was changed to %d\n", gain);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         StepDownAmpGain

  Description:  �A���v���������i�K������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
StepDownAmpGain(void)
{
    PMAmpGain   gain;

    if (PM_GetAmpGain(&gain) == PM_RESULT_SUCCESS)
    {
        switch (gain)
        {
        case PM_AMPGAIN_40:
            gain    =   PM_AMPGAIN_20;
            break;
        case PM_AMPGAIN_80:
            gain    =   PM_AMPGAIN_40;
            break;
        case PM_AMPGAIN_160:
            gain    =   PM_AMPGAIN_80;
            break;
        default:
            return;
        }
        if (PM_SetAmpGain(gain) == PM_RESULT_SUCCESS)
        {
            OS_Printf("Amplifier gain was changed to %d\n", gain);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         GetDefaultMicSamplingRate

  Description:  �}�C�N�����T���v�����O�̃T���v�����O���[�g�����肷��B

  Arguments:    None.

  Returns:      u32 -   �K�؂ȃT���v�����O���[�g��Ԃ��B
 *---------------------------------------------------------------------------*/
static u32
GetDefaultMicSamplingRate(void)
{
#ifdef  SDK_TWL
    if ((OS_IsRunOnTwl() == TRUE) && (GetI2SFrequency() == SNDEX_FREQUENCY_47610))
    {
        return MIC_SAMPLING_RATE_15870;
    }
    else
    {
        return MIC_SAMPLING_RATE_16360;
    }
#else
    return MIC_SAMPLING_RATE_16K;
#endif
}

#ifdef  SDK_TWL
#include    <twl/ltdmain_begin.h>
/*---------------------------------------------------------------------------*
  Name:         GetI2SFrequency

  Description:  I2S ������g�����擾����B

  Arguments:    None.

  Returns:      SDNEXFrequency  -   I2S ������g����Ԃ��B
 *---------------------------------------------------------------------------*/
static SNDEXFrequency
GetI2SFrequency(void)
{
    SNDEXResult     result;
    SNDEXFrequency  freq;
    s32             retry   =   0;

    SNDEX_Init();
    while (TRUE)
    {
        result  =   SNDEX_GetI2SFrequency(&freq);
        switch (result)
        {
        case SNDEX_RESULT_SUCCESS:          // ����
            return freq;
        case SNDEX_RESULT_EXCLUSIVE:        // �r���G���[
        case SNDEX_RESULT_PXI_SEND_ERROR:   // PXI �L���[����t
            if (++ retry <= RETRY_MAX_COUNT)
            {
                OS_Sleep(1);
                continue;
            }
            OS_TWarning("%s: Retry count overflow.\n", __FUNCTION__);
            break;
        case SNDEX_RESULT_BEFORE_INIT:      // �������O
        case SNDEX_RESULT_ILLEGAL_STATE:    // ��Ԉُ�
            OS_TWarning("%s: Illegal state to get I2S frequency.\n", __FUNCTION__);
            break;
        case SNDEX_RESULT_FATAL_ERROR:      // �v���I�ȃG���[
        default:                            // ���̑��̒v���I�ȃG���[
            OS_Panic("Fatal error (%d).\n", result);
            /* never return */
        }
    }
    return SNDEX_FREQUENCY_32730;
}
#include    <twl/ltdmain_end.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         StartMicSampling

  Description:  �}�C�N�����T���v�����O�J�n�֐����Ăяo���A�G���[�������s���B

  Arguments:    param   -   �}�C�N API �֐��Ɉ����n���p�����[�^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
StartMicSampling(const MICAutoParam* param)
{
    MICResult   result;
    s32         retry   =   0;

    while (TRUE)
    {
#ifdef  SDK_TWL
        result  =   MIC_StartLimitedSampling(param);
#else
        result  =   MIC_StartAutoSampling(param);
#endif
        switch (result)
        {
        case MIC_RESULT_SUCCESS:            // ����
            return;
        case MIC_RESULT_BUSY:               // ���X���b�h����}�C�N���쒆
        case MIC_RESULT_SEND_ERROR:         // PXI �L���[����t
            if (++retry <= RETRY_MAX_COUNT)
            {
                OS_Sleep(1);
                continue;
            }
            OS_TWarning("%s: Retry count overflow.\n", __FUNCTION__);
            return;
        case MIC_RESULT_ILLEGAL_STATUS:     // �����T���v�����O��~���łȂ�
            OS_TWarning("%s: Already started sampling.\n", __FUNCTION__);
            return;
        case MIC_RESULT_ILLEGAL_PARAMETER:  // �p�����[�^���T�|�[�g�O
            OS_TWarning("%s: Illegal parameter to start automatic sampling.\n", __FUNCTION__);
            return;
        default:                            // ���̑��̒v���I�ȃG���[
            OS_Panic("Fatal error (%d).\n", result);
            /* never return */
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         SetDrawData

  Description:  ����ł̍ŐV�T���v�����O�ς݃f�[�^��\���ɔ��f������o�b�t�@��
                �i�[����B

  Arguments:    address - �R���|�[�l���g�ɂ���čŐV�̃T���v�����O�f�[�^��
                          �i�[���ꂽ���C����������̈ʒu�B
                type    - �T���v�����O���(�r�b�g��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetDrawData(void *address, MICSamplingType type)
{
    s32     i;

    /* �T���v�����O�������s���Ă��Ȃ��ꍇ�͉��������ɏI��
       (�}�C�N�Ɩ��֌W�̃������̃L���b�V����j�����Ă��܂���) */
    if ((address < gMicData) || (address >= (gMicData + TEST_BUFFER_SIZE)))
    {
        return;
    }

    switch (type)
    {
    case MIC_SAMPLING_TYPE_8BIT:
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
        /* 8 bit �T���v�����O�̏ꍇ */
        {
            u8     *p;

            p = (u8 *)((u32)address - 191);
            if (p < gMicData)
            {
                p = (u8 *)((u32)p + TEST_BUFFER_SIZE);
            }
            DC_InvalidateRange((void *)((u32)p & ~(HW_CACHE_LINE_SIZE - 1)), HW_CACHE_LINE_SIZE);
            for (i = 0; i < 192; i++)
            {
                gDrawData[i] = *p;
                p++;
                if ((u32)p >= (u32)(gMicData + TEST_BUFFER_SIZE))
                {
                    p -= TEST_BUFFER_SIZE;
                }
                if (((u32)p % HW_CACHE_LINE_SIZE) == 0)
                {
                    DC_InvalidateRange(p, HW_CACHE_LINE_SIZE);
                }
            }
        }
        break;
    case MIC_SAMPLING_TYPE_12BIT:
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
    case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
    case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
        /* 12 �r�b�g�T���v�����O�̏ꍇ */
        {
            u16    *p;

            p = (u16 *)((u32)address - 382);
            if ((u32)p < (u32)gMicData)
            {
                p = (u16 *)((u32)p + TEST_BUFFER_SIZE);
            }
            DC_InvalidateRange((void *)((u32)p & ~(HW_CACHE_LINE_SIZE - 1)), HW_CACHE_LINE_SIZE);
            for (i = 0; i < 192; i++)
            {
                gDrawData[i] = (u8)((*p >> 8) & 0x00ff);
                p++;
                if ((u32)p >= (u32)(gMicData + TEST_BUFFER_SIZE))
                {
                    p = (u16 *)((u32)p - TEST_BUFFER_SIZE);
                }
                if (((u32)p % HW_CACHE_LINE_SIZE) == 0)
                {
                    DC_InvalidateRange(p, HW_CACHE_LINE_SIZE);
                }
            }
        }
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃x�N�g���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    /* IRQ �`�F�b�N�t���O���Z�b�g */
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         Init3D

  Description:  3D�ɂĕ\�����邽�߂̏���������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Init3D(void)
{
    G3X_Init();
    G3X_InitMtxStack();
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    G2_SetBG0Priority(0);
    G3X_AlphaTest(FALSE, 0);
    G3X_AntiAlias(TRUE);
    G3X_EdgeMarking(FALSE);
    G3X_SetFog(FALSE, (GXFogBlend)0, (GXFogSlope)0, 0);
    G3X_SetClearColor(0, 0, 0x7fff, 63, FALSE);
    G3_ViewPort(0, 0, 255, 191);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
}

/*---------------------------------------------------------------------------*
  Name:         Draw3D

  Description:  3D�\���ɂĔg�`��\��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Draw3D(void)
{
    G3X_Reset();
    G3_Identity();
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

    {
        s32     i;

        if ((gMicAutoParam.type == MIC_SAMPLING_TYPE_SIGNED_8BIT) ||
            (gMicAutoParam.type == MIC_SAMPLING_TYPE_SIGNED_12BIT) ||
            (gMicAutoParam.type == MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF))
        {
            for (i = 0; i < 191; i++)
            {
                DrawLine((s16)((s8)gDrawData[i]),
                         (s16)i, (s16)((s8)gDrawData[i + 1]), (s16)(i + 1));
            }
        }
        else
        {
            for (i = 0; i < 191; i++)
            {
                DrawLine((s16)(gDrawData[i]), (s16)i, (s16)(gDrawData[i + 1]), (s16)(i + 1));
            }
        }
    }

    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

/*---------------------------------------------------------------------------*
  Name:         DrawLine

  Description:  �O�p�|���S���Ő���`��

  Arguments:    sx  - �`�悷����̊J�n�_�̂����W
                sy  - �`�悷����̊J�n�_�̂����W
                ex  - �`�悷����̏I�_�̂����W
                ey  - �`�悷����̏I�_�̂����W

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DrawLine(s16 sx, s16 sy, s16 ex, s16 ey)
{
    fx16    fsx = (fx16)(((sx - 128) * 0x1000) / 128);
    fx16    fsy = (fx16)(((96 - sy) * 0x1000) / 96);
    fx16    fex = (fx16)(((ex - 128) * 0x1000) / 128);
    fx16    fey = (fx16)(((96 - ey) * 0x1000) / 96);

    G3_Begin(GX_BEGIN_TRIANGLES);
    {
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fsx, fsy, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fex, fey, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fsx, fsy, 1);
    }
    G3_End();
}

