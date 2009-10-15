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
#include "field/field_rail_loader.h"
#include "field/rail_location.h"


//------------------------------------------------------------------
// ���[���V�X�e���AWB�ł̃��[�N��
//------------------------------------------------------------------
#define FIELD_RAIL_WORK_MAX (48)


//------------------------------------------------------------------
// �����I�t�Z�b�g
// 1���́ARAIL_WALK_OFS���̋����ɂȂ�܂��B
//------------------------------------------------------------------
#define RAIL_WALK_OFS	(16)


//------------------------------------------------------------------
// RAIL�O���b�h�@�Ɓ@�I�t�Z�b�g�@�ϊ�
//------------------------------------------------------------------
// RAIL_GRID -> OFS
#define RAIL_GRID_TO_OFS(x)  ( (x) * RAIL_WALK_OFS )
// RAIL_ OFS -> GRID
#define RAIL_OFS_TO_GRID(x)  ( (x) / RAIL_WALK_OFS )


//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _FIELD_RAIL_MAN FIELD_RAIL_MAN;

//------------------------------------------------------------------
// 1�̃��[�����샏�[�N
//------------------------------------------------------------------
typedef struct _FIELD_RAIL_WORK FIELD_RAIL_WORK;

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
#define RAIL_CAMERA_SET_WORK_SIZ	(32)
typedef struct _RAIL_CAMERA_SET{
  u32 func_index;
	union
	{
		struct
		{
			u32 param0;
			u32 param1;
			u32 param2;
			u32 param3;
			u32 param4;
			u32 param5;
			u32 param6;
			u32 param7;
		};
		u8 work[RAIL_CAMERA_SET_WORK_SIZ];
	};
}RAIL_CAMERA_SET;

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
#define RAIL_LINEPOS_SET_WORK_SIZ	(32)
typedef struct _RAIL_LINEPOS_SET{
  u32 func_index;
  u32 func_dist_index;
	union
	{
		struct
		{
			u32 param0;
			u32 param1;
			u32 param2;
			u32 param3;
			u32 param4;
			u32 param5;
			u32 param6;
			u32 param7;
		};
		u8 work[RAIL_CAMERA_SET_WORK_SIZ];
	};
}RAIL_LINEPOS_SET;



//------------------------------------------------------------------
//------------------------------------------------------------------
typedef void (RAIL_POS_FUNC)(const FIELD_RAIL_WORK * work, VecFx32 * pos);
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
 * @brief   �P�����x�w��
 *
 * �O������A�ړ����x�A���w�肷�邽�߂̃R�}���h
 */
//------------------------------------------------------------------
typedef enum {  
  RAIL_FRAME_16,
  RAIL_FRAME_8,
  RAIL_FRAME_4,
  RAIL_FRAME_2,
  RAIL_FRAME_1,

  RAIL_FRAME_MAX,
}RAIL_FRAME;


//------------------------------------------------------------------
/**
 * @brief �e�[�u���C���f�b�N�X�����l
 */
//------------------------------------------------------------------
#define RAIL_TBL_NULL	( 0xffffffff )


//-----------------------------------------------------------------------------
/**
 *		���̃T�C�Y
 */
//-----------------------------------------------------------------------------
#define RAIL_NAME_BUF_SIZ	( 48 )

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
  u32 keys[RAIL_CONNECT_LINE_MAX];
	s32	width_ofs_max[RAIL_CONNECT_LINE_MAX];

  ///POINT�̈ʒu���W
  VecFx32 pos;

	u32 camera_set;
  char name[ RAIL_NAME_BUF_SIZ ];
};


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
struct _RAIL_LINE {
  //point_s --> point_e�ւ̓�����key�����𐳂Ƃ��Đ��䂷��
	u32 point_s;
	u32 point_e;
  u32 key;
	u32 line_pos_set;

	u32 camera_set;


  char name[ RAIL_NAME_BUF_SIZ ];
  //������K�v�ɂȂ邪���l����ƍ�������
  //fx32 width_margin;  ///<LINE�ɑ΂��ē����镝
  //u32 width_divider;
};


//------------------------------------------------------------------
/// ���[�����
//------------------------------------------------------------------
struct _RAIL_SETTING{
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

  fx32  ofs_unit;     // ���̈ړ��P��
} ;



