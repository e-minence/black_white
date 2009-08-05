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

//�J�������
#define	BTLEFF_CAMERA_SHAKE_VERTICAL		( 0 )
#define	BTLEFF_CAMERA_SHAKE_HORIZON		  ( 1 )

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
#define	BTLEFF_EMITTER_MOVE_CURVE_HALF				( 3 )

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
#define	BTLEFF_POKEMON_POS_E					    ( 10 )
#define	BTLEFF_POKEMON_POS_F							( 11 )
#define	BTLEFF_TRAINER_POS_AA					    ( 12 )
#define	BTLEFF_TRAINER_POS_BB					    ( 13 )
#define	BTLEFF_TRAINER_POS_A					    ( 14 )
#define	BTLEFF_TRAINER_POS_B					    ( 15 )
#define	BTLEFF_TRAINER_POS_C					    ( 16 )
#define	BTLEFF_TRAINER_POS_D					    ( 17 )

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
#define	BTLEFF_POKEMON_ALPHA_DIRECT					  ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_ALPHA_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_ALPHA_ROUNDTRIP				( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_ALPHA_ROUNDTRIP_LONG	  ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_MEPACHI_ON											( MCSS_MEPACHI_ON )
#define	BTLEFF_MEPACHI_OFF										( MCSS_MEPACHI_OFF )
#define	BTLEFF_MEPACHI_MABATAKI								( 2 )
#define	BTLEFF_ANM_STOP												( MCSS_ANM_STOP_ON )
#define	BTLEFF_ANM_START											( MCSS_ANM_STOP_OFF )

//�|�P�����~�ړ�
#define BTLEFF_AXIS_X_L                       ( 0 )     //�w����
#define BTLEFF_AXIS_X_R                       ( 1 )     //�w���E
#define BTLEFF_AXIS_Y_L                       ( 2 )     //�x����
#define BTLEFF_AXIS_Y_R                       ( 3 )     //�x���E
#define BTLEFF_AXIS_Z_L                       ( 4 )     //�y����
#define BTLEFF_AXIS_Z_R                       ( 5 )     //�y���E

#define BTLEFF_SHIFT_H_P                      ( 0 )     //�V�t�g�g�{
#define BTLEFF_SHIFT_H_M                      ( 1 )     //�V�t�g�g�|
#define BTLEFF_SHIFT_V_P                      ( 2 )     //�V�t�g�u�{
#define BTLEFF_SHIFT_V_M                      ( 3 )     //�V�t�g�u�|

//�g���[�i�[����
#define	BTLEFF_TRAINER_MOVE_DIRECT						( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_TRAINER_MOVE_INTERPOLATION			( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_TRAINER_MOVE_ROUNDTRIP					( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_TRAINER_MOVE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )


//SE�Đ��v���[���[��`
#define BTLEFF_SEPLAY_DEFAULT                 ( 0 )
#define BTLEFF_SEPLAY_SE1                     ( 1 )
#define BTLEFF_SEPLAY_SE2                     ( 2 )
#define BTLEFF_SEPLAY_PSG                     ( 3 )

//SE�X�g�b�v�v���[���[��`
#define BTLEFF_SESTOP_ALL                     ( 0 )
#define BTLEFF_SESTOP_SE1                     ( 1 )
#define BTLEFF_SESTOP_SE2                     ( 2 )
#define BTLEFF_SESTOP_PSG                     ( 3 )

//�G�t�F�N�g�I���҂�
#define	BTLEFF_EFFENDWAIT_ALL									( 0 )
#define	BTLEFF_EFFENDWAIT_CAMERA							( 1 )
#define	BTLEFF_EFFENDWAIT_PARTICLE						( 2 )
#define	BTLEFF_EFFENDWAIT_POKEMON							( 3 )
#define	BTLEFF_EFFENDWAIT_TRAINER							( 4 )
#define	BTLEFF_EFFENDWAIT_PALFADE_STAGE		  	( 5 )
#define	BTLEFF_EFFENDWAIT_PALFADE_FIELD		  	( 6 )
#define	BTLEFF_EFFENDWAIT_PALFADE_3D			    ( 7 )
#define	BTLEFF_EFFENDWAIT_PALFADE_EFFECT	    ( 8 )
#define BTLEFF_EFFENDWAIT_SEALL               ( 9 )
#define BTLEFF_EFFENDWAIT_SE1                 ( 10 )
#define BTLEFF_EFFENDWAIT_SE2                 ( 11 )

