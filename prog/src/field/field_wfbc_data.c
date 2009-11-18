//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc_data.c
 *	@brief  White Forest  Black City ��{�f�[�^
 *	@author	tomoya takahashi
 *	@date		2009.11.10
 *
 *	���W���[�����FFIELD_WFBC_CORE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "system/gfl_use.h"

#include "field/field_wfbc_data.h"

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

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static u32 WFBC_CORE_GetNotUseNpcID( const FIELD_WFBC_CORE* cp_wk, int idx );
static BOOL WFBC_CORE_IsUseNpc( const FIELD_WFBC_CORE* cp_wk, int npc_id );





//-------------------------------------
///	FIELD_WFBC_CORE�p�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  WFBC���[�N�̃N���A
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_Crear( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  p_wk->data_in = FALSE;
  p_wk->type    = FIELD_WFBC_CORE_TYPE_MAX;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Crear( &p_wk->people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�̏���ݒ肷��
 *
 *	@param	p_wk          ���[�N
 *	@param  cp_mystatus   �܂��X�e�[�^�X
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SetUp( FIELD_WFBC_CORE* p_wk )
{
  int i;
  int npc_idx;
  GF_ASSERT( p_wk );
  
  p_wk->data_in = TRUE;
#if PM_VERSION == VERSION_BLACK
  p_wk->type    = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
#else   // PM_VERSION == VERSION_BLACK
  p_wk->type    = FIELD_WFBC_CORE_TYPE_WHITE_FOREST; 
#endif  // PM_VERSION == VERSION_BLACK

  // �l��ݒ肷��
  for( i=0; i<FIELD_WFBC_INIT_PEOPLE_NUM; i++ )
  {
    npc_idx = GFUser_GetPublicRand(FIELD_WFBC_PEOPLE_MAX - i);
    if( 0 )
    {
      p_wk->people[i].data_in   = TRUE;
      p_wk->people[i].npc_id    = i;
    }
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�f�[�^�̐������`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �s��
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_IsConfomity( const FIELD_WFBC_CORE* cp_wk )
{
  int i;

  GF_ASSERT( cp_wk );
  
  if( (cp_wk->data_in == TRUE) && (cp_wk->type >= FIELD_WFBC_CORE_TYPE_MAX) )
  {
    return FALSE;
  }
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsConfomity( &cp_wk->people[i] ) == FALSE )
    {
      return FALSE;
    }
  }

  // OK
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^����  �s���f�[�^�̏ꍇ�́A����ȏ��ɏ��������܂��B
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_Management( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  // �ꉞ�f�[�^���Ȃ���Ԃ��Ǘ����Ă���
  if( (p_wk->data_in == FALSE) && (p_wk->type != FIELD_WFBC_CORE_TYPE_MAX) )
  {
    p_wk->type = FIELD_WFBC_CORE_TYPE_MAX;
  }

  // �X�^�C�v�̒���
  if( (p_wk->data_in == TRUE) && (p_wk->type >= FIELD_WFBC_CORE_TYPE_MAX) )
  {
    // @TODO �������ʐM�����ROM�R�[�h�ɍ��킹��ׂ�
    p_wk->type = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
  }

  // �l�����̒���
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Management( &p_wk->people[i] );
  }
}

// �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�̗L���`�F�b�N  �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE  �f�[�^����
 *	@retval FALSE �f�[�^�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_IsInData( const FIELD_WFBC_CORE* cp_wk )
{
  GF_ASSERT( cp_wk );
  
  if( !cp_wk->data_in )
  {
    return FALSE;
  }

  if( FIELD_WFBC_CORE_IsConfomity( cp_wk ) == FALSE )
  {
    return FALSE;
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�̐���Ԃ�
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �l�̐�
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_GetPeopleNum( const FIELD_WFBC_CORE* cp_wk )
{
  int i;
  int num;
  GF_ASSERT( cp_wk );


  num = 0;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->people[i] ) )
    {
      num ++;
    }
  }
  return num;
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
void FIELD_WFBC_CORE_PEOPLE_Crear( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  GF_ASSERT( p_wk );

  p_wk->data_in = FALSE;
  p_wk->npc_id  = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̐������`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �s��
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsConfomity( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^����  �s���f�[�^�̏ꍇ�́A����ȏ��ɏ��������܂��B
 *
 *	@param	p_wk  �l�����[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_Management( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  GF_ASSERT( p_wk );

  //
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
 *
 *	@param	cp_wk   �l�����[�N
 *
 *	@retval TRUE    �f�[�^����
 *	@retval FALSE   �f�[�^�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsInData( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  
  if( !cp_wk->data_in )
  {
    return FALSE;
  }
  if( FIELD_WFBC_CORE_PEOPLE_IsConfomity( cp_wk ) == FALSE )
  {
    return FALSE;
  }

  return TRUE;
}






//-----------------------------------------------------------------------------
/**
 *        private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �܂��A�l���f�[�^�Ƃ��ĕێ����Ă��Ȃ��l����IDX�Ԗڂ��擾����
 *
 *	@param	cp_wk     ���[�N
 *	@param	idx       �C���f�b�N�X
 */ 
//-----------------------------------------------------------------------------
static u32 WFBC_CORE_GetNotUseNpcID( const FIELD_WFBC_CORE* cp_wk, int idx )
{
#if 0
  int i;

  for(  )
  {
  }
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brieif NPC_ID�����[�N���̐l���ɂ���̂��`�F�b�N
 *
 *	@param	cp_wk     ���[�N
 *	@param	npc_id    NPCID
 *
 *	@retval TRUE  ����
 *	@retval FALSE ���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_IsUseNpc( const FIELD_WFBC_CORE* cp_wk, int npc_id )
{
#if 0
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
  }
#endif
}


