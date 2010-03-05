//============================================================================================
/**
 * @file	field_func_wbnormal_file.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "field/field_const.h"

#include "field_func_wbnormal_file.h"

#include "../field_buildmodel.h"
#include "../field_g3dmap_exwork.h"	// FLD_G3D_MAP�g�����[�N

#include "mapdata_attr.h"

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
	FILE_LOAD_START = FLD_G3D_MAP_LOAD_START,
	FILE_LOAD,
	RND_CREATE,
};

BOOL FieldLoadMapData_WBNormalFile( FLD_G3D_MAP* g3Dmap, void * exWork )
{
	FLD_G3D_MAP_LOAD_STATUS* ldst;
	FLD_G3D_MAP_EXWORK* p_exwork;	// FLD_G3D_MAP�g�����[�N


	// �g�����[�N�擾
	p_exwork = exWork;

	FLD_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );

	switch( ldst->seq ){

	case FILE_LOAD_START:
		FLD_G3D_MAP_ResetLoadStatus(g3Dmap);

		//���f���f�[�^���[�h�J�n
		{
			u32		datID;
			FLD_G3D_MAP_GetLoadDatID( g3Dmap, &datID );
			FLD_G3D_MAP_StartFileLoad( g3Dmap, datID );
		}
		ldst->seq = FILE_LOAD;
		break;

	case FILE_LOAD:
    {
      BOOL rc;
      rc = FLD_G3D_MAP_ContinueFileLoad(g3Dmap);
		  if( rc ){
        break;
		  }
    }
    ldst->mdlLoaded = TRUE;
    ldst->texLoaded = TRUE;
    ldst->attrLoaded = TRUE;

    ldst->seq = RND_CREATE;
    //break through
    
	case RND_CREATE:
		//�����_�[�쐬
		{
			void*				mem;
			WBGridMapPackHeaderSt*	fileHeader;

			//�w�b�_�[�ݒ�
			FLD_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );
			fileHeader = (WBGridMapPackHeaderSt*)mem;
			//���f�����\�[�X�ݒ�
			FLD_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + fileHeader->nsbmdOffset));
			//�e�N�X�`�����\�[�X�ݒ�
			//>>FLD_G3D_MAP_CreateResourceTex(g3Dmap, (void*)((u32)mem + fileHeader->nsbtxOffset)); 
			//�z�u�I�u�W�F�N�g�ݒ�
			if( fileHeader->positionOffset != fileHeader->endPos ){
				LayoutFormat* layout = (LayoutFormat*)((u32)mem + fileHeader->positionOffset);
				PositionSt* objStatus = (PositionSt*)&layout->posData;
        FIELD_BMODEL_MAN * bm = FLD_G3D_MAP_EXWORK_GetBModelMan(p_exwork);
        FIELD_BMODEL_MAN_ResistAllMapObjects(bm, g3Dmap, objStatus, layout->count);
			}
		}
		//>>FLD_G3D_MAP_SetTransVramParam( g3Dmap );	//�e�N�X�`���]���ݒ�
		FLD_G3D_MAP_MakeRenderObj( g3Dmap );

		// �n�ʃA�j���[�V�����̐ݒ�
		if( FLD_G3D_MAP_EXWORK_IsGranm( p_exwork ) ){
			FIELD_GRANM_WORK* p_granm;

			p_granm = FLD_G3D_MAP_EXWORK_GetGranmWork( p_exwork );
			FIELD_GRANM_WORK_Bind( p_granm, 
					FLD_G3D_MAP_GetResourceMdl(g3Dmap), FLD_G3D_MAP_GetResourceTex(g3Dmap), 
					FLD_G3D_MAP_GetRenderObj(g3Dmap) );
		}

    ldst->seq = FLD_G3D_MAP_LOAD_IDLING;
    return FALSE;
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
void FieldGetAttr_WBNormalFile( FLD_G3D_MAP_ATTRINFO* attrInfo, const void* mapdata, 
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
	WBGridMapPackHeaderSt* fileHeader = (WBGridMapPackHeaderSt*)mapdata;
	u32				attrAdrs = (u32)mapdata + fileHeader->vertexOffset;

  MAPDATA_ATR_GetAttrFunc( attrInfo, 0, attrAdrs, posInBlock, map_width, map_height );
	attrInfo->mapAttrCount = 1;
}

