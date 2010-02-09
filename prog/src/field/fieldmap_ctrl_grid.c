//======================================================================
/**
 * @file	fieldmap_ctrl_grid.c
 * @brief	�t�B�[���h�}�b�v�@�R���g���[���@�O���b�h����
 * @author	kagaya
 * @date	09.04.22
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field/zonedata.h"
#include "arc/fieldmap/zone_id.h"

#include "fldmmdl.h"
#include "fieldmap.h"

#include "fieldmap_ctrl_grid.h"

#include "arc/arc_def.h"
#include "arc/arc_def.h"
#include "arc/fieldmap/camera_scroll.naix"

#ifdef PM_DEBUG
//@todo 12���q�n�l�p�Ώ�
#include "../../../resource/fldmapdata/flagwork/flag_define.h"  //for SYS_FLAG_
#endif

//======================================================================
//	�͈͏��
//======================================================================
 



//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	�O���b�h�����@�V�[�P���X
//--------------------------------------------------------------
typedef enum
{
	GRIDPROC_SEQNO_INIT = 0,
	GRIDPROC_SEQNO_MAIN,
	GRIDPROC_SEQNO_MAX,
}GRIDPROC_SEQNO;

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
//	FIELDMAP_CTRL_GRID*
//--------------------------------------------------------------
struct _TAG_FIELDMAP_CTRL_GRID
{
	HEAPID heapID;
	FIELDMAP_WORK *fieldWork;
	
	u16 seq_proc;	
	u8 padding0;
	u8 padding1;

	BOOL jikiMovePause;
};

//======================================================================
//	proto
//======================================================================
static void mapCtrlGrid_Create(
	FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir );
static void mapCtrlGrid_Delete( FIELDMAP_WORK *fieldWork );
static void mapCtrlGrid_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos );
static const VecFx32 * mapCtrlGrid_GetCameraTarget( FIELDMAP_WORK *fieldWork );

static u16 grid_ChangeFourDir( u16 dir );


static BOOL gym_check( u16 zone_id );

//======================================================================
//	�t�B�[���h�}�b�v�@�O���b�h����
//======================================================================
//--------------------------------------------------------------
///	�}�b�v�R���g���[���@�O���b�h�ړ�
//--------------------------------------------------------------
const DEPEND_FUNCTIONS FieldMapCtrl_GridFunctions =
{
	FLDMAP_CTRLTYPE_GRID,
	mapCtrlGrid_Create,
	mapCtrlGrid_Main,
	mapCtrlGrid_Delete,
  mapCtrlGrid_GetCameraTarget,
};

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�O���b�h�����@������ 
 * @param	fieldWork	FIELDMAP_WORK
 * @param	pos	���@�����ʒu
 * @param	dir ���@��������
 * @retval	nothing
 */
//--------------------------------------------------------------
static void mapCtrlGrid_Create(
	FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir )
{
	HEAPID heapID;
	FIELDMAP_CTRL_GRID *gridWork;
	
	heapID = FIELDMAP_GetHeapID( fieldWork );
	gridWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELDMAP_CTRL_GRID) );
	
	gridWork->heapID = heapID;
	gridWork->fieldWork = fieldWork;
	
	{	//�r���{�[�h�ݒ�
#if 0 //�W�����g�p����
		VecFx32 scale = {
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
		};
		GFL_BBDACT_SYS *bbdActSys = FIELDMAP_GetBbdActSys( fieldWork );
		GFL_BBD_SetScale( GFL_BBDACT_GetBBDSystem(bbdActSys), &scale );
#endif
	}

#if 0
	{	//�}�b�v�`��I�t�Z�b�g
		VecFx32 offs = { -FX32_ONE*8, 0, FX32_ONE*8 };
		FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
		FLDMAPPER_SetDrawOffset( mapper, &offs );
	}
#endif

	{ //���@�쐬
		FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    MMDL *fmmdl = FIELD_PLAYER_GetMMdl( fld_player );
#if 0
    u16 dir4 = grid_ChangeFourDir( dir );
#else
   u16 dir4 = dir;
#endif
    MMDL_SetDirDisp( fmmdl, dir4 );
//    KAGAYA_Printf( "�킽���ꂽ���� %xH, %d\n", dir, dir4 );
		FIELD_PLAYER_SetUpGrid( fld_player, heapID );
    
    { //�J�������W�Z�b�g
      const VecFx32 *pos = MMDL_GetVectorPosAddress( fmmdl );
      FIELDMAP_SetNowPosTarget( fieldWork, pos );
    }
	}
  
	FIELDMAP_SetMapCtrlWork( fieldWork, gridWork );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�O���b�h�����@�I��
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void mapCtrlGrid_Delete( FIELDMAP_WORK *fieldWork )
{
	FIELDMAP_CTRL_GRID *gridWork;
	gridWork = FIELDMAP_GetMapCtrlWork( fieldWork );
	GFL_HEAP_FreeMemory( gridWork );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�O���b�h�����@���C��
 * @param	fieldWork FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void mapCtrlGrid_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos )
{
	FIELDMAP_CTRL_GRID *gridWork = FIELDMAP_GetMapCtrlWork( fieldWork );
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
	
  if( gridWork->jikiMovePause == FALSE )
	{	//���@�ړ�
		int key_trg = GFL_UI_KEY_GetTrg( );
		int key_cont = GFL_UI_KEY_GetCont( );


#ifdef PM_DEBUG
    //@todo 12���q�n�l�p�Ώ�
    {
      GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
      if ( !EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_RUNNINGSHOES) ){
        //B�{�^�����}�X�N
        if (key_trg & PAD_BUTTON_B) key_trg ^= PAD_BUTTON_B;
        if (key_cont & PAD_BUTTON_B) key_cont ^= PAD_BUTTON_B;
      }
    }
#endif  //PM_DEBUG

		FIELD_PLAYER_MoveGrid( fld_player, key_trg, key_cont );
	}
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�O���b�h�����@�J�����^�[�Q�b�g�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval VecFx32* �J�����^�[�Q�b�g
 */
//--------------------------------------------------------------
static const VecFx32 * mapCtrlGrid_GetCameraTarget( FIELDMAP_WORK *fieldWork )
{
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  const VecFx32 *pos = MMDL_GetVectorPosAddress( mmdl ); 
  return( pos );
}

//======================================================================
//	�O���b�h�����@�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * 360�x����->�S������
 * @param	dir	�����@0x10000�P��
 * @retval	u16 DIR_UP��
 */
//--------------------------------------------------------------
static u16 grid_ChangeFourDir( u16 dir )
{
	if( (dir>0x2000) && (dir<0x6000) ){
		dir = DIR_LEFT;
	}else if( (dir >= 0x6000) && (dir <= 0xa000) ){
		dir = DIR_DOWN;
	}else if( (dir > 0xa000)&&(dir < 0xe000) ){
		dir = DIR_RIGHT;
	}else{
		dir = DIR_UP;
	}
	return( dir );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�O���b�h�����@���@�����~
 * @param fieldMap FIELDMAP_WORK
 * @param flag TRUE=��~ FALSE=����
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELDMAP_CTRL_GRID_SetPlayerPause( FIELDMAP_WORK *fieldMap, BOOL flag )
{
  FIELDMAP_CTRL_GRID *gridWork = FIELDMAP_GetMapCtrlWork( fieldMap );
#ifdef PM_DEBUG
  {
    FLDMAP_CTRLTYPE type = FIELDMAP_GetMapControlType( fieldMap );
    GF_ASSERT( type == FLDMAP_CTRLTYPE_GRID );
  }
#endif
  gridWork->jikiMovePause = flag;
}




