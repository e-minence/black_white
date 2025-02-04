//==============================================================================
/**
 * @file    save_control_intr.c
 * @brief   イントロ画面用、中断機能付きセーブシステム
 * @author  matsuda
 * @date    2009.12.10(木)
 *
 * イントロ画面の裏で初回セーブを実行しておき、カードアクセスしたい場合に
 * 中断処理をかけることで、カードアクセスできるタイミングを作るシステムです
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control_intr.h"
#include "savedata/player_data.h"


//==============================================================================
//  定数定義
//==============================================================================
typedef enum{
  INTR_SAVE_STATUS_NULL,          ///<何も処理していない
  INTR_SAVE_STATUS_MAIN,          ///<セーブ実行中
  INTR_SAVE_STATUS_SUSPEND,       ///<セーブは中断しています
  INTR_SAVE_STATUS_FINISH,        ///<セーブ完了
}INTR_SAVE_STATUS;


//==============================================================================
//  構造体定義
//==============================================================================
struct _INTR_SAVE_CONTROL{
  SAVE_CONTROL_WORK *ctrl;    ///<セーブシステムへのポインタ
  
  u8 status;                  ///<セーブ状況(INTR_SAVE_STATUS)
  u8 backup_status;           ///<中断時、直前のセーブ状況をバックアップ
  u8 suspend_req;             ///<TRUE:中断リクエスト発生
  u8 no_save;                 ///<TRUE:既にセーブデータがあるのでセーブはしない
  
  BOOL outside_exists;        ///<管理外セーブ存在フラグの退避バッファ
  BOOL outside_break;         ///<管理外セーブ破損フラグの退避バッファ
};


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void _SoftReset_Callback_SaveCancel(void *work);


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * イントロ画面用、中断機能付きセーブシステム作成
 *
 * @param   heap_id		
 * @param   ctrl		
 *
 * @retval  INTR_SAVE_CONTROL *		
 */
//==================================================================
INTR_SAVE_CONTROL * IntrSave_Init(HEAPID heap_id, SAVE_CONTROL_WORK *ctrl)
{
  INTR_SAVE_CONTROL *isc;
  
  isc = GFL_HEAP_AllocClearMemory(heap_id, sizeof(INTR_SAVE_CONTROL));
  isc->ctrl = ctrl;
  if(SaveData_GetExistFlag(ctrl) == TRUE){
    isc->no_save = TRUE;
  }
  else{
    //セーブ時のソフトリセットコールバック設定
    GFL_UI_SoftResetSetFunc(_SoftReset_Callback_SaveCancel, isc);
  }
  
  return isc;
}

//==================================================================
/**
 * イントロ画面用、中断機能付きセーブシステム削除
 *
 * @param   isc		
 */
//==================================================================
void IntrSave_Exit(INTR_SAVE_CONTROL *isc)
{
  //セーブ時のソフトリセットコールバック解除
  GFL_UI_SoftResetSetFunc(NULL, NULL);
  
  if(isc->no_save == FALSE){
    GF_ASSERT(isc->status == INTR_SAVE_STATUS_FINISH);
  }
  GFL_HEAP_FreeMemory(isc);
}

//==================================================================
/**
 * セーブ開始
 *
 * @param   isc		
 */
//==================================================================
void IntrSave_Start(INTR_SAVE_CONTROL *isc)
{
  if(isc->no_save == TRUE){
    return;
  }
  SaveControl_SaveAsyncInit(isc->ctrl);
  if(isc->no_save == FALSE){
    //初回セットアップ状態にする
    SaveData_SetNowSaveModeSetupON(isc->ctrl);
    //管理外セーブに対して何もアクションしないように存在フラグを退避し、無し扱いにしておく
    isc->outside_exists = SaveData_GetOutsideExistFlag(isc->ctrl);
    isc->outside_break = SaveData_GetOutsideBreakFlag(isc->ctrl);
    SaveData_SetOutsideExistFlag(isc->ctrl, FALSE);
    SaveData_SetOutsideBreakFlag(isc->ctrl, FALSE);
  }
  isc->status = INTR_SAVE_STATUS_MAIN;
}

//==================================================================
/**
 * セーブメイン
 *
 * @param   isc		
 *
 * @retval  SAVE_RESULT		
 */
