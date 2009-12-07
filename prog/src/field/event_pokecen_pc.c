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


//=========================================================================================
// ���C�x���g���[�N
//=========================================================================================
typedef struct
{
  GAMESYS_WORK* gsys;
  FIELDMAP_WORK* fieldmap;

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
    ++(*seq);
    break;
  // �A�j��, SE�҂�
  case 1:
    ++(*seq);
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �|�P�Z����PC�N���C�x���g���쐬����
 *
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------
GMEVENT* EVENT_PokecenPcOn( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gsys, NULL, PcOnEvent, sizeof(EVENT_WORK) );

  // ������
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->fieldmap = fieldmap;
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
    ++(*seq);
    break;
  // �A�j��, SE�҂�
  case 1:
    ++(*seq);
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
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------
GMEVENT* EVENT_PokecenPcOff( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // ����
  event = GMEVENT_Create( gsys, NULL, PcOffEvent, sizeof(EVENT_WORK) );

  // ������
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->fieldmap = fieldmap;
  return event;
} 
