//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  フィールドマップ上で動作するタスク
 * @file   field_task.c
 * @author obata
 * @date   2009.11.13
 */
//////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "field_task.h"


//========================================================================================
// ■定数
//========================================================================================
//#define DEBUG_PRINT_ENABLE

//========================================================================================
// ■タスク
//========================================================================================
struct _FIELD_TASK
{
  FIELD_TASK_STATE state;  // 状態
  void*      work;   // ワーク

  FIELD_TASK_MAIN_FUNC* mainFunc;  // 処理関数
};


//========================================================================================
// ■非公開関数のプロトタイプ宣言
//========================================================================================
static void BootTask( FIELD_TASK* task );
static void StopTask( FIELD_TASK* task );
static void RunTask_EXECUTE( FIELD_TASK* task );


//========================================================================================
// ■作成・破棄
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief タスクを作成する
 *
 * @param heap_id   使用するヒープID
 * @param work_size ワークサイズ
 * @param main_func タスク処理関数
 *
 * @return 作成したタスク
 */
//----------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_Create( HEAPID heap_id, int work_size, 
                               FIELD_TASK_MAIN_FUNC* main_func )
{
  FIELD_TASK* task;

  // タスクを生成
  task           = GFL_HEAP_AllocMemory( heap_id, sizeof(FIELD_TASK) );
  task->state    = FIELD_TASK_STATE_WAIT;
  task->work     = GFL_HEAP_AllocClearMemory( heap_id, work_size ); 
  task->mainFunc = main_func;
  return task;
} 

//----------------------------------------------------------------------------------------
/**
 * @brief タスクを破棄する
 *
 * @param task 破棄するタスク
 */
//----------------------------------------------------------------------------------------
void FIELD_TASK_Delete( FIELD_TASK* task )
{
  GFL_HEAP_FreeMemory( task->work ); // ワークを破棄
  GFL_HEAP_FreeMemory( task );       // 本体を破棄
}


//========================================================================================
// ■動作
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief タスクを動かす
 *
 * @param task 動かすタスク
 */
//----------------------------------------------------------------------------------------
void FIELD_TASK_Main( FIELD_TASK* task )
{
  RunTask_EXECUTE( task );
}


//========================================================================================
// ■制御
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief タスクを起動する
 *
 * @param task 起動させるタスク
 */
//----------------------------------------------------------------------------------------
void FIELD_TASK_Boot( FIELD_TASK* task )
{
  BootTask( task );
}


//========================================================================================
// ■取得
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief タスクワークを取得する
 *
 * @param task ワークを取得するタスク
 *
 * @return 指定したタスクのワーク
 */
//----------------------------------------------------------------------------------------
extern void* FIELD_TASK_GetWork( const FIELD_TASK* task )
{
  return task->work;
}

//----------------------------------------------------------------------------------------
/**
 * @brief タスクが終了したかどうかを調べる
 *
 * @param task 調べるタスク
 * 
 * @return 終了していたら TRUE, そうでなければ FALSE
 */
//----------------------------------------------------------------------------------------
BOOL FIELD_TASK_IsEnd( const FIELD_TASK* task )
{
  return (task->state == FIELD_TASK_STATE_END);
}

//----------------------------------------------------------------------------------------
/**
 * @brief タスクの状態を取得する
 *
 * @param task 取得対象のタスク
 *
 * @return 指定タスクの状態
 */ 
//----------------------------------------------------------------------------------------
FIELD_TASK_STATE FIELD_TASK_GetState( const FIELD_TASK* task )
{
  return task->state;
}


//========================================================================================
// ■非公開関数
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief タスクを起動させる
 *
 * @param task 起動させるタスク
 */
//----------------------------------------------------------------------------------------
static void BootTask( FIELD_TASK* task )
{
  task->state = FIELD_TASK_STATE_EXECUTE;

#ifdef DEBUG_PRINT_ENABLE
  OBATA_Printf( "FIELD_TASK: boot\n" );
#endif
}

//----------------------------------------------------------------------------------------
/**
 * @brief タスクを停止させる
 *
 * @param task 起動させるタスク
 */
//----------------------------------------------------------------------------------------
static void StopTask( FIELD_TASK* task )
{
  task->state = FIELD_TASK_STATE_END;

#ifdef DEBUG_PRINT_ENABLE
  OBATA_Printf( "FIELD_TASK: stop\n" );
#endif
}

//----------------------------------------------------------------------------------------
/**
 * @brief 実行中のタスクを動かす
 *
 * @param task 動かすタスク
 */
//----------------------------------------------------------------------------------------
static void RunTask_EXECUTE( FIELD_TASK* task )
{
  BOOL retval;

  // 実行中じゃない
  if( task->state != FIELD_TASK_STATE_EXECUTE ) return;

  // 実行
  retval = task->mainFunc( task->work );

  // 停止
  if( retval == FIELD_TASK_RETVAL_FINISH )
  {
    StopTask( task );
  }
}
