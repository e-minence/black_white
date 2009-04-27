//======================================================================
/**
 * @file	field_player.h
 * @date	2008.9.29
 * @brief	�t�B�[���h�v���C���[����
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "field/fieldmap_proc.h"
#include "fldmmdl.h"
#include "field_g3d_mapper.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================
///FIELD_PLAYER
typedef struct _FIELD_PLAYER FIELD_PLAYER;

//======================================================================
//	extern
//======================================================================
//�쐬�A�폜�A�X�V
extern FIELD_PLAYER * FIELD_PLAYER_Create(
		FIELDMAP_WORK *fieldWork, const VecFx32 *pos, HEAPID heapID );
extern void FIELD_PLAYER_Delete( FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_Update( FIELD_PLAYER *fld_player );

//�Q�ƁA�ݒ�
extern void FIELD_PLAYER_GetPos(
		const FIELD_PLAYER *fld_player, VecFx32 *pos );
extern void FIELD_PLAYER_SetPos(
		FIELD_PLAYER *fld_player, const VecFx32 *pos );
extern u16 FIELD_PLAYER_GetDir( const FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_SetDir( FIELD_PLAYER *fld_player, u16 dir );
extern FIELDMAP_WORK * FIELD_PLAYER_GetFieldMapWork(
		FIELD_PLAYER *fld_player );
extern FLDMMDL * FIELD_PLAYER_GetFldMMdl( FIELD_PLAYER *fld_player );
extern FLDMAPPER_GRIDINFODATA * FIELD_PLAYER_GetGridInfoData(
		FIELD_PLAYER *fld_player );

//�c�[��
extern void FIELD_PLAYER_GetFrontGridPos(
		FIELD_PLAYER *fld_player, int *gx, int *gy, int *gz );

//======================================================================
//	�����܂�
//======================================================================
extern GFL_BBDACT_RESUNIT_ID GetPlayerBBdActResUnitID(
		FIELD_PLAYER *pcActCont );

#include "field_player_grid.h"
