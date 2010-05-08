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
static GMEVENT * EVENT_NameInput( GAMESYS_WORK* gameSystem,
    FIELDMAP_WORK* fieldmap, NAMEIN_PARAM* namein_param, BOOL fade_flag );
static GMEVENT_RESULT EVENT_FUNC_NameInput( GMEVENT* event, int* seq, void* wk ); 
static GMEVENT_RESULT EVENT_FUNC_NameInput_PartyPoke(
    GMEVENT* event, int* seq, void* wk );


//=====================================================================================
// �����O���͉�ʌĂяo���C�x���g
//===================================================================================== 

//-------------------------------------------------------------------------------------
// ���C�x���g���[�N
//-------------------------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK*  gameSystem;
	FIELDMAP_WORK* fieldmap;
  NAMEIN_PARAM*  nameInParam; // ���O���͉�ʂ̃p�����[�^
  BOOL           fade_flag;   // �A�v���Ăяo���O��Ƀt�F�[�h�����������Ȃ����ǂ���
} NAMEIN_WORK; 


//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g ( ���O���͉�ʌĂяo�� )
 * 
 * @param gameSystem
 * @param ret_wk      ���͌��ʂ��󂯎�郏�[�N(TRUE:�m��, FALSE:�L�����Z��)
 * @param party_index ���O����͂���|�P�����̃p�[�e�B�[���C���f�b�N�X
 *
 * @return �C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT* EVENT_NameInput( 
    GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, NAMEIN_PARAM* namein_param, BOOL fade_flag )
{
	GMEVENT* event;
	NAMEIN_WORK* niw;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, EVENT_FUNC_NameInput, sizeof(NAMEIN_WORK) );

  // �C�x���g���[�N������
  niw = GMEVENT_GetEventWork( event );
	niw->gameSystem = gameSystem;
	niw->fieldmap = fieldmap;
  niw->nameInParam = namein_param;
  niw->fade_flag = fade_flag;
	return event;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�(���O���͉�ʌĂяo���C�x���g)
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_NameInput(GMEVENT * event, int * seq, void * wk)
{
	NAMEIN_WORK*   niw = wk;
	GAMESYS_WORK* gameSystem = niw->gameSystem;

	switch( *seq ) 
  {
	case 0: // ���O���͉�ʌĂяo��
    if (niw->fade_flag) {
      GMEVENT_CallEvent( 
          event, EVENT_FieldSubProc(niw->gameSystem, niw->fieldmap, 
            FS_OVERLAY_ID(namein), &NameInputProcData, niw->nameInParam) );
    } else {
      GMEVENT_CallEvent( 
          event, EVENT_FieldSubProcNoFade(niw->gameSystem, niw->fieldmap, 
            FS_OVERLAY_ID(namein), &NameInputProcData, niw->nameInParam) );
    }
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
// ���C�x���g���[�N
//-------------------------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK*  gameSystem;  // �Q�[���V�X�e�� 
	FIELDMAP_WORK* fieldmap;    // �t�B�[���h�}�b�v
  NAMEIN_PARAM*  nameInParam; // ���O���͉�ʂ̃p�����[�^
  POKEMON_PARAM* pokeParam;   // ���O���͑Ώۃ|�P����
  u16*           retWork;     // ���ʂ̊i�[�惏�[�N�ւ̃|�C���^
  BOOL           fade_flag;   // �A�v���Ăяo���O��̃t�F�[�h�L�薳��
} PARTY_NAMEIN_WORK; 

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g(�莝���|�P���������͉�ʌĂяo��)
 * 
 * @param gameSystem
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
GMEVENT* EVENT_NameInput_PartyPoke(
    GAMESYS_WORK* gameSystem, u16* ret_wk, u16 party_index, BOOL fade_flag )
{
  GMEVENT* event;
  PARTY_NAMEIN_WORK* work;

  // �C���f�b�N�X������
  {
    GAMEDATA*  gdata = GAMESYSTEM_GetGameData( gameSystem );
    POKEPARTY* party = GAMEDATA_GetMyPokemon( gdata );
    int  party_count = PokeParty_GetPokeCount( party );
    if( (party_index < 0) || (party_count <= party_index) ) {
      OS_Printf( "���O���̓R�}���h: �C���f�b�N�X�w��Ɍ�肪����܂��B\n" );
      GF_ASSERT(0);
      party_index = 0;
    }
  }

  // �C�x���g����
  event = GMEVENT_Create( 
      gameSystem, NULL, EVENT_FUNC_NameInput_PartyPoke, sizeof(PARTY_NAMEIN_WORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );
  work->retWork    = ret_wk;
  work->fade_flag  = fade_flag;

  // �|�P���������͉�ʂ��쐬
  {
    GAMEDATA*   gdata = GAMESYSTEM_GetGameData( work->gameSystem );
    POKEPARTY*  party = GAMEDATA_GetMyPokemon( gdata );
    POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, party_index );
    MISC        *misc = SaveData_GetMisc( GAMEDATA_GetSaveControlWork(gdata) );
    u32        monsno = PP_Get( pp, ID_PARA_monsno, NULL );
    u32        formno = PP_Get( pp, ID_PARA_form_no, NULL ) | ( PP_Get( pp, ID_PARA_sex, NULL ) << 8 );
    work->nameInParam = NAMEIN_AllocParam( 
        HEAPID_PROC, NAMEIN_POKEMON, monsno, formno, NAMEIN_POKEMON_LENGTH, NULL, misc );
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
static GMEVENT_RESULT EVENT_FUNC_NameInput_PartyPoke( GMEVENT* event, int* seq, void* wk )
{
  PARTY_NAMEIN_WORK* pniw = wk;

  switch( *seq )
  {
  case 0: //----------------------------------------------- ���ʃC�x���g�Ăяo��
    GMEVENT_CallEvent( event,
        EVENT_NameInput(pniw->gameSystem, pniw->fieldmap, pniw->nameInParam, pniw->fade_flag) );
    ++(*seq);
    break;
  case 1: //----------------------------------------------- �C�x���g�I��
    // ���͂��ꂽ���O�𔽉f������
    if( pniw->nameInParam->cancel != TRUE ) { 
      PP_Put( pniw->pokeParam, ID_PARA_nickname, (u32)pniw->nameInParam->strbuf );
    }
    // ���͌��ʂ����[�N�ɕԂ�
    *(pniw->retWork) = (pniw->nameInParam->cancel != TRUE);
    if( *(pniw->retWork) == TRUE){
      //���O����͂��Ă�����A���R�[�h�����Z����
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( pniw->gameSystem );
      RECORD * rec = GAMEDATA_GetRecordPtr( gdata );
      RECORD_Inc( rec, RECID_NICKNAME );
    }
    NAMEIN_FreeParam( pniw->nameInParam );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
