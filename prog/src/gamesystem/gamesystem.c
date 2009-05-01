//============================================================================================
/**
 * @file	gamemsystem.c
 * @brief	ポケモンゲームシステム（ここが通常ゲームのトップレベル）
 * @author	tamada GAME FREAK Inc.
 * @date	08.10.20
 *
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"		//HEAPIDの参照用

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"

#include "gamesystem/game_init.h"

#include "src/field/event_mapchange.h"

//============================================================================================
//============================================================================================
enum {
	HEAPID_GAMESYS = HEAPID_PROC,
	
	HEAPSIZE_GAMESYS = 0x4000,
	//x200000
};

//------------------------------------------------------------------
/**
 * @brief	ゲーム制御システム用ワーク保持変数
 *
 * ※たぶんデバッグ用か限定時のグローバル参照用にしか使わないはず
 */
//------------------------------------------------------------------
static GAMESYS_WORK * GameSysWork;

//------------------------------------------------------------------
/**
 * @brief	ゲーム初期化パラメータ保持変数
 *
 */
//------------------------------------------------------------------
static GAME_INIT_WORK TestGameInitWork;

//============================================================================================
//============================================================================================
extern const GFL_PROC_DATA FieldProcData;
FS_EXTERN_OVERLAY(fieldmap);

static void GameSystem_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param);
static BOOL GameSystem_Main(GAMESYS_WORK * gsys);
static void GameSystem_End(GAMESYS_WORK * gsys);
static u32 GAMESYS_WORK_GetSize(void);
static void DEBUG_MyPokeAdd(GAMESYS_WORK * gsys);


//============================================================================================
/**
 * @brief	ゲームシステム用プロセス
 *
 * NitroMain->GFLUser_Main->GFL_PROC_Main()に登録される。
 * 基本的にはゲーム本編開始後はこのプロセスが動作し続けるはず
 */
//============================================================================================
static GFL_PROC_RESULT GameMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT GameMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT GameMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk);

//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT GameMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GAMESYS_WORK * gsys;
	u32 work_size = GAMESYS_WORK_GetSize();
	GAME_INIT_WORK *game_init = pwk;
	
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GAMESYS, HEAPSIZE_GAMESYS );
	gsys = GFL_PROC_AllocWork(proc, work_size, HEAPID_GAMESYS);
	GameSysWork = gsys;
	GameSystem_Init(gsys, HEAPID_GAMESYS, pwk);
#if 1		/* 暫定的にプロセス登録 */
	switch(game_init->mode){
	case GAMEINIT_MODE_CONTINUE:
		{
			GMEVENT * event = DEBUG_EVENT_SetFirstMapIn(gsys, pwk);
			GAMESYSTEM_SetEvent(gsys, event);
		}
		break;
	case GAMEINIT_MODE_FIRST:
	case GAMEINIT_MODE_DEBUG:
		{
			GMEVENT * event = DEBUG_EVENT_SetFirstMapIn(gsys, pwk);
			GAMESYSTEM_SetEvent(gsys, event);
			//適当に手持ちポケモンをAdd
			DEBUG_MyPokeAdd(gsys);
		}
		break;
	}
#endif
	return GFL_PROC_RES_FINISH;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT GameMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GAMESYS_WORK * gsys = mywk;
	if (GameSystem_Main(gsys)) {
		return GFL_PROC_RES_FINISH;
	} else {
		return GFL_PROC_RES_CONTINUE;
	}
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT GameMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GAMESYS_WORK * gsys = mywk;
	GameSystem_End(gsys);
	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap( HEAPID_GAMESYS );
	return GFL_PROC_RES_FINISH;
}
//------------------------------------------------------------------
/**
 * @brief		プロセス関数テーブル
 */
//------------------------------------------------------------------
const GFL_PROC_DATA GameMainProcData = {
	GameMainProcInit,
	GameMainProcMain,
	GameMainProcEnd,
};


//------------------------------------------------------------------
/**
 * @file	ゲーム開始初期化用ワーク取得
 */
