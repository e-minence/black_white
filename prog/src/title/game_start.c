//==============================================================================
/**
 * @file	game_start.c
 * @brief	「さいしょから」or「つづきから」を選択後の処理
 * @author	matsuda
 * @date	2009.01.07(水)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include "system\main.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "title/game_start.h"
#include "title/title.h"
#include "savedata/situation.h"
#include "app/name_input.h"
#include "test/testmode.h"
#include "select_moji_mode.h"
#include "message.naix"
#include "msg/msg_debugname.h"

//==============================================================================
//	
//==============================================================================
FS_EXTERN_OVERLAY(title);

typedef enum
{
  CONTINUE_MODE_SELECT,
  CONTINUE_MODE_COMM_OFF,
  CONTINUE_MODE_COMM_ON,
}CONTINUE_MODE_TYPE;

//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static GFL_PROC_RESULT GameStart_FirstProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_FirstProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT GameStart_FirstProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_ContinueProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_ContinueProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_ContinueProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugSelectNameProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugSelectNameProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugSelectNameProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//==============================================================================
//	データ
//==============================================================================
///最初から始める
static const GFL_PROC_DATA GameStart_FirstProcData = {
	GameStart_FirstProcInit,
	GameStart_FirstProcMain,
	GameStart_FirstProcEnd,
};

///続きから始める
static const GFL_PROC_DATA GameStart_ContinueProcData = {
	GameStart_ContinueProcInit,
	GameStart_ContinueProcMain,
	GameStart_ContinueProcEnd,
};

///デバッグ開始
static const GFL_PROC_DATA GameStart_DebugProcData = {
	GameStart_DebugProcInit,
	GameStart_DebugProcMain,
	GameStart_DebugProcEnd,
};

///じんめいせんたく開始
static const GFL_PROC_DATA GameStart_DebugSelectNameProcData = {
	GameStart_DebugSelectNameProcInit,
	GameStart_DebugSelectNameProcMain,
	GameStart_DebugSelectNameProcEnd,
};


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   「最初から始める」を選択
 */
//--------------------------------------------------------------
void GameStart_Beginning(void)
{
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_FirstProcData, NULL);
}

//--------------------------------------------------------------
/**
 * @brief   「続きから始める」を選択
 */
//--------------------------------------------------------------
void GameStart_Continue(void)
{
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_ContinueProcData, (void*)CONTINUE_MODE_SELECT);
}

//--------------------------------------------------------------
/**
 * @brief   「続きから始める(通信なし)」を選択
 */
//--------------------------------------------------------------
void GameStart_ContinueNetOff(void)
{
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_ContinueProcData, (void*)CONTINUE_MODE_COMM_OFF);
}

//--------------------------------------------------------------
/**
 * @brief   「続きから始める(常時通信)」を選択
 */
//--------------------------------------------------------------
void GameStart_ContinueNet(void)
{
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_ContinueProcData, (void*)CONTINUE_MODE_COMM_ON);
}

//--------------------------------------------------------------
/**
 * @brief   「デバッグ開始」を選択
 */
//--------------------------------------------------------------
void GameStart_Debug(void)
{
#ifdef PM_DEBUG
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_DebugProcData, NULL);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   「デバッグ開始(常時通信)」を選択
 */
//--------------------------------------------------------------
void GameStart_DebugNet(void)
{
#ifdef PM_DEBUG
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_DebugProcData, (void*)TRUE);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   testmenuの「じんめいせんたく」を選択
 */
//--------------------------------------------------------------
void GameStart_Debug_SelectName(void)
{
#ifdef PM_DEBUG
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_DebugSelectNameProcData, NULL);
#endif
}



//==============================================================================
//
//	最初から始める
//
//==============================================================================
typedef struct
{
  SELECT_MODE_INIT_WORK selModeParam;
	NAMEIN_PARAM *nameInParam;
}GAMESTART_FIRST_WORK;
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_FirstProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESTART_FIRST_WORK *work = GFL_PROC_AllocWork( proc , sizeof(GAMESTART_FIRST_WORK) , GFL_HEAPID_APP );
	SaveControl_ClearData(SaveControl_GetPointer());	//セーブデータクリア
	//FIXME 正しい値に
	work->nameInParam = NameIn_ParamAllocMake( GFL_HEAPID_APP , NAMEIN_MYNAME , 0 , PERSON_NAME_SIZE , NULL );
	work->selModeParam.type = SMT_START_GAME;
	work->selModeParam.configSave = SaveData_GetConfig( SaveControl_GetPointer() );
	work->selModeParam.mystatus = SaveData_GetMyStatus( SaveControl_GetPointer() );
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_FirstProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESTART_FIRST_WORK *work = mywk;
	switch(*seq){
	case 0:
	  //漢字選択
		GFL_PROC_SysCallProc(NO_OVERLAY_ID, &SelectModeProcData,&work->selModeParam);
		(*seq)++;
		break;
	case 1:
	  //名前入力
		GFL_PROC_SysCallProc(NO_OVERLAY_ID, &NameInputProcData,(void*)work->nameInParam);
		(*seq)++;
		break;
	case 2:
		return GFL_PROC_RES_FINISH;
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_FirstProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESTART_FIRST_WORK *work = mywk;
	GAME_INIT_WORK * init_param;
	VecFx32 pos = {0,0,0};
	MYSTATUS		*myStatus;
	
	//名前のセット
	myStatus = SaveData_GetMyStatus( SaveControl_GetPointer() );
	MyStatus_SetMyNameFromString( myStatus , work->nameInParam->strbuf );
	init_param = DEBUG_GetGameInitWork(GAMEINIT_MODE_FIRST, 0, &pos, 0, work->selModeParam.isComm);

	NameIn_ParamDelete(work->nameInParam);
	GFL_PROC_FreeWork( proc );
	
	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);

	return GFL_PROC_RES_FINISH;
}



