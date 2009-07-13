//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_rhythm.h
 *	@brief	�ԊO���~�j�Q�[��	���Y���`�F�b�N
 *	@author	Toru=Nagihashi
 *	@date		2009.05.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "gamesystem/gamesystem.h"
#include "net_app/compatible_irc_sys.h"
#include "net_app/irc_compatible.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�i�s��
//=====================================
typedef enum {
	IRCRHYTHM_RESULT_CLEAR,		//�N���A����
	IRCRHYTHM_RESULT_RETURN,	//�߂��I��
	IRCRHYTHM_RESULT_RESULT,	//���ʉ�ʂ�
} IRCRHYTHM_RESULT;



//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	PROC�ɓn���p�����[�^
//=====================================
typedef struct {
	GAMESYS_WORK				*p_gamesys;//[in]�Q�[���V�X�e��
	COMPATIBLE_IRC_SYS	*p_irc;	//[in]�ԊO���V�X�e��
	MYSTATUS						*p_you_status;//[in]�ʐM����̃X�e�[�^�X�i�o�b�t�@��n���Ă��������j
	IRCRHYTHM_RESULT		result;	//[out]PROC���߂��Ă����Ƃ��̓����i�s��
	u8									score;	//[out]���Y���`�F�b�N�œ����_��

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
	BOOL								is_only_play;	//[in]�f�o�b�O����p�A��l�v���C���[�h
#endif //DEBUG_IRC_COMPATIBLE_ONLYPLAY
}IRC_RHYTHM_PARAM;

//-------------------------------------
///	���Y���`�F�b�N�p�v���b�N�f�[�^
//=====================================
extern const GFL_PROC_DATA IrcRhythm_ProcData;

