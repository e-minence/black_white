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
  u32 i, endPos;

  for(i=0; i<NELEMS(wk->state); ++i){
    wk->state[i].fEnable = FALSE;
    wk->state[i].existPokeID = BTL_POKEID_NULL;
  }

  endPos = BTL_MAIN_GetEnablePosEnd( mainModule );
  for(i=0; i<=endPos; ++i){
    set_pos_state( wk, mainModule, pokeCon, i );
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
      BTL_N_Printf( DBGSTR_POSPOKE_Out, pokeID, i );
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
  GF_ASSERT_MSG(wk->state[pos].fEnable, "pos=%d\n", pos);
  wk->state[pos].existPokeID = pokeID;
  BTL_N_Printf( DBGSTR_POSPOKE_In, pokeID, pos );

  checkConfrontRec( wk, pos, pokeCon );
}
//=============================================================================================
/**
 * �|�P�������[�e�[�V�����̒ʒm�󂯎��
 *
 * @param   wk
 * @param   dir
 * @param   clientID
 * @param   inPokeID
 * @param   pokeCon
 */
//=============================================================================================
void BTL_POSPOKE_Rotate( BTL_POSPOKE_WORK* wk, BtlRotateDir dir, u8 clientID, const BTL_POKEPARAM* inPoke, BTL_POKE_CONTAINER* pokeCon )
{
  if( (dir == BTL_ROTATEDIR_STAY) || (dir == BTL_ROTATEDIR_NONE) ){
    return;
  }
  else
  {
    #ifdef ROTATION_NEW_SYSTEM

    u32 i;

    // �Y���N���C�A���g�̒S���ʒu�C���f�b�N�X��ۑ�
    for(i=0; i<NELEMS(wk->state); ++i)
    {
      if( wk->state[i].clientID == clientID ){ break; }
    }

    if( i != NELEMS(wk->state) )
    {
      if( !BPP_IsDead(inPoke) ){
        BTL_POSPOKE_PokeIn( wk, i, BPP_GetID(inPoke), pokeCon );
      }else{
        wk->state[ i ].existPokeID = BTL_POKEID_NULL;
      }
    }

    #else

    u8  idx[ BTL_ROTATION_FRONTPOS_NUM ];
    BtlPokePos slideInPos = BTL_POS_MAX;
    u32 cnt, i;

    // �Y���N���C�A���g�̒S���ʒu�C���f�b�N�X��ۑ�
    for(cnt=0, i=0; i<NELEMS(wk->state); ++i)
    {
      if( wk->state[i].clientID == clientID ){
        idx[cnt++] = i;
        if( cnt >= BTL_ROTATION_FRONTPOS_NUM ){
          break;
        }
      }
    }

    if( dir == BTL_ROTATEDIR_R ){
      wk->state[ idx[1] ] = wk->state[ idx[0] ];
      slideInPos = idx[0];
    }
    if( dir == BTL_ROTATEDIR_L ){
      wk->state[ idx[0] ] = wk->state[ idx[1] ];
      slideInPos = idx[1];
    }

    if( slideInPos != BTL_POS_MAX )
    {
      // ��q����O�q�ɏo���|�P�������Ă��������Ɠ�������
      if( !BPP_IsDead(inPoke) ){
        BTL_POSPOKE_PokeIn( wk, slideInPos, BPP_GetID(inPoke), pokeCon );
      // ����ł����炻�̈ʒu���󂯂�
      }else{
        wk->state[ slideInPos ].existPokeID = BTL_POKEID_NULL;
      }
    }
    #endif
  }
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
 * @param   bpp
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_POSPOKE_IsExist( const BTL_POSPOKE_WORK* wk, u8 pokeID )
{
  return ( BTL_POSPOKE_GetPokeExistPos(wk, pokeID) != BTL_POS_NULL );
}
//=============================================================================================
/**
 * �w��|�P����������ꏊID���擾
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BtlPokePos    �o�Ă���ꍇ�͈ʒuID�A�o�Ă��Ȃ��ꍇ�� BTL_POS_NULL
 */
//=============================================================================================
BtlPokePos BTL_POSPOKE_GetPokeExistPos( const BTL_POSPOKE_WORK* wk, u8 pokeID )
{
  u32 i;
  for(i=0; i<NELEMS(wk->state); ++i)
  {
    if( wk->state[i].fEnable && (wk->state[i].existPokeID == pokeID) ){
      return i;
    }
  }
  return BTL_POS_NULL;
}

//=============================================================================================
/**
 * �w��ʒu�ɂ���|�P����ID��Ԃ��i�N�����Ȃ���� BTL_POKEID_NULL�j
 *
 * @param   wk
 * @param   pos
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_POSPOKE_GetExistPokeID( const BTL_POSPOKE_WORK* wk, BtlPokePos pos )
{
  GF_ASSERT(pos<BTL_POS_MAX);
  GF_ASSERT(wk->state[pos].fEnable);

  return wk->state[ pos ].existPokeID;
}
