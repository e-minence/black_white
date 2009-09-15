/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MATH - demos
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
#include <nitro/fx/fx_trig.h>

#include <nitro/spi/common/pm_common.h>
#include <nitro/spi/ARM9/pm.h>


/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/

#define USE_FFTREAL

enum {
    DRAWMODE_REALTIME,
    DRAWMODE_BAR,
    DRAWMODE_SCALE,
    DRAWMODE_MAX
};

#define KEY_REPEAT_START    25     // �L�[���s�[�g�J�n�܂ł̃t���[����
#define KEY_REPEAT_SPAN     10     // �L�[���s�[�g�̊Ԋu�t���[����
#define SAMPLING_BUFFER_SIZE ( 1024 * 1024 ) // 1M

#define FFT_NSHIFT         11
#define FFT_N              (1 << FFT_NSHIFT)      // FFT ��K�p����T���v����
#define DRAW_STEP          MATH_MAX(FFT_N/2/256,1)    // �`�悷��X�e�b�v��
#define DRAW_MAX           MATH_MIN(FFT_N/2/DRAW_STEP, 256) // ���������ȊO�̓Ɨ��ȃp���[�l�Ɖ�ʂ̕��� min

#define SCALE_SAMPLING_OCTAVES 5

// �T�E���h�o�͊֘A
#define CHANNEL_NUM 4
#define ALARM_NUM 0
#define STREAM_THREAD_PRIO 12
#define THREAD_STACK_SIZE 1024
#define STRM_BUF_PAGESIZE 64*32
#define STRM_BUF_SIZE STRM_BUF_PAGESIZE*2
#define STRM_SAMPLE_RATE 44100
#define OSC_MAX_VOLUME 32767

// �L�[�ԍ�������g�������߂�
#define GetFreq(pitch) (SND_TIMER_CLOCK / SND_CalcTimer((SND_TIMER_CLOCK / 440), (pitch - 69 * 64)))

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
// �L�[���͏��
typedef struct KeyInformation
{
    u16     cnt;                       // �����H���͒l
    u16     trg;                       // �����g���K����
    u16     up;                        // �����g���K����
    u16     rep;                       // �����ێ����s�[�g����

}
KeyInformation;

// �T�E���h�o�͊֘A
// �X�g���[���I�u�W�F�N�g
typedef struct StreamInfo
{
    u32     bufPage;
}
StreamInfo;

// �^�b�`�p�l��
typedef struct TpInformation
{
    int     touch:1;
    int     trg:1;
    int     rls:1;
    u16     x;
    u16     y;
}
TpInformation;

// �I�V���[�^
typedef struct Oscillator
{
    fx16    index;
    fx16    step;
    fx32    rate;                      // �o�̓T���v�����O���[�g
    u16     gain;
    u16     dummy;
}
Oscillator;

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void);
static void Init3D(void);
static void Draw3D_Realtime(void);
static void Draw3D_Bar(void);
static void Draw3D_Scale(void);
static void DrawLine(s16 sx, s16 sy, s16 ex, s16 ey);
static void DrawBar(s16 sx, s16 sy, s16 ex, s16 ey);
static void DrawBarWithColor(s16 sx, s16 sy, s16 ex, s16 ey, u32 c);
static void VBlankIntr(void);
static void KeyRead(KeyInformation * pKey);
static void SetDrawData(void *address);
static void PrintfVariableData(void);

// �T�E���h�o�͊֘A
static void SoundAlarmHandler(void *arg);
static void StrmThread(void *arg);
static void Play(StreamInfo * strm);
static void Stop();
static void MakeStreamData(StreamInfo * strm);
static void TpRead(TpInformation* tp);

/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static MICAutoParam gMicAutoParam;
static u8 *gMicData;
static u8 gDrawData[DRAW_MAX];

// FFT �p�̃o�b�t�@�Q
static fx16 sinTable[FFT_N - FFT_N / 4];
#ifdef USE_FFTREAL
static fx16 sinTable2[(FFT_N - FFT_N / 4) / 2];
static fx32 data[FFT_N];
#else
static fx32 data[FFT_N * 2];
#endif
static s32 power[FFT_N/2+1];
static s32 smoothedPower[FFT_N/2+1];

static BOOL drawMode;
static u8 blockCount;

// �T�E���h�o�͊֘A
static u64 strmThreadStack[THREAD_STACK_SIZE / sizeof(u64)];
static OSThread strmThread;
static OSMessageQueue msgQ;
static OSMessage msgBuf[1];

