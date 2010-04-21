///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �a������m�F��ʌĂяo���C�x���g
 * @file  event_dendou_pc.c
 * @author obata
 * @date   2010.03.30
 */
/////////////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "event_dendou_pc.h"
#include "app/dendou_pc.h"
#include "event_fieldmap_control.h" // for EVENT_FieldSubProc
#include "system/main.h" // for HEAPID_PROC


//=====================================================================================
// ��index
//=====================================================================================
static GMEVENT_RESULT DendouCallEvent( GMEVENT* event, int* seq, void* wk );


//=====================================================================================
// ���C�x���g���[�N
//===================================================================================== 
typedef struct {
	GAMESYS_WORK*  gameSystem;
	FIELDMAP_WORK* fieldmap;
  DENDOUPC_PARAM dendouParam;
  u16*           ret_wk; // �I�����[�h���i�[���郏�[�N
} EVENT_WORK; 


//=====================================================================================
// ��public func
//===================================================================================== 
//-------------------------------------------------------------------------------------
/**
 * @brief �a������m�F��ʌĂяo���C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param call_mode �Ăяo�����[�h
 * @param ret_wk    �I�����[�h���i�[���郏�[�N
 */
//-------------------------------------------------------------------------------------
GMEVENT* EVENT_DendouCall( GAMESYS_WORK* gameSystem, u16 call_mode, u16* ret_wk )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // �C�x���g����
  event = GMEVENT_Create( 
      gameSystem, NULL, DendouCallEvent, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );
  work->ret_wk     = ret_wk;

  // �v���b�N�p�����[�^��ݒ�
  work->dendouParam.gamedata = GAMESYSTEM_GetGameData( gameSystem );
  work->dendouParam.callMode = call_mode;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//=====================================================================================
// ��private func
//===================================================================================== 
//-------------------------------------------------------------------------------------
/**
 * @brief �a������m�F��ʌĂяo���C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT DendouCallEvent( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = wk;

  switch( *seq ) {
  case 0:
    GMEVENT_CallEvent( event, 
        EVENT_FieldSubProc( work->gameSystem, work->fieldmap, 
          FS_OVERLAY_ID(dendou_pc), &DENDOUPC_ProcData, &work->dendouParam ) );
    ++(*seq);
    break;

  case 1: 
    // �I�����[�h��Ԃ�
    switch( work->dendouParam.retMode ) {
    case DENDOUPC_RET_NORMAL: *(work->ret_wk) = 0; break;
    case DENDOUPC_RET_CLOSE:  *(work->ret_wk) = 1; break;
    }
    OBATA_Printf( "ret_wk = %d\n", *(work->ret_wk) );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
