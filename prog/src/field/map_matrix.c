//======================================================================
/**
 * @file	map_matrix.c
 * @brief	�t�B�[���h�}�b�v�}�g���N�X
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "map_matrix.h"

//======================================================================
//	define
//======================================================================
#define MAP_MATRIX_WIDTH_MAX (30)
#define MAP_MATRIX_HEIGHT_MAX (30)
#define MAP_MATRIX_MAX (MAP_MATRIX_WIDTH_MAX*MAP_MATRIX_HEIGHT_MAX)
#define MAP_MATRIX_PREFIX_MAX (16)

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
	u32 zone_id;
	u32 matrix_id;
	
	u16 size_x;
	u16 size_z;
	u16 status_flag;
	u32 table_size;
	u32 zone_id_tbl[MAP_MATRIX_MAX];
	u32 map_res_id_tbl[MAP_MATRIX_MAX];
};

//======================================================================
//	proto
//======================================================================
static void MapMatrix_SetData(
	MAP_MATRIX *pMat, const void *pMatData, u32 matrix_id, u32 zone_id );

//======================================================================
//	�}�b�v�}�g���N�X
//======================================================================
//--------------------------------------------------------------
/**
 * MAP_MATRIX����
 * @param	heapID	���[�N���m�ۂ���HEAPID
 * @retval	MAP_MATRIX* MAP_MATRIX*
 */
//--------------------------------------------------------------
MAP_MATRIX * MAP_MATRIX_CreateWork( HEAPID heapID )
{
	MAP_MATRIX *pMat;
	pMat = GFL_HEAP_AllocClearMemory( heapID, sizeof(MAP_MATRIX) );
	pMat->heapID = heapID;
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
	MAP_MATRIX *pMat, const u16 matrix_id, const u16 zone_id )
{
	void *pMatData = GFL_ARC_LoadDataAlloc(
		ARCID_FLDMAP_MAPMATRIX, matrix_id, pMat->heapID );
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

//--------------------------------------------------------------
/**
 * �}�g���N�X�A�[�J�C�u�f�[�^ -> MAP_MATRIX
 * @param	pMat MAP_MATRIX
 * @param	pMatData �}�g���N�X�A�[�J�C�u�f�[�^
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MapMatrix_SetData(
	MAP_MATRIX *pMat, const void *pMatData, u32 matrix_id, u32 zone_id )
{
	u32 *pMatTbl;
	const u32 *pResTbl;
	const MAP_MATRIX_HEADER *pMatH;
	
	pMatH = pMatData;
	pMat->zone_id = zone_id;
	pMat->matrix_id = matrix_id;
	
	pMat->size_x = pMatH->size_h;
	pMat->size_z = pMatH->size_v;
	pMat->table_size = pMatH->size_h * pMatH->size_v;
	pMat->status_flag = 0;
	
	GF_ASSERT( pMat->size_x );
	GF_ASSERT( pMat->size_z );
	
	pMatTbl = pMat->map_res_id_tbl;
	pResTbl = (const u32*)((const u32)pMatData + sizeof(MAP_MATRIX_HEADER));
	MI_CpuCopy32( pResTbl, pMatTbl, sizeof(u32)*pMat->table_size );

	if( pMat->status_flag ){ //�]�[��ID�w��`�F�b�N
	}else{ //�]�[��ID�w��Ȃ�
		pMatTbl = pMat->zone_id_tbl;
		MI_CpuFill32( pMatTbl, zone_id, sizeof(u32)*MAP_MATRIX_MAX );
	}
	
#ifdef DEBUG_ONLY_FOR_kagaya
	OS_Printf( "�}�g���N�X��� ID=%d,X=%d,Z=%d\n",
		matrix_id, pMat->size_x, pMat->size_z );
#endif
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
 * @retval	u32		ZONE ID
 */
//--------------------------------------------------------------
u32 MAP_MATRIX_GetBlockPosZoneID( const MAP_MATRIX *pMat, int x, int z )
{
	return( 0 );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �}�b�vX�T�C�Y�擾
 * @param	pMat	MAP_MATRIX
 * @retval	u16	X�T�C�Y
 */
//--------------------------------------------------------------
u16 MAP_MATRIX_GetMapSizeX( const MAP_MATRIX *pMat )
{
	return( pMat->size_x );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �}�b�vZ�T�C�Y�擾
 * @param	pMat	MAP_MATRIX
 * @retval	u16	X�T�C�Y
 */
//--------------------------------------------------------------
u16 MAP_MATRIX_GetMapSizeZ( const MAP_MATRIX *pMat )
{
	return( pMat->size_z );
}

//--------------------------------------------------------------
/**
 * MAP_MATRIX �}�b�vXxZ�̑��T�C�Y�擾
 * @param	pMat	MAP_MATRIX
 * @retval	u32		���T�C�Y
 */
//--------------------------------------------------------------
u32 MAP_MATRIX_GetMapTotalSize( const MAP_MATRIX *pMat )
{
	return( pMat->table_size );
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

#if 0
		u8 *tbl;
		u32 matID;
		const MAP_MATRIX_HEADER *matH;
		
		matID = ZONEDATA_GetMatrixID( mapid );
		GFL_ARC_LoadData( matrix_buf, ARCID_FLDMAP_MAPMATRIX, matID );
		
		matH = matrix_buf;
		tbl = (u8*)matrix_buf + sizeof(MAP_MATRIX_HEADER);
		
		map_res->sizex = matH->size_h;
		map_res->sizez = matH->size_v;
		map_res->totalSize = matH->size_h * matH->size_v;
		map_res->blocks = (const FLDMAPPER_MAPDATA *)tbl;
#endif
