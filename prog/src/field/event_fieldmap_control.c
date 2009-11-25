//============================================================================================
/**
 * @file	event_fieldmap_control.c
 * @brief	�C�x���g�F�t�B�[���h�}�b�v����c�[��
 * @author	tamada GAMEFREAK inc.
 * @date	2008.12.10
 */
//============================================================================================

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap_call.h"  //FIELDMAP_IsReady,FIELDMAP_ForceUpdate
#include "field/fieldmap.h"

#include "./event_fieldmap_control.h"

#include "system/main.h"      // HEAPID_PROC
FS_EXTERN_OVERLAY(pokelist);
FS_EXTERN_OVERLAY(poke_status);

//============================================================================================
//============================================================================================

//============================================================================================
//
//		�T�u�C�x���g
//
//============================================================================================
typedef struct {
	FIELD_FADE_TYPE fade_type;
  FIELD_FADE_WAIT_TYPE wait_type;
}FADE_EVENT_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldFadeOutEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK * few = work;
	switch (*seq) {
	case 0:
    // @todo �t�F�[�h���x�����ɖ߂�
    // ��ƌ���Up�̂��߂Ƀt�F�[�h��Z�k 11/17 obata
		GFL_FADE_SetMasterBrightReq(
				GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
				0, 16, -8);
		(*seq) ++;
		break;
	case 1:
    if( few->wait_type == FIELD_FADE_NO_WAIT ){ return GMEVENT_RES_FINISH; }
		if( GFL_FADE_CheckFade() == FALSE ){ return GMEVENT_RES_FINISH; }
		break;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�A�E�g�C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldFadeOut( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                              FIELD_FADE_TYPE type, FIELD_FADE_WAIT_TYPE wait )
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldFadeOutEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
	few->fade_type = type;
  few->wait_type = wait;

	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldFadeInEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK * few = work;
	switch (*seq) {
	case 0:
    // @todo �t�F�[�h���x�����ɖ߂�
    // ��ƌ���Up�̂��߂Ƀt�F�[�h��Z�k 11/17 obata
		GFL_FADE_SetMasterBrightReq(
				GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
				16, 0, -8);
		(*seq) ++;
		break;
	case 1:
    if( few->wait_type == FIELD_FADE_NO_WAIT ){ return GMEVENT_RES_FINISH; }
		if( GFL_FADE_CheckFade() == FALSE ){ return GMEVENT_RES_FINISH; }
		break;
	}

	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�C���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldFadeIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                             FIELD_FADE_TYPE type, FIELD_FADE_WAIT_TYPE wait )
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldFadeInEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
	few->fade_type = type;
  few->wait_type = wait;

	return event;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	FIELDMAP_WORK * fieldmap;
	GAMEDATA * gamedata;
}FIELD_OPENCLOSE_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldCloseEvent(GMEVENT * event, int * seq, void *work)
{
	FIELD_OPENCLOSE_WORK * focw = work;
	GAMESYS_WORK * gsys = focw->gsys;
	FIELDMAP_WORK * fieldmap = focw->fieldmap;
	switch (*seq) {
	case 0:
		FIELDMAP_Close(fieldmap);
		(*seq) ++;
		break;
	case 1:
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldClose(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldCloseEvent, sizeof(FIELD_OPENCLOSE_WORK));
	FIELD_OPENCLOSE_WORK * focw = GMEVENT_GetEventWork(event);
	focw->gsys = gsys;
	focw->fieldmap = fieldmap;
	focw->gamedata = GAMESYSTEM_GetGameData(gsys);
	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldOpenEvent(GMEVENT * event, int *seq, void*work)
{
	FIELD_OPENCLOSE_WORK * focw = work;
	GAMESYS_WORK * gsys = focw->gsys;
	FIELDMAP_WORK * fieldmap;	// = focw->fieldmap;
	switch(*seq) {
	case 0:
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
		GFL_FADE_SetMasterBrightReq(
				GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
        16, 16, 0);
		GAMESYSTEM_CallFieldProc(gsys);
		(*seq) ++;
		break;
	case 1:
		fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
		if (FIELDMAP_IsReady(fieldmap) == FALSE) break;
		(*seq) ++;
		break;
	case 2:
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldOpen(GAMESYS_WORK *gsys)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldOpenEvent, sizeof(FIELD_OPENCLOSE_WORK));
	FIELD_OPENCLOSE_WORK * focw = GMEVENT_GetEventWork(event);
	focw->gsys = gsys;
	focw->fieldmap = NULL;
	focw->gamedata = GAMESYSTEM_GetGameData(gsys);
	return event;
}

//============================================================================================
//
//	�C�x���g�F�ʉ�ʌĂяo��
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	FIELDMAP_WORK * fieldmap;
	FSOverlayID ov_id;
	const GFL_PROC_DATA * proc_data;
	void * proc_work;
}CHANGE_SAMPLE_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent(GMEVENT * event, int * seq, void * work)
{
	CHANGE_SAMPLE_WORK * csw = work;
	GAMESYS_WORK *gsys = csw->gsys;

	switch(*seq) {
	case 0:
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, csw->fieldmap, 0, FIELD_FADE_WAIT));
		(*seq) ++;
		break;
	case 1:
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, csw->fieldmap));
		(*seq) ++;
		break;
	case 2:
		GAMESYSTEM_CallProc(gsys, csw->ov_id, csw->proc_data, csw->proc_work);
		(*seq) ++;
		break;
	case 3:
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
		(*seq) ++;
		break;
	case 4:
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
	case 5:
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, csw->fieldmap, 0, FIELD_FADE_WAIT));
		(*seq) ++;
		break;
	case 6:
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldSubProc(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work)
{
#if 1
	GMEVENT * event = GMEVENT_Create(gsys, NULL, GameChangeEvent, sizeof(CHANGE_SAMPLE_WORK));
	CHANGE_SAMPLE_WORK * csw = GMEVENT_GetEventWork(event);
	csw->gsys = gsys;
	csw->fieldmap = fieldmap;
	csw->ov_id = ov_id;
	csw->proc_data = proc_data;
	csw->proc_work = proc_work;
#endif
	return event;
}


