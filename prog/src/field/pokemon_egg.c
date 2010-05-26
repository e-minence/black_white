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
#include "poke_tool/tokusyu_def.h"// for TOKUSYU_xxxx
#include "item/itemsym.h"         // for ITEM_xxxx
#include "item/item.h"            // for ITEM_xxxx
#include "waza_tool/wazano_def.h" // for WAZANO_xxxx
#include "print/global_msg.h"     // for GlobalMsg_PokeName 
#include "arc/arc_def.h"          // for ARCID_PMSTBL

#include "pokemon_egg.h"


#define DEBUG_PRINT_ON      // �f�o�b�O�o�̓X�C�b�`
#define PRINT_TARGET    (2) // �f�o�b�O�o�͐�
#define RARE_EGG_CHANCE (5) // ���A�^�}�S���I��

// ���e�|�P����
typedef struct {
  const POKEMON_PARAM* father;
  const POKEMON_PARAM* mother;
} PARENT;


// prototype /////////////////////////////////////////////////////////////////////////////

// ����̌���
static void CorrectParent_bySex( PARENT* parent );
static void CorrectParent_byMetamon( PARENT* parent );
static void ReverseParent( PARENT* parent );
// �^�}�S�p�����[�^�̌���
static void EggCordinate_monsno( const PARENT* parent, POKEMON_PARAM* egg ); // �����X�^�[No.
static void EggCordinate_seikaku( const PARENT* parent, POKEMON_PARAM* egg ); // ���i
static void EggCordinate_tokusei( const PARENT* parent, POKEMON_PARAM* egg, HEAPID heap_id ); // ����
static void EggCordinate_ability_rand( const PARENT* parent, POKEMON_PARAM* egg ); // �̗���
static void EggCordinate_rare( const PARENT* parent, POKEMON_PARAM* egg ); // ���A���I
static void EggCordinate_waza_default( POKEMON_PARAM* egg ); // �f�t�H���g�Z�Z�b�g
static void EggCordinate_waza_egg( const PARENT* parent, POKEMON_PARAM* egg, HEAPID heap_id ); // �^�}�S�Z�Z�b�g
static void EggCordinate_waza_parent( const PARENT* parent, POKEMON_PARAM* egg ); // �p���Z ( ���e ) �Z�b�g
static void EggCordinate_waza_machine( const PARENT* parent, POKEMON_PARAM* egg, HEAPID heap_id ); // �p���Z ( �Z�}�V�� ) �Z�b�g
static void EggCordinate_pityuu( const PARENT* parent, POKEMON_PARAM* egg ); // ����^�}�S���� ( �s�`���[ )
static void EggCordinate_karanakusi( const PARENT* parent, POKEMON_PARAM* egg ); // ����^�}�S���� ( �J���i�N�V )
static void EggCordinate_waza_sort( POKEMON_PARAM* egg ); // �Z���\�[�g����
static void EggCordinate_finish( POKEMON_PARAM* egg, const MYSTATUS* mystatus, int memo, HEAPID heap_id );
static const POKEMON_PARAM* GetBasePokemon( const PARENT* parent );
static u32 ChangeEggMonsNo_manafi( const POKEMON_PARAM* base_poke, u32 monsno_egg );
static u32 ChangeEggMonsNo_item( const POKEMON_PARAM* base_poke, u32 monsno_egg );
// �^�}�S�z��
static void EggBirth( POKEMON_PARAM* egg, const MYSTATUS* owner, HEAPID heap_id ); // �^�}�S��z��������
// ���[�e�B���e�B
static void PokeWazaLearnByPush( POKEMON_PARAM* poke, u32 wazano ); // �Z�������o�����Ɋo�������� ( �d���`�F�b�N�L )
static BOOL CheckWazaHave( const POKEMON_PARAM* poke, u32 wazano ); // �w�肵���Z�������Ă��邩�ǂ������`�F�b�N����
static BOOL IsWazaMachineAbleToUse( HEAPID heap_id, u16 monsno, u16 formno, u16 itemno ); // �Z�}�V�����g���邩�ǂ������`�F�b�N����
static u16 GetSeedMonsNo( u16 monsno ); // ��|�P�������擾����
// �f�o�b�O
#ifdef PM_DEBUG
static POKEMON_PARAM* CreateDebugEgg( GAMEDATA* gameData, HEAPID heapID, u32 monsno ); // �f�o�b�O�p�^�}�S���쐬����
#endif



