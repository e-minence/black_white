//=============================================================================================
/**
 * @file	g3d_util.c                                                  
 * @brief	�R�c�Ǘ��V�X�e���v���O����
 * @date	
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
struct _GFL_G3D_UTIL_SCENE {
	GFL_G3D_RES**		g3DresTbl;
	u8*					g3DresReference;
	u16					g3DresCount;

	GFL_G3D_OBJ**		g3DobjTbl;
	u8*					g3DobjExResourceRef;
	u16					g3DobjCount;

	HEAPID				heapID;
};

enum {
	RES_NO_REFERENCE	= 0x00,
	RES_REFERENCE		= 0x01,
};

enum {
	EXRES_NULL			= 0x00,
	EXRES_MDL			= 0x01,
	EXRES_TEX			= 0x02,
};

//=============================================================================================
/**
 *
 *
 * �R�c�֘A�n���h���̊Ǘ����[�e�B���e�B�[
 *
 *
 */
//=============================================================================================
#define pHANDLE_SIZE (4)

static inline	GFL_G3D_RES* resourceLoad( const GFL_G3D_UTIL_SCENE_RES* resTbl );
static void		resourceLoadAll
			( GFL_G3D_RES** g3DresTbl, const GFL_G3D_UTIL_SCENE_SETUP* scene ); 
static void		resourceUnloadAll
			( GFL_G3D_RES** g3DresTbl, const u16 resCount );
