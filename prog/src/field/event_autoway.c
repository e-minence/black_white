///////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �ړ����ɂ�鋭���ړ��C�x���g
 * @file   event_autoway.c
 * @author obata
 * @date   2009.11.27
 *
 * @todo SE�����Ă�
 */
///////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "event_autoway.h"

#include "field_player.h"
#include "fldmmdl.h"
#include "fldmmdl_code.h"
#include "map_attr_def.h"  // for MATTR_xxxx


//=============================================================================================
// ���萔
//=============================================================================================
// �����V�[�P���X
enum
{
  SEQ_START,  // �C�x���g�J�n
  SEQ_MAIN,   // ���C������
  SEQ_EXIT,   // �C�x���g�I��
};


//=============================================================================================
// ���ړ���
//=============================================================================================
typedef struct 
{
  MAPATTR_VALUE attrval;  // �N���A�g���r���[�g
  u16              acmd;  // ���@�ɓK�p����A�j���[�V�����R�}���h( AC_xxxx )
  u16           moveDir;  // �ړ�����( DIR_xxxx )
  u16           rotWait;  // ��]���̌����ύX�Ԋu

} AUTOWAY;

//---------------------------------------------------------------------------------------------
/**
 * @brief �ړ����f�[�^
 */
//---------------------------------------------------------------------------------------------
static const AUTOWAY autoway[] = 
{
  {MATTR_MOVEF_LEFT,  AC_WALK_L_8F, DIR_LEFT,  2},
  {MATTR_MOVEF_RIGHT, AC_WALK_R_8F, DIR_RIGHT, 2},
  {MATTR_MOVEF_UP,    AC_WALK_U_8F, DIR_UP,    2},
  {MATTR_MOVEF_DOWN,  AC_WALK_D_8F, DIR_DOWN,  2},
};

//---------------------------------------------------------------------------------------------
/**
 * @brief �ړ��f�[�^���擾����
 *
 * @param attrval �A�g���r���[�g
 *
 * @return �w�肵���A�g���r���[�g�ɑΉ�����ړ��f�[�^
 *         �Ή�����f�[�^�����݂��Ȃ��ꍇ, NULL
 */
