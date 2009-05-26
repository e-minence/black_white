//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dmapobj.c
 *	@brief		�}�b�v�\�����Ǘ�
 *	@author		tomoya takahashi
 *	@data		2007.11.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>


#include "wifi_lobby.naix"

#include "wflby_3dmapobj.h"
#include "wflby_3dmatrix.h"
#include "wflby_3dmapobj_data.h"
#include "wflby_maparc.h"

#include "arc_def.h"
#include "system/gfl_use.h"


//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------

#ifdef PM_DEBUG
//#define WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME	// �������x��\��
#endif

#ifdef WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME

static OSTick	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_Tick;
#define WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_INIT	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_Tick = OS_GetTick();
#define WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT(line)	OS_TPrintf( "	od line[%d] time %d micro\n", (line), OS_TicksToMicroSeconds(OS_GetTick() - WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_Tick) );

#else		// WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME

#define		WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_INIT			((void)0);
#define		WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT(line)	((void)0);

#endif		// WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------




//-------------------------------------
///	�n�ʃ��[�N
//=====================================
typedef struct _WFLBY_3DMAPOBJ_MAP{
	BOOL		on;
	GFL_G3D_OBJSTATUS		objst[ WFLBY_3DMAPOBJ_MAPOBJ_NUM ];
	VecFx32 objrotate[ WFLBY_3DMAPOBJ_MAPOBJ_NUM ];
	GFL_G3D_OBJ *g3dobj[ WFLBY_3DMAPOBJ_MAPOBJ_NUM ];
	BOOL draw_flag[WFLBY_3DMAPOBJ_MAPOBJ_NUM];
	u32			anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];
	fx32		anm_frame[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];
}WFLBY_3DMAPOBJ_MAP;

//-------------------------------------
///	�n�ʃ��\�[�X
//=====================================
typedef struct {
	GFL_G3D_RND *rnder[ WFLBY_3DMAPOBJ_MAPOBJ_NUM ];
	GFL_G3D_RES *p_mdlres[ WFLBY_3DMAPOBJ_MAPOBJ_NUM ];
	GFL_G3D_RES	*p_anmres[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];
	GFL_G3D_ANM	*anm[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];
	BOOL		anm_load[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];
} WFLBY_3DMAPOBJ_MAPRES;


//-------------------------------------
///	�t���[�g���[�N
//=====================================
struct _WFLBY_3DMAPOBJ_FLOAT{
	u8			on;	
	u8			col;	// �F
	u8			mdlno;	// ���f���i���o�[
	u8			pad;
	GFL_G3D_OBJSTATUS		objst;
	VecFx32		objrotate;
	GFL_G3D_OBJ	*g3dobj;
	u32			anm_on[ WFLBY_3DMAPOBJ_FLOAT_ANM_NUM ];
	fx32		anm_frame[ WFLBY_3DMAPOBJ_FLOAT_ANM_NUM ];
	VecFx32		mat;
	VecFx32		ofs;
	u8			draw_flag;
	u8			padding[3];
};

//-------------------------------------
///	�t���[�g���\�[�X
//=====================================
typedef struct {
	GFL_G3D_RND	*rnder[ WFLBY_3DMAPOBJ_FLOAT_NUM ];
	GFL_G3D_RES *p_texres[ WFLBY_3DMAPOBJ_FLOAT_NUM ][ WFLBY_3DMAPOBJ_FLOAT_COL_NUM ];
	GFL_G3D_RES *p_mdlres[ WFLBY_3DMAPOBJ_FLOAT_NUM ];
	GFL_G3D_RES *p_anmres[ WFLBY_3DMAPOBJ_FLOAT_NUM ][ WFLBY_3DMAPOBJ_FLOAT_ANM_NUM ];
	GFL_G3D_ANM	*anm[ WFLBY_3DMAPOBJ_FLOAT_NUM ][ WFLBY_3DMAPOBJ_FLOAT_ANM_NUM ];
} WFLBY_3DMAPOBJ_FLOATRES;

