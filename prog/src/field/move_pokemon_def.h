//============================================================================================
/**
 * @file	move_pokemon_def.h
 * @brief	�ړ��|�P������`�Ƃ�
 * @author	Nozomu Saito
 * @date	06.05.11
 */
//============================================================================================
#ifndef __MOVE_POKEMON_DEF_H__
#define	__MOVE_POKEMON_DEF_H__

#define MOVE_POKE_MAX		(2)	//���C�J�~�E�J�U�J�~�A�\��2
#define MOVE_POKE_RAIKAMI	    (0)	//���C�J�~
#define MOVE_POKE_KAZAKAMI		(1)	//�J�U�J�~

#define	MVPOKE_LOCATION_MAX	(17)	//�ړ��ꏊ��
#define MVPOKE_ZONE_IDX_HIDE  (MVPOKE_LOCATION_MAX) //�B��Ă��鎞�̃]�[��Idx
#define MVPOKE_ZONE_NULL  (0xFFFF)  //�����]�[��ID(�B��Ă��鎞��ID)

///�ړ��|�P�����ړ�����^�C�v
#define MVPOKE_TYPE_NORMAL  (0) //�ʏ�
#define MVPOKE_TYPE_TIME    (1) //�^�C���]�[���ˑ�

///�ړ��|�P�������
#define MVPOKE_STATE_NONE (0) //���Ȃ�
#define MVPOKE_STATE_MOVE (1) //�ړ���
#define MVPOKE_STATE_DOWN (2) //�|���������
#define MVPOKE_STATE_GET  (3) //�߂܂���

#endif	 //__MOVE_POKEMON_DEF_H__
