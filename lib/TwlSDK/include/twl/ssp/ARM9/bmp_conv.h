/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SSP - include
  File:     bmp_conv.h

  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-01-09#$
  $Rev: 9810 $
  $Author: kitase_hirotake $
 *---------------------------------------------------------------------------*/

#ifndef TWL_SSP_BMP_H_
#define TWL_SSP_BMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

typedef enum
{
    SSP_BMP_YUV422 = 1,
    SSP_BMP_RGB555
}
SSPConvertBmpType;

/*---------------------------------------------------------------------------*
  Name:         SSP_YUV422ToRGB888b

  Description:  YUV422 �`���̃f�[�^�� RGB888(Bottom up) �`���̃f�[�^�ɃR���o�[�g����

  Arguments:    src    : ���̓f�[�^(YUV422)
                dst    : �o�̓f�[�^(RGB888)
                width  : �摜�̕�
                height : �摜�̍���

  Returns:      �T�C�Y��Ԃ�
 *---------------------------------------------------------------------------*/
u32 SSP_YUV422ToRGB888b( const void* src, void* dst, u16 width, u16 height );

/*---------------------------------------------------------------------------*
  Name:         SSP_RGB555ToRGB888b

  Description:  RGB555 �`���̃f�[�^�� RGB888(Bottom up) �`���̃f�[�^�ɃR���o�[�g����

  Arguments:    src    : ���̓f�[�^(YUV422)
                dst    : �o�̓f�[�^(RGB888)
                width  : �摜�̕�
                height : �摜�̍���

  Returns:      �T�C�Y��Ԃ�
 *---------------------------------------------------------------------------*/
u32 SSP_RGB555ToRGB888b( const void* src, void* dst, u16 width, u16 height );

/*---------------------------------------------------------------------------*
  Name:         SSP_GetRGB888Size

  Description:  RGB888 �`���̃f�[�^�̃T�C�Y���擾����

  Arguments:    width  : �摜�̕�
                height : �摜�̍���

  Returns:      �T�C�Y��Ԃ�
 *---------------------------------------------------------------------------*/
u32 SSP_GetRGB888Size( u16 width, u16 height );

/*---------------------------------------------------------------------------*
  Name:         SSP_CreateBmpHeader

  Description:  BMP �t�@�C���̃w�b�_���쐬����

  Arguments:    dst    : �o�̓f�[�^
                width  : �摜�̕�
                height : �摜�̍���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SSP_CreateBmpHeader( u8 *dst, u16 width, u16 height );

/*---------------------------------------------------------------------------*
  Name:         SSP_GetBmpHeaderSize

  Description:  BMP �t�@�C���̃w�b�_�̃T�C�Y���擾����

  Arguments:    None.

  Returns:      �T�C�Y��Ԃ�
 *---------------------------------------------------------------------------*/
u32 SSP_GetBmpHeaderSize();

/*---------------------------------------------------------------------------*
  Name:         SSP_GetBmpFileSize

  Description:  BMP �t�@�C���̃T�C�Y���擾����

  Arguments:    width  : �摜�̕�
                height : �摜�̍���

  Returns:      �T�C�Y��Ԃ�
 *---------------------------------------------------------------------------*/
u32 SSP_GetBmpFileSize(u16 width, u16 height);

/*---------------------------------------------------------------------------*
  Name:         SSP_CreateBmpFile

  Description:  BMP �t�@�C�����쐬����

  Arguments:    src    : ���̓f�[�^(YUV422 or RGB555)
                dst    : BMP�t�@�C�����i�[����o�b�t�@
                width  : �摜�̕�
                height : �摜�̍���
                type   : ���̓f�[�^�̎��

  Returns:      �쐬����BMP�t�@�C���̃T�C�Y��Ԃ�
 *---------------------------------------------------------------------------*/
u32 SSP_CreateBmpFile(const void* src, void* dst, u16 width, u16 height, SSPConvertBmpType type);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* TWL_SSP_BMP_H_ */
#endif
