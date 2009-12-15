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

#define MOVE_POKE_MAX		(4)	//���C�J�~�E�J�U�J�~�A�\��2
#define MOVE_POKE_RAIKAMI	    (0)	//���C�J�~
#define MOVE_POKE_KAZAKAMI		(1)	//�J�U�J�~

#define	LOCATION_MAX	(17)	//�ړ��ꏊ��

#define MVPOKE_ZONE_NULL  (0xFFFF)  //�����]�[��ID(�B��Ă��鎞��ID)

///�ړ��|�P�����ړ�����^�C�v
#define MVPOKE_TYPE_NORMAL  (0) //�ʏ�
#define MVPOKE_TYPE_TIME    (1) //�^�C���]�[���ˑ�

///�ړ��|�P�������
#define MVPOKE_STATE_NONE (0)
#define MVPOKE_STATE_DOWN (1) //�|���������
#define MVPOKE_STATE_GET  (2) //�߂܂���

#endif	 //__MOVE_POKEMON_DEF_H__
