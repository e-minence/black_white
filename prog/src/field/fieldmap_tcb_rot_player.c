////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �t�B�[���h�}�b�vTCB( ���@�̉�] )
 * @file   fieldmap_tcb_rot_player.c
 * @author obata
 * @date   2009.08.25
 *
 */
////////////////////////////////////////////////////////////////////////
#include <math.h>     // sqrt
#include "include/field/fieldmap_proc.h"
#include "include/gamesystem/game_event.h"
#include "fieldmap_tcb.h"
#include "fieldmap.h"
#include "field_player.h"


//==========================================================================================
/**
 * @brief TCB���[�N
 */
//==========================================================================================
typedef struct
{
  FIELDMAP_WORK* pFieldmap; // ����Ώۂ̃t�B�[���h�}�b�v
  u16            frame;     // ����t���[����
  u16            endFrame;  // �ŏI�t���[����
  u8             rotateNum; // ��]��
}
TASK_WORK;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
static void InitWork( TASK_WORK* work, FIELDMAP_WORK* fieldmap, u16 end_frame, u8 rot_num );
static void DeleteTask( GFL_TCB* tcb );

static void TCB_FUNC_RotPlayer( GFL_TCB* tcb, void* work );
static void TCB_FUNC_RotPlayer_SpeedUp( GFL_TCB* tcb, void* work );
static void TCB_FUNC_RotPlayer_SpeedDown( GFL_TCB* tcb, void* work );

static u16 CalcPlayerDir( float frame, float max_frame, float rotate_num );
static u16 CalcPlayerDir_SpeedUp( float frame, float max_frame, float rotate_num );
static u16 CalcPlayerDir_SpeedDown( float frame, float max_frame, float rotate_num );


//========================================================================================== 
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( ���@�̓�����] )
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_RotatePlayer( FIELDMAP_WORK* fieldmap, int frame, int rot_num )
{
  HEAPID     heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS* tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TASK_WORK*    work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TASK_WORK ) );

  // TCB���[�N������
  InitWork( work, fieldmap, frame, rot_num );

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_RotPlayer, work, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( ���@�̉�����] )
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_RotatePlayer_SpeedUp( FIELDMAP_WORK* fieldmap, int frame, int rot_num )
{
  HEAPID     heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS* tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TASK_WORK*    work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TASK_WORK ) );

  // TCB���[�N������
  InitWork( work, fieldmap, frame, rot_num );

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_RotPlayer_SpeedUp, work, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( ���@�̌�����] )
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_RotatePlayer_SpeedDown( FIELDMAP_WORK* fieldmap, int frame, int rot_num )
{
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*      tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TASK_WORK* work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TASK_WORK ) );

  // TCB���[�N������
  work->frame     = 0;
  work->endFrame  = frame;
  work->rotateNum = rot_num;
  work->pFieldmap = fieldmap;

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_RotPlayer_SpeedDown, work, 0 );
}


//========================================================================================== 
/*
 * @brief ����J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N�E���[�N������������
 *
 * @param work      ���������郏�[�N
 * @param fieldmap  ����Ώۂ̃t�B�[���h�}�b�v
 * @param end_frame �ŏI�t���[����
 * @param rot_num   ��]��
 */
//------------------------------------------------------------------------------------------
static void InitWork( TASK_WORK* work, FIELDMAP_WORK* fieldmap, u16 end_frame, u8 rot_num )
{
  work->pFieldmap = fieldmap;
  work->frame     = 0;
  work->endFrame  = end_frame;
  work->rotateNum = rot_num;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��j������
 *
 * @param tcb �j������^�X�N
 */
//------------------------------------------------------------------------------------------
static void DeleteTask( GFL_TCB* tcb )
{
  // �^�X�N�E���[�N��j��
  GFL_HEAP_FreeMemory( GFL_TCB_GetWork( tcb ) );

  // �^�X�N��j��
  GFL_TCB_DeleteTask( tcb );
}

//------------------------------------------------------------------------------------------
/**
 * @breif �^�X�N���s�֐�( ���@�̓�����] )
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_RotPlayer( GFL_TCB* tcb, void* work )
{
  TASK_WORK* tcbwork = work;
  FIELD_PLAYER*       player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*                 mmdl = FIELD_PLAYER_GetMMdl( player ); 
  u16                anm_cmd = CalcPlayerDir( tcbwork->frame, tcbwork->endFrame, tcbwork->rotateNum );

  // ���샂�f���̌������X�V
  MMDL_SetAcmd( mmdl, anm_cmd );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->endFrame < tcbwork->frame )
  {
    DeleteTask( tcb );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @breif TCB���s�֐�( ���@�̉�����] )
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_RotPlayer_SpeedUp( GFL_TCB* tcb, void* work )
{
  TASK_WORK*   tcbwork = work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
  u16          anm_cmd;

  // ���샂�f���̌������X�V
  anm_cmd = CalcPlayerDir_SpeedUp( tcbwork->frame, tcbwork->endFrame, tcbwork->rotateNum );
  MMDL_SetAcmd( mmdl, anm_cmd );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->endFrame < tcbwork->frame )
  {
    DeleteTask( tcb );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @breif TCB���s�֐�( ���@�̌�����] )
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_RotPlayer_SpeedDown( GFL_TCB* tcb, void* work )
{
  TASK_WORK*   tcbwork = work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
  u16          anm_cmd;
  
  // ���샂�f���̌������X�V
  anm_cmd = CalcPlayerDir_SpeedDown( tcbwork->frame, tcbwork->endFrame, tcbwork->rotateNum );
  MMDL_SetAcmd( mmdl, anm_cmd );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->endFrame < tcbwork->frame )
  {
    DeleteTask( tcb );
  }
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
