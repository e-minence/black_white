//============================================================================================
/**
 * @file	  itemuse_event.h
 * @brief	  �t�B�[���h�Ɋ֘A�����A�C�e���̎g�p����
 * @author	k.ohno
 * @date	  09.08.04
 */
//============================================================================================


#pragma once

#include "field/fieldmap_proc.h"
#include "gamesystem/gamesystem.h"

/// ���ʉ������Ăяo��
typedef GMEVENT* (ItemUseEventFunc)(FIELD_MAIN_WORK *fieldWork,GAMESYS_WORK *gsys);

/// ���]�Ԃ��g��
extern GMEVENT* EVENT_CycleUse(FIELD_MAIN_WORK *fieldWork,GAMESYS_WORK *gsys);

/// �p���X�ɃW�����v����
extern GMEVENT* EVENT_PalaceJumpUse(FIELD_MAIN_WORK *fieldWork,GAMESYS_WORK *gsys);

///�I�[�o�[���C�h�c��`
FS_EXTERN_OVERLAY(itemuse);

