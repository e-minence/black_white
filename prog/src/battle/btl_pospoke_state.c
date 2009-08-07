//=============================================================================================
/**
 * @file  btl_pospoke_state.c
 * @brief �|�P����WB �o�g���V�X�e��  ��ɂ���|�P�����̐����m�F�p�\���̂Ə������[�`��
 * @author  taya
 *
 * @date  2009.07.27  �쐬
 */
//=============================================================================================

#include "btl_common.h"
#include "btl_pokeparam.h"
#include "btl_pospoke_state.h"


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void set_pos_state( BTL_POSPOKE_WORK* wk, const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon, BtlPokePos pos );



void BTL_POSPOKE_InitWork( BTL_POSPOKE_WORK* wk, const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon, BtlRule rule )
{
  u32 i;

  for(i=0; i<NELEMS(wk->state); ++i){
    wk->state[i].fEnable = FALSE;
  }

  switch( rule ){
  case BTL_RULE_TRIPLE:
    set_pos_state( wk, mainModule, pokeCon, BTL_POS_1ST_2 );
    set_pos_state( wk, mainModule, pokeCon, BTL_POS_2ND_2 );
    /* fallthru */
  case BTL_RULE_DOUBLE:
    set_pos_state( wk, mainModule, pokeCon, BTL_POS_1ST_1 );
    set_pos_state( wk, mainModule, pokeCon, BTL_POS_2ND_1 );
    /* fallthru */
  case BTL_RULE_SINGLE:
    set_pos_state( wk, mainModule, pokeCon, BTL_POS_1ST_0 );
    set_pos_state( wk, mainModule, pokeCon, BTL_POS_2ND_0 );
    break;
  }
}
static void set_pos_state( BTL_POSPOKE_WORK* wk, const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon, BtlPokePos pos )
{
  const BTL_POKEPARAM* bpp;
  u8 clientID, memberIdx;

  BTL_MAIN_BtlPosToClientID_and_PosIdx( mainModule, pos, &clientID, &memberIdx );
  bpp = BTL_POKECON_GetClientPokeDataConst( pokeCon, clientID, memberIdx );
  if( bpp != NULL )
  {
    wk->state[ pos ].existPokeID = BPP_GetID( bpp );
  }
  else
  {
    wk->state[ pos ].existPokeID = BTL_POKEID_NULL;
  }
  wk->state[ pos ].clientID = clientID;
  wk->state[ pos ].fEnable = TRUE;
}

//=============================================================================================
/**
 * �|�P�����ޏ�̒ʒm�󂯎��
 *
 * @param   wk
 * @param   pokeID
 */
//=============================================================================================
void BTL_POSPOKE_PokeOut( BTL_POSPOKE_WORK* wk, u8 pokeID )
{
  u32 i;
  for(i=0; i<NELEMS(wk->state); ++i)
  {
    if( wk->state[i].fEnable && (wk->state[i].existPokeID == pokeID) ){
      wk->state[i].existPokeID = BTL_POKEID_NULL;
      BTL_Printf(" poke[%d] out from pos[%d]\n", pokeID, i );
      return;
    }
  }
  GF_ASSERT_MSG(0, "not exist pokeID=%d\n", pokeID);
}
//=============================================================================================
/**
 * �|�P��������̒ʒm�󂯎��
 *
 * @param   wk
 * @param   pokeID
 */
//=============================================================================================
void BTL_POSPOKE_PokeIn( BTL_POSPOKE_WORK* wk, BtlPokePos pos,  u8 pokeID )
{
  GF_ASSERT(wk->state[pos].fEnable);
  wk->state[pos].existPokeID = pokeID;
  BTL_Printf(" poke[%d] in to pos[%d]\n", pokeID, pos );
}

//=============================================================================================
/**
 * �w��N���C�A���g�̒S�����Ă���u�ʒu�v�̓��A�󂫂ɂȂ��Ă��鐔��Ԃ�
 *
 * @param   wk
 * @param   clientID
 * @param   pos         [out] �󂫈ʒuID�iBtlPokePos�j
 *
 * @retval  u8    �󂫈ʒu��
 */
//=============================================================================================
u8 BTL_POSPOKE_GetClientEmptyPos( const BTL_POSPOKE_WORK* wk, u8 clientID, u8* pos )
{
  u32 i, cnt=0;
  for(i=0; i<NELEMS(wk->state); ++i)
  {
    if( wk->state[i].fEnable && (wk->state[i].clientID == clientID) )
    {
      if( wk->state[i].existPokeID == BTL_POKEID_NULL ){
        pos[ cnt++ ] = i;
      }
    }
  }
  return cnt;
}

BOOL BTL_POSPOKE_IsExistPokemon( const BTL_POSPOKE_WORK* wk, u8 pokeID )
{
  u32 i;
  for(i=0; i<NELEMS(wk->state); ++i)
  {
    if( wk->state[i].fEnable && (wk->state[i].existPokeID == pokeID) ){
      return TRUE;
    }
  }
  return FALSE;
}

