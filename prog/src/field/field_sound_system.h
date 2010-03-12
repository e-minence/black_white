///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �t�B�[���h�̃T�E���h����
 * @file  field_sound_system.h
 * @author obata
 * @date   2009.12.25
 */
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "gflib.h"
#include "field_sound_proc.h"


//=================================================================================
// ���萔
//=================================================================================
// BGM �ޔ���
typedef enum {
  FSND_PUSHCOUNT_NONE,   // �ޔ��Ȃ�
  FSND_PUSHCOUNT_BASE,   // �t�B�[���h or �C�x���g BGM �ޔ�
  FSND_PUSHCOUNT_OVER,   // �ޔ𐔃I�[�o�[
  FSND_PUSHCOUNT_MAX = FSND_PUSHCOUNT_OVER-1  // �ő�ޔ�
} FSND_PUSHCOUNT;

// ���N�G�X�g
typedef enum{
  FSND_BGM_REQUEST_NONE,        // ���N�G�X�g�Ȃ�
  FSND_BGM_REQUEST_FADE_IN,     // BGM �t�F�[�h�C��
  FSND_BGM_REQUEST_FADE_OUT,    // BGM �t�F�[�h�A�E�g
  FSND_BGM_REQUEST_PUSH,        // BGM �ޔ�
  FSND_BGM_REQUEST_POP,         // BGM ���A
  FSND_BGM_REQUEST_CHANGE,      // BGM �ύX
  FSND_BGM_REQUEST_STAND_BY,    // BGM �Đ�����
  FSND_BGM_REQUEST_FORCE_PLAY,  // �����Đ�
  FSND_BGM_REQUEST_NUM
} FSND_BGM_REQUEST;


//=================================================================================
// ���擾
//================================================================================= 
// BGM�ޔ𐔂̎擾
extern FSND_PUSHCOUNT FIELD_SOUND_GetBGMPushCount( const FIELD_SOUND* fieldSound );

// �t�F�[�h�����ǂ��� (TRUE: �t�F�[�h��)
extern BOOL FIELD_SOUND_IsBGMFade( const FIELD_SOUND* fieldSound );

// �������̃��N�G�X�g�������Ă��邩�ǂ��� (TRUE: �����Ă���)
extern BOOL FIELD_SOUND_HaveRequest( const FIELD_SOUND* fieldSound );


//=================================================================================
// �����N�G�X�g
//
// ����{�I�Ɏg�p���Ȃ��ł��������B
// �@field_sound.h �ɐ錾����Ă���֐����g����BGM�̑�����s���Ă��������B
//=================================================================================
// �o�^
extern void FIELD_SOUND_RegisterRequest( FIELD_SOUND* fieldSound, 
                                         FSND_BGM_REQUEST request,
                                         u32 soundIdx, 
                                         u16 fadeOutFrame, 
                                         u16 fadeInFrame ); 

#define FIELD_SOUND_RegisterRequest_FADE_IN( fieldSound, fadeInFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_FADE_IN, 0, 0, fadeInFrame )

#define FIELD_SOUND_RegisterRequest_FADE_OUT( fieldSound, fadeOutFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_FADE_OUT, 0, fadeOutFrame, 0 )

#define FIELD_SOUND_RegisterRequest_PUSH( fieldSound, fadeOutFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_PUSH, 0, fadeOutFrame, 0 )

#define FIELD_SOUND_RegisterRequest_POP( fieldSound, fadeOutFrame, fadeInFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_POP, 0, fadeOutFrame, fadeInFrame )

#define FIELD_SOUND_RegisterRequest_CHANGE( fieldSound, soundIdx, fadeOutFrame, fadeInFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_CHANGE, soundIdx, fadeOutFrame, fadeInFrame ) 

#define FIELD_SOUND_RegisterRequest_STAND_BY( fieldSound, soundIdx, fadeOutFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_STAND_BY, soundIdx, fadeOutFrame, 0 )

#define FIELD_SOUND_RegisterRequest_FORCE_PLAY( fieldSound, soundIdx ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_FORCE_PLAY, soundIdx, 0, 0 )


//=================================================================================
// ���V�X�e������
//=================================================================================
extern void FIELD_SOUND_Main ( FIELD_SOUND* fieldSound );
extern void FIELD_SOUND_Reset( FIELD_SOUND* fieldSound );


//=================================================================================
// ���v���C���[�{�����[������
//
// ����{�I�Ɏg�p���Ȃ��ł��������B
// �@field_sound.h �ɐ錾����Ă���֐����g���Ă��������B
//=================================================================================
extern void FIELD_SOUND_ChangePlayerVolume( FIELD_SOUND* fieldSound, u8 volume, u8 fadeFrame );

//=================================================================================
// ��TV�g�����V�[�o�[���M������
//=================================================================================
extern void FIELD_SOUND_PlayTVTRingTone( FIELD_SOUND* fieldSound );
extern void FIELD_SOUND_StopTVTRingTone( FIELD_SOUND* fieldSound );


//=================================================================================
// ������
//=================================================================================
// �����̍Đ�
extern void FIELD_SOUND_PlayEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx );
extern void FIELD_SOUND_PlayEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol );
// �����̃{�����[������
extern void FIELD_SOUND_SetEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol );
// �����̒�~
extern void FIELD_SOUND_StopEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx );
// �����̈ꎞ��~
extern void FIELD_SOUND_PauseEnvSE( FIELD_SOUND* fieldSound );
// �����̍Ďn��
extern void FIELD_SOUND_RePlayEnvSE( FIELD_SOUND* fieldSound );