//------------------------------------------------------------------
GAME_INIT_WORK * DEBUG_GetGameInitWork(GAMEINIT_MODE mode, u16 mapid, VecFx32 *pos, s16 dir)
{
	TestGameInitWork.mode = mode;
	TestGameInitWork.mapid = mapid;
	TestGameInitWork.pos = *pos;
	TestGameInitWork.dir = dir;
	return &TestGameInitWork;
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	ゲーム制御システム用ワーク定義
 */
//------------------------------------------------------------------
struct _GAMESYS_WORK {
	HEAPID heapID;			///<使用するヒープ指定ID
	void * parent_work;		///<親（呼び出し元）から引き継いだワークへのポインタ

	GFL_PROCSYS * procsys;	///<使用しているPROCシステムへのポインタ

	BOOL proc_result;

	EVCHECK_FUNC evcheck_func;
	void * evcheck_context;
	GMEVENT * event;

	GAMEDATA * gamedata;
	void * fieldmap;
	void * comm_field;			///<フィールド通信ワーク
	void * comm_infowin;		///<INFOWIN通信ワーク
};

//------------------------------------------------------------------
//------------------------------------------------------------------
static u32 GAMESYS_WORK_GetSize(void)
{
	return sizeof(GAMESYS_WORK);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GAMESYS_WORK_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param)
{
	int i;
	gsys->heapID = heapID;
	gsys->parent_work = init_param;
	gsys->procsys = GFL_PROC_LOCAL_boot(gsys->heapID);
	gsys->proc_result = FALSE;
	gsys->evcheck_func = NULL;
	gsys->evcheck_context = NULL;
	gsys->event = NULL;

	gsys->gamedata = GAMEDATA_Create(gsys->heapID);
	gsys->fieldmap = NULL;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GAMESYS_WORK_Delete(GAMESYS_WORK * gsys)
{
	GAMEDATA_Delete(gsys->gamedata);
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GameSystem_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param)
{
	GAMESYS_WORK_Init(gsys, heapID, init_param);
}

//------------------------------------------------------------------
/**
 * @return BOOL	TRUEのとき、ゲーム終了
 */
//------------------------------------------------------------------
static BOOL GameSystem_Main(GAMESYS_WORK * gsys)
{
	//Game Server Proccess
	//PlayerController/Event Trigger
	//イベント起動チェック処理（シチュエーションにより分岐）
	GAMESYSTEM_EVENT_CheckSet(gsys, gsys->evcheck_func, gsys->evcheck_context);
	//イベント実行処理
	GAMESYSTEM_EVENT_Main(gsys);
	gsys->proc_result = GFL_PROC_LOCAL_Main(gsys->procsys);
	if (gsys->proc_result == FALSE && gsys->event == NULL) {
		//プロセスもイベントも存在しないとき、ゲーム終了
		return TRUE;
	} else {
		return FALSE;
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void GameSystem_End(GAMESYS_WORK * gsys)
{
	GFL_PROC_LOCAL_Exit(gsys->procsys);
	GAMESYS_WORK_Delete(gsys);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_EVENT_EntryCheckFunc(GAMESYS_WORK * gsys, EVCHECK_FUNC evcheck_func, void * context)
{
	gsys->evcheck_func = evcheck_func;
	gsys->evcheck_context = context;
}

//============================================================================================
//
//		外部インターフェイス関数
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_SetNextProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk)
{
	GFL_PROC_LOCAL_SetNextProc(gsys->procsys, ov_id, procdata, pwk);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_CallProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk)
{
	GFL_PROC_LOCAL_CallProc(gsys->procsys, ov_id, procdata, pwk);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_CallFieldProc(GAMESYS_WORK * gsys)
{
	GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(fieldmap), &FieldProcData, gsys);
}

//------------------------------------------------------------------
//	プロセス存在チェック
//------------------------------------------------------------------
BOOL GAMESYSTEM_IsProcExists(const GAMESYS_WORK * gsys)
{
	return gsys->proc_result;
}
//------------------------------------------------------------------
//	ゲームデータ取得
//------------------------------------------------------------------
GAMEDATA * GAMESYSTEM_GetGameData(GAMESYS_WORK * gsys)
{
	return gsys->gamedata;
}

//------------------------------------------------------------------
//	自分プレイヤーワーク取得
//------------------------------------------------------------------
PLAYER_WORK * GAMESYSTEM_GetMyPlayerWork(GAMESYS_WORK * gsys)
{
	return GAMEDATA_GetMyPlayerWork(gsys->gamedata);
}
//------------------------------------------------------------------
//	ヒープID取得
//------------------------------------------------------------------
HEAPID GAMESYSTEM_GetHeapID(GAMESYS_WORK * gsys)
{
	return gsys->heapID;
}

//------------------------------------------------------------------
//	イベント取得
//------------------------------------------------------------------
GMEVENT * GAMESYSTEM_GetEvent(GAMESYS_WORK * gsys)
{
	return gsys->event;
}

//------------------------------------------------------------------
//	イベント設定
//------------------------------------------------------------------
void GAMESYSTEM_SetEvent(GAMESYS_WORK * gsys, GMEVENT * event)
{
	gsys->event = event;
}

//------------------------------------------------------------------
//	フィールドマップワークへのポインタ取得
//------------------------------------------------------------------
void * GAMESYSTEM_GetFieldMapWork(GAMESYS_WORK * gsys)
{
	GF_ASSERT(gsys->fieldmap != NULL);
	return gsys->fieldmap;
}
//------------------------------------------------------------------
//	フィールドマップワークのポインタセット
//------------------------------------------------------------------
void GAMESYSTEM_SetFieldMapWork(GAMESYS_WORK * gsys, void * fieldmap)
{
	gsys->fieldmap = fieldmap;
}

//--------------------------------------------------------------
/**
 * @brief   フィールド通信ワークポインタ取得
 * @param   gsys		ゲーム制御システムへのポインタ
 * @retval  フィールド通信ワークポインタ(確保されていない場合はNULL)
 */
//--------------------------------------------------------------
void * GAMESYSTEM_GetCommFieldWork(GAMESYS_WORK * gsys)
{
	return gsys->comm_field;
}

//--------------------------------------------------------------
/**
 * @brief   フィールド通信ワークポインタセット
 * @param   gsys		ゲーム制御システムへのポインタ
 * @param   comm_field		フィールド通信ワークポインタ
 */
//--------------------------------------------------------------
void GAMESYSTEM_SetCommFieldWork(GAMESYS_WORK * gsys, void * comm_field)
{
	GF_ASSERT(gsys->comm_field == NULL);
	gsys->comm_field = comm_field;
}


//--------------------------------------------------------------
/**
 * @brief   デバッグ用に適当に手持ちポケモンをAdd
 * @param   gsys		
 */
//--------------------------------------------------------------
static void DEBUG_MyPokeAdd(GAMESYS_WORK * gsys)
{
	POKEPARTY *party;
	POKEMON_PARAM *pp;
	
	party = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(gsys));

	pp = PP_Create(150, 50, 123456, GFL_HEAPID_APP);
	
	PokeParty_Add(party, pp);
	PP_Setup(pp, 250, 100, 123456);
	PokeParty_Add(party, pp);
	PP_Setup(pp, 350, 100, 123456);
	PokeParty_Add(party, pp);
	PP_Setup(pp, 400, 100, 123456);
	PokeParty_Add(party, pp);
	
	GFL_HEAP_FreeMemory(pp);
}
