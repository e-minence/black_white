//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_flash.h
 *	@brief  �t���b�V�����[�N
 *	@author	tomoya takahashi
 *	@date		2009.11.18
 *
 *	���W���[�����FFIELD_FLASH
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�t���b�V���@���
//=====================================
typedef enum 
{
  FIELD_FLASH_STATUS_NONE,      // �����Ȃ�
  FIELD_FLASH_STATUS_NEAR,      // ���o����
  FIELD_FLASH_STATUS_FADEOUT,   // �t�F�[�h�A�E�g��
  FIELD_FLASH_STATUS_FAR,       // ���o�L��
} FIELD_FLASH_STATUS;

//-------------------------------------
///	�t���b�V���@���N�G�X�g
//=====================================
typedef enum 
{
  FIELD_FLASH_REQ_ON_NEAR,    // ���o������ݒ�
  FIELD_FLASH_REQ_FADEOUT,    // �t�F�[�h�A�E�g
  FIELD_FLASH_REQ_ON_FAR,     // �L����ԂŐݒ�
  FIELD_FLASH_REQ_OFF,        // ���S�ɏ���


  FIELD_FLASH_REQ_MAX,  
} FIELD_FLASH_REQ;

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t���b�V���Ǘ����[�N
//=====================================
typedef struct _FIELD_FLASH FIELD_FLASH;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// �����@�j��
extern FIELD_FLASH* FIELD_FLASH_Create( HEAPID heapID );
extern void FIELD_FLASH_Delete( FIELD_FLASH* p_wk );

// �X�V
extern void FIELD_FLASH_Update( FIELD_FLASH* p_wk );
extern void FIELD_FLASH_Draw( FIELD_FLASH* p_wk );

// ON OFF
extern void FIELD_FLASH_Control( FIELD_FLASH* p_wk, FIELD_FLASH_REQ req );

// ��Ԏ擾


#ifdef _cplusplus
}	// extern "C"{
#endif



