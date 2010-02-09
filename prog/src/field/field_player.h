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
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap_proc.h"
#include "fldmmdl.h"
#include "field_g3d_mapper.h"
#include "map_attr.h"

#include "field_player_code.h"
#include "field_effect.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
/// PLAYER_MOVE_VALUE
//--------------------------------------------------------------
typedef enum
{
  PLAYER_MOVE_VALUE_STOP, ///<��~���
  PLAYER_MOVE_VALUE_WALK, ///<�ړ���
  PLAYER_MOVE_VALUE_TURN, ///<�U�������
}PLAYER_MOVE_VALUE;

//--------------------------------------------------------------
/// PLAYER_MOVE_STATE
//--------------------------------------------------------------
typedef enum
{
  PLAYER_MOVE_STATE_OFF,    ///<����Ȃ�
  PLAYER_MOVE_STATE_START,  ///<����J�n
  PLAYER_MOVE_STATE_ON,     ///<���쒆
  PLAYER_MOVE_STATE_END,    ///<����I��
}PLAYER_MOVE_STATE;

#if 0 //PLAYER_WORK�ֈړ�
//--------------------------------------------------------------
/// PLAYER_MOVE_FORM
//--------------------------------------------------------------
typedef enum
{
  PLAYER_MOVE_FORM_NORMAL = 0,   ///<���@�񑫕��s
  PLAYER_MOVE_FORM_CYCLE,  ///<���@���]�Ԉړ�
  PLAYER_MOVE_FORM_SWIM, ///<���@�g���ړ�
  PLAYER_MOVE_FORM_MAX,
}PLAYER_MOVE_FORM;
#endif

//--------------------------------------------------------------
/// PLAYER_DRAW_FORM
//--------------------------------------------------------------
typedef enum
{
  PLAYER_DRAW_FORM_NORMAL = 0, ///<���@�񑫕��s
  PLAYER_DRAW_FORM_CYCLE, ///<���@���]�Ԉړ�
  PLAYER_DRAW_FORM_SWIM, ///<���@�g���ړ�
  PLAYER_DRAW_FORM_ITEMGET, ///<���@�A�C�e���Q�b�g
  PLAYER_DRAW_FORM_SAVEHERO, ///<���@�Z�[�u
  PLAYER_DRAW_FORM_PCHERO, ///<���@PC�a��
  PLAYER_DRAW_FORM_YURE, ///<���@���
  PLAYER_DRAW_FORM_FISHING, ///<���@�ނ�
  PLAYER_DRAW_FORM_CUTIN, ///<���@�J�b�g�C��
  PLAYER_DRAW_FORM_MAX,
}PLAYER_DRAW_FORM;

//--------------------------------------------------------------
/// PLAYER_EVENTBIT
//--------------------------------------------------------------
typedef enum
{
  PLAYER_EVENTBIT_NON = 0, ///<�C�x���g���@�Ȃ�
  PLAYER_EVENTBIT_KAIRIKI = (1<<0), ///<�C�x���g���@�����肫
  PLAYER_EVENTBIT_TAKINOBORI = (1<<1), ///<�C�x���g���� �����̂ڂ�
}PLAYER_EVENTBIT;

//======================================================================
//	struct
//======================================================================
///FIELD_PLAYER
typedef struct _FIELD_PLAYER FIELD_PLAYER;
///FIELD_PLAYER_REQBIT
typedef u32 FIELD_PLAYER_REQBIT;

//======================================================================
//	extern
//======================================================================
//======================================================================
//	Grid Rail Hybrid���ʏ���
//======================================================================
//�쐬�A�폜�A�X�V
extern FIELD_PLAYER * FIELD_PLAYER_Create(
    PLAYER_WORK *playerWork, FIELDMAP_WORK *fieldWork,
		const VecFx32 *pos, int sex, HEAPID heapID );
