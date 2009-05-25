//======================================================================
/**
 * @file	fieldmap.c
 * @brief	フィールドマップ処理
 * @author
 *
 * @todo  1/30で動作しているがキーは1/60で拾っているので取れないタイミングがある
 * @todo  外部からのフィールドマップ終了リクエストが同上のタイミングを考慮していない
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"					//GFL_HEAPID_APP
#include "system/wipe.h"
#include "arc_def.h"

#include "net/network_define.h"
#include "system/g3d_tool.h"

#include "print/gf_font.h"

#include "fieldmap.h"
#include "field_common.h"
#include "field_player.h"
#include "field_camera.h"
#include "field_data.h"
#include "field/field_msgbg.h"
#include "field_encount.h"

#include "weather.h"
#include "field_fog.h"
#include "field_light.h"
#include "field_buildmodel.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "field/zonedata.h"
#include "field/areadata.h"

#include "event_mapchange.h"
#include "event_debug_menu.h"
#include "event_fieldmap_menu.h"
#include "event_battle.h"
#include "event_ircbattle.h"
#include "event_fieldtalk.h"

#include "field_comm_actor.h"
#include "field_comm/field_comm_main.h"
#include "field_comm/field_comm_event.h"

#include "fldmmdl.h"

#include "field_debug.h"

#include "map_matrix.h"

#include "field/eventdata_sxy.h"

#include "arc/fieldmap/zone_id.h"
#include "field/weather_no.h"
#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "message.naix" //NARC_message_d_field_dat
#include "msg/msg_d_field.h"  //DEBUG_FIELD_STR00 DEBUG_FIELD_C_STR10

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	GAMEMODE
//--------------------------------------------------------------
typedef enum{
	GAMEMODE_BOOT = 0,
	GAMEMODE_NORMAL,
	GAMEMODE_FINISH,
}GAMEMODE;

//--------------------------------------------------------------
///	FLDMAPSEQ
//--------------------------------------------------------------
typedef enum{
	FLDMAPSEQ_SETUP_SYSTEM = 0,
	FLDMAPSEQ_SETUP,
	FLDMAPSEQ_READY,
	FLDMAPSEQ_UPDATE,
	FLDMAPSEQ_FREE,
	FLDMAPSEQ_END,
	
	FLDMAPSEQ_MAX,
}FLDMAPSEQ;

//--------------------------------------------------------------
///	定数
//--------------------------------------------------------------
enum { 
  ///3D面の描画プライオリティー
  FIELD_3D_FRAME_PRI    =   1,

  ///テクスチャ使用サイズ指定
  FIELD_3D_VRAM_SIZE    =   GFL_G3D_TEX256K,

  ///パレット使用サイズ指定
  FIELD_3D_PLTT_SIZE    =   GFL_G3D_PLT64K,

  ///DTCMエリアのサイズ
  FIELD_3D_DTCM_SIZE    =   0x1000, 

///billboardActで使用するリソースの最大設定可能数
  FIELD_G3D_BBDACT_RESMAX	    =   64,
///billboardActで使用するオブジェクトの最大設定可能数
  FIELD_G3D_BBDACT_ACTMAX	    =   256,

  ///<セルアクターリソース最大
  FIELD_CLSYS_RESOUCE_MAX		  =   64,

  ///フォグ色
  FIELD_DEFAULT_FOG_COLOR		  =   (GX_RGB(31,31,31)),
  ///背景色
  FIELD_DEFAULT_BG_COLOR	    =   (GX_RGB(30,31,31)),
};

///g3Dutilで使用するリソースの最大設定可能数
#define G3D_UTIL_RESCOUNT	(512)
///g3Dutilで使用するオブジェクトの最大設定可能数
#define G3D_UTIL_OBJCOUNT	(128)

#define MMDL_LIST_MAX	(64) ///<動作モデルリスト最大

//--------------------------------------------------------------
/**
 * @brief フィールドマップシーケンス動作関数の戻り値
 */
//--------------------------------------------------------------
typedef enum {  
  MAINSEQ_RESULT_CONTINUE,      ///<動作継続中
  MAINSEQ_RESULT_NEXTSEQ,       ///<次のシーケンスへ切り替え
  MAINSEQ_RESULT_FINISH,        ///<終了

  MAINSEQ_RESULT_MAX
}MAINSEQ_RESULT;

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
/// フィールドマップシーケンス動作関数の型定義
//--------------------------------------------------------------
typedef MAINSEQ_RESULT (*FIELDMAP_MAIN_FUNC)(GAMESYS_WORK*,FIELDMAP_WORK*);

//--------------------------------------------------------------
///	FIELDMAP_WORK
//--------------------------------------------------------------
struct _FIELDMAP_WORK
{
	HEAPID heapID;
	GAMESYS_WORK *gsys;
	
	FIELD_CAMERA *camera_control;
	FIELD_LIGHT *light;
	FIELD_FOG_WORK *fog;
	
	FIELD_WEATHER *weather_sys;
  FIELD_SUBSCREEN_WORK* fieldSubscreenWork;
	
	FIELD_COMM_MAIN *commSys;
	
	FLDMSGBG *fldMsgBG;
	
	FLDMMDLSYS *fldMMdlSys;
	
	FLDMAPPER *g3Dmapper;
	MAP_MATRIX *pMapMatrix;
	FLDMAPPER_RESISTDATA map_res;
	FIELD_PLAYER *field_player;
	FIELD_ENCOUNT *encount;
	
  GFL_G3D_CAMERA *g3Dcamera; //g3Dcamera Lib ハンドル
	GFL_G3D_LIGHTSET *g3Dlightset; //g3Dlight Lib ハンドル
	GFL_TCB *g3dVintr; //3D用vIntrTaskハンドル
	GFL_BBDACT_SYS *bbdActSys; //ビルボードアクトシステム設定ハンドル
	
	GAMEMODE gamemode;
	FLDMAPSEQ seq;
  u32 seq_switch;
	int timer;
//	int	key_trg;
//  int key_trg_tail;
//	int key_cont;
	u16 map_id;
	VecFx32 now_pos;
	LOCATION location;
	
	const DEPEND_FUNCTIONS *func_tbl;
	void *mapCtrlWork;
	
	FIELD_DEBUG_WORK *debugWork;
};

//--------------------------------------------------------------
///	MMDL_LIST
//--------------------------------------------------------------
typedef struct
{
	int count;
	u16 id_list[MMDL_LIST_MAX];
}MMDL_LIST;

