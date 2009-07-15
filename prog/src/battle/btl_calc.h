
#ifndef __BTL_CALC_H__
#define __BTL_CALC_H__

#include "item\item.h"
#include "waza_tool\wazano_def.h"

#include "battle\battle.h"
#include "btl_util.h"
#include "btl_pokeparam.h"
#include "btl_sideeff.h"

enum {
  BTL_CALC_BASERANK_DEFAULT = 0,

  BTL_CALC_HITRATIO_MID = 6,
  BTL_CALC_HITRATIO_MIN = 0,
  BTL_CALC_HITRATIO_MAX = 12,

  BTL_CALC_CRITICAL_MIN = 0,
  BTL_CALC_CRITICAL_MAX = 4,

// �_���[�W�v�Z�֘A
  BTL_CALC_DMG_TARGET_RATIO_PLURAL = FX32_CONST(0.75f),     ///< �����̂��Ώۃ��U�̃_���[�W�␳��
  BTL_CALC_DMG_TARGET_RATIO_NONE   = FX32_CONST(1),         ///< �Ώۂɂ��_���[�W�␳�Ȃ�
  BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE = FX32_CONST(1.5f),    ///< ���U�^�C�v���V��̑g�ݍ��킹�ŗL���ȏꍇ�̕␳��
  BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE = FX32_CONST(0.5f), ///< ���U�^�C�v���V��̑g�ݍ��킹�ŕs���ȏꍇ�̕␳��
  BTL_CALC_DMG_WEATHER_RATIO_NONE = FX32_CONST(1),            ///< ���U�^�C�v���V��̑g�ݍ��킹�ɂ��␳�Ȃ�


// ��Ԉُ폈���֘A
  BTL_NEMURI_TURN_MIN = 3,  ///< �u�˂ނ�v�ŏ��^�[����
  BTL_NEMURI_TURN_MAX = 8,  ///< �u�˂ނ�v�ő�^�[����
  BTL_NEMURI_TURN_RANGE = (BTL_NEMURI_TURN_MAX - BTL_NEMURI_TURN_MIN + 1),

  BTL_MAHI_EXE_PER = 25,        ///< �u�܂Ёv�ł��т�ē����Ȃ��m��
  BTL_MAHI_AGILITY_RATIO = 25,  ///< �u�܂Ёv���̂��΂₳������
  BTL_KORI_MELT_PER = 20,       ///< �u������v���n����m��
  BTL_MEROMERO_EXE_PER = 50,    ///< �u���������v�œ����Ȃ��m��

  BTL_DOKU_SPLIT_DENOM = 8,         ///< �u�ǂ��v�ōő�HP�̉����̂P���邩
  BTL_YAKEDO_SPLIT_DENOM = 8,       ///< �u�₯�ǁv�ōő�HP�̉����̂P���邩
  BTL_YAKEDO_DAMAGE_RATIO = 50,     ///< �u�₯�ǁv�ŕ����_���[�W�������闦
  BTL_MOUDOKU_SPLIT_DENOM = 16,     ///< �����ǂ����A�ő�HP�̉����̂P���邩�i��{�l=�ŏ��l�j
  BTL_MOUDOKU_COUNT_MAX = BTL_MOUDOKU_SPLIT_DENOM-1,  ///< �����ǂ����A�_���[�W�{���������܂ŃJ�E���g�A�b�v���邩


  BTL_CONF_EXE_RATIO = 30,    ///< �������̎����m��
  BTL_CONF_TURN_MIN = 2,
  BTL_CONF_TURN_MAX = 5,
  BTL_CONF_TURN_RANGE = (BTL_CONF_TURN_MAX - BTL_CONF_TURN_MIN)+1,

  BTL_CALC_SICK_TURN_PERMANENT = 0xff,  ///< �p���^�[������ݒ肵�Ȃ��i�i������j�ꍇ�̎w��l

  // �V��֘A
  BTL_CALC_WEATHER_MIST_HITRATIO = FX32_CONST(75),    ///< �u����v�̎��̖������␳��
  BTL_WEATHER_TURN_DEFAULT = 5,         ///< ���U�ɂ��V��ω��̌p���^�[�����f�t�H���g�l
  BTL_WEATHER_TURN_PERMANENT = 0xff,    ///< �V��p���^�[��������

