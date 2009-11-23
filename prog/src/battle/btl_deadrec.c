//=============================================================================================
/**
 * @file  btl_deadrec.c
 * @brief �|�P����WB ���S�|�P�L�^
 * @author  taya
 *
 * @date  2009.11.23  �쐬
 */
//=============================================================================================

#include "btl_deadrec.h"

/**
 *  �o�g���J�n���̏�����
 */
void BTL_DEADREC_Init( BTL_DEADREC* wk )
{
  GFL_STD_MemClear( wk, sizeof(BTL_DEADREC) );
}

/**
 *  �^�[���J�n���̏�����
 */
void BTL_DEADREC_StartTurn( BTL_DEADREC* wk )
{
  u32 i;
  for(i=BTL_DEADREC_TURN_MAX-1; i>0; --i)
  {
    wk->record[i] = wk->record[i-1];
  }
  GFL_STD_MemClear( &wk->record[i], sizeof(wk->record[0]) );
}

/**
 *  ���S�|�PID�ǉ�
 */
void BTL_DEADREC_Add( BTL_DEADREC* wk, u8 pokeID )
{
  u8 cnt = wk->record[0].cnt;
  if( cnt < NELEMS(wk->record[0].pokeID) ){
    wk->record[0].pokeID[cnt] = pokeID;
    wk->record[0].cnt++;
  }else{
    GF_ASSERT(0);
  }
}



u8 BTL_DEADREC_GetCount( const BTL_DEADREC* wk, u8 turn )
{
  if( turn < BTL_DEADREC_TURN_MAX ){
    return wk->record[ turn ].cnt;
  }else{
    GF_ASSERT(0);
    return 0;
  }
}
u8 BTL_DEADREC_GetPokeID( const BTL_DEADREC* wk, u8 turn, u8 idx )
{
  if( turn < BTL_DEADREC_TURN_MAX )
  {
    if( idx < wk->record[ turn ].cnt )
    {
      return wk->record[ turn ].pokeID[idx];
    }
  }
  GF_ASSERT(0);
  return BTL_POKEID_NULL;
}

