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

#define SCRIPT_FUNC_DEF(str) STA_SCRIPT_##str##_Func

#endif MUSICAL_SCRIPT_COMMAND_H__

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

#pragma mark [>Symbol
//���߃V���{���錾
	INIT_CMD
	DEF_CMD SCRIPT_FUNC_DEF(ScriptFinish)
	DEF_CMD SCRIPT_FUNC_DEF(FrameWait)
	DEF_CMD SCRIPT_FUNC_DEF(FrameWaitTime)

	//�J�[�e���n
	DEF_CMD SCRIPT_FUNC_DEF(CurtainUp)		//�J�[�e�����グ��
	DEF_CMD SCRIPT_FUNC_DEF(CurtainDown)		//�J�[�e����������
	DEF_CMD SCRIPT_FUNC_DEF(CurtainMove)		//2	//�J�[�e�����ړ�������

	//����n
	DEF_CMD SCRIPT_FUNC_DEF(StageMove)		//2	//�X�e�[�W���ړ�������
	DEF_CMD SCRIPT_FUNC_DEF(StageChangeBg)	//1	//BG�ǂݑւ�

	//�|�P�����n
	DEF_CMD SCRIPT_FUNC_DEF(PokeShow)			//2	//�|�P�����̕\���E��\������
	DEF_CMD SCRIPT_FUNC_DEF(PokeDir)			//2	//�|�P�����̌���(�E����)
	DEF_CMD SCRIPT_FUNC_DEF(PokeMove)			//5	//�|�P�����ړ�
	DEF_CMD SCRIPT_FUNC_DEF(PokeStopAnime)	//1	//�|�P�����A�j����~
	DEF_CMD SCRIPT_FUNC_DEF(PokeStartAnime)	//1	//�|�P�����A�j���J�n
	DEF_CMD SCRIPT_FUNC_DEF(PokeChangeAnime)	//2	//�|�P�����A�j���ύX
	DEF_CMD SCRIPT_FUNC_DEF(PokeActionJump)	//4	//�|�P�����A�N�V�����E���˂�

	//�I�u�W�F�N�g�n
	DEF_CMD SCRIPT_FUNC_DEF(ObjectCreate)		//2	//�I�u�W�F�N�g�쐬
	DEF_CMD SCRIPT_FUNC_DEF(ObjectDelete)		//1	//�I�u�W�F�N�g�j��
	DEF_CMD SCRIPT_FUNC_DEF(ObjectShow)		//1	//�I�u�W�F�N�g�\��
	DEF_CMD SCRIPT_FUNC_DEF(ObjectHide)		//1	//�I�u�W�F�N�g��\��
	DEF_CMD SCRIPT_FUNC_DEF(ObjectMove)		//5	//�|�P�����ړ�

	//�G�t�F�N�g�n
	DEF_CMD SCRIPT_FUNC_DEF(EffectCreate)		//2	//�G�t�F�N�g�쐬
	DEF_CMD SCRIPT_FUNC_DEF(EffectDelete)		//1	//�G�t�F�N�g�j��
	DEF_CMD SCRIPT_FUNC_DEF(EffectStart)		//5	//�G�t�F�N�g�Đ�
	DEF_CMD SCRIPT_FUNC_DEF(EffectStop)		//2	//�G�t�F�N�g��~
	DEF_CMD SCRIPT_FUNC_DEF(EffectRepeatStart)//10//�G�t�F�N�g�A���Đ�

	//���C�g�n
	DEF_CMD SCRIPT_FUNC_DEF(LightShowCircle)	//2	//���C�gON(�~�`
	DEF_CMD SCRIPT_FUNC_DEF(LightHide)		//1	//���C�gOFF
	DEF_CMD SCRIPT_FUNC_DEF(LightMove)		//5	//���C�g�ړ�
	DEF_CMD SCRIPT_FUNC_DEF(LightColor)		//3	//���C�g�F�ݒ�

	//���b�Z�[�W�n
	DEF_CMD SCRIPT_FUNC_DEF(MessageShow)		//2	//���b�Z�[�W�\��
	DEF_CMD SCRIPT_FUNC_DEF(MessageHide)			//���b�Z�[�W����
	DEF_CMD SCRIPT_FUNC_DEF(MessageColor)		//1	//���b�Z�[�W�F�ς�

	DEF_CMD	EC_SEQ_END
	
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
	.short	SCRIPT_FUNC_DEF(ScriptFinish)
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
	.short	SCRIPT_FUNC_DEF(FrameWait)
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
	.short	SCRIPT_FUNC_DEF(FrameWaitTime)
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
	.short	SCRIPT_FUNC_DEF(CurtainUp)
	.endm

//======================================================================
/**
 * @brief	�J�[�e���F����������(�Œ葬�x
 *
 * #param_num	0
 */
//======================================================================
	.macro	ComCurtainDown
	.short	SCRIPT_FUNC_DEF(CurtainDown)
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
	.short	SCRIPT_FUNC_DEF(CurtainMove)
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
	.short	SCRIPT_FUNC_DEF(StageMove)
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
	.short	SCRIPT_FUNC_DEF(StageChangeBg)
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
 * #param	COMBOBOX_VALUE	0	1
 */
//======================================================================
	.macro	ComPokeShow	pokeNo	flg
	.short	SCRIPT_FUNC_DEF(PokeShow)
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
	.short	SCRIPT_FUNC_DEF(PokeDir)
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
	.short	SCRIPT_FUNC_DEF(PokeMove)
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
	.short	SCRIPT_FUNC_DEF(PokeStopAnime)
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
	.short	SCRIPT_FUNC_DEF(PokeStartAnime)
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
	.short	SCRIPT_FUNC_DEF(PokeChangeAnime)
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
	.short	SCRIPT_FUNC_DEF(PokeActionJump)
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
	.short	SCRIPT_FUNC_DEF(ObjectCreate)
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
	.short	SCRIPT_FUNC_DEF(ObjectDelete)
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
	.short	SCRIPT_FUNC_DEF(ObjectShow)
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
	.short	SCRIPT_FUNC_DEF(ObjectHide)
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
	.short	SCRIPT_FUNC_DEF(ObjectMove)
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
	.short	SCRIPT_FUNC_DEF(EffectCreate)
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
	.short	SCRIPT_FUNC_DEF(EffectDelete)
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
	.short	SCRIPT_FUNC_DEF(EffectStart)
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
	.short	SCRIPT_FUNC_DEF(EffectStop)
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
	.short	SCRIPT_FUNC_DEF(EffectRepeatStart)
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
	.short	SCRIPT_FUNC_DEF(LightShowCircle)
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
	.short	SCRIPT_FUNC_DEF(LightHide)
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
	.short	SCRIPT_FUNC_DEF(LightMove)
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
	.short	SCRIPT_FUNC_DEF(LightColor)
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
	.short	SCRIPT_FUNC_DEF(MessageShow)
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
	.short	SCRIPT_FUNC_DEF(MessageHide)
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
	.short	SCRIPT_FUNC_DEF(MessageColor)
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
	.macro	SEQ_END
	.short	SCRIPT_FUNC_DEF(ScriptFinish)
	.endm

#endif //__C_NO_DEF_

#undef SCRIPT_FUNC_DEF
