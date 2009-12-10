//======================================================================
/**
 *
 * @file  mb_data_main.c
 * @brief セーブデータを読み込むところ
 *      基本的にバージョンごとの処理を分けるラッパー
 * @author  ariizumi
 * @data  09/11/20
 */
//======================================================================
#include <gflib.h>
#include <string.h>
#include <backup_system.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "savedata_def.h"

#include "multiboot/mb_data_main.h"
#include "./mb_data_pt.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  enum
//======================================================================

//======================================================================
//  typedef struct
//======================================================================

//======================================================================
//  proto
//======================================================================

MB_DATA_WORK* MB_DATA_InitSystem( int heapID );
void        MB_DATA_TermSystem( MB_DATA_WORK *dataWork );

BOOL  MB_DATA_LoadDataFirst( MB_DATA_WORK *dataWork );
BOOL  MB_DATA_SaveData( MB_DATA_WORK *dataWork );
u8  MB_DATA_GetErrorState( MB_DATA_WORK *dataWork );

BOOL  MB_DATA_IsFinishSaveFirst( MB_DATA_WORK *dataWork );
BOOL  MB_DATA_IsFinishSaveSecond( MB_DATA_WORK *dataWork );
void  MB_DATA_PermitLastSaveFirst( MB_DATA_WORK *dataWork );
void  MB_DATA_PermitLastSaveSecond( MB_DATA_WORK *dataWork );

