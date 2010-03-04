//============================================================================================
/**
 * @file	wifi_note.h
 * @bfief	WIFI�F�B�蒠
 * @author	k.ohno
 * @date	06.04.05
 */
//============================================================================================
#pragma once

//============================================================================================
//	��`
//============================================================================================

#include "gamesystem/game_data.h"

//------------------------------------------------------
/**
 * WIFI�Ƃ��������X�gPROC�p�����[�^
 */
//------------------------------------------------------
typedef struct {
  GAMEDATA* pGameData;
	u32	retMode;
}WIFINOTE_PROC_PARAM;

// �I�����[�h
enum {
	WIFINOTE_RET_MODE_CANCEL = 0,		// �L�����Z���i���{�^���j
	WIFINOTE_RET_MODE_EXIT,					// �w�{�^��
};

// �|�C���^�Q�Ƃ����ł��郌�R�[�h�R�[�i�[���[�N�\����
typedef struct _WIFINOTE_WORK WIFINOTE_WORK;	

// �v���Z�X��`�f�[�^
extern const GFL_PROC_DATA WifiNoteProcData;


FS_EXTERN_OVERLAY(wifinote);

