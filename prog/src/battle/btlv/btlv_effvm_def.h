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
#define	BTLV_EFFVM_REG_SIZE		( 8 )		///<�g�p���郌�W�X�^�̐�

//�X�N���v�g�A�h���X�e�[�u���p��`
#define	BTLEFF_AA_SAME			( 0xffffffff )		//�����ʒuAA�Ɠ���
#define	BTLEFF_AA_REVERSE		( 0xfffffffe )		//�����ʒuAA�̔��]
#define	BTLEFF_BB_SAME			( 0xfffffffd )		//�����ʒuBB�Ɠ���
#define	BTLEFF_BB_REVERSE		( 0xfffffffc )		//�����ʒuBB�̔��]
#define	BTLEFF_A_SAME				( 0xfffffffb )		//�����ʒuA�Ɠ���
#define	BTLEFF_A_REVERSE		( 0xfffffffa )		//�����ʒuA�̔��]
#define	BTLEFF_B_SAME				( 0xfffffff9 )		//�����ʒuB�Ɠ���
#define	BTLEFF_B_REVERSE		( 0xfffffff8 )		//�����ʒuB�̔��]
#define	BTLEFF_C_SAME				( 0xfffffff7 )		//�����ʒuC�Ɠ���
#define	BTLEFF_C_REVERSE		( 0xfffffff6 )		//�����ʒuC�̔��]
#define	BTLEFF_D_SAME				( 0xfffffff5 )		//�����ʒuD�Ɠ���
#define	BTLEFF_D_REVERSE		( 0xfffffff4 )		//�����ʒuD�̔��]

//�J�����ړ��^�C�v
#define	BTLEFF_CAMERA_MOVE_DIRECT										( 0 )		//�_�C���N�g
#define	BTLEFF_CAMERA_MOVE_INTERPOLATION						( 1 )		//�Ǐ]
#define	BTLEFF_CAMERA_MOVE_INTERPOLATION_RELATIVITY	( 2 )		//�Ǐ]�i���Ύw��j

//�J�����ړ���
#define	BTLEFF_CAMERA_POS_AA						( 0 )
#define	BTLEFF_CAMERA_POS_BB						( 1 )
#define	BTLEFF_CAMERA_POS_A							( 2 )
#define	BTLEFF_CAMERA_POS_B							( 3 )
#define	BTLEFF_CAMERA_POS_C							( 4 )
#define	BTLEFF_CAMERA_POS_D							( 5 )
#define	BTLEFF_CAMERA_POS_INIT					( 6 )
#define	BTLEFF_CAMERA_POS_ATTACK				( 7 )
#define	BTLEFF_CAMERA_POS_ATTACK_PAIR		( 8 )
#define	BTLEFF_CAMERA_POS_DEFENCE				( 9 )
#define	BTLEFF_CAMERA_POS_DEFENCE_PAIR	( 10 )

#define	BTLEFF_CAMERA_POS_NONE					( 0xffffffff )

//�ˉe���[�h
#define	BTLEFF_CAMERA_PROJECTION_ORTHO					( 0 )
#define	BTLEFF_CAMERA_PROJECTION_PERSPECTIVE		( 1 )

//�p�[�e�B�N���Đ�
#define	BTLEFF_PARTICLE_PLAY_POS_AA				( BTLEFF_CAMERA_POS_AA )
#define	BTLEFF_PARTICLE_PLAY_POS_BB				( BTLEFF_CAMERA_POS_BB )
#define	BTLEFF_PARTICLE_PLAY_POS_A				( BTLEFF_CAMERA_POS_A )
#define	BTLEFF_PARTICLE_PLAY_POS_B				( BTLEFF_CAMERA_POS_B )
#define	BTLEFF_PARTICLE_PLAY_POS_C				( BTLEFF_CAMERA_POS_C )
#define	BTLEFF_PARTICLE_PLAY_POS_D				( BTLEFF_CAMERA_POS_D )
#define	BTLEFF_PARTICLE_PLAY_SIDE_NONE		( BTLEFF_CAMERA_POS_INIT )
#define	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	( BTLEFF_CAMERA_POS_ATTACK )
#define	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	( BTLEFF_CAMERA_POS_DEFENCE )

//�G�~�b�^�ړ�
#define	BTLEFF_EMITTER_MOVE_NONE							( 0 )
#define	BTLEFF_EMITTER_MOVE_STRAIGHT					( 1 )
#define	BTLEFF_EMITTER_MOVE_CURVE							( 2 )

