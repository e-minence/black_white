//======================================================================
/**
 * @file	field_proc.c
 * @author	tamada
 * @date	2008.11.18
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"	//HEAPID参照のため

#include "gamesystem/gamesystem.h"
#include "field/fieldmap_proc.h"
#include "field/fieldmap_call.h"  //FIELDMAP_Create/FIELDMAP_Main/FIELDMAP_Delete

#include "field/zonedata.h" //ZONEDATA_IsUnionRoom
#include "system/net_err.h"
#include "field_data.h"   //FIELDDATA_GetFieldHeapSize

//======================================================================
//	define
//======================================================================
///HEAPID_FIELD_SUBSCREENのサイズ
#define HEAP_FIELD_SUBSCREEN_SIZE   (0xd000)

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FIELDPROC_WORK
//--------------------------------------------------------------
typedef struct {
	FIELDMAP_WORK *fieldWork;
}FIELDPROC_WORK;

//======================================================================
//	proto
//======================================================================
const GFL_PROC_DATA FieldProcData;

//======================================================================
//	
//======================================================================
#ifdef PM_DEBUG
extern BOOL DebugScanOnly;
#endif


//======================================================================
//	フィールドプロセス
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	プロセスの初期化
 * ここでヒープの生成や各種初期化処理を行う。
 * 初期段階ではmywkはNULLだが、GFL_PROC_AllocWorkを使用すると
 * 以降は確保したワークのアドレスとなる。
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FieldMapProcInit
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	FIELDMAP_WORK * fieldWork;
	FIELDPROC_WORK * fpwk;
	GAMESYS_WORK * gsys = pwk;
  u32 heap_size;

  switch(*seq){
  case 0:
    {
      u16 zone_id = PLAYERWORK_getZoneID( GAMESYSTEM_GetMyPlayerWork(gsys) );
      heap_size = FIELDDATA_GetFieldHeapSize( zone_id );
    }
    heap_size -= HEAP_FIELD_SUBSCREEN_SIZE;
  	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_FIELDMAP, heap_size );
  	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_FIELD_SUBSCREEN, HEAP_FIELD_SUBSCREEN_SIZE );
  	fpwk = GFL_PROC_AllocWork(proc, sizeof(FIELDPROC_WORK), HEAPID_FIELDMAP);
  	fpwk->fieldWork = FIELDMAP_Create(gsys, HEAPID_FIELDMAP );
  	GAMESYSTEM_SetFieldMapWork(gsys, fpwk->fieldWork);
  	(*seq)++;
  	break;
  
  case 1:
    //常時通信モード
    {
      if(GAMESYSTEM_GetAlwaysNetFlag(gsys) == FALSE){
        //常時通信はOFFモード
        GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
        GAME_COMM_NO comm_no = GameCommSys_BootCheck(game_comm);
        switch(comm_no){
        case GAME_COMM_NO_FIELD_BEACON_SEARCH:
        case GAME_COMM_NO_INVASION:
          OS_TPrintf("非通信モードの為、通信終了\n");
          GameCommSys_ExitReq(game_comm);
          break;
        }
      }
    }
  	return GFL_PROC_RES_FINISH;
  }
  
 	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	プロセスのメイン
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FieldMapProcMain
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESYS_WORK * gsys = pwk;
	FIELDPROC_WORK * fpwk = mywk;
	if( FIELDMAP_Main(gsys, fpwk->fieldWork) == TRUE ){
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	プロセスの終了処理
 *
 * 単に終了した場合、親プロセスに処理が返る。
 * GFL_PROC_SysSetNextProcを呼んでおくと、終了後そのプロセスに
 * 処理が遷移する。
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FieldMapProcEnd
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESYS_WORK * gsys = pwk;
	FIELDPROC_WORK * fpwk = mywk;
  
	FIELDMAP_Delete(fpwk->fieldWork);
	GAMESYSTEM_SetFieldMapWork(gsys, NULL);
	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap( HEAPID_FIELD_SUBSCREEN );
	GFL_HEAP_DeleteHeap( HEAPID_FIELDMAP );

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
///	フィールドプロセス　データ
//------------------------------------------------------------------
const GFL_PROC_DATA FieldProcData = {
	FieldMapProcInit,
	FieldMapProcMain,
	FieldMapProcEnd,
};
