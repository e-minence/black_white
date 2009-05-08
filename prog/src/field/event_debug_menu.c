//======================================================================
/**
 *
 * @file	event_debug_menu.c
 * @brief	フィールドデバッグメニューの制御イベント
 * @author	kagaya
 * @date	2008.11.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "field/field_msgbg.h"

#include "message.naix"
#include "msg/msg_d_field.h"

#include "field/zonedata.h"
#include "field_comm/field_comm_main.h"
#include "arc/fieldmap/zone_id.h"
#include "field/eventdata_sxy.h"

#include "event_debug_menu.h"
#include "event_mapchange.h"

#include "field_comm/field_comm_main.h"
#include "field_comm/field_comm_debug.h"
#include "ircbattle/ircbattlemenu.h"
#include "event_ircbattle.h"
#include "event_wificlub.h"
#include "field_subscreen.h"

#include "font/font.naix"

#include  "field/weather_no.h"
#include  "weather.h"
#include  "msg/msg_d_tomoya.h"

#include "field_debug.h"

//======================================================================
//	define
//======================================================================
#define DEBUG_BGFRAME_MENU (GFL_BG_FRAME1_M)	//使用フレーム
#define DEBUG_BGFRAME_MSG  (GFL_BG_FRAME2_M)	//使用フレーム
#define DEBUG_MENU_PANO	(14)
#define DEBUG_FONT_PANO	(15)

#define D_MENU_CHARSIZE_X (12)		//メニュー横幅
#define D_MENU_CHARSIZE_Y (16)		//メニュー縦幅

//--------------------------------------------------------------
///	DMENURET
//--------------------------------------------------------------
typedef enum
{
	DMENURET_CONTINUE,
	DMENURET_FINISH,
	DMENURET_NEXTEVENT,
}DMENURET;

//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	DEBUG_MENU_EVENT_WORK
//--------------------------------------------------------------
typedef struct _TAG_DEBUG_MENU_EVENT_WORK DEBUG_MENU_EVENT_WORK;

//--------------------------------------------------------------
///	メニュー呼び出し関数
///	BOOL TRUE=イベント継続 FALSE==デバッグメニューイベント終了
//--------------------------------------------------------------
typedef BOOL (* D_MENU_CALLPROC)(DEBUG_MENU_EVENT_WORK*);

//--------------------------------------------------------------
///	DEBUG_MENU_LISTDATA
//--------------------------------------------------------------
typedef struct
{
	u16 charsize_x;
	u16 charsize_y;
	u32 max;
	const FLDMENUFUNC_LIST *list;
}DEBUG_MENU_LISTDATA;

//--------------------------------------------------------------
///	DEBUG_MENU_EVENT_WORK
//--------------------------------------------------------------
struct _TAG_DEBUG_MENU_EVENT_WORK
{
	u32 page_id;
	HEAPID heapID;
	GMEVENT *gmEvent;
	GAMESYS_WORK *gmSys;
	FIELD_MAIN_WORK * fieldWork;
	
	D_MENU_CALLPROC call_proc;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
};

//======================================================================
//	proto
//======================================================================
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk );

static BOOL DMenuCallProc_GridCamera( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_GridScaleSwitch( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_GridScaleControl( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_OpenStartComm( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenStartInvasion( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_MapZoneSelect( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenCommDebugMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenIRCBTLMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenClubMenu( DEBUG_MENU_EVENT_WORK *wk );

static DMenuCallProc_MapSeasonSelect( DEBUG_MENU_EVENT_WORK *wk );
static DMenuCallProc_SubscreenSelect( DEBUG_MENU_EVENT_WORK *wk );

static void DEBUG_SetMenuWorkZoneIDNameAll(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID );
static void DEBUG_SetMenuWorkZoneIDName(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID, u32 zoneID );

static BOOL DMenuCallProc_ControlCamera( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_CameraList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_FldMMdlList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_ControlLight( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_WeatherList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_FieldPosData( DEBUG_MENU_EVENT_WORK *wk );

//--------------------------------------------------------------
///	デバッグメニューリスト　汎用
///	データを追加する事でメニューの項目も増えます。
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuList[] =
{
	{ DEBUG_FIELD_C_CHOICE00, DMenuCallProc_OpenCommDebugMenu },
	{ DEBUG_FIELD_STR02, DMenuCallProc_ControlCamera },
	{ DEBUG_FIELD_STR17, DMenuCallProc_FieldPosData },
	{ DEBUG_FIELD_STR16, DMenuCallProc_WeatherList },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
};

//--------------------------------------------------------------
///	デバッグメニューリスト	グリッド実験マップ用。
///	データを追加する事でメニューの項目も増えます。
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuListGrid[] =
{
	{ DEBUG_FIELD_STR17, DMenuCallProc_FieldPosData },
	{ DEBUG_FIELD_STR02, DMenuCallProc_ControlCamera },
	{ DEBUG_FIELD_STR03, DMenuCallProc_GridScaleSwitch },
	{ DEBUG_FIELD_STR04, DMenuCallProc_GridScaleControl },
	{ DEBUG_FIELD_STR05, DMenuCallProc_MapZoneSelect },
	{ DEBUG_FIELD_STR06, DMenuCallProc_MapSeasonSelect},
	{ DEBUG_FIELD_STR07, DMenuCallProc_CameraList },
	{ DEBUG_FIELD_STR13, DMenuCallProc_FldMMdlList },
	{ DEBUG_FIELD_C_CHOICE00, DMenuCallProc_OpenCommDebugMenu },
	{ DEBUG_FIELD_STR12, DMenuCallProc_OpenIRCBTLMenu },
	{ DEBUG_FIELD_STR19, DMenuCallProc_OpenClubMenu },
	{ DEBUG_FIELD_STR15, DMenuCallProc_ControlLight },
	{ DEBUG_FIELD_STR16, DMenuCallProc_WeatherList },
  { DEBUG_FIELD_STR_SUBSCRN, DMenuCallProc_SubscreenSelect },
	{ DEBUG_FIELD_STR01, NULL },
};

//--------------------------------------------------------------
///	デバッグメニューリストテーブル
//--------------------------------------------------------------
static const DEBUG_MENU_LISTDATA DATA_DebugMenuListTbl[] =
{
	{	//実験マップ 橋
		D_MENU_CHARSIZE_X,
		D_MENU_CHARSIZE_Y,
		NELEMS(DATA_DebugMenuList),
		DATA_DebugMenuList
	},
	{	//実験マップ　グリッド移動
		D_MENU_CHARSIZE_X,
		D_MENU_CHARSIZE_Y,
		NELEMS(DATA_DebugMenuListGrid),
		DATA_DebugMenuListGrid
	},
	{	//実験マップ　橋
		D_MENU_CHARSIZE_X,
		D_MENU_CHARSIZE_Y,
		NELEMS(DATA_DebugMenuList),
		DATA_DebugMenuList
	},
	{	//実験マップ　グリッド移動
		D_MENU_CHARSIZE_X,
		D_MENU_CHARSIZE_Y,
		NELEMS(DATA_DebugMenuListGrid),
		DATA_DebugMenuListGrid
	},
};

//メニュー最大数
#define D_MENULISTTBL_MAX (NELEMS(DATA_DebugMenuListTbl))

//--------------------------------------------------------------
///	メニューヘッダー
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_DebugMenuListHeader =
{
	1,		//リスト項目数
	9,		//表示最大項目数
	0,		//ラベル表示Ｘ座標
	13,		//項目表示Ｘ座標
	0,		//カーソル表示Ｘ座標
	0,		//表示Ｙ座標
	1,		//表示文字色
	15,		//表示背景色
	2,		//表示文字影色
	0,		//文字間隔Ｘ
	2,		//文字間隔Ｙ
	FLDMENUFUNC_SKIP_LRKEY,	//ページスキップタイプ
	12,		//文字サイズX(ドット
	12,		//文字サイズY(ドット
	0,		//表示座標X キャラ単位
	0,		//表示座標Y キャラ単位
	0,		//表示サイズX キャラ単位
	0,		//表示サイズY キャラ単位
};	

//======================================================================
//	イベント：フィールドデバッグメニュー
//======================================================================
//--------------------------------------------------------------
/**
 * フィールドデバッグメニューイベント起動
 * @param	gsys	GAMESYS_WORK
 * @param	fieldWork	FIELD_MAIN_WORK
 * @param	heapID	HEAPID
 * @param	page_id	デバッグメニューページ
 * @retval	GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_DebugMenu(
	GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork,
	HEAPID heapID, u16 page_id )
{
	DEBUG_MENU_EVENT_WORK * dmew;
	GMEVENT * event;
	
	event = GMEVENT_Create(
		gsys, NULL, DebugMenuEvent, sizeof(DEBUG_MENU_EVENT_WORK));
	
	dmew = GMEVENT_GetEventWork(event);
	MI_CpuClear8( dmew, sizeof(DEBUG_MENU_EVENT_WORK) );
	
	dmew->gmSys = gsys;
	dmew->gmEvent = event;
	dmew->fieldWork = fieldWork;
	dmew->heapID = heapID;
	dmew->page_id = page_id;
	
	if( dmew->page_id >= D_MENULISTTBL_MAX ){
		OS_Printf( "debug menu number error\n" );
		dmew->page_id = 0;
	}
	
	return event;
}

//--------------------------------------------------------------
/**
 * イベント：フィールドデバッグメニュー
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk )
{
	DEBUG_MENU_EVENT_WORK *work = wk;
	
	switch (*seq) {
	case 0:
		{
			FLDMSGBG *msgBG;
			FLDMENUFUNC_HEADER head;
			FLDMENUFUNC_LISTDATA *listdata;
			const FLDMENUFUNC_LIST *menulist;
			const DEBUG_MENU_LISTDATA *d_menu_listdata;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			
			work->msgData = FLDMSGBG_CreateMSGDATA(
						msgBG, NARC_message_d_field_dat );
			
			d_menu_listdata = &DATA_DebugMenuListTbl[work->page_id];
			menulist = d_menu_listdata->list;
			
			listdata = FLDMENUFUNC_CreateMakeListData(
				menulist, d_menu_listdata->max, work->msgData, work->heapID );
			
			head = DATA_DebugMenuListHeader;
			FLDMENUFUNC_InputHeaderListSize(
				&head, d_menu_listdata->max, 1, 1,
				d_menu_listdata->charsize_x, d_menu_listdata->charsize_y );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &head, listdata );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//操作無し
				break;
			}else if( ret == FLDMENUFUNC_CANCEL ){	//キャンセル
				(*seq)++;
			}else{							//決定
				work->call_proc = (D_MENU_CALLPROC)ret;
				(*seq)++;
			}
		}
		break;
	case 2:
		{
			FLDMENUFUNC_DeleteMenu( work->menuFunc );
			
			if( work->call_proc != NULL ){
				if( work->call_proc(work) == TRUE ){
					return( GMEVENT_RES_CONTINUE );
				}
			}
			
			return( GMEVENT_RES_FINISH );
		}
		break;
	}

	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//	デバッグメニュー呼び出し
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　グリッド用カメラ
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridCamera( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_MENU_EVENT_WORK *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_Camera( fieldWork );
	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　グリッド用スケール切り替え
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridScaleSwitch( DEBUG_MENU_EVENT_WORK *wk )
{
#if 0
	DEBUG_MENU_EVENT_WORK *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_ScaleChange( fieldWork );
#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　グリッド用スケール調節
 * @param	wk	DEBUG_MENU_EVENT_WORK *
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridScaleControl( DEBUG_MENU_EVENT_WORK *wk )
{
#if 0
	DEBUG_MENU_EVENT_WORK *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_ScaleControl( fieldWork );
#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　通信デバッグ子メニュー
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_OpenCommDebugMenu( DEBUG_MENU_EVENT_WORK *wk )
{
	GMEVENT *event = wk->gmEvent;
	const HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	GAMESYS_WORK	*gameSys	= wk->gmSys;
	FIELD_COMM_DEBUG_WORK *work;
	
	GMEVENT_Change( event,
		FIELD_COMM_DEBUG_CommDebugMenu, FIELD_COMM_DEBUG_GetWorkSize() );
	
	work = GMEVENT_GetEventWork( event );
	FIELD_COMM_DEBUG_InitWork( heapID , gameSys , fieldWork , event , work );

	return( TRUE );
}


//--------------------------------------------------------------
/**
 * IRCBATTLEメニュー呼びだし
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_OpenIRCBTLMenu( DEBUG_MENU_EVENT_WORK *wk )
{
	GMEVENT *event = wk->gmEvent;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	GAMESYS_WORK	*gameSys	= wk->gmSys;

    EVENT_IrcBattle(gameSys, fieldWork, event);
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * WiFiClubメニュー呼びだし
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_OpenClubMenu( DEBUG_MENU_EVENT_WORK *wk )
{
	GMEVENT *event = wk->gmEvent;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	GAMESYS_WORK	*gameSys	= wk->gmSys;

    EVENT_WiFiClub(gameSys, fieldWork, event);
	return( TRUE );
}





//======================================================================
//	デバッグメニュー どこでもジャンプ
//======================================================================
//--------------------------------------------------------------
///	DEBUG_ZONESEL_EVENT_WORK どこでもジャンプ処理用ワーク
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
}DEBUG_ZONESEL_EVENT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuZoneSelectEvent(
		GMEVENT *event, int *seq, void *work );

///	どこでもジャンプ　メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_ZoneSel =
{
	1,		//リスト項目数
	10,		//表示最大項目数
	0,		//ラベル表示Ｘ座標
	13,		//項目表示Ｘ座標
	0,		//カーソル表示Ｘ座標
	0,		//表示Ｙ座標
	1,		//表示文字色
	15,		//表示背景色
	2,		//表示文字影色
	0,		//文字間隔Ｘ
	1,		//文字間隔Ｙ
	FLDMENUFUNC_SKIP_LRKEY,	//ページスキップタイプ
	12,		//文字サイズX(ドット
	12,		//文字サイズY(ドット
	0,		//表示座標X キャラ単位
	0,		//表示座標Y キャラ単位
	0,		//表示サイズX キャラ単位
	0,		//表示サイズY キャラ単位
};

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　どこでもジャンプ
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_MapZoneSelect( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_ZONESEL_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuZoneSelectEvent, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：どこでもジャンプ
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuZoneSelectEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_ZONESEL_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			u32 max = ZONEDATA_GetZoneIDMax();
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_ZoneSel;
			FLDMENUFUNC_LISTDATA *pMenuListData;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
					msgBG, NARC_message_d_field_dat );
			pMenuListData = FLDMENUFUNC_CreateListData( max, work->heapID );
			DEBUG_SetMenuWorkZoneIDNameAll( pMenuListData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 11, 16 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu(
					msgBG, &menuH, pMenuListData );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//操作無し
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_CANCEL ){	//キャンセル
				return( GMEVENT_RES_FINISH );
			}
			
			DEBUG_EVENT_ChangeEventMapChange(	//決定
				work->gmSys, work->gmEvent, work->fieldWork, ret );
		}
		break;
	}

	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//	デバッグメニュー　四季ジャンプ
//======================================================================
//--------------------------------------------------------------
//	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuSeasonSelectEvent(
		GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　四季マップ間移動
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static DMenuCallProc_MapSeasonSelect( DEBUG_MENU_EVENT_WORK *wk )
{
	HEAPID heapID = wk->heapID;
	GMEVENT *event = wk->gmEvent;
	GAMESYS_WORK *gsys = wk->gmSys;
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
	PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
	ZONEID zone_id = PLAYERWORK_getZoneID( player );
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_ZONESEL_EVENT_WORK *work;
	
		GMEVENT_Change( event,
			DMenuSeasonSelectEvent, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
		work = GMEVENT_GetEventWork( event );
		MI_CpuClear8( work, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
	
		work->gmSys = gsys;
		work->gmEvent = event;
		work->heapID = heapID;
		work->fieldWork = fieldWork;
		return( TRUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
#include "gamesystem/pm_season.h"
static const FLDMENUFUNC_LIST DATA_SeasonMenuList[PMSEASON_TOTAL] =
{
	{ DEBUG_FIELD_STR_SPRING, (void*)PMSEASON_SPRING },
	{ DEBUG_FIELD_STR_SUMMER, (void*)PMSEASON_SUMMER },
	{ DEBUG_FIELD_STR_AUTUMN, (void*)PMSEASON_AUTUMN },
	{ DEBUG_FIELD_STR_WINTER, (void*)PMSEASON_WINTER },
};

//--------------------------------------------------------------
/**
 * イベント：四季ジャンプ
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuSeasonSelectEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_ZONESEL_EVENT_WORK *work = wk;

	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = NELEMS(DATA_SeasonMenuList);
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_ZoneSel;  //流用
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
				msgBG, NARC_message_d_field_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_SeasonMenuList, max, work->msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 16, 7 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( work->msgData );
		}
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_NULL ){	//操作無し
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_CANCEL ){	//キャンセル
				return( GMEVENT_RES_FINISH );
			}else{
				GMEVENT *event;
				GAMEDATA *gdata = GAMESYSTEM_GetGameData( work->gmSys );
				PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
				const VecFx32 *pos = PLAYERWORK_getPosition( player );
				u16 dir = PLAYERWORK_getDirection( player );
				ZONEID zone_id = PLAYERWORK_getZoneID(player);
				
				if( (dir>0x2000) && (dir<0x6000) ){
					dir = EXIT_TYPE_LEFT;
				}else if( (dir >= 0x6000) && (dir <= 0xa000) ){
					dir = EXIT_TYPE_DOWN;
				}else if( (dir > 0xa000) && (dir < 0xe000) ){
					dir = EXIT_TYPE_RIGHT;
				}else{
					dir = EXIT_TYPE_UP;
				}

				GAMEDATA_SetSeasonID(gdata, ret);
				event = DEBUG_EVENT_ChangeMapPos(
					work->gmSys, work->fieldWork, zone_id, pos, dir );
				GMEVENT_ChangeEvent( work->gmEvent, event );
				OS_Printf( "x = %xH, z = %xH\n", pos->x, pos->z );
			}
		}
		break;
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
typedef struct {  
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	FLDMENUFUNC *menuFunc;
  FIELD_SUBSCREEN_WORK * subscreen;
}DEBUG_MENU_EVENT_SUBSCRN_SELECT_WORK, DMESSWORK;

//--------------------------------------------------------------
//	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuSubscreenSelectEvent(
		GMEVENT *event, int *seq, void *wk );
static void setupTouchCameraSubscreen(DMESSWORK * dmess);
static void setupSoundViewerSubscreen(DMESSWORK * dmess);
static void setupNormalSubscreen(DMESSWORK * dmess);
static void setupTopMenuSubscreen(DMESSWORK * dmess);

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　四季マップ間移動
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static DMenuCallProc_SubscreenSelect( DEBUG_MENU_EVENT_WORK *wk )
{
	HEAPID heapID = wk->heapID;
	GMEVENT *event = wk->gmEvent;
	GAMESYS_WORK *gsys = wk->gmSys;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DMESSWORK *work;
	
		GMEVENT_Change( event,
			DMenuSubscreenSelectEvent, sizeof(DMESSWORK) );
		work = GMEVENT_GetEventWork( event );
		MI_CpuClear8( work, sizeof(DMESSWORK) );
	
		work->gmSys = gsys;
		work->gmEvent = event;
		work->heapID = heapID;
		work->fieldWork = fieldWork;
    work->subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
		return( TRUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_SubcreenMenuList[PMSEASON_TOTAL] =
{
	{ DEBUG_FIELD_STR_SUBSCRN01, (void*)setupTouchCameraSubscreen },
	{ DEBUG_FIELD_STR_SUBSCRN02, (void*)setupSoundViewerSubscreen },
	{ DEBUG_FIELD_STR_SUBSCRN03, (void*)setupNormalSubscreen },
	{ DEBUG_FIELD_STR_SUBSCRN04, (void*)setupTopMenuSubscreen },
};

//--------------------------------------------------------------
/**
 * イベント：四季ジャンプ
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuSubscreenSelectEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DMESSWORK *work = wk;

	switch( *seq )
  {
	case 0:
		{
			FLDMSGBG *msgBG;
	    GFL_MSGDATA *msgData;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = NELEMS(DATA_SubcreenMenuList);
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_ZoneSel;  //流用
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_d_field_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_SubcreenMenuList, max, msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 16, 7 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( msgData );
		}
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_NULL ){	//操作無し
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_CANCEL ){	//キャンセル
				return( GMEVENT_RES_FINISH );
			}else{
        typedef void (* CHANGE_FUNC)(DMESSWORK*);
        CHANGE_FUNC func = (CHANGE_FUNC)ret;
        func(work);
        //FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(work->fieldWork), ret);
				return( GMEVENT_RES_FINISH );
			}
		}
		break;
	}
	
	return GMEVENT_RES_CONTINUE ;
}

//--------------------------------------------------------------
/**
 * @brief サブスクリーンのカメラ操作に同期させる
 *
 * @todo  カメラをバインドしてから、どのように切り離すか？を確定させる
 */
