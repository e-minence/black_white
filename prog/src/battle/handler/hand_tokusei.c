//=============================================================================================
/**
 * @file	hand_tokusei.c
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[[�Ƃ�����]�ǉ�����
 * @author	taya
 *
 * @date	2008.11.11	�쐬
 */
//=============================================================================================

#include "poke_tool/poketype.h"
#include "poke_tool/monsno_def.h"

#include "../btl_common.h"
#include "../btl_calc.h"
#include "../btl_field.h"
#include "../btl_event_factor.h"

#include "hand_tokusei.h"

#include "tokusei/hand_tokusei_common.h"


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_Suisui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Youryokuso( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hayaasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tidoriasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Harikiri_HitRatio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Harikiri_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Fukugan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sunagakure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Yukigakure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Iromegane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HardRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sniper( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kasoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tekiouryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Mouka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gekiryu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sinryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MusinoSirase( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_hpborder_powerup( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType );
static void handler_Konjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Plus( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Minus( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tousousin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Technician( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_TetunoKobusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sutemi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_FusiginaUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SkillLink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Surudoime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tanjun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ReafGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Donkan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Amefurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hideri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sunaokosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Yukifurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_weather_change( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather );
static void handler_AirLock_Appear( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_AirLock_ChangeWeather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_IceBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_AmeukeZara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_weather_recover( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather );
static void handler_SunPower_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SunPower_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Rinpun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_TennoMegumi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_UruoiBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Dappi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_PoisonHeal( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KabutoArmor( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kyouun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_IkarinoTubo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DokunoToge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Seidenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HonoNoKarada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MeromeroBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Housi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_touchAddSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick, u8 per );
static void handler_Samehada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Syncro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Isiatama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NormalSkin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Trace( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenKaifuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Download( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Ganjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tennen_hitRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tennen_AtkRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tennen_DefRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tainetsu_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tainetsu_SickDmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL common_DmgRecover_Calc( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType, u8 denomHP, u32* recoverHP );
static void common_DmgRecover_Put( BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 recoverHP );
static void handler_Kandouhada_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kandouhada_DmgRecover( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tyosui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tikuden( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DenkiEngine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );





//=============================================================================================
/**
 * �|�P�����́u�Ƃ������v�n���h�����V�X�e���ɒǉ�
 *
 * @param   pp		
 *
 * @retval  BTL_EVENT_FACTOR*		
 */
//=============================================================================================
BTL_EVENT_FACTOR*  BTL_HANDLER_TOKUSEI_Add( const BTL_POKEPARAM* pp )
{
	static const struct {
		PokeTokusei				tokusei;
		pTokuseiEventAddFunc	func;
	}funcTbl[] = {
		{ POKETOKUSEI_IKAKU,					HAND_TOK_ADD_Ikaku },
		{ POKETOKUSEI_KURIABODY,			HAND_TOK_ADD_ClearBody },
		{ POKETOKUSEI_SIROIKEMURI,		HAND_TOK_ADD_ClearBody },	// ���낢���ނ�=�N���A�{�f�B�Ɠ���
		{ POKETOKUSEI_SEISINRYOKU,		HAND_TOK_ADD_Seisinryoku },
		{ POKETOKUSEI_FUKUTUNOKOKORO,	HAND_TOK_ADD_Fukutsuno },
		{ POKETOKUSEI_ATUISIBOU,			HAND_TOK_ADD_AtuiSibou },
		{ POKETOKUSEI_KAIRIKIBASAMI,	HAND_TOK_ADD_KairikiBasami },
		{ POKETOKUSEI_TIKARAMOTI,			HAND_TOK_ADD_Tikaramoti },
		{ POKETOKUSEI_YOGAPAWAA,			HAND_TOK_ADD_Tikaramoti },	// ���K�p���[ = ����������Ɠ���
		{ POKETOKUSEI_IROMEGANE,			HAND_TOK_ADD_Iromegane },
		{ POKETOKUSEI_KASOKU,					HAND_TOK_ADD_Kasoku },
		{ POKETOKUSEI_MOUKA,					HAND_TOK_ADD_Mouka },
		{ POKETOKUSEI_GEKIRYUU,				HAND_TOK_ADD_Gekiryu },
		{ POKETOKUSEI_SINRYOKU,				HAND_TOK_ADD_Sinryoku },
		{ POKETOKUSEI_MUSINOSIRASE,		HAND_TOK_ADD_MusinoSirase },
		{ POKETOKUSEI_KONJOU,					HAND_TOK_ADD_Konjou },
		{ POKETOKUSEI_SUKIRURINKU,		HAND_TOK_ADD_SkillLink },
		{ POKETOKUSEI_SURUDOIME,			HAND_TOK_ADD_Surudoime },
		{ POKETOKUSEI_TANJUN,					HAND_TOK_ADD_Tanjun },
		{ POKETOKUSEI_HAADOROKKU,			HAND_TOK_ADD_HardRock },
		{ POKETOKUSEI_FIRUTAA,				HAND_TOK_ADD_HardRock },	// �t�B���^�[ = �n�[�h���b�N�Ɠ���
		{ POKETOKUSEI_FUSIGINAUROKO,	HAND_TOK_ADD_FusiginaUroko },
		{ POKETOKUSEI_TOUSOUSIN,			HAND_TOK_ADD_Tousousin },
		{ POKETOKUSEI_RIIFUGAADO,			HAND_TOK_ADD_ReafGuard },
		{ POKETOKUSEI_AMEFURASI,			HAND_TOK_ADD_Amefurasi },
		{ POKETOKUSEI_HIDERI,					HAND_TOK_ADD_Hideri },
		{ POKETOKUSEI_SUNAOKOSI,			HAND_TOK_ADD_Sunaokosi },
		{ POKETOKUSEI_YUKIFURASI,			HAND_TOK_ADD_Yukifurasi },
		{ POKETOKUSEI_EAROKKU,				HAND_TOK_ADD_AirLock },
		{ POKETOKUSEI_NOOTENKI,				HAND_TOK_ADD_AirLock },	// �m�[�Ă� = �G�A���b�N�Ɠ���
		{ POKETOKUSEI_TEKUNISYAN,			HAND_TOK_ADD_Technician },
		{ POKETOKUSEI_DONKAN,					HAND_TOK_ADD_Donkan },
		{ POKETOKUSEI_URUOIBODY,			HAND_TOK_ADD_UruoiBody },
		{ POKETOKUSEI_POIZUNHIIRU,		HAND_TOK_ADD_PoisonHeal },
		{ POKETOKUSEI_AISUBODY,				HAND_TOK_ADD_IcoBody },
		{ POKETOKUSEI_AMEUKEZARA,			HAND_TOK_ADD_AmeukeZara },
		{ POKETOKUSEI_RINPUN,					HAND_TOK_ADD_Rinpun },
		{ POKETOKUSEI_TEKIOURYOKU,		HAND_TOK_ADD_Tekiouryoku },
		{ POKETOKUSEI_TENNOMEGUMI,		HAND_TOK_ADD_TennoMegumi },
		{ POKETOKUSEI_SANPAWAA,				HAND_TOK_ADD_SunPower },
		{ POKETOKUSEI_SUISUI,					HAND_TOK_ADD_Suisui },
		{ POKETOKUSEI_YOURYOKUSO,			HAND_TOK_ADD_Youryokuso },
		{ POKETOKUSEI_DAPPI,					HAND_TOK_ADD_Dappi },
		{ POKETOKUSEI_TIDORIASI,			HAND_TOK_ADD_Tidoriasi },
		{ POKETOKUSEI_HAYAASI,				HAND_TOK_ADD_Hayaasi },
		{ POKETOKUSEI_HARIKIRI,				HAND_TOK_ADD_Harikiri },
		{ POKETOKUSEI_KABUTOAAMAA,		HAND_TOK_ADD_KabutoArmor },
		{ POKETOKUSEI_SHERUAAMAA,			HAND_TOK_ADD_KabutoArmor },	// �V�F���A�[�}�[=�J�u�g�A�[�}�[�Ɠ���
		{ POKETOKUSEI_KYOUUN,					HAND_TOK_ADD_Kyouun },
		{ POKETOKUSEI_IKARINOTUBO,		HAND_TOK_ADD_IkarinoTubo },
		{ POKETOKUSEI_SUNAIPAA, 			HAND_TOK_ADD_Sniper },
		{ POKETOKUSEI_TETUNOKOBUSI,		HAND_TOK_ADD_TetunoKobusi },
		{ POKETOKUSEI_FUKUGAN,				HAND_TOK_ADD_Fukugan },
		{ POKETOKUSEI_ISIATAMA,				HAND_TOK_ADD_Isiatama },
		{ POKETOKUSEI_SUTEMI ,				HAND_TOK_ADD_Sutemi },
		{ POKETOKUSEI_SEIDENKI ,			HAND_TOK_ADD_Seidenki },
		{ POKETOKUSEI_DOKUNOTOGE,			HAND_TOK_ADD_DokunoToge },
		{ POKETOKUSEI_HONOONOKARADA,	HAND_TOK_ADD_HonoNoKarada },
		{ POKETOKUSEI_HOUSI,					HAND_TOK_ADD_Housi },
		{ POKETOKUSEI_PURASU,					HAND_TOK_ADD_Plus },
		{ POKETOKUSEI_MAINASU,				HAND_TOK_ADD_Minus },
		{ POKETOKUSEI_MEROMEROBODY,		HAND_TOK_ADD_MeromeroBody },
		{ POKETOKUSEI_SUNAGAKURE ,		HAND_TOK_ADD_Sunagakure },
		{ POKETOKUSEI_YUKIGAKURE ,		HAND_TOK_ADD_Yukigakure },
		{ POKETOKUSEI_TOREESU,				HAND_TOK_ADD_Trace },
		{ POKETOKUSEI_NOOMARUSUKIN,		HAND_TOK_ADD_NormalSkin },
		{ POKETOKUSEI_SAMEHADA,				HAND_TOK_ADD_Samehada },
		{ POKETOKUSEI_SIZENKAIFUKU,		HAND_TOK_ADD_SizenKaifuku },
		{ POKETOKUSEI_SINKURO,				HAND_TOK_ADD_Syncro },
		{ POKETOKUSEI_DAUNROODO,			HAND_TOK_ADD_DownLoad },
		{ POKETOKUSEI_GANJOU,					HAND_TOK_ADD_Ganjou },
		{ POKETOKUSEI_TAINETU,				HAND_TOK_ADD_Tainetu },
		{ POKETOKUSEI_TENNEN,					HAND_TOK_ADD_Tennen },
		{ POKETOKUSEI_KANSOUHADA,			HAND_TOK_ADD_Kansouhada },
		{ POKETOKUSEI_TIKUDEN,				HAND_TOK_ADD_Tikuden },
		{ POKETOKUSEI_TYOSUI,					HAND_TOK_ADD_Tyosui },
		{ POKETOKUSEI_DENKIENJIN,			HAND_TOK_ADD_DenkiEngine },
		{ POKETOKUSEI_JUUNAN,					HAND_TOK_ADD_Juunan },
		{ POKETOKUSEI_FUMIN,					HAND_TOK_ADD_Fumin },
		{ POKETOKUSEI_MAIPEESU,				HAND_TOK_ADD_MyPace },
		{ POKETOKUSEI_MAGUMANOYOROI,	HAND_TOK_ADD_MagumaNoYoroi },
		{ POKETOKUSEI_MENEKI,					HAND_TOK_ADD_Meneki },
		{ POKETOKUSEI_NOOGAADO,				HAND_TOK_ADD_NoGuard },

	};

	int i;
	u16 tokusei = BTL_POKEPARAM_GetValue( pp, BPP_TOKUSEI );

	for(i=0; i<NELEMS(funcTbl); i++)
	{
		if( funcTbl[i].tokusei == tokusei )
		{
			u16 agi = BTL_POKEPARAM_GetValue( pp, BPP_AGILITY );
			u8 pokeID = BTL_POKEPARAM_GetID( pp );

			BTL_Printf("�|�P����[%d]�� �Ƃ�����(%d)�n���h����ǉ�\n", pokeID, tokusei);
			return funcTbl[i].func( agi, pokeID );
		}
	}

	BTL_Printf("�Ƃ�����(%d)�̓n���h�����p�ӂ���Ă��Ȃ�\n", tokusei);
	return NULL;
}

//=============================================================================================
/**
 * �|�P�����́u�Ƃ������v�n���h�����V�X�e������폜
 *
 * @param   pp		
 *
 */
//=============================================================================================
void BTL_HANDLER_TOKUSEI_Remove( const BTL_POKEPARAM* pp )
{
	BTL_EVENT_FACTOR* factor;
	u8 pokeID = BTL_POKEPARAM_GetID( pp );

	while( (factor = BTL_EVENT_SeekFactor(BTL_EVENT_FACTOR_TOKUSEI, pokeID)) != NULL )
	{
		BTL_Printf("�|�P����[%d]�� �Ƃ������n���h��������\n", pokeID);
		BTL_EVENT_RemoveFactor( factor );
	}
}






//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���������v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_Suisui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		// �J�̂Ƃ��A���΂₳�Q�{
		if( BTL_FIELD_GetWeather() == BTL_WEATHER_RAIN )
		{
			u16 agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY ) * 2;
			BTL_EVENTVAR_SetValue( BTL_EVAR_AGILITY, agi );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Suisui( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_AGILITY, handler_Suisui },		// ���΂₳�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�悤��傭���v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_Youryokuso( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		// �͂�̎��A���΂₳�Q�{
		if( BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE )
		{
			u16 agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY ) * 2;
			BTL_EVENTVAR_SetValue( BTL_EVAR_AGILITY, agi );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Youryokuso( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_AGILITY, handler_Youryokuso },		// ���΂₳�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�͂₠���v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_Hayaasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		// ��Ԉُ�̎��A���΂₳1.5�{���}�q�ɂ�錸���𖳎�����
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_GetPokeSick(bpp) != POKESICK_NULL )
		{
			u32 agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
			agi = (agi * FX32_CONST(1.5f)) >> FX32_SHIFT;
			BTL_EVENTVAR_SetValue( BTL_EVAR_AGILITY, agi );
			BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Hayaasi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_AGILITY, handler_Hayaasi },		// ���΂₳�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���ǂ肠���v
 */
//------------------------------------------------------------------------------
// �������v�Z�n���h��
static void handler_Tidoriasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_KONRAN) )
		{
			u32 hitRatio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
			hitRatio = BTL_CALC_MulRatio( hitRatio, BTL_CALC_TOK_CHIDORI_HITRATIO );
			BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, hitRatio );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tidoriasi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_AGILITY, handler_Tidoriasi },		// ���΂₳�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�͂肫��v
 */
//------------------------------------------------------------------------------
// �������v�Z�n���h��
static void handler_Harikiri_HitRatio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �������U�͖���������
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
		{
			u32 hit_ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
			hit_ratio = BTL_CALC_MulRatio( hit_ratio, BTL_CALC_TOK_HARIKIRI_HITRATIO );
			BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, hit_ratio );
		}
	}
}
// �U���͌v�Z�n���h��
static void handler_Harikiri_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �������U�͈З͑���
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
		{
			u32 power = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
			power = BTL_CALC_MulRatio( power, BTL_CALC_TOK_HARIKIRI_POWRATIO );
			BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, power );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Harikiri( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_RATIO,	handler_Harikiri_HitRatio },	// �������v�Z�n���h��
		{ BTL_EVENT_ATTACKER_POWER,	handler_Harikiri_AtkPower },	// �U���͌v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ӂ�����v
 */
//------------------------------------------------------------------------------
// �������v�Z�n���h��
static void handler_Fukugan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		u32 hit_ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
		hit_ratio = BTL_CALC_MulRatio( hit_ratio, BTL_CALC_TOK_FUKUGAN_HITRATIO );
		if( hit_ratio > 100 ){ hit_ratio = 100; }
		BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, hit_ratio );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Fukugan( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_RATIO,	handler_Fukugan },	// �������v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���Ȃ�����v
 */
//------------------------------------------------------------------------------
// �������v�Z�n���h��
static void handler_Sunagakure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		if( BTL_FIELD_GetWeather() == BTL_WEATHER_SAND )
		{
			u32 hit_ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
			hit_ratio = BTL_CALC_MulRatio( hit_ratio, BTL_CALC_TOK_SUNAGAKURE_HITRATIO );
			BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, hit_ratio );
			BTL_Printf("�|�P[%d] �� ���Ȃ�����ɂ�薽������ ... %d%%\n", hit_ratio);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sunagakure( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_RATIO,	handler_Sunagakure },	// �������v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�䂫������v
 */
//------------------------------------------------------------------------------
// �������v�Z�n���h��
static void handler_Yukigakure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		if( BTL_FIELD_GetWeather() == BTL_WEATHER_SNOW )
		{
			u32 hit_ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
			hit_ratio = BTL_CALC_MulRatio( hit_ratio, BTL_CALC_TOK_SUNAGAKURE_HITRATIO );
			BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, hit_ratio );
			BTL_Printf("�|�P[%d] �� �䂫������ɂ�薽������ ... %d%%\n", hit_ratio);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Yukigakure( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_RATIO,	handler_Yukigakure },	// �������v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u����߂��ˁv
 */
//------------------------------------------------------------------------------
// �_���[�W�v�Z�ŏI�i�K�̃n���h��
static void handler_Iromegane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �����C�}�C�`�̎�
		BtlTypeAffAbout aff = BTL_CALC_TypeAffAbout( BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) );
		if( aff == BTL_TYPEAFF_ABOUT_DISADVANTAGE )
		{
			// �_���[�W�Q�{
			u32 dmg = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
			dmg *= 2;
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, dmg );
			BTL_Printf("�|�P[%d]�� ����߂��� �Ń_���[�W�Q�{\n", pokeID);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Iromegane( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_PROC2, handler_Iromegane },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�n�[�h���b�N�v���u�t�B���^�[�v
 */
//------------------------------------------------------------------------------
// �_���[�W�v�Z�ŏI�i�K�̃n���h��
static void handler_HardRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �h�䑤��������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// �������o�c�O���̎�
		BtlTypeAffAbout aff = BTL_CALC_TypeAffAbout( BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) );
		if( aff == BTL_TYPEAFF_ABOUT_ADVANTAGE )
		{
			// �_���[�W75��
			u32 dmg = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
			dmg = (dmg*75) / 100;
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, dmg );
			BTL_Printf("�|�P[%d]�� �n�[�h���b�N �Ń_���[�W75��\n", pokeID);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_HardRock( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_PROC2, handler_HardRock },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�X�i�C�p�[�v
 */
//------------------------------------------------------------------------------
// �_���[�W�v�Z�ŏI�i�K�̃n���h��
static void handler_Sniper( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �N���e�B�J���̎�
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_CRITICAL_FLAG) )
		{
			// �_���[�W�Q�{
			u32 dmg = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE ) * 2;
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, dmg );
			BTL_Printf("�|�P[%d]�� �X�i�C�p �Ń_���[�W�Q�{\n", pokeID);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sniper( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_PROC2, handler_Sniper },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}


//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�������v
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�̃n���h��
static void handler_Kasoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );

	BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
	BTL_SERVER_RECEPT_RankUpEffect( flowWk, myPos, BPP_AGILITY, 1 );
	BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kasoku( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_TURNCHECK_END, handler_Kasoku },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ă�������傭�v
 */
//------------------------------------------------------------------------------
// �^�C�v��v�␳���v�Z�n���h��
static void handler_Tekiouryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) )
		{
			BTL_EVENTVAR_SetValue(BTL_EVAR_RATIO, FX32_CONST(2));
			BTL_Printf("�|�P[%d]�� �Ă�������傭 �Ń^�C�v��v�␳���Q�{��\n", pokeID);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tekiouryoku( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_TYPEMATCH_RATIO, handler_Tekiouryoku },	// �^�C�v��v�␳���v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�������v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Mouka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_FIRE );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Mouka( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Mouka },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u������イ�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Gekiryu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_WATER );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Gekiryu( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Gekiryu },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�����傭�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Sinryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_KUSA );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sinryoku( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Sinryoku },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ނ��̂��点�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_MusinoSirase( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_MUSHI );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MusinoSirase( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_MusinoSirase },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//--------------------------------------------------------------------------
/**
 * HP��1/3�̎��ɁA����^�C�v�̃��U�З͂��グ��Ƃ������̋��ʏ���
 *
 * @param   flowWk		
 * @param   pokeID		
 * @param   wazaType		
 *
 */
//--------------------------------------------------------------------------
static void common_hpborder_powerup( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// HP 1/3 �ȉ���
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_GetHPBorder(bpp) <= BPP_HPBORDER_YELLOW )
		{
			// �g���̂��w��^�C�v���U�Ȃ�
			if( WAZADATA_GetType( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) ) == wazaType )
			{
				// �З͂Q�{
				u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				pow *= 2;
				BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, pow );
			}
		}
	}
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���񂶂傤�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Konjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// ��Ԉُ��
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_GetPokeSick(bpp) != POKESICK_NULL )
		{
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			// �_���[�W�^�C�v�������̎�
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
			{
				// �З͂Q�{
				u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				pow *= 2;
				BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, pow );
				BTL_Printf("�|�P[%d]�� ���񂶂傤 �ňЗ͂Q�{\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Konjou( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Konjou },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�v���X�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Plus( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// ��Ɂu�}�C�i�X�v����������
		if( BTL_SVFLOW_RECEPT_CheckExistTokuseiPokemon(flowWk, POKETOKUSEI_MAINASU) )
		{
			// �_���[�W�^�C�v������̎�
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
			{
				// �Ƃ�����1.5�{
				u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				pow = BTL_CALC_MulRatio( pow, BTL_CALC_TOK_PLUS_POWRATIO );
				BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, pow );
				BTL_Printf("�|�P[%d]�� �v���X �łƂ������A�b�v\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Plus( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Plus },	// �U���͌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�}�C�i�X�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Minus( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// ��Ɂu�v���X�v����������
		if( BTL_SVFLOW_RECEPT_CheckExistTokuseiPokemon(flowWk, POKETOKUSEI_PURASU) )
		{
			// �_���[�W�^�C�v������̎�
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
			{
				// �Ƃ�����1.5�{
				u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				pow = BTL_CALC_MulRatio( pow, BTL_CALC_TOK_PLUS_POWRATIO );
				BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, pow );
				BTL_Printf("�|�P[%d]�� �v���X �łƂ������A�b�v\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Minus( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Minus },	// �U���͌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}



//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ƃ���������v
 */
//------------------------------------------------------------------------------
// ���U�З͌���̃n���h��
static void handler_Tousousin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U����������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		const BTL_POKEPARAM* myParam = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		const BTL_POKEPARAM* targetParam = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk,
					BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

		u8 mySex = BTL_POKEPARAM_GetValue( myParam, BPP_SEX );
		u8 targetSex = BTL_POKEPARAM_GetValue( targetParam, BPP_SEX );

		// �݂��ɐ��ʕs������Ȃ��ꍇ
		if( (mySex!=PTL_SEX_UNKNOWN) && (targetSex!=PTL_SEX_UNKNOWN) )
		{
			u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
			if( mySex == targetSex ){
				pow = (pow * FX32_CONST(1.25f)) >> FX32_SHIFT;
			}else{
				pow = (pow * FX32_CONST(0.75f)) >> FX32_SHIFT;
			}
			BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tousousin( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER, handler_Tousousin },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�e�N�j�V�����v
 */
//------------------------------------------------------------------------------
// ���U�З͌���̃n���h��
static void handler_Technician( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U����������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �f�̈З�60�ȉ��ȃ��U�Ȃ�1.5�{�ɂ���
		WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_GetPower(waza) <= 60 )
		{
			u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
			pow = (pow * FX32_CONST(1.5f)) >> FX32_SHIFT;
			BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Technician( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER, handler_Technician },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ă̂��Ԃ��v
 */
//------------------------------------------------------------------------------
// ���U�З͌���̃n���h��
static void handler_TetunoKobusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U����������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �p���`�n�C���[�W�̃��U�Ȃ�З͑���
		WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_IsImage(waza, WAZA_IMG_PUNCH) )
		{
			u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
			BTL_CALC_MulRatio( pow, BTL_CALC_TOK_TETUNOKOBUSI_POWRATIO );
			BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_TetunoKobusi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER, handler_TetunoKobusi },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���Ă݁v
 */
//------------------------------------------------------------------------------
// ���U�З͌���̃n���h��
static void handler_Sutemi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U����������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �����̂���U���Ȃ�З͑���
		WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_GetReactionRatio(waza) )
		{
			u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
			BTL_CALC_MulRatio( pow, BTL_CALC_TOK_SUTEMI_POWRATIO );
			BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sutemi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER, handler_Sutemi },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ӂ����Ȃ��낱�v
 */
//------------------------------------------------------------------------------
///< �h��\�͌���̃n���h��
static void handler_FusiginaUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �h�䑤��������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// �~���J���X�ŏ�Ԉُ��
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( (BTL_POKEPARAM_GetPokeSick(bpp) != POKESICK_NULL)
		&&	(BTL_POKEPARAM_GetMonsNo(bpp) == MONSNO_MIROKAROSU)
		){
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			// �_���[�W�^�C�v������̎�
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
			{
				// �h��Q�{
				u32 guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
				guard *= 2;
				BTL_EVENTVAR_SetValue( BTL_EVAR_GUARD, guard );
				BTL_Printf("�|�P[%d]�� �ӂ����Ȃ܂��� �Ŗh��Q�{\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_FusiginaUroko( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DEFENDER_GUARD, handler_FusiginaUroko },	///< �h��\�͌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�X�L�������N�v
 */
//------------------------------------------------------------------------------
// �U���񐔌���̃n���h��
static void handler_SkillLink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U�����������̎�
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �q�b�g�񐔂��ő��
		u16 max = BTL_EVENTVAR_GetValue( BTL_EVAR_HITCOUNT_MAX );
		BTL_EVENTVAR_SetValue( BTL_EVAR_HITCOUNT, max );
		BTL_Printf("POKE[%d]�� �X�L�������N �ŁA�q�b�g�񐔂��ő��%d�ɂ���\n", max);
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_SkillLink( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_COUNT, handler_SkillLink },	// �U���񐔌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u����ǂ��߁v
 */
//------------------------------------------------------------------------------
// �����N�_�E�����O�����̃n���h��
static void handler_Surudoime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
	&&	(BTL_EVENTVAR_GetValue(BTL_EVAR_STATUS_TYPE) ==  WAZA_RANKEFF_HIT)
	){
		BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, TRUE );
		BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
		BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_RankdownFail_HIT, pokeID );
		BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Surudoime( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_BEFORE_RANKDOWN, handler_Surudoime },	// �����N�_�E�����O�����̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���񂶂��v
 */
//------------------------------------------------------------------------------
// �����N�_�E�����O�������N�A�b�v���O�̃n���h��
static void handler_Tanjun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		u8 volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
		volume *= 2;
		BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
		BTL_Printf("�|�P[%d]�� ���񂶂�� �ɂ����ʔ{��\n");
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tanjun( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_BEFORE_RANKDOWN, handler_Tanjun },	// �����N�_�E�����O�����̃n���h��
		{ BTL_EVENT_BEFORE_RANKUP, handler_Tanjun },		// �����N�A�b�v���O�����̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���[�t�K�[�h�v
 */
//------------------------------------------------------------------------------
// ��Ԉُ킭�炤�O�̃n���h��
static void handler_ReafGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// ���炤��������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// �V�󂪐���
		if( BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE )
		{
			// ��Ԉُ�̎�ނ��|�P�����n
			if( BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) < POKESICK_MAX )
			{
				BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, POKESICK_NULL );
				if( BTL_EVENTVAR_GetValue(BTL_EVAR_ALMOST_FLAG) )
				{
					BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
					BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_NoEffect, pokeID );
					BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
				}
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_ReafGuard( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_SICK, handler_ReafGuard },	// ��Ԉُ킭�炤�O�̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ǂ񂩂�v
 */
//------------------------------------------------------------------------------
// ��Ԉُ킭�炤�O�̃n���h��
static void handler_Donkan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// ���炤��������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// ��������
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) == WAZASICK_MEROMERO )
		{
			BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, WAZASICK_MEROMERO );
			if( BTL_EVENTVAR_GetValue(BTL_EVAR_ALMOST_FLAG) )
			{
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_NoEffect, pokeID );
				BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Donkan( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_SICK, handler_Donkan },	// ��Ԉُ킭�炤�O�̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���߂ӂ炵�v
 */
