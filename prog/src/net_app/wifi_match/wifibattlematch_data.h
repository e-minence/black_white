//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_data.h
 *	@brief  �f�[�^�o�b�t�@
 *	@author	Toru=Nagihashi
 *	@date		2009.12.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "poke_tool/pokeparty.h"
#include "buflen.h"
#include "system/pms_data.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���[�N
//=====================================
typedef struct _WIFIBATTLEMATCH_DATA_WORK WIFIBATTLEMATCH_DATA_WORK;

//-------------------------------------
/// �ۑ��f�[�^
//=====================================

//�ΐ�ҏ��
typedef struct 
{
  STRCODE   name[PERSON_NAME_SIZE+EOM_SIZE];
  u8        sex;
  u8        trainer_view;
  u8        nation;
  u8        area;
  u32       rate;
  PMS_DATA  pms;
  u32       pokeparty[0]; //����Ƀ|�P�p�[�e�B�̃f�[�^����
} WIFIBATTLEMATCH_ENEMYDATA;
//�ΐ�ҏ��̃T�C�Y
#define WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE (sizeof(WIFIBATTLEMATCH_ENEMYDATA) + PokeParty_GetWorkSize())

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//���[�N�쐬
extern WIFIBATTLEMATCH_DATA_WORK *WIFIBATTLEMATCH_DATA_Init( HEAPID heapID );
extern void WIFIBATTLEMATCH_DATA_Exit( WIFIBATTLEMATCH_DATA_WORK *p_wk );

//�f�[�^�擾�A�ݒ�
extern WIFIBATTLEMATCH_ENEMYDATA * WIFIBATTLEMATCH_DATA_GetPlayerDataPtr( WIFIBATTLEMATCH_DATA_WORK *p_wk );
extern WIFIBATTLEMATCH_ENEMYDATA * WIFIBATTLEMATCH_DATA_GetEnemyDataPtr( WIFIBATTLEMATCH_DATA_WORK *p_wk );
