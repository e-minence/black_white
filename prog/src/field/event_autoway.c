//////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �ړ����ɂ�鋭���ړ��C�x���g
 * @file   event_autoway.c
 * @author obata
 * @date   2009.11.27
 *
 * @todo SE�����Ă�
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "event_autoway.h"

#include "field_player.h"
#include "fldmmdl.h"
#include "fldmmdl_code.h"
#include "map_attr_def.h"  // for MATTR_xxxx


//====================================================================================================
// ���萔
//====================================================================================================
#define FRAME_PER_GRID (8)                             // 1�O���b�h�̈ړ��ɂ����鎞��
#define FRAME_PER_DIRCHENGE (FRAME_PER_GRID/DIR_MAX4)  // �����̕ύX�Ԋu

// �����V�[�P���X
enum
{
  SEQ_START,  // �C�x���g�J�n
  SEQ_MAIN,   // ���C������
  SEQ_EXIT,   // �C�x���g�I��
};


//====================================================================================================
// ���C�x���g���[�N
//====================================================================================================
typedef struct
{
  GAMESYS_WORK*      gsys;  // �Q�[���V�X�e��
  FIELDMAP_WORK* fieldmap;  // �t�B�[���h�}�b�v
  FIELD_PLAYER*    player;  // ���@
  MMDL*              mmdl;  // ���@�̓��샂�f��
  u16             moveDir;  // �ړ�����
  u16              rotDir;  // ���@�̌���
  u32            rotCount;  // ��]�J�E���^

} EVENT_WORK;


//====================================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//====================================================================================================
static void UpdateRotDir( EVENT_WORK* work );
static void UpdateMoveDir( EVENT_WORK* work, MAPATTR_VALUE attrval );
static void MoveStart( EVENT_WORK* work );
static MAPATTR_VALUE GetAttribute( EVENT_WORK* work );
static void AttributeCheck( EVENT_WORK* work );
static BOOL HitCheck( EVENT_WORK* work );


//====================================================================================================
// ���C�x���g�����֐�
//====================================================================================================
static GMEVENT_RESULT AutoMove( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  // �C�x���g�J�n
  case SEQ_START:
    // �������ړ����łȂ���ΏI��
    {
      MAPATTR_VALUE attrval = GetAttribute( work );
      if( (attrval != MATTR_MOVEF_LEFT) && (attrval != MATTR_MOVEF_RIGHT) &&
          (attrval != MATTR_MOVEF_UP)   && (attrval != MATTR_MOVEF_DOWN) )
      { 
        *seq = SEQ_EXIT;
        break;
      }
    }
    *seq = SEQ_MAIN;
    break;
  // ���C������
  case SEQ_MAIN:
    // ��]
    UpdateRotDir( work );
    // �ړ��I����
    if( MMDL_CheckEndAcmd(work->mmdl) ) // if(�ړ��I��)
    {
      // �A�g���r���[�g�`�F�b�N
      AttributeCheck( work );
      // �Փ˃`�F�b�N
      if( HitCheck(work) )  // if(�Փ�)
      { 
        *seq = SEQ_EXIT; 
        break;
      }
      // �ړ��J�n
      MoveStart( work );
    }
    break;
  // �C�x���g�I��
  case SEQ_EXIT:
    // �ύX�����X�e�[�^�X�r�b�g�����ɖ߂�
    MMDL_OffStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
    // �i�s����������
    MMDL_SetDirDisp( work->mmdl, work->moveDir );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//====================================================================================================
// ���O�����J�֐�
//====================================================================================================

//----------------------------------------------------------------------------------------------------
/**
 * @brief �ړ����C�x���g���쐬����
 *
 * @param parent   �e�ƂȂ�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �ړ����C�x���g
 */
//----------------------------------------------------------------------------------------------------
GMEVENT* EVENT_PlayerMoveOnAutoWay( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gsys, parent, AutoMove, sizeof(EVENT_WORK) );
  // ������
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->fieldmap = fieldmap;
  work->player   = FIELDMAP_GetFieldPlayer( fieldmap );
  work->mmdl     = FIELD_PLAYER_GetMMdl( work->player );
  work->moveDir  = DIR_LEFT;
  work->rotDir   = MMDL_GetDirDisp( work->mmdl );
  work->rotCount = 0;
  return event;
} 