//�|�P��������
#define	BTLEFF_POKEMON_SIDE_ATTACK				( 0 )		//�U����
#define	BTLEFF_POKEMON_SIDE_ATTACK_PAIR		( 1 )		//�U�����y�A
#define	BTLEFF_POKEMON_SIDE_DEFENCE				( 2 )		//�h�䑤
#define	BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	( 3 )		//�h�䑤�y�A
#define	BTLEFF_POKEMON_POS_AA							( 4 )
#define	BTLEFF_POKEMON_POS_BB							( 5 )
#define	BTLEFF_POKEMON_POS_A							( 6 )
#define	BTLEFF_POKEMON_POS_B							( 7 )
#define	BTLEFF_POKEMON_POS_C							( 8 )
#define	BTLEFF_POKEMON_POS_D							( 9 )

#define	BTLEFF_POKEMON_MOVE_DIRECT						( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_MOVE_INTERPOLATION			( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_MOVE_ROUNDTRIP					( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_MOVE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_POKEMON_SCALE_DIRECT						( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_SCALE_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_SCALE_ROUNDTRIP				( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_SCALE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_POKEMON_ROTATE_DIRECT					( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_ROTATE_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_ROTATE_ROUNDTRIP				( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_ROTATE_ROUNDTRIP_LONG	( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_MEPACHI_ON											( MCSS_MEPACHI_ON )
#define	BTLEFF_MEPACHI_OFF										( MCSS_MEPACHI_OFF )
#define	BTLEFF_MEPACHI_MABATAKI								( 2 )
#define	BTLEFF_ANM_STOP												( MCSS_ANM_STOP_ON )
#define	BTLEFF_ANM_START											( MCSS_ANM_STOP_OFF )

//�g���[�i�[����
#define	BTLEFF_TRAINER_MOVE_DIRECT						( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_TRAINER_MOVE_INTERPOLATION			( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_TRAINER_MOVE_ROUNDTRIP					( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_TRAINER_MOVE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

//�G�t�F�N�g�I���҂�
#define	BTLEFF_EFFENDWAIT_ALL									( 0 )
#define	BTLEFF_EFFENDWAIT_CAMERA							( 1 )
#define	BTLEFF_EFFENDWAIT_PARTICLE						( 2 )
#define	BTLEFF_EFFENDWAIT_POKEMON							( 3 )
#define	BTLEFF_EFFENDWAIT_TRAINER							( 4 )

//���䃂�[�h
#define	BTLEFF_CONTROL_MODE_CONTINUE					( 0 )
#define	BTLEFF_CONTROL_MODE_SUSPEND						( 1 )

#define BTLEFF_FX32_SHIFT											( 12 )

//BG�\��/��\���idisplay.h�@bg_sys.h�Œ�`����Ă���l�ɂ��킹��j
#define	BTLEFF_FRAME0_M			( 0 )
#define	BTLEFF_FRAME1_M			( 1 )
#define	BTLEFF_FRAME2_M			( 2 )
#define	BTLEFF_FRAME3_M			( 3 )

#define	BTLEFF_VISIBLE_OFF	( 0 )
#define	BTLEFF_VISIBLE_ON		( 1 )

#endif //__BTLV_EFFVM_DEF_H_

//====================================================================================
/**
 *	VisualStudio�ō쐬�����G�t�F�N�g�G�f�B�^�c�[���ł��g�p�����t�@�C���ł� 
 *	�e���߃}�N����`�̃R�����g����c�[���ŕ\�������R�}���h��������̌^�Ȃǂ𔻒f���܂�
 */
//====================================================================================

#if 0

