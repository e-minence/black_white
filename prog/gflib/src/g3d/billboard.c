//============================================================================================
/**
 * @file	billboard.c
 * @brief	�r���{�[�h�V�X�e��
 */
//============================================================================================
#include "gflib.h"

//���jGFL_G3D_SYSTEM ��ł̓���

//------------------------------------------------------------------
/**
 *
 * @brief	�^�錾
 *
 */
//------------------------------------------------------------------
#define RES_NULL		(0)
#define OBJ_NULL		(0xffff)

typedef struct {
	GFL_G3D_RES*	g3DresTex;			//GFL���\�[�X�Ǘ��n���h���i�e�N�X�`���p�j
	u16				texSizX;			//�e�N�X�`���f�[�^�T�C�YX
	u16				texSizY;			//�e�N�X�`���f�[�^�T�C�YY
	u8				celSizX;			//�Z���f�[�^�T�C�YX
	u8				celSizY;			//�Z���f�[�^�T�C�YY
	u8				celCountH;			//�e�N�X�`���f�[�^�㉡���C���ɂ�����̃Z����
	u8				celCountV;			//�e�N�X�`���f�[�^��c���C���ɂ�����̃Z����
	u32				texDataAdrs;		//�e�N�X�`���f�[�^�i�[VRAM�A�h���X
	u32				texPlttAdrs;		//�e�N�X�`���p���b�g�i�[VRAM�A�h���X
	GXTexFmt		texFmt;				//SDK�p�f�[�^�t�H�[�}�b�g��`
	GXTexSizeS		s;					//SDK�p�f�[�^�T�C�YX��`
	GXTexSizeT		t;					//SDK�p�f�[�^�T�C�YY��`
}BILLBOARD_RES;

typedef struct {
	u16		resIdx;
	VecFx32	trans;
	u16		celIdx;
	fx16	sizX;
	fx16	sizY;
	u8		alpha;
	u8		drawEnable:1;
	u8		flipS:1;
	u8		flipT:1;
	u8		lightMask:4;
}BILLBOARD_OBJ;

struct _GFL_BBD_SYS {
	HEAPID			heapID;
	BILLBOARD_RES*	res;
	BILLBOARD_OBJ*	obj;
	GFL_BBD_SETUP	setup;
};

//------------------------------------------------------------------
//SDK�p�f�[�^�t�H�[�}�b�g��`�@�ϊ��e�[�u���iGFL_BBD_TEXFMT �ɑΉ��j
static const GXTexFmt GX_fmtTbl[] = {
	GX_TEXFMT_PLTT16,
	GX_TEXFMT_PLTT256,
	GX_TEXFMT_PLTT4,
};

//SDK�p�f�[�^�T�C�Y��`�@�ϊ��e�[�u���iGFL_BBD_TEXSIZ �ɑΉ��j
typedef struct {
	GXTexSizeS	s;
	GXTexSizeT	t;
	u16			sizX;
	u16			sizY;
}GX_SIZ_TBL;

static const GX_SIZ_TBL GX_sizTbl[] = {
	{ GX_TEXSIZE_S8,	GX_TEXSIZE_T8,		8,		8		}, 
	{ GX_TEXSIZE_S16,	GX_TEXSIZE_T8,		16,		8		}, 
	{ GX_TEXSIZE_S16,	GX_TEXSIZE_T16,		16,		16		}, 
	{ GX_TEXSIZE_S32,	GX_TEXSIZE_T16,		32,		16		}, 
	{ GX_TEXSIZE_S32,	GX_TEXSIZE_T32,		32,		32		}, 
	{ GX_TEXSIZE_S64,	GX_TEXSIZE_T32,		64,		32		}, 
	{ GX_TEXSIZE_S64,	GX_TEXSIZE_T64,		64,		64		}, 
	{ GX_TEXSIZE_S128,	GX_TEXSIZE_T64,		128,	64		}, 
	{ GX_TEXSIZE_S128,	GX_TEXSIZE_T128,	128,	128		}, 
	{ GX_TEXSIZE_S256,	GX_TEXSIZE_T128,	256,	128		}, 
	{ GX_TEXSIZE_S256,	GX_TEXSIZE_T256,	256,	256		}, 
	{ GX_TEXSIZE_S512,	GX_TEXSIZE_T256,	512,	256		}, 
	{ GX_TEXSIZE_S512,	GX_TEXSIZE_T512,	512,	512		}, 
	{ GX_TEXSIZE_S1024,	GX_TEXSIZE_T512,	1024,	512		}, 
	{ GX_TEXSIZE_S1024,	GX_TEXSIZE_T1024,	1024,	1024	}, 
	{ GX_TEXSIZE_S32,	GX_TEXSIZE_T512,	32,		512		}, 
	{ GX_TEXSIZE_S32,	GX_TEXSIZE_T1024,	32,		1024	}, 
};

