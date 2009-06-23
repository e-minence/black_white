//=============================================================================================
/**
 * @file  hand_waza.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [���U] ����
 * @author  taya
 *
 * @date  2009.05.15  �쐬
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

#include "hand_side.h"
#include "hand_waza.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_STICK = EVENT_HANDLER_WORK_ELEMS - 1,
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL is_registerd( u8 pokeID, WazaID waza );
static void removeHandlerForce( u8 pokeID, WazaID waza );
static BTL_EVENT_FACTOR*  ADD_Texture( u16 pri, WazaID waza, u8 pokeID );
static void handler_Texture( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Refresh( u16 pri, WazaID waza, u8 pokeID );
static void handler_Refresh( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KumoNoSu( u16 pri, WazaID waza, u8 pokeID );
static void handler_KumoNoSu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KuroiKiri( u16 pri, WazaID waza, u8 pokeID );
static void handler_KuroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Haneru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Haneru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Yumekui( u16 pri, WazaID waza, u8 pokeID );
static void handler_Yumekui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Yuwaku( u16 pri, WazaID waza, u8 pokeID );
static void handler_Yuwaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_TriAttack( u16 pri, WazaID waza, u8 pokeID );
static void handler_TriAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_IkariNoMaeba( u16 pri, WazaID waza, u8 pokeID );
static void handler_IkariNoMaeba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_RyuuNoIkari( u16 pri, WazaID waza, u8 pokeID );
static void handler_RyuuNoIkari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SonicBoom( u16 pri, WazaID waza, u8 pokeID );
static void handler_SonicBoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Gamusyara( u16 pri, WazaID waza, u8 pokeID );
static void handler_Gamusyara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_TikyuuNage( u16 pri, WazaID waza, u8 pokeID );
static void handler_TikyuuNage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_PsycoWave( u16 pri, WazaID waza, u8 pokeID );
static void handler_PsycoWave( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ibiki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ibiki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Akumu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Akumu_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ikari( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ikari_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Ikari_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Abareru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Abareru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Korogaru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Korogaru_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Korogaru_ExeFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Korogaru_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Jitabata( u16 pri, WazaID waza, u8 pokeID );
static void handler_Jitabata( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Karagenki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Karagenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Sippegaesi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Sippegaesi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Funka( u16 pri, WazaID waza, u8 pokeID );
static void handler_Funka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Siboritoru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Siboritoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Siomizu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Siomizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ongaesi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ongaesi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Yatuatari( u16 pri, WazaID waza, u8 pokeID );
static void handler_Yatuatari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MezamasiBinta( u16 pri, WazaID waza, u8 pokeID );
static void handler_MezamasiBinta( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MezamasiBinta_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Kituke( u16 pri, WazaID waza, u8 pokeID );
static void handler_Kituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kituke_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Present( u16 pri, WazaID waza, u8 pokeID );
static void handler_Present_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Present_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Present_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Kirifuda( u16 pri, WazaID waza, u8 pokeID );
static void handler_Kirifuda( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Osioki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Osioki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_WeatherBall( u16 pri, WazaID waza, u8 pokeID );
static void handler_WeatherBall_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_WeatherBall_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Tatumaki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Tatumaki_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tatumaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SkyUpper( u16 pri, WazaID waza, u8 pokeID );
static void handler_SkyUpper( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Kaminari( u16 pri, WazaID waza, u8 pokeID );
static void handler_Kaminari_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kaminari_excuseHitCalc( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kaminari_hitRatio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Jisin( u16 pri, WazaID waza, u8 pokeID );
static void handler_Jisin_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Jisin_damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MezameruPower( u16 pri, WazaID waza, u8 pokeID );
static void handler_MezameruPower_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MezameruPower_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Magnitude( u16 pri, WazaID waza, u8 pokeID );
static void handler_Magnitude_pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Magnitude_effect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Naminori( u16 pri, WazaID waza, u8 pokeID );
static void handler_Naminori_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Naminori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Fumituke( u16 pri, WazaID waza, u8 pokeID );
static void handler_Fumituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Mineuti( u16 pri, WazaID waza, u8 pokeID );
static void handler_Mineuti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Koraeru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Koraeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Koraeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Mamoru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Mamoru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Mamoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Haradaiko( u16 pri, WazaID waza, u8 pokeID );
static void handler_Haradaiko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Feint( u16 pri, WazaID waza, u8 pokeID );
static void handler_Feint_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Feint_MamoruBreak( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Feint_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_TuboWoTuku( u16 pri, WazaID waza, u8 pokeID );
static void handler_TuboWoTuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Nemuru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Nemuru_exeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nemuru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Encore( u16 pri, WazaID waza, u8 pokeID );
static void handler_Encore( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Chouhatu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Chouhatu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ichamon( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ichamon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Fuuin( u16 pri, WazaID waza, u8 pokeID );
static void handler_Fuuin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Alomatherapy( u16 pri, WazaID waza, u8 pokeID );
static void handler_Alomatherapy( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_IyasiNoSuzu( u16 pri, WazaID waza, u8 pokeID );
static void handler_IyasiNoSuzu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_CureFriendPokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, u16 strID );
static BTL_EVENT_FACTOR*  ADD_Okimiyage( u16 pri, WazaID waza, u8 pokeID );
static void handler_Okimiyage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Urami( u16 pri, WazaID waza, u8 pokeID );
static void handler_Urami( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_LockON( u16 pri, WazaID waza, u8 pokeID );
static void handler_LockON( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Refrector( u16 pri, WazaID waza, u8 pokeID );
static void handler_Refrector( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_HikariNoKabe( u16 pri, WazaID waza, u8 pokeID );
static void handler_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SinpiNoMamori( u16 pri, WazaID waza, u8 pokeID );
static void handler_SinpiNoMamori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SiroiKiri( u16 pri, WazaID waza, u8 pokeID );
static void handler_SiroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Oikaze( u16 pri, WazaID waza, u8 pokeID );
static void handler_Oikaze( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Omajinai( u16 pri, WazaID waza, u8 pokeID );
static void handler_Omajinai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_SideEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, BPP_SICK_CONT cont, u16 strID );
static BTL_EVENT_FACTOR*  ADD_Ieki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Narikiri( u16 pri, WazaID waza, u8 pokeID );
static void handler_Narikiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Tedasuke( u16 pri, WazaID waza, u8 pokeID );
static void handler_Tedasuke_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tedasuke_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tedasuke_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Nekodamasi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Nekodamasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_AsaNoHizasi( u16 pri, WazaID waza, u8 pokeID );
static void handler_AsaNoHizasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SoraWoTobu( u16 pri, WazaID waza, u8 pokeID );
static void handler_SoraWoTobu_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SoraWoTobu_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_ShadowDive( u16 pri, WazaID waza, u8 pokeID );
static void handler_ShadowDive_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ShadowDive_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ShadowDive_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Tobihaneru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Tobihaneru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tobihaneru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Diving( u16 pri, WazaID waza, u8 pokeID );
static void handler_Diving_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Diving_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_AnaWoHoru( u16 pri, WazaID waza, u8 pokeID );
static void handler_AnaWoHoru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_AnaWoHoru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SolarBeam( u16 pri, WazaID waza, u8 pokeID );
static void handler_SolarBeam_TameSkip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SolarBeam_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SolarBeam_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Kamaitati( u16 pri, WazaID waza, u8 pokeID );
static void handler_Kamaitati_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_GodBird( u16 pri, WazaID waza, u8 pokeID );
static void handler_GodBird_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_RocketZutuki( u16 pri, WazaID waza, u8 pokeID );
static void handler_RocketZutuki_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* ADD_Waruagaki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Waruagaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Bakajikara( u16 pri, WazaID waza, u8 pokeID );
static void handler_Bakajikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Michidure( u16 pri, WazaID waza, u8 pokeID );
static void handler_Michidure_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Michidure_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Michidure_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Onnen( u16 pri, WazaID waza, u8 pokeID );
static void handler_Onnen_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Onnen_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Onnen_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Tiisakunaru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Tiisakunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Marukunaru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Marukunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );




//=============================================================================================
/**
 * �|�P�����́u���U�v�n���h�����V�X�e���ɒǉ�
 *
 * @param   pp
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  BTL_HANDLER_Waza_Add( const BTL_POKEPARAM* pp, WazaID waza )
{
  typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, WazaID wazaID, u8 pokeID );

  static const struct {
    WazaID         waza;
    pEventAddFunc  func;
  }funcTbl[] = {
    { WAZANO_TEKUSUTYAA,      ADD_Texture       },
    { WAZANO_KUROIKIRI,       ADD_KuroiKiri     },
    { WAZANO_YUMEKUI,         ADD_Yumekui       },
    { WAZANO_TORAIATAKKU,     ADD_TriAttack     },
    { WAZANO_IKARINOMAEBA,    ADD_IkariNoMaeba  },
    { WAZANO_RYUUNOIKARI,     ADD_RyuuNoIkari   },
    { WAZANO_TIKYUUNAGE,      ADD_TikyuuNage    },
    { WAZANO_NAITOHEDDO,      ADD_TikyuuNage    },  // �i�C�g�w�b�h=�����イ�Ȃ��Ɠ���
    { WAZANO_SAIKOWHEEBU,     ADD_PsycoWave     },
    { WAZANO_IBIKI,           ADD_Ibiki         },
    { WAZANO_ZITABATA,        ADD_Jitabata      },
    { WAZANO_KISIKAISEI,      ADD_Jitabata      },  // ������������=�����΂� �Ɠ���
    { WAZANO_MINEUTI,         ADD_Mineuti       },
    { WAZANO_KUMONOSU,        ADD_KumoNoSu      },
    { WAZANO_KUROIMANAZASI,   ADD_KumoNoSu      },  // ���낢�܂Ȃ��� = �����̂� �Ɠ���
    { WAZANO_TOOSENBOU,       ADD_KumoNoSu      },  // �Ƃ�����ڂ�  = �����̂� �Ɠ���
    { WAZANO_KORAERU,         ADD_Koraeru       },
    { WAZANO_SONIKKUBUUMU,    ADD_SonicBoom     },
    { WAZANO_NEKODAMASI,      ADD_Nekodamasi    },
    { WAZANO_GAMUSYARA,       ADD_Gamusyara     },
    { WAZANO_HUNKA,           ADD_Funka         },
    { WAZANO_SIOHUKI,         ADD_Funka         },  // �����ӂ�=�ӂ� �Ɠ���
    { WAZANO_RIHURESSYU,      ADD_Refresh       },
    { WAZANO_ASANOHIZASI,     ADD_AsaNoHizasi   },
    { WAZANO_TUKINOHIKARI,    ADD_AsaNoHizasi   },  // ���̂Ђ��� = �����̂Ђ��� �Ɠ���
    { WAZANO_KOUGOUSEI,       ADD_AsaNoHizasi   },  // ������������ = �����̂Ђ��� �Ɠ���
    { WAZANO_SIBORITORU,      ADD_Siboritoru    },
    { WAZANO_NIGIRITUBUSU,    ADD_Siboritoru    },  // �ɂ���Ԃ� = ���ڂ�Ƃ� �Ɠ���
    { WAZANO_WHEZAABOORU,     ADD_WeatherBall   },
    { WAZANO_MAMORU,          ADD_Mamoru        },
    { WAZANO_MIKIRI,          ADD_Mamoru        },  // �݂��� = �܂��� �Ɠ���
    { WAZANO_HANERU,          ADD_Haneru        },
    { WAZANO_ABARERU,         ADD_Abareru       },
    { WAZANO_HANABIRANOMAI,   ADD_Abareru       },  // �͂Ȃт�̂܂� = ���΂�� �Ɠ���
    { WAZANO_GEKIRIN,         ADD_Abareru       },  // ������� = ���΂�� �Ɠ���
    { WAZANO_SIOMIZU,         ADD_Siomizu       },
    { WAZANO_MEZAMASIBINTA,   ADD_MezamasiBinta },
    { WAZANO_KITUKE,          ADD_Kituke        },
    { WAZANO_TUBOWOTUKU,      ADD_TuboWoTuku    },
    { WAZANO_KIRIHUDA,        ADD_Kirifuda      },
    { WAZANO_SORAWOTOBU,      ADD_SoraWoTobu    },
    { WAZANO_TOBIHANERU,      ADD_Tobihaneru    },
    { WAZANO_DAIBINGU,        ADD_Diving        },
    { WAZANO_ANAWOHORU,       ADD_AnaWoHoru     },
    { WAZANO_SOORAABIIMU,     ADD_SolarBeam     },
    { WAZANO_KAMAITATI,       ADD_Kamaitati     },
    { WAZANO_GODDOBAADO,      ADD_GodBird       },
    { WAZANO_ROKETTOZUTUKI,   ADD_RocketZutuki  },
    { WAZANO_ANKOORU,         ADD_Encore        },
    { WAZANO_TATUMAKI,        ADD_Tatumaki      },
    { WAZANO_KAZEOKOSI,       ADD_Tatumaki      },
    { WAZANO_ZISIN,           ADD_Jisin         },
    { WAZANO_NAMINORI,        ADD_Naminori      },
    { WAZANO_SUKAIAPPAA,      ADD_SkyUpper      },
    { WAZANO_MAGUNITYUUDO,    ADD_Magnitude     },
    { WAZANO_FEINTO,          ADD_Feint         },
    { WAZANO_SYADOODAIBU,     ADD_ShadowDive    },
    { WAZANO_WARUAGAKI,       ADD_Waruagaki     },
    { WAZANO_OSIOKI,          ADD_Osioki        },
    { WAZANO_BAKADIKARA,      ADD_Bakajikara    },
    { WAZANO_TYOUHATU,        ADD_Chouhatu      },
    { WAZANO_YUUWAKU,         ADD_Yuwaku        },
    { WAZANO_HARADAIKO,       ADD_Haradaiko     },
    { WAZANO_MITIDURE,        ADD_Michidure     },
    { WAZANO_KARAGENKI,       ADD_Karagenki     },
    { WAZANO_SIPPEGAESI,      ADD_Sippegaesi    },
    { WAZANO_MEZAMERUPAWAA,   ADD_MezameruPower },
    { WAZANO_TIISAKUNARU,     ADD_Tiisakunaru   },
    { WAZANO_MARUKUNARU,      ADD_Marukunaru    },
    { WAZANO_HUMITUKE,        ADD_Fumituke      },
    { WAZANO_AKUMU,           ADD_Akumu         },
    { WAZANO_KOROGARU,        ADD_Korogaru      },
    { WAZANO_AISUBOORU,       ADD_Korogaru      },  // �A�C�X�{�[��=���낪�� �Ɠ���
    { WAZANO_AROMASERAPII,    ADD_Alomatherapy  },
    { WAZANO_IYASINOSUZU,     ADD_IyasiNoSuzu   },
    { WAZANO_OKIMIYAGE,       ADD_Okimiyage     },
    { WAZANO_URAMI,           ADD_Urami         },
    { WAZANO_NEMURU,          ADD_Nemuru        },
    { WAZANO_ROKKUON,         ADD_LockON        },
    { WAZANO_KOKORONOME,      ADD_LockON        },  // ������̂�=���b�N�I���Ɠ���
    { WAZANO_RIHUREKUTAA,     ADD_Refrector     },
    { WAZANO_HIKARINOKABE,    ADD_HikariNoKabe  },
    { WAZANO_SINPINOMAMORI,   ADD_SinpiNoMamori },
    { WAZANO_SIROIKIRI,       ADD_SiroiKiri     },
    { WAZANO_OIKAZE,          ADD_Oikaze        },
    { WAZANO_ONGAESI,         ADD_Ongaesi       },
    { WAZANO_YATUATARI,       ADD_Yatuatari     },
    { WAZANO_PUREZENTO,       ADD_Present       },
    { WAZANO_ITYAMON,         ADD_Ichamon       },
    { WAZANO_HUUIN,           ADD_Fuuin         },
    { WAZANO_ONNEN,           ADD_Onnen         },
    { WAZANO_TEDASUKE,        ADD_Tedasuke      },
    { WAZANO_IEKI,            ADD_Ieki          },
    { WAZANO_NARIKIRI,        ADD_Narikiri      },
  };

  int i;

  for(i=0; i<NELEMS(funcTbl); i++)
  {
    if( funcTbl[i].waza == waza )
    {
      u8 pokeID = BTL_POKEPARAM_GetID( pp );

      if( !is_registerd(pokeID, waza) ){
        u16 agi = BTL_POKEPARAM_GetValue( pp, BPP_AGILITY );
        BTL_Printf("���U�n���h��[%d]�o�^\n", waza);
        return funcTbl[i].func( agi, waza, pokeID );
      }
    }
  }

  return NULL;
}
//----------------------------------------------------------------------------------
/**
 * ���ɓ����|�P���������U�̃n���h�����o�^����Ă��邩�H
 *
 * @param   pokeID
 * @param   waza
 *
 * @retval  BOOL    �o�^����Ă�����TRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_registerd( u8 pokeID, WazaID waza )
{
  BTL_EVENT_FACTOR* factor;

  factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    if( BTL_EVENT_FACTOR_GetSubID(factor) == waza ){
      return TRUE;
    }
    factor = BTL_EVENT_GetNextFactor( factor );
  }
  return FALSE;
}
//=============================================================================================
/**
 * �|�P�����́u���U�v�n���h�����V�X�e������폜�i���U�̋����ɂ��\��t���Ă�����͍̂폜���Ȃ��j
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void BTL_HANDLER_Waza_Remove( const BTL_POKEPARAM* pp, WazaID waza )
{
  BTL_EVENT_FACTOR *factor, *next;
  u8 pokeID = BTL_POKEPARAM_GetID( pp );

  factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    next = BTL_EVENT_GetNextFactor( factor );
    if( (BTL_EVENT_FACTOR_GetSubID(factor) == waza)
    &&  (BTL_EVENT_FACTOR_GetWorkValue(factor, WORKIDX_STICK) == 0)
    ){
      BTL_Printf("pokeID[%d], waza[%d]�n���h���폜\n", pokeID, waza);
      BTL_EVENT_FACTOR_Remove( factor );
    }
    factor = next;
  }
}
//=============================================================================================
/**
 * �|�P�����́u���U�v�n���h�����V�X�e�����狭���폜�i�\��t���Ă�����̂������I�ɍ폜�j
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void BTL_HANDLER_Waza_RemoveForce( const BTL_POKEPARAM* pp, WazaID waza )
{
  u8 pokeID = BTL_POKEPARAM_GetID( pp );
  removeHandlerForce( pokeID, waza );
}
//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   pokeID
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void removeHandlerForce( u8 pokeID, WazaID waza )
{
  BTL_EVENT_FACTOR* factor;
  while( (factor = BTL_EVENT_SeekFactor(BTL_EVENT_FACTOR_WAZA, pokeID)) != NULL )
  {
    if( BTL_EVENT_FACTOR_GetSubID(factor) == waza){
      BTL_EVENT_FACTOR_Remove( factor );
    }
  }
}
//=============================================================================================
/**
 * ����|�P�����̃��U�n���h�����V�X�e������S�ċ����폜�i�\��t���Ă�����̂������I�ɍ폜�j
 *
 * @param   pp
 */
