//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		livematch_btlscore_data.c
 *	@brief  ���C�u�}�b�`��уZ�[�u�f�[�^
 *	@author	Toru=Nagihashi
 *	@data		2010.03.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�Z�[�u�f�[�^
#include "savedata/save_tbl.h"

//����
#include "battlematch_savedata_def.h"

//�O�����J
#include "savedata/livematch_savedata.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//----------------------------------------------------------
//�Z�[�u�f�[�^�V�X�e���Ɉˑ�����֐�
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �\���̃T�C�Y�擾
 *
 *	@return ���[�N�T�C�Y
 */
//-----------------------------------------------------------------------------
int LIVEMATCH_GetWorkSize( void )
{
  return sizeof( LIVEMATCH_DATA );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u�f�[�^������
 *
 *	@param	LIVEMATCH_DATA *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void LIVEMATCH_Init( LIVEMATCH_DATA *p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(LIVEMATCH_DATA) );
  OS_GetMacAddress( p_wk->mac_addr );
}


//----------------------------------------------------------
//�擾�A���삷��֐�
//----------------------------------------------------------
//�����Ɋ֌W����f�[�^
//�擾
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃}�b�N�A�h���X�󂯎��
 *
 *	@param	const LIVEMATCH_DATA *cp_wk   ���[�N
 *	@param	p_mac_addr                    �󂯎��iu8[6]�j
 */
//-----------------------------------------------------------------------------
void LIVEMATCH_DATA_GetMyMacAddr( const LIVEMATCH_DATA *cp_wk, u8* p_mac_addr )
{
  GFL_STD_MemCopy( cp_wk->mac_addr, p_mac_addr, sizeof(u8)*6 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̏��󂯎��
 *
 *	@param	const LIVEMATCH_DATA *cp_wk ���[�N
 *	@param	param                       �󂯎����C���f�b�N�X
 *
 *	@return ���
 */
//-----------------------------------------------------------------------------
int LIVEMATCH_DATA_GetMyParam( const LIVEMATCH_DATA *cp_wk, LIVEMATCH_MYDATA_PARAM param )
{
  GF_ASSERT( param < LIVEMATCH_MYDATA_PARAM_MAX );
  return cp_wk->param[ param ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̏��ݒ�
 *
 *	@param	const LIVEMATCH_DATA *cp_wk ���[�N
 *	@param	param                       �󂯎����C���f�b�N�X
 *	@param	data                        �f�[�^
 */
//-----------------------------------------------------------------------------
void LIVEMATCH_DATA_SetMyParam( LIVEMATCH_DATA *p_wk, LIVEMATCH_MYDATA_PARAM param, int data )
{
  GF_ASSERT( param < LIVEMATCH_MYDATA_PARAM_MAX );
  p_wk->param[ param ]  = data;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̏��֑�������
 *
 *	@param	const LIVEMATCH_DATA *cp_wk ���[�N
 *	@param	param                       �󂯎����C���f�b�N�X
 *	@param	data                        �������ޒl
 */
//-----------------------------------------------------------------------------
void LIVEMATCH_DATA_AddMyParam( LIVEMATCH_DATA *p_wk, LIVEMATCH_MYDATA_PARAM param, int data )
{
  GF_ASSERT( param < LIVEMATCH_MYDATA_PARAM_MAX );
  p_wk->param[ param ]  += data;
}


//����Ɋ֌W����f�[�^
//�擾
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ푊��̃}�b�N�A�h���X���擾
 *
 *	@param	const LIVEMATCH_DATA *cp_wk ���[�N
 *	@param	index                       �ΐ푊��C���f�b�N�X
 *	@param	p_mac_addr                  �󂯎��iu8[6]�j
 */
//-----------------------------------------------------------------------------
void LIVEMATCH_DATA_GetFoeMacAddr( const LIVEMATCH_DATA *cp_wk, u32 index, u8* p_mac_addr )
{
  const LIVEMATCH_FOEDATA *cp_data;

  //��荞��
  index %= LIVEMATCH_FOEDATA_MAX;

  cp_data  = &cp_wk->foe[ index ];

  GFL_STD_MemCopy( cp_data->mac_addr, p_mac_addr, sizeof(u8)*6 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ푊��̏����擾
 *
 *	@param	const LIVEMATCH_DATA *cp_wk ���[�N
 *	@param	index                       �ΐ푊��̃C���f�b�N�X
 *	@param	param                       �󂯎����C���f�b�N�X
 *
 *	@return ���
 */
//-----------------------------------------------------------------------------
int LIVEMATCH_DATA_GetFoeParam( const LIVEMATCH_DATA *cp_wk, u32 index, LIVEMATCH_FOEDATA_PARAM param )
{
  const LIVEMATCH_FOEDATA *cp_data;

  GF_ASSERT( param < LIVEMATCH_FOEDATA_PARAM_MAX );

  //��荞��
  index %= LIVEMATCH_FOEDATA_MAX;

  cp_data  = &cp_wk->foe[ index ];
  return cp_data->param[ param ];
}

//�ݒ�
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ푊��̃}�b�N�A�h���X��ݒ�
 *
 *	@param	LIVEMATCH_DATA *p_wk  ���[�N
 *	@param	index                 �ΐ푊��̃C���f�b�N�X
 *	@param	u8* cp_mac_addr       �ݒ肷��}�b�N�A�h���X�iu8[6]�j
 */
//-----------------------------------------------------------------------------
void LIVEMATCH_DATA_SetFoeMacAddr( LIVEMATCH_DATA *p_wk, u32 index, const u8* cp_mac_addr )
{
  LIVEMATCH_FOEDATA *p_data;

  //��荞��
  index %= LIVEMATCH_FOEDATA_MAX;

  p_data  = &p_wk->foe[ index ];

  GFL_STD_MemCopy( cp_mac_addr, p_data->mac_addr, sizeof(u8)*6 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ푊��̏���ݒ�
 *
 *	@param	LIVEMATCH_DATA *p_wk  ���[�N
 *	@param	index                 �ΐ푊��̃C���f�b�N�X
 *	@param	param                 �ݒ肷����C���f�b�N�X
 *	@param	data                  �Ł[��
 */
//-----------------------------------------------------------------------------
void LIVEMATCH_DATA_SetFoeParam( LIVEMATCH_DATA *p_wk, u32 index, LIVEMATCH_FOEDATA_PARAM param, int data )
{
  LIVEMATCH_FOEDATA *p_data;

  GF_ASSERT( param < LIVEMATCH_FOEDATA_PARAM_MAX );

  //��荞��
  index %= LIVEMATCH_FOEDATA_MAX;

  p_data  = &p_wk->foe[ index ];
  p_data->param[ param ]  = data;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����}�b�N�A�h���X�����邩�`�F�b�N
 *
 *	@param	const LIVEMATCH_DATA *cp_wk ���[�N
 *	@param	*p_mac_addr   ���ׂ�}�b�N�A�h���X
 *
 *	@return TRUE�������̂�����  FALSE�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL LIVEMATCH_DATA_IsSameMacAddress( const LIVEMATCH_DATA *cp_wk, const u8 *cp_mac_addr )
{ 
  int max = LIVEMATCH_DATA_GetMyParam( cp_wk, LIVEMATCH_MYDATA_PARAM_BTLCNT );

  int i;
  u8  foe_mac_addr[6];

  max =  MATH_IMin( max, LIVEMATCH_FOEDATA_MAX );
  for( i = 0; i < max; i++ )
  { 
    LIVEMATCH_DATA_GetFoeMacAddr( cp_wk, i, foe_mac_addr );

    if( GFL_STD_MemComp( cp_mac_addr, foe_mac_addr, sizeof(u8)*6 ) == 0 )
    { 
      return TRUE;
    }
  }

  return FALSE;
}
