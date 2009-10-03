//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_fog.c
 *	@brief		�t�H�O�Ǘ��V�X�e��
 *	@author		tomoya takahashi
 *	@date		2009.03.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "field_fog.h"

#include "arc_def.h"
#include "message.naix"

#include "print/wordset.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "msg/msg_d_tomoya.h"

#include "font/font.naix"

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
	
	s32	offset_start;
	s32	offset_dist;

	u32	slope_start;
	s32	slope_dist;

} FADE_WORK;


//-------------------------------------
///	�V�X�e�����[�N
//=====================================
struct _FIELD_FOG_WORK {
	u8		flag;			// on/off (TRUE/FALSE)
	u8		blendmode;		// �t�H�O���[�h
	u8		slope;			// ������
	u8		alpha;			// �t�H�O�J���[�A���t�@
	s32		offset;			// �I�t�Z�b�g�f�v�X�l
	u16		rgb;			// �t�H�O�J���[
  u16   pading;

	u8		fog_tbl[FIELD_FOG_TBL_MAX];	// 32�i�K�̃t�H�O�e�[�u��

	// �t�H�O�t�F�[�h�V�X�e��
	FADE_WORK fade;


	BOOL	change;		// �f�[�^���ύX���ꂽ

#ifdef PM_DEBUG
	WORDSET*		p_debug_wordset;
	GFL_FONT*		p_debug_font;
	GFL_MSGDATA*	p_debug_msgdata;
	STRBUF*			p_debug_strbuff;
	STRBUF*			p_debug_strbuff_tmp;
#endif	// PM_DEBUG
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
static void FADE_WORK_Init( FADE_WORK* p_wk, s32 offset_start, s32 offset_end, u16 slope_start, u16 slope_end, u16 count_max );
static void FADE_WORK_Main( FADE_WORK* p_wk );
static s32 FADE_WORK_GetOffset( const FADE_WORK* cp_wk );
static u16 FADE_WORK_GetSlope( const FADE_WORK* cp_wk );






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

	p_wk->blendmode = FIELD_FOG_BLEND_COLOR_ALPHA;
	p_wk->alpha		= 31;

