/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - demos - UnitTours/DEMOLib
  File:     DEMOBitmap.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17 #$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef DEMO_BITMAP_H_
#define DEMO_BITMAP_H_

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* constants */

#define DEMO_RGB_NONE   GX_RGBA(31, 31, 31, 0)
#define DEMO_RGB_CLEAR  GX_RGBA( 0,  0,  0, 0)

extern const u32 DEMOAsciiChr[8 * 0x100];


/*---------------------------------------------------------------------------*/
/* fundtions */

// �������Z�q�ɑ΂��闘�֐����l�������ȈՔŁB
#define DEMOSetBitmapTextColor(color)       DEMOiSetBitmapTextColor(DEMOVerifyGXRgb(color))
#define DEMOSetBitmapGroundColor(color)     DEMOiSetBitmapGroundColor(DEMOVerifyGXRgb(color))
#define DEMOFillRect(x, y, wx, wy, color)   DEMOiFillRect(x, y, wx, wy, DEMOVerifyGXRgb(color))
#define DEMODrawLine(sx, sy, tx, ty, color) DEMOiDrawLine(sx, sy, tx, ty, DEMOVerifyGXRgb(color))
#define DEMODrawFrame(x, y, wx, wy, color)  DEMOiDrawFrame(x, y, wx, wy, DEMOVerifyGXRgb(color))


/*---------------------------------------------------------------------------*
  Name:         DEMOInitDisplayBitmap

  Description:  �r�b�g�}�b�v�`�惂�[�h��DEMO��������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOInitDisplayBitmap(void);

/*---------------------------------------------------------------------------*
  Name:         DEMO_DrawFlip

  Description:  ���݂̕`����e��VRAM�֔��f����

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMO_DrawFlip();

/*---------------------------------------------------------------------------*
  Name:         DEMOHookConsole

  Description:  OS_PutString���t�b�N���ă��O�ɂ��o�͂���悤�ɐݒ�

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOHookConsole(void);

/*---------------------------------------------------------------------------*
  Name:         DEMOVerifyGXRgb

  Description:  �F�l�̐������`�F�b�N�B

  Arguments:    color : GXRgb�͈͓̔��Ǝv����F�l

  Returns:      ������GXRgb�Ȃ�color�����̂܂ܕԂ��A�͈͊O�Ȃ�0xFFFF
 *---------------------------------------------------------------------------*/
GXRgb DEMOVerifyGXRgb(int color);

/*---------------------------------------------------------------------------*
  Name:         DEMOSetBitmapTextColor

  Description:  �r�b�g�}�b�v�`��̃e�L�X�g�F��ݒ�

  Arguments:    color : �ݒ肷��F

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOiSetBitmapTextColor(GXRgb color);

/*---------------------------------------------------------------------------*
  Name:         DEMOSetBitmapGroundColor

  Description:  �r�b�g�}�b�v�`��̔w�i�F��ݒ�

  Arguments:    color : �ݒ肷��F

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOiSetBitmapGroundColor(GXRgb color);

/*---------------------------------------------------------------------------*
  Name:         DEMOFillRect

  Description:  �r�b�g�}�b�v�ɋ�`��`��

  Arguments:    x     : �`�� X ���W
                y     : �`�� Y ���W
                wx    : �`�� X ��
                wy    : �`�� Y ��
                color : �ݒ肷��F

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOiFillRect(int x, int y, int wx, int wy, GXRgb color);

/*---------------------------------------------------------------------------*
  Name:         DEMOBlitRect

  Description:  �r�b�g�}�b�v�ɋ�`��]��

  Arguments:    x      : �`�� X ���W
                y      : �`�� Y ���W
                wx     : �`�� X ��
                wy     : �`�� Y ��
                image  : �]�����̃C���[�W
                stroke : �]������1���C��������̃s�N�Z����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOBlitRect(int x, int y, int wx, int wy, const GXRgb *image, int stroke);

/*---------------------------------------------------------------------------*
  Name:         DEMOBlitTex16

  Description:  �r�b�g�}�b�v��16�F�e�N�X�`����]��

  Arguments:    x      : �`�� X ���W
                y      : �`�� Y ���W
                wx     : �`�� X �� (8�s�N�Z���̐����{�ł���K�v������)
                wy     : �`�� Y �� (8�s�N�Z���̐����{�ł���K�v������)
                chr    : �L�����N�^�C���[�W (4x8x8��1�����L�����N�^�t�H�[�}�b�g)
                plt    : �p���b�g�C���[�W (�C���f�b�N�X0�͓�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOBlitTex16(int x, int y, int wx, int wy, const void *chr, const GXRgb *plt);

/*---------------------------------------------------------------------------*
  Name:         DEMODrawLine

  Description:  �r�b�g�}�b�v�ɐ���`��

  Arguments:    sx     : �n�_ X ���W
                sy     : �n�_ Y ���W
                tx     : �I�_ X ��
                ty     : �I�_ Y ��
                color : �ݒ肷��F

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOiDrawLine(int sx, int sy, int tx, int ty, GXRgb color);

/*---------------------------------------------------------------------------*
  Name:         DEMODrawFrame

  Description:  �r�b�g�}�b�v�ɘg��`��

  Arguments:    x      : �`�� X ���W
                y      : �`�� Y ���W
                wx     : �`�� X ��
                wy     : �`�� Y ��
                color : �ݒ肷��F

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOiDrawFrame(int x, int y, int wx, int wy, GXRgb color);

/*---------------------------------------------------------------------------*
  Name:         DEMODrawText

  Description:  �r�b�g�}�b�v�ɕ������`��

  Arguments:    x      : �`�� X ���W
                y      : �`�� Y ���W
                format : ����������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMODrawText(int x, int y, const char *format, ...);

/*---------------------------------------------------------------------------*
  Name:         DEMOClearString

  Description:  BG�e�L�X�g��S�ď���

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOClearString(void);

/*---------------------------------------------------------------------------*
  Name:         DEMOPutString

  Description:  BG�e�L�X�g��`��

  Arguments:    x      : �`�� X ���W
                y      : �`�� Y ���W
                format : ����������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOPutString(int x, int y, const char *format, ...);

/*---------------------------------------------------------------------------*
  Name:         DEMOPutLog

  Description:  �T�u��ʂɃ��O�������\��

  Arguments:    format : ����������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOPutLog(const char *format, ...);

/*---------------------------------------------------------------------------*
  Name:         DEMOSetViewPort

  Description:  �r���[�|�[�g��Projection��ݒ�

  Arguments:    x      : ���� X ���W
                y      : ���� Y ���W
                wx     : �r���[�|�[�g X ��
                wy     : �r���[�|�[�g Y ��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOSetViewPort(int x, int y, int wx, int wy);


#ifdef __cplusplus
}/* extern "C" */
#endif

/* DEMO_BITMAP_H_ */
#endif
