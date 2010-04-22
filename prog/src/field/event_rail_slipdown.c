//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_rail_slipdown.c
 *	@brief  �`�����s�I�����[�h��p�@���[���}�b�v�@���藎���C�x���g
 *	@author	tomoya takahashi
 *	@date		2009.10.16
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "fieldmap.h"
#include "event_rail_slipdown.h"

#include "arc/fieldmap/zone_id.h"
#include "field_nogrid_mapper.h"

#include "rail_slipdown.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	���藎���C�x���g���[�N
//=====================================
typedef struct {
  RAIL_SLIPDOWN_WORK* p_wk;
} EVENT_RAIL_SLIPDOWN_WORK;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// ���藎��
static GMEVENT_RESULT EVENT_RailSlipDownMain(GMEVENT * p_event, int *  p_seq, void * p_work);




//----------------------------------------------------------------------------
/**
 *	@brief  �`�����s�I���}�b�v�@���[���}�b�v�@���藎������
 *
 *	@param	gsys        �Q�[���V�X�e��
 *	@param	fieldmap    �t�B�[���h�}�b�v
 *
 *	@return �ݒ肵���C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_RailSlipDown(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, u16 dir)
{
  GMEVENT *event;
  EVENT_RAIL_SLIPDOWN_WORK* p_slipdown;

  event = GMEVENT_Create(
    gsys, NULL, EVENT_RailSlipDownMain, sizeof(EVENT_RAIL_SLIPDOWN_WORK));

  // �`�����s�I�����[�h���`�F�b�N
  GF_ASSERT( FIELDMAP_GetZoneID( fieldmap ) == ZONE_ID_D09 );

  // ���[���}�b�v���`�F�b�N
  GF_ASSERT( FIELDMAP_GetMapControlType( fieldmap ) == FLDMAP_CTRLTYPE_NOGRID );

  // ���[�N�̏�����
  p_slipdown = GMEVENT_GetEventWork(event);


  // OVERLAY
	GFL_OVERLAY_Load( FS_OVERLAY_ID(field_d09_slipdown) );

  // ���@
  {
    FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( fieldmap );
    MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_player );

    FIELD_PLAYER_SetDir( p_player, dir );
    
    p_slipdown->p_wk = RailSlipDown_Create( gsys, fieldmap, p_mmdl, TRUE );
  }

  
  return event;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �`�����s�I���}�b�v�@���[���}�b�v�@���藎������
 *
 *	@param	gsys        �Q�[���V�X�e��
 *	@param	fieldmap    �t�B�[���h�}�b�v
 *	@param  mmdl        ���ׂ�~�肳���郂�f��
 *
 *	@return �ݒ肵���C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_RailSlipDownObj(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, MMDL* mmdl)
{
  GMEVENT *event;
  EVENT_RAIL_SLIPDOWN_WORK* p_slipdown;

  event = GMEVENT_Create(
    gsys, NULL, EVENT_RailSlipDownMain, sizeof(EVENT_RAIL_SLIPDOWN_WORK));

  // �`�����s�I�����[�h���`�F�b�N
  GF_ASSERT( FIELDMAP_GetZoneID( fieldmap ) == ZONE_ID_D09 );

  // ���[���}�b�v���`�F�b�N
  GF_ASSERT( FIELDMAP_GetMapControlType( fieldmap ) == FLDMAP_CTRLTYPE_NOGRID );

  // ���[�N�̏�����
  p_slipdown = GMEVENT_GetEventWork(event);

  // OVERLAY
	GFL_OVERLAY_Load( FS_OVERLAY_ID(field_d09_slipdown) );

  p_slipdown->p_wk = RailSlipDown_Create( gsys, fieldmap, mmdl, FALSE );

  
  return event;
}




//-----------------------------------------------------------------------------
/**
 *      ���������������֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �X���b�v�C�x���g
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_RailSlipDownMain(GMEVENT * p_event, int *  p_seq, void * p_work)
{
  EVENT_RAIL_SLIPDOWN_WORK* p_slipdown;

  //���[�N�̏����� 
  p_slipdown = GMEVENT_GetEventWork(p_event);

  if( RailSlipDown_IsEnd( p_slipdown->p_wk ) ){

    RailSlipDown_Delete( p_slipdown->p_wk );

		GFL_OVERLAY_Unload( FS_OVERLAY_ID(field_d09_slipdown) );
    return GMEVENT_RES_FINISH;
  }
  

  return GMEVENT_RES_CONTINUE;
}
