//==============================================================================
/**
 * @file    event_symbol.c
 * @brief   �N���F�V���{���G���J�E���g
 * @author  matsuda
 * @date    2010.03.16(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "message.naix"

#include "script.h"
#include "event_fieldtalk.h"

#include "print/wordset.h"
#include "event_comm_talk.h"
#include "field/event_fieldmap_control.h" //EVENT_FieldSubProc
#include "event_symbol.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "app/pdc.h"
#include "poke_tool/poke_tool.h"
#include "sound/pm_sndsys.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�V���{���|�P�����p�o�g��BGM
#define SND_SYMBOL_BATTLE_BGM   (SEQ_BGM_VS_NORAPOKE)
///�V���{���|�P�����p�G���J�E���g�G�t�F�N�gID
#define SYMBOL_ENCEFF_ID        (ENCEFFID_WILD_NORMAL)

///�V���{���|�P�����̃��x��
#define SYMBOL_POKE_LEVEL       (30)


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct{
  FIELDMAP_WORK *fieldWork;
  PDC_SETUP_PARAM *pdc_setup;
  POKEMON_PARAM *pp;
  BTL_FIELD_SITUATION bfs;
  HEAPID heap_id;
  u16 *result_ptr;
}EVENT_SYMBOL_BATTLE;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT EventSymbolPokeBattle( GMEVENT *event, int *seq, void *wk );



//==================================================================
/**
 * �V���{���|�P�����F�o�g��
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   spoke		      �o��������V���{���|�P�����f�[�^�ւ̃|�C���^
 * @param   result_ptr		���ʑ����(TRUE:�V���{���|�P������ߊl�����B�@FALSE:�ߊl���Ȃ�����)
 * @param   heap_id		    
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_SymbolPokeBattle(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const SYMBOL_POKEMON *spoke, u16 *result_ptr, HEAPID heap_id)
{
	GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  EVENT_SYMBOL_BATTLE *esb;
	GMEVENT *event = NULL;
	
 	event = GMEVENT_Create(
    		gsys, NULL,	EventSymbolPokeBattle, sizeof(EVENT_SYMBOL_BATTLE) );
  
	esb = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( esb, sizeof(EVENT_SYMBOL_BATTLE) );

  esb->fieldWork = fieldWork;
  esb->result_ptr = result_ptr;
  esb->heap_id = heap_id;
  
  { //�V���{���|�P������PokemonParam�Ƃ��Đ���
    u32 personal_rnd, id;
    
    id = MyStatus_GetID( GAMEDATA_GetMyStatus(gamedata) );
    
    esb->pp = PP_Create( spoke->monsno, SYMBOL_POKE_LEVEL, id, heap_id );
    personal_rnd = POKETOOL_CalcPersonalRandEx( 
      id, spoke->monsno, spoke->form_no, spoke->sex, 0, FALSE );
    PP_SetupEx( esb->pp, spoke->monsno, SYMBOL_POKE_LEVEL, id, PTL_SETUP_POW_AUTO, personal_rnd );
    PP_SetTokusei3( esb->pp, spoke->monsno, spoke->form_no );
  }

  BTL_FIELD_SITUATION_SetFromFieldStatus( &esb->bfs, gamedata, fieldWork );

	return( event );
}

//--------------------------------------------------------------
/**
 * �V���{���|�P�����F�o�g��
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		
 */
//--------------------------------------------------------------
static GMEVENT_RESULT EventSymbolPokeBattle( GMEVENT *event, int *seq, void *wk )
{
  EVENT_SYMBOL_BATTLE *esb = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
	enum{
    SEQ_BATTLE_BGM,
    SEQ_ENCEFF,
    SEQ_FADEOUT,
    SEQ_FIELD_CLOSE,
    SEQ_BATTLE,
    SEQ_RESULT,
    SEQ_FIELD_OPEN,
    SEQ_FADEIN,
    SEQ_BGMPOP,
  };
	
	switch( *seq ){
  case SEQ_BATTLE_BGM:
    // �퓬�p�a�f�l�Z�b�g
    GMEVENT_CallEvent(event, EVENT_FSND_PushPlayBattleBGM(gsys, SND_SYMBOL_BATTLE_BGM));
    (*seq)++;
    break;
  case SEQ_ENCEFF:
    ENCEFF_SetEncEff(FIELDMAP_GetEncEffCntPtr(esb->fieldWork), event, SYMBOL_ENCEFF_ID);
    (*seq)++;
    break;
  case SEQ_FADEOUT:
  #if 0 //�G���J�E���g�G�t�F�N�g�ł���Ă���̂ł���Ȃ�
    { // �t�F�[�h�A�E�g
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gsys, esb->fieldWork, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
  #endif
		(*seq) ++;
		break;
  case SEQ_FIELD_CLOSE:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, esb->fieldWork));
    (*seq)++;
    break;
	case SEQ_BATTLE:
    { // �T�u�v���Z�X�Ăяo���C�x���g
      GMEVENT* ev_sub;
      
      GFL_OVERLAY_Load( FS_OVERLAY_ID(pdc) );
      esb->pdc_setup = PDC_MakeSetUpParam( esb->pp, &esb->bfs, 
        GAMEDATA_GetMyStatus(gamedata), GAMEDATA_GetMyItem(gamedata), esb->heap_id );
      GMEVENT_CallProc( event, NO_OVERLAY_ID, &PDC_ProcData, esb->pdc_setup );
    }
		(*seq) ++;
		break;
  case SEQ_RESULT:
    //PDC_RESULT�̌��ʎ擾
    if(PDC_GetResult( esb->pdc_setup ) == PDC_RESULT_CAPTURE){
      *(esb->result_ptr) = TRUE;
    }
    else{
      *(esb->result_ptr) = FALSE;
    }
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(pdc) );
    (*seq)++;
    break;
  case SEQ_FIELD_OPEN:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq)++;
    break;
  case SEQ_FADEIN:
    { // �t�F�[�h�C��
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, esb->fieldWork, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
		(*seq) ++;
		break;
	case SEQ_BGMPOP:
	  // �t�B�[���hBGM���A
    GMEVENT_CallEvent(event, EVENT_FSND_PopPlayBGM_fromBattle(gsys));
    (*seq)++;
    break;

  default:
    GFL_HEAP_FreeMemory(esb->pp);
    GFL_HEAP_FreeMemory(esb->pdc_setup);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}
