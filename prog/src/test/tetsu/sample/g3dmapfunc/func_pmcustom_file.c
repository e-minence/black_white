//============================================================================================
/**
 * @file	func_pmcustom.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "..\g3d_map.h"
#include "map\dp3format.h"

#include "func_pmcustom_file.h"
//============================================================================================
/**
 *
 *
 *
 * @brief	�|�P�����Ŏg�p���ꂽ�f�[�^����������֐��S
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
//�|���S���f�[�^��
typedef struct SPLIT_GRID_DATA_tag{
	u16		NumX;		//�O���b�h�w��
	u16		NumZ;		//�O���b�h�y��

	fx32	StartX;		//�n�_�ʒu�w
	fx32	StartZ;		//�n�_�ʒu�y
	fx32	EndX;		//�I�_�ʒu�w
	fx32	EndZ;		//�I�_�ʒu�y
	fx32	SizeX;		//�O���b�h�w�T�C�Y
	fx32	SizeZ;		//�O���b�h�y�T�C�Y
}SPLIT_GRID_DATA;

typedef struct POLYGON_DATA_tag{
	u16 vtx_idx0;
	u16 vtx_idx1;
	u16 vtx_idx2;	//3�p�|���S�����`������A���_�f�[�^�z��ւ̃C���f�b�N�XNo
	u16	nrm_idx;	//�@���f�[�^�z��(���K���ς�)�ւ̃C���f�b�N�XNo
	
	fx32	paramD;		//���ʂ̕��������瓱�����A�␳�lD
}POLYGON_DATA;

typedef struct HEIGHT_ARRAY_tag
{
	fx32 Height;
	int Prev;
	int Next;
}HEIGHT_ARRAY;

typedef struct MAP_HEIGHT_INFO_tag{
	SPLIT_GRID_DATA	*SplitGridData;
	POLYGON_DATA	*PolygonData;
	u16				*GridDataTbl;
	u16				*LineDataTbl;
	VecFx32			*VertexArray;
	VecFx32			*NormalArray;
	u16				*PolyIDList;
//	BOOL			LoadOK;
	BOOL			DataValid;		//�f�[�^�L���L��
}MAP_HEIGHT_INFO;

typedef struct {
	int VtxNum;
	int NrmNum;
	int PolygonNum;
	int GridNum;
	int TotalPolyIDListSize;
	int TotalLineListSize;
	int LineEntryMax;
}READ_INFO;

//============================================================================================
/**
 *
 *
 *
 * @brief	�}�b�v�f�[�^�ǂݍ���
 *				���V�[�P���X0�Ԃ�IDLING 1�Ԃ̓X�^�[�g�h�c�w�ŌŒ�
 *
 *
 */
//============================================================================================
enum {
	FILE_HEADER_LOAD = LOAD_START,
	FILE_LOAD_START,
	FILE_LOAD,
	LOAD_END,
	TEX_TRANS,
	RND_CREATE,
};

BOOL LoadMapData_PMcustomFile( GFL_G3D_MAP* g3Dmap )
{
	GFL_G3D_MAP_LOAD_STATUS* ldst;
	void*	mem;
	u32		datID;

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );
	GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );

