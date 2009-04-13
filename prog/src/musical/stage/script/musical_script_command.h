//======================================================================
/**
 * @file	musical_script_command.h.h
 * @brief	�~���[�W�J���X�N���v�g�p�̃}�N����`�t�@�C��
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 */
//======================================================================

#ifndef MUSICAL_SCRIPT_COMMAND_H__
#define MUSICAL_SCRIPT_COMMAND_H__


#endif MUSICAL_SCRIPT_COMMAND_H__

//���߃V���{���錾
//COMMAND_START
#define SCRIPT_ENUM_ScriptFinish 	 (0)
#define SCRIPT_ENUM_FrameWait 	 (1)
#define SCRIPT_ENUM_FrameWaitTime 	 (2)
#define SCRIPT_ENUM_CurtainUp 	 (3)
#define SCRIPT_ENUM_CurtainDown 	 (4)
#define SCRIPT_ENUM_CurtainMove 	 (5)
#define SCRIPT_ENUM_StageMove 	 (6)
#define SCRIPT_ENUM_StageChangeBg 	 (7)
#define SCRIPT_ENUM_PokeShow 	 (8)
#define SCRIPT_ENUM_PokeDir 	 (9)
#define SCRIPT_ENUM_PokeMove 	 (10)
#define SCRIPT_ENUM_PokeStopAnime 	 (11)
#define SCRIPT_ENUM_PokeStartAnime 	 (12)
#define SCRIPT_ENUM_PokeChangeAnime 	 (13)
#define SCRIPT_ENUM_PokeActionJump 	 (14)
#define SCRIPT_ENUM_ObjectCreate 	 (15)
#define SCRIPT_ENUM_ObjectDelete 	 (16)
#define SCRIPT_ENUM_ObjectShow 	 (17)
#define SCRIPT_ENUM_ObjectHide 	 (18)
#define SCRIPT_ENUM_ObjectMove 	 (19)
#define SCRIPT_ENUM_EffectCreate 	 (20)
#define SCRIPT_ENUM_EffectDelete 	 (21)
#define SCRIPT_ENUM_EffectStart 	 (22)
#define SCRIPT_ENUM_EffectStop 	 (23)
#define SCRIPT_ENUM_EffectRepeatStart 	 (24)
#define SCRIPT_ENUM_LightShowCircle 	 (25)
#define SCRIPT_ENUM_LightHide 	 (26)
#define SCRIPT_ENUM_LightMove 	 (27)
#define SCRIPT_ENUM_LightColor 	 (28)
#define SCRIPT_ENUM_MessageShow 	 (29)
#define SCRIPT_ENUM_MessageHide 	 (30)
#define SCRIPT_ENUM_MessageColor 	 (31)
#define SEQ_END 	 (32)

#ifndef __C_NO_DEF_

//����Ƀp�f�B���O�������Ȃ��悤�ɂ���
	.option	alignment off
	
#pragma mark [>System Command
//���߃}�N����`
//======================================================================
/**
 * @brief	�V�X�e���F�~���[�W�J���I��
 *
 * #param_num	0
 */
//======================================================================
	.macro	ComScriptFinish
	.short	SCRIPT_ENUM_ScriptFinish)
	.endm

//======================================================================
/**
 * @brief	�V�X�e���F�w��t���[���E�F�C�g
 *
 * #param_num	1
 * @param	frame	�t���[����
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComFrameWait	frame
	.short	SCRIPT_ENUM_FrameWait
	.long	\frame
	.endm

//======================================================================
/**
 * @brief	�V�X�e���F�w��܂Ńt���[���E�F�C�g
 *
 * #param_num	1
 * @param	frame	�t���[����
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComFrameWaitTime	frame
	.short	SCRIPT_ENUM_FrameWaitTime
	.long	\frame
	.endm

#pragma mark [>Curtain Command
//======================================================================
/**
 * @brief	�J�[�e���F�����グ��(�Œ葬�x
 *
 * #param_num	0
 */
//======================================================================
	.macro	ComCurtainUp
	.short	SCRIPT_ENUM_CurtainUp
	.endm

//======================================================================
/**
 * @brief	�J�[�e���F����������(�Œ葬�x
 *
 * #param_num	0
 */
