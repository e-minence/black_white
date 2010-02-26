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

#include "sound/pm_sndsys.h"
#include "field_player.h"
#include "fldmmdl.h"
#include "fldmmdl_code.h"
#include "map_attr_def.h"     // for MATTR_xxxx
#include "fldeff_namipoke.h"  // for NAMIPOKE_


//=============================================================================================
// ���萔
//=============================================================================================
#define SEQ_JUMP   (SEQ_SE_DANSA)   // �W�����vSE
#define SEQ_SPLASH (SEQ_SE_FLD_83)  // ����SE
#define SEQ_WATER  (SEQ_SE_FLD_129) // ����SE

// �����V�[�P���X
enum {
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
  {MATTR_JUMP_RIGHT, AC_JUMP_R_3G_24F},  //�E�����փW�����v�ړ�
  {MATTR_JUMP_LEFT,  AC_JUMP_L_3G_24F},  //�������փW�����v�ړ�
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
  GAMESYS_WORK*  gsys;     // �Q�[���V�X�e��
  FIELDMAP_WORK* fieldmap; // �t�B�[���h�}�b�v
  FIELD_PLAYER*  player;   // ���@
  MMDL*          mmdl;     // ���@�̓��샂�f��

  const CURRENT_DATA* current; // �K�p���̐����f�[�^
  const ROCK_DATA*    rock;    // �K�p���̊�f�[�^

  BOOL jumpFlag; // �W�����v�����ǂ���

