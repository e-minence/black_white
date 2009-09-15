/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SSP - include
  File:     jpegenc.h

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

#ifndef TWL_SSP_JPEGENC_H_
#define TWL_SSP_JPEGENC_H_

#include <twl/ssp/common/ssp_jpeg_type.h>
#include <twl/ssp/ARM9/exifenc.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

/*---------------------------------------------------------------------------*
  Name:         SSP_GetJpegEncoderBufferSize

  Description:  �G���R�[�h�ɕK�v�ȃ��[�N�T�C�Y��Ԃ�

  Arguments:    width    : �摜�̉���
                height   : �摜�̏c��
                sampling : �T���v�����O(1�܂���2)
                option   : �I�v�V����(�r�b�g�_���a)
                           SSP_JPEG_RGB555: RGB555�`������G���R�[�h�B
                           SSP_JPEG_YUV422: YUV422�`������G���R�[�h�B
                                  �`���w�肪�����ꍇ��RGB555�`���Ƃ݂Ȃ��܂��B
                           SSP_JPEG_THUMBNAIL: �T���l�C���t���G���R�[�h�B

  Returns:      �K�v�ȃ������T�C�Y
 *---------------------------------------------------------------------------*/
u32 SSP_GetJpegEncoderBufferSize(u32 width, u32 height, u32 sampling, u32 option);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegEncoder

  Description:  JPEG�G���R�[�h����
                �T���v�����O���P�̏ꍇ�Awidth,height�͂W�̔{��
                �T���v�����O���Q�̏ꍇ�Awidth,height��16�̔{���łȂ��Ƃ����Ȃ�
                src, dst, wrk�ɂ�4�o�C�g�A���C�������g���K�v�ł��B

  Arguments:    src    :     �摜�f�[�^(���ォ��E���ւ�RGB555/YUV422�f�[�^)
                dst   :      �G���R�[�h���ꂽJPEG�f�[�^������ꏊ
                limit :      dst�̌��E�T�C�Y(����𒴂���ƃG���R�[�h���s)
                wrk :        ���[�N�G���A
                width :      �摜�̉���
                height :     �摜�̏c��
                quality :    �G���R�[�h�̃N�I���e�B(1-100)
                sampling :   ��摜�̃T���v�����O(1=YUV444,2=YUV420,3=YUV422�B�T���l�C���͓����I��3�Œ�)
                option : �I�v�V����(�r�b�g�_���a)
                           SSP_JPEG_RGB555: RGB555�`������G���R�[�h�B
                           SSP_JPEG_YUV422: YUV422�`������G���R�[�h�B
                                  �`���w�肪�����ꍇ��RGB555�`���Ƃ݂Ȃ��܂��B
                           SSP_JPEG_THUMBNAIL: �T���l�C���t���G���R�[�h�B

  Returns:      �G���R�[�h���ꂽJPEG�̃T�C�Y(0�̏ꍇ�̓G���R�[�h���s)
 *---------------------------------------------------------------------------*/
u32 SSP_StartJpegEncoder(const void* src, u8 *dst, u32 limit, u8 *wrk,
                         u32 width, u32 height,
                         u32 quality, u32 sampling, u32 option);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegEncoderEx

  Description:  �����@�\�̗L�����w�肵��JPEG�G���R�[�h����
                �T���v�����O���P�̏ꍇ�Awidth,height�͂W�̔{��
                �T���v�����O���Q�̏ꍇ�Awidth,height��16�̔{���łȂ��Ƃ����Ȃ�
                src, dst, wrk�ɂ�4�o�C�g�A���C�������g���K�v�ł��B

  Arguments:    src    :     �摜�f�[�^(���ォ��E���ւ�RGB555/YUV422�f�[�^)
                dst   :      �G���R�[�h���ꂽJPEG�f�[�^������ꏊ
                limit :      dst�̌��E�T�C�Y(����𒴂���ƃG���R�[�h���s)
                wrk :        ���[�N�G���A
                width :      �摜�̉���
                height :     �摜�̏c��
                quality :    �G���R�[�h�̃N�I���e�B(1-100)
                sampling :   ��摜�̃T���v�����O(1=YUV444,2=YUV420,3=YUV422�B�T���l�C���͓����I��3�Œ�)
                option : �I�v�V����(�r�b�g�_���a)
                           SSP_JPEG_RGB555: RGB555�`������G���R�[�h�B
                           SSP_JPEG_YUV422: YUV422�`������G���R�[�h�B
                                  �`���w�肪�����ꍇ��RGB555�`���Ƃ݂Ȃ��܂��B
                           SSP_JPEG_THUMBNAIL: �T���l�C���t���G���R�[�h�B
                sign :       �����t�����s���ꍇ�� TRUE ���w�肷��B

  Returns:      �G���R�[�h���ꂽJPEG�̃T�C�Y(0�̏ꍇ�̓G���R�[�h���s)
 *---------------------------------------------------------------------------*/
static inline u32 SSP_StartJpegEncoderEx(const void* src, u8 *dst, u32 limit, u8 *wrk,
                         u32 width, u32 height,
                         u32 quality, u32 sampling, u32 option, BOOL sign)
{
    u32 result;
    BOOL old_sign;
    
    old_sign = SSP_SetJpegEncoderSignMode(sign);
    result = SSP_StartJpegEncoder(src, dst, limit, wrk, width, height, quality, sampling, option);
    (void)SSP_SetJpegEncoderSignMode(old_sign);
    
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SSP_ConvertJpegEncodeData

  Description:  JPEG�G���R�[�h����`���ɓ��̓f�[�^���R���o�[�g����B
                �{�֐��̏I����ɓ��͌��f�[�^�̃N���A���\�ɂȂ�܂��B
                �T���v�����O���P�̏ꍇ�Awidth,height�͂W�̔{��
                �T���v�����O���Q�̏ꍇ�Awidth,height��16�̔{���łȂ��Ƃ����Ȃ�
                src, dst, wrk�ɂ�4�o�C�g�A���C�������g���K�v�ł��B

  Arguments:    src    :     �摜�f�[�^(���ォ��E���ւ�RGB555/YUV422�f�[�^)
                wrk :        ���[�N�G���A
                width :      �摜�̉���
                height :     �摜�̏c��
                sampling :   ��摜�̃T���v�����O(1=YUV444,2=YUV420,3=YUV422�B�T���l�C���͓����I��3�Œ�)
                option : �I�v�V����(�r�b�g�_���a)
                           SSP_JPEG_RGB555: RGB555�`������G���R�[�h�B
                           SSP_JPEG_YUV422: YUV422�`������G���R�[�h�B
                                  �`���w�肪�����ꍇ��RGB555�`���Ƃ݂Ȃ��܂��B
                           SSP_JPEG_THUMBNAIL: �T���l�C���t���G���R�[�h�B

  Returns:      �����Ȃ��TRUE
 *---------------------------------------------------------------------------*/
BOOL SSP_ConvertJpegEncodeData(const void* src, u8 *wrk,
                         u32 width, u32 height, u32 sampling, u32 option);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegEncoderWithEncodeData

  Description:  ���O�ɃR���o�[�g�����G���R�[�h�p�f�[�^����JPEG�G���R�[�h����
                �{�֐��̌Ăяo���O��SSP_ConvertJpegEncodeData�֐����Ăяo���K�v������܂��B
                �T���v�����O���P�̏ꍇ�Awidth,height�͂W�̔{��
                �T���v�����O���Q�̏ꍇ�Awidth,height��16�̔{���łȂ��Ƃ����Ȃ�
                dst, wrk�ɂ�4�o�C�g�A���C�������g���K�v�ł��B

  Arguments:    dst   :      �G���R�[�h���ꂽJPEG�f�[�^������ꏊ
                limit :      dst�̌��E�T�C�Y(����𒴂���ƃG���R�[�h���s)
                wrk :        ���[�N�G���A
                width :      �摜�̉���
                height :     �摜�̏c��
                quality :    �G���R�[�h�̃N�I���e�B(1-100)
                sampling :   ��摜�̃T���v�����O(1=YUV444,2=YUV420,3=YUV422�B�T���l�C���͓����I��3�Œ�)
                option : �I�v�V����(�r�b�g�_���a)
                           SSP_JPEG_RGB555: RGB555�`������G���R�[�h�B
                           SSP_JPEG_YUV422: YUV422�`������G���R�[�h�B
                                  �`���w�肪�����ꍇ��RGB555�`���Ƃ݂Ȃ��܂��B
                           SSP_JPEG_THUMBNAIL: �T���l�C���t���G���R�[�h�B

  Returns:      �G���R�[�h���ꂽJPEG�̃T�C�Y(0�̏ꍇ�̓G���R�[�h���s)
 *---------------------------------------------------------------------------*/
u32 SSP_StartJpegEncoderWithEncodeData(u8 *dst, u32 limit, u8 *wrk,
                         u32 width, u32 height,
                         u32 quality, u32 sampling, u32 option);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegEncoderWithEncodeDataEx

  Description:  ���O�ɃR���o�[�g�����G���R�[�h�p�f�[�^���珐���@�\�̗L�����w�肵��JPEG�G���R�[�h����
                �{�֐��̌Ăяo���O��SSP_ConvertJpegEncodeData�֐����Ăяo���K�v������܂��B
                �T���v�����O���P�̏ꍇ�Awidth,height�͂W�̔{��
                �T���v�����O���Q�̏ꍇ�Awidth,height��16�̔{���łȂ��Ƃ����Ȃ�
                dst, wrk�ɂ�4�o�C�g�A���C�������g���K�v�ł��B

  Arguments:    dst   :      �G���R�[�h���ꂽJPEG�f�[�^������ꏊ
                limit :      dst�̌��E�T�C�Y(����𒴂���ƃG���R�[�h���s)
                wrk :        ���[�N�G���A
                width :      �摜�̉���
                height :     �摜�̏c��
                quality :    �G���R�[�h�̃N�I���e�B(1-100)
                sampling :   ��摜�̃T���v�����O(1=YUV444,2=YUV420,3=YUV422�B�T���l�C���͓����I��3�Œ�)
                option : �I�v�V����(�r�b�g�_���a)
                           SSP_JPEG_RGB555: RGB555�`������G���R�[�h�B
                           SSP_JPEG_YUV422: YUV422�`������G���R�[�h�B
                                  �`���w�肪�����ꍇ��RGB555�`���Ƃ݂Ȃ��܂��B
                           SSP_JPEG_THUMBNAIL: �T���l�C���t���G���R�[�h�B
                sign :       �����t�����s���ꍇ�� TRUE ���w�肷��B

  Returns:      �G���R�[�h���ꂽJPEG�̃T�C�Y(0�̏ꍇ�̓G���R�[�h���s)
 *---------------------------------------------------------------------------*/
static inline u32 SSP_StartJpegEncoderWithEncodeDataEx(u8 *dst, u32 limit, u8 *wrk,
                         u32 width, u32 height,
                         u32 quality, u32 sampling, u32 option, BOOL sign)
{
    u32 result;
    BOOL old_sign;
    
    old_sign = SSP_SetJpegEncoderSignMode(sign);
    result = SSP_StartJpegEncoderWithEncodeData(dst, limit, wrk, width, height, quality, sampling, option);
    (void)SSP_SetJpegEncoderSignMode(old_sign);
    
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SSP_GetJpegEncoderLiteBufferSize

  Description:  �G���R�[�h�ɕK�v�ȃ��[�N�T�C�Y��Ԃ� (�ȃ�������)
                SSP_StartJpegEncoderLite�p�̃��[�N�T�C�Y�ł��B

  Arguments:    option   : �I�v�V����(�r�b�g�_���a)
                           SSP_JPEG_RGB555: RGB555�`������G���R�[�h�B
                           SSP_JPEG_YUV422: YUV422�`������G���R�[�h�B
                                  �`���w�肪�����ꍇ��RGB555�`���Ƃ݂Ȃ��܂��B
                           SSP_JPEG_THUMBNAIL: �T���l�C���t���G���R�[�h�B

  Returns:      �K�v�ȃ������T�C�Y
 *---------------------------------------------------------------------------*/
u32 SSP_GetJpegEncoderLiteBufferSize(u32 option);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegEncoderLite

  Description:  JPEG�G���R�[�h���� (�ȃ�������)
                ��SSP_StartJpegEncoderEx�Ƃ͖��֌W�B
                �T���v�����O���P�̏ꍇ�Awidth,height�͂W�̔{��
                �T���v�����O���Q�̏ꍇ�Awidth,height��16�̔{���łȂ��Ƃ����Ȃ�
                �T���v�����O���R�̏ꍇ�Awidth��16�̔{��,height�͂W�̔{���łȂ��Ƃ����Ȃ�
                src, dst, wrk�ɂ�4�o�C�g�A���C�������g���K�v�ł��B

  Arguments:    src    :     �摜�f�[�^(���ォ��E���ւ�RGB555/YUV422�f�[�^)
                dst   :      �G���R�[�h���ꂽJPEG�f�[�^������ꏊ
                limit :      dst�̌��E�T�C�Y(����𒴂���ƃG���R�[�h���s)
                wrk :        ���[�N�G���A
                width :      �摜�̉���
                height :     �摜�̏c��
                quality :    �G���R�[�h�̃N�I���e�B(1-100)
                sampling :   ��摜�̃T���v�����O(1=YUV444,2=YUV420,3=YUV422�B�T���l�C���͓����I��3�Œ�)
                option : �I�v�V����(�r�b�g�_���a)
                           SSP_JPEG_RGB555: RGB555�`������G���R�[�h�B
                           SSP_JPEG_YUV422: YUV422�`������G���R�[�h�B
                                  �`���w�肪�����ꍇ�̓G���[�ɂȂ�܂��B
                           SSP_JPEG_THUMBNAIL: �T���l�C���t���G���R�[�h�B

  Returns:      �G���R�[�h���ꂽJPEG�̃T�C�Y(0�̏ꍇ�̓G���R�[�h���s)
 *---------------------------------------------------------------------------*/
u32 SSP_StartJpegEncoderLite(const void* src, u8 *dst, u32 limit, u8 *wrk,
                             u32 width, u32 height,
                             u32 quality, u32 sampling, u32 option);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegEncoderLiteEx

  Description:  JPEG�G���R�[�h���� (�ȃ�������)
                ��SSP_StartJpegEncoderEx�Ƃ͖��֌W�B
                �T���v�����O���P�̏ꍇ�Awidth,height�͂W�̔{��
                �T���v�����O���Q�̏ꍇ�Awidth,height��16�̔{���łȂ��Ƃ����Ȃ�
                �T���v�����O���R�̏ꍇ�Awidth��16�̔{��,height�͂W�̔{���łȂ��Ƃ����Ȃ�
                src, dst, wrk�ɂ�4�o�C�g�A���C�������g���K�v�ł��B

  Arguments:    src    :     �摜�f�[�^(���ォ��E���ւ�RGB555/YUV422�f�[�^)
                dst   :      �G���R�[�h���ꂽJPEG�f�[�^������ꏊ
                limit :      dst�̌��E�T�C�Y(����𒴂���ƃG���R�[�h���s)
                wrk :        ���[�N�G���A
                width :      �摜�̉���
                height :     �摜�̏c��
                quality :    �G���R�[�h�̃N�I���e�B(1-100)
                sampling :   ��摜�̃T���v�����O(1=YUV444,2=YUV420,3=YUV422�B�T���l�C���͓����I��3�Œ�)
                option : �I�v�V����(�r�b�g�_���a)
                           SSP_JPEG_RGB555: RGB555�`������G���R�[�h�B
                           SSP_JPEG_YUV422: YUV422�`������G���R�[�h�B
                                  �`���w�肪�����ꍇ�̓G���[�ɂȂ�܂��B
                           SSP_JPEG_THUMBNAIL: �T���l�C���t���G���R�[�h�B
                sign :       �����t�����s���ꍇ�� TRUE ���w�肷��B

  Returns:      �G���R�[�h���ꂽJPEG�̃T�C�Y(0�̏ꍇ�̓G���R�[�h���s)
 *---------------------------------------------------------------------------*/
static inline u32 SSP_StartJpegEncoderLiteEx(const void* src, u8 *dst, u32 limit, u8 *wrk,
                             u32 width, u32 height,
                             u32 quality, u32 sampling, u32 option, BOOL sign)
{
    u32 result;
    BOOL old_sign;
    
    old_sign = SSP_SetJpegEncoderSignMode(sign);
    result = SSP_StartJpegEncoderLite(src, dst, limit, wrk, width, height, quality, sampling, option);
    (void)SSP_SetJpegEncoderSignMode(old_sign);
    
    return result;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

/* TWL_SSP_JPEGENC_H_ */
#endif
