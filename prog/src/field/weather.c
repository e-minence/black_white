//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather.c
 *	@brief		�V�C�V�X�e��
 *	@author		tomoya takahashi
 *	@data		2009.03.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <gflib.h>

#include "weather_task.h"

#include "weather.h"

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
///	�V�C�ύX�^�C�v
//=====================================
enum{
	FIELD_WEATHER_CHANGETYPE_NORMAL,		// �ʏ�
	FIELD_WEATHER_CHANGETYPE_MULTI,			// �}���`�i�����Ȃ���j

	FIELD_WEATHER_CHANGETYPE_NUM,			// �V�X�e�����Ŏg�p
} ;

//-------------------------------------
///	�V�[�P���X
//=====================================
// NORMAL
enum {
	FIELD_WEATHER_NORMAL_SEQ_NONE,
	
	FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUT,
	FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUTWAIT,
	FIELD_WEATHER_NORMAL_SEQ_NEXT_OVERLAY_LOAD,
	FIELD_WEATHER_NORMAL_SEQ_NEXT_FADEIN,
} ;

// MULTI
enum {
	FIELD_WEATHER_MULTI_SEQ_NONE,
	
	FIELD_WEATHER_MULTI_SEQ_FADE_INOUT,
	FIELD_WEATHER_MULTI_SEQ_FADE_OUTWAIT,
} ;



//-------------------------------------
///	�V�C���[�N�@�ێ���
//=====================================
enum{
	FIELD_WEATHER_WORK_NOW,
	FIELD_WEATHER_WORK_NEXT,

	FIELD_WEATHER_WORK_NUM,
};


