//============================================================================================
/**
 * @file	func_mapeditor_file.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "..\g3d_map.h"
#include "func_mapeditor_file.h"
//============================================================================================
/**
 *
 *
 *
 * @brief	�f�e�}�b�v�G�f�B�^���쐬�����f�[�^����������֐��S
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
//�����f�[�^�擾�p
typedef struct {
	fx16	vecN1_x;
	fx16	vecN1_y;
	fx16	vecN1_z;

	fx16	vecN2_x;
	fx16	vecN2_y;
	fx16	vecN2_z;

	fx32	vecN1_D;
	fx32	vecN2_D;

	u32		attr:31;
	u32		tryangleType:1;
}NormalVtxSt;

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
	L1_MDL_LOAD_START = LOAD_START,
	L1_MDL_LOAD,
	L1_TEX_LOAD_START,
	L1_TEX_LOAD,
	L1_RND_CREATE,
	L1_TEX_TRANS,
	L1_ATTR_LOAD_START,
	L1_ATTR_LOAD,
	L1_LOAD_END,
};

BOOL LoadMapData_MapEditorFile( GFL_G3D_MAP* g3Dmap )
{
	GFL_G3D_MAP_LOAD_STATUS* ldst;
	void*	mem;
	u32		datID;

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );
	GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );

	switch( ldst->seq ){
	case L1_MDL_LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);
		//���f���f�[�^���[�h�J�n
		GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + ldst->mOffs));

		GFL_G3D_MAP_GetLoadDatIDMdl( g3Dmap, &datID );
		GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );

		ldst->seq = L1_MDL_LOAD;
		break;
	case L1_MDL_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->mdlLoaded = TRUE;
			ldst->seq = L1_TEX_LOAD_START;
		}
		break;

	case L1_TEX_LOAD_START:
		GFL_G3D_MAP_GetLoadDatIDTex( g3Dmap, &datID );

		if( datID == NON_TEX ){
			ldst->seq = L1_RND_CREATE;
		} else {
			//�e�N�X�`�����[�h�J�n
			GFL_G3D_MAP_CreateResourceTex(g3Dmap, (void*)((u32)mem + ldst->mOffs));

			GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );
			ldst->seq = L1_TEX_LOAD;
		}
		break;
	case L1_TEX_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->texLoaded = TRUE;
			GFL_G3D_MAP_SetTransVramParam( g3Dmap );	//�e�N�X�`���]���ݒ�
			ldst->seq = L1_TEX_TRANS;
		}
		break;

	case L1_TEX_TRANS:
		if( GFL_G3D_MAP_TransVram(g3Dmap) == FALSE ){
			ldst->seq = L1_RND_CREATE;
		}
		break;

	case L1_RND_CREATE:
		//�����_�[�쐬
		GFL_G3D_MAP_MakeRenderObj( g3Dmap );
		ldst->seq = L1_ATTR_LOAD_START;
		break;

	case L1_ATTR_LOAD_START:
		GFL_G3D_MAP_GetLoadDatIDAttr( g3Dmap, &datID );

		if( datID == NON_ATTR ){
			ldst->seq = L1_LOAD_END;
		} else {
		//�A�g���r���[�g�f�[�^���[�h�J�n
			GFL_G3D_MAP_CreateResourceAttr(g3Dmap, (void*)((u32)mem + ldst->mOffs));
			GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );
			ldst->seq = L1_ATTR_LOAD;
		}
		break;
	case L1_ATTR_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->attrLoaded = TRUE;
			ldst->seq = L1_LOAD_END;
		}
		break;

	case L1_LOAD_END:
		GFL_G3D_MAP_MakeTestPos(g3Dmap);

		ldst->seq = LOAD_IDLING;
		return FALSE;
		break;
	}
	return TRUE;
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
void GetAttr_MapEditorFile( GFL_G3D_MAP_ATTRINFO* attrInfo, const u8* attr,
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
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
}

