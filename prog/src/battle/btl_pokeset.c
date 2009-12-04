//=============================================================================================
/**
 * @file    btl_pokeset.h
 * @brief   �|�P����WB �o�g���V�X�e�� ����ړI�̃|�P�����p�����[�^�Z�b�g�Ǘ�
 * @author  taya
 *
 * @date  2009.12.11  �쐬
 */
//=============================================================================================

#include "btl_pokeset.h"

/**
 *  �w��|�P�����Ɠ��w�c�̃f�[�^�݂̂��R�s�[
 *
 *  @return u32 �R�s�[�����|�P������
 */
u32 BTL_POKESET_CopyFriends( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* bpp, TARGET_POKE_REC* dst )
{
  BTL_POKEPARAM* bpp;
  u32 max, i;
  u8 ID1, ID2;

  BTL_POKESET_Clear( dst );
  ID1 = BPP_GetID( bpp );

  max = BTL_POKESET_GetCount( rec );
  for(i=0; i<max; ++i)
  {
    bpp = BTL_POKESET_Get( rec, i );
    ID2 = BPP_GetID( bpp );
    if( BTL_MAINUTIL_IsFriendPokeID(ID1, ID2) ){
      BTL_POKESET_Add( dst, bpp );
    }
  }
  return BTL_POKESET_GetCount( dst );
}
/**
 *  �w��|�P�����ƓG�`�[���̃|�P�����݂̂�ʃ��[�N�ɃR�s�[
 *
 *  @return u32 �R�s�[�����|�P������
 */
u32 BTL_POKESET_CopyEnemys( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* bpp, TARGET_POKE_REC* dst )
{
  BTL_POKEPARAM* bpp;
  u32 max, i;
  u8 ID1, ID2;

  BTL_POKESET_Clear( dst );
  ID1 = BPP_GetID( bpp );

  max = BTL_POKESET_GetCount( rec );
  for(i=0; i<max; ++i)
  {
    bpp = BTL_POKESET_Get( rec, i );
    ID2 = BPP_GetID( bpp );
    if( !BTL_MAINUTIL_IsFriendPokeID(ID1, ID2) ){
      BTL_POKESET_Add( dst, bpp );
    }
  }
  return BTL_POKESET_GetCount( dst );
}
/**
 *  ����ł�|�P�������폜
 */
void BTL_POKESET_RemoveDeadPokemon( TARGET_POKE_REC* rec )
{
  BTL_POKEPARAM* bpp;

  BTL_POKESET_GetStart( rec );

  while( (bpp = BTL_POKESET_GetNext(rec)) != NULL )
  {
    if( BPP_IsDead(bpp) ){
      BTL_POKESET_Remove( rec, bpp );
    }
  }
}

/**
 *  ���^�[���̌v�Z��f�������Ń\�[�g�i���^�[���ɍs�����Ă��Ȃ��|�P�͑f�̑f�����j
 */
void BTL_POKESET_SortByAgility( TARGET_POKE_REC* rec )
{

}

