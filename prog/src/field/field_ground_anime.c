//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_ground_anime.c
 *	@brief	�t�B�[���h�n�ʃA�j���[�V�����Ǘ��V�X�e��
 *	@author	tomoya takahshi
 *	@date		2009.04.30
 *
 *	���W���[�����FFIELD_GRANM
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "field_ground_anime.h"
#include "texanm_sys.h"


#ifdef PM_DEBUG

#ifdef DEBUG_ONLY_FOR_tomoya_takahashi
#define DEBUG_FIELD_GRANM_MEM_PRINT   // �������g�p�ʃ`�F�b�N
#endif

#ifdef DEBUG_ONLY_FOR_fuchino
#define DEBUG_FIELD_GRANM_MEM_PRINT   // �������g�p�ʃ`�F�b�N
#endif

#endif

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
#define FIELD_GRANM_WORK_ANIMEOBJ_WORK_MATNODE_MAX	( 50 )
#define FIELD_GRANM_WORK_ANIMEOBJ_WORK_SIZE	( sizeof(NNSG3dAnmObj) + (2*FIELD_GRANM_WORK_ANIMEOBJ_WORK_MATNODE_MAX) )


//-------------------------------------
///	ITP�A�j���[�V����
//=====================================
#define FIELD_GRANM_ITP_TRANS_ADDR_NONE	( 0xffffffff )
#define FIELD_GRANM_ITP_NUM (16)
#define FIELD_GRANM_ITP_TBL_NONE	( 0xff )



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
///	ITA�A�j���[�V�����Ǘ�
//=====================================
typedef struct 
{
	FIELD_GRANM_WORK* p_wkbuf;
	u32								wkmax;

	// �G���A�A�j�����
	void* p_anmfile;


  // �t���[����
	fx32	anime_speed;

} FIELD_GRANM_ITA;


//-------------------------------------
///	ITP�A�j���[�V�����Ǘ�
//=====================================
typedef struct
{
  u8  itp[ FIELD_GRANM_ITP_NUM ]; 
  u8  tex[ FIELD_GRANM_ITP_NUM ]; 
} FIELD_GRANM_ITP_TBL;

//-------------------------------------
///	ITP�A�j���[�V�����Ǘ�
//=====================================
typedef struct 
{

	// �P�O�̃t���[�����
	BOOL trans;
	TEXANM_DATA*	p_last_data;
	u32*					p_trans_addr;
	u32*					p_trans_size;
	fx32*					p_anime_frame;
	u32						anime_num;

	// �n�ʃe�N�X�`��
	const GFL_G3D_RES* cp_ground_tex;
	
	// �G���A�A�j�����
	void* p_anmfile;
	TEXANM_DATATBL anmtbl;
	GFL_G3D_RES* p_tex;


} FIELD_GRANM_ITP;

// �Ǘ��V�X�e��
typedef struct
{
  FIELD_GRANM_ITP wk[ FIELD_GRANM_ITP_NUM ];
}FIELD_GRANM_ITP_SYS;

//-------------------------------------
///	�t�B�[���h�n�ʃA�j���[�V�����Ǘ��V�X�e��
//=====================================
struct _FIELD_GRANM 
{
	
	// �A�j���[�V�����t���[�����
	u32								anime_flag;
	fx32							anime_speed;

	// ITA�A�j���[�V�����Ǘ�
	FIELD_GRANM_ITA	ita_anime;

	// ITP�A�j���[�V�����Ǘ�
	FIELD_GRANM_ITP_SYS	itp_anime;
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	ITA�A�j���[�V�����Ǘ�
//=====================================
static void FIELD_GRANM_Ita_Init( FIELD_GRANM_ITA* p_wk, u32 arcID, u32 dataID, u32 block_num, HEAPID heapID );
static void FIELD_GRANM_Ita_Exit( FIELD_GRANM_ITA* p_wk );
static void FIELD_GRANM_Ita_Main( FIELD_GRANM_ITA* p_wk, fx32 speed );
static FIELD_GRANM_WORK* FIELD_GRANM_Ita_GetWork( const FIELD_GRANM_ITA* cp_wk, u32 index );

//-------------------------------------
///	�t�B�[���h�n�ʃA�j���[�V�������[�N
//=====================================
static void FIELD_GRANM_Work_Init( FIELD_GRANM_WORK* p_wk, void* p_anmres, HEAPID heapID );
static void FIELD_GRANM_Work_Exit( FIELD_GRANM_WORK* p_wk );
static void FIELD_GRANM_Work_Bind( FIELD_GRANM_WORK* p_wk, const GFL_G3D_RES* cp_mdlres, const GFL_G3D_RES* cp_texres, NNSG3dRenderObj* p_rendobj );
static void FIELD_GRANM_Work_Release( FIELD_GRANM_WORK* p_wk );
static fx32 FIELD_GRANM_Work_SetAnimeFrame( FIELD_GRANM_WORK* p_wk, fx32 frame );


//-------------------------------------
///	ITP�A�j���[�V�����Ǘ�
//=====================================
static void FIELD_GRANM_ItpSys_Init( FIELD_GRANM_ITP_SYS* p_sys, u32 arcID, u32 tblID, u32 itparcID, u32 texarcID, const GFL_G3D_RES* cp_tex, HEAPID heapID );
static void FIELD_GRANM_ItpSys_Exit( FIELD_GRANM_ITP_SYS* p_sys );
static void FIELD_GRANM_ItpSys_Main( FIELD_GRANM_ITP_SYS* p_sys, fx32 speed );

static void FIELD_GRANM_Itp_Init( FIELD_GRANM_ITP* p_wk, u32 arcID, u32 tex_arcID, u32 anmID, u32 texID, const GFL_G3D_RES* cp_tex, HEAPID heapID );
static void FIELD_GRANM_Itp_Exit( FIELD_GRANM_ITP* p_wk );
static void FIELD_GRANM_Itp_Main( FIELD_GRANM_ITP* p_wk, fx32 speed );
static BOOL FIELD_GRANM_Itp_IsMove( const FIELD_GRANM_ITP* cp_wk );
static u32 FIELD_GRANM_Itp_GetTransTexAddr( const GFL_G3D_RES* cp_ground_tex, const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx );
static u32 FIELD_GRANM_Itp_GetTransTexSize( const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx );
static void* FIELD_GRANM_Itp_GetAnimeTex( const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx );



//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���Ǘ��V�X�e���@	����
 *
 *	@param	cp_setup			�Z�b�g�A�b�v�f�[�^
 *	@param	cp_tex				�n�ʃe�N�X�`��
 *	@param	heapID				�q�[�vID
 *
 *	@return	�n�ʃA�j���Ǘ��V�X�e��
 */
//-----------------------------------------------------------------------------
FIELD_GRANM* FIELD_GRANM_Create( const FIELD_GRANM_SETUP* cp_setup, const GFL_G3D_RES* cp_tex, HEAPID heapID )
{
	FIELD_GRANM* p_sys;

	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_GRANM) );

	// ITA�A�j���[�V�����̏���������
	if( cp_setup->ita_use ){
		FIELD_GRANM_Ita_Init( &p_sys->ita_anime, cp_setup->ita_arcID, cp_setup->ita_dataID, cp_setup->block_num, heapID );
	}

	// ITP�A�j���[�V�����̏���������
	if( cp_setup->itp_use ){
		FIELD_GRANM_ItpSys_Init( &p_sys->itp_anime, cp_setup->itp_arcID, cp_setup->itp_tblID, cp_setup->itp_itparcID, cp_setup->itp_texarcID, cp_tex, heapID );
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

	// ITP�A�j���[�V�����̔j��
	FIELD_GRANM_ItpSys_Exit( &p_sys->itp_anime );

	// ITA�A�j���[�V�����̔j������
	FIELD_GRANM_Ita_Exit( &p_sys->ita_anime );
	
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
  fx32 speed;
	// �I�[�g�A�j���[�V��������
	if( p_sys->anime_flag ){
    speed = p_sys->anime_speed;
	}else{
		speed = 0;
  }

	// ITA�A�j���[�V��������
	FIELD_GRANM_Ita_Main( &p_sys->ita_anime, speed );

	// ITP�A�j���[�V����
	FIELD_GRANM_ItpSys_Main( &p_sys->itp_anime, speed );
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
	return FIELD_GRANM_Ita_GetWork( &cp_sys->ita_anime, idx );
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
 *	@brief	ITA�A�j���[�V�����Ǘ��V�X�e��������
 *
 *	@param	p_wk				���[�N
 *	@param	arcID				�A�[�J�C�uID
 *	@param	dataID			�f�[�^ID
 *	@param	block_num		�u���b�N��
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Ita_Init( FIELD_GRANM_ITA* p_wk, u32 arcID, u32 dataID, u32 block_num, HEAPID heapID )
{
	int i;
	void* p_anmres;
  
	// �A�j���[�V�����t�@�C���ǂݍ���
	p_wk->p_anmfile = GFL_ARC_UTIL_Load( arcID, dataID, FALSE, heapID );
	p_anmres = NNS_G3dGetAnmByIdx( p_wk->p_anmfile, 0 );
	
	// �n�ʃA�j���Ǘ����[�N�o�b�t�@�쐬
	p_wk->p_wkbuf	= GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_GRANM_WORK)*block_num );
	p_wk->wkmax		= block_num;
	for( i=0; i<p_wk->wkmax; i++ )
	{
		FIELD_GRANM_Work_Init( &p_wk->p_wkbuf[i], p_anmres, heapID );
	}


#ifdef DEBUG_FIELD_GRANM_MEM_PRINT
  {
    u32 size;

    size = GFL_ARC_GetDataSize( arcID, dataID );
    size += sizeof(FIELD_GRANM_WORK)*block_num;
    size += FIELD_GRANM_WORK_ANIMEOBJ_WORK_SIZE*block_num;

    OS_TPrintf( "ita use mem size = 0x%x worknum = %d\n", size, block_num );
  }
#endif
}

//----------------------------------------------------------------------------
//
/**
 *	@brief	ITA�A�j���[�V�����Ǘ��V�X�e���j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Ita_Exit( FIELD_GRANM_ITA* p_wk )
{
	int i;

	// ����������Ă�����j��
	if(p_wk->p_anmfile){

		for( i=0; i<p_wk->wkmax; i++ )
		{
			FIELD_GRANM_Work_Exit( &p_wk->p_wkbuf[i] );
		}
		GFL_HEAP_FreeMemory( p_wk->p_wkbuf );
		p_wk->p_wkbuf = NULL;
		GFL_HEAP_FreeMemory( p_wk->p_anmfile );
		p_wk->p_anmfile = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ITA�A�j���[�V��������	�t���[���Ǘ�
 *
 *	@param	p_wk		���[�N
 *	@param	speed   �X�s�[�h�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Ita_Main( FIELD_GRANM_ITA* p_wk, fx32 speed )
{
	int i;

  // ITP�ƃA�j���[�V�����t���[�������킹�邽�߁A
  // ��΂ɃA�j���[�V�����͓�����
  p_wk->anime_speed += speed;

	// ����������Ă����珈������
	if( !p_wk->p_anmfile ){
		return;
	}

	// �t���[�����ݒ�
	for( i=0; i<p_wk->wkmax; i++ )
	{
		// FIELD_GRANM_Work_SetAnimeFrame�̒��ŁA
		// �A�j���[�V�����t���[���̍ő�l�ŁA�l�����[�v�����Ă��܂��B
		p_wk->anime_speed = FIELD_GRANM_Work_SetAnimeFrame( &p_wk->p_wkbuf[i], p_wk->anime_speed );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�u���b�N��ITA�A�j���[�V�������[�N���擾
 *
 *	@param	p_wk			���[�N
 *	@param	index			�C���f�b�N�X
 *
 *	@retval	���[�N
 *	@retval	NULL		ITA�A�j���[�V�����Ȃ�
 */
//-----------------------------------------------------------------------------
static FIELD_GRANM_WORK* FIELD_GRANM_Ita_GetWork( const FIELD_GRANM_ITA* cp_wk, u32 index )
{

	// ����������Ă����珈������
	if( !cp_wk->p_anmfile ){
		return NULL;
	}

	GF_ASSERT( index < cp_wk->wkmax );
	return &cp_wk->p_wkbuf[index];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�������[�N�̏�����
 *
 *	@param	p_wk				���[�N
 *	@param	p_anmres		�A�j���[�V�������\�[�X
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Work_Init( FIELD_GRANM_WORK* p_wk, void* p_anmres, HEAPID heapID )
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

  // �A�j���[�V�����������T�C�Y�`�F�b�N
  GF_ASSERT( NNS_G3dAnmObjCalcSizeRequired( p_wk->p_anmres, cp_mdl ) < FIELD_GRANM_WORK_ANIMEOBJ_WORK_SIZE );
	
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
		frame			= frame % (max_frame);
		
		NNS_G3dAnmObjSetFrame( p_wk->p_anmobj, frame );
	}

	return frame;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ITP�A�j���[�V�����V�X�e�� ������
 *    
 *	@param	p_sys     �V�X�e��
 *	@param	arcID     ITP�e�[�u���A�[�J�C�uID
 *	@param	cp_tex    ITP�e�[�u���f�[�^ID
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_ItpSys_Init( FIELD_GRANM_ITP_SYS* p_sys, u32 arcID, u32 tblID, u32 itparcID, u32 texarcID, const GFL_G3D_RES* cp_tex, HEAPID heapID )
{
  FIELD_GRANM_ITP_TBL* p_tbl;
  int i;

  p_tbl = GFL_ARC_UTIL_Load( arcID, tblID, FALSE, heapID );

  for( i=0; i<FIELD_GRANM_ITP_NUM; i++ ){
    // ITP�A�j���[�V�����̏�����
    if( p_tbl->itp[i] != FIELD_GRANM_ITP_TBL_NONE ){
      FIELD_GRANM_Itp_Init( &p_sys->wk[i], itparcID, texarcID, p_tbl->itp[i], p_tbl->tex[i], cp_tex, heapID );
    }
  }
  
  GFL_HEAP_FreeMemory( p_tbl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j������
 *
 *	@param	p_sys �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_ItpSys_Exit( FIELD_GRANM_ITP_SYS* p_sys )
{
  int i;

  for( i=0; i<FIELD_GRANM_ITP_NUM; i++ ){
    if( FIELD_GRANM_Itp_IsMove( &p_sys->wk[i] ) ){
      FIELD_GRANM_Itp_Exit( &p_sys->wk[i] );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C������
 *
 *	@param	p_sys   �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_ItpSys_Main( FIELD_GRANM_ITP_SYS* p_sys, fx32 speed )
{
  int i;

  for( i=0; i<FIELD_GRANM_ITP_NUM; i++ ){
    if( FIELD_GRANM_Itp_IsMove( &p_sys->wk[i] ) ){
      FIELD_GRANM_Itp_Main( &p_sys->wk[i], speed );
    }
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief	ITP�A�j���[�V�����Ǘ��V�X�e���̏�����
 *
 *	@param	p_wk			���[�N
 *	@param	arcID			�A�[�J�C�uID
 *	@param	tex_arcID	�A�[�J�C�uID
 *	@param	anmID			�A�j���[�V�����f�[�^ID
 *	@param	texID			�e�N�X�`���f�[�^ID
 *	@param	cp_tex		�n�ʃe�N�X�`��
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Itp_Init( FIELD_GRANM_ITP* p_wk, u32 arcID, u32 tex_arcID, u32 anmID, u32 texID, const GFL_G3D_RES* cp_tex, HEAPID heapID )
{
	// �n�ʃe�N�X�`���ۑ�
	p_wk->cp_ground_tex = cp_tex;
	
	// �A�j���[�V�������ǂݍ���
	p_wk->p_anmfile = GFL_ARC_UTIL_Load( arcID, anmID, FALSE, heapID );
	TEXANM_UnPackLoadFile( p_wk->p_anmfile, &p_wk->anmtbl );

	// �A�j���[�V�����e�N�X�`���ǂݍ���
	p_wk->p_tex = GFL_G3D_CreateResourceArc( tex_arcID, texID );

	// �A�j���[�V���������̏��쐬
	{
		int i;
		int j, frame_num;
    TEXANM_DATA texanm;
    u32 min_texidx;
    
		p_wk->anime_num			= TEXANM_GetAnimeNum( &p_wk->anmtbl );
		p_wk->p_last_data		= GFL_HEAP_AllocClearMemory( heapID, sizeof(TEXANM_DATA) * p_wk->anime_num );
		p_wk->p_trans_addr	= GFL_HEAP_AllocClearMemory( heapID, sizeof(u32) * p_wk->anime_num );
		p_wk->p_trans_size	= GFL_HEAP_AllocClearMemory( heapID, sizeof(u32) * p_wk->anime_num );
		p_wk->p_anime_frame	= GFL_HEAP_AllocClearMemory( heapID, sizeof(fx32) * p_wk->anime_num );

		for( i=0; i<p_wk->anime_num; i++ ){
			// �����t���[���擾
			p_wk->p_last_data[i]	= TEXANM_GetFrameData( &p_wk->anmtbl, i, 0 );

      // �]����e�N�X�`�����������Ă���t���[��������
      frame_num = TEXANM_GetLastKeyFrame( &p_wk->anmtbl, i );
      min_texidx = p_wk->p_last_data[i].tex_idx;
      for( j=0; j<frame_num; j++ ){
        texanm = TEXANM_GetFrameData( &p_wk->anmtbl, i, j );
        if( min_texidx > texanm.tex_idx ){
            min_texidx = texanm.tex_idx;
        }
      }
			p_wk->p_trans_addr[i]	= FIELD_GRANM_Itp_GetTransTexAddr( cp_tex, p_wk->p_tex, min_texidx );
			p_wk->p_trans_size[i]	= FIELD_GRANM_Itp_GetTransTexSize( p_wk->p_tex, min_texidx );
		}
	}

	// �����]������
	p_wk->trans = TRUE;

#ifdef DEBUG_FIELD_GRANM_MEM_PRINT
  {
    u32 size;

    size = GFL_ARC_GetDataSize( arcID, anmID );
    size += GFL_ARC_GetDataSize( tex_arcID, texID );
    size += sizeof(TEXANM_DATA) * p_wk->anime_num;
    size += sizeof(u32) * p_wk->anime_num;
    size += sizeof(u32) * p_wk->anime_num;
    size += sizeof(fx32) * p_wk->anime_num;

    OS_TPrintf( "itp use mem size = 0x%x animenum = %d\n", size, p_wk->anime_num );
  }
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	ITP�A�j���[�V�����Ǘ��V�X�e���̔j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Itp_Exit( FIELD_GRANM_ITP* p_wk )
{
	if( p_wk->p_anmfile ){

		// �A�j���[�V�������̏��
		GFL_HEAP_FreeMemory( p_wk->p_last_data );
		GFL_HEAP_FreeMemory( p_wk->p_trans_addr );
		GFL_HEAP_FreeMemory( p_wk->p_trans_size );
		GFL_HEAP_FreeMemory( p_wk->p_anime_frame );
		
		
		GFL_HEAP_FreeMemory( p_wk->p_anmfile );
		p_wk->p_anmfile = NULL;
		GFL_G3D_DeleteResource( p_wk->p_tex );
		p_wk->p_tex = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ITP�e�N�X�`���]���A�j���[�V�����Ǘ��@���C��
 *
 *	@param	p_wk			���[�N
 *	@param	speed			�A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
static void FIELD_GRANM_Itp_Main( FIELD_GRANM_ITP* p_wk, fx32 speed )
{
	int i;
	TEXANM_DATA now_data;
	fx32 frame_max;
	BOOL result;
	
	// ����������Ă��Ȃ��Ȃ�A�������Ȃ�
	if( !p_wk->p_anmfile ){
		return ;
	}
	
	// �A�j���[�V�������X�V�`�F�b�N
	frame_max = TEXANM_GetFrameMax( &p_wk->anmtbl ) << FX32_SHIFT;
	for( i=0; i<p_wk->anime_num; i++ ){

		// �A�j���[�V��������
		p_wk->p_anime_frame[i] += speed;
		if( p_wk->p_anime_frame[i] >= (frame_max) ){
			p_wk->p_anime_frame[i] = p_wk->p_anime_frame[i] % (frame_max);
		}
    else
    {
      
      // �`�F�b�N�̕K�v�����邩�`�F�b�N
      if( p_wk->p_trans_addr[i] != FIELD_GRANM_ITP_TRANS_ADDR_NONE ){
        
        now_data = TEXANM_GetFrameData( &p_wk->anmtbl, i, p_wk->p_anime_frame[i]>>FX32_SHIFT );	
        
        // �X�V�`�F�b�N
        if((now_data.tex_idx != p_wk->p_last_data[i].tex_idx) ||
          p_wk->trans ){

          // �]������
          result = NNS_GfdRegisterNewVramTransferTask(
              NNS_GFD_DST_3D_TEX_VRAM, 
              p_wk->p_trans_addr[i], 
              FIELD_GRANM_Itp_GetAnimeTex( p_wk->p_tex, now_data.tex_idx ), 
              p_wk->p_trans_size[i] );

          GF_ASSERT( result );

        }

        // �ۑ�
        p_wk->p_last_data[i] = now_data;
      }

    }
	}

	// �X�V����
	p_wk->trans = FALSE;

}

//----------------------------------------------------------------------------
/**
 *	@brief  GRANM_Itp�������Ă��邩�`�F�b�N
 *
 *	@param	cp_wk 
 *
 *	@retval TRUE  �����Ă���
 *	@retval FALSE �����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FIELD_GRANM_Itp_IsMove( const FIELD_GRANM_ITP* cp_wk )
{
	if( cp_wk->p_anmfile ){
		return TRUE;
	}
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]����A�h���X�̎擾����
 *
 *	@param	cp_ground_tex			�O���E���h�e�N�X�`��
 *	@param	cp_anmtex					�A�j���[�V�����e�N�X�`��
 *	@param	anm_tex_idx				���ׂ�A�j���[�V�����e�N�X�`���̃C���f�b�N�X
 *
 *	@retval	�]����A�h���X
 *	@retval	FIELD_GRANM_ITP_TRANS_ADDR_NONE	�]����Ȃ�
 */
//-----------------------------------------------------------------------------
static u32 FIELD_GRANM_Itp_GetTransTexAddr( const GFL_G3D_RES* cp_ground_tex, const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx )
{
	const NNSG3dResTex* cp_ground_restex;
	const NNSG3dResTex* cp_anm_restex;
	const NNSG3dResName* cp_name;
	const NNSG3dResDictTexData* cp_ground_dict_tex;
	int ground_tex_idx;
	u32	addr;

	// �e�N�X�`�����擾
	cp_ground_restex	= GFL_G3D_GetResTex( cp_ground_tex );
	cp_anm_restex			= GFL_G3D_GetResTex( cp_anmtex );

	cp_name = NNS_G3dGetTexNameByIdx( cp_anm_restex, anm_tex_idx );
	GF_ASSERT( cp_name );

	// �n�ʃe�N�X�`���ł�NAME�̃e�N�X�`���C���f�b�N�X�擾
	ground_tex_idx = NNS_G3dGetTexIdxByName( cp_ground_restex, cp_name );
	if( ground_tex_idx < 0 ){
		return FIELD_GRANM_ITP_TRANS_ADDR_NONE;
	}

	// �]����A�h���X�擾
	cp_ground_dict_tex = NNS_G3dGetTexDataByIdx( cp_ground_restex, ground_tex_idx );

	// �I�t�Z�b�g�l
	addr = (cp_ground_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_MASK)<<3;
	addr += NNS_GfdGetTexKeyAddr( cp_ground_restex->texInfo.vramKey );

	return addr;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]���e�N�X�`���T�C�Y�̎擾
 *
 *	@param	cp_anmtex		�A�j���[�V�����e�N�X�`��
 *	@param	anm_tex_idx	�e�N�X�`���C���f�b�N�X
 *
 *	@return	�T�C�Y
 */
//-----------------------------------------------------------------------------
static u32 FIELD_GRANM_Itp_GetTransTexSize( const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx )
{
	const NNSG3dResTex* cp_anm_restex;
	const NNSG3dResDictTexData* cp_dict_tex;
	u32 byte_size;
	u32 format;
	u32 t_size,s_size;
	u32 texel_size;
	// ���e�N�Z����1�o�C�g�Ȃ̂�
	static const u8 sc_TEXFMT_SIZE[] = {
		0xff,		//   GX_TEXFMT_NONE = 0,
		1,      //   GX_TEXFMT_A3I5 = 1,
		4,      //   GX_TEXFMT_PLTT4 = 2,
		2,      //   GX_TEXFMT_PLTT16 = 3,
		1,      //   GX_TEXFMT_PLTT256 = 4,
		0xff,   //   GX_TEXFMT_COMP4x4 = 5,
		1,			//   GX_TEXFMT_A5I3 = 6,
		0xff,		//   GX_TEXFMT_DIRECT = 7
	};        
  static const u16 sc_TEXSIZE[] = 
  {
    8,
    16,
    32,
    64,
    128,
    256,
    512,
    1024,
  };


	// �e�N�X�`�����擾
	cp_anm_restex			= GFL_G3D_GetResTex( cp_anmtex );

	// size
	cp_dict_tex = NNS_G3dGetTexDataByIdx( cp_anm_restex, anm_tex_idx );
	// �A�j���[�V�����ƃe�N�X�`���f�[�^�̐������������Ă��Ȃ�
	GF_ASSERT( cp_dict_tex );

	// format�𒲂ׂ�
	format = (cp_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_TEXFMT_MASK)>>NNS_G3D_TEXIMAGE_PARAM_TEXFMT_SHIFT;
	texel_size = sc_TEXFMT_SIZE[format];

	// �Ή����Ă��Ȃ��e�N�X�`��format�ł�
	GF_ASSERT( format< NELEMS(sc_TEXFMT_SIZE) );
	GF_ASSERT( texel_size != 0xff );

	//�e�N�X�`���̉��T�C�Y���擾
	s_size = (cp_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_S_SIZE_MASK)>>NNS_G3D_TEXIMAGE_PARAM_S_SIZE_SHIFT;
	s_size = sc_TEXSIZE[s_size];

	//�e�N�X�`���̏c�T�C�Y���擾
	t_size = (cp_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_T_SIZE_MASK)>>NNS_G3D_TEXIMAGE_PARAM_T_SIZE_SHIFT;
	t_size = sc_TEXSIZE[t_size];

  TOMOYA_Printf( "s_size %d t_size %d\n", s_size, t_size );

	//�o�C�g�T�C�Y���v�Z
	byte_size = (s_size*t_size)/texel_size;

	return byte_size;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]������e�N�X�`�����f�[�^�擾
 *
 *	@param	cp_anmtex			�A�j���[�V�����e�N�X�`��
 *	@param	anm_tex_idx		�A�j���[�V�����e�N�X�`���C���f�b�N�X
 *
 *	@return	���\�[�X���f�[�^
 */
//-----------------------------------------------------------------------------
static void* FIELD_GRANM_Itp_GetAnimeTex( const GFL_G3D_RES* cp_anmtex, u32 anm_tex_idx )
{
	const NNSG3dResTex* cp_anm_restex;
	const NNSG3dResDictTexData* cp_dict_tex;
	u32	offset;

	// �e�N�X�`�����擾
	cp_anm_restex			= GFL_G3D_GetResTex( cp_anmtex );

	// size
	cp_dict_tex = NNS_G3dGetTexDataByIdx( cp_anm_restex, anm_tex_idx );
	GF_ASSERT( cp_dict_tex );	// 

	// �I�t�Z�b�g�l
	offset = (cp_dict_tex->texImageParam & NNS_G3D_TEXIMAGE_PARAM_TEX_ADDR_MASK);

	return (void*)((u8*)cp_anm_restex + cp_anm_restex->texInfo.ofsTex + (offset << 3) );
}

