//============================================================================================
/**
 * @file    itemuse_event.h
 * @brief   �t�B�[���h�Ɋ֘A�����A�C�e���̎g�p����
 * @author  k.ohno
 * @date    09.08.04
 */
//============================================================================================


#pragma once

#include "field/fieldmap_proc.h"
#include "gamesystem/gamesystem.h"

/// ���ʉ������Ăяo��
typedef GMEVENT* (ItemUseEventFunc)(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys);

//=============================================================================
/**
 *  ����
 */
//=============================================================================
//-------------------------------------
/// �A�C�e���g�p�e�[�u��
//=====================================
typedef enum
{ 
  EVENT_ITEMUSE_CALL_CYCLE,
  EVENT_ITEMUSE_CALL_PALACEJUMP,
  EVENT_ITEMUSE_CALL_ANANUKENOHIMO,
  EVENT_ITEMUSE_CALL_AMAIMITU,
  EVENT_ITEMUSE_CALL_TURIZAO,

  EVENT_ITEMUSE_CALL_MAX,
}EVENT_ITEMUSE_CALL_TYPE;

//���ʌĂяo��
extern GMEVENT * EVENT_FieldItemUse( EVENT_ITEMUSE_CALL_TYPE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk );

//=============================================================================
/**
 *  �ʌĂяo��
 */
//=============================================================================

/// ���]�Ԃ��g��
extern GMEVENT* EVENT_CycleUse(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys);

/// �p���X�ɃW�����v����
extern GMEVENT* EVENT_PalaceJumpUse(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys);

///�I�[�o�[���C�h�c��`
FS_EXTERN_OVERLAY(itemuse);

