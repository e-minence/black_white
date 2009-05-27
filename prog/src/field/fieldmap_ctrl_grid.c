//======================================================================
/**
 * @file	fieldmap_ctrl_grid.c
 * @brief	�t�B�[���h�}�b�v�@�R���g���[���@�O���b�h����
 * @author	kagaya
 * @data	09.04.22
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fldmmdl.h"
#include "fieldmap.h"
#include "field_player_grid.h"

#include "fieldmap_ctrl_grid.h"

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
	
	FIELD_PLAYER_GRID *gridPlayer;
};

//======================================================================
//	proto
//======================================================================
static void mapCtrlGrid_Create(
	FIELDMAP_WORK *fieldWork, VecFx32 *pos, u16 dir );
static void mapCtrlGrid_Delete( FIELDMAP_WORK *fieldWork );
static void mapCtrlGrid_Main( FIELDMAP_WORK *fieldWork, VecFx32 *pos );

static u16 grid_ChangeFourDir( u16 dir );

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
		VecFx32 scale = {
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
			FX32_ONE+(FX32_ONE/2)+(FX32_ONE/4),
		};
		GFL_BBDACT_SYS *bbdActSys = FIELDMAP_GetBbdActSys( fieldWork );
		GFL_BBD_SetScale( GFL_BBDACT_GetBBDSystem(bbdActSys), &scale );
	}
	
	{	//�}�b�v�`��I�t�Z�b�g
		VecFx32 offs = { -FX32_ONE*8, 0, FX32_ONE*8 };
		FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
		FLDMAPPER_SetDrawOffset( mapper, &offs );
	}
	
	{ //���@�쐬
		FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( fld_player );
		u16 dir4 = grid_ChangeFourDir( dir );
    FLDMMDL_SetDirDisp( fmmdl, dir4 );
    KAGAYA_Printf( "�킽���ꂽ���� %xH, %d\n", dir, dir4 );
		gridWork->gridPlayer = FIELD_PLAYER_GRID_Init( fld_player, heapID );
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
	FIELD_PLAYER_GRID_Delete( gridWork->gridPlayer );
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
	FIELD_PLAYER_GRID *gridPlayer = gridWork->gridPlayer;
	
	{	//���@�ړ�
		int key_trg = GFL_UI_KEY_GetTrg( );
		int key_cont = GFL_UI_KEY_GetCont( );
		FIELD_PLAYER_GRID_Move( gridPlayer, key_trg, key_cont );
		
		{
			GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
			GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
			PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
			FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
			FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( fld_player );
			u16 tbl[DIR_MAX4] = { 0x0000, 0x8000, 0x4000, 0xc000 };
			int dir = FLDMMDL_GetDirDisp( fmmdl );
		
			FLDMMDL_GetVectorPos( fmmdl, pos );
			PLAYERWORK_setDirection( player, tbl[dir] );
			PLAYERWORK_setPosition( player, pos );
			FIELD_PLAYER_SetDir( fld_player, tbl[dir] );
			FIELD_PLAYER_SetPos( fld_player, pos );
		}
	}
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
