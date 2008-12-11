//==============================================================================
/**
 * @file	myitem_savedata.c
 * @brief	手持ちアイテム
 * @author	matsuda
 * @date	2008.12.04(木)
 */
//==============================================================================
#include <gflib.h>
#include "savedata/myitem_savedata.h"
#include "item/item.h"



//==============================================================================
//	構造体定義
//==============================================================================
//------------------------------------------------------------------
/**
 * @brief	手持ちアイテム構造体の定義
 */
//------------------------------------------------------------------
struct _MYITEM {
	ITEM_ST MyNormalItem[ BAG_NORMAL_ITEM_MAX ];	// 手持ちの普通の道具
	ITEM_ST MyEventItem[ BAG_EVENT_ITEM_MAX ];		// 手持ちの大切な道具
	ITEM_ST MySkillItem[ BAG_WAZA_ITEM_MAX ];		// 手持ちの技マシン
	ITEM_ST MySealItem[ BAG_SEAL_ITEM_MAX ];		// 手持ちのシール
	ITEM_ST MyDrugItem[ BAG_DRUG_ITEM_MAX ];		// 手持ちの薬
	ITEM_ST MyNutsItem[ BAG_NUTS_ITEM_MAX ];		// 手持ちの木の実
	ITEM_ST MyBallItem[ BAG_BALL_ITEM_MAX ];		// 手持ちのモンスターボール
	ITEM_ST MyBattleItem[ BAG_BATTLE_ITEM_MAX ];	// 手持ちの戦闘用アイテム
	u32	cnv_button;									// 便利ボタン
};

// フィールドのバッグのカーソル位置
typedef struct {
	u8	scr[8];
	u8	pos[8];
	u16	pocket;
	u16	dummy;
}FLDBAG_CURSOR;

// 戦闘のバッグのカーソル位置
typedef struct {
	u8	scr[5];
	u8	pos[5];
	u16	item;
	u16	page;
	u16	pocket;		// ポケットページの位置
}BTLBAG_CURSOR;

// バッグのカーソル位置データ
struct _BAG_CURSOR {
	FLDBAG_CURSOR	fld;
	BTLBAG_CURSOR	btl;
};

//==============================================================================
//	定数定義
//==============================================================================
#define	ITEM_CHECK_ERR	( 0xffffffff )		// チェックエラー

#define	ITEM_MAX_NORMAL			( 999 )		// 通常のアイテム所持数最大
#define	ITEM_MAX_WAZAMACHINE	( 99 )		// 技マシンの所持数最大

static u32 MyItemDataGet( MYITEM_PTR myitem, u16 id, ITEM_ST ** item, u32 * max, u32 heap );



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   手持ちアイテムワークのサイズ取得
 * @retval  サイズ
 */
//--------------------------------------------------------------
int MYITEM_GetWorkSize(void)
{
	return sizeof(MYITEM);
}

//------------------------------------------------------------------
/**
 * @brief	手持ちアイテムワークを生成する
 * @param	myitem	手持ちアイテム構造体へのポインタ
 * @return	MYITEMへのポインタ
 */
//------------------------------------------------------------------
MYITEM_PTR MYITEM_AllocWork(int heapID)
{
	MYITEM_PTR item;
	item = GFL_HEAP_AllocMemory(heapID, sizeof(MYITEM));
	MYITEM_Init(item);
	return item;
}

//------------------------------------------------------------------
/**
 * @brief	初期化処理
 * @param	item	MYITEMへのポインタ
 */
//------------------------------------------------------------------
void MYITEM_Init(MYITEM_PTR item)
{
	GFL_STD_MemClear(item, sizeof(MYITEM));
}

//------------------------------------------------------------------
/**
 * @brief	MYITEMのコピー
 * @param	from	コピー元へのポインタ
 * @param	to		コピー先へのポインタ
 */
//------------------------------------------------------------------
void MYITEM_Copy(const MYITEM_PTR from, MYITEM_PTR to)
{
	GFL_STD_MemCopy(from, to, sizeof(MYITEM));
}

//------------------------------------------------------------------
/**
 * @brief	便利ボタンに割り当てられているアイテム取得
 * @param	myitem	MYITEMへのポインタ
 * @return	アイテム番号
 */
//------------------------------------------------------------------
u32 MYITEM_CnvButtonItemGet( const MYITEM_PTR myitem )
{
	return myitem->cnv_button;
}

