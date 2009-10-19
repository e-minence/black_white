//============================================================================================
/**
 * @file  field_rail.c
 * @brief �m�[�O���b�h�ł̎��@�E�J��������\��
 * @author  tamada, tomoya
 * @date  2009.05.18
 *
 */
//============================================================================================

#include <gflib.h>

#include "field/fieldmap_proc.h"
#include "field/field_dir.h"

#include "field_rail.h"
#include "field_rail_access.h"

#include "field_camera.h"

#ifdef PM_DEBUG

//#define DEBUG_RAIL_PRINT  // ���[���̃f�o�b�N�o�́@�i��ʁj

#endif // PM_DEBUG


#ifdef DEBUG_RAIL_PRINT

#define DEBUG_RAIL_Printf( ... )  OS_TPrintf( __VA_ARGS__ )

#else // DEBUG_RAIL_PRINT

#define DEBUG_RAIL_Printf( ... )  ((void)0)

#endif // DEBUG_RAIL_PRINT


//============================================================================================
//============================================================================================

// �����J�E���g
static u8 RAIL_COUNTUP[ RAIL_FRAME_MAX ] = 
{
  1,        //RAIL_FRAME_16,
  2,        //RAIL_FRAME_8,
  4,        //RAIL_FRAME_4,
  8,       //RAIL_FRAME_2,
  16,       //RAIL_FRAME_2,
};

//============================================================================================
//
//
//
//    ���[���\�����������邽�߂̃c�[��
//
//
//
//============================================================================================

//------------------------------------------------------------------
//------------------------------------------------------------------
static const RAIL_LINEPOS_SET RAIL_LINEPOS_SET_Default =
{
  RAIL_TBL_NULL,
  RAIL_TBL_NULL,
  0,
  0,
  0,
  0,
};


//============================================================================================
//
//
//      �J�����w��̂��߂̃c�[��
//
//
//============================================================================================
//-----------------------------------------------------------------
//------------------------------------------------------------------
static const RAIL_CAMERA_SET RAIL_CAMERA_SET_Default =
{
  RAIL_TBL_NULL,
  0,
  0,
  0,
  0,
};


//------------------------------------------------------------------
//------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
struct _FIELD_RAIL_WORK{  
  FIELD_RAIL_TYPE type;
  union { 
    void * dummy;
    const RAIL_POINT * point;
    const RAIL_LINE * line;
  };
  u16 active;
	u16 dat_index;	// POINT or LINE�̃C���f�b�N�X
  /// LINE�ɂ���Ԃ́ALINE��ł̃I�t�Z�b�g�ʒu
  s32 line_ofs;
  s32 line_ofs_max;
  /// LINE�ɂ���Ԃ̕��I�t�Z�b�g�ʒu(XZ���ʏ��LINE�ɐ���)
  s32 width_ofs;
  s32 width_ofs_max;  // ��������������
  fx32 ofs_unit;       // 1�����̋���

  RAIL_KEY key;

	/// ���[���f�[�^
	const RAIL_SETTING* rail_dat;


	///<���t���[���ł̓���p���
	u8	  req_move;
	s8		ofs_move;
	u16		save_move_key;    // RAIL_KEY
	u16		save_move_frame;  // RAIL_FRAME

  /// �ړ����N�����ŐV�̃L�[�o�����[
  u8 last_key;           // RAIL_KEY
  u8 last_frame;         // RAIL_FRAME
  u8 last_move;         // �ŐV�̓���ŁA�ړ������̂��`�F�b�N
  u8 last_pad;          // �\��
  
  VecFx32 pos;  // ���ݍ��W
  RAIL_LOCATION now_location;

  // �ߋ��̏��
  RAIL_LOCATION last_location;


  const FIELD_RAIL_MAN* cp_man;
};

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
struct _FIELD_RAIL_MAN{
  HEAPID heapID;

  u16 active_flag;
  u16 user_active_flag;

  FIELD_RAIL_WORK * rail_work;
  int work_num;

  // �J�������
  FIELD_CAMERA * field_camera;
  const FIELD_RAIL_WORK * camera_bind_work;
  BOOL camera_update; // �J���������A�b�v�f�[�g

  // ���P�[�V�����݂̂ł̌v�Z�p
  FIELD_RAIL_WORK* calc_work;


  /// ���C���A�|�C���g�e�[�u��
	RAIL_SETTING rail_dat;
};

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static int RailKeyTokeyCont(RAIL_KEY key);
static RAIL_KEY keyContToRailKey(int cont);
static RAIL_KEY getReverseKey(RAIL_KEY key);
static RAIL_KEY getClockwiseKey(RAIL_KEY key);
static RAIL_KEY getAntiClockwiseKey(RAIL_KEY key);




//------------------------------------------------------------------
//------------------------------------------------------------------
static void initRail(FIELD_RAIL_WORK * work, const RAIL_SETTING* rail_dat, const FIELD_RAIL_MAN* cp_man );
static void clearRail( FIELD_RAIL_WORK * work, const RAIL_SETTING* rail_dat );
static BOOL isValidRail(const FIELD_RAIL_WORK * work);
static const RAIL_CAMERA_SET * getCameraSet(const FIELD_RAIL_WORK * work);
static const char * getRailName(const FIELD_RAIL_WORK * work);
static void getRailPosition(const FIELD_RAIL_WORK * work, VecFx32 * pos);
static void getRailLocation( const FIELD_RAIL_WORK * work, RAIL_LOCATION * location );
static s32 getLineOfsMax( const RAIL_LINE * line, fx32 unit, const RAIL_SETTING* rail_dat );
static s32 getLineWidthOfsMax( const RAIL_LINE * line, u32 line_ofs, u32 line_ofs_max, const RAIL_SETTING* rail_dat );
static RAIL_KEY updateLine( FIELD_RAIL_WORK * work, const RAIL_LINE * line, u32 line_ofs_max, u32 key );
static RAIL_KEY setLine(FIELD_RAIL_WORK * work, const RAIL_LINE * line, RAIL_KEY key, int l_ofs, int w_ofs, int l_ofs_max, int w_ofs_max);
static void setLineData(FIELD_RAIL_WORK * work, const RAIL_LINE * line, RAIL_KEY key, int l_ofs, int w_ofs, int l_ofs_max, int w_ofs_max);
static BOOL isLinePoint_S( const RAIL_SETTING * rail_dat, const RAIL_LINE * line, const RAIL_POINT * point );
static BOOL isLinePoint_E( const RAIL_SETTING * rail_dat, const RAIL_LINE * line, const RAIL_POINT * point );

//------------------------------------------------------------------
//------------------------------------------------------------------
static void clearRailDat( RAIL_SETTING * raildat );
static void initRailDat( RAIL_SETTING * raildat, const RAIL_SETTING * setting );
static const RAIL_POINT* getRailDatPoint( const RAIL_SETTING * raildat, u32 index );
static const RAIL_LINE* getRailDatLine( const RAIL_SETTING * raildat, u32 index );
static const RAIL_CAMERA_SET* getRailDatCamera( const RAIL_SETTING * raildat, u32 index );
static const RAIL_LINEPOS_SET* getRailDatLinePos( const RAIL_SETTING * raildat, u32 index );
static RAIL_CAMERA_FUNC*const getRailDatCameraFunc( const RAIL_SETTING * raildat, u32 index );
static RAIL_POS_FUNC*const getRailDatLinePosFunc( const RAIL_SETTING * raildat, u32 index );
static RAIL_LINE_DIST_FUNC*const getRailDatLineDistFunc( const RAIL_SETTING * raildat, u32 index );
static RAIL_LINE_HIT_LOCATION_FUNC*const getRailDatLineHitLocationFunc( const RAIL_SETTING * raildat, u32 index );

//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL debugCheckLineData(const RAIL_LINE * line, const RAIL_SETTING* rail_dat);
static BOOL debugCheckPointData(const RAIL_POINT * point, const RAIL_SETTING* rail_dat);
static void debugPrintPoint(const char * before, const RAIL_POINT * point, const char * after);
static const char * debugGetRailKeyName(RAIL_KEY key);
static void debugPrintRailInfo(const FIELD_RAIL_WORK * work);


static RAIL_KEY updateLineMove_new(FIELD_RAIL_WORK * work, RAIL_KEY key, u32 count_up);
//static RAIL_KEY updateLineMove(FIELD_RAIL_WORK * work, RAIL_KEY key);
static RAIL_KEY updatePointMove(FIELD_RAIL_WORK * work, RAIL_KEY key, u32 count_up);


static u32 getPointIndex( const RAIL_SETTING * raildat, const RAIL_POINT* point );
static u32 getLineIndex( const RAIL_SETTING * raildat, const RAIL_LINE* line );


static BOOL calcHitPlaneVec( const VecFx32* plane_vec1, const VecFx32* plane_vec2, const VecFx32* plane_pos, const VecFx32* vec_s, const VecFx32* vec_e, VecFx32* cross );



//============================================================================================
//============================================================================================
static void RAIL_LOCATION_Dump(const RAIL_LOCATION * railLoc)
{
  TAMADA_Printf("RAIL_LOC:type = %d, rail_index = %d\n",railLoc->type, railLoc->rail_index);
  TAMADA_Printf("RAIL_LOC:line_grid = %d, width_grid = %d\n", railLoc->line_grid, railLoc->width_grid);
  TAMADA_Printf("RAIL_LOC:key = %s\n", debugGetRailKeyName(railLoc->key));
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ���[������V�X�e���̐���
 */
//------------------------------------------------------------------
FIELD_RAIL_MAN * FIELD_RAIL_MAN_Create(HEAPID heapID, u32 work_num, FIELD_CAMERA * camera)
{ 
  int i;
  FIELD_RAIL_MAN * man = GFL_HEAP_AllocMemory(heapID, sizeof(FIELD_RAIL_MAN));

  man->heapID = heapID;
  man->active_flag = FALSE;
  man->user_active_flag = TRUE;
  man->field_camera = camera;
  man->camera_bind_work = NULL;

  // ���[�����[�N�̃������m��
  man->rail_work = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_RAIL_WORK)*work_num );
  man->work_num = work_num;
  for( i=0; i<work_num; i++ )
  {
    initRail(&man->rail_work[i], &man->rail_dat, man);
  }

  // �v�Z�p���[�N��������
  man->calc_work = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_RAIL_WORK) );
  initRail(man->calc_work, &man->rail_dat, man);
  
  return man;
}

