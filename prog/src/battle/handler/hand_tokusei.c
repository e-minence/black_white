//=============================================================================================
/**
 * @file  hand_tokusei.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[[�Ƃ�����]�ǉ�����
 * @author  taya
 *
 * @date  2008.11.11  �쐬
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




/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL Tokusei_IsExePer( BTL_SVFLOW_WORK* flowWk, u8 per );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Ikaku( u32* numElems );
static void handler_Ikaku_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Seisinryoku( u32* numElems );
static void handler_Seisinryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Fukutsuno( u32* numElems );
static void handler_FukutsunoKokoro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_AtuiSibou( u32* numElems );
static void handler_AtuiSibou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tikaramoti( u32* numElems );
static void handler_Tikaramoti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Suisui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Suisui( u32* numElems );
static void handler_Youryokuso( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Youryokuso( u32* numElems );
static void handler_Hayaasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hayaasi( u32* numElems );
static void handler_Tidoriasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tidoriasi( u32* numElems );
static void handler_Harikiri_HitRatio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Harikiri_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Harikiri( u32* numElems );
static void handler_Atodasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Atodasi( u32* numElems );
static void handler_SlowStart_Agility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SlowStart_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SlowStart( u32* numElems );
static void handler_Fukugan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Fukugan( u32* numElems );
static void handler_Sunagakure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sunagakure( u32* numElems );
static void handler_Yukigakure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yukigakure( u32* numElems );
static void handler_Iromegane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Iromegane( u32* numElems );
static void handler_HardRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_HardRock( u32* numElems );
static void handler_Sniper( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sniper( u32* numElems );
static void handler_Kasoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kasoku( u32* numElems );
static void handler_Tekiouryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tekiouryoku( u32* numElems );
static void handler_Mouka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Mouka( u32* numElems );
static void handler_Gekiryu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Gekiryu( u32* numElems );
static void handler_Sinryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sinryoku( u32* numElems );
static void handler_MusinoSirase( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MusinoSirase( u32* numElems );
static void common_hpborder_powerup( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType );
static void handler_Konjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Konjou( u32* numElems );
static void handler_Plus( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Plus( u32* numElems );
static void handler_Minus( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Minus( u32* numElems );
static void handler_FlowerGift_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_FlowerGift_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_FlowerGift( u32* numElems );
static void handler_Tousousin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tousousin( u32* numElems );
static void handler_Technician( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Technician( u32* numElems );
static void handler_TetunoKobusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_TetunoKobusi( u32* numElems );
static void handler_Sutemi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sutemi( u32* numElems );
static void handler_FusiginaUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_FusiginaUroko( u32* numElems );
static void handler_SkillLink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SkillLink( u32* numElems );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_KairikiBasami( u32* numElems );
static void handler_KairikiBasami_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KairikiBasami_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Surudoime( u32* numElems );
static void handler_Surudoime_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Surudoime_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_ClearBody( u32* numElems );
static void handler_ClearBody_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ClearBody_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_RankDownGuard_Check( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaRankEffect rankType );
static void common_RankDownGuard_Fixed( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, u16 strID );
static void handler_Tanjun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tanjun( u32* numElems );
static void handler_ReafGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_ReafGuard( u32* numElems );
static void handler_Juunan_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Juunan_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Juunan( u32* numElems );
static void handler_Fumin_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Fumin_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Fumin( u32* numElems );
static void handler_MagumaNoYoroi_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagumaNoYoroi_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MagumaNoYoroi( u32* numElems );
static void handler_Meneki_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Meneki_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Meneki( u32* numElems );
static void handler_MyPace_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MyPace_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MyPace( u32* numElems );
static void handler_MizuNoBale_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MizuNoBale_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MizuNoBale( u32* numElems );
static BOOL common_GuardWazaSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick guardSick );
static void handler_AddSickFailCommon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_Swap_CureSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick );
static void handler_Donkan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Donkan( u32* numElems );
static void handler_Amefurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Amefurasi( u32* numElems );
static void handler_Hideri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hideri( u32* numElems );
static void handler_Sunaokosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sunaokosi( u32* numElems );
static void handler_Yukifurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yukifurasi( u32* numElems );
static void common_weather_change( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather );
static void handler_AirLock_ChangeWeather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_AirLock( u32* numElems );
static void handler_IceBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_IcoBody( u32* numElems );
static void handler_AmeukeZara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_AmeukeZara( u32* numElems );
static void common_weather_recover( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather );
static void handler_SunPower_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SunPower_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SunPower( u32* numElems );
static void handler_Rinpun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Rinpun_Shrink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Rinpun( u32* numElems );
static void handler_TennoMegumi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_TennoMegumi( u32* numElems );
static void handler_UruoiBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_UruoiBody( u32* numElems );
static void handler_Dappi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Dappi( u32* numElems );
static void handler_PoisonHeal( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_PoisonHeal( u32* numElems );
static void handler_KabutoArmor( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_KabutoArmor( u32* numElems );
static void handler_Kyouun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kyouun( u32* numElems );
static void handler_IkarinoTubo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_IkarinoTubo( u32* numElems );
static void handler_DokunoToge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_DokunoToge( u32* numElems );
static void handler_Seidenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Seidenki( u32* numElems );
static void handler_HonoNoKarada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_HonoNoKarada( u32* numElems );
static void handler_MeromeroBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MeromeroBody( u32* numElems );
static void handler_Housi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Housi( u32* numElems );
static void common_touchAddSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick, BPP_SICK_CONT sickCont, u8 per );
static void handler_Samehada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Samehada( u32* numElems );
static void handler_Yuubaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yuubaku( u32* numElems );
static void handler_Hensyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hensyoku( u32* numElems );
static void handler_Syncro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Syncro( u32* numElems );
static void handler_Isiatama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Isiatama( u32* numElems );
static void handler_NormalSkin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_NormalSkin( u32* numElems );
static void handler_MultiType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MultiType( u32* numElems );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Trace( u32* numElems );
static void handler_Trace( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenKaifuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SizenKaifuku( u32* numElems );
static void handler_Download( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_DownLoad( u32* numElems );
static void handler_Yotimu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yotimu( u32* numElems );
static void handler_KikenYoti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL _check_kikenyoti_poke( const BTL_POKEPARAM* bppUser, const BTL_POKEPARAM* bppEnemy );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_KikenYoti( u32* numElems );
static void handler_Omitoosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Omitoosi( u32* numElems );
static void handler_Ganjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Ganjou( u32* numElems );
static void handler_Tennen_hitRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tennen_AtkRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tennen_DefRank( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tennen( u32* numElems );
static void handler_Tainetsu_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tainetsu_SickDmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tainetu( u32* numElems );
static void common_DmgToRecover_Check( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType matchType );
static void common_DmgToRecover_Fix( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, u8 denomHP );
static void handler_Kansouhada_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kansouhada_DmgRecover( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kansouhada_DmgRecoverFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kansouhada_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kansouhada( u32* numElems );
static void handler_Tyosui_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tyosui_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tyosui( u32* numElems );
static void handler_Tikuden_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tikuden_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tikuden( u32* numElems );
static void handler_DenkiEngine_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DenkiEngine_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_DenkiEngine( u32* numElems );
static void handler_NoGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_NoGuard( u32* numElems );
static void handler_Kimottama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kimottama( u32* numElems );
static void handler_Bouon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Bouon( u32* numElems );
static void handler_Fuyuu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Fuyuu( u32* numElems );
static void handler_FusiginaMamori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_FusiginaMamori( u32* numElems );
static void handler_Namake( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Namake( u32* numElems );
static void handler_Simerike( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Simerike( u32* numElems );
static void handler_Moraibi_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Moraibi_DmgRecoverFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Moraibi_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Moraibi( u32* numElems );
static void handler_Nightmare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Nightmare( u32* numElems );
static void handler_Nigeasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Nigeasi( u32* numElems );
static void handler_Arijigoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Arijigoku( u32* numElems );
static void handler_Kagefumi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kagefumi( u32* numElems );
static void handler_Jiryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Jiryoku( u32* numElems );
static void handler_Katayaburi_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL handler_Katayaburi_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );
static void handler_Katayaburi_Start( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Katayaburi_End( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Katayaburi( u32* numElems );
static void handler_Tenkiya_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tenkiya_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_TenkiFormChange( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tenkiya( u32* numElems );
static void handler_Yobimizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yobimizu( u32* numElems );
static void handler_Hiraisin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hiraisin( u32* numElems );
static void common_WazaTargetChangeToMe( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType );
static void handler_Kyuuban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kyuuban( u32* numElems );
static void handler_HedoroEki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_HedoroEki( u32* numElems );
static BOOL handler_Bukiyou_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Bukiyou( u32* numElems );
static void handler_Nenchaku_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nenchaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Nenchaku( u32* numElems );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Pressure( u32* numElems );
static void handler_Pressure_MemberIN( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Pressure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MagicGuard( u32* numElems );
static void handler_MagicGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_WaruiTeguse( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_WaruiTeguse( u32* numElems );
static void handler_NorowareBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_NorowareBody( u32* numElems );
static void handler_KudakeruYoroi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_KudakeruYoroi( u32* numElems );
static void handler_Tikarazuku_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tikarazuku_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tikarazuku( u32* numElems );
static BOOL IsTikarazukuEffecive( WazaID waza );
static void handler_Makenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Makenki( u32* numElems );
static void handler_Yowaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yowaki( u32* numElems );
static void handler_MultiScale( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MultiScale( u32* numElems );
static void handler_NakamaIsiki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_NakamaIsiki( u32* numElems );
static void handler_Moraterapi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Moraterapi( u32* numElems );
static void handler_Dokubousou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Dokubousou( u32* numElems );
static void handler_Netubousou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Netubousou( u32* numElems );
static void handler_AunNoIki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_AunNoIki( u32* numElems );
static void handler_Murakke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* sys_work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Murakke( u32* numElems );
static void handler_Boujin_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Boujin( u32* numElems );
static void handler_Dokusyu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Dokusyu( u32* numElems );
static void handler_SaiseiRyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SaiseiRyoku( u32* numElems );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hatomune( u32* numElems );
static void handler_Hatomune_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hatomune_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sunakaki( u32* numElems );
static void handler_Sunakaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MilacreSkin( u32* numElems );
static void handler_MilacreSkin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sinuti( u32* numElems );
static void handler_Sinuti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SunanoTikara( u32* numElems );
static void handler_SunanoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL handler_Surinuke_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );
static void handler_Surinuke_Start( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Surinuke_End( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Surinuke( u32* numElems );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_JisinKajou( u32* numElems );
static void handler_JisinKajou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SeiginoKokoro( u32* numElems );
static void handler_SeiginoKokoro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Bibiri( u32* numElems );
static void handler_Bibiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Miira( u32* numElems );
static void handler_Miira( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sousyoku_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sousyoku_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sousyoku( u32* numElems );
static void handler_ItazuraGokoro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_ItazuraGokoro( u32* numElems );
static void handler_MagicMirror_CheckRob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagicMirror_Reflect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MagicMirror( u32* numElems );
static void handler_Syuukaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Syuukaku( u32* numElems );
static void handler_Amanojaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Amanojaku( u32* numElems );
static void handler_Kinchoukan_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kinchoukan_CheckItemEquip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kinchoukan_MemberOutFixed( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kinchoukan_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kinchoukan_ChangeTok( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_KinchoukanOff( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kinchoukan( u32* numElems );
static void handler_Hensin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hensin( u32* numElems );
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Illusion( u32* numElems );
static void handler_Illusion_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Illusion_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Illusion_ChangeTok( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_IllusionBreak( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID );


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
  typedef const BtlEventHandlerTable* (*pTokuseiEventAddFunc)( u32* );

  static const struct {
    PokeTokusei           tokusei;
    pTokuseiEventAddFunc  func;
  }funcTbl[] = {
    { POKETOKUSEI_IKAKU,            HAND_TOK_ADD_Ikaku         },
    { POKETOKUSEI_KURIABODY,        HAND_TOK_ADD_ClearBody     },
    { POKETOKUSEI_SIROIKEMURI,      HAND_TOK_ADD_ClearBody     }, // ���낢���ނ�=�N���A�{�f�B�Ɠ���
    { POKETOKUSEI_SEISINRYOKU,      HAND_TOK_ADD_Seisinryoku   },
    { POKETOKUSEI_FUKUTUNOKOKORO,   HAND_TOK_ADD_Fukutsuno     },
    { POKETOKUSEI_ATUISIBOU,        HAND_TOK_ADD_AtuiSibou     },
    { POKETOKUSEI_KAIRIKIBASAMI,    HAND_TOK_ADD_KairikiBasami },
    { POKETOKUSEI_TIKARAMOTI,       HAND_TOK_ADD_Tikaramoti    },
    { POKETOKUSEI_YOGAPAWAA,        HAND_TOK_ADD_Tikaramoti    }, // ���K�p���[ = ����������Ɠ���
    { POKETOKUSEI_IROMEGANE,        HAND_TOK_ADD_Iromegane     },
    { POKETOKUSEI_KASOKU,           HAND_TOK_ADD_Kasoku        },
    { POKETOKUSEI_MOUKA,            HAND_TOK_ADD_Mouka         },
    { POKETOKUSEI_GEKIRYUU,         HAND_TOK_ADD_Gekiryu       },
    { POKETOKUSEI_SINRYOKU,         HAND_TOK_ADD_Sinryoku      },
    { POKETOKUSEI_MUSINOSIRASE,     HAND_TOK_ADD_MusinoSirase  },
    { POKETOKUSEI_KONJOU,           HAND_TOK_ADD_Konjou        },
    { POKETOKUSEI_SUKIRURINKU,      HAND_TOK_ADD_SkillLink     },
    { POKETOKUSEI_SURUDOIME,        HAND_TOK_ADD_Surudoime     },
    { POKETOKUSEI_TANJUN,           HAND_TOK_ADD_Tanjun        },
    { POKETOKUSEI_HAADOROKKU,       HAND_TOK_ADD_HardRock      },
    { POKETOKUSEI_FIRUTAA,          HAND_TOK_ADD_HardRock      }, // �t�B���^�[ = �n�[�h���b�N�Ɠ���
    { POKETOKUSEI_FUSIGINAUROKO,    HAND_TOK_ADD_FusiginaUroko },
    { POKETOKUSEI_TOUSOUSIN,        HAND_TOK_ADD_Tousousin     },
    { POKETOKUSEI_RIIFUGAADO,       HAND_TOK_ADD_ReafGuard     },
    { POKETOKUSEI_AMEFURASI,        HAND_TOK_ADD_Amefurasi     },
    { POKETOKUSEI_HIDERI,           HAND_TOK_ADD_Hideri        },
    { POKETOKUSEI_SUNAOKOSI,        HAND_TOK_ADD_Sunaokosi     },
    { POKETOKUSEI_YUKIFURASI,       HAND_TOK_ADD_Yukifurasi    },
    { POKETOKUSEI_EAROKKU,          HAND_TOK_ADD_AirLock       },
    { POKETOKUSEI_NOOTENKI,         HAND_TOK_ADD_AirLock       }, // �m�[�Ă� = �G�A���b�N�Ɠ���
    { POKETOKUSEI_TEKUNISYAN,       HAND_TOK_ADD_Technician    },
    { POKETOKUSEI_DONKAN,           HAND_TOK_ADD_Donkan        },
    { POKETOKUSEI_URUOIBODY,        HAND_TOK_ADD_UruoiBody     },
    { POKETOKUSEI_POIZUNHIIRU,      HAND_TOK_ADD_PoisonHeal    },
    { POKETOKUSEI_AISUBODY,         HAND_TOK_ADD_IcoBody       },
    { POKETOKUSEI_AMEUKEZARA,       HAND_TOK_ADD_AmeukeZara    },
    { POKETOKUSEI_RINPUN,           HAND_TOK_ADD_Rinpun        },
    { POKETOKUSEI_TEKIOURYOKU,      HAND_TOK_ADD_Tekiouryoku   },
    { POKETOKUSEI_TENNOMEGUMI,      HAND_TOK_ADD_TennoMegumi   },
    { POKETOKUSEI_SANPAWAA,         HAND_TOK_ADD_SunPower      },
    { POKETOKUSEI_SUISUI,           HAND_TOK_ADD_Suisui        },
    { POKETOKUSEI_YOURYOKUSO,       HAND_TOK_ADD_Youryokuso    },
    { POKETOKUSEI_DAPPI,            HAND_TOK_ADD_Dappi         },
    { POKETOKUSEI_TIDORIASI,        HAND_TOK_ADD_Tidoriasi     },
    { POKETOKUSEI_HAYAASI,          HAND_TOK_ADD_Hayaasi       },
    { POKETOKUSEI_HARIKIRI,         HAND_TOK_ADD_Harikiri      },
    { POKETOKUSEI_KABUTOAAMAA,      HAND_TOK_ADD_KabutoArmor   },
    { POKETOKUSEI_SHERUAAMAA,       HAND_TOK_ADD_KabutoArmor   }, // �V�F���A�[�}�[=�J�u�g�A�[�}�[�Ɠ���
    { POKETOKUSEI_KYOUUN,           HAND_TOK_ADD_Kyouun        },
    { POKETOKUSEI_IKARINOTUBO,      HAND_TOK_ADD_IkarinoTubo   },
    { POKETOKUSEI_SUNAIPAA,         HAND_TOK_ADD_Sniper        },
    { POKETOKUSEI_TETUNOKOBUSI,     HAND_TOK_ADD_TetunoKobusi  },
    { POKETOKUSEI_FUKUGAN,          HAND_TOK_ADD_Fukugan       },
    { POKETOKUSEI_ISIATAMA,         HAND_TOK_ADD_Isiatama      },
    { POKETOKUSEI_SUTEMI ,          HAND_TOK_ADD_Sutemi        },
    { POKETOKUSEI_SEIDENKI ,        HAND_TOK_ADD_Seidenki      },
    { POKETOKUSEI_DOKUNOTOGE,       HAND_TOK_ADD_DokunoToge    },
    { POKETOKUSEI_HONOONOKARADA,    HAND_TOK_ADD_HonoNoKarada  },
    { POKETOKUSEI_HOUSI,            HAND_TOK_ADD_Housi         },
    { POKETOKUSEI_PURASU,           HAND_TOK_ADD_Plus          },
    { POKETOKUSEI_MAINASU,          HAND_TOK_ADD_Minus         },
    { POKETOKUSEI_MEROMEROBODY,     HAND_TOK_ADD_MeromeroBody  },
    { POKETOKUSEI_SUNAGAKURE ,      HAND_TOK_ADD_Sunagakure    },
    { POKETOKUSEI_YUKIGAKURE ,      HAND_TOK_ADD_Yukigakure    },
    { POKETOKUSEI_TOREESU,          HAND_TOK_ADD_Trace         },
    { POKETOKUSEI_NOOMARUSUKIN,     HAND_TOK_ADD_NormalSkin    },
    { POKETOKUSEI_SAMEHADA,         HAND_TOK_ADD_Samehada      },
    { POKETOKUSEI_SIZENKAIFUKU,     HAND_TOK_ADD_SizenKaifuku  },
    { POKETOKUSEI_SINKURO,          HAND_TOK_ADD_Syncro        },
    { POKETOKUSEI_DAUNROODO,        HAND_TOK_ADD_DownLoad      },
    { POKETOKUSEI_GANJOU,           HAND_TOK_ADD_Ganjou        },
    { POKETOKUSEI_TAINETU,          HAND_TOK_ADD_Tainetu       },
    { POKETOKUSEI_TENNEN,           HAND_TOK_ADD_Tennen        },
    { POKETOKUSEI_KANSOUHADA,       HAND_TOK_ADD_Kansouhada    },
    { POKETOKUSEI_TIKUDEN,          HAND_TOK_ADD_Tikuden       },
    { POKETOKUSEI_TYOSUI,           HAND_TOK_ADD_Tyosui        },
    { POKETOKUSEI_DENKIENJIN,       HAND_TOK_ADD_DenkiEngine   },
    { POKETOKUSEI_JUUNAN,           HAND_TOK_ADD_Juunan        },
    { POKETOKUSEI_FUMIN,            HAND_TOK_ADD_Fumin         },
    { POKETOKUSEI_YARUKI,           HAND_TOK_ADD_Fumin         }, // ��邫=�ӂ݂�Ɠ���
    { POKETOKUSEI_MAIPEESU,         HAND_TOK_ADD_MyPace        },
    { POKETOKUSEI_MAGUMANOYOROI,    HAND_TOK_ADD_MagumaNoYoroi },
    { POKETOKUSEI_MIZUNOBEERU,      HAND_TOK_ADD_MizuNoBale    },
    { POKETOKUSEI_MENEKI,           HAND_TOK_ADD_Meneki        },
    { POKETOKUSEI_NOOGAADO,         HAND_TOK_ADD_NoGuard       },
    { POKETOKUSEI_KIMOTTAMA,        HAND_TOK_ADD_Kimottama     },
    { POKETOKUSEI_BOUON,            HAND_TOK_ADD_Bouon         },
    { POKETOKUSEI_FUYUU,            HAND_TOK_ADD_Fuyuu         },
    { POKETOKUSEI_FURAWAAGIFUTO,    HAND_TOK_ADD_FlowerGift    },
    { POKETOKUSEI_MORAIBI,          HAND_TOK_ADD_Moraibi       },
    { POKETOKUSEI_YOTIMU,           HAND_TOK_ADD_Yotimu        },
    { POKETOKUSEI_KIKENYOTI,        HAND_TOK_ADD_KikenYoti     },
    { POKETOKUSEI_OMITOOSI,         HAND_TOK_ADD_Omitoosi      },
    { POKETOKUSEI_YUUBAKU,          HAND_TOK_ADD_Yuubaku       },
    { POKETOKUSEI_NIGEASI,          HAND_TOK_ADD_Nigeasi       },
    { POKETOKUSEI_HENSYOKU,         HAND_TOK_ADD_Hensyoku      },
    { POKETOKUSEI_KATAYABURI,       HAND_TOK_ADD_Katayaburi    },
    { POKETOKUSEI_NAMAKE,           HAND_TOK_ADD_Namake        },
    { POKETOKUSEI_HIRAISIN,         HAND_TOK_ADD_Hiraisin      },
    { POKETOKUSEI_YOBIMIZU,         HAND_TOK_ADD_Yobimizu      },
    { POKETOKUSEI_SUROOSUTAATO,     HAND_TOK_ADD_SlowStart     },
    { POKETOKUSEI_ARIJIGOKU,        HAND_TOK_ADD_Arijigoku     },
    { POKETOKUSEI_KAGEFUMI,         HAND_TOK_ADD_Kagefumi      },
    { POKETOKUSEI_JIRYOKU,          HAND_TOK_ADD_Jiryoku       },
    { POKETOKUSEI_SIMERIKE,         HAND_TOK_ADD_Simerike      },
    { POKETOKUSEI_MARUTITAIPU,      HAND_TOK_ADD_MultiType     },
    { POKETOKUSEI_FUSIGINAMAMORI,   HAND_TOK_ADD_FusiginaMamori},
    { POKETOKUSEI_ATODASI,          HAND_TOK_ADD_Atodasi       },
    { POKETOKUSEI_TENKIYA,          HAND_TOK_ADD_Tenkiya       },
    { POKETOKUSEI_KYUUBAN,          HAND_TOK_ADD_Kyuuban       },
    { POKETOKUSEI_HEDOROEKI,        HAND_TOK_ADD_HedoroEki     },
    { POKETOKUSEI_BUKIYOU,          HAND_TOK_ADD_Bukiyou       },
    { POKETOKUSEI_NENCHAKU,         HAND_TOK_ADD_Nenchaku      },
    { POKETOKUSEI_PURESSYAA,        HAND_TOK_ADD_Pressure      },
    { POKETOKUSEI_MAJIKKUGAADO,     HAND_TOK_ADD_MagicGuard    },
    { POKETOKUSEI_NAITOMEA,         HAND_TOK_ADD_Nightmare     },

    { POKETOKUSEI_WARUITEGUSE,      HAND_TOK_ADD_WaruiTeguse   },
    { POKETOKUSEI_TIKARAZUKU,       HAND_TOK_ADD_Tikarazuku    },
    { POKETOKUSEI_MAKENKI,          HAND_TOK_ADD_Makenki       },
    { POKETOKUSEI_YOWAKI,           HAND_TOK_ADD_Yowaki        },
    { POKETOKUSEI_MARUTISUKEIRU,    HAND_TOK_ADD_MultiScale    },
    { POKETOKUSEI_AMANOJAKU,        HAND_TOK_ADD_Amanojaku     }, // ���܂̂��Ⴍ
    { POKETOKUSEI_KINCHOUKAN,       HAND_TOK_ADD_Kinchoukan    }, // ���񂿂傤����
    { POKETOKUSEI_NOROWAREBODY,     HAND_TOK_ADD_NorowareBody  }, // �̂���{�f�B
    { POKETOKUSEI_MORATERAPII,      HAND_TOK_ADD_Moraterapi    }, // �����e���s�[
    { POKETOKUSEI_NAKAMAISIKI,      HAND_TOK_ADD_NakamaIsiki   }, // �Ȃ��܂�����
    { POKETOKUSEI_KUDAKERUYOROI,    HAND_TOK_ADD_KudakeruYoroi }, // ���������낢
    { POKETOKUSEI_DOKUBOUSOU,       HAND_TOK_ADD_Dokubousou    }, // �ǂ��ڂ�����
    { POKETOKUSEI_NETUBOUSOU,       HAND_TOK_ADD_Netubousou    }, // �˂ڂ�����
    { POKETOKUSEI_SYUUKAKU,         HAND_TOK_ADD_Syuukaku      }, // ���イ����
    { POKETOKUSEI_AUNNOIKI,         HAND_TOK_ADD_AunNoIki      }, // ������̂���
    { POKETOKUSEI_MURAKKE,          HAND_TOK_ADD_Murakke       }, // ��������
    { POKETOKUSEI_BOUJIN,           HAND_TOK_ADD_Boujin        }, // �ڂ�����
    { POKETOKUSEI_DOKUSYU,          HAND_TOK_ADD_Dokusyu       }, // �ǂ�����
    { POKETOKUSEI_SAISEIRYOKU,      HAND_TOK_ADD_SaiseiRyoku   }, // ����������傭
    { POKETOKUSEI_HATOMUNE,         HAND_TOK_ADD_Hatomune      }, // �͂Ƃނ�
    { POKETOKUSEI_SUNAKAKI,         HAND_TOK_ADD_Sunakaki      }, // ���Ȃ���
    { POKETOKUSEI_MIRAKURUSUKIN,    HAND_TOK_ADD_MilacreSkin   }, // �~���N���X�L��
    { POKETOKUSEI_SINUTI,           HAND_TOK_ADD_Sinuti        }, // ���񂤂�
    { POKETOKUSEI_IRYUUJON,         HAND_TOK_ADD_Illusion      }, // �C�����[�W����
    { POKETOKUSEI_HENSIN,           HAND_TOK_ADD_Hensin        }, // �ւ񂵂�
    { POKETOKUSEI_SURINUKE,         HAND_TOK_ADD_Surinuke      }, // ����ʂ�
    { POKETOKUSEI_MIIRA,            HAND_TOK_ADD_Miira         }, // �~�C��
    { POKETOKUSEI_JISINKAJOU,       HAND_TOK_ADD_JisinKajou    }, // �����񂩂��傤
    { POKETOKUSEI_SEIGINOKOKORO,    HAND_TOK_ADD_SeiginoKokoro }, // �������̂�����
    { POKETOKUSEI_BIBIRI,           HAND_TOK_ADD_Bibiri        }, // �тт�
    { POKETOKUSEI_MAJIKKUMIRAA,     HAND_TOK_ADD_MagicMirror   }, // �}�W�b�N�~���[
    { POKETOKUSEI_SOUSYOKU,         HAND_TOK_ADD_Sousyoku      }, // �������傭
    { POKETOKUSEI_ITAZURAGOKORO,    HAND_TOK_ADD_ItazuraGokoro }, // �������炲����
    { POKETOKUSEI_SUNANOTIKARA,     HAND_TOK_ADD_SunanoTikara  }, // ���Ȃ̂�����
  };

//  if( !BPP_CheckSick(pp, WAZASICK_IEKI) )
  {
    u16 tokusei = BPP_GetValue( pp, BPP_TOKUSEI );
    int i;

    for(i=0; i<NELEMS(funcTbl); i++)
    {
      if( funcTbl[i].tokusei == tokusei )
      {
        u16 agi = BPP_GetValue( pp, BPP_AGILITY );
        u8 pokeID = BPP_GetID( pp );
        u32 numHandlers;
        const BtlEventHandlerTable* handlerTable;

        handlerTable = funcTbl[i].func( &numHandlers );

        return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokusei, agi, pokeID, handlerTable, numHandlers );
      }
    }
    BTL_Printf("�Ƃ�����(%d)�̓n���h�����p�ӂ���Ă��Ȃ�\n", tokusei);
  }
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
  u8 pokeID = BPP_GetID( pp );

  while( (factor = BTL_EVENT_SeekFactor(BTL_EVENT_FACTOR_TOKUSEI, pokeID)) != NULL )
  {
    u16 tokuseiID = BTL_EVENT_FACTOR_GetSubID( factor );
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

  ID_1 = BPP_GetID( pp1 );
  ID_2 = BPP_GetID( pp2 );
  factor1 = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_TOKUSEI, ID_1 );
  factor2 = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_TOKUSEI, ID_2 );

  if( factor1 && factor2 )
  {
    u16 pri_1 = BPP_GetValue( pp1, BPP_AGILITY );
    u16 pri_2 = BPP_GetValue( pp2, BPP_AGILITY );

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

//=============================================================================================
/**
 * �����_�������Ƃ������̋��ʃ`�F�b�N���[�`��
 */
