//======================================================================
/**
 * @file  field_menu.h
 * @brief �t�B�[���h���j���[
 * @author  ariizumi
 * @date  09/05/12
 */
//======================================================================

#include "field/field_menu_item.h"     //FIELD_MENU_ITEM_TYPE�Q��

#ifndef FIELD_MENU_H__
#define FIELD_MENU_H__

//----------------------------------------------------------------------------------
// ���j���[�^�C�v
//----------------------------------------------------------------------------------
enum{
  FIELD_MENU_NORMAL=0,              // �ʏ탁�j���[
  FIELD_MENU_UNION,                 // ���j�I�����[��
  FIELD_MENU_NO_ZUKAN,              // �}�Ӗ���
  FIELD_MENU_NO_POKEMON_NO_ZUKAN,   // �}�Ӗ����E�|�P��������
  FIELD_MENU_PLEASURE_BOAT,         // �V���D��
  FIELD_MENU_PALACE,                ///< �p���X�E���t�B�[���h�ɂ���ꍇ

  FIELD_MENU_TYPE_MAX,
};



typedef struct _FIELD_MENU_WORK FIELD_MENU_WORK;

extern FIELD_MENU_WORK* FIELD_MENU_InitMenu( const HEAPID heapId , const HEAPID tempHeapId , FIELD_SUBSCREEN_WORK* subScreenWork , FIELDMAP_WORK *fieldWork , const BOOL isScrollIn );
extern void FIELD_MENU_ExitMenu( FIELD_MENU_WORK* work );
extern void FIELD_MENU_UpdateMenu( FIELD_MENU_WORK* work );
extern void FIELD_MENU_DrawMenu( FIELD_MENU_WORK* work );

extern const FIELD_MENU_ITEM_TYPE FIELD_MENU_GetMenuItemNo( FIELD_MENU_WORK* work );
extern void FIELD_MENU_SetMenuItemNo( FIELD_MENU_WORK* work , const FIELD_MENU_ITEM_TYPE itemType );
extern void FIELDMENU_RewriteInfoScreen( HEAPID heapId );
extern int FIELDMENU_GetMenuType( GAMEDATA * gamedata, EVENTWORK *ev, int zoneId );



#endif //FIELD_MENU_H__
