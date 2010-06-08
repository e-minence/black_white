//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		rndmatch_savedata.h
 *	@brief	�����_���o�g���}�b�`�p�Z�[�u�f�[�^
 *	@author	Toru=Nagihashi
 *	@date		2009.12.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�擾�ł������
//=====================================
typedef enum
{
  RNDMATCH_PARAM_IDX_RATE,
  RNDMATCH_PARAM_IDX_WIN,
  RNDMATCH_PARAM_IDX_LOSE,
  
  RNDMATCH_PARAM_IDX_MAX
} RNDMATCH_PARAM_IDX;

//-------------------------------------
///	���
//=====================================
typedef enum
{
  RNDMATCH_TYPE_FREE_SINGLE,
  RNDMATCH_TYPE_FREE_DOUBLE,
  RNDMATCH_TYPE_FREE_TRIPLE,
  RNDMATCH_TYPE_FREE_ROTATE,
  RNDMATCH_TYPE_FREE_SHOOTER,
  RNDMATCH_TYPE_RATE_SINGLE,
  RNDMATCH_TYPE_RATE_DOUBLE,
  RNDMATCH_TYPE_RATE_TRIPLE,
  RNDMATCH_TYPE_RATE_ROTATE,
  RNDMATCH_TYPE_RATE_SHOOTER,
  
  RNDMATCH_TYPE_WIFI_CUP,

  RNDMATCH_TYPE_MAX
} RNDMATCH_TYPE;

//-------------------------------------
/// ����
//=====================================
#define RNDMATCH_LIMIT_MAX  (65535)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�Z�[�u�f�[�^�s���S�^
//=====================================
typedef struct _RNDMATCH_DATA RNDMATCH_DATA;

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//----------------------------------------------------------
//�Z�[�u�f�[�^�V�X�e���Ɉˑ�����֐�
//----------------------------------------------------------
extern int RNDMATCH_GetWorkSize( void );
extern void RNDMATCH_Init( RNDMATCH_DATA *p_wk );

//���[�N�擾�́B�o�g�����p�Z�[�u�f�[�^battlematch_savedata.h�ɂ���܂�

//----------------------------------------------------------
//�擾�A���삷��֐�
//----------------------------------------------------------
extern u16 RNDMATCH_GetParam( const RNDMATCH_DATA *cp_wk, RNDMATCH_TYPE type, RNDMATCH_PARAM_IDX idx );
extern void RNDMATCH_SetParam( RNDMATCH_DATA *p_wk, RNDMATCH_TYPE type, RNDMATCH_PARAM_IDX idx, s32 num );
extern void RNDMATCH_AddParam( RNDMATCH_DATA *p_wk, RNDMATCH_TYPE type, RNDMATCH_PARAM_IDX idx );