//--------------------------------------------------------------------------------------------
/**
 * �Z�b�g�A�b�v
 *
 * @param	scene					�ݒ�f�[�^
 * @param	heapID					�q�[�v�h�c
 *
 * @return	GFL_G3D_UTIL_SCENE*		�V�[���n���h��
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_UTIL_SCENE*
	GFL_G3D_UtilsysCreate
		( const GFL_G3D_UTIL_SCENE_SETUP* scene, HEAPID heapID )
{
	GFL_G3D_UTIL_SCENE*		g3DutilScene;
	const GFL_G3D_UTIL_SCENE_OBJ*	objTbl;
	GFL_G3D_RES				*g3DresMdl, *g3DresTex, *g3DresAnm;
	GFL_G3D_RND*			g3Drnd;
	GFL_G3D_ANM*			g3Danm;
	GFL_G3D_ANM**			g3DanmTbl;
	int						i,j,idx;

	//�Ǘ��̈�m��
	g3DutilScene = GFL_HEAP_AllocMemoryClear( heapID, sizeof(GFL_G3D_UTIL_SCENE) );

	//���\�[�X�Ǘ��z��쐬
	g3DutilScene->g3DresTbl = GFL_HEAP_AllocMemoryClear( heapID, pHANDLE_SIZE * scene->resCount );
	g3DutilScene->g3DresReference = GFL_HEAP_AllocMemoryClear( heapID, scene->resCount );
	g3DutilScene->g3DresCount = scene->resCount;
	//���\�[�X�ǂݍ���
	resourceLoadAll( g3DutilScene->g3DresTbl, scene );
	//���\�[�X�Q�ƃt���O������
	for( i=0; i<scene->objCount; i++ ){
		g3DutilScene->g3DresReference[i] = RES_NO_REFERENCE;
	}

	//�I�u�W�F�N�g�Ǘ��z��쐬
	g3DutilScene->g3DobjTbl = GFL_HEAP_AllocMemoryClear( heapID, pHANDLE_SIZE * scene->objCount );
	g3DutilScene->g3DobjExResourceRef = GFL_HEAP_AllocMemoryClear( heapID, scene->objCount );
	g3DutilScene->g3DobjCount = scene->objCount;
	//�I�u�W�F�N�g�ǉ����\�[�X�쐬�t���O������
	for( i=0; i<scene->objCount; i++ ){
		g3DutilScene->g3DobjExResourceRef[i] = EXRES_NULL;
	}

	//�I�u�W�F�N�g�쐬
	for( i=0; i<scene->objCount; i++ ){
		objTbl = &scene->objTbl[i];

		//���f�����\�[�X�̎Q�Ǝw��
		idx = objTbl->mdlresID;
		GF_ASSERT( idx < scene->resCount );
		if( g3DutilScene->g3DresReference[ idx ] == RES_REFERENCE ){
			//���f�����\�[�X�͋��L�ł��Ȃ��̂Œǉ��쐬����
			g3DutilScene->g3DobjExResourceRef[i] |= EXRES_MDL;	//�ǉ����\�[�X�쐬�t���O�Z�b�g
			g3DresMdl = resourceLoad( &scene->resTbl[ idx ] );
		} else {
			g3DutilScene->g3DresReference[ idx ] = RES_REFERENCE;		//�Q�ƍσt���O�Z�b�g
			g3DresMdl = g3DutilScene->g3DresTbl[ idx ];
		}

		//�e�N�X�`�����\�[�X�̎Q�Ǝw��
		idx = objTbl->texresID;
		GF_ASSERT( objTbl->texresID < scene->resCount );
		if( objTbl->texresID != objTbl->mdlresID ){
			if( GFL_G3D_ResTypeCheck
				( g3DutilScene->g3DresTbl[ idx ], GFL_G3D_RES_CHKTYPE_MDL ) == TRUE ){
				//���f������\�[�X�̏ꍇ�͋��L�`�F�b�N���s��
				if( g3DutilScene->g3DresReference[ idx ] == RES_REFERENCE ){
					//���f�����\�[�X�͋��L�ł��Ȃ��̂Œǉ��쐬����
					g3DutilScene->g3DobjExResourceRef[i] |= EXRES_TEX;//�ǉ����\�[�X�쐬�t���O�Z�b�g
					g3DresTex = resourceLoad( &scene->resTbl[ idx ] );
					GFL_G3D_VramLoadTex( g3DresTex );
				} else {
					g3DutilScene->g3DresReference[ idx ] |= RES_REFERENCE;	//�Q�ƍσt���O�Z�b�g
					g3DresTex = g3DutilScene->g3DresTbl[ idx ];
				}
			}
		} else {
			//���f�����\�[�X�Ɠ����w��̏ꍇ�̓��f���ˑ��ŋ��L
			g3DresTex = g3DresMdl;
			if( GFL_G3D_VramTexkeyLiveCheck( g3DresTex ) == FALSE ){
				//�e�N�X�`�����]���̏ꍇ�͓]������
				GFL_G3D_VramLoadTex( g3DresTex );
			}
		}
		//�����_�[�쐬
		g3Drnd = GFL_G3D_RndCreate( g3DresMdl, objTbl->mdldatID, g3DresTex ); 

		//�A�j���[�V�����n���h���e���|�����쐬
		g3DanmTbl = GFL_HEAP_AllocMemoryLowClear( heapID, objTbl->anmCount );

		//�A�j���[�V�����ݒ�
		for( j=0; j<objTbl->anmCount; j++ ){
			//�A�j���[�V�������\�[�X�̎Q�Ǝw��
			GF_ASSERT( objTbl->anmTbl[j].anmresID < scene->resCount );
			g3DresAnm = g3DutilScene->g3DresTbl[ objTbl->anmTbl[j].anmresID ];
			//�A�j���[�V�����쐬
			g3Danm = GFL_G3D_AnmCreate( g3Drnd, g3DresAnm, objTbl->anmTbl[j].anmdatID );
			g3DanmTbl[j] = g3Danm;
		}
		//�I�u�W�F�N�g�쐬
		g3DutilScene->g3DobjTbl[i] = GFL_G3D_ObjCreate( g3Drnd, g3DanmTbl, objTbl->anmCount );

		GFL_HEAP_FreeMemory( g3DanmTbl );
	}
	return g3DutilScene;
}

//--------------------------------------------------------------------------------------------
/**
 * �j��
 *
 * @param	GFL_G3D_UTIL_SCENE*		�V�[���n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_UtilsysDelete
		( GFL_G3D_UTIL_SCENE* g3DutilScene )
{
	GFL_G3D_RES			*g3DresMdl,	*g3DresTex, *g3DresAnm;
	GFL_G3D_RND*		g3Drnd;
	GFL_G3D_ANM*		g3Danm;
	GFL_G3D_OBJ*		g3Dobj;
	int					i,j;

	//�I�u�W�F�N�g�j��
	for( i=0; i<g3DutilScene->g3DobjCount; i++ ){
		g3Dobj = g3DutilScene->g3DobjTbl[i];
		g3Drnd = GFL_G3D_ObjG3DrndGet( g3Dobj );

		if( g3DutilScene->g3DobjExResourceRef[i] & (EXRES_TEX^0xff) ){//�ǉ����\�[�X�쐬�t���O�m�F
			//�e�N�X�`�����\�[�X���
			g3DresTex = GFL_G3D_RndG3DresTexGet( g3Drnd );
			GFL_G3D_VramUnloadTex( g3DresTex );
			GFL_G3D_ResDelete( g3DresTex );
		}

		if( g3DutilScene->g3DobjExResourceRef[i] & (EXRES_MDL^0xff) ){//�ǉ����\�[�X�쐬�t���O�m�F
			//���f�����\�[�X���
			g3DresMdl = GFL_G3D_RndG3DresMdlGet( g3Drnd );
			GFL_G3D_ResDelete( g3DresMdl );
		}	
		//�����_�[���
		GFL_G3D_RndDelete( g3Drnd ); 

		//�I�u�W�F�N�g���
		GFL_G3D_ObjDelete( g3DutilScene->g3DobjTbl[i] );
	}
	//�I�u�W�F�N�g�Ǘ��z����
	GFL_HEAP_FreeMemory( g3DutilScene->g3DobjExResourceRef );
	GFL_HEAP_FreeMemory( g3DutilScene->g3DobjTbl );

	//���\�[�X�j��
	resourceUnloadAll( g3DutilScene->g3DresTbl, g3DutilScene->g3DresCount );
	//���\�[�X�Ǘ��z��쐬
	GFL_HEAP_FreeMemory( g3DutilScene->g3DresReference );
	GFL_HEAP_FreeMemory( g3DutilScene->g3DresTbl );
	
	//�Ǘ��̈���
	GFL_HEAP_FreeMemory( g3DutilScene );
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�ǂݍ���
 */
