///////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief ���O���̓C�x���g
 * @file  event_name_input.c
 * @author obata
 * @date   2009.10.20
 *
 */
///////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "event_name_input.h"
#include "app/name_input.h"
#include "event_fieldmap_control.h" // for EVENT_FieldSubProc
#include "system/main.h" // for HEAPID_PROC


//=====================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//=====================================================================================
static GMEVENT * EVENT_NameInput( 
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, NAMEIN_PARAM* namein_param );
static GMEVENT_RESULT EVENT_FUNC_NameInput(GMEVENT * event, int * seq, void * work); 
static GMEVENT_RESULT EVENT_FUNC_NameInput_PartyPoke(
    GMEVENT * event, int * seq, void * work );

//=====================================================================================
// �����O���͉�ʌĂяo���C�x���g
//===================================================================================== 

//-------------------------------------------------------------------------------------
/**
 * @breif �C�x���g���[�N
 */
//-------------------------------------------------------------------------------------
typedef struct 
{
	GAMESYS_WORK*        gsys;  // �Q�[���V�X�e�� 
	FIELDMAP_WORK*   fieldmap;  // �t�B�[���h�}�b�v
  NAMEIN_PARAM* nameInParam;  // ���O���͉�ʂ̃p�����[�^
} NAMEIN_WORK; 

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g(���O���͉�ʌĂяo��)
 * 
 * @param gsys        �Q�[���V�X�e��
 * @param ret_wk      ���͌��ʂ��󂯎�郏�[�N(TRUE:�m��, FALSE:�L�����Z��)
 * @param party_index ���O����͂���|�P�����̃p�[�e�B�[���C���f�b�N�X
 *
 * @return �C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT * EVENT_NameInput( 
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, NAMEIN_PARAM* namein_param )
{
	GMEVENT* event;
	NAMEIN_WORK* niw;

  // �C�x���g����
  event = GMEVENT_Create( gsys, NULL, EVENT_FUNC_NameInput, sizeof(NAMEIN_WORK) );
  niw   = GMEVENT_GetEventWork( event );
	niw->gsys        = gsys;
	niw->fieldmap    = fieldmap;
  niw->nameInParam = namein_param;
	return event;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�(���O���͉�ʌĂяo���C�x���g)
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_NameInput(GMEVENT * event, int * seq, void * work)
{
	NAMEIN_WORK*   niw = work;
	GAMESYS_WORK* gsys = niw->gsys;

	switch( *seq ) 
  {
	case 0: // ���O���͉�ʌĂяo��
    GMEVENT_CallEvent( 
        event, EVENT_FieldSubProc(niw->gsys, niw->fieldmap, 
          NO_OVERLAY_ID, &NameInputProcData, niw->nameInParam) );
    (*seq)++;
		break;
	case 1: // �C�x���g�I��
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}


//=====================================================================================
// ���莝���|�P���������͉�ʌĂяo���C�x���g
//===================================================================================== 

//-------------------------------------------------------------------------------------
/**
 * @breif �C�x���g���[�N
 */
//-------------------------------------------------------------------------------------
typedef struct 
{
	GAMESYS_WORK*        gsys;  // �Q�[���V�X�e�� 
	FIELDMAP_WORK*   fieldmap;  // �t�B�[���h�}�b�v
  NAMEIN_PARAM* nameInParam;  // ���O���͉�ʂ̃p�����[�^
  POKEMON_PARAM*  pokeParam;  // ���O���͑Ώۃ|�P����
  u16*              retWork;  // ���ʂ̊i�[�惏�[�N�ւ̃|�C���^
} PARTY_NAMEIN_WORK; 

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g(�莝���|�P���������͉�ʌĂяo��)
 * 
 * @param gsys        �Q�[���V�X�e��
 * @param ret_wk      ���͌��ʂ��󂯎�郏�[�N(TRUE:�m��, FALSE:�L�����Z��)
 * @param party_index ���O����͂���|�P�����̃p�[�e�B�[���C���f�b�N�X
 *
 * @return �C�x���g
 *          �|�P���������͉�ʂ��Ăяo��, �w�肵���莝���|�P��������ݒ肷��B
 *          ���O���͂��m�肳�ꂽ���ǂ������w�肵�����[�N�ɕԂ��B
 *          TRUE:  ���͂������O���m��
 *          FALSE: ���O���͂��L�����Z��
 */
//-------------------------------------------------------------------------------------
GMEVENT * EVENT_NameInput_PartyPoke( 
    GAMESYS_WORK * gsys, u16* ret_wk, u16 party_index )
{
  GMEVENT* event;
  PARTY_NAMEIN_WORK* work;

  // �C�x���g����
  event = GMEVENT_Create( 
      gsys, NULL, EVENT_FUNC_NameInput_PartyPoke, sizeof(PARTY_NAMEIN_WORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  work->retWork  = ret_wk;

  // �|�P���������͉�ʂ��쐬
  {
    GAMEDATA*   gdata = GAMESYSTEM_GetGameData( work->gsys );
    POKEPARTY*  party = GAMEDATA_GetMyPokemon( gdata );
    POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, party_index );
    u32        monsno = PP_Get( pp, ID_PARA_monsno, NULL );
    u32        formno = PP_Get( pp, ID_PARA_form_no, NULL );
    work->nameInParam = NAMEIN_AllocParam( 
        HEAPID_PROC, NAMEIN_POKEMON, monsno, formno, NAMEIN_POKEMON_LENGTH, NULL );
    work->pokeParam   = pp;
  }

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�(�莝���|�P���������͉�ʌĂяo���C�x���g)
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_NameInput_PartyPoke(
    GMEVENT * event, int * seq, void * work )
{
  PARTY_NAMEIN_WORK* pniw = work;

  switch( *seq )
  {
  case 0: //----------------------------------------------- ���ʃC�x���g�Ăяo��
    GMEVENT_CallEvent( 
        event, EVENT_NameInput(pniw->gsys, pniw->fieldmap, pniw->nameInParam) );
    ++(*seq);
    break;
  case 1: //----------------------------------------------- �C�x���g�I��
    // ���͂��ꂽ���O�𔽉f������
    if( pniw->nameInParam->cancel != TRUE ) 
    { 
      PP_Put( pniw->pokeParam, ID_PARA_nickname, (u32)pniw->nameInParam->strbuf );
    }
    // ���͌��ʂ����[�N�ɕԂ�
    *(pniw->retWork) = (pniw->nameInParam->cancel != TRUE);
    NAMEIN_FreeParam( pniw->nameInParam );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}
