//============================================================================================
/**
 * @file	field_status_local.h
 * @brief �t�B�[���h�}�b�v�Ǘ����
 * @author  tamada GAMEFREAK inc.
 * @date    2009.10.03
 *
 * @note
 * field_status.h��field_status_local.h�Ƃ͑΂ɂȂ��Ă���B
 * field_status.h�́AFIELD_STATUS�Ƃ����I�u�W�F�N�g�������o�Ɏ�������A
 * �擾�����肷�邽�߂����̃w�b�_�B�i����gamedata.h�݂̂�z��j
 * field_status_local.h�́AFIELD_STATUS�̑���֐����܂܂��w�b�_�B
 * src/field�ȉ��̃\�[�X�t�@�C���݂̂ŃC���N���[�h����邱�Ƃ�z�肵�Ă���B
 */
//============================================================================================
#pragma once

#include "field/field_status.h"

//======================================================================
//======================================================================
//------------------------------------------------------------------
/**
 * @brief�}�b�v���[�h
 */
//------------------------------------------------------------------
typedef enum{
  MAPMODE_NORMAL,     ///<�ʏ���
  MAPMODE_INTRUDE,    ///<�N����
}MAPMODE;

//======================================================================
//======================================================================
//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern MAPMODE FIELD_STATUS_GetMapMode(const FIELD_STATUS * fldstatus);

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern void FIELD_STATUS_SetMapMode(FIELD_STATUS * fldstatus, MAPMODE map_mode);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_STATUS_SetFieldInitFlag( FIELD_STATUS * fldstatus, BOOL flag );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern BOOL FIELD_STATUS_GetFieldInitFlag( const FIELD_STATUS * fldstatus );


//------------------------------------------------------------------
// �t�B�[���h�Z�֘A
//------------------------------------------------------------------
//------------------------------------------------------------------
// �t�B�[���h�Z�@�t���b�V����ON�EOFF
//------------------------------------------------------------------
extern void FIELD_STATUS_SetFieldSkillFlash( FIELD_STATUS * fldstatus, BOOL flag );
extern BOOL FIELD_STATUS_IsFieldSkillFlash( const FIELD_STATUS * fldstatus );

//------------------------------------------------------------------
// �t�B�[���h�Z�@�}�b�v���ʏ��
// field/fieldskill_mapeff.h
//  FIELDSKILL_MAPEFF_MSK
//------------------------------------------------------------------
extern void FIELD_STATUS_SetFieldSkillMapEffectMsk( FIELD_STATUS * fldstatus, u32 msk );
extern u32 FIELD_STATUS_GetFieldSkillMapEffectMsk( const FIELD_STATUS * fldstatus );


