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


//==============================================================================
//  �萔��`
//==============================================================================
///�p���X�}�b�v�͈̔�(���̍��W�O�ɏo���ꍇ�A���[�v������K�v������)
enum{
  PALACE_MAP_LEFT = 0 << FX32_SHIFT,              ///<�p���X�}�b�v���[
  PALACE_MAP_RIGHT = 3072 << FX32_SHIFT,          ///<�p���X�}�b�v�E�[
  PALACE_MAP_LEN = PALACE_MAP_RIGHT - PALACE_MAP_LEFT,  ///<�p���X�}�b�v��

  PALACE_MAP_RANGE_LEFT_X = PALACE_MAP_LEFT + (32 << FX32_SHIFT),
  PALACE_MAP_RANGE_RIGHT_X = PALACE_MAP_RIGHT - (32 << FX32_SHIFT),

  PALACE_MAP_RANGE_LEN = (2056-1016) << FX32_SHIFT,     ///<�p���X�}�b�v��X��
};

//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void IntrudeField_UpdateCommSystem( FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);
extern BOOL IntrudeField_CheckTalk(INTRUDE_COMM_SYS_PTR intcomm, const FIELD_PLAYER *fld_player, u32 *hit_netid);
extern BOOL IntrudeField_CheckTalkedTo(INTRUDE_COMM_SYS_PTR intcomm, u32 *hit_netid);
extern void IntrudeField_ConnectMap(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, INTRUDE_COMM_SYS_PTR intcomm);


//--------------------------------------------------------------
//  �f�o�b�O�p
//--------------------------------------------------------------
extern GMEVENT * DEBUG_IntrudeTreeMapWarp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor, INTRUDE_COMM_SYS_PTR intcomm);
extern GMEVENT * DEBUG_PalaceJamp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);
