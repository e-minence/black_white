//======================================================================
/**
 * @file	field_menu.h
 * @brief	�t�B�[���h���j���[
 * @author	ariizumi
 * @data	09/05/12
 */
//======================================================================

#include "field/field_menu_item.h"     //FIELD_MENU_ITEM_TYPE�Q��

#ifndef FIELD_MENU_H__
#define FIELD_MENU_H__

typedef struct _FIELD_MENU_WORK FIELD_MENU_WORK;

extern FIELD_MENU_WORK* FIELD_MENU_InitMenu( const HEAPID heapId , FIELD_SUBSCREEN_WORK* subScreenWork , FIELDMAP_WORK *fieldWork , const BOOL isScrollIn );
extern void FIELD_MENU_ExitMenu( FIELD_MENU_WORK* work );
extern void FIELD_MENU_UpdateMenu( FIELD_MENU_WORK* work );
extern void FIELD_MENU_DrawMenu( FIELD_MENU_WORK* work );

extern const FIELD_MENU_ITEM_TYPE FIELD_MENU_GetMenuItemNo( FIELD_MENU_WORK* work );
extern void FIELD_MENU_SetMenuItemNo( FIELD_MENU_WORK* work , const FIELD_MENU_ITEM_TYPE itemType );

#endif //FIELD_MENU_H__