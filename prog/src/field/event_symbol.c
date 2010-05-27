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
#include "field/event_fieldmap_control.h" //EVENT_FieldSubProc
#include "event_symbol.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "app/pdc.h"
#include "event_pdc_return.h"
#include "poke_tool/poke_tool.h"
#include "sound/pm_sndsys.h"
#include "field_comm/intrude_work.h"
#include "event_mapchange.h"

#include "field/field_const.h"
#include "symbol_map.h"
#include "item/itemsym.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�V���{���|�P�����p�o�g��BGM
#define SND_SYMBOL_BATTLE_BGM   (SEQ_BGM_VS_NORAPOKE)
///�V���{���|�P�����p�G���J�E���g�G�t�F�N�gID
#define SYMBOL_ENCEFF_ID        (ENCEFFID_WILD_NORMAL)


//==============================================================================
//  �\���̒�`
//==============================================================================
//--------------------------------------------------------------
/// �V���{���G���J�E���g�F�o�g���C�x���g����p���[�N��`
//--------------------------------------------------------------
typedef struct{
  FIELDMAP_WORK *fieldWork;
  PDC_SETUP_PARAM *pdc_setup;
  PDCRET_PARAM *pdcret;
  POKEMON_PARAM *pp;
  BTL_FIELD_SITUATION bfs;
  SYMBOL_POKEMON sympoke;
  HEAPID heap_id;
  u16 *result_ptr;    ///<�߂܂������ǂ�����Ԃ����߂̃|�C���^
  
  u16 pp_crc;    ///<�s���`�F�b�N�p��CRC
  u32 wrong_first;
  u32 wrong_second;
  u32 wrong_therd;
}EVENT_SYMBOL_BATTLE;

//--------------------------------------------------------------
///   �V���{���}�b�v�F�|�P�����f�[�^�̒ʐM�擾�C�x���g���䃏�[�N
//--------------------------------------------------------------
typedef struct{
  u16 *result_ptr;      ///<�擾���ʁi�������ǂ����j��Ԃ����߂̃|�C���^
  u8 symbol_map_id;     ///<���N�G�X�g����V���{���}�b�vID
  u8 padding[2];
}EVENT_REQ_SYMBOL_PARAM;

