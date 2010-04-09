////////////////////////////////////////////////////////////////////////////////////
/**
 * @file	event_field_fade.h
 * @brief	�C�x���g�F�t�B�[���h�t�F�[�h����c�[��
 * @author	tamada GAMEFREAK inc.
 * @date	2008.12.10
 *
 * 2009.12.22 tamada event_fieldmap_control���番��
 */
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "map_change_type.h"


//==================================================================================
// ���t�F�[�h�p�����[�^
//==================================================================================

// �t�F�[�h�̎�ގw��
typedef enum {
	FIELD_FADE_BLACK,        // �P�x�t�F�[�h(�u���b�N)
	FIELD_FADE_WHITE,        // �P�x�t�F�[�h(�z���C�g)
  FIELD_FADE_CROSS,        // �N���X�t�F�[�h
  FIELD_FADE_SEASON,       // �G�߃t�F�[�h
  FIELD_FADE_HOLE,         // ���C�v�t�F�[�h ( �z�[�� )
  FIELD_FADE_SHUTTER_DOWN, // ���C�v�t�F�[�h ( �V���b�^�[ �� )
  FIELD_FADE_SHUTTER_UP,   // ���C�v�t�F�[�h ( �V���b�^�[ �� )
  FIELD_FADE_SLIDE_RIGHT,  // ���C�v�t�F�[�h ( �X���C�g �� )
  FIELD_FADE_SLIDE_LEFT,   // ���C�v�t�F�[�h ( �X���C�g �� )
  FIELD_FADE_PLAYER_DIR,   // ���@�̌����Ɉˑ������t�F�[�h
} FIELD_FADE_TYPE;

// �t�F�[�h���������҂��t���O
typedef enum{
  FIELD_FADE_WAIT,    // �t�F�[�h������҂�
  FIELD_FADE_NO_WAIT, // �t�F�[�h������҂��Ȃ�
} FIELD_FADE_WAIT_FLAG;

// BG ���������C�x���g���ōs�����ǂ���
typedef enum{
  FIELD_FADE_BG_INIT,       // ����������
  FIELD_FADE_NOT_BG_INIT,   // ���������Ȃ�
} FIELD_FADE_BG_INIT_FLAG;

//Fade�V�X�e���ɓn���E�F�C�g
enum {
  FIELD_FADE_DEFAULT_WAIT = -1,
  FIELD_FADE_QUICK_WAIT = -16,
  FIELD_FADE_SLOW_WAIT = 4,
};