//------------------------------------------------------------------
/**
 * @brief ���[������V�X�e���̍폜
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Delete(FIELD_RAIL_MAN * man)
{ 
  GFL_HEAP_FreeMemory( man->calc_work );
  GFL_HEAP_FreeMemory( man->rail_work );
  GFL_HEAP_FreeMemory(man);
}

//------------------------------------------------------------------
/**
 * @brief ���[���f�[�^�̓ǂݍ���
 *
 * �Ƃ肠�����A�ŏ���POINT����n�܂�Ɖ��肷��
 *
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Load(FIELD_RAIL_MAN * man, const RAIL_SETTING * setting)
{ 
  man->active_flag = TRUE;

  // �|�C���g�A���C���e�[�u���ݒ�
	initRailDat( &man->rail_dat, setting );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �N���A
 *
 *	@param	man �}�l�[�W��
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_Clear(FIELD_RAIL_MAN * man)
{
  man->active_flag = FALSE;
  // �|�C���g�A���C���e�[�u���ݒ�
	clearRailDat( &man->rail_dat );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����̗L�����m�F
 *
 *	@param	man   �}�l�[�W��
 *
 *	@retval TRUE  ����
 *	@retval FALSE �Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_IsLoad( const FIELD_RAIL_MAN * man )
{
  if( man->rail_dat.point_count == 0 )
  {
    return FALSE;
  }
  return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��Ǘ����[�N�̐���
 *
 *	@param	man   �}�l�[�W��
 *
 *	@return �ړ��Ǘ����[�N
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_WORK* FIELD_RAIL_MAN_CreateWork( FIELD_RAIL_MAN * man )
{
  FIELD_RAIL_WORK* work;
  int i;

  // �󂢂Ă��郏�[�N��T��
  work = NULL;
  for( i=0; i<man->work_num; i++ )
  {
    if( isValidRail( &man->rail_work[i] ) == FALSE )
    {
      work = &man->rail_work[i];
      break;
    }
  }
  // �����Ȃ��I
  GF_ASSERT( work );

  return work;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��Ǘ����[�N�̔j��
 *
 *	@param	man   �}�l�[�W��
 *	@param	work  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_DeleteWork( FIELD_RAIL_MAN * man, FIELD_RAIL_WORK* work )
{
  initRail( work, &man->rail_dat, man );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����Ƀ��[�����[�N���o�C���h����
 *
 *	@param	man   �}�l�[�W��
 *	@param	work  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_BindCamera( FIELD_RAIL_MAN * man, const FIELD_RAIL_WORK* work )
{
  man->camera_bind_work = work;
  man->camera_update    = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����̃��[�����[�N�̃o�C���h���قǂ�
 *
 *	@param	man   �}�l�[�W��
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_UnBindCamera( FIELD_RAIL_MAN * man )
{
  man->camera_bind_work = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�C���h�����A���[�����[�N�̍��W�擾
 *
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_GetBindWorkPos( const FIELD_RAIL_MAN * man, VecFx32* pos )
{
  GF_ASSERT( man );
  if(man->camera_bind_work)
  {
    FIELD_RAIL_WORK_GetPos( man->camera_bind_work, pos );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�C���h�����A���[�����[�N�����삵�Ă��邩�`�F�b�N
 *
 *	@retval TRUE  ���삵�Ă���
 *	@retval FALSE ���삵�Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_IsBindWorkMove( const FIELD_RAIL_MAN * man )
{
  GF_ASSERT( man );
  if(man->camera_bind_work)
  {
    if( FIELD_RAIL_WORK_IsAction( man->camera_bind_work ) || FIELD_RAIL_WORK_IsLastAction( man->camera_bind_work ) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef PM_DEBUG
// �f�o�b�N��p�A�o�C���h�������[�N��ҏW�\�Ȍ`�Ŏ擾
FIELD_RAIL_WORK* FIELD_RAIL_MAN_DEBUG_GetBindWork( const FIELD_RAIL_MAN * man )
{
  return (FIELD_RAIL_WORK*)man->camera_bind_work;
}


// �f�[�^�����肩����
void FIELD_RAIL_MAN_DEBUG_ChangeData(FIELD_RAIL_MAN * man, const RAIL_SETTING * setting)
{
  initRailDat( &man->rail_dat, setting );
}
#endif

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_SetActiveFlag(FIELD_RAIL_MAN * man, BOOL flag)
{
  man->user_active_flag = flag;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_GetActiveFlag(const FIELD_RAIL_MAN *man)
{
  return man->user_active_flag;
}

//------------------------------------------------------------------
//  �S���[�N�̃A�b�v�f�[�g
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Update(FIELD_RAIL_MAN * man)
{
  int i;


  if( !man->active_flag )
  {
    return ;
  }

  if( !man->user_active_flag )
  {
    return ;
  }

#if 0 // �e���[�N���ƂɃA�b�v�f�[�g����悤�ɕύX
  for( i=0; i<man->work_num; i++ )
  {
    FIELD_RAIL_WORK_Update( &man->rail_work[i] );
  }
#endif
}


//------------------------------------------------------------------
/**
 * @brief �J������Ԃ̃A�b�v�f�[�g
 *
 * @retval  TRUE  �J������Ԃ̍X�V
 * @retval  FALSE �J������Ԃ�ς��Ȃ�����
 */
