
#ifndef __BTL_CALC_H__
#define __BTL_CALC_H__

#include "item\item.h"
#include "waza_tool\wazano_def.h"

#include "battle\battle.h"
#include "btl_util.h"
#include "btl_pokeparam.h"
#include "btl_sideeff.h"
#include "btl_const.h"


/**
 *  �v�f��->bit�t���O�t�B�[���h�ɕK�v�ȃo�b�t�@�T�C�Y�i�o�C�g�j�v�Z
 */
#define BTL_CALC_BITFLAG_BUFSIZE(max)   (((max)/8)+(((max)%8)!=0))


static inline u32 BTL_CALC_MulRatio( u32 value, fx32 ratio )
{
  return (value * ratio) >> FX32_SHIFT;
}

static inline u32 BTL_CALC_MulRatio_OverZero( u32 value, fx32 ratio )
{
  value = (value * ratio) >> FX32_SHIFT;
  if( value == 0 ){
    value = 1;
  }
  return value;
}

static inline u32 BTL_CALC_IsOccurPer( u32 per )
{
  return (GFL_STD_MtRand(100) < per);
}

static inline int BTL_CALC_Roundup( int value, int min )
{
  if( value < min ){ value = min; }
  return value;
}

static inline u32 BTL_CALC_QuotMaxHP( const BTL_POKEPARAM* bpp, u32 denom )
{
  u32 ret = BPP_GetValue( bpp, BPP_MAX_HP ) / denom;
  if( ret == 0 ){ ret = 1; }
  return ret;
}

static inline u32 BTL_CALC_RandRange( u32 min, u32 max )
{
  if( min > max ){
    u32 tmp = min;
    min = max;
    max = tmp;
  }
  {
    u32 range = 1 + (max-min);
    return min + GFL_STD_MtRand( range );
  }
}
static inline void BTL_CALC_BITFLG_Construction( u8* flags, u8 bufsize )
{
  u32 i;
  flags[0] = bufsize;
  for(i=1; i<bufsize; ++i){
    flags[i] = 0;
  }
}
static inline void BTL_CALC_BITFLG_Set( u8* flags, u32 index )
{
  u8 byte = 1 + index / 8;
  u8 bit = index & 8;
  if( byte < flags[0] ){
    flags[ byte ] |= (1 << bit);
  }
}
static inline BOOL BTL_CALC_BITFLG_Check( const u8* flags, u32 index )
{
  u8 byte = 1 + index / 8;
  u8 bit = index & 8;
  if( byte < flags[0] ){
    return (flags[ byte ] & (1 << bit)) != 0;
  }
  return 0;
}
static inline void BTL_CALC_BITFLG_Off( u8* flags, u32 index )
{
  u8 byte = 1 + index / 8;
  u8 bit = index & 8;
  if( byte < flags[0] ){
    flags[ byte ] &= (~((u8)(1 << bit)));
  }
}

static inline u32 BTL_CALC_ABS( int value )
{
  if( value < 0 ){
    value *= -1;
  }
  return value;
}

//--------------------------------------------------------------------
/**
 *  �^�C�v�����i 0, 25, 50, 100, 200, 400% �̂U�i�K�j
 */
//--------------------------------------------------------------------
typedef enum {

  BTL_TYPEAFF_0 = 0,  ///< ����
  BTL_TYPEAFF_25,
  BTL_TYPEAFF_50,
  BTL_TYPEAFF_100,
  BTL_TYPEAFF_200,
  BTL_TYPEAFF_400,

}BtlTypeAff;
//--------------------------------------------------------------------
/**
 *  �ȈՃ^�C�v�����i�������Ȃ��E�ӂ��E�΂���E�����ЂƂ̂S�i�K�j
 */
//--------------------------------------------------------------------
typedef enum {

  BTL_TYPEAFF_ABOUT_NONE = 0,
  BTL_TYPEAFF_ABOUT_NORMAL,
  BTL_TYPEAFF_ABOUT_ADVANTAGE,
  BTL_TYPEAFF_ABOUT_DISADVANTAGE,

}BtlTypeAffAbout;

//--------------------------------------------------------------------
/**
 *  �^�C�v���� -> �ȈՃ^�C�v�����ϊ�
 */
//--------------------------------------------------------------------
static inline BtlTypeAffAbout BTL_CALC_TypeAffAbout( BtlTypeAff aff )
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

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
extern u16 BTL_CALC_StatusRank( u16 defaultVal, u8 rank );
extern u8 BTL_CALC_HitPer( u8 defPer, u8 rank );
extern BOOL BTL_CALC_CheckCritical( u8 rank );

extern BtlTypeAff BTL_CALC_TypeAff( PokeType wazaType, PokeTypePair defenderType );
extern u32 BTL_CALC_AffDamage( u32 rawDamage, BtlTypeAff aff );

extern u8 BTL_CALC_HitCountMax( u8 numHitMax );
extern u16 BTL_CALC_RecvWeatherDamage( const BTL_POKEPARAM* bpp, BtlWeather weather );
extern PokeType  BTL_CALC_RandomResistType( PokeType type );



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
extern void BTL_CALC_WazaSickContToBppSickCont( WAZA_SICKCONT_PARAM wazaSickCont, const BTL_POKEPARAM* attacker, BPP_SICK_CONT* sickCont );

BPP_SICK_CONT BTL_CALC_MakeDefaultPokeSickCont( PokeSick sick );
extern void BTL_CALC_MakeDefaultWazaSickCont( WazaSick sick, const BTL_POKEPARAM* attacker, BPP_SICK_CONT* cont );
extern BPP_SICK_CONT BTL_CALC_MakeWazaSickCont_Turn( u8 turn_count );
extern BPP_SICK_CONT BTL_CALC_MakeWazaSickCont_Poke( u8 pokeID );

extern BtlSideEffect  BTL_CALC_WazaIDtoSideEffectID( WazaID waza );
extern WazaID  BTL_CALC_SideEffectIDtoWazaID( BtlSideEffect sideEffect );


//=============================================================================================
/**
 * �ύX�ł��Ȃ��Ƃ������̃`�F�b�N
 *
 * @param   tok     �Ƃ������w��
 *
 * @retval  BOOL    �w�肳�ꂽ�Ƃ������ɕύX����̂��֎~�Ȃ�TRUE
 */
//=============================================================================================
extern BOOL BTL_CALC_TOK_CheckCantChange( PokeTokusei tok );


extern void BTL_CALC_ITEM_InitSystem( HEAPID heapID );
extern void BTL_CALC_ITEM_QuitSystem(void);
extern s32  BTL_CALC_ITEM_GetParam( u16 item, u16 paramID );

extern WazaID BTL_CALC_RandWaza( const u16* excludeWazaTbl, u16 tblElems );
extern u32 BTL_CALC_CalcBaseExp( const BTL_POKEPARAM* bpp );


//=============================================================================================
//=============================================================================================
extern u8 BTL_RULE_GetNumFrontPos( BtlRule rule );
extern BOOL BTL_RULE_IsNeedSelectTarget( BtlRule rule );
extern u8 BTL_RULE_HandPokeIndex( BtlRule rule, u8 numCoverPos );


#endif

