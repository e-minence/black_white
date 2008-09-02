//============================================================================================
/**
 * @file	g3d_map.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

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
typedef struct {
	u32			id;
	VecFx32		trans;
	u16			rotate;
}MAP_OBJ_DATA;

typedef struct {
	u32			id;
	VecFx32		trans;
}MAP_DDOBJ_DATA;

struct _GFL_G3D_MAP 
{
	BOOL						drawSw;
	VecFx32						trans;
	ARCHANDLE*					arc;
	GFL_G3D_MAPDATA_FILETYPE	fileType;

	u32							mapDataHeapSize;
	u32							texVramSize;
	const MAPFILE_FUNC*			mapFileFunc;

	NNSG3dRenderObj*			NNSrnd;	//�n�`�����_�[

	u32							datID;
	u32							texID;
	u32							attrID;

	u8*							attr;

	GFL_G3D_RES*				groundResMdl;	//�n�`���f�����\�[�X
	GFL_G3D_RES*				groundResTex;	//���[�J���n�`�e�N�X�`�����\�[�X
	NNSGfdTexKey				groundTexKey;
	NNSGfdPlttKey				groundPlttKey;

	MAP_OBJ_DATA				object[OBJ_COUNT];	//�z�u�I�u�W�F�N�g
	MAP_DDOBJ_DATA				directDrawObject[DDOBJ_COUNT];

	GFL_G3D_MAP_LOAD_STATUS		ldst;
	void*						mapData;

	GFL_G3D_RES*				grobalResTex;	//�O���[�o���n�`�e�N�X�`�����\�[�X
#if 0
	BOOL				objLoadReq;
	u32					objID;
	GFL_G3D_RES*		g3DobjectResMdl[LOCAL_OBJRES_COUNT];//���[�J���I�u�W�F�N�g���f�����\�[�X
	u8					obj[MAPOBJ_SIZE];

	NNSGfdTexKey		objectTexKey;
	NNSGfdPlttKey		objectPlttKey;
#endif
};

typedef BOOL (GFL_G3D_MAPLOAD_FUNC)( GFL_G3D_MAP* g3Dmap );

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
static void		TrashMapRender( NNSG3dRenderObj* rndObj );
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

	//�֐��e�[�u���o�^
	g3Dmap->mapFileFunc = setup->mapFileFunc;

	//�f�[�^�i�[�G���A�m��
	g3Dmap->mapDataHeapSize = setup->mapDataHeapSize;
	g3Dmap->mapData = GFL_HEAP_AllocClearMemory( heapID, setup->mapDataHeapSize );

	//�����_�[�쐬
	g3Dmap->NNSrnd = NNS_G3dAllocRenderObj( GFL_G3D_GetAllocaterPointer() );
	g3Dmap->NNSrnd->resMdl = NULL; 

	//�A�[�J�C�u�n���h���쐬
	g3Dmap->arc = NULL;

	//���f�����\�[�X�w�b�_�쐬
	g3Dmap->groundResMdl = GFL_HEAP_AllocClearMemory( heapID, g3DresHeaderSize );

	//�e�N�X�`�����\�[�X�w�b�_�쐬
	g3Dmap->groundResTex = GFL_HEAP_AllocClearMemory( heapID, g3DresHeaderSize );
	g3Dmap->grobalResTex = NULL;

	//�e�N�X�`�����p���b�g�u�q�`�l�m��
	if( setup->texVramSize != 0 ){
		g3Dmap->texVramSize = setup->texVramSize;
		g3Dmap->groundTexKey = NNS_GfdAllocTexVram( setup->texVramSize, FALSE, 0 );
		g3Dmap->groundPlttKey = NNS_GfdAllocPlttVram( MAPPLTT_SIZE, FALSE, 0 );

		GF_ASSERT( g3Dmap->groundTexKey != NNS_GFD_ALLOC_ERROR_TEXKEY );
		GF_ASSERT( g3Dmap->groundPlttKey != NNS_GFD_ALLOC_ERROR_PLTTKEY );
	} else {
		g3Dmap->groundTexKey = 0;
		g3Dmap->groundPlttKey = 0;
	}
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

	GFL_G3D_MAP_ReleaseArc( g3Dmap );		//�A�[�J�C�u�n���h�������i�ی��j

	if( g3Dmap->texVramSize != 0 ){
		NNS_GfdFreePlttVram( g3Dmap->groundPlttKey );
		NNS_GfdFreeTexVram( g3Dmap->groundTexKey );
	}
	GFL_HEAP_FreeMemory( g3Dmap->groundResTex );
	GFL_HEAP_FreeMemory( g3Dmap->groundResMdl );

	NNS_G3dFreeRenderObj( GFL_G3D_GetAllocaterPointer(), g3Dmap->NNSrnd );

	GFL_HEAP_FreeMemory( g3Dmap->mapData );

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
	GFL_G3D_MAP_FILELOAD_FUNC*	loadFunc;

	GF_ASSERT( g3Dmap );

	if( g3Dmap->ldst.seq != LOAD_IDLING ){
		loadFunc = g3Dmap->mapFileFunc[g3Dmap->fileType].loadFunc;

		if( loadFunc != NULL ){
			loadFunc( g3Dmap );
		}
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
 * @brief	�R�c�}�b�v�O���[�o���e�N�X�`�����\�[�X�o�^
 */
