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
  u16  frame;     // ����t���[����
  u16  maxFrame;  // �ő�t���[����
  u8   rotateNum; // ��]��
  FIELDMAP_WORK* pFieldmap; // ����Ώۂ̃t�B�[���h�}�b�v
}
TCB_WORK_ROT_HERO;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
static void DeleteTask( GFL_TCB* tcb );

static void TCB_FUNC_RotPlayer_Accel( GFL_TCB* tcb, void* work );
static void TCB_FUNC_RotPlayer_Decel( GFL_TCB* tcb, void* work );

static u16 CalcRotateDir_Accel( float frame, float max_frame, float rotate_num );
static u16 CalcRotateDir_Decel( float frame, float max_frame, float rotate_num );


//========================================================================================== 
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief ���@��]�^�X�N��ǉ�����
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_ROT_PLAYER_AddTask_SpeedUp( FIELDMAP_WORK* fieldmap, int frame, int rot_num )
{
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*      tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TCB_WORK_ROT_HERO* work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TCB_WORK_ROT_HERO ) );
  GFL_TCB*            tcb;

  // TCB���[�N������
  work->frame     = 0;
  work->maxFrame  = frame;
  work->rotateNum = rot_num;
  work->pFieldmap = fieldmap;

  // TCB��ǉ�
  tcb = GFL_TCB_AddTask( tcbsys, TCB_FUNC_RotPlayer_Accel, work, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���@��]�^�X�N��ǉ�����
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_ROT_PLAYER_AddTask_SlowDown( FIELDMAP_WORK* fieldmap, int frame, int rot_num )
{
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*      tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TCB_WORK_ROT_HERO* work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TCB_WORK_ROT_HERO ) );
  GFL_TCB*            tcb;

  // TCB���[�N������
  work->frame     = 0;
  work->maxFrame  = frame;
  work->rotateNum = rot_num;
  work->pFieldmap = fieldmap;

  // TCB��ǉ�
  tcb = GFL_TCB_AddTask( tcbsys, TCB_FUNC_RotPlayer_Decel, work, 0 );
}


//========================================================================================== 
/*
 * @brief ����J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��j������
 *
 * @param tcb �j������^�X�N
 */
//------------------------------------------------------------------------------------------
static void DeleteTask( GFL_TCB* tcb )
{
  // TCB���[�N��j��
  GFL_HEAP_FreeMemory( GFL_TCB_GetWork( tcb ) );

  // �^�X�N��j��
  GFL_TCB_DeleteTask( tcb );
}

//------------------------------------------------------------------------------------------
/**
 * @breif TCB���s�֐�( ���@�̉�����] )
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_RotPlayer_Accel( GFL_TCB* tcb, void* work )
{
  TCB_WORK_ROT_HERO* tcbwork = work;
  FIELD_PLAYER*       player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*                 mmdl = FIELD_PLAYER_GetMMdl( player ); 
  u16                anm_cmd = CalcRotateDir_Accel( tcbwork->frame, tcbwork->maxFrame, tcbwork->rotateNum );

  // ���샂�f���̌������X�V
  MMDL_SetAcmd( mmdl, anm_cmd );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->maxFrame < tcbwork->frame )
  {
    DeleteTask( tcb );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @breif TCB���s�֐�( ���@�̌�����] )
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_RotPlayer_Decel( GFL_TCB* tcb, void* work )
{
  TCB_WORK_ROT_HERO* tcbwork = work;
  FIELD_PLAYER*       player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*                 mmdl = FIELD_PLAYER_GetMMdl( player ); 
  u16                anm_cmd = CalcRotateDir_Decel( tcbwork->frame, tcbwork->maxFrame, tcbwork->rotateNum );

  // ���샂�f���̌������X�V
  MMDL_SetAcmd( mmdl, anm_cmd );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->maxFrame < tcbwork->frame )
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
static u16 CalcRotateDir_Accel( float frame, float max_frame, float rotate_num )
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
static u16 CalcRotateDir_Decel( float frame, float max_frame, float rotate_num )
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