//============================================================================================
//
//	�C�x���g�F�ʉ�ʌĂяo��(�R�[���o�b�N�Ăяo���t��)
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	FIELDMAP_WORK * fieldmap;
	FSOverlayID ov_id;
	const GFL_PROC_DATA * proc_data;
	void * proc_work;
  void ( *callback )( void* );  // �R�[���o�b�N�֐�
  void * callback_work;         // �R�[���o�b�N�֐��̈���
}SUBPROC_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent_Callback(GMEVENT * event, int * seq, void * work)
{
	SUBPROC_WORK * spw = work;
	GAMESYS_WORK *gsys = spw->gsys;

	switch(*seq) 
  {
	case 0: // �t�F�[�h�A�E�g
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, spw->fieldmap, 0, FIELD_FADE_WAIT));
		(*seq) ++;
		break;
	case 1: // �t�B�[���h�}�b�v�I��
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, spw->fieldmap));
		(*seq) ++;
		break;
	case 2: // �v���Z�X�Ăяo��
		GAMESYSTEM_CallProc(gsys, spw->ov_id, spw->proc_data, spw->proc_work);
		(*seq) ++;
		break;
	case 3: // �v���Z�X�I���҂�
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
		(*seq) ++;
		break;
	case 4: // �t�B�[���h�}�b�v���A
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
	case 5: // �t�F�[�h�C��
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, spw->fieldmap, 0, FIELD_FADE_WAIT));
		(*seq) ++;
		break;
	case 6: // �R�[���o�b�N�֐��Ăяo��
    if( spw->callback ) spw->callback( spw->callback_work );
		(*seq) ++;
    break;
  case 7:
    if( spw->callback_work ) GFL_HEAP_FreeMemory( spw->callback_work );
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldSubProc_Callback(
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work,
    void (*callback)(void*), void* callback_work )
{
	GMEVENT *     event = GMEVENT_Create(gsys, NULL, GameChangeEvent_Callback, sizeof(SUBPROC_WORK));
	SUBPROC_WORK *  spw = GMEVENT_GetEventWork(event);
	spw->gsys          = gsys;
	spw->fieldmap      = fieldmap;
	spw->ov_id         = ov_id;
	spw->proc_data     = proc_data;
	spw->proc_work     = proc_work;
  spw->callback      = callback;
  spw->callback_work = callback_work;
	return event;
}