//=============================================================================================
void BTL_HANDLER_Waza_RemoveForceAll( const BTL_POKEPARAM* pp )
{
  u8 pokeID = BTL_POKEPARAM_GetID( pp );
  BTL_EVENT_FACTOR* factor;
  while( (factor = BTL_EVENT_SeekFactor(BTL_EVENT_FACTOR_WAZA, pokeID)) != NULL )
  {
    BTL_EVENT_FACTOR_Remove( factor );
  }
}


//----------------------------------------------------------------------------------
/**
 * �e�N�X�`���[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Texture( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_Texture },    // �����ރ��U�n���h��
//    { BTL_EVENT_USE_ITEM,      handler_KuraboNomi_Exe },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Texture( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u16 waza_cnt = BTL_POKEPARAM_GetWazaCount( bpp );
    PokeType next_type;
    {
      PokeType type[ PTL_WAZA_MAX ];
      u16 i;
      for(i=0; i<waza_cnt; ++i){
        type[i] = WAZADATA_GetType( BTL_POKEPARAM_GetWazaNumber(bpp, i) );
      }
      i = GFL_STD_MtRand( waza_cnt );
      next_type = PokeTypePair_MakePure( type[i] );
    }

    {
      BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
      param->next_type = next_type;
      param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���t���b�V��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Refresh( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_Refresh },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Refresh( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    PokeSick sick = BTL_POKEPARAM_GetPokeSick( bpp );

    if( (sick == POKESICK_DOKU)
    ||  (sick == POKESICK_MAHI)
    ||  (sick == POKESICK_YAKEDO)
    ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->sickCode = sick;
      param->pokeID[0] = pokeID;
      param->poke_cnt = 1;
      // @@@ ���b�Z�[�W���邩�H
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �N���̂��A���낢�܂Ȃ����A�Ƃ�����ڂ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KumoNoSu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_KumoNoSu },    // �����ރ��U�n���h��
//    { BTL_EVENT_USE_ITEM,      handler_KuraboNomi_Exe },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_KumoNoSu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BTL_POKEPARAM_CheckSick(bpp, WAZASICK_CANT_ESCAPE) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->sickID = WAZASICK_CANT_ESCAPE;
      param->sickCont = BTL_CALC_MakeWazaSickCont_Poke( targetPokeID );

      param->fExMsg = TRUE;
      param->exStrID = BTL_STRID_SET_CantEscWaza;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ���낢����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KuroiKiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_KuroiKiri },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_KuroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    reset_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_STD, pokeID );

    {
      BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
      BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_ALL, myPos );

      reset_param->poke_cnt = BTL_SERVERFLOW_RECEPT_GetTargetPokeID( flowWk, expos, reset_param->pokeID );
    }

    msg_param->strID = BTL_STRID_STD_RankReset;
    msg_param->arg_cnt = 0;
  }
}

//----------------------------------------------------------------------------------
/**
 * �͂˂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Haneru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_Haneru },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Haneru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_STD, pokeID );
    param->strID = BTL_STRID_STD_NotHappen;
    param->arg_cnt = 0;
  }
}

//----------------------------------------------------------------------------------
/**
 * ��߂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yumekui( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Yumekui },    // �������`�F�b�N���x���Q�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Yumekui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BTL_POKEPARAM_CheckSick(target, POKESICK_NEMURI) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �䂤�킭
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yuwaku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Yuwaku },    // �������`�F�b�N���x���Q�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Yuwaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );
    const BTL_POKEPARAM* me = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

    u8 my_sex = BTL_POKEPARAM_GetValue( me, BPP_SEX );
    u8 target_sex = BTL_POKEPARAM_GetValue( target, BPP_SEX );

    if( (my_sex == PTL_SEX_UNKNOWN)
    ||  (target_sex == PTL_SEX_UNKNOWN)
    ||  (my_sex == target_sex)
    ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �g���C�A�^�b�N
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TriAttack( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_TriAttack },    // �ǉ����ʂɂ���Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_TriAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    static const PokeSick sick_tbl[] = {
      POKESICK_MAHI, POKESICK_YAKEDO, POKESICK_KOORI
    };
    PokeSick sick;
    BPP_SICK_CONT cont;
    u8 rand = GFL_STD_MtRand( NELEMS(sick_tbl) );

    sick = sick_tbl[ rand ];
    BTL_CALC_MakeDefaultPokeSickCont( sick, &cont );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 20 );
  }
}

//----------------------------------------------------------------------------------
/**
 * ������̂܂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_IkariNoMaeba( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_IkariNoMaeba },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_IkariNoMaeba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    u16 damage = BTL_POKEPARAM_GetValue(target, BPP_HP) / 2;

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��イ�̂�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_RyuuNoIkari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_RyuuNoIkari },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_RyuuNoIkari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, 40 );
  }
}
//----------------------------------------------------------------------------------
/**
 * �\�j�b�N�u�[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SonicBoom( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_SonicBoom },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_SonicBoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, 20 );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���ނ����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Gamusyara( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_Gamusyara },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Gamusyara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM *attacker, *defender;
    int diff;

    attacker = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    defender = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    diff = BTL_POKEPARAM_GetValue(defender, BPP_HP) - BTL_POKEPARAM_GetValue(attacker, BPP_HP);
    if( diff < 0 ){
      diff = 0;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, diff );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����イ�Ȃ��A�i�C�g�w�b�h
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TikyuuNage( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_TikyuuNage },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_TikyuuNage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 level = BTL_POKEPARAM_GetValue( bpp, BPP_LEVEL );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, level );
  }
}
//----------------------------------------------------------------------------------
/**
 * �T�C�R�E�F�[�u
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PsycoWave( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_PsycoWave },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_PsycoWave( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 level = BTL_POKEPARAM_GetValue( bpp, BPP_LEVEL );
    u16 rand = 50 + GFL_STD_MtRand(101);
    u16 damage = level * rand / 100;
    if( damage == 0 ){
      damage = 1;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���т�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ibiki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_Ibiki },    // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Ibiki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    SV_WazaFailCause  fail_cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );

    if( fail_cause == SV_WAZAFAIL_NEMURI ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_NULL );
    }else{
      if( !BTL_POKEPARAM_CheckSick(bpp, POKESICK_NEMURI) ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Akumu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Akumu_NoEff },    // ���U�����`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Akumu_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF);
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BTL_POKEPARAM_CheckSick(bpp, POKESICK_NEMURI) ){
      BTL_Printf("����[%d]���Q�ĂȂ����玸�s\n", targetPokeID);
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ikari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Ikari_Exe },    // ���U�o���m��n���h��
    { BTL_EVENT_WAZA_DMG_AFTER, handler_Ikari_React },    // �_���[�W�����n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Ikari_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) == BTL_EVENT_FACTOR_GetSubID(myHandle) ){
      work[ WORKIDX_STICK ] = 1;
    }else{
      const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
      BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
}
static void handler_Ikari_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( !BTL_POKEPARAM_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

      param->rankType = WAZA_RANKEFF_ATTACK;
      param->rankVolume = 1;
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
    }
    BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���΂��E�͂Ȃт�̂܂��E�������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Abareru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Abareru },            // ���U�o���m��n���h��
    { BTL_EVENT_TURNCHECK_END,    handler_Abareru_turnCheck },  // �^�[���`�F�b�N�I���n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Abareru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���������U���b�N��ԂɂȂ��Ă��Ȃ��Ȃ�A���U���b�N��Ԃɂ���
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( (!BTL_POKEPARAM_CheckSick(bpp, WAZASICK_WAZALOCK_HARD))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      u8 turns;

      turns = 3 + GFL_STD_MtRand(2);

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK_HARD;
      param->sickCont = BPP_SICKCONT_MakeTurn( turns );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fExMsg = FALSE;

      work[ WORKIDX_STICK ] = 1;
    }
  }
}
static void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] != 0)
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( !BTL_POKEPARAM_CheckSick(bpp, WAZASICK_WAZALOCK_HARD)
    &&  !BTL_POKEPARAM_CheckSick(bpp, WAZASICK_KONRAN)
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_KONRAN;
      BTL_CALC_MakeDefaultWazaSickCont( param->sickID, bpp, &param->sickCont );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fExMsg = FALSE;

      BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���낪��E�A�C�X�{�[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Korogaru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,       handler_Korogaru_ExeFix    },  // ���U�o���m��n���h��
    { BTL_EVENT_WAZA_EXECUTE_FAIL,      handler_Korogaru_ExeFail   },  // ���U�o�����s�m��n���h��
    { BTL_EVENT_WAZA_EXECUTE_NO_EFFECT, handler_Korogaru_ExeFail  },   // ���U�����n���h��
    { BTL_EVENT_WAZA_POWER,             handler_Korogaru_Pow      },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Korogaru_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���������U���b�N��ԂɂȂ��Ă��Ȃ��Ȃ�A���U���b�N��Ԃɂ���
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( (!BTL_POKEPARAM_CheckSick(bpp, WAZASICK_WAZALOCK_HARD))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;

      u8 turns = 5;

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK_HARD;
      param->sickCont = BPP_SICKCONT_MakeTurn( turns );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fExMsg = FALSE;

      work[0] = 0;
      work[ WORKIDX_STICK ] = 1;
    }
  }
}
// ���U�������s
static void handler_Korogaru_ExeFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���s�����烏�U���b�N�������\��t������
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
    param->pokeID[0] = pokeID;
    param->poke_cnt = 1;
    param->sickCode = WAZASICK_WAZALOCK_HARD;

    removeHandlerForce( pokeID, BTL_EVENT_FACTOR_GetSubID(myHandle) );
  }
}
// �З͌���
static void handler_Korogaru_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 mul = 1, i;

    for(i=0; i<work[0]; ++i){
      mul *= 2;
    }
    if( BTL_POKEPARAM_GetContFlag(bpp, BPP_CONTFLG_MARUKUNARU) ){
      mul *= 2;
    }

    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(mul) );

    work[0]++;
    if( work[0] >= 5 ){
      removeHandlerForce( pokeID, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �����΂��A������������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Jitabata( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Jitabata },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Jitabata( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const struct {
    fx32  ratio;
    u32   pow;
  }tbl[]={
    { FX32_CONST(2.0),  200 },
    { FX32_CONST(7.8),  150 },
    { FX32_CONST(18.7), 100 },
    { FX32_CONST(32.8),  80 },
    { FX32_CONST(65.6),  40 },
    { FX32_CONST(100),   20 },
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    fx32 r = BTL_POKEPARAM_GetHPRatio( bpp );
    u32 i, max;

    max = NELEMS(tbl) - 1;
    for(i=0; i<max; ++i)
    {
      if( tbl[i].ratio < r ){ break; }
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, tbl[i].pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���炰��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Karagenki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Karagenki },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Karagenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    PokeSick  sick = BTL_POKEPARAM_GetPokeSick( bpp );
    if( (sick == POKESICK_DOKU)
    ||  (sick == POKESICK_MAHI)
    ||  (sick == POKESICK_YAKEDO)
    ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �����؂�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Sippegaesi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Sippegaesi },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Sippegaesi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BTL_POKEPARAM_GetTurnFlag(bpp, BPP_TURNFLG_WAZA_EXE) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂ񂩁A�����ӂ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Funka( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Funka },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Funka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    fx32 r = BTL_POKEPARAM_GetHPRatio( bpp );
    u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );

    pow = BTL_CALC_MulRatio_OverZero( pow, r );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���ڂ�Ƃ�E�ɂ���Ԃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Siboritoru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Siboritoru },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Siboritoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    fx32 r = BTL_POKEPARAM_GetHPRatio( bpp );
    u32 pow = BTL_CALC_MulRatio_OverZero( 120, r );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����݂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Siomizu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Siomizu },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Siomizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    fx32 r = BTL_POKEPARAM_GetHPRatio( bpp );
    if( r <= FX32_CONST(50) )
    {
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      pow *= 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���񂪂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ongaesi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Ongaesi },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ongaesi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BTL_POKEPARAM_GetSrcData( bpp );
    u32 natsuki = PP_Get( pp, ID_PARA_friend, NULL );
    u32 pow = natsuki * 10 / 25;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * �������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yatuatari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Yatuatari },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Yatuatari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BTL_POKEPARAM_GetSrcData( bpp );
    u32 natsuki = PP_Get( pp, ID_PARA_friend, NULL );
    u32 pow = (255 - natsuki) * 10 / 25;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * �߂��܂��r���^
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MezamasiBinta( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_MezamasiBinta },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_AFTER_DAMAGE, handler_MezamasiBinta_AfterDamage },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MezamasiBinta( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_NEMURI) ){
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      pow *= 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
static void handler_MezamasiBinta_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
    if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_NEMURI) ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->pokeID[0] = target_pokeID;
      param->poke_cnt = 1;
      param->sickCode = WAZASICK_NEMURI;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kituke( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Kituke },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_AFTER_DAMAGE, handler_Kituke_AfterDamage },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_MAHI) ){
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      pow *= 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
static void handler_Kituke_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
    if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_MAHI) ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->pokeID[0] = target_pokeID;
      param->poke_cnt = 1;
      param->sickCode = WAZASICK_MAHI;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �v���[���g
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Present( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,            handler_Present_Pow },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,  handler_Present_Check }, // �_���[�W���U�񕜃`�F�b�N�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_FIX,    handler_Present_Fix },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Present_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 rand = GFL_STD_MtRand( 100 );
    u32 pow = 0;

    if( rand < 40 ){
      pow = 40;
    }else if( rand < 70 ){
      pow = 80;
    }else if( rand < 80 ){
      pow = 120;
    }

    if( pow ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }else{
      work[0] = 1;  // �_���[�W����Ȃ��񕜂������Ⴄ�t���O
    }
  }
}
static void handler_Present_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[0] == 1)
  ){
    work[1] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
static void handler_Present_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[1])
  ){
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );

    if( !BTL_POKEPARAM_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP*  param = BTL_SVFLOW_HANDLERWORK_Push( flowWk ,BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = target_pokeID;
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 4 );

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->pokeID = target_pokeID;
      msg_param->strID = BTL_STRID_SET_HP_Recover;
    }
    else
    {
      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->pokeID = target_pokeID;
      msg_param->strID = BTL_STRID_SET_NoEffect;
    }

  }

}

//----------------------------------------------------------------------------------
/**
 * ����ӂ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kirifuda( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Kirifuda },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kirifuda( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u16 powTbl[] = {
    0, 200, 80, 60, 50, 40,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 wazaIdx, pp;

    wazaIdx = BTL_POKEPARAM_GetWazaIdx( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    if( wazaIdx != PTL_WAZA_MAX ){
      pp = BTL_POKEPARAM_GetPP( bpp, wazaIdx );
    }else{
      GF_ASSERT(0);
      pp = NELEMS(powTbl) - 1;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, powTbl[pp] );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Osioki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Osioki },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Osioki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const BppValueID  statusTbl[] = {
    BPP_ATTACK_RANK, BPP_DEFENCE_RANK, BPP_SP_ATTACK_RANK, BPP_SP_DEFENCE_RANK, BPP_AGILITY,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    u32 pow, i;
    int rank, rankSum;

    rankSum = 0;
    for(i=0; i<NELEMS(statusTbl); ++i)
    {
      rank = BTL_POKEPARAM_GetValue( bpp, statusTbl[i] );
      if( rank > 0 ){ rankSum += rank; }
    }
    pow = 60 + rankSum * 20;
    if( pow > 200 ){
      pow = 200;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * �E�F�U�[�{�[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_WeatherBall( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM, handler_WeatherBall_Type },  // ���U�p�����[�^�`�F�b�N�n���h��
    { BTL_EVENT_WAZA_POWER, handler_WeatherBall_Pow },   // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_WeatherBall_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  {
    BtlWeather weather = BTL_FIELD_GetWeather();
    PokeType type = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    switch( weather ){
    case BTL_WEATHER_SHINE:  type = POKETYPE_HONOO; break;
    case BTL_WEATHER_RAIN:   type = POKETYPE_MIZU; break;
    case BTL_WEATHER_SAND:   type = POKETYPE_IWA; break;
    case BTL_WEATHER_SNOW:   type = POKETYPE_KOORI; break;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}
static void handler_WeatherBall_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) != POKETYPE_NORMAL)
  ){
    u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    pow *= 2;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���܂��E����������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tatumaki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Tatumaki_checkHide },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Tatumaki },     // �_���[�W�ŏI�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Tatumaki_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BPP_CONTFLG_SORAWOTOBU ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, FALSE );
    }
  }
}
static void handler_Tatumaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BTL_POKEPARAM_GetContFlag(target, BPP_CONTFLG_SORAWOTOBU) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �X�J�C�A�b�p�[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SkyUpper( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_SkyUpper },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SkyUpper( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BPP_CONTFLG_SORAWOTOBU ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, FALSE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���݂Ȃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kaminari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Kaminari_checkHide }, // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_EXCUSE_CALC_HIT,   handler_Kaminari_excuseHitCalc },  // �q�b�g�m���v�Z�X�L�b�v
    { BTL_EVENT_WAZA_HIT_RATIO,    handler_Kaminari_hitRatio },  // �_���[�W�ŏI�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kaminari_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BPP_CONTFLG_SORAWOTOBU ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, FALSE );
    }
  }
}
static void handler_Kaminari_excuseHitCalc( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_FIELD_GetWeather() == BTL_WEATHER_RAIN ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }
  }
}
static void handler_Kaminari_hitRatio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, 50 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Jisin( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Jisin_checkHide },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Jisin_damage },     // �_���[�W�ŏI�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Jisin_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BPP_CONTFLG_ANAWOHORU ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, FALSE );
    }
  }
}
static void handler_Jisin_damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BTL_POKEPARAM_GetContFlag(target, BPP_CONTFLG_ANAWOHORU) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �߂��߂�p���[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MezameruPower( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_MezameruPower_Type },  // �킴�p�����[�^�`�F�b�N
    { BTL_EVENT_WAZA_POWER,   handler_MezameruPower_Pow  },  // �킴�З̓`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MezameruPower_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u8 typeTbl[] = {
    POKETYPE_KAKUTOU, POKETYPE_HIKOU, POKETYPE_DOKU,   POKETYPE_JIMEN,
    POKETYPE_IWA,     POKETYPE_MUSHI, POKETYPE_GHOST,  POKETYPE_HAGANE,
    POKETYPE_HONOO,   POKETYPE_MIZU,  POKETYPE_KUSA,   POKETYPE_DENKI,
    POKETYPE_ESPER,   POKETYPE_KOORI, POKETYPE_DRAGON, POKETYPE_AKU,
  };
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BTL_POKEPARAM_GetSrcData( bpp );
    u32 val=0;
    if( PP_Get(pp, ID_PARA_hp_rnd, NULL) & 1){ val += 1; }
    if( PP_Get(pp, ID_PARA_pow_rnd, NULL) & 1){ val += 2; }
    if( PP_Get(pp, ID_PARA_def_rnd, NULL) & 1){ val += 4; }
    if( PP_Get(pp, ID_PARA_agi_rnd, NULL) & 1){ val += 8; }
    if( PP_Get(pp, ID_PARA_spepow_rnd, NULL) & 1){ val += 16; }
    if( PP_Get(pp, ID_PARA_spedef_rnd, NULL) & 1){ val += 32; }

    val = val * 15 / 63;
    while( val > NELEMS(typeTbl) ){ // �e�[�u���T�C�Y���z���邱�Ƃ͗L�蓾�Ȃ��n�Y�����O�̂���
      val -= NELEMS(typeTbl);
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, typeTbl[val] );
  }
}
static void handler_MezameruPower_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BTL_POKEPARAM_GetSrcData( bpp );
    u32 val=0;
    if( (PP_Get(pp, ID_PARA_hp_rnd, NULL) % 4)    > 1){ val += 1; }
    if( (PP_Get(pp, ID_PARA_pow_rnd, NULL) % 4)   > 1){ val += 2; }
    if( (PP_Get(pp, ID_PARA_def_rnd, NULL) % 4)   > 1){ val += 4; }
    if( (PP_Get(pp, ID_PARA_agi_rnd, NULL) % 4)   > 1){ val += 8; }
    if( (PP_Get(pp, ID_PARA_spepow_rnd, NULL) % 4)> 1){ val += 16; }
    if( (PP_Get(pp, ID_PARA_spedef_rnd, NULL) % 4)> 1){ val += 32; }

    val = 30 + (val * 40 / 63);
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, val );
  }

}

//----------------------------------------------------------------------------------
/**
 * �}�O�j�`���[�h
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Magnitude( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Jisin_checkHide },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Jisin_damage    },  // �_���[�W�ŏI�`�F�b�N
    { BTL_EVENT_WAZA_POWER,        handler_Magnitude_pow   },  // ���U�З͌���
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Magnitude_effect},  // ���ʊm��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Magnitude_pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const struct {
    u8  per;
    u8  pow;
  }powTbl[] = {
    {   5,  10 },
    {  15,  30 },
    {  35,  50 },
    {  65,  70 },
    {  85,  90 },
    {  95, 110 },
    { 100, 150 },
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 i, per = GFL_STD_MtRand( 100 );

    for(i=0; i<NELEMS(powTbl); ++i)
    {
      if( per < powTbl[i].per ){
        break;
      }
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, powTbl[i].pow );
    work[0] = i;
    BTL_Printf("�}�O�j�`���[�h idx=%d, �З͒l=%d\n", i, powTbl[i].pow);
  }
}
static void handler_Magnitude_effect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_STD, pokeID );
    param->strID = BTL_STRID_STD_Magnitude1 + work[0];
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ȃ݂̂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Naminori( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Naminori_checkHide },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Naminori },     // �_���[�W�ŏI�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Naminori_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BPP_CONTFLG_DIVING ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, FALSE );
    }
  }
}
static void handler_Naminori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BTL_POKEPARAM_GetContFlag(target, BPP_CONTFLG_DIVING) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂ݂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Fumituke( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Fumituke },     // �_���[�W�ŏI�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Fumituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BTL_POKEPARAM_GetContFlag(target, BPP_CONTFLG_TIISAKUNARU) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �݂˂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Mineuti( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK, handler_Mineuti },    // ���炦��`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Mineuti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BPP_KORAE_WAZA_ATTACKER );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���炦��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Koraeru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Koraeru_ExeCheck }, // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_KORAERU_CHECK,       handler_Koraeru },          // ���炦��`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Koraeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �����������񂾂񉺂���e�[�u�� 1/2, 1/4, 1/8, ---
  static const u8 randRange[] = {
    2, 4, 8,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 counter = BTL_POKEPARAM_GetSameWazaUsedCounter( bpp );

    if( counter )
    {
      if( counter >= NELEMS(randRange) ){
        counter = NELEMS(randRange) - 1;
      }
      if( GFL_STD_MtRand( randRange[counter] ) != 0 )
      {
        // �A�����p�ɂ�鎸�s�B���s������n���h�����Ə��ł��邱�ƂŁA���炦��`�F�b�N�ɔ������Ȃ��B
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
        BTL_EVENT_FACTOR_Remove( myHandle );
      }
    }
  }
}
static void handler_Koraeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BPP_KORAE_WAZA_DEFENDER );
  }
}
//----------------------------------------------------------------------------------
/**
 * �܂���E�݂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Mamoru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Mamoru_ExeCheck }, // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_UNCATEGORY_WAZA,     handler_Mamoru },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Mamoru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �����������񂾂񉺂���e�[�u�� 1/2, 1/4, 1/8, ---
  static const u8 randRange[] = {
    2, 4, 8,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 counter = BTL_POKEPARAM_GetSameWazaUsedCounter( bpp );

    if( counter )
    {
      if( counter >= NELEMS(randRange) ){
        counter = NELEMS(randRange) - 1;
      }
      if( GFL_STD_MtRand( randRange[counter] ) != 0 )
      {
        // �A�����p�ɂ�鎸�s�B���s������n���h�����Ə��ł��邱�ƂŁA���̌�̃C�x���g�ɔ������Ȃ��B
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
        BTL_EVENT_FACTOR_Remove( myHandle );
      }
    }
  }
}
static void handler_Mamoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_TURNFLAG* flagParam;

    flagParam = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_TURNFLAG, pokeID );
    flagParam->pokeID = pokeID;
    flagParam->flag = BPP_TURNFLG_MAMORU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �͂炾����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Haradaiko( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,     handler_Haradaiko },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Haradaiko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 downHP = BTL_POKEPARAM_GetValue( bpp, BPP_MAX_HP ) / 2;
    u32 upVolume = BTL_POKEPARAM_RankEffectUpLimit( bpp, BPP_ATTACK );
    if( (BTL_POKEPARAM_GetValue(bpp, BPP_HP) > downHP) && (upVolume != 0) )
    {
      BTL_HANDEX_PARAM_DAMAGE       *dmg_param;
      BTL_HANDEX_PARAM_RANK_EFFECT  *rank_param;
      BTL_HANDEX_PARAM_MESSAGE      *msg_param;

      dmg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
      dmg_param->poke_cnt = 1;
      dmg_param->pokeID[0] = pokeID;
      dmg_param->damage[0] = downHP;

      rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->rankType = BPP_ATTACK;
      rank_param->rankVolume = upVolume;
      rank_param->poke_cnt = 0;
      rank_param->pokeID[0] = pokeID;
      rank_param->fStdMsgDisable = TRUE;
      rank_param->fAlmost = TRUE;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->strID = BTL_STRID_SET_Haradaiko;
      msg_param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �t�F�C���g
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Feint( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,  handler_Feint_NoEffect },      // ���U�����`�F�b�NLv2
    { BTL_EVENT_CHECK_MAMORU_BREAK, handler_Feint_MamoruBreak },   // �܂��閳�����`�F�b�N
    { BTL_EVENT_AFTER_DAMAGE,       handler_Feint_AfterDamage },   // �_���[�W������
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Feint_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    if( !BTL_POKEPARAM_GetTurnFlag(bpp, BPP_TURNFLG_MAMORU) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
static void handler_Feint_MamoruBreak( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
static void handler_Feint_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );

    if( BTL_POKEPARAM_GetTurnFlag(bpp, BPP_TURNFLG_MAMORU) )
    {
      BTL_HANDEX_PARAM_TURNFLAG* flg_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      flg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
      flg_param->pokeID = target_pokeID;
      flg_param->flag = BPP_TURNFLG_MAMORU;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->pokeID = target_pokeID;
      msg_param->strID = BTL_STRID_SET_Feint;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ڂ���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TuboWoTuku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,     handler_TuboWoTuku },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_TuboWoTuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    static const BppValueID rankType[] = {
      BPP_ATTACK, BPP_DEFENCE, BPP_AGILITY, BPP_SP_ATTACK, BPP_SP_DEFENCE,
    };
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 valid_cnt, i;

    for(i=0, valid_cnt=0; i<NELEMS(rankType); ++i){
      if( BTL_POKEPARAM_IsRankEffectValid(bpp, rankType[i], 2) ){
        ++valid_cnt;
      }
    }

    if( valid_cnt )
    {
      u8 idx = GFL_STD_MtRand( valid_cnt );
      for(i=0; i<NELEMS(rankType); ++i)
      {
        if( BTL_POKEPARAM_IsRankEffectValid(bpp, rankType[i], 2) )
        {
          if( idx == 0 )
          {
            BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
            param->pokeID[0] = pokeID;
            param->poke_cnt = 1;
            param->rankType = rankType[i];
            param->rankVolume = 2;
            param->fAlmost = FALSE;
            break;
          }
          else
          {
            --idx;
          }
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �˂ނ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nemuru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Nemuru_exeCheck },
    { BTL_EVENT_UNCATEGORY_WAZA,     handler_Nemuru },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Nemuru_exeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    SV_WazaFailCause  cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );
    if( cause == SV_WAZAFAIL_NULL ){
      const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

      if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_KAIHUKUHUUJI );
      }
      if( BTL_POKEPARAM_IsHPFull(bpp) )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_HPFULL );
      }
      {
        u16 tok = BTL_POKEPARAM_GetValue( bpp, BPP_TOKUSEI );
        if( (tok == POKETOKUSEI_FUMIN) || (tok == POKETOKUSEI_YARUKI) ){
          BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_FUMIN );
        }
      }
    }
  }
}
static void handler_Nemuru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( !BTL_POKEPARAM_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP    *hp_param;
      BTL_HANDEX_PARAM_ADD_SICK      *sick_param;

      hp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      hp_param->pokeID = pokeID;
      hp_param->recoverHP = (BTL_POKEPARAM_GetValue(bpp, BPP_MAX_HP) - BTL_POKEPARAM_GetValue(bpp, BPP_HP));

      if( BTL_POKEPARAM_GetPokeSick(bpp) != POKESICK_NULL )
      {
        BTL_HANDEX_PARAM_CURE_SICK *cure_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        cure_param->poke_cnt = 1;
        cure_param->pokeID[0] = pokeID;
        cure_param->sickCode = WAZASICK_EX_POKEFULL;
        cure_param->fStdMsgDisable = TRUE;
      }

      sick_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      sick_param->poke_cnt = 1;
      sick_param->pokeID[0] = pokeID;
      sick_param->sickID = WAZASICK_NEMURI;
      sick_param->sickCont = BTL_CALC_MakeWazaSickCont_Turn( 3 );
      sick_param->fExMsg = TRUE;
      sick_param->exStrID = BTL_STRID_SET_Nemuru;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �A���R�[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Encore( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,     handler_Encore },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Encore( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BTL_POKEPARAM_CheckSick(target, WAZASICK_WAZALOCK)
    &&  (BTL_POKEPARAM_GetPrevWazaNumber(target) != WAZANO_NULL)
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      u8 turns = BTL_CALC_RandRange( 3, 7 );
      param->sickID = WAZASICK_WAZALOCK;
      param->sickCont = BTL_CALC_MakeWazaSickCont_Turn( turns );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->fAlmost = FALSE;
      param->fExMsg = TRUE;
      param->exStrID = BTL_STRID_SET_Encore;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���傤�͂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Chouhatu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,     handler_Chouhatu },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };

  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Chouhatu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BTL_POKEPARAM_CheckSick(target, WAZASICK_TYOUHATSU) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      u8 turns = BTL_CALC_RandRange( 3, 5 );
      param->sickID = WAZASICK_TYOUHATSU;
      param->sickCont = BTL_CALC_MakeWazaSickCont_Turn( turns );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->fAlmost = FALSE;
      param->fExMsg = TRUE;
      param->exStrID = BTL_STRID_SET_Chouhatu;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ichamon( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,     handler_Ichamon },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };

  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ichamon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BTL_POKEPARAM_CheckSick(target, WAZASICK_ICHAMON) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_ICHAMON;
      param->sickCont = BPP_SICKCONT_MakePermanent();
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->fAlmost = FALSE;
      param->fExMsg = TRUE;
      param->exStrID = BTL_STRID_SET_Ichamon;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂ�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Fuuin( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,     handler_Fuuin },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };

  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Fuuin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BTL_POKEPARAM_CheckSick(target, WAZASICK_ICHAMON) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_FUUIN;
      param->sickCont = BPP_SICKCONT_MakePermanent();
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->fAlmost = FALSE;
      param->fExMsg = TRUE;
      param->exStrID = BTL_STRID_SET_Fuuin;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �A���}�Z���s�[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Alomatherapy( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,  handler_Alomatherapy   },  // ���U�З͌���
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Alomatherapy( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_CureFriendPokeSick( myHandle, flowWk, pokeID, work, BTL_STRID_STD_AlomaTherapy );
}
//----------------------------------------------------------------------------------
/**
 * ���₵�̂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_IyasiNoSuzu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,  handler_IyasiNoSuzu   },  // ���U�З͌���
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_IyasiNoSuzu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_CureFriendPokeSick( myHandle, flowWk, pokeID, work, BTL_STRID_STD_IyasinoSuzu );
}

static void common_CureFriendPokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, u16 strID )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE   *msg_param;
    BTL_HANDEX_PARAM_CURE_SICK *cure_param;
    BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
    BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_MYSIDE_ALL, myPos );

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_STD, pokeID );
    msg_param->strID = strID;

    cure_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
    cure_param->poke_cnt = BTL_SERVERFLOW_RECEPT_GetTargetPokeID( flowWk, expos, cure_param->pokeID );
    cure_param->sickCode = WAZASICK_EX_POKEFULL;
  }
}
//----------------------------------------------------------------------------------
/**
 * �����݂₰
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Okimiyage( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,  handler_Okimiyage   },  // ���U�З͌���
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Okimiyage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* kill_param;
    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    kill_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    kill_param->pokeID = pokeID;

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = targetPokeID;
    rank_param->rankType = BPP_ATTACK;
    rank_param->rankVolume = -2;
    rank_param->fAlmost = TRUE;

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = targetPokeID;
    rank_param->rankType = BPP_SP_ATTACK;
    rank_param->rankVolume = -2;
    rank_param->fAlmost = TRUE;
  }
}
//----------------------------------------------------------------------------------
/**
 * �����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Urami( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,  handler_Urami   },  // ���U�З͌���
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Urami( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  enum {
    URAMI_DECREMENT_PP_VOLUME = 4,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );
    WazaID prev_waza = BTL_POKEPARAM_GetPrevWazaNumber( bpp );
    u8 wazaIdx = BTL_POKEPARAM_GetWazaIdx( bpp, prev_waza );

    if( wazaIdx != PTL_WAZA_MAX )
    {
      u8 volume = BTL_POKEPARAM_GetPP( bpp, wazaIdx );
      if( volume > URAMI_DECREMENT_PP_VOLUME ){ volume = URAMI_DECREMENT_PP_VOLUME; }
      if( volume )
      {
        BTL_HANDEX_PARAM_PP*      decpp_param;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        decpp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DECREMENT_PP, pokeID );
        decpp_param->pokeID = targetPokeID;
        decpp_param->volume = volume;
        decpp_param->wazaIdx = wazaIdx;

        msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
        msg_param->pokeID = targetPokeID;
        msg_param->strID = BTL_STRID_SET_Urami;
        msg_param->args[0] = prev_waza;
        msg_param->args[1] = volume;
        msg_param->arg_cnt = 2;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���b�N�I��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_LockON( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,  handler_LockON   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_LockON( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->sickID = WAZASICK_MUSTHIT_TARGET;
    param->sickCont = BPP_SICKCONT_Set_TurnParam( 2, targetPokeID );
    param->fExMsg = TRUE;
    param->exStrID = BTL_STRID_SET_LockOn;
    param->exStrArgs[0] = targetPokeID;
    param->exStrArgCnt = 1;
  }
}
//----------------------------------------------------------------------------------
/**
 * ���t���N�^�[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Refrector( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA_NO_TARGET,  handler_Refrector   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Refrector( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 5 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_REFRECTOR, cont, BTL_STRID_STD_Reflector );
}
//----------------------------------------------------------------------------------
/**
 * �Ђ���̂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HikariNoKabe( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA_NO_TARGET,  handler_HikariNoKabe   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 5 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, side, work, side, BTL_SIDEEFF_HIKARINOKABE, cont, BTL_STRID_STD_HikariNoKabe );
}
//----------------------------------------------------------------------------------
/**
 * ����҂̂܂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SinpiNoMamori( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA_NO_TARGET,  handler_SinpiNoMamori   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SinpiNoMamori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 5 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_SINPINOMAMORI, cont, BTL_STRID_STD_SinpiNoMamori );
}
//----------------------------------------------------------------------------------
/**
 * ���낢����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SiroiKiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA_NO_TARGET,  handler_SiroiKiri   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SiroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 5 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_SIROIKIRI, cont, BTL_STRID_STD_SiroiKiri );
}
//----------------------------------------------------------------------------------
/**
 * ��������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Oikaze( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA_NO_TARGET,  handler_Oikaze   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Oikaze( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 3 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_OIKAZE, cont, BTL_STRID_STD_Oikaze );
}
//----------------------------------------------------------------------------------
/**
 * ���܂��Ȃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Omajinai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA_NO_TARGET,  handler_Omajinai   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Omajinai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 5 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_OMAJINAI, cont, BTL_STRID_STD_Omajinai );
}
static void common_SideEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, BPP_SICK_CONT cont, u16 strID )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_HANDLER_SIDE_Add(side, effect, cont) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_STD, pokeID );
      param->strID = strID;
      param->args[0] = side;
      param->arg_cnt = 1;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ieki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,  handler_Ieki   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_TOKUSEI_CHANGE *tok_param;
    BTL_HANDEX_PARAM_MESSAGE        *msg_param;
    BTL_HANDEX_PARAM_ADD_SICK       *sick_param;
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    sick_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_TOKUSEI_CHANGE, pokeID );
    sick_param->poke_cnt = 1;
    sick_param->pokeID[0] = pokeID;
    sick_param->sickID = WAZASICK_IEKI;
    sick_param->sickCont = BPP_SICKCONT_MakePermanent();

    tok_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_TOKUSEI_CHANGE, pokeID );
    tok_param->pokeID = target_pokeID;
    tok_param->tokuseiID = POKETOKUSEI_NULL;

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
    msg_param->strID = BTL_STRID_SET_Ieki;
    msg_param->pokeID = target_pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ȃ肫��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Narikiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,  handler_Narikiri   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Narikiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_TOKUSEI_CHANGE*  tok_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    tok_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_TOKUSEI_CHANGE, pokeID );
    tok_param->pokeID = pokeID;
    tok_param->tokuseiID = BTL_POKEPARAM_GetValue( target, BPP_TOKUSEI );

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
    msg_param->strID = BTL_STRID_SET_Narikiri;
    msg_param->pokeID = pokeID;
    msg_param->args[0] = target_pokeID;
    msg_param->arg_cnt = 1;
  }
}



//----------------------------------------------------------------------------------
/**
 * �Ă�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tedasuke( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,    handler_Tedasuke_Ready },       // �����ރ��U�n���h��
    { BTL_EVENT_WAZA_POWER,         handler_Tedasuke_WazaPow },     // ���U�З̓`�F�b�N
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_Tedasuke_TurnCheck },   // �^�[���`�F�b�N�J�n�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Tedasuke_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_SVFLOW_GetRule(flowWk) != BTL_RULE_SINGLE )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );

      if( !BTL_POKEPARAM_IsDead(bpp) )
      {
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );

        param->strID = BTL_STRID_SET_Tedasuke;
        param->pokeID = pokeID;
        param->args[0] = target_pokeID;
        param->arg_cnt = 1;

        work[0] = target_pokeID;
        work[ WORKIDX_STICK ] = 1;
      }
    }
  }
}
static void handler_Tedasuke_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (work[WORKIDX_STICK] == 1)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK)==work[0]) )
  {
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5f) );
  }
}
static void handler_Tedasuke_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_EVENT_FACTOR_Remove( myHandle );  // �^�[���`�F�b�N�ŋ������E
}


//----------------------------------------------------------------------------------
/**
 * �˂����܂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nekodamasi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,       handler_Nekodamasi },     // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Nekodamasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BTL_POKEPARAM_GetTurnCount(bpp) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �����̂Ђ����E�������������E���̂Ђ���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_AsaNoHizasi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RECOVER_HP_RATIO,       handler_AsaNoHizasi },     // HP�񕜗��v�Z�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_AsaNoHizasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlWeather w = BTL_FIELD_GetWeather();
    u8 ratio = 50;
    switch( w ){
    case BTL_WEATHER_SHINE:  ratio = 66; break;
    case BTL_WEATHER_RAIN:
    case BTL_WEATHER_SAND:
    case BTL_WEATHER_SNOW:
      ratio = 25;
      break;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, ratio );
  }
}
//----------------------------------------------------------------------------------
/**
 * ������Ƃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SoraWoTobu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_SoraWoTobu_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,     handler_SoraWoTobu_TameRelease },   // ���߉��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SoraWoTobu_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->pokeID = pokeID;
      msg_param->arg_cnt = 0;
      msg_param->strID = BTL_STRID_SET_SoraWoTobu;
    }
  }
}
static void handler_SoraWoTobu_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �V���h�[�_�C�u
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_ShadowDive( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,         handler_ShadowDive_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,       handler_ShadowDive_TameRelease },   // ���߉��
    { BTL_EVENT_CHECK_MAMORU_BREAK, handler_Feint_MamoruBreak },        // �܂��閳�����`�F�b�N
    { BTL_EVENT_AFTER_DAMAGE,       handler_ShadowDive_AfterDamage },   // �_���[�W������
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_ShadowDive_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->pokeID = pokeID;
      msg_param->arg_cnt = 0;
      msg_param->strID = BTL_STRID_SET_ShadowDive;
    }
  }
}
static void handler_ShadowDive_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
  }
}
static void handler_ShadowDive_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );

    if( BTL_POKEPARAM_GetTurnFlag(bpp, BPP_TURNFLG_MAMORU) )
    {
      BTL_HANDEX_PARAM_TURNFLAG* flg_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      flg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
      flg_param->pokeID = target_pokeID;
      flg_param->flag = BPP_TURNFLG_MAMORU;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->pokeID = target_pokeID;
      msg_param->strID = BTL_STRID_SET_MamoruBreak;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �Ƃт͂˂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tobihaneru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Tobihaneru_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,     handler_Tobihaneru_TameRelease },   // ���߉��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Tobihaneru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->pokeID = pokeID;
      msg_param->arg_cnt = 0;
      msg_param->strID = BTL_STRID_SET_Tobihaneru;
    }
  }
}
static void handler_Tobihaneru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �_�C�r���O
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Diving( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Diving_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,     handler_Diving_TameRelease },   // ���߉��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Diving_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_DIVING;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->pokeID = pokeID;
      msg_param->arg_cnt = 0;
      msg_param->strID = BTL_STRID_SET_Diving;
    }
  }
}
static void handler_Diving_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_DIVING;
  }
}
//----------------------------------------------------------------------------------
/**
 * ���Ȃ��ق�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_AnaWoHoru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_AnaWoHoru_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,     handler_AnaWoHoru_TameRelease },   // ���߉��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_AnaWoHoru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_ANAWOHORU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->pokeID = pokeID;
      msg_param->arg_cnt = 0;
      msg_param->strID = BTL_STRID_SET_AnaWoHoru;
    }
  }
}
static void handler_AnaWoHoru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_ANAWOHORU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �\�[���[�r�[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SolarBeam( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_TAMETURN_SKIP,  handler_SolarBeam_TameSkip },    // ���߃X�L�b�v����
    { BTL_EVENT_TAME_START,           handler_SolarBeam_TameStart },   // ���ߊJ�n
    { BTL_EVENT_WAZA_POWER,           handler_SolarBeam_Power     },   // ���U�З͌���
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SolarBeam_TameSkip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }
  }
}
static void handler_SolarBeam_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
    msg_param->pokeID = pokeID;
    msg_param->arg_cnt = 0;
    msg_param->strID = BTL_STRID_SET_SolarBeam;
  }
}
static void handler_SolarBeam_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  ( BTL_FIELD_GetWeather() == BTL_WEATHER_RAIN )
  ){
    u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    pow /= 2;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}

//----------------------------------------------------------------------------------
/**
 * ���܂�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kamaitati( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Kamaitati_TameStart },     // ���ߊJ�n
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kamaitati_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
    msg_param->pokeID = pokeID;
    msg_param->arg_cnt = 0;
    msg_param->strID = BTL_STRID_SET_Kamaitati;
  }
}
//----------------------------------------------------------------------------------
/**
 * �S�b�h�o�[�h
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_GodBird( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_GodBird_TameStart },     // ���ߊJ�n
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_GodBird_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
    msg_param->pokeID = pokeID;
    msg_param->arg_cnt = 0;
    msg_param->strID = BTL_STRID_SET_GodBird;
  }
}
//----------------------------------------------------------------------------------
/**
 * ���P�b�g����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_RocketZutuki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_RocketZutuki_TameStart },     // ���ߊJ�n
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_RocketZutuki_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
    msg_param->pokeID = pokeID;
    msg_param->arg_cnt = 0;
    msg_param->strID = BTL_STRID_SET_RocketZutuki;

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = pokeID;
    rank_param->rankType = WAZA_RANKEFF_DEFENCE;
    rank_param->rankVolume = 1;
    rank_param->fAlmost = TRUE;
  }
}
//------------------------------------------------------------------------------
/**
 *  ��邠����
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Waruagaki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_AFTER_DAMAGE,  handler_Waruagaki },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, waza, pri, pokeID, HandlerTable );
}
static void handler_Waruagaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->damage[0] = BTL_CALC_QuotMaxHP( bpp, 4 );
  }
}
//----------------------------------------------------------------------------------
/**
 * �΂�������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Bakajikara( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_AFTER_DAMAGE,       handler_Bakajikara },   // �_���[�W������
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Bakajikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->rankType = WAZA_RANKEFF_ATTACK;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->rankType = WAZA_RANKEFF_DEFENCE;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂��Â�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Michidure( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,    handler_Michidure_Ready },       // �����ރ��U�n���h��
    { BTL_EVENT_WAZA_DMG_AFTER,     handler_Michidure_WazaDamage },  // ���U�_���[�W������
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_Michidure_TurnCheck },   // �^�[���`�F�b�N�J�n�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Michidure_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );

    param->strID = BTL_STRID_SET_MichidureReady;
    param->pokeID = pokeID;
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_Michidure_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BTL_POKEPARAM_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_KILL* kill_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      kill_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_KILL, pokeID );
      kill_param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->strID = BTL_STRID_SET_MichidureDone;
      msg_param->pokeID = pokeID;

      BTL_EVENT_FACTOR_Remove( myHandle );  // ���A�ꐬ���Ŏ��E
    }
  }
}
static void handler_Michidure_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_EVENT_FACTOR_Remove( myHandle );  // �^�[���`�F�b�N�ŋ������E
}
//----------------------------------------------------------------------------------
/**
 * ����˂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Onnen( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA,    handler_Onnen_Ready },       // �����ރ��U�n���h��
    { BTL_EVENT_WAZA_DMG_AFTER,     handler_Onnen_WazaDamage },  // ���U�_���[�W������
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_Onnen_TurnCheck },   // �^�[���`�F�b�N�J�n�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Onnen_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );

    param->strID = BTL_STRID_SET_OnnenReady;
    param->pokeID = pokeID;
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_Onnen_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BTL_POKEPARAM_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_PP* pp_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      WazaID  waza;

      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      const BTL_POKEPARAM* targetPoke = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );

      waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );

      pp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DECREMENT_PP, pokeID );
      pp_param->pokeID = target_pokeID;
      pp_param->wazaIdx = BTL_POKEPARAM_GetWazaIdx( targetPoke, waza );
      pp_param->volume = BTL_POKEPARAM_GetPP( targetPoke, pp_param->wazaIdx ) * -1;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
      msg_param->strID = BTL_STRID_SET_OnnenDone;
      msg_param->pokeID = target_pokeID;
      msg_param->args[0] = waza;
      msg_param->arg_cnt = 1;

      BTL_EVENT_FACTOR_Remove( myHandle );  // ���A�ꐬ���Ŏ��E
    }
  }
}
static void handler_Onnen_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_EVENT_FACTOR_Remove( myHandle );  // �^�[���`�F�b�N�ŋ������E
}

//----------------------------------------------------------------------------------
/**
 * ���������Ȃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tiisakunaru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Tiisakunaru   },  // ���U�З͌���
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Tiisakunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_TIISAKUNARU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �܂邭�Ȃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Marukunaru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Marukunaru   },  // ���U�З͌���
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Marukunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_MARUKUNARU;
  }
}
