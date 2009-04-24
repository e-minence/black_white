//=============================================================================================
/**
 * @file	hand_tokusei.c
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[[�Ƃ�����]�ǉ�����
 * @author	taya
 *
 * @date	2008.11.11	�쐬
 */
//=============================================================================================

#include "poke_tool\poketype.h"
#include "poke_tool\monsno_def.h"
#include "waza_tool\wazano_def.h"

#include "..\btl_common.h"
#include "..\btl_calc.h"
#include "..\btl_field.h"
#include "..\btl_client.h"
#include "..\btl_event_factor.h"

#include "hand_tokusei.h"

#include "tokusei\hand_tokusei_common.h"


/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_Suisui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Youryokuso( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hayaasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tidoriasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Harikiri_HitRatio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Harikiri_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Agility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SlowStart_Agility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SlowStart_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static void handler_FlowerGift_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_FlowerGift_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tousousin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Technician( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_TetunoKobusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sutemi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_FusiginaUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SkillLink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Surudoime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tanjun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ReafGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Juunan_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Juunan_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Fumin_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Fumin_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagumaNoYoroi_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagumaNoYoroi_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Meneki_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Meneki_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MyPace_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MyPace_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MizuNoBale_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MizuNoBale_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_DiscardWazaSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick );
static void common_Swap_CureSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick );
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
static void handler_Rinpun_Shrink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static void common_touchAddSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick, BPP_SICK_CONT sickCont, u8 per );
static void handler_Samehada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Yuubaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hensyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Syncro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Isiatama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NormalSkin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MultiType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Trace( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenKaifuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Download( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Yotimu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KikenYoti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL _check_kikenyoti_poke( const BTL_POKEPARAM* bppUser, const BTL_POKEPARAM* bppEnemy );
static void handler_Omitoosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static void handler_NoGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kimottama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Bouon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Fuyuu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_FusiginaMamori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Namake( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Simerike( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Moraibi_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Moraibi_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nightmare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nigeasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Arijigoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kagefumi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Jiryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Katayaburi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tenkiya_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tenkiya_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_TenkiFormChange( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather );
static void handler_Yobimizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hiraisin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_WazaTargetChangeToMe( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType );
static void handler_Kyuuban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HedoroEki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );





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
		{ POKETOKUSEI_YARUKI,					HAND_TOK_ADD_Fumin },		// ��邫=�ӂ݂�Ɠ���
		{ POKETOKUSEI_MAIPEESU,				HAND_TOK_ADD_MyPace },
		{ POKETOKUSEI_MAGUMANOYOROI,	HAND_TOK_ADD_MagumaNoYoroi },
		{ POKETOKUSEI_MIZUNOBEERU,		HAND_TOK_ADD_MizuNoBale },
		{ POKETOKUSEI_MENEKI,					HAND_TOK_ADD_Meneki },
		{ POKETOKUSEI_NOOGAADO,				HAND_TOK_ADD_NoGuard },
		{ POKETOKUSEI_KIMOTTAMA,			HAND_TOK_ADD_Kimottama },
		{ POKETOKUSEI_BOUON,					HAND_TOK_ADD_Bouon },
		{ POKETOKUSEI_FUYUU,					HAND_TOK_ADD_Fuyuu },
		{ POKETOKUSEI_FURAWAAGIFUTO,	HAND_TOK_ADD_FlowerGift },
		{ POKETOKUSEI_MORAIBI,				HAND_TOK_ADD_Moraibi },
		{ POKETOKUSEI_YOTIMU,					HAND_TOK_ADD_Yotimu },
		{ POKETOKUSEI_KIKENYOTI,			HAND_TOK_ADD_KikenYoti },
		{ POKETOKUSEI_OMITOOSI,				HAND_TOK_ADD_Omitoosi },
		{ POKETOKUSEI_YOBIMIZU,				HAND_TOK_ADD_Yuubaku },
		{ POKETOKUSEI_NIGEASI,				HAND_TOK_ADD_Nigeasi },
		{ POKETOKUSEI_HENSYOKU,				HAND_TOK_ADD_Hensyoku },
		{ POKETOKUSEI_KATAYABURI,			HAND_TOK_ADD_Katayaburi },
		{ POKETOKUSEI_NAMAKE,					HAND_TOK_ADD_Namake },
		{ POKETOKUSEI_HIRAISIN,				HAND_TOK_ADD_Hiraisin },
		{ POKETOKUSEI_YOBIMIZU,				HAND_TOK_ADD_Yobimizu },
		{ POKETOKUSEI_SUROOSUTAATO,		HAND_TOK_ADD_SlowStart },
		{ POKETOKUSEI_KATAYABURI,			HAND_TOK_ADD_Katayaburi },
		{ POKETOKUSEI_ARIJIGOKU,			HAND_TOK_ADD_Arijigoku },
		{ POKETOKUSEI_KAGEFUMI,				HAND_TOK_ADD_Kagefumi },
		{ POKETOKUSEI_JIRYOKU,				HAND_TOK_ADD_Jiryoku },
		{ POKETOKUSEI_SIMERIKE,				HAND_TOK_ADD_Simerike },
		{ POKETOKUSEI_MARUTITAIPU,		HAND_TOK_ADD_MultiType },
		{ POKETOKUSEI_FUSIGINAMAMORI,	HAND_TOK_ADD_FusiginaMamori },
		{ POKETOKUSEI_ATODASI,				HAND_TOK_ADD_Agility },
		{ POKETOKUSEI_TENKIYA,				HAND_TOK_ADD_Tenkiya },
		{ POKETOKUSEI_KYUUBAN,				HAND_TOK_ADD_Kyuuban },
		{ POKETOKUSEI_HEDOROEKI,			HAND_TOK_ADD_HedoroEki },

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
			return funcTbl[i].func( agi, tokusei, pokeID );
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
		BTL_EVENT_FACTOR_Remove( factor );
	}
}


void BTL_HANDLER_TOKUSEI_Pause( const BTL_POKEPARAM* pp )
{
	
}

void BTL_HANDLER_TOKUSEI_Swap( const BTL_POKEPARAM* pp1, const BTL_POKEPARAM* pp2 )
{
	BTL_EVENT_FACTOR *factor1, *factor2;
	u8 ID_1, ID_2;

	ID_1 = BTL_POKEPARAM_GetID( pp1 );
	ID_2 = BTL_POKEPARAM_GetID( pp2 );
	factor1 = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_TOKUSEI, ID_1 );
	factor2 = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_TOKUSEI, ID_2 );

	if( factor1 && factor2 )
	{
		u16 pri_1 = BTL_POKEPARAM_GetValue( pp1, BPP_AGILITY );
		u16 pri_2 = BTL_POKEPARAM_GetValue( pp2, BPP_AGILITY );

		BTL_EVENT_FACTOR_ChangePokeParam( factor1, ID_2, pri_2 );
		BTL_EVENT_FACTOR_ChangePokeParam( factor2, ID_1, pri_1 );
	}
}

