/*---------------------------------------------------------------------------*
  Project:  TwlSDK - dsp - 
  File:     dsp_graphics.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-02#$
  $Rev: 8184 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/

#ifndef TWL_DSP_GRAPHICS_H_
#define TWL_DSP_GRAPHICS_H_

#include <twl/dsp/common/pipe.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/* constants */
typedef DSPWord DSPGraphicsScalingMode;
#define DSP_GRAPHICS_SCALING_MODE_N_NEIGHBOR (DSPGraphicsScalingMode)0x0001
#define DSP_GRAPHICS_SCALING_MODE_BILINEAR   (DSPGraphicsScalingMode)0x0002
#define DSP_GRAPHICS_SCALING_MODE_BICUBIC    (DSPGraphicsScalingMode)0x0003

#define DSP_GRAPHICS_SCALING_MODE_NPARTSHRINK (DSPGraphicsScalingMode)0x000A

// �g�p���郌�W�X�^
#define DSP_GRAPHICS_COM_REGISTER 0    // �������e�ʒm�p���W�X�^�ԍ�
#define DSP_GRAPHICS_REP_REGISTER 1    // �������ʒʒm�p���W�X�^�ԍ�

/*---------------------------------------------------------------------------*/
/* DSP <-> ARM �ԂŎg���\���́A�񋓌^ */

// �Z�}�t�H���W�X�^�ł��Ƃ肷��l
typedef enum DspState
{
    DSP_STATE_FAIL        = 0x0000,
    DSP_STATE_SUCCESS     = 0x0001
//    DSP_STATE_END_ONEPROC = 0x0002
} DSPSTATE;

// ARM ������w�肳���ADSP�����s���鏈����\���l
typedef enum _GraphicsFuncID
{
    DSP_G_FUNCID_SCALING = 0x0001,
    DSP_G_FUNCID_YUV2RGB,
    
    DSP_FUNCID_NUM
} DSPGraphicsFuncID;

// YUV <-> RGB �ϊ��ɗp����f�[�^�\����
typedef struct _Yuv2RgbParam
{
    u32 size;
    u32 src;
    u32 dst;
} DSPYuv2RgbParam;

// Scaling �ɗp����f�[�^�\����
typedef struct _ScalingParam
{
    u32 src;
    u32 dst;
    u16 mode;
    u16 img_width;
    u16 img_height;
    u16 rate_w;
    u16 rate_h;
    u16 block_x;
    u16 block_y;
    u16 width;
    u16 height;
    u16 pad[1];    // _ScalingParam �̃T�C�Y�� 4�̔{���ɑ����邽��
} DSPScalingParam;

typedef void (*DSP_GraphicsCallback)(void);  // �����I�����ɌĂяo���A���[�U�w��̃R�[���o�b�N�֐�

#ifdef SDK_TWL
/*---------------------------------------------------------------------------*
    �}�N����`
*---------------------------------------------------------------------------*/
// DSP_Scaling* �ŏo�͂����T�C�Y���v�Z����}�N��
#define DSP_CALC_SCALING_SIZE(value, ratio) ((u32)(value * (u32)(ratio * 1000) / 1000))

#define DSP_CALC_SCALING_SIZE_FX(value, ratio) ((u32)( value * (u32)(ratio * 1000 / 4096.0f) / 1000))

/*---------------------------------------------------------------------------*
    �����ϐ���`
*---------------------------------------------------------------------------*/
static DSP_GraphicsCallback callBackFunc;   // �񓯊������I�����ɌĂ΂��R�[���o�b�N�֐�

static volatile BOOL isBusy;                         // DSP �����炩�̏��������s���Ȃ�� TRUE
static volatile BOOL isAsync;                        // DSP �����炩�̔񓯊����������s���Ȃ�� TRUE

/*---------------------------------------------------------------------------*
    internal core functions placed on LTDMAIN.
*---------------------------------------------------------------------------*/
void DSPi_OpenStaticComponentGraphicsCore(FSFile *file);
BOOL DSPi_LoadGraphicsCore(FSFile *file, int slotB, int slotC);
void DSPi_UnloadGraphicsCore(void);
BOOL DSPi_ConvertYuvToRgbCore(const void* src, void* dst, u32 size, DSP_GraphicsCallback callback, BOOL async);
BOOL DSPi_ScalingCore(const void* src, void* dst, u16 img_width, u16 img_height, f32 rw, f32 ry, DSPGraphicsScalingMode mode,
                      u16 x, u16 y, u16 width, u16 height, DSP_GraphicsCallback callback, BOOL async);