ex)
 * #param_num	5
 * @param	type			�J�����ړ��^�C�v
 * @param	move_pos	�ړ���
 * @param	frame			�J�����^�C�v���Ǐ]�̂Ƃ��A���t���[���ňړ���ɓ��B���邩���w��
 * @param	wait			�ړ��E�G�C�g
 * @param	brake			�u���[�L�������͂��߂�t���[�����w��
 *
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_DIRECT	BTLEFF_CAMERA_MOVE_INTERPOLATION
 * #param	COMBOBOX_TEXT	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D	�����ʒu	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D	BTLEFF_CAMERA_POS_INIT BTLEFF_CAMERA_POS_ATTACK BTLEFF_CAMERA_POS_DEFENCE
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT

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
	�^�̐錾�̌��ɁA���l���̓_�C�A���O�\���p�̃��x�����L�q�ł��܂�

�EVALUE_INT
	INT�^�̒l�i�c�[����ł́A�����Ȃ��̐��l���͂ɂȂ�܂��j
	�^�̐錾�̌��ɁA���l���̓_�C�A���O�\���p�̃��x�����L�q�ł��܂�

�EVALUE_VECFX32
	VecFx32�^�̒l�@�^�̐錾�̌��Ƀ^�u��؂�ŁA���l���̓_�C�A���O�\���p�̃��x�����L�q�ł��܂�
ex)
	VALUE_VECFX32	camPosX	camPosY	camPosZ
	
	�������邱�Ƃɂ���āA�c�[���̃_�C�A���O�ɁA���͂��邽�߂̒l�̈Ӗ���\�������邱�Ƃ��ł��܂�

�EFILE_DIALOG
�@�t�@�C���_�C�A���O��\�����ăt�@�C���I���𑣂��@�I�����i��g���q���w��ł��܂�

�EFILE_DIALOG_COMBOBOX
�@�t�@�C���_�C�A���O�őI�������t�@�C�����R���{�{�b�N�X�ŕ\�����܂��@�I�����i��g���q���w��ł��܂�

�ECOMBOBOX_HEADER
�@FILE_DIALOG_COMBOBOX�őI�������t�@�C���̊g���q.h�t�@�C����ǂݍ���ŃR���{�{�b�N�X��\�����܂�

#endif

//���߃V���{���錾
//COMMAND_START
#define	EC_CAMERA_MOVE							( 0 )
#define	EC_CAMERA_MOVE_COORDINATE		( 1 )
#define	EC_CAMERA_MOVE_ANGLE				( 2 )
#define	EC_CAMERA_PROJECTION				( 3 )
#define	EC_PARTICLE_LOAD						( 4 )
#define	EC_PARTICLE_PLAY						( 5 )
#define	EC_PARTICLE_PLAY_COORDINATE	( 6 )
#define	EC_PARTICLE_DELETE					( 7 )
#define	EC_EMITTER_MOVE							( 8 )
#define	EC_EMITTER_MOVE_COORDINATE	( 9 )
#define	EC_POKEMON_MOVE							( 10 )
#define	EC_POKEMON_SCALE						( 11 )
#define	EC_POKEMON_ROTATE						( 12 )
#define	EC_POKEMON_SET_MEPACHI_FLAG	( 13 )
#define	EC_POKEMON_SET_ANM_FLAG			( 14 )
#define	EC_POKEMON_PAL_FADE					( 15 )
#define	EC_TRAINER_SET							( 16 )
#define	EC_TRAINER_MOVE							( 17 )
#define	EC_TRAINER_ANIME_SET				( 18 )
#define	EC_TRAINER_DEL							( 19 )
#define	EC_BG_VISIBLE								( 20 )
#define	EC_SE_PLAY									( 21 )
#define	EC_SE_STOP									( 22 )
#define	EC_EFFECT_END_WAIT					( 23 )
#define	EC_WAIT											( 24 )
#define	EC_CONTROL_MODE							( 25 )

//�I���R�}���h�͕K����ԉ��ɂȂ�悤�ɂ���
#define	EC_SEQ_END									( 26 )

#ifndef __C_NO_DEF_

//����Ƀp�f�B���O�������Ȃ��悤�ɂ���
	.option	alignment off

//���߃}�N����`
//======================================================================
/**
 * @brief		�J�����ړ�
 *
 * #param_num	5
 * @param	type			�J�����ړ��^�C�v
 * @param	move_pos	�ړ���
 * @param	frame			�J�����^�C�v���Ǐ]�̂Ƃ��A���t���[���ňړ���ɓ��B���邩���w��
 * @param	wait			�ړ��E�G�C�g
 * @param	brake			�u���[�L�������͂��߂�t���[�����w��
 *
 * #param	COMBOBOX_TEXT	�Ǐ]	�_�C���N�g
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_INTERPOLATION	BTLEFF_CAMERA_MOVE_DIRECT
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	�����ʒu	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_POS_ATTACK BTLEFF_CAMERA_POS_ATTACK_PAIR	BTLEFF_CAMERA_POS_DEFENCE BTLEFF_CAMERA_POS_DEFENCE_PAIR	BTLEFF_CAMERA_POS_INIT	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D
 * #param	VALUE_INT	�ړ��t���[����
 * #param	VALUE_INT	�ړ��E�G�C�g
 * #param	VALUE_INT	�u���[�L�J�n�t���[��
 */
//======================================================================
	.macro	CAMERA_MOVE	type, move_pos, frame, wait, brake
	.short	EC_CAMERA_MOVE
	.long		\type
	.long		\move_pos
	.long		\frame
	.long		\wait
	.long		\brake
	.endm

//======================================================================
/**
 * @brief		�J�����ړ��i���W�w��j
 *
 * #param_num	6
 * @param	type			�J�����ړ��^�C�v
 * @param	move_pos	�ړ���ʒu
 * @param	move_tar	�ړ���^�[�Q�b�g
 * @param	frame			�J�����^�C�v���Ǐ]�̂Ƃ��A���t���[���ňړ���ɓ��B���邩���w��
 * @param	wait			�ړ��E�G�C�g
 * @param	brake			�u���[�L�������͂��߂�t���[�����w��
 *
 * #param	COMBOBOX_TEXT	�Ǐ]	�Ǐ]�i���Ύw��j	�_�C���N�g
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_INTERPOLATION	BTLEFF_CAMERA_MOVE_INTERPOLATION_RELATIVITY	BTLEFF_CAMERA_MOVE_DIRECT
 * #param	VALUE_VECFX32	CamPosX	CamPosY	CamPosZ
 * #param	VALUE_VECFX32	CamTarX	CamTarY	CamTarZ
 * #param	VALUE_INT	�ړ��t���[����
 * #param	VALUE_INT	�ړ��E�G�C�g
 * #param	VALUE_INT	�u���[�L�J�n�t���[��
 */
//======================================================================
	.macro	CAMERA_MOVE_COODINATE	type, pos_x, pos_y, pos_z, tar_x, tar_y, tar_z, frame, wait, brake
	.short	EC_CAMERA_MOVE_COORDINATE
	.long		\type
	.long		\pos_x
	.long		\pos_y
	.long		\pos_z
	.long		\tar_x
	.long		\tar_y
	.long		\tar_z
	.long		\frame
	.long		\wait
	.long		\brake
	.endm

//======================================================================
/**
 * @brief		�J�����ړ��i�p�x�w��j
 *
 * #param_num	6
 * @param	type				�J�����ړ��^�C�v
 * @param	angle_phi		�ړ���Ӂi�Ǐ]�̂Ƃ��́A���ݒl����̑��΁j
 * @param	angle_theta	�ړ���Ɓi�Ǐ]�̂Ƃ��́A���ݒl����̑��΁j
 * @param	frame				�J�����^�C�v���Ǐ]�̂Ƃ��A���t���[���ňړ���ɓ��B���邩���w��
 * @param	wait				�ړ��E�G�C�g
 * @param	brake				�u���[�L�������͂��߂�t���[�����w��
 *
 * #param	COMBOBOX_TEXT	�Ǐ]	�_�C���N�g
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_INTERPOLATION	BTLEFF_CAMERA_MOVE_DIRECT
 * #param	VALUE_INT	�ړ����
 * #param	VALUE_INT	�ړ����
 * #param	VALUE_INT	�ړ��t���[����
 * #param	VALUE_INT	�ړ��E�G�C�g
 * #param	VALUE_INT	�u���[�L�J�n�t���[��
 */
//======================================================================
	.macro	CAMERA_MOVE_ANGLE	type, angle_phi, angle_theta, frame, wait, brake
	.short	EC_CAMERA_MOVE_ANGLE
	.long		\type
	.long		\angle_phi
	.long		\angle_theta
	.long		\frame
	.long		\wait
	.long		\brake
	.endm

//======================================================================
/**
 * @brief		�ˉe���[�h
 *
 * #param_num	1
 * @param	type				�ˉe�^�C�v
 *
 * #param	COMBOBOX_TEXT	���ˉe	�����ˉe
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_PROJECTION_ORTHO	BTLEFF_CAMERA_PROJECTION_PERSPECTIVE
 */
//======================================================================
	.macro	CAMERA_PROJECTION	type
	.short	EC_CAMERA_PROJECTION
	.long		\type
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���f�[�^���[�h
 *
 * #param_num	1
 * @param	datID	�A�[�J�C�u�f�[�^��datID
 *
 * #param	FILE_DIALOG_WITH_ADD	.spa	�p�[�e�B�N���Đ�	�p�[�e�B�N���Đ��i���W�w��j	�G�~�b�^�ړ�
 */
//======================================================================
	.macro	PARTICLE_LOAD	datID
	.short	EC_PARTICLE_LOAD
	.long		\datID
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���Đ�
 *
 * #param_num	6
 * @param	num				�Đ��p�[�e�B�N���i���o�[
 * @param	index			spa���C���f�b�N�X�i���o�[
 * @param	start_pos	�p�[�e�B�N���Đ��J�n�����ʒu
 * @param	dir_pos		�p�[�e�B�N���Đ����������ʒu
 * @param	ofs_y			�p�[�e�B�N���Đ�Y�����I�t�Z�b�g
 * @param	dir_angle	�p�[�e�B�N���Đ�����Y�p�x
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	�U����	�h�䑤	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D
 * #param	COMBOBOX_TEXT	��������	�U����	�h�䑤	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_NONE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D
 * #param	VALUE_FX32	�p�[�e�B�N���Đ�Y�����I�t�Z�b�g
 * #param	VALUE_FX32	�p�[�e�B�N���Đ�����Y�p�x
 */
//======================================================================
	.macro	PARTICLE_PLAY	num, index, start_pos, dir_pos, ofs_y, dir_angle
	.short	EC_PARTICLE_PLAY
	.long		\num
	.long		\index
	.long		\start_pos
	.long		\dir_pos
	.long		\ofs_y
	.long		\dir_angle
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���Đ��i���W�w��j
 *
 * #param_num	6
 * @param	num				�Đ��p�[�e�B�N���i���o�[
 * @param	index			spa���C���f�b�N�X�i���o�[
 * @param	start_pos	�p�[�e�B�N���Đ��J�n�ʒu
 * @param	dir_pos		�p�[�e�B�N���Đ������ʒu
 * @param	ofs_y			�p�[�e�B�N���Đ�Y�����I�t�Z�b�g
 * @param	dir_angle	�p�[�e�B�N���Đ�����Y�p�x
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	VALUE_VECFX32	POS_X	POS_Y	POS_Z
 * #param	VALUE_VECFX32	DIR_X	DIR_Y	DIR_Z
 * #param	VALUE_FX32	�p�[�e�B�N���Đ�Y�����I�t�Z�b�g
 * #param	VALUE_FX32	�p�[�e�B�N���Đ�����Y�p�x
 */
//======================================================================
	.macro	PARTICLE_PLAY_COODINATE	num, index, start_pos_x, start_pos_y, start_pos_z, dir_pos_x, dir_pos_y, dir_pos_z, ofs_y, dir_angle
	.short	EC_PARTICLE_PLAY_COODINATE
	.long		\num
	.long		\index
	.long		\start_pos_x
	.long		\start_pos_y
	.long		\start_pos_z
	.long		\dir_pos_x
	.long		\dir_pos_y
	.long		\dir_pos_z
	.long		\ofs_y
	.long		\dir_angle
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���f�[�^�폜
 *
 * #param_num	1
 * @param	num			�폜����p�[�e�B�N���i���o�[
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 */
//======================================================================
	.macro	PARTICLE_DELETE	num
	.short	EC_PARTICLE_DELETE
	.long		\num
	.endm

//======================================================================
/**
 * @brief	�G�~�b�^�ړ�
 *
 * #param_num	6
 * @param	num					�Đ��p�[�e�B�N���i���o�[
 * @param	index				spa���C���f�b�N�X�i���o�[
 * @param	move_type		�ړ��^�C�v�i�����A�������j
 * @param	start_pos		�ړ��J�n�����ʒu
 * @param	end_pos			�ړ��I�������ʒu
 * @param	move_param	�����ʒuY�����I�t�Z�b�g(ofs_y)	�ړ��t���[��(move_frame)	���������_�i���������̂݁j(top)
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	����	������
 * #param	COMBOBOX_VALUE	BTLEFF_EMITTER_MOVE_STRAIGHT	BTLEFF_EMITTER_MOVE_CURVE
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE
 * #param	VALUE_VECFX32	�����ʒuY�����I�t�Z�b�g	�ړ��t���[��	���������_�i���������̂݁j
 */
//======================================================================
	.macro	EMITTER_MOVE	num, index, move_type, start_pos, end_pos, ofs_y, move_frame, top
	.short	EC_EMITTER_MOVE
	.long		\num
	.long		\index
	.long		\move_type
	.long		\start_pos
	.long		\end_pos
	.long		\ofs_y
	.long		\move_frame
	.long		\top
	.endm

//======================================================================
/**
 * @brief	�G�~�b�^�ړ��i���W�w��j
 *
 * #param_num	6
 * @param	num					�Đ��p�[�e�B�N���i���o�[
 * @param	index				spa���C���f�b�N�X�i���o�[
 * @param	move_type		�ړ��^�C�v�i�����A�������j
 * @param	start_pos		�ړ��J�n���W
 * @param	end_pos			�ړ��I�������ʒu
 * @param	move_param	�����ʒuY�����I�t�Z�b�g(ofs_y)	�ړ��t���[��(move_frame)	���������_�i���������̂݁j(top)
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	����	������
 * #param	COMBOBOX_VALUE	BTLEFF_EMITTER_MOVE_STRAIGHT	BTLEFF_EMITTER_MOVE_CURVE
 * #param	VALUE_VECFX32	X���W	Y���W	Z���W
 * #param	COMBOBOX_TEXT	�U����	�h�䑤 POS_AA POS_BB POS_A POS_B POS_C POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_PARTICLE_PLAY_POS_AA	BTLEFF_PARTICLE_PLAY_POS_BB	BTLEFF_PARTICLE_PLAY_POS_A	BTLEFF_PARTICLE_PLAY_POS_B	BTLEFF_PARTICLE_PLAY_POS_C	BTLEFF_PARTICLE_PLAY_POS_D
 * #param	VALUE_VECFX32	�����ʒuY�����I�t�Z�b�g	�ړ��t���[��	���������_�i���������̂݁j
 */
//======================================================================
	.macro	EMITTER_MOVE_COORDINATE	num, index, move_type, start_pos_x, start_pos_y, start_pos_z, end_pos, ofs_y, move_frame, top
	.short	EC_EMITTER_MOVE_COORDINATE
	.long		\num
	.long		\index
	.long		\move_type
	.long		\start_pos_x
	.long		\start_pos_y
	.long		\start_pos_z
	.long		\end_pos
	.long		\ofs_y
	.long		\move_frame
	.long		\top
	.endm

//======================================================================
/**
 * @brief	�|�P�����ړ�
 *
 * #param_num	7
 * @param	pos					�ړ�������|�P�����̗����ʒu
 * @param	type				�ړ��^�C�v
 * @param	move_pos_x	�ړ���X���W
 * @param	move_pos_y	�ړ���Y���W
 * @param	frame				�ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param	wait				�ړ��E�G�C�g
 * @param	count				�����J�E���g�i�ړ��^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_MOVE_DIRECT	BTLEFF_POKEMON_MOVE_INTERPOLATION	BTLEFF_POKEMON_MOVE_ROUNDTRIP	BTLEFF_POKEMON_MOVE_ROUNDTRIP_LONG
 * #param	VALUE_FX32	�ړ���X���W
 * #param	VALUE_FX32	�ړ���Y���W
 * #param	VALUE_INT		�ړ��t���[����
 * #param	VALUE_INT		�ړ��E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	POKEMON_MOVE	pos, type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_POKEMON_MOVE
	.long		\pos
	.long		\type
	.long		\move_pos_x
	.long		\move_pos_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P�����g�k
 *
 * #param_num	7
 * @param	pos			�g�k������|�P�����̗����ʒu
 * @param	type		�g�k�^�C�v
 * @param	scale_x	X�����g�k��
 * @param	scale_y	Y�����g�k��
 * @param	frame		�g�k�t���[�����i�ݒ肵���g�k�l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		�g�k�E�G�C�g
 * @param	count		�����J�E���g�i�g�k�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SCALE_DIRECT	BTLEFF_POKEMON_SCALE_INTERPOLATION	BTLEFF_POKEMON_SCALE_ROUNDTRIP	BTLEFF_POKEMON_SCALE_ROUNDTRIP_LONG
 * #param	VALUE_FX32	X�����g�k��
 * #param	VALUE_FX32	Y�����g�k��
 * #param	VALUE_INT		�g�k�t���[����
 * #param	VALUE_INT		�g�k�E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	POKEMON_SCALE	pos, type, scale_x, scale_y, frame, wait, count
	.short	EC_POKEMON_SCALE
	.long		\pos
	.long		\type
	.long		\scale_x
	.long		\scale_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P������]
 *
 * #param_num	6
 * @param	pos			��]������|�P�����̗����ʒu
 * @param	type		��]�^�C�v
 * @param	rotate	��]�p�x
 * @param	frame		��]�t���[�����i�ݒ肵����]�l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		��]�E�G�C�g
 * @param	count		�����J�E���g�i��]�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_ROTATE_DIRECT	BTLEFF_POKEMON_ROTATE_INTERPOLATION	BTLEFF_POKEMON_ROTATE_ROUNDTRIP	BTLEFF_POKEMON_ROTATE_ROUNDTRIP_LONG
 * #param	VALUE_FX32	��]�p�x
 * #param	VALUE_INT		��]�t���[�����i�ݒ肵����]�l�܂ŉ��t���[���œ��B���邩�j
 * #param	VALUE_INT		��]�E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	POKEMON_ROTATE	pos, type, rotate, frame, wait, count
	.short	EC_POKEMON_ROTATE
	.long		\pos
	.long		\type
	.long		\rotate
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P�������p�`
 *
 * #param_num	4
 * @param	pos			���p�`������|�P�����̗����ʒu
 * @param	type		���p�`�^�C�v
 * @param	wait		�܂΂����E�G�C�g�i���p�`�^�C�v���܂΂����̂Ƃ������L���j
 * @param	count		�܂΂����J�E���g�i���p�`�^�C�v���܂΂����̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	����	�J����	�܂΂���
 * #param	COMBOBOX_VALUE	BTLEFF_MEPACHI_ON	BTLEFF_MEPACHI_OFF	BTLEFF_MEPACHI_MABATAKI
 * #param	VALUE_INT	�܂΂����E�G�C�g�i�܂΂������L���j
 * #param	VALUE_INT	�܂΂����J�E���g�i�܂΂������L���j
 */
//======================================================================
	.macro	POKEMON_SET_MEPACHI_FLAG	pos, type, wait, count
	.short	EC_POKEMON_SET_MEPACHI_FLAG
	.long		\pos
	.long		\type
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�|�P�����A�j��
 *
 * #param_num	2
 * @param	pos			�A�j�����삷��|�P�����̗����ʒu
 * @param	flag		�A�j���t���O
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�A�j���X�g�b�v	�A�j���X�^�[�g
 * #param	COMBOBOX_VALUE	BTLEFF_ANM_STOP	BTLEFF_ANM_START
 */
//======================================================================
	.macro	POKEMON_SET_ANM_FLAG	pos, flag
	.short	EC_POKEMON_SET_ANM_FLAG
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	�|�P�����p���b�g�t�F�[�h
 *
 * #param_num	5
 * @param	pos				�p���b�g�t�F�[�h����|�P�����̗����ʒu
 * @param	start_evy	START_EVY�l
 * @param	end_evy		END_EVY�l
 * @param	wait			�E�G�C�g
 * @param	rgb				�J�nor�I�����̐F
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D
 * #param	VALUE_INT	START_EVY�l
 * #param	VALUE_INT	END_EVY�l
 * #param	VALUE_INT	�E�G�C�g
 * #param	VALUE_VECFX32	R�l(0-31)	G�l(0-31)	B�l(0-31)
 */
//======================================================================
	.macro	POKEMON_PAL_FADE	pos,	start_evy,	end_evy,	wait,	r, g, b
	.short	EC_POKEMON_PAL_FADE
	.long		\pos
	.long		\start_evy
	.long		\end_evy
	.long		\wait
	.long		( ( ( \b >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 10 ) | ( ( ( \g >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 5 ) | ( ( \r >> BTLEFF_FX32_SHIFT ) & 0x1f )
	.endm

//======================================================================
/**
 * @brief	�g���[�i�[�Z�b�g
 *
 * #param_num	4
 * @param	trtype		�Z�b�g����g���[�i�[�^�C�v
 * @param	position	�Z�b�g����g���[�i�[�̗����ʒu
 * @param	pos_x			�Z�b�g����X���W
 * @param	pos_y			�Z�b�g����Y���W
 *
 * #param	VALUE_INT	�g���[�i�[�^�C�v
 * #param	VALUE_INT	�g���[�i�[�̗����ʒu
 * #param	VALUE_INT	X���W
 * #param	VALUE_INT	Y���W
 */
//======================================================================
	.macro	TRAINER_SET	index, position, pos_x, pos_y
	.short	EC_TRAINER_SET
	.long		\index
	.long		\position
	.long		\pos_x
	.long		\pos_y
	.endm

//======================================================================
/**
 * @brief	�g���[�i�[�ړ�
 *
 * #param_num	7
 * @param	index				�ړ�������g���[�i�[�̗����ʒu
 * @param	type				�ړ��^�C�v
 * @param	move_pos_x	�ړ���X���W
 * @param	move_pos_y	�ړ���Y���W
 * @param	frame				�ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param	wait				�ړ��E�G�C�g
 * @param	count				�����J�E���g�i�ړ��^�C�v�������̂Ƃ������L���j
 *
 * #param	VALUE_INT		�g���[�i�[�̗����ʒu
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_TRAINER_MOVE_DIRECT	BTLEFF_TRAINER_MOVE_INTERPOLATION	BTLEFF_TRAINER_MOVE_ROUNDTRIP	BTLEFF_TRAINER_MOVE_ROUNDTRIP_LONG
 * #param	VALUE_INT		�ړ���X���W
 * #param	VALUE_INT		�ړ���Y���W
 * #param	VALUE_INT		�ړ��t���[����
 * #param	VALUE_INT		�ړ��E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	TRAINER_MOVE	pos, type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_TRAINER_MOVE
	.long		\pos
	.long		\type
	.long		\move_pos_x
	.long		\move_pos_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	�g���[�i�[�A�j���Z�b�g
 *
 * #param_num	2
 * @param	pos			�A�j���Z�b�g����g���[�i�[�̗����ʒu
 * @param	anm_no	�Z�b�g����A�j���i���o�[
 *
 * #param	VALUE_INT		�g���[�i�[�̗����ʒu
 * #param	VALUE_INT		�A�j���i���o�[
 */
//======================================================================
	.macro	TRAINER_ANIME_SET	pos, anm_no
	.short	EC_TRAINER_ANIME_SET
	.long		\pos
	.long		\anm_no
	.endm

//======================================================================
/**
 * @brief	�g���[�i�[�폜
 *
 * #param_num	1
 * @param	index		�폜����g���[�i�[�̗����ʒu
 *
 * #param	VALUE_INT		�g���[�i�[�̗����ʒu
 */
//======================================================================
	.macro	TRAINER_DEL	pos
	.short	EC_TRAINER_DEL
	.long		\pos
	.endm

//======================================================================
/**
 * @brief	BG�̕\��/��\��
 *
 * #param_num	2
 * @param	bg_num	���삷��BG
 * @param	sw			�\��/��\�����w��
 *
 * #param	COMBOBOX_TEXT	BG0	BG1	BG2	BG3
 * #param	COMBOBOX_VALUE	BTLEFF_FRAME0_M	BTLEFF_FRAME1_M	BTLEFF_FRAME2_M	BTLEFF_FRAME3_M
 * #param	COMBOBOX_TEXT	�\��	��\��
 * #param	COMBOBOX_VALUE	BTLEFF_VISIBLE_ON	BTLEFF_VISIBLE_OFF
 */
//======================================================================
	.macro	BG_VISIBLE	bg_num, sw
	.short	EC_BG_VISIBLE
	.long		\bg_num
	.long		\sw
	.endm

//======================================================================
/**
 * @brief	SE�Đ�
 *
 * #param_num	1
 * @param	se_no		�Đ�����SE�i���o�[
 *
 * #param	VALUE_INT		�Đ�����SE�i���o�[
 */
//======================================================================
	.macro	SE_PLAY	se_no
	.short	EC_SE_PLAY
	.long		\se_no
	.endm

//======================================================================
/**
 * @brief	SE�X�g�b�v
 *
 * #param_num	0
 */
//======================================================================
	.macro	SE_STOP
	.short	EC_SE_STOP
	.endm

//======================================================================
/**
 * @brief	�G�t�F�N�g�I���҂�
 *
 * #param_num	1
 * @param	kind	�I���҂�������
 *
 * #param	COMBOBOX_TEXT	���ׂ�	�J����	�p�[�e�B�N��	�|�P����	�g���[�i�[
 * #param COMBOBOX_VALUE	BTLEFF_EFFENDWAIT_ALL	BTLEFF_EFFENDWAIT_CAMERA	BTLEFF_EFFENDWAIT_PARTICLE	BTLEFF_EFFENDWAIT_POKEMON	BTLEFF_EFFENDWAIT_TRAINER
 */
//======================================================================
	.macro	EFFECT_END_WAIT	kind
	.short	EC_EFFECT_END_WAIT
	.long		\kind
	.endm

//======================================================================
/**
 * @brief	�^�C�}�[�ɂ��E�G�C�g
 *
 * #param_num	1
 * @param		wait	�E�G�C�g����
 *
 * #param	VALUE_INT	�E�G�C�g����
 */
//======================================================================
	.macro	WAIT	wait
	.short	EC_WAIT
	.long		\wait
	.endm

//======================================================================
/**
 * @brief	���䃂�[�h�ύX
 *
 * #param_num	1
 * @param		mode	���䃂�[�h�i�p���F���[�h���؂�ւ�邩�A�G�t�F�N�g�I���܂��܂ŏ������p�����Ď��s�@�����F1�s���ƂɎ��s�j
 *
 * #param	COMBOBOX_TEXT	�p��	����
 * #param	COMBOBOX_VALUE	BTLEFF_CONTROL_MODE_CONTINUE	BTLEFF_CONTROL_MODE_SUSPEND
 */
//======================================================================
	.macro	CONTROL_MODE	mode
	.short	EC_CONTROL_MODE
	.long		\mode
	.endm

//======================================================================
/**
 * 	�G�t�F�N�g�I���͂��Ȃ炸�Ō�ɂȂ�悤�ɂ���
 */
//======================================================================
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
