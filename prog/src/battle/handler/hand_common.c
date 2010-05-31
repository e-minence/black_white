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


/**
 *  �}�W�b�N�R�[�g�i���肪�g���T�C�h�G�t�F�N�g�Ń}�W�b�N�R�[�g�Ώۃ��U�͎��s
 */
void HandCommon_MagicCoat_CheckSideEffWaza( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  u8 wazaUserPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( !BTL_MAINUTIL_IsFriendClientID(wazaUserPokeID, pokeID) )
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( (WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_SIDE_EFFECT)
    &&  (WAZADATA_GetFlag(waza, WAZAFLAG_MagicCoat))
    ){
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_NO_REACTION) ){
        BTL_SVFRET_AddMagicCoatAction( flowWk, pokeID, wazaUserPokeID );
      }
    }
  }
}
/**
 *  �}�W�b�N�R�[�g�i���������U�ΏۂȂ疳�����j
 */
void HandCommon_MagicCoat_Wait( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_MAGICCOAT_FLAG) == FALSE)
  ){
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );

    if( WAZADATA_GetFlag(waza, WAZAFLAG_MagicCoat) )
    {
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, TRUE) )
      {
        // �}�W�b�N�R�[�g�����Ăяo��
        u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
        BTL_SVFRET_AddMagicCoatAction( flowWk, pokeID, atkPokeID );

        // ���̎��_�Ń��A�N�V�����Ȃ�
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
      }
    }
  }
}
/**
 *  �}�W�b�N�R�[�g�i���˕Ԃ��m�蔽���j
 */
void HandCommon_MagicCoat_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MagicCoatExe );
    HANDEX_STR_AddArg( &param->str, pokeID );
    HANDEX_STR_AddArg( &param->str, BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
  }
}



