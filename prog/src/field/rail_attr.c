//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		rail_attr.c
 *	@brief  ���[���@�A�g���r���[�g  
 *	@author	tomoya takahashi
 *	@date		2009.08.21
 *
 *	���W���[�����FRAIL_ATTR
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "arc_def.h"

#include "field_rail.h"

#include "rail_attr.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

#define RAIL_ATTR_ARCID ( ARCID_RAIL_DATA )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�A�g���r���[�g�f�[�^�w�b�_�[
//=====================================
typedef struct 
{
  u32 line_num;
} RAIL_ATTR_RESOURCE;


//-------------------------------------
///	���C���@�A�g���r���[�g
//=====================================
typedef struct 
{
  u16 x;      // ���O���b�h��
  u16 z;      // ���s���O���b�h��

  MAPATTR attr[];
} RAIL_ATTR_LINE;


//-------------------------------------
///	RAIL_ATTR_DATA
//=====================================
struct _RAIL_ATTR_DATA 
{
  // �t�@�C���w�b�_�[
  RAIL_ATTR_RESOURCE* p_file;
  u32 datasize;
  
  // ���C���A�g���r���[�g���
  u32 line_max;
  RAIL_ATTR_LINE** pp_line;
};    


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  ���[���A�g���r���[�g�f�[�^�@�i�[����������
 *
 *	@param	heapID  �q�[�vID
 *  
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
RAIL_ATTR_DATA* RAIL_ATTR_Create( u32 heapID )
{
  RAIL_ATTR_DATA* p_work;

  p_work = GFL_HEAP_AllocClearMemory( heapID, sizeof(RAIL_ATTR_DATA) );

  return p_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���A�g���r���[�g�f�[�^�@�i�[�������j��
 *
 *
 *	@param	p_work  ���[�N
 */
//-----------------------------------------------------------------------------
void RAIL_ATTR_Delete( RAIL_ATTR_DATA* p_work )
{
  RAIL_ATTR_Release( p_work );
  GFL_HEAP_FreeMemory( p_work );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[���A�g���r���[�g�f�[�^  ���\�[�X�ǂݍ���
 *  
 *	@param	p_work    ���[�N
 *	@param	datano    �f�[�^�i���o�[
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
void RAIL_ATTR_Load( RAIL_ATTR_DATA* p_work, u32 datano, u32 heapID )
{
  int i;
  u32 size;
  
  GF_ASSERT( p_work );
  GF_ASSERT( p_work->p_file == NULL );
  
  // ���̓ǂݍ���
  p_work->p_file = GFL_ARC_UTIL_LoadEx( RAIL_ATTR_ARCID, datano, FALSE, heapID, &p_work->datasize );

  // ���C�����̃o�b�t�@�𐶐�
  p_work->line_max  = p_work->p_file->line_num;
  p_work->pp_line   = GFL_HEAP_AllocClearMemory( heapID, sizeof(RAIL_ATTR_LINE*)*p_work->line_max );

  TOMOYA_Printf( "rail attr linemax = %d\n", p_work->line_max );

  // �|�C���^��ݒ�
  size = sizeof( RAIL_ATTR_RESOURCE );
  for( i=0; i<p_work->line_max; i++ )
  {
    p_work->pp_line[i] = (RAIL_ATTR_LINE*)( ((u8*)p_work->p_file) + size );
    TOMOYA_Printf( "index %d x=[%d] z=[%d]\n", i, p_work->pp_line[i]->x, p_work->pp_line[i]->z );
    size += 4 + ( sizeof(MAPATTR) * (p_work->pp_line[i]->x * p_work->pp_line[i]->z) );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���A�g���r���[�g�f�[�^�@���\�[�X�j��
 *
 *	@param	p_work  ���[�N
 */ 
//-----------------------------------------------------------------------------
void RAIL_ATTR_Release( RAIL_ATTR_DATA* p_work )
{
  if( p_work->p_file )
  {
    GF_ASSERT( p_work->pp_line );

    GFL_HEAP_FreeMemory( p_work->pp_line );
    GFL_HEAP_FreeMemory( p_work->p_file );

    p_work->line_max  = 0;
    p_work->pp_line   = NULL;
    p_work->p_file    = NULL;
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���A�g���r���[�g�f�[�^  �ǂݍ��ݏ��̗L���@�擾
 *
 *	@param	cp_work   ���[�N
 *
 *	@retval TRUE  ����
 *	@retval FALSE �Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL RAIL_ATTR_IsLoadData( const RAIL_ATTR_DATA* cp_work )
{
  GF_ASSERT( cp_work );

  if( cp_work->p_file )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���̃A�g���r���[�g�擾
 *
 *	@param	cp_work       ���[�N
 *	@param	cp_location   ���[�����P�[�V����
 *
 *	@return �A�g���r���[�g
 */
//-----------------------------------------------------------------------------
MAPATTR RAIL_ATTR_GetAttribute( const RAIL_ATTR_DATA* cp_work, const RAIL_LOCATION* cp_location )
{
  const RAIL_ATTR_LINE* cp_lineattr;
  s32 x, z;
  s32 index;
  MAPATTR attr = MAPATTR_ERROR;
  
  GF_ASSERT( cp_work );
  GF_ASSERT( cp_location );
  
  if( cp_location->type == FIELD_RAIL_TYPE_LINE )
  {
    if( cp_work->line_max > cp_location->rail_index )
    {
    
      // ���C���A�g���r���[�g�f�[�^�̎擾
      cp_lineattr = cp_work->pp_line[ cp_location->rail_index ];

      // width_grid�́A-�`+�̐����Ȃ̂ŁA+�݂̂̒l�ɂ���B
      x = ( cp_location->width_grid + (cp_lineattr->x / 2) );
      z = cp_location->line_grid;

//      TOMOYA_Printf( "x = %d, z= %d\n", x,z );

      // �͈͓��`�F�b�N
      if( (cp_lineattr->x > x) && (cp_lineattr->z) > z )
      {
      
        index = (cp_lineattr->x * z) + x;
        
        // �A�g���r���[�g�C���f�b�N�X�`�F�b�N
        attr = cp_lineattr->attr[ index ];
      }
    }
  }

  return attr;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ����~��`�F�b�N
 *
 *	@param	val   
 *
 *	@retval TRUE  
 *	@retval FALSE  
 */
//-----------------------------------------------------------------------------
BOOL RAIL_ATTR_VALUE_CheckSlipDown( const MAPATTR_VALUE val )
{
  if( (val == 0xa0) || (val == 0xa1) || (val == 0xa2) )
  {
    return TRUE;
  }
  return FALSE;
}



#ifdef PM_DEBUG
void RAIL_ATTR_DEBUG_LoadBynary( RAIL_ATTR_DATA* p_work, void* p_data, u32 datasize, u32 heapID )
{
  int i;
  u32 size;
  
  GF_ASSERT( p_work );
  GF_ASSERT( p_work->p_file == NULL );
  
  // ���̓ǂݍ���
  p_work->p_file = p_data;
  p_work->datasize = datasize;

  // ���C�����̃o�b�t�@�𐶐�
  p_work->line_max  = p_work->p_file->line_num;
  p_work->pp_line   = GFL_HEAP_AllocClearMemory( heapID, sizeof(RAIL_ATTR_LINE*)*p_work->line_max );

  TOMOYA_Printf( "rail attr linemax = %d\n", p_work->line_max );

  // �|�C���^��ݒ�
  size = sizeof( RAIL_ATTR_RESOURCE );
  for( i=0; i<p_work->line_max; i++ )
  {
    p_work->pp_line[i] = (RAIL_ATTR_LINE*)( ((u8*)p_work->p_file) + size );
    TOMOYA_Printf( "index %d x=[%d] z=[%d]\n", i, p_work->pp_line[i]->x, p_work->pp_line[i]->z );
    size += 4 + ( sizeof(MAPATTR) * (p_work->pp_line[i]->x * p_work->pp_line[i]->z) );
  }

}



//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�̎擾
 */
//-----------------------------------------------------------------------------
const void* RAIL_ATTR_DEBUG_GetData( const RAIL_ATTR_DATA* cp_work )
{
  GF_ASSERT( cp_work );
  GF_ASSERT( cp_work->p_file );

  return  cp_work->p_file;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�T�C�Y�̎擾
 */
//-----------------------------------------------------------------------------
u32 RAIL_ATTR_DEBUG_GetDataSize( const RAIL_ATTR_DATA* cp_work )
{
  GF_ASSERT( cp_work );
  GF_ASSERT( cp_work->p_file );

  return  cp_work->datasize;
}
#endif