//-------------------------------------
///	���̑��̕����[�N
//	(�\����\�����炢�����o���Ȃ�)
//=====================================
struct _WFLBY_3DMAPOBJ_WK{
	u16			on;
	u16			mdlid;
	GFL_G3D_OBJSTATUS		objst;
	VecFx32		objrotate;
	GFL_G3D_OBJ	*g3dobj;
	GFL_G3D_ANM	*anm[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	u8			anm_on[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	u8			anm_on_pad;
	u8			play[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	u8			play_pad;
	fx32		anm_frame[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	u8			wait_def;							// �����_���E�G�C�g�萔	
	u8			wait[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];	// �E�G�C�g��
	pWFLBY_3DMAPOBJ_WK_AnmCallBack	p_anmcallback[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	fx32		speed;		// �A�j���X�s�[�h
	u8			alpha_flag;	// �A���t�@�l�ݒ�t���O
	u8			alpha;		// �A���t�@�l
	u8			def_alpha;	// ��{�A���t�@�l
	u8			pad;	// ��{�A���t�@�l
	
	u8			draw_flag;
	u8			padding[3];
};

//-------------------------------------
///	���̑��̔z�u�I�u�W�F���\�[�X
//=====================================
typedef struct {
	GFL_G3D_RND *rnder[ WFLBY_3DMAPOBJ_WK_NUM ];
	GFL_G3D_RES *p_mdlres[ WFLBY_3DMAPOBJ_WK_NUM ];
#if WB_FIX
	void*		p_anm[ WFLBY_3DMAPOBJ_WK_NUM ][ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
#else
	GFL_G3D_RES* p_anmres[ WFLBY_3DMAPOBJ_WK_NUM ][ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	GFL_G3D_ANM* anm[ WFLBY_3DMAPOBJ_WK_NUM ][ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
#endif
} WFLBY_3DMAPOBJ_WKRES;

//-------------------------------------
///	�}�b�v�\�����Ǘ��V�X�e��
//=====================================
struct _WFLBY_3DMAPOBJ{
	WFLBY_3DMAPOBJ_MAP		map;		// �}�b�v���[�N	
	WFLBY_3DMAPOBJ_FLOAT*	p_float;	// �t���[�g
	WFLBY_3DMAPOBJ_WK*		p_obj;		// �z�u�I�u�W�F
	u8						floatnum;	// �t���[�g��
	u8						objnum;		// �z�u�I�u�W�F��
	u8						room;		// �����^�C�v
	u8						season;		// �V�[�Y���^�C�v

	BOOL						res_load;								// ���\�[�X��ǂݍ��񂾂�
	WFLBY_3DMAPOBJ_MAPRES		mapres;									// �}�b�v���[�N	
	WFLBY_3DMAPOBJ_FLOATRES		floatres;								// �t���[�g
	WFLBY_3DMAPOBJ_WKRES		objres;									// �z�u�I�u�W�F
#if WB_FIX
	NNSFndAllocator				allocator;								// �A���P�[�^
#endif
};



//-----------------------------------------------------------------------------
/**
 *				�z�u�I�u�W�F�N�g�f�[�^
 */
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// �V�[�Y���̕����̓ǂݍ��݃f�[�^�쐬
static WFLBY_3DMAPOBJ_MDL_DATA* WFLBY_3DMAPOBJ_MDLRES_DATA_Init( WFLBY_ROOM_TYPE room, WFLBY_SEASON_TYPE season, u32 heapID );
static void WFLBY_3DMAPOBJ_MDLRES_DATA_Exit( WFLBY_3DMAPOBJ_MDL_DATA* p_data );


// �L��p���f���ǂݍ��݃V�X�e��
static void WFLBY_3DMAPOBJ_MDL_Load( GFL_G3D_RES** p_res, ARCHANDLE* p_handle, u32 data_idx, u32 gheapID );
static void WFLBY_3DMAPOBJ_MDL_Delete( GFL_G3D_RES* p_mdl );

// �A�j���t���[���Ǘ�
static void WFLBY_3DMAPOBJ_ANM_Loop( fx32* p_frame, const GFL_G3D_ANM* cp_anm, fx32 speed );
static BOOL WFLBY_3DMAPOBJ_ANM_NoLoop( fx32* p_frame, const GFL_G3D_ANM* cp_anm, fx32 speed );
static void WFLBY_3DMAPOBJ_ANM_BackLoop( fx32* p_frame, const GFL_G3D_ANM* cp_anm, fx32 speed );
static BOOL WFLBY_3DMAPOBJ_ANM_BackNoLoop( fx32* p_frame, const GFL_G3D_ANM* cp_anm, fx32 speed );

// �}�b�v���[�N
static void WFLBY_3DMAPOBJ_MAP_Load( WFLBY_3DMAPOBJ_MAPRES* p_wk, ARCHANDLE* p_handle, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID );
static void WFLBY_3DMAPOBJ_MAP_Release( WFLBY_3DMAPOBJ_MAPRES* p_wk );
static void WFLBY_3DMAPOBJ_MAP_Add( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res );
static void WFLBY_3DMAPOBJ_MAP_Delete( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res );
static void WFLBY_3DMAPOBJ_MAP_Main( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res );
static void WFLBY_3DMAPOBJ_MAP_Draw( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res );

// �t���[�g���[�N
static WFLBY_3DMAPOBJ_FLOAT* WFLBY_3DMAPOBJ_FLOAT_GetCleanWk( WFLBY_3DMAPOBJ* p_sys );
static void WFLBY_3DMAPOBJ_FLOAT_Load( WFLBY_3DMAPOBJ_FLOATRES* p_wk, ARCHANDLE* p_handle, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID );
static void WFLBY_3DMAPOBJ_FLOAT_Release( WFLBY_3DMAPOBJ_FLOATRES* p_wk );
static void WFLBY_3DMAPOBJ_FLOAT_Main( WFLBY_3DMAPOBJ_FLOAT* p_wk, WFLBY_3DMAPOBJ_FLOATRES* p_res );
static void WFLBY_3DMAPOBJ_FLOAT_Draw( WFLBY_3DMAPOBJ_FLOAT* p_wk, WFLBY_3DMAPOBJ_FLOATRES* p_res );

// �z�u�I�u�W�F�N�g���[�N
static WFLBY_3DMAPOBJ_WK* WFLBY_3DMAPOBJ_WK_GetCleanWk( WFLBY_3DMAPOBJ* p_sys );
static void WFLBY_3DMAPOBJ_WK_Load( WFLBY_3DMAPOBJ_WKRES* p_wk, ARCHANDLE* p_handle, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID );
static void WFLBY_3DMAPOBJ_WK_Release( WFLBY_3DMAPOBJ_WKRES* p_wk );
static void WFLBY_3DMAPOBJ_WK_Draw( WFLBY_3DMAPOBJ_WKRES* p_res, WFLBY_3DMAPOBJ_WK* p_wk );
static void WFLBY_3DMAPOBJ_WK_Anm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );

static void WFLBY_G3DOBJSTATUS_Init(GFL_G3D_OBJSTATUS *status, VecFx32 *rotate);
static void WFLBY_RotateMtx(VecFx32 *rotate, MtxFx33 *dst_mtx);


// �V�X�e���Ǘ�
//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�I�u�W�F�V�X�e��	������
 *
 *	@param	float_num		�t���[�g��
 *	@param	objwk_num		���̂��̕\������
 *	@param	heapID			�q�[�vID
 *	@param	gheapID			�O���t�B�b�N�q�[�vID
 *
 *	@return	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ* WFLBY_3DMAPOBJ_Init( u32 float_num, u32 objwk_num, u32 heapID, u32 gheapID )
{
	WFLBY_3DMAPOBJ* p_sys;

	p_sys = GFL_HEAP_AllocMemory( heapID, sizeof(WFLBY_3DMAPOBJ) );
	GFL_STD_MemFill( p_sys,  0, sizeof(WFLBY_3DMAPOBJ) );

	// �e���[�N���쐬
	p_sys->p_float	= GFL_HEAP_AllocMemory( heapID, sizeof(WFLBY_3DMAPOBJ_FLOAT)*float_num );
	p_sys->p_obj	= GFL_HEAP_AllocMemory( heapID, sizeof(WFLBY_3DMAPOBJ_WK)*objwk_num );
	GFL_STD_MemFill( p_sys->p_float, 0, sizeof(WFLBY_3DMAPOBJ_FLOAT)*float_num );
	GFL_STD_MemFill( p_sys->p_obj, 0, sizeof(WFLBY_3DMAPOBJ_WK)*objwk_num );
	p_sys->floatnum	= float_num;
	p_sys->objnum	= objwk_num;
	
	
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�I�u�W�F�V�X�e��	�j��
 *
 *	@param	p_sys		���[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_Exit( WFLBY_3DMAPOBJ* p_sys )
{
	// �f�[�^��ǂݍ��ݍς݂Ȃ�j������
	if( p_sys->res_load ){
		WFLBY_3DMAPOBJ_ResRelease( p_sys );
	}

	// �S��������j��
	GFL_HEAP_FreeMemory( p_sys->p_float );
	GFL_HEAP_FreeMemory( p_sys->p_obj );
	GFL_HEAP_FreeMemory( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�I�u�W�F�V�X�e��	���C������
 *
 *	@param	p_sys		���[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_Main( WFLBY_3DMAPOBJ* p_sys )
{
	// �A�j�����C��
	
	//  �}�b�v���C��
	WFLBY_3DMAPOBJ_MAP_Main( &p_sys->map, &p_sys->mapres );
	
	// �t���[�g���C��
	{
		int i;

		for( i=0; i<p_sys->floatnum; i++ ){
//			OS_TPrintf( "float idx=%d ",  i );
			WFLBY_3DMAPOBJ_FLOAT_Main( &p_sys->p_float[i], &p_sys->floatres );
		}
	}

	// �z�u�I�u�W�F���C��
	{
		int i;

		for( i=0; i<p_sys->objnum; i++ ){
			WFLBY_3DMAPOBJ_WK_Anm( p_sys, &p_sys->p_obj[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�I�u�W�F�V�X�e��	�`�揈��
 *
 *	@param	p_sys		���[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_Draw( WFLBY_3DMAPOBJ* p_sys )
{
	int i;
	GF_ASSERT( p_sys );
	GF_ASSERT( p_sys->p_obj );
	GF_ASSERT( p_sys->p_float );

//	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_INIT;
	
	// �}�b�v�`��
	WFLBY_3DMAPOBJ_MAP_Draw( &p_sys->map, &p_sys->mapres );
//	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );
	
	// �z�u�I�u�W�F�N�g�`��
	for( i=0; i<p_sys->objnum; i++ ){
		WFLBY_3DMAPOBJ_WK_Draw( &p_sys->objres, &p_sys->p_obj[i] );
	}
//	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );
	
	// �t���[�g�`��
	for( i=0; i<p_sys->floatnum; i++ ){
//		OS_TPrintf( "float idx=%d ",  i );
		WFLBY_3DMAPOBJ_FLOAT_Draw( &p_sys->p_float[i], &p_sys->floatres );
	}
//	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�I�u�W�F�V�X�e��	VBLANK����
 *
 *	@param	p_sys		���[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_VBlank( WFLBY_3DMAPOBJ* p_sys )
{
	// VBlank�֐�
}

// �����̃��\�[�X
//----------------------------------------------------------------------------
/**
 *	@brief	�����̃��\�[�X��ǂݍ���
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	room		�����^�C�v
 *	@param	season		�V�[�Y���^�C�v
 *	@param	heapID		�q�[�vID
 *	@param	gheapID		�O���t�B�b�N�q�[�vID
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_ResLoad( WFLBY_3DMAPOBJ* p_sys, WFLBY_ROOM_TYPE room, WFLBY_SEASON_TYPE season, u32 heapID, u32 gheapID )
{
	ARCHANDLE* p_handle;
	WFLBY_3DMAPOBJ_MDL_DATA* p_data;

	// �V�[�Y���ƕ�����ۑ�
	p_sys->season	= season;
	p_sys->room		= room;

	// ���f���ǂݍ��݃f�[�^���擾����
	p_data = WFLBY_3DMAPOBJ_MDLRES_DATA_Init( room, season, heapID );
	
	// �V�[�Y���̊e���\�[�X��ǂݍ���
	p_handle = GFL_ARC_OpenDataHandle( ARCID_WFLBY_GRA, heapID );	// �n���h���I�[�v��

#if WB_FIX
	// �A���P�[�^���쐬
	sys_InitAllocator( &p_sys->allocator, gheapID, 4 );
#endif

	// �}�b�v
	WFLBY_3DMAPOBJ_MAP_Load( &p_sys->mapres, p_handle, p_data, gheapID );

	// �z�u�I�u�W�F�N�g
	WFLBY_3DMAPOBJ_WK_Load( &p_sys->objres, p_handle, p_data, gheapID );


	// �t���[�g�I�u�W�F�N�g
	WFLBY_3DMAPOBJ_FLOAT_Load( &p_sys->floatres, p_handle, p_data, gheapID );

	GFL_ARC_CloseDataHandle( p_handle );
	
	// �j��
	WFLBY_3DMAPOBJ_MDLRES_DATA_Exit( p_data );	

	// �}�b�v�͂��łɓo�^����
	WFLBY_3DMAPOBJ_MAP_Add( &p_sys->map, &p_sys->mapres );

	p_sys->res_load = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�������\�[�X��j������
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_ResRelease( WFLBY_3DMAPOBJ* p_sys )
{
	// �}�b�v��j������
	WFLBY_3DMAPOBJ_MAP_Delete( &p_sys->map, &p_sys->mapres );
	
	// �}�b�v���[�N
	WFLBY_3DMAPOBJ_MAP_Release( &p_sys->mapres );
	
	// �z�u�I�u�W�F���[�N
	WFLBY_3DMAPOBJ_WK_Release( &p_sys->objres );
	
	// �t���[�g���[�N
	WFLBY_3DMAPOBJ_FLOAT_Release( &p_sys->floatres );

	p_sys->res_load = FALSE;
}


// �}�b�v���[�N����
//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�̃|�[����\��
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_MAP_OnPoll( WFLBY_3DMAPOBJ* p_sys )
{
	if( p_sys->mapres.anm_load[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] == TRUE ){
		if( p_sys->map.anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] == FALSE ){
		#if WB_FIX
			p_sys->map.anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] = TRUE;
			D3DOBJ_AddAnm( &p_sys->map.obj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], &p_sys->mapres.anm[WFLBY_3DMAPOBJ_MAP_ANM_POLL] );
		#else
			u16 anm_idx;
			p_sys->map.anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] = TRUE;
			//���1�����A�j�����������Ȃ��悤�ɂ��Ă��邽�߁A�K��index��0�̂͂�
			GFL_G3D_OBJECT_EnableAnime( p_sys->map.g3dobj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], WFLBY_3DMAPOBJ_MAP_ANM_POLL );
		#endif
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�̃|�[����\�����Ȃ�����
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_MAP_OffPoll( WFLBY_3DMAPOBJ* p_sys )
{
	if( p_sys->mapres.anm_load[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] == TRUE ){
		if( p_sys->map.anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] == TRUE ){
		#if WB_FIX
			D3DOBJ_DelAnm( &p_sys->map.obj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], &p_sys->mapres.anm[WFLBY_3DMAPOBJ_MAP_ANM_POLL] );
		#else
			GFL_G3D_OBJECT_DisableAnime( p_sys->map.g3dobj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], WFLBY_3DMAPOBJ_MAP_ANM_POLL );
		#endif
			p_sys->map.anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] = FALSE;
		}
	}
}


// �t���[�g���[�N����
//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g���[�N�ǂݍ���
 *	
 *	@param	p_sys		���[�N
 *	@param	floattype	�t���[�g�i���o�[
 *	@param	floatcol	�t���[�g�J���[
 *	@param	cp_mat		���W
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ_FLOAT* WFLBY_3DMAPOBJ_FLOAT_Add( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_FLOAT_TYPE floattype, WFLBY_3DMAPOBJ_FLOAT_COL floatcol, const VecFx32* cp_mat )
{
	WFLBY_3DMAPOBJ_FLOAT* p_wk;

	GF_ASSERT( floattype <= WFLBY_3DMAPOBJ_FLOAT_NUM );
	GF_ASSERT( floatcol <= WFLBY_3DMAPOBJ_FLOAT_COL_NUM );
	
	// �󂢂Ă���t���[�g���[�N���擾����	
	p_wk = WFLBY_3DMAPOBJ_FLOAT_GetCleanWk( p_sys );


#if WB_FIX
	// �G���������Ă�����
	D3DOBJ_Init( &p_wk->obj, &p_sys->floatres.mdl[ floattype ] );

	// �`��J�n
	D3DOBJ_SetDraw( &p_wk->obj, TRUE );
#else
	// �G���������Ă�����
	GF_ASSERT(p_wk->g3dobj == NULL);
	p_wk->g3dobj = GFL_G3D_OBJECT_Create( p_sys->floatres.rnder[ floattype ], p_sys->floatres.anm[ floattype ], WFLBY_3DMAPOBJ_FLOAT_ANM_NUM );
	WFLBY_G3DOBJSTATUS_Init(&p_wk->objst, &p_wk->objrotate);

	// �`��J�n
	p_wk->draw_flag = TRUE;
#endif

	// ���W��ݒ�
	WFLBY_3DMAPOBJ_FLOAT_SetPos( p_wk, cp_mat );
	{
		VecFx32 mat = {0,0,0};
		WFLBY_3DMAPOBJ_FLOAT_SetOfsPos( p_wk, &mat );
	}

	// ���f���i���o�[�ƃJ���[�i���o�[��ۑ�
	p_wk->col	= floatcol;
	p_wk->mdlno	= floattype;

	p_wk->on = TRUE;


	// ��ɃA�j��������̂��A�j�������Ă���
	{
		p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_00] = TRUE;
		p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_02] = TRUE;
	}
	
	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g�j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_Del( WFLBY_3DMAPOBJ_FLOAT* p_wk )
{
	//  �`��t���O�𗎂Ƃ��āA�N���A����
#if WB_FIX
	D3DOBJ_SetDraw( &p_wk->obj, FALSE );
#else
	p_wk->draw_flag = FALSE;
	GFL_G3D_OBJECT_Delete(p_wk->g3dobj);
	p_wk->g3dobj = NULL;
#endif
	
	p_wk->on = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g���W��ݒ�
 *
 *	@param	p_wk		���[�N
 *	@param	pos			���W
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_SetPos( WFLBY_3DMAPOBJ_FLOAT* p_wk, const VecFx32* cp_mat )
{
	p_wk->mat	= *cp_mat;
#if WB_FIX
	D3DOBJ_SetMatrix( &p_wk->obj, p_wk->mat.x+p_wk->ofs.x, p_wk->mat.y+p_wk->ofs.y, p_wk->mat.z+p_wk->ofs.z );
#else
	VEC_Set(&p_wk->objst.trans, p_wk->mat.x+p_wk->ofs.x, p_wk->mat.y+p_wk->ofs.y, p_wk->mat.z+p_wk->ofs.z );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��I�t�Z�b�g�l��ݒ肷��
 *
 *	@param	p_wk		���[�N
 *	@param	cp_mat		���W
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_SetOfsPos( WFLBY_3DMAPOBJ_FLOAT* p_wk, const VecFx32* cp_mat )
{
	p_wk->ofs	= *cp_mat;
#if WB_FIX
	D3DOBJ_SetMatrix( &p_wk->obj, p_wk->mat.x+p_wk->ofs.x, p_wk->mat.y+p_wk->ofs.y, p_wk->mat.z+p_wk->ofs.z );
#else
	VEC_Set(&p_wk->objst.trans, p_wk->mat.x+p_wk->ofs.x, p_wk->mat.y+p_wk->ofs.y, p_wk->mat.z+p_wk->ofs.z );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g���W���擾
 *
 *	@param	p_wk	���[�N
 *	@param	p_mat	�}�g���b�N�X
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_GetPos( const WFLBY_3DMAPOBJ_FLOAT* cp_wk, VecFx32* p_mat )
{
#if WB_FIX
	D3DOBJ_GetMatrix( &cp_wk->obj, &p_mat->x, &p_mat->y, &p_mat->z );
#else
	*p_mat = cp_wk->objst.trans;
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g�\���̂n�m�n�e�e
 *
 *	@param	p_wk	���[�N
 *	@param	flag	�t���O
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_SetDraw( WFLBY_3DMAPOBJ_FLOAT* p_wk, BOOL flag )
{
#if WB_FIX
	D3DOBJ_SetDraw( &p_wk->obj, flag );
#else
	p_wk->draw_flag = flag;
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g�`��`�F�b�N
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	�`�撆
 *	@retval	FALSE	�`�悵�ĂȂ�
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_GetDraw( const WFLBY_3DMAPOBJ_FLOAT* cp_wk )
{
#if WB_FIX
	return D3DOBJ_GetDraw( &cp_wk->obj );
#else
	return cp_wk->draw_flag;
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	Sound�A�j��
 *
 *	@param	p_wk	���[�N
 *
 *	@retval	TRUE	�Đ�����
 *	@retval	FALSE	�Đ����Ȃ̂ōĐ����Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_SetAnmSound( WFLBY_3DMAPOBJ_FLOAT* p_wk )
{
	if( p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_01] == FALSE ){
		p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_01] = TRUE;
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	body�A�j��
 *
 *	@param	p_wk	���[�N
 *
 *	@retval	TRUE	�Đ�����
 *	@retval	FALSE	�Đ����Ȃ̂ōĐ����Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_SetAnmBody( WFLBY_3DMAPOBJ_FLOAT* p_wk )
{
	if( p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_03] == FALSE ){
		p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_03] = TRUE;
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��]�p�x��ݒ肷��
 *
 *	@param	p_wk		���[�N
 *	@param	x			���p�x
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_SetRotaX( WFLBY_3DMAPOBJ_FLOAT* p_wk, u16 x )
{
#if WB_FIX
	D3DOBJ_SetRota( &p_wk->obj, x, D3DOBJ_ROTA_WAY_X );
#else
	p_wk->objrotate.x = x;
	WFLBY_RotateMtx(&p_wk->objrotate, &p_wk->objst.rotate);
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g�����A�j���Đ��`�F�b�N
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	�����A�j��	�Đ���
 *	@retval	FALSE	�����A�j��	�Đ����ĂȂ�
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_CheckAnmSound( const WFLBY_3DMAPOBJ_FLOAT* cp_wk )
{
	return cp_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_01];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g���̃A�j��	�Đ���
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	���̃A�j��	�Đ���
 *	@retval	FALSE	���̃A�j��	��~��
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_CheckAnmBody( const WFLBY_3DMAPOBJ_FLOAT* cp_wk )
{
	return cp_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_03];
}



// �z�u�I�u�W�F����
//----------------------------------------------------------------------------
/**
 *	@brief	�z�u�I�u�W�F�쐬
 *
 *	@param	p_sys		�V�X�e��
 *	@param	objtype		�I�u�W�F�N�gNO
 *	@param	gridx		�O���b�hX
 *	@param	gridy		�O���b�hY
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ_WK* WFLBY_3DMAPOBJ_WK_Add( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK_TYPE objtype, u32 gridx, u32 gridy )
{
	WFLBY_3DMAPOBJ_WK* p_wk;
	int i;
	
	// �󂢂Ă��郏�[�N���擾
	p_wk = WFLBY_3DMAPOBJ_WK_GetCleanWk( p_sys );


	// �I�������O���t�B�b�N��ݒ�
#if WB_FIX
	D3DOBJ_Init( &p_wk->obj, &p_sys->objres.mdl[objtype] );
#else
	GF_ASSERT(p_wk->g3dobj == NULL);
	p_wk->g3dobj = GFL_G3D_OBJECT_Create( p_sys->objres.rnder[ objtype ], p_sys->objres.anm[ objtype ], WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	WFLBY_G3DOBJSTATUS_Init(&p_wk->objst, &p_wk->objrotate);
#endif

	// �A�j���I�u�W�F�̏�����
	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
	#if WB_FIX
		if( p_sys->objres.p_anm[objtype][i] != NULL ){
			// �A�j���V�����I�u�W�F�̏�����
			D3DOBJ_AnmLoad_Data( &p_wk->anm[i], &p_sys->objres.mdl[objtype],
					p_sys->objres.p_anm[objtype][i] );
		}
	#else
		if( p_sys->objres.p_anmres[objtype][i] != NULL ){
			// �A�j���V�����I�u�W�F�̏�����
			GF_ASSERT(p_wk->anm[i] == NULL);
			p_wk->anm[i] = GFL_G3D_ANIME_Create( 
				p_sys->objres.rnder[ objtype ], p_sys->objres.p_anmres[objtype][i], 0 ); 
		}
	#endif
	}
	
	// �`��J�n
#if WB_FIX
	D3DOBJ_SetDraw( &p_wk->obj, TRUE );
#else
	p_wk->draw_flag = TRUE;
#endif

	// �O���b�h���W��ݒ�
	{
		WF2DMAP_POS	pos;
		pos.x	= WF2DMAP_GRID2POS(gridx);
		pos.y	= WF2DMAP_GRID2POS(gridy);
		WFLBY_3DMAPOBJ_WK_SetPos( p_wk, pos );
	}

	p_wk->on	= TRUE;
	p_wk->mdlid	= objtype;

	// �A�j���ݒ�
	p_wk->wait_def = WFLBY_3DMAPOBJ_WK_ANM_RANDWAIT_WAITDEF;

	// �X�s�[�h
	p_wk->speed = FX32_ONE;

	// �A���t�@�ݒ�
	p_wk->alpha_flag	= FALSE;
	p_wk->alpha			= 31;
	p_wk->def_alpha		= 31;

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�z�u�I�u�W�F�j��
 *
 *	@param	p_sys		�V�X�e��
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_Del( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	int i;
	
	//  �`��t���O�𗎂Ƃ��āA�N���A����
#if WB_FIX
	D3DOBJ_SetDraw( &p_wk->obj, FALSE );
#else
	p_wk->draw_flag = FALSE;
	GFL_G3D_OBJECT_Delete( p_wk->g3dobj );
	p_wk->g3dobj = NULL;
#endif

	// �A�j���I�u�W�F�̔j��
	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
	#if WB_FIX
		if( p_sys->objres.p_anm[p_wk->mdlid][i] != NULL ){
			// �A�j���V�����I�u�W�F�̏�����
			D3DOBJ_AnmLoad_Data( &p_wk->anm[i], &p_sys->objres.mdl[p_wk->mdlid],
					p_sys->objres.p_anm[p_wk->mdlid][i] );
		}
	#else
		if( p_sys->objres.p_anmres[p_wk->mdlid][i] != NULL ){
			// �A�j���V�����I�u�W�F�̏�����
			GFL_G3D_ANIME_Delete( p_wk->anm[i] );
			p_wk->anm[i] = NULL;
		}
	#endif
	}
	
	p_wk->on = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�^�C�v���擾����
 *
 *	@param	cp_wk		���[�N
 *
 *	@return	�I�u�W�F�^�C�v
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ_WK_TYPE WFLBY_3DMAPOBJ_WK_GetType( const WFLBY_3DMAPOBJ_WK* cp_wk )
{
	return cp_wk->mdlid;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���W�ݒ�
 *	
 *	@param	p_wk	���[�N
 *	@param	pos		���W
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetPos( WFLBY_3DMAPOBJ_WK* p_wk, WF2DMAP_POS pos )
{
	VecFx32 matrix;

	WFLBY_3DMATRIX_GetPosVec( &pos, &matrix );
#if WB_FIX
	D3DOBJ_SetMatrix( &p_wk->obj, matrix.x, matrix.y, matrix.z );
#else
	VEC_Set( &p_wk->objst.trans, matrix.x, matrix.y, matrix.z );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	���W���擾����
 *
 *	@param	p_wk	���[�N
 *
 *	@return	���W
 */
//-----------------------------------------------------------------------------
WF2DMAP_POS WFLBY_3DMAPOBJ_WK_GetPos( const WFLBY_3DMAPOBJ_WK* cp_wk )
{
	VecFx32		matrix;
	WF2DMAP_POS	pos;

#if WB_FIX
	D3DOBJ_GetMatrix( &cp_wk->obj, &matrix.x, &matrix.y, &matrix.z );
#else
	matrix = cp_wk->objst.trans;
#endif
	WFLBY_3DMATRIX_GetVecPos( &matrix, &pos );

	return pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���o�^
 *
 *	@param	p_sys		�I�u�W�F�V�X�e��
 *	@param	p_wk		���[�N
 *	@param	anm			�A�j��
 *	@param	play		�v���C����
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_AddAnm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm, WFLBY_3DMAPOBJ_WK_ANM_PLAY play )
{
	WFLBY_3DMAPOBJ_WK_AddAnmAnmCallBack( p_sys, p_wk, anm, play, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���Đ�	�I���R�[���o�b�N��
 *
 *	@param	p_sys		�I�u�W�F�V�X�e��
 *	@param	p_wk		���[�N
 *	@param	anm			�A�j��
 *	@param	play		�v���C����
 *	@param	p_callback	�A�j���I���R�[���o�b�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_AddAnmAnmCallBack( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm, WFLBY_3DMAPOBJ_WK_ANM_PLAY play, pWFLBY_3DMAPOBJ_WK_AnmCallBack p_callback )
{
	int get_anmframe, set_anmframe, index;
	int test, test2;
	
	// ���̃A�j�������邩�`�F�b�N
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( play < WFLBY_3DMAPOBJ_WK_ANM_PLAYNUM );
	GF_ASSERT( p_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );

	index = 0;
	
#if WB_FIX
	if( (p_sys->objres.p_anm[p_wk->mdlid][anm] != NULL) ){
#else
	if( (p_sys->objres.p_anmres[p_wk->mdlid][anm] != NULL) ){
#endif
		if( p_wk->anm_on[ anm ] == FALSE ){
			//  �A�j���ݒ�
		#if WB_FIX
			D3DOBJ_AddAnm( &p_wk->obj, &p_wk->anm[anm] );
		#else
			GFL_G3D_OBJECT_RemoveAnime( p_wk->g3dobj, index);
			test = GFL_G3D_OBJECT_AddAnime( p_wk->g3dobj, p_wk->anm[anm] );
			test2 = GFL_G3D_OBJECT_EnableAnime( p_wk->g3dobj, index );
			GF_ASSERT(index == test);
			GF_ASSERT(test == 0);
			GF_ASSERT(test2 == TRUE);
		#endif
		}

		p_wk->anm_on[ anm ]		= TRUE;
		p_wk->play[anm]			= play;
		p_wk->p_anmcallback[anm]= p_callback;
		p_wk->wait[ anm ]		= 0;

		switch( play ){
		case WFLBY_3DMAPOBJ_WK_ANM_LOOP:
		case WFLBY_3DMAPOBJ_WK_ANM_NOLOOP:
		case WFLBY_3DMAPOBJ_WK_ANM_NOAUTO:
			p_wk->anm_frame[ anm ]	= 0;
			break;
		case WFLBY_3DMAPOBJ_WK_ANM_BACKLOOP:
		case WFLBY_3DMAPOBJ_WK_ANM_BACKNOLOOP:
		#if WB_FIX
			p_wk->anm_frame[ anm ]	= D3DOBJ_AnmGetFrameNum( &p_wk->anm[anm] );
		#else
			p_wk->anm_frame[ anm ]	= NNS_G3dAnmObjGetNumFrame(GFL_G3D_ANIME_GetAnmObj(p_wk->anm[anm]));
		#endif
			break;

		case WFLBY_3DMAPOBJ_WK_ANM_RANDWAIT:
			p_wk->anm_frame[ anm ]	= 0;
			p_wk->wait[ anm ] = GFUser_GetPublicRand(p_wk->wait_def);
			break;

		case WFLBY_3DMAPOBJ_WK_ANM_RANDSTART:
			p_wk->anm_frame[ anm ]	= 0;
			p_wk->wait[ anm ] = GFUser_GetPublicRand(p_wk->wait_def);
			break;
		}

	#if WB_FIX
		D3DOBJ_AnmSet( &p_wk->anm[anm], p_wk->anm_frame[anm] );
	#else
		set_anmframe = p_wk->anm_frame[anm];
		GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj, index, &set_anmframe );
		OS_TPrintf("aaa SetAnimeFrame frame=%d, index=%d\n", set_anmframe, index);
	#endif
	}	
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���j��
 *
 *	@param	p_sys		�I�u�W�F�V�X�e��
 *	@param	p_wk		���[�N
 *	@param	anm			�A�j��
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_DelAnm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm )
{
  int index;
  BOOL ret;
  
  index = 0;
  
	// ���̃A�j�������邩�`�F�b�N
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( p_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );

#if WB_FIX
	if( (p_sys->objres.p_anm[p_wk->mdlid][anm] != NULL) && (p_wk->anm_on[anm] == TRUE) ){
#else
	if( (p_sys->objres.p_anmres[p_wk->mdlid][anm] != NULL) && (p_wk->anm_on[anm] == TRUE) ){
#endif
		//  �A�j�����͂���
	#if WB_FIX
		D3DOBJ_DelAnm( &p_wk->obj, &p_wk->anm[ anm ] );
	#else
		ret = GFL_G3D_OBJECT_DisableAnime( p_wk->g3dobj, index ); //anm );
		GF_ASSERT(ret == TRUE);
	#endif
	
		p_wk->anm_on[ anm ]		= FALSE;
		p_wk->anm_frame[ anm ]	= 0;
		p_wk->play[ anm ]		= 0;
		p_wk->wait[ anm ]		= 0;
		p_wk->p_anmcallback[anm]= NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�S�A�j����OFF����
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	p_wk		�z�u�I�u�W�F���[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_DelAnmAll( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	int i;

	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
		if( WFLBY_3DMAPOBJ_WK_GetAnmFlag( p_wk, i ) == TRUE ){
			WFLBY_3DMAPOBJ_WK_DelAnm( p_sys,  p_wk, i );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���Đ������`�F�b�N
 *
 *	@param	cp_sys		�V�X�e��
 *	@param	cp_wk		���[�N
 *	@param	anm			�A�j��
 *
 *	@retval	TRUE	�Đ���
 *	@retval	FALSE	�Đ����ĂȂ�
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_WK_CheckAnm( const WFLBY_3DMAPOBJ* cp_sys, const WFLBY_3DMAPOBJ_WK* cp_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm )
{
	// ���̃A�j�������邩�`�F�b�N
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( cp_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );
	
#if WB_FIX
	if( cp_sys->objres.p_anm[cp_wk->mdlid][anm] != NULL ){
#else
	if( cp_sys->objres.p_anmres[cp_wk->mdlid][anm] != NULL ){
#endif
		return cp_wk->anm_on[ anm ];
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����t���[����ݒ肷��
 *
 *	@param	p_sys		�V�X�e��
 *	@param	p_wk		���[�N
 *	@param	anm			�A�j���^�C�v
 *	@param	frame		�t���[����
 *
 *	�A�j���[�V�����^�C�v��WFLBY_3DMAPOBJ_WK_ANM_NOAUTO�̂Ƃ���p
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetAnmFrame( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm, fx32 frame )
{
	int set_anmframe;
	
	// ���̃A�j�������邩�`�F�b�N
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( p_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );
#if WB_FIX
	GF_ASSERT( p_sys->objres.p_anm[p_wk->mdlid][anm] != NULL );
#else
	GF_ASSERT( p_sys->objres.p_anmres[p_wk->mdlid][anm] != NULL );
#endif

	if( p_wk->play[ anm ] != WFLBY_3DMAPOBJ_WK_ANM_NOAUTO ){
		return ; 	// �I�[�g�A�j�����Ȃ̂ő��삵�Ȃ�
	}
	p_wk->anm_frame[ anm ]	= frame;
#if WB_FIX
	D3DOBJ_AnmSet( &p_wk->anm[anm], p_wk->anm_frame[anm] );
#else
	set_anmframe = p_wk->anm_frame[anm];
	GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj, anm, &set_anmframe );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���ݒ肵�Ă��邩�`�F�b�N
 *
 *	@param	cp_wk		���[�N
 *	@param	anm			�A�j���^�C�v
 *
 *	@retval	TRUE	�A�j����
 *	@retval	FALSE	�A�j�����ĂȂ�
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_WK_GetAnmFlag( const WFLBY_3DMAPOBJ_WK* cp_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm )
{
	// ���̃A�j�������邩�`�F�b�N
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( cp_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );
	return cp_wk->anm_on[ anm ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���t���[�����擾����
 *
 *	@param	cp_wk		���[�N
 *	@param	anm			�A�j���^�C�v
 *
 *	@return	�t���[����
 */
//-----------------------------------------------------------------------------
fx32 WFLBY_3DMAPOBJ_WK_GetAnmFrame( const WFLBY_3DMAPOBJ_WK* cp_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm )
{
	// ���̃A�j�������邩�`�F�b�N
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( cp_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );
	return cp_wk->anm_frame[ anm ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����_���E�G�C�g�Đ��p	�E�G�C�g���Ԑݒ�
 *
 *	@param	p_sys		�V�X�e��
 *	@param	p_wk		���[�N
 *	@param	wait		�E�G�C�g�萔
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetRandWait( WFLBY_3DMAPOBJ_WK* p_wk, u32 wait )
{
	p_wk->wait_def = wait;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��t���O��ݒ�
 *
 *	@param	p_wk	���[�N
 *	@param	flag	�`��t���O
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetDraw( WFLBY_3DMAPOBJ_WK* p_wk, BOOL flag )
{
#if WB_FIX
	D3DOBJ_SetDraw( &p_wk->obj, flag );
#else
	p_wk->draw_flag = flag;
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�g�t���O��ݒ肷��
 *			�y���̃��f���Ɠ����`�̃��f���͂��ׂĂ��̃��C�g�ݒ�ɂȂ��Ă��܂��̂�
 *			���ӂ��K�v�ł��B�z
 *
 *	@param	p_sys			�V�X�e��
 *	@param	p_wk			���[�N
 *	@param	light_flag		���C�g�t���O
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetLightFlag( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, u32 light_flag )
{
	NNSG3dResMdlSet*		pMdlset;
	NNSG3dResMdl*			pMdl;
	
	// ���̃A�j�������邩�`�F�b�N
	GF_ASSERT( p_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );

	// �t�B�[���h�̐ݒ�ɕύX
#if WB_FIX
	NNS_G3dMdlSetMdlLightEnableFlagAll( p_sys->objres.mdl[ p_wk->mdlid ].pModel, light_flag );
#else
	pMdlset = NNS_G3dGetMdlSet(GFL_G3D_GetResourceFileHeader(p_sys->objres.p_mdlres[p_wk->mdlid]));
	pMdl = NNS_G3dGetMdlByIdx( pMdlset, 0 );
	NNS_G3dMdlSetMdlLightEnableFlagAll( pMdl, light_flag );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���X�s�[�h�ݒ�
 *
 *	@param	p_wk	���[�N
 *	@param	speed	�X�s�[�h
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetAnmSpeed( WFLBY_3DMAPOBJ_WK* p_wk, fx32 speed )
{
	p_wk->speed = speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���X�s�[�h���擾����
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�X�s�[�h
 */
//-----------------------------------------------------------------------------
fx32 WFLBY_3DMAPOBJ_WK_GetAnmSpeed( const WFLBY_3DMAPOBJ_WK* cp_wk )
{
	return cp_wk->speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A���t�@�l�ݒ�
 *	
 *	@param	p_wk	���[�N
 *	@param	alpha	�A���t�@
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetAlpha( WFLBY_3DMAPOBJ_WK* p_wk, u8 alpha, u8 def_alpha )
{
	p_wk->alpha_flag	= TRUE;
	p_wk->alpha			= alpha;
	p_wk->def_alpha		= def_alpha;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A���t�@�l�̃��Z�b�g
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_ResetAlpha( WFLBY_3DMAPOBJ_WK* p_wk )
{
	p_wk->alpha_flag	= FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�g��l��ݒ�
 *
 *	@param	p_wk	���[�N
 *	@param	x		������
 *	@param	y		������
 *	@param	z		������
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetScale( WFLBY_3DMAPOBJ_WK* p_wk, fx32 x, fx32 y, fx32 z )
{
#if WB_FIX
	D3DOBJ_SetScale( &p_wk->obj, x, y, z );
#else
	VEC_Set(&p_wk->objst.scale, x, y, z);
#endif
}




//-----------------------------------------------------------------------------
/**
 *				�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	�����i���o�[Season����ǂݍ��݃f�[�^���擾����
 *
 *	@param	room		�����i���o�[
 *	@param	season		Season�i���o�[
 *	@param	heapID		�q�[�vID
 *
 *	@return	�ǂݍ��݃O���t�B�b�N�f�[�^
 */	
//-----------------------------------------------------------------------------
static WFLBY_3DMAPOBJ_MDL_DATA* WFLBY_3DMAPOBJ_MDLRES_DATA_Init( WFLBY_ROOM_TYPE room, WFLBY_SEASON_TYPE season, u32 heapID )
{
	void* p_data;
	u32 idx;

	// �����C���f�b�N�X
	idx = (season * WFLBY_ROOM_NUM) + room;

	GF_ASSERT( idx < (WFLBY_SEASON_NUM*WFLBY_ROOM_NUM) );
	
	p_data = GFL_ARC_UTIL_Load( ARCID_WFLBY_MAP, 
			NARC_wflby_map_wflby_3dmapobj_data_0_dat+idx,
			FALSE, GFL_HEAP_LOWID(heapID));

	return p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ǂݍ��݃O���t�B�b�N�f�[�^�擾
 *
 *	@param	p_data	���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MDLRES_DATA_Exit( WFLBY_3DMAPOBJ_MDL_DATA* p_data )
{
	GFL_HEAP_FreeMemory( p_data );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�L��p���f���ǂݍ���
 *
 *	@param	p_mdl		���f�����[�N
 *	@param	p_handle	�n���h��
 *	@param	data_idx	�f�[�^�C���f�b�N�X
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MDL_Load( GFL_G3D_RES** p_res, ARCHANDLE* p_handle, u32 data_idx, u32 gheapID )
{
	// �e�N�X�`����������j�������e�N�X�`�����\�[�X��ǂݍ���
	WFLBY_3DMAPOBJ_TEX_LoatCutTex( p_res, p_handle, data_idx, gheapID );
	
	// �{�f�[�^�̊e�|�C���^���擾����
#if WB_FIX	//GFL_G3D_RENDER_Create�̓����ōs���̂ł��K�v�Ȃ�
	{
		NNSG3dResFileHeader* nnsg3d_head = GFL_G3D_GetResourceFileHeader(*p_res);

		p_mdl->pModelSet	= NNS_G3dGetMdlSet( nnsg3d_head );
		p_mdl->pModel		= NNS_G3dGetMdlByIdx( p_mdl->pModelSet, 0 );
		p_mdl->pMdlTex		= NNS_G3dGetTex( nnsg3d_head );
	}

	// �e�N�X�`���L�[��ݒ肵�āA���f���Ƀo�C���h����
	{
		BindTexture( p_mdl->pResMdl, p_mdl->pMdlTex );
	}
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L��p���f���j��
 *
 *	@param	p_mdl		���f�����[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MDL_Delete( GFL_G3D_RES* p_mdl )
{
#if WB_FIX
	D3DOBJ_MdlDelete( p_mdl );
#else
	GFL_G3D_FreeVramTexture(p_mdl);
	GFL_G3D_DeleteResource(p_mdl);
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[���Ǘ�
 *
 *	@param	p_frame		�t���[���|�C���^
 *	@param	cp_anm		�ΏۃA�j���[�V�����I�u�W�F
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_ANM_Loop( fx32* p_frame, const GFL_G3D_ANM* cp_anm, fx32 speed )
{
	fx32 end_frame;

#if WB_FIX
	end_frame = D3DOBJ_AnmGetFrameNum( cp_anm );
#else
	end_frame = NNS_G3dAnmObjGetNumFrame(GFL_G3D_ANIME_GetAnmObj(cp_anm));
#endif

	// �A�j����i�߂�
	if( ((*p_frame) + speed) < end_frame ){
		(*p_frame) += speed;
	}else{
		(*p_frame) = ((*p_frame) + speed) % end_frame;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[���Ǘ�
 *
 *	@param	p_frame		�t���[���|�C���^
 *	@param	cp_anm		�ΏۃA�j���[�V�����I�u�W�F
 *
 *	@retval	TRUE	�A�j���I��
 *	@retval	FALSE	�A�j���r��
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DMAPOBJ_ANM_NoLoop( fx32* p_frame, const GFL_G3D_ANM* cp_anm, fx32 speed )
{
	fx32 end_frame;
	BOOL ret;

#if WB_FIX
	end_frame = D3DOBJ_AnmGetFrameNum( cp_anm );
#else
	end_frame = NNS_G3dAnmObjGetNumFrame(GFL_G3D_ANIME_GetAnmObj(cp_anm));
#endif

	// �A�j����i�߂�
	if( ((*p_frame) + speed) < end_frame ){
		(*p_frame) += speed;
		ret = FALSE;
	}else{
		(*p_frame) =  end_frame-FX32_HALF;
		ret = TRUE;
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�Đ��A�j��
 *	
 *	@param	p_frame		�t���[��
 *	@param	cp_anm		�A�j���I�u�W�F
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_ANM_BackLoop( fx32* p_frame, const GFL_G3D_ANM* cp_anm, fx32 speed )
{
	fx32 end_frame;

#if WB_FIX
	end_frame = D3DOBJ_AnmGetFrameNum( cp_anm );
#else
	end_frame = NNS_G3dAnmObjGetNumFrame(GFL_G3D_ANIME_GetAnmObj(cp_anm));

#endif

	// �A�j����i�߂�
	if( ((*p_frame) - speed) >= 0 ){
		(*p_frame) -= speed;
	}else{
		(*p_frame) = ((*p_frame) - speed) + end_frame;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�Đ����[�v�Ȃ��A�j��
 *
 *	@param	p_frame		�t���[��
 *	@param	cp_anm		�A�j��
 *
 *	@retval	TRUE	�A�j���I��
 *	@retval	FALSE	�A�j���r��
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DMAPOBJ_ANM_BackNoLoop( fx32* p_frame, const GFL_G3D_ANM* cp_anm, fx32 speed )
{
	BOOL ret;

	// �A�j����i�߂�
	if( ((*p_frame) - speed) > 0 ){
		(*p_frame) -= speed;
		ret = FALSE;
	}else{
		(*p_frame) =  0;
		ret = TRUE;
	}

	return ret;
}



// �}�b�v���[�N
//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�ǂݍ���
 *
 *	@param	p_wk		���[�N
 *	@param	p_handle	�n���h��
 *	@param	cp_data		�f�[�^
 *	@param	gheapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Load( WFLBY_3DMAPOBJ_MAPRES* p_wk, ARCHANDLE* p_handle, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID )
{
	int i, j;
	// ���f�����\�[�X�ǂݍ���
	for( i=0; i<WFLBY_3DMAPOBJ_MAPOBJ_NUM; i++ ){
//		OS_TPrintf( "map load mdl idx = %d\n",  cp_data->map_idx[i] );
		WFLBY_3DMAPOBJ_MDL_Load( &p_wk->p_mdlres[ i ],
				p_handle, cp_data->map_idx[ i ], gheapID );
		p_wk->rnder[i] = GFL_G3D_RENDER_Create( p_wk->p_mdlres[i], 0, p_wk->p_mdlres[i] );


		// ���C�g�ݒ�
	#if WB_FIX
		WFLBY_LIGHT_SetUpMdl( p_wk->mdl[ i ].pResMdl );	// ���C�g���f
	#else
		WFLBY_LIGHT_SetUpMdl( GFL_G3D_GetResourceFileHeader(p_wk->p_mdlres[i]) );	// ���C�g���f
	#endif
	}

	// �A�j�����\�[�X��ǂݍ���
	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM; i++ ){

		// �A�j��IDX�����f��IDX�ƈ������ǂݍ���
		if( cp_data->map_anmidx[i] == cp_data->map_idx[WFLBY_3DMAPOBJ_MAPOBJ_MAP] ){
			p_wk->anm_load[i] = FALSE;
		}else{

			p_wk->anm_load[i] = TRUE;

//			OS_TPrintf( "map load anm idx = %d\n",  cp_data->map_anmidx[i] );

			if( i != WFLBY_3DMAPOBJ_MAP_ANM_MAT ){
			#if WB_FIX
				D3DOBJ_AnmLoadH( &p_wk->anm[i], &p_wk->mdl[ WFLBY_3DMAPOBJ_MAPOBJ_MAP ],
						p_handle, 
						cp_data->map_anmidx[i], gheapID, p_allocator );
			#else
				p_wk->p_anmres[i] = GFL_G3D_CreateResourceHandle( 
					p_handle, cp_data->map_anmidx[i] );
				p_wk->anm[i] = GFL_G3D_ANIME_Create( 
					p_wk->rnder[WFLBY_3DMAPOBJ_MAPOBJ_MAP], p_wk->p_anmres[i], 0 ); 
			#endif
			}else{

			#if WB_FIX
				D3DOBJ_AnmLoadH( &p_wk->anm[i], &p_wk->mdl[ WFLBY_3DMAPOBJ_MAPOBJ_MAT ],
						p_handle, 
						cp_data->map_anmidx[i], gheapID, p_allocator );
			#else
				p_wk->p_anmres[i] = GFL_G3D_CreateResourceHandle( 
					p_handle, cp_data->map_anmidx[i] );
				p_wk->anm[i] = GFL_G3D_ANIME_Create( 
					p_wk->rnder[WFLBY_3DMAPOBJ_MAPOBJ_MAT], p_wk->p_anmres[i], 0 ); 
			#endif
			}

#if 0
			// ���삳����NODEIDX���w��
			for( j=0; j<cp_data->map_nodenum; j++ ){
				if( cp_data->map_anmnodeidx[i] != j ){
					NNS_G3dAnmObjDisableID( p_wk->anm[i].pAnmObj, j );
				}
			}
#endif
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���f�����\�[�X�̔j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Release( WFLBY_3DMAPOBJ_MAPRES* p_wk )
{
	int i;

	// �A�j���j��
	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM; i++ ){
		if( p_wk->anm_load[i] ){
		#if WB_FIX
			D3DOBJ_AnmDelete( &p_wk->anm[i], p_allocator );
		#else
			GFL_G3D_ANIME_Delete( p_wk->anm[i] );
			GFL_G3D_DeleteResource( p_wk->p_anmres[i] );
		#endif
			p_wk->anm_load[i] = FALSE;
		}
	}

	//  ���f���j��
	for( i=0; i<WFLBY_3DMAPOBJ_MAPOBJ_NUM; i++ ){
		WFLBY_3DMAPOBJ_MDL_Delete( p_wk->p_mdlres[i] );	
		GFL_G3D_RENDER_Delete(p_wk->rnder[i]);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃI�u�W�F�̍쐬
 *
 *	@param	p_wk		���[�N
 *	@param	p_res		���\�[�X 
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Add( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res )
{
	int i;

	p_wk->on = TRUE;
	
	for( i=0; i<WFLBY_3DMAPOBJ_MAPOBJ_NUM; i++ ){
	#if WB_FIX
		D3DOBJ_Init( &p_wk->obj[i],  &p_res->mdl[i] );
		// �`��J�n
		D3DOBJ_SetDraw( &p_wk->obj[i], TRUE );
	#else
		p_wk->g3dobj[i] = GFL_G3D_OBJECT_Create( p_res->rnder[ i ], p_res->anm, WFLBY_3DMAPOBJ_MAP_ANM_NUM );
		WFLBY_G3DOBJSTATUS_Init(&p_wk->objst[i], &p_wk->objrotate[i]);
		// �`��J�n
		p_wk->draw_flag[i] = TRUE;
	#endif
	
		// ���O���b�h���炷	
//		D3DOBJ_SetMatrix( &p_wk->obj[i], -WFLBY_3DMATRIX_GRID_GRIDSIZ_HALF, 0, 0 );	
	}

	// POLL�ȊO�̃A�j�����J�n
	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM-1; i++ ){

		if( p_res->anm_load[i] == TRUE ){
			p_wk->anm_on[i] = TRUE;

			// �V�[�g�p�}�b�g�A�j���ȊO��MAP�ɃA�j���ǉ�
			if( i != WFLBY_3DMAPOBJ_MAP_ANM_MAT ){

			#if WB_FIX
				D3DOBJ_AddAnm( &p_wk->obj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], 
						&p_res->anm[i] );
			#else
				GFL_G3D_OBJECT_EnableAnime( p_wk->g3dobj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], i );
			#endif
			}else{
			#if WB_FIX
				D3DOBJ_AddAnm( &p_wk->obj[WFLBY_3DMAPOBJ_MAPOBJ_MAT], 
						&p_res->anm[i] );
			#else
				GFL_G3D_OBJECT_EnableAnime( p_wk->g3dobj[WFLBY_3DMAPOBJ_MAPOBJ_MAT], i );
			#endif
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�I�u�W�F	�j��
 *
 *	@param	p_wk	���[�N
 *	@param	p_res	���\�[�X���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Delete( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res )
{
	int i, k;

	// �S�A�j�����͂���
	for( k=0; k<WFLBY_3DMAPOBJ_MAPOBJ_NUM; k++){
  	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM-1; i++ ){

  		if( p_wk->anm_on[i] == TRUE ){
  			p_wk->anm_on[i] = FALSE;

  			// �V�[�g�p�}�b�g�A�j���ȊO��MAP�ɃA�j���ǉ�
  			if( i != WFLBY_3DMAPOBJ_MAP_ANM_MAT ){

  			#if WB_FIX
  				D3DOBJ_DelAnm( &p_wk->obj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], 
  						&p_res->anm[i] );
  			#else
  				GFL_G3D_OBJECT_DisableAnime( p_wk->g3dobj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], i );
  			#endif
  			}else{
  			#if WB_FIX
  				D3DOBJ_DelAnm( &p_wk->obj[WFLBY_3DMAPOBJ_MAPOBJ_MAT], 
  						&p_res->anm[i] );
  			#else
  				GFL_G3D_OBJECT_DisableAnime( p_wk->g3dobj[WFLBY_3DMAPOBJ_MAPOBJ_MAT], i );
  			#endif
  			}
  		}
  	}
		GFL_G3D_OBJECT_Delete( p_wk->g3dobj[k] );
		p_wk->g3dobj[k] = NULL;
	}
	
	p_wk->on = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v���C������
 *
 *	@param	p_wk	���[�N
 *	@param	p_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Main( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res )
{
	int i, set_anmframe;
	int k;
	
	// �A�j�����J�n���Ă�����t���[����ݒ肷��
	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM; i++ ){
		if( p_wk->anm_on[i] ){
			WFLBY_3DMAPOBJ_ANM_Loop( &p_wk->anm_frame[i], p_res->anm[i], FX32_ONE );
			// �t���[�������킹��
		#if WB_FIX
			D3DOBJ_AnmSet( &p_res->anm[i], p_wk->anm_frame[i] );
		#else
			for(k = 0; k < WFLBY_3DMAPOBJ_MAPOBJ_NUM; k++){
				set_anmframe = p_wk->anm_frame[i];
				GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj[k], i, &set_anmframe );
			}
		#endif
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�`��
 *
 *	@param	p_wk	���[�N
 *	@param	p_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Draw( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res )
{
	int i, j;
	if( p_wk->on ){
		for( i=0; i<WFLBY_3DMAPOBJ_MAPOBJ_NUM; i++ ){

		#if WB_FIX
			D3DOBJ_Draw( &p_wk->obj[i] );
		#else
			if(p_wk->draw_flag[i] == TRUE){
				GFL_G3D_DRAW_DrawObjectCullingON(p_wk->g3dobj[i], &p_wk->objst[i]);
			}
		#endif
		
		}
	}
}


// �t���[�g���[�N
//----------------------------------------------------------------------------
/**
 *	@brief	�󂢂Ă��郏�[�N���擾����
 *
 *	@param	p_sys		���[�N
 *
 *	@return	�󂢂Ă��郏�[�N
 */
//-----------------------------------------------------------------------------
static WFLBY_3DMAPOBJ_FLOAT* WFLBY_3DMAPOBJ_FLOAT_GetCleanWk( WFLBY_3DMAPOBJ* p_sys )
{
	int i;

	for( i=0; i<p_sys->floatnum; i++ ){
		if( p_sys->p_float[i].on == FALSE ){
			return &p_sys->p_float[i];
		}
	}

	GF_ASSERT(0);
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g�̓ǂݍ���
 *	
 *	@param	p_wk		���[�N
 *	@param	p_handle	�n���h��
 *	@param	cp_data		�f�[�^
 *	@param	gheapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_FLOAT_Load( WFLBY_3DMAPOBJ_FLOATRES* p_wk, ARCHANDLE* p_handle, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID )
{
	int i, j, k;

	// �e�N�X�`���ǂݍ���
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
			for( j=0; j<WFLBY_3DMAPOBJ_FLOAT_COL_NUM; j++ ){
				WFLBY_3DMAPOBJ_TEX_LoatCutTex( &p_wk->p_texres[i][j], p_handle,
						cp_data->float_texidx[i][j], gheapID );
			}
		}
	}

	// ���f���ǂݍ���
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){

		#if WB_FIX	//GFL_G3D_RENDER_Create�̓����ōs���̂ł��K�v�Ȃ�

			// ���f����ǂݍ���
			p_wk->mdl[i].pResMdl	= GFL_ARCHDL_UTIL_Load( p_handle, 
						cp_data->float_idx[i], FALSE, gheapID );
			p_wk->mdl[i].pModelSet	= NNS_G3dGetMdlSet( p_wk->mdl[i].pResMdl );
			p_wk->mdl[i].pModel		= NNS_G3dGetMdlByIdx( p_wk->mdl[i].pModelSet, 0 );

			// �e�N�X�`���͌�Ńo�C���h����
			// �Ƃ肠�������̓A�j�������p�ɂO�̂��̂�����
			p_wk->mdl[i].pMdlTex = NNS_G3dGetTex( p_wk->p_texres[i][0] );
		#else
			p_wk->p_mdlres[i] = GFL_G3D_CreateResourceHandle( p_handle, cp_data->float_idx[i] ) ;
		#endif
		}
	}

	//�����_�[�쐬
	{
		for(i = 0; i < WFLBY_3DMAPOBJ_FLOAT_NUM; i++){
			// �e�N�X�`���͌�Ńo�C���h����
			// �Ƃ肠�������̓A�j�������p�ɂO�̂��̂�����
			p_wk->rnder[i] = GFL_G3D_RENDER_Create( p_wk->p_mdlres[i], 0, p_wk->p_texres[i][0] );
		}
	}
	
	// �A�j���̓ǂݍ���
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
			for( j=0; j<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; j++ ){
	
				// ���f��IDX�ƃA�j��IDX����v������ǂݍ��܂Ȃ�
				if( cp_data->float_idx[0] != cp_data->float_anmidx[i][j] ){

				#if WB_FIX
					D3DOBJ_AnmLoadH( &p_wk->anm[i][j], &p_wk->mdl[i], p_handle, 
							cp_data->float_anmidx[i][j], gheapID );
				#else
					p_wk->p_anmres[i][j] = GFL_G3D_CreateResourceHandle( p_handle, 
						cp_data->float_anmidx[i][j] ) ;
					p_wk->anm[i][j] = GFL_G3D_ANIME_Create( 
						p_wk->rnder[i], p_wk->p_anmres[i][j], 0 ); 
				#endif
				
					// ���ƐK���i�����j�̃A�j���Ȃ甽�f��m�[�hID��ݒ�
					if( j >= WFLBY_3DMAPOBJ_FLOAT_ANM_01 ){

						// ���f���m�[�h�����A�j���ݒ��OFF
						for( k=0; k<cp_data->float_nodenum[i]; k++ ){
							if( cp_data->float_anm_node[i][j-WFLBY_3DMAPOBJ_FLOAT_ANM_01] != k ){
							#if WB_FIX
								NNS_G3dAnmObjDisableID( p_wk->anm[i][j].pAnmObj, k );
							#else
								NNS_G3dAnmObjDisableID(
									GFL_G3D_ANIME_GetAnmObj(p_wk->anm[i][j]), k);
							#endif
							}
						}
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g���\�[�X�j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_FLOAT_Release( WFLBY_3DMAPOBJ_FLOATRES* p_wk )
{
	int i, j;

  // �����_�[�̔j��
  for(i = 0; i < WFLBY_3DMAPOBJ_FLOAT_NUM; i++){
    GFL_G3D_RENDER_Delete(p_wk->rnder[i]);
  }
  
	// �A�j���̔j��
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
			for( j=0; j<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; j++ ){
			#if WB_FIX
				D3DOBJ_AnmDelete( &p_wk->anm[i][j], p_allocator );
			#else
				GFL_G3D_ANIME_Delete( p_wk->anm[i][j] );
				GFL_G3D_DeleteResource( p_wk->p_anmres[i][j] );
			#endif
			}
		}
	}

	// ���f���j��
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
		#if WB_FIX
			GFL_HEAP_FreeMemory( p_wk->mdl[i].pResMdl );
		#else
			GFL_G3D_DeleteResource(p_wk->p_mdlres[i]);
		#endif
		}
	}

	// �e�N�X�`���j��
	{
		NNSG3dTexKey texKey;
		NNSG3dTexKey tex4x4Key;
		NNSG3dPlttKey plttKey;
		NNSG3dResTex* p_tex;


		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
			for( j=0; j<WFLBY_3DMAPOBJ_FLOAT_COL_NUM; j++ ){
			#if WB_FIX
				p_tex = NNS_G3dGetTex( p_wk->p_texres[i][j] );
				//VramKey�j��
				NNS_G3dTexReleaseTexKey( p_tex, &texKey, &tex4x4Key );
				NNS_GfdFreeTexVram( texKey );	
				NNS_GfdFreeTexVram( tex4x4Key );	

				plttKey = NNS_G3dPlttReleasePlttKey( p_tex );
				NNS_GfdFreePlttVram( plttKey );

				// �������j��
				GFL_HEAP_FreeMemory( p_wk->p_texres[i][j] );
			#else
				//�����Ő�����Ԃ悤�Ȃ�WFLBY_3DMAPOBJ_TEX_LoatCutTex�Ńe�N�X�`���q�[�v��
				//���̕����̉�������Ă��邩�炩�� 2009.03.25(��) matsuda
				GFL_G3D_FreeVramTexture(p_wk->p_texres[i][j]);
				GFL_G3D_DeleteResource(p_wk->p_texres[i][j]);
			#endif
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g�@���C������
 *
 *	@param	p_wk		���[�N
 *	@param	p_res		���\�[�X
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_FLOAT_Main( WFLBY_3DMAPOBJ_FLOAT* p_wk, WFLBY_3DMAPOBJ_FLOATRES* p_res )
{
	int i;

	// �A�j��������
	{
		fx32 end_frame;
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; i++ ){
			if( p_wk->anm_on[i] ){
				switch(i){
				case WFLBY_3DMAPOBJ_FLOAT_ANM_00:
				case WFLBY_3DMAPOBJ_FLOAT_ANM_02:		// ��
//					OS_TPrintf( "anm roop %d=%d  ",  i, p_wk->anm_frame[i] );
					WFLBY_3DMAPOBJ_ANM_Loop( &p_wk->anm_frame[i], p_res->anm[ p_wk->mdlno ][ i ], FX32_ONE );
					break;
				case WFLBY_3DMAPOBJ_FLOAT_ANM_01:		// �������p
				case WFLBY_3DMAPOBJ_FLOAT_ANM_03:		// �K��
					{
						BOOL result;
//						OS_TPrintf( "anm noroop %d=%d  ",  i, p_wk->anm_frame[i] );
						result = WFLBY_3DMAPOBJ_ANM_NoLoop( &p_wk->anm_frame[i], p_res->anm[ p_wk->mdlno ][ i ], FX32_ONE );
						if( result == TRUE ){
							// �A�j����~
							p_wk->anm_on[i] = FALSE;
							p_wk->anm_frame[i] = 0;
						}
					}
					break;
				default:
					GF_ASSERT(0);
					break;
				}
			}
		}
	}			
//	OS_TPrintf( "\n",  i, p_wk->anm_frame[i] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���[�g�̕`�揈��
 *
 *	@param	p_wk	���[�N
 *	@param	p_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_FLOAT_Draw( WFLBY_3DMAPOBJ_FLOAT* p_wk, WFLBY_3DMAPOBJ_FLOATRES* p_res )
{
	BOOL result;
	int i, set_anmframe;
	GFL_G3D_RND *g3dRnd;
	
	if( p_wk->on == FALSE ){
		return ;
	}

	g3dRnd = GFL_G3D_OBJECT_GetG3Drnd(p_wk->g3dobj);
	
	// FLOAT�`��`�F�b�N
#if WB_FIX	//�`��̕��ŃJ�����OON�ł���ׁA�����̏����͂Ȃ�
	if( WFLBY_3DMAPOBJ_MDL_BOXCheck( &p_res->mdl[ p_wk->mdlno ], &p_wk->obj ) == FALSE ){
		return ;
	}
#endif

	GF_ASSERT( p_wk->col < WFLBY_3DMAPOBJ_FLOAT_COL_NUM );
	GF_ASSERT( p_wk->mdlno < WFLBY_3DMAPOBJ_FLOAT_NUM );


	// �J���[�̃e�N�X�`�����o�C���h
#if WB_FIX
	p_res->mdl[ p_wk->mdlno ].pMdlTex = NNS_G3dGetTex( p_res->p_texres[ p_wk->mdlno ][ p_wk->col ] );
	result = NNS_G3dBindMdlSet(p_res->mdl[ p_wk->mdlno ].pModelSet, p_res->mdl[ p_wk->mdlno ].pMdlTex );
	GF_ASSERT( result );
#else
	GFL_G3D_RENDER_SetTexture( g3dRnd, 0, p_res->p_texres[ p_wk->mdlno ][ p_wk->col ] );
#endif

	// �A�j���̓K�p
	for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; i++ ){
		if( p_wk->anm_on[i] ){
		#if WB_FIX
			D3DOBJ_AddAnm( &p_wk->obj, &p_res->anm[ p_wk->mdlno ][ i ] );
			// �t���[�������킹��
			D3DOBJ_AnmSet( &p_res->anm[ p_wk->mdlno ][ i ], p_wk->anm_frame[i] );
		#else
			GFL_G3D_OBJECT_EnableAnime( p_wk->g3dobj, i );
			// �t���[�������킹��
			set_anmframe = p_wk->anm_frame[i];
			GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj, i, &set_anmframe );
		#endif
		}
	}

#if WB_FIX
	D3DOBJ_Draw( &p_wk->obj );
#else
	if(p_wk->draw_flag == TRUE){
		GFL_G3D_DRAW_DrawObjectCullingON(p_wk->g3dobj, &p_wk->objst);
	}
#endif

	// �A�j����j��
	for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; i++ ){
		if( p_wk->anm_on[i] ){
		#if WB_FIX
			D3DOBJ_DelAnm( &p_wk->obj, &p_res->anm[ p_wk->mdlno ][ i ] );
		#else
			GFL_G3D_OBJECT_DisableAnime(p_wk->g3dobj, i);
		#endif
		}
	}

	// �o�C���h����
#if WB_FIX
	NNS_G3dReleaseMdlSet(p_res->mdl[ p_wk->mdlno ].pModelSet );
#else
	{
		NNSG3dResFileHeader *file_head;
		file_head = GFL_G3D_GetResourceFileHeader(GFL_G3D_RENDER_GetG3DresMdl(g3dRnd));
		NNS_G3dReleaseMdlSet( NNS_G3dGetMdlSet( file_head ) );
	}
#endif
}

// �z�u�I�u�W�F�N�g���[�N
//----------------------------------------------------------------------------
/**
 *	@brief	�󂢂Ă��郏�[�N���擾����
 *
 *	@param	p_sys	�V�X�e�����[�N
 *	
 *	@return	�󂢂Ă��郏�[�N
 */
//-----------------------------------------------------------------------------
static WFLBY_3DMAPOBJ_WK* WFLBY_3DMAPOBJ_WK_GetCleanWk( WFLBY_3DMAPOBJ* p_sys )
{
	int i;

	for( i=0; i<p_sys->objnum; i++ ){
		if( p_sys->p_obj[i].on == FALSE ){
			return &p_sys->p_obj[i];
		}
	}

	GF_ASSERT(0);
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�z�u�I�u�W�F���\�[�X�ǂݍ���
 *
 *	@param	p_wk		���[�N
 *	@param	p_handle	�n���h��
 *	@param	cp_data		�f�[�^
 *	@param	gheapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_WK_Load( WFLBY_3DMAPOBJ_WKRES* p_wk, ARCHANDLE* p_handle, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID )
{
	int i, j;

	for( i=0; i<WFLBY_3DMAPOBJ_WK_NUM; i++ ){
//		OS_TPrintf( "wk load mdl idx = %d\n",  cp_data->obj_idx[i] );
		WFLBY_3DMAPOBJ_MDL_Load( &p_wk->p_mdlres[i], p_handle, cp_data->obj_idx[i], gheapID );
		p_wk->rnder[i] = GFL_G3D_RENDER_Create( p_wk->p_mdlres[i], 0, p_wk->p_mdlres[i] );

		// ���C�g�ݒ�
		if( (i != WFLBY_3DMAPOBJ_LAMP00) && 
			(i != WFLBY_3DMAPOBJ_LAMP01) ){
		#if WB_FIX
			WFLBY_LIGHT_SetUpMdl( p_wk->mdl[ i ].pResMdl );	// ���C�g���f
		#else
			WFLBY_LIGHT_SetUpMdl( GFL_G3D_GetResourceFileHeader(p_wk->p_mdlres[ i ]) );	// ���C�g���f
		#endif
		}

		// ���f�����̃A�j����ǂݍ���
		for( j=0; j<WFLBY_3DMAPOBJ_ALL_ANM_MAX; j++ ){
			
			// �I�u�W�F��ID�Ɠ����Ȃ�ǂݍ��܂Ȃ�
			if( cp_data->obj_anm[i][j] != cp_data->obj_idx[i] ){
			#if WB_FIX
				p_wk->p_anm[i][j] = GFL_ARCHDL_UTIL_Load( p_handle, cp_data->obj_anm[i][j], FALSE, gheapID );
			#else
				p_wk->p_anmres[i][j] = GFL_G3D_CreateResourceHandle(p_handle, cp_data->obj_anm[i][j]);
			#endif
			}else{
				p_wk->p_anmres[i][j] = NULL;
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�z�u�I�u�W�F���\�[�X�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_WK_Release( WFLBY_3DMAPOBJ_WKRES* p_wk )
{
	int i, j;

	for( i=0; i<WFLBY_3DMAPOBJ_WK_NUM; i++ ){

		// �A�j�����\�[�X��j��
		for( j=0; j<WFLBY_3DMAPOBJ_ALL_ANM_MAX; j++ ){
/*			if( p_wk->anm[i][j].pResAnm != NULL ){
				D3DOBJ_AnmDelete( &p_wk->anm[i][j], p_allocator );
				p_wk->anm[i][j].pResAnm = NULL;
			}//*/

			if( p_wk->p_anmres[i][j] != NULL ){
			#if WB_FIX
				GFL_HEAP_FreeMemory( p_wk->p_anm[i][j] );
			#else
				GFL_G3D_DeleteResource( p_wk->p_anmres[i][j] );
			#endif
				p_wk->p_anmres[i][j] = NULL;
			}

		}
		
		WFLBY_3DMAPOBJ_MDL_Delete( p_wk->p_mdlres[i] );
		GFL_G3D_RENDER_Delete(p_wk->rnder[i]);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�N�g�`��
 *
 *	@param	p_res	���\�[�X�I�u�W�F
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_WK_Draw( WFLBY_3DMAPOBJ_WKRES* p_res, WFLBY_3DMAPOBJ_WK* p_wk )
{
	int i;
	NNSG3dResMdlSet*		pMdlset;
	NNSG3dResMdl*			pMdl;

	if( p_wk->on == FALSE ){
		return ;
	}

	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_INIT;
	
	// �`��`�F�b�N
#if WB_FIX	//�`��̕��ŃJ�����OON�ł���ׁA�����̏����͂Ȃ�
	if( WFLBY_3DMAPOBJ_MDL_BOXCheck( &p_res->mdl[ p_wk->mdlid ], &p_wk->obj ) == FALSE ){
		return ;
	}
#endif
	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );


	/*
	// �A�j��i�̃t���[���ݒ�
	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
		// �A�j����ON�ɂȂ��Ă���RANDWAIT��wait���I����Ă���A�j���t���[���ݒ�
		if( p_wk->anm_on[i] && (p_wk->wait[i] == 0) ){
			// �t���[�������킹��
			D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
		}
	}
	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );
	//*/

	pMdlset = NNS_G3dGetMdlSet(GFL_G3D_GetResourceFileHeader(p_res->p_mdlres[p_wk->mdlid]));
	pMdl = NNS_G3dGetMdlByIdx( pMdlset, 0 );

	// �A���t�@�l�̓K�p
	if( p_wk->alpha_flag == TRUE ){
		NNS_G3dMdlSetMdlAlphaAll( pMdl, p_wk->alpha );
	}

#if WB_FIX
	D3DOBJ_Draw( &p_wk->obj );
#else
	if(p_wk->draw_flag == TRUE){
		GFL_G3D_DRAW_DrawObjectCullingON(p_wk->g3dobj, &p_wk->objst);
	}
#endif
	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );

	// �A���t�@�l��߂�
	if( p_wk->alpha_flag == TRUE ){
		NNS_G3dMdlSetMdlAlphaAll( pMdl, p_wk->def_alpha );
	}

}


//----------------------------------------------------------------------------
/**
 *	@brief	�N�����̃A�j�������[�v�Đ�
 *
 *	@param	p_sys	�V�X�e�����[�N
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_WK_Anm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	int i, set_anmframe;
	BOOL result;
	pWFLBY_3DMAPOBJ_WK_AnmCallBack p_callback;

	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
		if( p_wk->anm_on[i] ){
			switch( p_wk->play[i] ){
			// ���[�v
			case WFLBY_3DMAPOBJ_WK_ANM_LOOP:
				WFLBY_3DMAPOBJ_ANM_Loop( &p_wk->anm_frame[i], p_wk->anm[i], p_wk->speed );
			#if WB_FIX
				D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
			#else
				set_anmframe = p_wk->anm_frame[i];
				GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj, i, &set_anmframe );
			#endif
				break;

			// ���[�v�Ȃ�
			case WFLBY_3DMAPOBJ_WK_ANM_NOLOOP:
				result = WFLBY_3DMAPOBJ_ANM_NoLoop( &p_wk->anm_frame[i], p_wk->anm[i], p_wk->speed );
			#if WB_FIX
				D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
			#else
				set_anmframe = p_wk->anm_frame[i];
				GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj, i, &set_anmframe );
			#endif
				if( result == TRUE ){
					
					// �ۑ�
					p_callback = p_wk->p_anmcallback[i];

					//  �A�j����j��
					WFLBY_3DMAPOBJ_WK_DelAnm( p_sys, p_wk, i );

					// �R�[���o�b�N����������Ă�
					if( p_callback ){
						p_callback( p_sys, p_wk );
					}

				}
				break;

			// �t�Đ����[�v
			case WFLBY_3DMAPOBJ_WK_ANM_BACKLOOP:
				WFLBY_3DMAPOBJ_ANM_BackLoop( &p_wk->anm_frame[i], p_wk->anm[i], p_wk->speed );
			#if WB_FIX
				D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
			#else
				set_anmframe = p_wk->anm_frame[i];
				GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj, i, &set_anmframe );
			#endif
				break;

			// �t�Đ����[�v�Ȃ�
			case WFLBY_3DMAPOBJ_WK_ANM_BACKNOLOOP:
				result = WFLBY_3DMAPOBJ_ANM_BackNoLoop( &p_wk->anm_frame[i], p_wk->anm[i], p_wk->speed );
			#if WB_FIX
				D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
			#else
				set_anmframe = p_wk->anm_frame[i];
				GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj, i, &set_anmframe );
			#endif
				if( result == TRUE ){
					// �ۑ�
					p_callback = p_wk->p_anmcallback[i];

					//  �A�j����j��
					WFLBY_3DMAPOBJ_WK_DelAnm( p_sys, p_wk, i );

					// �R�[���o�b�N����������Ă�
					if( p_callback ){
						p_callback( p_sys, p_wk );
					}

				}
				break;

			// �����_���E�G�C�g�Đ�	���[�v
			case WFLBY_3DMAPOBJ_WK_ANM_RANDWAIT:
				// �E�G�C�g���Ă���Đ�
				if( p_wk->wait[i] > 0 ){
					p_wk->wait[i]--;
				}else{
					result = WFLBY_3DMAPOBJ_ANM_NoLoop( &p_wk->anm_frame[i], p_wk->anm[i], p_wk->speed );
					if( result == TRUE ){
						// ���[�v�Đ�
						p_wk->wait[i] = GFUser_GetPublicRand(p_wk->wait_def);
						p_wk->anm_frame[i] = 0;
					}
				#if WB_FIX
					D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
				#else
					set_anmframe = p_wk->anm_frame[i];
					GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj, i, &set_anmframe );
				#endif
				}
				break;

			// �����_���X�^�[�g�Đ�	���[�v
			case WFLBY_3DMAPOBJ_WK_ANM_RANDSTART:
				// �E�G�C�g���Ă���Đ�
				if( p_wk->wait[i] > 0 ){
					p_wk->wait[i]--;
				}else{
					WFLBY_3DMAPOBJ_ANM_Loop( &p_wk->anm_frame[i], p_wk->anm[i], p_wk->speed );
				#if WB_FIX
					D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
				#else
					set_anmframe = p_wk->anm_frame[i];
					GFL_G3D_OBJECT_SetAnimeFrame( p_wk->g3dobj, i, &set_anmframe );
				#endif
				}
				break;

			// �Ȃɂ����Ȃ�
			case WFLBY_3DMAPOBJ_WK_ANM_NOAUTO:
			default:
				break;

			}

		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   GFL_G3D_OBJSTATUS�̏����p�����[�^�Z�b�g
 *
 * @param   status		
 */
//--------------------------------------------------------------
static void WFLBY_G3DOBJSTATUS_Init(GFL_G3D_OBJSTATUS *status, VecFx32 *rotate)
{
	VEC_Set(&status->trans, 0, 0, 0);
	VEC_Set(&status->scale, FX32_ONE, FX32_ONE, FX32_ONE);
	MTX_Identity33(&status->rotate);
	
	VEC_Set(rotate, 0, 0, 0);
}

//--------------------------------------------------------------
/**
 * @brief   ��]�s��̌v�Z
 *
 * @param   status		
 * @param   rotate		
 */
//--------------------------------------------------------------
static void WFLBY_RotateMtx(VecFx32 *rotate, MtxFx33 *dst_mtx)
{
	MtxFx33 mtx, calc_mtx;
	
	MTX_Identity33( &mtx );
	MTX_RotX33( &calc_mtx, FX_SinIdx( rotate->x ), FX_CosIdx( rotate->x ) );
	MTX_Concat33( &calc_mtx, &mtx, &mtx );
	MTX_RotZ33( &calc_mtx, FX_SinIdx( rotate->z ), FX_CosIdx( rotate->z ) );
	MTX_Concat33( &calc_mtx, &mtx, &mtx );
	MTX_RotY33( &calc_mtx, FX_SinIdx( rotate->y ), FX_CosIdx( rotate->y ) );
	MTX_Concat33( &calc_mtx, &mtx, &mtx );
	
	*dst_mtx = mtx;
}

