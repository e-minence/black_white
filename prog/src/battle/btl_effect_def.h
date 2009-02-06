//======================================================================
/**
 * @file	btl_effect_def.h
 * @brief	�G�t�F�N�g�V�[�P���X�p�̃}�N����`�t�@�C��
 * @author	HisashiSogabe
 * @date	2009.01.21
 */
//======================================================================

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
 * #param	COMBOBOX_TEXT	�����ʒu	�_�C���N�g	�Ǐ]
 * #param	COMBOBOX_VALUE	CAMERA_MOVE_INIT	CAMERA_MOVE_DIRECT	CAMERA_MOVE_INTERPOLATION
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
�@�t�@�C���_�C�A���O��\�����ăt�@�C���I���𑣂��@�I�����i��g���q�w��ł��܂�

#endif


#define	__ASM_NO_DEF_

	.macro	INIT_CMD
def_cmd_count = 0
	.endm

	.macro	DEF_CMD	symname
\symname = def_cmd_count
def_cmd_count = ( def_cmd_count + 1 )
	.endm

//���߃V���{���錾
	INIT_CMD
	DEF_CMD	WS_CAMERA_MOVE
	DEF_CMD	WS_PARTICLE_LOAD
	DEF_CMD	WS_PARTICLE_PLAY
	DEF_CMD	WS_PARTICLE_PLAY_WITH_POS

//���߃}�N����`
//======================================================================
/**
 * @brief		�J�����ړ�
 *
 * #param_num	4
 * @param	type	�J�����ړ��^�C�v
 * @param	pos_x	�J�����ړ���X���W
 * @param	pos_y	�J�����ړ���Y���W
 * @param	pos_z	�J�����ړ���Z���W
 *
 * #param	COMBOBOX_TEXT	�����ʒu	�_�C���N�g	�Ǐ]
 * #param	COMBOBOX_VALUE	CAMERA_MOVE_INIT	CAMERA_MOVE_DIRECT	CAMERA_MOVE_INTERPOLATION
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 */
//======================================================================
	.macro	CAMERA_MOVE	type, pos_x, pos_y, pos_z
	.long	WS_CAMERA_MOVE
	.long	\pos_x
	.long	\pos_y
	.long	\pos_z
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
	.long	WS_PARTICLE_LOAD
	.long	\datID
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���Đ�
 *
 * #param_num	3
 * @param	num			�Đ��p�[�e�B�N���i���o�[
 * @param	start_pos	�p�[�e�B�N���Đ��J�n�����ʒu
 * @param	dir_pos		�p�[�e�B�N���Đ����������ʒu
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 * #param	COMBOBOX_TEXT	�U����	�h�䑤
 */
//======================================================================
	.macro	PARTICLE_PLAY	pos_x, pos_y, pos_z, angle
	.long	WS_PARTICLE_PLAY
	.long	\num
	.long	\start_pos
	.long	\dir_pos
	.endm

//======================================================================
/**
 * @brief	�p�[�e�B�N���Đ��i���W�w�肠��j
 *
 * #param_num	5
 * @param	num		�Đ��p�[�e�B�N���i���o�[
 * @param	pos_x	�p�[�e�B�N���Đ��ʒuX���W
 * @param	pos_y	�p�[�e�B�N���Đ��ʒuY���W
 * @param	pos_z	�p�[�e�B�N���Đ��ʒuZ���W
 * @param	angle	�p�[�e�B�N���p�x
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 */
//======================================================================
	.macro	PARTICLE_PLAY_WITH_POS	num, pos_x, pos_y, pos_z, angle
	.long	WS_PARTICLE_PLAY_WITH_POS
	.long	\num
	.long	\pos_x
	.long	\pos_y
	.long	\pos_z
	.long	\angle
	.endm