//=============================================================================================
static BOOL Tokusei_IsExePer( BTL_SVFLOW_WORK* flowWk, u8 per )
{
  if( BTL_CALC_IsOccurPer(per) ){
    return TRUE;
  }
  if( BTL_SVFTOOL_GetDebugFlag(flowWk, BTL_DEBUGFLAG_MUST_TOKUSEI) ){
    return TRUE;
  }
  return FALSE;
}

//static +BTL_EVENT_FACTOR\* +.*\(.*\)
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�������v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Ikaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN, handler_Ikaku_MemberIn },
  };

  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �|�P�������o�ꂵ�����̃n���h��
static void handler_Ikaku_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

      param->rankType = BPP_ATTACK;
      param->rankVolume = -1;
      param->fAlmost = TRUE;

      {
        BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokeID( flowWk, pokeID );
        BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );

        param->poke_cnt = BTL_SVFTOOL_ExpandPokeID( flowWk, expos, param->pokeID );
      }
    }
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���������傭�v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Seisinryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SHRINK_CHECK, handler_Seisinryoku },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// BTL_EVENT_SHRINK_CHECK:�Ђ�ݔ����`�F�b�N
static void handler_Seisinryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������Ђ�܂����Ώ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �������s�t���O���n�m
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ӂ��̂�����v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Fukutsuno( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FAIL, handler_FukutsunoKokoro },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// ���U�o�����s�m��
static void handler_FukutsunoKokoro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ���U���s���R=�Ђ��, �Ώۃ|�P=�����Ŕ���
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_CAUSE) == SV_WAZAFAIL_SHRINK)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  ){
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    BTL_HANDEX_PARAM_HEADER* header;

    header = (BTL_HANDEX_PARAM_HEADER*)param;
    header->tokwin_flag = TRUE;

    param->rankType = BPP_AGILITY;
    param->rankVolume = 1;
    param->fAlmost = FALSE;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�������ڂ��v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_AtuiSibou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_AtuiSibou },
  };

  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �U�����̍U���l����
