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
  RANK_STATUS_MIN = 0,
  RANK_STATUS_MAX = 12,
  RANK_STATUS_DEF = 6,

  RANK_CRITICAL_MIN = 0,
  RANK_CRITICAL_MAX = 4,
  RANK_CRITICAL_DEF = 0,

  TURNFLG_BUF_SIZE = (BPP_TURNFLG_MAX/8)+(BPP_TURNFLG_MAX%8!=0),
  ACTFLG_BUF_SIZE  = (BPP_ACTFLG_MAX/8)+(BPP_ACTFLG_MAX%8!=0),
  CONTFLG_BUF_SIZE = (BPP_CONTFLG_MAX/8)+(BPP_CONTFLG_MAX%8!=0),

  TURNCOUNT_NULL = BTL_TURNCOUNT_MAX+1,

  WAZADMG_REC_TURN_MAX = BPP_WAZADMG_REC_TURN_MAX,  ///< ���U�_���[�W���R�[�h�F���^�[�����̋L�^����邩�H
  WAZADMG_REC_MAX = BPP_WAZADMG_REC_MAX,            ///< ���U�_���[�W���R�[�h�F�P�^�[���ɂ��A�������܂ŋL�^���邩�H
};


//--------------------------------------------------------------
/**
 *  ��{�p�����[�^  --�o�g�����A�s�ςȗv�f--
 */
//--------------------------------------------------------------
typedef struct {
  const POKEMON_PARAM*  ppSrc;
  const POKEMON_PARAM*  hensinSrc;
  u16   hp;
  u16   item;
  u16   consumedItem;
  u8    myID;
}BPP_CORE_PARAM;

//--------------------------------------------------------------
/**
 *  ��{�p�����[�^  --�o�g�����A�s�ςȗv�f--
 */
//--------------------------------------------------------------
typedef struct {

  u16 monsno;       ///< �|�P�����i���o�[
  u16 hpMax;        ///< �ő�HP

  u8  attack;       ///< ��������
  u8  defence;      ///< �ڂ�����
  u8  sp_attack;    ///< �Ƃ�����
  u8  sp_defence;   ///< �Ƃ��ڂ�
  u8  agility;      ///< ���΂₳

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
  s8  critical;   ///< �N���e�B�J����

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
  u8   usedFlag;          ///< �g�p�����t���O
}BPP_WAZA;


struct _BTL_POKEPARAM {

  BPP_CORE_PARAM      coreParam;

  BPP_BASE_PARAM      baseParam;
  BPP_VARIABLE_PARAM  varyParam;
  BPP_WAZA            waza[ PTL_WAZA_MAX ];

  PokeTypePair  type;
  u16  tokusei;
  u16  actionAgility;

  u16 turnCount;        ///< �p�����Đ퓬�ɏo�Ă���J�E���^
  u16 appearedTurn;     ///< �퓬�ɏo���^�[�����L�^

  u16 prevWazaID;             ///< ���O�Ɏg�������U�i���o�[
  u16 sameWazaCounter;        ///< �������U�����A���ŏo���Ă��邩�J�E���^
  BtlPokePos  prevTargetPos;  ///< ���O�ɑ_��������

  BPP_SICK_CONT   sickCont[ WAZASICK_MAX ];
  u8  wazaSickCounter[ WAZASICK_MAX ];

  u8  turnFlag[ TURNFLG_BUF_SIZE ];
  u8  actFlag [ ACTFLG_BUF_SIZE ];
  u8  contFlag[ CONTFLG_BUF_SIZE ];
  u8  counter[ BPP_COUNTER_MAX ];

  BPP_WAZADMG_REC  wazaDamageRec[ WAZADMG_REC_TURN_MAX ][ WAZADMG_REC_MAX ];
  u8               dmgrecCount[ WAZADMG_REC_TURN_MAX ];
  u8               dmgrecTurnPtr;
  u8               dmgrecPtr;

