//=============================================================================================
/**
 * @file  btl_pokeparam.c
 * @brief �|�P����WB �o�g���V�X�e��  �Q���|�P�����퓬�p�f�[�^
 * @author  taya
 *
 * @date  2008.10.08  �쐬
 */
//=============================================================================================
#include <gflib.h>
#include "poke_tool/poke_tool.h"
#include "waza_tool/wazadata.h"
#include "item/itemsym.h"

#include "btl_common.h"
#include "btl_calc.h"
#include "btl_sick.h"

#include "btl_pokeparam.h"



/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {

  RANK_CRITICAL_MIN = 0,
  RANK_CRITICAL_MAX = 4,
  RANK_CRITICAL_DEF = 0,

  TURNFLG_BUF_SIZE = (BPP_TURNFLG_MAX/8)+(BPP_TURNFLG_MAX%8!=0),
  CONTFLG_BUF_SIZE = (BPP_CONTFLG_MAX/8)+(BPP_CONTFLG_MAX%8!=0),

  TURNCOUNT_NULL = BTL_TURNCOUNT_MAX+1,

  WAZADMG_REC_TURN_MAX = BPP_WAZADMG_REC_TURN_MAX,  ///< ���U�_���[�W���R�[�h�F���^�[�����̋L�^����邩�H
  WAZADMG_REC_MAX = BPP_WAZADMG_REC_MAX,            ///< ���U�_���[�W���R�[�h�F�P�^�[���ɂ��A�������܂ŋL�^���邩�H
};


//--------------------------------------------------------------
/**
 *  �R�A�p�����[�^  - �ւ񂵂񂵂Ă����������Ȃ��v�f -
 */
//--------------------------------------------------------------
typedef struct {
  const POKEMON_PARAM*  ppSrc;
  const POKEMON_PARAM*  ppFake;
  u32   exp;
  u16   monsno;
  u16   hpMax;        ///< �ő�HP
  u16   hp;
  u16   item;
  u16   usedItem;
  u8    myID;
  u8    defaultFormNo;
  u8    fHensin;
  u8    fFakeEnable;
}BPP_CORE_PARAM;

//--------------------------------------------------------------
/**
 *  ��{�p�����[�^
 */
//--------------------------------------------------------------
typedef struct {

  u16 monsno;       ///< �|�P�����i���o�[

  u16 attack;       ///< ��������
  u16 defence;      ///< �ڂ�����
  u16 sp_attack;    ///< �Ƃ�����
  u16 sp_defence;   ///< �Ƃ��ڂ�
  u16 agility;      ///< ���΂₳

  u8  level;        ///< ���x��
  u8  type1;        ///< �^�C�v�P
  u8  type2;        ///< �^�C�v�Q
  u8  sex;          ///< ����

}BPP_BASE_PARAM;

//--------------------------------------------------------------
/**
 *  �����N����
 */
//--------------------------------------------------------------
typedef struct {

  s8  attack;     ///< ��������
  s8  defence;    ///< �ڂ�����
  s8  sp_attack;  ///< �Ƃ�����
  s8  sp_defence; ///< �Ƃ��ڂ�
  s8  agility;    ///< ���΂₳
  s8  hit;        ///< ������
  s8  avoid;      ///< ���

}BPP_VARIABLE_PARAM;

//--------------------------------------------------------------
/**
 *  ���U
 */
//--------------------------------------------------------------
typedef struct {
  u16  number;            ///< ���U�i���o�[
  u16  recoverNumber;     ///< �o�g�����Ƀ��U����������������ɁA�����߂��p�ɑO�̃��U��ۑ�
  u8   pp;                ///< PP�l
  u8   ppMax;             ///< PP�ő�l
  u8   usedFlag;          ///< �g�p�����t���O�i���S�E����ւ��ȂǂŃN���A�j
  u8   usedFlagFix;       ///< �g�p�����t���O�i���S�E����ւ��Ȃǂł��ێ��j
}BPP_WAZA;


struct _BTL_POKEPARAM {

  BPP_CORE_PARAM      coreParam;

  BPP_BASE_PARAM      baseParam;
  BPP_VARIABLE_PARAM  varyParam;
  BPP_WAZA            waza[ PTL_WAZA_MAX ];

  u16  tokusei;
  u16  weight;

  BPP_SICK_CONT   sickCont[ WAZASICK_MAX ];
  u8  wazaSickCounter[ WAZASICK_MAX ];
  u8  wazaCnt;
  u8  formNo;
  u8  criticalRank;
  u8  usedWazaCount;

  u16 turnCount;        ///< �p�����Đ퓬�ɏo�Ă���J�E���^
  u16 appearedTurn;     ///< �퓬�ɏo���^�[�����L�^
  u16 prevSelectWazaID;       ///< ���O�ɑI���������UID�iWAZALOCK��ԓ��ŋ����I�ɑI�����ꂽ���U > �N���C�A���g���I�������������U�j
  u16 prevActWazaID;          ///< ���O�Ɏ��s���ꂽ���UID�i�h�����U���g�����ꍇ��prevSelectWazaID�ƈقȂ�j
  u16 wazaContCounter;        ///< �����U�A�������J�E���^�iprevActWazaID�̃��U������A���Ő������������B���O���s�Ȃ�[���j
  u16 prevDamagedWaza;        ///< ���O�Ƀ_���[�W���󂯂����U
  BtlPokePos  prevTargetPos;  ///< ���O�ɑ_��������

  u8  turnFlag[ TURNFLG_BUF_SIZE ];
  u8  contFlag[ CONTFLG_BUF_SIZE ];
  u8  counter[ BPP_COUNTER_MAX ];
  BPP_WAZADMG_REC  wazaDamageRec[ WAZADMG_REC_TURN_MAX ][ WAZADMG_REC_MAX ];
  u8               dmgrecCount[ WAZADMG_REC_TURN_MAX ];
  u8               dmgrecTurnPtr;
  u8               dmgrecPtr;

  u8  confrontRecCount;
  u8  confrontRec[ BTL_POKEID_MAX ];

  u16 migawariHP;

  WazaID  combiWazaID;
  u8      combiPokeID;
//  u32 dmy;
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void setupBySrcData( BTL_POKEPARAM* bpp, const POKEMON_PARAM* srcPP, BOOL fReflectHP );
static void setupBySrcDataBase( BTL_POKEPARAM* bpp, const POKEMON_PARAM* srcPP );
static void clearUsedWazaFlag( BTL_POKEPARAM* bpp );
static void clearCounter( BTL_POKEPARAM* bpp );
static void Effrank_Init( BPP_VARIABLE_PARAM* rank );
static void Effrank_Reset( BPP_VARIABLE_PARAM* rank );
static BOOL Effrank_Recover( BPP_VARIABLE_PARAM* rank );
static void splitTypeCore( const BTL_POKEPARAM* bpp, PokeType* type1, PokeType* type2 );
static BppValueID ConvertValueID( const BTL_POKEPARAM* bpp, BppValueID vid );
static const s8* getRankVaryStatusConst( const BTL_POKEPARAM* bpp, BppValueID type, s8* min, s8* max );
static s8* getRankVaryStatus( BTL_POKEPARAM* pp, BppValueID type, s8* min, s8* max );
static void cureDependSick( BTL_POKEPARAM* bpp, WazaSick sickID  );
static void clearWazaSickWork( BTL_POKEPARAM* bpp, BOOL fPokeSickInclude );
static void dmgrecClearWork( BTL_POKEPARAM* bpp );
static void dmgrecFwdTurn( BTL_POKEPARAM* bpp );
static inline void flgbuf_clear( u8* buf, u32 size );
static inline void flgbuf_set( u8* buf, u32 flagID );
static inline void flgbuf_reset( u8* buf, u32 flagID );
static inline BOOL flgbuf_get( const u8* buf, u32 flagID );
static void ConfrontRec_Clear( BTL_POKEPARAM* bpp );
static inline BOOL IsMatchTokusei( const BTL_POKEPARAM* bpp, PokeTokusei tokusei );
static inline BOOL IsMatchItem( const BTL_POKEPARAM* bpp, u16 itemID );



//=============================================================================================
/**
 * �o�g���p�|�P�����p�����[�^����
 *
 * @param   pp      �Q�[�����ėp�|�P�����p�����[�^
 * @param   pokeID    �o�g���Q���|�PID�i�o�g���ɎQ�����Ă���|�P�����Ɋ���U���ӂ̐��l�j
 * @param   heapID
 *
 * @retval  BTL_POKEPARAM*
 */
//=============================================================================================
BTL_POKEPARAM*  BTL_POKEPARAM_Create( const POKEMON_PARAM* pp, u8 pokeID, HEAPID heapID )
{
  BTL_POKEPARAM* bpp = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_POKEPARAM) );

  bpp->coreParam.myID = pokeID;
  bpp->coreParam.monsno = PP_Get( pp, ID_PARA_monsno, 0 );
  bpp->coreParam.ppSrc = pp;
  bpp->coreParam.hp = PP_Get( pp, ID_PARA_hp, 0 );
  bpp->coreParam.hpMax = PP_Get( pp, ID_PARA_hpmax, 0 );
  bpp->coreParam.item = PP_Get( pp, ID_PARA_item, NULL );
  bpp->coreParam.usedItem = ITEM_DUMMY_DATA;
  bpp->coreParam.fHensin = FALSE;
  bpp->coreParam.ppFake = NULL;
  bpp->coreParam.fFakeEnable = NULL;
  bpp->coreParam.defaultFormNo = PP_Get( pp, ID_PARA_form_no, NULL );

  setupBySrcData( bpp, pp, TRUE );

  BTL_Printf("setup pokeID=%d, monsno=%d, ppSrc=%p\n", pokeID, bpp->coreParam.monsno, pp );

  // �����N���ʏ�����
  Effrank_Init( &bpp->varyParam );

  // ��Ԉُ탏�[�N������
  clearWazaSickWork( bpp, TRUE );

  // ���̎��_�Ń|�P�����p��Ԉُ�ɂȂ��Ă���Έ����p��
  {
    PokeSick sick = PP_GetSick( pp );
    if( sick != POKESICK_NULL )
    {
      bpp->sickCont[ sick ] = BTL_CALC_MakeDefaultPokeSickCont( sick );
      bpp->wazaSickCounter[ sick ] = 0;
    }
  }

  // �e�탏�[�N�̈揉����
  bpp->appearedTurn = TURNCOUNT_NULL;
  bpp->turnCount = 0;
  bpp->migawariHP = 0;
  bpp->prevSelectWazaID = WAZANO_NULL;
  bpp->prevActWazaID    = WAZANO_NULL;
  bpp->wazaContCounter   = 0;
  bpp->combiWazaID = WAZANO_NULL;
  bpp->combiPokeID = BTL_POKEID_NULL;
  bpp->criticalRank = 0;

  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
  flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

  dmgrecClearWork( bpp );
  ConfrontRec_Clear( bpp );

  return bpp;
}
//----------------------------------------------------------------------------------
/**
 * ���ɂȂ�|�P�����p�����[�^�\���̂���K�v�������\�z
 *
 * @param   bpp
 * @param   srcPP
 */
//----------------------------------------------------------------------------------
static void setupBySrcData( BTL_POKEPARAM* bpp, const POKEMON_PARAM* srcPP, BOOL fReflectHP )
{
  u16 monsno = bpp->coreParam.monsno;
  int i;

  // 2010.4.15 HP���f�i���x���A�b�v�����S��HP���c���Ă��܂����ɑΏ��j
  if( fReflectHP ){
    bpp->coreParam.hp = PP_Get( srcPP, ID_PARA_hp, 0 );
    bpp->coreParam.hpMax = PP_Get( srcPP, ID_PARA_hpmax, 0 );
  }

  // ��{�p�����^������
  setupBySrcDataBase( bpp, srcPP );

  // ���L���U�f�[�^������
  bpp->wazaCnt = 0;
  for(i=0; i<PTL_WAZA_MAX; i++)
  {
    bpp->waza[i].number = PP_Get( srcPP, ID_PARA_waza1+i, 0 );
    if( bpp->waza[i].number != WAZANO_NULL )
    {
      bpp->waza[i].pp = PP_Get( srcPP, ID_PARA_pp1+i, 0 );
      bpp->waza[i].ppMax = PP_Get( srcPP, ID_PARA_pp_max1+i, 0 );
      bpp->wazaCnt++;
    }
    else
    {
      bpp->waza[i].pp = 0;
      bpp->waza[i].ppMax = 0;
    }
    bpp->waza[i].usedFlag = FALSE;
    bpp->waza[i].usedFlagFix = FALSE;
    bpp->waza[i].recoverNumber = bpp->waza[i].number;
  }

  bpp->usedWazaCount = 0;
  bpp->tokusei = PP_Get( srcPP, ID_PARA_speabino, 0 );
  bpp->formNo = PP_Get( srcPP, ID_PARA_form_no, 0 );
  bpp->coreParam.exp = PP_Get( srcPP, ID_PARA_exp, NULL );
  bpp->weight = POKETOOL_GetPersonalParam( bpp->coreParam.monsno, bpp->formNo, POKEPER_ID_weight ) / 10;
  if( bpp->weight < BTL_POKE_WEIGHT_MIN ){
    bpp->weight = BTL_POKE_WEIGHT_MIN;
  }
}
//----------------------------------------------------------------------------------
/**
 * ���ɂȂ�|�P�����p�����[�^�\���̂���o�g���p�����[�^�����̂ݍ\�z
 *
 * @param   bpp
 * @param   srcPP
 */