//==============================================================================
//
//	続きから始める
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_ContinueProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESTART_FIRST_WORK *work = GFL_PROC_AllocWork( proc , sizeof(GAMESTART_FIRST_WORK) , GFL_HEAPID_APP );
	work->selModeParam.type = SMT_CONTINUE_GAME;
	work->selModeParam.configSave = NULL;
	work->selModeParam.mystatus = NULL;
	
	
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_ContinueProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESTART_FIRST_WORK *work = mywk;
	CONTINUE_MODE_TYPE continueType = (CONTINUE_MODE_TYPE)pwk;
	switch(*seq){
	case 0:
	  //通信選択
  	if( continueType == CONTINUE_MODE_SELECT )
  	{
  		GFL_PROC_SysCallProc(NO_OVERLAY_ID, &SelectModeProcData,&work->selModeParam);
  	}
  	else
  	if( continueType == CONTINUE_MODE_COMM_OFF )
  	{
      work->selModeParam.isComm = FALSE;
    }
    else
  	if( continueType == CONTINUE_MODE_COMM_ON )
  	{
      work->selModeParam.isComm = TRUE;
    }
		(*seq)++;
		break;
	case 1:
		return GFL_PROC_RES_FINISH;
		break;
	}
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_ContinueProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESTART_FIRST_WORK *work = mywk;
	GAME_INIT_WORK * init_param;
	PLAYERWORK_SAVE plsv;

	SaveControl_Load(SaveControl_GetPointer());
	SaveData_SituationLoad_PlayerWorkSave(SaveControl_GetPointer(), &plsv);

	init_param = DEBUG_GetGameInitWork(
	GAMEINIT_MODE_CONTINUE, plsv.zoneID, &plsv.position, plsv.direction, work->selModeParam.isComm);
	
	GFL_PROC_FreeWork( proc );
	
	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);

	return GFL_PROC_RES_FINISH;
}



//==============================================================================
//
//	デバッグスタート
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
	GAME_INIT_WORK * init_param;
	VecFx32 pos = {0,0,0};
	BOOL always_net;
	
	always_net = (BOOL)pwk;   //TRUE:常時通信で「続きから」

	SaveControl_ClearData(SaveControl_GetPointer());  //セーブデータクリア
  
  {//名前のセット
  	MYSTATUS		*myStatus;
  	GFL_MSGDATA *msgman;
  	STRBUF *namebuf;
  	u32 msg_id;
  	
  	msgman = GFL_MSG_Create( 
  	  GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_debugname_dat, GFL_HEAPID_APP );
  #if PM_VERSION == VERSION_BLACK
  	namebuf = GFL_MSG_CreateString( msgman, DEBUG_NAME_BLACK );
  #else
    namebuf = GFL_MSG_CreateString( msgman, DEBUG_NAME_WHITE );
  #endif
  	
  	myStatus = SaveData_GetMyStatus( SaveControl_GetPointer() );
  	MyStatus_SetMyNameFromString( myStatus , namebuf );
  	
  	GFL_STR_DeleteBuffer(namebuf);
  	GFL_MSG_Delete(msgman);
  }
	
	init_param = DEBUG_GetGameInitWork(GAMEINIT_MODE_DEBUG, 0, &pos, 0, always_net);
	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);
#endif

	return GFL_PROC_RES_FINISH;
}


//==============================================================================
//
//	じんめいせんたくでスタート
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugSelectNameProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
	TESTMODE_PROC_WORK *work;
	work = GFL_PROC_AllocWork( proc, sizeof(TESTMODE_PROC_WORK), GFL_HEAPID_APP );
	work->startMode_ = TESTMODE_NAMESELECT;
	work->work_ = (void*)0;
	SaveControl_ClearData(SaveControl_GetPointer());	//セーブデータクリア
#endif
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugSelectNameProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
	FS_EXTERN_OVERLAY(testmode);

	switch(*seq){
	case 0:
		GFL_PROC_SysCallProc(FS_OVERLAY_ID(testmode), &TestMainProcData, (void*)mywk);
		(*seq)++;
		break;
	case 1:
		return GFL_PROC_RES_FINISH;
	}
#endif
	return GFL_PROC_RES_CONTINUE;

}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugSelectNameProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
	TESTMODE_PROC_WORK *work = mywk;
	if( (int)work->work_ == 0 )
	{
		GAME_INIT_WORK * init_param;
		VecFx32 pos = {0,0,0};
		
		init_param = DEBUG_GetGameInitWork(GAMEINIT_MODE_FIRST, 0, &pos, 0, FALSE);
		GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);
	}
	else
	{
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(testmode), &TestMainProcData, NULL );
	}
	GFL_PROC_FreeWork(proc);
#endif
	return GFL_PROC_RES_FINISH;
}