	FIELD_FOG_TBL_SetUpDefault( p_wk );

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
		p_wk->slope		= FADE_WORK_GetSlope( &p_wk->fade );
		p_wk->change	= TRUE;	// �f�[�^�ݒ�
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^���f�֐�
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_Reflect( FIELD_FOG_WORK* p_wk )
{
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
	FOG_FADE_ASSERT(p_wk);
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
void FIELD_FOG_SetOffset( FIELD_FOG_WORK* p_wk, s32 depth_offset )
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
 32*/
//-----------------------------------------------------------------------------
void FIELD_FOG_SetColorRgb( FIELD_FOG_WORK* p_wk, GXRgb rgb )
{
	GF_ASSERT( p_wk );
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
s32 FIELD_FOG_GetOffset( const FIELD_FOG_WORK* cp_wk )
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
 *	@param	slope_end		�������I��
 *	@param	count_max		�t�F�[�h�V���N��
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_FADE_Init( FIELD_FOG_WORK* p_wk, s32 offset_end, FIELD_FOG_SLOPE slope_end, u32 count_max )
{
	GF_ASSERT( p_wk );
	FADE_WORK_Init( &p_wk->fade, p_wk->offset, offset_end, p_wk->slope, slope_end, count_max );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�H�O�t�F�[�h	�ڍ׏�����
 *
 *	@param	p_wk			���[�N
 *	@param	offset_start	�I�t�Z�b�g�J�n�l
 *	@param	offset_end		�I�t�Z�b�g�I���l
 *	@param	slope_start		�X���[�v�J�n�l
 *	@param	slope_end		�X���[�v�I���l
 *	@param	count_max		�t�F�[�h�Ɏg�p����V���N��
 */
//-----------------------------------------------------------------------------
void FIELD_FOG_FADE_InitEx( FIELD_FOG_WORK* p_wk, s32 offset_start, s32 offset_end, FIELD_FOG_SLOPE slope_start, FIELD_FOG_SLOPE slope_end, u16 count_max  )
{
	GF_ASSERT( p_wk );
	FADE_WORK_Init( &p_wk->fade, offset_start, offset_end, slope_start, slope_end, count_max );
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
	if( cp_wk->count >= cp_wk->count_max ){
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
 *	@param	slope_start		�������J�n�l
 *	@param	slope_end		�������I���l
 *	@param	count_max		�t�F�[�h�Ɏg�p����V���N��
 */
//-----------------------------------------------------------------------------
static void FADE_WORK_Init( FADE_WORK* p_wk, s32 offset_start, s32 offset_end, u16 slope_start, u16 slope_end, u16 count_max )
{
	p_wk->count			= 0;
	p_wk->count_max		= count_max;

	p_wk->offset_start	= offset_start;
	p_wk->offset_dist	= offset_end - offset_start;

	p_wk->slope_start	= slope_start;
	p_wk->slope_dist	= slope_end - slope_start;
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
static s32 FADE_WORK_GetOffset( const FADE_WORK* cp_wk )
{
	s32 ans;

	ans = (cp_wk->offset_dist * cp_wk->count) / cp_wk->count_max;
	ans += cp_wk->offset_start;

	return ans;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�������̎擾
 *
 *	@param	cp_wk	���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static u16 FADE_WORK_GetSlope( const FADE_WORK* cp_wk )
{
	u32 ans;

	ans = (cp_wk->slope_dist * cp_wk->count) / cp_wk->count_max;
	ans += cp_wk->slope_start;

	return ans;
}



#ifdef PM_DEBUG
void FIELD_FOG_DEBUG_Init( FIELD_FOG_WORK* p_wk, u32 heapID )
{
	GF_ASSERT( !p_wk->p_debug_wordset );
	GF_ASSERT( !p_wk->p_debug_msgdata );
	
	// ���[�h�Z�b�g�쐬
	p_wk->p_debug_wordset = WORDSET_Create( heapID );
	p_wk->p_debug_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_tomoya_dat, heapID );

	p_wk->p_debug_strbuff		= GFL_STR_CreateBuffer( 256, heapID );
	p_wk->p_debug_strbuff_tmp	= GFL_STR_CreateBuffer( 256, heapID );

	// �t�H���g�f�[�^
	p_wk->p_debug_font = GFL_FONT_Create(
		ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

	GFL_UI_KEY_SetRepeatSpeed( 4,8 );
}

void FIELD_FOG_DEBUG_Exit( FIELD_FOG_WORK* p_wk )
{
	// �t�H���g�f�[�^
	GFL_FONT_Delete( p_wk->p_debug_font );
	p_wk->p_debug_font = NULL;


	GFL_MSG_Delete( p_wk->p_debug_msgdata );
	p_wk->p_debug_msgdata = NULL;

	WORDSET_Delete( p_wk->p_debug_wordset );
	p_wk->p_debug_wordset = NULL;

	GFL_STR_DeleteBuffer( p_wk->p_debug_strbuff );
	p_wk->p_debug_strbuff = NULL;
	GFL_STR_DeleteBuffer( p_wk->p_debug_strbuff_tmp );
	p_wk->p_debug_strbuff_tmp = NULL;

	GFL_UI_KEY_SetRepeatSpeed( 8,15 );
}


void FIELD_FOG_DEBUG_Control( FIELD_FOG_WORK* p_wk )
{
	s32 offset = FIELD_FOG_GetOffset(p_wk);
	s32 slope = FIELD_FOG_GetSlope(p_wk);

	// FOGON
	FIELD_FOG_SetFlag( p_wk, TRUE );
	
	if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )
	{
		if( (offset+0x20) <= 0x7fff )
		{
			offset += 0x20;
		}
		else
		{
			offset = 0x7fff;
		}
		if( FIELD_FOG_FADE_IsFade( p_wk ) == FALSE )
		{
			FIELD_FOG_SetOffset( p_wk, offset );
		}
	}
	else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )
	{
		if( (offset-0x20) >= 0 )
		{
			offset -= 0x20;
		}
		else
		{
			offset = 0;
		}
		if( FIELD_FOG_FADE_IsFade( p_wk ) == FALSE )
		{
			FIELD_FOG_SetOffset( p_wk, offset );
		}
	}

	if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_X) )
	{
		if( (slope + 1) < FIELD_FOG_SLOPE_MAX )
		{
			slope ++;
		}
		if( FIELD_FOG_FADE_IsFade( p_wk ) == FALSE )
		{
			FIELD_FOG_SetSlope( p_wk, slope );
		}
	}
	else if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_B|PAD_BUTTON_Y) )
	{
		if( (slope - 1) >= 0 )
		{	
			slope --;
		}
		if( FIELD_FOG_FADE_IsFade( p_wk ) == FALSE )
		{
			FIELD_FOG_SetSlope( p_wk, slope );
		}
	}
}

void FIELD_FOG_DEBUG_PrintData( FIELD_FOG_WORK* p_wk, GFL_BMPWIN* p_win )
{
			
	// �t���[�� 
	WORDSET_RegisterNumber( p_wk->p_debug_wordset, 0, FIELD_FOG_GetOffset(p_wk), 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_RegisterNumber( p_wk->p_debug_wordset, 1, FIELD_FOG_GetSlope(p_wk), 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	// �v�����g
	GFL_MSG_GetString( p_wk->p_debug_msgdata, D_TOMOYA_FOG_OFFSET, p_wk->p_debug_strbuff_tmp );

	WORDSET_ExpandStr( p_wk->p_debug_wordset, p_wk->p_debug_strbuff, p_wk->p_debug_strbuff_tmp );
	PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_win ), 0, 0, p_wk->p_debug_strbuff, p_wk->p_debug_font );

	GFL_BMPWIN_TransVramCharacter( p_win );
}
#endif
