//============================================================================================
/**
 * @file	field_func_bridge_file.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include <gflib.h>
#include "system\main.h"

#include "field_func_bridge_file.h"

#include "height.h"

/// �S�����p�b�N�����t�@�C���̃w�b�_�[
typedef struct {
    u16 DataID;         ////< DP3PACK_HEADER
    u16 dummy1;
    u32 nsbmdOffset;    ///< �t�@�C���̐擪����nsbmd�̏ꏊ�܂ł�OFFSET
    u32 nsbtxOffset;    ///< �t�@�C���̐擪����nsbtx�̏ꏊ�܂ł�OFFSET
    u32 bhcOffset;	    ///< �t�@�C���̐擪����bhc�̏ꏊ�܂ł�OFFSET
    u32 vertexOffset;   ///< �t�@�C���̐擪����@���{�A�g���r���[�g�̏ꏊ�܂ł�OFFSET
    u32 positionOffset; ///< �t�@�C���̐擪����|�W�V�����̏ꏊ�܂ł�OFFSET
    u32 endPos;         ///< �t�@�C���̐擪����|�W�V�����̍Ō�܂ł�OFFSET
} BridgePackHeaderSt;


void CheckHeightData(const void *mem, MHI_PTR outMHI);
extern BOOL GetHeightForBlock(const fx32 inNowY, const fx32 inX, const fx32 inZ,
		MHI_CONST_PTR inMap3DInfo,fx32 *outY);
//============================================================================================
/**
 *
 *
 *
 * @brief	�f�[�^����������֐��Q
 *
 *
 *
 */
//============================================================================================
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
	FILE_LOAD_START = GFL_G3D_MAP_LOAD_START,
	FILE_LOAD,
	RND_CREATE,
	TEX_TRANS,
};

BOOL FieldLoadMapData_BridgeFile( GFL_G3D_MAP* g3Dmap )
{
	GFL_G3D_MAP_LOAD_STATUS* ldst;

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );

	switch( ldst->seq ){

	case FILE_LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);

		//�������擪�ɂ̓f�[�^�擾�p����z�u���邽�߁A�ǂݍ��݃|�C���^�����炷
		ldst->mOffs += sizeof(MAP_HEIGHT_INFO);

		//���f���f�[�^���[�h�J�n
		{
			u32		datID;
			GFL_G3D_MAP_GetLoadDatID( g3Dmap, &datID );
			GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );
		}
		ldst->seq = FILE_LOAD;
		break;

	case FILE_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->mdlLoaded = TRUE;
			ldst->texLoaded = TRUE;
			ldst->attrLoaded = TRUE;

			ldst->seq = RND_CREATE;
		}
		break;

	case RND_CREATE:
		//�����_�[�쐬
		{
			void*				mem;
			BridgePackHeaderSt*	fileHeader;
			//�w�b�_�[�ݒ�
			GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );
			mem = ((u8*)mem + sizeof(MAP_HEIGHT_INFO));
			fileHeader = (BridgePackHeaderSt*)mem;
			//���f�����\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + fileHeader->nsbmdOffset));
			//�e�N�X�`�����\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceTex(g3Dmap, (void*)((u32)mem + fileHeader->nsbtxOffset)); 
			//�z�u�I�u�W�F�N�g�ݒ�
			if( fileHeader->positionOffset != fileHeader->endPos ){
				LayoutFormat* layout = (LayoutFormat*)((u32)mem + fileHeader->positionOffset);
				PositionSt* objStatus = (PositionSt*)&layout->posData;
				GFL_G3D_MAP_GLOBALOBJ_ST status;
				int i, count = layout->count;

				for( i=0; i<count; i++ ){
					status.id = objStatus[i].resourceID;
					VEC_Set( &status.trans, 
							objStatus[i].xpos, objStatus[i].ypos, -objStatus[i].zpos );
					status.rotate = objStatus[i].rotate;
					GFL_G3D_MAP_ResistGlobalObj( g3Dmap, &status, i );
				}
			//===========
			} else {
				//GFL_G3D_MAP_MakeTestPos( g3Dmap );
			//===========
			}
