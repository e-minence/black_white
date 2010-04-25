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
#define IRC_COMPATIBLE_SV_RANKING_MAX	(30)		//�����L���O�ő吔

#define IRC_COMPATIBLE_SV_DATA_SCORE_MAX		(100)		//�X�R�A�ő�
#define IRC_COMPATIBLE_SV_DATA_PLAYCNT_MAX	(999)		//�V�񂾉񐔍ő�

#define IRC_COMPATIBLE_SV_DATA_NAME_LEN			(PERSON_NAME_SIZE + EOM_SIZE)	//���O�̒���

#define IRC_COMPATIBLE_SEX_MALE     (1)
#define IRC_COMPATIBLE_SEX_FEMALE   (0)

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
extern u32 IRC_COMPATIBLE_SV_GetSex( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u32 IRC_COMPATIBLE_SV_GetBirthMonth( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );
extern u32 IRC_COMPATIBLE_SV_GetBirthDay( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank );

extern void IRC_COMPATIBLE_SV_GetPokeData( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank, u16 *p_mons_no, u8 *p_form_no, u8 *p_mons_sex, u8 *p_egg );

extern u8	IRC_COMPATIBLE_SV_GetNewRank( const IRC_COMPATIBLE_SAVEDATA *cp_sv );
extern u8 IRC_COMPATIBLE_SV_GetBioRhythm( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank, const RTCDate * cp_now_date );
extern u32 IRC_COMPATIBLE_SV_GetBestBioRhythm( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u8 player_birth_month, u8 player_birth_day, const RTCDate * cp_now_date );

//-------------------------------------
///	irc_compatible.c�ł̂ݎg��private�֐�
//=====================================
extern u8 Irc_Compatible_SV_CalcBioRhythm( u8 birth_month, u8 birth_day, const RTCDate * cp_now_date );

//-------------------------------------
///	�f�[�^�ݒ�֐�
//=====================================
extern u8 IRC_COMPATIBLE_SV_AddRanking_New( IRC_COMPATIBLE_SAVEDATA *p_sv, const STRCODE *cp_name, u8 score, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID, u16 mons_no, u8 form_no, u8 mons_sex, u8 egg );

inline u8 IRC_COMPATIBLE_SV_AddRanking( IRC_COMPATIBLE_SAVEDATA *p_sv, const STRCODE *cp_name, u8 score, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID )
{ 

  return IRC_COMPATIBLE_SV_AddRanking_New( p_sv, cp_name, score, sex, birth_month, birth_day, trainerID, 1, 0, 0, 0 );
}

//-------------------------------------
///	�Z�[�u�f�[�^�V�X�e���Ŏg�p����֐�
//=====================================
extern int IRC_COMPATIBLE_SV_GetWorkSize( void );
extern void IRC_COMPATIBLE_SV_Init( IRC_COMPATIBLE_SAVEDATA *p_sv );

