//======================================================================
/**
 * @file	dressup_system.h
 * @brief	ドレスアップ 試着メイン
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "musical_item.naix"

#include "test/ariizumi/ari_debug.h"
#include "musical/mus_item_draw.h"
#include "musical/dressup/dup_fitting_item.h"


//======================================================================
//	define
//======================================================================
static const u8 ITEM_HIT_SIZE = 12;

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

struct _FIT_ITEM_WORK
{
	u16	itemId;
	u16	newItemId;
	MUS_ITEM_DRAW_WORK	*itemWork;
	GFL_POINT	pos;
	fx32	scale;
	u16		cnt;	//汎用カウンタ
	
	//単方向リスト構造
	FIT_ITEM_WORK* next;
};

struct _FIT_ITEM_GROUP
{
	HEAPID heapId;
	u16	workSize;
	u16	num;
	u16 max;
	FIT_ITEM_WORK *start;
	FIT_ITEM_WORK *end;
	
};
//======================================================================
//	proto
//======================================================================

//--------------------------------------------------------------
//グループの作成と削除
//--------------------------------------------------------------
FIT_ITEM_GROUP* DUP_FIT_ITEMGROUP_CreateGroup( HEAPID heapId , const u16 maxNum )
{
	FIT_ITEM_GROUP *group = GFL_HEAP_AllocMemory( heapId , sizeof(FIT_ITEM_GROUP) );
	
	group->heapId = heapId;
	group->num = 0;
	group->max = maxNum;
	group->start = NULL;
	group->end = NULL;
	
	return group;
}

void DUP_FIT_ITEMGROUP_DeleteGroup( FIT_ITEM_GROUP* group )
{
	if( group->num > 0 )
	{
		ARI_TPrintf("DressUp ItemGroup Delete: Item is not release yet[least %d]\n",group->num);
	}
	GFL_HEAP_FreeMemory( group );
}

//--------------------------------------------------------------
//アイテムの追加・削除(リンクにつなぐだけなのでメモリの取得開放はしない
//--------------------------------------------------------------
void DUP_FIT_ITEMGROUP_AddItem( FIT_ITEM_GROUP *group , FIT_ITEM_WORK *item )
{
	GF_ASSERT_MSG(item->next == NULL,"DressUp ItemGroup AddItem:Item link yet!!\n");
	if( group->num >= group->max )
	{
		GF_ASSERT_MSG(NULL,"DressUp ItemGroup AddItem:max over![max %d]\n",group->max);
		return;
	}
	if( group->start == NULL )
	{
		group->start = item;
		group->end	 = item;
	}
	else
	{
		FIT_ITEM_WORK *endItem = group->end;
		endItem->next = item;
		group->end = item;
	}
	item->next = NULL;
	group->num++;
	ARI_TPrintf("group add[%d]\n",group->num);
}
void DUP_FIT_ITEMGROUP_AddItemTop( FIT_ITEM_GROUP *group , FIT_ITEM_WORK *item )
{
	GF_ASSERT_MSG(item->next == NULL,"DressUp ItemGroup AddItem:Item link yet!!\n");
	if( group->num >= group->max )
	{
		GF_ASSERT_MSG(NULL,"DressUp ItemGroup AddItem:max over![max %d]\n",group->max);
		return;
	}
	if( group->start == NULL )
	{
		group->start = item;
		group->end	 = item;
		item->next = NULL;
	}
	else
	{
		FIT_ITEM_WORK *topItem = group->start;
		item->next = topItem;
		group->start = item;
	}
	group->num++;
	ARI_TPrintf("group add[%d]\n",group->num);
}

void DUP_FIT_ITEMGROUP_RemoveItem( FIT_ITEM_GROUP *group , FIT_ITEM_WORK *item )
{
	FIT_ITEM_WORK *befItem = NULL;
	FIT_ITEM_WORK *listItem = group->start;
	while( listItem != item && listItem != NULL )
	{
		befItem = listItem;
		listItem = listItem->next;
	}
	if( listItem == NULL )
	{
		GF_ASSERT_MSG(NULL,"DressUp ItemGroup RemoveItem:item not found!!\n");
		return;
	}

	if( befItem != NULL )
	{
		befItem->next = listItem->next;
	}
	else
	{
		group->start = listItem->next;
	}
	
	if( group->end == item )
	{
		group->end = befItem;
	}
	listItem->next = NULL;
	group->num--;
	ARI_TPrintf("groupe remove[%d]\n",group->num);
}

//--------------------------------------------------------------
//リンクのスタートを取得
//--------------------------------------------------------------
FIT_ITEM_WORK* DUP_FIT_ITEMGROUP_GetStartItem( FIT_ITEM_GROUP *group )
{
	return group->start;
}

//--------------------------------------------------------------
//各数値の取得
//--------------------------------------------------------------
const u16 DUP_FIT_ITEMGROUP_GetItemNum( FIT_ITEM_GROUP *group )
{
	return group->num;
}
const BOOL DUP_FIT_ITEMGROUP_IsItemMax( FIT_ITEM_GROUP *group )
{
	return ( group->num == group->max ? TRUE : FALSE );
}

//--------------------------------------------------------------
//アイテムの作成・削除
//--------------------------------------------------------------
FIT_ITEM_WORK* DUP_FIT_ITEM_CreateItem( HEAPID heapId , MUS_ITEM_DRAW_SYSTEM *itemDrawSys , u16 itemId , GFL_G3D_RES *res , VecFx32 *pos )
{
	FIT_ITEM_WORK *item = GFL_HEAP_AllocMemory( heapId , sizeof( FIT_ITEM_WORK ));
	
	item->next = NULL;
	item->itemId = itemId;
	item->itemWork = MUS_ITEM_DRAW_AddResource( itemDrawSys , itemId , res , pos );
	item->cnt = 0;
	return item;
}
void DUP_FIT_ITEM_DeleteItem( FIT_ITEM_WORK *item , MUS_ITEM_DRAW_SYSTEM *itemDrawSys )
{
	MUS_ITEM_DRAW_DelItem( itemDrawSys , item->itemWork );
	GFL_HEAP_FreeMemory( item );
}

//--------------------------------------------------------------
//絵の読み替え
//--------------------------------------------------------------
void DUP_FIT_ITEM_ChengeGraphic( FIT_ITEM_WORK *item , MUS_ITEM_DRAW_SYSTEM *itemDrawSys , u16 itemId , GFL_G3D_RES *res )
{
	item->itemId = itemId;
	MUS_ITEM_DRAW_ChengeGraphic( itemDrawSys , item->itemWork , itemId , res );
}

//--------------------------------------------------------------
//次のアイテムの取得
//--------------------------------------------------------------
FIT_ITEM_WORK* DUP_FIT_ITEM_GetNextItem( FIT_ITEM_WORK *item )
{
	return item->next;
}

//--------------------------------------------------------------
//各数値の取得
//--------------------------------------------------------------
void DUP_FIT_ITEM_SetItemIdx( FIT_ITEM_WORK *item , const u16 idx)
{
	item->itemId = idx;
}
u16 DUP_FIT_ITEM_GetItemIdx( FIT_ITEM_WORK *item )
{
	return item->itemId;
}
void DUP_FIT_ITEM_SetNewItemIdx( FIT_ITEM_WORK *item , const u16 idx)
{
	item->newItemId = idx;
}
u16 DUP_FIT_ITEM_GetNewItemIdx( FIT_ITEM_WORK *item )
{
	return item->newItemId;
}
MUS_ITEM_DRAW_WORK* DUP_FIT_ITEM_GetItemDrawWork( FIT_ITEM_WORK *item )
{
	return item->itemWork;
}

void DUP_FIT_ITEM_SetPosition( FIT_ITEM_WORK *item , const GFL_POINT *pos )
{
	item->pos = *pos;
}
GFL_POINT* DUP_FIT_ITEM_GetPosition( FIT_ITEM_WORK *item )
{
	return &item->pos;
}
void DUP_FIT_ITEM_SetScale( FIT_ITEM_WORK *item , const fx32 scale )
{
	item->scale = scale;
}
fx32 DUP_FIT_ITEM_GetScale( FIT_ITEM_WORK *item )
{
	return item->scale;
}
void DUP_FIT_ITEM_SetCount( FIT_ITEM_WORK *item , const u16 count )
{
	item->cnt = count;
}
u16	 DUP_FIT_ITEM_GetCount( FIT_ITEM_WORK *item )
{
	return item->cnt;
}

//--------------------------------------------------------------
//座標とのチェック
//--------------------------------------------------------------
const BOOL DUP_FIT_ITEM_CheckHit( FIT_ITEM_WORK *item , u32 posX , u32 posY )
{
	GFL_BBD_TEXSIZ texSize;
	u8 xRate,yRate;
	GFL_POINT ofsPos;
	s16 subX,subY;

	u16 arcIdx = MUS_ITEM_DRAW_GetArcIdx( item->itemId );
	MUS_ITEM_DRAW_GetPicSize( item->itemWork , &xRate,&yRate );
	MUS_ITEM_DRAW_GetOffsetPos( item->itemWork , &ofsPos );
	if( MUS_ITEM_DRAW_GetUseOffset( NULL , item->itemWork ) == TRUE )
	{
		subX = (item->pos.x-ofsPos.x) - posX;
		subY = (item->pos.y-ofsPos.y) - posY;
	}
	else
	{
		subX = item->pos.x - posX;
		subY = item->pos.y - posY;
	}

	if( subX < ITEM_HIT_SIZE*xRate &&
		subX >-ITEM_HIT_SIZE*xRate &&
		subY < ITEM_HIT_SIZE*yRate &&
		subY >-ITEM_HIT_SIZE*yRate )
	{
		return TRUE;
	}
	return FALSE;
}
const u32 DUP_FIT_ITEM_CheckLength( FIT_ITEM_WORK *item , u32 posX , u32 posY )
{
	const s16 subX = item->pos.x - posX;
	const s16 subY = item->pos.y - posY;
	return ( subX*subX + subY*subY );
}

const u16 DUP_FIT_ITEM_CheckLengthSqrt( FIT_ITEM_WORK *item , u32 posX , u32 posY )
{
	const u32 len = DUP_FIT_ITEM_CheckLength( item , posX , posY );
	CP_SetSqrt32( len );
	return CP_GetSqrtResult16();
}

#if DEB_ARI
void	DUP_FIT_ITEM_DumpList( FIT_ITEM_GROUP *group , u8 dispIdx )
{
	int	i = 0;
	FIT_ITEM_WORK *listItem = group->start;
	ARI_TPrintf("DUMP ITEM LIST[%d] [Num:%d]\n",dispIdx,group->num);
	while( listItem != NULL )
	{
		ARI_TPrintf("[id:%2d][itemId:%2d]\n",i,listItem->itemId);
		listItem = listItem->next;
		i++;
	}
	ARI_TPrintf("DUMP ITEM LIST[%d] FNISH\n");
	
}
#endif
