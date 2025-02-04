//==============================================================================
/**
 * @file    union_tool.h
 * @brief   ユニオンルームで使用するツール類：オーバーレイに配置
 * @author  matsuda
 * @date    2009.07.25(土)
 */
//==============================================================================
#pragma once

#include "net_app/union/union_subdisp.h"
#include "savedata/wifilist.h"

//==============================================================================
//  外部関数宣言
//==============================================================================
extern BOOL UnionTool_CheckWayOut(FIELDMAP_WORK *fieldWork);
extern UNION_APPEAL UnionTool_PlayCategory_to_AppealNo(UNION_PLAY_CATEGORY play_category);
extern BOOL UnionTool_CheckDwcFriend(WIFI_LIST *wifilist, UNION_BEACON *beacon);
