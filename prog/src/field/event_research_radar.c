///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[��ʌĂяo���C�x���g
 * @file   event_research_radar.c
 * @author obata
 * @date   2010.02.02
 */
///////////////////////////////////////////////////////////////////////////////////////
#include "event_research_radar.h"
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "app/research_radar.h"
#include "event_field_fade.h"
#include "event_fieldmap_control.h"

FS_EXTERN_OVERLAY( research_radar );


//=====================================================================================
// ���C�x���g���[�N
//=====================================================================================
typedef struct 
{
	GAMESYS_WORK*  gameSystem;
	FIELDMAP_WORK* fieldmap;

  RESEARCH_PARAM* researchParam;  // �������[�_�[�Ăяo���p�����[�^

} EVENT_WORK;


//-------------------------------------------------------------------------------------
/**
 * @brief �������[�_�[��ʌĂяo���C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT ResearchRadarCallEvent( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
  FIELDMAP_WORK* fieldmap   = work->fieldmap;

  // �C�x���g �V�[�P���X
  enum {
    SEQ_FADE_OUT,           // �t�F�[�h�A�E�g
    SEQ_FIELDMAP_CLOSE,     // �t�B�[���h�}�b�v�I��
    SEQ_CALL_RESEARCH_PROC, // �������[�_�[��ʌĂяo��
    SEQ_FIELDMAP_OPEN,      // �t�B�[���h�}�b�v���A
    SEQ_FADE_IN,            // �t�F�[�h�C��
    SEQ_END,                // �C�x���g�I��
  };

	switch( *seq ) 
  {
  // �t�F�[�h�A�E�g
	case SEQ_FADE_OUT: 
    GMEVENT_CallEvent( event, 
        EVENT_FieldFadeOut_Black( gameSystem, fieldmap,  FIELD_FADE_WAIT ) );
    *seq = SEQ_FIELDMAP_CLOSE;
		break;

  // �t�B�[���h�}�b�v�I��
	case SEQ_FIELDMAP_CLOSE: 
		GMEVENT_CallEvent( event, EVENT_FieldClose( gameSystem, fieldmap ) );
    *seq = SEQ_CALL_RESEARCH_PROC;
		break;

  // �������[�_�[��ʌĂяo��
  case SEQ_CALL_RESEARCH_PROC:
    GAMESYSTEM_CallProc( gameSystem, FS_OVERLAY_ID(research_radar), &ResearchRadarProcData, work->researchParam );
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
    GFL_HEAP_FreeMemory( work->researchParam );
		return GMEVENT_RES_FINISH; 
	} 
	return GMEVENT_RES_CONTINUE;
} 


//-------------------------------------------------------------------------------------
/**
 * @brief �������[�_�[��ʌĂяo���C�x���g ����
 *
 * @param gameSystem
 * @param fieldmap
 */
//-------------------------------------------------------------------------------------
GMEVENT* EVENT_ResearchRadar( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gameSystem, NULL, ResearchRadarCallEvent, sizeof(EVENT_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // ������
	work->gameSystem    = gameSystem;
	work->fieldmap      = fieldmap;
  work->researchParam = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(RESEARCH_PARAM) );

  // �������[�_�[�Ăяo���p�����[�^�ݒ�
  work->researchParam->gameSystem = gameSystem;

	return event;
}
