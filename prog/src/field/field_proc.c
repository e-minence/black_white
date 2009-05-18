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

//======================================================================
//	define
//======================================================================

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

  switch(*seq){
  case 0:
  	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_FIELDMAP, 0x140000 );
  	fpwk = GFL_PROC_AllocWork(proc, sizeof(FIELDPROC_WORK), HEAPID_FIELDMAP);
  	fpwk->fieldWork = FIELDMAP_Create(gsys, HEAPID_FIELDMAP );
  	GAMESYSTEM_SetFieldMapWork(gsys, fpwk->fieldWork);
  	(*seq)++;
  	return GFL_PROC_RES_CONTINUE;
  
  default:
    (*seq)++;
  	return GFL_PROC_RES_CONTINUE;
  case 10:   //※check　超暫定
    //常時通信モード
    {
      GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);
      if(GAMESYSTEM_GetAlwaysNetFlag(gsys) == TRUE 
          && GameCommSys_BootCheck(gcsp) == GAME_COMM_NO_NULL){
        GameCommSys_Boot(gcsp, GAME_COMM_NO_FIELD_BEACON_SEARCH, gcsp);
      }
    }
    break;
  }
  
	return GFL_PROC_RES_FINISH;
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