  // �Ƃ������֘A
  BTL_CALC_TOK_CHIDORI_HITRATIO = FX32_CONST(0.8f),     /// �u���ǂ肠���v�̌���������
  BTL_CALC_TOK_HARIKIRI_HITRATIO = FX32_CONST(0.8f),    /// �u�͂肫��v�̖������ω���
  BTL_CALC_TOK_FUKUGAN_HITRATIO = FX32_CONST(1.3f),     /// �u�ӂ�����v�̖����ω���
  BTL_CALC_TOK_SUNAGAKURE_HITRATIO = FX32_CONST(0.8f),  /// �u���Ȃ�����v�̖������ω���
  BTL_CALC_TOK_YUKIGAKURE_HITRATIO = FX32_CONST(0.8f),  /// �u�䂫������v�̖����ω���
  BTL_CALC_TOK_HARIKIRI_POWRATIO = FX32_CONST(1.5f),    /// �u�͂肫��v�̍U���͕ω���
  BTL_CALC_TOK_TETUNOKOBUSI_POWRATIO = FX32_CONST(1.2f),/// �u�Ă̂��Ԃ��v�̍U���͕ω���
  BTL_CALC_TOK_SUTEMI_POWRATIO = FX32_CONST(1.2f),      /// �u���Ă݁v�̍U���͕ω���
  BTL_CALC_TOK_PLUS_POWRATIO = FX32_CONST(1.5f),        /// �u�v���X�v�̍U���͕ω���
  BTL_CALC_TOK_MINUS_POWRATIO = FX32_CONST(1.5f),       /// �u�}�C�i�X�v�̍U���͕ω���
  BTL_CALC_TOK_FLOWERGIFT_POWRATIO = FX32_CONST(1.5f),  /// �u�t�����[�M�t�g�v�̍U���͕ω���
  BTL_CALC_TOK_FLOWERGIFT_GUARDRATIO = FX32_CONST(1.5f),/// �u�t�����[�M�t�g�v�̓��h�ω���
  BTL_CALC_TOK_MORAIBI_POWRATIO = FX32_CONST(1.5f),     /// �u���炢�сv�̍U���͕ω���
  BTL_CALC_TOK_TECKNICIAN_POWRATIO = FX32_CONST(1.5f),  /// �u�e�N�j�V�����v�̍U���͕ω���
  BTL_CALC_TOK_HAYAASI_AGIRATIO = FX32_CONST(1.5f),     /// �u�͂₠���v�̂��΂₳�ω���
  BTL_CALC_TOK_SLOWSTART_ENABLE_TURN = 5,               /// �u�X���[�X�^�[�g�v�̓K�p�^�[��
  BTL_CALC_TOK_SLOWSTART_AGIRATIO = FX32_CONST(0.5f),   /// �u�X���[�X�^�[�g�v�̂��΂₳�ω���
  BTL_CALC_TOK_SLOWSTART_ATKRATIO = FX32_CONST(0.5f),   /// �u�X���[�X�^�[�g�v�̂��������ω���
  BTL_CALC_TOK_FUSIGINAUROKO_GDRATIO = FX32_CONST(1.5f),  /// �u�ӂ����Ȃ��낱�v�̂Ƃ��ڂ��ω���

  BTL_CALC_TOK_DOKUNOTOGE_PER   = 30,   ///�u�ǂ��̃g�Q�v�����m��
  BTL_CALC_TOK_HONONOKARADA_PER = 30,   ///�u�ق̂��̂��炾�v�����m��
  BTL_CALC_TOK_SEIDENKI_PER     = 30,   ///�u�����ł񂫁v�����m��
  BTL_CALC_TOK_HOUSI_PER        = 30,   ///�u�ق����v�����m��
  BTL_CALC_TOK_MEROMEROBODY_PER = 30,   ///�u���������{�f�B�v�����m��

  // �f�����␳�֘A
  BTL_CALC_AGILITY_MAX = 10000,

};

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
extern BPP_SICK_CONT BTL_CALC_MakeMoudokuSickCont( void );
extern BPP_SICK_CONT BTL_CALC_MakeWazaSickCont_Turn( u8 turn_count );
extern BPP_SICK_CONT BTL_CALC_MakeWazaSickCont_Poke( u8 pokeID );

extern BtlSideEffect  BTL_CALC_WazaIDtoSideEffectID( WazaID waza );
extern WazaID  BTL_CALC_SideEffectIDtoWazaID( BtlSideEffect sideEffect );


//=============================================================================================
/**
 * �u�g���[�X�v�ɂ���ăR�s�[�ł��Ȃ��Ƃ���������
 *
 * @param   tok
 *
 * @retval  BOOL    �R�s�[�ł��Ȃ��ꍇ��TRUE
 */
//=============================================================================================
extern BOOL BTL_CALC_TOK_CheckCant_Trace( PokeTokusei tok );

//=============================================================================================
/**
 * �u�X�L���X���b�v�v�ɂ���ăR�s�[�ł��Ȃ��Ƃ���������
 *
 * @param   tok
 *
 * @retval  BOOL    �R�s�[�ł��Ȃ��ꍇ��TRUE
 */
//=============================================================================================
extern BOOL BTL_CALC_TOK_CheckCant_Swap( PokeTokusei tok );

extern void BTL_CALC_ITEM_InitSystem( HEAPID heapID );
extern void BTL_CALC_ITEM_QuitSystem(void);
extern s32  BTL_CALC_ITEM_GetParam( u16 item, u16 paramID );

extern WazaID BTL_CALC_RandWaza( const u16* excludeWazaTbl, u16 tblElems );

#endif

