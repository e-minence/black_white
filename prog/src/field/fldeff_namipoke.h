//======================================================================
/**
 * @file	fldeff_namipoke.h
 * @brief	�t�B�[���h �g���|�P����
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
/// NAMIPOKE_JOINT �g���|�P�����A����Ƃ̍��W�ڑ�
//--------------------------------------------------------------
typedef enum
{
  NAMIPOKE_JOINT_OFF = 0, ///<�ڑ����Ȃ�
  NAMIPOKE_JOINT_ON, ///<�ڑ��B�h�������ɔ��f
  NAMIPOKE_JOINT_ONLY, ///<�ڑ��̂݁@�h�ꎩ�̂�����
}NAMIPOKE_JOINT;

//--------------------------------------------------------------
/// NAMIPOKE_EFFECT_TYPE �g���|�P�����G�t�F�N�g�@���
//--------------------------------------------------------------
typedef enum
{
  NAMIPOKE_EFFECT_TYPE_TAKI_SPLASH = 0, ///<���n����
  NAMIPOKE_EFFECT_TYPE_TAKI_START_F, ///<��o��J�n�G�t�F�N�g�@�O
  NAMIPOKE_EFFECT_TYPE_TAKI_LOOP_F, ///<��o��ړ����G�t�F�N�g�@�O
  NAMIPOKE_EFFECT_TYPE_TAKI_START_S, ///<��o��J�n�G�t�F�N�g�@��
  NAMIPOKE_EFFECT_TYPE_TAKI_LOOP_S, ///<��o��J�n�G�t�F�N�g�@��
  NAMIPOKE_EFFECT_TYPE_MAX,
}NAMIPOKE_EFFECT_TYPE;

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern void * FLDEFF_NAMIPOKE_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_NAMIPOKE_Delete( FLDEFF_CTRL *fectrl, void *work );

extern FLDEFF_TASK * FLDEFF_NAMIPOKE_SetMMdl( FLDEFF_CTRL *fectrl,
    u16 dir, const VecFx32 *pos, MMDL *mmdl, NAMIPOKE_JOINT joint );
extern void FLDEFF_NAMIPOKE_SetJointFlag( FLDEFF_TASK *task, NAMIPOKE_JOINT flag );

extern void * FLDEFF_NAMIPOKE_EFFECT_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_NAMIPOKE_EFFECT_Delete( FLDEFF_CTRL *fectrl, void *work );

extern FLDEFF_TASK * FLDEFF_NAMIPOKE_EFFECT_SetEffect( FLDEFF_CTRL *fectrl,
    NAMIPOKE_EFFECT_TYPE type, const FLDEFF_TASK *efftask_namipoke );
extern FLDEFF_TASK * FLDEFF_NAMIPOKE_EFFECT_SetEffectAlone( FLDEFF_CTRL *fectrl,
    NAMIPOKE_EFFECT_TYPE type, const VecFx32 *pos );
extern BOOL FLDEFF_NAMIPOKE_EFFECT_CheckTaskEnd( const FLDEFF_TASK *task );
