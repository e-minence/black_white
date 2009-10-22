///////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief �|�P�����n�̉�ʌĂяo���C�x���g
 * @file  event_poke_status.c
 * @author ariizumi
 * @date   2009.10.22
 *
 */
///////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "event_poke_status.h"
#include "app/pokelist.h"
#include "app/p_status.h"
#include "event_fieldmap_control.h" // for EVENT_FieldSubProc


//====================================================================
// ������J�֐�
//====================================================================
static GMEVENT_RESULT EVENT_FUNC_PokeSelect(GMEVENT * event, int * seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_WazaSelect(GMEVENT * event, int * seq, void * work);

FS_EXTERN_OVERLAY(poke_status);

//------------------------------------------------------------------
/**
 * @brief �|�P�����I�����[�N
 */
//------------------------------------------------------------------
typedef struct 
{
	GAMESYS_WORK*      gsys;  // �Q�[���V�X�e��
	FIELDMAP_WORK* fieldmap;  // �t�B�[���h�}�b�v
  PLIST_DATA*      plData;  // �|�P�������X�g
  u16*            retDecide;  // �I���������̃`�F�b�N
  u16*            retPos;     // �I���|�P�����ԍ��̊i�[�惏�[�N
}SELECT_POKE_WORK;

//------------------------------------------------------------------
/**
 * @brief �|�P�����I���C�x���g
 *
 * @param gsys      �Q�[���V�X�e��
 * @param fieldmap  �t�B�[���h�}�b�v
 * @param retDecide �I�����ʂ̊i�[�惏�[�N
 * @param retValue  �I���ʒu�̊i�[�惏�[�N
 */
//------------------------------------------------------------------
GMEVENT * EVENT_CreatePokeSelect( 
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, u16* retDecide , u16* retPos )
{
	GMEVENT* event;
	SELECT_POKE_WORK* psw;
  PLIST_DATA* pl_data;
  GAMEDATA*  gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY* party = GAMEDATA_GetMyPokemon( gdata );

  // �|�P�������X�g����
  pl_data    = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(PLIST_DATA) );
  pl_data->pp = party;
  pl_data->mode = PL_MODE_SET;
  pl_data->type = PL_TYPE_SINGLE;
  pl_data->ret_sel = 0;

  // �C�x���g����
  event = GMEVENT_Create(gsys, NULL, EVENT_FUNC_PokeSelect, sizeof(SELECT_POKE_WORK));
  psw   = GMEVENT_GetEventWork(event);
	psw->gsys      = gsys;
	psw->fieldmap  = fieldmap;
  psw->plData    = pl_data;
  psw->retDecide = retDecide;
  psw->retPos    = retPos;
	return event;
}

//------------------------------------------------------------------
/**
 * @brief �C�x���g����֐� (�|�P�����I���C�x���g)
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_PokeSelect(GMEVENT * event, int * seq, void * work)
{
	SELECT_POKE_WORK * psw = work;
	GAMESYS_WORK *gsys = psw->gsys;

  // �V�[�P���X�̒�`
  enum
  {
    SEQ_FADE_OUT,
    SEQ_FIELDMAP_CLOSE,
    SEQ_SELECT_POKEMON,
    SEQ_FIELDMAP_OPEN,
    SEQ_FADE_IN,
    SEQ_END,
  };

	switch( *seq ) 
  {
	case SEQ_FADE_OUT: //// �t�F�[�h�A�E�g
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, psw->fieldmap, 0));
    *seq = SEQ_FIELDMAP_CLOSE;
		break;
	case SEQ_FIELDMAP_CLOSE: //// �t�B�[���h�}�b�v�I��
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, psw->fieldmap));
    *seq = SEQ_SELECT_POKEMON;
		break;
  case SEQ_SELECT_POKEMON:
		GMEVENT_CallEvent(event, EVENT_PokeSelect(gsys, psw->fieldmap, psw->plData, NULL));
    *seq = SEQ_FIELDMAP_OPEN;
    break;
	case SEQ_FIELDMAP_OPEN: //// �t�B�[���h�}�b�v���A
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    *seq = SEQ_FADE_IN;
		break;
	case SEQ_FADE_IN: //// �t�F�[�h�C��
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, psw->fieldmap, 0));
    *seq = SEQ_END;
		break;
	case SEQ_END: //// �C�x���g�I��
    // �I�����ʂ����[�N�Ɋi�[
	  if( psw->plData->ret_sel <= PL_SEL_POS_POKE6 )
	  {
      *psw->retPos = psw->plData->ret_sel;
      *psw->retDecide = TRUE;
    }
    else
    {
      *psw->retPos = 0;
      *psw->retDecide = FALSE;
    }
    GFL_HEAP_FreeMemory( psw->plData );
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
} 



//------------------------------------------------------------------
/**
 * @brief �Z�I�����[�N
 */
