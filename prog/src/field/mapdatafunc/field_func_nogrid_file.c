//============================================================================================
/**
 * @file	field_func_nogrid_file.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include <gflib.h>
#include "system\main.h"

#include "field_func_nogrid_file.h"


#include "../field_g3dmap_exwork.h"	// GFL_G3D_MAP�g�����[�N

#include "height.h"

/// �S�����p�b�N�����t�@�C���̃w�b�_�[
typedef struct {
    u16 DataID;         ////< NOGRIDPACK_HEADER
    u16 dummy1;
    u32 nsbmdOffset;    ///< �t�@�C���̐擪����nsbmd�̏ꏊ�܂ł�OFFSET
    u32 positionOffset; ///< �t�@�C���̐擪����|�W�V�����̏ꏊ�܂ł�OFFSET
    u32 endPos;         ///< �t�@�C���̐擪����|�W�V�����̍Ō�܂ł�OFFSET
} NoGridPackHeaderSt;


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

BOOL FieldLoadMapData_NoGridFile( GFL_G3D_MAP* g3Dmap, void * exWork )
{
	GFL_G3D_MAP_LOAD_STATUS* ldst;
	FLD_G3D_MAP_EXWORK* p_exwork;	// GFL_G3D_MAP�g�����[�N

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );

	// �g�����[�N�擾
	p_exwork = exWork;

	switch( ldst->seq ){

	case FILE_LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);

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
			NoGridPackHeaderSt*	fileHeader;
			//�w�b�_�[�ݒ�
			GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );
			fileHeader = (NoGridPackHeaderSt*)mem;

			//���f�����\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + fileHeader->nsbmdOffset));
			//�z�u�I�u�W�F�N�g�ݒ�
			if( fileHeader->positionOffset != fileHeader->endPos ){
				LayoutFormat* layout = (LayoutFormat*)((u32)mem + fileHeader->positionOffset);
				PositionSt* objStatus = (PositionSt*)&layout->posData;
        FIELD_BMODEL_MAN * bm = FLD_G3D_MAP_EXWORK_GetBModelMan(p_exwork);
				GFL_G3D_MAP_GLOBALOBJ_ST status;
				int i, count = layout->count;

				for( i=0; i<count; i++ ){
					FIELD_BMODEL_MAN_ResistMapObject( bm, g3Dmap, &objStatus[i], i );

				}
			}
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
		{
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
void FieldGetAttr_NoGridFile( GFL_G3D_MAP_ATTRINFO* attrInfo, const void* mapdata, 
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
  // �A�g���r���[�g���������Ȃ��I
  attrInfo->mapAttrCount = 0;
}