#if 0
    OS_Printf("DataID=%08x\n",fileHeader->DataID);         ////< DP3PACK_HEADER
    OS_Printf("dummy1=%08x\n",fileHeader->dummy1);
    OS_Printf("nsbmdOffset=%08x\n",fileHeader->nsbmdOffset);    ///< �t�@�C���̐擪����nsbmd�̏ꏊ�܂ł�OFFSET
    OS_Printf("nsbtxOffset=%08x\n",fileHeader->nsbtxOffset);    ///< �t�@�C���̐擪����nsbtx�̏ꏊ�܂ł�OFFSET
    OS_Printf("bhcOffset=%08x\n",fileHeader->bhcOffset);	    ///< �t�@�C���̐擪����bhc�̏ꏊ�܂ł�OFFSET
    OS_Printf("vertexOffset=%08x\n",fileHeader->vertexOffset);   ///< �t�@�C���̐擪����@���{�A�g���r���[�g�̏ꏊ�܂ł�OFFSET
    OS_Printf("positionOffset=%08x\n",fileHeader->positionOffset); ///< �t�@�C���̐擪����|�W�V�����̏ꏊ�܂ł�OFFSET
    OS_Printf("endPos=%08x\n",fileHeader->endPos);         ///< �t�@�C���̐擪����|�W�V�����̍Ō�܂ł�OFFSET
#endif
			CheckHeightData(
					(void*)((u32)mem + fileHeader->bhcOffset),
					(MHI_PTR)((u8*)mem - sizeof(MAP_HEIGHT_INFO))
					);
		}
		GFL_G3D_MAP_SetTransVramParam( g3Dmap );	//�e�N�X�`���]���ݒ�
		GFL_G3D_MAP_MakeRenderObj( g3Dmap );

		ldst->seq = TEX_TRANS;
		break;

	case TEX_TRANS:
		if( GFL_G3D_MAP_TransVram(g3Dmap) == FALSE ){
			ldst->seq = GFL_G3D_MAP_LOAD_IDLING;
			return FALSE;
		}
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
void FieldGetAttr_BridgeFile( GFL_G3D_MAP_ATTRINFO* attrInfo, const void* mapdata, 
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
#if 0
	fx32			grid_w, grid_x, grid_z;
	u32				grid_idx;
	VecFx32			pos, vecN;
	fx32			by, valD;
	NormalVtxSt*	nvs;
	u32				attrAdrs;
	BridgePackHeaderSt* fileHeader;
	void * nmapdata = ((u8*)mapdata + sizeof(MAP_HEIGHT_INFO));
	fileHeader = (BridgePackHeaderSt*)nmapdata;
	attrAdrs = (u32)nmapdata + fileHeader->vertexOffset;

	VEC_Set( &pos, posInBlock->x + map_width/2, posInBlock->y, posInBlock->z + map_width/2 );
	//�O���b�h�����擾
	grid_w = map_width / MAP_GRIDCOUNT;	//�}�b�v�����O���b�h���ŕ���
	grid_idx = ( pos.z / grid_w ) * MAP_GRIDCOUNT + ( pos.x / grid_w );
	grid_x = pos.x % grid_w;
	grid_z = pos.z % grid_w;

	//���擾(���̎������Ⴄ�̂Ŗ@���x�N�g����Z���])
	nvs = (NormalVtxSt*)(attrAdrs + grid_idx * sizeof(NormalVtxSt));

	//�O���b�h���O�p�`�̔���
	if( nvs->tryangleType == 0 ){
		//0-2-1,3-1-2�̃p�^�[��
		if( grid_x + grid_z < grid_w ){
			VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
			valD = nvs->vecN1_D;
		} else {
			VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
			valD = nvs->vecN2_D;
		}
	} else {
		//2-3-0,1-0-3�̃p�^�[��
		if( grid_x > grid_z ){
			VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
			valD = nvs->vecN1_D;
		} else {
			VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
			valD = nvs->vecN2_D;
		}
	}
	//vecN = attrInfo->mapAttr[0];...�K������O�p�`�̖@���x�N�g��
	VEC_Set( &vecN, 
			attrInfo->mapAttr[0].vecN.x, attrInfo->mapAttr[0].vecN.y, attrInfo->mapAttr[0].vecN.z );
	by = -( FX_Mul(vecN.x, posInBlock->x) + FX_Mul(vecN.z, posInBlock->z) + valD );
	attrInfo->mapAttr[0].attr = nvs->attr;

	attrInfo->mapAttr[0].height = FX_Div( by, vecN.y ) + map_height;

	attrInfo->mapAttrCount = 1;
#endif
	{
		fx32 outY;
		BridgePackHeaderSt * fileHeader = (void*)((u8*)mapdata + sizeof(MAP_HEIGHT_INFO));
		MHI_PTR mhi = (MHI_PTR)mapdata;
		GetHeightForBlock(posInBlock->y, posInBlock->x, posInBlock->z, mhi, &outY);
		attrInfo->mapAttr[0].attr = 0;
		attrInfo->mapAttr[0].height = outY + map_height;
		VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, 0, FX16_ONE, 0 );
		OS_Printf("Height:%08x\n",outY + map_height);
		attrInfo->mapAttrCount = 1;
	}
}