//----------------------------------------------------------------------------------
static void setupBySrcDataBase( BTL_POKEPARAM* bpp, const POKEMON_PARAM* srcPP )
{
  bpp->baseParam.type1 = PP_Get( srcPP, ID_PARA_type1, 0 );
  bpp->baseParam.type2 = PP_Get( srcPP, ID_PARA_type2, 0 );
  bpp->baseParam.sex = PP_GetSex( srcPP );
  bpp->baseParam.level = PP_Get( srcPP, ID_PARA_level, 0 );
  bpp->baseParam.attack = PP_Get( srcPP, ID_PARA_pow, 0 );
  bpp->baseParam.defence = PP_Get( srcPP, ID_PARA_def, 0 );
  bpp->baseParam.sp_attack = PP_Get( srcPP, ID_PARA_spepow, 0 );
  bpp->baseParam.sp_defence = PP_Get( srcPP, ID_PARA_spedef, 0 );
  bpp->baseParam.agility = PP_Get( srcPP, ID_PARA_agi, 0 );
}

//----------------------------------------------------------------------------------
/**
 * ���U�g�p�t���O�̃N���A
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void clearUsedWazaFlag( BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<NELEMS(bpp->waza); ++i){
    bpp->waza[i].usedFlag = FALSE;
  }

  bpp->prevSelectWazaID = WAZANO_NULL;
  bpp->prevActWazaID    = WAZANO_NULL;
  bpp->wazaContCounter  = 0;
}
//----------------------------------------------------------------------------------
/**
 * �e��J�E���^�l���N���A
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void clearCounter( BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<NELEMS(bpp->counter); ++i){
    bpp->counter[i] = 0;
  }
}

//=============================================================================================
/**
 * �o�g���p�|�P�����p�����[�^�폜
 *
 * @param   bpp
 *
 */
//=============================================================================================
void BTL_POKEPARAM_Delete( BTL_POKEPARAM* bpp )
{
  GFL_HEAP_FreeMemory( bpp );
}

//=============================================================================================
/**
 * �f�[�^�R�s�[
 *
 * @param   dst
 * @param   src
 *
 */
