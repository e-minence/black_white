//============================================================================================
/**
 * @file	b_bag_item.c
 * @brief	�퓬�p�o�b�O��� �A�C�e������֘A
 * @author	Hiroyuki Nakamura
 * @date	09.08.26
 */
//============================================================================================
#include <gflib.h>
/*��[GS_CONVERT_TAG]*/
/*
#include "system/msgdata.h"
#include "system/numfont.h"
#include "system/wordset.h"
*/
//#include "system/clact_tool.h"
/*��[GS_CONVERT_TAG]*/
/*
#include "battle/battle_common.h"
#include "itemtool/myitem.h"
*/
#include "b_app_tool.h"

#include "b_bag.h"
#include "b_bag_main.h"
#include "b_bag_item.h"


typedef struct {
	u16	item;
	u16	cost;
}SHOOTER_ITEM;

// �|�P�b�gID�e�[�u��
static const u8 PocketNum[] = {
	2, 3, 0, 1, 0
};

// �V���[�^�[�p�A�C�e���e�[�u��
static const SHOOTER_ITEM ShooterItemTable[] =
{
	{ ITEM_KURITHIKATTAA, 1 },		// 56: �N���e�B�J�b�^�[
	{ ITEM_SUPIIDAA, 1 },					// 59: �X�s�[�_�[
	{ ITEM_SUPESYARUAPPU, 1 },		// 61: �X�y�V�����A�b�v
	{ ITEM_SUPESYARUGAADO, 1 },		// 62: �X�y�V�����K�[�h
	{ ITEM_DHIFENDAA, 1 },				// 58: �f�B�t�F���_�[
	{ ITEM_PURASUPAWAA, 1 },			// 57: �v���X�p���[
	{ ITEM_YOKUATAARU, 1 },				// 60: ���N�A�^�[��
	{ ITEM_EFEKUTOGAADO, 1 },			// 55: �G�t�F�N�g�K�[�h

	{ ITEM_KIZUGUSURI, 2 },				// 17: �L�Y������

	{ ITEM_PIIPIIEIDO, 3 },				// 38: �s�[�s�[�G�C�h

	{ ITEM_IIKIZUGUSURI, 4 },			// 26: �����L�Y������
	{ ITEM_KOORINAOSI, 4 },				// 20: ������Ȃ���
	{ ITEM_DOKUKESI, 4 },					// 18: �ǂ�����
	{ ITEM_NEMUKEZAMASI, 4 },			// 21: �˂ނ����܂�
	{ ITEM_MAHINAOSI, 4 },				// 22: �܂ЂȂ���
	{ ITEM_YAKEDONAOSI, 4 },			// 19: �₯�ǂȂ���

	{ ITEM_SUGOIKIZUGUSURI, 6 },	// 25: �������L�Y������
	{ ITEM_NANDEMONAOSI, 6 },			// 27: �Ȃ�ł��Ȃ���
	{ ITEM_PIIPIIEIDAA, 6 },			// 40: �s�[�s�[�G�C�_�[

	{ ITEM_PIIPIIRIKABAA, 7 },		// 39: �s�[�s�[���J�o�[

	{ ITEM_MANTANNOKUSURI, 8 },		// 24: �܂񂽂�̂�����

	{ ITEM_PIIPIIMAKKUSU, 10 },		// 41: �s�[�s�[�}�b�N�X

	{ ITEM_GENKINOKAKERA, 11 },		// 28: ���񂫂̂�����

	{ ITEM_KAIHUKUNOKUSURI, 13 },	// 23: �����ӂ��̂�����
	{ ITEM_GENKINOKATAMARI, 14 },	// 29: ���񂫂̂����܂�

	{ ITEM_DUMMY_DATA, 0 },				// �I��
};



//--------------------------------------------------------------------------------------------
/**
 * �O��g�p�����A�C�e�����`�F�b�N
 *
 * @param	wk		���[�N
 *
 * @retval	"TRUE = ����"
 * @retval	"FALSE = �Ȃ�"
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
 * �Ō�Ɏg��������̃J�[�\���ʒu�Đݒ�
 *
 * @param	wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_CorsorReset( BBAG_WORK * wk )
{
	u32	i;

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
 * �A�C�e����퓬�|�P�b�g�ɐU�蕪����
 *
 * @param	wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_PocketInit( BBAG_WORK * wk )
{
	ITEM_ST * item;
	u32	i, j, k;
	s32	prm;

	for( i=0; i<BAG_POKE_MAX; i++ ){
		j = 0;
		while(1){
//			item = MyItem_PosItemGet( wk->dat->myitem, i, j );
			item = MYITEM_PosItemGet( wk->dat->myitem, i, j );
			if( item == NULL ){ break; }
			if( !( item->id == 0 || item->no == 0 ) ){
				prm = ITEM_GetParam( item->id, ITEM_PRM_BTL_POCKET, wk->dat->heap );
				for( k=0; k<5; k++ ){
					if( ( prm & (1<<k) ) == 0 ){ continue; }
					wk->pocket[ PocketNum[k] ][wk->item_max[ PocketNum[k] ]] = *item;
					wk->item_max[ PocketNum[k] ]++;
				}
			}
			j++;
		}
	}

	for( i=0; i<5; i++ ){
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

void BattleBag_ShooterPocketInit( BBAG_WORK * wk )
{
	ITEM_ST	item;
	u32	i, j, k;
	s32	prm;

	i = 0;
	while(1){
		if( ShooterItemTable[i].item == ITEM_DUMMY_DATA || ShooterItemTable[i].cost == 0 ){
			break;
		}
		wk->pocket[0][wk->item_max[0]].id = ShooterItemTable[i].item;
		wk->pocket[0][wk->item_max[0]].no = 1;
		wk->item_max[0]++;
		i++;
	}

	wk->scr_max[0] = (wk->item_max[0]-1) / 6;
}

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�ɃA�C�e�������邩
 *
 * @param	wk		���[�N
 * @param	pos		�ʒu�i�O�`�T�j
 *
 * @retval	"���� = �A�C�e���ԍ�"
 * @retval	"�Ȃ� = 0"
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
 * �A�C�e���@�\���擾
 *
 * @param	wk		���[�N
 *
 * @return	�A�C�e���@�\
 */
//--------------------------------------------------------------------------------------------
u8 BattleBag_ItemUseCheck( BBAG_WORK * wk )
{
//	return (u8)ItemParamGet( wk->dat->ret_item, ITEM_PRM_BATTLE, wk->dat->heap );
	return 0;
}


//--------------------------------------------------------------------------------------------
/**
 * �V���[�^�[���̃R�X�g�擾
 *
 * @param		item		�A�C�e���ԍ�
 *
 * @return	�A�C�e���̃R�X�g
 */
//--------------------------------------------------------------------------------------------
u16 BBAGITEM_GetCost( u16 item )
{
	u32	i = 0;

	while(1){
		if( ShooterItemTable[i].item == 0 || ShooterItemTable[i].cost == 0 ){
			break;
		}
		if( ShooterItemTable[i].item == item ){
			break;
		}
		i++;
	}
	return ShooterItemTable[i].cost;
}
