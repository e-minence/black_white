//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_ground_anime.c
 *	@brief	�t�B�[���h�n�ʃA�j���[�V�����Ǘ��V�X�e��
 *	@author	tomoya takahshi
 *	@data		2009.04.30
 *
 *	���W���[�����FFIELD_GRANM
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "field_ground_anime.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�n�ʃA�j���[�V�������[�N
// 
//=====================================
//NNSG3dAnmObj�̃������T�C�Y	�v���`�i�̂P�u���b�N���̃}�e���A���ő吔��
#define FIELD_GRANM_WORK_ANIMEOBJ_WORK_MATNODE_MAX	( 40 )
#define FIELD_GRANM_WORK_ANIMEOBJ_WORK_SIZE	( sizeof(NNSG3dAnmObj) + (2*FIELD_GRANM_WORK_ANIMEOBJ_WORK_MATNODE_MAX) )


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�t�B�[���h�n�ʃA�j���[�V�������[�N
//=====================================
struct _FIELD_GRANM_WORK
{
	u32 bind:1;									// �����_�[�I�u�W�F�Ɋ֘A�t�����Ă��邩�B
	u32 pad:31;									// �\��
	NNSG3dAnmObj* p_anmobj;			// �A�j���[�V�����I�u�W�F�N�g
	void*					p_anmres;			// �A�j���[�V�������\�[�X
};


//-------------------------------------
///	�t�B�[���h�n�ʃA�j���[�V�����Ǘ��V�X�e��
//=====================================
struct _FIELD_GRANM 
{
	FIELD_GRANM_WORK* p_wkbuf;
	u16								wkmax;
	
	// �A�j���[�V�����t���[�����
	u16								anime_flag;
	fx32							anime_frame;
	fx32							anime_speed;

	// �G���A�A�j�����
	void* p_anmfile;
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�t�B�[���h�n�ʃA�j���[�V�������[�N
//=====================================
static void FIELD_GRANM_Work_Init( FIELD_GRANM_WORK* p_wk, void* p_anmres, u32 heapID );
static void FIELD_GRANM_Work_Exit( FIELD_GRANM_WORK* p_wk );
static void FIELD_GRANM_Work_Bind( FIELD_GRANM_WORK* p_wk, const GFL_G3D_RES* cp_mdlres, const GFL_G3D_RES* cp_texres, NNSG3dRenderObj* p_rendobj );
static void FIELD_GRANM_Work_Release( FIELD_GRANM_WORK* p_wk );
static fx32 FIELD_GRANM_Work_SetAnimeFrame( FIELD_GRANM_WORK* p_wk, fx32 frame );



//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���Ǘ��V�X�e���@	����
 *
 *	@param	arcID					�A�j�����\�[�X�A�[�J�C�uID
 *	@param	dataID				�A�j�����\�[�X�A�[�J�C�u�f�[�^ID
 *	@param	block_num			�u���b�N��
 *	@param	heapID				�q�[�vID
 *
 *	@return	�n�ʃA�j���Ǘ��V�X�e��
 */
//-----------------------------------------------------------------------------
FIELD_GRANM* FIELD_GRANM_Create( u32 arcID, u32 dataID, u32 block_num, u32 heapID )
{
	FIELD_GRANM* p_sys;
	int i;
	void* p_anmres;

	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_GRANM) );

	// �A�j���[�V�����t�@�C���ǂݍ���
	p_sys->p_anmfile = GFL_ARC_UTIL_Load( arcID, dataID, FALSE, heapID );
	p_anmres = NNS_G3dGetAnmByIdx( p_sys->p_anmfile, 0 );
	
	// �n�ʃA�j���Ǘ����[�N�o�b�t�@�쐬
	p_sys->p_wkbuf	= GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_GRANM_WORK)*block_num );
	p_sys->wkmax		= block_num;
	for( i=0; i<p_sys->wkmax; i++ )
	{
		FIELD_GRANM_Work_Init( &p_sys->p_wkbuf[i], p_anmres, heapID );
	}

	// �I�[�g�A�j���[�V�����t���O�ݒ�
	p_sys->anime_flag		= TRUE;
	p_sys->anime_speed	= FX32_ONE;
	
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�����V�X�e���j��
 *
 *	@param	p_sys			�V�X�e��
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_Delete( FIELD_GRANM* p_sys )
{
	int i;

	for( i=0; i<p_sys->wkmax; i++ )
	{
		FIELD_GRANM_Work_Exit( &p_sys->p_wkbuf[i] );
	}
	GFL_HEAP_FreeMemory( p_sys->p_wkbuf );
	GFL_HEAP_FreeMemory( p_sys->p_anmfile );
	GFL_HEAP_FreeMemory( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�����V�X�e���@���C��
 *
 *	@param	p_sys			�V�X�e��
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_Main( FIELD_GRANM* p_sys )
{
	int i;

	// �I�[�g�A�j���[�V��������
	if( p_sys->anime_flag )
	{
		p_sys->anime_frame += p_sys->anime_speed;
	}

	// �t���[�����ݒ�
	for( i=0; i<p_sys->wkmax; i++ )
	{
		// FIELD_GRANM_Work_SetAnimeFrame�̒��ŁA
		// �A�j���[�V�����t���[���̍ő�l�ŁA�l�����[�v�����Ă��܂��B
		p_sys->anime_frame = FIELD_GRANM_Work_SetAnimeFrame( &p_sys->p_wkbuf[i], p_sys->anime_frame );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�����V�X�e���@�n�ʃA�j���[�V�������[�N�̎擾
 *
 *	@param	cp_sys		�V�X�e�����[�N
 *	@param	idx				�n�ʃu���b�N�C���f�b�N�X
 *
 *	@return	�n�ʃA�j���[�V�������[�N
 */
//-----------------------------------------------------------------------------
FIELD_GRANM_WORK* FIELD_GRANM_GetWork( const FIELD_GRANM* cp_sys, u32 idx )
{
	GF_ASSERT( cp_sys );
	GF_ASSERT( idx < cp_sys->wkmax );

	return &cp_sys->p_wkbuf[idx];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����X�s�[�h�ݒ�
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	speed			�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_SetAnimeSpeed( FIELD_GRANM* p_sys, fx32 speed )
{
	p_sys->anime_speed = speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����X�s�[�h�̎擾
 *
 *	@param	cp_sys	�V�X�e�����[�N
 *
 *	@return	�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
fx32 FIELD_GRANM_GetAnimeSpeed( const FIELD_GRANM* cp_sys )
{
	return cp_sys->anime_speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����t���O�̐ݒ�
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	flag		�t���O
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_SetAutoAnime( FIELD_GRANM* p_sys, BOOL flag )
{
	p_sys->anime_flag = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�g�A�j���[�V�����t���O�̎擾
 *
 *	@param	cp_sys	�V�X�e�����[�N
 *		
 *	@retval	TRUE	�I�[�g�A�j���[�V������
 *	@retval	FALSE	�A�j���[�V������~��
 */
//-----------------------------------------------------------------------------
BOOL FIELD_GRANM_GetAutoAnime( const FIELD_GRANM* cp_sys )
{
	return  cp_sys->anime_flag;
}



//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�������[�N�@�����_�[�I�u�W�F�Ɋ֘A�t����
 *
 *	@param	p_wk				���[�N
 *	@param	cp_mdlres		���f�����\�[�X
 *	@param	cp_texres		�e�N�X�`�����\�[�X					itp�ȊO��NULL���\
 *	@param	p_rendobj		�֘A�t���郌���_�[�I�u�W�F
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_WORK_Bind( FIELD_GRANM_WORK* p_wk, const GFL_G3D_RES* cp_mdlres, const GFL_G3D_RES* cp_texres, NNSG3dRenderObj* p_rendobj )
{
	FIELD_GRANM_Work_Bind( p_wk, cp_mdlres, cp_texres, p_rendobj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�������[�N	�����_�[�I�u�W�F�Ƃ̊֘A�t��������
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_GRANM_WORK_Release( FIELD_GRANM_WORK* p_wk )
{
	FIELD_GRANM_Work_Release( p_wk );
}





//-----------------------------------------------------------------------------
/**
 *				�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�������[�N�̏�����
 *
 *	@param	p_wk				���[�N
 *	@param	p_anmres		�A�j���[�V�������\�[�X
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Work_Init( FIELD_GRANM_WORK* p_wk, void* p_anmres, u32 heapID )
{
	GF_ASSERT( p_wk->p_anmobj == NULL );
	p_wk->p_anmobj = GFL_HEAP_AllocClearMemory( heapID, FIELD_GRANM_WORK_ANIMEOBJ_WORK_SIZE );	
	p_wk->p_anmres = p_anmres;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�������[�N�̔j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Work_Exit( FIELD_GRANM_WORK* p_wk )
{
	GF_ASSERT( p_wk->p_anmobj );
	GFL_HEAP_FreeMemory( p_wk->p_anmobj );
	p_wk->p_anmobj = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�������[�N�@�����_�[�I�u�W�F�Ɋ֘A�t����
 *
 *	@param	p_wk					���[�N
 *	@param	cp_mdlres			���f�����\�[�X
 *	@param	cp_texres			�e�N�X�`�����\�[�X					itp�ȊO��NULL���\
 *	@param	p_rendobj			�֘A�t���郌���_�[�I�u�W�F
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Work_Bind( FIELD_GRANM_WORK* p_wk, const GFL_G3D_RES* cp_mdlres, const GFL_G3D_RES* cp_texres, NNSG3dRenderObj* p_rendobj )
{
	const NNSG3dResMdl* cp_mdl;
	const NNSG3dResTex* cp_tex;

	GF_ASSERT( cp_mdlres );
	GF_ASSERT( p_rendobj );

	
	// ���f�����擾
	cp_mdl = NNS_G3dGetMdlByIdx( NNS_G3dGetMdlSet( GFL_G3D_GetResourceFileHeader( cp_mdlres ) ), 0 );

	// ���f�����}�e���A�����`�F�b�N
	GF_ASSERT( cp_mdl->info.numMat < FIELD_GRANM_WORK_ANIMEOBJ_WORK_MATNODE_MAX );

	if( cp_texres )
	{
		cp_tex = GFL_G3D_GetResTex( cp_texres );
	}
	else
	{
		cp_tex = NULL;
	}
	
	// �A�j���[�V�����I�u�W�F�̏�����
	GFL_STD_MemClear( p_wk->p_anmobj, FIELD_GRANM_WORK_ANIMEOBJ_WORK_SIZE );
	NNS_G3dAnmObjInit( p_wk->p_anmobj, p_wk->p_anmres, cp_mdl, cp_tex );

	// �����_�[�I�u�W�F�Ɋ֘A�t��
	NNS_G3dRenderObjAddAnmObj( p_rendobj, p_wk->p_anmobj );

	p_wk->bind = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�������[�N	�����_�[�I�u�W�F�Ƃ̊֘A�t������
 *
 *	@param	p_wk			���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Work_Release( FIELD_GRANM_WORK* p_wk )
{
	p_wk->bind = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�������[�N�@�t���[���ݒ�
 *
 *	@param	p_wk			���[�N
 *	@param	frame			�t���[��
 *
 *	@return	�ݒ�t���[��
 */
//-----------------------------------------------------------------------------
static fx32 FIELD_GRANM_Work_SetAnimeFrame( FIELD_GRANM_WORK* p_wk, fx32 frame )
{
	if( p_wk->bind )
	{
		fx32 max_frame;
		
		max_frame = NNS_G3dAnmObjGetNumFrame( p_wk->p_anmobj );
		frame			= frame % max_frame;
		
		NNS_G3dAnmObjSetFrame( p_wk->p_anmobj, frame );
	}

	return frame;
}



