//============================================================================================
/**
 * @file	event_gameclear.c
 * @brief	�C�x���g�F�Q�[���N���A����
 * @date  2009.12.14
 * @author	tamada GAME FREAK inc.
 *
 * 2009.12.14 event_gameover.c����R�s�y�ō쐬
 *
 * @todo
 * �Q�[���I�[�o�[���b�Z�[�W��PROC�ɂ����̂Ń�����������Ȃ��Ƃ��ɂ�
 * �ʃI�[�o�[���C�̈�Ɉ��z������
 */
//============================================================================================
#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"

#include "event_gameclear.h"  //GAMECLEAR_MODE

#include "system/main.h"  //HEAPID_�`

#include "script.h"       //SCRIPT_CallScript
#include "event_gamestart.h"

#include "demo/demo3d.h"  //Demo3DProcData etc.
#include "app/local_tvt_sys.h"  //LocalTvt_ProcData etc.

#include "sound/pm_sndsys.h"  //PMSND_

#include "move_pokemon.h"

#include "proc_gameclear_save.h"

#include "savedata/save_tbl.h"
#include "savedata/record.h"
#include "savedata/gametime.h"
#include "savedata/playtime.h"
#include "ev_time.h"

//==============================================================================================
//==============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA* gamedata;
  GAMECLEAR_MODE clear_mode;
  BOOL saveSuccessFlag;
  const MYSTATUS * mystatus;

  BOOL dendouSaveInitFlag;   //�u�a������v�f�[�^�̕����Z�[�u�������������������ǂ���
  BOOL dendouSaveFinishFlag; //�u�a������v�f�[�^�̕����Z�[�u�������������ǂ���

  GAMECLEAR_MSG_PARAM para_child;
}GAMECLEAR_WORK;


//============================================================================================
//============================================================================================
// ���C���V�[�P���X
enum {
	GMCLEAR_SEQ_INIT,			    // ������
	GMCLEAR_SEQ_DENDOU_DEMO,	// �a������f��
	GMCLEAR_SEQ_SAVE_START,		// �Z�[�u�J�n
	GMCLEAR_SEQ_SAVE_MESSAGE,	// �Z�[�u�����b�Z�[�W�\��
	GMCLEAR_SEQ_SAVE_MAIN,		// �Z�[�u���C��
	GMCLEAR_SEQ_SAVE_END,			// �Z�[�u�I��
	GMCLEAR_SEQ_WAIT1,				// �E�F�C�g�P
	GMCLEAR_SEQ_FADE_OUT,			// �t�F�[�h�A�E�g�Z�b�g
	GMCLEAR_SEQ_BGM_FADE_OUT,	// BGM�t�F�[�h�A�E�g�Z�b�g
	GMCLEAR_SEQ_WAIT2,				// �E�F�C�g�Q
	GMCLEAR_SEQ_ENDING_DEMO,	// �G���f�B���O�f��
	GMCLEAR_SEQ_END,				  // �I��
};


//============================================================================================
//============================================================================================
static void ElboardStartChampNews( GAMECLEAR_WORK* work ); // �d���f���Ƀ`�����s�I���j���[�X�̕\�����J�n����
static void UpdateFirstClearDendouData( GAMECLEAR_WORK* work ); // ����N���A���̓a������f�[�^��o�^����
static void UpdateFirstClearRecord( GAMECLEAR_WORK* work ); // ����N���A���̃��R�[�h��o�^����
static void UpdateDendouRecord( GAMECLEAR_WORK* work ); // �a�����莞�̃��R�[�h��o�^����
static void DendouSave_init( GAMECLEAR_WORK* work ); // �a������f�[�^�̍X�V����
static void DendouSave_main( GAMECLEAR_WORK* work ); // �a������f�[�^�̍X�V����

//============================================================================================
//============================================================================================