//------------------------------------------------------------------
/**
 *
 * @brief	�Z�b�g�A�b�v
 *
 */
//------------------------------------------------------------------
const GFL_BBD_SETUP defaultSetup = {
	64, 256, 
	{ FX32_ONE, FX32_ONE, FX32_ONE },
	GX_RGB(31, 31, 31),
	GX_RGB(16, 16, 16),
	GX_RGB(16, 16, 16), 
	GX_RGB(0, 0, 0),
	63,
};

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�V�X�e���쐬
 */
//------------------------------------------------------------------
GFL_BBD_SYS*	GFL_BBD_CreateSys( const GFL_BBD_SETUP* setup, HEAPID heapID )
{
	GFL_BBD_SYS* billboardSys = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_BBD_SYS) );
	int	i;

	billboardSys->heapID = heapID;
	billboardSys->setup = *setup;
	billboardSys->res = GFL_HEAP_AllocClearMemory
				( heapID, sizeof(BILLBOARD_RES) * billboardSys->setup.resCountMax );
	billboardSys->obj = GFL_HEAP_AllocClearMemory
				( heapID, sizeof(BILLBOARD_OBJ) * billboardSys->setup.objCountMax );

	for( i=0; i<billboardSys->setup.resCountMax; i++ ){
		billboardSys->res[i].g3DresTex = RES_NULL;
	}
	for( i=0; i<billboardSys->setup.objCountMax; i++ ){
		billboardSys->obj[i].resIdx	= OBJ_NULL;
	}
	return billboardSys;
}

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�V�X�e���j��
 */
