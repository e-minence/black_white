//=============================================================================
/**
 * @file	location.h	
 * @brief	�t�B�[���h�}�b�v�V�X�e��
 * @author	tamada GAME FREAK inc.
 * @date	2005.12.16
 *
 * 2008.11.21	DP�v���W�F�N�g����ڐA
 */
//=============================================================================

#pragma once

#include "field/rail_location.h"
#include "field/eventdata_type.h" //EXIT_DIR

//=============================================================================
//=============================================================================
//--------------------------------------------------------------
//  ���P�[�V�����I�t�Z�b�g
//--------------------------------------------------------------
enum {
  LOCATION_DEFAULT_EXIT_OFS = 0,
};
// 4bit                  4bit
// �o�����O���b�h�T�C�Y�@�I�t�Z�b�g
typedef u16 LOC_EXIT_OFS;

#define LOC_EXIT_OFS_DIR_SHIFT    (8)
#define LOC_EXIT_OFS_SIZE_SHIFT   (4)
#define LOC_EXIT_OFS_OFS_SHIFT    (0)
#define LOC_EXIT_OFS_DIR_MSK    (0xf00)
#define LOC_EXIT_OFS_SIZE_MSK   (0xf0)
#define LOC_EXIT_OFS_OFS_MSK    (0x0f)

#define LOC_EXIT_OFS_GET_DIR( loc_exit_ofs )   ( (loc_exit_ofs & LOC_EXIT_OFS_DIR_MSK) >> LOC_EXIT_OFS_DIR_SHIFT )
#define LOC_EXIT_OFS_GET_SIZE( loc_exit_ofs )   ( (loc_exit_ofs & LOC_EXIT_OFS_SIZE_MSK) >> LOC_EXIT_OFS_SIZE_SHIFT )
#define LOC_EXIT_OFS_GET_OFS( loc_exit_ofs )   ( (loc_exit_ofs & LOC_EXIT_OFS_OFS_MSK) >> LOC_EXIT_OFS_OFS_SHIFT )
#define LOC_EXIT_OFS_DEF( size, ofs, dir )   ( (size << LOC_EXIT_OFS_SIZE_SHIFT) | (ofs << LOC_EXIT_OFS_OFS_SHIFT) | (dir << LOC_EXIT_OFS_DIR_SHIFT) )



//--------------------------------------------------------------
//--------------------------------------------------------------
typedef enum {
	LOCATION_POS_TYPE_3D = 0, // 3D
	LOCATION_POS_TYPE_RAIL,   // RAIL

	LOCATION_POS_TYPE_MAX,
}LOCATION_POS_TYPE;

//--------------------------------------------------------------
// ���P�[�V�����|�W�V����
// �RD���W�ƃ��[�����P�[�V�����̏��������܂��B
//--------------------------------------------------------------
typedef struct 
{
  LOCATION_POS_TYPE type;
  union{
    //  3D�f�[�^
    VecFx32 pos;
    // ���[���f�[�^
    RAIL_LOCATION railpos;
  };
} LOCATION_POS;



//--------------------------------------------------------------
//--------------------------------------------------------------
typedef enum {
	LOCATION_TYPE_INIT = 0,
	LOCATION_TYPE_DIRECT,
	LOCATION_TYPE_EXITID,
	LOCATION_TYPE_SPID,
}LOCATION_TYPE;
//--------------------------------------------------------------
///	LOCATION�\����
//--------------------------------------------------------------
typedef struct {
	LOCATION_TYPE type;
	s16 zone_id;
	s16 exit_id;
	s16 dir_id;
  LOC_EXIT_OFS exit_ofs;
	LOCATION_POS location_pos;
}LOCATION;

//=============================================================================
//=============================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
extern void LOCATION_SetDefaultPos(LOCATION * loc, u16 zone_id);

//--------------------------------------------------------------
/**
 * @brief	�Q�[���J�n���̈ʒu�擾
 */
//--------------------------------------------------------------
extern void LOCATION_SetGameStart(LOCATION * loc);

//--------------------------------------------------------------
/**
 * @brief	LOCATION�̏�����
 */
//--------------------------------------------------------------
extern void LOCATION_Init(LOCATION * loc);

//--------------------------------------------------------------
/**
 * @brief	LOCATION�̃Z�b�g
 */
//--------------------------------------------------------------
extern void LOCATION_Set(LOCATION * loc, int zone, s16 door, s16 dir, LOC_EXIT_OFS ofs, fx32 x, fx32 y, fx32 z);
extern void LOCATION_SetRail(LOCATION * loc, int zone, s16 door, s16 dir, LOC_EXIT_OFS ofs, u16 rail_index, u16 line_grid, s16 width_grid);

//--------------------------------------------------------------
/**
 * @brief	LOCATION�̃Z�b�g�izone_id,exit_id�̂�)
 */
//--------------------------------------------------------------
extern void LOCATION_SetID(LOCATION * loc, u16 zone_id, u16 exit_id, u16 ofs);
//--------------------------------------------------------------
/**
 * @brief	LOCATION�̃Z�b�g�i���ڈʒu�w��j
 */
//--------------------------------------------------------------
extern void LOCATION_SetDirect(LOCATION * loc, int zone, s16 dir, fx32 x, fx32 y, fx32 z);
extern void LOCATION_SetDirectRail(LOCATION * loc, int zone, s16 dir, u16 rail_index, u16 line_grid, s16 width_grid);

//--------------------------------------------------------------
/**
 * @brief	LOCATION���̍��W�^�C�v���擾
 */
//--------------------------------------------------------------
extern LOCATION_POS_TYPE LOCATION_GetPosType( const LOCATION * loc );

//--------------------------------------------------------------
/**
 * @brief	LOCATION����A�RD���W�̐ݒ�E�擾
 */
//--------------------------------------------------------------
extern void LOCATION_Set3DPos( LOCATION * loc, const VecFx32 * pos );
extern void LOCATION_Get3DPos( const LOCATION * loc, VecFx32 * pos );

//--------------------------------------------------------------
/**
 * @brief	LOCATION����A���[�����P�[�V�����̐ݒ�E�擾
 */
//--------------------------------------------------------------
extern void LOCATION_SetRailLocation( LOCATION * loc, const RAIL_LOCATION * location );
extern void LOCATION_GetRailLocation( const LOCATION * loc, RAIL_LOCATION * location );


extern u16 EXITDIR_toDIR( EXIT_DIR exit_dir );
extern EXIT_DIR EXITDIR_fromDIR( u16 dir );


