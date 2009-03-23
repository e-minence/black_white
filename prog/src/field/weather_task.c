//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_task.c
 *	@brief		�P�V�C�@����Ǘ�
 *	@author		tomoya takahshi
 *	@data		2009.03.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "weather_task.h"

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
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WEATHER_TASK
//=====================================
#define WEATHER_TASK_OBJBUFF_MAX	( 48 )

// ��ԑJ��
enum{
	WEATHER_TASK_SEQ_NONE,				// �Ȃ�
	WEATHER_TASK_SEQ_INIT_ALL,			// ��C�ɏ�����
	WEATHER_TASK_SEQ_INIT_DIV_OAM,		// �����ǂݍ��� OAM
	WEATHER_TASK_SEQ_INIT_DIV_BG,		// �����ǂݍ��� BG
	WEATHER_TASK_SEQ_INIT_DIV_CREATE,	// �����ǂݍ��� �\�z
	WEATHER_TASK_SEQ_CALL_INIT,			// �Ǘ��֐��@������	�@		�Ăяo��
	WEATHER_TASK_SEQ_CALL_FADEIN,		// �Ǘ��֐�  �t�F�[�h�C��	�Ăяo��
	WEATHER_TASK_SEQ_CALL_NOFADE,		// �Ǘ��֐�	 �t�F�[�h�Ȃ�	�Ăяo��
	WEATHER_TASK_SEQ_CALL_MAIN,			// �Ǘ��֐��@���C��			�Ăяo��
	WEATHER_TASK_SEQ_CALL_FADEOUT_INIT,	// �Ǘ��֐��@�t�F�[�h�A�E�g �Ăяo��
	WEATHER_TASK_SEQ_CALL_FADEOUT,		// �Ǘ��֐��@�t�F�[�h�A�E�g �Ăяo��
	WEATHER_TASK_SEQ_CALL_DEST,			// �Ǘ��֐��@�j��			�Ăяo��
	WEATHER_TASK_SEQ_DELETE_ALL,		// �S���̔j��

	WEATHER_TASK_SEQ_NUM,				// �V�X�e�����Ŏg�p
};


//-------------------------------------
///	�O���t�B�b�N�萔
//=====================================
#define WEATHER_GRAPHIC_OAM_LOAD_DRAW_TYPE	( CLSYS_DRAW_MAIN )


//-------------------------------------
///	�V�C�I�u�W�F�萔
//=====================================
#define WEATHER_OBJ_WORK_BG_PRI		( 0 )
#define WEATHER_OBJ_WORK_SOFT_PRI	( 64 )
#define WEATHER_OBJ_WORK_SERFACE_NO	( CLSYS_DEFREND_MAIN )


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�C�P�I�u�W�F�i��̗��ȂǗp�@OAM�j
//=====================================
struct _WEATHER_OBJ_WORK {
	// ����܂�D���ȏ���������Ȃ����ǁE�E�E
	// �e�̃��[�N�|�C���^
	WEATHER_TASK* p_parent;

	// �`�惏�[�N
	GFL_CLWK* p_clwk;

	// ���[�U���[�N
	u8	user_work[ WEATHER_OBJ_WORK_USE_WORKSIZE ];

	// �����N���
	WEATHER_OBJ_WORK* p_last;
	WEATHER_OBJ_WORK* p_next;
};

//-------------------------------------
///	�O���t�B�b�N���
//=====================================
typedef struct {
	u16 oam_load;
	u16	oam_cgx_id;
	u16	oam_pltt_id;
	u16 oam_cell_id;

	ARCHANDLE* p_handle;
} WEATHER_TASK_GRAPHIC;

//-------------------------------------
///	���[�v�����
//=====================================
typedef struct {
	u16	play;
	u16 snd_no;
} WEATHER_TASK_SND_LOOP;


//-------------------------------------
///	�I�u�W�F�N�g�ǉ��t�F�[�h
//=====================================
typedef struct {
	WEATHER_TASK* p_parent;

	// �J�E���^
	s16 obj_addnum;		// �I�u�W�F�N�g�o�^��
	s16 obj_addtmg;		// �o�^�^�C�~���O�J�E���^
	s16 obj_addtmg_max;	// �o�^�^�C�~���O
	s16 obj_addnum_sum;	// �I�u�W�F�N�g�o�^�������J�E���^
	
	// �萔
	s16	obj_add_num_end;		// �I�u�W�F�N�g�P��̓o�^���@�I���l
	s16	obj_add_tmg_end;		// �o�^�^�C�~���O�J�E���^�@�I���l
	s16	obj_add_tmg_sum;		// �o�^�^�C�~���O�J�E���^�ύX�l
	s16 obj_add_num_sum_tmg;	// �I�u�W�F�N�g�P��̓o�^����ύX����^�C�~���O
	s32 obj_add_num_sum;		// �ύX����l

	// �֐�
	WEATHER_TASK_OBJADD_FUNC* p_addfunc;

} WEATHER_TASK_OBJ_FADE;

//-------------------------------------
///	�X�N���[�����W�Ǘ�
//=====================================
typedef struct {
	VecFx32 last_target;
} WEATHER_TASK_SCROLL_WORK;


//-------------------------------------
///	�V�C�^�X�N
//=====================================
struct _WEATHER_TASK {

	// �V�C��{���
	const WEATHER_TASK_DATA*	cp_data;
	u8							fog_cont;	// �t�H�O����̋���
	u8							fade;		// �t�F�[�h�L��
	u8							init_mode;	// ���������[�h
	u8							seq;		// �S�̃V�[�P���X

	// �O���t�B�b�N���
	WEATHER_TASK_GRAPHIC graphic;
	
	// GFL_CLUNIT
	GFL_CLUNIT* p_unit;
	
	// �t�H�O���
	
	// ���C�g���
	
	// �J�������
	const FIELD_CAMERA* cp_camera;
	
	// ���[�U���[�N
	void*	p_user_work;

	// �����
	WEATHER_TASK_SND_LOOP	snd_loop;

	// ����I�u�W�F�ǉ��t�F�[�h�V�X�e��
	WEATHER_TASK_OBJ_FADE obj_fade;

	// �t�H�O�t�F�[�h�V�X�e��
	
	// �X�N���[���Ǘ����[�N
	WEATHER_TASK_SCROLL_WORK scroll;
	
	// ����I�u�W�F���[�N
	WEATHER_OBJ_WORK	objbuff[ WEATHER_TASK_OBJBUFF_MAX ];
	WEATHER_OBJ_WORK*	p_objlist;
	u32					active_objnum;

};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�X�e���Ǘ�
//=====================================
static void WEATHER_TASK_WK_Clear( WEATHER_TASK* p_wk );
static WEATHER_TASK_FUNC_RESULT WEATHER_TASK_WK_CallFunc( WEATHER_TASK* p_wk, WEATHER_TASK_FUNC* p_func, u32 heapID );
static WEATHER_TASK_INFO WEATHER_TASK_WK_GetInfo( const WEATHER_TASK* cp_wk );
static WEATHER_OBJ_WORK* WEATHER_TASK_WK_GetClearObj( WEATHER_TASK* p_wk );
static void WEATHER_TASK_WK_PushObjList( WEATHER_TASK* p_wk, WEATHER_OBJ_WORK* p_obj );
static void WEATHER_TASK_WK_PopObjList( WEATHER_TASK* p_wk, WEATHER_OBJ_WORK* p_obj );
static void WEATHER_TASK_WK_MainObjList( WEATHER_TASK* p_wk );
static void WEATHER_TASK_WK_InitOam( WEATHER_TASK* p_wk, u32 heapID );
static void WEATHER_TASK_WK_ExitOam( WEATHER_TASK* p_wk );
static void WEATHER_TASK_WK_InitBg( WEATHER_TASK* p_wk, u32 heapID );
static void WEATHER_TASK_WK_ExitBg( WEATHER_TASK* p_wk );
static void WEATHER_TASK_WK_InitOther( WEATHER_TASK* p_wk, u32 heapID );
static void WEATHER_TASK_WK_ExitOther( WEATHER_TASK* p_wk );