//------------------------------------------------------------------------------
// �������o��n���h��
static void handler_Amefurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_change( flowWk, pokeID, BTL_WEATHER_RAIN );
	BTL_EVENT_RemoveFactor( myHandle );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Amefurasi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Amefurasi },	// �������o��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ђł�v
 */
//------------------------------------------------------------------------------
// �������o��n���h��
static void handler_Hideri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_change( flowWk, pokeID, BTL_WEATHER_SHINE );
	BTL_EVENT_RemoveFactor( myHandle );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Hideri( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Hideri },	// �������o��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���Ȃ������v
 */
//------------------------------------------------------------------------------
// �������o��n���h��
static void handler_Sunaokosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_change( flowWk, pokeID, BTL_WEATHER_SAND );
	BTL_EVENT_RemoveFactor( myHandle );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sunaokosi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Sunaokosi },	// �������o��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�䂫�ӂ炵�v
 */
//------------------------------------------------------------------------------
// �������o��n���h��
static void handler_Yukifurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_change( flowWk, pokeID, BTL_WEATHER_SNOW );
	BTL_EVENT_RemoveFactor( myHandle );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Yukifurasi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Yukifurasi },	// �������o��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//-------------------------------------------
/**
 *	�V��ω��Ƃ������̋��ʏ���
 */
