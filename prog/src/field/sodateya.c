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
#include "gamesystem/game_data.h"
#include "savedata/mystatus.h"
#include "savedata/perapvoice.h"
#include "savedata/sodateya_work.h" 
#include "print/global_msg.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/poke_personal.h"
#include "poke_tool/poke_memo.h"
#include "waza_tool/wazano_def.h" // for WAZANO_xxxx 

#include "fieldmap.h"
#include "pokemon_egg.h" 

#include "arc/arc_def.h" // for ARCID_PMSTBL

#include "sodateya.h" 

//========================================================================================
// �� �萔
//========================================================================================
#define CHARGE_MIN         (100) // �ŏ�����
#define CHARGE_PER_LEVEL   (100) // 1���x�����̈�����藿��
#define EXP_PER_WALK         (1) // �������тɉ��Z����o���l
#define EGG_CHECK_INTERVAL (256) // �Y������̕p�x

// ����(�^�}�S���ł���m��[%])
#define LOVE_LV_GOOD   (70)  // �u�Ƃ��Ă� �悢 �悤����v
#define LOVE_LV_NORMAL (50)  // �u�܂��܂��� �悤����v
#define LOVE_LV_BAD    (20)  // �u����ق� �悭�Ȃ����Ȃ��v
#define LOVE_LV_WORST   (0)  // �u������ �|�P���������� ������ł��邪�Ȃ��v


//========================================================================================
// ����ĉ����񃏁[�N
//========================================================================================
struct _SODATEYA {

  HEAPID         heapID;      // �q�[�vID
  FIELDMAP_WORK* fieldmap;    // �t�B�[���h�}�b�v
  SODATEYA_WORK* work;        // �Ǘ����[�N
  u16            layEggCount; // �Y���J�E���^

};