//=============================================================================================
void BTL_POKEPARAM_Copy( BTL_POKEPARAM* dst, const BTL_POKEPARAM* src )
{
  *dst = *src;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

static void Effrank_Init( BPP_VARIABLE_PARAM* rank )
{
  Effrank_Reset( rank );
}
static void Effrank_Reset( BPP_VARIABLE_PARAM* rank )
{
  rank->attack = BPP_RANK_STATUS_DEFAULT;
  rank->defence = BPP_RANK_STATUS_DEFAULT;
  rank->sp_attack = BPP_RANK_STATUS_DEFAULT;
  rank->sp_defence = BPP_RANK_STATUS_DEFAULT;
  rank->agility = BPP_RANK_STATUS_DEFAULT;
  rank->hit = BPP_RANK_STATUS_DEFAULT;
  rank->avoid = BPP_RANK_STATUS_DEFAULT;
}
static BOOL Effrank_Recover( BPP_VARIABLE_PARAM* rank )
{
  BOOL fEffective = FALSE;

  if( rank->attack < BPP_RANK_STATUS_DEFAULT ){
    rank->attack = BPP_RANK_STATUS_DEFAULT;
    fEffective = TRUE;
  };
  if( rank->defence < BPP_RANK_STATUS_DEFAULT ){
    rank->defence = BPP_RANK_STATUS_DEFAULT;
    fEffective = TRUE;
  }
  if( rank->sp_attack < BPP_RANK_STATUS_DEFAULT ){
    rank->sp_attack = BPP_RANK_STATUS_DEFAULT;
    fEffective = TRUE;
  }
  if( rank->sp_defence < BPP_RANK_STATUS_DEFAULT ){
    rank->sp_defence = BPP_RANK_STATUS_DEFAULT;
    fEffective = TRUE;
  }
  if( rank->agility < BPP_RANK_STATUS_DEFAULT ){
    rank->agility = BPP_RANK_STATUS_DEFAULT;
    fEffective = TRUE;
  }
  if( rank->hit < BPP_RANK_STATUS_DEFAULT ){
    rank->hit = BPP_RANK_STATUS_DEFAULT;
    fEffective = TRUE;
  }
  if( rank->avoid < BPP_RANK_STATUS_DEFAULT ){
    rank->avoid = BPP_RANK_STATUS_DEFAULT;
    fEffective = TRUE;
  }

  return fEffective;
}



//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

u8 BPP_GetID( const BTL_POKEPARAM* bpp )
{
  return bpp->coreParam.myID;
}

u16 BPP_GetMonsNo( const BTL_POKEPARAM* bpp )
{
  return bpp->coreParam.monsno;
}
/**
 *  ���U��������Ԃ�
 */
u8 BPP_WAZA_GetCount( const BTL_POKEPARAM* bpp )
{
  return bpp->wazaCnt;
}
/**
 *  �����Ă���ԂɎg�������U�̐���Ԃ��i���񂾂烊�Z�b�g�����j
 */
u8 BPP_WAZA_GetUsedCountInAlive( const BTL_POKEPARAM* bpp )
{
  u8 cnt, i;
  for(i=0, cnt=0; i<bpp->wazaCnt; ++i)
  {
    if( bpp->waza[i].usedFlag ){
      ++cnt;
    }
  }
  return cnt;
}
/**
 *  �g�������U�̐���Ԃ��i����ł��J�E���g�A�b�v�j
 */
u8 BPP_WAZA_GetUsedCount( const BTL_POKEPARAM* bpp )
{
  return bpp->usedWazaCount;
}

u8 BPP_WAZA_GetUsableCount( const BTL_POKEPARAM* bpp )
{
  u8 cnt, i;
  for(i=0, cnt=0; i<bpp->wazaCnt; ++i)
  {
    if( bpp->waza[i].pp ){
      ++cnt;
    }
  }
  return cnt;
}

WazaID BPP_WAZA_GetID( const BTL_POKEPARAM* bpp, u8 idx )
{
  GF_ASSERT(idx < bpp->wazaCnt);
  return bpp->waza[idx].number;
}
BOOL BPP_WAZA_CheckUsedInAlive( const BTL_POKEPARAM* bpp, u8 idx )
{
  GF_ASSERT(idx < bpp->wazaCnt);
  return bpp->waza[idx].usedFlag;
}
//=============================================================================================
/**
 * [���U�p�����[�^] ID, ��PP, �ő�PP����x�Ɏ擾
 *
 * @param   bpp
 * @param   idx     �Ώۃ��U�C���f�b�N�X
 * @param   PP      [out] ��PP���擾����ϐ��A�h���X
 * @param   PPMax   [out] �ő�PP���擾����ϐ��A�h���X
 *
 * @retval  WazaID  ���UID
 */
//=============================================================================================
WazaID BPP_WAZA_GetParticular( const BTL_POKEPARAM* bpp, u8 idx, u8* PP, u8* PPMax )
{
  GF_ASSERT(idx < bpp->wazaCnt);
  *PP = bpp->waza[idx].pp;
  *PPMax = bpp->waza[idx].ppMax;

  return bpp->waza[idx].number;
}

//=============================================================================================
/**
 * [���U�p�����[�^] PP�s�������擾
 *
 * @param   bpp
 * @param   idx       �Ώۃ��U�C���f�b�N�X
 *
 * @retval  u8        PP�s����
 */
//=============================================================================================
u8 BPP_WAZA_GetPPShort( const BTL_POKEPARAM* bpp, u8 idx )
{
  GF_ASSERT(idx < bpp->wazaCnt);
  return (bpp->waza[idx].ppMax - bpp->waza[idx].pp);
}

static void splitTypeCore( const BTL_POKEPARAM* bpp, PokeType* type1, PokeType* type2 )
{
  BOOL fHaneYasume = BPP_CheckSick( bpp, WAZASICK_HANEYASUME );

  if( (bpp->baseParam.type1 == POKETYPE_HIKOU) && (fHaneYasume) ){
    *type1 = POKETYPE_NULL;
  }else{
    *type1 = bpp->baseParam.type1;
  }

  if( (bpp->baseParam.type2 == POKETYPE_HIKOU) && (fHaneYasume) ){
    *type2 = POKETYPE_NULL;
  }else{
    *type2 = bpp->baseParam.type2;
  }
}

PokeTypePair BPP_GetPokeType( const BTL_POKEPARAM* bpp )
{
  PokeTypePair  typePair;
  PokeType  type1, type2;

  splitTypeCore( bpp, &type1, &type2 );
  typePair = PokeTypePair_Make( type1, type2 );

  return typePair;
}

//=============================================================================================
/**
 * �|�P���������Q�^�C�v�ɁA�w��^�C�v���܂܂�Ă��邩����
 *
 * @param   bpp
 * @param   type
 *
 * @retval  BOOL    �܂܂�Ă����TRUE
 */
//=============================================================================================
BOOL BPP_IsMatchType( const BTL_POKEPARAM* bpp, PokeType type )
{
  if( type != POKETYPE_NULL )
  {
    PokeType  type1, type2;
    splitTypeCore( bpp, &type1, &type2 );
    if( (type1 == type) || (type2 == type) ){
      return TRUE;
    }
  }
  return FALSE;
}

const POKEMON_PARAM* BPP_GetSrcData( const BTL_POKEPARAM* bpp )
{
  return bpp->coreParam.ppSrc;
}
void BPP_SetViewSrcData( BTL_POKEPARAM* bpp, const POKEMON_PARAM* fakePP )
{
  bpp->coreParam.ppFake =  fakePP;
  bpp->coreParam.fFakeEnable = TRUE;
}
void BPP_ClearViewSrcData( BTL_POKEPARAM* bpp )
{
  bpp->coreParam.ppFake = NULL;
  bpp->coreParam.fFakeEnable = FALSE;
}
const POKEMON_PARAM* BPP_GetViewSrcData( const BTL_POKEPARAM* bpp )
{
  if( (bpp->coreParam.ppFake != NULL)
  &&  (bpp->coreParam.fFakeEnable)
  ){
    return bpp->coreParam.ppFake;
  }
  return bpp->coreParam.ppSrc;
}

//----------------------------------------------------------------------------------
/**
 * ��Ԉُ퓙�̏����ɂ��A�p�����[�^�擾�p��ID��ύX����
 * ������A�����_�[���[���i�S�|�P�����̖h��E���h�����ւ���j�ɂ̂ݗ��p���Ă���B
 *
 * @param   bpp
 * @param   BppValueID    �p�����[�^�擾�p�Ƀ��N�G�X�g���ꂽ�p�����[�^ID
 *
 * @retval  BppValueID    ���ۂɓK�p����p�����[�^ID
 */
//----------------------------------------------------------------------------------
static BppValueID ConvertValueID( const BTL_POKEPARAM* bpp, BppValueID vid )
{
  switch( vid ){
  case BPP_DEFENCE:
    if( BTL_FIELD_CheckEffect(BTL_FLDEFF_WONDERROOM) ){
      vid = BPP_SP_DEFENCE;
    }
    break;

  case BPP_SP_DEFENCE:
    if( BTL_FIELD_CheckEffect(BTL_FLDEFF_WONDERROOM) ){
      vid = BPP_DEFENCE;
    }
    break;

  }
  return vid;
}
//=============================================================================================
/**
 * �����N�␳�t���b�g�ȏ�Ԃ̃X�e�[�^�X�擾
 *
 * @param   bpp
 * @param   vid   �K�v�ȃp�����[�^ID
 *
 * @retval  int
 */
//=============================================================================================
int BPP_GetValue_Base( const BTL_POKEPARAM* bpp, BppValueID vid )
{
  vid = ConvertValueID( bpp, vid );

  switch( vid ){
  case BPP_ATTACK:            return bpp->baseParam.attack;
  case BPP_DEFENCE:           return bpp->baseParam.defence;
  case BPP_SP_ATTACK:         return bpp->baseParam.sp_attack;
  case BPP_SP_DEFENCE:        return bpp->baseParam.sp_defence;
  case BPP_AGILITY:           return bpp->baseParam.agility;

  case BPP_HIT_RATIO:         return BPP_RANK_STATUS_DEFAULT;
  case BPP_AVOID_RATIO:       return BPP_RANK_STATUS_DEFAULT;

  default:
    return BPP_GetValue( bpp, vid );
  };
}
//=============================================================================================
/**
 * �\�͒l����������
 *
 * @param   bpp
 * @param   vid
 * @param   value
 */
//=============================================================================================
void BPP_SetBaseStatus( BTL_POKEPARAM* bpp, BppValueID vid, u16 value )
{
  vid = ConvertValueID( bpp, vid );

  switch( vid ){
  case BPP_ATTACK:      bpp->baseParam.attack = value; break;
  case BPP_DEFENCE:     bpp->baseParam.defence = value; break;
  case BPP_SP_ATTACK:   bpp->baseParam.sp_attack = value; break;
  case BPP_SP_DEFENCE:  bpp->baseParam.sp_defence = value; break;
  case BPP_AGILITY:     bpp->baseParam.agility = value; break;

  default:
    GF_ASSERT(0);
    break;
  }

  BTL_Printf("�|�P[%d]�̔\��(%d)��%d�ɏ���������\n", bpp->coreParam.myID, vid, value);
}

//=============================================================================================
/**
 * �e��p�����[�^�擾
 *
 * @param   pp
 * @param   vid   �K�v�ȃp�����[�^ID
 *
 * @retval  int
 */
//=============================================================================================
int BPP_GetValue( const BTL_POKEPARAM* bpp, BppValueID vid )
{
  vid = ConvertValueID( bpp, vid );

  switch( vid ){
  case BPP_ATTACK:          return BTL_CALC_StatusRank( BPP_GetValue_Base(bpp, vid), bpp->varyParam.attack );
  case BPP_DEFENCE:         return BTL_CALC_StatusRank( BPP_GetValue_Base(bpp, vid), bpp->varyParam.defence );
  case BPP_SP_ATTACK:       return BTL_CALC_StatusRank( BPP_GetValue_Base(bpp, vid), bpp->varyParam.sp_attack );
  case BPP_SP_DEFENCE:      return BTL_CALC_StatusRank( BPP_GetValue_Base(bpp, vid), bpp->varyParam.sp_defence );
  case BPP_AGILITY:         return BTL_CALC_StatusRank( BPP_GetValue_Base(bpp, vid), bpp->varyParam.agility );

  case BPP_ATTACK_RANK:     return bpp->varyParam.attack;
  case BPP_DEFENCE_RANK:    return bpp->varyParam.defence;
  case BPP_SP_ATTACK_RANK:  return bpp->varyParam.sp_attack;
  case BPP_SP_DEFENCE_RANK: return bpp->varyParam.sp_defence;
  case BPP_AGILITY_RANK:    return bpp->varyParam.agility;

  case BPP_HIT_RATIO:       return bpp->varyParam.hit;
  case BPP_AVOID_RATIO:     return bpp->varyParam.avoid;

  case BPP_LEVEL:           return bpp->baseParam.level;
  case BPP_SEX:             return bpp->baseParam.sex;

  case BPP_HP:              return bpp->coreParam.hp;
  case BPP_MAX_HP:          return bpp->coreParam.hpMax;

  case BPP_TOKUSEI_EFFECTIVE:
    if( BPP_CheckSick(bpp, WAZASICK_IEKI) ){
      return POKETOKUSEI_NULL;
    }
    /* fallthru */
  case BPP_TOKUSEI:           return bpp->tokusei;

  case BPP_FORM:            return bpp->formNo;
  case BPP_EXP:             return bpp->coreParam.exp;

  default:
    GF_ASSERT(0);
    return 0;
  };
}
//=============================================================================================
/**
 * �N���e�B�J���q�b�g���̃p�����[�^�擾�i�U�����ɕs���ȃ����N�␳���t���b�g�ɂ���j
 *
 * @param   pp
 * @param   vid
 *
 * @retval  int
 */
//=============================================================================================
int BPP_GetValue_Critical( const BTL_POKEPARAM* bpp, BppValueID vid )
{
  BOOL fFlatParam = FALSE;
  BppValueID  vid_org = vid;

  vid = ConvertValueID( bpp, vid_org );

  switch( vid ){
  case BPP_ATTACK:     fFlatParam = (bpp->varyParam.attack < BPP_RANK_STATUS_DEFAULT); break;
  case BPP_SP_ATTACK:  fFlatParam = (bpp->varyParam.sp_attack < BPP_RANK_STATUS_DEFAULT); break;
  case BPP_DEFENCE:    fFlatParam = (bpp->varyParam.defence > BPP_RANK_STATUS_DEFAULT); break;
  case BPP_SP_DEFENCE: fFlatParam = (bpp->varyParam.sp_defence > BPP_RANK_STATUS_DEFAULT); break;

  default:
    break;
  }

  if( fFlatParam ){
    return BPP_GetValue_Base( bpp, vid_org );
  }else{
    return BPP_GetValue( bpp, vid_org );
  }
}
//=============================================================================================
/**
 * ���L�A�C�e��ID��Ԃ�
 *
 * @param   pp
 *
 * @retval  u32
 */
//=============================================================================================
u32 BPP_GetItem( const BTL_POKEPARAM* pp )
{
  return pp->coreParam.item;
}
void BPP_SetItem( BTL_POKEPARAM* pp, u16 itemID )
{
  pp->coreParam.item = itemID;
}

//=============================================================================================
/**
 * HP���^�����`�F�b�N
 *
 * @param   pp
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsHPFull( const BTL_POKEPARAM* pp )
{
  return BPP_GetValue(pp, BPP_HP) == BPP_GetValue(pp, BPP_MAX_HP);
}
//=============================================================================================
/**
 * �Ђ񂵏�Ԃ��`�F�b�N
 *
 * @param   pp
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsDead( const BTL_POKEPARAM* pp )
{
  return BPP_GetValue( pp, BPP_HP ) == 0;
}
BOOL BPP_IsFightEnable( const BTL_POKEPARAM* bpp )
{
  if( PP_Get(bpp->coreParam.ppSrc, ID_PARA_tamago_flag, NULL)){
    TAYA_Printf("pokeID=%d, ���܂��ł�\n", bpp->coreParam.myID );
    return FALSE;
  }
  if( BPP_IsDead(bpp) ){
    return FALSE;
  }
  return TRUE;
}

//=============================================================================================
/**
 * ���UPP�l���擾
 *
 * @param   pp
 * @param   wazaIdx
 *
 */
//=============================================================================================
u16 BPP_WAZA_GetPP( const BTL_POKEPARAM* pp, u8 wazaIdx )
{
  if( wazaIdx < pp->wazaCnt ){
    return  pp->waza[wazaIdx].pp;
  }else{
    GF_ASSERT_MSG(0,"wazaIdx:%d, wazaCnt:%d", wazaIdx, pp->wazaCnt);
    return 0;
  }
}
//=============================================================================================
/**
 * ���UPP�l�����^�����ǂ����`�F�b�N
 *
 * @param   pp
 * @param   wazaIdx
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsPPFull( const BTL_POKEPARAM* pp, u8 wazaIdx )
{
  GF_ASSERT(wazaIdx < pp->wazaCnt);
  return  pp->waza[wazaIdx].pp == pp->waza[wazaIdx].ppMax;
}
//=============================================================================================
/**
 * �p�����ď�ɏo�Ă���^�[������Ԃ�
 *
 * @param   pp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BPP_GetTurnCount( const BTL_POKEPARAM* pp )
{
  return pp->turnCount;
}
//=============================================================================================
/**
 * ��ɏo�����̃^�[������Ԃ�
 *
 * @param   pp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BPP_GetAppearTurn( const BTL_POKEPARAM* pp )
{
  return pp->appearedTurn;
}
//=============================================================================================
/**
 * �^�[���t���O�l�擾
 *
 * @param   pp
 * @param   flagID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_TURNFLAG_Get( const BTL_POKEPARAM* pp, BppTurnFlag flagID )
{
  return flgbuf_get( pp->turnFlag, flagID );
}
//=============================================================================================
/**
 * �i���t���O�l�擾
 *
 * @param   pp
 * @param   flagID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_CONTFLAG_Get( const BTL_POKEPARAM* pp, BppContFlag flagID )
{
  return flgbuf_get( pp->contFlag, flagID );
}
//=============================================================================================
/**
 * HP�c�ʂ̂߂₷�i���ʁE�����E�s���`�Ƃ��j��Ԃ�
 *
 * @param   pp
 *
 * @retval  BppHpBorder
 */
//=============================================================================================
BppHpBorder BPP_CheckHPBorder( const BTL_POKEPARAM* pp, u32 hp )
{
  if( hp <= (pp->coreParam.hpMax / 8) )
  {
    return BPP_HPBORDER_RED;
  }
  if( hp <= (pp->coreParam.hpMax / 3) )
  {
    return BPP_HPBORDER_YELLOW;
  }
  return BPP_HPBORDER_GREEN;
}
//=============================================================================================
/**
 * HP�c�ʂ̂߂₷�i���ʁE�����E�s���`�Ƃ��j��Ԃ�
 *
 * @param   pp
 *
 * @retval  BppHpBorder
 */
//=============================================================================================
BppHpBorder BPP_GetHPBorder( const BTL_POKEPARAM* pp )
{
  return BPP_CheckHPBorder( pp, pp->coreParam.hp );
}

//=============================================================================================
/**
 * �c��HP/MaxHP �䗦�i�p�[�Z���e�[�W�j�擾
 *
 * @param   pp
 *
 * @retval  fx32
 */
//=============================================================================================
fx32 BPP_GetHPRatio( const BTL_POKEPARAM* pp )
{
  double r = (double)(pp->coreParam.hp * 100) / (double)(pp->coreParam.hpMax);
  return FX32_CONST( r );
}
//=============================================================================================
/**
 * �w�胏�U��index��Ԃ��i�����������Ă��Ȃ����U�Ȃ�PTL_WAZA_MAX��Ԃ�)
  *
 * @param   pp
 * @param   waza
 *
 * @retval  u8
 */
//=============================================================================================
u8 BPP_WAZA_SearchIdx( const BTL_POKEPARAM* pp, WazaID waza )
{
  if( waza != WAZANO_NULL )
  {
    u32 i;
    for(i=0; i<PTL_WAZA_MAX; ++i){
      if( pp->waza[i].number == waza ){
        return i;
      }
    }
  }
  return PTL_WAZA_MAX;
}

//-----------------------------
static const s8* getRankVaryStatusConst( const BTL_POKEPARAM* bpp, BppValueID type, s8* min, s8* max )
{
  const s8* ptr;

  *min = BPP_RANK_STATUS_MIN;
  *max = BPP_RANK_STATUS_MAX;

  switch( type ) {
  case BPP_ATTACK_RANK:     ptr = &bpp->varyParam.attack; break;
  case BPP_DEFENCE_RANK:    ptr = &bpp->varyParam.defence; break;
  case BPP_SP_ATTACK_RANK:  ptr = &bpp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE_RANK: ptr = &bpp->varyParam.sp_defence; break;
  case BPP_AGILITY_RANK:    ptr = &bpp->varyParam.agility; break;
  case BPP_HIT_RATIO:       ptr = &bpp->varyParam.hit; break;
  case BPP_AVOID_RATIO:     ptr = &bpp->varyParam.avoid; break;

  default:
    GF_ASSERT_MSG(0, "illegal rank Type ->%d", type);
    return NULL;
  }
  return ptr;
}
static s8* getRankVaryStatus( BTL_POKEPARAM* pp, BppValueID type, s8* min, s8* max )
{
  return (s8*) getRankVaryStatusConst( pp, type, min, max );
}

//=============================================================================================
/**
 * �����N�������ʂ��L�����H
 *
 * @param   bpp
 * @param   rankType
 * @param   volume
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsRankEffectValid( const BTL_POKEPARAM* bpp, BppValueID rankType, int volume )
{
  const s8* ptr;
  s8  min, max, next_value;

  ptr = getRankVaryStatusConst( bpp, rankType, &min, &max );
  BTL_Printf("  ����=%d, �ŏ�=%d, �ő�=%d, ���ʒl=%d\n", *ptr, min, max, volume);
  if( volume > 0 ){
    return ((*ptr) < max);
  }else{
    return ((*ptr) > min);
  }
}
//=============================================================================================
/**
 * �����N�������ʂ����Ɖ��i�K����������
 *
 * @param   pp
 * @param   rankType
 *
 * @retval  int   �i�K��
 */
//=============================================================================================
int BPP_RankEffectUpLimit( const BTL_POKEPARAM* pp, BppValueID rankType )
{
  const s8* ptr;
  s8  min, max;

  ptr = getRankVaryStatusConst( pp, rankType, &min, &max );
  BTL_Printf("�|�P[%d]�̃����N����(%d) ... ���i�K=%d, �㏸���E=%d\n", pp->coreParam.myID, rankType, *ptr, (max-(*ptr)));
  return max - (*ptr);
}
//=============================================================================================
/**
 * �����N�������ʂ����Ɖ��i�K����������
 *
 * @param   pp
 * @param   rankType
 *
 * @retval  int   �i�K���i�}�C�i�X�j
 */
//=============================================================================================
int BPP_RankEffectDownLimit( const BTL_POKEPARAM* bpp, BppValueID rankType )
{
  const s8* ptr;
  s8  min, max;

  ptr = getRankVaryStatusConst( bpp, rankType, &min, &max );
  return (*ptr) - min;
}
//=============================================================================================
/**
 * �t���b�g��胉���N���������Ă���\�͂��P�ȏ゠�邩�`�F�b�N
 *
 * @param   bpp
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsRankEffectDowned( const BTL_POKEPARAM* bpp )
{
  if( bpp->varyParam.attack < BPP_RANK_STATUS_DEFAULT ){ return TRUE; }
  if( bpp->varyParam.defence  < BPP_RANK_STATUS_DEFAULT ){ return TRUE; }
  if( bpp->varyParam.sp_attack  < BPP_RANK_STATUS_DEFAULT ){ return TRUE; }
  if( bpp->varyParam.sp_defence  < BPP_RANK_STATUS_DEFAULT ){ return TRUE; }
  if( bpp->varyParam.agility  < BPP_RANK_STATUS_DEFAULT ){ return TRUE; }
  if( bpp->varyParam.hit  < BPP_RANK_STATUS_DEFAULT ){ return TRUE; }
  if( bpp->varyParam.avoid  < BPP_RANK_STATUS_DEFAULT ){ return TRUE; }
  return FALSE;
}

//=============================================================================================
/**
 * �����N�A�b�v����
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 * @retval  u8 result   ���ۂɏオ�����i�K��
 */
//=============================================================================================
u8 BPP_RankUp( BTL_POKEPARAM* bpp, BppValueID rankType, u8 volume )
{
  s8 *ptr;
  s8 max = BPP_RANK_STATUS_MAX;

  switch( rankType ){
  case BPP_ATTACK_RANK:     ptr = &bpp->varyParam.attack; break;
  case BPP_DEFENCE_RANK:    ptr = &bpp->varyParam.defence; break;
  case BPP_SP_ATTACK_RANK:  ptr = &bpp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE_RANK: ptr = &bpp->varyParam.sp_defence; break;
  case BPP_AGILITY_RANK:    ptr = &bpp->varyParam.agility; break;
  case BPP_HIT_RATIO:       ptr = &bpp->varyParam.hit; break;
  case BPP_AVOID_RATIO:     ptr = &bpp->varyParam.avoid; break;

  default:
    GF_ASSERT(0);
    return FALSE;
  }

  if( *ptr < max )
  {
    if( (*ptr + volume) > max ){
      volume = max - (*ptr);
    }
    *ptr += volume;
    return volume;
  }

  return 0;
}
//=============================================================================================
/**
 * �����N�_�E������
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 * @retval  u8    ���ۂɉ��������i�K��
 */
//=============================================================================================
u8 BPP_RankDown( BTL_POKEPARAM* bpp, BppValueID rankType, u8 volume )
{
  s8 *ptr;
  s8 min = BPP_RANK_STATUS_MIN;

  switch( rankType ){
  case BPP_ATTACK_RANK:     ptr = &bpp->varyParam.attack; break;
  case BPP_DEFENCE_RANK:    ptr = &bpp->varyParam.defence; break;
  case BPP_SP_ATTACK_RANK:  ptr = &bpp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE_RANK: ptr = &bpp->varyParam.sp_defence; break;
  case BPP_AGILITY_RANK:    ptr = &bpp->varyParam.agility; break;
  case BPP_HIT_RATIO:       ptr = &bpp->varyParam.hit; break;
  case BPP_AVOID_RATIO:     ptr = &bpp->varyParam.avoid; break;

  default:
    GF_ASSERT(0);
    return FALSE;
  }

  if( *ptr > min )
  {
    if( (*ptr - volume) < min ){
      volume = (*ptr) - min;
    }
    *ptr -= volume;
    return volume;
  }
  return 0;
}
//=============================================================================================
/**
 * �����N�l�����Z�b�g
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 */
//=============================================================================================
void BPP_RankSet( BTL_POKEPARAM* pp, BppValueID rankType, u8 value )
{
  s8 *ptr;
  u8 min = BPP_RANK_STATUS_MIN;
  u8 max = BPP_RANK_STATUS_MAX;

  switch( rankType ){
  case BPP_ATTACK_RANK:     ptr = &pp->varyParam.attack; break;
  case BPP_DEFENCE_RANK:    ptr = &pp->varyParam.defence; break;
  case BPP_SP_ATTACK_RANK:  ptr = &pp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE_RANK: ptr = &pp->varyParam.sp_defence; break;
  case BPP_AGILITY_RANK:    ptr = &pp->varyParam.agility; break;
  case BPP_HIT_RATIO:       ptr = &pp->varyParam.hit; break;
  case BPP_AVOID_RATIO:     ptr = &pp->varyParam.avoid; break;

  default:
    GF_ASSERT(0);
    return;
  }

  if( (min <= value) && (value <=max) ){
    *ptr = value;
  }else{
    GF_ASSERT_MSG(0, "value=%d", value);
  }
}
//=============================================================================================
/**
 * �����N���ʁi�V��j�̓��A�������Ă�����̂��t���b�g�ɖ߂�
 *
 * @param   bpp
 *
 * @retval  BOOL    �L���������i�����N���������Ă���\�͂��P�ȏ゠�����j�ꍇ�ATRUE
 */
//=============================================================================================
BOOL BPP_RankRecover( BTL_POKEPARAM* bpp )
{
  return Effrank_Recover( &bpp->varyParam );
}
//=============================================================================================
/**
 * �����N���ʁi�V��j���t���b�g�ɖ߂�
 *
 * @param   pp
 */
//=============================================================================================
void BPP_RankReset( BTL_POKEPARAM* bpp )
{
  Effrank_Reset( &bpp->varyParam );
}

//=============================================================================================
/**
 * �N���e�B�J�������N�擾
 *
 * @param   bpp
 *
 * @retval  u8
 */
//=============================================================================================
u8 BPP_GetCriticalRank( const BTL_POKEPARAM* bpp )
{
  u8 result = bpp->criticalRank;
  if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_KIAIDAME) ){
    result += 2;
    if( result > RANK_CRITICAL_MAX ){
      result = RANK_CRITICAL_MAX;
    }
  }
  return result;
}
//=============================================================================================
/**
 * �N���e�B�J�������N�ϓ�
 *
 * @param   bpp
 * @param   value
 *
 * @retval  BOOL    �ϓ������ꍇTRUE�^���ɏ���i�����j�ŕϓ��ł��Ȃ��ꍇFALSE
 */
