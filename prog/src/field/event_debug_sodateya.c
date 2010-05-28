///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �f�o�b�O���j���[�w��ĉ��x
 * @file   event_debug_sodateya.c
 * @author obata
 * @date   2010.04.22
 */
///////////////////////////////////////////////////////////////////////////////
#ifdef PM_DEBUG
#include <gflib.h> 
#include "print/global_msg.h" 
#include "field/field_msgbg.h"
#include "fieldmap.h"
#include "event_debug_local.h"
#include "pokemon_egg.h"
#include "sodateya.h"
#include "field/zonedata.h"

#include "arc/message.naix"
#include "msg/msg_debug_sodateya.h"

#include "event_debug_sodateya.h"


//=============================================================================
// ����ĉ� ���j���[�w�b�_
//=============================================================================
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

//=============================================================================
// �����j���[���X�g
//=============================================================================
static const FLDMENUFUNC_LIST menuList[] = 
{
  {sodateya_azukeru,       (void*)sodateya_azukeru},       // ��������
  {sodateya_hikitoru,      (void*)sodateya_hikitoru},      // �Ђ��Ƃ�
  {sodateya_shiiku_1,      (void*)sodateya_shiiku_1},      // ���炷�� ( 100���� )
  {sodateya_shiiku_2,      (void*)sodateya_shiiku_2},      // ���炷�� ( 1������ )
  {sodateya_shiiku_3,      (void*)sodateya_shiiku_3},      // ���炷�� ( 100������ )
  {sodateya_tamago_get,    (void*)sodateya_tamago_get},    // �^�}�S��Ⴄ
  {sodateya_tamago_del,    (void*)sodateya_tamago_del},    // �^�}�S���̂Ă�
  {sodateya_tamago_breed,  (void*)sodateya_tamago_breed},  // �^�}�S�����߂�
  {sodateya_tamago_hatch,  (void*)sodateya_tamago_hatch},  // �^�}�S��z��
  {sodateya_tamago_delete, (void*)sodateya_tamago_delete}, // �^�}�S��z��
};

//=============================================================================
// �����j���[�������\����
//=============================================================================
static const DEBUG_MENU_INITIALIZER menuInitializer = 
{
  NARC_message_debug_sodateya_dat,
  NELEMS(menuList),
  menuList,
  &menuHeader,
  1, 1, 20, 16,
  NULL,
  NULL,
};

//=============================================================================
// ���^�}�S�쐬�C�x���g���[�N
//=============================================================================
typedef struct {

  GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
  FIELDMAP_WORK* fieldmap;
  FLDMENUFUNC*   menuFunc;
  HEAPID         heap_id;
  u32            select;

} EVENT_WORK;


//=============================================================================
// ��prototype
//=============================================================================
static GMEVENT_RESULT DebugMenuSodateyaEvent( GMEVENT *event, int *seq, void *wk );
static void DebugSodateya_Azukeru( EVENT_WORK* work ); // �擪�|�P��������ĉ��ɗa����
static void DebugSodateya_Hikitoru( EVENT_WORK* work ); // ��ĉ��̐擪�|�P�������������
static void DebugSodateya_Shiiku( EVENT_WORK* work, u32 time ); // �a�����|�P���������炷��
static void DebugSodateya_TamagoGet( EVENT_WORK* work ); // �^�}�S��Ⴄ
static void DebugSodateya_TamagoDel( EVENT_WORK* work ); // �^�}�S���̂Ă�
static void DebugSodateya_TamagoBreed( EVENT_WORK* work ); // �^�}�S�����߂�
static void DebugSodateya_TamagoHatch( EVENT_WORK* work ); // �^�}�S��z��
static void DebugSodateya_DeleteTailPoke( EVENT_WORK* work ); // �����|�P�������폜����


//=============================================================================
// ��public functions
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief ��ĉ��f�o�b�O���j���[�C�x���g�𐶐�����
 *
 * @parma gameSystem
 * @param wk         FIELDMAP_WORK�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_DebugMenuSodateya( GAMESYS_WORK* gameSystem, void* wk )
{
  GMEVENT* event;
  EVENT_WORK* work;
  FIELDMAP_WORK* fieldmap = (FIELDMAP_WORK*)wk;
  
  event = GMEVENT_Create(
      gameSystem, NULL, DebugMenuSodateyaEvent, sizeof(EVENT_WORK) );

  work = GMEVENT_GetEventWork( event );
	work->gameSystem = gameSystem;
  work->gameData   = GAMESYSTEM_GetGameData( gameSystem ); 
	work->heap_id    = FIELDMAP_GetHeapID( fieldmap );
	work->fieldmap   = fieldmap;

  return event;
}


//=============================================================================
// ��private functions
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief ��ĉ��f�o�b�O���j���[�C�x���g
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT DebugMenuSodateyaEvent( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK* work = wk;
  
  switch( *seq ) {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( 
        work->fieldmap, work->heap_id,  &menuInitializer );
    (*seq)++;
    break;

  case 1:
    work->select = FLDMENUFUNC_ProcMenu( work->menuFunc );

    if( work->select != FLDMENUFUNC_NULL ) {
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      (*seq)++; 
    }
    break; 

  case 2: 
    switch( work->select ) {
    case sodateya_azukeru:       DebugSodateya_Azukeru( work );          break;
    case sodateya_hikitoru:      DebugSodateya_Hikitoru( work );         break;
    case sodateya_shiiku_1:      DebugSodateya_Shiiku( work, 100 );      break;
    case sodateya_shiiku_2:      DebugSodateya_Shiiku( work, 10000 );    break;
    case sodateya_shiiku_3:      DebugSodateya_Shiiku( work, 1000000 );  break;
    case sodateya_tamago_get:    DebugSodateya_TamagoGet( work );        break;
    case sodateya_tamago_del:    DebugSodateya_TamagoDel( work );        break;
    case sodateya_tamago_breed:  DebugSodateya_TamagoBreed( work );      break;
    case sodateya_tamago_hatch:  DebugSodateya_TamagoHatch( work );      break;
    case sodateya_tamago_delete: DebugSodateya_DeleteTailPoke( work );   break;
    } 

    // �L�����Z���ŏI��
    if( work->select == FLDMENUFUNC_CANCEL ) { 
      return GMEVENT_RES_FINISH;
    }

    // ���[�v����
    *seq = 0;
    break;
  } 
  return( GMEVENT_RES_CONTINUE );
}


//-----------------------------------------------------------------------------
/**
 * @brief �擪�|�P��������ĉ��ɗa����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DebugSodateya_Azukeru( EVENT_WORK* work )
{
  SODATEYA*  sodateya   = FIELDMAP_GetSodateya( work->fieldmap );
  POKEPARTY* party      = GAMEDATA_GetMyPokemon( work->gameData );
  int        poke_num   = PokeParty_GetPokeCount( party );
  int        shiiku_num = SODATEYA_GetPokemonNum( sodateya );

  // ����ȏ�a�����Ȃ�
  if( 2 <= shiiku_num ) {
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_BEEP );
  }
  // �a������
  else {
    SODATEYA_TakePokemon( sodateya, party, 0 );
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_DECIDE2 );
  }
}
//-----------------------------------------------------------------------------
/**
 * @brief ��ĉ��̐擪�|�P�������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DebugSodateya_Hikitoru( EVENT_WORK* work )
{
  POKEPARTY* party      = GAMEDATA_GetMyPokemon( work->gameData );
  SODATEYA*  sodateya   = FIELDMAP_GetSodateya( work->fieldmap );
  int        shiiku_num = SODATEYA_GetPokemonNum( sodateya );

  // �a���Ă��Ȃ�
  if( shiiku_num == 0 ) {
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_BEEP );
  }
  // �a���Ă���
  else {
    SODATEYA_TakeBackPokemon( sodateya, 0, party );
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_DECIDE2 );
  }
}
//-----------------------------------------------------------------------------
/**
 * @brief �a�����|�P���������炷��
 *
 * @param work
 * @param time ���炷��� ( ��������Ă邩 )
 */
//-----------------------------------------------------------------------------
static void DebugSodateya_Shiiku( EVENT_WORK* work, u32 time )
{
  u32 count;
  BOOL egg = FALSE;
  SODATEYA* sodateya = FIELDMAP_GetSodateya( work->fieldmap );
  
  for( count=0; count<time; count++ )
  {
    if( SODATEYA_BreedPokemon(sodateya) ) {
      egg = TRUE; // �^�}�S���Y�܂ꂽ!!
    }
  }

  // ����!!
  PMSND_StopSE();
  if( egg ) {
    PMSND_PlaySE( SEQ_SE_DECIDE4 );
  }
  else {
    PMSND_PlaySE( SEQ_SE_EXPMAX );
  }
}
//-----------------------------------------------------------------------------
/**
 * @brief �^�}�S��Ⴄ
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DebugSodateya_TamagoGet( EVENT_WORK* work )
{
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( work->gameData );
  SODATEYA*  sodateya = FIELDMAP_GetSodateya( work->fieldmap );

  // �^�}�S������ꍇ
  if( SODATEYA_HaveEgg(sodateya) ) {
    SODATEYA_TakeBackEgg( sodateya, party );
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_DECIDE2 );
  }
  // �^�}�S���Ȃ��ꍇ
  else {
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_BEEP );
  }
}
//-----------------------------------------------------------------------------
/**
 * @brief �^�}�S���̂Ă�
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DebugSodateya_TamagoDel( EVENT_WORK* work )
{
  SODATEYA* sodateya = FIELDMAP_GetSodateya( work->fieldmap );

  // �^�}�S������ꍇ
  if( SODATEYA_HaveEgg(sodateya) ) {
    SODATEYA_DeleteEgg( sodateya );
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_DECIDE2 );
  }
  // �^�}�S���Ȃ��ꍇ
  else {
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_BEEP );
  }
} 
//-----------------------------------------------------------------------------
/**
 * @brief �^�}�S�����߂�
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DebugSodateya_TamagoBreed( EVENT_WORK* work )
{
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( work->gameData );
  int        poke_num = PokeParty_GetPokeCount( party );

  int i;

  // �莝���ɂ���S�Ẵ^�}�S��z�����O�̏�Ԃɂ���
  for( i=0; i<poke_num; i++ )
  {
    POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, i );
    u32 tamago_flag = PP_Get( pp, ID_PARA_tamago_flag, NULL );
    if( tamago_flag ) {
      PP_Put( pp, ID_PARA_friend, 0 );  // �z������
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �^�}�S��z��
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DebugSodateya_TamagoHatch( EVENT_WORK* work )
{
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( work->gameData );
  int        poke_num = PokeParty_GetPokeCount( party );
  MYSTATUS*  owner    = GAMEDATA_GetMyStatus( work->gameData );
  u32        zone_id  = FIELDMAP_GetZoneID( work->fieldmap );
  u32        place_id = ZONEDATA_GetPlaceNameID( zone_id );

  int i;

  // �莝���ɂ���S�Ẵ^�}�S��z��������
  for( i=0; i<poke_num; i++ )
  {
    POKEMON_PARAM* poke = PokeParty_GetMemberPointer( party, i );
    u32 tamago_flag = PP_Get( poke, ID_PARA_tamago_flag, NULL );
    if( tamago_flag ) {
      PP_Birth( poke, owner, place_id, work->heap_id );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �����|�P�������폜����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DebugSodateya_DeleteTailPoke( EVENT_WORK* work )
{
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( work->gameData );
  int        poke_num = PokeParty_GetPokeCount( party );

  if( 2 <= poke_num ) {
    PokeParty_Delete( party, poke_num - 1 );
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_DECIDE2 );
  }
  else {
    PMSND_StopSE();
    PMSND_PlaySE( SEQ_SE_BEEP );
  }
}

#endif // PM_DEBUG
