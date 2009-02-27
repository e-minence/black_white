//======================================================================
/**
 * @file	btlv_effvm_def.h
 * @brief	�G�t�F�N�g�V�[�P���X�p�̃}�N����`�t�@�C��
 * @author	HisashiSogabe
 * @date	2009.02.12
 */
//======================================================================

//====================================================================================
/**
 *	�萔��`
 */
//====================================================================================

#ifndef __BTLV_EFFVM_DEF_H_
#define __BTLV_EFFVM_DEF_H_

#define	BTLV_EFFVM_STACK_SIZE	( 16 )	///<�g�p����X�^�b�N�̃T�C�Y
#define	BTLV_EFFVM_REG_SIZE		( 8 )	///<�g�p���郌�W�X�^�̐�

//�X�N���v�g�A�h���X�e�[�u���p��`
#define	BTLEFF_AA_SAME			( 0xffffffff )		//�����ʒuAA�Ɠ���
#define	BTLEFF_AA_REVERSE		( 0xfffffffe )		//�����ʒuAA�̔��]
#define	BTLEFF_BB_SAME			( 0xfffffffd )		//�����ʒuBB�Ɠ���
#define	BTLEFF_BB_REVERSE		( 0xfffffffc )		//�����ʒuBB�̔��]
#define	BTLEFF_A_SAME			( 0xfffffffb )		//�����ʒuA�Ɠ���
#define	BTLEFF_A_REVERSE		( 0xfffffffa )		//�����ʒuA�̔��]
#define	BTLEFF_B_SAME			( 0xfffffff9 )		//�����ʒuB�Ɠ���
#define	BTLEFF_B_REVERSE		( 0xfffffff8 )		//�����ʒuB�̔��]
#define	BTLEFF_C_SAME			( 0xfffffff7 )		//�����ʒuC�Ɠ���
#define	BTLEFF_C_REVERSE		( 0xfffffff6 )		//�����ʒuC�̔��]
#define	BTLEFF_D_SAME			( 0xfffffff5 )		//�����ʒuD�Ɠ���
#define	BTLEFF_D_REVERSE		( 0xfffffff4 )		//�����ʒuD�̔��]

//�J�����ړ��^�C�v
#define	BTLEFF_CAMERA_MOVE_DIRECT			( 0 )		//�_�C���N�g
#define	BTLEFF_CAMERA_MOVE_INTERPOLATION	( 1 )		//�Ǐ]

//�J�����ړ���
#define	BTLEFF_CAMERA_POS_AA		( 0 )
#define	BTLEFF_CAMERA_POS_BB		( 1 )
#define	BTLEFF_CAMERA_POS_A			( 2 )
#define	BTLEFF_CAMERA_POS_B			( 3 )
#define	BTLEFF_CAMERA_POS_C			( 4 )
#define	BTLEFF_CAMERA_POS_D			( 5 )
#define	BTLEFF_CAMERA_POS_INIT		( 6 )

//�p�[�e�B�N���Đ�
#define	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	( 0 )
#define	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	( 1 )

//�|�P��������
#define	BTLEFF_POKEMON_SIDE_ATTACK				( 0 )		//�U����
#define	BTLEFF_POKEMON_SIDE_ATTACK_PAIR			( 1 )		//�U�����y�A
#define	BTLEFF_POKEMON_SIDE_DEFENCE				( 2 )		//�h�䑤
#define	BTLEFF_POKEMON_SIDE_DEFENCE_PAIR		( 3 )		//�h�䑤�y�A

#define	BTLEFF_POKEMON_MOVE_DIRECT				( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_MOVE_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_MOVE_ROUNDTRIP			( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_MOVE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_POKEMON_SCALE_DIRECT				( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_SCALE_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_SCALE_ROUNDTRIP			( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_SCALE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_POKEMON_ROTATE_DIRECT			( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_ROTATE_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_ROTATE_ROUNDTRIP			( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_ROTATE_ROUNDTRIP_LONG	( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_MEPACHI_ON						( POKE_MCSS_MEPACHI_ON )
#define	BTLEFF_MEPACHI_OFF						( POKE_MCSS_MEPACHI_OFF )
#define	BTLEFF_MEPACHI_MABATAKI					( 2 )
#define	BTLEFF_ANM_STOP							( POKE_MCSS_ANM_STOP_ON )
#define	BTLEFF_ANM_START						( POKE_MCSS_ANM_STOP_OFF )

#endif //__BTLV_EFFVM_DEF_H_

//====================================================================================
/**
 *	VisualStudio�ō쐬�����G�t�F�N�g�G�f�B�^�c�[���ł��g�p�����t�@�C���ł� 
 *	�e���߃}�N����`�̃R�����g����c�[���ŕ\�������R�}���h��������̌^�Ȃǂ𔻒f���܂�
 */
//====================================================================================

#if 0

ex)
 * #param_num	4
 * @param	type	�J�����ړ��^�C�v
 * @param	pos_x	�J�����ړ���X���W
 * @param	pos_y	�J�����ړ���Y���W
 * @param	pos_z	�J�����ړ���Z���W
 *
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_DIRECT	BTLEFF_CAMERA_MOVE_INTERPOLATION
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_FX32

