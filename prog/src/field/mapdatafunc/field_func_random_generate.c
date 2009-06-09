//============================================================================================
/**
 * @file	field_func_wbnormal_file.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "field_func_random_generate.h"

#include "../field_buildmodel.h"
#include "../field_g3dmap_exwork.h"	// GFL_G3D_MAP�g�����[�N


#include "savedata/save_control.h"
#include "savedata/randommap_save.h"
#include "debug/debugwin_sys.h"

//============================================================================================
/**
 *
 *
 *
 * @brief	�f�e�}�b�v�G�f�B�^���쐬�����f�[�^����������֐��S
 *  //�z���C�g�t�H���X�g�E�u���b�N�V�e�B�p�����_������
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

typedef enum
{
  RMM_LEFT_TOP,
  RMM_RIGHT_TOP,
  RMM_LEFT_BOTTOM,
  RMM_RIGHT_BOTTOM,
}RANDOM_MAP_MAPPOS;

static const RANDOM_MAP_MAPPOS FieldFuncRandom_CheckMapPos( GFL_G3D_MAP* g3Dmap , const u8 mapIdx );
static const u8 FieldFuncRandom_GetBilduingHeight( const u8 idxTop , const u8 idxLeft );

BOOL FieldLoadMapData_RandomGenerate( GFL_G3D_MAP* g3Dmap, void * exWork )
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
			Dp3packHeaderSt*	fileHeader;

			//�w�b�_�[�ݒ�
			GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );
			fileHeader = (Dp3packHeaderSt*)mem;
			//���f�����\�[�X�ݒ�
			GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + fileHeader->nsbmdOffset));
			//�e�N�X�`�����\�[�X�ݒ�
			//>>GFL_G3D_MAP_CreateResourceTex(g3Dmap, (void*)((u32)mem + fileHeader->nsbtxOffset)); 
			//�z�u�I�u�W�F�N�g�ݒ�
			if( fileHeader->positionOffset != fileHeader->endPos )
			{
        //���x���擾(��
        SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
        RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
        u16 level = RANDOMMAP_SAVE_GetCityLevel( mapSave );

				LayoutFormat* layout = (LayoutFormat*)((u32)mem + fileHeader->positionOffset);
				PositionSt* objStatus = (PositionSt*)&layout->posData;
				GFL_G3D_MAP_GLOBALOBJ_ST status;
				int i, count = layout->count;
        FIELD_BMODEL_MAN * bm = FLD_G3D_MAP_EXWORK_GetBModelMan(p_exwork);
        const u8 mapIndex = FLD_G3D_MAP_EXWORK_GetMapIndex(p_exwork);
        const RANDOM_MAP_MAPPOS mapPos = FieldFuncRandom_CheckMapPos( g3Dmap , mapIndex );
        u8 x,z;
        fx32 top,left;
        u8  idxTop,idxLeft;
        switch(mapPos)
        {
        case RMM_LEFT_TOP:
          top = FX32_CONST(-144.0f);
          left = FX32_CONST(-144.0f);
          idxTop = 0;
          idxLeft = 0;
          break;

        case RMM_RIGHT_TOP:
          top = FX32_CONST(-144.0f);
          left = FX32_CONST(-176.0f);
          idxTop = 0;
          idxLeft = 3;
          break;

        case RMM_LEFT_BOTTOM:
          top = FX32_CONST(-176.0f);
          left = FX32_CONST(-144.0f);
          idxTop = 3;
          idxLeft = 0;
          break;

        case RMM_RIGHT_BOTTOM:
          top = FX32_CONST(-176.0f);
          left = FX32_CONST(-176.0f);
          idxTop = 3;
          idxLeft = 3;
          break;
        }
        i=0;
				for( x=0; x<3; x++ )
				{
  				for( z=0; z<3; z++ )
  				{
            s8 height = FieldFuncRandom_GetBilduingHeight(idxTop+z,idxLeft+x) + level - 10;
            if( height > 10 )
            {
              height = 10;
            }
            
            if( height > 0 )
            {
    					status.id = height-1;
    					VEC_Set( &status.trans, 
    							left+FX32_CONST( x*160.0f ), 0, top+FX32_CONST( z*160.0f) );
  					  status.rotate = 0;
  					  GFL_G3D_MAP_ResistGlobalObj( g3Dmap, &status, i );
  					  i++;
  					}
          }
        }
        /*
				for( i=0; i<count; i++ )
				{
					//status.id = FIELD_BMODEL_MAN_GetEntryIndex(bm, objStatus[i].resourceID);
					status.id = GFL_STD_MtRand0(15);
					VEC_Set( &status.trans, 
							objStatus[i].xpos, objStatus[i].ypos, -objStatus[i].zpos );
					status.rotate = (u16)(objStatus[i].rotate);
					OS_Printf("[%f:%f:%f]\n",FX_FX32_TO_F32(status.trans.x),FX_FX32_TO_F32(status.trans.y),FX_FX32_TO_F32(status.trans.z));
					//OS_Printf("bm id = %d, rotate = %04x\n",i, status.rotate);
					GFL_G3D_MAP_ResistGlobalObj( g3Dmap, &status, i );
				}
				*/
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
void FieldGetAttr_RandomGenerate( GFL_G3D_MAP_ATTRINFO* attrInfo, const void* mapdata, 
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
	fx32			grid_w, grid_x, grid_z;
	u32				grid_idx;
	VecFx32			pos, vecN;
	fx32			by, valD;
	NormalVtxSt*	nvs;
	Dp3packHeaderSt* fileHeader = (Dp3packHeaderSt*)mapdata;
	u32				attrAdrs = (u32)mapdata + fileHeader->vertexOffset;

	VEC_Set( &pos, posInBlock->x + map_width/2, posInBlock->y, posInBlock->z + map_width/2 );
	//�O���b�h�����擾
	grid_w = map_width / MAP_GRIDCOUNT;	//�}�b�v�����O���b�h���ŕ���
	grid_idx = ( pos.z / grid_w ) * MAP_GRIDCOUNT + ( pos.x / grid_w );
	grid_x = pos.x % grid_w;
	grid_z = pos.z % grid_w;

	//���擾(���̎������Ⴄ�̂Ŗ@���x�N�g����Z���])
	nvs = (NormalVtxSt*)(attrAdrs + sizeof(NormalVtxFormat) + grid_idx * sizeof(NormalVtxSt));

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
	VEC_Set( &vecN, 
			attrInfo->mapAttr[0].vecN.x, attrInfo->mapAttr[0].vecN.y, attrInfo->mapAttr[0].vecN.z );
	by = -( FX_Mul(vecN.x, posInBlock->x) + FX_Mul(vecN.z, posInBlock->z) + valD );
	attrInfo->mapAttr[0].attr = nvs->attr;

	attrInfo->mapAttr[0].height = FX_Div( by, vecN.y ) + map_height;

	attrInfo->mapAttrCount = 1;
}


