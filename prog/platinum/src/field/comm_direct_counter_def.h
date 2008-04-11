//=============================================================================
/**
 * @file	comm_direct_counter_def.h
 * @bfief	�ʐM�_�C���N�g�R�[�i�[�̃J�E���^�[ �X�N���v�g�Ƌ��p�̒�`
 * @author	katsumi ohno
 * @date	05/08/04
 */
//=============================================================================

#ifndef __COMM_DIRECT_COUNTER_DEF_H__
#define __COMM_DIRECT_COUNTER_DEF_H__


#define DBC_TIM_BATTLE_MPDS (3)  // �ΐ핔��->�o�g��
#define DBC_TIM_BATTLE_DSOK (4)  // �o�g�����[�h�ɂȂ���
#define DBC_TIM_BATTLE_MPOK (5)  // �������[�h�ɂȂ���


#define DBC_TIM_BATTLE_BACK (30)    // �ΐ핔���ҋ@��Ԉʒu�ړ�����
#define DBC_TIM_BATTLE_PAUSE (92)    // �ΐ핔���ҋ@���
#define DBC_TIM_BATTLE_START (93)    // �ΐ핔���I�� > �o�g��
#define DBC_TIM_BATTLE_EXIT  (94)    // �ΐ핔���o�čs���Ƃ�
#define DBC_TIM_BATTLE_EXIT2  (91)    // �ΐ핔���o�čs���Ƃ�  �������

