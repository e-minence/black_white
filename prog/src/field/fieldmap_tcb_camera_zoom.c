////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �t�B�[���h�}�b�vTCB( �J�����Y�[������ )
 * @file   fieldmap_tcb_camera_zoom.c
 * @author obata
 * @date   2009.08.26
 *
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include "include/field/fieldmap_proc.h"
#include "include/gamesystem/game_event.h"
#include "fieldmap_tcb.h"
#include "fieldmap.h"
#include "field_player.h"
#include "field/field_camera.h"


//==========================================================================================
/**
 * @brief TCB���[�N�G���A
 */
//==========================================================================================
typedef struct
{
  FIELDMAP_WORK* pFieldmap; // �����Ώۂ̃t�B�[���h�}�b�v
  u16            frame;     // �o�߃t���[����
  u16            endFrame;  // �I���t���[��
  fx32           startDist; // �J�������������l
  fx32           endDist;   // �J���������ŏI�l
}
ZOOM_WORK;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
static void TCB_FUNC_Zoom( GFL_TCB* tcb, void* work ); 
static void UpdateDist( ZOOM_WORK* p_work );


//========================================================================================== 
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( �Y�[�� )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param dist     �ړ�����
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_CameraZoom( FIELDMAP_WORK* fieldmap, int frame, fx32 dist )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_CAMERA*   cam = FIELDMAP_GetFieldCamera( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  ZOOM_WORK* work     = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( ZOOM_WORK ) ); 

  // TCB���[�N������
  work->frame     = 0;
  work->endFrame  = frame;
  work->startDist = FIELD_CAMERA_GetAngleLen( cam );
  work->endDist   = FIELD_CAMERA_GetAngleLen( cam ) + dist;
  work->pFieldmap = fieldmap;

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_Zoom, work, 0 );
}


//========================================================================================== 
/**
 * @brief ����J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief TCB���s�֐�( �Y�[�� )
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_Zoom( GFL_TCB* tcb, void* work )
{
  ZOOM_WORK* tcbwork = work;

  // �J�����������X�V
  UpdateDist( tcbwork );

  // ���񐔓��삵����I��
  ++(tcbwork->frame);
  if( tcbwork->endFrame < tcbwork->frame )
  {
    GFL_HEAP_FreeMemory( work );  // ���[�N��j��
    GFL_TCB_DeleteTask( tcb );    // �^�X�N��j��(���E)
  }
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J�����������X�V����
 *
 * @param p_work �Y�[��TCB�C�x���g���[�N
 */
//-----------------------------------------------------------------------------------------------
static void UpdateDist( ZOOM_WORK* p_work )
{
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->pFieldmap );

  // ������Ԓ��Ȃ�, �X�V����
  if( p_work->frame <= p_work->endFrame)
  {
    // ���`�⊮
    float    t = p_work->frame / (float)p_work->endFrame;
    float   d1 = FX_FX32_TO_F32( p_work->startDist );
    float   d2 = FX_FX32_TO_F32( p_work->endDist );
    float dist = (1-t) * d1 + ( t * d2);
    FIELD_CAMERA_SetAngleLen( p_camera, FX_F32_TO_FX32( dist ) ); 
  }
}
