///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�J���� �A�j���[�V����
 * @file   field_camera_anime.h
 * @author obata
 * @date   2010.04.10
 */
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "field/fieldmap_proc.h"
#include "field_camera.h"


//=============================================================================
// ���J�����p�����[�^
//=============================================================================
typedef struct {

  u16     pitch;        // �s�b�`
  u16     yaw;          // ���[
  fx32    length;       // ����
  VecFx32 targetPos;    // �^�[�Q�b�g���W
  VecFx32 targetOffset; // �^�[�Q�b�g�I�t�Z�b�g

} FCAM_PARAM;

// ���݂̃J�����p�����[�^���擾����
extern void FCAM_PARAM_GetCurrentParam( const FIELD_CAMERA* camera, FCAM_PARAM* dest );


//=============================================================================
// ���A�j���[�V�����f�[�^
//=============================================================================
typedef struct {

  u32 frame; // ����t���[����

  FCAM_PARAM startParam; // �J�n�p�����[�^
  FCAM_PARAM endParam;   // �ŏI�p�����[�^

  BOOL targetBindOffFlag; // �^�[�Q�b�g�̃o�C���h��OFF�ɂ��邩�ǂ���
  BOOL cameraAreaOffFlag; // �J�����G���A��OFF�ɂ��邩�ǂ���

} FCAM_ANIME_DATA;


//=============================================================================
// ���J�������샏�[�N�̕s���S�^
//=============================================================================
typedef struct _FCAM_ANIME_WORK FCAM_ANIME_WORK;

// �J�������샏�[�N�𐶐�����
extern FCAM_ANIME_WORK* FCAM_ANIME_CreateWork( FIELDMAP_WORK* fieldamp );
// �J�������샏�[�N��j������
extern void FCAM_ANIME_DeleteWork( FCAM_ANIME_WORK* work );
// �A�j���[�V�����f�[�^��ݒ肷��
extern void FCAM_ANIME_SetAnimeData( FCAM_ANIME_WORK* work, const FCAM_ANIME_DATA* data );
// �J�������Z�b�g�A�b�v����
extern void FCAM_ANIME_SetupCamera( FCAM_ANIME_WORK* work );
// �J�����̏�Ԃ��J�n�p�����[�^�̐ݒ�ɂ���
extern void FCAM_ANIME_SetCameraStartParam( const FCAM_ANIME_WORK* work );
// �A�j���[�V�������J�n����
extern void FCAM_ANIME_StartAnime( const FCAM_ANIME_WORK* work );
// �A�j���[�V�����̏I�����`�F�b�N����
extern BOOL FCAM_ANIME_IsAnimeFinished( const FCAM_ANIME_WORK* work );
// �J�����𕜋A����
extern void FCAM_ANIME_RecoverCamera( const FCAM_ANIME_WORK* work );