  u8  wazaCnt;
  u8  formNo;

//  u32 dmy;
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void setupBySrcData( BTL_POKEPARAM* bpp, const POKEMON_PARAM* srcPP );
static void clearUsedWazaFlag( BTL_POKEPARAM* bpp );
static void clearCounter( BTL_POKEPARAM* bpp );
static void Effrank_Init( BPP_VARIABLE_PARAM* rank );
static void Effrank_Reset( BPP_VARIABLE_PARAM* rank );
static void Effrank_Recover( BPP_VARIABLE_PARAM* rank );
static BppValueID ConvertValueID( const BTL_POKEPARAM* bpp, BppValueID vid );
static const s8* getRankVaryStatusConst( const BTL_POKEPARAM* pp, BppValueID type, s8* min, s8* max );
static s8* getRankVaryStatus( BTL_POKEPARAM* pp, BppValueID type, s8* min, s8* max );
static void clearWazaSickWork( BTL_POKEPARAM* bpp, BOOL fPokeSickInclude );
static void dmgrecClearWork( BTL_POKEPARAM* bpp );
static void dmgrecFwdTurn( BTL_POKEPARAM* bpp );
static inline void flgbuf_clear( u8* buf, u32 size );
static inline void flgbuf_set( u8* buf, u32 flagID );
static inline void flgbuf_reset( u8* buf, u32 flagID );
static inline BOOL flgbuf_get( const u8* buf, u32 flagID );



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
  bpp->coreParam.ppSrc = pp;
  bpp->coreParam.hp = PP_Get( pp, ID_PARA_hp, 0 );
  bpp->coreParam.item = PP_Get( pp, ID_PARA_item, NULL );
  bpp->coreParam.consumedItem = ITEM_DUMMY_DATA;
  bpp->coreParam.hensinSrc = NULL;

  setupBySrcData( bpp, pp );

  // �����N���ʏ�����
  Effrank_Init( &bpp->varyParam );

  // ��Ԉُ탏�[�N������
  clearWazaSickWork( bpp, TRUE );

  // ���̎��_�Ń|�P�����p��Ԉُ�ɂȂ��Ă���Έ����p��
  {
    PokeSick sick = PP_GetSick( pp );
    if( sick == POKESICK_NEMURI )
    {
      u32 turns = BTL_CALC_RandRange( BTL_NEMURI_TURN_MIN, BTL_NEMURI_TURN_MAX );
      bpp->sickCont[WAZASICK_NEMURI] = BPP_SICKCONT_MakeTurn( turns );
      bpp->wazaSickCounter[WAZASICK_NEMURI] = 0;
    }
  }

  // �e�탏�[�N�̈揉����
  bpp->appearedTurn = TURNCOUNT_NULL;
  bpp->turnCount = 0;
  bpp->prevWazaID = WAZANO_NULL;
  bpp->sameWazaCounter = 0;
  bpp->actionAgility = 0;

  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
  flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

  dmgrecClearWork( bpp );

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
static void setupBySrcData( BTL_POKEPARAM* bpp, const POKEMON_PARAM* srcPP )
{
  u16 monsno = PP_Get( srcPP, ID_PARA_monsno, 0 );
  int i;

  // ��{�p�����^������
  bpp->baseParam.monsno = monsno;
  bpp->baseParam.level = PP_Get( srcPP, ID_PARA_level, 0 );
  bpp->baseParam.hpMax = PP_Get( srcPP, ID_PARA_hpmax, 0 );
  bpp->baseParam.attack = PP_Get( srcPP, ID_PARA_pow, 0 );
  bpp->baseParam.defence = PP_Get( srcPP, ID_PARA_def, 0 );
  bpp->baseParam.sp_attack = PP_Get( srcPP, ID_PARA_spepow, 0 );
  bpp->baseParam.sp_defence = PP_Get( srcPP, ID_PARA_spedef, 0 );
  bpp->baseParam.agility = PP_Get( srcPP, ID_PARA_agi, 0 );
  bpp->baseParam.type1 = PP_Get( srcPP, ID_PARA_type1, 0 );
  bpp->baseParam.type2 = PP_Get( srcPP, ID_PARA_type2, 0 );
  bpp->baseParam.sex = PP_GetSex( srcPP );

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
    bpp->waza[i].recoverNumber = bpp->waza[i].number;
  }

  bpp->type = PokeTypePair_Make( bpp->baseParam.type1, bpp->baseParam.type2 );
  bpp->tokusei = PP_Get( srcPP, ID_PARA_speabino, 0 );

