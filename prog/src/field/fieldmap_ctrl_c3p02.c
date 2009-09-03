//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fieldmap_ctrl_c3p02.c
 *	@brief  C3P02�p�}�b�v�R���g���[��
 *	@author tomoya takahashi	
 *	@date		2009.08.04
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "system/gfl_use.h"

#include "arc/fieldmap/field_rail_data.naix"
#include "arc/fieldmap/field_scenearea_data.naix"

#include "fieldmap.h"
#include "field_player_nogrid.h"
#include "fieldmap_ctrl_c3p02.h"
#include "fieldmap_ctrl_nogrid_work.h"

#include "field_rail.h"
#include "field/field_rail_loader.h"
#include "field/zonedata.h"

#include "fld_scenearea.h"
#include "fld_scenearea_loader.h"


#ifdef PM_DEBUG

#ifdef DEBUG_ONLY_FOR_lee_hyunjung
#define C3P02_DEBUG_CAMERA_NOT_MOVE // �J�����𓮂����Ȃ�
#endif //DEBUG_ONLY_FOR_lee_hyunjung

#endif  // 

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

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


static void mapCtrlC3P02_Create( FIELDMAP_WORK* p_fieldwork, VecFx32* p_p_pos, u16 dir );
static void mapCtrlC3P02_Delete( FIELDMAP_WORK* p_fieldwork );
static void mapCtrlC3P02_Main( FIELDMAP_WORK* p_fieldwork, VecFx32* p_p_pos );


//--------------------------------------------------------------
///	�}�b�v�R���g���[��
//--------------------------------------------------------------
const DEPEND_FUNCTIONS FieldMapCtrl_C3P02Functions =
{
	FLDMAP_CTRLTYPE_NOGRID,
	mapCtrlC3P02_Create,
	mapCtrlC3P02_Main,
	mapCtrlC3P02_Delete,
};





//-----------------------------------------------------------------------------
/**
 *        private�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 */
//-----------------------------------------------------------------------------
static void mapCtrlC3P02_Create( FIELDMAP_WORK* p_fieldwork, VecFx32* p_pos, u16 dir )
{
  u32 heapID;
	FIELD_PLAYER * p_fld_player = FIELDMAP_GetFieldPlayer( p_fieldwork );
  FIELD_CAMERA * p_camera = FIELDMAP_GetFieldCamera(p_fieldwork);
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_fieldwork );
  FIELDMAP_CTRL_NOGRID_WORK* p_wk;
  FIELD_PLAYER_NOGRID* p_player;

  // �m�[�O���b�h���ݒ�
  {
    FLDNOGRID_RESISTDATA resist;

    resist.railDataID = ZONEDATA_GetRailDataID( FIELDMAP_GetZoneID(p_fieldwork) );
    resist.areaDataID = NARC_field_scenearea_data_c03p02_dat;
    
    FLDNOGRID_MAPPER_ResistData( p_mapper, &resist, FIELDMAP_GetHeapID(p_fieldwork) );
  }
  

  heapID = FIELDMAP_GetHeapID( p_fieldwork );

  p_wk = FIELDMAP_CTRL_NOGRID_WORK_Create( p_fieldwork, heapID );
	FIELDMAP_SetMapCtrlWork( p_fieldwork, p_wk );

  p_player = FIELDMAP_CTRL_NOGRID_WORK_GetNogridPlayerWork( p_wk );
  FIELD_PLAYER_NOGRID_GetPos( p_player, p_pos );
  FIELD_PLAYER_SetPos( p_fld_player, p_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 */
//-----------------------------------------------------------------------------
static void mapCtrlC3P02_Delete( FIELDMAP_WORK* p_fieldwork )
{
	FIELDMAP_CTRL_NOGRID_WORK* p_wk = FIELDMAP_GetMapCtrlWork( p_fieldwork );
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_fieldwork );
  
  FIELDMAP_CTRL_NOGRID_WORK_Delete( p_wk );
  
  FLDNOGRID_MAPPER_Release( p_mapper );

	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C��
 */
//-----------------------------------------------------------------------------
static void mapCtrlC3P02_Main( FIELDMAP_WORK* p_fieldwork, VecFx32* p_pos )
{
	FIELDMAP_CTRL_NOGRID_WORK* p_wk = FIELDMAP_GetMapCtrlWork( p_fieldwork );
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( p_fieldwork ); 
	FIELD_PLAYER *p_fld_player = FIELDMAP_GetFieldPlayer( p_fieldwork );
  const FLD_SCENEAREA* cp_fldscenearea = FLDNOGRID_MAPPER_GetSceneAreaMan( p_mapper );
  PLAYER_WORK * p_player_data = GAMEDATA_GetMyPlayerWork(
      GAMESYSTEM_GetGameData(FIELDMAP_GetGameSysWork(p_fieldwork)) );
  BOOL auto_move = FALSE;
  u32 now_areaID;
  FIELD_PLAYER_NOGRID* p_player = FIELDMAP_CTRL_NOGRID_WORK_GetNogridPlayerWork( p_wk );
  FIELD_RAIL_WORK* p_railwork = FIELD_PLAYER_NOGRID_GetRailWork( p_player );

  // �G���A����A�������암���ƁA�ʏ퓮�암���𔻒�
  now_areaID = FLD_SCENEAREA_GetActiveArea(cp_fldscenearea);
  if( now_areaID != FLD_SCENEAREA_ACTIVE_NONE )
  {
    if( now_areaID < 5 )
    {
      // ���E�������H
      if( (FIELD_RAIL_WORK_GetActionKey( p_railwork ) == RAIL_KEY_RIGHT) ||
          (FIELD_RAIL_WORK_GetActionKey( p_railwork ) == RAIL_KEY_LEFT) )
      {
        auto_move = TRUE;
      }
    }
    else
    {
      
      // �㉺�������H
      if( (FIELD_RAIL_WORK_GetActionKey( p_railwork ) == RAIL_KEY_UP) ||
          (FIELD_RAIL_WORK_GetActionKey( p_railwork ) == RAIL_KEY_DOWN) )
      {
        auto_move = TRUE;
      }
    }
  }

  
  // �ړ������̐ݒ�
  // �ʏ퓮�암��
  if( auto_move == FALSE )
  {
    FIELDMAP_CTRL_NOGRID_WORK_Main( p_wk );
  }
  else
  {
    static const u32 sc_key[] = 
    {
      0,
      PAD_KEY_UP,
      PAD_KEY_RIGHT,
      PAD_KEY_DOWN,
      PAD_KEY_LEFT,
    };
    u32 last_action;
    
    last_action = FIELD_RAIL_WORK_GetActionKey( p_railwork );
    
    // �������암��
    FIELD_PLAYER_NOGRID_Move( p_player, sc_key[last_action], sc_key[last_action] );
  }

  FIELD_PLAYER_GetPos( p_fld_player, p_pos );
  PLAYERWORK_setPosition( p_player_data, p_pos );



#ifdef C3P02_DEBUG_CAMERA_NOT_MOVE
  {
    FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldwork );
    
    // �J�����𓮂����Ȃ��ŁA��l���Ɍ�����
    FLDNOGRID_MAPPER_DEBUG_SetRailCameraActive( p_mapper, FALSE );

    // �A���O�����[�h�ŁA��l����ǂ��悤�ɂ���B
    FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
  }
#endif

}