//------------------------------------------------------------------
/**
 * @brief	便利ボタンにアイテムセット
 * @param	myitem	MYITEMへのポインタ
 * @param	setitem	セットするアイテム
 * @return	アイテム番号
 */
//------------------------------------------------------------------
void MYITEM_CnvButtonItemSet( MYITEM_PTR myitem, u32 setitem )
{
	myitem->cnv_button = setitem;
}

//------------------------------------------------------------------
/**
 * @brief	MYITEMと最大数取得
 * @param	id		アイテム番号
 * @param	item	MYITEM取得場所
 * @param	max		最大数取得場所
 * @param	heap		ヒープID
 * @return	ポケット番号
 */
//------------------------------------------------------------------
static u32 MyItemDataGet( MYITEM_PTR myitem, u16 id, ITEM_ST ** item, u32 * max, u32 heap )
{
	s32 pocket = ITEM_GetParam( id, ITEM_PRM_POCKET, heap );
	
	switch( pocket ){
	case BAG_POKE_EVENT:	// 大切な物
		*item = myitem->MyEventItem;
		*max = BAG_EVENT_ITEM_MAX;
		break;
	case BAG_POKE_NORMAL:	// 道具
		*item = myitem->MyNormalItem;
		*max = BAG_NORMAL_ITEM_MAX;
		break;
	case BAG_POKE_NUTS:		// 木の実
		*item = myitem->MyNutsItem;
		*max = BAG_NUTS_ITEM_MAX;
		break;
	case BAG_POKE_DRUG:		// 薬
		*item = myitem->MyDrugItem;
		*max = BAG_DRUG_ITEM_MAX;
		break;
	case BAG_POKE_BALL:		// ボール
		*item = myitem->MyBallItem;
		*max = BAG_BALL_ITEM_MAX;
		break;
	case BAG_POKE_BATTLE:	// 戦闘用
		*item = myitem->MyBattleItem;
		*max = BAG_BATTLE_ITEM_MAX;
		break;
	case BAG_POKE_SEAL:		// シール
		*item = myitem->MySealItem;
		*max = BAG_SEAL_ITEM_MAX;
		break;
	case BAG_POKE_WAZA:		// 技マシン
		*item = myitem->MySkillItem;
		*max = BAG_WAZA_ITEM_MAX;
		break;
	}
	return pocket;
}


//------------------------------------------------------------------
/**
 * @brief	加えるアイテムをチェック
 * @param	item		手持ちアイテム構造体へのポインタ
 * @param	siz			手持ちアイテム構造体のサイズ（個数）
 * @param	id			指定するアイテムのナンバー
 * @param	num			加えるアイテムの個数
 * @param	max			アイテム最大数
 * @retval	成功 = アイテム位置
 * @retval	失敗 = NULL
 */
//------------------------------------------------------------------
static ITEM_ST * AddItemSearch( ITEM_ST * item, u32 siz, u16 id, u16 num, u16 max )
{
	u32	i;
	u32	pos = ITEM_CHECK_ERR;

	for( i=0; i<siz; i++ ){
		if( item[i].id == id ){
			if( ( item[i].no + num ) > max ){
				return NULL;
			}
			return &item[i];
		}
		if( pos == ITEM_CHECK_ERR ){
			if( item[i].id == 0 && item[i].no == 0 ){
				pos = i;
			}
		}
	}

	if( pos == ITEM_CHECK_ERR ){
		return NULL;
	}
	return &item[pos];
}

//------------------------------------------------------------------
/**
 * @brief	アイテムを加える場所を取得
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	item_no		指定するアイテムのナンバー
 * @param	num			アイテムの個数
 * @param	heap		ヒープID
 * @retval	成功 = アイテム位置
 * @retval	失敗 = NULL
 */
//------------------------------------------------------------------
static ITEM_ST * AddItemPosGet( MYITEM_PTR myitem, u16 item_no, u16 num, u32 heap )
{
	ITEM_ST * add;
	u32	max;
	u32	pocket;

	pocket = MyItemDataGet( myitem, item_no, &add, &max, heap );
	if( pocket == BAG_POKE_WAZA ){
		return AddItemSearch( add, max, item_no, num, ITEM_MAX_WAZAMACHINE );
	}
	return AddItemSearch( add, max, item_no, num, ITEM_MAX_NORMAL );
}

