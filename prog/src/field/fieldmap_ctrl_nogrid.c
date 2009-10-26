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
#include "arc/fieldmap/camera_scroll_original.naix"
#include "arc/fieldmap/zone_id.h"

#include "fieldmap.h"
#include "field_player_nogrid.h"
#include "fldeff_shadow.h"
#include "field/zonedata.h"

#include "gamesystem/game_data.h"  //PLAYER_WORK

#include "fieldmap_ctrl_nogrid.h"

#include "fieldmap_ctrl_nogrid_work.h"

#include "field_rail.h"
#include "field_rail_func.h"



#ifdef PM_DEBUG

#ifdef DEBUG_ONLY_FOR_lee_hyunjung
#define C3P02_DEBUG_CAMERA_NOT_MOVE // �J�����𓮂����Ȃ�
#endif //DEBUG_ONLY_FOR_lee_hyunjung

//@TODO �l������]���Ă��܂����߁A�A���O���|�C���^��n���Ȃ�
#define DEBUG_PEOPLE_ANGLE_CUT 

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
  FIELD_PLAYER_NOGRID* p_ngrid_player;

  // ���[���}�b�v�p�v���C���[���[�N�쐬
  fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
	work = FIELDMAP_CTRL_NOGRID_WORK_Create( fieldWork, FIELDMAP_GetHeapID( fieldWork ) );
	FIELDMAP_SetMapCtrlWork( fieldWork, work );
  p_ngrid_player = FIELDMAP_CTRL_NOGRID_WORK_GetNogridPlayerWork( work );

  //�J�������W�Z�b�g
  {
    MMDL* mmdl = FIELD_PLAYER_GetMMdl( fld_player );
    FIELDMAP_SetNowPosTarget( fieldWork, MMDL_GetVectorPosAddress( mmdl ) );
  }

  // ���P�[�V�����̐ݒ�
  FIELD_PLAYER_NOGRID_SetLocation( p_ngrid_player, &locationStart );
  FIELD_PLAYER_NOGRID_GetPos( p_ngrid_player, pos );
//
	
#ifdef DEBUG_PEOPLE_ANGLE_CUT
    MMDLSYS_SetJoinShadow( FIELDMAP_GetMMdlSys( fieldWork ), FALSE );
	  MMDLSYS_SetupDrawProc( FIELDMAP_GetMMdlSys( fieldWork ), NULL );
#endif

	FIELD_PLAYER_SetPos( fld_player, pos );
	FIELD_PLAYER_SetDir( fld_player, 0 );



  //  �J�����͈�
  if( FIELDMAP_GetZoneID( fieldWork ) == ZONE_ID_C03 )
  {
    FIELD_CAMERA_AREA * p_area = GFL_ARC_UTIL_Load( ARCID_CAMERA_ORG_SCRL, 
        NARC_camera_scroll_original_C03_bin,
        FALSE, FIELDMAP_GetHeapID(fieldWork) );

    FIELD_CAMERA_SetCameraArea( FIELDMAP_GetFieldCamera( fieldWork ), p_area );

    GFL_HEAP_FreeMemory( p_area );
  }

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

#ifdef DEBUG_PEOPLE_ANGLE_CUT
    MMDLSYS_SetJoinShadow( FIELDMAP_GetMMdlSys( fieldWork ), TRUE );
#endif

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
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( fieldWork );
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
	FIELDMAP_CTRL_NOGRID_WORK *work = FIELDMAP_GetMapCtrlWork( fieldWork );
  FIELD_PLAYER_NOGRID* p_ngrid_player = FIELDMAP_CTRL_NOGRID_WORK_GetNogridPlayerWork( work );

  PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork(
      GAMESYSTEM_GetGameData(FIELDMAP_GetGameSysWork(fieldWork)) );
  MMDL* mmdl = FIELD_PLAYER_GetMMdl( fld_player );

  if( FIELDMAP_GetZoneID( fieldWork ) != ZONE_ID_C03P02 )
  {
    // �ʏ�}�b�v�͂��ꂾ���ł悢
    FIELDMAP_CTRL_NOGRID_WORK_Main( work );
  }
  else
  {
    // C03P02�́A���ꏈ��
    u32 now_areaID;
    BOOL auto_move = FALSE;
    const FLD_SCENEAREA* cp_fldscenearea = FLDNOGRID_MAPPER_GetSceneAreaMan( p_mapper );
    FIELD_RAIL_WORK* p_railwork = FIELD_PLAYER_NOGRID_GetRailWork( p_ngrid_player );

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
      FIELDMAP_CTRL_NOGRID_WORK_Main( work );
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
      FIELD_PLAYER_NOGRID_Move( p_ngrid_player, sc_key[last_action], sc_key[last_action] );
    }


#ifdef C3P02_DEBUG_CAMERA_NOT_MOVE
    {
      FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( fieldWork );
      
      // �J�����𓮂����Ȃ��ŁA��l���Ɍ�����
      FLDNOGRID_MAPPER_SetRailCameraActive( p_mapper, FALSE );

      // �A���O�����[�h�ŁA��l����ǂ��悤�ɂ���B
      FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    }
#endif

  }

  
  MMDL_GetVectorPos( mmdl, pos );
//  PLAYERWORK_setPosition( player, pos );
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
