/*
 *  @file field_encount_local.h
 *  @brief  field_encount.c ���[�J���w�b�_
 *  @author Miyuki Iwasawa
 *  @date   09.10.19
 */

#pragma once

///�G���J�E���g����@�v���C���[���
typedef struct _EWK_PLAYER{
  u16 pos_x;    //�Ō�ɃG���J�E���g�����v���C���[�O���b�h�|�W�V����
  u16 pos_y;
  u16 pos_z;
  u16 move_f;   //
  u32 walk_ct;
}EWK_PLAYER;

///�G���J�E���g���䃏�[�N
struct _TAG_ENCOUNT_WORK
{
  EWK_PLAYER player;
};



