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


typedef struct _ECAM_WORK ECAM_WORK;


// ���@�̈���ړ��̗L��
typedef enum {
  ECAM_ONESTEP_ON,  // ����ړ�����
  ECAM_ONESTEP_OFF, // ����ړ��Ȃ�
} ECAM_ONESTEP;

// �V�`���G�[�V����
typedef enum {
  ECAM_SITUATION_IN,  // �o������֓���
  ECAM_SITUATION_OUT, // �o���������o��
} ECAM_SITUATION;


// ���o�p�����[�^
typedef struct { 

  EXIT_TYPE      exitType;  // �o������^�C�v
  ECAM_SITUATION situation; // ���� or �o��
  ECAM_ONESTEP   oneStep;   // ���@�̈���ړ������邩�ǂ���

} ECAM_PARAM;


// ���[�N�̐����E�j��
extern ECAM_WORK* ENTRANCE_CAMERA_CreateWork( FIELDMAP_WORK* fieldmap );
extern void ENTRANCE_CAMERA_DeleteWork( ECAM_WORK* work );

// ���o���Z�b�g�A�b�v����
extern void ENTRANCE_CAMERA_Setup( ECAM_WORK* work, const ECAM_PARAM* param );
// ���o���J�n����
extern void ENTRANCE_CAMERA_Start( ECAM_WORK* work );
// ���o�ɂ���đ��삵���J�����𕜋A����
extern void ENTRANCE_CAMERA_Recover( ECAM_WORK* work );

// ���o�̗L�����`�F�b�N����
extern BOOL ENTRANCE_CAMERA_IsAnimeExist( const ECAM_WORK* work );
// ���o���O�t���[�����ǂ������`�F�b�N����
extern BOOL ENTRANCE_CAMERA_IsZeroFrameAnime( const ECAM_WORK* work );
