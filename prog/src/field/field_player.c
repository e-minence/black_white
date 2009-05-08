//======================================================================
/**
 * @file	field_player.c
 * @date	2008.9.29
 * @brief	�t�B�[���h�v���C���[����
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "field_g3d_mapper.h"
#include "fldmmdl.h"
#include "field_player.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FIELD_PLAYER
//--------------------------------------------------------------
struct _FIELD_PLAYER
{
	HEAPID heapID;
	FIELDMAP_WORK *fieldWork;
	
	u16 dir;
	u16 padding0;
	VecFx32 pos;
	
	FLDMMDL *fldmmdl;
	FLDMAPPER_GRIDINFODATA gridInfoData;
	
	//�����܂�
	GFL_BBDACT_RESUNIT_ID	bbdActResUnitID;
	GFL_BBDACT_ACTUNIT_ID	bbdActActUnitID;
};

//======================================================================
//	proto
//======================================================================
static const FLDMMDL_HEADER playerdata_FldMMdlHeader;

//======================================================================
//	�t�B�[���h�v���C���[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@�쐬
 * @param FIELDMAP_WORK *fieldWork
 * @param pos �������W
 * @param heapID HEAPID
 * @retval FIELD_PLAYER*
 */
//--------------------------------------------------------------
FIELD_PLAYER * FIELD_PLAYER_Create(
		FIELDMAP_WORK *fieldWork, const VecFx32 *pos, HEAPID heapID )
{
	FLDMMDLSYS *fmmdlsys;
	FIELD_PLAYER *fld_player;
	
	fld_player = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLAYER) );
	fld_player->fieldWork = fieldWork;
	fld_player->pos = *pos;

	FLDMAPPER_GRIDINFODATA_Init( &fld_player->gridInfoData );
	
	//FLDMMDL�Z�b�g�A�b�v
	fmmdlsys = FIELDMAP_GetFldMMdlSys( fieldWork );
	
	fld_player->fldmmdl =
		FLDMMDLSYS_SearchOBJID( fmmdlsys, FLDMMDL_ID_PLAYER );

	if( fld_player->fldmmdl == NULL )	//�V�K
	{
		FLDMMDL_HEADER head;
		head = playerdata_FldMMdlHeader;
		head.gx = SIZE_GRID_FX32( pos->x );
		head.gz = SIZE_GRID_FX32( pos->z );
		head.y = pos->y;
		fld_player->fldmmdl = FLDMMDLSYS_AddFldMMdl( fmmdlsys, &head, 0 );
	}
	else //���A
	{
		int gx = SIZE_GRID_FX32( pos->x );
		int gy = SIZE_GRID_FX32( pos->y );
		int gz = SIZE_GRID_FX32( pos->z );
		FLDMMDL *fmmdl = fld_player->fldmmdl;
		
		FLDMMDL_SetGridPosX( fmmdl, gx );
		FLDMMDL_SetGridPosY( fmmdl, gy );
		FLDMMDL_SetGridPosZ( fmmdl, gz );
		FLDMMDL_SetVectorPos( fmmdl, pos );
	}
	
	FLDMMDL_SetStatusBitNotZoneDelete( fld_player->fldmmdl, TRUE );
	return( fld_player );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@�폜
 * @param fld_player FIELD_PLAYER
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_Delete( FIELD_PLAYER *fld_player )
{
	//���샂�f���̍폜�̓t�B�[���h���C������
	GFL_HEAP_FreeMemory( fld_player );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�v���C���[�@�X�V
 * @param fld_player FIELD_PLAYER
 * @param dir ����
 * @param pos ���W
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_Update( FIELD_PLAYER *fld_player )
{
}

//======================================================================
//	FILED_PLAYER�@�Q�ƁA�ݒ�
//======================================================================
//--------------------------------------------------------------
/**
 * FILED_PLAYER�@���W�擾
 * @param fld_player FIELD_PLAYER
 * @param pos ���W�i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GetPos( const FIELD_PLAYER *fld_player, VecFx32 *pos )
{
	*pos = fld_player->pos;
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@���W�Z�b�g
 * @param fld_player FIELD_PLAYER
 * @param pos �Z�b�g������W
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetPos( FIELD_PLAYER *fld_player, const VecFx32 *pos )
{
	int gx = SIZE_GRID_FX32( pos->x );
	int gy = SIZE_GRID_FX32( pos->y );
	int gz = SIZE_GRID_FX32( pos->z );
	FLDMMDL *fmmdl = fld_player->fldmmdl;
	
	FLDMMDL_SetOldGridPosX( fmmdl, FLDMMDL_GetGridPosX(fmmdl) );
	FLDMMDL_SetOldGridPosY( fmmdl, FLDMMDL_GetGridPosY(fmmdl) );
	FLDMMDL_SetOldGridPosZ( fmmdl, FLDMMDL_GetGridPosZ(fmmdl) );
	FLDMMDL_SetGridPosX( fmmdl, gx );
	FLDMMDL_SetGridPosY( fmmdl, gy );
	FLDMMDL_SetGridPosZ( fmmdl, gz );
	FLDMMDL_SetVectorPos( fmmdl, pos );
	
	fld_player->pos = *pos;
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@�����擾
 * @param fld_player FIELD_PLAYER
 * @retval u16 ����
 */
//--------------------------------------------------------------
u16 FIELD_PLAYER_GetDir( const FIELD_PLAYER *fld_player )
{
	return( fld_player->dir );
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@�����Z�b�g
 * @param fld_player FIELD_PLAYER
 * @param dir ����
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetDir( FIELD_PLAYER *fld_player, u16 dir )
{
	fld_player->dir = dir;
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@FIELDMAP_WORK�擾
 * @param fld_player FIELD_PLAYER
 * @retval FIELDMAP_WORK*
 */
//--------------------------------------------------------------
FIELDMAP_WORK * FIELD_PLAYER_GetFieldMapWork( FIELD_PLAYER *fld_player )
{
	return( fld_player->fieldWork );
}

//--------------------------------------------------------------
/**
 * FILED_PLAYER�@FLDMMDL�擾
 * @param fld_player FIELD_PLAYER
 * @retval FLDMMDL*
 */
//--------------------------------------------------------------
FLDMMDL * FIELD_PLAYER_GetFldMMdl( FIELD_PLAYER *fld_player )
{
	return( fld_player->fldmmdl );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER FLDMAPPER_GRIDINFODATA�擾
 * @param fld_player* FIELD_PLAYER
 * @retval FLDMAPPER_GRIDINFODATA*
 */
//--------------------------------------------------------------
FLDMAPPER_GRIDINFODATA * FIELD_PLAYER_GetGridInfoData(
		FIELD_PLAYER *fld_player )
{
	return( &fld_player->gridInfoData );
}

//======================================================================
//	FILED_PLAYER�@�c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�̑O���ʒu���O���b�h�P�ʂŎ擾
 * @param fld_player FIELD_PLAYER
 * @param gx X���W�i�[��
 * @param gy Y���W�i�[��
 * @param gz Z���W�i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GetFrontGridPos(
		FIELD_PLAYER *fld_player, int *gx, int *gy, int *gz )
{
	int dir;
	FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( fld_player );
	
	*gx = FLDMMDL_GetGridPosX( fmmdl );
	*gy = FLDMMDL_GetGridPosY( fmmdl );
	*gz = FLDMMDL_GetGridPosZ( fmmdl );
	dir = FLDMMDL_GetDirDisp( fmmdl );
	
	*gx += FLDMMDL_TOOL_GetDirAddValueGridX( dir );
	*gz += FLDMMDL_TOOL_GetDirAddValueGridZ( dir );
}

//======================================================================
//	data
//======================================================================
//--------------------------------------------------------------
/// ���@���샂�f���w�b�_�[
//--------------------------------------------------------------
static const FLDMMDL_HEADER playerdata_FldMMdlHeader =
{
	FLDMMDL_ID_PLAYER,	///<����ID
	HERO,	///<�\������OBJ�R�[�h
	MV_DMY,	///<����R�[�h
	0,	///<�C�x���g�^�C�v
	0,	///<�C�x���g�t���O
	0,	///<�C�x���gID
	0,	///<�w�����
	0,	///<�w��p�����^ 0
	0,	///<�w��p�����^ 1
	0,	///<�w��p�����^ 2
	MOVE_LIMIT_NOT,	///<X�����ړ�����
	MOVE_LIMIT_NOT,	///<Z�����ړ�����
	0,	///<�O���b�hX
	0,	///<�O���b�hZ
	0,	///<Y�l fx32�^
};

//======================================================================
//	�ȉ������܂�
//======================================================================
GFL_BBDACT_RESUNIT_ID GetPlayerBBdActResUnitID( FIELD_PLAYER *fld_player )
{
	return( fld_player->bbdActResUnitID );
}

#if 0
void PlayerActGrid_Update(
	FIELD_PLAYER *fld_player, u16 dir, const VecFx32 *pos )
{
	VecFx32 trans;
	fld_player->pos = *pos;
	
	SetGridPlayerActTrans( fld_player, pos );
	FLDMMDL_SetForceDirDisp( fld_player->fldmmdl, dir );
}
#endif

#if 0
void SetGridPlayerActTrans( FIELD_PLAYER* fld_player, const VecFx32* trans )
{
	int gx = SIZE_GRID_FX32( trans->x );
	int gy = SIZE_GRID_FX32( trans->y );
	int gz = SIZE_GRID_FX32( trans->z );
	
	FLDMMDL_SetOldGridPosX( fld_player->fldmmdl,
		FLDMMDL_GetGridPosX(fld_player->fldmmdl) );
	FLDMMDL_SetOldGridPosY( fld_player->fldmmdl,
		FLDMMDL_GetGridPosY(fld_player->fldmmdl) );
	FLDMMDL_SetOldGridPosZ( fld_player->fldmmdl,
		FLDMMDL_GetGridPosZ(fld_player->fldmmdl) );
	
	FLDMMDL_SetGridPosX( fld_player->fldmmdl, gx );
	FLDMMDL_SetGridPosY( fld_player->fldmmdl, gy );
	FLDMMDL_SetGridPosZ( fld_player->fldmmdl, gz );
	FLDMMDL_SetVectorPos( fld_player->fldmmdl, trans );
	
	VEC_Set( &fld_player->pos, trans->x, trans->y, trans->z );
}
#endif

#if 0
void PlayerActGrid_AnimeSet(
	FIELD_PLAYER *fld_player, int dir, PLAYER_ANIME_FLAG flag )
{
	switch( flag ){
	case PLAYER_ANIME_FLAG_STOP:
		FLDMMDL_SetDrawStatus( fld_player->fldmmdl, DRAW_STA_STOP );
		break;
	case PLAYER_ANIME_FLAG_WALK:
		FLDMMDL_SetDrawStatus( fld_player->fldmmdl, DRAW_STA_WALK_8F );
		break;
	default:
		FLDMMDL_SetDrawStatus( fld_player->fldmmdl, DRAW_STA_WALK_4F );
		break;
	}
}
#endif

#if 0
FLDMAPPER_GRIDINFODATA * PlayerActGrid_GridInfoGet( FIELD_PLAYER *fld_player )
{
	return( &fld_player->gridInfoData );
}
#endif

#if 0
void PlayerActGrid_ScaleSizeSet(
	FIELD_PLAYER *fld_player, fx16 sizeX, fx16 sizeY )
{
	GFL_BBDACT_SYS *bbdActSys = FIELDMAP_GetBbdActSys( fld_player->fieldWork );
	int idx = GFL_BBDACT_GetBBDActIdxResIdx(
			bbdActSys, fld_player->bbdActActUnitID );
	GFL_BBD_SetObjectSiz(
		GFL_BBDACT_GetBBDSystem(bbdActSys),
		idx, &sizeX, &sizeY );
}
#endif

#if 0
void PLAYER_GRID_GetFrontGridPos(
	FIELD_PLAYER *fld_player, int *gx, int *gy, int *gz )
{
	int dir;
	FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( fld_player );
	
	*gx = FLDMMDL_GetGridPosX( fmmdl );
	*gy = FLDMMDL_GetGridPosY( fmmdl );
	*gz = FLDMMDL_GetGridPosZ( fmmdl );
	dir = FLDMMDL_GetDirDisp( fmmdl );
	
	*gx += FLDMMDL_TOOL_GetDirAddValueGridX( dir );
	*gz += FLDMMDL_TOOL_GetDirAddValueGridZ( dir );
}
#endif