//-----------------------------------------------------------------------------
/**
 * @brief	�Q�[���N���A�C�x���g
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @retval	TRUE		�C�x���g�I��
 * @retval	FALSE		�C�x���g�p����
 *
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_GameClear(GMEVENT * event, int * seq, void *work)
{
  GAMECLEAR_WORK * gcwk = work;
  GMEVENT * call_event;
  GAMEDATA * gamedata = gcwk->gamedata;
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork( gamedata );

	switch (*seq) {
	case GMCLEAR_SEQ_INIT:
    PMSND_FadeOutBGM( 30 );
		(*seq) ++;
		break;

	case GMCLEAR_SEQ_DENDOU_DEMO:
    //if ( PMSND_CheckFadeOnBGM() == FALSE ) { break; }
    //if ( GFL_FADE_CheckFade() == TRUE ) { break; }
    //�t�B�[���h�}�b�v���I��������
    { 
      FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gcwk->gsys );
      GMEVENT * new_event = DEBUG_EVENT_GameEnd( gcwk->gsys, fieldmap );
      GMEVENT_CallEvent( event, new_event );
    }
    SCRIPT_CallGameClearScript( gcwk->gsys, HEAPID_PROC ); 
		(*seq) ++;
		break;

	case GMCLEAR_SEQ_SAVE_START:
    // �d���f���Ƀ`�����s�I���j���[�X��\��
    ElboardStartChampNews( gcwk );
    //�u����N���A�v���Z�[�u
    if( gcwk->clear_mode == GAMECLEAR_MODE_FIRST ) {
      UpdateFirstClearDendouData( gcwk ); // ���u�a������v�f�[�^
      EVTIME_SetGameClearDateTime( gamedata ); // ����N���A�̓���
      UpdateFirstClearRecord( gcwk ); // ���R�[�h�f�[�^
    }
    //�u�a������v���Z�[�u
    else {
      UpdateDendouRecord( gcwk );
      DendouSave_init( gcwk );
    }
    (*seq) ++;
    break;

  case GMCLEAR_SEQ_SAVE_MESSAGE:
    GMEVENT_CallProc(
        event, FS_OVERLAY_ID(gameclear_demo), &GameClearMsgProcData, &gcwk->para_child );
    (*seq) ++;
		break;

	case GMCLEAR_SEQ_SAVE_MAIN:
    // �a������f�[�^���Z�[�u
    if( gcwk->dendouSaveInitFlag ) {
      DendouSave_main( gcwk );

      // �Z�[�u�I��
      if( gcwk->dendouSaveFinishFlag ) {
        *seq = GMCLEAR_SEQ_END;
      }
    }
    else {
      *seq = GMCLEAR_SEQ_END;
    }
		break;

	case GMCLEAR_SEQ_END:
    return GMEVENT_RES_FINISH;

  case GMCLEAR_SEQ_END + 1:
    /* �������[�v */
    return GMEVENT_RES_CONTINUE;

	}
	return GMEVENT_RES_CONTINUE;
}