//-------------------------------------------
static void common_weather_change( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
		BTL_SVFLOW_RECEPT_ChangeWeather( flowWk, weather );
		BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
	}
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�G�A���b�N�v&�u�m�[�Ă񂫁v
 */
//------------------------------------------------------------------------------
// �������o��n���h��
static void handler_AirLock_Appear( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BTL_Printf("�|�P[%d]�̃G�A���b�N�I �����o��n���h��\n", pokeID);
		if( BTL_FIELD_GetWeather() != BTL_WEATHER_NONE )
		{
			BTL_Printf("  ... �V����t���b�g�ɂ����\n");
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			BTL_SVFLOW_RECEPT_ChangeWeather( flowWk, BTL_WEATHER_NONE );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}
	}
}
// �V��ω��n���h��
static void handler_AirLock_ChangeWeather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	BTL_Printf("�|�P[%d]�̃G�A���b�N�I �V��ω��n���h��\n", pokeID);
	BTL_EVENTVAR_SetValue( BTL_EVAR_WEATHER, BTL_WEATHER_NONE );

	BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
	BTL_SERVER_RECTPT_StdMessage( flowWk, BTL_STRID_STD_WeatherLocked );
	BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_AirLock( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN,			handler_AirLock_Appear },					// �������o��n���h��
		{ BTL_EVENT_WEATHER_CHANGE, handler_AirLock_ChangeWeather },	// �V��ω��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�A�C�X�{�f�B�v
 */