BOOL DSPi_ScalingFxCore(const void* src, void* dst, u16 img_width, u16 img_height, fx32 rw, fx32 ry, DSPGraphicsScalingMode mode,
                      u16 x, u16 y, u16 width, u16 height, DSP_GraphicsCallback callback, BOOL async);


/*---------------------------------------------------------------------------*
  Name:         DSP_OpenStaticComponentGraphics

  Description:  �O���t�B�b�N�X�R���|�[�l���g�p�̃������t�@�C�����J���B
                �t�@�C���V�X�e���Ɏ��O�ɗp�ӂ��Ă����K�v���Ȃ��Ȃ邪
                �ÓI�������Ƃ��ă����N����邽�߃v���O�����T�C�Y����������B
  
  Arguments:    file : �������t�@�C�����J��FSFile�\���́B
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void DSP_OpenStaticComponentGraphics(FSFile *file)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_OpenStaticComponentGraphicsCore(file);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadGraphics

  Description:  DSP�̃O���t�B�b�N�X�R���|�[�l���g�����[�h����B

  Arguments:    file  : �O���t�B�b�N�X�R���|�[�l���g�̃t�@�C���B
                slotB : �R�[�h�������̂��߂Ɏg�p�������ꂽWRAM-B�B
                slotC : �f�[�^�������̂��߂Ɏg�p�������ꂽWRAM-C�B
  
  Returns:      �O���t�B�b�N�X�R���|�[�l���g��������DSP�փ��[�h������TRUE�B
 *---------------------------------------------------------------------------*/
static inline BOOL DSP_LoadGraphics(FSFile *file, int slotB, int slotC)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return DSPi_LoadGraphicsCore(file, slotB, slotC);
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_UnloadGraphics

  Description:  DSP�̃O���t�B�b�N�X�R���|�[�l���g���I������B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
static inline void DSP_UnloadGraphics(void)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_UnloadGraphicsCore();
    }
}

//--------------------------------------------------------------------------------
//    YUV -> RGB convert
//
/*---------------------------------------------------------------------------*
  Name:         DSPi_YuvToRgbConvert[Async]

  Description:  YUV->RGB �ϊ����s���܂�

  Arguments:    src       : �������f�[�^�A�h���X
                dest      : ������f�[�^�i�[��A�h���X
                size      : src �f�[�^�T�C�Y
     Async �̂�[callback  : �ϊ��I����Ɏ��s�����R�[���o�b�N�֐��̃|�C���^]

  Returns:      �����Ȃ�TRUE
 *---------------------------------------------------------------------------*/
static inline BOOL DSP_ConvertYuvToRgb(const void* src, void* dst, u32 size)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ConvertYuvToRgbCore(src, dst, size, NULL, FALSE);
    }
    return FALSE;
}

static inline BOOL DSP_ConvertYuvToRgbAsync(const void* src, void* dst, u32 size, DSP_GraphicsCallback callback)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ConvertYuvToRgbCore(src, dst, size, callback, TRUE);
    }
    return FALSE;
}

//--------------------------------------------------------------------------------
//    
//

//--------------------------------------------------------------------------------
//    Scaling
//

/*---------------------------------------------------------------------------*
  Name:         DSP_Scaling[Fx][Async][Ex]

  Description:  �摜�f�[�^�̊g��i�k���j���������s
                *Async �ł́A�񓯊��ŏ��������s���邱�Ƃ��\�B�I�����ɂ͓o�^�����R�[���o�b�N���Ă΂��B
                *Ex �ł́A�摜�̔C�ӂ̗̈���w�肷�邱�Ƃ��\�B

  Arguments:    src       : �����Ώۂ̉摜�f�[�^�ւ̃|�C���^�i2 �o�C�g�A���C�������g�j
                dst       : �������ʂ̊i�[��|�C���^�i2 �o�C�g�A���C�������g�A������̃f�[�^�T�C�Y���m�ۂ��Ă��邱�Ɓj
                img_width : src �̉��T�C�Y
                img_height: src �̏c�T�C�Y
                rx        : �������̔{��(31�`0.001 �����_��4�ʈȉ��͐؂�̂�) : [ Fx�� �����l ]
                ry        : �c�����̔{��(31�`0.001 �����_��4�ʈȉ��͐؂�̂�) : [ Fx�� �����l ]
                mode      : ��ԕ��@ (nearest neighbor, bilinear, bicubic)
       Ex �̂�[ x         : �����Ώۗ̈�̍���� x ���W  ]
       Ex �̂�[ y         : �����Ώۗ̈�̍���� y ���W  ]
       Ex �̂�[ width     : �����Ώۗ̈�̉��T�C�Y       ]
       Ex �̂�[ height    : �����Ώۗ̈�̏c�T�C�Y       ]
    Async �̂�[ callback  : �����I�����̃R�[���o�b�N�֐� ]

  Returns:      �����Ȃ�� TRUE ��Ԃ�
 *---------------------------------------------------------------------------*/