//------------------------------------------------------------------
void	GFL_BBD_DeleteSys( GFL_BBD_SYS* billboardSys )
{
	GFL_BBD_RemoveResourceAll( billboardSys );	//�c���\�[�X�j��

	GFL_HEAP_FreeMemory( billboardSys->obj );
	GFL_HEAP_FreeMemory( billboardSys->res );
	GFL_HEAP_FreeMemory( billboardSys );
}

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�V�X�e���e��p�����[�^�̎擾�ƕύX
 */
//------------------------------------------------------------------
//Scale
void	GFL_BBD_GetScale( GFL_BBD_SYS* billboardSys, VecFx32* scale )
{
	*scale = billboardSys->setup.scale;
}

void	GFL_BBD_SetScale( GFL_BBD_SYS* billboardSys, VecFx32* scale )
{
	billboardSys->setup.scale = *scale;
}

//Diffuse
void	GFL_BBD_GetDiffuse( GFL_BBD_SYS* billboardSys, GXRgb* diffuse )
{
	*diffuse = billboardSys->setup.diffuse;
}

void	GFL_BBD_SetDiffuse( GFL_BBD_SYS* billboardSys, GXRgb* diffuse )
{
	billboardSys->setup.diffuse = *diffuse;
}

//Ambient
void	GFL_BBD_GetAmbient( GFL_BBD_SYS* billboardSys, GXRgb* ambient )
{
	*ambient = billboardSys->setup.ambient;
}

void	GFL_BBD_SetAmbient( GFL_BBD_SYS* billboardSys, GXRgb* ambient )
{
	billboardSys->setup.ambient = *ambient;
}

//Specular
void	GFL_BBD_GetSpecular( GFL_BBD_SYS* billboardSys, GXRgb* specular )
{
	*specular = billboardSys->setup.specular;
}

void	GFL_BBD_SetSpecular( GFL_BBD_SYS* billboardSys, GXRgb* specular )
{
	billboardSys->setup.specular = *specular;
}

//Emission
void	GFL_BBD_GetEmission( GFL_BBD_SYS* billboardSys, GXRgb* emission )
{
	*emission = billboardSys->setup.emission;
}

void	GFL_BBD_SetEmission( GFL_BBD_SYS* billboardSys, GXRgb* emission )
{
	billboardSys->setup.emission = *emission;
}

//Emission
void	GFL_BBD_GetPolID( GFL_BBD_SYS* billboardSys, u8* polID )
{
	*polID = billboardSys->setup.polID;
}

void	GFL_BBD_SetPolID( GFL_BBD_SYS* billboardSys, u8* polID )
{
	billboardSys->setup.polID = *polID;
}

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h���\�[�X�ǉ�
 */
//------------------------------------------------------------------
static BOOL getResourceIdx( GFL_BBD_SYS* billboardSys, int* resIdx )
{
	int	i;

	for( i=0; i<billboardSys->setup.resCountMax; i++ ){

		if( billboardSys->res[i].g3DresTex == RES_NULL ){
			*resIdx = i;
			return TRUE;
		}
	}
	OS_Printf("billboard texture resource overflow");
	*resIdx = 0;
	return FALSE;
}

static void setResourceTex( BILLBOARD_RES* res, GFL_G3D_RES* g3DresTex )
{
	NNSG3dTexKey	texDataKey;
	NNSG3dPlttKey	texPlttKey;

	res->g3DresTex = g3DresTex;

	if( GFL_G3D_TransVramTexture( g3DresTex ) == FALSE ){
		OS_Printf("cannnot transVRAM billboard texture");
		res->texDataAdrs = 0;
		res->texPlttAdrs = 0;
		return;
	}
	texDataKey = GFL_G3D_GetTextureDataVramKey( g3DresTex );
	texPlttKey = GFL_G3D_GetTexturePlttVramKey( g3DresTex );

	res->texDataAdrs = NNS_GfdGetTexKeyAddr( texDataKey );
	res->texPlttAdrs = NNS_GfdGetPlttKeyAddr( texPlttKey );
}

static void setResourceFmt( BILLBOARD_RES* res, u8 texFmt, u8 texSiz, u8 celSizX, u8 celSizY ) 
{
	GFL_BBD_TEXFMT fmt = (GFL_BBD_TEXFMT)texFmt;
	GFL_BBD_TEXSIZ siz = (GFL_BBD_TEXSIZ)texSiz;

	GF_ASSERT( fmt < GFL_BBD_TEXFMT_ERROR );
	GF_ASSERT( siz < GFL_BBD_TEXSIZ_ERROR );

	res->texFmt = GX_fmtTbl[fmt];

	res->s = GX_sizTbl[siz].s;
	res->t = GX_sizTbl[siz].t;
	res->texSizX = GX_sizTbl[siz].sizX;
	res->texSizY = GX_sizTbl[siz].sizY;

	res->celSizX = celSizX;
	res->celSizY = celSizY;

	res->celCountH = res->texSizX / res->celSizX;
	res->celCountV = res->texSizY / res->celSizY;
}

//------------------------------------------------------------------
// �A�[�J�C�u�h�c�ɂ��ǂݍ���
int GFL_BBD_AddResourceArc( GFL_BBD_SYS* billboardSys, int arcID, int datID,
								u8 texFmt, u8 texSiz, u8 celSizX, u8 celSizY ) 
{
	int	resIdx;

	if( getResourceIdx( billboardSys, &resIdx ) == TRUE ){
		GFL_G3D_RES* g3DresTex = GFL_G3D_CreateResourceArc( arcID, datID ); 
		setResourceTex( &billboardSys->res[resIdx], g3DresTex );
		setResourceFmt( &billboardSys->res[resIdx], texFmt, texSiz, celSizX, celSizY );
	} else {
		GF_ASSERT(0);
	}
	return resIdx;
}

//-------------------------------
// �A�[�J�C�u�t�@�C���p�X�ɂ��ǂݍ���
int GFL_BBD_AddResourcePath( GFL_BBD_SYS* billboardSys, const char* path, int datID, 
								u8 texFmt, u8 texSiz, u8 celSizX, u8 celSizY ) 
{
	int	resIdx;

	if( getResourceIdx( billboardSys, &resIdx ) == TRUE ){
		GFL_G3D_RES* g3DresTex = GFL_G3D_CreateResourcePath( path, datID ); 
		setResourceTex( &billboardSys->res[resIdx], g3DresTex );
		setResourceFmt( &billboardSys->res[resIdx], texFmt, texSiz, celSizX, celSizY );
	} else {
		GF_ASSERT(0);
	}
	return resIdx;
}

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h���\�[�X�f�[�^�|�C���^�擾
 */
//------------------------------------------------------------------
u32		GFL_BBD_GetResourceData( GFL_BBD_SYS* billboardSys, int resIdx )
{
	GFL_G3D_RES*			g3DresTex = billboardSys->res[resIdx].g3DresTex;
	NNSG3dResFileHeader*	file = GFL_G3D_GetResourceFileHeader( g3DresTex ); 
	NNSG3dResTex*			texfile = GFL_G3D_GetResTex( g3DresTex );
	u32						texData = (u32)((u8*)texfile + texfile->texInfo.ofsTex);

	return texData;
}

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h���\�[�X�f�[�^�J�b�g
 */
//------------------------------------------------------------------
void	GFL_BBD_CutResourceData( GFL_BBD_SYS* billboardSys, int resIdx )
{
	GFL_G3D_RES*			g3DresTex = billboardSys->res[resIdx].g3DresTex;
	NNSG3dResFileHeader*	file = GFL_G3D_GetResourceFileHeader( g3DresTex ); 
	NNSG3dResTex*			texfile = GFL_G3D_GetResTex( g3DresTex );
	u32						texData = (u32)((u8*)texfile + texfile->texInfo.ofsTex);
	u32						dataSize = texData - (u32)file;

	GFL_HEAP_ResizeMemory( (void*)file, dataSize );
}

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h���\�[�X�j��
 */
//------------------------------------------------------------------
// ���\�[�X�h�c�w�ɂ��j��
void	GFL_BBD_RemoveResource( GFL_BBD_SYS* billboardSys, int resIdx )
{
	GF_ASSERT( resIdx < billboardSys->setup.resCountMax );

	if( billboardSys->res[resIdx].g3DresTex != RES_NULL ){
		GFL_G3D_FreeVramTexture( billboardSys->res[resIdx].g3DresTex );
		GFL_G3D_DeleteResource( billboardSys->res[resIdx].g3DresTex );
		billboardSys->res[resIdx].g3DresTex = RES_NULL;
	}
}	

//-------------------------------
// �S�j��
void	GFL_BBD_RemoveResourceAll( GFL_BBD_SYS* billboardSys )
{
	int i;

	for( i=0; i<billboardSys->setup.resCountMax; i++ ){
		GFL_BBD_RemoveResource( billboardSys, i );
	}
}

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h���\�[�X�e��p�����[�^�̎擾
 */
//------------------------------------------------------------------
//texDataAdrs
void	GFL_BBD_GetResourceTexDataAdrs( GFL_BBD_SYS* billboardSys, int resIdx, u32* texDataAdrs )
{
	GF_ASSERT( resIdx < billboardSys->setup.resCountMax );

	*texDataAdrs = billboardSys->res[resIdx].texDataAdrs;
}

//texPlttAdrs
void	GFL_BBD_GetResourceTexPlttAdrs( GFL_BBD_SYS* billboardSys, int resIdx, u32* texPlttAdrs )
{
	GF_ASSERT( resIdx < billboardSys->setup.resCountMax );

	*texPlttAdrs = billboardSys->res[resIdx].texPlttAdrs;
}

//cel�v�Z
void	GFL_BBD_GetResourceCelOffset( GFL_BBD_SYS* billboardSys, int resIdx, 
								const u16 celIdx, u32* dataOffs, u32* celDataSiz, u8 type )
{
	BILLBOARD_RES*	res;

	GF_ASSERT( resIdx < billboardSys->setup.resCountMax );

	res = &billboardSys->res[resIdx];

	switch( res->texFmt ){
	case GX_TEXFMT_PLTT16:
		*celDataSiz = 0x20 * res->celSizX/8 * res->celSizY/8;
		break;
	case GX_TEXFMT_PLTT256:
		*celDataSiz = 0x40 * res->celSizX/8 * res->celSizY/8;
		break;
	case GX_TEXFMT_PLTT4:
		*celDataSiz = 0x10 * res->celSizX/8 * res->celSizY/8;
		break;
	}
	if( type == CEL_OFFS_1D ){
		*dataOffs = celIdx * *celDataSiz;
	} else {
		*dataOffs = ( (celIdx/res->texSizX)*(*celDataSiz) ) + (celIdx%res->texSizX)*res->celSizX;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�I�u�W�F�N�g�ǉ�
 */
//------------------------------------------------------------------
int		GFL_BBD_AddObject
		( GFL_BBD_SYS* billboardSys, int resIdx, const fx16 sizX, const fx16 sizY, 
			const VecFx32* trans, const u8 alpha, const GFL_BBD_LIGHTMASK lightMask )
{
	int objIdx;
	GF_ASSERT_MSG( sizX < FX16_ONE*8, "billboard sizX over" );	//	X�T�C�Y�I�[�o�[
	GF_ASSERT_MSG( sizY < FX16_ONE*8, "billboard sizY over" );	//	Y�T�C�Y�I�[�o�[

	for( objIdx=0; objIdx<billboardSys->setup.objCountMax; objIdx++ ){
		if( billboardSys->obj[objIdx].resIdx == OBJ_NULL ){
			billboardSys->obj[objIdx].resIdx = resIdx;
			billboardSys->obj[objIdx].sizX = sizX;
			billboardSys->obj[objIdx].sizY = sizY;
			billboardSys->obj[objIdx].trans = *trans;
			billboardSys->obj[objIdx].alpha = alpha;
			billboardSys->obj[objIdx].drawEnable = 0;
			billboardSys->obj[objIdx].lightMask = lightMask;
			billboardSys->obj[objIdx].flipS = 0;
			billboardSys->obj[objIdx].flipT = 0;
			billboardSys->obj[objIdx].celIdx = 0; 
			return objIdx;
		}
	}
	GF_ASSERT(0);
	return 0;
}	

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�I�u�W�F�N�g�j��
 */
//------------------------------------------------------------------
void	GFL_BBD_RemoveObject( GFL_BBD_SYS* billboardSys, int objIdx )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	if( billboardSys->obj[objIdx].resIdx != OBJ_NULL ){
		billboardSys->obj[objIdx].resIdx = OBJ_NULL;
	}
}	

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�I�u�W�F�N�g�e��p�����[�^�̎擾�ƕύX
 */
//------------------------------------------------------------------
//resIdx
void	GFL_BBD_GetObjectResIdx
			( GFL_BBD_SYS* billboardSys, int objIdx, u16* resIdx )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );
	GF_ASSERT( *resIdx < billboardSys->setup.resCountMax );

	*resIdx = billboardSys->obj[objIdx].resIdx;
}

void	GFL_BBD_SetObjectResIdx
			( GFL_BBD_SYS* billboardSys, int objIdx, const u16* resIdx )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );
	GF_ASSERT( *resIdx < billboardSys->setup.resCountMax );

	billboardSys->obj[objIdx].resIdx = *resIdx;
}

//trans
void	GFL_BBD_GetObjectTrans
			( GFL_BBD_SYS* billboardSys, int objIdx, VecFx32* trans )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	*trans = billboardSys->obj[objIdx].trans;
}

void	GFL_BBD_SetObjectTrans
			( GFL_BBD_SYS* billboardSys, int objIdx, const VecFx32* trans )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	billboardSys->obj[objIdx].trans = *trans;
}

//celIdx
void	GFL_BBD_GetObjectCelIdx
			( GFL_BBD_SYS* billboardSys, int objIdx, u16* celIdx )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	*celIdx = billboardSys->obj[objIdx].celIdx;
}

void	GFL_BBD_SetObjectCelIdx
			( GFL_BBD_SYS* billboardSys, int objIdx, const u16* celIdx )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	billboardSys->obj[objIdx].celIdx = *celIdx;
}

//siz
void	GFL_BBD_GetObjectSiz
			( GFL_BBD_SYS* billboardSys, int objIdx, fx16* sizX, fx16* sizY )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	*sizX = billboardSys->obj[objIdx].sizX;
	*sizY = billboardSys->obj[objIdx].sizY;
}

void	GFL_BBD_SetObjectSiz
			( GFL_BBD_SYS* billboardSys, int objIdx, const fx16* sizX, const fx16* sizY )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	billboardSys->obj[objIdx].sizX = *sizX;
	billboardSys->obj[objIdx].sizY = *sizY;
}

//alpha
void	GFL_BBD_GetObjectAlpha
			( GFL_BBD_SYS* billboardSys, int objIdx, u8* alpha )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	*alpha = billboardSys->obj[objIdx].alpha;
}

