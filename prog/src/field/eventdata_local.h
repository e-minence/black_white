//============================================================================================
/**
 * @file	eventdata_local.h
 * @brief
 * @date	2008.12.05
 * @author	tamada GAMEFREAK inc.
 */
//============================================================================================

#pragma once

//------------------------------------------------------------------
//------------------------------------------------------------------
struct _EVENTDATA_HEADER {
	u32 bg_count;
	u32 obj_count;
	u32 connect_count;
	u32 pos_count;
};

//------------------------------------------------------------------
///ドアリンク用データ構造体
//------------------------------------------------------------------
struct _CONNECT_DATA{
	VecFx32 pos;
	u16	link_zone_id;
	u16	link_exit_id;
	u8	exit_dir;
	u8	exit_type;
};

//------------------------------------------------------------------
///BG話かけデータ構造体
//------------------------------------------------------------------
struct _BG_TALK_DATA{
	u16	id;			// ID
	u16	type;		// データタイプ
	int	gx;			// X座標
	int	gz;			// Y座標
	int	height;		// 高さ
	u16	dir;		// 話しかけ方向タイプ
};

//------------------------------------------------------------------
///POS発動イベントデータ構造体
//------------------------------------------------------------------
struct _POS_EVENT_DATA{
	u16	id;			//ID
	u16	gx;			//x
	u16	gz;			//z
	u16	sx;			//sizeX
	u16	sz;			//sizeZ
	u16	height;		//height
	u16 param;
	u16 workID;
};