  bpp->formNo = PP_Get( srcPP, ID_PARA_form_no, 0 );

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
  rank->critical = RANK_CRITICAL_DEF;
}
static void Effrank_Reset( BPP_VARIABLE_PARAM* rank )
{
  rank->attack = RANK_STATUS_DEF;
  rank->defence = RANK_STATUS_DEF;
  rank->sp_attack = RANK_STATUS_DEF;
  rank->sp_defence = RANK_STATUS_DEF;
  rank->agility = RANK_STATUS_DEF;
  rank->hit = RANK_STATUS_DEF;
  rank->avoid = RANK_STATUS_DEF;
}
static void Effrank_Recover( BPP_VARIABLE_PARAM* rank )
{
  if( rank->attack < RANK_STATUS_DEF ){ rank->attack = RANK_STATUS_DEF; };
  if( rank->defence < RANK_STATUS_DEF ){  rank->defence = RANK_STATUS_DEF; }
  if( rank->sp_attack < RANK_STATUS_DEF ){ rank->sp_attack = RANK_STATUS_DEF; }
  if( rank->sp_defence < RANK_STATUS_DEF ){  rank->sp_defence = RANK_STATUS_DEF; }
  if( rank->agility < RANK_STATUS_DEF ){ rank->agility = RANK_STATUS_DEF; }
  if( rank->hit < RANK_STATUS_DEF ){ rank->hit = RANK_STATUS_DEF; }
  if( rank->avoid < RANK_STATUS_DEF ) { rank->avoid = RANK_STATUS_DEF; }
}



//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

u8 BPP_GetID( const BTL_POKEPARAM* pp )
{
  return pp->coreParam.myID;
}

u16 BPP_GetMonsNo( const BTL_POKEPARAM* pp )
{
  return pp->baseParam.monsno;
}

u8 BPP_WAZA_GetCount( const BTL_POKEPARAM* pp )
{
  return pp->wazaCnt;
}

u8 BPP_WAZA_GetUsedCount( const BTL_POKEPARAM* pp )
{
  u8 cnt, i;
  for(i=0, cnt=0; i<pp->wazaCnt; ++i)
  {
    if( pp->waza[i].usedFlag ){
      ++cnt;
    }
  }
  return cnt;
}

WazaID BPP_WAZA_GetID( const BTL_POKEPARAM* pp, u8 idx )
{
  GF_ASSERT(idx < pp->wazaCnt);
  return pp->waza[idx].number;
}

WazaID BPP_WAZA_GetParticular( const BTL_POKEPARAM* pp, u8 idx, u8* PP, u8* PPMax )
{
  GF_ASSERT(idx < pp->wazaCnt);
  *PP = pp->waza[idx].pp;
  *PPMax = pp->waza[idx].ppMax;

  return pp->waza[idx].number;
}


PokeTypePair BPP_GetPokeType( const BTL_POKEPARAM* pp )
{
  return pp->type;
}

BOOL BPP_IsMatchType( const BTL_POKEPARAM* pp, PokeType type )
{
  PokeTypePair pair = PokeTypePair_Make( pp->baseParam.type1, pp->baseParam.type2 );
  return PokeTypePair_IsMatch( pair, type );
}

const POKEMON_PARAM* BPP_GetSrcData( const BTL_POKEPARAM* bpp )
{
  return bpp->coreParam.ppSrc;
}

