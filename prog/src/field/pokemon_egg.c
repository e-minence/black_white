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
#include "poke_tool/status_rcv.h"
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

#include "bugfix.h"

#define DEBUG_PRINT_ON      // �f�o�b�O�o�̓X�C�b�`
#define PRINT_TARGET    (2) // �f�o�b�O�o�͐�
#define RARE_EGG_CHANCE (5) // ���A�^�}�S���I��

// ���e�|�P����
typedef struct {
  const POKEMON_PARAM* father;
  const POKEMON_PARAM* mother;
} PARENT;

// �^�}�S�̃p�����[�^
typedef struct {
  u32 monsno;
  u32 formno;
  u32 level;
  u32 seikaku;
  u32 tokusei;
  u32 friend;
  u32 ability_rnd[ PTL_ABILITY_MAX ];
  u32 id_no;
  u32 personal_rnd;
  u32 waza_num;
  u32 wazano[ PTL_WAZA_MAX ];

  u32 tokusei_idx; // ���Ԗڂ̓�����[0, 2]
  BOOL derive_seikaku_from_father;
  BOOL derive_seikaku_from_mother;
  BOOL derive_ability_from_father[ PTL_ABILITY_MAX ];
  BOOL derive_ability_from_mother[ PTL_ABILITY_MAX ];
  u32 derive_ability_num;
} EGG_PARAM;


// prototype /////////////////////////////////////////////////////////////////////////////

