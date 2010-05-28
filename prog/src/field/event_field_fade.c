//============================================================================================
/**
 * @file	event_field_fade.c
 * @brief	�C�x���g�F�t�B�[���h�t�F�[�h����c�[��
 * @author	tamada GAMEFREAK inc.
 * @date	2008.12.10
 *
 * 2009.12.22 tamada event_fieldmap_control���番��
 */
//============================================================================================

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"
#include "gamesystem/pm_season.h"  // for PMSEASON_xxxx 
#include "field/zonedata.h"        // for ZONEDATA_xxxx
#include "field/fieldmap.h" 
#include "system/screentex.h"
#include "system/wipe.h"

#include "vblank_bg_scale_expand.h"
#include "event_field_fade.h"
#include "event_season_display.h"  // for EVENT_SeasonDisplay
#include "map_change_type.h"  // for MC_TYPE_xxxx

#ifdef PM_DEBUG
BOOL DebugBGInitEnd = FALSE;    //BG�������Ď��t���O
#endif 

#define BG_FRAME_CROSS_FADE ( FLDBG_MFRM_EFF1 ) 



//============================================================================================
// ���֐��C���f�b�N�X
//============================================================================================
static int GetBrightFadeMode( FIELD_FADE_TYPE fadeType );
static FIELD_FADE_TYPE GetFadeOutType_byMapChangeType( MC_TYPE prevMapChangeType, MC_TYPE nextMapChangeType );
static FIELD_FADE_TYPE GetFadeInType_byMapChangeType( MC_TYPE prevMapChangeType, MC_TYPE nextMapChangeType ); 
static void ResetFieldBG( FIELDMAP_WORK* fieldmap );
static void InitFieldBG( FIELDMAP_WORK* fieldmap );
//--------------------------------------------------------------------------------------------
// ���C�x���g�����֐�
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_BrightOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,                   // �P�x�t�F�[�h�A�E�g 
                                 FIELD_FADE_TYPE fadeType, FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag ); //
static GMEVENT* EVENT_BrightIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, // �P�x�t�F�[�h�C��
                                 FIELD_FADE_TYPE fadeType,                         // 
                                 FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag,          // 
                                 FIELD_FADE_BG_INIT_FLAG BGInitFlag );             //