//======================================================================
	.macro	ComCurtainDown
	.short	SCRIPT_ENUM_CurtainDown
	.endm

//======================================================================
/**
 * @brief	�J�[�e���F���𓮂���(���x�ݒ��
 *
 * #param_num	2
 * @param	frame	�t���[����
 * @param	heaight	Y���W
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComCurtainMove	frame	height
	.short	SCRIPT_ENUM_CurtainMove
	.long	\frame
	.long	\height
	.endm

#pragma mark [>Stage Command
//======================================================================
/**
 * @brief	�X�e�[�W�F����̃X�N���[��
 *
 * #param_num	2
 * @param	frame	�t���[����
 * @param	pos		X���W
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComStageMove	frame	pos
	.short	SCRIPT_ENUM_StageMove
	.long	\frame
	.long	\pos
	.endm

//======================================================================
/**
 * @brief	�X�e�[�W�F�w�i�̓ǂݑւ�
 *
 * #param_num	1
 * @param	bgNo	�w�i�ԍ�
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComStageChangeBg	bgNo
	.short	SCRIPT_ENUM_StageChangeBg
	.long	\bgNo
	.endm

#pragma mark [>Pokemon Command
//======================================================================
/**
 * @brief	�|�P�����F�\���ؑ�
 *
 * #param_num	2
 * @param	pokeNo	�|�P�����ԍ�
 * @param	flg		ON/OFF
 *
 * #param	VALUE_INT
 * #param	COMBOBOX_TEXT	ON	OFF
 * #param	COMBOBOX_VALUE	1	0
 */
//======================================================================
	.macro	ComPokeShow	pokeNo	flg
	.short	SCRIPT_ENUM_PokeShow
	.long	\pokeNo
	.long	\flg
	.endm

//======================================================================
/**
 * @brief	�|�P�����F�����ݒ�
 *
 * #param_num	2
 * @param	pokeNo	�|�P�����ԍ�
 * @param	dir		����(���F�E)
 *
 * #param	VALUE_INT
 * #param	COMBOBOX_TEXT	��	�E
 * #param	COMBOBOX_VALUE	0	1
 */
//======================================================================
	.macro	ComPokeDir	pokeNo	dir
	.short	SCRIPT_ENUM_PokeDir
	.long	\pokeNo
	.long	\dir
	.endm

//======================================================================
/**
 * @brief	�|�P�����F�ړ�
 *
 * #param_num	5
 * @param	pokeNo	�|�P�����ԍ�
 * @param	frame	�t���[����
 * @param	posX	�w���W
 * @param	posY	�x���W
 * @param	posZ	�y���W
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 */
//======================================================================
	.macro	ComPokeMove	pokeNo	frame	posX	posY	posZ
	.short	SCRIPT_ENUM_PokeMove
	.long	\pokeNo
	.long	\frame
	.long	\posX
	.long	\posY
	.long	\posZ
	.endm

//======================================================================
/**
 * @brief	�|�P�����F�A�j����~
 *
 * #param_num	1
 * @param	pokeNo	�|�P�����ԍ�
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComPokeStopAnime	pokeNo
	.short	SCRIPT_ENUM_PokeStopAnime
	.long	\pokeNo
	.endm

//======================================================================
/**
 * @brief	�|�P�����F�A�j���J�n
 *
 * #param_num	1
 * @param	pokeNo	�|�P�����ԍ�
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComPokeStartAnime	pokeNo
	.short	SCRIPT_ENUM_PokeStartAnime
	.long	\pokeNo
	.endm

//======================================================================
/**
 * @brief	�|�P�����F�A�j���ύX
 *
 * #param_num	2
 * @param	pokeNo	�|�P�����ԍ�
 * @param	animeNo	�A�j���ԍ�
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComPokeChangeAnime	pokeNo	animeNo
	.short	SCRIPT_ENUM_PokeChangeAnime
	.long	\pokeNo
	.long	\animeNo
	.endm

#pragma mark [>Pokemon Action Command
//======================================================================
/**
 * @brief	�|�P�����A�N�V�����F���˂�
 *
 * #param_num	4
 * @param	pokeNo	�|�P�����ԍ�
 * @param	interval	�Ԋu
 * @param	repeat	��
 * @param	height	����
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_FX32
 */