//=============================================================================================
/**
 * �Ƃ������n���h���Ǘ��p�Ƀ^�[���̏���ɕK���Ăяo�����
 */
//=============================================================================================
/*
void BTL_HANDLER_TOKUSEI_InitTurn( void )
{
	GFL_STD_MemClear( DisableTokTable, sizeof(DisableTokTable) );
}
*/

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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_AGILITY, agi );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Suisui( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_AGILITY, handler_Suisui },		// ���΂₳�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_AGILITY, agi );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Youryokuso( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_AGILITY, handler_Youryokuso },		// ���΂₳�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			agi = BTL_CALC_MulRatio( agi, BTL_CALC_TOK_HAYAASI_AGIRATIO );
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_AGILITY, agi );
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, FALSE );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Hayaasi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_AGILITY, handler_Hayaasi },		// ���΂₳�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, hitRatio );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tidoriasi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_RATIO, handler_Tidoriasi },		// �������v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, hit_ratio );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, power );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Harikiri( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_RATIO,	handler_Harikiri_HitRatio },	// �������v�Z�n���h��
		{ BTL_EVENT_ATTACKER_POWER,	handler_Harikiri_AtkPower },	// �U���͌v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���Ƃ����v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_Agility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_AGILITY, 0 );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Agility( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_AGILITY,		handler_Agility },	// ���΂₳�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�X���[�X�^�[�g�v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_SlowStart_Agility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( (BTL_POKEPARAM_GetMonsNo(bpp) == MONSNO_REZIGIGASU)
		&&	(BTL_POKEPARAM_GetTurnCount(bpp) < BTL_CALC_TOK_SLOWSTART_ENABLE_TURN )
		){
			u32 agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
			agi = BTL_CALC_MulRatio( agi, BTL_CALC_TOK_SLOWSTART_AGIRATIO );
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_AGILITY, agi );
		}
	}
}
// �U���͌v�Z�n���h��
static void handler_SlowStart_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( (BTL_POKEPARAM_GetMonsNo(bpp) == MONSNO_REZIGIGASU)
		&&	(BTL_POKEPARAM_GetTurnCount(bpp) < BTL_CALC_TOK_SLOWSTART_ENABLE_TURN )
		){
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
			{
				u32 atk = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				atk = BTL_CALC_MulRatio( atk, BTL_CALC_TOK_SLOWSTART_ATKRATIO );
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, atk );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_SlowStart( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_AGILITY,		handler_SlowStart_Agility },	// ���΂₳�v�Z�n���h��
		{ BTL_EVENT_ATTACKER_POWER,	handler_SlowStart_AtkPower },	// �U���͌v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, hit_ratio );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Fukugan( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_RATIO,	handler_Fukugan },	// �������v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, hit_ratio );
			BTL_Printf("�|�P[%d] �� ���Ȃ�����ɂ�薽������ ... %d%%\n", hit_ratio);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sunagakure( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_RATIO,	handler_Sunagakure },	// �������v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, hit_ratio );
			BTL_Printf("�|�P[%d] �� �䂫������ɂ�薽������ ... %d%%\n", hit_ratio);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Yukigakure( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_RATIO,	handler_Yukigakure },	// �������v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, dmg );
			BTL_Printf("�|�P[%d]�� ����߂��� �Ń_���[�W�Q�{\n", pokeID);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Iromegane( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_PROC2, handler_Iromegane },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, dmg );
			BTL_Printf("�|�P[%d]�� �n�[�h���b�N �Ń_���[�W75��\n", pokeID);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_HardRock( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_PROC2, handler_HardRock },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, dmg );
			BTL_Printf("�|�P[%d]�� �X�i�C�p �Ń_���[�W�Q�{\n", pokeID);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sniper( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_PROC2, handler_Sniper },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
	BTL_SERVER_RECEPT_RankUpEffect( flowWk, myPos, BPP_AGILITY, 1, TRUE );
	BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kasoku( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_TURNCHECK_END, handler_Kasoku },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue(BTL_EVAR_RATIO, FX32_CONST(2));
			BTL_Printf("�|�P[%d]�� �Ă�������傭 �Ń^�C�v��v�␳���Q�{��\n", pokeID);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tekiouryoku( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_TYPEMATCH_RATIO, handler_Tekiouryoku },	// �^�C�v��v�␳���v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�������v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Mouka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_HONOO );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Mouka( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Mouka },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u������イ�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Gekiryu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_MIZU );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Gekiryu( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Gekiryu },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sinryoku( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Sinryoku },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MusinoSirase( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_MusinoSirase },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
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
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
				BTL_Printf("�|�P[%d]�� ���񂶂傤 �ňЗ͂Q�{\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Konjou( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Konjou },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
				BTL_Printf("�|�P[%d]�� �v���X �łƂ������A�b�v\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Plus( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Plus },	// �U���͌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
				BTL_Printf("�|�P[%d]�� �v���X �łƂ������A�b�v\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Minus( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Minus },	// �U���͌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�t�����[�M�t�g�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_FlowerGift_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �V�C�u�͂�v��
	if( BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE )
	{
		// �U�����������������̂Ƃ�
		u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
		if( BTL_MAINUTIL_IsFriendPokeID(pokeID, atkPokeID) )
		{
			// ��������1.5�{
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
			{
				u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				pow = BTL_CALC_MulRatio( pow, BTL_CALC_TOK_FLOWERGIFT_POWRATIO );
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
			}
		}
	}
}
// �h��͌���̃n���h��
static void handler_FlowerGift_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �V�C�u�͂�v��
	if( BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE )
	{
		// �h�䑤�������������̂Ƃ�
		u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
		if( BTL_MAINUTIL_IsFriendPokeID(pokeID, atkPokeID) )
		{
			// �Ƃ��ڂ�1.5�{
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
			{
				u32 guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
				guard = BTL_CALC_MulRatio( guard, BTL_CALC_TOK_FLOWERGIFT_GUARDRATIO );
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_GUARD, guard );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_FlowerGift( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_FlowerGift_Power },	// �U���͌���̃n���h��
		{ BTL_EVENT_DEFENDER_GUARD, handler_FlowerGift_Guard },	// �h��͌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tousousin( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER, handler_Tousousin },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			pow = BTL_CALC_MulRatio( pow, BTL_CALC_TOK_TECKNICIAN_POWRATIO );
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Technician( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER, handler_Technician },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_TetunoKobusi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER, handler_TetunoKobusi },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sutemi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER, handler_Sutemi },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		// ��Ԉُ��
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_GetPokeSick(bpp) != POKESICK_NULL )
		{
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			// �_���[�W�^�C�v������̎�
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
			{
				// �h��Q�{
				u32 guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
				guard = BTL_CALC_MulRatio( guard, BTL_CALC_TOK_FUSIGINAUROKO_GDRATIO );
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_GUARD, guard );
				BTL_Printf("�|�P[%d]�� �ӂ����Ȃ��낱 �Ŗh��1.5�{\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_FusiginaUroko( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DEFENDER_GUARD, handler_FusiginaUroko },	///< �h��\�͌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_HITCOUNT, max );
		BTL_Printf("POKE[%d]�� �X�L�������N �ŁA�q�b�g�񐔂��ő��%d�ɂ���\n", max);
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_SkillLink( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_COUNT, handler_SkillLink },	// �U���񐔌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u����ǂ��߁v
 */
//------------------------------------------------------------------------------
// �����N�_�E�����O�����̃n���h��
static void handler_Surudoime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	WazaRankEffect  effType = WAZA_RANKEFF_HIT;
	if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
	&&	(BTL_EVENTVAR_GetValue(BTL_EVAR_STATUS_TYPE) ==  effType)
	){
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_VOLUME) < 0 )
		{
			BTL_EVWK_CHECK_RANKEFF* evwk = (BTL_EVWK_CHECK_RANKEFF*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
			evwk->failFlag = TRUE;
			evwk->failTokuseiFlag = TRUE;
			evwk->failSpecificType = effType;
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Surudoime( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CHECK_RANKEFF, handler_Surudoime },	// �����N�_�E�����O�����̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���񂶂��v
 */
//------------------------------------------------------------------------------
// ���U�ɂ�郉���N�������ʃ`�F�b�N�n���h��
static void handler_Tanjun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		int volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
		volume *= 2;
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, volume );
		BTL_Printf("�|�P[%d]�� ���񂶂�� �ɂ����ʔ{��\n");
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tanjun( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_GET_RANKEFF_VALUE, handler_Tanjun },	// ���U�ɂ�郉���N�������ʃ`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���[�t�K�[�h�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_ReafGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// ���炤��������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// �V�󂪐���
		if( BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE )
		{
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, POKESICK_NULL );
			if( BTL_EVENTVAR_GetValue(BTL_EVAR_ALMOST_FLAG) )
			{
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_NoEffect, pokeID );
				BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_ReafGuard( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK, handler_ReafGuard },	// �|�P�����n��Ԉُ폈���n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���イ�Ȃ�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_Juunan_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_DiscardWazaSick( flowWk, pokeID, POKESICK_MAHI );
}
// �Ƃ���������ւ��n���h��
static void handler_Juunan_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_Swap_CureSick( flowWk, pokeID, POKESICK_MAHI );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Juunan( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK,handler_Juunan_PokeSick },	// �|�P�����n��Ԉُ폈���n���h��
		{ BTL_EVENT_SKILL_SWAP,		handler_Juunan_Swap },			// �Ƃ���������ւ��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ӂ݂�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_Fumin_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_DiscardWazaSick( flowWk, pokeID, POKESICK_NEMURI );
}
// �Ƃ���������ւ��n���h��
static void handler_Fumin_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_Swap_CureSick( flowWk, pokeID, POKESICK_NEMURI );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Fumin( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK,handler_Fumin_PokeSick },	// �|�P�����n��Ԉُ폈���n���h��
		{ BTL_EVENT_SKILL_SWAP,		handler_Fumin_Swap },			// �Ƃ���������ւ��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�}�O�}�̂�낢�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_MagumaNoYoroi_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_DiscardWazaSick( flowWk, pokeID, POKESICK_YAKEDO );
}
// �Ƃ���������ւ��n���h��
static void handler_MagumaNoYoroi_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_Swap_CureSick( flowWk, pokeID, POKESICK_YAKEDO );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MagumaNoYoroi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK,handler_MagumaNoYoroi_PokeSick },	// �|�P�����n��Ԉُ폈���n���h��
		{ BTL_EVENT_SKILL_SWAP,		handler_MagumaNoYoroi_Swap },			// �Ƃ���������ւ��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�߂񂦂��v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_Meneki_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_DiscardWazaSick( flowWk, pokeID, POKESICK_DOKU );
}
// �Ƃ���������ւ��n���h��
static void handler_Meneki_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_Swap_CureSick( flowWk, pokeID, POKESICK_DOKU );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Meneki( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK,handler_Meneki_PokeSick },	// �|�P�����n��Ԉُ폈���n���h��
		{ BTL_EVENT_SKILL_SWAP,		handler_Meneki_Swap },			// �Ƃ���������ւ��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�}�C�y�[�X�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_MyPace_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_DiscardWazaSick( flowWk, pokeID, WAZASICK_KONRAN );
}
// �Ƃ���������ւ��n���h��
static void handler_MyPace_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_Swap_CureSick( flowWk, pokeID, WAZASICK_KONRAN );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MyPace( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK,handler_MyPace_PokeSick },	// �|�P�����n��Ԉُ폈���n���h��
		{ BTL_EVENT_SKILL_SWAP,		handler_MyPace_Swap },			// �Ƃ���������ւ��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�݂��̃x�[���v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_MizuNoBale_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_DiscardWazaSick( flowWk, pokeID, WAZASICK_YAKEDO );
}
// �Ƃ���������ւ��n���h��
static void handler_MizuNoBale_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_Swap_CureSick( flowWk, pokeID, WAZASICK_YAKEDO );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MizuNoBale( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK,handler_MizuNoBale_PokeSick },	// �|�P�����n��Ԉُ폈���n���h��
		{ BTL_EVENT_SKILL_SWAP,		handler_MizuNoBale_Swap },			// �Ƃ���������ւ��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