static u8 strmBuf[STRM_BUF_SIZE] ATTRIBUTE_ALIGN(32);

static Oscillator osc;
static TPCalibrateParam calibrate;

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    KeyInformation key;
    TpInformation tp;
    StreamInfo strm;

    // �e�평����
    OS_Init();
    OS_InitTick();
    OS_InitThread();
    FX_Init();
    GX_Init();
    GX_DispOff();
    GXS_DispOff();
    SND_Init();
    TP_Init();

    // �\���ݒ菉����
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();
    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);

    // 3D�֘A������
    Init3D();

    // �^�b�`�p�l���ǂݍ��ݏ�����
    (void)TP_GetUserInfo(&calibrate);
    TP_SetCalibrateParam(&calibrate);

    // �I�V���[�^�[�ݒ�
    osc.rate = STRM_SAMPLE_RATE << FX32_SHIFT;

    // �����ݐݒ�
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    //****************************************************************
    // Initialize MIC.
    InitializeAllocateSystem();
    // OS_Alloc�Ŋm�ۂ�����������32�o�C�gALIGN�ɂȂ��Ă���̂ŁA
    // �L���b�V�����삵�Ă����̃��������󂳂Ȃ��B
    gMicData = (u8 *)OS_Alloc(SAMPLING_BUFFER_SIZE);
    SDK_NULL_ASSERT(gMicData);
    MIC_Init();

    // PMIC������
    PM_Init();
    // AMP�I��
    (void)PM_SetAmp(PM_AMP_ON);
    // AMP�Q�C������
    (void)PM_SetAmpGain(PM_AMPGAIN_80);

    // �t�[���G�ϊ��̐��x���グ�邽�߂� 12bit �ŃT���v�����O���s��
    gMicAutoParam.type = MIC_SAMPLING_TYPE_12BIT;
    gMicAutoParam.buffer = (void *)gMicData;
    gMicAutoParam.size = SAMPLING_BUFFER_SIZE;
    gMicAutoParam.loop_enable = TRUE;
    gMicAutoParam.full_callback = NULL;
#ifdef SDK_TWL
    gMicAutoParam.rate = MIC_SAMPLING_RATE_8180;
    (void)MIC_StartLimitedSampling(&gMicAutoParam);
#else   // ifdef SDK_TWL
    gMicAutoParam.rate = MIC_SAMPLING_RATE_8K;
    (void)MIC_StartAutoSampling(&gMicAutoParam);
#endif  // ifdef SDK_TWL else

    //****************************************************************

    // �����ϐ�������
    MATH_MakeFFTSinTable(sinTable, FFT_NSHIFT);
#ifdef USE_FFTREAL
    MATH_MakeFFTSinTable(sinTable2, FFT_NSHIFT - 1);
