//======================================================================
/**
 * @file	field_player_nogrid.h
 * @brief	�m���O���b�h�ړ��@�t�B�[���h�v���C���[����
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "field_player.h"
#include "field_player_core.h"
#include "field_camera.h"
#include "fldmmdl.h"
#include "field_effect.h"


#include "field_rail.h"

#include "field_sound.h"

//======================================================================
//	define
//======================================================================
//-------------------------------------
///	�X��ŉ�]����SE
//=====================================
#define FIELD_PLAYER_SE_NOGRID_ICE_SPIN ( SEQ_SE_FLD_66 )

//======================================================================
//	struct
//======================================================================
//-------------------------------------
///	���[������p�⏕���[�N
//=====================================
typedef struct _FIELD_PLAYER_NOGRID FIELD_PLAYER_NOGRID;


//======================================================================
//	FIELD_PLAYER�����Ŏg�p�����֐�
//======================================================================

// �����E�j��
extern FIELD_PLAYER_NOGRID* FIELD_PLAYER_NOGRID_Create( FIELD_PLAYER_CORE* p_player_core, HEAPID heapID );
extern void FIELD_PLAYER_NOGRID_Delete( FIELD_PLAYER_NOGRID* p_player );

// ���샊�Z�b�g�@�č\�z
extern void FIELD_PLAYER_NOGRID_Restart( FIELD_PLAYER_NOGRID* p_player, const RAIL_LOCATION* cp_pos );
extern void FIELD_PLAYER_NOGRID_Stop( FIELD_PLAYER_NOGRID* p_player );

// ����
extern void FIELD_PLAYER_NOGRID_Move( FIELD_PLAYER_NOGRID* p_player, int key_trg, int key_cont );

// �ړ��s�\�q�b�g�`�F�b�N
extern BOOL FIELD_PLAYER_NOGRID_IsHitch( const FIELD_PLAYER_NOGRID* cp_player );

// �e��p�����[�^�̐ݒ�E�擾
extern void FIELD_PLAYER_NOGRID_SetLocation( FIELD_PLAYER_NOGRID* p_player, const RAIL_LOCATION* cp_location );
extern void FIELD_PLAYER_NOGRID_GetLocation( const FIELD_PLAYER_NOGRID* cp_player, RAIL_LOCATION* p_location );
extern void FIELD_PLAYER_NOGRID_GetDirLocation( const FIELD_PLAYER_NOGRID* cp_player, u16 dir, RAIL_LOCATION* p_location );
extern void FIELD_PLAYER_NOGRID_GetPos( const FIELD_PLAYER_NOGRID* cp_player, VecFx32* p_pos );
extern FIELD_RAIL_WORK* FIELD_PLAYER_NOGRID_GetRailWork( const FIELD_PLAYER_NOGRID* cp_player );

// ���@����̒�~
extern void FIELD_PLAYER_NOGRID_ForceStop( FIELD_PLAYER_NOGRID* p_player );

#ifdef PM_DEBUG
//======================================================================
//	������
//======================================================================
extern void FIELD_PLAYER_NOGRID_Free_Move( FIELD_PLAYER *fld_player, int key, fx32 onedist );
extern void FIELD_PLAYER_C3_Move(
		FIELD_PLAYER *fld_player, int key, u16 angle );
#endif


