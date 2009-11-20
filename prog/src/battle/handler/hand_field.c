//=============================================================================================
/**
 * @file  hand_field.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [�t�B�[���h�G�t�F�N�g]
 * @author  taya
 *
 * @date  2009.06.18  �쐬
 */
//=============================================================================================

#include "poke_tool\poketype.h"
#include "battle\battle.h"

#include "..\btl_common.h"
#include "..\btl_calc.h"
#include "..\btl_field.h"
#include "..\btl_client.h"
#include "..\btl_event_factor.h"

#include "hand_field.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_CONT = EVENT_HANDLER_WORK_ELEMS - 1,
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR* ADD_Fld_Weather( u16 pri, BtlFieldEffect effect, u8 subParam );
static void handler_fld_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work );
static BTL_EVENT_FACTOR* ADD_Fld_TrickRoom( u16 pri, BtlFieldEffect effect, u8 subParam );
static void handler_fld_TrickRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work );
static BTL_EVENT_FACTOR* ADD_Fld_Juryoku( u16 pri, BtlFieldEffect effect, u8 subParam );
static void handler_fld_Jyuryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work );
static BTL_EVENT_FACTOR* ADD_Fld_Fuin( u16 pri, BtlFieldEffect effect, u8 subParam );
static BTL_EVENT_FACTOR* ADD_Fld_Sawagu( u16 pri, BtlFieldEffect effect, u8 subParam );
static BTL_EVENT_FACTOR* ADD_Fld_MizuAsobi( u16 pri, BtlFieldEffect effect, u8 subParam );
static void handler_fld_MizuAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work );
static BTL_EVENT_FACTOR* ADD_Fld_DoroAsobi( u16 pri, BtlFieldEffect effect, u8 subParam );
static void handler_fld_DoroAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work );
static BTL_EVENT_FACTOR* ADD_Fld_WonderRoom( u16 pri, BtlFieldEffect effect, u8 subParam );
static void handler_fld_WonderRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work );




//=============================================================================================
/**
 * �t�B�[���h�G�t�F�N�g�n���h�����V�X�e���ɒǉ�
 *
 * @param   effect      �G�t�F�N�g�^�C�v
 * @param   sub_param   �G�t�F�N�g�T�u�p�����[�^�i����A�V��̎�ނ��w�肷��̂݁j
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  BTL_HANDLER_FLD_Add( BtlFieldEffect effect, u8 sub_param )
{
  typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, BtlFieldEffect effect, u8 subParam );

  static const struct {
    BtlFieldEffect eff;
    pEventAddFunc  func;
  }funcTbl[] = {
    { BTL_FLDEFF_WEATHER,    ADD_Fld_Weather     }, ///< �e��V��
    { BTL_FLDEFF_TRICKROOM,  ADD_Fld_TrickRoom   }, ///< �g���b�N���[��
    { BTL_FLDEFF_JURYOKU,    ADD_Fld_Juryoku     }, ///< ���イ��傭
    { BTL_FLDEFF_FUIN ,      ADD_Fld_Fuin        }, ///< �ӂ�����
    { BTL_FLDEFF_MIZUASOBI,  ADD_Fld_MizuAsobi   }, ///< �݂�������
    { BTL_FLDEFF_DOROASOBI,  ADD_Fld_DoroAsobi   }, ///< �ǂ날����
    { BTL_FLDEFF_WONDERROOM, ADD_Fld_WonderRoom  }, ///< �����_�[���[��
  };

  {
    u32 i;
    for(i=0; i<NELEMS(funcTbl); ++i)
    {
      if( funcTbl[i].eff == effect )
      {
        return funcTbl[i].func( 0, effect, sub_param );
      }
    }
  }
  return NULL;
}

//=============================================================================================
/**
 * �n���h�����폜
 *
 * @param   factor
 */
//=============================================================================================
void BTL_HANDLER_FLD_Remove( BTL_EVENT_FACTOR* factor )
{
  BTL_EVENT_FACTOR_Remove( factor );
}


//--------------------------------------------------------------------------------------
/**
 *  �V��
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Fld_Weather( u16 pri, BtlFieldEffect effect, u8 subParam )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,  handler_fld_Weather   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_FIELD, effect, pri, subParam, HandlerTable );
}
static void handler_fld_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work )
{

}
//--------------------------------------------------------------------------------------
/**
 *  �g���b�N���[��
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Fld_TrickRoom( u16 pri, BtlFieldEffect effect, u8 subParam )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,  handler_fld_TrickRoom   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_FIELD, effect, pri, subParam, HandlerTable );
}
static void handler_fld_TrickRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work )
{
  u16 agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
  agi = BTL_CALC_AGILITY_MAX - agi;
  BTL_EVENTVAR_RewriteValue( BTL_EVAR_AGILITY, agi );
}
//--------------------------------------------------------------------------------------
/**
 *  ���イ��傭
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Fld_Juryoku( u16 pri, BtlFieldEffect effect, u8 subParam )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,  handler_fld_Jyuryoku   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_FIELD, effect, pri, subParam, HandlerTable );
}
static void handler_fld_Jyuryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work )
{
  BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.67) );
}
//--------------------------------------------------------------------------------------
/**
 *  �ӂ�����
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Fld_Fuin( u16 pri, BtlFieldEffect effect, u8 subParam )
{
  // ���̂Ƃ���A�����n���h����p�ӂ���K�v�������Ȃ��Ă��܂����c
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_FIELD, effect, pri, subParam, HandlerTable );
}
//--------------------------------------------------------------------------------------
/**
 *  ���킮
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Fld_Sawagu( u16 pri, BtlFieldEffect effect, u8 subParam )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,  handler_fld_Weather   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_FIELD, effect, pri, subParam, HandlerTable );
}
//--------------------------------------------------------------------------------------
/**
 *  �݂�������
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Fld_MizuAsobi( u16 pri, BtlFieldEffect effect, u8 subParam )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,  handler_fld_MizuAsobi   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_FIELD, effect, pri, subParam, HandlerTable );
}
static void handler_fld_MizuAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_MIZU ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.5) );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �ǂ날����
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Fld_DoroAsobi( u16 pri, BtlFieldEffect effect, u8 subParam )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,  handler_fld_DoroAsobi   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_FIELD, effect, pri, subParam, HandlerTable );
}
static void handler_fld_DoroAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_DENKI ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.5) );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �����_�[���[��
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Fld_WonderRoom( u16 pri, BtlFieldEffect effect, u8 subParam )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD_PREV,  handler_fld_WonderRoom   },  // �h�䑤�\�͒l�v�Z�̑O
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_FIELD, effect, pri, subParam, HandlerTable );
}
static void handler_fld_WonderRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 subParam, int* work )
{
  int swap_cnt = BTL_EVENTVAR_GetValue( BTL_EVAR_VID_SWAP_CNT);
  BTL_EVENTVAR_RewriteValue( BTL_EVAR_VID_SWAP_CNT, (swap_cnt+1) );
}


