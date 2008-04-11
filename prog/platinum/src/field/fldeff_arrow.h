//==============================================================================
/**
 * @file	fldeff_arrow.h
 * @brief	フィールドOBJ矢印
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef FLDEFF_ARROW_H_FILE
#define FLDEFF_ARROW_H_FILE

#include "fieldobj.h"
#include "field_effect.h"
#include "player.h"

//==============================================================================
//	define
//==============================================================================

//==============================================================================
//	typedef
//==============================================================================

//==============================================================================
//	外部参照
//==============================================================================
extern void * FE_Arrow_Init( FE_SYS *fes );
extern void FE_Arrow_Delete( void *work );

extern void FE_PlayerExitArrow_Add( PLAYER_STATE_PTR jiki );

#endif //FLDEFF_ARROW_H_FILE
