//=============================================================================================
/**
 * @file	g3d_system.c                                                  
 * @brief	�R�c�Ǘ��V�X�e���v���O����
 * @date	2006/4/26
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�萔�錾
//=============================================================================================
#define TEX_BLOCKNUM	(128)
#define PLT_BLOCKNUM	(256)

#define TEX_SLOTSIZ		(0x20000)
#define PLT_SLOTSIZ		(0x2000)

//=============================================================================================
//	�^�錾
//=============================================================================================
typedef void	(GFL_G3D_DRAW_FLASHFUNC)( void );

/**
 * @brief	�R�c�V�X�e���}�l�[�W���\����
 */
typedef struct GFL_G3D_MAN_tag
{
	NNSFndAllocator			allocater;			///<�������A���P�[�^(NNS�Ŏg�p)
	void*					plt_memory;			///<�p���b�g�}�l�[�W���p������
	void*					tex_memory;			///<�e�N�X�`���}�l�[�W���p������
	HEAPID					heapID;				///<�q�[�v�h�c

	GFL_G3D_PROJECTION		projection;			///<�O���[�o���X�e�[�g�i�ˉe�j
	GFL_G3D_LIGHT			light[4];			///<�O���[�o���X�e�[�g�i���C�g�j
	GFL_G3D_LOOKAT			lookAt;				///<�O���[�o���X�e�[�g�i�J�����j
	GFL_G3D_SWAPBUFMODE		swapBufMode;		///<�O���[�o���X�e�[�g�i�����_�����O�o�b�t�@�j

	GFL_G3D_DRAW_FLASHFUNC*	drawFlushFunc;		///<�`��t���b�V���^�C�v
	GFL_G3D_VMAN_MODE		texmanMode;			///<�e�N�X�`���}�l�[�W�����[�h
	GFL_G3D_VMAN_MODE		pltmanMode;			///<�p���b�g�}�l�[�W�����[�h
}GFL_G3D_MAN;

static GFL_G3D_MAN*  g3Dman = NULL;

//=============================================================================================
//=============================================================================================
/**
 *
 *
 * �V�X�e���}�l�[�W���֐�
 *
 *
 */