#endif
    MI_CpuClear8(gDrawData, sizeof(gDrawData));

    // LCD�\���J�n
    GX_DispOn();
    GXS_DispOn();

    // �f�o�b�O������o��
    OS_Printf("ARM9: FFT spectrum demo started.\n");
    OS_Printf("   up/down    -> change bar width (Bar Mode)\n");
    OS_Printf("   left/right -> change draw mode\n");
    OS_Printf("\n");
    PrintfVariableData();

    // �L�[���͏��擾�̋�Ăяo��(IPL �ł� A �{�^�������΍�)
    KeyRead(&key);

    // �`�����l�������b�N����
    SND_LockChannel(1 << CHANNEL_NUM, 0);

    /* �X�g���[���X���b�h�̋N�� */
    OS_CreateThread(&strmThread,
                    StrmThread,
                    NULL,
                    strmThreadStack + THREAD_STACK_SIZE / sizeof(u64),
                    THREAD_STACK_SIZE, STREAM_THREAD_PRIO);
    OS_WakeupThreadDirect(&strmThread);

    // �\�����[�h�̏����l�̓o�[�\��
    drawMode = DRAWMODE_BAR;
    blockCount = 4;

    // main loop
    while (TRUE)
    {
        // �`�q�l�V�R�}���h������M
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
        {
        }

        // �L�[���͏��擾
        KeyRead(&key);

        // �^�b�`�p�l���̓ǂݍ���
        TpRead(&tp);

        // �σp�����[�^�ύX
        {

            // �\�����[�h��ύX
            if ((key.trg | key.rep) & (PAD_KEY_LEFT | PAD_KEY_RIGHT))
            {
                if ((key.trg | key.rep) & PAD_KEY_RIGHT)
                {
                    drawMode++;
                }
                if ((key.trg | key.rep) & PAD_KEY_LEFT)
                {
                    drawMode--;
                }

                if (drawMode < 0)
                {
                    drawMode += DRAWMODE_MAX;
                }
                if (drawMode >= DRAWMODE_MAX)
                {
                    drawMode -= DRAWMODE_MAX;
                }

                // �\�����[�h��؂�ւ���ꍇ�́A�ݐσf�[�^�̏�����
                MI_CpuClear32(smoothedPower, sizeof(smoothedPower));
            }

            // �u���b�N���ύX
            if ((key.trg | key.rep) & PAD_KEY_UP)
            {
                blockCount += 1;
                if (blockCount >= 16)
                {
                    blockCount = 16;
                }
                OS_Printf("block = %d\n", blockCount);
            }
            if ((key.trg | key.rep) & PAD_KEY_DOWN)
            {
                blockCount -= 1;
                if (blockCount <= 2)
                {
                    blockCount = 2;
                }
                OS_TPrintf("block = %d\n", blockCount);
            }
        }

        // �g�`�f�[�^�̌v�Z
        SetDrawData(MIC_GetLastSamplingAddress());

        // �g�`�`��
        // drawMode���Ƃɐ؂�ւ���
        switch (drawMode)
        {
        case DRAWMODE_REALTIME:
            Draw3D_Realtime();
            break;

        case DRAWMODE_BAR:
            Draw3D_Bar();
            break;

        case DRAWMODE_SCALE:
            Draw3D_Scale();
            break;
        }

        // �^�b�`�p�l���̐G�����ʒu�ɉ������g�`���o��
        if (tp.touch)
        {
            osc.step = (fx16)FX_Div(GetFreq(tp.x * 12 + 60 * 64) << FX32_SHIFT, osc.rate);
            osc.gain = (u16)(tp.y * OSC_MAX_VOLUME / 192);
        }

        // PSG��`�g���Đ�
        if (tp.trg)
        {
            Play(&strm);
        }

        if (tp.rls)
        {
            Stop();
        }

        // �R�}���h�t���b�V��
        (void)SND_FlushCommand(SND_COMMAND_NOBLOCK);
        // �u�u�����N�҂�
        OS_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         InitializeAllocateSystem

  Description:  ���C����������̃A���[�i�ɂă����������ăV�X�e��������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void)
{
    void   *tempLo;
    OSHeapHandle hh;

    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
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
  Name:         Draw3D_Realtime

  Description:  3D�\���ɂĔg�`�����A���^�C���\��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Draw3D_Realtime(void)
{
    G3X_Reset();
    G3_Identity();
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

    {
        s32     i;

        for (i = 0; i < DRAW_MAX-1; i++)
        {
            DrawLine((s16)i, (s16)(192 - gDrawData[i]),
                     (s16)(i + 1), (s16)(192 - gDrawData[i + 1]));
        }
    }

    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

/*---------------------------------------------------------------------------*
  Name:         Draw3D_Bar

  Description:  3D�\���ɂĔg�`���o�[�\��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Draw3D_Bar(void)
{
    G3X_Reset();
    G3_Identity();
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

    {
        s32     i;

        for (i = 0; i < DRAW_MAX-1; i += blockCount)
        {
            DrawBar((s16)i, (s16)(192 - gDrawData[i]), (s16)(i + blockCount), (s16)192);
        }
    }

    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

/*---------------------------------------------------------------------------*
  Name:         Draw3D_Scale

  Description:  3D�\���ɂĔg�`�����K�ʕ\��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Draw3D_Scale(void)
{
    G3X_Reset();
    G3_Identity();
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

    {
        s32     i;

        for (i = 0; i < 12; i++)
        {
            s32     j;
            for (j = 0; j < SCALE_SAMPLING_OCTAVES; j++)
            {
                DrawBarWithColor((s16)((i * (SCALE_SAMPLING_OCTAVES + 2) + j) * 3 + 2),
                                 (s16)(192 - gDrawData[i * SCALE_SAMPLING_OCTAVES + j]),
                                 (s16)((i * (SCALE_SAMPLING_OCTAVES + 2) + j) * 3 + 5),
                                 (s16)192, (u32)(j + 1));
            }
        }
    }

    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

const GXRgb ColorTable[8] =
{
    GX_RGB( 0,  0,  0),
    GX_RGB(20, 20, 31),
    GX_RGB(20, 31, 20),
    GX_RGB(28, 31, 20),
    GX_RGB(31, 25, 20),
    GX_RGB(31, 20, 20),
    GX_RGB(31, 24, 24),
    GX_RGB(31, 31, 31),
};

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

/*---------------------------------------------------------------------------*
  Name:         DrawBar

  Description:  �l�p(�����`)�|���S���Ńo�[��`��

  Arguments:    sx  - �`�悷��l�p�̍���̂����W
                sy  - �`�悷��l�p�̍���̂����W
                ex  - �`�悷��l�p�̉E���̂����W
                ey  - �`�悷��l�p�̉E���̂����W

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DrawBar(s16 sx, s16 sy, s16 ex, s16 ey)
{
    fx16    fsx = (fx16)(((sx - 128) * 0x1000) / 128);
    fx16    fsy = (fx16)(((96 - sy) * 0x1000) / 96);
    fx16    fex = (fx16)(((ex - 128) * 0x1000) / 128);
    fx16    fey = (fx16)(((96 - ey) * 0x1000) / 96);

    G3_Begin(GX_BEGIN_QUADS);
    {
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fsx, fsy, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fsx, fey, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fex, fey, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fex, fsy, 0);
    }
    G3_End();
}

/*---------------------------------------------------------------------------*
  Name:         DrawBarWithColor

  Description:  �l�p(�����`)�|���S���Ńo�[��`��

  Arguments:    sx  - �`�悷��l�p�̍���̂����W
                sy  - �`�悷��l�p�̍���̂����W
                ex  - �`�悷��l�p�̉E���̂����W
                ey  - �`�悷��l�p�̉E���̂����W
                c   - �`�悷��F

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DrawBarWithColor(s16 sx, s16 sy, s16 ex, s16 ey, u32 c)
{
    fx16    fsx = (fx16)(((sx - 128) * 0x1000) / 128);
    fx16    fsy = (fx16)(((96 - sy) * 0x1000) / 96);
    fx16    fex = (fx16)(((ex - 128) * 0x1000) / 128);
    fx16    fey = (fx16)(((96 - ey) * 0x1000) / 96);
    GXRgb color;

    if ( c >= sizeof(ColorTable)/sizeof(ColorTable[0]) )
    {
        c = 0;
    }
    color = ColorTable[c];

    G3_Begin(GX_BEGIN_QUADS);
    {
        G3_Color(color);
        G3_Vtx(fsx, fsy, 0);
        G3_Color(color);
        G3_Vtx(fsx, fey, 0);
        G3_Color(color);
        G3_Vtx(fex, fey, 0);
        G3_Color(color);
        G3_Vtx(fex, fsy, 0);
    }
    G3_End();
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃x�N�g���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    // IRQ �`�F�b�N�t���O���Z�b�g
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  �L�[���͏���ҏW����B
                �����g���K�A�����g���K�A�����p�����s�[�g�����o�B

  Arguments:    pKey  - �ҏW����L�[���͏��\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void KeyRead(KeyInformation * pKey)
{
    static u16 repeat_count[12];
    int     i;
    u16     r;

    r = PAD_Read();
    pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < 12; i++)
    {
        if (r & (0x0001 << i))
        {
            if (!(pKey->cnt & (0x0001 << i)))
            {
                pKey->trg |= (0x0001 << i);     // �����g���K
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // �����p�����s�[�g
                    repeat_count[i] = KEY_REPEAT_START - KEY_REPEAT_SPAN;
                }
                else
                {
                    repeat_count[i]++;
                }
            }
        }
        else
        {
            if (pKey->cnt & (0x0001 << i))
            {
                pKey->up |= (0x0001 << i);      // �����g���K
            }
        }
    }
    pKey->cnt = r;                     // �����H�L�[����
}

// �e���K�ɑΉ�����t�[���G�ϊ���̔z��̃C���f�b�N�X
// �Q�l���:
//   �T���v�����O�藝���A�t�[���G�ϊ��ŌW�����擾�\�ȍő���g����
//   �T���v�����O���g���̔����ł��� 4000Hz �ƂȂ�܂��B
//   �܂��A�t�[���G�ϊ��œ�����Ɨ������p���[�l�� FFT_N/2 �ł��B
//   4000Hz / (FFT_N/2) ���A�t�[���G�ϊ��̌��ʓ�����X�y�N�g����
//   ���g���𑜓x�ƂȂ�܂��B
//   (1) ���g���𑜓x���グ����
//     FFT_N �𑝂₷ �� �������Ԃ������� and ���ԉ𑜓x��������
//     �T���v�����O���g����������i���̓f�[�^���Ԉ�������j �� ���g���ш悪������
//   (2) ���g���ш���グ����
//     �T���v�����O���g�����グ�� �� ���g���𑜓x��������
static u32 scaleSamplingPoints[12][SCALE_SAMPLING_OCTAVES][2] =
{
    // FFT_N = 2048
    { // C
        { 33 , 34  }, /* (130.813Hz : 33.4881) */
        { 65 , 69  }, /* (261.626Hz : 66.9761) */
        { 130, 138 }, /* (523.251Hz : 133.952) */
        { 260, 276 }, /* (1046.5 Hz : 267.905) */
        { 521, 552 }, /* (2093   Hz : 535.809) */
    },
    { // C#
        { 34 , 37  }, /* (138.591Hz : 35.4794) */
        { 69 , 73  }, /* (277.183Hz : 70.9588) */
        { 138, 146 }, /* (554.365Hz : 141.918) */
        { 276, 292 }, /* (1108.73Hz : 283.835) */
        { 552, 584 }, /* (2217.46Hz : 567.67 ) */
    },
    { // D
        { 37 , 39  }, /* (146.832Hz : 37.5891) */
        { 73 , 77  }, /* (293.665Hz : 75.1782) */
        { 146, 155 }, /* (587.33 Hz : 150.356) */
        { 292, 310 }, /* (1174.66Hz : 300.713) */
        { 584, 619 }, /* (2349.32Hz : 601.425) */
    },
    { // D#
        { 39 , 41  }, /* (155.563Hz : 39.8243) */
        { 77 , 82  }, /* (311.127Hz : 79.6485) */
        { 155, 164 }, /* (622.254Hz : 159.297) */
        { 310, 328 }, /* (1244.51Hz : 318.594) */
        { 619, 656 }, /* (2489.02Hz : 637.188) */
    },
    { // E
        { 41 , 43  }, /* (164.814Hz : 42.1923) */
        { 82 , 87  }, /* (329.628Hz : 84.3847) */
        { 164, 174 }, /* (659.255Hz : 168.769) */
        { 328, 347 }, /* (1318.51Hz : 337.539) */
        { 656, 695 }, /* (2637.02Hz : 675.077) */
    },
    { // F
        { 43 , 46  }, /* (174.614Hz : 44.7012) */
        { 87 , 92  }, /* (349.228Hz : 89.4024) */
        { 174, 184 }, /* (698.456Hz : 178.805) */
        { 347, 368 }, /* (1396.91Hz : 357.61 ) */
        { 695, 736 }, /* (2793.83Hz : 715.219) */
    },
    { // F#
        { 46 , 49  }, /* (184.997Hz : 47.3593) */
        { 92 , 97  }, /* (369.994Hz : 94.7186) */
        { 184, 195 }, /* (739.989Hz : 189.437) */
        { 368, 390 }, /* (1479.98Hz : 378.874) */
        { 736, 780 }, /* (2959.96Hz : 757.749) */
    },
    { // G
        { 49 , 52  }, /* (195.998Hz : 50.1754) */
        { 97 , 103 }, /* (391.995Hz : 100.351) */
        { 195, 207 }, /* (783.991Hz : 200.702) */
        { 390, 413 }, /* (1567.98Hz : 401.403) */
        { 780, 826 }, /* (3135.96Hz : 802.807) */
    },
    { // G#
        { 52 , 55  }, /* (207.652Hz : 53.159 ) */
        { 103, 109 }, /* (415.305Hz : 106.318) */
        { 207, 219 }, /* (830.609Hz : 212.636) */
        { 413, 438 }, /* (1661.22Hz : 425.272) */
        { 826, 875 }, /* (3322.44Hz : 850.544) */
    },
    { // A
        { 55 , 58  }, /* (220    Hz : 56.32  ) */
        { 109, 116 }, /* (440    Hz : 112.64 ) */
        { 219, 232 }, /* (880    Hz : 225.28 ) */
        { 438, 464 }, /* (1760   Hz : 450.56 ) */
        { 875, 928 }, /* (3520   Hz : 901.12 ) */
    },
    { // A#
        { 58 , 61  }, /* (233.082Hz : 59.669 ) */
        { 116, 123 }, /* (466.164Hz : 119.338) */
        { 232, 246 }, /* (932.328Hz : 238.676) */
        { 464, 491 }, /* (1864.66Hz : 477.352) */
        { 928, 983 }, /* (3729.31Hz : 954.703) */
    },
    { // B
        { 61 , 65  }, /* (246.942Hz : 63.2171) */
        { 123, 130 }, /* (493.883Hz : 126.434) */
        { 246, 260 }, /* (987.767Hz : 252.868) */
        { 491, 521 }, /* (1975.53Hz : 505.737) */
        { 983, 1023 }, /* (3951.07Hz : 1011.47) */
    },
};

/*---------------------------------------------------------------------------*
  Name:         SetDrawData

  Description:  ����ł̍ŐV�T���v�����O�ς݃f�[�^��\���ɔ��f������o�b�t�@��
                �i�[����B

  Arguments:    address - �R���|�[�l���g�ɂ���čŐV�̃T���v�����O�f�[�^��
                          �i�[���ꂽ���C����������̈ʒu�B
                          
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetDrawData(void *address)
{
    s32     i;
    u16    *p;

    // �T���v�����O�������s���Ă��Ȃ��ꍇ�͉��������ɏI��
    // (�}�C�N�Ɩ��֌W�̃������̃L���b�V����j�����Ă��܂���)
    if ((address < gMicData) || (address >= (gMicData + SAMPLING_BUFFER_SIZE)))
    {
        return;
    }

    // ���߂� FFT_N �̃T���v�����O�l�ɑ΂��� FFT ��K�p
    // 12bit �T���v�����O�͊e�l��2�o�C�g�ƂȂ�
    p = (u16 *)((u32)address - (FFT_N-1)*2);
    if ((u32)p < (u32)gMicData)
    {
        p = (u16 *)((u32)p + SAMPLING_BUFFER_SIZE);
    }
    DC_InvalidateRange((void *)((u32)p & 0xffffffe0), 32);
    for (i = 0; i < FFT_N; i++)
    {
#ifdef USE_FFTREAL
        data[i] = ((*p) << (FX32_SHIFT - (16 - 12)));
#else
        data[i * 2] = ((*p) << (FX32_SHIFT - (16 - 12)));
        data[i * 2 + 1] = 0; // �����ɂ� 0 ����
#endif
        p++;
        if ((u32)p >= (u32)(gMicData + SAMPLING_BUFFER_SIZE))
        {
            p = (u16 *)((u32)p - SAMPLING_BUFFER_SIZE);
        }
        if (((u32)p % 32) == 0)
        {
            DC_InvalidateRange(p, 32);
        }
    }

#ifdef USE_FFTREAL
    MATH_FFTReal(data, FFT_NSHIFT, sinTable, sinTable2);
#else
    MATH_FFT(data, FFT_NSHIFT, sinTable);
#endif

    // FFT_N/2 ����͂���ȉ����������Ĕ��]�������l���������Ȃ��̂Ōv�Z���Ȃ�
    for (i = 0; i <= FFT_N/2; i++)
    {
        fx32 real;
        fx32 imm;

#ifdef USE_FFTREAL
        if (0 < i  && i < FFT_N/2)
        {
            real = data[i * 2];
            imm  = data[i * 2 + 1];
        }
        else
        {
            // MATH_FFTReal �֐��̌��ʂ̊i�[���@�͓���Ȃ̂ŁA�������������
            if (i == 0)
            {
                real = data[0];
            }
            else // i == FFT_N/2
            {
                real = data[1];
            }
            imm  = 0;
        }
#else
        real = data[i * 2];
        imm  = data[i * 2 + 1];
#endif

        // �e���g���̃p���[���v�Z����
        // �l�̑召�݂̂��K�v�ȏꍇ�� Sqrt ���ȗ����邱�Ƃō��������\ 
        power[i] = FX_Whole(FX_Sqrt(FX_MUL(real, real) + FX_MUL(imm, imm)));

        // �e���g���̃p���[�̎��ԗݐϒl���L�^����
        // ���������������Ă���
        smoothedPower[i] += power[i] - (smoothedPower[i]/8);
    }

    switch (drawMode)
    {
    case DRAWMODE_REALTIME:
        // ���A���^�C���\��

        for (i = 0; i < DRAW_MAX; i++)
        {
            s32     j, index;
            fx32    tmpPower;

            // +1 �̗��R: power[0] �̃f�[�^�͒��������Ȃ̂ŕ\�����Ȃ��Ă���
            index = i * DRAW_STEP + 1;
            tmpPower = 0;
            for (j = 0; j < DRAW_STEP; j++)
            {
                tmpPower += power[index];
                index++;
            }
            SDK_ASSERT(index <= sizeof(power)/sizeof(power[0]));
            tmpPower /= DRAW_STEP /*j*/;

            // ��ʂɎ��܂�悤�ȓK���Ȕ{���������č��W�l�Ƃ���B
            // �\���͏c�����ɐL�΂��̂ŁA192�܂ł����\���ł��Ȃ��B
            gDrawData[i] = (u8)MATH_MIN(tmpPower, 191);
        }

        break;

    case DRAWMODE_BAR:
        // �o�[�\��

        for (i = 0; i < DRAW_MAX; i += blockCount)
        {
            s32     j, index;
            fx32    tmpPower;

            // +1 �̗��R: power[0] �̃f�[�^�͒��������Ȃ̂ŕ\�����Ȃ��Ă���
            index = i * DRAW_STEP + 1;
            tmpPower = 0;
            for (j = 0; j < blockCount * DRAW_STEP && index < sizeof(smoothedPower)/sizeof(smoothedPower[0]); j++)
            {
                tmpPower += smoothedPower[index];
                index++;
            }
            tmpPower /= j;

            // ��ʂɎ��܂�悤�ȓK���Ȕ{���������č��W�l�Ƃ���B
            // �\���͏c�����ɐL�΂��̂ŁA192�܂ł����\���ł��Ȃ��B
            gDrawData[i] = (u8)MATH_MIN(tmpPower/8, 191);
        }
        break;

    case DRAWMODE_SCALE:
        // ���K�\��

        for (i = 0; i < 12; i ++)
        {
            s32     j;

            for (j = 0; j < SCALE_SAMPLING_OCTAVES ; j++)
            {
                u32 k;
                fx32 maxPower;

                // ���K���̎��g���тł̍ő�l
                maxPower = 0;
                for (k = scaleSamplingPoints[i][j][0]; k < scaleSamplingPoints[i][j][1]; k++)
                {
                    fx32 tmpPower;
                    // ���g���̊ԂɃs�[�N������ꍇ���P�A���邽�߁A�אڂ���l�̘a�����
                    // k+1 �� [1, FFT_N/2-1] ������Ȃ��悤�� scaleSamplingPoints ���ݒ肳��Ă��܂�
                    tmpPower = smoothedPower[k] + smoothedPower[k+1];
                    if ( maxPower < tmpPower )
                    {
                        maxPower = tmpPower;
                    }
                }

                // ��ʂɎ��܂�悤�ȓK���Ȕ{���������č��W�l�Ƃ���B
                // �\���͏c�����ɐL�΂��̂ŁA192�܂ł����\���ł��Ȃ��B
                gDrawData[i * SCALE_SAMPLING_OCTAVES + j] = (u8)MATH_MIN(maxPower/16, 191);
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         PrintfVariableData

  Description:  �ςȃT���v�����O�ݒ���v�����g�o�͂���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintfVariableData(void)
{
    s32     range = 0;

    OS_Printf(" sampling-span: %d , bit-range: ", gMicAutoParam.rate);
    switch (gMicAutoParam.type)
    {
    case MIC_SAMPLING_TYPE_8BIT:
        OS_Printf("8");
        break;
    case MIC_SAMPLING_TYPE_12BIT:
        OS_Printf("12");
        break;
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
        OS_Printf("signed 8");
        break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
        OS_Printf("signed 12");
        break;
    case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
        OS_Printf("12(filter off)");
        break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
        OS_Printf("signed 12(filter off)");
        break;
    }
    if (gMicAutoParam.loop_enable)
    {
        OS_Printf(" , loop: on\n");
    }
    else
    {
        OS_Printf(" , loop: off\n");
    }
}


/*---------------------------------------------------------------------------*
  Name:         Play

  Description:  �X�g���[���Đ����Đ�

  Arguments:    strm - �X�g���[���I�u�W�F�N�g
                filename - �X�g���[���Đ�����t�@�C����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Play(StreamInfo * strm)
{
    s32     timerValue;
    u32     alarmPeriod;

    osc.index = 0;

    /* �p�����[�^�ݒ� */
    timerValue = SND_TIMER_CLOCK / STRM_SAMPLE_RATE;
    alarmPeriod = timerValue * (STRM_BUF_PAGESIZE / sizeof(s16)) / 32;

    // �����X�g���[���f�[�^�ǂݍ���
    strm->bufPage = 0;
    MakeStreamData(strm);
    MakeStreamData(strm);

    // �`�����l���ƃA���[�����Z�b�g�A�b�v
    SND_SetupChannelPcm(CHANNEL_NUM,
                        SND_WAVE_FORMAT_PCM16,
                        strmBuf,
                        SND_CHANNEL_LOOP_REPEAT,
                        0,
                        STRM_BUF_SIZE / sizeof(u32),
                        127, SND_CHANNEL_DATASHIFT_NONE, timerValue, 0);
    SND_SetupAlarm(ALARM_NUM, alarmPeriod, alarmPeriod, SoundAlarmHandler, strm);
    SND_StartTimer(1 << CHANNEL_NUM, 0, 1 << ALARM_NUM, 0);
}

/*---------------------------------------------------------------------------*
  Name:         StopStream

  Description:  �X�g���[���Đ����~

  Arguments:    strm - �X�g���[���I�u�W�F�N�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Stop()
{
    SND_StopTimer(1 << CHANNEL_NUM, 0, 1 << ALARM_NUM, 0);
}

/*---------------------------------------------------------------------------*
  Name:         StrmThread

  Description:  �X�g���[���X���b�h

  Arguments:    arg - ���[�U�[�f�[�^�i���g�p�j

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void StrmThread(void * arg)
{
#pragma unused(arg)
    OSMessage message;

    OS_InitMessageQueue(&msgQ, msgBuf, 1);

    while (1)
    {
        (void)OS_ReceiveMessage(&msgQ, &message, OS_MESSAGE_BLOCK);
        (void)MakeStreamData((StreamInfo *) message);
    }
}

/*---------------------------------------------------------------------------*
  Name:         SoundAlarmHandler

  Description:  �A���[���R�[���o�b�N�֐�

  Arguments:    arg - �X�g���[���I�u�W�F�N�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SoundAlarmHandler(void *arg)
{
    (void)OS_SendMessage(&msgQ, (OSMessage)arg, OS_MESSAGE_NOBLOCK);
}

/*---------------------------------------------------------------------------*
  Name:         MakeStreamData

  Description:  �X�g���[���f�[�^�𐶐�

  Arguments:    strm - �X�g���[���I�u�W�F�N�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MakeStreamData(StreamInfo * strm)
{
    u8     *buf;
    int     i;

    // �o�b�t�@�̃y�[�W�ݒ�
    if (strm->bufPage == 0)
    {
        buf = strmBuf;
        strm->bufPage = 1;
    }
    else
    {
        buf = strmBuf + STRM_BUF_PAGESIZE;
        strm->bufPage = 0;
    }

    // �f�[�^����
    for (i = 0; i < STRM_BUF_PAGESIZE / sizeof(s16); i++)
    {
        ((s16 *)buf)[i] = (s16)FX_Whole(FX_Mul32x64c(osc.gain << FX32_SHIFT,
                                                     FX_SinFx64c(FX_Mul32x64c(osc.index,
                                                                              FX64C_TWOPI))));
        osc.index += osc.step;
        osc.index &= FX32_DEC_MASK;
    }
}

/*---------------------------------------------------------------------------*
  Name:         TpRead

  Description:  �^�b�`�p�l����ǂ݂���

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TpRead(TpInformation* tp)
{
    TPData  tp_data;
    TPData  tp_raw;
    int     old;

    old = tp->touch;
    while (TP_RequestRawSampling(&tp_raw) != 0)
    {
    };
    TP_GetCalibratedPoint(&tp_data, &tp_raw);

    tp->touch = tp_data.touch;
    tp->trg = tp->touch & (tp->touch ^ old);
    tp->rls = old & (old ^ tp->touch);

    switch (tp_data.validity)
    {
    case TP_VALIDITY_VALID:
        tp->x = tp_data.x;
        tp->y = tp_data.y;
        break;
    case TP_VALIDITY_INVALID_X:
        tp->y = tp_data.y;
        break;
    case TP_VALIDITY_INVALID_Y:
        tp->x = tp_data.x;
        break;
    case TP_VALIDITY_INVALID_XY:
        break;
    default:
        break;
    }
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
