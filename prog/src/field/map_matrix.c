//======================================================================
/**
 * @file	map_matrix.c
 * @brief	�t�B�[���h�}�b�v�}�g���N�X
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "gamesystem/game_data.h"
#include "field/map_matrix.h"

#include "arc/fieldmap/map_replace.naix"
#include "arc/fieldmap/map_replace.h"
#include "map_replace.h"

//======================================================================
//	define
//======================================================================
//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	MAP_MATRIX_HEADER
//--------------------------------------------------------------
typedef struct
{
	u16 flag;
	u16 kind;
	u16 size_h;
	u16 size_v;
}MAP_MATRIX_HEADER;

//--------------------------------------------------------------
//	MAP_MATRIX
//--------------------------------------------------------------
struct _TAG_MAP_MATRIX
{
	HEAPID heapID;
	u16 zone_id;
	u32 matrix_id;
	
	u16 size_w;
	u16 size_h;
	u16 status_flag;
	u32 table_size;
	u16 zone_id_tbl[MAP_MATRIX_MAX];
  u32 map_res_id_tbl[MAP_MATRIX_MAX];
};

//======================================================================
//	proto
//======================================================================
static void MapMatrix_SetData(
	MAP_MATRIX *pMat, const void *pMatData, u32 matrix_id, u16 zone_id );
static int MapMatrix_ChgBlockPos( fx32 pos );

//======================================================================
//	�}�b�v�}�g���N�X
//======================================================================
//--------------------------------------------------------------
/**
 * MAP_MATRIX����
 * @param	heapID	  ���[�N���m�ۂ���HEAPID
 * @retval	MAP_MATRIX* MAP_MATRIX*
 */
//--------------------------------------------------------------
MAP_MATRIX * MAP_MATRIX_Create( HEAPID heapID )
{
	MAP_MATRIX *pMat;
	
  pMat = GFL_HEAP_AllocClearMemory( heapID, sizeof(MAP_MATRIX) );
	
  pMat->heapID = heapID;
	
  MI_CpuFill32( pMat->map_res_id_tbl,
		MAP_MATRIX_RES_ID_NON, sizeof(u32)*MAP_MATRIX_MAX );
	MI_CpuFill32( pMat->zone_id_tbl,
		MAP_MATRIX_ZONE_ID_NON, sizeof(u16)*MAP_MATRIX_MAX );
	return( pMat );
}

//--------------------------------------------------------------
/**
 * �}�g���N�XID����MAP_MATRIX������
 * @param	matrix_id ��������}�g���N�XID
 * @param	zone_id	matrix_id��������]�[��ID
 * @retval	MAP_MATRIX*
 */
//--------------------------------------------------------------
void MAP_MATRIX_Init(
	MAP_MATRIX *pMat, const u16 matrix_id, const u16 zone_id, HEAPID temp_heapID )
{
	void *pMatData = GFL_ARC_LoadDataAlloc(
		ARCID_FLDMAP_MAPMATRIX, matrix_id, temp_heapID );
	MapMatrix_SetData( pMat, pMatData, matrix_id, zone_id );
	GFL_HEAP_FreeMemory( pMatData );
}

//--------------------------------------------------------------
/**
 * �������ꂽMAP_MATRIX�폜
 * @param	pMat ���������}�g���N�XID
 * @retval	nothing
 */
//--------------------------------------------------------------
void MAP_MATRIX_Delete( MAP_MATRIX *pMat )
{
	GFL_HEAP_FreeMemory( pMat );
}

//======================================================================
//	MAP_MATRIX �Q��
//======================================================================
//--------------------------------------------------------------
/**
 * MAP_MATRIX �Z�b�g����Ă���}�g���N�XID���擾
 * @param	pMat	MAP_MATRIX
 * @retval	u16		�}�b�v�}�g���N�XID
 */
