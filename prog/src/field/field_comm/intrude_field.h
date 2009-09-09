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
//  �O���֐��錾
//==============================================================================
extern void IntrudeField_UpdateCommSystem( FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);


//--------------------------------------------------------------
//  �f�o�b�O�p
//--------------------------------------------------------------
extern GMEVENT * DEBUG_IntrudeTreeMapWarp(FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor, INTRUDE_COMM_SYS_PTR intcomm);
extern GMEVENT * DEBUG_PalaceJamp(FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);
