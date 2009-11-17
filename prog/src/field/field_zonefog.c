//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_zonefog.c
 *	@brief	�]�[���p�t�H�O�E���C�g���
 *	@author	tomoya takahashi
 *	@date		2009.07.09
 *
 *	���W���[�����FFIELD_ZONEFOGLIGHT
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc_def.h"
#include "fieldmap/field_fog_table.naix"
#include "fieldmap/field_zone_light.naix"

#include "field_zonefog.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�Q��FOG�f�[�^
//=====================================
typedef struct {
  u16 zone_id;
  u16 data_id;
} ZONE_FOG_DATA;

//-------------------------------------
///	�Q��LIGHT�f�[�^
//=====================================
typedef struct {
  u16 zone_id;
  u16 data_id;
} ZONE_LIGHT_DATA;



//-------------------------------------
///	�f�[�^�\����
//=====================================
typedef struct {
	s32 offset;
	u32	slope;
} FOG_DATA;

//-------------------------------------
///	FIELD_ZONEFOGLIGHT�V�X�e��
//=====================================
struct _FIELD_ZONEFOGLIGHT	
{
	FOG_DATA* p_data;

	u32 light;

  ZONE_FOG_DATA* p_fog_list;
  ZONE_LIGHT_DATA* p_light_list;

  u16 fog_list_max;
  u16 light_list_max;

  ARCHANDLE* p_handle;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief	�]�[���t�H�O���Ǘ��V�X�e���@����
 *
 *	@param	heapID		�q�[�vID
 *
 *	@return	�]�[���t�H�O���Ǘ��V�X�e��
 */
//-----------------------------------------------------------------------------
FIELD_ZONEFOGLIGHT* FIELD_ZONEFOGLIGHT_Create( u32 heapID )
{
	FIELD_ZONEFOGLIGHT* p_sys;
  u32 size;

	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_ZONEFOGLIGHT) );

	p_sys->light = FIELD_ZONEFOGLIGHT_DATA_NONE;

  p_sys->p_handle = GFL_ARC_OpenDataHandle( ARCID_ZONEFOG_TABLE, heapID );

  p_sys->p_fog_list = GFL_ARCHDL_UTIL_LoadEx( p_sys->p_handle, NARC_field_fog_table_zonefog_table_bin, FALSE, heapID, &size );
  p_sys->fog_list_max = size / sizeof(ZONE_FOG_DATA);
  
  p_sys->p_light_list = GFL_ARCHDL_UTIL_LoadEx( p_sys->p_handle, NARC_field_zone_light_light_list_bin, FALSE, heapID, &size );
  p_sys->light_list_max = size / sizeof(ZONE_LIGHT_DATA);
  

	return  p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]�[���t�H�O���Ǘ��V�X�e��	�j��
 *
 *	@param	p_sys			�V�X�e��
 */
//-----------------------------------------------------------------------------
void FIELD_ZONEFOGLIGHT_Delete( FIELD_ZONEFOGLIGHT* p_sys )
{
  GFL_ARC_CloseDataHandle( p_sys->p_handle );
  
  GFL_HEAP_FreeMemory( p_sys->p_fog_list );
  GFL_HEAP_FreeMemory( p_sys->p_light_list );
  
	FIELD_ZONEFOGLIGHT_Clear( p_sys );
	GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �]�[��ID����A���̃]�[���̃t�H�O�⃉�C�g�ݒ肪����̂��`�F�b�N
 *
 *	@param	p_sys     �V�X�e��
 *	@param	zone_id   �]�[��ID
 *	@param	heapID    �q�[�v
 */
//-----------------------------------------------------------------------------
void FIELD_ZONEFOGLIGHT_LoadZoneID( FIELD_ZONEFOGLIGHT* p_sys, u32 zone_id, u32 heapID )
{
  int i;
  u32 fog_id = FIELD_ZONEFOGLIGHT_DATA_NONE;
  u32 light_id = FIELD_ZONEFOGLIGHT_DATA_NONE;
  
  GF_ASSERT( p_sys );

  
  // FOG���X�g
  for( i=0; i<p_sys->fog_list_max; i++ )
  {
    if( p_sys->p_fog_list[i].zone_id == zone_id )
    {
      fog_id = p_sys->p_fog_list[i].data_id;
    }
  }

  // LIGHT���X�g
  for( i=0; i<p_sys->light_list_max; i++ )
  {
    if( p_sys->p_light_list[i].zone_id == zone_id )
    {
      light_id = p_sys->p_light_list[i].data_id;
    }
  }

  FIELD_ZONEFOGLIGHT_Load( p_sys, fog_id, light_id, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]�[���t�H�O���Ǘ��V�X�e��	�t�H�O���ǂݍ���
 *
 *	@param	p_sys			�V�X�e��
 *	@param	datano		�t�H�O�f�[�^�i���o�[
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_ZONEFOGLIGHT_Load( FIELD_ZONEFOGLIGHT* p_sys, u32 fogno, u32 lightno, u32 heapID )
{
	GF_ASSERT( p_sys );
	GF_ASSERT( p_sys->p_data == NULL );

	if( fogno != FIELD_ZONEFOGLIGHT_DATA_NONE )
	{
		p_sys->p_data = GFL_ARCHDL_UTIL_Load( p_sys->p_handle, fogno, FALSE, heapID );
	}
	p_sys->light	= lightno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]�[���t�H�O���Ǘ��V�X�e��	�t�H�O���j��
 *
 *	@param	p_sys			�V�X�e��
 */
//-----------------------------------------------------------------------------
void FIELD_ZONEFOGLIGHT_Clear( FIELD_ZONEFOGLIGHT* p_sys )
{
	GF_ASSERT( p_sys );

	if( p_sys->p_data )
	{
		GFL_HEAP_FreeMemory( p_sys->p_data );
		p_sys->p_data = NULL;
	}
	p_sys->light = FIELD_ZONEFOGLIGHT_DATA_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��񂪂��邩�`�F�b�N
 *
 *	@param	cp_sys �V�X�e��
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_ZONEFOGLIGHT_IsFogData( const FIELD_ZONEFOGLIGHT* cp_sys )
{
	GF_ASSERT( cp_sys );
	if( cp_sys->p_data )
	{
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�g��񂪂��邩�`�F�b�N
 *
 *	@param	cp_sys		�V�X�e��
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_ZONEFOGLIGHT_IsLightData( const FIELD_ZONEFOGLIGHT* cp_sys )
{
	if( cp_sys->light == FIELD_ZONEFOGLIGHT_DATA_NONE )
	{
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���̏��̃I�t�Z�b�g�l�擾
 *
 *	@param	cp_sys		�V�X�e��
 *
 *	@return	�I�t�Z�b�g�l
 */
//-----------------------------------------------------------------------------
s32 FIELD_ZONEFOGLIGHT_GetOffset( const FIELD_ZONEFOGLIGHT* cp_sys )
{
	GF_ASSERT( cp_sys  );
	if( cp_sys->p_data )
	{
		return cp_sys->p_data->offset;
	}
	return FIELD_ZONEFOGLIGHT_DATA_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���̏��̃X���[�v�l�擾
 *
 *	@param	cp_sys	�V�X�e��
 *
 *	@return	�X���[�v
 */
//-----------------------------------------------------------------------------
u32 FIELD_ZONEFOGLIGHT_GetSlope( const FIELD_ZONEFOGLIGHT* cp_sys )
{
	GF_ASSERT( cp_sys  );

	if( cp_sys->p_data )
	{
		return cp_sys->p_data->slope;
	}
	return FIELD_ZONEFOGLIGHT_DATA_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	���C�g�C���f�b�N�X	�擾
 *
 *	@param	cp_sys	�V�X�e��
 *
 *	@return	���C�g�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 FIELD_ZONEFOGLIGHT_GetLightIndex( const FIELD_ZONEFOGLIGHT* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->light;
}