//======================================================================
//	proto
//======================================================================
//フィールドメインシーケンス関数群
static MAINSEQ_RESULT mainSeqFunc_setup_system(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_setup(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_ready(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_update(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_free(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_end(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

//bg
static void	fldmap_BG_Init( FIELDMAP_WORK *fieldWork );
static void	fldmap_BG_Exit( FIELDMAP_WORK *fieldWork );

//g3d
static void fldmap_G3D_CallBackSetUp( void );
static void fldmap_G3D_Load( FIELDMAP_WORK * fieldWork );
static void fldmap_G3D_Control( FIELDMAP_WORK * fieldWork );
static void fldmap_G3D_Draw( FIELDMAP_WORK * fieldWork );
static void fldmap_G3D_Unload( FIELDMAP_WORK * fieldWork );
static void	fldmap_G3D_VBlank( GFL_TCB* tcb, void* work );
static void	fldmap_G3D_BBDTrans(
		GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz );

//fldmmdl
static void fldmapMain_FLDMMDL_Init( FIELDMAP_WORK *fieldWork );
static void fldmapMain_FLDMMDL_Finish( FIELDMAP_WORK *fieldWork );
static void fldmap_FLDMMDL_InitList(
		MMDL_LIST *mlist, int list_id, HEAPID heapID );

//event
static GMEVENT * fldmap_Event_CheckConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos );
static GMEVENT * fldmap_Event_CheckPushConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * fldmapFunc_Event_CheckEvent(GAMESYS_WORK *gsys, void *work);

//zonechange
static BOOL fldmapMain_UpdateMoveZone( FIELDMAP_WORK *fieldWork );
static BOOL fldmap_CheckPlayerPosUpdate( FIELDMAP_WORK *fieldWork );
static BOOL fldmap_CheckMoveZoneChange( FIELDMAP_WORK *fieldWork );
static void fldmap_ZoneChange( FIELDMAP_WORK *fieldWork );

static void zoneChange_SetMMdl(
		FLDMMDLSYS *fmmdlsys, EVENTDATA_SYSTEM *evdata, u32 zone_id );
static void zoneChange_SetBGM( GAMEDATA *gdata, u32 zone_id );
static void zoneChange_SetWeather( FIELDMAP_WORK *fieldWork, u32 zone_id );
static void zoneChange_UpdatePlayerWork( GAMEDATA *gdata, u32 zone_id );

//etc
static void fldmap_ClearMapCtrlWork( FIELDMAP_WORK *fieldWork );

//data
static const GFL_DISP_VRAM fldmapdata_dispVram;
static const GFL_BG_SYS_HEADER fldmapdata_bgsysHeader;
static const GFL_CLSYS_INIT fldmapdata_CLSYS_Init;
static const VecFx32 fldmapdata_cameraTarget;
static const VecFx32 fldmapdata_cameraPos;
static const GFL_G3D_LIGHTSET_SETUP fldmapdata_light0Setup;
static const GXRgb fldmapdata_edgeColorTable[8];
static const u32 fldmapdata_fogColorTable[8];

static const u16 fldmapdata_bgColorTable[16];

//======================================================================
//	フィールドマップ　生成　削除
//======================================================================
//--------------------------------------------------------------
/**
 * フィールドマップ　生成
 * @param	gsys	GAMESYS_WORK
 * @param	heapID HEAPID
 * @retval	FIELDMAP_WORK
 */
//--------------------------------------------------------------
FIELDMAP_WORK * FIELDMAP_Create( GAMESYS_WORK *gsys, HEAPID heapID )
{
	FIELDMAP_WORK *fieldWork;
  COMM_FIELD_SYS_PTR comm_field;
	
	fieldWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELDMAP_WORK) );
	fieldWork->heapID = heapID;
	fieldWork->gamemode = GAMEMODE_BOOT;
	fieldWork->gsys = gsys;
	
	{
		PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork(gsys);
		fieldWork->map_id = PLAYERWORK_getZoneID( pw );
	}
	
	//マップコントロール
	fieldWork->func_tbl = FIELDDATA_GetFieldFunctions( fieldWork->map_id );

	//マップマトリクス
	fieldWork->pMapMatrix = MAP_MATRIX_Create( heapID );
	
	//通信用処理 
  fieldWork->commSys = FIELD_COMM_MAIN_InitSystem( heapID, GFL_HEAPID_APP, GAMESYSTEM_GetGameCommSysPtr(gsys) );
  FIELD_COMM_MAIN_CommFieldMapInit(FIELD_COMM_MAIN_GetCommFieldSysPtr(fieldWork->commSys));
	FIELD_COMM_MAIN_SetCommActor(fieldWork->commSys,
      GAMEDATA_GetFldMMdlSys(GAMESYSTEM_GetGameData(gsys)));
  
#if 0   //※check　超暫定
  //常時通信モード
  {
    GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);
    if(GAMESYSTEM_GetAlwaysNetFlag(gsys) == TRUE 
        && GameCommSys_BootCheck(gcsp) == GAME_COMM_NO_NULL){
      GameCommSys_Boot(gcsp, GAME_COMM_NO_FIELD_BEACON_SEARCH, gcsp);
    }
  }
#endif
	return fieldWork;
}

//--------------------------------------------------------------
/**
 * フィールドマップ　削除
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FIELDMAP_Delete( FIELDMAP_WORK *fieldWork )
{
	//マップマトリクス
	MAP_MATRIX_Delete( fieldWork->pMapMatrix );
	
	GFL_HEAP_FreeMemory( fieldWork );
}

//======================================================================
//	フィールドマップ　メイン
//======================================================================
//  フィールドメインシーケンス関数群
//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_setup_system(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
	HEAPID heapID = fieldWork->heapID;
	
	//乱数初期化
	GFL_STD_MtRandInit(0);

	//VRAMクリア
	GFL_DISP_ClearVRAM( GX_VRAM_D );
	//VRAM設定
	GFL_DISP_SetBank( &fldmapdata_dispVram );

	//BG初期化
	fldmap_BG_Init( fieldWork );
	
	// CLACT初期化
	GFL_CLACT_SYS_Create(
			&fldmapdata_CLSYS_Init, &fldmapdata_dispVram, heapID );
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

	//BMP初期化
	GFL_BMPWIN_Init( heapID );
	
	//FONT初期化
	GFL_FONTSYS_Init();
	
	//３Ｄシステム起動
	GFL_G3D_Init(
			GFL_G3D_VMANLNK, FIELD_3D_VRAM_SIZE,
      GFL_G3D_VMANLNK, FIELD_3D_PLTT_SIZE,
			FIELD_3D_DTCM_SIZE, fieldWork->heapID, fldmap_G3D_CallBackSetUp );
	DEBUG_GFL_G3D_SetVManSize( FIELD_3D_VRAM_SIZE, FIELD_3D_PLTT_SIZE );

	//３Ｄデータのロード
	fldmap_G3D_Load( fieldWork );
	
	//配置物設定
	fieldWork->g3Dmapper = FLDMAPPER_Create( fieldWork->heapID );

	TAMADA_Printf("TEX:%06x PLT:%04x\n",
			DEBUG_GFL_G3D_GetBlankTextureSize(),
			DEBUG_GFL_G3D_GetBlankPaletteSize());

	//VBlankTCB
	fieldWork->g3dVintr =
		GFUser_VIntr_CreateTCB( fldmap_G3D_VBlank, (void*)fieldWork, 0 );

	GFL_UI_StartFrameRateMode( GFL_UI_FRAMERATE_30 );
	
  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_setup(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
  fieldWork->fldMsgBG = FLDMSGBG_Setup(
      fieldWork->heapID, fieldWork->g3Dcamera );

  fieldWork->camera_control = FIELD_CAMERA_Create(
      fieldWork,
      ZONEDATA_GetCameraID(fieldWork->map_id),
      fieldWork->g3Dcamera,
      &fieldWork->now_pos,
      fieldWork->heapID );

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( fieldWork->g3Dmapper );
    
    FIELDDATA_SetMapperData(fieldWork->map_id,
        GAMEDATA_GetSeasonID(gamedata),
        &fieldWork->map_res,
        fieldWork->pMapMatrix );
    { //とりあえず、電光掲示板用文字列を登録
      FIELD_BMODEL_MAN_EntryELStringID(bmodel_man,
          FIELD_BMODEL_ELBOARD_ID1, NARC_message_d_field_dat, DEBUG_FIELD_C_STR10);
      FIELD_BMODEL_MAN_EntryELStringID(bmodel_man,
          FIELD_BMODEL_ELBOARD_ID2, NARC_message_d_field_dat, DEBUG_FIELD_STR00);
    }
    //とりあえずここで配置モデルリストをセットする
    FIELD_BMODEL_MAN_Load(bmodel_man, fieldWork->map_id);
  }
  
  //フィールドマップ用ロケーション作成
  fieldWork->location.zone_id = fieldWork->map_id;
  fieldWork->location.pos = fieldWork->now_pos;
  
  //マップデータ登録
  FLDMAPPER_ResistData( fieldWork->g3Dmapper, &fieldWork->map_res );
  
  //動作モデル初期化
  fldmapMain_FLDMMDL_Init(fieldWork);
  
  {
    const PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork(gsys);
    const u16 dir = pw->direction;
    const VecFx32 *pos = &pw->position;

    //自機作成
    fieldWork->field_player =
      FIELD_PLAYER_Create( fieldWork, pos, fieldWork->heapID );
    
    //登録テーブルごとに個別の初期化処理を呼び出し
    fieldWork->now_pos = *pos;
    fieldWork->func_tbl->create_func(fieldWork, &fieldWork->now_pos, dir);
    
    FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
    
    TAMADA_Printf("start X,Y,Z=%d,%d,%d\n", FX_Whole(pos->x), FX_Whole(pos->y), FX_Whole(pos->z));
    TAMADA_Printf( "Start Dir = %d\n", pw->direction );
  }
  
  // フォグシステム生成
  fieldWork->fog	= FIELD_FOG_Create( fieldWork->heapID );

  // ライトシステム生成
  {
    GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
    u16 area_id = ZONEDATA_GetAreaID(fieldWork->map_id);
    u16 area_season_id = area_id + ( AREADATA_HasSeason(area_id) ? GAMEDATA_GetSeasonID(gamedata) : 0 );

    fieldWork->light = FIELD_LIGHT_Create( AREADATA_GetLightType( area_season_id ), 
        0, 
        fieldWork->fog, fieldWork->g3Dlightset, fieldWork->heapID );
  }

  // 天気システム生成
  fieldWork->weather_sys = FIELD_WEATHER_Init(
      fieldWork->camera_control,
      fieldWork->light,
      fieldWork->fog,
      fieldWork->heapID );
  
  // 天気晴れ
  FIELD_WEATHER_Set(
      fieldWork->weather_sys, WEATHER_NO_SUNNY, fieldWork->heapID );
  
  //情報バーの初期化
	{
		GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
		fieldWork->fieldSubscreenWork = FIELD_SUBSCREEN_Init(fieldWork->heapID, fieldWork, GAMEDATA_GetSubScreenMode(gamedata));
	}
  
  //フィールドエンカウント初期化
  fieldWork->encount = FIELD_ENCOUNT_Create( fieldWork );
  
  //フィールドデバッグ初期化
  fieldWork->debugWork = FIELD_DEBUG_Init( fieldWork, fieldWork->heapID );

  return MAINSEQ_RESULT_NEXTSEQ;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_ready(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
	fldmap_G3D_Control( fieldWork );
	fldmap_G3D_Draw( fieldWork );
	GFL_CLACT_SYS_Main(); // CLSYSメイン
  
  FLDMSGBG_PrintMain( fieldWork->fldMsgBG );
  
  FIELD_DEBUG_UpdateProc( fieldWork->debugWork );
  
  if( fieldWork->fldMMdlSys != NULL ){
    FLDMMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
  }
  
  if( FLDMAPPER_CheckTrans(fieldWork->g3Dmapper) == FALSE ){
    return MAINSEQ_RESULT_CONTINUE;
  }
  
  //フィールドマップ用イベント起動チェックをセットする
  GAMESYSTEM_EVENT_EntryCheckFunc(
      gsys, fldmapFunc_Event_CheckEvent, fieldWork );
  
  fieldWork->gamemode = GAMEMODE_NORMAL;
  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_update_top(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
	//30フレームに変更
	GFL_UI_ChangeFrameRate(GFL_UI_FRAMERATE_30);
	GFL_UI_ResetFrameRate();
	//ゾーン更新処理
  fldmapMain_UpdateMoveZone( fieldWork );

  //マップ別 登録処理
//  fieldWork->key_trg = 0;
//  fieldWork->key_cont = 0;

  if( GAMESYSTEM_GetEvent(gsys) == NULL) {
//    fieldWork->key_trg = GFL_UI_KEY_GetTrg() | fieldWork->key_trg_tail;
//    fieldWork->key_cont = GFL_UI_KEY_GetCont();
    
    //登録テーブルごとに個別のメイン処理を呼び出し
    fieldWork->func_tbl->main_func( fieldWork, &fieldWork->now_pos );
    
    //Mapシステムに位置を渡している。
    //これがないとマップ移動しないので注意
    FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
  }
  
  //自機更新
  FIELD_PLAYER_Update( fieldWork->field_player );

  //通信用処理(プレイヤーの座標の設定とか
  FIELD_COMM_MAIN_UpdateCommSystem( fieldWork,
      fieldWork->gsys, fieldWork->field_player, fieldWork->commSys );
  
  FIELD_SUBSCREEN_Main(fieldWork->fieldSubscreenWork);
  FIELD_DEBUG_UpdateProc( fieldWork->debugWork );

	fldmap_G3D_Control( fieldWork );
  
  if( fieldWork->fldMMdlSys != NULL ){
    FLDMMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
  }

	//60フレームに変更
	GFL_UI_ChangeFrameRate(GFL_UI_FRAMERATE_60);
  return MAINSEQ_RESULT_CONTINUE;
}

static MAINSEQ_RESULT mainSeqFunc_update_tail(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
	//30フレームに変更
	GFL_UI_ChangeFrameRate(GFL_UI_FRAMERATE_30);
//  fieldWork->key_trg_tail = GFL_UI_KEY_GetTrg();
  
  FIELD_SUBSCREEN_Draw(fieldWork->fieldSubscreenWork);
  FIELD_CAMERA_Main( fieldWork->camera_control, GFL_UI_KEY_GetCont() );
  
  FLDMSGBG_PrintMain( fieldWork->fldMsgBG );
  
	fldmap_G3D_Draw( fieldWork );
	GFL_CLACT_SYS_Main(); // CLSYSメイン
  
	//60フレームに変更
	GFL_UI_ChangeFrameRate(GFL_UI_FRAMERATE_60);
  return MAINSEQ_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_free(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
  //イベント起動チェックを停止する
  GAMESYSTEM_EVENT_EntryCheckFunc(gsys, NULL, NULL);
  
  { //アクターが持つプレイヤー現在位置をPLAYER_WORKに反映する
    VecFx32 player_pos;
    PLAYER_WORK * pw = GAMESYSTEM_GetMyPlayerWork(gsys);
    FIELD_PLAYER_GetPos(fieldWork->field_player, &player_pos);
    PLAYERWORK_setPosition(pw, &player_pos);
  }
  
  //フィールドエンカウント破棄
  FIELD_ENCOUNT_Delete( fieldWork->encount );

  //情報バーの開放
	{
		GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
		GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_Exit(fieldWork->fieldSubscreenWork ));
	}

  // 天気システム破棄
  FIELD_WEATHER_Exit( fieldWork->weather_sys );
  fieldWork->weather_sys = NULL;

  // ライトシステム破棄
  FIELD_LIGHT_Delete( fieldWork->light );
  
  // フォグシステム破棄
  FIELD_FOG_Delete( fieldWork->fog );
  
  FIELD_CAMERA_Delete( fieldWork->camera_control );

	//フィールド通信削除
	FIELD_COMM_MAIN_TermSystem( fieldWork, fieldWork->commSys );
  
  //登録テーブルごとに個別の終了処理を呼び出し
  fieldWork->func_tbl->delete_func(fieldWork);
  fldmap_ClearMapCtrlWork( fieldWork );
  
  fldmapMain_FLDMMDL_Finish(fieldWork);
  
  //自機破棄
  FIELD_PLAYER_Delete( fieldWork->field_player );
  
  FLDMAPPER_ReleaseData( fieldWork->g3Dmapper );
  
  FLDMSGBG_Delete( fieldWork->fldMsgBG );
  
  FIELD_DEBUG_Delete( fieldWork->debugWork );

  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_end(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
	GFL_UI_StartFrameRateMode( GFL_UI_FRAMERATE_60 );

	GFL_TCB_DeleteTask( fieldWork->g3dVintr );

	FLDMAPPER_Delete( fieldWork->g3Dmapper );

	fldmap_G3D_Unload( fieldWork );	//３Ｄデータ破棄
	
	GFL_CLACT_SYS_Delete(); // CLACT破棄
	
	GFL_BMPWIN_Exit();
	
	fldmap_BG_Exit( fieldWork );
  return MAINSEQ_RESULT_FINISH;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * フィールドマップ　メイン
 * @param	gsys	GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval	TRUE    フィールドマップ終了
 * @retval  FALSE   動作継続中
 */
//--------------------------------------------------------------
BOOL FIELDMAP_Main( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  static const FIELDMAP_MAIN_FUNC mainfuncTable[FLDMAPSEQ_MAX][2] = { 
    //FLDMAPSEQ_SETUP_SYSTEM: 基本システムセットアップ
    { 
      mainSeqFunc_setup_system,
      mainSeqFunc_setup_system,
    },
    //FLDMAPSEQ_SETUP: セットアップ
    { 
      mainSeqFunc_setup,
      mainSeqFunc_setup,
    },
    //FLDMAPSEQ_READY: セットアップ完了
    { 
      mainSeqFunc_ready,
      mainSeqFunc_ready,
    },
    //FLDMAPSEQ_UPDATE: フィールドマップ更新
    { 
      mainSeqFunc_update_top,
      mainSeqFunc_update_tail,
    },
    //FLDMAPSEQ_FREE: フィールドマップ開放
    { 
      mainSeqFunc_free,
      mainSeqFunc_free,
    },
    //FLDMAPSEQ_END: 終了
    { 
      mainSeqFunc_end,
      mainSeqFunc_end,
    },
  };

  MAINSEQ_RESULT result;
  FIELDMAP_MAIN_FUNC func;


  GF_ASSERT(fieldWork->seq < NELEMS(mainfuncTable));
	fieldWork->timer++;
	
  result = MAINSEQ_RESULT_CONTINUE;
  func = mainfuncTable[fieldWork->seq][fieldWork->seq_switch];
  if (func != NULL) 
  { 
    result = func(gsys, fieldWork);
  }

  switch (result) 
  { 
  case MAINSEQ_RESULT_CONTINUE:
    fieldWork->seq_switch = !fieldWork->seq_switch;
    break;
  case MAINSEQ_RESULT_NEXTSEQ:
    fieldWork->seq ++;
    fieldWork->seq_switch = 0;
    break;
  case MAINSEQ_RESULT_FINISH:
    return TRUE;
  }
  return FALSE;
}


//======================================================================
//	フィールドマップ　ツール
//======================================================================
//--------------------------------------------------------------
/**
 * フィールドマップ　終了リクエストセット
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELDMAP_Close( FIELDMAP_WORK *fieldWork )
{
	fieldWork->gamemode = GAMEMODE_FINISH;
	fieldWork->seq = FLDMAPSEQ_FREE;
}

//--------------------------------------------------------------
/**
 * フィールドマップ　更新可能状態チェック
 * @param	fieldWork FIELDMAP_WORK
 * @retval BOOL TRUE=更新可能
 */
//--------------------------------------------------------------
const BOOL FIELDMAP_IsReady( const FIELDMAP_WORK *fieldWork )
{
	return (fieldWork->gamemode == GAMEMODE_NORMAL);
}

//--------------------------------------------------------------
/**
 * フィールドマップ　フィールド強制更新
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELDMAP_ForceUpdate( FIELDMAP_WORK *fieldWork )
{
	//キー入力は無いものとする
//	fieldWork->key_trg = 0;
//	fieldWork->key_cont = 0;
	
	//登録テーブルごとに個別のメイン処理を呼び出し
	fieldWork->func_tbl->main_func( fieldWork, &fieldWork->now_pos );
	
	//Mapシステムに位置を渡している。
	//これがないとマップ移動しないので注意
	FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
}

//======================================================================
//	フィールドマップ　参照、設定
//======================================================================
//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_COMM_MAIN取得
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	void*
 */
//--------------------------------------------------------------
void * FIELDMAP_GetCommSys( const FIELDMAP_WORK *fieldWork )
{
	return (void*)fieldWork->commSys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FLDMSGBG取得
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FLDMSGBG*
 */
//--------------------------------------------------------------
FLDMSGBG * FIELDMAP_GetFldMsgBG( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->fldMsgBG;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_CAMERA取得
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_CAMERA*
 */
//--------------------------------------------------------------
FIELD_CAMERA * FIELDMAP_GetFieldCamera( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->camera_control;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_LIGHT取得
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_LIGHT*
 */
//--------------------------------------------------------------
FIELD_LIGHT * FIELDMAP_GetFieldLight( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->light;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_FOG_WORK取得
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_FOG_WORK
 */
//--------------------------------------------------------------
FIELD_FOG_WORK * FIELDMAP_GetFieldFog( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->fog;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_WEATHER*取得
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_WEATHER*
 */
//--------------------------------------------------------------
FIELD_WEATHER * FIELDMAP_GetFieldWeather( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->weather_sys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FLDMMDLSYS取得
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FLDMMDLSYS*
 */
//--------------------------------------------------------------
FLDMMDLSYS * FIELDMAP_GetFldMMdlSys( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->fldMMdlSys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_DEBUG_WORK取得
 * @param	fieldWork FIELDMAP_WORK
 * @retval	FIELD_DEBUG_WORK*
 */
//--------------------------------------------------------------
FIELD_DEBUG_WORK * FIELDMAP_GetDebugWork( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->debugWork;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK GAMESYS_WORK取得
 * @param	fieldWork	FIELDMAP_WORK
 * @retval GAMESYS_WORK*
 */
//--------------------------------------------------------------
GAMESYS_WORK * FIELDMAP_GetGameSysWork( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->gsys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK MAP_MATRIX取得
 * @param fieldWork	FIELDMAP_WORK
 * @retval MAP_MATRIX*
 */
//--------------------------------------------------------------
MAP_MATRIX * FIELDMAP_GetMapMatrix( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->pMapMatrix;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK HEAPID取得
 * @param fieldWork	FIELDMAP_WORK
 * @retval HEAPID
 */
//--------------------------------------------------------------
HEAPID FIELDMAP_GetHeapID( FIELDMAP_WORK *fieldWork )
{
	return( fieldWork->heapID );
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK GFL_BBDACT_SYS取得
 * @param fieldWork FIELDMAP_WORK
 * @retval GFL_BBDACT_SYS
 */
//--------------------------------------------------------------
GFL_BBDACT_SYS * FIELDMAP_GetBbdActSys( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->bbdActSys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FLDMAPPER*取得
 * @param fieldWork FIELDMAP_WORK
 * @retval FLDMAPPER*
 */
//--------------------------------------------------------------
FLDMAPPER * FIELDMAP_GetFieldG3Dmapper( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->g3Dmapper;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK マップコントロール用ワーク取得
 * @param fieldWork FIELDMAP_WORK
 * @retval void* マップコントロール用ワーク
 */
//--------------------------------------------------------------
void * FIELDMAP_GetMapCtrlWork( FIELDMAP_WORK *fieldWork )
{
	GF_ASSERT( fieldWork->mapCtrlWork != NULL ); //未登録参照は禁止
	return fieldWork->mapCtrlWork;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK マップコントロール用ワークセット
 * @param fieldWork FIELDMAP_WORK
 * @param	ctrlWork マップコントロール用ワーク
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELDMAP_SetMapCtrlWork( FIELDMAP_WORK *fieldWork, void *ctrlWork )
{
	GF_ASSERT( fieldWork->mapCtrlWork == NULL ); //二重登録禁止
	fieldWork->mapCtrlWork = ctrlWork;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK キーコンテニュー取得
 * @param fieldWork FIELDMAP_WORK
 * @retval int キーコンティニュー PAD_BUTTON_A等
 */
//--------------------------------------------------------------
int FIELDMAP_GetKeyCont( const FIELDMAP_WORK *fieldWork )
{
	return GFL_UI_KEY_GetCont();//fieldWork->key_cont;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK キートリガ取得
 * @param fieldWork FIELDMAP_WORK
 * @retval int キートリガ PAD_BUTTON_A等
 */
//--------------------------------------------------------------
int FIELDMAP_GetKeyTrg( const FIELDMAP_WORK *fieldWork )
{
	return GFL_UI_KEY_GetTrg();//fieldWork->key_trg;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_PLAYER取得
 * @param fieldWork FIELDMAP_WORK
 * @retval FIELD_PLAYER*
 */
//--------------------------------------------------------------
FIELD_PLAYER * FIELDMAP_GetFieldPlayer( FIELDMAP_WORK *fieldWork )
{
	GF_ASSERT( fieldWork->field_player != NULL ); //未登録参照は禁止
	return fieldWork->field_player;
}

//--------------------------------------------------------------
/**
 * @brief  FIELD_SUBSCREEN_WORKを得る
 * @param fieldWork FIELDMAP_WORK
 * @retval FIELD_SUBSCREEN_WORK*
 */
//--------------------------------------------------------------
FIELD_SUBSCREEN_WORK * FIELDMAP_GetFieldSubscreenWork(
    FIELDMAP_WORK *fieldWork )
{
	return fieldWork->fieldSubscreenWork;
}

//--------------------------------------------------------------
/**
 * @brief  FIELD_SUBSCREEN_WORKを設定する
 * @param  fieldWork FIELDMAP_WORK
 * @retval FIELD_SUBSCREEN_WORK*
 */
//--------------------------------------------------------------
void FIELDMAP_SetFieldSubscreenWork(
    FIELDMAP_WORK *fieldWork, FIELD_SUBSCREEN_WORK *pWork )
{
  fieldWork->fieldSubscreenWork = pWork;
}

//======================================================================
//	フィールドマップ　サブ　BG関連
//======================================================================
//--------------------------------------------------------------
/**
 * BG 初期化
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void	fldmap_BG_Init( FIELDMAP_WORK *fieldWork )
{
	//ＢＧシステム起動
	GFL_BG_Init( fieldWork->heapID );
	
	//背景色パレット転送
  GFL_BG_LoadPalette( GFL_BG_FRAME0_M, (void*)fldmapdata_bgColorTable, 16*2, 0 ); //メイン画面の背景色
  GFL_BG_LoadPalette( GFL_BG_FRAME0_S, (void*)fldmapdata_bgColorTable, 16*2, 0 ); //サブ画面の背景色

	//ＢＧモード設定
	GFL_BG_SetBGMode( &fldmapdata_bgsysHeader );

	//ＢＧコントロール設定
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 8 );
	
	GFL_BG_SetBGControl3D( FIELD_3D_FRAME_PRI );

	//ディスプレイ面の選択
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();
}

//--------------------------------------------------------------
/**
 * BG 削除
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void	fldmap_BG_Exit( FIELDMAP_WORK *fieldWork )
{
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_G3D_Exit();
	GFL_BG_Exit();
}

//======================================================================
//	フィールドマップ　サブ　G3D関連
//======================================================================
//--------------------------------------------------------------
/**
 * G3D セットアップコールバック
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_CallBackSetUp( void )
{
	//各種描画モードの設定(シェード＆アンチエイリアス＆半透明)
	G3X_SetShading( GX_SHADING_TOON );
	G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// アルファテスト　　オフ
	G3X_AlphaBlend( TRUE );

	// フォグセットアップ
  G3X_SetFog(TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x00e00 );
  G3X_SetFogColor(FIELD_DEFAULT_FOG_COLOR, 0);
  G3X_SetFogTable(fldmapdata_fogColorTable);
	
	// クリアカラーの設定
	//color,alpha,depth,polygonID,fog
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);
	
	G3X_SetEdgeColorTable( fldmapdata_edgeColorTable ); 
	G3X_EdgeMarking( FALSE );
	
	// ビューポートの設定
	G3_ViewPort(0, 0, 255, 191);
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_W );
}

//--------------------------------------------------------------
/**
 * G3D ロード
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_Load( FIELDMAP_WORK *fieldWork )
{
	fieldWork->bbdActSys = GFL_BBDACT_CreateSys(
			FIELD_G3D_BBDACT_RESMAX, FIELD_G3D_BBDACT_ACTMAX,
			fldmap_G3D_BBDTrans, fieldWork->heapID );

	//カメラ作成
	fieldWork->g3Dcamera = GFL_G3D_CAMERA_CreateDefault(
			&fldmapdata_cameraPos, &fldmapdata_cameraTarget, fieldWork->heapID );
	
	//ライト作成
	fieldWork->g3Dlightset = GFL_G3D_LIGHT_Create(
			&fldmapdata_light0Setup, fieldWork->heapID );

	//カメラライト0反映
	GFL_G3D_CAMERA_Switching( fieldWork->g3Dcamera );
	GFL_G3D_LIGHT_Switching( fieldWork->g3Dlightset );
	
}

//--------------------------------------------------------------
/**
 * G3D 動作
 * @param fieldWork FIELMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_Control( FIELDMAP_WORK * fieldWork )
{
	FLDMAPPER_Main( fieldWork->g3Dmapper );
	GFL_BBDACT_Main( fieldWork->bbdActSys );
	
	FIELD_WEATHER_Main( fieldWork->weather_sys, fieldWork->heapID );
	FIELD_FOG_Main( fieldWork->fog );
	{
		static int time;
		time += 30;
		time %= 24*3600;
		FIELD_LIGHT_Main( fieldWork->light, time );
	}
}

//--------------------------------------------------------------
/**
 * G3D 描画
 * @param fieldWork FIELMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_Draw( FIELDMAP_WORK * fieldWork )
{
	GFL_G3D_CAMERA_Switching( fieldWork->g3Dcamera );
	GFL_G3D_LIGHT_Switching( fieldWork->g3Dlightset );
  
  FLDMSGBG_PrintG3D( fieldWork->fldMsgBG );
  
	FLDMAPPER_Draw( fieldWork->g3Dmapper, fieldWork->g3Dcamera );
 
	GFL_BBDACT_Draw(
			fieldWork->bbdActSys, fieldWork->g3Dcamera, fieldWork->g3Dlightset );
	
  FIELD_WEATHER_3DWrite( fieldWork->weather_sys );	// 天気描画処理
	
  GFL_G3D_DRAW_End(); //描画終了（バッファスワップ）
}

//--------------------------------------------------------------
/**
 * G3D 破棄
 * @param fieldWork FIELMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_Unload( FIELDMAP_WORK * fieldWork )
{
	GFL_G3D_LIGHT_Delete( fieldWork->g3Dlightset );
	GFL_G3D_CAMERA_Delete( fieldWork->g3Dcamera );
	GFL_BBDACT_DeleteSys( fieldWork->bbdActSys );
}

//--------------------------------------------------------------
/**
 * G3D VBlank処理
 * @param TCB GFL_TCB
 * @param work tcb work
 * @retval nothing
 */
//--------------------------------------------------------------
static void	fldmap_G3D_VBlank( GFL_TCB *tcb, void *work )
{
	FIELDMAP_WORK * fieldWork = (FIELDMAP_WORK*)work;
	GFL_CLACT_SYS_VBlankFunc();	//セルアクターVBlank
}

//--------------------------------------------------------------
/**
 * G3D ビルボードアクター用VRAM転送処理
 * @param type GFL_BBDACT_TRANSTYPE
 * @param dst テクスチャデータ転送先アドレス
 * @param src テクスチャデータソースアドレス
 * @param	siz テクスチャデータサイズ
 * @retval nothing
 */
//--------------------------------------------------------------
static void	fldmap_G3D_BBDTrans(
		GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz )
{
	NNS_GFD_DST_TYPE transType;
	
	if( type == GFL_BBDACT_TRANSTYPE_DATA ){
		transType = NNS_GFD_DST_3D_TEX_VRAM;
	}else{
		transType = NNS_GFD_DST_3D_TEX_PLTT;
	}
	
	NNS_GfdRegisterNewVramTransferTask( transType, dst, (void*)src, siz );
}

//======================================================================
//	フィールドマップ　サブ　動作モデル関連
//======================================================================
//--------------------------------------------------------------
/**
 * 動作モデル　初期化
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmapMain_FLDMMDL_Init( FIELDMAP_WORK *fieldWork )
{
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( fieldWork->gsys );
	FLDMMDLSYS *fmmdlsys = GAMEDATA_GetFldMMdlSys( gdata );
		
	fieldWork->fldMMdlSys = fmmdlsys;
	
	FLDMMDLSYS_SetupProc( fmmdlsys,	//動作モデルシステム　セットアップ
		fieldWork->heapID, fieldWork->g3Dmapper );
		
	FLDMMDL_BLACTCONT_Setup(		//動作モデルビルボード　セットアップ
		fieldWork->fldMMdlSys, fieldWork->bbdActSys, 32 );
	
	{ //ビルボードリソース登録
			int list_area_id = 0;
			MMDL_LIST mlist;
			fldmap_FLDMMDL_InitList( &mlist, list_area_id, fieldWork->heapID );
			FLDMMDL_BLACTCONT_AddOBJCodeRes( fieldWork->fldMMdlSys, HERO );
			FLDMMDL_BLACTCONT_AddResourceTex(
					fieldWork->fldMMdlSys, mlist.id_list, mlist.count );
	}

	//動作モデル描画　セットアップ
	FLDMMDLSYS_SetupDrawProc( fieldWork->fldMMdlSys );
	
	//動作モデル　復帰
	FLDMMDLSYS_Pop( fieldWork->fldMMdlSys );
}

//--------------------------------------------------------------
/**
 * 動作モデル　終了
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmapMain_FLDMMDL_Finish( FIELDMAP_WORK *fieldWork )
{
	FLDMMDLSYS_Push( fieldWork->fldMMdlSys );
	FLDMMDLSYS_DeleteProc( fieldWork->fldMMdlSys );
	fieldWork->fldMMdlSys = NULL;
}

//--------------------------------------------------------------
/**
 * 動作モデル　リスト作成
 * @param mlist MMDL_LIST
 * @param list_id 作成するリストID
 * @param	heapID テンポラリ用HEAPID
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_FLDMMDL_InitList(
		MMDL_LIST *mlist, int list_id, HEAPID heapID )
{
	int i = 0;
	u16 *pList;
	pList = GFL_ARC_LoadDataAlloc( ARCID_FLDMMDL_LIST, list_id, heapID );
	mlist->count = 0;
	
	while( pList[i] != OBJCODEMAX ){
		mlist->id_list[i] = pList[i];
		i++;
		GF_ASSERT( i < MMDL_LIST_MAX );
	}
	
	OS_Printf( "モデルリスト総数 %d\n", i );
	
	mlist->count = i;
	mlist->id_list[i] = OBJCODEMAX;
	GFL_HEAP_FreeMemory( pList );
}

//======================================================================
//	フィールドマップ　サブ　イベント
//======================================================================
//--------------------------------------------------------------
/**
 * イベント マップ接続チェック
 * @param gsys GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param now_pos チェックする座標
 * @retval GMEVENT NULL=接続なし
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_CheckConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos )
{
	const CONNECT_DATA * cnct;
	GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
	int idx = EVENTDATA_SearchConnectIDByPos(evdata, now_pos);
	
	if( idx == EXIT_ID_NONE ){
		return NULL;
	}

	//マップ遷移発生の場合、出入口を記憶しておく
	{
		LOCATION ent_loc;
		LOCATION_Set(
				&ent_loc, fieldWork->map_id, idx, 0,
				now_pos->x, now_pos->y, now_pos->z);
		GAMEDATA_SetEntranceLocation(gamedata, &ent_loc);
	}

	cnct = EVENTDATA_GetConnectByID(evdata, idx);
	
	if (CONNECTDATA_IsSpecialExit(cnct)) {
		//特殊接続先が指定されている場合、記憶しておいた場所に飛ぶ
		const LOCATION * sp = GAMEDATA_GetSpecialLocation(gamedata);
		return EVENT_ChangeMap(gsys, fieldWork, sp);
	}else{
		LOCATION loc_req;
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
	}
}

//--------------------------------------------------------------
/**
 * イベント キー入力イベント起動チェック
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL イベント無し
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_CheckPushConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
	VecFx32 now_pos = fieldWork->now_pos;
	
	switch( FIELDMAP_GetKeyCont(fieldWork) ) {
	case PAD_KEY_UP:		now_pos.z -= FX32_ONE * 16; break;
	case PAD_KEY_DOWN:	now_pos.z += FX32_ONE * 16; break;
	case PAD_KEY_LEFT:	now_pos.x -= FX32_ONE * 16; break;
	case PAD_KEY_RIGHT:	now_pos.x += FX32_ONE * 16; break;
	default: return NULL;
	}
	
	{	
		u32 attr = 0;
		FLDMAPPER_GRIDINFO gridInfo;
		FLDMAPPER *g3Dmapper = fieldWork->g3Dmapper;
		
		if( FLDMAPPER_GetGridInfo(g3Dmapper,&now_pos,&gridInfo) == TRUE ){
			attr = gridInfo.gridData[0].attr;
		}

		if( attr == 0 ){
			return NULL;
		}
	}
	
	return fldmap_Event_CheckConnect(gsys, fieldWork, &now_pos);
}

//--------------------------------------------------------------
/**
 * サブスクリーンからのイベント起動チェック
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL イベント無し
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_Check_SubScreen(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GMEVENT* event=NULL;
  
  switch(FIELD_SUBSCREEN_GetAction(FIELDMAP_GetFieldSubscreenWork(fieldWork))){
  case FIELD_SUBSCREEN_ACTION_DEBUGIRC:
    event = EVENT_IrcBattle(gsys, fieldWork, NULL, TRUE);
    FIELD_SUBSCREEN_ResetAction(FIELDMAP_GetFieldSubscreenWork(fieldWork));
    break;

#if PM_DEBUG
  case FIELD_SUBSCREEN_ACTION_DEBUG_PALACEJUMP:
		event = DEBUG_PalaceJamp(fieldWork , gsys, fieldWork->field_player);
    FIELD_SUBSCREEN_ResetAction(FIELDMAP_GetFieldSubscreenWork(fieldWork));
		break;
#endif


	}
  return event;
}

//--------------------------------------------------------------
/**
 * イベント　イベント起動チェック
 * @param	gsys GAMESYS_WORK
 * @param work FIELDMAP_WORK
 * @retval GMEVENT NULL=イベント無し
 */
//--------------------------------------------------------------
static GMEVENT * fldmapFunc_Event_CheckEvent( GAMESYS_WORK *gsys, void *work )
{
	enum{
		resetCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START,
		chgCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_SELECT
	};
	
	GMEVENT *event;
	FIELDMAP_WORK *fieldWork = work;
	int	trg = FIELDMAP_GetKeyTrg( fieldWork );
	int cont = FIELDMAP_GetKeyCont( fieldWork );
	PLAYER_MOVE_STATE state;
  PLAYER_MOVE_VALUE value;

  { //自機動作ステータス更新
    FIELD_PLAYER_UpdateMoveStatus( fieldWork->field_player );
  }
  
  value = FIELD_PLAYER_GetMoveValue( fieldWork->field_player );
  state = FIELD_PLAYER_GetMoveState( fieldWork->field_player );
  
#ifdef  PM_DEBUG
	//ソフトリセットチェック
	if( (cont&resetCont) == resetCont ){
		return DEBUG_EVENT_GameEnd(gsys, fieldWork);
	}
	
	//マップ変更チェック
	if( (cont&chgCont) == chgCont ){
		return DEBUG_EVENT_ChangeToNextMap(gsys, fieldWork);
	}
	
	//デバッグメニュー起動チェック
	//if( trg == PAD_BUTTON_SELECT ){
  if( (trg & PAD_BUTTON_X) && (cont & PAD_BUTTON_R) )
  {
		return DEBUG_EVENT_DebugMenu(gsys, fieldWork, 
				fieldWork->heapID, ZONEDATA_GetMapRscID(fieldWork->map_id));
	}
#endif //debug
	
  //戦闘移行チェック
  if( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_GRID ){
    #ifdef PM_DEBUG
    if( (cont & PAD_BUTTON_R) == 0 ){
      if( state == PLAYER_MOVE_STATE_END ){
        if( FIELD_ENCOUNT_CheckEncount(fieldWork->encount) == TRUE ){
          return EVENT_Battle( gsys, fieldWork );
        }
      }
    }
    #else
    if( state == PLAYER_MOVE_STATE_END ){
      if( FIELD_ENCOUNT_CheckEncount(fieldWork->encount) == TRUE ){
        return EVENT_Battle( gsys, fieldWork );
      }
    }
    #endif
  }
  
	//座標接続チェック
	event = fldmap_Event_CheckConnect(gsys, fieldWork, &fieldWork->now_pos);
	if( event != NULL ){
		return event;
	}
	
	//キー入力接続チェック
	event = fldmap_Event_CheckPushConnect(gsys, fieldWork);
	if( event != NULL ){
		return event;
	}
	
	//メニュー起動チェック
	if( trg == PAD_BUTTON_X ){
		if(WIPE_SYS_EndCheck()){
  			return EVENT_FieldMapMenu( gsys, fieldWork, fieldWork->heapID );
		}
	}

	//サブスクリーンからのイベント起動チェック
	event = fldmap_Event_Check_SubScreen(gsys, fieldWork);
	if( event != NULL )
  {
		return event;
	}
  
	///通信用会話処理(仮
	//話しかける側
	if( trg == PAD_BUTTON_A ){
		if( FIELD_COMM_MAIN_CanTalk( fieldWork->commSys ) == TRUE ){
			return FIELD_COMM_EVENT_StartTalk(
					gsys , fieldWork , fieldWork->commSys );
		}
	}

	//話しかけられる側(中で一緒に話せる状態かのチェックもしてしまう
	if( FIELD_COMM_MAIN_CheckReserveTalk( fieldWork->commSys ) == TRUE ){
		return FIELD_COMM_EVENT_StartTalkPartner(
				gsys , fieldWork , fieldWork->commSys );
	}
	
	//フィールド話し掛けチェック
	if(	fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_GRID )
	{
		if( trg == PAD_BUTTON_A )
		{
			int gx,gy,gz;
			FLDMMDL *fmmdl_talk;
			FIELD_PLAYER_GetFrontGridPos( fieldWork->field_player, &gx, &gy, &gz );
			fmmdl_talk = FLDMMDLSYS_SearchGridPos(
					fieldWork->fldMMdlSys, gx, gz, FALSE );
			
			if( fmmdl_talk != NULL )
			{
				u32 scr_id = FLDMMDL_GetEventID( fmmdl_talk );
				FLDMMDL *fmmdl_player = FIELD_PLAYER_GetFldMMdl(
						fieldWork->field_player );
				return EVENT_FieldTalk( gsys, fieldWork,
					scr_id, fmmdl_player, fmmdl_talk, fieldWork->heapID );
			}
		}
	}
	
	//デバッグ：パレスで木に触れたらワープ
  {
    GMEVENT *ev;
    ev =  DEBUG_PalaceTreeMapWarp(fieldWork, gsys, fieldWork->field_player);
    if(ev != NULL){
      return ev;
    }
	}
	return NULL;
}

//======================================================================
//	フィールドマップ　サブ　ゾーン更新
//======================================================================
//--------------------------------------------------------------
/**
 * 自機移動によるゾーン更新
 * @param fieldWork FIELDMAP_WORK
 * @retval BOOL TRUE=ゾーン更新発生
 */
//--------------------------------------------------------------
static BOOL fldmapMain_UpdateMoveZone( FIELDMAP_WORK *fieldWork )
{
	if( fldmap_CheckPlayerPosUpdate(fieldWork) == TRUE ){
		if( fldmap_CheckMoveZoneChange(fieldWork) == TRUE ){
			fldmap_ZoneChange( fieldWork );
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * 自機座標更新チェック
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	BOOL	TRUE=座標更新
 */
//--------------------------------------------------------------
static BOOL fldmap_CheckPlayerPosUpdate( FIELDMAP_WORK *fieldWork )
{
	LOCATION *lc = &fieldWork->location;
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( fieldWork->gsys );
	PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
	const VecFx32 *pos = PLAYERWORK_getPosition( player );
	
	if( pos->x != lc->pos.x || pos->z != lc->pos.z ){
		lc->pos = *pos;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替えチェック 
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	BOOL TRUE=ゾーン切り替え発生
 */
//--------------------------------------------------------------
static BOOL fldmap_CheckMoveZoneChange( FIELDMAP_WORK *fieldWork )
{
	LOCATION *lc = &fieldWork->location;
	MAP_MATRIX *mat = fieldWork->pMapMatrix;
	
	if( MAP_MATRIX_CheckVectorPosRange(mat,lc->pos.x,lc->pos.z) == TRUE ){
		u32 zone_id =
			MAP_MATRIX_GetVectorPosZoneID( mat, lc->pos.x, lc->pos.z );
		
		//現状 IDが無い場合は更新しない
		if( zone_id != MAP_MATRIX_ZONE_ID_NON ){
			if( zone_id != (u32)lc->zone_id ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え時の処理
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fldmap_ZoneChange( FIELDMAP_WORK *fieldWork )
{
	LOCATION *lc = &fieldWork->location;
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( fieldWork->gsys );
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
	FLDMMDLSYS *fmmdlsys = fieldWork->fldMMdlSys;
	
	MAP_MATRIX *mat = fieldWork->pMapMatrix;
	u32 new_zone_id = MAP_MATRIX_GetVectorPosZoneID(
			mat, lc->pos.x, lc->pos.z );
	u32 old_zone_id = lc->zone_id;
	
	GF_ASSERT( new_zone_id != MAP_MATRIX_ZONE_ID_NON );
	
	//旧ゾーン配置動作モデル削除
	FLDMMDLSYS_DeleteZoneUpdateFldMMdl( fmmdlsys );
	
	//次のイベントデータをロード
	EVENTDATA_SYS_Load( evdata, new_zone_id );
	
	//新規ゾーンに配置する動作モデルセット
	zoneChange_SetMMdl( fmmdlsys, evdata, new_zone_id );
	
	//BGM切り替え
	zoneChange_SetBGM( gdata, new_zone_id );
	
	//天候リクエスト
	zoneChange_SetWeather( fieldWork, new_zone_id );
	
	//PLAYER_WORK更新
	zoneChange_UpdatePlayerWork( gdata, new_zone_id );
	
	//ゾーンID更新
	lc->zone_id = new_zone_id;
	
	KAGAYA_Printf( "ゾーン更新完了 %d -> %d\n", lc->zone_id, new_zone_id );
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え時の処理　新規ゾーン動作モデルセット
 * @param	fmmdlsys	FLDMMDLSYS
 * @param	evdata	EVENTDATA_SYSTEM
 * @param	zone_id	次のゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_SetMMdl(
		FLDMMDLSYS *fmmdlsys, EVENTDATA_SYSTEM *evdata, u32 zone_id )
{
	u16 count = EVENTDATA_GetNpcCount( evdata );
	
	if( count ){
		const FLDMMDL_HEADER *header = EVENTDATA_GetNpcData( evdata );
		FLDMMDLSYS_SetFldMMdl( fmmdlsys, header, zone_id, count );
	}
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え時の処理　BGM切り替え
 * @param	gdata	GAMEDATA
 * @param	zone_id	次のゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_SetBGM( GAMEDATA *gdata, u32 zone_id )
{
	u16 trackBit = 0xfcff;	// track 9,10 OFF
	#if 0
	u16 nextBGM = ZONEDATA_GetBGMID(
			new_zone_id, GAMEDATA_GetSeasonID(gdata) );
	#else
	u16 nextBGM = 0;
	switch( zone_id ){
	case ZONE_ID_T01:
		nextBGM = SEQ_WB_T_01; break;
	case ZONE_ID_C01:
		nextBGM = SEQ_WB_TITLE; break;
	case ZONE_ID_R01:
		nextBGM = SEQ_WB_R_A_SP; break;
	case ZONE_ID_D01:
		nextBGM = SEQ_WB_SHINKA; break;
		break;
	default:
		break;
	}
	#endif
	
	if( nextBGM != 0 ){
		if( PMSND_GetBGMsoundNo() != nextBGM ){
			PMSND_PlayNextBGM_EX( nextBGM, trackBit );
		}
	}
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え時の処理　天候リクエスト
 * @param	fieldWork	FIELDMAP_WORK*
 * @param	zone_id	次のゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_SetWeather( FIELDMAP_WORK *fieldWork, u32 zone_id )
{
	u32 w_no = WEATHER_NO_NUM;
	FIELD_WEATHER *we = FIELDMAP_GetFieldWeather( fieldWork );
	
	switch( zone_id ){
	case ZONE_ID_T01:
		w_no = WEATHER_NO_SNOW;
		break;
	case ZONE_ID_R01:
		w_no = WEATHER_NO_RAIN;
		break;
	}
	
	if( w_no != WEATHER_NO_NUM && w_no != FIELD_WEATHER_GetWeatherNo(we) ){
		FIELD_WEATHER_Change( we, w_no );
	}
}

//--------------------------------------------------------------
/**
 * ゾーン切り替え時の処理　PLAYER_WORK更新
 * @param	gdata	GAMEDATA
 * @param	zone_id	次のゾーンID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_UpdatePlayerWork( GAMEDATA *gdata, u32 zone_id )
{
	PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
	PLAYERWORK_setZoneID( player, zone_id );
}

//======================================================================
//	フィールドマップ　その他
//======================================================================
//--------------------------------------------------------------
/**
 * FIELDMAP_WORK マップコントロール用ワーククリア
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_ClearMapCtrlWork( FIELDMAP_WORK *fieldWork )
{
	fieldWork->mapCtrlWork = NULL;
}

//======================================================================
//	data
//======================================================================
//--------------------------------------------------------------
/// ディスプレイ環境データ
//--------------------------------------------------------------
static const GFL_DISP_VRAM fldmapdata_dispVram =
{
	GX_VRAM_BG_128_C, //メイン2DエンジンのBGに割り当て 
	GX_VRAM_BGEXTPLTT_NONE, //メイン2DエンジンのBG拡張パレットに割り当て
	GX_VRAM_SUB_BG_32_H, //サブ2DエンジンのBGに割り当て
	GX_VRAM_SUB_BGEXTPLTT_NONE, //サブ2DエンジンのBG拡張パレットに割り当て
	GX_VRAM_OBJ_64_E, //メイン2DエンジンのOBJに割り当て
	GX_VRAM_OBJEXTPLTT_NONE, //メイン2DエンジンのOBJ拡張パレットにに割り当て
	GX_VRAM_SUB_OBJ_16_I, //サブ2DエンジンのOBJに割り当て
	GX_VRAM_SUB_OBJEXTPLTT_NONE, //サブ2DエンジンのOBJ拡張パレットにに割り当て
	GX_VRAM_TEX_01_AB, //テクスチャイメージスロットに割り当て
	GX_VRAM_TEXPLTT_0_G, //テクスチャパレットスロットに割り当て
	GX_OBJVRAMMODE_CHAR_1D_64K, // メインOBJマッピングモード
	GX_OBJVRAMMODE_CHAR_1D_32K, // サブOBJマッピングモード
};

//--------------------------------------------------------------
///	BGヘッダー
//--------------------------------------------------------------
static const GFL_BG_SYS_HEADER fldmapdata_bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};

//--------------------------------------------------------------
///	セルアクター　初期化データ
//--------------------------------------------------------------
static const GFL_CLSYS_INIT fldmapdata_CLSYS_Init =
{
	0, 0,
	0, 512,
	GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL,
	GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL, //通信アイコン部分
	0,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
  16, 16,
};

//--------------------------------------------------------------
///	カメラ初期設定データ
//--------------------------------------------------------------
static const VecFx32 fldmapdata_cameraTarget	= { 0, 0, 0 };

static const VecFx32 fldmapdata_cameraPos	= {
	0, (FX32_ONE * 64), (FX32_ONE * 128)
};

//--------------------------------------------------------------
///	ライト初期設定データ
//--------------------------------------------------------------
static const GFL_G3D_LIGHT_DATA light0Tbl[] =
{
	{ 0, {{-(FX16_ONE-1),-(FX16_ONE-1),-(FX16_ONE-1)},GX_RGB(31,31,31)} },
	{ 1, {{(FX16_ONE-1),-(FX16_ONE-1),-(FX16_ONE-1)},GX_RGB(31,31,31)} },
	{ 2, {{-(FX16_ONE-1),-(FX16_ONE-1),-(FX16_ONE-1)},GX_RGB(31,31,31)} },
	{ 3, {{-(FX16_ONE-1),-(FX16_ONE-1),-(FX16_ONE-1)},GX_RGB(31,31,31)} },
};

static const GFL_G3D_LIGHTSET_SETUP fldmapdata_light0Setup = {
	light0Tbl, NELEMS(light0Tbl)
};

//--------------------------------------------------------------
///	3D エッジカラー
//--------------------------------------------------------------
static const GXRgb fldmapdata_edgeColorTable[8] = {
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
};

//--------------------------------------------------------------
//  3D  フォグカラー
//--------------------------------------------------------------
#define FOG_VALUE(v) (u32)(((v*16)<<0) | ((v*16+4)<<8) | ((v*16+8)<<16) | ((v*16+12)<<24))
static const u32 fldmapdata_fogColorTable[8] = {  
  FOG_VALUE(0),
  FOG_VALUE(1),
  FOG_VALUE(2),
  FOG_VALUE(3),
  FOG_VALUE(4),
  FOG_VALUE(5),
  FOG_VALUE(6),
  FOG_VALUE(7),
};
//--------------------------------------------------------------
//  BG  背景カラー
//--------------------------------------------------------------
static const u16 fldmapdata_bgColorTable[16] = { 
  FIELD_DEFAULT_BG_COLOR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

