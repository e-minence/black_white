/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SSP - include
  File:     jpegdec.h

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-07-06#$
  $Rev: 10865 $
  $Author: kitase_hirotake $
 *---------------------------------------------------------------------------*/

#ifndef TWL_SSP_JPEGDEC_H_
#define TWL_SSP_JPEGDEC_H_

#include <twl/ssp/common/ssp_jpeg_type.h>
#include <twl/ssp/ARM9/exifdec.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>


// errorCode��s8�͈̔͂Œ�`���Ă��������B
// �u�f�[�^�������ł��܂���ł����B�v�̌����́A
// �f�[�^�`�����s�����T�|�[�g����Ă��Ȃ��A���邢�̓f�[�^���r���ŏI����Ă��܂��B
typedef enum SSPJpegDecoderErrorCode
{
    SSP_JPEG_DECODER_OK = 0,       // �����B
    SSP_JPEG_DECODER_ERROR_ARGUMENT = (-1),   // option�ȊO�̈����Ɍ�肪����܂��B
    SSP_JPEG_DECODER_ERROR_WORK_ALIGN = (-2),   // pCtx��4�o�C�g�A���C�������g����Ă��܂���B
    SSP_JPEG_DECODER_ERROR_OPTION = (-3),   // option�����Ɍ�肪����܂��B
    SSP_JPEG_DECODER_ERROR_SIGN = (-10),

    SSP_JPEG_DECODER_ERROR_WIDTH_HEIGHT = (-20),  // �����������A�w�肳�ꂽ�ő�l�𒴂��Ă��܂��BpCtx->width�����pCtx->height�Ɏ��T�C�Y�������Ă��܂��B

    SSP_JPEG_DECODER_ERROR_EXIF_0 = (-30),   // �f�[�^�������ł��܂���ł����B
// �T���l�C�������݂��Ȃ��摜�ɑ΂��ăT���l�C���̃f�R�[�h���w�������ꍇ�ɂ��A
// ���̃G���[�R�[�h���Ԃ�\��������܂��B

    SSP_JPEG_DECODER_ERROR_MARKER_COMBINATION = (-50),   // �f�[�^�������ł��܂���ł����B
// �}�[�J�[�̑g�ݍ��킹������������܂���B
// �Ⴆ�΁ASOF�}�[�J�[�ɑΉ�����SOS�}�[�J�[��������Ȃ������ꍇ�A
// ���̃G���[�R�[�h���Ԃ�\��������܂��B

    SSP_JPEG_DECODER_ERROR_SOI = (-60),   // �f�[�^(�}�[�J�[)�������ł��܂���ł����B
    SSP_JPEG_DECODER_ERROR_SOF = (-61),
    SSP_JPEG_DECODER_ERROR_SOF_BLOCK_ID = (-62),
    SSP_JPEG_DECODER_ERROR_DHT = (-63),
    SSP_JPEG_DECODER_ERROR_SOS = (-64),
    SSP_JPEG_DECODER_ERROR_DQT = (-65),
    SSP_JPEG_DECODER_ERROR_DRI = (-66),

    SSP_JPEG_DECODER_ERROR_UNDERRUN_0 = (-90),   // �f�[�^�������ł��܂���ł����B
    SSP_JPEG_DECODER_ERROR_UNDERRUN_1 = (-91),
    SSP_JPEG_DECODER_ERROR_UNDERRUN_2 = (-92),
    SSP_JPEG_DECODER_ERROR_UNDERRUN_3 = (-93),
    SSP_JPEG_DECODER_ERROR_UNDERRUN_4 = (-94),
    SSP_JPEG_DECODER_ERROR_UNDERRUN_5 = (-95),

    SSP_JPEG_DECODER_ERROR_RANGE_0 = (-110),  // �f�[�^�������ł��܂���ł����B
    SSP_JPEG_DECODER_ERROR_RANGE_1 = (-111),
    SSP_JPEG_DECODER_ERROR_RANGE_2 = (-112),
    SSP_JPEG_DECODER_ERROR_RANGE_3 = (-113),
    SSP_JPEG_DECODER_ERROR_RANGE_4 = (-114),
    SSP_JPEG_DECODER_ERROR_RANGE_5 = (-115),

    SSP_JPEG_DECODER_ERROR_HLB_0 = (-120),  // �f�[�^�������ł��܂���ł����B

    SSP_JPEG_DECODER_ERROR_INTERNAL_CTX_SIZE = (-128)  // �����G���[�B�ʏ�͔������܂���B
} SSPJpegDecoderErrorCode;

// Fast�łɂ́A�u�R���e�L�X�g�\���́v�ƌĂԍ�Ɨp���������K�v�ł��B
// �֐��Ăяo���O��̏������͕s�v�ł��B�֐��I����ɂ̓��C�u��������g�p����܂���B
// �R���e�L�X�g�\���̂̓T�C�Y��8KBytes�Ƒ傫���̂ŁA
// ���[�J���ϐ��̂悤�ɃX�^�b�N���犄�蓖�ĂȂ��ł��������B

