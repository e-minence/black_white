/////////////////////////////////////////////////////////////////////
/**
 * @brief  ����o������̃J�������o�f�[�^
 * @file   entrance_camera.c
 * @author obata
 * @date   2010.02.16
 */
/////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "entrance_camera.h"

#include "fieldmap.h"
#include "field_task.h"  
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"

#include "field/eventdata_type.h"  // for EXIT_TYPE_xxxx
#include "arc/arc_def.h"           // for ARCID_xxxx
#include "../../resource/fldmapdata/entrance_camera/entrance_camera.naix"  // for NARC_xxxx

// �f�o�b�O�o�̓X�C�b�`
#define DEBUG_PRINT_ON


//-------------------------------------------------------------------
/**
 * @brief �w�肵���o������^�C�v�ɊY������f�[�^���擾����
 *
 * @param dest     �擾�����f�[�^�̊i�[��
 * @param exitType �f�[�^�擾�Ώۂ̏o������^�C�v
 */
//-------------------------------------------------------------------
void ENTRANCE_CAMERA_LoadData( ENTRANCE_CAMERA* dest, EXIT_TYPE exitType )
{
  u32 datID;

  // EXIT_TYPE_SPxx�ȊO�̓_��
  GF_ASSERT( EXIT_TYPE_SP1 <= exitType );
  GF_ASSERT( exitType < EXIT_TYPE_MAX );

  // �ǂݍ��ރf�[�^������
  datID = exitType - EXIT_TYPE_SP1;

  // �ǂݍ���
  GFL_ARC_LoadData( dest, ARCID_ENTRANCE_CAMERA, datID );

#ifdef DEBUG_PRINT_ON
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: frame   = %d\n", dest->frame );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: pitch   = %x\n", dest->pitch );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: yaw     = %x\n", dest->yaw );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: length  = %x\n", dest->length );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: offsetX = %x\n", dest->targetOffsetX );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: offsetY = %x\n", dest->targetOffsetX );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: offsetZ = %x\n", dest->targetOffsetX );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: validFlag_IN  = %x\n", dest->validFlag_IN );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: validFlag_OUT = %x\n", dest->validFlag_OUT );
#endif
}


//-------------------------------------------------------------------
/**
 * @brief ����o������ɓ��鎞�̃J��������^�X�N��o�^����
 *
 * @param fieldmap
 * @param data     �K�p����J��������f�[�^
 */
//-------------------------------------------------------------------
void ENTRANCE_CAMERA_AddDoorInTask( 
    FIELDMAP_WORK* fieldmap, const ENTRANCE_CAMERA* data )
{
  GF_ASSERT( data->validFlag_IN ); // ���鎞�ɗL���ȃf�[�^�ł͂Ȃ�

  {
    u16 frame;
    u16 pitch, yaw;
    fx32 length;
    VecFx32 targetOffset;

    // �e�p�����[�^�擾
    frame  = data->frame;
    pitch  = data->pitch;
    yaw    = data->yaw;
    length = data->length << FX32_SHIFT;
    VEC_Set( &targetOffset, 
        data->targetOffsetX << FX32_SHIFT,
        data->targetOffsetY << FX32_SHIFT,
        data->targetOffsetZ << FX32_SHIFT );

    // �^�X�N�o�^
    {
      FIELD_TASK_MAN* taskMan;
      FIELD_TASK* zoomTaks;
      FIELD_TASK* pitchTask;
      FIELD_TASK* yawTask;
      FIELD_TASK* targetOffsetTask;

      // �^�X�N�𐶐�
      zoomTaks         = FIELD_TASK_CameraLinearZoom  ( fieldmap, frame, length );
      pitchTask        = FIELD_TASK_CameraRot_Pitch   ( fieldmap, frame, pitch );
      yawTask          = FIELD_TASK_CameraRot_Yaw     ( fieldmap, frame, yaw );
      targetOffsetTask = FIELD_TASK_CameraTargetOffset( fieldmap, frame, &targetOffset );

      // �^�X�N��o�^
      taskMan = FIELDMAP_GetTaskManager( fieldmap );
      FIELD_TASK_MAN_AddTask( taskMan, zoomTaks, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, pitchTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, yawTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, targetOffsetTask, NULL );
    }
  }
}