#param_num�������āA�R�}���h�̈����̐���錾�����܂�(@param�̑����ɂȂ�܂��j
doxygen������@param�͂��̂܂܃c�[����̃R�����g�Ƃ��ĕ\������܂�
#param���������ƂŁA�c�[���œǂݍ��ރp�����[�^�ł��邱�Ƃ̐錾�����܂�
�p�����[�^�̌^���L�q���܂�
�ECOMBOBOX_TEXT
�@�R���{�{�b�N�X�^�ɂ��āA�R���{�{�b�N�X�ŕ\�����郊�X�g�f�[�^���L���܂�
�ECOMBOBOX_VALUE
�@��ɐݒ肵���R���{�{�b�N�X�^�̃C���f�b�N�X�ɑΉ����Ď��ۂ̃V�[�P���X�\�[�X�ɏ����o����郉�x����`�Ȃǂ��L�q���܂�
�EVALUE_FX32
�@FX32�^�̒l�i�c�[����ł́A��������̐��l���͂��\�ɂ��܂��j
�EFILE_DIALOG
�@�t�@�C���_�C�A���O��\�����ăt�@�C���I���𑣂��@�I�����i��g���q���w��ł��܂�
�EFILE_DIALOG_COMBOBOX
�@�t�@�C���_�C�A���O�őI�������t�@�C�����R���{�{�b�N�X�ŕ\�����܂��@�I�����i��g���q���w��ł��܂�

#endif

#ifndef __C_NO_DEF_

//����Ƀp�f�B���O�������Ȃ��悤�ɂ���
	.option	alignment off

	.macro	INIT_CMD
def_cmd_count = 0
	.endm

	.macro	DEF_CMD	symname
\symname = def_cmd_count
def_cmd_count = ( def_cmd_count + 1 )
	.endm

//���߃V���{���錾
	INIT_CMD
	DEF_CMD	EC_CAMERA_MOVE
	DEF_CMD	EC_PARTICLE_LOAD
	DEF_CMD	EC_PARTICLE_PLAY
	DEF_CMD	EC_PARTICLE_PLAY_WITH_DIR
	DEF_CMD	EC_POKEMON_MOVE
	DEF_CMD	EC_POKEMON_SCALE
	DEF_CMD	EC_POKEMON_ROTATE
	DEF_CMD	EC_POKEMON_SET_MEPACHI_FLAG
	DEF_CMD	EC_POKEMON_SET_ANM_FLAG

//�I���R�}���h�͕K����ԉ��ɂȂ�悤�ɂ���
	DEF_CMD	EC_SEQ_END

//���߃}�N����`
//======================================================================
/**
 * @brief		�J�����ړ�
 *
 * #param_num	4
 * @param	type		�J�����ړ��^�C�v
 * @param	move_pos	�ړ���
 * @param	frame		�J�����^�C�v���Ǐ]�̂Ƃ��A���t���[���ňړ���ɓ��B���邩���w��
 * @param	brake		�u���[�L�������͂��߂�t���[�����w��
 *
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_DIRECT	BTLEFF_CAMERA_MOVE_INTERPOLATION
 * #param	COMBOBOX_TEXT	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D	�����ʒu
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D	BTLEFF_CAMERA_POS_INIT
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	CAMERA_MOVE	type, move_pos, frame, brake
	.short	EC_CAMERA_MOVE
	.long	\type
	.long	\move_pos
	.long	\frame
	.long	\brake
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���f�[�^���[�h
 *
 * #param_num	1
 * @param	datID	�A�[�J�C�u�f�[�^��datID
 *
 * #param	FILE_DIALOG	.spa
 */
//======================================================================
	.macro	PARTICLE_LOAD	datID
	.short	EC_PARTICLE_LOAD
	.long	\datID
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���Đ�
 *
 * #param_num	3
 * @param	num			�Đ��p�[�e�B�N���i���o�[
 * @param	index		spa���C���f�b�N�X�i���o�[
 * @param	start_pos	�p�[�e�B�N���Đ��J�n�����ʒu
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	VALUE_INT
 * #param	COMBOBOX_TEXT	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D	BTLEFF_CAMERA_POS_INIT
 */
//======================================================================
	.macro	PARTICLE_PLAY	num, index, start_pos
	.short	EC_PARTICLE_PLAY
	.long	\num
	.long	\index
	.long	\start_pos
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���Đ��i�����w�肠��j
 *
 * #param_num	5
 * @param	num			�Đ��p�[�e�B�N���i���o�[
 * @param	index		spa���C���f�b�N�X�i���o�[
 * @param	start_pos	�p�[�e�B�N���Đ��J�n�����ʒu
 * @param	dir_pos		�p�[�e�B�N���Đ����������ʒu
 * @param	dir_angle	�p�[�e�B�N���Đ�����Y�p�x
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	VALUE_INT
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE
 * #param	VALUE_FX32
 */
//======================================================================
	.macro	PARTICLE_PLAY_WITH_DIR	num, index, start_pos, dir_pos, dir_angle
	.short	EC_PARTICLE_PLAY_WITH_DIR
	.long	\num
	.long	\index
	.long	\start_pos
	.long	\dir_pos
	.long	\dir_angle
	.endm

//======================================================================
/**
 * @brief	�|�P�����ړ�
 *
 * #param_num	7
 * @param	pos			�ړ�������|�P�����̗����ʒu
 * @param	type		�ړ��^�C�v
 * @param	move_pos_x	�ړ���X���W
 * @param	move_pos_y	�ړ���Y���W
 * @param	frame		�ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		�ړ��E�G�C�g
 * @param	count		�����J�E���g�i�ړ��^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A�@�h�䑤�@�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_MOVE_DIRECT	BTLEFF_POKEMON_MOVE_INTERPOLATION	BTLEFF_POKEMON_MOVE_ROUNDTRIP	BTLEFF_POKEMON_MOVE_ROUNDTRIP_LONG
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	POKEMON_MOVE	pos, type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_POKEMON_MOVE
	.long	\pos
	.long	\type
	.long	\move_pos_x
	.long	\move_pos_y
	.long	\frame
	.long	\wait
	.long	\count
	.endm

//======================================================================
/**
 * @brief	�|�P�����g�k
 *
 * #param_num	7
 * @param	pos			�g�k������|�P�����̗����ʒu
 * @param	type		�g�k�^�C�v
 * @param	scale_x		X�����g�k��
 * @param	scale_y		Y�����g�k��
 * @param	frame		�g�k�t���[�����i�ݒ肵���g�k�l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		�g�k�E�G�C�g
 * @param	count		�����J�E���g�i�g�k�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A�@�h�䑤�@�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SCALE_DIRECT	BTLEFF_POKEMON_SCALE_INTERPOLATION	BTLEFF_POKEMON_SCALE_ROUNDTRIP	BTLEFF_POKEMON_SCALE_ROUNDTRIP_LONG
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	POKEMON_SCALE	pos, type, scale_x, scale_y, frame, wait, count
	.short	EC_POKEMON_SCALE
	.long	\pos
	.long	\type
	.long	\scale_x
	.long	\scale_y
	.long	\frame
	.long	\wait
	.long	\count
	.endm

//======================================================================
/**
 * @brief	�|�P������]
 *
 * #param_num	6
 * @param	pos			��]������|�P�����̗����ʒu
 * @param	type		��]�^�C�v
 * @param	rotate		��]�p�x
 * @param	frame		��]�t���[�����i�ݒ肵����]�l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		��]�E�G�C�g
 * @param	count		�����J�E���g�i��]�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A�@�h�䑤�@�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_ROTATE_DIRECT	BTLEFF_POKEMON_ROTATE_INTERPOLATION	BTLEFF_POKEMON_ROTATE_ROUNDTRIP	BTLEFF_POKEMON_ROTATE_ROUNDTRIP_LONG
 * #param	VALUE_FX32
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	POKEMON_ROTATE	pos, type, rotate, frame, wait, count
	.short	EC_POKEMON_ROTATE
	.long	\pos
	.long	\type
	.long	\rotate
	.long	\frame
	.long	\wait
	.long	\count
	.endm

//======================================================================
/**
 * @brief	�|�P�������p�`
 *
 * #param_num	4
 * @param	pos			���p�`������|�P�����̗����ʒu
 * @param	type		���p�`�^�C�v
 * @param	wait		���p�`�E�G�C�g�i���p�`�^�C�v���܂΂����̂Ƃ������L���j
 * @param	count		���p�`�J�E���g�i���p�`�^�C�v���܂΂����̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A�@�h�䑤�@�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	����	�J����	�܂΂���
 * #param	COMBOBOX_VALUE	BTLEFF_MEPACHI_ON	BTLEFF_MEPACHI_OFF	BTLEFF_MEPACHI_MABATAKI
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	POKEMON_SET_MEPACHI_FLAG	pos, type, wait, count
	.short	EC_POKEMON_SET_MEPACHI_FLAG
	.long	\pos
	.long	\type
	.long	\wait
	.long	\count
	.endm

//======================================================================
/**
 * @brief	�|�P�����A�j��
 *
 * #param_num	2
 * @param	pos			�A�j�����삷��|�P�����̗����ʒu
 * @param	flag		�A�j���t���O
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A�@�h�䑤�@�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�A�j���X�g�b�v	�A�j���X�^�[�g
 * #param	COMBOBOX_VALUE	BTLEFF_ANM_STOP	BTLEFF_ANM_START
 */
//======================================================================
	.macro	POKEMON_SET_ANM_FLAG	pos, flag
	.short	EC_POKEMON_SET_ANM_FLAG
	.long	\pos
	.long	\flag
	.endm

//======================================================================
/**
 * @brief	�G�t�F�N�g�I��
 *
 * #param_num	0
 */
//======================================================================
	.macro	SEQ_END
	.short	EC_SEQ_END
	.endm

#endif
