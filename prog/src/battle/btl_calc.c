//=============================================================================================
/**
 * @file  btl_calc.c
 * @brief �|�P����WB �o�g�� -�V�X�e��-  �e��A���l�̌v�Z���[�`��
 * @author  taya
 *
 * @date  2008.09.06  �쐬
 */
//=============================================================================================
#include <gflib.h>

#include "tr_tool\trtype_def.h"
#include "tr_tool\tr_tool.h"

#include "btl_common.h"
#include "btl_util.h"

#include "btl_calc.h"

//--------------------------------------------------------------
/**
 *  Global
 */
//--------------------------------------------------------------
static GFL_STD_RandContext gRandContext = {0};
static u16* gWazaStoreWork = NULL;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL is_include( const u16* tbl, u32 tblElems, u16 wazaID );
static u32 calcWinMoney_Sub( const BSP_TRAINER_DATA* trData, const POKEPARTY * party );


//=============================================================================================
/**
 * �V�X�e��������
 *
 * @param   randContext   [in] �����R���e�L�X�g
 * @param   heapID        [in] �q�[�vID
 */
//=============================================================================================
void BTL_CALC_InitSys( const GFL_STD_RandContext* randContext, HEAPID heapID )
{
  gRandContext = *randContext;
  gWazaStoreWork = GFL_HEAP_AllocClearMemory( heapID, WAZANO_MAX * sizeof(u16) );
}
//=============================================================================================
/**
 *
 *
 * @param   randContext
 */
//=============================================================================================
void BTL_CALC_ResetSys( const GFL_STD_RandContext* randContext )
{
  gRandContext = *randContext;
}

//=============================================================================================
/**
 * �V�X�e���I��
 */
//=============================================================================================
void BTL_CALC_QuitSys( void )
{
  if( gWazaStoreWork )
  {
    GFL_HEAP_FreeMemory( gWazaStoreWork );
    gWazaStoreWork = NULL;
  }
}

//=============================================================================================
/**
 * �����l��Ԃ�
 *  ���Ԃ��l�� 0�`(pattern-1) �͈̔́Bpattern==0���ƁA32bit�S�͈́B
 *
 * @param   pattern   �����̎�肤��l�͈̔�
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CALC_GetRand( u32 range )
{
  u32 result;
  result = GFL_STD_Rand( &gRandContext, range );
  return result;
}

//--------------------------------------------------------------
/**
 *  �X�e�[�^�X�����N�␳�e�[�u��
 */
//--------------------------------------------------------------
static const struct {
  u8  num;
  u8  denom;
}StatusRankTable[] = {
  { 2, 8 }, // x(2/8) = 25%
  { 2, 7 }, // x(2/7) = 29%
  { 2, 6 }, // x(2/6) = 33%
  { 2, 5 }, // x(2/5) = 40%
  { 2, 4 }, // x(2/4) = 50%
  { 2, 3 }, // x(2/3) = 67%
  { 2, 2 }, // x(2/2) = 100%
  { 3, 2 }, // x(3/2) = 150%
  { 4, 2 }, // x(4/2) = 200%
  { 5, 2 }, // x(5/2) = 250%
  { 6, 2 }, // x(6/2) = 300%
  { 7, 2 }, // x(7/2) = 350%
  { 8, 2 }, // x(8/2) = 400%
};

u16 BTL_CALC_StatusRank( u16 defaultVal, u8 rank )
{
  GF_ASSERT(rank < NELEMS(StatusRankTable));

  {
    defaultVal = defaultVal * StatusRankTable[rank].num;
    defaultVal /= StatusRankTable[rank].denom;
    return defaultVal;
  }
}


//--------------------------------------------------------------
/**
 *  �I�����␳�e�[�u��
 */
//--------------------------------------------------------------
static const struct {
  u8  num;
  u8  denom;
}HitRankTable[] = {
  {   6,  18 },
  {   6,  16 },
  {   6,  14 },
  {   6,  12 },
  {   6,  10 },
  {   6,   8 },
  {   6,   6 },
  {   8,   6 },
  {  10,   6 },
  {  12,   6 },
  {  14,   6 },
  {  16,   6 },
  {  18,   6 },
};

u8 BTL_CALC_HitPer( u8 defPer, u8 rank )
{
  GF_ASSERT(rank < NELEMS(HitRankTable));

  {
    u32 per = defPer * HitRankTable[rank].num / HitRankTable[rank].denom;
    if( per > 100 )
    {
      per = 100;
    }
    return per;
  }
}

//--------------------------------------------------------------
/**
 *  �N���e�B�J�����e�[�u��
 */
//--------------------------------------------------------------
BOOL BTL_CALC_CheckCritical( u8 rank )
{
  static const u8 CriticalRankTable[] = {
    16, 8, 4, 3, 2,
  };

  GF_ASSERT(rank < NELEMS(CriticalRankTable));

  {
    u8 rp = CriticalRankTable[ rank ];
    u8 ret = BTL_CALC_GetRand( rp );

    return (ret == 0);
  }
}

//--------------------------------------------------------------
/**
 *  �^�C�v�����l
 */
//--------------------------------------------------------------
typedef enum {
    x0 = 0,
    xH = 2,
    x1 = 4,
    x2 = 8,
}TypeAffValue;

//--------------------------------------------------------------
/**
 *  �^�C�v�����e�[�u��
 */
