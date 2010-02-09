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
#include "arc/fieldmap/zone_id.h"

#include "fieldmap.h"
#include "fldeff_shadow.h"
#include "field/zonedata.h"

#include "gamesystem/game_data.h"  //PLAYER_WORK

#include "fieldmap_ctrl_nogrid.h"

#include "fieldmap_ctrl_nogrid_work.h"

#include "field_rail.h"
#include "field_rail_func.h"



#ifdef PM_DEBUG

#endif  // 

//======================================================================
//	�͈͏��
//======================================================================

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================



//======================================================================
//	proto
//======================================================================
static void mapCtrlNoGrid_Create(
		FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir );
static void mapCtrlNoGrid_Delete( FIELDMAP_WORK *fieldWork );
static void mapCtrlNoGrid_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos );
static const VecFx32 * mapCtrlNoGrid_GetCameraTarget( FIELDMAP_WORK *fieldWork );

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
  mapCtrlNoGrid_GetCameraTarget,
};

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�m���O���b�h�����@������
 * @param	fieldWork	FIELDMAP_WORK
 * @param	pos	���@�����ʒu
 * @param	dir ���@��������
 */
//--------------------------------------------------------------
static void mapCtrlNoGrid_Create(
		FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir )
{
	FIELD_PLAYER *fld_player;
  FIELD_CAMERA * camera = FIELDMAP_GetFieldCamera(fieldWork);
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( fieldWork );
	FIELDMAP_CTRL_NOGRID_WORK *work;

  // ���[���}�b�v�p�v���C���[���[�N�쐬
  fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
	work = FIELDMAP_CTRL_NOGRID_WORK_Create( fieldWork, FIELDMAP_GetHeapID( fieldWork ) );
	FIELDMAP_SetMapCtrlWork( fieldWork, work );

  //�J�������W�Z�b�g
  {
    MMDL* mmdl = FIELD_PLAYER_GetMMdl( fld_player );
    FIELDMAP_SetNowPosTarget( fieldWork, MMDL_GetVectorPosAddress( mmdl ) );
  }

  // ���P�[�V�����̐ݒ�
  {
    PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork(
        GAMESYSTEM_GetGameData(FIELDMAP_GetGameSysWork(fieldWork)) );
    const RAIL_LOCATION * railLoc = PLAYERWORK_getRailPosition( player );
    // �v���C���[���[�N���烍�P�[�V������ݒ�
    FIELD_PLAYER_SetNoGridLocation( fld_player, railLoc );
    FIELD_PLAYER_GetNoGridPos( fld_player, pos );
  }
//
	
	FIELD_PLAYER_SetPos( fld_player, pos );
	FIELD_PLAYER_SetDir( fld_player, dir );
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
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( fieldWork );
	FIELDMAP_CTRL_NOGRID_WORK* work = FIELDMAP_GetMapCtrlWork( fieldWork );

  FIELDMAP_CTRL_NOGRID_WORK_Delete( work );
  
	FLDNOGRID_MAPPER_Release( p_mapper );
  FLDNOGRID_MAPPER_UnBindCameraWork( p_mapper );
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
	FIELDMAP_CTRL_NOGRID_WORK *work = FIELDMAP_GetMapCtrlWork( fieldWork );
  // �ʏ�}�b�v�͂��ꂾ���ł悢
  FIELDMAP_CTRL_NOGRID_WORK_Main( work );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�m���O���b�h�����@�J�����^�[�Q�b�g�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval VecFx32* �J�����^�[�Q�b�g
 */
//--------------------------------------------------------------
static const VecFx32 * mapCtrlNoGrid_GetCameraTarget( FIELDMAP_WORK *fieldWork )
{
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
  MMDL* mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  return MMDL_GetVectorPosAddress( mmdl );
}

//======================================================================
//======================================================================
//======================================================================
//
//    POINT��`
//
//======================================================================