static GMEVENT* EVENT_CrossOut( GAMESYS_WORK *gameSystem, FIELDMAP_WORK * fieldmap ); // �N���X�t�F�[�h�A�E�g
static GMEVENT* EVENT_CrossIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �N���X�t�F�[�h�C��
static GMEVENT* EVENT_SeasonIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, u8 startSeason, u8 endSeason ); // �G�߃t�F�[�h�C��
static GMEVENT* EVENT_HoleOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �z�[���A�E�g
static GMEVENT* EVENT_ShutterDownOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �V���b�^�[�A�E�g(��)
static GMEVENT* EVENT_ShutterDownIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �V���b�^�[�C��(��)
static GMEVENT* EVENT_ShutterUpOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �V���b�^�[�A�E�g(��)
static GMEVENT* EVENT_ShutterUpIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �V���b�^�[�C��(��)
static GMEVENT* EVENT_SlideRightOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �X���C�h�A�E�g(��)
static GMEVENT* EVENT_SlideRightIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �X���C�h�C��(��)
static GMEVENT* EVENT_SlideLeftOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �X���C�h�A�E�g(��)
static GMEVENT* EVENT_SlideLeftIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // �X���C�h�C��(��) 
static GMEVENT* EVENT_PlayerDirOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // ���@�̌����Ɉˑ������t�F�[�h�A�E�g
static GMEVENT* EVENT_PlayerDirIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap ); // ���@�̌����Ɉˑ������t�F�[�h�C��
//--------------------------------------------------------------------------------------------
// ���C�x���g�����֐�
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT BrightOutEvent( GMEVENT* event, int* seq, void* wk ); // �P�x�t�F�[�h�A�E�g
static GMEVENT_RESULT BrightInEvent( GMEVENT* event, int* seq, void* wk ); // �P�x�t�F�[�h�C��
static GMEVENT_RESULT CrossOutEvent( GMEVENT* event, int* seq, void* wk ); // �N���X�t�F�[�h�A�E�g
static GMEVENT_RESULT CrossInEvent( GMEVENT* event, int* seq, void* wk ); // �N���X�t�F�[�h�C��
static GMEVENT_RESULT SeasonInEvent( GMEVENT* event, int* seq, void* wk ); // �G�߃t�F�[�h�C��
static GMEVENT_RESULT SeasonInEvent_Callback( GMEVENT* event, int* seq, void* wk ); // �G�߃t�F�[�h�C�� ( �R�[���o�b�N�Ăяo��+ )
static GMEVENT_RESULT HoleOutEvent( GMEVENT* event, int* seq, void* wk ); // �z�[���A�E�g
static GMEVENT_RESULT ShutterDownOutEvent( GMEVENT* event, int* seq, void* wk ); // �V���b�^�[�A�E�g(��)
static GMEVENT_RESULT ShutterDownInEvent( GMEVENT* event, int* seq, void* wk ); // �V���b�^�[�C��(��)
static GMEVENT_RESULT ShutterUpOutEvent( GMEVENT* event, int* seq, void* wk ); // �V���b�^�[�A�E�g(��)
static GMEVENT_RESULT ShutterUpInEvent( GMEVENT* event, int* seq, void* wk ); // �V���b�^�[�C��(��)
static GMEVENT_RESULT SlideRightOutEvent( GMEVENT* event, int* seq, void* wk ); // �X���C�h�A�E�g(��)
static GMEVENT_RESULT SlideRightInEvent( GMEVENT* event, int* seq, void* wk ); // �X���C�h�C��(��)
static GMEVENT_RESULT SlideLeftOutEvent( GMEVENT* event, int* seq, void* wk ); // �X���C�h�A�E�g(��)
static GMEVENT_RESULT SlideLeftInEvent( GMEVENT* event, int* seq, void* wk ); // �X���C�h�C��(��)
static GMEVENT_RESULT SlideLeftInEvent( GMEVENT* event, int* seq, void* wk ); // �X���C�h�C��(��)
static GMEVENT_RESULT QuickOutEvent( GMEVENT* event, int* seq, void* wk ); // �����t�F�[�h�A�E�g
static GMEVENT_RESULT QuickInEvent( GMEVENT* event, int* seq, void* wk );  // �����t�F�[�h�C��
static GMEVENT_RESULT FlySkyBrightInEvent( GMEVENT* event, int* seq, void* wk ); // ����ԗp �P�x�t�F�[�h�C��


//============================================================================================
// ���t�F�[�h�C�x���g���[�N
//============================================================================================ 
typedef struct 
{
  GAMESYS_WORK*  gameSystem;
  FIELDMAP_WORK* fieldmap;

	FIELD_FADE_TYPE fadeType;  // �t�F�[�h�̎�� ( �N���X�t�F�[�h, �P�x�t�F�[�h�Ȃ� )

  // �P�x�t�F�[�h
  int                     brightFadeMode;      // �t�F�[�h���[�h
  int                     FadeSpeed;
  FIELD_FADE_WAIT_FLAG    fadeFinishWaitFlag;  // �t�F�[�h�����̊�����҂��ǂ���
  FIELD_FADE_BG_INIT_FLAG BGInitFlag;          // BG�ʂ̏��������s�����ǂ���

  // �N���X�t�F�[�h
	int alphaWork;

  // �G�߃t�F�[�h
  u8 startSeason;  // �ŏ��ɕ\������G��
  u8 endSeason;    // �Ō�ɕ\������G��
  SEASON_DISP_CALLBACK_FUNC callback_func; // �R�[���o�b�N�֐�
  void* callback_param; // �R�[���o�b�N�֐��ɓn�����[�N

} FADE_EVENT_WORK;

//============================================================================================
// ���t�F�[�h����֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief �O��̃]�[��ID����, �t�F�[�h�A�E�g�̎�ނ����肷��
 *
 * @param prevZoneID �J�ڌ�̃]�[��ID
 * @param nextZoneID �J�ڑO�̃]�[��ID
 *
 * @return �w�肵���]�[���Ԃ�J�ڂ���ۂ̃t�F�[�h�A�E�g�̎��
 */
//--------------------------------------------------------------------------------------------
extern FIELD_FADE_TYPE FIELD_FADE_GetFadeOutType( u16 prevZoneID, u16 nextZoneID )
{
  MC_TYPE prevMapChangeType;
  MC_TYPE nextMapChangeType;

  // �O��̃]�[���̃}�b�v�؂�ւ��^�C�v���擾
  prevMapChangeType = ZONEDATA_GetMapChangeType( prevZoneID );
  nextMapChangeType = ZONEDATA_GetMapChangeType( nextZoneID );

  return GetFadeOutType_byMapChangeType( prevMapChangeType, nextMapChangeType );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �O��̃]�[��ID����, �t�F�[�h�C���̎�ނ����肷��
 *
 * @param prevZoneID �J�ڌ�̃]�[��ID
 * @param nextZoneID �J�ڑO�̃]�[��ID
 *
 * @return �w�肵���]�[���Ԃ�J�ڂ���ۂ̃t�F�[�h�C���̎��
 */
//--------------------------------------------------------------------------------------------
extern FIELD_FADE_TYPE FIELD_FADE_GetFadeInType( u16 prevZoneID, u16 nextZoneID )
{
  MC_TYPE prevMapChangeType;
  MC_TYPE nextMapChangeType;

  // �O��̃]�[���̃}�b�v�؂�ւ��^�C�v���擾
  prevMapChangeType = ZONEDATA_GetMapChangeType( prevZoneID );
  nextMapChangeType = ZONEDATA_GetMapChangeType( nextZoneID );

  return GetFadeInType_byMapChangeType( prevMapChangeType, nextMapChangeType );
}


//============================================================================================
// ����{�t�F�[�h�֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldFadeOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                             FIELD_FADE_TYPE fadeType, 
                             FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag ) 
{ 
  GMEVENT* event;

  // �t�F�[�h�C�x���g����
  switch( fadeType ) {
  default:
  case FIELD_FADE_CROSS: // �N���X�t�F�[�h
    event = EVENT_CrossOut( gameSystem, fieldmap );
    break;
  case FIELD_FADE_BLACK:  // �P�x�t�F�[�h(�u���b�N)
  case FIELD_FADE_WHITE:  // �P�x�t�F�[�h(�z���C�g)
  case FIELD_FADE_SEASON: // �G�߃t�F�[�h
    event = EVENT_BrightOut( gameSystem, fieldmap, fadeType, fadeFinishWaitFlag );
    break;
  case FIELD_FADE_HOLE: // �z�[���A�E�g
    event = EVENT_HoleOut( gameSystem, fieldmap );
    break;
  case FIELD_FADE_SHUTTER_DOWN: // �V���b�^�[�A�E�g(��)
    event = EVENT_ShutterDownOut( gameSystem, fieldmap );
    break;
  case FIELD_FADE_SHUTTER_UP: // �V���b�^�[�A�E�g(��)
    event = EVENT_ShutterUpOut( gameSystem, fieldmap );
    break;
  case FIELD_FADE_SLIDE_RIGHT: // �X���C�h�A�E�g(��)
    event = EVENT_SlideRightOut( gameSystem, fieldmap );
    break;
  case FIELD_FADE_SLIDE_LEFT: // �X���C�h�A�E�g(��)
    event = EVENT_SlideLeftOut( gameSystem, fieldmap );
    break;
  case FIELD_FADE_PLAYER_DIR: // ���@�̌����Ɉˑ������t�F�[�h
    event = EVENT_PlayerDirOut( gameSystem, fieldmap );
    break;
  }
	return event;
}

//--------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldFadeIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                            FIELD_FADE_TYPE fadeType, 
                            FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag, 
                            FIELD_FADE_BG_INIT_FLAG BGInitFlag,
                            u8 startSeason, u8 endSeason )
{
  GMEVENT* event;

  switch( fadeType ) {
  default:
  case FIELD_FADE_CROSS: // �N���X�t�F�[�h
    event = EVENT_CrossIn( gameSystem, fieldmap );
    break;
  case FIELD_FADE_BLACK: // �P�x�t�F�[�h(�u���b�N)
  case FIELD_FADE_WHITE: // �P�x�t�F�[�h(�z���C�g)
    event = EVENT_BrightIn( gameSystem, fieldmap, fadeType, fadeFinishWaitFlag, BGInitFlag );
    break;
  case FIELD_FADE_SEASON: // �G�߃t�F�[�h
    event = EVENT_SeasonIn( gameSystem, fieldmap, startSeason, endSeason );
    break;
  case FIELD_FADE_SHUTTER_DOWN: // �V���b�^�[�C��(��)
    event = EVENT_ShutterDownIn( gameSystem, fieldmap );
    break;
  case FIELD_FADE_SHUTTER_UP: // �V���b�^�[�C��(��)
    event = EVENT_ShutterUpIn( gameSystem, fieldmap );
    break;
  case FIELD_FADE_SLIDE_RIGHT: // �X���C�h�C��(��)
    event = EVENT_SlideRightIn( gameSystem, fieldmap );
    break;
  case FIELD_FADE_SLIDE_LEFT: // �X���C�h�C��(��)
    event = EVENT_SlideLeftIn( gameSystem, fieldmap );
    break;
  case FIELD_FADE_PLAYER_DIR: // ���@�̌����Ɉˑ������t�F�[�h
    event = EVENT_PlayerDirIn( gameSystem, fieldmap );
    break;
  }
	return event; 
}

//----------------------------------------------------------------------------------
/**
 * @brief	�R�[���o�b�N�t�� �G�߃t�F�[�h�C�� �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap 
 * @param startSeason     �ŏ��ɕ\������G�� ( �G�߃t�F�[�h���̂ݗL�� )
 * @param endSeason       �Ō�ɕ\������G�� ( �G�߃t�F�[�h���̂ݗL�� )
 * @param callback_func   �R�[���o�b�N�֐�
 * @param callback_param  �R�[���o�b�N�֐��ɓn�����[�N
 *
 * @return ���������C�x���g
 */
//----------------------------------------------------------------------------------
GMEVENT* EVENT_SeasonIn_Callback( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                                  u8 startSeason, u8 endSeason,
                                  SEASON_DISP_CALLBACK_FUNC callback_func, void* callback_param )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, SeasonInEvent_Callback, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem     = gameSystem;
  work->fieldmap       = fieldmap;
	work->fadeType       = FIELD_FADE_SEASON;
  work->startSeason    = startSeason;
  work->endSeason      = endSeason;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );
  work->callback_func  = callback_func;
  work->callback_param = callback_param;

	return event;
}

//--------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FlySkyBrightIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                               FIELD_FADE_TYPE fadeType, FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag,
                               const int fade_speed )
{
	GMEVENT * event;
	FADE_EVENT_WORK * work;

	event = GMEVENT_Create( gameSystem, NULL, FlySkyBrightInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem         = gameSystem;
  work->fieldmap           = fieldmap;
	work->fadeType           = fadeType;
  work->fadeFinishWaitFlag = fadeFinishWaitFlag;
  work->brightFadeMode     = GetBrightFadeMode( fadeType );
  work->FadeSpeed          = fade_speed;

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����A�E�g �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_QuickFadeOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, QuickOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;

  return event;
}

//--------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FlySkyBrightOut( GAMESYS_WORK *gameSystem, FIELDMAP_WORK * fieldmap, 
                                FIELD_FADE_TYPE fadeType, FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, BrightOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem         = gameSystem;
  work->fieldmap           = fieldmap;
	work->fadeType           = fadeType;
  work->fadeFinishWaitFlag = fadeFinishWaitFlag;
  work->brightFadeMode     = GetBrightFadeMode( fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����A�E�g
 *
 * @param	gameSystem
 * @param	fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_QuickFadeIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
  FADE_EVENT_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, QuickInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;

  return event;
} 


//===========================================================================================
//=========================================================================================== 

//-------------------------------------------------------------------------------------------
/**
 * @brief �P�x�t�F�[�h���[�h���擾����
 *
 * @param fadeType �t�F�[�h�^�C�v ( �N���X�t�F�[�h, �P�x�t�F�[�h �Ȃ� )
 *
 * @return �w��t�F�[�h�^�C�v�ɉ������P�x�t�F�[�h���[�h
 */
//-------------------------------------------------------------------------------------------
static int GetBrightFadeMode( FIELD_FADE_TYPE fadeType )
{
  int brightFadeMode;

  switch( fadeType ) {
  default:
  case FIELD_FADE_BLACK: // �P�x�t�F�[�h(�u���b�N)
    brightFadeMode = GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB;
    break;
  case FIELD_FADE_WHITE: // �P�x�t�F�[�h(�z���C�g)
    brightFadeMode = GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN | GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB;
    break;
  case FIELD_FADE_CROSS: // �N���X�t�F�[�h
  case FIELD_FADE_SEASON: // �G�߃t�F�[�h
  case FIELD_FADE_HOLE: // ���C�v�t�F�[�h ( �z�[�� )
  case FIELD_FADE_SHUTTER_DOWN: // ���C�v�t�F�[�h ( �V���b�^�[ �� )
  case FIELD_FADE_SHUTTER_UP: // ���C�v�t�F�[�h ( �V���b�^�[ �� )
  case FIELD_FADE_SLIDE_RIGHT: // ���C�v�t�F�[�h ( �X���C�g �� )
  case FIELD_FADE_SLIDE_LEFT: // ���C�v�t�F�[�h ( �X���C�g �� )
  case FIELD_FADE_PLAYER_DIR: // ���@�̌����Ɉˑ������t�F�[�h
    brightFadeMode = GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB;
    break;
  }

  return brightFadeMode;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �}�b�v�؂�ւ��^�C�v����, �t�F�[�h�A�E�g�̎�ނ����肷��
 *
 * @param prevMapChangeType �J�ڑO�]�[���̃}�b�v�؂�ւ��^�C�v
 * @param nextMapChangeType �J�ڌ�]�[���̃}�b�v�؂�ւ��^�C�v
 *
 * @return �w�肵���}�b�v�؂�ւ��^�C�v�Ԃ�J�ڂ���ۂ̃t�F�[�h�A�E�g�̎��
 */
//--------------------------------------------------------------------------------------------
static FIELD_FADE_TYPE GetFadeOutType_byMapChangeType( MC_TYPE prevMapChangeType, MC_TYPE nextMapChangeType )
{
  // �t�F�[�h�^�C�v [ �OMC_TYPE ][ ��MC_TYPE ]
  const FIELD_FADE_TYPE fadeType[ MC_TYPE_NUM ][MC_TYPE_NUM ] =
  {
                /* FIELD                  ROOM                   GATE                   DUNGEON                BRIDGE                 OTHER            */
    /* FIELD   */{ FIELD_FADE_PLAYER_DIR, FIELD_FADE_CROSS,      FIELD_FADE_PLAYER_DIR, FIELD_FADE_HOLE,       FIELD_FADE_PLAYER_DIR, FIELD_FADE_BLACK },
    /* ROOM    */{ FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_HOLE,       FIELD_FADE_BLACK,      FIELD_FADE_BLACK }, 
    /* GATE    */{ FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_CROSS,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK },
    /* DUNGEON */{ FIELD_FADE_WHITE,      FIELD_FADE_WHITE,      FIELD_FADE_WHITE,      FIELD_FADE_BLACK,      FIELD_FADE_WHITE,      FIELD_FADE_BLACK },
    /* BRIDGE  */{ FIELD_FADE_PLAYER_DIR, FIELD_FADE_BLACK,      FIELD_FADE_PLAYER_DIR, FIELD_FADE_BLACK,      FIELD_FADE_CROSS,      FIELD_FADE_BLACK },
    /* OTHER   */{ FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK },
  };

  return fadeType[ prevMapChangeType ][ nextMapChangeType ];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �}�b�v�؂�ւ��^�C�v����, �t�F�[�h�C���̎�ނ����肷��
 *
 * @param prevMapChangeType �J�ڑO�]�[���̃}�b�v�؂�ւ��^�C�v
 * @param nextMapChangeType �J�ڌ�]�[���̃}�b�v�؂�ւ��^�C�v
 *
 * @return �w�肵���}�b�v�؂�ւ��^�C�v�Ԃ�J�ڂ���ۂ̃t�F�[�h�C���̎��
 */
//--------------------------------------------------------------------------------------------
static FIELD_FADE_TYPE GetFadeInType_byMapChangeType( MC_TYPE prevMapChangeType, MC_TYPE nextMapChangeType )
{
  // �t�F�[�h�^�C�v [ �OMC_TYPE ][ ��MC_TYPE ]
  const FIELD_FADE_TYPE fadeType[ MC_TYPE_NUM ][ MC_TYPE_NUM ] =
  {
                /* FIELD                  ROOM                   GATE                   DUNGEON                BRIDGE                 OTHER            */
    /* FIELD   */{ FIELD_FADE_PLAYER_DIR, FIELD_FADE_CROSS,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_PLAYER_DIR, FIELD_FADE_BLACK },
    /* ROOM    */{ FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK },
    /* GATE    */{ FIELD_FADE_PLAYER_DIR, FIELD_FADE_BLACK,      FIELD_FADE_CROSS,      FIELD_FADE_BLACK,      FIELD_FADE_PLAYER_DIR, FIELD_FADE_BLACK },
    /* DUNGEON */{ FIELD_FADE_WHITE,      FIELD_FADE_WHITE,      FIELD_FADE_WHITE,      FIELD_FADE_BLACK,      FIELD_FADE_WHITE,      FIELD_FADE_BLACK },
    /* BRIDGE  */{ FIELD_FADE_PLAYER_DIR, FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_CROSS,      FIELD_FADE_BLACK },
    /* OTHER   */{ FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK,      FIELD_FADE_BLACK },
  };

  return fadeType[ prevMapChangeType ][ nextMapChangeType ];
}


//============================================================================================
// ���C�x���g�����֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief	�P�x�t�F�[�h�A�E�g �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType		        �t�F�[�h�̎�ގw�� ( FIELD_FADE_BLACK or FIELD_FADE_WHITE )
 * @param fadeFinishWaitFlag  �t�F�[�h������҂��ǂ���
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_BrightOut( GAMESYS_WORK* gameSystem, 
                                      FIELDMAP_WORK* fieldmap, 
                                      FIELD_FADE_TYPE fadeType, 
                                      FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, BrightOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem         = gameSystem;
  work->fieldmap           = fieldmap;
  work->fadeType           = fadeType;
  work->fadeFinishWaitFlag = fadeFinishWaitFlag;
  work->brightFadeMode     = GetBrightFadeMode( fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�P�x�t�F�[�h�C�� �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType		       �t�F�[�h�̎�ގw�� ( FADE_TYPE_BLACK or FADE_TYPE_WHITE )
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ���
 * @param BGInitFlag         BG������������ł����Ȃ����ǂ���
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_BrightIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                                     FIELD_FADE_TYPE fadeType, 
                                     FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag,
                                     FIELD_FADE_BG_INIT_FLAG BGInitFlag )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, BrightInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem          = gameSystem;
  work->fieldmap            = fieldmap;
	work->fadeType            = fadeType;
  work->fadeFinishWaitFlag  = fadeFinishWaitFlag;
  work->brightFadeMode      = GetBrightFadeMode( fadeType );
  work->BGInitFlag          = BGInitFlag;

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�N���X�t�F�[�h�A�E�g �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_CrossOut( GAMESYS_WORK *gameSystem, FIELDMAP_WORK * fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, CrossOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
  work->fadeType   = FIELD_FADE_CROSS;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�N���X�t�F�[�h�C�� �C�x���g����
 * @param	gameSystem
 * @param	fieldmap 
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_CrossIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, CrossInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
  work->fadeType   = FIELD_FADE_CROSS;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�G�߃t�F�[�h�C�� �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param startSeason �\������ŏ��̋G��
 * @param endSeason   �\������Ō�̋G��
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_SeasonIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                                     u8 startSeason, u8 endSeason )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, SeasonInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem     = gameSystem;
  work->fieldmap       = fieldmap;
	work->fadeType       = FIELD_FADE_SEASON;
  work->startSeason    = startSeason;
  work->endSeason      = endSeason;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );
  work->callback_func  = NULL;
  work->callback_param = NULL;

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �z�[���A�E�g �C�x���g����
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_HoleOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, HoleOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
	work->fadeType   = FIELD_FADE_HOLE;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �V���b�^�[�A�E�g(��)
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_ShutterDownOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  // �C�x���g����
	event = GMEVENT_Create( gameSystem, NULL, ShutterDownOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
	work->fadeType   = FIELD_FADE_SHUTTER_DOWN;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �V���b�^�[�C��(��)
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_ShutterDownIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  // �C�x���g����
	event = GMEVENT_Create( gameSystem, NULL, ShutterDownInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
	work->fadeType   = FIELD_FADE_SHUTTER_DOWN;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �V���b�^�[�A�E�g(��)
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_ShutterUpOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  // �C�x���g����
	event = GMEVENT_Create( gameSystem, NULL, ShutterUpOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
	work->fadeType   = FIELD_FADE_SHUTTER_UP;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �V���b�^�[�C��(��)
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_ShutterUpIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  // �C�x���g����
	event = GMEVENT_Create( gameSystem, NULL, ShutterUpInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
	work->fadeType   = FIELD_FADE_SHUTTER_UP;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �X���C�h�A�E�g(��)
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_SlideRightOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  // �C�x���g����
	event = GMEVENT_Create( gameSystem, NULL, SlideRightOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
	work->fadeType   = FIELD_FADE_SLIDE_RIGHT;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �X���C�h�C��(��)
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_SlideRightIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  // �C�x���g����
	event = GMEVENT_Create( gameSystem, NULL, SlideRightInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
	work->fadeType   = FIELD_FADE_SLIDE_RIGHT;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �X���C�h�A�E�g(��)
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_SlideLeftOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  // �C�x���g����
	event = GMEVENT_Create( gameSystem, NULL, SlideLeftOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
	work->fadeType   = FIELD_FADE_SLIDE_LEFT;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �X���C�h�C��(��)
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_SlideLeftIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  // �C�x���g����
	event = GMEVENT_Create( gameSystem, NULL, SlideLeftInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
	work->fadeType   = FIELD_FADE_SLIDE_LEFT;
  work->brightFadeMode = GetBrightFadeMode( work->fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���@�Ɉˑ������t�F�[�h�A�E�g�C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���@�̌����ɉ������t�F�[�h�A�E�g�C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_PlayerDirOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  int dir;

  // ���@�̌������擾
  {
    FIELD_PLAYER* player;
    player = FIELDMAP_GetFieldPlayer( fieldmap );
    dir = FIELD_PLAYER_GetDir( player );
  }

  // ���@�̌����ɉ������C�x���g��I��
  switch( dir ) {
  default: GF_ASSERT(0);
  case DIR_LEFT:  return EVENT_SlideRightOut( gameSystem, fieldmap );
  case DIR_RIGHT: return EVENT_SlideLeftOut( gameSystem, fieldmap );
  case DIR_UP:    return EVENT_ShutterDownOut( gameSystem, fieldmap );
  case DIR_DOWN:  return EVENT_ShutterUpOut( gameSystem, fieldmap );
  }

  GF_ASSERT(0);
	return NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���@�Ɉˑ������t�F�[�h�A�E�g�C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���@�̌����ɉ������t�F�[�h�A�E�g�C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_PlayerDirIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  int dir;

  // ���@�̌������擾
  {
    FIELD_PLAYER* player;
    player = FIELDMAP_GetFieldPlayer( fieldmap );
    dir = FIELD_PLAYER_GetDir( player );
  }

  // ���@�̌����ɉ������C�x���g��I��
  switch( dir ) {
  default: GF_ASSERT(0);
  case DIR_LEFT:  return EVENT_SlideRightIn( gameSystem, fieldmap );
  case DIR_RIGHT: return EVENT_SlideLeftIn( gameSystem, fieldmap );
  case DIR_UP:    return EVENT_ShutterDownIn( gameSystem, fieldmap );
  case DIR_DOWN:  return EVENT_ShutterUpIn( gameSystem, fieldmap );
  }

  GF_ASSERT(0);
	return NULL;
}



//============================================================================================
// ���C�x���g�����֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief �P�x�t�F�[�h�A�E�g �C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT BrightOutEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq ) {
	case 0:
    // �t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 0, 16, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

	case 1:
    if( work->fadeFinishWaitFlag == FIELD_FADE_NO_WAIT ){ return GMEVENT_RES_FINISH; } // �t�F�[�h�҂��Ȃ�
		if( GFL_FADE_CheckFade() == FALSE ){ return GMEVENT_RES_FINISH; }  // �t�F�[�h����
		break;
	}
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �P�x�t�F�[�h�C�� �C�x���g
 */ 
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT BrightInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq ) {
  case 0:
    // �t�B�[���hBG����
    if ( work->BGInitFlag ) {
      ResetFieldBG( work->fieldmap );
      InitFieldBG( work->fieldmap );
    }
    // �t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 16, 0, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

	case 1:
    if( work->fadeFinishWaitFlag == FIELD_FADE_NO_WAIT ){ return GMEVENT_RES_FINISH; }  // �t�F�[�h�҂��Ȃ�
    if( GFL_FADE_CheckFade() == FALSE ){ return GMEVENT_RES_FINISH; }  // �t�F�[�h����
		break;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �N���X�t�F�[�h�A�E�g �C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT CrossOutEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq ) {
	case 0:
    // �t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 0, 16, FIELD_FADE_DEFAULT_WAIT );

		// ��ʃL���v�`����VRAM_D
		GX_SetBankForLCDC(GX_VRAM_LCDC_D);
		GX_SetCapture(GX_CAPTURE_SIZE_256x192,				// Capture size
				          GX_CAPTURE_MODE_A,							// Capture mode
				          GX_CAPTURE_SRCA_2D3D,						// Blend src A
				          GX_CAPTURE_SRCB_VRAM_0x00000,		// Blend src B
									GX_CAPTURE_DEST_VRAM_D_0x00000,	// dst
								  16, 0);             // Blend parameter for src A, B

		OS_WaitVBlankIntr();	// 0���C���҂��E�G�C�g
		OS_WaitVBlankIntr();	// �L���v�`���[�҂��E�G�C�g
#if 0
		(*seq)++;
		break;

	case 1:	// 0���C���҂��E�G�C�g
	case 2:	// �L���v�`���[�҂��E�G�C�g
		(*seq)++;
		break;

	case 3:
#endif
		//�`�惂�[�h�ύX
		{
			const GFL_BG_SYS_HEADER bg_sys_header = 
						{ GX_DISPMODE_GRAPHICS,GX_BGMODE_5,GX_BGMODE_0,GX_BG0_AS_3D };
			GFL_BG_SetBGMode( &bg_sys_header );
		}

		//�a�f�Z�b�g�A�b�v
		G2_SetBG2ControlDCBmp
			(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
		GX_SetBankForBG(GX_VRAM_BG_128_D);

		//�A���t�@�u�����h
		//G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE, GX_BLEND_PLANEMASK_BG2, 0,0);
		GFL_BG_SetPriority(BG_FRAME_CROSS_FADE, 0);
		GFL_BG_SetVisible( BG_FRAME_CROSS_FADE, VISIBLE_ON );

    // �L���v�`���ʂ̊g��J�n
    StartVBlankBGExpand();

		return GMEVENT_RES_FINISH;
	}

	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �N���X�t�F�[�h�C���C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT CrossInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq ) {
	case 0:
    //���C�������t�b�N
    FIELDMAP_SetMainFuncHookFlg(work->fieldmap, TRUE);

    work->alphaWork = 16;
    GFL_BG_SetPriority(BG_FRAME_CROSS_FADE, 0);
    GFL_BG_SetPriority(FLDBG_MFRM_3D, 1);
    GFL_BG_SetVisible( FLDBG_MFRM_3D, VISIBLE_ON );
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG0, work->alphaWork, 0 );

    // �t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 16, 0, FIELD_FADE_DEFAULT_WAIT );
    (*seq)++;
    break;

	case 1:	// �N���X�t�F�[�h
		if( work->alphaWork ) {
			work->alphaWork--;
			G2_ChangeBlendAlpha( work->alphaWork, 16 - work->alphaWork );
		} else {
			GFL_BG_SetVisible( BG_FRAME_CROSS_FADE, VISIBLE_OFF );

			//VRAM�N���A
			GX_SetBankForLCDC(GX_VRAM_LCDC_D);
			MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_VRAM_D_SIZE);
			(void)GX_DisableBankForLCDC();

			// ���C��BG�ւ̊��蓖�ĕ��A(fieldmap.c�Ɛ��������Ƃ邱��)
			GX_SetBankForBG(GX_VRAM_LCDC_D);	

      // �L���v�`��BG�ʂ̊g��I��
      EndVBlankBGExpand();

      (*seq)++;
		}
		break;

	case 2:	// �N���X�t�F�[�hEND
		//OS_WaitVBlankIntr();	// ��ʂ�����h�~�p�E�G�C�g
    // �t�B�[���hBG����
    ResetFieldBG( work->fieldmap );
    (*seq)++;
    break;
  case 3:
    //�a�f���A�g�����X�͂��ƂŁB
    FIELDMAP_InitBgNoTrans(work->fieldmap);
    (*seq)++;
    break;
  case 4:
    {
      FLDMSGBG * fmb = FIELDMAP_GetFldMsgBG( work->fieldmap );

      if ( FLDMSGBG_WaitResetBGResource( fmb ) )
      {
        FLDMSGBG_TranceResourceParts( fmb, MSGBG_TRANS_RES_FONTPAL );
        (*seq)++;
      }
    }
    break;
  case 5:
    {
      FLDMSGBG * fmb = FIELDMAP_GetFldMsgBG( work->fieldmap );
      FLDMSGBG_TranceResourceParts( fmb, MSGBG_TRANS_RES_WINFRM );
      (*seq)++;
    }
    break;
  case 6:
    {
      FLDMSGBG * fmb = FIELDMAP_GetFldMsgBG( work->fieldmap );
      FLDMSGBG_TranceResourceParts( fmb, MSGBG_TRANS_RES_SYSWIN );
      (*seq)++;
    }
    break;
  case 7:
#ifdef PM_DEBUG
    FIELDMAP_InitDebugWork( work->fieldmap );
#endif
    (*seq)++;
    //NO BREAK
	case 8:	// �N���X�t�F�[�hEND
    //���C�������t�b�N����
    FIELDMAP_SetMainFuncHookFlg(work->fieldmap, FALSE);
		return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �G�߃t�F�[�h�C�� �C�x���g
 */ 
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT SeasonInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
	case 0:
    // �G�ߕ\���C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_SeasonDisplay( gameSystem, fieldmap, work->startSeason, work->endSeason ) );
		(*seq)++;
		break;
  case 1:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �G�߃t�F�[�h�C�� �C�x���g ( �R�[���o�b�N�Ăяo���{ )
 */ 
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT SeasonInEvent_Callback( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
	case 0:
    // �G�ߕ\���C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_SeasonDisplay_Callback( 
          gameSystem, fieldmap, work->startSeason, work->endSeason,
          work->callback_func, work->callback_param ) );
		(*seq)++;
		break;
  case 1:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �z�[���A�E�g �C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT HoleOutEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
  // �t�F�[�h�J�n
  case 0:
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 0, 16, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

  // ���C�v�A�E�g�J�n
	case 1: 
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_HOLEOUT, WIPE_TYPE_HOLEOUT, 
        0x0000, WIPE_DEF_DIV, WIPE_DEF_SYNC, FIELDMAP_GetHeapID(fieldmap) );
    (*seq)++;
		break;

  // ���C�v�A�E�g�����҂�
  case 2:
    if( WIPE_SYS_EndCheck() ) { (*seq)++; }
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �V���b�^�[�A�E�g(��)
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT ShutterDownOutEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
  // �t�F�[�h�J�n
  case 0:
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 0, 16, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

  // �V���b�^�[�A�E�g�J�n
	case 1: 
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_SHUTTEROUT_DOWN, WIPE_TYPE_SHUTTEROUT_DOWN, 
        0x0000, WIPE_DEF_DIV, WIPE_DEF_SYNC, FIELDMAP_GetHeapID(fieldmap) );
    (*seq)++;
		break;

  // ���C�v�A�E�g�����҂�
  case 2:
    if( WIPE_SYS_EndCheck() ) {
      (*seq)++;
    }
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �V���b�^�[�A�E�g(��)
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT ShutterUpOutEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
  // �t�F�[�h�J�n
  case 0:
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 0, 16, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

  // �V���b�^�[�A�E�g�J�n
	case 1: 
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_SHUTTEROUT_UP, WIPE_TYPE_SHUTTEROUT_UP, 
        0x0000, WIPE_DEF_DIV, WIPE_DEF_SYNC, FIELDMAP_GetHeapID(fieldmap) );
    (*seq)++;
		break;

  // ���C�v�A�E�g�����҂�
  case 2:
    if( WIPE_SYS_EndCheck() ) { (*seq)++; }
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �X���C�h�A�E�g(��)
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT SlideRightOutEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
  // �t�F�[�h�J�n
  case 0:
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 0, 16, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

  // �V���b�^�[�A�E�g�J�n
	case 1: 
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_SLIDEOUT_LR, WIPE_TYPE_SLIDEOUT_LR, 
        0x0000, WIPE_DEF_DIV, WIPE_DEF_SYNC, FIELDMAP_GetHeapID(fieldmap) );
    (*seq)++;
		break;

  // ���C�v�A�E�g�����҂�
  case 2:
    if( WIPE_SYS_EndCheck() ) { (*seq)++; }
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �X���C�h�A�E�g(��)
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT SlideLeftOutEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
  // �t�F�[�h�J�n
  case 0:
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 0, 16, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

  // �V���b�^�[�A�E�g�J�n
	case 1: 
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_SLIDEOUT, WIPE_TYPE_SLIDEOUT, 
        0x0000, WIPE_DEF_DIV, WIPE_DEF_SYNC, FIELDMAP_GetHeapID(fieldmap) );
    (*seq)++;
		break;

  // ���C�v�A�E�g�����҂�
  case 2:
    if( WIPE_SYS_EndCheck() ) {
      (*seq)++;
    }
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �V���b�^�[�C��(��)
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT ShutterDownInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
  case 0:
    // �t�B�[���hBG����
    ResetFieldBG( work->fieldmap );
    InitFieldBG( work->fieldmap );
    // �t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 16, 0, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

  // �V���b�^�[�C���J�n
	case 1: 
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_SHUTTERIN_DOWN, WIPE_TYPE_SHUTTERIN_DOWN, 
        0x0000, WIPE_DEF_DIV, WIPE_DEF_SYNC, FIELDMAP_GetHeapID(fieldmap) );
    (*seq)++;
		break;

  // ���C�v�C�������҂�
  case 2:
    if( WIPE_SYS_EndCheck() ) { (*seq)++; }
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �V���b�^�[�C��(��)
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT ShutterUpInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
  case 0:
    // �t�B�[���hBG����
    ResetFieldBG( work->fieldmap );
    InitFieldBG( work->fieldmap );
    // �t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 16, 0, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

  // �V���b�^�[�C���J�n
	case 1: 
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_SHUTTERIN_UP, WIPE_TYPE_SHUTTERIN_UP, 
        0x0000, WIPE_DEF_DIV, WIPE_DEF_SYNC, FIELDMAP_GetHeapID(fieldmap) );
    (*seq)++;
		break;

  // ���C�v�C�������҂�
  case 2:
    if( WIPE_SYS_EndCheck() ) { (*seq)++; }
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �X���C�h�C��(��)
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT SlideRightInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
  case 0:
    // �t�B�[���hBG����
    ResetFieldBG( work->fieldmap );
    InitFieldBG( work->fieldmap );
    // �t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 16, 0, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

  // �V���b�^�[�C���J�n
	case 1: 
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_SLIDEIN, WIPE_TYPE_SLIDEIN, 
        0x0000, WIPE_DEF_DIV, WIPE_DEF_SYNC, FIELDMAP_GetHeapID(fieldmap) );
    (*seq)++;
		break;

  // ���C�v�C�������҂�
  case 2:
    if( WIPE_SYS_EndCheck() ) { (*seq)++; }
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �X���C�h�C��(��)
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT SlideLeftInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq ) {
  case 0:
    // �t�B�[���hBG����
    ResetFieldBG( work->fieldmap );
    InitFieldBG( work->fieldmap );
    // �t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 16, 0, FIELD_FADE_DEFAULT_WAIT );
		(*seq)++;
		break;

  // �V���b�^�[�C���J�n
	case 1: 
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_SLIDEIN_LR, WIPE_TYPE_SLIDEIN_LR, 
        0x0000, WIPE_DEF_DIV, WIPE_DEF_SYNC, FIELDMAP_GetHeapID(fieldmap) );
    (*seq)++;
		break;

  // ���C�v�C�������҂�
  case 2:
    if( WIPE_SYS_EndCheck() ) { (*seq)++; }
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�p �����A�E�g�C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT QuickOutEvent( GMEVENT* event, int* seq, void* wk )
{
  GFL_FADE_SetMasterBrightReq( 
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 
      FIELD_FADE_QUICK_WAIT);

  return GMEVENT_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�p �����t�F�[�h�C��
 */ 
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT QuickInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      16, 0, FIELD_FADE_QUICK_WAIT);

  // �t�B�[���hBG����
  ResetFieldBG( work->fieldmap );
  InitFieldBG( work->fieldmap );

  return GMEVENT_RES_FINISH;
} 

//--------------------------------------------------------------------------------------------
/**
 * @brief ����� �P�x�t�F�[�h�C�� �C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT FlySkyBrightInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq ) {
	case 0:
    // �P�x�t�F�[�h�J�n
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 16, 0, work->FadeSpeed );
    // �t�B�[���hBG����
    ResetFieldBG( work->fieldmap );
    InitFieldBG( work->fieldmap );
		(*seq)++;
		break;

	case 1:
    // �t�F�[�h�҂��Ȃ�
    if( work->fadeFinishWaitFlag == FIELD_FADE_NO_WAIT ){ return GMEVENT_RES_FINISH; }  
    // �t�F�[�h����
    if( GFL_FADE_CheckFade() == FALSE ){ return GMEVENT_RES_FINISH; }  
		break;
	} 
	return GMEVENT_RES_CONTINUE;
} 

//============================================================================================
// ��BG���A����
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * @brief BG���[�h�ݒ�ƕ\���ݒ�̕��A
 *
 * @param fieldmap
 */
//--------------------------------------------------------------------------------------------
static void ResetFieldBG( FIELDMAP_WORK* fieldmap )
{
  int mv = GFL_DISP_GetMainVisible();
  FIELDMAP_InitBGMode();
  GFL_DISP_GX_SetVisibleControlDirect( mv );
}
//--------------------------------------------------------------------------------------------
/**
 * @brief �t�B�[���hBG Vram�f�t�H���g������
 *
 * @param fieldmap
 */
//--------------------------------------------------------------------------------------------
static void InitFieldBG( FIELDMAP_WORK* fieldmap )
{
  FIELDMAP_InitBG( fieldmap );
}