static void handler_AtuiSibou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �_���[�W�Ώۂ�����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    PokeType  type = WAZADATA_GetType( waza );
    // ���U�^�C�v���X����
    if( (type == POKETYPE_KOORI) || (type == POKETYPE_HONOO) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u����������v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tikaramoti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Tikaramoti },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �U�����̍U���l����
static void handler_Tikaramoti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �U������̂�����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    // �_���[�W�^�C�v������
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���������v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_Suisui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // �J�̂Ƃ��A���΂₳�Q�{
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_RAIN )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Suisui( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_Suisui },   // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�悤��傭���v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_Youryokuso( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // �͂�̎��A���΂₳�Q�{
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SHINE )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Youryokuso( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_Youryokuso },   // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�͂₠���v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_Hayaasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ��Ԉُ�̎��A���΂₳1.5�{���}�q�ɂ�錸���𖳎�����
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetPokeSick(bpp) != POKESICK_NULL )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_AGILITY, BTL_CALC_TOK_HAYAASI_AGIRATIO );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, FALSE );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hayaasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_Hayaasi },    // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���ǂ肠���v
 */
//------------------------------------------------------------------------------
// �������v�Z�n���h��
static void handler_Tidoriasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_CheckSick(bpp, WAZASICK_KONRAN) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_CHIDORI_HITRATIO );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tidoriasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_Tidoriasi },    // �������v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�͂肫��v
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
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_HARIKIRI_HITRATIO );
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Harikiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_Harikiri_HitRatio },  // �������v�Z�n���h��
    { BTL_EVENT_ATTACKER_POWER, handler_Harikiri_AtkPower },  // �U���͌v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���Ƃ����v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_Atodasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SP_PRIORITY_B, BTL_SPPRI_B_LOW );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Atodasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,   handler_Atodasi },  // ����D��x�`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�X���[�X�^�[�g�v
 */
//------------------------------------------------------------------------------
// ���΂₳�v�Z�n���h��
static void handler_SlowStart_Agility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (BPP_GetMonsNo(bpp) == MONSNO_REZIGIGASU)
    &&  (BPP_GetTurnCount(bpp) < BTL_CALC_TOK_SLOWSTART_ENABLE_TURN )
    ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_SLOWSTART_AGIRATIO );
    }
  }
}
// �U���͌v�Z�n���h��
static void handler_SlowStart_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (BPP_GetMonsNo(bpp) == MONSNO_REZIGIGASU)
    &&  (BPP_GetTurnCount(bpp) < BTL_CALC_TOK_SLOWSTART_ENABLE_TURN )
    ){
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_SLOWSTART_ATKRATIO );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SlowStart( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,   handler_SlowStart_Agility },  // ���΂₳�v�Z�n���h��
    { BTL_EVENT_ATTACKER_POWER, handler_SlowStart_AtkPower }, // �U���͌v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ӂ�����v
 */
//------------------------------------------------------------------------------
// �������v�Z�n���h��
static void handler_Fukugan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_FUKUGAN_HITRATIO );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Fukugan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_Fukugan },  // �������v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���Ȃ�����v
 */
//------------------------------------------------------------------------------
// �������v�Z�n���h��
static void handler_Sunagakure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SAND )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_SUNAGAKURE_HITRATIO );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sunagakure( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_Sunagakure }, // �������v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�䂫������v
 */
//------------------------------------------------------------------------------
// �������v�Z�n���h��
static void handler_Yukigakure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SNOW )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_SUNAGAKURE_HITRATIO );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yukigakure( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_Yukigakure }, // �������v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u����߂��ˁv
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
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Iromegane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_Iromegane },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�n�[�h���b�N�v���u�t�B���^�[�v
 */
//------------------------------------------------------------------------------
// �_���[�W�v�Z�ŏI�i�K�̃n���h��
static void handler_HardRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �h�䑤��������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �������o�c�O���̎�
    BtlTypeAff  aff = BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF);
    BtlTypeAffAbout affAbout = BTL_CALC_TypeAffAbout( aff );
    if( affAbout == BTL_TYPEAFF_ABOUT_ADVANTAGE )
    {
      // �_���[�W75��
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.75) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_HardRock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_HardRock },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�X�i�C�p�[�v
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
      // �_���[�W1.5�{�i�N���e�B�J���Ȃ̂Ŏ����R�{�j
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sniper( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_Sniper },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}


//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�������v
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�̃n���h��
static void handler_Kasoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->header.tokwin_flag = TRUE;
    param->rankType = BPP_AGILITY;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->rankVolume = 1;
    param->fAlmost = TRUE;
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kasoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END, handler_Kasoku },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ă�������傭�v
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tekiouryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TYPEMATCH_RATIO, handler_Tekiouryoku }, // �^�C�v��v�␳���v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�������v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Mouka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_hpborder_powerup( flowWk, pokeID, POKETYPE_HONOO );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Mouka( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Mouka },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u������イ�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Gekiryu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_hpborder_powerup( flowWk, pokeID, POKETYPE_MIZU );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Gekiryu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Gekiryu },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����傭�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Sinryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_hpborder_powerup( flowWk, pokeID, POKETYPE_KUSA );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sinryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Sinryoku },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ނ��̂��点�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_MusinoSirase( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_hpborder_powerup( flowWk, pokeID, POKETYPE_MUSHI );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MusinoSirase( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_MusinoSirase },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32  borderHP = BTL_CALC_QuotMaxHP( bpp, 3 );
    u32  hp = BPP_GetValue( bpp, BPP_HP );
    BTL_Printf("hp=%d, borderHP=%d\n", hp, borderHP);
    if( hp <= borderHP )
    {
      // �g���̂��w��^�C�v���U�Ȃ�
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == wazaType )
      {
        // �З͂Q�{
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���񂶂傤�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Konjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �U������������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ��Ԉُ��
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetPokeSick(bpp) != POKESICK_NULL )
    {
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      // �_���[�W�^�C�v�������̎�
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
      {
        // �З͂Q�{
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Konjou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Konjou },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�v���X�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Plus( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �U������������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ��Ɂu�}�C�i�X�v����������
    if( BTL_SVFTOOL_CheckExistTokuseiPokemon(flowWk, POKETOKUSEI_MAINASU) )
    {
      // �_���[�W�^�C�v������̎�
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        // �Ƃ�����1.5�{
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_PLUS_POWRATIO );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Plus( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Plus }, // �U���͌���̃n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�}�C�i�X�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Minus( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �U������������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ��Ɂu�v���X�v����������
    if( BTL_SVFTOOL_CheckExistTokuseiPokemon(flowWk, POKETOKUSEI_PURASU) )
    {
      // �_���[�W�^�C�v������̎�
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        // �Ƃ�����1.5�{
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_PLUS_POWRATIO );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Minus( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_Minus },  // �U���͌���̃n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�t�����[�M�t�g�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_FlowerGift_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �V�C�u�͂�v��
  if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SHINE )
  {
    // �U�����������������̂Ƃ�
    u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( BTL_MAINUTIL_IsFriendPokeID(pokeID, atkPokeID) )
    {
      // ��������1.5�{
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_FLOWERGIFT_POWRATIO );
      }
    }
  }
}
// �h��͌���̃n���h��
static void handler_FlowerGift_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �V�C�u�͂�v��
  if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SHINE )
  {
    // �h�䑤�������������̂Ƃ�
    u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    if( BTL_MAINUTIL_IsFriendPokeID(pokeID, atkPokeID) )
    {
      // �Ƃ��ڂ�1.5�{
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_FLOWERGIFT_GUARDRATIO );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_FlowerGift( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_FlowerGift_Power }, // �U���͌���̃n���h��
    { BTL_EVENT_DEFENDER_GUARD, handler_FlowerGift_Guard }, // �h��͌���̃n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}



//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ƃ���������v
 */
//------------------------------------------------------------------------------
// ���U�З͌���̃n���h��
static void handler_Tousousin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �U����������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* myParam = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* targetParam = BTL_SVFTOOL_GetPokeParam( flowWk,
          BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    u8 mySex = BPP_GetValue( myParam, BPP_SEX );
    u8 targetSex = BPP_GetValue( targetParam, BPP_SEX );

    // �݂��ɐ��ʕs������Ȃ��ꍇ
    if( (mySex!=PTL_SEX_UNKNOWN) && (targetSex!=PTL_SEX_UNKNOWN) )
    {
      if( mySex == targetSex ){
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.25) );

      }else{
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.75) );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tousousin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Tousousin },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�e�N�j�V�����v
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
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, BTL_CALC_TOK_TECKNICIAN_POWRATIO );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Technician( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Technician },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ă̂��Ԃ��v
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
    if( WAZADATA_GetFlag(waza, WAZAFLAG_Punch) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, BTL_CALC_TOK_TETUNOKOBUSI_POWRATIO );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_TetunoKobusi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_TetunoKobusi },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���Ă݁v
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
    if( WAZADATA_GetParam(waza, WAZAPARAM_DAMAGE_REACTION_RATIO) )
    {
      BTL_Printf("�͂�ǂ�����̂Ŏ̂Đg����\n");
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, BTL_CALC_TOK_SUTEMI_POWRATIO );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sutemi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Sutemi },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ӂ����Ȃ��낱�v
 */
//------------------------------------------------------------------------------
///< �h��\�͌���̃n���h��
static void handler_FusiginaUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �h�䑤��������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // ��Ԉُ��
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetPokeSick(bpp) != POKESICK_NULL )
    {
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      // �_���[�W�^�C�v������̎��A�h��㏸
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_FUSIGINAUROKO_GDRATIO );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_FusiginaUroko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_FusiginaUroko },  ///< �h��\�͌���̃n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�X�L�������N�v
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
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SkillLink( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_COUNT, handler_SkillLink },  // �U���񐔌���̃n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����肫�΂��݁v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_KairikiBasami( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK, handler_KairikiBasami_Check },
    { BTL_EVENT_RANKEFF_FAILED,     handler_KairikiBasami_Guard },

  };

  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
static void handler_KairikiBasami_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_RankDownGuard_Check( flowWk, pokeID, work, WAZA_RANKEFF_ATTACK );
}
static void handler_KairikiBasami_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_RankDownGuard_Fixed( flowWk, pokeID, work, BTL_STRID_SET_RankdownFail_ATK );
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u����ǂ��߁v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Surudoime( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK, handler_Surudoime_Check }, // �����N�_�E�������`�F�b�N
    { BTL_EVENT_RANKEFF_FAILED,     handler_Surudoime_Guard },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

// �����N�_�E�����O�����̃n���h��
static void handler_Surudoime_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_RankDownGuard_Check( flowWk, pokeID, work, WAZA_RANKEFF_HIT );
}
static void handler_Surudoime_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_RankDownGuard_Fixed( flowWk, pokeID, work, BTL_STRID_SET_RankdownFail_HIT );
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�N���A�{�f�B�v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_ClearBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK, handler_ClearBody_Check }, // �����N�_�E�������`�F�b�N
    { BTL_EVENT_RANKEFF_FAILED,     handler_ClearBody_Guard },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

// �����N�_�E�����O�����̃n���h��
static void handler_ClearBody_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_RankDownGuard_Check( flowWk, pokeID, work, WAZA_RANKEFF_MAX );
}
static void handler_ClearBody_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_RankDownGuard_Fixed( flowWk, pokeID, work, BTL_STRID_SET_RankdownFail );
}

//----------------------------------------------------------------------------------
/**
 * �����N�_�E����h���Ƃ������̋��ʏ����i�`�F�b�N���j
 *
 * @param   flowWk
 * @param   pokeID
 * @param   work
 * @param   rankType    �h�������N�^�C�v�iWAZA_RANKEFF_MAX�Ȃ�S�āj
 */
