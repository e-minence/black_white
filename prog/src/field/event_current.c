///////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �����ɂ�鋭���ړ��C�x���g
 * @file   event_current.c
 * @author obata
 * @date   2009.11.27
 */
///////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "event_current.h"

#include "field_player.h"
#include "fldmmdl.h"
#include "fldmmdl_code.h"
#include "map_attr_def.h"  // for MATTR_xxxx
#include "fldeff_namipoke.h"  // for NAMIPOKE_
#include "field_player_grid.h"  // for FIELD_PLAYER_GRID
#include "fieldmap_ctrl.h"  // for FIELDMAP_GetPlayerGrid


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
// ������
//=============================================================================================
typedef struct 
{
  MAPATTR_VALUE attrval;  // �N���A�g���r���[�g
  u16          moveAcmd;  // ���@�ɓK�p����ړ��A�j���[�V�����R�}���h( AC_xxxx )
  u16           moveDir;  // �ړ�����( DIR_xxxx )

} CURRENT_DATA;

//---------------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^
 */
//---------------------------------------------------------------------------------------------
static const CURRENT_DATA current[] = 
{
  {MATTR_CURRENT_LEFT,   AC_WALK_L_3F, DIR_LEFT},  //�E�����E�C
  {MATTR_CURRENT_RIGHT,  AC_WALK_R_3F, DIR_RIGHT}, //�������E�C
  {MATTR_CURRENT_UP,     AC_WALK_U_3F, DIR_UP},    //�㐅���E�C
  {MATTR_CURRENT_DOWN,   AC_WALK_D_3F, DIR_DOWN},  //�������E�C
};

//---------------------------------------------------------------------------------------------
/**
 * @brief �����f�[�^���擾����
 *
 * @param attrval �A�g���r���[�g
 *
 * @return �w�肵���A�g���r���[�g�ɑΉ����鐅���f�[�^
 *         �Ή�����f�[�^�����݂��Ȃ��ꍇ, NULL
 */
