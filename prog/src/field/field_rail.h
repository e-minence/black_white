//============================================================================================
/**
 * @file  field_rail.h
 * @brief �m�[�O���b�h�ł̎��@�E�J��������\��
 * @author  tamada, tomoya
 * @date  2009.05.18
 *
 */
//============================================================================================

#pragma once

#include "field_camera.h"

#include "field/rail_location.h"
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _FIELD_RAIL_MAN FIELD_RAIL_MAN;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _FIELD_RAIL FIELD_RAIL;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _RAIL_POINT RAIL_POINT;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _RAIL_LINE RAIL_LINE;



//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
typedef struct _RAIL_CAMERA_SET{
  u32 func_index;
  u32 param0;
  u32 param1;
  u32 param2;
  u32 param3;
  u32 param4;
  u32 param5;
}RAIL_CAMERA_SET;

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
typedef struct _RAIL_LINEPOS_SET{
  u32 func_index;
  u32 func_dist_index;
  u32 param0;
  u32 param1;
  u32 param2;
  u32 param3;
}RAIL_LINEPOS_SET;



//------------------------------------------------------------------
//------------------------------------------------------------------
typedef void (RAIL_POS_FUNC)(const FIELD_RAIL * man, VecFx32 * pos);
typedef void (RAIL_CAMERA_FUNC)(const FIELD_RAIL_MAN * man);
typedef fx32 (RAIL_LINE_DIST_FUNC)(const RAIL_POINT * point_s, const RAIL_POINT * point_e, const RAIL_LINEPOS_SET * line_pos_set );

//------------------------------------------------------------------
/**
 * @brief �L�[�̒�`
 *
 * �ǂ̓��͂łǂ����������ւ̈ړ��ɂȂ邩�H�̌��т��̂��߂̒�`
 */
//------------------------------------------------------------------
typedef enum {
  RAIL_KEY_NULL = 0,
  RAIL_KEY_UP,
  RAIL_KEY_RIGHT,
  RAIL_KEY_DOWN,
  RAIL_KEY_LEFT,

  RAIL_KEY_MAX
}RAIL_KEY;

//------------------------------------------------------------------
/**
 * @brief ���[���^�C�v
 *
 * LOCATION�Ȃǂ̓��e���APOINT�Ȃ̂�LINE�Ȃ̂��𔻒f���邽�߂̒�`
 */
//------------------------------------------------------------------
typedef enum {  
  FIELD_RAIL_TYPE_POINT = 0,
  FIELD_RAIL_TYPE_LINE,

  FIELD_RAIL_TYPE_MAX
}FIELD_RAIL_TYPE;


//------------------------------------------------------------------
/**
 * @brief �e�[�u���C���f�b�N�X�����l
 */
//------------------------------------------------------------------
#define RAIL_TBL_NULL	( 0xffffffff )

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
enum {  
  RAIL_CONNECT_LINE_MAX = 4,
  RAIL_CONNECT_POINT_MAX = 2,
};


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
struct _RAIL_POINT {  
  //keys[n]�Ƀ}�b�`������lines[n]�Ɉړ�����
	u32 lines[RAIL_CONNECT_LINE_MAX];
  RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];

  ///POINT�̈ʒu���W
  VecFx32 pos;

	u32 camera_set;
  const char * name;
};


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
struct _RAIL_LINE {
  //point_s --> point_e�ւ̓�����key�����𐳂Ƃ��Đ��䂷��
	u32 point_s;
	u32 point_e;
  RAIL_KEY key;
	u32 line_pos_set;

	u32 camera_set;


  const char * name;
  //������K�v�ɂȂ邪���l����ƍ�������
  //fx32 width_margin;  ///<LINE�ɑ΂��ē����镝
  //u32 width_divider;
};


//------------------------------------------------------------------
/// ���[�����
//------------------------------------------------------------------
typedef struct {
  u16 point_count;
  u16 line_count;
  u16 camera_count;
  u16 linepos_count;
  u16 camera_func_count;
  u16 linepos_func_count;
  u16 line_dist_func_count;
	u16 dummy00;
  const RAIL_POINT* point_table;
  const RAIL_LINE*	line_table;
	const RAIL_CAMERA_SET*	camera_table;
	const RAIL_LINEPOS_SET* linepos_table;
	RAIL_CAMERA_FUNC*const*	camera_func;
	RAIL_POS_FUNC*const*		line_pos_func;
	RAIL_LINE_DIST_FUNC*const* line_dist_func;

  s32   ofs_max;      // ��������
  fx32  ofs_unit;     // ���̈ړ��P��
} RAIL_SETTING;



//============================================================================================
//
//
//  ���[������̂��߂̊֐��Q
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
extern FIELD_RAIL_MAN * FIELD_RAIL_MAN_Create(HEAPID heapID, FIELD_CAMERA * camera);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Delete(FIELD_RAIL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Update(FIELD_RAIL_MAN * man, int key_cont);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Load(FIELD_RAIL_MAN * man, const RAIL_SETTING * setting);
extern void FIELD_RAIL_MAN_GetLocation(const FIELD_RAIL_MAN * man, RAIL_LOCATION * location);
extern void FIELD_RAIL_MAN_SetLocation(FIELD_RAIL_MAN * man, const RAIL_LOCATION * location);


//------------------------------------------------------------------
//  ���Ԃ�f�o�b�O�p�r�̂�
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_SetActiveFlag(FIELD_RAIL_MAN * man, BOOL flag);
extern BOOL FIELD_RAIL_MAN_GetActiveFlag(const FIELD_RAIL_MAN *man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_GetPos(const FIELD_RAIL_MAN * man, VecFx32 * pos);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_UpdateCamera(const FIELD_RAIL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_GetDirection(const FIELD_RAIL_MAN * man, VecFx32 * dir);


//------------------------------------------------------------------
//  LINE�ړ��֐�
//------------------------------------------------------------------
extern void FIELD_RAIL_POSFUNC_StraitLine(const FIELD_RAIL * rail, VecFx32 * pos);
extern void FIELD_RAIL_POSFUNC_CurveLine(const FIELD_RAIL * rail, VecFx32 * pos);

extern const RAIL_LINEPOS_SET RAIL_LINEPOS_SET_Default;
extern const RAIL_CAMERA_SET RAIL_CAMERA_SET_Default;
extern void FIELD_RAIL_CAMERAFUNC_FixPosCamera(const FIELD_RAIL_MAN* man);
extern void FIELD_RAIL_CAMERAFUNC_FixAngleCamera(const FIELD_RAIL_MAN* man);
extern void FIELD_RAIL_CAMERAFUNC_OfsAngleCamera(const FIELD_RAIL_MAN* man);
extern void FIELD_RAIL_CAMERAFUNC_FixAllCamera(const FIELD_RAIL_MAN* man);

extern fx32 FIELD_RAIL_LINE_DIST_FUNC_StraitLine( const RAIL_POINT * point_s, const RAIL_POINT * point_e, const RAIL_LINEPOS_SET * line_pos_set );

//-----------------------------------------------------------------------------
//  �~����
//-----------------------------------------------------------------------------
extern void FIELD_RAIL_POSFUNC_CircleCamera( const FIELD_RAIL_MAN * man );
extern void FIELD_RAIL_POSFUNC_FixLenCircleCamera( const FIELD_RAIL_MAN * man );

extern fx32 FIELD_RAIL_LINE_DIST_FUNC_CircleLine( const RAIL_POINT * point_s, const RAIL_POINT * point_e, const RAIL_LINEPOS_SET * line_pos_set );
