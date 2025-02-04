//======================================================================
/**
 *
 * @file  mb_data_gs.c 
 * @brief wbダウンロードプレイ　HGSS用機種別コード
 * @author  ariizumi
 * @data  09/11/20
 */
//======================================================================

#pragma once

#include "./gs_save.h"

extern BOOL MB_DATA_GS_LoadData( MB_DATA_WORK *dataWork );
extern BOOL MB_DATA_GS_SaveData( MB_DATA_WORK *dataWork );
extern BOOL MB_DATA_GS_LoadRomCRC( MB_DATA_WORK *dataWork );

extern u32   MB_DATA_GS_GetStartAddress( const GS_GMDATA_ID id );

extern void* MB_DATA_GS_GetBoxPPP( MB_DATA_WORK *dataWork , const u8 tray , const u8 idx );
extern u16*  MB_DATA_GS_GetBoxName( MB_DATA_WORK *dataWork , const u8 tray );
extern void  MB_DATA_GS_ClearBoxPPP( MB_DATA_WORK *dataWork , const u8 tray , const u8 idx );
extern void  MB_DATA_GS_AddItem( MB_DATA_WORK *dataWork , u16 itemNo );
extern const u16 MB_DATA_GS_GetItemNum( MB_DATA_WORK *dataWork , const u16 itemNo );
extern const BOOL MB_DATA_GS_CheckLockCapsule( MB_DATA_WORK *dataWork );
extern void MB_DATA_GS_RemoveLockCapsule( MB_DATA_WORK *dataWork );