//============================================================================================
//
//	�C�x���g�F�|�P�����I��
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	FIELDMAP_WORK * fieldmap;
  PLIST_DATA* plData;
  PSTATUS_DATA* psData;
}POKE_SELECT_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_PokeSelect(GMEVENT * event, int * seq, void * work)
{
	POKE_SELECT_WORK * psw = work;
	GAMESYS_WORK *gsys = psw->gsys;

  // �V�[�P���X��`
  enum
  {
    SEQ_CALL_POKELIST = 0,
    SEQ_WAIT_POKELIST,
    SEQ_EXIT_POKELIST,
    SEQ_CALL_POKESTATUS,
    SEQ_WAIT_POKESTATUS,
    SEQ_END,
  };

	switch( *seq ) 
  {
	case SEQ_CALL_POKELIST: //// �|�P�������X�g�Ăяo��
		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(pokelist), &PokeList_ProcData, psw->plData);
    *seq = SEQ_WAIT_POKELIST;
		break;
	case SEQ_WAIT_POKELIST: //// �v���b�N�I���҂�
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
    *seq = SEQ_EXIT_POKELIST;
		break;
  case SEQ_EXIT_POKELIST: //// �|�P�������X�g�̏I����Ԃŕ���
    if( psw->plData->ret_mode == PL_RET_NORMAL )   // �I���I��
    {
      *seq = SEQ_END;
    }
    else if( psw->plData->ret_mode == PL_RET_STATUS )  //�u�悳���݂�v��I��
    {
      psw->psData->pos = psw->plData->ret_sel;   // �\������f�[�^�ʒu = �I���|�P
      *seq = SEQ_CALL_POKESTATUS;
    }
    else  // ���Ή��ȍ��ڂ�I��
    {
      OBATA_Printf( "----------------------------------------------------\n" );
      OBATA_Printf( "�|�P�����I���C�x���g: ���Ή��ȍ��ڂ��I������܂����B\n" );
      OBATA_Printf( "----------------------------------------------------\n" );
      *seq = SEQ_END;
    }
    break;
  case SEQ_CALL_POKESTATUS: //// �|�P�����X�e�[�^�X�Ăяo��
		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(poke_status), &PokeStatus_ProcData, psw->psData);
    *seq = SEQ_WAIT_POKESTATUS;
		break;
	case SEQ_WAIT_POKESTATUS: //// �v���b�N�I���҂�
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
    psw->plData->ret_sel = psw->psData->pos;  // �X�e�[�^�X��ʏI�����̃|�P�����ɃJ�[�\�������킹��
    *seq = SEQ_CALL_POKELIST;
		break;
	case SEQ_END: //// �C�x���g�I��
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_PokeSelect( 
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    PLIST_DATA* list_data, PSTATUS_DATA* status_data )
{
	GMEVENT* event;
	POKE_SELECT_WORK* psw;

  // �C�x���g����
  event = GMEVENT_Create(gsys, NULL, EVENT_FUNC_PokeSelect, sizeof(POKE_SELECT_WORK));
  psw   = GMEVENT_GetEventWork(event);
	psw->gsys      = gsys;
	psw->fieldmap  = fieldmap;
  psw->plData    = list_data;
  psw->psData    = status_data;
	return event;
}