//--------------------------------------------------------------------------
/**
 * ��Ԉُ���󂯕t���Ȃ��Ƃ������̋��ʏ���
 *
 * @param   flowWk		
 * @param   pokeID		
 * @param   sick		
 *
 */
//--------------------------------------------------------------------------
static void common_DiscardWazaSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick )
{
	// ���炤��������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// ���炤�a�C���w��ʂ�
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) == sick )
		{
			BTL_EVWK_ADDSICK* evwk = (BTL_EVWK_ADDSICK*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );

			evwk->reaction = BTL_EV_SICK_REACTION_DISCARD;
			evwk->discardSickType = sick;
			evwk->fDiscardByTokusei = TRUE;
			evwk->discardPokeID = pokeID;
		}
	}
}
static void common_Swap_CureSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick )
{
	if(	(BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
	||	(BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
	){
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_CheckSick(bpp, sick) )
		{
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			if( sick < POKESICK_MAX )
			{
				BTL_SVFLOW_RECEPT_CurePokeSick( flowWk, pokeID );
			}
			else
			{
				BTL_SVFLOW_RECEPT_CureWazaSick( flowWk, pokeID, sick );
			}
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}
	}
}


//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ǂ񂩂�v
 */
//------------------------------------------------------------------------------
// ���U�n��Ԉُ�n���h��
static void handler_Donkan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// ���炤��������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// ��������
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) == WAZASICK_MEROMERO )
		{
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, WAZASICK_NULL );
			if( BTL_EVENTVAR_GetValue(BTL_EVAR_ALMOST_FLAG) )
			{
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_NoEffect, pokeID );
				BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Donkan( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_WAZASICK, handler_Donkan },	// ���U�n��Ԉُ�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���߂ӂ炵�v
 */