//-------------------------------------
///	�O���t�B�b�N�V�X�e��
//=====================================
static void WEATHER_TASK_GRAPHIC_Clear( WEATHER_TASK_GRAPHIC* p_wk );
static void WEATHER_TASK_GRAPHIC_InitHandle( WEATHER_TASK_GRAPHIC* p_wk, const WEATHER_TASK_DATA* cp_data, u32 heapID );
static void WEATHER_TASK_GRAPHIC_ExitHandle( WEATHER_TASK_GRAPHIC* p_wk );
static void WEATHER_TASK_GRAPHIC_InitOam( WEATHER_TASK_GRAPHIC* p_wk, const WEATHER_TASK_DATA* cp_data, u32 heapID );
static void WEATHER_TASK_GRAPHIC_ExitOam( WEATHER_TASK_GRAPHIC* p_wk );
static void WEATHER_TASK_GRAPHIC_InitBg( WEATHER_TASK_GRAPHIC* p_wk, const WEATHER_TASK_DATA* cp_data, u32 heapID );
static void WEATHER_TASK_GRAPHIC_ExitBg( WEATHER_TASK_GRAPHIC* p_wk );


//-------------------------------------
///	�I�u�W�F�N�g�ǉ��t�F�[�h
//=====================================
static void WEATHER_OBJ_FADE_InitFade( WEATHER_TASK_OBJ_FADE* p_wk, WEATHER_TASK* p_sys, s32 objAddTmgMax, s32 objAddNum, s32 obj_add_num_end, s32 obj_add_tmg_end, s32 obj_add_tmg_sum, s32 obj_add_num_sum_tmg, s32 obj_add_num_sum, WEATHER_TASK_OBJADD_FUNC* p_addfunc );

static void WEATHER_OBJ_FADE_SetFadeOut( WEATHER_TASK_OBJ_FADE* p_wk, s32 obj_add_num_end, s32 obj_add_tmg_end, s32 obj_add_tmg_sum, s32 obj_add_num_sum );
static BOOL WEATHER_OBJ_FADE_Main( WEATHER_TASK_OBJ_FADE* p_wk, u32 heapID );
static void WEATHER_TASK_OBJ_FADE_NoFadeMain( WEATHER_TASK_OBJ_FADE* p_wk, u32 heapID );


//-------------------------------------
///	�V�C�I�u�W�F���[�N�Ǘ�
//=====================================
static void WEATHER_OBJ_WK_Clear( WEATHER_OBJ_WORK* p_wk );
static BOOL WEATHER_OBJ_WK_IsUse( const WEATHER_OBJ_WORK* cp_wk );
static void WEATHER_OBJ_WK_Init( WEATHER_OBJ_WORK* p_wk, WEATHER_TASK* p_parent, const WEATHER_TASK_GRAPHIC* cp_graphic, GFL_CLUNIT* p_unit, u32 heapID );
static void WEATHER_OBJ_WK_Exit( WEATHER_OBJ_WORK* p_wk );
static void* WEATHER_OBJ_WK_GetWork( const WEATHER_OBJ_WORK* cp_wk );
static GFL_CLWK* WEATHER_OBJ_WK_GetClWk( const WEATHER_OBJ_WORK* cp_wk );


//-------------------------------------
///	�T�E���h�V�X�e��
//=====================================
static void WEATHER_SND_LOOP_Play( WEATHER_TASK_SND_LOOP* p_wk, int snd_no );
static void WEATHER_SND_LOOP_Stop( WEATHER_TASK_SND_LOOP* p_wk );


//-------------------------------------
///	�c�[��
//=====================================
static void TOOL_GetPerspectiveScreenSize( u16 perspway, fx32 dist, fx32 aspect, fx32* p_width, fx32* p_height );