//------------------------------------------------------------------------------
// �V��_���[�W�v�Z�n���h��
static void handler_IceBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_recover( flowWk, pokeID, BTL_WEATHER_SNOW );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_IcoBody( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_WEATHER_DAMAGE, handler_IceBody },	// �V��_���[�W�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���߂�������v
 */
//------------------------------------------------------------------------------
// �V��_���[�W�v�Z�n���h��
static void handler_AmeukeZara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_recover( flowWk, pokeID, BTL_WEATHER_RAIN );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_AmeukeZara( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_WEATHER_DAMAGE, handler_AmeukeZara },	// �V��_���[�W�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

/**
 *	�V��ɉ�����HP�񕜂���Ƃ������̋��ʏ���
 */
static void common_weather_recover( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_WEATHER) == weather )
		{
			const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
			int recoverHP = BTL_CALC_QuotMaxHP( bpp, 8 ) * -1;
			recoverHP *= -1;
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, recoverHP );
		}
		BTL_SVFLOW_RECEPT_ChangeWeather( flowWk, weather );
	}
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�T���p���[�v
 */
//------------------------------------------------------------------------------
// �V��_���[�W�v�Z�n���h��
static void handler_SunPower_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		// ����̎��AMAXHP�� 1/8 ����
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_WEATHER) == BTL_WEATHER_SHINE )
		{
			const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
			int damage = BTL_CALC_QuotMaxHP( bpp, 8 );
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, damage );
		}
	}
}
// �U���͌���n���h��
static void handler_SunPower_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �V�󂪐����
		if( BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE )
		{
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			// �_���[�W�^�C�v������̎�
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
			{
				// �З�1.5�{
				u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				pow = (pow * FX32_CONST(1.5f)) >> FX32_SHIFT;
				BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, pow );
				BTL_Printf("�|�P[%d]�� �T���p���[ �ňЗ�1.5�{\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_SunPower( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_WEATHER_DAMAGE, handler_SunPower_Weather },// �V��_���[�W�v�Z�n���h��
		{ BTL_EVENT_ATTACKER_POWER,	 handler_SunPower_AtkPower },		// �U���͌���n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���Ղ�v
 */
//------------------------------------------------------------------------------
// �ǉ����ʁi�Ђ�݁A��Ԉُ�C�����N���ʋ��ʁj�n���h��
static void handler_Rinpun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
	{
		BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, 0 );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Rinpun( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ADD_SICK,				handler_Rinpun },	// �ǉ����ʁi��Ԉُ�j�`�F�b�N�n���h��
		{ BTL_EVENT_ADD_RANK_TARGET,handler_Rinpun },	// �ǉ����ʁi�����N���ʁj�`�F�b�N�n���h��
		{ BTL_EVENT_SHRINK_CHECK,		handler_Rinpun },	// �Ђ�݃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ă�̂߂��݁v
 */
//------------------------------------------------------------------------------
// �ǉ����ʁi�Ђ�݁A��Ԉُ�C�����N���ʋ��ʁj�n���h��
static void handler_TennoMegumi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
	{
		u16 per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER ) * 2;
		BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
		BTL_Printf("�|�P[%d]�� �Ă�̂߂��� �Œǉ��������Q�{=%d%%\n", pokeID, per);
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_TennoMegumi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ADD_SICK,				handler_TennoMegumi },	// �ǉ����ʁi��Ԉُ�j�`�F�b�N�n���h��
		{ BTL_EVENT_ADD_RANK_TARGET,handler_TennoMegumi },	// �ǉ����ʁi�����N���ʁj�`�F�b�N�n���h��
		{ BTL_EVENT_SHRINK_CHECK,		handler_TennoMegumi },	// �Ђ�݃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���邨���{�f�B�v
 */
//------------------------------------------------------------------------------
// ��Ԉُ�_���[�W�n���h��
static void handler_UruoiBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) )
	{
		if( BTL_FIELD_GetWeather() == BTL_WEATHER_RAIN )
		{
			WazaSick sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
			if( sick < POKESICK_MAX )
			{
				BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, POKESICK_NULL );
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				BTL_SVFLOW_RECEPT_CurePokeSick( flowWk, pokeID );
				BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_UruoiBody( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_SICK_DAMAGE,			handler_UruoiBody },	// ��Ԉُ�_���[�W�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�����ҁv
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�J�n�n���h��
static void handler_Dappi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
	if( BTL_POKEPARAM_GetPokeSick(bpp) != POKESICK_NULL )
	{
		if( GFL_STD_MtRand(100) < 33 )
		{
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			BTL_SVFLOW_RECEPT_CurePokeSick( flowWk, pokeID );
			BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_PokeSick_Cure, pokeID );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Dappi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_TURNCHECK_BEGIN,			handler_Dappi },	// �^�[���`�F�b�N�J�n
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�|�C�Y���q�[���v
 */
//------------------------------------------------------------------------------
// ��Ԉُ�_���[�W�n���h��
static void handler_PoisonHeal( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) )
	{
		WazaSick sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
		if( (sick == WAZASICK_DOKU) || (sick == WAZASICK_MOUDOKU) )
		{
			const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
			int recoverHP = BTL_CALC_QuotMaxHP( bpp, 8 );
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, 0 );
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			BTL_SERVER_RECEPT_HP_Add( flowWk, pokeID, recoverHP );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_PoisonHeal( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_SICK_DAMAGE,			handler_PoisonHeal },	// ��Ԉُ�_���[�W�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�J�u�g�A�[�}�[�v���u�V�F���A�[�}�[�v
 */
//------------------------------------------------------------------------------
// �N���e�B�J���`�F�b�N�_���[�W�n���h��
static void handler_KabutoArmor( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
	{
		BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_KabutoArmor( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CRITICAL_CHECK,			handler_KabutoArmor },	// �N���e�B�J���`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���傤����v
 */
//------------------------------------------------------------------------------
// �N���e�B�J���`�F�b�N�_���[�W�n���h��
static void handler_Kyouun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
	{
		u8 rank = BTL_EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK ) + 1;
		BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_RANK, rank );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kyouun( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CRITICAL_CHECK,			handler_Kyouun },	// �N���e�B�J���`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u������̂ځv
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_IkarinoTubo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
	{
		// �N���e�B�J����������U���P�i�A�b�v
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) )
		{
			BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			BTL_SERVER_RECEPT_RankUpEffect( flowWk, myPos, BPP_ATTACK, 1 );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_IkarinoTubo( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_IkarinoTubo },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ǂ��̃g�Q�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_DokunoToge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_touchAddSick( flowWk, pokeID, POKESICK_DOKU, BTL_CALC_TOK_DOKUNOTOGE_PER );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_DokunoToge( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_DokunoToge },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�����ł񂫁v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Seidenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_touchAddSick( flowWk, pokeID, POKESICK_MAHI, BTL_CALC_TOK_SEIDENKI_PER );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Seidenki( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_Seidenki },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ق̂��̂��炾�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_HonoNoKarada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_touchAddSick( flowWk, pokeID, POKESICK_YAKEDO, BTL_CALC_TOK_HONONOKARADA_PER );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_HonoNoKarada( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_HonoNoKarada },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���������{�f�B�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_MeromeroBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
	{
		const BTL_POKEPARAM* myParam = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		const BTL_POKEPARAM* targetParam = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk,
					BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) );

		u8 mySex = BTL_POKEPARAM_GetValue( myParam, BPP_SEX );
		u8 targetSex = BTL_POKEPARAM_GetValue( targetParam, BPP_SEX );

		if(	((mySex!=PTL_SEX_UNKNOWN) && (targetSex!=PTL_SEX_UNKNOWN))
		&&	(mySex != targetSex)
		){
			common_touchAddSick( flowWk, pokeID, WAZASICK_MEROMERO, BTL_CALC_TOK_MEROMEROBODY_PER );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MeromeroBody( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_MeromeroBody },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ق����v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Housi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
	{
		WazaSick sick;
		u32 rand = GFL_STD_MtRand(90);
		if( rand < 30 ){
			sick = WAZASICK_DOKU;
		}else if( rand < 60 ){
			sick = WAZASICK_MAHI;
		}else{
			sick = WAZASICK_YAKEDO;
		}
		common_touchAddSick( flowWk, pokeID, sick, BTL_CALC_TOK_HOUSI_PER );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Housi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_Housi },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//--------------------------------------------------------------------------
/**
 * �ڐG���U�ɂ��_���[�W��A����ɑ΂��ď�Ԉُ�𔭏ǂ�����Ƃ������̋��ʏ���
 *
 * @param   flowWk		
 * @param   pokeID		
 * @param   sick			��Ԉُ�ID
 * @param   per				�����m��
 *
 */
//--------------------------------------------------------------------------
static void common_touchAddSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick, u8 per )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
	{
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_IsTouch(waza) )
		{
			if( BTL_CALC_IsOccurPer(per) )
			{
				u8 attackerPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				BTL_SVFLOW_RECEPT_MakePokeSick( flowWk, attackerPokeID, pokeID, sick, FALSE );
				BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
			}
		}
	}
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���߂͂��v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Samehada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
	{
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_IsTouch(waza) )
		{
			u8 attackerPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
			const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, attackerPokeID );
			int dmg = BTL_CALC_QuotMaxHP( bpp, 16 ) * -1;
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			BTL_SERVER_RECEPT_HP_Add( flowWk, attackerPokeID, dmg );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Samehada( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_Samehada },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�V���N���v
 */
//------------------------------------------------------------------------------
// ��Ԉُ�n���h��
static void handler_Syncro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
	{
		PokeSick sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
		if( (sick==POKESICK_DOKU)||(sick==POKESICK_MAHI) ||(sick==POKESICK_YAKEDO) )
		{
			u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
			if( targetPokeID != BTL_POKEID_NULL )
			{
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				BTL_SVFLOW_RECEPT_MakePokeSick( flowWk, targetPokeID, pokeID, sick, TRUE );
				BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Syncro( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_SICK,			handler_Syncro },	// ��Ԉُ�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}


//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���������܁v
 */
//------------------------------------------------------------------------------
// �����v�Z�n���h��
static void handler_Isiatama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
	{
		BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, 0 );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Isiatama( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_REACTION,			handler_Isiatama },	// �����v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�m�[�}���X�L���v
 */
//------------------------------------------------------------------------------
// ���U�^�C�v����n���h��
static void handler_NormalSkin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
	{
		BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, POKETYPE_NORMAL );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_NormalSkin( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_TYPE,		handler_NormalSkin },	// ���U�^�C�v����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�g���[�X�v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Trace( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		u8  allPokeID[ BTL_POSIDX_MAX ];
		u8  targetPokeID[ BTL_POSIDX_MAX ];
		u8  pokeCnt, targetCnt, i;

		targetCnt = 0;
		pokeCnt = BTL_SVFLOW_RECEPT_GetAllOpponentFrontPokeID( flowWk, pokeID, allPokeID );
		BTL_Printf("�g���[�X�n���h���ł��F����|�P��=%d\n", pokeCnt);
		for(i=0; i<pokeCnt; ++i)
		{
			const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, allPokeID[i] );
			PokeTokusei tok = BTL_POKEPARAM_GetValue( bpp, BPP_TOKUSEI );
			if( !BTL_CALC_TOK_IsUntracable( tok ) )
			{
				targetPokeID[ targetCnt++ ] = allPokeID[i];
			}
		}
		if( targetCnt )
		{
			u8 idx = (targetCnt==1)? 0 : GFL_STD_MtRand( targetCnt );
			BTL_Printf("�g���[�X�n���h���ł��F�Ώۃ|�P��=%d, �����_������=%d\n", targetCnt, idx);
			BTL_SVFLOW_RECEPT_TraceTokusei( flowWk, pokeID, targetPokeID[idx] );
			BTL_EVENT_RemoveFactor( myHandle );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Trace( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Trace },			// ����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�����񂩂��ӂ��v
 */
//------------------------------------------------------------------------------
// �����o�[�ޏo�n���h��
static void handler_SizenKaifuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BTL_SVFLOW_RECEPT_CurePokeSick( flowWk, pokeID );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_SizenKaifuku( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_OUT, handler_SizenKaifuku },		// �����o�[�ޏo�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�_�E�����[�h�v
 */
//------------------------------------------------------------------------------
// �����o�[����n���h��
static void handler_Download( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		u8 ePokeID[BTL_POSIDX_MAX]; 
		u8 pokeCnt = BTL_SVFLOW_RECEPT_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
		if( pokeCnt ){
			u8 idx = 0;
			if( pokeCnt > 1 ){
				idx = GFL_STD_MtRand( pokeCnt );
			}

			// ����̂ڂ�����E�Ƃ��ڂ����r���āA�ア���ɑΉ����������̂��������E�Ƃ��������A�b�v
			{
				const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, ePokeID[idx] );
				u16 def = BTL_POKEPARAM_GetValue( bpp, BPP_DEFENCE );
				u16 sp_def = BTL_POKEPARAM_GetValue( bpp, BPP_SP_DEFENCE );
				u8 valueID;
				if( def > sp_def ){
					valueID = BPP_SP_ATTACK;
				}else if( def < sp_def ) {
					valueID = BPP_ATTACK;
				}else {
					valueID = (GFL_STD_MtRand(2))? BPP_SP_ATTACK : BPP_ATTACK;
				}

				{
					BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
					BTL_SERVER_RECEPT_RankUpEffect( flowWk, myPos, valueID, 1 );
				}
			}
		}
		BTL_EVENT_RemoveFactor( myHandle );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_DownLoad( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Download },		// �����o�[����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���񂶂傤�v
 */
//------------------------------------------------------------------------------
// �ꌂ�`�F�b�N�n���h��
static void handler_Ganjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Ganjou( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ICHIGEKI_CHECK, handler_Ganjou },		// �ꌂ�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ă�˂�v
 */
//------------------------------------------------------------------------------
// �������E��𗦃`�F�b�N�n���h��
static void handler_Tennen_hitRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������U�����̎��A�h�䑤�̉�𗦂��t���b�g��
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		BTL_EVENTVAR_SetValue( BTL_EVAR_AVOID_RANK, BTL_CALC_HITRATIO_MID );
	}
	// �������h�䑤�̎��A�U�����̖��������t���b�g��
	else if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		BTL_EVENTVAR_SetValue( BTL_EVAR_HIT_RANK, BTL_CALC_HITRATIO_MID );
	}
}
// �U�������N�`�F�b�N�n���h��
static void handler_Tennen_AtkRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������h�䑤�̎��A�U�����̃����N���t���b�g��
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE );
	}
}
// �h�䃉���N�`�F�b�N�n���h��
static void handler_Tennen_DefRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������U�����̎��A�h�䑤�̃����N���t���b�g��
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tennen( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ICHIGEKI_CHECK, handler_Tennen_hitRank },		// �������E��𗦃`�F�b�N�n���h��
		{ BTL_EVENT_ATTACKER_POWER_PREV, handler_Tennen_AtkRank }, // �U�������N�`�F�b�N�n���h��
		{ BTL_EVENT_DEFENDER_GUARD_PREV, handler_Tennen_DefRank }, // �h�䃉���N�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�����˂v
 */
