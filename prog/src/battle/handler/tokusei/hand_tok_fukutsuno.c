//=============================================================================================
/**
 * @file  hand_tok_fukutsuno.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[[�Ƃ�����]�F�w�ӂ��̂�����x
 * @author  taya
 *
 * @date  2009.03.11  �쐬
 */
//=============================================================================================
#include "hand_tokusei_common.h"

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_WazaExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );


//--------------------------------------------------------------
/**
 * �n���h���e�[�u��
*/
//--------------------------------------------------------------
static const BtlEventHandlerTable HandlerTable[] = {
  { BTL_EVENT_WAZA_EXECUTE_FAIL, handler_WazaExeFix },
  { BTL_EVENT_NULL, NULL },
};

BTL_EVENT_FACTOR*  HAND_TOK_ADD_Fukutsuno( u16 pri, u16 tokID, u8 pokeID )
{
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}


// ���U�o�����s�m��
static void handler_WazaExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ���U���s���R=�Ђ��, �Ώۃ|�P=�����Ŕ���
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_CAUSE) == SV_WAZAFAIL_SHRINK)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  ){
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    BTL_HANDEX_PARAM_HEADER* header;

    header = (BTL_HANDEX_PARAM_HEADER*)param;
    header->tokwin_flag = TRUE;

    param->rankType = BPP_AGILITY;
    param->rankVolume = 1;
    param->fAlmost = FALSE;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
  }
}