//=============================================================================================
BOOL BPP_AddCriticalRank( BTL_POKEPARAM* bpp, int value )
{
  if( value > 0 )
  {
    if( bpp->criticalRank < RANK_CRITICAL_MAX )
    {
      bpp->criticalRank += value;
      if( bpp->criticalRank > RANK_CRITICAL_MAX ){
        bpp->criticalRank = RANK_CRITICAL_MAX;
      }
      return TRUE;
    }
  }
  else
  {
    value *= -1;
    if( bpp->criticalRank )
    {
      if( bpp->criticalRank > value ){
        bpp->criticalRank -= value;
      }else{
        bpp->criticalRank = 0;
      }
      return TRUE;
    }
  }
  return FALSE;
}



//=============================================================================================
/**
 * HP�l������
 *
 * @param   pp
 * @param   value
 *
 */
//=============================================================================================
void BPP_HpMinus( BTL_POKEPARAM* pp, u16 value )
{
  if( pp->coreParam.hp > value )
  {
    pp->coreParam.hp -= value;
  }
  else
  {
    pp->coreParam.hp = 0;
  }
}
//=============================================================================================
/**
 * HP�l�𑝉�
 *
 * @param   pp
 * @param   value
 *
 */
//=============================================================================================
void BPP_HpPlus( BTL_POKEPARAM* pp, u16 value )
{
  pp->coreParam.hp += value;
  if( pp->coreParam.hp > pp->coreParam.hpMax )
  {
    pp->coreParam.hp = pp->coreParam.hpMax;
  }
}
//=============================================================================================
/**
 * HP�l���[���ɂ���
 *
 * @param   pp
 *
 */
//=============================================================================================
void BPP_HpZero( BTL_POKEPARAM* pp )
{
  pp->coreParam.hp = 0;
}
//=============================================================================================
/**
 * ���UPP�l������
 *
 * @param   pp
 * @param   wazaIdx
 * @param   value
 *
 */
//=============================================================================================
void BPP_PPMinus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value )
{
  GF_ASSERT(wazaIdx < pp->wazaCnt);

  if( pp->waza[wazaIdx].pp >= value )
  {
    pp->waza[wazaIdx].pp -= value;
  }
  else
  {
    pp->waza[wazaIdx].pp = 0;
  }
}
//=============================================================================================
/**
 * ���UPP�l�𑝉�
 *
 * @param   pp
 * @param   wazaIdx
 * @param   value
 *
 */
//=============================================================================================
void BPP_PPPlus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value )
{
  GF_ASSERT(wazaIdx < pp->wazaCnt);

  pp->waza[wazaIdx].pp += value;
  if( pp->waza[wazaIdx].pp > pp->waza[wazaIdx].ppMax )
  {
    pp->waza[wazaIdx].pp = pp->waza[wazaIdx].ppMax;
  }
}
//=============================================================================================
/**
 * �g�p�������U�t���O�𗧂Ă�
 *
 * @param   pp
 * @param   wazaIdx
 */
//=============================================================================================
void BPP_WAZA_SetUsedFlag( BTL_POKEPARAM* bpp, u8 wazaIdx )
{
  BPP_WAZA* pWaza = &bpp->waza[ wazaIdx ];

  if( pWaza->usedFlagFix == FALSE ){
    pWaza->usedFlagFix = TRUE;
    bpp->usedWazaCount++;
  }
  pWaza->usedFlag = TRUE;
}
//=============================================================================================
/**
 * ���U�i���o�[�㏑��
 *
 * @param   pp
 * @param   wazaIdx       ���Ԗڂ̃��U[0-3]�H
 * @param   waza          �㏑���ハ�U�i���o�[
 * @param   ppMax         PP�ő�l�̏���i�ő�ł��f�t�H���g�l�܂ŁB0�Ȃ�f�t�H���g�l�ɂȂ�j
 * @param   fPermenent    �i���t���O�iTRUE�Ȃ�o�g����܂ň����p���^FALSE�Ȃ�m���E���ꂩ���Ō��ɖ߂�j
 */
