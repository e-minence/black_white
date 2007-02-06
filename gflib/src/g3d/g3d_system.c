//=============================================================================================
/**
 * @file	g3d_system.c                                                  
 * @brief	�R�c�`��Ǘ��V�X�e���v���O����
 * @date	2006/4/26
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
#define TEX_BLOCKNUM	(128)
#define PLT_BLOCKNUM	(256)

#define TEX_SLOTSIZ		(0x20000)
#define PLT_SLOTSIZ		(0x2000)

//=============================================================================================
//	�^�錾
//=============================================================================================
//	�O���[�o���X�e�[�g�\���́i�ˉe�j
typedef struct {
	GFL_G3D_PROJECTION_TYPE	type;	///<�ˉe�s��^�C�v
	fx32		param1;				///<�p�����[�^�P�i�^�C�v�ɂ���Ďg�p�@�͈قȂ�j
	fx32		param2;				///<�p�����[�^�Q�i�^�C�v�ɂ���Ďg�p�@�͈قȂ�j
	fx32		param3;				///<�p�����[�^�R�i�^�C�v�ɂ���Ďg�p�@�͈قȂ�j
	fx32		param4;				///<�p�����[�^�S�i�^�C�v�ɂ���Ďg�p�@�͈قȂ�j
	fx32		near;				///<���_����near�N���b�v�ʂ܂ł̋���
	fx32		far;				///<���_����far�N���b�v�ʂ܂ł̋���
	fx32		scaleW;				///<�r���[�{�����[���̐��x�����p�����[�^�i�g�p���Ȃ��Ƃ���0�j
}GFL_G3D_PROJECTION;

//	�O���[�o���X�e�[�g�\���́i���C�g�j
typedef struct {
	VecFx16		vec;				///<���C�g����
	u16			color;				///<�F
}GFL_G3D_LIGHT;

//	�O���[�o���X�e�[�g�\���́i�J�����j
typedef struct {
	VecFx32		camPos;		///<�J�����̈ʒu(�����_)
	VecFx32		camUp;		///<�J�����̏����
	VecFx32		target;		///<�J�����̏œ_(�������_)
} GFL_G3D_LOOKAT;

//	�O���[�o���X�e�[�g�\���́i�����_�����O�o�b�t�@�j
typedef struct {
	GXSortMode		aw;
	GXBufferMode	zw;
} GFL_G3D_SWAPBUFMODE;

/**
 * @brief	�R�c�V�X�e���}�l�[�W���\����
 */
typedef struct GFL_G3D_MAN_tag
{
	NNSFndAllocator		allocater;			///<�������A���P�[�^(NNS�Ŏg�p)
	void*				plt_memory;			///<�p���b�g�}�l�[�W���p������
	void*				tex_memory;			///<�e�N�X�`���}�l�[�W���p������
	HEAPID				heapID;				///<�q�[�v�h�c

	GFL_G3D_PROJECTION	projection;			///<�O���[�o���X�e�[�g�i�ˉe�j
	GFL_G3D_LIGHT		light[4];			///<�O���[�o���X�e�[�g�i���C�g�j
	GFL_G3D_LOOKAT		lookAt;				///<�O���[�o���X�e�[�g�i�J�����j
	GFL_G3D_SWAPBUFMODE	swapBufMode;		///<�O���[�o���X�e�[�g�i�����_�����O�o�b�t�@�j

}GFL_G3D_MAN;

static GFL_G3D_MAN*  g3Dman = NULL;

static inline BOOL G3DMAN_CHECK( void ){
	if( g3Dman == NULL ){
		return FALSE;
	}
	return TRUE;
}