//----------------------------------------------------------------------------------
/**
 * ��Ԉُ퓙�̏����ɂ��A�p�����[�^�擾�p��ID��ύX����
 * ������A�p���[�g���b�N�i���������E�ڂ���������ւ���j�ɂ̂ݗ��p���Ă���B
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
  case BPP_ATTACK:
    if( BPP_CheckSick(bpp, WAZASICK_POWERTRICK) ){ vid = BPP_DEFENCE; }
    break;

  case BPP_DEFENCE:
    if( BPP_CheckSick(bpp, WAZASICK_POWERTRICK) ){ vid = BPP_ATTACK; }
    break;
  }
  return vid;
}
//=============================================================================================
/**
 * �����N�␳�t���b�g�ȏ�Ԃ̃p�����[�^�擾
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

  case BPP_HIT_RATIO:         return RANK_STATUS_DEF;
  case BPP_AVOID_RATIO:       return RANK_STATUS_DEF;
  case BPP_CRITICAL_RATIO:    return RANK_CRITICAL_DEF;

  default:
    return BPP_GetValue( bpp, vid );
  };
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
  case BPP_CRITICAL_RATIO:  return bpp->varyParam.critical;

  case BPP_LEVEL:           return bpp->baseParam.level;
  case BPP_HP:              return bpp->coreParam.hp;
  case BPP_MAX_HP:          return bpp->baseParam.hpMax;
  case BPP_SEX:             return bpp->baseParam.sex;

  case BPP_TOKUSEI:         return bpp->tokusei;
  case BPP_FORM:            return bpp->formNo;

  case BPP_HEAVY:           return 50;  // @@@ ���͂Ă��Ƃ�

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
int BPP_GetValue_Critical( const BTL_POKEPARAM* pp, BppValueID vid )
{
  BOOL fFlatParam = FALSE;
  switch( vid ){
  case BPP_ATTACK:     fFlatParam = (pp->varyParam.attack < 0); break;
  case BPP_SP_ATTACK:  fFlatParam = (pp->varyParam.sp_attack < 0); break;
  case BPP_DEFENCE:    fFlatParam = (pp->varyParam.defence > 0); break;
  case BPP_SP_DEFENCE: fFlatParam = (pp->varyParam.sp_defence > 0); break;

  default:
    return BPP_GetValue( pp, vid );
  }

  if( fFlatParam ){
    return BPP_GetValue_Base( pp, vid );
  }else{
    return BPP_GetValue( pp, vid );
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
 * �A�N�V�������N���A�t���O�̎擾
 *
 * @param   pp
 * @param   flagID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_GetActFlag( const BTL_POKEPARAM* pp, BppActFlag flagID )
{
  return flgbuf_get( pp->actFlag, flagID );
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
  if( hp <= (pp->baseParam.hpMax / 8) )
  {
    return BPP_HPBORDER_RED;
  }
  if( hp <= (pp->baseParam.hpMax / 3) )
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
  double r = (double)(pp->coreParam.hp * 100) / (double)(pp->baseParam.hpMax);
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
  u32 i;
  for(i=0; i<PTL_WAZA_MAX; ++i){
    if( pp->waza[i].number == waza ){
      return i;
    }
  }
  return PTL_WAZA_MAX;
}

//-----------------------------
static const s8* getRankVaryStatusConst( const BTL_POKEPARAM* pp, BppValueID type, s8* min, s8* max )
{
  const s8* ptr;

  *min = RANK_STATUS_MIN;
  *max = RANK_STATUS_MAX;

  switch( type ) {
  case BPP_ATTACK:      ptr = &pp->varyParam.attack; break;
  case BPP_DEFENCE:     ptr = &pp->varyParam.defence; break;
  case BPP_SP_ATTACK:   ptr = &pp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE:  ptr = &pp->varyParam.sp_defence; break;
  case BPP_AGILITY:     ptr = &pp->varyParam.agility; break;
  case BPP_HIT_RATIO:   ptr = &pp->varyParam.hit; break;
  case BPP_AVOID_RATIO: ptr = &pp->varyParam.avoid; break;
  case BPP_CRITICAL_RATIO:
    ptr = &pp->varyParam.critical;
    *min = RANK_CRITICAL_MIN;
    *max = RANK_CRITICAL_MAX;
    break;
  default:
    GF_ASSERT(0);
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
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsRankEffectValid( const BTL_POKEPARAM* pp, BppValueID rankType, int volume )
{
  const s8* ptr;
  s8  min, max;

  ptr = getRankVaryStatusConst( pp, rankType, &min, &max );
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
int BPP_RankEffectDownLimit( const BTL_POKEPARAM* pp, BppValueID rankType )
{
  const s8* ptr;
  s8  min, max;

  ptr = getRankVaryStatusConst( pp, rankType, &min, &max );
  return (*ptr) - min;
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
  s8 max = RANK_STATUS_MAX;

  switch( rankType ){
  case BPP_ATTACK:      ptr = &bpp->varyParam.attack; break;
  case BPP_DEFENCE:     ptr = &bpp->varyParam.defence; break;
  case BPP_SP_ATTACK:   ptr = &bpp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE:  ptr = &bpp->varyParam.sp_defence; break;
  case BPP_AGILITY:     ptr = &bpp->varyParam.agility; break;
  case BPP_HIT_RATIO:   ptr = &bpp->varyParam.hit; break;
  case BPP_AVOID_RATIO: ptr = &bpp->varyParam.avoid; break;
  case BPP_CRITICAL_RATIO:
    ptr = &bpp->varyParam.critical;
    max = RANK_CRITICAL_MAX;
    break;
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
  s8 min = RANK_STATUS_MIN;

  switch( rankType ){
  case BPP_ATTACK:       ptr = &bpp->varyParam.attack; break;
  case BPP_DEFENCE:      ptr = &bpp->varyParam.defence; break;
  case BPP_SP_ATTACK:    ptr = &bpp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE:   ptr = &bpp->varyParam.sp_defence; break;
  case BPP_AGILITY:      ptr = &bpp->varyParam.agility; break;
  case BPP_HIT_RATIO:    ptr = &bpp->varyParam.hit; break;
  case BPP_AVOID_RATIO:  ptr = &bpp->varyParam.avoid; break;
  case BPP_CRITICAL_RATIO:
    ptr = &bpp->varyParam.critical;
    min = RANK_CRITICAL_MIN;
    break;
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
  u8 min = RANK_STATUS_MIN;
  u8 max = RANK_STATUS_MAX;

  switch( rankType ){
  case BPP_ATTACK:        ptr = &pp->varyParam.attack; break;
  case BPP_DEFENCE:       ptr = &pp->varyParam.defence; break;
  case BPP_SP_ATTACK:     ptr = &pp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE:    ptr = &pp->varyParam.sp_defence; break;
  case BPP_AGILITY:       ptr = &pp->varyParam.agility; break;
  case BPP_HIT_RATIO:     ptr = &pp->varyParam.hit; break;
  case BPP_AVOID_RATIO:   ptr = &pp->varyParam.avoid; break;
  case BPP_CRITICAL_RATIO:
    ptr = &pp->varyParam.critical;
    min = RANK_CRITICAL_MIN;
    max = RANK_CRITICAL_MAX;
    break;
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
 * �������Ă��郉���N���ʂ��t���b�g�ɖ߂�
 *
 * @param   pp
 */