//----------------------------------------------------------------------------------
static void common_RankDownGuard_Check( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaRankEffect rankType )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)
  ){
    if( (rankType == WAZA_RANKEFF_MAX)
    ||  (BTL_EVENTVAR_GetValue(BTL_EVAR_STATUS_TYPE) == rankType)
    ){
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_VOLUME) < 0 ){
        work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �����N�_�E����h���Ƃ������̋��ʏ����i�m�莞�j
 *
 * @param   flowWk
 * @param   pokeID
 * @param   work
 * @param   strID   �\�����b�Z�[�WID
 */
//----------------------------------------------------------------------------------
static void common_RankDownGuard_Fixed( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, u16 strID )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0])
  ){
    BTL_HANDEX_PARAM_MESSAGE* param;

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( &param->str, pokeID );

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );

    work[0] = 0;
  }
}



//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���񂶂��v
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tanjun( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_RANKEFF_VALUE, handler_Tanjun },  // ���U�ɂ�郉���N�������ʃ`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���[�t�K�[�h�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_ReafGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ���炤��������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �V�󂪐���
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SHINE )
    {
      // �|�P�����n��Ԉُ�ɂ͂Ȃ�Ȃ�
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) < POKESICK_MAX )
      {
        work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_ReafGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL, handler_ReafGuard         }, // �|�P�����n��Ԉُ폈���n���h��
    { BTL_EVENT_ADDSICK_FAILED,    handler_AddSickFailCommon },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���イ�Ȃ�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_Juunan_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  work[0] = common_GuardWazaSick( flowWk, pokeID, POKESICK_MAHI );
}
// �Ƃ��������������n���h��
static void handler_Juunan_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Swap_CureSick( flowWk, pokeID, POKESICK_MAHI );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Juunan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_Juunan_PokeSick },  // �|�P�����n��Ԉُ폈���n���h��
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Juunan_Swap },      // �Ƃ��������������n���h��
    { BTL_EVENT_ADDSICK_FAILED,       handler_AddSickFailCommon },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ӂ݂�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_Fumin_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  work[0] = common_GuardWazaSick( flowWk, pokeID, WAZASICK_NEMURI );
  if( work[0] == 0 ){
    work[0] = common_GuardWazaSick( flowWk, pokeID, WAZASICK_AKUBI );
  }
}
// �Ƃ��������������n���h��
static void handler_Fumin_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Swap_CureSick( flowWk, pokeID, POKESICK_NEMURI );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Fumin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_Fumin_PokeSick }, // �|�P�����n��Ԉُ폈���n���h��
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_Fumin_Swap },     // �Ƃ��������������n���h��
    { BTL_EVENT_ADDSICK_FAILED,       handler_AddSickFailCommon },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�}�O�}�̂�낢�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_MagumaNoYoroi_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  work[0] = common_GuardWazaSick( flowWk, pokeID, POKESICK_YAKEDO );
}
// �Ƃ��������������n���h��
static void handler_MagumaNoYoroi_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Swap_CureSick( flowWk, pokeID, POKESICK_YAKEDO );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MagumaNoYoroi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,     handler_MagumaNoYoroi_PokeSick }, // �|�P�����n��Ԉُ폈���n���h��
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_MagumaNoYoroi_Swap },     // �Ƃ��������������n���h��
    { BTL_EVENT_ADDSICK_FAILED,        handler_AddSickFailCommon },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�߂񂦂��v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_Meneki_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  work[0] = common_GuardWazaSick( flowWk, pokeID, POKESICK_DOKU );
}
// �Ƃ��������������n���h��
static void handler_Meneki_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Swap_CureSick( flowWk, pokeID, POKESICK_DOKU );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Meneki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,     handler_Meneki_PokeSick },  // �|�P�����n��Ԉُ폈���n���h��
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_Meneki_Swap },      // �Ƃ��������������n���h��
    { BTL_EVENT_ADDSICK_FAILED,        handler_AddSickFailCommon },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�}�C�y�[�X�v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_MyPace_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  work[0] = common_GuardWazaSick( flowWk, pokeID, WAZASICK_KONRAN );
}
// �Ƃ��������������n���h��
static void handler_MyPace_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Swap_CureSick( flowWk, pokeID, WAZASICK_KONRAN );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MyPace( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,     handler_MyPace_PokeSick },  // �|�P�����n��Ԉُ폈���n���h��
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER,  handler_MyPace_Swap },      // �Ƃ��������������n���h��
    { BTL_EVENT_ADDSICK_FAILED,        handler_AddSickFailCommon },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�݂��̃x�[���v
 */
//------------------------------------------------------------------------------
// �|�P�����n��Ԉُ폈���n���h��
static void handler_MizuNoBale_PokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  work[0] = common_GuardWazaSick( flowWk, pokeID, WAZASICK_YAKEDO );
}
// �Ƃ��������������n���h��
static void handler_MizuNoBale_Swap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Swap_CureSick( flowWk, pokeID, WAZASICK_YAKEDO );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MizuNoBale( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_MizuNoBale_PokeSick },  // �|�P�����n��Ԉُ폈���n���h��
    { BTL_EVENT_CHANGE_TOKUSEI_AFTER, handler_MizuNoBale_Swap },      // �Ƃ��������������n���h��
    { BTL_EVENT_ADDSICK_FAILED,       handler_AddSickFailCommon },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//----------------------------------------------------------------------------------
/**
 * ��Ԉُ���󂯕t���Ȃ��Ƃ������̋��ʏ���
 *
 * @param   flowWk
 * @param   pokeID
 * @param   sick
 *
 * @retval  BOOL      ���̏����ɂ��󂯕t�����ۂ����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL common_GuardWazaSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick guardSick )
{
  // ���炤��������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // ���炤�a�C���w��ʂ�
    WazaSick  attackSick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
    if( (guardSick == attackSick)
    ||  ((guardSick == WAZASICK_DOKU) && (attackSick == WAZASICK_DOKUDOKU))
    ){
      return BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
    }
  }
  return FALSE;
}
static void handler_AddSickFailCommon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (work[0] == TRUE)
  ){
    BTL_HANDEX_PARAM_MESSAGE* param;

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
    HANDEX_STR_AddArg( &param->str, pokeID );

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    work[0] = FALSE;
  }
}
static void common_Swap_CureSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_CheckSick(bpp, sick) )
    {
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        param->sickCode = sick;
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}


//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ǂ񂩂�v
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
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Donkan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL, handler_Donkan },  // ��Ԉُ편�s�`�F�b�N�n���h��
    { BTL_EVENT_ADDSICK_FAILED,    handler_AddSickFailCommon },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���߂ӂ炵�v
 */
//------------------------------------------------------------------------------
static void handler_Amefurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_weather_change( flowWk, pokeID, BTL_WEATHER_RAIN );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Amefurasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN, handler_Amefurasi }, // �������o��n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ђł�v
 */
//------------------------------------------------------------------------------
static void handler_Hideri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_weather_change( flowWk, pokeID, BTL_WEATHER_SHINE );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hideri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN, handler_Hideri },  // �������o��n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���Ȃ������v
 */
//------------------------------------------------------------------------------
static void handler_Sunaokosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_weather_change( flowWk, pokeID, BTL_WEATHER_SAND );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sunaokosi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN, handler_Sunaokosi }, // �������o��n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�䂫�ӂ炵�v
 */
//------------------------------------------------------------------------------
static void handler_Yukifurasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_weather_change( flowWk, pokeID, BTL_WEATHER_SNOW );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yukifurasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN, handler_Yukifurasi },  // �������o��n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//-------------------------------------------
/**
 *  ���ꎞ�ɓV��ω�������Ƃ������̋��ʏ���
 */
//-------------------------------------------
static void common_weather_change( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_CHANGE_WEATHER* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_WEATHER, pokeID );

    param->header.tokwin_flag = TRUE;
    param->weather = weather;
    param->turn = BTL_WEATHER_TURN_PERMANENT;
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�G�A���b�N�v&�u�m�[�Ă񂫁v
 */
//------------------------------------------------------------------------------
// �������o��n���h��
#if 0
 static void handler_AirLock_Appear( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_SVFTOOL_GetWeather(flowWk) != BTL_WEATHER_NONE )
    {
      BTL_HANDEX_PARAM_CHANGE_WEATHER* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_WEATHER, pokeID );

      param->header.tokwin_flag = TRUE;
      param->weather = BTL_WEATHER_NONE;
      param->turn = BTL_WEATHER_TURN_PERMANENT;
    }
  }
}
#endif
// �V��ω��n���h��
static void handler_AirLock_ChangeWeather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_AirLock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
//    { BTL_EVENT_MEMBER_IN,      handler_AirLock_Appear },         // �������o��n���h��
    { BTL_EVENT_WEATHER_CHECK,  handler_AirLock_ChangeWeather },  // �V��`�F�b�N�n���h��
    { BTL_EVENT_WEATHER_CHANGE, handler_AirLock_ChangeWeather },  // �V��ω��n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�A�C�X�{�f�B�v
 */
//------------------------------------------------------------------------------
// �V��_���[�W�v�Z�n���h��
static void handler_IceBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_weather_recover( flowWk, pokeID, BTL_WEATHER_SNOW );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_IcoBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION, handler_IceBody },   // �^�[���`�F�b�N�J�n�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���߂�������v
 */
//------------------------------------------------------------------------------
// �V��_���[�W�v�Z�n���h��
static void handler_AmeukeZara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_weather_recover( flowWk, pokeID, BTL_WEATHER_RAIN );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_AmeukeZara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION, handler_AmeukeZara },  // �V��_���[�W�v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

/**
 *  �V��ɉ�����HP�񕜂���Ƃ������̋��ʏ���
 */
static void common_weather_recover( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_WEATHER) == weather )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    param->header.tokwin_flag = TRUE;
    param->pokeID = pokeID;
    param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 8 );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�T���p���[�v
 */
//------------------------------------------------------------------------------
// �V��_���[�W�v�Z�n���h��
static void handler_SunPower_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ����̎��AMAXHP�� 1/8 ����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WEATHER) == BTL_WEATHER_SHINE )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      int damage = BTL_CALC_QuotMaxHP( bpp, 8 );
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
        param->pokeID = pokeID;
        param->damage = damage;
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
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
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SHINE )
    {
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      // �_���[�W�^�C�v������̎�
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        // �З�1.5�{
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5f) );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SunPower( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION, handler_SunPower_Weather },  // �V��_���[�W�v�Z�n���h��
    { BTL_EVENT_ATTACKER_POWER,  handler_SunPower_AtkPower },  // �U���͌���n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���Ղ�v
 */
//------------------------------------------------------------------------------
// �ǉ����ʁi��Ԉُ�C�����N���ʋ��ʁj�n���h��
static void handler_Rinpun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF))
  &&  (pokeID != BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK))
  ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Rinpun( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,       handler_Rinpun }, // �ǉ����ʁi��Ԉُ�j�`�F�b�N�n���h��
    { BTL_EVENT_ADD_RANK_TARGET,handler_Rinpun }, // �ǉ����ʁi�����N���ʁj�`�F�b�N�n���h��
    { BTL_EVENT_SHRINK_CHECK,   handler_Rinpun_Shrink },  // �Ђ�݃`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ă�̂߂��݁v
 */
//------------------------------------------------------------------------------
// �ǉ����ʁi��Ԉُ�C�����N���ʋ��ʁj�n���h��
static void handler_TennoMegumi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    u16 per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER ) * 2;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
    BTL_Printf("�|�P[%d]�� �Ă�̂߂��� �Œǉ��������Q�{=%d%%\n", pokeID, per);
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_TennoMegumi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,         handler_TennoMegumi },  // �ǉ����ʁi��Ԉُ�j�`�F�b�N�n���h��
    { BTL_EVENT_ADD_RANK_TARGET,  handler_TennoMegumi },  // �ǉ����ʁi�����N���ʁj�`�F�b�N�n���h��
    { BTL_EVENT_WAZA_SHRINK_PER,  handler_TennoMegumi },  // �Ђ�݃`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���邨���{�f�B�v
 */
//------------------------------------------------------------------------------
// ��Ԉُ�_���[�W�n���h��
static void handler_UruoiBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_RAIN )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( BPP_GetPokeSick(bpp) != POKESICK_NULL )
      {
        BTL_HANDEX_PARAM_CURE_SICK* param;

        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        param->sickCode = WAZASICK_EX_POKEFULL;
        param->pokeID[0] = pokeID;
        param->poke_cnt = 1;

        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );

      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_UruoiBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,      handler_UruoiBody },  // �^�[���`�F�b�N�J�n
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����ҁv
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�J�n�n���h��
static void handler_Dappi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetPokeSick(bpp) != POKESICK_NULL )
    {
      if( Tokusei_IsExePer(flowWk, 33) )
      {
        BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );

        param->header.tokwin_flag = TRUE;
        param->sickCode = WAZASICK_EX_POKEFULL;
        param->pokeID[0] = pokeID;
        param->poke_cnt = 1;
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Dappi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,      handler_Dappi },  // �^�[���`�F�b�N�J�n
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�|�C�Y���q�[���v
 */
//------------------------------------------------------------------------------
// ��Ԉُ�_���[�W�n���h��
static void handler_PoisonHeal( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) )
  {
    WazaSick sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
    if( (sick == WAZASICK_DOKU) || (sick == WAZASICK_DOKUDOKU) )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      BTL_HANDEX_PARAM_RECOVER_HP* param;

      BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, 0 );

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 8 );
      param->pokeID = pokeID;
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );

    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_PoisonHeal( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SICK_DAMAGE,      handler_PoisonHeal }, // ��Ԉُ�_���[�W�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�J�u�g�A�[�}�[�v���u�V�F���A�[�}�[�v
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_KabutoArmor( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,     handler_KabutoArmor },  // �N���e�B�J���`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���傤����v
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kyouun( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,     handler_Kyouun }, // �N���e�B�J���`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u������̂ځv
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_IkarinoTubo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
  {
    // �N���e�B�J����������U���}�b�N�X�܂ŃA�b�v
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_CRITICAL_FLAG) )
    {
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
        const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
        param->rankType = WAZA_RANKEFF_ATTACK;
        param->rankVolume = BPP_RankEffectUpLimit( bpp, BPP_ATTACK );
        param->fAlmost = TRUE;
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->fStdMsgDisable = TRUE;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_IkarinoTubo );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_IkarinoTubo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_IkarinoTubo },  // �_���[�W����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ǂ��̃g�Q�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_DokunoToge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  PokeSick sick = POKESICK_DOKU;
  BPP_SICK_CONT cont = BTL_CALC_MakeDefaultPokeSickCont( sick );
  common_touchAddSick( flowWk, pokeID, sick, cont, BTL_CALC_TOK_DOKUNOTOGE_PER );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_DokunoToge( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_DokunoToge }, // �_���[�W����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����ł񂫁v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Seidenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  PokeSick sick = POKESICK_MAHI;
  BPP_SICK_CONT cont = BTL_CALC_MakeDefaultPokeSickCont( sick );
  common_touchAddSick( flowWk, pokeID, sick, cont, BTL_CALC_TOK_DOKUNOTOGE_PER );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Seidenki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Seidenki }, // �_���[�W����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ق̂��̂��炾�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_HonoNoKarada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  PokeSick sick = POKESICK_YAKEDO;
  BPP_SICK_CONT cont = BTL_CALC_MakeDefaultPokeSickCont( sick );
  common_touchAddSick( flowWk, pokeID, sick, cont, BTL_CALC_TOK_DOKUNOTOGE_PER );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_HonoNoKarada( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_HonoNoKarada }, // �_���[�W����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���������{�f�B�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_MeromeroBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* myParam = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* targetParam = BTL_SVFTOOL_GetPokeParam( flowWk,
          BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) );

    u8 mySex = BPP_GetValue( myParam, BPP_SEX );
    u8 targetSex = BPP_GetValue( targetParam, BPP_SEX );

    if( ((mySex!=PTL_SEX_UNKNOWN) && (targetSex!=PTL_SEX_UNKNOWN))
    &&  (mySex != targetSex)
    ){
      BPP_SICK_CONT cont;
      BTL_CALC_MakeDefaultWazaSickCont( WAZASICK_MEROMERO, myParam, &cont );
      common_touchAddSick( flowWk, pokeID, WAZASICK_MEROMERO, cont, BTL_CALC_TOK_MEROMEROBODY_PER );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MeromeroBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_MeromeroBody }, // �_���[�W����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ق����v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Housi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    PokeSick sick;
    BPP_SICK_CONT cont;
    u32 rand = BTL_SVFTOOL_GetRand( flowWk, 90);
    if( rand < 30 ){
      sick = WAZASICK_DOKU;
    }else if( rand < 60 ){
      sick = WAZASICK_MAHI;
    }else{
      sick = WAZASICK_YAKEDO;
    }
    cont = BTL_CALC_MakeDefaultPokeSickCont( sick );
    common_touchAddSick( flowWk, pokeID, sick, cont, BTL_CALC_TOK_HOUSI_PER );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Housi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Housi },  // �_���[�W����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//--------------------------------------------------------------------------
