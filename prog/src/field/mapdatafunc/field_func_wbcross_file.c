//============================================================================================
/**
 * @file    field_func_wbcorss_file.c
 * @brief   �m�[�}���O���b�h���̌����V�O�l�`���[
 * @author	Miyuki Iwasawa
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "field/field_const.h"

#include "field_func_wbcross_file.h"

#include "../field_buildmodel.h"
#include "../field_g3dmap_exwork.h"	// GFL_G3D_MAP�g�����[�N
#include "field/map_attr.h"	// GFL_G3D_MAP�g�����[�N

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
	FILE_LOAD_START = GFL_G3D_MAP_LOAD_START,
	FILE_LOAD,
	RND_CREATE,
	TEX_TRANS,
};

enum{
 CROSS_LAYER_DEF,
 CROSS_LAYER_EX,
 CROSS_LAYER_NUM,
};


///�v���g�^�C�v
static void fgr_WBCrossFileCore( GFL_G3D_MAP_ATTRINFO* attrInfo, const u8 idx, const u32 attrAdrs,
    const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height );

BOOL FieldLoadMapData_WBCrossFile( GFL_G3D_MAP* g3Dmap, void * exWork )
{
	GFL_G3D_MAP_LOAD_STATUS* ldst;
	FLD_G3D_MAP_EXWORK* p_exwork;	// GFL_G3D_MAP�g�����[�N


	// �g�����[�N�擾
	p_exwork = exWork;

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );

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
			WBGridCrossMapPackHeaderSt*	fileHeader;

			//�w�b�_�[�ݒ�
			GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );
			fileHeader = (WBGridCrossMapPackHeaderSt*)mem;
			//���f�����\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + fileHeader->nsbmdOffset));
			//�e�N�X�`�����\�[�X�ݒ�
			//>>GFL_G3D_MAP_CreateResourceTex(g3Dmap, (void*)((u32)mem + fileHeader->nsbtxOffset)); 
			//�z�u�I�u�W�F�N�g�ݒ�
			if( fileHeader->positionOffset != fileHeader->endPos ){
				LayoutFormat* layout = (LayoutFormat*)((u32)mem + fileHeader->positionOffset);
				PositionSt* objStatus = (PositionSt*)&layout->posData;
        FIELD_BMODEL_MAN * bm = FLD_G3D_MAP_EXWORK_GetBModelMan(p_exwork);
        FIELD_BMODEL_MAN_ResistAllMapObjects(bm, g3Dmap, objStatus, layout->count);
			}
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
		//>>if( GFL_G3D_MAP_TransVram(g3Dmap) == FALSE ){
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
void FieldGetAttr_WBCrossFile( GFL_G3D_MAP_ATTRINFO* attrInfo, const void* mapdata, 
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
  GFL_G3D_MAP_ATTRINFO attr;

	WBGridCrossMapPackHeaderSt* fileHeader = (WBGridCrossMapPackHeaderSt*)mapdata;

  MI_CpuClear8(&attr,sizeof(GFL_G3D_MAP_ATTRINFO));

  //�f�t�H���g�K�w���[�h
  fgr_WBCrossFileCore( &attr, CROSS_LAYER_DEF, (u32)mapdata + fileHeader->vertexOffset,
    posInBlock, map_width, map_height );
  
  //���̌����K�w���[�h
  fgr_WBCrossFileCore( &attr, CROSS_LAYER_EX, (u32)mapdata + fileHeader->exAttrOffset,
    posInBlock, map_width, map_height );

  {
    int i;
    u8 target = CROSS_LAYER_DEF;
    fx32 diff[CROSS_LAYER_NUM] = {0,0};

    //Ex���i���\�̎�
    if(!MAPATTR_GetHitchFlag(attr.mapAttr[CROSS_LAYER_EX].attr)){
      //Def���i���s�Ȃ�Ex�����
      if(MAPATTR_GetHitchFlag(attr.mapAttr[CROSS_LAYER_DEF].attr)){
        target = CROSS_LAYER_EX;
      }else{
        //�����i���\�Ȃ�A�����̍����̐�Βl�����ċ߂������^�[�Q�b�g�Ƃ���
        for(i = 0;i < CROSS_LAYER_NUM;i++){
          diff[i] = posInBlock->y - attr.mapAttr[i].height;
          if(diff[i] < 0){
            diff[i] *= -1;
          }
        }
        if(diff[CROSS_LAYER_EX] < diff[CROSS_LAYER_DEF]){
          target = CROSS_LAYER_EX;
        }
      }
    }
    attrInfo->mapAttr[0] = attr.mapAttr[target];
  }
  attrInfo->mapAttrCount = 1;
}

/**
  @brief  �A�g���r���[�g�t�@�C���̉���
 */
static void fgr_WBCrossFileCore( GFL_G3D_MAP_ATTRINFO* attrInfo, const u8 idx, const u32 attrAdrs,
    const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
	fx32			grid_w, grid_x, grid_z;
	u32				grid_idx;
	u32				grid_count;
	VecFx32			pos, vecN;
	fx32			by, valD;
	NormalVtxSt*	nvs;

  if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_R){
    OS_Printf("GetAttr idx%d = y=%08x\n",idx,posInBlock->y);
  }

	VEC_Set( &pos, posInBlock->x + map_width/2, posInBlock->y, posInBlock->z + map_width/2 );
	//�O���b�h�����擾
	grid_count = (map_width>>FX32_SHIFT) /  FIELD_CONST_GRID_SIZE;
	grid_w = FIELD_CONST_GRID_FX32_SIZE;	//�}�b�v�����O���b�h���ŕ���
	grid_idx = ( pos.z / grid_w ) * grid_count + ( pos.x / grid_w );
	grid_x = pos.x % grid_w;
	grid_z = pos.z % grid_w;

	//���擾(���̎������Ⴄ�̂Ŗ@���x�N�g����Z���])
	nvs = (NormalVtxSt*)(attrAdrs + sizeof(NormalVtxFormat) + grid_idx * sizeof(NormalVtxSt));

	//�O���b�h���O�p�`�̔���
	if( nvs->tryangleType == 0 ){
		//0-2-1,3-1-2�̃p�^�[��
		if( grid_x + grid_z < grid_w ){
			VEC_Fx16Set( &attrInfo->mapAttr[idx].vecN, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
			valD = nvs->vecN1_D;
		} else {
			VEC_Fx16Set( &attrInfo->mapAttr[idx].vecN, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
			valD = nvs->vecN2_D;
		}
	} else {
		//2-3-0,1-0-3�̃p�^�[��
		if( grid_x > grid_z ){
			VEC_Fx16Set( &attrInfo->mapAttr[idx].vecN, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
			valD = nvs->vecN1_D;
		} else {
			VEC_Fx16Set( &attrInfo->mapAttr[idx].vecN, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
			valD = nvs->vecN2_D;
		}
	}
	VEC_Set( &vecN, 
			attrInfo->mapAttr[idx].vecN.x, attrInfo->mapAttr[idx].vecN.y, attrInfo->mapAttr[idx].vecN.z );
	by = -( FX_Mul(vecN.x, posInBlock->x) + FX_Mul(vecN.z, posInBlock->z) + valD );
	attrInfo->mapAttr[idx].attr = nvs->attr;

	attrInfo->mapAttr[idx].height = FX_Div( by, vecN.y ) + map_height;
}