//--------------------------------------------------------------
static const u8 TypeAffTbl[ POKETYPE_NUMS ][ POKETYPE_NUMS ] = {
//          �m,  �i,  ��,  ��,  �n,  ��,  ��,  �H,  �|,  ��,  ��,  ��,  �d,  ��,  �X,  ��,  ��,
/* �m */ {  x1,  x1,  x1,  x1,  x1,  xH,  x1,  x0,  xH,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1, },
/* �i */ {  x2,  x1,  xH,  xH,  x1,  x2,  xH,  x0,  x2,  x1,  x1,  x1,  x1,  xH,  x2,  x1,  x2, },
/* �� */ {  x1,  x2,  x1,  x1,  x1,  xH,  x2,  x1,  xH,  x1,  x1,  x2,  xH,  x1,  x1,  x1,  x1, },
/* �� */ {  x1,  x1,  x1,  xH,  xH,  xH,  x1,  xH,  x0,  x1,  x1,  x2,  x1,  x1,  x1,  x1,  x1, },
/* �n */ {  x1,  x1,  x0,  x2,  x1,  x2,  xH,  x1,  x2,  x2,  x1,  xH,  x2,  x1,  x1,  x1,  x1, },
/* �� */ {  x1,  xH,  x2,  x1,  xH,  x1,  x2,  x1,  xH,  x2,  x1,  x1,  x1,  x1,  x2,  x1,  x1, },
/* �� */ {  x1,  xH,  xH,  xH,  x1,  x1,  x1,  xH,  xH,  xH,  x1,  x2,  x1,  x2,  x1,  x1,  x2, },
/* �H */ {  x0,  x1,  x1,  x1,  x1,  x1,  x1,  x2,  xH,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  xH, },
/* �| */ {  x1,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  xH,  xH,  xH,  x1,  xH,  x1,  x2,  x1,  x1, },
/* �� */ {  x1,  x1,  x1,  x1,  x1,  xH,  x2,  x1,  x2,  xH,  xH,  x2,  x1,  x1,  x2,  xH,  x1, },
/* �� */ {  x1,  x1,  x1,  x1,  x2,  x2,  x1,  x1,  x1,  x2,  xH,  xH,  x1,  x1,  x1,  xH,  x1, },
/* �� */ {  x1,  x1,  xH,  xH,  x2,  x2,  xH,  x1,  xH,  xH,  x2,  xH,  x1,  x1,  x1,  xH,  x1, },
/* �d */ {  x1,  x1,  x2,  x1,  x0,  x1,  x1,  x1,  x1,  x1,  x2,  xH,  xH,  x1,  x1,  xH,  x1, },
/* �� */ {  x1,  x2,  x1,  x2,  x1,  x1,  x1,  x1,  xH,  x1,  x1,  x1,  x1,  xH,  x1,  x1,  x0, },
/* �X */ {  x1,  x1,  x2,  x1,  x2,  x1,  x1,  x1,  xH,  xH,  xH,  x2,  x1,  x1,  xH,  x2,  x1, },
/* �� */ {  x1,  x1,  x1,  x1,  x1,  x1,  x1,  x1,  xH,  x1,  x1,  x1,  x1,  x1,  x1,  x2,  x1, },
/* �� */ {  x1,  xH,  x1,  x1,  x1,  x1,  x1,  x2,  xH,  x1,  x1,  x1,  x1,  x2,  x1,  x1,  xH, },
};

//--------------------------------------------------------------
/**
 *  �^�C�v�����v�Z
 */
//--------------------------------------------------------------
BtlTypeAff BTL_CALC_TypeAff( PokeType wazaType, PokeType pokeType )
{
  if( (wazaType == POKETYPE_NULL) || (pokeType == POKETYPE_NULL) ){
    return BTL_TYPEAFF_100;
  }
  else
  {
    u8 affinity = TypeAffTbl[ wazaType ][ pokeType ];

    switch( affinity ){
    case x0:    return BTL_TYPEAFF_0;
    case xH:    return BTL_TYPEAFF_50;
    case x1:    return BTL_TYPEAFF_100;
    case x2:    return BTL_TYPEAFF_200;
    }
  }

  GF_ASSERT(0);
  return BTL_TYPEAFF_0;
}
//--------------------------------------------------------------
/**
 *  �^�C�v�����v�Z�i�|�P�����^�C�v���̂܂܎g�p�\�j
 */
//--------------------------------------------------------------
BtlTypeAff BTL_CALC_TypeAffPair( PokeType wazaType, PokeTypePair pokeType )
{
  PokeType type1, type2;

  PokeTypePair_Split( pokeType, &type1, &type2 );

  if( PokeTypePair_IsPure(pokeType) )
  {
    return BTL_CALC_TypeAff( wazaType, type1 );
  }
  else
  {
    BtlTypeAff  aff1, aff2;
    aff1 = BTL_CALC_TypeAff( wazaType, type1 );
    aff2 = BTL_CALC_TypeAff( wazaType, type2 );
    return BTL_CALC_TypeAffMul( aff1, aff2 );
  }
}
//=============================================================================================
/**
 * �����l�̊|�����킹�i�Q�^�C�v����|�P�����Ή��j
 *
 * @param   aff1
 * @param   aff2
 *
 * @retval  BtlTypeAff    �|�����킹��̑����l
 */
