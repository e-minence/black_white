//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		battlematch_savedata.h
 *	@brief  �o�g�����p�Z�[�u�f�[�^
 *	        �i���Ƃ��Ƃ͕ʁX���������A�Z�[�u��256�A���C�����g�̂��ߋ��ʂɂ����j
 *	@author	Toru=Nagihashi
 *	@date		2010.03.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "save_control.h"

#include "rndmatch_savedata.h"
#include "livematch_savedata.h"
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
///	�Z�[�u�f�[�^�s���S�^
//=====================================
typedef struct _BATTLEMATCH_DATA BATTLEMATCH_DATA;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================

//----------------------------------------------------------
//�Z�[�u�f�[�^�V�X�e���Ɉˑ�����֐�
//----------------------------------------------------------
extern int BATTLEMATCH_GetWorkSize( void );
extern void BATTLEMATCH_Init( BATTLEMATCH_DATA *p_wk );

//----------------------------------------------------------
//�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern const BATTLEMATCH_DATA * SaveData_GetBattleMatchConst( const SAVE_CONTROL_WORK * cp_sv );
extern BATTLEMATCH_DATA * SaveData_GetBattleMatch( SAVE_CONTROL_WORK * p_sv );

//----------------------------------------------------------
//���f�[�^����̎擾
//----------------------------------------------------------
extern const RNDMATCH_DATA * BATTLEMATCH_GetRndMatchConst( const BATTLEMATCH_DATA * cp_sv );
extern RNDMATCH_DATA * BATTLEMATCH_GetRndMatch( BATTLEMATCH_DATA * p_sv );
extern const LIVEMATCH_DATA * BATTLEMATCH_GetLiveMatchConst( const BATTLEMATCH_DATA * cp_sv );
extern LIVEMATCH_DATA * BATTLEMATCH_GetLiveMatch( BATTLEMATCH_DATA * p_sv );
