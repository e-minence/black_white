//============================================================================================
/**
 * @file	g3d_mapper.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "include\system\gfl_use.h"	//�����p

#include "g3d_mapper.h"

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
#define MAP_BLOCK_COUNT		(9)
#define MAPID_NULL			(0xffffffff)
#define MAPARC_NULL			(0xffffffff)
#define LOCAL_OBJID_NULL	(0xffffffff)
#define DDRAW_OBJID_NULL	(0xffffffff)

#define GROBAL_OBJ_COUNT	(64)
#define COMMON_OBJ_RESCOUNT	(64)
#define LOCAL_OBJRES_COUNT	(16)
#define LOCAL_OBJRND_COUNT	(32)
#define LOCAL_DIRECTDRAW_OBJ_COUNT	(64)

//#define MAP_BLOCK_LEN		(FX32_ONE * 256 * 2 )

typedef struct {
	u32			blockIdx;
	VecFx32		trans;
}BLOCK_IDX;

typedef struct {
	u32					id;
	VecFx32				trans;
}MAP_OBJECT_DATA;

typedef struct {
	u32			id;
	VecFx32		trans;
}DIRECT_OBJ;

typedef struct {
	NNSG3dRenderObj*	NNSrnd_H;	//�����_�[(High Q)
	GFL_G3D_RES*		g3Dres_H;	//���\�[�X(High Q)
	NNSG3dRenderObj*	NNSrnd_L;	//�����_�[(Low Q)
	GFL_G3D_RES*		g3Dres_L;	//���\�[�X(High Q)
}MAP_GROBAL_OBJECT_DATA;

typedef struct {
	u32				idx;
	VecFx32			trans;
	ARCHANDLE*		arc;

	u32				loadSeq;
	u32				loadCount;

	NNSG3dRenderObj*	NNSrnd;	//�n�`�����_�[

	BOOL			dataLoadReq;
	u32				dataID;
	GFL_G3D_RES*	g3DgroundResMdl;	//�n�`���f�����\�[�X
	u8				data[MAPDATA_SIZE];

	BOOL			texLoadReq;
	u32				texID;
	GFL_G3D_RES*	g3DgroundResTex;	//���[�J���n�`�e�N�X�`�����\�[�X
	u8				tex[MAPTEX_SIZE];

	BOOL			attrLoadReq;
	u32				attrID;
	u8				attr[MAPATTR_SIZE];

	NNSGfdTexKey	groundTexKey;
	NNSGfdPlttKey	groundPlttKey;

	MAP_OBJECT_DATA	object[LOCAL_OBJRND_COUNT];	//�z�u�I�u�W�F�N�g

	BOOL			objLoadReq;
	u32				objID;
	GFL_G3D_RES*	g3DobjectResMdl[LOCAL_OBJRES_COUNT];//���[�J���I�u�W�F�N�g���f�����\�[�X
	u8				obj[MAPOBJ_SIZE];

	NNSGfdTexKey	objectTexKey;
	NNSGfdPlttKey	objectPlttKey;

	DIRECT_OBJ		directDrawObject[LOCAL_DIRECTDRAW_OBJ_COUNT];
}MAP_BLOCK_DATA;

struct _G3D_MAPPER {
	HEAPID				heapID;
	MAP_BLOCK_DATA		mapBlock[MAP_BLOCK_COUNT];

	u32					nowBlockIdx;				
	VecFx32				posCont;
	u16					sizex;		//���u���b�N��
	u16					sizez;		//�c�u���b�N��
	u32					totalSize;	//�z��T�C�Y
	fx32				width;		//�u���b�N�P�ӂ̕�
	fx32				height;		//�u���b�N����
	G3D_MAPPER_MODE		mode;		//���샂�[�h
	u32					arcID;		//�O���t�B�b�N�A�[�J�C�u�h�c
	const G3D_MAPPER_DATA*	data;	//���}�b�v�f�[�^

	GFL_G3D_RES*		objectG3DresMdl[COMMON_OBJ_RESCOUNT];	//���ʃI�u�W�F�N�g���f�����\�[�X
	u32					objectG3DresCount;

	MAP_GROBAL_OBJECT_DATA grobalObject[GROBAL_OBJ_COUNT];	//���ʃI�u�W�F�N�g
	//-----------------
	GFL_G3D_RES*	tree_g3Dres;
	u32				tree_texDataAdrs;
	u32				tree_texPlttAdrs;
	//-----------------
};

static void	Load3Dmap( G3D_MAPPER* g3Dmapper );

static void	CreateMapGraphicResource( G3D_MAPPER* g3Dmapper );
static void	DeleteMapGraphicResource( G3D_MAPPER* g3Dmapper );
static void	InitMapGraphicResource( MAP_BLOCK_DATA*	mapBlock );

static void MakeMapRender( NNSG3dRenderObj* rndObj, GFL_G3D_RES* mdl, GFL_G3D_RES* tex );

static void GetMapperBlockIdxAll( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx );
static void GetMapperBlockIdxXZ( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx );
static void GetMapperBlockIdxY( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx );
static BOOL	ReloadMapperBlock( G3D_MAPPER* g3Dmapper, BLOCK_IDX* new );

static void	DirectDraw3Dmapper
	( G3D_MAPPER* g3Dmapper, MAP_BLOCK_DATA* mapBlock, GFL_G3D_CAMERA* g3Dcamera );

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���쐬
 */
