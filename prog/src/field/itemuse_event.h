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
#include "app/itemuse.h"

typedef enum{
 ITEMUSE_OK,
 ITEMUSE_NG,
 ITEMUSE_CYCLE_GETOFF_NG,
}ITEMUSE_CHECK_CODE;

/// ���ʉ������Ăяo��
typedef GMEVENT* (*ItemUseEventFunc)(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys);
typedef BOOL (*ItemUseCheckFunc)( GAMEDATA* gdata, FIELDMAP_WORK *fieldWork, PLAYER_WORK* playerWork );

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
  EVENT_ITEMUSE_CALL_DOWSINGMACHINE,

  EVENT_ITEMUSE_CALL_MAX,
}EVENT_ITEMUSE_CALL_TYPE;

//----------------------------------------------------------------------------
/**
 *  @brief  �o�b�O��ʂɈ����n���A�C�e���g�p�`�F�b�N
 *
 *  @param  GMEVENT *event  �C�x���g
 *  @param  *seq            �V�[�P���X
 *  @param  *wk_adrs        ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
extern void ITEMUSE_InitCheckWork( ITEMCHECK_WORK* icwk, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldWork );

//----------------------------------------------------------------------------
/**
 *  @brief  ���ʃA�C�e���g�p�`�F�b�N�֐��Ăяo��
 *
 *  @param  EVENT_ITEMUSE_CALL_TYPE type  �Ăяo���`�F�b�N�^�C�v
 *  @param  *gsys       �Q�[���V�X�e��
 *  @param  *field_wk   �t�B�[���h
 *
 *  @return �C�x���g
 */
//-----------------------------------------------------------------------------
extern BOOL ITEMUSE_UseCheckCall( ITEMCHECK_ENABLE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk );

//----------------------------------------------------------------------------
/**
 *  @brief  ���ʃA�C�e���g�p�C�x���g�Ăяo��
 *
 *  @param  EVENT_ITEMUSE_CALL_TYPE type  �Ăяo���A�C�e��
 *  @param  *gsys       �Q�[���V�X�e��
 *  @param  *field_wk   �t�B�[���h
 *
 *  @return �C�x���g
 */
//-----------------------------------------------------------------------------
extern GMEVENT * EVENT_FieldItemUse( EVENT_ITEMUSE_CALL_TYPE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk );

//----------------------------------------------------------------------------
/**
 *  @brief  ���ʃA�C�e���g�p�`�F�b�N�Ăяo��
 *
 *  @param  EVENT_ITEMUSE_CALL_TYPE type  �Ăяo���A�C�e��
 *  @param  *gsys       �Q�[���V�X�e��
 *  @param  *field_wk   �t�B�[���h
 *
 *  @retval TRUE�F�g�p��
 *  @retval FALSE�F�g�p�s��
 */
//-----------------------------------------------------------------------------
extern BOOL FieldItemUseCheck( EVENT_ITEMUSE_CALL_TYPE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk );


//=============================================================================
/**
 *  �ʌĂяo��
 */
//=============================================================================

/// ���]�Ԃ��g��
extern GMEVENT* EVENT_CycleUse(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys);

/// �p���X�ɃW�����v����
extern GMEVENT* EVENT_PalaceJumpUse(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys);

/// �_�E�W���O�}�V�����g��
extern GMEVENT* EVENT_DowsingMachineUse(FIELDMAP_WORK* fieldWork, GAMESYS_WORK* gsys);

///�I�[�o�[���C�h�c��`
//FS_EXTERN_OVERLAY(itemuse);

