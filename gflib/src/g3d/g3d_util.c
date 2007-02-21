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
	RES_ON_REFERENCE	= 0x01,
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

#define RES_MDL_CHECK( res ) ( GFL_G3D_ResTypeCheck( res, GFL_G3D_RES_CHKTYPE_MDL ) )
#define RES_TEX_CHECK( res ) ( GFL_G3D_ResTypeCheck( res, GFL_G3D_RES_CHKTYPE_TEX ) )
			
static inline BOOL resourceLoad
				( const GFL_G3D_UTIL_SCENE_RES* resTbl, GFL_G3D_RES** p_g3Dres );
static inline BOOL resourceHandleGet
				( GFL_G3D_UTIL_SCENE* g3DutilScene, const GFL_G3D_UTIL_SCENE_SETUP* scene,
					GFL_G3D_RES** p_g3Dres, u16 idx );
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
	int						i,j;

	//�Ǘ��̈�m��
	g3DutilScene = GFL_HEAP_AllocMemoryClear( heapID, sizeof(GFL_G3D_UTIL_SCENE) );

	//���\�[�X�Ǘ��z��쐬
	g3DutilScene->g3DresTbl = GFL_HEAP_AllocMemoryClear( heapID, pHANDLE_SIZE * scene->resCount );
	g3DutilScene->g3DresReference = GFL_HEAP_AllocMemoryClear( heapID, scene->resCount );
	g3DutilScene->g3DresCount = scene->resCount;
	//���\�[�X�ǂݍ���
	for( i=0; i<scene->resCount; i++ ){
		resourceLoad( &scene->resTbl[i], &g3DutilScene->g3DresTbl[i] );
	}
	//���\�[�X�Q�ƃt���O������
	for( i=0; i<scene->objCount; i++ ){
		g3DutilScene->g3DresReference[i] = RES_NO_REFERENCE;
	}
	//�e�N�X�`���f�[�^�A���]���i�����Ɍ�����������������Ȃ���������Ă݂�j
	{
		GX_BeginLoadTex();
		for( i=0; i<scene->resCount; i++ ){
			if( RES_TEX_CHECK( g3DutilScene->g3DresTbl[i] ) == TRUE ){
				GFL_G3D_VramLoadTexDataOnly( g3DutilScene->g3DresTbl[i] );
			}
		}
		GX_EndLoadTex();
	}
	//�e�N�X�`���p���b�g�A���]���i�����Ɍ�����������������Ȃ���������Ă݂�j
	{
		GX_BeginLoadTexPltt();
		for( i=0; i<scene->resCount; i++ ){
			if( RES_TEX_CHECK( g3DutilScene->g3DresTbl[i] ) == TRUE ){
				GFL_G3D_VramLoadTexPlttOnly( g3DutilScene->g3DresTbl[i] );
			}
		}
		GX_EndLoadTexPltt();
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

		//���f�����\�[�X�̎Q�Ǝw��(���\�[�X�����L�o���Ȃ��̂Œǉ��ǂݍ��݃`�F�b�N���s��)
		if( resourceHandleGet(g3DutilScene,scene,&g3DresMdl,objTbl->mdlresID) == TRUE ){
			g3DutilScene->g3DobjExResourceRef[i] |= EXRES_MDL;	//�ǉ����\�[�X�t���O�Z�b�g
		}
		//�e�N�X�`�����\�[�X�̎Q�Ǝw��
		if( objTbl->texresID != objTbl->mdlresID ){
			//���f������\�[�X�̏ꍇ�͋��L�`�F�b�N���s��
			if( RES_MDL_CHECK( g3DutilScene->g3DresTbl[ objTbl->texresID ] ) == TRUE ){
				if( resourceHandleGet(g3DutilScene,scene,&g3DresTex,objTbl->texresID) == TRUE){
					g3DutilScene->g3DobjExResourceRef[i] |= EXRES_TEX;	//�ǉ����\�[�X�t���O�Z�b�g
					GFL_G3D_VramLoadTex( g3DresTex );
				}
			} else {
				g3DresTex = g3DutilScene->g3DresTbl[ objTbl->texresID ];
			}
		} else {
			//���f�����\�[�X�Ɠ����w��̏ꍇ�̓��f���ˑ��ŋ��L
			g3DresTex = g3DresMdl;
			//�e�N�X�`��VRAM���]���̏ꍇ�͓]������
			if( GFL_G3D_VramTexkeyLiveCheck( g3DresTex ) == FALSE ){
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

		//�ǉ��e�N�X�`�����\�[�X�쐬�t���O�m�F
		if( g3DutilScene->g3DobjExResourceRef[i] & (EXRES_TEX^0xff) ){
			//�e�N�X�`�����\�[�X���
			g3DresTex = GFL_G3D_RndG3DresTexGet( g3Drnd );
			GFL_G3D_VramUnloadTex( g3DresTex );
			GFL_G3D_ResDelete( g3DresTex );
		}
		//�ǉ����f�����\�[�X�쐬�t���O�m�F
		if( g3DutilScene->g3DobjExResourceRef[i] & (EXRES_MDL^0xff) ){
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
	for( i=0; i<g3DutilScene->g3DresCount; i++ ){
		if( g3DutilScene->g3DresTbl[i] ){
			if( RES_TEX_CHECK( g3DutilScene->g3DresTbl[i] ) == TRUE ){
				//�e�N�X�`�����\�[�X�̏ꍇ�u�q�`�l����i�����œ]���������̃`�F�b�N�͍s���Ă���j
				GFL_G3D_VramUnloadTex( g3DutilScene->g3DresTbl[i] ); 
			}
			GFL_G3D_ResDelete( g3DutilScene->g3DresTbl[i] ); 
		}
	}
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
static inline BOOL resourceLoad
				( const GFL_G3D_UTIL_SCENE_RES* resTbl, GFL_G3D_RES** p_g3Dres )
{
	if( resTbl->arcType == GFL_G3D_UTIL_RESARC ){
		//�A�[�J�C�u�h�c���
		*p_g3Dres = GFL_G3D_ResCreateArc( (int)resTbl->arcive, resTbl->datID ); 
		return TRUE;
	} else {			
		//�A�[�J�C�u�p�X���
		*p_g3Dres = GFL_G3D_ResCreatePath( (const char*)resTbl->arcive, resTbl->datID ); 
		return TRUE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�n���h�����擾�i�Q�ƍς݂̏ꍇ�͒ǉ��ǂݍ��݁j
 *	���L�ł��Ȃ����\�[�X�ݒ�̍ۂɎg�p
 */
//--------------------------------------------------------------------------------------------
static inline BOOL resourceHandleGet
					( GFL_G3D_UTIL_SCENE* g3DutilScene, const GFL_G3D_UTIL_SCENE_SETUP* scene,
						GFL_G3D_RES** p_g3Dres, u16 idx )
{
	GF_ASSERT( idx < scene->resCount );

	if( g3DutilScene->g3DresReference[ idx ] == RES_ON_REFERENCE ){
		resourceLoad( &scene->resTbl[ idx ], p_g3Dres );	//�ǉ��ǂݍ���
		return TRUE;
	} else {
		g3DutilScene->g3DresReference[ idx ] = RES_ON_REFERENCE;//�Q�ƍσt���O�Z�b�g
		*p_g3Dres = g3DutilScene->g3DresTbl[ idx ];
		return FALSE;
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



