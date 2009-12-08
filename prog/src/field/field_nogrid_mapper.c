//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_nogrid_mapper.c
 *	@brief  �m�[�O���b�h����@�}�b�p�[
 *	@author	tomoya takahashi
 *	@date		2009.08.25
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc/fieldmap/field_rail_data.naix"
#include "arc_def.h"

#include "field_nogrid_mapper.h"

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
///	�m�[�O���b�h����}�b�p�[���[�N
//=====================================
struct _FLDNOGRID_MAPPER 
{
  // �e���W���[��
  FIELD_RAIL_MAN*       p_railMan;
  FIELD_RAIL_LOADER*    p_railLoader;
  RAIL_ATTR_DATA*       p_attr;
  FLD_SCENEAREA*        p_areaMan;
  FLD_SCENEAREA_LOADER* p_areaLoader;

  BOOL rail_camera_stop;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h����}�b�p�[�@���[�N����
 *
 *	@param	heapID   �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
FLDNOGRID_MAPPER* FLDNOGRID_MAPPER_Create( u32 heapID, FIELD_CAMERA* p_camera, FLD_SCENEAREA* p_sceneArea, FLD_SCENEAREA_LOADER* p_sceneAreaLoader )
{
  FLDNOGRID_MAPPER* p_mapper;

  p_mapper = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDNOGRID_MAPPER) );

  // �e���W���[�����[�N����
  p_mapper->p_railMan       = FIELD_RAIL_MAN_Create( heapID, FIELD_RAIL_WORK_MAX, p_camera ); 
  p_mapper->p_railLoader    = FIELD_RAIL_LOADER_Create( heapID );
  p_mapper->p_areaMan       = p_sceneArea;
  p_mapper->p_areaLoader    = p_sceneAreaLoader;
  p_mapper->p_attr          = RAIL_ATTR_Create( heapID );

  return p_mapper;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h����}�b�p�[�j��
 *
 *	@param	p_mapper  �}�b�p�[���[�N
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_Delete( FLDNOGRID_MAPPER* p_mapper )
{
  GF_ASSERT( p_mapper );

  RAIL_ATTR_Delete( p_mapper->p_attr );
  FIELD_RAIL_MAN_Delete( p_mapper->p_railMan );
  FIELD_RAIL_LOADER_Delete( p_mapper->p_railLoader );

  GFL_HEAP_FreeMemory( p_mapper );
}





//----------------------------------------------------------------------------
/**
 *	@brief  �A�N�e�B�u�ݒ�
 *
 *	@param	p_mapper    �}�b�p�[
 *	@param	flag        �t���O
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_SetActive( FLDNOGRID_MAPPER* p_mapper, BOOL flag )
{
  GF_ASSERT( p_mapper );
	FIELD_RAIL_MAN_SetActiveFlag(p_mapper->p_railMan, flag);
	FLD_SCENEAREA_SetActiveFlag(p_mapper->p_areaMan, flag);
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�N�e�B�u��Ԃ��`�F�b�N
 *
 *	@param	cp_mapper   �}�b�p�[
 *
 *	@retval TRUE    �A�N�e�B�u
 *	@retval FALSE   ��A�N�e�B�u
 */
//-----------------------------------------------------------------------------
BOOL FLDNOGRID_MAPPER_IsActive( const FLDNOGRID_MAPPER* cp_mapper )
{
  BOOL result;
  
  GF_ASSERT( cp_mapper );

  result = FIELD_RAIL_MAN_GetActiveFlag( cp_mapper->p_railMan );

  return result;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[���J���������s���邩�ݒ�
 *
 *	@param	p_mapper  �}�b�p�[
 *	@param	flag      �t���O
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_SetRailCameraActive( FLDNOGRID_MAPPER* p_mapper, BOOL flag )
{
  if( flag )
  {
    p_mapper->rail_camera_stop = FALSE;
  }
  else
  {
    p_mapper->rail_camera_stop = TRUE;
  }
	FLD_SCENEAREA_SetActiveFlag(p_mapper->p_areaMan, flag);
}



//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�̔��f
 *
 *	@param	p_mapper  �}�b�p�[
 *	@param	cp_data   �f�[�^
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_ResistData( FLDNOGRID_MAPPER* p_mapper, const FLDNOGRID_RESISTDATA* cp_data, u32 heapID )
{
  u32 filecnt;
  
  GF_ASSERT( p_mapper );
  
  // �j������
  FLDNOGRID_MAPPER_Release( p_mapper );

  // ��񃍁[�h���\�z
  if( cp_data->railDataID != FLDNOGRID_RESISTDATA_NONE )
  {
    // �S�t�@�C�����̔�������ATTR�͎n�܂�
    filecnt = GFL_ARC_GetDataFileCnt( ARCID_RAIL_DATA );
    filecnt /= 2;
    
    FIELD_RAIL_LOADER_Load( p_mapper->p_railLoader, cp_data->railDataID, heapID );
    FIELD_RAIL_MAN_Load( p_mapper->p_railMan, FIELD_RAIL_LOADER_GetData( p_mapper->p_railLoader ) );

    RAIL_ATTR_Load( p_mapper->p_attr, cp_data->railDataID + filecnt, heapID );
  }
  if( cp_data->areaDataID != FLDNOGRID_RESISTDATA_NONE )
  {
    FLD_SCENEAREA_LOADER_Load( p_mapper->p_areaLoader, cp_data->areaDataID, heapID );
    FLD_SCENEAREA_Load( p_mapper->p_areaMan, 
        FLD_SCENEAREA_LOADER_GetData( p_mapper->p_areaLoader ),
        FLD_SCENEAREA_LOADER_GetDataNum( p_mapper->p_areaLoader ),
        FLD_SCENEAREA_LOADER_GetFunc( p_mapper->p_areaLoader ) );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�[�J�C�u�f�[�^����f�[�^�ݒ�
 *
 *	@param	p_mapper
 *	@param	dataID
 *	@param	heapID 
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_ResistDataArc( FLDNOGRID_MAPPER* p_mapper, u32 dataID, u32 heapID )
{
  FLDNOGRID_RESISTDATA* p_resist;
  u32 size;

  size = GFL_ARC_GetDataSize( ARCID_FLD_RAILSETUP, dataID );

  p_resist = GFL_HEAP_AllocClearMemoryLo( heapID, size );

  GFL_ARC_LoadData( p_resist, ARCID_FLD_RAILSETUP, dataID );

  FLDNOGRID_MAPPER_ResistData( p_mapper, p_resist, heapID );  

  GFL_HEAP_FreeMemory( p_resist );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h����@���j��
 *
 *	@param	p_mapper  �}�b�p�[
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_Release( FLDNOGRID_MAPPER* p_mapper )
{
  GF_ASSERT( p_mapper );

  FLD_SCENEAREA_Release( p_mapper->p_areaMan );
  FIELD_RAIL_MAN_Clear( p_mapper->p_railMan );
  FIELD_RAIL_LOADER_Clear( p_mapper->p_railLoader );
  FLD_SCENEAREA_LOADER_Clear( p_mapper->p_areaLoader );
  RAIL_ATTR_Release( p_mapper->p_attr );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h����  �ݒ�f�[�^�̗L��
 *
 *	@param	cp_mapper   �}�b�p�[
 *
 *	@retval TRUE  ����
 *	@retval FALSE �Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FLDNOGRID_MAPPER_IsResistData( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );

  if( FIELD_RAIL_MAN_IsLoad( cp_mapper->p_railMan ) )
  {
    return TRUE;
  }
  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h����@���C������
 *
 *	@param	p_mapper  �}�b�p�[���[�N
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_Main( FLDNOGRID_MAPPER* p_mapper )
{
  VecFx32 rail_pos;
  BOOL camera_move = FALSE;

  // ���[���V�X�e�����C��
  FIELD_RAIL_MAN_Update( p_mapper->p_railMan );
  if( p_mapper->rail_camera_stop == FALSE )
  {
    FIELD_RAIL_MAN_UpdateCamera( p_mapper->p_railMan );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����[�N�̐���
 *
 *	@param	p_mapper  �}�b�p�[
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_WORK* FLDNOGRID_MAPPER_CreateRailWork( FLDNOGRID_MAPPER* p_mapper )
{
  GF_ASSERT( p_mapper );
  return FIELD_RAIL_MAN_CreateWork( p_mapper->p_railMan );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����[�N�̔j��
 *
 *	@param	p_mapper    �}�b�p�[
 *	@param	p_railWork  �j�����郌�[�����[�N
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DeleteRailWork( FLDNOGRID_MAPPER* p_mapper, FIELD_RAIL_WORK* p_railWork )
{
  GF_ASSERT( p_mapper );
  FIELD_RAIL_MAN_DeleteWork( p_mapper->p_railMan, p_railWork );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �J�����ƃo�C���h���郌�[�����[�N���擾
 *
 *	@param	p_mapper      �}�b�p�[
 *	@param	p_railWork    ���[�����[�N
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_BindCameraWork( FLDNOGRID_MAPPER* p_mapper, const FIELD_RAIL_WORK* cp_railWork )
{
  GF_ASSERT( p_mapper );
  GF_ASSERT( cp_railWork );
  FIELD_RAIL_MAN_BindCamera( p_mapper->p_railMan, cp_railWork );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����Ƃ̃o�C���h��j��
 *
 *	@param	p_mapper  �}�b�p�[
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_UnBindCameraWork( FLDNOGRID_MAPPER* p_mapper )
{
  GF_ASSERT( p_mapper );
  FIELD_RAIL_MAN_UnBindCamera( p_mapper->p_railMan );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[���A�g���r���[�g���̎擾
 *
 *	@param	cp_mapper     �}�b�p�[
 *	@param	cp_location   ���[�����P�[�V����
 *
 *	@return �}�b�v�A�g���r���[�g
 */
//-----------------------------------------------------------------------------
MAPATTR FLDNOGRID_MAPPER_GetAttr( const FLDNOGRID_MAPPER* cp_mapper, const RAIL_LOCATION* cp_location )
{
  GF_ASSERT( cp_mapper );
  return RAIL_ATTR_GetAttribute( cp_mapper->p_attr, cp_location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[��  ���C�������ON�EOFF
 *
 *	@param	p_mapper    �}�b�p�[
 *	@param	line_index  ���C���C���f�b�N�X
 *	@param	flag        �t���O  TRUE�F����ON�@�@FALSE�F����OFF
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_SetLineActive( FLDNOGRID_MAPPER* p_mapper, u32 line_index, BOOL flag )
{
  GF_ASSERT( p_mapper );
  FIELD_RAIL_MAN_SetLineActive( p_mapper->p_railMan, line_index, flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���@���C�������ON�EOFF�擾
 *
 *	@param	cp_mapper   �}�b�p�[
 *	@param	line_index  ���C���C���f�b�N�X
 *  
 *	@retval TRUE    ���C������\
 *	@retval FALSE   ���C������s�\
 */
//-----------------------------------------------------------------------------
BOOL FLDNOGRID_MAPPER_GetLineActive( const FLDNOGRID_MAPPER* cp_mapper, u32 line_index )
{
  GF_ASSERT( cp_mapper );
  return FIELD_RAIL_MAN_GetLineActive( cp_mapper->p_railMan, line_index );
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���[���}�l�[�W�����W���[���̎擾
 *
 *	@param	cp_mapper   �}�b�p�[
 *
 *	@return ���W���[��
 */
//-----------------------------------------------------------------------------
const FIELD_RAIL_MAN* FLDNOGRID_MAPPER_GetRailMan( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_railMan;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���A�}�l�[�W�����W���[���̎擾
 *
 *	@param	cp_mapper   �}�b�p�[
 *
 *	@return ���W���[��
 */
//-----------------------------------------------------------------------------
const FLD_SCENEAREA* FLDNOGRID_MAPPER_GetSceneAreaMan( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_areaMan;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����[�_�[���W���[���̎擾
 *
 *	@param	cp_mapper   �}�b�p�[
 *
 *	@return ���W���[��
 */
//-----------------------------------------------------------------------------
const FIELD_RAIL_LOADER* FLDNOGRID_MAPPER_GetRailLoader( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_railLoader;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���A���[�_�[�̃��W���[���擾
 *
 *	@param	cp_mapper   �}�b�p�[
 *
 *	@return ���W���[��
 */
//-----------------------------------------------------------------------------
const FLD_SCENEAREA_LOADER* FLDNOGRID_MAPPER_GetSceneAreaLoader( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_areaLoader;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���r���[�g�f�[�^�̎擾
 *
 *	@param	cp_mapper   �}�b�p�[
 *
 *	@return �A�g���r���[�g�f�[�^
 */
//-----------------------------------------------------------------------------
const RAIL_ATTR_DATA* FLDNOGRID_MAPPER_GetRailAttrData( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_attr;
}


// �f�o�b�N�p
#ifdef PM_DEBUG
//----------------------------------------------------------------------------
/**
 *	@brief  ���[��binary�̓ǂݍ���
 *
 *	@param	p_mapper    �}�b�p�[
 *	@param	p_dat       �f�[�^
 *	@param	size        �f�[�^�T�C�Y
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DEBUG_LoadRailBynary( FLDNOGRID_MAPPER* p_mapper, void* p_dat, u32 size )
{
  GF_ASSERT( p_mapper );
  GF_ASSERT( p_dat );

  FIELD_RAIL_LOADER_Clear( p_mapper->p_railLoader );
  FIELD_RAIL_LOADER_DEBUG_LoadBinary( p_mapper->p_railLoader, p_dat, size );

  FIELD_RAIL_MAN_Clear( p_mapper->p_railMan );
	FIELD_RAIL_MAN_Load( p_mapper->p_railMan, FIELD_RAIL_LOADER_GetData( p_mapper->p_railLoader ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���Abinary�̓ǂݍ���
 *
 *	@param	p_mapper  �}�b�p�[
 *	@param	p_dat     �f�[�^
 *	@param	size      �f�[�^�T�C�Y
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DEBUG_LoadAreaBynary( FLDNOGRID_MAPPER* p_mapper, void* p_dat, u32 size )
{
  GF_ASSERT( p_mapper );
  GF_ASSERT( p_dat );

  FLD_SCENEAREA_LOADER_Clear( p_mapper->p_areaLoader );
  FLD_SCENEAREA_LOADER_LoadBinary( p_mapper->p_areaLoader, p_dat, size );


	FLD_SCENEAREA_Release( p_mapper->p_areaMan );
	FLD_SCENEAREA_Load( p_mapper->p_areaMan, 
			FLD_SCENEAREA_LOADER_GetData( p_mapper->p_areaLoader ),
			FLD_SCENEAREA_LOADER_GetDataNum( p_mapper->p_areaLoader ),
			FLD_SCENEAREA_LOADER_GetFunc( p_mapper->p_areaLoader ) );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���r���[�gbinary�̓ǂݍ���
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DEBUG_LoadAttrBynary( FLDNOGRID_MAPPER* p_mapper, void* p_dat, u32 size, u32 heapID )
{
  RAIL_ATTR_Release( p_mapper->p_attr );
  
  RAIL_ATTR_DEBUG_LoadBynary( p_mapper->p_attr, p_dat, size, heapID );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���R�ɃA�N�Z�X�ł��郌�[���}�l�[�W�����擾
 *
 *	@param	cp_mapper 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_MAN* FLDNOGRID_MAPPER_DEBUG_GetRailMan( const FLDNOGRID_MAPPER* cp_mapper )
{
  return cp_mapper->p_railMan;
}


#endif



