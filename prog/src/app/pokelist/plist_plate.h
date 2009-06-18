//======================================================================
/**
 * @file	plist_plate.c
 * @brief	ポケモンリスト プレート
 * @author	ariizumi
 * @data	09/06/10
 *
 * モジュール名：PLIST_PLATE
 */
//======================================================================
#pragma once

#include "plist_local_def.h"
#include "poke_tool\poke_tool.h"

//プレートの色の種類(そのままパレット番号に対応
typedef enum
{
  PPC_NORMAL        = 3,  //通常
  PPC_NORMAL_SELECT = 7,  //通常(選択
  PPC_CHANGE        = 6,  //入れ替え
}PLIST_PLATE_COLTYPE;

extern PLIST_PLATE_WORK* PLIST_PLATE_CreatePlate( PLIST_WORK *work , const u8 idx , POKEMON_PARAM *pp );
extern PLIST_PLATE_WORK* PLIST_PLATE_CreatePlate_Blank( PLIST_WORK *work , const u8 idx );

extern void PLIST_PLATE_DeletePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );
extern void PLIST_PLATE_UpdatePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );

extern void PLIST_PLATE_SetActivePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , const BOOL isActive );

extern void PLIST_PLATE_ChangeColor( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , PLIST_PLATE_COLTYPE col );

extern const BOOL PLIST_PLATE_CanSelect( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );
//プレートの位置取得(clact用
extern void PLIST_PLATE_GetPlatePosition( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , GFL_CLACTPOS *pos );
//プレートの位置取得(タッチ判定用
extern void PLIST_PLATE_GetPlateRect( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , GFL_UI_TP_HITTBL *hitTbl );