//------------------------------------------------------------------
typedef struct 
{
	GAMESYS_WORK*      gsys;  // �Q�[���V�X�e��
	FIELDMAP_WORK* fieldmap;  // �t�B�[���h�}�b�v
  PSTATUS_DATA*   psData;  // �|�P�������X�g
  u16*            retDecide;  // �I���������̃`�F�b�N
  u16*            retPos;     // �Z�ʒu�̊i�[�惏�[�N
}SELECT_WAZA_WORK;

//------------------------------------------------------------------
/**
 * @brief �|�P�����I���C�x���g
 *
 * @param gsys      �Q�[���V�X�e��
 * @param fieldmap  �t�B�[���h�}�b�v
 * @param pokePos   �|�P�����̈ʒu
 * @param retDecide �I�����ʂ̊i�[�惏�[�N
 * @param retPos    �I���ʒu�̊i�[�惏�[�N
 */
//------------------------------------------------------------------
GMEVENT * EVENT_CreateWazaSelect( 
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, u16 pokePos , u16* retDecide , u16* retPos )
{
	GMEVENT* event;
	SELECT_WAZA_WORK* wsw;
  PSTATUS_DATA* ps_data;
  GAMEDATA*  gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY* party = GAMEDATA_GetMyPokemon( gdata );

  // �|�P�������X�g����
  ps_data    = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(PLIST_DATA) );
  ps_data->ppd = party;
  ps_data->ppt = PST_PP_TYPE_POKEPARTY;
  ps_data->mode = PST_MODE_WAZAADD;
  ps_data->max = PokeParty_GetPokeCount( party );
  ps_data->pos = pokePos;
  ps_data->waza = 0;
  ps_data->canExitButton = FALSE;

  // �C�x���g����
  event = GMEVENT_Create(gsys, NULL, EVENT_FUNC_WazaSelect, sizeof(SELECT_WAZA_WORK));
  wsw   = GMEVENT_GetEventWork(event);
	wsw->gsys      = gsys;
	wsw->fieldmap  = fieldmap;
  wsw->psData    = ps_data;
  wsw->retDecide = retDecide;
  wsw->retPos    = retPos;
	return event;
}

//------------------------------------------------------------------
/**
 * @brief �C�x���g����֐� (�|�P�����I���C�x���g)
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_WazaSelect(GMEVENT * event, int * seq, void * work)
{
	SELECT_WAZA_WORK * wsw = work;
	GAMESYS_WORK *gsys = wsw->gsys;

  // �V�[�P���X�̒�`
  enum
  {
    SEQ_FADE_OUT,
    SEQ_FIELDMAP_CLOSE,
    SEQ_SELECT_WAZA,
    SEQ_FIELDMAP_OPEN,
    SEQ_FADE_IN,
    SEQ_END,
  };

	switch( *seq ) 
  {
	case SEQ_FADE_OUT: //// �t�F�[�h�A�E�g
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, wsw->fieldmap, 0));
    *seq = SEQ_FIELDMAP_CLOSE;
		break;
	case SEQ_FIELDMAP_CLOSE: //// �t�B�[���h�}�b�v�I��
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, wsw->fieldmap));
    *seq = SEQ_SELECT_WAZA;
		break;
  case SEQ_SELECT_WAZA:
		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(poke_status), &PokeStatus_ProcData, wsw->psData);
    *seq = SEQ_FIELDMAP_OPEN;
    break;
	case SEQ_FIELDMAP_OPEN: //// �t�B�[���h�}�b�v���A
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    *seq = SEQ_FADE_IN;
		break;
	case SEQ_FADE_IN: //// �t�F�[�h�C��
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, wsw->fieldmap, 0));
    *seq = SEQ_END;
		break;
	case SEQ_END: //// �C�x���g�I��
    // �I�����ʂ����[�N�Ɋi�[
	  if( wsw->psData->ret_mode == PST_RET_DECIDE )
	  {
      *wsw->retPos = wsw->psData->ret_sel;
      *wsw->retDecide = TRUE;
    }
    else
    {
      *wsw->retPos = 0;
      *wsw->retDecide = FALSE;
    }
    GFL_HEAP_FreeMemory( wsw->psData );
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
} 
