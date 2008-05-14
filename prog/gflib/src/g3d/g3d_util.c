//=============================================================================================
/**
 * @file	g3d_util.c                                                  
 * @brief	�R�c�f�[�^�Ǘ����[�e�B���e�B�[�v���O����
 * @date	
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
#define UNIT_COUNT_MAX	(64)
#define UNIT_NULL		(0xffff)

typedef struct {
	u16	resIdx;
	u16	resCount;
	u16	objIdx;
	u16	objCount;

}GFL_G3D_UTIL_UNIT;

struct _GFL_G3D_UTIL {
	GFL_G3D_RES**		g3DresTbl;
	u8*					g3DresReference;
	u16					g3DresCount;

	GFL_G3D_OBJ**		g3DobjTbl;
	u8*					g3DobjExResourceRef;
	u16					g3DobjCount;

	GFL_G3D_UTIL_UNIT	unit[ UNIT_COUNT_MAX ];

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
#define RES_MDL_CHECK( res ) ( GFL_G3D_CheckResourceType( res, GFL_G3D_RES_CHKTYPE_MDL ) )
#define RES_TEX_CHECK( res ) ( GFL_G3D_CheckResourceType( res, GFL_G3D_RES_CHKTYPE_TEX ) )
			
static void addResource
	( GFL_G3D_UTIL* g3Dutil, u16 unitIdx, const GFL_G3D_UTIL_SETUP* setup, HEAPID heapID );
static void delResource
	( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
static void addObject
	( GFL_G3D_UTIL* g3Dutil, u16 unitIdx, const GFL_G3D_UTIL_SETUP* setup, HEAPID heapID );
static void delObject
	( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * �Z�b�g�A�b�v
 *
 * @param	resCountMax			�ݒ胊�\�[�X�ő吔��
 * @param	objCountMax			�ݒ�I�u�W�F�N�g�ő吔
 * @param	heapID				�q�[�v�h�c
 *
 * @return	GFL_G3D_UTIL*		�f�[�^�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_UTIL*
	GFL_G3D_UTIL_Create
		( u16 resCountMax, u16 objCountMax, HEAPID heapID )
{
	GFL_G3D_UTIL*			g3Dutil;
	int						i;

	//�Ǘ��̈�m��
	g3Dutil = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_G3D_UTIL) );

	//���\�[�X�Ǘ��z��쐬
	g3Dutil->g3DresTbl = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_G3D_RES*) * resCountMax );
	g3Dutil->g3DresReference = GFL_HEAP_AllocClearMemory( heapID, resCountMax );
	g3Dutil->g3DresCount = resCountMax;
	//���\�[�X�Q�ƃt���O������
	for( i=0; i<resCountMax; i++ ){
		g3Dutil->g3DresReference[i] = RES_NO_REFERENCE;
	}
	//�I�u�W�F�N�g�Ǘ��z��쐬
	g3Dutil->g3DobjTbl = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_G3D_OBJ*) * objCountMax );
	g3Dutil->g3DobjExResourceRef = GFL_HEAP_AllocClearMemory( heapID, objCountMax );
	g3Dutil->g3DobjCount = objCountMax;
	//�I�u�W�F�N�g�ǉ����\�[�X�쐬�t���O������
	for( i=0; i<objCountMax; i++ ){
		g3Dutil->g3DobjExResourceRef[i] = EXRES_NULL;
	}
#if 0
	addResource( g3Dutil, setup, heapID );
	addObject( g3Dutil, setup, heapID );
#else
	for( i=0; i<UNIT_COUNT_MAX; i++ ){
		g3Dutil->unit[i].resIdx = UNIT_NULL;
		g3Dutil->unit[i].resCount = UNIT_NULL;
		g3Dutil->unit[i].objIdx = UNIT_NULL;
		g3Dutil->unit[i].objCount = UNIT_NULL;
	}
#endif
	return g3Dutil;
}

//--------------------------------------------------------------------------------------------
/**
 * �j��
 *
 * @param	GFL_G3D_UTIL*		�f�[�^�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_UTIL_Delete
		( GFL_G3D_UTIL* g3Dutil )
{
#if 0
	delObject( g3Dutil );
	delResource( g3Dutil );
#else
	 int i;

	for( i=0; i<UNIT_COUNT_MAX; i++ ){
		if( g3Dutil->unit[i].resIdx != UNIT_NULL ){
			GF_ASSERT(0);
		}
	}
#endif
	//�I�u�W�F�N�g�Ǘ��z����
	GFL_HEAP_FreeMemory( g3Dutil->g3DobjExResourceRef );
	GFL_HEAP_FreeMemory( g3Dutil->g3DobjTbl );
	//���\�[�X�Ǘ��z��J��
	GFL_HEAP_FreeMemory( g3Dutil->g3DresReference );
	GFL_HEAP_FreeMemory( g3Dutil->g3DresTbl );
	
	//�Ǘ��̈���
	GFL_HEAP_FreeMemory( g3Dutil );
}

//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g�Z�b�g�A�b�v
 *
 * @param	GFL_G3D_UTIL*		�f�[�^�Z�b�g�n���h��
 * @param	setup				�ݒ�f�[�^
 *
 * @return	unitIdx				���j�b�g�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_UTIL_AddUnit
		( GFL_G3D_UTIL* g3Dutil, const GFL_G3D_UTIL_SETUP* setup )
{
	int unitIdx;
	u16 resIdx, objIdx;

	resIdx = 0;
	objIdx = 0;

	for( unitIdx=0; unitIdx<UNIT_COUNT_MAX; unitIdx++ ){
		if( g3Dutil->unit[unitIdx].resIdx == UNIT_NULL ){
			if( resIdx + setup->resCount > g3Dutil->g3DresCount ){
				GF_ASSERT(0);
			}
			if( objIdx + setup->objCount > g3Dutil->g3DobjCount ){
				GF_ASSERT(0);
			}
			g3Dutil->unit[unitIdx].resIdx =	resIdx; 
			g3Dutil->unit[unitIdx].resCount = setup->resCount;
			g3Dutil->unit[unitIdx].objIdx = objIdx;
			g3Dutil->unit[unitIdx].objCount = setup->objCount;

			addResource( g3Dutil, unitIdx, setup, g3Dutil->heapID );
			addObject( g3Dutil, unitIdx, setup, g3Dutil->heapID );
			return unitIdx;
		}
		resIdx = g3Dutil->unit[unitIdx].resIdx + g3Dutil->unit[unitIdx].resCount;
		objIdx = g3Dutil->unit[unitIdx].objIdx + g3Dutil->unit[unitIdx].objCount;
	}
	GF_ASSERT(0);
	return UNIT_NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g�j��
 *
 * @param	GFL_G3D_UTIL*		�f�[�^�Z�b�g�n���h��
 * @param	unitIdx				���j�b�g�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_UTIL_DelUnit
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx )
{
	if( g3Dutil->unit[unitIdx].resIdx == UNIT_NULL ){
		GF_ASSERT(0);
	}
	delObject( g3Dutil, unitIdx );
	delResource( g3Dutil, unitIdx );

	g3Dutil->unit[unitIdx].resIdx =	UNIT_NULL;
	g3Dutil->unit[unitIdx].resCount = UNIT_NULL;
	g3Dutil->unit[unitIdx].objIdx = UNIT_NULL;
	g3Dutil->unit[unitIdx].objCount = UNIT_NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�ǂݍ���
 */