//=============================================================================================
BtlTypeAff BTL_CALC_TypeAffMul( BtlTypeAff aff1, BtlTypeAff aff2 )
{
  enum {
    VALUE_0   = 0,
    VALUE_25  = 1,
    VALUE_50  = 2,
    VALUE_100 = 4,
    VALUE_200 = 8,
    VALUE_400 = 16,
  };

  static const u8 valueTable[ BTL_TYPEAFF_MAX ] = {
    VALUE_0,  VALUE_25, VALUE_50, VALUE_100, VALUE_200, VALUE_400
  };
  u32 mulValue;

  GF_ASSERT(aff1<BTL_TYPEAFF_MAX);
  GF_ASSERT(aff2<BTL_TYPEAFF_MAX);

  mulValue = (valueTable[ aff1 ] * valueTable[ aff2 ]) / VALUE_100;

  switch( mulValue ){
  case VALUE_0:    return BTL_TYPEAFF_0;
  case VALUE_25:   return BTL_TYPEAFF_25;
  case VALUE_50:   return BTL_TYPEAFF_50;
  case VALUE_100:  return BTL_TYPEAFF_100;
  case VALUE_200:  return BTL_TYPEAFF_200;
  case VALUE_400:  return BTL_TYPEAFF_400;
  default:
    GF_ASSERT(0);
    return BTL_TYPEAFF_0;
  }
}

//--------------------------------------------------------------
/**
 *  �����_���[�W�v�Z
 */
//--------------------------------------------------------------
u32 BTL_CALC_AffDamage( u32 rawDamage, BtlTypeAff aff )
{
  switch( aff ){
  case BTL_TYPEAFF_0:   return 0;
  case BTL_TYPEAFF_25:  return (rawDamage / 4);
  case BTL_TYPEAFF_50:  return (rawDamage / 2);
  case BTL_TYPEAFF_100: return rawDamage;
  case BTL_TYPEAFF_200: return rawDamage * 2;
  case BTL_TYPEAFF_400: return rawDamage * 4;
  default:
    GF_ASSERT(0);
    return rawDamage;
  }
}
//=============================================================================================
/**
 *  ��R������S�Ď擾
 *
 * @param   type    �^�C�v
 * @param   dst     [out] ��R���������^�C�v���i�[����z��
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_CALC_GetResistTypes( PokeType type, PokeType* dst )
{
  u8  cnt, i;

  for(i=0, cnt=0; i<POKETYPE_NUMS; ++i)
  {
    if( (TypeAffTbl[type][i] == x0) || (TypeAffTbl[type][i] == xH) ){
      dst[cnt++] = i;
    }
  }

  return cnt;
}
//=============================================================================================
/**
 * �^�C�v�����i�����j->�^�C�v�����i�����܂��j�ɕύX
 *
 * @param   aff
 *
 * @retval  BtlTypeAffAbout
 */
//=============================================================================================
BtlTypeAffAbout BTL_CALC_TypeAffAbout( BtlTypeAff aff )
{
  if( aff > BTL_TYPEAFF_100 )
  {
    return BTL_TYPEAFF_ABOUT_ADVANTAGE;
  }
  if( aff == BTL_TYPEAFF_100 )
  {
    return BTL_TYPEAFF_ABOUT_NORMAL;
  }
  if( aff != BTL_TYPEAFF_0 )
  {
    return BTL_TYPEAFF_ABOUT_DISADVANTAGE;
  }
  return BTL_TYPEAFF_ABOUT_NONE;
}
//=============================================================================================
/**
 * �m�����ۃ`�F�b�N
 *
 * @param   per
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CALC_IsOccurPer( u32 per )
{
  return (BTL_CALC_GetRand(100) < per);
}
//=============================================================================================
/**
 * ���l�؂�グ
 *
 * @param   value
 * @param   min
 *
 * @retval  int
 */
//=============================================================================================
int BTL_CALC_Roundup( int value, int min )
{
  if( value < min ){ value = min; }
  return value;
}
//=============================================================================================
/**
 *
 *
 * @param   value
 * @param   ratio   �{���i���j
 *
 * @retval  extern u32
 */
