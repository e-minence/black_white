//======================================================================
/**
 * @file	sta_act_button.h
 * @brief	ステージ用　一発逆転ボタン
 * @author	ariizumi
 * @data	09/08/26
 */
//======================================================================
#pragma once

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

typedef struct _STA_BUTTON_SYS STA_BUTTON_SYS;

//======================================================================
//	proto
//======================================================================
STA_BUTTON_SYS* STA_BUTTON_InitSystem( HEAPID heapId , ACTING_WORK* actWork , MUSICAL_POKE_PARAM *musPoke );
void STA_BUTTON_ExitSystem( STA_BUTTON_SYS *work );
void STA_BUTTON_UpdateSystem( STA_BUTTON_SYS *work );
void STA_BUTTON_SetCanUseButton( STA_BUTTON_SYS *work , const BOOL flg );