//=============================================================================================
void BPP_WAZA_UpdateID( BTL_POKEPARAM* pp, u8 wazaIdx, WazaID waza, u8 ppMax, BOOL fPermenent )
{
  BPP_WAZA* pWaza = &pp->waza[ wazaIdx ];

  if( !fPermenent ){
    pWaza->recoverNumber = pWaza->number;
  }else{
    pWaza->recoverNumber = waza;
  }
  pWaza->number = waza;
  pWaza->usedFlag = FALSE;
  pWaza->usedFlagFix = FALSE;
  pWaza->ppMax = WAZADATA_GetMaxPP( waza, 0 );

  if( (ppMax != 0)
  &&  (pWaza->ppMax > ppMax)
  ){
    pWaza->ppMax = ppMax;
  }
  pWaza->pp = pWaza->ppMax;
}
//=============================================================================================
/**
 * �w�胏�U���o���Ă��邩�`�F�b�N
 *
 * @param   bpp
 * @param   waza
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_WAZA_IsUsable( const BTL_POKEPARAM* bpp, WazaID waza )
{
  int i;
  for(i=0; i<NELEMS(bpp->waza); ++i)
  {
    if( bpp->waza[i].number == waza ){
      return TRUE;
    }
  }
  return FALSE;
}
//=============================================================================================
/**
 * �P�^�[���L���t���O�̃Z�b�g
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BPP_TURNFLAG_Set( BTL_POKEPARAM* pp, BppTurnFlag flagID )
{
  flgbuf_set( pp->turnFlag, flagID );
}
//=============================================================================================
/**
 * �i���t���O�̃Z�b�g
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BPP_CONTFLAG_Set( BTL_POKEPARAM* pp, BppContFlag flagID )
{
  if( flagID == BPP_CONTFLG_POWERTRICK ){
    BTL_Printf("�|�P[%d]�Ƀp���[�g���b�N�t���O\n", pp->coreParam.myID);
  }
  flgbuf_set( pp->contFlag, flagID );
}
//=============================================================================================
/**
 * �i���t���O�̃��Z�b�g
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BPP_CONTFLAG_Clear( BTL_POKEPARAM* pp, BppContFlag flagID )
{
  flgbuf_reset( pp->contFlag, flagID );
}

//==================================================================================================
// ��Ԉُ�֘A
//==================================================================================================


//=============================================================================================
/**
 * ��Ԉُ��ݒ�
 *
 * @param   pp
 * @param   sick
 * @param   contParam   �p���p�����[�^
 *
 */
//=============================================================================================
void BPP_SetWazaSick( BTL_POKEPARAM* bpp, WazaSick sick, BPP_SICK_CONT contParam )
{
  if( BTL_CALC_IsBasicSickID(sick) )
  {
    PokeSick pokeSick = BPP_GetPokeSick( bpp );
    GF_ASSERT(pokeSick == POKESICK_NULL);
  }

  bpp->sickCont[ sick ] = contParam;
  bpp->wazaSickCounter[sick] = 0;
  if( sick == WAZASICK_NEMURI ){
  }

}


//=============================================================================================
/**
 * ��Ԉُ�̃^�[���`�F�b�N����
 *
 * @param   bpp
 * @param   callbackFunc    �������Ă����Ԉُ킲�ƂɌĂяo�����R�[���o�b�N�֐�
 * @param   callbackArg     �R�[���o�b�N�Ɉ����n���C�ӈ���
 */
//=============================================================================================
void BPP_WazaSick_TurnCheck( BTL_POKEPARAM* bpp, BtlSickTurnCheckFunc callbackFunc, void* callbackWork )
{
  WazaSick  sick;


  for(sick=0; sick<NELEMS(bpp->sickCont); ++sick)
  {
    // �˂ނ�E������̓A�N�V�����J�n�̃^�C�~���O�œƎ��̃J�E���^�f�N�������g����
    if( (sick == WAZASICK_NEMURI) || (sick == WAZASICK_KONRAN) ){
      continue;
    }
    if( bpp->sickCont[sick].type != WAZASICK_CONT_NONE )
    {
      u32 turnMax = BPP_SICCONT_GetTurnMax( bpp->sickCont[sick] );
      BOOL fCure = FALSE;
      BPP_SICK_CONT oldCont;

      oldCont = bpp->sickCont[ sick ];

      // ������胍�b�N�́A�Y�����郏�U�������Ă��Ȃ��Ȃ璼��
      if( sick == WAZASICK_KODAWARI )
      {
        WazaID  waza = BPP_SICKCONT_GetParam( oldCont );
        if( !BPP_WAZA_IsUsable(bpp, waza) )
        {
          bpp->sickCont[sick] = BPP_SICKCONT_MakeNull();;
          bpp->wazaSickCounter[sick] = 0;
          fCure = TRUE;
        }
      }

      // �p���^�[���o�߃`�F�b�N
      if( turnMax )
      {
        bpp->wazaSickCounter[sick] += 1;

        if( bpp->wazaSickCounter[sick] >= turnMax )
        {
          BTL_Printf("�o�߃^�[�����ő�^�[�����z����\n");
          bpp->sickCont[sick] = BPP_SICKCONT_MakeNull();;
          bpp->wazaSickCounter[sick] = 0;
          fCure = TRUE;
        }
      }
      // �i���^�ōő�^�[�������w�肳��Ă�����̂̓J�E���^���C���N�������g
      // �i����A���̋@�\�𗘗p���Ă���̂́u�ǂ��ǂ��v�̂݁j
      else if( bpp->sickCont[sick].type == WAZASICK_CONT_PERMANENT )
      {
        if( (bpp->sickCont[sick].permanent.count_max != 0 )
        &&  (bpp->wazaSickCounter[sick] < bpp->sickCont[sick].permanent.count_max)
        ){
          bpp->wazaSickCounter[sick]++;
        }
      }
      if( callbackFunc != NULL ){
        callbackFunc( bpp, sick, oldCont, fCure, callbackWork );
      }
    }
  }
}

//=============================================================================================
/**
 * �u�˂ނ�v�Ǝ��̃^�[���`�F�b�N����
 * ��Ԉُ�J�E���^���C���N�������g���A�N����^�N���Ȃ��̃`�F�b�N���s��
 *
 * @param   bpp
 *
 * @retval  BOOL    ����̌Ăяo���ɂ���ċN�����ꍇ��TRUE�^����ȊO��FALSE
 */
//=============================================================================================
BOOL BPP_CheckNemuriWakeUp( BTL_POKEPARAM* bpp )
{
  BPP_SICK_CONT* cont = &(bpp->sickCont[ WAZASICK_NEMURI ]);

  if( cont->type != WAZASICK_CONT_NONE )
  {
    u32 turnMax = BPP_SICCONT_GetTurnMax( *cont );
    if( turnMax )
    {
      // �Ƃ������u�͂₨���v�͖���J�E���^�Q�{��
      u32 incValue = IsMatchTokusei(bpp, POKETOKUSEI_HAYAOKI)? 2 : 1;

      bpp->wazaSickCounter[ WAZASICK_NEMURI ] += incValue;
      BTL_N_Printf( DBGSTR_BPP_NemuriWakeCheck, bpp->coreParam.myID, turnMax, bpp->wazaSickCounter[ WAZASICK_NEMURI ] );
      if( bpp->wazaSickCounter[ WAZASICK_NEMURI ] >= turnMax )
      {
        *cont = BPP_SICKCONT_MakeNull();;
        BPP_CureWazaSick( bpp, WAZASICK_NEMURI );
        return TRUE;
      }
    }
  }
  return FALSE;
}
//=============================================================================================
/**
 * �u������v�Ǝ��̃^�[���`�F�b�N����
 * ��Ԉُ�J�E���^���C���N�������g���A���߂�^���߂Ȃ��̃`�F�b�N���s��
 *
 * @param   bpp
 *
 * @retval  BOOL    ����̌Ăяo���ɂ���Ċo�߂��ꍇ��TRUE�^����ȊO��FALSE
 */
//=============================================================================================
BOOL BPP_CheckKonranWakeUp( BTL_POKEPARAM* bpp )
{
  enum {
    SICK_ID = WAZASICK_KONRAN,
  };
  BPP_SICK_CONT* cont = &(bpp->sickCont[ SICK_ID ]);

  if( cont->type != WAZASICK_CONT_NONE )
  {
    u32 turnMax = BPP_SICCONT_GetTurnMax( *cont );
    if( turnMax )
    {
      bpp->wazaSickCounter[ SICK_ID ]++;
      if( bpp->wazaSickCounter[ SICK_ID ] >= turnMax )
      {
        *cont = BPP_SICKCONT_MakeNull();;
        BPP_CureWazaSick( bpp, SICK_ID );
        return TRUE;
      }
    }
  }
  return FALSE;
}

//=============================================================================================
/**
 * �|�P�����n��Ԉُ���񕜂�����
 *
 * @param   pp
 *
 */
