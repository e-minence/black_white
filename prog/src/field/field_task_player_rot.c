////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N(���@�̉�]����)
 * @file   field_task_player_rot.c
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <math.h>     // sqrt
#include "fieldmap.h"
#include "field_player.h"
#include "field_task.h"
#include "field_task_player_rot.h"


//==========================================================================================
// ���^�X�N���[�N
//==========================================================================================
typedef struct
{
  FIELDMAP_WORK* fieldmap; // ����Ώۂ̃t�B�[���h�}�b�v
  MMDL*          mmdl;      // ����Ώۂ̓��샂�f��
  u16            frame;     // ����t���[����
  u16            endFrame;  // �ŏI�t���[����
  u8             rotateNum; // ��]��
  u16            initDir;   // �C�x���g�J�n���̌���

} TASK_WORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static u16 CalcPlayerDir( float frame, float max_frame, float rotate_num );
static u16 CalcPlayerDir_SpeedUp( float frame, float max_frame, float rotate_num );
static u16 CalcPlayerDir_SpeedDown( float frame, float max_frame, float rotate_num );
static FIELD_TASK_RETVAL RotatePlayer( void *wk );
static FIELD_TASK_RETVAL RotatePlayer_SpeedUp( void *wk );
static FIELD_TASK_RETVAL RotatePlayer_SpeedDown( void *wk );


//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̓�����]�^�X�N���쐬����
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_PlayerRotate( FIELDMAP_WORK* fieldmap, int frame, int rot_num )
{
  FIELD_PLAYER* player;
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id;

  heap_id = FIELDMAP_GetHeapID( fieldmap );
  player  = FIELDMAP_GetFieldPlayer( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), RotatePlayer );

  // ������
  work = FIELD_TASK_GetWork( task );
  work->fieldmap  = fieldmap;
  work->mmdl      = FIELD_PLAYER_GetMMdl( player ); 
  work->frame     = 0;
  work->endFrame  = frame;
  work->rotateNum = rot_num;
  work->initDir   = MMDL_GetDirDisp( work->mmdl );

  return task;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̓�����]�^�X�N���쐬����
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 * @param mmdl     ����Ώۂ̓��샂�f��
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_PlayerRotateEX( FIELDMAP_WORK* fieldmap, int frame, int rot_num, MMDL* mmdl )
{
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id;

  heap_id = FIELDMAP_GetHeapID( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), RotatePlayer );

  // ������
  work = FIELD_TASK_GetWork( task );
  work->fieldmap  = fieldmap;
  work->mmdl      = mmdl;
  work->frame     = 0;
  work->endFrame  = frame;
  work->rotateNum = rot_num;
  work->initDir   = MMDL_GetDirDisp( mmdl );

  return task;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̉�����]�^�X�N���쐬����
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_PlayerRotate_SpeedUp( FIELDMAP_WORK* fieldmap, int frame, int rot_num )
{
  FIELD_PLAYER* player;
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id;

  heap_id = FIELDMAP_GetHeapID( fieldmap );
  player  = FIELDMAP_GetFieldPlayer( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), RotatePlayer_SpeedUp );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->fieldmap  = fieldmap;
  work->mmdl      = FIELD_PLAYER_GetMMdl( player ); 
  work->frame     = 0;
  work->endFrame  = frame;
  work->rotateNum = rot_num;
  work->initDir   = MMDL_GetDirDisp( work->mmdl );

  return task;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̌�����]�^�X�N���쐬����
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_PlayerRotate_SpeedDown( FIELDMAP_WORK* fieldmap, int frame, int rot_num )
{
  FIELD_PLAYER* player;
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id;

  heap_id = FIELDMAP_GetHeapID( fieldmap );
  player  = FIELDMAP_GetFieldPlayer( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), RotatePlayer_SpeedDown );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->fieldmap  = fieldmap;
  work->mmdl      = FIELD_PLAYER_GetMMdl( player ); 
  work->frame     = 0;
  work->endFrame  = frame;
  work->rotateNum = rot_num;
  work->initDir   = MMDL_GetDirDisp( work->mmdl );

  return task;
}


//========================================================================================== 
// ������J�֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
 /**
  * @brief ��]���̌������Z�o����
  *
  * @param frame      ���݂̃t���[����
  * @param max_frame  �ő�t���[����
  * @param rotate_num �S��]��
  *
  * @return ���샂�f���̃A�j���[�V�����R�}���h( AC_DIR_D �Ȃ� )
  */
//------------------------------------------------------------------------------------------
static u16 CalcPlayerDir( float frame, float max_frame, float rotate_num )
{
  u16 acmd_list[] =
  {
    AC_DIR_D,
    AC_DIR_R,
    AC_DIR_U,
    AC_DIR_L,
  };

  float x     = frame;
  float x_max = max_frame;
  float y_max = 4 * rotate_num;
  float a     = y_max / x_max;
  float y     = a * x;
  int anime   = (int)y % 4;

  return acmd_list[ anime ];
} 