// publid functions //////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S���쐬����
 *
 * @param heap_id  �g�p����q�[�vID
 * @param mystatus �^�}�S�̏��L��
 * @param memo     �g���[�i�[�����̋L��
 * @param poke1    �e�|�P����1
 * @param poke2    �e�|�P����2
 * @param egg      �쐬�����^�}�S�̊i�[��
 */
//---------------------------------------------------------------------------------------- 
void POKEMON_EGG_Create( 
    HEAPID heap_id, const MYSTATUS* mystatus, int memo, 
    const POKEMON_PARAM* poke1, const POKEMON_PARAM* poke2, POKEMON_PARAM* egg )
{
  PARENT parent;

  // ���������
  parent.father = poke1;
  parent.mother = poke2;
  CorrectParent_bySex( &parent );
  CorrectParent_byMetamon( &parent );

  // �^�}�S�̃p�����[�^��ݒ�
  EggCordinate_monsno( &parent, egg );                 // �����X�^�[�i���o�[
  EggCordinate_seikaku( &parent, egg );                // ���i
  EggCordinate_tokusei( &parent, egg, heap_id );       // ����
  EggCordinate_ability_rand( &parent, egg );           // �̗���
  EggCordinate_rare( &parent, egg );                   // ���A�|�P���I
  EggCordinate_waza_default( egg );                    // �Z ( �f�t�H���g )
  EggCordinate_waza_egg( &parent, egg, heap_id );      // �Z ( �^�}�S�Z )
  EggCordinate_waza_parent( &parent, egg );            // �Z ( ���e���ʂ̋Z )
  EggCordinate_waza_machine( &parent, egg, heap_id );  // �Z ( ���̃}�V���Z )
  EggCordinate_pityuu( &parent, egg );                 // ����^�}�S ( �s�`���[ )
  EggCordinate_karanakusi( &parent, egg );             // ����^�}�S ( �J���i�N�V )
  EggCordinate_waza_sort( egg );                       // �Z�\�[�g
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



// private functions /////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------------------- 
/**
 * @brief ����̑g�ݍ��킹��������� ( ���� )
 */
//---------------------------------------------------------------------------------------- 
static void CorrectParent_bySex( PARENT* parent )
{
  u32 sex_father, sex_mother;

  sex_father = PP_Get( parent->father, ID_PARA_sex, NULL );
  sex_mother = PP_Get( parent->mother, ID_PARA_sex, NULL );

  if( sex_father == PTL_SEX_FEMALE ) {
    ReverseParent( parent );
  }
  if( sex_mother == PTL_SEX_MALE ) {
    ReverseParent( parent );
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ����̑g�ݍ��킹��������� ( ���^���� )
 *
 * ���^���� �~ �� ==> ( ��, �� ) = ( ��, ���^���� )
 * ���^���� �~ �� ==> ( ��, �� ) = ( ���^����, �� )
 * ���^���� �~ �H ==> ( ��, �� ) = ( �H, ���^���� )
 */
//---------------------------------------------------------------------------------------- 
static void CorrectParent_byMetamon( PARENT* parent )
{
  u32 monsno_father;
  u32 sex_mother;

  monsno_father = PP_Get( parent->father, ID_PARA_monsno, NULL );
  sex_mother = PP_Get( parent->mother, ID_PARA_sex, NULL );

  if( (monsno_father == MONSNO_METAMON) && (sex_mother == PTL_SEX_UNKNOWN) ) {
    ReverseParent( parent );
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ��������ւ���
 */
//---------------------------------------------------------------------------------------- 
static void ReverseParent( PARENT* parent )
{
  const POKEMON_PARAM* temp = parent->father;
  parent->father = parent->mother;
  parent->mother = temp;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃����X�^�[�i���o�[�����肷��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_monsno( const PARENT* parent, POKEMON_PARAM* egg )
{
  const POKEMON_PARAM* base_poke;
  u32 monsno_base, monsno_egg;

  // ��{���[���Ŏq������
  base_poke   = GetBasePokemon( parent );
  monsno_base = PP_Get( base_poke, ID_PARA_monsno, NULL );
  monsno_egg  = GetSeedMonsNo( monsno_base );

  // ��O���[���ɂ��q�|�P�����̕ω�
  monsno_egg = ChangeEggMonsNo_manafi( base_poke, monsno_egg );
  monsno_egg = ChangeEggMonsNo_item( base_poke, monsno_egg );
  
  // �|�P�����p�����[�^�\�z
  PP_SetupEx( egg, monsno_egg, 1, 
      PTL_SETUP_ID_AUTO, PTL_SETUP_POW_AUTO, PTL_SETUP_RND_AUTO );

#ifdef DEBUG_PRINT_ON
  {
    u32 monsno_father = PP_Get( parent->father, ID_PARA_monsno, NULL );
    u32 monsno_mother = PP_Get( parent->mother, ID_PARA_monsno, NULL );
    OS_TFPrintf( PRINT_TARGET,
        "POKEMON-EGG: <MonsNo> father[%d], mother[%d], egg[%d]\n", 
        monsno_father, monsno_mother, monsno_egg );
  }
#endif
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �q�|�P�����̃x�[�X�ƂȂ�e�|�P�������擾����
 */
//---------------------------------------------------------------------------------------- 
static const POKEMON_PARAM* GetBasePokemon( const PARENT* parent )
{
  u32 monsno_father = PP_Get( parent->father, ID_PARA_monsno, NULL );
  u32 monsno_mother = PP_Get( parent->mother, ID_PARA_monsno, NULL );

  // ��e�����^���� ==> ���e
  if( monsno_mother == MONSNO_METAMON ) {
    return parent->father;
  }
  // �ʏ� ==> ��e
  else {
    return parent->mother;
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �q�|�P������ϊ����� ( ��O�P�[�X�F�}�i�t�B )
 *
 * �}�i�t�B�̎q�̓t�B�I�l
 */
//---------------------------------------------------------------------------------------- 
static u32 ChangeEggMonsNo_manafi( const POKEMON_PARAM* base_poke, u32 monsno_egg )
{
  u32 monsno_base;

  monsno_base = PP_Get( base_poke, ID_PARA_monsno, NULL );

  if( monsno_base == MONSNO_MANAFI ) {
    return MONSNO_FIONE;
  }
  else { 
    return monsno_egg;
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �q�|�P������ϊ����� ( ��O�P�[�X�F�A�C�e�� )
 */
//---------------------------------------------------------------------------------------- 
static u32 ChangeEggMonsNo_item( const POKEMON_PARAM* base_poke, u32 monsno_egg )
{
  // ��O�e�[�u���K�p
  int i;
  u32 monsno_base = PP_Get( base_poke, ID_PARA_monsno, NULL );
  u32 itemno_base = PP_Get( base_poke, ID_PARA_item, NULL );

  const int table_size = 9;
  u32 exception_table[table_size][3] = // ��O�e�[�u��
  {
    // ���|�P����      // �A�C�e��          // �q�|�P����
    {MONSNO_MARIRU,    ITEM_USIONOOKOU,     MONSNO_MARIRU},
    {MONSNO_SOONANSU,  ITEM_NONKINOOKOU,    MONSNO_SOONANSU},
    {MONSNO_BARIYAADO, ITEM_AYASIIOKOU,     MONSNO_BARIYAADO},
    {MONSNO_USOKKII,   ITEM_GANSEKIOKOU,    MONSNO_USOKKII},
    {MONSNO_KABIGON,   ITEM_MANPUKUOKOU,    MONSNO_KABIGON},
    {MONSNO_MANTAIN,   ITEM_SAZANAMINOOKOU, MONSNO_MANTAIN},
    {MONSNO_ROZERIA,   ITEM_OHANANOOKOU,    MONSNO_ROZERIA},
    {MONSNO_RAKKII,    ITEM_KOUUNNOOKOU,    MONSNO_RAKKII},
    {MONSNO_TIRIIN,    ITEM_KIYOMENOOKOU,   MONSNO_TIRIIN},
  };

  // ��O�e�[�u���Ƃ̏ƍ�
  for( i=0; i<table_size; i++ )
  {
    // ���|�P������A�C�e���������Ă��Ȃ�������, �q�|�P�������ω�
    if( ( monsno_base == exception_table[i][0] ) &&
        ( itemno_base != exception_table[i][1] ) ) {
      return exception_table[i][2];
    }
  }

  return monsno_egg;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̐��i�����肷��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_seikaku( const PARENT* parent, POKEMON_PARAM* egg )
{
  BOOL kawarazu_father; // ���e���w����炸�̂����x�������Ă��邩�ǂ���
  BOOL kawarazu_mother; // ��e���w����炸�̂����x�������Ă��邩�ǂ���
  u32 seikaku_father = PP_Get( parent->father, ID_PARA_seikaku, NULL ); // ���e�̐��i
  u32 seikaku_mother = PP_Get( parent->mother, ID_PARA_seikaku, NULL ); // ��e�̐��i
  u32 seikaku_egg; // �q�̐��i

  //�w����炸�̂����x�������Ă��邩�ǂ������擾
  kawarazu_father = ( PP_Get( parent->father, ID_PARA_item, NULL ) == ITEM_KAWARAZUNOISI );
  kawarazu_mother = ( PP_Get( parent->mother, ID_PARA_item, NULL ) == ITEM_KAWARAZUNOISI );

  // �f�t�H���g�͗����Ō���
  seikaku_egg = GFUser_GetPublicRand0( PTL_SEIKAKU_MAX );

  // ���e�Ƃ��u����炸�̂����v�������Ă���
  if( kawarazu_father && kawarazu_mother ) {
    // 1/2�Ōp������
    if( GFUser_GetPublicRand0(2) == 0 ) {
      // ���e����p��
      if( GFUser_GetPublicRand0(2) == 0 )  {
        seikaku_egg = seikaku_father;
      }
      // ��e����p��
      else {
        seikaku_egg = seikaku_mother;
      }
    }
  }
  // ���e�̂ݎ����Ă���
  else if( kawarazu_father ) {
    // 1/2�Ōp������
    if( GFUser_GetPublicRand0(2) == 0 ) {
      seikaku_egg = seikaku_father;
    }
  }
  // ��e�̂ݎ����Ă���
  else if( kawarazu_mother ) {
    // 1/2�Ōp������
    if( GFUser_GetPublicRand0(2) == 0 ) {
      seikaku_egg = seikaku_mother;
    }
  }

  // ���i���Z�b�g
  PP_Put( egg, ID_PARA_seikaku, seikaku_egg );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <Seikaku> father[%d], mother[%d] ==> egg[%d]\n", 
      seikaku_father, seikaku_mother, seikaku_egg );
#endif
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̓��������肷�� ( ��e�̓�������`���� )
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_tokusei( const PARENT* parent, POKEMON_PARAM* egg, HEAPID heap_id )
{
  u32 rnd;
  u32 speabi_egg_idx;    // ���Ԗڂ̓������p�����邩
  u32 speabi_mother;     // ��̓���
  u32 speabi_mother_idx; // ��̓����͉��Ԗڂ̂Ƃ������Ȃ̂�
  u32 speabi_list[3];    // �������X�g

  // ��e�̓������擾
  speabi_mother = PP_Get( parent->mother, ID_PARA_speabino, NULL );

  // ��̓������X�g ( ��̓��� ) ���擾
  {
    POKEMON_PERSONAL_DATA* personalData;
    u32 monsno     = PP_Get( parent->mother, ID_PARA_monsno, NULL );
    u32 formno     = PP_Get( parent->mother, ID_PARA_form_no, NULL );
    personalData   = POKE_PERSONAL_OpenHandle( monsno, formno, heap_id );
    speabi_list[0] = POKE_PERSONAL_GetParam( personalData, POKEPER_ID_speabi1 );
    speabi_list[1] = POKE_PERSONAL_GetParam( personalData, POKEPER_ID_speabi2 );
    speabi_list[2] = POKE_PERSONAL_GetParam( personalData, POKEPER_ID_speabi3 );
    POKE_PERSONAL_CloseHandle( personalData );
  }
  
  // ��e�̓����ԍ��𒲂ׂ�
  for( speabi_mother_idx=0; speabi_mother_idx<3; speabi_mother_idx++ )
  {
    // ����
    if( speabi_mother == speabi_list[ speabi_mother_idx ] ) { break; }
  }
  // ��e���s���ȓ����������Ă���ꍇ
  if( 3 <= speabi_mother_idx ) { 
    OS_Printf( "��|�P�����̓����Ɍ�肪����܂��B\n" );
    GF_ASSERT(0);
    speabi_mother_idx = 0; // �s���l�Ȃ�, ����1���p�����邱�Ƃɂ���
  }

  // �p���������������
  rnd = GFUser_GetPublicRand0(100);  // �����擾[0, 99]
  switch( speabi_mother_idx ) {
  // �ꂪ����1
  case 0:
    if( rnd < 80 ) { speabi_egg_idx = 0; } // 80% ==> �q�̓���1
    else           { speabi_egg_idx = 1; } // 20% ==> �q�̓���2
    break;
  // �ꂪ����2
  case 1: 
    if( rnd < 20 ) { speabi_egg_idx = 0; } // 20% ==> �q�̓���1
    else           { speabi_egg_idx = 1; } // 80% ==> �q�̓���2
    break;
  // �ꂪ����3
  case 2: 
    if( rnd < 20 )      { speabi_egg_idx = 0; } // 20% ==> �q�̓���1
    else if( rnd < 40 ) { speabi_egg_idx = 1; } // 20% ==> �q�̓���2
    else                { speabi_egg_idx = 2; } // 60% ==> �q�̓���3
    break;
  default:
    GF_ASSERT(0);
    break;
  }

  // �������Z�b�g
  if( speabi_egg_idx == 2 ) { // ��O����
    u32 monsno = PP_Get( parent->mother, ID_PARA_monsno, NULL );
    u32 formno = PP_Get( parent->mother, ID_PARA_form_no, NULL );
    PP_SetTokusei3( egg, monsno, formno );
  }
  else {
    u32 speabi_egg = speabi_list[ speabi_egg_idx ]; 

    // ���肵���ԍ��ɓ������Ȃ��ꍇ, ����0�ɂ���
    if( speabi_egg == TOKUSYU_NULL ) {
      speabi_egg = speabi_list[0];
    } 
    PP_Put( egg, ID_PARA_speabino, speabi_egg );
  }


#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <Tokusei> mother[%d](%d, %d, %d) ==> egg[%d]\n", 
      speabi_mother, speabi_list[0], speabi_list[1], speabi_list[2], 
      PP_Get( egg, ID_PARA_speabino, NULL ) );
#endif
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̌̗��������肷��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_ability_rand( const PARENT* parent, POKEMON_PARAM* egg )
{
  int i = 0;
  u32 ability_type[3];  // �p������̗����̃^�C�v
  int num = 0; // �p������̗����̃^�C�v�̌��萔

  // ���e���w�p���[XXXX�x�������Ă���ꍇ
  {
    u32 itemno = PP_Get( parent->father, ID_PARA_item, NULL );
    switch( itemno ) {
    case ITEM_PAWAAUEITO:  ability_type[num++] = PTL_ABILITY_HP;    break;
    case ITEM_PAWAARISUTO: ability_type[num++] = PTL_ABILITY_ATK;   break;
    case ITEM_PAWAABERUTO: ability_type[num++] = PTL_ABILITY_DEF;   break;
    case ITEM_PAWAARENZU:  ability_type[num++] = PTL_ABILITY_SPATK; break;
    case ITEM_PAWAABANDO:  ability_type[num++] = PTL_ABILITY_SPDEF; break;
    case ITEM_PAWAAANKURU: ability_type[num++] = PTL_ABILITY_AGI;   break;
    }
  }

  // ��e���w�p���[XXXX�x�������Ă���ꍇ
  if( ( num == 0 ) ||
      ( GFUser_GetPublicRand0(2) == 0 ) ) // ���e�������Ă����ꍇ, 1/2�ŏ㏑������
  {
    u32 itemno = PP_Get( parent->father, ID_PARA_item, NULL );
    num = 0;  // ���e�̌p�����㏑��
    switch( itemno ) {
    case ITEM_PAWAAUEITO:  ability_type[num++] = PTL_ABILITY_HP;    break;
    case ITEM_PAWAARISUTO: ability_type[num++] = PTL_ABILITY_ATK;   break;
    case ITEM_PAWAABERUTO: ability_type[num++] = PTL_ABILITY_DEF;   break;
    case ITEM_PAWAARENZU:  ability_type[num++] = PTL_ABILITY_SPATK; break;
    case ITEM_PAWAABANDO:  ability_type[num++] = PTL_ABILITY_SPDEF; break;
    case ITEM_PAWAAANKURU: ability_type[num++] = PTL_ABILITY_AGI;   break;
    }
  }

  // �p������̗����������_���ɑI��
  while( num < 3 )
  {
    // �����_���ɑI��
    u32 type = GFUser_GetPublicRand0(PTL_ABILITY_MAX);

    // ���łɑI�΂�Ă�����, �Ē��I����
    for( i=0; i<num; i++ )
    {
      if( ability_type[i] == type ) { continue; }
    }

    // �p������̗����̃^�C�v��o�^
    ability_type[num++] = type;
  }

  // �ꉞ�`�F�b�N
  GF_ASSERT( num == 3 );

  // �I�񂾌̗������p������ 
  for( i=0; i<num; i++ )
  {
    const POKEMON_PARAM* param;
    int param_id;
    u32 value;

    // ���E��̂ǂ��炩��󂯌p���̂�������
    if( GFUser_GetPublicRand0(2) == 0 ) { param = parent->father; }
    else                                { param = parent->mother; }

    // �p��
    switch( ability_type[i] ) {
    case PTL_ABILITY_HP:    param_id = ID_PARA_hp_rnd;     break;
    case PTL_ABILITY_ATK:   param_id = ID_PARA_pow_rnd;    break;
    case PTL_ABILITY_DEF:   param_id = ID_PARA_def_rnd;    break;
    case PTL_ABILITY_SPATK: param_id = ID_PARA_spepow_rnd; break;
    case PTL_ABILITY_SPDEF: param_id = ID_PARA_spedef_rnd; break;
    case PTL_ABILITY_AGI:   param_id = ID_PARA_agi_rnd;    break;
    default: GF_ASSERT(0); break;
    }
    value = PP_Get( param, param_id, NULL );
    PP_Put( egg, param_id, value );
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <AbilityRnd> father(%d, %d, %d, %d, %d, %d)\n", 
      PP_Get( parent->father, ID_PARA_hp_rnd, NULL ), PP_Get( parent->father, ID_PARA_pow_rnd, NULL ),
      PP_Get( parent->father, ID_PARA_def_rnd, NULL ), PP_Get( parent->father, ID_PARA_spepow_rnd, NULL ),
      PP_Get( parent->father, ID_PARA_spedef_rnd, NULL ), PP_Get( parent->father, ID_PARA_agi_rnd, NULL ) );
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <AbilityRnd> mother(%d, %d, %d, %d, %d, %d)\n", 
      PP_Get( parent->mother, ID_PARA_hp_rnd, NULL ), PP_Get( parent->mother, ID_PARA_pow_rnd, NULL ),
      PP_Get( parent->mother, ID_PARA_def_rnd, NULL ), PP_Get( parent->mother, ID_PARA_spepow_rnd, NULL ),
      PP_Get( parent->mother, ID_PARA_spedef_rnd, NULL ), PP_Get( parent->mother, ID_PARA_agi_rnd, NULL ) );
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <AbilityRnd> ==> egg(%d, %d, %d, %d, %d, %d)\n", 
      PP_Get( egg, ID_PARA_hp_rnd, NULL ), PP_Get( egg, ID_PARA_pow_rnd, NULL ),
      PP_Get( egg, ID_PARA_def_rnd, NULL ), PP_Get( egg, ID_PARA_spepow_rnd, NULL ),
      PP_Get( egg, ID_PARA_spedef_rnd, NULL ), PP_Get( egg, ID_PARA_agi_rnd, NULL ) );
#endif
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ���A�|�P�����̒��I���s��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_rare( const PARENT* parent, POKEMON_PARAM* egg )
{
  int i;
  u32 id, rnd;
  u32 country_father = PP_Get( parent->father, ID_PARA_country_code, NULL ); // ���̍��R�[�h
  u32 country_mother = PP_Get( parent->mother, ID_PARA_country_code, NULL ); // ��̍��R�[�h
  BOOL egg_is_rare = FALSE; // �q�����A�|�P���ǂ���
  BOOL chance_flag = TRUE; // ���A�|�P�̒��I���s�����ǂ���

  // �e���m�̍��R�[�h�������Ȃ�, �������Ȃ�
  if( country_father == country_mother ) { chance_flag = FALSE; }

  // ���łɃ��A�Ȃ�, �������Ȃ�
  id  = PP_Get( egg, ID_PARA_id_no, NULL );
  rnd = PP_Get( egg, ID_PARA_personal_rnd, NULL );
  if( POKETOOL_CheckRare( id, rnd ) ) { egg_is_rare = TRUE; }

  // ���I
  if( (egg_is_rare == FALSE) && (chance_flag == TRUE) ) {

    // �ő� RARE_EGG_CHANCE ��̒��I���s��
    for( i=0; i<RARE_EGG_CHANCE; i++ )
    {
      rnd = GFUser_GetPublicRand0( 0xffffffff );

      // ���A���o�������������������, ���I���I��
      if( POKETOOL_CheckRare( id, rnd ) ) {
        PP_Put( egg, ID_PARA_personal_rnd, rnd );
        egg_is_rare = TRUE;
        break;
      }
    }
  }

  // �����A���o�Ȃ������̂ł���Ό������͏��������Ȃ�

#ifdef DEBUG_PRINT_ON
  if( egg_is_rare ) {
    OS_TFPrintf( PRINT_TARGET,
        "POKEMON-EGG: <PersonalRnd> egg[%d] ==> Rare!!\n", 
        PP_Get( egg, ID_PARA_personal_rnd, NULL ) );
  }
  else {
    OS_TFPrintf( PRINT_TARGET,
        "POKEMON-EGG: <PersonalRnd> egg[%d] ==> NotRare\n", 
        PP_Get( egg, ID_PARA_personal_rnd, NULL ) );
  }
#endif
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
  POKEPER_WAZAOBOE_CODE wazaTable[ POKEPER_WAZAOBOE_TABLE_ELEMS ];

  // ��{�����擾
  monsno = PP_Get( egg, ID_PARA_monsno, NULL );
  formno = PP_Get( egg, ID_PARA_form_no, NULL );
  level  = PP_Get( egg, ID_PARA_level, NULL );

  // Lv.1 �Ŋo����Z���Z�b�g
  POKE_PERSONAL_LoadWazaOboeTable( monsno, formno, wazaTable );
  i = 0;
  while( !POKEPER_WAZAOBOE_IsEndCode( wazaTable[i] ) )
  {
    int waza_lv = POKEPER_WAZAOBOE_GetLevel( wazaTable[i] );
    int waza_id = POKEPER_WAZAOBOE_GetWazaID( wazaTable[i] );
    if( level < waza_lv ) { break; }
    PokeWazaLearnByPush( egg, waza_id );
    i++;
  }


#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <DefaultWaza> ==> egg(%d, %d, %d, %d)\n",
      PP_Get( egg, ID_PARA_waza1, NULL ), PP_Get( egg, ID_PARA_waza2, NULL ),
      PP_Get( egg, ID_PARA_waza3, NULL ), PP_Get( egg, ID_PARA_waza4, NULL ) );
#endif
}

//----------------------------------------------------------------------------------------
/**
 * @brief �^�}�S�Z���o��������
 *
 * �^�}�S����z��|�P�����݂̂��o������Z�̂���, ���|�P�������o���Ă���Z���K��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_waza_egg( const PARENT* parent, POKEMON_PARAM* egg, HEAPID heap_id )
{
  int i, j;
  u32 monsno_egg;
  u32 id_para[ PTL_WAZA_MAX ] = { ID_PARA_waza1, ID_PARA_waza2, ID_PARA_waza3, ID_PARA_waza4 };
  u16 kowaza_num;
  u16* kowaza_table;

  // �q�Z�e�[�u�����擾
  monsno_egg   = PP_Get( egg, ID_PARA_monsno, NULL );
  kowaza_table = GFL_ARC_LoadDataAlloc( ARCID_KOWAZA, monsno_egg, heap_id );
  kowaza_num   = kowaza_table[0];

  // ���e�̑S�Z���`�F�b�N����
  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    u32 wazano = PP_Get( parent->father, id_para[i], NULL );

    // ���e�̋Z���^�}�S�Z�Ȃ�, �K������
    for( j=1; j<=kowaza_num; j++ )
    {
      if( wazano == kowaza_table[j] ) {
        PokeWazaLearnByPush( egg, wazano );
        break;
      }
    }
  }

  // �q�Z�e�[�u����j��
  GFL_HEAP_FreeMemory( kowaza_table );


#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <TamagoWaza> father(%d, %d, %d, %d)\n",
      PP_Get( parent->father, ID_PARA_waza1, NULL ), PP_Get( parent->father, ID_PARA_waza2, NULL ),
      PP_Get( parent->father, ID_PARA_waza3, NULL ), PP_Get( parent->father, ID_PARA_waza4, NULL ) );
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <TamagoWaza> ==> egg(%d, %d, %d, %d)\n",
      PP_Get( egg, ID_PARA_waza1, NULL ), PP_Get( egg, ID_PARA_waza2, NULL ),
      PP_Get( egg, ID_PARA_waza3, NULL ), PP_Get( egg, ID_PARA_waza4, NULL ) );
#endif
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ���e�̋Z���󂯌p��
 *
 * ���������x�����グ�邱�ƂŊo����Z�̂���, ���e�Ƃ��Ɋo���Ă���Z���K��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_waza_parent( const PARENT* parent, POKEMON_PARAM* egg )
{
  int i, j;
  u32 id_para[ PTL_WAZA_MAX ] = { ID_PARA_waza1, ID_PARA_waza2, ID_PARA_waza3, ID_PARA_waza4 };
  POKEPER_WAZAOBOE_CODE wazaTable[ POKEPER_WAZAOBOE_TABLE_ELEMS ]; // �q�̋Z�o���e�[�u��

  // �q�̋Z�o���e�[�u�����擾
  {
    u32 monsno = PP_Get( egg, ID_PARA_monsno, NULL );
    u32 formno = PP_Get( egg, ID_PARA_form_no, NULL );
    POKE_PERSONAL_LoadWazaOboeTable( monsno, formno, wazaTable );
  }

  // �q�̑S���x���A�b�v�K���Z���`�F�b�N
  i = 0;
  while( !POKEPER_WAZAOBOE_IsEndCode( wazaTable[i] ) )
  {
    u32  wazano      = POKEPER_WAZAOBOE_GetWazaID( wazaTable[i] );
    BOOL father_have = FALSE; // ���e���o���Ă���Z���ǂ���
    BOOL mother_have = FALSE; // ��e���o���Ă���Z���ǂ���

    // �����o���Ă��邩�ǂ����`�F�b�N
    for( j=0; j<PTL_WAZA_MAX; j++ )
    {
      u32 wazano_father = PP_Get( parent->father, id_para[j], NULL );

      // �o���Ă���
      if( wazano_father == wazano ) {
        father_have = TRUE;
        break;
      }
    }

    // �ꂪ�o���Ă��邩�ǂ����`�F�b�N
    for( j=0; j<PTL_WAZA_MAX; j++ )
    {
      u32 wazano_mother = PP_Get( parent->mother, id_para[j], NULL );

      // �o���Ă���
      if( wazano_mother == wazano ) {
        mother_have = TRUE;
        break;
      }
    }

    // ���e�Ƃ��o���Ă�����p������
    if( father_have && mother_have ) {
      PokeWazaLearnByPush( egg, wazano );
    }

    // ���̋Z��
    i++;
  }


#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <ParentWaza> father(%d, %d, %d, %d)\n",
      PP_Get( parent->father, ID_PARA_waza1, NULL ), PP_Get( parent->father, ID_PARA_waza2, NULL ),
      PP_Get( parent->father, ID_PARA_waza3, NULL ), PP_Get( parent->father, ID_PARA_waza4, NULL ) );
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <ParentWaza> mother(%d, %d, %d, %d)\n",
      PP_Get( parent->mother, ID_PARA_waza1, NULL ), PP_Get( parent->mother, ID_PARA_waza2, NULL ),
      PP_Get( parent->mother, ID_PARA_waza3, NULL ), PP_Get( parent->mother, ID_PARA_waza4, NULL ) );
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <ParentWaza> ==> egg(%d, %d, %d, %d)\n",
      PP_Get( egg, ID_PARA_waza1, NULL ), PP_Get( egg, ID_PARA_waza2, NULL ),
      PP_Get( egg, ID_PARA_waza3, NULL ), PP_Get( egg, ID_PARA_waza4, NULL ) );
#endif
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ���|�P�����̃}�V���Z���󂯌p��
 *
 * �������o���邱�Ƃ̏o����}�V���Z�̂���, ���|�P�������o���Ă���Z���K��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_waza_machine( const PARENT* parent, POKEMON_PARAM* egg, HEAPID heap_id )
{
  int i, itemno;
  u32 wazano;
  u32 monsno_egg = PP_Get( egg, ID_PARA_monsno, NULL );
  u32 formno_egg = PP_Get( egg, ID_PARA_form_no, NULL );
  u32 id_para[ PTL_WAZA_MAX ] = { ID_PARA_waza1, ID_PARA_waza2, ID_PARA_waza3, ID_PARA_waza4 };

  // ���̑S�Z���`�F�b�N
  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    // �Z�R�[�h�擾
    wazano = PP_Get( parent->father, id_para[i], NULL );

    // ���Z�̋Z�}�V�������݂��邩�ǂ����`�F�b�N
    for( itemno=ITEM_WAZAMASIN01; itemno<=ITEM_DATA_MAX; itemno++ )
    {
      // �Z�}�V��������, �^�}�S�|�P�Ɏg�p�ł���Ȃ�, �K������
      if( ( wazano == ITEM_GetWazaNo(itemno) ) &&
          ( IsWazaMachineAbleToUse(heap_id, monsno_egg, formno_egg, itemno) ) ) {
        PokeWazaLearnByPush( egg, wazano );
        break;
      }
    }
  }


#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <MachineWaza> father(%d, %d, %d, %d)\n",
      PP_Get( parent->father, ID_PARA_waza1, NULL ), PP_Get( parent->father, ID_PARA_waza2, NULL ),
      PP_Get( parent->father, ID_PARA_waza3, NULL ), PP_Get( parent->father, ID_PARA_waza4, NULL ) );
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <MachineWaza> ==> egg(%d, %d, %d, %d)\n",
      PP_Get( egg, ID_PARA_waza1, NULL ), PP_Get( egg, ID_PARA_waza2, NULL ),
      PP_Get( egg, ID_PARA_waza3, NULL ), PP_Get( egg, ID_PARA_waza4, NULL ) );
#endif
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ����^�}�S�쐬���� ( �s�`���[ )
 *        �e�|�P���A�C�e���w�ł񂫂��܁x�������Ă�����, �Z�w�{���e�b�J�[�x���K��
 *
 * @param father ���e�|�P����
 * @param mother ��e�|�P����
 * @param egg    �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_pityuu( const PARENT* parent, POKEMON_PARAM* egg )
{
  // �s�`���[�̃^�}�S�łȂ���Ή������Ȃ�
  if( PP_Get( egg, ID_PARA_monsno, NULL ) != MONSNO_PITYUU ) { return; }

  // ���e�̂ǂ��炩���w�ł񂫂��܁x�������Ă�����,�w�{���e�b�J�[�x���K��
  if( ( PP_Get( parent->father, ID_PARA_item, NULL ) == ITEM_DENKIDAMA ) ||
      ( PP_Get( parent->mother, ID_PARA_item, NULL ) == ITEM_DENKIDAMA ) ) {
    PokeWazaLearnByPush( egg, WAZANO_BORUTEKKAA );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "POKEMON-EGG: <Pityuu> learn BORUTEKKAA\n" );
#endif 
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
static void EggCordinate_karanakusi( const PARENT* parent, POKEMON_PARAM* egg )
{
  u32 monsno_mother = PP_Get( parent->mother, ID_PARA_monsno, NULL );

  // ��e���w�J���i�N�V�xor�w�g���g�h���x�Ȃ�, �t�H�������p��
  if( (monsno_mother == MONSNO_KARANAKUSI) ||
      (monsno_mother == MONSNO_TORITODON) ) {

    u32 formno = PP_Get( parent->mother, ID_PARA_form_no, NULL );
    PP_Put( egg, ID_PARA_form_no, formno );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "POKEMON-EGG: <Karanakusi> derive form[%d]\n", formno );
#endif 
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �Z���\�[�g����
 *        �K�����Ă���Z��O�l�߂���
 *
 * @param egg  �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_waza_sort( POKEMON_PARAM* egg )
{
  int i, num;
  u32 wazano[ PTL_WAZA_MAX ]; 
  u32 id_para[ PTL_WAZA_MAX ] = { ID_PARA_waza1, ID_PARA_waza2, ID_PARA_waza3, ID_PARA_waza4 };

  // �S�Ă̋Z���擾
  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    wazano[i] = PP_Get( egg, id_para[i], NULL );
  }

  // �S�Ă̋Z���N���A
  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    PP_Put( egg, id_para[i], WAZANO_NULL );
  }

  // �o���Ă����Z��O�l�߂ŃZ�b�g
  for( num=0,i=0; i<PTL_WAZA_MAX; i++ )
  {
    if( wazano[i] != WAZANO_NULL ) {
      PP_Put( egg, id_para[num], wazano[i] );
      num++;
    }
  }


#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET,
      "POKEMON-EGG: <WazaSort> ==> egg(%d, %d, %d, %d)\n",
      PP_Get( egg, ID_PARA_waza1, NULL ), PP_Get( egg, ID_PARA_waza2, NULL ),
      PP_Get( egg, ID_PARA_waza3, NULL ), PP_Get( egg, ID_PARA_waza4, NULL ) );
#endif
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

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "POKEMON-EGG: <Finish>\n" );
#endif 
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
 * @brief �Z�������o�����Ɋo�������� ( �d���`�F�b�N�L )
 *
 * @param poke
 * @param wazano
 */
//---------------------------------------------------------------------------------------- 
static void PokeWazaLearnByPush( POKEMON_PARAM* poke, u32 wazano )
{
  int i;
  u32 id_para[ PTL_WAZA_MAX ] = { ID_PARA_waza1, ID_PARA_waza2, ID_PARA_waza3, ID_PARA_waza4 };

  // �Z�ԍ����s��
  if( wazano == WAZANO_NULL ) { return; }

  // ���łɊo���Ă���
  if( CheckWazaHave(poke, wazano) ) { return; }

  // �󂫗v�f�ɃZ�b�g
  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    if( PP_Get( poke, id_para[i], NULL ) == WAZANO_NULL ) {
      PP_Put( poke, id_para[i], wazano );
      return;
    }
  }

  // �󂫂��Ȃ���Ή����o��
  PP_SetWazaPush( poke, wazano );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �w�肵���Z�������Ă��邩�ǂ������`�F�b�N����
 *
 * @param poke   �`�F�b�N����|�P����
 * @param wazano �`�F�b�N����Z
 *
 * @return �w�肵���Z�������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------------- 
static BOOL CheckWazaHave( const POKEMON_PARAM* poke, u32 wazano )
{
  int i;
  u32 id_para[ PTL_WAZA_MAX ] = { ID_PARA_waza1, ID_PARA_waza2, ID_PARA_waza3, ID_PARA_waza4 };

  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    // ����
    if( PP_Get( poke, id_para[i], NULL ) == wazano ) {
      return TRUE;
    }
  }

  return FALSE;
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

  // �����X�^�[�i���o�[���`�F�b�N
  if( MONSNO_END <= monsno ) {
    OBATA_Printf( "IsWazaMachineAbleToUse: �����ȃ����X�^�[�i���o�[���w�肳��܂����B\n" );
    GF_ASSERT(0);
    return FALSE;
  }

  // �A�C�e�����Z�}�V�����ǂ������`�F�b�N
	if( ITEM_CheckWazaMachine( itemno ) == FALSE ) {
    OBATA_Printf( "IsWazaMachineAbleToUse: �Z�}�V���ȊO�̃A�C�e�����w�肳��܂����B\n" );
    return FALSE;
  } 

  // �p�[�\�i���f�[�^���I�[�v��
  ppd = POKE_PERSONAL_OpenHandle( monsno, formno, heap_id );

  // �Z�}�V���ԍ�(�[���I���W��)���擾
  machine_no = ITEM_GetWazaMashineNo( itemno );

  // �Y������Z�}�V���t���O�E�����r�b�g�ʒu���擾
  if( machine_no < 32*1 ) {
    flag = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_machine1 );
    bit_pos = machine_no;
  }
  else if( machine_no < 32*2 ) {
    flag = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_machine2 );
    bit_pos = machine_no - 32;
  }
  else if( machine_no < 32*3 ) {
    flag = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_machine3 );
    bit_pos = machine_no - 32*2;
  }
  else {
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
  if( MONSNO_END <= monsno ) {
    OBATA_Printf( "GetSeedMonsNo: �����ȃ����X�^�[�i���o�[���w�肳��܂����B\n" );
    GF_ASSERT(0);
    return monsno; // �s���l�Ȃ�, ���̂܂ܕԂ�
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
