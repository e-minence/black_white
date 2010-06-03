//=============================================================================================
/**
 * @file  btl_deadrec.c
 * @brief �|�P����WB ���S�|�P�L�^
 * @author  taya
 *
 * @date  2009.11.23  �쐬
 */
//=============================================================================================

#include "btl_util.h"
#include "btl_deadrec.h"


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void clearTurnRecord( BTL_DEADREC_UNIT* unit );



/**
 *  �^�[���L�^�P�ʂ̏�����
 */
static void clearTurnRecord( BTL_DEADREC_UNIT* unit )
{
  u32 i;

  unit->cnt = 0;
  for(i=0; i<BTL_POKEID_MAX; ++i)
  {
    unit->deadPokeID[ i ] = BTL_POKEID_NULL;
    unit->fExpChecked[ i ] = FALSE;
  }
}

//--------------------------------------------------
/**
 *  �o�g���J�n���̏�����
 */
//--------------------------------------------------
void BTL_DEADREC_Init( BTL_DEADREC* wk )
{
  u32 i;
  for(i=0; i<BTL_DEADREC_TURN_MAX; ++i)
  {
    clearTurnRecord( &wk->record[i] );
  }
}
//--------------------------------------------------
/**
 *  �^�[���J�n���̏�����
 */
//--------------------------------------------------
void BTL_DEADREC_StartTurn( BTL_DEADREC* wk )
{
  u32 i;
  for(i=BTL_DEADREC_TURN_MAX-1; i>0; --i)
  {
    wk->record[i] = wk->record[i-1];
  }
  clearTurnRecord( &wk->record[0] );
}


//--------------------------------------------------
/**
 *  ���S�|�PID�ǉ�
 */
//--------------------------------------------------
void BTL_DEADREC_Add( BTL_DEADREC* wk, u8 pokeID )
{
  u8 cnt = wk->record[0].cnt;
  if( cnt < BTL_POKEID_MAX )
  {
    BTL_N_Printf( DBGSTR_DEADREC_Add, cnt, pokeID );
    wk->record[0].deadPokeID[ cnt ] = pokeID;
    wk->record[0].fExpChecked[ cnt ] = FALSE;
    wk->record[0].cnt++;
  }
  else{
    GF_ASSERT(0);
  }
}

//--------------------------------------------------
/**
 *  �w��^�[�����Ɏ��񂾃|�P�����擾
 */
//--------------------------------------------------
u8 BTL_DEADREC_GetCount( const BTL_DEADREC* wk, u8 turn )
{
  if( turn < BTL_DEADREC_TURN_MAX ){
    return wk->record[ turn ].cnt;
  }else{
    GF_ASSERT(0);
    return 0;
  }
}

//--------------------------------------------------
/**
 *  �w��^�[�����Ɏ��񂾃|�PID���擾
 */
//--------------------------------------------------
u8 BTL_DEADREC_GetPokeID( const BTL_DEADREC* wk, u8 turn, u8 idx )
{
  if( turn < BTL_DEADREC_TURN_MAX )
  {
    if( idx < wk->record[turn].cnt )
    {
      return wk->record[turn].deadPokeID[ idx ];
    }
  }
  return BTL_POKEID_NULL;
}

//=============================================================================================
/**
 * �o���l�����ς݃t���O�擾
 *
 * @param   wk
 * @param   turn    �k��^�[�����i���^�[���Ȃ�0�j
 * @param   idx     ���S�|�P����Index
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_DEADREC_GetExpCheckedFlag( const BTL_DEADREC* wk, u8 turn, u8 idx )
{
  if( turn < BTL_DEADREC_TURN_MAX )
  {
    if( idx < wk->record[turn].cnt )
    {
      return wk->record[turn].fExpChecked[ idx ];
    }
  }
  return FALSE;
}
//=============================================================================================
/**
 * �o���l�����ς݃t���O�Z�b�g
 *
 * @param   wk
 * @param   turn    �k��^�[�����i���^�[���Ȃ�0�j
 * @param   idx     ���S�|�P����Index
 */
//=============================================================================================
void BTL_DEADREC_SetExpCheckedFlag( BTL_DEADREC* wk, u8 turn, u8 idx )
{
  if( turn < BTL_DEADREC_TURN_MAX )
  {
    if( idx < wk->record[turn].cnt )
    {
      wk->record[turn].fExpChecked[ idx ] = TRUE;
    }
  }
}

//=============================================================================================
/**
 * �Ō�Ɏ��S�����|�P����ID���擾
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_DEADREC_GetLastDeadPokeID( const BTL_DEADREC* wk )
{
  u32 i;
  for(i=0; i<BTL_DEADREC_TURN_MAX; ++i)
  {
    if( wk->record[i].cnt )
    {
      u32 n = wk->record[i].cnt - 1;
      return wk->record[i].deadPokeID[n];
    }
  }
  return BTL_POKEID_NULL;
}

