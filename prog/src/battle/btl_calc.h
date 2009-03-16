
#ifndef __BTL_CALC_H__
#define __BTL_CALC_H__

#include "battle\battle.h"
#include "btl_util.h"
#include "btl_pokeparam.h"

enum {
	BTL_CALC_BASERANK_DEFAULT = 0,

	BTL_CALC_HITRATIO_MID = 6,
	BTL_CALC_HITRATIO_MIN = 0,
	BTL_CALC_HITRATIO_MAX = 12,

	BTL_CALC_CRITICAL_MIN = 0,
	BTL_CALC_CRITICAL_MAX = 4,

// �_���[�W�v�Z�֘A
	BTL_CALC_DMG_TARGET_RATIO_PLURAL = FX32_CONST(0.75f),			///< �����̂��Ώۃ��U�̃_���[�W�␳��
	BTL_CALC_DMG_TARGET_RATIO_NONE   = FX32_CONST(1),					///< �Ώۂɂ��_���[�W�␳�Ȃ�
	BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE = FX32_CONST(1.5f),		///< ���U�^�C�v���V��̑g�ݍ��킹�ŗL���ȏꍇ�̕␳��
	BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE = FX32_CONST(0.5f),	///< ���U�^�C�v���V��̑g�ݍ��킹�ŕs���ȏꍇ�̕␳��
	BTL_CALC_DMG_WEATHER_RATIO_NONE = FX32_CONST(1),						///< ���U�^�C�v���V��̑g�ݍ��킹�ɂ��␳�Ȃ�


// ��Ԉُ폈���֘A
	BTL_NEMURI_TURN_MIN = 3,	///< �u�˂ނ�v�ŏ��^�[����
	BTL_NEMURI_TURN_MAX = 8,	///< �u�˂ނ�v�ő�^�[����
	BTL_NEMURI_TURN_RANGE = (BTL_NEMURI_TURN_MAX - BTL_NEMURI_TURN_MIN + 1),

	BTL_MAHI_EXE_RATIO = 25,	///< �u�܂Ёv�ł��т�ē����Ȃ��m��
	BTL_KORI_MELT_RATIO = 20,	///< �u������v���n����m��

	BTL_DOKU_SPLIT_DENOM = 8,					///< �u�ǂ��v�ōő�HP�̉����̂P���邩
	BTL_YAKEDO_SPLIT_DENOM = 8,				///< �u�₯�ǁv�ōő�HP�̉����̂P���邩
	BTL_YAKEDO_DAMAGE_RATIO = 50,			///< �u�₯�ǁv�ŕ����_���[�W�������闦
	BTL_MOUDOKU_SPLIT_DENOM = 16,			///< �����ǂ����A�ő�HP�̉����̂P���邩�i��{�l=�ŏ��l�j
	BTL_MOUDOKU_COUNT_MAX = BTL_MOUDOKU_SPLIT_DENOM-1,	///< �����ǂ����A�_���[�W�{���������܂ŃJ�E���g�A�b�v���邩

	BTL_CONF_EXE_RATIO = 30,		///< �������̎����m��
	BTL_CONF_TURN_MIN = 2,
	BTL_CONF_TURN_MAX = 5,
	BTL_CONF_TURN_RANGE = (BTL_CONF_TURN_MAX - BTL_CONF_TURN_MIN)+1,

	BTL_CALC_SICK_TURN_PERMANENT = 0xff,	///< �p���^�[������ݒ肵�Ȃ��i�i������j�ꍇ�̎w��l

	// �V��֘A
	BTL_CALC_WEATHER_MIST_HITRATIO = FX32_CONST(75),		///< �u����v�̎��̖������␳��
	BTL_WEATHER_TURN_DEFAULT = 5,					///< ���U�ɂ��V��ω��̌p���^�[����
	BTL_WEATHER_TURN_PERMANENT = 0xff,

};

//--------------------------------------------------------------------
/**
 *	�^�C�v�����i 0, 25, 50, 100, 200, 400% �̂U�i�K�j
 */
//--------------------------------------------------------------------
typedef enum {

	BTL_TYPEAFF_0 = 0,	///< ����
	BTL_TYPEAFF_25,
	BTL_TYPEAFF_50,
	BTL_TYPEAFF_100,
	BTL_TYPEAFF_200,
	BTL_TYPEAFF_400,

}BtlTypeAff;
//--------------------------------------------------------------------
/**
 *	�ȈՃ^�C�v�����i�������Ȃ��E�ӂ��E�΂���E�����ЂƂ̂S�i�K�j
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
 *	�^�C�v���� -> �ȈՃ^�C�v�����ϊ�
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


//=============================================================================================
/**
 * ��Ԉُ�̌p���^�[���������肷��
 *
 * @param   sick		��Ԉُ�ID
 *
 * @retval  u8			�p���^�[�����i�i������ُ�̏ꍇ��0�j
 */
//=============================================================================================
extern u8 BTL_CALC_DecideSickTurn( WazaSick sick );

#endif

