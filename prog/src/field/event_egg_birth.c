///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �^�}�S�z���C�x���g
 * @file   event_egg_birth.c
 * @author obata
 * @date   2010.02.05
 */
///////////////////////////////////////////////////////////////////////////////////////
#include "event_egg_birth.h"
#include "demo/egg_demo.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "event_field_fade.h"
#include "event_fieldmap_control.h"


//=====================================================================================
// ���C�x���g���[�N
//=====================================================================================
typedef struct 
{
	GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
	FIELDMAP_WORK* fieldmap;
  
  POKEMON_PARAM* egg;        // �z��������^�}�S
  EGG_DEMO_PARAM demoParam;  // �z���f�� �p�����[�^

} EVENT_WORK;


//-------------------------------------------------------------------------------------
/**
 * @brief �^�}�S�z���C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EggBirthEvent( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
  GAMEDATA*      gameData   = work->gameData;
  FIELDMAP_WORK* fieldmap   = work->fieldmap;

  // �C�x���g �V�[�P���X
  enum {
    SEQ_EGG_CHECK,          // �^�}�S�`�F�b�N
    SEQ_INIT_DEMO_PARAM,    // �f������
    SEQ_FADE_OUT,           // �t�F�[�h�A�E�g
    SEQ_FIELDMAP_CLOSE,     // �t�B�[���h�}�b�v�I��
    SEQ_CALL_DEMO,          // �f����ʌĂяo��
    SEQ_FIELDMAP_OPEN,      // �t�B�[���h�}�b�v���A
    SEQ_FADE_IN,            // �t�F�[�h�C��
    SEQ_END,                // �C�x���g�I��
  };

	switch( *seq ) 
  {
  // �^�}�S�`�F�b�N
  case SEQ_EGG_CHECK:
    // �^�}�S����Ȃ� or �s���^�}�S
    if( ( PP_Get( work->egg, ID_PARA_tamago_flag,       NULL ) == FALSE ) ||
        ( PP_Get( work->egg, ID_PARA_fusei_tamago_flag, NULL ) == TRUE  ) )
    {
      *seq = SEQ_END;
    }
    else
    {
      *seq = SEQ_INIT_DEMO_PARAM;
    }
    break;

  // �f������
  case SEQ_INIT_DEMO_PARAM:
    //GFL_OVERLAY_Load( FS_OVERLAY_ID(egg_demo) );
    //EGG_DEMO_InitParam( &(work->demoParam), gameData, work->egg );
    //GFL_OVERLAY_Unload( FS_OVERLAY_ID(egg_demo) );
    work->demoParam.gamedata = gameData;
    work->demoParam.pp       = work->egg;
    *seq = SEQ_FADE_OUT;
    break;

  // �t�F�[�h�A�E�g
	case SEQ_FADE_OUT: 
    GMEVENT_CallEvent( event, 
        EVENT_FieldFadeOut_Black( gameSystem, fieldmap,  FIELD_FADE_WAIT ) );
    *seq = SEQ_FIELDMAP_CLOSE;
		break;

  // �t�B�[���h�}�b�v�I��
	case SEQ_FIELDMAP_CLOSE: 
		GMEVENT_CallEvent( event, EVENT_FieldClose( gameSystem, fieldmap ) );
    *seq = SEQ_CALL_DEMO;
		break;

  // �f����ʌĂяo��
  case SEQ_CALL_DEMO:
    GAMESYSTEM_CallProc( gameSystem, FS_OVERLAY_ID(egg_demo), &EGG_DEMO_ProcData, &(work->demoParam) );
    *seq = SEQ_FIELDMAP_OPEN;
    break;

  // �t�B�[���h�}�b�v���A
	case SEQ_FIELDMAP_OPEN:
		GMEVENT_CallEvent( event, EVENT_FieldOpen( gameSystem ) );
    *seq = SEQ_FADE_IN;
		break;

  // �t�F�[�h�C��
	case SEQ_FADE_IN:
    GMEVENT_CallEvent(event, 
        EVENT_FieldFadeIn_Black( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    *seq = SEQ_END;
		break;

  // �C�x���g�I��
	case SEQ_END: 
		return GMEVENT_RES_FINISH; 
	} 
	return GMEVENT_RES_CONTINUE;
} 

//-------------------------------------------------------------------------------------
/**
 * @brief �^�}�S�z���C�x���g ����
 *
 * @param gameSystem
 * @param fieldmap
 * @param egg        �z��������^�}�S
 */
//-------------------------------------------------------------------------------------
GMEVENT* EVENT_EggBirth( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, POKEMON_PARAM* egg )
{
	GMEVENT* event;
	EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gameSystem, NULL, EggBirthEvent, sizeof(EVENT_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // ������
	work->gameSystem = gameSystem;
	work->fieldmap   = fieldmap;
  work->gameData   = GAMESYSTEM_GetGameData( gameSystem );
  work->egg        = egg;

  GAMEBEACON_Set_EggHatch( PP_Get( egg, ID_PARA_monsno, NULL ) );

	return event;
}