//------------------------------------------------------------------
G3D_MAPPER*	Create3Dmapper( HEAPID heapID )
{
	G3D_MAPPER* g3Dmapper = GFL_HEAP_AllocClearMemory( heapID, sizeof(G3D_MAPPER) );
	int i;

	g3Dmapper->heapID = heapID;

	CreateMapGraphicResource( g3Dmapper );

	VEC_Set( &g3Dmapper->posCont, 0, 0, 0 );
	g3Dmapper->sizex = 0;
	g3Dmapper->sizez = 0;
	g3Dmapper->totalSize = 0;
	g3Dmapper->width = 0;
	g3Dmapper->height = 0;
	g3Dmapper->mode = G3D_MAPPER_MODE_SCROLL_XZ;
	g3Dmapper->arcID = MAPARC_NULL;
	g3Dmapper->data = NULL;
	
	g3Dmapper->objectG3DresCount = 0;
	for( i=0; i<COMMON_OBJ_RESCOUNT; i++ ){
		g3Dmapper->objectG3DresMdl[i] = NULL;
	}
	for( i=0; i<GROBAL_OBJ_COUNT; i++ ){
		g3Dmapper->grobalObject[i].NNSrnd_H = NULL;
		g3Dmapper->grobalObject[i].g3Dres_H = NULL;
		g3Dmapper->grobalObject[i].NNSrnd_L = NULL;
		g3Dmapper->grobalObject[i].g3Dres_L = NULL;
	}

	//---------------------
	{
		NNSG3dTexKey	texDataKey;
		NNSG3dPlttKey	texPlttKey;
		u32				arcID = 3;	//ARCID_SAMPLEMAP
		u32				datID = 48;	//NARC_sample_map_sample_tree_nsbtx

		GFL_G3D_RES* g3DresTex = GFL_G3D_CreateResourceArc( arcID, datID ); 

		g3Dmapper->tree_g3Dres = g3DresTex;

		if( GFL_G3D_TransVramTexture( g3DresTex ) == FALSE ){
			GF_ASSERT(0);
		}
		texDataKey = GFL_G3D_GetTextureDataVramKey( g3DresTex );
		texPlttKey = GFL_G3D_GetTexturePlttVramKey( g3DresTex );

		g3Dmapper->tree_texDataAdrs = NNS_GfdGetTexKeyAddr( texDataKey );
		g3Dmapper->tree_texPlttAdrs = NNS_GfdGetPlttKeyAddr( texPlttKey );

		//OS_Printf("tree_texDataAdrs = %x, tree_texPlttAdrs = %x\n",
		//			g3Dmapper->tree_texDataAdrs, g3Dmapper->tree_texPlttAdrs );
	}	
	//---------------------
	OS_Printf("g3DmapperSize = %x\n", sizeof(G3D_MAPPER));
	OS_Printf("mapBlockDataSize = %x\n", sizeof(MAP_BLOCK_DATA));
	OS_Printf("rndObjSize = %x\n", sizeof(NNSG3dRenderObj));
	
	return g3Dmapper;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e�����C��
 */
//------------------------------------------------------------------
void	Main3Dmapper( G3D_MAPPER* g3Dmapper )
{
	BLOCK_IDX nowBlockIdx[MAP_BLOCK_COUNT];
	int i;

	GF_ASSERT( g3Dmapper );

	if( g3Dmapper->data == NULL ){
		return;
	}
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		nowBlockIdx[i].blockIdx = MAPID_NULL;
	}

	switch( g3Dmapper->mode ){
	case G3D_MAPPER_MODE_SCROLL_NONE: 
		GetMapperBlockIdxAll( g3Dmapper, &g3Dmapper->posCont, &nowBlockIdx[0] );
		break;
	default:
	case G3D_MAPPER_MODE_SCROLL_XZ: 
		GetMapperBlockIdxXZ( g3Dmapper, &g3Dmapper->posCont, &nowBlockIdx[0] );
		break;
	case G3D_MAPPER_MODE_SCROLL_Y: 
		GetMapperBlockIdxY( g3Dmapper, &g3Dmapper->posCont, &nowBlockIdx[0] );
		break;
	}
	ReloadMapperBlock( g3Dmapper, &nowBlockIdx[0] );
	Load3Dmap( g3Dmapper );

	//���݃u���b�N��index�擾
	{
		u32 blockx = FX_Whole( FX_Div( g3Dmapper->posCont.x, g3Dmapper->width ) );
		u32 blockz = FX_Whole( FX_Div( g3Dmapper->posCont.z, g3Dmapper->width ) );

		g3Dmapper->nowBlockIdx = blockz * g3Dmapper->sizex + blockx;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���f�B�X�v���C
 */
static BOOL checkCullingBoxTest( NNSG3dRenderObj* rnd );
static BOOL checkCullingLength( GFL_G3D_CAMERA* g3Dcamera, VecFx32* pos );
static fx32 getCullingLength( GFL_G3D_CAMERA* g3Dcamera, VecFx32* pos );
//------------------------------------------------------------------
void	Draw3Dmapper( G3D_MAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera )
{
	MAP_BLOCK_DATA*		mapBlock;
	BOOL				cullResult; 
	VecFx32				trans;
	VecFx32				scale = { FX32_ONE, FX32_ONE, FX32_ONE };
	MtxFx33				rotate = { FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE };
	NNSG3dRenderObj		*NNSrnd, *NNSrnd_L;
	int					i, j;
	fx32				length, limitLength = 512 * FX32_ONE;

	//�`��J�n
	GFL_G3D_DRAW_Start();
	//�J�����O���[�o���X�e�[�g�ݒ�		
 	GFL_G3D_DRAW_SetLookAt();

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];
		if( (mapBlock->idx != MAPID_NULL)
		  &&(NNS_G3dRenderObjGetResMdl( mapBlock->NNSrnd ) != NULL )){
			
			NNS_G3dGlbSetBaseTrans( &mapBlock->trans );	// �ʒu�ݒ�
			NNS_G3dGlbSetBaseRot( &rotate );	// �p�x�ݒ�
			NNS_G3dGlbSetBaseScale( &scale );	// �X�P�[���ݒ�
			NNS_G3dGlbFlush();					//�O���[�o���X�e�[�g���f

			if( checkCullingBoxTest( mapBlock->NNSrnd ) == TRUE ){
				//�n�`�`��
				NNS_G3dDraw( mapBlock->NNSrnd );	

				//�z�u�I�u�W�F�N�g�`��
				for( j=0; j<LOCAL_OBJRND_COUNT; j++ ){
					if(	mapBlock->object[j].id != LOCAL_OBJID_NULL ){
//						if( checkCullingLength(g3Dcamera, &mapBlock->object[j].trans) == TRUE){
							VEC_Add( &mapBlock->object[j].trans, &mapBlock->trans, &trans );

							length = getCullingLength( g3Dcamera, &trans );
				
							NNSrnd = g3Dmapper->grobalObject[ mapBlock->object[j].id ].NNSrnd_H;
							NNSrnd_L = g3Dmapper->grobalObject[ mapBlock->object[j].id ].NNSrnd_L;

							if( ( length > limitLength )&&( NNSrnd_L != NULL ) ){
								NNSrnd = NNSrnd_L;
							}

//							if( ( checkCullingBoxTest( NNSrnd ) == TRUE )
//								&&( NNS_G3dRenderObjGetResMdl( NNSrnd ) != NULL ) ){
							if( NNS_G3dRenderObjGetResMdl( NNSrnd ) != NULL ){
	
								NNS_G3dGlbSetBaseTrans( &trans );// �ʒu�ݒ�
								NNS_G3dGlbSetBaseRot( &rotate );		// �p�x�ݒ�
								NNS_G3dGlbSetBaseScale( &scale );		// �X�P�[���ݒ�
								NNS_G3dGlbFlush();						//�O���[�o���X�e�[�g���f

								//�I�u�W�F�N�g�`��
								NNS_G3dDraw( NNSrnd );
							}
//						}
					}
				}
				DirectDraw3Dmapper( g3Dmapper, mapBlock, g3Dcamera );
			}
		}
	}
	//�`��I���i�o�b�t�@�X���b�v�j
	//GFL_G3D_DRAW_End();							
}

//--------------------------------------------------------------------------------------------
//�{�b�N�X���̐ϓ��O�e�X�g�ɂ��J�����O�`�F�b�N
//�i�I�u�W�F�N�g�̃O���[�o���X�e�[�^�X���f��ɌĂԂ��Ɓj
static s32 testBoundingBox( const GXBoxTestParam* boundingBox );

static BOOL checkCullingBoxTest( NNSG3dRenderObj* rnd )
{
	NNSG3dResMdlInfo*	modelInfo;		// ���f���f�[�^����̃{�b�N�X�e�X�g�p�p�����[�^�擾�p
	BOOL				result = TRUE;
	GXBoxTestParam		boundingBox;

	//�{�b�N�X�e�X�g�p�p�����[�^�擾
	modelInfo = NNS_G3dGetMdlInfo( NNS_G3dRenderObjGetResMdl(rnd) );

	//�o�E���f�B���O�{�b�N�X�쐬
	boundingBox.x		= modelInfo->boxX;
	boundingBox.y		= modelInfo->boxY;
	boundingBox.z		= modelInfo->boxZ;
	boundingBox.width	= modelInfo->boxW;
	boundingBox.height	= modelInfo->boxH;
	boundingBox.depth	= modelInfo->boxD;

	//��̃{�b�N�X�e�X�g�̔��̒l��posScale�ł����邱�Ƃɂ��{���̒l�ɂȂ�
	NNS_G3dGePushMtx();
	NNS_G3dGeScale( modelInfo->boxPosScale, modelInfo->boxPosScale, modelInfo->boxPosScale );
	
	//�`�F�b�N
	if( !testBoundingBox( &boundingBox ) ){
		result = FALSE;
	}
	NNS_G3dGePopMtx(1);

	return result;
}

static s32 testBoundingBox( const GXBoxTestParam* boundingBox )
{
	s32 testResult = 1;

	//�|���S�������́uFAR�ʌ����N���b�s���O�t���O�v�Ɓu�P�h�b�g�|���S���\���t���O�v��1�ɐݒ�
	//�����C�u��������̗v��
	NNS_G3dGePolygonAttr( GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE,           
					0, 0, GX_POLYGON_ATTR_MISC_FAR_CLIPPING | GX_POLYGON_ATTR_MISC_DISP_1DOT );

	//�|���S���A�g���r���[�g���f �����C�u�����w��菇
	NNS_G3dGeBegin( GX_BEGIN_TRIANGLES );							
	NNS_G3dGeEnd();	

	NNS_G3dGeBoxTest( boundingBox );	// �{�b�N�X�e�X�g
	NNS_G3dGeFlushBuffer();				// ���������

	//�`�F�b�N�{�́i�Ԃ�l��0�̏ꍇ�e�X�g�I���j�B 0:���̐ϓ��A����ȊO�̒l�͎��̐ϊO
	while ( G3X_GetBoxTestResult( &testResult ) != 0 ) ;

	return testResult;
}

//--------------------------------------------------------------------------------------------
// �J�����Ƃ̋����ɂ��J�����O�`�F�b�N
static fx32 getCullingLength( GFL_G3D_CAMERA* g3Dcamera, VecFx32* pos )
{
	VecFx32 camPos, vecLength;

	GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );

	VEC_Subtract( pos, &camPos, &vecLength );

	return VEC_Mag( &vecLength );
}