//--------------------------------------------------------------
/// �V���{���}�b�v�F�p���X�̐X�}�b�v�J�ڃC�x���g���䃏�[�N
//--------------------------------------------------------------
typedef struct{
  u16 *result_ptr;        ///<�J�ڂł������ǂ����̌��ʂ�Ԃ����߂̃|�C���^
  u16 warp_zone_id;       ///<�J�ڐ�]�[��ID
  VecFx32 warp_pos;       ///<�J�ڐ�ł̃}�b�v�ʒu
  u16 warp_dir;           ///<�J�ڐ�ł̌���
  u8 symbol_map_id;       ///<���N�G�X�g����V���{���}�b�vID
  u16 data_recv_result;   ///<���ʃC�x���g����̌��ʂ��󂯎�邽�߂̃��[�N
  BOOL se_request;        ///<�}�b�v�؂�ւ�SE��炷���ǂ���
  BOOL my_palace;         ///<TRUE:�����̃p���X�ɂ���
}EVENT_SYMBOL_MAP_WARP;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT EventSymbolPokeBattle( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT EventReqIntrudeSymbolParam( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT EventSymbolMapWarp( GMEVENT *event, int *seq, void *wk );



//--------------------------------------------------------------
/**
 * �s���`�F�b�N�FSYMBOL_POKEMON��Ώۂɕs���`�F�b�N
 *
 * @param   heap_id		
 * @param   gamedata		
 * @param   sympoke		
 *
 * @retval  inline BOOL		TRUE:����
 */
//--------------------------------------------------------------
static inline BOOL _CheckWrongSympoke(HEAPID heap_id, GAMEDATA *gamedata, const SYMBOL_POKEMON *sympoke)
{
  BOOL ret;
  ret = SymbolSave_CheckFlashLoad(GAMEDATA_GetSaveControlWork(gamedata), sympoke, heap_id, FALSE);
  if(!ret){
    OS_TPrintf("symbol wrong!!\n");
  }
  return ret;
}

//--------------------------------------------------------------
/**
 * �s���`�F�b�N�FPOKEMON_PARAM��Ώۂɕs���`�F�b�N
 *
 * @param   heap_id		
 * @param   gamedata		
 * @param   pp		
 *
 * @retval  inline BOOL		
 */
//--------------------------------------------------------------
static inline BOOL _CheckWrongPokePara(HEAPID heap_id, GAMEDATA *gamedata, POKEMON_PARAM *pp)
{
  SYMBOL_POKEMON spoke;
  BOOL ret;
  
  spoke.monsno = PP_Get( pp, ID_PARA_monsno, NULL);
  spoke.form_no = PP_Get( pp, ID_PARA_form_no, NULL);
  ret = SymbolSave_CheckFlashLoad(GAMEDATA_GetSaveControlWork(gamedata), &spoke, heap_id, TRUE);
  if(!ret){
    OS_TPrintf("symbol pp wrong!!\n");
  }
  return ret;
}




//==================================================================
/**
 * �V���{���|�P�����F�o�g��
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   spoke		      �o��������V���{���|�P�����f�[�^(�s���`�F�b�N�p�ɎQ�Ƃ̂�)
 * @param   result_ptr		���ʑ����(TRUE:�V���{���|�P������ߊl�����B�@FALSE:�ߊl���Ȃ�����)
 * @param   heap_id		    
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_SymbolPokeBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, POKEMON_PARAM *pp, SYMBOL_POKEMON sympoke, u16 *result_ptr, HEAPID heap_id)
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
  
  esb->pp = pp;
  esb->sympoke = sympoke;
  esb->pp_crc = GFL_STD_CrcCalc( pp, POKETOOL_GetWorkSize() );
  if(!_CheckWrongSympoke(heap_id, gamedata, &sympoke)){
    esb->sympoke.monsno = MONSNO_MAX; //�ł���߂ɂȂ�p�����[�^���Z�b�g
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
    SEQ_BGMPOP,
    SEQ_FIELD_OPEN,
    //SEQ_FADEIN,
  };
	
	switch( *seq ){
  case SEQ_BATTLE_BGM:
    // �퓬�p�a�f�l�Z�b�g
    GMEVENT_CallEvent(event, EVENT_FSND_PushPlayBattleBGM(gsys, SND_SYMBOL_BATTLE_BGM));
    (*seq)++;
    break;
  case SEQ_ENCEFF:
    if(!_CheckWrongPokePara(esb->heap_id, gamedata, esb->pp)){
      esb->sympoke.form_no = 76; //�ł���߂ɂȂ�p�����[�^���Z�b�g
    }
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
      SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);

      //�h���[���{�[�����o�b�O�ɉ�����
      MYITEM_AddItem( GAMEDATA_GetMyItem(gamedata), ITEM_DORIIMUBOORU, 1, esb->heap_id );
      
      GFL_OVERLAY_Load( FS_OVERLAY_ID(pdc) );
      esb->pdc_setup = PDC_MakeSetUpParam( gamedata, esb->pp, &esb->bfs, 
        GAMEDATA_GetMyStatus(gamedata), GAMEDATA_GetMyItem(gamedata), 
        SaveData_GetConfig( sv ), ZUKAN_SAVEDATA_GetZukanSave(sv), esb->heap_id );
      GMEVENT_CallProc( event, NO_OVERLAY_ID, &PDC_ProcData, esb->pdc_setup );
    }
		(*seq) ++;
		break;
  case SEQ_RESULT:
    {
      //PDC_RESULT�̌��ʎ擾
      PDC_RESULT pdc_result = PDC_GetResult( esb->pdc_setup );

      if(_CheckWrongPokePara(esb->heap_id, gamedata, esb->pp) == FALSE
          || _CheckWrongSympoke(esb->heap_id, gamedata, &esb->sympoke) == FALSE){
        pdc_result = FALSE;
      }

      if(pdc_result == PDC_RESULT_CAPTURE){
        *(esb->result_ptr) = TRUE;
      }
      else{
        *(esb->result_ptr) = FALSE;
      }
      GFL_OVERLAY_Unload( FS_OVERLAY_ID(pdc) );
      { // �T�u�v���Z�X�Ăяo���C�x���g
        GFL_OVERLAY_Load( FS_OVERLAY_ID(pdc_return) );
        esb->pdcret = PDCRET_AllocParam( gamedata, pdc_result, esb->pp, esb->heap_id );
        GMEVENT_CallProc( event, NO_OVERLAY_ID, &PDCRET_ProcData, esb->pdcret );
      }
      
      //�h���[���{�[�����A�C�e������O��
      MYITEM_SubItem( GAMEDATA_GetMyItem(gamedata), ITEM_DORIIMUBOORU, 1, esb->heap_id );
    }
    (*seq)++;
    break;
	case SEQ_BGMPOP:
    {
      PDCRET_FreeParam( esb->pdcret );
      GFL_OVERLAY_Unload( FS_OVERLAY_ID(pdc_return) );
    }
	  // �t�B�[���hBGM���A
    GMEVENT_CallEvent(event, EVENT_FSND_PopPlayBGM_fromBattle(gsys));
    (*seq)++;
    break;
  case SEQ_FIELD_OPEN:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq)++;
    break;
#if 0
    �t�F�[�h�C�������̓X�N���v�g���ł����Ȃ�
  case SEQ_FADEIN:
    { // �t�F�[�h�C��
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, esb->fieldWork, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
		(*seq) ++;
		break;
#endif

  default:
    GFL_HEAP_FreeMemory(esb->pp);
    GFL_HEAP_FreeMemory(esb->pdc_setup);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}



//==============================================================================
//
//  �N����̃V���{���f�[�^��v�����A��M
//
//==============================================================================
//==================================================================
/**
 * �N����̃V���{���f�[�^��v�����A��M����܂ł̏������C�x���g�Ƃ��čs���܂�
 *
 * @param   gsys		
 * @param   result_ptr		���ʑ����(TRUE:����Ƀf�[�^��M�B�@FALSE:��M�o���Ȃ�����(�ʐM�G���[))
 * @param   symbol_map_id	����֗v������V���{���}�b�vID
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_ReqIntrudeSymbolParam(GAMESYS_WORK *gsys, u16 *result_ptr, SYMBOL_MAP_ID symbol_map_id)
{
	GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  EVENT_REQ_SYMBOL_PARAM *ersp;
	GMEVENT *event = NULL;
	
 	event = GMEVENT_Create(
    		gsys, NULL,	EventReqIntrudeSymbolParam, sizeof(EVENT_REQ_SYMBOL_PARAM) );
  
	ersp = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( ersp, sizeof(EVENT_REQ_SYMBOL_PARAM) );

  ersp->result_ptr = result_ptr;
  ersp->symbol_map_id = symbol_map_id;

	return( event );
}

//--------------------------------------------------------------
/**
 * �N����̃V���{���f�[�^��v�����A��M����܂ł̏������C�x���g�Ƃ��čs���܂�
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		
 */
//--------------------------------------------------------------
static GMEVENT_RESULT EventReqIntrudeSymbolParam( GMEVENT *event, int *seq, void *wk )
{
  EVENT_REQ_SYMBOL_PARAM *ersp = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	
	intcomm = Intrude_Check_CommConnect(game_comm);
	if(intcomm == NULL){
    if(ersp->result_ptr != NULL){
      *(ersp->result_ptr) = FALSE;
    }
    return GMEVENT_RES_FINISH;
  }

	switch( *seq ){
	case 0:
	  if(IntrudeSymbol_ReqSymbolData(intcomm, ersp->symbol_map_id) == TRUE){
      (*seq)++;
    }
    break;
  case 1:
    if(IntrudeSymbol_CheckRecvSymbolData(intcomm) == TRUE){
      (*seq)++;
    }
    break;
  case 2:
    if(ersp->result_ptr != NULL){
      *(ersp->result_ptr) = TRUE;
    }
    return GMEVENT_RES_FINISH;
  }
  
	return GMEVENT_RES_CONTINUE;
}




//==============================================================================
//
//  �V���{���}�b�v�J��
//
//==============================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static void getNewPos( const VecFx32 * now_pos, u16 dir, VecFx32 * next_pos )
{
  *next_pos = *now_pos;
  switch ( dir )
  {
  case DIR_UP:
    next_pos->z = (24 * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_HALF_SIZE ) << FX32_SHIFT;
    break;

  case DIR_DOWN:
    next_pos->z = (9 * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_HALF_SIZE ) << FX32_SHIFT;
    break;

  case DIR_LEFT:
    next_pos->x = (26 * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_HALF_SIZE ) << FX32_SHIFT;
    break;

  case DIR_RIGHT:
    next_pos->x = (5 * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_HALF_SIZE ) << FX32_SHIFT;
    break;

  case DIR_NOT:
    //next_pos->x = (15 * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_HALF_SIZE ) << FX32_SHIFT;
    //next_pos->z = (24 * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_HALF_SIZE ) << FX32_SHIFT;
    break;
  }
}
//==================================================================
/**
 * �V���{���}�b�v�J��
 *    �V���{���}�b�v�̃f�[�^�v�� �� �}�b�v�J��
 * �܂ł��C�x���g�Ƃ��čs���܂�
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   result_ptr		���ʑ����  TRUE:����Ƀf�[�^��M���ă}�b�v�J�ڂ����B
 *                                    FALSE:��M�o���Ȃ������׃}�b�v�J�ڂ����Ă��Ȃ�(�ʐM�G���[)
 * @param   warp_pos		
 * @param   warp_dir    
 * @param   symbol_map_id	����֗v������V���{���}�b�vID
 *
 * @retval  GMEVENT *		
 *
 * �ʐM�G���[�����������ꍇ�̓}�b�v�J�ڂ��s�킸�ɃC�x���g�I�����܂�
 */
//==================================================================
GMEVENT * EVENT_SymbolMapWarp(
    GAMESYS_WORK *gsys, BOOL se_request,
    const VecFx32 *warp_pos, u16 warp_dir, SYMBOL_MAP_ID symbol_map_id)
{
	GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  EVENT_SYMBOL_MAP_WARP *esmw;
	GMEVENT *event = NULL;
	
 	event = GMEVENT_Create(gsys, NULL,	EventSymbolMapWarp, sizeof(EVENT_SYMBOL_MAP_WARP));
  
	esmw = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( esmw, sizeof(EVENT_SYMBOL_MAP_WARP) );

  esmw->warp_pos = *warp_pos;
  esmw->warp_dir = warp_dir;
  esmw->result_ptr = NULL;
  esmw->symbol_map_id = symbol_map_id;
  esmw->se_request = se_request;
  if(IntrudeSymbol_CheckIntrudeNetID(GAMESYSTEM_GetGameCommSysPtr(gsys), gamedata) == INTRUDE_NETID_NULL){
    esmw->my_palace = TRUE;
  }

	return( event );
}

//==================================================================
//==================================================================
GMEVENT * EVENT_SymbolMapWarpEasy( GAMESYS_WORK * gsys, u16 warp_dir, SYMBOL_MAP_ID symbol_map_id )
{
  VecFx32 next_pos;
  BOOL se_request;
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  {
    FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
    MMDL * mmdl = FIELD_PLAYER_GetMMdl( fld_player );
    const VecFx32 * now_pos = MMDL_GetVectorPosAddress( mmdl );
    getNewPos( now_pos, warp_dir, &next_pos );
  }
  se_request = ( warp_dir != DIR_NOT );
  return EVENT_SymbolMapWarp( gsys, se_request, &next_pos, warp_dir, symbol_map_id );
}


//--------------------------------------------------------------
/**
 * �V���{���}�b�v�J��
 *    �V���{���}�b�v�̃f�[�^�v�� �� �}�b�v�J��
 * �܂ł��C�x���g�Ƃ��čs���܂�
 * 
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		
 * 
 * �ʐM�G���[�����������ꍇ�̓}�b�v�J�ڂ��s�킸�ɃC�x���g�I�����܂�
 */
//--------------------------------------------------------------
static GMEVENT_RESULT EventSymbolMapWarp( GMEVENT *event, int *seq, void *wk )
{
  EVENT_SYMBOL_MAP_WARP *esmw = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
	FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
	enum{
    _SEQ_INIT,
    _SEQ_REQ_WAIT,
    _SEQ_FADEOUT,
    _SEQ_CHANGE_MAP,
    _SEQ_FADEIN,
    _SEQ_FADEIN_WAIT,
    _SEQ_FINISH,
  };
  
	switch( *seq ){
	case _SEQ_INIT:
    MMDLSYS_PauseMoveProc( FIELDMAP_GetMMdlSys( fieldWork ) );
    
  	if(esmw->my_palace == TRUE){
      *seq = _SEQ_FADEOUT;
    }
    else{
  	  GMEVENT_CallEvent(event, 
  	    EVENT_ReqIntrudeSymbolParam(gsys, &esmw->data_recv_result, esmw->symbol_map_id));
      (*seq)++;
    }
    break;
  case _SEQ_REQ_WAIT:
    if(esmw->data_recv_result == FALSE){
      if(esmw->result_ptr != NULL){
        *(esmw->result_ptr) = FALSE;
      }
      (*seq) = _SEQ_FINISH;
      break;
    }
    (*seq)++;
    break;
  case _SEQ_FADEOUT:
    if (esmw->se_request)
    {
      PMSND_PlaySE( SEQ_SE_KAIDAN );
    }
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_PlayerDir( gsys, fieldWork ) );
    (*seq)++;
    break;
  case _SEQ_CHANGE_MAP:
    esmw->warp_zone_id = SYMBOLMAP_GetZoneID( gsys, esmw->symbol_map_id );
    GAMEDATA_SetSymbolMapID(gamedata, esmw->symbol_map_id);
    GMEVENT_CallEvent(event, 
      EVENT_ChangeMapPosNoFade(
        gsys, fieldWork, esmw->warp_zone_id, &esmw->warp_pos, esmw->warp_dir));
    (*seq)++;
    break;
  case _SEQ_FADEIN:
    {
      FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
      MMDLSYS * mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      if ( MMDL_BLACTCONT_IsThereReserve( mmdlsys ) == TRUE )
      {
        break;
      }
    }
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_PlayerDir( gsys, fieldWork ) );
    (*seq)++;
    break;
  case _SEQ_FADEIN_WAIT:
    if(esmw->result_ptr != NULL){
      *(esmw->result_ptr) = TRUE;
    }
    *seq = _SEQ_FINISH;
    break;
  case _SEQ_FINISH:
 	  MMDLSYS_ClearPauseMoveProc(FIELDMAP_GetMMdlSys(fieldWork));
    return GMEVENT_RES_FINISH;
  }
  
	return GMEVENT_RES_CONTINUE;
}

