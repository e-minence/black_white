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
#include <gflib.h>

#include "savedata/randommap_save.h"
#include "savedata/save_tbl.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _RANDOMMAP_SAVE
{
  u16 cityLevel;
  u8 type;  //�e�X�g�p�X�̎��
  u8 pad;
};


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
int RANDOMMAP_SAVE_GetWorkSize(void)
{
	return sizeof( RANDOMMAP_SAVE );
}

void RANDOMMAP_SAVE_InitWork(RANDOMMAP_SAVE *randomMapSave)
{
	randomMapSave->cityLevel = 5;
	randomMapSave->type = RMT_BLACK_CITY;
}

//----------------------------------------------------------
//	�f�[�^����̂��߂̊֐�
//----------------------------------------------------------
RANDOMMAP_SAVE* RANDOMMAP_SAVE_GetRandomMapSave( SAVE_CONTROL_WORK *sv )
{
	return (RANDOMMAP_SAVE*)SaveControl_DataPtrGet( sv , GMDATA_ID_RANDOMMAP );
}

void RANDOMMAP_SAVE_SetCityLevel( RANDOMMAP_SAVE *randomMapSave , const u16 level )
{
  randomMapSave->cityLevel = level;
}

const u16 RANDOMMAP_SAVE_GetCityLevel( RANDOMMAP_SAVE *randomMapSave )
{
  return randomMapSave->cityLevel;
}

//----------------------------------------------------------
//	�e�X�g�p�X�̎��
//----------------------------------------------------------
void RANDOMMAP_SAVE_SetCityType( RANDOMMAP_SAVE *randomMapSave , const u8 type )
{
  randomMapSave->type = type;
}

const u8 RANDOMMAP_SAVE_GetCityType( RANDOMMAP_SAVE *randomMapSave )
{
  return randomMapSave->type;
}