//------------------------------------------------------------------------------------------
 /**
  * @brief ��]���̌������Z�o����
  *
  * @param frame      ���݂̃t���[����
  * @param max_frame  �ő�t���[����
  * @param rotate_num �S��]��
  *
  * @return ���샂�f���̃A�j���[�V�����R�}���h( AC_DIR_D �Ȃ� )
  */
//------------------------------------------------------------------------------------------
static u16 CalcPlayerDir_SpeedUp( float frame, float max_frame, float rotate_num )
{
  u16 acmd_list[] =
  {
    AC_DIR_D,
    AC_DIR_R,
    AC_DIR_U,
    AC_DIR_L,
  };

  float x     = frame;
  float x_max = max_frame;
  float y_max = 4 * rotate_num;
  float a     = y_max / ( x_max * x_max );
  float y     = a * x * x;
  int anime   = (int)y % 4;

  return acmd_list[ anime ];
} 

//------------------------------------------------------------------------------------------
 /**
  * @brief ��]���̌������Z�o����
  *
  * @param frame      ���݂̃t���[����
  * @param max_frame  �ő�t���[����
  * @param rotate_num �S��]��
  *
  * @return ���샂�f���̃A�j���[�V�����R�}���h( AC_DIR_D �Ȃ� )
  */
//------------------------------------------------------------------------------------------
static u16 CalcPlayerDir_SpeedDown( float frame, float max_frame, float rotate_num )
{
  u16 acmd_list[] =
  {
    AC_DIR_D,
    AC_DIR_R,
    AC_DIR_U,
    AC_DIR_L,
  };

  float x     = frame;
  float x_max = max_frame;
  float y_max = 4 * rotate_num;
  float a     = (y_max * y_max) / x_max;
  float y     = sqrt(a * x);
  int anime   = (int)y % 4;

  return acmd_list[ anime ];
}


//========================================================================================== 
// ���^�X�N�����֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @breif ���@�̓�����]����
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL RotatePlayer( void *wk )
{
  TASK_WORK* work    = wk;
  MMDL*      mmdl    = work->mmdl;
  u16        anm_cmd = CalcPlayerDir( work->frame, work->endFrame, work->rotateNum );

  // ���샂�f���̌������X�V
  MMDL_SetAcmd( mmdl, anm_cmd );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  work->frame++;
  if( work->endFrame < work->frame ) {
    // ���@�̌��������ɖ߂�
    switch( work->initDir ) {
    default:
    case DIR_UP:    anm_cmd = AC_DIR_U; break;
    case DIR_DOWN:  anm_cmd = AC_DIR_D; break;
    case DIR_LEFT:  anm_cmd = AC_DIR_L; break;
    case DIR_RIGHT: anm_cmd = AC_DIR_R; break;
    }
    MMDL_SetAcmd( mmdl, anm_cmd );
    // �I��
    return FIELD_TASK_RETVAL_FINISH;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @breif ���@�̉�����]����
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL RotatePlayer_SpeedUp( void *wk )
{
  TASK_WORK* work    = wk;
  MMDL*      mmdl    = work->mmdl;
  u16        anm_cmd = CalcPlayerDir_SpeedUp( work->frame, work->endFrame, work->rotateNum );

  // ���샂�f���̌������X�V
  MMDL_SetAcmd( mmdl, anm_cmd );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  work->frame++;
  if( work->endFrame < work->frame ) {
    // ���@�̌��������ɖ߂�
    switch( work->initDir ) {
    default:
    case DIR_UP:    anm_cmd = AC_DIR_U; break;
    case DIR_DOWN:  anm_cmd = AC_DIR_D; break;
    case DIR_LEFT:  anm_cmd = AC_DIR_L; break;
    case DIR_RIGHT: anm_cmd = AC_DIR_R; break;
    }
    MMDL_SetAcmd( mmdl, anm_cmd );
    // �I��
    return FIELD_TASK_RETVAL_FINISH;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @breif ���@�̌�����]����
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL RotatePlayer_SpeedDown( void *wk )
{
  TASK_WORK* work    = wk;
  MMDL*      mmdl    = work->mmdl;
  u16        anm_cmd = CalcPlayerDir_SpeedDown( work->frame, work->endFrame, work->rotateNum );

  // ���샂�f���̌������X�V
  MMDL_SetAcmd( mmdl, anm_cmd );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  work->frame++;
  if( work->endFrame < work->frame ) {
    // ���@�̌��������ɖ߂�
    switch( work->initDir ) {
    default:
    case DIR_UP:    anm_cmd = AC_DIR_U; break;
    case DIR_DOWN:  anm_cmd = AC_DIR_D; break;
    case DIR_LEFT:  anm_cmd = AC_DIR_L; break;
    case DIR_RIGHT: anm_cmd = AC_DIR_R; break;
    }
    //MMDL_SetAcmd( mmdl, anm_cmd );
    MMDL_SetDirDisp( mmdl, work->initDir );
    return FIELD_TASK_RETVAL_FINISH;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
} 
