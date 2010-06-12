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
  u16             sortWork[ BTL_POS_MAX ];
  u8              fMigawariDamage[ BTL_POS_MAX ];
  u8              count;
  u8              countMax;
  u8              getIdx;
  u8              targetPosCount;
}BTL_POKESET;


typedef BTL_POKESET  TARGET_POKE_REC;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/


/**
 *  ������
 */
extern void BTL_POKESET_Clear( BTL_POKESET* set );


/**
 *  �|�P�����P�̓o�^�i�_���[�W�L�^�j
 */
extern void BTL_POKESET_AddWithDamage( BTL_POKESET* rec, BTL_POKEPARAM* bpp, u16 damage, BOOL fMigawariHit );

/**
 *  �|�P�����P�̓o�^
 */
extern void BTL_POKESET_Add( BTL_POKESET* rec, BTL_POKEPARAM* bpp );

/**
 *  �|�P�����P�̏��O
 */
extern void BTL_POKESET_Remove( BTL_POKESET* rec, BTL_POKEPARAM* bpp );

/**
 *  �|�P�����f�[�^�擾
 */
extern BTL_POKEPARAM* BTL_POKESET_Get( const BTL_POKESET* rec, u32 idx );

/**
 *  �����Ă�|�P��������
 */
extern u32 BTL_POKESET_CopyAlive( const BTL_POKESET* src, BTL_POKESET* dst );

/**
 *  ���������R�s�[
 */
extern u32 BTL_POKESET_CopyFriends( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp, BTL_POKESET* dst );

/**
 *  ���肾���R�s�[
 */
extern u32 BTL_POKESET_CopyEnemys( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp, BTL_POKESET* dst );

/**
 *  ����ł�̂����O
 */
extern void BTL_POKESET_RemoveDeadPoke( BTL_POKESET* rec );

/**
 *  �f�������\�[�g
 */
extern void BTL_POKESET_SortByAgility( BTL_POKESET* rec, BTL_SVFLOW_WORK* flowWk );

/**
 *  ���ԃA�N�Z�X�J�n
 */
extern void BTL_POKESET_SeekStart( BTL_POKESET* rec );

/**
 *  ���ԃA�N�Z�X�iNULL���Ԃ�����C���j
 */
extern BTL_POKEPARAM* BTL_POKESET_SeekNext( BTL_POKESET* rec );

/**
 *  �_���[�W�L�^�擾�i���́E�݂����Ƃ��j
 */
extern u32 BTL_POKESET_GetDamage( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp );

/**
 *  �_���[�W�L�^�擾�i���̂̂݁j
 */
extern u32 BTL_POKESET_GetDamageReal( const BTL_POKESET* rec, const BTL_POKEPARAM* bpp );


/**
 *  ���ݓo�^����Ă���|�P�����������擾
 */
extern u32 BTL_POKESET_GetCount( const BTL_POKESET* rec );

/**
 *  ��������A�L�^���ꂽ�|�P�����������擾
 */
extern u32 BTL_POKESET_GetCountMax( const BTL_POKESET* rec );

/**
 *
 */
 extern void BTL_POKESET_SetDefaultTargetCount( BTL_POKESET* rec, u8 posCnt );

/**
 *  �{���͂P�̈ȏア���n�Y�̃^�[�Q�b�g�����݂�0�ɂȂ��Ă��܂����P�[�X���`�F�b�N
 */
extern BOOL BTL_POKESET_IsRemovedAll( const BTL_POKESET* rec );

