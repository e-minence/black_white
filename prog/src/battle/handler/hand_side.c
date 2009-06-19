//=============================================================================================
/**
 * @file  hand_side.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [�T�C�h�G�t�F�N�g]
 * @author  taya
 *
 * @date  2009.06.18  �쐬
 */
//=============================================================================================

#include "poke_tool\poketype.h"
#include "poke_tool\monsno_def.h"

#include "..\btl_common.h"
#include "..\btl_calc.h"
#include "..\btl_field.h"
#include "..\btl_client.h"
#include "..\btl_event_factor.h"

#include "hand_side.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_CONT = EVENT_HANDLER_WORK_ELEMS - 1,
};


/**
 *  �d���Ǘ��p���[�N
 */
typedef struct {

  BPP_SICK_CONT  contParam;
  u16            counter;
  u8             enableFlag;

}EXIST_EFFECT;

static EXIST_EFFECT ExistEffect[ BTL_SIDE_MAX ][ BTL_SIDEEFF_MAX ];


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR* ADD_Refrector( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_Refrector( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 side, int* work );
static BTL_EVENT_FACTOR* ADD_Hikarinokabe( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_Sinpinomamori( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_SinpiNoMamori_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static void handler_SinpiNoMamori_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_Siroikiri( u16 pri, BtlSide side, BtlSideEffect eff );
static BTL_EVENT_FACTOR* ADD_Makibisi( u16 pri, BtlSide side, BtlSideEffect eff );
static BTL_EVENT_FACTOR* ADD_Dokubisi( u16 pri, BtlSide side, BtlSideEffect eff );




//=============================================================================================
/**
 * �V�X�e��������
 *
 * @param   none
 */
//=============================================================================================
void BTL_HANDLER_SIDE_InitSystem( void )
{
  GFL_STD_MemClear( ExistEffect, sizeof(ExistEffect) );
}

//=============================================================================================
/**
 * �T�C�h�G�t�F�N�g�n���h�����V�X�e���ɒǉ�
 *
 * @param   side
 * @param   sideEffect
 * @param   contParam
 *
 * @retval  BTL_EVENT_FACTOR*   �ǉ����ꂽ�C�x���g�n���h���i�d�����Ēǉ��ł��Ȃ��ꍇNULL�j
 */
//=============================================================================================
BTL_EVENT_FACTOR*  BTL_HANDLER_SIDE_Add( BtlSide side, BtlSideEffect sideEffect, BPP_SICK_CONT contParam )
{
  typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, BtlSide side, BtlSideEffect eff );

  static const struct {
    BtlSideEffect  eff;
    pEventAddFunc  func;
  }funcTbl[] = {
    { BTL_SIDEEFF_REFRECTOR,      ADD_Refrector     },
    { BTL_SIDEEFF_HIKARINOKABE,   ADD_Hikarinokabe  },
    { BTL_SIDEEFF_SINPINOMAMORI,  ADD_Sinpinomamori },
    { BTL_SIDEEFF_SIROIKIRI,      ADD_Siroikiri     },
    { BTL_SIDEEFF_MAKIBISI,       ADD_Makibisi      },
    { BTL_SIDEEFF_DOKUBISI,       ADD_Dokubisi      },
  };

  GF_ASSERT(side < BTL_SIDE_MAX);
  GF_ASSERT(sideEffect < BTL_SIDEEFF_MAX);

  if( ExistEffect[ side ][ sideEffect ].enableFlag == FALSE )
  {
    u32 i;
    for(i=0; i<NELEMS(funcTbl); ++i)
    {
      if( funcTbl[i].eff == sideEffect )
      {
        BTL_EVENT_FACTOR* factor = funcTbl[i].func( 0, side, sideEffect );
        BTL_EVENT_FACTOR_SetWorkValue( factor, WORKIDX_CONT, contParam.raw );

        ExistEffect[ side ][ sideEffect ].enableFlag = TRUE;
        ExistEffect[ side ][ sideEffect ].counter = 0;
        ExistEffect[ side ][ sideEffect ].contParam = contParam;
        return factor;
      }
    }
  }
  return NULL;
}

void BTL_HANDLER_SIDE_TurnCheck( pSideEffEndCallBack callBack, void* callbackArg )
{
  u32 side, i;
  for(side = 0; side < BTL_SIDE_MAX; ++side)
  {
    for(i=0; i<BTL_SIDEEFF_MAX; ++i)
    {
      if( ExistEffect[side][i].enableFlag )
      {
        EXIST_EFFECT* eff = &ExistEffect[ side ][ i ];
        if( eff->contParam.type == WAZASICK_CONT_TURN )
        {
          if( ++(eff->counter) >= eff->contParam.turn.count )
          {
            eff->enableFlag = FALSE;
            eff->contParam.type = WAZASICK_CONT_NONE;
            callBack( side, i, callbackArg );
          }
        }
      }
    }
  }
}






//--------------------------------------------------------------------------------------
/**
 *  ���t���N�^�[
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Refrector( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,  handler_Refrector   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_Refrector( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 side, int* work )
{
  u8 defPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( BTL_MAINUTIL_PokeIDtoSide(defPokeID) == side )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_PHYSIC )
    {
      BTL_Printf("���t���N�^�[�ł͂�Ԃ��\n");
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5f) );
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �Ђ���̂���
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Hikarinokabe( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,  handler_HikariNoKabe   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 defPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( BTL_MAINUTIL_PokeIDtoSide(defPokeID) == mySide )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_SPECIAL )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5f) );
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ����҂̂܂���
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Sinpinomamori( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,  handler_SinpiNoMamori_CheckFail   },  // ��Ԉُ편�s�`�F�b�N
    { BTL_EVENT_ADDSICK_FAILED,     handler_SinpiNoMamori_FixFail     },  // ���s�m��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_SinpiNoMamori_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) < POKESICK_MAX)
  ){
    work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
    if( work[0] ){
      BTL_Printf("����҂̂܂���Ŗh����\n");
    }
  }
}
static void handler_SinpiNoMamori_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  if( work[0] )
  {
    u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
    param->pokeID = pokeID;
    param->strID = BTL_STRID_SET_SinpiNoMamori_Exe;
    work[0] = 0;
  }
}


static BTL_EVENT_FACTOR* ADD_Siroikiri( u16 pri, BtlSide side, BtlSideEffect eff )
{
  return NULL;
}
static BTL_EVENT_FACTOR* ADD_Makibisi( u16 pri, BtlSide side, BtlSideEffect eff )
{
  return NULL;
}
static BTL_EVENT_FACTOR* ADD_Dokubisi( u16 pri, BtlSide side, BtlSideEffect eff )
{
  return NULL;
}