#if 0
	switch( ldst->seq ){
	case FILE_HEADER_LOAD:
		{
			Dp3packHeaderSt	header;
			ARCHANDLE*		arc;

			GFL_G3D_MAP_GetLoadArcHandle( g3Dmap, &arc );
			GFL_G3D_MAP_GetLoadDatIDMdl( g3Dmap, &datID );
			GFL_ARC_LoadDataOfsByHandle( arc, datID, 0, sizeof(Dp3packHeaderSt), &header ); 

			//���f�����\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + header.nsbmdOffset));
			//�e�N�X�`�����\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceTex(g3Dmap, (void*)((u32)mem + header.nsbtxOffset));
			//�A�g���r���[�g���\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceAttr(g3Dmap, (void*)((u32)mem + header.vertexOffset));
			OS_Printf("fileID = %x, ", header.DataID );
			//OS_Printf("nsbmdOffset = %x, ", header.nsbmdOffset );
			//OS_Printf("nsbtxOffset = %x, ", header.nsbtxOffset );
			//OS_Printf("nsbtpOffset = %x, ", header.nsbtpOffset );
			//OS_Printf("vertexOffset = %x, ", header.vertexOffset );
			//OS_Printf("positionOffset = %x, ", header.positionOffset );
			//OS_Printf("endPos = %x\n", header.endPos );
			ldst->seq = FILE_LOAD_START;
		}
		break;

	case FILE_LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);
		//���f���f�[�^���[�h�J�n
		GFL_G3D_MAP_GetLoadDatIDMdl( g3Dmap, &datID );
		GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );

		ldst->seq = FILE_LOAD;
		break;

	case FILE_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->mdlLoaded = TRUE;
			ldst->texLoaded = TRUE;
			ldst->attrLoaded = TRUE;
			GFL_G3D_MAP_SetTransVramParam( g3Dmap );	//�e�N�X�`���]���ݒ�

			ldst->seq = TEX_TRANS;
		}
		break;

	case TEX_TRANS:
		if( GFL_G3D_MAP_TransVram(g3Dmap) == FALSE ){
			ldst->seq = RND_CREATE;
		}
		break;

	case RND_CREATE:
		//�����_�[�쐬
		GFL_G3D_MAP_MakeRenderObj( g3Dmap );

		ldst->seq = LOAD_IDLING;
		return FALSE;
		break;
	}
	return TRUE;
#else
	switch( ldst->seq ){
	case LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);

		ldst->seq++;
		break;

	case LOAD_START+1:
		ldst->mdlLoaded = TRUE;
		ldst->texLoaded = TRUE;
		ldst->attrLoaded = TRUE;
		ldst->seq = LOAD_IDLING;
		return FALSE;
		break;
	}
	return TRUE;
#endif
}

//============================================================================================
/**
 *
 *
 *
 * @brief	�R�c�}�b�v���擾
 *
 *
 *
 */
//============================================================================================
void GetAttr_PMcustomFile( GFL_G3D_MAP_ATTRINFO* attrInfo, const u8* attr,
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
#if 0
	fx32			grid_w, grid_x, grid_z;
	u32				grid_idx;
	VecFx32			vecN;
	fx32			by, valD;
	NormalVtxSt*	nvs;
	u8				triangleType;

	//�O���b�h�����擾
	grid_w = map_width / MAP_GRIDCOUNT;	//�}�b�v�����O���b�h���ŕ���
	grid_idx = ( posInBlock->z / grid_w ) * MAP_GRIDCOUNT + ( posInBlock->x / grid_w );
	grid_x = posInBlock->x % grid_w;
	grid_z = posInBlock->z % grid_w;

	//���擾(���̎������Ⴄ�̂Ŗ@���x�N�g����Z���])
	nvs = (NormalVtxSt*)((u32)attr + grid_idx * sizeof(NormalVtxSt));

	//�O���b�h���O�p�`�̔���
	if( nvs->tryangleType == 0 ){
		//0-2-1,3-1-2�̃p�^�[��
		if( grid_x + grid_z < grid_w ){
			triangleType = 0;
		} else {
			triangleType = 1;
		}
	} else {
		//2-3-0,1-0-3�̃p�^�[��
		if( grid_x > grid_z ){
			triangleType = 0;
		} else {
			triangleType = 1;
		}
	}
	if( triangleType == 0 ){
		VEC_Fx16Set( &attrInfo->vecN, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
		valD = nvs->vecN1_D;
	} else {
		VEC_Fx16Set( &attrInfo->vecN, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
		valD = nvs->vecN2_D;
	}
	VEC_Set( &vecN, attrInfo->vecN.x, attrInfo->vecN.y, attrInfo->vecN.z );
	by = -( FX_Mul(vecN.x, posInBlock->x - map_width/2) 
			+ FX_Mul(vecN.z, posInBlock->z - map_width/2) + valD );
	attrInfo->attr = nvs->attr;

	attrInfo->height = FX_Div( by, attrInfo->vecN.y ) + map_height;
#endif
}