//------------------------------------------------------------------
void	GFL_G3D_MAP_ResistGrobalTex( GFL_G3D_MAP* g3Dmap, GFL_G3D_RES* grobalResTex )
{
	g3Dmap->grobalResTex = grobalResTex;
}

void	GFL_G3D_MAP_ReleaseGrobalTex( GFL_G3D_MAP* g3Dmap )
{
	g3Dmap->grobalResTex = NULL;
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

	TrashMapRender( g3Dmap->NNSrnd );
	GFL_G3D_MAP_DeleteResourceMdl( g3Dmap );
	GFL_G3D_MAP_DeleteResourceTex( g3Dmap );
	GFL_G3D_MAP_DeleteResourceAttr( g3Dmap );

	InitMapObject( g3Dmap );

	g3Dmap->datID = datID;
	g3Dmap->texID = texID;
	g3Dmap->attrID = attrID;

	g3Dmap->ldst.seq = LOAD_START;
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

//------------------------------------------------------------------
/**
 * @brief	���[�h�X�e�[�^�X�|�C���^�擾
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_GetLoadStatusPointer( GFL_G3D_MAP* g3Dmap, GFL_G3D_MAP_LOAD_STATUS** ldst )
{
	GF_ASSERT( g3Dmap );

	*ldst = &g3Dmap->ldst;
}

//------------------------------------------------------------------
/**
 * @brief	���[�h�������|�C���^�擾
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_GetLoadMemoryPointer( GFL_G3D_MAP* g3Dmap, void** mem )
{
	GF_ASSERT( g3Dmap );

	*mem = g3Dmap->mapData;
}

//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�n���h���擾
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_GetLoadArcHandle( GFL_G3D_MAP* g3Dmap, ARCHANDLE** handle )
{
	GF_ASSERT( g3Dmap );

	*handle = g3Dmap->arc;
}

//------------------------------------------------------------------
/**
 * @brief	���[�h�f�[�^�h�c�擾
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_GetLoadDatID( GFL_G3D_MAP* g3Dmap, u32* ID )
{
	GF_ASSERT( g3Dmap );

	*ID = g3Dmap->datID;
}

void GFL_G3D_MAP_GetLoadDatIDMdl( GFL_G3D_MAP* g3Dmap, u32* ID )	//��
{
	GF_ASSERT( g3Dmap );

	*ID = g3Dmap->datID;
}

void GFL_G3D_MAP_GetLoadDatIDTex( GFL_G3D_MAP* g3Dmap, u32* ID )	//��
{
	GF_ASSERT( g3Dmap );

	*ID = g3Dmap->texID;
}

void GFL_G3D_MAP_GetLoadDatIDAttr( GFL_G3D_MAP* g3Dmap, u32* ID )	//��
{
	GF_ASSERT( g3Dmap );

	*ID = g3Dmap->attrID;
}

//------------------------------------------------------------------
/**
 * @brief	���f�����\�[�X�ݒ�
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_CreateResourceMdl( GFL_G3D_MAP* g3Dmap, void* mem )
{
	GF_ASSERT( g3Dmap );

	GFL_G3D_CreateResource( g3Dmap->groundResMdl, GFL_G3D_RES_CHKTYPE_MDL, mem );
}

void GFL_G3D_MAP_DeleteResourceMdl( GFL_G3D_MAP* g3Dmap )
{
	GF_ASSERT( g3Dmap );

	GFL_G3D_CreateResource( g3Dmap->groundResMdl, GFL_G3D_RES_CHKTYPE_MDL, NULL );
}

//------------------------------------------------------------------
/**
 * @brief	�e�N�X�`�����\�[�X�ݒ�
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_CreateResourceTex( GFL_G3D_MAP* g3Dmap, void* mem )
{
	GF_ASSERT( g3Dmap );

	GFL_G3D_CreateResource( g3Dmap->groundResTex, GFL_G3D_RES_CHKTYPE_TEX, mem );
}

void GFL_G3D_MAP_DeleteResourceTex( GFL_G3D_MAP* g3Dmap )
{
	GF_ASSERT( g3Dmap );

	GFL_G3D_CreateResource( g3Dmap->groundResTex, GFL_G3D_RES_CHKTYPE_TEX, NULL );
}

//------------------------------------------------------------------
/**
 * @brief	�A�g���r���[�g���\�[�X�ݒ�
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_CreateResourceAttr( GFL_G3D_MAP* g3Dmap, void* mem )
{
	GF_ASSERT( g3Dmap );

	g3Dmap->attr = mem;
}

void GFL_G3D_MAP_DeleteResourceAttr( GFL_G3D_MAP* g3Dmap )
{
	GF_ASSERT( g3Dmap );

	g3Dmap->attr = NULL;
}

//------------------------------------------------------------------
/**
 * @brief	�����_�[�쐬
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_MakeRenderObj( GFL_G3D_MAP* g3Dmap )
{
	GF_ASSERT( g3Dmap );

	if( (GFL_G3D_GetResourceFileHeader( g3Dmap->groundResTex ) == NULL )
		&&( g3Dmap->grobalResTex != NULL )){
		MakeMapRender( g3Dmap->NNSrnd, g3Dmap->groundResMdl, g3Dmap->grobalResTex );
	} else {
		MakeMapRender( g3Dmap->NNSrnd, g3Dmap->groundResMdl, g3Dmap->groundResTex );
	}
}

//------------------------------------------------------------------
/**
 * @brief	���[�h�X�e�[�^�X������
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_ResetLoadStatus( GFL_G3D_MAP* g3Dmap )
{
	GF_ASSERT( g3Dmap );

	g3Dmap->ldst.mOffs = 0;
	g3Dmap->ldst.tOffs = 0;
	g3Dmap->ldst.mdlLoaded = FALSE;
	g3Dmap->ldst.texLoaded = FALSE;
	g3Dmap->ldst.attrLoaded = FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�t�@�C���ǂݍ��݊֐�
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_StartFileLoad( GFL_G3D_MAP* g3Dmap, u32 datID )
{
	void* dst = (void*)((u32)g3Dmap->mapData + g3Dmap->ldst.mOffs);

	GF_ASSERT( (g3Dmap->ldst.mOffs + MAPLOAD_SIZE) <= g3Dmap->mapDataHeapSize );

	//���[�h�J�n
	g3Dmap->ldst.fSize = GFL_ARC_GetDataSizeByHandle( g3Dmap->arc, datID );
	g3Dmap->ldst.fOffs = 0;

	GFL_ARC_LoadDataOfsByHandle( g3Dmap->arc, datID, 0, MAPLOAD_SIZE, dst ); 

	g3Dmap->ldst.mOffs += MAPLOAD_SIZE;
	g3Dmap->ldst.fOffs += MAPLOAD_SIZE;
}

BOOL GFL_G3D_MAP_ContinueFileLoad( GFL_G3D_MAP* g3Dmap )
{
	void*	dst = (void*)((u32)g3Dmap->mapData + g3Dmap->ldst.mOffs);
	u32		rest;

	if( g3Dmap->ldst.fSize < g3Dmap->ldst.fOffs){
		return FALSE;
	}
	rest = g3Dmap->ldst.fSize - g3Dmap->ldst.fOffs;

	if( rest > MAPLOAD_SIZE ){ 
		GF_ASSERT( (g3Dmap->ldst.mOffs + MAPLOAD_SIZE) <= g3Dmap->mapDataHeapSize );

		//���[�h�p��
		GFL_ARC_LoadDataByHandleContinue( g3Dmap->arc, MAPLOAD_SIZE, dst ); 

		g3Dmap->ldst.mOffs += MAPLOAD_SIZE;
		g3Dmap->ldst.fOffs += MAPLOAD_SIZE;
		return TRUE;
	}
	GF_ASSERT( (g3Dmap->ldst.mOffs + rest) <= g3Dmap->mapDataHeapSize );

	//�ŏI���[�h
	GFL_ARC_LoadDataByHandleContinue( g3Dmap->arc, rest, dst );

	g3Dmap->ldst.mOffs += rest;
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	���[�J���e�N�X�`��VRAM�]���֐�
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_SetTransVramParam( GFL_G3D_MAP* g3Dmap )
{
	if( g3Dmap->texVramSize == 0 ){
		return;
	}
	//�e�N�X�`�����\�[�X�ւ̂u�q�`�l�L�[�̐ݒ�
	NNS_G3dTexSetTexKey( GFL_G3D_GetResTex(g3Dmap->groundResTex), g3Dmap->groundTexKey, 0 );
	NNS_G3dPlttSetPlttKey( GFL_G3D_GetResTex(g3Dmap->groundResTex), g3Dmap->groundPlttKey );

	g3Dmap->ldst.tSize = g3Dmap->texVramSize;	//�e�N�X�`���]���ʐݒ�
	g3Dmap->ldst.tOffs = 0;
}

BOOL GFL_G3D_MAP_TransVram( GFL_G3D_MAP* g3Dmap )
{
	void*	src;
	u32		dst;
	u32		rest;

	if( g3Dmap->texVramSize == 0 ){
		return FALSE;
	}
	if( g3Dmap->ldst.tOffs >= g3Dmap->ldst.tSize ){
		src = (void*)GFL_G3D_GetAdrsTexturePltt(g3Dmap->groundResTex);
		dst = NNS_GfdGetPlttKeyAddr( g3Dmap->groundPlttKey );

		NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_PLTT, dst, src, MAPPLTT_SIZE );
		return FALSE;
	}
	src = (void*)(GFL_G3D_GetAdrsTextureData(g3Dmap->groundResTex) + g3Dmap->ldst.tOffs );
	dst = NNS_GfdGetTexKeyAddr( g3Dmap->groundTexKey ) + g3Dmap->ldst.tOffs;
	rest = g3Dmap->ldst.tSize - g3Dmap->ldst.tOffs;

	if( rest > MAPTRANS_SIZE){
		//�]���p��
		NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, dst, src, MAPTRANS_SIZE );
		g3Dmap->ldst.tOffs += MAPTRANS_SIZE;
	} else {
		//�ŏI�]��
		NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, dst, src, rest );
		g3Dmap->ldst.tOffs = g3Dmap->ldst.tSize;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�t�@�C�����ʐݒ�i���j��������̓f�[�^�t�@�C���̒��Ɏ��ʂ𖄂ߍ���
 */
//------------------------------------------------------------------
void GFL_G3D_MAP_ResistFileType( GFL_G3D_MAP* g3Dmap, GFL_G3D_MAPDATA_FILETYPE fileType )
{
	GF_ASSERT( g3Dmap );

	g3Dmap->fileType = fileType;
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
	MAP_DDOBJ_DATA*			ddObject;
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
	GFL_G3D_MAP_FILEATTR_FUNC*	attrFunc;
	VecFx32 posInBlock;
	fx32	map_height;

	GF_ASSERT( g3Dmap );

	if(( g3Dmap->attr == NULL )||( g3Dmap->ldst.attrLoaded == FALSE )){
		VEC_Fx16Set( &attrInfo->vecN, 0, 0, 0 );
		attrInfo->height = 0;
		attrInfo->attr = 0;
		return;
	}
	map_height = g3Dmap->trans.y;

	//�u���b�N�����擾
	{
		VecFx32 posBlock, mapTopOffs;

		VEC_Set( &mapTopOffs, -map_width/2, -map_height, -map_width/2 );
		VEC_Add( &g3Dmap->trans, &mapTopOffs, &posBlock );
		VEC_Subtract( pos, &posBlock, &posInBlock );
	}
	attrFunc = g3Dmap->mapFileFunc[g3Dmap->fileType].attrFunc;

	if( attrFunc != NULL ){
		attrFunc( attrInfo, g3Dmap->attr, &posInBlock, map_width, map_height );
	}
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

	g3Dmap->ldst.seq = LOAD_IDLING;
	g3Dmap->datID = 0;
	g3Dmap->texID = NON_TEX;
	g3Dmap->attrID = NON_ATTR;

	InitMapObject( g3Dmap );
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�����_�[�쐬
 */
//------------------------------------------------------------------
static void MakeMapRender( NNSG3dRenderObj* rndObj, GFL_G3D_RES* mdl, GFL_G3D_RES* tex )
{
	NNSG3dResFileHeader* mdlFH = GFL_G3D_GetResourceFileHeader( mdl );
	NNSG3dResFileHeader* texFH = GFL_G3D_GetResourceFileHeader( tex );
	NNSG3dResMdl* pMdl = NULL;
	NNSG3dResTex* pTex = NULL;

	//���f�����e�N�X�`�����\�[�X�|�C���^�擾
	pMdl = NNS_G3dGetMdlByIdx( NNS_G3dGetMdlSet( mdlFH ), 0 );	//0th mdl
	GF_ASSERT(pMdl);
	if( texFH != NULL ){
		pTex = NNS_G3dGetTex( texFH );
	}

	//���f���ƃe�N�X�`�����\�[�X�Ƃ̊֘A�t��
	if( pTex ){
		NNS_G3dBindMdlTex( pMdl, pTex );
		NNS_G3dBindMdlPltt( pMdl, pTex );
	}
	//�����_�����O�I�u�W�F�N�g������
	NNS_G3dRenderObjInit( rndObj, pMdl );
}

static void TrashMapRender( NNSG3dRenderObj* rndObj )
{
	rndObj->resMdl = NULL; 
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





//============================================================================================
/**
 *
 *
 *
 * @brief	�e�X�g�֐�
 *
 *
 *
 */
//============================================================================================
#include "system\gfl_use.h"	//�����p

void GFL_G3D_MAP_MakeTestPos( GFL_G3D_MAP* g3Dmap )
{
	int i;
	fx32 cOffs, rOffs, gWidth, mapWidth;
	VecFx32 pWorld;
	GFL_G3D_MAP_ATTRINFO attrInfo;

	if( g3Dmap->trans.y == 0 ){

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
}