void	GFL_BBD_SetObjectAlpha
			( GFL_BBD_SYS* billboardSys, int objIdx, const u8* alpha )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	billboardSys->obj[objIdx].alpha = *alpha;
}

//drawEnable
void	GFL_BBD_SetObjectDrawEnable
			( GFL_BBD_SYS* billboardSys, int objIdx, const BOOL* drawEnable )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	if( *drawEnable == TRUE ){
		billboardSys->obj[objIdx].drawEnable = 1;
	} else {
		billboardSys->obj[objIdx].drawEnable = 0;
	}
}

//lightMask
void	GFL_BBD_SetObjectLightMask
			( GFL_BBD_SYS* billboardSys, int objIdx, const GFL_BBD_LIGHTMASK* lightMask )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	billboardSys->obj[objIdx].lightMask = *lightMask;
}

//flipS
void	GFL_BBD_SetObjectFlipS
			( GFL_BBD_SYS* billboardSys, int objIdx, const BOOL* flipS )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	billboardSys->obj[objIdx].flipS = *flipS;
}

//flipT
void	GFL_BBD_SetObjectFlipT
			( GFL_BBD_SYS* billboardSys, int objIdx, const BOOL* flipT )
{
	GF_ASSERT( objIdx < billboardSys->setup.objCountMax );

	billboardSys->obj[objIdx].flipT = *flipT;
}

