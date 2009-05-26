//======================================================================
/**
 * @file	fieldmap.h
 * @brief フィールドマップ
 * @author
 * @data
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "field/field_msgbg.h"

#include "field_g3d_mapper.h"
#include "map_matrix.h"

#include "field/fieldmap_proc.h"

#include "field_player.h"
#include "field_camera.h"
#include "field_light.h"
#include "field_fog.h"
#include "weather.h"

#include "field_debug.h"
#include "field_subscreen.h"
#include "field_rail.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	フィールドマップ　コントロールタイプ
//--------------------------------------------------------------
typedef enum
{
	FLDMAP_CTRLTYPE_GRID = 0, ///<グリッドタイプ
	FLDMAP_CTRLTYPE_NOGRID, ///<非グリッドタイプ
	FLDMAP_CTRLTYPE_MAX,
}FLDMAP_CTRLTYPE;

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	DEPEND_FUNCTIONS
//--------------------------------------------------------------
typedef struct
{
	FLDMAP_CTRLTYPE type;
	void (*create_func)(FIELDMAP_WORK*, VecFx32*, u16);
	void (*main_func)(FIELDMAP_WORK*, VecFx32*);
	void (*delete_func)(FIELDMAP_WORK*);
}DEPEND_FUNCTIONS;

//======================================================================
//	extern
//======================================================================
//FIELDMAP_WORK 設定、参照
extern void * FIELDMAP_GetCommSys( const FIELDMAP_WORK *fieldWork );
extern FLDMSGBG * FIELDMAP_GetFldMsgBG( FIELDMAP_WORK *fieldWork );
extern FIELD_CAMERA * FIELDMAP_GetFieldCamera( FIELDMAP_WORK *fieldWork );
extern FIELD_RAIL_MAN * FIELDMAP_GetFieldRailMan( FIELDMAP_WORK *fieldWork );
extern FIELD_LIGHT * FIELDMAP_GetFieldLight( FIELDMAP_WORK *fieldWork );
extern FIELD_FOG_WORK * FIELDMAP_GetFieldFog( FIELDMAP_WORK *fieldWork );
extern FIELD_WEATHER * FIELDMAP_GetFieldWeather( FIELDMAP_WORK *fieldWork );
extern FLDMMDLSYS * FIELDMAP_GetFldMMdlSys( FIELDMAP_WORK *fieldWork );
extern FIELD_DEBUG_WORK * FIELDMAP_GetDebugWork( FIELDMAP_WORK *fieldWork );
extern GAMESYS_WORK * FIELDMAP_GetGameSysWork( FIELDMAP_WORK *fieldWork );
extern MAP_MATRIX * FIELDMAP_GetMapMatrix( FIELDMAP_WORK *fieldWork );
extern HEAPID FIELDMAP_GetHeapID( FIELDMAP_WORK *fieldWork );
extern GFL_BBDACT_SYS * FIELDMAP_GetBbdActSys( FIELDMAP_WORK *fieldWork );
extern FLDMAPPER * FIELDMAP_GetFieldG3Dmapper( FIELDMAP_WORK *fieldWork );
extern void * FIELDMAP_GetMapCtrlWork( FIELDMAP_WORK *fieldWork );
extern void FIELDMAP_SetMapCtrlWork(FIELDMAP_WORK *fieldWork,void *ctrlWork);
extern FIELD_PLAYER * FIELDMAP_GetFieldPlayer( FIELDMAP_WORK *fieldWork );
extern FIELD_SUBSCREEN_WORK* FIELDMAP_GetFieldSubscreenWork(
    FIELDMAP_WORK *fieldWork );
extern void FIELDMAP_SetFieldSubscreenWork(
    FIELDMAP_WORK *fieldWork,FIELD_SUBSCREEN_WORK* pWork );
