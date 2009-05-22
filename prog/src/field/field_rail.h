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
//------------------------------------------------------------------
typedef void (RAIL_POS_FUNC)(const FIELD_RAIL * man, VecFx32 * pos);
typedef void (RAIL_CAMERA_FUNC)(const FIELD_RAIL_MAN * man);


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
typedef struct _RAIL_CAMERA_SET{
  RAIL_CAMERA_FUNC * func;
  u32 param0;
  u32 param1;
  u32 param2;
  u32 param3;
}RAIL_CAMERA_SET;

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
typedef struct _RAIL_LINEPOS_SET{
  RAIL_POS_FUNC * func;
  u32 param0;
  u32 param1;
  u32 param2;
  u32 param3;
}RAIL_LINEPOS_SET;

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
  const RAIL_LINE * lines[RAIL_CONNECT_LINE_MAX];
  RAIL_KEY keys[RAIL_CONNECT_LINE_MAX];

  ///POINT�̈ʒu���W
  VecFx32 pos;

  const RAIL_CAMERA_SET * camera_set;
  const char * name;
};


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
struct _RAIL_LINE { 
  //point_s --> point_e�ւ̓�����key�Ő��䂷��
  const RAIL_POINT * point_s;
  const RAIL_POINT * point_e;
  RAIL_KEY key;
  const RAIL_LINEPOS_SET * line_pos_set;

  /// ������ł̓���
  u32 line_divider;

  const RAIL_CAMERA_SET * camera_set;

  const char * name;
  //������K�v�ɂȂ邪���l����ƍ�������
  //fx32 width_margin;  ///<LINE�ɑ΂��ē����镝
  //u32 width_divider;
};



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
extern void FIELD_RAIL_MAN_Load(FIELD_RAIL_MAN * man, const RAIL_POINT * railPointData);

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


//-----------------------------------------------------------------------------
//  �~����
//-----------------------------------------------------------------------------
extern void FIELD_RAIL_POSFUNC_CircleLine( const FIELD_RAIL * rail, VecFx32 * pos );
extern void FIELD_RAIL_POSFUNC_CircleCamera( const FIELD_RAIL_MAN * man );

