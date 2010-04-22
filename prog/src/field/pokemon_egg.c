//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �|�P�����^�}�S
 * @file   pokemon_egg.c
 * @author obata
 * @date   2010.04.21
 */
////////////////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h> 
#include "gamesystem/game_data.h"
#include "savedata/mystatus.h"
#include "system/gfl_use.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/poke_personal.h"
#include "poke_tool/poke_memo.h"
#include "item/itemsym.h"         // for ITEM_xxxx
#include "item/item.h"            // for ITEM_xxxx
#include "waza_tool/wazano_def.h" // for WAZANO_xxxx
#include "print/global_msg.h"     // for GlobalMsg_PokeName 
#include "arc/arc_def.h"          // for ARCID_PMSTBL

#include "pokemon_egg.h"


//========================================================================================
// �� �萔
//========================================================================================
#define RARE_EGG_CHANCE (5) // ���A�^�}�S���I��


//========================================================================================
// ��prototype
//========================================================================================
// �^�}�S�p�����[�^�̌���
static void EggCordinate_monsno( const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg ); // �����X�^�[No.
static void EggCordinate_seikaku( const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg ); // ���i
static void EggCordinate_tokusei( HEAPID heap_id, const POKEMON_PARAM* mother, POKEMON_PARAM* egg ); // ����
static void EggCordinate_ability_rand( const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg ); // �̗���
static void EggCordinate_rare( const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg ); // ���A���I
static void EggCordinate_waza_default( POKEMON_PARAM* egg ); // �f�t�H���g�Z�Z�b�g
static void EggCordinate_waza_egg( HEAPID heap_id, const POKEMON_PARAM* father, POKEMON_PARAM* egg ); // �^�}�S�Z�Z�b�g
static void EggCordinate_waza_parent( const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg ); // �p���Z ( ���e ) �Z�b�g
static void EggCordinate_waza_machine( HEAPID heap_id, const POKEMON_PARAM* father, POKEMON_PARAM* egg ); // �p���Z ( �Z�}�V�� ) �Z�b�g
static void EggCordinate_pityuu( const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg ); // ����^�}�S���� ( �s�`���[ )
static void EggCordinate_karanakusi( const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg ); // ����^�}�S���� ( �J���i�N�V )
static void EggCordinate_finish( POKEMON_PARAM* egg, const MYSTATUS* mystatus, int memo, HEAPID heap_id );
// �^�}�S�z��
static void EggBirth( POKEMON_PARAM* egg, const MYSTATUS* owner, HEAPID heap_id ); // �^�}�S��z��������
// ���[�e�B���e�B
static BOOL IsWazaMachineAbleToUse( HEAPID heap_id, u16 monsno, u16 formno, u16 itemno ); // �Z�}�V�����g���邩�ǂ������`�F�b�N����
static u16 GetSeedMonsNo( u16 monsno ); // ��|�P�������擾����
// �f�o�b�O
#ifdef PM_DEBUG
static POKEMON_PARAM* CreateDebugEgg( GAMEDATA* gameData, HEAPID heapID, u32 monsno ); // �f�o�b�O�p�^�}�S���쐬����
#endif


