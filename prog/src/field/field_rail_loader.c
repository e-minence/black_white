//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_rail_loader.c
 *	@brief	�t�B�[���h���[�����[�_�[
 *	@author	tomoya takahashi
 *	@date		2009.07.07
 *
 *	���W���[�����FFIELD_RAIL_LOADER
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc_def.h"

#include "field_rail_loader_func.h"
#include "field_rail_func.h"

#include "field/field_rail_loader.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
#define RAIL_DATA_HEADER_LABEL	(0x4c52)

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�I�t�Z�b�g���
//=====================================
typedef struct {
  fx32  ofs_unit;     // ���̈ړ��P��
} OFS_DATA;


//-------------------------------------
///	header
//=====================================
typedef struct {
	u16 label;
	u16 dummy;
	u32 ofsdata_offset;
	u32 pointdata_offset;
	u32 linedata_offset;
	u32 cameradata_offset;
	u32 lineposdata_offset;
	u32 end_pos;
} RAIL_DATA_HEADER;


//-------------------------------------
///	���[�N
//=====================================
struct _FIELD_RAIL_LOADER 
{
	RAIL_DATA_HEADER* p_work;
	u32 work_size;
	RAIL_SETTING setting;
};


//-----------------------------------------------------------------------------
/**
 *			�J��������A���W����A�����擾�@�֐��e�[�u��
 */
//-----------------------------------------------------------------------------
// �J��������֐��e�[�u��
static RAIL_CAMERA_FUNC* sp_RAIL_CAMERA_FUNC[FIELD_RAIL_LOADER_CAMERA_FUNC_MAX] = {
	FIELD_RAIL_CAMERAFUNC_FixPosCamera,
	FIELD_RAIL_CAMERAFUNC_FixAngleCamera,
	FIELD_RAIL_CAMERAFUNC_OfsAngleCamera,
	FIELD_RAIL_CAMERAFUNC_FixAllCamera,
	FIELD_RAIL_CAMERAFUNC_CircleCamera,
	FIELD_RAIL_CAMERAFUNC_FixLenCircleCamera,
};

// ���W����֐��e�[�u��
static RAIL_POS_FUNC* sp_RAIL_POS_FUNC[FIELD_RAIL_LOADER_LINEPOS_FUNC_MAX] = 
{
	FIELD_RAIL_POSFUNC_StraitLine,
	FIELD_RAIL_POSFUNC_CurveLine,
};

// ���W����֐��e�[�u��
static RAIL_LINE_DIST_FUNC* sp_RAIL_LINE_DIST_FUNC[FIELD_RAIL_LOADER_LINEDIST_FUNC_MAX] = 
{
	FIELD_RAIL_LINE_DIST_FUNC_StraitLine,
	FIELD_RAIL_LINE_DIST_FUNC_CircleLine,
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static void SettingClear( RAIL_SETTING* p_setting )
{
	GFL_STD_MemClear( p_setting, sizeof(RAIL_SETTING) );
	p_setting->camera_func_count		= FIELD_RAIL_LOADER_CAMERA_FUNC_MAX;
	p_setting->linepos_func_count		= FIELD_RAIL_LOADER_LINEPOS_FUNC_MAX;
	p_setting->line_dist_func_count	= FIELD_RAIL_LOADER_LINEDIST_FUNC_MAX;
	p_setting->camera_func					= sp_RAIL_CAMERA_FUNC;
	p_setting->line_pos_func				= sp_RAIL_POS_FUNC;
	p_setting->line_dist_func				= sp_RAIL_LINE_DIST_FUNC;
}
#ifdef PM_DEBUG
static void PrintPoint( const RAIL_POINT* cp_point )
{
	int i;

	TOMOYA_Printf( "*point name = %s\n", cp_point->name );
	for( i=0; i<RAIL_CONNECT_LINE_MAX; i++ )
	{
		TOMOYA_Printf( "lines[%d] = %d\n", i, cp_point->lines[i] );
		TOMOYA_Printf( "keys[%d] = %d\n", i, cp_point->keys[i] );
	}
	TOMOYA_Printf( "pos x[0x%x]y[0x%x]z[0x%x]\n", cp_point->pos.x, cp_point->pos.y, cp_point->pos.z );
	TOMOYA_Printf( "camera_set = %d\n", cp_point->camera_set );
	TOMOYA_Printf( "\n" );
}

static void PrintLine( const RAIL_LINE* cp_line )
{
	TOMOYA_Printf( "*line name = %s\n", cp_line->name );
	TOMOYA_Printf( "point_s = %d\n", cp_line->point_s );
	TOMOYA_Printf( "point_e = %d\n", cp_line->point_e );
	TOMOYA_Printf( "key = %d\n", cp_line->key );
	TOMOYA_Printf( "line_pos_set = %d\n", cp_line->line_pos_set );
	TOMOYA_Printf( "camera_set = %d\n", cp_line->camera_set );
	TOMOYA_Printf( "\n" );
}

static void PrintCamera( const RAIL_CAMERA_SET* cp_camera )
{
	TOMOYA_Printf( "camera_set\n" );
	TOMOYA_Printf( "funcindex = %d\n", cp_camera->func_index );
	TOMOYA_Printf( "\n" );
}

static void PrintLinePos( const RAIL_LINEPOS_SET* cp_linepos )
{
	TOMOYA_Printf( "linepos_set\n" );
	TOMOYA_Printf( "funcindex = %d\n", cp_linepos->func_index );
	TOMOYA_Printf( "funcdistindex = %d\n", cp_linepos->func_dist_index );
	TOMOYA_Printf( "\n" );
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����[�_�[	����
 *
 *	@param	heapID	�q�[�vID
 *
 *	@return	���[���R���g���[��
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_LOADER* FIELD_RAIL_LOADER_Create( u32 heapID )
{
	FIELD_RAIL_LOADER* p_loader;

	p_loader = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_RAIL_LOADER) );

	// �֐��z��̐ݒ�
	SettingClear( &p_loader->setting );

	return p_loader;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����[�_�[�@�j��
 *
 *	@param	p_sys		�V�X�e��
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_LOADER_Delete( FIELD_RAIL_LOADER* p_sys )
{
	FIELD_RAIL_LOADER_Clear( p_sys );
	GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	���[�����[�_�[�@���ǂݍ���
 *
 *	@param	p_sys				�V�X�e��
 *	@param	datano			�f�[�^�i���o�[
 *	@param	heapID			�q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_LOADER_Load( FIELD_RAIL_LOADER* p_sys, u32 datano, u32 heapID )
{
	u32 all_size;
	u32 siz;
	OFS_DATA* ofs_data;
	
	GF_ASSERT( p_sys );
	GF_ASSERT( !p_sys->p_work );
	p_sys->p_work = GFL_ARC_UTIL_LoadEx( ARCID_RAIL_DATA, datano, FALSE, heapID, &all_size );
	p_sys->work_size	= all_size;

	TOMOYA_Printf( "rail_data size = %d\n", all_size );

	GF_ASSERT( p_sys->p_work->label == RAIL_DATA_HEADER_LABEL );
	
	// �|�C���g�e�[�u��
	p_sys->setting.point_table = (RAIL_POINT*)(((u32)p_sys->p_work) + p_sys->p_work->pointdata_offset);
	siz = p_sys->p_work->linedata_offset - p_sys->p_work->pointdata_offset;
	siz /= sizeof(RAIL_POINT);
	p_sys->setting.point_count = siz;
	TOMOYA_Printf( "point_count = %d\n", p_sys->setting.point_count );

	// ���C���e�[�u��
	p_sys->setting.line_table = (RAIL_LINE*)(((u32)p_sys->p_work) + p_sys->p_work->linedata_offset);
	siz = p_sys->p_work->cameradata_offset - p_sys->p_work->linedata_offset;
	siz /= sizeof(RAIL_LINE);
	p_sys->setting.line_count = siz;
	TOMOYA_Printf( "line_count = %d\n", p_sys->setting.line_count );

	// �J�����Z�b�g�e�[�u��
	p_sys->setting.camera_table = (RAIL_CAMERA_SET*)(((u32)p_sys->p_work) + p_sys->p_work->cameradata_offset);
	siz = p_sys->p_work->lineposdata_offset - p_sys->p_work->cameradata_offset;
	siz /= sizeof(RAIL_CAMERA_SET);
	p_sys->setting.camera_count = siz;
	TOMOYA_Printf( "camera_count = %d\n", p_sys->setting.camera_count );

	// ���W�Z�b�g�e�[�u��
	p_sys->setting.linepos_table = (RAIL_LINEPOS_SET*)(((u32)p_sys->p_work) + p_sys->p_work->lineposdata_offset);
	siz = p_sys->p_work->end_pos - p_sys->p_work->lineposdata_offset;
	siz /= sizeof(RAIL_LINEPOS_SET);
	p_sys->setting.linepos_count = siz;
	TOMOYA_Printf( "linepos_count = %d\n", p_sys->setting.linepos_count );
	

	// �I�t�Z�b�g�p�����[�^
	ofs_data = (OFS_DATA*)(((u32)p_sys->p_work) + p_sys->p_work->ofsdata_offset);
	p_sys->setting.ofs_unit = ofs_data->ofs_unit;

	TOMOYA_Printf( "ofs_unit = %d\n", p_sys->setting.ofs_unit );



#ifdef PM_DEBUG
	{
		int i;
		
		for( i=0; i<p_sys->setting.point_count; i++ )
		{
			PrintPoint( &p_sys->setting.point_table[i] );
		}
		for( i=0; i<p_sys->setting.line_count; i++ )
		{
			PrintLine( &p_sys->setting.line_table[i] );
		}
		for( i=0; i<p_sys->setting.camera_count; i++ )
		{
			PrintCamera( &p_sys->setting.camera_table[i] );
		}
		for( i=0; i<p_sys->setting.linepos_count; i++ )
		{
			PrintLinePos( &p_sys->setting.linepos_table[i] );
		}
	}
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�_�[	���N���A
 *
 *	@param	p_sys			�V�X�e��
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_LOADER_Clear( FIELD_RAIL_LOADER* p_sys )
{
	GF_ASSERT( p_sys );
	if( p_sys->p_work )
	{
		GFL_HEAP_FreeMemory( p_sys->p_work );
		p_sys->p_work = NULL;
	}
	
	// �֐��z��̐ݒ�
	SettingClear( &p_sys->setting );
}


//----------------------------------------------------------------------------
/**
 *	@brief	���[�_�[	���[�����擾
 *
 *	@param	cp_sys	�V�X�e��
 *
 *	@return	���[�����
 */
//-----------------------------------------------------------------------------
const RAIL_SETTING* FIELD_RAIL_LOADER_GetData( const FIELD_RAIL_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );
	return &cp_sys->setting;
}


// �f�o�b�N�@�\
#ifdef PM_DEBUG
void FIELD_RAIL_LOADER_DEBUG_LoadBinary( FIELD_RAIL_LOADER* p_sys, void* p_dat, u32 size )
{
	u32 siz;
	OFS_DATA* ofs_data;
	
	GF_ASSERT( p_sys );
	GF_ASSERT( !p_sys->p_work );
	p_sys->p_work = p_dat;

	p_sys->work_size	= size;

	GF_ASSERT( p_sys->p_work->label == RAIL_DATA_HEADER_LABEL );
	
	// �|�C���g�e�[�u��
	p_sys->setting.point_table = (RAIL_POINT*)(((u32)p_sys->p_work) + p_sys->p_work->pointdata_offset);
	siz = p_sys->p_work->linedata_offset - p_sys->p_work->pointdata_offset;
	siz /= sizeof(RAIL_POINT);
	p_sys->setting.point_count = siz;
	TOMOYA_Printf( "point_count = %d\n", p_sys->setting.point_count );

	// ���C���e�[�u��
	p_sys->setting.line_table = (RAIL_LINE*)(((u32)p_sys->p_work) + p_sys->p_work->linedata_offset);
	siz = p_sys->p_work->cameradata_offset - p_sys->p_work->linedata_offset;
	siz /= sizeof(RAIL_LINE);
	p_sys->setting.line_count = siz;
	TOMOYA_Printf( "line_count = %d\n", p_sys->setting.line_count );

	// �J�����Z�b�g�e�[�u��
	p_sys->setting.camera_table = (RAIL_CAMERA_SET*)(((u32)p_sys->p_work) + p_sys->p_work->cameradata_offset);
	siz = p_sys->p_work->lineposdata_offset - p_sys->p_work->cameradata_offset;
	siz /= sizeof(RAIL_CAMERA_SET);
	p_sys->setting.camera_count = siz;
	TOMOYA_Printf( "camera_count = %d\n", p_sys->setting.camera_count );

	// ���W�Z�b�g�e�[�u��
	p_sys->setting.linepos_table = (RAIL_LINEPOS_SET*)(((u32)p_sys->p_work) + p_sys->p_work->lineposdata_offset);
	siz = p_sys->p_work->end_pos - p_sys->p_work->lineposdata_offset;
	siz /= sizeof(RAIL_LINEPOS_SET);
	p_sys->setting.linepos_count = siz;
	TOMOYA_Printf( "linepos_count = %d\n", p_sys->setting.linepos_count );
	

	// �I�t�Z�b�g�p�����[�^
	ofs_data = (OFS_DATA*)(((u32)p_sys->p_work) + p_sys->p_work->ofsdata_offset);
	p_sys->setting.ofs_unit = ofs_data->ofs_unit;

	TOMOYA_Printf( "ofs_unit = %d\n", p_sys->setting.ofs_unit );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����̎擾
 *
 *	@param	cp_sys	���[�N
 *
 *	@return	���[�����
 */
//-----------------------------------------------------------------------------
void* FIELD_RAIL_LOADER_DEBUG_GetData( const FIELD_RAIL_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->p_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����T�C�Y�̎擾
 *
 *	@param	cp_sys	���[�N
 *
 *	@return	���T�C�Y
 */
//-----------------------------------------------------------------------------
u32 FIELD_RAIL_LOADER_DEBUG_GetDataSize( const FIELD_RAIL_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->work_size;
}

#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
u32 FIELD_RAIL_LOADER_GetNearestPoint( const FIELD_RAIL_LOADER* cp_sys, const VecFx32 * pos)
{
  fx32 near_len;
  u32 count, near_count;
  u32 c_max = cp_sys->setting.point_count;
  const RAIL_POINT * point = cp_sys->setting.point_table;

  GF_ASSERT(c_max > 0);
  for (count = 0; count < c_max; count ++)
  {
    fx32 len = VEC_Distance(pos, &point[count].pos);
    if (count == 0 || near_len > len)
    { //���񂩁A�Z�������̏ꍇ�̓Z�b�g
      near_len = len;
      near_count = count;
    }
  }
  return near_count;
}