//=============================================================================================
/**
 *
 *
 * �V�X�e���}�l�[�W���֐�
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �V�X�e���}�l�[�W���ݒ�
 *
 * @param	texmanMode		�e�N�X�`���}�l�[�W���g�p���[�h
 * @param	texmanSize		�e�N�X�`���}�l�[�W���g�p�̈�T�C�Y
 * @param	palmanMode		�p���b�g�}�l�[�W���g�p���[�h
 * @param	palmanSize		�p���b�g�}�l�[�W���g�p�̈�T�C�Y
 * @param	GeBufEnable		�W�I���g���o�b�t�@�̎g�p�T�C�Y(0:�g�p���Ȃ�)
 * @param	heapID			�������擾�p�q�[�v�G���A
 * @param	setUp			�Z�b�g�A�b�v�֐�(NULL�̎���DefaultSetUp)
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysInit
		( GFL_G3D_VMAN_MODE texmanMode, GFL_G3D_VMAN_TEXSIZE texmanSize, 
			GFL_G3D_VMAN_MODE pltmanMode, GFL_G3D_VMAN_PLTSIZE pltmanSize,
				u16 GeBufSize, HEAPID heapID, GFL_G3D_SETUP_FUNC setup )
{
	int	tex_size,plt_size;
	g3Dman = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_G3D_MAN) );

	g3Dman->heapID = heapID;

	// NitroSystem:�R�c�G���W���̏�����
	NNS_G3dInit();
	// �}�g���N�X�X�^�b�N�̏�����
    G3X_InitMtxStack();
	// �O���[�o���X�e�[�g������
	NNS_G3dGlbInit();

	// �W�I���g���R�}���h�o�b�t�@�̐ݒ�
	if( GeBufSize ){
		NNSG3dGeBuffer* geBuf;

		if( GeBufSize > GEBUF_SIZE_MAX ){
			GeBufSize = GEBUF_SIZE_MAX;
		}
		geBuf = ( NNSG3dGeBuffer* )GFL_HEAP_DTCM_AllocMemory( GeBufSize );
		NNS_G3dGeSetBuffer( geBuf );
	}
	// �W�I���g���G���W���N����K���Ă΂�Ȃ���΂Ȃ�Ȃ�
    G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );

	//�m�m�r�֐��Ŏg�p����A���P�[�^�̃Z�b�g�A�b�v
	GFL_HEAP_InitAllocator( &g3Dman->allocater, g3Dman->heapID, 4 ); 

	//�u�q�`�l�e�N�X�`���}�l�[�W���̐ݒ�
	if( texmanMode == GFL_G3D_VMANLNK ){
		//�����N�h���[�h�̐ݒ�
		tex_size = NNS_GfdGetLnkTexVramManagerWorkSize( TEX_BLOCKNUM * texmanSize );
		g3Dman->tex_memory = GFL_HEAP_AllocMemory( g3Dman->heapID, tex_size );

		// �}�l�[�W�����e�N�X�`���C���[�W�X���b�g���w��X���b�g���Ǘ��ł���悤�ɂ���
		NNS_GfdInitLnkTexVramManager
			( TEX_SLOTSIZ*texmanSize, 0, g3Dman->tex_memory, tex_size, TRUE);
	} else {
		//�t���[�����[�h�̐ݒ�
		NNS_GfdInitFrmTexVramManager( texmanSize, TRUE);
		g3Dman->tex_memory = NULL;
	}

	//�u�q�`�l�p���b�g�}�l�[�W���̐ݒ�
	if( pltmanMode == GFL_G3D_VMANLNK ){
		//�����N�h���[�h�̐ݒ�
		plt_size = NNS_GfdGetLnkPlttVramManagerWorkSize( PLT_BLOCKNUM * pltmanSize );
		g3Dman->plt_memory = GFL_HEAP_AllocMemory( g3Dman->heapID, plt_size );

		// �}�l�[�W�����e�N�X�`���C���[�W�X���b�g���w��T�C�Y���Ǘ��ł���悤�ɂ���
		NNS_GfdInitLnkPlttVramManager
			( PLT_SLOTSIZ * pltmanSize, g3Dman->plt_memory, plt_size, TRUE);
	} else {
		//�t���[�����[�h�̐ݒ�
		NNS_GfdInitFrmTexVramManager( PLT_SLOTSIZ * pltmanSize, TRUE);
		g3Dman->tex_memory = NULL;
	}

	//�O���[�o���X�e�[�g������񏉊���
	{
		VecFx32 initVec32 = { 0, 0, 0 };
		VecFx16 initVec16 = { FX32_ONE-1, FX32_ONE-1, FX32_ONE-1 };

		//�ˉe
		GFL_G3D_sysProjectionSet( GFL_G3D_PRJPERS,
						FX_SinIdx( 0x1000 ), FX_CosIdx( 0x1000 ), ( FX32_ONE * 4 / 3 ), 0, 
						( 1 << FX32_SHIFT ), ( 1024 << FX32_SHIFT ), 0 );
		//���C�g
		GFL_G3D_sysLightSet( 0, &initVec16, 0x7fff );
		GFL_G3D_sysLightSet( 1, &initVec16, 0x7fff );
		GFL_G3D_sysLightSet( 2, &initVec16, 0x7fff );
		GFL_G3D_sysLightSet( 3, &initVec16, 0x7fff );
		//�J����
		GFL_G3D_sysLookAtSet( &initVec32, &initVec32, &initVec32 );
		g3Dman->lookAt.camPos.z	= ( 256 << FX32_SHIFT );
		g3Dman->lookAt.camUp.y	= FX32_ONE;
		//�����_�����O�X���b�v�o�b�t�@
		GFL_G3D_sysSwapBufferModeSet( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
	}

	if( setup != NULL ){
		setup();
	} else {	//�e�탂�[�h�f�t�H���g�Z�b�g�A�b�v
		// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
		G3X_SetShading( GX_SHADING_TOON );
		G3X_AntiAlias( FALSE );
		G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
		G3X_AlphaBlend( FALSE );		// �A���t�@�u�����h�@�I��
		G3X_EdgeMarking( FALSE );
		G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

		// �N���A�J���[�̐ݒ�
		G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

		// �r���[�|�[�g�̐ݒ�
		G3_ViewPort(0, 0, 255, 191);
	}
}

//--------------------------------------------------------------------------------------------
/**
 *
 * �}�l�[�W���I��
 *
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysExit
		( void )
{
	if( G3DMAN_CHECK() == FALSE ){
		OS_Printf("g3D_system_maneger is nothing(GFL_G3D_sysExit)\n");
	} else {
		//�u�q�`�l�p���b�g�}�l�[�W���̉��
		if( g3Dman->plt_memory != NULL ){
			GFL_HEAP_FreeMemory( g3Dman->plt_memory );
		}
		//�u�q�`�l�e�N�X�`���}�l�[�W���̉��
		if( g3Dman->tex_memory != NULL ){
			GFL_HEAP_FreeMemory( g3Dman->tex_memory );
		}
		// �W�I���g���R�}���h�o�b�t�@�̉��
		if( NNS_G3dGeIsBufferExist() == TRUE ){
			NNSG3dGeBuffer* geBuf = NNS_G3dGeReleaseBuffer();
			GFL_HEAP_DTCM_FreeMemory( geBuf );
		}
		GFL_HEAP_FreeMemory( g3Dman );
	}
}

//--------------------------------------------------------------------------------------------
/**
 *
 * �����ێ����i�O���[�o���X�e�[�g�j�ւ̃A�N�Z�X
 *
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 * �ˉe�s��̐ݒ�
 *	�ۑ����Ă����K�v�����邩��킩��Ȃ����A�Ƃ肠�����B
 *	���ڎˉe�s��𒼐ڐݒ肷��ꍇ�A���̍s��͊O���Ŏ���
 *
 * @param	type		�ˉe�^�C�v
 * @param	param1		PRJPERS			��fovySin :�c(Y)�����̎��E�p�x(��p)/2�̐������Ƃ����l
 *						PRJPERS,PRJORTH	��top	  :near�N���b�v�ʏ�ӂ�Y���W
 * @param	param2		PRJPERS			��fovyCos :�c(Y)�����̎��E�p�x(��p)/2�̗]�����Ƃ����l	
 *						PRJPERS,PRJORTH	��bottom  :near�N���b�v�ʉ��ӂ�Y���W
 * @param	param3		PRJPERS			��aspect  :�c�ɑ΂��鎋�E�̊���(�c����F���E�ł̕��^����)
 *						PRJPERS,PRJORTH	��left	  :near�N���b�v�ʍ��ӂ�X���W
 * @param	param4		PRJPERS			�����g�p 
 *						PRJPERS,PRJORTH	��right	  :near�N���b�v�ʉE�ӂ�X���W
 * @param	near		���_����near�N���b�v�ʂ܂ł̋���	
 * @param	far			���_����far�N���b�v�ʂ܂ł̋���	
 * @param	scaleW		�r���[�{�����[���̐��x�����p�����[�^�i�g�p���Ȃ��Ƃ���0�j
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysProjectionSet
		( GFL_G3D_PROJECTION_TYPE type, fx32 param1, fx32 param2, fx32 param3, fx32 param4, 
			fx32 near, fx32 far, fx32 scaleW )
{
	g3Dman->projection.type		= type;
	g3Dman->projection.param1	= param1;
	g3Dman->projection.param2	= param2;
	g3Dman->projection.param3	= param3;
	g3Dman->projection.param4	= param4;
	g3Dman->projection.near		= near;
	g3Dman->projection.far		= far;
	g3Dman->projection.scaleW	= scaleW;

	switch( type ){
		case GFL_G3D_PRJPERS:	// �����ˉe��ݒ�
			if( !scaleW ){
				NNS_G3dGlbPerspective( param1, param2, param3, near, far );
			} else {
				NNS_G3dGlbPerspectiveW( param1, param2, param3, near, far, scaleW );
			}
			break;
		case GFL_G3D_PRJFRST:	// �����ˉe��ݒ�
			if( !scaleW ){
				NNS_G3dGlbFrustum( param1, param2, param3, param4, near, far );
			} else {
				NNS_G3dGlbFrustumW( param1, param2, param3, param4, near, far, scaleW );
			}
			break;
		case GFL_G3D_PRJORTH:	// ���ˉe��ݒ�
			if( !scaleW ){
				NNS_G3dGlbOrtho( param1, param2, param3, param4, near, far );
			} else {
				NNS_G3dGlbOrthoW( param1, param2, param3, param4, near, far, scaleW );
			}
			break;
	}
}

void
	GFL_G3D_sysProjectionSetDirect
		( MtxFx44* param )
{
	g3Dman->projection.type		= GFL_G3D_PRJMTX;
	g3Dman->projection.param1	= 0;
	g3Dman->projection.param2	= 0;
	g3Dman->projection.param3	= 0;
	g3Dman->projection.param4	= 0;
	g3Dman->projection.near		= 0;
	g3Dman->projection.far		= 0;

	NNS_G3dGlbSetProjectionMtx( param );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�̐ݒ�
 *
 * @param	lightID			���C�g�h�c
 * @param	vec				���C�g�̃x�N�g���|�C���^
 * @param	color			�F
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysLightSet
		( u8 lightID, VecFx16* vec, u16 color )
{
	g3Dman->light[ lightID ].vec.x = vec->x;
	g3Dman->light[ lightID ].vec.y = vec->y;
	g3Dman->light[ lightID ].vec.z = vec->z;
	g3Dman->light[ lightID ].color = color;

	NNS_G3dGlbLightVector( lightID, vec->x, vec->y, vec->z );
	NNS_G3dGlbLightColor( lightID, color );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����s��̐ݒ�
 *
 * @param	camPos			�J�����ʒu�x�N�g���|�C���^
 * @param	camUp			�J�����̏�����̃x�N�g���ւ̃|�C���^
 * @param	target			�J�����œ_�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysLookAtSet
		( VecFx32* camPos, VecFx32* camUp, VecFx32* target )
{
	g3Dman->lookAt.camPos	= *camPos;
	g3Dman->lookAt.camUp	= *camUp;
	g3Dman->lookAt.target	= *target;
}

//--------------------------------------------------------------------------------------------
/**
 * �����_�����O�X���b�v�o�b�t�@�̐ݒ�
 *
 * @param	sortMode		�\�[�g���[�h
 * @param	bufferMode		�f�v�X�o�b�t�@���[�h
 * �ݒ�l�ɂ��Ă�NitroSDK��GX_SwapBuffer�֐����Q��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysSwapBufferModeSet
		( GXSortMode sortMode, GXBufferMode bufferMode )
{
	g3Dman->swapBufMode.aw = sortMode;
	g3Dman->swapBufMode.zw = bufferMode;
}





//=============================================================================================
/**
 *
 *
 * �R�c���\�[�X�Ǘ��֐�
 *
 *
 */
