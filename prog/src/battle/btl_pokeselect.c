//=============================================================================================
/**
 * @file  btl_pokeselect.c
 * @brief �|�P����WB �o�g���V�X�e�� �|�P�����I����ʗp�p�����[�^
 * @author  taya
 *
 * @date  2008.09.25  �쐬
 */
//=============================================================================================

#include "btl_common.h"
#include "btl_main.h"

#include "btl_pokeselect.h"


//=============================================================================================
/**
 * �p�����[�^�\���̂�������
 *
 * @param   param       [out]
 * @param   party       �p�[�e�B�f�[�^�|�C���^
 * @param   numSelect   �I������K�v�̂���|�P������
 * @param   bplMode     �o�g�����X�g��ʃ��[�h
 *
 */
//=============================================================================================
void BTL_POKESELECT_PARAM_Init( BTL_POKESELECT_PARAM* param, const BTL_PARTY* party, u8 numSelect, u8 bplMode )
{
  int i;
  param->party = party;
  param->numSelect = numSelect;
  param->bplMode = bplMode;
  for(i=0; i<NELEMS(param->prohibit); i++)
  {
    param->prohibit[i] = BTL_POKESEL_CANT_NONE;
  }
}
//=============================================================================================
/**
 * ���ɐ퓬�ɏo�Ă���|�P������I�ׂȂ��悤�ɐݒ肷��
 *
 * @param   param       [out]
 * @param   numCover    �퓬�ɏo�Ă���|�P������
 *
 */
//=============================================================================================
void BTL_POKESELECT_PARAM_SetProhibitFighting( BTL_POKESELECT_PARAM* param, u8 numCover )
{
  u8 i;
  for(i=0; i<numCover; i++)
  {
    BTL_POKESELECT_PARAM_SetProhibit( param, BTL_POKESEL_CANT_FIGHTING, i );
  }
}
//=============================================================================================
/**
 * �p�����[�^�\���̂ɑ΂��A�ǉ����Ƃ��ČʂɑI���ł��Ȃ����R��ݒ肷��
 *
 * @param   param
 * @param   reason
 * @param   idx
 *
 */
//=============================================================================================
void BTL_POKESELECT_PARAM_SetProhibit( BTL_POKESELECT_PARAM* param, BtlPokeselReason reason, u8 idx )
{
  GF_ASSERT(idx<NELEMS(param->prohibit));
  param->prohibit[ idx ] = reason;
}
//=============================================================================================
/**
 * �I������ׂ������擾
 *
 * @param   param
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKESELECT_PARAM_GetNumSelect( const BTL_POKESELECT_PARAM* param )
{
  return param->numSelect;
}



//=============================================================================================
/**
 * ���U���g�\���̂�������
 *
 * @param   result    [out]
 *
 */
//=============================================================================================
void BTL_POKESELECT_RESULT_Init( BTL_POKESELECT_RESULT *result, const BTL_POKESELECT_PARAM* param )
{
  result->cnt = 0;
  result->max = param->numSelect;
}
//=============================================================================================
/**
 * ���U���g�\���̂ɑI�����ʂ��P���ǉ�
 *
 * @param   result    [out]
 * @param   idx       �I������
 *
 */
//=============================================================================================
void BTL_POKESELECT_RESULT_Push( BTL_POKESELECT_RESULT *result, u8 idx )
{
  if( result->cnt < result->max )
  {
    result->selIdx[ result->cnt++ ] = idx;
  }
  else
  {
    GF_ASSERT_MSG(0, "cnt=%d, max=%d", result->cnt,result->max);
  }
}
//=============================================================================================
/**
 * ���U���g�\���̂���I�����ʂ��P���폜
 *
 * @param   result    [out]
 *
 */
//=============================================================================================
void BTL_POKESELECT_RESULT_Pop( BTL_POKESELECT_RESULT* result )
{
  if( result->cnt )
  {
    result->cnt--;
  }
}
//=============================================================================================
/**
 * �L�����Z�����ꂽ������
 *
 * @param   result
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_POKESELECT_IsCancel( const BTL_POKESELECT_RESULT* result )
{
  return result->fCancel;
}
//=============================================================================================
/**
 * �I�����I�����������
 *
 * @param   result    ���U���g�\���̃|�C���^
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_POKESELECT_IsDone( const BTL_POKESELECT_RESULT* result )
{
  return result->cnt == result->max;
}

//=============================================================================================
/**
 * �I�񂾐����擾
 *
 * @param   result
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKESELECT_RESULT_GetCount( const BTL_POKESELECT_RESULT* result )
{
  return result->cnt;
}

//=============================================================================================
/**
 * �Ō�ɑI�񂾃|�P�����̕��я���Ԃ�
 *
 * @param   result
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKESELECT_RESULT_GetLast( const BTL_POKESELECT_RESULT* result )
{
  if( result->cnt > 0 )
  {
    return result->selIdx[ result->cnt - 1 ];
  }
  else
  {
    return BTL_PARTY_MEMBER_MAX;
  }
}
//=============================================================================================
/**
 * �I�񂾃|�P�����̕��я���Ԃ�
 *
 * @param   result
 * @param   idx       ���ԖڂɑI�񂾁H
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POKESELECT_RESULT_Get( const BTL_POKESELECT_RESULT* result, u8 idx )
{
  if( idx < result->cnt ){
    return result->selIdx[ idx ];
  }
  return BTL_PARTY_MEMBER_MAX;
}



