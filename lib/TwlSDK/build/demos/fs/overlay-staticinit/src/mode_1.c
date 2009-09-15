/*---------------------------------------------------------------------------*
  Project:  TWLSDK - demos - FS - overlay-staticinit
  File:     mode_1.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-11-06 #$
  $Rev: 2135 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "mode.h"
#include "DEMO.h"

// NitroStaticInit() �� static initializer �Ɏw�肷��ɂ�
// ���̃w�b�_���C���N���[�h���܂�
#include <nitro/sinit.h>


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         MyUpdateFrame

  Description:  ���݂̃��[�h�� 1 �t���[��������������Ԃ��X�V���܂�

  Arguments:    frame_count      ���݂̓���t���[���J�E���g
                input            ���͏��̔z��
                player_count     ���݂̃v���[���[���� (input �̗L���v�f��)
                own_player_id    ���g�̃v���[���[�ԍ�

  Returns:      ���݂̃��[�h������̃t���[���ŏI������Ȃ�� FALSE ���A
                �����łȂ���� TRUE ��Ԃ��܂��B
 *---------------------------------------------------------------------------*/
static BOOL MyUpdateFrame(int frame_count,
                          const InputData * input, int player_count, int own_player_id)
{
    (void)frame_count;
    (void)player_count;

    // ��������������I��
    return !IS_INPUT_(input[own_player_id], ~0, push);
}

/*---------------------------------------------------------------------------*
  Name:         MyDrawFrame

  Description:  ���݂̃��[�h�œ�����Ԃ����Ƃɕ`��X�V���܂�

  Arguments:    frame_count      ���݂̓���t���[���J�E���g

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyDrawFrame(int frame_count)
{
    DEMOFillRect(0, 0, GX_LCD_SIZE_X, GX_LCD_SIZE_Y, DEMO_RGB_CLEAR);
    // �t�@�C�����ƊȈՑ�������̕\��
    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 31, 1));
    DEMODrawText(0, 10, "%s", __FILE__);
    DEMODrawText(30, 40, "press any key to return");
    // �P�Ƀt���[���J�E���g�̕\��
    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 16, 1));
    DEMODrawText(40, 80, "frame count = %d", frame_count);
}

/*---------------------------------------------------------------------------*
  Name:         MyEndFrame

  Description:  ���݂̃��[�h���I�����܂�

  Arguments:    p_next_mode      ���̃��[�h�𖾎��I�Ɏw�肷��ꍇ��
                                 ���̃|�C���^���w�����ID���㏑�����܂��B
                                 ���Ɏw�肵�Ȃ��ꍇ, ���݂̃��[�h���Ăяo����
                                 ���[�h���I������܂��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyEndFrame(FSOverlayID *p_next_mode)
{
    // ���������g�b�v���j���[�ɕԂ�
    (void)p_next_mode;
}

/*---------------------------------------------------------------------------*
  Name:         NitroStaticInit

  Description:  static initializer �Ƃ��Ă̎����������֐�

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void NitroStaticInit(void)
{
    UpdateFrame = MyUpdateFrame;
    DrawFrame = MyDrawFrame;
    EndFrame = MyEndFrame;

    // ���[�h���ƂɕK�v�ȏ����������������ōs���܂�

}
