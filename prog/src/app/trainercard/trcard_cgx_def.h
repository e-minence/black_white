//============================================================================================
/**
 * @file   trcard_cgx_def.h
 * @bfief  �g���[�i�[�J�[�h��ʒ�`�w�b�_
 * @author Akito Mori / Nozomu Saito (WB�ڐA�͗L�򂭂�)
 * @date   10.03.09
 */
//============================================================================================
#ifndef __TRCARD_CGX_DEF_H__
#define __TRCARD_CGX_DEF_H__

#define TR_BG_BASE    (1) //�v���X�����Ɏg���Ă���
//�g���[�i�[�摜�T�C�Y  (�j����l���̃L������32x4�A���j�I���g���[�i�[��10*11)
#define TR_BG_SIZE    (32*4)  

//���b�Z�[�W�\���̈�(�}�C�i�X�����Ɏg���Ă���)
#define MSG_BG_BASE (1023)

//�T�C��
#define SIGN_CGX  (TR_BG_BASE)
#define SIGN_PX (4)
#define SIGN_PY (11)
#define SIGN_SX (24)
#define SIGN_SY (8)
#define SIGN_BYTE_SIZE  (SIGN_SX*SIGN_SY*64)
#define TR_SIGN_SIZE  (SIGN_SX*SIGN_SY) //�T�C���T�C�Y

#define SIGN_ANIME_PX ( 10 )
#define SIGN_ANIME_PY ( 11 )
#define SIGN_ANIME_SX ( 12 )
#define SIGN_ANIME_SY (  8 )


//���[�_�[�A�C�R��
#define LEADER_ICON_X (7)
#define LEADER_ICON_JY  (7)
#define LEADER_ICON_KY  (14)
#define LEADER_ICON_OX  (6)
#define LEADER_ICON_OY  (5)
#define LEADER_ICON_SX  (5)
#define LEADER_ICON_SY  (4)
#define LEADER_ICON_PY  (9)

#endif  //__TRCARD_CGX_DEF_H__