//--------------------------------------------------------------
static void setupTouchCameraSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_Change(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
  { 
    void * inner_work;
    FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(dmess->fieldWork);
    inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(dmess->subscreen);
    FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work);
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupSoundViewerSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_Change(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupNormalSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_Change(dmess->subscreen, FIELD_SUBSCREEN_NORMAL);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupTopMenuSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_Change(dmess->subscreen, FIELD_SUBSCREEN_TOPMENU);
}

//======================================================================
//	デバッグ用ZoneID文字列変換
//======================================================================
//--------------------------------------------------------------
//	文字コード
//--------------------------------------------------------------
#define ASCII_EOM (0x00)
#define ASCII_0 (0x30)
#define ASCII_9 (0x39)
#define ASCII_A (0x41)
#define ASCII_Z (0x5a)

//半角
#define UTF16H_0 (0x0030)
#define UTF16H_9 (0x0039)
#define UTF16H_A (0x0041)
#define UTF16H_Z (0x005a)

//全角
#define UTF16_0 (0xff10)
#define UTF16_9 (0xff19)
#define UTF16_A (0xff21)
#define UTF16_Z (0xff3a)

//--------------------------------------------------------------
/**
 * ASCII->UTF-16
 * @param	str	ASCII 半角英数字
 * @retval	u16 UTF-16 str
 */
//--------------------------------------------------------------
static u16 DEBUG_ASCIICODE_UTF16( u8 code )
{
	if( code == ASCII_EOM ){
		return( GFL_STR_GetEOMCode() );
	}
	
	if( code >= ASCII_0 && code <= ASCII_9 ){
		code -= ASCII_0;
		return( UTF16H_0 + code );
	}
	
	if( code >= ASCII_A && code <= ASCII_Z ){
		code -= ASCII_A;
		return( UTF16H_A + code );
	}
	
	GF_ASSERT( 0 ); 					//未対応文字
	return( GFL_STR_GetEOMCode() );
}

//--------------------------------------------------------------
/**
 * ZONE_ID->STRCODE
 * @param	heapID	文字列バッファ確保用ヒープID
 * @param	zoneID	文字列を取得したいzoneID
 * @retval	u16*	zoneID文字列が格納されたu16*(開放が必要
 */
//--------------------------------------------------------------
static u16 * DEBUG_GetZoneNameUTF16( u32 heapID, u32 zoneID )
{
	int i;
	u16 utf16,utf16_eom;
	u16 *pStrBuf;
	char name8[128];
	
	pStrBuf = GFL_HEAP_AllocClearMemory( heapID, sizeof(u16)*128 );
	ZONEDATA_GetZoneName( heapID, name8, zoneID );
	utf16_eom = GFL_STR_GetEOMCode();
//	OS_Printf( "変換 %s\n", name8 );
	
	for( i = 0; i < 128; i++ ){
		utf16 = DEBUG_ASCIICODE_UTF16( name8[i] );
		pStrBuf[i] = utf16;
		
		if( utf16 == utf16_eom ){
			return( pStrBuf );
		}
	}
	
	pStrBuf[i-1] = utf16_eom;	//文字数オーバー
	GF_ASSERT( 0 );
	return( pStrBuf );
}

//--------------------------------------------------------------
/**
 * Zone ID Name -> BMP_MENULIST_DATA 
 * @param	heapID	テンポラリ文字列確保用heapID
 * @param	zoneID	文字列を取得したいzoneID
 * @param	strBuf	文字列格納先STRBUF
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetSTRBUF_ZoneIDName(
		u32 heapID, u32 zoneID, STRBUF *strBuf )
{
	u16 *str = DEBUG_GetZoneNameUTF16( heapID, zoneID );
	GFL_STR_SetStringCode( strBuf, str );
	GFL_HEAP_FreeMemory( str );
}

//--------------------------------------------------------------
/**
 * 何処でもジャンプ用BMP_MENULIST_DATAセット
 * @param	list	セット先BMP_MENULIST_DATA
 * @param	heapID	文字列バッファ確保用HEAPID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkZoneIDNameAll(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID )
{
	int id,max = ZONEDATA_GetZoneIDMax();
	STRBUF *strBuf = GFL_STR_CreateBuffer( 128, heapID );
	
	for( id = 0; id < max; id++ ){
		GFL_STR_ClearBuffer( strBuf );
		DEBUG_SetSTRBUF_ZoneIDName( heapID, id, strBuf );
		FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
	}
	
	GFL_HEAP_FreeMemory( strBuf );
}

//--------------------------------------------------------------
/**
 * ZONE_ID文字列をBMP_MENULIST_DATAにセット
 * @param	list	セット先BMP_MENULIST_DATA
 * @param	heapID	文字列バッファ確保用HEAPID
 * @param	zoneID	セットするZONE_ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkZoneIDName(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID, u32 zoneID )
{
	int max = ZONEDATA_GetZoneIDMax();
	STRBUF *strBuf = GFL_STR_CreateBuffer( 128, heapID );
	
	GFL_STR_ClearBuffer( strBuf );
	DEBUG_SetSTRBUF_ZoneIDName( heapID, zoneID, strBuf );
	FLDMENUFUNC_AddStringListData( list, strBuf, zoneID, heapID );
	GFL_HEAP_FreeMemory( strBuf );
}

//======================================================================
//	デバッグメニュー　カメラ操作
//======================================================================
#define CM_RT_SPEED (FX32_ONE/8)
#define CM_HEIGHT_MV (FX32_ONE*2)
#define CM_NEARFAR_MV (FX32_ONE)

//--------------------------------------------------------------
///	DEBUG_CTLCAMERA_WORK カメラ操作ワーク1
//--------------------------------------------------------------
typedef struct
{
	int vanish;
	GAMESYS_WORK *gsys;
	GMEVENT *event;
	HEAPID heapID;
	FIELD_MAIN_WORK *fieldWork;
	FLDMSGBG *pMsgBG;
	FLDMSGWIN *pMsgWin;
	STRBUF *pStrBuf;
}DEBUG_CTLCAMERA_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlCamera(
		GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　カメラ操作
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_ControlCamera( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_CTLCAMERA_WORK *work;
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	
	GMEVENT_Change( event, DMenuControlCamera, sizeof(DEBUG_CTLCAMERA_WORK) );
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_CTLCAMERA_WORK) );
	
	work->gsys = gsys;
	work->event = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	work->pMsgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
	work->pStrBuf = GFL_STR_CreateBuffer( 128, work->heapID );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：カメラ操作
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlCamera(
		GMEVENT *event, int *seq, void *wk )
{
	int update = FALSE;
	DEBUG_CTLCAMERA_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		work->pMsgWin = FLDMSGWIN_Add( work->pMsgBG, NULL, 21, 1, 10, 10 );
		update = TRUE;
		(*seq)++;
	case 1:
		{
			fx32 height, near, far;
			u16 dir,length;
			int trg = GFL_UI_KEY_GetTrg();
			int cont = GFL_UI_KEY_GetCont();
			FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( work->fieldWork );
			
			if( trg & PAD_BUTTON_B ){
				(*seq)++;
				break;
			}
			
			dir = FIELD_CAMERA_GetDirectionOnXZ( camera );
			length = FIELD_CAMERA_GetLengthOnXZ( camera );
			height = FIELD_CAMERA_GetHeightOnXZ( camera );
			near = FIELD_CAMERA_GetNear( camera );
			far = FIELD_CAMERA_GetFar( camera );

			if( cont & PAD_BUTTON_R ){
				dir -= CM_RT_SPEED;
				update = TRUE;
			}
	
			if( cont & PAD_BUTTON_L ){
				dir += CM_RT_SPEED;
				update = TRUE;
			}
	
			if( cont & PAD_KEY_LEFT ){
				if( length > 8 ){
					length -= 8;
					update = TRUE;
				}
			}

			if( cont & PAD_KEY_RIGHT ){
				if( length < 4096 ){
					length += 8;
					update = TRUE;
				}
			}
		
			if( !((cont & PAD_BUTTON_Y) || (cont & PAD_BUTTON_A)) ){	// near far操作以外
				if( cont & PAD_KEY_UP ){
					height -= CM_HEIGHT_MV;
					update = TRUE;
				}
		
				if( cont & PAD_KEY_DOWN ){
					height += CM_HEIGHT_MV;
					update = TRUE;
				}	
			}

			if( (cont & PAD_BUTTON_Y) ){	// near
				if( cont & PAD_KEY_UP ){
					near += CM_NEARFAR_MV;
					update = TRUE;
				}
		
				if( cont & PAD_KEY_DOWN ){
					near -= CM_NEARFAR_MV;
					update = TRUE;
				}	
			}

			if( (cont & PAD_BUTTON_A) ){	// far
				if( cont & PAD_KEY_UP ){
					far += CM_NEARFAR_MV;
					update = TRUE;
				}
		
				if( cont & PAD_KEY_DOWN ){
					far -= CM_NEARFAR_MV;
					update = TRUE;
				}	
			}

			FIELD_CAMERA_SetDirectionOnXZ( camera, dir );
			FIELD_CAMERA_SetLengthOnXZ( camera, length );
			FIELD_CAMERA_SetHeightOnXZ( camera, height );
			FIELD_CAMERA_SetNear( camera, near );
			FIELD_CAMERA_SetFar( camera, far );
			
			if( update == TRUE && work->vanish == FALSE ){
				int len = 128;
				char sjis[128];
				u16 ucode[128];
				
				FLDMSGWIN_ClearWindow( work->pMsgWin );

				sprintf( sjis, "LENGTH:%xH \0", length );
				STD_ConvertStringSjisToUnicode(
						ucode, &len, sjis, NULL, NULL );
				GFL_STR_SetStringCodeOrderLength( 
						work->pStrBuf, ucode, len );
				FLDMSGWIN_PrintStrBuf( work->pMsgWin, 1, 1, work->pStrBuf );
				
				len = 128;
				sprintf( sjis, "HEIGHT:%xH \0", height );
				STD_ConvertStringSjisToUnicode(
						ucode, &len, sjis, NULL, NULL );
				GFL_STR_SetStringCodeOrderLength(
						work->pStrBuf, ucode, len );
				FLDMSGWIN_PrintStrBuf( work->pMsgWin, 1, 12, work->pStrBuf );
				
				len = 128;
				sprintf( sjis, "DIR:%xH \0", dir );
				STD_ConvertStringSjisToUnicode(
						ucode, &len, sjis, NULL, NULL );
				GFL_STR_SetStringCodeOrderLength(
						work->pStrBuf, ucode, len );
				FLDMSGWIN_PrintStrBuf( work->pMsgWin, 1, 23, work->pStrBuf );

				len = 128;
				sprintf( sjis, "NEAR:%xH \0", near );
				STD_ConvertStringSjisToUnicode(
						ucode, &len, sjis, NULL, NULL );
				GFL_STR_SetStringCodeOrderLength(
						work->pStrBuf, ucode, len );
				FLDMSGWIN_PrintStrBuf( work->pMsgWin, 1, 34, work->pStrBuf );

				len = 128;
				sprintf( sjis, "FAR:%xH \0", far );
				STD_ConvertStringSjisToUnicode(
						ucode, &len, sjis, NULL, NULL );
				GFL_STR_SetStringCodeOrderLength(
						work->pStrBuf, ucode, len );
				FLDMSGWIN_PrintStrBuf( work->pMsgWin, 1, 45, work->pStrBuf );
				
				FIELD_CAMERA_Main( camera, 0 );
			}
		}
		break;
	case 2:
		FLDMSGWIN_Delete( work->pMsgWin );
		GFL_STR_DeleteBuffer( work->pStrBuf );
		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//	デバッグメニュー　テストカメラリスト
//======================================================================
//--------------------------------------------------------------
///	DEBUG_TESTCAMERALIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
}DEBUG_TESTCAMERALIST_EVENT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuTestCameraListEvent(
		GMEVENT *event, int *seq, void *work );

///カメラリスト最大
#define TESTCAMERALISTMAX (4)

///テストカメラリスト メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_TestCameraList =
{
	1,		//リスト項目数
	4,		//表示最大項目数
	0,		//ラベル表示Ｘ座標
	13,		//項目表示Ｘ座標
	0,		//カーソル表示Ｘ座標
	0,		//表示Ｙ座標
	1,		//表示文字色
	15,		//表示背景色
	2,		//表示文字影色
	0,		//文字間隔Ｘ
	1,		//文字間隔Ｙ
	FLDMENUFUNC_SKIP_LRKEY,	//ページスキップタイプ
	12,		//文字サイズX(ドット
	12,		//文字サイズY(ドット
	0,		//表示座標X キャラ単位
	0,		//表示座標Y キャラ単位
	0,		//表示サイズX キャラ単位
	0,		//表示サイズY キャラ単位
};

///テストカメラメニューリスト
static const FLDMENUFUNC_LIST DATA_TestCameraMenuList[TESTCAMERALISTMAX] =
{
	{ DEBUG_FIELD_STR09, (void*)0 },
	{ DEBUG_FIELD_STR10, (void*)1 },
	{ DEBUG_FIELD_STR11, (void*)2 },
	{ DEBUG_FIELD_STR08, (void*)3 },
};

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　テストカメラリスト
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_CameraList( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_TESTCAMERALIST_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuTestCameraListEvent, sizeof(DEBUG_TESTCAMERALIST_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_TESTCAMERALIST_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：テストカメラリスト
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuTestCameraListEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_ZONESEL_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = TESTCAMERALISTMAX;
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_TestCameraList;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
				msgBG, NARC_message_d_field_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_TestCameraMenuList, max, work->msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 8, 7 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//操作無し
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );
			
			if( ret != FLDMENUFUNC_CANCEL ){	//決定
				u16 length[TESTCAMERALISTMAX] =
					{ 0x0090, 0x0078, 0x0080, 0x0078 };
				fx32 height[TESTCAMERALISTMAX] =
					{ 0xae000, 0xa0000, 0xab000, 0xd8000 };
				u16 dir[TESTCAMERALISTMAX] =
					{ 0x0000, 0x0000, 0x0000, 0x0000 };
				FIELD_CAMERA *camera =
					FIELDMAP_GetFieldCamera( work->fieldWork );
				FIELD_CAMERA_SetDirectionOnXZ( camera, dir[ret] );
				FIELD_CAMERA_SetLengthOnXZ( camera, length[ret] );
				FIELD_CAMERA_SetHeightOnXZ( camera, height[ret] );
			}
			
			return( GMEVENT_RES_FINISH );
		}
		break;
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//	デバッグメニュー　動作モデル一覧
//======================================================================
//--------------------------------------------------------------
///	DEBUG_FLDMMDL_LIST_EVENT_WORK 動作モデルリスト処理用ワーク
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
	FLDMMDLSYS *fldmmdlsys;

	u16 obj_code;
	u16 res_add;
	FLDMMDL *fmmdl;
}DEBUG_FLDMMDLLIST_EVENT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuFldMMdlListEvent(
		GMEVENT *event, int *seq, void *wk );
static u16 * DEBUG_GetOBJCodeStrBuf( HEAPID heapID, u16 code );
static void DEBUG_SetMenuWorkFldMMdlList(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID );

///	動作モデルリスト メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_FldMMdlList =
{
	1,		//リスト項目数
	10,		//表示最大項目数
	0,		//ラベル表示Ｘ座標
	13,		//項目表示Ｘ座標
	0,		//カーソル表示Ｘ座標
	0,		//表示Ｙ座標
	1,		//表示文字色
	15,		//表示背景色
	2,		//表示文字影色
	0,		//文字間隔Ｘ
	1,		//文字間隔Ｙ
	FLDMENUFUNC_SKIP_LRKEY,	//ページスキップタイプ
	12,		//文字サイズX(ドット
	12,		//文字サイズY(ドット
	0,		//表示座標X キャラ単位
	0,		//表示座標Y キャラ単位
	0,		//表示サイズX キャラ単位
	0,		//表示サイズY キャラ単位
};

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　動作モデル一覧
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_FldMMdlList( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_FLDMMDLLIST_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuFldMMdlListEvent, sizeof(DEBUG_FLDMMDLLIST_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_FLDMMDLLIST_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	
	{
		GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
		work->fldmmdlsys = GAMEDATA_GetFldMMdlSys( gdata );
	}

	return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：動作モデル一覧
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuFldMMdlListEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_FLDMMDLLIST_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			u32 max = OBJCODEMAX;
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_FldMMdlList;
			FLDMENUFUNC_LISTDATA *pMenuListData;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
					msgBG, NARC_message_d_field_dat );
			pMenuListData = FLDMENUFUNC_CreateListData( max, work->heapID );
			DEBUG_SetMenuWorkFldMMdlList( pMenuListData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 11, 16 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu(
					msgBG, &menuH, pMenuListData );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//操作無し
				break;
			}
			
			if( ret == FLDMENUFUNC_CANCEL || ret == NONDRAW ){ //キャンセル
				FLDMENUFUNC_DeleteMenu( work->menuFunc );
				return( GMEVENT_RES_FINISH );
			}
			
			work->obj_code = ret;
			work->res_add = FLDMMDL_BLACTCONT_AddOBJCodeRes(
					work->fldmmdlsys, work->obj_code );
			
			{
				//VecFx32 pos;
				FLDMMDL *jiki;
				FLDMMDL_HEADER head = {
					0,	///<識別ID
					0,	///<表示するOBJコード
					MV_RND,	///<動作コード
					0,	///<イベントタイプ
					0,	///<イベントフラグ
					0,	///<イベントID
					0,	///<指定方向
					0,	///<指定パラメタ 0
					0,	///<指定パラメタ 1
					0,	///<指定パラメタ 2
					4,	///<X方向移動制限
					4,	///<Z方向移動制限
					0,	///<グリッドX
					0,	///<グリッドZ
					0,	///<Y値 fx32型
				};
				
				jiki = FLDMMDLSYS_SearchOBJID(
					work->fldmmdlsys, FLDMMDL_ID_PLAYER );
				
				head.id = 250;
				head.gx = FLDMMDL_GetGridPosX( jiki ) + 2;
				head.gz = FLDMMDL_GetGridPosZ( jiki );
				head.y = FLDMMDL_GetVectorPosY( jiki );
				head.obj_code = work->obj_code;
				work->fmmdl = FLDMMDLSYS_AddFldMMdl(
					work->fldmmdlsys, &head, 0 );
			}
			
			(*seq)++;
			break;
		case 2:
			{
				int key_trg = GFL_UI_KEY_GetTrg();
				FLDMMDL_UpdateMoveProc( work->fmmdl );

				if( (key_trg & PAD_BUTTON_B) ){
					FLDMMDL_Delete( work->fmmdl );
					
					if( work->res_add == TRUE ){
						FLDMMDL_BLACTCONT_DeleteOBJCodeRes(
								work->fldmmdlsys, work->obj_code );
					}
					
					(*seq) = 1;
				}
			}
			break;
		}
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * 動作モデルOBJコード->STRCODE
 * @param	heapID	文字列バッファ確保用ヒープID
 * @param	code	文字列を取得したいOBJコード
 * @retval	u16*	文字列が格納されたu16*(開放が必要
 */
//--------------------------------------------------------------
static u16 * DEBUG_GetOBJCodeStrBuf( HEAPID heapID, u16 code )
{
	int i;
	u16 utf16,utf16_eom;
	u16 *pStrBuf;
	u8 *name8;
	
	pStrBuf = GFL_HEAP_AllocClearMemory( heapID,
			sizeof(u16)*DEBUG_OBJCODE_STR_LENGTH );
	name8 = DEBUG_FLDMMDL_GetOBJCodeString( code, heapID );
	utf16_eom = GFL_STR_GetEOMCode();
//	OS_Printf( "変換 %s\n", name8 );
	
	for( i = 0; i < DEBUG_OBJCODE_STR_LENGTH; i++ ){
		utf16 = DEBUG_ASCIICODE_UTF16( name8[i] );
		pStrBuf[i] = utf16;
		if( utf16 == utf16_eom ){
			break;
		}
	}
	
	GFL_HEAP_FreeMemory( name8 );
	
	if( i >= DEBUG_OBJCODE_STR_LENGTH ){ //文字数オーバー
		GF_ASSERT( 0 );
		pStrBuf[DEBUG_OBJCODE_STR_LENGTH-1] = utf16_eom;
	}
	
	return( pStrBuf );
}

//--------------------------------------------------------------
/**
 * 動作モデルリスト用BMP_MENULIST_DATAセット
 * @param	list	セット先BMP_MENULIST_DATA
 * @param	heapID	文字列バッファ確保用HEAPID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkFldMMdlList(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID )
{
	u16 *str;
	int id,max = OBJCODEMAX;
	STRBUF *strBuf = GFL_STR_CreateBuffer( DEBUG_OBJCODE_STR_LENGTH, heapID );
	
	for( id = 0; id < max; id++ ){
		GFL_STR_ClearBuffer( strBuf );
		str = DEBUG_GetOBJCodeStrBuf( heapID, id );
		GFL_STR_SetStringCode( strBuf, str );
		GFL_HEAP_FreeMemory( str );
		FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
	}
	
	GFL_HEAP_FreeMemory( strBuf );
}




//======================================================================
//	デバッグメニュー　ライト操作
//======================================================================

//--------------------------------------------------------------
///	DEBUG_CTLIGHT_WORK ライト操作ワーク
//--------------------------------------------------------------
typedef struct
{
	GAMESYS_WORK *gsys;
	GMEVENT *event;
	HEAPID heapID;
	FIELD_MAIN_WORK *fieldWork;

	GFL_BMPWIN* p_win;
}DEBUG_CTLLIGHT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlLight(
		GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　カメラ操作
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_ControlLight( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_CTLLIGHT_WORK *work;
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	
	GMEVENT_Change( event, DMenuControlLight, sizeof(DEBUG_CTLLIGHT_WORK) );
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_CTLLIGHT_WORK) );
	
	work->gsys = gsys;
	work->event = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：カメラ操作
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlLight(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_CTLLIGHT_WORK *work = wk;
	FIELD_LIGHT* p_light;

	// ライト取得
	p_light = FIELDMAP_GetFieldLight( work->fieldWork );
	
	switch( (*seq) ){
	case 0:
		// ライト管理開始
		FIELD_LIGHT_DEBUG_Init( p_light, work->heapID );

		// インフォーバーの非表示
		FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(work->fieldWork));
		GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

		// ビットマップウィンドウ初期化
		{
			static const GFL_BG_BGCNT_HEADER header_sub3 = {
				0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
				GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
			};

			GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
			GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
			GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

			// パレット情報を転送
			GFL_ARC_UTIL_TransVramPalette(
				ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, work->heapID );
			
			// ビットマップウィンドウを作成
			work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
				0, 0, 32, 24,
				FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_F );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
			GFL_BMPWIN_MakeScreen( work->p_win );
			GFL_BMPWIN_TransVramCharacter( work->p_win );
			GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );
		}

		FIELD_LIGHT_DEBUG_PrintData( p_light, work->p_win );

		(*seq)++;
	case 1:
		// ライト管理メイン
		FIELD_LIGHT_DEBUG_Control( p_light );
		FIELD_LIGHT_DEBUG_PrintData( p_light, work->p_win );

		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
			(*seq)++;
		}
		break;
	case 2:
		// ライト管理終了
		FIELD_LIGHT_DEBUG_Exit( p_light );

		// ビットマップウィンドウ破棄
		{
			GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
			GFL_BMPWIN_Delete( work->p_win );
		}

		// インフォーバーの表示
    FIELDMAP_SetFieldSubscreenWork(work->fieldWork,
        FIELD_SUBSCREEN_Init( work->heapID, FIELD_SUBSCREEN_NORMAL ));

		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}




//======================================================================
//	デバッグメニュー　てんきリスト
//======================================================================
//--------------------------------------------------------------
///	DEBUG_WEATERLIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
}DEBUG_WEATERLIST_EVENT_WORK;


//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuWeatherListEvent(
		GMEVENT *event, int *seq, void *work );

///テストカメラリスト メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_WeatherList =
{
	1,		//リスト項目数
	6,		//表示最大項目数
	0,		//ラベル表示Ｘ座標
	13,		//項目表示Ｘ座標
	0,		//カーソル表示Ｘ座標
	0,		//表示Ｙ座標
	1,		//表示文字色
	15,		//表示背景色
	2,		//表示文字影色
	0,		//文字間隔Ｘ
	1,		//文字間隔Ｙ
	FLDMENUFUNC_SKIP_LRKEY,	//ページスキップタイプ
	12,		//文字サイズX(ドット
	12,		//文字サイズY(ドット
	0,		//表示座標X キャラ単位
	0,		//表示座標Y キャラ単位
	0,		//表示サイズX キャラ単位
	0,		//表示サイズY キャラ単位
};

///てんきメニューリスト
static const FLDMENUFUNC_LIST DATA_WeatherMenuList[] =
{
	{ D_TOMOYA_WEATEHR00, (void*)WEATHER_NO_SUNNY },
	{ D_TOMOYA_WEATEHR01, (void*)WEATHER_NO_SNOW },
	{ D_TOMOYA_WEATEHR02, (void*)WEATHER_NO_RAIN },
	{ D_TOMOYA_WEATEHR03, (void*)WEATHER_NO_STORM },
	{ D_TOMOYA_WEATEHR05, (void*)WEATHER_NO_SPARK },
	{ D_TOMOYA_WEATEHR06, (void*)WEATHER_NO_SNOWSTORM },
	{ D_TOMOYA_WEATEHR07, (void*)WEATHER_NO_ARARE },
	{ D_TOMOYA_WEATEHR08, (void*)WEATHER_NO_MIRAGE },
};
#define DEBUG_WEATHERLIST_LIST_MAX	( NELEMS(DATA_WeatherMenuList) )

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　天気リスト
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_WeatherList( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_WEATERLIST_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuWeatherListEvent, sizeof(DEBUG_WEATERLIST_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_WEATERLIST_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * イベント：てんきリスト
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuWeatherListEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_ZONESEL_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = DEBUG_WEATHERLIST_LIST_MAX;
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_WeatherList;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
				msgBG, NARC_message_d_tomoya_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_WeatherMenuList, max, work->msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 8, 11 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//操作無し
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );
			
			if( ret != FLDMENUFUNC_CANCEL ){	//決定
				FIELD_WEATHER_Change( FIELDMAP_GetFieldWeather( work->fieldWork ), ret );
			}
			
			return( GMEVENT_RES_FINISH );
		}
		break;
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//	デバッグメニュー　位置情報
//======================================================================
extern FIELD_DEBUG_WORK * FIELDMAP_GetDebugWork( FIELD_MAIN_WORK *fieldWork );

//--------------------------------------------------------------
/**
 * デバッグメニュー呼び出し　フィールド位置情報
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=イベント継続
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_FieldPosData( DEBUG_MENU_EVENT_WORK *wk )
{
	FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( wk->fieldWork );
	FIELD_DEBUG_SetPosPrint( debug );
	return( FALSE );
}
