//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc.c
 *	@brief  White Forest  Black City 
 *	@author	tomoya takahashi
 *	@date		2009.11.09
 *
 *	���W���[�����FFIELD_WFBC
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "field_wfbc.h"

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
//-------------------------------------
///	�l�����
//=====================================
struct _FIELD_WFBC_PEOPLE 
{
  u8  status;   // FIELD_WFBC_PEOPLE_STATUS
  u8  pad;
  u16 objcode;  // ������
};

//-------------------------------------
///	WFBC���[�N
//=====================================
struct _FIELD_WFBC 
{
  FIELD_WFBC_PEOPLE people[ FIELD_WFBC_PEOPLE_MAX ];
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	FIELD_WFBC
//=====================================
static int WFBC_GetClearPeople( const FIELD_WFBC* cp_wk );
static int WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk );

//-------------------------------------
///	FIELD_WFBC_PEOPLE
//=====================================
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people );


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�p���[�N�̐���
 *
 *	@param	heapID  �q�[�vID
 *
 *	@return WFBC���[�N
 */
//-----------------------------------------------------------------------------
FIELD_WFBC* FIELD_WFBC_Create( HEAPID heapID )
{
  FIELD_WFBC* p_wk;
  int i;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WFBC) );

  //�@�l�����[�N���N���A
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    WFBC_PEOPLE_Clear( &p_wk->people[i] );
  }
  
  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�p���[�N�̔j��
 *
 *	@param	p_wk  ���[�N
 */ 
//-----------------------------------------------------------------------------
void FIELD_WFBC_Delete( FIELD_WFBC* p_wk )
{
  GF_ASSERT( p_wk );

  GFL_HEAP_FreeMemory( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�p�����e�[�u���C���f�b�N�X��Ԃ�
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �����e�[�u���̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk )
{
  GF_ASSERT( cp_wk );

  // �l���𐔂���
  return WFBC_GetPeopleNum( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l�����̎擾
 *
 *	@param	cp_wk   ���[�N
 *	@param	index   �C���f�b�N�X
 *
 *	@return �l�����
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_PEOPLE* FIELD_WFBC_GetPeople( const FIELD_WFBC* cp_wk, int index )
{
  GF_ASSERT( index < FIELD_WFBC_PEOPLE_MAX );
  GF_ASSERT( cp_wk );
  
  return &cp_wk->people[index];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���̒ǉ�
 *
 *	@param	p_wk    ���[�N
 *	@param	objcode �I�u�W�F�R�[�h
 *
 *	@retval �ǉ������C���f�b�N�X
 */
//-----------------------------------------------------------------------------
int FIELD_WFBC_AddPeople( FIELD_WFBC* p_wk, u32 objcode )
{
  FIELD_WFBC_PEOPLE* p_people;
  int index;

  GF_ASSERT( p_wk );

  // �󂢂Ă��郏�[�N���擾
  index = WFBC_GetClearPeople( p_wk );

  p_people          = &p_wk->people[index];
  p_people->status  = FIELD_WFBC_PEOPLE_STATUS_NORMAL;
  p_people->objcode = objcode;

  return index;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���̔j��
 *
 *	@param	p_wk    ���[�N
 *	@param	index   �j������l���̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_DeletePeople( FIELD_WFBC* p_wk, int index )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( index < FIELD_WFBC_PEOPLE_MAX );
  
  // �C���f�b�N�X�̃��[�N���N���[��
  WFBC_PEOPLE_Clear( &p_wk->people[index] );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���@�����ڂ̎擾
 *
 *	@param	cp_people   ���[�N
 *
 *	@return ������
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_PEOPLE_GetOBJCode( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return cp_people->objcode;
}





//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �󂢂Ă���l�����[�N���擾����
 *
 *	@param	cp_wk     ���[�N
 *
 *	@return �󂢂Ă��郏�[�N�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static int WFBC_GetClearPeople( const FIELD_WFBC* cp_wk )
{
  int i;

  GF_ASSERT( cp_wk );

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( cp_wk->people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
    {
      return i;
    }
  }

  GF_ASSERT_MSG( 0, "WFBC People Over\n" );
  return 0; // �������j��Ȃǂ��Ȃ��悤�ɂO��Ԃ�
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l���̐����擾����
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �l���̐�
 */
//-----------------------------------------------------------------------------
static int WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk )
{
  int num;
  int i;
  
  GF_ASSERT( cp_wk );

  num = 0;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( cp_wk->people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
    {
      num ++;
    }
  }

  return num;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�̃N���A
 *
 *	@param	p_people  �l��
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people )
{
  p_people->status  = FIELD_WFBC_PEOPLE_STATUS_NONE;
  p_people->objcode = 0;
}




