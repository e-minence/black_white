////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �t�B�[���h�}�b�vTCB( ���@�̈ړ� )
 * @file   fieldmap_tcb_move_player.c
 * @author obata
 * @date   2009.08.25
 *
 */
////////////////////////////////////////////////////////////////////////
#include "include/field/fieldmap_proc.h"
#include "include/gamesystem/game_event.h"
#include "fieldmap_tcb.h"
#include "fieldmap.h"
#include "field_player.h"


//==========================================================================================
/**
 * @brief TCB���[�N�G���A
 */
//==========================================================================================
typedef struct
{
  FIELDMAP_WORK* pFieldmap; // ����Ώۂ̃t�B�[���h�}�b�v
  u16            frame;     // ����t���[����
  u16            endFrame;  // �ő�t���[����
  float          moveDist;  // �ړ�����
}
MOVE_WORK;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
void InitWork( MOVE_WORK* work, FIELDMAP_WORK* fieldmap, u16 end_frame, float move_dist );
void DeleteTask( GFL_TCB* tcb );

void TCB_FUNC_DisappearPlayer_LinearUp( GFL_TCB* tcb, void* work );
void TCB_FUNC_AppearPlayer_LinearDown( GFL_TCB* tcb, void* work );
void TCB_FUNC_AppearPlayer_Fall( GFL_TCB* tcb, void* work );

void CalcDrawOffset_LinearUp( VecFx32* p_vec, float frame, float max_frame, float max_y );
void CalcDrawOffset_LinearDown( VecFx32* p_vec, float frame, float max_frame, float max_y );
void CalcDrawOffset_Fall( VecFx32* p_vec, float frame, float max_frame, float max_y );


//========================================================================================== 
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( �㏸�ޏ� )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param dist     �ړ�����
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_DisappearPlayer_LinearUp( FIELDMAP_WORK* fieldmap, int frame, int dist )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  MOVE_WORK*     work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( MOVE_WORK ) );

  // TCB���[�N������
  InitWork( work, fieldmap, frame, dist );

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_DisappearPlayer_LinearUp, work, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( �~���o�� )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param dist     �ړ�����
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_AppearPlayer_LinearDown( FIELDMAP_WORK* fieldmap, int frame, int dist )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  MOVE_WORK*     work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( MOVE_WORK ) );

  // TCB���[�N������
  InitWork( work, fieldmap, frame, dist );

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_AppearPlayer_LinearDown, work, 0 );

  // ��l���̌�����ݒ�
  {
    FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
    MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
    MMDL_SetAcmd( mmdl, AC_DIR_D );
  }
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( �����o�� )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param dist     �ړ�����
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_AppearPlayer_Fall( FIELDMAP_WORK* fieldmap, int frame, int dist )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  MOVE_WORK*     work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( MOVE_WORK ) );

  // TCB���[�N������
  InitWork( work, fieldmap, frame, dist );

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_AppearPlayer_Fall, work, 0 );

  // ��l���̌�����ݒ�
  {
    FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
    MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
    MMDL_SetAcmd( mmdl, AC_DIR_D );
  }
} 


//========================================================================================== 
/**
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
 * @param move_dist �ړ�����
 */
