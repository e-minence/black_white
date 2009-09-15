/*---------------------------------------------------------------------------*
  Project:  TWLSDK - demos - FS - overlay-staticinit
  File:     mode_2.c

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
/* variables */

// �^�b�`�y���������
static BOOL is_tp_on;
static struct Point { int x, y; }  bak_pos, cur_pos;


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

    // �^�b�`�y���̓��͏�Ԃ��X�V
    bak_pos = cur_pos;
    is_tp_on = IS_INPUT_(input[own_player_id], PAD_BUTTON_TP, push);
    if (is_tp_on)
    {
        cur_pos.x = input[own_player_id].tp.x;
        cur_pos.y = input[own_player_id].tp.y;
    }
    // �^�b�`�y���ȊO�̉�������������I��
    return !IS_INPUT_(input[own_player_id], PAD_ALL_MASK, push);
}

/*---------------------------------------------------------------------------*
  Name:         MyDrawFrame

  Description:  ���݂̃��[�h�œ�����Ԃ����Ƃɕ`��X�V���܂�

  Arguments:    frame_count      ���݂̓���t���[���J�E���g

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyDrawFrame(int frame_count)
{
    // ����t���[���͉�ʂ��N���A
    if (frame_count == 0)
    {
        DEMOFillRect(0, 0, GX_LCD_SIZE_X, GX_LCD_SIZE_Y, DEMO_RGB_CLEAR);
        DEMOSetBitmapTextColor(GX_RGBA(31, 31, 31, 1));
        DEMODrawText(0, 10, "%s", __FILE__);
        DEMODrawText(30, 40, "touch screen : draw line");
        DEMODrawText(30, 50, "press any key to return");
    }
    // ���݈ʒu�ƋO�Ղ�`��
    DEMOFillRect(cur_pos.x - 2, cur_pos.y - 2, 4, 4, GX_RGBA(0, 31, 0, 1));
    if (is_tp_on)
    {
        DEMODrawLine(bak_pos.x, bak_pos.y, cur_pos.x, cur_pos.y, GX_RGBA(16, 16, 31, 1));
    }
}

/*---------------------------------------------------------------------------*
  Name:         MyEndFrame

  Description:  ���݂̃��[�h���I�����܂�.

  Arguments:    p_next_mode      ���̃��[�h�𖾎��I�Ɏw�肷��ꍇ��.
                                 ���̃|�C���^���w�����ID���㏑�����܂�.
                                 ���Ɏw�肵�Ȃ��ꍇ, ���݂̃��[�h���Ăяo����
                                 ���[�h���I������܂�.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyEndFrame(FSOverlayID *p_next_mode)
{
    (void)p_next_mode;
}

/*---------------------------------------------------------------------------*
  Name:         NitroStaticInit

  Description:  static initializer �Ƃ��Ă̎����������֐�.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void NitroStaticInit(void)
{
    UpdateFrame = MyUpdateFrame;
    DrawFrame = MyDrawFrame;
    EndFrame = MyEndFrame;

    /* ���[�h���ƂɕK�v�ȏ����������������ōs���܂�. */

    is_tp_on = FALSE;
    cur_pos.x = HW_LCD_WIDTH / 2;
    cur_pos.y = HW_LCD_HEIGHT / 2;
    bak_pos = cur_pos;
}