//=============================================================================================
u32 BTL_CALC_MulRatio( u32 value, fx32 ratio )
{
  u32 decimal;

  value *= ratio;
  decimal = value & ( (1 << FX32_SHIFT) -1 );
  value >>= FX32_SHIFT;
  if( decimal > (1 << (FX32_SHIFT-1)) ){
    ++value;
  }

  return value;
}
//=============================================================================================
/**
 *
 *
 * @param   value
 * @param   ratio   �{���i���j
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CALC_MulRatio_OverZero( u32 value, fx32 ratio )
{
  value = BTL_CALC_MulRatio( value, ratio );
  if( value == 0 ){
    value = 1;
  }
  return value;
}
//=============================================================================================
/**
 * �|�P�����̍ő�HP * 1/n ���v�Z�i�Œ�P�ɂȂ�悤�ɕ␳�j
 *
 * @param   bpp
 * @param   denom
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CALC_QuotMaxHP( const BTL_POKEPARAM* bpp, u32 denom )
{
  u32 ret = BPP_GetValue( bpp, BPP_MAX_HP ) / denom;
  if( ret == 0 ){ ret = 1; }
  return ret;
}
//=============================================================================================
/**
 *  min�ȏ� �` max�ȉ��͈͓̔��ŗ����擾
 *
 * @param   min
 * @param   max
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CALC_RandRange( u32 min, u32 max )
{
  if( min > max ){
    u32 tmp = min;
    min = max;
    max = tmp;
  }
  {
    u32 range = 1 + (max-min);
    return min + BTL_CALC_GetRand( range );
  }
}




//=============================================================================================
/**
 * �ő�q�b�g�񐔂��󂯎��A���ۂɃq�b�g����񐔁i�����_���␳�j��Ԃ�
 *
 * @param   numHitMax
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_CALC_HitCountStd( u8 numHitMax )
{
  if( numHitMax == 5 )
  {
    static const u8 perTbl[] = {
      0, 0, 35, 70, 85, 100,
    };
    u8 rnd = BTL_CALC_GetRand( 100 );
    u8 i;

    for(i=0; i<NELEMS(perTbl); ++i)
    {
      if( rnd < perTbl[i] ){
        return i;
      }
    }
    return numHitMax;
  }
  else{
    return numHitMax;
  }
}

//=============================================================================================
/**
 * �Y���̓V��ɂ��^�[���`�F�b�N�_���[�W���v�Z
 *
 * @param   bpp
 * @param   weather
 *
 * @retval  u16     �_���[�W�l
 */
//=============================================================================================
u16 BTL_CALC_RecvWeatherDamage( const BTL_POKEPARAM* bpp, BtlWeather weather )
{
  switch( weather ){
  case BTL_WEATHER_SAND:
    if( BPP_IsMatchType(bpp, POKETYPE_IWA) ){ return 0; }
    if( BPP_IsMatchType(bpp, POKETYPE_HAGANE) ){ return 0; }
    if( BPP_IsMatchType(bpp, POKETYPE_JIMEN) ){ return 0; }
    break;

  case BTL_WEATHER_SNOW:
    if( BPP_IsMatchType(bpp, POKETYPE_KOORI) ){ return 0; }
    break;

  default:
    return 0;
  }

  {
    u16 dmg = BPP_GetValue( bpp, BPP_MAX_HP ) / 16;
    if( dmg == 0 ){ dmg = 1; }
    return dmg;
  }
}
//=============================================================================================
/**
 * �V��E���U�^�C�v�����ɂ��З͔{���␳
 *
 * @param   weather
 * @param   wazaType
 *
 * @retval  fx32
 */
//=============================================================================================
fx32 BTL_CALC_GetWeatherDmgRatio( BtlWeather weather, PokeType wazaType )
{
  switch( weather ){
  case BTL_WEATHER_SHINE:
    if( wazaType == POKETYPE_HONOO ){
      return BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE;
    }
    if( wazaType == POKETYPE_MIZU ){
      return BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE;
     }
    break;

  case BTL_WEATHER_RAIN:
    if( wazaType == POKETYPE_HONOO ){
      return BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE;
    }
    if( wazaType == POKETYPE_MIZU ){
      return BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE;
    }
    break;

  default:
    break;
  }
  return BTL_CALC_DMG_WEATHER_RATIO_NONE;
}


//=============================================================================================
/**
 * ���U�f�[�^�̏�Ԉُ�p���p�����[�^�l����A�o�g���Ŏg����Ԉُ�p���p�����[�^�l�֕ϊ�
 *
 * @param   wazaSickCont
 * @param   attacker    ���U���g�����|�P����
 * @param   sickCont    [out]
 *
 */
//=============================================================================================
void BTL_CALC_WazaSickContToBppSickCont( WAZA_SICKCONT_PARAM wazaSickCont, const BTL_POKEPARAM* attacker, BPP_SICK_CONT* sickCont )
{
  switch( wazaSickCont.type ){
  case WAZASICK_CONT_POKE:
    GF_ASSERT(attacker!=NULL);
    {
      *sickCont = BPP_SICKCONT_MakePoke( BPP_GetID(attacker) );
    }
    break;

  case WAZASICK_CONT_TURN:
    {
      u8 turn = BTL_CALC_RandRange( wazaSickCont.turnMin, wazaSickCont.turnMax );
      *sickCont = BPP_SICKCONT_MakeTurn( turn );
    }
    break;

  case WAZASICK_CONT_PERMANENT:
    *sickCont = BPP_SICKCONT_MakePermanentIncParam( wazaSickCont.turnMax, wazaSickCont.turnMin );
    break;

  case WAZASICK_CONT_POKETURN:
    GF_ASSERT(attacker!=NULL);
    {
      u8 pokeID = BPP_GetID( attacker );
      u8 turn = BTL_CALC_RandRange( wazaSickCont.turnMin, wazaSickCont.turnMax );
      *sickCont = BPP_SICKCONT_MakePokeTurn( pokeID, turn );
    }
    break;
  }
}
//=============================================================================================
/**
 * �|�P�����n��Ԉُ�́A�f�t�H���g�̌p���p�����[�^���쐬
 *
 * @param   sick
 * @param   cont    [out]
 *
 */
