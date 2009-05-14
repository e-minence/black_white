//============================================================================================
/**
 * @file	field_g3d_mapper.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "arc\arc_def.h"	// �A�[�J�C�u�f�[�^

#include "mapdatafunc/field_func_mapeditor_file.h"
#include "mapdatafunc/field_func_pmcustom_file.h"
#include "mapdatafunc/field_func_bridge_file.h"
#include "mapdatafunc/field_func_wbnormal_file.h"

#include "field_g3d_mapper.h"
#include "fieldmap_resist.h"
#include "field_g3dmap_exwork.h"
#include "field_ground_anime.h"

#include "system/g3d_tool.h"

//============================================================================================
/**
 *
 *
 *
 * @brief	�R�c�}�b�v�R���g���[��
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
#define MAPID_NULL			(0xffffffff)
#define MAPARC_NULL			(0xffffffff)
//------------------------------------------------------------------
//------------------------------------------------------------------

#define	MAPMDL_SIZE		(0xf000)	//���f���f�[�^�p�������m�ۃT�C�Y 
#define	MAPTEX_SIZE		(0) 	    //�e�N�X�`���f�[�^�p�u�q�`�l���������m�ۃT�C�Y 
#define	MAPATTR_SIZE	(0x6004) 	//�A�g���r���[�g�i�������j�p�������m�ۃT�C�Y 



#define GLOBAL_OBJ_COUNT	(64)
#define GLOBAL_OBJ_ANMCOUNT	(4)
#define GLOBAL_DDOBJ_COUNT	(32)
//------------------------------------------------------------------
typedef struct {
	u32			blockIdx;
	VecFx32		trans;
}BLOCKINFO;

//------------------------------------------------------------------
typedef struct {
	GFL_G3D_RES*	g3DresMdl;						//���f�����\�[�X(High Q)
	GFL_G3D_RES*	g3DresTex;						//�e�N�X�`�����\�[�X
	GFL_G3D_RES*	g3DresAnm[GLOBAL_OBJ_ANMCOUNT];	//�A�j�����\�[�X
	GFL_G3D_OBJ*	g3Dobj;							//�I�u�W�F�N�g�n���h��
}GLOBALOBJ_RES;

//-------------------------------------
// GFL_G3D_MAP�g�����[�N
struct _FLD_G3D_MAP_EXWORK{
	FIELD_GRANM_WORK* p_granm_wk;		// �n�ʃA�j���[�V����
	FIELD_BMODEL_MAN * bmodel_man;	// 
};


//-------------------------------------
///	�u���b�N�Ǘ����[�N
typedef struct {
	GFL_G3D_MAP*		g3Dmap;       // �}�b�v���
	FLD_G3D_MAP_EXWORK	g3DmapExWork; // �g�����[�N
	BLOCKINFO				blockInfo;    //  ���̏��
} BLOCK_WORK;

//-------------------------------------
///	�u���b�N�X�V���N�G�X�g���[�N
typedef struct {
  BLOCKINFO       newBlockInfo; //  ���N�G�X�g���
} BLOCK_NEWREQ;




//------------------------------------------------------------------
struct _FLD_G3D_MAPPER {
	HEAPID					heapID;

	FLDMAPPER_FILETYPE	g3DmapFileType;	//g3Dmap�t�@�C�����ʃ^�C�v�i���j
	fx32				blockWidth;		//�u���b�N�P�ӂ̕�
	fx32				blockHeight;		//�u���b�N����
	FLDMAPPER_MODE		mode;		//���샂�[�h
	u32					arcID;		//�O���t�B�b�N�A�[�J�C�u�h�c

	u16					sizex;		//���u���b�N��
	u16					sizez;		//�c�u���b�N��
	u32					totalSize;	//�z��T�C�Y
	const FLDMAPPER_MAPDATA*	blocks;	//���}�b�v�f�[�^
	
  FLDMAPPER_TEXTYPE gtexType;

  BLOCK_WORK*   blockWk;      // �u���b�N���[�N
  BLOCK_NEWREQ* blockNew;     // �u���b�N�X�V���N�G�X�g���[�N
  u8            blockXNum;    // �������z�u���u���b�N��
  u8            blockZNum;    // �������z�u�c�u���b�N��
  u16           blockNum;     // �������z�u�u���b�N����
  
	u32					nowBlockIdx;				
	VecFx32				posCont;

	VecFx32 globalDrawOffset;		//���ʍ��W�I�t�Z�b�g

	GFL_G3D_RES*			globalTexture;					//���ʒn�`�e�N�X�`��

	u32						globalObjResCount;				//���ʃI�u�W�F�N�g��
	GLOBALOBJ_RES*			globalObjRes;					//���ʃI�u�W�F�N�g

	GFL_G3D_MAP_GLOBALOBJ	globalObj;						//���ʃI�u�W�F�N�g

  FIELD_BMODEL_MAN* bmodel_man; //�z�u���f���}�l�W���[
	FIELD_GRANM*	granime;	// �n�ʃA�j���[�V�����V�X�e��
};

//------------------------------------------------------------------
typedef struct {
	u32 arcID;
	u32	datID;
	u32	inDatNum;
}MAKE_RES_PARAM;

enum {  MAKE_RES_NONPARAM = (0xffffffff) };

typedef struct {
	MAKE_RES_PARAM	mdl;
	MAKE_RES_PARAM	tex;
	MAKE_RES_PARAM	anm[GLOBAL_OBJ_ANMCOUNT];
}MAKE_OBJ_PARAM;

//------------------------------------------------------------------
static void CreateGlobalTexture( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData );
static void DeleteGlobalTexture( FLDMAPPER* g3Dmapper );
static void CreateGlobalObject( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData );
static void DeleteGlobalObject( FLDMAPPER* g3Dmapper );

static void CreateGlobalObj_forBModel(FLDMAPPER * g3Dmapper, FIELD_BMODEL_MAN * bmodel_man);
static void CreateGrobalObj_forTbl( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA_OBJTBL* gobjTbl );
static void CreateGrobalObj_forBin( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA_OBJBIN* gobjBin );

static void CreateGlobalObj( GLOBALOBJ_RES* objRes, const MAKE_OBJ_PARAM* param );
static void DeleteGlobalObj( GLOBALOBJ_RES* objRes );

static void CreateGlobalDDobj( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA_DDOBJ* resistData );
static void DeleteGlobalDDobj( FLDMAPPER* g3Dmapper );

static void GetMapperBlockIdxAll( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static void GetMapperBlockIdxXZ( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static void GetMapperBlockIdxY( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static BOOL	ReloadMapperBlock( FLDMAPPER* g3Dmapper, BLOCK_NEWREQ* new );

static s32 GetMapperBlockStartIdxEven( u32 blockNum, fx32 width, fx32 pos );
static s32 GetMapperBlockStartIdxUnEven( u32 blockNum, fx32 width, fx32 pos );



static FIELD_GRANM * createGroundAnime( u32 blockNum, GFL_G3D_RES* globalTexture,
    const FLDMAPPER_RESIST_GROUND_ANIME * resistGroundAnimeData, u32 heapID );
// FLD_G3D_MAP_EXWORK�@����֐�
static void FLD_G3D_MAP_ExWork_Init( FLD_G3D_MAP_EXWORK* p_wk, FLDMAPPER* g3Dmapper, u32 index );
static void FLD_G3D_MAP_ExWork_Exit( FLD_G3D_MAP_EXWORK* p_wk );
static void FLD_G3D_MAP_ExWork_ClearBlockData( FLD_G3D_MAP_EXWORK* p_wk );
static BOOL FLD_G3D_MAP_ExWork_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk );
static FIELD_GRANM_WORK* FLD_G3D_MAP_ExWork_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk );

static void BLOCKINFO_init(BLOCKINFO * info);
static void BLOCKINFO_SetBlockIdx(BLOCKINFO * info, u32 blockIdx);
static BOOL BLOCKINFO_IsInBlockData(const BLOCKINFO * info);
static u32 BLOCKINFO_GetBlockIdx(const BLOCKINFO * info);
static void BLOCKINFO_SetBlockTrans(BLOCKINFO * info, fx32 x, fx32 y, fx32 z );
static void BLOCKINFO_SetBlockTransVec(BLOCKINFO * info, const VecFx32* trans);
static void BLOCKINFO_GetBlockTrans(const BLOCKINFO * info, VecFx32* trans);

static const GFL_G3D_MAP_DDOBJ_DATA drawTreeData;
//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static const GFL_G3D_MAP_FILE_FUNC mapFileFuncTbl[] = {
	{ DP3PACK_HEADER, FieldLoadMapData_WBNormalFile, FieldGetAttr_WBNormalFile },
	{ DP3PACK_HEADER, FieldLoadMapData_MapEditorFile, FieldGetAttr_MapEditorFile },
	{ BRIDGEPACK_HEADER, FieldLoadMapData_BridgeFile, FieldGetAttr_BridgeFile },
	{ MAPFILE_FUNC_DEFAULT, FieldLoadMapData_PMcustomFile, FieldGetAttr_PMcustomFile },	//TableEnd&default	
};

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���쐬
 */
