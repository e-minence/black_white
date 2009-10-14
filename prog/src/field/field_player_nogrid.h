//======================================================================
/**
 * @file	field_player_nogrid.h
 * @brief	�m���O���b�h�ړ��@�t�B�[���h�v���C���[����
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "field_player.h"
#include "field_camera.h"
#include "fldmmdl.h"
#include "field_effect.h"


#include "field_rail.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================
//-------------------------------------
///	���[������p�⏕���[�N
//=====================================
typedef struct _FIELD_PLAYER_NOGRID FIELD_PLAYER_NOGRID;


//======================================================================
//	�V����
//======================================================================

// �����E�j��
extern FIELD_PLAYER_NOGRID* FIELD_PLAYER_NOGRID_Create( FIELD_PLAYER* p_player, HEAPID heapID );
extern void FIELD_PLAYER_NOGRID_Delete( FIELD_PLAYER_NOGRID* p_player );

// ����
extern void FIELD_PLAYER_NOGRID_Move( FIELD_PLAYER_NOGRID* p_player, int key_trg, int key_cont );


// �e��p�����[�^�̐ݒ�E�擾
extern void FIELD_PLAYER_NOGRID_SetLocation( FIELD_PLAYER_NOGRID* p_player, const RAIL_LOCATION* cp_location );
extern void FIELD_PLAYER_NOGRID_GetLocation( const FIELD_PLAYER_NOGRID* cp_player, RAIL_LOCATION* p_location );
extern void FIELD_PLAYER_NOGRID_GetPos( const FIELD_PLAYER_NOGRID* cp_player, VecFx32* p_pos );
extern FIELD_RAIL_WORK* FIELD_PLAYER_NOGRID_GetRailWork( const FIELD_PLAYER_NOGRID* cp_player );


// ���@FORM�̕ύX
extern void FIELD_PLAYER_NOGRID_ChangeForm( FIELD_PLAYER_NOGRID* p_player, PLAYER_MOVE_FORM form );

// ���@����̒�~
extern void FIELD_PLAYER_NOGRID_ForceStop( FIELD_PLAYER_NOGRID* p_player );



//======================================================================
//	������
//======================================================================
extern void FIELD_PLAYER_NOGRID_Free_Move( FIELD_PLAYER *fld_player, int key, fx32 onedist );
extern void FIELD_PLAYER_C3_Move(
		FIELD_PLAYER *fld_player, int key, u16 angle );


