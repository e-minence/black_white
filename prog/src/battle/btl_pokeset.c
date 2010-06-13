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

enum {
  PRINT_FLG = TRUE,
};


/**
 *  ������
 */
void BTL_POKESET_Clear( BTL_POKESET* set )
{
  GFL_STD_MemClear( set, sizeof(BTL_POKESET) );
}

/**
 *  �|�P�����P�̓o�^�i�_���[�W�L�^�j
 */
void BTL_POKESET_AddWithDamage( BTL_POKESET* rec, BTL_POKEPARAM* bpp, u16 damage, BOOL fMigawariDamage )
{
  if( rec->count < NELEMS(rec->bpp) )
  {
    u32 i;
    for(i=0; i<rec->count; ++i)
    {
      if( rec->bpp[i] == bpp ){
        rec->damage[ i ] += damage;
        return;
      }
    }
    rec->bpp[ rec->count ] = bpp;
    rec->damage[ rec->count ] = damage;
    rec->fMigawariDamage[ rec->count ] = fMigawariDamage;
    rec->count++;
    if( rec->count > rec->countMax ){
      rec->countMax = rec->count;
    }
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_PSET_Add, rec, BPP_GetID(bpp), rec->count, rec->countMax );
  }
  else
  {
    GF_ASSERT(0);
  }
}
/**
 *  �|�P�����P�̓o�^
 */
void BTL_POKESET_Add( BTL_POKESET* rec, BTL_POKEPARAM* bpp )
{
  BTL_POKESET_AddWithDamage( rec, bpp, 0, FALSE );
}
/**
 *  �|�P�����P�̏��O
 */
void BTL_POKESET_Remove( BTL_POKESET* rec, BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<rec->count; ++i)
  {
    if( rec->bpp[i] == bpp )
    {
      u32 j;
      for(j=i+1; j<rec->count; ++j)
      {
        rec->bpp[j-1] = rec->bpp[j];
      }
      rec->count--;
      rec->getIdx--;
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_PSET_Remove, rec, BPP_GetID(bpp), rec->count, rec->countMax );
      break;
    }
  }
}
/**
 *  �|�P�����f�[�^�擾
 */
BTL_POKEPARAM* BTL_POKESET_Get( const BTL_POKESET* rec, u32 idx )
{
  if( idx < rec->count )
  {
    return (BTL_POKEPARAM*)(rec->bpp[ idx ]);
  }
  return NULL;
}
/**
 *  ���ԃA�N�Z�X�J�n
 */
void BTL_POKESET_SeekStart( BTL_POKESET* rec )
{
  rec->getIdx = 0;
}
/**
 *  ���ԃA�N�Z�X�iNULL���Ԃ�����C���j
 */
BTL_POKEPARAM* BTL_POKESET_SeekNext( BTL_POKESET* rec )
{
  if( rec->getIdx < rec->count )
  {
    return rec->bpp[ rec->getIdx++ ];
  }
  else
  {
    return NULL;
  }
}
/**
 *  �_���[�W�L�^�擾�i���́E�݂����Ƃ��j
 */
u32 BTL_POKESET_GetDamage( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<rec->count; ++i)
  {
    if( rec->bpp[i] == bpp )
    {
      return rec->damage[i];
    }
  }
  GF_ASSERT(0); // �|�P����������Ȃ�
  return 0;
}
/**
 *  �_���[�W�L�^�擾�i���̂̂݁j
 */
u32 BTL_POKESET_GetDamageReal( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<rec->count; ++i)
  {
    if( (rec->bpp[i] == bpp)
    &&  (rec->fMigawariDamage[i] == FALSE)
    ){
      return rec->damage[i];
    }
  }
  return 0;
}
/**
 *  ���ݓo�^����Ă���|�P�����������擾
 */
u32 BTL_POKESET_GetCount( const BTL_POKESET* rec )
{
  return rec->count;
}
/**
 *  ��������A�L�^���ꂽ�|�P�����������擾
 */
u32 BTL_POKESET_GetCountMax( const BTL_POKESET* rec )
{
  return rec->countMax;
}

void BTL_POKESET_SetDefaultTargetCount( BTL_POKESET* rec, u8 cnt )
{
  rec->targetPosCount = cnt;
}

/**
 *  �{���͂P�̈ȏア���n�Y�̃^�[�Q�b�g�����݂�0�ɂȂ��Ă��܂����P�[�X���`�F�b�N
 */
BOOL BTL_POKESET_IsRemovedAll( const BTL_POKESET* rec )
{
  if( (rec->countMax > 0) || ( rec->targetPosCount > 0) )
  {
    return (rec->count == 0);
  }
  return FALSE;
}

/**
 *  �����Ă�|�P�����̂݃R�s�[
 */
u32 BTL_POKESET_CopyAlive( const BTL_POKESET* src, BTL_POKESET* dst )
{
  BTL_POKEPARAM* member;
  u32 max, i;

  BTL_POKESET_Clear( dst );

  max = BTL_POKESET_GetCount( src );
  for(i=0; i<max; ++i)
  {
    member = BTL_POKESET_Get( src, i );
    if( !BPP_IsDead(member) )
    {
      BTL_POKESET_Add( dst, member );
    }
  }
  return BTL_POKESET_GetCount( dst );

}
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
    if( BPP_IsDead(bpp) )
    {
      BTL_N_Printf( DBGSTR_POKESET_RemoveDeadPoke, BPP_GetID(bpp));
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
    set->sortWork[i] = BTL_SVFTOOL_CalcAgility( flowWk, set->bpp[i], TRUE );
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