//=============================================================================================
void BPP_CurePokeSick( BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=POKESICK_ORIGIN; i<POKESICK_MAX; ++i)
  {
    bpp->sickCont[ i ] = BPP_SICKCONT_MakeNull();
    bpp->wazaSickCounter[ i ] = 0;
    cureDependSick( bpp, i );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����Ԉُ킪���������ɓ����Ɏ����Ԃ̏���
 *
 * @param   bpp
 * @param   sickID
 */
//----------------------------------------------------------------------------------
static void cureDependSick( BTL_POKEPARAM* bpp, WazaSick sickID  )
{
  switch( sickID ){
  case WAZASICK_NEMURI:
    bpp->sickCont[ WAZASICK_AKUMU ] = BPP_SICKCONT_MakeNull();    // ���肪����΁g�����ށh������
    bpp->wazaSickCounter[ WAZASICK_AKUMU ] = 0;
    break;
  }
}
//=============================================================================================
/**
 * ���U�n��Ԉُ���񕜂�����
 *
 * @param   pp
 * @param   sick
 *
 */
//=============================================================================================
void BPP_CureWazaSick( BTL_POKEPARAM* bpp, WazaSick sick )
{
  if( BTL_CALC_IsBasicSickID(sick) )
  {
    BPP_CurePokeSick( bpp );
  }
  else
  {
    bpp->sickCont[ sick ] = BPP_SICKCONT_MakeNull();
    bpp->wazaSickCounter[ sick ] = 0;
  }
}
//=============================================================================================
/**
 * ����|�P�����Ɉˑ����Ă����Ԉُ���񕜂�����
 *
 * @param   pp
 * @param   depend_pokeID
 */
//=============================================================================================
void BPP_CureWazaSickDependPoke( BTL_POKEPARAM* bpp, u8 depend_pokeID, BppCureWazaSickDependPokeCallback callbackFunc, void* callbackArg )
{
  u32 i;
  u8 fCure;
  for(i=0; i<WAZASICK_MAX; ++i)
  {
    switch( bpp->sickCont[i].type ){
    case WAZASICK_CONT_POKE:
      fCure = ( bpp->sickCont[i].poke.ID == depend_pokeID );
      break;
    case WAZASICK_CONT_POKETURN:
      fCure = ( bpp->sickCont[i].poketurn.pokeID == depend_pokeID );
      break;
    default:
      fCure = FALSE;
    }
    if( fCure ){
      bpp->sickCont[i] = BPP_SICKCONT_MakeNull();
      cureDependSick( bpp, i );
      callbackFunc( callbackArg, bpp, i, depend_pokeID );
    }
  }
}
//=============================================================================================
/**
 * �|�P������Ԉُ�ɂ������Ă��邩�`�F�b�N
 *
 * @param   pp
 *
 * @retval  PokeSick    �������Ă����Ԉُ�̎��ʎq�i�������Ă��Ȃ��ꍇ POKESICK_NULL�j
 */
//=============================================================================================
PokeSick BPP_GetPokeSick( const BTL_POKEPARAM* pp )
{
  u32 i;
  for(i=POKESICK_ORIGIN; i<POKESICK_MAX; ++i)
  {
    if( pp->sickCont[i].type != WAZASICK_CONT_NONE ){
      return i;
    }
  }
  return POKESICK_NULL;
}
//=============================================================================================
/**
 * ����̏�Ԉُ�ɂ������Ă��邩�`�F�b�N
 *
 * @param   pp
 * @param   sickType
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_CheckSick( const BTL_POKEPARAM* bpp, WazaSick sickType )
{
  GF_ASSERT(sickType < NELEMS(bpp->sickCont));

  if( bpp->sickCont[ sickType ].type != WAZASICK_CONT_NONE )
  {
    if( (sickType == WAZASICK_WAZALOCK) || (sickType == WAZASICK_ENCORE) ){
      BTL_Printf("�Y���ُ�ɂȂ��Ă܂� pokeID=%d, sick=%d\n", bpp->coreParam.myID, sickType);
    }
    return TRUE;
  }
  return FALSE;
}
//=============================================================================================
/**
 * �����Ԉُ�ɐݒ肳�ꂽ�p�����[�^�l�i8bit�j���擾
 *
 * @param   pp
 * @param   sick
 *
 * @retval  u8
 */
//=============================================================================================
u16 BPP_GetSickParam( const BTL_POKEPARAM* pp, WazaSick sick )
{
  GF_ASSERT(sick < NELEMS(pp->sickCont));

  switch(  pp->sickCont[sick].type ){
  case WAZASICK_CONT_TURN:
    return pp->sickCont[sick].turn.param;

  case WAZASICK_CONT_POKE:
    return pp->sickCont[sick].poke.ID;

  case WAZASICK_CONT_POKETURN:
    return pp->sickCont[sick].poketurn.pokeID;

  }

  GF_ASSERT(0); // �p���������̂ɌĂяo���ꂽ
  return 0;
}
//=============================================================================================
/**
 * �����Ԉُ�̌p���p�����[�^�擾
 *
 * @param   bpp
 * @param   sick
 *
 * @retval  BPP_SICK_CONT
 */
//=============================================================================================
BPP_SICK_CONT BPP_GetSickCont( const BTL_POKEPARAM* bpp, WazaSick sick )
{
  GF_ASSERT(sick < NELEMS(bpp->sickCont));
  return bpp->sickCont[ sick ];
}
//=============================================================================================
/**
 * �����Ԉُ�̌p���^�[���J�E���^���擾
 *
 * @param   bpp
 * @param   sick
 *
 * @retval  u8
 */
//=============================================================================================
u8 BPP_GetSickTurnCount( const BTL_POKEPARAM* bpp, WazaSick sick )
{
  GF_ASSERT(sick < NELEMS(bpp->sickCont));
  return bpp->wazaSickCounter[ sick ];
}
//=============================================================================================
/**
 * ��Ԉُ�̃^�[���`�F�b�N�Ō���HP�̗ʂ��v�Z
 *
 * @param   pp
 *
 * @retval  int
 */
//=============================================================================================
int BPP_CalcSickDamage( const BTL_POKEPARAM* bpp, WazaSick sick )
{
  if( BPP_CheckSick(bpp, sick) )
  {
    switch( sick ){
    case WAZASICK_DOKU:
      // �u�ǂ��ǂ��v��ԂȂ�^�[�����Ń_���[�W����
      if( BPP_SICKCONT_IsMoudokuCont(bpp->sickCont[sick]) ){
        return BTL_CALC_QuotMaxHP(bpp, 16) * bpp->wazaSickCounter[sick];
      }else{
        return BTL_CALC_QuotMaxHP( bpp, 8 );
      }
      break;

    case WAZASICK_YAKEDO:
      return BTL_CALC_QuotMaxHP( bpp, 8 );

    case WAZASICK_AKUMU:
      if( BPP_CheckSick(bpp, WAZASICK_NEMURI) ){
        return BTL_CALC_QuotMaxHP( bpp, 4 );
      }
      break;

    case WAZASICK_NOROI:
      return BTL_CALC_QuotMaxHP( bpp, 4 );

    default:
      return 0;
    }
  }
  return 0;
}
//----------------------------------------------------------------------------------
/**
 * ���U�n��Ԉُ�̃J�E���^�N���A
 *
 * @param   pp
 */
//----------------------------------------------------------------------------------
static void clearWazaSickWork( BTL_POKEPARAM* bpp, BOOL fPokeSickInclude )
{
  u32 i, start;

  start = (fPokeSickInclude)? 0 : POKESICK_MAX;

  for(i=start; i<NELEMS(bpp->sickCont); ++i){
    bpp->sickCont[i].raw = 0;
    bpp->sickCont[i].type = WAZASICK_CONT_NONE;
  }
  GFL_STD_MemClear( bpp->wazaSickCounter, sizeof(bpp->wazaSickCounter) );
}

//=============================================================================================
/**
 * ��ɓ��ꂵ�����̃^�[���i���o�[���Z�b�g
 *
 * @param   pp
 * @param   turn
 *
 */
//=============================================================================================
void BPP_SetAppearTurn( BTL_POKEPARAM* bpp, u16 turn )
{
  GF_ASSERT(turn < BTL_TURNCOUNT_MAX);
  bpp->appearedTurn = turn;
  bpp->turnCount = 0;
  dmgrecClearWork( bpp );
}
//=============================================================================================
/**
 * �^�[���I������
 *
 * @param   pp
 *
 */
//=============================================================================================
void BPP_TurnCheck( BTL_POKEPARAM* bpp )
{
  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );

  if( bpp->turnCount < BTL_TURNCOUNT_MAX )
  {
    bpp->turnCount++;
  }

  dmgrecFwdTurn( bpp );
}
//=============================================================================================
/**
 * �^�[���t���O�̌ʓr���N���A
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BPP_TURNFLAG_ForceOff( BTL_POKEPARAM* bpp, BppTurnFlag flagID )
{
  flgbuf_reset( bpp->turnFlag, flagID );
}
//=============================================================================================
/**
 * ���S�ɂ��e���ԃN���A
 *
 * @param   pp
 */
//=============================================================================================
void BPP_Clear_ForDead( BTL_POKEPARAM* bpp )
{
  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
  flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

  clearUsedWazaFlag( bpp );
  clearCounter( bpp );
  BPP_MIGAWARI_Delete( bpp );
  BPP_CombiWaza_ClearParam( bpp );

  clearWazaSickWork( bpp, TRUE );
//  ConfrontRec_Clear( bpp );
  Effrank_Init( &bpp->varyParam );

  if( bpp->coreParam.ppFake ){
    bpp->coreParam.fFakeEnable = TRUE;
  }
  bpp->formNo = bpp->coreParam.defaultFormNo;
}
//=============================================================================================
/**
 * �ޏ�ɂ��e���ԃN���A
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_Clear_ForOut( BTL_POKEPARAM* bpp )
{
  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
  /*
   * ���o�g���^�b�`�ň����p�������L�^���Ă��邽�߁A
   * �ޏꎞ�Ɍp���t���O�N���A�͂����Ă͂����Ȃ�
   */

  clearUsedWazaFlag( bpp );
  clearCounter( bpp );
  BPP_CombiWaza_ClearParam( bpp );

//  ConfrontRec_Clear( bpp );
  if( bpp->coreParam.ppFake ){
    bpp->coreParam.fFakeEnable = TRUE;
  }
  bpp->formNo = bpp->coreParam.defaultFormNo;


}
//=============================================================================================
/**
 * �ē���ɂ��e���ԏ�����
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_Clear_ForIn( BTL_POKEPARAM* bpp )
{
  setupBySrcData( bpp, bpp->coreParam.ppSrc, FALSE );
  flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

  clearWazaSickWork( bpp, FALSE );
  Effrank_Init( &bpp->varyParam );
}
//=============================================================================================
/**
 * �o�g���^�b�`�ɂ��p�����[�^�󂯌p��
 *
 * @param   target    �󂯌p����
 * @param   user      �󂯌p�����鑤
 */
//=============================================================================================
void BPP_BatonTouchParam( BTL_POKEPARAM* target, const BTL_POKEPARAM* user )
{
  u32 i;

  target->varyParam = user->varyParam;

  BTL_Printf("[%d]->[%d]�փo�g���^�b�`�ň����p���ꂽ:�h�䃉���N=%d, ���h�����N=%d\n",
    user->coreParam.myID, target->coreParam.myID,
    target->varyParam.defence, target->varyParam.sp_defence);

  // ����̏�Ԉُ�����̂܂܈����p��
  for(i=0; i<WAZASICK_MAX; ++i)
  {
    if( (user->sickCont[i].type != WAZASICK_CONT_NONE)
    &&  (BTL_SICK_CheckBatonTouch(i))
    ){
      target->sickCont[i] = user->sickCont[i];
      target->wazaSickCounter[i] = user->wazaSickCounter[i];
    }
  }

  // ����̌p���t���O�������p���i���̂Ƃ���p���[�g���b�N�����j
  if( BPP_CONTFLAG_Get(user, BPP_CONTFLG_POWERTRICK) ){
    u8 atk, def;
    BPP_CONTFLAG_Set(target, BPP_CONTFLG_POWERTRICK);
    atk = BPP_GetValue_Base( target, BPP_ATTACK );
    def = BPP_GetValue_Base( target, BPP_DEFENCE );
    BPP_SetBaseStatus( target, BPP_ATTACK, def );
    BPP_SetBaseStatus( target, BPP_DEFENCE, atk );
    BTL_Printf("�p���[�g���b�N�����p��: Atk(%d) <-> Def(%d)\n", atk, def);
  }
}

//=============================================================================================
/**
 * �^�C�v��������
 *
 * @param   pp
 * @param   type
 */
//=============================================================================================
void BPP_ChangePokeType( BTL_POKEPARAM* bpp, PokeTypePair type )
{
  bpp->baseParam.type1 = PokeTypePair_GetType1( type );
  bpp->baseParam.type2 = PokeTypePair_GetType2( type );
}
//=============================================================================================
/**
 * �Ƃ�������������
 *
 * @param   pp
 * @param   tok
 */
//=============================================================================================
void BPP_ChangeTokusei( BTL_POKEPARAM* bpp, PokeTokusei tok )
{
  bpp->tokusei = tok;
}
//=============================================================================================
/**
 * �t�H�����ύX�i�T�[�o�����p�F�\�[�X�f�[�^���������E�p�����[�^�ύX�Ή��j
 *
 * @param   pp
 * @param   formNo
 */
//=============================================================================================
void BPP_ChangeFormPutSrcData( BTL_POKEPARAM* bpp, u8 formNo )
{
  bpp->formNo = formNo;
  PP_Put( (POKEMON_PARAM*)(bpp->coreParam.ppSrc), ID_PARA_form_no, formNo );
  setupBySrcDataBase( bpp, bpp->coreParam.ppSrc );
}

//=============================================================================================
/**
 * �t�H�����ύX�i�N���C�A���g�����p�F�\�[�X�f�[�^�͏��������Ȃ��j
 *
 * @param   bpp
 * @param   formNo
 */
//=============================================================================================
void BPP_ChangeForm( BTL_POKEPARAM* bpp, u8 formNo )
{
  bpp->formNo = formNo;
}


//=============================================================================================
/**
 * ���L�A�C�e���폜�i���L�A�C�e���������Ȃ邾���j
 *
 * @param   pp
 */
//=============================================================================================
void BPP_RemoveItem( BTL_POKEPARAM* bpp )
{
  bpp->coreParam.usedItem = bpp->coreParam.item;
  bpp->coreParam.item = ITEM_DUMMY_DATA;
}
//=============================================================================================
/**
 * ���L�A�C�e������i���L�A�C�e���������Ȃ�A����o�b�t�@�ɋL������j
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_ConsumeItem( BTL_POKEPARAM* bpp )
{
  bpp->coreParam.usedItem = bpp->coreParam.item;
  bpp->coreParam.item = ITEM_DUMMY_DATA;
}
//=============================================================================================
/**
 * �A�C�e��������̃N���A
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_ClearConsumedItem( BTL_POKEPARAM* bpp )
{
  bpp->coreParam.usedItem = ITEM_DUMMY_DATA;
}
//=============================================================================================
/**
 * ������A�C�e���i���o�[��Ԃ�
 *
 * @param   bpp
 *
 * @retval  u16   ������A�C�e���i���o�[�i����Ă��Ȃ����ITEM_DUMMY_DATA�j
 */
//=============================================================================================
u16 BPP_GetConsumedItem( const BTL_POKEPARAM* bpp )
{
  return bpp->coreParam.usedItem;
}


//=============================================================================================
/**
 * ���U�v���Z�X�I���ʒm
 *
 * @param   bpp
 * @param   actTargetPos  actWaza �̑Ώۈʒu
 * @param   fActEnable    actWaza �����ʂ����������i���U�G�t�F�N�g���o�邩�j
 * @param   actWaza       ���s���ꂽ���U�i
 * @param   orgWaza       �N���C�A���g���I���������U�i��т��ӂ铙�A�h�����U�̏ꍇ�̂� actWaza �ƈقȂ�j
 */
//=============================================================================================
void BPP_UpdateWazaProcResult( BTL_POKEPARAM* bpp, BtlPokePos actTargetPos, BOOL fActEnable, WazaID actWaza, WazaID orgWaza )
{
  WazaID  prevActWaza = bpp->prevActWazaID;

  bpp->prevSelectWazaID = orgWaza;
  bpp->prevActWazaID = actWaza;
  bpp->prevTargetPos = actTargetPos;

  if( prevActWaza == actWaza )
  {
    if( fActEnable ){
      bpp->wazaContCounter++;
    }else{
      bpp->wazaContCounter = 0;
    }
  }
  else{
    bpp->wazaContCounter = ( fActEnable )? 1 : 0;
  }
}

