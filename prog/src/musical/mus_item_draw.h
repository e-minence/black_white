//======================================================================
/**
 * @file	dressup_system.h
 * @brief	ミュージカル用 ポケモン描画
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef MUS_ITEM_DRAW_H__
#define MUS_ITEM_DRAW_H__

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
//描画システム
typedef struct _MUS_ITEM_DRAW_SYSTEM MUS_ITEM_DRAW_SYSTEM;
//描画１匹分
typedef struct _MUS_ITEM_DRAW_WORK MUS_ITEM_DRAW_WORK;

//======================================================================
//	proto
//======================================================================

//システムの初期化と開放
MUS_ITEM_DRAW_SYSTEM*	MUS_ITEM_DRAW_InitSystem( GFL_BBD_SYS *bbdSys , u16 itemMax , HEAPID heapId );
void MUS_ITEM_DRAW_TermSystem( MUS_ITEM_DRAW_SYSTEM* work );

//更新
void MUS_ITEM_DRAW_UpdateSystem( MUS_ITEM_DRAW_SYSTEM* work );
void MUS_ITEM_DRAW_DrawSystem( MUS_ITEM_DRAW_SYSTEM* work );

//アイテムの追加・削除
//アイテム番号からリソースの読み込み
GFL_G3D_RES* MUS_ITEM_DRAW_LoadResource( u16 itemIdx );
void MUS_ITEM_DRAW_DeleteResource( GFL_G3D_RES *res );
//番号から追加と、読み込み済みリソースから追加
MUS_ITEM_DRAW_WORK* MUS_ITEM_DRAW_AddItemId( MUS_ITEM_DRAW_SYSTEM* work , u16 itemIdx , VecFx32 *pos );
MUS_ITEM_DRAW_WORK* MUS_ITEM_DRAW_AddResource( MUS_ITEM_DRAW_SYSTEM* work ,  u16 itemIdx ,GFL_G3D_RES* g3DresTex, VecFx32 *pos );
void MUS_ITEM_DRAW_DelItem( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork );

//アイテム番号からARCの番号を調べる
u16 MUS_ITEM_DRAW_GetArcIdx( const u16 itemIdx );
void MUS_ITEM_DRAW_GetPicSize( const u16 fileId , GFL_BBD_TEXSIZ *texSize , u8 *sizeXRate , u8 *sizeYRate );

//絵の変更
void MUS_ITEM_DRAW_ChengeGraphic( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , u16 newId ,  GFL_G3D_RES *newRes);

//値変換
void MUS_ITEM_DRAW_SetDrawEnable( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , BOOL flg );
void MUS_ITEM_DRAW_SetPosition( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , VecFx32 *pos );
void MUS_ITEM_DRAW_GetPosition( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , VecFx32 *pos );
void MUS_ITEM_DRAW_SetSize( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , fx16 sizeX , fx16 sizeY );
void MUS_ITEM_DRAW_GetSize( MUS_ITEM_DRAW_SYSTEM* work , MUS_ITEM_DRAW_WORK *itemWork , fx16 *sizeX , fx16 *sizeY );

//座標チェック系
void MUS_ITEM_DRAW_CheckHit( MUS_ITEM_DRAW_WORK *itemWork , fx16 posX , fx16 posY );

#endif MUS_ITEM_DRAW_H__