//------------------------------------------------------------------
/**
 *
 * @brief	�r���{�[�h�`��
 *
 */
//------------------------------------------------------------------
void	GFL_BBD_Draw
		( GFL_BBD_SYS* billboardSys, GFL_G3D_CAMERA* g3Dcamera, GFL_G3D_LIGHTSET* g3Dlightset )
{
	GFL_BBD_SETUP*	p_setup = &billboardSys->setup;
	BILLBOARD_RES*		res;
	BILLBOARD_OBJ*		obj;
	MtxFx33				mtxBillboard;
	VecFx16				vecN;
	fx32				s0, t0, s1, t1, tmp;
	int					objIdx, resIdx;

	G3X_Reset();

	//�J�����ݒ�擾
	{
		VecFx32		camPos, camUp, target, vecNtmp;
		MtxFx43		mtxCamera, mtxCameraInv;

		GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
		GFL_G3D_CAMERA_GetCamUp( g3Dcamera, &camUp );
		GFL_G3D_CAMERA_GetTarget( g3Dcamera, &target );

		G3_LookAt( &camPos, &camUp, &target, &mtxCamera );	//mtxCamera�ɂ͍s��v�Z���ʂ��Ԃ�
		MTX_Inverse43( &mtxCamera, &mtxCameraInv );			//�J�����t�s��擾
		MTX_Copy43To33( &mtxCameraInv, &mtxBillboard );		//�J�����t�s�񂩂��]�s������o��

		VEC_Subtract( &camPos, &target, &vecNtmp );
		VEC_Normalize( &vecNtmp, &vecNtmp );
		VEC_Fx16Set( &vecN, vecNtmp.x, vecNtmp.y, vecNtmp.z );
	}
	GFL_G3D_LIGHT_Switching( g3Dlightset );

	//�O���[�o���X�P�[���ݒ�
	G3_Scale( p_setup->scale.x, p_setup->scale.y, p_setup->scale.z );

	for( objIdx=0; objIdx<p_setup->objCountMax; objIdx++ ){
		obj = &billboardSys->obj[objIdx];

		if(( obj->resIdx != OBJ_NULL )&&( obj->drawEnable )){
			res = &billboardSys->res[obj->resIdx];

			if( obj->flipS ){
				//S���]
				s1 = ( obj->celIdx % res->celCountH * res->celSizX ) * FX32_ONE;
				s0 = s1 + ( res->celSizX * FX32_ONE );
			} else {
				//S�ʏ�
				s0 = ( obj->celIdx % res->celCountH * res->celSizX ) * FX32_ONE;
				s1 = s0 + ( res->celSizX * FX32_ONE );
			}
			if( obj->flipT ){
				//T���]
				t1 = ( obj->celIdx / res->celCountH * res->celSizY ) * FX32_ONE;
				t0 = t1 + ( res->celSizY * FX32_ONE );
			} else {
				//T�ʏ�
				t0 = ( obj->celIdx / res->celCountH * res->celSizY ) * FX32_ONE;
				t1 = t0 + ( res->celSizY * FX32_ONE );
			}

			G3_PushMtx();

			//��]�A���s�ړ��p�����[�^�ݒ�
			G3_MultTransMtx33( &mtxBillboard, &obj->trans );

			G3_TexImageParam(	res->texFmt, GX_TEXGEN_TEXCOORD, res->s, res->t,
								GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
								GX_TEXPLTTCOLOR0_TRNS, res->texDataAdrs );
			G3_TexPlttBase( res->texPlttAdrs, res->texFmt );

			//�}�e���A���ݒ�
			G3_MaterialColorDiffAmb( p_setup->diffuse, p_setup->ambient, TRUE );
			G3_MaterialColorSpecEmi( p_setup->specular, p_setup->emission, FALSE );
			G3_PolygonAttr(		obj->lightMask, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
								p_setup->polID, obj->alpha, 0 );
	
			G3_Begin( GX_BEGIN_QUADS );

			//���ʃ|���S���Ȃ̂Ŗ@���x�N�g����4���_�ŋ��p
			if( obj->lightMask ){
				G3_Normal( vecN.x, vecN.y, vecN.z );
			} else {
				G3_Color( GX_RGB( 31, 31, 31 ) );
			}
			G3_TexCoord( s0, t0 );
			G3_Vtx( -obj->sizX, obj->sizY, 0 );

			G3_TexCoord( s0, t1 );
			G3_Vtx( -obj->sizX, -obj->sizY, 0 );

			G3_TexCoord( s1, t1 );
			G3_Vtx( obj->sizX, -obj->sizY, 0 );

			G3_TexCoord( s1, t0 );
			G3_Vtx( obj->sizX, obj->sizY, 0 );

			G3_End();
			G3_PopMtx(1);
		}
	}
	//�����G3D_System�ōs���̂ŁA�����ł͂��Ȃ�
	//G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