//------------------------------------------------------------------------------
static void handler_Amefurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_change( flowWk, pokeID, BTL_WEATHER_RAIN );
	BTL_EVENT_FACTOR_Remove( myHandle );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Amefurasi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Amefurasi },	// �������o��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ђł�v
 */
//------------------------------------------------------------------------------
static void handler_Hideri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_change( flowWk, pokeID, BTL_WEATHER_SHINE );
	BTL_EVENT_FACTOR_Remove( myHandle );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Hideri( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Hideri },	// �������o��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���Ȃ������v
 */
//------------------------------------------------------------------------------
static void handler_Sunaokosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_change( flowWk, pokeID, BTL_WEATHER_SAND );
	BTL_EVENT_FACTOR_Remove( myHandle );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sunaokosi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Sunaokosi },	// �������o��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�䂫�ӂ炵�v
 */
//------------------------------------------------------------------------------
static void handler_Yukifurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_weather_change( flowWk, pokeID, BTL_WEATHER_SNOW );
	BTL_EVENT_FACTOR_Remove( myHandle );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Yukifurasi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Yukifurasi },	// �������o��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
	BTL_EVENTVAR_RewriteValue( BTL_EVAR_WEATHER, BTL_WEATHER_NONE );

	BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
	BTL_SERVER_RECTPT_StdMessage( flowWk, BTL_STRID_STD_WeatherLocked );
	BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_AirLock( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN,			handler_AirLock_Appear },					// �������o��n���h��
		{ BTL_EVENT_WEATHER_CHANGE, handler_AirLock_ChangeWeather },	// �V��ω��n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
BTL_EVENT_FACTOR*  HAND_TOK_ADD_IcoBody( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_WEATHER_DAMAGE, handler_IceBody },	// �V��_���[�W�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
BTL_EVENT_FACTOR*  HAND_TOK_ADD_AmeukeZara( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_WEATHER_DAMAGE, handler_AmeukeZara },	// �V��_���[�W�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, recoverHP );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
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
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
				BTL_Printf("�|�P[%d]�� �T���p���[ �ňЗ�1.5�{\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_SunPower( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_WEATHER_DAMAGE, handler_SunPower_Weather },// �V��_���[�W�v�Z�n���h��
		{ BTL_EVENT_ATTACKER_POWER,	 handler_SunPower_AtkPower },		// �U���͌���n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���Ղ�v
 */
//------------------------------------------------------------------------------
// �ǉ����ʁi��Ԉُ�C�����N���ʋ��ʁj�n���h��
static void handler_Rinpun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
	{
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 0 );
	}
}
// �Ђ�݃`�F�b�N�n���h��
static void handler_Rinpun_Shrink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
	{
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Rinpun( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ADD_SICK,				handler_Rinpun },	// �ǉ����ʁi��Ԉُ�j�`�F�b�N�n���h��
		{ BTL_EVENT_ADD_RANK_TARGET,handler_Rinpun },	// �ǉ����ʁi�����N���ʁj�`�F�b�N�n���h��
		{ BTL_EVENT_SHRINK_CHECK,		handler_Rinpun_Shrink },	// �Ђ�݃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
		BTL_Printf("�|�P[%d]�� �Ă�̂߂��� �Œǉ��������Q�{=%d%%\n", pokeID, per);
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_TennoMegumi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ADD_SICK,				handler_TennoMegumi },	// �ǉ����ʁi��Ԉُ�j�`�F�b�N�n���h��
		{ BTL_EVENT_ADD_RANK_TARGET,handler_TennoMegumi },	// �ǉ����ʁi�����N���ʁj�`�F�b�N�n���h��
		{ BTL_EVENT_SHRINK_CHECK,		handler_TennoMegumi },	// �Ђ�݃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, POKESICK_NULL );
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				BTL_SVFLOW_RECEPT_CurePokeSick( flowWk, pokeID );
				BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_UruoiBody( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_SICK_DAMAGE,			handler_UruoiBody },	// ��Ԉُ�_���[�W�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Dappi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_TURNCHECK_BEGIN,			handler_Dappi },	// �^�[���`�F�b�N�J�n
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, 0 );
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			BTL_SERVER_RECEPT_HP_Add( flowWk, pokeID, recoverHP );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_PoisonHeal( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_SICK_DAMAGE,			handler_PoisonHeal },	// ��Ԉُ�_���[�W�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_KabutoArmor( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CRITICAL_CHECK,			handler_KabutoArmor },	// �N���e�B�J���`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kyouun( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CRITICAL_CHECK,			handler_Kyouun },	// �N���e�B�J���`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_CRITICAL_FLAG) )
		{
			BTL_EVWK_DAMAGE_REACTION* evwk = (BTL_EVWK_DAMAGE_REACTION*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );

			evwk->reaction = BTL_EV_AFTER_DAMAGED_REACTION_RANK;
			evwk->targetPokeID = pokeID;
			evwk->rankType = WAZA_RANKEFF_ATTACK;
			evwk->rankVolume = 1;
			evwk->tokFlag = TRUE;
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_IkarinoTubo( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_IkarinoTubo },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ǂ��̃g�Q�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_DokunoToge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	PokeSick sick = POKESICK_DOKU;
	BPP_SICK_CONT cont;
	BTL_CALC_MakeDefaultPokeSickCont( sick, &cont );
	common_touchAddSick( flowWk, pokeID, sick, cont, BTL_CALC_TOK_DOKUNOTOGE_PER );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_DokunoToge( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_DokunoToge },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�����ł񂫁v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Seidenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	PokeSick sick = POKESICK_MAHI;
	BPP_SICK_CONT cont;
	BTL_CALC_MakeDefaultPokeSickCont( sick, &cont );
	common_touchAddSick( flowWk, pokeID, sick, cont, BTL_CALC_TOK_DOKUNOTOGE_PER );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Seidenki( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_Seidenki },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ق̂��̂��炾�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_HonoNoKarada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	PokeSick sick = POKESICK_YAKEDO;
	BPP_SICK_CONT cont;
	BTL_CALC_MakeDefaultPokeSickCont( sick, &cont );
	common_touchAddSick( flowWk, pokeID, sick, cont, BTL_CALC_TOK_DOKUNOTOGE_PER );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_HonoNoKarada( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_HonoNoKarada },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			BPP_SICK_CONT cont;
			BTL_CALC_MakeDefaultWazaSickCont( WAZASICK_MEROMERO, myParam, &cont );
			common_touchAddSick( flowWk, pokeID, WAZASICK_MEROMERO, cont, BTL_CALC_TOK_MEROMEROBODY_PER );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MeromeroBody( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_MeromeroBody },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		PokeSick sick;
		BPP_SICK_CONT cont;
		u32 rand = GFL_STD_MtRand(90);
		if( rand < 30 ){
			sick = WAZASICK_DOKU;
		}else if( rand < 60 ){
			sick = WAZASICK_MAHI;
		}else{
			sick = WAZASICK_YAKEDO;
		}
		BTL_CALC_MakeDefaultPokeSickCont( sick, &cont );
		common_touchAddSick( flowWk, pokeID, sick, cont, BTL_CALC_TOK_HOUSI_PER );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Housi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_Housi },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
