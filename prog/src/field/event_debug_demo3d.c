//=============================================================================
/**
 *
 *	@file		event_debug_demo3d.c
 *	@brief  デバッグメニュ−：デモ再生
 *	@author		hosaka genya
 *	@data		2009.12.07
 *
 */
//=============================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_comm.h"

#include "demo/demo3d.h"

#include "fieldmap.h"

#include "message.naix"  // NARC_xxxx
#include "msg/msg_d_field.h"  // for DEBUG_FIELD_STR_xxxx
#include "event_debug_local.h"  // for DEBUG_MENU_INITIALIZER

#include "event_fieldmap_control.h"

#include "event_debug_demo3d.h"


//=============================================================================
/**
 *								定数定義
 */
//=============================================================================

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================
//======================================================================================
// ■イベントワーク
//======================================================================================
typedef struct
{
  HEAPID           heapID;
  GAMESYS_WORK*      gsys;
  GAMEDATA*         gdata;
  GAME_COMM_SYS_PTR gcsp;
  FIELDMAP_WORK* fieldmap;
  FLDMENUFUNC*   menuFunc;
  DEMO3D_PARAM   param;

  int            demo_id;
  int            scene_id;
} EVENT_WORK;

typedef struct _SCENE_TABLE{
  u8  demo_id:4;  
  u8  scene_id:4;  
}SCENE_TABLE;

static const SCENE_TABLE DATA_SceneTable[] = {
  { DEMO3D_ID_C_CRUISER, 0 },       //1
  { DEMO3D_ID_C_CRUISER_PORT, 0 },  //2
	{ DEMO3D_ID_G_SHIP_PORT, 0 },     //3
	{ DEMO3D_ID_INTRO_TOWN, 0 },      //4
	{ DEMO3D_ID_N_CASTLE, 0 },        //5
	{ DEMO3D_ID_N_LEGEND_BORN_B, 0 }, //6
	{ DEMO3D_ID_N_LEGEND_BORN_W, 0 }, //7
	{ DEMO3D_ID_N_LEGEND_COME_B, 0 }, //8
	{ DEMO3D_ID_N_LEGEND_COME_W, 0 }, //9
	{ DEMO3D_ID_N_LEGEND_MEAT_B, 0 }, //10
	{ DEMO3D_ID_N_LEGEND_MEAT_W, 0 }, //11
	{ DEMO3D_ID_RIZA_BRIDGE, 0 },     //12
	{ DEMO3D_ID_V_BOAT_PORT, 0 },     //13
	{ DEMO3D_ID_F_WHEEL, 0 },         //14
	{ DEMO3D_ID_F_WHEEL, 1 },         //15
	{ DEMO3D_ID_F_WHEEL, 2 },         //16
	{ DEMO3D_ID_F_WHEEL, 3 },         //17
	{ DEMO3D_ID_F_WHEEL, 4 },         //18
	{ DEMO3D_ID_F_WHEEL, 5 },         //19
	{ DEMO3D_ID_F_WHEEL, 6 },         //20
	{ DEMO3D_ID_F_WHEEL, 7 },         //21
	{ DEMO3D_ID_F_WHEEL, 8 },         //22
};

#define DEMO_SCENE_MAX  (NELEMS(DATA_SceneTable))

//=============================================================================
/**
 *							プロトタイプ宣言
 */
//=============================================================================
static GMEVENT_RESULT debugMenuDemo3DSelectEvent( GMEVENT *event, int *seq, void *wk );
static void DebugMenuList_Make( GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work );
static u16 DebugMenuList_GetMax( GAMESYS_WORK* gsys, void* cb_work );

//=============================================================================
/**
 *							データテーブル
 */
//=============================================================================

/// どこでもジャンプ　メニューヘッダー
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_ZoneSel =
{
  1,    //リスト項目数
  9,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  1,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
};

