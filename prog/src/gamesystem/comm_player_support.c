//==============================================================================
/**
 * @file    comm_player_support.c
 * @brief   通信プレイヤーからのサポート
 * @author  matsuda
 * @date    2009.12.10(木)
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/comm_player_support.h"
#include "savedata/mystatus_local.h"


//==============================================================================
//  構造体定義
//==============================================================================
///通信相手からのサポート
struct _COMM_PLAYER_SUPPORT{
  SUPPORT_TYPE type;            ///<サポートの種類
  SUPPORT_TYPE used_type;       ///<実際に戦闘で使用されたサポートの種類
  SUPPORT_TYPE battle_end_type; ///<戦闘終了後サポートの種類状況

  MYSTATUS mystatus;            ///<サポートしてくれた通信相手のMYSTATUS
  MYSTATUS used_mystatus;       ///<実際に戦闘画面で反映された相手のMYSTATUS
};


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * サポートデータAlloc
 *
 * @param   cps		
 */
//==================================================================
COMM_PLAYER_SUPPORT * COMM_PLAYER_SUPPORT_Alloc(HEAPID heap_id)
{
  COMM_PLAYER_SUPPORT *cps;
  
  cps = GFL_HEAP_AllocClearMemory(heap_id, sizeof(COMM_PLAYER_SUPPORT));
  COMM_PLAYER_SUPPORT_Init(cps);
  return cps;
}

//==================================================================
/**
 * サポートデータFreeMemory
 *
 * @param   cps		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_Free(COMM_PLAYER_SUPPORT *cps)
{
  GFL_HEAP_FreeMemory(cps);
}

//==================================================================
/**
 * サポートデータ初期化
 *
 * @param   cps		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_Init(COMM_PLAYER_SUPPORT *cps)
{
  GFL_STD_MemClear(cps, sizeof(COMM_PLAYER_SUPPORT));
  cps->type = SUPPORT_TYPE_NULL;
  cps->used_type = SUPPORT_TYPE_NULL;
  cps->battle_end_type = SUPPORT_TYPE_NULL;
  MyStatus_Init(&cps->mystatus);
  MyStatus_Init(&cps->used_mystatus);
}

//==================================================================
/**
 * サポートパラメータをセットする
 *
 * @param   dest_cps		
 * @param   type		    
 * @param   mystatus		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_SetParam(COMM_PLAYER_SUPPORT *dest_cps, SUPPORT_TYPE type, const MYSTATUS *mystatus)
{
  dest_cps->type = type;
  dest_cps->mystatus = *mystatus;
}

//==================================================================
/**
 * サポートデータからMYSTATUSを取得
 *
 * @param   cps		
 *
 * @retval  const MYSTATUS *		
 */
//==================================================================
const MYSTATUS * COMM_PLAYER_SUPPORT_GetMyStatus(const COMM_PLAYER_SUPPORT *cps)
{
  return &cps->mystatus;
}

//==================================================================
/**
 * サポートデータからサポートタイプを取得
 *
 * @param   cps		
 *
 * @retval  SUPPORT_TYPE		
 */
//==================================================================
SUPPORT_TYPE COMM_PLAYER_SUPPORT_GetSupportType(const COMM_PLAYER_SUPPORT *cps)
{
  return cps->type;
}

//==================================================================
/**
 * サポートデータを使用済み状態にする
 *
 * @param   cps		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_SetUsed(COMM_PLAYER_SUPPORT *cps)
{
  cps->used_type = cps->type;
  cps->used_mystatus = cps->mystatus;

  cps->type = SUPPORT_TYPE_USED;
}

//==================================================================
/**
 * 戦闘後の状況の反映
 *
 * @param   cps		
 */
//==================================================================
void COMM_PLAYER_SUPPORT_SetBattleEnd(COMM_PLAYER_SUPPORT *cps)
{
  cps->battle_end_type = cps->type;
  cps->type = SUPPORT_TYPE_NULL;
}

//==================================================================
/**
 * 戦闘後、サポートが発生していたかチェック
 *
 * @param   cps		
 *
 * @retval  BOOL		TRUE:サポートが発生していた
 */
//==================================================================
BOOL COMM_PLAYER_SUPPORT_GetBattleEndSupportType(const COMM_PLAYER_SUPPORT *cps)
{
  if(cps->used_type != SUPPORT_TYPE_NULL || cps->battle_end_type != SUPPORT_TYPE_NULL){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 戦闘後に使用するツール：実際に助けてくれた人のMYSTATUSを取得する
 *
 * @param   cps		
 *
 * @retval  const MYSTATUS *		
 *
 * 戦闘画面で反映されたサポートがいるならその人優先
 * 反映されたサポートがないなら最後に受信している人のデータを返す
 */
//==================================================================
const MYSTATUS * COMM_PLAYER_SUPPORT_GetSupportedMyStatus(const COMM_PLAYER_SUPPORT *cps)
{
  GF_ASSERT(COMM_PLAYER_SUPPORT_GetBattleEndSupportType(cps) == TRUE);
  
  if(cps->used_type != SUPPORT_TYPE_NULL){
    return &cps->used_mystatus;
  }
  return &cps->mystatus;
}