//--------------------------------------------------------------------------------------------
static inline GFL_G3D_RES* resourceLoad( const GFL_G3D_UTIL_SCENE_RES* resTbl )
{
	if( resTbl->arcType == GFL_G3D_UTIL_RESARC ){
		//�A�[�J�C�u�h�c���
		return GFL_G3D_ResCreateArc( (int)resTbl->arcive, resTbl->datID ); 
	} else {			
		//�A�[�J�C�u�p�X���
		return GFL_G3D_ResCreatePath( (const char*)resTbl->arcive, resTbl->datID ); 
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�ݒ�
 */
//--------------------------------------------------------------------------------------------
//���\�[�X��z��ɒǉ�
static void resourceLoadAll
			( GFL_G3D_RES** g3DresTbl, const GFL_G3D_UTIL_SCENE_SETUP* scene )  
{
	int	i;

	//�z��ɃZ�b�g�A�b�v
	for( i=0; i<scene->resCount; i++ ){
		g3DresTbl[i] = resourceLoad( &scene->resTbl[i] );
	}

	//�e�N�X�`���f�[�^�A���]���i�����Ɍ�����������������Ȃ���������Ă݂�j
	GX_BeginLoadTex();
	for( i=0; i<scene->resCount; i++ ){
		if( GFL_G3D_ResTypeCheck( g3DresTbl[i], GFL_G3D_RES_CHKTYPE_TEX ) == TRUE ){
			GFL_G3D_VramLoadTexDataOnly( g3DresTbl[i] );
		}
	}
	GX_EndLoadTex();

	//�e�N�X�`���p���b�g�A���]���i�����Ɍ�����������������Ȃ���������Ă݂�j
	GX_BeginLoadTexPltt();
	for( i=0; i<scene->resCount; i++ ){
		if( GFL_G3D_ResTypeCheck( g3DresTbl[i], GFL_G3D_RES_CHKTYPE_TEX ) == TRUE ){
			GFL_G3D_VramLoadTexPlttOnly( g3DresTbl[i] );
		}
	}
	GX_EndLoadTexPltt();
}

//���\�[�X��z�񂩂�폜
static void resourceUnloadAll
			( GFL_G3D_RES** g3DresTbl, const u16 resCount )  
{
	int	i;

	for( i=0; i<resCount; i++ ){
		if( g3DresTbl[i] ){
			if( GFL_G3D_ResTypeCheck( g3DresTbl[i], GFL_G3D_RES_CHKTYPE_TEX ) == TRUE ){
				//�e�N�X�`�����\�[�X�̏ꍇ�u�q�`�l����i�����œ]���������̃`�F�b�N�͍s���Ă���j
				GFL_G3D_VramUnloadTex( g3DresTbl[i] ); 
			}
			GFL_G3D_ResDelete( g3DresTbl[i] ); 
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�n���h���擾
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_OBJ*
	GFL_G3D_UtilsysObjHandleGet
		( GFL_G3D_UTIL_SCENE* g3DutilScene, u16 idx )
{
	return g3DutilScene->g3DobjTbl[idx];
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g���擾
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_UtilsysObjCountGet
		( GFL_G3D_UTIL_SCENE* g3DutilScene )
{
	return g3DutilScene->g3DobjCount;
}



