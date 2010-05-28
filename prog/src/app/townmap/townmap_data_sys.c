//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		townmap_data_sys.c
 *	@brief	�^�E���}�b�v�f�[�^�ǂݍ���
 *	@author	Toru=Nagihashi
 *	@data		2009.07.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>

//archive
#include "arc_def.h"

//mine
#include "app/townmap_data_sys.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�u�������f�[�^
//=====================================
typedef enum
{
  TOWNMAP_REPLACE_PARAM_DST_ZONE_ID,  //�u��������]�[���O���[�v
  TOWNMAP_REPLACE_PARAM_SRC_ZONE_ID_00,  //�u���������]�[���O���[�v00
  TOWNMAP_REPLACE_PARAM_MAX,
} TOWNMAP_REPLACE_PARAM;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�f�[�^�{��
//=====================================
typedef struct 
{
	u16	param[TOWNMAP_DATA_PARAM_MAX];
} TOWNMAP_DATA_STRUCT;

//-------------------------------------
///	�u�������f�[�^�{��
//=====================================
typedef struct {
  u16 param[ TOWNMAP_REPLACE_PARAM_MAX ];
} TOWNMAP_REPLACE_STRUCT;


//-------------------------------------
///	�^�E���}�b�v�f�[�^
//=====================================
/*
typedef struct _TOWNMAP_DATA TOWNMAP_DATA;
*/

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^�A���P�[�g
 *
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	�}�b�v�f�[�^�n���h��
 */
//-----------------------------------------------------------------------------
TOWNMAP_DATA *TOWNMAP_DATA_Alloc( HEAPID heapID )
{	
	void *p_handle;
	p_handle	= GFL_ARC_UTIL_Load( ARCID_TOWNMAP_DATA, 
			0, FALSE, heapID );

	return p_handle;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^���
 *
 *	@param	TOWNMAP_DATA *p_wk	�}�b�v�f�[�^�n���h��
 */
//-----------------------------------------------------------------------------
void TOWNMAP_DATA_Free( TOWNMAP_DATA *p_wk )
{	
	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�f�[�^���擾
 *
 *	@param	const		TOWNMAP_DATA *cp_wk	�}�b�v�f�[�^�n���h��
 *	@param	idx			�ꏊ�C���f�b�N�X�iMAX�@TOWNMAP_DATA_MAX�j
 *	@param	param		TOWNMAP_DATA_PARAM��
 *
 *	@return	�l
 */
//-----------------------------------------------------------------------------
u16 TOWNMAP_DATA_GetParam( const TOWNMAP_DATA *cp_wk, u16 idx, TOWNMAP_DATA_PARAM param )
{	
	GF_ASSERT( cp_wk );
	GF_ASSERT( idx < TOWNMAP_DATA_MAX );
	GF_ASSERT( param < TOWNMAP_DATA_PARAM_MAX );

	{	
		const TOWNMAP_DATA_STRUCT *cp_data;
		cp_data	= (const TOWNMAP_DATA_STRUCT*)((const u8*)(cp_wk) + sizeof(TOWNMAP_DATA_STRUCT) * idx);

		return cp_data->param[ param ];
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  townmap_util.h��FIELD_TOWNMAP_GetRootZoneID�֐��œ���
 *	ZONEID���ȉ��ɓn���Ə�L�^�E���}�b�v�f�[�^��idx��Ԃ�
 *
 *	@param	const TOWNMAP_DATA *cp_wk   ���[�N
 *	@param	zoneID                      �^�E���}�b�v�Ŏg�p����p��ID
 *
 *	@return �^�E���}�b�v�f�[�^�̃C���f�b�N�X  ������Ȃ������ꍇTOWNMAP_DATA_ERROR��Ԃ�
 */
//-----------------------------------------------------------------------------
u16 TOWNMAP_DATA_SearchRootZoneID( const TOWNMAP_DATA *cp_wk, u16 zoneID )
{ 
  int i;
  for( i = 0; i < TOWNMAP_DATA_MAX; i++ )
  { 
    if( TOWNMAP_DATA_GetParam( cp_wk, i, TOWNMAP_DATA_PARAM_ZONE_ID ) == zoneID )
    { 
      return i;
    }
  }

  return TOWNMAP_DATA_ERROR;
}


//=============================================================================
/**
 *    �u������
 */
//=============================================================================
#if 0 //cdat���������߂Ȃ��Ȃ�܂���
//----------------------------------------------------------------------------
/**
 *	@brief  �u�������f�[�^�擾
 *
 *	@param	HEAPID heapID   �q�[�vID
 *
 *	@return �n���h��
 */
//-----------------------------------------------------------------------------
TOWNMAP_REPLACE_DATA *TOWNMAP_REPLACE_DATA_Alloc( HEAPID heapID )
{ 
	void *p_handle;
	p_handle	= GFL_ARC_UTIL_Load( ARCID_TOWNMAP_DATA, 
			1, FALSE, heapID );

	return p_handle;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �u�������f�[�^�j��
 *
 *	@param	TOWNMAP_REPLACE_DATA *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void TOWNMAP_REPLACE_DATA_Free( TOWNMAP_REPLACE_DATA *p_wk )
{ 
	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u�������֐�
 *
 *	@param	const TOWNMAP_REPLACE_DATA *cp_wk ���[�N
 *	@param	zoneID  �u���������]�[��ID
 *
 *	@return �u��������]�[��ID  ���������Ă��Ȃ����TOWNMAP_DATA_ERROR
 */
//-----------------------------------------------------------------------------
u16 TOWNMAP_REPLACE_DATA_GetReplace( const TOWNMAP_REPLACE_DATA *cp_wk, u16 zoneID )
{ 
  GF_ASSERT( cp_wk );

	{	
    int i;
		const TOWNMAP_REPLACE_STRUCT *cp_data;

    for( i = 0; i < TOWNMAP_REPLACE_MAX; i++ )
    { 
      cp_data	= (const TOWNMAP_REPLACE_STRUCT*)((const u8*)(cp_wk) + sizeof(TOWNMAP_REPLACE_STRUCT) * i);

      if( cp_data->param[ TOWNMAP_REPLACE_PARAM_SRC_ZONE_ID_00 ] == zoneID )
      { 
        return cp_data->param[ TOWNMAP_REPLACE_PARAM_DST_ZONE_ID ];
      }
    }

		return TOWNMAP_DATA_ERROR;
	}
}

#endif