//=============================================================================================
/**
 * �������U��A������o���Ă��邩�Ԃ��i�A�����Ă��Ȃ��ꍇ��0�A�A������1�I���W���̒l���Ԃ�j
 *
 * @param   pp
 *
 * @retval  u32
 */
//=============================================================================================
u32 BPP_GetWazaContCounter( const BTL_POKEPARAM* bpp )
{
  return bpp->wazaContCounter;
}
//=============================================================================================
/**
 * ���߂Ɏ��s���ꂽ���U�i���o�[��Ԃ��i��ɏo�Ă���g�������U�������ꍇ�� WAZANO_NULL �j
 *
 * @param   pp
 *
 * @retval  WazaID
 */
//=============================================================================================
WazaID BPP_GetPrevWazaID( const BTL_POKEPARAM* bpp )
{
  return bpp->prevActWazaID;
}
//=============================================================================================
/**
 * ���߂ɑI�����ꂽ���U�i���o�[��Ԃ��i�h�����U��I�������ꍇ�ȂǁABPP_GetPrevWazaID�ƒl���قȂ�ꍇ������j
 *
 * @param   bpp
 *
 * @retval  WazaID
 */
//=============================================================================================
WazaID BPP_GetPrevOrgWazaID( const BTL_POKEPARAM* bpp )
{
  return bpp->prevSelectWazaID;
}

//=============================================================================================
/**
 * ���O�ɑ_��������̈ʒu��Ԃ�
 *
 * @param   pp
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BPP_GetPrevTargetPos( const BTL_POKEPARAM* pp )
{
  return pp->prevTargetPos;
}


//=============================================================================================
/**
 * �̏d��ݒ�
 *
 * @param   bpp
 * @param   weight
 */
//=============================================================================================
void BPP_SetWeight( BTL_POKEPARAM* bpp, u16 weight )
{
  if( weight < BTL_POKE_WEIGHT_MIN ){
    weight = BTL_POKE_WEIGHT_MIN;
  }
  bpp->weight = weight;
}
//=============================================================================================
/**
 * �̏d���擾
 *
 * @param   bpp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BPP_GetWeight( const BTL_POKEPARAM* bpp )
{
  u16 weight = bpp->weight;

  if( IsMatchTokusei(bpp, POKETOKUSEI_HEVIMETARU) ){
    weight *= 2;
  }else if( IsMatchTokusei(bpp, POKETOKUSEI_RAITOMETARU) ){
    weight /= 2;
  }

  if( IsMatchItem(bpp, ITEM_KARUISI) ){
    weight /= 2;
  }

  if( weight < BTL_POKE_WEIGHT_MIN ){
    weight = BTL_POKE_WEIGHT_MIN;
  }

  return weight;
}
//----------------------------------------------------------------------------------
/**
 * ���U�_���[�W���R�[�h�F���[�N������
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void dmgrecClearWork( BTL_POKEPARAM* bpp )
{
  GFL_STD_MemClear( bpp->wazaDamageRec, sizeof(bpp->wazaDamageRec) );
  GFL_STD_MemClear( bpp->dmgrecCount, sizeof(bpp->dmgrecCount) );
  bpp->dmgrecTurnPtr = 0;
  bpp->dmgrecPtr = 0;
}
//----------------------------------------------------------------------------------
/**
 * ���U�_���[�W���R�[�h�F�^�[���`�F�b�N
 *
 * @param   pp
 */
//----------------------------------------------------------------------------------
static void dmgrecFwdTurn( BTL_POKEPARAM* bpp )
{
  if( ++(bpp->dmgrecTurnPtr) >= WAZADMG_REC_TURN_MAX ){
    bpp->dmgrecTurnPtr = 0;
  }
  bpp->dmgrecCount[ bpp->dmgrecTurnPtr ] = 0;
}
//=============================================================================================
/**
 * ���U�_���[�W���R�[�h�F�P���L�^
 *
 * @param   pp
 * @param   rec
 */
//=============================================================================================
void BPP_WAZADMGREC_Add( BTL_POKEPARAM* bpp, const BPP_WAZADMG_REC* rec )
{
  u8 turn, idx;

  turn = bpp->dmgrecTurnPtr;
  idx = bpp->dmgrecCount[ turn ];

  // �ő�L�^���𒴂��Ă�����Â����̂�j������
  if( idx == WAZADMG_REC_MAX )
  {
    u32 i;
    for(i=0; i<(WAZADMG_REC_MAX-1); ++i){
      bpp->wazaDamageRec[ turn ][ i ] = bpp->wazaDamageRec[ turn ][ i+1 ];
    }
    --idx;
  }

  bpp->wazaDamageRec[ turn ][ idx ] = *rec;

  if( bpp->dmgrecCount[ turn ] < WAZADMG_REC_MAX ){
    bpp->dmgrecCount[ turn ]++;
  }
}
//=============================================================================================
/**
 * ���U�_���[�W���R�[�h�F�w��^�[���̋L�^�������擾
 *
 * @param   pp
 * @param   turn_ridx   �k��^�[�����i0�Ȃ瓖�Y�^�[���̋L�^�j
 */
//=============================================================================================
u8 BPP_WAZADMGREC_GetCount( const BTL_POKEPARAM* bpp, u8 turn_ridx )
{
  if( turn_ridx < WAZADMG_REC_TURN_MAX )
  {
    int turn = bpp->dmgrecTurnPtr - turn_ridx;
    if( turn < 0 ){
      turn += WAZADMG_REC_TURN_MAX;
    }
    return bpp->dmgrecCount[ turn ];
  }
  else
  {
    GF_ASSERT( 0 );
    return 0;
  }
}
//=============================================================================================
/**
 * ���U�_���[�W���R�[�h�F���R�[�h���擾
 *
 * @param   bpp
 * @param   turn_ridx   �k��^�[�����i0�Ȃ瓖�Y�^�[���̋L�^�j
 * @param   rec_ridx    �����ڂ̃��R�[�h���H�i0=�ŐV�j
 * @param   dst         [out] �擾�惏�[�N�A�h���X
 *
 * @retval  BOOL        ���R�[�h���擾�ł�����TRUE�^����ȏ�̋L�^�������ꍇ�Ȃ�FALSE
 */
//=============================================================================================
BOOL BPP_WAZADMGREC_Get( const BTL_POKEPARAM* bpp, u8 turn_ridx, u8 rec_ridx, BPP_WAZADMG_REC* dst )
{
  if( turn_ridx < WAZADMG_REC_TURN_MAX )
  {
    int turn = bpp->dmgrecTurnPtr - turn_ridx;
    if( turn < 0 ){
      turn += WAZADMG_REC_TURN_MAX;
    }
    if( rec_ridx < bpp->dmgrecCount[ turn ] )
    {
      u8 idx = bpp->dmgrecCount[ turn ] - rec_ridx - 1;
      *dst = bpp->wazaDamageRec[ turn ][ idx ];
      return TRUE;
    }
  }
  return FALSE;
}

//=============================================================================================
/**
 * �J�E���^�l���Z�b�g
 *
 * @param   bpp
 * @param   cnt     �J�E���^�p�r
 * @param   value   �J�E���^�l
 */
//=============================================================================================
void BPP_COUNTER_Set( BTL_POKEPARAM* bpp, BppCounter cnt, u8 value )
{
  GF_ASSERT(cnt < BPP_COUNTER_MAX);
  bpp->counter[ cnt ] = value;
}
//=============================================================================================
/**
 * �J�E���^�l���擾
 *
 * @param   bpp
 * @param   cnt   �J�E���^�p�r
 *
 * @retval  u8
 */
//=============================================================================================
u8 BPP_COUNTER_Get( const BTL_POKEPARAM* bpp, BppCounter cnt )
{
  GF_ASSERT(cnt < BPP_COUNTER_MAX);
  return bpp->counter[ cnt ];
}

//=============================================================================================
/**
 * �o���l���Z�i�T�[�o�p�FSrc�|�P�����f�[�^�ւ̔��f�͍s��Ȃ��j
 *
 * @param   bpp
 * @param   expRest  [io] ���Z����o���l�^���x���A�b�v���͎c��̉��Z�o���l
 * @param   info     [out] ���x���A�b�v���Ɋe��p�����[�^�̏㏸�l���i�[����
 *
 * @retval  BOOL    ���x���A�b�v������TRUE
 */
//=============================================================================================
BOOL BPP_AddExp( BTL_POKEPARAM* bpp, u32* expRest, BTL_LEVELUP_INFO* info )
{
  if( bpp->baseParam.level < PTL_LEVEL_MAX )
  {
    u32 expNow, expSum, expBorder;

    expNow = bpp->coreParam.exp;
    expSum = expNow + (*expRest);
    expBorder = POKETOOL_GetMinExp( bpp->coreParam.monsno, bpp->formNo, bpp->baseParam.level+1 );

    if( expSum >= expBorder )
    {
      u32 expAdd = (expBorder - expNow);
      u16 prevHP   = bpp->coreParam.hpMax;
      info->atk    = bpp->baseParam.attack;
      info->def    = bpp->baseParam.defence;
      info->sp_atk = bpp->baseParam.sp_attack;
      info->sp_def = bpp->baseParam.sp_defence;
      info->agi    = bpp->baseParam.agility;

      bpp->coreParam.exp = (expNow + expAdd);
      PP_Put( (POKEMON_PARAM*)(bpp->coreParam.ppSrc), ID_PARA_exp, bpp->coreParam.exp );
      PP_Renew( (POKEMON_PARAM*)(bpp->coreParam.ppSrc) );

      bpp->coreParam.hpMax = PP_Get( bpp->coreParam.ppSrc, ID_PARA_hpmax, 0 );
      bpp->baseParam.level = PP_Get( bpp->coreParam.ppSrc, ID_PARA_level, 0 );
      bpp->baseParam.attack = PP_Get( bpp->coreParam.ppSrc, ID_PARA_pow, 0 );
      bpp->baseParam.defence = PP_Get( bpp->coreParam.ppSrc, ID_PARA_def, 0 );
      bpp->baseParam.sp_attack = PP_Get( bpp->coreParam.ppSrc, ID_PARA_spepow, 0 );
      bpp->baseParam.sp_defence = PP_Get( bpp->coreParam.ppSrc, ID_PARA_spedef, 0 );
      bpp->baseParam.agility = PP_Get( bpp->coreParam.ppSrc, ID_PARA_agi, 0 );

      info->level  = bpp->baseParam.level;
      info->hp     = bpp->coreParam.hpMax - prevHP;
      info->atk    = bpp->baseParam.attack - info->atk;
      info->def    = bpp->baseParam.defence - info->def;
      info->sp_atk = bpp->baseParam.sp_attack - info->sp_atk;
      info->sp_def = bpp->baseParam.sp_defence - info->sp_def;
      info->agi    = bpp->baseParam.agility - info->agi;

      bpp->coreParam.hp += info->hp;
      PP_Put((POKEMON_PARAM*)(bpp->coreParam.ppSrc), ID_PARA_hp, bpp->coreParam.hp );

//      PP_Put( (POKEMON_PARAM*)(bpp->coreParam.ppSrc), ID_PARA_exp, bpp->exp );
//      PP_Renew( (POKEMON_PARAM*)(bpp->coreParam.ppSrc) );

      *expRest -= expAdd;
      return TRUE;
    }
    else
    {
      BTL_Printf("pp[%p]�Ɍo���l %d->%d\n", bpp->coreParam.ppSrc, bpp->coreParam.exp, expSum);
      bpp->coreParam.exp = expSum;
      PP_Put( (POKEMON_PARAM*)(bpp->coreParam.ppSrc), ID_PARA_exp, bpp->coreParam.exp );
      BTL_Printf("  ... exp=%d=%d\n", PP_Get(bpp->coreParam.ppSrc, ID_PARA_exp, NULL),bpp->coreParam.exp);
    }
  }

  GFL_STD_MemClear( info, sizeof(*info) );
  return FALSE;
}
//=============================================================================================
/**
 * ���x��100�ɂȂ�܂łɕK�v�Ȍo���l���擾
 *
 * @param   bpp
 *
 * @retval  u32
 */
//=============================================================================================
u32 BPP_GetExpMargin( const BTL_POKEPARAM* bpp )
{
  return POKETOOL_GetMinExp( bpp->coreParam.monsno, bpp->formNo, 100 );
}