//MAP�̈ʒu���`�F�b�N
static const RANDOM_MAP_MAPPOS FieldFuncRandom_CheckMapPos( GFL_G3D_MAP* g3Dmap , const u8 mapIdx )
{
  VecFx32 pos;
  GFL_G3D_MAP_GetTrans( g3Dmap , &pos );
  OS_Printf("[%d][%f:%f:%f]\n",mapIdx,FX_FX32_TO_F32(pos.x),FX_FX32_TO_F32(pos.y),FX_FX32_TO_F32(pos.z));
/*
  if( pos.x < FX32_CONST( 256.0f+256.0f ) )
  {
    if( pos.z < FX32_CONST( 256.0f+256.0f ) )
    {
      return RMM_LEFT_TOP;
    }
    else
    {
      return RMM_LEFT_BOTTOM;
    }
  }
  else
  {
    if( pos.z < FX32_CONST( 256.0f+256.0f ) )
    {
      return RMM_RIGHT_TOP;
    }
    else
    {
      return RMM_RIGHT_BOTTOM;
    }
  }
*/
  return (RANDOM_MAP_MAPPOS)(mapIdx);
}

static const u8 FieldFuncRandom_GetBilduingHeight( const u8 idxTop , const u8 idxLeft )
{
  static const u8 idxArr[6][6] =
  {
    {1,3,6,5,4,2},
    {2,5,7,8,6,3},
    {4,7,10,9,7,5},
    {3,6,8,10,6,4},
    {2,4,5,6,5,3},
    {1,2,4,3,2,1},
  };
  
  return idxArr[idxTop][idxLeft];
}


//�f�o�b�O����

static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item );


void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId )
{
  DEBUGWIN_AddGroupToTop( 10 , "RandomMap" , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityLevel ,DEBWIN_Draw_CityLevel , 
                             NULL , 10 , heapId );
}

void FIELD_FUNC_RANDOM_GENERATE_TermDebug( void )
{
  DEBUGWIN_RemoveGroup( 10 );
}

static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u16 level = RANDOMMAP_SAVE_GetCityLevel( mapSave );
  
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    if( level < 20 )
    {
      level++;
      RANDOMMAP_SAVE_SetCityLevel( mapSave , level );
      DEBUGWIN_RefreshScreen();
    }
  }
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    if( level > 0 )
    {
      level--;
      RANDOMMAP_SAVE_SetCityLevel( mapSave , level );
      DEBUGWIN_RefreshScreen();
    }
  }
}

static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u16 level = RANDOMMAP_SAVE_GetCityLevel( mapSave );
  DEBUGWIN_ITEM_SetNameV( item , "Level[%d]",level );
}