//------------------------------------------------------------------
/**
 * @brief	手持ちにアイテムを加えられるかチェック
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	item_no		指定するアイテムのナンバー
 * @param	num			アイテムの個数
 * @param	heap		ヒープID
 * @retval	TRUE	成功
 * @retval	FALSE	失敗
 */
//------------------------------------------------------------------
BOOL MYITEM_AddCheck( MYITEM_PTR myitem, u16 item_no, u16 num, u32 heap )
{
	if( AddItemPosGet( myitem, item_no, num, heap ) == NULL ){
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	手持ちにアイテムを加える
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	item_no		指定するアイテムのナンバー
 * @param	num			アイテムの個数
 * @param	heap		ヒープID
 * @retval	TRUE	成功
 * @retval	FALSE	失敗
 */
//------------------------------------------------------------------
BOOL MYITEM_AddItem( MYITEM_PTR myitem, u16 item_no, u16 num, u32 heap )
{
	ITEM_ST * add = AddItemPosGet( myitem, item_no, num, heap );

	if( add == NULL ){ return FALSE; }

	add->id = item_no;
	add->no += num;

	{
		u32	pocket;
		u32	max;
		pocket = MyItemDataGet( myitem, item_no, &add, &max, heap );
		if( pocket == BAG_POKE_NUTS || pocket == BAG_POKE_WAZA ){
			MYITEM_SortNumber( add, max );
		}
	}

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_ITEM)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_ITEM);
#endif //CRC_LOADCHECK
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	取り除くアイテムをチェック
 * @param	item		手持ちアイテム構造体へのポインタ
 * @param	siz			手持ちアイテム構造体のサイズ（個数）
 * @param	id			指定するアイテムのナンバー
 * @param	num			アイテムの個数
 * @retval	成功 = アイテム位置
 * @retval	失敗 = NULL
 */
//------------------------------------------------------------------
static ITEM_ST * SubItemSearch( ITEM_ST * item, u32 siz, u16 id, u16 num )
{
	u32	i;

	for( i=0; i<siz; i++ ){
		if( item[i].id == id ){
			if( item[i].no >= num ){
				return &item[i];
			}else{
				return NULL;
			}
		}
	}
	return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	アイテムを取り除く場所を取得
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	item_no		指定するアイテムのナンバー
 * @param	num			アイテムの個数
 * @param	heap		ヒープID
 * @retval	成功 = アイテム位置
 * @retval	失敗 = NULL
 */
//------------------------------------------------------------------
static ITEM_ST * SubItemPosGet( MYITEM_PTR myitem, u16 item_no, u16 num, u32 heap )
{
	ITEM_ST * sub;
	u32	max;

	MyItemDataGet( myitem, item_no, &sub, &max, heap );
	return SubItemSearch( sub, max, item_no, num );

/*
	switch( ItemParamGet( item_no, ITEM_PRM_POCKET, heap ) ){
	case BAG_POKE_EVENT:	// 大切な物
		sub = SubItemSearch( myitem->MyEventItem, BAG_EVENT_ITEM_MAX, item_no, num );
		break;
	case BAG_POKE_NORMAL:	// 道具
		sub = SubItemSearch( myitem->MyNormalItem, BAG_NORMAL_ITEM_MAX, item_no, num );
		break;
	case BAG_POKE_NUTS:		// 木の実
		sub = SubItemSearch( myitem->MyNutsItem, BAG_NUTS_ITEM_MAX, item_no, num );
		break;
	case BAG_POKE_DRUG:		// 薬
		sub = SubItemSearch( myitem->MyDrugItem, BAG_DRUG_ITEM_MAX, item_no, num );
		break;
	case BAG_POKE_BALL:		// ボール
		sub = SubItemSearch( myitem->MyBallItem, BAG_BALL_ITEM_MAX, item_no, num );
		break;
	case BAG_POKE_BATTLE:	// 戦闘用
		sub = SubItemSearch( myitem->MyBattleItem, BAG_BATTLE_ITEM_MAX, item_no, num );
		break;
	case BAG_POKE_SEAL:		// シール
		sub = SubItemSearch( myitem->MySealItem, BAG_SEAL_ITEM_MAX, item_no, num );
		break;
	case BAG_POKE_WAZA:		// 技マシン
		sub = SubItemSearch( myitem->MySkillItem, BAG_WAZA_ITEM_MAX, item_no, num );
		break;
	default:
		return NULL;
	}

	return sub;
*/

}

//------------------------------------------------------------------
/**
 * @brief	手持ちからアイテムを取り除く
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	item_no		指定するアイテムのナンバー
 * @param	num			アイテムの個数
 * @param	heap		ヒープID
 * @retval	TRUE	成功
 * @retval	FALSE	失敗
 */
//------------------------------------------------------------------
BOOL MYITEM_SubItem( MYITEM_PTR myitem, u16 item_no, u16 num, u32 heap )
{
	ITEM_ST * sub = SubItemPosGet( myitem, item_no, num, heap );

	if( sub == NULL ){ return FALSE; }

	sub->no -= num;
	if( sub->no == 0 ){
		sub->id = 0;
	}

	{
		u32	max;

		MyItemDataGet( myitem, item_no, &sub, &max, heap );
		MYITEM_SortSpace( sub, max );
	}

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_ITEM)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_ITEM);
#endif //CRC_LOADCHECK
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	手持ちからアイテムを取り除く（ポケット指定）
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	max			検索数
 * @param	item_no		指定するアイテムのナンバー
 * @param	num			アイテムの個数
 * @param	heap		ヒープID
 * @retval	TRUE	成功
 * @retval	FALSE	失敗
 */
//------------------------------------------------------------------
BOOL MYITEM_SubItemDirect( ITEM_ST * myitem, u32 max, u16 item_no, u16 num, u32 heap )
{
	ITEM_ST * sub = SubItemSearch( myitem, max, item_no, num );

	if( sub == NULL ){ return FALSE; }

	sub->no -= num;
	if( sub->no == 0 ){
		sub->id = 0;
	}

	MYITEM_SortSpace( myitem, max );

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_ITEM)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_ITEM);
#endif //CRC_LOADCHECK
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	手持ちアイテム内での存在チェック
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	item_no		指定するアイテムのナンバー
 * @param	num			アイテムの個数
 * @param	heap		ヒープID
 * @retval	TRUE	存在する
 * @retval	FALSE	存在しない
 */