//============================================================================================
/**
 * �ȉ��̓|�P����DP����R�s�y
 */
//============================================================================================
static inline fx32 Min(fx32 n1,fx32 n2)
{
	if(n1 < n2){
		return n1;
	}else{
		return n2;
	}
}

static inline fx32 Max(fx32 n1,fx32 n2)
{
	if(n1 > n2){
		return n1;
	}else{
		return n2;
	}

}

//�O�p�`�̓��O����ZX�ˉe��
static inline BOOL BG3D_CheckTriangleIObyZX(const VecFx32 inT_Vtx,const VecFx32 inVtx1,const VecFx32 inVtx2,const VecFx32 inVtx3)
{
	//�O�ς����߂�iVEC_CrossProduct���g�p�j
	fx32 cross;
	fx32 cross1;
	fx32 cross2;
	fx32 cross3;
	fx32 cross_total;
	cross = FX_Mul(inVtx3.z,inVtx1.x-inVtx2.x) + FX_Mul(inVtx1.z,inVtx2.x-inVtx3.x) + FX_Mul(inVtx2.z,inVtx3.x-inVtx1.x);
	cross1 = FX_Mul(inVtx2.z,inT_Vtx.x-inVtx1.x) + FX_Mul(inT_Vtx.z,inVtx1.x-inVtx2.x) + FX_Mul(inVtx1.z,inVtx2.x-inT_Vtx.x);
	cross2 = FX_Mul(inVtx3.z,inT_Vtx.x-inVtx2.x) + FX_Mul(inT_Vtx.z,inVtx2.x-inVtx3.x) + FX_Mul(inVtx2.z,inVtx3.x-inT_Vtx.x);
	cross3 = FX_Mul(inVtx1.z,inT_Vtx.x-inVtx3.x) + FX_Mul(inT_Vtx.z,inVtx3.x-inVtx1.x) + FX_Mul(inVtx3.z,inVtx1.x-inT_Vtx.x);

	if (cross < 0){
///		OS_Printf("out\n");
		return FALSE;
	}
	if (cross1 < 0){
///		OS_Printf("out1\n");
		return FALSE;
	}
	if (cross2 < 0){
///		OS_Printf("out2\n");
		return FALSE;
	}
	if (cross3 < 0){
///		OS_Printf("out3\n");
		return FALSE;
	}
#if 0
	OS_Printf("cross:%d\n",cross);
	OS_Printf("cross1:%d\n",cross1);
	OS_Printf("cross2:%d\n",cross2);
	OS_Printf("cross3:%d\n",cross3);
	OS_Printf("cross_total:%d\n",cross_total);
	OS_Printf("z:%x\n",inT_Vtx.z);
	OS_Printf("x:%x\n",inT_Vtx.x);
	OS_Printf("z1:%x\n",inVtx1.z);
	OS_Printf("x1:%x\n",inVtx1.x);
	OS_Printf("z2:%x\n",inVtx2.z);
	OS_Printf("x2:%x\n",inVtx2.x);
	OS_Printf("z3:%x\n",inVtx3.z);
	OS_Printf("x3:%x\n",inVtx3.x);
#endif	
	return TRUE;
}

extern VecFx32 MoveRevise(fx32 inMoveVal,VecFx32 inNewVec,VecFx32 inOldVec);
#include "height_check.c"

extern void SetupHeightData(const char  *path, MHI_PTR outMapHeightInfo, u8 * inHeightMem);
#include "height_load.c"

