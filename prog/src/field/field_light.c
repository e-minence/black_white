//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_light.c
 *	@brief		�t�B�[���h���C�g�V�X�e��
 *	@author		tomoya takahashi
 *	@data		2009.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include  "field_light.h"

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
///	���C�g�t�F�[�h
//=====================================
#define LIGHT_FADE_COUNT_MAX	( 30 )

//-------------------------------------
///	�A�[�J�C�u�f�[�^
//=====================================
#define LIGHT_ARC_ID			( 0 )
#define LIGHT_ARC_SEASON_NUM	( 4 )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	���C�g�P�f�[�^
//=====================================
typedef struct {
	u32			endtime;
	u8			light_flag[4];
	GXRgb		light_color[4];
	VecFx16		light_vec[4];

	GXRgb		diffuse;
	GXRgb		ambient;
	GXRgb		specular;
	GXRgb		emission;
	GXRgb		fog_color;
} LIGHT_DATA;

//-------------------------------------
///	RGB�t�F�[�h
//=====================================
typedef struct {
	u16	r_start;
	s16	r_dist;
	u16	g_start;
	s16	g_dist;
	u16	b_start;
	s16	b_dist;
} RGB_FADE;


//-------------------------------------
///	�����x�N�g���t�F�[�h
//=====================================
typedef struct {
	VecFx32	start;	// �J�n�x�N�g��
	VecFx32	normal;	// ��]��
	u16	way_dist;	// ��]��
} VEC_FADE;


//-------------------------------------
///	���C�g�t�F�[�h
//=====================================
typedef struct {

	// �t�F�[�h���Ȃ����
	u32			endtime;
	u8			light_flag[4];

	RGB_FADE		light_color[4];
	VEC_FADE		light_vec[4];
	
	RGB_FADE		diffuse;
	RGB_FADE		ambient;
	RGB_FADE		specular;
	RGB_FADE		emission;
	RGB_FADE		fog_color;

	u16				count;
	u16				count_max;
} LIGHT_FADE;


//-------------------------------------
///	�t�B�[���h���C�g�V�X�e��
//=====================================
struct _FIELD_LIGHT {
	// FOG�V�X�e��
	FIELD_FOG_WORK* p_fog;
	
	// �f�[�^�o�b�t�@
	u32			data_num;			// �f�[�^��
	LIGHT_DATA* p_data;				// �f�[�^
	u16			now_index;			// ���̔��f�C���f�b�N�X

	// ���f���
	LIGHT_DATA	reflect_data;		// ���f�f�[�^
	u16			reflect_flag;		// ���f�t���O
	BOOL		change;				// �f�[�^�ݒ�t���O

	// ���f�t�F�[�h���
	LIGHT_FADE	fade;

	// ���f����
	s32			time_second;

};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�V�X�e��
//=====================================
static void FIELD_LIGHT_Reflect( const FIELD_LIGHT* cp_sys, FIELD_FOG_WORK* p_fog );
static void FIELD_LIGHT_LoadData( FIELD_LIGHT* p_sys, u32 light_no, u32 season, u32 heapID );
static void FIELD_LIGHT_LoadDataEx( FIELD_LIGHT* p_sys, u32 arcid, u32 dataid, u32 heapID );
static void FIELD_LIGHT_ReleaseData( FIELD_LIGHT* p_sys );
static s32	FIELD_LIGHT_SearchNowIndex( const FIELD_LIGHT* cp_sys, int rtc_second );


//-------------------------------------
///	RGB�t�F�[�h
//=====================================
static void RGB_FADE_Init( RGB_FADE* p_wk, GXRgb start, GXRgb end );
static GXRgb RGB_FADE_Calc( const RGB_FADE* cp_wk, u16 count, u16 count_max );

//-------------------------------------
///	�����x�N�g���t�F�[�h
//=====================================
static void VEC_FADE_Init( VEC_FADE* p_wk, const VecFx16* cp_start, const VecFx16* cp_end );
static void VEC_FADE_Calc( const VEC_FADE* cp_wk, u16 count, u16 count_max, VecFx16* p_ans );

//-------------------------------------
///	���C�g�t�F�[�h
//=====================================
static void LIGHT_FADE_Init( LIGHT_FADE* p_wk, const LIGHT_DATA* cp_start, const LIGHT_DATA* cp_end );
static void LIGHT_FADE_Main( LIGHT_FADE* p_wk );
static BOOL LIGHT_FADE_IsFade( const LIGHT_FADE* cp_wk );
static void LIGHT_FADE_GetData( const LIGHT_FADE* cp_wk, LIGHT_DATA* p_data );








//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h���C�g	�V�X�e���쐬
 *
 *	@param	light_no		���C�g�i���o�[
 *	@param	season			�G��
 *	@param	rtc_second		�b��
 *	@param	p_fog			�t�H�O�V�X�e��
 *	@param	heapID			�q�[�v
 *	
 *	@return	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
FIELD_LIGHT* FIELD_LIGHT_Create( u32 light_no, u32 season, int rtc_second, FIELD_FOG_WORK* p_fog, u32 heapID )
{
	FIELD_LIGHT* p_sys;

	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_LIGHT) );


	// ���C�g���ǂݍ���
	FIELD_LIGHT_LoadData( p_sys, light_no, season, heapID );

	// �f�[�^���f
	p_sys->reflect_flag = TRUE;

	// ��������ݒ�
	p_sys->now_index = FIELD_LIGHT_SearchNowIndex( p_sys, rtc_second );

	// �f�[�^���f
	GFL_STD_MemCopy( &p_sys->p_data[p_sys->now_index], &p_sys->reflect_data, sizeof(LIGHT_DATA) );
	p_sys->change = TRUE;

	// �t�H�O�V�X�e����ۑ�
	p_sys->p_fog = p_fog;

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h���C�g	�V�X�e���j��
 *
 *	@param	p_sys			�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_Delete( FIELD_LIGHT* p_sys )
{
	// �f�[�^�j��
	FIELD_LIGHT_ReleaseData( p_sys );

	GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h���C�g	�V�X�e�����C��
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	rtc_second		����RTC���ԁi�b�P�ʁj
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_Main( FIELD_LIGHT* p_sys, int rtc_second )
{
	int starttime;
	
	// ���C�g�f�[�^�̗L���`�F�b�N
	if( p_sys->data_num == 0 ){
		return ;
	}

	// ���C�g�f�[�^�ύX�`�F�b�N
	{
		if( (p_sys->now_index - 1) < 0 ){
			starttime = 0;
		}else{
			starttime = p_sys->p_data[ p_sys->now_index-1 ].endtime;
		}
		
		// ���̃e�[�u���͈͓̔�����Ȃ����`�F�b�N
		if( (starttime > rtc_second) ||
			(p_sys->p_data[ p_sys->now_index ].endtime <= rtc_second) ){

			// �ύX
			p_sys->now_index  = (p_sys->now_index + 1) % p_sys->data_num;

			// �t�F�[�h�ݒ�
			LIGHT_FADE_Init( &p_sys->fade, &p_sys->reflect_data, &p_sys->p_data[ p_sys->now_index ] );
		}
	}

	// ���C�g�t�F�[�h
	if( LIGHT_FADE_IsFade( &p_sys->fade ) ){
		LIGHT_FADE_Main( &p_sys->fade );
		LIGHT_FADE_GetData( &p_sys->fade, &p_sys->reflect_data );
		p_sys->change = TRUE;
	}

	// �f�[�^�ݒ菈����
	if( p_sys->change ){
		FIELD_LIGHT_Reflect( p_sys, p_sys->p_fog );
		p_sys->change = FALSE;
	}

	// rtc���Ԃ�ۑ�
	p_sys->time_second = rtc_second;

}


//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h���C�g	���C�g���̕ύX
 *
 *	@param	light_no		���C�g�i���o�[
 *	@param	season			�G��
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_Change( FIELD_LIGHT* p_sys, u32 light_no, u32 season, u32 heapID )
{
	// ���C�g�����ēǂݍ���
	FIELD_LIGHT_LoadData( p_sys, light_no, season, heapID );

	// ��������ݒ�
	p_sys->now_index = FIELD_LIGHT_SearchNowIndex( p_sys, p_sys->time_second );

	// �t�F�[�h�J�n
	LIGHT_FADE_Init( &p_sys->fade, &p_sys->reflect_data, &p_sys->p_data[ p_sys->now_index ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h���C�g	���C�g���̕ύX�@�O�����
 *
 *	@param	arcid			�A�[�NID
 *	@param	dataid			�f�[�^ID
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_ChangeEx( FIELD_LIGHT* p_sys, u32 arcid, u32 dataid, u32 heapID )
{
	// ���C�g�����ēǂݍ���
	FIELD_LIGHT_LoadDataEx( p_sys, arcid, dataid, heapID );

	// ��������ݒ�
	p_sys->now_index = FIELD_LIGHT_SearchNowIndex( p_sys, p_sys->time_second );

	// �t�F�[�h�J�n
	LIGHT_FADE_Init( &p_sys->fade, &p_sys->reflect_data, &p_sys->p_data[ p_sys->now_index ] );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h���C�g	���f�t���O�ݒ�
 *
 *	@param	p_sys		���[�N
 *	@param	flag		�t���O
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_SetReflect( FIELD_LIGHT* p_sys, BOOL flag )
{
	p_sys->reflect_flag = flag;
	if( p_sys->reflect_flag ){
		// �f�[�^���f
		p_sys->change = TRUE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h���C�g	���f�t���O���擾
 *
 *	@param	cp_sys		���[�N
 *
 *	@retval	TRUE	���f
 *	@retval	FALSE	���f���Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_LIGHT_GetReflect( const FIELD_LIGHT* cp_sys )
{
	return cp_sys->reflect_flag;
}



//----------------------------------------------------------------------------
/**
 *	@brief	�����邩�擾
 *
 *	@param	cp_sys		�V�X�e�����[�N
 *
 *	@retval	TRUE	��
 *	@retval	FALSE	����ȊO
 */
//-----------------------------------------------------------------------------
BOOL FIELD_LIGHT_GetNight( const FIELD_LIGHT* cp_sys )
{
	if( (cp_sys->time_second >= 34200) || (cp_sys->time_second < 7200) ){
		return TRUE;
	}
	return FALSE;
}








//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	���C�g���̐ݒ�
 *
 *	@param	cp_sys	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_LIGHT_Reflect( const FIELD_LIGHT* cp_sys, FIELD_FOG_WORK* p_fog )
{
	int i;
	
	if( cp_sys->reflect_flag ){

		for( i=0; i<4; i++ ){
			if( cp_sys->reflect_data.light_flag[i] ){
				NNS_G3dGlbLightVector( i, 
						cp_sys->reflect_data.light_vec[i].x, 
						cp_sys->reflect_data.light_vec[i].y,
						cp_sys->reflect_data.light_vec[i].z );

				NNS_G3dGlbLightColor( i, 
						cp_sys->reflect_data.light_color[i] );
			}else{
				
				NNS_G3dGlbLightVector( i, 0, 0, 0 );
				NNS_G3dGlbLightColor( i, 0 );
			}
		}

		NNS_G3dGlbMaterialColorDiffAmb( cp_sys->reflect_data.diffuse,
				cp_sys->reflect_data.ambient, TRUE );

		NNS_G3dGlbMaterialColorSpecEmi( cp_sys->reflect_data.specular,
				cp_sys->reflect_data.emission, FALSE );

		FIELD_FOG_SetColorRgb( p_fog, cp_sys->reflect_data.fog_color );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�g���̓ǂݍ��ݏ���
 *	
 *	@param	p_sys		�V�X�e�����[�N
 *	@param	light_no	���C�g�i���o�[
 *	@param	season		�G��
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_LIGHT_LoadData( FIELD_LIGHT* p_sys, u32 light_no, u32 season, u32 heapID )
{
	FIELD_LIGHT_LoadDataEx( p_sys, LIGHT_ARC_ID, (light_no*LIGHT_ARC_SEASON_NUM)*season, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�g���̓ǂݍ��ݏ���
 *
 *	@param	p_sys			�V�X�e�����[�N
 *	@param	arcid			�A�[�J�C�uID
 *	@param	dataid			�f�[�^ID
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
static void FIELD_LIGHT_LoadDataEx( FIELD_LIGHT* p_sys, u32 arcid, u32 dataid, u32 heapID )
{
	u32 size;
	GF_ASSERT( !p_sys->p_data );

	p_sys->p_data	= GFL_ARC_UTIL_LoadEx( arcid, dataid, FALSE, heapID, &size );
	p_sys->data_num	= size / sizeof(LIGHT_DATA);
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�g���̔j���@����
 *
 *	@param	p_sys		�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void FIELD_LIGHT_ReleaseData( FIELD_LIGHT* p_sys )
{
	if( p_sys->p_data ){
		GFL_HEAP_FreeMemory( p_sys->p_data );
		p_sys->p_data = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	rtc�b�̂Ƃ��̃f�[�^�C���f�b�N�X���擾
 *
 *	@param	cp_sys			���[�N
 *	@param	rtc_second		rtc�b
 *	
 *	@return	�f�[�^�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static s32	FIELD_LIGHT_SearchNowIndex( const FIELD_LIGHT* cp_sys, int rtc_second )
{
	int i;

	for( i=0; i<cp_sys->data_num; i++ ){

		if( cp_sys->p_data[i].endtime > rtc_second ){
			return i;
		}
	}

	// ���肦�Ȃ�
	GF_ASSERT( 0 );
	return 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief	RGB�t�F�[�h	������
 *
 *	@param	p_wk		���[�N
 *	@param	start		�J�n�F
 *	@param	end			�I���F
 */
//-----------------------------------------------------------------------------
static void RGB_FADE_Init( RGB_FADE* p_wk, GXRgb start, GXRgb end )
{
	p_wk->r_start		= (start & GX_RGB_R_MASK )>> GX_RGB_R_SHIFT;
	p_wk->r_dist		= ((end & GX_RGB_R_MASK )>> GX_RGB_R_SHIFT) - p_wk->r_start;
	p_wk->g_start		= (start & GX_RGB_G_MASK )>> GX_RGB_G_SHIFT;
	p_wk->g_dist		= ((end & GX_RGB_G_MASK )>> GX_RGB_G_SHIFT) - p_wk->g_start;
	p_wk->b_start		= (start & GX_RGB_B_MASK )>> GX_RGB_B_SHIFT;
	p_wk->b_dist		= ((end & GX_RGB_B_MASK )>> GX_RGB_B_SHIFT) - p_wk->b_start;
}

//----------------------------------------------------------------------------
/**
 *	@brief	RGB�t�F�[�h	�v�Z����
 *
 *	@param	cp_wk			���[�N
 *	@param	count			�J�E���g�l
 *	@param	count_max		�J�E���g�ő�l
 *
 *	@return	RGB�J���[
 */
//-----------------------------------------------------------------------------
static GXRgb RGB_FADE_Calc( const RGB_FADE* cp_wk, u16 count, u16 count_max )
{
	u8 r, g, b;

	r = (cp_wk->r_dist * count) / count_max;
	r += cp_wk->r_start;
	g = (cp_wk->g_dist * count) / count_max;
	g += cp_wk->g_start;
	b = (cp_wk->b_dist * count) / count_max;
	b += cp_wk->b_start;

	return GX_RGB( r, g, b );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����x�N�g���t�F�[�h	������
 *
 *	@param	p_wk			���[�N
 *	@param	cp_start		�J�n
 *	@param	cp_end			�I��
 */
//-----------------------------------------------------------------------------
static void VEC_FADE_Init( VEC_FADE* p_wk, const VecFx16* cp_start, const VecFx16* cp_end )
{
	fx32 cos;
	VecFx16 normal;

	VEC_Set( &p_wk->start, cp_start->x, cp_start->y, cp_start->z );
	VEC_Fx16CrossProduct( cp_start, cp_end, &normal ); 
	VEC_Set( &p_wk->normal, normal.x, normal.y, normal.z );
	cos				= VEC_Fx16DotProduct( cp_start, cp_end ); 
	p_wk->way_dist	= FX_AcosIdx( cos );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����x�N�g���̃t�F�[�h���v�Z
 *
 *	@param	cp_wk			���[�N
 *	@param	count			�J�E���g�l
 *	@param	count_max		�J�E���g�ő�l
 *	@param	p_ans			�v�Z����
 */	
//-----------------------------------------------------------------------------
static void VEC_FADE_Calc( const VEC_FADE* cp_wk, u16 count, u16 count_max, VecFx16* p_ans )
{
	u16	rotate;
	MtxFx43	mtx;
	VecFx32 way;

	rotate = (cp_wk->way_dist * count) / count_max;
	MTX_RotAxis43( &mtx, &cp_wk->normal, FX_SinIdx( rotate ), FX_CosIdx( rotate ) );

	MTX_MultVec43( &cp_wk->start, &mtx, &way );
	VEC_Normalize( &way, &way );

	VEC_Fx16Set( p_ans, way.x, way.y, way.z );
}


//----------------------------------------------------------------------------
/**
 *	@brief	���C�g�t�F�[�h����	������
 *
 *	@param	p_wk			���[�N
 *	@param	cp_start		�J�n���C�g���
 *	@param	cp_end			�I�����C�g���
 */
//-----------------------------------------------------------------------------
static void LIGHT_FADE_Init( LIGHT_FADE* p_wk, const LIGHT_DATA* cp_start, const LIGHT_DATA* cp_end )
{
	int i;

	p_wk->count			= 0;
	p_wk->count_max		= LIGHT_FADE_COUNT_MAX;


	// �t�F�[�h���Ȃ������ۑ�
	p_wk->endtime = cp_end->endtime;
	
	for( i=0; i<4; i++ ){

		RGB_FADE_Init( &p_wk->light_color[i], cp_start->light_color[i], cp_end->light_color[i] );
		VEC_FADE_Init( &p_wk->light_vec[i], &cp_start->light_vec[i], &cp_end->light_vec[i] );

		p_wk->light_flag[i] = cp_end->light_flag[i];
	}

	RGB_FADE_Init( &p_wk->diffuse, cp_start->diffuse, cp_end->diffuse );
	RGB_FADE_Init( &p_wk->ambient, cp_start->ambient, cp_end->ambient );
	RGB_FADE_Init( &p_wk->specular, cp_start->specular, cp_end->specular );
	RGB_FADE_Init( &p_wk->emission, cp_start->emission, cp_end->emission );
	RGB_FADE_Init( &p_wk->fog_color, cp_start->fog_color, cp_end->fog_color );

}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�g�t�F�[�h����	���C��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void LIGHT_FADE_Main( LIGHT_FADE* p_wk )
{
	if( p_wk->count < p_wk->count_max ){
		p_wk->count ++;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�g�t�F�[�h����	�t�F�[�h���`�F�b�N
 *
 *	@param	cp_wk	���[�N
 *
 *	@retval	TRUE	�t�F�[�h��
 *	@retval	FALSE	�t�F�[�h���ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL LIGHT_FADE_IsFade( const LIGHT_FADE* cp_wk )
{
	if( cp_wk->count <= cp_wk->count_max ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�g�t�F�[�h����	���C�g�f�[�^�擾
 *
 *	@param	cp_wk		���[�N	
 *	@param	p_data		���C�g�f�[�^�i�[��
 */
//-----------------------------------------------------------------------------
static void LIGHT_FADE_GetData( const LIGHT_FADE* cp_wk, LIGHT_DATA* p_data )
{
	int i;

	p_data->endtime		= cp_wk->endtime;
	
	for( i=0; i<4; i++ ){
		p_data->light_color[i]	= RGB_FADE_Calc( &cp_wk->light_color[i], cp_wk->count, cp_wk->count_max );
		VEC_FADE_Calc( &cp_wk->light_vec[i], cp_wk->count, cp_wk->count_max, &p_data->light_vec[i] );

		p_data->light_flag[i]	= cp_wk->light_flag[i];
	}

	p_data->diffuse		= RGB_FADE_Calc( &cp_wk->diffuse, cp_wk->count, cp_wk->count_max );
	p_data->ambient		= RGB_FADE_Calc( &cp_wk->ambient, cp_wk->count, cp_wk->count_max );
	p_data->specular	= RGB_FADE_Calc( &cp_wk->specular, cp_wk->count, cp_wk->count_max );
	p_data->emission	= RGB_FADE_Calc( &cp_wk->emission, cp_wk->count, cp_wk->count_max );
	p_data->fog_color	= RGB_FADE_Calc( &cp_wk->fog_color, cp_wk->count, cp_wk->count_max );

}





