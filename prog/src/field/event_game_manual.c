///////////////////////////////////////////////////////////////////////////
/**
 * @brief  �Q�[�����}�j���A���̌Ăяo���C�x���g
 * @file   event_game_manual.c
 * @author obata
 * @date   2010.04.27
 */
///////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamesystem.h" 
#include "gamesystem/game_event.h" 
#include "system/main.h"
#include "app/manual.h"
#include "fieldmap.h"
#include "script_def.h"
#include "event_fieldmap_control.h"
#include "event_game_manual.h"


//=========================================================================
// ���C�x���g���[�N
//=========================================================================
typedef struct {

  GAMESYS_WORK*  gameSystem;
  FIELDMAP_WORK* fieldmap;
  MANUAL_PARAM*  manualParam;
  u16*           ret_wk;  // �I�����ʂ̊i�[��

} EVENT_WORK;


//=========================================================================
// ��prototype
//=========================================================================
static GMEVENT_RESULT GameManualEvent( GMEVENT* event, int* seq, void* wk );
static void ReturnManualResult( EVENT_WORK* work ); // �Q�[���}�j���A���̏I�����ʂ�Ԃ�


//=========================================================================
// ��public functions
//=========================================================================

//-------------------------------------------------------------------------
/**
 * @brief �Q�[�����}�j���A���̌Ăяo���C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param ret_wk     �I�����ʂ��i�[���郏�[�N�ւ̃|�C���^
 *
 * @return �Q�[�����}�j���A���Ăяo���C�x���g
 */
//-------------------------------------------------------------------------
GMEVENT* EVENT_GameManual( GAMESYS_WORK* gameSystem, u16* ret_wk )
{
  GMEVENT* event;
  EVENT_WORK* work;
  GAMEDATA* gameData;
  FIELDMAP_WORK* fieldmap;

  gameData = GAMESYSTEM_GetGameData( gameSystem );
  fieldmap = GAMESYSTEM_GetFieldMapWork( gameSystem );

  // �C�x���g�𐶐�
  event = GMEVENT_Create( gameSystem, NULL, GameManualEvent, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldmap = fieldmap;

  // �}�j���A���Ăяo������
  work->manualParam = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(MANUAL_PARAM) );
  work->manualParam->gamedata = gameData;

  return event;
}


//=========================================================================
// ��private functions
//=========================================================================

//------------------------------------------------------------------------- 
/**
 * @brief �Q�[�����}�j���A���Ăяo���C�x���g
 *
 * @param event
 * @param seq
 * @param wk
 */
//-------------------------------------------------------------------------
static GMEVENT_RESULT GameManualEvent( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = wk;

  switch( *seq ) {
  case 0:
    // �T�u�v���Z�X�Ăяo��
    GMEVENT_CallEvent( event,
        EVENT_FieldSubProc( work->gameSystem, work->fieldmap, 
          FS_OVERLAY_ID(manual), &MANUAL_ProcData, work->manualParam ) );
    (*seq)++;
    break;

  case 1:
    // �I�����ʂ��i�[
    ReturnManualResult( work );
    // ��Еt��
    GFL_HEAP_FreeMemory( work->manualParam );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-------------------------------------------------------------------------
/**
 * @brief �Q�[���}�j���A���̏I�����ʂ�Ԃ�
 *
 * @param work
 */
//-------------------------------------------------------------------------
static void ReturnManualResult( EVENT_WORK* work )
{
  if( work->manualParam->result == MANUAL_RESULT_CLOSE ) {
    *(work->ret_wk) = SCR_MANUAL_RESULT_CLOSE;
  }
  else {
    *(work->ret_wk) = SCR_MANUAL_RESULT_RETURN;
  }
}