// デバッグメニュー初期化
static const DEBUG_MENU_INITIALIZER DebugDemo3DJumpMenuData = {
  NARC_message_d_field_dat,
  0,
  NULL,
  &DATA_DebugMenuList_ZoneSel, //流用
  1, 1, 16, 17,
  DebugMenuList_Make,
  DebugMenuList_GetMax,
};

//=============================================================================
/**
 *								外部公開関数
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  イベント開始
 *
 *	@param	GAMESYS_WORK * gsys
 *	@param	heapID 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_FLDMENU_Demo3DSelect( GAMESYS_WORK * gsys, HEAPID heapID )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // 生成
  event = GMEVENT_Create( gsys, NULL, debugMenuDemo3DSelectEvent, sizeof(EVENT_WORK) );
   // 初期化
  work = GMEVENT_GetEventWork( event );
  work->heapID   = heapID;
  work->gsys     = gsys;
  work->gdata    = GAMESYSTEM_GetGameData( gsys );
  work->gcsp     = GAMESYSTEM_GetGameCommSysPtr( gsys );
  work->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  work->menuFunc = NULL;

  return event;
}

//=============================================================================
/**
 *								static関数
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  イベント Main処理
 *
 *	@param	GMEVENT *event
 *	@param	*seq
 *	@param	*wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT debugMenuDemo3DSelectEvent( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK *work = wk;
  GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr( work->gsys );
 
  switch( (*seq) )
  {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldmap, work->heapID,  
                                        &DebugDemo3DJumpMenuData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //操作無し
        break;
      }
      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){ //キャンセル
        return( GMEVENT_RES_FINISH );
      }
      work->demo_id = DATA_SceneTable[ret].demo_id; 
      work->scene_id = DATA_SceneTable[ret].scene_id; 
      if( GameCommSys_BootCheck( work->gcsp ) == GAME_COMM_NO_NULL ){
        (*seq) += 2;
      }else{
        GameCommSys_ExitReq( work->gcsp );
        (*seq) += 1;
      }
    }
    break;
  case 2:
    if(GameCommSys_BootCheck( work->gcsp) == GAME_COMM_NO_NULL){
      (*seq) += 1;
    }
    break;
  case 3:
     DEMO3D_PARAM_SetFromRTC( &work->param, work->gsys, work->demo_id, work->scene_id );
     GMEVENT_CallEvent( event, EVENT_FieldSubProc( work->gsys, work->fieldmap,
        FS_OVERLAY_ID(demo3d), &Demo3DProcData, &work->param ) );
     (*seq)++;
    break;
  case 4:
    (*seq) = 0;
    break;
  }

  return ( GMEVENT_RES_CONTINUE );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  リスト初期化 リスト生成
 *
 *	@param	GAMESYS_WORK * gsys
 *	@param	*list
 *	@param	heapID
 *	@param	msgData
 *	@param	cb_work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void DebugMenuList_Make( GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  int id;
  STRBUF *strBuf1 = GFL_STR_CreateBuffer( 64, heapID );
  FIELDMAP_WORK * fieldmap  = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMSGBG * msgBG          = FIELDMAP_GetFldMsgBG( fieldmap );
  GFL_MSGDATA * pMsgData    = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_d_field_dat );
  
  for( id = 0; id < DEMO_SCENE_MAX; id++ ){
    GFL_STR_ClearBuffer( strBuf1 );
    GFL_MSG_GetString( pMsgData, DEBUG_FIELD_DEMO3D_DEMO_01 + id, strBuf1 );  // 文字列を取得
    FLDMENUFUNC_AddStringListData( list, strBuf1, id, heapID );
  }
  
  GFL_MSG_Delete( pMsgData );
  GFL_HEAP_FreeMemory( strBuf1 );

}

//-----------------------------------------------------------------------------
/**
 *	@brief  リスト初期化 リスト項目数を返す
 *
 *	@param	GAMESYS_WORK* gsys
 *	@param	cb_work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static u16 DebugMenuList_GetMax( GAMESYS_WORK* gsys, void* cb_work )
{
  return DEMO_SCENE_MAX;
}

