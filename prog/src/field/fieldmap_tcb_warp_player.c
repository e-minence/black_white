////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �t�B�[���h�}�b�vTCB( ���@�̃��[�v�ړ� )
 * @file   fieldmap_tcb_wap_player.c
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
  u16   frame;    // ����t���[����
  u16   maxFrame; // �ő�t���[����
  float maxDist;  // �ő�ړ�����
  FIELDMAP_WORK* pFieldmap; // ����Ώۂ̃t�B�[���h�}�b�v
}
TCB_WORK_WARP;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
void DeleteTask( GFL_TCB* tcb );

void TCB_FUNC_WarpPlayer_DisappearUp( GFL_TCB* tcb, void* work );
void TCB_FUNC_WarpPlayer_AppearDown( GFL_TCB* tcb, void* work );
void TCB_FUNC_WarpPlayer_FallIn( GFL_TCB* tcb, void* work );

void CalcDrawOffset_LinearUp( VecFx32* p_vec, float frame, float max_frame, float max_y );
void CalcDrawOffset_LinearDown( VecFx32* p_vec, float frame, float max_frame, float max_y );
void CalcDrawOffset_FallIn( VecFx32* p_vec, float frame, float max_frame, float max_y );


//========================================================================================== 
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================== 

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
 * @brief ���@���[�v�^�X�N��ǉ�����( �㏸�ɂ��ޏ� )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param dist     �ړ�����
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_WARP_PLAYER_AddTask_DisappearUp( FIELDMAP_WORK* fieldmap, int frame, int dist )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TCB_WORK_WARP* work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TCB_WORK_WARP ) );
  GFL_TCB*        tcb;

  // TCB���[�N������
  work->frame     = 0;
  work->maxFrame  = frame;
  work->maxDist   = dist;
  work->pFieldmap = fieldmap;

  // TCB��ǉ�
  tcb = GFL_TCB_AddTask( tcbsys, TCB_FUNC_WarpPlayer_DisappearUp, work, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���@���[�v�^�X�N��ǉ�����( �~���ɂ��o�� )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param dist     �ړ�����
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_WARP_PLAYER_AddTask_AppearDown( FIELDMAP_WORK* fieldmap, int frame, int dist )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TCB_WORK_WARP* work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TCB_WORK_WARP ) );
  GFL_TCB*        tcb;

  // TCB���[�N������
  work->frame     = 0;
  work->maxFrame  = frame;
  work->maxDist   = dist;
  work->pFieldmap = fieldmap;

  // TCB��ǉ�
  tcb = GFL_TCB_AddTask( tcbsys, TCB_FUNC_WarpPlayer_AppearDown, work, 0 );
} 

//------------------------------------------------------------------------------------------
/**
 * @brief ���@���[�v�^�X�N��ǉ�����( �����ɂ��o�� )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param dist     �ړ�����
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_WARP_PLAYER_AddTask_FallIn( FIELDMAP_WORK* fieldmap, int frame, int dist )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TCB_WORK_WARP* work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TCB_WORK_WARP ) );

  // TCB���[�N������
  work->frame     = 0;
  work->maxFrame  = frame;
  work->maxDist   = dist;
  work->pFieldmap = fieldmap;

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_WarpPlayer_FallIn, work, 0 );

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
 * @breif TCB���s�֐�( ���@�̏㏸ )
 */
//------------------------------------------------------------------------------------------
void TCB_FUNC_WarpPlayer_DisappearUp( GFL_TCB* tcb, void* work )
{
  TCB_WORK_WARP* tcbwork = work;
  FIELD_PLAYER*   player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*             mmdl = FIELD_PLAYER_GetMMdl( player ); 
  VecFx32         offset;

  // ���@�̕`��I�t�Z�b�g���X�V
  CalcDrawOffset_LinearUp( &offset, tcbwork->frame, tcbwork->maxFrame, tcbwork->maxDist );
  MMDL_SetVectorDrawOffsetPos( mmdl, &offset );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->maxFrame < tcbwork->frame )
  {
    DeleteTask( tcb );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @breif TCB���s�֐�( ���@�̍~�� )
 */
//------------------------------------------------------------------------------------------
void TCB_FUNC_WarpPlayer_AppearDown( GFL_TCB* tcb, void* work )
{
  TCB_WORK_WARP* tcbwork = work;
  FIELD_PLAYER*   player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*             mmdl = FIELD_PLAYER_GetMMdl( player ); 
  VecFx32         offset;

  // ���@�̕`��I�t�Z�b�g���X�V
  CalcDrawOffset_LinearDown( &offset, tcbwork->frame, tcbwork->maxFrame, tcbwork->maxDist );
  MMDL_SetVectorDrawOffsetPos( mmdl, &offset );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->maxFrame < tcbwork->frame )
  {
    DeleteTask( tcb );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @breif TCB���s�֐�( �����o�� )
 */
//------------------------------------------------------------------------------------------
void TCB_FUNC_WarpPlayer_FallIn( GFL_TCB* tcb, void* work )
{
  TCB_WORK_WARP* tcbwork = work;
  FIELD_PLAYER*   player = FIELDMAP_GetFieldPlayer( tcbwork->pFieldmap );
  MMDL*             mmdl = FIELD_PLAYER_GetMMdl( player ); 
  VecFx32         offset;

  // ���@�̕`��I�t�Z�b�g���X�V
  CalcDrawOffset_FallIn( &offset, tcbwork->frame, tcbwork->maxFrame, tcbwork->maxDist );
  MMDL_SetVectorDrawOffsetPos( mmdl, &offset );

  // �w��t���[�����o�߂�����, �^�X�N�I��
  tcbwork->frame++;
  if( tcbwork->maxFrame < tcbwork->frame )
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
void CalcDrawOffset_FallIn( VecFx32* p_vec, float frame, float max_frame, float max_y )
{
  float x = frame;
  float a = max_y / (max_frame * max_frame);
  float y = max_y - (a * x * x);

  p_vec->x = 0;
  p_vec->y = FX_F32_TO_FX32( y );
  p_vec->z = 0;
} 
