//////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  ��ĉ�����
 * @file   sodateya.c
 * @author obata
 * @date   2009.10.01
 *
 */
//////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/monsno_def.h"
#include "item/itemsym.h"
#include "item/item.h"
#include "waza_tool/wazano_def.h"
#include "savedata/sodateya_work.h" 
#include "fieldmap.h"
#include "sodateya.h" 

#include "arc/arc_def.h"  // ARCID_PMSTBL
#include "arc/kowaza.naix"
#include "gamesystem/game_data.h"
#include "savedata/mystatus.h"


//========================================================================================
// �� �萔
//========================================================================================
#define CHARGE_MIN         (100) // �ŏ�����
#define CHARGE_PER_LEVEL   (100) // 1���x�����̈�����藿��
#define EXP_PER_WALK         (1) // �������тɉ��Z����o���l
#define EGG_CHECK_INTERVAL (256) // �Y������̕p�x
#define RARE_EGG_CHANCE      (5) // ���A�^�}�S���I��

// ����(�^�}�S���ł���m��[%])
#define LOVE_LV_GOOD   (70)  // �u�Ƃ��Ă� �悢 �悤����v
#define LOVE_LV_NORMAL (50)  // �u�܂��܂��� �悤����v
#define LOVE_LV_BAD    (20)  // �u����ق� �悭�Ȃ����Ȃ��v
#define LOVE_LV_WORST   (0)  // �u������ �|�P���������� ������ł��邪�Ȃ��v


//========================================================================================
// ����ĉ����񃏁[�N
//========================================================================================
struct _SODATEYA
{
  HEAPID           heapID; // �q�[�vID
  FIELDMAP_WORK* fieldmap; // �t�B�[���h�}�b�v
  SODATEYA_WORK*     work; // �Ǘ����[�N
  u16         layEggCount; // �Y���J�E���^
};


//========================================================================================
// ���v���g�^�C�v�錾
//========================================================================================
static void GrowUpPokemon( POKEMON_PARAM* poke, u32 exp );
static void SortSodateyaPokemon( SODATEYA_WORK* work );
static u32 LoveCheck( const POKEMON_PARAM* poke1, const POKEMON_PARAM* poke2 );
static u32 CalcLoveLv_normal( const POKEMON_PARAM* poke1, const POKEMON_PARAM* poke2 );
static BOOL LayEggCheck( SODATEYA* sodateya );
static void CreateEgg( SODATEYA* sodateya, POKEMON_PARAM* egg );
static void EggCordinate_monsno( 
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg );
static void EggCordinate_personality(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg );
static void EggCordinate_special_ability(
    HEAPID heap_id, const POKEMON_PARAM* mother, POKEMON_PARAM* egg );
static void EggCordinate_ability_rand(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg );
static void EggCordinate_rare(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg );
static void EggCordinate_waza_default( POKEMON_PARAM* egg );
static void EggCordinate_waza_egg( 
    HEAPID heap_id, const POKEMON_PARAM* father, POKEMON_PARAM* egg );
static void EggCordinate_waza_parent(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg );
static void EggCordinate_waza_machine(
    HEAPID heap_id, const POKEMON_PARAM* father, POKEMON_PARAM* egg );
static void EggCordinate_pityuu(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg );
static void EggCordinate_karanakusi(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg );
static void EggCordinate_finish( POKEMON_PARAM* egg, SODATEYA* sodateya );
static BOOL IsWazaMachineAbleToUse( HEAPID heap_id, u16 monsno, u16 formno, u16 itemno );
static u16 GetSeedMonsNo( u16 monsno );