static BOOL checkCullingLength( GFL_G3D_CAMERA* g3Dcamera, VecFx32* pos )
{
	fx32 limitLength, length;

	GFL_G3D_CAMERA_GetFar( g3Dcamera, &limitLength );

	length = getCullingLength( g3Dcamera, pos );

	if( length < limitLength ){
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���j��
 */
//------------------------------------------------------------------
void	Delete3Dmapper( G3D_MAPPER* g3Dmapper )
{
	GF_ASSERT( g3Dmapper );

	//---------------------
	GFL_G3D_FreeVramTexture( g3Dmapper->tree_g3Dres );
	GFL_G3D_DeleteResource( g3Dmapper->tree_g3Dres );
	//---------------------
	ReleaseObjRes3Dmapper( g3Dmapper );	//�o�^���ꂽ�܂܂̏ꍇ��z�肵�č폜
	DeleteMapGraphicResource( g3Dmapper );

	GFL_HEAP_FreeMemory( g3Dmapper );
}


//------------------------------------------------------------------
/**
 * @brief	�}�b�v�f�[�^�Z�b�g
 */
//------------------------------------------------------------------
void ResistData3Dmapper( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST* resistData )
{
	MAP_BLOCK_DATA*	mapBlock;
	int i;

	GF_ASSERT( g3Dmapper );

	g3Dmapper->sizex = resistData->sizex;
	g3Dmapper->sizez = resistData->sizez;
	g3Dmapper->totalSize = resistData->totalSize;
	g3Dmapper->width = resistData->width;
	g3Dmapper->height = resistData->height;
	g3Dmapper->mode = resistData->mode;
	switch( g3Dmapper->mode ){
	case G3D_MAPPER_MODE_SCROLL_NONE:
		if( g3Dmapper->totalSize > MAP_BLOCK_COUNT ){
			OS_Printf("mapper mode set Error\n");
			g3Dmapper->mode = G3D_MAPPER_MODE_SCROLL_XZ;
		}
		break;
	case G3D_MAPPER_MODE_SCROLL_Y:
		if( g3Dmapper->sizex * g3Dmapper->sizez > MAP_BLOCK_COUNT/2 ){
			OS_Printf("mapper mode set Error\n");
			g3Dmapper->mode = G3D_MAPPER_MODE_SCROLL_XZ;
		}
		break;
	}
	g3Dmapper->arcID = resistData->arcID;
	g3Dmapper->data = resistData->data;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];

		//������
		InitMapGraphicResource( mapBlock );

		//�A�[�J�C�u�n���h���쐬
		if( mapBlock->arc != NULL ){
			GFL_ARC_CloseDataHandle( mapBlock->arc );
		}
		mapBlock->NNSrnd->resMdl = NULL; 
		mapBlock->arc = GFL_ARC_OpenDataHandle( g3Dmapper->arcID, g3Dmapper->heapID );
		GF_ASSERT( mapBlock->arc );
	}
}

//------------------------------------------------------------------
/**
 * @brief	�I�u�W�F�N�g���\�[�X�Z�b�g
 */
//------------------------------------------------------------------
void ResistObjRes3Dmapper( G3D_MAPPER* g3Dmapper, const G3D_MAPPEROBJ_RESIST* resistData )
{
	int i;

	GF_ASSERT( resistData->count <= GROBAL_OBJ_COUNT );
	
	ReleaseObjRes3Dmapper( g3Dmapper );

	for( i=0; i< resistData->count; i++ ){
		g3Dmapper->grobalObject[i].g3Dres_H = GFL_G3D_CreateResourceArc
										( resistData->arcID, resistData->data[i].highQ_ID );
		GFL_G3D_TransVramTexture( g3Dmapper->grobalObject[i].g3Dres_H ); 

		g3Dmapper->grobalObject[i].NNSrnd_H = NNS_G3dAllocRenderObj
												( GFL_G3D_GetAllocaterPointer() );

		MakeMapRender(	g3Dmapper->grobalObject[i].NNSrnd_H,
						g3Dmapper->grobalObject[i].g3Dres_H,
						g3Dmapper->grobalObject[i].g3Dres_H );

		if( resistData->data[i].lowQ_ID != NON_LOWQ ){
			g3Dmapper->grobalObject[i].g3Dres_L = GFL_G3D_CreateResourceArc
											( resistData->arcID, resistData->data[i].lowQ_ID );
			GFL_G3D_TransVramTexture( g3Dmapper->grobalObject[i].g3Dres_L ); 

			g3Dmapper->grobalObject[i].NNSrnd_L = NNS_G3dAllocRenderObj
													( GFL_G3D_GetAllocaterPointer() );

			MakeMapRender(	g3Dmapper->grobalObject[i].NNSrnd_L,
							g3Dmapper->grobalObject[i].g3Dres_L,
							g3Dmapper->grobalObject[i].g3Dres_L );
		}
	}
}

