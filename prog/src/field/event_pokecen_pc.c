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


//=========================================================================================
// ���C�x���g���[�N
//=========================================================================================
typedef struct
{
  GAMESYS_WORK* gsys;
  FIELDMAP_WORK* fieldmap;
  G3DMAPOBJST* pcStatus;

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
      G3DMAPOBJST** objst;
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
      objst  = FIELD_BMODEL_MAN_CreateObjStatusList( man, &rect, BM_SEARCH_ID_PC, &objnum );
      // �A�j���Đ�
      if( objst )
      { 
        work->pcStatus = objst[0];
        G3DMAPOBJST_setAnime( man, objst[0], 0, BMANM_REQ_START );
        GFL_HEAP_FreeMemory( objst );
      }
    }
    ++(*seq);
    break;
  // �A�j��, SE�҂�
  case 1:
    if( PMSND_CheckPlaySE() == FALSE )
    {
      ++(*seq);
    }
    break;
  // PC�N�����A�j���Đ�
  case 2:
    if( work->pcStatus )
    {
      FLDMAPPER* mapper;
      FIELD_BMODEL_MAN* man;
      mapper = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
      man    = FLDMAPPER_GetBuildModelManager( mapper );
      G3DMAPOBJST_setAnime( man, work->pcStatus, 1, BMANM_REQ_START );
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
    PMSND_PlaySE( SEQ_SE_PC_LOGOFF );
    {
      FIELD_PLAYER* player;
      VecFx32 pos;
      G3DMAPOBJST** objst;
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
      objst  = FIELD_BMODEL_MAN_CreateObjStatusList( man, &rect, BM_SEARCH_ID_PC, &objnum );
      // �A�j���Đ�
      if( objst )
      { 
        work->pcStatus = objst[0];
        G3DMAPOBJST_setAnime( man, objst[0], 2, BMANM_REQ_START );
        GFL_HEAP_FreeMemory( objst );
      }
    }
    ++(*seq);
    break;
  // �A�j��, SE�҂�
  case 1:
    if( PMSND_CheckPlaySE() == FALSE )
    {
      ++(*seq);
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
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------
GMEVENT* EVENT_PokecenPcOff( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
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
  return event;
} 