//-----------------------------------------------------------------------------
/**
 * @brief	�C�x���g�R�}���h�F�ʏ�S�ŏ���
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_GameClear( GAMESYS_WORK * gsys, GAMECLEAR_MODE mode )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
	GMEVENT * event;
  GAMECLEAR_WORK * gcwk;

  event = GMEVENT_Create( gsys, NULL, GMEVENT_GameClear, sizeof(GAMECLEAR_WORK) );

  gcwk = GMEVENT_GetEventWork( event );
  gcwk->gsys = gsys;
  gcwk->gamedata = gamedata;
  gcwk->clear_mode = mode;
  gcwk->mystatus = GAMEDATA_GetMyStatus( gamedata );
  gcwk->dendouSaveInitFlag = FALSE;
  gcwk->dendouSaveFinishFlag = FALSE;

  //�q�v���Z�X�ɓn���p�����[�^
  gcwk->para_child.gsys = gsys;
  gcwk->para_child.clear_mode = mode;

  //�ړ��|�P���������`�F�b�N
  MP_RecoverMovePoke( gamedata );

  return event;
}


//-----------------------------------------------------------------------------
/**
 * @brief �d���f���Ƀ`�����s�I���j���[�X�̕\�����J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void ElboardStartChampNews( GAMECLEAR_WORK* work )
{
  MISC* misc;
  int minutes;

  misc = GAMEDATA_GetMiscWork( work->gamedata );
  minutes = 60 * 24; // �\�����鎞��[min]
  MISC_SetChampNewsMinutes( misc, minutes );
}

//-----------------------------------------------------------------------------
/**
 * @brief ����N���A���̓a������f�[�^��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateFirstClearDendouData( GAMECLEAR_WORK* work )
{
  RTCDate date;
  POKEPARTY* party; 
  DENDOU_RECORD* record; 

  RTC_GetDate( &date );
  party  = GAMEDATA_GetMyPokemon( work->gamedata ); 
  record = GAMEDATA_GetDendouRecord( work->gamedata ); 
  DendouRecord_Add( record, party, &date, HEAPID_PROC );
}

//-----------------------------------------------------------------------------
/**
 * @brief ����N���A���̃��R�[�h��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateFirstClearRecord( GAMECLEAR_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  RECORD* record;
  PLAYTIME* ptime;
  u32 hour, minute, second, time;

  save   = GAMEDATA_GetSaveControlWork( work->gamedata );
  record = GAMEDATA_GetRecordPtr( work->gamedata );
  ptime  = SaveData_GetPlayTime( save );

  // �v���C���Ԃ��擾
  hour   = PLAYTIME_GetHour( ptime );
  minute = PLAYTIME_GetMinute( ptime );
  second = PLAYTIME_GetSecond( ptime ); 
  GF_ASSERT( hour   <= PTIME_HOUR_MAX );
  GF_ASSERT( minute <= PTIME_MINUTE_MAX );
  GF_ASSERT( second <= PTIME_SECOND_MAX );

  // �v���C���Ԃ�o�^
  time = hour * 10000 + minute * 100 + second;
  RECORD_Set( record, RECID_CLEAR_TIME, time );
}

//-----------------------------------------------------------------------------
/**
 * @brief �a�����莞�̃��R�[�h��o�^����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateDendouRecord( GAMECLEAR_WORK* work )
{
  RECORD* record;

  record = GAMEDATA_GetRecordPtr( work->gamedata );
  RECORD_Add( record, RECID_DENDOU_CNT, 1 );
}

//-----------------------------------------------------------------------------
/**
 * @brief �a������f�[�^�̍X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DendouSave_init( GAMECLEAR_WORK* work )
{
  RTCDate date;
  POKEPARTY* party;
  SAVE_CONTROL_WORK* save;
  DENDOU_SAVEDATA* dendouData;
  LOAD_RESULT result; 

  RTC_GetDate( &date );
  party = GAMEDATA_GetMyPokemon( work->gamedata ); 
  save = GAMEDATA_GetSaveControlWork( work->gamedata );
  result = SaveControl_Extra_Load( save, SAVE_EXTRA_ID_DENDOU, HEAPID_PROC );
  dendouData = SaveControl_Extra_DataPtrGet( save, SAVE_EXTRA_ID_DENDOU, EXGMDATA_ID_DENDOU );

  DendouData_AddRecord( dendouData, party, &date, HEAPID_PROC ); 
  SaveControl_Extra_SaveAsyncInit( save, SAVE_EXTRA_ID_DENDOU ); 

  work->dendouSaveInitFlag = TRUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �a������f�[�^�̍X�V����
 *
 * @param work 
 */
//-----------------------------------------------------------------------------
static void DendouSave_main( GAMECLEAR_WORK* work )
{
  SAVE_CONTROL_WORK* save;
  SAVE_RESULT save_ret;

  // �Z�[�u���s
  save     = GAMEDATA_GetSaveControlWork( work->gamedata );
  save_ret = SaveControl_Extra_SaveAsyncMain( save, SAVE_EXTRA_ID_DENDOU ); 

  // �Z�[�u�I��
  if( save_ret == SAVE_RESULT_OK || save_ret == SAVE_RESULT_NG ) { 
    SaveControl_Extra_Unload( save, SAVE_EXTRA_ID_DENDOU ); // �O���f�[�^�����
    work->dendouSaveFinishFlag = TRUE;
  }
}
