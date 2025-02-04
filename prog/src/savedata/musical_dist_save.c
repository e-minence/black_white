//============================================================================================
/**
 * @file	musical_dist_save.c
 * @brief	ミュージカル配信用セーブデータ
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 */
//============================================================================================
#include <gflib.h>

#include "savedata/save_tbl.h"
#include "savedata/musical_dist_save.h"
#include "savedata/musical_save.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
//確保するサイズ
#define MUSICAL_DIST_SAVE_SIZE (127*1024)
//Allocするサイズ
#define MUSICAL_DIST_SAVE_WORK_SIZE (128*1024)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct
{
  u32 size;
  u32 pad[3];
}DIST_DATA_HEADER;

struct _MUSICAL_DIST_SAVE
{
  HEAPID heapId;
  BOOL isEnableData;
  int  saveSeq;
  DIST_DATA_HEADER dataHeader;
  void *saveData;
  
  DIST_DATA_HEADER *saveDataHeader;
  void *saveDataArc;
  GAMEDATA *gamedata;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

int MUSICAL_DIST_SAVE_GetWorkSize(void)
{
  //ヘッダ分減らしておく
  //殿堂入りでさらに減るかも
	return MUSICAL_DIST_SAVE_SIZE;
}

void MUSICAL_DIST_SAVE_InitWork(MUSICAL_DIST_SAVE *musDistSave)
{
  //処理なし
}

MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_LoadData( GAMEDATA *gamedata , HEAPID heapId )
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);
  MUSICAL_DIST_SAVE *distSave = GFL_HEAP_AllocClearMemory( heapId , sizeof(MUSICAL_DIST_SAVE) );
  LOAD_RESULT ret;
  distSave->saveData = GFL_HEAP_AllocClearMemory( heapId , MUSICAL_DIST_SAVE_WORK_SIZE );
  ret = SaveControl_Extra_LoadWork( sv , SAVE_EXTRA_ID_MUSICAL_DIST , heapId , distSave->saveData , MUSICAL_DIST_SAVE_WORK_SIZE );
  
  distSave->heapId = heapId;
  distSave->gamedata = gamedata;
  distSave->saveSeq = 0;
  if( ret == LOAD_RESULT_OK )
  {
    //データがある
    distSave->isEnableData = TRUE;
    distSave->saveData = SaveControl_Extra_DataPtrGet( sv , SAVE_EXTRA_ID_MUSICAL_DIST , EXGMDATA_ID_MUSICAL_DIST );
    distSave->saveDataHeader = distSave->saveData;
    distSave->saveDataArc = (void*)((u32)distSave->saveData + sizeof(DIST_DATA_HEADER));
  }
  else
  {
    //データが無い
    distSave->isEnableData = FALSE;
    distSave->saveData = SaveControl_Extra_DataPtrGet( sv , SAVE_EXTRA_ID_MUSICAL_DIST , EXGMDATA_ID_MUSICAL_DIST );
    distSave->saveDataHeader = distSave->saveData;
    distSave->saveDataArc = (void*)((u32)distSave->saveData + sizeof(DIST_DATA_HEADER));
  }
  return distSave;
}

void MUSICAL_DIST_SAVE_UnloadData( MUSICAL_DIST_SAVE *distSave )
{
  SaveControl_Extra_Unload( GAMEDATA_GetSaveControlWork(distSave->gamedata) , SAVE_EXTRA_ID_MUSICAL_DIST );
  if( distSave->saveData != NULL )
  {
    GFL_HEAP_FreeMemory( distSave->saveData );
  }
  GFL_HEAP_FreeMemory( distSave );
}

//InitとMainでセーブの確保・開放、データ有効フラグのセットなどを行います。
MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_SaveMusicalArchive_Init( GAMEDATA *gamedata , void *arcData , const u32 size , const HEAPID heapId )
{
  MUSICAL_DIST_SAVE *distSave = MUSICAL_DIST_SAVE_LoadData( gamedata , heapId );
  if( distSave->saveData != NULL )
  {
    distSave->dataHeader.size = size;
    GFL_STD_MemCopy( &distSave->dataHeader , distSave->saveData , sizeof( DIST_DATA_HEADER ) );
    GFL_STD_MemCopy( arcData , distSave->saveDataArc , size );
  }
  return distSave;
}

//処理が終わるとMUSICAL_DIST_SAVEは無効になっています。
const BOOL MUSICAL_DIST_SAVE_SaveMusicalArchive_Main( MUSICAL_DIST_SAVE *distSave )
{
  switch( distSave->saveSeq )
  {
  case 0:
    if( distSave->saveData == NULL )
    {
      //データが壊れていてセーブできない。
      MUSICAL_DIST_SAVE_UnloadData( distSave );
      return TRUE;
    }
    else
    {
      MUSICAL_SAVE* musSave = MUSICAL_SAVE_GetMusicalSave( GAMEDATA_GetSaveControlWork(distSave->gamedata) );
      //有効フラグの設定
      MUSICAL_SAVE_SetEnableDistributData( musSave , TRUE );
      //演目タイトルの退避
      {
        u32 msgDataSize;
        ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandleByMemory( distSave->saveDataArc , distSave->dataHeader.size , GFL_HEAP_LOWID(distSave->heapId) );
        void *msgData = GFL_ARCHDL_UTIL_LoadEx( arcHandle , MUSICAL_ARCDATAID_GMMDATA , FALSE , GFL_HEAP_LOWID(distSave->heapId) , &msgDataSize );
        GFL_MSGDATA *msgHandle = GFL_MSG_Construct( msgData, GFL_HEAP_LOWID(distSave->heapId) );
        STRBUF *tmpBuf = GFL_MSG_CreateString( msgHandle , 0 );
        STRCODE *saveBuf = MUSICAL_SAVE_GetDistributTitle( musSave );

        GFL_STR_GetStringCode( tmpBuf , saveBuf , MUSICAL_PROGRAM_NAME_MAX );

        GFL_STR_DeleteBuffer( tmpBuf );
        GFL_MSG_Delete( msgHandle );
        GFL_HEAP_FreeMemory( msgData );
        GFL_ARC_CloseDataHandle( arcHandle );
      }
      GAMEDATA_ExtraSaveAsyncStart( distSave->gamedata , SAVE_EXTRA_ID_MUSICAL_DIST );
      distSave->saveSeq++;
    }
    break;
  case 1:
    {
      const SAVE_RESULT ret = GAMEDATA_ExtraSaveAsyncMain( distSave->gamedata , SAVE_EXTRA_ID_MUSICAL_DIST );
      if( ret == SAVE_RESULT_OK )
      {
        MUSICAL_DIST_SAVE_UnloadData( distSave );
        return TRUE;
      }
    }
    break;
  }
  return FALSE;
}



#pragma mark [> proto

void* MUSICAL_DIST_SAVE_GetMusicalArc( MUSICAL_DIST_SAVE *distSave )
{
  return distSave->saveDataArc;
}
const u32 MUSICAL_DIST_SAVE_GetMusicalArcSize( MUSICAL_DIST_SAVE *distSave )
{
  return distSave->saveDataHeader->size;
}