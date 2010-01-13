//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc_data.c
 *	@brief  White Forest  Black City ��{�f�[�^ �풓
 *	@author	tomoya takahashi
 *	@date		2009.11.10
 *
 *	���W���[�����FFIELD_WFBC_CORE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "field/field_wfbc_data.h"
#include "field/zonedata.h"


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 day*					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  ZONEDATA��WFBC�̐ݒ���s��
 *
 *	@param	cp_wk   ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SetUpZoneData( const FIELD_WFBC_CORE* cp_wk )
{
  if( cp_wk->type == FIELD_WFBC_CORE_TYPE_WHITE_FOREST )
  {
    ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_BC_WF_ID, TRUE );
  }
  if( cp_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_BC_WF_ID, FALSE );
  }
}


//-------------------------------------
///	FIELD_WFBC_CORE�p�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  WFBC���[�N�̃N���A
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_Clear( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  p_wk->data_in = FALSE;
  p_wk->type    = FIELD_WFBC_CORE_TYPE_MAX;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
  }
}

//-------------------------------------
///	FIELD_WFBC_CORE_PEOPLE�p�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�̃N���A
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_Clear( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  GF_ASSERT( p_wk );

  p_wk->data_in = FALSE;
  p_wk->npc_id  = 0;
  p_wk->mood    = 0;
  p_wk->one_day_msk  = 0;
}





//----------------------------------------------------------------------------
/**
 *	@brief  �A�C�e���z�u��񂷂ׂĂ��肠�@
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
void WFBC_CORE_ITEM_ClaerAll( FIELD_WFBC_CORE_ITEM* p_wk )
{
  int i;
  
  // �A�C�e���Ȃ�
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    p_wk->scr_item[i] = FIELD_WFBC_ITEM_NONE;
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�̃N���A
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_EVENT_Clear( FIELD_WFBC_EVENT* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(FIELD_WFBC_EVENT) );
  p_wk->bc_npc_win_target = FIELD_WFBC_EVENT_NPC_WIN_TARGET_INIT;
}