// ����̌���
static void CorrectParent_bySex( PARENT* parent );
static void CorrectParent_byMetamon( PARENT* parent );
static void ReverseParent( PARENT* parent );
// �^�}�S�p�����[�^�̌���
static void EggCordinate_init( EGG_PARAM* egg );
static void EggCordinate_MonsNo_basic( const PARENT* parent, EGG_PARAM* egg ); // �����X�^�[No.
static void EggCordinate_MonsNo_nidoran( const PARENT* parent, EGG_PARAM* egg );
static void EggCordinate_MonsNo_barubiito( const PARENT* parent, EGG_PARAM* egg );
static void EggCordinate_MonsNo_fione( const PARENT* parent, EGG_PARAM* egg );
static void EggCordinate_MonsNo_baby( const PARENT* parent, EGG_PARAM* egg );
static const POKEMON_PARAM* GetBasePokemon_for_monsno( const PARENT* parent );
static void EggCordinate_Form_basic( const PARENT* parent, EGG_PARAM* egg ); // �t�H����
static void EggCordinate_Form_karanakusi( const PARENT* parent, EGG_PARAM* egg );
static void EggCordinate_Form_minomutti( const PARENT* parent, EGG_PARAM* egg );
static void EggCordinate_Form_basurao( const PARENT* parent, EGG_PARAM* egg );
static const POKEMON_PARAM* GetBasePokemon_for_form( const PARENT* parent );
static void EggCordinate_Seikaku_basic( const PARENT* parent, EGG_PARAM* egg ); // ���i
static void EggCordinate_Seikaku_kawarazunoisi( const PARENT* parent, EGG_PARAM* egg );
static void EggCordinate_TokuseiIndex_basic( const PARENT* parent, EGG_PARAM* egg, HEAPID heap_id ); // ����
static void EggCordinate_TokuseiIndex_metamon( const PARENT* parent, EGG_PARAM* egg );
static void EggCordinate_Tokusei( const PARENT* parent, EGG_PARAM* egg, HEAPID heap_id );
static void EggCordinate_Friend( EGG_PARAM* egg, HEAPID heap_id ); // �Ȃ��x ( �z���J�E���g )
static void EggCordinate_AbilityRnd_select_by_item( const PARENT* parent, EGG_PARAM* egg ); // �̗���
static void EggCordinate_AbilityRnd_select_at_random( const PARENT* parent, EGG_PARAM* egg );
static void EggCordinate_AbilityRnd_deirve_from_parent( const PARENT* parent, EGG_PARAM* egg );
static void EggCordinate_ID( const MYSTATUS* mystatus, EGG_PARAM* egg ); // ID
static void EggCordinate_PersonalRnd_at_random( const PARENT* parent, EGG_PARAM* egg ); // ������
static void EggCordinate_PersonalRnd_for_rare( const PARENT* parent, EGG_PARAM* egg );
static u32 GetPersonalRnd_at_random( void );
static void EggCordinate_Waza_default( EGG_PARAM* egg ); // �f�t�H���g�Z�Z�b�g
static void EggCordinate_Waza_egg( const PARENT* parent, EGG_PARAM* egg, HEAPID heap_id ); // �^�}�S�Z�Z�b�g
static void EggCordinate_Waza_parent( const PARENT* parent, EGG_PARAM* egg ); // �p���Z ( ���e ) �Z�b�g
static void EggCordinate_Waza_machine( const PARENT* parent, EGG_PARAM* egg, HEAPID heap_id ); // �p���Z ( �Z�}�V�� ) �Z�b�g
static void EggCordinate_Waza_pityuu( const PARENT* parent, EGG_PARAM* egg ); // ����^�}�S���� ( �s�`���[ )
static void PushWaza_to_EggParam( EGG_PARAM* egg, u32 wazano );
static void MakeEgg( POKEMON_PARAM* egg, const EGG_PARAM* egg_param, const MYSTATUS* owner, int memo, HEAPID heap_id );
// ���[�e�B���e�B
static BOOL IsWazaMachineAbleToUse( HEAPID heap_id, u16 monsno, u16 formno, u16 itemno ); // �Z�}�V�����g���邩�ǂ������`�F�b�N����
static u16 GetSeedMonsNo( u16 monsno ); // ��|�P�������擾����
static u8 GetTokuseiIndex( u16 monsno, u16 formno, u16 tokusei, HEAPID heap_id ); // ���Ԗڂ̓����Ȃ̂����擾����
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
  EGG_PARAM egg_param;

  // ���������
  parent.father = poke1;
  parent.mother = poke2;
  CorrectParent_bySex( &parent );
  CorrectParent_byMetamon( &parent );

  // �^�}�S�̃p�����[�^������
  EggCordinate_init( &egg_param );
  EggCordinate_MonsNo_basic( &parent, &egg_param ); // �����X�^�[�i���o�[
  EggCordinate_MonsNo_nidoran( &parent, &egg_param );
  EggCordinate_MonsNo_barubiito( &parent, &egg_param );
  EggCordinate_MonsNo_fione( &parent, &egg_param );
  EggCordinate_MonsNo_baby( &parent, &egg_param ); 
  EggCordinate_Form_basic( &parent, &egg_param ); // �t�H����
  EggCordinate_Form_karanakusi( &parent, &egg_param ); 
  EggCordinate_Form_minomutti( &parent, &egg_param );
  EggCordinate_Form_basurao( &parent, &egg_param );
  EggCordinate_Seikaku_basic( &parent, &egg_param ); // ���i
  EggCordinate_Seikaku_kawarazunoisi( &parent, &egg_param ); 
  EggCordinate_TokuseiIndex_basic( &parent, &egg_param, heap_id ); // ����
  EggCordinate_TokuseiIndex_metamon( &parent, &egg_param );
  EggCordinate_Tokusei( &parent, &egg_param, heap_id );
  EggCordinate_Friend( &egg_param, heap_id ); // �Ȃ��x ( �z���J�E���g )
  EggCordinate_AbilityRnd_select_by_item( &parent, &egg_param ); // �̗���
  EggCordinate_AbilityRnd_select_at_random( &parent, &egg_param );
  EggCordinate_AbilityRnd_deirve_from_parent( &parent, &egg_param );
  EggCordinate_ID( mystatus, &egg_param ); // ID
  EggCordinate_PersonalRnd_at_random( &parent, &egg_param ); // ������
  EggCordinate_PersonalRnd_for_rare( &parent, &egg_param ); 
  EggCordinate_Waza_default( &egg_param ); // �Z ( �f�t�H���g ) 
  EggCordinate_Waza_egg( &parent, &egg_param, heap_id ); // �Z ( �^�}�S�Z )
  EggCordinate_Waza_parent( &parent, &egg_param ); // �Z ( ���e���ʂ̋Z ) 
  EggCordinate_Waza_machine( &parent, &egg_param, heap_id ); // �Z ( ���̃}�V���Z ) 
  EggCordinate_Waza_pityuu( &parent, &egg_param ); // �Z ( �s�`���[ )

  // �^�}�S���쐬
  MakeEgg( egg, &egg_param, mystatus, memo, heap_id ); 
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

  // ���e�`�F�b�N
  sex_father = PP_Get( parent->father, ID_PARA_sex, NULL );
  sex_mother = PP_Get( parent->mother, ID_PARA_sex, NULL ); 
  if( sex_father == PTL_SEX_FEMALE ) {
    ReverseParent( parent );
  }

  // ��e�`�F�b�N
  sex_father = PP_Get( parent->father, ID_PARA_sex, NULL );
  sex_mother = PP_Get( parent->mother, ID_PARA_sex, NULL ); 
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
 * @brief �^�}�S�̃p�����[�^������������
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_init( EGG_PARAM* egg )
{
  GFL_STD_MemClear( egg, sizeof(EGG_PARAM) );
  egg->level = 1;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃����X�^�[�i���o�[�����肷�� ( ��{���[�� )
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_MonsNo_basic( const PARENT* parent, EGG_PARAM* egg )
{
  const POKEMON_PARAM* base_poke;
  u32 base_monsno;

  base_poke   = GetBasePokemon_for_monsno( parent );
  base_monsno = PP_Get( base_poke, ID_PARA_monsno, NULL );
  egg->monsno = GetSeedMonsNo( base_monsno );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃����X�^�[�i���o�[�����肷�� ( ��O�P�[�X�F�j�h���� )
 *
 * �j�h������
 * �j�h������
 * �j�h���[�m
 * �j�h�L���O�@�����, �j�h������ ( 50% ) or �j�h������ ( 50% ) ���Y�܂��
 *
 * ���j�h���[�i
 * �@�j�h�N�C���@�̓^�}�S�O���[�v���w�����B�x�Ȃ̂�, �^�}�S�͔�������Ȃ�
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_MonsNo_nidoran( const PARENT* parent, EGG_PARAM* egg )
{
  const POKEMON_PARAM* base_poke;
  u32 base_monsno;

  base_poke   = GetBasePokemon_for_monsno( parent );
  base_monsno = PP_Get( base_poke, ID_PARA_monsno, NULL );

  if( (base_monsno == MONSNO_NIDORAN_F) ||
      (base_monsno == MONSNO_NIDORAN_M) ||
      (base_monsno == MONSNO_NIDORIINO) ||
      (base_monsno == MONSNO_NIDOKINGU) ) {
    if( GFUser_GetPublicRand0(2) == 0 ) {
      egg->monsno = MONSNO_NIDORAN_F;
    }
    else {
      egg->monsno = MONSNO_NIDORAN_M;
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃����X�^�[�i���o�[�����肷�� ( ��O�P�[�X�F�o���r�[�g, �C���~�[�[ )
 *
 * �o���r�[�g
 * �C���~�[�[ �����, �o���r�[�g ( 50% ) or �C���~�[�[ ( 50% ) ���Y�܂��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_MonsNo_barubiito( const PARENT* parent, EGG_PARAM* egg )
{
  const POKEMON_PARAM* base_poke;
  u32 base_monsno;

  base_poke   = GetBasePokemon_for_monsno( parent );
  base_monsno = PP_Get( base_poke, ID_PARA_monsno, NULL );

  if( (base_monsno == MONSNO_BARUBIITO) ||
      (base_monsno == MONSNO_IRUMIIZE) ) {
    if( GFUser_GetPublicRand0(2) == 0 ) {
      egg->monsno = MONSNO_BARUBIITO;
    }
    else {
      egg->monsno = MONSNO_IRUMIIZE;
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃����X�^�[�i���o�[�����肷�� ( ��O�P�[�X�F�t�B�I�l )
 *
 * �}�i�t�B�̎q�̓t�B�I�l
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_MonsNo_fione( const PARENT* parent, EGG_PARAM* egg )
{
  const POKEMON_PARAM* base_poke;
  u32 base_monsno;

  base_poke   = GetBasePokemon_for_monsno( parent );
  base_monsno = PP_Get( base_poke, ID_PARA_monsno, NULL );

  if( base_monsno == MONSNO_MANAFI ) {
    egg->monsno = MONSNO_FIONE;
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃����X�^�[�i���o�[�����肷�� ( ��O�P�[�X�F�׃r�B�|�P���� )
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_MonsNo_baby( const PARENT* parent, EGG_PARAM* egg )
{
  int i;
  const POKEMON_PARAM* base_poke;
  u32 base_monsno, base_itemno;
  u32 father_item, mother_item;

  const int table_size = 12;
  const u32 exception_table[ table_size ][3] = // ��O�e�[�u��
  {
    // �׃r�B�|�P����  // �K�{�A�C�e��      // �q�|�P����
    {MONSNO_RURIRI,    ITEM_USIONOOKOU,     MONSNO_MARIRU},
    {MONSNO_SOONANO,   ITEM_NONKINOOKOU,    MONSNO_SOONANSU},
    {MONSNO_MANENE,    ITEM_AYASIIOKOU,     MONSNO_BARIYAADO},
    {MONSNO_USOHATI,   ITEM_GANSEKIOKOU,    MONSNO_USOKKII},
    {MONSNO_GONBE,     ITEM_MANPUKUOKOU,    MONSNO_KABIGON},
    {MONSNO_TAMANTA,   ITEM_SAZANAMINOOKOU, MONSNO_MANTAIN},
    {MONSNO_SUBOMII,   ITEM_OHANANOOKOU,    MONSNO_ROZERIA},
    {MONSNO_PINPUKU,   ITEM_KOUUNNOOKOU,    MONSNO_RAKKII},
    {MONSNO_RIISYAN,   ITEM_KIYOMENOOKOU,   MONSNO_TIRIIN},
  };

  base_poke   = GetBasePokemon_for_monsno( parent );
  base_monsno = PP_Get( base_poke, ID_PARA_monsno, NULL );
  base_itemno = PP_Get( base_poke, ID_PARA_item, NULL );

  father_item = PP_Get( parent->father, ID_PARA_item, NULL );
  mother_item = PP_Get( parent->mother, ID_PARA_item, NULL );

  // ��O�e�[�u���Ƃ̏ƍ�
  for( i=0; i<table_size; i++ )
  {
    if( (egg->monsno == exception_table[i][0]) &&
        (father_item != exception_table[i][1]) &&
        (mother_item != exception_table[i][1]) ) {
      egg->monsno = exception_table[i][2];
      break;
    }
  }
}

/**
 * @brief �q�|�P�����̃����X�^�[No.����̃x�[�X�ƂȂ�e�|�P�������擾����
 */
static const POKEMON_PARAM* GetBasePokemon_for_monsno( const PARENT* parent )
{
  u32 mother_monsno = PP_Get( parent->mother, ID_PARA_monsno, NULL );

  // ��e�����^���� ==> ���e
  if( mother_monsno == MONSNO_METAMON ) {
    return parent->father;
  }
  // �ʏ� ==> ��e
  else {
    return parent->mother;
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃t�H���������肷�� ( ��{���[�� )
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Form_basic( const PARENT* parent, EGG_PARAM* egg )
{
  egg->formno = 0;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃t�H���������肷�� ( ��O�P�[�X�F�J���i�N�V )
 *
 * ��e���w�J���i�N�V�xor�w�g���g�h���x�Ȃ�, �t�H�������p��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Form_karanakusi( const PARENT* parent, EGG_PARAM* egg )
{
  const POKEMON_PARAM* base_poke;
  u32 base_monsno;

  base_poke   = GetBasePokemon_for_form( parent );
  base_monsno = PP_Get( base_poke, ID_PARA_monsno, NULL );

  if( (base_monsno == MONSNO_KARANAKUSI) ||
      (base_monsno == MONSNO_TORITODON) ) { 
    egg->formno = PP_Get( base_poke, ID_PARA_form_no, NULL );
  }
}


//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃t�H���������肷�� ( ��O�P�[�X�F�~�m���b�` )
 *
 * ��e���w�~�m���b�`�xor�w�~�m�}�_���x�Ȃ�, �t�H�������p��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Form_minomutti( const PARENT* parent, EGG_PARAM* egg )
{
  const POKEMON_PARAM* base_poke;
  u32 base_monsno;

  base_poke   = GetBasePokemon_for_form( parent );
  base_monsno = PP_Get( base_poke, ID_PARA_monsno, NULL );

  if( (base_monsno == MONSNO_MINOMUTTI) ||
      (base_monsno == MONSNO_MINOMADAMU) ) { 
    egg->formno = PP_Get( base_poke, ID_PARA_form_no, NULL );
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̃t�H���������肷�� ( ��O�P�[�X�F�o�X���I )
 *
 * ��e���w�o�X���I�x�Ȃ�, �t�H�������p��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Form_basurao( const PARENT* parent, EGG_PARAM* egg )
{
  const POKEMON_PARAM* base_poke;
  u32 base_monsno;

  base_poke   = GetBasePokemon_for_form( parent );
  base_monsno = PP_Get( base_poke, ID_PARA_monsno, NULL );

  if( base_monsno == MONSNO_BASURAO ) {
    egg->formno = PP_Get( base_poke, ID_PARA_form_no, NULL );
  }
}

/**
 * @brief �q�|�P�����̃t�H��������̃x�[�X�ƂȂ�e�|�P�������擾����
 */
static const POKEMON_PARAM* GetBasePokemon_for_form( const PARENT* parent )
{
  u32 mother_monsno = PP_Get( parent->mother, ID_PARA_monsno, NULL );

  // ��e�����^���� ==> ���e
  if( mother_monsno == MONSNO_METAMON ) {
    return parent->father;
  }
  // �ʏ� ==> ��e
  else {
    return parent->mother;
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̐��i�����肷�� ( ��{���[�� )
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Seikaku_basic( const PARENT* parent, EGG_PARAM* egg )
{
  egg->seikaku = GFUser_GetPublicRand0( PTL_SEIKAKU_MAX );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̐��i�����肷�� ( ��O�P�[�X�F����炸�̂��� )
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Seikaku_kawarazunoisi( const PARENT* parent, EGG_PARAM* egg )
{
  BOOL father_have_kawarazunoisi;
  BOOL mother_have_kawarazunoisi;
  u32 father_seikaku;
  u32 mother_seikaku;

  // ���e�̃f�[�^���擾
  father_have_kawarazunoisi = ( PP_Get( parent->father, ID_PARA_item, NULL ) == ITEM_KAWARAZUNOISI );
  mother_have_kawarazunoisi = ( PP_Get( parent->mother, ID_PARA_item, NULL ) == ITEM_KAWARAZUNOISI );
  father_seikaku = PP_Get( parent->father, ID_PARA_seikaku, NULL );
  mother_seikaku = PP_Get( parent->mother, ID_PARA_seikaku, NULL );

  // �ǂ��炩�������Ă��Ă�, 1/2�Ōp�����Ȃ�
  if( father_have_kawarazunoisi || mother_have_kawarazunoisi ) {
    if( GFUser_GetPublicRand0(2) == 0 ) {
      return;
    }
  }

  // �p��
  if( father_have_kawarazunoisi && mother_have_kawarazunoisi ) { // ���e�Ƃ�����
    if( GFUser_GetPublicRand0(2) == 0 )  {
      egg->seikaku = father_seikaku;
      egg->derive_seikaku_from_father = TRUE;
    }
    else {
      egg->seikaku = mother_seikaku;
      egg->derive_seikaku_from_mother = TRUE;
    }
  }
  else if( father_have_kawarazunoisi ) { // ���e�̂ݏ���
    egg->seikaku = father_seikaku;
    egg->derive_seikaku_from_father = TRUE;
  }
  else if( mother_have_kawarazunoisi ) { // ��e�̂ݎ���
    egg->seikaku = mother_seikaku;
    egg->derive_seikaku_from_mother = TRUE;
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�����Ԗڂ̓����������肷�� ( ��{���[�� )
 *
 * ��e�̓����ԍ����p�����悤�Ƃ���
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_TokuseiIndex_basic( const PARENT* parent, EGG_PARAM* egg, HEAPID heap_id )
{
  u32 rnd;
  u32 mother_tokusei_idx;

  // ��|�P�����̓����C���f�b�N�X���擾
  {
    u32 mother_monsno  = PP_Get( parent->mother, ID_PARA_monsno, NULL );
    u32 mother_formno  = PP_Get( parent->mother, ID_PARA_form_no, NULL );
    u32 mother_tokusei = PP_Get( parent->mother, ID_PARA_speabino, NULL );
    mother_tokusei_idx = GetTokuseiIndex( mother_monsno, mother_formno, mother_tokusei, heap_id );
  }

  rnd = GFUser_GetPublicRand0(100);  // �����擾[0, 99]

  // �����ԍ�������
  switch( mother_tokusei_idx ) {
  default:
  // �ꂪ����1
  case 0:
    if( rnd < 80 ) { egg->tokusei_idx = 0; } // 80% ==> �q�̓���1
    else           { egg->tokusei_idx = 1; } // 20% ==> �q�̓���2
    break;

  // �ꂪ����2
  case 1: 
    if( rnd < 20 ) { egg->tokusei_idx = 0; } // 20% ==> �q�̓���1
    else           { egg->tokusei_idx = 1; } // 80% ==> �q�̓���2
    break;

  // �ꂪ����3
  case 2: 
    if( rnd < 20 )      { egg->tokusei_idx = 0; } // 20% ==> �q�̓���1
    else if( rnd < 40 ) { egg->tokusei_idx = 1; } // 20% ==> �q�̓���2
    else                { egg->tokusei_idx = 2; } // 60% ==> �q�̓���3
    break;
  } 
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�����Ԗڂ̓����������肷�� ( ��O���[���F���^���� )
 *
 * ���e�̂ǂ��炩�����^�����̏ꍇ, �^�}�S�̓�����
 * 50% ==> ����1
 * 50% ==> ����2 
 * �ɂȂ�
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_TokuseiIndex_metamon( const PARENT* parent, EGG_PARAM* egg )
{
  u32 father_monsno = PP_Get( parent->father, ID_PARA_monsno, NULL );
  u32 mother_monsno = PP_Get( parent->mother, ID_PARA_monsno, NULL );

  if( (father_monsno == MONSNO_METAMON) || (mother_monsno == MONSNO_METAMON) ) {
    u32 rnd = GFUser_GetPublicRand0(2); // �����擾
    if( rnd == 0 ) {
      egg->tokusei_idx = 0;
    }
    else {
      egg->tokusei_idx = 1;
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̓��������肷��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Tokusei( const PARENT* parent, EGG_PARAM* egg, HEAPID heap_id )
{
  u32 tokusei_list[3];

  // �������X�g ( ��̓��� ) ���擾
  {
    POKEMON_PERSONAL_DATA* personalData;
    personalData   = POKE_PERSONAL_OpenHandle( egg->monsno, egg->formno, heap_id );
    tokusei_list[0] = POKE_PERSONAL_GetParam( personalData, POKEPER_ID_speabi1 );
    tokusei_list[1] = POKE_PERSONAL_GetParam( personalData, POKEPER_ID_speabi2 );
    tokusei_list[2] = POKE_PERSONAL_GetParam( personalData, POKEPER_ID_speabi3 );
    POKE_PERSONAL_CloseHandle( personalData );
  }

  if( egg->tokusei_idx < 2 ) { // ������ or ������

    egg->tokusei = tokusei_list[ egg->tokusei_idx ]; 

    // ���肵���ԍ��ɓ������Ȃ��ꍇ, ����0�ɂ���
    if( egg->tokusei == TOKUSYU_NULL ) {
      egg->tokusei = tokusei_list[0];
    } 
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̂Ȃ��x ( �z���J�E���g ) �����肷��
 *
 * �^�}�S�̊Ԃ�, �Ȃ��x��z���J�E���^�Ƃ��ė��p����
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Friend( EGG_PARAM* egg, HEAPID heap_id )
{
  u32 birth_count;
  POKEMON_PERSONAL_DATA* personal;

  // �|�P�������Ƃ̛z���������擾
  personal = POKE_PERSONAL_OpenHandle( egg->monsno, egg->formno, heap_id );
  birth_count = POKE_PERSONAL_GetParam( personal, POKEPER_ID_egg_birth );
  POKE_PERSONAL_CloseHandle( personal );

  egg->friend = birth_count;
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̌̗��������肷�� ( �A�C�e������ )
 *
 * �p���[xxxx�n�̃A�C�e�����������Ă���ꍇ, 
 * �A�C�e���ɑΉ�����̗����𗼐e����v�P�p������
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_AbilityRnd_select_by_item( const PARENT* parent, EGG_PARAM* egg )
{
  int i;
  BOOL father_have_spitem = FALSE;
  BOOL mother_have_spitem = FALSE;
  int derive_ability_idx_from_father;
  int derive_ability_idx_from_mother;
  u32 father_item = PP_Get( parent->father, ID_PARA_item, NULL );
  u32 mother_item = PP_Get( parent->mother, ID_PARA_item, NULL );
  u32 sp_items[ PTL_ABILITY_MAX ] = 
  {
    ITEM_PAWAAUEITO,
    ITEM_PAWAARISUTO,
    ITEM_PAWAABERUTO,
    ITEM_PAWAARENZU,
    ITEM_PAWAABANDO,
    ITEM_PAWAAANKURU,
  };

  // ���e�̏����A�C�e�����`�F�b�N
  for( i=0; i<PTL_ABILITY_MAX; i++ )
  {
    if( father_item == sp_items[i] ) {
      father_have_spitem = TRUE;
      derive_ability_idx_from_father = i;
      break;
    }
  }

  // ��e�̏����A�C�e�����`�F�b�N
  for( i=0; i<PTL_ABILITY_MAX; i++ )
  {
    if( mother_item == sp_items[i] ) {
      mother_have_spitem = TRUE;
      derive_ability_idx_from_mother = i;
      break;
    }
  }

  if( father_have_spitem && mother_have_spitem ) { // ���e�Ƃ�����
    if( GFUser_GetPublicRand0(2) == 0 ) { 
      egg->derive_ability_from_father[ derive_ability_idx_from_father ] = TRUE;
      egg->derive_ability_num++;
    }
    else {
      egg->derive_ability_from_mother[ derive_ability_idx_from_mother ] = TRUE;
      egg->derive_ability_num++;
    }
  }
  else if( father_have_spitem ) { // ���e�݂̂�����
    egg->derive_ability_from_father[ derive_ability_idx_from_father ] = TRUE;
    egg->derive_ability_num++;
  }
  else if( mother_have_spitem ) { // ��e�݂̂�����
    egg->derive_ability_from_mother[ derive_ability_idx_from_mother ] = TRUE;
    egg->derive_ability_num++;
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̌̗��������肷�� ( ��{���[�� )
 *
 * ���e����v�R�̌̗����������_���Ɍp������
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_AbilityRnd_select_at_random( const PARENT* parent, EGG_PARAM* egg )
{
  while( egg->derive_ability_num < 3 )
  {
    int derive_ability_idx = GFUser_GetPublicRand0( PTL_ABILITY_MAX );

    if( GFUser_GetPublicRand0(2) == 0 ) {
      if( (egg->derive_ability_from_father[ derive_ability_idx ] == FALSE) &&
          (egg->derive_ability_from_mother[ derive_ability_idx ] == FALSE) ) {
        egg->derive_ability_from_father[ derive_ability_idx ] = TRUE;
        egg->derive_ability_num++;
      }
    }
    else {
      if( (egg->derive_ability_from_father[ derive_ability_idx ] == FALSE) &&
          (egg->derive_ability_from_mother[ derive_ability_idx ] == FALSE) ) {
        egg->derive_ability_from_mother[ derive_ability_idx ] = TRUE;
        egg->derive_ability_num++;
      }
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̌̗��������肷��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_AbilityRnd_deirve_from_parent( const PARENT* parent, EGG_PARAM* egg )
{
  int i;
  u32 param_id[ PTL_ABILITY_MAX ] =
  {
    ID_PARA_hp_rnd,
    ID_PARA_pow_rnd,
    ID_PARA_def_rnd,
    ID_PARA_spepow_rnd,
    ID_PARA_spedef_rnd,
    ID_PARA_agi_rnd,
  };

  for( i=0; i<PTL_ABILITY_MAX; i++ )
  {
    if( egg->derive_ability_from_father[i] ) { 
      egg->ability_rnd[i] = PP_Get( parent->father, param_id[i], NULL );
    }
    if( egg->derive_ability_from_mother[i] ) {
      egg->ability_rnd[i] = PP_Get( parent->mother, param_id[i], NULL );
    }
  } 
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S��ID�����肷��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_ID( const MYSTATUS* mystatus, EGG_PARAM* egg )
{
  egg->id_no = MyStatus_GetID( mystatus );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̌����������肷�� ( ��{���[�� )
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_PersonalRnd_at_random( const PARENT* parent, EGG_PARAM* egg )
{
  egg->personal_rnd = GetPersonalRnd_at_random();
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�̌����������肷�� ( ���A���I )
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_PersonalRnd_for_rare( const PARENT* parent, EGG_PARAM* egg )
{
  int i;
  u32 father_country = PP_Get( parent->father, ID_PARA_country_code, NULL );
  u32 mother_country = PP_Get( parent->mother, ID_PARA_country_code, NULL );

  // �e���m�̍��R�[�h������
  if( father_country == mother_country ) { 
    return; 
  } 
  // ���łɃ��A
  if( POKETOOL_CheckRare( egg->id_no, egg->personal_rnd ) ) { 
    return; 
  }

  // ���I
  for( i=0; i<RARE_EGG_CHANCE; i++ )
  {
    egg->personal_rnd = GetPersonalRnd_at_random();

    // ���A�ɂȂ���
    if( POKETOOL_CheckRare( egg->id_no, egg->personal_rnd ) ) { 
      return; 
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �����_���Ȍ̗������擾����
 */
//---------------------------------------------------------------------------------------- 
static u32 GetPersonalRnd_at_random( void )
{
  return GFUser_GetPublicRand0( 0xffffffff );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S�Ƀf�t�H���g�Z���o��������
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Waza_default( EGG_PARAM* egg )
{
  int i;
  POKEPER_WAZAOBOE_CODE wazaTable[ POKEPER_WAZAOBOE_TABLE_ELEMS ];

  // Lv.1 �Ŋo����Z���Z�b�g
  POKE_PERSONAL_LoadWazaOboeTable( egg->monsno, egg->formno, wazaTable );
  i = 0;
  while( POKEPER_WAZAOBOE_IsEndCode( wazaTable[i] ) == FALSE )
  {
    int waza_lv = POKEPER_WAZAOBOE_GetLevel( wazaTable[i] );
    int waza_id = POKEPER_WAZAOBOE_GetWazaID( wazaTable[i] );
    if( egg->level < waza_lv ) { 
      break; 
    }
    PushWaza_to_EggParam( egg, waza_id );
    i++;
  }
}

//----------------------------------------------------------------------------------------
/**
 * @brief �^�}�S�Z���o��������
 *
 * �^�}�S����z��|�P�����݂̂��o������Z�̂���, ���|�P�������o���Ă���Z���K��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Waza_egg( const PARENT* parent, EGG_PARAM* egg, HEAPID heap_id )
{
  int i, j;
  u16 kowaza_num;
  u16* kowaza_table;
  u32 id_para[ PTL_WAZA_MAX ] = 
  { 
    ID_PARA_waza1, 
    ID_PARA_waza2, 
    ID_PARA_waza3, 
    ID_PARA_waza4 
  };

  // �q�Z�e�[�u�����擾
  kowaza_table = GFL_ARC_LoadDataAlloc( ARCID_KOWAZA, egg->monsno, heap_id );
  kowaza_num   = kowaza_table[0];

  // ���e�̑S�Z���`�F�b�N����
  for( i=0; i<PTL_WAZA_MAX; i++ )
  {
    u32 wazano = PP_Get( parent->father, id_para[i], NULL );

    // ���e�̋Z���^�}�S�Z�Ȃ�, �K������
    for( j=1; j<=kowaza_num; j++ )
    {
      if( wazano == kowaza_table[j] ) {
        PushWaza_to_EggParam( egg, wazano );
        break;
      }
    }
  }

  // �q�Z�e�[�u����j��
  GFL_HEAP_FreeMemory( kowaza_table );
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ���e�̋Z���󂯌p��
 *
 * ���������x�����グ�邱�ƂŊo����Z�̂���, ���e�Ƃ��Ɋo���Ă���Z���K��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Waza_parent( const PARENT* parent, EGG_PARAM* egg )
{
  int i, j;
  POKEPER_WAZAOBOE_CODE wazaTable[ POKEPER_WAZAOBOE_TABLE_ELEMS ]; // �q�̋Z�o���e�[�u��
  u32 id_para[ PTL_WAZA_MAX ] = 
  { 
    ID_PARA_waza1, 
    ID_PARA_waza2, 
    ID_PARA_waza3, 
    ID_PARA_waza4 
  };

  // �q�̋Z�o���e�[�u�����擾
  POKE_PERSONAL_LoadWazaOboeTable( egg->monsno, egg->formno, wazaTable );

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
      PushWaza_to_EggParam( egg, wazano );
    }

    // ���̋Z��
    i++;
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ���|�P�����̃}�V���Z���󂯌p��
 *
 * �������o���邱�Ƃ̏o����}�V���Z�̂���, ���|�P�������o���Ă���Z���K��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Waza_machine( const PARENT* parent, EGG_PARAM* egg, HEAPID heap_id )
{
  int i, itemno;
  u32 wazano;
  u32 id_para[ PTL_WAZA_MAX ] = 
  { 
    ID_PARA_waza1, 
    ID_PARA_waza2, 
    ID_PARA_waza3, 
    ID_PARA_waza4 
  };

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
          ( IsWazaMachineAbleToUse(heap_id, egg->monsno, egg->formno, itemno) ) ) {
        PushWaza_to_EggParam( egg, wazano );
        break;
      }
    }
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief ����^�}�S�쐬���� ( �s�`���[ )
 *
 * �e�|�P���A�C�e���w�ł񂫂��܁x�������Ă�����, �Z�w�{���e�b�J�[�x���K��
 */
//---------------------------------------------------------------------------------------- 
static void EggCordinate_Waza_pityuu( const PARENT* parent, EGG_PARAM* egg )
{
  if( egg->monsno != MONSNO_PITYUU ) { return; }

  // ���e�̂ǂ��炩���w�ł񂫂��܁x�������Ă�����,�w�{���e�b�J�[�x���K��
  if( ( PP_Get( parent->father, ID_PARA_item, NULL ) == ITEM_DENKIDAMA ) ||
      ( PP_Get( parent->mother, ID_PARA_item, NULL ) == ITEM_DENKIDAMA ) ) {
    PushWaza_to_EggParam( egg, WAZANO_BORUTEKKAA );
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �Z�������o�����ɒǉ�����
 */
//---------------------------------------------------------------------------------------- 
static void PushWaza_to_EggParam( EGG_PARAM* egg, u32 wazano )
{
  int i; 

  // �Z�ԍ����s��
  if( wazano == WAZANO_NULL ) { return; }

  // ���łɊo���Ă���
  for( i=0; i<egg->waza_num; i++ )
  {
    if( egg->wazano[i] == wazano ) { return; }
  }

  // push
  if( egg->waza_num < PTL_WAZA_MAX ) {
    egg->wazano[ egg->waza_num ] = wazano;
    egg->waza_num++;
  }
  else {
    for( i=0; i < PTL_WAZA_MAX - 1; i++ )
    {
      egg->wazano[i] = egg->wazano[i+1];
    }
    egg->wazano[i] = wazano;
  }
}

//---------------------------------------------------------------------------------------- 
/**
 * @brief �^�}�S���쐬����
 *
 * @param egg      �ݒ肷��^�}�S
 * @param mystatus �^�}�S�̏��L��
 * @param memo     �g���[�i�[�����w��
 * @param heap_id  �q�[�vID
 */
//---------------------------------------------------------------------------------------- 
static void MakeEgg(
    POKEMON_PARAM* egg, const EGG_PARAM* egg_param, 
    const MYSTATUS* owner, int memo, HEAPID heap_id )
{
  int i;
  u32 ability_rnd_param_id[ PTL_ABILITY_MAX ] =
  {
    ID_PARA_hp_rnd,
    ID_PARA_pow_rnd,
    ID_PARA_def_rnd,
    ID_PARA_spepow_rnd,
    ID_PARA_spedef_rnd,
    ID_PARA_agi_rnd,
  };

  PP_SetupEx( egg, egg_param->monsno, egg_param->level, 
      egg_param->id_no, PTL_SETUP_POW_AUTO, egg_param->personal_rnd );

  PP_Put( egg, ID_PARA_form_no, egg_param->formno ); 
  PP_Put( egg, ID_PARA_seikaku, egg_param->seikaku );
  PP_Put( egg, ID_PARA_friend, egg_param->friend );

  // ����
  if( egg_param->tokusei_idx == 2 ) { // ��O����
    PP_SetTokusei3( egg, egg_param->monsno, egg_param->formno );
  }
  else {
#ifdef  BUGFIX_BTS7955_20100728
    PP_Put( egg, ID_PARA_end, egg_param->tokusei );
#else
    PP_Put( egg, ID_PARA_speabino, egg_param->tokusei );
#endif
  }

  // �̗���
  for( i=0; i<PTL_ABILITY_MAX; i++ )
  {
    if( egg_param->derive_ability_from_father[i] || 
        egg_param->derive_ability_from_mother[i] ) {
      PP_Put( egg, ability_rnd_param_id[i], egg_param->ability_rnd[i] ); 
    }
  }

  // �Z
  PP_Put( egg, ID_PARA_waza1, egg_param->wazano[0] );
  PP_Put( egg, ID_PARA_waza2, egg_param->wazano[1] );
  PP_Put( egg, ID_PARA_waza3, egg_param->wazano[2] );
  PP_Put( egg, ID_PARA_waza4, egg_param->wazano[3] );

  // �e�̖��O
  {
    STRBUF* name = MyStatus_CreateNameString( owner, heap_id );
    PP_Put( egg, ID_PARA_oyaname, (u32)name );
    GFL_STR_DeleteBuffer( name );
  } 

  // �j�b�N�l�[�� ( �^�}�S )
  {
    STRBUF* name = GFL_MSG_CreateString( GlobalMsg_PokeName, MONSNO_TAMAGO );
    PP_Put( egg, ID_PARA_nickname, (u32)name );
    GFL_STR_DeleteBuffer( name );
  } 

  // �g���[�i�[����
  POKE_MEMO_SetTrainerMemoPP( egg, POKE_MEMO_EGG_FIRST, owner, memo, heap_id );

  // �d�グ
  PP_Put( egg, ID_PARA_tamago_flag, TRUE ); 
  PP_RecoverWazaPPAll( egg );
  PP_Renew( egg );
  STATUS_RCV_PokeParam_RecoverAll( egg );
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

//---------------------------------------------------------------------------------------- 
/**
 * @brief ���Ԗڂ̓����Ȃ̂����擾����
 */
//---------------------------------------------------------------------------------------- 
static u8 GetTokuseiIndex( u16 monsno, u16 formno, u16 tokusei, HEAPID heap_id )
{
  u32 tokusei_idx;
  u32 tokusei_list[3];

  // �������X�g ( ��̓��� ) ���擾
  {
    POKEMON_PERSONAL_DATA* personalData;
    personalData = POKE_PERSONAL_OpenHandle( monsno, formno, heap_id );
    tokusei_list[0] = POKE_PERSONAL_GetParam( personalData, POKEPER_ID_speabi1 );
    tokusei_list[1] = POKE_PERSONAL_GetParam( personalData, POKEPER_ID_speabi2 );
    tokusei_list[2] = POKE_PERSONAL_GetParam( personalData, POKEPER_ID_speabi3 );
    POKE_PERSONAL_CloseHandle( personalData );
  }
  
  // �����ԍ��𒲂ׂ�
  for( tokusei_idx=0; tokusei_idx<3; tokusei_idx++ )
  {
    if( tokusei == tokusei_list[ tokusei_idx ] ) { 
      break; 
    }
  }

  // �s���ȓ����������ꍇ
  if( 3 <= tokusei_idx ) { 
    GF_ASSERT(0);
    tokusei_idx = 0; // �s���l�Ȃ����1�Ƃ���
  }

  return tokusei_idx;
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
