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

#include "field_rail.h"
#include "field/field_rail_loader.h"
#include "field/zonedata.h"

#include "fld_scenearea.h"
#include "fld_scenearea_loader.h"

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
///	C3P02���샏�[�N
//=====================================
typedef struct
{
  FIELD_PLAYER_NOGRID_WORK player_work;

} C3P02_MOVE_WORK;

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
  FIELD_RAIL_MAN* p_railman = FIELDMAP_GetFieldRailMan(p_fieldwork);
	FIELD_RAIL_LOADER* p_rail_loader = FIELDMAP_GetFieldRailLoader(p_fieldwork);
  FLD_SCENEAREA* p_scenearea = FIELDMAP_GetFldSceneArea( p_fieldwork );
  FLD_SCENEAREA_LOADER* p_scenearea_loader = FIELDMAP_GetFldSceneAreaLoader( p_fieldwork );
  C3P02_MOVE_WORK* p_wk;

  heapID = FIELDMAP_GetHeapID( p_fieldwork );
  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(C3P02_MOVE_WORK) );
	FIELDMAP_SetMapCtrlWork( p_fieldwork, p_wk );

  
  // ���[���̋N��
	FIELD_RAIL_LOADER_Load( p_rail_loader, ZONEDATA_GetRailDataID( FIELDMAP_GetZoneID(p_fieldwork) ), heapID );
  FIELD_RAIL_MAN_Load(p_railman, FIELD_RAIL_LOADER_GetData(p_rail_loader));

  p_wk->player_work.railwork = FIELD_RAIL_MAN_CreateWork( p_railman );
  FIELD_RAIL_MAN_BindCamera( p_railman, p_wk->player_work.railwork );
  
  FIELD_RAIL_WORK_GetPos(p_wk->player_work.railwork, p_pos );

  // scenearea�N��
	FLD_SCENEAREA_LOADER_Load( p_scenearea_loader, NARC_field_scenearea_data_c03p02_dat, heapID );
  FLD_SCENEAREA_Load( p_scenearea, 
			FLD_SCENEAREA_LOADER_GetData(p_scenearea_loader),
			FLD_SCENEAREA_LOADER_GetDataNum(p_scenearea_loader),
			FLD_SCENEAREA_LOADER_GetFunc(p_scenearea_loader) );


  // �m�[�O���b�h�}�b�v�p�v���C���[�̏�����
  FIELD_PLAYER_NOGRID_Rail_SetUp( p_fld_player, &p_wk->player_work );
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 */
//-----------------------------------------------------------------------------
static void mapCtrlC3P02_Delete( FIELDMAP_WORK* p_fieldwork )
{
	C3P02_MOVE_WORK* p_wk = FIELDMAP_GetMapCtrlWork( p_fieldwork );
  FIELD_RAIL_MAN* p_railman = FIELDMAP_GetFieldRailMan( p_fieldwork );
	FIELD_RAIL_LOADER* p_rail_loader = FIELDMAP_GetFieldRailLoader(p_fieldwork);
  FLD_SCENEAREA* p_scenearea = FIELDMAP_GetFldSceneArea( p_fieldwork );
  FLD_SCENEAREA_LOADER* p_scenearea_loader = FIELDMAP_GetFldSceneAreaLoader( p_fieldwork );

  FLD_SCENEAREA_Release( p_scenearea );
	FLD_SCENEAREA_LOADER_Clear( p_scenearea_loader );


	FIELD_RAIL_LOADER_Clear( p_rail_loader );

  FIELD_RAIL_MAN_DeleteWork( p_railman, p_wk->player_work.railwork );

	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C��
 */
//-----------------------------------------------------------------------------
static void mapCtrlC3P02_Main( FIELDMAP_WORK* p_fieldwork, VecFx32* p_pos )
{
	C3P02_MOVE_WORK* p_wk = FIELDMAP_GetMapCtrlWork( p_fieldwork );
  FIELD_RAIL_MAN* p_railman = FIELDMAP_GetFieldRailMan( p_fieldwork );
	FIELD_PLAYER *p_fld_player = FIELDMAP_GetFieldPlayer( p_fieldwork );
  FLD_SCENEAREA* p_fldscenearea = FIELDMAP_GetFldSceneArea( p_fieldwork );
  PLAYER_WORK * p_player = GAMEDATA_GetMyPlayerWork(
      GAMESYSTEM_GetGameData(FIELDMAP_GetGameSysWork(p_fieldwork)) );
  BOOL auto_move = FALSE;
  u32 now_areaID;

  // �G���A����A�������암���ƁA�ʏ퓮�암���𔻒�
  now_areaID = FLD_SCENEAREA_GetActiveArea(p_fldscenearea);
  if( now_areaID != FLD_SCENEAREA_ACTIVE_NONE )
  {
    if( now_areaID < 5 )
    {
      // ���E�������H
      if( (FIELD_RAIL_WORK_GetActionKey( p_wk->player_work.railwork ) == RAIL_KEY_RIGHT) ||
          (FIELD_RAIL_WORK_GetActionKey( p_wk->player_work.railwork ) == RAIL_KEY_LEFT) )
      {
        auto_move = TRUE;
      }
    }
    else
    {
      
      // �㉺�������H
      if( (FIELD_RAIL_WORK_GetActionKey( p_wk->player_work.railwork ) == RAIL_KEY_UP) ||
          (FIELD_RAIL_WORK_GetActionKey( p_wk->player_work.railwork ) == RAIL_KEY_DOWN) )
      {
        auto_move = TRUE;
      }
    }
  }

  
  // �ړ������̐ݒ�
  // �ʏ퓮�암��
  if( auto_move == FALSE )
  {
    FIELD_PLAYER_NOGRID_Rail_Move( p_fld_player, FIELDMAP_GetFldEffCtrl(p_fieldwork), FIELDMAP_GetFieldCamera(p_fieldwork), GFL_UI_KEY_GetCont(), &p_wk->player_work );
  }
  else
  {
    // �������암��
    FIELD_RAIL_WORK_ForwardReq( p_wk->player_work.railwork, RAIL_FRAME_8, FIELD_RAIL_WORK_GetActionKey( p_wk->player_work.railwork ) );
    FIELD_RAIL_WORK_GetPos( p_wk->player_work.railwork, p_pos );
    FIELD_PLAYER_SetPos( p_fld_player, p_pos );
  }

  FIELD_PLAYER_GetPos( p_fld_player, p_pos );
  PLAYERWORK_setPosition( p_player, p_pos );


}