//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�^�X�N�V�X�e��������
 *
 *	@param	p_clunit		�Z���A�N�^�[���j�b�g
 *	@param	cp_camera		�t�B�[���h�J����
 *	@param	heapID			�q�[�vID
 *	
 *	@return	�V�C�^�X�N���[�N
 */
//-----------------------------------------------------------------------------
WEATHER_TASK* WEATHER_TASK_Init( GFL_CLUNIT* p_clunit, const FIELD_CAMERA* cp_camera, u32 heapID )
{
	WEATHER_TASK* p_wk;

	p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(WEATHER_TASK) );

	// ��{���ݒ�
	p_wk->p_unit		= p_clunit;	
	p_wk->cp_camera		= cp_camera;	

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�^�X�N�V�X�e���j��
 *
 *	@param	p_wk		�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_Exit( WEATHER_TASK* p_wk )
{
	// ���s�V�C�̔j��
	WEATHER_TASK_ForceEnd( p_wk );

	// �j��
	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_Main( WEATHER_TASK* p_wk, u32 heapID )
{
	// ��������
	switch( p_wk->seq ){
	// �Ȃ�
	case WEATHER_TASK_SEQ_NONE:				
		break;
		
	// ��C�ɏ�����
	case WEATHER_TASK_SEQ_INIT_ALL:			
		WEATHER_TASK_WK_InitOam( p_wk, heapID );
		WEATHER_TASK_WK_InitBg( p_wk, heapID );
		WEATHER_TASK_WK_InitOther( p_wk, heapID );

		p_wk->seq = WEATHER_TASK_SEQ_CALL_INIT;
		break;

	// �����ǂݍ��� OAM
	case WEATHER_TASK_SEQ_INIT_DIV_OAM:		
		WEATHER_TASK_WK_InitOam( p_wk, heapID );

		p_wk->seq = WEATHER_TASK_SEQ_INIT_DIV_BG;
		break;

	// �����ǂݍ��� BG
	case WEATHER_TASK_SEQ_INIT_DIV_BG:		
		WEATHER_TASK_WK_InitBg( p_wk, heapID );

		p_wk->seq = WEATHER_TASK_SEQ_INIT_DIV_CREATE;
		break;

	// �����ǂݍ��� �\�z
	case WEATHER_TASK_SEQ_INIT_DIV_CREATE:	
		WEATHER_TASK_WK_InitOther( p_wk, heapID );

		p_wk->seq = WEATHER_TASK_SEQ_CALL_INIT;
		break;

	// �Ǘ��֐��@������	�@		�Ăяo��
	case WEATHER_TASK_SEQ_CALL_INIT:		
		if( WEATHER_TASK_WK_CallFunc( p_wk, p_wk->cp_data->p_f_init, heapID ) ){
			if( p_wk->fade ){
				p_wk->seq = WEATHER_TASK_SEQ_CALL_FADEIN;
			}else{
				p_wk->seq = WEATHER_TASK_SEQ_CALL_NOFADE;
			}
		}
		break;

	// �Ǘ��֐�  �t�F�[�h�C��	�Ăяo��
	case WEATHER_TASK_SEQ_CALL_FADEIN:		
		if( WEATHER_TASK_WK_CallFunc( p_wk, p_wk->cp_data->p_f_fadein, heapID ) ){
			p_wk->seq = WEATHER_TASK_SEQ_CALL_MAIN;
		}
		WEATHER_TASK_WK_MainObjList( p_wk );
		break;

	// �Ǘ��֐�	 �t�F�[�h�Ȃ�	�Ăяo��
	case WEATHER_TASK_SEQ_CALL_NOFADE:		
		if( WEATHER_TASK_WK_CallFunc( p_wk, p_wk->cp_data->p_f_nofade, heapID ) ){
			p_wk->seq = WEATHER_TASK_SEQ_CALL_MAIN;
		}
		break;

	// �Ǘ��֐��@���C��			�Ăяo��
	case WEATHER_TASK_SEQ_CALL_MAIN:		
		// *�O���I�ȃ��N�G�X�g�ŁA�I������
		WEATHER_TASK_WK_CallFunc( p_wk, p_wk->cp_data->p_f_main, heapID );
		WEATHER_TASK_WK_MainObjList( p_wk );
		break;

	// �Ǘ��֐��@�t�F�[�h�A�E�g �Ăяo��
	case WEATHER_TASK_SEQ_CALL_FADEOUT_INIT:
		if( WEATHER_TASK_WK_CallFunc( p_wk, p_wk->cp_data->p_f_init_fadeout, heapID ) ){
			p_wk->seq = WEATHER_TASK_SEQ_CALL_FADEOUT;
		}
		WEATHER_TASK_WK_MainObjList( p_wk );
		break;

	// �Ǘ��֐��@�t�F�[�h�A�E�g �Ăяo��
	case WEATHER_TASK_SEQ_CALL_FADEOUT:		
		if( WEATHER_TASK_WK_CallFunc( p_wk, p_wk->cp_data->p_f_fadeout, heapID ) ){
			p_wk->seq = WEATHER_TASK_SEQ_CALL_DEST;
		}
		WEATHER_TASK_WK_MainObjList( p_wk );
		break;

	// �Ǘ��֐��@�j��			�Ăяo��
	// �P��ŏI���K�v������
	case WEATHER_TASK_SEQ_CALL_DEST:
		{
			WEATHER_TASK_FUNC_RESULT result;
			result = WEATHER_TASK_WK_CallFunc( p_wk, p_wk->cp_data->p_f_dest, heapID );

			// �j�������͂P�V���N�ŏI���K�v������܂��B
			GF_ASSERT( result == WEATHER_TASK_FUNC_RESULT_FINISH );
			p_wk->seq = WEATHER_TASK_SEQ_DELETE_ALL;
		}
		break;

	// �S���̔j��
	//
	// *�j�������́AWEATHER_TASK_ForceEnd�ɂ�����܂��B
	case WEATHER_TASK_SEQ_DELETE_ALL:		
		WEATHER_TASK_WK_ExitOther( p_wk );
		WEATHER_TASK_WK_ExitBg( p_wk );
		WEATHER_TASK_WK_ExitOam( p_wk );
		WEATHER_TASK_WK_Clear( p_wk );

		p_wk->seq = WEATHER_TASK_SEQ_NONE;
		break;

	default:
		// ���肦�Ȃ�
		GF_ASSERT(0);
		break;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	�V�C����
 *
 *	@param	p_wk			���[�N
 *	@param	cp_data			�V�C�f�[�^
 *	@param	init_mode		���������[�h
 *	@param	fade			�t�F�[�h�̗L��	�@TRUE�F����@FALSE�F�Ȃ�
 *	@param	fog_cont		�t�H�O�Ǘ��̗L���@TRUE�F����@FALSE�F�Ȃ�
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_Start( WEATHER_TASK* p_wk, const WEATHER_TASK_DATA* cp_data, WEATEHR_TASK_INIT_MODE init_mode, BOOL fade, BOOL fog_cont, u32 heapID )
{
	GF_ASSERT( p_wk->seq == WEATHER_TASK_SEQ_NONE );
	
	// ���̐ݒ�
	p_wk->cp_data		= cp_data;
	p_wk->fog_cont		= fog_cont;
	p_wk->fade			= fade;
	p_wk->init_mode		= init_mode;


	// �������V�[�P���X�ݒ�
	if( p_wk->init_mode == WEATHER_TASK_INIT_NORMAL ){
		p_wk->seq = WEATHER_TASK_SEQ_INIT_ALL;
	}
	else if( p_wk->init_mode == WEATHER_TASK_INIT_DIV ){
		p_wk->seq = WEATHER_TASK_SEQ_INIT_DIV_OAM;
	}
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�j��
 *
 *	@param	p_wk			���[�N
 *	@param	fade			�t�F�[�h�̗L��	�@TRUE�F����@FALSE�F�Ȃ�
 *	@param	fog_cont		�t�H�O�Ǘ��̗L���@TRUE�F����@FALSE�F�Ȃ�
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_End( WEATHER_TASK* p_wk, BOOL fade, BOOL fog_cont )
{
	u32 info;

	info = WEATHER_TASK_WK_GetInfo( p_wk );

	// ���쒆���`�F�b�N
	if( p_wk->seq == WEATHER_TASK_SEQ_NONE ){
		return ;
	}
	
	// �j�����`�F�b�N
	if( (info == WEATHER_TASK_INFO_DEST) || 
		(info == WEATHER_TASK_INFO_FADEOUT) ){

		// �����j����
		GF_ASSERT( 0 );
		return ;
	}


	// �j����
	p_wk->fade		= fade;
	p_wk->fog_cont	= fog_cont;
	if( p_wk->fade ){
		p_wk->seq = WEATHER_TASK_SEQ_CALL_FADEOUT_INIT;
	}else{
		p_wk->seq = WEATHER_TASK_SEQ_CALL_DEST;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����I��	�i���̊֐����Ă񂾏u�ԂɑS�j�����܂��B�j
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_ForceEnd( WEATHER_TASK* p_wk )
{
	if( p_wk->seq != WEATHER_TASK_SEQ_NONE ){
		// callback dest
		if( p_wk->seq <= WEATHER_TASK_SEQ_CALL_DEST ){
			WEATHER_TASK_FUNC_RESULT result;
			// �������ɔj�������Ńq�[�v�m�ۂ͂��Ȃ����낤
			result = WEATHER_TASK_WK_CallFunc( p_wk, p_wk->cp_data->p_f_dest, 0 );

			// �j�������͂P�V���N�ŏI���K�v������܂��B
			GF_ASSERT( result == WEATHER_TASK_FUNC_RESULT_FINISH );
		}

		// delete all 
		if( p_wk->seq <= WEATHER_TASK_SEQ_DELETE_ALL ){
			WEATHER_TASK_WK_ExitOther( p_wk );
			WEATHER_TASK_WK_ExitBg( p_wk );
			WEATHER_TASK_WK_ExitOam( p_wk );
			WEATHER_TASK_WK_Clear( p_wk );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�C���̎擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	WEATHER_TASK_INFO_NONE,				// ���Ȃ�
 *	@retval	WEATHER_TASK_INFO_LOADING,			// �ǂݍ��ݒ�
 *	@retval	WEATHER_TASK_INFO_FADEIN,			// �t�F�[�h�C����
 *	@retval	WEATHER_TASK_INFO_MAIN,				// ���C�����쒆
 *	@retval	WEATHER_TASK_INFO_FADEOUT,			// �t�F�[�h�A�E�g��
 *	@retval	WEATHER_TASK_INFO_DEST,				// �j����
 */
//-----------------------------------------------------------------------------
WEATHER_TASK_INFO WEATHER_TASK_GetInfo( const WEATHER_TASK* cp_wk )
{
	return WEATHER_TASK_WK_GetInfo( cp_wk );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�C	���[�U���[�N�̎擾
 *
 *	@param	cp_wk		���[�N
 *
 *	@return	���[�U���[�N	WEATHER_TASK_DATA.work_byte == 0�̏ꍇNULL
 */
//-----------------------------------------------------------------------------
void* WEATHER_TASK_GetWorkData( const WEATHER_TASK* cp_wk )
{
	return  cp_wk->p_user_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���삵�Ă���I�u�W�F�N�g�̐����擾
 *
 *	@param	cp_wk		���[�N
 *
 *	@return	���삵�Ă���I�u�W�F�N�g�̐�
 */
//-----------------------------------------------------------------------------
u32 WEATHER_TASK_GetActiveObjNum( const WEATHER_TASK* cp_wk )
{
	return cp_wk->active_objnum;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�ǉ�����
 *
 *	@param	p_wk			���[�N
 *	@param	heapID			�q�[�vID
 *
 *	@retval	�ǉ��I�u�W�F
 *	@retval	NULL			�����ǉ��ł��Ȃ�
 */
//-----------------------------------------------------------------------------
WEATHER_OBJ_WORK* WEATHER_TASK_CreateObj( WEATHER_TASK* p_wk, u32 heapID )
{
	WEATHER_OBJ_WORK* p_obj;

	p_obj = WEATHER_TASK_WK_GetClearObj( p_wk );
	if( p_obj ){

		// ������
		WEATHER_OBJ_WK_Init( p_obj, p_wk, &p_wk->graphic, p_wk->p_unit, heapID );

		// �����N���X�g�ɐݒ�
		WEATHER_TASK_WK_PushObjList( p_wk, p_obj );
	}
	
	return p_obj;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�N�g�̔j������
 *
 *	@param	p_obj	���[�N
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_DeleteObj( WEATHER_OBJ_WORK* p_obj )
{
	if( WEATHER_OBJ_WK_IsUse( p_obj ) ){
		// ���X�g����j��
		WEATHER_TASK_WK_PopObjList( p_obj->p_parent, p_obj );

		// �j��
		WEATHER_OBJ_WK_Exit( p_obj );

		//  ���[�N�N���A
		WEATHER_OBJ_WK_Clear( p_obj );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�ǉ��t�F�[�h�@�t�F�[�h�C���J�n
 *
 *	@param	p_wk						�V�X�e�����[�N
 *	@param	objAddNum					�I�u�W�F�ǉ���
 *	@param	objAddTmgMax				�I�u�W�F�ǉ��^�C�~���O�}�b�N�X
 *	@param	obj_add_num_end				�I�u�W�F�ǉ����@�I���l
 *	@param	obj_add_tmg_end				�I�u�W�F�ǉ��^�C�~���O�@�I���l
 *	@param	obj_add_tmg_sum				�I�u�W�F�ǉ��^�C�~���O�@�����Z�l
 *	@param	obj_add_num_sum_tmg			�I�u�W�F�ǉ��^�C�~���O�@�����Z�^�C�~���O
 *	@param	obj_add_num_sum				�I�u�W�F�ǉ����@���Z�l
 *	@param	p_addfunc					�ǉ��֐�
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_ObjFade_Init( WEATHER_TASK* p_wk, s32 objAddNum, s32 objAddTmgMax, s32 obj_add_num_end, s32 obj_add_tmg_end, s32 obj_add_tmg_sum, s32 obj_add_num_sum_tmg, s32 obj_add_num_sum, WEATHER_TASK_OBJADD_FUNC* p_addfunc )
{
	WEATHER_OBJ_FADE_InitFade( &p_wk->obj_fade, p_wk, objAddTmgMax, objAddNum, obj_add_num_end, obj_add_tmg_end, obj_add_tmg_sum, obj_add_num_sum_tmg, obj_add_num_sum, p_addfunc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�ǉ��t�F�[�h	�t�F�[�h�A�E�g�J�n
 *
 *	@param	p_wk					���[�N
 *	@param	obj_add_num_end				�I�u�W�F�ǉ����@�I���l
 *	@param	obj_add_tmg_end				�I�u�W�F�ǉ��^�C�~���O�@�I���l
 *	@param	obj_add_tmg_sum				�I�u�W�F�ǉ��^�C�~���O�@�����Z�l
 *	@param	obj_add_num_sum				�I�u�W�F�ǉ����@���Z�l
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_ObjFade_SetOut( WEATHER_TASK* p_wk, s32 obj_add_num_end, s32 obj_add_tmg_end, s32 obj_add_tmg_sum, s32 obj_add_num_sum )
{
	WEATHER_OBJ_FADE_SetFadeOut( &p_wk->obj_fade, obj_add_num_end, obj_add_tmg_end, obj_add_tmg_sum, obj_add_num_sum );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�ǉ��t�F�[�h	���C��
 *
 *	@param	p_wk				���[�N
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�r��
 */
//-----------------------------------------------------------------------------
BOOL WEATHER_TASK_ObjFade_Main( WEATHER_TASK* p_wk, u32 heapID )
{
	return WEATHER_OBJ_FADE_Main( &p_wk->obj_fade, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�͂����A�I�u�W�F�̓����o�^�^�C�~���O�ŁA�������@�I�u�W�F��o�^���܂��B
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_ObjFade_NoFadeMain( WEATHER_TASK* p_wk, u32 heapID )
{
	WEATHER_TASK_OBJ_FADE_NoFadeMain( &p_wk->obj_fade, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�𕪎U������
 *
 *	@param	p_wk				���[�N
 *	@param	o_add_func			�ǉ��֐�
 *	@param	num					�o�^��
 *	@param	dust_div_num		���I�u�W�F�N�g���Ƃɓ��쐔��ύX���邩
 *	@param	dust_div_move		�e�I�u�W�F�N�g�̓��쐔���ǂ̂��炢������
 *
 * num���̃I�u�W�F�N�g��o�^���A0�Ԗڂ̃I�u�W�F�N�g����dust_div_move�񐔕�
 * �����I�u�W�F�N�g�𓮍삳����
 * 0�Ԗڂ̃I�u�W�F�N�g�@0
 * 1�Ԗڂ̃I�u�W�F�N�g	dust_div_move
 * 2�Ԗڂ̃I�u�W�F�N�g	dust_div_move * 2
 * ....
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_DustObj( WEATHER_TASK* p_wk, WEATHER_TASK_OBJADD_FUNC* p_add_func, int num, int dust_div_num, int dust_div_move, u32 heapID )
{
	int i, j;
	int move_num;
	WEATHER_OBJ_WORK* p_obj;
	WEATHER_OBJ_WORK* p_obj_next;
	
	// �I�u�W�F�N�g�o�^
	p_add_func( p_wk, num, heapID );


	// �I�u�W�F�N�g��i�߂�
	move_num = 0;
	p_obj = p_wk->p_objlist;
	p_obj_next = p_obj->p_next;
	for( i=0; i<num; i++ ){	
		
		for( j=0; j<move_num; j++ ){
			p_wk->cp_data->p_f_objmove( p_obj );

			// �j�����ꂽ��I���
			if( !WEATHER_OBJ_WK_IsUse(p_obj) ){
				break;
			}
		}

		// ����
		p_obj = p_obj_next;
		p_obj_next = p_obj->p_next;	// move_func���ŃI�u�W�F�N�g���j������邱�Ƃ����邽�߁A���̃I�u�W�F�N�g�|�C���^��ۑ����Ă���

		// ���쐔�ύX�v�Z
		if( i >= dust_div_num ){
			if( (i % dust_div_num) == 0 ){
				move_num += dust_div_move;
			}
		}

		
		// �o�^���s���Ă���Ƃ���num���̃I�u�W�F�N�g���o�^����Ă��Ȃ�
		if( (u32)p_obj == (u32)p_wk->p_objlist ){
			break;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�t�F�[�h������
 *
 *	@param	p_wk
 *	@param	fog_slope
 *	@param	fog_offs
 *	@param	color
 *	@param	timing 
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_FogFade_Init( WEATHER_TASK* p_wk, int fog_slope, int fog_offs, GXRgb color, int timing )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�t�F�[�h���C��
 *
 *	@param	p_wk		���[�N
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�r��
 */
//-----------------------------------------------------------------------------
BOOL WEATHER_TASK_FogFade_Main( WEATHER_TASK* p_wk )
{
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[�������@�v�Z���[�N�@������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_InitScrollDist( WEATHER_TASK* p_wk )
{
	VecFx32	now_mat;				// ���̍��W
	const GFL_G3D_CAMERA* cp_camera;// �J�������

	if( !p_wk->cp_camera ){
		return;
	}

	//  �J�������̎擾
	cp_camera = FIELD_CAMERA_GetCameraPtr( p_wk->cp_camera );
	GFL_G3D_CAMERA_GetTarget( cp_camera, &now_mat );

	p_wk->scroll.last_target = now_mat;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG��OAM�ȂǂQD�f�ނ̃X�N���[�����������߂�
 *
 *	@param	p_wk			���[�N
 *	@param	p_x				�������i�[��
 *	@param	p_y				�������i�[��
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_GetScrollDist( WEATHER_TASK* p_wk, int* p_x, int* p_y )
{
	VecFx32	now_mat;				// ���̍��W
	VecFx32	now_camera_pos;			// ���̃J�������W
	fx32 scl_x, scl_y;				// Scroll���W
	fx32 d_x, d_y;					// 2d�łP�̂R���̒l
	fx32 dist_x, dist_y;			// �ړ������l
	int	mark;						// ����
	const GFL_G3D_CAMERA* cp_camera;// �J�������
	fx32 camera_dist;			// �J��������

	if( !p_wk->cp_camera ){
		*p_x = 0;
		*p_y = 0;
		return;
	}

	//  �J�������̎擾
	cp_camera = FIELD_CAMERA_GetCameraPtr( p_wk->cp_camera );

	// �������瓮���������ɂO�D�T�����i�����_�̌덷�Œl���ς��Ȃ����߁j
	GFL_G3D_CAMERA_GetTarget( cp_camera, &now_mat );
	GFL_G3D_CAMERA_GetPos( cp_camera, &now_camera_pos );
	
	dist_x = (now_mat.x - p_wk->scroll.last_target.x);
	dist_y = (now_mat.z - p_wk->scroll.last_target.z);

	
	// ���̃J������2d��1�̂R���̒l�����߂�
	camera_dist = VEC_Distance( &now_mat, &now_camera_pos );
	TOOL_GetPerspectiveScreenSize( 
			defaultCameraFovy/2 *PERSPWAY_COEFFICIENT,
			camera_dist,
			defaultCameraAspect,
			&d_x, &d_y);
	d_x = FX_Div(d_x, 256*FX32_ONE);

	// ��ɐi��ł���Ƃ��A
	// ���ɐi��ł���Ƃ�
	if( dist_y <= 0 ){
		d_y = FX_Div(d_y, 0xbe8d0);
	}else{
		d_y = FX_Div(d_y, 0xbe811);
	}


	// �v���X�Ōv�Z����
	mark = FX32_ONE;
	if(dist_x < 0){		// �[�̎��́{�̒l�ɂ���
		mark = -FX32_ONE;
		dist_x = FX_Mul( dist_x, -FX32_ONE );
	}
	scl_x = FX_Div(dist_x, d_x);	// �X�N���[�����W���v�Z
	if( mark < 0 ){		// ���̕����ɖ߂�
		scl_x = FX_Mul( scl_x, mark );	// ���̕����ɖ߂�
	}

	mark = FX32_ONE;
	if(dist_y < 0){		// �[�̎��́{�̒l�ɂ���
		mark = -FX32_ONE;
		dist_y = FX_Mul( dist_y, -FX32_ONE );
	}	
	scl_y = FX_Div(dist_y, d_y);	// �X�N���[�����W�����߂�
	if( mark < 0 ){
		scl_y = FX_Mul( scl_y, mark );	// ���̕����ɖ߂�
	}
		
	// ���̍��W���擾
	if((scl_x + scl_y) != 0){
		p_wk->scroll.last_target = now_mat;
	}

	*p_x = scl_x>>FX32_SHIFT;
	*p_y = scl_y>>FX32_SHIFT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�N�g��S���X�N���[��������
 *
 *	@param	p_wk		���[�N
 *	@param	x			�X�N���[�������W
 *	@param	y			�X�N���[�������W
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_ScrollObj( WEATHER_TASK* p_wk, int x, int y )
{
	WEATHER_OBJ_WORK* p_objwk;
	GFL_CLWK* p_clwk;
	GFL_CLACTPOS pos;

	// �I�u�W�F�L���`�F�b�N
	if( !p_wk->p_objlist ){
		return ;
	}
	
	p_objwk = p_wk->p_objlist;
	do{
		p_clwk = WEATHER_OBJ_WK_GetClWk( p_objwk );
		GFL_CLACT_WK_GetWldPos( p_clwk, &pos );
		pos.x -= x;
		pos.y -= y;
		GFL_CLACT_WK_SetWldPos( p_clwk, &pos );

		p_objwk = p_objwk->p_next;
	}while( (u32)p_objwk != (u32)p_wk->p_objlist );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�ʂ̃X�N���[��
 *
 *	@param	p_wk		���[�N
 *	@param	x			�X�N���[�������W
 *	@param	y			�X�N���[�������W
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_ScrollBg( WEATHER_TASK* p_wk, int x, int y )
{
}



//----------------------------------------------------------------------------
/**
 *	@brief	���[�vSE�̍Đ�
 *
 *	@param	p_wk		���[�N
 *	@param	snd_no		SE�i���o�[
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_PlayLoopSnd( WEATHER_TASK* p_wk, int snd_no )
{
	WEATHER_SND_LOOP_Play( &p_wk->snd_loop, snd_no );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�vSE�̒�~
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void WEATHER_TASK_StopLoopSnd( WEATHER_TASK* p_wk )
{
	WEATHER_SND_LOOP_Stop( &p_wk->snd_loop );
}





//-----------------------------------------------------------------------------
/**
 *		WEATHER_OBJ_WORK	�Ǘ��֐�
 *
 *		�V�C�I�u�W�F�̎����Ă��郏�[�N�T�C�Y�͌Œ�ł��B
 *		WEATHER_OBJ_WORK_USE_WORKSIZE
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	���[�U�[���[�N�̎擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	���[�U���[�N
 */
//-----------------------------------------------------------------------------
void* WEATHER_OBJ_WORK_GetWork( const WEATHER_OBJ_WORK* cp_wk )
{
	return WEATHER_OBJ_WK_GetWork( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���[�N�̎擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�Z���A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
GFL_CLWK* WEATHER_OBJ_WORK_GetClWk( const WEATHER_OBJ_WORK* cp_wk )
{
	return WEATHER_OBJ_WK_GetClWk( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���⃏�[�N�̎擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�e�̓V�C���[�N
 */
//-----------------------------------------------------------------------------
const WEATHER_TASK* WEATHER_OBJ_WORK_GetParent( const WEATHER_OBJ_WORK* cp_wk )
{
	return cp_wk->p_parent;
}






//----------------------------------------------------------------------------
/**
 *	@brief	���`���샏�[�N	������
 *
 *	@param	p_wk			���[�N
 *	@param	s_x				�J�n�ʒu
 *	@param	e_x				�I���ʒu
 *	@param	count_max		�J�E���g�ő�l
 */
//-----------------------------------------------------------------------------
void WT_MOVE_WORK_Init( WT_MOVE_WORK* p_wk, int s_x, int e_x, u32 count_max )
{
	p_wk->x			= s_x;
	p_wk->s_x		= s_x;
	p_wk->d_x		= e_x - s_x;
	p_wk->count_max = count_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���`���상�C��
 *
 *	@param	p_wk		���[�N
 *	@param	count		�J�E���^
 *
 *	@retval	TRUE	���슮��
 *	@retval	FALSE	����r��
 */
//-----------------------------------------------------------------------------
BOOL WT_MOVE_WORK_Main( WT_MOVE_WORK* p_wk, u32 count )
{
	BOOL ret = FALSE;
	u32 calc_count;
	
	// �v�Z�J�E���g�l�����߂�
	if( (count >= p_wk->count_max) ){
		ret = TRUE;
		calc_count = p_wk->count_max;
	}else{
		calc_count = count;
	}


	p_wk->x = (p_wk->d_x * calc_count) / p_wk->count_max;

	return ret;
}





//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�^�X�N���[�N�̃N���A
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_Clear( WEATHER_TASK* p_wk )
{
	GFL_CLUNIT* p_unit;

	// �ꎞ�ޔ�
	p_unit = p_wk->p_unit;

	// �N���A
	GFL_STD_MemClear( p_wk, sizeof(WEATHER_TASK) );

	// ���A
	p_wk->p_unit = p_unit;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�^�X�N	�Ǘ��֐��̌Ăяo��
 *
 *	@param	p_wk		���[�N
 *	@param	p_func		�֐�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_TASK_WK_CallFunc( WEATHER_TASK* p_wk, WEATHER_TASK_FUNC* p_func, u32 heapID )
{
	if( p_func ){
		return p_func( p_wk, p_wk->fog_cont, heapID );
	}
	return WEATHER_TASK_FUNC_RESULT_FINISH;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�^�X�N	�C���t�H�̎擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�C���t�H���
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_INFO WEATHER_TASK_WK_GetInfo( const WEATHER_TASK* cp_wk )
{
	static const u8 sc_WEATHER_TASK_WK_SEQ_TO_INFO[WEATHER_TASK_SEQ_NUM]={
		WEATHER_TASK_INFO_NONE,		// WEATHER_TASK_SEQ_NONE,				// �Ȃ�
		WEATHER_TASK_INFO_LOADING,	// WEATHER_TASK_SEQ_INIT_ALL,			// ��C�ɏ�����
		WEATHER_TASK_INFO_LOADING,	// WEATHER_TASK_SEQ_INIT_DIV_OAM,		// �����ǂݍ��� OAM
		WEATHER_TASK_INFO_LOADING,	// WEATHER_TASK_SEQ_INIT_DIV_BG,		// �����ǂݍ��� BG
		WEATHER_TASK_INFO_LOADING,	// WEATHER_TASK_SEQ_INIT_DIV_CREATE,	// �����ǂݍ��� �\�z
		WEATHER_TASK_INFO_FADEIN,	// WEATHER_TASK_SEQ_CALL_INIT,			// �Ǘ��֐��@������	�@		�Ăяo��
		WEATHER_TASK_INFO_FADEIN,	// WEATHER_TASK_SEQ_CALL_FADEIN,		// �Ǘ��֐�  �t�F�[�h�C��	�Ăяo��
		WEATHER_TASK_INFO_FADEIN,	// WEATHER_TASK_SEQ_CALL_NOFADE,		// �Ǘ��֐�	 �t�F�[�h�Ȃ�	�Ăяo��
		WEATHER_TASK_INFO_MAIN,		// WEATHER_TASK_SEQ_CALL_MAIN,			// �Ǘ��֐��@���C��			�Ăяo��
		WEATHER_TASK_INFO_FADEOUT,	// WEATHER_TASK_SEQ_CALL_FADEOUT_INIT,		// �Ǘ��֐��@�t�F�[�h�A�E�g �Ăяo��
		WEATHER_TASK_INFO_FADEOUT,	// WEATHER_TASK_SEQ_CALL_FADEOUT,		// �Ǘ��֐��@�t�F�[�h�A�E�g �Ăяo��
		WEATHER_TASK_INFO_DEST,		// WEATHER_TASK_SEQ_CALL_DEST,			// �Ǘ��֐��@�j��			�Ăяo��
		WEATHER_TASK_INFO_DEST,		// WEATHER_TASK_SEQ_DELETE_ALL,		// �S���̔j��
	};
	
	GF_ASSERT( cp_wk->seq < NELEMS(sc_WEATHER_TASK_WK_SEQ_TO_INFO) );
	return sc_WEATHER_TASK_WK_SEQ_TO_INFO[ cp_wk->seq ];
}


//----------------------------------------------------------------------------
/**
 *	@brief	��̃I�u�W�F���擾
 *
 *	@param	p_wk	���[�N
 *
 *	@return	��̃I�u�W�F���[�N
 */
//-----------------------------------------------------------------------------
static WEATHER_OBJ_WORK* WEATHER_TASK_WK_GetClearObj( WEATHER_TASK* p_wk )
{
	int i;

	for( i=0; i<WEATHER_TASK_OBJBUFF_MAX; i++ ){

		if( WEATHER_OBJ_WK_IsUse( &p_wk->objbuff[i] ) == FALSE ){
			return &p_wk->objbuff[i];
		}
	}

	return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F���X�g�ւ̓o�^
 *
 *	@param	p_wk		���[�N
 *	@param	p_obj		�I�u�W�F���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_PushObjList( WEATHER_TASK* p_wk, WEATHER_OBJ_WORK* p_obj )
{
	WEATHER_OBJ_WORK* p_last;

	p_wk->active_objnum ++;
	
	// �擪�`�F�b�N
	if( !p_wk->p_objlist ){
		p_wk->p_objlist = p_obj;
		p_wk->p_objlist->p_next = p_obj;
		p_wk->p_objlist->p_last = p_obj;
		return ;
	}

	// �ŏI�ɒǉ�
	p_last = p_wk->p_objlist->p_last;

	// p_last�̎��ɓo�^
	p_obj->p_last			= p_last;
	p_obj->p_next			= p_last->p_next;
	p_last->p_next->p_last	= p_obj;
	p_last->p_next			= p_obj;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F���X�g����͂���
 *
 *	@param	p_wk		���[�N	
 *	@param	p_obj		�I�u�W�F���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_PopObjList( WEATHER_TASK* p_wk, WEATHER_OBJ_WORK* p_obj )
{

	p_wk->active_objnum --;
	
	// �������擪���`�F�b�N
	if( p_wk->p_objlist == p_obj ){
		// �����̎���������������I���
		if( p_obj->p_next == p_obj ){
			p_wk->p_objlist = NULL;
		}else{
			// �擪�������̎��̂�ɏ���
			p_wk->p_objlist = p_obj->p_next;
		}
	}
	// �͂���
	p_obj->p_next->p_last = p_obj->p_last;
	p_obj->p_last->p_next = p_obj->p_next;
	// �����̃��X�g�|�C���^���N���A
	p_obj->p_next = NULL;
	p_obj->p_last = NULL;

}


//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F���X�g�@���C������
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_MainObjList( WEATHER_TASK* p_wk )
{
	WEATHER_OBJ_WORK* p_objwk;
	WEATHER_OBJ_WORK* p_objwk_next;

	// �I�u�W�F�L���`�F�b�N
	if( !p_wk->p_objlist ){
		return ;
	}

	// �֐������邩�H
	if( !p_wk->cp_data->p_f_objmove ){
		return ;
	}
	
	p_objwk = p_wk->p_objlist;
	p_objwk_next = p_objwk->p_next;
	do{

		// ���C���֐����܂킷
		p_wk->cp_data->p_f_objmove( p_objwk );

		p_objwk = p_objwk_next;
		p_objwk_next = p_objwk->p_next;
	}while( ((u32)p_objwk != (u32)p_wk->p_objlist) && (p_wk->p_objlist != NULL) );
}


//----------------------------------------------------------------------------
/**
 *	@brief	OAM�̓ǂݍ���
 *
 *	@param	p_wk	���[�N
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_InitOam( WEATHER_TASK* p_wk, u32 heapID )
{
	// �O���t�B�b�N�ǂݍ���
	WEATHER_TASK_GRAPHIC_InitHandle( &p_wk->graphic, p_wk->cp_data, heapID );
	WEATHER_TASK_GRAPHIC_InitOam( &p_wk->graphic, p_wk->cp_data, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM���\�[�X�̔j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_ExitOam( WEATHER_TASK* p_wk )
{
	WEATHER_TASK_GRAPHIC_ExitOam( &p_wk->graphic );
	WEATHER_TASK_GRAPHIC_ExitHandle( &p_wk->graphic );
}
	
//----------------------------------------------------------------------------
/**
 *	@brief	BG���\�[�X�̓ǂݍ���
 *
 *	@param	p_wk		���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_InitBg( WEATHER_TASK* p_wk, u32 heapID )
{
	WEATHER_TASK_GRAPHIC_InitBg( &p_wk->graphic, p_wk->cp_data, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG���\�[�X�̔j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_ExitBg( WEATHER_TASK* p_wk )
{
	WEATHER_TASK_GRAPHIC_ExitBg( &p_wk->graphic );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���̑��̏�����
 *
 *	@param	p_wk		���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_InitOther( WEATHER_TASK* p_wk, u32 heapID )
{
	GF_ASSERT( !p_wk->p_user_work );
	p_wk->p_user_work = GFL_HEAP_AllocClearMemory( heapID, p_wk->cp_data->work_byte );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���̑��̔j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_WK_ExitOther( WEATHER_TASK* p_wk )
{
	GF_ASSERT( p_wk->p_user_work );
	GFL_HEAP_FreeMemory( p_wk->p_user_work );
	p_wk->p_user_work = NULL;
}




//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N���[�N���N���A����
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_GRAPHIC_Clear( WEATHER_TASK_GRAPHIC* p_wk )
{
	GF_ASSERT( !p_wk->oam_load );
	GF_ASSERT( !p_wk->p_handle );

	GFL_STD_MemClear( p_wk, sizeof(WEATHER_TASK_GRAPHIC) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�[�J�C�u�n���h��������
 *
 *	@param	p_wk			���[�N
 *	@param	cp_data			�V�C���
 *	@param	heapID			�q�[�vID
 */	
//-----------------------------------------------------------------------------
static void WEATHER_TASK_GRAPHIC_InitHandle( WEATHER_TASK_GRAPHIC* p_wk, const WEATHER_TASK_DATA* cp_data, u32 heapID )
{
	GF_ASSERT( !p_wk->p_handle );
	if( cp_data->use_oam || cp_data->use_bg ){
		p_wk->p_handle = GFL_ARC_OpenDataHandle( cp_data->arc_id, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�[�J�C�u�n���h���j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_GRAPHIC_ExitHandle( WEATHER_TASK_GRAPHIC* p_wk )
{
	if( p_wk->p_handle ){
		GFL_ARC_CloseDataHandle( p_wk->p_handle );
		p_wk->p_handle = NULL;
	}
}
	
//----------------------------------------------------------------------------
/**
 *	@brief	OAM���\�[�X�ǂݍ���
 *
 *	@param	p_wk		���[�N
 *	@param	cp_data		�V�C���
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_GRAPHIC_InitOam( WEATHER_TASK_GRAPHIC* p_wk, const WEATHER_TASK_DATA* cp_data, u32 heapID )
{
	GF_ASSERT( !p_wk->oam_load );

	if( cp_data->use_oam ){
		p_wk->oam_cgx_id = GFL_CLGRP_CGR_Register( p_wk->p_handle, cp_data->oam_cg, FALSE, 
				WEATHER_GRAPHIC_OAM_LOAD_DRAW_TYPE, heapID );
		p_wk->oam_pltt_id = GFL_CLGRP_PLTT_RegisterEx( p_wk->p_handle, cp_data->oam_pltt, 
				WEATHER_GRAPHIC_OAM_LOAD_DRAW_TYPE, WEATHER_TASK_GRAPHIC_OAM_PLTT_OFS, 0, 
				WEATHER_TASK_GRAPHIC_OAM_PLTT_NUM_MAX, heapID );
		p_wk->oam_cell_id = GFL_CLGRP_CELLANIM_Register( p_wk->p_handle, cp_data->oam_cell, cp_data->oam_cellanm, heapID );

		p_wk->oam_load = TRUE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM���\�[�X�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_GRAPHIC_ExitOam( WEATHER_TASK_GRAPHIC* p_wk )
{
	if( p_wk->oam_load ){

		GFL_CLGRP_CGR_Release( p_wk->oam_cgx_id );
		GFL_CLGRP_PLTT_Release( p_wk->oam_pltt_id );
		GFL_CLGRP_CELLANIM_Release( p_wk->oam_cell_id );

		p_wk->oam_load = FALSE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG���\�[�X�ǂݍ���
 *
 *	@param	p_wk		���[�N
 *	@param	cp_data		�f�[�^
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_GRAPHIC_InitBg( WEATHER_TASK_GRAPHIC* p_wk, const WEATHER_TASK_DATA* cp_data, u32 heapID )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG���\�[�X�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_GRAPHIC_ExitBg( WEATHER_TASK_GRAPHIC* p_wk )
{
}





//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�ǉ��t�F�[�h�V�X�e��	�������i�t�F�[�h�C���j
 *
 *	@param	p_wk						���[�N
 *	@param	p_sys						�V�X�e��
 *	@param	objAddTmgMax				�I�u�W�F�ǉ��^�C�~���O�}�b�N�X
 *	@param	objAddNum					�I�u�W�F�ǉ���
 *	@param	obj_add_num_end				�I�u�W�F�ǉ����@�I���l
 *	@param	obj_add_tmg_end				�I�u�W�F�ǉ��^�C�~���O�@�I���l
 *	@param	obj_add_tmg_sum				�I�u�W�F�ǉ��^�C�~���O�@�����Z�l
 *	@param	obj_add_num_sum_tmg			�I�u�W�F�ǉ��^�C�~���O�@�����Z�^�C�~���O
 *	@param	obj_add_num_sum				�I�u�W�F�ǉ����@���Z�l
 *	@param	p_addfunc					�ǉ��֐�
 */
//-----------------------------------------------------------------------------
static void WEATHER_OBJ_FADE_InitFade( WEATHER_TASK_OBJ_FADE* p_wk, WEATHER_TASK* p_sys, s32 objAddTmgMax, s32 objAddNum, s32 obj_add_num_end, s32 obj_add_tmg_end, s32 obj_add_tmg_sum, s32 obj_add_num_sum_tmg, s32 obj_add_num_sum, WEATHER_TASK_OBJADD_FUNC* p_addfunc )
{
	p_wk->p_parent			= p_sys;
	
	p_wk->obj_addnum			= objAddNum;
	p_wk->obj_addtmg			= 0;
	p_wk->obj_addtmg_max		= objAddTmgMax;
	p_wk->obj_addnum_sum		= 0;
	p_wk->obj_add_num_end		= obj_add_num_end;
	p_wk->obj_add_tmg_end		= obj_add_tmg_end;
	p_wk->obj_add_tmg_sum		= obj_add_tmg_sum;
	p_wk->obj_add_num_sum_tmg	= obj_add_num_sum_tmg;
	p_wk->obj_add_num_sum		= obj_add_num_sum;

	p_wk->p_addfunc				= p_addfunc;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�ǉ��t�F�[�h�V�X�e��	�t�F�[�h�A�E�g�ݒ�
 *
 *	@param	p_wk					���[�N
 *	@param	obj_add_num_end			�I�u�W�F�ǉ����@�I���l
 *	@param	obj_add_tmg_end			�I�u�W�F�ǉ��^�C�~���O�@�I���l
 *	@param	obj_add_tmg_sum			�I�u�W�F�ǉ��^�C�~���O�@�ω��l
 *	@param	obj_add_num_sum			�I�u�W�F�ǉ����@�ω��l
 */
//-----------------------------------------------------------------------------
static void WEATHER_OBJ_FADE_SetFadeOut( WEATHER_TASK_OBJ_FADE* p_wk, s32 obj_add_num_end, s32 obj_add_tmg_end, s32 obj_add_tmg_sum, s32 obj_add_num_sum )
{
	p_wk->obj_add_num_end	= obj_add_num_end;
	p_wk->obj_add_tmg_end	= obj_add_tmg_end;
	p_wk->obj_add_tmg_sum	= obj_add_tmg_sum;
	p_wk->obj_add_num_sum	= obj_add_num_sum;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�ǉ��t�F�[�h�V�X�e���@���C��
 *
 *	@param	p_wk				���[�N
 *
 *	@retval	TRUE	�t�F�[�h�C�� or �t�F�[�h�A�E�g�̊���
 *	@retval	FALSE	�r��
 */
//-----------------------------------------------------------------------------
static BOOL WEATHER_OBJ_FADE_Main( WEATHER_TASK_OBJ_FADE* p_wk, u32 heapID )
{
	int fade_flag;
	int ret = 0;
	

	// �t�F�[�h�I���`�F�b�N
	// �^�C�~���O-------------------------------------------
	// �����l���O�Ȃ�^�C�~���O�t�F�[�h�I��
	if(p_wk->obj_add_tmg_sum == 0){
		ret |= 1;
	}else{
		// �t�F�[�h�C���A�A�E�g�`�F�b�N
		if(p_wk->obj_add_tmg_sum > 0){
			fade_flag = 0;		// �ύX�l���v���X�Ȃ̂Ńt�F�[�h�C��
		}else{
			fade_flag = 1;		// �ύX�l���}�C�i�X�Ȃ̂Ńt�F�[�h�A�E�g
		}
		// �o�^�^�C�~���O�I���_�ɂ��Ă��Ȃ����`�F�b�N
		if(((p_wk->obj_addtmg_max >= p_wk->obj_add_tmg_end)&&(fade_flag == 0)) ||
			((p_wk->obj_addtmg_max <= p_wk->obj_add_tmg_end)&&(fade_flag == 1))){
			ret |= 1;

			p_wk->obj_addtmg_max = p_wk->obj_add_tmg_end;
		}
	}

	// �o�^��---------------------------------------------------
	// �����l���O�Ȃ�o�^���t�F�[�h�I��
	if(p_wk->obj_add_num_sum == 0){
		ret |= 2;
	}else{
		
		// �t�F�[�h�C���A�A�E�g�`�F�b�N
		if(p_wk->obj_add_num_sum > 0){
			fade_flag = 0;		// �ύX�l���v���X�Ȃ̂Ńt�F�[�h�C��
		}else{
			fade_flag = 1;		// �ύX�l���}�C�i�X�Ȃ̂Ńt�F�[�h�A�E�g
		}
		// �I�u�W�F�N�g�o�^�I���_�ɂ��Ă��Ȃ����`�F�b�N
		if(((p_wk->obj_addnum >= p_wk->obj_add_num_end)&&(fade_flag == 0)) ||
			((p_wk->obj_addnum <= p_wk->obj_add_num_end)&&(fade_flag == 1))){
		
			ret |= 2;
			p_wk->obj_addnum = p_wk->obj_add_num_end;
		}
	}
	

	// �^�C�~���O�J�E���^���ŏ��ɂȂ�܂ŃJ�E���g���
	(p_wk->obj_addtmg)--;
	if(p_wk->obj_addtmg <= 0){

		// �o�^
		p_wk->p_addfunc(p_wk->p_parent, p_wk->obj_addnum, heapID);

		p_wk->obj_addtmg = p_wk->obj_addtmg_max;			// �o�^�^�C�~���O�Z�b�g
		
		// �^�C�~���O-------------------------------------------
		if((ret & 1) == 0){
			p_wk->obj_addtmg_max += p_wk->obj_add_tmg_sum;	// �J�^�C�~���O�v�Z
		}
		
	
		// �o�^��---------------------------------------------------
		if((ret & 2) == 0){
			(p_wk->obj_addnum_sum)++;
			if(p_wk->obj_addnum_sum >= p_wk->obj_add_num_sum_tmg){
				p_wk->obj_addnum_sum = 0;
				p_wk->obj_addnum += p_wk->obj_add_num_sum;	// �o�^���A�b�h
			}
		}
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�͂����A�I�u�W�F�̓����o�^�^�C�~���O�ŁA�������@�I�u�W�F��o�^���܂��B
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_TASK_OBJ_FADE_NoFadeMain( WEATHER_TASK_OBJ_FADE* p_wk, u32 heapID )
{

	// �^�C�~���O�J�E���^���ŏ��ɂȂ�܂ŃJ�E���g���
	p_wk->obj_addtmg--;
	if(p_wk->obj_addtmg <= 0){

		// �o�^
		p_wk->p_addfunc(p_wk->p_parent, p_wk->obj_addnum, heapID);

		p_wk->obj_addtmg = p_wk->obj_addtmg_max;			// �o�^�^�C�~���O�Z�b�g
	}
}








//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F���[�N�N���A
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_OBJ_WK_Clear( WEATHER_OBJ_WORK* p_wk )
{
	GFL_STD_MemClear( p_wk, sizeof(WEATHER_OBJ_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F���[�N���g���Ă��邩�`�F�b�N
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	�g���Ă���
 *	@retval	FALSE	�g���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WEATHER_OBJ_WK_IsUse( const WEATHER_OBJ_WORK* cp_wk )
{
	if( cp_wk->p_parent == NULL ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F���[�N	����������
 *
 *	@param	p_wk			���[�N
 *	@param	p_parent		�e�̓V�C�^�X�N�V�X�e��
 *	@param	cp_graphic		�O���t�B�b�N���[�N
 *	@param	p_unit			�Z�����j�b�g
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WEATHER_OBJ_WK_Init( WEATHER_OBJ_WORK* p_wk, WEATHER_TASK* p_parent, const WEATHER_TASK_GRAPHIC* cp_graphic, GFL_CLUNIT* p_unit, u32 heapID )
{
	static const GFL_CLWK_DATA cladd = {
		0, 0,
		0, 
		WEATHER_OBJ_WORK_SOFT_PRI,
		WEATHER_OBJ_WORK_BG_PRI
	};

	// �O���t�B�b�N�͓ǂݍ��ݍς݂��H
	GF_ASSERT( cp_graphic->oam_load );
	
	
	// ����ݒ�
	p_wk->p_parent = p_parent; 

	// �A�N�^�[����
	p_wk->p_clwk = GFL_CLACT_WK_Create( p_unit, 
			cp_graphic->oam_cgx_id, cp_graphic->oam_pltt_id, 
			cp_graphic->oam_cell_id, &cladd, 
			WEATHER_OBJ_WORK_SERFACE_NO, heapID );

}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F���[�N	�j������
 *
 *	@param	p_wk			���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_OBJ_WK_Exit( WEATHER_OBJ_WORK* p_wk )
{
	if( p_wk->p_clwk ){
		// �A�N�^�[�j��
		GFL_CLACT_WK_Remove( p_wk->p_clwk );
		p_wk->p_clwk = NULL;
	}
	
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�̃��[�U���[�N���擾
 *
 *	@param	cp_wk	���[�N
 *	
 *	@return	���[�U���[�N
 */
//-----------------------------------------------------------------------------
static void* WEATHER_OBJ_WK_GetWork( const WEATHER_OBJ_WORK* cp_wk )
{
	return (void*)cp_wk->user_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�u�W�F�̃A�N�^�[���[�N�擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�A�N�^�[���[�N
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* WEATHER_OBJ_WK_GetClWk( const WEATHER_OBJ_WORK* cp_wk )
{
	return cp_wk->p_clwk;
}





//----------------------------------------------------------------------------
/**
 *	@brief	���[�v�T�E���h�Đ�
 *
 *	@param	p_wk		���[�N
 *	@param	snd_no		�T�E���h�萔
 */
//-----------------------------------------------------------------------------
static void WEATHER_SND_LOOP_Play( WEATHER_TASK_SND_LOOP* p_wk, int snd_no )
{
	if( p_wk->play ){
		WEATHER_SND_LOOP_Stop( p_wk );
	}

	p_wk->play		= TRUE;
	p_wk->snd_no	= snd_no;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�v�T�E���h��~
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WEATHER_SND_LOOP_Stop( WEATHER_TASK_SND_LOOP* p_wk )
{
	p_wk->play = FALSE;
}




//----------------------------------------------------------------------------
/**
 *	@brief	�ˉe�s��̏�񂩂�A�����ƕ���Ԃ�
 *
 *	@param	perspway			����p
 *	@param	dist				�^�[�Q�b�g�܂ł̋���
 *	@param	aspect				�A�X�y�N�g��
 *	@param	p_width				��ʂ̕��i�[��
 *	@param	p_height			��ʂ̍����i�[��
 */
//-----------------------------------------------------------------------------
static void TOOL_GetPerspectiveScreenSize( u16 perspway, fx32 dist, fx32 aspect, fx32* p_width, fx32* p_height )
{
	fx32 fovySin;
	fx32 fovyCos;
	fx32 fovyTan;

	fovySin = FX_SinIdx( perspway );
	fovyCos = FX_CosIdx( perspway );

	fovyTan = FX_Div( fovySin, fovyCos );
	
	// ���������߂�
	*p_height = FX_Mul(dist, fovyTan);				// (fovySin / fovyCos)*TargetDist
	*p_height = FX_Mul(*p_height, 2*FX32_ONE);		// �Q������Ɖ�ʂ̍����ɂȂ�
	
	// �������߂�(�A�X�y�N�g��4/3�ŌŒ�)
	*p_width  = FX_Mul(*p_height, aspect );

}