//========================================================================================
// ���v���g�^�C�v�錾
//========================================================================================
static u32 CalcExpAdd( u32 exp1, u32 exp2 );
static void GrowUpPokemon( POKEMON_PARAM* poke, u32 exp );
static void LearnNewWaza( POKEMON_PARAM* poke, u32 wazano );
static BOOL CheckWazaHave( const POKEMON_PARAM* poke, u32 wazano );
static void SortSodateyaPokemon( SODATEYA_WORK* work );
static u32 LoveCheck( const POKEMON_PARAM* poke1, const POKEMON_PARAM* poke2 );
static u32 CalcLoveLv_normal( const POKEMON_PARAM* poke1, const POKEMON_PARAM* poke2 );
static BOOL LayEggCheck( SODATEYA* sodateya );
static void CreateEgg( SODATEYA* sodateya, POKEMON_PARAM* egg );
static void DeletePerapVoice( FIELDMAP_WORK* fieldmap, POKEPARTY* party );


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
    OBATA_Printf( "SODATEYA_TakePokemon: ���łɍő吔�̃|�P������a�����Ă��܂��B\n" );
    return;
  }

  // �|�P�������ړ� (�莝������ĉ�)
  poke = PokeParty_GetMemberPointer( party, pos );
  SODATEYA_WORK_SetPokemon( sodateya->work, index, poke );
  PokeParty_Delete( party, pos );

  // �؃��b�v�^���f�[�^���폜
  DeletePerapVoice( sodateya->fieldmap, party );
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
    OBATA_Printf( "SODATEYA_TakeBackPokemon: �w��C���f�b�N�X�ɂ͎���|�P�����܂���B\n" );
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
    OBATA_Printf( "SODATEYA_TakeBackEgg: �^�}�S������܂���B\n" );
    return;
  }
  GAMEBEACON_Set_SodateyaEgg();

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
    OBATA_Printf( "SODATEYA_TakeBackEgg: �^�}�S������܂���B\n" );
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
BOOL SODATEYA_BreedPokemon( SODATEYA* sodateya )
{
  // �o���l�����Z
  SODATEYA_WORK_AddGrowUpExp( sodateya->work, EXP_PER_WALK );

  // �Y���`�F�b�N
  if( LayEggCheck(sodateya) == TRUE )
  {
    // �^�}�S���Y�܂ꂽ���Ƃ��L��
    SODATEYA_WORK_SetEgg( sodateya->work );
    return TRUE;
  }
  return FALSE;
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
  u64 before_exp, after_exp;
  u32 before_level, after_level;
  const POKEMON_PARAM* poke;

  // �w��C���f�b�N�X�Ƀ|�P���������Ȃ��ꍇ
  if( SODATEYA_WORK_IsValidPokemon( sodateya->work, index ) != TRUE )
  {
    OBATA_Printf( "SODATEYA_GetPokeLv_Current: �w��C���f�b�N�X�ɂ͎���|�P�����܂���B\n" );
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
  after_exp   = CalcExpAdd( before_exp, SODATEYA_WORK_GetGrowUpExp( sodateya->work, index ) );
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
    OBATA_Printf( "SODATEYA_GetLoveLevel: �a���|�P������2�̂��܂���B\n" );
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
    OBATA_Printf( "SODATEYA_GetPokemonParam: �w��C���f�b�N�X�ɂ͎���|�P�����܂���B\n" );
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
 * @brief �o���l�̉��Z���s��
 *
 * @param exp1 �o���l1
 * @param exp2 �o���l2
 *
 * @return �o���l1 + �o���l2
 *        ( u32 �ŃI�[�o�[�t���[���N�������ꍇ �ő�l�ł��� 0xffffffff ��Ԃ� )
 */
//---------------------------------------------------------------------------------------- 
static u32 CalcExpAdd( u32 exp1, u32 exp2 )
{
  u64 sum;

  sum = (u64)exp1 + (u64)exp2;

  // ���Z�l��␳
  if( 0xffffffff < sum )
  {
    sum = 0xffffffff;
  } 
  return sum;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �|�P�����𐬒�������
 *
 * @param poke ����������|�P����
 * @param addExp  ���Z����o���l
 */
//---------------------------------------------------------------------------------------- 
static void GrowUpPokemon( POKEMON_PARAM* poke, u32 addExp )
{
  int i;
  u32 monsno, formno, exp;
  int before_lv, after_lv;
  POKEPER_WAZAOBOE_CODE waza_table[POKEPER_WAZAOBOE_TABLE_ELEMS];

  // ��{�����擾
  monsno = PP_Get( poke, ID_PARA_monsno, NULL );
  formno = PP_Get( poke, ID_PARA_form_no, NULL );

  // �a�����������̃��x�����擾
  before_lv = PP_Get( poke, ID_PARA_level, NULL ); 

  // �o���l�����Z��, �p�����[�^���Čv�Z
  exp = PP_Get( poke, ID_PARA_exp, NULL );
  exp = CalcExpAdd( exp, addExp );
  PP_Put( poke, ID_PARA_exp, exp );
  PP_Renew( poke );

  // ������̃��x�����擾
  after_lv = PP_Get( poke, ID_PARA_level, NULL );

  // �Z���X�V ( �����ߒ��Ŋo����Z���K�� )
  POKE_PERSONAL_LoadWazaOboeTable( monsno, formno, waza_table );
  i = 0;
  while( !POKEPER_WAZAOBOE_IsEndCode( waza_table[i] ) )
  {
    int waza_lv = POKEPER_WAZAOBOE_GetLevel( waza_table[i] );
    int waza_id = POKEPER_WAZAOBOE_GetWazaID( waza_table[i] );
    if( after_lv < waza_lv ) { break; }
    if( before_lv < waza_lv ) { LearnNewWaza( poke, waza_id ); }
    i++;
  } 
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �Z���o��������
 *
 * @param poke
 * @param wazano
 */
//---------------------------------------------------------------------------------------- 
static void LearnNewWaza( POKEMON_PARAM* poke, u32 wazano )
{
  int pos;
  u32 param_id[ PTL_WAZA_MAX ] = 
  {
    ID_PARA_waza1,
    ID_PARA_waza2,
    ID_PARA_waza3,
    ID_PARA_waza4,
  };

  if( wazano == WAZANO_NULL ) { return; } // �Z�ԍ����s��
  if( CheckWazaHave(poke, wazano) ) { return; } // ���łɊo���Ă���

  // �󂫗v�f�ɒǉ�
  for( pos=0; pos < PTL_WAZA_MAX; pos++ )
  {
    u32 waza = PP_Get( poke, param_id[ pos ], NULL );

    if( waza == WAZANO_NULL ) {
      PP_SetWazaPos( poke, wazano, pos );
      return;
    }
  }

  // �󂫂��Ȃ���΃v�b�V������
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
  u32 id_para[ PTL_WAZA_MAX ] = 
  { 
    ID_PARA_waza1, 
    ID_PARA_waza2, 
    ID_PARA_waza3, 
    ID_PARA_waza4 
  };

  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    if( PP_Get( poke, id_para[i], NULL ) == wazano ) {
      return TRUE;
    }
  }

  return FALSE;
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

    // �^�}�S�O���[�v�̕s���l�����o
    GF_ASSERT( egg_group1[i] != POKEPER_EGG_GROUP_NONE );
    GF_ASSERT( egg_group2[i] != POKEPER_EGG_GROUP_NONE );
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
  OBATA_Printf( "LoveCheck: �Ӑ}���Ȃ������t���[����!!\n" );
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
      if( GFUser_GetPublicRand0(100) < SODATEYA_GetLoveLevel(sodateya) ) 
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
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( sodateya->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS* mystatus = GAMEDATA_GetMyStatus( gdata ); 
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
  POKEMON_EGG_Create( 
      sodateya->heapID, mystatus, POKE_MEMO_PERSON_SODATEYA, father, mother, egg );
}



//========================================================================================
// ���|�P�����ʏ���
//========================================================================================

//---------------------------------------------------------------------------------------- 
/**
 * @brief �؃��b�v�́u������ׂ�v�f�[�^���폜����
 *
 * @param sodateya
 */
//---------------------------------------------------------------------------------------- 
static void DeletePerapVoice( FIELDMAP_WORK* fieldmap, POKEPARTY* party )
{
  GAMESYS_WORK* gameSystem;
  GAMEDATA* gameData;
  PERAPVOICE* perapVoice;

  gameSystem = FIELDMAP_GetGameSysWork( fieldmap );
  gameData = GAMESYSTEM_GetGameData( gameSystem );
  perapVoice = GAMEDATA_GetPerapVoice( gameData );

  PERAPVOICE_CheckPerapInPokeParty( perapVoice, party );
}