//------------------------------------------------------------------------------------------
void InitWork( MOVE_WORK* work, FIELDMAP_WORK* fieldmap, u16 end_frame, float move_dist )
{
  work->pFieldmap = fieldmap;
  work->frame     = 0;
  work->endFrame  = end_frame;
  work->moveDist  = move_dist;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��j������
 *
 * @param tcb �j������^�X�N
 */
//------------------------------------------------------------------------------------------
void DeleteTask( GFL_TCB* tcb )
{
  // TCB���[�N��j��
  GFL_HEAP_FreeMemory( GFL_TCB_GetWork( tcb ) );

  // �^�X�N��j��
  GFL_TCB_DeleteTask( tcb );
}

//------------------------------------------------------------------------------------------
/**
 * @breif TCB���s�֐�( ���@�̏㏸ )
 */
//------------------------------------------------------------------------------------------
void TCB_FUNC_DisappearPlayer_LinearUp( GFL_TCB* tcb, void* work )
{
  MOVE_WORK*   tcbwork = work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
  VecFx32       offset;

  // ���@�̕`��I�t�Z�b�g���X�V
  CalcDrawOffset_LinearUp( &offset, tcbwork->frame, tcbwork->endFrame, tcbwork->moveDist );
  MMDL_SetVectorDrawOffsetPos( mmdl, &offset );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->endFrame < tcbwork->frame )
  {
    DeleteTask( tcb );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @breif TCB���s�֐�( ���@�̍~�� )
 */
//------------------------------------------------------------------------------------------
void TCB_FUNC_AppearPlayer_LinearDown( GFL_TCB* tcb, void* work )
{
  MOVE_WORK*   tcbwork = work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
  VecFx32       offset;

  // ���@�̕`��I�t�Z�b�g���X�V
  CalcDrawOffset_LinearDown( &offset, tcbwork->frame, tcbwork->endFrame, tcbwork->moveDist );
  MMDL_SetVectorDrawOffsetPos( mmdl, &offset );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->endFrame < tcbwork->frame )
  {
    DeleteTask( tcb );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @breif TCB���s�֐�( �����o�� )
 */
//------------------------------------------------------------------------------------------
void TCB_FUNC_AppearPlayer_Fall( GFL_TCB* tcb, void* work )
{
  MOVE_WORK*   tcbwork = work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
  VecFx32       offset;

  // ���@�̕`��I�t�Z�b�g���X�V
  CalcDrawOffset_Fall( &offset, tcbwork->frame, tcbwork->endFrame, tcbwork->moveDist );
  MMDL_SetVectorDrawOffsetPos( mmdl, &offset );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->endFrame < tcbwork->frame )
  {
    DeleteTask( tcb );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief y���W�̕`��I�t�Z�b�g���Z�o����( �㏸ )
 *
 * @param p_vec     �v�Z���ʂ̊i�[��
 * @param frame     ���݂̃t���[����
 * @param max_frame �ő�t���[����
 * @param max_y     �ő�I�t�Z�b�g
 */
//------------------------------------------------------------------------------------------
void CalcDrawOffset_LinearUp( VecFx32* p_vec, float frame, float max_frame, float max_y )
{
  // ���`�⊮
  float x = frame;
  float a = max_y / max_frame;
  float y = a * x;

  p_vec->x = 0;
  p_vec->y = FX_F32_TO_FX32( y );
  p_vec->z = 0;
}

//------------------------------------------------------------------------------------------
/**
 * @brief y���W�̕`��I�t�Z�b�g���Z�o����( �~�� ) 
 *
 * @param p_vec     �v�Z���ʂ̊i�[��
 * @param frame     ���݂̃t���[����
 * @param max_frame �ő�t���[����
 * @param max_y     �ő�I�t�Z�b�g
 */
//------------------------------------------------------------------------------------------
void CalcDrawOffset_LinearDown( VecFx32* p_vec, float frame, float max_frame, float max_y )
{
  // ���`�⊮
  float x = frame;
  float a = max_y / max_frame;
  float y = max_y - (a * x);

  p_vec->x = 0;
  p_vec->y = FX_F32_TO_FX32( y );
  p_vec->z = 0;
} 

//------------------------------------------------------------------------------------------
/**
 * @brief y���W�̕`��I�t�Z�b�g���Z�o����( ���� ) 
 *
 * @param p_vec     �v�Z���ʂ̊i�[��
 * @param frame     ���݂̃t���[����
 * @param max_frame �ő�t���[����
 * @param max_y     �ő�I�t�Z�b�g
 */
//------------------------------------------------------------------------------------------
void CalcDrawOffset_Fall( VecFx32* p_vec, float frame, float max_frame, float max_y )
{
  // y = ax^2
  float x = frame;
  float a = max_y / (max_frame * max_frame);
  float y = max_y - (a * x * x);

  p_vec->x = 0;
  p_vec->y = FX_F32_TO_FX32( y );
  p_vec->z = 0;
} 