//------------------------------------------------------------------
BOOL MYITEM_CheckItem( MYITEM_PTR myitem, u16 item_no, u16 num, u32 heap )
{
	if( SubItemPosGet( myitem, item_no, num, heap ) == NULL ){
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	ポケットにアイテムが存在するかチェック
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	pocket		ポケット番号
 * @retval	TRUE	存在する
 * @retval	FALSE	存在しない
 */
//------------------------------------------------------------------
BOOL MYITEM_CheckItemPocket( MYITEM_PTR myitem, u32 pocket )
{
	ITEM_ST * item;
	u32	max;
	u32	i;

	switch( pocket ){
	case BAG_POKE_EVENT:	// 大切な物
		item = myitem->MyEventItem;
		max  = BAG_EVENT_ITEM_MAX;
		break;
	case BAG_POKE_NORMAL:	// 道具
		item = myitem->MyNormalItem;
		max  = BAG_NORMAL_ITEM_MAX;
		break;
	case BAG_POKE_NUTS:		// 木の実
		item = myitem->MyNutsItem;
		max  = BAG_NUTS_ITEM_MAX;
		break;
	case BAG_POKE_DRUG:		// 薬
		item = myitem->MyDrugItem;
		max  = BAG_DRUG_ITEM_MAX;
		break;
	case BAG_POKE_BALL:		// ボール
		item = myitem->MyBallItem;
		max  = BAG_BALL_ITEM_MAX;
		break;
	case BAG_POKE_BATTLE:	// 戦闘用
		item = myitem->MyBattleItem;
		max  = BAG_BATTLE_ITEM_MAX;
		break;
	case BAG_POKE_SEAL:		// シール
		item = myitem->MySealItem;
		max  = BAG_SEAL_ITEM_MAX;
		break;
	case BAG_POKE_WAZA:		// 技マシン
		item = myitem->MySkillItem;
		max  = BAG_WAZA_ITEM_MAX;
		break;
	default:
		return FALSE;
	}

	for( i=0; i<max; i++ ){
		if( item[i].id != 0 ){ return TRUE; }
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	手持ちの個数チェック
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	item_no		指定するアイテムのナンバー
 * @param	heap		ヒープID
 * @return	手持ち数
 */
//------------------------------------------------------------------
u16 MYITEM_GetItemNum( MYITEM_PTR myitem, u16 item_no, u32 heap )
{
	ITEM_ST * sub = SubItemPosGet( myitem, item_no, 1, heap );

	if( sub == NULL ){
		return 0;
	}
	return sub->no;
}

//------------------------------------------------------------------
/**
 * @brief	手持ちの個数チェック（ポケット指定）
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	max			検索数
 * @param	item_no		指定するアイテムのナンバー
 * @param	num			アイテムの個数
 * @param	heap		ヒープID
 * @retval	TRUE	成功
 * @retval	FALSE	失敗
 */
//------------------------------------------------------------------
u16 MYITEM_GetItemNumDirect( ITEM_ST * myitem, u32 max, u16 item_no, u32 heap )
{
	ITEM_ST * sub = SubItemSearch( myitem, max, item_no, 1 );

	if( sub == NULL ){ return 0; }

	return sub->no;
}

//------------------------------------------------------------------
/**
 * @brief	指定個所のアイテムを入れ替え
 * @param	p1		入れ替えるデータ1
 * @param	p2		入れ替えるデータ2
 * @return	none
 */
//------------------------------------------------------------------
static void ChengeItem( ITEM_ST * p1, ITEM_ST * p2 )
{
	ITEM_ST	copy;

	copy = *p1;
	*p1  = *p2;
	*p2  = copy;
}

//------------------------------------------------------------------
/**
 * @brief	アイテムソート（スペースをつめる）
 * @param	item	アイテムデータ
 * @param	max		最大値
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_SortSpace( ITEM_ST * item, const u32 max )
{
	u32	i, j;

	for( i=0; i<max-1; i++ ){
		for( j=i+1; j<max; j++ ){
			if( item[i].no == 0 ){
				ChengeItem( &item[i], &item[j] );
			}
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	アイテムソート（番号順）
 * @param	p	ポケットデータ
 * @param	max		最大値
 * @return	none
 */	
//------------------------------------------------------------------
void MYITEM_SortNumber( ITEM_ST * item, const u32 max )
{
	u32	i, j;

	for( i=0; i<max-1; i++ ){
		for( j=i+1; j<max; j++ ){
			if( item[i].no == 0 || ( item[j].no != 0 && item[i].id > item[j].id ) ){
				ChengeItem( &item[i], &item[j] );
			}
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	バッグのデータ作成
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	list		ポケットリスト
 * @param	heap		ヒープID
 */
//------------------------------------------------------------------
#if 0	//まだバッグ画面はないのでとりあえず消す 2008.12.05(金) matsuda
void * MYITEM_MakeBagData( MYITEM_PTR myitem, const u8 * list, u32 heap )
{
	BAG_DATA * bag;
	int i;

	bag = BagSystemDataAlloc( heap );
	for( i=0; list[i] != 0xff; i++ ){
		switch( list[i] ){
		case BAG_POKE_EVENT:
			BagPocketDataMake( bag, myitem->MyEventItem, BAG_POKE_EVENT, i );
			break;
		case BAG_POKE_NORMAL:
			BagPocketDataMake( bag, myitem->MyNormalItem, BAG_POKE_NORMAL, i );
			break;
		case BAG_POKE_NUTS:
			BagPocketDataMake( bag, myitem->MyNutsItem, BAG_POKE_NUTS, i );
			break;
		case BAG_POKE_DRUG:
			BagPocketDataMake( bag, myitem->MyDrugItem, BAG_POKE_DRUG, i );
			break;
		case BAG_POKE_BALL:
			BagPocketDataMake( bag, myitem->MyBallItem, BAG_POKE_BALL, i );
			break;
		case BAG_POKE_BATTLE:
			BagPocketDataMake( bag, myitem->MyBattleItem, BAG_POKE_BATTLE, i );
			break;
		case BAG_POKE_SEAL:
			BagPocketDataMake( bag, myitem->MySealItem, BAG_POKE_SEAL, i );
			break;
		case BAG_POKE_WAZA:
			BagPocketDataMake( bag, myitem->MySkillItem, BAG_POKE_WAZA, i );
			break;
		}
	}
	return bag;
}
#endif


//------------------------------------------------------------------
/**
 * @brief	指定ポケットの指定位置のアイテムを取得
 * @param	myitem		手持ちアイテム構造体へのポインタ
 * @param	pocket		ポケットID
 * @param	pos			位置
 * @return	アイテムデータ
 */
//------------------------------------------------------------------
ITEM_ST * MYITEM_PosItemGet( MYITEM_PTR myitem, u16 pocket, u16 pos )
{
	ITEM_ST * item;
	u16	max;

	switch( pocket ){
	case BAG_POKE_EVENT:	// 大切な物
		item = myitem->MyEventItem;
		max  = BAG_EVENT_ITEM_MAX;
		break;
	case BAG_POKE_NORMAL:	// 道具
		item = myitem->MyNormalItem;
		max  = BAG_NORMAL_ITEM_MAX;
		break;
	case BAG_POKE_NUTS:		// 木の実
		item = myitem->MyNutsItem;
		max  = BAG_NUTS_ITEM_MAX;
		break;
	case BAG_POKE_DRUG:		// 薬
		item = myitem->MyDrugItem;
		max  = BAG_DRUG_ITEM_MAX;
		break;
	case BAG_POKE_BALL:		// ボール
		item = myitem->MyBallItem;
		max  = BAG_BALL_ITEM_MAX;
		break;
	case BAG_POKE_BATTLE:	// 戦闘用
		item = myitem->MyBattleItem;
		max  = BAG_BATTLE_ITEM_MAX;
		break;
	case BAG_POKE_SEAL:		// シール
		item = myitem->MySealItem;
		max  = BAG_SEAL_ITEM_MAX;
		break;
	case BAG_POKE_WAZA:		// 技マシン
		item = myitem->MySkillItem;
		max  = BAG_WAZA_ITEM_MAX;
		break;
	}

	if( pos >= max ){ return NULL; }

	return &item[pos];
}


//------------------------------------------------------------------
/**
 * @brief	デバッグ用：適当に手持ちを生成する
 * @param	myitem	手持ちアイテム構造体へのポインタ
 */
//------------------------------------------------------------------
static const ITEM_ST DebugItem[] = {
	{ ITEM_MASUTAABOORU,	111 },
	{ ITEM_MONSUTAABOORU,	222 },
	{ ITEM_SUUPAABOORU,		333 },
	{ ITEM_HAIPAABOORU,		444 },
	{ ITEM_PUREMIABOORU,	555 },
	{ ITEM_DAIBUBOORU,		666 },
	{ ITEM_TAIMAABOORU,		777 },
	{ ITEM_RIPIITOBOORU,	888 },
	{ ITEM_NESUTOBOORU,		999 },
	{ ITEM_GOOZYASUBOORU,	100 },
	{ ITEM_KIZUGUSURI,		123 },
	{ ITEM_NEMUKEZAMASI,	456 },
	{ ITEM_BATORUREKOODAA,  1},  // バトルレコーダー
	{ ITEM_TAUNMAPPU,		1 },
	{ ITEM_TANKENSETTO,		1 },
	{ ITEM_ZITENSYA,		1 },
	{ ITEM_NANDEMONAOSI,	18 },
	{ ITEM_PIIPIIRIKABAA,	18 },
	{ ITEM_PIIPIIMAKKUSU,	18 },
	{ ITEM_DOKUKESI,		18 },		// どくけし
	{ ITEM_YAKEDONAOSI,		19 },		// やけどなおし
	{ ITEM_KOORINAOSI,		20 },		// こおりなおし
	{ ITEM_MAHINAOSI,		22 },		// まひなおし
	{ ITEM_EFEKUTOGAADO,	54 },		// エフェクトガード
	{ ITEM_KURITHIKATTAA,	55 },		// クリティカッター
	{ ITEM_PURASUPAWAA,		56 },		// プラスパワー
	{ ITEM_DHIFENDAA,		57 },		// ディフェンダー
	{ ITEM_SUPIIDAA,		58 },		// スピーダー
	{ ITEM_YOKUATAARU,		59 },		// ヨクアタール
	{ ITEM_SUPESYARUAPPU,	60 },		// スペシャルアップ
	{ ITEM_SUPESYARUGAADO,	61 },		// スペシャルガード
	{ ITEM_PIPPININGYOU,	62 },		// ピッピにんぎょう
	{ ITEM_ENEKONOSIPPO,	63 },		// エネコのシッポ
	{ ITEM_GENKINOKAKERA,	28 },		// げんきのかけら
	{ ITEM_KAIHUKUNOKUSURI,	28 },		// げんきのかけら
	{ ITEM_PIIPIIEIDO,	28 },
	{ ITEM_PIIPIIEIDAA,	28 },
	{ ITEM_DAAKUBOORU,	13 },		// ダークボール
	{ ITEM_HIIRUBOORU,  14 },		// ヒールボール
	{ ITEM_KUIKKUBOORU,	15 },		// クイックボール
	{ ITEM_PURESYASUBOORU,	16 },	// プレシアボール
	{ ITEM_TOMODATITETYOU,  1},  // ともだち手帳
	{ ITEM_POFINKEESU,  1},  // ポルトケース
	{ ITEM_MOKOSINOMI,	50},	//モコシのみ
	{ ITEM_GOSUNOMI,	50},	//ゴスのみ
	{ ITEM_RABUTANOMI,	50},	//ラブタのみ
};

void Debug_MYITEM_MakeBag(MYITEM_PTR myitem, int heapID)
{
	u32	i;

	GFL_STD_MemClear( myitem->MyEventItem, sizeof(ITEM_ST) * BAG_EVENT_ITEM_MAX );
	GFL_STD_MemClear( myitem->MyNormalItem, sizeof(ITEM_ST) * BAG_NORMAL_ITEM_MAX );
	GFL_STD_MemClear( myitem->MyNutsItem, sizeof(ITEM_ST) * BAG_NUTS_ITEM_MAX );
	GFL_STD_MemClear( myitem->MyDrugItem, sizeof(ITEM_ST) * BAG_DRUG_ITEM_MAX );
	GFL_STD_MemClear( myitem->MyBallItem, sizeof(ITEM_ST) * BAG_BALL_ITEM_MAX );
	GFL_STD_MemClear( myitem->MyBattleItem, sizeof(ITEM_ST) * BAG_BATTLE_ITEM_MAX );
	GFL_STD_MemClear( myitem->MySealItem, sizeof(ITEM_ST) * BAG_SEAL_ITEM_MAX );
	GFL_STD_MemClear( myitem->MySkillItem, sizeof(ITEM_ST) * BAG_WAZA_ITEM_MAX );

	for( i=0; i<NELEMS(DebugItem); i++ ){
		MYITEM_AddItem( myitem, DebugItem[i].id, DebugItem[i].no, heapID );
	}
}


//------------------------------------------------------------------
/**
 * @brief	バッグのカーソル位置データ確保
 * @param	heapID	ヒープID
 * @return	カーソル位置データ
 */
//------------------------------------------------------------------
BAG_CURSOR * MYITEM_BagCursorAlloc( u32 heapID )
{
	BAG_CURSOR * cursor;

	cursor = GFL_HEAP_AllocMemory( heapID, sizeof(BAG_CURSOR) );
	GFL_STD_MemClear16( cursor, sizeof(BAG_CURSOR) );
	return cursor;
}

//------------------------------------------------------------------
/**
 * @brief	フィールドバッグのカーソル位置取得
 * @param	wk		カーソルデータ
 * @param	pocket	ポケットID
 * @param	pos		カーソル位置取得場所
 * @param	scr		スクロールカウンタ取得場所
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_FieldBagCursorGet( BAG_CURSOR * wk, u32 pocket, u8 * pos, u8 * scr )
{
	*pos = wk->fld.pos[pocket];
	*scr = wk->fld.scr[pocket];
}

//------------------------------------------------------------------
/**
 * @brief	フィールドバッグのポケット位置取得
 * @param	wk		カーソルデータ
 * @return	none
 */
//------------------------------------------------------------------
u16 MYITEM_FieldBagPocketGet( BAG_CURSOR * wk )
{
	return wk->fld.pocket;
}

//------------------------------------------------------------------
/**
 * @brief	フィールドバッグのカーソル位置セット
 * @param	wk		カーソルデータ
 * @param	pocket	ポケットID
 * @param	pos		カーソル位置
 * @param	scr		スクロールカウンタ
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_FieldBagCursorSet( BAG_CURSOR * wk, u32 pocket, u8 pos, u8 scr )
{
	wk->fld.pos[pocket] = pos;
	wk->fld.scr[pocket] = scr;
}

//------------------------------------------------------------------
/**
 * @brief	フィールドバッグのポケット位置セット
 * @param	wk		カーソルデータ
 * @param	pocket	ポケットID
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_FieldBagPocketSet( BAG_CURSOR * wk, u16 pocket )
{
	wk->fld.pocket = pocket;
}

//------------------------------------------------------------------
/**
 * @brief	戦闘バッグのカーソル位置取得
 * @param	wk		カーソルデータ
 * @param	pocket	ポケットID
 * @param	pos		カーソル位置取得場所
 * @param	scr		スクロールカウンタ取得場所
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_BattleBagCursorGet( BAG_CURSOR * wk, u32 pocket, u8 * pos, u8 * scr )
{
	*pos = wk->btl.pos[pocket];
	*scr = wk->btl.scr[pocket];
}

//------------------------------------------------------------------
/**
 * @brief	戦闘バッグの最後に使った道具取得
 * @param	wk		カーソルデータ
 * @return	最後に使った道具
 */
//------------------------------------------------------------------
u16 MYITEM_BattleBagLastItemGet( BAG_CURSOR * wk )
{
	return wk->btl.item;
}

//------------------------------------------------------------------
/**
 * @brief	戦闘バッグの最後に使った道具のポケット取得
 * @param	wk		カーソルデータ
 * @return	最後に使った道具のポケット
 */
//------------------------------------------------------------------
u16 MYITEM_BattleBagLastPageGet( BAG_CURSOR * wk )
{
	return wk->btl.page;
}

//------------------------------------------------------------------
/**
 * @brief	戦闘バッグのポケットページのカーソル位置取得
 * @param	wk		カーソルデータ
 * @return	最後に使った道具
 */
//------------------------------------------------------------------
u16 MYITEM_BattleBagPocketPagePosGet( BAG_CURSOR * wk )
{
	return wk->btl.pocket;
}

//------------------------------------------------------------------
/**
 * @brief	戦闘バッグのカーソル位置セット
 * @param	wk		カーソルデータ
 * @param	pocket	ポケットID
 * @param	pos		カーソル位置
 * @param	scr		スクロールカウンタ
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_BattleBagCursorSet( BAG_CURSOR * wk, u32 pocket, u8 pos, u8 scr )
{
	wk->btl.pos[pocket] = pos;
	wk->btl.scr[pocket] = scr;
}

//------------------------------------------------------------------
/**
 * @brief	戦闘バッグのカーソル位置初期化
 * @param	wk		カーソルデータ
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_BattleBagCursorPosInit( BAG_CURSOR * wk )
{
	u32	i;

	for( i=0; i<5; i++ ){
		MYITEM_BattleBagCursorSet( wk, i, 0, 0 );
	}
	MYITEM_BattleBagPocketPagePosSet( wk, 0 );
}

//------------------------------------------------------------------
/**
 * @brief	戦闘バッグの最後に使った道具をセット
 * @param	wk		カーソルデータ
 * @param	item	最後に使ったアイテム
 * @param	page	最後に使ったアイテムのページ
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_BattleBagLastItemSet( BAG_CURSOR * wk, u16 item, u16 page )
{
	wk->btl.item = item;
	wk->btl.page = page;
}

//------------------------------------------------------------------
/**
 * @brief	戦闘バッグのポケットページのカーソル位置セット
 * @param	wk		カーソルデータ
 * @return	最後に使った道具
 */
//------------------------------------------------------------------
void MYITEM_BattleBagPocketPagePosSet( BAG_CURSOR * wk, u16 pocket )
{
	wk->btl.pocket = pocket;
}

// 外部参照インデックスを作る時のみ有効(ゲーム中は無効)
#ifdef CREATE_INDEX
void *Index_Get_Myitem_Offset(MYITEM_PTR item, int type)
{
  switch(type){
  case BAG_POKE_NORMAL: return item->MyNormalItem; break;
  case BAG_POKE_EVENT: return item->MyEventItem; break;
  case BAG_POKE_WAZA: return item->MySkillItem; break;
  case BAG_POKE_SEAL: return item->MySealItem; break;
  case BAG_POKE_DRUG: return item->MyDrugItem; break;
  case BAG_POKE_NUTS: return item->MyNutsItem; break;
  case BAG_POKE_BALL: return item->MyBallItem; break;
  case BAG_POKE_BATTLE: return item->MyBattleItem; break;
  }
}
#endif


