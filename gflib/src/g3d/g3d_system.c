//=============================================================================================
/**
 * @file	g3d_system.c                                                  
 * @brief	�R�c�`��Ǘ��V�X�e���v���O����
 * @date	2006/4/26
 */
//=============================================================================================
#include "gflib.h"
#include "g3d_system.h"

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
/**
 * @brief	�R�c�V�X�e���}�l�[�W���\����
 */
typedef struct GFL_G3D_MAN_tag
{
	NNSFndAllocator		allocater;	///<�������A���P�[�^(NNS�Ŏg�p)
	void*				plt_memory;	///<�p���b�g�}�l�[�W���p������
	void*				tex_memory;	///<�e�N�X�`���}�l�[�W���p������
	u16					heapID;		///<�q�[�v�h�c
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
				u16 GeBufSize, u16 heapID, GFL_G3D_SETUP_FUNC setup )
{
	int	tex_size,plt_size;
	g3Dman = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_G3D_MAN) );

	g3Dman->heapID = heapID;

	// �W�I���g���R�}���h�o�b�t�@�̐ݒ�
	if( GeBufSize ){
		NNSG3dGeBuffer* geBuf;

		if( GeBufSize > GEBUF_SIZE_MAX ){
			GeBufSize = GEBUF_SIZE_MAX;
		}
		geBuf = ( NNSG3dGeBuffer* )GFL_HEAP_DTCM_AllocMemory( GeBufSize );
		NNS_G3dGeSetBuffer( geBuf );
	}

	// NitroSystem:�R�c�G���W���̏�����
	NNS_G3dInit();
	// �}�g���N�X�X�^�b�N�̏�����
    G3X_InitMtxStack();

	// �W�I���g���G���W���N����K���Ă΂�Ȃ���΂Ȃ�Ȃ�
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

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
	RES_TYPE_UNKNOWN = 0,	//�s���ȃf�[�^
	RES_TYPE_MDLTEX,		//�t�@�C�����Ƀ��f�����O����уe�N�X�`���f�[�^����
	RES_TYPE_MDL,			//�t�@�C�����Ƀ��f�����O�f�[�^����
	RES_TYPE_TEX,			//�t�@�C�����Ƀe�N�X�`���f�[�^����
	RES_TYPE_ANM,			//�t�@�C�����ɃA�j���[�V�����f�[�^����
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
static BOOL
	GFL_G3D_ObjTexkeyLiveCheck( NNSG3dResTex* restex );

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

	OS_Printf("3D_resource check now...\n");
	//�t�@�C���^�C�v�̔���
	switch( *(u32*)&header[0] )
	{
		case NNS_G3D_SIGNATURE_NSBMD:
			OS_Printf("nsbmd file check...\n");
			if( NNS_G3dGetTex( header ) == NULL ){
				OS_Printf("this 3D_resource is model_data\n");
				g3Dres->type = RES_TYPE_MDL;	//���f�����O�f�[�^����
			} else {
				OS_Printf("this 3D_resource is model_data & texture_data\n");
				g3Dres->type = RES_TYPE_MDLTEX;	//���f�����O����уe�N�X�`���f�[�^����
			}
			break;
		case NNS_G3D_SIGNATURE_NSBTX:
			OS_Printf("this 3D_resource is texture_data\n");
			g3Dres->type = RES_TYPE_TEX;		//�e�N�X�`���f�[�^����
			break;
		case NNS_G3D_SIGNATURE_NSBCA:
		case NNS_G3D_SIGNATURE_NSBVA:
		case NNS_G3D_SIGNATURE_NSBMA:
		case NNS_G3D_SIGNATURE_NSBTP:
		case NNS_G3D_SIGNATURE_NSBTA:
			OS_Printf("this 3D_resource is animetion_data\n");
			g3Dres->type = RES_TYPE_ANM;		//�t�@�C�����ɃA�j���[�V�����f�[�^����
			break;
		default:
			OS_Printf("this 3D_resource is unknown\n");
			g3Dres->type = RES_TYPE_UNKNOWN;	//�s���ȃf�[�^
			break;
	}
	g3Dres->magicnum = G3DRES_MAGICNUM;
	//�t�@�C���|�C���^�̐ݒ�
	g3Dres->file = ( void* )header;
	OS_Printf("3D_resource is loaded\n");

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
	OS_Printf("3D_resource loading...\n");
	//�ΏۃA�[�J�C�u�t�@�C������w�b�_�f�[�^��ǂݍ���
	header = GFL_ARC_DataLoadFilePathMalloc( path, datID, g3Dman->heapID );

	return GFL_G3D_ResourceCreate( header );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c���\�[�X�̔j��
 *
 * @param	g3Dres	�R�c���\�[�X�|�C���^
 *
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
 * �e�N�X�`�����\�[�X�̓]��(���u�q�`�l)
 *
 * @param	g3Dres	�R�c���\�[�X�|�C���^
 *
 * @return	BOOL	����(����=TRUE)
 */
//--------------------------------------------------------------------------------------------
BOOL 
	GFL_G3D_TransVramTex
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;
	NNSGfdPlttKey			plttKey;

	if( G3DRES_FILE_CHECK( g3Dres ) == FALSE ){
		OS_Printf("file is not 3D_resource (GFL_G3D_TransTex)\n");
		return FALSE;
	}
	if(( g3Dres->type != RES_TYPE_MDLTEX )&&( g3Dres->type != RES_TYPE_TEX )){
		OS_Printf("file is not texture_resource (GFL_G3D_TransTex)\n");
		return FALSE;
	}
	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//���\�[�X��]�����邽�߂̂u�q�`�l�L�[�̎擾
		if( GFL_G3D_GetTexDataVramkey( texture, &texKey, &tex4x4Key ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_TransTex)\n");
			return FALSE;
		}
		if( GFL_G3D_GetTexPlttVramkey( texture, &plttKey ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_TransTex)\n");
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
 * �e�N�X�`���f�[�^���\�[�X�̓]��(���u�q�`�l)
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
	GFL_G3D_TransVramTexDataOnly
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;

	if( G3DRES_FILE_CHECK( g3Dres ) == FALSE ){
		OS_Printf("file is not 3D_resource (GFL_G3D_TransTexDataOnly)\n");
		return FALSE;
	}
	if(( g3Dres->type != RES_TYPE_MDLTEX )&&( g3Dres->type != RES_TYPE_TEX )){
		OS_Printf("file is not texture_resource (GFL_G3D_TransTexDataOnly)\n");
		return FALSE;
	}
	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//���\�[�X��]�����邽�߂̂u�q�`�l�L�[�̎擾
		if( GFL_G3D_GetTexDataVramkey( texture, &texKey, &tex4x4Key ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_TransTexDataOnly)\n");
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
 * �e�N�X�`���p���b�g���\�[�X�̓]��(���u�q�`�l)
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
	GFL_G3D_TransVramTexPlttOnly
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdPlttKey			plttKey;

	if( G3DRES_FILE_CHECK( g3Dres ) == FALSE ){
		OS_Printf("file is not 3D_resource (GFL_G3D_TransPlttOnly)\n");
		return FALSE;
	}
	if(( g3Dres->type != RES_TYPE_MDLTEX )&&( g3Dres->type != RES_TYPE_TEX )){
		OS_Printf("file is not texture_resource (GFL_G3D_TransPlttOnly)\n");
		return FALSE;
	}
	//�e�N�X�`�����\�[�X�|�C���^�̎擾
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//���\�[�X��]�����邽�߂̂u�q�`�l�L�[�̎擾
		if( GFL_G3D_GetTexPlttVramkey( texture, &plttKey ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_TransPlttOnly)\n");
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
static BOOL
	GFL_G3D_ObjTexkeyLiveCheck
		( NNSG3dResTex* restex )
{
	// ���ʂ̃e�N�X�`����4*4�e�N�Z�����k�e�N�X�`��
	// ��Vram�ɓW�J����Ă��邩���`�F�b�N
	if((restex->texInfo.flag & NNS_G3D_RESTEX_LOADED) ||
	   (restex->tex4x4Info.flag & NNS_G3D_RESTEX4x4_LOADED)){
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
	u8						drawSW;
	u8						anmSW;

	NNSG3dRenderObj*		rndobj;
	NNSG3dAnmObj*			anmobj;

	NNSG3dResMdl*			pMdl;
	NNSG3dResTex*			pTex;
	void*					pAnm;

	VecFx32					trans;			//���W
	VecFx32					scale;			//�X�P�[��
	VecFx32					rotate;			//��]	
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
	   (( mdl->type == RES_TYPE_MDLTEX )||( mdl->type == RES_TYPE_MDL )) ){
		//���f���f�[�^���\�[�X�|�C���^�擾
		header = (NNSG3dResFileHeader*)mdl->file;
		pMdlset = NNS_G3dGetMdlSet( header );
		pMdl = NNS_G3dGetMdlByIdx( pMdlset, mdlidx );
	} else {
		pMdl = NULL;
	}
	if(( G3DRES_FILE_CHECK( tex ) == TRUE )&&
	   (( tex->type == RES_TYPE_MDLTEX )||( tex->type == RES_TYPE_TEX )) ){
		//�e�N�X�`�����\�[�X�|�C���^�擾
		header = (NNSG3dResFileHeader*)tex->file;
		pTex = NNS_G3dGetTex( header );
	} else {
		pTex = NULL;
	}
	if(( G3DRES_FILE_CHECK( anm ) == TRUE )&&( anm->type == RES_TYPE_ANM )){
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

	//�z�u�p�����[�^������
	{
		VecFx32 init_trans	= { 0, 0, 0 };
		VecFx32 init_scale	= { FX32_ONE, FX32_ONE, FX32_ONE };
		VecFx32 init_rotate = { 0, 0, 0 };

		g3Dobj->trans	= init_trans;
		g3Dobj->scale	= init_scale;
		g3Dobj->rotate	= init_rotate;
	}

	//�`��X�C�b�`������
	g3Dobj->drawSW = 0; 
	g3Dobj->anmSW = 0;

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
 *
 */
//=============================================================================================
static inline void G3DOBJ_DRAWCALC( VecFx32* pTrans, MtxFx33* pRotate, VecFx32* pScale )
{
	// �ʒu�ݒ�
	NNS_G3dGlbSetBaseTrans( pTrans );
	// �p�x�ݒ�
	NNS_G3dGlbSetBaseRot( pRotate );
	// �X�P�[���ݒ�
	NNS_G3dGlbSetBaseScale( pScale );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̕`��̊J�n
 *
 * �O���[�o���X�e�[�g
 * �i�ˉe�ϊ��s��A�J�����s��A���f�����O�s��A�r���[�|�[�g�A���C�g�ݒ�A�}�e���A���J���[���j
 * ���W�I���g���G���W���ɑ��M����B
 * ���ɂ��̃X�e�[�g�ɊO�����H�����Ă��Ȃ���΁A�e�I�u�W�F�N�g�`��֐�(ObjDraw)���O��
 * �����ꂩ�̊֐��𖈉�Ăяo�����ƂɂȂ�B
 */
//--------------------------------------------------------------------------------------------
// �J�����g�ˉe�s��Ɏˉe�ϊ��s�񂪁A
// �J�����g�ʒu���W�s��ƕ����x�N�g���s��ɃJ�����s��ƃ��f�����O�s�񂪍������ꂽ�s��
// ���ݒ肳��܂��B
void
	GFL_G3D_ObjDrawStart
		( void ) 
{
	NNS_G3dGlbFlush();
}

// �J�����g�ˉe�s��Ɏˉe�ϊ��s��ƃJ�����s�񂪍������ꂽ�s�񂪁A
// �J�����g�ʒu���W�s��ƕ����x�N�g���s��Ƀ��f�����O�s��
// ���ݒ肳��܂��B
void
	GFL_G3D_ObjDrawStartVP
		( void ) 
{
	NNS_G3dGlbFlushVP();
}

// �J�����g�ˉe�s��Ɏˉe�ϊ��s��ƃJ�����s��ƃ��f�����O�s�񂪍������ꂽ�s�񂪁A
// �J�����g�ʒu���W�s��ƕ����x�N�g���s��ɒP�ʍs��
// ���ݒ肳��܂��B
void
	GFL_G3D_ObjDrawStartWVP
		( void ) 
{
	NNS_G3dGlbFlushWVP();
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̕`��
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	rotate	��]�s��
 *
 * �ʏ�`��֐�
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjDraw
		( GFL_G3D_OBJ* g3Dobj, MtxFx33* rotate )
{
	G3DOBJ_DRAWCALC( &g3Dobj->trans, rotate, &g3Dobj->scale );

	NNS_G3dDraw( g3Dobj->rndobj );
	NNS_G3dGeFlushBuffer();
}
		
//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̕`��(1mat1shape)
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	rotate	��]�s��
 *
 * �P�̃��f���ɂP�̃}�e���A���̂ݐݒ肳��Ă���Ƃ��ɍ����`�悷�邽�߂̊֐�
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjDraw1mat1shape
		( GFL_G3D_OBJ* g3Dobj, MtxFx33* rotate )
{
	G3DOBJ_DRAWCALC( &g3Dobj->trans, rotate, &g3Dobj->scale );

    NNS_G3dDraw1Mat1Shp( g3Dobj->pMdl, 0, 0, TRUE );
	NNS_G3dGeFlushBuffer();
}
		
//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̉�]�s��̍쐬
 *
 * @param	g3Dobj	�R�c�I�u�W�F�N�g�n���h��
 * @param	dst		�v�Z��̉�]�s��i�[�|�C���^
 *
 * ���̊֐������g�p���A�I�u�W�F�N�g���ɓK�؂ȉ�]�s����쐬�������̂��A�`��ɗ����B
 */
//--------------------------------------------------------------------------------------------
// �w���x���y�̏��ԂŌv�Z
void
	GFL_G3D_ObjDrawRotateCalcXY
		( GFL_G3D_OBJ* g3Dobj, MtxFx33* rotate )
{
	VecFx32* src = &g3Dobj->rotate; 
	MtxFx33 tmp;

	MTX_RotY33(	rotate, FX_SinIdx((u16)src->x), FX_CosIdx((u16)src->x) );

	MTX_RotX33(	&tmp, FX_SinIdx((u16)src->y), FX_CosIdx((u16)src->y) );
	MTX_Concat33( rotate, &tmp, rotate );

	MTX_RotZ33(	&tmp, FX_SinIdx((u16)src->z), FX_CosIdx((u16)src->z) );
	MTX_Concat33( rotate, &tmp, rotate );
}

// �w���x���y�̏��ԂŌv�Z�i���΁j
void
	GFL_G3D_ObjDrawRotateCalcXY_Rev
		( GFL_G3D_OBJ* g3Dobj, MtxFx33* rotate )
{
	VecFx32* src = &g3Dobj->rotate; 
	MtxFx33 tmp;

	MTX_RotY33(	rotate, FX_SinIdx((u16)src->y), FX_CosIdx((u16)src->y) );

	MTX_RotX33(	&tmp, FX_SinIdx((u16)-src->x), FX_CosIdx((u16)-src->x) );
	MTX_Concat33( rotate, &tmp, rotate );

	MTX_RotZ33(	&tmp, FX_CosIdx((u16)src->z), FX_SinIdx((u16)src->z) );
	MTX_Concat33( rotate, &tmp, rotate );
}

// �x���w���y�̏��ԂŌv�Z
void
	GFL_G3D_ObjDrawRotateCalcYX
		( GFL_G3D_OBJ* g3Dobj, MtxFx33* rotate )
{
	VecFx32* src = &g3Dobj->rotate; 
	MtxFx33 tmp;

	MTX_RotY33(	rotate, FX_SinIdx((u16)src->y), FX_CosIdx((u16)src->y) );

	MTX_RotX33(	&tmp, FX_SinIdx((u16)src->x), FX_CosIdx((u16)src->x) );
	MTX_Concat33( rotate, &tmp, rotate );

	MTX_RotZ33(	&tmp, FX_SinIdx((u16)src->z), FX_CosIdx((u16)src->z) );
	MTX_Concat33( rotate,&tmp, rotate );
}

// �x���w���y�̏��ԂŌv�Z�i���΁j
void
	GFL_G3D_ObjDrawRotateCalcYX_Rev
		( GFL_G3D_OBJ* g3Dobj, MtxFx33* rotate )
{
	VecFx32* src = &g3Dobj->rotate; 
	MtxFx33 tmp;

	MTX_RotY33(	rotate, FX_SinIdx((u16)src->x), FX_CosIdx((u16)src->x) );

	MTX_RotX33(	&tmp, FX_SinIdx((u16)-src->y), FX_CosIdx((u16)-src->y) );
	MTX_Concat33( rotate, &tmp, rotate );

	MTX_RotZ33(	&tmp,FX_CosIdx((u16)src->z), FX_SinIdx((u16)src->z) );
	MTX_Concat33( rotate, &tmp, rotate );
}