static void common_touchAddSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick, BPP_SICK_CONT sickCont, u8 per )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
	{
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		if( WAZADATA_IsTouch(waza) )
		{
			if( BTL_CALC_IsOccurPer(per) )
			{
				BTL_EVWK_DAMAGE_REACTION* evwk = (BTL_EVWK_DAMAGE_REACTION*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );

				evwk->reaction = BTL_EV_AFTER_DAMAGED_REACTION_SICK;
				evwk->targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
				evwk->sick = sick;
				evwk->sickCont = sickCont;
				evwk->tokFlag = TRUE;
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
			BTL_EVWK_DAMAGE_REACTION* evwk = (BTL_EVWK_DAMAGE_REACTION*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
			u8 attackerPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
			const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, attackerPokeID );

			evwk->reaction = BTL_EV_AFTER_DAMAGED_REACTION_DAMAGE;
			evwk->targetPokeID = attackerPokeID;
			evwk->damage = BTL_CALC_QuotMaxHP( bpp, 16 );
			evwk->tokFlag = TRUE;
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Samehada( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_Samehada },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�䂤�΂��v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Yuubaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
	{
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_IsDead(bpp) )
		{
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			if( WAZADATA_IsTouch(waza) )
			{
				BTL_EVWK_DAMAGE_REACTION* evwk = (BTL_EVWK_DAMAGE_REACTION*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
				u8 attackerPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
				const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, attackerPokeID );

				evwk->reaction = BTL_EV_AFTER_DAMAGED_REACTION_DAMAGE;
				evwk->targetPokeID = attackerPokeID;
				evwk->damage = BTL_CALC_QuotMaxHP( bpp, 4 );
				evwk->tokFlag = TRUE;
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Yuubaku( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_Yuubaku },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ւ񂵂傭�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Hensyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( !BTL_POKEPARAM_IsDead(bpp) )
		{
			PokeType type = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
			if( !BTL_POKEPARAM_IsMatchType(bpp, type) )
			{
				BTL_EVWK_DAMAGE_REACTION* evwk = (BTL_EVWK_DAMAGE_REACTION*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );

				evwk->reaction = BTL_EV_AFTER_DAMAGED_REACTION_POKETYPE;
				evwk->targetPokeID = pokeID;
				evwk->pokeType = type;
				evwk->tokFlag = TRUE;
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Hensyoku( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,			handler_Hensyoku },	// �_���[�W����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�V���N���v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ�n���h��
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
				BTL_EVWK_ADDSICK* evwk = (BTL_EVWK_ADDSICK*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
				evwk->reaction = BTL_EV_SICK_REACTION_REFRECT;
/*
				BPP_SICK_CONT cont;
				cont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				BTL_SVFLOW_RECEPT_AddSick( flowWk, targetPokeID, pokeID, sick, cont, TRUE );
				BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
*/
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Syncro( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK,			handler_Syncro },	// �|�P�����n��Ԉُ�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, 0 );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Isiatama( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_REACTION,			handler_Isiatama },	// �����v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, POKETYPE_NORMAL );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_NormalSkin( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_PARAM,		handler_NormalSkin },	// ���U�^�C�v����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�}���`�^�C�v�v
 */
//------------------------------------------------------------------------------
// ���U�^�C�v����n���h��
static void handler_MultiType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
	{
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_GetMonsNo(bpp) == MONSNO_ARUSEUSU )
		{
			// @@@ �����Ă���A�C�e���Ń^�C�v��ς���
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, POKETYPE_NORMAL );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MultiType( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_PARAM,		handler_MultiType },	// ���U�^�C�v����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
			if( !BTL_CALC_TOK_CheckCant_Trace( tok ) )
			{
				targetPokeID[ targetCnt++ ] = allPokeID[i];
			}
		}
		if( targetCnt )
		{
			u8 idx = (targetCnt==1)? 0 : GFL_STD_MtRand( targetCnt );
			BTL_Printf("�g���[�X�n���h���ł��F�Ώۃ|�P��=%d, �����_������=%d\n", targetCnt, idx);
			BTL_SVFLOW_RECEPT_TraceTokusei( flowWk, pokeID, targetPokeID[idx] );
			BTL_EVENT_FACTOR_Remove( myHandle );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Trace( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Trace },			// ����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
BTL_EVENT_FACTOR*  HAND_TOK_ADD_SizenKaifuku( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_OUT, handler_SizenKaifuku },		// �����o�[�ޏo�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
					BTL_EVENT_FACTOR_Remove( myHandle );
				}
			}
		}
		BTL_EVENT_FACTOR_Remove( myHandle );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_DownLoad( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN, handler_Download },		// �����o�[����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�悿�ށv
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Yotimu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		const BTL_POKEPARAM* bpp;
		u8 ePokeID[BTL_POSIDX_MAX]; 
		u8 pokeCnt = BTL_SVFLOW_RECEPT_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
		u8 pokeIdx = GFL_STD_MtRand( pokeCnt );
		bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, ePokeID[pokeIdx] );

		{
			u8 wazaCnt = BTL_POKEPARAM_GetWazaCount( bpp );
			u8 wazaIdx = GFL_STD_MtRand( wazaCnt );
			WazaID waza = BTL_POKEPARAM_GetWazaNumber( bpp, wazaIdx );

			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
//			BTL_SVFLOW_RECEPT_AddSick( flowWk, targetPokeID, pokeID, sick, cont, TRUE );
			BTL_SERVER_RECTPT_SetMessageEx( flowWk, BTL_STRID_SET_YotimuExe, pokeID, waza );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );

			BTL_EVENT_FACTOR_Remove( myHandle );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Yotimu( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN,				handler_Yotimu },	// ����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u������悿�v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_KikenYoti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		const BTL_POKEPARAM* bppUser;
		const BTL_POKEPARAM* bppEnemy;
		u8 ePokeID[BTL_POSIDX_MAX]; 
		u8 pokeCnt = BTL_SVFLOW_RECEPT_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
		u8 i;
		bppUser = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		for(i=0; i<pokeCnt; ++i)
		{
			bppEnemy = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, ePokeID[i] );
			if( _check_kikenyoti_poke(bppUser, bppEnemy) )
			{
				break;
			}
		}
		if( i != pokeCnt )
		{
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_Kikenyoti, pokeID );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}
		BTL_EVENT_FACTOR_Remove( myHandle );
	}
}
static BOOL _check_kikenyoti_poke( const BTL_POKEPARAM* bppUser, const BTL_POKEPARAM* bppEnemy )
{
	WazaID  waza;
	PokeTypePair user_type;
	PokeType     waza_type;
	u8 waza_cnt, i;

	user_type = BTL_POKEPARAM_GetPokeType( bppUser );
	waza_cnt = BTL_POKEPARAM_GetWazaCount( bppEnemy );
	for(i=0; i<waza_cnt; ++i)
	{
		// �ꌂ�K�E���U�̓L�P��
		waza = BTL_POKEPARAM_GetWazaNumber( bppEnemy, i );
		if( WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_ICHIGEKI ){ return TRUE; }

		// �����ɑ����o�c�O���ȓz���L�P��
		waza_type = WAZADATA_GetType( waza );
		if( BTL_CALC_TypeAff(waza_type, user_type) > BTL_TYPEAFF_100 ){ return TRUE; }

		// @@@ �܂������n�����Ă��Ȃ�
	}
	return FALSE;
}