//=============================================================================================
void BPP_RankRecover( BTL_POKEPARAM* pp )
{
  Effrank_Recover( &pp->varyParam );
}
//=============================================================================================
/**
 * �S�Ẵ����N���ʂ��t���b�g�ɖ߂�
 *
 * @param   pp
 */
//=============================================================================================
void BPP_RankReset( BTL_POKEPARAM* pp )
{
  Effrank_Reset( &pp->varyParam );
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
  if( pp->coreParam.hp > pp->baseParam.hpMax )
  {
    pp->coreParam.hp = pp->baseParam.hpMax;
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
void BPP_WAZA_SetUsedFlag( BTL_POKEPARAM* pp, u8 wazaIdx )
{
  pp->waza[ wazaIdx ].usedFlag = TRUE;
}
//=============================================================================================
/**
 * ���U�i���o�[�㏑��
 *
 * @param   pp
 * @param   wazaIdx       ���Ԗڂ̃��U[0-3]�H
 * @param   waza          �㏑���ハ�U�i���o�[
 * @param   ppMax         PP�ő�l�̏���i0�Ȃ�f�t�H���g�l�j
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
  pWaza->ppMax = WAZADATA_GetMaxPP( waza, 0 );
  if( (ppMax != 0)
  &&  (pWaza->ppMax < ppMax)
  ){
    pWaza->ppMax = ppMax;
  }
  pWaza->pp = pWaza->ppMax;
  BTL_Printf("���U�㏑��: ppMax�w��=%d, ���n:%d\n", ppMax, pWaza->ppMax);
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
 * �A�N�V�������ƃt���O�̃Z�b�g
 *
 * @param   pp
 * @param   flagID
 */
//=============================================================================================
void BPP_ACTFLAG_Set( BTL_POKEPARAM* pp, BppActFlag flagID )
{
  flgbuf_set( pp->actFlag, flagID );
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
  if( sick < POKESICK_MAX )
  {
    PokeSick pokeSick = BPP_GetPokeSick( bpp );
    GF_ASSERT(pokeSick == POKESICK_NULL);
  }

  bpp->sickCont[ sick ] = contParam;
  bpp->wazaSickCounter[sick] = 0;
}


//=============================================================================================
/**
 * �S��Ԉُ�̃^�[���`�F�b�N����
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
    if( bpp->sickCont[sick].type != WAZASICK_CONT_NONE )
    {
      u32 turnMax = BPP_SICCONT_GetTurnMax( bpp->sickCont[sick] );
      BOOL fCure = FALSE;
      BPP_SICK_CONT oldCont;

      oldCont = bpp->sickCont[ sick ];

      // �p���^�[���o�߃`�F�b�N
      if( turnMax )
      {
        u8 n = 1;
        if( (sick == WAZASICK_NEMURI) && (bpp->tokusei == POKETOKUSEI_HAYAOKI) ){
          n = 2;    // �Ƃ������u�͂₨���v�͖���J�E���^�Q�{��
        }
        BTL_Printf("�|�P[%d - %p], ��Ԉُ�[%d] �ő�^�[��=%d, counter=%d ->",
          bpp->coreParam.myID, bpp, sick, turnMax, bpp->wazaSickCounter[sick] );
        bpp->wazaSickCounter[sick] += n;
        OS_TPrintf(" %d (adrs=%p)\n", bpp->wazaSickCounter[sick], &(bpp->wazaSickCounter[sick]));

        if( bpp->wazaSickCounter[sick] >= turnMax )
        {
          bpp->wazaSickCounter[sick] = 0;
          // ���莩�̂̃I�t�͍s���`�F�b�N���ɍs��
          if( sick != WAZASICK_NEMURI ){
            bpp->sickCont[sick].type = WAZASICK_CONT_NONE;
            fCure = TRUE;
          }
        }
      }
      // �i���^�ōő�^�[�������w�肳��Ă�����̂̓J�E���^���C���N�������g
      // �i����A���̋@�\�𗘗p���Ă���̂́u�����ǂ��v�̂݁j
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
 * �|�P�����n��Ԉُ���񕜂�����
 *
 * @param   pp
 *
 */
//=============================================================================================
void BPP_CurePokeSick( BTL_POKEPARAM* pp )
{
  u32 i;
  for(i=POKESICK_ORIGIN; i<POKESICK_MAX; ++i)
  {
    pp->sickCont[ i ] = BPP_SICKCONT_MakeNull();
  }
  pp->sickCont[WAZASICK_AKUMU] = BPP_SICKCONT_MakeNull();  // ���肪����΁g�����ށh������
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
void BPP_CureWazaSick( BTL_POKEPARAM* pp, WazaSick sick )
{
  if( sick < POKESICK_MAX )
  {
    BPP_CurePokeSick( pp );
  }
  else
  {
    pp->sickCont[ sick ] = BPP_SICKCONT_MakeNull();
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
void BPP_CureWazaSickDependPoke( BTL_POKEPARAM* pp, u8 depend_pokeID )
{
  u32 i;
  u8 fCure;
  for(i=0; i<WAZASICK_MAX; ++i)
  {
    switch( pp->sickCont[i].type ){
    case WAZASICK_CONT_POKE:
      fCure = ( pp->sickCont[i].poke.ID == depend_pokeID );
      break;
    case WAZASICK_CONT_POKETURN:
      fCure = ( pp->sickCont[i].poketurn.pokeID == depend_pokeID );
      break;
    default:
      fCure = FALSE;
    }
    if( fCure ){
      pp->sickCont[i] = BPP_SICKCONT_MakeNull();
      if( i == WAZASICK_NEMURI ){
        pp->sickCont[WAZASICK_AKUMU] = BPP_SICKCONT_MakeNull();
      }
    }
  }
}
//=============================================================================================
/**
 * �u�˂ނ�v�^�[���i�s�@���|�P������Ԉُ�`�F�b�N�́A�u���������v��I�񂾏ꍇ�ɂ̂ݍs��
 *
 * @param   pp
 *
 * @retval  BOOL    �ڂ��o�߂��ꍇ��True
 */
//=============================================================================================
BOOL BPP_Nemuri_CheckWake( BTL_POKEPARAM* pp )
{
  if( BPP_CheckSick(pp, WAZASICK_NEMURI) )
  {
    u8 turnMax = BPP_SICCONT_GetTurnMax( pp->sickCont[POKESICK_NEMURI] );
    if( (turnMax != 0 ) && (pp->wazaSickCounter[POKESICK_NEMURI] >= turnMax) )
    {
      pp->sickCont[ POKESICK_NEMURI ] = BPP_SICKCONT_MakeNull();
      pp->sickCont[ WAZASICK_AKUMU ] = BPP_SICKCONT_MakeNull();
     return TRUE;
    }
  }else{
    GF_ASSERT(0);
  }
  return FALSE;
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
BOOL BPP_CheckSick( const BTL_POKEPARAM* pp, WazaSick sickType )
{
  GF_ASSERT(sickType < NELEMS(pp->sickCont));

  return pp->sickCont[ sickType ].type != WAZASICK_CONT_NONE;
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
u8 BPP_GetSickParam( const BTL_POKEPARAM* pp, WazaSick sick )
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
      // �J�E���^��0�Ȃ�ʏ�́u�ǂ��v
      if( bpp->wazaSickCounter[sick] == 0 ){
        return BTL_CALC_QuotMaxHP( bpp, 8 );
      }
      // �J�E���^��1�`�Ȃ�u�ǂ��ǂ��v
      else{
        return (bpp->baseParam.hpMax / 16) * bpp->wazaSickCounter[sick];
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
  BTL_Printf("�|�P[%d]�̏�Ԉُ�J�E���^���N���A���ꂽ\n", bpp->coreParam.myID);
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
void BPP_SetAppearTurn( BTL_POKEPARAM* pp, u16 turn )
{
  GF_ASSERT(turn < BTL_TURNCOUNT_MAX);
  pp->appearedTurn = turn;
  pp->turnCount = 0;
  dmgrecClearWork( pp );
}
//=============================================================================================
/**
 * �^�[���I������
 *
 * @param   pp
 *
 */
//=============================================================================================
void BPP_TurnCheck( BTL_POKEPARAM* pp )
{
  flgbuf_clear( pp->turnFlag, sizeof(pp->turnFlag) );

  if( pp->turnCount < BTL_TURNCOUNT_MAX )
  {
    pp->turnCount++;
  }

  dmgrecFwdTurn( pp );
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
 * �A�N�V�������ƃt���O�̃N���A
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_ACTFLAG_Clear( BTL_POKEPARAM* bpp )
{
  flgbuf_clear( bpp->actFlag, sizeof(bpp->actFlag) );
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
  flgbuf_clear( bpp->actFlag, sizeof(bpp->actFlag) );
  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
  flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

  clearUsedWazaFlag( bpp );
  clearCounter( bpp );

  clearWazaSickWork( bpp, TRUE );
  Effrank_Init( &bpp->varyParam );
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
  flgbuf_clear( bpp->actFlag, sizeof(bpp->actFlag) );
  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
  flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

  clearUsedWazaFlag( bpp );
  clearCounter( bpp );
}
//=============================================================================================
/**
 * ����ɂ��e���ԃN���A
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_Clear_ForIn( BTL_POKEPARAM* bpp )
{
  clearWazaSickWork( bpp, FALSE );
  Effrank_Init( &bpp->varyParam );
}
//=============================================================================================
/**
 * �^�C�v��������
 *
 * @param   pp
 * @param   type
 */
//=============================================================================================
void BPP_ChangePokeType( BTL_POKEPARAM* pp, PokeTypePair type )
{
  pp->type = type;
}
//=============================================================================================
/**
 * �Ƃ�������������
 *
 * @param   pp
 * @param   tok
 */
//=============================================================================================
void BPP_ChangeTokusei( BTL_POKEPARAM* pp, PokeTokusei tok )
{
  pp->tokusei = tok;
}
//=============================================================================================
/**
 * �t�H�����ύX
 *
 * @param   pp
 * @param   formNo
 */
//=============================================================================================
void BPP_ChangeForm( BTL_POKEPARAM* pp, u8 formNo )
{
  pp->formNo = formNo;
}
//=============================================================================================
/**
 * ���L�A�C�e���폜
 *
 * @param   pp
 */
//=============================================================================================
void BPP_RemoveItem( BTL_POKEPARAM* bpp )
{
  bpp->coreParam.consumedItem = bpp->coreParam.item;
  bpp->coreParam.item = ITEM_DUMMY_DATA;
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
  return bpp->coreParam.consumedItem;
}


//=============================================================================================
/**
 * ���O�Ɏg�������U�i���o�[���L�^
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void BPP_UpdatePrevWazaID( BTL_POKEPARAM* pp, WazaID waza, BtlPokePos targetPos )
{
  // @@@ �܂���E�݂���̊֌W
  WazaID prev = pp->prevWazaID;
  if( prev != waza ){
    pp->prevWazaID = waza;
    pp->sameWazaCounter = 0;
  }else
  {
    pp->sameWazaCounter++;
  }
  pp->prevTargetPos = targetPos;
}
//=============================================================================================
/**
 * ���O�Ɏg�������U�i���o�[���N���A
 *
 * @param   pp
 */
//=============================================================================================
void BPP_ResetWazaContConter( BTL_POKEPARAM* bpp )
{
  bpp->sameWazaCounter = 0;
}
//=============================================================================================
/**
 * ���O�Ɏg�������U�i���o�[��Ԃ��i��ɏo�Ă���g�������U�������ꍇ�� WAZANO_NULL �j
 *
 * @param   pp
 *
 * @retval  WazaID
 */
//=============================================================================================
WazaID BPP_GetPrevWazaID( const BTL_POKEPARAM* pp )
{
  return pp->prevWazaID;
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
 * �������U��A������o���Ă��邩�Ԃ��i�A�����Ă��Ȃ��ꍇ��0�A�A������1�I���W���̒l���Ԃ�j
 *
 * @param   pp
 *
 * @retval  u32
 */
//=============================================================================================
u32 BPP_GetWazaContCounter( const BTL_POKEPARAM* pp )
{
  return pp->sameWazaCounter;
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
 * �s�����\�[�g�̎��Ɏg�p���ꂽ�u���΂₳�v���L�^���Ă���
 *�i�Ƃ������A�����N���ʁA�A�C�e���������ʂȂǂ����f���ꂽ���́j
 *
 * @param   bpp
 * @param   actionAgility
 */
//=============================================================================================
void BPP_SetActionAgility( BTL_POKEPARAM* bpp, u16 actionAgility )
{
  bpp->actionAgility = actionAgility;
}
//=============================================================================================
/**
 * �s�����\�[�g�̎��Ɏg�p���ꂽ�u���΂₳�v���擾
 *�i�Ƃ������A�����N���ʁA�A�C�e���������ʂȂǂ����f���ꂽ���́j
 *
 * @param   bpp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BPP_GetActionAgility( const BTL_POKEPARAM* bpp )
{
  return bpp->actionAgility;
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
  BTL_Printf("�o�g���^�b�`�ň����p���ꂽ:�h��=%d, ���h=%d\n",
    target->varyParam.defence, target->varyParam.sp_defence);
  for(i=0; i<WAZASICK_MAX; ++i)
  {
    if( (user->sickCont[i].type != WAZASICK_CONT_NONE)
    &&  (BTL_SICK_CheckBatonTouch(i))
    ){
      target->sickCont[i] = user->sickCont[i];
      target->wazaSickCounter[i] = user->wazaSickCounter[i];
    }
  }
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
  if( (bpp->coreParam.hensinSrc == NULL)
  &&  (target->coreParam.hensinSrc == NULL)
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
    bpp->prevWazaID = WAZANO_NULL;
    bpp->sameWazaCounter = 0;
    bpp->actionAgility = 0;

    bpp->coreParam.hensinSrc = target->coreParam.ppSrc;

    return TRUE;
  }

  return FALSE;
}


