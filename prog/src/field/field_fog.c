//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_fog.c
 *	@brief		�t�H�O�Ǘ��V�X�e��
 *	@author		tomoya takahashi
 *	@data		2009.03.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "field_fog.h"

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

// �t�F�[�h�`�F�b�N�A�T�[�g
#define FOG_FADE_ASSERT(x)		GF_ASSERT( FADE_WORK_IsFade( &(x)->fade ) == FALSE )


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�H�O�t�F�[�h�V�X�e��
//=====================================
typedef struct {
	u16 count;
	u16 count_max;
	
	u32	offset_start;
	s32	offset_dist;

	u16	r_start;
	s16	r_dist;
	u16	g_start;
	s16	g_dist;
	u16	b_start;
	s16	b_dist;

	u16 alpha_start;
	s16 alpha_dist;
} FADE_WORK;


//-------------------------------------
///	�V�X�e�����[�N
//=====================================
struct _FIELD_FOG_WORK {
	u8		flag;			// on/off (TRUE/FALSE)
	u8		blendmode;		// �t�H�O���[�h
	u8		slope;			// ������
	u8		alpha;			// �t�H�O�J���[�A���t�@
	u16		offset;			// �I�t�Z�b�g�f�v�X�l
	u16		rgb;			// �t�H�O�J���[

	u8		fog_tbl[FIELD_FOG_TBL_MAX];	// 32�i�K�̃t�H�O�e�[�u��

	// �t�H�O�t�F�[�h�V�X�e��
	FADE_WORK fade;


	BOOL	change;		// �f�[�^���ύX���ꂽ
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�t�F�[�h�V�X�e��
//=====================================
static BOOL FADE_WORK_IsFade( const FADE_WORK* cp_wk );
static void FADE_WORK_Init( FADE_WORK* p_wk, u16 offset_start, u16 offset_end, GXRgb color_start, GXRgb color_end, u8 alpha_start, u8 alpha_end, u16 count_max );
static void FADE_WORK_Main( FADE_WORK* p_wk );
static u16 FADE_WORK_GetOffset( const FADE_WORK* cp_wk );
static GXRgb FADE_WORK_GetColorRgb( const FADE_WORK* cp_wk );
static u8 FADE_WORK_GetColorA( const FADE_WORK* cp_wk );





//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�V�X�e���@�쐬
 *
 *	@param	heapID		�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
FIELD_FOG_WORK* FIELD_FOG_Create( u32 heapID )
{
	FIELD_FOG_WORK* p_wk;

	p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_FOG_WORK) );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�V�X�e���̔j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_Delete( FIELD_FOG_WORK* p_wk )
{
	GFL_HEAP_FreeMemory( p_wk );
}



//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�V�X�e���̃��C��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_Main( FIELD_FOG_WORK* p_wk )
{
	GF_ASSERT( p_wk );
	
	// �t�F�[�h�V�X�e�����C��
	if( FADE_WORK_IsFade( &p_wk->fade ) ){
		
		//  
		FADE_WORK_Main( &p_wk->fade );


		// �t�F�[�h����ݒ�
		p_wk->offset	= FADE_WORK_GetOffset( &p_wk->fade );
		p_wk->rgb		= FADE_WORK_GetColorRgb( &p_wk->fade );
		p_wk->alpha		= FADE_WORK_GetColorA( &p_wk->fade );
		p_wk->change	= TRUE;	// �f�[�^�ݒ�
	}

	// �f�[�^���ύX���ꂽ�̂ŁASDK�ɔ��f
	if( p_wk->change ){

		G3X_SetFog( p_wk->flag, p_wk->blendmode,
				p_wk->slope, p_wk->offset );
	
		G3X_SetFogColor( p_wk->rgb, p_wk->alpha );
	
		G3X_SetFogTable( (u32*)p_wk->fog_tbl );	

		p_wk->change = FALSE;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O	ON/OFF�ݒ�
 *
 *	@param	p_wk		���[�N
 *	@param	flag		�t���O
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_SetFlag( FIELD_FOG_WORK* p_wk, BOOL flag )
{
	GF_ASSERT( p_wk );
	p_wk->flag		= flag;
	p_wk->change	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�@�u�����h���[�h�ݒ�
 *
 *	@param	p_wk		���[�N
 *	@param	blendmode	�u�����h���[�h
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_SetBlendMode( FIELD_FOG_WORK* p_wk, FIELD_FOG_BLEND blendmode )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( blendmode < FIELD_FOG_BLEND_MAX );
	p_wk->blendmode = blendmode;
	p_wk->change	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�@�������ݒ�
 *
 *	@param	p_wk		���[�N
 *	@param	slope		������
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_SetSlope( FIELD_FOG_WORK* p_wk, FIELD_FOG_SLOPE slope )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( slope < FIELD_FOG_SLOPE_MAX );
	p_wk->slope		= slope;
	p_wk->change	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�I�t�Z�b�g�@�ݒ�
 *
 *	@param	p_wk			���[�N
 *	@param	depth_offset	�I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_SetOffset( FIELD_FOG_WORK* p_wk, u16 depth_offset )
{
	GF_ASSERT( p_wk );
	FOG_FADE_ASSERT(p_wk);
	p_wk->offset	= depth_offset;
	p_wk->change	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�J���[�@�ݒ�
 *
 *	@param	p_wk		���[�N
 *	@param	rgb			�J���[
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_SetColorRgb( FIELD_FOG_WORK* p_wk, GXRgb rgb )
{
	GF_ASSERT( p_wk );
	FOG_FADE_ASSERT(p_wk);
	p_wk->rgb		= rgb;
	p_wk->change	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�A���t�@	�ݒ�
 *
 *	@param	p_wk	���[�N
 *	@param	a		�A���t�@�l
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_SetColorA( FIELD_FOG_WORK* p_wk, u8 a )
{
	GF_ASSERT( p_wk );
	FOG_FADE_ASSERT(p_wk);
	p_wk->alpha		= a;
	p_wk->change	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�e�[�u��	�ݒ�
 *
 *	@param	p_wk		���[�N
 *	@param	index		�C���f�b�N�X
 *	@param	data		�f�[�^
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_SetTbl( FIELD_FOG_WORK* p_wk, u8 index, u8 data )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( index < FIELD_FOG_TBL_MAX );
	GF_ASSERT( data < 127 );
	p_wk->fog_tbl[index]	= data;
	p_wk->change			= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�e�[�u��	�o�b�t�@���Ɛݒ�
 *
 *	@param	p_wk		���[�N
 *	@param	cp_tbl		�e�[�u�����[�N	�iu8*32�j��
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_SetTblAll( FIELD_FOG_WORK* p_wk, const u8* cp_tbl )
{
	GF_ASSERT( p_wk );
	GF_ASSERT( cp_tbl );
	GFL_STD_MemCopy32( cp_tbl, p_wk->fog_tbl, sizeof(u8)*FIELD_FOG_TBL_MAX );
	p_wk->change	= TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O ON/OFF	�t���O�擾
 *
 *	@param	cp_wk	���[�N
 *	
 *	@retval	TRUE	ON
 *	@retval	FALSE	OFF
 */
//-----------------------------------------------------------------------------
BOOL FIELD_FOG_GetFlag( const FIELD_FOG_WORK* cp_wk )
{
	GF_ASSERT( cp_wk );
	return cp_wk->flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�@���[�h�@�擾
 *
 *	@param	cp_wk		���[�N
 *
 *	@return	�t�H�O���[�h
 */
//-----------------------------------------------------------------------------
FIELD_FOG_BLEND FIELD_FOG_GetBlendMode( const FIELD_FOG_WORK* cp_wk )
{
	GF_ASSERT( cp_wk );
	return cp_wk->blendmode;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O	�X���[�v�i�������j�擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	������
 */
//-----------------------------------------------------------------------------
FIELD_FOG_SLOPE FIELD_FOG_GetSlope( const FIELD_FOG_WORK* cp_wk )
{
	GF_ASSERT( cp_wk );
	return cp_wk->slope;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�@�I�t�Z�b�g	�擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�I�t�Z�b�g
 */
//-----------------------------------------------------------------------------
u16 FIELD_FOG_GetOffset( const FIELD_FOG_WORK* cp_wk )
{
	GF_ASSERT( cp_wk );
	return cp_wk->offset;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�@�J���[�@RGB�@�擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�J���[
 */
//-----------------------------------------------------------------------------
GXRgb FIELD_FOG_GetColorRgb( const FIELD_FOG_WORK* cp_wk )
{
	GF_ASSERT( cp_wk );
	return cp_wk->rgb;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�@�J���[�@�A���t�@�@�擾
 *
 *	@param	cp_wk	���[�N
 *	
 *	@return	�A���t�@�l
 */
//-----------------------------------------------------------------------------
u8 FIELD_FOG_GetColorA( const FIELD_FOG_WORK* cp_wk )
{
	GF_ASSERT( cp_wk );
	return cp_wk->alpha;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�e�[�u���@�l�擾
 *
 *	@param	cp_wk	���[�N
 *	@param	index	�C���f�b�N�X
 *
 *	@return	�l
 */
//-----------------------------------------------------------------------------
u8 FIELD_FOG_GetTbl( const FIELD_FOG_WORK* cp_wk, u8 index )
{
	GF_ASSERT( cp_wk );
	GF_ASSERT( index < FIELD_FOG_TBL_MAX );
	return cp_wk->fog_tbl[index];
}


//----------------------------------------------------------------------------
/**
 *	@brief	��{�I�ȃt�H�O�e�[�u�����쐬
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_TBL_SetUpDefault( FIELD_FOG_WORK* p_wk )
{
	int i;

	GF_ASSERT( p_wk );

	for( i=0; i<FIELD_FOG_TBL_MAX; i++ ){
		p_wk->fog_tbl[i] = i*4;
		p_wk->fog_tbl[i] += (i*4) / FIELD_FOG_TBL_MAX;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�t�F�[�h	������
 *
 *	@param	p_wk			���[�N
 *	@param	offset_end		�I�t�Z�b�g�I��
 *	@param	color_end		�F�I��
 *	@param	alpha_end		�A���t�@�I��
 *	@param	count_max		�t�F�[�h�V���N��
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_FADE_Init( FIELD_FOG_WORK* p_wk, u16 offset_end, GXRgb color_end, u8 alpha_end, u32 count_max )
{
	GF_ASSERT( p_wk );
	FADE_WORK_Init( &p_wk->fade, p_wk->offset, offset_end, p_wk->rgb, color_end, p_wk->alpha, alpha_end, count_max );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�t�F�[�h	�ڍ׏�����
 *
 *	@param	p_wk			���[�N
 *	@param	offset_start	�I�t�Z�b�g�J�n�l
 *	@param	offset_end		�I�t�Z�b�g�I���l
 *	@param	color_start		�F�J�n�l
 *	@param	color_end		�F�I���l
 *	@param	alpha_start		�A���t�@�J�n�l
 *	@param	alpha_end		�A���t�@�I���l
 *	@param	count_max		�t�F�[�h�Ɏg�p����V���N��
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_FADE_InitEx( FIELD_FOG_WORK* p_wk, u16 offset_start, u16 offset_end, GXRgb color_start, GXRgb color_end, u8 alpha_start, u8 alpha_end, u16 count_max  )
{
	GF_ASSERT( p_wk );
	FADE_WORK_Init( &p_wk->fade, offset_start, offset_end, color_start, color_end, alpha_start, alpha_end, count_max );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�@�t�F�[�h�����`�F�b�N
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	�t�F�[�h��
 *	@retval	FALSE	�t�F�[�h���ĂȂ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_FOG_FADE_IsFade( const FIELD_FOG_WORK* cp_wk )
{
	GF_ASSERT( cp_wk );

	return FADE_WORK_IsFade( &cp_wk->fade );
}







//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�����`�F�b�N
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	�t�F�[�h��
 *	@retval	FALSE	�t�F�[�h���ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL FADE_WORK_IsFade( const FADE_WORK* cp_wk )
{
	if( cp_wk->count <= cp_wk->count_max ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�t�F�[�h����	������
 *
 *	@param	p_wk			���[�N
 *	@param	offset_start	�I�t�Z�b�g�J�n�l
 *	@param	offset_end		�I�t�Z�b�g�I���l
 *	@param	color_start		�F�J�n�l
 *	@param	color_end		�F�I���l
 *	@param	alpha_start		�A���t�@�J�n�l
 *	@param	alpha_end		�A���t�@�I���l
 *	@param	count_max		�t�F�[�h�Ɏg�p����V���N��
 */
//-----------------------------------------------------------------------------
static void FADE_WORK_Init( FADE_WORK* p_wk, u16 offset_start, u16 offset_end, GXRgb color_start, GXRgb color_end, u8 alpha_start, u8 alpha_end, u16 count_max )
{
	p_wk->count			= 0;
	p_wk->count_max		= count_max;

	p_wk->offset_start	= offset_start;
	p_wk->offset_dist	= offset_end - offset_start;

	p_wk->r_start		= (color_start & GX_RGB_R_MASK )>> GX_RGB_R_SHIFT;
	p_wk->r_dist		= ((color_end & GX_RGB_R_MASK )>> GX_RGB_R_SHIFT) - p_wk->r_start;
	p_wk->g_start		= (color_start & GX_RGB_G_MASK )>> GX_RGB_G_SHIFT;
	p_wk->g_dist		= ((color_end & GX_RGB_G_MASK )>> GX_RGB_G_SHIFT) - p_wk->g_start;
	p_wk->b_start		= (color_start & GX_RGB_B_MASK )>> GX_RGB_B_SHIFT;
	p_wk->b_dist		= ((color_end & GX_RGB_B_MASK )>> GX_RGB_B_SHIFT) - p_wk->b_start;

	p_wk->alpha_start	= alpha_start;
	p_wk->alpha_dist	= alpha_end - alpha_start;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h����
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void FADE_WORK_Main( FADE_WORK* p_wk )
{
	if( p_wk->count < p_wk->count_max  ){
		p_wk->count++;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�t�Z�b�g�̒l���擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�I�t�Z�b�g�̒l
 */
//-----------------------------------------------------------------------------
static u16 FADE_WORK_GetOffset( const FADE_WORK* cp_wk )
{
	u16 ans;

	ans = (cp_wk->offset_dist * cp_wk->count) / cp_wk->count_max;
	ans += cp_wk->offset_start;

	return ans;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J���[�̒l���擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�J���[
 */
//-----------------------------------------------------------------------------
static GXRgb FADE_WORK_GetColorRgb( const FADE_WORK* cp_wk )
{
	u8 r, g, b;

	r = (cp_wk->r_dist * cp_wk->count) / cp_wk->count_max;
	r += cp_wk->r_start;
	g = (cp_wk->g_dist * cp_wk->count) / cp_wk->count_max;
	g += cp_wk->g_start;
	b = (cp_wk->b_dist * cp_wk->count) / cp_wk->count_max;
	b += cp_wk->b_start;

	return GX_RGB( r, g, b );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A���t�@�l���擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return	�A���t�@�l
 */
//-----------------------------------------------------------------------------
static u8 FADE_WORK_GetColorA( const FADE_WORK* cp_wk )
{
	u8 a;

	a = (cp_wk->alpha_dist * cp_wk->count) / cp_wk->count_max;
	a += cp_wk->alpha_start;

	return a;
}