//========================================================================================
// ��public functions
//========================================================================================

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S���쐬����
 *
 * @param heap_id  �g�p����q�[�vID
 * @param mystatus �^�}�S�̏��L��
 * @param memo     �g���[�i�[�����̋L��
 * @param father   ���|�P����
 * @param mother   ��|�P����
 * @param egg      �쐬�����^�}�S�̊i�[��
 */
//---------------------------------------------------------------------------------------- 
void POKEMON_EGG_Create( 
    HEAPID heap_id, const MYSTATUS* mystatus, int memo, 
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  // �^�}�S�̃p�����[�^��ݒ�
  EggCordinate_monsno( father, mother, egg );          // �����X�^�[�i���o�[
  EggCordinate_seikaku( father, mother, egg );         // ���i
  EggCordinate_tokusei( heap_id, mother, egg );        // ����
  EggCordinate_ability_rand( father, mother, egg );    // �̗���
  EggCordinate_rare( father, mother, egg );            // ���A�|�P���I
  EggCordinate_waza_default( egg );                    // �Z ( �f�t�H���g )
  EggCordinate_waza_egg( heap_id, father, egg );       // �Z ( �^�}�S�Z )
  EggCordinate_waza_parent( father, mother, egg );     // �Z ( ���e���ʂ̋Z )
  EggCordinate_waza_machine( heap_id, father, egg );   // �Z ( ���̃}�V���Z )
  EggCordinate_pityuu( father, mother, egg );          // ����^�}�S ( �s�`���[ )
  EggCordinate_karanakusi( father, mother, egg );      // ����^�}�S ( �J���i�N�V )
  EggCordinate_finish( egg, mystatus, memo, heap_id ); // �d�グ
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S��z��������
 *
 * @param egg     �z��������^�}�S
 * @param owner   �^�}�S�̏�����
 * @param heap_id �g�p����q�[�vID
 */
//---------------------------------------------------------------------------------------- 
void POKEMON_EGG_Birth( POKEMON_PARAM* egg, const MYSTATUS* owner, HEAPID heap_id )
{
  EggBirth( egg, owner, heap_id );
}


//========================================================================================
// ��private functions
//========================================================================================

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃����X�^�[�i���o�[�����肷��
 *
 * @param father ���e�|�P����
 * @param mother ��e�|�P����
 * @param egg    �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_monsno( 
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  int i;
  u32 monsno_egg;
  u32 monsno_father = PP_Get( father, ID_PARA_monsno, NULL );
  u32 monsno_mother = PP_Get( mother, ID_PARA_monsno, NULL );
  u32 itemno_mother = PP_Get( mother, ID_PARA_item, NULL );
  const int table_size = 9;
  u32 exception_table[table_size][3] = // ��O�e�[�u��
  {
    // ��|�P����      // �A�C�e��          // �q�|�P����
    {MONSNO_MARIRU,    ITEM_USIONOOKOU,     MONSNO_MARIRU},
    {MONSNO_SOONANSU,  ITEM_NONKINOOKOU,    MONSNO_SOONANO},
    {MONSNO_BARIYAADO, ITEM_AYASIIOKOU,     MONSNO_MANENE},
    {MONSNO_USOKKII,   ITEM_GANSEKIOKOU,    MONSNO_USOHATI},
    {MONSNO_KABIGON,   ITEM_MANPUKUOKOU,    MONSNO_GONBE},
    {MONSNO_MANTAIN,   ITEM_SAZANAMINOOKOU, MONSNO_TAMANTA},
    {MONSNO_ROZERIA,   ITEM_OHANANOOKOU,    MONSNO_SUBOMII},
    {MONSNO_RAKKII,    ITEM_KOUUNNOOKOU,    MONSNO_PINPUKU},
    {MONSNO_TIRIIN,    ITEM_KIYOMENOOKOU,   MONSNO_RIISYAN},
  };
  
  // �ʏ�͕�e�̎�|�P����
  monsno_egg = GetSeedMonsNo( monsno_mother );

  // ��e�����^�����Ȃ�, ���e�̎�|�P�������Y�܂��
  if( monsno_mother == MONSNO_METAMON ) 
  {
    monsno_egg = GetSeedMonsNo( monsno_father );
  }

  // ��e���}�i�t�B�Ȃ�, �t�B�I�l���Y�܂��
  if( monsno_mother == MONSNO_MANAFI )
  {
    monsno_egg = MONSNO_FIONE;
  }

  // ��O�e�[�u���K�p
  for( i=0; i<table_size; i++ )
  {
    // ����̕�|�P������A�C�e���������Ă�����, �q�|�P�������ω�
    if( ( monsno_mother == exception_table[i][0] ) &&
        ( itemno_mother == exception_table[i][1] ) )
    {
      monsno_egg = exception_table[i][2];
      break;
    }
  }

  // �|�P�����p�����[�^�\�z
  PP_SetupEx( egg, monsno_egg, 1, 
      PTL_SETUP_ID_AUTO, PTL_SETUP_POW_AUTO, PTL_SETUP_RND_AUTO );

  // DEBUG:
  OBATA_Printf( 
      "EggCordinate_monsno: father=%d, mother=%d, egg=%d\n", 
      monsno_father, monsno_mother, monsno_egg );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̐��i�����肷��
 *
 * @param father ���e�|�P����
 * @param mother ��e�|�P����
 * @param egg    �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_seikaku(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  BOOL kawarazu_f; // ���e���w����炸�̂����x�������Ă��邩�ǂ���
  BOOL kawarazu_m; // ��e���w����炸�̂����x�������Ă��邩�ǂ���
  u32 seikaku_f = PP_Get( father, ID_PARA_seikaku, NULL ); // ���e�̐��i
  u32 seikaku_m = PP_Get( mother, ID_PARA_seikaku, NULL ); // ��e�̐��i

  //�w����炸�̂����x�������Ă��邩�ǂ������擾
  kawarazu_f = ( PP_Get( father, ID_PARA_item, NULL ) == ITEM_KAWARAZUNOISI );
  kawarazu_m = ( PP_Get( mother, ID_PARA_item, NULL ) == ITEM_KAWARAZUNOISI );

  // ���e�Ƃ������Ă���
  if( kawarazu_f && kawarazu_m ) {
    // 1/2�Ōp������
    if( GFUser_GetPublicRand0(2) == 0 ) {
      // ���e����p��
      if( GFUser_GetPublicRand0(2) == 0 )  {
        PP_Put( egg, ID_PARA_seikaku, seikaku_f );
      }
      // ��e����p��
      else {
        PP_Put( egg, ID_PARA_seikaku, seikaku_m );
      }
    }
  }
  // ���e�̂ݎ����Ă���
  else if( kawarazu_f ) {
    // 1/2�Ōp������
    if( GFUser_GetPublicRand0(2) == 0 ) {
      PP_Put( egg, ID_PARA_seikaku, seikaku_f );
    }
  }
  // ��e�̂ݎ����Ă���
  else if( kawarazu_m ) {
    // 1/2�Ōp������
    if( GFUser_GetPublicRand0(2) == 0 ) {
      PP_Put( egg, ID_PARA_seikaku, seikaku_m );
    }
  }
  // ���e�Ƃ������Ă��Ȃ�
  else {
    // �����Ō���
    PP_Put( egg, ID_PARA_seikaku, GFUser_GetPublicRand0( PTL_SEIKAKU_MAX ) );
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̓��������肷�� ( ��e�̓�������`���� )
 *
 * @param heap_id �g�p����q�[�vID
 * @param mother  ��e�|�P����
 * @param egg     �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_tokusei(
    HEAPID heap_id, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  u32 rnd;
  u32 monsno, formno; // ��̃p�����[�^
  u32 speabi_egg;     // �q�̓���
  u32 speabi_mother;  // ��̓���
  u32 speabi_index;   // ��̓����͓������Ԃ��H
  u32 speabi_list[3]; // �������X�g
  POKEMON_PERSONAL_DATA* ppd;

  // ��e�̓������擾
  speabi_mother = PP_Get( mother, ID_PARA_speabino, NULL );

  // �������X�g(��̓���)���擾
  monsno         = PP_Get( mother, ID_PARA_monsno, NULL );
  formno         = PP_Get( mother, ID_PARA_form_no, NULL );
  ppd            = POKE_PERSONAL_OpenHandle( monsno, formno, heap_id );
  speabi_list[0] = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi1 );
  speabi_list[1] = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi2 );
  speabi_list[2] = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi3 );
  POKE_PERSONAL_CloseHandle( ppd );
  
  // ��e�̓����ԍ��𒲂ׂ�
  for( speabi_index=0; speabi_index<3; speabi_index++ )
  {
    if( speabi_mother == speabi_list[speabi_index] ) break;
  }
  if( 3 <= speabi_index )
  { // ��̓�����, ��̓����Ɉ�v���Ȃ��ꍇ
    OBATA_Printf( "��|�P�����̓����Ɍ�肪����܂��B\n" );
    speabi_index = 0; // �Ƃ肠����, ����1���p�����邱�Ƃɂ���
  }

  // �p���������������
  rnd = GFUser_GetPublicRand0(100);  // �����擾[0, 99]
  switch( speabi_index )
  {
  // �ꂪ����1
  case 0:
    if( rnd < 80 ) speabi_egg = speabi_list[0]; // 80% ==> �q�̓���1
    else           speabi_egg = speabi_list[1]; // 20% ==> �q�̓���2
    break;
  // �ꂪ����2
  case 1: 
    if( rnd < 20 ) speabi_egg = speabi_list[0]; // 20% ==> �q�̓���1
    else           speabi_egg = speabi_list[1]; // 80% ==> �q�̓���2
    break;
  // �ꂪ����3
  case 2: 
    if( rnd < 20 )      speabi_egg = speabi_list[0]; // 20% ==> �q�̓���1
    else if( rnd < 40 ) speabi_egg = speabi_list[1]; // 20% ==> �q�̓���2
    else                speabi_egg = speabi_list[2]; // 60% ==> �q�̓���3
    break;
  }

  // �������p��
  PP_Put( egg, ID_PARA_speabino, speabi_egg );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̌̗��������肷��
 *
 * @param father ���e�|�P����
 * @param mother ��e�|�P����
 * @param egg    �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_ability_rand(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  int i=0, j=0;
  u32 itemno;
  u32 ability[3];   // �p������̗���

  // ���e���w�p���[XXXX�x�������Ă���ꍇ
  itemno = PP_Get( father, ID_PARA_item, NULL );
  switch( itemno )
  {
  case ITEM_PAWAAUEITO:  ability[i++] = PTL_ABILITY_HP;    break;
  case ITEM_PAWAARISUTO: ability[i++] = PTL_ABILITY_ATK;   break;
  case ITEM_PAWAABERUTO: ability[i++] = PTL_ABILITY_DEF;   break;
  case ITEM_PAWAARENZU:  ability[i++] = PTL_ABILITY_SPATK; break;
  case ITEM_PAWAABANDO:  ability[i++] = PTL_ABILITY_SPDEF; break;
  case ITEM_PAWAAANKURU: ability[i++] = PTL_ABILITY_AGI;   break;
  }

  // ��e���w�p���[XXXX�x�������Ă���ꍇ
  if( ( i == 0 ) ||
      ( GFUser_GetPublicRand0(2) == 0 ) )    // ���e�������Ă����ꍇ, 1/2�ŏ㏑������
  {
    i = 0;
    itemno = PP_Get( father, ID_PARA_item, NULL );
    switch( itemno )
    {
    case ITEM_PAWAAUEITO:  ability[i++] = PTL_ABILITY_HP;    break;
    case ITEM_PAWAARISUTO: ability[i++] = PTL_ABILITY_ATK;   break;
    case ITEM_PAWAABERUTO: ability[i++] = PTL_ABILITY_DEF;   break;
    case ITEM_PAWAARENZU:  ability[i++] = PTL_ABILITY_SPATK; break;
    case ITEM_PAWAABANDO:  ability[i++] = PTL_ABILITY_SPDEF; break;
    case ITEM_PAWAAANKURU: ability[i++] = PTL_ABILITY_AGI;   break;
    }
  }

  // �p������̗����������_���ɑI��
  for( ; i<3; i++ )
  {
    // �����_���ɑI��
    ability[i++] = GFUser_GetPublicRand0(PTL_ABILITY_MAX);

    // ���łɑI�΂�Ă�����, �I�ђ���
    for( j=0; j<i; j++ )
    {
      if( ability[i] == ability[j] ) i--;
    }
  }

  // �I�񂾌̗������p������ 
  for( i=0; i<3; i++ )
  {
    const POKEMON_PARAM* pp;
    int id;
    u32 arg;

    // ���E��̂ǂ��炩��󂯌p���̂�������
    if( GFUser_GetPublicRand0(2) == 0 ) pp = father;
    else                             pp = mother;

    // �p��
    switch( ability[i] )
    {
    case PTL_ABILITY_HP:    id = ID_PARA_hp_rnd;     break;
    case PTL_ABILITY_ATK:   id = ID_PARA_pow_rnd;    break;
    case PTL_ABILITY_DEF:   id = ID_PARA_def_rnd;    break;
    case PTL_ABILITY_SPATK: id = ID_PARA_spepow_rnd; break;
    case PTL_ABILITY_SPDEF: id = ID_PARA_spedef_rnd; break;
    case PTL_ABILITY_AGI:   id = ID_PARA_agi_rnd;    break;
    }
    arg = PP_Get( pp, id, NULL );
    PP_Put( egg, id, arg );
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ���A�|�P�����̒��I���s��
 *
 * @param father ���e�|�P����
 * @param mother ��e�|�P����
 * @param egg    �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_rare(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  int i;
  u32 id, rnd;
  u32 country_father = PP_Get( father, ID_PARA_country_code, NULL );
  u32 country_mother = PP_Get( mother, ID_PARA_country_code, NULL );

  // �e���m�̍��R�[�h�������Ȃ�, �������Ȃ�
  if( country_father == country_mother ) return;

  // ���łɃ��A�Ȃ�, �������Ȃ�
  id  = PP_Get( egg, ID_PARA_id_no, NULL );
  rnd = PP_Get( egg, ID_PARA_personal_rnd, NULL );
  if( POKETOOL_CheckRare( id, rnd ) ) return;

  // �ő�RARE_EGG_CHANCE��̒��I���s��
  for( i=0; i<RARE_EGG_CHANCE; i++ )
  {
    rnd = GFUser_GetPublicRand0( 0xffffffff );
    if( POKETOOL_CheckRare( id, rnd ) ) {
      // ���A���o���璊�I�I���Ƃ��A������������������
      PP_Put( egg, ID_PARA_personal_rnd, rnd );
      return;
    }
  }

  // ���A���o�Ȃ������̂ł���Ό������͏��������Ȃ��i����炸�̂����Ή��j
  
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�Ƀf�t�H���g�Z���o��������
 *
 * @param egg     �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_waza_default( POKEMON_PARAM* egg )
{
  int i;
  u32 monsno, formno, level;
  POKEPER_WAZAOBOE_CODE waza_table[POKEPER_WAZAOBOE_TABLE_ELEMS];

  // ��{�����擾
  monsno = PP_Get( egg, ID_PARA_monsno, NULL );
  formno = PP_Get( egg, ID_PARA_form_no, NULL );
  level  = PP_Get( egg, ID_PARA_level, NULL );

  // Lv.1�Ŋo����Z���Z�b�g
  POKE_PERSONAL_LoadWazaOboeTable( monsno, formno, waza_table );
  i = 0;
  while( POKEPER_WAZAOBOE_IsEndCode( waza_table[i] ) )
  {
    int waza_lv = POKEPER_WAZAOBOE_GetLevel( waza_table[i] );
    int waza_id = POKEPER_WAZAOBOE_GetWazaID( waza_table[i] );
    if( level < waza_lv ) break;
    else PP_SetWazaPush( egg, waza_id );
    i++;
  } 
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�Z���o��������
 *        �^�}�S����z��|�P�����݂̂��o������Z�̂���, ���|�P�������o���Ă���Z���K��
 *
 * @param heap_id �g�p����q�[�vID 
 * @param father  ���e�|�P����
 * @param egg     �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_waza_egg( 
    HEAPID heap_id, const POKEMON_PARAM* father, POKEMON_PARAM* egg )
{
  int i, j;
  u32 monsno = PP_Get( egg, ID_PARA_monsno, NULL );
  u32 id_para[] = { ID_PARA_waza1, ID_PARA_waza2, ID_PARA_waza3, ID_PARA_waza4 };
  u16 kowaza_num;
  u16* kowaza;

  // �q�Z�e�[�u�����擾
  kowaza = GFL_ARC_LoadDataAlloc( ARCID_KOWAZA, monsno, heap_id );
  kowaza_num = kowaza[0];

  // ���e�̑S�Z���`�F�b�N����
  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    u32 wazano = PP_Get( father, id_para[i], NULL );

    // ���e�̋Z���^�}�S�Z�Ȃ�, �K������
    for( j=1; j<=kowaza_num; j++ )
    {
      if( wazano == kowaza[j] )
      {
        PP_SetWazaPush( egg, wazano );
        break;
      }
    }
  }

  // �q�Z�e�[�u���j��
  GFL_HEAP_FreeMemory( kowaza );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ���e�̋Z���󂯌p��
 *        ���������x�����グ�邱�ƂŊo����Z�̂���, ���e�Ƃ��Ɋo���Ă���Z���K��
 *
 * @param father ���e�|�P����
 * @param mother ��e�|�P����
 * @param egg    �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_waza_parent(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  int ifa, imo;
  u32 id_para[] = { ID_PARA_waza1, ID_PARA_waza2, ID_PARA_waza3, ID_PARA_waza4 };

  // ���̑S�Z���`�F�b�N
  for( ifa=0; ifa<PTL_WAZA_MAX; ifa++ )
  {
    // �Z�R�[�h�擾
    u32 wazano = PP_Get( father, id_para[ifa], NULL );

    // ����o���Ă��邩�ǂ������`�F�b�N
    for( imo=0; imo<PTL_WAZA_MAX; imo++ )
    {
      // ����o���Ă�����, �p������
      if( wazano == PP_Get( mother, id_para[imo], NULL ) )
      {
        PP_SetWazaPush( egg, wazano );
        break;
      }
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ���|�P�����̃}�V���Z���󂯌p��
 *        �������o���邱�Ƃ̏o����Z�}�V���̂���, ���|�P�������o���Ă���Z���K��
 *
 * @param heap_id �g�p����q�[�vID 
 * @param father  ���e�|�P����
 * @param egg     �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_waza_machine(
    HEAPID heap_id, const POKEMON_PARAM* father, POKEMON_PARAM* egg )
{
  int i, itemno;
  u32 wazano;
  u32 monsno_egg = PP_Get( egg, ID_PARA_monsno, NULL );
  u32 formno_egg = PP_Get( egg, ID_PARA_form_no, NULL );
  u32 id_para[] = { ID_PARA_waza1, ID_PARA_waza2, ID_PARA_waza3, ID_PARA_waza4 };

  // ���̑S�Z���`�F�b�N
  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    // �Z�R�[�h�擾
    wazano = PP_Get( father, id_para[i], NULL );

    // ���Z�̋Z�}�V�������݂��邩�ǂ����`�F�b�N
    for( itemno=ITEM_WAZAMASIN01; itemno<=ITEM_DATA_MAX; itemno++ )
    {
      // �Z�}�V��������, �^�}�S�|�P�����Ɏg�p�ł���Ȃ�, �K������
      if( (wazano == ITEM_GetWazaNo( itemno )) &&
          (IsWazaMachineAbleToUse( heap_id, monsno_egg, formno_egg, itemno)) )
      {
        PP_SetWazaPush( egg, wazano );
        break;
      }
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ����^�}�S�쐬���� (�s�`���[)
 *        �e�|�P���w�ł񂫂��܁x�������Ă�����,�w�{���e�b�J�[�x���K��
 *
 * @param father ���e�|�P����
 * @param mother ��e�|�P����
 * @param egg    �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_pityuu(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  // �s�`���[�̃^�}�S�łȂ���Ή������Ȃ�
  if( PP_Get( egg, ID_PARA_monsno, NULL ) != MONSNO_PITYUU ) return;

  // ���e�̂ǂ��炩���w�ł񂫂��܁x�������Ă�����,�w�{���e�b�J�[�x���K��
  if( ( PP_Get( father, ID_PARA_item, NULL ) == ITEM_DENKIDAMA ) ||
      ( PP_Get( mother, ID_PARA_item, NULL ) == ITEM_DENKIDAMA ) )
  {
    PP_SetWazaPush( egg, WAZANO_BORUTEKKAA );
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ����^�}�S�쐬���� (�J���i�N�V)
 *        ��e���w�J���i�N�V�xor�w�g���g�h���x�Ȃ�, �t�H�������p��
 *
 * @param father ���e�|�P����
 * @param mother ��e�|�P����
 * @param egg    �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_karanakusi(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  u32 monsno_mother = PP_Get( mother, ID_PARA_monsno, NULL );

  // ��e���w�J���i�N�V�xor�w�g���g�h���x�Ȃ�, �t�H�������p��
  if( (monsno_mother == MONSNO_KARANAKUSI) ||
      (monsno_mother == MONSNO_TORITODON) )
  {
    u32 formno = PP_Get( mother, ID_PARA_form_no, NULL );
    PP_Put( egg, ID_PARA_form_no, formno );
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�쐬�d�グ����
 *
 * @param egg      �ݒ肷��^�}�S
 * @param mystatus �^�}�S�̏��L��
 * @param memo     �g���[�i�[�����w��
 * @param heap_id  �q�[�vID
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_finish( 
    POKEMON_PARAM* egg, const MYSTATUS* mystatus, int memo, HEAPID heap_id )
{
  // �Čv�Z
  PP_Renew( egg );

  // ID
  {
    u32 id = MyStatus_GetID( mystatus );
    PP_Put( egg, ID_PARA_id_no, id );
    OBATA_Printf( "EggCordinate_finish: id = %d\n", id );
  }

  // �e�̖��O
  {
    STRBUF* name = MyStatus_CreateNameString( mystatus, heap_id );
    PP_Put( egg, ID_PARA_oyaname, (u32)name );
    GFL_STR_DeleteBuffer( name );
  } 

  // �j�b�N�l�[�� ( �^�}�S )
  {
    STRBUF* name;

    name = GFL_MSG_CreateString( GlobalMsg_PokeName, MONSNO_TAMAGO );
    PP_Put( egg, ID_PARA_nickname, (u32)name );
    GFL_STR_DeleteBuffer( name );
  } 

  // �z������
  {
    u32 monsno, formno, birth;
    POKEMON_PERSONAL_DATA* personal;

    // �|�P�������Ƃ̛z���������擾
    monsno   = PP_Get( egg, ID_PARA_monsno, NULL );
    formno   = PP_Get( egg, ID_PARA_form_no, NULL );
    personal = POKE_PERSONAL_OpenHandle( monsno, formno, heap_id );
    birth    = POKE_PERSONAL_GetParam( personal, POKEPER_ID_egg_birth );
    POKE_PERSONAL_CloseHandle( personal );

    // �^�}�S�̊Ԃ�, �Ȃ��x��z���J�E���^�Ƃ��ė��p����
    PP_Put( egg, ID_PARA_friend, birth );
  }

  // �g���[�i�[����
  {
    POKE_MEMO_SetTrainerMemoPP( egg, POKE_MEMO_EGG_FIRST, mystatus, memo, heap_id );
  }

  // �^�}�S�t���O
  PP_Put( egg, ID_PARA_tamago_flag, TRUE );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S��z��������
 *
 * @param egg     �z��������^�}�S
 * @param owner   �^�}�S�̏�����
 * @param heap_id �g�p����q�[�vID
 */
//---------------------------------------------------------------------------------------- 
static void EggBirth( POKEMON_PARAM* egg, const MYSTATUS* owner, HEAPID heap_id )
{ 
  // �^�}�S�t���O
  PP_Put( egg, ID_PARA_tamago_flag, FALSE );

  // �Ȃ��x ==> �p�[�\�i���̏����l��ݒ�
  {
    u32 monsno, formno, friend;
    POKEMON_PERSONAL_DATA* personal;
    monsno   = PP_Get( egg, ID_PARA_monsno, NULL );
    formno   = PP_Get( egg, ID_PARA_form_no, NULL );
    personal = POKE_PERSONAL_OpenHandle( monsno, formno, heap_id );
    friend   = POKE_PERSONAL_GetParam( personal, POKEPER_ID_friend );
    PP_Put( egg, ID_PARA_friend, friend );
    POKE_PERSONAL_CloseHandle( personal );
  }

  // ID ==> �^�}�S�����҂�ID��ݒ�
  {
    u32 id = MyStatus_GetID( owner );
    PP_Put( egg, ID_PARA_id_no, id );
  } 
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �Z�}�V�����g�p�ł��邩�ǂ����𔻒肷��
 *
 * @param heap_id �g�p����q�[�vID
 * @param monsno  �Z�}�V���g�p�Ώۃ|�P�����̃����X�^�[�i���o�[
 * @param formno  �����X�^�[�̃t�H�[���i���o�[
 * @param itemno  �g�p����Z�}�V���̃A�C�e���i���o�[
 * @return �Z�}�V�����g�p�ł���ꍇ TRUE
 */
//---------------------------------------------------------------------------------------- 
static BOOL IsWazaMachineAbleToUse( HEAPID heap_id, u16 monsno, u16 formno, u16 itemno )
{
  u16 seed_monsno;
  u32 bit_pos;
  u32 flag;
  u8 machine_no;
  POKEMON_PERSONAL_DATA* ppd;

  // �����X�^�[�i���o�[������
  if( MONSNO_END <= monsno )
  {
    OBATA_Printf( "IsWazaMachineAbleToUse: �����ȃ����X�^�[�i���o�[���w�肳��܂����B\n" );
    return FALSE;
  } 
  // �Z�}�V���ȊO�̃A�C�e�����w�肳�ꂽ�ꍇ
//  if( (itemno < ITEM_WAZAMASIN01 ) || (ITEM_HIDENMASIN08 < itemno ) )
	if( ITEM_CheckWazaMachine( itemno ) == FALSE )
  {
    OBATA_Printf( "IsWazaMachineAbleToUse: �Z�}�V���ȊO�̃A�C�e�����w�肳��܂����B\n" );
    return FALSE;
  } 

  // �p�[�\�i���f�[�^���I�[�v��
  ppd = POKE_PERSONAL_OpenHandle( monsno, formno, heap_id );

  // �Z�}�V���ԍ�(�[���I���W��)���擾
  machine_no = ITEM_GetWazaMashineNo( itemno );

  // �Y������Z�}�V���t���O�E�����r�b�g�ʒu���擾
  if( machine_no < 32*1 )
  {
    flag = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_machine1 );
    bit_pos = machine_no;
  }
  else if( machine_no < 32*2 ) 
  {
    flag = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_machine2 );
    bit_pos = machine_no - 32;
  }
  else if( machine_no < 32*3 ) 
  {
    flag = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_machine3 );
    bit_pos = machine_no - 32*2;
  }
  else
  {
    flag = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_machine4 );
    bit_pos = machine_no - 32*3;
  }

  // �p�[�\�i���f�[�^���N���[�Y
  POKE_PERSONAL_CloseHandle( ppd );

  // �t���O�̓���r�b�g������
  return ( flag & (1 << bit_pos) );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �w�肵�������X�^�[�̎�|�P�������擾����
 *
 * @param monsno ��|�P�����𒲂ׂ郂���X�^�[
 * @return �w�胂���X�^�[�̎�|�P�����̃����X�^�[�i���o�[
 */
//---------------------------------------------------------------------------------------- 
static u16 GetSeedMonsNo( u16 monsno )
{
  u16 seed_monsno;

  // �����X�^�[�i���o�[������
  if( MONSNO_END <= monsno )
  {
    OBATA_Printf( "GetSeedMonsNo: �����ȃ����X�^�[�i���o�[���w�肳��܂����B\n" );
    return monsno; // �Ƃ肠����, ���̂܂ܕԂ�
  } 

  // ��|�P����No���擾
  GFL_ARC_LoadDataOfs( &seed_monsno, ARCID_PMSTBL, monsno, 0, sizeof(u16) );
  return seed_monsno;
} 


#ifdef PM_DEBUG
//========================================================================================
// ��debug functions
//========================================================================================

//---------------------------------------------------------------------------------------- 
/**
 * @brief �f�o�b�O�p�^�}�S���쐬���� ( �Y�܂ꂽ�� )
 *
 * @param gameData
 * @param heapID
 *
 * @return �쐬�����^�}�S
 */
//---------------------------------------------------------------------------------------- 
POKEMON_PARAM* DEBUG_POKEMON_EGG_CreatePlainEgg( GAMEDATA* gameData, HEAPID heap_id )
{
  POKEMON_PARAM* egg;

  egg = CreateDebugEgg( gameData, heap_id, MONSNO_HUSIGIDANE );

  return egg;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �f�o�b�O�p�^�}�S���쐬���� ( ���������z�� )
 *
 * @param gameData
 * @param heapID
 *
 * @return �쐬�����^�}�S
 */
//---------------------------------------------------------------------------------------- 
POKEMON_PARAM* DEBUG_POKEMON_EGG_CreateQuickEgg( GAMEDATA* gameData, HEAPID heap_id )
{
  POKEMON_PARAM* egg;

  egg = CreateDebugEgg( gameData, heap_id, MONSNO_HUSIGIDANE );
  PP_Put( egg, ID_PARA_friend, 0 );  // �z������

  return egg;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �f�o�b�O�p�^�}�S���쐬���� ( ���߃^�}�S )
 *
 * @param gameData
 * @param heapID
 *
 * @return �쐬�����^�}�S
 */
//---------------------------------------------------------------------------------------- 
POKEMON_PARAM* DEBUG_POKEMON_EGG_CreateIllegalEgg( GAMEDATA* gameData, HEAPID heap_id )
{
  POKEMON_PARAM* egg;

  egg = CreateDebugEgg( gameData, heap_id, MONSNO_HUSIGIDANE );
  PP_Put( egg, ID_PARA_fusei_tamago_flag, TRUE );  // �_�����܂��t���O

  return egg;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �f�o�b�O�p�^�}�S���쐬����
 *
 * @param gameData
 * @param heapID
 * @param monsno   �����X�^�[No.
 */
//---------------------------------------------------------------------------------------- 
static POKEMON_PARAM* CreateDebugEgg( GAMEDATA* gameData, HEAPID heapID, u32 monsno )
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


#endif // PM_DEBUG
