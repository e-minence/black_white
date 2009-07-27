//======================================================================
/**
 * @file	fieldmap_ctrl_nogrid.c
 * @brief	�t�B�[���h�}�b�v�@�R���g���[���@�m���O���b�h����
 * @author	tamada
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "arc/fieldmap/field_rail_data.naix"

#include "fieldmap.h"
#include "field_player_nogrid.h"

#include "fieldmap_ctrl_nogrid.h"

#include "field_rail.h"
#include "field_rail_func.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================


static const RAIL_LOCATION locationStart = {
  FIELD_RAIL_TYPE_POINT,
  0,
};

//======================================================================
//	proto
//======================================================================
static void mapCtrlNoGrid_Create(
		FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir );
static void mapCtrlNoGrid_Delete( FIELDMAP_WORK *fieldWork );
static void mapCtrlNoGrid_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos );

//======================================================================
//	�t�B�[���h�}�b�v�@�m���O���b�h����
//======================================================================
//--------------------------------------------------------------
///	�}�b�v�R���g���[���@�O���b�h�ړ�
//--------------------------------------------------------------
const DEPEND_FUNCTIONS FieldMapCtrl_NoGridFunctions =
{
	FLDMAP_CTRLTYPE_NOGRID,
	mapCtrlNoGrid_Create,
	mapCtrlNoGrid_Main,
	mapCtrlNoGrid_Delete,
};

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�m���O���b�h�����@������
 * @param	fieldWork	FIELD_MAIN_WORK
 * @param	pos	���@�����ʒu
 * @param	dir ���@��������
 */
//--------------------------------------------------------------
static void mapCtrlNoGrid_Create(
		FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir )
{
	FIELD_PLAYER *fld_player;
  FIELD_CAMERA * camera = FIELDMAP_GetFieldCamera(fieldWork);
  FIELD_RAIL_MAN * railMan = FIELDMAP_GetFieldRailMan(fieldWork);
	FIELD_RAIL_LOADER* railLoader = FIELDMAP_GetFieldRailLoader(fieldWork);

	FIELD_RAIL_LOADER_Load( railLoader, NARC_field_rail_data_h01_dat, FIELDMAP_GetHeapID(fieldWork) );
  FIELD_RAIL_MAN_Load(railMan, FIELD_RAIL_LOADER_GetData(railLoader));
  FIELD_RAIL_MAN_SetLocation( railMan, &locationStart );
  //FIELD_RAIL_MAN_Load(railMan, &point_c03_start);
  FIELD_RAIL_MAN_GetPos(railMan, pos );
//  FIELD_CAMERA_BindNoCamera(camera, TRUE);

  fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
	FIELD_PLAYER_SetPos( fld_player, pos );
	FIELD_PLAYER_SetDir( fld_player, dir );

  FIELD_CAMERA_SetTargetPos( camera, pos );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�m���O���b�h�����@�폜
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void mapCtrlNoGrid_Delete( FIELDMAP_WORK *fieldWork )
{
	FIELD_RAIL_LOADER* railLoader = FIELDMAP_GetFieldRailLoader(fieldWork);
	FIELD_RAIL_LOADER_Clear( railLoader );
	//DeletePlayerAct( fieldWork->field_player );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�m���O���b�h�����@���C��
 * @param fieldWork FIELDMAP_WORK
 * @param pos
 * @retval nothing
 */
//--------------------------------------------------------------
static void mapCtrlNoGrid_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos )
{
	int key_cont = GFL_UI_KEY_GetCont();
  int key_trg = GFL_UI_KEY_GetTrg();
  FIELD_RAIL_MAN * railMan = FIELDMAP_GetFieldRailMan(fieldWork);
  BOOL rail_flag = FIELD_RAIL_MAN_GetActiveFlag(railMan);
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  //�f�o�b�O�̂��߁A���[��������ON/OFF
  if (key_trg & PAD_BUTTON_L)
  {
    FIELD_RAIL_MAN_SetActiveFlag(railMan, !rail_flag);
    if (!rail_flag)
    {
			FIELD_CAMERA_SetMode( FIELDMAP_GetFieldCamera(fieldWork), FIELD_CAMERA_MODE_DIRECT_POS );
//      FIELD_CAMERA_BindNoCamera(FIELDMAP_GetFieldCamera(fieldWork), TRUE);
  //    FIELD_CAMERA_SetCameraType(FIELDMAP_GetFieldCamera(fieldWork), FIELD_CAMERA_TYPE_H01);
    }
    else
    {
			FIELD_CAMERA_SetMode( FIELDMAP_GetFieldCamera(fieldWork), FIELD_CAMERA_MODE_CALC_CAMERA_POS );
//      FIELD_CAMERA_BindNoCamera(FIELDMAP_GetFieldCamera(fieldWork), FALSE);
   //   FIELD_CAMERA_SetCameraType(FIELDMAP_GetFieldCamera(fieldWork), FIELD_CAMERA_TYPE_NOUSE);
    }
  }

	FIELD_PLAYER_NOGRID_Move( fld_player, key_cont, FX32_ONE );
	FIELD_PLAYER_GetPos( fld_player, pos );

  if (rail_flag)
  {
    FIELD_RAIL_MAN_GetPos(railMan, pos );
    FIELD_PLAYER_SetPos( fld_player, pos );
    FIELD_CAMERA_SetTargetPos( FIELDMAP_GetFieldCamera(fieldWork), pos );
  }
  {
    GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
    PLAYERWORK_setPosition( player, pos );
	}
}


//======================================================================
//======================================================================
//======================================================================
//
//    POINT��`
//
//======================================================================
