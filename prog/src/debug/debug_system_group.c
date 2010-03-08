//======================================================================
/**
 * @file	debug_system_group.c
 * @brief	�f�o�b�O�V�X�e���̃f�t�H���g�O���[�v�o�^
 * @author	ariizumi
 * @data	09/05/08
 */
//======================================================================
#include <gflib.h>

#include "debug/debugwin_sys.h"
#include "debug/debug_system_group.h"
#include "debug/debug_flg.h"
#include "savedata/config.h"
#include "savedata/dreamworld_data.h"
#include "test/performance.h"
#include "test/debug_pause.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

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
  RTCTime rtcTime;
  RTCDate rtcDate;
  
}DEBUG_SYS_GROUP_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void DEBWIN_Update_PMeterOn( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_PMeterOff( void* userWork , DEBUGWIN_ITEM* item );

static void DEBWIN_Update_Pause( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_Pause( void* userWork , DEBUGWIN_ITEM* item );

static void DEBWIN_Update_Kanji( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_Kanji( void* userWork , DEBUGWIN_ITEM* item );

static void DEBWIN_Update_RTC_year( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_RTC_month( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_RTC_day( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_RTC_hour( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_RTC_minute( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_RTC_sec( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_RTC_apply( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_RTC_get( void* userWork , DEBUGWIN_ITEM* item );

static void DEBWIN_Draw_RTC_year( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_RTC_month( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_RTC_day( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_RTC_hour( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_RTC_minute( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_RTC_sec( void* userWork , DEBUGWIN_ITEM* item );

static void DEBWIN_U_PDW_Account( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_D_PDW_Account( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_U_PDW_SetFurniture( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_D_PDW_SetFurniture( void* userWork , DEBUGWIN_ITEM* item );

static const BOOL DEBWIN_UTIL_UpdateU32( u32 *value , const u32 min, const u32 max );
static const BOOL DEBWIN_UTIL_UpdateBOOL( BOOL *value );

DEBUG_SYS_GROUP_WORK *sysGroupWork = NULL;
//--------------------------------------------------------------
//	
//--------------------------------------------------------------

#pragma mark [> Init&Term

void DEBUGWIN_AddSystemGroup( const HEAPID heapId )
{
  sysGroupWork = GFL_HEAP_AllocMemory( heapId , sizeof(DEBUG_SYS_GROUP_WORK) );
  
  GFL_RTC_GetDateTime( &sysGroupWork->rtcDate , &sysGroupWork->rtcTime );
  
  DEBUGWIN_AddGroupToTop( DEBUGWIN_GROUPID_SYSTEM , "System" , heapId );
  DEBUGWIN_AddGroupToTop( DEBUGWIN_GROUPID_PDW , "PDW" , heapId );
  
  //System��
  DEBUGWIN_AddGroupToGroup( DEBUGWIN_GROUPID_RTC , "RTC" , DEBUGWIN_GROUPID_SYSTEM , heapId );
  DEBUGWIN_AddItemToGroup( "�ӂ����[�^�[ON",DEBWIN_Update_PMeterOn , NULL , DEBUGWIN_GROUPID_SYSTEM , heapId );
  DEBUGWIN_AddItemToGroup( "�ӂ����[�^�[OFF",DEBWIN_Update_PMeterOff , NULL , DEBUGWIN_GROUPID_SYSTEM , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_Pause   ,DEBWIN_Draw_Pause   , (void*)sysGroupWork , DEBUGWIN_GROUPID_SYSTEM , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_Kanji   ,DEBWIN_Draw_Kanji   , (void*)sysGroupWork , DEBUGWIN_GROUPID_SYSTEM , heapId );

  //RTC��
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_RTC_year   ,DEBWIN_Draw_RTC_year   , (void*)sysGroupWork , DEBUGWIN_GROUPID_RTC , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_RTC_month  ,DEBWIN_Draw_RTC_month  , (void*)sysGroupWork , DEBUGWIN_GROUPID_RTC , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_RTC_day    ,DEBWIN_Draw_RTC_day    , (void*)sysGroupWork , DEBUGWIN_GROUPID_RTC , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_RTC_hour   ,DEBWIN_Draw_RTC_hour   , (void*)sysGroupWork , DEBUGWIN_GROUPID_RTC , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_RTC_minute ,DEBWIN_Draw_RTC_minute , (void*)sysGroupWork , DEBUGWIN_GROUPID_RTC , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_RTC_sec    ,DEBWIN_Draw_RTC_sec    , (void*)sysGroupWork , DEBUGWIN_GROUPID_RTC , heapId );
  DEBUGWIN_AddItemToGroup( "�Ă��悤",DEBWIN_Update_RTC_apply , (void*)sysGroupWork , DEBUGWIN_GROUPID_RTC , heapId );
  DEBUGWIN_AddItemToGroup( "����Ƃ�",DEBWIN_Update_RTC_get , (void*)sysGroupWork , DEBUGWIN_GROUPID_RTC , heapId );

  //PDW��
  DEBUGWIN_AddItemToGroupEx( DEBWIN_U_PDW_Account   ,DEBWIN_D_PDW_Account   , (void*)sysGroupWork , DEBUGWIN_GROUPID_PDW , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_U_PDW_SetFurniture   ,DEBWIN_D_PDW_SetFurniture   , (void*)sysGroupWork , DEBUGWIN_GROUPID_PDW , heapId );
  
  
  DEBUG_FLG_CreateDebugGoupe( heapId );
}

void DEBUGWIN_RemoveSystemGroup(void)
{
  DEBUG_FLG_DeleteDebugGoupe();
  DEBUGWIN_RemoveGroup( DEBUGWIN_GROUPID_SYSTEM );
}

#pragma mark [> system

//�p�t�H�[�}���X���[�^�[
static void DEBWIN_Update_PMeterOn(  void* userWork , DEBUGWIN_ITEM* item )
{
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    DEBUG_PerformanceSetActive( TRUE );
  }
}
static void DEBWIN_Update_PMeterOff(  void* userWork , DEBUGWIN_ITEM* item )
{
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    DEBUG_PerformanceSetActive( FALSE );
  }
}

//�ꎞ��~�@�\
static void DEBWIN_Update_Pause( void* userWork , DEBUGWIN_ITEM* item )
{
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    const BOOL flg = DEBUG_PAUSE_GetEnable();
    DEBUG_PAUSE_SetEnable( !flg );
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_Draw_Pause( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL flg = DEBUG_PAUSE_GetEnable();
  if( flg == TRUE )
  {
    DEBUGWIN_ITEM_SetNameV( item , "�|�[�Y[ON]" );
  }
  else
  {
    DEBUGWIN_ITEM_SetNameV( item , "�|�[�Y[OFF]" );
  }
}

//����
static void DEBWIN_Update_Kanji( void* userWork , DEBUGWIN_ITEM* item )
{
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    CONFIG *save = SaveData_GetConfig( SaveControl_GetPointer() );
    const MOJIMODE mojiMode = CONFIG_GetMojiMode(save );

    if( mojiMode == MOJIMODE_HIRAGANA )
    {
      GFL_MSGSYS_SetLangID( 1 );
      CONFIG_SetMojiMode(save,MOJIMODE_KANJI );
    }
    else
    {
      GFL_MSGSYS_SetLangID( 0 );
      CONFIG_SetMojiMode(save,MOJIMODE_HIRAGANA );
    }
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_Draw_Kanji( void* userWork , DEBUGWIN_ITEM* item )
{
  const u8 langId = GFL_MSGSYS_GetLangID();
  if( langId == 0 )
  {
    DEBUGWIN_ITEM_SetNameV( item , "���񂶃��[�h[OFF]" );
  }
  else
  {
    DEBUGWIN_ITEM_SetNameV( item , "���񂶃��[�h[ON]" );
  }
}

#pragma mark [> RTC

//RTC
static void DEBWIN_Update_RTC_year( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBWIN_UTIL_UpdateU32( &work->rtcDate.year , 0 , 99 );
}
static void DEBWIN_Update_RTC_month( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBWIN_UTIL_UpdateU32( &work->rtcDate.month , 1 , 12 );
}
static void DEBWIN_Update_RTC_day( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBWIN_UTIL_UpdateU32( &work->rtcDate.day , 1 , 31 );
}
static void DEBWIN_Update_RTC_hour( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBWIN_UTIL_UpdateU32( &work->rtcTime.hour , 0 , 23 );
}
static void DEBWIN_Update_RTC_minute( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBWIN_UTIL_UpdateU32( &work->rtcTime.minute , 0 , 59 );
}
static void DEBWIN_Update_RTC_sec( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBWIN_UTIL_UpdateU32( &work->rtcTime.second , 0 , 59 );
}
static void DEBWIN_Update_RTC_apply( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    RTC_SetDateTime( &work->rtcDate , &work->rtcTime );
  }
}
static void DEBWIN_Update_RTC_get( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    GFL_RTC_GetDateTime( &work->rtcDate , &work->rtcTime );
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_Draw_RTC_year( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "�˂�[%4d]",work->rtcDate.year+2000 );
}
static void DEBWIN_Draw_RTC_month( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "��[%2d]",work->rtcDate.month );
}
static void DEBWIN_Draw_RTC_day( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "�ɂ�[%2d]",work->rtcDate.day );
}
static void DEBWIN_Draw_RTC_hour( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "��[%2d]",work->rtcTime.hour );
}
static void DEBWIN_Draw_RTC_minute( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "�ӂ�[%2d]",work->rtcTime.minute );
}
static void DEBWIN_Draw_RTC_sec( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUG_SYS_GROUP_WORK *work = (DEBUG_SYS_GROUP_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "�т傤[%2d]",work->rtcTime.second );
}

#pragma mark [> PWD

static void DEBWIN_U_PDW_Account( void* userWork , DEBUGWIN_ITEM* item )
{
  DREAMWORLD_SAVEDATA *pdwSave = DREAMWORLD_SV_GetDreamWorldSaveData(SaveControl_GetPointer());
  BOOL isAccount = DREAMWORLD_SV_GetAccount( pdwSave );
  if( DEBWIN_UTIL_UpdateBOOL( &isAccount ) == TRUE )
  {
    DREAMWORLD_SV_SetAccount( pdwSave , isAccount );
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_D_PDW_Account( void* userWork , DEBUGWIN_ITEM* item )
{
  DREAMWORLD_SAVEDATA *pdwSave = DREAMWORLD_SV_GetDreamWorldSaveData(SaveControl_GetPointer());
  BOOL isAccount = DREAMWORLD_SV_GetAccount( pdwSave );
  
  if( isAccount )
  {
    DEBUGWIN_ITEM_SetNameV( item , "�A�J�E���gO");
  }
  else
  {
    DEBUGWIN_ITEM_SetNameV( item , "�A�J�E���gX");
  }
}

static void DEBWIN_U_PDW_SetFurniture( void* userWork , DEBUGWIN_ITEM* item )
{
  DREAMWORLD_SAVEDATA *pdwSave = DREAMWORLD_SV_GetDreamWorldSaveData(SaveControl_GetPointer());
  DREAM_WORLD_FURNITUREDATA *topFurData = DREAMWORLD_SV_GetFurnitureData( pdwSave , 0 );
  BOOL setEnable = FALSE;
  if( topFurData->id == DREAM_WORLD_INVALID_FURNITURE )
  {
    setEnable = TRUE;
  }
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    u8 i;
    for( i=0;i<DREAM_WORLD_DATA_MAX_FURNITURE;i++ )
    {
      DREAM_WORLD_FURNITUREDATA *furData = DREAMWORLD_SV_GetFurnitureData( pdwSave , i );
      if( setEnable == TRUE )
      {
        furData->id = i+1;
        furData->furnitureName[0] = L'��';
        furData->furnitureName[1] = L'��';
        furData->furnitureName[2] = L'��';
        furData->furnitureName[3] = L'��';
        furData->furnitureName[4] = L'��';
        furData->furnitureName[5] = L'��';
        furData->furnitureName[6] = L'1'+i;
        furData->furnitureName[7] = 0xFFFF;
      }
      else
      {
        furData->id = 0;
      }
    }
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_D_PDW_SetFurniture( void* userWork , DEBUGWIN_ITEM* item )
{
  DREAMWORLD_SAVEDATA *pdwSave = DREAMWORLD_SV_GetDreamWorldSaveData(SaveControl_GetPointer());
  DREAM_WORLD_FURNITUREDATA *topFurData = DREAMWORLD_SV_GetFurnitureData( pdwSave , 0 );
  if( topFurData->id == DREAM_WORLD_INVALID_FURNITURE )
  {
    DEBUGWIN_ITEM_SetNameV( item , "�����J�^���OX");
  }
  else
  {
    DEBUGWIN_ITEM_SetNameV( item , "�����J�^���OO");
  }
}

#pragma mark [> util
static const BOOL DEBWIN_UTIL_UpdateU32( u32 *value , const u32 min, const u32 max )
{
  u32 addValue = 1;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    addValue = 5;
  }
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
  {
    if( *value + addValue <= max )
    {
      *value += addValue;
    }
    else
    {
      *value = min;
    }
    DEBUGWIN_RefreshScreen();
    return TRUE;
  }
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
  {
    if( *value  < min + addValue )
    {
      *value = max;
    }
    else
    {
      *value -= addValue;
    }
    DEBUGWIN_RefreshScreen();
    return TRUE;
  }
  return FALSE;
}

static const BOOL DEBWIN_UTIL_UpdateBOOL( BOOL *value )
{
  if( (GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT) ||
      (GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT)  ||
      (GFL_UI_KEY_GetTrg() & PAD_BUTTON_A)  )
  {
    *value = !*value;
    DEBUGWIN_RefreshScreen();
    return TRUE;
  }
  return FALSE;
}