//------------------------------------------------------------------------------
// ���U�З͌v�Z�n���h��
static void handler_Tainetsu_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������h�䑤�̎��A�����U�̈З͂𔼕���
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_GetType(waza) == POKETYPE_FIRE )
		{
			u32 pow = BTL_CALC_Roundup( BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER ) / 2, 1 );
			BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
// ��Ԉُ�_���[�W�v�Z�n���h��
static void handler_Tainetsu_SickDmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// ��������炤���A�₯�ǂȂ�_���[�W������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		WazaSick sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
		if( sick == WAZASICK_YAKEDO )
		{
			u32 dmg = BTL_CALC_Roundup( BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE ) / 2, 1 );
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, dmg );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tainetu( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER,		handler_Tainetsu_WazaPow },		// ���U�З͌v�Z�n���h��
		{ BTL_EVENT_SICK_DAMAGE,	handler_Tainetsu_SickDmg },		// ��Ԉُ�_���[�W�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}



//--------------------------------------------------------------------------
/**
 * �_���[�W���U���������񕜂���Ƃ������̋��ʏ��� - �v�Z
 *
 * @param   flowWk		
 * @param   pokeID		
 * @param   type			�Ώۃ��U�^�C�v
 * @param   denomHP		MAXHP�̉����̂P�񕜂��邩�H
 * @param   recoverHP	[out] �񕜂���HP��
 *
 * @retval  BOOL			����������ꍇ��TRUE
 */
