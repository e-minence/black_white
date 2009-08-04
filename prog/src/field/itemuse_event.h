//============================================================================================
/**
 * @file	  itemuse_event.h
 * @brief	  �t�B�[���h�Ɋ֘A�����A�C�e���̎g�p����
 * @author	k.ohno
 * @date	  09.08.04
 */
//============================================================================================


#pragma once

#include "gamesystem/gamesystem.h"

/// ���ʉ������Ăяo��
typedef GMEVENT* (ItemUseEventFunc)(GAMESYS_WORK *gsys);

/// ���]�Ԃ��g��
extern GMEVENT * EVENT_CycleUse(GAMESYS_WORK *gsys);

///�I�[�o�[���C�h�c��`
FS_EXTERN_OVERLAY(itemuse);