//-------------------------------------
///	�Z�����j�b�g���[�N��
//=====================================
#define FIELD_WEATHER_CLUNIT_WORK_MAX	( 96 )


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�C�V�X�e�����[�N
//=====================================
struct _FIELD_WEATHER{

	// �؂�ւ��V�[�P���X
	u16		change_type;
	u16		seq;
	u16		now_weather;
	u16		next_weather;

	// �\��
	u16		booking_weather;

	// �Z���A�N�^�[���j�b�g
	GFL_CLUNIT*	p_unit;

	// �V�C����V�X�e��
	WEATHER_TASK* p_task[ FIELD_WEATHER_WORK_NUM ];
	
	// ���̃I�[�o���CID
	FSOverlayID	now_overlayID;
};



//-----------------------------------------------------------------------------
/**
 *					�V�C���
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�C���^
//=====================================
typedef struct {
	const WEATHER_TASK_DATA*	cp_data;			// �V�C���
	FSOverlayID					overlay_id;			// �I�[�o�[���CID
} FIELD_WEATHER_DATA;
static const FIELD_WEATHER_DATA sc_FIELD_WEATHER_DATA[] = {
	{
		NULL,
		NO_OVERLAY_ID
	},
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�X�e���Ǘ�
//=====================================
static void FIELD_WEATHER_OVERLAY_Load( FIELD_WEATHER* p_sys, FSOverlayID overlay );
static void FIELD_WEATHER_OVERLAY_UnLoad( FIELD_WEATHER* p_sys );

//-------------------------------------
///	�V�C�؂�ւ�����
//=====================================
static void FIELD_WEATHER_CHANGE_Normal( FIELD_WEATHER* p_sys, u32 heapID );
static void FIELD_WEATHER_CHANGE_Multi( FIELD_WEATHER* p_sys, u32 heapID );




//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�V�C�V�X�e��	������
 *
 *	@param	cp_camera	�t�B�[���h�J����
 *	@param	heapID		�q�[�vID
 *
 *	@return	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
FIELD_WEATHER* FIELD_WEATHER_Init( const FIELD_CAMERA* cp_camera, u32 heapID )
{
	FIELD_WEATHER* p_sys;
	int i;

	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WEATHER) );

	// �����o������
	p_sys->change_type		= FIELD_WEATHER_CHANGETYPE_NORMAL;
	p_sys->seq				= 0;
	p_sys->now_weather		= FIELD_WEATHER_NO_NONE;
	p_sys->next_weather		= FIELD_WEATHER_NO_NONE;
	p_sys->booking_weather	= FIELD_WEATHER_NO_NONE;
	p_sys->now_overlayID	= GFL_OVERLAY_BLANK_ID;

	// �Z�����j�b�g������
	p_sys->p_unit = GFL_CLACT_UNIT_Create( FIELD_WEATHER_CLUNIT_WORK_MAX, FIELD_WEATHER_CLUNIT_PRI, heapID );
	

	for( i=0; i<FIELD_WEATHER_WORK_NUM; i++ ){
		p_sys->p_task[ i ] = WEATHER_TASK_Init( p_sys->p_unit, cp_camera, heapID );
	}

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�V�C�j��
 *
 *	@param	p_sys 
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_Exit( FIELD_WEATHER* p_sys )
{
	int i;

	for( i=0; i<FIELD_WEATHER_WORK_NUM; i++ ){
		WEATHER_TASK_Exit( p_sys->p_task[i] );
	}

	// �I�[�o�[���C�̃A�����[�h
	FIELD_WEATHER_OVERLAY_UnLoad( p_sys );

	// �Z�����j�b�g�j��
	GFL_CLACT_UNIT_Delete( p_sys->p_unit );

	GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_Main( FIELD_WEATHER* p_sys, u32 heapID )
{
	int i;
	static void (*pFunc[FIELD_WEATHER_CHANGETYPE_NUM])( FIELD_WEATHER* p_sys, u32 heapID ) ={
		FIELD_WEATHER_CHANGE_Normal,
		FIELD_WEATHER_CHANGE_Multi,
	};

	// �؂�ւ�����
	GF_ASSERT( NELEMS( pFunc ) > p_sys->change_type );
	pFunc[ p_sys->change_type ]( p_sys, heapID );

	// �\��`�F�b�N
	if( p_sys->seq == 0 ){
		if( p_sys->booking_weather != FIELD_WEATHER_NO_NONE ){
			FIELD_WEATHER_Change( p_sys, p_sys->booking_weather );
			p_sys->booking_weather = FIELD_WEATHER_NO_NONE;
		}
	}

	// �^�X�N���C��
	for( i=0; i<FIELD_WEATHER_WORK_NUM; i++ ){
		WEATHER_TASK_Main( p_sys->p_task[i], heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�t�F�[�h�C���Ȃ��ݒ�
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	weather_no		�V�C�i���o�[
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_Set( FIELD_WEATHER* p_sys, u32 weather_no, u32 heapID )
{
	int i;
	
	// �V�C�i���o�[�`�F�b�N
	GF_ASSERT( weather_no < WEATHER_NO_NUM );
	GF_ASSERT( NELEMS(sc_FIELD_WEATHER_DATA) > weather_no );

	// �ύX����Ӗ������邩�H
	if( weather_no != p_sys->now_weather ){

		// �S����j��
		for( i=0; i<FIELD_WEATHER_WORK_NUM; i++ ){
			WEATHER_TASK_ForceEnd( p_sys->p_task[i] );
		}

		// �I�[�o�[���C�j��
		FIELD_WEATHER_OVERLAY_UnLoad( p_sys );
		
		// �V�[�P���X��������
		p_sys->seq = 0;

		// �I�[�o�[���C�ǂݍ���
		FIELD_WEATHER_OVERLAY_Load( p_sys, sc_FIELD_WEATHER_DATA[ weather_no ].overlay_id );

		// �t�F�[�h�Ȃ�������
		WEATHER_TASK_Start( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ], 
				sc_FIELD_WEATHER_DATA[ weather_no ].cp_data, WEATHER_TASK_INIT_NORMAL, FALSE, TRUE, heapID );

		p_sys->now_weather	= weather_no;
		p_sys->next_weather	= FIELD_WEATHER_NO_NONE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C������@�V�C�ύX
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	weather_no		�V�C�i���o�[
 */
//-----------------------------------------------------------------------------
void FIELD_WEATHER_Change( FIELD_WEATHER* p_sys, u32 weather_no )
{

	// �V�C�i���o�[�`�F�b�N
	GF_ASSERT( weather_no < WEATHER_NO_NUM );
	GF_ASSERT( NELEMS(sc_FIELD_WEATHER_DATA) > weather_no );

	// ���s���̃��N�G�X�g�����邩�`�F�b�N
	if( p_sys->seq != 0 ){
		p_sys->booking_weather = weather_no;
		return ;
	}

	// �V�C���ꏏ�Ȃ牽�����Ȃ�
	if( p_sys->now_weather == weather_no ){
		return ;
	}

	// ���̓V�C��ݒ�
	p_sys->next_weather = weather_no;
	
	// �t�F�[�h�A�E�g�{�t�F�[�h�C��
	p_sys->change_type	= FIELD_WEATHER_CHANGETYPE_NORMAL;
	p_sys->seq			= FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�i���o�[�̎擾
 *
 *	@param	cp_sys	�V�X�e�����[�N
 *	
 *	@return	���̓V�C�i���o�[���擾
 */
//-----------------------------------------------------------------------------
u32 FIELD_WEATHER_GetWeatherNo( const FIELD_WEATHER* cp_sys )
{
	return cp_sys->now_weather;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���̓V�C�i���o�[�̎擾
 *
 *	@param	cp_sys	�V�X�e�����[�N
 *
 *	@return	���̓V�C�i���o�[
 */
//-----------------------------------------------------------------------------
u32 FIELD_WEATHER_GetNextWeatherNo( const FIELD_WEATHER* cp_sys )
{
	return cp_sys->next_weather;
}






//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�o�[���C�ǂݍ���
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	overlay		�I�[�o�[���C
 */
//-----------------------------------------------------------------------------
static void FIELD_WEATHER_OVERLAY_Load( FIELD_WEATHER* p_sys, FSOverlayID overlay )
{
	GF_ASSERT( p_sys->now_overlayID == GFL_OVERLAY_BLANK_ID );
	p_sys->now_overlayID = overlay;
	GFL_OVERLAY_Load( p_sys->now_overlayID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�[�o�[���C�j��
 *
 *	@param	p_sys		�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_WEATHER_OVERLAY_UnLoad( FIELD_WEATHER* p_sys )
{
	if( p_sys->now_overlayID != GFL_OVERLAY_BLANK_ID ){
		GFL_OVERLAY_Unload( p_sys->now_overlayID );
		p_sys->now_overlayID = GFL_OVERLAY_BLANK_ID;
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�ύX�@����	�ʏ�
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_WEATHER_CHANGE_Normal( FIELD_WEATHER* p_sys, u32 heapID )
{
	switch( p_sys->seq ){
	case FIELD_WEATHER_NORMAL_SEQ_NONE:
		break;
	
		
	case FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUT:
		WEATHER_TASK_End( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ], 
				TRUE, TRUE );
		p_sys->seq = FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUTWAIT;
		break;

	case FIELD_WEATHER_NORMAL_SEQ_NOW_FADEOUTWAIT:
		if( WEATHER_TASK_GetInfo( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ] ) == WEATHER_TASK_INFO_NONE ){
			//  �I�[�o�[���C�̃A�����[�h
			FIELD_WEATHER_OVERLAY_UnLoad( p_sys );

			p_sys->seq = FIELD_WEATHER_NORMAL_SEQ_NEXT_OVERLAY_LOAD;
		}
		break;

	case FIELD_WEATHER_NORMAL_SEQ_NEXT_OVERLAY_LOAD:
		//  �I�[�o�[���C�̃A�����[�h
		FIELD_WEATHER_OVERLAY_Load( p_sys, sc_FIELD_WEATHER_DATA[ p_sys->next_weather ].overlay_id );
		p_sys->seq = FIELD_WEATHER_NORMAL_SEQ_NEXT_FADEIN;
		break;

	case FIELD_WEATHER_NORMAL_SEQ_NEXT_FADEIN:
		WEATHER_TASK_Start( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ], 
				sc_FIELD_WEATHER_DATA[ p_sys->next_weather ].cp_data, 
				WEATHER_TASK_INIT_DIV, TRUE, TRUE, heapID );

		// ���ւ̏�������
		p_sys->now_weather	= p_sys->next_weather;
		p_sys->next_weather	= FIELD_WEATHER_NO_NONE;

		p_sys->seq = FIELD_WEATHER_NORMAL_SEQ_NONE;
		break;

	default:
		GF_ASSERT(0);
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�C�}���`�ύX����
 *	
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_WEATHER_CHANGE_Multi( FIELD_WEATHER* p_sys, u32 heapID )
{
	switch( p_sys->seq ){
	case FIELD_WEATHER_MULTI_SEQ_NONE:
		break;
	
		
	case FIELD_WEATHER_MULTI_SEQ_FADE_INOUT:
		WEATHER_TASK_Start( p_sys->p_task[ FIELD_WEATHER_WORK_NEXT ], 
				sc_FIELD_WEATHER_DATA[ p_sys->next_weather ].cp_data, 
				WEATHER_TASK_INIT_DIV, TRUE, FALSE, heapID );

		WEATHER_TASK_End( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ], 
				TRUE, FALSE );

		p_sys->seq = FIELD_WEATHER_MULTI_SEQ_FADE_OUTWAIT;
		break;
	
	case FIELD_WEATHER_MULTI_SEQ_FADE_OUTWAIT:
		if( WEATHER_TASK_GetInfo( p_sys->p_task[ FIELD_WEATHER_WORK_NOW ] ) == WEATHER_TASK_INFO_NONE ){

			// ��������
			{
				WEATHER_TASK* p_tmp;

				p_tmp = p_sys->p_task[ FIELD_WEATHER_WORK_NOW ];
				p_sys->p_task[ FIELD_WEATHER_WORK_NOW ]		= p_sys->p_task[ FIELD_WEATHER_WORK_NEXT ];
				p_sys->p_task[ FIELD_WEATHER_WORK_NEXT ]	= p_tmp;
				p_sys->now_weather	= p_sys->next_weather;
				p_sys->next_weather = FIELD_WEATHER_NO_NONE;
			}
			
			p_sys->seq = FIELD_WEATHER_MULTI_SEQ_NONE;
		}
		break;
	
	default:
		GF_ASSERT(0);
		break;
	}
}