//======================================================================
	.macro	ComPokeActionJump	pokeNo	interval	repeat	height
	.short	SCRIPT_ENUM_PokeActionJump
	.long	\pokeNo
	.long	\interval
	.long	\repeat
	.long	\height
	.endm

#pragma mark [>Object Command
//======================================================================
/**
 * @brief	�I�u�W�F�N�g�F�쐬
 *
 * #param_num	2
 * @param	objNo	�I�u�W�F�N�g�Ǘ��ԍ�
 * @param	type	�I�u�W�F�N�g�ԍ�
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComObjectCreate	objNo	type
	.short	SCRIPT_ENUM_ObjectCreate
	.long	\objNo
	.long	\type
	.endm

//======================================================================
/**
 * @brief	�I�u�W�F�N�g�F�폜
 *
 * #param_num	1
 * @param	objNo	�I�u�W�F�N�g�Ǘ��ԍ�
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComObjectDelete	objNo
	.short	SCRIPT_ENUM_ObjectDelete
	.long	\objNo
	.endm

//======================================================================
/**
 * @brief	�I�u�W�F�N�g�F�\��
 *
 * #param_num	1
 * @param	objNo	�I�u�W�F�N�g�Ǘ��ԍ�
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComObjectShow	objNo
	.short	SCRIPT_ENUM_ObjectShow
	.long	\objNo
	.endm

//======================================================================
/**
 * @brief	�I�u�W�F�N�g�F��\��
 *
 * #param_num	1
 * @param	objNo	�I�u�W�F�N�g�Ǘ��ԍ�
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComObjectHide	objNo
	.short	SCRIPT_ENUM_ObjectHide
	.long	\objNo
	.endm

//======================================================================
/**
 * @brief	�I�u�W�F�N�g�F�ړ�
 *
 * #param_num	5
 * @param	objNo	�I�u�W�F�N�g�Ǘ��ԍ�
 * @param	frame	�t���[����
 * @param	posX	�w���W
 * @param	posY	�x���W
 * @param	posZ	�y���W
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 */
//======================================================================
	.macro	ComObjectMove	objNo	frame	posX	posY	posZ
	.short	SCRIPT_ENUM_ObjectMove
	.long	\objNo
	.long	\frame
	.long	\posX
	.long	\posY
	.long	\posZ
	.endm


#pragma mark [>Effect Command
//======================================================================
/**
 * @brief	�G�t�F�N�g�F�쐬
 *
 * #param_num	2
 * @param	effNo	�G�t�F�N�g�Ǘ��ԍ�
 * @param	type	�G�t�F�N�g�ԍ�
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComEffectCreate	effNo	type
	.short	SCRIPT_ENUM_EffectCreate
	.long	\effNo
	.long	\type
	.endm

//======================================================================
/**
 * @brief	�G�t�F�N�g�F�j��
 *
 * #param_num	1
 * @param	effNo	�G�t�F�N�g�Ǘ��ԍ�
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComEffectDelete	effNo
	.short	SCRIPT_ENUM_EffectDelete
	.long	\effNo
	.endm

//======================================================================
/**
 * @brief	�G�t�F�N�g�F�Đ�
 *
 * #param_num	5
 * @param	effNo	�G�t�F�N�g�Ǘ��ԍ�
 * @param	emitNo	�G�~�b�^�ԍ�
 * @param	posX	�w���W
 * @param	posY	�x���W
 * @param	posZ	�y���W
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 */
//======================================================================
	.macro	ComEffectStart	effNo	emitNo	posX	posY	posZ
	.short	SCRIPT_ENUM_EffectStart
	.long	\effNo
	.long	\emitNo
	.long	\posX
	.long	\posY
	.long	\posZ
	.endm

