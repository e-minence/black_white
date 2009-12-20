//==============================================================================
/**
 * @file    intrude_minimono.h
 * @brief   �~�j���m���X�����̃w�b�_
 * @author  matsuda
 * @date    2009.10.26(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void MINIMONO_AddMMdl(FIELDMAP_WORK *fieldWork, u16 grid_x, u16 grid_z, fx32 fx_y);
extern void MINIMONO_AddPosRand(GAME_COMM_SYS_PTR game_comm, FIELDMAP_WORK *fieldWork);

//--------------------------------------------------------------
//  �f�o�b�O
//--------------------------------------------------------------
#ifdef PM_DEBUG
extern void DEBUG_INTRUDE_Pokemon_Add(FIELDMAP_WORK *fieldWork);
extern void DEBUG_INTRUDE_BingoPokeSet(FIELDMAP_WORK *fieldWork);
#endif
