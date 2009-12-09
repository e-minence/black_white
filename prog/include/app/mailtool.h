 /**
 *  @file mailtool.h
 *  @brief  ���[���V�X�e���w�b�_
 *  @author Miyuki Iwasaw
 *  @date 06.02.07
 */

#pragma once


//���[���V�X�e���@�p�����[�^���p�����[�N
#include "app/mail/mail_param.h"

extern GFL_PROC_RESULT MailSysProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT MailSysProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT MailSysProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk );


extern const GFL_PROC_DATA MailSysProcData;

/**
 *  @brief  ���[����ʌĂяo�����[�N�쐬(Create)
 *
 *  @retval NULL  �V�K�쐬�̈悪�Ȃ�
 *  @retval ���[�N�|�C���^  ���[���쐬��ʌĂяo��
 */
extern MAIL_PARAM* MAILSYS_GetWorkCreate( GAMEDATA* gamedata,
  MAILBLOCK_ID blockID,u8 poke_pos,u8 design, int heapID);


/**
 *  @brief  ���[����ʌĂяo�����[�N�쐬(View)
 *
 *  @retval NULL  �V�K�쐬�̈悪�Ȃ�
 *  @retval ���[�N�|�C���^  ���[���`���ʌĂяo��
 */
extern MAIL_PARAM* MailSys_GetWorkView( GAMEDATA* gamedata, MAILBLOCK_ID blockID, 
                                        u16 dataID, int heapID );

/**
 *  @brief  ���[����ʌĂяo�����[�N�쐬(View/POKEMON_PARAM)
 */
extern MAIL_PARAM* MailSys_GetWorkViewPoke( GAMEDATA* gamedata, POKEMON_PARAM* poke, int heapID );


/**
 *  @brief  ���[����ʌĂяo�����[�N�쐬(View/�󃁁[���v���r���[)
 */
extern MAIL_PARAM* MailSys_GetWorkViewPrev( GAMEDATA* gamedata, u8 designNo, int heapID );



/**
 *  @brief  ���O�̃��W���[���Ăяo���Ńf�[�^���쐬���ꂽ���ǂ����H
 */
extern BOOL MailSys_IsDataCreate( MAIL_PARAM* wk);

/**
 *  @brief  ���O�̃��W���[���Ăяo���ō쐬���ꂽ�f�[�^���Z�[�u�f�[�^�ɔ��f
 */
extern int MailSys_PushDataToSave( MAIL_PARAM* wk,MAILBLOCK_ID blockID,u8 dataID );

/**
 *  @brief  ���O�̃��W���[���Ăяo���ō쐬���ꂽ�f�[�^���Z�[�u�f�[�^�ɔ��f(Poke)
 */
extern int MailSys_PushDataToSavePoke( MAIL_PARAM* wk, POKEMON_PARAM* poke );

/**
 *  @brief  ���[�����W���[���Ăяo�����[�N�����
 */
extern void MailSys_ReleaseCallWork( MAIL_PARAM* wk);




/**
 *  @brief  �|�P�������[�����p�\���[���Ɉړ�
 *
 *  @retval MAILDATA_NULLID �󂫂��Ȃ��̂œ]���ł��Ȃ�
 *  @retval "���̑�"�@�]�������f�[�^ID
 */
extern int  MailSys_MoveMailPoke2Paso(MAIL_BLOCK* block,POKEMON_PARAM* poke,HEAPID heapID);

/**
 *  @brief  �p�\���[�����|�P�����Ɉړ�
 *
 *  @retval MAILDATA_NULLID ID���s���Ȃ̂łȂɂ����Ȃ�����
 *  @retval "���̑�"�@�]�������f�[�^ID
 */
extern int  MailSys_MoveMailPaso2Poke(MAIL_BLOCK* block,u16 id,POKEMON_PARAM* poke,HEAPID heapID);



