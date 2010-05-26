//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_3dbg.c
 *	@brief		�t�B�[���h�@�R�c�a�f��
 *	@author		tomoya takahshi
 *	@date		2009.03.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "field_3dbg.h"



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
#endif


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
///	�e�N�X�`�����
//=====================================
typedef struct {
	u8		texsiz_s;		// GXTexSizeS
	u8		texsiz_t;		// GXTexSizeT
	u8		repeat;			// GXTexRepeat
	u8		flip;			// GXTexFlip
	u8		texfmt;			// GXTexFmt
	u8		texpltt;		// GXTexPlttColor0

	u8		load;			// �ǂݍ��݂��s���Ă��邩
	u8		trans_req;	// �]�����N�G�X�g

	GFL_G3D_RES* p_textmp;	//  �e�N�X�`���f�[�^
} FIELD_3DBG_TEX;

//-------------------------------------
///	�`��ʏ��
//=====================================
typedef struct {
	fx32	size_half_x;
	fx32	size_half_y;
	fx32	z;
	fx32	base_scale;
	fx32	u0;
	fx32	u1;
	fx32	v0;
	fx32	v1;
	MtxFx33 tex_mtx;
} FIELD_3DPLANE;



//-------------------------------------
///	�t�B�[���h�R�c�a�f�V�X�e��
//=====================================
struct _FIELD_3DBG {
	u16		size_x;
	u16		size_y;
	fx32	camera_dist;
	BOOL	visible;
	s32		scroll_x;
	s32		scroll_y;
	fx32	scale_x;	
	fx32	scale_y;	
	u16		rotate;
	u8		polygon_id;
	u8		alpha;			// �A���t�@�ݒ�
	
	
	FIELD_3DBG_TEX	tex;
	FIELD_3DPLANE	plane;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�e�N�X�`�����
//=====================================
static void FIELD_3DBG_TEX_Init( FIELD_3DBG_TEX* p_wk, ARCHANDLE* p_handle, const FIELD_3DBG_WRITE_DATA* cp_data, u32 heapID );
static void FIELD_3DBG_TEX_Delete( FIELD_3DBG_TEX* p_wk );
static void FIELD_3DBG_TEX_TexTrans( FIELD_3DBG_TEX* p_wk );
static BOOL FIELD_3DBG_TEX_IsLoad( const FIELD_3DBG_TEX* cp_wk );


//-------------------------------------
///	�`����
//=====================================
static void FIELD_3DBG_PLANE_SetData( FIELD_3DPLANE* p_wk, const FIELD_3DBG* cp_sys, const FIELD_3DBG_TEX* cp_tex, const MtxFx44* cp_projection );
static void FIELD_3DBG_PLANE_Write( const FIELD_3DPLANE* cp_wk );


//-------------------------------------
///	�`��֌W
//=====================================
static void FIELD_3DBG_SetPolyAttr( const FIELD_3DBG* cp_sys );
static void FIELD_3DBG_SetTexParam( const FIELD_3DBG* cp_sys );

//----------------------------------------------------------------------------
/**
 *	@brief	�RDBG�`��V�X�e��	����
 *
 *	@param	cp_setup		��{���
 *	@param	heapID			�q�[�vID		
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
FIELD_3DBG* FIELD_3DBG_Create( const FIELD_3DBG_SETUP* cp_setup, u32 heapID )
{
	FIELD_3DBG* p_sys;

	// �������m��
	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_3DBG) );
  TOMOYA_Printf( "FIELD_3DBG_Memory 0x%x\n", sizeof(FIELD_3DBG) );

	// ��{���̐ݒ�
	p_sys->size_x		= cp_setup->size_x;
	p_sys->size_y		= cp_setup->size_y;
	p_sys->camera_dist	= cp_setup->camera_dist;
	p_sys->polygon_id	= cp_setup->polygon_id;

	// �`��ON
	p_sys->visible = TRUE;

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�RDBG�`��V�X�e��	�j��
 *
 *	@param	p_sys		�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_Delete( FIELD_3DBG* p_sys )
{
	GF_ASSERT( p_sys );

	// �e�N�X�`�����̔j��
	if( FIELD_3DBG_TEX_IsLoad( &p_sys->tex ) ){
		FIELD_3DBG_TEX_Delete( &p_sys->tex );
	}

	GFL_HEAP_FreeMemory( p_sys );
}
	 
//----------------------------------------------------------------------------
/**
 *	@brief	3DBG�`��V�X�e��	�`��
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	cp_camera		�J�������[�N
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_Write( FIELD_3DBG* p_sys )
{
	MtxFx44 projection;
	
	if( !p_sys ){
		return ;
	}
	
	if( (!p_sys->visible) ){
		return;
	}

	//  �e�N�X�`�����ǂݍ��܂�Ă��邩�H
	if( !FIELD_3DBG_TEX_IsLoad( &p_sys->tex ) ){
		return;
	}

	// �`�揈��
	// NNS�n�̃W�I���g���R�}���h�W�J
	NNS_G3dGeFlushBuffer();

	// �ˉe�s��ݒ�
	G3_MtxMode( GX_MTXMODE_PROJECTION );
	projection = *NNS_G3dGlbGetProjectionMtx();
	G3_LoadMtx44( &projection );
	
	// �J�����͒P�ʍs��
	G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
	G3_Identity();

	G3_PushMtx();
	{
	
		// �ˉe��񂩂�camera����̋����̂Ƃ��́AX�T�C�YY�T�C�Y�����߂�
		// texture��񂩂�UV�l�����߂�
		FIELD_3DBG_PLANE_SetData( &p_sys->plane, p_sys, &p_sys->tex, &projection );

		// �|���S���A�g���r���[�g
		FIELD_3DBG_SetPolyAttr( p_sys );
		
		// texture�p�����[�^
		FIELD_3DBG_SetTexParam( p_sys );

		// �`��
		FIELD_3DBG_PLANE_Write( &p_sys->plane );
	}
	G3_PopMtx(1);
}

//----------------------------------------------------------------------------
/**
 *	@brief	3DBG�`��V�X�e��	�e�N�X�`���W�J
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_VBlank( FIELD_3DBG* p_sys )
{
	if( p_sys ){
		// �e�N�X�`���̓W�J
		FIELD_3DBG_TEX_TexTrans( &p_sys->tex );
	}
}	



//----------------------------------------------------------------------------
/**
 *	@brief	�`����̃N���A
 *
 *	@param	p_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_ClearWriteData( FIELD_3DBG* p_sys )
{
	GF_ASSERT( p_sys );

	// �e�N�X�`���j��
	if( FIELD_3DBG_TEX_IsLoad( &p_sys->tex ) ){
		FIELD_3DBG_TEX_Delete( &p_sys->tex );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e�N�X�`���ݒ�
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	p_handle	�A�[�N�n���h��
 *	@param	cp_data		�f�[�^
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetWriteData( FIELD_3DBG* p_sys, ARCHANDLE* p_handle, const FIELD_3DBG_WRITE_DATA* cp_data, u32 heapID )
{
	GF_ASSERT( p_sys );
	GF_ASSERT( p_handle );
	GF_ASSERT( cp_data );
	
	// �j��
	if( FIELD_3DBG_TEX_IsLoad( &p_sys->tex ) ){
		FIELD_3DBG_TEX_Delete( &p_sys->tex );
	}

	// �ǂݍ���
	FIELD_3DBG_TEX_Init( &p_sys->tex, p_handle, cp_data, heapID );

	p_sys->alpha		= cp_data->alpha;			// �A���t�@�ݒ�

	p_sys->scale_x		= FX32_ONE;
	p_sys->scale_y		= FX32_ONE;

  p_sys->rotate = 0;
}



//----------------------------------------------------------------------------
/**
 *	@brief	�`��ON�EOFF
 *
 *	@param	p_sys	�V�X�e��
 *	@param	flag	�t���O
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetVisible( FIELD_3DBG* p_sys, BOOL flag )
{
	GF_ASSERT( p_sys );
	p_sys->visible = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��ON/OFF�̎擾
 *
 *	@param	cp_sys	�V�X�e�����[�N
 *
 *	@retval	TRUE	�\��
 *	@retval	FALSE	��\��
 */
//-----------------------------------------------------------------------------
BOOL FIELD_3DBG_GetVisible( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->visible;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�RDBG�@�X�N���[���T�C�Y�̐ݒ�
 *	
 *	@param	p_sys		�V�X�e��
 *	@param	size_x		�s�N�Z���P�ʂ̂a�f�O�ɕ`�悷�邳���̖ʃT�C�Y 
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetScreenSizeX( FIELD_3DBG* p_sys, u16 size_x )
{
	GF_ASSERT( p_sys );
	p_sys->size_x = size_x;
}

//----------------------------------------------------------------------------
/**
 *	@brief	3DBG	�X�N���[���T�C�Y�̎擾
 *
 *	@param	cp_sys		���[�N
 *
 *	@return	�X�N���[���T�C�Y
 */
//-----------------------------------------------------------------------------
u16 FIELD_3DBG_GetScreenSizeX( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->size_x;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�RDBG�@�X�N���[���T�C�Y�̐ݒ�
 *	
 *	@param	p_sys		�V�X�e��
 *	@param	size_y		�s�N�Z���P�ʂ̂a�f�O�ɕ`�悷�邳���̖ʃT�C�Y 
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetScreenSizeY( FIELD_3DBG* p_sys, u16 size_y )
{
	GF_ASSERT( p_sys );
	p_sys->size_y = size_y;
}

//----------------------------------------------------------------------------
/**
 *	@brief	3DBG	�X�N���[���T�C�Y�̎擾
 *
 *	@param	cp_sys		���[�N
 *
 *	@return	�X�N���[���T�C�Y
 */
//-----------------------------------------------------------------------------
u16 FIELD_3DBG_GetScreenSizeY( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->size_x;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�J��������̋�����ݒ�
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	dist		����
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetCameraDist( FIELD_3DBG* p_sys, fx32 dist )
{
	GF_ASSERT( p_sys );

	p_sys->camera_dist = dist;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J��������̋������擾
 *
 *	@param	cp_sys		�V�X�e�����[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
fx32 FIELD_3DBG_GetCameraDist( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->camera_dist;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|���S��ID�̐ݒ�
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	polygon_id	�|���S��ID
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetPolygonID( FIELD_3DBG* p_sys, u8 polygon_id )
{
	GF_ASSERT( p_sys );
	p_sys->polygon_id = polygon_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|���S��ID�̎擾
 *
 *	@param	cp_sys		�V�X�e�����[�N
 *
 *	@return	�|���S��ID
 */
//-----------------------------------------------------------------------------
u8 FIELD_3DBG_GetPolygonID( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->polygon_id;
}


//----------------------------------------------------------------------------
/**
 *	@brief	���s�[�g�ݒ�
 *
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	repeat		���s�[�g
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetRepeat( FIELD_3DBG* p_sys, GXTexRepeat repeat )
{
	GF_ASSERT( p_sys );
	p_sys->tex.repeat = repeat;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���s�[�g�擾
 *	
 *	@param	cp_sys		�V�X�e�����[�N
 *
 *	@return	���s�[�g
 */
//-----------------------------------------------------------------------------
GXTexRepeat FIELD_3DBG_GetRepeat( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->tex.repeat;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�t���b�v�ݒ�
 *	
 *	@param	p_sys	�V�X�e�����[�N
 *	@param	flip	�t���b�v
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetFlip( FIELD_3DBG* p_sys, GXTexFlip flip )
{
	GF_ASSERT( p_sys );
	p_sys->tex.flip = flip;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t���b�v�擾
 *
 *	@param	cp_sys	�V�X�e�����[�N
 *
 *	@return	�t���b�v
 */
//-----------------------------------------------------------------------------
GXTexFlip FIELD_3DBG_GetFlip( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->tex.flip;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A���t�@�ݒ�
 *
 *	@param	p_sys	�V�X�e�����[�N
 *	@param	alpha	�A���t�@ 
// *�g�p�ɂ͒��ӂ��K�v�ł��B
// �������|���S�����t�B�[���h�ɕ`�悳��Ă���ꏊ�ł�
// �������ɂ��Ȃ��ł��������B
// �d�Ȃ�������������Ȃ�܂��B
 *
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetAlpha( FIELD_3DBG* p_sys, u8 alpha )
{
	GF_ASSERT( p_sys );
	p_sys->alpha = alpha;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A���t�@�擾
 *
 *	@param	cp_sys	�V�X�e�����[�N
 *
 *	@return	�A���t�@
 */
//-----------------------------------------------------------------------------
u8 FIELD_3DBG_GetAlpha( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->alpha;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[�������W
 *
 *	@param	p_sys	�V�X�e�����[�N
 *	@param	x		�����W
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetScrollX( FIELD_3DBG* p_sys, s32 x )
{
	GF_ASSERT( p_sys );
	p_sys->scroll_x = x;
}
s32 FIELD_3DBG_GetScrollX( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->scroll_x;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[�������W
 *
 *	@param	p_sys
 *	@param	y 
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetScrollY( FIELD_3DBG* p_sys, s32 y )
{
	GF_ASSERT( p_sys );
	p_sys->scroll_y = y;
}
s32 FIELD_3DBG_GetScrollY( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->scroll_y;
}


//----------------------------------------------------------------------------
/**
 *	@brief	��]
 *
 *	@param	p_sys		�V�X�e��
 *	@param	rotate		��]�p�x
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetRotate( FIELD_3DBG* p_sys, u16 rotate )
{
	GF_ASSERT( p_sys );
	p_sys->rotate = rotate;
}
u16 FIELD_3DBG_GetRotate( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->rotate;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�X�P�[�������W
 *
 *	@param	p_sys	�V�X�e�����[�N
 *	@param	x		���X�P�[���l
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetScaleX( FIELD_3DBG* p_sys, fx32 x )
{
	GF_ASSERT( p_sys );
	p_sys->scale_x = x;
}
fx32 FIELD_3DBG_GetScaleX( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->scale_x;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�P�[��Y���W
 *
 *	@param	p_sys
 *	@param	y 
 */
//-----------------------------------------------------------------------------
void FIELD_3DBG_SetScaleY( FIELD_3DBG* p_sys, fx32 y )
{
	GF_ASSERT( p_sys );
	p_sys->scale_y = y;
}
fx32 FIELD_3DBG_GetScaleY( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->scale_y;
}





//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐��S
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	�e�N�X�`��������
 *
 *	@param	p_wk			���[�N
 *	@param	p_handle		�n���h��
 *	@param	cp_data			�f�[�^
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_3DBG_TEX_Init( FIELD_3DBG_TEX* p_wk, ARCHANDLE* p_handle, const FIELD_3DBG_WRITE_DATA* cp_data, u32 heapID )
{
	BOOL result;
	
	GF_ASSERT( p_wk );
	GF_ASSERT( p_handle );
	GF_ASSERT( cp_data );
	
	// ��{���ݒ�
	p_wk->texsiz_s	= cp_data->texsiz_s;		// GXTexSizeS
	p_wk->texsiz_t	= cp_data->texsiz_t;		// GXTexSizeT
	p_wk->repeat	= cp_data->repeat;			// GXTexRepeat
	p_wk->flip		= cp_data->flip;			// GXTexFlip
	p_wk->texfmt	= cp_data->texfmt;			// GXTexFmt
	p_wk->texpltt	= cp_data->texpltt;			// GXTexPlttColor0

	// texture�ǂݍ���
  p_wk->p_textmp = GFL_HEAP_AllocClearMemory( heapID, GFL_G3D_GetResourceHeaderSize() );
	GFL_G3D_CreateResourceAuto( p_wk->p_textmp, GFL_ARC_LoadDataAllocByHandle( p_handle, cp_data->nsbtex_id, heapID ) );

	result = GFL_G3D_AllocVramTexture( p_wk->p_textmp );
	GF_ASSERT( result );

	// �ǂݍ��݊���
	p_wk->load			= TRUE;
	p_wk->trans_req		= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e�N�X�`���j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_3DBG_TEX_Delete( FIELD_3DBG_TEX* p_wk )
{
	BOOL result;
	GF_ASSERT( p_wk->load );
	
	
	// �j��
	result = GFL_G3D_FreeVramTexture( p_wk->p_textmp );
	GF_ASSERT(result);
	GFL_G3D_DeleteResource( p_wk->p_textmp );
	p_wk->p_textmp = NULL;


	p_wk->load = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e�N�X�`��VRAM�]��
 *
 *	@param	cp_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_3DBG_TEX_TexTrans( FIELD_3DBG_TEX* p_wk )
{
	BOOL result;
	
	if( p_wk->trans_req ){

		result = GFL_G3D_TransOnlyTexture( p_wk->p_textmp );
		GF_ASSERT( result );
 
		p_wk->trans_req = FALSE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e�N�X�`����񂪓����Ă��邩
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	�����Ă���
 *	@retval	FALSE	�����ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL FIELD_3DBG_TEX_IsLoad( const FIELD_3DBG_TEX* cp_wk )
{
	GF_ASSERT( cp_wk );
	return cp_wk->load;
}



//----------------------------------------------------------------------------
/**
 *	@brief	�ʏ��̌v�Z
 *
 *	@param	p_wk			���[�N
 *	@param	cp_sys			�V�X�e�����[�N
 *	@param	cp_tex			texture�f�[�^
 *	@param	cp_projection	�ˉe�s��
 */
//-----------------------------------------------------------------------------
static void FIELD_3DBG_PLANE_SetData( FIELD_3DPLANE* p_wk, const FIELD_3DBG* cp_sys, const FIELD_3DBG_TEX* cp_tex, const MtxFx44* cp_projection )
{
	fx32 u_dist;
	fx32 v_dist;
	MtxFx33 mtx_work;

	// �ˉe�s�񂩂�Acamera����camera_dist��́A��ʍ����A�������߂�
	p_wk->size_half_y	= FX_Div( cp_sys->camera_dist, cp_projection->_11 );
	p_wk->size_half_x	= FX_Div( cp_sys->camera_dist, cp_projection->_00 );
	p_wk->z				= cp_sys->camera_dist;

	// �x�[�X�X�P�[���l�����߂�
	if( (p_wk->size_half_y > p_wk->size_half_x) ){
		if( p_wk->size_half_y > p_wk->z ){
			p_wk->base_scale = FX_Div( p_wk->size_half_y, 4<<FX32_SHIFT );
		}else{
			p_wk->base_scale = FX_Div( p_wk->z, 4<<FX32_SHIFT );
		}
	}else{
		if( p_wk->size_half_x > p_wk->z ){
			p_wk->base_scale = FX_Div( p_wk->size_half_x, 4<<FX32_SHIFT );
		}else{
			p_wk->base_scale = FX_Div( p_wk->z, 4<<FX32_SHIFT );
		}
	}
	p_wk->size_half_x	= FX_Div( p_wk->size_half_x, p_wk->base_scale );
	p_wk->size_half_y	= FX_Div( p_wk->size_half_y, p_wk->base_scale );
	p_wk->z				= FX_Div( p_wk->z, p_wk->base_scale );

	// UV�l�����߂�
	p_wk->u0 = cp_sys->scroll_x<<FX32_SHIFT;
	p_wk->v0 = cp_sys->scroll_y<<FX32_SHIFT;
	p_wk->u1 = p_wk->u0 + (cp_sys->size_x<<FX32_SHIFT);
	p_wk->v1 = p_wk->v0 + (cp_sys->size_y<<FX32_SHIFT);


	// texture�̕ϊ��s������߂�
	MTX_Identity33( &p_wk->tex_mtx );
	MTX_RotZ33( &p_wk->tex_mtx, FX_SinIdx(cp_sys->rotate), FX_CosIdx(cp_sys->rotate) );
	MTX_Scale33( &mtx_work, cp_sys->scale_x, cp_sys->scale_y, FX32_ONE );
	MTX_Concat33( &p_wk->tex_mtx, &mtx_work, &p_wk->tex_mtx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ʂ̕`��
 *		
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_3DBG_PLANE_Write( const FIELD_3DPLANE* cp_wk )
{
  // texture�ϊ��ݒ�
  G3_MtxMode( GX_MTXMODE_TEXTURE );
  G3_Identity();
  G3_MultMtx33( &cp_wk->tex_mtx );

  // ���ʂ̕`��
  G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
  G3_Scale( cp_wk->base_scale, cp_wk->base_scale, cp_wk->base_scale );

  G3_Begin( GX_BEGIN_QUADS );

  //���ʃ|���S���Ȃ̂Ŗ@���x�N�g����4���_�ŋ��p
  G3_TexCoord( cp_wk->u0, cp_wk->v0 );
  G3_Vtx( -cp_wk->size_half_x, cp_wk->size_half_y, -cp_wk->z );

  G3_TexCoord( cp_wk->u0, cp_wk->v1 );
  G3_Vtx( -cp_wk->size_half_x, -cp_wk->size_half_y, -cp_wk->z );

  G3_TexCoord( cp_wk->u1, cp_wk->v1 );
  G3_Vtx( cp_wk->size_half_x, -cp_wk->size_half_y, -cp_wk->z );

  G3_TexCoord( cp_wk->u1, cp_wk->v0 );
  G3_Vtx( cp_wk->size_half_x, cp_wk->size_half_y, -cp_wk->z );

  G3_End();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|���S���A�g���r���[�g�ݒ�
 *
 *	@param	cp_sys	�V�X�e��
 */
//-----------------------------------------------------------------------------
static void FIELD_3DBG_SetPolyAttr( const FIELD_3DBG* cp_sys )
{
	// �}�e���A���ݒ�
	G3_MaterialColorDiffAmb( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), TRUE );
	
	G3_PolygonAttr( 0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 
			cp_sys->polygon_id, cp_sys->alpha, GX_POLYGON_ATTR_MISC_FOG );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e�N�X�`���[�p�����[�^�ݒ�
 *
 *	@param	cp_sys	�V�X�e��
 */
//-----------------------------------------------------------------------------
static void FIELD_3DBG_SetTexParam( const FIELD_3DBG* cp_sys )
{
	GF_ASSERT( cp_sys->tex.load );
	G3_TexImageParam( cp_sys->tex.texfmt,
			GX_TEXGEN_TEXCOORD, 
			cp_sys->tex.texsiz_s, cp_sys->tex.texsiz_t,
			cp_sys->tex.repeat, cp_sys->tex.flip, 
			cp_sys->tex.texpltt,
			NNS_GfdGetTexKeyAddr(GFL_G3D_GetTextureDataVramKey(cp_sys->tex.p_textmp)) );

	G3_TexPlttBase( NNS_GfdGetPlttKeyAddr(GFL_G3D_GetTexturePlttVramKey(cp_sys->tex.p_textmp)),
			cp_sys->tex.texfmt );
}




