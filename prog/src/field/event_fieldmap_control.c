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

#include "./field_sound_system.h"
#include "./event_fieldmap_control.h"
#include "./event_fieldmap_control_local.h"
#include "./event_gamestart.h"

#include "system/main.h"      // HEAPID_PROC


FS_EXTERN_OVERLAY(pokelist);
FS_EXTERN_OVERLAY(poke_status);



//============================================================================================
// ���t�B�[���h�}�b�v�̊J�n/�I��
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	FIELDMAP_WORK * fieldmap;
	GAMEDATA * gamedata;

  BOOL unload_fieldinit;
}FIELD_OPENCLOSE_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldCloseEvent(GMEVENT * event, int * seq, void *work)
{
	FIELD_OPENCLOSE_WORK * focw = work;
	GAMESYS_WORK * gsys = focw->gsys;
	FIELDMAP_WORK * fieldmap = focw->fieldmap;
	FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( focw->gamedata );

	switch (*seq) {
	case 0:
		FIELDMAP_Close(fieldmap);
		(*seq) ++;
		break;
	case 1:
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
		if (FIELD_SOUND_HaveRequest(fsnd) == TRUE) break;
     
    if( focw->unload_fieldinit ){
      // �t�B�[���h�������p�I�[�o�[���C�j��
      GAMESTART_OVERLAY_FIELD_INIT_UnLoad();
    }
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
  focw->unload_fieldinit = TRUE;
	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldCloseEvent_NoWait(GMEVENT * event, int * seq, void *work)
{
	FIELD_OPENCLOSE_WORK * focw = work;
	GAMESYS_WORK * gsys = focw->gsys;
	FIELDMAP_WORK * fieldmap = focw->fieldmap;
	FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( focw->gamedata );

	switch (*seq) {
	case 0:
		FIELDMAP_Close(fieldmap);
		(*seq) ++;
		break;
	case 1:
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
     
    if( focw->unload_fieldinit ){
      // �t�B�[���h�������p�I�[�o�[���C�j��
      GAMESTART_OVERLAY_FIELD_INIT_UnLoad();
    }
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
// event_mapchange��p
//------------------------------------------------------------------
GMEVENT * EVENT_FieldClose_FieldProcOnly(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldCloseEvent_NoWait, sizeof(FIELD_OPENCLOSE_WORK));
	FIELD_OPENCLOSE_WORK * focw = GMEVENT_GetEventWork(event);
	focw->gsys = gsys;
	focw->fieldmap = fieldmap;
	focw->gamedata = GAMESYSTEM_GetGameData(gsys);
  focw->unload_fieldinit = FALSE;
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
#if 0
		GFL_FADE_SetMasterBrightReq(
				GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
        16, 16, 0);
#endif
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

  // �t�B�[���h�������p�I�[�o�[���C�ǂݍ���
  GAMESTART_OVERLAY_FIELD_INIT_Load();

	return event;
}

//------------------------------------------------------------------
// event_mapchange��p
//------------------------------------------------------------------
GMEVENT * EVENT_FieldOpen_FieldProcOnly(GAMESYS_WORK *gsys)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldOpenEvent, sizeof(FIELD_OPENCLOSE_WORK));
	FIELD_OPENCLOSE_WORK * focw = GMEVENT_GetEventWork(event);
	focw->gsys = gsys;
	focw->fieldmap = NULL;
	focw->gamedata = GAMESYSTEM_GetGameData(gsys);

	return event;
}



//============================================================================================
// ���T�u�v���Z�X�Ăяo��
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct
{
	GAMESYS_WORK*             gsys;
	FIELDMAP_WORK*        fieldmap;
	FSOverlayID              ov_id;
	const GFL_PROC_DATA* proc_data;
	void*                proc_work;

} CHANGE_SAMPLE_WORK;
//------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�
 */
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent( GMEVENT* event, int* seq, void* work )
{
	CHANGE_SAMPLE_WORK* csw = work;
	GAMESYS_WORK*      gsys = csw->gsys;

	switch( *seq )
  {
	case 0:
    { // �t�F�[�h�A�E�g
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gsys, csw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
		(*seq) ++;
		break;
	case 1:
    { // �T�u�v���Z�X�Ăяo���C�x���g
      GMEVENT* ev_sub;
      ev_sub = EVENT_FieldSubProcNoFade( csw->gsys, csw->fieldmap, 
                                        csw->ov_id, csw->proc_data, csw->proc_work );
      GMEVENT_CallEvent( event, ev_sub );
    }
		(*seq) ++;
		break;
	case 2:
    { // �t�F�[�h�C��
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, csw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
		(*seq) ++;
		break;
	case 3:
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}
//------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�(�t�F�[�h�Ȃ�)
 */
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent_NoFade(GMEVENT * event, int * seq, void * work)
{
	CHANGE_SAMPLE_WORK* csw = work;
	GAMESYS_WORK*      gsys = csw->gsys;

	switch( *seq )
  {
	case 0:  // �t�B�[���h�}�b�v�j��
		GMEVENT_CallEvent( event, EVENT_FieldClose(gsys, csw->fieldmap) );
		(*seq) ++;
		break;
	case 1:  // �T�u�v���Z�X�Ăяo��
		GAMESYSTEM_CallProc( gsys, csw->ov_id, csw->proc_data, csw->proc_work );
		(*seq) ++;
		break;
	case 2:  // �T�u�v���Z�X�I���҂�
		if( GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL ) break;
		(*seq) ++;
		break;
	case 3:  // �t�B�[���h�}�b�v�ĊJ
		GMEVENT_CallEvent( event, EVENT_FieldOpen(gsys) );
		(*seq) ++;
		break;
	case 4:
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		  �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	�J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	�J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @return ���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
GMEVENT* EVENT_FieldSubProc( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap,
		                         FSOverlayID ov_id, 
                             const GFL_PROC_DATA* proc_data, void* proc_work )
{
	GMEVENT* event;
	CHANGE_SAMPLE_WORK* csw; 

  // ����
  event = GMEVENT_Create( gsys, NULL, GameChangeEvent, sizeof(CHANGE_SAMPLE_WORK) );
  // ������
	csw = GMEVENT_GetEventWork( event );
	csw->gsys      = gsys;
	csw->fieldmap  = fieldmap;
	csw->ov_id     = ov_id;
	csw->proc_data = proc_data;
	csw->proc_work = proc_work;
	return event;
}

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����(�t�F�[�h�Ȃ�)
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		  �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	�J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	�J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @return ���������C�x���g�ւ̃|�C���^
 *
 * �t�B�[���h�}�b�v�I�� �� �T�u�v���Z�X�Ăяo�� �� �t�B�[���h�}�b�v�ĊJ
 */
//------------------------------------------------------------------
GMEVENT* EVENT_FieldSubProcNoFade( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap,
		                               FSOverlayID ov_id, 
                                   const GFL_PROC_DATA* proc_data, void* proc_work )
{
	GMEVENT* event;
	CHANGE_SAMPLE_WORK* csw; 

  // ����
  event = GMEVENT_Create( gsys, NULL, GameChangeEvent_NoFade, sizeof(CHANGE_SAMPLE_WORK) );
  // ������
	csw = GMEVENT_GetEventWork( event );
	csw->gsys      = gsys;
	csw->fieldmap  = fieldmap;
	csw->ov_id     = ov_id;
	csw->proc_data = proc_data;
	csw->proc_work = proc_work;
	return event;
}


//============================================================================================
//
//	�C�x���g�F�ʉ�ʌĂяo��(�R�[���o�b�N�Ăяo���t��)
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct
{
	GAMESYS_WORK*             gsys;
	FIELDMAP_WORK*        fieldmap;
	FSOverlayID              ov_id;
	const GFL_PROC_DATA* proc_data;
	void*                proc_work;
  void ( *callback )( void* );      // �R�[���o�b�N�֐�
  void*            callback_work;   // �R�[���o�b�N�֐��̈���

} SUBPROC_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent_Callback(GMEVENT * event, int * seq, void * work)
{
	SUBPROC_WORK * spw = work;
	GAMESYS_WORK *gsys = spw->gsys;

	switch(*seq) 
  {
	case 0:
    { // �T�u�v���Z�X�Ăяo���C�x���g
      GMEVENT* ev_sub;
      ev_sub = EVENT_FieldSubProc( spw->gsys, spw->fieldmap, 
                                  spw->ov_id, spw->proc_data, spw->proc_work );
      GMEVENT_CallEvent( event, ev_sub );
    }
		(*seq) ++;
		break;
	case 1: // �R�[���o�b�N�֐��Ăяo��
    if( spw->callback ) spw->callback( spw->callback_work );
		(*seq) ++;
    break;
  case 2:  // �R�[���o�b�N���[�N�j��
    if( spw->callback_work ){ GFL_HEAP_FreeMemory( spw->callback_work ); }
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
} 
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEventNoFade_Callback(GMEVENT * event, int * seq, void * work)
{
	SUBPROC_WORK * spw = work;
	GAMESYS_WORK *gsys = spw->gsys;

	switch(*seq) 
  {
	case 0:
    { // �T�u�v���Z�X�Ăяo���C�x���g
      GMEVENT* ev_sub;
      ev_sub = EVENT_FieldSubProcNoFade( spw->gsys, spw->fieldmap, 
                                        spw->ov_id, spw->proc_data, spw->proc_work );
      GMEVENT_CallEvent( event, ev_sub );
    }
		(*seq) ++;
		break;
	case 1: // �R�[���o�b�N�֐��Ăяo��
    if( spw->callback ) spw->callback( spw->callback_work );
		(*seq) ++;
    break;
  case 2:  // �R�[���o�b�N���[�N�j��
    if( spw->callback_work ){ GFL_HEAP_FreeMemory( spw->callback_work ); }
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����(�R�[���o�b�N�֐��t��)
 * @param	gsys		      GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	    �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		      �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	    �J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	    �J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @param callback      �R�[���o�b�N�֐�
 * @param callback_work �R�[���o�b�N�֐��ɂ킽���|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
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

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����(�t�F�[�h�Ȃ�, �R�[���o�b�N�֐��t��)
 * @param	gsys		      GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	    �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		      �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	    �J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	    �J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @param callback      �R�[���o�b�N�֐�
 * @param callback_work �R�[���o�b�N�֐��ɂ킽���|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldSubProcNoFade_Callback(
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work,
    void (*callback)(void*), void* callback_work )
{
	GMEVENT*     event = GMEVENT_Create(gsys, NULL, GameChangeEventNoFade_Callback, sizeof(SUBPROC_WORK));
	SUBPROC_WORK*  spw = GMEVENT_GetEventWork(event);
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
