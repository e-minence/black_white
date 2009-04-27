//======================================================================
/**
 * @file	fieldmap_ctrl_nogrid.c
 * @brief	�t�B�[���h�}�b�v�@�R���g���[���@�m���O���b�h����
 * @author	tamada
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_player_nogrid.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================

//======================================================================
//	proto
//======================================================================

//======================================================================
//	�t�B�[���h�}�b�v�@�m���O���b�h����
//======================================================================
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
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
	FIELD_PLAYER_SetPos( fieldWork->field_player, pos );
	FIELD_PLAYER_SetDir( fieldWork->field_player, dir );
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
static void NoGridMain( FIELD_MAIN_WORK* fieldWork, VecFx32 * pos )
{
	int key_cont = FIELDMAP_GetKeyCont( fieldWork );
	FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
	FIELD_PLAYER_NOGRID_Move( fld_player, key_cont );
	FIELD_PLAYER_GetPos( fld_player, pos );
}