BTL_EVENT_FACTOR*  HAND_TOK_ADD_KikenYoti( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN,				handler_KikenYoti },	// ����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���݂Ƃ����v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Omitoosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		const BTL_POKEPARAM* bpp;
		u16 itemID[ BTL_POSIDX_MAX ];
		u8  ePokeID[ BTL_POSIDX_MAX ];
		u8 pokeCnt = BTL_SVFLOW_RECEPT_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
		u8 itemCnt, i;
		for(itemCnt=0, i=0; i<pokeCnt; ++i)
		{
			bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, ePokeID[i] );
			itemID[ itemCnt ] = BTL_POKEPARAM_GetItem( bpp );
			if( itemID[itemCnt] ){
				++itemCnt;
			}
		}

		if( itemCnt ){
			i = GFL_STD_MtRand( itemCnt );
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			BTL_SERVER_RECTPT_SetMessageEx( flowWk, BTL_STRID_SET_Omitoosi, pokeID, itemID[i] );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}

		BTL_EVENT_FACTOR_Remove( myHandle );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Omitoosi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN,				handler_Omitoosi },	// ����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Ganjou( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ICHIGEKI_CHECK, handler_Ganjou },		// �ꌂ�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_AVOID_RANK, BTL_CALC_HITRATIO_MID );
	}
	// �������h�䑤�̎��A�U�����̖��������t���b�g��
	else if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_HIT_RANK, BTL_CALC_HITRATIO_MID );
	}
}
// �U�������N�`�F�b�N�n���h��
static void handler_Tennen_AtkRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������h�䑤�̎��A�U�����̃����N���t���b�g��
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
	}
}
// �h�䃉���N�`�F�b�N�n���h��
static void handler_Tennen_DefRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������U�����̎��A�h�䑤�̃����N���t���b�g��
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tennen( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ICHIGEKI_CHECK, handler_Tennen_hitRank },		// �������E��𗦃`�F�b�N�n���h��
		{ BTL_EVENT_ATTACKER_POWER_PREV, handler_Tennen_AtkRank }, // �U�������N�`�F�b�N�n���h��
		{ BTL_EVENT_DEFENDER_GUARD_PREV, handler_Tennen_DefRank }, // �h�䃉���N�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		if( WAZADATA_GetType(waza) == POKETYPE_HONOO )
		{
			u32 pow = BTL_CALC_Roundup( BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER ) / 2, 1 );
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, dmg );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tainetu( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER,		handler_Tainetsu_WazaPow },		// ���U�З͌v�Z�n���h��
		{ BTL_EVENT_SICK_DAMAGE,	handler_Tainetsu_SickDmg },		// ��Ԉُ�_���[�W�v�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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

			BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
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
	BTL_EVWK_DMG_TO_RECOVER* evwk = (BTL_EVWK_DMG_TO_RECOVER*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );

	evwk->pokeID = pokeID;
	evwk->recoverHP = recoverHP;
	evwk->tokFlag = TRUE;
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
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
		// �J�̎��AMAXHP�� 1/8 ��
		}else if( weather == BTL_WEATHER_RAIN ) {
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, -damage );
		}
	}
}
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Kandouhada_DmgRecover( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	u32 recoverHP;

	if( common_DmgRecover_Calc(flowWk, pokeID, POKETYPE_MIZU, 4, &recoverHP) )
	{
		common_DmgRecover_Put( flowWk, pokeID, recoverHP );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kansouhada( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_WEATHER_DAMAGE,	handler_Kandouhada_Weather },			// �V��_���[�W�v�Z�n���h��
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Kandouhada_DmgRecover },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
	if( common_DmgRecover_Calc(flowWk, pokeID, POKETYPE_MIZU, 4, &recoverHP) )
	{
		common_DmgRecover_Put( flowWk, pokeID, recoverHP );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tyosui( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Tyosui },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		common_DmgRecover_Put( flowWk, pokeID, recoverHP );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tikuden( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,				handler_Tikuden },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
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
		common_DmgRecover_Put( flowWk, pokeID, recoverHP );
		{
			BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
			BTL_SERVER_RECEPT_RankUpEffect( flowWk, myPos, BPP_AGILITY, 1, TRUE );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_DenkiEngine( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DMG_TO_RECOVER,		handler_DenkiEngine },	// �_���[�W���U�񕜃`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�m�[�K�[�h�v
 */
//------------------------------------------------------------------------------
// �^�C�v�ɂ�閳�����`�F�b�N�n���h��
static void handler_NoGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U�������N�ł���A�K����Ԃɂ���
	u8 atkID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
	const BTL_POKEPARAM* attacker = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, atkID );
	if( !BTL_POKEPARAM_CheckSick(attacker, WAZASICK_MUSTHIT) )
	{
		BPP_SICK_CONT cont;
		BPP_SICKCONT_Set_Turn( &cont, 1 );
		BTL_SVFLOW_RECEPT_AddSick( flowWk, atkID, pokeID, WAZASICK_MUSTHIT, cont, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_NoGuard( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_NOEFFECT_TYPE_CHECK,				handler_NoGuard },	// �^�C�v�ɂ�閳�����`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���������܁v
 */
//------------------------------------------------------------------------------
// �^�C�v�ɂ�閳�����`�F�b�N�n���h��
static void handler_Kimottama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������U�����̎�
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �S�[�X�g�^�C�v�ւ̖�����������
		u8 defID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
		const BTL_POKEPARAM* defender = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, defID );
		if( BTL_POKEPARAM_IsMatchType(defender, POKETYPE_GHOST ) )
		{
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, FALSE );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kimottama( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_NOEFFECT_TYPE_CHECK,				handler_Kimottama },	// �^�C�v�ɂ�閳�����`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ڂ�����v
 */
//------------------------------------------------------------------------------
// �������`�F�b�NLv1�n���h��
static void handler_Bouon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������h�䑤�̎�
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// �T�E���h���U����
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
		if( WAZADATA_IsImage( waza, WAZA_IMG_SOUND ) )
		{
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Bouon( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_NOEFFECT_CHECK_L1,				handler_Bouon },	// �������`�F�b�NLv1�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ӂ䂤�v
 */
//------------------------------------------------------------------------------
// �������`�F�b�NLv1�n���h��
static void handler_Fuyuu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������h�䑤�̎�
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// ��Ɂu���イ��傭�v�������Ă��Ȃ����
		if( BTL_FIELD_CheckState(BTL_FLDSTATE_GRAVITY) )
		{
			// ���߂�^�C�v���U����
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
			if( WAZADATA_GetType(waza) == POKETYPE_JIMEN )
			{
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Fuyuu( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_NOEFFECT_CHECK_L1,				handler_Fuyuu },	// �������`�F�b�NLv1�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ӂ����Ȃ܂���v
 */
//------------------------------------------------------------------------------
// �������`�F�b�NLv2�n���h��
static void handler_FusiginaMamori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������h�䑤��
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// �k�P�j���Ȃ�
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_GetMonsNo(bpp) == MONSNO_NUKENIN )
		{
			// ���ʃo�c�O���ȊO�͖���
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			PokeType waza_type = WAZADATA_GetType( waza );
			BtlTypeAff aff = BTL_CALC_TypeAff( waza_type, BTL_POKEPARAM_GetPokeType(bpp) );
			if( aff <= BTL_TYPEAFF_100 )
			{
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_FusiginaMamori( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_NOEFFECT_CHECK_L2,				handler_FusiginaMamori },	// �������`�F�b�NLv2�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ȃ܂��v
 */
//------------------------------------------------------------------------------
// ���U�o�����ۃ`�F�b�N�n���h��
static void handler_Namake( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		u8 poke_turn = BTL_POKEPARAM_GetTurnCount( bpp );
		u8 sys_turn = BTL_SVFLOW_GetTurnCount( flowWk );
		if( (poke_turn&1) != (sys_turn&1) )
		{
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_NAMAKE );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Namake( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_EXECUTE,				handler_Namake },	// ���U�o�����ۃ`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���߂肯�v
 */
//------------------------------------------------------------------------------
// ���U�o�����ۃ`�F�b�N�n���h��
static void handler_Simerike( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );

	if(	(waza == WAZANO_DAIBAKUHATU)
	||	(waza == WAZANO_ZIBAKU)
	){
		SV_WazaFailCause cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );
		if( cause != SV_WAZAFAIL_NULL )
		{
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_TOKUSEI );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Simerike( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_EXECUTE,				handler_Simerike },	// ���U�o�����ۃ`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���炢�сv
 */
//------------------------------------------------------------------------------
// �������`�F�b�NLv1�n���h��
static void handler_Moraibi_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �������h�䑤�̎�
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// �����U�͖������u���炢�сv����
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		PokeType waza_type = WAZADATA_GetType( waza );
		if( waza_type == POKETYPE_HONOO )
		{
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
			if( work[0] == 0 )
			{
				work[0] = 1;	// �u���炢�сv�����t���O�Ƃ��ė��p
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_MoraibiExe, pokeID );
				BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
			}
		}
	}
}
// �U���͌���n���h��
static void handler_Moraibi_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �u���炢�сv��Ԃ̎�
		if( work[0] )
		{
			// �����U�̈З�1.5�{
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			if( WAZADATA_GetType(waza) == POKETYPE_HONOO )
			{
				u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				pow = BTL_CALC_MulRatio( pow, BTL_CALC_TOK_MORAIBI_POWRATIO );
				BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Moraibi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_NOEFFECT_CHECK_L1,		handler_Moraibi_NoEffect },	// �������`�F�b�NLv1�n���h��
		{ BTL_EVENT_ATTACKER_POWER,				handler_Moraibi_AtkPower },	// �U���͌���n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�i�C�g���A�v
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�ŏI�n���h��
static void handler_Nightmare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	const BTL_POKEPARAM* bpp;
	u8 ePokeID[ BTL_POSIDX_MAX ]; 
	u8 pokeCnt = BTL_SVFLOW_RECEPT_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
	u8 exe_flag = FALSE;
	u8 i;
	for(i=0; i<pokeCnt; ++i)
	{
		bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, ePokeID[i] );
		if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_NEMURI) )
		{
			int dmg = BTL_CALC_QuotMaxHP( bpp, 8 ) * -1;

			if( exe_flag == FALSE ){
				BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
				exe_flag = TRUE;
			}
			BTL_SERVER_RECEPT_HP_Add( flowWk, pokeID, dmg );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Nightmare( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_TURNCHECK_END,				handler_Nightmare },	// �^�[���`�F�b�N�ŏI�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ɂ������v
 */
//------------------------------------------------------------------------------
// �ɂ���`�F�b�N�n���h��
static void handler_Nigeasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Nigeasi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CHECK_ESCAPE,				handler_Nigeasi },	// �ɂ���`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���肶�����v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Arijigoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BTL_SVFLOW_RECEPT_CantEscapeAdd( flowWk, pokeID, BTL_CANTESC_ARIJIGOKU );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Arijigoku( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN,				handler_Arijigoku },	// ����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�����ӂ݁v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Kagefumi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BTL_SVFLOW_RECEPT_CantEscapeAdd( flowWk, pokeID, BTL_CANTESC_KAGEFUMI );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kagefumi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN,				handler_Kagefumi },	// ����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u����傭�v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Jiryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BTL_SVFLOW_RECEPT_CantEscapeAdd( flowWk, pokeID, BTL_CANTESC_JIRYOKU );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Jiryoku( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN,				handler_Jiryoku },	// ����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u������Ԃ�v
 */
//------------------------------------------------------------------------------
// ���U�p�����[�^����n���h��
static void handler_Katayaburi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �P�^�[������������Ƃ������Q
		static const u16 disableTokTable[] = {
			POKETOKUSEI_FUSIGINAMAMORI,	POKETOKUSEI_BOUON,			POKETOKUSEI_FUYUU,
			POKETOKUSEI_SUNAGAKURE,			POKETOKUSEI_YUKIGAKURE,	POKETOKUSEI_TYOSUI,
			POKETOKUSEI_KABUTOAAMAA,		POKETOKUSEI_HIRAISIN,		POKETOKUSEI_YOBIMIZU,
			POKETOKUSEI_SHERUAAMAA,			POKETOKUSEI_TENNEN,			POKETOKUSEI_HIRAISIN,
			POKETOKUSEI_YOBIMIZU,				POKETOKUSEI_KYUUBAN,		POKETOKUSEI_TANJUN,
			POKETOKUSEI_TIDORIASI,			POKETOKUSEI_HAADOROKKU,	POKETOKUSEI_FIRUTAA,
			POKETOKUSEI_MORAIBI,				POKETOKUSEI_DENKIENJIN,	POKETOKUSEI_NENCHAKU,
			POKETOKUSEI_FUSIGINAUROKO,	POKETOKUSEI_ATUISIBOU,	POKETOKUSEI_TAINETU,
			POKETOKUSEI_SIROIKEMURI,		POKETOKUSEI_KURIABODY,	POKETOKUSEI_SURUDOIME,
			POKETOKUSEI_KAIRIKIBASAMI,	POKETOKUSEI_SEISINRYOKU,POKETOKUSEI_RINPUN,
			POKETOKUSEI_GANJOU,					POKETOKUSEI_SIMERIKE,		POKETOKUSEI_DONKAN,
			POKETOKUSEI_JUUNAN,					POKETOKUSEI_MAIPEESU,		POKETOKUSEI_MIZUNOBEERU,
			POKETOKUSEI_HONOONOKARADA,	POKETOKUSEI_RIIFUGAADO,	POKETOKUSEI_FUMIN,
			POKETOKUSEI_YARUKI,					POKETOKUSEI_MENEKI,			
		};
		u32 i;
		for(i=0; i<NELEMS(disableTokTable); ++i)
		{
			BTL_EVENT_SleepFactor( BTL_EVENT_FACTOR_TOKUSEI, disableTokTable[i] );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Katayaburi( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_PARAM,				handler_Katayaburi },	// ���U�p�����[�^����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ă񂫂�v
 */
//------------------------------------------------------------------------------
// �|�P����n���h��
static void handler_Tenkiya_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BtlWeather weather = BTL_FIELD_GetWeather();
		common_TenkiFormChange( flowWk, pokeID, weather );
	}
}
// �V��ω��n���h��
static void handler_Tenkiya_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		BtlWeather weather = BTL_EVENTVAR_GetValue( BTL_EVAR_WEATHER );
		common_TenkiFormChange( flowWk, pokeID, weather );
	}
}
static void common_TenkiFormChange( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather )
{
	const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
	if( BTL_POKEPARAM_GetMonsNo(bpp) == MONSNO_POWARUN )
	{
		u8 form_now, form_next;

		form_now = BTL_POKEPARAM_GetValue( bpp, BPP_FORM );
		switch( weather ){
		case BTL_WEATHER_NONE:	form_next = FORMNO_POWARUN_NORMAL; break;
		case BTL_WEATHER_SHINE:	form_next = FORMNO_POWARUN_SUN; break;
		case BTL_WEATHER_RAIN:	form_next = FORMNO_POWARUN_RAIN; break;
		case BTL_WEATHER_SNOW:	form_next = FORMNO_POWARUN_SNOW; break;
		default:
			form_next = form_now;
		}

		if( form_next != form_now )
		{
			BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
			BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_MoraibiExe, pokeID );
			BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tenkiya( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MEMBER_IN,						handler_Tenkiya_MemberIn 	},	// �|�P����n���h��
		{ BTL_EVENT_WEATHER_CHANGE_AFTER,	handler_Tenkiya_Weather		},	// �V��ω���n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u��т݂��v
 */
//------------------------------------------------------------------------------
// ���U�^�[�Q�b�g����n���h��
static void handler_Yobimizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_WazaTargetChangeToMe( flowWk, pokeID, POKETYPE_MIZU );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Yobimizu( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DECIDE_TARGET,		handler_Yobimizu 	},	// ���U�^�[�Q�b�g����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ђ炢����v
 */
//------------------------------------------------------------------------------
// ���U�^�[�Q�b�g����n���h��
static void handler_Hiraisin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_WazaTargetChangeToMe( flowWk, pokeID, POKETYPE_DENKI );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Hiraisin( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DECIDE_TARGET,		handler_Hiraisin 	},	// ���U�^�[�Q�b�g����n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}
//--------------------------------------------------------------------------
/**
 * �Ƃ��Ă��^�C�v�̃��U�̑Ώۂ������I�Ɏ����ɂ���Ƃ������̋��ʏ���
 *
 * @param   flowWk		
 * @param   pokeID		
 * @param   wazaType		
 *
 */
//--------------------------------------------------------------------------
static void common_WazaTargetChangeToMe( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
	{
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == wazaType )
		{
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, pokeID );
		}
	}
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���イ�΂�v
 */
//------------------------------------------------------------------------------
// �ӂ��Ƃ΂��`�F�b�N�n���h��
static void handler_Kyuuban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kyuuban( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CHECK_PUSHOUT,		handler_Kyuuban 	},	// �ӂ��Ƃ΂��`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�w�h�������v
 */
//------------------------------------------------------------------------------
// �h���C���ʌv�Z�n���h��
static void handler_HedoroEki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		int vol = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
		vol *= -1;
		BTL_EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, vol );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_HedoroEki( u16 pri, u16 tokID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_CALC_DRAIN_VOLUME,		handler_HedoroEki 	},	// �h���C���ʌv�Z�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

