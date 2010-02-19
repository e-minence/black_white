//======================================================================
/**
 *
 * @file  mb_data_pt.c 
 * @brief wb�_�E�����[�h�v���C�@�v���`�i�p�@��ʃR�[�h
 * @author  ariizumi
 * @data  09/11/20
 */
//======================================================================

#pragma once

#include "./pt_save.h"

extern BOOL MB_DATA_PT_LoadData( MB_DATA_WORK *dataWork );
extern BOOL MB_DATA_PT_SaveData( MB_DATA_WORK *dataWork );

extern u32   MB_DATA_PT_GetStartAddress( const PT_GMDATA_ID id );
extern u32   MB_DATA_DP_GetStartAddress( const PT_GMDATA_ID id );

extern void* MB_DATA_PT_GetBoxPPP( MB_DATA_WORK *dataWork , const u8 tray , const u8 idx );
extern u16*  MB_DATA_PT_GetBoxName( MB_DATA_WORK *dataWork , const u8 tray );
extern void  MB_DATA_PT_ClearBoxPPP( MB_DATA_WORK *dataWork , const u8 tray , const u8 idx );
extern void  MB_DATA_PT_AddItem( MB_DATA_WORK *dataWork , u16 itemNo );
extern const u16 MB_DATA_PT_GetItemNum( MB_DATA_WORK *dataWork , const u16 itemNo );