  // SE�v���C���[ID
  SEPLAYER_ID SEPlayerID_jump;   // �W�����v
  SEPLAYER_ID SEPlayerID_splash; // ����
  SEPLAYER_ID SEPlayerID_water;  // ����

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
static void PlayJumpSE( EVENT_WORK* work );
static void PlaySplashSE( EVENT_WORK* work );
static void StartWaterSE( EVENT_WORK* work );
static void StopWaterSE( EVENT_WORK* work );


//=============================================================================================
// ���C�x���g�����֐�
//=============================================================================================
static GMEVENT_RESULT AutoMoveEvent( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  // �C�x���g�J�n
  case SEQ_START:
    // �������ړ����łȂ���ΏI��
    if( !IsCurrent( GetNowAttribute(work) ) ) {
      *seq = SEQ_EXIT;
      break;
    }
    // �g�|�P�㕔�̐��򖗃G�t�F�N�gOFF
    {
      FLDEFF_TASK* task;
      task   = FIELD_PLAYER_GetEffectTaskWork( work->player );
      FLDEFF_NAMIPOKE_SetRippleEffect( task, FALSE );
    }
    // ����SE�Đ��J�n
    StartWaterSE( work );
    *seq = SEQ_MAIN;
    break;

  // ���C������
  case SEQ_MAIN:
    // ����SE�Đ��J�n
    StartWaterSE( work );

    // �ړ��I����
    if( MMDL_CheckEndAcmd(work->mmdl) )
    {
      SplashCheck( work );  // �����`�F�b�N
      CurrentCheck( work ); // �����`�F�b�N
      RockCheck( work );    // ��`�F�b�N

      // �������Ȃ��Ȃ�����I��
      if( (work->current == NULL) && (work->rock == NULL) ) {
        StopWaterSE( work ); // ����SE��~
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
  FIELD_PLAYER* player;

  player = FIELDMAP_GetFieldPlayer( fieldmap );

  // ����
  event = GMEVENT_Create( gsys, parent, AutoMoveEvent, sizeof(EVENT_WORK) );

  // ������
  work = GMEVENT_GetEventWork( event );
  work->gsys              = gsys;
  work->fieldmap          = fieldmap;
  work->player            = player;
  work->mmdl              = FIELD_PLAYER_GetMMdl( player );
  work->current           = NULL;
  work->rock              = NULL;
  work->jumpFlag          = FALSE;
  work->SEPlayerID_jump   = PMSND_GetSE_DefaultPlayerID( SEQ_JUMP );
  work->SEPlayerID_splash = PMSND_GetSE_DefaultPlayerID( SEQ_SPLASH );
  work->SEPlayerID_water  = PMSND_GetSE_DefaultPlayerID( SEQ_WATER );

  OS_TFPrintf( 3, "jump   : %d\n",work->SEPlayerID_jump );
  OS_TFPrintf( 3, "splash : %d\n",work->SEPlayerID_splash );
  OS_TFPrintf( 3, "water  : %d\n",work->SEPlayerID_water );
  return event;
} 

//---------------------------------------------------------------------------------------------
/**
 * @brief �����A�g���r���[�g����
 *
 * @param attrval �A�g���r���[�g
 *
 * @return �w�肵���A�g���r���[�g�������Ȃ� TRUE, �����łȂ��Ȃ� FALSE
 */
//---------------------------------------------------------------------------------------------
BOOL CheckAttributeIsCurrent( MAPATTR_VALUE attrval )
{
  return IsCurrent( attrval );
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
  if( !work->current ) { return 0; }
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
  if( !work->rock ) { return 0; }
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
  if( !work->current ) { return 0; }
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
  if( work->rock ) { // �W�����v
    MMDL_SetAcmd( work->mmdl, GetJumpAcmd(work) );
    PlayJumpSE( work );
    work->jumpFlag = TRUE;
  }
  else if( work->current ) { // �ړ�
    MMDL_SetAcmd( work->mmdl, GetMoveAcmd(work) );
  }  
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
    FLDEFF_CTRL *fectrl;
    FLDEFF_PROCID proc_id;
    NAMIPOKE_EFFECT_TYPE type;
    VecFx32 pos;

    fectrl  = FIELDMAP_GetFldEffCtrl( work->fieldmap );
    proc_id = FLDEFF_PROCID_NAMIPOKE_EFFECT;

    // �G�t�F�N�g�o�^
    if( FLDEFF_CTRL_CheckRegistEffect( fectrl, proc_id ) == FALSE ) {
      FLDEFF_CTRL_RegistEffect( fectrl, &proc_id, 1 );
    }
    // �G�t�F�N�g��\��
    type = NAMIPOKE_EFFECT_TYPE_TAKI_SPLASH;
    FIELD_PLAYER_GetPos( work->player, &pos );
    FLDEFF_NAMIPOKE_EFFECT_SetEffectAlone( fectrl, type, &pos );

    // ����SE �Đ�
    PlaySplashSE( work );

    // �W�����v�I��
    work->rock = NULL;
    work->jumpFlag = FALSE;
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
  // �g�|�P�㕔�̐��򖗃G�t�F�N�gON
  {
    FLDEFF_TASK* task;
    task = FIELD_PLAYER_GetEffectTaskWork( work->player );
    FLDEFF_NAMIPOKE_SetRippleEffect( task, TRUE );
  }
}

//---------------------------------------------------------------------------------------------
/**
 * @brief �W�����vSE���Đ�����
 *
 * @param work
 */
//---------------------------------------------------------------------------------------------
static void PlayJumpSE( EVENT_WORK* work )
{
  // �T�E���h���[�h��
  if( PMSND_IsLoading() ) { return; }

  PMSND_StopSE_byPlayerID( work->SEPlayerID_water );
  PMSND_PlaySE_byPlayerID( SEQ_JUMP, work->SEPlayerID_jump );
}

//---------------------------------------------------------------------------------------------
/**
 * @brief ����SE���Đ�����
 *
 * @param work
 */
//---------------------------------------------------------------------------------------------
static void PlaySplashSE( EVENT_WORK* work )
{
  // �T�E���h���[�h��
  if( PMSND_IsLoading() ) { return; }

  PMSND_PlaySE_byPlayerID( SEQ_SPLASH, work->SEPlayerID_splash );
}

//---------------------------------------------------------------------------------------------
/**
 * @brief ����SE�̍Đ����J�n����
 *
 * @param work
 */
//---------------------------------------------------------------------------------------------
static void StartWaterSE( EVENT_WORK* work )
{
  // �T�E���h���[�h��
  if( PMSND_IsLoading() ) { return; }

  // �W�����v�I��
  if( ( work->jumpFlag == FALSE ) &&
      ( PMSND_CheckPlaySE_byPlayerID( work->SEPlayerID_water ) == FALSE ) )
  {
    PMSND_PlaySE_byPlayerID( SEQ_WATER, work->SEPlayerID_water );
  }
} 

//---------------------------------------------------------------------------------------------
/**
 * @brief ����SE���~����
 *
 * @param work
 */
//---------------------------------------------------------------------------------------------
static void StopWaterSE( EVENT_WORK* work )
{
  PMSND_StopSE_byPlayerID( work->SEPlayerID_water );
}
