//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_work_func.h
 *	@brief	�t�B�[���h���[���@��{�֐��S
 *	@author	tomoya takahashi
 *	@date		2009.07.06
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "field_rail.h"

//-----------------------------------------------------------------------------
/**
 *					�ʒu�ړ�
*/
//-----------------------------------------------------------------------------
//------------------------------------------------------------------
//  ����
//------------------------------------------------------------------
extern void FIELD_RAIL_POSFUNC_StraitLine(const FIELD_RAIL_WORK * work, VecFx32 * pos);

//------------------------------------------------------------------
//  �J�[�u
//------------------------------------------------------------------
typedef struct
{
	// ���S���W
	fx32 x;
	fx32 y;
	fx32 z;
} RAIL_POSFUNC_CURVE_WORK;
extern void FIELD_RAIL_POSFUNC_CurveLine(const FIELD_RAIL_WORK * work, VecFx32 * pos);


//-----------------------------------------------------------------------------
/**
 *					�����v�Z
*/
//-----------------------------------------------------------------------------
//------------------------------------------------------------------
//  ����
//------------------------------------------------------------------
extern fx32 FIELD_RAIL_LINE_DIST_FUNC_StraitLine( const RAIL_POINT * point_s, const RAIL_POINT * point_e, const RAIL_LINEPOS_SET * line_pos_set );
//------------------------------------------------------------------
//  �J�[�u
//	FIELD_RAIL_POSFUNC_CurveLine��p
//------------------------------------------------------------------
extern fx32 FIELD_RAIL_LINE_DIST_FUNC_CircleLine( const RAIL_POINT * point_s, const RAIL_POINT * point_e, const RAIL_LINEPOS_SET * line_pos_set );


//-----------------------------------------------------------------------------
/**
 *					�J����
*/
//-----------------------------------------------------------------------------
//------------------------------------------------------------------
//  �Œ�ʒu�J����
//------------------------------------------------------------------
typedef struct
{
	// �J�������W
	fx32 x;
	fx32 y;
	fx32 z;
} RAIL_CAMERAFUNC_FIXPOS_WORK;
extern void FIELD_RAIL_CAMERAFUNC_FixPosCamera(const FIELD_RAIL_MAN* man);

//------------------------------------------------------------------
//  �Œ�A���O���J����
//------------------------------------------------------------------
typedef struct
{
	// �J�������W
	u32	pitch;
	u32 yaw;
	u32 len;
} RAIL_CAMERAFUNC_FIXANGLE_WORK;
extern void FIELD_RAIL_CAMERAFUNC_FixAngleCamera(const FIELD_RAIL_MAN* man);

//------------------------------------------------------------------
//  �I�t�Z�b�g�A���O���J����	�i���C����p�j
//  �J�n�|�C���g - �I���|�C���g�@�́@FixAngleCamera�ł���K�v������܂��B
//------------------------------------------------------------------
extern void FIELD_RAIL_CAMERAFUNC_OfsAngleCamera(const FIELD_RAIL_MAN* man);

//------------------------------------------------------------------
//  ���S�Œ�J����
//------------------------------------------------------------------
typedef struct
{
	// �J�������W
	fx32 pos_x;
	fx32 pos_y;
	fx32 pos_z;

	// �^�[�Q�b�g���W
	fx32 target_x;
	fx32 target_y;
	fx32 target_z;
} RAIL_CAMERAFUNC_FIXALL_WORK;
extern void FIELD_RAIL_CAMERAFUNC_FixAllCamera(const FIELD_RAIL_MAN* man);

//------------------------------------------------------------------
//  �~����J����
//------------------------------------------------------------------
typedef struct
{
	u32		pitch;
	fx32	len;
	
	fx32 target_x;
	fx32 target_y;
	fx32 target_z;
} RAIL_CAMERAFUNC_CIRCLE_WORK;
extern void FIELD_RAIL_CAMERAFUNC_CircleCamera( const FIELD_RAIL_MAN * man );

//------------------------------------------------------------------
//  �����Œ�~����J����
//------------------------------------------------------------------
typedef struct
{
	u32		pitch;
	fx32	len;
	
	fx32 target_x;
	fx32 target_y;
	fx32 target_z;
} RAIL_CAMERAFUNC_FIXLEN_CIRCLE_WORK;
extern void FIELD_RAIL_CAMERAFUNC_FixLenCircleCamera( const FIELD_RAIL_MAN * man );


//------------------------------------------------------------------
//  ���C���A�^�[�Q�b�g�ύX �~����J����
//------------------------------------------------------------------
typedef struct
{
	u32		pitch;
	fx32	len;
  
	fx32 target_sx;
	fx32 target_sy;
	fx32 target_sz;

	fx32 target_ex;
	fx32 target_ey;
	fx32 target_ez;
} RAIL_CAMERAFUNC_TARGETCHANGE_CIRCLE_WORK;
extern void FIELD_RAIL_CAMERAFUNC_TargetChangeCircleCamera( const FIELD_RAIL_MAN * man );


//------------------------------------------------------------------
//  �o�����������^�[�Q�b�g �~����J����
//------------------------------------------------------------------
typedef struct
{
	u32		pitch;
	fx32	len;
  
	fx32 center_x;
	fx32 center_z;
} RAIL_CAMERAFUNC_PLAYERTARGET_CIRCLE_WORK;
extern void FIELD_RAIL_CAMERAFUNC_PlayerTargetCircleCamera( const FIELD_RAIL_MAN * man );


#ifdef _cplusplus
}	// extern "C"{
#endif