extern void FIELD_PLAYER_Delete( FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_Update( FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_UpdateMoveStatus( FIELD_PLAYER *fld_player );
extern GMEVENT * FIELD_PLAYER_CheckMoveEvent( FIELD_PLAYER *fld_player,
    int key_trg, int key_cont, PLAYER_EVENTBIT evbit );

// �v���C���[����ύX REQBIT 
extern void FIELD_PLAYER_SetRequest(
  FIELD_PLAYER * fld_player, FIELD_PLAYER_REQBIT req_bit );
extern void FIELD_PLAYER_UpdateRequest( FIELD_PLAYER * fld_player );

// ���@�I�[�_�[�`�F�b�N
extern BOOL FIELD_PLAYER_CheckStartMove(
    FIELD_PLAYER * fld_player, u16 dir );

// ������~
extern void FIELD_PLAYER_ForceStop( FIELD_PLAYER * fld_player );
extern void FIELD_PLAYER_SetMoveStop( FIELD_PLAYER * fld_player );

// EffectTask �g���̃|�P�����Ȃ�
extern void FIELD_PLAYER_SetEffectTaskWork(
    FIELD_PLAYER * fld_player, FLDEFF_TASK *task );
extern FLDEFF_TASK * FIELD_PLAYER_GetEffectTaskWork(
    FIELD_PLAYER * fld_player );

// �w����������@�ɓn���Ƃǂ��Ȃ邩
extern PLAYER_MOVE_VALUE FIELD_PLAYER_GetDirMoveValue(
    FIELD_PLAYER * fld_player, u16 dir );

// �L�[���͕���
extern u16 FIELD_PLAYER_GetKeyDir( const FIELD_PLAYER* fld_player, int key );

// �g���
extern void FIELD_PLAYER_SetNaminori( FIELD_PLAYER * fld_player );
extern void FIELD_PLAYER_SetNaminoriEnd( FIELD_PLAYER * fld_player );

//�Q�ƁA�ݒ�
extern void FIELD_PLAYER_GetPos(
		const FIELD_PLAYER *fld_player, VecFx32 *pos );
extern void FIELD_PLAYER_SetPos(
		FIELD_PLAYER *fld_player, const VecFx32 *pos );
extern u16 FIELD_PLAYER_GetDir( const FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_SetDir( FIELD_PLAYER *fld_player, u16 dir );

extern FIELDMAP_WORK * FIELD_PLAYER_GetFieldMapWork(
		FIELD_PLAYER *fld_player );
extern MMDL * FIELD_PLAYER_GetMMdl( const FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_SetMoveValue(
    FIELD_PLAYER *fld_player, PLAYER_MOVE_VALUE val );
extern PLAYER_MOVE_VALUE FIELD_PLAYER_GetMoveValue(
    const FIELD_PLAYER *fld_player );
extern PLAYER_MOVE_STATE FIELD_PLAYER_GetMoveState(
    const FIELD_PLAYER *fld_player );
extern PLAYER_MOVE_FORM FIELD_PLAYER_GetMoveForm(
    const FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_SetMoveForm(
    FIELD_PLAYER *fld_player, PLAYER_MOVE_FORM form );
extern int FIELD_PLAYER_GetSex( const FIELD_PLAYER *fld_player );
extern GAMESYS_WORK * FIELD_PLAYER_GetGameSysWork( FIELD_PLAYER *fld_player );

extern MAPATTR FIELD_PLAYER_GetMapAttr( const FIELD_PLAYER *fld_player );
extern MAPATTR FIELD_PLAYER_GetDirMapAttr( const FIELD_PLAYER *fld_player, u16 dir );

//���ʁAOBJ�R�[�h�A�e�t�H�[��
extern u16 FIELD_PLAYER_GetDrawFormToOBJCode(
    int sex, PLAYER_DRAW_FORM form );
extern PLAYER_MOVE_FORM FIELD_PLAYER_GetOBJCodeToMoveForm(
    int sex, u16 code );
extern u16 FIELD_PLAYER_GetMoveFormToOBJCode(
    int sex, PLAYER_MOVE_FORM form );
extern PLAYER_DRAW_FORM FIELD_PLAYER_GetOBJCodeToDrawForm(
    int sex, u16 code );
extern PLAYER_DRAW_FORM FIELD_PLAYER_CheckOBJCodeToDrawForm(
    u16 code );
extern BOOL FIELD_PLAYER_CheckChangeEventDrawForm( FIELD_PLAYER *fld_player );

//�c�[��
extern void FIELD_PLAYER_GetDirPos(
		const FIELD_PLAYER *fld_player, u16 dir, VecFx32 *pos );
extern void FIELD_PLAYER_GetDirWay( 
    const FIELD_PLAYER *fld_player, u16 dir, VecFx32* way );
extern BOOL FIELD_PLAYER_CheckLiveMMdl( const FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_ChangeMoveForm(
    FIELD_PLAYER *fld_player, PLAYER_MOVE_FORM form );
extern void FIELD_PLAYER_ResetMoveForm( FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_ChangeDrawForm(
    FIELD_PLAYER *fld_player, PLAYER_DRAW_FORM form );
extern PLAYER_DRAW_FORM FIELD_PLAYER_GetDrawForm( FIELD_PLAYER *fld_player );
extern void FIELD_PLAYER_ChangeFormRequest( FIELD_PLAYER *fld_player, PLAYER_DRAW_FORM form );
extern BOOL FIELD_PLAYER_ChangeFormWait( FIELD_PLAYER *fld_player );
extern BOOL FIELD_PLAYER_CheckAttrNaminori(
    FIELD_PLAYER *fld_player, MAPATTR nattr, MAPATTR fattr );
extern void FIELD_PLAYER_CheckSpecialDrawForm(
    FIELD_PLAYER *fld_player, BOOL menu_open_flag );
extern void FIELD_PLAYER_ChangeOBJCode( FIELD_PLAYER *fld_player, u16 code );
extern void FIELD_PLAYER_ClearOBJCodeFix( FIELD_PLAYER *fld_player );
extern BOOL FIELD_PLAYER_CheckIllegalOBJCode( FIELD_PLAYER *fld_player );

// ���f������
extern MMDL * FIELD_PLAYER_GetFrontMMdl( const FIELD_PLAYER *fld_player );


//======================================================================
//	Grid ��p����
//======================================================================
// GRID���[�N�̃Z�b�g�A�b�v
extern void FIELD_PLAYER_SetUpGrid( FIELD_PLAYER *fld_player, HEAPID heapID );
// GRID���[�N�̓��쏈��
extern void FIELD_PLAYER_MoveGrid( FIELD_PLAYER *fld_player, int key_trg, int key_cont );

//TOOL
extern void FIELD_PLAYER_GetDirGridPos(
		const FIELD_PLAYER *fld_player, u16 dir, s16 *gx, s16 *gy, s16 *gz );
extern void FIELD_PLAYER_GetFrontGridPos(
		const FIELD_PLAYER *fld_player, s16 *gx, s16 *gy, s16 *gz );




//======================================================================
//	NoGrid ��p����
//======================================================================
// NOGRID���[�N�̃Z�b�g�A�b�v
extern void FIELD_PLAYER_SetUpNoGrid( FIELD_PLAYER *fld_player, HEAPID heapID );
// NOGRID���[�N�̓��쏈��
extern void FIELD_PLAYER_MoveNoGrid( FIELD_PLAYER *fld_player, int key_trg, int key_cont );

// NOGRID ���X�^�[�g
extern void FIELD_PLAYER_RestartNoGrid( FIELD_PLAYER *fld_player, const RAIL_LOCATION* cp_pos );
// NOGRID�@��~
extern void FIELD_PLAYER_StopNoGrid( FIELD_PLAYER *fld_player );

// RAILWK�̎擾
extern FIELD_RAIL_WORK* FIELD_PLAYER_GetNoGridRailWork( const FIELD_PLAYER *fld_player );

// Location
extern void FIELD_PLAYER_SetNoGridLocation( FIELD_PLAYER* p_player, const RAIL_LOCATION* cp_location );
extern void FIELD_PLAYER_GetNoGridLocation( const FIELD_PLAYER* cp_player, RAIL_LOCATION* p_location );

// RailPos
extern void FIELD_PLAYER_GetNoGridPos( const FIELD_PLAYER* cp_player, VecFx32* p_pos );

