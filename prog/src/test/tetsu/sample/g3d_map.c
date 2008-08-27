//============================================================================================
/**
 * @file	g3d_map.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "system\gfl_use.h"	//�����p

#include "g3d_map.h"

//============================================================================================
/**
 *
 *
 *
 * @brief	�R�c�}�b�v�f�[�^�R���g���[��
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
#define OBJID_NULL			(0xffffffff)

//#define LOCAL_OBJRES_COUNT	(16)
#define OBJ_COUNT	(32)
#define DDOBJ_COUNT	(64)

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

//------------------------------------------------------------------
typedef struct {
	u32			id;
	VecFx32		trans;
}MAP_OBJECT_DATA;

struct _GFL_G3D_MAP 
{
	BOOL				drawSw;
	VecFx32				trans;
	ARCHANDLE*			arc;

	u32					mdlHeapSize;
	u32					texHeapSize;
	u32					attrHeapSize;

	u32					loadSeq;
	u32					loadCount;

	NNSG3dRenderObj*	NNSrnd;	//�n�`�����_�[

	BOOL				mdlLoadReq;
	u32					mdlID;
	GFL_G3D_RES*		groundResMdl;	//�n�`���f�����\�[�X
	u8*					mdl;

	BOOL				texLoadReq;
	u32					texID;
	GFL_G3D_RES*		groundResTex;	//���[�J���n�`�e�N�X�`�����\�[�X
	u8*					tex;

	BOOL				attrLoadReq;
	u32					attrID;
	u8*					attr;

	NNSGfdTexKey		groundTexKey;
	NNSGfdPlttKey		groundPlttKey;

	MAP_OBJECT_DATA		object[OBJ_COUNT];	//�z�u�I�u�W�F�N�g
	MAP_OBJECT_DATA		directDrawObject[DDOBJ_COUNT];

#if 0
	BOOL				objLoadReq;
	u32					objID;
	GFL_G3D_RES*		g3DobjectResMdl[LOCAL_OBJRES_COUNT];//���[�J���I�u�W�F�N�g���f�����\�[�X
	u8					obj[MAPOBJ_SIZE];

	NNSGfdTexKey		objectTexKey;
	NNSGfdPlttKey		objectPlttKey;
#endif
};

enum {
	LOAD_IDLING = 0,
	MDL_LOAD_START,
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

static const VecFx32 defaultScale = { FX32_ONE, FX32_ONE, FX32_ONE };
static const MtxFx33 defaultRotate = { FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE };
//------------------------------------------------------------------
static BOOL	DrawGround( GFL_G3D_MAP* g3Dmap, GFL_G3D_CAMERA* g3Dcamera );
static void	DrawObj
			( GFL_G3D_MAP* g3Dmap, const GFL_G3D_MAP_OBJ* obj, GFL_G3D_CAMERA* g3Dcamera );
static void	DirectDrawObj
			( GFL_G3D_MAP* g3Dmap, const GFL_G3D_MAP_DDOBJ* ddobj, GFL_G3D_CAMERA* g3Dcamera );

static void		getViewLength( GFL_G3D_CAMERA* g3Dcamera, const VecFx32* pos, fx32* result );
static void		getYbillboardMtx( GFL_G3D_CAMERA* g3Dcamera, MtxFx33* result );

static void		InitMapObject( GFL_G3D_MAP* g3Dmap );
static void		InitWork( GFL_G3D_MAP* g3Dmap );
static void		MakeMapRender( NNSG3dRenderObj* rndObj, GFL_G3D_RES* mdl, GFL_G3D_RES* tex );
static s32		testBoundingBox( const GXBoxTestParam* boundingBox );
static BOOL		checkCullingBoxTest( NNSG3dRenderObj* rnd );

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�V�X�e���쐬
 */
