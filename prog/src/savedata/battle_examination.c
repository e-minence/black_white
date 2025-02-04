//======================================================================
/**
 * @file     battle_examination.c
 * @brief    バトル検定セーブデータ
 * @authaor  k.ohno
 * @date     10.01.08
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "savedata/save_tbl.h"
#include "poke_tool/poke_tool.h"

#include "savedata/save_control.h"
#include "savedata/battle_examination.h"
#include "battle/bsubway_battle_data.h"


//--------------------------------------------------------------------------------------------
/**
 * @brief   構造体サイズを得る
 * @return	サイズ
 */
//--------------------------------------------------------------------------------------------
int BATTLE_EXAMINATION_SAVE_GetWorkSize(void)
{
	return sizeof(BATTLE_EXAMINATION_SAVEDATA);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   構造体を得る
 * @return	構造体
 */
//--------------------------------------------------------------------------------------------

BATTLE_EXAMINATION_SAVEDATA* BATTLE_EXAMINATION_SAVE_AllocWork(HEAPID heapID)
{
	return GFL_HEAP_AllocClearMemory(heapID, BATTLE_EXAMINATION_SAVE_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   中身を初期化する
 * @return	サイズ
 */
//--------------------------------------------------------------------------------------------

void BATTLE_EXAMINATION_SAVE_Init(BATTLE_EXAMINATION_SAVEDATA* pSV)
{
  GFL_STD_MemClear(pSV,sizeof(BATTLE_EXAMINATION_SAVEDATA));
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   データが入っていて正しいかどうかを返す
 * @return	正しいならTRUE
 */
//--------------------------------------------------------------------------------------------

BOOL BATTLE_EXAMINATION_SAVE_IsInData(BATTLE_EXAMINATION_SAVEDATA* pSV)
{
  if(pSV->crc == GFL_STD_CrcCalc(pSV, sizeof(BATTLE_EXAMINATION_SAVEDATA)-2)){
    if(pSV->bActive == BATTLE_EXAMINATION_MAGIC_KEY){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   シングルバトルかどうか
 * @return	SINGLEならTRUE
 */
//--------------------------------------------------------------------------------------------

BOOL BATTLE_EXAMINATION_SAVE_IsSingleBattle(BATTLE_EXAMINATION_SAVEDATA* pSV)
{
  return ((pSV->dataNo & 0x8000)==0);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   開催番号を得る
 * @return	0-127
 */
//--------------------------------------------------------------------------------------------

u16 BATTLE_EXAMINATION_SAVE_GetDataNo(BATTLE_EXAMINATION_SAVEDATA* pSV)
{
  return pSV->dataNo & 0x7f;
}

//----------------------------------------------------------------------------
/**
 *	@brief	データのポインタ取得
 *
 *	@param	SAVE_CONTROL_WORK * p_sv	セーブデータ保持ワークへのポインタ
 *
 *	@return	BATTLE_EXAMINATION_SAVEDATA
 */
//-----------------------------------------------------------------------------
BATTLE_EXAMINATION_SAVEDATA *BATTLE_EXAMINATION_SAVE_GetSvPtr( SAVE_CONTROL_WORK * p_sv)
{	
	BATTLE_EXAMINATION_SAVEDATA	*data;
  data = SaveControl_Extra_DataPtrGet(p_sv, SAVE_EXTRA_ID_BATTLE_EXAMINATION, 0);
//  data = SaveControl_DataPtrGet(p_sv, GMDATA_ID_EXAMINATION);
	return data;
}

//----------------------------------------------------------
//	セーブデータ取得のための関数
//----------------------------------------------------------

//  if(LOAD_RESULT_OK== SaveControl_Extra_LoadWork(pSave
//,            SAVE_EXTRA_ID_BATTLE_EXAMINATION, HEAPID_FIELDMAP,
  //                      pCGearWork,SAVESIZE_EXTRA_BATTLE_EXAMINATION)){

//--------------------------------------------------------------------------------------------
/**
 * @brief   検定用データを取得する
 * @param   pSV   セーブデータポインタ
 * @param   取得インデックス　0〜BATTLE_EXAMINATION_MAX-1
 * @return	データ先頭アドレス
 */
//--------------------------------------------------------------------------------------------
BSUBWAY_PARTNER_DATA *BATTLE_EXAMINATION_SAVE_GetData(BATTLE_EXAMINATION_SAVEDATA* pSV, const u32 inIdx)
{
  BSUBWAY_PARTNER_DATA *data;
  if ( inIdx >= BATTLE_EXAMINATION_MAX ){
    GF_ASSERT_MSG(0, " idx error $d",inIdx);
    data = &(pSV->trainer[0]);
  }
  else data = &(pSV->trainer[inIdx]);

  return data;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   検定用データセーブする
 * @param   pSave   セーブデータポインタ
 * @param   pBattleEx   検定データポインタ
 * @param   heapID    ヒープ
 */
//--------------------------------------------------------------------------------------------
void BATTLE_EXAMINATION_SAVE_Write(GAMEDATA *gamedata , BATTLE_EXAMINATION_SAVEDATA* pBattleEx, HEAPID heapID)
{
  SAVE_CONTROL_WORK *pSave = GAMEDATA_GetSaveControlWork(gamedata);
  void* pWork = GFL_HEAP_AllocMemory(heapID,SAVESIZE_EXTRA_BATTLE_EXAMINATION);
  
  SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_BATTLE_EXAMINATION, heapID,
                             pWork, SAVESIZE_EXTRA_BATTLE_EXAMINATION);

  GFL_STD_MemCopy(pBattleEx, pWork, sizeof(BATTLE_EXAMINATION_SAVEDATA));

  GAMEDATA_ExtraSaveAsyncStart(gamedata,SAVE_EXTRA_ID_BATTLE_EXAMINATION);
  while(1){
    if(SAVE_RESULT_OK==GAMEDATA_ExtraSaveAsyncMain(gamedata,SAVE_EXTRA_ID_BATTLE_EXAMINATION)){
      break;
    }
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
  }
  SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_BATTLE_EXAMINATION);

  GFL_HEAP_FreeMemory(pWork);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   データを使用したことにするキーをセット
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BATTLE_EXAMINATION_SAVE_SetDataUsedKey(BATTLE_EXAMINATION_SAVEDATA* pSV)
{
  if(pSV->crc == GFL_STD_CrcCalc(pSV, sizeof(BATTLE_EXAMINATION_SAVEDATA)-2)){
    int datasize;
    pSV->bActive = BATTLE_EXAMINATION_USED_MAGIC_KEY;
    datasize = sizeof(BATTLE_EXAMINATION_SAVEDATA);   //データ全体サイズ
    //ＣＲＣ再計算
    pSV->crc = GFL_STD_CrcCalc(pSV, datasize-2);
  }
}

