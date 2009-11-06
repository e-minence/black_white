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
static void checkConfrontRec( BTL_POSPOKE_WORK* wk, BtlPokePos pos, BTL_POKE_CONTAINER* pokeCon );



void BTL_POSPOKE_InitWork( BTL_POSPOKE_WORK* wk, const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon, BtlRule rule )
{
  u32 i;

  for(i=0; i<NELEMS(wk->state); ++i){
    wk->state[i].fEnable = FALSE;
    wk->state[i].existPokeID = BTL_POKEID_NULL;
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

  if( bpp != NULL && !BPP_IsDead(bpp) )
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
void BTL_POSPOKE_PokeIn( BTL_POSPOKE_WORK* wk, BtlPokePos pos,  u8 pokeID, BTL_POKE_CONTAINER* pokeCon )
{
  GF_ASSERT(wk->state[pos].fEnable);
  wk->state[pos].existPokeID = pokeID;
  BTL_Printf(" poke[%d] in to pos[%d]\n", pokeID, pos );

  checkConfrontRec( wk, pos, pokeCon );
}
//----------------------------------------------------------------------------------
/**
 * �Ζʃ��R�[�h�X�V
 *
 * @param   wk
 * @param   pos
 * @param   pokeCon
 */
//----------------------------------------------------------------------------------
static void checkConfrontRec( BTL_POSPOKE_WORK* wk, BtlPokePos pos, BTL_POKE_CONTAINER* pokeCon )
{
  u32 i;
  u8 pokeID = wk->state[pos].existPokeID;

  if( pokeID != BTL_POKEID_NULL )
  {
    for(i=0; i<NELEMS(wk->state); ++i)
    {
      if( (wk->state[i].existPokeID != BTL_POKEID_NULL)
      &&  (wk->state[i].existPokeID != pokeID )
      ){
        if( !BTL_MAINUTIL_IsFriendPokeID(wk->state[i].existPokeID, pokeID) )
        {
          BTL_POKEPARAM* bpp;

          BTL_Printf("%d ��%d ���Ζ�\n", pokeID, wk->state[i].existPokeID);

          bpp = BTL_POKECON_GetPokeParam( pokeCon, wk->state[i].existPokeID );
          BPP_CONFRONT_REC_Set( bpp, pokeID );

          bpp = BTL_POKECON_GetPokeParam( pokeCon, pokeID );
          BPP_CONFRONT_REC_Set( bpp, wk->state[i].existPokeID );
        }
      }
    }
  }
}

//=============================================================================================
/**
 * �|�P�����ʒu����
 *
 * @param   wk
 * @param   pos1
 * @param   pos2
 */
//=============================================================================================
void BTL_POSPOKE_Swap( BTL_POSPOKE_WORK* wk, BtlPokePos pos1, BtlPokePos pos2 )
{
  BTL_POSPOKE_STATE tmp = wk->state[ pos1 ];
  wk->state[ pos1 ] = wk->state[ pos2 ];
  wk->state[ pos2 ] = tmp;
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
//=============================================================================================
/**
 * �w��|�P�������퓬�̏�ɏo�Ă��邩����
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BOOL    �o�Ă���ꍇ��TRUE
 */
//=============================================================================================
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