//-------------------------------------------------------------------
/**
 * @brief ����o���������o�鎞�̃J��������^�X�N��o�^����
 *
 * @param fieldmap
 * @param data     �K�p����J��������f�[�^
 */
//-------------------------------------------------------------------
void ENTRANCE_CAMERA_AddDoorOutTask( 
    FIELDMAP_WORK* fieldmap, const ENTRANCE_CAMERA* data )
{
  FIELD_CAMERA* camera;

  GF_ASSERT( data->validFlag_OUT ); // �o�鎞�ɗL���ȃf�[�^�ł͂Ȃ�

  camera = FIELDMAP_GetFieldCamera( fieldmap );

  {
    u16 frame;
    u16 pitch, yaw;
    fx32 length;
    VecFx32 targetOffset;

    // �e�p�����[�^�擾
    {
      FLD_CAMERA_PARAM defaultParam; 

      // �f�t�H���g�p�����[�^���擾
      FIELD_CAMERA_GetInitialParameter( camera, &defaultParam );

      frame  = data->frame;
      pitch  = defaultParam.Angle.x;
      yaw    = defaultParam.Angle.y;
      length = defaultParam.Distance << FX32_SHIFT;
      VEC_Set( &targetOffset, 
          defaultParam.Shift.x, defaultParam.Shift.y, defaultParam.Shift.z );
    }

    // �^�X�N�o�^
    {
      FIELD_TASK_MAN* taskMan;
      FIELD_TASK* zoomTaks;
      FIELD_TASK* pitchTask;
      FIELD_TASK* yawTask;
      FIELD_TASK* targetOffsetTask;

      // �^�X�N�𐶐�
      zoomTaks         = FIELD_TASK_CameraLinearZoom  ( fieldmap, frame, length );
      pitchTask        = FIELD_TASK_CameraRot_Pitch   ( fieldmap, frame, pitch );
      yawTask          = FIELD_TASK_CameraRot_Yaw     ( fieldmap, frame, yaw );
      targetOffsetTask = FIELD_TASK_CameraTargetOffset( fieldmap, frame, &targetOffset );

      // �^�X�N��o�^
      taskMan = FIELDMAP_GetTaskManager( fieldmap );
      FIELD_TASK_MAN_AddTask( taskMan, zoomTaks, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, pitchTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, yawTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, targetOffsetTask, NULL );
    }
  }
}

//-------------------------------------------------------------------
/**
 * @brief ����o���������o�鎞�̃J��������^�X�N�̂��߂̏��������s��
 *
 * @param fieldmap
 * @param data     �K�p����J��������f�[�^
 */
//-------------------------------------------------------------------
void ENTRANCE_CAMERA_PrepareForDoorOut( 
    FIELDMAP_WORK* fieldmap, const ENTRANCE_CAMERA* data )
{
  FIELD_CAMERA* camera;
  VecFx32 targetOffset;

  GF_ASSERT( data->validFlag_OUT ); // �o�鎞�ɗL���ȃf�[�^�ł͂Ȃ�

  camera = FIELDMAP_GetFieldCamera( fieldmap );

  VEC_Set( &targetOffset, 
      data->targetOffsetX << FX32_SHIFT,
      data->targetOffsetY << FX32_SHIFT,
      data->targetOffsetZ << FX32_SHIFT );

  FIELD_CAMERA_SetAnglePitch( camera, data->pitch );
  FIELD_CAMERA_SetAngleYaw( camera, data->yaw );
  FIELD_CAMERA_SetAngleLen( camera, data->length << FX32_SHIFT );
  FIELD_CAMERA_SetTargetOffset( camera, &targetOffset );
} 
