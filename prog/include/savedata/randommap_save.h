//============================================================================================
/**
 * @file	randommap_save.c
 * @brief	�����_�������}�b�v�p�Z�[�u�f�[�^
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 *
 * ���W���[�����FRANDOMMAP_SAVE
 */
//============================================================================================

#pragma once

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���

//======================================================================
//	typedef struct
//======================================================================

typedef struct _RANDOMMAP_SAVE RANDOMMAP_SAVE;

//======================================================================
//	proto
//======================================================================

//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int RANDOMMAP_SAVE_GetWorkSize(void);
extern void RANDOMMAP_SAVE_InitWork(RANDOMMAP_SAVE *randomMapSave);

//----------------------------------------------------------
//	�f�[�^����̂��߂̊֐�
//----------------------------------------------------------
extern RANDOMMAP_SAVE* RANDOMMAP_SAVE_GetRandomMapSave( SAVE_CONTROL_WORK *sv );

extern void RANDOMMAP_SAVE_SetCityLevel( RANDOMMAP_SAVE *randomMapSave , const u16 level );
extern const u16 RANDOMMAP_SAVE_GetCityLevel( RANDOMMAP_SAVE *randomMapSave );

