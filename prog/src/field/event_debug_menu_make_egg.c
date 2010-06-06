////////////////////////////////////////////////////////////////
/**
 * @brief  �f�o�b�O���j���[�u�^�}�S�쐬�v
 * @file   event_debug_menu_make_egg.c
 * @author obata
 * @date   2010.04.16
 */
////////////////////////////////////////////////////////////////
#include <gflib.h> 
#include "print/global_msg.h" 
#include "field/field_msgbg.h"
#include "fieldmap.h"
#include "event_debug_local.h"
#include "pokemon_egg.h"

#include "arc/debug_message.naix"
#include "msg/debug/msg_debug_make_egg.h"

#include "event_debug_menu_make_egg.h"

#ifdef PM_DEBUG

//--------------------------------------------------------------
// �^�}�S�쐬�@���j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER menuHeader =
{
  1,    //���X�g���ڐ�
  9,    //�\���ő區�ڐ�
  0,    //���x���\���w���W
  13,   //���ڕ\���w���W
  0,    //�J�[�\���\���w���W
  0,    //�\���x���W
  1,    //�\�������F
  15,   //�\���w�i�F
  2,    //�\�������e�F
  0,    //�����Ԋu�w
  1,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_LRKEY, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
// ���j���[���X�g
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST menuList[] = 
{
  {STR_NEWBORN,     (void*)STR_NEWBORN},      // ���܂ꂽ��
  {STR_OLDBORN,     (void*)STR_OLDBORN},      // ���������z��
  {STR_ILLEGAL_EGG, (void*)STR_ILLEGAL_EGG},  // ���߃^�}�S
};

//--------------------------------------------------------------
// ���j���[�������\����
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER menuInitializer = 
{
  NARC_debug_message_debug_make_egg_dat,
  NELEMS(menuList),
  menuList,
  &menuHeader,
  1, 1, 20, 16,
  NULL,
  NULL,
};

//--------------------------------------------------------------
/// �^�}�S�쐬�C�x���g���[�N
//--------------------------------------------------------------
typedef struct {

  HEAPID         heapID;
  GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
  FIELDMAP_WORK* fieldmap;
  FLDMENUFUNC*   menuFunc;
  u32            select;

} EVENT_WORK;


//--------------------------------------------------------------
/**
 * @brief �^�}�S�쐬�C�x���g
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuMakeEggEvent( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK* work = wk;
  
  switch( *seq ) {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldmap, work->heapID,  &menuInitializer );
    (*seq)++;
    break;

  case 1:
    work->select = FLDMENUFUNC_ProcMenu( work->menuFunc );

    if( work->select != FLDMENUFUNC_NULL )
    {
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      (*seq)++; 
    }
    break; 

  case 2: 
    // �^�}�S�쐬
    {
      POKEMON_PARAM* egg = NULL;

      // �쐬
      if( work->select == STR_NEWBORN ) { 
        //�u���܂ꂽ�āv
        egg = DEBUG_POKEMON_EGG_CreatePlainEgg( work->gameData, work->heapID );
      }
      else if( work->select == STR_OLDBORN ) { 
        //�u���������z���v
        egg = DEBUG_POKEMON_EGG_CreateQuickEgg( work->gameData, work->heapID );
      }
      else if( work->select == STR_ILLEGAL_EGG ) { 
        //�u���߃^�}�S�v
        egg = DEBUG_POKEMON_EGG_CreateIllegalEgg( work->gameData, work->heapID );
      }

      // �莝���ɒǉ�
      if( egg ) {
        POKEPARTY* party = GAMEDATA_GetMyPokemon( work->gameData );
        PokeParty_Add( party, egg ); 
        GFL_HEAP_FreeMemory( egg );
      }
    }
    (*seq)++;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * @brief �^�}�S�쐬�C�x���g����
 *
 * @parma gameSystem
 * @param wk FIELDMAP_WORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
GMEVENT* DEBUG_EVENT_FLDMENU_MakeEgg( GAMESYS_WORK* gameSystem, void* wk )
{
  GMEVENT* event;
  EVENT_WORK* work;
  FIELDMAP_WORK* fieldmap = (FIELDMAP_WORK*)wk;
  
  event = GMEVENT_Create(
      gameSystem, NULL, debugMenuMakeEggEvent, sizeof(EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
	work->gameSystem = gameSystem;
  work->gameData   = GAMESYSTEM_GetGameData( gameSystem ); 
	work->heapID     = FIELDMAP_GetHeapID( fieldmap );
	work->fieldmap   = fieldmap;

  return event;
}


#endif //PM_DEBUG
