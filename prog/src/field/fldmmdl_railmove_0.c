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

//-------------------------------------
///	MV_RAIL_RND_WORK�@����V�[�P���X
//=====================================
enum
{
  MV_RND_MOVE_SEQ_ACT,      ///<�U������A�N�V����
  MV_RND_MOVE_SEQ_ACT_WAIT, ///<����҂�
  MV_RND_MOVE_SEQ_TIME_WAIT,///<�҂�����
  MV_RND_MOVE_SEQ_WALK_ACT, ///<�ړ��A�N�V����
  MV_RND_MOVE_SEQ_WALK_ACT_WAIT, ///<�ړ��A�N�V����
};


//-------------------------------------
///	MV_RAIL_RT2_WORK�@����V�[�P���X
//=====================================
enum
{
  MV_RT2_MOVE_SEQ_CHECK_DIR,      ///<���������`�F�b�N
  MV_RT2_MOVE_SEQ_ACT_SET,        ///<����ݒ�
  MV_RT2_MOVE_SEQ_ACT_WAIT,       ///<����҂�
};

//--------------------------------------------------------------
///	�����e�[�u��ID
//--------------------------------------------------------------
enum
{
	DIRID_MvDirRndDirTbl = 0,
	DIRID_MvDirRndDirTbl_UL,
	DIRID_MvDirRndDirTbl_UR,
	DIRID_MvDirRndDirTbl_DL,
	DIRID_MvDirRndDirTbl_DR,
	DIRID_MvDirRndDirTbl_UDL,
	DIRID_MvDirRndDirTbl_UDR,
	DIRID_MvDirRndDirTbl_ULR,
	DIRID_MvDirRndDirTbl_DLR,
	DIRID_MvDirRndDirTbl_UD,
	DIRID_MvDirRndDirTbl_LR,
	DIRID_END,
	DIRID_MAX,

	WAIT_END = -1,
};

//--------------------------------------------------------------
//	DIR_TBL�\����
//--------------------------------------------------------------
typedef struct
{
	int id;
	const int *tbl;
}DIR_TBL;
//--------------------------------------------------------------
///	DIR_RND �E�F�C�g�e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndWaitTbl[] =
{ 16, 32, 48, 64, WAIT_END };

