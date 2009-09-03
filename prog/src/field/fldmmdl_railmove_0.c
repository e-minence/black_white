//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fldfmmdl_railmove_0.c
 * @brief	  �t�B�[���h���샂�f���@���[����{����n����0
 *	@author	tomoya takahashi 
 *	@date		2009.08.26
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "fldmmdl.h"

#include "fldmmdl_procmove.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------


// DIR���烌�[���L�[�ɕϊ�
static const u8 sc_DIR_TO_RAILKEY[DIR_MAX4] = 
{
  RAIL_KEY_UP,
  RAIL_KEY_DOWN,
  RAIL_KEY_LEFT,
  RAIL_KEY_RIGHT,
};


//-------------------------------------
///	GRID R�����Ƃ߂鎮
//=====================================
#define MMDL_GET_RAILGRID_R(grid_size)    ( FX_Div( (grid_size), 4<<FX32_SHIFT ) )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���[�����ʍ\����
//=====================================
typedef struct 
{
  FIELD_RAIL_WORK* rail_wk;           ///<���[���ړ����[�N 4byte
  RAIL_LOCATION rail_location;        ///<���[���ړ��p�ʒu��� 8byte
} MV_RAIL_COMMON_WORK;
#define MV_RAIL_COMMON_WORK_SIZE (sizeof(MV_RAIL_COMMON_WORK))		///<MV_RAIL_COMMON�T�C�Y


//-------------------------------------
///	MV_RAIL_DMY_WORK�\����
//=====================================
typedef struct 
{
  MV_RAIL_COMMON_WORK rail_wk;
} MV_RAIL_DMY_WORK;
#define MV_RAIL_DMY_WORK_SIZE (sizeof(MV_RAIL_DMY_WORK))		///<MV_RAIL_DMY�T�C�Y

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// ��{�֐�
static RAIL_KEY MMdl_ConvertDir_RailKey( u16 dir );
static RAIL_FRAME MMdl_ConvertWait_RailFrame( s16 wait );

// COMMON
static void MMdl_RailCommon_Init( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl );
static void MMdl_RailCommon_Move( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl );
static void MMdl_RailCommon_Delete( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl );
static void MMdl_RailCommon_Return( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl );



//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�����̐ݒ�
 *
 *	@param	fmmdl        ���f�����[�N
 *	@param	location    ���P�[�V����
 */
//-----------------------------------------------------------------------------
void MMDL_SetRailLocation( MMDL * fmmdl, const RAIL_LOCATION* location )
{
  MV_RAIL_COMMON_WORK* p_work;
  VecFx32 pos;

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) );

	p_work = MMDL_GetMoveProcWork( fmmdl );

  FIELD_RAIL_WORK_SetLocation( p_work->rail_wk, location );
  p_work->rail_location = *location;

  // ���W�̍X�V
  FIELD_RAIL_WORK_GetPos( p_work->rail_wk, &pos );
	MMDL_SetVectorPos( fmmdl, &pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�����̎擾
 *
 *	@param	fmmdl        ���f�����[�N
 *	@param	location    ���P�[�V�����i�[��
 */
//-----------------------------------------------------------------------------
void MMDL_GetRailLocation( const MMDL * fmmdl, RAIL_LOCATION* location )
{
  const MV_RAIL_COMMON_WORK* cp_work;

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) );

	cp_work = MMDL_GetMoveProcWork( (MMDL*)fmmdl );

  *location = cp_work->rail_location;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �P�O�̃��P�[�V�����̎擾
 *
 *	@param	fmmdl        ���f�����[�N
 *	@param	location    ���P�[�V�����i�[��
 */
//-----------------------------------------------------------------------------
void MMDL_GetOldRailLocation( const MMDL * fmmdl, RAIL_LOCATION* location )
{
  const MV_RAIL_COMMON_WORK* cp_work;

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) );

	cp_work = MMDL_GetMoveProcWork( (MMDL*)fmmdl );
  FIELD_RAIL_WORK_GetLastLocation( cp_work->rail_wk, location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����[�N�̎擾
 *
 *	@param	fmmdl  ���f�����[�N
 *
 *	@return ���[�����[�N
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_WORK* MMDL_GetRailWork( const MMDL * fmmdl )
{
  const MV_RAIL_COMMON_WORK* cp_work;

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) );

	cp_work = MMDL_GetMoveProcWork( (MMDL*)fmmdl );

  return cp_work->rail_wk;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[������@���N�G�X�g
 *
 *	@param	fmmdl   ���[�N
 *	@param	dir     ����  
 *	@param	wait    �E�G�C�g  16,8,4,2
 */
//-----------------------------------------------------------------------------
BOOL MMDL_ReqRailMove( MMDL * fmmdl, u16 dir, s16 wait )
{
  MV_RAIL_COMMON_WORK* p_work;
  u32 railkey;
  u32 frame;


  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) );

	p_work = MMDL_GetMoveProcWork( fmmdl );


  //
  railkey = MMdl_ConvertDir_RailKey( dir );
  frame = MMdl_ConvertWait_RailFrame( wait );
  
  return FIELD_RAIL_WORK_ForwardReq( p_work->rail_wk, frame, railkey );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h���[�����샂�f���@�ړ��`�F�b�N
 *
 *	@param	mmdl              ���f��
 *	@param	now_location      ���݂̃��P�[�V����
 *	@param  next_location     ���̃��P�[�V����
 *
 * @retval	u32		�q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//-----------------------------------------------------------------------------
u32 MMDL_HitCheckRailMove( const MMDL *mmdl, const RAIL_LOCATION* now_location, const RAIL_LOCATION* next_location )
{
  const MV_RAIL_COMMON_WORK* cp_work;
  u32 ret = MMDL_MOVEHITBIT_NON;
  FLDNOGRID_MAPPER* p_mapper;

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) );

  // ���[�����샏�[�N�擾
	cp_work = MMDL_GetMoveProcWork( (MMDL*)mmdl );

  // �}�b�p�[�擾
  p_mapper = MMDLSYS_GetNOGRIDMapper( MMDL_GetMMdlSys( mmdl ) );

  // next_location�̃��[�������邩�H
  if( FIELD_RAIL_WORK_CheckLocation( cp_work->rail_wk, next_location ) == FALSE )
  {
    TOMOYA_Printf( "CHECK LOCATION MOVE LIMIT\n" );
    ret |= MMDL_MOVEHITBIT_LIM;
  }

  // �A�g���r���[�g�`�F�b�N
	if( MMDL_CheckMoveBitAttrGetOFF(mmdl) == FALSE )
  {
    MAPATTR attr;
    MAPATTR_FLAG attr_flag;

    attr = FLDNOGRID_MAPPER_GetAttr( p_mapper, next_location );
    attr_flag = MAPATTR_GetAttrFlag( attr );

    if( attr_flag & MAPATTR_FLAGBIT_HITCH )
    {
		  ret |= MMDL_MOVEHITBIT_ATTR;
    }
  }

  // ���f�������蔻��
  if( MMDL_HitCheckRailMoveFellow(mmdl, next_location) )
  {
    TOMOYA_Printf( "HIT OBJ\n" );
		ret |= MMDL_MOVEHITBIT_OBJ;
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h���[�����샂�f��  �ړ��`�F�b�N  ���̏ꏊ����
 *
 *	@param	mmdl              ���f��
 *	@param  next_location     ���̃��P�[�V����
 *
 * @retval	u32		�q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//-----------------------------------------------------------------------------
u32 MMDL_HitCheckRailMoveCurrent( const MMDL *mmdl, const RAIL_LOCATION* next_location )
{
  const MV_RAIL_COMMON_WORK* cp_work;
  RAIL_LOCATION location;

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) );

  // ���[�����샏�[�N�擾
	cp_work = MMDL_GetMoveProcWork( (MMDL*)mmdl );

  // ���̃��P�[�V�����擾
  FIELD_RAIL_WORK_GetLocation( cp_work->rail_wk, &location );

  return MMDL_HitCheckRailMove(mmdl, &location, next_location);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h���[�����샂�f��  �ړ��`�F�b�N  ���̏ꏊ����dir�̕�����
 *
 *	@param	mmdl              ���f��
 *	@param  dir               �ړ����� 
 *
 * @retval	u32		�q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//-----------------------------------------------------------------------------
u32 MMDL_HitCheckRailMoveDir( const MMDL *mmdl, u16 dir )
{
  const MV_RAIL_COMMON_WORK* cp_work;
  RAIL_LOCATION now_location;
  RAIL_LOCATION next_location;
  BOOL check_next_location;
  const FLDNOGRID_MAPPER* cp_mapper = MMDLSYS_GetNOGRIDMapper( MMDL_GetMMdlSys( mmdl ) );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( cp_mapper );

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) );

  // ���[�����샏�[�N�擾
	cp_work = MMDL_GetMoveProcWork( (MMDL*)mmdl );

  // DIR�ɑΉ������A���P�[�V�������擾
  FIELD_RAIL_WORK_GetLocation( cp_work->rail_wk, &now_location );
  check_next_location = FIELD_RAIL_MAN_CalcRailKeyLocation( cp_railman, &now_location, MMdl_ConvertDir_RailKey( dir ), &next_location );

  
  if( check_next_location )
  {
    return MMDL_HitCheckRailMove(mmdl, &now_location, &next_location);
  }

  TOMOYA_Printf( "MOVE LIMIT\n" );
  return MMDL_MOVEHITBIT_LIM;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����쒆�̕��̂����蔻��
 *
 *	@param	mmdl      ���f��
 *	@param  location  �ʒu
 *
 *	@retval TRUE  �Փ�
 *	@retval FALSE �Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL MMDL_HitCheckRailMoveFellow( const MMDL * mmdl, const RAIL_LOCATION* next_location )
{
  const MMDLSYS* cp_sys = MMDL_GetMMdlSys( mmdl );
  const FLDNOGRID_MAPPER* cp_mapper = MMDLSYS_GetNOGRIDMapper( cp_sys );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( cp_mapper );
  VecFx32 my_pos;
  VecFx32 mdl_pos;
  fx32 dist;
  MMDL* cmdl;
  u32 no = 0;
  fx32 grid_r;
  
  FIELD_RAIL_MAN_GetLocationPosition( cp_railman, next_location, &my_pos );

  grid_r = FIELD_RAIL_MAN_GetRailGridSize( cp_railman );
  grid_r = MMDL_GET_RAILGRID_R( grid_r );

	while( MMDLSYS_SearchUseMMdl(cp_sys,&cmdl,&no) == TRUE )
  {
		if( cmdl != mmdl )
    {
			if( MMDL_CheckStatusBit( cmdl,MMDL_STABIT_FELLOW_HIT_NON) == 0 )
      {
        MMDL_GetVectorPos( cmdl, &mdl_pos );

//        TOMOYA_Printf( "grid_r 0x%x my_pos x[0x%x]y[0x%x]z[0x%x] mdl_pos x[0x%x]y[0x%x]z[0x%x] \n", grid_r, my_pos.x, my_pos.y, my_pos.z, mdl_pos.x, mdl_pos.y, mdl_pos.z );
        if( FIELD_RAIL_TOOL_HitCheckSphere( &my_pos, &mdl_pos, grid_r ) )
        {
          return TRUE;
        }
      }
    }
  }
    
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�����̂�����ɂ���OBJ���擾����
 *
 *	@param	sys         MMDLSYS
 *	@param	location    ���P�[�V����
 *	@param	old_hit     �O�̃��P�[�V�����������������邩
 *
 *	@return �q�b�g����OBJ
 */
//-----------------------------------------------------------------------------
MMDL * MMDLSYS_SearchRailLocation( const MMDLSYS *sys, const RAIL_LOCATION* location, BOOL old_hit )
{
	u32 no = 0;
	MMDL *mmdl;
  const FLDNOGRID_MAPPER* cp_mapper = MMDLSYS_GetNOGRIDMapper( sys );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( cp_mapper );
  VecFx32 check_pos;
  VecFx32 mdl_pos;
  RAIL_LOCATION old_location;
  fx32 grid_r;
  

  FIELD_RAIL_MAN_GetLocationPosition( cp_railman, location, &check_pos );
  grid_r = FIELD_RAIL_MAN_GetRailGridSize( cp_railman );
  grid_r = MMDL_GET_RAILGRID_R( grid_r );

	while( MMDLSYS_SearchUseMMdl(sys,&mmdl,&no) == TRUE ){
		if( old_hit ){
      MMDL_GetOldRailLocation( mmdl, &old_location );
      FIELD_RAIL_MAN_GetLocationPosition( cp_railman, &old_location, &mdl_pos );

			if( FIELD_RAIL_TOOL_HitCheckSphere( &check_pos, &mdl_pos, grid_r ) ){
				return( mmdl );
			}
		}
		
    MMDL_GetVectorPos( mmdl, &mdl_pos );
    if( FIELD_RAIL_TOOL_HitCheckSphere( &check_pos, &mdl_pos, grid_r ) ){
      return( mmdl );
		}
	}
	
	return( NULL );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[������Ȃ��@������
 *
 *	@param	fmmdl   ���f�����[�N
 */
//-----------------------------------------------------------------------------
void MMDL_RailDmy_Init( MMDL * fmmdl )
{
  MV_RAIL_DMY_WORK* p_work;
	p_work = MMDL_InitMoveProcWork( fmmdl, MV_RAIL_DMY_WORK_SIZE );
  MMdl_RailCommon_Init( &p_work->rail_wk, fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[������Ȃ��@����
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_RailDmy_Move( MMDL * fmmdl )
{
  MV_RAIL_DMY_WORK* p_work;
	p_work = MMDL_GetMoveProcWork( fmmdl );
  MMdl_RailCommon_Move( &p_work->rail_wk, fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[������Ȃ��@�j��
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_RailDmy_Delete( MMDL * fmmdl )
{
  MV_RAIL_DMY_WORK* p_work;
	p_work = MMDL_GetMoveProcWork( fmmdl );
  MMdl_RailCommon_Delete( &p_work->rail_wk, fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[������Ȃ��@���A
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_RailDmy_Return( MMDL * fmmdl )
{
  MV_RAIL_DMY_WORK* p_work;
	p_work = MMDL_GetMoveProcWork( fmmdl );
  MMdl_RailCommon_Return( &p_work->rail_wk, fmmdl );
}





//-----------------------------------------------------------------------------
/**
 *          private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  ���������[���L�[�ɒu��������
 *
 *	@param	dir   ����
 *
 *	@return �L�[
 */
//-----------------------------------------------------------------------------
static RAIL_KEY MMdl_ConvertDir_RailKey( u16 dir )
{
  GF_ASSERT( dir < DIR_MAX4 );
  return sc_DIR_TO_RAILKEY[dir];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �E�G�C�g�����[���t���[���ɒu��������
 *
 *	@param	s16 wait 
 *
 *	@return ���[���t���[��
 */
//-----------------------------------------------------------------------------
static RAIL_FRAME MMdl_ConvertWait_RailFrame( s16 wait )
{
  u32 ret = RAIL_FRAME_8;
  
  switch( wait )
  {
  case 16:
    ret = RAIL_FRAME_16;
    break;

  case 8:
    ret = RAIL_FRAME_8;
    break;

  case 4:
    ret = RAIL_FRAME_4;
    break;

  case 2:
    ret = RAIL_FRAME_2;
    break;

  default:
    GF_ASSERT( 0 );
    break;
  }

  return ret;
}


// COMMON
//----------------------------------------------------------------------------
/**
 *	@brief  ���[���ړ�����  ������
 *
 *	@param	fmmdl   ���[�N
 */
//-----------------------------------------------------------------------------
static void MMdl_RailCommon_Init( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl )
{
  const MMDLSYS* cp_sys = MMDL_GetMMdlSys( fmmdl );
  FLDNOGRID_MAPPER* p_mapper = MMDLSYS_GetNOGRIDMapper( cp_sys );

  p_work->rail_wk = FLDNOGRID_MAPPER_CreateRailWork( p_mapper );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���ړ�����  ����
 *
 *	@param	fmmdl   ���[�N
 */
//-----------------------------------------------------------------------------
static void MMdl_RailCommon_Move( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl )
{
  // ���P�[�V�����̕ۑ�
  FIELD_RAIL_WORK_GetLocation( p_work->rail_wk, &p_work->rail_location );  
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���ړ�����  �j��
 *
 *	@param	fmmdl   ���[�N
 */
//-----------------------------------------------------------------------------
static void MMdl_RailCommon_Delete( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl )
{
  const MMDLSYS* cp_sys = MMDL_GetMMdlSys( fmmdl );
  FLDNOGRID_MAPPER* p_mapper = MMDLSYS_GetNOGRIDMapper( cp_sys );

  FLDNOGRID_MAPPER_DeleteRailWork( p_mapper, p_work->rail_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���ړ�����  ���A
 *
 *	@param	fmmdl   ���[�N
 */
//-----------------------------------------------------------------------------
static void MMdl_RailCommon_Return( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl )
{
  const MMDLSYS* cp_sys = MMDL_GetMMdlSys( fmmdl );
  FLDNOGRID_MAPPER* p_mapper = MMDLSYS_GetNOGRIDMapper( cp_sys );

  p_work->rail_wk = FLDNOGRID_MAPPER_CreateRailWork( p_mapper );
}




