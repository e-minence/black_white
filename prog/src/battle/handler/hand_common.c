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


