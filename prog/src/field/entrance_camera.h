/////////////////////////////////////////////////////////////////////
/**
 * @brief  ����o������̃J�������o�f�[�^
 * @file   entrance_camera.h
 * @author obata
 * @date   2010.02.16
 */
/////////////////////////////////////////////////////////////////////
#pragma once 
#include <gflib.h>
#include "field/eventdata_type.h" // for EXIT_TYPE_xxxx
#include "field/fieldmap_proc.h"  // for FIELDMAP_WORK


//===================================================================
// ������i���C�x���g�̃J��������f�[�^
//===================================================================
typedef struct {

  u32 exitType;        // �o������^�C�v
  u32 pitch;           // �s�b�`
  u32 yaw;             // ���[
  u32 length;          // ����
  u32 targetOffsetX;   // �^�[�Q�b�g�I�t�Z�b�gx
  u32 targetOffsetY;   // �^�[�Q�b�g�I�t�Z�b�gy
  u32 targetOffsetZ;   // �^�[�Q�b�g�I�t�Z�b�gz
  u32 frame;           // ����t���[����
  u8  validFlag_IN;    // �i�����ɗL���ȃf�[�^���ǂ���
  u8  validFlag_OUT;   // �ޏo���ɗL���ȃf�[�^���ǂ���

} ENTRANCE_CAMERA;


// ����o������̃J��������f�[�^��ǂݍ���
extern void ENTRANCE_CAMERA_LoadData( 
    ENTRANCE_CAMERA* dest, EXIT_TYPE exitType );

// ����o������ɓ��鎞�̃J��������^�X�N��o�^����
extern void ENTRANCE_CAMERA_AddDoorInTask( 
    FIELDMAP_WORK* fieldmap, const ENTRANCE_CAMERA* data );

// ����o���������o�鎞�̃J��������^�X�N��o�^����
extern void ENTRANCE_CAMERA_AddDoorOutTask( 
    FIELDMAP_WORK* fieldmap, const ENTRANCE_CAMERA* data );

// ����o���������o�鎞�̃J��������^�X�N�̂��߂̏��������s��
extern void ENTRANCE_CAMERA_PrepareForDoorOut( 
    FIELDMAP_WORK* fieldmap, const ENTRANCE_CAMERA* data );
