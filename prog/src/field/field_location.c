//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_location.c
 *	@brief
 *  @author  tamada GAME FREAK inc.
 *	@date		2009.08.07
 *
 *	���P�[�V�����p�̊֐���rail����ڐA  tomoya takahashi
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "gflib.h"
#include "field/rail_location.h"
#include "field/field_dir.h"

#include "field_rail.h"

//============================================================================================
//============================================================================================

//============================================================================================
//
//
//      DIR��RAIL_KEY�̑Ή��f�[�^
//
//
//============================================================================================
static const u8 sc_DIR_TO_RAILKEY[DIR_MAX4] = 
{
  RAIL_KEY_UP,
  RAIL_KEY_DOWN,
  RAIL_KEY_LEFT,
  RAIL_KEY_RIGHT,
};
//------------------------------------------------------------------
//------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief RAIL_LOCATION�\���̂̏���������
 */
//------------------------------------------------------------------
void RAIL_LOCATION_Init(RAIL_LOCATION * railLoc)
{
  railLoc->type = FIELD_RAIL_TYPE_POINT;
  railLoc->rail_index = 0;
  railLoc->line_grid = 0;
  railLoc->width_grid = 0;
  railLoc->key = RAIL_KEY_NULL;
}


//------------------------------------------------------------------
// RAIL_KEY��DIR�̕ϊ�
//  
//  DIR_�`��RAIL_KEY_�`�ɕϊ�
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  RAIL_KEY��DIR�ɕϊ�
 *
 *	@param	key �L�[
 *
 *	@return�@DIR
 */
//-----------------------------------------------------------------------------
u32 FIELD_RAIL_TOOL_ConvertRailKeyToDir( u32 key )
{
  int i;

  GF_ASSERT( key < RAIL_KEY_MAX );

  for( i=0; i<DIR_MAX4; i++ )
  {
    if( sc_DIR_TO_RAILKEY[i] == key )
    {
      return i;
    }
  }

  GF_ASSERT( 0 );
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  DIR��RAIL_KEY�ɕϊ�
 *
 *	@param	dir   DIR
 *
 *	@return RAIL_KEY
 */
//-----------------------------------------------------------------------------
u32 FIELD_RAIL_TOOL_ConvertDirToRailKey( u32 dir )
{
  // �Ή����Ă���̂�
  // DIR_UP DIR_DOWN DIR_LEFT DIR_RIGHT
  GF_ASSERT( dir < DIR_MAX4 );

  return sc_DIR_TO_RAILKEY[dir];
}