//==================================================================
SAVE_RESULT IntrSave_Main(INTR_SAVE_CONTROL *isc)
{
  SAVE_RESULT result;
  
  if(isc->no_save == TRUE){
    return SAVE_RESULT_OK;
  }
  
  switch(isc->status){
  case INTR_SAVE_STATUS_MAIN:
    if(isc->suspend_req == TRUE && GFL_BACKUP_SAVEASYNC_CheckCardLock() == FALSE){
      isc->backup_status = isc->status;
      isc->status = INTR_SAVE_STATUS_SUSPEND;
      OS_TPrintf("IntSave ... カードUnlock確認 stateサスペンド\n");
      return SAVE_RESULT_CONTINUE;
    }
    
    result = SaveControl_SaveAsyncMain(isc->ctrl);
    if(result == SAVE_RESULT_OK || result == SAVE_RESULT_NG){
      isc->status = INTR_SAVE_STATUS_FINISH;
      OS_TPrintf("IntSave ... セーブ終了\n");
      //セーブが終了したので管理外セーブの存在フラグを復帰
      SaveData_SetOutsideExistFlag(isc->ctrl, isc->outside_exists);
      SaveData_SetOutsideBreakFlag(isc->ctrl, isc->outside_break);
    }
    return result;
  
  case INTR_SAVE_STATUS_SUSPEND:
    if(isc->suspend_req == FALSE){
      isc->status = isc->backup_status;
      OS_TPrintf("セーブは中断しています...\n");
    }
    return SAVE_RESULT_CONTINUE;

  case INTR_SAVE_STATUS_FINISH:
    return SAVE_RESULT_OK;
  }
  
  return SAVE_RESULT_CONTINUE;
}

//==================================================================
/**
 * セーブ中断リクエスト
 *
 * @param   isc		
 *
 * 実際に中断されたかはIntrSave_CheckSuspend関数で監視してください
 */
//==================================================================
void IntrSave_ReqSuspend(INTR_SAVE_CONTROL *isc)
{
  if(isc->status == INTR_SAVE_STATUS_FINISH){
    return;
  }
  
  GF_ASSERT(isc->suspend_req == FALSE);
  isc->suspend_req = TRUE;
}

//==================================================================
/**
 * 中断していたセーブを再開します
 *
 * @param   isc		
 */
//==================================================================
void IntrSave_Resume(INTR_SAVE_CONTROL *isc)
{
  if(isc->status == INTR_SAVE_STATUS_FINISH){
    return;
  }
  
  GF_ASSERT(isc->suspend_req == TRUE);
  isc->suspend_req = FALSE;
}

//==================================================================
/**
 * セーブを中断しているか調べる
 *
 * @param   isc		
 *
 * @retval  BOOL		TRUE:中断している(カードアクセス可能)
 * @retval  BOOL		FALSE:中断していない(カードアクセス禁止)
 */
//==================================================================
BOOL IntrSave_CheckSuspend(INTR_SAVE_CONTROL *isc)
{
  if(isc->no_save == TRUE 
      || isc->status == INTR_SAVE_STATUS_FINISH){
    //外部シーケンスの進行停止の原因になるのでセーブが終了の時はTRUEを返す
    return TRUE;
  }
  
  if(isc->status == INTR_SAVE_STATUS_SUSPEND){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 第1段階のセーブが完了後、MYSTATUSに性別、名前をセットした後、
 * この関数を呼び出してください
 * MYSTATUSセット完了後の2度目のセーブを行います
 *
 * @param   isc		
 */
//==================================================================
void IntrSave_ReqMyStatusSave(INTR_SAVE_CONTROL *isc)
{
//  isc->mystatus_save_req = TRUE;
}

//==================================================================
/**
 * 全てのセーブが完了しているか調べる
 *
 * @param   isc		
 *
 * @retval  BOOL		TRUE:全てのセーブが完了している
 * @retval  BOOL		FALSE:まだセーブが残っている
 */
//==================================================================
BOOL IntrSave_CheckAllSaveEnd(INTR_SAVE_CONTROL *isc)
{
  if(isc->no_save == TRUE){
    return TRUE;
  }
  if(isc->status == INTR_SAVE_STATUS_FINISH){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ソフトウェアリセット直前時に呼ばれるコールバック関数
 *
 * @param   work		
 */
//--------------------------------------------------------------
static void _SoftReset_Callback_SaveCancel(void *work)
{
  INTR_SAVE_CONTROL *isc = work;
  
  OS_TPrintf("intr_save softreset callback\n");
  if(isc->no_save == FALSE && isc->status != INTR_SAVE_STATUS_FINISH){
    SaveControl_SaveAsyncCancel(isc->ctrl);
    isc->no_save = TRUE;  //一応、セーブ処理の方にいかないようにno_save扱いにしておく
  }
}