//==================================================================================
// ���t�F�[�h����֐�
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief �O��̃]�[��ID����, �t�F�[�h�A�E�g�̎�ނ����肷��
 *
 * @param prevZoneID �J�ڌ�̃]�[��ID
 * @param nextZoneID �J�ڑO�̃]�[��ID
 *
 * @return �w�肵���]�[���Ԃ�J�ڂ���ۂ̃t�F�[�h�A�E�g�̎��
 */
//----------------------------------------------------------------------------------
extern FIELD_FADE_TYPE FIELD_FADE_GetFadeOutType( u16 prevZoneID, u16 nextZoneID );
//----------------------------------------------------------------------------------
/**
 * @brief �O��̃]�[��ID����, �t�F�[�h�C���̎�ނ����肷��
 *
 * @param prevZoneID �J�ڌ�̃]�[��ID
 * @param nextZoneID �J�ڑO�̃]�[��ID
 *
 * @return �w�肵���]�[���Ԃ�J�ڂ���ۂ̃t�F�[�h�C���̎��
 */
//----------------------------------------------------------------------------------
extern FIELD_FADE_TYPE FIELD_FADE_GetFadeInType( u16 prevZoneID, u16 nextZoneID );


//==================================================================================
// ����{�t�F�[�h�֐�
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�A�E�g�C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType		       �t�F�[�h�̎�ގw��
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ���(�P�x�t�F�[�h���̂ݗL��)
 *
 * @return ���������C�x���g
 */
//----------------------------------------------------------------------------------
extern GMEVENT* EVENT_FieldFadeOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                                    FIELD_FADE_TYPE fadeType, 
                                    FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag ); 
// �N���X�t�F�[�h
#define EVENT_FieldFadeOut_Cross( gsys, fieldmap ) \
        EVENT_FieldFadeOut( gsys, fieldmap, FIELD_FADE_CROSS, 0 )
// �P�x�t�F�[�h(�u���b�N)
#define EVENT_FieldFadeOut_Black( gsys, fieldmap, wait ) \
        EVENT_FieldFadeOut( gsys, fieldmap, FIELD_FADE_BLACK, wait )
// �P�x�t�F�[�h(�z���C�g)
#define EVENT_FieldFadeOut_White( gsys, fieldmap, wait ) \
        EVENT_FieldFadeOut( gsys, fieldmap, FIELD_FADE_WHITE, wait )
// �G�߃t�F�[�h
#define EVENT_FieldFadeOut_Season( gsys, fieldmap, wait ) \
        EVENT_FieldFadeOut( gsys, fieldmap, FIELD_FADE_BLACK, wait )
// �z�[���A�E�g
#define EVENT_FieldFadeOut_Hole( gsys, fieldmap ) \
        EVENT_FieldFadeOut( gsys, fieldamp, FIELD_FADE_HOLE, 0 )
// �V���b�^�[�A�E�g(��)
#define EVENT_FieldFadeOut_ShutterDown( gsys, fieldmap ) \
        EVENT_FieldFadeOut( gsys, fieldamp, FIELD_FADE_SHUTTER_DOWN, 0 )
// �V���b�^�[�A�E�g(��)
#define EVENT_FieldFadeOut_ShutterUp( gsys, fieldmap ) \
        EVENT_FieldFadeOut( gsys, fieldamp, FIELD_FADE_SHUTTER_UP, 0 )
// �X���C�h�A�E�g(��)
#define EVENT_FieldFadeOut_SlideRight( gsys, fieldmap ) \
        EVENT_FieldFadeOut( gsys, fieldamp, FIELD_FADE_SLIDE_RIGHT, 0 )
// �X���C�h�A�E�g(��)
#define EVENT_FieldFadeOut_SlideLeft( gsys, fieldmap ) \
        EVENT_FieldFadeOut( gsys, fieldamp, FIELD_FADE_SLIDE_LEFT, 0 )
// ���@�̌����Ɉˑ������t�F�[�h
#define EVENT_FieldFadeOut_PlayerDir( gsys, fieldmap ) \
        EVENT_FieldFadeOut( gsys, fieldamp, FIELD_FADE_PLAYER_DIR, 0 )


//----------------------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�C�� �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap 
 * @param	fadeType		       �t�F�[�h�̎�ގw��
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ��� ( �P�x�t�F�[�h���̂ݗL�� )
 * @param BGInitFlag         BG������������ł����Ȃ����ǂ��� ( �P�x�t�F�[�h���̂ݗL�� )
 * @param startSeason        �ŏ��ɕ\������G�� ( �G�߃t�F�[�h���̂ݗL�� )
 * @param endSeason          �Ō�ɕ\������G�� ( �G�߃t�F�[�h���̂ݗL�� )
 *
 * @return ���������C�x���g
 */
//----------------------------------------------------------------------------------
extern GMEVENT* EVENT_FieldFadeIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                                   FIELD_FADE_TYPE fadeType, 
                                   FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag, 
                                   FIELD_FADE_BG_INIT_FLAG BGInitFlag,
                                   u8 startSeason, u8 endSeason ); 
// �N���X�t�F�[�h
#define EVENT_FieldFadeIn_Cross( gsys, fieldmap ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_CROSS, 0, 0, 0, 0 )
// �P�x�t�F�[�h(�u���b�N)
#define EVENT_FieldFadeIn_Black( gsys, fieldmap, wait ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_BLACK, wait, TRUE, 0, 0 )
// �P�x�t�F�[�h(�z���C�g)
#define EVENT_FieldFadeIn_White( gsys, fieldmap, wait ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_WHITE, wait, TRUE, 0, 0 )
// �G�߃t�F�[�h
#define EVENT_FieldFadeIn_Season( gsys, fieldmap, startSeason, endSeason ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_SEASON, 0, 0, startSeason, endSeason )
// ���j���[��ԃt�F�[�h ( Y�{�^�����j���[���o�����܂܂Ȃ� )
#define EVENT_FieldFadeIn_Menu( gsys, fieldmap, wait ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_BLACK, wait, FALSE, 0, 0 )
// �V���b�^�[�C��(��)
#define EVENT_FieldFadeIn_ShutterDown( gsys, fieldmap ) \
        EVENT_FieldFadeIn( gsys, fieldamp, FIELD_FADE_SHUTTER_DOWN, 0 )
// �V���b�^�[�C��(��)
#define EVENT_FieldFadeIn_ShutterUp( gsys, fieldmap ) \
        EVENT_FieldFadeIn( gsys, fieldamp, FIELD_FADE_SHUTTER_UP, 0 )
// �X���C�h�C��(��)
#define EVENT_FieldFadeIn_SlideRight( gsys, fieldmap ) \
        EVENT_FieldFadeIn( gsys, fieldamp, FIELD_FADE_SLIDE_RIGHT, 0 )
// �X���C�h�C��(��)
#define EVENT_FieldFadeIn_SlideLeft( gsys, fieldmap ) \
        EVENT_FieldFadeIn( gsys, fieldamp, FIELD_FADE_SLIDE_LEFT, 0 )
// ���@�̌����Ɉˑ������t�F�[�h
#define EVENT_FieldFadeIn_PlayerDir( gsys, fieldmap ) \
        EVENT_FieldFadeIn( gsys, fieldamp, FIELD_FADE_PLAYER_DIR, 0 )


//==================================================================================
// ����O�I�ȃt�F�[�h�֐�
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief	����� �P�x�t�F�[�h�A�E�g �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType	         �t�F�[�h�̎�ގw�� ( FIELD_FADE_BLACK or FIELD_FADE_WHITE )
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ���
 *
 * @return ���������C�x���g
 */
//----------------------------------------------------------------------------------
extern GMEVENT* EVENT_FlySkyBrightOut( 
    GAMESYS_WORK *gameSystem, FIELDMAP_WORK * fieldmap, 
    FIELD_FADE_TYPE fadeType, FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag ); 
//----------------------------------------------------------------------------------
/**
 * @brief	����� �P�x�t�F�[�h�C�� �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType		       �t�F�[�h�̎�ގw�� ( FADE_TYPE_BLACK or FADE_TYPE_WHITE )
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ���
 * @param fade_speed
 *
 * @return ���������C�x���g
 */
//----------------------------------------------------------------------------------
extern GMEVENT* EVENT_FlySkyBrightIn( 
    GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
    FIELD_FADE_TYPE fadeType, FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag,
    const int fade_speed ); 
//----------------------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����A�E�g �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 *
 * @return ���������C�x���g
 */
//----------------------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_QuickFadeOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); 
//----------------------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����A�E�g
 *
 * @param	gameSystem
 * @param	fieldmap
 *
 * @return ���������C�x���g
 */
//----------------------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_QuickFadeIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );
