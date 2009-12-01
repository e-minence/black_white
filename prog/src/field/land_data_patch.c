//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		land_data_patch.h
 *	@brief  �}�b�v�̊�{�I�ȃA�g���r���[�g�A�z�u�f�[�^�����������A�ǉ��ݒ�
 *	@author	tomoya takahashi
 *	@date		2009.11.30
 *
 *	���W���[�����FFIELD_DATA_PATCH
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc/arc_def.h"
#include "arc/fieldmap/field_landdata_patch.naix"
#include "arc/fieldmap/land_data_patch_def.h"

#include "field/field_const.h"

#include "land_data_patch.h"

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
///	LAND_DATA_PATCH
//=====================================
typedef struct 
{
  u16 ofs_attr;   // �A�g���r���[�g�ւ̃I�t�Z�b�g
  u16 ofs_3dmd;   // �z�u���ւ̃I�t�Z�b�g
} LAND_DATA_PATCH_HEADER;


//-------------------------------------
///	  LAND_DATA�p�b�`���
//=====================================
struct _FIELD_DATA_PATCH
{
  LAND_DATA_PATCH_HEADER* p_file;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// �A�g���r���[�g���̎�
static NormalVtxFormat* FIELD_LAND_DATA_PATCH_GetAttrData( const FIELD_DATA_PATCH* cp_sys );
static LayoutFormat* FIELD_LAND_DATA_PATCH_Get3DmdData( const FIELD_DATA_PATCH* cp_sys );


//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v���̕������������p�b�`  ����
 *
 *	@param	data_id   �f�[�^ID
 *	@param	heapID    �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
FIELD_DATA_PATCH* FIELD_DATA_PATCH_Create( u32 data_id, HEAPID heapID )
{
  FIELD_DATA_PATCH* p_wk;
  
  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_DATA_PATCH) );

  GF_ASSERT( data_id < LAND_DATA_PATCH_MAX );
  p_wk->p_file = GFL_ARC_UTIL_Load( ARCID_LAND_DATA_PATCH, data_id, FALSE, heapID );

  TOMOYA_Printf( "load_id %d\n", data_id );

  TOMOYA_Printf( "attr ofs 0x%x\n", p_wk->p_file->ofs_attr );
  TOMOYA_Printf( "3dmd ofs 0x%x\n", p_wk->p_file->ofs_3dmd );
  
  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v���̕������������p�b�`  �j��
 *
 *	@param	p_sys   �V�X�e��
 */
//-----------------------------------------------------------------------------
void FIELD_DATA_PATCH_Delete( FIELD_DATA_PATCH* p_sys )
{
  GFL_HEAP_FreeMemory( p_sys->p_file );
  GFL_HEAP_FreeMemory( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v���̕������������p�b�`�@�A�g���r���[�g�㏑��
 *
 *	@param	cp_sys    �V�X�e��
 *	@param	p_buff    �A�g���r���[�g�o�b�t�@
 */
//-----------------------------------------------------------------------------
void FIELD_DATA_PATCH_OverWriteAttr( const FIELD_DATA_PATCH* cp_sys, NormalVtxFormat* p_buff, u32 grid_x, u32 grid_z )
{
  int i, j;
  int write_index;
  int read_index;
  const NormalVtxFormat* cp_attr;
  NormalVtxSt* p_write_buff;
  const NormalVtxSt* cp_read_buff;
  u32 addr;

  // �ǂݍ��݃o�b�t�@�擾
  cp_attr = FIELD_LAND_DATA_PATCH_GetAttrData( cp_sys );
  
  addr = (u32)p_buff;
  addr += sizeof(NormalVtxFormat);
  p_write_buff = (NormalVtxSt*)(addr);

  addr = (u32)cp_attr;
  addr += sizeof(NormalVtxFormat);
  cp_read_buff = (const NormalVtxSt*)(addr); //�w�b�_�[�����炷

  TOMOYA_Printf( "attr height %d width %d\n", cp_attr->height, cp_attr->width );
  
  // �����㏑��
  for( i=0; i<cp_attr->height; i++ )
  {
    // �����C����C�ɏ�������
    // �����o����
    write_index = ((grid_z + i) * p_buff->width) + (grid_x);
    GF_ASSERT( (grid_z + i) < p_buff->height );
    GF_ASSERT( (grid_x + cp_attr->width) <= p_buff->width );
      
    // �ǂݍ��ݐ�
    read_index = (i*cp_attr->width);

    GFL_STD_MemCopy( &cp_read_buff[read_index], &p_write_buff[write_index], sizeof(NormalVtxSt) * cp_attr->width );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v���̕������������p�b�`  �z�u���f���̒ǉ��o�^
 *
 *	@param	cp_sys          �V�X�e��
 *	@param	p_bmodel        �z�u���f���Ǘ��V�X�e��
 *	@param	g3Dmap          �RD�}�b�v���
 *	@param	count_start     �z�u���f���o�^�J�n�J�E���g�l
 *	@param  grid_x          ���O���b�h
 *	@param  grid_z          ���O���b�h
 *
 *	@return ���f���o�^�@�I���J�E���g�l
 */
//-----------------------------------------------------------------------------
int FIELD_LAND_DATA_PATCH_AddBuildModel( const FIELD_DATA_PATCH* cp_sys, FIELD_BMODEL_MAN * p_bmodel, GFL_G3D_MAP * g3Dmap, int count_start, u32 grid_x, u32 grid_z )
{
  const LayoutFormat* cp_3dmd;
  const PositionSt* cp_position;
  PositionSt regist;
  int i;

  // �z�u�����擾
  cp_3dmd = FIELD_LAND_DATA_PATCH_Get3DmdData( cp_sys );
  cp_position = (const PositionSt*)&cp_3dmd->posData;

  TOMOYA_Printf( "3dmd count %d\n", cp_3dmd->count );

  TOMOYA_Printf( "grid x=%d z=%d\n", grid_x, grid_z );

  // �ǉ��o�^
  for( i=0; i<cp_3dmd->count; i++ )
  {
    GFL_STD_MemCopy( &cp_position[i], &regist, sizeof(PositionSt) );
    
    regist.xpos += GRID_TO_FX32( grid_x );
    regist.zpos -= GRID_TO_FX32( grid_z );

    TOMOYA_Printf( "bmodel idx = %d\n", regist.resourceID );
    TOMOYA_Printf( "regist pos x=%d z=%d\n", FX_Whole( regist.xpos ), FX_Whole( regist.zpos ) );
    FIELD_BMODEL_MAN_ResistAllMapObjects( p_bmodel, g3Dmap, &regist, count_start + i);
  }

  return count_start + cp_3dmd->count;
}







//-----------------------------------------------------------------------------
/**
 *        private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���r���[�g�����擾
 *
 *	@param	cp_sys  �V�X�e��
 *
 *	@return �A�g���r���[�g���
 */
//-----------------------------------------------------------------------------
static NormalVtxFormat* FIELD_LAND_DATA_PATCH_GetAttrData( const FIELD_DATA_PATCH* cp_sys )
{
  u32 addr;
  
  addr = (u32)cp_sys->p_file;
  addr += cp_sys->p_file->ofs_attr;

  return (NormalVtxFormat*)(addr);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �z�u�����擾
 *
 *	@param	cp_sys  �V�X�e��
 *
 *	@return �z�u���
 */
//-----------------------------------------------------------------------------
static LayoutFormat* FIELD_LAND_DATA_PATCH_Get3DmdData( const FIELD_DATA_PATCH* cp_sys )
{
  u32 addr;
  
  addr = (u32)cp_sys->p_file;
  addr += cp_sys->p_file->ofs_3dmd;

  return (LayoutFormat*)(addr);
}