//------------------------------------------------------------------
FLDMAPPER*	FLDMAPPER_Create( HEAPID heapID )
{
	int i;
	FLDMAPPER* g3Dmapper = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDMAPPER) );

	g3Dmapper->heapID = heapID;

	VEC_Set( &g3Dmapper->posCont, 0, 0, 0 );
	g3Dmapper->sizex = 0;
	g3Dmapper->sizez = 0;
	g3Dmapper->totalSize = 0;
	g3Dmapper->blocks = NULL;
	g3Dmapper->blockWidth = 0;
	g3Dmapper->blockHeight = 0;
	g3Dmapper->mode = FLDMAPPER_MODE_SCROLL_XZ;
	g3Dmapper->arcID = MAPARC_NULL;
	
	g3Dmapper->globalObjRes = NULL;
	g3Dmapper->globalObjResCount = 0;

	g3Dmapper->globalObj.gobj = NULL;
	g3Dmapper->globalObj.gobjCount = 0;
	g3Dmapper->globalObj.gobjIDexchange = NULL;

	g3Dmapper->globalObj.gddobj = NULL;
	g3Dmapper->globalObj.gddobjCount = 0;
	g3Dmapper->globalObj.gddobjIDexchange = NULL;
	
  g3Dmapper->granime = NULL;
  //  �z�u���f���}�l�W���[����
  g3Dmapper->bmodel_man = FIELD_BMODEL_MAN_Create(g3Dmapper->heapID);
	return g3Dmapper;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���j��
 */
//------------------------------------------------------------------
void	FLDMAPPER_Delete( FLDMAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );

	FLDMAPPER_ReleaseData( g3Dmapper );	//�o�^���ꂽ�܂܂̏ꍇ��z�肵�č폜

  // �z�u���f���}�l�W���[�j��
  if(g3Dmapper->bmodel_man) 
  { 
    FIELD_BMODEL_MAN_Delete(g3Dmapper->bmodel_man);
    g3Dmapper->bmodel_man = NULL;
  }
	GFL_HEAP_FreeMemory( g3Dmapper );
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e�����C��
 */