//=============================================================================================
#define G3DRES_MAGICNUM	(0x48BF)

enum {
	GFL_G3D_RES_TYPE_UNKNOWN = 0,	//�s���ȃf�[�^
	GFL_G3D_RES_TYPE_MDLTEX,		//�t�@�C�����Ƀ��f�����O����уe�N�X�`���f�[�^����
	GFL_G3D_RES_TYPE_MDL,			//�t�@�C�����Ƀ��f�����O�f�[�^����
	GFL_G3D_RES_TYPE_TEX,			//�t�@�C�����Ƀe�N�X�`���f�[�^����
	GFL_G3D_RES_TYPE_ANM,			//�t�@�C�����ɃA�j���[�V�����f�[�^����
};

///	�R�c���\�[�X�\����
struct _GFL_G3D_RES {
	u16		magicnum;
	u16		type;
	void* 	file;
};

static BOOL
	GFL_G3D_GetTexDataVramkey( NNSG3dResTex* res, NNSGfdTexKey* tex, NNSGfdTexKey* tex4x4 );
static BOOL
	GFL_G3D_GetTexPlttVramkey( NNSG3dResTex* res, NNSGfdPlttKey* pltt );

static inline BOOL G3DRES_FILE_CHECK( GFL_G3D_RES* g3Dres )
{
	if( g3Dres->magicnum == G3DRES_MAGICNUM ){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c���\�[�X�̓ǂݍ���
 *
 * @param	arcID or path	�A�[�J�C�u�h�c or �t�@�C���p�X
 * @param	datID			�f�[�^�h�c
 *
 * @return	g3Dres	�R�c���\�[�X�|�C���^(���s=NULL)
 */
//--------------------------------------------------------------------------------------------
static GFL_G3D_RES*
	GFL_G3D_ResourceCreate
		( NNSG3dResFileHeader* header )
{
	//���\�[�X�Ǘ��n���h���쐬
	GFL_G3D_RES* g3Dres = GFL_HEAP_AllocMemory( g3Dman->heapID, sizeof(GFL_G3D_RES) );

	//OS_Printf("3D_resource check now...\n");
	//�t�@�C���^�C�v�̔���
	switch( *(u32*)&header[0] )
	{
		case NNS_G3D_SIGNATURE_NSBMD:
			//OS_Printf("nsbmd file check...\n");
			if( NNS_G3dGetTex( header ) == NULL ){
				//OS_Printf("this 3D_resource is model_data\n");
				g3Dres->type = GFL_G3D_RES_TYPE_MDL;	//���f�����O�f�[�^����
			} else {
				//OS_Printf("this 3D_resource is model_data & texture_data\n");
				g3Dres->type = GFL_G3D_RES_TYPE_MDLTEX;	//���f�����O����уe�N�X�`���f�[�^����
			}
			break;
		case NNS_G3D_SIGNATURE_NSBTX:
			//OS_Printf("this 3D_resource is texture_data\n");
			g3Dres->type = GFL_G3D_RES_TYPE_TEX;		//�e�N�X�`���f�[�^����
			break;
		case NNS_G3D_SIGNATURE_NSBCA:
		case NNS_G3D_SIGNATURE_NSBVA:
		case NNS_G3D_SIGNATURE_NSBMA:
		case NNS_G3D_SIGNATURE_NSBTP:
		case NNS_G3D_SIGNATURE_NSBTA:
			//OS_Printf("this 3D_resource is animetion_data\n");
			g3Dres->type = GFL_G3D_RES_TYPE_ANM;		//�t�@�C�����ɃA�j���[�V�����f�[�^����
			break;
		default:
			//OS_Printf("this 3D_resource is unknown\n");
			g3Dres->type = GFL_G3D_RES_TYPE_UNKNOWN;	//�s���ȃf�[�^
			break;
	}
	g3Dres->magicnum = G3DRES_MAGICNUM;
	//�t�@�C���|�C���^�̐ݒ�
	g3Dres->file = ( void* )header;
	//OS_Printf("3D_resource is loaded\n");

	return g3Dres;
}

//-------------------------------
// �A�[�J�C�u�h�c�ɂ��ǂݍ���
GFL_G3D_RES*
	GFL_G3D_ResourceCreateArc
		( int arcID, int datID ) 
{
	NNSG3dResFileHeader* header;

	//�V�X�e���`�F�b�N
	if( G3DMAN_CHECK() == FALSE ){
		OS_Panic("please setup 3D_system_manager (GFL_G3D_ResourceCreateArc)\n");
		return NULL;
	}
	//OS_Printf("3D_resource loading...\n");
	//�ΏۃA�[�J�C�u�h�m�c�d�w����w�b�_�f�[�^��ǂݍ���
	header = GFL_ARC_DataLoadMalloc( arcID, datID, g3Dman->heapID );

	return GFL_G3D_ResourceCreate( header );
}

//-------------------------------
// �A�[�J�C�u�t�@�C���p�X�ɂ��ǂݍ���
GFL_G3D_RES*
	GFL_G3D_ResourceCreatePath
		( const char* path, int datID ) 
{
	NNSG3dResFileHeader* header;

	//�V�X�e���`�F�b�N
	if( G3DMAN_CHECK() == FALSE ){
		OS_Panic("please setup 3D_system_manager (GFL_G3D_ResourceCreateArc)\n");
		return NULL;
	}
	//OS_Printf("3D_resource loading...\n");
	//�ΏۃA�[�J�C�u�t�@�C������w�b�_�f�[�^��ǂݍ���
	header = GFL_ARC_DataLoadFilePathMalloc( path, datID, g3Dman->heapID );

	return GFL_G3D_ResourceCreate( header );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c���\�[�X�̔j��
 *
 * @param	g3Dres	�R�c���\�[�X�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ResourceDelete
		( GFL_G3D_RES* g3Dres ) 
{
	if( G3DRES_FILE_CHECK( g3Dres ) == FALSE ){
		OS_Printf("file is not 3D_resource (GFL_G3D_ResourceDelete)\n");
		return;
	}
	GFL_HEAP_FreeMemory( g3Dres->file );
	GFL_HEAP_FreeMemory( g3Dres );
}
	
//--------------------------------------------------------------------------------------------
/**
 * �R�c���\�[�X�^�C�v�̊m�F
 *
 * @param	g3Dres		�R�c���\�[�X�|�C���^
 * @param	checkType	�`�F�b�N����^�C�v
 *
 * @return	BOOL		TRUE�ň�v
 */
//--------------------------------------------------------------------------------------------
BOOL
	GFL_G3D_ResourceTypeCheck
		( GFL_G3D_RES* g3Dres, GFL_G3D_RES_CHKTYPE checkType ) 
{
	u16 resType;

	if( G3DRES_FILE_CHECK( g3Dres ) == FALSE ){
		OS_Printf("file is not 3D_resource (GFL_G3D_ResourceTypeCheck)\n");
		return FALSE;
	}
	resType = g3Dres->type;

	switch( checkType ){
		case GFL_G3D_RES_CHKTYPE_UKN:
			if( resType == GFL_G3D_RES_TYPE_UNKNOWN ){
				return TRUE;
			} else {
				return FALSE;
			}
			break;

		case GFL_G3D_RES_CHKTYPE_MDL:
			if(( resType == GFL_G3D_RES_TYPE_MDLTEX )||( resType == GFL_G3D_RES_TYPE_MDL )){
				return TRUE;
			} else {
				return FALSE;
			}
			break;

		case GFL_G3D_RES_CHKTYPE_TEX:
			if(( resType == GFL_G3D_RES_TYPE_MDLTEX )||( resType == GFL_G3D_RES_TYPE_TEX )){
				return TRUE;
			} else {
				return FALSE;
			}
			break;

		case GFL_G3D_RES_CHKTYPE_ANM:
			if( resType == GFL_G3D_RES_TYPE_ANM ){
				return TRUE;
			} else {
				return FALSE;
			}
			break;
	}
	return FALSE;
}
	




//=============================================================================================
/**
 *
 *
 * �R�c���\�[�X�]���֐��i������e�N�X�`���f�[�^�A�e�N�X�`���p���b�g�̂݁j
 *
 *
 */
//=============================================================================================
static inline BOOL G3DRES_FILE_CHECK_EQU_TEX( GFL_G3D_RES* g3Dres )
{
	if( g3Dres->magicnum == G3DRES_MAGICNUM ){
		if(( g3Dres->type == GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type == GFL_G3D_RES_TYPE_TEX )){
			return TRUE;
		}
	}
	OS_Panic("file is not texture_resource (GFL_G3D_VramLoadTex)\n");
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �e�N�X�`�����\�[�X�̂u�q�`�l�̈�m�ۂƓ]��
 *
 * @param	g3Dres	�R�c���\�[�X�|�C���^
 *
 * @return	BOOL	����(����=TRUE)
 */
//--------------------------------------------------------------------------------------------
BOOL 
	GFL_G3D_VramLoadTex
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;
	NNSGfdPlttKey			plttKey;

	G3DRES_FILE_CHECK_EQU_TEX( g3Dres );

	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//���\�[�X��]�����邽�߂̂u�q�`�l�L�[�̎擾
		if( GFL_G3D_GetTexDataVramkey( texture, &texKey, &tex4x4Key ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadTex)\n");
			return FALSE;
		}
		if( GFL_G3D_GetTexPlttVramkey( texture, &plttKey ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadTex)\n");
			return FALSE;
		}
		//���\�[�X�ւ̂u�q�`�l�L�[�̐ݒ�
		NNS_G3dTexSetTexKey( texture, texKey, tex4x4Key );
		NNS_G3dPlttSetPlttKey( texture, plttKey );

		//�����łc�l�`�]������̂œ]�������t���b�V������
		DC_FlushRange( header, header->fileSize );
		//�L�[���Q�Ƃ��āA���\�[�X���u�q�`�l�]��
		NNS_G3dTexLoad( texture, TRUE );
		NNS_G3dPlttLoad( texture, TRUE );
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �e�N�X�`�����\�[�X���u�q�`�l������
 *
 * @param	g3Dres	�R�c���\�[�X�|�C���^
 *
 * @return	BOOL	����(����=TRUE)
 */
//--------------------------------------------------------------------------------------------
BOOL 
	GFL_G3D_VramUnloadTex
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;
	NNSGfdPlttKey			plttKey;

	G3DRES_FILE_CHECK_EQU_TEX( g3Dres );

	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	//�u�q�`�l�L�[�̕����t���O�����\�[�X�ɐݒ�
	NNS_G3dTexReleaseTexKey( texture, &texKey, &tex4x4Key );
	plttKey = NNS_G3dPlttReleasePlttKey( texture );

	if( NNS_GfdFreePlttVram( plttKey ) ){
		OS_Printf("Vramkey cannot free (GFL_G3D_VramUnloadTex)\n");
	}
	if( NNS_GfdFreeLnkTexVram( tex4x4Key )){
		OS_Printf("Vramkey cannot free (GFL_G3D_VramUnloadTex)\n");
	}
	if( NNS_GfdFreeLnkTexVram( texKey )){
		OS_Printf("Vramkey cannot free (GFL_G3D_VramUnloadTex)\n");
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �e�N�X�`���f�[�^���\�[�X�̂u�q�`�l�̈�m�ۂƓ]��
 *
 * @param	g3Dres	�R�c���\�[�X�|�C���^
 *
 * @return	BOOL	����(����=TRUE)
 *
 * �A�����ăe�N�X�`����ǂݍ��ޏꍇ�Ɍ����I�Ɏg�p���邽�߂̊֐�
 * �A���ǂݍ��݂̍ۂ́A�O����
 *    �J�n����GX_BeginLoadTex�֐�
 *    �I������GX_EndLoadTex�֐�
 * ���ĂԕK�v������
 */
//--------------------------------------------------------------------------------------------
BOOL 
	GFL_G3D_VramLoadTexDataOnly
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;

	G3DRES_FILE_CHECK_EQU_TEX( g3Dres );

	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//���\�[�X��]�����邽�߂̂u�q�`�l�L�[�̎擾
		if( GFL_G3D_GetTexDataVramkey( texture, &texKey, &tex4x4Key ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadTexDataOnly)\n");
			return FALSE;
		}
		//���\�[�X�ւ̂u�q�`�l�L�[�̐ݒ�
		NNS_G3dTexSetTexKey( texture, texKey, tex4x4Key );

		//�����łc�l�`�]������̂œ]�������t���b�V������
		DC_FlushRange( header, header->fileSize );
		//�L�[���Q�Ƃ��āA���\�[�X���u�q�`�l�]��
		NNS_G3dTexLoad( texture, FALSE );
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �e�N�X�`���p���b�g���\�[�X�̂u�q�`�l�̈�m�ۂƓ]��
 *
 * @param	g3Dres	�R�c���\�[�X�|�C���^
 *
 * @return	BOOL	����(����=TRUE)
 *
 * �A�����ăp���b�g��ǂݍ��ޏꍇ�Ɍ����I�Ɏg�p���邽�߂̊֐�
 * �A���ǂݍ��݂̍ۂ́A�O����
 *    �J�n����GX_BeginLoadPltt�֐�
 *    �I������GX_EndLoadPltt�֐�
 * ���ĂԕK�v������
 */
//--------------------------------------------------------------------------------------------
BOOL 
	GFL_G3D_VramLoadTexPlttOnly
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdPlttKey			plttKey;

	G3DRES_FILE_CHECK_EQU_TEX( g3Dres );

	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//���\�[�X��]�����邽�߂̂u�q�`�l�L�[�̎擾
		if( GFL_G3D_GetTexPlttVramkey( texture, &plttKey ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadPlttOnly)\n");
			return FALSE;
		}
		//���\�[�X�ւ̂u�q�`�l�L�[�̐ݒ�
		NNS_G3dPlttSetPlttKey( texture, plttKey );

		//�����łc�l�`�]������̂œ]�������t���b�V������
		DC_FlushRange( header, header->fileSize );
		//�L�[���Q�Ƃ��āA���\�[�X���u�q�`�l�]��
		NNS_G3dPlttLoad( texture, FALSE );
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *
 *@brief	�e�N�X�`���L�[�̎擾
 *
 *	@param	res		�e�N�X�`�����\�[�X�Q�ƃ|�C���^
 *	@param	tex		�e�N�X�`���L�[�ۑ��|�C���^
 *	@param	tex4x4	4x4���k�e�N�X�`���ۑ��|�C���^
 *	@param	pltt	�p���b�g�L�[�ۑ��|�C���^
 */
//-----------------------------------------------------------------------------
static BOOL
	GFL_G3D_GetTexDataVramkey
		( NNSG3dResTex* res, NNSGfdTexKey* tex, NNSGfdTexKey* tex4x4 )
{
	//�e���\�[�X�T�C�Y�擾
	u32 tex_size		= NNS_G3dTexGetRequiredSize( res );
	u32 tex_4x4_size	= NNS_G3dTex4x4GetRequiredSize( res );

	//�L�[�擾(�u�q�`�l�m��)
	if( tex_size != 0 ){
		*tex = NNS_GfdAllocTexVram( tex_size, FALSE, 0 );
		if( *tex == NNS_GFD_ALLOC_ERROR_TEXKEY ){
			return FALSE;
		}
	}
	if( tex_4x4_size != 0 ){
		*tex4x4 = NNS_GfdAllocTexVram( tex_size, TRUE, 0 );
		if( *tex4x4 == NNS_GFD_ALLOC_ERROR_TEXKEY ){
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *
 *@brief	�p���b�g�L�[�̎擾
 *
 *	@param	res		�e�N�X�`�����\�[�X�Q�ƃ|�C���^
 *	@param	pltt	�p���b�g�L�[�ۑ��|�C���^
 */
//-----------------------------------------------------------------------------
static BOOL
	GFL_G3D_GetTexPlttVramkey
		( NNSG3dResTex* res, NNSGfdPlttKey* pltt )
{
	//���\�[�X�T�C�Y�擾
	u32 pltt_size		= NNS_G3dPlttGetRequiredSize( res );

	//�L�[�擾(�u�q�`�l�m��)
	if( pltt_size != 0 ){
		BOOL b4Pltt = res->tex4x4Info.flag & NNS_G3D_RESPLTT_USEPLTT4;
		*pltt = NNS_GfdAllocPlttVram( pltt_size, b4Pltt, 0 );
		if( *pltt == NNS_GFD_ALLOC_ERROR_PLTTKEY ){
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *
 *@brief	�e�N�X�`���L�[������U���Ă��邩�`�F�b�N
 *
 *@param	g3Dobj		�R�c�I�u�W�F�N�g�n���h��
 *	
 *@return	BOOL		TRUE:����U���Ă�AFALSE:����U���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL
	GFL_G3D_ObjTexkeyLiveCheck
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			restex;

	G3DRES_FILE_CHECK_EQU_TEX( g3Dres );

	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	restex = NNS_G3dGetTex( header ); 

	// ���ʂ̃e�N�X�`����4*4�e�N�Z�����k�e�N�X�`��
	// ��Vram�ɓW�J����Ă��邩���`�F�b�N
	if(( restex->texInfo.flag & NNS_G3D_RESTEX_LOADED ) ||
	   ( restex->tex4x4Info.flag & NNS_G3D_RESTEX4x4_LOADED )||
	   ( restex->plttInfo.flag & NNS_G3D_RESPLTT_LOADED )){
		return TRUE;
	}
	return FALSE;
}





//=============================================================================================
/**
 *
 *
 * �e�I�u�W�F�N�g�Ǘ��֐�
 *
 *
 */
//=============================================================================================
#define G3DOBJ_MAGICNUM	(0x7A14)
///	�R�c���f������p�\����
struct _GFL_G3D_OBJ
{
	u16						magicnum;
	u16						dummy;

	NNSG3dRenderObj*		rndobj;
	NNSG3dAnmObj*			anmobj;

	NNSG3dResMdl*			pMdl;
	NNSG3dResTex*			pTex;
	void*					pAnm;

	VecFx32					trans;
	VecFx32					scale;
	MtxFx33					rotate;
};

static inline BOOL G3DOBJ_HANDLE_CHECK( GFL_G3D_OBJ* g3Dobj )
{
	if( g3Dobj->magicnum == G3DOBJ_MAGICNUM ){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̍쐬
 *
 * @param	mdl			�Q�ƃ��f���Z�b�g���\�[�X�\���̃|�C���^
 * @param	mdlidx		mdl���f�[�^�C���f�b�N�X(�����o�^����Ă���ꍇ�B�P�̏ꍇ��0)
 * @param	tex			�Q�ƃe�N�X�`�����\�[�X�\���̃|�C���^(�g�p���Ȃ��ꍇ��NULL)
 * @param	anm			�Q�ƃA�j���[�V�������\�[�X�\���̃|�C���^(�g�p���Ȃ��ꍇ��NULL)
 * @param	anmidx		anm���f�[�^�C���f�b�N�X(�����o�^����Ă���ꍇ�B�P�̏ꍇ��0)
 *
 * @return	GFL_3D_OBJ	�R�c�I�u�W�F�N�g�n���h��
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_OBJ*
	GFL_G3D_ObjCreate
		( GFL_G3D_RES* mdl, int mdlidx, GFL_G3D_RES* tex, GFL_G3D_RES* anm, int anmidx )  
{
	NNSG3dResFileHeader*	header;
	NNSG3dResMdlSet*		pMdlset;
	NNSG3dResMdl*			pMdl;
	NNSG3dResTex*			pTex;
	void*					pAnm;
	GFL_G3D_OBJ*			g3Dobj;

	//�������`�F�b�N����ъe�탊�\�[�X�|�C���^�擾
	if(( G3DRES_FILE_CHECK( mdl ) == TRUE )&&
	   (( mdl->type == GFL_G3D_RES_TYPE_MDLTEX )||( mdl->type == GFL_G3D_RES_TYPE_MDL )) ){
		//���f���f�[�^���\�[�X�|�C���^�擾
		header = (NNSG3dResFileHeader*)mdl->file;
		pMdlset = NNS_G3dGetMdlSet( header );
		pMdl = NNS_G3dGetMdlByIdx( pMdlset, mdlidx );
	} else {
		pMdl = NULL;
	}
	if(( G3DRES_FILE_CHECK( tex ) == TRUE )&&
	   (( tex->type == GFL_G3D_RES_TYPE_MDLTEX )||( tex->type == GFL_G3D_RES_TYPE_TEX )) ){
		//�e�N�X�`�����\�[�X�|�C���^�擾
		header = (NNSG3dResFileHeader*)tex->file;
		pTex = NNS_G3dGetTex( header );
	} else {
		pTex = NULL;
	}
	if(( G3DRES_FILE_CHECK( anm ) == TRUE )&&( anm->type == GFL_G3D_RES_TYPE_ANM )){
		//�A�j���[�V�������\�[�X�|�C���^�擾
		pAnm = NNS_G3dGetAnmByIdx( anm->file, anmidx );
	} else {
		pAnm = NULL;
	}
	if(( pMdl == NULL )&&( pAnm == NULL )){
		OS_Panic("cannot cleate 3D_object (GFL_G3D_ObjCreate)\n");
		return NULL;
	}

	//�I�u�W�F�N�g�n���h���̍쐬
	g3Dobj = GFL_HEAP_AllocMemory( g3Dman->heapID, sizeof(GFL_G3D_OBJ) );
	g3Dobj->magicnum = G3DOBJ_MAGICNUM;
	g3Dobj->pMdl = pMdl;
	g3Dobj->pTex = pTex;
	g3Dobj->pAnm = pAnm;

	//�����_�����O�I�u�W�F�N�g�ݒ�
	if( pMdl ){
		//�����_�����O�I�u�W�F�N�g�̈�̊m��
		g3Dobj->rndobj = NNS_G3dAllocRenderObj( &g3Dman->allocater );
		if( g3Dobj->rndobj == NULL ){
			OS_Panic("cannot alloc rndobj (GFL_G3D_ObjCreate)\n");
			return NULL;
		}
		//�e�N�X�`�����\�[�X�Ƃ̊֘A�t��
		NNS_G3dBindMdlSet( pMdlset, pTex );
		//�����_�����O�I�u�W�F�N�g������
		NNS_G3dRenderObjInit( g3Dobj->rndobj, pMdl);
	} else {
		g3Dobj->rndobj = NULL;	//�����_�����O�Ȃ����A�j���R���g���[���I�u�W�F�N�g�ɂȂ�
	}

	//�A�j���[�V�����I�u�W�F�N�g�̈�̊m�ۂƐݒ�
	if( pAnm ){
		//�A�j���[�V�����I�u�W�F�N�g�̈�̊m��
		g3Dobj->anmobj = NNS_G3dAllocAnmObj( &g3Dman->allocater, pAnm, pMdl );
		if( g3Dobj->anmobj == NULL ){
			OS_Panic("cannot alloc anmobj (GFL_G3D_ObjCreate)\n");
			return NULL;
		}
		//�A�j���[�V�����I�u�W�F�N�g������
		NNS_G3dAnmObjInit( g3Dobj->anmobj, pAnm, pMdl, pTex );
		//�����_�����O�I�u�W�F�N�g�Ƃ̊֘A�t��
		NNS_G3dRenderObjAddAnmObj( g3Dobj->rndobj, g3Dobj->anmobj );
	} else {
		g3Dobj->anmobj = NULL;
	}

	//�I�u�W�F�N�g�X�e�[�^�X���[�N������
	{
		VecFx32 init_trans	= { 0, 0, 0 };
		VecFx32 init_scale	= { FX32_ONE, FX32_ONE, FX32_ONE };
		MtxFx33 init_rotate = { FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE };

		g3Dobj->trans	= init_trans;
		g3Dobj->scale	= init_scale;
		g3Dobj->rotate	= init_rotate;
	}
	return g3Dobj;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̔j��
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjDelete
		( GFL_G3D_OBJ* g3Dobj ) 
{
	if( G3DOBJ_HANDLE_CHECK( g3Dobj ) == FALSE ){
		OS_Printf("handle is not 3D_object (GFL_G3D_ObjDelete)\n");
		return;
	}
	if( g3Dobj->anmobj != NULL ){
		NNS_G3dFreeAnmObj( &g3Dman->allocater, g3Dobj->anmobj );
	}
	if( g3Dobj->rndobj != NULL ){
		NNS_G3dFreeRenderObj( &g3Dman->allocater, g3Dobj->rndobj );
	}
	GFL_HEAP_FreeMemory( g3Dobj );
}
	




//=============================================================================================
/**
 *
 *
 * �e�I�u�W�F�N�g�`��֐�
 *
 *	SAMPLE
 *	{
 *		GFL_G3D_DrawStart();							//�`��J�n
 *		GFL_G3D_DrawLookAt();							//�J�����O���[�o���X�e�[�g�ݒ�		
 *		{
 *			GFL_G3D_ObjDraw( g3Dobj );					//�e�I�u�W�F�N�g�`��
 *		}
 *		GFL_G3D_DrawEnd();								//�`��I���i�o�b�t�@�X���b�v�j
 *	}
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �R�c�`��̊J�n
 *
 * �S�̕`��֐����A�`��J�n���ɌĂяo�����
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_DrawStart
		( void )
{
	G3X_Reset();
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�`��̏I��
 *
 * �S�̕`��֐����A�`����I�����ɌĂяo�����
 * �����Ń����_�����O�o�b�t�@�̃X���b�v���s���B
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_DrawEnd
		( void )
{
	G3_SwapBuffers( g3Dman->swapBufMode.aw, g3Dman->swapBufMode.zw );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����O���[�o���X�e�[�g�̐ݒ�
 *
 * �S�̕`��֐����ɌĂяo�����
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_DrawLookAt
		( void )
{
	NNS_G3dGlbLookAt( &g3Dman->lookAt.camPos, &g3Dman->lookAt.camUp, &g3Dman->lookAt.target );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̕`��
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 *
 * �`Draw	:�J�����g�ˉe�s��Ɏˉe�ϊ��s�񂪁A
 *			 �J�����g�ʒu���W�s��ƕ����x�N�g���s��ɃJ�����s��ƃ��f�����O�s�񂪍������ꂽ�s��
 *			 ���ݒ肳��܂��B
 *
 * �`DrawVP	:�J�����g�ˉe�s��Ɏˉe�ϊ��s��ƃJ�����s�񂪍������ꂽ�s�񂪁A
 *			 �J�����g�ʒu���W�s��ƕ����x�N�g���s��Ƀ��f�����O�s��
 *			 ���ݒ肳��܂��B
 *
 * �`DrawWVP:�J�����g�ˉe�s��Ɏˉe�ϊ��s��ƃJ�����s��ƃ��f�����O�s�񂪍������ꂽ�s�񂪁A
 *			 �J�����g�ʒu���W�s��ƕ����x�N�g���s��ɒP�ʍs��
 *			 ���ݒ肳��܂��B
 */
//--------------------------------------------------------------------------------------------
// �R�c�I�u�W�F�N�g�̏��ݒ�
static inline void statusSet( VecFx32* pTrans, MtxFx33* pRotate, VecFx32* pScale )
{
	// �ʒu�ݒ�
	NNS_G3dGlbSetBaseTrans( pTrans );
	// �p�x�ݒ�
	NNS_G3dGlbSetBaseRot( pRotate );
	// �X�P�[���ݒ�
	NNS_G3dGlbSetBaseScale( pScale );
}

//--------------------------------------------------------------------------------
// �ʏ�`��
//--------------------------------------------------------------------------------
void
	GFL_G3D_ObjDraw
		( GFL_G3D_OBJ* g3Dobj )
{
	statusSet( &g3Dobj->trans, &g3Dobj->rotate, &g3Dobj->scale );
	NNS_G3dGlbFlush();
	NNS_G3dDraw( g3Dobj->rndobj );
	NNS_G3dGeFlushBuffer();
}

void
	GFL_G3D_ObjDrawVP
		( GFL_G3D_OBJ* g3Dobj )
{
	statusSet( &g3Dobj->trans, &g3Dobj->rotate, &g3Dobj->scale );
	NNS_G3dGlbFlushVP();
	NNS_G3dDraw( g3Dobj->rndobj );
	NNS_G3dGeFlushBuffer();
}

void
	GFL_G3D_ObjDrawWVP
		( GFL_G3D_OBJ* g3Dobj )
{
	statusSet( &g3Dobj->trans, &g3Dobj->rotate, &g3Dobj->scale );
	NNS_G3dGlbFlushWVP();
	NNS_G3dDraw( g3Dobj->rndobj );
	NNS_G3dGeFlushBuffer();
}

//--------------------------------------------------------------------------------
// �P�̃��f���ɂP�̃}�e���A���̂ݐݒ肳��Ă���Ƃ��ɍ����`�悷�邽�߂̊֐�
//		matID	�`�悷��}�e���A���ւ̃C���f�b�N�X 
//		shpID	�`�悷��V�F�C�v�ւ̃C���f�b�N�X 
//		sendMat	�}�e���A�������W�I���g���G���W���ɑ��M���邩�ǂ��� 
//--------------------------------------------------------------------------------
void
	GFL_G3D_ObjDraw1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat )

{
	statusSet( &g3Dobj->trans, &g3Dobj->rotate, &g3Dobj->scale );
	NNS_G3dGlbFlush();
	NNS_G3dDraw1Mat1Shp( g3Dobj->pMdl, matID, shpID, sendMat );
	NNS_G3dGeFlushBuffer();
}

void
	GFL_G3D_ObjDrawVP1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat )
{
	statusSet( &g3Dobj->trans, &g3Dobj->rotate, &g3Dobj->scale );
	NNS_G3dGlbFlushVP();
	NNS_G3dDraw1Mat1Shp( g3Dobj->pMdl, matID, shpID, sendMat );
	NNS_G3dGeFlushBuffer();
}

void
	GFL_G3D_ObjDrawWVP1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat )
{
	statusSet( &g3Dobj->trans, &g3Dobj->rotate, &g3Dobj->scale );
	NNS_G3dGlbFlushWVP();
	NNS_G3dDraw1Mat1Shp( g3Dobj->pMdl, matID, shpID, sendMat );
	NNS_G3dGeFlushBuffer();
}





//=============================================================================================
/**
 *
 *
 * �e�I�u�W�F�N�g�R���g���[���֐�
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �ʒu�����Z�b�g
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	trans	�Z�b�g����ʒu���
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjContSetTrans
		( GFL_G3D_OBJ* g3Dobj, const VecFx32* trans ) 
{
	if( G3DOBJ_HANDLE_CHECK( g3Dobj ) == FALSE ){
		OS_Printf("handle is not 3D_object (GFL_G3D_ObjContSetTrans)\n");
		return;
	}
	g3Dobj->trans = *trans;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�P�[�������Z�b�g
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	trans	�Z�b�g����X�P�[�����
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjContSetScale
		( GFL_G3D_OBJ* g3Dobj, const VecFx32* scale ) 
{
	if( G3DOBJ_HANDLE_CHECK( g3Dobj ) == FALSE ){
		OS_Printf("handle is not 3D_object (GFL_G3D_ObjContSetScale)\n");
		return;
	}
	g3Dobj->scale = *scale;
}

//--------------------------------------------------------------------------------------------
/**
 * ��]�����Z�b�g
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	rotate	�Z�b�g�����]���
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjContSetRotate
		( GFL_G3D_OBJ* g3Dobj, const MtxFx33* rotate ) 
{
	if( G3DOBJ_HANDLE_CHECK( g3Dobj ) == FALSE ){
		OS_Printf("handle is not 3D_object (GFL_G3D_ObjContSetRotate)\n");
		return;
	}
	g3Dobj->rotate = *rotate;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�j���[�V�����t���[�������Z�b�g
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjContAnmFrameReset
		( GFL_G3D_OBJ* g3Dobj ) 
{
	if( G3DOBJ_HANDLE_CHECK( g3Dobj ) == FALSE ){
		OS_Printf("handle is not 3D_object (GFL_G3D_ObjContAnmFrameInc)\n");
		return;
	}
	if( g3Dobj->anmobj == NULL ){
		OS_Printf("this handle is not have animetion (GFL_G3D_ObjContAnmFrameInc)\n");
		return;
	}
	g3Dobj->anmobj->frame = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�j���[�V�����t���[����i�߂�
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	count	�������iFX32_ONE�łP�t���[���i�߂�j
 *
 * @return	BOOL	FALSE�ŏI�����o
 */
//--------------------------------------------------------------------------------------------
BOOL
	GFL_G3D_ObjContAnmFrameInc
		( GFL_G3D_OBJ* g3Dobj, fx32 count ) 
{
	if( G3DOBJ_HANDLE_CHECK( g3Dobj ) == FALSE ){
		OS_Printf("handle is not 3D_object (GFL_G3D_ObjContAnmFrameInc)\n");
		return FALSE;
	}
	if( g3Dobj->anmobj == NULL ){
		OS_Printf("this handle is not have animetion (GFL_G3D_ObjContAnmFrameInc)\n");
		return FALSE;
	}
	g3Dobj->anmobj->frame += count;
	if( g3Dobj->anmobj->frame >= NNS_G3dAnmObjGetNumFrame( g3Dobj->anmobj )){
		return FALSE;
	} else {
		return TRUE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �A�j���[�V�����I�[�g���[�v
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	count	�������iFX32_ONE�łP�t���[���i�߂�j
 *
 * @return	BOOL	FALSE�łP���[�v�I�����o
 */
//--------------------------------------------------------------------------------------------
BOOL
	GFL_G3D_ObjContAnmFrameAutoLoop
		( GFL_G3D_OBJ* g3Dobj, fx32 count ) 
{
	if( G3DOBJ_HANDLE_CHECK( g3Dobj ) == FALSE ){
		OS_Printf("handle is not 3D_object (GFL_G3D_ObjContAnmFrameInc)\n");
		return FALSE;
	}
	if( g3Dobj->anmobj == NULL ){
		OS_Printf("this handle is not have animetion (GFL_G3D_ObjContAnmFrameInc)\n");
		return FALSE;
	}
	g3Dobj->anmobj->frame += count;
	if( g3Dobj->anmobj->frame >= NNS_G3dAnmObjGetNumFrame( g3Dobj->anmobj )){
		g3Dobj->anmobj->frame = 0;
		return FALSE;
	}
	return TRUE;
}





//=============================================================================================
/**
 *
 *
 * �f�[�^
 *
 *
 */
//=============================================================================================



