//============================================================================================
/**
 * @file		b_bag_item.c
 * @brief		�퓬�p�o�b�O��� �A�C�e������֘A
 * @author	Hiroyuki Nakamura
 * @date		09.08.26
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "b_app_tool.h"

#include "b_bag.h"
#include "b_bag_main.h"
#include "b_bag_item.h"


//============================================================================================
//  �萔��`
//============================================================================================

//============================================================================================
//  �O���[�o��
//============================================================================================

// �퓬�Ŏg�p����A�C�e���������Ă���t�B�[���h�̃|�P�b�g
static const u8 SearchPocket[] = {
	BAG_POKE_NORMAL, BAG_POKE_DRUG, BAG_POKE_NUTS,
};

// �|�P�b�gID�e�[�u��
static const u8 PocketNum[] = {
  2, 3, 0, 1
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		�O��g�p�����A�C�e�����`�F�b�N
 *
 * @param		wk    ���[�N
 *
 * @retval  "TRUE = ����"
 * @retval  "FALSE = �Ȃ�"
 */
//--------------------------------------------------------------------------------------------
BOOL BattleBag_UsedItemChack( BBAG_WORK * wk )
{
  if( wk->used_item == ITEM_DUMMY_DATA ){ return FALSE; }

  if( MYITEM_CheckItem( wk->dat->myitem, wk->used_item, 1, wk->dat->heap ) == FALSE ){
    wk->used_item = ITEM_DUMMY_DATA;
    wk->used_poke = 0;
    return FALSE;
  }

  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Ō�Ɏg��������̃J�[�\���ʒu�Đݒ�
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_CorsorReset( BBAG_WORK * wk )
{
  u32 i;

  for( i=0; i<BBAG_POCKET_IN_MAX; i++ ){
    if( wk->used_item == wk->pocket[wk->poke_id][i].id ){
      wk->dat->item_pos[wk->poke_id] = i%6;
      wk->dat->item_scr[wk->poke_id] = i/6;
      break;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e����퓬�|�P�b�g�ɐU�蕪����
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_PocketInit( BBAG_WORK * wk )
{
/*
  ITEM_ST * item;
	ARCHANDLE * ah;
	void * buf;
  u32 i, j, k;
  s32 prm;

	ah = ITEM_OpenItemDataArcHandle( wk->dat->heap );

	for( i=0; i<NELEMS(SearchPocket); i++ ){
    j = 0;
    while(1){
      item = MYITEM_PosItemGet( wk->dat->myitem, SearchPocket[i], j );
      if( item == NULL ){ break; }
      if( !( item->id == 0 || item->no == 0 ) ){
				buf = ITEM_GetItemDataArcHandle( ah, item->id, wk->dat->heap );
				prm = ITEM_GetBufParam( buf, ITEM_PRM_BTL_POCKET );
				GFL_HEAP_FreeMemory( buf );
        for( k=0; k<4; k++ ){
          if( ( prm & (1<<k) ) == 0 ){ continue; }
          wk->pocket[ PocketNum[k] ][wk->item_max[ PocketNum[k] ]] = *item;
          wk->item_max[ PocketNum[k] ]++;
        }
      }
      j++;
    }
  }

	GFL_ARC_CloseDataHandle( ah );
*/
  ITEM_ST * item;
	u8 * pocket;
  u32 i, j, k;
  s32 prm;

	pocket = GFL_ARC_LoadDataAlloc( ARCID_ITEM_BATTLE_POCKET, 0, wk->dat->heap );

	for( i=0; i<NELEMS(SearchPocket); i++ ){
    j = 0;
    while(1){
      item = MYITEM_PosItemGet( wk->dat->myitem, SearchPocket[i], j );
      if( item == NULL ){ break; }
      if( !( item->id == 0 || item->no == 0 ) ){
				prm = pocket[item->id];
        for( k=0; k<4; k++ ){
          if( ( prm & (1<<k) ) == 0 ){ continue; }
          wk->pocket[ PocketNum[k] ][wk->item_max[ PocketNum[k] ]] = *item;
          wk->item_max[ PocketNum[k] ]++;
        }
      }
      j++;
    }
  }

	GFL_HEAP_FreeMemory( pocket );

  for( i=0; i<BATTLE_BAG_POKE_MAX; i++ ){
    if( wk->item_max[i] == 0 ){
      wk->scr_max[i] = 0;
    }else{
      wk->scr_max[i] = (wk->item_max[i]-1) / 6;
    }
    if( wk->scr_max[i] < wk->dat->item_scr[i] ){
      wk->dat->item_scr[i] = wk->scr_max[i];
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e����퓬�|�P�b�g�ɐU�蕪����i�V���[�^�[�p�j
 *
 * @param   wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_ShooterPocketInit( BBAG_WORK * wk )
{
  u32 i;

  for( i=0; i<SHOOTER_ITEM_MAX; i++ ){
    if( SHOOTER_ITEM_IsUse( wk->dat->shooter_item_bit, i ) == TRUE ){
      wk->pocket[0][wk->item_max[0]].id = SHOOTER_ITEM_ShooterIndexToItemIndex( i );
      wk->pocket[0][wk->item_max[0]].no = 1;
      wk->item_max[0]++;
    }
  }
  wk->scr_max[0] = (wk->item_max[0]-1) / 6;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e����퓬�|�P�b�g�ɐU�蕪����i�ߊl�f���p�j
 *
 * @param   wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_GetDemoPocketInit( BBAG_WORK * wk )
{
  wk->pocket[2][0].id = ITEM_MONSUTAABOORU;
  wk->pocket[2][0].no = 30;
  wk->item_max[2] = 1;
  wk->scr_max[2] = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�w��ʒu�ɃA�C�e�������邩
 *
 * @param		wk    ���[�N
 * @param		pos   �ʒu�i�O�`�T�j
 *
 * @retval  "���� = �A�C�e���ԍ�"
 * @retval  "�Ȃ� = 0"
 */
//--------------------------------------------------------------------------------------------
u16 BattleBag_PosItemCheck( BBAG_WORK * wk, u32 pos )
{
  if( wk->pocket[wk->poke_id][wk->dat->item_scr[wk->poke_id]*6+pos].id != 0 &&
    wk->pocket[wk->poke_id][wk->dat->item_scr[wk->poke_id]*6+pos].no != 0 ){
    return wk->pocket[wk->poke_id][wk->dat->item_scr[wk->poke_id]*6+pos].id;
  }
  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�V���[�^�[���̃R�X�g�擾
 *
 * @param   item    �A�C�e���ԍ�
 *
 * @return  �A�C�e���̃R�X�g
 */
//--------------------------------------------------------------------------------------------
u16 BBAGITEM_GetCost( u16 item )
{
  return SHOOTER_ITEM_ItemIndexToCost( item );
}