//=============================================================================================
BPP_SICK_CONT BTL_CALC_MakeDefaultPokeSickCont( PokeSick sick )
{
  BPP_SICK_CONT  cont;

  cont.raw = 0;
  switch( sick ){
  case POKESICK_DOKU:
  case POKESICK_YAKEDO:
  case POKESICK_MAHI:
  case POKESICK_KOORI:
    cont.type = WAZASICK_CONT_PERMANENT;
    break;
  case POKESICK_NEMURI:
    cont.type = WAZASICK_CONT_TURN;
    cont.turn.count = BTL_CALC_RandRange( BTL_NEMURI_TURN_MIN, BTL_NEMURI_TURN_MAX );
    break;
  default:
    GF_ASSERT_MSG(0, "illegal sick ID(%d)\n", sick);
    cont.type = WAZASICK_CONT_NONE;
    break;
  }

  return cont;
}

//=============================================================================================
/**
 * ��{��Ԉُ킩
 *
 * @param   sickID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_CALC_IsBasicSickID( WazaSick sickID )
{
  return sickID < POKESICK_MAX;
}

//=============================================================================================
/**
 * ���U�n��Ԉُ�̌p���p�����[�^�i�^�[�����^�j���쐬
 *
 * @param   turn_count
 */
//=============================================================================================
BPP_SICK_CONT BTL_CALC_MakeWazaSickCont_Turn( u8 turn_count )
{
  BPP_SICK_CONT cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_TURN;
  cont.turn.count = turn_count;
  return cont;
}
//=============================================================================================
/**
 * ���U�n��Ԉُ�̌p���p�����[�^�i�|�P�����ˑ��^�j���쐬
 *
 * @param   pokeID
 */
//=============================================================================================
BPP_SICK_CONT BTL_CALC_MakeWazaSickCont_Poke( u8 pokeID )
{
  BPP_SICK_CONT cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_POKE;
  cont.poke.ID = pokeID;
  return cont;
}


//=============================================================================================
/**
 * ���U�n��Ԉُ�́A�f�t�H���g�̌p���p�����[�^���쐬
 *
 * @param   sick
 * @param   cont    [out]
 *
 */
//=============================================================================================
void BTL_CALC_MakeDefaultWazaSickCont( WazaSick sick, const BTL_POKEPARAM* attacker, BPP_SICK_CONT* cont )
{
  if( sick < POKESICK_MAX ){
    *cont = BTL_CALC_MakeDefaultPokeSickCont( sick );
    return;
  }

  cont->raw = 0;

  switch( sick ){
  case WAZASICK_MEROMERO:
    {
      u8 pokeID = BPP_GetID( attacker );
      *cont = BPP_SICKCONT_MakePoke( pokeID );
    }
    break;

  case WAZASICK_KONRAN:
    {
      u8 turns = BTL_CALC_RandRange( BTL_CONF_TURN_MIN, BTL_CONF_TURN_MAX );
      *cont = BPP_SICKCONT_MakeTurn( turns );
    }
    break;

  default:
    *cont = BPP_SICKCONT_MakePermanent();
    break;
  }
}

//=============================================================================================
/**
 * �u�X�L���X���b�v�v�ɂ���Č����ł��Ȃ��Ƃ���������
 *
 * @param   tok
 *
 * @retval  BOOL    �R�s�[�ł��Ȃ��ꍇ��TRUE
 */
//=============================================================================================
BOOL BTL_CALC_TOK_CheckCantChange( PokeTokusei tok )
{
  static const u16 prohibits[] = {
    POKETOKUSEI_FUSIGINAMAMORI, POKETOKUSEI_MARUTITAIPU,
  };
  u16 i;
  for(i=0; i<NELEMS(prohibits); ++i)
  {
    if( prohibits[i] == tok )
    {
      return TRUE;
    }
  }
  return FALSE;
}


//=============================================================================================
// �T�C�h�G�t�F�N�g�֘A
//=============================================================================================
static const struct {
  u16  waza;
  u16  side;
}WazaSideDispatchTbl[] = {
  { WAZANO_RIHUREKUTAA,    BTL_SIDEEFF_REFRECTOR },      ///< �����U���𔼌�
  { WAZANO_HIKARINOKABE,   BTL_SIDEEFF_HIKARINOKABE },   ///< ����U���𔼌�
  { WAZANO_SINPINOMAMORI,  BTL_SIDEEFF_SINPINOMAMORI },  ///< �|�P�n��Ԉُ�ɂȂ�Ȃ�
  { WAZANO_SIROIKIRI,      BTL_SIDEEFF_SIROIKIRI },      ///< �����N�_�E�����ʂ��󂯂Ȃ�
  { WAZANO_OIKAZE,         BTL_SIDEEFF_OIKAZE },         ///< ���΂₳�Q�{
  { WAZANO_OMAZINAI,       BTL_SIDEEFF_OMAJINAI },       ///< �U�����}���ɓ�����Ȃ�
  { WAZANO_MAKIBISI,       BTL_SIDEEFF_MAKIBISI },       ///< ����ւ��ďo�Ă����|�P�����Ƀ_���[�W�i�R�i�K�j
  { WAZANO_DOKUBISI,       BTL_SIDEEFF_DOKUBISI },       ///< ����ւ��ďo�Ă����|�P�����ɓŁi�Q�i�K�j
  { WAZANO_SUTERUSUROKKU,  BTL_SIDEEFF_STEALTHROCK },    ///< ����ւ��ďo�Ă����|�P�����Ƀ_���[�W�i�����v�Z����j
//  { WAZANO_WAIDOGAADO,     BTL_SIDEEFF_WIDEGUARD },      ///< �u�G�S�́v�u�����ȊO�S���v�̍U�����U��h��
};