//--------------------------------------------------------------
///	DIR_RND �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl[] =
{ DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UL �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UL[] =
{ DIR_UP, DIR_LEFT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UR[] =
{ DIR_UP, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_DL �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_DL[] =
{ DIR_DOWN, DIR_LEFT, DIR_NOT };

//--------------------------------------------------------------
/// RND_DR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_DR[] =
{ DIR_DOWN, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UDL �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UDL[] =
{ DIR_UP, DIR_DOWN, DIR_LEFT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UDR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UDR[] =
{ DIR_UP, DIR_DOWN, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_ULR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_ULR[] =
{ DIR_UP, DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_DLR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_DLR[] =
{ DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UD �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UD[] =
{ DIR_UP, DIR_DOWN, DIR_NOT };

//--------------------------------------------------------------
/// RND_LR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_LR[] =
{ DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
///	�e�[�u���f�[�^
//--------------------------------------------------------------
static const DIR_TBL DATA_MoveDirTbl[DIRID_MAX] =
{
	{DIRID_MvDirRndDirTbl,DATA_MvDirRndDirTbl},
	{DIRID_MvDirRndDirTbl_UL,DATA_MvDirRndDirTbl_UL},
	{DIRID_MvDirRndDirTbl_UR,DATA_MvDirRndDirTbl_UR},
	{DIRID_MvDirRndDirTbl_DL,DATA_MvDirRndDirTbl_DL},
	{DIRID_MvDirRndDirTbl_DR,DATA_MvDirRndDirTbl_DR},
	{DIRID_MvDirRndDirTbl_UDL,DATA_MvDirRndDirTbl_UDL},
	{DIRID_MvDirRndDirTbl_UDR,DATA_MvDirRndDirTbl_UDR},
	{DIRID_MvDirRndDirTbl_ULR,DATA_MvDirRndDirTbl_ULR},
	{DIRID_MvDirRndDirTbl_DLR,DATA_MvDirRndDirTbl_DLR},
	{DIRID_MvDirRndDirTbl_UD,DATA_MvDirRndDirTbl_UD},
	{DIRID_MvDirRndDirTbl_LR,DATA_MvDirRndDirTbl_LR},
	{DIRID_END,NULL},
};


//-------------------------------------
///	GRID R�����Ƃ߂鎮
//=====================================
#define MMDL_GET_RAILGRID_R(grid_size)    ( FX_Div( (grid_size), 4<<FX32_SHIFT ) )



//--------------------------------------------------------------
///	���[���L�[���O���̏ꍇ�́A�c�h�q�̕ϊ��e�[�u��
//--------------------------------------------------------------
static const u8 sc_RAILLINE_DIR_CHANGE_TBL[ RAIL_KEY_MAX ][ DIR_MAX4 ] = 
{
  // NULL�͊֌W�Ȃ�
  {
    0
  },
  // RAIL_KEY_UP
  {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
  },
  // RAIL_KEY_RIGHT
  {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_UP,
  },
  // RAIL_KEY_DOWN
  {
    DIR_DOWN,
    DIR_UP,
    DIR_RIGHT,
    DIR_LEFT,
  },
  // RAIL_KEY_LEFT
  {
    DIR_RIGHT,
    DIR_LEFT,
    DIR_UP,
    DIR_DOWN,
  },
}; 

//--------------------------------------------------------------
///	�e�c�h�q�̉�]���@�t�B�[���x�R�c���W�n�ł̉�]���
// -z
//      ��
//   ���@�@�E   
//      ��
// +z
//--------------------------------------------------------------
static const u16 sc_RAILLINE_DIR_ROT_Y[ DIR_MAX4 ] =
{
  // DIR_UP
  0,
  // DIR_DOWN
  0x8000,
  // DIR_LEFT
  0x4000,
  // DIR_RIGHT
  0xC000,
};


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���[�����ʍ\����
//�@�K���A���[�N�̃g�b�v�ɂ��̍\���̂����邱�ƁI�I�I
//
//location.type �� FIELD_RAIL_TYPE_MAX�̂Ƃ��A�ݒ胍�P�[�V�����f�[�^�Ȃ��ɂȂ�܂��B
//=====================================
typedef struct 
{
  FIELD_RAIL_WORK* rail_wk;           ///<���[���ړ����[�N 4byte
  RAIL_LOCATION location;             ///<���[�����P�[�V���� 8bytie   
                                      // MMdl_RailCommon_SetSaveLocation�Ŗ��t���[���X�V
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

//-------------------------------------
///	MV_RAIL_DIR_RND_WORK�\����
//=====================================
typedef struct 
{
  MV_RAIL_COMMON_WORK rail_wk;
  s16 wait;
  u16 tbl_id;
} MV_RAIL_DIR_RND_WORK;
#define MV_RAIL_DIR_RND_WORK_SIZE (sizeof(MV_RAIL_DIR_RND_WORK))		///<MV_RAIL_DIR_RND�T�C�Y

//-------------------------------------
///	MV_RAIL_RND_WORK�\����
//=====================================
typedef struct 
{
  MV_RAIL_COMMON_WORK rail_wk;
  u8 seq_no:4;
  u8 tbl_id:4;
  s8 wait;
  s8 init_width;
  u8 init_line;
} MV_RAIL_RND_WORK;
#define MV_RAIL_RND_WORK_SIZE (sizeof(MV_RAIL_RND_WORK))		///<MV_RAIL_RND�T�C�Y


//-------------------------------------
///	MV_RAIL_RT2_WORK�\����
//=====================================
typedef struct 
{
  MV_RAIL_COMMON_WORK rail_wk;
  u8 seq_no;
  u8 turn_flag;
  s8 init_width;
  u8 init_line;
} MV_RAIL_RT2_WORK;
#define MV_RAIL_RT2_WORK_SIZE (sizeof(MV_RAIL_RT2_WORK))		///<MV_RAIL_RT�T�C�Y

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// ��{�֐�
static RAIL_KEY MMdl_ConvertDir_RailKey( u16 dir );
static RAIL_FRAME MMdl_ConvertWait_RailFrame( s16 wait );

static void* MMdl_RailDefaultInit( MMDL* fmmdl, u32 work_size );

static BOOL MMdl_CheckRailLimit( const MMDL* fmmdl, u8 init_line, s8 init_width, const RAIL_LOCATION* cp_location );

// RND�e�[�u��
static int TblNumGet( const int *tbl, int end );
static int TblRndGet( const int *tbl, int end );
static int TblIDRndGet( int id, int end );
static const int * MoveDirTblIDSearch( int id );

// COMMON
static void MMdl_RailCommon_Init( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl );
static void MMdl_RailCommon_Move( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl );
static void MMdl_RailCommon_Delete( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl );
static void MMdl_RailCommon_Return( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl );

static void MMdl_RailCommon_SetSaveLocation( MV_RAIL_COMMON_WORK* p_work, const RAIL_LOCATION* location );
static void MMdl_RailCommon_ReflectSaveLocation( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl );



// DIR_RND
static void DirRndWorkInit( MMDL * fmmdl, int id );
static void DirRndWorkMove( MMDL * fmmdl );
static void DirRndWorkDelete( MMDL * fmmdl );


// MV_RND
static void MvRndWorkInit( MMDL * fmmdl, int id );
static void MvRndWorkMove( MMDL * fmmdl );
static void MvRndWorkDelete( MMDL * fmmdl );


// MV_RT2
static void MvRoot2WorkInit( MMDL * fmmdl );
static void MvRoot2WorkMove( MMDL * fmmdl );
static void MvRoot2WorkDelete( MMDL * fmmdl );

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

  // �ۑ�
  MMdl_RailCommon_SetSaveLocation( p_work, location );

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
  const FLDNOGRID_MAPPER* cp_mapper = MMDLSYS_GetNOGRIDMapper( MMDL_GetMMdlSys( fmmdl ) );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( cp_mapper );

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) );

	cp_work = MMDL_GetMoveProcWork( (MMDL*)fmmdl );

  if( FIELD_RAIL_WORK_IsAction( cp_work->rail_wk ) )
  {
    // �ړ����Ԃ�
    FIELD_RAIL_WORK_GetLocation( cp_work->rail_wk, location );
    FIELD_RAIL_MAN_CalcRailKeyLocation( cp_railman, location, FIELD_RAIL_WORK_GetActionKey( cp_work->rail_wk ), location );
  }
  else
  {
    // ����Ԃ�
    FIELD_RAIL_WORK_GetLocation( cp_work->rail_wk, location );
  }
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
  FIELD_RAIL_WORK_GetLocation( cp_work->rail_wk, location );
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
 *	@brief  ���[���̃A�b�v�f�[�g�������s��
 *
 *	@param	fmmdl   ���[�N
 */
//-----------------------------------------------------------------------------
void MMDL_UpdateRail( MMDL* fmmdl )
{
  MV_RAIL_COMMON_WORK* p_work;

  // ���[������`�F�b�N
  GF_ASSERT( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) );

	p_work = MMDL_GetMoveProcWork( (MMDL*)fmmdl );
  FIELD_RAIL_WORK_Update( p_work->rail_wk );

  // ���P�[�V�����̃Z�[�u
  MMdl_RailCommon_Move( p_work, fmmdl );
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
  return MMDL_HitCheckRailMoveEx( mmdl, now_location, next_location, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h���[�����샂�f���@�ړ��`�F�b�N
 *
 *	@param	mmdl              ���f��
 *	@param	now_location      ���݂̃��P�[�V����
 *	@param  next_location     ���̃��P�[�V����
 *	@param	p_attr            ���̃��P�[�V�����̃A�g���r���[�g 
 *
 * @retval	u32		�q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//-----------------------------------------------------------------------------
u32 MMDL_HitCheckRailMoveEx( const MMDL *mmdl, const RAIL_LOCATION* now_location, const RAIL_LOCATION* next_location, MAPATTR* p_attr )
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

  // now_location�̃��[�������邩�H
  if( FIELD_RAIL_WORK_CheckLocation( cp_work->rail_wk, now_location ) == FALSE )
  {
    TOMOYA_Printf( "CHECK NOW LOCATION MOVE LIMIT\n" );
    ret |= MMDL_MOVEHITBIT_LIM;
  }


  // next_location�̃��[�������邩�H
  if( FIELD_RAIL_WORK_CheckLocation( cp_work->rail_wk, next_location ) == FALSE )
  {
    TOMOYA_Printf( "CHECK NEXT LOCATION MOVE LIMIT\n" );
    ret |= MMDL_MOVEHITBIT_LIM;
  }

  // �A�g���r���[�g�`�F�b�N
	if( MMDL_CheckStatusBitAttrGetOFF(mmdl) == FALSE )
  {
    MAPATTR attr;
    MAPATTR_FLAG attr_flag;

    attr = FLDNOGRID_MAPPER_GetAttr( p_mapper, next_location );
    attr_flag = MAPATTR_GetAttrFlag( attr );

    if( attr_flag & MAPATTR_FLAGBIT_HITCH )
    {
      TOMOYA_Printf( "HIT ATTR\n" );
		  ret |= MMDL_MOVEHITBIT_ATTR;
    }

    if( p_attr ){
      *p_attr = attr;
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
  return MMDL_HitCheckRailMoveDirEx( mmdl, dir, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h���[�����샂�f��  �ړ��`�F�b�N  ���̏ꏊ����dir�̕�����
 *
 *	@param	mmdl              ���f��
 *	@param  dir               �ړ����� 
 *	@param	p_attr            �ړ���A�g���r���[�g�i�[��
 *
 * @retval	u32		�q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//-----------------------------------------------------------------------------
u32 MMDL_HitCheckRailMoveDirEx( const MMDL *mmdl, u16 dir, MAPATTR* p_attr )
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
    return MMDL_HitCheckRailMoveEx(mmdl, &now_location, &next_location, p_attr);
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
  RAIL_LOCATION mdl_location;
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
        if( !MMDL_CheckStatusBit( cmdl,MMDL_STABIT_RAIL_MOVE) == 0 )
        {
          // NOW
          MMDL_GetRailLocation( cmdl, &mdl_location );
          FIELD_RAIL_MAN_GetLocationPosition( cp_railman, &mdl_location, &mdl_pos );

          //TOMOYA_Printf( "now grid_r 0x%x my_pos x[0x%x]y[0x%x]z[0x%x] mdl_pos x[0x%x]y[0x%x]z[0x%x] \n", grid_r, my_pos.x, my_pos.y, my_pos.z, mdl_pos.x, mdl_pos.y, mdl_pos.z );
          //TOMOYA_Printf( "mdl_location line%d width%d\n", mdl_location.line_grid, mdl_location.width_grid );
          if( FIELD_RAIL_TOOL_HitCheckSphere( &my_pos, &mdl_pos, grid_r ) )
          {
            return TRUE;
          }

          // OLD
          MMDL_GetOldRailLocation( cmdl, &mdl_location );
          FIELD_RAIL_MAN_GetLocationPosition( cp_railman, &mdl_location, &mdl_pos );

          //TOMOYA_Printf( "last grid_r 0x%x my_pos x[0x%x]y[0x%x]z[0x%x] mdl_pos x[0x%x]y[0x%x]z[0x%x] \n", grid_r, my_pos.x, my_pos.y, my_pos.z, mdl_pos.x, mdl_pos.y, mdl_pos.z );
          //TOMOYA_Printf( "mdl_location line%d width%d\n", mdl_location.line_grid, mdl_location.width_grid );
          if( FIELD_RAIL_TOOL_HitCheckSphere( &my_pos, &mdl_pos, grid_r ) )
          {
            return TRUE;
          }
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
  RAIL_LOCATION mdl_location;
  fx32 grid_r;
  

  FIELD_RAIL_MAN_GetLocationPosition( cp_railman, location, &check_pos );
  grid_r = FIELD_RAIL_MAN_GetRailGridSize( cp_railman );
  grid_r = MMDL_GET_RAILGRID_R( grid_r );

	while( MMDLSYS_SearchUseMMdl(sys,&mmdl,&no) == TRUE ){

    if( !MMDL_CheckStatusBit( mmdl,MMDL_STABIT_RAIL_MOVE) == 0 )
    {

      if( old_hit ){
        MMDL_GetOldRailLocation( mmdl, &old_location );
        FIELD_RAIL_MAN_GetLocationPosition( cp_railman, &old_location, &mdl_pos );

        if( FIELD_RAIL_TOOL_HitCheckSphere( &check_pos, &mdl_pos, grid_r ) ){
          return( mmdl );
        }
      }
      
      MMDL_GetRailLocation( mmdl, &mdl_location );
      FIELD_RAIL_MAN_GetLocationPosition( cp_railman, &mdl_location, &mdl_pos );
      if( FIELD_RAIL_TOOL_HitCheckSphere( &check_pos, &mdl_pos, grid_r ) ){
        return( mmdl );
      }
    }
	}
	
	return( NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ڂ̑O�̃��P�[�V�������擾
 *
 *	@param	mmdl
 *	@param	location 
 *
 *	@retval TRUE  �ڂ̑O������
 *	@retval FALSE �ڂɂ͂����Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL MMDL_GetRailFrontLocation( const MMDL *mmdl, RAIL_LOCATION* location )
{
  const MMDLSYS* cp_sys = MMDL_GetMMdlSys( mmdl );
  const FLDNOGRID_MAPPER* cp_mapper = MMDLSYS_GetNOGRIDMapper( cp_sys );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( cp_mapper );
  u16 dir;

  dir = MMDL_GetDirDisp( mmdl );


  MMDL_GetRailLocation( mmdl, location );
//  TOMOYA_Printf( "dir %d  line_grid %d\n", dir, location->line_grid );
  return FIELD_RAIL_MAN_CalcRailKeyLocation( cp_railman, location, FIELD_RAIL_TOOL_ConvertDirToRailKey(dir), location ); 
}

//----------------------------------------------------------------------------
/**
 *	@brief  dir�����̃��P�[�V�������擾
 *
 *	@param	mmdl        ���f��
 *	@param	dir         ����
 *	@param	location    ���[�����P�[�V�����i�[��
 *
 *	@retval TRUE  �ړ��\
 *	@retval FALSE �ړ��s�\
 */
//-----------------------------------------------------------------------------
BOOL MMDL_GetRailDirLocation( const MMDL *mmdl, u16 dir, RAIL_LOCATION* location )
{
  const MMDLSYS* cp_sys = MMDL_GetMMdlSys( mmdl );
  const FLDNOGRID_MAPPER* cp_mapper = MMDLSYS_GetNOGRIDMapper( cp_sys );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( cp_mapper );

  MMDL_GetRailLocation( mmdl, location );
//  TOMOYA_Printf( "dir %d  line_grid %d\n", dir, location->line_grid );
  return FIELD_RAIL_MAN_CalcRailKeyLocation( cp_railman, location, FIELD_RAIL_TOOL_ConvertDirToRailKey(dir), location ); 
}

//----------------------------------------------------------------------------
/**
 *	@brief  dir�����Ƀ��[���Ői�񂾏ꍇ�̃O���b�h���W��ݒ肷��
 *
 *	@param	mmdl    ���f�����[�N
 *	@param	dir     ����
 */
//-----------------------------------------------------------------------------
void MMDL_Rail_UpdateGridPosDir( MMDL *mmdl, u16 dir )
{
  const MMDLSYS* cp_sys = MMDL_GetMMdlSys( mmdl );
  const FLDNOGRID_MAPPER* cp_mapper = MMDLSYS_GetNOGRIDMapper( cp_sys );
  const FIELD_RAIL_MAN* cp_railman = FLDNOGRID_MAPPER_GetRailMan( cp_mapper );
  VecFx32 pos;
  RAIL_LOCATION location;

  MMDL_GetRailLocation( mmdl, &location );
  FIELD_RAIL_MAN_CalcRailKeyPos( cp_railman, &location, FIELD_RAIL_TOOL_ConvertDirToRailKey(dir), &pos ); 
  
  // 3D���W���O���b�h�ɐݒ肷��
	MMDL_SetOldGridPosX( mmdl, MMDL_GetGridPosX(mmdl) );
	MMDL_SetOldGridPosY( mmdl, MMDL_GetGridPosY(mmdl) );
	MMDL_SetOldGridPosZ( mmdl, MMDL_GetGridPosZ(mmdl) );
	
	MMDL_SetGridPosX( mmdl, SIZE_GRID_FX32(pos.x) );
	MMDL_SetGridPosY( mmdl, SIZE_GRID_FX32(pos.y) );
	MMDL_SetGridPosZ( mmdl, SIZE_GRID_FX32(pos.z) );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �v���C���[�i�s�������擾����
 *	        
 *
 *	@param  mmdl    ���f�����[�N
 *	@param	way     ����
 */
//-----------------------------------------------------------------------------
void MMDL_Rail_GetFrontWay( const MMDL *mmdl, VecFx16* way )
{
  FIELD_RAIL_WORK* p_railwk;
  int dir;
  
  GF_ASSERT( mmdl );
  GF_ASSERT( way );

  p_railwk = MMDL_GetRailWork( mmdl );

  dir = MMDL_GetDirDisp( mmdl );
  MMDL_Rail_GetDirLineWay( mmdl, dir, way );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C����dir�����̎擾
 *
 *	@param	mmdl    ���f�����[�N
 *	@param	dir     ���f���̌������
 *	@param	way     ����
 */
//-----------------------------------------------------------------------------
void MMDL_Rail_GetDirLineWay( const MMDL *mmdl, u16 dir, VecFx16* way )
{
  FIELD_RAIL_WORK* p_railwk;
  int rail_key;
  u16 rail_dir;
  u16 rot;
  MtxFx33 mtx; 
  VecFx32 calc_vec;
  
  GF_ASSERT( mmdl );
  GF_ASSERT( way );
  GF_ASSERT( dir < DIR_MAX4 );

  p_railwk = MMDL_GetRailWork( mmdl );
  FIELD_RAIL_WORK_GetFrontWay( p_railwk, way );

  // �v���C���[�̕����ƃ��C���O���������g���A�p�x��ϊ�����
  rail_key = FIELD_RAIL_WORK_GetFrontKey( p_railwk );
  GF_ASSERT( rail_key != RAIL_KEY_NULL );

  // ���[���O�������̏�񂩂�Away�̉�]�p�x�����߂�
  rail_dir  = sc_RAILLINE_DIR_CHANGE_TBL[ rail_key ][ dir ];
  rot       = sc_RAILLINE_DIR_ROT_Y[ rail_dir ];

  // Y�̒l�́Acos�œ�����
  way->y = FX_Mul( FX_CosIdx( rot ), way->y );

  // XZ�̃p�����[�^������]������
  VEC_Set( &calc_vec, way->x, way->y, way->z );
  MTX_RotY33( &mtx, FX_SinIdx( rot ), FX_CosIdx( rot ) );
  MTX_MultVec33( &calc_vec, &mtx, &calc_vec );
  VEC_Normalize( &calc_vec, &calc_vec );
  VEC_Fx16Set( way, calc_vec.x, calc_vec.y, calc_vec.z );
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
	p_work = MMdl_RailDefaultInit( fmmdl, MV_RAIL_DMY_WORK_SIZE );
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




//----------------------------------------------------------------------------
/**
 *	@brief  �U������@�S���������_��  ������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_ALL_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@UP�@LEFT�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_UL_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@UP�@RIGHT�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_UR_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UR );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@DOWN�@LEFT�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_DL_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_DL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@DOWN�@RIGHT�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_DR_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_DR );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@UP�@DOWN�@LEFT�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_UDL_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UDL );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@UP�@DOWN�@RIGHT�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_UDR_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UDR );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@UP�@LEFT�@RIGHT�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_ULR_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_ULR );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@DOWN�@LEFT�@RIGHT�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_DLR_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_DLR );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@UP�@DOWN�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_UD_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UD );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �U������@LEFT�@RIGHT�@���������_���@������
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_LR_Init( MMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_LR );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���������_���@���C��
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_Move( MMDL * fmmdl )
{
  DirRndWorkMove( fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���������_���@�j��
 */
//-----------------------------------------------------------------------------
void MMDL_RailDirRnd_Delete( MMDL * fmmdl )
{
  DirRndWorkDelete( fmmdl );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��@�S���������_��  ������
 */
//-----------------------------------------------------------------------------
void MMDL_RailRnd_ALL_Init( MMDL * fmmdl )
{
  MvRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��@�c���������_��  ������
 */
//-----------------------------------------------------------------------------
void MMDL_RailRnd_V_Init( MMDL * fmmdl )
{
  MvRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UD );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��@�����������_��  ������
 */
//-----------------------------------------------------------------------------
void MMDL_RailRnd_H_Init( MMDL * fmmdl )
{
  MvRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_LR );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��@�����_��  ���C��
 */
//-----------------------------------------------------------------------------
void MMDL_RailRnd_Move( MMDL * fmmdl )
{
  MvRndWorkMove( fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��@�����_���@�j��
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_RailRnd_Delete( MMDL * fmmdl )
{
  MvRndWorkDelete( fmmdl );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �Q���򃋁[�g�ړ��@������
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_RailRoot2_Init( MMDL * fmmdl )
{
  MvRoot2WorkInit( fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Q���򃋁[�g�ړ��@����
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_RailRoot2_Move( MMDL * fmmdl )
{
  MvRoot2WorkMove( fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Q���򃋁[�g�ړ��@�j��
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_RailRoot2_Delete( MMDL * fmmdl )
{
  MvRoot2WorkDelete( fmmdl );
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
  return FIELD_RAIL_TOOL_ConvertDirToRailKey(dir);
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


//----------------------------------------------------------------------------
/**
 *	@brief  ��{����������
 *
 *	@param	fmmdl       ���f��
 *	@param	work_size   ���[�N�T�C�Y
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
static void* MMdl_RailDefaultInit( MMDL* fmmdl, u32 work_size )
{
  MV_RAIL_COMMON_WORK* p_work;
  RAIL_LOCATION location;
  BOOL setup;

  setup = MMdl_GetSetUpLocation( fmmdl, &location );
  
	p_work = MMDL_InitMoveProcWork( fmmdl, work_size );
  MMdl_RailCommon_Init( p_work, fmmdl );

  if( setup )
  {
    MMDL_SetRailLocation( fmmdl, &location );
  }
  else
  {
    //0�N���A
    GFL_STD_MemClear( &location, sizeof(RAIL_LOCATION) );
    MMDL_SetRailLocation( fmmdl, &location );
  }

  return p_work;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����~�b�g�`�F�b�N
 *
 *	@param	fmmdl       ���f��
 *	@param	init_line   ���C���O���b�h�����l
 *	@param	init_width  ���O���b�h�����l
 *	@param  cp_location �`�F�b�N���P�[�V����
 *
 *	@retval TRUE  ���~�b�g�q�b�g
 *	@retval FALSE ���~�b�g���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL MMdl_CheckRailLimit( const MMDL* fmmdl, u8 init_line, s8 init_width, const RAIL_LOCATION* cp_location )
{
  int limit_x, limit_z, max, min;
  
  limit_x = MMDL_GetMoveLimitX( fmmdl );  // Width
  limit_z = MMDL_GetMoveLimitZ( fmmdl );  // Line

  // �ړ����~�b�g�`�F�b�N
  if( limit_x != MOVE_LIMIT_NOT )
  {
    min = init_width - limit_x;
    max = init_width + limit_x;
    if( (min > cp_location->width_grid) || (max < cp_location->width_grid) )
    {
      return TRUE;
    }
  }

  if( limit_z != MOVE_LIMIT_NOT )
  {
    min = init_line - limit_z;
    max = init_line + limit_z;
    if( (min > cp_location->line_grid) || (max < cp_location->line_grid) )
    {
      return TRUE;
    }
  }

  return FALSE;
}


//--------------------------------------------------------------
/**
 * �����e�[�u���v�f���擾
 * @param	tbl		�f�[�^�e�[�u���B�I�[��end
 * @param	end		�e�[�u���I�[�Ƃ���l
 * @retval	int		tbl�v�f��
 */
//--------------------------------------------------------------
static int TblNumGet( const int *tbl, int end )
{
	int i=0; while(tbl[i]!=end){i++;} GF_ASSERT(i&&"TblNumGet()�v�f��0"); return(i);
}

//--------------------------------------------------------------
/**
 * �e�[�u���f�[�^���烉���_���Œl�擾
 * @param	tbl		�f�[�^�e�[�u���B�I�[��end
 * @param	end		�e�[�u���I�[�Ƃ���l
 * @retval	int		tbl�����̒l
 */
//--------------------------------------------------------------
static int TblRndGet( const int *tbl, int end )
{
#if 0
	return( tbl[gf_rand()%TblNumGet(tbl,end)] );
#else
	return( tbl[GFUser_GetPublicRand(TblNumGet(tbl,end))] );
#endif
}

//--------------------------------------------------------------
/**
 * ����ID����e�[�u���f�[�^���擾�������_���Œl�擾
 * @param	tbl		�f�[�^�e�[�u���B�I�[��end
 * @param	end		�e�[�u���I�[�Ƃ���l
 * @retval	int		tbl�����̒l
 */
//--------------------------------------------------------------
static int TblIDRndGet( int id, int end )
{
	const int *tbl = MoveDirTblIDSearch( id );
	return( tbl[GFUser_GetPublicRand(TblNumGet(tbl,end))] );
}

//--------------------------------------------------------------
/**
 * DATA_MoveDirTbl����
 * @param	id		����ID
 * @retval	int		�q�b�g�����e�[�u��*
 */
//--------------------------------------------------------------
static const int * MoveDirTblIDSearch( int id )
{
	const DIR_TBL *tbl = DATA_MoveDirTbl;
	
	while( tbl->id != DIRID_END ){
		if( tbl->id == id ){ return( tbl->tbl ); }
		tbl++;
	}
	
	GF_ASSERT( 0 && "MoveDirTblIDSearch()ID�ُ�" );
	return( NULL );
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
  p_work->location.type = FIELD_RAIL_TYPE_MAX; // �ݒ�f�[�^�Ȃ�
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
  RAIL_LOCATION location;
  // �ʒu���̕ۑ�
  FIELD_RAIL_WORK_GetLocation( p_work->rail_wk, &location );
//  TOMOYA_Printf( "location index %d  line %d  side %d\n", location.rail_index, location.line_grid, location.width_grid );
  MMdl_RailCommon_SetSaveLocation( p_work, &location );
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

  // ���[���̃t���O��j��
  MMDL_OffStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE );
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

  // �Z�[�u���P�[�V�����̔��f
  MMdl_RailCommon_ReflectSaveLocation( p_work, fmmdl );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u���P�[�V�����̐ݒ�
 *
 *	@param	p_work
 *	@param	location 
 */
//-----------------------------------------------------------------------------
static void MMdl_RailCommon_SetSaveLocation( MV_RAIL_COMMON_WORK* p_work, const RAIL_LOCATION* location )
{
  p_work->location = *location;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u���P�[�V�����̔��f
 *
 *	@param	p_work
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_RailCommon_ReflectSaveLocation( MV_RAIL_COMMON_WORK* p_work, MMDL * fmmdl )
{
  if( p_work->location.type != FIELD_RAIL_TYPE_MAX )
  {
    VecFx32 pos;
    
    FIELD_RAIL_WORK_SetLocation( p_work->rail_wk, &p_work->location );
    //p_work->location.type = FIELD_RAIL_TYPE_MAX;

    // �|�W�V����
    FIELD_RAIL_WORK_GetPos( p_work->rail_wk, &pos );
    MMDL_SetVectorPos( fmmdl, &pos );
  }else{
    GF_ASSERT(0);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���������_�����[�N�@������
 *
 *	@param	fmmdl   ���f��
 *	@param	id      �����e�[�u��ID
 */
//-----------------------------------------------------------------------------
static void DirRndWorkInit( MMDL * fmmdl, int id )
{
  MV_RAIL_DIR_RND_WORK* p_wk;

  p_wk = MMdl_RailDefaultInit( fmmdl, MV_RAIL_DIR_RND_WORK_SIZE );

  p_wk->wait    = TblRndGet( DATA_MvDirRndWaitTbl, WAIT_END );			//�҂����ԃZ�b�g
  p_wk->tbl_id  = id;

	MMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );					//�`��X�e�[�^�X�@��~
	MMDL_OffMoveBitMove( fmmdl );								//��ɒ�~��
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���������_�����[�N�@���C��
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
static void DirRndWorkMove( MMDL * fmmdl )
{
	MV_RAIL_DIR_RND_WORK* p_wk = MMDL_GetMoveProcWork( fmmdl );
	
  p_wk->wait--;

  if( p_wk->wait <= 0 )
  {									//�҂����� 0
    p_wk->wait = TblRndGet( DATA_MvDirRndWaitTbl, WAIT_END );
    MMDL_SetDirDisp( fmmdl, TblIDRndGet(p_wk->tbl_id,DIR_NOT) );
  }

  MMdl_RailCommon_Move( &p_wk->rail_wk, fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���������_�����[�N�@�j��
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
static void DirRndWorkDelete( MMDL * fmmdl )
{
	MV_RAIL_DIR_RND_WORK* p_wk = MMDL_GetMoveProcWork( fmmdl );
  MMdl_RailCommon_Delete( &p_wk->rail_wk, fmmdl );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �����_���ړ��@������
 *
 *	@param	fmmdl   
 *	@param	id            �����e�[�u��ID
 */
//-----------------------------------------------------------------------------
static void MvRndWorkInit( MMDL * fmmdl, int id )
{
  MV_RAIL_RND_WORK* p_wk;
  RAIL_LOCATION location;

  p_wk = MMdl_RailDefaultInit( fmmdl, MV_RAIL_RND_WORK_SIZE );

  MMDL_GetRailLocation( fmmdl, &location );

  p_wk->seq_no = 0;
  p_wk->wait = 0;
  p_wk->tbl_id = id;

  // ���[�N�T�C�Y���I�[�o�[���Ȃ��悤�Ƀ`�F�b�N
  GF_ASSERT( location.line_grid < 0xff );
  GF_ASSERT( location.width_grid < 0x7f );

  p_wk->init_line   = location.line_grid;
  p_wk->init_width  = location.width_grid;

	MMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );
	MMDL_OffMoveBitMove( fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����_���ړ��@���C��
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
static void MvRndWorkMove( MMDL * fmmdl )
{
	MV_RAIL_RND_WORK* p_wk = MMDL_GetMoveProcWork( fmmdl );
  u32 ret;
	
	switch( p_wk->seq_no ){
	case MV_RND_MOVE_SEQ_ACT:
		MMDL_OffMoveBitMove( fmmdl );
		MMDL_OffMoveBitMoveEnd( fmmdl );
		
		ret = MMDL_GetDirDisp( fmmdl );
		ret = MMDL_ChangeDirAcmdCode( ret, AC_DIR_U );
		MMDL_SetLocalAcmd( fmmdl, ret );
		
		p_wk->seq_no++;
		break;
	case MV_RND_MOVE_SEQ_ACT_WAIT:
		if( MMDL_ActionLocalAcmd(fmmdl) == FALSE ){
			break;
		}
		
		p_wk->wait = TblRndGet( DATA_MvDirRndWaitTbl, WAIT_END );
		p_wk->seq_no++;
	case MV_RND_MOVE_SEQ_TIME_WAIT:
		p_wk->wait--;
		
		if( p_wk->wait ){
			break;
		}
		
		p_wk->seq_no++;
	case MV_RND_MOVE_SEQ_WALK_ACT:
		ret = TblIDRndGet( p_wk->tbl_id, DIR_NOT );
		MMDL_SetDirAll( fmmdl, ret );

    // �q�b�g�`�F�b�N
		{
			u32 hit = MMDL_HitCheckRailMoveDir( fmmdl, ret );
			
			if( hit != MMDL_MOVEHITBIT_NON ){
        p_wk->seq_no = 0;
        break;
			}
		}

    // ���C����芷���`�F�b�N
    // �ړ����~�b�g�`�F�b�N
    {
      RAIL_LOCATION now_location;
      RAIL_LOCATION next_location;

      MMDL_GetRailLocation( fmmdl, &now_location );
      MMDL_GetRailDirLocation( fmmdl, ret, &next_location );

      // ���C����芷���`�F�b�N
      if( now_location.rail_index != next_location.rail_index )
      {
        p_wk->seq_no = 0;
        break;
      }


      // �ړ����~�b�g�`�F�b�N
      if( MMdl_CheckRailLimit( fmmdl, p_wk->init_line, p_wk->init_width, &next_location ) )
      {
        p_wk->seq_no = 0;
        break;
      }
    }
		
		ret = MMDL_ChangeDirAcmdCode( ret, AC_WALK_U_8F );
		MMDL_SetLocalAcmd( fmmdl, ret );
			
		MMDL_OnMoveBitMove( fmmdl );
		p_wk->seq_no++;
	case MV_RND_MOVE_SEQ_WALK_ACT_WAIT:
		if( MMDL_ActionLocalAcmd(fmmdl) == FALSE ){
			break;
		}
		
		MMDL_OffMoveBitMove( fmmdl );
		p_wk->seq_no = 0;
    break;

  default:
    GF_ASSERT(0);
    break;
	}

  MMdl_RailCommon_Move( &p_wk->rail_wk, fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����_���ړ��@�j��
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
static void MvRndWorkDelete( MMDL * fmmdl )
{ 
	MV_RAIL_RND_WORK* p_wk = MMDL_GetMoveProcWork( fmmdl );
  MMdl_RailCommon_Delete( &p_wk->rail_wk, fmmdl );
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���[�g�ړ�  ������
 *    
 *	@param	fmmdl
 *	@param	id        �e�[�u��ID
 */
//-----------------------------------------------------------------------------
static void MvRoot2WorkInit( MMDL * fmmdl )
{
  MV_RAIL_RT2_WORK* p_wk;
  RAIL_LOCATION location;

  p_wk = MMdl_RailDefaultInit( fmmdl, MV_RAIL_RT2_WORK_SIZE );

  MMDL_GetRailLocation( fmmdl, &location );

  p_wk->seq_no = 0;
  p_wk->turn_flag = 0;

  // ���[�N�T�C�Y���I�[�o�[���Ȃ��悤�Ƀ`�F�b�N
  GF_ASSERT( location.line_grid < 0xff );
  GF_ASSERT( location.width_grid < 0x7f );

  p_wk->init_line   = location.line_grid;
  p_wk->init_width  = location.width_grid;

	MMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );
	MMDL_OffMoveBitMove( fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�g�ړ��@���C��
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
static void MvRoot2WorkMove( MMDL * fmmdl )
{
	MV_RAIL_RT2_WORK* p_wk = MMDL_GetMoveProcWork( fmmdl );

  switch( p_wk->seq_no )
  {
  ///<���������`�F�b�N
  case MV_RT2_MOVE_SEQ_CHECK_DIR:      
    {
      int dir;
      
      dir = MMDL_GetDirHeader( fmmdl );						//�w�b�_�w�����

      if( p_wk->turn_flag == TRUE ){
        dir = MMDL_TOOL_FlipDir( dir );
      }
      
      MMDL_SetDirAll( fmmdl, dir );

      p_wk->seq_no = MV_RT2_MOVE_SEQ_ACT_SET;
    }
    ///����
    
  ///<����ݒ�
  case MV_RT2_MOVE_SEQ_ACT_SET:        
    {
      RAIL_LOCATION now_location;
      RAIL_LOCATION next_location;

      // ����̃��P�[�V�����擾
      MMDL_GetRailLocation( fmmdl, &now_location );
      
      // ���]���@�߂蒆�Ȃ�A���ɖ߂����`�F�b�N
      if( p_wk->turn_flag ){
        
        if( (p_wk->init_line == now_location.line_grid) && 
            (p_wk->init_width == now_location.width_grid) )
        {
          int dir = MMDL_TOOL_FlipDir( MMDL_GetDirMove(fmmdl) );
          
          MMDL_SetDirAll( fmmdl, dir );

          p_wk->turn_flag = FALSE;
        }
      }

      // �ړ��ݒ�
      {
        int dir, acmd;
        u32 ret;
        BOOL limit;
        BOOL line_change = FALSE;

        dir = MMDL_GetDirMove( fmmdl );

        ret = MMDL_HitCheckRailMoveDir( fmmdl, dir );

        // �ړ��惍�P�[�V�����擾
        MMDL_GetRailDirLocation( fmmdl, dir, &next_location );
        
        // ���[�����~�b�g���v�Z
        limit = MMdl_CheckRailLimit( fmmdl, p_wk->init_line, p_wk->init_width, &next_location );

        // ���C�����ς��Ȃ��悤�Ƀ`�F�b�N
        if( now_location.rail_index != next_location.rail_index )
        {
          line_change = TRUE;
        }

        // ���~�b�g�Ŕ��]
        if( (ret & MMDL_MOVEHITBIT_LIM) || limit || line_change )
        {
          p_wk->turn_flag = TRUE;
          dir = MMDL_TOOL_FlipDir( dir );
          ret = MMDL_HitCheckRailMoveDir( fmmdl, dir );
          MMDL_GetRailDirLocation( fmmdl, dir, &next_location );
        }

        acmd = AC_WALK_U_8F;
        if( ret != MMDL_MOVEHITBIT_NON )
        {
          acmd = AC_STAY_WALK_U_8F;
        }


        acmd = MMDL_ChangeDirAcmdCode( dir, acmd );
        MMDL_SetLocalAcmd( fmmdl, acmd );
      }

      MMDL_OnMoveBitMove( fmmdl );
      p_wk->seq_no = MV_RT2_MOVE_SEQ_ACT_WAIT;
    }
    break;
  ///<����҂�
  case MV_RT2_MOVE_SEQ_ACT_WAIT:       
    if( MMDL_ActionLocalAcmd(fmmdl) == TRUE ){
      MMDL_OffMoveBitMove( fmmdl );
      
      p_wk->seq_no = MV_RT2_MOVE_SEQ_CHECK_DIR;
    }
    break;

  // �G���[
  default:
    GF_ASSERT(0);
    break;
  }
	
  MMdl_RailCommon_Move( &p_wk->rail_wk, fmmdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�g�ړ��@�j��
 *
 *	@param	fmmdl 
 */
//-----------------------------------------------------------------------------
static void MvRoot2WorkDelete( MMDL * fmmdl )
{
	MV_RAIL_RT2_WORK* p_wk = MMDL_GetMoveProcWork( fmmdl );
  MMdl_RailCommon_Delete( &p_wk->rail_wk, fmmdl );
}