//======================================================================
/**
 * @brief	�G�t�F�N�g�F��~
 *
 * #param_num	2
 * @param	effNo	�G�t�F�N�g�Ǘ��ԍ�
 * @param	emitNo	�G�~�b�^�ԍ�
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComEffectStop	effNo	emitNo
	.short	SCRIPT_ENUM_EffectStop
	.long	\effNo
	.long	\emitNo
	.endm

//======================================================================
/**
 * @brief	�G�t�F�N�g�F�A���Đ�
 *
 * #param_num	7
 * @param	effNo	�G�t�F�N�g�Ǘ��ԍ�
 * @param	emitNo	�G�~�b�^�ԍ�
 * @param	interval	�Ԋu
 * @param	repeat	��
 * @param	posX1	�w���W
 * @param	posY1	�x���W
 * @param	posZ1	�y���W
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 */
//======================================================================
	.macro	ComEffectRepeatStart	effNo	emitNo	interval	repeat	posX1	posY1	posZ1
	.short	SCRIPT_ENUM_EffectRepeatStart
	.long	\effNo
	.long	\emitNo
	.long	\interval
	.long	\repeat
	.long	\posX1
	.long	\posX1
	.long	\posY1
	.long	\posY1
	.long	\posZ1
	.long	\posZ1
	.endm

#pragma mark [>Light Command
//======================================================================
/**
 * @brief	�X�|�b�g���C�g�F�쐬
 *
 * #param_num	2
 * @param	lightNo	�X�|�b�g���C�g�Ǘ��ԍ�
 * @param	radian	���a(������)
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComLightShowCircle	lightNo	radian
	.short	SCRIPT_ENUM_LightShowCircle
	.long	\lightNo
	.long	\radian
	.endm

//======================================================================
/**
 * @brief	�X�|�b�g���C�g�F�j��
 *
 * #param_num	1
 * @param	lightNo	�X�|�b�g���C�g�Ǘ��ԍ�
 *
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComLightHide	lightNo
	.short	SCRIPT_ENUM_LightHide
	.long	\lightNo
	.endm

//======================================================================
/**
 * @brief	�X�|�b�g���C�g�F�ړ�
 *
 * #param_num	5
 * @param	lightNo	�X�|�b�g���C�g�Ǘ��ԍ�
 * @param	frame	�t���[����
 * @param	posX	�w���W
 * @param	posY	�x���W
 * @param	posZ	�y���W
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 */
//======================================================================
	.macro	ComLightMove	lightNo	frame	posX	posY	posZ
	.short	SCRIPT_ENUM_LightMove
	.long	\lightNo
	.long	\frame
	.long	\posX
	.long	\posY
	.long	\posZ
	.endm

//======================================================================
/**
 * @brief	�X�|�b�g���C�g�F�F�ݒ�
 *
 * #param_num	5
 * @param	lightNo	�X�|�b�g���C�g�Ǘ��ԍ�
 * @param	colR	�F�F�q
 * @param	colG	�F�F�f
 * @param	colB	�F�F�a
 * @param	alpha	�����x
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComLightColor	lightNo	colR	colG	colB	alpha
	.short	SCRIPT_ENUM_LightColor
	.long	\lightNo
	.long	\colR
	.long	\colG
	.long	\colB
	.long	\alpha
	.endm

#pragma mark [>Message Command
//======================================================================
/**
 * @brief	���b�Z�[�W�F�\��
 *
 * #param_num	2
 * @param	msgNo	���b�Z�[�W�ԍ�
 * @param	speed	���b�Z�[�W���x(1�����ӂ�̃t���[����)
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComMessageShow	msgNo	speed
	.short	SCRIPT_ENUM_MessageShow
	.long	\msgNo
	.long	\speed
	.endm

//======================================================================
/**
 * @brief	���b�Z�[�W�F��\��
 *
 * #param_num	0
 *
 */
//======================================================================
	.macro	ComMessageHide	
	.short	SCRIPT_ENUM_MessageHide
	.endm

//======================================================================
/**
 * @brief	���b�Z�[�W�F�F�ύX
 *
 * #param_num	3
 * @param	colLetter	�����F
 * @param	colShadow	�e�F
 * @param	colBack	�w�i�F
 *
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
 */
//======================================================================
	.macro	ComMessageColor	colLetter	colShadow	colBack
	.short	SCRIPT_ENUM_MessageColor
	.long	\colLetter
	.long	\colShadow
	.long	\colBack
	.endm

#pragma mark [>Other Command
//======================================================================
/**
 * @brief	�~���[�W�J���I��(�_�~�[
 *
 * #param_num	0
 */
//======================================================================
	.macro	ComSEQ_END
	.short	SCRIPT_ENUM_ScriptFinish
	.endm

#endif //__C_NO_DEF_