BtlSideEffect  BTL_CALC_WazaIDtoSideEffectID( WazaID waza )
{
  u32 i;
  for(i=0; i<NELEMS(WazaSideDispatchTbl); ++i){
    if( WazaSideDispatchTbl[i].waza == waza ){
      return WazaSideDispatchTbl[i].side;
    }
  }
  return BTL_SIDEEFF_NULL;
}
WazaID  BTL_CALC_SideEffectIDtoWazaID( BtlSideEffect sideEffect )
{
  u32 i;
  for(i=0; i<NELEMS(WazaSideDispatchTbl); ++i){
    if( WazaSideDispatchTbl[i].side == sideEffect ){
      return WazaSideDispatchTbl[i].waza;
    }
  }
  return WAZANO_NULL;
}
//=============================================================================================
// �����_���Z�I��
//=============================================================================================


//=============================================================================================
/**
 * ����̃��U�����O���������烉���_���Ń��U������i��т��ӂ��p�H�j
 *
 * @param   excludeWazaTbl    ���O���郏�UID�e�[�u��
 * @param   tblElems          ���O���郏�UID�e�[�u���̗v�f��
 *
 * @retval  WazaID    ���肵�����UID
 */
//=============================================================================================
WazaID BTL_CALC_RandWaza( const u16* wazaTbl, u16 tblElems )
{
  u16 waza, cnt, i;

  for(cnt=0, waza=1; waza<WAZANO_MAX; ++waza)
  {
    if( !is_include(wazaTbl, tblElems, waza) ){
      gWazaStoreWork[ cnt++ ] = waza;
    }
  }

  i = BTL_CALC_GetRand( cnt );
  return gWazaStoreWork[ i ];
}
static BOOL is_include( const u16* tbl, u32 tblElems, u16 wazaID )
{
  u32 i;
  for(i=0; i<tblElems; ++i){
    if( tbl[i] == wazaID ){
      return TRUE;
    }
  }
  return FALSE;
}


//=============================================================================================
/**
 * �|�P������|�������̃x�[�X�o���l���v�Z
 *
 * @param   bpp   �|���ꂽ�|�P����
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CALC_CalcBaseExp( const BTL_POKEPARAM* bpp )
{
  u32 monsno = BPP_GetMonsNo( bpp );
  u16 formNo = BPP_GetValue( bpp, BPP_FORM );
  u16 level  = BPP_GetValue( bpp, BPP_LEVEL );

  u32 exp = POKETOOL_GetPersonalParam( monsno, formNo, POKEPER_ID_give_exp );
  exp = exp * level / 6;

  return exp;
}

//=============================================================================================
// ���[�����Ƃɕς��v�f
//=============================================================================================
//=============================================================================================
/**
 * ���U�̑Ώۂ�I������K�v�����邩�H
 */
//=============================================================================================
BOOL BTL_RULE_IsNeedSelectTarget( BtlRule rule )
{
#ifdef ROTATION_NEW_SYSTEM
  return ((rule != BTL_RULE_SINGLE) && (rule != BTL_RULE_ROTATION));
#else
  return (rule != BTL_RULE_SINGLE);
#endif
}
//=============================================================================================
/**
 * ���݂��̐w�c�ɑ��݂���ʒu�̐�
 */
//=============================================================================================
u8 BTL_RULE_GetNumFrontPos( BtlRule rule )
{
  switch( rule ){
  case BTL_RULE_SINGLE:
    return 1;
  case BTL_RULE_DOUBLE:
    return 2;
  case BTL_RULE_TRIPLE:
    return 3;

  case BTL_RULE_ROTATION:
  #ifdef ROTATION_NEW_SYSTEM
    return 1;
  #else
    return 2;
  #endif
  }
  GF_ASSERT(0);
  return 1;
}
//=============================================================================================
/**
 * �莝���|�P�����̊J�n�C���f�b�N�X
 *
 * @param   rule
 * @param   numCoverPos   �N���C�A���g�̒S���ʒu���i�_�u�����A�ʏ�͂Q�C�}���`�Ȃ�P�ɂȂ�j
 */
//=============================================================================================
u8 BTL_RULE_HandPokeIndex( BtlRule rule, u8 numCoverPos )
{
  if( rule == BTL_RULE_ROTATION )
  {
    // ���[�e�[�V�����̏ꍇ�A�R�̖ڂ܂Ő퓬�ɏo�Ă���Ƃ݂Ȃ�
    return BTL_ROTATE_NUM;
  }
  return numCoverPos;
}

//=============================================================================================
// �g���[�i�[�^�C�v���ʏ���
//=============================================================================================
/**
 *  �g���[�i�[�^�C�v���W�����[�_�[�ɊY�����邩����
 */