void ReleaseObjRes3Dmapper( G3D_MAPPER* g3Dmapper )
{
	int i;

	for( i=0; i<GROBAL_OBJ_COUNT; i++ ){
		if( g3Dmapper->grobalObject[i].NNSrnd_L != NULL ){
			NNS_G3dFreeRenderObj(	GFL_G3D_GetAllocaterPointer(), 
									g3Dmapper->grobalObject[i].NNSrnd_L );
			g3Dmapper->grobalObject[i].NNSrnd_L = NULL;
		}
		if( g3Dmapper->grobalObject[i].g3Dres_L != NULL ){
			GFL_G3D_FreeVramTexture( g3Dmapper->grobalObject[i].g3Dres_L );
			GFL_G3D_DeleteResource( g3Dmapper->grobalObject[i].g3Dres_L );
			g3Dmapper->grobalObject[i].g3Dres_L = NULL;
		}
		if( g3Dmapper->grobalObject[i].NNSrnd_H != NULL ){
			NNS_G3dFreeRenderObj(	GFL_G3D_GetAllocaterPointer(), 
									g3Dmapper->grobalObject[i].NNSrnd_H );
			g3Dmapper->grobalObject[i].NNSrnd_H = NULL;
		}
		if( g3Dmapper->grobalObject[i].g3Dres_H != NULL ){
			GFL_G3D_FreeVramTexture( g3Dmapper->grobalObject[i].g3Dres_H );
			GFL_G3D_DeleteResource( g3Dmapper->grobalObject[i].g3Dres_H );
			g3Dmapper->grobalObject[i].g3Dres_H = NULL;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�ʒu�Z�b�g
 */
//------------------------------------------------------------------
void SetPos3Dmapper( G3D_MAPPER* g3Dmapper, const VecFx32* pos )
{
	VEC_Set( &g3Dmapper->posCont, pos->x, pos->y, pos->z );
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�f�[�^�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static void	CreateMapGraphicResource( G3D_MAPPER* g3Dmapper )
{
	MAP_BLOCK_DATA*	mapBlock;
	int				i;
	u32				g3DresHeaderSize = GFL_G3D_GetResourceHeaderSize();

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];

		//������
		InitMapGraphicResource( mapBlock );

		mapBlock->NNSrnd = NNS_G3dAllocRenderObj( GFL_G3D_GetAllocaterPointer() );
		mapBlock->NNSrnd->resMdl = NULL; 

		//�A�[�J�C�u�n���h���쐬
		//mapBlock->arc = GFL_ARC_OpenDataHandle( g3Dmapper->arcID, g3Dmapper->heapID );
		//GF_ASSERT( mapBlock->arc );
		mapBlock->arc = NULL;
		//���f�����\�[�X�w�b�_�쐬
		mapBlock->g3DgroundResMdl = GFL_HEAP_AllocClearMemory(g3Dmapper->heapID,g3DresHeaderSize);
		//�e�N�X�`�����\�[�X�w�b�_�쐬
		mapBlock->g3DgroundResTex = GFL_HEAP_AllocClearMemory(g3Dmapper->heapID,g3DresHeaderSize);

		//���f�����\�[�X�n���h�����e�ݒ�
		GFL_G3D_CreateResource( mapBlock->g3DgroundResMdl, 
								GFL_G3D_RES_CHKTYPE_MDL,(void*)&mapBlock->data[0] );
		//�e�N�X�`�����\�[�X�n���h�����e�ݒ�
		GFL_G3D_CreateResource( mapBlock->g3DgroundResTex, 
								GFL_G3D_RES_CHKTYPE_TEX, (void*)&mapBlock->tex[0] );

		//�e�N�X�`���u�q�`�l�m��
		mapBlock->groundTexKey = NNS_GfdAllocTexVram( MAPTEX_SIZE, FALSE, 0 );
		//�p���b�g�u�q�`�l�m��
		mapBlock->groundPlttKey = NNS_GfdAllocPlttVram( MAPPLTT_SIZE, FALSE, 0 );

		GF_ASSERT( mapBlock->groundTexKey != NNS_GFD_ALLOC_ERROR_TEXKEY );
		GF_ASSERT( mapBlock->groundPlttKey != NNS_GFD_ALLOC_ERROR_PLTTKEY );
	}
}

//------------------------------------------------------------------
static void	DeleteMapGraphicResource( G3D_MAPPER* g3Dmapper )
{
	MAP_BLOCK_DATA*	mapBlock;
	int				i, j;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];

		NNS_G3dFreeRenderObj( GFL_G3D_GetAllocaterPointer(), mapBlock->NNSrnd );

		NNS_GfdFreePlttVram( mapBlock->groundPlttKey );
		NNS_GfdFreeTexVram( mapBlock->groundTexKey );

		GFL_HEAP_FreeMemory( mapBlock->g3DgroundResTex );
		GFL_HEAP_FreeMemory( mapBlock->g3DgroundResMdl );
		if( mapBlock->arc != NULL ){
			GFL_ARC_CloseDataHandle( mapBlock->arc );
		}
	}
}

//------------------------------------------------------------------
static void	InitMapGraphicData( MAP_BLOCK_DATA*	mapBlock )
{
	int i;

	for( i=0; i<LOCAL_OBJRND_COUNT; i++ ){
		mapBlock->object[i].id = LOCAL_OBJID_NULL;
		VEC_Set( &mapBlock->object[i].trans, 0, 0, 0 );
	}
	for( i=0; i<LOCAL_DIRECTDRAW_OBJ_COUNT; i++ ){
		mapBlock->directDrawObject[i].id = DDRAW_OBJID_NULL;
		VEC_Set( &mapBlock->directDrawObject[i].trans, 0, 0, 0 );
	}
}

static void	InitMapGraphicResource( MAP_BLOCK_DATA*	mapBlock )
{
	mapBlock->idx = MAPID_NULL;
	VEC_Set( &mapBlock->trans, 0, 0, 0 );
	mapBlock->loadSeq = 0;
	mapBlock->loadCount = 0;

	mapBlock->dataLoadReq = FALSE;
	mapBlock->dataID = 0;
	mapBlock->texLoadReq = FALSE;
	mapBlock->texID = 0;
	mapBlock->attrLoadReq = FALSE;
	mapBlock->attrID = 0;

	InitMapGraphicData( mapBlock );
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�����_�[�쐬
 */
//------------------------------------------------------------------
static void MakeMapRender( NNSG3dRenderObj* rndObj, GFL_G3D_RES* mdl, GFL_G3D_RES* tex )
{
	NNSG3dResFileHeader*	mdlFH = GFL_G3D_GetResourceFileHeader( mdl );
	NNSG3dResFileHeader*	texFH = GFL_G3D_GetResourceFileHeader( tex );
	//���f�����e�N�X�`�����\�[�X�|�C���^�擾
	NNSG3dResMdl*			pMdl = NNS_G3dGetMdlByIdx( NNS_G3dGetMdlSet( mdlFH ), 0 );	//0th mdl
	NNSG3dResTex*			pTex = NNS_G3dGetTex( texFH );

	GF_ASSERT(pMdl);

	//���f���ƃe�N�X�`�����\�[�X�Ƃ̊֘A�t��
	if( pTex ){
		NNS_G3dBindMdlTex( pMdl, pTex );
		NNS_G3dBindMdlPltt( pMdl, pTex );
	}
	//�����_�����O�I�u�W�F�N�g������
	NNS_G3dRenderObjInit( rndObj, pMdl );
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���[�h
 */
//------------------------------------------------------------------
enum {
	MDL_LOAD_START = 0,
	MDL_LOAD,
	TEX_LOAD_START,
	TEX_LOAD,
	RND_CREATE,
	TEX_TRANS,
	PLTT_TRANS,
	ATTR_LOAD_START,
	ATTR_LOAD,
	LOAD_END,
};

static void	Load3Dmap( G3D_MAPPER* g3Dmapper )
{
	MAP_BLOCK_DATA*	mapBlock;
	int				i;
	u32				dataAdrs;

	GF_ASSERT( g3Dmapper );

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];

		if( mapBlock->dataLoadReq == TRUE ){
			switch( mapBlock->loadSeq ){
			case MDL_LOAD_START:
				mapBlock->NNSrnd->resMdl = NULL; 
				InitMapGraphicData( mapBlock );

				//���f���f�[�^���[�h�J�n
				GFL_ARC_LoadDataOfsByHandle( mapBlock->arc, mapBlock->dataID, 
												0, MAPLOAD_SIZE, (void*)mapBlock->data );
				mapBlock->loadCount = 0;
				mapBlock->loadCount++;
				mapBlock->loadSeq = MDL_LOAD;
				break;
			case MDL_LOAD:
				dataAdrs = mapBlock->loadCount * MAPLOAD_SIZE;

				if( (dataAdrs + MAPLOAD_SIZE) < MAPDATA_SIZE){
					//���[�h�p��
					GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPLOAD_SIZE, 
														(void*)&mapBlock->data[dataAdrs] );
					mapBlock->loadCount++;
					break;
				}
				//�ŏI���[�h
				GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPDATA_SIZE - dataAdrs,
													(void*)&mapBlock->data[dataAdrs] );
				mapBlock->loadSeq = TEX_LOAD_START;
				break;
			case TEX_LOAD_START:
				if( mapBlock->texLoadReq == FALSE ){
					mapBlock->loadSeq = RND_CREATE;
					break;
				}
				//�e�N�X�`�����[�h�J�n
				GFL_ARC_LoadDataOfsByHandle( mapBlock->arc, mapBlock->texID, 
												0, MAPLOAD_SIZE, (void*)mapBlock->tex );
				mapBlock->loadCount = 0;
				mapBlock->loadCount++;
				mapBlock->loadSeq = TEX_LOAD;
				break;
			case TEX_LOAD:
				dataAdrs = mapBlock->loadCount * MAPLOAD_SIZE;

				if( (dataAdrs + MAPLOAD_SIZE) < MAPTEX_SIZE){
					//���[�h�p��
					GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPLOAD_SIZE, 
														(void*)&mapBlock->tex[dataAdrs] );
					mapBlock->loadCount++;
					break;
				}
				//�ŏI���[�h
				GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPTEX_SIZE - dataAdrs,
													(void*)&mapBlock->tex[dataAdrs] );
				//�e�N�X�`�����\�[�X�ւ̂u�q�`�l�L�[�̐ݒ�
				NNS_G3dTexSetTexKey( GFL_G3D_GetResTex(mapBlock->g3DgroundResTex), 
										mapBlock->groundTexKey, 0 );
				NNS_G3dPlttSetPlttKey( GFL_G3D_GetResTex(mapBlock->g3DgroundResTex), 
										mapBlock->groundPlttKey );

				mapBlock->loadSeq = RND_CREATE;
				break;
			case RND_CREATE:
				//�����_�[�쐬
				MakeMapRender
					( mapBlock->NNSrnd, mapBlock->g3DgroundResMdl, mapBlock->g3DgroundResTex );
				mapBlock->loadCount = 0;
				mapBlock->loadSeq = TEX_TRANS;
				break;
			case TEX_TRANS:
				dataAdrs = mapBlock->loadCount * MAPTRANS_SIZE;

				if( (dataAdrs + MAPTRANS_SIZE) < MAPTEX_SIZE){
					NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, 
						NNS_GfdGetTexKeyAddr( mapBlock->groundTexKey ) + dataAdrs,
						(void*)(GFL_G3D_GetAdrsTextureData
									( mapBlock->g3DgroundResTex ) + dataAdrs),
						MAPTRANS_SIZE );
					//�]���p��
					mapBlock->loadCount++;
					break;
				}
				//�ŏI�]��
				NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, 
						NNS_GfdGetTexKeyAddr( mapBlock->groundTexKey ) + dataAdrs,
						(void*)(GFL_G3D_GetAdrsTextureData
									( mapBlock->g3DgroundResTex ) + dataAdrs),
						MAPTEX_SIZE - dataAdrs );
				mapBlock->loadSeq = PLTT_TRANS;
				break;
			case PLTT_TRANS:
				NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_PLTT, 
						NNS_GfdGetPlttKeyAddr( mapBlock->groundPlttKey ),
						(void*)GFL_G3D_GetAdrsTexturePltt( mapBlock->g3DgroundResTex ),
						MAPPLTT_SIZE );
				mapBlock->loadSeq = ATTR_LOAD_START;
				//mapBlock->loadSeq = LOAD_END;
				break;
			case ATTR_LOAD_START:
				//�A�g���r���[�g�f�[�^���[�h�J�n
				if( mapBlock->attrID == NON_ATTR ){
					GFL_STD_MemClear32( (void*)mapBlock->attr, MAPATTR_SIZE );
					mapBlock->loadSeq = LOAD_END;
					break;
				}
				GFL_ARC_LoadDataOfsByHandle( mapBlock->arc, mapBlock->attrID, 
												0, MAPLOAD_SIZE, (void*)mapBlock->attr );
				mapBlock->loadCount = 0;
				mapBlock->loadCount++;
				mapBlock->loadSeq = ATTR_LOAD;
				break;
			case ATTR_LOAD:
				dataAdrs = mapBlock->loadCount * MAPLOAD_SIZE;

				if( (dataAdrs + MAPLOAD_SIZE) < MAPATTR_SIZE){
					//���[�h�p��
					GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPLOAD_SIZE, 
														(void*)&mapBlock->attr[dataAdrs] );
					mapBlock->loadCount++;
					break;
				}
				//�ŏI���[�h
				GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPATTR_SIZE - dataAdrs,
													(void*)&mapBlock->attr[dataAdrs] );
				mapBlock->loadSeq = LOAD_END;
				break;
			case LOAD_END:
