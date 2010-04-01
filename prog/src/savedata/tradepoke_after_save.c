//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		tradepoke_after_save.c
 *	@brief  �|�P���������@���̌�@�Z�[�u�f�[�^
 *	@author	tomoya takahashi
 *	@date		2010.03.26
 *
 *	���W���[�����FTRPOKE_AFTER_SAVE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "savedata/tradepoke_after_save.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
#define SPEABINO_THREE_NONE (0xff)  // ��R�����Ȃ��@��`

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	1�f�[�^
//=====================================
typedef struct {
  u32 personal_rnd;
	STRCODE	nickname[MONS_NAME_SIZE+EOM_SIZE];	//16h	�j�b�N�l�[��(MONS_NAME_SIZE=10)+(EOM_SIZE=1)=11
  u8  indata:1;
  u8  level:7;
  u8 speabino;
  u32 id;
} TRPOKE_AFTER_DATA;


//-------------------------------------
///	�����|�P�����@���̌�@�Z�[�u���[�N
//=====================================
struct _TRPOKE_AFTER_SAVE 
{
  TRPOKE_AFTER_DATA data[TRPOKE_AFTER_SAVE_TYPE_MAX];
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
int TRPOKE_AFTER_SV_GetWorkSize(void)
{
  return sizeof(TRPOKE_AFTER_SAVE);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������m��
 */
//-----------------------------------------------------------------------------
TRPOKE_AFTER_SAVE* TRPOKE_AFTER_SV_AllocWork(HEAPID heapID)
{
  TRPOKE_AFTER_SAVE* sv;
  sv = GFL_HEAP_AllocClearMemory( heapID, sizeof(TRPOKE_AFTER_SAVE) );
  return sv;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������
 */
//-----------------------------------------------------------------------------
void TRPOKE_AFTER_SV_Init(TRPOKE_AFTER_SAVE* sv)
{
  GFL_STD_MemClear( sv, sizeof(TRPOKE_AFTER_SAVE) );
}


//----------------------------------------------------------
//	�f�[�^�A�N�Z�X
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�L��
 *
 *	@param	sv      �Z�[�u
 *	@param	type    �^�C�v
 *
 *	@retval TRUE    �Z�[�u����
 *	@retval FALSE   �Z�[�u�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL TRPOKE_AFTER_SV_IsData( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[ type ].indata;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���x�����擾
 *
 *	@param	sv    ���[�N
 *	@param	type  �^�C�v
 *
 *	@return ���x��
 */
//-----------------------------------------------------------------------------
u8 TRPOKE_AFTER_SV_GetLevel( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  )
{
  u8 ret;
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  ret = sv->data[ type ].level + TRPOKE_AFTER_LEVEL_ADD;
  if( ret > TRPOKE_AFTER_LEVEL_MAX ){
    ret = TRPOKE_AFTER_LEVEL_MAX;
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��R�Ƃ����������邩�`�F�b�N
 *
 *	@param	sv      ���[�N
 *	@param	type    �^�C�v
 *
 *	@retval TRUE  ����
 *	@retval FALSE �Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL TRPOKE_AFTER_SV_IsSpeabino3( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  if( sv->data[ type ].speabino == SPEABINO_THREE_NONE )
  {
    return FALSE;
  }
  return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��R�Ƃ��������擾
 *
 *	@param	sv      ���[�N
 *	@param	type    �^�C�v
 *
 *	@return ����
 */
//-----------------------------------------------------------------------------
u8 TRPOKE_AFTER_SV_GetSpeabino3( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[ type ].speabino;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ő̗������擾
 *
 *	@param	sv      ���[�N
 *	@param	type    �^�C�v
 *
 *	@return �ő̗���
 */
//-----------------------------------------------------------------------------
u32 TRPOKE_AFTER_SV_GetRnd( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[ type ].personal_rnd;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j�b�N�l�[�����擾
 *
 *	@param	sv      ���[�N
 *	@param	type    �^�C�v
 *	@param	str     ������i�[��
 */
//-----------------------------------------------------------------------------
const STRCODE* TRPOKE_AFTER_SV_GetNickName( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[type].nickname;
}

//----------------------------------------------------------------------------
/**
 *	@brief  IDno�̎擾
 *
 *	@param	sv
 *	@param	type  
 *
 *	@return ID�@no
 */
//-----------------------------------------------------------------------------
u32 TRPOKE_AFTER_SV_GetID( const TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type  )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );
  return sv->data[type].id;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �|�P��������ݒ�
 *
 *	@param	sv      ���[�N
 *	@param	type    �^�C�v
 *	@param	pp      �|�P�������
 */
//-----------------------------------------------------------------------------
void TRPOKE_AFTER_SV_SetData( TRPOKE_AFTER_SAVE* sv, TRPOKE_AFTER_SAVE_TYPE type, const POKEMON_PARAM * pp )
{
  GF_ASSERT( type < TRPOKE_AFTER_SAVE_TYPE_MAX );

  sv->data[type].level = PP_Get( pp, ID_PARA_level, NULL );
  if( PP_Get( pp, ID_PARA_tokusei_3_flag, NULL ) ){
    sv->data[type].speabino = PP_Get( pp, ID_PARA_speabino, NULL );
  }else{
    sv->data[type].speabino = SPEABINO_THREE_NONE;
  }
  sv->data[type].personal_rnd = PP_Get( pp, ID_PARA_personal_rnd, NULL );
  sv->data[type].id = PP_Get( pp, ID_PARA_id_no, NULL );
  PP_Get( pp, ID_PARA_nickname_raw, sv->data[type].nickname );
  sv->data[type].indata = TRUE;
}



