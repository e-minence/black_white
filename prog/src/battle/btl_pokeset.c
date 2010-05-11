//=============================================================================================
/**
 * @file    btl_pokeset.C
 * @brief   ポケモンWB バトルシステム 特定目的のポケモンパラメータセット管理
 * @author  taya
 *
 * @date  2009.12.11  作成
 */
//=============================================================================================

#include "btl_pokeset.h"
#include "btl_calc.h"

/**
 *  初期化
 */
void BTL_POKESET_Clear( BTL_POKESET* set )
{
  GFL_STD_MemClear( set, sizeof(BTL_POKESET) );
}

/**
 *  コピー
 */
void BTL_POKESET_Copy( const BTL_POKESET* src, BTL_POKESET* dst )
{
  GFL_STD_MemCopy( src, dst, sizeof(BTL_POKESET) );
}

/**
 *  ポケモン１体登録（ダメージ記録）
 */
void BTL_POKESET_AddWithDamage( BTL_POKESET* rec, BTL_POKEPARAM* bpp, u16 damage )
{
  if( rec->count < NELEMS(rec->bpp) )
  {
    rec->bpp[ rec->count ] = bpp;
    rec->damage[ rec->count ] = damage;
    rec->count++;
    if( rec->count > rec->countMax ){
      rec->countMax = rec->count;
    }
  }
  else
  {
    GF_ASSERT(0);
  }
}
/**
 *  ポケモン１体登録
 */
void BTL_POKESET_Add( BTL_POKESET* rec, BTL_POKEPARAM* bpp )
{
  BTL_POKESET_AddWithDamage( rec, bpp, 0 );
}
/**
 *  ポケモン１体除外
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
      break;
    }
  }
}
/**
 *  ポケモンデータ取得
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
 *  順番アクセス開始
 */
void BTL_POKESET_SeekStart( BTL_POKESET* rec )
{
  rec->getIdx = 0;
}
/**
 *  順番アクセス（NULLが返ったら修了）
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
 *  ダメージ記録取得
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
  GF_ASSERT(0); // ポケモン見つからない
  return 0;
}
/**
 *  現在登録されているポケモン総数を取得
 */
u32 BTL_POKESET_GetCount( const BTL_POKESET* rec )
{
  return rec->count;
}
/**
 *  初期化後、記録されたポケモン総数を取得
 */
u32 BTL_POKESET_GetCountMax( const BTL_POKESET* rec )
{
  return rec->countMax;
}

/**
 *  本来は１体以上いたハズのターゲットが現在は0になってしまったケースをチェック
 */
BOOL BTL_POKESET_IsRemovedAll( const BTL_POKESET* rec )
{
  return (rec->countMax > 0) && (rec->count == 0);
}



/**
 *  指定ポケモンと同陣営のデータのみをコピー
 *
 *  @return u32 コピーしたポケモン数
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
 *  指定ポケモンと敵チームのポケモンのみを別ワークにコピー
 *
 *  @return u32 コピーしたポケモン数
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
 *  死んでるポケモンを削除
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
 *  当ターンの計算後素早さ順でソート（当ターンに行動していないポケは素の素早さ）
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
      ||  ((set->sortWork[j] == set->sortWork[i]) && BTL_CALC_GetRand(2)) // 素早さ一致ならランダム
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