//------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_UpdateCamera(FIELD_RAIL_MAN * man)
{
  const FIELD_RAIL_WORK * work = man->camera_bind_work;
	const RAIL_CAMERA_SET* camera_set;
  RAIL_CAMERA_FUNC * func = NULL;

  if (!man->active_flag)
  {
    return FALSE;
  }
  
  if ( !work )
  {
    return FALSE;
  }

  if (isValidRail(work) == FALSE)
  {
    return FALSE;
  }

  // ���삵���t���[�����`�F�b�N
  if( (FIELD_RAIL_WORK_IsLastAction( work ) == FALSE) && (man->camera_update == FALSE) )
  {
    return FALSE;
  }


  switch (work->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    if (work->point->camera_set != RAIL_TBL_NULL)
    {
			camera_set = getRailDatCamera( work->rail_dat, work->point->camera_set );
      func = getRailDatCameraFunc( work->rail_dat, camera_set->func_index );
    }
    break;
  case FIELD_RAIL_TYPE_LINE:
    if (work->line->camera_set != RAIL_TBL_NULL)
    {
			camera_set = getRailDatCamera( work->rail_dat, work->line->camera_set );
      func = getRailDatCameraFunc( work->rail_dat, camera_set->func_index );
    }
    break;
  default:
    GF_ASSERT(0); //NO TYPE;
  }
  if (func)
  {
    func(man);
  }

  man->camera_update = FALSE;

  return TRUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �R�c���W����q�b�g���郌�[�����P�[�V�������擾
 *
 *	@param	cp_man          ���[���}�l�[�W��
 *	@param	cp_pos          ���W
 *	@param	p_location      ���[�����P�[�V����
 *	@param  p_locpos        ���P�[�V�����̍��W
 *
 *	@retval TRUE    �Ή����郌�[�����P�[�V����������
 *	@retval FALSE   �Ή����郌�[�����P�[�V�������Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_Calc3DPosRailLocation( const FIELD_RAIL_MAN * cp_man, const VecFx32* cp_pos, RAIL_LOCATION* p_location, VecFx32* p_locpos )
{
  int i;
  const RAIL_LINE* cp_line;
  const RAIL_POINT* cp_point_s;
  const RAIL_POINT* cp_point_e;
  BOOL result;
  VecFx32 pos;
  
  GF_ASSERT( cp_man );
  GF_ASSERT( cp_pos );
  GF_ASSERT( p_location );

  pos = *cp_pos;

  // �x���W���덷�ȓ��ŁA���̓_�ɂȂ��郉�C����ɂ���̂��i���ς��g�p����j�B
  for( i=0; i<cp_man->rail_dat.line_count; i++ )
  {
    cp_line = getRailDatLine( &cp_man->rail_dat, i );
    cp_point_s = getRailDatPoint( &cp_man->rail_dat, cp_line->point_s );
    cp_point_e = getRailDatPoint( &cp_man->rail_dat, cp_line->point_e );
    
    // ���߂�
    {
	    RAIL_LINE_HIT_LOCATION_FUNC* func;
      func = getRailDatLineHitLocationFunc( &cp_man->rail_dat, cp_line->line_pos_set );
      if( func )
      {
        result = func( i, cp_man, &pos, p_location, p_locpos );
        // �q�b�g
        if( result )
        {
          return result;
        }
      }
    }
  }

  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �R�c�x�N�g������q�b�g���郌�[�����P�[�V�������擾
 *  
 *	@param	cp_man        ���[���}�l�[�W��
 *	@param	cp_startpos   �J�n���W
 *	@param	cp_endpos     �I�����W
 *	@param	p_location    ���[�����P�[�V����
 *
 *	@retval TRUE    �Ή����郌�[�����P�[�V����������
 *	@retval FALSE   �Ή����郌�[�����P�[�V�������Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_Calc3DVecRailLocation( const FIELD_RAIL_MAN * cp_man, const VecFx32* cp_startpos, const VecFx32* cp_endpos, RAIL_LOCATION* p_location, VecFx32* p_locpos )
{
  int i;
  const RAIL_LINE* cp_line;
  const RAIL_POINT* cp_point_s;
  const RAIL_POINT* cp_point_e;
  const RAIL_LINEPOS_SET* cp_linepos;
  VecFx32 cross_pos, plane_vec1, plane_vec2;
  VecFx32 default_vec = {0, FX32_ONE, 0};
  RAIL_LOCATION min_location;
  VecFx32 min_pos;
  fx32 min_dist, dist;
  BOOL result;
  BOOL data_in;
  VecFx32 startpos, endpos;
  GF_ASSERT( cp_man );
  GF_ASSERT( cp_startpos );
  GF_ASSERT( cp_endpos );
  GF_ASSERT( p_location );

  startpos = *cp_startpos;
  endpos = *cp_endpos;

  // �x���W���덷�ȓ��ŁA���̓_�ɂȂ��郉�C����ɂ���̂��i���ς��g�p����j�B
  data_in = FALSE;
  for( i=0; i<cp_man->rail_dat.line_count; i++ )
  {
    cp_line = getRailDatLine( &cp_man->rail_dat, i );
    cp_point_s = getRailDatPoint( &cp_man->rail_dat, cp_line->point_s );
    cp_point_e = getRailDatPoint( &cp_man->rail_dat, cp_line->point_e );
    cp_linepos = getRailDatLinePos( &cp_man->rail_dat, cp_line->line_pos_set );

    DEBUG_RAIL_Printf( "\nlinename %s\n", cp_line->name );

    VEC_Subtract( &cp_point_s->pos, &cp_point_e->pos, &plane_vec1 );
    VEC_CrossProduct( &plane_vec1, &default_vec, &plane_vec2 );

    // ���ʂƃx�N�g���̌�_����A�q�b�g������s���B
    result = calcHitPlaneVec( &plane_vec1, &plane_vec2, &cp_point_e->pos, &startpos, &endpos, &cross_pos ); 

    if( result == FALSE )
    {
      continue;
    }

    // ���߂�
    {
	    RAIL_LINE_HIT_LOCATION_FUNC* func;
      func = getRailDatLineHitLocationFunc( &cp_man->rail_dat, cp_linepos->func_index );
      if( func )
      {
        result = func( i, cp_man, &cross_pos, p_location, p_locpos );
        // �q�b�g
        if( result )
        {
          // ��ԋ߂��|�W�V�����̏����g��
          if( data_in == FALSE )
          {
            min_location = *p_location;
            min_pos = *p_locpos;
            min_dist = VEC_Distance( &startpos, p_locpos );

            data_in = TRUE;
          }
          else
          {
            // �ǂ������������H
            dist = VEC_Distance( &startpos, p_locpos );
            if( dist < min_dist )
            {
              min_location = *p_location;
              min_pos = *p_locpos;
              min_dist = dist;
            }
          }
        }
      }
    }
  }

  if( data_in )
  {
    *p_location = min_location;
    *p_locpos = min_pos;
  }

  return data_in;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[���O���b�h�̃T�C�Y���擾
 *
 *	@param	man   �}�l�[�W��
 *
 *	@return
 */
//-----------------------------------------------------------------------------
fx32 FIELD_RAIL_MAN_GetRailGridSize( const FIELD_RAIL_MAN * man )
{
  GF_ASSERT( man );

  return FX_Mul( man->rail_dat.ofs_unit, RAIL_WALK_OFS<<FX32_SHIFT );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�������L�[�̕����ɂP�O���b�h���i�߂��RD���W���擾
 *
 *	@param	man             �}�l�[�W��
 *	@param	now_location    ���̃��P�[�V����
 *	@param	key             �L�[
 *	@param	pos             �RD���W
 *
 *	@retval TRUE  �P�O���b�h�i�߂�
 *	@retval FALSE �P�O���b�h�����߂Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_CalcRailKeyPos(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * now_location, RAIL_KEY key, VecFx32* pos)
{
  GF_ASSERT( man );
  GF_ASSERT( now_location );
  GF_ASSERT( pos );

  FIELD_RAIL_WORK_SetLocation( man->calc_work, now_location );
  FIELD_RAIL_WORK_ForwardReq( man->calc_work, RAIL_FRAME_1, key );
  FIELD_RAIL_WORK_Update( man->calc_work );
  FIELD_RAIL_WORK_GetPos( man->calc_work, pos );

  return FIELD_RAIL_WORK_IsLastAction( man->calc_work );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�������L�[�̕����ɂP�O���b�h���i�߂����P�[�V�������擾
 *
 *	@param	man             �}�l�[�W��
 *	@param	now_location    ���̃��P�[�V����
 *	@param	key             �L�[
 *	@param	next_location   ���̃��P�[�V�����i�[��
 *
 *	@retval TRUE  �P�O���b�h�i�߂�
 *	@retval FALSE �P�O���b�h�����߂Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_CalcRailKeyLocation(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * now_location, RAIL_KEY key, RAIL_LOCATION * next_location)
{
  GF_ASSERT( man );
  GF_ASSERT( now_location );
  GF_ASSERT( next_location );

  FIELD_RAIL_WORK_SetLocation( man->calc_work, now_location );
  FIELD_RAIL_WORK_ForwardReq( man->calc_work, RAIL_FRAME_1, key );
  FIELD_RAIL_WORK_Update( man->calc_work );
  FIELD_RAIL_WORK_GetLocation( man->calc_work, next_location );

  return FIELD_RAIL_WORK_IsLastAction( man->calc_work );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�����̂RD���W���擾����
 *
 *	@param	man           �}�l�[�W��
 *	@param	cp_location   ���P�[�V����
 *	@param	pos           �ʒu
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_MAN_GetLocationPosition(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * location, VecFx32* pos )
{
  GF_ASSERT( man );
  GF_ASSERT( location );

  FIELD_RAIL_WORK_SetLocation( man->calc_work, location );
  FIELD_RAIL_WORK_GetPos( man->calc_work, pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  location�̃��C���̑S���O���b�h�T�C�Y���擾
 *
 *	@param	man         �܂ˁ[����
 *	@param	location    ���P�[�V����
 *
 *	@return �O���b�h�T�C�Y
 */
//-----------------------------------------------------------------------------
u32 FIELD_RAIL_MAN_GetLocationLineOfsMaxGrid(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * location )
{
  GF_ASSERT( man );
  GF_ASSERT( location );

  FIELD_RAIL_WORK_SetLocation( man->calc_work, location );
  return RAIL_OFS_TO_GRID( man->calc_work->line_ofs_max  );
}

//----------------------------------------------------------------------------
/**
 *	@brief  location�̃��C���̕��O���b�h�T�C�Y���擾
 *
 *	@param	man       �}�l�[�W��
 *	@param	location  ���P�[�V����
 *
 *	@return ���T�C�Y
 */
//-----------------------------------------------------------------------------
u32 FIELD_RAIL_MAN_GetLocationWidthGrid(const FIELD_RAIL_MAN * man, const RAIL_LOCATION * location )
{
  GF_ASSERT( man );
  GF_ASSERT( location );

  FIELD_RAIL_WORK_SetLocation( man->calc_work, location );
  return RAIL_OFS_TO_GRID( man->calc_work->width_ofs_max );
}


//============================================================================================
//
//
//  ���[���ړ�����̂��߂̊֐��Q
//
//
//============================================================================================



//----------------------------------------------------------------------------
/**
 *	@brief  ���[���ʒu���̎擾
 *
 *	@param	work      ���[�N
 *	@param	location  �ʒu���i�[��
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_WORK_GetLocation(const FIELD_RAIL_WORK * work, RAIL_LOCATION * location)
{
  GF_ASSERT( work );
  GF_ASSERT( location );

  // ���P�[�V�����̎擾
  *location = work->now_location;
  RAIL_LOCATION_Dump(location);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �P�O�̃��P�[�V�����̎擾
 *
 *	@param	work        ���[�N
 *	@param	location    ���P�[�V����
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_WORK_GetLastLocation(const FIELD_RAIL_WORK * work, RAIL_LOCATION * location)
{
  GF_ASSERT( work );
  GF_ASSERT( location );

  // ���P�[�V�����̎擾
  *location = work->last_location;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�����̗L�������`�F�b�N
 *
 *	@param	work        ���[�N
 *	@param	location    ���P�[�V����
 *
 *	@retval TRUE    �L��
 *	@retval FALSE   ����
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_WORK_CheckLocation( const FIELD_RAIL_WORK * work, const RAIL_LOCATION * location )
{
  GF_ASSERT( work );
  GF_ASSERT( location );


  if( location->type==FIELD_RAIL_TYPE_POINT )
  {
    // �|�C���g�C���f�b�N�X
    if( location->rail_index >= work->rail_dat->point_count )
    {
      return FALSE;
    }
  }
  else
  {
    const RAIL_LINE* line = NULL;
    s32 line_ofs;
    s32 width_ofs;
    s32 line_max;
    s32 width_max;
    
    // ���C���C���f�b�N�X
    if( location->rail_index >= work->rail_dat->line_count )
    {
      return FALSE;
    }

    line        = &work->rail_dat->line_table[ location->rail_index ];
    line_ofs    = RAIL_GRID_TO_OFS(location->line_grid);
    width_ofs   = RAIL_GRID_TO_OFS(location->width_grid);
    line_max    = getLineOfsMax( line, work->ofs_unit, work->rail_dat );
    width_max   = getLineWidthOfsMax( line, line_ofs, line_max, work->rail_dat );

    // ��
    if( !((-width_max <= width_ofs) && (width_ofs <= width_max)) )
    {
      return FALSE;
    }

    // ���C��
    if( !((line_ofs <= line_max)) )
    {
      return FALSE;
    }
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���ʒu���̐ݒ�
 *
 *	@param	man       �}�l�[�W��
 *	@param	location  �w��ʒu���
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_WORK_SetLocation(FIELD_RAIL_WORK * work, const RAIL_LOCATION * location)
{
  int i;
  const RAIL_LINE* line = NULL;
  int width_ofs_max;
  u32 line_ofs_max;
  int line_ofs;
  int width_ofs;
  int key;

  RAIL_LOCATION_Dump(location);
  // ������
  clearRail( work, work->rail_dat );

  GF_ASSERT( location->type < FIELD_RAIL_TYPE_MAX );
  
  if( location->type==FIELD_RAIL_TYPE_POINT )
  {
    // �_������
    work->type          = FIELD_RAIL_TYPE_POINT;
    work->point         = &work->rail_dat->point_table[ location->rail_index ];

    work->width_ofs_max = work->point->width_ofs_max[0];

    for( i=0; i<RAIL_CONNECT_LINE_MAX; i++ )
    {
      if( work->point->lines[i] != RAIL_TBL_NULL )
      {
        if( line == NULL )
        {
          line = &work->rail_dat->line_table[ work->point->lines[i] ];
          if( line->point_s == location->rail_index )
          {
            line_ofs  = 0;
            width_ofs = 0;
            key       = work->point->keys[i];
          }
          else if( line->point_e == location->rail_index )
          {
            line_ofs  = getLineOfsMax( line, work->ofs_unit, work->rail_dat );
            width_ofs = 0;
            key       = work->point->keys[i];
          }
        }
        else
        {
          // ������₪����ꍇ�ɂ́A�ŏ��Ɍ����������C�����g�p���܂��B
          OS_TPrintf( "work::SetLocation Point ���C������������܂��B\n" );
        }
      }
    }
    GF_ASSERT( line );
#ifdef PM_DEBUG
    if( !line )
    {
      // �|�C���g�ɍ~�藧�i�G�f�B�b�g�p�j
      work->type          = FIELD_RAIL_TYPE_POINT;
      work->point         = &work->rail_dat->point_table[ location->rail_index ];

      work->width_ofs_max = work->point->width_ofs_max[0];
      work->active = TRUE;
      return ;
    }
#endif
  }
  else
  {
    
    // ���C��������
    line        = &work->rail_dat->line_table[ location->rail_index ];
    line_ofs    = RAIL_GRID_TO_OFS(location->line_grid);
    width_ofs   = RAIL_GRID_TO_OFS(location->width_grid);
    key         = location->key;
  }

  line_ofs_max  = getLineOfsMax( line, work->ofs_unit, work->rail_dat );
  width_ofs_max = getLineWidthOfsMax( line, line_ofs, line_ofs_max, work->rail_dat );
  setLineData( work, line, key, line_ofs, width_ofs, line_ofs_max, width_ofs_max );

  TOMOYA_Printf( "key %d\n", key );

#ifdef PM_DEBUG
  
#if 0
  // ���C����line_ofs_max�ƕ��̍ő吔���擾����
  {
    int i;
    int attr_max;
    int width_s, width_e;
    attr_max = 0;
    for( i=0; i<work->rail_dat->line_count; i++ )
    {
      line_ofs_max = getLineOfsMax( &work->rail_dat->line_table[i], work->ofs_unit, work->rail_dat );

      width_s = getLineWidthOfsMax( &work->rail_dat->line_table[i], 0, line_ofs_max, work->rail_dat );
      width_e = getLineWidthOfsMax( &work->rail_dat->line_table[i], line_ofs_max, line_ofs_max, work->rail_dat );

      line_ofs_max /= RAIL_WALK_OFS;
      width_s      /= RAIL_WALK_OFS;
      width_e      /= RAIL_WALK_OFS;

      OS_TPrintf( "lineNo[%d] ofs_max[%d] width_s[%d] width_e[%d]\n", i, line_ofs_max, width_s, width_e );
      
      if( width_e > width_s )
      {
        width_s = width_e;  // width_s�̂ق����ő�ɂ��ċ��߂�
      }

      attr_max += ((width_s*2)+1) * line_ofs_max;
    }
    
    OS_TPrintf( "attr_max = [%d]\n", attr_max );
  }
#endif 

#endif
  
  // �����ݒ�
  work->last_frame = RAIL_FRAME_8;
  work->last_key = key;
  work->active = TRUE;


  // ���W�̏�����
  getRailPosition(work, &work->pos);
  // ���P�[�V�����̏�����
  work->now_location = *location;
  work->last_location = *location;
}

//------------------------------------------------------------------
/**
 * @brief ���݈ʒu�̎擾
 */
//------------------------------------------------------------------
void FIELD_RAIL_WORK_GetPos(const FIELD_RAIL_WORK * work, VecFx32 * pos)
{
  GF_ASSERT( work );
  *pos = work->pos;
}


//----------------------------------------------------------------------------
/**
 *	@brief	���[���V�X�e���@�ŐV�̓�����N���������̓���t���[��
 *
 *	@param	man		
 *
 *	@return	�L�[���
 */
//-----------------------------------------------------------------------------
RAIL_FRAME FIELD_RAIL_WORK_GetActionFrame( const FIELD_RAIL_WORK * work )
{
	GF_ASSERT( work );
	return work->last_frame;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[���V�X�e���@�Ō�Ɉړ������������擾
 *
 *	@param	man		
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
RAIL_KEY FIELD_RAIL_WORK_GetActionKey( const FIELD_RAIL_WORK * work )
{
	GF_ASSERT( work );
	return work->last_key;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������Ԃ��`�F�b�N
 *
 *	@param	work  ���[�N
 *
 *	@retval TRUE  ������
 *	@retval FALSE ��~���
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_WORK_IsAction( const FIELD_RAIL_WORK * work )
{
  GF_ASSERT( work );

  return work->req_move;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���V�X�e��  �ŐV��UPDATE�ł��ǂ������̂�
 *
 *	@param	work 
 *  
 *	@retval TRUE  �ړ�����
 *	@retval FALSE �ړ����Ȃ�����
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_WORK_IsLastAction( const FIELD_RAIL_WORK * work )
{
	GF_ASSERT( work );
	return work->last_move;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���������擾����
 *
 *	@param	work    ���[�N
 *	@param	way     �����i�[��
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_WORK_GetFrontWay( const FIELD_RAIL_WORK * work, VecFx16* way )
{
  FIELD_RAIL_WORK* local_work;
  GF_ASSERT( work );
  

  // �l�ύX�㌳�ɖ߂��̂ŁAconst�łn�j�Ƃ���
  local_work = (FIELD_RAIL_WORK*)work;

  // �O�̕��������߂�
  {
    VecFx32 pos;
    s32 line_ofs_tmp = local_work->line_ofs;
    if( local_work->line_ofs == 0 )
    {
      local_work->line_ofs += RAIL_WALK_OFS;
    }
    else
    {
      local_work->line_ofs -= RAIL_WALK_OFS;
    }

    // ���W�����߂�
    getRailPosition(local_work, &pos);

    // ���ɖ߂�
    local_work->line_ofs = line_ofs_tmp;

    VEC_Subtract( &pos, &local_work->pos, &pos );
    VEC_Normalize( &pos, &pos );
    VEC_Fx16Set( way, pos.x, pos.y, pos.z );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����̃��C���̑O�������֌������L�[���擾
 *
 *	@param	work  ���[�N
 *
 *	@return �L�[
 */
//-----------------------------------------------------------------------------
RAIL_KEY FIELD_RAIL_WORK_GetFrontKey( const FIELD_RAIL_WORK * work )
{
  GF_ASSERT( work );
  GF_ASSERT( work->type == FIELD_RAIL_TYPE_LINE );
  GF_ASSERT( work->line );
  return work->line->key;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�P�ʂ̓���ݒ�
 *
 *	@param	work
 *	@param	flag 
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_WORK_SetActiveFlag( FIELD_RAIL_WORK * work, BOOL flag )
{
  GF_ASSERT(work);
  work->active = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N����`�F�b�N
 *
 *	@param	work  ���[�N
 *
 *	@retval TRUE  ����
 *	@retval FALSE ���삵�ĂȂ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_WORK_IsActive( const FIELD_RAIL_WORK * work )
{
  GF_ASSERT(work);
  return work->active;
}




//----------------------------------------------------------------------------
/**
 *	@brief  1���ړ��̃��N�G�X�g
 *
 *	@param	work      ���[�N
 *	@param	frame     �ړ��t���[����
 *	@param	key       �ړ������L�[
 *
 *	@retval TRUE  ���N�G�X�g��
 *	@retval FALSE ���ݓ��쒆�Ȃ̂ŁA�󗝂ł��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_RAIL_WORK_ForwardReq( FIELD_RAIL_WORK * work, RAIL_FRAME frame, RAIL_KEY key )
{
  // ���삵�Ă��Ȃ��Ƃ�
  if( !work->req_move )
  {
    work->req_move = TRUE;
    work->ofs_move = 0;
    work->save_move_key = key;
    work->save_move_frame = frame;

    return TRUE;
  }

  return FALSE;
}


//------------------------------------------------------------------
/**
 * @brief ���݈ʒu�̃A�b�v�f�[�g
 */
//------------------------------------------------------------------
void FIELD_RAIL_WORK_Update(FIELD_RAIL_WORK * work)
{ 
  if( !work->cp_man->active_flag )
  {
    return ;
  }
  
  if (!work->active)
  {
    return;
  }

  if (isValidRail(work) == FALSE)
  {
    return;
  }
  
  work->last_move   = FALSE;
	
	if( work->req_move )
	{
		FIELD_RAIL_TYPE type = work->type;
		RAIL_KEY set_key = RAIL_KEY_NULL;
		RAIL_KEY key = work->save_move_key;
		u32 count_up = RAIL_COUNTUP[ work->save_move_frame ];

		work->ofs_move += count_up;
		

		if(work->type == FIELD_RAIL_TYPE_POINT)
		{ //POINT��̂Ƃ��̈ړ�����
			TOMOYA_Printf( "count_up point %d\n", count_up );
			set_key = updatePointMove(work, key, count_up);
		}
		else if(work->type == FIELD_RAIL_TYPE_LINE)
		{ //LINE��̂Ƃ��̈ړ�����
			set_key = updateLineMove_new(work, key, count_up);
		}

		if (set_key != RAIL_KEY_NULL)
    {
			DEBUG_RAIL_Printf("RAIL:%s :line_ofs=%d line_ofs_max=%d width_ofs=%d width_ofs_max=%d\n",
					debugGetRailKeyName(set_key), 
          RAIL_OFS_TO_GRID(work->line_ofs), RAIL_OFS_TO_GRID(work->line_ofs_max), 
          RAIL_OFS_TO_GRID(work->width_ofs), 
          (RAIL_OFS_TO_GRID(work->width_ofs_max)*2) );

      work->last_key    = set_key;
      work->last_frame  = work->save_move_frame;
      work->last_move   = TRUE;

      // ���W�̎擾
      getRailPosition(work, &work->pos);
		}

		if (type != work->type)
		{
			DEBUG_RAIL_Printf("RAIL:change to ");
			debugPrintRailInfo(work);
		}

    // ���N�G�X�g�ړ�����
		if( (work->ofs_move >= RAIL_WALK_OFS) || (set_key == RAIL_KEY_NULL) )
		{
			work->req_move = FALSE;


      // ���P�[�V�����̍X�V
      work->last_location = work->now_location;
      getRailLocation( work, &work->now_location );
		}
	}

}



//------------------------------------------------------------------
//  FIELD_RAIL_MAN�A�N�Z�X�֐�
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	�J�����|�C���^�擾
 */
//-----------------------------------------------------------------------------
FIELD_CAMERA* FIELD_RAIL_MAN_GetCamera( const FIELD_RAIL_MAN * man )
{
	GF_ASSERT( man );
	return  man->field_camera;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���̃��[�����[�N
 */
//-----------------------------------------------------------------------------
const FIELD_RAIL_WORK* FIELD_RAIL_MAN_GetBindWork( const FIELD_RAIL_MAN * man )
{
	GF_ASSERT( man );
	return man->camera_bind_work;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �|�C���g���̎擾
 */
//-----------------------------------------------------------------------------
const RAIL_POINT* FIELD_RAIL_MAN_GetRailDatPoint( const FIELD_RAIL_MAN * man, u32 index )
{
  GF_ASSERT( man );
  return getRailDatPoint( &man->rail_dat, index );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�������擾
 */
//-----------------------------------------------------------------------------
const RAIL_LINE* FIELD_RAIL_MAN_GetRailDatLine( const FIELD_RAIL_MAN * man, u32 index )
{
  GF_ASSERT( man );
  return getRailDatLine( &man->rail_dat, index );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������̎擾
 */
//-----------------------------------------------------------------------------
const RAIL_CAMERA_SET* FIELD_RAIL_MAN_GetRailDatCamera( const FIELD_RAIL_MAN * man, u32 index )
{
  GF_ASSERT( man );
  return getRailDatCamera( &man->rail_dat, index );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�W�V�����v�Z���̎擾
 */
//-----------------------------------------------------------------------------
const RAIL_LINEPOS_SET* FIELD_RAIL_MAN_GetRailDatLinePos( const FIELD_RAIL_MAN * man, u32 index )
{
  GF_ASSERT( man );
  return getRailDatLinePos( &man->rail_dat, index );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���j�b�g�T�C�Y�̎擾
 */
//-----------------------------------------------------------------------------
fx32 FIELD_RAIL_MAN_GetRailDatUnitSize( const FIELD_RAIL_MAN * man )
{
  GF_ASSERT( man );
  return man->rail_dat.ofs_unit;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �v�Z�p�@���[�����[�N���擾����
 *
 *	@param	man   �}�l�[�W��
 *
 *	@return ���[�����[�N
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_WORK* FIELD_RAIL_MAN_GetCalcRailWork( const FIELD_RAIL_MAN * man )
{
  GF_ASSERT( man );
  return man->calc_work;
}



//------------------------------------------------------------------
//  FIELD_RAIL_WORK�A�N�Z�X�֐�
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	���[���^�C�v�̎擾
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_TYPE FIELD_RAIL_GetType( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->type;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[���|�C���g�̎擾
 */
//-----------------------------------------------------------------------------
const RAIL_POINT* FIELD_RAIL_GetPoint( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_POINT );
	return work->point;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�����C���̎擾
 */
//-----------------------------------------------------------------------------
const RAIL_LINE* FIELD_RAIL_GetLine( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_LINE );
	return work->line;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C���I�t�Z�b�g�̎擾
 */
//-----------------------------------------------------------------------------
s32 FIELD_RAIL_GetLineOfs( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->line_ofs;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C���I�t�Z�b�g�ő�l�̎擾
 */
//-----------------------------------------------------------------------------
s32 FIELD_RAIL_GetLineOfsMax( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->line_ofs_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���I�t�Z�b�g�̎擾
 */
//-----------------------------------------------------------------------------
s32 FIELD_RAIL_GetWidthOfs( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->width_ofs;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���I�t�Z�b�g�ő�l�̎擾
 */
//-----------------------------------------------------------------------------
s32 FIELD_RAIL_GetWidthOfsMax( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->width_ofs_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ړ��P�ʂ̎擾
 */
//-----------------------------------------------------------------------------
fx32 FIELD_RAIL_GetOfsUnit( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return work->ofs_unit;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�����Z�b�g�̎擾
 */
//-----------------------------------------------------------------------------
const RAIL_CAMERA_SET* FIELD_RAIL_GetCameraSet( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT( work );
	return getCameraSet( work );
}




//------------------------------------------------------------------
//  RAIL_LINE�A�N�Z�X�֐�
//------------------------------------------------------------------
const RAIL_POINT* FIELD_RAIL_GetPointStart( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT(work);
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_LINE );
	return getRailDatPoint( work->rail_dat, work->line->point_s );
}
const RAIL_POINT* FIELD_RAIL_GetPointEnd( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT(work);
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_LINE );
	return getRailDatPoint( work->rail_dat, work->line->point_e );
}
const RAIL_LINEPOS_SET* FIELD_RAIL_GetLinePosSet( const FIELD_RAIL_WORK* work )
{
	GF_ASSERT(work);
	GF_ASSERT( work->type == FIELD_RAIL_TYPE_LINE );
	return getRailDatLinePos( work->rail_dat, work->line->line_pos_set );
}

//------------------------------------------------------------------
//  RAIL_POINT
//------------------------------------------------------------------
const RAIL_CAMERA_SET* FIELD_RAIL_POINT_GetCameraSet( const FIELD_RAIL_WORK* work, const RAIL_POINT* point )
{
	GF_ASSERT( work );
	GF_ASSERT( point );
	return getRailDatCamera( work->rail_dat, point->camera_set);
}

//------------------------------------------------------------------
// ����n�c�[��
//------------------------------------------------------------------
BOOL FIELD_RAIL_TOOL_HitCheckSphere( const VecFx32* person, const VecFx32* check, fx32 r )
{
  enum 
  { 
    HIT_HEIGHT  = FX32_ONE * 8,   // �c�̔���덷
  };
  VecFx32 pos1, pos2;
  // ���ʂ̋���
  fx32 len;
  fx32 dist_y;

  VEC_Set( &pos1, person->x, 0, person->z ); 
  VEC_Set( &pos2, check->x, 0, check->z ); 


  len = VEC_Distance( &pos1, &pos2 );
  dist_y = MATH_ABS( person->y - check->y );


  if( (dist_y < HIT_HEIGHT) && (len < FX_Mul( r, 2<<FX32_SHIFT ) ) )
  {
    return TRUE;
  }
  return FALSE;
}



#ifdef PM_DEBUG
// ���[���O���b�h�f�o�b�N�o��
void FIELD_RAIL_WORK_DEBUG_PrintRailGrid( const FIELD_RAIL_WORK * work )
{
  GF_ASSERT( work );

  if( work->type == FIELD_RAIL_TYPE_LINE )
  {
    s32 width_ofs_max_s, width_ofs_max_e;
    s32 width_ofs_max;

    width_ofs_max_s = getLineWidthOfsMax( &work->line[ work->dat_index ], 0, work->line_ofs_max, work->rail_dat );
    width_ofs_max_e = getLineWidthOfsMax( &work->line[ work->dat_index ], work->line_ofs_max, work->line_ofs_max, work->rail_dat );
    
    if( width_ofs_max_s >= width_ofs_max_e )
    {
      width_ofs_max = width_ofs_max_s;
    }
    else
    {
      width_ofs_max = width_ofs_max_e;
    }
    
    OS_TPrintf("RAIL:%s :front_grid=%d side_grid=%d \n",
        work->line[ work->dat_index ].name, 
        RAIL_OFS_TO_GRID(work->line_ofs), 
        RAIL_OFS_TO_GRID(work->width_ofs) + RAIL_OFS_TO_GRID(width_ofs_max) );
  }

}

#endif



//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static const RAIL_LINE * RAILPOINT_getLineByKey(const RAIL_POINT * point, RAIL_KEY key, const RAIL_SETTING* rail_dat)
{
  int i;
  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
    if (point->keys[i] == key)
    {
      return getRailDatLine( rail_dat, point->lines[i] );
    }
  }
  return NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY setLine(FIELD_RAIL_WORK * work, const RAIL_LINE * line, RAIL_KEY key, int l_ofs, int w_ofs, int l_ofs_max, int w_ofs_max)
{
  GF_ASSERT(work->line != line);
  GF_ASSERT(work->line);
  GF_ASSERT(line->name);
  if (work->type == FIELD_RAIL_TYPE_LINE)
  {
    TAMADA_Printf("RAIL: LINE \"%s\" to %s \"%s\"\n",
        work->line->name, debugGetRailKeyName(key), line->name);
  }
  else
  {
    TAMADA_Printf("RAIL: POINT \"%s\" to %s \"%s\"\n",
        work->point->name, debugGetRailKeyName(key), line->name);
  }

  setLineData( work, line, key, l_ofs, w_ofs, l_ofs_max, w_ofs_max );

  debugCheckLineData(line, work->rail_dat);
  return key;
}

// ���[���V�X�e���Ƀ��C���̏���ݒ肷��
static void setLineData(FIELD_RAIL_WORK * work, const RAIL_LINE * line, RAIL_KEY key, int l_ofs, int w_ofs, int l_ofs_max, int w_ofs_max)
{
  GF_ASSERT( work );
  GF_ASSERT( line );
  work->type = FIELD_RAIL_TYPE_LINE;
  work->line = line;
  work->key = key;
  work->line_ofs = l_ofs;
  work->width_ofs = w_ofs;
  work->line_ofs_max = l_ofs_max;
  work->width_ofs_max = w_ofs_max;
}

// ���C���̊J�n�|�C���g�ƁA������point����v���邩�`�F�b�N
static BOOL isLinePoint_S( const RAIL_SETTING * rail_dat, const RAIL_LINE * line, const RAIL_POINT * point )
{
	const RAIL_POINT * point_s;

	point_s = getRailDatPoint( rail_dat, line->point_s );

	if( point_s == point )
	{
		return TRUE;
	}
	return FALSE;
}

// ���C���̏I���|�C���g�ƁA������point����v���邩�`�F�b�N
static BOOL isLinePoint_E( const RAIL_SETTING * rail_dat, const RAIL_LINE * line, const RAIL_POINT * point )
{
	const RAIL_POINT * point_e;

	point_e = getRailDatPoint( rail_dat, line->point_e );

	if( point_e == point )
	{
		return TRUE;
	}
	return FALSE;
}



//------------------------------------------------------------------
//------------------------------------------------------------------
static void clearRailDat( RAIL_SETTING * raildat )
{
  GFL_STD_MemClear( raildat, sizeof(RAIL_SETTING) );
}
// ���[���f�[�^
static void initRailDat( RAIL_SETTING * raildat, const RAIL_SETTING * setting )
{
  GFL_STD_MemCopy( setting, raildat, sizeof(RAIL_SETTING) );
}
// �|�C���g���擾
static const RAIL_POINT* getRailDatPoint( const RAIL_SETTING * raildat, u32 index )
{
	if(raildat->point_count > index)
	{
		return &raildat->point_table[ index ];	
	}
	GF_ASSERT( RAIL_TBL_NULL==index );
	return NULL;
}
// ���C�����擾
static const RAIL_LINE* getRailDatLine( const RAIL_SETTING * raildat, u32 index )
{
	if(raildat->line_count > index)
	{
		return &raildat->line_table[ index ];
	}
	GF_ASSERT( RAIL_TBL_NULL==index );
	return NULL;
}
// �J�������擾
static const RAIL_CAMERA_SET* getRailDatCamera( const RAIL_SETTING * raildat, u32 index )
{
	if(raildat->camera_count > index)
	{
		return &raildat->camera_table[ index ];
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return &RAIL_CAMERA_SET_Default;
}
// ���C���ʒu�v�Z���擾
static const RAIL_LINEPOS_SET* getRailDatLinePos( const RAIL_SETTING * raildat, u32 index )
{
	if(raildat->linepos_count > index)
	{
		return &raildat->linepos_table[ index ];	
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return &RAIL_LINEPOS_SET_Default;
}
// �J�����ʒu�v�Z�֐��擾
static RAIL_CAMERA_FUNC*const getRailDatCameraFunc( const RAIL_SETTING * raildat, u32 index )
{
	if( raildat->camera_func_count > index )
	{
		return raildat->camera_func[ index ];
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return NULL;
}
// ���W�v�Z�֐��擾
static RAIL_POS_FUNC*const getRailDatLinePosFunc( const RAIL_SETTING * raildat, u32 index )
{
	if( raildat->linepos_func_count > index )
	{	
		return raildat->line_pos_func[ index ];
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return NULL;
}
// ���C�������擾�֐�
static RAIL_LINE_DIST_FUNC*const getRailDatLineDistFunc( const RAIL_SETTING * raildat, u32 index )
{
	if( raildat->line_dist_func_count > index )
	{	
		return raildat->line_dist_func[ index ];
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return NULL;
}
// ���C���ƍ��W�̓����蔻��
static RAIL_LINE_HIT_LOCATION_FUNC*const getRailDatLineHitLocationFunc( const RAIL_SETTING * raildat, u32 index )
{
	if( raildat->line_hit_location_count > index )
	{	
		return raildat->line_hit_location_func[ index ];
	}
	GF_ASSERT( index == RAIL_TBL_NULL );
	return NULL;
}



//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY updateLineMove_new(FIELD_RAIL_WORK * work, RAIL_KEY key, u32 count_up)
{ //LINE��̂Ƃ��̈ړ�����
  const RAIL_LINE * nLine = work->line;
  s32 nLine_ofs_max = getLineOfsMax( nLine, work->ofs_unit, work->rail_dat ); // ����LINE�̃I�t�Z�b�g�ő�l
  s32 ofs_max;  // �e���C���̃I�t�Z�b�g�ő�l
	s32 next_line_width_max;
	s32 now_line_width_max;

  if (key == nLine->key)
  {//�������L�[�̏ꍇ
    const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_e );
    const RAIL_LINE * front = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
    const RAIL_LINE * left = RAILPOINT_getLineByKey(nPoint, getAntiClockwiseKey(key), work->rail_dat);
    const RAIL_LINE * right = RAILPOINT_getLineByKey(nPoint, getClockwiseKey(key), work->rail_dat);
    BOOL width_over = FALSE;
    //const RAIL_LINE * back = RAILPOINT_getLineByKey(nPoint, getReverseKey(key), work->rail_dat);
    TAMADA_Printf("��");
    work->line_ofs += count_up;
    if (work->line_ofs <= nLine_ofs_max) {
      // ���̓����`�F�b�N �I�[�o�[���ĂȂ���΁A�ʏ�̍X�V
      now_line_width_max = getLineWidthOfsMax( nLine, work->line_ofs, nLine_ofs_max, work->rail_dat );
      if( now_line_width_max >= MATH_ABS(work->width_ofs) )
      {
        return key;
      }
      // �I�[�o�[���Ă�����A���̓��ɑJ�ڂ���
      width_over = TRUE;
		}
    if ((front!=NULL) && (width_over == FALSE))
    { //���ʈڍs����
      debugCheckPointData(nPoint, work->rail_dat);
			// ���`�F�b�N
			ofs_max = getLineOfsMax( front, work->ofs_unit, work->rail_dat );
			next_line_width_max = getLineWidthOfsMax( front, work->line_ofs - nLine_ofs_max, ofs_max, work->rail_dat );
			if( next_line_width_max >= MATH_ABS(work->width_ofs) )
			{
				return setLine(work, front, key,
						/*l_ofs*/work->line_ofs - nLine_ofs_max,
						/*w_ofs*/work->width_ofs,
						/*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max); //���̂܂�
			}
    }
    if ( (left && work->width_ofs < 0) )
    { //�����ڍs����
      if ( isLinePoint_S( work->rail_dat, left, nPoint ) )
      { //�n�[�̏ꍇ
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( left, -work->width_ofs, ofs_max, work->rail_dat );
        // �������C���͈͓̔����`�F�b�N
        if( (-work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs-nLine_ofs_max)) )
        {
          return setLine(work, left, key,
              /*l_ofs*/-work->width_ofs,
              /*w_ofs*/(work->line_ofs-nLine_ofs_max),
              /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max); 
        }
      }
      else if (isLinePoint_E( work->rail_dat, left, nPoint ))
      { //�I�[�̏ꍇ
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( left, ofs_max+work->width_ofs, ofs_max, work->rail_dat );
        // �������C���͈͓̔����`�F�b�N
        if( (-work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs-nLine_ofs_max)) )
        {
          return setLine(work, left, key,
              /*l_ofs*/ofs_max + work->width_ofs, //width_ofs < 0�Ȃ̂Ŏ������Z
              /*w_ofs*/-(work->line_ofs-nLine_ofs_max),
              /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max); 
        }
      }
      else GF_ASSERT_MSG(width_over, "%s����%s�ւ̐ڑ��ُ�\n", nLine->name, left->name);
    }
    if (right && work->width_ofs > 0)
    { //�E���ڍs����
      if ( isLinePoint_S( work->rail_dat, right, nPoint ) )
      { //�n�[�̏ꍇ
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( right, work->width_ofs, ofs_max, work->rail_dat );
        // �E�����C���͈͓̔����`�F�b�N
        if( (work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs-nLine_ofs_max)) )
        {
          return setLine(work, right, key,
              /*l_ofs*/work->width_ofs,
              /*w_ofs*/-(work->line_ofs-nLine_ofs_max),
              /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max); 
        }
      }
			else if ( isLinePoint_E( work->rail_dat, right, nPoint ) )
      { //�I�[�̏ꍇ
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( right, ofs_max - work->width_ofs, ofs_max, work->rail_dat );
        // �E�����C���͈͓̔����`�F�b�N
        if( (work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs-nLine_ofs_max)) )
        {
          return setLine(work, right, key,
              /*l_ofs*/ofs_max - work->width_ofs,
              /*w_ofs*/(work->line_ofs-nLine_ofs_max),
              /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        }
      }
      else GF_ASSERT_MSG(width_over, "%s����%s�ւ̐ڑ��ُ�\n", nLine->name, right->name);
    }

    if( width_over )
    {
			return updateLine( work, nLine, nLine_ofs_max, key );
    }

    //�s�����Ă��Ȃ��cLINE���̂܂܁A���Z���Ƃ肯��
    work->line_ofs = nLine_ofs_max;
    return RAIL_KEY_NULL;
  }
  else if (key == getReverseKey(nLine->key))
  {//�t�����L�[�̏ꍇ
    const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_s );
    //const RAIL_LINE * front = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
    const RAIL_LINE * left = RAILPOINT_getLineByKey(nPoint, getClockwiseKey(key), work->rail_dat);
    const RAIL_LINE * right = RAILPOINT_getLineByKey(nPoint, getAntiClockwiseKey(key), work->rail_dat);
    const RAIL_LINE * back = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
    BOOL width_over = FALSE;
    TAMADA_Printf("��");
    work->line_ofs -= count_up;
    if (work->line_ofs >= 0) {

      // ���̓����`�F�b�N �I�[�o�[���ĂȂ���΁A�ʏ�̍X�V
      now_line_width_max = getLineWidthOfsMax( nLine, work->line_ofs, nLine_ofs_max, work->rail_dat );
      if( now_line_width_max >= MATH_ABS(work->width_ofs) )
      {
        return key;
      }
      // �I�[�o�[���Ă�����A���̓��ɑJ�ڂ���
      width_over = TRUE;
		}
    if ((back != NULL) && (width_over == FALSE))
    {//�w�ʈڍs����
      debugCheckPointData(nPoint, work->rail_dat);
      ofs_max = getLineOfsMax( back, work->ofs_unit, work->rail_dat );
			// ���`�F�b�N
			next_line_width_max = getLineWidthOfsMax( back, ofs_max - MATH_ABS(work->line_ofs), ofs_max, work->rail_dat );
			if( next_line_width_max >= MATH_ABS(work->width_ofs) )
			{
	      return setLine(work, back, key,
		        /*l_ofs*/ofs_max - MATH_ABS(work->line_ofs),	// �����ɂ́A�K�����̒l������
			      /*w_ofs*/work->width_ofs,
				    /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
			}
    }
    if (left && work->width_ofs < 0)
    { //�����ڍs����
			if ( isLinePoint_S( work->rail_dat, left, nPoint ) )
      { //�n�[�̏ꍇ
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( left, -work->width_ofs, ofs_max, work->rail_dat );
        // �������C���͈͓̔����`�F�b�N
        if( (-work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs)) )
        {
          return setLine(work, left, key,
              /*l_ofs*/-work->width_ofs,
              /*w_ofs*/work->line_ofs,
            /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        }
      }
			else if ( isLinePoint_E( work->rail_dat, left, nPoint ) )
      { //�I�[�̏ꍇ
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( left, ofs_max+work->width_ofs, ofs_max, work->rail_dat );
        // �������C���͈͓̔����`�F�b�N
        if( (-work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs)) )
        {
          return setLine(work, left, key,
              /*l_ofs*/ofs_max + work->width_ofs, //width_ofs < 0�Ȃ̂Ŏ������Z
              /*w_ofs*/-work->line_ofs,
            /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        }
      }
      else GF_ASSERT_MSG(width_over, "%s����%s�ւ̐ڑ��ُ�\n", nLine->name, left->name);
    }
    if (right && work->width_ofs > 0)
    { //�E���ڍs����
			if ( isLinePoint_S( work->rail_dat, right, nPoint ) )
      { //�n�[�̏ꍇ
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( right, work->width_ofs, ofs_max, work->rail_dat );
        // �E�����C���͈͓̔����`�F�b�N
        if( (work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs)) )
        {
          return setLine(work, right, key,
              /*l_ofs*/work->width_ofs,
              /*w_ofs*/-work->line_ofs,
            /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        } 
      }
			if ( isLinePoint_E( work->rail_dat, right, nPoint ) )
      { //�I�[�̏ꍇ
        debugCheckPointData(nPoint, work->rail_dat);
        ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
			  next_line_width_max = getLineWidthOfsMax( right, ofs_max - work->width_ofs, ofs_max, work->rail_dat );
        // �E�����C���͈͓̔����`�F�b�N
        if( (work->width_ofs <= ofs_max) && (next_line_width_max >= MATH_ABS(work->line_ofs)) )
        {
          return setLine(work, right, key,
              /*l_ofs*/ofs_max - work->width_ofs,
              /*w_ofs*/work->line_ofs,
            /*ofs_max*/ofs_max,
            /*w_ofs_max*/next_line_width_max);
        }
      }
      else GF_ASSERT_MSG(width_over, "%s����%s�ւ̐ڑ��ُ�\n", nLine->name, right->name);
    }

    //  ���I�[�o�[����
    if( width_over )
    {
			return updateLine( work, nLine, nLine_ofs_max, key );
    }
    //�s�����Ă��Ȃ��cLINE���̂܂܁A���Z��������
    work->line_ofs = 0;
    return RAIL_KEY_NULL;
  }
  else if (key == getClockwiseKey(nLine->key))
  {//���v���ו����L�[�̏ꍇ
    TAMADA_Printf("��");
    work->width_ofs += count_up;
    if (work->width_ofs <= work->width_ofs_max)
    {//�͈͓��̏ꍇ�A�I��
			return updateLine( work, nLine, nLine_ofs_max, key );
    }
		{
			const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_s );
			const RAIL_LINE *right = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
			if(right)
			{
				if ( isLinePoint_S( work->rail_dat, right, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( right, work->width_ofs, ofs_max, work->rail_dat );
					//�E��LINE�͎n�[����̏ꍇ
					if(work->line_ofs <= next_line_width_max)
					{
						return setLine(work, right, key,
								work->width_ofs,
								- work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
				if ( isLinePoint_E( work->rail_dat, right, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( right, ofs_max - work->width_ofs, ofs_max, work->rail_dat );
					//�E��LINE�͎n�[����̏ꍇ
					if(work->line_ofs <= next_line_width_max)
					{
						return setLine(work, right, key,
								ofs_max - work->width_ofs,
								work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
			}
		}
		{
			const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_e );
			const RAIL_LINE *right = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
			if(right)
			{
				if ( isLinePoint_S( work->rail_dat, right, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( right, work->width_ofs, ofs_max, work->rail_dat );
					//�E��LINE�͏I�[����̏ꍇ
					if( work->line_ofs >= (nLine_ofs_max - next_line_width_max) )
					{
						return setLine(work, right, key,
								work->width_ofs,
								nLine_ofs_max - work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
				if ( isLinePoint_E( work->rail_dat, right, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( right, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( right, ofs_max - work->width_ofs, ofs_max, work->rail_dat );
					//�E��LINE�͏I�[����̏ꍇ
					if( work->line_ofs >= (nLine_ofs_max - next_line_width_max) )
					{
						return setLine(work, right, key,
								ofs_max - work->width_ofs,
								- (nLine_ofs_max - work->line_ofs),
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}	
				}
			}
    }
    //���Ԓn�_�̏ꍇ�A�s�����Ă��Ȃ��ꍇ�cLINE���̂܂܁A���Z��������
    work->width_ofs = work->width_ofs_max;
    return RAIL_KEY_NULL;
  }
  else if (key == getAntiClockwiseKey(nLine->key))
  {//�����v���ו����L�[�̏ꍇ
    TAMADA_Printf("��");
    work->width_ofs -= count_up;
    if (MATH_ABS(work->width_ofs) <= work->width_ofs_max)
    {//�͈͓��̏ꍇ�A�I��
			return updateLine( work, nLine, nLine_ofs_max, key );
    }
		{
      const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_s );
      const RAIL_LINE* left = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
			if(left)
			{
				if ( isLinePoint_S( work->rail_dat, left, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( left, MATH_ABS(work->width_ofs), ofs_max, work->rail_dat );
					//����LINE�͎n�[����̏ꍇ
					if(work->line_ofs <= next_line_width_max)
					{
						return setLine(work, left, key,
								MATH_ABS(work->width_ofs), 
								work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
				if ( isLinePoint_E( work->rail_dat, left, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( left, ofs_max - MATH_ABS(work->width_ofs), ofs_max, work->rail_dat );
					//����LINE�͎n�[����̏ꍇ
					if(work->line_ofs <= next_line_width_max)
					{
						return setLine(work, left, key,
								ofs_max - MATH_ABS(work->width_ofs),
								- work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
			}
		}
		{
      const RAIL_POINT* nPoint = getRailDatPoint( work->rail_dat, nLine->point_e );
      const RAIL_LINE* left = RAILPOINT_getLineByKey(nPoint, key, work->rail_dat);
			if(left)
			{
				if ( isLinePoint_S( work->rail_dat, left, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( left, MATH_ABS(work->width_ofs), ofs_max, work->rail_dat );
					//����LINE�͏I�[����̏ꍇ
					if( work->line_ofs >= (nLine_ofs_max - next_line_width_max) )
					{
						return setLine(work, left, key,
								MATH_ABS(work->width_ofs),
								- (nLine_ofs_max - work->line_ofs),
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}
				}
				if ( isLinePoint_E( work->rail_dat, left, nPoint ) )
				{
					debugCheckPointData(nPoint, work->rail_dat);
					ofs_max = getLineOfsMax( left, work->ofs_unit, work->rail_dat );
					next_line_width_max = getLineWidthOfsMax( left, ofs_max - MATH_ABS(work->width_ofs), ofs_max, work->rail_dat );
					//����LINE�͏I�[����̏ꍇ
					if( work->line_ofs >= (nLine_ofs_max - next_line_width_max) )
					{
						return setLine(work, left, key,
								ofs_max - MATH_ABS(work->width_ofs),
								nLine_ofs_max - work->line_ofs,
								ofs_max,
            /*w_ofs_max*/next_line_width_max);
					}	
				}
			}
		}
    //���Ԓn�_�̏ꍇ�A�s�����Ă��Ȃ��ꍇ�cLINE���̂܂܁A���Z��������
    work->width_ofs += count_up;
    return RAIL_KEY_NULL;
  }
  return RAIL_KEY_NULL;
}

#if 0
//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY updateLineMove(FIELD_RAIL_WORK * work, RAIL_KEY key)
{ //LINE��̂Ƃ��̈ړ�����
  const RAIL_LINE * nLine = work->line;
  if (key == nLine->key)
  {//�������L�[�̏ꍇ
    work->line_ofs ++;
    if (work->line_ofs == work->line_ofs_max)
    { // LINE --> point_e �ւ̈ڍs����
      work->type = FIELD_RAIL_TYPE_POINT;
      work->point = nLine->point_e;
      debugCheckPointData(work->point);
    }
    return key;
  }
  else if (key == getReverseKey(nLine->key))
  {//�t�����L�[�̏ꍇ
    work->line_ofs --;
    if (work->line_ofs == 0)
    { // LINE --> POINT_S �ւ̈ڍs����
      work->type = FIELD_RAIL_TYPE_POINT;
      work->point = nLine->point_s;
      debugCheckPointData(work->point);
    }
    return key;
  }
  else if (key == getClockwiseKey(nLine->key))
  {//���v���ƂȂ�����L�[�̏ꍇ
    if (work->width_ofs < work->width_ofs_max)
    {
      work->width_ofs ++;
    }
    return key;
  }
  else if (key == getAntiClockwiseKey(nLine->key))
  {//�����v���ƂȂ�����L�[�̏ꍇ
    if (work->width_ofs > - work->width_ofs_max)
    {
      work->width_ofs --;
    }
    return key;
  }
  return RAIL_KEY_NULL;
}
#endif

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY updatePointMove(FIELD_RAIL_WORK * work, RAIL_KEY key, u32 count_up)
{//POINT��̂Ƃ��̈ړ�����
  int i;
  const RAIL_POINT * nPoint = work->point;
  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
    if (nPoint->keys[i] == key)
    { // POINT --> LINE[i] �ւ̈ڍs����
      const RAIL_LINE * nLine = getRailDatLine( work->rail_dat, nPoint->lines[i] );
      s32 ofs_max;

      // 
      ofs_max = getLineOfsMax( nLine, work->ofs_unit, work->rail_dat );
      
			if ( isLinePoint_S( work->rail_dat, nLine, nPoint ) )
      { 
        setLine(work, nLine, key, count_up, 0, ofs_max, nPoint->width_ofs_max[i]);
        //work->line_ofs = 1;  //�ŏ��͂P����I
      }
			else if ( isLinePoint_E( work->rail_dat, nLine, nPoint ) )
      {
        setLine(work, nLine, key, ofs_max - count_up, 0, ofs_max, nPoint->width_ofs_max[i]);
        //work->line_ofs = nLine->ofs_max - 1; //�Ō�[1����I
      }
      //�Ȃ�����LINE������RAIL�Ƃ��ăZ�b�g
      //work->type = FIELD_RAIL_TYPE_LINE;
      //work->line = nLine;
      //debugCheckLineData(work->line, work->rail_dat);
      return key;
    }
  }
  return RAIL_KEY_NULL;
}



//============================================================================================
/**
 */
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void initRail(FIELD_RAIL_WORK * work, const RAIL_SETTING* rail_dat, const FIELD_RAIL_MAN* cp_man )
{
  work->cp_man = cp_man;
  
  work->type = FIELD_RAIL_TYPE_MAX;
  work->dummy = NULL;
  work->line_ofs = 0;
  work->width_ofs = 0;
  work->key = RAIL_KEY_NULL;
	work->rail_dat = rail_dat;
  work->ofs_unit = rail_dat->ofs_unit;


  work->last_frame = 0;
  work->last_key = RAIL_KEY_NULL;

  work->req_move = FALSE;

  work->active = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[����Ԃ̃N���A
 *
 *	@param	work
 *	@param	rail_dat 
 */
//-----------------------------------------------------------------------------
static void clearRail( FIELD_RAIL_WORK * work, const RAIL_SETTING* rail_dat )
{
  initRail( work, rail_dat, work->cp_man );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL isValidRail(const FIELD_RAIL_WORK * work)
{
  if (work->type < FIELD_RAIL_TYPE_MAX && work->dummy != NULL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static const RAIL_CAMERA_SET * getCameraSet(const FIELD_RAIL_WORK * work)
{
  switch (work->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    return getRailDatCamera( work->rail_dat, work->point->camera_set );
    break;
  case FIELD_RAIL_TYPE_LINE:
    return getRailDatCamera( work->rail_dat, work->line->camera_set);
    break;
  }
  GF_ASSERT(0);
  return &RAIL_CAMERA_SET_Default;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void getRailPosition(const FIELD_RAIL_WORK * work, VecFx32 * pos)
{
  RAIL_POS_FUNC * func = NULL;
	const RAIL_LINEPOS_SET* line_pos_set;

  if(work->type == FIELD_RAIL_TYPE_POINT)
  {
    //POINT�ɂ���Ƃ���POINT�̍��W
    *pos = work->point->pos;
  }
  else if(work->type == FIELD_RAIL_TYPE_LINE)
  {
		line_pos_set = getRailDatLinePos( work->rail_dat, work->line->line_pos_set );
    func = getRailDatLinePosFunc( work->rail_dat, line_pos_set->func_index );
    if (func)
    {
			func(work, pos);
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����P�[�V�����̎擾
 *
 *	@param	work      ���[�N
 *	@param	location  ���P�[�V�����i�[��
 */
//-----------------------------------------------------------------------------
static void getRailLocation( const FIELD_RAIL_WORK * work, RAIL_LOCATION * location )
{
  location->type        = work->type;

  // �C���f�b�N�X�擾
  if( work->type==FIELD_RAIL_TYPE_POINT ){
    location->rail_index  = getPointIndex( work->rail_dat, work->point );
  }else{
    location->rail_index  = getLineIndex( work->rail_dat, work->line );
  }

  // �e��I�t�Z�b�g
  location->line_grid  = RAIL_OFS_TO_GRID(work->line_ofs);
  location->width_grid = RAIL_OFS_TO_GRID(work->width_ofs);
  location->key       = work->key;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���C���̃I�t�Z�b�g�ő吔�����߂�
 *
 *	@param	line    ���C�����
 *	@param	unit    �P�I�t�Z�b�g�̋���
 *
 *	@return �I�t�Z�b�g�ő吔
 */
//-----------------------------------------------------------------------------
static s32 getLineOfsMax( const RAIL_LINE * line, fx32 unit, const RAIL_SETTING* rail_dat )
{
  const RAIL_LINEPOS_SET * line_pos_set;
	const RAIL_POINT* point_s;
	const RAIL_POINT* point_e;
	RAIL_LINE_DIST_FUNC* dist_func;
  fx32 dist;
  s32 div;
  s32 amari;

	point_s = getRailDatPoint( rail_dat, line->point_s );
	point_e = getRailDatPoint( rail_dat, line->point_e );

  // ���C����������擾
  line_pos_set = getRailDatLinePos( rail_dat, line->line_pos_set );

	// �����̎擾
	dist_func = getRailDatLineDistFunc( rail_dat, line_pos_set->func_dist_index );
	if( dist_func )
	{
		dist = dist_func( point_s, point_e, line_pos_set );
		div   = FX_Div( dist, unit ) >> FX32_SHIFT; // �����_�͖���
	}
	else
	{
		div = 0;
	}

#if 0
  // RAIL_WALK_OFS�Ŋ���؂��l�ɂ���
  amari = div % RAIL_WALK_OFS;
  if( amari >=  (RAIL_WALK_OFS/2) )
  {
    div += RAIL_WALK_OFS - amari;
    TOMOYA_Printf( "���C���I�t�Z�b�g�@���܂�@������܂��B amari=%d\n", amari );
  }
  else if( amari )
  {
    div -= amari;
    TOMOYA_Printf( "���C���I�t�Z�b�g�@���܂�@������܂��B amari=%d\n", amari );
  }
#else

  amari = div % RAIL_WALK_OFS;
  if(amari)
  {
    div -= amari;
    TOMOYA_Printf( "���C���I�t�Z�b�g�@���܂�@������܂��B amari=%d\n", amari );
  }

#endif

//  OS_TPrintf( "div = %d dist = 0x%x unit = 0x%x\n", div, dist, unit );

  return div;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C�����I�t�Z�b�g�ő�l�����߂�
 *
 *	@param	line
 *	@param	line_ofs		���C���@�I�t�Z�b�g�l�i�i�݋�j
 *	@param	rail_dat 
 *
 *	@return	���I�t�Z�b�g�ő�l
 */
//-----------------------------------------------------------------------------
static s32 getLineWidthOfsMax( const RAIL_LINE * line, u32 line_ofs, u32 line_ofs_max, const RAIL_SETTING* rail_dat )
{
	s32 width_ofs_max;
	s32 width_ofs_div;
	const RAIL_POINT* cp_point_s;
	const RAIL_POINT* cp_point_e;
	int i, j;
	int width_s, width_e;

	// ���C���̎n�_�ƏI�_���擾
	GF_ASSERT( line->point_s != RAIL_TBL_NULL );
	cp_point_s = &rail_dat->point_table[ line->point_s ];
	GF_ASSERT( line->point_e != RAIL_TBL_NULL );
	cp_point_e = &rail_dat->point_table[ line->point_e ];

	// ��v���郉�C���̕����擾
	width_e = -1;
	width_s = -1;
	for( i=0; i<RAIL_CONNECT_LINE_MAX; i++ )
	{
		if( cp_point_s->lines[i] != RAIL_TBL_NULL )
		{
			if( (u32)(&rail_dat->line_table[cp_point_s->lines[i]]) == (u32)(line) )
			{
				width_s = cp_point_s->width_ofs_max[i];
			}
		}

		if( cp_point_e->lines[i] != RAIL_TBL_NULL )
		{
			if( (u32)(&rail_dat->line_table[cp_point_e->lines[i]]) == (u32)(line) )
			{
				width_e = cp_point_e->width_ofs_max[i];
			}
		}
	}
	if( width_s < 0 )
	{
		width_s = width_e;
	}
	if( width_e < 0 )
	{
		width_e = width_s;
	}
	GF_ASSERT( width_s >= 0 );

	width_ofs_div = width_e - width_s;
	width_ofs_max = (width_ofs_div * (s32)line_ofs) / (s32)line_ofs_max;
	width_ofs_max += width_s;
	

	return width_ofs_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C������@�ʏ�X�V�i���`�F�b�N������j
 *
 *	@param	work					���[���V�X�e��
 *	@param	line					�i�s���̃��C��
 *	@param	line_ofs_max	���C���I�t�Z�b�g�ő�l
 *	@param	key						������Ă���L�[
 *
 *	@return	����ύX�����L�[
 */
//-----------------------------------------------------------------------------
static RAIL_KEY updateLine( FIELD_RAIL_WORK * work, const RAIL_LINE * line, u32 line_ofs_max, u32 key )
{
  s32 width_ofs_max = getLineWidthOfsMax( line, work->line_ofs, line_ofs_max, work->rail_dat ); // ����LINE�̃I�t�Z�b�g�ő�l

	work->width_ofs_max = width_ofs_max;
	if( MATH_ABS(work->width_ofs) >= width_ofs_max )
	{
		if(work->width_ofs > 0)
		{
			work->width_ofs = width_ofs_max;
		}
		else
		{
			work->width_ofs = -width_ofs_max;
		}
	}

	return key;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static int RailKeyTokeyCont(RAIL_KEY key)
{
  static const u16 sc_KEYCONT[ RAIL_KEY_MAX ] = 
  {
    0,
    PAD_KEY_UP,
    PAD_KEY_RIGHT,
    PAD_KEY_DOWN,
    PAD_KEY_LEFT,
  };

  return sc_KEYCONT[ key ];
}

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
static RAIL_KEY keyContToRailKey(int cont)
{
  if (cont & PAD_KEY_UP) return RAIL_KEY_UP;
  if (cont & PAD_KEY_LEFT) return RAIL_KEY_LEFT;
  if (cont & PAD_KEY_RIGHT) return RAIL_KEY_RIGHT;
  if (cont & PAD_KEY_DOWN) return RAIL_KEY_DOWN;
  return RAIL_KEY_NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY getReverseKey(RAIL_KEY key)
{
  switch (key) {
  case RAIL_KEY_UP: 
    return RAIL_KEY_DOWN;
  case RAIL_KEY_DOWN:
    return RAIL_KEY_UP;
  case RAIL_KEY_LEFT:
    return RAIL_KEY_RIGHT;
  case RAIL_KEY_RIGHT:
    return RAIL_KEY_LEFT;
  }
  return RAIL_KEY_NULL;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY getClockwiseKey(RAIL_KEY key)
{
  switch (key) {
  case RAIL_KEY_UP: 
    return RAIL_KEY_RIGHT;
  case RAIL_KEY_DOWN:
    return RAIL_KEY_LEFT;
  case RAIL_KEY_LEFT:
    return RAIL_KEY_UP;
  case RAIL_KEY_RIGHT:
    return RAIL_KEY_DOWN;
  }
  return RAIL_KEY_NULL;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY getAntiClockwiseKey(RAIL_KEY key)
{
  switch (key) {
  case RAIL_KEY_UP: 
    return RAIL_KEY_LEFT;
  case RAIL_KEY_DOWN:
    return RAIL_KEY_RIGHT;
  case RAIL_KEY_LEFT:
    return RAIL_KEY_DOWN;
  case RAIL_KEY_RIGHT:
    return RAIL_KEY_UP;
  }
  return RAIL_KEY_NULL;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static const char * getRailName(const FIELD_RAIL_WORK * work)
{
  switch (work->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    return work->point->name;
  case FIELD_RAIL_TYPE_LINE:
    return work->line->name;
  }
  return "NO ENTRY";
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY searchLineInPoint(const RAIL_POINT *point, const RAIL_LINE * line, const RAIL_SETTING* rail_dat)
{
  int i;
	const RAIL_LINE* point_line;
  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
		point_line = getRailDatLine( rail_dat, point->lines[i] );
    if (point_line == line) return point->keys[i];
  }
  return RAIL_KEY_NULL;
}

//============================================================================================
//
//
//    �f�o�b�O�p�֐�
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL debugCheckLineData(const RAIL_LINE * line, const RAIL_SETTING* rail_dat)
{

  RAIL_KEY key;
	const RAIL_POINT* point_s;
	const RAIL_POINT* point_e;

  if(line->point_s == RAIL_TBL_NULL || line->point_e == RAIL_TBL_NULL)
  {
    OS_TPrintf("%s��POINT������������܂���\n", line->name);
    return FALSE;
  }
	point_s = getRailDatPoint( rail_dat, line->point_s );
	point_e = getRailDatPoint( rail_dat, line->point_e );
	
  key = searchLineInPoint(point_s, line, rail_dat);
  if (key == RAIL_KEY_NULL)
  {
    OS_TPrintf("%s��%s�Ɛ������ڑ��ł��Ă��܂���\n", line->name, point_s->name);
    return FALSE;
  }
  if (key != line->key)
  {
    OS_TPrintf("%s�͎n�[%s�ƃL�[�������Ă��܂���\n", line->name, point_s->name);
    return FALSE;
  }
  key = searchLineInPoint(point_e, line, rail_dat);
  if (key == RAIL_KEY_NULL)
  {
    OS_TPrintf("%s��%s�Ɛ������ڑ��ł��Ă��܂���\n", line->name, point_e->name);
    return FALSE;
  }
  if (key != getReverseKey(line->key))
  {
    OS_TPrintf("%s�͏I�[%s�ƃL�[�������Ă��܂���\n", line->name, point_e->name);
    return FALSE;
  }
  return TRUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL debugCheckPointData(const RAIL_POINT * point, const RAIL_SETTING* rail_dat)
{
  int i, j;
	const RAIL_LINE* line;
	const RAIL_LINE* line_sarch;
	const RAIL_POINT* point_s;
	const RAIL_POINT* point_e;

  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
    if ((point->keys[i] == RAIL_KEY_NULL) != (point->lines[i] == RAIL_TBL_NULL) )
    {
      OS_TPrintf("%s:LINE��RAIL_KEY�̎w��ɖ���������܂�\n", point->name);
      return FALSE;
    }

    if (point->lines[i] == RAIL_TBL_NULL) continue;

		// ���C�����擾
		line = getRailDatLine( rail_dat, point->lines[i] );
		point_s = getRailDatPoint( rail_dat, line->point_s );
		point_e = getRailDatPoint( rail_dat, line->point_e );
		
    if (point_s != point && point_e != point)
    {
      OS_TPrintf("%s��%s�Ɛڑ����Ă��܂���\n",point->name, line->name);
      return FALSE;
    }

#if 0 // line��񂪁A��Ɉ������̃L�[�Ő�������Ă���K�v�͂Ȃ��H
    if (point->keys[i] != RAIL_KEY_NULL)
    {
      for (j = i+1; j < RAIL_CONNECT_LINE_MAX; j++)
      {
        if (point->keys[i] == getReverseKey(point->keys[j]))
        {
					line_sarch = getRailDatLine( rail_dat, point->lines[j] );
          if (line->key != line_sarch->key)
          {
            OS_Printf("%s ���� %s �o�R�� %s �֌������L�[�ɖ���������܂�\n",
                line->name, point->name, line_sarch->name);
            return FALSE;
          }
        }
      }
    }
#endif
  }
  return TRUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintWholeVector(const char * before, const VecFx32 * vec, const char * after)
{
  if (before)
  {
    DEBUG_RAIL_Printf("%s",before);
  }
  DEBUG_RAIL_Printf("( %d, %d, %d)", FX_Whole(vec->x), FX_Whole(vec->y), FX_Whole(vec->z));
  if (after)
  {
    DEBUG_RAIL_Printf("%s",after);
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintHexVector(const VecFx32 * vec)
{
  DEBUG_RAIL_Printf("(%08x, %08x, %08x)\n", (vec->x), (vec->y), (vec->z));
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintPoint(const char * before, const RAIL_POINT * point, const char * after)
{
  if (point != NULL)
  {
    debugPrintWholeVector(before, &point->pos, after);
  }
  else
  {
    if (before) DEBUG_RAIL_Printf("%s", before);
    DEBUG_RAIL_Printf("( NULL )");
    if (after) DEBUG_RAIL_Printf("%s", after);
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintDegree(const VecFx32 * p0, const VecFx32 * p1)
{
    VecFx32 s,e;
    fx32 angle;
    VEC_Normalize( p0, &s );
    VEC_Normalize( p1, &e );
    angle = FX_AcosIdx( VEC_DotProduct( &s, &e ) );
    DEBUG_RAIL_Printf("Degree: %08x\n",angle);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static const char * debugGetRailKeyName(RAIL_KEY key)
{
    static const char * const names[] = {
      "RAIL_KEY_NULL","RAIL_KEY_UP","RAIL_KEY_RIGHT","RAIL_KEY_DOWN","RAIL_KEY_LEFT"
    };
    GF_ASSERT(key < RAIL_KEY_MAX);
    return names[key];
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintRailInfo(const FIELD_RAIL_WORK * work)
{
  FIELD_RAIL_TYPE type = work->type;
  DEBUG_RAIL_Printf("%d(%s:%08x)\n",
      type,
      getRailName(work),
      work->dummy);
  if (type == FIELD_RAIL_TYPE_POINT)
  {
    debugPrintPoint(NULL, work->point ,"\n");
  } else if (type == FIELD_RAIL_TYPE_LINE)
  {
		const RAIL_POINT* point_s;
		const RAIL_POINT* point_e;

		point_s = getRailDatPoint( work->rail_dat, work->line->point_s );
		point_e = getRailDatPoint( work->rail_dat, work->line->point_e );
    debugPrintPoint("start", point_s, NULL);
    debugPrintPoint("-end", point_e, "\n");
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  �|�C���g�̃e�[�u���C���f�b�N�X���擾
 *
 *	@param	man       �}�l�[�W��
 *	@param	point     �|�C���g
 *
 *	@return �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u32 getPointIndex( const RAIL_SETTING * raildat, const RAIL_POINT* point )
{
  int i;

  for( i=0; i<raildat->point_count; i++ ){
    if( (u32)&raildat->point_table[i] == (u32)point ){
      return i;
    }
  }

  GF_ASSERT_MSG( i<raildat->point_count, "�s����point�ł��B" );
  return 0; // �t���[�Y���
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C���̃e�[�u���C���f�b�N�X���擾
 *
 *	@param	man       �}�l�[�W��
 *	@param	line      ���C��
 *
 *	@return �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u32 getLineIndex( const RAIL_SETTING * raildat, const RAIL_LINE* line )
{
  int i;

  for( i=0; i<raildat->line_count; i++ ){
    if( (u32)&raildat->line_table[i] == (u32)line ){
      return i;
    }
  }

  GF_ASSERT_MSG( i<raildat->line_count, "�s����line�ł��B" );
  return 0; // �t���[�Y���
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���ʂƒ����̌�_�����߂�
 *
 *	@param	plane_vec1    ���ʃx�N�g���P
 *	@param	plane_vec2    ���ʃx�N�g���Q
 *	@param  plane_pos     ���ʂ̈�_
 *	@param	vec_s         �����n�_
 *	@param	vec_e         �����I�_
 *	@param	cross         ��_�i�[��
 *
 *	@retval TRUE  ��_������
 *	@retval FALSE ��_�͂Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL calcHitPlaneVec( const VecFx32* plane_vec1, const VecFx32* plane_vec2, const VecFx32* plane_pos, const VecFx32* vec_s, const VecFx32* vec_e, VecFx32* cross )
{
  VecFx32 normal;
  VecFx32 vector;
  VecFx32 plane_nvec1, plane_nvec2;
  fx32 d;
  fx32 cross_dist;
  fx32 de;
  fx32 vector_dist;


  DEBUG_RAIL_Printf( "\ncalcHitPlaneVec\n" );

  // �x�N�g�������߂�
  VEC_Subtract( vec_e, vec_s, &vector );
  vector_dist = VEC_Mag( &vector );
  VEC_Normalize( &vector, &vector );
  
  // ���ʂ̕����������߂�B
  // �@�������߁A�c�̒l�����߂�
  VEC_Normalize( plane_vec1, &plane_nvec1);  // �傫���͂���Ȃ�
  VEC_Normalize( plane_vec2, &plane_nvec2);  // �傫���͂���Ȃ�
  VEC_CrossProduct( &plane_nvec1, &plane_nvec2, &normal );
  VEC_Normalize( &normal, &normal);  // �傫���͂���Ȃ�
  d = FX_Mul( normal.x, plane_pos->x ) + FX_Mul( normal.y, plane_pos->y ) + FX_Mul( normal.z, plane_pos->z );
  
  DEBUG_RAIL_Printf( "normal x[%d] y[%d] z[%d]\n", FX_Whole( normal.x ), FX_Whole( normal.y ), FX_Whole( normal.z ) );

  // ��_�����߂�B
  // P(��_) = A(�n�_) + (dist * Vec)
  // dist = {d - (n_a*Ax + n_b*Ay + n_c*Az)} / (n_a*Vecx + n_b*Vecy + n_c*Vecz)
  cross_dist = d - (FX_Mul( normal.x, vec_s->x ) + FX_Mul( normal.y, vec_s->y ) + FX_Mul( normal.z, vec_s->z ));
  de = (FX_Mul( normal.x, vector.x ) + FX_Mul( normal.y, vector.y ) + FX_Mul( normal.z, vector.z ));

  if( de == 0 )
  {
    DEBUG_RAIL_Printf( "de == 0\n" );
    return FALSE; // ��_�Ȃ�
  }

  DEBUG_RAIL_Printf( "cross_dist[0x%x] de[0x%x]\n", cross_dist, de );

  cross_dist = FX_Div( cross_dist, de );
  

   
  // ��_�̓x�N�g���̐�
  if( cross_dist > vector_dist )
  {
    DEBUG_RAIL_Printf( "cross_dist > vector_dist\n" );
    return FALSE;
  }

  // ��_�̓x�N�g���̐�
  // cross_dist == 0�����郉�C���̕��ʏ�I
  if( cross_dist <= 0 )
  {
    DEBUG_RAIL_Printf( "cross_dist <= 0\n" );
    return FALSE;
  }

  DEBUG_RAIL_Printf( "cross_dist [%d]\n", FX_Whole( cross_dist ) );
  VEC_Set( cross, 
      vec_s->x + FX_Mul( vector.x, cross_dist ),
      vec_s->y + FX_Mul( vector.y, cross_dist ),
      vec_s->z + FX_Mul( vector.z, cross_dist ) );
  
  return TRUE;
}