#if 1
				//--------------------
				if( mapBlock->trans.y == 0 ){
					int j, r;
					fx32 randTransx, randTransz;
					fx32 cOffs, rOffs, gWidth;
					VecFx32 pWorld;

					cOffs = -g3Dmapper->width/2;
					gWidth = g3Dmapper->width/MAP_GRIDCOUNT;
					for( j=0; j<4; j++ ){
						if( j<2 ){
							mapBlock->object[j].id = 0;
						} else {
							mapBlock->object[j].id = 1;
						}

						rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
						mapBlock->object[j].trans.x = rOffs + cOffs;
						mapBlock->object[j].trans.y = 0;
						rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
						mapBlock->object[j].trans.z = rOffs + cOffs;

						VEC_Add( &mapBlock->object[j].trans, &mapBlock->trans, &pWorld );
						Get3DmapperHeight( g3Dmapper, &pWorld, &mapBlock->object[j].trans.y );
					}
					for( j=0; j<32; j++ ){
						mapBlock->directDrawObject[j].id = 1;

						rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
						mapBlock->directDrawObject[j].trans.x = rOffs + cOffs;
						mapBlock->directDrawObject[j].trans.y = 0;
						rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
						mapBlock->directDrawObject[j].trans.z = rOffs + cOffs;
						VEC_Add( &mapBlock->directDrawObject[j].trans, &mapBlock->trans, &pWorld );
						Get3DmapperHeight
							( g3Dmapper, &pWorld, &mapBlock->directDrawObject[j].trans.y );
					}
				}
				//--------------------
#endif
				mapBlock->dataLoadReq = FALSE;
				mapBlock->texLoadReq = FALSE;
				mapBlock->attrLoadReq = FALSE;
				mapBlock->loadSeq = 0;
				break;
			}
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���[�h���N�G�X�g�ݒ�
 */
//------------------------------------------------------------------
static void	Set3DmapLoadReq( G3D_MAPPER* g3Dmapper, const int blockID, const VecFx32* trans, 
								const u32 datID, const u32 texID, const u32 attrID )
{
	MAP_BLOCK_DATA* mapBlock;

	GF_ASSERT( blockID < MAP_BLOCK_COUNT );

	mapBlock = &g3Dmapper->mapBlock[blockID];

	if( mapBlock->texID != texID ){
		mapBlock->texID = texID;
		mapBlock->texLoadReq = TRUE;
	}
	mapBlock->attrID = attrID;
	mapBlock->attrLoadReq = TRUE;
	mapBlock->dataID = datID;
	mapBlock->dataLoadReq = TRUE;

	mapBlock->loadCount = 0;
	mapBlock->loadSeq = 0;

	VEC_Set( &mapBlock->trans, trans->x, trans->y, trans->z );
}


//------------------------------------------------------------------
/**
 * @brief	�}�b�v�X�V�u���b�N�擾
 */
//------------------------------------------------------------------
static void GetMapperBlockIdxAll( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx )
{
	u32		idx, county, countxz;
	fx32	width, height;
	int		i, j, offsx, offsz;

	countxz = g3Dmapper->sizex * g3Dmapper->sizez;
	county = g3Dmapper->totalSize / countxz;
	if( g3Dmapper->totalSize % countxz ){
		county++;
	}
	width = g3Dmapper->width;
	height = g3Dmapper->height;

	idx = 0;

	for( i=0; i<county; i++ ){
		for( j=0; j<countxz; j++ ){
			offsx = j % g3Dmapper->sizex;
			offsz = j / g3Dmapper->sizex;

			if( idx >= g3Dmapper->totalSize ){
				idx = MAPID_NULL;
			}
			blockIdx[idx].blockIdx = idx;
			blockIdx[idx].trans.x = offsx * width + width/2;
			blockIdx[idx].trans.y = i * height;
			blockIdx[idx].trans.z = offsz * width + width/2;
			idx++;
		}
	}
}

//------------------------------------------------------------------
typedef struct {
	s16	z;
	s16	x;
}BLOCK_OFFS;

static const BLOCK_OFFS blockPat_Around[] = {//�����̂���u���b�N������͕����ɂX�u���b�N�Ƃ�
	{-1,-1},{-1, 0},{-1, 1},{ 0,-1},{ 0, 0},{ 0, 1},{ 1,-1},{ 1, 0},{ 1, 1},
};

static void GetMapperBlockIdxXZ( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx )
{
	u16		sizex, sizez;
	u32		idx, idxmax, blockx, blockz;
	fx32	inBlockx, inBlockz, width;
	int		i, offsx, offsz;

	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	idxmax = sizex * sizez;
	width = g3Dmapper->width;

	blockx = FX_Whole( FX_Div( pos->x, width ) );
	blockz = FX_Whole( FX_Div( pos->z, width ) );

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		offsx = blockx + blockPat_Around[i].x;
		offsz = blockz + blockPat_Around[i].z;

		if((offsx < 0)||(offsx >= sizex)||(offsz < 0)||(offsz >= sizez)){
			idx = MAPID_NULL;
		} else {
			idx = offsz * sizex + offsx;

			if( idx >= idxmax ){
				idx = MAPID_NULL;
			}
		}
		blockIdx[i].blockIdx = idx;
		blockIdx[i].trans.x = offsx * width + width/2;
		blockIdx[i].trans.y = 0;
		blockIdx[i].trans.z = offsz * width + width/2;
	}
}