//------------------------------------------------------------------
GFL_G3D_MAP*	GFL_G3D_MAP_Create( GFL_G3D_MAP_SETUP* setup, HEAPID heapID )
{
	GFL_G3D_MAP*	g3Dmap = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_G3D_MAP) );
	u32				g3DresHeaderSize = GFL_G3D_GetResourceHeaderSize();

	//������
	InitWork( g3Dmap );

	//�f�[�^�i�[�G���A�m��
	g3Dmap->mdlHeapSize = setup->mdlHeapSize;
	g3Dmap->texHeapSize = setup->texHeapSize;
	g3Dmap->attrHeapSize = setup->attrHeapSize;
	g3Dmap->mdl = GFL_HEAP_AllocClearMemory( heapID, g3Dmap->mdlHeapSize );
	g3Dmap->tex = GFL_HEAP_AllocClearMemory( heapID, g3Dmap->texHeapSize );
	g3Dmap->attr = GFL_HEAP_AllocClearMemory( heapID, g3Dmap->attrHeapSize );

	//�����_�[�쐬
	g3Dmap->NNSrnd = NNS_G3dAllocRenderObj( GFL_G3D_GetAllocaterPointer() );
	g3Dmap->NNSrnd->resMdl = NULL; 

	//�A�[�J�C�u�n���h���쐬
	g3Dmap->arc = NULL;

	//���f�����\�[�X�w�b�_�쐬
	g3Dmap->groundResMdl = GFL_HEAP_AllocClearMemory( heapID, g3DresHeaderSize );
	GFL_G3D_CreateResource( g3Dmap->groundResMdl, 
								GFL_G3D_RES_CHKTYPE_MDL,(void*)g3Dmap->mdl );

	//�e�N�X�`�����\�[�X�w�b�_�쐬
	g3Dmap->groundResTex = GFL_HEAP_AllocClearMemory( heapID, g3DresHeaderSize );
	GFL_G3D_CreateResource( g3Dmap->groundResTex, 
								GFL_G3D_RES_CHKTYPE_TEX, (void*)g3Dmap->tex );

	//�e�N�X�`�����p���b�g�u�q�`�l�m��
	g3Dmap->groundTexKey = NNS_GfdAllocTexVram( g3Dmap->texHeapSize, FALSE, 0 );
	g3Dmap->groundPlttKey = NNS_GfdAllocPlttVram( MAPPLTT_SIZE, FALSE, 0 );

	GF_ASSERT( g3Dmap->groundTexKey != NNS_GFD_ALLOC_ERROR_TEXKEY );
	GF_ASSERT( g3Dmap->groundPlttKey != NNS_GFD_ALLOC_ERROR_PLTTKEY );

	return g3Dmap;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�V�X�e���j��
 */
