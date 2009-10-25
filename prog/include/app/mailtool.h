 /**
 *	@file	mail.h
 *	@brief	���[���V�X�e���w�b�_
 *	@author	Miyuki Iwasaw
 *	@date	06.02.07
 */

#pragma once

#if 0 //@todo�܂��K�v�Ȃ������̂ňڍs���Ă��Ȃ�

//���[���V�X�e���@�p�����[�^���p�����[�N
#include "application/mail/mail_param.h"

extern PROC_RESULT MailSysProc_Init( PROC *proc,int *seq);
extern PROC_RESULT MailSysProc_Main( PROC *proc,int *seq);
extern PROC_RESULT MailSysProc_End( PROC *proc,int *seq);

/**
 *	@brief	���[����ʌĂяo�����[�N�쐬(Create)
 *
 *	@retval	NULL	�V�K�쐬�̈悪�Ȃ�
 *	@retval ���[�N�|�C���^	���[���쐬��ʌĂяo��
 */
extern MAIL_PARAM* MailSys_GetWorkCreate(SAVEDATA* savedata,
	MAILBLOCK_ID blockID,u8 poke_pos,u8 design,int heapID);

/**
 *	@brief	���[����ʌĂяo�����[�N�쐬(View)
 *
 *	@retval	NULL	�V�K�쐬�̈悪�Ȃ�
 *	@retval ���[�N�|�C���^	���[���`���ʌĂяo��
 */
extern MAIL_PARAM* MailSys_GetWorkView(SAVEDATA* savedata,
		MAILBLOCK_ID blockID,u16 dataID,int heapID);

/**
 *	@brief	���[����ʌĂяo�����[�N�쐬(View/POKEMON_PARAM)
 */
extern MAIL_PARAM* MailSys_GetWorkViewPoke(SAVEDATA* savedata,POKEMON_PARAM* poke,int heapID);

/**
 *	@brief	���[����ʌĂяo�����[�N�쐬(View/�󃁁[���v���r���[)
 */
extern MAIL_PARAM* MailSys_GetWorkViewPrev(SAVEDATA* savedata,u8 designNo,int heapID);


/**
 *	@brief	���O�̃��W���[���Ăяo���Ńf�[�^���쐬���ꂽ���ǂ����H
 */
extern BOOL MailSys_IsDataCreate(MAIL_PARAM* wk);

/**
 *	@brief	���O�̃��W���[���Ăяo���ō쐬���ꂽ�f�[�^���Z�[�u�f�[�^�ɔ��f
 */
extern int MailSys_PushDataToSave(MAIL_PARAM* wk,MAILBLOCK_ID blockID,u8 dataID);

/**
 *	@brief	���O�̃��W���[���Ăяo���ō쐬���ꂽ�f�[�^���Z�[�u�f�[�^�ɔ��f(Poke)
 */
extern int MailSys_PushDataToSavePoke(MAIL_PARAM* wk,POKEMON_PARAM* poke);

/**
 *	@brief	���[�����W���[���Ăяo�����[�N�����
 */
extern void MailSys_ReleaseCallWork(MAIL_PARAM* wk);


#endif


/**
 *	@brief	�|�P�������[�����p�\���[���Ɉړ�
 *
 *	@retval	MAILDATA_NULLID	�󂫂��Ȃ��̂œ]���ł��Ȃ�
 *	@retval	"���̑�"�@�]�������f�[�^ID
 */
extern int	MailSys_MoveMailPoke2Paso(MAIL_BLOCK* block,POKEMON_PARAM* poke,HEAPID heapID);

/**
 *	@brief	�p�\���[�����|�P�����Ɉړ�
 *
 *	@retval	MAILDATA_NULLID ID���s���Ȃ̂łȂɂ����Ȃ�����
 *	@retval	"���̑�"�@�]�������f�[�^ID
 */
extern int	MailSys_MoveMailPaso2Poke(MAIL_BLOCK* block,u16 id,POKEMON_PARAM* poke,HEAPID heapID);