static inline BOOL DSP_Scaling(const void* src, void* dst, u16 img_width, u16 img_height, f32 rx, f32 ry, DSPGraphicsScalingMode mode)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ScalingCore(src, dst, img_width, img_height, rx, ry, mode, 0, 0, img_width, img_height, NULL, FALSE);
    }
    return FALSE;
}

static inline BOOL DSP_ScalingFx(const void* src, void* dst, u16 img_width, u16 img_height, fx32 rx, fx32 ry, DSPGraphicsScalingMode mode)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ScalingFxCore(src, dst, img_width, img_height, rx, ry, mode, 0, 0, img_width, img_height, NULL, FALSE);
    }
    return FALSE;
}

static inline BOOL DSP_ScalingAsync(const void* src, void* dst, u16 img_width, u16 img_height, f32 rx, f32 ry,
                             DSPGraphicsScalingMode mode, DSP_GraphicsCallback callback)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ScalingCore(src, dst, img_width, img_height, rx, ry, mode, 0, 0, img_width, img_height, callback, TRUE);
    }
    return FALSE;
}

static inline BOOL DSP_ScalingFxAsync(const void* src, void* dst, u16 img_width, u16 img_height, fx32 rx, fx32 ry,
                             DSPGraphicsScalingMode mode, DSP_GraphicsCallback callback)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ScalingFxCore(src, dst, img_width, img_height, rx, ry, mode, 0, 0, img_width, img_height, callback, TRUE);
    }
    return FALSE;
}

static inline BOOL DSP_ScalingEx(const void* src, void* dst, u16 img_width, u16 img_height, f32 rx, f32 ry, DSPGraphicsScalingMode mode,
                                u16 x, u16 y, u16 width, u16 height)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ScalingCore(src, dst, img_width, img_height, rx, ry, mode, x, y, width, height, NULL, FALSE);
    }
    return FALSE;
}

static inline BOOL DSP_ScalingFxEx(const void* src, void* dst, u16 img_width, u16 img_height, fx32 rx, fx32 ry, DSPGraphicsScalingMode mode,
                                u16 x, u16 y, u16 width, u16 height)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ScalingFxCore(src, dst, img_width, img_height, rx, ry, mode, x, y, width, height, NULL, FALSE);
    }
    return FALSE;
}

static inline BOOL DSP_ScalingAsyncEx(const void* src, void* dst, u16 img_width, u16 img_height, f32 rx, f32 ry, DSPGraphicsScalingMode mode,
                                u16 x, u16 y, u16 width, u16 height, DSP_GraphicsCallback callback)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ScalingCore(src, dst, img_width, img_height, rx, ry, mode, x, y, width, height, callback, TRUE);
    }
    return FALSE;
}

static inline BOOL DSP_ScalingFxAsyncEx(const void* src, void* dst, u16 img_width, u16 img_height, fx32 rx, fx32 ry, DSPGraphicsScalingMode mode,
                                  u16 x, u16 y, u16 width, u16 height, DSP_GraphicsCallback callback)
{
    if (OS_IsRunOnTwl() && !isBusy)
    {
        return DSPi_ScalingFxCore(src, dst, img_width, img_height, rx, ry, mode, x, y, width, height, callback, TRUE);
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_CalcScalingFactor[Fx]

  Description:  ���͒l�Əo�͒l����ADSP_Scaling �Ɏw�肷��{�����t�Z����֐��B
                0.001 �ȉ��Ő؂�̂Ă��Ă��܂��l�ɂ��Ă̌덷�C���͓���Ă��邪�A
                �{���̐����A���S�ɂǂ�ȏo�̓T�C�Y�ł��Z�o�ł���{���𓱂��o���邩�͖����؂ł��B

  Arguments:    src_size : ���̓T�C�Y
                dst_size : �o�̓T�C�Y

  Returns:      dst_size �𓱂��{���if32 or fx32)��Ԃ��B
 *---------------------------------------------------------------------------*/
static inline f32 DSP_CalcScalingFactorF32(const u16 src_size, const u16 dst_size)
{
    // DSP_Scaling �̈����ƂȂ�{���̓��͐��� �`0.001 �ɂ��덷���C�����邽�߂� 0.0009f ��������
    return (dst_size / (f32)src_size + 0.0009f);
}

static inline fx32 DSP_CalcScalingFactorFx32(const u16 src_size, const u16 dst_size)
{
    return FX_F32_TO_FX32(dst_size / (f32)src_size + 0.0009f);
}

/*===========================================================================*/

#endif    // SDK_TWL

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_GRAPHICS_H_ */
