//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fldmmdl_railsetup.c
 *	@brief  ���샂�f���@���[���Z�b�g�A�b�v����
 *	@author	tomoya takahshi
 *	@date		2009.10.12
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "fldmmdl.h"

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
#define RAIL_SETUP_DATA_MAGIC_NUMBER  ( 0xefefefef )
//-------------------------------------
///	���[���������\����
//  
//  ����o�b�t�@���g�p���܂��B
//=====================================
typedef struct 
{
  u32 indata;       ///<�f�[�^�����邱�Ƃ�m�点��p�����[�^
  RAIL_LOCATION location;             ///<���[�����P�[�V���� 8bytie
} MV_RAIL_SETUP_WORK;
#define MV_RAIL_SETUP_WORK_SIZE (sizeof(MV_RAIL_SETUP_WORK))		///<MV_RAIL_SETUP�T�C�Y

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static void MMdl_SetSetUpLocation( MMDL* mmdl, const RAIL_LOCATION* location );
  

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���p�@�w�b�_�[���f����    �o�^�����@�O����
 *
 *	@param	mmdl
 *	@param	head 
 */
//-----------------------------------------------------------------------------
void MMDL_SetRailHeaderBefore( MMDL* mmdl, const MMDL_HEADER* head )
{
  GF_ASSERT( head->pos_type == MMDL_HEADER_POSTYPE_RAIL );

 MMDL_OnStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���p�@�w�b�_�[���f����    �o�^�����@�㏈��
 *
 *	@param	mmdl
 *	@param	head 
 */
//-----------------------------------------------------------------------------
void MMDL_SetRailHeaderAfter( MMDL* mmdl, const MMDL_HEADER* head )
{
  const MMDL_HEADER_RAILPOS* cp_railpos;
  MV_RAIL_SETUP_WORK* p_work;
  RAIL_LOCATION location;
  
  GF_ASSERT( head->pos_type == MMDL_HEADER_POSTYPE_RAIL );
  GF_ASSERT( MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) );


  cp_railpos = (const MMDL_HEADER_RAILPOS*)head->pos_buf;

  // ���W�̔��f
  location.rail_index   = cp_railpos->rail_index;
  location.type         = FIELD_RAIL_TYPE_LINE;
  location.key          = FIELD_RAIL_TOOL_ConvertDirToRailKey(head->dir);
  location.line_grid    = cp_railpos->front_grid;
  location.width_grid   = cp_railpos->side_grid;


  // MAP CHANGE��AddMMdl���ꂽ�ꍇ�A
  // MOVE_PROC�̏����������삵�Ȃ��̂ŁA
  // rail_wk���Ȃ��ꍇ������B
  //
  // ���̏ꍇ�@��ʂɕ`�悳���O��Return���Ă΂��
  if( MMDLSYS_CheckStatusBit( MMDL_GetMMdlSys(mmdl), MMDLSYS_STABIT_MOVE_INIT_COMP ) )
  {
    MMDL_SetRailLocation( mmdl, &location );
  }
  else
  {
	  p_work = MMDL_InitMoveProcWork( mmdl, MV_RAIL_SETUP_WORK_SIZE );
    MMdl_SetSetUpLocation( mmdl, &location );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�����Z�[�u�f�[�^�̗L�����`�F�b�N
 *
 *	@param	mmdl  ���f���f�[�^
 *
 *	@retval TRUE  ����
 *	@retval FALSE �Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL MMdl_CheckSetUpLocation( const MMDL* mmdl )
{
  const MV_RAIL_SETUP_WORK* cp_work = MMDL_GetMoveProcWork( (MMDL*)mmdl );
  if( cp_work->indata == RAIL_SETUP_DATA_MAGIC_NUMBER )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�������̕ۑ�
 *
 *	@param	mmdl      ���f���f�[�^
 *	@param	location  ���P�[�V����
 */
//-----------------------------------------------------------------------------
static void MMdl_SetSetUpLocation( MMDL* mmdl, const RAIL_LOCATION* location )
{
  MV_RAIL_SETUP_WORK* p_work = MMDL_GetMoveProcWork( mmdl );

  p_work->location = *location;
  p_work->indata = RAIL_SETUP_DATA_MAGIC_NUMBER;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�������̎擾
 *
 *	@param	mmdl
 *	@param	location 
 *
 *	@retval TRUE    �f�[�^��������
 *	@retval FALSE  �f�[�^���Ȃ�����
 */
//-----------------------------------------------------------------------------
BOOL MMdl_GetSetUpLocation( const MMDL* mmdl, RAIL_LOCATION* location )
{
  const MV_RAIL_SETUP_WORK* cp_work = MMDL_GetMoveProcWork( (MMDL*)mmdl );
  if( cp_work->indata == RAIL_SETUP_DATA_MAGIC_NUMBER )
  {
    *location = cp_work->location;
    return TRUE;
  }
  return FALSE;
}