//=============================================================================================
/**
 * ���x���A�b�v�p�����[�^�𔽉f������i�N���C�A���g�p�j
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_ReflectLevelup( BTL_POKEPARAM* bpp, u8 nextLevel, u8 hpMax, u8 atk, u8 def, u8 spAtk, u8 spDef, u8 agi )
{
  bpp->coreParam.exp = POKETOOL_GetMinExp( bpp->coreParam.monsno, bpp->formNo, nextLevel );

  bpp->coreParam.hp     += hpMax;
  bpp->coreParam.hpMax  += hpMax;

  bpp->baseParam.level = nextLevel;
  bpp->baseParam.attack     += atk;
  bpp->baseParam.defence    += def;
  bpp->baseParam.sp_attack  += spAtk;
  bpp->baseParam.sp_defence += spDef;
  bpp->baseParam.agility    += agi;
}
//=============================================================================================
/**
 * �o���l�A�b�v�𔽉f������i�N���C�A���g�p�j
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_ReflectExpAdd( BTL_POKEPARAM* bpp )
{
  POKEMON_PARAM* pp = (POKEMON_PARAM*)(bpp->coreParam.ppSrc);
  bpp->coreParam.exp = PP_Get( pp, ID_PARA_exp, NULL );
}

//=============================================================================================
/**
 * Src�|�P�����f�[�^�Ɍ��݂̃p�����[�^�𔽉f
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_ReflectToPP( BTL_POKEPARAM* bpp )
{
  POKEMON_PARAM* pp = (POKEMON_PARAM*)(bpp->coreParam.ppSrc);
  u32 i;

  PP_Put( pp, ID_PARA_exp, bpp->coreParam.exp );
  PP_Put( pp, ID_PARA_hp, bpp->coreParam.hp );
  if( bpp->coreParam.hp )
  {
    PokeSick  sick = BPP_GetPokeSick( bpp );
    PP_SetSick( pp, sick );
  }
  else{
    PP_SetSick( pp, POKESICK_NULL );
  }

  for(i=0; i<PTL_WAZA_MAX; ++i)
  {
    PP_SetWazaPos( pp, bpp->waza[i].number, i );
    PP_Put( pp, ID_PARA_pp1+i, bpp->waza[i].pp );
  }

  PP_Put( pp, ID_PARA_item, bpp->coreParam.item );
  PP_Put( pp, ID_PARA_form_no, bpp->formNo );
}
//=============================================================================================
/**
 * Src�|�P�����f�[�^����p�����[�^���č\�z
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_ReflectByPP( BTL_POKEPARAM* bpp )
{
  setupBySrcData( bpp, bpp->coreParam.ppSrc, TRUE );
}
//=============================================================================================
/**
 * �t�F�C�N��PP���L���ȏ�Ԃ��ǂ�������
 *
 * @param   bpp
 */
//=============================================================================================
BOOL BPP_IsFakeEnable( const BTL_POKEPARAM* bpp )
{
  return bpp->coreParam.fFakeEnable;
}
//=============================================================================================
/**
 * �t�F�C�N��PP�𖳌��ɂ���
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_FakeDisable( BTL_POKEPARAM* bpp )
{
  bpp->coreParam.fFakeEnable = FALSE;
}


//=============================================================================================
/**
 * Src�|�P�����f�[�^�|�C���^�������ւ���
 *
 * @param   bpp
 * @param   pp
 */
//=============================================================================================
void BPP_SetSrcPP( BTL_POKEPARAM* bpp, POKEMON_PARAM* pp )
{
  {
    u16 monsno = PP_Get( pp, ID_PARA_monsno, NULL );
    GF_ASSERT_MSG( (monsno == bpp->coreParam.monsno), "bppMonsNo=%d, ppMonsNo=%d", bpp->coreParam.monsno, monsno );
  }
  bpp->coreParam.ppSrc = pp;
}

//---------------------------------------------------------------------------------------------
// bit�t���O�o�b�t�@����
//---------------------------------------------------------------------------------------------
static inline void flgbuf_clear( u8* buf, u32 size )
{
  GFL_STD_MemClear( buf, size );
}
static inline void flgbuf_set( u8* buf, u32 flagID )
{
  {
    u8 byte = flagID / 8;
    u8 bit = (1 << (flagID%8));
    buf[ byte ] |= bit;
  }
}
static inline void flgbuf_reset( u8* buf, u32 flagID )
{
  {
    u8 byte = flagID / 8;
    u8 bit = (1 << (flagID%8));
    buf[ byte ] &= (~bit);
  }
}
static inline BOOL flgbuf_get( const u8* buf, u32 flagID )
{
//  GF_ASSERT(flagID<BPP_TURNFLG_MAX);
  {
    u8 byte = flagID / 8;
    u8 bit = (1 << (flagID%8));
    return (buf[ byte ] & bit) != 0;
  }
}

//---------------------------------------------------------------------------------------------
// �ϐg�p���[�N����
//---------------------------------------------------------------------------------------------

BOOL BPP_HENSIN_Set( BTL_POKEPARAM* bpp, const BTL_POKEPARAM* target )
{
  if( (bpp->coreParam.fHensin == FALSE)
  &&  (target->coreParam.fHensin == FALSE)
  &&  (target->migawariHP == 0)
  ){
    static BPP_CORE_PARAM  core = {0};
    int i;

    core = bpp->coreParam;

    *bpp = *target;
    bpp->coreParam = core;

    // ���U��MAXPP�͍ő�5�ɑ�����
    for(i=0; i<PTL_WAZA_MAX; ++i)
    {
      if( bpp->waza[i].number != WAZANO_NULL )
      {
        if( bpp->waza[i].ppMax > 5 ){
          bpp->waza[i].ppMax = 5;
        }
        bpp->waza[i].pp = bpp->waza[i].ppMax;
        bpp->waza[i].usedFlag = FALSE;
      }
    }

    clearWazaSickWork( bpp, TRUE );
    flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
    flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

    bpp->appearedTurn = TURNCOUNT_NULL;
    bpp->turnCount = 0;
    bpp->prevSelectWazaID = WAZANO_NULL;
    bpp->prevActWazaID = WAZANO_NULL;
    bpp->wazaContCounter = 0;

    bpp->coreParam.fHensin = TRUE;

    return TRUE;
  }

  return FALSE;
}

//---------------------------------------------------------------------------------------------
// �݂����֘A
//---------------------------------------------------------------------------------------------

void BPP_MIGAWARI_Create( BTL_POKEPARAM* bpp, u16 migawariHP )
{
  GF_ASSERT(bpp->migawariHP==0);
  bpp->migawariHP = migawariHP;
}
void BPP_MIGAWARI_Delete( BTL_POKEPARAM* bpp )
{
  bpp->migawariHP = 0;
}
BOOL BPP_MIGAWARI_IsExist( const BTL_POKEPARAM* bpp )
{
  return bpp->migawariHP != 0;
}

BOOL BPP_MIGAWARI_AddDamage( BTL_POKEPARAM* bpp, u16 damage )
{
  GF_ASSERT(bpp->migawariHP!=0);

  if( bpp->migawariHP <= damage ){
    bpp->migawariHP = 0;
    return TRUE;
  }else{
    bpp->migawariHP -= damage;
    return FALSE;
  }
}

//---------------------------------------------------------------------------------------------
// �ΖʋL�^�֘A
//---------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/**
 * ������
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void ConfrontRec_Clear( BTL_POKEPARAM* bpp )
{
  bpp->confrontRecCount = 0;
}
//=============================================================================================
/**
 * �ΖʋL�^�Z�b�g
 *
 * @param   bpp
 * @param   pokeID    �Ζʂ����|�P����ID
 */
//=============================================================================================
void BPP_CONFRONT_REC_Set( BTL_POKEPARAM* bpp, u8 pokeID )
{
  u32 i;
  for(i=0; i<bpp->confrontRecCount; ++i)
  {
    if( bpp->confrontRec[i] == pokeID ){
      return;
    }
  }
  if( i < NELEMS(bpp->confrontRec) ){
    BTL_Printf("�|�P[%d]��[%d]�ƑΖʂ���\n", bpp->coreParam.myID, pokeID);
    bpp->confrontRec[i] = pokeID;
    bpp->confrontRecCount++;
  }
}
//=============================================================================================
/**
 * �ΖʋL�^�̌����擾
 *
 * @param   bpp
 *
 * @retval  u8    �L�^����
 */
//=============================================================================================
u8 BPP_CONFRONT_REC_GetCount( const BTL_POKEPARAM* bpp )
{
  return bpp->confrontRecCount;
}
//=============================================================================================
/**
 * �ΖʋL�^����Ă���|�P����ID���擾
 *
 * @param   bpp
 * @param   idx   �C���f�b�N�X�i0 <= idx < BPP_CONFRONT_REC_GetCount() )
 *
 * @retval  u8    �|�P����ID
 */
//=============================================================================================
u8 BPP_CONFRONT_REC_GetPokeID( const BTL_POKEPARAM* bpp, u8 idx )
{
  if( idx < bpp->confrontRecCount ){
    return bpp->confrontRec[ idx ];
  }
  return BTL_POKEID_NULL;
}
//=============================================================================================
/**
 * �ߊl���ꂽ���̃{�[���A�C�e��ID���Z�b�g����
 *
 * @param   bpp
 * @param   ballItemID
 */
//=============================================================================================
void BPP_SetCaptureBallID( const BTL_POKEPARAM* bpp, u16 ballItemID )
{
  BALL_ID  ballID = ITEM_GetBallID( ballItemID );
  if( ballID == BALLID_NULL ){
    ballID = BALLID_MONSUTAABOORU;
  }
  PP_Put( (POKEMON_PARAM*)(bpp->coreParam.ppSrc), ID_PARA_get_ball, ballID );
}


//---------------------------------------------------------------------------------------------
// ���̃��U�֘A
//---------------------------------------------------------------------------------------------

//=============================================================================================
/**
 * ���̃��U�Ώہi�㔭�j�Ƃ��Ẵp�����[�^�Z�b�g
 *
 * @param   bpp
 * @param   combiPokeID     �攭�|�P����ID
 * @param   combiUsedWaza   �攭�|�P�������g�������UID
 */
//=============================================================================================
void BPP_CombiWaza_SetParam( BTL_POKEPARAM* bpp, u8 combiPokeID, WazaID combiUsedWaza )
{
  bpp->combiPokeID = combiPokeID;
  bpp->combiWazaID = combiUsedWaza;
}
//=============================================================================================
/**
 * ���̃��U�p�����[�^�擾
 *
 * @param   bpp
 * @param   combiPokeID     [out]
 * @param   combiUsedWaza   [out]
 *
 * @retval  BOOL    �Z�b�g���ꂽ�p�����[�^������ꍇ�ATRUE
 */
//=============================================================================================
BOOL BPP_CombiWaza_GetParam( const BTL_POKEPARAM* bpp, u8* combiPokeID, WazaID* combiUsedWaza )
{
  if( bpp->combiPokeID != BTL_POKEID_NULL )
  {
    *combiPokeID = bpp->combiPokeID;
    *combiUsedWaza = bpp->combiWazaID;
    return TRUE;
  }
  return FALSE;
}
//=============================================================================================
/**
 * ���̃��U�p�����[�^���ݒ肳��Ă��邩�`�F�b�N
 *
 * @param   bpp
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_CombiWaza_IsSetParam( const BTL_POKEPARAM* bpp )
{
  return bpp->combiPokeID != BTL_POKEID_NULL;
}
//=============================================================================================
/**
 * ���̃��U�p�����[�^�N���A
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_CombiWaza_ClearParam( BTL_POKEPARAM* bpp )
{
  if( bpp->combiPokeID!= BTL_POKEID_NULL)
  {
    bpp->combiPokeID = BTL_POKEID_NULL;
    bpp->combiWazaID = WAZANO_NULL;
  }
}


//---------------------------------------------------------------------------------------------
// �Ƃ������E�A�C�e���Q�Ɓi�������A�������������̉e�����l�����āA���ڎQ�Ƃ��Ȃ��悤�ɂ��邽�߁j
//---------------------------------------------------------------------------------------------
static inline BOOL IsMatchTokusei( const BTL_POKEPARAM* bpp, PokeTokusei tokusei )
{
  if( BPP_CheckSick(bpp, WAZASICK_IEKI) ){
    return FALSE;
  }
  return (bpp->tokusei == tokusei);
}
static inline BOOL IsMatchItem( const BTL_POKEPARAM* bpp, u16 itemID )
{
  return (bpp->coreParam.item == itemID);
}