/**
 * �ڐG���U�ɂ��_���[�W��A����ɑ΂��ď�Ԉُ�𔭏ǂ�����Ƃ������̋��ʏ���
 *
 * @param   flowWk
 * @param   pokeID
 * @param   sick      ��Ԉُ�ID
 * @param   per       �����m��
 *
 */
//--------------------------------------------------------------------------
static void common_touchAddSick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sick, BPP_SICK_CONT sickCont, u8 per )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag(waza, WAZAFLAG_Touch) )
    {
      if( Tokusei_IsExePer(flowWk, per) )
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

        param->header.tokwin_flag = TRUE;
        param->sickID = sick;
        param->sickCont = sickCont;
        param->fAlmost = FALSE;
        param->poke_cnt = 1;
        param->pokeID[0] = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���߂͂��v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Samehada( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag(waza, WAZAFLAG_Touch) )
    {
      u8 attackerPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, attackerPokeID );

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
        param->pokeID = attackerPokeID;
        param->damage = BTL_CALC_QuotMaxHP( bpp, 16 );
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Samehada( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Samehada }, // �_���[�W����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�䂤�΂��v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Yuubaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_IsDead(bpp) )
    {
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetFlag(waza, WAZAFLAG_Touch) )
      {
        u8 attackerPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
        const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, attackerPokeID );

        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
        {
          BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
          param->pokeID = attackerPokeID;
          param->damage = BTL_CALC_QuotMaxHP( bpp, 4 );
        }
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yuubaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Yuubaku },  // �_���[�W����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ւ񂵂傭�v
 */
//------------------------------------------------------------------------------
// �_���[�W����n���h��
static void handler_Hensyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_IsDead(bpp) )
    {
      PokeType type = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      if( !BPP_IsMatchType(bpp, type) )
      {
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
        {
          BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
          param->next_type = PokeTypePair_MakePure( type );
          param->pokeID = pokeID;
        }
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hensyoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Hensyoku }, // �_���[�W����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�V���N���v
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
      u8 attackPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      if( attackPokeID != BTL_POKEID_NULL )
      {
        BTL_HANDEX_PARAM_ADD_SICK* param;

        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        param->poke_cnt = 1;
        param->pokeID[0] = attackPokeID;
        param->sickID = sick;
        param->sickCont = BPP_SICKCONT_MakePermanent();
        param->fAlmost = TRUE;

        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Syncro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_POKESICK_FIXED,      handler_Syncro }, // �|�P�����n��Ԉُ�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}


//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���������܁v
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Isiatama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_KICKBACK,      handler_Isiatama }, // �����v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�m�[�}���X�L���v
 */
//------------------------------------------------------------------------------
// ���U�^�C�v����n���h��
static void handler_NormalSkin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, POKETYPE_NORMAL );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_NormalSkin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_NormalSkin }, // ���U�^�C�v����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�}���`�^�C�v�v
 */
//------------------------------------------------------------------------------
// ���U�^�C�v����n���h��
static void handler_MultiType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetMonsNo(bpp) == MONSNO_ARUSEUSU )
    {
      // @@@ �����Ă���A�C�e���Ń^�C�v��ς���
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, POKETYPE_NORMAL );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MultiType( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_MultiType },  // ���U�^�C�v����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�g���[�X�v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Trace( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN, handler_Trace },     // ����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// ����n���h��
static void handler_Trace( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8  allPokeID[ BTL_POSIDX_MAX ];
    u8  targetPokeID[ BTL_POSIDX_MAX ];
    const BTL_POKEPARAM* bpp;
    u8  pokeCnt, targetCnt, i;
    PokeTokusei  tok;

    targetCnt = 0;
    pokeCnt = BTL_SVFTOOL_GetAllOpponentFrontPokeID( flowWk, pokeID, allPokeID );
    for(i=0; i<pokeCnt; ++i)
    {
      bpp = BTL_SVFTOOL_GetPokeParam( flowWk, allPokeID[i] );
      tok = BPP_GetValue( bpp, BPP_TOKUSEI );
      if( !BTL_CALC_TOK_CheckCantChange(tok) && (tok != POKETOKUSEI_TOREESU) )
      {
        targetPokeID[ targetCnt++ ] = allPokeID[i];
      }
    }
    if( targetCnt )
    {
      BTL_HANDEX_PARAM_CHANGE_TOKUSEI*  param;

      u8 idx = (targetCnt==1)? 0 : BTL_SVFTOOL_GetRand( flowWk, targetCnt );
      bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID[idx] );

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
      param->pokeID = pokeID;
      param->tokuseiID = BPP_GetValue( bpp, BPP_TOKUSEI );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Trace );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID[idx] );
      HANDEX_STR_AddArg( &param->exStr, param->tokuseiID );

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����񂩂��ӂ��v
 */
//------------------------------------------------------------------------------
// �����o�[�ޏo�n���h��
static void handler_SizenKaifuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
    param->sickCode = WAZASICK_EX_POKEFULL;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fStdMsgDisable = TRUE;
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SizenKaifuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_OUT_FIXED, handler_SizenKaifuku },   // �����o�[�ޏo�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�_�E�����[�h�v
 */
//------------------------------------------------------------------------------
// �����o�[����n���h��
static void handler_Download( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 ePokeID[BTL_POSIDX_MAX];
    u8 pokeCnt = BTL_SVFTOOL_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
    if( pokeCnt )
    {
      const BTL_POKEPARAM* bpp;
      u16 sumDef = 0;
      u16 sumSpDef = 0;
      u8  valueID;
      u8 i;
      for(i=0; i<pokeCnt; ++i){
        bpp = BTL_SVFTOOL_GetPokeParam( flowWk, ePokeID[i] );
        sumDef += BPP_GetValue( bpp, BPP_DEFENCE );
        sumSpDef += BPP_GetValue( bpp, BPP_SP_DEFENCE );
      }

      // ����̂ڂ�����E�Ƃ��ڂ����r���āA�ア���ɑΉ����������̂��������E�Ƃ��������A�b�v
      if( sumDef > sumSpDef ){
        valueID = BPP_SP_ATTACK;
      }else{
        valueID = BPP_ATTACK;
      }

      {
        BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->rankType = valueID;
        param->rankVolume = 1;
      }

    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_DownLoad( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN, handler_Download },    // �����o�[����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�悿�ށv
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Yotimu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp;
    u8 ePokeID[BTL_POSIDX_MAX];
    u8 pokeCnt = BTL_SVFTOOL_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
    u8 pokeIdx = BTL_SVFTOOL_GetRand( flowWk, pokeCnt );
    bpp = BTL_SVFTOOL_GetPokeParam( flowWk, ePokeID[pokeIdx] );

    {
      u8 wazaCnt = BPP_WAZA_GetCount( bpp );
      u8 wazaIdx = BTL_SVFTOOL_GetRand( flowWk, wazaCnt );
      WazaID waza = BPP_WAZA_GetID( bpp, wazaIdx );

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_YotimuExe );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_AddArg( &param->str, waza );
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );


      BTL_EVENT_FACTOR_Remove( myHandle );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yotimu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,        handler_Yotimu }, // ����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u������悿�v
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
    u8 pokeCnt = BTL_SVFTOOL_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
    u8 i;
    bppUser = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    for(i=0; i<pokeCnt; ++i)
    {
      bppEnemy = BTL_SVFTOOL_GetPokeParam( flowWk, ePokeID[i] );
      if( _check_kikenyoti_poke(bppUser, bppEnemy) )
      {
        break;
      }
    }
    if( i != pokeCnt )
    {
      BTL_HANDEX_PARAM_MESSAGE* param;

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Kikenyoti );
      HANDEX_STR_AddArg( &param->str, pokeID );
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
static BOOL _check_kikenyoti_poke( const BTL_POKEPARAM* bppUser, const BTL_POKEPARAM* bppEnemy )
{
  WazaID  waza;
  PokeTypePair user_type;
  PokeType     waza_type;
  u8 waza_cnt, i;

  user_type = BPP_GetPokeType( bppUser );
  waza_cnt = BPP_WAZA_GetCount( bppEnemy );
  for(i=0; i<waza_cnt; ++i)
  {
    // �ꌂ�K�E���U�̓L�P��
    waza = BPP_WAZA_GetID( bppEnemy, i );
    if( WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_ICHIGEKI ){ return TRUE; }

    // �����ɑ����o�c�O���ȓz���L�P��
    waza_type = WAZADATA_GetType( waza );
    if( BTL_CALC_TypeAff(waza_type, user_type) > BTL_TYPEAFF_100 ){ return TRUE; }

    // @@@ �܂������n�����Ă��Ȃ�
  }
  return FALSE;
}

static  const BtlEventHandlerTable*  HAND_TOK_ADD_KikenYoti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,        handler_KikenYoti },  // ����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���݂Ƃ����v
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
    u8 pokeCnt = BTL_SVFTOOL_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
    u8 itemCnt, i;
    for(itemCnt=0, i=0; i<pokeCnt; ++i)
    {
      bpp = BTL_SVFTOOL_GetPokeParam( flowWk, ePokeID[i] );
      itemID[ itemCnt ] = BPP_GetItem( bpp );
      if( itemID[itemCnt] ){
        ++itemCnt;
      }
    }

    if( itemCnt ){
      i = BTL_SVFTOOL_GetRand( flowWk, itemCnt );
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Omitoosi );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_AddArg( &param->str, itemID[i] );
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }

    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Omitoosi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,        handler_Omitoosi }, // ����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���񂶂傤�v
 */
//------------------------------------------------------------------------------
// �ꌂ�`�F�b�N�n���h��
static void handler_Ganjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_FAIL_FLAG, TRUE) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param;

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
      HANDEX_STR_AddArg( &param->str, pokeID );
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Ganjou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ICHIGEKI_CHECK, handler_Ganjou },   // �ꌂ�`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ă�˂�v
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tennen( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RANK,       handler_Tennen_hitRank }, // �������E��𗦃`�F�b�N�n���h��
    { BTL_EVENT_ATTACKER_POWER_PREV, handler_Tennen_AtkRank }, // �U�������N�`�F�b�N�n���h��
    { BTL_EVENT_DEFENDER_GUARD_PREV, handler_Tennen_DefRank }, // �h�䃉���N�`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����˂v
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
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.5) );
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tainetu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Tainetsu_WazaPow },   // ���U�З͌v�Z�n���h��
    { BTL_EVENT_SICK_DAMAGE,  handler_Tainetsu_SickDmg },   // ��Ԉُ�_���[�W�v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}





static void common_DmgToRecover_Check( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType matchType )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == matchType)
  ){
    work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );;
  }
}
static void common_DmgToRecover_Fix( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, u8 denomHP )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (work[0])
  ){
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );

    if( !BPP_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP*  param = BTL_SVF_HANDEX_Push( flowWk ,BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = pokeID;
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, denomHP );

      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
    else
    {
      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���񂻂��͂��v
 */
//------------------------------------------------------------------------------
// �V��_���[�W�v�Z�n���h��
static void handler_Kansouhada_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BtlWeather weather = BTL_EVENTVAR_GetValue( BTL_EVAR_WEATHER );

    // ����̎��AMAXHP�� 1/8 ����
    if( weather == BTL_WEATHER_SHINE )
    {
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
        param->pokeID = pokeID;
        param->damage = BTL_CALC_QuotMaxHP( bpp, 8 );
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );

    // �J�̎��AMAXHP�� 1/8 ��
    }
    else if( weather == BTL_WEATHER_RAIN )
    {
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
        param->pokeID = pokeID;
        param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 8 );
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Kansouhada_DmgRecover( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Check( flowWk, pokeID, work, POKETYPE_MIZU );
}
// �_���[�W���U�񕜉�����n���h��
static void handler_Kansouhada_DmgRecoverFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Fix( flowWk, pokeID, work, 4 );
}
// ���U�З͌v�Z�n���h��
static void handler_Kansouhada_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤�̎��A�ق̂����U�̈З͂�1.25�{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_HONOO )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.25) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kansouhada( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION,      handler_Kansouhada_Weather },       // �V��_���[�W�v�Z�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,  handler_Kansouhada_DmgRecover },    // �_���[�W���U�񕜃`�F�b�N�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_FIX,    handler_Kansouhada_DmgRecoverFix }, // �_���[�W���U�񕜉�����n���h��
    { BTL_EVENT_WAZA_POWER,            handler_Kansouhada_WazaPow   },     // ���U�З͌v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���傷���v
 */