//------------------------------------------------------------------
static void GetMapperBlockIdxY( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx )
{
	u16		sizex, sizez;
	u32		idx, blocky, countxz;
	fx32	width, height;
	int		i, p, offsx, offsy, offsz;

	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	countxz = sizex * sizez;
	width = g3Dmapper->width;
	height = g3Dmapper->height;

	blocky = FX_Whole( FX_Div( pos->y, height ) );
	if( pos->y - ( blocky * height ) > height/2 ){
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
		blockIdx[p].blockIdx = idx;
		blockIdx[p].trans.x = offsx * width + width/2;
		blockIdx[p].trans.y = blocky * height;
		blockIdx[p].trans.z = offsz * width + width/2;
		p++;
	}
	for( i=0; i<countxz; i++ ){
		offsx = i % g3Dmapper->sizex;
		offsz = i / g3Dmapper->sizex;

		idx = (blocky + offsy) * countxz + offsz * sizex + offsx;

		if( idx >= g3Dmapper->totalSize ){
			idx = MAPID_NULL;
		}
		blockIdx[p].blockIdx = idx;
		blockIdx[p].trans.x = offsx * width + width/2;
		blockIdx[p].trans.y = (blocky + offsy) * height;
		blockIdx[p].trans.z = offsz * width + width/2;
		p++;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�u���b�N�X�V�`�F�b�N
 */
//------------------------------------------------------------------
static BOOL	ReloadMapperBlock( G3D_MAPPER* g3Dmapper, BLOCK_IDX* new )
{
	BOOL addFlag, delFlag, delProcFlag, addProcFlag, reloadFlag;
	int i, j, c;

	reloadFlag = FALSE;

	//�X�V�u���b�N�`�F�b�N
	delProcFlag = FALSE;
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		if( g3Dmapper->mapBlock[i].idx != MAPID_NULL ){
			delFlag = FALSE;
			for( j=0; j<MAP_BLOCK_COUNT; j++ ){
				if(( g3Dmapper->mapBlock[i].idx == new[j].blockIdx )&&(delFlag == FALSE )){
					new[j].blockIdx = MAPID_NULL;
					delFlag = TRUE;
				}
			}
			if( delFlag == FALSE ){
				g3Dmapper->mapBlock[i].idx = MAPID_NULL;
				delProcFlag = TRUE;
			}
		}
	}
	//�X�V
	addProcFlag = FALSE;
	c = 0;
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		if( new[i].blockIdx != MAPID_NULL ){
			addFlag = FALSE;
			for( j=0; j<MAP_BLOCK_COUNT; j++ ){
				if(( g3Dmapper->mapBlock[j].idx == MAPID_NULL )&&(addFlag == FALSE )){

					Set3DmapLoadReq( g3Dmapper, j, &new[i].trans, 
										(u32)g3Dmapper->data[new[i].blockIdx].datID,
										(u32)g3Dmapper->data[new[i].blockIdx].texID,
										(u32)g3Dmapper->data[new[i].blockIdx].attrID );
					g3Dmapper->mapBlock[j].idx = new[i].blockIdx;
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
typedef void (DIRECT_DRAW_FUNC)( u32 texDataAdrs, u32 texPlttAdrs, VecFx16* vecN, BOOL lodSt );

enum {
	DRAW_NORMAL = 0,
	DRAW_YBILLBOARD,
};

typedef struct {
	GXRgb				diffuse;
    GXRgb				ambient;
	GXRgb				specular;
    GXRgb				emission;
	u8					polID;
	u8					alpha;
	u8					drawType;	
	DIRECT_DRAW_FUNC*	func;

}DIRECT_DRAW_DATA;

static const DIRECT_DRAW_DATA drawTreeData;

static const DIRECT_DRAW_DATA* directDrawDataTable[] = {
	NULL,
	&drawTreeData,
};

static void	DirectDraw3Dmapper
	( G3D_MAPPER* g3Dmapper, MAP_BLOCK_DATA* mapBlock, GFL_G3D_CAMERA* g3Dcamera )
{
	const DIRECT_DRAW_DATA* ddData;
	DIRECT_OBJ*		ddObject;
	MtxFx33	mtxBillboard;
	VecFx32	trans, gTrans;
	VecFx16	vecN;
	int		scaleVal = 8;
	int		i;

	G3X_Reset();

	//�J�����ݒ�擾
	{
		VecFx32		camPos, camUp, target, vecNtmp;

		GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
		GFL_G3D_CAMERA_GetCamUp( g3Dcamera, &camUp );
		GFL_G3D_CAMERA_GetTarget( g3Dcamera, &target );

		G3_LookAt( &camPos, &camUp, &target, NULL );

		VEC_Subtract( &camPos, &target, &vecNtmp );
		VEC_Normalize( &vecNtmp, &vecNtmp );
		VEC_Fx16Set( &vecN, vecNtmp.x, vecNtmp.y, vecNtmp.z );

		vecNtmp.y = 0;
		VEC_Normalize( &vecNtmp, &vecNtmp );
		MTX_Identity33( &mtxBillboard );
		MTX_RotY33( &mtxBillboard, vecNtmp.x, vecNtmp.z );
	}
	//GFL_G3D_LIGHT_Switching( g3Dlightset );

	//�O���[�o���X�P�[���ݒ�
	G3_Scale( FX32_ONE * scaleVal, FX32_ONE * scaleVal, FX32_ONE * scaleVal );

	for( i=0; i<LOCAL_DIRECTDRAW_OBJ_COUNT; i++ ){
		ddObject = &mapBlock->directDrawObject[i];

		if( ddObject->id != DDRAW_OBJID_NULL ){
			GF_ASSERT( ddObject->id  < NELEMS(directDrawDataTable) );

			VEC_Add( &ddObject->trans, &mapBlock->trans, &gTrans );

			if( checkCullingLength( g3Dcamera, &gTrans ) == TRUE ){
				G3_PushMtx();

				ddData = directDrawDataTable[ ddObject->id ]; 

				//��]�A���s�ړ��p�����[�^�ݒ�
				trans.x = gTrans.x / scaleVal;
				trans.y = gTrans.y / scaleVal;
				trans.z = gTrans.z / scaleVal;
				if( ddData->drawType == DRAW_YBILLBOARD ){
					G3_MultTransMtx33( &mtxBillboard, &trans );
				} else {
					G3_Translate( trans.x, trans.y, trans.z );
				}

				//�}�e���A���ݒ�
				G3_MaterialColorDiffAmb( ddData->diffuse, ddData->ambient, TRUE );
				G3_MaterialColorSpecEmi( ddData->specular, ddData->emission, FALSE );
				G3_PolygonAttr(	0/*���C�g�}�X�N*/, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
							ddData->polID, ddData->alpha, GX_POLYGON_ATTR_MISC_FOG );

				if( ddData->func != NULL ){
					fx32 limitLength = 512 * FX32_ONE;
					fx32 length = getCullingLength( g3Dcamera, &gTrans );
				
					ddData->func(	g3Dmapper->tree_texDataAdrs, g3Dmapper->tree_texPlttAdrs, 
									&vecN, ( length < limitLength) );
				}

				G3_PopMtx(1);
			}
		}
	}
	//�����G3D_System�ōs���̂ŁA�����ł͂��Ȃ�
	//G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

//------------------------------------------------------------------
/**
 *
 * @brief	��
 *
 */
//------------------------------------------------------------------
static void	_drawTree0( u32 texDataAdrs, u32 texPlttAdrs, VecFx16* vecN, BOOL lodSt )
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

static const DIRECT_DRAW_DATA drawTreeData = {
	GX_RGB(31,31,31), GX_RGB(16,16,16), GX_RGB(16,16,16), GX_RGB(0,0,0),
	63, 31, DRAW_YBILLBOARD,
	_drawTree0,
};





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
void InitGet3DmapperGridInfoData( G3D_MAPPER_INFODATA* gridInfoData )
{
	VEC_Set( &gridInfoData->vecN, 0, 0, 0 );
	gridInfoData->attr = 0;
	gridInfoData->height = 0;
}

void InitGet3DmapperGridInfo( G3D_MAPPER_GRIDINFO* gridInfo )
{
	int i;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		InitGet3DmapperGridInfoData( &gridInfo->gridData[i] );
	}
	gridInfo->count = 0;
}

#if 0
//------------------------------------------------------------------
/**
 * @brief	�u���b�N���擾�P
 */
//------------------------------------------------------------------
static BOOL	GetMapBlockInMap( G3D_MAPPER* g3Dmapper, const VecFx32* pos, 
							u32* blockNum, fx32* inBlockx, fx32* inBlockz ) 
{
	u32		blockx, blockz;
	fx32	width;
	VecFx32	blockTop, vecInBlock;
	
	width = g3Dmapper->width;

	//���݃u���b�N��XZindex�擾
	blockx = FX_Whole( FX_Div( pos->x, width ) );
	blockz = FX_Whole( FX_Div( pos->z, width ) );

	blockTop.x = blockx * width;
	blockTop.y = 0;
	blockTop.z = blockz * width;

	VEC_Subtract( pos, &blockTop, &vecInBlock );

	*blockNum = blockz * g3Dmapper->sizex + blockx;
	*inBlockx = vecInBlock.x;
	*inBlockz = vecInBlock.z;

	//OS_Printf("pos {%x,%x,%x}, blockNum = %x, blockx = %x, blockz = %x\n", 
	//			pos->x, pos->y, pos->z, *blockIdx, blockx, blockz );

	if( *blockNum >= g3Dmapper->sizex * g3Dmapper->sizez ){
		return FALSE;
	}
	return TRUE;
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�u���b�N���擾�Q
 */
//------------------------------------------------------------------
static BOOL	GetMapBlockInBuffer( G3D_MAPPER* g3Dmapper, const VecFx32* pos, 
							u32* blockIdx, fx32* inBlockx, fx32* inBlockz ) 
{
	VecFx32 vecTop, vecGrid, inBlockPos;
	VecFx32 vecDefault = {0,0,0};
	fx32	width = g3Dmapper->width;
	fx32	height = g3Dmapper->height;
	int i;

	if( g3Dmapper->mode == G3D_MAPPER_MODE_SCROLL_Y ){
		int count = 0;
		int data = 0;
		for( i=0; i<MAP_BLOCK_COUNT; i++ ){
			if( g3Dmapper->mapBlock[i].idx != MAPID_NULL){
				vecTop.x = g3Dmapper->mapBlock[i].trans.x - width/2;
				//vecTop.y = g3Dmapper->mapBlock[i].trans.y;
				vecTop.z = g3Dmapper->mapBlock[i].trans.z - width/2;
	
				if(	(pos->x >= vecTop.x)&&(pos->x < vecTop.x + width)
					&&(pos->y >= vecTop.y)&&(pos->y < vecTop.y + height)
					&&(pos->z >= vecTop.z)&&(pos->z < vecTop.z + width) ){
	
					*blockIdx = i;
					VEC_Subtract( &vecDefault, &vecTop, &vecGrid );
					VEC_Add( &vecGrid, pos, &inBlockPos );
					*inBlockx = inBlockPos.x;
					*inBlockz = inBlockPos.z;
					count++;
				}
				data++;
			}
		}
		if( count ){
			//OS_Printf("data = %x, count = %x\n", data, count);
			return TRUE;
		}

	} else {
		for( i=0; i<MAP_BLOCK_COUNT; i++ ){
			if( g3Dmapper->mapBlock[i].idx != MAPID_NULL){
				vecTop.x = g3Dmapper->mapBlock[i].trans.x - width/2;
				vecTop.y = g3Dmapper->mapBlock[i].trans.y;
				vecTop.z = g3Dmapper->mapBlock[i].trans.z - width/2;
	
				if(	(pos->x >= vecTop.x)&&(pos->x < vecTop.x + width)
					&&(pos->z >= vecTop.z)&&(pos->z < vecTop.z + width) ){
	
					*blockIdx = i;
					VEC_Subtract( &vecDefault, &vecTop, &vecGrid );
					VEC_Add( &vecGrid, pos, &inBlockPos );
					*inBlockx = inBlockPos.x;
					*inBlockz = inBlockPos.z;
					return TRUE;
				}
			}
		}
	}
	*blockIdx = 0;
	*inBlockx = 0;
	*inBlockz = 0;
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�O���b�h���擾
 */
//------------------------------------------------------------------
static void	GetMapGrid( G3D_MAPPER* g3Dmapper, const fx32 inBlockx, const fx32 inBlockz,
						u32* gridIdx, fx32* inGridx, fx32* inGridz ) 
{
	u32		gridx, gridz;
	fx32	gridWidth = g3Dmapper->width / MAP_GRIDCOUNT;

	gridx = inBlockx / gridWidth;
	gridz = inBlockz / gridWidth;
	*inGridx = inBlockx % gridWidth;
	*inGridz = inBlockz % gridWidth;

	*gridIdx = gridz * MAP_GRIDCOUNT + gridx;

	//OS_Printf("gridIdx = %x, gridx = %x, gridz = %x\n", *gridIdx, gridx, gridz );
}

//------------------------------------------------------------------
/**
 * @brief	�O���b�h���O�p�`�p�^�[���擾
 */
//------------------------------------------------------------------
static u32 GetMapTriangleID( G3D_MAPPER* g3Dmapper, 
					const fx32 inGridx, const fx32 inGridz, const u32 gridTryPattern )
{
	fx32	gridWidth = g3Dmapper->width / MAP_GRIDCOUNT;

	//�O���b�h���O�p�`�̔���
	if( gridTryPattern == 0 ){
		//0-2-1,3-1-2�̃p�^�[��
		if( inGridx + inGridz < gridWidth ){
			return 0;
		} else {
			return 1;
		}
	} else {
		//2-3-0,1-0-3�̃p�^�[��
		if( inGridx > inGridz ){
			return 0;
		} else {
			return 1;
		}
	}
	return 0;
}

//------------------------------------------------------------------
/**
 * @brief	�O���b�h���擾
 */
//------------------------------------------------------------------
static void GetGridInfoData( G3D_MAPPER_INFODATA* gridInfo, const MAP_BLOCK_DATA* mapBlock,
								const VecFx32* pos, const fx32 map_width )
{
	const VecFx32*	mapTrans;
	fx32			block_hw, grid_w, block_x, block_z, grid_x, grid_z;
	u32				grid_idx;
	fx32			by, valD;
	NormalVtxSt*	nvs;
	u8				triangleType;

	mapTrans = &mapBlock->trans;
	block_hw = map_width / 2;			//�}�b�v���̔���
	grid_w = map_width / MAP_GRIDCOUNT;	//�}�b�v�����O���b�h���ŕ���

	//�u���b�N�����擾
	block_x = pos->x - ( mapTrans->x - block_hw );
	block_z = pos->z - ( mapTrans->z - block_hw );

	//�O���b�h�����擾
	grid_idx = ( block_z / grid_w ) * MAP_GRIDCOUNT + ( block_x / grid_w );
	grid_x = block_x % grid_w;
	grid_z = block_z % grid_w;

	//���擾(���̎������Ⴄ�̂Ŗ@���x�N�g����Z���])
	nvs = &((NormalVtxSt*)&mapBlock->attr)[grid_idx];

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
		VEC_Set( &gridInfo->vecN, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
		valD = nvs->vecN1_D;
	} else {
		VEC_Set( &gridInfo->vecN, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
		valD = nvs->vecN2_D;
	}
	by = -( FX_Mul( gridInfo->vecN.x, pos->x - mapTrans->x )
			+ FX_Mul( gridInfo->vecN.z, pos->z - mapTrans->z ) + valD );
	gridInfo->attr = nvs->attr;

	gridInfo->height = FX_Div( by, gridInfo->vecN.y ) + mapTrans->y;
}
	
//------------------------------------------------------------------
/**
 * @brief	�O���b�h���擾
 */
//------------------------------------------------------------------
void Get3DmapperGridInfo
	( G3D_MAPPER* g3Dmapper, const VecFx32* pos, G3D_MAPPER_GRIDINFO* gridInfo )
{
	int		i, p;

	InitGet3DmapperGridInfo( gridInfo );
	
	p = 0;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		if( g3Dmapper->mapBlock[i].idx != MAPID_NULL){

			fx32 min_x = g3Dmapper->mapBlock[i].trans.x - g3Dmapper->width/2;
			fx32 min_z = g3Dmapper->mapBlock[i].trans.z - g3Dmapper->width/2;
			fx32 max_x = g3Dmapper->mapBlock[i].trans.x + g3Dmapper->width/2;
			fx32 max_z = g3Dmapper->mapBlock[i].trans.z + g3Dmapper->width/2;

			//�u���b�N�͈͓��`�F�b�N�i�}�b�v�u���b�N�̂w�y���ʏ�n�_�j
			if(	(pos->x >= min_x)&&(pos->x < max_x)&&(pos->z >= min_z)&&(pos->z < max_z) ){

				GetGridInfoData( &gridInfo->gridData[p], &g3Dmapper->mapBlock[i],
									pos, g3Dmapper->width );
				p++;
			}
		}
	}
	gridInfo->count = p;
}

//------------------------------------------------------------------
/**
 * @brief	���݈ʒu�n�`�̖@���x�N�g���擾
 */
//------------------------------------------------------------------
void Get3DmapperVecN( G3D_MAPPER* g3Dmapper, const VecFx32* pos, VecFx32* vecN )
{
	u32		blockIdx;
	fx32	inBlockx, inBlockz;
	u32		gridIdx;
	fx32	inGridx, inGridz;
	NormalVtxSt* mapData;

	vecN->x = 0; 
	vecN->y = FX32_ONE;
	vecN->z = 0; 

	if( g3Dmapper->data == NULL ){
		return;
	}
	if( GetMapBlockInBuffer( g3Dmapper, pos, &blockIdx, &inBlockx, &inBlockz ) == FALSE ){
		return;
	}
	mapData = (NormalVtxSt*)&g3Dmapper->mapBlock[blockIdx].attr;

	GetMapGrid( g3Dmapper, inBlockx, inBlockz, &gridIdx, &inGridx, &inGridz ); 

	if( GetMapTriangleID( g3Dmapper, inGridx, inGridz, mapData[gridIdx].tryangleType ) == 0 ){
		vecN->x = mapData[gridIdx].vecN1_x; 
		vecN->y = mapData[gridIdx].vecN1_y; 
		vecN->z = -mapData[gridIdx].vecN1_z; 
	} else {
		vecN->x = mapData[gridIdx].vecN2_x; 
		vecN->y = mapData[gridIdx].vecN2_y; 
		vecN->z = -mapData[gridIdx].vecN2_z; 
	}
	//OS_Printf("vecN{ %x, %x, %x }\n", vecN->x, vecN->y, vecN->z);
}

//------------------------------------------------------------------
#if 0
void Get3DmapperVecN_fromROM( G3D_MAPPER* g3Dmapper, const VecFx32* pos, VecFx32* vecN )
{
	u32		blockNum;
	fx32	inBlockx, inBlockz;
	u32		gridIdx;
	fx32	inGridx, inGridz;
	NormalVtxSt mapData;
	u16		attrID;

	vecN->x = 0; 
	vecN->y = FX32_ONE;
	vecN->z = 0; 

	if( g3Dmapper->data == NULL ){
		return;
	}
	if( GetMapBlockInMap( g3Dmapper, pos, &blockNum, &inBlockx, &inBlockz ) == FALSE ){
		return;
	}
	GetMapGrid( g3Dmapper, inBlockx, inBlockz, &gridIdx, &inGridx, &inGridz ); 

	attrID = g3Dmapper->data[blockNum].attrID;

	if( attrID == NON_ATTR ){
		return;
	}
	GFL_ARC_LoadDataOfs( (void*)&mapData, g3Dmapper->arcID, (u32)attrID,
							sizeof(NormalVtxSt)*gridIdx, sizeof(NormalVtxSt) );

	if( GetMapTriangleID( g3Dmapper, inGridx, inGridz, mapData.tryangleType ) == 0 ){
		vecN->x = mapData.vecN1_x; 
		vecN->y = mapData.vecN1_y; 
		vecN->z = -mapData.vecN1_z; 
	} else {
		vecN->x = mapData.vecN2_x; 
		vecN->y = mapData.vecN2_y; 
		vecN->z = -mapData.vecN2_z; 
	}
}
#endif

//------------------------------------------------------------------
/**
 * @brief	���݈ʒu�n�`�̍����擾
 */
//------------------------------------------------------------------
void Get3DmapperHeight( G3D_MAPPER* g3Dmapper, const VecFx32* pos, fx32* height )
{
	u32		blockIdx, blockNum;
	fx32	inBlockx, inBlockz;
	u32		gridIdx;
	fx32	inGridx, inGridz;
	NormalVtxSt* mapData;
	fx32	by, valD;
	VecFx32 vecN, posMap, posLocalMap;
	u16		attrID;

	*height = 0;

	if( g3Dmapper->data == NULL ){
		return;
	}
	if( GetMapBlockInBuffer( g3Dmapper, pos, &blockIdx, &inBlockx, &inBlockz ) == FALSE ){
		return;
	}
	blockNum = g3Dmapper->mapBlock[blockIdx].idx;
	mapData = (NormalVtxSt*)&g3Dmapper->mapBlock[blockIdx].attr;
	VEC_Subtract( pos, &g3Dmapper->mapBlock[blockIdx].trans, &posLocalMap );

	GetMapGrid( g3Dmapper, inBlockx, inBlockz, &gridIdx, &inGridx, &inGridz ); 

	if( GetMapTriangleID( g3Dmapper, inGridx, inGridz, mapData[gridIdx].tryangleType ) == 0 ){
		vecN.x = mapData[gridIdx].vecN1_x; 
		vecN.y = mapData[gridIdx].vecN1_y; 
		vecN.z = -mapData[gridIdx].vecN1_z;
		valD = mapData[gridIdx].vecN1_D;		//���̎������Ⴄ�̂Ŕ��]
	} else {
		vecN.x = mapData[gridIdx].vecN2_x; 
		vecN.y = mapData[gridIdx].vecN2_y; 
		vecN.z = -mapData[gridIdx].vecN2_z;
		valD = mapData[gridIdx].vecN2_D;		//���̎������Ⴄ�̂Ŕ��]
	}
	by = -( FX_Mul( vecN.x, posLocalMap.x ) + FX_Mul( vecN.z, posLocalMap.z ) + valD );
	*height = FX_Div( by, vecN.y ) + g3Dmapper->mapBlock[blockIdx].trans.y;
}

//------------------------------------------------------------------
#if 0
void Get3DmapperHeight_fromROM( G3D_MAPPER* g3Dmapper, const VecFx32* pos, fx32* height )
{
	u32		blockNum;
	fx32	inBlockx, inBlockz;
	u32		gridIdx;
	fx32	inGridx, inGridz;
	NormalVtxSt mapData;
	fx32	by, valD, width;
	VecFx32 vecN, posMap, posLocalMap;
	u16		attrID;

	*height = 0;

	if( g3Dmapper->data == NULL ){
		return;
	}
	if( GetMapBlockInMap( g3Dmapper, pos, &blockNum, &inBlockx, &inBlockz ) == FALSE ){
		return;
	}
	width = g3Dmapper->width;
	posMap.x = FX_Whole( FX_Div( pos->x, width ) ) * width + width/2;
	posMap.y = 0;
	posMap.z = FX_Whole( FX_Div( pos->z, width ) ) * width + width/2;
	VEC_Subtract( pos, &posMap, &posLocalMap );
	//OS_Printf("blockNum = %x, inBlockx = %x, inBlockz = %x ", blockNum, inBlockx, inBlockz );

	GetMapGrid( g3Dmapper, inBlockx, inBlockz, &gridIdx, &inGridx, &inGridz ); 

	attrID = g3Dmapper->data[blockNum].attrID;

	if( attrID == NON_ATTR ){
		return;
	}
	GFL_ARC_LoadDataOfs( (void*)&mapData, g3Dmapper->arcID, (u32)attrID,
							sizeof(NormalVtxSt)*gridIdx, sizeof(NormalVtxSt) );

	if( GetMapTriangleID( g3Dmapper, inGridx, inGridz, mapData.tryangleType ) == 0 ){
		vecN.x = mapData.vecN1_x; 
		vecN.y = mapData.vecN1_y; 
		vecN.z = -mapData.vecN1_z;
		valD = mapData.vecN1_D;		//���̎������Ⴄ�̂Ŕ��]
	} else {
		vecN.x = mapData.vecN2_x; 
		vecN.y = mapData.vecN2_y; 
		vecN.z = -mapData.vecN2_z;
		valD = mapData.vecN2_D;		//���̎������Ⴄ�̂Ŕ��]
	}
	by = -( FX_Mul( vecN.x, posLocalMap.x ) + FX_Mul( vecN.z, posLocalMap.z ) + valD );
	*height = FX_Div( by, vecN.y );
	//���z�u���ꂽ�ꍇ��Y���W�𑫂����ށi�������j
}
#endif


//------------------------------------------------------------------
/**
 * @brief	�ړ������̒n�`�ɉ������x�N�g���擾
 */
//------------------------------------------------------------------
void Get3DmapperGroundMoveVec
	( G3D_MAPPER* g3Dmapper, const VecFx32* pos, const VecFx32* vecMove, VecFx32* vecResult )
{
	VecFx32 vecN, vecH, vecV;

	//���ʂ̖@���x�N�g���ɂ��ړ��x�N�g���ɐ����ŎΖʂɕ��s�ȃx�N�g�����Z�o
	Get3DmapperVecN( g3Dmapper, pos, &vecN );	//���ʂ̖@�����擾
	VEC_CrossProduct( &vecN, vecMove, &vecH );		//���ʏ�̐����x�N�g���Z�o
	VEC_CrossProduct( &vecN, &vecH, &vecV );		//���ʏ�̎Ζʃx�N�g���Z�o

	if( VEC_DotProduct( &vecV, vecMove ) < 0 ){
		//�t�����ւ̃x�N�g�����o�Ă��܂����ꍇ�C��
		VEC_Set( vecResult, -vecV.x, -vecV.y, -vecV.z );
	} else {
		VEC_Set( vecResult, vecV.x, vecV.y, vecV.z );
	}
	VEC_Normalize( vecResult, vecResult );
}

#if 0
void Get3DmapperGroundMoveVec_fromROM
	( G3D_MAPPER* g3Dmapper, const VecFx32* pos, const VecFx32* vecMove, VecFx32* vecResult )
{
	VecFx32 vecN, vecH, vecV;

	//���ʂ̖@���x�N�g���ɂ��ړ��x�N�g���ɐ����ŎΖʂɕ��s�ȃx�N�g�����Z�o
	Get3DmapperVecN_fromROM( g3Dmapper, pos, &vecN );	//���ʂ̖@�����擾
	VEC_CrossProduct( &vecN, vecMove, &vecH );		//���ʏ�̐����x�N�g���Z�o
	VEC_CrossProduct( &vecN, &vecH, &vecV );		//���ʏ�̎Ζʃx�N�g���Z�o

	if( VEC_DotProduct( &vecV, vecMove ) < 0 ){
		//�t�����ւ̃x�N�g�����o�Ă��܂����ꍇ�C��
		VEC_Set( vecResult, -vecV.x, -vecV.y, -vecV.z );
	} else {
		VEC_Set( vecResult, vecV.x, vecV.y, vecV.z );
	}
	VEC_Normalize( vecResult, vecResult );
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�͈͊O�`�F�b�N
 */
//------------------------------------------------------------------
BOOL Check3DmapperOutRange( G3D_MAPPER* g3Dmapper, const VecFx32* pos )
{
	fx32 widthx, widthz;

	widthx = g3Dmapper->sizex * g3Dmapper->width;
	widthz = g3Dmapper->sizez * g3Dmapper->width;

	if( ( pos->x >= 0 )&&( pos->x < widthx )&&( pos->z >= 0 )&&( pos->z < widthz ) ){
		return FALSE;
	}
	return TRUE;
}