//---------------------------------------------------------------------------------------------
static const AUTOWAY* GetAutoWay( MAPATTR_VALUE attrval )
{
  int i;
  for(i=0; i<NELEMS(autoway); i++ )
  {
    if( autoway[i].attrval == attrval )
    {
      return &autoway[i];
    }
  }
  return NULL;
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �ړ����A�g���r���[�g���ǂ����𔻒肷��
 *
 * @param attrval �A�g���r���[�g
 *
 * @return �w��A�g���r���[�g���ړ����Ȃ� TRUE, �����łȂ��Ȃ� FALSE
 */
//---------------------------------------------------------------------------------------------
static BOOL IsAutoWay( MAPATTR_VALUE attrval )
{
  return (GetAutoWay(attrval) != NULL);
}


//=============================================================================================
// ���C�x���g���[�N
//=============================================================================================
typedef struct
{
  GAMESYS_WORK*      gsys;  // �Q�[���V�X�e��
  FIELDMAP_WORK* fieldmap;  // �t�B�[���h�}�b�v
  FIELD_PLAYER*    player;  // ���@
  MMDL*              mmdl;  // ���@�̓��샂�f��
  const AUTOWAY*  autoWay;  // �K�p���̈ړ����f�[�^
  u16              rotDir;  // ���@�̌���
  u32            rotCount;  // ��]�J�E���^

} EVENT_WORK;


//=============================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//=============================================================================================
static u16 GetAcmd( const EVENT_WORK* work );
static u16 GetMoveDir( const EVENT_WORK* work );
static u16 GetRotWait( const EVENT_WORK* work );
static MAPATTR_VALUE GetAttribute( const EVENT_WORK* work );
static void UpdateRotDir( EVENT_WORK* work );
static void MoveStart( EVENT_WORK* work );
static void AttributeCheck( EVENT_WORK* work );
static BOOL HitCheck( EVENT_WORK* work );
static void ExitEvent( EVENT_WORK* work );


//=============================================================================================
// ���C�x���g�����֐�
//=============================================================================================
static GMEVENT_RESULT AutoMove( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  // �C�x���g�J�n
  case SEQ_START:
    // �������ړ����łȂ���ΏI��
    if( !IsAutoWay( GetAttribute(work) ) )
    {
      *seq = SEQ_EXIT;
      break;
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
    ExitEvent( work );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//=============================================================================================
// ���O�����J�֐�
//=============================================================================================

//---------------------------------------------------------------------------------------------
/**
 * @brief �ړ����C�x���g���쐬����
 *
 * @param parent   �e�ƂȂ�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �ړ����C�x���g
 */
//---------------------------------------------------------------------------------------------
GMEVENT* EVENT_PlayerMoveOnAutoWay( GMEVENT* parent, 
                                    GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
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
  work->autoWay  = 0;
  work->rotDir   = MMDL_GetDirDisp( work->mmdl );
  work->rotCount = 0;
  return event;
} 


//=============================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//=============================================================================================

//---------------------------------------------------------------------------------------------
/**
 * @brief ���ݎQ�ƒ��̈ړ��f�[�^���擾����
 *
 * @param work �C�x���g���[�N
 *
 * @return ���ݎQ�ƒ��̈ړ��f�[�^(�A�j���[�V�����R�}���h AC_xxxx)
 */
//---------------------------------------------------------------------------------------------
static u16 GetAcmd( const EVENT_WORK* work )
{
  if( !work->autoWay ){ return 0; }
  return work->autoWay->acmd;
}

//---------------------------------------------------------------------------------------------
/**
 * @brief ���ݎQ�ƒ��̈ړ��f�[�^���擾����
 *
 * @param work �C�x���g���[�N
 *
 * @return ���ݎQ�ƒ��̈ړ��f�[�^(�ړ����� DIR_xxxx)
 */
//---------------------------------------------------------------------------------------------
static u16 GetMoveDir( const EVENT_WORK* work )
{
  if( !work->autoWay ){ return 0; }
  return work->autoWay->moveDir;
}

//---------------------------------------------------------------------------------------------
/**
 * @brief ���ݎQ�ƒ��̈ړ��f�[�^���擾����
 *
 * @param work �C�x���g���[�N
 *
 * @return ���ݎQ�ƒ��̈ړ��f�[�^(��]�҂�����)
 */
//---------------------------------------------------------------------------------------------
static u16 GetRotWait( const EVENT_WORK* work )
{
  if( !work->autoWay ){ return 0; }
  return work->autoWay->rotWait;
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �����̃A�g���r���[�g���擾����
 *
 * @param work �C�x���g���[�N
 *
 * @return ��l���̑����̃A�g���r���[�g
 */
//---------------------------------------------------------------------------------------------
static MAPATTR_VALUE GetAttribute( const EVENT_WORK* work )
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

//---------------------------------------------------------------------------------------------
/**
 * @brief ���@�̌������X�V����
 *
 * @param work    �C�x���g���[�N
 */
//---------------------------------------------------------------------------------------------
static void UpdateRotDir( EVENT_WORK* work )
{ 
  if( ++work->rotCount % GetRotWait(work) == 0 )
  {
    // ������ύX
    work->rotDir = (work->rotDir + 1) % DIR_MAX4;
    // ���샂�f���ɔ��f
    MMDL_OffStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
    MMDL_SetDirDisp( work->mmdl, work->rotDir );
    MMDL_OnStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
  }
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �ړ����J�n����
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------------------------
static void MoveStart( EVENT_WORK* work )
{ 
  // �A�j���[�V�����R�}���h���Z�b�g
  MMDL_SetAcmd( work->mmdl, GetAcmd(work) ); 

  // �ړ��A�j���[�V�����ɂ������̕ύX���ꎞ��~
  MMDL_OnStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �A�g���r���[�g�`�F�b�N
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------------------------
static void AttributeCheck( EVENT_WORK* work )
{
  const AUTOWAY* new_way;
  MAPATTR_VALUE attrval;

  // �����̃A�g���r���[�g���擾
  attrval = GetAttribute( work );

  // �Q�Ƃ���ړ��f�[�^���X�V
  new_way = GetAutoWay( attrval );
  if( new_way ){ work->autoWay = new_way; }
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �Փ˃`�F�b�N
 *
 * @param work �C�x���g���[�N
 *
 * @return �Փ˂����� TRUE, �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------------------
static BOOL HitCheck( EVENT_WORK* work )
{
  // �i�s�����̃O���b�h���`�F�b�N
  u32 hitbit = MMDL_HitCheckMoveDir( work->mmdl, GetMoveDir(work) );
  return (hitbit != MMDL_MOVEHITBIT_NON);
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�I������
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------------------------
static void ExitEvent( EVENT_WORK* work )
{
  // �ύX�����X�e�[�^�X�r�b�g�����ɖ߂�
  MMDL_OffStatusBit( work->mmdl, MMDL_STABIT_PAUSE_DIR );

  // �i�s����������
  if( work->autoWay )
  {
    MMDL_SetDirDisp( work->mmdl, GetMoveDir(work) );
  }
}