//------------------------------------------------------------------------------
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Tyosui_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Check( flowWk, pokeID, work, POKETYPE_MIZU );
}
// �_���[�W���U�񕜉�����n���h��
static void handler_Tyosui_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Fix( flowWk, pokeID, work, 4 );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tyosui( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,   handler_Tyosui_Check }, // �_���[�W���U�񕜃`�F�b�N�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_FIX,     handler_Tyosui_Fix },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����ł�v
 */
//------------------------------------------------------------------------------
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Tikuden_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Check( flowWk, pokeID, work, POKETYPE_DENKI );
}
// �_���[�W���U�񕜉�����n���h��
static void handler_Tikuden_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Fix( flowWk, pokeID, work, 4 );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tikuden( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,   handler_Tikuden_Check }, // �_���[�W���U�񕜃`�F�b�N�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_FIX,     handler_Tikuden_Fix },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ł񂫃G���W���v
 */
//------------------------------------------------------------------------------
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_DenkiEngine_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Check( flowWk, pokeID, work, POKETYPE_DENKI );
}
// �_���[�W���U�񕜉�����n���h��
static void handler_DenkiEngine_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Fix( flowWk, pokeID, work, 4 );
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;
    param->rankType = BPP_AGILITY;
    param->rankVolume = 1;
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_DenkiEngine( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,   handler_DenkiEngine_Check }, // �_���[�W���U�񕜃`�F�b�N�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_FIX,     handler_DenkiEngine_Fix   },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�m�[�K�[�h�v
 */
//------------------------------------------------------------------------------
// �^�C�v�ɂ�閳�����`�F�b�N�n���h��
static void handler_NoGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �U�������N�ł���A�K����Ԃɂ���
  u8 atkID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  const BTL_POKEPARAM* attacker = BTL_SVFTOOL_GetPokeParam( flowWk, atkID );
  if( !BPP_CheckSick(attacker, WAZASICK_MUSTHIT) )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    param->poke_cnt = 1;
    param->pokeID[0] = atkID;
    param->sickID = WAZASICK_MUSTHIT;;
    param->sickCont = BPP_SICKCONT_MakeTurn( 1 );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_NoGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,        handler_NoGuard },  // �^�C�v�ɂ�閳�����`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���������܁v
 */
//------------------------------------------------------------------------------
// �^�C�v�ɂ�閳�����`�F�b�N�n���h��
static void handler_Kimottama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����̎�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �S�[�X�g�^�C�v�ւ̖�����������
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKE_TYPE) == POKETYPE_GHOST )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, TRUE );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kimottama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,        handler_Kimottama },  // �^�C�v�ɂ�閳�����`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ڂ�����v
 */
//------------------------------------------------------------------------------
// �������`�F�b�NLv1�n���h��
static void handler_Bouon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤�̎�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �T�E���h���U����
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag( waza, WAZAFLAG_Sound ) )
    {
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, TRUE) )
      {
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
        {
          BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
          HANDEX_STR_AddArg( &param->str, pokeID );
        }
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Bouon( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L1,        handler_Bouon },  // �������`�F�b�NLv1�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ӂ䂤�v
 */
//------------------------------------------------------------------------------
// ��s�t���O�`�F�b�N�n���h��
static void handler_Fuyuu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Fuyuu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_FLYING,        handler_Fuyuu },  // ��s�t���O�`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ӂ����Ȃ܂���v
 */
//------------------------------------------------------------------------------
// �������`�F�b�NLv2�n���h��
static void handler_FusiginaMamori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤��
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)
  ){
    // �k�P�j���Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetMonsNo(bpp) == MONSNO_NUKENIN )
    {
      // ���ʃo�c�O���ȊO�͖���
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      PokeType waza_type = WAZADATA_GetType( waza );
      PokeTypePair myType = BPP_GetPokeType( bpp );
      BtlTypeAff aff = BTL_CALC_TypeAffPair( waza_type, myType );
      if( aff <= BTL_TYPEAFF_100 )
      {
        if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, TRUE) )
        {
          BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
          {
            BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
            HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
            HANDEX_STR_AddArg( &param->str, pokeID );
          }
          BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
        }
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_FusiginaMamori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,        handler_FusiginaMamori }, // �������`�F�b�NLv2�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ȃ܂��v
 */
//------------------------------------------------------------------------------
// ���U�o�����ۃ`�F�b�N�n���h��
static void handler_Namake( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 poke_turn = BPP_GetTurnCount( bpp );
    u8 appear_turn = BPP_GetAppearTurn( bpp );
    BTL_Printf("�|�P�^�[��=%d, �V�X�e���^�[��=%d\n", poke_turn, appear_turn);
    if( (poke_turn&1) != (appear_turn&1) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_NAMAKE );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Namake( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_1ST,       handler_Namake }, // ���U�o�����ۃ`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���߂肯�v
 */
//------------------------------------------------------------------------------
// ���U�o�����ۃ`�F�b�N�n���h��
static void handler_Simerike( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );

  if( (waza == WAZANO_DAIBAKUHATU)
  ||  (waza == WAZANO_ZIBAKU)
  ){
    SV_WazaFailCause cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );
    if( cause == SV_WAZAFAIL_NULL )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_TOKUSEI );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Simerike( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,       handler_Simerike }, // ���U�o�����ۃ`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���炢�сv
 */
//------------------------------------------------------------------------------
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Moraibi_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤�̎�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �����U�͖������u���炢�сv����
    PokeType waza_type = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    if( waza_type == POKETYPE_HONOO )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }
  }
}
// �_���[�W���U�񕜉�����n���h��
static void handler_Moraibi_DmgRecoverFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( work[0] == 0 )
    {
      work[0] = 1;  // �u���炢�сv�����t���O�Ƃ��ė��p

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MoraibiExe );
        HANDEX_STR_AddArg( &param->str, pokeID );
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
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
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, BTL_CALC_TOK_MORAIBI_POWRATIO );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Moraibi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DMG_TO_RECOVER_CHECK, handler_Moraibi_NoEffect      }, // �_���[�W���U�񕜉��`�F�b�N�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_FIX,   handler_Moraibi_DmgRecoverFix }, // �_���[�W�񕜉�����n���h��
    { BTL_EVENT_ATTACKER_POWER,       handler_Moraibi_AtkPower      }, // �U���͌���n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�i�C�g���A�v
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�ŏI�n���h��
static void handler_Nightmare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp;
    u8 ePokeID[ BTL_POSIDX_MAX ];
    u8 pokeCnt = BTL_SVFTOOL_GetAllOpponentFrontPokeID( flowWk, pokeID, ePokeID );
    u8 exe_flag = FALSE;
    u8 i;
    for(i=0; i<pokeCnt; ++i)
    {
      bpp = BTL_SVFTOOL_GetPokeParam( flowWk, ePokeID[i] );
      if( BPP_CheckSick(bpp, WAZASICK_NEMURI) )
      {
        BTL_HANDEX_PARAM_DAMAGE* param;

        if( exe_flag == FALSE ){
          exe_flag = TRUE;
          BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
        }

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
        param->pokeID = ePokeID[i];
        param->damage = BTL_CALC_QuotMaxHP( bpp, 8 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_NightMare );
        HANDEX_STR_AddArg( &param->exStr, ePokeID[i] );
      }
    }
    if( exe_flag ){
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Nightmare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,        handler_Nightmare },  // �^�[���`�F�b�N�ŏI�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ɂ������v
 */
//------------------------------------------------------------------------------
// �ɂ���`�F�b�N�n���h��
static void handler_Nigeasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Nigeasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SKIP_NIGERU_CALC,       handler_Nigeasi },  // �ɂ���`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���肶�����v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Arijigoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_SVFRET_CantEscapeAdd( flowWk, pokeID, BTL_CANTESC_ARIJIGOKU );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Arijigoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,        handler_Arijigoku },  // ����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����ӂ݁v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Kagefumi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_SVFRET_CantEscapeAdd( flowWk, pokeID, BTL_CANTESC_KAGEFUMI );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kagefumi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,        handler_Kagefumi }, // ����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u����傭�v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Jiryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_SVFRET_CantEscapeAdd( flowWk, pokeID, BTL_CANTESC_JIRYOKU );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Jiryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,        handler_Jiryoku },  // ����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u������Ԃ�v
 */
//------------------------------------------------------------------------------
// �����o�[����n���h��
static void handler_Katayaburi_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Katayaburi );
    HANDEX_STR_AddArg( &param->str, pokeID );
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}
// �X�L�b�v�`�F�b�N�n���h��
static BOOL handler_Katayaburi_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID )
{
  BTL_Printf("������Ԃ�̃X�L�b�v�`�F�b�N�n���h�����Ă΂ꂽ\n");
  if( factorType == BTL_EVENT_FACTOR_TOKUSEI )
  {
    // �^�[������������Ƃ������Q
    static const u16 disableTokTable[] = {
      POKETOKUSEI_FUSIGINAMAMORI, POKETOKUSEI_BOUON,      POKETOKUSEI_FUYUU,
      POKETOKUSEI_SUNAGAKURE,     POKETOKUSEI_YUKIGAKURE, POKETOKUSEI_TYOSUI,
      POKETOKUSEI_KABUTOAAMAA,    POKETOKUSEI_HIRAISIN,   POKETOKUSEI_YOBIMIZU,
      POKETOKUSEI_SHERUAAMAA,     POKETOKUSEI_TENNEN,     POKETOKUSEI_HIRAISIN,
      POKETOKUSEI_YOBIMIZU,       POKETOKUSEI_KYUUBAN,    POKETOKUSEI_TANJUN,
      POKETOKUSEI_TIDORIASI,      POKETOKUSEI_HAADOROKKU, POKETOKUSEI_FIRUTAA,
      POKETOKUSEI_MORAIBI,        POKETOKUSEI_DENKIENJIN, POKETOKUSEI_NENCHAKU,
      POKETOKUSEI_FUSIGINAUROKO,  POKETOKUSEI_ATUISIBOU,  POKETOKUSEI_TAINETU,
      POKETOKUSEI_SIROIKEMURI,    POKETOKUSEI_KURIABODY,  POKETOKUSEI_SURUDOIME,
      POKETOKUSEI_KAIRIKIBASAMI,  POKETOKUSEI_SEISINRYOKU,POKETOKUSEI_RINPUN,
      POKETOKUSEI_GANJOU,         POKETOKUSEI_SIMERIKE,   POKETOKUSEI_DONKAN,
      POKETOKUSEI_JUUNAN,         POKETOKUSEI_MAIPEESU,   POKETOKUSEI_MIZUNOBEERU,
      POKETOKUSEI_HONOONOKARADA,  POKETOKUSEI_RIIFUGAADO, POKETOKUSEI_FUMIN,
      POKETOKUSEI_YARUKI,         POKETOKUSEI_MENEKI,
    };
    u32 i;
    for(i=0; i<NELEMS(disableTokTable); ++i)
    {
      if( disableTokTable[i] == subID ){
        BTL_Printf("����̓X�L�b�v������Ƃ�������\n");
        return TRUE;
      }
    }
  }
  return FALSE;
}
// ���U�����J�n�n���h��
static void handler_Katayaburi_Start( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����Ȃ�X�L�b�v�`�F�b�N�n���h�����A�^�b�`����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] == 0 ){
      BTL_EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Katayaburi_SkipCheck );
      work[0] = 1;
    }
  }
}
// ���U�����I���n���h��
static void handler_Katayaburi_End( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����Ȃ�X�L�b�v�`�F�b�N�n���h�����f�^�b�`����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] == 1 ){
      BTL_EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
      work[0] = 0;
    }
  }
}

static  const BtlEventHandlerTable*  HAND_TOK_ADD_Katayaburi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,        handler_Katayaburi_MemberIn },  // ����n���h��
    { BTL_EVENT_WAZASEQ_START,    handler_Katayaburi_Start },     // ���U�����J�n�n���h��
    { BTL_EVENT_WAZASEQ_END,      handler_Katayaburi_End   },     // ���U�����I���n���h��
  };

  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ă񂫂�v
 */
//------------------------------------------------------------------------------
// �|�P����n���h��
static void handler_Tenkiya_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlWeather weather = BTL_SVFTOOL_GetWeather(flowWk);
    common_TenkiFormChange( flowWk, pokeID, weather );
  }
}
// �V��ω��n���h��
static void handler_Tenkiya_Weather( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BtlWeather weather = BTL_EVENTVAR_GetValue( BTL_EVAR_WEATHER );
  common_TenkiFormChange( flowWk, pokeID, weather );
}
static void common_TenkiFormChange( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  if( BPP_GetMonsNo(bpp) == MONSNO_POWARUN )
  {
    u8 form_now, form_next;

    form_now = BPP_GetValue( bpp, BPP_FORM );
    switch( weather ){
    case BTL_WEATHER_NONE:  form_next = FORMNO_POWARUN_NORMAL; break;
    case BTL_WEATHER_SHINE: form_next = FORMNO_POWARUN_SUN; break;
    case BTL_WEATHER_RAIN:  form_next = FORMNO_POWARUN_RAIN; break;
    case BTL_WEATHER_SNOW:  form_next = FORMNO_POWARUN_SNOW; break;
    default:
      form_next = form_now;
    }

    if( form_next != form_now )
    {
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
      {
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MoraibiExe );
        HANDEX_STR_AddArg( &param->str, pokeID );
      }
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tenkiya( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Tenkiya_MemberIn  },  // �|�P����n���h��
    { BTL_EVENT_WEATHER_CHANGE_AFTER, handler_Tenkiya_Weather   },  // �V��ω���n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u��т݂��v
 */
//------------------------------------------------------------------------------
// ���U�^�[�Q�b�g����n���h��
static void handler_Yobimizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaTargetChangeToMe( flowWk, pokeID, POKETYPE_MIZU );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yobimizu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DECIDE_TARGET,    handler_Yobimizu  },  // ���U�^�[�Q�b�g����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ђ炢����v
 */
//------------------------------------------------------------------------------
// ���U�^�[�Q�b�g����n���h��
static void handler_Hiraisin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaTargetChangeToMe( flowWk, pokeID, POKETYPE_DENKI );
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hiraisin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DECIDE_TARGET,    handler_Hiraisin  },  // ���U�^�[�Q�b�g����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
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
 *  �Ƃ������u���イ�΂�v
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
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kyuuban( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_PUSHOUT,    handler_Kyuuban   },  // �ӂ��Ƃ΂��`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�w�h�������v
 */
//------------------------------------------------------------------------------
// �h���C���ʌv�Z�n���h��
static void handler_HedoroEki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    u16 damage = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, 0 );
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
    {
      BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
      param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      param->damage = damage;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HedoroEki );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );

    }
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_HedoroEki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_DRAIN,    handler_HedoroEki   },  // �h���C���ʌv�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ԃ��悤�v
 */
