//=============================================================================================
/**
 * @file    btl_pokeset.h
 * @brief   �|�P����WB �o�g���V�X�e�� ����ړI�̃|�P�����p�����[�^�Z�b�g�Ǘ�
 * @author  taya
 *
 * @date  2009.12.11  �쐬
 */
//=============================================================================================
#pragma once

#include "btl_common.h"
#include "btl_pokeparam.h"
#include "btl_server_flow.h"

//----------------------------------------------------------------------
/**
 * �ΏۂƂȂ�|�P�����p�����[�^���L�^���Ă������߂̃��[�N
 */
//----------------------------------------------------------------------
typedef struct {

  BTL_POKEPARAM*  bpp[ BTL_POS_MAX ];
  u16             damage[ BTL_POS_MAX ];
  u8              count;
  u8              countMax;
  u8              getIdx;
}BTL_POKESET;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
extern void BTL_POKESET_SortByAgility( BTL_POKESET* rec );
extern u32 BTL_POKESET_CopyFriends( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp, BTL_POKESET* dst );
extern u32 BTL_POKESET_CopyEnemys( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp, BTL_POKESET* dst );
extern void BTL_POKESET_RemoveDeadPokemon( BTL_POKESET* rec );



/**
 *  ������
 */
static inline void BTL_POKESET_Clear( BTL_POKESET* set )
{
  GFL_STD_MemClear( set, sizeof(BTL_POKESET) );
}
/**
 *  �|�P�����P�̓o�^�i�_���[�W�L�^�j
 */
static inline void BTL_POKESET_AddWithDamage( BTL_POKESET* rec, BTL_POKEPARAM* bpp, u16 damage )
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
 *  �|�P�����P�̓o�^
 */
static inline void BTL_POKESET_Add( BTL_POKESET* rec, BTL_POKEPARAM* bpp )
{
  BTL_POKESET_AddWithDamage( rec, bpp, 0 );
}
/**
 *  �|�P�����P�̏��O
 */
static inline void BTL_POKESET_Remove( BTL_POKESET* rec, BTL_POKEPARAM* bpp )
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
 *  �|�P�����f�[�^�擾
 */
static inline BTL_POKEPARAM* BTL_POKESET_Get( const BTL_POKESET* rec, u32 idx )
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
static inline void BTL_POKESET_SeekStart( BTL_POKESET* rec )
{
  rec->getIdx = 0;
}
/**
 *  ���ԃA�N�Z�X�iNULL���Ԃ�����C���j
 */
static inline BTL_POKEPARAM* BTL_POKESET_SeekNext( BTL_POKESET* rec )
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
 *  �_���[�W�L�^�擾
 */
static inline u32 BTL_POKESET_GetDamage( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp )
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
 *  ���ݓo�^����Ă���|�P�����������擾
 */
static inline u32 BTL_POKESET_GetCount( const BTL_POKESET* rec )
{
  return rec->count;
}
/**
 *  ��������A�L�^���ꂽ�|�P�����������擾
 */
static inline u32 BTL_POKESET_GetCountMax( const BTL_POKESET* rec )
{
  return rec->countMax;
}


/**
 *  �{���͂P�̈ȏア���n�Y�̃^�[�Q�b�g�����݂�0�ɂȂ��Ă��܂����P�[�X���`�F�b�N
 */
static inline BOOL BTL_POKESET_IsTargetRemoved( const BTL_POKESET* rec )
{
  return (rec->countMax > 0) && (rec->count == 0);
}
