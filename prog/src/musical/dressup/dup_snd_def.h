//======================================================================
/**
 * @file	dup_local_def.h
 * @brief	�~���[�W�J���@�h���X�A�b�v�p����`
 * @author	ariizumi
 * @data	09/05/29
 */
//======================================================================

#pragma once

#include "sound/pm_sndsys.h"

//���艹
#define DUP_SE_DECIDE (SEQ_SE_MSCL_01)
//�L�����Z����
#define DUP_SE_CANCEL (SEQ_SE_CANCEL1)
//�h���X�A�b�v�I�����L���L����
#define DUP_SE_KIRAKIRA (SEQ_SE_DECIDE3)

//���X�g��]��
#define DUP_SE_LIST_ROTATE (SEQ_SE_MSCL_03)
//����炷���߂̒l
#define DUP_LIST_ROTATE_SE_VALUE (30*0x10000/360)
//�O�b�Y�^�b�`��
#define DUP_SE_LIST_TOUCH_GOODS (SEQ_SE_MSCL_02)
//���X�g����O�b�Y�����o����
#define DUP_SE_LIST_TAKE_GOODS (SEQ_SE_MSCL_04)
//���X�g�ɃO�b�Y��߂���
#define DUP_SE_LIST_RETURN_GOODS (SEQ_SE_MSCL_05)
//�O�b�Y�ɐG��Ȃ���
#define DUP_SE_LIST_NO_GOODS (SEQ_SE_WALL_HIT)

//�O�b�Y���|�P�����ɑ������鉹
#define DUP_SE_POKE_SET_GOODS (SEQ_SE_MSCL_06)
//�|�P�����^�b�`���i�O�b�Y�񑕒��j
#define DUP_SE_POKE_TOUCH (SEQ_SE_MSCL_07)
//�O�b�Y�ɌX�΂����鉹
#define DUP_SE_POKE_ADJUST (SEQ_SE_MSCL_08)
//����炷���߂̒l
#define DUP_POKE_ADJUST_SE_VALUE (4*0x10000/360)

//  PMSND_PlaySE(  );