//---------------------------------------------------------------------------------------------
static const CURRENT_DATA* GetCurrentData( MAPATTR_VALUE attrval )
{
  int i;
  for(i=0; i<NELEMS(current); i++ )
  {
    if( current[i].attrval == attrval )
    {
      return &current[i];
    }
  }
  return NULL;
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �����A�g���r���[�g���ǂ����𔻒肷��
 *
 * @param attrval �A�g���r���[�g
 *
 * @return �w��A�g���r���[�g���ړ����Ȃ� TRUE, �����łȂ��Ȃ� FALSE
 */
//---------------------------------------------------------------------------------------------
static BOOL IsCurrent( MAPATTR_VALUE attrval )
{
  return (GetCurrentData(attrval) != NULL);
}


//=============================================================================================
// ����
//=============================================================================================
typedef struct
{
  MAPATTR_VALUE attrval;  // �A�g���r���[�g
  u16          jumpAcmd;  // ���@�ɓK�p����W�����v�A�j���[�V�����R�}���h( AC_xxxx )

} ROCK_DATA;

//---------------------------------------------------------------------------------------------
/**
 * @brief ��f�[�^
 */
//---------------------------------------------------------------------------------------------
static const ROCK_DATA rock[] = 
{
  {MATTR_JUMP_RIGHT, AC_JUMP_L_3G_24F},  //�E�����փW�����v�ړ�
  {MATTR_JUMP_LEFT,  AC_JUMP_R_3G_24F},  //�������փW�����v�ړ�
  {MATTR_JUMP_UP,    AC_JUMP_U_3G_24F},  //������փW�����v�ړ�
  {MATTR_JUMP_DOWN,  AC_JUMP_D_3G_24F},  //�������փW�����v�ړ�
};

//---------------------------------------------------------------------------------------------
/**
 * @brief ��f�[�^���擾����
 *
 * @param attrval �A�g���r���[�g
 *
 * @return �w�肵���A�g���r���[�g�ɑΉ������f�[�^
 *         �Ή�����f�[�^�����݂��Ȃ��ꍇ, NULL
 */
//---------------------------------------------------------------------------------------------
static const ROCK_DATA* GetRockData( MAPATTR_VALUE attrval )
{
  int i;
  for(i=0; i<NELEMS(rock); i++ )
  {
    if( rock[i].attrval == attrval )
    {
      return &rock[i];
    }
  }
  return NULL;
}

//---------------------------------------------------------------------------------------------
/**
 * @brief ��A�g���r���[�g���ǂ����𔻒肷��
 *
 * @param attrval �A�g���r���[�g
 *
 * @return �w��A�g���r���[�g���ړ����Ȃ� TRUE, �����łȂ��Ȃ� FALSE
 */
//---------------------------------------------------------------------------------------------
static BOOL IsRock( MAPATTR_VALUE attrval )
{
  return (GetRockData(attrval) != NULL);
}


//=============================================================================================
// ���C�x���g���[�N
//=============================================================================================
typedef struct
{
  GAMESYS_WORK*          gsys;  // �Q�[���V�X�e��
  FIELDMAP_WORK*     fieldmap;  // �t�B�[���h�}�b�v
  FIELD_PLAYER*        player;  // ���@
  MMDL*                  mmdl;  // ���@�̓��샂�f��
  const CURRENT_DATA* current;  // �K�p���̐����f�[�^
  const ROCK_DATA*       rock;  // �K�p���̊�f�[�^

} EVENT_WORK;


//=============================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//=============================================================================================
static u16 GetMoveAcmd( const EVENT_WORK* work );
static u16 GetJumpAcmd( const EVENT_WORK* work );
static u16 GetMoveDir( const EVENT_WORK* work );
static MAPATTR_VALUE GetNowAttribute( const EVENT_WORK* work );
static MAPATTR_VALUE GetFrontAttribute( const EVENT_WORK* work );
static void MoveStart( EVENT_WORK* work );
static void SplashCheck( EVENT_WORK* work );
static void CurrentCheck( EVENT_WORK* work );
static void RockCheck( EVENT_WORK* work );
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
    if( !IsCurrent( GetNowAttribute(work) ) )
    {
      *seq = SEQ_EXIT;
      break;
    }
    *seq = SEQ_MAIN;
    break;
  // ���C������
  case SEQ_MAIN:
    // �ړ��I����
    if( MMDL_CheckEndAcmd(work->mmdl) ) // if(�ړ��I��)
    {
      // �����`�F�b�N
      SplashCheck( work );
      // �����`�F�b�N
      CurrentCheck( work );
      // ��`�F�b�N
      RockCheck( work );
      // �������Ȃ��Ȃ�����I��
      if( (work->current == NULL) && (work->rock == NULL) )
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
 * @brief �����ړ��C�x���g���쐬����
 *
 * @param parent   �e�ƂȂ�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �ړ����C�x���g
 */
//---------------------------------------------------------------------------------------------
GMEVENT* EVENT_PlayerMoveOnCurrent( GMEVENT* parent, 
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
  work->current  = NULL;
  work->rock     = NULL;
  return event;
} 


//=============================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//=============================================================================================

//---------------------------------------------------------------------------------------------
/**
 * @brief ���ݎQ�ƒ��̐����f�[�^���擾����
 *
 * @param work �C�x���g���[�N
 *
 * @return ���ݎQ�ƒ��̐����f�[�^(�ړ��A�j���[�V�����R�}���h AC_xxxx)
 */
//---------------------------------------------------------------------------------------------
static u16 GetMoveAcmd( const EVENT_WORK* work )
{
  if( !work->current ){ return 0; }
  return work->current->moveAcmd;
}

//---------------------------------------------------------------------------------------------
/**
 * @brief ���ݎQ�ƒ��̐����f�[�^���擾����
 *
 * @param work �C�x���g���[�N
 *
 * @return ���ݎQ�ƒ��̐����f�[�^(�W�����v�E�A�j���[�V�����R�}���h AC_xxxx)
 */
//---------------------------------------------------------------------------------------------
static u16 GetJumpAcmd( const EVENT_WORK* work )
{
  if( !work->rock ){ return 0; }
  return work->rock->jumpAcmd;
}

//---------------------------------------------------------------------------------------------
/**
 * @brief ���ݎQ�ƒ��̐����f�[�^���擾����
 *
 * @param work �C�x���g���[�N
 *
 * @return ���ݎQ�ƒ��̐����f�[�^(�ړ����� DIR_xxxx)
 */
//---------------------------------------------------------------------------------------------
static u16 GetMoveDir( const EVENT_WORK* work )
{
  if( !work->current ){ return 0; }
  return work->current->moveDir;
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
static MAPATTR_VALUE GetNowAttribute( const EVENT_WORK* work )
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
 * @brief �O���̃A�g���r���[�g���擾����
 *
 * @param work �C�x���g���[�N
 *
 * @return ��l����1�O���b�h��̃A�g���r���[�g
 */
//---------------------------------------------------------------------------------------------
static MAPATTR_VALUE GetFrontAttribute( const EVENT_WORK* work )
{
  u16 dir;
  VecFx32 pos;
  MAPATTR attr;
  MAPATTR_VALUE attrval;
  FLDMAPPER* mapper;

  // 1�O���b�h��̃A�g���r���[�g���擾
  dir = MMDL_GetDirDisp( work->mmdl );
  FIELD_PLAYER_GetPos( work->player, &pos );
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  mapper  = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
  attr    = MAPATTR_GetAttribute( mapper, &pos );
  attrval = MAPATTR_GetAttrValue( attr );
  return attrval;
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
  if( work->rock ){ MMDL_SetAcmd( work->mmdl, GetJumpAcmd(work) ); }  // �W�����v
  else if( work->current ){ MMDL_SetAcmd( work->mmdl, GetMoveAcmd(work) ); }  // �ړ�
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �����`�F�b�N
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------------------------
static void SplashCheck( EVENT_WORK* work )
{
  if( work->rock )
  {
    FLDEFF_TASK *task;
    FLDEFF_CTRL *fectrl;
    NAMIPOKE_EFFECT_TYPE type;
    FIELD_PLAYER_GRID *gjiki;
    
    fectrl = FIELDMAP_GetFldEffCtrl( work->fieldmap );
    gjiki  = FIELDMAP_GetPlayerGrid( work->fieldmap );
    type   = NAMIPOKE_EFFECT_TYPE_TAKI_SPLASH;
    task   = FIELD_PLAYER_GRID_GetEffectTaskWork( gjiki );
    FLDEFF_NAMIPOKE_SetJointFlag( task, NAMIPOKE_JOINT_ON );
    FLDEFF_NAMIPOKE_EFFECT_SetEffect( fectrl, type, task );
  }
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �����A�g���r���[�g�`�F�b�N
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------------------------
static void CurrentCheck( EVENT_WORK* work )
{
  MAPATTR_VALUE attrval;

  // �����̃A�g���r���[�g���擾
  attrval = GetNowAttribute( work );

  // �Q�Ƃ��鐅���f�[�^���X�V
  work->current = GetCurrentData( attrval );
}

//---------------------------------------------------------------------------------------------
/**
 * @brief ��A�g���r���[�g�`�F�b�N
 *
 * @param work �C�x���g���[�N
 */
//---------------------------------------------------------------------------------------------
static void RockCheck( EVENT_WORK* work )
{
  const ROCK_DATA* rock;
  MAPATTR_VALUE attrval;

  // �����̃A�g���r���[�g���擾
  attrval = GetNowAttribute( work );
  
  // �Y�������f�[�^���擾
  work->rock = GetRockData( attrval );
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
}
