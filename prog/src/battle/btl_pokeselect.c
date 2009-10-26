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
 * @param   aliveOnly   �����Ă���|�P���������I�������Ȃ��ꍇTRUE
 *
 */
//=============================================================================================
void BTL_POKESELECT_PARAM_Init( BTL_POKESELECT_PARAM* param, const BTL_PARTY* party, u8 numSelect, u8 aliveOnly )
{
  int i;
  param->party = party;
  param->numSelect = numSelect;
  param->aliveOnly = aliveOnly;
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
    GF_ASSERT(0);
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



//=============================================================================================
/**
 * �I�����Ă��ǂ��|�P�������ǂ�������
 *
 * @param   param     [in]
 * @param   result    [in]
 * @param   idx       �I���|�P�����C���f�b�N�X
 *
 * @retval  BtlPokeselReason    �s���R�i�s�łȂ���� BTL_POKESEL_CANT_NONE�j
 */
//=============================================================================================
BtlPokeselReason BTL_POKESELECT_CheckProhibit( const BTL_POKESELECT_PARAM* param, const BTL_POKESELECT_RESULT* result, u8 idx )
{
  // �����Ă�|�P�����I�ׂȂ��ꍇ�̃`�F�b�N
  if( param->aliveOnly )
  {
    const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( param->party, idx );

    if( BPP_IsDead(bpp) )
    {
      return BTL_POKESEL_CANT_DEAD;
    }
  }

  // �ʗ��R�`�F�b�N
  if( param->prohibit[idx] != BTL_POKESEL_CANT_NONE )
  {
    return param->prohibit[idx];
  }

  // ���ɑI�񂾃|�P�͑I�ׂȂ�
  {
    u8 i;
    for(i=0; i<result->cnt; i++)
    {
      if( result->selIdx[i] == idx )
      {
        return BTL_POKESEL_CANT_SELECTED;
      }
    }
  }

  return BTL_POKESEL_CANT_NONE;
}
