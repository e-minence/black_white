//=============================================================================================
/**
 * @file  hand_common.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�n���h�����L�֐��Q
 * @author  taya
 *
 * @date  2010.05.19  �쐬
 */
//=============================================================================================


#include "..\btl_common.h"
#include "..\btl_event_factor.h"


#include "hand_common.h"


/**
 *  �����̃^�[�Q�b�g�|�P����ID���ݒ肳��Ă���n���h���ŁA�w��ID�̃|�P�������^�[�Q�b�g�Ɋ܂܂�邩����
 */
BOOL HandCommon_CheckTargetPokeID( u8 pokeID )
{
  u32 i, target_cnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  for(i=0; i<target_cnt; ++i)
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1+i) == pokeID ){
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  �w��|�P�����̍s�������Ōォ�ǂ�������
 */
BOOL HandCommon_IsPokeOrderLast( BTL_SVFLOW_WORK* flowWk, u8 pokeID )
{
  u8 myOrder, maxOrder;
  if( BTL_SVFTOOL_GetMyActionOrder(flowWk, pokeID, &myOrder, &maxOrder) )
  {
    if( (myOrder+1) == maxOrder ){
      return TRUE;
    }
  }
  return FALSE;
}

/**
 *  ���[����A����̂ǂ��������킹��s�ׂ��֎~����Ă���|�P�̃`�F�b�N
 * �i�쐶��ő���̏ꍇ���`�F�b�N�j
 */
BOOL HandCommon_CheckCantStealPoke( BTL_SVFLOW_WORK* flowWk, u8 pokeID )
{
  if( BTL_SVFTOOL_GetCompetitor(flowWk) == BTL_COMPETITOR_WILD )
  {
    u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );
    if( (clientID == BTL_CLIENT_ENEMY1) || (clientID == BTL_CLIENT_ENEMY1) ){
      return TRUE;
    }
  }
  return FALSE;
}

