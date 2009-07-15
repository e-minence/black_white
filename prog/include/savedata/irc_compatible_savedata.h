//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_compatible_savedata.h
 *	@brief	�ԊO�������`�F�b�N�p�Z�[�u�f�[�^
 *	@author	Toru=Nagihashi
 *	@date		2009.07.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include <strbuf.h>
#include "savedata/save_control.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define IRC_COMPATIBLE_SV_NEW_NONE		(127)		//�V�K�����N�����Ȃ�
#define IRC_COMPATIBLE_SV_ADD_NONE		(0xFF)	//�����L���O�ɓ���Ȃ�����
#define IRC_COMPATIBLE_SV_RANKING_MAX	(50)		//�����L���O�ő吔


#define IRC_COMPATIBLE_SV_DATA_SCORE_MAX		(100)		//�X�R�A�ő�
#define IRC_COMPATIBLE_SV_DATA_PLAYCNT_MAX	(999)		//�V�񂾉񐔍ő�

#define IRC_COMPATIBLE_SV_DATA_NAME_LEN			(PERSON_NAME_SIZE + EOM_SIZE)	//���O�̒���

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�����`�F�b�N�p�Z�[�u�f�[�^
//=====================================
typedef struct _IRC_COMPATIBLE_SAVEDATA IRC_COMPATIBLE_SAVEDATA;
//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�Z�[�u�f�[�^�擾�擾
//=====================================
extern IRC_COMPATIBLE_SAVEDATA * IRC_COMPATIBLE_SV_GetSavedata( SAVE_CONTROL_WORK* p_sv_ctrl );

//-------------------------------------
///	�f�[�^�擾�֐�
//=====================================
extern u32 IRC_COMPATIBLE_SV_GetRankNum( const IRC_COMPATIBLE_SAVEDATA *cp_sv );
extern const STRCODE* IRC_COMPATIBLE_SV_GetPlayerName( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u8	IRC_COMPATIBLE_SV_GetScore( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u16 IRC_COMPATIBLE_SV_GetPlayCount( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u8	IRC_COMPATIBLE_SV_GetNewRank( const IRC_COMPATIBLE_SAVEDATA *cp_sv );

//-------------------------------------
///	�f�[�^�ݒ�֐�
//=====================================
extern u8 IRC_COMPATIBLE_SV_AddRanking( IRC_COMPATIBLE_SAVEDATA *p_sv, const STRCODE *cp_name, u8 score, u32 trainerID );

//-------------------------------------
///	�Z�[�u�f�[�^�V�X�e���Ŏg�p����֐�
//=====================================
extern int IRC_COMPATIBLE_SV_GetWorkSize( void );
extern void IRC_COMPATIBLE_SV_Init( IRC_COMPATIBLE_SAVEDATA *p_sv );