//--------------------------------------------------------------------------
static BOOL common_DmgRecover_Calc( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType, u8 denomHP, u32* recoverHP )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_GetType(waza) == wazaType )
		{
			const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

			BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE );
			if( !BTL_POKEPARAM_IsFullHP( bpp ) ){
				*recoverHP = BTL_CALC_QuotMaxHP( bpp, denomHP );
			}
			else{
				*recoverHP = 0;
			}
			return TRUE;
		}
	}
	*recoverHP = 0;
	return FALSE;
}
//--------------------------------------------------------------------------
/**
 * �_���[�W���U���������񕜂���Ƃ������̋��ʏ��� - �R�}���h�o��
 *
 * @param   flowWk		
 * @param   pokeID		
 * @param   recoverHP		
 *
 */
//--------------------------------------------------------------------------
static void common_DmgRecover_Put( BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 recoverHP )
{
	if( recoverHP )
	{
		BTL_SERVER_RECEPT_HP_Add( flowWk, pokeID, recoverHP );
		BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_HP_Recover, pokeID );
	}
	else
	{
		BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_NoEffect, pokeID );
	}
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���񂻂��͂��v
 */
//------------------------------------------------------------------------------
// �V��_���[�W�v�Z�n���h��
static void handler_Kandouhada_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		int damage = BTL_CALC_QuotMaxHP( bpp, 8 );
		BtlWeather weather = BTL_EVENTVAR_GetValue( BTL_EVAR_WEATHER );

		// ����̎��AMAXHP�� 1/8 ����
		if( weather == BTL_WEATHER_SHINE ){
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, damage );
		// �J�̎��AMAXHP�� 1/8 ��
		}else if( weather == BTL_WEATHER_RAIN ) {
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, -damage );
		}
	}
}
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Kandouhada_DmgRecover( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	u32 recoverHP;

	if( common_DmgRecover_Calc(flowWk, pokeID, POKETYPE_WATER, 4, &recoverHP) )
	{
		BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
		common_DmgRecover_Put( flowWk, pokeID, recoverHP );
		BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kansouhada( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_WEATHER_DAMAGE,	handler_Kandouhada_Weather },			// �V��_���[�W�v�Z�n���h��
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Kandouhada_DmgRecover },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���傷���v
 */
//------------------------------------------------------------------------------
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Tyosui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	u32 recoverHP;
	if( common_DmgRecover_Calc(flowWk, pokeID, POKETYPE_WATER, 4, &recoverHP) )
	{
		BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
		common_DmgRecover_Put( flowWk, pokeID, recoverHP );
		BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tyosui( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Tyosui },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�����ł�v
 */
//------------------------------------------------------------------------------
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Tikuden( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	u32 recoverHP;
	if( common_DmgRecover_Calc(flowWk, pokeID, POKETYPE_ELECTRIC, 4, &recoverHP) )
	{
		BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
		common_DmgRecover_Put( flowWk, pokeID, recoverHP );
		BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tikuden( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Tikuden },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ł񂫃G���W���v
 */
//------------------------------------------------------------------------------
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_DenkiEngine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	u32 recoverHP;
	if( common_DmgRecover_Calc(flowWk, pokeID, POKETYPE_ELECTRIC, 4, &recoverHP) )
	{
		BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
		common_DmgRecover_Put( flowWk, pokeID, recoverHP );
		{
			BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
			BTL_SERVER_RECEPT_RankUpEffect( flowWk, myPos, BPP_AGILITY, 1 );
		}
		BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_DenkiEngine( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,		handler_DenkiEngine },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

BTL_EVENT_FACTOR*  HAND_TOK_ADD_Juunan( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Kandouhada_DmgRecover },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Fumin( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Kandouhada_DmgRecover },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MyPace( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Kandouhada_DmgRecover },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MagumaNoYoroi( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Kandouhada_DmgRecover },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Meneki( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Kandouhada_DmgRecover },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_NoGuard( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Kandouhada_DmgRecover },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