#define DBC_TIM_BATTLE_TR (95)    // �ΐ핔���g���[�i�[�J�[�h�����ҋ@
#define DBC_TIM_BATTLE_ROOMIN	(96) // �ΐ핔���ɓ���Ƃ�
#define DBC_TIM_BATTLE_DSCHANGE (97)  // �f�[�^�`���؂�ւ�
#define DBC_TIM_BATTLE_DSEND (98) // �؂�ւ�����
//-----------------------------------------------------------------------------
//�X�e�[�W
//-----------------------------------------------------------------------------
#define DBC_TIM_STAGE_POKETYPE_SEL	(100)	// �|�P�����^�C�v�I��O�̓���
#define DBC_TIM_STAGE_BATTLE		(101)	// �퓬�O�̓���
#define DBC_TIM_STAGE_MENU			(102)	// �Â���I�񂾌�̓���
#define DBC_TIM_STAGE_TR_ENEMY		(103)	// �g���[�i�[�ƃ|�P������ʐM�I���̓���
#define DBC_TIM_STAGE_END			(104)	// �^�C�v�I����ʏI���̓���
#define DBC_TIM_STAGE_LOSE_END		(105)	// �퓬�����I���̓���
#define DBC_TIM_STAGE_SAVE_AFTER	(106)	// �Z�[�u��̃}�b�v�J�ڑO�̓���
#define DBC_TIM_STAGE_GIVE			(107)	// ��߂�I�񂾌�̓���
//d32r0401.ev
#define STAGE_COMM_MULTI_POKE_SELECT	(108)
#define STAGE_COMM_MULTI_AUTO_SAVE		(109)
#define STAGE_COMM_MULTI_GO_SAVE		(110)
//-----------------------------------------------------------------------------
//�L���b�X��
//-----------------------------------------------------------------------------
#define DBC_TIM_CASTLE_RENTAL		(120)	// �����^����ʂ֍s���O�̓���
#define DBC_TIM_CASTLE_BATTLE		(121)	// �퓬�O�̓���
#define DBC_TIM_CASTLE_MENU			(122)	// �Â����I�񂾌�̓���
//#define DBC_TIM_CASTLE_TRADE		(123)	// ������ʂ֍s���O�̓���
#define DBC_TIM_CASTLE_GIVE			(124)	// ��߂��I�񂾌�̓���
//#define DBC_TIM_CASTLE_TRADE_END	(125)	// ������ʏI���̓���
#define DBC_TIM_CASTLE_CHANGE		(126)	// �ʉ�ʂɍs���O�̓���
#define DBC_TIM_CASTLE_MENU_BEFORE	(127)	// ���j���[�\���O�̓���
#define DBC_TIM_CASTLE_LOSE_END		(128)	// �퓬�����I���̓���
#define DBC_TIM_CASTLE_SAVE_AFTER	(129)	// �Z�[�u��̃}�b�v�J�ڑO�̓���
//�莝��
#define DBC_TIM_CASTLE_MINE_UP		(130)	// �莝���F�����N�A�b�v��̃��b�Z�[�W�I���̓���
#define DBC_TIM_CASTLE_MINE_END		(131)	// �莝���F��ʏI���̓���
#define DBC_TIM_CASTLE_MINE_CANCEL	(132)	// �莝���F�u�߂�v�L�����Z���̓���
//�G�g���[�i�[
#define DBC_TIM_CASTLE_ENEMY_UP		(133)	// �G�F�����N�A�b�v��̃��b�Z�[�W�I���̓���
#define DBC_TIM_CASTLE_ENEMY_END	(134)	// �G�F��ʏI���̓���
#define DBC_TIM_CASTLE_ENEMY_CANCEL	(135)	// �G�F�u�߂�v�L�����Z���̓���
//d32r0501.ev
#define CASTLE_COMM_MULTI_POKE_SELECT	(136)
#define CASTLE_COMM_MULTI_AUTO_SAVE		(137)
#define CASTLE_COMM_MULTI_GO_SAVE		(138)
#define CASTLE_COMM_MULTI_SIO_END		(139)
//-----------------------------------------------------------------------------
//���[���b�g
//-----------------------------------------------------------------------------
#define DBC_TIM_ROULETTE_RENTAL			(140)	// �g���[�i�[�o��O�̓���
#define DBC_TIM_ROULETTE_BATTLE			(141)	// �퓬�O�̓���
#define DBC_TIM_ROULETTE_MENU			(142)	// �Â����I�񂾌�̓���
#define DBC_TIM_ROULETTE_CALL			(143)	// ���[���b�g��ʂ֍s���O�̓���
#define DBC_TIM_ROULETTE_GIVE			(144)	// ��߂��I�񂾌�̓���
//#define DBC_TIM_ROULETTE_TRADE_END	(145)	// ������ʏI���̓���
#define DBC_TIM_ROULETTE_CHANGE			(146)	// �ʉ�ʂɍs���O�̓���
//#define DBC_TIM_ROULETTE_MENU_BEFORE	(147)	// ���j���[�\���O�̓���
#define DBC_TIM_ROULETTE_LOSE_END		(148)	// �퓬�����I���̓���
#define DBC_TIM_ROULETTE_PANEL_INFO		(149)	// �p�l���̐�����̓���
#define DBC_TIM_ROULETTE_SAVE_AFTER		(150)	// �Z�[�u��̃}�b�v�J�ڑO�̓���
//�����N
#define DBC_TIM_ROULETTE_UP				(151)	// �����N�F�����N�A�b�v��̃��b�Z�[�W�I���̓���
#define DBC_TIM_ROULETTE_END			(152)	// �����N�F��ʏI���̓���
#define DBC_TIM_ROULETTE_CANCEL			(153)	// �����N�F�u�߂�v�L�����Z���̓���
//d32r0601.ev
#define ROULETTE_COMM_MULTI_POKE_SELECT	(154)
#define ROULETTE_COMM_MULTI_AUTO_SAVE	(155)
#define ROULETTE_COMM_MULTI_GO_SAVE		(156)
#define ROULETTE_COMM_MULTI_SIO_END		(157)
//-----------------------------------------------------------------------------
//�t�@�N�g���[
//-----------------------------------------------------------------------------
#define DBC_TIM_FACTORY_RENTAL			(160)	// �����^����ʂ֍s���O�̓���
#define DBC_TIM_FACTORY_BATTLE			(161)	// �퓬�O�̓���
#define DBC_TIM_FACTORY_MENU			(162)	// �Â���I�񂾌�̓���
#define DBC_TIM_FACTORY_TRADE			(163)	// ������ʂ֍s���O�̓���
#define DBC_TIM_FACTORY_RENTAL_END		(164)	// �����^����ʏI���̓���
#define DBC_TIM_FACTORY_TRADE_END		(165)	// ������ʏI���̓���
#define DBC_TIM_FACTORY_LOSE_END		(166)	// �퓬�����I���̓���
#define DBC_TIM_FACTORY_SAVE_AFTER		(167)	// �Z�[�u��̃}�b�v�J�ڑO�̓���
#define DBC_TIM_FACTORY_GIVE			(168)	// ��߂�I�񂾌�̓���
//d32r0301.ev
#define FACTORY_COMM_MULTI_AUTO_SAVE	(169)
//-----------------------------------------------------------------------------
//�t�����e�B�AWiFi��t
//-----------------------------------------------------------------------------
#define DBC_TIM_FR_WIFI_COUNTER_YAMERU	(170)	// �u��߂�v�I��������̓���
#define DBC_TIM_FR_WIFI_COUNTER_GO		(171)	// �e�{�݂֍s���O�̓���
#define DBC_TIM_FR_WIFI_COUNTER_END		(172)	// ���������V�΂Ȃ��ŏI�����鎞�̓���


#define REGULATION_RET_NONE  (3)   // ���M�����[�V��������Ȃ�
#define REGULATION_RET_OK  (1)   // ���M�����[�V��������
#define REGULATION_RET_END (2)   // ���M�����[�V������I�������ɏI��


#endif //__COMM_DIRECT_COUNTER_DEF_H__
