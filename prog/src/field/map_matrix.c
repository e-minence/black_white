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
	
	u16 size_w;
	u16 size_h;
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
static int MapMatrix_ChgBlockPos( fx32 pos );

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
MAP_MATRIX * MAP_MATRIX_Create( HEAPID heapID )
{
	MAP_MATRIX *pMat;
	pMat = GFL_HEAP_AllocClearMemory( heapID, sizeof(MAP_MATRIX) );
	pMat->heapID = heapID;
	MI_CpuFill32( pMat->map_res_id_tbl,
		MAP_MATRIX_RES_ID_NON, sizeof(u32)*MAP_MATRIX_MAX );
	MI_CpuFill32( pMat->zone_id_tbl,
		MAP_MATRIX_ZONE_ID_NON, sizeof(u32)*MAP_MATRIX_MAX );
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
	u32 zone_id;
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
 * @retval	u32		ZONE ID
 */
//--------------------------------------------------------------
u32 MAP_MATRIX_GetVectorPosZoneID( const MAP_MATRIX *pMat, fx32 x, fx32 z )
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
	MAP_MATRIX *pMat, const void *pMatData, u32 matrix_id, u32 zone_id )
{
	u32 *pMatTbl;
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
	
	pMatTbl = pMat->zone_id_tbl;
	
	if( pMat->status_flag == 1 ){ //�]�[��ID�z��L��
		pResTbl = (const u32*)((const u32)pMatData +
			sizeof(MAP_MATRIX_HEADER) + (sizeof(u32)*pMat->table_size) );
		MI_CpuCopy32( pResTbl, pMatTbl, sizeof(u32)*pMat->table_size );
	}else{ //�]�[��ID�w�薳��
		MI_CpuFill32( pMatTbl, zone_id, sizeof(u32)*MAP_MATRIX_MAX );
	}
	
#ifdef DEBUG_ONLY_FOR_kagaya
	OS_Printf( "�}�g���N�X��� ID=%d,X=%d,Z=%d,FLAG=%d\n",
		matrix_id, pMat->size_w, pMat->size_h, pMat->status_flag );
#endif
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