//------------------------------------------------------------------
void	GFL_G3D_MAP_Delete( GFL_G3D_MAP* g3Dmap )
{
	GF_ASSERT( g3Dmap );

	if( g3Dmap->arc != NULL ){
		GFL_ARC_CloseDataHandle( g3Dmap->arc );
	}
	NNS_GfdFreePlttVram( g3Dmap->groundPlttKey );
	NNS_GfdFreeTexVram( g3Dmap->groundTexKey );

	GFL_HEAP_FreeMemory( g3Dmap->groundResTex );
	GFL_HEAP_FreeMemory( g3Dmap->groundResMdl );

	NNS_G3dFreeRenderObj( GFL_G3D_GetAllocaterPointer(), g3Dmap->NNSrnd );

	GFL_HEAP_FreeMemory( g3Dmap->attr );
	GFL_HEAP_FreeMemory( g3Dmap->tex );
	GFL_HEAP_FreeMemory( g3Dmap->mdl );

	GFL_HEAP_FreeMemory( g3Dmap );
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���C��
 *				���f�[�^�ǂݍ���
 */
//------------------------------------------------------------------
void	GFL_G3D_MAP_Main( GFL_G3D_MAP* g3Dmap )
{
	u32	dataAdrs;

	GF_ASSERT( g3Dmap );

	switch( g3Dmap->loadSeq ){
	case LOAD_IDLING:
		break;

	case MDL_LOAD_START:
		g3Dmap->NNSrnd->resMdl = NULL; 
		InitMapObject( g3Dmap );

		//���f���f�[�^���[�h�J�n
		GFL_ARC_LoadDataOfsByHandle( g3Dmap->arc, g3Dmap->mdlID, 
										0, MAPLOAD_SIZE, (void*)g3Dmap->mdl );
		g3Dmap->loadCount = 0;
		g3Dmap->loadCount++;
		g3Dmap->loadSeq = MDL_LOAD;
		break;
	case MDL_LOAD:
		dataAdrs = g3Dmap->loadCount * MAPLOAD_SIZE;

		if( (dataAdrs + MAPLOAD_SIZE) < g3Dmap->mdlHeapSize){
			//���[�h�p��
			GFL_ARC_LoadDataByHandleContinue(	g3Dmap->arc, MAPLOAD_SIZE, 
												(void*)((u32)g3Dmap->mdl + dataAdrs) );
			g3Dmap->loadCount++;
			break;
		}
		//�ŏI���[�h
		GFL_ARC_LoadDataByHandleContinue(	g3Dmap->arc, g3Dmap->mdlHeapSize - dataAdrs,
											(void*)((u32)g3Dmap->mdl + dataAdrs) );
		g3Dmap->mdlLoadReq = FALSE;
		g3Dmap->loadSeq = TEX_LOAD_START;
		break;
	case TEX_LOAD_START:
		if( g3Dmap->texLoadReq == FALSE ){
			g3Dmap->loadSeq = RND_CREATE;
			break;
		}
		//�e�N�X�`�����[�h�J�n
		GFL_ARC_LoadDataOfsByHandle( g3Dmap->arc, g3Dmap->texID, 
										0, MAPLOAD_SIZE, (void*)g3Dmap->tex );
		g3Dmap->loadCount = 0;
		g3Dmap->loadCount++;
		g3Dmap->loadSeq = TEX_LOAD;
		break;
	case TEX_LOAD:
		dataAdrs = g3Dmap->loadCount * MAPLOAD_SIZE;

		if( (dataAdrs + MAPLOAD_SIZE) < g3Dmap->texHeapSize){
			//���[�h�p��
			GFL_ARC_LoadDataByHandleContinue(	g3Dmap->arc, MAPLOAD_SIZE, 
												(void*)((u32)g3Dmap->tex + dataAdrs) );
			g3Dmap->loadCount++;
			break;
		}
		//�ŏI���[�h
		GFL_ARC_LoadDataByHandleContinue(	g3Dmap->arc, g3Dmap->texHeapSize - dataAdrs,
											(void*)((u32)g3Dmap->tex + dataAdrs) );
		g3Dmap->texLoadReq = FALSE;
		//�e�N�X�`�����\�[�X�ւ̂u�q�`�l�L�[�̐ݒ�
		NNS_G3dTexSetTexKey( GFL_G3D_GetResTex(g3Dmap->groundResTex), 
								g3Dmap->groundTexKey, 0 );
		NNS_G3dPlttSetPlttKey( GFL_G3D_GetResTex(g3Dmap->groundResTex), 
								g3Dmap->groundPlttKey );
		g3Dmap->loadSeq = RND_CREATE;
		break;
	case RND_CREATE:
		//�����_�[�쐬
		MakeMapRender
			( g3Dmap->NNSrnd, g3Dmap->groundResMdl, g3Dmap->groundResTex );
		g3Dmap->loadCount = 0;
		g3Dmap->loadSeq = TEX_TRANS;
		break;
	case TEX_TRANS:
		dataAdrs = g3Dmap->loadCount * MAPTRANS_SIZE;

		if( (dataAdrs + MAPTRANS_SIZE) < g3Dmap->texHeapSize){
			NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, 
				NNS_GfdGetTexKeyAddr( g3Dmap->groundTexKey ) + dataAdrs,
				(void*)(GFL_G3D_GetAdrsTextureData( g3Dmap->groundResTex ) + dataAdrs),
				MAPTRANS_SIZE );
			//�]���p��
			g3Dmap->loadCount++;
			break;
		}
		//�ŏI�]��
		NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, 
				NNS_GfdGetTexKeyAddr( g3Dmap->groundTexKey ) + dataAdrs,
				(void*)(GFL_G3D_GetAdrsTextureData( g3Dmap->groundResTex ) + dataAdrs),
				g3Dmap->texHeapSize - dataAdrs );
		g3Dmap->loadSeq = PLTT_TRANS;
		break;
	case PLTT_TRANS:
		NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_PLTT, 
				NNS_GfdGetPlttKeyAddr( g3Dmap->groundPlttKey ),
				(void*)GFL_G3D_GetAdrsTexturePltt( g3Dmap->groundResTex ),
				MAPPLTT_SIZE );
		g3Dmap->loadSeq = ATTR_LOAD_START;
		break;
	case ATTR_LOAD_START:
		//�A�g���r���[�g�f�[�^���[�h�J�n
		if( g3Dmap->attrID == NON_ATTR ){
			GFL_STD_MemClear32( (void*)g3Dmap->attr, g3Dmap->attrHeapSize );
			g3Dmap->loadSeq = LOAD_END;
			break;
		}
		GFL_ARC_LoadDataOfsByHandle( g3Dmap->arc, g3Dmap->attrID, 
										0, MAPLOAD_SIZE, (void*)g3Dmap->attr );
		g3Dmap->loadCount = 0;
		g3Dmap->loadCount++;
		g3Dmap->loadSeq = ATTR_LOAD;
		break;
	case ATTR_LOAD:
		dataAdrs = g3Dmap->loadCount * MAPLOAD_SIZE;

		if( (dataAdrs + MAPLOAD_SIZE) < g3Dmap->attrHeapSize){
			//���[�h�p��
			GFL_ARC_LoadDataByHandleContinue(	g3Dmap->arc, MAPLOAD_SIZE, 
												(void*)((u32)g3Dmap->attr + dataAdrs) );
			g3Dmap->loadCount++;
			break;
		}
		//�ŏI���[�h
		GFL_ARC_LoadDataByHandleContinue(	g3Dmap->arc, g3Dmap->attrHeapSize - dataAdrs,
											(void*)((u32)g3Dmap->attr + dataAdrs) );
		g3Dmap->attrLoadReq = FALSE;
		g3Dmap->loadSeq = LOAD_END;
		break;
	case LOAD_END:
		g3Dmap->loadSeq = LOAD_IDLING;
#if 1
		//--------------------
		if( g3Dmap->trans.y == 0 ){
			int i, r;
			fx32 randTransx, randTransz;
			fx32 cOffs, rOffs, gWidth, mapWidth;
			VecFx32 pWorld;
			GFL_G3D_MAP_ATTRINFO attrInfo;
			BOOL f;

			mapWidth = 512 * FX32_ONE;
			cOffs = -mapWidth/2;
			gWidth = mapWidth/MAP_GRIDCOUNT;
			for( i=0; i<4; i++ ){
				if( i<2 ){
					g3Dmap->object[i].id = 0;
				} else {
					g3Dmap->object[i].id = 1;
				}

				rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
				g3Dmap->object[i].trans.x = rOffs + cOffs;
				g3Dmap->object[i].trans.y = 0;
				rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
				g3Dmap->object[i].trans.z = rOffs + cOffs;

				VEC_Add( &g3Dmap->object[i].trans, &g3Dmap->trans, &pWorld );
				GFL_G3D_MAP_GetAttr( &attrInfo, g3Dmap, &pWorld, mapWidth );
				g3Dmap->object[i].trans.y = attrInfo.height;
			}
			for( i=0; i<32; i++ ){
				g3Dmap->directDrawObject[i].id = 0;

				rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
				g3Dmap->directDrawObject[i].trans.x = rOffs + cOffs;
				g3Dmap->directDrawObject[i].trans.y = 0;
				rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
				g3Dmap->directDrawObject[i].trans.z = rOffs + cOffs;
				VEC_Add( &g3Dmap->directDrawObject[i].trans, &g3Dmap->trans, &pWorld );
				GFL_G3D_MAP_GetAttr( &attrInfo, g3Dmap, &pWorld, mapWidth );
				g3Dmap->directDrawObject[i].trans.y = attrInfo.height;
			}
		}
		//--------------------
#endif
		break;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�`��
 *		�J�n�I������уO���[�o���X�e�[�g�ݒ�͊֐��O�ōs���Ă�������
 *
 *		ex)
 *		{
 *			GFL_G3D_MAP_StartDraw();	//�`��J�n
 *
 *			for( i=0; i<g3Dmap_count; i++ ){
 *				GFL_G3D_MAP_Draw( ...... );		//�e�}�b�v�`��֐�
 *			
 *			GFL_G3D_MAP_EndDraw();		//�`��I��
 *		}
 */
//------------------------------------------------------------------
void	GFL_G3D_MAP_StartDraw( void )
{
	GFL_G3D_DRAW_Start();		//�`��J�n
	GFL_G3D_DRAW_SetLookAt();	//�J�����O���[�o���X�e�[�g�ݒ�		
}

void	GFL_G3D_MAP_Draw( GFL_G3D_MAP* g3Dmap, GFL_G3D_CAMERA* g3Dcamera,
							const GFL_G3D_MAP_OBJ* obj, const GFL_G3D_MAP_DDOBJ* ddobj )
{
	GF_ASSERT( g3Dmap );

	//�n�`�`��
	if( DrawGround( g3Dmap, g3Dcamera ) == TRUE ){
		//�z�u�I�u�W�F�N�g�`��
		if( obj != NULL ){
			DrawObj( g3Dmap, obj, g3Dcamera );
		}
		if( ddobj != NULL ){
			DirectDrawObj( g3Dmap, ddobj, g3Dcamera );
		}
	}
}

void	GFL_G3D_MAP_EndDraw( void )
{
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�A�[�J�C�u�f�[�^�o�^
 */
//------------------------------------------------------------------
void	GFL_G3D_MAP_ResistArc( GFL_G3D_MAP* g3Dmap, const u32 arcID, HEAPID heapID )
{
	GF_ASSERT( g3Dmap );

	//������
	InitWork( g3Dmap );

	//�A�[�J�C�u�n���h�������ɃI�[�v������Ă���ꍇ�̓N���[�Y
	GFL_G3D_MAP_ReleaseArc( g3Dmap );
	g3Dmap->NNSrnd->resMdl = NULL; 

	//�A�[�J�C�u�n���h���쐬
	g3Dmap->arc = GFL_ARC_OpenDataHandle( arcID, heapID );
	GF_ASSERT( g3Dmap->arc );
}

void	GFL_G3D_MAP_ReleaseArc( GFL_G3D_MAP* g3Dmap )
{
	GF_ASSERT( g3Dmap );

	if( g3Dmap->arc != NULL ){
		GFL_ARC_CloseDataHandle( g3Dmap->arc );
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���[�h���N�G�X�g�ݒ�
 */
//------------------------------------------------------------------
void	GFL_G3D_MAP_SetLoadReq( GFL_G3D_MAP* g3Dmap,
								const u32 datID, const u32 texID, const u32 attrID )
{
	GF_ASSERT( g3Dmap );
	GF_ASSERT( g3Dmap->arc );

	if( g3Dmap->texID != texID ){
		g3Dmap->texID = texID;
		g3Dmap->texLoadReq = TRUE;
	}
	g3Dmap->attrID = attrID;
	g3Dmap->attrLoadReq = TRUE;
	g3Dmap->mdlID = datID;
	g3Dmap->mdlLoadReq = TRUE;

	g3Dmap->loadCount = 0;
	g3Dmap->loadSeq = MDL_LOAD_START;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�`��n�m�^�n�e�e���擾
 */
//------------------------------------------------------------------
void	GFL_G3D_MAP_SetDrawSw( GFL_G3D_MAP* g3Dmap, BOOL drawSw )
{
	GF_ASSERT( g3Dmap );

	g3Dmap->drawSw = drawSw;
}

BOOL	GFL_G3D_MAP_GetDrawSw( GFL_G3D_MAP* g3Dmap )
{
	GF_ASSERT( g3Dmap );

	return g3Dmap->drawSw;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�ʒu�ݒ聕�擾
 */
//------------------------------------------------------------------
void	GFL_G3D_MAP_SetTrans( GFL_G3D_MAP* g3Dmap, const VecFx32* trans )
{
	GF_ASSERT( g3Dmap );

	g3Dmap->trans = *trans;
}

void	GFL_G3D_MAP_GetTrans( GFL_G3D_MAP* g3Dmap, VecFx32* trans )
{
	GF_ASSERT( g3Dmap );

	*trans = g3Dmap->trans;
}




//============================================================================================
/**
 *
 *
 *
 * @brief	�e��`��
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�n�`�`��
 */
//------------------------------------------------------------------
static BOOL	DrawGround( GFL_G3D_MAP* g3Dmap, GFL_G3D_CAMERA* g3Dcamera )
{
	if( (g3Dmap->drawSw != FALSE)&&(NNS_G3dRenderObjGetResMdl(g3Dmap->NNSrnd) != NULL )){
		
		NNS_G3dGlbSetBaseTrans( &g3Dmap->trans );	// �ʒu�ݒ�
		NNS_G3dGlbSetBaseRot( &defaultRotate );		// �p�x�ݒ�
		NNS_G3dGlbSetBaseScale( &defaultScale );	// �X�P�[���ݒ�
		NNS_G3dGlbFlush();							//�O���[�o���X�e�[�g���f

		if( checkCullingBoxTest( g3Dmap->NNSrnd ) == TRUE ){
			//�n�`�`��
			NNS_G3dDraw( g3Dmap->NNSrnd );	
			return TRUE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�z�u�I�u�W�F�N�g�`��
 */
//------------------------------------------------------------------
static void	DrawObj
	( GFL_G3D_MAP* g3Dmap, const GFL_G3D_MAP_OBJ* obj, GFL_G3D_CAMERA* g3Dcamera )
{
	NNSG3dRenderObj	*NNSrnd, *NNSrnd_L;
	VecFx32			grobalTrans;
	fx32			length;
	int				i;

	for( i=0; i<OBJ_COUNT; i++ ){
		if(	g3Dmap->object[i].id != OBJID_NULL ){

			VEC_Add( &g3Dmap->object[i].trans, &g3Dmap->trans, &grobalTrans );

			getViewLength( g3Dcamera, &grobalTrans, &length );

			if( length <= DRAW_LIMIT ){
	
				NNSrnd = obj[ g3Dmap->object[i].id ].NNSrnd_H;
				NNSrnd_L = obj[ g3Dmap->object[i].id ].NNSrnd_L;

				if( ( length > LOD_LIMIT )&&( NNSrnd_L != NULL ) ){
					NNSrnd = NNSrnd_L;
				}

				if( NNS_G3dRenderObjGetResMdl( NNSrnd ) != NULL ){
	
					NNS_G3dGlbSetBaseTrans( &grobalTrans );		// �ʒu�ݒ�
					NNS_G3dGlbSetBaseRot( &defaultRotate );		// �p�x�ݒ�
					NNS_G3dGlbSetBaseScale( &defaultScale );	// �X�P�[���ݒ�
					NNS_G3dGlbFlush();							//�O���[�o���X�e�[�g���f
	
					if( checkCullingBoxTest( NNSrnd ) == TRUE ){
						//�I�u�W�F�N�g�`��
						NNS_G3dDraw( NNSrnd );
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�z�u�I�u�W�F�N�g�`��(�W�I���g��������)
 */
//------------------------------------------------------------------
static void	DirectDrawObj
	( GFL_G3D_MAP* g3Dmap, const GFL_G3D_MAP_DDOBJ* ddobj, GFL_G3D_CAMERA* g3Dcamera )
{
	const GFL_G3D_MAP_DDOBJ*	objData;
	MAP_OBJECT_DATA*			ddObject;
	MtxFx33		mtxBillboard;
	VecFx32		trans, grobalTrans;
	VecFx16		vecView;
	fx32		length;
	int			i;

	G3X_Reset();

	//�J�����ݒ�
	{
		VecFx32	camPos, camUp, target, tmp;

		GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
		GFL_G3D_CAMERA_GetCamUp( g3Dcamera, &camUp );
		GFL_G3D_CAMERA_GetTarget( g3Dcamera, &target );
		G3_LookAt( &camPos, &camUp, &target, NULL );

		VEC_Subtract( &camPos, &target, &tmp );
		VEC_Normalize( &tmp, &tmp );
		VEC_Fx16Set( &vecView, tmp.x, tmp.y, tmp.z );
	}
	//Y���r���{�[�h�p��]MTX�擾
	getYbillboardMtx( g3Dcamera, &mtxBillboard );

	for( i=0; i<DDOBJ_COUNT; i++ ){
		ddObject = &g3Dmap->directDrawObject[i];

		if( ddObject->id != OBJID_NULL ){
			objData = &ddobj[ ddObject->id ];

			VEC_Add( &ddObject->trans, &g3Dmap->trans, &grobalTrans );

			getViewLength( g3Dcamera, &grobalTrans, &length );

			if( length <= DRAW_LIMIT ){
				G3_PushMtx();

				//�X�P�[���ݒ�
				G3_Scale(	FX32_ONE * objData->data->scaleVal, 
							FX32_ONE * objData->data->scaleVal, 
							FX32_ONE * objData->data->scaleVal );
				//��]�A���s�ړ��p�����[�^�ݒ�
				trans.x = grobalTrans.x / objData->data->scaleVal;
				trans.y = grobalTrans.y / objData->data->scaleVal;
				trans.z = grobalTrans.z / objData->data->scaleVal;
				if( objData->data->drawType == DRAW_YBILLBOARD ){
					G3_MultTransMtx33( &mtxBillboard, &trans );
				} else {
					G3_Translate( trans.x, trans.y, trans.z );
				}
				//�}�e���A���ݒ�
				G3_MaterialColorDiffAmb( objData->data->diffuse, objData->data->ambient, TRUE );
				G3_MaterialColorSpecEmi( objData->data->specular, objData->data->emission, FALSE );
				G3_PolygonAttr(	objData->data->lightMask, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
							objData->data->polID, objData->data->alpha, GX_POLYGON_ATTR_MISC_FOG );

				if( objData->data->func != NULL ){
					objData->data->func(	ddobj[ ddObject->id ].texDataAdrs, 
											ddobj[ ddObject->id ].texPlttAdrs, 
											&vecView, (length < LOD_LIMIT) );
				}
				G3_PopMtx(1);
			}
		}
	}
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
 * @brief	�A�g���r���[�g�擾���[�N������
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_InitAttr( GFL_G3D_MAP_ATTRINFO* attrInfo )
{
	VEC_Fx16Set( &attrInfo->vecN, 0, 0, 0 );
	attrInfo->attr = 0;
	attrInfo->height = 0;
}

//------------------------------------------------------------------
/**
 * @brief	�A�g���r���[�g�擾
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_GetAttr( GFL_G3D_MAP_ATTRINFO* attrInfo, const GFL_G3D_MAP* g3Dmap,
							const VecFx32* pos, const fx32 map_width )
{
	const VecFx32*	mapTrans;
	fx32			block_hw, grid_w, block_x, block_z, grid_x, grid_z;
	u32				grid_idx;
	VecFx32			vecN;
	fx32			by, valD;
	NormalVtxSt*	nvs;
	u8				triangleType;

	if( g3Dmap->attrLoadReq == TRUE ){
		VEC_Fx16Set( &attrInfo->vecN, 0, 0, 0 );
		attrInfo->height = 0;
		attrInfo->attr = 0;
		return;
	}
	mapTrans = &g3Dmap->trans;
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
	nvs = (NormalVtxSt*)(g3Dmap->attr + grid_idx * sizeof(NormalVtxSt));

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
	by = -( FX_Mul(vecN.x, pos->x - mapTrans->x) + FX_Mul(vecN.z, pos->z - mapTrans->z) + valD );
	attrInfo->attr = nvs->attr;

	attrInfo->height = FX_Div( by, attrInfo->vecN.y ) + mapTrans->y;
}





//============================================================================================
/**
 *
 *
 *
 * @brief	���[�J���֐�
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
static void	InitMapObject( GFL_G3D_MAP* g3Dmap )
{
	int i;

	for( i=0; i<OBJ_COUNT; i++ ){
		g3Dmap->object[i].id = OBJID_NULL;
		VEC_Set( &g3Dmap->object[i].trans, 0, 0, 0 );
	}
	for( i=0; i<DDOBJ_COUNT; i++ ){
		g3Dmap->directDrawObject[i].id = OBJID_NULL;
		VEC_Set( &g3Dmap->directDrawObject[i].trans, 0, 0, 0 );
	}
}

static void	InitWork( GFL_G3D_MAP* g3Dmap )
{
	g3Dmap->drawSw = FALSE;
	VEC_Set( &g3Dmap->trans, 0, 0, 0 );
	g3Dmap->loadSeq = LOAD_IDLING;
	g3Dmap->loadCount = 0;

	g3Dmap->mdlLoadReq = FALSE;
	g3Dmap->mdlID = 0;
	g3Dmap->texLoadReq = FALSE;
	g3Dmap->texID = 0;
	g3Dmap->attrLoadReq = FALSE;
	g3Dmap->attrID = 0;

	InitMapObject( g3Dmap );
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
// �J�����Ƃ̋����v�Z
static void getViewLength( GFL_G3D_CAMERA* g3Dcamera, const VecFx32* pos, fx32* result )
{
	VecFx32 camPos, vecLength;

	GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );

	VEC_Subtract( pos, &camPos, &vecLength );

	*result = VEC_Mag( &vecLength );
}

//Y���r���{�[�h�p��]MTX�v�Z
static void getYbillboardMtx( GFL_G3D_CAMERA* g3Dcamera, MtxFx33* result )
{
	VecFx32	camPos, target, tmp;

	GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
	GFL_G3D_CAMERA_GetTarget( g3Dcamera, &target );

	VEC_Subtract( &camPos, &target, &tmp );
	tmp.y = 0;
	VEC_Normalize( &tmp, &tmp );
	MTX_Identity33( result );
	MTX_RotY33( result, tmp.x, tmp.z );
}