//============================================================================================
//
//
//  ���[���V�X�e������̂��߂̊֐��Q
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
extern FIELD_RAIL_MAN * FIELD_RAIL_MAN_Create(HEAPID heapID, u32 work_num, FIELD_CAMERA * camera);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Delete(FIELD_RAIL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Load(FIELD_RAIL_MAN * man, const RAIL_SETTING * setting);
extern void FIELD_RAIL_MAN_Clear(FIELD_RAIL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern BOOL FIELD_RAIL_MAN_IsLoad( const FIELD_RAIL_MAN * man );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_Update(FIELD_RAIL_MAN * man);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern BOOL FIELD_RAIL_MAN_UpdateCamera(FIELD_RAIL_MAN * man);


//------------------------------------------------------------------
// ��ƂȂ郌�[���O���b�h�T�C�Y�̎擾
// *�Ȃ���񂳂Ȃǂ����邽�߁A�K�����̒l�ł͂Ȃ�
//------------------------------------------------------------------
extern fx32 FIELD_RAIL_MAN_GetRailGridSize( const FIELD_RAIL_MAN * man );

// ���P�[�V�����ł̌v�Z
extern BOOL FIELD_RAIL_MAN_CalcRailKeyPos(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * now_location, RAIL_KEY key, VecFx32* pos);
extern BOOL FIELD_RAIL_MAN_CalcRailKeyLocation(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * now_location, RAIL_KEY key, RAIL_LOCATION * next_location);
extern void FIELD_RAIL_MAN_GetLocationPosition(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * location, VecFx32* pos );
extern u32 FIELD_RAIL_MAN_GetLocationLineOfsMaxGrid(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * location );
extern u32 FIELD_RAIL_MAN_GetLocationWidthGrid(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * location );

//------------------------------------------------------------------
// �ړ��Ǘ�1���̂̐����E�j��
//------------------------------------------------------------------
extern FIELD_RAIL_WORK* FIELD_RAIL_MAN_CreateWork( FIELD_RAIL_MAN * man );
extern void FIELD_RAIL_MAN_DeleteWork( FIELD_RAIL_MAN * man, FIELD_RAIL_WORK* work );

//------------------------------------------------------------------
// �o�C���h����FIELD_RAIL_WORK�̐ݒ�
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_BindCamera( FIELD_RAIL_MAN * man, const FIELD_RAIL_WORK* work );
extern void FIELD_RAIL_MAN_UnBindCamera( FIELD_RAIL_MAN * man );
extern void FIELD_RAIL_MAN_GetBindWorkPos( const FIELD_RAIL_MAN * man, VecFx32* pos );
extern BOOL FIELD_RAIL_MAN_IsBindWorkMove( const FIELD_RAIL_MAN * man );

#ifdef PM_DEBUG
// ������ �F�X�ȕ����ŁARAILMAN����Location�Ȃǂ��擾���Ă��邽�߁A�쐬
// ������g�p���Ă��镔���͌�X�́APLAYER�̏�񂩂�擾�ł���悤�ɂȂ�͂�
extern FIELD_RAIL_WORK* FIELD_RAIL_MAN_DEBUG_GetBindWork( const FIELD_RAIL_MAN * man );

// �f�[�^�\�����ς���Ă��Ȃ�binary�́A������Ƃ����f�[�^�ύX
extern void FIELD_RAIL_MAN_DEBUG_ChangeData(FIELD_RAIL_MAN * man, const RAIL_SETTING * setting);
#endif

//------------------------------------------------------------------
//  ���Ԃ�f�o�b�O�p�r�̂�
//------------------------------------------------------------------
extern void FIELD_RAIL_MAN_SetActiveFlag(FIELD_RAIL_MAN * man, BOOL flag);
extern BOOL FIELD_RAIL_MAN_GetActiveFlag(const FIELD_RAIL_MAN *man);


//============================================================================================
//
//
//  ���[���ړ�����̂��߂̊֐��Q
//
//
//============================================================================================
//------------------------------------------------------------------
// �P���ړ����N�G�X�g
//------------------------------------------------------------------
extern BOOL FIELD_RAIL_WORK_ForwardReq( FIELD_RAIL_WORK * work, RAIL_FRAME frame, RAIL_KEY key );

extern void FIELD_RAIL_WORK_SetLocation(FIELD_RAIL_WORK * work, const RAIL_LOCATION * location);
extern void FIELD_RAIL_WORK_GetLocation(const FIELD_RAIL_WORK * work, RAIL_LOCATION * location);
extern void FIELD_RAIL_WORK_GetLastLocation(const FIELD_RAIL_WORK * work, RAIL_LOCATION * location);
extern BOOL FIELD_RAIL_WORK_CheckLocation( const FIELD_RAIL_WORK * work, const RAIL_LOCATION * location );


//------------------------------------------------------------------
//  ����A�b�v�f�[�g
//------------------------------------------------------------------
extern void FIELD_RAIL_WORK_Update(FIELD_RAIL_WORK * work);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_RAIL_WORK_GetPos(const FIELD_RAIL_WORK * work, VecFx32 * pos);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern RAIL_FRAME FIELD_RAIL_WORK_GetActionFrame( const FIELD_RAIL_WORK * work );
extern RAIL_KEY FIELD_RAIL_WORK_GetActionKey( const FIELD_RAIL_WORK * work );
extern BOOL FIELD_RAIL_WORK_IsAction( const FIELD_RAIL_WORK * work );
extern BOOL FIELD_RAIL_WORK_IsLastAction( const FIELD_RAIL_WORK * work );


//------------------------------------------------------------------
// ���[�N�P�ʂ̓���ݒ�
//------------------------------------------------------------------
extern void FIELD_RAIL_WORK_SetActiveFlag( FIELD_RAIL_WORK * work, BOOL flag );
extern BOOL FIELD_RAIL_WORK_IsActive( const FIELD_RAIL_WORK * work );



//------------------------------------------------------------------
// ����n�c�[��
//------------------------------------------------------------------
extern BOOL FIELD_RAIL_TOOL_HitCheckSphere( const VecFx32* person, const VecFx32* check, fx32 r );

 
#ifdef PM_DEBUG
// ���[���O���b�h�f�o�b�N�o��
extern void FIELD_RAIL_WORK_DEBUG_PrintRailGrid( const FIELD_RAIL_WORK * work );
#endif
