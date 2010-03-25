
#include <gflib.h> 
#include "event_debug_menu_make_egg.h"

#include "field/field_msgbg.h"
#include "print/global_msg.h"
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
  {STR_NEWBORN,     (void*)STR_NEWBORN},      // ���܂ꂽ��
  {STR_OLDBORN,     (void*)STR_OLDBORN},      // ���������z��
  {STR_ILLEGAL_EGG, (void*)STR_ILLEGAL_EGG},  // ���߃^�}�S
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
// proto
//--------------------------------------------------------------
static POKEMON_PARAM* CreateEgg( GAMEDATA* gameData, HEAPID heapID, u32 monsno );


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
    // �^�}�S�쐬
    {
      POKEMON_PARAM* egg = NULL;

      // �쐬
      if( work->select == STR_NEWBORN )
      { //�u���܂ꂽ�āv
        egg = CreateEgg( work->gameData, work->heapID, 1 );
      }
      else if( work->select == STR_OLDBORN )
      { //�u���������z���v
        egg = CreateEgg( work->gameData, work->heapID, 1 );
        PP_Put( egg, ID_PARA_friend, 0 );  // �z������
      }
      else if( work->select == STR_ILLEGAL_EGG )
      { //�u���߃^�}�S�v
        egg = CreateEgg( work->gameData, work->heapID, 1 );
        PP_Put( egg, ID_PARA_fusei_tamago_flag, TRUE );  // �ʖڃ^�}�S�t���O
      }

      // �莝���ɒǉ�
      if( egg )
      {
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

//--------------------------------------------------------------
/**
 * @brief �^�}�S���쐬����
 *
 * @param gameData
 * @param heapID
 * @param monsno   �����X�^�[No.
 */
//--------------------------------------------------------------
static POKEMON_PARAM* CreateEgg( GAMEDATA* gameData, HEAPID heapID, u32 monsno )
{
  POKEMON_PARAM* egg;

  egg = PP_Create( monsno, 1, 1, heapID );

  // �e�̖��O
  {
    MYSTATUS* myStatus;
    STRBUF* name;

    myStatus = GAMEDATA_GetMyStatus( gameData );
    name     = MyStatus_CreateNameString( myStatus, heapID );
    PP_Put( egg, ID_PARA_oyaname, (u32)name );
    GFL_STR_DeleteBuffer( name );
  } 

  // �z������
  {
    u32 monsno, formno, birth;
    POKEMON_PERSONAL_DATA* personal;

    monsno   = PP_Get( egg, ID_PARA_monsno, NULL );
    formno   = PP_Get( egg, ID_PARA_form_no, NULL );
    personal = POKE_PERSONAL_OpenHandle( monsno, formno, heapID );
    birth    = POKE_PERSONAL_GetParam( personal, POKEPER_ID_egg_birth );
    POKE_PERSONAL_CloseHandle( personal );

    // �^�}�S�̊Ԃ�, �Ȃ��x��z���J�E���^�Ƃ��ė��p����
    PP_Put( egg, ID_PARA_friend, birth );
  }

  // �^�}�S�t���O
  PP_Put( egg, ID_PARA_tamago_flag, TRUE );

  // �j�b�N�l�[�� ( �^�}�S )
  {
    STRBUF* name;

    name = GFL_MSG_CreateString( GlobalMsg_PokeName, MONSNO_TAMAGO );
    PP_Put( egg, ID_PARA_nickname, (u32)name );
    GFL_STR_DeleteBuffer( name );
  } 

  PP_Renew( egg );
  return egg;
}