//========================================================================================
// ���쐬�E�j��
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief ��ĉ�������쐬����
 *
 * @param heap_id  �g�p����q�[�vID
 * @param fieldmap �֘A�t����t�B�[���h�}�b�v
 * @param work     �Ǘ������ĉ����[�N
 * @return �쐬������ĉ�����
 */
//---------------------------------------------------------------------------------------- 
SODATEYA* SODATEYA_Create( HEAPID heap_id, FIELDMAP_WORK* fieldmap, SODATEYA_WORK* work )
{
  SODATEYA* sodateya;

  // ��ĉ����񃏁[�N�m��
  sodateya = (SODATEYA*)GFL_HEAP_AllocMemory( heap_id, sizeof(SODATEYA) );

  // ���[�N������
  sodateya->heapID      = heap_id;
  sodateya->fieldmap    = fieldmap;
  sodateya->work        = work;
  sodateya->layEggCount = 0;
  return sodateya;
}

//----------------------------------------------------------------------------------------
/**
 * @brief ��ĉ������j������
 *
 * @param sodateya �j�������ĉ�����
 */
//----------------------------------------------------------------------------------------
void SODATEYA_Delete( SODATEYA* sodateya )
{
  GFL_HEAP_FreeMemory( sodateya );
}


//========================================================================================
// ���|�P����, �^�}�S�̎󂯓n��
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �|�P������a����
 *
 * @param sodateya ��ĉ�����
 * @param party    �a����|�P������������p�[�e�B�[
 * @param pos      �a����|�P�������w��
 */
//---------------------------------------------------------------------------------------- 
void SODATEYA_TakePokemon( SODATEYA* sodateya, POKEPARTY* party, int pos )
{
  int index;
  POKEMON_PARAM* poke;

  // �i�[��C���f�b�N�X���擾
  index = SODATEYA_WORK_GetPokemonNum( sodateya->work );

  // ���łɍő吔�a�����Ă���ꍇ
  if( SODATEYA_POKE_MAX <= index )
  {
    OBATA_Printf( "--------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_TakePokemon: ���łɍő吔�̃|�P������a�����Ă��܂��B\n" );
    OBATA_Printf( "--------------------------------------------------------------\n" );
    return;
  }

  // �|�P�������ړ� (�莝������ĉ�)
  poke = PokeParty_GetMemberPointer( party, pos );
  SODATEYA_WORK_SetPokemon( sodateya->work, index, poke );
  PokeParty_Delete( party, pos );
}

//----------------------------------------------------------------------------------------
/**
 * @brief �|�P������Ԃ�
 *
 * @param sodateya ��ĉ�����
 * @param index    �Ԃ��|�P�������w��
 * @param party    �Ԃ���̃p�[�e�B�[
 */
//---------------------------------------------------------------------------------------- 
void SODATEYA_TakeBackPokemon( SODATEYA* sodateya, int index, POKEPARTY* party )
{
  const POKEMON_PARAM* poke;
  u32 exp;

  // �w��C���f�b�N�X�Ƀ|�P���������Ȃ��ꍇ
  if( SODATEYA_WORK_IsValidPokemon( sodateya->work, index ) != TRUE )
  {
    OBATA_Printf( "------------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_TakeBackPokemon: �w��C���f�b�N�X�ɂ͎���|�P�����܂���B\n" );
    OBATA_Printf( "------------------------------------------------------------------\n" );
    return;
  }

  // �|�P�����𐬒�������
  poke = SODATEYA_WORK_GetPokemon( sodateya->work, index );
  exp  = SODATEYA_WORK_GetGrowUpExp( sodateya->work, index );
  GrowUpPokemon( (POKEMON_PARAM*)poke, exp );

  // �|�P�������ړ� (��ĉ����莝��)
  PokeParty_Add( party, poke );
  SODATEYA_WORK_ClrPokemon( sodateya->work, index );
  SortSodateyaPokemon( sodateya->work );
}

//----------------------------------------------------------------------------------------
/**
 * @brief �^�}�S��n��
 *
 * @param sodateya ��ĉ�����
 * @param party    �n����̃p�[�e�B�[
 */
//---------------------------------------------------------------------------------------- 
void SODATEYA_TakeBackEgg( SODATEYA* sodateya, POKEPARTY* party )
{
  POKEMON_PARAM* egg = PP_Create( 1, 1, 1, sodateya->heapID );

  // �^�}�S�����݂��Ȃ��ꍇ
  if( SODATEYA_WORK_IsValidEgg( sodateya->work ) != TRUE )
  {
    OBATA_Printf( "------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_TakeBackEgg: �^�}�S������܂���B\n" );
    OBATA_Printf( "------------------------------------------\n" );
    return;
  }

  // �^�}�S���쐬��, �莝���ɒǉ�
  CreateEgg( sodateya, egg );
  PokeParty_Add( party, egg );

  // �^�}�S���폜
  SODATEYA_WORK_ClrEgg( sodateya->work ); 
  GFL_HEAP_FreeMemory( egg );
}

//----------------------------------------------------------------------------------------
/**
 * @brief �^�}�S��j������
 *
 * @param sodateya ��ĉ�����
 */
//---------------------------------------------------------------------------------------- 
void SODATEYA_DeleteEgg( SODATEYA* sodateya )
{
  // �^�}�S�����݂��Ȃ��ꍇ
  if( SODATEYA_WORK_IsValidEgg( sodateya->work ) != TRUE )
  {
    OBATA_Printf( "------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_TakeBackEgg: �^�}�S������܂���B\n" );
    OBATA_Printf( "------------------------------------------\n" );
  }

  SODATEYA_WORK_ClrEgg( sodateya->work );
}


//========================================================================================
// ����ĉ�����d���֐�
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �|�P��������Ă� (1����)
 *
 * @param sodateya �|�P��������Ă��ĉ�����
 */
//---------------------------------------------------------------------------------------- 
void SODATEYA_BreedPokemon( SODATEYA* sodateya )
{
  // �o���l�����Z
  SODATEYA_WORK_AddGrowUpExp( sodateya->work, EXP_PER_WALK );

  // �Y���`�F�b�N
  if( LayEggCheck(sodateya) == TRUE )
  {
    // �^�}�S���Y�܂ꂽ���Ƃ��L��
    SODATEYA_WORK_SetEgg( sodateya->work );
  }
}


//========================================================================================
// ���擾�֐�
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @breif �^�}�S�̗L�����擾����
 *
 * @param sodateya ��ĉ�����
 * @return �^�}�S������ꍇ TRUE
 */
//----------------------------------------------------------------------------------------
BOOL SODATEYA_HaveEgg( const SODATEYA* sodateya )
{
  return SODATEYA_WORK_IsValidEgg( sodateya->work );
}

//----------------------------------------------------------------------------------------
/**
 * @breif ���璆�̃|�P�����̐����擾����
 *
 * @param sodateya ��ĉ�����
 * @return ���璆�̃|�P������
 */
//----------------------------------------------------------------------------------------
u8 SODATEYA_GetPokemonNum( const SODATEYA* sodateya )
{
  return SODATEYA_WORK_GetPokemonNum( sodateya->work );
}

//----------------------------------------------------------------------------------------
/**
 * @brief ����|�P�����ɂ���, ���݂̃��x�����擾����
 *
 * @param sodateya ��ĉ�����
 * @param index    �ΏۂƂȂ�|�P�������w��
 * @return �w��|�P�����̌����_�̃��x��
 */
//---------------------------------------------------------------------------------------- 
u32 SODATEYA_GetPokeLv_Current( const SODATEYA* sodateya, int index )
{
  u32 monsno, formno;
  u32 before_exp, after_exp;
  u32 before_level, after_level;
  const POKEMON_PARAM* poke;

  // �w��C���f�b�N�X�Ƀ|�P���������Ȃ��ꍇ
  if( SODATEYA_WORK_IsValidPokemon( sodateya->work, index ) != TRUE )
  {
    OBATA_Printf( "--------------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_GetPokeLv_Current: �w��C���f�b�N�X�ɂ͎���|�P�����܂���B\n" );
    OBATA_Printf( "--------------------------------------------------------------------\n" );
    return 1;
  }

  // �w��|�P�����̃p�����[�^���擾
  poke   = SODATEYA_WORK_GetPokemon( sodateya->work, index );
  monsno = PP_Get( poke, ID_PARA_monsno, NULL );
  formno = PP_Get( poke, ID_PARA_form_no, NULL );

  // �a�������̃��x�����擾
  before_exp   = PP_Get( poke, ID_PARA_exp, NULL );
  before_level = PP_Get( poke, ID_PARA_level, NULL );

  // ���݂̃��x�����Z�o
  after_exp   = before_exp + SODATEYA_WORK_GetGrowUpExp( sodateya->work, index );
  after_level = POKETOOL_CalcLevel( monsno, formno, after_exp );

  // ���݂̃��x����Ԃ�
  return after_level;
}

//----------------------------------------------------------------------------------------
/**
 * @brief ����|�P�����ɂ���, ��������x�����擾����
 *
 * @param sodateya ��ĉ�����
 * @param index    �ΏۂƂȂ�|�P�������w��
 * @return �w��|�P�����̐������x����
 */
//---------------------------------------------------------------------------------------- 
u32 SODATEYA_GetPokeLv_GrowUp( const SODATEYA* sodateya, int index )
{
  u32 before_level, after_level;
  const POKEMON_PARAM* poke;

  // �a������ & ���݂̃��x�����擾
  poke         = SODATEYA_WORK_GetPokemon( sodateya->work, index );
  before_level = PP_Get( poke, ID_PARA_level, NULL );
  after_level  = SODATEYA_GetPokeLv_Current( sodateya, index );

  // ��������x����Ԃ�
  return ( after_level - before_level );
}

//----------------------------------------------------------------------------------------
/**
 * @brief ����|�P�����̈�����藿�����擾����
 *
 * @param sodateya ��ĉ�����
 * @param index    �ΏۂƂȂ�|�P�������w��
 * @return �w��|�P�����̈��������z
 */
//---------------------------------------------------------------------------------------- 
u32 SODATEYA_GetCharge( const SODATEYA* sodateya, int index )
{
  u32 grow_level;
  u32 charge;
  
  // �����v�Z (��{�� + ���x����)
  grow_level = SODATEYA_GetPokeLv_GrowUp( sodateya, index );
  charge     = CHARGE_MIN + (grow_level * CHARGE_PER_LEVEL); 
  return charge;
}

//----------------------------------------------------------------------------------------
/**
 * @brief ����|�P�����̑������擾����
 *
 * @param sodateya ��ĉ�����
 * @return ����|�P�������m�̑���[%]
 */
//---------------------------------------------------------------------------------------- 
u8 SODATEYA_GetLoveLevel( const SODATEYA* sodateya )
{
  const POKEMON_PARAM* poke1;
  const POKEMON_PARAM* poke2;

  // ����|�P������2�̂��Ȃ��ꍇ
  if( SODATEYA_WORK_GetPokemonNum( sodateya->work ) < 2 )
  {
    OBATA_Printf( "--------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_GetLoveLevel: �a���|�P������2�̂��܂���B\n" );
    OBATA_Printf( "--------------------------------------------------\n" );
    return LOVE_LV_WORST;
  }

  // �|�P�����p�����[�^�擾
  poke1 = SODATEYA_WORK_GetPokemon( sodateya->work, 0 );
  poke2 = SODATEYA_WORK_GetPokemon( sodateya->work, 1 );

  // ������Ԃ�
  return LoveCheck( poke1, poke2 );
}

//----------------------------------------------------------------------------------------
/**
 * @brief ����|�P�����̗a�����������̃p�����[�^���擾����
 *
 * @param sodateya ��ĉ�����
 * @param index    �ΏۂƂȂ�|�P�������w��
 * @return �w��|�P�����̎���J�n�����̃p�����[�^
 */
//---------------------------------------------------------------------------------------- 
const POKEMON_PARAM* SODATEYA_GetPokemonParam( const SODATEYA* sodateya, int index )
{
  // �w��C���f�b�N�X�Ƀ|�P���������Ȃ��ꍇ
  if( SODATEYA_WORK_IsValidPokemon( sodateya->work, index ) != TRUE )
  {
    OBATA_Printf( "--------------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_GetPokeLv_Current: �w��C���f�b�N�X�ɂ͎���|�P�����܂���B\n" );
    OBATA_Printf( "--------------------------------------------------------------------\n" );
  }

  return SODATEYA_WORK_GetPokemon( sodateya->work, index );
}


#if 0
//----------------------------------------------------------------------------------------
/**
 * @brief ���[�N�擾
 *
 * @param sodateya ��ĉ�����
 * @return ��ĉ����[�N
 */
//---------------------------------------------------------------------------------------- 
SODATEYA_WORK* SODATEYA_GetSodateyaWork( const SODATEYA* sodateya )
{
  return sodateya->work;
}
#endif


//========================================================================================
// ������J�֐�
//========================================================================================

//---------------------------------------------------------------------------------------- 
/**
 * @brief �|�P�����𐬒�������
 *
 * @param poke ����������|�P����
 * @param exp  ���Z����o���l
 */
//---------------------------------------------------------------------------------------- 
static void GrowUpPokemon( POKEMON_PARAM* poke, u32 exp )
{
  int i;
  u32 monsno, formno;
  int before_lv, after_lv;
  POKEPER_WAZAOBOE_CODE waza_table[POKEPER_WAZAOBOE_TABLE_ELEMS];

  // ��{�����擾
  monsno = PP_Get( poke, ID_PARA_monsno, NULL );
  formno = PP_Get( poke, ID_PARA_form_no, NULL );

  // �a�����������̃��x�����擾
  before_lv = PP_Get( poke, ID_PARA_level, NULL ); 

  // �o���l�����Z��, �p�����[�^���Čv�Z
  PP_Add( poke, ID_PARA_exp, exp );
  PP_Renew( poke );

  // ������̃��x�����擾
  after_lv = PP_Get( poke, ID_PARA_level, NULL );

  // �Z���X�V (�����ߒ��Ŋo����Z���v�b�V��)
  POKE_PERSONAL_LoadWazaOboeTable( monsno, formno, waza_table );
  i = 0;
  while( POKEPER_WAZAOBOE_IsEndCode( waza_table[i] ) )
  {
    int waza_lv = POKEPER_WAZAOBOE_GetLevel( waza_table[i] );
    int waza_id = POKEPER_WAZAOBOE_GetWazaID( waza_table[i] );
    if( after_lv < waza_lv ) break;
    if( before_lv < waza_lv ) PP_SetWazaPush( poke, waza_id );
    i++;
  } 
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ����|�P��������בւ��� (�O�l�߂ɂȂ�悤�ɂ���)
 *
 * @param work ��ĉ����[�N
 */
//---------------------------------------------------------------------------------------- 
static void SortSodateyaPokemon( SODATEYA_WORK* work )
{
  int i;
  int j;

  // �󂫗v�f������
  for( i=0; i<SODATEYA_POKE_MAX; i++ )
  {
    // �󂫗v�f�𔭌�������, �ȍ~�̃f�[�^���l�߂�
    if( SODATEYA_WORK_IsValidPokemon( work, i ) != TRUE )
    {
      for( j=i+1; j<SODATEYA_POKE_MAX; j++ )
      {
        if( SODATEYA_WORK_IsValidPokemon( work, j ) == TRUE )
        {
          SODATEYA_WORK_ExchangePokemon( work, j, j-1 );
        }
      }
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �w�肵���|�P�����̑������`�F�b�N����
 *
 * @param poke1 �|�P����1
 * @param poke2 �|�P����2
 *
 * @return ����[%]
 */
//---------------------------------------------------------------------------------------- 
static u32 LoveCheck( const POKEMON_PARAM* poke1, const POKEMON_PARAM* poke2 )
{
  int i;
  u32 monsno[2];
  u32 id_no[2];
  u32 form[2];
  u32 sex[2];
  u32 egg_group1[2];
  u32 egg_group2[2];
  const POKEMON_PARAM* param[2];

  // 2�̂��ꂼ��̃p�����[�^���擾
  param[0] = poke1;
  param[1] = poke2;
  for( i=0; i<2; i++ )
  {
    monsno[i]     = PP_Get( param[i], ID_PARA_monsno, NULL );
    id_no[i]      = PP_Get( param[i], ID_PARA_id_no, NULL );
    form[i]       = PP_Get( param[i], ID_PARA_form_no, NULL );
    sex[i]        = PP_Get( param[i], ID_PARA_sex, NULL );
    egg_group1[i] = POKETOOL_GetPersonalParam( monsno[i], form[i], POKEPER_ID_egg_group1 );
    egg_group2[i] = POKETOOL_GetPersonalParam( monsno[i], form[i], POKEPER_ID_egg_group2 );
  }

  //---------------------------
  // ���Â���O���[�v�w�����B�x
  //---------------------------
  // �ǂ��炩�̃^�}�S�O���[�v���w�����B�x==> �~
  if( ( egg_group1[0] == POKEPER_EGG_GROUP_MUSEISYOKU ) ||
      ( egg_group1[1] == POKEPER_EGG_GROUP_MUSEISYOKU ) ||
      ( egg_group2[0] == POKEPER_EGG_GROUP_MUSEISYOKU ) ||
      ( egg_group2[1] == POKEPER_EGG_GROUP_MUSEISYOKU )  )
  {
    return LOVE_LV_WORST;
  }

  //----------
  // ���^����
  //----------
  // �����Ƃ����^���� ==> �~
  if( ( monsno[0] == MONSNO_METAMON ) &&
      ( monsno[1] == MONSNO_METAMON ) )
  {
    return LOVE_LV_WORST;
  }
  // �ǂ��炩��������^���� ==> �ʏ픻��
  else if( ( monsno[0] == MONSNO_METAMON ) ||
           ( monsno[1] == MONSNO_METAMON ) )
  {
    return CalcLoveLv_normal( poke1, poke2 );
  }

  //-------
  // ����
  //-------
  // ���� ==> �~
  if( sex[0] == sex[1] ) return LOVE_LV_WORST;

  // �ǂ��炩�����ʂȂ� ==> �~
  if( ( sex[0] == PTL_SEX_UNKNOWN ) ||
      ( sex[1] == PTL_SEX_UNKNOWN ) )
  {
    return LOVE_LV_WORST;
  }

  //---------------------------------
  // ���Â���O���[�v�w�����B�x�ȊO
  //---------------------------------
  // �O���[�v���Ⴄ ==> �~
  if( ( egg_group1[0] != egg_group1[1] ) && 
      ( egg_group1[0] != egg_group2[1] ) &&
      ( egg_group2[0] != egg_group1[1] ) &&
      ( egg_group2[0] != egg_group2[1] )  )
  {
    return LOVE_LV_WORST;
  }
  // �O���[�v������ ==> �ʏ픻��
  else 
  {
    return CalcLoveLv_normal( poke1, poke2 );
  }

  // �R�R�ɏ����͗��Ȃ��n�Y!!
  OBATA_Printf( "-------------------------------------\n" );
  OBATA_Printf( "LoveCheck: �Ӑ}���Ȃ������t���[����!!\n" );
  OBATA_Printf( "-------------------------------------\n" );
  return LOVE_LV_WORST;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �w�肵���|�P�����̑��������߂� (�����^�}�S�O���[�v�̏ꍇ)
 *
 * [���[��]
 *  ���� and ��ID then ��
 *  ���� and ��ID then ��
 *  �َ� and ��ID then ��
 *  �َ� and ��ID then ��
 *
 * @param poke1 �|�P����1
 * @param poke2 �|�P����2
 *
 * @return ����[%]
 */
//---------------------------------------------------------------------------------------- 
static u32 CalcLoveLv_normal( const POKEMON_PARAM* poke1, const POKEMON_PARAM* poke2 )
{
  int i;
  u32 monsno[2];
  u32 id_no[2];
  const POKEMON_PARAM* param[2];

  // 2�̂��ꂼ��̃p�����[�^���擾
  param[0] = poke1;
  param[1] = poke2;
  for( i=0; i<2; i++ )
  {
    monsno[i]     = PP_Get( param[i], ID_PARA_monsno, NULL );
    id_no[i]      = PP_Get( param[i], ID_PARA_id_no, NULL );
  }

  // �|�P�����̎�ނ��Ⴄ�ꍇ
  if( monsno[0] != monsno[1] ) 
  {
    if( id_no[0] == id_no[1] ) // ID������ ==> ��
    {
      return LOVE_LV_BAD;
    }
    else                       // ID���Ⴄ ==> ��
    {
      return LOVE_LV_NORMAL;
    }
  }
  // �|�P�����̎�ނ������ꍇ
  else 
  {
    if( id_no[0] == id_no[1] ) // ID������ ==> ��
    {
      return LOVE_LV_NORMAL;
    }
    else                       // ID���Ⴄ ==> ��
    {
      return LOVE_LV_GOOD;
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �Y���`�F�b�N
 *
 * @param sodateya �`�F�b�N�����ĉ�����
 * @return �^�}�S���Y�܂��ꍇ TRUE
 */
//---------------------------------------------------------------------------------------- 
static BOOL LayEggCheck( SODATEYA* sodateya )
{
  // ���Ԋu�Ŕ���
  if( EGG_CHECK_INTERVAL < sodateya->layEggCount++ )
  {
    // �Y���J�E���^�����Z�b�g
    sodateya->layEggCount = 0;

    // 2�̗a���Ă�����Y������
    if( 2 <= SODATEYA_WORK_GetPokemonNum( sodateya->work ) )
    { 
      // ���� < ���� ==> �Y��
      if( GFL_STD_MtRand(100) < SODATEYA_GetLoveLevel(sodateya) ) 
      {
        return TRUE; 
      }
    }
  }

  // �Y�܂Ȃ�
  return FALSE;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S���쐬����
 *
 * @param sodateya ��ĉ�����
 * @param egg      �쐬�����^�}�S�p�����[�^�̊i�[��
 */
//---------------------------------------------------------------------------------------- 
static void CreateEgg( SODATEYA* sodateya, POKEMON_PARAM* egg )
{
  int i;
  HEAPID heap_id = sodateya->heapID;
  const POKEMON_PARAM* father = NULL;
  const POKEMON_PARAM* mother = NULL;

  // ���E��|�P�������擾
  for( i=0; i<2; i++ )
  {
    const POKEMON_PARAM* pp = SODATEYA_WORK_GetPokemon( sodateya->work, i );
    u32 sex = PP_GetSex( pp );
    switch( sex )
    {
    case PTL_SEX_MALE:   father = pp; break; 
    case PTL_SEX_FEMALE: mother = pp; break;
    default:
      if( !father ) father = pp;
      if( !mother ) mother = pp;
      break;
    }
  }

  // �^�}�S���쐬
  EggCordinate_monsno( father, mother, egg );              // �����X�^�[�i���o�[
  EggCordinate_personality( father, mother, egg );         // ���i
  EggCordinate_special_ability( heap_id, mother, egg );    // ����
  EggCordinate_ability_rand( father, mother, egg );        // �̗���
  EggCordinate_rare( father, mother, egg );                // ���A�|�P���I
  EggCordinate_waza_default( egg );                        // �Z (�f�t�H���g)
  EggCordinate_waza_egg( heap_id, father, egg );           // �Z (�^�}�S�Z)
  EggCordinate_waza_parent( father, mother, egg );         // �Z (���e���ʂ̋Z)
  EggCordinate_waza_machine( heap_id, father, egg );       // �Z (���̃}�V���Z)
  EggCordinate_pityuu( father, mother, egg );              // ����^�}�S (�s�`���[)
  EggCordinate_karanakusi( father, mother, egg );          // ����^�}�S (�J���i�N�V)
  EggCordinate_finish( egg, sodateya );                    // �d�グ
}

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
static void EggCordinate_personality(
    const POKEMON_PARAM* father, const POKEMON_PARAM* mother, POKEMON_PARAM* egg )
{
  BOOL kawarazu_f; // ���e���w����炸�̂����x�������Ă��邩�ǂ���
  BOOL kawarazu_m; // ��e���w����炸�̂����x�������Ă��邩�ǂ���
  u32 personal_f = PP_Get( father, ID_PARA_personal_rnd, NULL );    // ���e�̌�����
  u32 personal_m = PP_Get( mother, ID_PARA_personal_rnd, NULL );    // ��e�̌�����

  //�w����炸�̂����x�������Ă��邩�ǂ������擾
  kawarazu_f = ( PP_Get( father, ID_PARA_item, NULL ) == ITEM_KAWARAZUNOISI );
  kawarazu_m = ( PP_Get( mother, ID_PARA_item, NULL ) == ITEM_KAWARAZUNOISI );

  //�w����炸�̂����x�������Ă�����, ���������p��
  if( kawarazu_f && kawarazu_m ) // ���e�Ƃ������Ă���
  {
    // 1/2�Ōp������
    if( GFL_STD_MtRand(100)%2 == 0 )
    {
      if( GFL_STD_MtRand(100)%2 == 0 )  // 1/2�ŕ��e����p��
      {
        PP_Put( egg, ID_PARA_personal_rnd, personal_f );
      }
      else                             // 1/2�ŕ�e����p��
      {
        PP_Put( egg, ID_PARA_personal_rnd, personal_m );
      }
    }
  }
  else if( kawarazu_f ) // ���e�̂ݎ����Ă��� ==> 1/2�Ōp��
  {
    if( GFL_STD_MtRand(100)%2 == 0 )
    {
      PP_Put( egg, ID_PARA_personal_rnd, personal_f );
    }
  }
  else if( kawarazu_m ) // ��e�̂ݎ����Ă��� ==> 1/2�Ōp��
  {
    if( GFL_STD_MtRand(100)%2 == 0 ) 
    {
      PP_Put( egg, ID_PARA_personal_rnd, personal_m );
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̓��������肷��
 *        ��e�̓�������`����
 *
 * @param heap_id �g�p����q�[�vID
 * @param mother  ��e�|�P����
 * @param egg     �ݒ肷��^�}�S
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_special_ability(
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
    OBATA_Printf( "----------------------------------\n" );
    OBATA_Printf( "��|�P�����̓����Ɍ�肪����܂��B\n" );
    OBATA_Printf( "----------------------------------\n" );
    speabi_index = 0; // �Ƃ肠����, ����1���p�����邱�Ƃɂ���
  }

  // �p���������������
  rnd = GFL_STD_MtRand(100);  // �����擾[0, 99]
  switch( speabi_index )
  {
  case 0: //---------------------------------------------- �ꂪ����1
    if( rnd < 80 ) speabi_egg = speabi_list[0]; // 80% ==> �q�̓���1
    else           speabi_egg = speabi_list[1]; // 20% ==> �q�̓���2
    break;
  case 1: //---------------------------------------------- �ꂪ����2
    if( rnd < 20 ) speabi_egg = speabi_list[0]; // 20% ==> �q�̓���1
    else           speabi_egg = speabi_list[1]; // 80% ==> �q�̓���2
    break;
  case 2: //---------------------------------------------- �ꂪ����3
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
      ( GFL_STD_MtRand(100)%2 == 0 ) )    // ���e�������Ă����ꍇ, 1/2�ŏ㏑������
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
    ability[i++] = GFL_STD_MtRand(PTL_ABILITY_MAX);

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
    if( GFL_STD_MtRand(100)%2 == 0 ) pp = father;
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
    rnd = GFL_STD_MtRand( 0xffffffff );
    if( POKETOOL_CheckRare( id, rnd ) ) break;  // ���A���o���璊�I�I��
  }

  // ������������������
  PP_Put( egg, ID_PARA_personal_rnd, rnd );
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
    for( itemno=ITEM_WAZAMASIN01; itemno<=ITEM_WAZAMASIN92; itemno++ )
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
 * @param sodateya ��ĉ�����
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_finish( POKEMON_PARAM* egg, SODATEYA* sodateya )
{
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( sodateya->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS* mystatus = GAMEDATA_GetMyStatus( gdata );

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
    STRBUF* name = MyStatus_CreateNameString( mystatus, sodateya->heapID );
    PP_Put( egg, ID_PARA_oyaname, (u32)name );
    GFL_STR_DeleteBuffer( name );
  } 

  // �^�}�S�t���O
  PP_Put( egg, ID_PARA_tamago_flag, TRUE );
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
    OBATA_Printf( "------------------------------------------------------------------\n" );
    OBATA_Printf( "IsWazaMachineAbleToUse: �����ȃ����X�^�[�i���o�[���w�肳��܂����B\n" );
    OBATA_Printf( "------------------------------------------------------------------\n" );
    return FALSE;
  } 
  // �Z�}�V���ȊO�̃A�C�e�����w�肳�ꂽ�ꍇ
  if( (itemno < ITEM_WAZAMASIN01 ) || (ITEM_HIDENMASIN08 < itemno ) )
  {
    OBATA_Printf( "----------------------------------------------------------------\n" );
    OBATA_Printf( "IsWazaMachineAbleToUse: �Z�}�V���ȊO�̃A�C�e�����w�肳��܂����B\n" );
    OBATA_Printf( "----------------------------------------------------------------\n" );
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
    OBATA_Printf( "---------------------------------------------------------\n" );
    OBATA_Printf( "GetSeedMonsNo: �����ȃ����X�^�[�i���o�[���w�肳��܂����B\n" );
    OBATA_Printf( "---------------------------------------------------------\n" );
    return monsno; // �Ƃ肠����, ���̂܂ܕԂ�
  } 

  // ��|�P����No���擾
  GFL_ARC_LoadDataOfs( &seed_monsno, ARCID_PMSTBL, monsno, 0, sizeof(u16) );
  return seed_monsno;
} 
