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
  FIELD_WFBC_CORE mapdata;
  FIELD_WFBC_CORE_ITEM item;

  FIELD_WFBC_EVENT event;
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
  // �N���A
  FIELD_WFBC_CORE_Clear( &randomMapSave->mapdata );
  WFBC_CORE_ITEM_ClaerAll( &randomMapSave->item );

}

//----------------------------------------------------------
//	�f�[�^����̂��߂̊֐�
//----------------------------------------------------------
RANDOMMAP_SAVE* RANDOMMAP_SAVE_GetRandomMapSave( SAVE_CONTROL_WORK *sv )
{
	return (RANDOMMAP_SAVE*)SaveControl_DataPtrGet( sv , GMDATA_ID_RANDOMMAP );
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC��{����ݒ�
 *
 *	@param	sv        �Z�[�u���[�N
 *	@param	cp_buff   ��{���
 */
//-----------------------------------------------------------------------------
void RANDOMMAP_SAVE_SetCoreWork( RANDOMMAP_SAVE* sv, const FIELD_WFBC_CORE* cp_buff )
{
  GF_ASSERT( sv );
  GF_ASSERT( cp_buff );
  GFL_STD_MemCopy( cp_buff, &sv->mapdata, sizeof(FIELD_WFBC_CORE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC��{�����擾����
 *
 *	@param	sv        �Z�[�u���[�N
 *	@param	p_buff    �o�b�t�@
 */
//-----------------------------------------------------------------------------
void RANDOMMAP_SAVE_GetCoreWork( const RANDOMMAP_SAVE* sv, FIELD_WFBC_CORE* p_buff )
{
  GF_ASSERT( sv );
  GF_ASSERT( p_buff );
  
  GFL_STD_MemCopy( &sv->mapdata, p_buff, sizeof(FIELD_WFBC_CORE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�C�e���z�u�����擾
 *
 *	@param	sv        ���[�N
 *	@param	p_buff    �i�[�o�b�t�@
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_ITEM* RANDOMMAP_SAVE_GetItemData( RANDOMMAP_SAVE* sv )
{
  GF_ASSERT( sv );
  return &sv->item;
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�C�x���g�Z�[�u�f�[�^���擾
 *
 *	@param	sv  �Z�[�u���[�N
 *
 *	@return �Z�[�u�f�[�^
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_EVENT* RANDOMMAP_SAVE_GetEventData( RANDOMMAP_SAVE* sv )
{
  GF_ASSERT( sv );
  return &sv->event;
}