//------------------------------------------------------------------------------
static BOOL handler_Bukiyou_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID )
{
  if( factorType == BTL_EVENT_FACTOR_ITEM ){
    if( BTL_EVENT_FACTOR_GetPokeID(myHandle) == pokeID ){
      return TRUE;
    }
  }
  return FALSE;
}
/// �����o�[����n���h��
static void handler_Bukiyou_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 ){
    BTL_EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Bukiyou_SkipCheck );
    work[0] = 1;
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Bukiyou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN, handler_Bukiyou_MemberIn },  /// �����o�[����n���h��
  };

  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�˂񂿂Ⴍ�v
 */
//------------------------------------------------------------------------------
static void handler_Nenchaku_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤�̂Ƃ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �g���b�N�E���肩���͖���
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( (waza == WAZANO_TORIKKU)
    ||  (waza == WAZANO_SURIKAE)
    ||  (waza == WAZANO_DOROBOU)
    ){
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, TRUE) )
      {
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
        {
          BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
          HANDEX_STR_AddArg( &param->str, pokeID );
        }
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
static void handler_Nenchaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_ITEM) == ITEM_DUMMY_DATA)
  ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Nenchaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,    handler_Nenchaku_NoEff },  // �������`�F�b�NLv2�n���h��
    { BTL_EVENT_ITEMSET_CHECK,        handler_Nenchaku   },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�v���b�V���[�v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Pressure( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,            handler_Pressure_MemberIN },  // �����o�[����n���h��
    { BTL_EVENT_DECREMENT_PP_VOLUME,  handler_Pressure          },  // PP����`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �����o�[����n���h��
static void handler_Pressure_MemberIN( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
    {
       BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
       HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Pressure );
       HANDEX_STR_AddArg( &param->str, pokeID );
    }
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}
// PP����`�F�b�N�n���h��
static void handler_Pressure( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  u32 cnt, i;
  cnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
  for(i=0; i<cnt; ++i)
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1+i) == pokeID )
    {
      u8 dec = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
      ++dec;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, dec );
      break;
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�}�W�b�N�K�[�h�v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MagicGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SIMPLE_DAMAGE_ENABLE,  handler_MagicGuard },  // ���U�ȊO�_���[�W�`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// ���U�ȊO�_���[�W�`�F�b�N�n���h��
static void handler_MagicGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, FALSE );
  }
}


//------------------------------------------------------------------------------
/**
 * �Ƃ������u��邢�Ă����v
 *
 * �����U�����󂯂��ہA����ɋ󂫂�����Α���̎�������D�����B
 */
//------------------------------------------------------------------------------
// �_���[�W�����n���h��
static void handler_WaruiTeguse( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    const BTL_POKEPARAM* me = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* attacker = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( (BPP_GetItem(me) == ITEM_DUMMY_DATA) && (BPP_GetItem(attacker) != ITEM_DUMMY_DATA) )
    {
      BTL_HANDEX_PARAM_SWAP_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SWAP_ITEM, pokeID );
      param->header.tokwin_flag = TRUE;
      param->pokeID = targetPokeID;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_WaruiTeguse );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID );
      HANDEX_STR_AddArg( &param->exStr, BPP_GetItem(attacker) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_WaruiTeguse( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,    handler_WaruiTeguse },  // �_���[�W�����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 * �Ƃ������u�̂���{�f�B�v
 */
//------------------------------------------------------------------------------
static void handler_NorowareBody( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, WAZASICK_KANASIBARI) )
    {
      if( Tokusei_IsExePer(flowWk, 30) )
      {
        WazaID  prevWaza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
        if( prevWaza != WAZANO_NULL )
        {
          BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

          param->sickID = WAZASICK_KANASIBARI;
          param->sickCont = BPP_SICKCONT_MakeTurnParam( 4, prevWaza );
          param->poke_cnt = 1;
          param->pokeID[0] = targetPokeID;
          param->header.tokwin_flag = TRUE;
          BTL_Printf("�̂���{�f�B����\n");
        }
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_NorowareBody( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,    handler_NorowareBody },  // �_���[�W�����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���������낢�v
 */
//------------------------------------------------------------------------------
static void handler_KudakeruYoroi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BOOL fEffective = FALSE;

    if( BPP_IsRankEffectValid(bpp, BPP_DEFENCE, -1)
    ||  BPP_IsRankEffectValid(bpp, BPP_AGILITY,  1)
    ){
      fEffective = TRUE;
    }

    if( fEffective )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param;

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->fAlmost = TRUE;
        param->rankType = BPP_DEFENCE;
        param->rankVolume = -1;

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->fAlmost = TRUE;
        param->rankType = BPP_AGILITY;
        param->rankVolume = 1;

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_KudakeruYoroi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,    handler_KudakeruYoroi },  // �_���[�W�����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����炸���v
 */
//------------------------------------------------------------------------------
// �З͌v�Z�n���h��
static void handler_Tikarazuku_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( IsTikarazukuEffecive(waza) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.3) );
    }
  }
}
// �ǉ����ʃ`�F�b�N�n���h��
static void handler_Tikarazuku_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( IsTikarazukuEffecive(waza) )
    {
      BTL_Printf("�����炸���Ȃ̂Ō��ʂ��������܂���\n");
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
    }
  }
}

static  const BtlEventHandlerTable*  HAND_TOK_ADD_Tikarazuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,      handler_Tikarazuku_WazaPow   }, // ���U�З͌v�Z�n���h��
    { BTL_EVENT_ADD_SICK,        handler_Tikarazuku_CheckFail }, // �ǉ����ʁi��Ԉُ�j�`�F�b�N�n���h��
    { BTL_EVENT_ADD_RANK_TARGET, handler_Tikarazuku_CheckFail }, // �ǉ����ʁi�����N���ʁj�`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

/**
 *  �u�����炸���v���������U���ǂ�������
 */
static BOOL IsTikarazukuEffecive( WazaID waza )
{
  WazaCategory category = WAZADATA_GetCategory( waza );

  switch( category ){
  // ����������Ɍ��ʂ���������^�C�v�͖������ɗL��
  case WAZADATA_CATEGORY_DAMAGE_SICK:
  case WAZADATA_CATEGORY_DAMAGE_EFFECT:
    return TRUE;

  // �����Ɍ��ʂ���������^�C�v�́A�����N�_�E���̌��ʂ����郏�U�̂ݖ���
  case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
    {
      u32 numEffects = WAZADATA_GetRankEffectCount( waza );
      int value;
      u32 i;
      for(i=0; i<numEffects; ++i)
      {
        WAZADATA_GetRankEffect( waza, i, &value );
        if( value < 0 ){
          return FALSE;
        }
      }
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�܂��񂫁v
 */
//------------------------------------------------------------------------------
static void handler_Makenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    int volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    if( volume < 0 )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

      param->header.tokwin_flag = TRUE;
      param->rankType = BPP_ATTACK;
      param->rankVolume = 2;
      param->fAlmost = TRUE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Makenki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_FIXED,    handler_Makenki },  // �����N���ʊm��n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u��킫�v
 */
//------------------------------------------------------------------------------
static void handler_Yowaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 hp = BPP_GetValue( bpp, BPP_HP );
    u32 hp_half = BTL_CALC_QuotMaxHP( bpp, 2 );
    if( hp <= hp_half )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5f) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Yowaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER,    handler_Yowaki },  // �U�����\�͂̌v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�}���`�X�P�C���v
 */
//------------------------------------------------------------------------------
static void handler_MultiScale( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // HP���^���Ȃ�_���[�W����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_IsHPFull(bpp) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5f) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MultiScale( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_MultiScale },  // �_���[�W�v�Z�ŏI�`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�Ȃ��܂������v
 */
//------------------------------------------------------------------------------
static void handler_NakamaIsiki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  u8 defPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

  // �����ȊO�̖����ւ̃_���[�W����
  if( (defPokeID != pokeID)
  &&  BTL_MAINUTIL_IsFriendPokeID(pokeID, defPokeID)
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5f) );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_NakamaIsiki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_NakamaIsiki },  // �_���[�W�v�Z�ŏI�`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����e���s�[�v
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�n���h��
static void handler_Moraterapi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlPokePos  myPos = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
    BtlExPos    exPos = EXPOS_MAKE( BTL_EXPOS_AREA_FRIENDS, myPos );

    const BTL_POKEPARAM* bpp;
    u8 targetID[ BTL_POSIDX_MAX ];
    u8 cnt, i;

    cnt = BTL_SVFTOOL_ExpandPokeID( flowWk, exPos, targetID );
    for(i=0; i<cnt; ++i)
    {
      if( targetID[ i ] == pokeID ){ continue; }
      bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetID[i] );
      if( BPP_GetPokeSick(bpp) != POKESICK_NULL )
      {
        if( Tokusei_IsExePer(flowWk, 30) )
        {
          BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );

          param->header.tokwin_flag = TRUE;
          param->poke_cnt = 1;
          param->pokeID[0] = targetID[i];
          param->sickCode = WAZASICK_EX_POKEFULL;
        }
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Moraterapi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_Moraterapi },  // �^�[���`�F�b�N�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ǂ��ڂ������v
 */
//------------------------------------------------------------------------------
// �З͌v�Z�n���h��
static void handler_Dokubousou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �������u�ǂ��v�Ȃ畨���U���̈З͂��{
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_CheckSick( bpp, WAZASICK_DOKU )
    &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_PHYSIC)
    ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Dokubousou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,      handler_Dokubousou   }, // ���U�З͌v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�˂ڂ������v
 */
//------------------------------------------------------------------------------
// �З͌v�Z�n���h��
static void handler_Netubousou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �������u�₯�ǁv�Ȃ����U���̈З͂��{
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_CheckSick( bpp, WAZASICK_YAKEDO )
    &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_SPECIAL)
    ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Netubousou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,      handler_Netubousou   }, // ���U�З͌v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u������̂����v
 */
//------------------------------------------------------------------------------
// �������`�F�b�NLv2�n���h��
static void handler_AunNoIki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �U�����������Ȃ疳��
    u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( BTL_MAINUTIL_IsFriendPokeID(pokeID, atkPokeID) )
    {
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, TRUE) )
      {
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
        {
          BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
          HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_AunNoIki );
          HANDEX_STR_AddArg( &param->str, pokeID );
        }
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_AunNoIki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,        handler_AunNoIki }, // �������`�F�b�NLv2�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ނ�����v
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�ŏI�n���h��
static void handler_Murakke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* sys_work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    enum {
      PATTERN_MAX = ((BPP_RANKVALUE_RANGE * BPP_RANKVALUE_RANGE) - BPP_RANKVALUE_RANGE) * 2,
    };

    // �d�����Ȃ��u�㏸�E���~�v�̑g�ݍ��킹��񋓂��邽�߂̌v�Z�p���[�N
    // ���K���A�قȂ�Q�̔\�̓����N���u�㏸�v�u���~�v����悤�ɂ���B
    // �i�S�\�̓����N���オ�肫���Ă���i�����肫���Ă���j�ꍇ�͏����j
    typedef struct {

      u16 patterns[ PATTERN_MAX ];
      u8  upEffects[ BPP_RANKVALUE_RANGE ];
      u8  downEffects[ BPP_RANKVALUE_RANGE ];

    }CALC_WORK;


    CALC_WORK* work = BTL_SVFTOOL_GetTmpWork( flowWk, sizeof(CALC_WORK) );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 upEffect, downEffect, upCnt, downCnt, ptnCnt;
    u32 i;

  //  BTL_Printf("pokeID=%d\n, bpp=%p\n", pokeID, bpp);

    // �オ�肤��\�́A�����肤��\�͂�񋓁A�J�E���g����
    upCnt = downCnt = 0;
    for(i=0; i<BPP_RANKVALUE_RANGE; ++i){
      if( BPP_IsRankEffectValid(bpp, BPP_ATTACK+i, 1) ){
        work->upEffects[ upCnt++ ] = BPP_ATTACK+i;
      }
      if( BPP_IsRankEffectValid(bpp, BPP_ATTACK+i, -1) ){
        work->downEffects[ downCnt++ ] = BPP_ATTACK+i;
      }
    }


    // �オ�肤��i�����肤��j�\�͂��O�̏ꍇ�A�����_���łP�����オ��i������j
    upEffect = downEffect = BPP_VALUE_NULL;
    if( (upCnt == 0) && (downCnt != 0) ){
      i = BTL_SVFTOOL_GetRand( flowWk, downCnt );
      downEffect = work->downEffects[ i ];
    }
    else if( (upCnt != 0) && (downCnt == 0) ){
      i = BTL_SVFTOOL_GetRand( flowWk, upCnt );
      upEffect = work->upEffects[ i ];
    }
    // �オ�肤��\�́A�����肤��\�͂����ɂP�ȏ�̏ꍇ�A�l������g�ݍ��킹��񋓁A�J�E���g����
    else if( (upCnt != 0) && (downCnt != 0) )
    {
      u32 j;
      ptnCnt = 0;
      for(i=0; i<upCnt; ++i)
      {
        for(j=0; j<downCnt; ++j)
        {
          if( work->upEffects[i] != work->downEffects[j] )
          {
            work->patterns[ ptnCnt++ ] = (work->upEffects[i] << 8) | (work->downEffects[j]);
            if( ptnCnt >= NELEMS(work->patterns) ){
              j = downCnt;  // for loop out
              break;
            }
          }
        }
      }
      // �g�ݍ��킹�̒�����P�������_������
      i = BTL_SVFTOOL_GetRand( flowWk, ptnCnt );
      upEffect = (work->patterns[i] >> 8) & 0xff;
      downEffect = work->patterns[i] & 0xff;
    }

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param;

      if( upEffect != BPP_VALUE_NULL )
      {
        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->rankType = upEffect;
        param->rankVolume = 1;
      }
      if( downEffect != BPP_VALUE_NULL )
      {
        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->rankType = downEffect;
        param->rankVolume = -1;
      }
    }
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );

  } // if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Murakke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,        handler_Murakke },  // �^�[���`�F�b�N�ŏI�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ڂ�����v
 */
//------------------------------------------------------------------------------
// �V��_���[�W�v�Z�n���h��
static void handler_Boujin_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE) > 0)
  ){
    if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_FAIL_FLAG, TRUE) )
    {
//      BTL_EVENT
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Boujin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEATHER_REACTION,    handler_Boujin_CalcDamage   }, // �V��_���[�W�v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ǂ�����v
 */
