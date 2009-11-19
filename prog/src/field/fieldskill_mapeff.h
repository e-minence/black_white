//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fieldskill_mapeff.h
 *	@brief  �t�B�[���h�Z�@�]�[���G�t�F�N�g
 *	@author	tomoya takahashi
 *	@date		2009.11.19
 *
 *	���W���[�����FFIELDSKILL_MAPEFF
 *
 *
 *	�t���b�V���@��@����΂炢�@�Ȃǉ�ʑS�̂𕢂��@�t�B�[���h�Z���ʂ��Ǘ�
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "field_flash.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�B�[���h�X�L���@���ʐݒ�}�X�N
//=====================================
typedef enum 
{
  FIELDSKILL_MAPEFF_MSK_FLASH_NEAR   = 1 << 0,    // �t���b�V�����ʁ@NEAR���
  FIELDSKILL_MAPEFF_MSK_FLASH_FAR    = 1 << 1,    // �t���b�V�����ʁ@FAR���
  
} FIELDSKILL_MAPEFF_MSK;


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�B�[���h�X�L���@�]�[�����Ƃ̌���
//=====================================
typedef struct _FIELDSKILL_MAPEFF FIELDSKILL_MAPEFF;


//-------------------------------------
///	�t�B�[���h�X�L���@�]�[�����Ƃ̌��ʃf�[�^
//=====================================
typedef struct 
{
  u16 zone_id;
  u16 msk;    // FIELDSKILL_MAPEFF_MSK
} FIELDSKILL_MAPEFF_DATA;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// �����E�j��
extern FIELDSKILL_MAPEFF* FIELDSKILL_MAPEFF_Create( FIELDSKILL_MAPEFF_MSK msk, HEAPID heapID );
extern void FIELDSKILL_MAPEFF_Delete( FIELDSKILL_MAPEFF* p_wk );

// ���C��
extern void FIELDSKILL_MAPEFF_Main( FIELDSKILL_MAPEFF* p_wk );
extern void FIELDSKILL_MAPEFF_Draw( FIELDSKILL_MAPEFF* p_wk );


// ���ꂼ��̃��[�N�擾
extern BOOL FIELDSKILL_MAPEFF_IsFlash( const FIELDSKILL_MAPEFF* cp_wk );
extern FIELD_FLASH* FIELDSKILL_MAPEFF_GetFlash( const FIELDSKILL_MAPEFF* cp_wk );

#ifdef _cplusplus
}	// extern "C"{
#endif



