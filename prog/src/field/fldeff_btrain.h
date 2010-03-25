//======================================================================
/**
 * @file	fldeff_btrain.h
 * @brief	�t�B�[���h �o�g���g���C��
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// FLDEFF_BTRAIN_TYPE
//--------------------------------------------------------------
typedef enum
{
  FLDEFF_BTRAIN_TYPE_01, //�V���O���g���C��
  FLDEFF_BTRAIN_TYPE_02, //�X�[�p�[�V���O���g���C��
  FLDEFF_BTRAIN_TYPE_03, //�_�u���g���C��
  FLDEFF_BTRAIN_TYPE_04, //�X�[�p�[�_�u���g���C��
  FLDEFF_BTRAIN_TYPE_05, //�}���`�g���C��
  FLDEFF_BTRAIN_TYPE_06, //�X�[�p�[�}���`�g���C��
  FLDEFF_BTRAIN_TYPE_07, //WiFi�g���C��
  FLDEFF_BTRAIN_TYPE_08, //�g���C���^�E���p�g���C��
  FLDEFF_BTRAIN_TYPE_MAX,
}FLDEFF_BTRAIN_TYPE;

//--------------------------------------------------------------
/// FLDEFF_BTRAIN_ANIME_TYPE
//--------------------------------------------------------------
typedef enum
{
  FLDEFF_BTRAIN_ANIME_TYPE_START_GEAR, //�M�A�X�e�o���A�j���@������A����
  FLDEFF_BTRAIN_ANIME_TYPE_ARRIVAL, //�r���w�d�Ԑi���A�j��
  FLDEFF_BTRAIN_ANIME_TYPE_ARRIVAL_HOME, //�z�[���i���A�d�ԊJ���A�j��
  FLDEFF_BTRAIN_ANIME_TYPE_START_HOME,  //�z�[���o���A�j�� ������A����
  FLDEFF_BTRAIN_ANIME_TYPE_MAX,
}FLDEFF_BTRAIN_ANIME_TYPE;
  
//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern void * FLDEFF_BTRAIN_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_BTRAIN_Delete( FLDEFF_CTRL *fectrl, void *work );

extern void FLDEFF_BTRAIN_InitResource(
    FLDEFF_CTRL *fectrl, FLDEFF_BTRAIN_TYPE type );

extern FLDEFF_TASK * FLDEFF_BTRAIN_SetTrain(
    FLDEFF_CTRL *fectrl, FLDEFF_BTRAIN_TYPE type, const VecFx32 *pos );
extern void FLDEFF_BTRAIN_SetAnime(
    FLDEFF_TASK *task, FLDEFF_BTRAIN_ANIME_TYPE type );
extern BOOL FLDEFF_BTRAIN_CheckAnimeEnd( FLDEFF_TASK *task );
extern void FLDEFF_BTRAIN_SetVanishFlag( FLDEFF_TASK *task, BOOL flag );
