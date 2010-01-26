
#include <gflib.h> 
#include "event_debug_menu_make_egg.h"

#include "field/field_msgbg.h"
#include "event_debug_local.h"

#include "arc/message.naix"
#include "msg/msg_debug_make_egg.h"


//--------------------------------------------------------------
// �^�}�S�쐬�@���j���[�w�b�_�[
//--------------------------------------------------------------
const FLDMENUFUNC_HEADER menuHeader =
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
const FLDMENUFUNC_LIST menuList[] = 
{
  {STR_TEMOTI, STR_TEMOTI},  // �莝��
};

//--------------------------------------------------------------
// ���j���[�������\����
//--------------------------------------------------------------
const DEBUG_MENU_INITIALIZER menuInitializer = 
{
  NARC_message_debug_make_egg_dat,
  NELEMS(menuList),
  menuList,
  &menuHeader,
  1, 1, 20, 16,
  NULL,
  NULL,
};

//--------------------------------------------------------------
/// �^�}�S�쐬�������[�N
//--------------------------------------------------------------
typedef struct
{
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
  
  switch( *seq )
  {
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
    // �L�����Z��
    if( work->select == FLDMENUFUNC_CANCEL ){ (*seq)++; }

    //�u�莝���ɒǉ��v
    if( work->select == STR_TEMOTI )
    {
      POKEMON_PARAM* tamago;
      POKEPARTY* party = GAMEDATA_GetMyPokemon( work->gameData );

      tamago = PP_Create( 1, 1, 1, work->heapID );

      // �e�̖��O
      {
        MYSTATUS* mystatus = GAMEDATA_GetMyStatus( work->gameData );
        STRBUF* name = MyStatus_CreateNameString( mystatus, work->heapID );
        PP_Put( tamago, ID_PARA_oyaname, (u32)name );
        GFL_STR_DeleteBuffer( name );
      } 

      PP_Put( tamago, ID_PARA_tamago_flag, TRUE );
      PP_Renew( tamago );
      PokeParty_Add( party, tamago );
      GFL_HEAP_FreeMemory( tamago );
      (*seq)++;
    }
    break;
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
 * @param heapID
 */
//--------------------------------------------------------------
GMEVENT* DEBUG_EVENT_FLDMENU_MakeEgg( GAMESYS_WORK* gameSystem, HEAPID heapID )
{
  GMEVENT* newEvent;
  EVENT_WORK* work;
  
  newEvent = GMEVENT_Create(
      gameSystem, NULL, debugMenuMakeEggEvent, sizeof(EVENT_WORK) );

  work = GMEVENT_GetEventWork( newEvent );
	work->gameSystem = gameSystem;
  work->gameData   = GAMESYSTEM_GetGameData( gameSystem ); 
	work->heapID     = heapID;
	work->fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );

  return newEvent;
}
