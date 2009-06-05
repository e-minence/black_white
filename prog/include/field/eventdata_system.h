//============================================================================================
/**
 * @file	eventdata_system.h
 * @brief	イベント起動用データのロード・保持システム
 * @author	tamada
 * @date	2008.11.20
 */
//============================================================================================
#pragma once

#define EVENTDATA_ID_NONE (0xffff) ///<イベントデータなし

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _EVDATA_SYS EVENTDATA_SYSTEM;

//------------------------------------------------------------------
//------------------------------------------------------------------
extern EVENTDATA_SYSTEM * EVENTDATA_SYS_Create(HEAPID heapID);
extern void EVENTDATA_SYS_Delete(EVENTDATA_SYSTEM * evdata);
extern void EVENTDATA_SYS_Clear(EVENTDATA_SYSTEM * evdata);
extern void EVENTDATA_SYS_Load(EVENTDATA_SYSTEM * evdata, u16 mapid);

#include "field/eventwork.h"

extern u16 EVENTDATA_CheckPosEvent(
  const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos );

extern u16 EVENTDATA_CheckTalkBGEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const VecFx32 *pos, u16 talk_dir );
