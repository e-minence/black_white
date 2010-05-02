//==============================================================================
/**
 * @file    intrude_field.h
 * @brief   �N���Ńt�B�[���h���ɍs�����C���̃w�b�_
 * @author  matsuda
 * @date    2009.09.03(��)
 */
//==============================================================================
#pragma once

#include "field/field_comm/intrude_types.h"
#include "field/field_const.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�p���X�}�b�v�͈̔�(���̍��W�O�ɏo���ꍇ�A���[�v������K�v������)
enum{
  PALACE_MAP_LEFT = 0 << FX32_SHIFT,              ///<�p���X�}�b�v���[
  PALACE_MAP_RIGHT = 1024 << FX32_SHIFT,          ///<�p���X�}�b�v�E�[
  PALACE_MAP_LEN = PALACE_MAP_RIGHT - PALACE_MAP_LEFT,  ///<�p���X�}�b�v��
  PALACE_MAP_LEN_GRID = PALACE_MAP_LEN / FIELD_CONST_GRID_FX32_SIZE,
  
  PALACE_MAP_WARP_OFFSET = 0, //(PALACE_MAP_LEN / 4) * 3,///�c�肱��ȓ��̍��W�ɂȂ����烏�[�v
  
//  PALACE_MAP_RANGE_LEN = (2056-1016) << FX32_SHIFT,     ///<�p���X�}�b�v��X��
  
  PALACE_MAP_TREE_X = 504 << FX32_SHIFT,            ///<�p���X�̖� �z�u���W
  PALACE_MAP_TREE_Y = 32 << FX32_SHIFT,
  PALACE_MAP_TREE_Z = 440 << FX32_SHIFT,
  PALACE_MAP_MONOLITH_X = 488 << FX32_SHIFT,       ///<���m���X�z�u�J�n���WX
  PALACE_MAP_MONOLITH_Y = 32,
  PALACE_MAP_MONOLITH_Z = 472 << FX32_SHIFT,
  PALACE_MAP_MONOLITH_NUM = 3,                      ///<���m���X�̘A���ݒu��
};

//==============================================================================
//  �\���̒�`
//==============================================================================
///�p���X�]�[���ݒ�f�[�^
typedef struct{
  u32 zone_id:10;         ///<���̃f�[�^�Ώۂ̃]�[��ID
  u32 reverse_zone_id:10; ///<���̃f�[�^�Ώۂ̃]�[��ID�̗��]�[��ID
  u32 warp_zone_id:10;    ///<���[�v��]�[��ID
  u32             :2;
  s16 warp_grid_x;        ///<���[�v��X���W
  s16 warp_grid_y;        ///<���[�v��Y���W
  s16 warp_grid_z;        ///<���[�v��Z���W
  u8 sub_x;               ///<����ʍ��WX
  u8 sub_y;               ///<����ʍ��WY
}PALACE_ZONE_SETTING;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void IntrudeField_UpdateCommSystem( FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);
extern BOOL IntrudeField_CheckTalk(INTRUDE_COMM_SYS_PTR intcomm, const FIELD_PLAYER *fld_player, u32 *hit_netid);
extern BOOL IntrudeField_CheckTalkedTo(INTRUDE_COMM_SYS_PTR intcomm, u32 *hit_netid);
extern void IntrudeField_ConnectMapInit(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, INTRUDE_COMM_SYS_PTR intcomm);
extern void IntrudeField_ConnectMap(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, INTRUDE_COMM_SYS_PTR intcomm);
extern void IntrudeField_ConnectMapOne(FIELDMAP_WORK *fieldWork);
extern void IntrudeField_PlayerDisguise(INTRUDE_COMM_SYS_PTR intcomm, GAMESYS_WORK *gsys, u32 disguise_code, u8 disguise_type, u8 disguise_sex);
extern GMEVENT * IntrudeField_CheckSecretItemEvent(GAMESYS_WORK *gsys, INTRUDE_COMM_SYS_PTR intcomm, const FIELD_PLAYER *pcActor);
extern GMEVENT * Intrude_CheckPosEvent(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);
extern GMEVENT * Intrude_CheckPushEvent(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *pcActor, const VecFx32 *now_pos, const VecFx32 *front_pos, u16 player_dir);
extern BOOL IntrudeField_GetPalaceTownZoneID(u16 zone_id, VecFx32 *vec);
extern const PALACE_ZONE_SETTING * IntrudeField_GetZoneSettingData(u16 zone_id);
extern void IntrudeField_MonolithStatus_Init(MONOLITH_STATUS *monost);
extern BOOL IntrudeField_MonolithStatus_CheckOcc(const MONOLITH_STATUS *monost);
extern void IntrudeField_PalaceMMdlAllAdd(FIELDMAP_WORK *fieldWork);
extern BOOL IntrudeField_CheckIntrudeShutdown(GAMESYS_WORK *gsys, u16 zone_id);


//--------------------------------------------------------------
//  �f�o�b�O�p
//--------------------------------------------------------------
extern GMEVENT * DEBUG_PalaceJamp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);

#ifdef PM_DEBUG
extern GMEVENT * DEBUG_EVENT_PalaceBarrierMove(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir);
#endif