//���䃂�[�h
#define	BTLEFF_CONTROL_MODE_CONTINUE					( 0 )
#define	BTLEFF_CONTROL_MODE_SUSPEND						( 1 )

#define BTLEFF_FX32_SHIFT											( 12 )

//BG�p���b�g�t�F�[�h�Ώے�`
#define BTLEFF_PAL_FADE_STAGE                 ( 0 )
#define BTLEFF_PAL_FADE_FIELD                 ( 1 )
#define BTLEFF_PAL_FADE_3D                    ( 2 )
#define BTLEFF_PAL_FADE_EFFECT                ( 3 )
#define BTLEFF_PAL_FADE_ALL                   ( 4 )

//BG�\��/��\��
#define	BTLEFF_STAGE			( 0 )
#define	BTLEFF_FIELD			( 1 )
#define	BTLEFF_EFFECT			( 2 )

#define	BTLEFF_VANISH_OFF	( 0 )
#define	BTLEFF_VANISH_ON	( 1 )

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
 
�EVALUE_ANGLE
	�p�x���͗p�̒l�i0�`360�j
  �R���o�[�^�Łi0�`0xffff�j�ɕϊ����܂�

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
#define	EC_CAMERA_SHAKE				      ( 3 )
#define	EC_CAMERA_PROJECTION				( 4 )
#define	EC_PARTICLE_LOAD						( 5 )
#define	EC_PARTICLE_PLAY						( 6 )
#define	EC_PARTICLE_PLAY_COORDINATE	( 7 )
#define	EC_PARTICLE_DELETE					( 8 )
#define	EC_EMITTER_MOVE							( 9 )
#define	EC_EMITTER_MOVE_COORDINATE	( 10 )
#define	EC_POKEMON_MOVE							( 11 )
#define	EC_POKEMON_CIRCLE_MOVE			( 12 )
#define	EC_POKEMON_SCALE						( 13 )
#define	EC_POKEMON_ROTATE						( 14 )
#define	EC_POKEMON_ALPHA						( 15 )
#define	EC_POKEMON_SET_MEPACHI_FLAG	( 16 )
#define	EC_POKEMON_SET_ANM_FLAG			( 17 )
#define	EC_POKEMON_PAL_FADE					( 18 )
#define	EC_POKEMON_VANISH					  ( 19 )
#define	EC_POKEMON_SHADOW_VANISH	  ( 20 )
#define	EC_TRAINER_SET							( 21 )
#define	EC_TRAINER_MOVE							( 22 )
#define	EC_TRAINER_ANIME_SET				( 23 )
#define	EC_TRAINER_DEL							( 24 )
#define	EC_BG_PAL_FADE		   				( 25 )
#define	EC_BG_VANISH								( 26 )
#define	EC_SE_PLAY									( 27 )
#define	EC_SE_STOP									( 28 )
#define	EC_SE_PITCH									( 29 )
#define	EC_EFFECT_END_WAIT					( 30 )
#define	EC_WAIT											( 31 )
#define	EC_CONTROL_MODE							( 32 )

