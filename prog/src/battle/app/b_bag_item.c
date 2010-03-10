//============================================================================================
/**
 * @file	b_bag_item.c
 * @brief	�퓬�p�o�b�O��� �A�C�e������֘A
 * @author	Hiroyuki Nakamura
 * @date	09.08.26
 */
//============================================================================================
#include <gflib.h>

#include "b_app_tool.h"

#include "b_bag.h"
#include "b_bag_main.h"
#include "b_bag_item.h"


//============================================================================================
//	�萔��`
//============================================================================================
#define	TEST_SHOOTER_ITEM

#ifdef TEST_SHOOTER_ITEM
typedef struct {
	u16	item;
	u16	cost;
}SHOOTER_ITEM;
#endif	// TEST_SHOOTER_ITEM

//============================================================================================
//	�O���[�o��
//============================================================================================

// �|�P�b�gID�e�[�u��
static const u8 PocketNum[] = {
	2, 3, 0, 1, 0
};


#ifdef TEST_SHOOTER_ITEM
// �V���[�^�[�p�A�C�e���e�[�u��
static const SHOOTER_ITEM ShooterItemTable[] =
{
	{ ITEM_KURITHIKATTAA, 1 },		// �N���e�B�J�b�^�[
	{ ITEM_SUPIIDAA, 1 },					// �X�s�[�_�[
	{ ITEM_SUPESYARUAPPU, 1 },		// �X�y�V�����A�b�v
	{ ITEM_SUPESYARUGAADO, 1 },		// �X�y�V�����K�[�h
	{ ITEM_DHIFENDAA, 1 },				// �f�B�t�F���_�[
	{ ITEM_PURASUPAWAA, 1 },			// �v���X�p���[
	{ ITEM_YOKUATAARU, 1 },				// ���N�A�^�[��
	{ ITEM_EFEKUTOGAADO, 1 },			// �G�t�F�N�g�K�[�h

	{ ITEM_KIZUGUSURI, 2 },				// �L�Y������

	{ ITEM_PIIPIIEIDO, 3 },				// �s�[�s�[�G�C�h
	{ ITEM_AITEMUKOORU, 3 },			// �A�C�e���R�[��
	{ ITEM_KURITHIKATTO2, 3 },		// �N���e�B�J�b�g�Q
	{ ITEM_SUPIIDAA2, 3 },				// �X�s�[�_�[�Q
	{ ITEM_spAPPU2, 3 },					// �r�o�A�b�v�Q
	{ ITEM_spGAADO2, 3 },					// �r�o�K�[�h�Q
	{ ITEM_DHIFENDAA2, 3 },				// �f�B�t�F���_�[�Q
	{ ITEM_PURASUPAWAA2, 3 },			// �v���X�p���[�Q
	{ ITEM_YOKUATAARU2, 3 },			// ���N�A�^�[���Q

	{ ITEM_IIKIZUGUSURI, 4 },			// �����L�Y������
	{ ITEM_KOORINAOSI, 4 },				// ������Ȃ���
	{ ITEM_DOKUKESI, 4 },					// �ǂ�����
	{ ITEM_NEMUKEZAMASI, 4 },			// �˂ނ����܂�
	{ ITEM_MAHINAOSI, 4 },				// �܂ЂȂ���
	{ ITEM_YAKEDONAOSI, 4 },			// �₯�ǂȂ���

	{ ITEM_SUKIRUKOORU, 5 },			// �X�L���R�[��
	{ ITEM_KURITHIKATTO3, 5 },		// �N���e�B�J�b�g�R
	{ ITEM_SUPIIDAA3, 5 },				// �X�s�[�_�[�R
	{ ITEM_spAPPU3, 5 },					// �r�o�A�b�v�R
	{ ITEM_spGAADO3, 5 },					// �r�o�K�[�h�R
	{ ITEM_DHIFENDAA3, 5 },				// �f�B�t�F���_�[�R
	{ ITEM_PURASUPAWAA3, 5 },			// �v���X�p���[�R
	{ ITEM_YOKUATAARU3, 5 },			// ���N�A�^�[���R

	{ ITEM_SUGOIKIZUGUSURI, 6 },	// �������L�Y������
	{ ITEM_NANDEMONAOSI, 6 },			// �Ȃ�ł��Ȃ���
	{ ITEM_PIIPIIEIDAA, 6 },			// �s�[�s�[�G�C�_�[

	{ ITEM_PIIPIIRIKABAA, 7 },		// �s�[�s�[���J�o�[
	{ ITEM_AITEMUDOROPPU, 7 },		// �A�C�e���h���b�v

	{ ITEM_MANTANNOKUSURI, 8 },		// �܂񂽂�̂�����

	{ ITEM_HURATTOKOORU, 9 },			// �t���b�g�R�[��

	{ ITEM_PIIPIIMAKKUSU, 10 },		// �s�[�s�[�}�b�N�X

	{ ITEM_GENKINOKAKERA, 11 },		// ���񂫂̂�����

	{ ITEM_SUPIIDAA6, 12 },				// �X�s�[�_�[�U
	{ ITEM_spAPPU6, 12 },					// �r�o�A�b�v�U
	{ ITEM_spGAADO6, 12 },				// �r�o�K�[�h�U
	{ ITEM_DHIFENDAA6, 12 },			// �f�B�t�F���_�[�U
	{ ITEM_PURASUPAWAA6, 12 },		// �v���X�p���[�U
	{ ITEM_YOKUATAARU6, 12 },			// ���N�A�^�[���U

	{ ITEM_KAIHUKUNOKUSURI, 13 },	// �����ӂ��̂�����

	{ ITEM_GENKINOKATAMARI, 14 },	// ���񂫂̂����܂�

	{ ITEM_DUMMY_DATA, 0 },				// �I��
};
#endif	// TEST_SHOOTER_ITEM


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
 * �A�C�e����퓬�|�P�b�g�ɐU�蕪����i�V���[�^�[�p�j
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_ShooterPocketInit( BBAG_WORK * wk )
{
#ifdef TEST_SHOOTER_ITEM
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
#else
	u32	i;

	for( i=0; i<SHOOTER_ITEM_MAX; i++ ){
		if( SHOOTER_ITEM_IsUse( wk->dat->shooter_item_bit, i ) == TRUE ){
			wk->pocket[0][wk->item_max[0]].id = SHOOTER_ITEM_ShooterIndexToItemIndex( i );
			wk->pocket[0][wk->item_max[0]].no = 1;
			wk->item_max[0]++;
		}
	}
	wk->scr_max[0] = (wk->item_max[0]-1) / 6;
#endif	// #ifdef TEST_SHOOTER_ITEM
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e����퓬�|�P�b�g�ɐU�蕪����i�ߊl�f���p�j
 *
 * @param		wk		���[�N
 *
 * @return	none
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
#ifdef TEST_SHOOTER_ITEM
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
#else
	return SHOOTER_ITEM_ItemIndexToCost( item );
#endif
}
