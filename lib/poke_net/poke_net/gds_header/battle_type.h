#ifndef __POKE_GDS_BATTLETYPE_DEF_H_
#define __POKE_GDS_BATTLETYPE_DEF_H_

//�퓬��ʃt���O�i�퓬�V�X�e�����̔��ʂŎg�p���Ă��܂�BattleParam��fight_type�̎w��ɂ͎g��Ȃ��ł��������j
#define	FIGHT_TYPE_1vs1			(0x00000000)		///<1vs1
#define	FIGHT_TYPE_YASEI		(0x00000000)		///<�쐶��1vs1
#define	FIGHT_TYPE_TRAINER		(0x00000001)		///<�g���[�i�[��
#define	FIGHT_TYPE_2vs2			(0x00000002)		///<2vs2
#define	FIGHT_TYPE_SIO			(0x00000004)		///<�ʐM�ΐ�
#define	FIGHT_TYPE_MULTI		(0x00000008)		///<�}���`�ΐ�
#define	FIGHT_TYPE_TAG			(0x00000010)		///<�^�b�O
#define	FIGHT_TYPE_SAFARI		(0x00000020)		///<�T�t�@���]�[��
#define	FIGHT_TYPE_AI			(0x00000040)		///<AI�}���`
#define	FIGHT_TYPE_TOWER		(0x00000080)		///<�o�g���^���[
#define	FIGHT_TYPE_MOVE			(0x00000100)		///<�ړ��|�P����
#define	FIGHT_TYPE_POKE_PARK	(0x00000200)		///<�|�P�p�[�N
#define	FIGHT_TYPE_GET_DEMO		(0x00000400)		///<�ߊl�f��

#define	FIGHT_TYPE_DEBUG		(0x80000000)		///<�f�o�b�O�퓬

//�퓬��ʃt���O�i�K�v�ȃt���O��OR�ς݂̂���BattleParam��fight_type�w��ɂ͂�������g���Ă��������j
#define	FIGHT_TYPE_1vs1_YASEI				(FIGHT_TYPE_1vs1|FIGHT_TYPE_YASEI)						///<1vs1�쐶��
#define	FIGHT_TYPE_2vs2_YASEI				(FIGHT_TYPE_2vs2|FIGHT_TYPE_MULTI|FIGHT_TYPE_AI)		///<2vs2�쐶��
#define	FIGHT_TYPE_1vs1_TRAINER				(FIGHT_TYPE_1vs1|FIGHT_TYPE_TRAINER)					///<1vs1�g���[�i�[��
#define	FIGHT_TYPE_2vs2_TRAINER				(FIGHT_TYPE_2vs2|FIGHT_TYPE_TRAINER)					///<2vs2�g���[�i�[��
#define	FIGHT_TYPE_1vs1_SIO					(FIGHT_TYPE_SIO|FIGHT_TYPE_TRAINER)						///<1vs1�ʐM�g���[�i�[��
#define	FIGHT_TYPE_2vs2_SIO					(FIGHT_TYPE_1vs1_SIO|FIGHT_TYPE_2vs2)					///<2vs2�ʐM�g���[�i�[��
#define	FIGHT_TYPE_MULTI_SIO				(FIGHT_TYPE_2vs2_SIO|FIGHT_TYPE_MULTI)					///<2vs2�ʐM�g���[�i�[��
#define	FIGHT_TYPE_AI_MULTI					(FIGHT_TYPE_2vs2_TRAINER|FIGHT_TYPE_MULTI|FIGHT_TYPE_AI)///<2vs2AI�}���`��
#define	FIGHT_TYPE_BATTLE_TOWER_1vs1		(FIGHT_TYPE_1vs1_TRAINER|FIGHT_TYPE_TOWER)				///<1vs1�o�g���^���[��
#define	FIGHT_TYPE_BATTLE_TOWER_2vs2		(FIGHT_TYPE_2vs2_TRAINER|FIGHT_TYPE_TOWER)				///<2vs2�o�g���^���[��
#define	FIGHT_TYPE_BATTLE_TOWER_AI_MULTI	(FIGHT_TYPE_AI_MULTI|FIGHT_TYPE_TOWER)					///<�o�g���^���[AI�}���`��
#define	FIGHT_TYPE_BATTLE_TOWER_SIO_MULTI	(FIGHT_TYPE_MULTI_SIO|FIGHT_TYPE_TOWER)					///<�o�g���^���[�ʐM�}���`��
#define	FIGHT_TYPE_TAG_BATTLE				(FIGHT_TYPE_2vs2_TRAINER|FIGHT_TYPE_TAG)				///<�^�b�O�o�g����


#endif // __POKE_GDS_BATTLETYPE_DEF_H_
