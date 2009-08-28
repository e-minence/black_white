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

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) );

	p_work = MMDL_GetMoveProcWork( fmmdl );

  FIELD_RAIL_WORK_SetLocation( p_work->rail_wk, location );
  p_work->rail_location = *location;
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
  
  return FIELD_RAIL_WORK_ForwardReq( p_work->rail_wk, railkey, frame );
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