BOOL BTL_CALC_IsTrtypeGymLeader( u16 trType )
{
  static const u16 typeTbl[] = {
    TRTYPE_LEADER1A,
    TRTYPE_LEADER1B,
    TRTYPE_LEADER1C,
    TRTYPE_LEADER2,
    TRTYPE_LEADER3,
    TRTYPE_LEADER4,
    TRTYPE_LEADER5,
    TRTYPE_LEADER6,
    TRTYPE_LEADER7,
    TRTYPE_LEADER8A,
    TRTYPE_LEADER8B,
  };
  u32 i;
  for(i=0; i<NELEMS(typeTbl); ++i){
    if( typeTbl[i] == trType ){
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  �g���[�i�[�^�C�v���l�V���ɊY�����邩����
 */
BOOL BTL_CALC_IsTrtypeBig4( u16 trType )
{
  static const u16 typeTbl[] = {
    TRTYPE_BIGFOUR1,
    TRTYPE_BIGFOUR2,
    TRTYPE_BIGFOUR3,
    TRTYPE_BIGFOUR4,
  };
  u32 i;
  for(i=0; i<NELEMS(typeTbl); ++i){
    if( typeTbl[i] == trType ){
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  �g���[�i�[�^�C�v���{�X��i�Ȃ��x�㏸�j�ɊY�����邩����
 */
BOOL BTL_CALC_IsTrTypeBoss( u16 trType )
{
  if( BTL_CALC_IsTrtypeGymLeader(trType) ){
    return TRUE;
  }
  if( BTL_CALC_IsTrtypeBig4(trType) ){
    return TRUE;
  }
  if( trType == TRTYPE_CHAMPION ){
    return TRUE;
  }
  return FALSE;
}

//=============================================================================================
// �܋��v�Z
//=============================================================================================

//=============================================================================================
/**
 * �������Ƃ��ɂ��炦�邨���Â������z�̌v�Z
 *
 * @param   sp
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CALC_WinMoney( const BATTLE_SETUP_PARAM* sp )
{
  u32 money = 0;
  if( sp->competitor == BTL_COMPETITOR_TRAINER )
  {
    u8 clientID = BTL_CLIENT_ENEMY1;
    if( (sp->tr_data[clientID] != NULL) && (sp->party[clientID] != NULL) ){
      money += calcWinMoney_Sub( sp->tr_data[clientID], sp->party[clientID] );
    }

    clientID = BTL_CLIENT_ENEMY2;
    if( (sp->tr_data[clientID] != NULL) && (sp->party[clientID] != NULL) ){
      money += calcWinMoney_Sub( sp->tr_data[clientID], sp->party[clientID] );
    }
  }
  return money;
}

static u32 calcWinMoney_Sub( const BSP_TRAINER_DATA* trData, const POKEPARTY * party )
{
  if( party )
  {
    u8 poke_cnt = PokeParty_GetPokeCount( party );
    if( poke_cnt )
    {
      const POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, poke_cnt-1 );
      u32 tr_money_ratio = TT_TrainerDataParaGet( trData->tr_id, ID_TD_gold );

      return (PP_Get(pp, ID_PARA_level, NULL) * tr_money_ratio * 4);
    }
  }
  return 0;
}



//=============================================================================================
/**
 * ���������Ɏx�������z�v�Z
 *
 * @param   badgeCount    �v���C���[�o�b�W��
 * @param   party         �p�[�e�B�f�[�^
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CALC_LoseMoney( u8 badgeCount, const BTL_PARTY* party )
{
  static const u8 ratio[] = {
    2, 4, 6, 9, 12, 16, 20, 25, 30,
  };
  u32 maxLevel, pokeCnt, i;

  pokeCnt = BTL_PARTY_GetMemberCount( party );
  maxLevel = 1;
  for(i=0; i<pokeCnt; ++i)
  {
    const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( party, i );
    u32 level = BPP_GetValue( bpp, BPP_LEVEL );
    if( level > maxLevel ){
      maxLevel = level;
    }
  }

  // �L�蓾�Ȃ����ǈꉞ
  if( badgeCount >= NELEMS(ratio) ){
    badgeCount = NELEMS(ratio) - 1;
  }

  return maxLevel * 4 * ratio[ badgeCount ];
}


//=============================================================================================
// �A�C�e���f�[�^�擾�֘A
//=============================================================================================
static HEAPID Heap_ItemParam;

//=============================================================================================
/**
 *
 *
 * @param   heapID
 */
//=============================================================================================
void BTL_CALC_ITEM_InitSystem( HEAPID heapID )
{
  Heap_ItemParam = heapID;
}
void BTL_CALC_ITEM_QuitSystem( void )
{
  //
}
//=============================================================================================
/**
 * �A�C�e���p�����[�^�擾
 *
 * @param   item
 * @param   paramID
 *
 * @retval  s32
 */
//=============================================================================================
s32 BTL_CALC_ITEM_GetParam( u16 item, u16 paramID )
{
  return ITEM_GetParam( item, paramID, Heap_ItemParam );
}



//=============================================================================================
/**
 * �u�̂낢�v�̑I��Ώۃ^�C�v��Ԃ�
 *
 * @param   attacker
 *
 * @retval  WazaTarget
 */
//=============================================================================================
WazaTarget BTL_CALC_GetNoroiTargetType( const BTL_POKEPARAM* attacker )
{
  return BPP_IsMatchType(attacker, POKETYPE_GHOST)? WAZA_TARGET_OTHER_SELECT : WAZA_TARGET_USER;
}

//=============================================================================================
/**
 * ���U�^�[�Q�b�g�������_���Ŏ�������
 *
 * @param   mainModule
 * @param   bpp
 * @param   waza
 *
 * @retval  BtlTargetPos
 */
//=============================================================================================
BtlPokePos BTL_CALC_DecideWazaTargetAuto( const BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon, const BTL_POKEPARAM* bpp, WazaID waza )
{
  BtlRule rule = BTL_MAIN_GetRule( mainModule );
  BtlPokePos targetPos = BTL_POS_NULL;
  BtlPokePos myPos = BTL_MAIN_PokeIDtoPokePos( mainModule, pokeCon, BPP_GetID(bpp) );
  WazaTarget  targetType = WAZADATA_GetParam( waza, WAZAPARAM_TARGET );

  if( waza == WAZANO_NOROI ){
    targetType = BTL_CALC_GetNoroiTargetType( bpp );
  }

  // �V���O��
  if( rule == BTL_RULE_SINGLE )
  {
    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:  ///< �����ȊO�̂P�́i�I���j
    case WAZA_TARGET_ENEMY_SELECT:  ///< �G�P�́i�I���j
    case WAZA_TARGET_ENEMY_RANDOM:  ///< �G�����_��
    case WAZA_TARGET_ENEMY_ALL:     ///< �G���Q��
    case WAZA_TARGET_OTHER_ALL:     ///< �����ȊO�S��
      targetPos = BTL_MAIN_GetOpponentPokePos( mainModule, myPos, 0 );
      return targetPos;

    case WAZA_TARGET_USER:                ///< �����P�̂̂�
    case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P��
      targetPos = myPos;
      return targetPos;

    case WAZA_TARGET_ALL:
    case WAZA_TARGET_UNKNOWN:
    default:
      break;
    }
    return BTL_POS_NULL;
  }
  // �_�u���A�g���v���A���[�e����
  else
  {
    BtlExPos exPos;
    u8 pokeID[ BTL_POSIDX_MAX ];
    u8 pokeCnt = 0;

    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:       ///< �ʏ�|�P�i�P�̑I���j
    case WAZA_TARGET_ENEMY_SELECT:       ///< ���葤�|�P�i�P�̑I���j
    case WAZA_TARGET_ENEMY_RANDOM:       ///< ����|�P�P�̃����_��
      exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );
      break;

    case WAZA_TARGET_FRIEND_USER_SELECT: ///< �������܂ޖ����|�P�i�P�̑I���j
      exPos = EXPOS_MAKE( BTL_EXPOS_AREA_MYTEAM, myPos );
      break;

    case WAZA_TARGET_FRIEND_SELECT:      ///< �����ȊO�̖����|�P�i�P�̑I���j
      exPos = EXPOS_MAKE( BTL_EXPOS_AREA_FRIENDS, myPos );
      break;

    case WAZA_TARGET_USER:               ///< �����̂�
      return myPos;

    default:
      return BTL_POS_NULL;
    }

    pokeCnt = BTL_MAIN_ExpandExistPokeID( mainModule, pokeCon, exPos, pokeID );
    if( pokeCnt )
    {
      u8 rnd = BTL_CALC_GetRand( pokeCnt );
      return BTL_MAIN_PokeIDtoPokePos( mainModule, pokeCon, pokeID[rnd] );
    }

    return BTL_POS_NULL;
  }
}


//=============================================================================================
/**
 *
 *
 * @param   info
 */
//=============================================================================================
void BTL_ESCAPEINFO_Clear( BTL_ESCAPEINFO* info )
{
  u32 i;
  for(i=0; i<NELEMS(info->clientID); ++i){
    info->clientID[i] = BTL_CLIENTID_NULL;
  }
  info->count = 0;
}
void BTL_ESCAPEINFO_Add( BTL_ESCAPEINFO* info, u8 clientID )
{
  u32 i;
  for(i=0; i<NELEMS(info->clientID); ++i)
  {
    if( info->clientID[i] == clientID ){ break; }
    if( info->clientID[i] == BTL_CLIENTID_NULL )
    {
      info->clientID[i] = clientID;
      info->count++;
      break;
    }
  }
}
u32 BTL_ESCAPEINFO_GetCount( const BTL_ESCAPEINFO* info )
{
  return info->count;
}

BtlResult BTL_ESCAPEINFO_CheckWinner( const BTL_ESCAPEINFO* info, u8 myClientID )
{
  BOOL fMyTeamEscaped = FALSE;
  BOOL fOtherTeamEscaped = FALSE;
  u32 i;

  TAYA_Printf("�������N���C�A���g��=%d\n", info->count);

  for(i=0; i<info->count; ++i)
  {
    if( BTL_MAINUTIL_IsFriendClientID( info->clientID[i], myClientID ) ){
      TAYA_Printf("ClientID=%d �������� ... �����̃`�[��\n", info->clientID[i]);
      fMyTeamEscaped = TRUE;
    }else{
      TAYA_Printf("ClientID=%d �������� ... ����̃`�[��\n", info->clientID[i]);
      fOtherTeamEscaped = TRUE;
    }
  }

  if( fMyTeamEscaped && fOtherTeamEscaped ){
    TAYA_Printf("���`�[���������������������\n");
    return BTL_RESULT_DRAW;
  }
  if( fMyTeamEscaped ){
    TAYA_Printf("�����`�[�������������畉��\n");
    return BTL_RESULT_RUN;
  }
  TAYA_Printf("����`�[�������������珟��\n");
  return BTL_RESULT_RUN_ENEMY;
}