//初期化
MB_DATA_WORK* MB_DATA_InitSystem( int heapID )
{
  MB_DATA_WORK *dataWork;

  dataWork = GFL_HEAP_AllocClearMemory(  heapID, sizeof( MB_DATA_WORK ) );

  dataWork->heapId = heapID;

  dataWork->mainSeq = 0;
  dataWork->subSeq   = 0;
  dataWork->pBoxData = NULL;
  dataWork->errorState = DES_NONE;
  dataWork->cardType = CARD_TYPE_INVALID;
  dataWork->isFinishSaveFirst = FALSE;
  dataWork->isFinishSaveSecond = FALSE;
  dataWork->permitLastSaveFirst = FALSE;
  dataWork->permitLastSaveSecond = FALSE;
  MATH_CRC16CCITTInitTable( &dataWork->crcTable_ ); //CRC初期化

  {
    //ROMの識別を行う
    CARDRomHeader *headerData;
    s32 lockID = OS_GetLockID();

    GF_ASSERT( lockID != OS_LOCK_ID_ERROR );
    CARD_LockRom( (u16)lockID );
    headerData = (CARDRomHeader*)CARD_GetRomHeader();
    CARD_UnlockRom( (u16)lockID );
    OS_ReleaseLockID( (u16)lockID );
    
    MB_DATA_TPrintf("RomName[%s]\n",headerData->game_name);
    
    //FIX ME:正しい判別処理を入れる
    if( STD_CompareString( headerData->game_name , "POKEMON D" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON P" ) == 0 )
    {
      dataWork->cardType = CARD_TYPE_DP;
    }
    else if( STD_CompareString( headerData->game_name , "POKEMON PL" ) == 0 )
    {
      dataWork->cardType = CARD_TYPE_PT;
    }
    else if( STD_CompareString( headerData->game_name , "POKEMON HG" ) == 0 ||
      STD_CompareString( headerData->game_name , "POKEMON SS" ) == 0 )
    {
      dataWork->cardType = CARD_TYPE_GS;
    }
    else if( STD_CompareString( headerData->game_name , "NINTENDO    NTRJ01" ) == 0 ||
             STD_CompareString( headerData->game_name , "SKEL" ) == 0 )
    {
      //MBでバグROMかsrl直起動
      dataWork->cardType = CARD_TYPE_DUMMY;
    }
  }
  
  return dataWork;
}

//開放
void  MB_DATA_TermSystem( MB_DATA_WORK *dataWork )
{
  GFL_HEAP_FreeMemory( dataWork->pData );
  GFL_HEAP_FreeMemory( dataWork->pDataMirror );
  GFL_HEAP_FreeMemory( dataWork );
}

//セーブデータの読み込み
BOOL  MB_DATA_LoadDataFirst( MB_DATA_WORK *dataWork )
{
  //自前で読み込む・・・
  switch( dataWork->cardType )
  {
  case CARD_TYPE_DP:
    return MB_DATA_PT_LoadData( dataWork );
    break;

  case CARD_TYPE_PT:
    return MB_DATA_PT_LoadData( dataWork );
    break;

  case CARD_TYPE_GS:
    return TRUE;
    break;
  }
  return FALSE;
}

BOOL  MB_DATA_SaveData( MB_DATA_WORK *dataWork )
{
    //自前で書き込む・・・
  switch( dataWork->cardType )
  {
  case CARD_TYPE_DP:
    return MB_DATA_PT_SaveData( dataWork );
    break;

  case CARD_TYPE_PT:
    return MB_DATA_PT_SaveData( dataWork );
    break;

  case CARD_TYPE_GS:
    return TRUE;
    break;
  }
  return FALSE;
}

//Boxデータの取得(PPP・Name
void* MB_DATA_GetBoxPPP( MB_DATA_WORK *dataWork , const u8 tray , const u8 idx )
{
  switch( dataWork->cardType )
  {
  case CARD_TYPE_DP:
    return MB_DATA_PT_GetBoxPPP( dataWork , tray , idx  );
    break;

  case CARD_TYPE_PT:
    return MB_DATA_PT_GetBoxPPP( dataWork , tray , idx );
    break;

  case CARD_TYPE_GS:
    return NULL;
    break;
  }
  return NULL;
}

u16* MB_DATA_GetBoxName( MB_DATA_WORK *dataWork , const u8 tray )
{
  switch( dataWork->cardType )
  {
  case CARD_TYPE_DP:
    return MB_DATA_PT_GetBoxName( dataWork , tray );
    break;

  case CARD_TYPE_PT:
    return MB_DATA_PT_GetBoxName( dataWork , tray );
    break;

  case CARD_TYPE_GS:
    return NULL;
    break;
  }
  return NULL;
}


//Boxデータの削除(PPP
void MB_DATA_ClearBoxPPP( MB_DATA_WORK *dataWork , const u8 tray , const u8 idx )
{
  switch( dataWork->cardType )
  {
  case CARD_TYPE_DP:
    MB_DATA_PT_ClearBoxPPP( dataWork , tray , idx  );
    break;

  case CARD_TYPE_PT:
    MB_DATA_PT_ClearBoxPPP( dataWork , tray , idx );
    break;

  case CARD_TYPE_GS:
    break;
  }
}

//刺さっているカードの種類の取得設定(設定はデバッグ用
const DLPLAY_CARD_TYPE MB_DATA_GetCardType( MB_DATA_WORK *dataWork )
{
  return dataWork->cardType;
}

void MB_DATA_SetCardType( MB_DATA_WORK *dataWork , const DLPLAY_CARD_TYPE type )
{
  dataWork->cardType = type;
}

u8  MB_DATA_GetErrorState( MB_DATA_WORK *dataWork )
{
  return dataWork->errorState;
}

BOOL  MB_DATA_IsFinishSaveFirst( MB_DATA_WORK *dataWork )
{
  return dataWork->isFinishSaveFirst;
}

BOOL  MB_DATA_IsFinishSaveSecond( MB_DATA_WORK *dataWork )
{
  return dataWork->isFinishSaveSecond;
}

BOOL  MB_DATA_IsFinishSaveAll( MB_DATA_WORK *dataWork )
{
  return dataWork->isFinishSaveAll_;
}

void  MB_DATA_PermitLastSaveFirst( MB_DATA_WORK *dataWork )
{
  dataWork->permitLastSaveFirst = TRUE;
}

void  MB_DATA_PermitLastSaveSecond( MB_DATA_WORK *dataWork )
{
  dataWork->permitLastSaveSecond = TRUE;
}