//=============================================================================================
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
	GFL_G3D_Init
		( GFL_G3D_VMAN_MODE texmanMode, GFL_G3D_VMAN_TEXSIZE texmanSize, 
			GFL_G3D_VMAN_MODE pltmanMode, GFL_G3D_VMAN_PLTSIZE pltmanSize,
				u16 GeBufSize, HEAPID heapID, GFL_G3D_SETUP_FUNC setup )
{
	int	tex_size,plt_size;

	GF_ASSERT( g3Dman == NULL );

	g3Dman = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_G3D_MAN) );

	g3Dman->heapID = heapID;
	g3Dman->texmanMode = texmanMode;
	g3Dman->pltmanMode = pltmanMode;

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
		{	//�ˉe
			GFL_G3D_PROJECTION initProjection = { GFL_G3D_PRJPERS, 0, 0, ( FX32_ONE * 4 / 3 ), 0, 
												( 1 << FX32_SHIFT ), ( 1024 << FX32_SHIFT ), 0 };
			initProjection.param1 = FX_SinIdx( 40/2 *PERSPWAY_COEFFICIENT ); 
			initProjection.param2 = FX_CosIdx( 40/2 *PERSPWAY_COEFFICIENT ); 
			GFL_G3D_SetSystemProjection( &initProjection );
		}
		{ //���C�g
			GFL_G3D_LIGHT initLight = { { -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, 0x7fff };
			GFL_G3D_SetSystemLight( 0, &initLight );
			GFL_G3D_SetSystemLight( 1, &initLight );
			GFL_G3D_SetSystemLight( 2, &initLight );
			GFL_G3D_SetSystemLight( 3, &initLight );
		}
		{	//�J����
			GFL_G3D_LOOKAT initCamera = {{0,0,(256<<FX32_SHIFT)},{0,FX32_ONE,0},{0,0,0}};
			GFL_G3D_SetSystemLookAt( &initCamera );
		}
		//�����_�����O�X���b�v�o�b�t�@
		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
	}
	GFL_G3D_SetDrawFlushMode( GFL_G3D_FLUSH_P );

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
	GFL_G3D_Exit
		( void )
{
	GF_ASSERT( g3Dman != NULL );

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
	g3Dman = NULL;
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
 *
 * �V�X�e���N���`�F�b�N
 *
 */
//--------------------------------------------------------------------------------------------
BOOL
	GFL_G3D_CheckBootSystem
		( void )
{
	if( g3Dman != NULL ){
		return FALSE;
	} else {
		return TRUE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �ˉe�s��̎擾�Ɛݒ�
 *	�ۑ����Ă����K�v�����邩��킩��Ȃ����A�Ƃ肠�����B
 *	���ڎˉe�s��𒼐ڐݒ肷��ꍇ�A���̍s��͊O���Ŏ���
 *
 *	type		�ˉe�^�C�v
 *	param1		PRJPERS			��fovySin :�c(Y)�����̎��E�p�x(��p)/2�̐������Ƃ����l
 *				PRJFRST,PRJORTH	��top	  :near�N���b�v�ʏ�ӂ�Y���W
 *	param2		PRJPERS			��fovyCos :�c(Y)�����̎��E�p�x(��p)/2�̗]�����Ƃ����l	
 *				PRJFRST,PRJORTH	��bottom  :near�N���b�v�ʉ��ӂ�Y���W
 *	param3		PRJPERS			��aspect  :�c�ɑ΂��鎋�E�̊���(�c����F���E�ł̕��^����)
 *				PRJFRST,PRJORTH	��left	  :near�N���b�v�ʍ��ӂ�X���W
 *	param4		PRJPERS			�����g�p 
 *				PRJFRST,PRJORTH	��right	  :near�N���b�v�ʉE�ӂ�X���W
 *	near		���_����near�N���b�v�ʂ܂ł̋���	
 *	far			���_����far�N���b�v�ʂ܂ł̋���	
 *	scaleW		�r���[�{�����[���̐��x�����p�����[�^�i�g�p���Ȃ��Ƃ���0�j
 *
 * @param	projection		�擾or�ݒ�p�ˉe�p�����[�^�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_GetSystemProjection
		( GFL_G3D_PROJECTION* projection )
{
	GF_ASSERT( g3Dman != NULL );

	projection->type	= g3Dman->projection.type;
	projection->param1	= g3Dman->projection.param1;
	projection->param2	= g3Dman->projection.param2;
	projection->param3	= g3Dman->projection.param3;
	projection->param4	= g3Dman->projection.param4;
	projection->near	= g3Dman->projection.near;
	projection->far		= g3Dman->projection.far;
	projection->scaleW	= g3Dman->projection.scaleW;
}

void
	GFL_G3D_SetSystemProjection
		( const GFL_G3D_PROJECTION* projection )
{
	GF_ASSERT( g3Dman != NULL );

	g3Dman->projection.type		= projection->type;
	g3Dman->projection.param1	= projection->param1;
	g3Dman->projection.param2	= projection->param2;
	g3Dman->projection.param3	= projection->param3;
	g3Dman->projection.param4	= projection->param4;
	g3Dman->projection.near		= projection->near;
	g3Dman->projection.far		= projection->far;
	g3Dman->projection.scaleW	= projection->scaleW;

	switch( g3Dman->projection.type ){
		case GFL_G3D_PRJPERS:	// �����ˉe��ݒ�
			if( !g3Dman->projection.scaleW ){
				NNS_G3dGlbPerspective(	g3Dman->projection.param1, g3Dman->projection.param2, 
										g3Dman->projection.param3, 
										g3Dman->projection.near, g3Dman->projection.far );
			} else {
				NNS_G3dGlbPerspectiveW( g3Dman->projection.param1, g3Dman->projection.param2, 
										g3Dman->projection.param3, 
										g3Dman->projection.near, g3Dman->projection.far, 
										g3Dman->projection.scaleW );
			}
			break;
		case GFL_G3D_PRJFRST:	// �����ˉe��ݒ�
			if( !g3Dman->projection.scaleW ){
				NNS_G3dGlbFrustum(	g3Dman->projection.param1, g3Dman->projection.param2, 
									g3Dman->projection.param3, g3Dman->projection.param4, 
									g3Dman->projection.near, g3Dman->projection.far );
			} else {
				NNS_G3dGlbFrustumW( g3Dman->projection.param1, g3Dman->projection.param2, 
									g3Dman->projection.param3, g3Dman->projection.param4, 
									g3Dman->projection.near, g3Dman->projection.far, 
									g3Dman->projection.scaleW );
			}
			break;
		case GFL_G3D_PRJORTH:	// ���ˉe��ݒ�
			if( !g3Dman->projection.scaleW ){
				NNS_G3dGlbOrtho(	g3Dman->projection.param1, g3Dman->projection.param2, 
									g3Dman->projection.param3, g3Dman->projection.param4, 
									g3Dman->projection.near, g3Dman->projection.far );
			} else {
				NNS_G3dGlbOrthoW(	g3Dman->projection.param1, g3Dman->projection.param2,
									g3Dman->projection.param3, g3Dman->projection.param4, 
									g3Dman->projection.near, g3Dman->projection.far, 
									g3Dman->projection.scaleW );
			}
			break;
	}
}

void
	GFL_G3D_GetSystemProjectionDirect
		( MtxFx44* param )
{
	*param = *(NNS_G3dGlbGetProjectionMtx());
}
void
	GFL_G3D_SetSystemProjectionDirect
		( const MtxFx44* param )
{
	NNS_G3dGlbSetProjectionMtx( param );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�̎擾�Ɛݒ�
 *
 * @param	lightID			���C�g�h�c
 * @param	light			�擾or�ݒ�p���C�g�p�����[�^�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_GetSystemLight
		( const u8 lightID, GFL_G3D_LIGHT* light )
{
	GF_ASSERT( g3Dman != NULL );

	light->vec		= g3Dman->light[ lightID ].vec;
	light->color	= g3Dman->light[ lightID ].color;
}

void
	GFL_G3D_SetSystemLight
		( const u8 lightID, const GFL_G3D_LIGHT* light )
{
	GF_ASSERT( g3Dman != NULL );

	g3Dman->light[ lightID ].vec	= light->vec;
	g3Dman->light[ lightID ].color	= light->color;

	NNS_G3dGlbLightVector(	lightID, 
							g3Dman->light[ lightID ].vec.x,
							g3Dman->light[ lightID ].vec.y,
							g3Dman->light[ lightID ].vec.z );
	NNS_G3dGlbLightColor( lightID, g3Dman->light[ lightID ].color );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����s��̎擾�Ɛݒ�
 *
 * @param	lookAt			�擾or�ݒ�p�J�����s��p�����[�^�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_GetSystemLookAt
		( GFL_G3D_LOOKAT* lookAt )
{
	GF_ASSERT( g3Dman != NULL );

	lookAt->camPos	= g3Dman->lookAt.camPos;
	lookAt->camUp	= g3Dman->lookAt.camUp;
	lookAt->target	= g3Dman->lookAt.target;
}

void
	GFL_G3D_SetSystemLookAt
		( const GFL_G3D_LOOKAT* lookAt )
{
	GF_ASSERT( g3Dman != NULL );

	g3Dman->lookAt.camPos	= lookAt->camPos;
	g3Dman->lookAt.camUp	= lookAt->camUp;
	g3Dman->lookAt.target	= lookAt->target;

	NNS_G3dGlbLookAt( &g3Dman->lookAt.camPos, &g3Dman->lookAt.camUp, &g3Dman->lookAt.target );
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
	GFL_G3D_SetSystemSwapBufferMode
		( const GXSortMode sortMode, const GXBufferMode bufferMode )
{
	GF_ASSERT( g3Dman != NULL );

	g3Dman->swapBufMode.aw = sortMode;
	g3Dman->swapBufMode.zw = bufferMode;
}

//--------------------------------------------------------------------------------------------
/**
 * �e�N�X�`������уp���b�g�}�l�[�W����Ԃ̎擾
 *
 * return GFL_G3D_VMAN_MODE		�}�l�[�W�����[�h
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_VMAN_MODE
	GFL_G3D_GetTextureManagerMode
		( void )
{
	return g3Dman->texmanMode;
}

GFL_G3D_VMAN_MODE
	GFL_G3D_GetPaletteManagerMode
		( void )
{
	return g3Dman->pltmanMode;
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�`��t���b�V���i�O���[�o����ԑ��M�j�^�C�v�̐ݒ�
 *
 * @param	type	�`��^�C�v
 *
 * GFL_G3D_FLUSH_P:		�J�����g�ˉe�s��Ɏˉe�ϊ��s�񂪁A
 *						�J�����g�ʒu���W�s��ƕ����x�N�g���s���
 *						�J�����s��ƃ��f�����O�s�񂪍������ꂽ�s�񂪐ݒ肳��܂��B
 *
 * GFL_G3D_FLUSH_VP:	�J�����g�ˉe�s��Ɏˉe�ϊ��s��ƃJ�����s�񂪍������ꂽ�s�񂪁A
 *						�J�����g�ʒu���W�s��ƕ����x�N�g���s��Ƀ��f�����O�s��
 *						���ݒ肳��܂��B
 *
 * GFL_G3D_FLUSH_WVP:	�J�����g�ˉe�s���
 *						�ˉe�ϊ��s��ƃJ�����s��ƃ��f�����O�s�񂪍������ꂽ�s�񂪁A
 *						�J�����g�ʒu���W�s��ƕ����x�N�g���s��ɒP�ʍs�񂪐ݒ肳��܂��B
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SetDrawFlushMode
		( GFL_G3D_DRAWFLUSH_TYPE type )
{
	GF_ASSERT( g3Dman != NULL );

	switch( type ){
		default:
		case GFL_G3D_FLUSH_P:
			g3Dman->drawFlushFunc = NNS_G3dGlbFlush;
			break;
		case GFL_G3D_FLUSH_VP:
			g3Dman->drawFlushFunc = NNS_G3dGlbFlushVP;
			break;
		case GFL_G3D_FLUSH_WVP:
			g3Dman->drawFlushFunc = NNS_G3dGlbFlushWVP;
			break;
	}
}





//=============================================================================================
//=============================================================================================
/**
 *
 *
 * �R�c�f�[�^�Ǘ��֐�
 *
 *	�R�c�f�[�^�͂R�K�w�ɋ敪����B
 *	�쐬�菇�Ƃ��Ă͈ȉ��̂Ƃ���
 *
 *	�P�j�t�@�C���Ŕz�u�����q�n�l�f�[�^�̓ǂݍ���		�F�����\�[�X�Ǘ��y�у��\�[�X�]���Q��
 *	�Q�j�P�̃��\�[�X���e�\���v�f�̍쐬				�F�������_�[�y�уA�j���[�V�����Ǘ��Q��
 *	�R�j�Q�̗v�f��g�ݍ��킹�ăf�[�^�I�u�W�F�N�g���쐬	�F���I�u�W�F�N�g�Ǘ��Q��
 *
 */
//=============================================================================================
//=============================================================================================
//=============================================================================================
/**
 *
 * �R�c���\�[�X�Ǘ��֐�
 *
 * �@�e�탊�\�[�X�t�@�C���ibmd,btx,bca,bva,bma,btp,bta�j����̌^�ňꌳ�Ǘ�����B
 * �@���\�[�X�^�C�v�̓n���h���ɓ���B
 *�@ �S�̃T�C�Y�̓t�@�C���Ɉˑ�����̂ŁA�������s��B
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
	GFL_G3D_GetTextureDataKey( NNSG3dResTex* res, NNSGfdTexKey* tex, NNSGfdTexKey* tex4x4 );
static BOOL
	GFL_G3D_GetTexturePlttKey( NNSG3dResTex* res, NNSGfdPlttKey* pltt );

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
	GFL_G3D_CreateResource
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
	GFL_G3D_CreateResourceArc
		( int arcID, int datID ) 
{
	NNSG3dResFileHeader* header;

	GF_ASSERT( g3Dman != NULL );

	//OS_Printf("3D_resource loading...\n");
	//�ΏۃA�[�J�C�u�h�m�c�d�w����w�b�_�f�[�^��ǂݍ���
	header = GFL_ARC_LoadDataAlloc( arcID, datID, g3Dman->heapID );

	return GFL_G3D_CreateResource( header );
}

//-------------------------------
// �A�[�J�C�u�t�@�C���p�X�ɂ��ǂݍ���
GFL_G3D_RES*
	GFL_G3D_CreateResourcePath
		( const char* path, int datID ) 
{
	NNSG3dResFileHeader* header;

	GF_ASSERT( g3Dman != NULL );

	//OS_Printf("3D_resource loading...\n");
	//�ΏۃA�[�J�C�u�t�@�C������w�b�_�f�[�^��ǂݍ���
	header = GFL_ARC_LoadDataFilePathAlloc( path, datID, g3Dman->heapID );

	return GFL_G3D_CreateResource( header );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c���\�[�X�̔j��
 *
 * @param	g3Dres	�R�c���\�[�X�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_DeleteResource
		( GFL_G3D_RES* g3Dres ) 
{
	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );

	GFL_HEAP_FreeMemory( g3Dres->file );
	GFL_HEAP_FreeMemory( g3Dres );
}
	
//--------------------------------------------------------------------------------------------
/**
 * �R�c���\�[�X�w�b�_�|�C���^�̎擾
 *
 * @param	g3Dres		�R�c���\�[�X�|�C���^
 *
 * @return	NNSG3dResFileHeader*
 *
 * �@�m�m�r�֐����O���Ŏg�p�������ꍇ�ȂǂɎg�p����
 */
//--------------------------------------------------------------------------------------------
NNSG3dResFileHeader*
	GFL_G3D_GetResourceFileHeader
		( GFL_G3D_RES* g3Dres ) 
{
	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );

	return (NNSG3dResFileHeader*)g3Dres->file;
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
	GFL_G3D_CheckResourceType
		( GFL_G3D_RES* g3Dres, GFL_G3D_RES_CHKTYPE checkType ) 
{
	u16 resType;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );

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
	GFL_G3D_TransVramTexture
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;
	NNSGfdPlttKey			plttKey;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX ));

	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//���\�[�X��]�����邽�߂̂u�q�`�l�L�[�̎擾
		if( GFL_G3D_GetTextureDataKey( texture, &texKey, &tex4x4Key ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadTex)\n");
			return FALSE;
		}
		if( GFL_G3D_GetTexturePlttKey( texture, &plttKey ) == FALSE ){
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
		return TRUE;
	}
	return FALSE;
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
	GFL_G3D_TransVramTextureDataOnly
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX ));

	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//���\�[�X��]�����邽�߂̂u�q�`�l�L�[�̎擾
		if( GFL_G3D_GetTextureDataKey( texture, &texKey, &tex4x4Key ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadTexDataOnly)\n");
			return FALSE;
		}
		//���\�[�X�ւ̂u�q�`�l�L�[�̐ݒ�
		NNS_G3dTexSetTexKey( texture, texKey, tex4x4Key );

		//�����łc�l�`�]������̂œ]�������t���b�V������
		DC_FlushRange( header, header->fileSize );
		//�L�[���Q�Ƃ��āA���\�[�X���u�q�`�l�]��
		NNS_G3dTexLoad( texture, FALSE );
		return TRUE;
	}
	return FALSE;
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
	GFL_G3D_TransVramTexturePlttOnly
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdPlttKey			plttKey;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX ));

	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//���\�[�X��]�����邽�߂̂u�q�`�l�L�[�̎擾
		if( GFL_G3D_GetTexturePlttKey( texture, &plttKey ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadPlttOnly)\n");
			return FALSE;
		}
		//���\�[�X�ւ̂u�q�`�l�L�[�̐ݒ�
		NNS_G3dPlttSetPlttKey( texture, plttKey );

		//�����łc�l�`�]������̂œ]�������t���b�V������
		DC_FlushRange( header, header->fileSize );
		//�L�[���Q�Ƃ��āA���\�[�X���u�q�`�l�]��
		NNS_G3dPlttLoad( texture, FALSE );
		return TRUE;
	}
	return FALSE;
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
	GFL_G3D_FreeVramTexture
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;
	NNSGfdPlttKey			plttKey;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX ));

	//�u�q�`�l�]���ς݂��ǂ����m�F
	if( GFL_G3D_CheckTextureKeyLive( g3Dres ) == TRUE ){

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
	return FALSE;
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
	GFL_G3D_GetTextureDataKey
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
	GFL_G3D_GetTexturePlttKey
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
	GFL_G3D_CheckTextureKeyLive
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			restex;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX )); 

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
 * �R�c�����_�[�Ǘ��֐�
 *
 * �@���f�����O�f�[�^�ƃe�N�X�`���[�Ńf�[�^���Ǘ�����B
 * �@���f�����\�[�X�Ɋւ��ẮA�e�N�X�`���\��t���̍ہA���̓��������������邽��
 * �@�P�̃����_�[�ɂ��P�̃��f�����\�[�X�����K�v������B
 *�@ �S�̃T�C�Y�͈��B
 *
 */
//=============================================================================================
#define G3DRND_MAGICNUM	(0x7A14)
///	�R�c�����_�[����p�\����
struct _GFL_G3D_RND
{
	u16						magicnum;
	u16						dummy;

	NNSG3dRenderObj*		rndobj;
	GFL_G3D_RES*			mdl;
	GFL_G3D_RES*			tex;
};


//--------------------------------------------------------------------------------------------
/**
 * �R�c�����_�[�̍쐬
 *
 * @param	mdl			�Q�ƃ��f���Z�b�g���\�[�X�\���̃|�C���^
 * @param	mdlidx		mdl���f�[�^�C���f�b�N�X(�����o�^����Ă���ꍇ�B�P�̏ꍇ��0)
 * @param	tex			�Q�ƃe�N�X�`�����\�[�X�\���̃|�C���^(�g�p���Ȃ��ꍇ��NULL)
 *
 * @return	GFL_3D_RND	�R�c�����_�[�n���h��
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RND*
	GFL_G3D_RENDER_Create
		( GFL_G3D_RES* mdl, int mdlidx, GFL_G3D_RES* tex )
{
	NNSG3dResMdlSet*		pMdlset;
	NNSG3dResMdl*			pMdl = NULL;
	NNSG3dResTex*			pTex = NULL;
	GFL_G3D_RND*			g3Drnd;

	//���f���f�[�^���\�[�X�|�C���^�擾
	GF_ASSERT( mdl->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT( GFL_G3D_CheckResourceType( mdl, GFL_G3D_RES_CHKTYPE_MDL ) == TRUE );
	pMdlset = NNS_G3dGetMdlSet( (NNSG3dResFileHeader*)mdl->file );
	pMdl = NNS_G3dGetMdlByIdx( pMdlset, mdlidx );
	GF_ASSERT( pMdl != NULL );

	//�e�N�X�`�����\�[�X�|�C���^�擾
	if( tex != NULL ){	//�e�N�X�`���[�Ȃ����w�肷�邱�Ƃ��ł���
		GF_ASSERT( tex->magicnum == G3DRES_MAGICNUM );
		GF_ASSERT( GFL_G3D_CheckResourceType( tex, GFL_G3D_RES_CHKTYPE_TEX ) == TRUE );
		pTex = NNS_G3dGetTex( (NNSG3dResFileHeader*)tex->file );
		GF_ASSERT( pTex != NULL );
	}

	//�����_�[�n���h���̍쐬
	g3Drnd = GFL_HEAP_AllocMemory( g3Dman->heapID, sizeof(GFL_G3D_RND) );
	g3Drnd->magicnum = G3DRND_MAGICNUM;
	g3Drnd->mdl = mdl;
	g3Drnd->tex = tex;

	//�����_�����O�I�u�W�F�N�g�̈�̊m��
	g3Drnd->rndobj = NNS_G3dAllocRenderObj( &g3Dman->allocater );
	GF_ASSERT( g3Drnd->rndobj != NULL );

	//�e�N�X�`�����\�[�X�Ƃ̊֘A�t��
	if( pTex ){
		BOOL result = TRUE;
		result &= NNS_G3dBindMdlTex( pMdl, pTex );
		result &= NNS_G3dBindMdlPltt( pMdl, pTex );
		if( result == FALSE ){
			OS_Printf("texture bind failed\n");
		}
	}
	//�����_�����O�I�u�W�F�N�g������
	NNS_G3dRenderObjInit( g3Drnd->rndobj, pMdl );

	return g3Drnd;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�����_�[�̔j��
 *
 * @param	g3Drnd	�R�c�����_�[�n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_RENDER_Delete
		( GFL_G3D_RND* g3Drnd ) 
{
	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	NNS_G3dFreeRenderObj( &g3Dman->allocater, g3Drnd->rndobj );
	GFL_HEAP_FreeMemory( g3Drnd );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�����_�[�̃��f�����\�[�X�|�C���^���擾
 *
 * @param	g3Drnd	�R�c�����_�[�n���h��
 *
 * @return	GFL_G3D_RES*
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RES*
	GFL_G3D_RENDER_GetG3DresMdl
		( GFL_G3D_RND* g3Drnd ) 
{
	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	return g3Drnd->mdl;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�����_�[�̃��f�����\�[�X�|�C���^���擾
 *
 * @param	g3Drnd	�R�c�����_�[�n���h��
 *
 * @return	GFL_G3D_RES*
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RES*
	GFL_G3D_RENDER_GetG3DresTex
		( GFL_G3D_RND* g3Drnd ) 
{
	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	return g3Drnd->tex;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�����_�[�̃����_�����O�I�u�W�F�N�g�|�C���^���擾
 *
 * @param	g3Drnd	�R�c�����_�[�n���h��
 *
 * @return	NNSG3dRenderObj*
 *
 * �@�m�m�r�֐����O���Ŏg�p�������ꍇ�ȂǂɎg�p����
 */
//--------------------------------------------------------------------------------------------
NNSG3dRenderObj*
	GFL_G3D_RENDER_GetRenderObj
		( GFL_G3D_RND* g3Drnd ) 
{
	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	return g3Drnd->rndobj;
}





//=============================================================================================
/**
 *
 * �R�c�A�j���[�V�����Ǘ��֐�
 *
 * �@�e�N�X�`���A�j���[�V�����{�W���C���g�A�j���[�V������
 * �@�e��A�j���[�V�������Ǘ�����B
 * �@���̃A�j���[�V�����͈�̃����_�[�Ɉ��ނƂ����킯�ł͂Ȃ��̂�
 * �@�����_�[�Ƃ͕ʂ̍\���v�f�Ƃ��ĊǗ�����B
 * �@�쐬�̍ہA�Ώۂ̃����_�[�n���h���i�A���P�[�g�o�b�t�@�T�C�Y�Ɋ֌W����j��
 * �@�K�v�Ƃ���̂ŁA�쐬�菇�ɒ��ӂ��邱�ƁB
 *
 */
//=============================================================================================
#define G3DANM_MAGICNUM	(0x59d1)
///	�R�c�A�j���[�V��������p�\����
struct _GFL_G3D_ANM
{
	u16				magicnum;
	u16				dummy;

	NNSG3dAnmObj*	anmobj;
	GFL_G3D_RES*	anm;
};

//--------------------------------------------------------------------------------------------
/**
 * �R�c�A�j���[�V�����̍쐬
 *
 * @param	g3Drnd		�t�������Ώۂ̂R�c�����_�[�n���h��
 * @param	anm			�Q�ƃA�j���[�V�������\�[�X�\���̃|�C���^
 * @param	anmidx		anm���f�[�^�C���f�b�N�X(�����o�^����Ă���ꍇ�B�P�̏ꍇ��0)
 *
 * @return	GFL_3D_ANM	�R�c�A�j���[�V�����n���h��
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_ANM*
	GFL_G3D_ANIME_Create
		( GFL_G3D_RND* g3Drnd, GFL_G3D_RES* anm, int anmidx )  
{
	GFL_G3D_ANM*	g3Danm;
	NNSG3dResMdl*	pMdl;
	NNSG3dResTex*	pTex;
	void*			pAnm;

	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );
	GF_ASSERT( anm->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT( anm->type == GFL_G3D_RES_TYPE_ANM );

	//�A�j���[�V�����n���h���̍쐬
	g3Danm = GFL_HEAP_AllocMemory( g3Dman->heapID, sizeof(GFL_G3D_ANM) );
	g3Danm->magicnum = G3DANM_MAGICNUM;
	g3Danm->anm = anm;

	//�A�j���[�V�������\�[�X�|�C���^�擾
	pAnm = NNS_G3dGetAnmByIdx( anm->file, anmidx );
	GF_ASSERT( pAnm != NULL );

	pMdl = NNS_G3dRenderObjGetResMdl( g3Drnd->rndobj );
	pTex = NNS_G3dGetTex( (NNSG3dResFileHeader*)g3Drnd->tex->file );

	//�A�j���[�V�����I�u�W�F�N�g�̈�̊m��
	g3Danm->anmobj = NNS_G3dAllocAnmObj( &g3Dman->allocater, pAnm, pMdl );
	GF_ASSERT( g3Danm->anmobj != NULL );
	//�A�j���[�V�����I�u�W�F�N�g������
	NNS_G3dAnmObjInit( g3Danm->anmobj, pAnm, pMdl, pTex );

	return g3Danm;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�A�j���[�V�����̔j��
 *
 * @param	g3Danm	�R�c�A�j���[�V�����n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ANIME_Delete
		( GFL_G3D_ANM* g3Danm ) 
{
	GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

	NNS_G3dFreeAnmObj( &g3Dman->allocater, g3Danm->anmobj );
	GFL_HEAP_FreeMemory( g3Danm );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�A�j���[�V�������\�[�X�|�C���^���擾
 *
 * @param	g3Danm	�R�c�A�j���[�V�����n���h��
 *
 * @return	GFL_G3D_RES*
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RES*
	GFL_G3D_ANIME_GetG3Dres
		( GFL_G3D_ANM* g3Danm ) 
{
	GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

	return g3Danm->anm;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�A�j���[�V�����̃A�j���I�u�W�F�N�g�|�C���^���擾
 *
 * @param	g3Danm	�R�c�A�j���[�V�����n���h��
 *
 * @return	NNSG3dAnmObj*
 *
 * �@�m�m�r�֐����O���Ŏg�p�������ꍇ�ȂǂɎg�p����
 */
//--------------------------------------------------------------------------------------------
NNSG3dAnmObj*
	GFL_G3D_ANIME_GetAnmObj
		( GFL_G3D_ANM* g3Danm ) 
{
	GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

	return g3Danm->anmobj;
}





//=============================================================================================
/**
 *
 * �R�c�I�u�W�F�N�g�Ǘ��֐�
 *
 * �@�����_�[�y�уA�j���[�V�����ɂ���č\�������B
 * �@�X�e�[�^�X�i���W�A�X�P�[���A��]�j�̓I�u�W�F�N�g�n���h���ɂ͓���Ȃ��B
 * �@����̃I�u�W�F�N�g�͂����܂ł��`�惊�\�[�X�f�[�^�S�ł���
 * �@�@���ꂪ�P���̂Ƃ͌���Ȃ����߁B
 * �@�@�@��j�X��`�悷��ہA�؂̃I�u�W�F�N�g����p�ӂ��A�C�ӂ̃X�e�[�^�X�z�u�ŕ����`�悷��B
 * �@�܂��A�j���[�V�����͕����o�^�ł���悤�ɍ\������Ă���B�i�Ȃ����\�j
 * �@�@�@��j�e�N�X�`���A�j���[�V�����{�W���C���g�A�j���[�V����
 *�@ ���̂��߃n���h���T�C�Y�͕s��ɂȂ��Ă��邱�Ƃɒ��ӂ��邱�ƁB
 *
 */
//=============================================================================================
#define G3DOBJ_MAGICNUM	(0x2185)
///	�R�c�I�u�W�F�N�g����p�\����
struct _GFL_G3D_OBJ
{
	u16				magicnum;

	GFL_G3D_RND*	g3Drnd;
	GFL_G3D_ANM**	anmTbl;
	u16				anmCount;
};

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̍쐬
 *
 * @param	g3Drnd		�����_�[�n���h��
 * @param	anmTbl		�t������A�j���[�V�����n���h���e�[�u���i�����ݒ���\�ɂ��邽�߁j
 * @param	anmCount	�t������A�������͕t���\��̃A�j���[�V�����̐�
 *
 * @return	GFL_3D_OBJ	�R�c�I�u�W�F�N�g�n���h��
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_OBJ*
	GFL_G3D_OBJECT_Create
		( GFL_G3D_RND* g3Drnd, GFL_G3D_ANM** anmTbl, u16 anmCount )  
{
	GFL_G3D_OBJ*	g3Dobj;
	GFL_G3D_ANM*	g3Danm;
	int	i;

	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	//�I�u�W�F�N�g�n���h���̍쐬
	g3Dobj = GFL_HEAP_AllocMemory( g3Dman->heapID, sizeof(GFL_G3D_OBJ) );
	g3Dobj->magicnum	= G3DOBJ_MAGICNUM;
	g3Dobj->g3Drnd		= g3Drnd;
	g3Dobj->anmCount	= anmCount;

	//�A�j���[�V�����z��쐬
	g3Dobj->anmTbl = GFL_HEAP_AllocClearMemory( g3Dman->heapID, sizeof(GFL_G3D_ANM*) * anmCount );
	for( i=0; i<anmCount; i++ ){
		g3Danm = anmTbl[i];
		if( g3Danm != NULL ){
			GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

			//�����̎��_�ł͂n�m�ɂ��Ȃ�(ica�������ݒ肳��Ă���ƃ}�[�W����Ă��܂�����)
			//NNS_G3dRenderObjAddAnmObj( g3Drnd->rndobj, g3Danm->anmobj );
		}
		g3Dobj->anmTbl[i] = g3Danm;
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
	GFL_G3D_OBJECT_Delete
		( GFL_G3D_OBJ* g3Dobj ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );

	GFL_HEAP_FreeMemory( g3Dobj->anmTbl );
	GFL_HEAP_FreeMemory( g3Dobj );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̃����_�[�n���h���|�C���^���擾
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 *
 * @return	GFL_G3D_RND*
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RND*
	GFL_G3D_OBJECT_GetG3Drnd
		( GFL_G3D_OBJ* g3Dobj ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );

	return g3Dobj->g3Drnd;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̃A�j���[�V�����n���h���|�C���^���擾
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	anmIdx	�o�^����Ă���A�j���[�V�����C���f�b�N�X
 *
 * @return	GFL_G3D_ANM*
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_ANM*
	GFL_G3D_OBJECT_GetG3Danm
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	return g3Dobj->anmTbl[ anmIdx ];
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̃A�j���[�V�����ݒ萔���擾
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 *
 * @return	u16
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_OBJECT_GetAnimeCount
		( GFL_G3D_OBJ* g3Dobj ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );

	return g3Dobj->anmCount;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̃A�j���[�V������L���ɂ���
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	anmIdx	�o�^����Ă���A�j���[�V�����C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_OBJECT_EnableAnime
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	//�����_�����O�I�u�W�F�N�g�Ƃ̊֘A�t��
	NNS_G3dRenderObjAddAnmObj( g3Dobj->g3Drnd->rndobj, g3Dobj->anmTbl[ anmIdx ]->anmobj );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̃A�j���[�V�����𖳌��ɂ���
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	anmIdx	�o�^����Ă���A�j���[�V�����C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_OBJECT_DisableAnime
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	//�����_�����O�I�u�W�F�N�g�Ƃ̊֘A�t��������
	NNS_G3dRenderObjRemoveAnmObj( g3Dobj->g3Drnd->rndobj, g3Dobj->anmTbl[ anmIdx ]->anmobj );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�ɃA�j���[�V������ǉ�����
 *
 * @param	g3Dobj		�R�c�I�u�W�F�N�g�n���h��
 * @param	g3Danm		�R�c�A�j���[�V�����n���h��
 *
 * @return	u16			�o�^�ʒu
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_OBJECT_AddAnime
		( GFL_G3D_OBJ* g3Dobj, GFL_G3D_ANM* g3Danm )
{
	u16	i;

	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

	for( i=0; i<g3Dobj->anmCount; i++ ){
		if( g3Dobj->anmTbl[i] == NULL ){
			//�����_�����O�I�u�W�F�N�g�Ƃ̊֘A�t��
			//NNS_G3dRenderObjAddAnmObj( g3Dobj->g3Drnd->rndobj, g3Danm->anmobj );
			g3Dobj->anmTbl[ i ] = g3Danm;
			return i;
		}
	}
	GF_ASSERT(0);
	return 0xffff;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g����A�j���[�V�������폜����
 *
 * @param	g3Dobj		�R�c�I�u�W�F�N�g�n���h��
 * @param	anmIdx		�o�^����Ă���A�j���[�V�����C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_OBJECT_RemoveAnime
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx )
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	//�����_�����O�I�u�W�F�N�g�Ƃ̊֘A�t��������
	//NNS_G3dRenderObjRemoveAnmObj( g3Dobj->g3Drnd->rndobj, g3Dobj->anmTbl[ anmIdx ]->anmobj );
	g3Dobj->anmTbl[ anmIdx ] = NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̃A�j���[�V�����t���[�������Z�b�g
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	anmIdx	�o�^����Ă���A�j���[�V�����C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_OBJECT_ResetAnimeFrame
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx )
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	g3Dobj->anmTbl[ anmIdx ]->anmobj->frame = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̃A�j���[�V�����t���[����i�߂�
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	anmIdx	�o�^����Ă���A�j���[�V�����C���f�b�N�X
 * @param	count	�������iFX32_ONE�łP�t���[���i�߂�j
 *
 * @return	BOOL	FALSE�ŏI�����o
 */
//--------------------------------------------------------------------------------------------
BOOL
	GFL_G3D_OBJECT_IncAnimeFrame
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx, const fx32 count ) 
{
	NNSG3dAnmObj* anmobj;

	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	anmobj = g3Dobj->anmTbl[ anmIdx ]->anmobj;
	anmobj->frame += count;
	
	if( anmobj->frame >= NNS_G3dAnmObjGetNumFrame( anmobj )){
		return FALSE;
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̃A�j���[�V�����I�[�g���[�v
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	anmIdx	�o�^����Ă���A�j���[�V�����C���f�b�N�X
 * @param	count	�������iFX32_ONE�łP�t���[���i�߂�j
 *
 * @return	BOOL	FALSE�łP���[�v�I�����o
 */
//--------------------------------------------------------------------------------------------
BOOL
	GFL_G3D_OBJECT_LoopAnimeFrame
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx, const fx32 count ) 
{
	NNSG3dAnmObj* anmobj;

	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	anmobj = g3Dobj->anmTbl[ anmIdx ]->anmobj;
	anmobj->frame += count;
	
	if( anmobj->frame >= NNS_G3dAnmObjGetNumFrame( anmobj )){
		anmobj->frame = 0;
		return FALSE;
	}
	return TRUE;
}





//=============================================================================================
//=============================================================================================
/**
 *
 * �`��֐�
 *
 * �@�R�c�I�u�W�F�N�g�n���h���Ƃ����̃X�e�[�^�X�i���W�A�X�P�[���A��]�j�������Ƃ��ĕ`����s���B
 *
 *	SAMPLE
 *	{
 *		GFL_G3D_DRAW_Start();							//�`��J�n
 *		GFL_G3D_DRAW_SetLookAt();						//�J�����O���[�o���X�e�[�g�ݒ�		
 *		{
 *			GFL_G3D_DRAW_DrawObject( g3Dobj, status );	//�e�I�u�W�F�N�g�`��
 *		}
 *		GFL_G3D_DRAW_End();								//�`��I���i�o�b�t�@�X���b�v�j
 *	}
 *
 */
//=============================================================================================
static BOOL GFL_G3D_DRAW_CheckCulling( GFL_G3D_OBJ* g3Dobj );
static s32	GFL_G3D_DRAW_TestBoundingBox( const GXBoxTestParam* boundingBox );
static void GFL_G3D_DRAW_GetTRSMatrix( const GFL_G3D_OBJSTATUS* status, MtxFx43* mtxTRS );
//u32 DebugCullingCount = 0;
//u32 DebugPrintCount = 0;
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �R�c�`��̊J�n
 *
 * �S�̕`��֐����A�`��J�n���ɌĂяo�����
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_DRAW_Start
		( void )
{
	G3X_Reset();
	//DebugPrintCount = 0;
	//DebugCullingCount = 0;
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
	GFL_G3D_DRAW_End
		( void )
{
	NNS_G3dGeFlushBuffer();
	G3_SwapBuffers( g3Dman->swapBufMode.aw, g3Dman->swapBufMode.zw );
	//OS_Printf(" CullingCount is %d PrintCount is %d\n", DebugCullingCount, DebugPrintCount );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����O���[�o���X�e�[�g�̐ݒ�
 *
 * �S�̕`��֐����ɌĂяo�����
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_DRAW_SetLookAt
		( void )
{
	NNS_G3dGlbLookAt( &g3Dman->lookAt.camPos, &g3Dman->lookAt.camUp, &g3Dman->lookAt.target );
	g3Dman->drawFlushFunc();	//�O���[�o���X�e�[�g���f
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̕`��
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 */
//--------------------------------------------------------------------------------------------
// �R�c�I�u�W�F�N�g�̏��ݒ�
static inline void 
	statusSet( const VecFx32* pTrans, const MtxFx33* pRotate, const VecFx32* pScale )
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
//�����u��������΍���������邩�H��
/*
#if 0
	statusSet( &status->trans, &status->rotate, &status->scale );
	g3Dman->drawFlushFunc();
	NNS_G3dDraw( GFL_G3D_RENDER_GetRenderObj( GFL_G3D_OBJECT_GetG3Drnd( g3Dobj ) ) );
	NNS_G3dGeFlushBuffer();
#else
	MtxFx43 mtxTRS;

	NNS_G3dGeLoadMtx43( &NNS_G3dGlb.cameraMtx );
	GFL_G3D_DRAW_GetTRSMatrix( status, &mtxTRS );
	NNS_G3dGePushMtx();
	G3_MultMtx43( &mtxTRS );
	NNS_G3dDraw( GFL_G3D_RENDER_GetRenderObj( GFL_G3D_OBJECT_GetG3Drnd( g3Dobj ) ) );
	NNS_G3dGePopMtx(1);
	NNS_G3dGeFlushBuffer();
#endif
*/

//�J�����O�Ȃ�
void
	GFL_G3D_DRAW_DrawObject
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status )
{
    NNSG3dRenderObj* renderobj = GFL_G3D_RENDER_GetRenderObj( GFL_G3D_OBJECT_GetG3Drnd(g3Dobj) );

	statusSet( &status->trans, &status->rotate, &status->scale );
	g3Dman->drawFlushFunc();
	NNS_G3dDraw( renderobj );
	//NNS_G3dGeFlushBuffer();
}

//�J�����O����
BOOL
	GFL_G3D_DRAW_DrawObjectCullingON
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status )
{
    NNSG3dRenderObj* renderobj = GFL_G3D_RENDER_GetRenderObj( GFL_G3D_OBJECT_GetG3Drnd(g3Dobj) );
	BOOL result;

	statusSet( &status->trans, &status->rotate, &status->scale );
	g3Dman->drawFlushFunc();
	result = GFL_G3D_DRAW_CheckCulling( g3Dobj );
	if( result == TRUE ){
		NNS_G3dDraw( renderobj );
		//DebugPrintCount++;
	}
	//NNS_G3dGeFlushBuffer();
	return result;
}

//--------------------------------------------------------------------------------
//�A�N�Z�T���[�̕`��
//	�Ώ�GFL_G3D_OBJ�̃����_�����O���\�[�X(imd)��g3dcvtr -s �ŏo�͂���Ă���K�v������B
//	������ɂ��eNode�s�񂪍s��X�^�b�N�Ɋi�[���ꂽ��Ԃŕ`����I���邽�߁A
//	�@�{�̒���ɂ��̊֐����ĂԂ��Ƃɂ��A�w��Node�s����Q�Ƃ�����Ԃŕ`�悷�邱�Ƃ��ł���B
//	�@Node���̍ő��30�܂ŁB�R���o�[�g�̍ۂɐ���������̂����̂��߂炵���B
//	  �܂��ANNS_G3DGlbFlash���ĂԕK�v�͂Ȃ��B
//�@�@�J�����O�n�m�ɂ��Ă͖{�̂Ɉˑ����Ă������ȂƎv���Ă���̂łƂ肠�������Ȃ��B
void
	GFL_G3D_DRAW_DrawAccesory
		( GFL_G3D_OBJ* g3Dobj, GFL_G3D_OBJ* g3Dobj_Accesory,
			const GFL_G3D_OBJSTATUS* status, int jntID )
{
    NNSG3dRenderObj* renderobj = GFL_G3D_RENDER_GetRenderObj( GFL_G3D_OBJECT_GetG3Drnd(g3Dobj) );
    NNSG3dRenderObj* renderobj_Accesory = GFL_G3D_RENDER_GetRenderObj
											( GFL_G3D_OBJECT_GetG3Drnd(g3Dobj_Accesory) );
	BOOL result;

	// �����_�����O�I�u�W�F�N�g��jntID�m�[�h�ɑΉ�����s����J�����g�s��Ɏ����Ă���
	// �Q�Ԗڂ̈�����MtxFx43�ւ̃|�C���^���w�肷��ƈʒu���W�s�񂪁A
	// �R�Ԗڂ̈�����MtxFx33�ւ̃|�C���^���w�肷��ƕ����x�N�g���s�񂪓�����B
	result = NNS_G3dGetResultMtx( renderobj, NULL, NULL, (u32)jntID );

	// jntID�m�[�h�ɑΉ�����s�񂪂Ȃ������ꍇ��FALSE���Ԃ��Ă���̂ŔO�̂��߃`�F�b�N
	if( result != FALSE ){
		// �`�撼�O�ɂ���posScale�ł̃X�P�[�����O�͂�����Ȃ��̂ŁA
		// �����I�ɃX�P�[�����O���Ȃ��Ă͂Ȃ�Ȃ��B
		NNS_G3dGeTranslateVec( &status->trans );
		NNS_G3dGeScaleVec( &status->scale );
		NNS_G3dGeMultMtx33( &status->rotate );
		NNS_G3dDraw( renderobj_Accesory );
	}
	//NNS_G3dGeFlushBuffer();
}

//�W���C���g���ɂ��`��
void
	GFL_G3D_DRAW_DrawAccesoryByName
		( GFL_G3D_OBJ* g3Dobj, GFL_G3D_OBJ* g3Dobj_Accesory,
			const GFL_G3D_OBJSTATUS* status, const char* jntName )
{
	int jntID;
    NNSG3dRenderObj* renderobj = GFL_G3D_RENDER_GetRenderObj( GFL_G3D_OBJECT_GetG3Drnd(g3Dobj) );

	// �W���C���gID���擾����B�����Ō�����Ȃ��ꍇ�͕��̒l���Ԃ�B
	//define NNS_G3D_GET_JNTID(pMdl, pJntID, literal)
	jntID = NNS_G3dGetNodeIdxByName( NNS_G3dGetNodeInfo( renderobj->resMdl ), 
									(NNSG3dResName*)jntName );
	GF_ASSERT(jntID > 0);

	GFL_G3D_DRAW_DrawAccesory( g3Dobj, g3Dobj_Accesory, status, jntID );
}

//--------------------------------------------------------------------------------
// �P�̃��f���ɂP�̃}�e���A���̂ݐݒ肳��Ă���Ƃ��ɍ����`�悷�邽�߂̊֐�
//		matID	�`�悷��}�e���A���ւ̃C���f�b�N�X 
//		shpID	�`�悷��V�F�C�v�ւ̃C���f�b�N�X 
//		sendMat	�}�e���A�������W�I���g���G���W���ɑ��M���邩�ǂ��� 
//--------------------------------------------------------------------------------
//�J�����O�Ȃ�
void
	GFL_G3D_DRAW_DrawObject1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat, 
			const GFL_G3D_OBJSTATUS* status )

{
    NNSG3dRenderObj* renderobj = GFL_G3D_RENDER_GetRenderObj( GFL_G3D_OBJECT_GetG3Drnd(g3Dobj) );

	statusSet( &status->trans, &status->rotate, &status->scale );
	g3Dman->drawFlushFunc();
	NNS_G3dDraw1Mat1Shp( NNS_G3dRenderObjGetResMdl( renderobj ), matID, shpID, sendMat );
	//NNS_G3dGeFlushBuffer();
}

//�J�����O����
BOOL
	GFL_G3D_DRAW_DrawObject1mat1shpCullingON
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat, 
			const GFL_G3D_OBJSTATUS* status )

{
    NNSG3dRenderObj* renderobj = GFL_G3D_RENDER_GetRenderObj( GFL_G3D_OBJECT_GetG3Drnd(g3Dobj) );
	BOOL result;

	statusSet( &status->trans, &status->rotate, &status->scale );
	g3Dman->drawFlushFunc();
	result = GFL_G3D_DRAW_CheckCulling( g3Dobj );
	if( result == TRUE ){
		NNS_G3dDraw1Mat1Shp( NNS_G3dRenderObjGetResMdl( renderobj ), matID, shpID, sendMat );
	}
	//NNS_G3dGeFlushBuffer();
	return result;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����O�`�F�b�N
 *
 * �I�u�W�F�N�g�̃O���[�o���X�e�[�^�X���f��ɌĂԂ���
 */
//--------------------------------------------------------------------------------------------
static BOOL
	GFL_G3D_DRAW_CheckCulling
		( GFL_G3D_OBJ* g3Dobj )
{
	NNSG3dResMdlInfo*	modelInfo;		// ���f���f�[�^����̃{�b�N�X�e�X�g�p�p�����[�^�擾�p
	BOOL				result = TRUE;
	GXBoxTestParam		boundingBox;

	//�{�b�N�X�e�X�g�p�p�����[�^�擾
	modelInfo = NNS_G3dGetMdlInfo
	( NNS_G3dRenderObjGetResMdl(GFL_G3D_RENDER_GetRenderObj(GFL_G3D_OBJECT_GetG3Drnd(g3Dobj))));

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
	if( !GFL_G3D_DRAW_TestBoundingBox( &boundingBox ) ){
		result = FALSE;
		//DebugCullingCount++;
	}
	NNS_G3dGePopMtx(1);

	return result;
}

//----------------------------------------------------------------------------
/**
 * �{�b�N�X���̐ϓ��O�e�X�g
 *
 * @param	boundingBox		�{�b�N�X�e�X�g�p�p�����[�^�ւ̃|�C���^
 *
 * @return	s32				0�ȊO�̒l�ł���΁A�{�b�N�X�̈ꕔ�܂��͑S�������̐ϓ�
 *							0�ł���΁A�{�b�N�X�̑S�Ă����̐ϊO 
 */
//-----------------------------------------------------------------------------
static s32
	GFL_G3D_DRAW_TestBoundingBox
		( const GXBoxTestParam* boundingBox )
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
/**
 * TRS�}�g���N�X���擾
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void 
	GFL_G3D_DRAW_GetTRSMatrix
		( const GFL_G3D_OBJSTATUS* status, MtxFx43* mtxTRS )
{
	MtxFx43	rot;
	MtxFx43 scl;

	MTX_Identity43( mtxTRS );
	MTX_TransApply43( mtxTRS, mtxTRS, status->trans.x, status->trans.y, status->trans.z);
	MTX_Copy33To43( &status->rotate, &rot );
	MTX_Scale43( &scl, status->scale.x, status->scale.y, status->scale.z);
	MTX_Concat43( &rot, mtxTRS, mtxTRS );
	MTX_Concat43( &scl, mtxTRS, mtxTRS );
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