//------------------------------------------------------------------------------
// �_���[�W�����n���h��
static void handler_Dokusyu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �ڐG���U�Ȃ�
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag(waza, WAZAFLAG_Touch) )
    {
      // �R���̊m���ő�����ǂ��ɂ���
      if( Tokusei_IsExePer(flowWk, 30) )
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        param->header.tokwin_flag = TRUE;
        param->poke_cnt = 1;
        param->pokeID[0] = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
        param->sickID = WAZASICK_DOKU;
        param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( WAZASICK_DOKU );

        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Dokusyu );
        HANDEX_STR_AddArg( &param->exStr, param->pokeID[0] );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Dokusyu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,        handler_Dokusyu }, // �_���[�W�����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u����������傭�v
 */
//------------------------------------------------------------------------------
// �����o�[�ޏ�m��n���h��
static void handler_SaiseiRyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_IsDead(bpp) && !BPP_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_SHIFT_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
      u32 recoverHP = BTL_CALC_QuotMaxHP( bpp, 3 );
      u32 recoverLimit = BPP_GetValue(bpp, BPP_MAX_HP) - BPP_GetValue(bpp, BPP_HP);
      if( recoverHP > recoverLimit ){
        recoverHP = recoverLimit;
      }
      param->pokeID[0] = pokeID;
      param->volume[0] = recoverHP;
      param->poke_cnt = 1;
      param->fEffectDisable = TRUE;
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SaiseiRyoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_OUT_FIXED, handler_SaiseiRyoku },   // �����o�[�ޏ�m��n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�͂Ƃނˁv
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hatomune( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK, handler_Hatomune_Check }, // �����N�_�E�������`�F�b�N
    { BTL_EVENT_RANKEFF_FAILED,     handler_Hatomune_Guard },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

// �����N�_�E�����O�����̃n���h��
static void handler_Hatomune_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_RankDownGuard_Check( flowWk, pokeID, work, WAZA_RANKEFF_DEFENCE );
}
static void handler_Hatomune_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_RankDownGuard_Fixed( flowWk, pokeID, work, BTL_STRID_SET_RankdownFail_DEF );
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���Ȃ����v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sunakaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_Sunakaki   },    /// ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}

// ���΂₳�v�Z�n���h��
static void handler_Sunakaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // �V�󂪂��Ȃ��炵�Ȃ�f�����Q�{
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SAND ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�~���N���X�L���v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_MilacreSkin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO, handler_MilacreSkin   },    /// ���U�������擾�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
/// ���U�������擾�n���h��
static void handler_MilacreSkin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h���
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // ��_���[�W���U�Ȃ疽����50%
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_NONE )
    {
      u32 per = BTL_EVENTVAR_GetValue( BTL_EVAR_HIT_PER );
      if( per > 50 ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_HIT_PER, 50 );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���񂤂��v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sinuti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Sinuti   },    // ���U�З͌v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// ���U�З͌v�Z�n���h��
static void handler_Sinuti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 myOrder, totalAction;
    // ���ԍŌ�Ȃ�З͑���
    if( BTL_SVFTOOL_GetMyActionOrder(flowWk, pokeID, &myOrder, &totalAction) ){
      if( (myOrder+1) == totalAction )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.3) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���Ȃ̂�����v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SunanoTikara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_SunanoTikara   },    // ���U�З͌v�Z�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// ���U�З͌v�Z�n���h��
static void handler_SunanoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �V�󂷂Ȃ��炵�Ȃ�З͑���
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SAND )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.3) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u����ʂ��v
 */
//------------------------------------------------------------------------------
// �u����ʂ��v�X�L�b�v�`�F�b�N�n���h��
static BOOL handler_Surinuke_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID )
{
  if( factorType == BTL_EVENT_FACTOR_SIDE )
  {
    if( (subID == BTL_SIDEEFF_REFRECTOR)        // ���t���N�^�[����
    ||  (subID == BTL_SIDEEFF_HIKARINOKABE)     // �Ђ���̂��ז���
    ||  (subID == BTL_SIDEEFF_SINPINOMAMORI)    // ����҂̂܂��薳��
    ){
      return TRUE;
    }
  }
  return FALSE;
}
// ���U�����J�n
static void handler_Surinuke_Start( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����Ȃ�X�L�b�v�`�F�b�N�n���h�����A�^�b�`����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Surinuke_SkipCheck );
  }
}
// ���U�����I��
static void handler_Surinuke_End( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����Ȃ�X�L�b�v�`�F�b�N�n���h�����f�^�b�`����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Surinuke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,      handler_Surinuke_Start },  // ���U�����J�n
    { BTL_EVENT_WAZASEQ_END,        handler_Surinuke_End   },  // ���U�����I��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�����񂩂��傤�v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_JisinKajou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END, handler_JisinKajou   },    // ���U�_���[�W�v���Z�X�ŏI�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// ���U�_���[�W�v���Z�X�ŏI�n���h��
static void handler_JisinKajou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ���񂾃|�P����Ȃ�U�������N�A�b�v
    const BTL_POKEPARAM* bpp;
    u32 i, targetCnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    for(i=0; i<targetCnt; ++i)
    {
      bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1+i) );
      if( BPP_IsDead(bpp) )
      {
        BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
        param->header.tokwin_flag = TRUE;
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->rankType = BPP_ATTACK;
        param->rankVolume = 1;
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�������̂�����v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_SeiginoKokoro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_SeiginoKokoro   },    // �_���[�W�����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_SeiginoKokoro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �󂯂����U�^�C�v�����Ȃ�U�������N�A�b�v
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_AKU )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->header.tokwin_flag = TRUE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_ATTACK;
      param->rankVolume = 1;
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�тт�v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Bibiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_Bibiri   },    // �_���[�W�����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_Bibiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �󂯂����U�^�C�v�����E���E�S�[�X�g�Ȃ�f���������N�A�b�v
    PokeType  type = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    if( (type == POKETYPE_AKU)
    ||  (type == POKETYPE_MUSHI)
    ||  (type == POKETYPE_GHOST)
    ){
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->header.tokwin_flag = TRUE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_AGILITY;
      param->rankVolume = 1;
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�~�C���v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Miira( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_Miira   },    // �_���[�W�����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_Miira( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �ڐG���U�Ȃ瑊��̂Ƃ��������u�~�C���v��
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag(waza, WAZAFLAG_Touch) )
    {
      BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
      param->tokuseiID = POKETOKUSEI_MIIRA;
      param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Miira );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�������傭�v
 */
//------------------------------------------------------------------------------
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Sousyoku_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Check( flowWk, pokeID, work, POKETYPE_KUSA );
}
// �_���[�W���U�񕜉�����n���h��
static void handler_Sousyoku_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DmgToRecover_Fix( flowWk, pokeID, work, 4 );
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->rankType = BPP_SP_ATTACK;
    param->rankVolume = 1;
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Sousyoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,   handler_Sousyoku_Check }, // �_���[�W���U�񕜃`�F�b�N�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_FIX,     handler_Sousyoku_Fix   },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�������炲����v
 */
//------------------------------------------------------------------------------
// ���U�v���C�I���e�B�擾�n���h��
static void handler_ItazuraGokoro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �_���[�W���U�ȊO�Ȃ�v���C�I���e�B�A�b�v
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_NONE )
    {
      u8 pri = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_PRI );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_PRI, pri+1 );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_ItazuraGokoro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_WAZA_PRI,   handler_ItazuraGokoro }, // ���U�v���C�I���e�B�擾�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�}�W�b�N�~���[�v
 */
//------------------------------------------------------------------------------
// �킴������n���h��
static void handler_MagicMirror_CheckRob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  u8 fEnable = FALSE;

  work[0] = 0;

  if( atkPokeID != pokeID )
  {
    u8 i, targetCnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    for(i=0; i<targetCnt; ++i)
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1+i) == pokeID ){
        fEnable = TRUE;
        break;
      }
    }
  }

  if( fEnable )
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag(waza, WAZAFLAG_MagicCoat) )
    {
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_POKEID, pokeID) )
      {
        // ����S�̃��U�ȊO�̓^�[�Q�b�g���Œ��
        if( WAZADATA_GetParam(waza, WAZAPARAM_TARGET) != WAZA_TARGET_ENEMY_ALL ){
          BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, atkPokeID );
        }
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
        work[0] = 1;
      }
    }
  }
}
//
static void handler_MagicMirror_Reflect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] == 1)
  ){
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
    {
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MajicMirror );
      HANDEX_STR_AddArg( &param->str, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) );
      HANDEX_STR_AddArg( &param->str, BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
    }
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
    work[0] = 0;
  }
}

static  const BtlEventHandlerTable*  HAND_TOK_ADD_MagicMirror( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_WAZA_ROB,     handler_MagicMirror_CheckRob },  // �킴�����蔻��n���h��
    { BTL_EVENT_WAZASEQ_REFRECT,    handler_MagicMirror_Reflect  },  // �킴���˕Ԃ��m��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���イ�����v
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�J�n�n���h��
static void handler_Syuukaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_ITEM_REMOVED) )
    {
      u16 usedItem = BPP_GetUsedItem( bpp );
      if( ITEM_CheckNuts(usedItem) )
      {
        BTL_HANDEX_PARAM_SET_ITEM* param;

        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
        param->itemID = usedItem;
        param->pokeID = pokeID;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Syuukaku );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, usedItem );

        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
      }
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Syuukaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,        handler_Syuukaku },  // �^�[���`�F�b�N�J�n�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���܂̂��Ⴍ�v
 */
//------------------------------------------------------------------------------
// �����N�����l�C���n���h��
static void handler_Amanojaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    int volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    if( (volume > 0)
    ||  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)
    ){
      volume *= -1;
      work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, volume );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Amanojaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKVALUE_CHANGE,        handler_Amanojaku },  // �����N�����l�C���n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u���񂿂傤����v
 */
//------------------------------------------------------------------------------
// �����o�[����
static void handler_Kinchoukan_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;
    BtlSide side = BTL_MAINUTIL_GetOpponentSide( BTL_MAINUTIL_PokeIDtoSide(pokeID) );

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_IN, pokeID );
    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Kinchoukan );
    HANDEX_STR_AddArg( &param->str, side );
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TOKWIN_OUT, pokeID );
  }
}
// �����A�C�e���g�p�`�F�b�N�n���h��
static void handler_Kinchoukan_CheckItemEquip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // work[0] ��������ԂȂ瑊�葤�̑����A�C�e���g�p�����s������
  if( work[0] == 0 )
  {
    u8 userPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
    if( userPokeID != pokeID )
    {
      u16 itemID = BTL_EVENTVAR_GetValue( BTL_EVAR_ITEM );
      if( ITEM_CheckNuts(itemID) )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
      }
    }
  }
}
// �����o�[�ޏ�m��n���h��
static void handler_Kinchoukan_MemberOutFixed( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    common_KinchoukanOff( flowWk, pokeID, work );
  }
}
// �������m��n���h��
static void handler_Kinchoukan_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    common_KinchoukanOff( flowWk, pokeID, work );
  }
}
// �Ƃ������ύX�m��n���h��
static void handler_Kinchoukan_ChangeTok( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_TOKUSEI) != BTL_EVENT_FACTOR_GetSubID(myHandle))
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  ){
    common_KinchoukanOff( flowWk, pokeID, work );
  }
}
// ���񂿂傤���񖳌�������
static void common_KinchoukanOff( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp;
  u8 targetPokeID[ BTL_POSIDX_MAX ];
  u8 targetCnt, i;

  BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokeID( flowWk, pokeID );
  BtlExPos   exPos = EXPOS_MAKE( BTL_EXPOS_FULL_ENEMY, myPos );

  // work[0] �ɒl�����đ����A�C�e���g�p�`�F�b�N�@�\���I�t
  work[0] = 1;

  targetCnt = BTL_SVFTOOL_ExpandPokeID( flowWk, exPos, targetPokeID );
  for(i=0; i<targetCnt; ++i)
  {
    bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID[i] );
    if( !BPP_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_EQUIP_ITEM* param;
      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_EQUIP_ITEM, pokeID );
      param->pokeID = targetPokeID[ i ];
    }
  }
}

static  const BtlEventHandlerTable*  HAND_TOK_ADD_Kinchoukan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,             handler_Kinchoukan_MemberIn       }, // �����o�[����n���h��
    { BTL_EVENT_CHECK_ITEMEQUIP_FAIL,  handler_Kinchoukan_CheckItemEquip }, // �����A�C�e���g�p�`�F�b�N�n���h��
    { BTL_EVENT_MEMBER_OUT_FIXED,      handler_Kinchoukan_MemberOutFixed }, // �����o�[�ޏ�m��n���h��
    { BTL_EVENT_IEKI_FIXED,            handler_Kinchoukan_Ieki           }, // �������m��n���h��
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE, handler_Kinchoukan_ChangeTok      }, // �Ƃ������ϊ����O
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�ւ񂵂�v
 */
//------------------------------------------------------------------------------
// ����n���h��
static void handler_Hensin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlPokePos myPos = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
    BtlRule rule = BTL_SVFTOOL_GetRule( flowWk );
    BtlPokePos tgtPos = BTL_MAINUTIL_GetFacedPokePos( rule, myPos );

    u8 tgtPokeID = BTL_SVFTOOL_PokePosToPokeID( flowWk, tgtPos );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, tgtPokeID );
    if( !BPP_IsDead(target) )
    {
      BTL_HANDEX_PARAM_HENSIN* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_HENSIN, pokeID );
      param->header.tokwin_flag = TRUE;
      param->pokeID = tgtPokeID;

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Hensin );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );
    }
  }
}
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Hensin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,        handler_Hensin },  // ����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �Ƃ������u�C�����[�W�����v
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_TOK_ADD_Illusion( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Illusion_Damage    },   // �_���[�W�����n���h��
    { BTL_EVENT_IEKI_FIXED,            handler_Illusion_Ieki      },   // �������m��n���h��
    { BTL_EVENT_CHANGE_TOKUSEI_BEFORE, handler_Illusion_ChangeTok },   // �Ƃ������ϊ����O
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_Illusion_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤�Ȃ�C�����[�W��������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    common_IllusionBreak( myHandle, flowWk, pokeID );
  }
}
// �������m��n���h��
static void handler_Illusion_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    common_IllusionBreak( myHandle, flowWk, pokeID );
  }
}
// �Ƃ������ϊ����O�n���h��
static void handler_Illusion_ChangeTok( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_TOKUSEI) != BTL_EVENT_FACTOR_GetSubID(myHandle))
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  ){
    common_IllusionBreak( myHandle, flowWk, pokeID );
  }
}

static void common_IllusionBreak( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID )
{
  // �C�����[�W�����p�����Ȃ琳�̂𖾂���
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  if( BPP_IsFakeEnable(bpp) )
  {
    BTL_HANDEX_PARAM_FAKE_BREAK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_FAKE_BREAK, pokeID );
    param->pokeID = pokeID;
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Illusion_Break );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  }
}