//�I���R�}���h�͕K����ԉ��ɂȂ�悤�ɂ���
#define	EC_SEQ_END									( 33 )

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
 * @brief		�J�������
 *
 * #param_num	6
 * @param	dir     ������
 * @param	value   ��ꕝ
 * @param	offset	��ꕝ�I�t�Z�b�g
 * @param	frame   �P���ɂ�����t���[��
 * @param	wait    �P�t���[�����̃E�G�C�g
 * @param	count   ����
 *
 * #param	COMBOBOX_TEXT	�c���	�����
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_SHAKE_VERTICAL	BTLEFF_CAMERA_SHAKE_HORIZON
 * #param	VALUE_FX32	��ꕝ
 * #param	VALUE_FX32	��ꕝ�I�t�Z�b�g�i�P��ꂲ�Ɓj
 * #param	VALUE_INT	�P���ɂ�����t���[��
 * #param	VALUE_INT	�P�t���[�����̃E�G�C�g
 * #param	VALUE_INT	����
 */
//======================================================================
	.macro	CAMERA_SHAKE	dir, value, offset, frame, wait, count
	.short	EC_CAMERA_SHAKE
	.long		\dir
	.long		\value
	.long		\offset
	.long		\frame
	.long		\wait
	.long		\count
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
	.macro	PARTICLE_PLAY_COORDINATE	num, index, start_pos_x, start_pos_y, start_pos_z, dir_pos_x, dir_pos_y, dir_pos_z, ofs_y, dir_angle
	.short	EC_PARTICLE_PLAY_COORDINATE
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
 * #param	COMBOBOX_TEXT	����	������  �������i�����j
 * #param	COMBOBOX_VALUE	BTLEFF_EMITTER_MOVE_STRAIGHT	BTLEFF_EMITTER_MOVE_CURVE BTLEFF_EMITTER_MOVE_CURVE_HALF
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
 * #param	COMBOBOX_TEXT	����	������  �������i�����j
 * #param	COMBOBOX_VALUE	BTLEFF_EMITTER_MOVE_STRAIGHT	BTLEFF_EMITTER_MOVE_CURVE BTLEFF_EMITTER_MOVE_CURVE_HALF
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
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
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
 * @brief	�|�P�����~�ړ�
 *
 * #param_num	7
 * @param	pos				      	�~�ړ�������|�P�����̗����ʒu
 * @param	axis			      	��]��
 * @param	shift			      	��]�V�t�g
 * @param	radius_h	        ���������a
 * @param	radius_v	        �c�������a
 * @param	rotate_param			��]�t���[�����i1��]���t���[���ł��邩�j:�ړ��E�G�C�g:��]�J�E���g
 * @param	rotate_after_wait 1��]�������Ƃ̃E�G�C�g�i2��]�ȏ�ňӖ��̂���l�ł��j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�w����  �w���E  �x����  �x���E  �y����  �y���E
 * #param	COMBOBOX_VALUE	BTLEFF_AXIS_X_L BTLEFF_AXIS_X_R BTLEFF_AXIS_Y_L BTLEFF_AXIS_Y_R BTLEFF_AXIS_Z_L BTLEFF_AXIS_Z_R
 * #param	COMBOBOX_TEXT �V�t�g�g�{  �V�t�g�g�|  �V�t�g�u�{  �V�t�g�u�|
 * #param	COMBOBOX_VALUE	BTLEFF_SHIFT_H_P BTLEFF_SHIFT_H_M BTLEFF_SHIFT_V_P BTLEFF_SHIFT_V_M
 * #param	VALUE_FX32	���������a
 * #param	VALUE_FX32	�c�������a
 * #param	VALUE_VECFX32		��]�t���[����  �ړ��E�G�C�g  ��]�J�E���g
 * #param	VALUE_INT 1��]�������Ƃ̃E�G�C�g�i2��]�ȏ�ňӖ��̂���l�ł��j
 */
