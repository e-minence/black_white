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


#include "../field_g3dmap_exwork.h"	// GFL_G3D_MAP�g�����[�N

#include "height.h"

/// �S�����p�b�N�����t�@�C���̃w�b�_�[
typedef struct {
    u16 DataID;         ////< DP3PACK_HEADER
    u16 dummy1;
    u32 nsbmdOffset;    ///< �t�@�C���̐擪����nsbmd�̏ꏊ�܂ł�OFFSET
    //u32 nsbtxOffset;    ///< �t�@�C���̐擪����nsbtx�̏ꏊ�܂ł�OFFSET
    u32 bhcOffset;	    ///< �t�@�C���̐擪����bhc�̏ꏊ�܂ł�OFFSET
    u32 positionOffset; ///< �t�@�C���̐擪����|�W�V�����̏ꏊ�܂ł�OFFSET
    u32 endPos;         ///< �t�@�C���̐擪����|�W�V�����̍Ō�܂ł�OFFSET
} BridgePackHeaderSt;


void CheckHeightData(const void *mem, MHI_PTR outMHI);
void SetInvalidHeightData(MHI_PTR outMap3DInfo);
extern BOOL GetHeightForBlock(const fx32 inNowY, const fx32 inX, const fx32 inZ,
		MHI_CONST_PTR inMap3DInfo,fx32 *outY);
extern int GetDPFormatHeight(const fx32 inX, const fx32 inZ, MHI_CONST_PTR inMap3DInfo, GFL_G3D_MAP_ATTRINFO* attrInfo);
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

BOOL FieldLoadMapData_BridgeFile( GFL_G3D_MAP* g3Dmap, void * exWork )
{
	GFL_G3D_MAP_LOAD_STATUS* ldst;
	FLD_G3D_MAP_EXWORK* p_exwork;	// GFL_G3D_MAP�g�����[�N

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );

	// �g�����[�N�擾
	p_exwork = exWork;

	switch( ldst->seq ){

	case FILE_LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);

		SetInvalidHeightData((MHI_PTR)ldst->mOffs);
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
			//>>GFL_G3D_MAP_CreateResourceTex(g3Dmap, (void*)((u32)mem + fileHeader->nsbtxOffset)); 
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
		//>>GFL_G3D_MAP_SetTransVramParam( g3Dmap );	//�e�N�X�`���]���ݒ�
		GFL_G3D_MAP_MakeRenderObj( g3Dmap );



		// �n�ʃA�j���[�V�����̐ݒ�
		if( FLD_G3D_MAP_EXWORK_IsGranm( p_exwork ) ){
			FIELD_GRANM_WORK* p_granm;

			p_granm = FLD_G3D_MAP_EXWORK_GetGranmWork( p_exwork );
			FIELD_GRANM_WORK_Bind( p_granm, 
					GFL_G3D_MAP_GetResourceMdl(g3Dmap), GFL_G3D_MAP_GetResourceTex(g3Dmap), 
					GFL_G3D_MAP_GetRenderObj(g3Dmap) );
		}

		ldst->seq = TEX_TRANS;
		break;

	case TEX_TRANS:
		//>>if( GFL_G3D_MAP_TransVram(g3Dmap) == FALSE )
		{
			ldst->seq = GFL_G3D_MAP_LOAD_IDLING;
			return FALSE;
		}
		break;
	}
	return TRUE;
}

#include "../field_easytp.h"
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
	BridgePackHeaderSt * fileHeader = (void*)((u8*)mapdata + sizeof(MAP_HEIGHT_INFO));
	MHI_PTR mhi = (MHI_PTR)mapdata;
	u32 pol_count;
	pol_count = GetDPFormatHeight(posInBlock->x, posInBlock->z, mhi, attrInfo);
	attrInfo->mapAttrCount = pol_count;
#if 0
	if (FieldEasyTP_TouchDirGet() == FLDEASYTP_TCHDIR_UP) {
		fx32 outY;
		GetHeightForBlock(posInBlock->y, posInBlock->x, posInBlock->z, mhi, &outY);
	}
#endif
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

static void DEBUG_PutVector(const VecFx32 * vec)
{
	OS_Printf("(%08x,%08x,%08x)",vec->x, vec->y, vec->z);
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
		return FALSE;
	}
	if (cross1 < 0){
		return FALSE;
	}
	if (cross2 < 0){
		return FALSE;
	}
	if (cross3 < 0){
		return FALSE;
	}
	return TRUE;
}

extern VecFx32 MoveRevise(fx32 inMoveVal,VecFx32 inNewVec,VecFx32 inOldVec);
#include "height_check.c"

extern void SetupHeightData(const char  *path, MHI_PTR outMapHeightInfo, u8 * inHeightMem);
#include "height_load.c"

