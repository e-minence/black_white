///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �|�P�Z��PC�֘A�C�x���g
 * @file   event_pokecen_pc.c
 * @author obata
 * @date   2009.12.07
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "event_pokecen_pc.h"
#include "gamesystem/game_event.h"
#include "sound/pm_sndsys.h"
#include "field_player.h"
#include "field_buildmodel.h"
#include "field/field_const.h"  // for FIELD_CONST_GRID_SIZE
#include "fieldmap.h"
#include "script_def.h"


//=========================================================================================
// ���C�x���g���[�N
//=========================================================================================
typedef struct
{
  GAMESYS_WORK*  gsys;
  FIELDMAP_WORK* fieldmap;
  G3DMAPOBJST*   pcStatus;
  int            count;
  int            off_mode; // PC��~���@

} EVENT_WORK;


//=========================================================================================
// ��PC�N���C�x���g
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief PC�N���C�x���g����
 */
//-----------------------------------------------------------------------------------------
static GMEVENT_RESULT PcOnEvent( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  // �A�j��, SE�Đ�
  case 0:
    PMSND_PlaySE( SEQ_SE_PC_ON );
    {
      FIELD_PLAYER* player;
      VecFx32 pos;
      G3DMAPOBJST* objst;
      FLDHIT_RECT rect;
      u32 objnum;
      FLDMAPPER* mapper;
      FIELD_BMODEL_MAN* man;
      // �����͈͂�����
      player = FIELDMAP_GetFieldPlayer( work->fieldmap );
      FIELD_PLAYER_GetPos( player, &pos );
      rect.top    = pos.z - (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      rect.bottom = pos.z + (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      rect.left   = pos.x - (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      rect.right  = pos.x + (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      // PC������
      mapper = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
      man    = FLDMAPPER_GetBuildModelManager( mapper );
      objst  = FIELD_BMODEL_MAN_SearchObjStatusRect( man, BM_SEARCH_ID_PC, &rect );
      // �A�j���Đ�
      if( objst )
      { 
        work->pcStatus = objst;
        G3DMAPOBJST_setAnime( man, objst, 0, BMANM_REQ_START );
      }
    }
    ++(*seq);
    break;
  // �A�j��, SE�҂�
  case 1:
    {
      SEPLAYER_ID player_id;
      player_id = PMSND_GetSE_DefaultPlayerID( SEQ_SE_PC_ON );
      if( PMSND_CheckPlaySE_byPlayerID(player_id) == FALSE ){ ++(*seq); }
    }
    break;
  // PC�N�����A�j���Đ�
  case 2:
    PMSND_PlaySE( SEQ_SE_PC_LOGIN );
    if( work->pcStatus )
    {
      FLDMAPPER* mapper;
      FIELD_BMODEL_MAN* man;
      mapper = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
      man    = FLDMAPPER_GetBuildModelManager( mapper );
      G3DMAPOBJST_setAnime( man, work->pcStatus, 1, BMANM_REQ_LOOP );
    }
    ++(*seq);
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �|�P�Z����PC�N���C�x���g���쐬����
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------
GMEVENT* EVENT_PokecenPcOn( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gsys, parent, PcOnEvent, sizeof(EVENT_WORK) );

  // ������
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->fieldmap = fieldmap;
  work->pcStatus = NULL;
  return event;
} 


//=========================================================================================
// ��PC�N�����C�x���g
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief PC�N�����C�x���g����
 */
//-----------------------------------------------------------------------------------------
static GMEVENT_RESULT PcRunEvent( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  case 0:
    // PC�N�����A�j���Đ�
    {
      FIELD_PLAYER* player;
      VecFx32 pos;
      G3DMAPOBJST* objst;
      FLDHIT_RECT rect;
      u32 objnum;
      FLDMAPPER* mapper;
      FIELD_BMODEL_MAN* man;
      // �����͈͂�����
      player = FIELDMAP_GetFieldPlayer( work->fieldmap );
      FIELD_PLAYER_GetPos( player, &pos );
      rect.top    = pos.z - (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      rect.bottom = pos.z + (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      rect.left   = pos.x - (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      rect.right  = pos.x + (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      // PC������
      mapper = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
      man    = FLDMAPPER_GetBuildModelManager( mapper );
      objst  = FIELD_BMODEL_MAN_SearchObjStatusRect( man, BM_SEARCH_ID_PC, &rect );
      // �A�j���Đ�
      if( objst )
      { 
        work->pcStatus = objst;
        G3DMAPOBJST_setAnime( man, objst, 1, BMANM_REQ_LOOP );
      }
    }
    ++(*seq);
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �|�P�Z����PC�N�����C�x���g���쐬����
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------
GMEVENT* EVENT_PokecenPcRun( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gsys, parent, PcRunEvent, sizeof(EVENT_WORK) );

  // ������
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->fieldmap = fieldmap;
  work->pcStatus = NULL;
  return event;
} 


//=========================================================================================
// ��PC��~�C�x���g
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief PC��~�C�x���g����
 */
//-----------------------------------------------------------------------------------------
static GMEVENT_RESULT PcOffEvent( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  // �A�j��, SE�Đ�
  case 0:
    if( work->off_mode == SCR_PASOKON_OFF_NORMAL ) {
      PMSND_PlaySE( SEQ_SE_PC_LOGOFF );
    }
    {
      FIELD_PLAYER* player;
      VecFx32 pos;
      G3DMAPOBJST* objst;
      FLDHIT_RECT rect;
      u32 objnum;
      FLDMAPPER* mapper;
      FIELD_BMODEL_MAN* man;
      // �����͈͂�����
      player = FIELDMAP_GetFieldPlayer( work->fieldmap );
      FIELD_PLAYER_GetPos( player, &pos );
      rect.top    = pos.z - (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      rect.bottom = pos.z + (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      rect.left   = pos.x - (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      rect.right  = pos.x + (FIELD_CONST_GRID_SIZE << FX32_SHIFT);
      // PC������
      mapper = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
      man    = FLDMAPPER_GetBuildModelManager( mapper );
      objst  = FIELD_BMODEL_MAN_SearchObjStatusRect( man, BM_SEARCH_ID_PC, &rect );
      // �A�j���Đ�
      if( objst )
      { 
        work->pcStatus = objst;
        G3DMAPOBJST_setAnime( man, objst, 2, BMANM_REQ_START );
      }
    }
    ++(*seq);
    break;

  // �A�j��, SE�҂�
  case 1:
    if( work->off_mode == SCR_PASOKON_OFF_NORMAL ) {
      SEPLAYER_ID player_id;
      player_id = PMSND_GetSE_DefaultPlayerID( SEQ_SE_PC_LOGOFF );
      if( PMSND_CheckPlaySE_byPlayerID(player_id) == FALSE ){ ++(*seq); }
    }
    else {
      (*seq)++;
    }
    break;

  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �|�P�Z����PC��~�C�x���g���쐬����
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 * @param off_mode PC��~���@ ( SCR_PASOKON_OFF_xxxx )
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------
GMEVENT* EVENT_PokecenPcOff( 
    GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, int off_mode )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gsys, parent, PcOffEvent, sizeof(EVENT_WORK) );

  // ������
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->fieldmap = fieldmap;
  work->pcStatus = NULL;
  work->off_mode = off_mode;
  return event;
} 