//======================================================================
	.macro	POKEMON_CIRCLE_MOVE	pos, axis, shift, radius_h, radius_v, frame, rotate_wait, count, rotate_after_wait
	.short	EC_POKEMON_CIRCLE_MOVE
	.long		\pos
	.long		\axis
	.long		\shift
	.long		\radius_h
	.long		\radius_v
	.long		\frame
	.long		\rotate_wait
	.long		\count
	.long		\rotate_after_wait
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
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
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
 * #param	VALUE_ANGLE	��]�p�x
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
 * @brief	�|�P�������l
 *
 * #param_num	6
 * @param	pos			���l�Z�b�g����|�P�����̗����ʒu
 * @param	type		���l�^�C�v
 * @param	alpha	  ���l
 * @param	frame		�t���[�����i�ݒ肵�����l�܂ŉ��t���[���œ��B���邩�j
 * @param	wait		�E�G�C�g
 * @param	count		�����J�E���g�i���l�^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_ROTATE_DIRECT	BTLEFF_POKEMON_ROTATE_INTERPOLATION	BTLEFF_POKEMON_ROTATE_ROUNDTRIP	BTLEFF_POKEMON_ROTATE_ROUNDTRIP_LONG
 * #param	VALUE_INT	  ���l
 * #param	VALUE_INT		�t���[�����i�ݒ肵�����l�܂ŉ��t���[���œ��B���邩�j
 * #param	VALUE_INT		�E�G�C�g
 * #param	VALUE_INT		�����J�E���g�i�������L���j
 */
//======================================================================
	.macro	POKEMON_ALPHA	pos, type, alpha, frame, wait, count
	.short	EC_POKEMON_ALPHA
	.long		\pos
	.long		\type
	.long		\alpha
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
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
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
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
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
 * @brief	�|�P�����o�j�b�V��
 *
 * #param_num	2
 * @param	pos			�o�j�b�V�����삷��|�P�����̗����ʒu
 * @param	flag		�o�j�b�V���t���O
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�o�j�b�V���I��	�o�j�b�V���I�t
 * #param	COMBOBOX_VALUE	BTLEFF_VANISH_ON	BTLEFF_VANISH_OFF
 */
//======================================================================
	.macro	POKEMON_VANISH	pos, flag
	.short	EC_POKEMON_VANISH
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	�|�P�����e�o�j�b�V��
 *
 * #param_num	2
 * @param	pos			�o�j�b�V�����삷��|�P�����̗����ʒu
 * @param	flag		�o�j�b�V���t���O
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A	�h�䑤	�h�䑤�y�A	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	�o�j�b�V���I��	�o�j�b�V���I�t
 * #param	COMBOBOX_VALUE	BTLEFF_VANISH_ON	BTLEFF_VANISH_OFF
 */
//======================================================================
	.macro	POKEMON_SHADOW_VANISH	pos, flag
	.short	EC_POKEMON_SHADOW_VANISH
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	�g���[�i�[�Z�b�g
 *
 * #param_num	3
 * @param	trtype		�Z�b�g����g���[�i�[�^�C�v
 * @param	position	�Z�b�g����g���[�i�[�̗����ʒu
 * @param	pos			  �Z�b�g������W(X:Y:Z)
 *
 * #param	VALUE_INT	�g���[�i�[�^�C�v
 * #param	COMBOBOX_TEXT POS_AA  POS_BB  POS_A POS_B POS_C POS_D
 * #param	COMBOBOX_VALUE  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	VALUE_VECFX32	X���W Y���W Z���W
 */
//======================================================================
	.macro	TRAINER_SET	index, position, pos_x, pos_y, pos_z
	.short	EC_TRAINER_SET
	.long		\index
	.long		\position
	.long		\pos_x
	.long		\pos_y
	.long		\pos_z
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
 * @brief	BG�̃p���b�g�t�F�[�h
 *
 * #param_num	5
 * @param	bg_num    ���삷��BG
 * @param	start_evy	START_EVY�l
 * @param	end_evy		END_EVY�l
 * @param	wait			�E�G�C�g
 * @param	rgb				�J�nor�I�����̐F
 *
 * #param	COMBOBOX_TEXT ���~  �t�B�[���h  ���~�{�t�B�[���h  �G�t�F�N�g��  ���ׂ�
 * #param	COMBOBOX_VALUE  BTLEFF_PAL_FADE_STAGE BTLEFF_PAL_FADE_FIELD BTLEFF_PAL_FADE_3D  BTLEFF_PAL_FADE_EFFECT BTLEFF_PAL_FADE_ALL
 * #param	VALUE_INT	START_EVY�l
 * #param	VALUE_INT	END_EVY�l
 * #param	VALUE_INT	�E�G�C�g
 * #param	VALUE_VECFX32	R�l(0-31)	G�l(0-31)	B�l(0-31)
 */
//======================================================================
	.macro	BG_PAL_FADE bg_num, start_evy, end_evy, wait, r, g, b
	.short	EC_BG_PAL_FADE
	.long		\bg_num
	.long		\start_evy
	.long		\end_evy
	.long		\wait
	.long		( ( ( \b >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 10 ) | ( ( ( \g >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 5 ) | ( ( \r >> BTLEFF_FX32_SHIFT ) & 0x1f )
	.endm

//======================================================================
/**
 * @brief	BG�̕\��/��\��
 *
 * #param_num	2
 * @param	bg_num	���삷��BG
 * @param	sw			�\��/��\�����w��
 *
 * #param	COMBOBOX_TEXT	���~  �t�B�[���h  �G�t�F�N�g
 * #param	COMBOBOX_VALUE  BTLEFF_STAGE  BTLEFF_FIELD  BTLEFF_EFFECT
 * #param	COMBOBOX_TEXT ��\��  �\��
 * #param	COMBOBOX_VALUE  BTLEFF_VANISH_ON BTLEFF_VANISH_OFF
 */
//======================================================================
	.macro	BG_VISIBLE	bg_num, sw
	.short	EC_BG_VANISH
	.long		\bg_num
	.long		\sw
	.endm

//======================================================================
/**
 * @brief	SE�Đ�
 *
 * #param_num	7
 * @param	se_no	      �Đ�����SE�i���o�[
 * @param player      �Đ�����PlayerNo
 * @param wait        �Đ��܂ł̃E�G�C�g
 * @param pitch       �Đ��s�b�`
 * @param vol         �Đ��{�����[��
 * @param mod_depth   �Đ����W�����[�V�����f�v�X
 * @param mod_speed   �Đ����W�����[�V�����X�s�[�h
 *
 * #param	VALUE_INT   �Đ�����SE�i���o�[
 * #param COMBOBOX_TEXT �f�t�H���g  SE1 SE2 PSG
 * #param COMBOBOX_VALUE BTLEFF_SEPLAY_DEFAULT  BTLEFF_SEPLAY_SE1 BTLEFF_SEPLAY_SE2 BTLEFF_SEPLAY_PSG
 * #param VALUE_INT   �Đ��܂ł̃E�G�C�g
 * #param VALUE_INT   �Đ��s�b�`
 * #param VALUE_INT   �Đ��{�����[��
 * #param VALUE_INT   �Đ����W�����[�V�����f�v�X
 * #param VALUE_INT   �Đ����W�����[�V�����X�s�[�h
 */
//======================================================================
	.macro	SE_PLAY	se_no, player, wait, pitch, vol, mod_depth, mod_speed
	.short	EC_SE_PLAY
	.long		\se_no
	.long   \player
	.long   \wait
  .long   \pitch
  .long   \vol
  .long   \mod_depth
  .long   \mod_speed
	.endm

//======================================================================
/**
 * @brief	SE�X�g�b�v
 *
 * #param_num	1
 * @param player  �X�g�b�v����PlayerNo
 *
 * #param COMBOBOX_TEXT ���ׂ�  SE1 SE2 PSG
 * #param COMBOBOX_VALUE BTLEFF_SESTOP_ALL  BTLEFF_SESTOP_SE1 BTLEFF_SESTOP_SE2 BTLEFF_SESTOP_PSG
 */
//======================================================================
	.macro	SE_STOP player
	.short	EC_SE_STOP
  .long   \player
	.endm

//======================================================================
/**
 * @brief	SE�s�b�`�ύX
 *
 * #param_num	1
 * @param player  �ύX����PlayerNo
 * @param start   �J�n�s�b�`
 * @param end     �I���s�b�`
 *
 * #param COMBOBOX_TEXT SE1 SE2
 * #param COMBOBOX_VALUE BTLEFF_SEPLAY_SE1 BTLEFF_SEPLAY_SE2
 */
//======================================================================
  .macro  SE_PITCH player, start, end
  .short  EC_SE_PITCH	
  .long   \player
  .long   \start
  .long   \end
  .endm

//======================================================================
/**
 * @brief	�G�t�F�N�g�I���҂�
 *
 * #param_num	1
 * @param	kind	�I���҂�������
 *
 * #param	COMBOBOX_TEXT	���ׂ�	�J����	�p�[�e�B�N��	�|�P����	�g���[�i�[  PALFADE���~  PALFADE�t�B�[���h PALFADE���~�{�t�B�[���h  PALFADE�G�t�F�N�g SEPLAY���ׂ�  SEPLAY1 SEPLAY2  
 * #param COMBOBOX_VALUE	BTLEFF_EFFENDWAIT_ALL	BTLEFF_EFFENDWAIT_CAMERA	BTLEFF_EFFENDWAIT_PARTICLE	BTLEFF_EFFENDWAIT_POKEMON	BTLEFF_EFFENDWAIT_TRAINER BTLEFF_EFFENDWAIT_PALFADE_STAGE  BTLEFF_EFFENDWAIT_PALFADE_FIELD  BTLEFF_EFFENDWAIT_PALFADE_3D BTLEFF_EFFENDWAIT_PALFADE_EFFECT BTLEFF_EFFENDWAIT_SEALL  BTLEFF_EFFENDWAIT_SE1  BTLEFF_EFFENDWAIT_SE2  
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
