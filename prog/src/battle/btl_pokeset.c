//=============================================================================================
/**
 * @file    btl_pokeset.C
 * @brief   �|�P����WB �o�g���V�X�e�� ����ړI�̃|�P�����p�����[�^�Z�b�g�Ǘ�
 * @author  taya
 *
 * @date  2009.12.11  �쐬
 */
//=============================================================================================

#include "btl_pokeset.h"
#include "btl_calc.h"

/**
 *  �w��|�P�����Ɠ��w�c�̃f�[�^�݂̂��R�s�[
 *
 *  @return u32 �R�s�[�����|�P������
 */
u32 BTL_POKESET_CopyFriends( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp, BTL_POKESET* dst )
{
  BTL_POKEPARAM* member;
  u32 max, i;
  u8 ID1, ID2;

  BTL_POKESET_Clear( dst );
  ID1 = BPP_GetID( bpp );

  max = BTL_POKESET_GetCount( rec );
  for(i=0; i<max; ++i)
  {
    member = BTL_POKESET_Get( rec, i );
    ID2 = BPP_GetID( member );
    if( BTL_MAINUTIL_IsFriendPokeID(ID1, ID2) ){
      BTL_POKESET_Add( dst, member );
    }
  }
  return BTL_POKESET_GetCount( dst );
}
/**
 *  �w��|�P�����ƓG�`�[���̃|�P�����݂̂�ʃ��[�N�ɃR�s�[
 *
 *  @return u32 �R�s�[�����|�P������
 */
u32 BTL_POKESET_CopyEnemys( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp, BTL_POKESET* dst )
{
  BTL_POKEPARAM* member;
  u32 max, i;
  u8 ID1, ID2;

  BTL_POKESET_Clear( dst );
  ID1 = BPP_GetID( bpp );

  max = BTL_POKESET_GetCount( rec );
  for(i=0; i<max; ++i)
  {
    member = BTL_POKESET_Get( rec, i );
    ID2 = BPP_GetID( member );
    if( !BTL_MAINUTIL_IsFriendPokeID(ID1, ID2) ){
      BTL_POKESET_Add( dst, member );
    }
  }
  return BTL_POKESET_GetCount( dst );
}
/**
 *  ����ł�|�P�������폜
 */
void BTL_POKESET_RemoveDeadPoke( BTL_POKESET* rec )
{
  BTL_POKEPARAM* bpp;

  BTL_POKESET_SeekStart( rec );
  while( (bpp = BTL_POKESET_SeekNext(rec)) != NULL )
  {
    if( BPP_IsDead(bpp) ){
      BTL_POKESET_Remove( rec, bpp );
    }
  }
}

/**
 *  ���^�[���̌v�Z��f�������Ń\�[�g�i���^�[���ɍs�����Ă��Ȃ��|�P�͑f�̑f�����j
 */
void BTL_POKESET_SortByAgility( BTL_POKESET* set, BTL_SVFLOW_WORK* flowWk )
{
  u32 i, j;

  for(i=0; i<set->count; ++i){
    set->sortWork[i] = BTL_SVFTOOL_CalcAgility( flowWk, set->bpp[i] );
  }

  for(i=0; i<set->count; ++i)
  {
    for(j=i+1; j<set->count; ++j)
    {
      if( (set->sortWork[j] > set->sortWork[i])
      ||  ((set->sortWork[j] == set->sortWork[i]) && BTL_CALC_GetRand(2)) // �f������v�Ȃ烉���_��
      ){
        BTL_POKEPARAM* tmpBpp;
        u16            tmpDmg;
        u16            tmpAgi;

        tmpBpp = set->bpp[i];
        tmpDmg = set->damage[i];
        tmpAgi = set->sortWork[i];
        set->bpp[i]      = set->bpp[j];
        set->damage[i]   = set->damage[j];
        set->sortWork[i] = set->sortWork[j];
        set->bpp[j]      = tmpBpp;
        set->damage[j]   = tmpDmg;
        set->sortWork[j] = tmpAgi;
      }
    }
  }

}