//--------------------------------------------------------------
u32 MAP_MATRIX_GetMatrixID( const MAP_MATRIX *pMat )
{
	return( pMat->matrix_id );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �w��ʒu�̃]�[��ID���擾
 * @param	pMat	MAP_MATRIX
 * @param	x		X���W(�u���b�N�P��)
 * @param	z		Z���W(�u���b�N�P��)
 * @retval	u16		ZONE ID
 */
//--------------------------------------------------------------
u16 MAP_MATRIX_GetBlockPosZoneID( const MAP_MATRIX *pMat, int x, int z )
{
	u16 zone_id;
	GF_ASSERT( 0 <= x && x < pMat->size_w );
	GF_ASSERT( 0 <= z && z < pMat->size_h );
	zone_id = pMat->zone_id_tbl[x + (z*pMat->size_w)];
	return( zone_id );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �w����W�̃]�[��ID���擾
 * @param	pMat	MAP_MATRIX
 * @param	x		X���W(�����W fx32
 * @param	z		Z���W(�����W fx32
 * @retval	u16		ZONE ID
 */
//--------------------------------------------------------------
u16 MAP_MATRIX_GetVectorPosZoneID( const MAP_MATRIX *pMat, fx32 x, fx32 z )
{
	int bx = MapMatrix_ChgBlockPos( x );
	int bz = MapMatrix_ChgBlockPos( z );
	return( MAP_MATRIX_GetBlockPosZoneID(pMat,bx,bz) );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �}�b�v���u���b�N�T�C�Y�擾
 * @param	pMat	MAP_MATRIX
 * @retval	u16	X�T�C�Y
 */
//--------------------------------------------------------------
u16 MAP_MATRIX_GetMapBlockSizeWidth( const MAP_MATRIX *pMat )
{
	return( pMat->size_w );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �}�b�vZ�T�C�Y�擾
 * @param	pMat	MAP_MATRIX
 * @retval	u16	X�T�C�Y
 */
//--------------------------------------------------------------
u16 MAP_MATRIX_GetMapBlockSizeHeight( const MAP_MATRIX *pMat )
{
	return( pMat->size_h );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �}�b�vXxZ�̑��T�C�Y�擾
 * @param	pMat	MAP_MATRIX
 * @retval	u32		���T�C�Y
 */
//--------------------------------------------------------------
u32 MAP_MATRIX_GetMapBlockTotalSize( const MAP_MATRIX *pMat )
{
	return( pMat->table_size );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �}�b�vXxZ�̑��T�C�Y�ύX
 * @param	pMat	MAP_MATRIX
 * @param xsize X�T�C�Y
 * @param zsize Z�T�C�Y
 */
//--------------------------------------------------------------
void MAP_MATRIX_SetTableSize( MAP_MATRIX *pMat, u8 xsize, u8 zsize )
{ 
  u32 total = xsize * zsize;

  if( (MAP_MATRIX_WIDTH_MAX  < xsize) ||
      (MAP_MATRIX_HEIGHT_MAX < zsize) ||
      (MAP_MATRIX_MAX        < total) )
  {
    return;
  }

  pMat->size_w     = xsize;
  pMat->size_h     = zsize;
  pMat->table_size = total;
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX ���\�[�X�f�[�^ID���i�[���ꂽ�u���b�N�e�[�u�����擾
 * @param	pMat	MAP_MATRIX
 * @retval	u32*	�u���b�N�e�[�u���̃|�C���^
 */
//--------------------------------------------------------------
const u32 * MAP_MATRIX_GetMapResIDTable( const MAP_MATRIX *pMat )
{
	return( pMat->map_res_id_tbl );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �w��ʒu���u���b�N�͈͓����`�F�b�N
 * @param	pMat	MAP_MATRIX
 * @param	x		X���W(�u���b�N�P��)
 * @param	z		Z���W(�u���b�N�P��)
 * @retval	BOOL	TRUE=�͈͓� FALSE=�͈͊O
 */
//--------------------------------------------------------------
BOOL MAP_MATRIX_CheckBlockPosRange( const MAP_MATRIX *pMat, int x, int z )
{
	if( z >= 0 && z < pMat->size_h ){
		if( x >= 0 && x < pMat->size_w ){
			return( TRUE );
		}
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �w��ʒu���u���b�N�͈͓����`�F�b�N�@
 * @param	pMat	MAP_MATRIX
 * @param	x		X���W(�����W fx32
 * @param	z		Z���W(�����W fx32
 * @retval	BOOL	TRUE=�͈͓� FALSE=�͈͊O
 */
//--------------------------------------------------------------
BOOL MAP_MATRIX_CheckVectorPosRange(
		const MAP_MATRIX *pMat, fx32 x, fx32 z )
{
	int bx = MapMatrix_ChgBlockPos( x );
	int bz = MapMatrix_ChgBlockPos( z );
	return( MAP_MATRIX_CheckBlockPosRange(pMat,bx,bz) );
}


//======================================================================
//
//  �����ɂ��}�b�v��������
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �}�b�v�u������
 * @param	pMat	MAP_MATRIX
 * @param ctrl  �}�b�v�u�������f�[�^���䃏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void MapMatrix_Replace(
    MAP_MATRIX *pMat, const MAPREPLACE_CTRL * ctrl, HEAPID temp_heapID )
{
  REPLACE_REQUEST req;
  u32 before, after;
  int i;
  req = MAPREPLACE_GetReplaceValue( ctrl, &before, &after );

  switch ( (REPLACE_REQUEST)req )
  {
  case REPLACE_REQ_NON:
    break;
  case REPLACE_REQ_MATRIX:
    MAP_MATRIX_Init( pMat, after, pMat->zone_id, temp_heapID );
    OS_Printf("MapReplace:MATRIX: %d --> %d\n", before, after );
    break;
  case REPLACE_REQ_BLOCK:
    for ( i = 0; i < pMat->table_size; i++ )
    {
      if ( pMat->map_res_id_tbl[i] == before )
      {
        pMat->map_res_id_tbl[i] = after;
        OS_Printf("MapReplace:BLOCK:(%02d,%02d) %d->%d\n",
            i % pMat->size_w, i / pMat->size_w, before, after );
      }
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * @brief �}�b�v���������`�F�b�N�����f
 * @param	pMat MAP_MATRIX
 * @param gamesys
 */
//--------------------------------------------------------------
void MAP_MATRIX_CheckReplace(
    MAP_MATRIX *pMat, GAMESYS_WORK * gamesys, HEAPID temp_heapID )
{
  int count ,data_max;
  MAPREPLACE_CTRL * ctrl = MAPREPLACE_Create( pMat->heapID, gamesys );
  data_max = MAPREPLACE_GetDataMax( ctrl );
  for ( count = 0; count < data_max; count ++ )
  {
    u32 mat_id = MAPREPLACE_Load( ctrl, count );
    if ( mat_id != pMat->matrix_id ) continue;
    //�}�g���b�N�X����v������u�������������s��
    MapMatrix_Replace( pMat, ctrl, temp_heapID );
  }
  MAPREPLACE_Delete( ctrl );
}

//======================================================================
//	�}�b�v�}�g���N�X�@�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �}�g���N�X�A�[�J�C�u�f�[�^ -> MAP_MATRIX
 * @param	pMat MAP_MATRIX
 * @param	pMatData �}�g���N�X�A�[�J�C�u�f�[�^
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MapMatrix_SetData(
	MAP_MATRIX *pMat, const void *pMatData, u32 matrix_id, u16 zone_id )
{
	u32 *pMatTbl;
  u16 *pZoneTbl;
	const u32 *pResTbl;
	const MAP_MATRIX_HEADER *pMatH;
	
	pMatH = pMatData;
	pMat->zone_id = zone_id;
	pMat->matrix_id = matrix_id;
	
	pMat->size_w = pMatH->size_h;
	pMat->size_h = pMatH->size_v;
	pMat->table_size = pMat->size_w * pMat->size_h;
	pMat->status_flag = pMatH->flag;
	
	GF_ASSERT( pMat->size_w );
	GF_ASSERT( pMat->size_h );
	
	pMatTbl = pMat->map_res_id_tbl;
	pResTbl = (const u32*)((const u32)pMatData + sizeof(MAP_MATRIX_HEADER));
	MI_CpuCopy32( pResTbl, pMatTbl, sizeof(u32)*pMat->table_size );
	
	pZoneTbl = pMat->zone_id_tbl;
	
	if( pMat->status_flag == 1 ){ //�]�[��ID�z��L��
		pResTbl = (const u32*)((const u32)pMatData +
			sizeof(MAP_MATRIX_HEADER) + (sizeof(u32)*pMat->table_size) );
    {
      int i;
      for (i = 0; i < pMat->table_size; i++)
      {
        pZoneTbl[i] = pResTbl[i];
      }
    }
	}else{ //�]�[��ID�w�薳��
    MI_CpuFill16( pZoneTbl, zone_id, sizeof(u16)*MAP_MATRIX_MAX );
	}
	
//#ifdef DEBUG_ONLY_FOR_kagaya
	OS_Printf( "�}�g���N�X��� ID=%d,X=%d,Z=%d,FLAG=%d\n",
		matrix_id, pMat->size_w, pMat->size_h, pMat->status_flag );
//#endif
}

//--------------------------------------------------------------
/**
 * �����W���u���b�N���W��
 * @param	pos	���W
 * @retval	int �u���b�N���W�ϊ���̒l
 */
//--------------------------------------------------------------
static int MapMatrix_ChgBlockPos( fx32 pos )
{
	return( ((pos/FX32_ONE)/16)/32 );
}
