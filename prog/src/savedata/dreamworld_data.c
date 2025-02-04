//============================================================================================
/**
 * @file	  dreamworld_data.c
 * @brief	  ポケモンドリームワールドに必要なデータ
 * @author	k.ohno
 * @date	  2009.05.14
 */
//============================================================================================

#include <gflib.h>

#include "dreamworld_data_local.h"



//--------------------------------------------------------------------------------------------
/**
 * @brief   構造体サイズを得る
 * @return	サイズ
 */
//--------------------------------------------------------------------------------------------
int DREAMWORLD_SV_GetWorkSize(void)
{
	return sizeof(DREAMWORLD_SAVEDATA);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   DREAMWORLD_SAVEDATA構造体を得る
 * @return	DREAMWORLD_SAVEDATA構造体
 */
//--------------------------------------------------------------------------------------------

DREAMWORLD_SAVEDATA* DREAMWORLD_SV_AllocWork(HEAPID heapID)
{
	return GFL_HEAP_AllocClearMemory(heapID, DREAMWORLD_SV_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   中身を初期化する
 * @param   DREAMWORLD_SAVEDATAのポインタ
 * @return	サイズ
 */
//--------------------------------------------------------------------------------------------

void DREAMWORLD_SV_Init(DREAMWORLD_SAVEDATA* pSV)
{
  u8 i;
	GFL_STD_MemClear(pSV, DREAMWORLD_SV_GetWorkSize());
  DREAMWORLD_SV_SetTime(pSV , GFDATE_Set(2010,1,1,0));

  pSV->uploadCount = 0xffffffff;
  pSV->musicalNo = DREAM_WORLD_NOPICTURE;
  pSV->cgearNo = DREAM_WORLD_NOPICTURE;
  pSV->zukanNo = DREAM_WORLD_NOPICTURE;
  pSV->furnitureNo = DREAM_WORLD_NOFURNITURE;
  
  for( i=0;i<DREAM_WORLD_DATA_MAX_FURNITURE;i++ )
  {
    pSV->furnitureID[i].id = DREAM_WORLD_INVALID_FURNITURE;
  }
}



//--------------------------------------------------------------------------------------------
/**
 * @brief   寝てるポケモンを得る
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	POKEMON_PASO_PARAM
 */
//--------------------------------------------------------------------------------------------
POKEMON_PARAM* DREAMWORLD_SV_GetSleepPokemon(DREAMWORLD_SAVEDATA* pSV)
{
  return &pSV->pp;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   寝てるポケモンをセットする
            データの検査はここではしてないので必ず入れる前にする事
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  POKEMON_PASO_PARAM*
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetSleepPokemon(DREAMWORLD_SAVEDATA* pSV,POKEMON_PARAM* pp)
{
  GFL_STD_MemCopy(pp,&pSV->pp,POKETOOL_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   眠る事ができるポケモンかどうか
 * @param	  u16 pData   眠るリスト
 * @param	  u16 monsno  モンスター番号
 * @return  ねむれればTRUE
 */
//--------------------------------------------------------------------------------------------
BOOL DREAMWORLD_SV_GetSleepEnable(u16* pData,u16 monsno)
{
  u8* pArray = (u8*)pData;
  int no = monsno/8;
  int bitno = monsno%8;

  if(pArray[no] & (0x01 << bitno)){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   PDW登録ずみフラグを入れる
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  PDW登録ずみ
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetSignin(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg)
{
  pSV->signin = bFlg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   PDW登録ずみかどうか
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	PDW登録ずみ時はTRUE
 */
//--------------------------------------------------------------------------------------------
BOOL DREAMWORLD_SV_GetSignin(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->signin;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アカウント登録を一回でも起動したかどうか
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  アカウント登録していたらTRUE
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetAccount(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg)
{
  pSV->bAccount = bFlg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アカウント登録を一回でも起動したかどうか
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	アカウント登録時はTRUE
 */
//--------------------------------------------------------------------------------------------
BOOL DREAMWORLD_SV_GetAccount(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->bAccount;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   寝てるポケモンをセットしたらフラグをON
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  ポケモンセット時はTRUE
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetSleepPokemonFlg(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg)
{
  pSV->pokemonIn = bFlg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   寝てるポケモンをセットしたかどうか確認
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	ポケモンセット時はTRUE
 */
//--------------------------------------------------------------------------------------------
BOOL DREAMWORLD_SV_GetSleepPokemonFlg(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->pokemonIn;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   貰ったお土産のアイテムNoを得る
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  index インデックス
 * @return	アイテムNo
 */
//--------------------------------------------------------------------------------------------

u16 DREAMWORLD_SV_GetItem(DREAMWORLD_SAVEDATA* pSV,int index)
{
  if(index > DREAM_WORLD_DATA_MAX_ITEMBOX){
    return 0;
  }
  if(pSV->itemID[index] > ITEM_DATA_MAX){
    return 0;
  }
  return pSV->itemID[index];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテムの数を取得
 * @param	  pSV		DREAMWORLD_SAVEDATAポインタ
 * @param	  index		場所
 * @return	  num		 数
 */
//--------------------------------------------------------------------------------------------

u8 DREAMWORLD_SV_GetItemNum(DREAMWORLD_SAVEDATA* pSV, int index)
{
  if(index > DREAM_WORLD_DATA_MAX_ITEMBOX){
    return 0;
  }
  if(DREAM_WORLD_DATA_MAX_ITEMNUM < pSV->itemNum[index]){
    return 0;
  }
  return pSV->itemNum[index];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   貰ったアイテムをいれる
 * @param	  pSV		DREAMWORLD_SAVEDATAポインタ
 * @param	  index		場所
 * @param	  itemNo		アイテム番号
 * @param	  num		 数
 */
//--------------------------------------------------------------------------------------------

void DREAMWORLD_SV_SetItem(DREAMWORLD_SAVEDATA* pSV,int index ,int itemNo, int num)
{
  if(index > DREAM_WORLD_DATA_MAX_ITEMBOX){
    return;
  }
  pSV->itemID[index] = itemNo;
  pSV->itemNum[index] = num;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   貰ったアイテム消す
 * @param	  pSV		DREAMWORLD_SAVEDATAポインタ
 * @param	  index		場所
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_DeleteItem(DREAMWORLD_SAVEDATA* pSV,int index)
{
  DREAMWORLD_SV_SetItem(pSV,index,0,0);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   貰ったアイテム全部消す
 * @param	  pSV		DREAMWORLD_SAVEDATAポインタ
 */
//--------------------------------------------------------------------------------------------

void DREAMWORLD_SV_ClearAllItem(DREAMWORLD_SAVEDATA* pSV)
{
  int i;

  for(i=0;i<DREAM_WORLD_DATA_MAX_ITEMBOX;i++){
    DREAMWORLD_SV_DeleteItem(pSV,i);
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテムの残っているindex数を返す
 * @param	  pSV		DREAMWORLD_SAVEDATAポインタ
 * @retval	残り数
 */
//--------------------------------------------------------------------------------------------

int DREAMWORLD_SV_GetItemRestNum(DREAMWORLD_SAVEDATA* pSV)
{
  int count=0,i;

  for(i=0;i<DREAM_WORLD_DATA_MAX_ITEMBOX;i++){
    if(0!=DREAMWORLD_SV_GetItemNum(pSV,i)){
      count++;
    }
  }
  return count;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   残っているアイテムを構造体に入れて返す
 * @param	  pSV		DREAMWORLD_SAVEDATAポインタ
 * @param	  index		何番目のデータなのか返す
 * @param	  pItem		アイテムの種類と数を構造体で返す
 * @retval	返せたらTRUE
 */
//--------------------------------------------------------------------------------------------

BOOL DREAMWORLD_SV_GetRestItem(DREAMWORLD_SAVEDATA* pSV, int* index, DREAMWORLD_ITEM_DATA* pItem)
{
  int i;

  for(i=0;i<DREAM_WORLD_DATA_MAX_ITEMBOX;i++){
    if(0 != DREAMWORLD_SV_GetItemNum(pSV,i)){
      pItem->itemindex = DREAMWORLD_SV_GetItem(pSV,i);
      pItem->itemnum = DREAMWORLD_SV_GetItemNum(pSV,i);
      return TRUE;
    }
  }
  return FALSE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   家具データをえる
 * @param	  pSV		DREAMWORLD_SAVEDATAポインタ
 * @return	DREAM_WORLD_FURNITUREDATA*
 */
//--------------------------------------------------------------------------------------------
DREAM_WORLD_FURNITUREDATA* DREAMWORLD_SV_GetFurnitureData(DREAMWORLD_SAVEDATA* pSV,int index)
{
  GF_ASSERT(index < DREAM_WORLD_DATA_MAX_FURNITURE);
  if(!(index < DREAM_WORLD_DATA_MAX_FURNITURE)){
    return NULL;
  }
  return &pSV->furnitureID[index];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   家具データを格納する
 * @param	  pSV		DREAMWORLD_SAVEDATAポインタ
 * @return	DREAM_WORLD_FURNITUREDATA*
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetFurnitureData(DREAMWORLD_SAVEDATA* pSV,int index,DREAM_WORLD_FURNITUREDATA* pF)
{
  GF_ASSERT(index < DREAM_WORLD_DATA_MAX_FURNITURE);
  if(!(index < DREAM_WORLD_DATA_MAX_FURNITURE)){
    return;
  }
  GFL_STD_MemCopy(pF,&pSV->furnitureID[index],sizeof(DREAM_WORLD_FURNITUREDATA));
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   寝てるポケモンのステータスを得る
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	DREAM_WORLD_RESULT_TYPE
 */
//--------------------------------------------------------------------------------------------
DREAM_WORLD_SLEEP_TYPE DREAMWORLD_SV_GetPokemonStatus(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->pokemoStatus;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   寝てるポケモンのステータスをセット
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  status
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetPokemonStatus(DREAMWORLD_SAVEDATA* pSV,DREAM_WORLD_SLEEP_TYPE status)
{
  GF_ASSERT(status < DREAM_WORLD_SLEEP_TYPE_MAX);
  if(!(status < DREAM_WORLD_SLEEP_TYPE_MAX)){
    return;
  }
  pSV->pokemoStatus = status;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   GSYNCを行い、預けた時間を得る
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	GFDATE
 */
//--------------------------------------------------------------------------------------------
GFDATE DREAMWORLD_SV_GetTime(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->gsyncTime;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   預けた時間を書く
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  GFDATE  時間
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetTime(DREAMWORLD_SAVEDATA* pSV,GFDATE date)
{
  pSV->gsyncTime = date;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   GSYNCを行い、預けた回数を取得
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	回数
 */
//--------------------------------------------------------------------------------------------
int DREAMWORLD_SV_GetUploadCount(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->uploadCount;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   預けた回数を書く
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  回数
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetUploadCount(DREAMWORLD_SAVEDATA* pSV,int count)
{
  pSV->uploadCount = count;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   現在適応中ミュージカルデータ番号を得る
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	ミュージカルデータ番号
 */
//--------------------------------------------------------------------------------------------
int DREAMWORLD_SV_GetMusicalNo(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->musicalNo;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   現在適応中ミュージカルデータ番号をセットする
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  ミュージカルデータ番号
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetMusicalNo(DREAMWORLD_SAVEDATA* pSV,int no)
{
  pSV->musicalNo = no;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   現在適応中CGEAR番号を得る
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	CGEARデータ番号
 */
//--------------------------------------------------------------------------------------------
int DREAMWORLD_SV_GetCGearNo(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->cgearNo;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   現在適応中CGEAR番号をセットする
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  CGEARデータ番号
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetCGearNo(DREAMWORLD_SAVEDATA* pSV,int no)
{
  pSV->cgearNo = no;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   現在適応中図鑑番号を得る
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	図鑑データ番号
 */
//--------------------------------------------------------------------------------------------
int DREAMWORLD_SV_GetZukanNo(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->zukanNo;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   現在適応中図鑑番号をセットする
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  図鑑データ番号
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetZukanNo(DREAMWORLD_SAVEDATA* pSV,int no)
{
  pSV->zukanNo = no;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   家具の番号を得る
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	選んだ家具のインデックス
 */
//--------------------------------------------------------------------------------------------
int DREAMWORLD_SV_GetSelectFurnitureNo(DREAMWORLD_SAVEDATA* pSV)
{
  if(pSV->furnitureNo < DREAM_WORLD_DATA_MAX_FURNITURE || (pSV->furnitureNo == DREAM_WORLD_NOFURNITURE)){
    return pSV->furnitureNo;
  }
  return DREAM_WORLD_NOFURNITURE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   選んだ家具の番号セット
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  選んだ家具のインデックス
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetSelectFurnitureNo(DREAMWORLD_SAVEDATA* pSV,int no)
{
  if(no < DREAM_WORLD_DATA_MAX_FURNITURE || (no == DREAM_WORLD_NOFURNITURE)){
    pSV->furnitureNo = no;
    OS_TPrintf("選んだ家具%d  \n",no);
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   家具をSyncで送ったか？
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	BOOL
 */
//--------------------------------------------------------------------------------------------
BOOL DREAMWORLD_SV_GetIsSyncFurniture(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->isSyncFurniture;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   家具をSyncで送ったか？
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  BOOL
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetIsSyncFurniture(DREAMWORLD_SAVEDATA* pSV,BOOL flg)
{
  pSV->isSyncFurniture = flg;
}




//----------------------------------------------------------
//	セーブデータ取得のための関数
//----------------------------------------------------------
DREAMWORLD_SAVEDATA* DREAMWORLD_SV_GetDreamWorldSaveData(SAVE_CONTROL_WORK* pSave)
{
	DREAMWORLD_SAVEDATA* pData;
	pData = SaveControl_DataPtrGet(pSave, GMDATA_ID_DREAMWORLD);
	return pData;

}