// �R���e�L�X�g�\���́B
typedef struct SSPJpegDecoderFastContext {
    u8*     pSrc;
    u32     inputDataSize;
    void*   pDst;
    u32     option;
    u16     maxWidth;       // ���e�ő�摜��(65536����)�BSSP_StartJpegDecoderFast()�̈���maxWidth���R�s�[����܂��B
    u16     maxHeight;      // ���e�ő�摜����(65536����)�BSSP_StartJpegDecoderFast()�̈���maxHeight���R�s�[����܂��B
    u16     width;          // ���ۂ̉摜���B
    u16     height;         // ���ۂ̉摜�����B
    SSPJpegDecoderErrorCode errorCode; // 0�Ȃ�G���[�����B
    u8      reserved[4];
    u32     work[0x7f8];
} SSPJpegDecoderFastContext;

/*---------------------------------------------------------------------------*
  Name:         SSP_CheckJpegDecoderSign

  Description:  JPEG�t�@�C���̏����`�F�b�N�݂̂��s��

  Arguments:    data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y

  Returns:      �����Ȃ��TRUE��Ԃ�
 *---------------------------------------------------------------------------*/
BOOL SSP_CheckJpegDecoderSign(u8* data, u32 size);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegDecoder

  Description:  JPEG�t�@�C�����f�R�[�h����

  Arguments:    data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y
                dst    : �f�R�[�h��o�b�t�@ (����RGB555�̂�)
                width  : �����Ƃ��ĉ摜���T�C�Y�̍ő�l��^���܂��B
                         �f�R�[�h�������ɂ̓f�R�[�h�����摜�̉��T�C�Y���Ԃ�܂��B
                height : �����Ƃ��ĉ摜�c�T�C�Y�̍ő�l��^���܂��B
                         �f�R�[�h�������ɂ̓f�R�[�h�����摜�̏c�T�C�Y���Ԃ�܂��B
                option : �I�v�V����(�r�b�g�_���a)
                         SSP_JPEG_RGB555: RGB555�`���Ńf�R�[�h�B
                                ���͎w�肵�Ȃ��Ă�RGB555�`���Ńf�R�[�h���܂��B
                         SSP_JPEG_THUMBNAIL: �T���l�C���̃f�R�[�h�B���w�莞�͎�摜�̃f�R�[�h�B
                         SSP_JPEG_EXIF: EXIF���̂ݎ擾����

  Returns:      �����Ȃ��TRUE��Ԃ�
 *---------------------------------------------------------------------------*/
BOOL SSP_StartJpegDecoder(u8* data, u32 size, void* dst, s16* width, s16* height, u32 option);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegDecoderEx

  Description:  �����@�\�̗L�����w�肵��JPEG�t�@�C�����f�R�[�h����

  Arguments:    data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y
                dst    : �f�R�[�h��o�b�t�@ (����RGB555�̂�)
                width  : �����Ƃ��ĉ摜���T�C�Y�̍ő�l��^���܂��B
                         �f�R�[�h�������ɂ̓f�R�[�h�����摜�̉��T�C�Y���Ԃ�܂��B
                height : �����Ƃ��ĉ摜�c�T�C�Y�̍ő�l��^���܂��B
                         �f�R�[�h�������ɂ̓f�R�[�h�����摜�̏c�T�C�Y���Ԃ�܂��B
                option : �I�v�V����(�r�b�g�_���a)
                         SSP_JPEG_RGB555: RGB555�`���Ńf�R�[�h�B
                                ���͎w�肵�Ȃ��Ă�RGB555�`���Ńf�R�[�h���܂��B
                         SSP_JPEG_THUMBNAIL: �T���l�C���̃f�R�[�h�B���w�莞�͎�摜�̃f�R�[�h�B
                         SSP_JPEG_EXIF: EXIF���̂ݎ擾����
                sign   : �����`�F�b�N���s���ꍇ�� TRUE ���w�肷��B

  Returns:      �����Ȃ��TRUE��Ԃ�
 *---------------------------------------------------------------------------*/
static inline BOOL SSP_StartJpegDecoderEx(u8* data, u32 size, void* dst, s16* width, s16* height, u32 option, BOOL sign)
{
    BOOL result;
    BOOL old_sign;
    
    old_sign = SSP_SetJpegDecoderSignMode(sign);
    result = SSP_StartJpegDecoder(data, size, dst, width, height, option);
    (void)SSP_SetJpegDecoderSignMode(old_sign);
    
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SSP_ExtractJpegDecoderExif

  Description:  JPEG�t�@�C������Exif��񂾂��𒊏o����
                ���ۂɒ��o���ꂽ�l�͊e�� Get �֐��Ŏ擾���邱��

  Arguments:    data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y
                
  Returns:      �����Ȃ��TRUE��Ԃ�
 *---------------------------------------------------------------------------*/
static inline BOOL SSP_ExtractJpegDecoderExif(u8* data, u32 size)
{
    return SSP_StartJpegDecoder(data, size, 0, 0, 0, SSP_JPEG_EXIF);
}

 /*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegDecoderFast

  Description:  JPEG�t�@�C�����f�R�[�h���� (Fast��)

                �摜�T�C�Y��8�s�N�Z�����邢��16�s�N�Z���̔{��(�f�[�^�`���ɂ��)
                �ł���ꍇ�A�ł������Ƀf�R�[�h�ł��܂��B

  Arguments:    pCtx   : �R���e�L�X�g�\���̂ւ̃|�C���^�B
                         �֐��Ăяo���O��̏������͕s�v�ł��B�֐��I����ɂ̓��C�u��������g�p����܂���B
                         �R���e�L�X�g�\���̂̓T�C�Y���傫���̂ŁA���[�J���ϐ��̂悤�ɃX�^�b�N���犄�蓖�ĂȂ��ł��������B
                         �����_�ł͕����̃X���b�h���瓯���ɌĂяo�����Ƃ͂ł��܂���B
                data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y
                dst    : �f�R�[�h��o�b�t�@ (����RGB555�̂�)
                         �o�͌`���ɂ�����炸�A4�o�C�g�A���C�������g���K�v�ł��B
                maxWidth  : �����Ƃ��ĉ摜���T�C�Y�̍ő�l��^���܂��B(65535�܂�)
                            �f�R�[�h�������ɂ�pCtx->width�ɁA�f�R�[�h�����摜�̉��T�C�Y���Ԃ�܂��B
                maxHeight : �����Ƃ��ĉ摜�c�T�C�Y�̍ő�l��^���܂��B(65535�܂�)
                            �f�R�[�h�������ɂ�pCtx->height�ɁA�f�R�[�h�����摜�̏c�T�C�Y���Ԃ�܂��B
                option : �I�v�V����(�r�b�g�_���a)
                         SSP_JPEG_RGB555: RGB555�`���Ńf�R�[�h�B
                                ����RGB555�݂̂ł����A�w��͏ȗ��ł��܂���B
                         SSP_JPEG_THUMBNAIL: �T���l�C���̃f�R�[�h�B���w�莞�͎�摜�̃f�R�[�h�B

  Returns:      �����Ȃ��TRUE��Ԃ�
                ���s�����ꍇ�ApCtx->errorCode�ɏڍׂȃG���[�R�[�h���i�[����܂��B
 *---------------------------------------------------------------------------*/
BOOL SSP_StartJpegDecoderFast(SSPJpegDecoderFastContext* pCtx, u8* data, u32 size, void* dst, u32 maxWidth, u32 maxHeight, u32 option);

 /*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegDecoderFastEx

  Description:  �����@�\�̗L�����w�肵��JPEG�t�@�C�����f�R�[�h���� (Fast��)

                �摜�T�C�Y��8�s�N�Z�����邢��16�s�N�Z���̔{��(�f�[�^�`���ɂ��)
                �ł���ꍇ�A�ł������Ƀf�R�[�h�ł��܂��B

  Arguments:    pCtx   : �R���e�L�X�g�\���̂ւ̃|�C���^�B
                         �֐��Ăяo���O��̏������͕s�v�ł��B�֐��I����ɂ̓��C�u��������g�p����܂���B
                         �R���e�L�X�g�\���̂̓T�C�Y���傫���̂ŁA���[�J���ϐ��̂悤�ɃX�^�b�N���犄�蓖�ĂȂ��ł��������B
                         �����_�ł͕����̃X���b�h���瓯���ɌĂяo�����Ƃ͂ł��܂���B
                data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y
                dst    : �f�R�[�h��o�b�t�@ (����RGB555�̂�)
                         �o�͌`���ɂ�����炸�A4�o�C�g�A���C�������g���K�v�ł��B
                maxWidth  : �����Ƃ��ĉ摜���T�C�Y�̍ő�l��^���܂��B(65535�܂�)
                            �f�R�[�h�������ɂ�pCtx->width�ɁA�f�R�[�h�����摜�̉��T�C�Y���Ԃ�܂��B
                maxHeight : �����Ƃ��ĉ摜�c�T�C�Y�̍ő�l��^���܂��B(65535�܂�)
                            �f�R�[�h�������ɂ�pCtx->height�ɁA�f�R�[�h�����摜�̏c�T�C�Y���Ԃ�܂��B
                option : �I�v�V����(�r�b�g�_���a)
                         SSP_JPEG_RGB555: RGB555�`���Ńf�R�[�h�B
                                ����RGB555�݂̂ł����A�w��͏ȗ��ł��܂���B
                         SSP_JPEG_THUMBNAIL: �T���l�C���̃f�R�[�h�B���w�莞�͎�摜�̃f�R�[�h�B
                sign   : �����`�F�b�N���s���ꍇ�� TRUE ���w�肷��B

  Returns:      �����Ȃ��TRUE��Ԃ�
                ���s�����ꍇ�ApCtx->errorCode�ɏڍׂȃG���[�R�[�h���i�[����܂��B
 *---------------------------------------------------------------------------*/
BOOL SSP_StartJpegDecoderFastEx(SSPJpegDecoderFastContext* pCtx, u8* data, u32 size, void* dst, u32 maxWidth, u32 maxHeight, u32 option, BOOL sign);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* TWL_SSP_JPEGDEC_H_ */
#endif