//------------------------------------------------------------------
void	FLDMAPPER_Main( FLDMAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );

	if( g3Dmapper->blocks == NULL ){
		return;
	}
	for( i=0; i<g3Dmapper->blockNum; i++ ){
    BLOCKINFO_init(&g3Dmapper->blockNew[i].newBlockInfo);
	}

	switch( g3Dmapper->mode ){
	case FLDMAPPER_MODE_SCROLL_NONE: 
		GetMapperBlockIdxAll( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	default:
	case FLDMAPPER_MODE_SCROLL_XZ: 
		GetMapperBlockIdxXZ( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	case FLDMAPPER_MODE_SCROLL_Y: 
		GetMapperBlockIdxY( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	}
	ReloadMapperBlock( g3Dmapper, g3Dmapper->blockNew );

	//�u���b�N���䃁�C��
	for( i=0; i<g3Dmapper->blockNum; i++ ){
		GFL_G3D_MAP_Main( g3Dmapper->blockWk[i].g3Dmap );
	}

	//���݃u���b�N��index�擾
	{
		u32 blockx = FX_Whole( FX_Div( g3Dmapper->posCont.x, g3Dmapper->blockWidth ) );
		u32 blockz = FX_Whole( FX_Div( g3Dmapper->posCont.z, g3Dmapper->blockWidth ) );

		g3Dmapper->nowBlockIdx = blockz * g3Dmapper->sizex + blockx;
	}
	//�A�j���[�V�����R���g���[���i�b��Ńt���[�����[�v�����Ă��邾���j
	{
		GLOBALOBJ_RES* objRes;
		int j;

		for( i=0; i<g3Dmapper->globalObjResCount; i++ ){
			objRes = &g3Dmapper->globalObjRes[i];

			if( objRes->g3Dobj != NULL ){
#if 0
				for( j=0; j<GLOBAL_OBJ_ANMCOUNT; j++ ){
					GFL_G3D_OBJECT_LoopAnimeFrame( objRes->g3Dobj, j, FX32_ONE ); 
				}
#else
				GFL_G3D_OBJECT_LoopAnimeFrame( objRes->g3Dobj, 0, FX32_ONE ); 
#endif
			}
		}
	}

	// �n�ʃA�j���[�V�����Ǘ�
	if( g3Dmapper->granime ){
		FIELD_GRANM_Main( g3Dmapper->granime );
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���f�B�X�v���C
 */
//------------------------------------------------------------------
void	FLDMAPPER_Draw( const FLDMAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera )
{
	int i;
	VecFx32 org_pos,draw_pos;
	
	GF_ASSERT( g3Dmapper );
	
	GFL_G3D_MAP_StartDraw();

	for( i=0; i<g3Dmapper->blockNum; i++ ){
		GFL_G3D_MAP_GetTrans( g3Dmapper->blockWk[i].g3Dmap, &org_pos );
		draw_pos.x = org_pos.x + g3Dmapper->globalDrawOffset.x;
		draw_pos.y = org_pos.y + g3Dmapper->globalDrawOffset.y;
		draw_pos.z = org_pos.z + g3Dmapper->globalDrawOffset.z;
		GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[i].g3Dmap, &draw_pos );
		GFL_G3D_MAP_Draw( g3Dmapper->blockWk[i].g3Dmap, g3Dcamera );
		GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[i].g3Dmap, &org_pos );
	}
	GFL_G3D_MAP_EndDraw();
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FLDMAPPER_CheckTrans( const FLDMAPPER* g3Dmapper )
{
	int i;
	for ( i=0; i<g3Dmapper->blockNum; i++ ){
		GFL_G3D_MAP_LOAD_STATUS *ldst;
		GFL_G3D_MAP_GetLoadStatusPointer( g3Dmapper->blockWk[i].g3Dmap, &ldst );
		if (ldst->seq != GFL_G3D_MAP_LOAD_IDLING) {
			return FALSE;
		}
	}
	return TRUE;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�}�b�v�f�[�^�o�^
 */
//------------------------------------------------------------------
void FLDMAPPER_ResistData( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData )
{
	GF_ASSERT( g3Dmapper );

	g3Dmapper->g3DmapFileType = resistData->g3DmapFileType;
	g3Dmapper->blockWidth = resistData->blockWidth;
	g3Dmapper->blockHeight = resistData->blockHeight;
	g3Dmapper->arcID = resistData->arcID;
	g3Dmapper->sizex = resistData->sizex;
	g3Dmapper->sizez = resistData->sizez;
	g3Dmapper->totalSize = resistData->totalSize;
	g3Dmapper->blocks = resistData->blocks;
	g3Dmapper->mode = resistData->mode;
  g3Dmapper->blockXNum  = resistData->blockXNum;
  g3Dmapper->blockZNum  = resistData->blockZNum;
  g3Dmapper->blockNum   = resistData->blockXNum * resistData->blockZNum;

  {//mode�ُ�̑Ώ� 
    switch( g3Dmapper->mode ){
    case FLDMAPPER_MODE_SCROLL_NONE:
      if( g3Dmapper->totalSize > g3Dmapper->blockNum ){
        OS_Printf("mapper mode set Error\n");
        g3Dmapper->mode = FLDMAPPER_MODE_SCROLL_XZ;
      }
      break;
    case FLDMAPPER_MODE_SCROLL_Y:
      if( g3Dmapper->sizex * g3Dmapper->sizez > g3Dmapper->blockNum/2 ){
        OS_Printf("mapper mode set Error\n");
        g3Dmapper->mode = FLDMAPPER_MODE_SCROLL_XZ;
      }
      break;
    }
  }

  g3Dmapper->gtexType = resistData->gtexType;
	//�O���[�o���e�N�X�`���쐬
	CreateGlobalTexture( g3Dmapper, resistData );
	//�O���[�o���I�u�W�F�N�g�쐬
	//CreateGlobalObject( g3Dmapper, resistData );
  

	// �n�ʃA�j���[�V�����쐬
  g3Dmapper->granime = createGroundAnime(g3Dmapper->blockNum, g3Dmapper->globalTexture, &resistData->ground_anime, g3Dmapper->heapID );
	
	{
		int i;
		GFL_G3D_MAP_SETUP setup;

		if (g3Dmapper->globalTexture != NULL) {
			setup.mapDataHeapSize = MAPMDL_SIZE + MAPTEX_SIZE + MAPATTR_SIZE;
			setup.texVramSize = 0;
			setup.mapFileFunc = mapFileFuncTbl;
			setup.externalWork = NULL;
		} else {
			setup.mapDataHeapSize = MAPMDL_SIZE + MAPTEX_SIZE + MAPATTR_SIZE;
			setup.texVramSize = MAPTEX_SIZE;
			setup.mapFileFunc = mapFileFuncTbl;
			setup.externalWork = NULL;
		}


		//�u���b�N����n���h���쐬
    GF_ASSERT( resistData->blockXNum > 0 );
    GF_ASSERT( resistData->blockZNum > 0 );
    g3Dmapper->blockWk = GFL_HEAP_AllocClearMemory( g3Dmapper->heapID, sizeof(BLOCK_WORK) * g3Dmapper->blockNum );
    g3Dmapper->blockNew = GFL_HEAP_AllocClearMemory( g3Dmapper->heapID, sizeof(BLOCK_NEWREQ) * g3Dmapper->blockNum );
		for( i=0; i<g3Dmapper->blockNum; i++ ){
      GFL_G3D_MAP * g3dmap;

      BLOCKINFO_init(&g3Dmapper->blockWk[i].blockInfo);

			// �g�����[�N�̏�����
			FLD_G3D_MAP_ExWork_Init( &g3Dmapper->blockWk[i].g3DmapExWork, g3Dmapper, i );
			setup.externalWork = &g3Dmapper->blockWk[i].g3DmapExWork;

			// �u���b�N��񐶐�
      g3dmap = GFL_G3D_MAP_Create( &setup, g3Dmapper->heapID );
			g3Dmapper->blockWk[i].g3Dmap = g3dmap;

      //�V�A�[�J�C�u�h�c��o�^
      GFL_G3D_MAP_ResistArc( g3dmap, g3Dmapper->arcID, g3Dmapper->heapID );

      //�O���[�o�����\�[�X�o�^
      if( g3Dmapper->globalTexture != NULL ){
        GFL_G3D_MAP_ResistGlobalTexResource( g3dmap, g3Dmapper->globalTexture );
      }
      GFL_G3D_MAP_ResistGlobalObjResource( g3dmap, &g3Dmapper->globalObj );

      //�t�@�C�����ʐݒ�i���j
      GFL_G3D_MAP_ResistFileType( g3dmap, resistData->g3DmapFileType );
    }

	}

	//�O���[�o���I�u�W�F�N�g�쐬
	CreateGlobalObject( g3Dmapper, resistData );

}

void FLDMAPPER_ReleaseData( FLDMAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );

  if( g3Dmapper->blockWk ){

    //�}�b�v�u���b�N�������
    for( i=0; i<g3Dmapper->blockNum; i++ ){
      GFL_G3D_MAP_ReleaseGlobalObjResource( g3Dmapper->blockWk[i].g3Dmap );
      GFL_G3D_MAP_ReleaseGlobalTexResource( g3Dmapper->blockWk[i].g3Dmap );
      GFL_G3D_MAP_ReleaseArc( g3Dmapper->blockWk[i].g3Dmap );

      GFL_G3D_MAP_Delete( g3Dmapper->blockWk[i].g3Dmap );

      // �g�����[�N�̔j��
      FLD_G3D_MAP_ExWork_Exit( &g3Dmapper->blockWk[i].g3DmapExWork );
    }


    GFL_HEAP_FreeMemory( g3Dmapper->blockWk );
    GFL_HEAP_FreeMemory( g3Dmapper->blockNew );
    g3Dmapper->blockNew = NULL;
    g3Dmapper->blockWk = NULL;
    g3Dmapper->blockNum = 0;
  }

	// �n�ʃA�j���[�V�����j��
	if( g3Dmapper->granime != NULL ){
		FIELD_GRANM_Delete( g3Dmapper->granime );
		g3Dmapper->granime = NULL;
	}
	
	DeleteGlobalObject( g3Dmapper );
	DeleteGlobalTexture( g3Dmapper );
}

//------------------------------------------------------------------
/**
 * @brief	�O���[�o���e�N�X�`���쐬
 */
//------------------------------------------------------------------
static void CreateGlobalTexture( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData )
{
	switch (resistData->gtexType) {
	case FLDMAPPER_TEXTYPE_USE:
		{
			const FLDMAPPER_RESIST_TEX* gtexData = &resistData->gtexData;
			g3Dmapper->globalTexture = GFL_G3D_CreateResourceArc( gtexData->arcID, gtexData->datID );
			GFL_G3D_TransVramTexture( g3Dmapper->globalTexture );
		}
		break;
	case FLDMAPPER_TEXTYPE_NONE:
		g3Dmapper->globalTexture = NULL;
	}
}

static void DeleteGlobalTexture( FLDMAPPER* g3Dmapper )
{
	if( g3Dmapper->globalTexture != NULL ){
		GFL_G3D_FreeVramTexture( g3Dmapper->globalTexture );
		GFL_G3D_DeleteResource( g3Dmapper->globalTexture );
		g3Dmapper->globalTexture = NULL;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�O���[�o���I�u�W�F�N�g�쐬
 */
//------------------------------------------------------------------
static void CreateGlobalObject( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData )
{
	switch (resistData->gobjType) {
  case FLDMAPPER_RESIST_OBJTYPE_BMODEL:
		GF_ASSERT(g3Dmapper->bmodel_man);
    CreateGlobalObj_forBModel( g3Dmapper, g3Dmapper->bmodel_man);
    break;
	case FLDMAPPER_RESIST_OBJTYPE_TBL:
		GF_ASSERT(resistData->gobjData);
		CreateGrobalObj_forTbl( g3Dmapper, (const FLDMAPPER_RESISTDATA_OBJTBL*)resistData->gobjData );
		break;
	case FLDMAPPER_RESIST_OBJTYPE_BIN:
		GF_ASSERT(resistData->gobjData);
		CreateGrobalObj_forBin( g3Dmapper, (const FLDMAPPER_RESISTDATA_OBJBIN*)resistData->gobjData );
		break;
	case FLDMAPPER_RESIST_OBJTYPE_NONE:
		/* do nothing */
		break;
	}
}

static void DeleteGlobalObject( FLDMAPPER* g3Dmapper )
{
	if( g3Dmapper->globalObj.gddobj != NULL ){
		DeleteGlobalDDobj( g3Dmapper );
		GFL_HEAP_FreeMemory( g3Dmapper->globalObj.gddobj );
		g3Dmapper->globalObj.gddobj = NULL;
	}
	if( g3Dmapper->globalObj.gddobjIDexchange != NULL ){
		GFL_HEAP_FreeMemory( g3Dmapper->globalObj.gddobjIDexchange );
		g3Dmapper->globalObj.gddobjIDexchange = NULL;
	}
	if( g3Dmapper->globalObj.gobj != NULL ){
		GFL_HEAP_FreeMemory( g3Dmapper->globalObj.gobj );
		g3Dmapper->globalObj.gobj = NULL;
	}
	if( g3Dmapper->globalObj.gobjIDexchange != NULL ){
		GFL_HEAP_FreeMemory( g3Dmapper->globalObj.gobjIDexchange );
		g3Dmapper->globalObj.gobjIDexchange = NULL;
	}

	if( g3Dmapper->globalObjRes != NULL ){
		int i;

		for( i=0; i<g3Dmapper->globalObjResCount; i++ ){
			DeleteGlobalObj( &g3Dmapper->globalObjRes[i] );
		}
		GFL_HEAP_FreeMemory( g3Dmapper->globalObjRes );
		g3Dmapper->globalObjResCount = 0;
		g3Dmapper->globalObjRes = NULL;
	}
}

//------------------------------------------------------------------
// �I�u�W�F�N�g���\�[�X���쐬
//�ʏ�MDL
static void CreateGlobalObj( GLOBALOBJ_RES* objRes, const MAKE_OBJ_PARAM* param )
{
	GFL_G3D_RND* g3Drnd;
	GFL_G3D_RES* resTex;
	GFL_G3D_ANM* anmTbl[GLOBAL_OBJ_ANMCOUNT];
	int i;

  //���f���f�[�^����
	objRes->g3DresMdl = GFL_G3D_CreateResourceArc( param->mdl.arcID, param->mdl.datID );

  //�e�N�X�`���o�^
	if( param->tex.arcID != MAKE_RES_NONPARAM ){
		objRes->g3DresTex = GFL_G3D_CreateResourceArc( param->tex.arcID, param->tex.datID );
		resTex = objRes->g3DresTex;
	} else {
		objRes->g3DresTex = NULL;
		resTex = objRes->g3DresMdl;
	}
	GFL_G3D_TransVramTexture( resTex );

  //�����_�[����
	g3Drnd = GFL_G3D_RENDER_Create( objRes->g3DresMdl, 0, resTex );

  //�A�j���I�u�W�F�N�g����
	for( i=0; i<GLOBAL_OBJ_ANMCOUNT; i++ ){
		if( param->anm[i].arcID != MAKE_RES_NONPARAM ){
			objRes->g3DresAnm[i] = GFL_G3D_CreateResourceArc
										( param->anm[i].arcID, param->anm[i].datID );
			anmTbl[i] = GFL_G3D_ANIME_Create( g3Drnd, objRes->g3DresAnm[i], 0 );  
		} else {
			anmTbl[i] = NULL;
		}
	}
  //GFL_G3D_OBJECT����
	objRes->g3Dobj = GFL_G3D_OBJECT_Create( g3Drnd, anmTbl, GLOBAL_OBJ_ANMCOUNT );
}

static void DeleteGlobalObj( GLOBALOBJ_RES* objRes )
{
	GFL_G3D_RND*	g3Drnd;
	GFL_G3D_RES*	resTex;
	GFL_G3D_ANM*	g3Danm[GLOBAL_OBJ_ANMCOUNT] = { NULL, NULL, NULL, NULL };
	u16				g3DanmCount;
	int i;

	if( objRes->g3Dobj != NULL ){
		//�e��n���h���擾
		g3DanmCount = GFL_G3D_OBJECT_GetAnimeCount( objRes->g3Dobj );
		for( i=0; i<g3DanmCount; i++ ){
			g3Danm[i] = GFL_G3D_OBJECT_GetG3Danm( objRes->g3Dobj, i ); 
		}
		g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( objRes->g3Dobj );
	
		//�e��n���h�������\�[�X�폜
		GFL_G3D_OBJECT_Delete( objRes->g3Dobj );
		objRes->g3Dobj = NULL;

		for( i=0; i<GLOBAL_OBJ_ANMCOUNT; i++ ){
			if( g3Danm[i] != NULL ){ 
				GFL_G3D_ANIME_Delete( g3Danm[i] ); 
			}
			if( objRes->g3DresAnm[i] != NULL ){
				GFL_G3D_DeleteResource( objRes->g3DresAnm[i] );
				objRes->g3DresAnm[i] = NULL;
			}
		}
		GFL_G3D_RENDER_Delete( g3Drnd );
		if( objRes->g3DresTex == NULL ){
			resTex = objRes->g3DresMdl;
		} else {
			resTex = objRes->g3DresTex;
			GFL_G3D_DeleteResource( objRes->g3DresTex );
			objRes->g3DresTex = NULL;
		}
		GFL_G3D_FreeVramTexture( resTex );

		if( objRes->g3DresMdl != NULL ){
			GFL_G3D_DeleteResource( objRes->g3DresMdl );
			objRes->g3DresMdl = NULL;
		}
	}
}

//------------------------------------------------------------------
//DirectDraw
static void CreateGlobalDDobj( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA_DDOBJ* resistData )
{
	GFL_G3D_RES*	g3DresTex;
	NNSG3dTexKey	texDataKey;
	NNSG3dPlttKey	texPlttKey;
	int i;

	GF_ASSERT( resistData->count <= GLOBAL_DDOBJ_COUNT );
	
	for( i=0; i< resistData->count; i++ ){
		g3DresTex = GFL_G3D_CreateResourceArc( resistData->arcID, resistData->data[i] );

		g3Dmapper->globalObj.gddobj[i].g3Dres = g3DresTex;
										
		if( GFL_G3D_TransVramTexture( g3DresTex ) == FALSE ){
			GF_ASSERT(0);
		}
		texDataKey = GFL_G3D_GetTextureDataVramKey( g3DresTex );
		texPlttKey = GFL_G3D_GetTexturePlttVramKey( g3DresTex );

		g3Dmapper->globalObj.gddobj[i].texDataAdrs = NNS_GfdGetTexKeyAddr( texDataKey );
		g3Dmapper->globalObj.gddobj[i].texPlttAdrs = NNS_GfdGetPlttKeyAddr( texPlttKey );
		g3Dmapper->globalObj.gddobj[i].data = &drawTreeData;
	}
	g3Dmapper->globalObj.gddobjCount = resistData->count;
}

static void DeleteGlobalDDobj( FLDMAPPER* g3Dmapper )
{
	int i;

	for( i=0; i<g3Dmapper->globalObj.gddobjCount; i++ ){
		if( g3Dmapper->globalObj.gddobj[i].g3Dres != NULL ){
			GFL_G3D_FreeVramTexture( g3Dmapper->globalObj.gddobj[i].g3Dres );
			GFL_G3D_DeleteResource( g3Dmapper->globalObj.gddobj[i].g3Dres );
			g3Dmapper->globalObj.gddobj[i].g3Dres = NULL;
		}
	}
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�}�b�v�ʒu�Z�b�g
 */
//------------------------------------------------------------------
void FLDMAPPER_SetPos( FLDMAPPER* g3Dmapper, const VecFx32* pos )
{
	GF_ASSERT( g3Dmapper );

	VEC_Set( &g3Dmapper->posCont, pos->x, pos->y, pos->z );
}

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�X�V�u���b�N�擾
 */
//------------------------------------------------------------------
static void GetMapperBlockIdxAll( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u32		idx, county, countxz;
	fx32	blockWidth, blockHeight;
  fx32  blockHalfWidth;
	int		i, j, offsx, offsz;

	countxz = g3Dmapper->sizex * g3Dmapper->sizez;
	county = g3Dmapper->totalSize / countxz;
	if( g3Dmapper->totalSize % countxz ){
		county++;
	}
	blockWidth      = g3Dmapper->blockWidth;
  blockHalfWidth  = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );
	blockHeight     = g3Dmapper->blockHeight;

	idx = 0;

	for( i=0; i<county; i++ ){
		for( j=0; j<countxz; j++ ){
			offsx = j % g3Dmapper->sizex;
			offsz = j / g3Dmapper->sizex;

			if( idx >= g3Dmapper->totalSize ){
				idx = MAPID_NULL;
			}
      BLOCKINFO_SetBlockIdx( &new[idx].newBlockInfo, idx );
      BLOCKINFO_SetBlockTrans( &new[idx].newBlockInfo, 
         FX_Mul(offsx<<FX32_SHIFT, blockWidth) + blockHalfWidth,
         FX_Mul( i<<FX32_SHIFT,blockHeight ),
         FX_Mul(offsz<<FX32_SHIFT, blockWidth) + blockHalfWidth );
			idx++;
		}
	}
}

//------------------------------------------------------------------
static void GetMapperBlockIdxXZ( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u16		sizex, sizez;
	u32		idx, idxmax;
	fx32	blockWidth, blockHalfWidth;
	int		i, j;
  int   start_x, start_z;
  int   set_x, set_z;


	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	idxmax = sizex * sizez;
	blockWidth = g3Dmapper->blockWidth;
  blockHalfWidth = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );

  // XZ�����u���b�N�J�n�ʒu�����߂�
  if( (g3Dmapper->blockXNum % 2) == 0 ){
    // �����̏ꍇ
    start_x = GetMapperBlockStartIdxEven( g3Dmapper->blockXNum, blockWidth, pos->x );
  }else{
    // ��̏ꍇ
    start_x = GetMapperBlockStartIdxUnEven( g3Dmapper->blockXNum, blockWidth, pos->x );
  }
  if( (g3Dmapper->blockXNum % 2) == 0 ){
    // �����̏ꍇ
    start_z = GetMapperBlockStartIdxEven( g3Dmapper->blockZNum, blockWidth, pos->z );
  }else{
    // ��̏ꍇ
    start_z = GetMapperBlockStartIdxUnEven( g3Dmapper->blockZNum, blockWidth, pos->z );
  }

  // �ǂݍ��݃��N�G�X�g����ݒ肷��
	for( i=0; i<g3Dmapper->blockZNum; i++ )
  {
    // �ݒ肷��u���b�N��Z�l�����߂� 
    // �u���b�N�f�[�^�͈͓̔����`�F�b�N����
    set_z = start_z + i;
    if( (set_z >= 0) && (set_z < sizez) )
    {
  	  for( j=0; j<g3Dmapper->blockXNum; j++ )
      {
        // �ݒ肷��u���b�N��X�l�����߂� 
        // �u���b�N�f�[�^�͈͓̔����`�F�b�N����
        set_x = start_x + j;
        if( (set_x >= 0) && (set_x < sizex) )
        {
          idx = (i*g3Dmapper->blockXNum) + j;
          BLOCKINFO_SetBlockIdx( &new[idx].newBlockInfo, (set_z * sizex) + set_x );
          BLOCKINFO_SetBlockTrans( &new[idx].newBlockInfo, 
             FX_Mul(set_x<<FX32_SHIFT, blockWidth) + blockHalfWidth,
             0,
             FX_Mul(set_z<<FX32_SHIFT, blockWidth) + blockHalfWidth );
        }
      }
    }
  }
}

//------------------------------------------------------------------
static void GetMapperBlockIdxY( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u16		sizex, sizez;
	u32		idx, blocky, countxz;
	fx32	blockWidth, blockHeight;
  fx32  blockHalfWidth;
	int		i, p, offsx, offsy, offsz;

	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	countxz = sizex * sizez;
	blockWidth      = g3Dmapper->blockWidth;
  blockHalfWidth  = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );
	blockHeight     = g3Dmapper->blockHeight;

	blocky = FX_Whole( FX_Div( pos->y, blockHeight ) );
	if( pos->y - ( blocky * blockHeight ) > blockHeight/2 ){
		offsy = 1;
	} else {
		offsy = -1;
	}
	p = 0;

	for( i=0; i<countxz; i++ ){
		offsx = i % g3Dmapper->sizex;
		offsz = i / g3Dmapper->sizex;

		idx = blocky * countxz + offsz * sizex + offsx;

		if( idx >= g3Dmapper->totalSize ){
			idx = MAPID_NULL;
		}
    BLOCKINFO_SetBlockIdx( &new[p].newBlockInfo, idx );
    BLOCKINFO_SetBlockTrans( &new[p].newBlockInfo, 
       FX_Mul( offsx<<FX32_SHIFT, blockWidth ) + blockHalfWidth,
       FX_Mul( blocky<<FX32_SHIFT, blockHeight ),
       FX_Mul( offsz<<FX32_SHIFT, blockWidth) + blockHalfWidth );
		p++;
	}
	for( i=0; i<countxz; i++ ){
		offsx = i % g3Dmapper->sizex;
		offsz = i / g3Dmapper->sizex;

		idx = (blocky + offsy) * countxz + offsz * sizex + offsx;

		if( idx >= g3Dmapper->totalSize ){
			idx = MAPID_NULL;
		}
    BLOCKINFO_SetBlockIdx( &new[p].newBlockInfo, idx );
    BLOCKINFO_SetBlockTrans( &new[p].newBlockInfo, 
       FX_Mul( offsx<<FX32_SHIFT, blockWidth ) + blockHalfWidth,
       FX_Mul( (blocky + offsy)<<FX32_SHIFT, blockHeight ),
       FX_Mul( offsz<<FX32_SHIFT, blockWidth ) + blockHalfWidth );
		p++;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�u���b�N�X�V�`�F�b�N
 */
//------------------------------------------------------------------
static BOOL	ReloadMapperBlock( FLDMAPPER* g3Dmapper, BLOCK_NEWREQ* new )
{
	BOOL addFlag, delFlag, delProcFlag, addProcFlag, reloadFlag;
	int i, j, c;
  u32 now_blockIdx, new_blockIdx;

	reloadFlag = FALSE;

	//�X�V�u���b�N�`�F�b�N
	delProcFlag = FALSE;
	for( i=0; i<g3Dmapper->blockNum; i++ ){
		if( BLOCKINFO_IsInBlockData( &g3Dmapper->blockWk[i].blockInfo)  ){
			delFlag = FALSE;
      now_blockIdx = BLOCKINFO_GetBlockIdx( &g3Dmapper->blockWk[i].blockInfo );
			for( j=0; j<g3Dmapper->blockNum; j++ ){
        new_blockIdx = BLOCKINFO_GetBlockIdx( &new[j].newBlockInfo );
				if(( now_blockIdx == new_blockIdx )&&(delFlag == FALSE )){
          
          BLOCKINFO_init( &new[j].newBlockInfo );
					delFlag = TRUE;
				}
			}
			if( delFlag == FALSE ){
        BLOCKINFO_init( &g3Dmapper->blockWk[i].blockInfo );
				GFL_G3D_MAP_SetDrawSw( g3Dmapper->blockWk[i].g3Dmap, FALSE );

				// �g�����[�N�̏����N���A
				FLD_G3D_MAP_ExWork_ClearBlockData( &g3Dmapper->blockWk[i].g3DmapExWork );
				delProcFlag = TRUE;
			}
		}
	}
	//�X�V
	addProcFlag = FALSE;
	c = 0;
	for( i=0; i<g3Dmapper->blockNum; i++ ){
      
		if( BLOCKINFO_IsInBlockData( &new[i].newBlockInfo) ){
			addFlag = FALSE;
			for( j=0; j<g3Dmapper->blockNum; j++ ){

				if(( BLOCKINFO_IsInBlockData( &g3Dmapper->blockWk[j].blockInfo ) == FALSE )&&(addFlag == FALSE )){
          u32 blockIdx = BLOCKINFO_GetBlockIdx( &new[i].newBlockInfo );
					u32 mapdatID = g3Dmapper->blocks[blockIdx].datID;
          VecFx32 trans;

          BLOCKINFO_GetBlockTrans( &new[i].newBlockInfo, &trans );

					if( mapdatID != FLDMAPPER_MAPDATA_NULL ){

						GFL_G3D_MAP_SetLoadReq( g3Dmapper->blockWk[j].g3Dmap, mapdatID );
						GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[j].g3Dmap, &trans );
						GFL_G3D_MAP_SetDrawSw( g3Dmapper->blockWk[j].g3Dmap, TRUE );
					}
					g3Dmapper->blockWk[j].blockInfo = new[i].newBlockInfo;
					addFlag = TRUE;
					addProcFlag = TRUE;
				}
			}
			if( addFlag == FALSE ){
				GF_ASSERT(0);
			}
		}
	}
	if( addProcFlag == TRUE ){
		reloadFlag = TRUE;
	}
	return reloadFlag;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N�������̂Ƃ��́A�u���b�N�J�n�ʒu�����߂�
 *
 *	@param	blockNum    �u���b�N��
 *	@param	width       �P�u���b�N��
 *	@param	pos         �����̈ʒu
 *
 *	@return �u���b�N�J�n�ʒu  �i���̒l�̏ꍇ������j
 */
//-----------------------------------------------------------------------------
static s32 GetMapperBlockStartIdxEven( u32 blockNum, fx32 width, fx32 pos )
{
  fx32 offs;
  fx32 block;
  fx32 width_half;
  s32 block_start;

  block_start = -((blockNum/2)-1); // �u���b�N�̒[�̈ʒu�����߂�A���炩���ߎ������܂߂邽�߂�-1���Ă���

  block = pos / width;
  offs  = pos % width;
  width_half = FX_Div( width, 2*FX32_ONE );

  if( offs < width_half ){
    block_start += block-1;
  }else{
    block_start += block;
  }

  return block_start;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N����̂Ƃ��́A�u���b�N�J�n�ʒu�����߂�
 *
 *	@param	blockNum    �u���b�N��
 *	@param	width       �P�u���b�N��
 *	@param	pos         �����̈ʒu
 *
 *	@return �u���b�N�J�n�ʒu  �i���̒l�̏ꍇ������j
 */
//-----------------------------------------------------------------------------
static s32 GetMapperBlockStartIdxUnEven( u32 blockNum, fx32 width, fx32 pos )
{
  fx32 block;
  s32 block_start;

  block_start = -(blockNum/2);  // �u���b�N�̒[�̈ʒu�����߂�B
                                // ��́A�Q�Ŋ��邱�ƂŁA����Ɏ����̕�������

  block = pos / width;
  block_start += block;

  return block_start;
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
//------------------------------------------------------------------
/**
 * @brief	���[�N������
 */
//------------------------------------------------------------------
void FLDMAPPER_GRIDINFODATA_Init( FLDMAPPER_GRIDINFODATA* gridInfoData )
{
	VEC_Fx16Set( &gridInfoData->vecN, 0, 0, 0 );
	gridInfoData->attr = 0;
	gridInfoData->height = 0;
}

void FLDMAPPER_GRIDINFO_Init( FLDMAPPER_GRIDINFO* gridInfo )
{
	int i;

	for( i=0; i<FLDMAPPER_GRIDINFO_MAX; i++ ){
		FLDMAPPER_GRIDINFODATA_Init( &gridInfo->gridData[i] );
	}
	gridInfo->count = 0;
}

//------------------------------------------------------------------
/**
 * @brief	�A�g���r���[�g���擾
 */
//------------------------------------------------------------------
BOOL FLDMAPPER_GetGridInfo
	( const FLDMAPPER* g3Dmapper, const VecFx32* pos, FLDMAPPER_GRIDINFO* gridInfo )
{
	GFL_G3D_MAP_ATTRINFO attrInfo;
	VecFx32 trans;
	int		i, p;

	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		OS_Printf("�f�[�^���ǂݍ��܂�Ă��Ȃ�\n");
		return FALSE;
	}

	FLDMAPPER_GRIDINFO_Init( gridInfo );
	
	p = 0;

	for( i=0; i<g3Dmapper->blockNum; i++ ){
		if( GFL_G3D_MAP_GetDrawSw( g3Dmapper->blockWk[i].g3Dmap ) == TRUE ){
			fx32 min_x, min_z, max_x, max_z;

			GFL_G3D_MAP_GetTrans( g3Dmapper->blockWk[i].g3Dmap, &trans );

			min_x = trans.x - g3Dmapper->blockWidth/2;
			min_z = trans.z - g3Dmapper->blockWidth/2;
			max_x = trans.x + g3Dmapper->blockWidth/2;
			max_z = trans.z + g3Dmapper->blockWidth/2;

			//�u���b�N�͈͓��`�F�b�N�i�}�b�v�u���b�N�̂w�y���ʏ�n�_�j
			if(	(pos->x >= min_x)&&(pos->x < max_x)&&(pos->z >= min_z)&&(pos->z < max_z) ){

				GFL_G3D_MAP_GetAttr( &attrInfo, g3Dmapper->blockWk[i].g3Dmap, pos, g3Dmapper->blockWidth );
				if( attrInfo.mapAttrCount ){
					int j;

					if( (p + attrInfo.mapAttrCount) >= FLDMAPPER_GRIDINFO_MAX ){
						GF_ASSERT("height count over\n");
					}
					for( j=0; j<attrInfo.mapAttrCount; j++ ){
						gridInfo->gridData[p+j].vecN = attrInfo.mapAttr[j].vecN;
						gridInfo->gridData[p+j].attr = attrInfo.mapAttr[j].attr;
						gridInfo->gridData[p+j].height = attrInfo.mapAttr[j].height;
					}
					p += attrInfo.mapAttrCount;
				}
			}
		}
	}
	gridInfo->count = p;
	if( gridInfo->count ){
		return TRUE;
	}
//	OS_Printf("�f�[�^�����݂��Ă��Ȃ�\n");
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�͈͊O�`�F�b�N
 */
//------------------------------------------------------------------
BOOL FLDMAPPER_CheckOutRange( const FLDMAPPER* g3Dmapper, const VecFx32* pos )
{
	fx32 widthx, widthz;

	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		return TRUE;
	}

	widthx = g3Dmapper->sizex * g3Dmapper->blockWidth;
	widthz = g3Dmapper->sizez * g3Dmapper->blockWidth;

	if( ( pos->x >= 0 )&&( pos->x < widthx )&&( pos->z >= 0 )&&( pos->z < widthz ) ){
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�T�C�Y�擾
 */
//------------------------------------------------------------------
void FLDMAPPER_GetSize( const FLDMAPPER* g3Dmapper, fx32* x, fx32* z )
{
	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		*x = 0;
		*z = 0;
		return;
	}
	*x = g3Dmapper->sizex * g3Dmapper->blockWidth;
	*z = g3Dmapper->sizez * g3Dmapper->blockWidth;
	return;
}

#if 0
//--------------------------------------------------------------
/**
 * �}�b�v�f�[�^�t�@�C���^�C�v���擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
FLDMAPPER_FILETYPE FLDMAPPER_GetFileType( const FLDMAPPER *g3Dmapper )
{
	return( g3Dmapper->g3DmapFileType );
}
#endif

//--------------------------------------------------------------
//	�`��I�t�Z�b�g
//--------------------------------------------------------------
void FLDMAPPER_SetDrawOffset( FLDMAPPER *g3Dmapper, const VecFx32 *offs )
{
	g3Dmapper->globalDrawOffset = *offs;
}

void FLDMAPPER_GetDrawOffset( const FLDMAPPER *g3Dmapper, VecFx32 *offs )
{
	*offs = g3Dmapper->globalDrawOffset;
}


//--------------------------------------------------------------
/**
 * @brief �z�u���f���}�l�W���[���擾����
 */
//--------------------------------------------------------------
FIELD_BMODEL_MAN * FLDMAPPER_GetBuildModelManager( FLDMAPPER* g3Dmapper)
{ 
  return g3Dmapper->bmodel_man;
}

//============================================================================================
/**
 *
 *
 *
 * @brief	�W�I���g���������֐�
 *
 *
 *
 */
//============================================================================================
static void	_drawTree0( u32 texDataAdrs, u32 texPlttAdrs, VecFx16* vecView, BOOL lodSt )
{
	GXTexFmt	texFmt = GX_TEXFMT_PLTT16;
	GXTexSizeS	s;
	GXTexSizeT	t;
	fx32		smax, tmax;

	if( lodSt == TRUE ){
		s = GX_TEXSIZE_S64;
		t = GX_TEXSIZE_T64;
		smax = 64 * FX32_ONE;
		tmax = 64 * FX32_ONE;

		G3_TexImageParam(	texFmt, GX_TEXGEN_TEXCOORD, s, t, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
						GX_TEXPLTTCOLOR0_TRNS, texDataAdrs + 0x200 );
		G3_TexPlttBase( texPlttAdrs + 0x20, texFmt );

		G3_Begin( GX_BEGIN_QUADS );

		G3_Normal( 0.000 * FX16_ONE, 1.000 * FX16_ONE, 0.000 * FX16_ONE );
		G3_Color( GX_RGB( 31, 31, 31 ) );
		G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
		G3_Vtx( -1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, -1.500 * FX16_ONE );	//vtx0
		G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
		G3_Vtx( -1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, 1.500 * FX16_ONE );	//vtx2
		G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
		G3_Vtx( 1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, 1.500 * FX16_ONE );	//vtx3
		G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
		G3_Vtx( 1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, -1.500 * FX16_ONE );	//vtx1

		G3_Normal( 0.000 * FX16_ONE, 0.819 * FX16_ONE, 0.574 * FX16_ONE );
		G3_Color( GX_RGB( 31, 31, 31 ) );
		G3_TexCoord( FX_Mul(0.344 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
		G3_Vtx( -2.000 * FX16_ONE, 2.675 * FX16_ONE, -1.793 * FX16_ONE );	//vtx0
		G3_TexCoord( FX_Mul(0.344 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
		G3_Vtx( -2.000 * FX16_ONE, 0.725 * FX16_ONE, 0.993 * FX16_ONE );	//vtx2
		G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
		G3_Vtx( 2.000 * FX16_ONE, 0.725 * FX16_ONE, 0.993 * FX16_ONE );		//vtx3
		G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
		G3_Vtx( 2.000 * FX16_ONE, 2.675 * FX16_ONE, -1.793 * FX16_ONE );	//vtx1

		G3_Normal( 0.000 * FX16_ONE, 0.819 * FX16_ONE, 0.574 * FX16_ONE );
		G3_Color( GX_RGB( 31, 31, 31 ) );
		G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
		G3_Vtx( -1.650 * FX16_ONE, 3.560 * FX16_ONE, -1.729 * FX16_ONE );	//vtx0
		G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(0.469 * FX32_ONE,tmax) );
		G3_Vtx( -1.650 * FX16_ONE, 1.840 * FX16_ONE, 0.729 * FX16_ONE );	//vtx2
		G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.469 * FX32_ONE,tmax) );
		G3_Vtx( 1.650 * FX16_ONE, 1.840 * FX16_ONE, 0.729 * FX16_ONE );		//vtx3
		G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
		G3_Vtx( 1.650 * FX16_ONE, 3.560 * FX16_ONE, -1.729 * FX16_ONE );	//vtx1

		G3_Normal( 0.000 * FX16_ONE, 0.819 * FX16_ONE, 0.574 * FX16_ONE );
		G3_Color( GX_RGB( 31, 31, 31 ) );
		G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(0.656 * FX32_ONE,tmax) );
		G3_Vtx( -1.100 * FX16_ONE, 4.231 * FX16_ONE, -1.601 * FX16_ONE );	//vtx0
		G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
		G3_Vtx( -1.100 * FX16_ONE, 2.969 * FX16_ONE, 0.201 * FX16_ONE );	//vtx2
		G3_TexCoord( FX_Mul(0.344 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
		G3_Vtx( 1.100 * FX16_ONE, 2.969 * FX16_ONE, 0.201 * FX16_ONE );		//vtx3
		G3_TexCoord( FX_Mul(0.344 * FX32_ONE,smax), FX_Mul(0.656 * FX32_ONE,tmax) );
		G3_Vtx( 1.100 * FX16_ONE, 4.231 * FX16_ONE, -1.601 * FX16_ONE );	//vtx1

		G3_End();
	} else {
		s = GX_TEXSIZE_S32;
		t = GX_TEXSIZE_T32;
		smax = 32 * FX32_ONE;
		tmax = 32 * FX32_ONE;

		G3_TexImageParam(	texFmt, GX_TEXGEN_TEXCOORD, s, t, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
						GX_TEXPLTTCOLOR0_TRNS, texDataAdrs + 0x000 );
		G3_TexPlttBase( texPlttAdrs + 0x00, texFmt );

		G3_Begin( GX_BEGIN_QUADS );

		G3_Normal( 0.000 * FX16_ONE, 1.000 * FX16_ONE, 0.000 * FX16_ONE );
		G3_Color( GX_RGB( 31, 31, 31 ) );
		G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
		G3_Vtx( -2.000 * FX16_ONE, 4.231 * FX16_ONE, 0 * FX16_ONE );	//vtx0
		G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
		G3_Vtx( -2.000 * FX16_ONE, 0.000 * FX16_ONE, 0 * FX16_ONE );	//vtx2
		G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
		G3_Vtx( 2.000 * FX16_ONE, 0.000 * FX16_ONE, 0 * FX16_ONE );	//vtx3
		G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
		G3_Vtx( 2.000 * FX16_ONE, 4.231 * FX16_ONE, 0 * FX16_ONE );	//vtx1

		G3_End();
	}
}

static const GFL_G3D_MAP_DDOBJ_DATA drawTreeData = {
	GFL_G3D_MAP_LIGHT_NONE,
	GX_RGB(31,31,31), GX_RGB(16,16,16), GX_RGB(16,16,16), GX_RGB(0,0,0),
	63, 31, GFL_G3D_MAP_DRAW_YBILLBOARD, 8,
	_drawTree0,
};


//============================================================================================
/**
 *
 *
 *
 * @brief	�R�c�O���[�o���I�u�W�F�N�g�ǂݍ���
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void MAKE_OBJ_PARAM_init(MAKE_OBJ_PARAM * objParam)
{ 
  int i;
  objParam->mdl.arcID = MAKE_RES_NONPARAM;
  objParam->mdl.datID = MAKE_RES_NONPARAM;
  objParam->mdl.inDatNum = 0;

  objParam->tex.arcID = MAKE_RES_NONPARAM;
  objParam->tex.datID = MAKE_RES_NONPARAM;
  objParam->tex.inDatNum = 0;

  for( i=0; i<GLOBAL_OBJ_ANMCOUNT; i++ ){
    objParam->anm[i].arcID = MAKE_RES_NONPARAM;
    objParam->anm[i].datID = MAKE_RES_NONPARAM;
    objParam->anm[i].inDatNum = 0;
  }

}
//------------------------------------------------------------------
//�z�u���f���}�l�W���[����̓��e�Ő���
//------------------------------------------------------------------
static void CreateGlobalObj_forBModel(FLDMAPPER * g3Dmapper, FIELD_BMODEL_MAN * bmodel_man)
{ 
	int i;
  const FLDMAPPER_RESISTDATA_OBJTBL * gobjTbl;
  gobjTbl = FIELD_BMODEL_MAN_GetOBJTBL(bmodel_man);

	if( gobjTbl->objCount ){
		MAKE_OBJ_PARAM objParam;
    MAKE_OBJ_PARAM_init(&objParam);

	  g3Dmapper->globalObjResCount = gobjTbl->objCount;
		g3Dmapper->globalObjRes = GFL_HEAP_AllocClearMemory
						( g3Dmapper->heapID, sizeof(GLOBALOBJ_RES) * gobjTbl->objCount );

	  g3Dmapper->globalObj.gobjCount = gobjTbl->objCount;
		g3Dmapper->globalObj.gobj = GFL_HEAP_AllocClearMemory
						( g3Dmapper->heapID, sizeof(GFL_G3D_MAP_OBJ) * gobjTbl->objCount );

		for( i=0 ; i<gobjTbl->objCount; i++ ){
			objParam.mdl.arcID = gobjTbl->objArcID;
			objParam.mdl.datID = gobjTbl->objData[i].highQ_ID;
			objParam.mdl.inDatNum = 0;
			CreateGlobalObj( &g3Dmapper->globalObjRes[i], &objParam );
			g3Dmapper->globalObj.gobj[i].g3DobjHQ = g3Dmapper->globalObjRes[i].g3Dobj;
			g3Dmapper->globalObj.gobj[i].g3DobjLQ = NULL;
		}
	}
}

//------------------------------------------------------------------
//�e�[�u���f�[�^���쐬
//------------------------------------------------------------------
static void CreateGrobalObj_forTbl( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA_OBJTBL* gobjTbl )
{
	int i, resCount = 0;

	if( gobjTbl->objCount ){
		MAKE_OBJ_PARAM objParam;
    MAKE_OBJ_PARAM_init(&objParam);
		g3Dmapper->globalObjRes = GFL_HEAP_AllocClearMemory//HQ,LQ �̂Q���m��
						( g3Dmapper->heapID, sizeof(GLOBALOBJ_RES) * (gobjTbl->objCount*2) );
		g3Dmapper->globalObj.gobj = GFL_HEAP_AllocClearMemory
						( g3Dmapper->heapID, sizeof(GFL_G3D_MAP_OBJ) * gobjTbl->objCount );

		for( i=0, resCount=0; i<gobjTbl->objCount; i++ ){
			objParam.mdl.arcID = gobjTbl->objArcID;
			objParam.mdl.inDatNum = 0;

			objParam.mdl.datID = gobjTbl->objData[i].highQ_ID;
			CreateGlobalObj( &g3Dmapper->globalObjRes[resCount], &objParam );
			g3Dmapper->globalObj.gobj[i].g3DobjHQ = g3Dmapper->globalObjRes[resCount].g3Dobj;
			resCount++;

			if( gobjTbl->objData[i].lowQ_ID != NON_LOWQ ){
				objParam.mdl.datID = gobjTbl->objData[i].lowQ_ID;
				CreateGlobalObj( &g3Dmapper->globalObjRes[resCount], &objParam );
				g3Dmapper->globalObj.gobj[i].g3DobjLQ = g3Dmapper->globalObjRes[resCount].g3Dobj;
				resCount++;
			} else {
				g3Dmapper->globalObj.gobj[i].g3DobjLQ = NULL;
			}
		}
		g3Dmapper->globalObj.gobjCount = gobjTbl->objCount;
	}
	g3Dmapper->globalObjResCount = resCount;

	if( gobjTbl->ddobjCount ){
	  FLDMAPPER_RESISTDATA_DDOBJ resistDDobj;

		g3Dmapper->globalObj.gddobj = GFL_HEAP_AllocClearMemory
					( g3Dmapper->heapID, sizeof(GFL_G3D_MAP_DDOBJ) * gobjTbl->ddobjCount );
		resistDDobj.arcID = gobjTbl->ddobjArcID;
		resistDDobj.data = gobjTbl->ddobjData;
		resistDDobj.count = gobjTbl->ddobjCount;

		CreateGlobalDDobj( g3Dmapper, &resistDDobj );
	}
}

//------------------------------------------------------------------
//�o�C�i���f�[�^���쐬(�|�P�����f�r����)
//------------------------------------------------------------------
typedef struct {
	u16			count;
	u16			data;
}GOBJ_BINDATA;

typedef struct {
	u8 Flg;			//�A�j�����邩�ǂ���
	u8 Type;		//�A�j���^�C�v
	u8 Suicide;		//���E�t���O
	u8 RepeatEntry;	//�d���o�^�t���O
	u8 Door;
	u8 Dummy;
	u8 AnmNum;
	u8 SetNum;
	int Code[4];	//�A�j���R�[�h
}GOBJ_ANMTBL_HEADER;

static void CreateGrobalObj_forBin( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA_OBJBIN* gobjBin )
{
	GOBJ_BINDATA*					gobjListHeader;
	u16*							gobjList;

	gobjListHeader = GFL_ARC_LoadDataAlloc( gobjBin->areaObjArcID, 
											gobjBin->areaObjDatID,
											GetHeapLowID(g3Dmapper->heapID) );
	gobjList = (u16*)&gobjListHeader->data;

	g3Dmapper->globalObjResCount = gobjListHeader->count;

	if( gobjListHeader->count ){
		GOBJ_ANMTBL_HEADER	gobjAnmListHeader;
		MAKE_OBJ_PARAM objParam;
		int i, j;

		g3Dmapper->globalObjRes = GFL_HEAP_AllocClearMemory
				( g3Dmapper->heapID, sizeof(GLOBALOBJ_RES) * gobjListHeader->count );
		g3Dmapper->globalObj.gobj = GFL_HEAP_AllocClearMemory
				( g3Dmapper->heapID, sizeof(GFL_G3D_MAP_OBJ) * gobjListHeader->count );
		g3Dmapper->globalObj.gobjIDexchange = GFL_HEAP_AllocClearMemory
				( g3Dmapper->heapID, sizeof(u16) * gobjListHeader->count );

		//�I�u�W�F�N�g�����_�[�쐬
		for( i=0; i<gobjListHeader->count; i++ ){
			GFL_ARC_LoadDataOfs( &gobjAnmListHeader, gobjBin->areaObjAnmTblArcID, 
									gobjList[i], 0, sizeof(GOBJ_ANMTBL_HEADER) );

			objParam.mdl.arcID = gobjBin->objArcID;
			objParam.mdl.datID = gobjList[i];
			objParam.mdl.inDatNum = 0;

			objParam.tex.arcID = MAKE_RES_NONPARAM;
			objParam.tex.datID = MAKE_RES_NONPARAM;
			objParam.tex.inDatNum = 0;

			for( j=0; j<GLOBAL_OBJ_ANMCOUNT; j++ ){
				if( j<gobjAnmListHeader.SetNum ){
					objParam.anm[j].arcID = gobjBin->objanmArcID;
					objParam.anm[j].datID = gobjAnmListHeader.Code[j];
					objParam.anm[j].inDatNum = gobjAnmListHeader.AnmNum;
				} else {
					objParam.anm[j].arcID = MAKE_RES_NONPARAM;
					objParam.anm[j].datID = MAKE_RES_NONPARAM;
					objParam.anm[j].inDatNum = 0;
				}
			}

			(g3Dmapper->globalObj.gobjIDexchange)[i] = gobjList[i];	//�z�u�h�c�ϊ��p

			CreateGlobalObj( &g3Dmapper->globalObjRes[i], &objParam );
			g3Dmapper->globalObj.gobj[i].g3DobjHQ = g3Dmapper->globalObjRes[i].g3Dobj;
			g3Dmapper->globalObj.gobj[i].g3DobjLQ = NULL;

			for( j=0; j<gobjAnmListHeader.SetNum; j++ ){
				GFL_G3D_OBJECT_EnableAnime( g3Dmapper->globalObjRes[i].g3Dobj, j ); 
				GFL_G3D_OBJECT_ResetAnimeFrame( g3Dmapper->globalObjRes[i].g3Dobj, j ); 
			}
		}
		g3Dmapper->globalObj.gobjCount = gobjListHeader->count;
	}
	GFL_HEAP_FreeMemory( gobjListHeader );
}

//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static FIELD_GRANM * createGroundAnime( u32 blockNum, GFL_G3D_RES* globalTexture,
    const FLDMAPPER_RESIST_GROUND_ANIME * resistGroundAnimeData, u32 heapID )
{ 
  static FIELD_GRANM_SETUP granm_setup = {
    FALSE, FALSE,
    0,
    ARCID_AREA_ITA, 0,
    ARCID_AREA_ITP, 0, 
    ARCID_AREA_ITP_TEX, 0, 
  };

  //  �u���b�N��
  granm_setup.block_num = blockNum;

  // ITA�A�j���[�V�����̐ݒ�
  if( resistGroundAnimeData->ita_datID == FLDMAPPER_MAPDATA_NULL ){
    granm_setup.ita_use			= FALSE;
  }else{
    granm_setup.ita_use			= TRUE;
    granm_setup.ita_dataID	= resistGroundAnimeData->ita_datID;
  }

  // ITP�A�j���[�V�����̐ݒ�
  if( resistGroundAnimeData->itp_anm_datID == FLDMAPPER_MAPDATA_NULL ){
    granm_setup.itp_use			= FALSE;
  }else{
    granm_setup.itp_use			= TRUE;
    granm_setup.itp_anmID		= resistGroundAnimeData->itp_anm_datID;
    granm_setup.itp_texID		= resistGroundAnimeData->itp_anm_datID;
  }
	return FIELD_GRANM_Create( &granm_setup, globalTexture, heapID );
}


//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
/**
 *			GFL_G3D_MAP�g�����[�N����
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	�g�����[�N����A�n�ʃA�j���[�V�����̗L�����擾
 *
 *	@param	cp_wk			���[�N
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FLD_G3D_MAP_EXWORK_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return FLD_G3D_MAP_ExWork_IsGranm( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�g�����[�N����A�n�ʃA�j���[�V�������[�N���擾
 *
 *	@param	cp_wk		���[�N
 *	
 *	@return	�n�ʃA�j���[�V������[��
 */
//-----------------------------------------------------------------------------
FIELD_GRANM_WORK* FLD_G3D_MAP_EXWORK_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return FLD_G3D_MAP_ExWork_GetGranmWork( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�z�u���f���̐��䃏�[�N�擾
 *
 *	@param	cp_wk		���[�N
 *
 *	@return	�z�u���f���̐��䃏�[�N
 */
//-----------------------------------------------------------------------------
FIELD_BMODEL_MAN* FLD_G3D_MAP_EXWORK_GetBModelMan( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return  cp_wk->bmodel_man;
}



//----------------------------------------------------------------------------
/**
 *	@brief	GFL_G3D_MAP�̊g�����[�N������
 *
 *	@param	p_wk				���[�N
 *	@param	g3Dmapper		�}�b�p�[���[�N
 *	@param	index				�u���b�N�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void FLD_G3D_MAP_ExWork_Init( FLD_G3D_MAP_EXWORK* p_wk, FLDMAPPER* g3Dmapper, u32 index )
{
	// �n�ʃA�j��
	p_wk->p_granm_wk = FIELD_GRANM_GetWork( g3Dmapper->granime, index );
  p_wk->bmodel_man = g3Dmapper->bmodel_man;
}

//----------------------------------------------------------------------------
/**
 *	@brief	GFL_G3D_MAP�̊g�����[�N�j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void FLD_G3D_MAP_ExWork_Exit( FLD_G3D_MAP_EXWORK* p_wk )
{
	if( p_wk->p_granm_wk ){
		// �u���b�N���f���Ƃ̃����N������
		FIELD_GRANM_WORK_Release( p_wk->p_granm_wk );
		p_wk->p_granm_wk = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�u���b�N�Ɋ��蓖�ĂĂ��������N���A����
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void FLD_G3D_MAP_ExWork_ClearBlockData( FLD_G3D_MAP_EXWORK* p_wk )
{
	if( p_wk->p_granm_wk ){
		// �u���b�N���f���Ƃ̃����N������
		FIELD_GRANM_WORK_Release( p_wk->p_granm_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V���������邩�`�F�b�N
 *
 *	@param	cp_wk		���[�N
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FLD_G3D_MAP_ExWork_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	if( cp_wk->p_granm_wk ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�����̃��[�N���擾����
 *
 *	@param	cp_wk		���[�N
 *
 *	@retval	�n�ʃA�j���[�V�����̃��[�N
 *	@retval	NULL �n�ʃA�j���[�V�������Ȃ�
 */
//-----------------------------------------------------------------------------
static FIELD_GRANM_WORK* FLD_G3D_MAP_ExWork_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return cp_wk->p_granm_wk;
}

//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void BLOCKINFO_init(BLOCKINFO * info)
{ 
		info->blockIdx = MAPID_NULL;
		VEC_Set( &info->trans, 0, 0, 0 );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N�C���f�b�N�X�̐ݒ�
 *
 *	@param	info      �C���t�H���[�N
 *	@param	blockIdx  �u���b�N�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void BLOCKINFO_SetBlockIdx(BLOCKINFO * info, u32 blockIdx)
{
  info->blockIdx = blockIdx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N��񂪓����Ă��邩�`�F�b�N
 *
 *	@param	info  �C���t�H���[�N
 *
 *	@retval TRUE  �����Ă���
 *	@retval FALSE �����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BLOCKINFO_IsInBlockData(const BLOCKINFO * info)
{
  if( info->blockIdx == MAPID_NULL ){
    return FALSE;
  }
  return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  
 *
 *	@param	info
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static u32 BLOCKINFO_GetBlockIdx(const BLOCKINFO * info)
{
  return info->blockIdx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N�ʒu��ݒ�
 *  
 *	@param	info      ���[�N
 *	@param	trans     �ʒu
 */
//-----------------------------------------------------------------------------
static void BLOCKINFO_SetBlockTrans(BLOCKINFO * info, fx32 x, fx32 y, fx32 z )
{
	VEC_Set( &info->trans, x, y, z );
}
static void BLOCKINFO_SetBlockTransVec(BLOCKINFO * info, const VecFx32* trans)
{
  info->trans = *trans;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N�ʒu�̎擾
 *
 *	@param	info      ���[�N
 *	@param	trans     �ʒu�i�[��
 */
//-----------------------------------------------------------------------------
static void BLOCKINFO_GetBlockTrans(const BLOCKINFO * info, VecFx32* trans)
{
   *trans = info->trans;
}