//--------------------------------------------------------------------------------------------
static inline BOOL loadResource( const GFL_G3D_UTIL_RES* resTbl, GFL_G3D_RES** p_g3Dres )
{
	if( resTbl->arcType == GFL_G3D_UTIL_RESARC ){
		//�A�[�J�C�u�h�c���
		*p_g3Dres = GFL_G3D_CreateResourceArc( (int)resTbl->arcive, resTbl->datID ); 
		return TRUE;
	} else {			
		//�A�[�J�C�u�p�X���
		*p_g3Dres = GFL_G3D_CreateResourcePath( (const char*)resTbl->arcive, resTbl->datID ); 
		return TRUE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�n���h�����擾�i�Q�ƍς݂̏ꍇ�͒ǉ��ǂݍ��݁j
 *	���L�ł��Ȃ����\�[�X�ݒ�̍ۂɎg�p
 */
//--------------------------------------------------------------------------------------------
static inline BOOL getResourceHandle( GFL_G3D_UTIL* g3Dutil, const GFL_G3D_UTIL_SETUP* setup,
										GFL_G3D_RES** p_g3Dres, u16 idxoffs, u16 idx )
{
//	GF_ASSERT( idx < setup->resCount );

	if( g3Dutil->g3DresReference[ idxoffs+idx ] == RES_ON_REFERENCE ){
		loadResource( &setup->resTbl[ idx ], p_g3Dres );	//�ǉ��ǂݍ���
		return TRUE;
	} else {
		g3Dutil->g3DresReference[ idxoffs+idx ] = RES_ON_REFERENCE;//�Q�ƍσt���O�Z�b�g
		*p_g3Dres = g3Dutil->g3DresTbl[ idxoffs+idx ];
		return FALSE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �e�N�X�`���]��
 */
//--------------------------------------------------------------------------------------------
static inline void transTexture( GFL_G3D_RES* g3DresTex )
{
	//�e�N�X�`��VRAM���]���̏ꍇ�͓]������
	if( GFL_G3D_CheckTextureKeyLive( g3DresTex ) == FALSE ){
		GFL_G3D_TransVramTexture( g3DresTex );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�Z�b�g�A�b�v�ƍ폜
 */
//--------------------------------------------------------------------------------------------
static void addResource
	( GFL_G3D_UTIL* g3Dutil, u16 unitIdx, const GFL_G3D_UTIL_SETUP* setup, HEAPID heapID )
{
	int i;
	u16 resIdx = g3Dutil->unit[unitIdx].resIdx;
	u16 resCount = g3Dutil->unit[unitIdx].resCount;

	for( i=0; i<resCount; i++ ){
		//���\�[�X�ǂݍ���
		loadResource( &setup->resTbl[i], &g3Dutil->g3DresTbl[resIdx+i] );
	}
}

static void delResource
	( GFL_G3D_UTIL* g3Dutil, u16 unitIdx )
{
	int i;
	u16 resIdx = g3Dutil->unit[unitIdx].resIdx;
	u16 resCount = g3Dutil->unit[unitIdx].resCount;
	GFL_G3D_RES* g3Dres;

	//���\�[�X�j��
	for( i=0; i<resCount; i++ ){
		g3Dres = g3Dutil->g3DresTbl[resIdx+i];
		if( g3Dres ){
			if( RES_TEX_CHECK( g3Dres ) == TRUE ){
				//�e�N�X�`�����\�[�X�̏ꍇ�u�q�`�l����i�����œ]���������̃`�F�b�N�͍s���Ă���j
				GFL_G3D_FreeVramTexture( g3Dres ); 
			}
			GFL_G3D_DeleteResource( g3Dres ); 
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�Z�b�g�A�b�v�ƍ폜
 */
//--------------------------------------------------------------------------------------------
static void addObject
	( GFL_G3D_UTIL* g3Dutil, u16 unitIdx, const GFL_G3D_UTIL_SETUP* setup, HEAPID heapID )
{
	GFL_G3D_RES				*g3DresMdl, *g3DresTex, *g3DresAnm;
	GFL_G3D_RND*			g3Drnd;
	GFL_G3D_ANM*			g3Danm;
	GFL_G3D_ANM**			g3DanmTbl;
	const GFL_G3D_UTIL_OBJ*	objTbl;
	int						i,j;
	u16 resIdx = g3Dutil->unit[unitIdx].resIdx;
	u16 resCount = g3Dutil->unit[unitIdx].resCount;
	u16 objIdx = g3Dutil->unit[unitIdx].objIdx;
	u16 objCount = g3Dutil->unit[unitIdx].objCount;

	//�I�u�W�F�N�g�쐬
	for( i=0; i<objCount; i++ ){
		objTbl = &setup->objTbl[i];

		//���f�����\�[�X�̎Q�Ǝw��(���\�[�X�����L�o���Ȃ��̂Œǉ��ǂݍ��݃`�F�b�N���s��)
		if( getResourceHandle(g3Dutil, setup, &g3DresMdl, resIdx, objTbl->mdlresID ) == TRUE ){
			g3Dutil->g3DobjExResourceRef[resIdx+i] |= EXRES_MDL;	//�ǉ����\�[�X�t���O�Z�b�g
		}
		//�e�N�X�`�����\�[�X�̎Q�Ǝw��
		if( objTbl->texresID != objTbl->mdlresID ){
			//���f������\�[�X�̏ꍇ�͋��L�`�F�b�N���s��
			if( RES_MDL_CHECK( g3Dutil->g3DresTbl[ resIdx+objTbl->texresID ] ) == TRUE ){
				if( getResourceHandle(g3Dutil,setup,&g3DresTex,resIdx,objTbl->texresID) == TRUE){
					g3Dutil->g3DobjExResourceRef[resIdx+i] |= EXRES_TEX;//�ǉ����\�[�X�t���O�Z�b�g
					GFL_G3D_TransVramTexture( g3DresTex );
					//transTexture( g3DresTex );
				}
			} else {
				g3DresTex = g3Dutil->g3DresTbl[ resIdx+objTbl->texresID ];
				GFL_G3D_TransVramTexture( g3DresTex );
				//transTexture( g3DresTex );
			}
		} else {
			//���f�����\�[�X�Ɠ����w��̏ꍇ�̓��f���ˑ��ŋ��L
			g3DresTex = g3DresMdl;
			//�e�N�X�`��VRAM���]���̏ꍇ�͓]������
			transTexture( g3DresTex );
		}
		//�����_�[�쐬
		g3Drnd = GFL_G3D_RENDER_Create( g3DresMdl, objTbl->mdldatID, g3DresTex ); 

		//�A�j���[�V�����n���h���e���|�����쐬
		g3DanmTbl = GFL_HEAP_AllocClearMemoryLo( heapID, objTbl->anmCount * sizeof(GFL_G3D_ANM*) );

		//�A�j���[�V�����ݒ�
		for( j=0; j<objTbl->anmCount; j++ ){
			//�A�j���[�V�������\�[�X�̎Q�Ǝw��
			u16 anmresID = objTbl->anmTbl[j].anmresID;

			GF_ASSERT( (anmresID < resCount)||(anmresID == GFL_G3D_UTIL_NORESOURCE_ID) );
			if( anmresID == GFL_G3D_UTIL_NORESOURCE_ID ){
				g3Danm = NULL;	//�_�~�[�|�C���^
			} else {
				g3DresAnm = g3Dutil->g3DresTbl[ resIdx + anmresID ];
				//�A�j���[�V�����쐬
				g3Danm = GFL_G3D_ANIME_Create( g3Drnd, g3DresAnm, objTbl->anmTbl[j].anmdatID );
			}
			g3DanmTbl[j] = g3Danm;
		}
		//�I�u�W�F�N�g�쐬
		g3Dutil->g3DobjTbl[objIdx+i] = GFL_G3D_OBJECT_Create( g3Drnd, g3DanmTbl, objTbl->anmCount );

		GFL_HEAP_FreeMemory( g3DanmTbl );
	}
}

static void delObject
	( GFL_G3D_UTIL* g3Dutil, u16 unitIdx )
{
	GFL_G3D_RES			*g3DresMdl,	*g3DresTex, *g3DresAnm;
	GFL_G3D_RND*		g3Drnd;
	GFL_G3D_ANM*		g3Danm;
	GFL_G3D_OBJ*		g3Dobj;
	u16					anmCount;
	int					i,j;
	u16 resIdx = g3Dutil->unit[unitIdx].resIdx;
	u16 resCount = g3Dutil->unit[unitIdx].resCount;
	u16 objIdx = g3Dutil->unit[unitIdx].objIdx;
	u16 objCount = g3Dutil->unit[unitIdx].objCount;

	//�I�u�W�F�N�g�j��
	for( i=0; i<objCount; i++ ){
		g3Dobj = g3Dutil->g3DobjTbl[objIdx+i];
		g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( g3Dobj );

		//�A�j���[�V�������
		anmCount = GFL_G3D_OBJECT_GetAnimeCount( g3Dobj );
		for( j=0; j<anmCount; j++ ){
			GFL_G3D_ANIME_Delete( GFL_G3D_OBJECT_GetG3Danm( g3Dobj, j ) );
		}
		//�ǉ��e�N�X�`�����\�[�X�쐬�t���O�m�F
		if( g3Dutil->g3DobjExResourceRef[resIdx+i] & (EXRES_TEX^0xff) ){
			//�e�N�X�`�����\�[�X���
			g3DresTex = GFL_G3D_RENDER_GetG3DresTex( g3Drnd );
			GFL_G3D_FreeVramTexture( g3DresTex );
			GFL_G3D_DeleteResource( g3DresTex );
		}
		//�ǉ����f�����\�[�X�쐬�t���O�m�F
		if( g3Dutil->g3DobjExResourceRef[resIdx+i] & (EXRES_MDL^0xff) ){
			//���f�����\�[�X���
			g3DresMdl = GFL_G3D_RENDER_GetG3DresMdl( g3Drnd );
			GFL_G3D_DeleteResource( g3DresMdl );
		}	
		//�����_�[���
		GFL_G3D_RENDER_Delete( g3Drnd ); 

		//�I�u�W�F�N�g���
		GFL_G3D_OBJECT_Delete( g3Dobj );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g���\�[�X�擪�C���f�b�N�X�擾
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_UTIL_GetUnitResIdx
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx )
{
	return g3Dutil->unit[unitIdx].resIdx;
}

//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g���\�[�X���擾
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_UTIL_GetUnitResCount
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx )
{
	return g3Dutil->unit[unitIdx].resCount;
}

//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g�I�u�W�F�N�g�擪�C���f�b�N�X�擾
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_UTIL_GetUnitObjIdx
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx )
{
	return g3Dutil->unit[unitIdx].objIdx;
}

//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g�I�u�W�F�N�g���擾
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_UTIL_GetUnitObjCount
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx )
{
	return g3Dutil->unit[unitIdx].objCount;
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�n���h���擾
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RES*
	GFL_G3D_UTIL_GetResHandle
		( GFL_G3D_UTIL* g3Dutil, u16 idx )
{
	return g3Dutil->g3DresTbl[idx];
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�n���h���擾
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_OBJ*
	GFL_G3D_UTIL_GetObjHandle
		( GFL_G3D_UTIL* g3Dutil, u16 idx )
{
	return g3Dutil->g3DobjTbl[idx];
}

//--------------------------------------------------------------------------------------------
/**
 * ���I�u�W�F�N�g���擾
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_UTIL_GetObjCount
		( GFL_G3D_UTIL* g3Dutil )
{
	int i;
	u16 count = 0;

	for( i=0; i<UNIT_COUNT_MAX; i++ ){
		if( g3Dutil->unit[i].resIdx != UNIT_NULL ){
			count += g3Dutil->unit[i].objCount;
		}
	}
	return count;
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�ɃA�j���[�V������ǉ��ݒ肷��i�ʃ��j�b�g�̃A�j����ݒ肷��Ȃǁj
 */
//--------------------------------------------------------------------------------------------
BOOL
	GFL_G3D_UTIL_SetObjAnotherUnitAnime
		( GFL_G3D_UTIL* g3Dutil, u16 objUnitIdx, u16 objIdx, 
			u16 anmUnitIdx, const GFL_G3D_UTIL_ANM* anmTbl, u16 anmCount )
{
	GFL_G3D_OBJ*	g3Dobj;
	GFL_G3D_RND*	g3Drnd;
	GFL_G3D_ANM*	g3Danm;
	u16				anmresID;
	int i;
	
	GF_ASSERT( objUnitIdx + objIdx <  g3Dutil->g3DobjCount );
	g3Dobj = g3Dutil->g3DobjTbl[ objUnitIdx + objIdx ];
	g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( g3Dobj ); 
 
	//�A�j���[�V�����ݒ�
	for( i=0; i<anmCount; i++ ){
		//�A�j���[�V�������\�[�X�̎Q�Ǝw��
		anmresID = anmTbl[i].anmresID;

		GF_ASSERT( (anmresID < g3Dutil->unit[anmUnitIdx].resCount)
					||(anmresID == GFL_G3D_UTIL_NORESOURCE_ID) );

		if( anmresID == GFL_G3D_UTIL_NORESOURCE_ID ){
			g3Danm = NULL;	//�_�~�[�|�C���^
		} else {
			//�A�j���[�V�����쐬
			g3Danm = GFL_G3D_ANIME_Create(	g3Drnd, 
											g3Dutil->g3DresTbl[ anmUnitIdx + anmresID ], 
											anmTbl[i].anmdatID );
		}
		GFL_G3D_OBJECT_AddAnime( g3Dobj, g3Danm );
	}

	return TRUE;
}