//====================================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//====================================================================================================

//----------------------------------------------------------------------------------------------------
/**
 * @brief ���@�̌������X�V����
 *
 * @param work    �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------------------
static void UpdateRotDir( EVENT_WORK* work )
{ 
  if( ++work->rotCount % FRAME_PER_DIRCHENGE == 0 )
  {
    // �X�V
    work->rotDir = (work->rotDir + 1) % DIR_MAX4;
    // ���샂�f���ɔ��f
    MMDL_OffStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
    MMDL_SetDirDisp( work->mmdl, work->rotDir );
    MMDL_OnStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief �ړ��������X�V����
 *
 * @param work    �C�x���g���[�N
 * @param attrval �����̃A�g���r���[�g
 */
//----------------------------------------------------------------------------------------------------
static void UpdateMoveDir( EVENT_WORK* work, MAPATTR_VALUE attrval )
{
  switch( attrval ) 
  {
  case MATTR_MOVEF_RIGHT: work->moveDir = DIR_RIGHT; break;
  case MATTR_MOVEF_LEFT:  work->moveDir = DIR_LEFT;  break;
  case MATTR_MOVEF_UP:    work->moveDir = DIR_UP;    break;
  case MATTR_MOVEF_DOWN:  work->moveDir = DIR_DOWN;  break;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief �ړ����J�n����
 *
 * @param work �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------------------
static void MoveStart( EVENT_WORK* work )
{ 
  switch( work->moveDir )
  {
  case DIR_LEFT:  MMDL_SetAcmd( work->mmdl, AC_WALK_L_8F ); break;
  case DIR_RIGHT: MMDL_SetAcmd( work->mmdl, AC_WALK_R_8F ); break;
  case DIR_UP:    MMDL_SetAcmd( work->mmdl, AC_WALK_U_8F ); break;
  case DIR_DOWN:  MMDL_SetAcmd( work->mmdl, AC_WALK_D_8F ); break;
  default:        return;
  }

  // �ړ��A�j���[�V�����ɂ������̕ύX���ꎞ��~
  MMDL_OnStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief �����̃A�g���r���[�g���擾����
 *
 * @param work �C�x���g���[�N
 *
 * @return ��l���̑����̃A�g���r���[�g
 */
//----------------------------------------------------------------------------------------------------
static MAPATTR_VALUE GetAttribute( EVENT_WORK* work )
{
  VecFx32 pos;
  MAPATTR attr;
  MAPATTR_VALUE attrval;
  FLDMAPPER* mapper;

  // �����̃A�g���r���[�g���擾
  FIELD_PLAYER_GetPos( work->player, &pos );
  mapper  = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
  attr    = MAPATTR_GetAttribute( mapper, &pos );
  attrval = MAPATTR_GetAttrValue( attr );
  return attrval;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief �A�g���r���[�g�`�F�b�N
 *
 * @param work �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------------------
static void AttributeCheck( EVENT_WORK* work )
{
  MAPATTR_VALUE attrval;

  // �����̃A�g���r���[�g���擾
  attrval = GetAttribute( work );

  // �ړ��������X�V
  UpdateMoveDir( work, attrval );
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief �Փ˃`�F�b�N
 *
 * @param work �C�x���g���[�N
 *
 * @return �Փ˂����� TRUE, �����łȂ���� FALSE
 */
//----------------------------------------------------------------------------------------------------
static BOOL HitCheck( EVENT_WORK* work )
{
  // �i�s�����̃O���b�h���`�F�b�N
  u32 hitbit = MMDL_HitCheckMoveDir( work->mmdl, work->moveDir );
  return (hitbit != MMDL_MOVEHITBIT_NON);
}
