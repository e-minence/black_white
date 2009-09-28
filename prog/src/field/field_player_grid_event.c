//======================================================================
/**
 * @file  field_player_grid_event.c
 * @brief �O���b�h��p �t�B�[���h�v���C���[����@�C�x���g�֘A
 * @author kagaya
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "fldmmdl.h"
#include "field_player.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "fieldmap_ctrl_grid.h"
#include "field_player_grid.h"
#include "map_attr.h"

#include "fldeff_namipoke.h"

#include "field_sound.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================

//======================================================================
//  �O���b�h���@�@�C�x���g�ړ��֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�C�x���g�ړ��`�F�b�N
 * @param *gjiki FIELD_PLAYER_GRID 
 * @param dir �ړ�����
 * @param evbit PLAYER_EVENTBIT
 * @retval BOOL TRUE=���@�C�x���g�ړ�����
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_CheckMoveEvent(
    FIELD_PLAYER *fld_player, FIELD_PLAYER_GRID *gjiki,
    u16 dir, PLAYER_EVENTBIT evbit )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  
#if 0 //�����ړ��C�x���g
 	if( Player_MoveBitCheck_Force(jiki) == TRUE ){
		if( JikiEventCheck_ForceMoveHitEvent(fsys,jiki,flag) == TRUE ){
			return( TRUE );
		}
#endif
  
#if 0 //�ړ��J�n�\�`�F�b�N	
	if( Player_MoveStartCheck(jiki,dir) == FALSE ){
		return( FALSE );
	}
#endif

#if 0 //�L�[���͖����ł���������C�x���g�`�F�b�N
	if( dir == DIR_NOT ){
		if( JikiEventCheck_KeyOFFEvent(fsys,jiki,dir,flag) == TRUE ){
			return( TRUE );
		}
		
		return( FALSE );
	}
#endif

#if 0 //�C�x���g�q�b�g�`�F�b�N
	if( JikiEventCheck_MoveHitEvent(fsys,jiki,dir,flag) == TRUE ){
		return( TRUE );
	}
#endif
	return( FALSE );
}


