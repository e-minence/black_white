//=============================================================================================
/**
 * @file  hand_waza.c
 * @brief ポケモンWB バトルシステム イベントファクター [ワザ] 処理
 * @author  taya
 *
 * @date  2009.05.15  作成
 */
//=============================================================================================

#include "poke_tool\poketype.h"
#include "poke_tool\monsno_def.h"
#include "waza_tool\wazano_def.h"
#include "item\itemsym.h"
#include "item\item.h"

#include "..\btl_common.h"
#include "..\btl_calc.h"
#include "..\btl_field.h"
#include "..\btl_client.h"
#include "..\btl_event_factor.h"

#include "hand_side.h"
#include "hand_field.h"
#include "hand_waza.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_STICK = EVENT_HANDLER_WORK_ELEMS - 1,
};



/* @todo 新ワザ名シンボルを一時的に手打ちしておく */
enum {
  WAZANO_KARI_TUMETOGI = 468,
  WAZANO_KARI_WAIDOGAADO,
  WAZANO_KARI_GAADOSHEA,
  WAZANO_KARI_PAWAASHEA,
  WAZANO_KARI_WANDAARUUMU,
  WAZANO_KARI_SAIKOSYOKKU,
  WAZANO_KARI_BENOMUSHOKKU,
  WAZANO_KARI_BODYPAAZI,
  WAZANO_KARI_IKARINOKONA,
  WAZANO_KARI_TEREKINESISU,
  WAZANO_KARI_MAZIKKURUUMU,
  WAZANO_KARI_UTIOTOSU,
  WAZANO_KARI_GORINKUDAKI,
  WAZANO_KARI_HAZIKERUHONOO,
  WAZANO_KARI_HEDOROWHEEBU,
  WAZANO_KARI_TYOUNOMAI,
  WAZANO_KARI_HEBIIBONBAA,
  WAZANO_KARI_SINKURONOIZU,
  WAZANO_KARI_EREKUTOBOORU,
  WAZANO_KARI_MIZUBITASI,
  WAZANO_KARI_NITOROTYAAZI,
  WAZANO_KARI_TOGUROWOMAKU,
  WAZANO_KARI_ROOKIKKU,
  WAZANO_KARI_ASSIDOBOMU,
  WAZANO_KARI_IKASAMA,
  WAZANO_KARI_SINPURUBIIMU,
  WAZANO_KARI_NAKAMADUKURI,
  WAZANO_KARI_OSAKINIDOUZO,
  WAZANO_KARI_RINSYOU,
  WAZANO_KARI_EKOOBOISU,
  WAZANO_KARI_NASIKUZUSI,
  WAZANO_KARI_KURIASUMOGGU,
  WAZANO_KARI_ASISUTOPAWAA,
  WAZANO_KARI_FASTOGAADO,
  WAZANO_KARI_SAIDOCHENZI,
  WAZANO_KARI_NETTOU,
  WAZANO_KARI_KARAWOYABURU,
  WAZANO_KARI_IYASINOHADOU,
  WAZANO_KARI_TATARIME,
  WAZANO_KARI_FURIIFOORU,
  WAZANO_KARI_GIACHENZI,
  WAZANO_KARI_TOMOENAGE,
  WAZANO_KARI_YAKITUKUSU,
  WAZANO_KARI_SAKIOKURI,
  WAZANO_KARI_AKUROBATTO,
  WAZANO_KARI_MIRAATAIPU,
  WAZANO_KARI_KATAKIUTI,
  WAZANO_KARI_INOTIGAKE,
  WAZANO_KARI_GIHUTOPASU,
  WAZANO_KARI_RENGOKU,
  WAZANO_KARI_MIZUNOTIKAI,
  WAZANO_KARI_HONOONOTIKAI,
  WAZANO_KARI_KUSANOTIKAI,
  WAZANO_KARI_BORUTOCHENZI,
  WAZANO_KARI_MUSINOTEIKOU,
  WAZANO_KARI_ZINARASI,
  WAZANO_KARI_KOORINOIBUKI,
  WAZANO_KARI_DORAGONTEERU,
  WAZANO_KARI_HURUITATERU,
  WAZANO_KARI_EREKINETTO,
  WAZANO_KARI_WAIRUDOBORUTO,
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL is_registable( u8 pokeID, WazaID waza, u8* fRegistered );
static void removeHandlerForce( u8 pokeID, WazaID waza );
static BTL_EVENT_FACTOR*  ADD_Texture( u16 pri, WazaID waza, u8 pokeID );
static void handler_Texture( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Hogosyoku( u16 pri, WazaID waza, u8 pokeID );
static void handler_Hogosyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_TrickRoom( u16 pri, WazaID waza, u8 pokeID );
static void handler_TrickRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Juryoku( u16 pri, WazaID waza, u8 pokeID );
static void handler_Juryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MizuAsobi( u16 pri, WazaID waza, u8 pokeID );
static void handler_MizuAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_DoroAsobi( u16 pri, WazaID waza, u8 pokeID );
static void handler_DoroAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Kiribarai( u16 pri, WazaID waza, u8 pokeID );
static void handler_Kiribarai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Kawarawari( u16 pri, WazaID waza, u8 pokeID );
static void handler_Kawarawari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Tobigeri( u16 pri, WazaID waza, u8 pokeID );
static void handler_Tobigeri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Monomane( u16 pri, WazaID waza, u8 pokeID );
static void handler_Monomane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Sketch( u16 pri, WazaID waza, u8 pokeID );
static void handler_Sketch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KonoyubiTomare( u16 pri, WazaID waza, u8 pokeID );
static void handler_KonoyubiTomare_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KonoyubiTomare_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KonoyubiTomare_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KonoyubiTomare_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Refresh( u16 pri, WazaID waza, u8 pokeID );
static void handler_Refresh( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KumoNoSu( u16 pri, WazaID waza, u8 pokeID );
static void handler_KumoNoSu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KuroiKiri( u16 pri, WazaID waza, u8 pokeID );
static void handler_KuroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Haneru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Haneru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Noroi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Noroi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_NayamiNoTane( u16 pri, WazaID waza, u8 pokeID );
static void handler_NayamiNoTane_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NayamiNoTane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Yumekui( u16 pri, WazaID waza, u8 pokeID );
static void handler_Yumekui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Yuwaku( u16 pri, WazaID waza, u8 pokeID );
static void handler_Yuwaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_TriAttack( u16 pri, WazaID waza, u8 pokeID );
static void handler_TriAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_HimituNoTikara( u16 pri, WazaID waza, u8 pokeID );
static void handler_HimituNoTikara_Sick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Osyaberi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Osyaberi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Makituku( u16 pri, WazaID waza, u8 pokeID );
static void handler_Makituku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Uzusio( u16 pri, WazaID waza, u8 pokeID );
static void handler_Uzusio_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR*  ADD_Takuwaeru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Takuwaeru_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Takuwaeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Hakidasu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Hakidasu_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hakidasu_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hakidasu_Done( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Nomikomu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Nomikomu_Ratio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Counter( u16 pri, WazaID waza, u8 pokeID );
static void handler_Counter_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Counter_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Counter_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MilerCoat( u16 pri, WazaID waza, u8 pokeID );
static void handler_MilerCoat_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MilerCoat_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MilerCoat_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MetalBurst( u16 pri, WazaID waza, u8 pokeID );
static void handler_MetalBurst_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MetalBurst_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MetalBurst_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_Counter_ExeCheck( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType );
static void common_Counter_SetTarget( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType );
static void common_Counter_CalcDamage( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType );
static BOOL common_Counter_GetRec( const BTL_POKEPARAM* bpp, WazaDamageType dmgType, BPP_WAZADMG_REC* rec );
static BTL_EVENT_FACTOR*  ADD_Totteoki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Totteoki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ibiki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ibiki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Akumu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Akumu_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Fuiuti( u16 pri, WazaID waza, u8 pokeID );
static void handler_Fuiuti_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Oiuti( u16 pri, WazaID waza, u8 pokeID );
static void handler_Oiuti_Intr( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Oiuti_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Daibakuhatsu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Daibakuhatsu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Daibakuhatsu_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Juden( u16 pri, WazaID waza, u8 pokeID );
static void handler_Juden_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Juden_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_HorobiNoUta( u16 pri, WazaID waza, u8 pokeID );
static void handler_HorobiNoUta_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_YadorigiNoTane( u16 pri, WazaID waza, u8 pokeID );
static void handler_YadorigiNoTane_Param( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_NekoNiKoban( u16 pri, WazaID waza, u8 pokeID );
static void handler_NekoNiKoban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ikari( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ikari_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Ikari_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_AquaRing( u16 pri, WazaID waza, u8 pokeID );
static void handler_AquaRing( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_AquaRing_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Abareru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Abareru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Sawagu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Sawagu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sawagu_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sawagu_CheckSickFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Korogaru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Korogaru_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Korogaru_ExeFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Korogaru_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_TripleKick( u16 pri, WazaID waza, u8 pokeID );
static void handler_TripleKick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_GyroBall( u16 pri, WazaID waza, u8 pokeID );
static void handler_GyroBall( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Revenge( u16 pri, WazaID waza, u8 pokeID );
static void handler_Revenge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR*  ADD_RenzokuGiri( u16 pri, WazaID waza, u8 pokeID );
static void handler_RenzokuGiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Dameosi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Dameosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ketaguri( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ketaguri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR*  ADD_SabakiNoTubute( u16 pri, WazaID waza, u8 pokeID );
static void handler_SabakiNoTubute( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MezameruPower( u16 pri, WazaID waza, u8 pokeID );
static void handler_MezameruPower_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MezameruPower_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SizenNoMegumi( u16 pri, WazaID waza, u8 pokeID );
static void handler_SizenNoMegumi_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenNoMegumi_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenNoMegumi_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenNoMegumi_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Hatakiotosu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Hatakiotosu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MagicCoat( u16 pri, WazaID waza, u8 pokeID );
static void handler_MagicCoat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagicCoat_Rob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagicCoat_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Yokodori( u16 pri, WazaID waza, u8 pokeID );
static void handler_Yokodori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Yokodori_Rob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Yokodori_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Dorobou( u16 pri, WazaID waza, u8 pokeID );
static void handler_Dorobou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Trick( u16 pri, WazaID waza, u8 pokeID );
static void handler_Trick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR*  ADD_Gaman( u16 pri, WazaID waza, u8 pokeID );
static void handler_Gaman( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gaman_WazaMsg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gaman_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gaman_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gamen_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gaman_Fail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void gaman_ReleaseStick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static u32 gaman_getTotalDamage( const BTL_POKEPARAM* bpp );
static BTL_EVENT_FACTOR*  ADD_Recycle( u16 pri, WazaID waza, u8 pokeID );
static void handler_Recycle( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_PsycoShift( u16 pri, WazaID waza, u8 pokeID );
static void handler_PsycoShift( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Itamiwake( u16 pri, WazaID waza, u8 pokeID );
static void handler_Itamiwake( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR*  ADD_Meromero( u16 pri, WazaID waza, u8 pokeID );
static void handler_Meromero_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Texture2( u16 pri, WazaID waza, u8 pokeID );
static void handler_Texture2_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Texture2( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Encore( u16 pri, WazaID waza, u8 pokeID );
static void handler_Encore( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Chouhatu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Chouhatu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ichamon( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ichamon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Kanasibari( u16 pri, WazaID waza, u8 pokeID );
static void handler_Kanasibari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR*  ADD_JikoAnji( u16 pri, WazaID waza, u8 pokeID );
static void handler_JikoAnji( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_HeartSwap( u16 pri, WazaID waza, u8 pokeID );
static void handler_HeartSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_PowerSwap( u16 pri, WazaID waza, u8 pokeID );
static void handler_PowerSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_GuardSwap( u16 pri, WazaID waza, u8 pokeID );
static void handler_GuardSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_PowerTrick( u16 pri, WazaID waza, u8 pokeID );
static void handler_PowerTrick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_PowerShare( u16 pri, WazaID waza, u8 pokeID );
static void handler_PowerShare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_GuardShare( u16 pri, WazaID waza, u8 pokeID );
static void handler_GuardShare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR*  ADD_Makibisi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Makibisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Dokubisi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Dokubisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_StealthRock( u16 pri, WazaID waza, u8 pokeID );
static void handler_StealthRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_WideGuard( u16 pri, WazaID waza, u8 pokeID );
static void handler_WideGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_SideEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, BPP_SICK_CONT cont, u16 strID );
static BTL_EVENT_FACTOR*  ADD_MikadukiNoMai( u16 pri, WazaID waza, u8 pokeID );
static void handler_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_IyasiNoNegai( u16 pri, WazaID waza, u8 pokeID );
static void handler_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Negaigoto( u16 pri, WazaID waza, u8 pokeID );
static void handler_Negaigoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Miraiyoti( u16 pri, WazaID waza, u8 pokeID );
static void handler_Miraiyoti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_HametuNoNegai( u16 pri, WazaID waza, u8 pokeID );
static void handler_HametuNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ieki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Narikiri( u16 pri, WazaID waza, u8 pokeID );
static void handler_Narikiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_TonboGaeri( u16 pri, WazaID waza, u8 pokeID );
static void handler_TonboGaeri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KousokuSpin( u16 pri, WazaID waza, u8 pokeID );
static void handler_KousokuSpin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_BatonTouch( u16 pri, WazaID waza, u8 pokeID );
static void handler_BatonTouch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Teleport( u16 pri, WazaID waza, u8 pokeID );
static void handler_Teleport_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Teleport( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Teleport_ExMsg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Nagetukeru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Nagetukeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nagetukeru_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nagetukeru_WazaPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nagetukeru_DmgAfter( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_DenjiFuyuu( u16 pri, WazaID waza, u8 pokeID );
static void handler_DenjiFuyuu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Tedasuke( u16 pri, WazaID waza, u8 pokeID );
static void handler_Tedasuke_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tedasuke_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tedasuke_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_FukuroDataki( u16 pri, WazaID waza, u8 pokeID );
static void handler_FukuroDataki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR* ADD_Tuibamu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Tuibamu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR*  ADD_Haneyasume( u16 pri, WazaID waza, u8 pokeID );
static void handler_Haneyasume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KiaiPunch( u16 pri, WazaID waza, u8 pokeID );
static void handler_KiaiPunch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL isReqWazaExclude( WazaID waza );
static BTL_EVENT_FACTOR*  ADD_YubiWoFuru( u16 pri, WazaID waza, u8 pokeID );
static void handler_YubiWoFuru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_YubiWoFuru_Msg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SizenNoTikara( u16 pri, WazaID waza, u8 pokeID );
static void handler_SizenNoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenNoTikara_Msg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Nekonote( u16 pri, WazaID waza, u8 pokeID );
static void handler_Nekonote( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nekonote_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Negoto( u16 pri, WazaID waza, u8 pokeID );
static void handler_Negoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Negoto_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_OumuGaesi( u16 pri, WazaID waza, u8 pokeID );
static void handler_OumuGaesi_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Sakidori( u16 pri, WazaID waza, u8 pokeID );
static void handler_Sakidori_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sakidori_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Manekko( u16 pri, WazaID waza, u8 pokeID );
static void handler_Manekko_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_BenomShock( u16 pri, WazaID waza, u8 pokeID );
static void handler_BenomShock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Tatarime( u16 pri, WazaID waza, u8 pokeID );
static void handler_Tatarime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Acrobat( u16 pri, WazaID waza, u8 pokeID );
static void handler_Acrobat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_AsistPower( u16 pri, WazaID waza, u8 pokeID );
static void handler_AsistPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_HeavyBomber( u16 pri, WazaID waza, u8 pokeID );
static void handler_HeavyBomber( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_ElectBall( u16 pri, WazaID waza, u8 pokeID );
static void handler_ElectBall( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_EchoVoice( u16 pri, WazaID waza, u8 pokeID );
static void handler_EchoVoice( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ikasama( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ikasama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Mizubitasi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Mizubitasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SimpleBeem( u16 pri, WazaID waza, u8 pokeID );
static void handler_SimpleBeem( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_NakamaDukuri( u16 pri, WazaID waza, u8 pokeID );
static void handler_NakamaDukuri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_ClearSmog( u16 pri, WazaID waza, u8 pokeID );
static void handler_ClearSmog( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KarawoYaburu( u16 pri, WazaID waza, u8 pokeID );
static void handler_KarawoYaburu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MirrorType( u16 pri, WazaID waza, u8 pokeID );
static void handler_MirrorType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_BodyPurge( u16 pri, WazaID waza, u8 pokeID );
static void handler_BodyPurge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_PsycoShock( u16 pri, WazaID waza, u8 pokeID );
static void handler_PsycoShock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_NasiKuzusi( u16 pri, WazaID waza, u8 pokeID );
static void handler_NasiKuzusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_WonderRoom( u16 pri, WazaID waza, u8 pokeID );
static void handler_WonderRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MagicRoom( u16 pri, WazaID waza, u8 pokeID );
static void handler_MagicRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_HajikeruHonoo( u16 pri, WazaID waza, u8 pokeID );
static void handler_HajikeruHonoo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SyncroNoise( u16 pri, WazaID waza, u8 pokeID );
static void handler_SyncroNoise( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Yakitukusu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Yakitukusu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_GiftPass( u16 pri, WazaID waza, u8 pokeID );
static void handler_GiftPass( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );


//=============================================================================================
/**
 * ポケモンの「ワザ」ハンドラをシステムに追加
 *
 * @param   pp
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BOOL  BTL_HANDLER_Waza_Add( const BTL_POKEPARAM* pp, WazaID waza )
{
  typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, WazaID wazaID, u8 pokeID );

  static const struct {
    WazaID         waza;
    pEventAddFunc  func;
  }funcTbl[] = {
    { WAZANO_TEKUSUTYAA,      ADD_Texture       },
    { WAZANO_HOGOSYOKU,       ADD_Hogosyoku     },
    { WAZANO_KUROIKIRI,       ADD_KuroiKiri     },
    { WAZANO_YUMEKUI,         ADD_Yumekui       },
    { WAZANO_TORAIATAKKU,     ADD_TriAttack     },
    { WAZANO_HIMITUNOTIKARA,  ADD_HimituNoTikara},
    { WAZANO_OSYABERI,        ADD_Osyaberi      },
    { WAZANO_IKARINOMAEBA,    ADD_IkariNoMaeba  },
    { WAZANO_RYUUNOIKARI,     ADD_RyuuNoIkari   },
    { WAZANO_TIKYUUNAGE,      ADD_TikyuuNage    },
    { WAZANO_NAITOHEDDO,      ADD_TikyuuNage    },  // ナイトヘッド=ちきゅうなげと等価
    { WAZANO_SAIKOWHEEBU,     ADD_PsycoWave     },
    { WAZANO_IBIKI,           ADD_Ibiki         },
    { WAZANO_TOTTEOKI,        ADD_Totteoki      },
    { WAZANO_ZITABATA,        ADD_Jitabata      },
    { WAZANO_KISIKAISEI,      ADD_Jitabata      },  // きしかいせい=じたばた と等価
    { WAZANO_MINEUTI,         ADD_Mineuti       },
    { WAZANO_KUMONOSU,        ADD_KumoNoSu      },
    { WAZANO_KUROIMANAZASI,   ADD_KumoNoSu      },  // くろいまなざし = くものす と等価
    { WAZANO_TOOSENBOU,       ADD_KumoNoSu      },  // とおせんぼう  = くものす と等価
    { WAZANO_KORAERU,         ADD_Koraeru       },
    { WAZANO_SONIKKUBUUMU,    ADD_SonicBoom     },
    { WAZANO_NEKODAMASI,      ADD_Nekodamasi    },
    { WAZANO_GAMUSYARA,       ADD_Gamusyara     },
    { WAZANO_HUNKA,           ADD_Funka         },
    { WAZANO_SIOHUKI,         ADD_Funka         },  // しおふき=ふんか と等価
    { WAZANO_RIHURESSYU,      ADD_Refresh       },
    { WAZANO_ASANOHIZASI,     ADD_AsaNoHizasi   },
    { WAZANO_TUKINOHIKARI,    ADD_AsaNoHizasi   },  // つきのひかり = あさのひざし と等価
    { WAZANO_KOUGOUSEI,       ADD_AsaNoHizasi   },  // こうごうせい = あさのひざし と等価
    { WAZANO_SIBORITORU,      ADD_Siboritoru    },
    { WAZANO_NIGIRITUBUSU,    ADD_Siboritoru    },  // にぎりつぶす = しぼりとる と等価
    { WAZANO_WHEZAABOORU,     ADD_WeatherBall   },
    { WAZANO_MAMORU,          ADD_Mamoru        },
    { WAZANO_MIKIRI,          ADD_Mamoru        },  // みきり = まもる と等価
    { WAZANO_HANERU,          ADD_Haneru        },
    { WAZANO_NOROI,           ADD_Noroi         },
    { WAZANO_ABARERU,         ADD_Abareru       },
    { WAZANO_SAWAGU,          ADD_Sawagu        },
    { WAZANO_HANABIRANOMAI,   ADD_Abareru       },  // はなびらのまい = あばれる と等価
    { WAZANO_GEKIRIN,         ADD_Abareru       },  // げきりん = あばれる と等価
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
    { WAZANO_HUIUTI,          ADD_Fuiuti        },
    { WAZANO_KOROGARU,        ADD_Korogaru      },
    { WAZANO_AISUBOORU,       ADD_Korogaru      },  // アイスボール=ころがる と等価
    { WAZANO_AROMASERAPII,    ADD_Alomatherapy  },
    { WAZANO_IYASINOSUZU,     ADD_IyasiNoSuzu   },
    { WAZANO_OKIMIYAGE,       ADD_Okimiyage     },
    { WAZANO_URAMI,           ADD_Urami         },
    { WAZANO_NEMURU,          ADD_Nemuru        },
    { WAZANO_ROKKUON,         ADD_LockON        },
    { WAZANO_KOKORONOME,      ADD_LockON        },  // こころのめ=ロックオンと等価
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
    { WAZANO_ZYUURYOKU,       ADD_Juryoku       },
    { WAZANO_ONNEN,           ADD_Onnen         },
    { WAZANO_TEDASUKE,        ADD_Tedasuke      },
    { WAZANO_IEKI,            ADD_Ieki          },
    { WAZANO_NARIKIRI,        ADD_Narikiri      },
    { WAZANO_MAKIBISI,        ADD_Makibisi      },
    { WAZANO_DOKUBISI,        ADD_Dokubisi      },
    { WAZANO_SUTERUSUROKKU,   ADD_StealthRock   },
    { WAZANO_HANEYASUME,      ADD_Haneyasume    },
    { WAZANO_DENZIHUYUU,      ADD_DenjiFuyuu    },
    { WAZANO_RENZOKUGIRI,     ADD_RenzokuGiri   },
    { WAZANO_SAIKOSIHUTO,     ADD_PsycoShift    },
    { WAZANO_DAMEOSI,         ADD_Dameosi       },
    { WAZANO_TEKUSUTYAA2,     ADD_Texture2      },
    { WAZANO_KAUNTAA,         ADD_Counter       },
    { WAZANO_MIRAAKOOTO,      ADD_MilerCoat     },
    { WAZANO_METARUBAASUTO,   ADD_MetalBurst    },
    { WAZANO_RIBENZI,         ADD_Revenge       },
    { WAZANO_YUKINADARE,      ADD_Revenge       },  // ゆきなだれ=リベンジと等価
    { WAZANO_TORIPURUKIKKU,   ADD_TripleKick    },
    { WAZANO_ZYAIROBOORU,     ADD_GyroBall      },
    { WAZANO_ITAMIWAKE,       ADD_Itamiwake     },
    { WAZANO_KONOYUBITOMARE,  ADD_KonoyubiTomare},
    { WAZANO_NAYAMINOTANE,    ADD_NayamiNoTane  },
    { WAZANO_ZIKOANZI,        ADD_JikoAnji      },
    { WAZANO_HAATOSUWAPPU,    ADD_HeartSwap     },
    { WAZANO_PAWAASUWAPPU,    ADD_PowerSwap     },
    { WAZANO_GAADOSUWAPPU,    ADD_GuardSwap     },
    { WAZANO_MEROMERO,        ADD_Meromero      },
    { WAZANO_SABAKINOTUBUTE,  ADD_SabakiNoTubute},
    { WAZANO_SIZENNOMEGUMI,   ADD_SizenNoMegumi },
    { WAZANO_HATAKIOTOSU,     ADD_Hatakiotosu   },
    { WAZANO_KANASIBARI,      ADD_Kanasibari    },
    { WAZANO_DOROBOU,         ADD_Dorobou       },
    { WAZANO_HOSIGARU,        ADD_Dorobou       },  // ほしがる=どろぼう と等価
    { WAZANO_TORIKKU,         ADD_Trick         },
    { WAZANO_SURIKAE,         ADD_Trick         },  // すりかえ=トリックと等価
    { WAZANO_MONOMANE,        ADD_Monomane      },
    { WAZANO_SUKETTI,         ADD_Sketch        },
    { WAZANO_TOBIGERI,        ADD_Tobigeri      },
    { WAZANO_TOBIHIZAGERI,    ADD_Tobigeri      },
    { WAZANO_KIRIBARAI,       ADD_Kiribarai     },
    { WAZANO_KAWARAWARI,      ADD_Kawarawari    },
    { WAZANO_TORIKKURUUMU,    ADD_TrickRoom     },
    { WAZANO_MIZUASOBI,       ADD_MizuAsobi     },
    { WAZANO_DOROASOBI,       ADD_DoroAsobi     },
    { WAZANO_ZYUUDEN,         ADD_Juden         },
    { WAZANO_HOROBINOUTA,     ADD_HorobiNoUta   },
    { WAZANO_YADORIGINOTANE,  ADD_YadorigiNoTane},
    { WAZANO_HUKURODATAKI,    ADD_FukuroDataki  },
    { WAZANO_AKUARINGU,       ADD_AquaRing      },
    { WAZANO_MIKADUKINOMAI,   ADD_MikadukiNoMai },
    { WAZANO_IYASINONEGAI,    ADD_IyasiNoNegai  },
    { WAZANO_YUBIWOHURU,      ADD_YubiWoFuru    },
    { WAZANO_SIZENNOTIKARA,   ADD_SizenNoTikara },
    { WAZANO_NEKONOTE,        ADD_Nekonote      },
    { WAZANO_OUMUGAESI,       ADD_OumuGaesi     },
    { WAZANO_SAKIDORI,        ADD_Sakidori      },
    { WAZANO_MANEKKO,         ADD_Manekko       },
    { WAZANO_NEGOTO,          ADD_Negoto        },
    { WAZANO_KETAGURI,        ADD_Ketaguri      },
    { WAZANO_KUSAMUSUBI,      ADD_Ketaguri      },  // くさむすび=けたぐりと等価
    { WAZANO_KIAIPANTI,       ADD_KiaiPunch     },
    { WAZANO_TAKUWAERU,       ADD_Takuwaeru     },
    { WAZANO_HAKIDASU,        ADD_Hakidasu      },
    { WAZANO_NOMIKOMU,        ADD_Nomikomu      },
    { WAZANO_MIRAIYOTI,       ADD_Miraiyoti     },
    { WAZANO_HAMETUNONEGAI,   ADD_HametuNoNegai },
    { WAZANO_RISAIKURU,       ADD_Recycle       },
    { WAZANO_OIUTI,           ADD_Oiuti         },
    { WAZANO_NEKONIKOBAN,     ADD_NekoNiKoban   },
    { WAZANO_GAMAN,           ADD_Gaman         },
    { WAZANO_YOKODORI,        ADD_Yokodori      },
    { WAZANO_MAZIKKUKOOTO,    ADD_MagicCoat     },
    { WAZANO_TEREPOOTO,       ADD_Teleport      },
    { WAZANO_TONBOGAERI,      ADD_TonboGaeri    },
    { WAZANO_BATONTATTI,      ADD_BatonTouch    },
    { WAZANO_TUIBAMU,         ADD_Tuibamu       },
    { WAZANO_MUSIKUI,         ADD_Tuibamu       },  // むしくい=ついばむ と等価
    { WAZANO_NAGETUKERU,      ADD_Nagetukeru    },
    { WAZANO_MAKITUKU,        ADD_Makituku      },
    { WAZANO_SIMETUKERU,      ADD_Makituku      },
    { WAZANO_HONOONOUZU,      ADD_Makituku      },
    { WAZANO_KARADEHASAMU,    ADD_Makituku      },
    { WAZANO_SUNAZIGOKU,      ADD_Makituku      },
    { WAZANO_MAGUMASUTOOMU,   ADD_Makituku      },
    { WAZANO_UZUSIO,          ADD_Uzusio        },
    { WAZANO_HUKITOBASI,      ADD_KousokuSpin   },
    { WAZANO_PAWAATORIKKU,    ADD_PowerTrick    },

    // 以下、新ワザ
    { WAZANO_KARI_BENOMUSHOKKU,     ADD_BenomShock      },
    { WAZANO_KARI_IKARINOKONA,      ADD_KonoyubiTomare  },  // いかりのこな=このゆびとまれ
    { WAZANO_KARI_MIZUBITASI,       ADD_Mizubitasi      },
    { WAZANO_KARI_SINPURUBIIMU,     ADD_SimpleBeem      },
    { WAZANO_KARI_NAKAMADUKURI,     ADD_NakamaDukuri    },
    { WAZANO_KARI_KURIASUMOGGU,     ADD_ClearSmog       },
    { WAZANO_KARI_ASISUTOPAWAA,     ADD_AsistPower      },
    { WAZANO_KARI_KARAWOYABURU,     ADD_KarawoYaburu    },
    { WAZANO_KARI_TATARIME,         ADD_Tatarime        },
    { WAZANO_KARI_AKUROBATTO,       ADD_Acrobat         },
    { WAZANO_KARI_BORUTOCHENZI,     ADD_TonboGaeri      },  // ボルトチェンジ=とんぼがえり
    { WAZANO_KARI_WAIDOGAADO,       ADD_WideGuard       },
    { WAZANO_KARI_MIRAATAIPU,       ADD_MirrorType      },
    { WAZANO_KARI_PAWAASHEA,        ADD_PowerShare      },
    { WAZANO_KARI_GAADOSHEA,        ADD_GuardShare      },
    { WAZANO_KARI_BODYPAAZI,        ADD_BodyPurge       },
    { WAZANO_KARI_HEBIIBONBAA,      ADD_HeavyBomber     },
    { WAZANO_KARI_WANDAARUUMU,      ADD_WonderRoom      },
    { WAZANO_KARI_MAZIKKURUUMU,     ADD_MagicRoom       },
    { WAZANO_KARI_SAIKOSYOKKU,      ADD_PsycoShock      },
    { WAZANO_KARI_HAZIKERUHONOO,    ADD_HajikeruHonoo   },
    { WAZANO_KARI_EREKUTOBOORU,     ADD_ElectBall       },
    { WAZANO_KARI_SINKURONOIZU,     ADD_SyncroNoise     },
    { WAZANO_KARI_NASIKUZUSI,       ADD_NasiKuzusi      },
    { WAZANO_KARI_EKOOBOISU,        ADD_EchoVoice       },
    { WAZANO_KARI_YAKITUKUSU,       ADD_Yakitukusu      },
    { WAZANO_KARI_GIHUTOPASU,       ADD_GiftPass        },
  };

  int i;

  for(i=0; i<NELEMS(funcTbl); i++)
  {
    if( funcTbl[i].waza == waza )
    {
      u8 pokeID = BPP_GetID( pp );
      u8 fRegistered = FALSE;

      if( is_registable(pokeID, waza, &fRegistered) )
      {
        u16 agi = BPP_GetValue( pp, BPP_AGILITY );
        BTL_Printf("ワザハンドラ[%d]登録\n", waza);
        if( funcTbl[i].func( agi, waza, pokeID ) ){
          return TRUE;
        }
      }
      else if( fRegistered ){
        BTL_Printf("ワザハンドラ[%d]はすでに登録済み\n", waza);
        return TRUE;
      }
      return FALSE;
    }
  }

  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * 登録できる条件を満たしているか判定
 * ※ 同じポケかつ同じワザのハンドラは１つまで
 * ※ ポケモン１体につき EVENT_WAZA_STICK_MAX まで 登録できる
 *
 * @param   pokeID
 * @param   waza
 *
 * @retval  BOOL    条件を満たしていたらTRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_registable( u8 pokeID, WazaID waza, u8* fRegistered )
{
  BTL_EVENT_FACTOR* factor;
  u32 cnt = 0;

  factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    if( BTL_EVENT_FACTOR_GetSubID(factor) == waza ){
      BTL_Printf("既に同じワザが登録されている");
      *fRegistered = TRUE;
      return FALSE;
    }
    if( ++cnt > EVENT_WAZA_STICK_MAX ){
      BTL_Printf("同じポケモン[%d]の貼り付きワザが%dを越えた\n", pokeID, EVENT_WAZA_STICK_MAX);
      return FALSE;
    }
    factor = BTL_EVENT_GetNextFactor( factor );
  }
  return TRUE;
}
//=============================================================================================
/**
 * ポケモンの「ワザ」ハンドラをシステムから削除（ワザの挙動により貼り付いているものは削除しない）
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void BTL_HANDLER_Waza_Remove( const BTL_POKEPARAM* pp, WazaID waza )
{
  BTL_EVENT_FACTOR *factor, *next;
  u8 pokeID = BPP_GetID( pp );

  factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    next = BTL_EVENT_GetNextFactor( factor );
    if( (BTL_EVENT_FACTOR_GetSubID(factor) == waza)
    &&  (BTL_EVENT_FACTOR_GetWorkValue(factor, WORKIDX_STICK) == 0)
    ){
      BTL_Printf("pokeID[%d], waza[%d]ハンドラ削除\n", pokeID, waza);
      BTL_EVENT_FACTOR_Remove( factor );
    }
    factor = next;
  }
}
//=============================================================================================
/**
 * ポケモンの「ワザ」ハンドラをシステムから強制削除（貼り付いているものも強制的に削除）
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void BTL_HANDLER_Waza_RemoveForce( const BTL_POKEPARAM* pp, WazaID waza )
{
  u8 pokeID = BPP_GetID( pp );
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
 * 特定ポケモンのワザハンドラをシステムから全て強制削除（貼り付いているものも強制的に削除）
 *
 * @param   pp
 */
//=============================================================================================
void BTL_HANDLER_Waza_RemoveForceAll( const BTL_POKEPARAM* pp )
{
  u8 pokeID = BPP_GetID( pp );
  BTL_EVENT_FACTOR* factor;
  while( (factor = BTL_EVENT_SeekFactor(BTL_EVENT_FACTOR_WAZA, pokeID)) != NULL )
  {
    BTL_EVENT_FACTOR_Remove( factor );
  }
}
//----------------------------------------------------------------------------------
/**
 * テクスチャー
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Texture( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Texture },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Texture( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 waza_cnt = BPP_WAZA_GetCount( bpp );
    PokeType next_type;
    {
      PokeType type[ PTL_WAZA_MAX ];
      u16 i;
      for(i=0; i<waza_cnt; ++i){
        type[i] = WAZADATA_GetType( BPP_WAZA_GetID(bpp, i) );
      }
      i = GFL_STD_MtRand( waza_cnt );
      next_type = PokeTypePair_MakePure( type[i] );
    }

    {
      BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
      param->next_type = next_type;
      param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ほごしょく
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Hogosyoku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Hogosyoku },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Hogosyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlLandForm  land = BTL_SVFLOW_GetLandForm( flowWk );
    PokeType type = POKETYPE_NORMAL;

    switch( land ){
    case BTL_LANDFORM_GRASS:   type = POKETYPE_KUSA;   break;  ///< 草むら
    case BTL_LANDFORM_SAND:    type = POKETYPE_JIMEN;  break;  ///< 砂地
    case BTL_LANDFORM_SEA:     type = POKETYPE_MIZU;   break;  ///< 海
    case BTL_LANDFORM_RIVER:   type = POKETYPE_MIZU;   break;  ///< 川
    case BTL_LANDFORM_SNOW:    type = POKETYPE_KOORI;  break;  ///< 雪原
    case BTL_LANDFORM_CAVE:    type = POKETYPE_IWA;    break;  ///< 洞窟
    case BTL_LANDFORM_ROCK:    type = POKETYPE_IWA;    break;  ///< 岩場
    case BTL_LANDFORM_ROOM:    type = POKETYPE_NORMAL; break;  ///< 室内
    }

    {
      PokeTypePair  pairType = PokeTypePair_MakePure( type );
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

      if( BPP_GetPokeType(bpp) != pairType )
      {
        BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
        param->next_type = pairType;
        param->pokeID = pokeID;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * トリックルーム
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TrickRoom( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_TrickRoom },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_TrickRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_TRICKROOM) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_TRICKROOM;
      param->cont = BPP_SICKCONT_MakeTurn( 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TrickRoom );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;
      BTL_HANDEX_PARAM_MESSAGE*  msg_param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_REMOVE_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_TRICKROOM;

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TrickRoomOff );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );

    }
  }
}
//----------------------------------------------------------------------------------
/**
 * じゅうりょく
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Juryoku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL,     handler_Juryoku },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };

  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Juryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;
    BTL_HANDEX_PARAM_MESSAGE*  msg_param;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
    param->effect = BTL_FLDEFF_JURYOKU;
    param->cont = BPP_SICKCONT_MakeTurn( 5 );

    param->exStr.type = BTL_STRTYPE_STD;
    param->exStr.ID = BTL_STRID_STD_Jyuryoku;
  }
}
//----------------------------------------------------------------------------------
/**
 * みずあそび
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MizuAsobi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_MizuAsobi },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MizuAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
    param->effect = BTL_FLDEFF_MIZUASOBI;
    param->cont = BPP_SICKCONT_MakePoke( pokeID );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Mizuasobi );
  }
}
//----------------------------------------------------------------------------------
/**
 * どろあそび
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_DoroAsobi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_DoroAsobi },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_DoroAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
    param->effect = BTL_FLDEFF_MIZUASOBI;
    param->cont = BPP_SICKCONT_MakePoke( pokeID );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Doroasobi );
  }
}
//----------------------------------------------------------------------------------
/**
 * きりばらい
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kiribarai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Kiribarai },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kiribarai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;
    BTL_HANDEX_PARAM_SIDEEFF_REMOVE* remove_param;

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->rankType = WAZA_RANKEFF_AVOID;
    rank_param->rankVolume = -1;
    rank_param->fAlmost = TRUE;
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = targetPokeID;

    remove_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
    remove_param->side = BTL_MAINUTIL_PokeIDtoSide( targetPokeID );
    BTL_CALC_BITFLG_Construction( remove_param->flags, sizeof(remove_param->flags) );
    BTL_CALC_BITFLG_Set( remove_param->flags, BTL_SIDEEFF_REFRECTOR );
    BTL_CALC_BITFLG_Set( remove_param->flags, BTL_SIDEEFF_HIKARINOKABE );
    BTL_CALC_BITFLG_Set( remove_param->flags, BTL_SIDEEFF_SIROIKIRI );
    BTL_CALC_BITFLG_Set( remove_param->flags, BTL_SIDEEFF_SINPINOMAMORI );
    BTL_CALC_BITFLG_Set( remove_param->flags, BTL_SIDEEFF_MAKIBISI );
    BTL_CALC_BITFLG_Set( remove_param->flags, BTL_SIDEEFF_DOKUBISI );
    BTL_CALC_BITFLG_Set( remove_param->flags, BTL_SIDEEFF_STEALTHROCK );
    remove_param->fExMsg = TRUE;
    remove_param->exStrID = BTL_STRID_SET_Kiribarai;
  }
}
//----------------------------------------------------------------------------------
/**
 * かわらわり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kawarawari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_DETERMINE, handler_Kawarawari },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kawarawari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    BtlSide side = BTL_MAINUTIL_PokeIDtoSide( targetPokeID );

    if( BTL_HANDER_SIDE_IsExist(side, BTL_SIDEEFF_REFRECTOR)
    ||  BTL_HANDER_SIDE_IsExist(side, BTL_SIDEEFF_HIKARINOKABE)
    ){
      BTL_HANDEX_PARAM_SIDEEFF_REMOVE* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
      param->side = side;
      BTL_CALC_BITFLG_Construction( param->flags, sizeof(param->flags) );
      BTL_CALC_BITFLG_Set( param->flags, BTL_SIDEEFF_REFRECTOR );
      BTL_CALC_BITFLG_Set( param->flags, BTL_SIDEEFF_HIKARINOKABE );

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Kawarawari_Break );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * とびげり・とびひざげり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tobigeri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_AVOID, handler_Tobigeri },    // ワザはずれた
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Tobigeri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    u32 damage = BTL_SVFLOW_SimulationDamage( flowWk, pokeID, targetPokeID, waza, FALSE, TRUE );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    u32 maxHP = BPP_GetValue( target, BPP_MAX_HP );

    damage /= 2;
    maxHP /= 2;
    if( damage > maxHP ){
      damage = maxHP;
    }

    {
      BTL_HANDEX_PARAM_DAMAGE* param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
      param->pokeID = pokeID;
      param->damage = damage;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HazureJibaku );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ものまね
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Monomane( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Monomane },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Monomane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    WazaID waza = BPP_GetPrevWazaID( target );
    if( waza != WAZANO_NULL)
    {
      u8 wazaIdx = BPP_WAZA_SearchIdx( self, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        BTL_HANDEX_PARAM_UPDATE_WAZA*  param;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_UPDATE_WAZA, pokeID );
        param->pokeID = pokeID;
        param->wazaIdx = wazaIdx;
        param->waza = waza;
        param->ppMax = 5;
        param->fPermanent = FALSE;

        msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Monomane );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
        HANDEX_STR_AddArg( &msg_param->str, waza );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * スケッチ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Sketch( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Sketch },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Sketch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    WazaID waza = BPP_GetPrevWazaID( target );
    if( waza != WAZANO_NULL)
    {
      u8 wazaIdx = BPP_WAZA_SearchIdx( self, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        BTL_HANDEX_PARAM_UPDATE_WAZA*  param;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_UPDATE_WAZA, pokeID );
        param->pokeID = pokeID;
        param->wazaIdx = wazaIdx;
        param->waza = waza;
        param->ppMax = 0;
        param->fPermanent = TRUE;

        msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Monomane );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
        HANDEX_STR_AddArg( &msg_param->str, waza );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * このゆびとまれ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KonoyubiTomare( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_KonoyubiTomare_ExeCheck },    // ワザ出し成否チェック
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_KonoyubiTomare_Exe   },     // 未分類ワザ実行
    { BTL_EVENT_DECIDE_TARGET,      handler_KonoyubiTomare_Target },      // ターゲット決定
    { BTL_EVENT_TAME_START,         handler_KonoyubiTomare_TurnCheck },   // ターンチェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
// ワザ出し成否チェック：シングルなら失敗する
static void handler_KonoyubiTomare_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_SVFLOW_GetRule(flowWk) == BTL_RULE_SINGLE )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_CAUSE) == SV_WAZAFAIL_NULL ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
      }
    }
  }
}
// ワザ出し成否確定：メッセージ表示
static void handler_KonoyubiTomare_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Konoyubi );
    HANDEX_STR_AddArg( &param->str, pokeID );
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_KonoyubiTomare_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_SVFLOW_GetRule(flowWk) != BTL_RULE_SINGLE )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, pokeID );
    }
  }
}
static void handler_KonoyubiTomare_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  removeHandlerForce( pokeID, BTL_EVENT_FACTOR_GetSubID(myHandle) );
}

//----------------------------------------------------------------------------------
/**
 * リフレッシュ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Refresh( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Refresh },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Refresh( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    PokeSick sick = BPP_GetPokeSick( bpp );

    if( (sick == POKESICK_DOKU)
    ||  (sick == POKESICK_MAHI)
    ||  (sick == POKESICK_YAKEDO)
    ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->sickCode = sick;
      param->pokeID[0] = pokeID;
      param->poke_cnt = 1;
      // @@@ メッセージいるか？
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * クモのす、くろいまなざし、とおせんぼう
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KumoNoSu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_KumoNoSu },    // 未分類ワザハンドラ
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(bpp, WAZASICK_TOOSENBOU) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->sickID = WAZASICK_TOOSENBOU;
      param->sickCont = BTL_CALC_MakeWazaSickCont_Poke( targetPokeID );

      param->fExMsg = TRUE;
      param->exStr.type = BTL_STRTYPE_SET;
      param->exStr.ID = BTL_STRID_SET_CantEscWaza;
      param->exStr.argCnt = 1;
      param->exStr.args[0] = targetPokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * くろいきり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KuroiKiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_KuroiKiri },    // 未分類ワザハンドラ
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

    reset_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    {
      BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokeID( flowWk, pokeID );
      BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_ALL, myPos );

      reset_param->poke_cnt = BTL_SVFTOOL_ExpandPokeID( flowWk, expos, reset_param->pokeID );
    }

    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_RankReset );
  }
}
//----------------------------------------------------------------------------------
/**
 * はねる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Haneru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Haneru },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Haneru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_NotHappen );
  }
}
//----------------------------------------------------------------------------------
/**
 * のろい
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Noroi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Noroi },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Noroi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    if( BPP_IsMatchType(bpp, POKETYPE_GHOST) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param;
      u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->sickID = WAZASICK_NOROI;
      param->sickCont = BPP_SICKCONT_MakePermanent();
/*
      param->fExMsg = TRUE;
      param->exStr.type = BTL_STRTYPE_SET;
      param->exStr.ID = BTL_STRID_SET_Noroi;
      param->exStr.argCnt = 1;
      param->exStr.args[0] = targetPokeID;
*/
    }
    else
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param;
      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_AGILITY;
      param->rankVolume = -1;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_ATTACK;
      param->rankVolume = 1;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_DEFENCE;
      param->rankVolume = 1;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * なやみのタネ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_NayamiNoTane( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_NayamiNoTane_NoEff },    // 無効化チェックレベル２ハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_NayamiNoTane       },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_NayamiNoTane_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    // なまけ，ふみん持ちには効かない
    if( (BPP_GetValue(target, BPP_TOKUSEI) == POKETOKUSEI_NAMAKE)
    ||  (BPP_GetValue(target, BPP_TOKUSEI) == POKETOKUSEI_FUMIN)
    ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
static void handler_NayamiNoTane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 対象のとくせいを「ふみん」に書き換え
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );

    param->pokeID = targetPokeID;
    param->tokuseiID = POKETOKUSEI_FUMIN;
  }
}

//----------------------------------------------------------------------------------
/**
 * ゆめくい
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yumekui( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Yumekui },    // 無効化チェックレベル２ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Yumekui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, POKESICK_NEMURI) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ゆうわく
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yuwaku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Yuwaku },    // 無効化チェックレベル２ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Yuwaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    const BTL_POKEPARAM* me = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    u8 my_sex = BPP_GetValue( me, BPP_SEX );
    u8 target_sex = BPP_GetValue( target, BPP_SEX );

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
 * トライアタック
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TriAttack( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_TriAttack },    // 追加効果による状態異常チェックハンドラ
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
    cont = BTL_CALC_MakeDefaultPokeSickCont( sick );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 20 );
  }
}
//----------------------------------------------------------------------------------
/**
 * ひみつのちから
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HimituNoTikara( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_HimituNoTikara_Sick },    // 追加効果による状態異常チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_HimituNoTikara_Sick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BtlLandForm  land = BTL_SVFLOW_GetLandForm( flowWk );
    WazaSick  sick;
    BPP_SICK_CONT  cont;
    WazaID  waza = BTL_EVENT_FACTOR_GetSubID( myHandle );

    // @@@ 本来は地形に応じてもっとフクザツに
    if( land == BTL_LANDFORM_SNOW ){
      sick = WAZASICK_KOORI;
    }else{
      sick = WAZASICK_MAHI;
    }

    BTL_CALC_MakeDefaultWazaSickCont( sick, bpp, &cont );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, cont.raw );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, WAZADATA_GetParam(waza, WAZAPARAM_SICK_PER) );
  }
}
//----------------------------------------------------------------------------------
/**
 * おしゃべり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Osyaberi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_Osyaberi },    // 追加効果による状態異常チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Osyaberi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetMonsNo(bpp) == MONSNO_PERAPPU )
    {
      PokeSick sick = WAZASICK_KONRAN;
      BPP_SICK_CONT cont = BTL_CALC_MakeDefaultPokeSickCont( sick );
      const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
      u32 rnd = PP_Get( pp, ID_PARA_personal_rnd, NULL );
      u8 per = (rnd&1)? 10 : 30;

      BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, sick );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * しめつける・まきつく・ほのおのうず等々
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Makituku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_Makituku },    // 追加効果による状態異常チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Makituku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BPP_SICK_CONT cont;
    WazaID waza = BTL_EVENT_FACTOR_GetSubID( myHandle );

    cont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
    BPP_SICKCONT_AddParam( &cont, waza );
    BTL_Printf("  巻き付く系 : poketurn, pokeID=%d, turn=%d\n", cont.poketurn.pokeID, cont.poketurn.count);
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}
//----------------------------------------------------------------------------------
/**
 * うずしお
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Uzusio( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_Makituku },    // 追加効果による状態異常チェックハンドラ = まきつくと一緒
    { BTL_EVENT_WAZA_DMG_PROC2, handler_Uzusio_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Uzusio_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_DIVING) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いかりのまえば
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_IkariNoMaeba( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_IkariNoMaeba },    // ダメージ計算最終ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_IkariNoMaeba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    u16 damage = BPP_GetValue(target, BPP_HP) / 2;

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
  }
}
//----------------------------------------------------------------------------------
/**
 * りゅうのいかり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_RyuuNoIkari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_RyuuNoIkari },    // ダメージ計算最終ハンドラ
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
 * ソニックブーム
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SonicBoom( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_SonicBoom },    // ダメージ計算最終ハンドラ
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
 * がむしゃら
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Gamusyara( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_Gamusyara },    // ダメージ計算最終ハンドラ
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

    attacker = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    defender = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    diff = BPP_GetValue(defender, BPP_HP) - BPP_GetValue(attacker, BPP_HP);
    if( diff < 0 ){
      diff = 0;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, diff );
  }
}
//----------------------------------------------------------------------------------
/**
 * ちきゅうなげ、ナイトヘッド
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TikyuuNage( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_TikyuuNage },    // ダメージ計算最終ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_TikyuuNage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 level = BPP_GetValue( bpp, BPP_LEVEL );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, level );
  }
}
//----------------------------------------------------------------------------------
/**
 * サイコウェーブ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PsycoWave( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_PsycoWave },    // ダメージ計算最終ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_PsycoWave( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 level = BPP_GetValue( bpp, BPP_LEVEL );
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
 * たくわえる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Takuwaeru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Takuwaeru_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Takuwaeru            },    // 未分類ワザ処理
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
// 成否チェック（カウンタが３貯まっていたら失敗する）
static void handler_Takuwaeru_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU) >= 3 ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
static void handler_Takuwaeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 takuwae_cnt = BPP_COUNTER_Get( bpp, BPP_COUNTER_TAKUWAERU );
    if( takuwae_cnt < 3 )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;
      BTL_HANDEX_PARAM_COUNTER*     counter_param;
      BTL_HANDEX_PARAM_MESSAGE*     msg_param;

      counter_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
      counter_param->pokeID = pokeID;
      counter_param->counterID = BPP_COUNTER_TAKUWAERU;
      counter_param->value = takuwae_cnt + 1;
      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Takuwaeru );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, takuwae_cnt+1 );


      rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->poke_cnt = 1;
      rank_param->pokeID[0] = pokeID;
      rank_param->rankType = BPP_DEFENCE;
      rank_param->rankVolume = 1;
      counter_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
      counter_param->header.failSkipFlag = TRUE;
      counter_param->pokeID = pokeID;
      counter_param->counterID = BPP_COUNTER_TAKUWAERU_DEF;
      counter_param->value = BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU_DEF) + 1;


      rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->poke_cnt = 1;
      rank_param->pokeID[0] = pokeID;
      rank_param->rankType = BPP_SP_DEFENCE;
      rank_param->rankVolume = 1;
      counter_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
      counter_param->header.failSkipFlag = TRUE;
      counter_param->pokeID = pokeID;
      counter_param->counterID = BPP_COUNTER_TAKUWAERU_SPDEF;
      counter_param->value = BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU_SPDEF) + 1;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * はきだす
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Hakidasu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Hakidasu_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_WAZA_POWER,          handler_Hakidasu_Pow        },    // ワザ威力決定
    { BTL_EVENT_WAZA_EXECUTE_DONE,   handler_Hakidasu_Done       },    // ワザ出し終了
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
// 成否チェック（カウンタが０だったら失敗する）
static void handler_Hakidasu_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU) == 0 ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
static void handler_Hakidasu_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 pow = BPP_COUNTER_Get( bpp, BPP_COUNTER_TAKUWAERU ) * 100;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
static void handler_Hakidasu_Done( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_COUNTER*  counter_param;
    BTL_HANDEX_PARAM_MESSAGE*  msg_param;

    int def_rank   = BPP_GetValue( bpp, BPP_DEFENCE_RANK );
    int spdef_rank = BPP_GetValue( bpp, BPP_SP_DEFENCE_RANK );

    def_rank -= BPP_COUNTER_Get( bpp, BPP_COUNTER_TAKUWAERU_DEF );
    if( def_rank < 0 ){
      def_rank = 0;
    }
    spdef_rank -= BPP_COUNTER_Get( bpp, BPP_COUNTER_TAKUWAERU_SPDEF );
    if( spdef_rank < 0 ){
      spdef_rank = 0;
    }

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID     = pokeID;
    rank_param->attack     = BPP_GetValue( bpp, BPP_ATTACK_RANK );
    rank_param->defence    = def_rank;
    rank_param->sp_attack  = BPP_GetValue( bpp, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence = spdef_rank;
    rank_param->agility    = BPP_GetValue( bpp, BPP_AGILITY_RANK );

    counter_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
    counter_param->pokeID = pokeID;
    counter_param->counterID = BPP_COUNTER_TAKUWAERU;
    counter_param->value = 0;

    counter_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
    counter_param->pokeID = pokeID;
    counter_param->counterID = BPP_COUNTER_TAKUWAERU_DEF;
    counter_param->value = 0;

    counter_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
    counter_param->pokeID = pokeID;
    counter_param->counterID = BPP_COUNTER_TAKUWAERU_SPDEF;
    counter_param->value = 0;

    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TakuwaeOff );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * のみこむ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nomikomu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Hakidasu_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_RECOVER_HP_RATIO,    handler_Nomikomu_Ratio      },    // HP回復率決定
    { BTL_EVENT_WAZA_EXECUTE_DONE,   handler_Hakidasu_Done       },    // ワザ出し終了
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Nomikomu_Ratio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 cnt = BPP_COUNTER_Get( bpp, BPP_COUNTER_TAKUWAERU );
    u8 ratio;
    switch( cnt ){
    case 1:
    default:
      ratio = 25;
      break;
    case 2:   ratio =  50; break;
    case 3:   ratio = 100; break;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, ratio );
  }
}

//----------------------------------------------------------------------------------
/**
 * カウンター
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Counter( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Counter_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_DECIDE_TARGET,       handler_Counter_Target     },    // ターゲット決定
    { BTL_EVENT_WAZA_DMG_PROC1,      handler_Counter_CalcDamage },    // ダメージ計算最終ハンドラ

    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Counter_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Counter_ExeCheck( flowWk, pokeID, work, WAZADATA_DMG_PHYSIC );
}
static void handler_Counter_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Counter_SetTarget( flowWk, pokeID, work, WAZADATA_DMG_PHYSIC );
}
static void handler_Counter_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Counter_CalcDamage( flowWk, pokeID, work, WAZADATA_DMG_PHYSIC );
}
//----------------------------------------------------------------------------------
/**
 * ミラーコート
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MilerCoat( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_MilerCoat_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_DECIDE_TARGET,       handler_MilerCoat_Target     },    // ターゲット決定
    { BTL_EVENT_WAZA_DMG_PROC1,      handler_MilerCoat_CalcDamage },    // ダメージ計算最終ハンドラ

    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MilerCoat_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Counter_ExeCheck( flowWk, pokeID, work, WAZADATA_DMG_SPECIAL );
}
static void handler_MilerCoat_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Counter_SetTarget( flowWk, pokeID, work, WAZADATA_DMG_SPECIAL );
}
static void handler_MilerCoat_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Counter_CalcDamage( flowWk, pokeID, work, WAZADATA_DMG_SPECIAL );
}
//----------------------------------------------------------------------------------
/**
 * メタルバースト
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MetalBurst( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_MetalBurst_CheckExe   },    // ワザ出し成功判定
    { BTL_EVENT_DECIDE_TARGET,       handler_MetalBurst_Target     },    // ターゲット決定
    { BTL_EVENT_WAZA_DMG_PROC1,      handler_MetalBurst_CalcDamage },    // ダメージ計算最終ハンドラ

    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MetalBurst_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Counter_ExeCheck( flowWk, pokeID, work, WAZADATA_DMG_NONE );
}
static void handler_MetalBurst_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Counter_SetTarget( flowWk, pokeID, work, WAZADATA_DMG_NONE );
}
static void handler_MetalBurst_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Counter_CalcDamage( flowWk, pokeID, work, WAZADATA_DMG_NONE );
}

//----------------------------------------------------------------------------------
//
// カウンター系共通：ワザだしチェック
//
//----------------------------------------------------------------------------------
static void common_Counter_ExeCheck( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BPP_WAZADMG_REC  rec;

    if( !common_Counter_GetRec(bpp, dmgType, &rec) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
//
// カウンター系共通：ターゲット決定
//
//----------------------------------------------------------------------------------
static void common_Counter_SetTarget( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BPP_WAZADMG_REC  rec;

    if( common_Counter_GetRec(bpp, dmgType, &rec) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, rec.pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
//
// カウンター系共通：ダメージ計算
//
//----------------------------------------------------------------------------------
static void common_Counter_CalcDamage( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BPP_WAZADMG_REC  rec;

    if( common_Counter_GetRec(bpp, dmgType, &rec) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, rec.damage*2 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * カウンター系共通下請け：対象となるダメージレコードを取得
 *
 * @param   bpp
 * @param   dmgType   対象ダメージタイプ（物理or特殊：WAZADATA_DMG_NONEなら両対応）
 * @param   rec       [out]取得先構造体アドレス
 *
 * @retval  BOOL      取得できたらTRUE
 */
//----------------------------------------------------------------------------------
static BOOL common_Counter_GetRec( const BTL_POKEPARAM* bpp, WazaDamageType dmgType, BPP_WAZADMG_REC* rec )
{
  u8 pokeID = BPP_GetID( bpp );
  u8 idx = 0;
  while( BPP_WAZADMGREC_Get(bpp, 0, idx++, rec) )
  {
    if( (!BTL_MAINUTIL_IsFriendPokeID( pokeID, rec->pokeID ))
    &&  ((dmgType==WAZADATA_DMG_NONE) || (WAZADATA_GetDamageType(rec->wazaID) == dmgType))
    ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * とっておき
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Totteoki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_Totteoki },    // ワザ出し成否チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Totteoki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 wazaCnt = BPP_WAZA_GetCount( bpp );
    if( (wazaCnt < 2)
    ||  (BPP_WAZA_GetUsedCount(bpp) < (wazaCnt-1) )
    ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いびき
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ibiki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_Ibiki },    // ワザ出し成否チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Ibiki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    SV_WazaFailCause  fail_cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );

    if( fail_cause == SV_WAZAFAIL_NEMURI ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_NULL );
    }else{
      if( !BPP_CheckSick(bpp, POKESICK_NEMURI) ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * あくむ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Akumu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Akumu_NoEff },    // ワザ無効チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Akumu_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF);
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(bpp, POKESICK_NEMURI) ){
      BTL_Printf("相手[%d]が寝てないから失敗\n", targetPokeID);
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ふいうち
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Fuiuti( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Fuiuti_NoEff },    // ワザ無効チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Fuiuti_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    BTL_ACTION_PARAM  action;
    WazaID waza;
    BOOL fSuccess = FALSE;

    do {
      // 相手が既に行動していたら失敗
      if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_WAZA_EXE) ){ break; }

      // 現ターンのアクションが取得できなかったら失敗
      if( !BTL_SVFLOW_GetThisTurnAction( flowWk, targetPokeID, &action ) ){ break; }

      // 相手がダメージワザを選択していない場合も失敗
      waza = BTL_ACTION_GetWazaID( &action );
      if( (waza == WAZANO_NULL)
      ||  (!WAZADATA_IsDamage(waza))
      ){
        break;
      }
      // ここまで来たら成功
      fSuccess = TRUE;
    }while(0);

    if( !fSuccess )
    {
      WazaID waza = BTL_ACTION_GetWazaID( &action );
      if( (waza == WAZANO_NULL)
      ||  (!WAZADATA_IsDamage(waza))
      ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * おいうち
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Oiuti( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MENBERCHANGE_INTR, handler_Oiuti_Intr },    // 入れ替え割り込みハンドラ
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Oiuti_Dmg  },    // ダメージ計算最終チェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Oiuti_Intr( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_ACTION_PARAM  action;

  if( BTL_SVFLOW_GetThisTurnAction(flowWk, pokeID, &action) )
  {
    u8 targetPokeID = BTL_SVFLOW_PokePosToPokeID( flowWk, action.fight.targetPos );
    BTL_Printf("ワシ[%d]が今から狙うポケ=%d, 入れ替わろうとしてるポケ=%d（位置:%d)\n",
      pokeID, targetPokeID, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1), action.fight.targetPos);
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) == targetPokeID )
    {
      BTL_Printf("割り込みます\n");
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_ATK, pokeID );
    }
  }
}
static void handler_Oiuti_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_ACTION_PARAM  action;
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    if( BTL_SVFLOW_GetThisTurnAction(flowWk, targetPokeID, &action) )
    {
      if( BTL_ACTION_GetAction(&action) == BTL_ACTION_CHANGE ){
        BTL_Printf("入れ替わろうとしてるヤツには２倍です\n");
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * だいばくはつ・じばく
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Daibakuhatsu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD,   handler_Daibakuhatsu },         // 防御力チェックハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Daibakuhatsu_ExeFix },  // わざ出し確定ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Daibakuhatsu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
  }
}
static void handler_Daibakuhatsu_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    param->pokeID = pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * じゅうでん
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Juden( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Juden_Exe },    // ワザ出し確定ハンドラ
    { BTL_EVENT_WAZA_POWER,       handler_Juden_Pow },    // ワザ威力計算ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Juden_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) == BTL_EVENT_FACTOR_GetSubID(myHandle) ){
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Juuden );
      HANDEX_STR_AddArg( &param->str, pokeID );

      work[ WORKIDX_STICK ] = 1;
      work[0] = 1;
    }
  }
}
static void handler_Juden_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 1 )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_DENKI ){
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
        BTL_EVENT_FACTOR_Remove( myHandle );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ほろびのうた
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HorobiNoUta( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_HorobiNoUta_Exe },    // ワザ出し確定ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_HorobiNoUta_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_HorobiNoUta );
  }
}
//----------------------------------------------------------------------------------
/**
 * やどりぎのたね
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_YadorigiNoTane( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM, handler_YadorigiNoTane_Param },    // ワザ出し確定ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_YadorigiNoTane_Param( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BPP_SICK_CONT cont;
    BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokeID( flowWk, pokeID );
    cont = BPP_SICKCONT_MakePermanentParam( myPos );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}
//----------------------------------------------------------------------------------
/**
 * ネコにこばん
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_NekoNiKoban( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER, handler_NekoNiKoban },    // ダメージ反応ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_NekoNiKoban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_SVFLOW_IsStandAlonePlayerSide( flowWk, pokeID ) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param;
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

      u32 volume = BPP_GetValue( bpp, BPP_LEVEL ) * 20;
      BTL_SVFLOW_AddBonusMoney( flowWk, volume );

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_NekoNiKoban );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いかり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ikari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Ikari_Exe },    // ワザ出し確定ハンドラ
    { BTL_EVENT_WAZA_DMG_AFTER, handler_Ikari_React },    // ダメージ反応ハンドラ
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
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
}
static void handler_Ikari_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

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
 * アクアリング
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_AquaRing( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_AquaRing },            // ワザ出し確定ハンドラ
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_AquaRing_turnCheck },  // ターンチェック開始ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_AquaRing( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_AquaRing );
    HANDEX_STR_AddArg( &param->str, pokeID );
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_AquaRing_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] != 0)
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_IsHPFull(bpp)
    &&  !BPP_IsDead(bpp)
    ){
      BTL_HANDEX_PARAM_RECOVER_HP* param;
      const BTL_POKEPARAM* bpp;

      bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = pokeID;
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 16 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_AquaRingRecover );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * あばれる・はなびらのまい・げきりん
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Abareru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Abareru },            // ワザ出し確定ハンドラ
    { BTL_EVENT_TURNCHECK_END,    handler_Abareru_turnCheck },  // ターンチェック終了ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Abareru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 自分がワザロック状態になっていないなら、ワザロック状態にする
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (!BPP_CheckSick(bpp, WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      u8 turns;

      turns = 3 + GFL_STD_MtRand(2);

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK;
      param->sickCont = BPP_SICKCONT_MakeTurn( turns );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      work[ WORKIDX_STICK ] = 1;
    }
  }
}
static void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] != 0)
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_CheckSick(bpp, WAZASICK_WAZALOCK)
    &&  !BPP_CheckSick(bpp, WAZASICK_KONRAN)
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_KONRAN;
      BTL_CALC_MakeDefaultWazaSickCont( param->sickID, bpp, &param->sickCont );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * さわぐ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Sawagu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Sawagu },            // ワザ出し確定ハンドラ
    { BTL_EVENT_TURNCHECK_END,     handler_Sawagu_turnCheck },  // ターンチェック終了ハンドラ
    { BTL_EVENT_ADDSICK_CHECKFAIL, handler_Sawagu_CheckSickFail },  // 状態異常失敗チェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Sawagu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 自分がワザロック状態になっていないなら、ワザロック状態にする
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (!BPP_CheckSick(bpp, WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      u8 turns;

      turns = BTL_CALC_RandRange( 2, 5 );
      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK;
      param->sickCont = BPP_SICKCONT_MakeTurn( turns );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Sawagu );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );

      {
        BTL_HANDEX_PARAM_CURE_SICK* cure_param;
        BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokeID( flowWk, pokeID );
        BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_MYSIDE_ALL, myPos );

        cure_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        cure_param->sickCode = WAZASICK_NEMURI;
        cure_param->poke_cnt = BTL_SVFTOOL_ExpandPokeID( flowWk, expos, cure_param->pokeID );
        cure_param->fExMsg = TRUE;
        cure_param->exStrID = BTL_STRID_SET_SawaguWake;
      }

      work[ WORKIDX_STICK ] = 1;
    }
  }
}
static void handler_Sawagu_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_CheckSick(bpp, WAZASICK_WAZALOCK)
    &&  !BPP_IsDead(bpp)
    ){
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SawaguCure );
      HANDEX_STR_AddArg( &param->str, pokeID );

      BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
}
static void handler_Sawagu_CheckSickFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) == WAZASICK_NEMURI )
    {
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_FAIL_FLAG, TRUE) )
      {
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        u8 defPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
        u16 strID = (defPokeID == pokeID)? BTL_STRID_SET_SawaguSleeplessSelf : BTL_STRID_SET_SawaguSleepless;
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
        HANDEX_STR_AddArg( &param->str, defPokeID );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ころがる・アイスボール
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Korogaru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,       handler_Korogaru_ExeFix    },  // ワザ出し確定ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_FAIL,      handler_Korogaru_ExeFail   },  // ワザ出し失敗確定ハンドラ
    { BTL_EVENT_WAZA_EXECUTE_NO_EFFECT, handler_Korogaru_ExeFail  },   // ワザ無効ハンドラ
    { BTL_EVENT_WAZA_POWER,             handler_Korogaru_Pow      },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Korogaru_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  enum {
    KOROGARU_TURNS = 5,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 自分がワザロック状態になっていないなら、ワザロック状態にする
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (!BPP_CheckSick(bpp, WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK;
      param->sickCont = BPP_SICKCONT_MakeTurn( KOROGARU_TURNS );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      work[0] = 0;
      work[ WORKIDX_STICK ] = 1;
      BTL_Printf("ポケ(%d)がころがる。ワザハンドラ貼り付き\n", pokeID);
    }
  }
}
// ワザだし失敗
static void handler_Korogaru_ExeFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 失敗したらワザロック解除＆貼り付き解除
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
    param->pokeID[0] = pokeID;
    param->poke_cnt = 1;
    param->sickCode = WAZASICK_WAZALOCK;

    removeHandlerForce( pokeID, BTL_EVENT_FACTOR_GetSubID(myHandle) );
  }
}
// 威力決定
static void handler_Korogaru_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 mul = 1, i;

    for(i=0; i<work[0]; ++i){
      mul *= 2;
    }
    if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_MARUKUNARU) ){
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
 * トリプルキック
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TripleKick( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_TripleKick },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_TripleKick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    work[0]++;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, work[0]*10 );
  }
}
//----------------------------------------------------------------------------------
/**
 * ジャイロボール
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_GyroBall( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_GyroBall },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_GyroBall( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    u16 selfAgi = BPP_GetActionAgility( self );
    u16 targetAgi = BPP_GetActionAgility( target );

    u32 pow = 1 + (25 * targetAgi / selfAgi);
    if( pow > 150 ){
      pow = 150;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}


//----------------------------------------------------------------------------------
/**
 * リベンジ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Revenge( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Revenge },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Revenge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BPP_WAZADMG_REC  rec;
    u8  idx = 0;

    // 狙う相手から現ターンにダメージ受けてたら威力２倍
    while( BPP_WAZADMGREC_Get(bpp, 0, idx++, &rec) )
    {
      if( rec.pokeID == target_pokeID ){
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
        break;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * じたばた、きしかいせい
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Jitabata( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Jitabata },    // ワザ威力チェックハンドラ
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    fx32 r = BPP_GetHPRatio( bpp );
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
 * からげんき
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Karagenki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Karagenki },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Karagenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    PokeSick  sick = BPP_GetPokeSick( bpp );
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
 * しっぺがえし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Sippegaesi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Sippegaesi },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Sippegaesi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_WAZA_EXE) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ふんか、しおふき
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Funka( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Funka },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Funka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    fx32 r = BPP_GetHPRatio( bpp );
    u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );

    pow = BTL_CALC_MulRatio_OverZero( pow, r );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * しぼりとる・にぎりつぶす
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Siboritoru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Siboritoru },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Siboritoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    fx32 r = BPP_GetHPRatio( bpp );
    u32 pow = BTL_CALC_MulRatio_OverZero( 120, r );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * しおみず
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Siomizu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Siomizu },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Siomizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    fx32 r = BPP_GetHPRatio( bpp );
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
 * おんがえし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ongaesi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Ongaesi },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ongaesi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
    u32 natsuki = PP_Get( pp, ID_PARA_friend, NULL );
    u32 pow = natsuki * 10 / 25;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * やつあたり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yatuatari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Yatuatari },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Yatuatari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
    u32 natsuki = PP_Get( pp, ID_PARA_friend, NULL );
    u32 pow = (255 - natsuki) * 10 / 25;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * めざましビンタ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MezamasiBinta( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_MezamasiBinta },    // ワザ威力チェックハンドラ
    { BTL_EVENT_DAMAGEPROC_END, handler_MezamasiBinta_AfterDamage },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MezamasiBinta( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CheckSick(bpp, WAZASICK_NEMURI) ){
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
    if( BPP_CheckSick(bpp, WAZASICK_NEMURI) ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->pokeID[0] = target_pokeID;
      param->poke_cnt = 1;
      param->sickCode = WAZASICK_NEMURI;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * きつけ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kituke( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Kituke },    // ワザ威力チェックハンドラ
    { BTL_EVENT_DAMAGEPROC_END, handler_Kituke_AfterDamage },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // マヒ状態の相手に威力２倍
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CheckSick(bpp, WAZASICK_MAHI) ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
static void handler_Kituke_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
    if( BPP_CheckSick(bpp, WAZASICK_MAHI) ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->pokeID[0] = target_pokeID;
      param->poke_cnt = 1;
      param->sickCode = WAZASICK_MAHI;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * プレゼント
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Present( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,            handler_Present_Pow },    // ワザ威力チェックハンドラ
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,  handler_Present_Check }, // ダメージワザ回復チェックハンドラ
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
      work[0] = 1;  // ダメージじゃなく回復させちゃうフラグ
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    if( !BPP_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP*  param = BTL_SVF_HANEX_Push( flowWk ,BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = target_pokeID;
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 4 );

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    }
    else
    {
      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * きりふだ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kirifuda( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Kirifuda },    // ワザ威力チェックハンドラ
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 wazaIdx, pp;

    wazaIdx = BPP_WAZA_SearchIdx( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    if( wazaIdx != PTL_WAZA_MAX ){
      pp = BPP_WAZA_GetPP( bpp, wazaIdx );
    }else{
      GF_ASSERT(0);
      pp = NELEMS(powTbl) - 1;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, powTbl[pp] );
  }
}
//----------------------------------------------------------------------------------
/**
 * おしおき
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Osioki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Osioki },    // ワザ威力チェックハンドラ
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    u32 pow, i;
    int rank, rankSum;

    rankSum = 0;
    for(i=0; i<NELEMS(statusTbl); ++i)
    {
      rank = BPP_GetValue( bpp, statusTbl[i] );
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
 * れんぞくぎり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_RenzokuGiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_RenzokuGiri },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_RenzokuGiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 cnt = BPP_GetWazaContCounter( bpp );
    u32 ratio, i;

    for(i=0, ratio=1; i<cnt; ++i){
      ratio *= 2;
    }

    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(ratio) );
  }
}
//----------------------------------------------------------------------------------
/**
 * ダメおし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Dameosi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Dameosi },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Dameosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_DAMAGED) ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * けたぐり・くさむすび
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ketaguri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Ketaguri },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ketaguri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    u32 heavy = BPP_GetWeight( bpp );
    u32 pow;

    if( heavy >= 200 ){
      pow = 120;
    }else if( heavy >= 100 ){
      pow = 100;
    }else if( heavy >= 50 ){
      pow = 80;
    }else if( heavy >= 25 ){
      pow = 60;
    }else if( heavy >= 10 ){
      pow = 40;
    }else{
      pow = 20;
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ウェザーボール
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_WeatherBall( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM, handler_WeatherBall_Type },  // ワザパラメータチェックハンドラ
    { BTL_EVENT_WAZA_POWER, handler_WeatherBall_Pow },   // ワザ威力チェックハンドラ
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
 * たつまき・かぜおこし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tatumaki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Tatumaki_checkHide },  // 消えポケヒットチェック
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Tatumaki },     // ダメージ最終チェック
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
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_SORAWOTOBU) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * スカイアッパー
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SkyUpper( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_SkyUpper },  // 消えポケヒットチェック
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
 * かみなり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kaminari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Kaminari_checkHide },      // 消えポケヒットチェック
    { BTL_EVENT_EXCUSE_CALC_HIT,   handler_Kaminari_excuseHitCalc },  // ヒット確率計算スキップ
    { BTL_EVENT_WAZA_HIT_RATIO,    handler_Kaminari_hitRatio },       // 命中率計算
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
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * じしん
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Jisin( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Jisin_checkHide },  // 消えポケヒットチェック
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Jisin_damage },     // ダメージ最終チェック
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
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_ANAWOHORU) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * さばきのつぶて
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SabakiNoTubute( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_SabakiNoTubute },  // わざパラメータチェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SabakiNoTubute( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u8 typeTbl[] = {
    POKETYPE_KAKUTOU, POKETYPE_HIKOU, POKETYPE_DOKU,   POKETYPE_JIMEN,
    POKETYPE_IWA,     POKETYPE_MUSHI, POKETYPE_GHOST,  POKETYPE_HAGANE,
    POKETYPE_HONOO,   POKETYPE_MIZU,  POKETYPE_KUSA,   POKETYPE_DENKI,
    POKETYPE_ESPER,   POKETYPE_KOORI, POKETYPE_DRAGON, POKETYPE_AKU,
  };
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 item = BPP_GetItem( bpp );
    u8 type = POKETYPE_NORMAL;

    switch( item ){
    case ITEM_HINOTAMAPUREETO:  type = POKETYPE_HONOO; break;
    case ITEM_SIZUKUPUREETO:    type = POKETYPE_MIZU; break;
    case ITEM_IKAZUTIPUREETO:   type = POKETYPE_DENKI; break;
    case ITEM_MIDORINOPUREETO:  type = POKETYPE_KUSA; break;
    case ITEM_TURARANOPUREETO:  type = POKETYPE_KOORI; break;
    case ITEM_KOBUSINOPUREETO:  type = POKETYPE_KAKUTOU; break;
    case ITEM_MOUDOKUPUREETO:   type = POKETYPE_DOKU; break;
    case ITEM_DAITINOPUREETO:   type = POKETYPE_JIMEN; break;
    case ITEM_AOZORAPUREETO:    type = POKETYPE_HIKOU; break;
    case ITEM_HUSIGINOPUREETO:  type = POKETYPE_ESPER; break;
    case ITEM_TAMAMUSIPUREETO:  type = POKETYPE_MUSHI; break;
    case ITEM_GANSEKIPUREETO:   type = POKETYPE_IWA; break;
    case ITEM_MONONOKEPUREETO:  type = POKETYPE_GHOST; break;
    case ITEM_RYUUNOPUREETO:    type = POKETYPE_DRAGON; break;
    case ITEM_KOWAMOTEPUREETO:  type = POKETYPE_AKU; break;
    case ITEM_KOUTETUPUREETO:   type = POKETYPE_HAGANE; break;
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}
//----------------------------------------------------------------------------------
/**
 * めざめるパワー
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MezameruPower( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_MezameruPower_Type },  // わざパラメータチェック
    { BTL_EVENT_WAZA_POWER,   handler_MezameruPower_Pow  },  // わざ威力チェック
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
    u32 val=0;
    if( PP_Get(pp, ID_PARA_hp_rnd, NULL) & 1){ val += 1; }
    if( PP_Get(pp, ID_PARA_pow_rnd, NULL) & 1){ val += 2; }
    if( PP_Get(pp, ID_PARA_def_rnd, NULL) & 1){ val += 4; }
    if( PP_Get(pp, ID_PARA_agi_rnd, NULL) & 1){ val += 8; }
    if( PP_Get(pp, ID_PARA_spepow_rnd, NULL) & 1){ val += 16; }
    if( PP_Get(pp, ID_PARA_spedef_rnd, NULL) & 1){ val += 32; }

    val = val * 15 / 63;
    while( val > NELEMS(typeTbl) ){ // テーブルサイズを越えることは有り得ないハズだが念のため
      val -= NELEMS(typeTbl);
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, typeTbl[val] );
  }
}
static void handler_MezameruPower_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
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
 * しぜんのめぐみ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SizenNoMegumi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_SizenNoMegumi_ExeCheck },    // わざ出し成否チェック
    { BTL_EVENT_DAMAGEPROC_END,        handler_SizenNoMegumi_AfterDamage }, // わざ出し確定
    { BTL_EVENT_WAZA_PARAM,   handler_SizenNoMegumi_Type },               // わざパラメータチェック
    { BTL_EVENT_WAZA_POWER,   handler_SizenNoMegumi_Pow  },               // わざ威力チェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SizenNoMegumi_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 item = BPP_GetItem( bpp );
    if( BTL_CALC_ITEM_GetParam(item, ITEM_PRM_SIZENNOMEGUMI_ATC) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
static void handler_SizenNoMegumi_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 item = BPP_GetItem( bpp );
    s32 type = BTL_CALC_ITEM_GetParam( item, ITEM_PRM_SIZENNOMEGUMI_TYPE );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}
static void handler_SizenNoMegumi_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 item = BPP_GetItem( bpp );
    s32 pow = BTL_CALC_ITEM_GetParam( item, ITEM_PRM_SIZENNOMEGUMI_ATC );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
static void handler_SizenNoMegumi_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
    param->pokeID = pokeID;
    param->itemID = ITEM_DUMMY_DATA;
  }
}
//----------------------------------------------------------------------------------
/**
 * はたきおとす
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Hatakiotosu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END,      handler_Hatakiotosu }, // ダメージ後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Hatakiotosu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
    u16 itemID = BPP_GetItem( bpp );
    if( itemID != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      param->pokeID = target_pokeID;
      param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Hatakiotosu );
      HANDEX_STR_AddArg( &param->exStr, target_pokeID );
      HANDEX_STR_AddArg( &param->exStr, itemID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * マジックコート
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MagicCoat( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_MagicCoat },           // 未分類ワザ処理
    { BTL_EVENT_CHECK_WAZA_ROB,               handler_MagicCoat_Rob },       // ワザ乗っ取り
    { BTL_EVENT_TURNCHECK_BEGIN,              handler_MagicCoat_TurnCheck }, // ターンチェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MagicCoat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MagicCoatReady );
    HANDEX_STR_AddArg( &param->str, pokeID );
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_MagicCoat_Rob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  if( atkPokeID != pokeID )
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag(waza, WAZAFLAG_MagicCoat) )
    {
      BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );

      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID, pokeID );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, atkPokeID );
      HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_MagicCoatExe );
      HANDEX_STR_AddArg( str, atkPokeID );
      HANDEX_STR_AddArg( str, waza );
    }
  }
}
static void handler_MagicCoat_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ターンチェックで自殺
  BTL_EVENT_FACTOR_Remove( myHandle );
}
//----------------------------------------------------------------------------------
/**
 * よこどり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yokodori( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Yokodori },        // 未分類ワザ処理
    { BTL_EVENT_CHECK_WAZA_ROB,               handler_Yokodori_Rob },    // 横取り判定
    { BTL_EVENT_TURNCHECK_BEGIN,              handler_Yokodori_TurnCheck }, // ターンチェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Yokodori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_YokodoriReady );
    HANDEX_STR_AddArg( &param->str, pokeID );
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_Yokodori_Rob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  if( atkPokeID != pokeID )
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );

    if( WAZADATA_GetFlag(waza, WAZAFLAG_Yokodori) )
    {
      BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );

      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID, pokeID );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, atkPokeID );
      HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_YokodoriExe );
      HANDEX_STR_AddArg( str, atkPokeID );
      HANDEX_STR_AddArg( str, waza );
    }
  }
}
static void handler_Yokodori_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_EVENT_FACTOR_Remove( myHandle );
}
//----------------------------------------------------------------------------------
/**
 * どろぼう・ほしがる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Dorobou( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END,      handler_Dorobou }, // ダメージ後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Dorobou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetItem(self) == ITEM_DUMMY_DATA )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
      if( BPP_GetItem(target) != ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_SWAP_ITEM* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SWAP_ITEM, pokeID );
        param->pokeID = target_pokeID;
        param->fSucceedMsg = TRUE;
        param->succeedStrID = BTL_STRID_SET_Dorobou;
        param->succeedStrArgCnt = 2;
        param->succeedStrArgs[0] = target_pokeID;
        param->succeedStrArgs[1] = BPP_GetItem( target );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * トリック・すりかえ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Trick( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END,      handler_Trick }, // ダメージ後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Trick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetItem(self) != ITEM_DUMMY_DATA )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
      if( BPP_GetItem(target) != ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_SWAP_ITEM* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SWAP_ITEM, pokeID );
        param->pokeID = target_pokeID;
        param->fSucceedMsg = TRUE;
        param->succeedStrID = BTL_STRID_SET_Trick;
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * マグニチュード
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Magnitude( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Jisin_checkHide },  // 消えポケヒットチェック
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Jisin_damage    },  // ダメージ最終チェック
    { BTL_EVENT_WAZA_POWER,        handler_Magnitude_pow   },  // ワザ威力決定
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Magnitude_effect},  // 効果確定
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
    BTL_Printf("マグニチュード idx=%d, 威力値=%d\n", i, powTbl[i].pow);
  }
}
static void handler_Magnitude_effect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Magnitude1 + work[0] );
  }
}
//----------------------------------------------------------------------------------
/**
 * なみのり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Naminori( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Naminori_checkHide },  // 消えポケヒットチェック
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Naminori },     // ダメージ最終チェック
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
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_DIVING) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ふみつけ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Fumituke( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Fumituke },     // ダメージ最終チェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Fumituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_TIISAKUNARU) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * みねうち
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Mineuti( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK, handler_Mineuti },    // こらえるチェックハンドラ
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
 * こらえる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Koraeru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Koraeru_ExeCheck }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_KORAERU_CHECK,       handler_Koraeru },          // こらえるチェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Koraeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 成功率がだんだん下がるテーブル 1/2, 1/4, 1/8, ---
  static const u8 randRange[] = {
    2, 4, 8,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 counter = BPP_GetWazaContCounter( bpp );

    if( counter )
    {
      if( counter >= NELEMS(randRange) ){
        counter = NELEMS(randRange) - 1;
      }
      if( GFL_STD_MtRand( randRange[counter] ) != 0 )
      {
        // 連続利用による失敗。失敗したらハンドラごと消滅することで、こらえるチェックに反応しない。
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
 * まもる・みきり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Mamoru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Mamoru_ExeCheck }, // ワザ出し成否チェックハンドラ
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Mamoru },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Mamoru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 成功率がだんだん下がるテーブル 1/2, 1/4, 1/8, ---
  static const u8 randRange[] = {
    2, 4, 8,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 counter = BPP_GetWazaContCounter( bpp );

    if( counter )
    {
      if( counter >= NELEMS(randRange) ){
        counter = NELEMS(randRange) - 1;
      }
      if( GFL_STD_MtRand( randRange[counter] ) != 0 )
      {
        // 連続利用による失敗。失敗したらハンドラごと消滅することで、その後のイベントに反応しない。
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

    flagParam = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_TURNFLAG, pokeID );
    flagParam->pokeID = pokeID;
    flagParam->flag = BPP_TURNFLG_MAMORU;
  }
}
//----------------------------------------------------------------------------------
/**
 * がまん
 */
//----------------------------------------------------------------------------------
enum {
  GAMAN_STATE_1ST = 0,
  GAMAN_STATE_2ND,
  GAMAN_STATE_3RD,
};

static BTL_EVENT_FACTOR*  ADD_Gaman( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,     handler_Gaman },          //
    { BTL_EVENT_REQWAZA_MSG,          handler_Gaman_WazaMsg },
    { BTL_EVENT_WAZA_EXECUTE_CHECK,   handler_Gamen_ExeCheck },
    { BTL_EVENT_WAZA_EXECUTE_FAIL,    handler_Gaman_Fail },
    { BTL_EVENT_DECIDE_TARGET,        handler_Gaman_Target },
    { BTL_EVENT_WAZA_DMG_PROC2,       handler_Gaman_Damage },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Gaman( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    switch( work[0] ){
    case GAMAN_STATE_1ST:
      // １ターン目：自分をワザロック状態にして、処理を抜ける。
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        param->sickID = WAZASICK_WAZALOCK;
        param->sickCont = BPP_SICKCONT_MakePermanent();
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
        work[WORKIDX_STICK] = 1;
        work[0] = GAMAN_STATE_2ND;
      }
      break;
    case GAMAN_STATE_2ND:
      // ２ターン目：何もせず処理を抜ける
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
        work[0] = GAMAN_STATE_3RD;
      }
      break;
    case GAMAN_STATE_3RD:
      // ３ターン目：貼り付き＆ワザロック状態解除
      gaman_ReleaseStick( flowWk, pokeID, work );
      break;
    }
  }
}
static void handler_Gaman_WazaMsg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_STR_PARAMS* param = (BTL_HANDEX_STR_PARAMS*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );

    switch( work[0] ){
    case GAMAN_STATE_2ND:
      HANDEX_STR_Setup( param, BTL_STRTYPE_SET, BTL_STRID_SET_Gaman );
      HANDEX_STR_AddArg( param, pokeID );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
      break;
    case GAMAN_STATE_3RD:
      HANDEX_STR_Setup( param, BTL_STRTYPE_SET, BTL_STRID_SET_GamanEnd );
      HANDEX_STR_AddArg( param, pokeID );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
      break;
    }
  }
}
static void handler_Gaman_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[0] == GAMAN_STATE_3RD)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 t, cnt;
    for(t=0; t<3; ++t)
    {
      cnt = BPP_WAZADMGREC_GetCount( bpp, t );
      if( cnt ){
        BPP_WAZADMG_REC  rec;
        BPP_WAZADMGREC_Get( bpp, t, 0, &rec );
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, rec.pokeID );
        break;
      }
    }
  }
}
static void handler_Gaman_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[0] == GAMAN_STATE_3RD)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 dmg_sum = gaman_getTotalDamage( bpp );
    if( dmg_sum ){
      dmg_sum *= 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, dmg_sum );
      BTL_Printf("がまん３ターン目：ダメージ吐き出し=%d\n", dmg_sum);
    }
  }
}
static void handler_Gamen_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] == GAMAN_STATE_3RD)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 dmg_sum = gaman_getTotalDamage( bpp );
    if( dmg_sum == 0 ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
      BTL_Printf("がまん３ターン目：ダメージ受けてないので失敗\n");
    }
  }
}
static void handler_Gaman_Fail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 失敗したらすぐに貼り付き＆ワザロック状態解除
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    gaman_ReleaseStick( flowWk, pokeID, work );
  }
}
static void gaman_ReleaseStick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] ){
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
    param->sickCode = WAZASICK_WAZALOCK;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fStdMsgDisable = TRUE;
    work[ WORKIDX_STICK ] = 0;
  }
}
static u32 gaman_getTotalDamage( const BTL_POKEPARAM* bpp )
{
  u32 t, i, cnt, dmg_sum = 0;
  BPP_WAZADMG_REC  rec;
  for(t=0; t<3; ++t)
  {
    cnt = BPP_WAZADMGREC_GetCount( bpp, t );
    for(i=0; i<cnt; ++i)
    {
      BPP_WAZADMGREC_Get( bpp, t, i, &rec );
      dmg_sum += rec.damage;
    }
  }
  return dmg_sum;
}

//----------------------------------------------------------------------------------
/**
 * リサイクル
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Recycle( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Recycle },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Recycle( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 itemID = BPP_GetConsumedItem( bpp );
    if( itemID != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );

      param->itemID = itemID;
      param->pokeID = pokeID;

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Recycle );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, itemID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * サイコシフト
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PsycoShift( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_PsycoShift },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_PsycoShift( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    WazaSick sick = BPP_GetPokeSick( bpp );
    // 自分がポケモン系の状態異常で、相手がそうでない場合のみ効果がある
    if( sick != WAZASICK_NULL )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
      if( BPP_GetPokeSick(target) == WAZASICK_NULL )
      {
        BTL_HANDEX_PARAM_ADD_SICK    *sick_param;
        BTL_HANDEX_PARAM_CURE_SICK   *cure_param;

        sick_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        sick_param->sickID = sick;
        sick_param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( sick );
        sick_param->poke_cnt = 1;
        sick_param->pokeID[0] = target_pokeID;
        sick_param->fAlmost = TRUE;

        cure_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        cure_param->sickCode = sick;
        cure_param->poke_cnt = 1;
        cure_param->pokeID[0] = pokeID;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いたみわけ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Itamiwake( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Itamiwake },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Itamiwake( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp_me = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* bpp_target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    // 両者の平均値にHPを合わせる
    u32 hp_me     = BPP_GetValue( bpp_me, BPP_HP );
    u32 hp_target = BPP_GetValue( bpp_target, BPP_HP );
    u32 hp_avrg = (hp_me + hp_target) / 2;

    BTL_HANDEX_PARAM_SHIFT_HP* param;
    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
    param->poke_cnt = 2;
    param->pokeID[0] = pokeID;
    param->volume[0] = hp_avrg - hp_me;
    param->pokeID[1] = target_pokeID;
    param->pokeID[1] = hp_avrg - hp_target;

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Itamiwake );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * はらだいこ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Haradaiko( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Haradaiko },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Haradaiko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 downHP = BPP_GetValue( bpp, BPP_MAX_HP ) / 2;
    u32 upVolume = BPP_RankEffectUpLimit( bpp, BPP_ATTACK );
    if( (BPP_GetValue(bpp, BPP_HP) > downHP) && (upVolume != 0) )
    {
      BTL_HANDEX_PARAM_SHIFT_HP     *hp_param;
      BTL_HANDEX_PARAM_RANK_EFFECT  *rank_param;
      BTL_HANDEX_PARAM_MESSAGE      *msg_param;

      hp_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
      hp_param->poke_cnt = 1;
      hp_param->pokeID[0] = pokeID;
      hp_param->volume[0] = -downHP;

      rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->rankType = BPP_ATTACK;
      rank_param->rankVolume = upVolume;
      rank_param->poke_cnt = 0;
      rank_param->pokeID[0] = pokeID;
      rank_param->fStdMsgDisable = TRUE;
      rank_param->fAlmost = TRUE;

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Haradaiko );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * フェイント
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Feint( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,  handler_Feint_NoEffect },      // ワザ無効チェックLv2
    { BTL_EVENT_CHECK_MAMORU_BREAK, handler_Feint_MamoruBreak },   // まもる無効化チェック
    { BTL_EVENT_DAMAGEPROC_END,       handler_Feint_AfterDamage },   // ダメージ処理後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Feint_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    if( !BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_MAMORU) ){
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_MAMORU) )
    {
      BTL_HANDEX_PARAM_TURNFLAG* flg_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      flg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
      flg_param->pokeID = target_pokeID;
      flg_param->flag = BPP_TURNFLG_MAMORU;

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Feint );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * つぼをつく
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TuboWoTuku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_TuboWoTuku },          // 未分類ワザハンドラ
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 valid_cnt, i;

    for(i=0, valid_cnt=0; i<NELEMS(rankType); ++i){
      if( BPP_IsRankEffectValid(bpp, rankType[i], 2) ){
        ++valid_cnt;
      }
    }

    if( valid_cnt )
    {
      u8 idx = GFL_STD_MtRand( valid_cnt );
      for(i=0; i<NELEMS(rankType); ++i)
      {
        if( BPP_IsRankEffectValid(bpp, rankType[i], 2) )
        {
          if( idx == 0 )
          {
            BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
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
 * ねむる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nemuru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Nemuru_exeCheck },
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Nemuru },          // 未分類ワザハンドラ
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
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

      if( BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_KAIHUKUHUUJI );
      }
      if( BPP_IsHPFull(bpp) )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_HPFULL );
      }
      {
        u16 tok = BPP_GetValue( bpp, BPP_TOKUSEI );
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP    *hp_param;
      BTL_HANDEX_PARAM_ADD_SICK      *sick_param;

      hp_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      hp_param->pokeID = pokeID;
      hp_param->recoverHP = (BPP_GetValue(bpp, BPP_MAX_HP) - BPP_GetValue(bpp, BPP_HP));

      if( BPP_GetPokeSick(bpp) != POKESICK_NULL )
      {
        BTL_HANDEX_PARAM_CURE_SICK *cure_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        cure_param->poke_cnt = 1;
        cure_param->pokeID[0] = pokeID;
        cure_param->sickCode = WAZASICK_EX_POKEFULL;
        cure_param->fStdMsgDisable = TRUE;
      }

      sick_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      sick_param->poke_cnt = 1;
      sick_param->pokeID[0] = pokeID;
      sick_param->sickID = WAZASICK_NEMURI;
      sick_param->sickCont = BTL_CALC_MakeWazaSickCont_Turn( 3 );
      sick_param->fExMsg = TRUE;
      sick_param->exStr.ID = BTL_STRID_SET_Nemuru;
      sick_param->exStr.argCnt = 1;
      sick_param->exStr.args[0] = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * メロメロ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Meromero( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,   handler_Meromero_CheckNoEffect },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Meromero_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    u8 my_sex = BPP_GetValue( self, BPP_SEX );
    u8 target_sex = BPP_GetValue( target, BPP_SEX );

    if( (my_sex == PTL_SEX_UNKNOWN)
    ||  (target_sex == PTL_SEX_UNKNOWN)
    ||  ( my_sex == target_sex )
    ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * テクスチャー２
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Texture2( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,   handler_Texture2_CheckNoEffect },
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Texture2 },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Texture2_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_GetPrevWazaID(bpp) == WAZANO_NULL ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
static void handler_Texture2( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    WazaID  waza = BPP_GetPrevWazaID( bpp );
    if( waza != WAZANO_NULL )
    {
      BTL_HANDEX_PARAM_CHANGE_TYPE* param;
      PokeType type = WAZADATA_GetType( waza );
      PokeType next_type = BTL_CALC_RandomResistType( type );

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
      param->next_type = PokeTypePair_MakePure( next_type );
      BTL_Printf("対象ポケ=%d, そのワザ=%d, そのタイプ=%d, 抵抗タイプ=%d\n",
        BPP_GetID(bpp), waza, type, param->next_type);
      param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * アンコール
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Encore( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Encore },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Encore( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, WAZASICK_ENCORE)
    &&  (BPP_GetPrevWazaID(target) != WAZANO_NULL)
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      u8 turns = BTL_CALC_RandRange( 3, 7 );
      param->sickID = WAZASICK_ENCORE;
      param->sickCont = BTL_CALC_MakeWazaSickCont_Turn( turns );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->fAlmost = FALSE;
      param->fExMsg = TRUE;
      param->exStr.type = BTL_STRTYPE_SET;
      param->exStr.ID = BTL_STRID_SET_Encore;
      param->exStr.argCnt = 1;
      param->exStr.args[0] = targetPokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ちょうはつ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Chouhatu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Chouhatu },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };

  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Chouhatu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, WAZASICK_TYOUHATSU) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      u8 turns = BTL_CALC_RandRange( 3, 5 );
      param->sickID = WAZASICK_TYOUHATSU;
      param->sickCont = BTL_CALC_MakeWazaSickCont_Turn( turns );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
/*
      param->fExMsg = TRUE;
      param->exStr.type = BTL_STRTYPE_SET;
      param->exStr.ID = BTL_STRID_SET_Chouhatu;
      param->exStr.argCnt = 1;
      param->exStr.args[0] = targetPokeID;
*/
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いちゃもん
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ichamon( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Ichamon },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };

  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ichamon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, WAZASICK_ICHAMON) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_ICHAMON;
      param->sickCont = BPP_SICKCONT_MakePermanent();
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
/*
      param->fExMsg = TRUE;
      param->exStr.type = BTL_STRTYPE_SET;
      param->exStr.ID = BTL_STRID_SET_Ichamon;
      param->exStr.argCnt = 1;
      param->exStr.args[0] = targetPokeID;
*/
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * かなしばり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kanasibari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Kanasibari },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };

  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kanasibari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, WAZASICK_KANASIBARI) )
    {
      WazaID  prevWaza = BPP_GetPrevWazaID( target );
      if( prevWaza != WAZANO_NULL )
      {
        u8 prevWazaIdx = BPP_WAZA_SearchIdx( target, prevWaza );
        BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        u8 turns = BTL_CALC_RandRange( 2, 5 );

        param->sickID = WAZASICK_KANASIBARI;
        param->sickCont = BPP_SICKCONT_MakeTurnParam( turns, prevWazaIdx );
        param->poke_cnt = 1;
        param->pokeID[0] = targetPokeID;

        param->fExMsg = TRUE;
        param->exStr.type = BTL_STRTYPE_SET;
        param->exStr.ID = BTL_STRID_SET_Kanasibari;
        param->exStr.argCnt = 2;
        param->exStr.args[0] = targetPokeID;
        param->exStr.args[1] = prevWaza;
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ふういん
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Fuuin( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Fuuin },          // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };

  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Fuuin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_FUIN) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );

      param->effect = BTL_FLDEFF_FUIN;
      param->cont = BPP_SICKCONT_MakePoke( pokeID );
      param->exStr.type = BTL_STRTYPE_SET;
      param->exStr.ID = BTL_STRID_SET_Fuuin;
      param->exStr.argCnt = 1;
      param->exStr.args[0] = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * アロマセラピー
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Alomatherapy( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Alomatherapy   },  // ワザ威力決定
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
 * いやしのすず
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_IyasiNoSuzu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_IyasiNoSuzu   },  // ワザ威力決定
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
    BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokeID( flowWk, pokeID );
    BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_MYSIDE_ALL, myPos );

    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, strID );

    cure_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
    cure_param->poke_cnt = BTL_SVFTOOL_ExpandPokeID( flowWk, expos, cure_param->pokeID );
    cure_param->sickCode = WAZASICK_EX_POKEFULL;
  }
}
//----------------------------------------------------------------------------------
/**
 * おきみやげ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Okimiyage( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Okimiyage   },  // ワザ威力決定
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

    kill_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    kill_param->pokeID = pokeID;

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = targetPokeID;
    rank_param->rankType = BPP_ATTACK;
    rank_param->rankVolume = -2;
    rank_param->fAlmost = TRUE;

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = targetPokeID;
    rank_param->rankType = BPP_SP_ATTACK;
    rank_param->rankVolume = -2;
    rank_param->fAlmost = TRUE;
  }
}
//----------------------------------------------------------------------------------
/**
 * うらみ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Urami( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Urami   },  // ワザ威力決定
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    WazaID prev_waza = BPP_GetPrevWazaID( bpp );
    u8 wazaIdx = BPP_WAZA_SearchIdx( bpp, prev_waza );

    if( wazaIdx != PTL_WAZA_MAX )
    {
      u8 volume = BPP_WAZA_GetPP( bpp, wazaIdx );
      if( volume > URAMI_DECREMENT_PP_VOLUME ){ volume = URAMI_DECREMENT_PP_VOLUME; }
      if( volume )
      {
        BTL_HANDEX_PARAM_PP*      decpp_param;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        decpp_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_DECREMENT_PP, pokeID );
        decpp_param->pokeID = targetPokeID;
        decpp_param->volume = volume;
        decpp_param->wazaIdx = wazaIdx;

        msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Urami );
        HANDEX_STR_AddArg( &msg_param->str, targetPokeID );
        HANDEX_STR_AddArg( &msg_param->str, prev_waza );
        HANDEX_STR_AddArg( &msg_param->str, volume );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * じこあんじ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_JikoAnji( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_JikoAnji   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_JikoAnji( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = pokeID;
    rank_param->attack     = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_attack  = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( target, BPP_AGILITY_RANK );


    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_JikoAnji );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
    HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ハートスワップ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HeartSwap( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_HeartSwap   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_HeartSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = pokeID;
    rank_param->attack     = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_attack  = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( target, BPP_AGILITY_RANK );

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = target_pokeID;
    rank_param->attack     = BPP_GetValue( self, BPP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( self, BPP_DEFENCE_RANK );
    rank_param->sp_attack  = BPP_GetValue( self, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence = BPP_GetValue( self, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( self, BPP_AGILITY_RANK );

    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HeartSwap );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * パワースワップ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PowerSwap( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerSwap   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_PowerSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = pokeID;
    rank_param->attack     = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->sp_attack  = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( self,   BPP_DEFENCE_RANK );
    rank_param->sp_defence = BPP_GetValue( self,   BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( self,   BPP_AGILITY_RANK );

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = target_pokeID;
    rank_param->attack     = BPP_GetValue( self,   BPP_ATTACK_RANK );
    rank_param->sp_attack  = BPP_GetValue( self,   BPP_SP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_defence = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( target, BPP_AGILITY_RANK );

    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_PowerSwap );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ガードスワップ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_GuardSwap( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GuardSwap   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_GuardSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = pokeID;
    rank_param->attack     = BPP_GetValue( self,   BPP_ATTACK_RANK );
    rank_param->sp_attack  = BPP_GetValue( self,   BPP_SP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_defence = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( self,   BPP_AGILITY_RANK );

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = target_pokeID;
    rank_param->attack     = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->sp_attack  = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( self,   BPP_DEFENCE_RANK );
    rank_param->sp_defence = BPP_GetValue( self,   BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( target, BPP_AGILITY_RANK );

    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_GuardSwap );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * パワートリック
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PowerTrick( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerTrick   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_PowerTrick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_STATUS* param;
    BTL_HANDEX_PARAM_SET_CONTFLAG*  flag_param;
    const BTL_POKEPARAM* bpp;

    bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );

    param->pokeID  = pokeID;
    param->attack  = BPP_GetValue_Base( bpp, BPP_DEFENCE );
    param->defence = BPP_GetValue_Base( bpp, BPP_ATTACK );
    param->fAttackEnable  = TRUE;
    param->fDefenceEnable = TRUE;

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_PowerTrick );
    HANDEX_STR_AddArg( &param->exStr, pokeID );

    flag_param = (BTL_HANDEX_PARAM_SET_CONTFLAG*)BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    flag_param->pokeID = pokeID;
    flag_param->flag = BPP_CONTFLG_POWERTRICK;
  }
}
//----------------------------------------------------------------------------------
/**
 * パワーシェア
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PowerShare( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerShare   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_PowerShare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_STATUS* param;
    const BTL_POKEPARAM *user, *target;
    u8 target_pokeID;
    u8 avrg, sp_avrg;

    target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    user = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    avrg = (BPP_GetValue_Base(user, BPP_ATTACK) + BPP_GetValue_Base(target, BPP_ATTACK)) / 2;
    sp_avrg = (BPP_GetValue_Base(user, BPP_SP_ATTACK) + BPP_GetValue_Base(target, BPP_SP_ATTACK)) / 2;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );
    param->pokeID = target_pokeID;
    param->attack = avrg;
    param->sp_attack = sp_avrg;
    param->fAttackEnable = TRUE;
    param->fSpAttackEnable = TRUE;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );
    param->pokeID = pokeID;
    param->attack = avrg;
    param->sp_attack = sp_avrg;
    param->fAttackEnable = TRUE;
    param->fSpAttackEnable = TRUE;

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_PowerShare );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ガードシェア
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_GuardShare( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GuardShare   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_GuardShare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_STATUS* param;
    const BTL_POKEPARAM *user, *target;
    u8 target_pokeID;
    u8 avrg, sp_avrg;

    target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    user = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    avrg = (BPP_GetValue_Base(user, BPP_DEFENCE) + BPP_GetValue_Base(target, BPP_DEFENCE)) / 2;
    sp_avrg = (BPP_GetValue_Base(user, BPP_SP_DEFENCE) + BPP_GetValue_Base(target, BPP_SP_DEFENCE)) / 2;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );
    param->pokeID = target_pokeID;
    param->defence = avrg;
    param->sp_defence = sp_avrg;
    param->fDefenceEnable = TRUE;
    param->fSpDefenceEnable = TRUE;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );
    param->pokeID = pokeID;
    param->defence = avrg;
    param->sp_defence = sp_avrg;
    param->fDefenceEnable = TRUE;
    param->fSpDefenceEnable = TRUE;

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_GuardShare );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ロックオン
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_LockON( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_LockON   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_LockON( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->sickID = WAZASICK_MUSTHIT_TARGET;
    param->sickCont = BPP_SICKCONT_MakeTurnParam( 2, targetPokeID );
    param->fExMsg = TRUE;
    param->exStr.type = BTL_STRTYPE_SET;
    param->exStr.ID = BTL_STRID_SET_LockOn;
    param->exStr.argCnt = 1;
    param->exStr.args[0] = pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * リフレクター
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Refrector( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Refrector   },  // 未分類ワザ
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
 * ひかりのかべ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HikariNoKabe( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_HikariNoKabe   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 5 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_HIKARINOKABE, cont, BTL_STRID_STD_HikariNoKabe );
}
//----------------------------------------------------------------------------------
/**
 * しんぴのまもり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SinpiNoMamori( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_SinpiNoMamori   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SinpiNoMamori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
#if 1
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 5 );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_SINPINOMAMORI, cont, BTL_STRID_STD_SinpiNoMamori );
#else
// @todo ワイドガードを試すための一時的措置
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 1 );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_WIDEGUARD, cont, BTL_STRID_STD_WideGuard );
#endif

}
//----------------------------------------------------------------------------------
/**
 * しろいきり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SiroiKiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_SiroiKiri   },  // 未分類ワザ
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
 * おいかぜ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Oikaze( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Oikaze   },  // 未分類ワザ
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
 * おまじない
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Omajinai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Omajinai   },  // 未分類ワザ
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
//----------------------------------------------------------------------------------
/**
 * まきびし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Makibisi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Makibisi   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Makibisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakePermanent();
  BtlSide side = BTL_MAINUTIL_PokeIDtoOpponentSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_MAKIBISI, cont, BTL_STRID_STD_Makibisi );
}
//----------------------------------------------------------------------------------
/**
 * どくびし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Dokubisi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Dokubisi   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Dokubisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakePermanent();
  BtlSide side = BTL_MAINUTIL_PokeIDtoOpponentSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_DOKUBISI, cont, BTL_STRID_STD_Dokubisi );
}
//----------------------------------------------------------------------------------
/**
 * ステルスロック
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_StealthRock( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_StealthRock   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_StealthRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakePermanent();
  BtlSide side = BTL_MAINUTIL_PokeIDtoOpponentSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_STEALTHROCK, cont, BTL_STRID_STD_StealthRock );
}
//----------------------------------------------------------------------------------
/**
 * ワイドガード
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_WideGuard( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_WideGuard   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_WideGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 1 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_WIDEGUARD, cont, BTL_STRID_STD_WideGuard );
}

//-------------------------------------
/**
 *  サイドエフェクトワザ共通
 */
//-------------------------------------
static void common_SideEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, BPP_SICK_CONT cont, u16 strID )
{
  BTL_Printf("ワシ=%d, 使ったポケ=%d, エフェクト=%d\n", pokeID, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK), effect);
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_HANDLER_SIDE_Add(side, effect, cont) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, strID );
      HANDEX_STR_AddArg( &param->str, side );
      BTL_Printf("サイドエフェクト[%d]発動　文字列=%d\n", effect, strID);
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * みかづきのまい
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MikadukiNoMai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_MikadukiNoMai   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* kill_param;
    BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

    eff_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_MIKADUKINOMAI;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, pokeID );

    kill_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    kill_param->pokeID = pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * いやしのねがい
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_IyasiNoNegai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_IyasiNoNegai   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* kill_param;
    BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

    eff_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_IYASINONEGAI;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, pokeID );

    kill_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    kill_param->pokeID = pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * ねがいごと
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Negaigoto( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Negaigoto   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Negaigoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* kill_param;
    BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

    eff_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_NEGAIGOTO;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * みらいよち・はめつのねがい
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Miraiyoti( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Miraiyoti   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Miraiyoti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    eff_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_DELAY_ATTACK;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, targetPokeID );
    eff_param->param[0] = 3;
    eff_param->param[1] = BTL_EVENT_FACTOR_GetSubID( myHandle );
    eff_param->param_cnt = 2;

    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Miraiyoti );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * みらいよち・はめつのねがい
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HametuNoNegai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_HametuNoNegai   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_HametuNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    eff_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_DELAY_ATTACK;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, targetPokeID );
    eff_param->param[0] = 3; // delay turn
    eff_param->param[1] = BTL_EVENT_FACTOR_GetSubID( myHandle );
    eff_param->param_cnt = 2;

    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HametuNoNegai );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * いえき
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ieki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Ieki   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK       *sick_param;
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    sick_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    sick_param->poke_cnt = 1;
    sick_param->pokeID[0] = target_pokeID;
    sick_param->sickID = WAZASICK_IEKI;
    sick_param->fAlmost = TRUE;
    sick_param->sickCont = BPP_SICKCONT_MakePermanent();
    HANDEX_STR_Setup( &sick_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Ieki );
    HANDEX_STR_AddArg( &sick_param->exStr, target_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * なりきり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Narikiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Narikiri   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Narikiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_CHANGE_TOKUSEI*  tok_param;

    tok_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
    tok_param->pokeID = pokeID;
    tok_param->tokuseiID = BPP_GetValue( target, BPP_TOKUSEI );
    HANDEX_STR_Setup( &tok_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Narikiri );
    HANDEX_STR_AddArg( &tok_param->exStr, pokeID );
    HANDEX_STR_AddArg( &tok_param->exStr, target_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * とんぼがえり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TonboGaeri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,  handler_TonboGaeri   },         // ダメージ直後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_TonboGaeri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_PARTY* party = BTL_SVFLOW_GetPartyData( flowWk, pokeID );
    u8 numCoverPos = BTL_SVFLOW_GetClientCoverPosCount( flowWk, pokeID );
    if( BTL_PARTY_GetAliveMemberCountRear(party, numCoverPos) )
    {
      BTL_HANDEX_PARAM_CHANGE_MEMBER* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Tonbogaeri );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, clientID );

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_MEMBER, pokeID );
      param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * こうそくスピン
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KousokuSpin( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,  handler_KousokuSpin   },         // ダメージ直後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_KousokuSpin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_CURE_SICK* cure_param;
    BTL_HANDEX_PARAM_SIDEEFF_REMOVE* side_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    BtlSide  side;

    if( BPP_CheckSick(bpp, WAZASICK_YADORIGI) ){
      cure_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      cure_param->poke_cnt = 1;
      cure_param->pokeID[0] = pokeID;
      cure_param->sickCode = WAZASICK_YADORIGI;
    }
    if( BPP_CheckSick(bpp, WAZASICK_BIND) ){
      cure_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      cure_param->poke_cnt = 1;
      cure_param->pokeID[0] = pokeID;
      cure_param->sickCode = WAZASICK_BIND;
    }

    side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
    if( BTL_HANDER_SIDE_IsExist(side, BTL_SIDEEFF_MAKIBISI)
    ){
      side_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
      side_param->side = side;
      BTL_CALC_BITFLG_Construction( side_param->flags, sizeof(side_param->flags) );
      BTL_CALC_BITFLG_Set( side_param->flags, BTL_SIDEEFF_MAKIBISI );

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KousokuSpin );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, WAZANO_MAKIBISI );
    }
    if( BTL_HANDER_SIDE_IsExist(side, BTL_SIDEEFF_DOKUBISI)
    ){
      side_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
      side_param->side = side;
      BTL_CALC_BITFLG_Construction( side_param->flags, sizeof(side_param->flags) );
      BTL_CALC_BITFLG_Set( side_param->flags, BTL_SIDEEFF_DOKUBISI );

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KousokuSpin );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, WAZANO_DOKUBISI );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * バトンタッチ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_BatonTouch( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_BatonTouch   },         // ダメージ直後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_BatonTouch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_PARTY* party = BTL_SVFLOW_GetPartyData( flowWk, pokeID );
    u8 numCoverPos = BTL_SVFLOW_GetClientCoverPosCount( flowWk, pokeID );
    if( BTL_PARTY_GetAliveMemberCountRear(party, numCoverPos) )
    {
      BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;
      BTL_HANDEX_PARAM_CHANGE_MEMBER* change_param;

      eff_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
      eff_param->effect = BTL_POSEFF_BATONTOUCH;
      eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, pokeID );
      eff_param->param[0] = pokeID;
      eff_param->param_cnt = 1;

      change_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_MEMBER, pokeID );
      change_param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * テレポート
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Teleport( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Teleport   },         // 未分類ワザ
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_Teleport_ExeCheck },  // 実行チェック
    { BTL_EVENT_NIGERU_EXMSG,       handler_Teleport_ExMsg },     // 逃げるときの特殊メッセージ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Teleport_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_SVFLOW_GetCompetitor(flowWk) != BTL_COMPETITOR_WILD ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
  }
}
static void handler_Teleport( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_QUIT_BATTLE, pokeID );
  }
}
static void handler_Teleport_ExMsg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, TRUE) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Teleport );
    HANDEX_STR_AddArg( &param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * なげつける
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nagetukeru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_Nagetukeru_ExeCheck },
    { BTL_EVENT_WAZA_EXECUTE_FIX,   handler_Nagetukeru_ExeFix   },
    { BTL_EVENT_WAZA_POWER,         handler_Nagetukeru_WazaPower},
    { BTL_EVENT_WAZA_DMG_AFTER,     handler_Nagetukeru_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Nagetukeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 itemID = BPP_GetItem( bpp );
    if( (itemID == ITEM_DUMMY_DATA)
    ||  (BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_NAGETUKERU_ATC) == 0 )
    ){
      BTL_EVENTVAR_RewriteValue(BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER);
    }
  }
}
static void handler_Nagetukeru_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 itemID = BPP_GetItem( bpp );
    if( itemID != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_SET_ITEM* param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      param->pokeID = pokeID;
      param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Nagetukeru );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, itemID );
      work[0] = itemID;
    }
  }
}
static void handler_Nagetukeru_WazaPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 pow = BTL_CALC_ITEM_GetParam( work[0], ITEM_PRM_NAGETUKERU_ATC );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
static void handler_Nagetukeru_DmgAfter( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u16 itemID = work[0];
    if( itemID != ITEM_DUMMY_DATA )
    {
      int equip = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_NAGETUKERU_EFF );
      if( equip )
      {
         BTL_HANDEX_PARAM_EQUIP_ITEM*  param;
         param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_EQUIP_ITEM, pokeID );
         param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
         param->itemID = itemID;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * でんじふゆう
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_DenjiFuyuu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_DenjiFuyuu   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_DenjiFuyuu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK*  param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->sickID = WAZASICK_FLYING;
    param->sickCont = BPP_SICKCONT_MakeTurn( 5 );
    param->fExMsg = TRUE;
    param->exStr.ID = BTL_STRID_SET_DenjiFuyu;
    param->exStr.argCnt = 1;
    param->exStr.args[0] = pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * てだすけ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tedasuke( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,    handler_Tedasuke_Ready },       // 未分類ワザハンドラ
    { BTL_EVENT_WAZA_POWER,         handler_Tedasuke_WazaPow },     // ワザ威力チェック
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_Tedasuke_TurnCheck },   // ターンチェック開始ハンドラ
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
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

      if( !BPP_IsDead(bpp) )
      {
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Tedasuke );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_AddArg( &param->str, target_pokeID );

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
  BTL_EVENT_FACTOR_Remove( myHandle );  // ターンチェックで強制自殺
}
//----------------------------------------------------------------------------------
/**
 * ふくろだたき
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_FukuroDataki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_COUNT,  handler_FukuroDataki },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_FukuroDataki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_PARTY* party = BTL_SVFLOW_GetPartyData( flowWk, pokeID );
    const BTL_POKEPARAM* bpp;
    u32 cnt_max, cnt, i;

    cnt_max = cnt = BTL_PARTY_GetMemberCount( party );
    for(i=0; i<cnt_max; ++i){
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( BPP_IsDead(bpp)
      ||  (BPP_GetPokeSick(bpp) != POKESICK_NULL)
      ){
        --cnt;
      }
    }
    if( cnt ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_HITCOUNT, cnt );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * ねこだまし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nekodamasi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,       handler_Nekodamasi },     // ワザ出し成否チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Nekodamasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetTurnCount(bpp) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * あさのひざし・こうごうせい・つきのひかり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_AsaNoHizasi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RECOVER_HP_RATIO,       handler_AsaNoHizasi },     // HP回復率計算ハンドラ
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
 * そらをとぶ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SoraWoTobu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_SoraWoTobu_TameStart },     // 溜め開始
    { BTL_EVENT_TAME_RELEASE,     handler_SoraWoTobu_TameRelease },   // 溜め解放
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SoraWoTobu_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SoraWoTobu );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_SoraWoTobu_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
  }
}
//----------------------------------------------------------------------------------
/**
 * シャドーダイブ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_ShadowDive( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,         handler_ShadowDive_TameStart },     // 溜め開始
    { BTL_EVENT_TAME_RELEASE,       handler_ShadowDive_TameRelease },   // 溜め解放
    { BTL_EVENT_CHECK_MAMORU_BREAK, handler_Feint_MamoruBreak },        // まもる無効化チェック
    { BTL_EVENT_DAMAGEPROC_END,     handler_ShadowDive_AfterDamage },   // ダメージ処理後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_ShadowDive_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_ShadowDive );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_ShadowDive_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
  }
}
static void handler_ShadowDive_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_MAMORU) )
    {
      BTL_HANDEX_PARAM_TURNFLAG* flg_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      flg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
      flg_param->pokeID = target_pokeID;
      flg_param->flag = BPP_TURNFLG_MAMORU;

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MamoruBreak );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * とびはねる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tobihaneru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Tobihaneru_TameStart },     // 溜め開始
    { BTL_EVENT_TAME_RELEASE,     handler_Tobihaneru_TameRelease },   // 溜め解放
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Tobihaneru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Tobihaneru );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_Tobihaneru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
  }
}
//----------------------------------------------------------------------------------
/**
 * ダイビング
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Diving( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Diving_TameStart },     // 溜め開始
    { BTL_EVENT_TAME_RELEASE,     handler_Diving_TameRelease },   // 溜め解放
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Diving_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_DIVING;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Diving );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_Diving_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_DIVING;
  }
}
//----------------------------------------------------------------------------------
/**
 * あなをほる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_AnaWoHoru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_AnaWoHoru_TameStart },     // 溜め開始
    { BTL_EVENT_TAME_RELEASE,     handler_AnaWoHoru_TameRelease },   // 溜め解放
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_AnaWoHoru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_ANAWOHORU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_AnaWoHoru );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_AnaWoHoru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_ANAWOHORU;
  }
}
//----------------------------------------------------------------------------------
/**
 * ソーラービーム
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SolarBeam( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_TAMETURN_SKIP,  handler_SolarBeam_TameSkip },    // 溜めスキップ判定
    { BTL_EVENT_TAME_START,           handler_SolarBeam_TameStart },   // 溜め開始
    { BTL_EVENT_WAZA_POWER,           handler_SolarBeam_Power     },   // ワザ威力決定
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
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SolarBeam );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
static void handler_SolarBeam_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  ( BTL_FIELD_GetWeather() == BTL_WEATHER_RAIN )
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.5) );
  }
}
//----------------------------------------------------------------------------------
/**
 * かまいたち
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kamaitati( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Kamaitati_TameStart },     // 溜め開始
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kamaitati_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Kamaitati );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ゴッドバード
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_GodBird( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_GodBird_TameStart },     // 溜め開始
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_GodBird_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_GodBird );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ロケットずつき
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_RocketZutuki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_RocketZutuki_TameStart },     // 溜め開始
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

    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RocketZutuki );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );

    rank_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = pokeID;
    rank_param->rankType = WAZA_RANKEFF_DEFENCE;
    rank_param->rankVolume = 1;
    rank_param->fAlmost = TRUE;
  }
}
//------------------------------------------------------------------------------
/**
 *  ついばむ・むしくい
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Tuibamu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END,  handler_Tuibamu },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Tuibamu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    u16 itemID = BPP_GetItem( target );
    if( ITEM_CheckNuts(itemID) )
    {
      BTL_HANDEX_PARAM_SET_ITEM* item_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      BTL_HANDEX_PARAM_EQUIP_ITEM* eq_param;

      item_param->pokeID = targetPokeID;
      item_param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &item_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Tuibamu );
      HANDEX_STR_AddArg( &item_param->exStr, pokeID );
      HANDEX_STR_AddArg( &item_param->exStr, itemID );

      eq_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_EQUIP_ITEM, pokeID );
      eq_param->header.failSkipFlag = TRUE;
      eq_param->itemID = itemID;
      eq_param->pokeID = pokeID;
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  わるあがき
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_Waruagaki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END,  handler_Waruagaki },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Waruagaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_SHIFT_HP* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->volume[0] = -BTL_CALC_QuotMaxHP( bpp, 4 );
  }
}
//----------------------------------------------------------------------------------
/**
 * ばかじから
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Bakajikara( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END,       handler_Bakajikara },   // ダメージ処理後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Bakajikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->rankType = WAZA_RANKEFF_ATTACK;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->rankType = WAZA_RANKEFF_DEFENCE;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;
  }
}
//----------------------------------------------------------------------------------
/**
 * みちづれ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Michidure( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,    handler_Michidure_Ready },       // 未分類ワザハンドラ
    { BTL_EVENT_WAZA_DMG_AFTER,     handler_Michidure_WazaDamage },  // ワザダメージ処理後
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_Michidure_TurnCheck },   // ターンチェック開始ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Michidure_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MichidureReady );
    HANDEX_STR_AddArg( &param->str, pokeID );

    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_Michidure_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_KILL* kill_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      kill_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
      kill_param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MichidureDone );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );

      BTL_EVENT_FACTOR_Remove( myHandle );  // 道連れ成功で自殺
    }
  }
}
static void handler_Michidure_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_EVENT_FACTOR_Remove( myHandle );  // ターンチェックで強制自殺
}
//----------------------------------------------------------------------------------
/**
 * おんねん
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Onnen( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,    handler_Onnen_Ready },       // 未分類ワザハンドラ
    { BTL_EVENT_WAZA_DMG_AFTER,     handler_Onnen_WazaDamage },  // ワザダメージ処理後
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_Onnen_TurnCheck },   // ターンチェック開始ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Onnen_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_OnnenReady );
    HANDEX_STR_AddArg( &param->str, pokeID );

    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_Onnen_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_PP* pp_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      WazaID  waza;

      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      const BTL_POKEPARAM* targetPoke = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

      waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );

      pp_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_DECREMENT_PP, pokeID );
      pp_param->pokeID = target_pokeID;
      pp_param->wazaIdx = BPP_WAZA_SearchIdx( targetPoke, waza );
      pp_param->volume = BPP_WAZA_GetPP( targetPoke, pp_param->wazaIdx ) * -1;

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_OnnenDone );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
      HANDEX_STR_AddArg( &msg_param->str, waza );

      BTL_EVENT_FACTOR_Remove( myHandle );  // 道連れ成功で自殺
    }
  }
}
static void handler_Onnen_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_EVENT_FACTOR_Remove( myHandle );  // ターンチェックで強制自殺
}

//----------------------------------------------------------------------------------
/**
 * ちいさくなる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tiisakunaru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Tiisakunaru   },  // ワザ出し確定決定
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Tiisakunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_TIISAKUNARU;
  }
}
//----------------------------------------------------------------------------------
/**
 * まるくなる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Marukunaru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Marukunaru   },  // ワザ威力決定
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Marukunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_MARUKUNARU;
  }
}
//----------------------------------------------------------------------------------
/**
 * はねやすめ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Haneyasume( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Haneyasume   },  // ワザ出し確定決定
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Haneyasume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_TURNFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_TURNFLG_FLYING;
  }
}
//----------------------------------------------------------------------------------
/**
 * きあいパンチ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KiaiPunch( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_BEFORE_FIGHT,  handler_KiaiPunch   },  // ターン最初のワザシーケンス直前
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_KiaiPunch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_TURNFLAG* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_TURNFLAG, pokeID );
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    param->pokeID = pokeID;
    param->flag = BPP_TURNFLG_MUST_SHRINK;

    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KiaiPunch );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}


//------------------------------------------------------------------------------------------------------
/**
 *  他のワザを呼び出すワザ
 */
//------------------------------------------------------------------------------------------------------

// 除外するワザのテーブル
// ※※ 必ず昇順にソートしておくこと！！ ※※
static const u16 ReqWazaExcludeTbl[] = {
  WAZANO_YUBIWOHURU,    WAZANO_OUMUGAESI,   WAZANO_WARUAGAKI,
  WAZANO_SUKETTI,       WAZANO_NEGOTO,      WAZANO_SIZENNOTIKARA,
  WAZANO_NEKONOTE,      WAZANO_FEINTO,      WAZANO_SAKIDORI,
  WAZANO_SAKIDORI,      WAZANO_MANEKKO,     WAZANO_OSYABERI,
};

static BOOL isReqWazaExclude( WazaID waza )
{
  int i;
  for(i=0; i<NELEMS(ReqWazaExcludeTbl); ++i)
  {
    if( ReqWazaExcludeTbl[i] == waza ){
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * ゆびをふる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_YubiWoFuru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_YubiWoFuru     },  // 他ワザ呼び出し（順序計算前から）
    { BTL_EVENT_REQWAZA_MSG,            handler_YubiWoFuru_Msg },  // 他ワザ実行時メッセージ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_YubiWoFuru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaID     waza = BTL_CALC_RandWaza( ReqWazaExcludeTbl, NELEMS(ReqWazaExcludeTbl) );
    BtlPokePos pos  = BTL_SVFLOW_ReqWazaTargetAuto( flowWk, pokeID, waza );

    #if 0
    #ifdef PM_DEBUG
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
      waza = WAZANO_MAHHAPANTI;
      pos  = BTL_SVFLOW_ReqWazaTargetAuto( flowWk, pokeID, waza );
    }
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){
      waza = WAZANO_MAMORU;
      pos  = BTL_SVFLOW_ReqWazaTargetAuto( flowWk, pokeID, waza );
    }
    #endif
    #endif

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  waza );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
  }
}

static void handler_YubiWoFuru_Msg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    HANDEX_STR_Setup( str, BTL_STRTYPE_STD, BTL_STRID_STD_YubiWoFuru );
    HANDEX_STR_AddArg( str, waza );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
//----------------------------------------------------------------------------------
/**
 * しぜんのちから
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SizenNoTikara( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_SizenNoTikara     },  // 他ワザ呼び出し（順序計算前から）
    { BTL_EVENT_REQWAZA_MSG,            handler_SizenNoTikara_Msg },  // 他ワザ実行時メッセージ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SizenNoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlLandForm  land = BTL_SVFLOW_GetLandForm( flowWk );
    WazaID  waza;
    BtlPokePos pos;

    // @@@ 本来は地形に応じてもっとフクザツに
    waza = (land & 1)? WAZANO_HAIDOROPONPU : WAZANO_REITOUBIIMU;
    pos  = BTL_SVFLOW_ReqWazaTargetAuto( flowWk, pokeID, waza );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  waza );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
  }
}
static void handler_SizenNoTikara_Msg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    HANDEX_STR_Setup( str, BTL_STRTYPE_STD, BTL_STRID_STD_SizenNoTikara );
    HANDEX_STR_AddArg( str, waza );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
//----------------------------------------------------------------------------------
/**
 * ねこのて
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nekonote( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_Nekonote     },         // 他ワザ呼び出し（順序計算前から）
    { BTL_EVENT_REQWAZA_PARAM,          handler_Nekonote_CheckParam },  // 他ワザパラメータチェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Nekonote( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_PARTY* party = BTL_SVFLOW_GetPartyData( flowWk, pokeID );
    u16* wazaAry = BTL_SVFLOW_GetHandlerTmpWork( flowWk );
    const BTL_POKEPARAM* bpp;

    u8 frontMemberCnt, memberCnt, wazaCnt, i;

    frontMemberCnt = BTL_SVFLOW_GetClientCoverPosCount( flowWk, pokeID );
    memberCnt = BTL_PARTY_GetMemberCount( party );
    for(i=frontMemberCnt, wazaCnt=0; i<memberCnt; ++i)
    {
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( !BPP_IsDead(bpp) )
      {
        u8 pokeWazaMax = BPP_WAZA_GetCount( bpp );
        u8 j;
        u16 waza;
        for(j=0; j<pokeWazaMax; ++j)
        {
          waza = BPP_WAZA_GetID( bpp, j );
          if( !isReqWazaExclude(waza) ){
            wazaAry[wazaCnt++] = waza;
          }
        }
      }
    }

    if( wazaCnt )
    {
      u8 idx = GFL_STD_MtRand( wazaCnt );
      BtlPokePos  pos= BTL_SVFLOW_ReqWazaTargetAuto( flowWk, pokeID, wazaAry[idx] );

      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  wazaAry[idx] );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
    }
  }
}
static void handler_Nekonote_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( waza == WAZANO_NULL ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ねごと
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Negoto( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_Negoto     },         // 他ワザ呼び出し（順序計算前から）
    { BTL_EVENT_REQWAZA_PARAM,          handler_Negoto_CheckParam },  // 他ワザパラメータチェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Negoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_CheckSick(bpp, WAZASICK_NEMURI) )
    {
      u16 waza;
      u8 wazaMax, i, cnt;
      wazaMax = BPP_WAZA_GetCount( bpp );
      for(i=0, cnt=0; i<wazaMax; ++i)
      {
        waza = BPP_WAZA_GetID( bpp, i );
        if( isReqWazaExclude(waza) ){
          work[cnt++] = wazaMax;
        }
      }
      if( cnt )
      {
        u8 idx = GFL_STD_MtRand( cnt );
        BtlPokePos  pos= BTL_SVFLOW_ReqWazaTargetAuto( flowWk, pokeID, work[idx] );

        BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  work[idx] );
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
      }
    }
  }
}
static void handler_Negoto_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( waza == WAZANO_NULL ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * オウムがえし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_OumuGaesi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,          handler_OumuGaesi_CheckParam },  // 他ワザパラメータチェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_OumuGaesi_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlPokePos targetPos = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEPOS_ORG );
    u8* targetPokeIDAry = (u8*)work;
    u8  poke_cnt, i;

    // シングル戦の場合、相手選択の必要がないのでNULLが来る
    if( targetPos == BTL_POS_NULL ){
      BtlPokePos  myPos = BTL_SVFTOOL_GetExistFrontPokeID( flowWk, pokeID );
      BtlRule  rule = BTL_SVFLOW_GetRule( flowWk );
      targetPos = BTL_MAINUTIL_GetOpponentPokePos( rule, myPos, 0 );
    }


    if( BTL_SVFTOOL_ExpandPokeID(flowWk, targetPos, targetPokeIDAry) )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeIDAry[0] );
      WazaID  waza = BPP_GetPrevWazaID( bpp );
      {
        BTL_Printf( "オウム：狙った位置にポケモンいた。位置=%d, ID=%dです\n", targetPos, targetPokeIDAry[0]);
      }
      if( (waza == WAZANO_NULL)
      ||  (isReqWazaExclude(waza))
      ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
      }
      else{
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID, waza );
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, targetPos );
      }
    }
    else
    {
      BTL_Printf( "オウム：狙った位置にポケモンいない\n");
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * さきどり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Sakidori( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,        handler_Sakidori_CheckParam },  // 他ワザパラメータチェック
    { BTL_EVENT_WAZA_POWER,           handler_Sakidori_Power      },  // ワザ威力決定
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Sakidori_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlPokePos targetPos = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEPOS_ORG );
    u8* targetPokeIDAry = (u8*)work;
    u8  poke_cnt, i;

    // シングル戦の場合、相手選択の必要がないのでNULLが来る
    if( targetPos == BTL_POS_NULL ){
      BtlPokePos  myPos = BTL_SVFTOOL_GetExistFrontPokeID( flowWk, pokeID );
      BtlRule  rule = BTL_SVFLOW_GetRule( flowWk );
      targetPos = BTL_MAINUTIL_GetOpponentPokePos( rule, myPos, 0 );
    }

    if( BTL_SVFTOOL_ExpandPokeID(flowWk, targetPos, targetPokeIDAry) )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeIDAry[0] );
      BTL_ACTION_PARAM action;
      WazaID waza;
      u8 fSucceess = FALSE;

      do{

        // 相手が既に行動していたら失敗
        if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_WAZA_EXE) ){ break; }

        // 現ターンのアクションが取得できなかったら失敗
        if( !BTL_SVFLOW_GetThisTurnAction( flowWk, targetPokeIDAry[0], &action ) ){ break; }

        // 相手がダメージワザを選択していない場合も失敗
        waza = BTL_ACTION_GetWazaID( &action );
        if( (waza == WAZANO_NULL)
        ||  (!WAZADATA_IsDamage(waza))
        ){
          break;
        }
        // ここまで来たら成功
        fSucceess = TRUE;
      }while(0);

      if( fSucceess ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID, waza );
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, targetPos );
      }else{
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
      }
    }
  }
}
static void handler_Sakidori_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5) );
  }
}
//----------------------------------------------------------------------------------
/**
 * まねっこ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Manekko( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,         handler_Manekko_CheckParam },  // 他ワザパラメータチェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Manekko_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaID waza = BTL_SVFLOW_GetPrevExeWaza( flowWk );
    if( (waza != WAZANO_NULL)
    &&  (!isReqWazaExclude(waza))
    ){
      BtlPokePos  pos = BTL_SVFLOW_ReqWazaTargetAuto( flowWk, pokeID, waza );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID, waza );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
    }
  }
}





//----------------------------------------------------------------------------------
/**
 * ベノムショック
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_BenomShock( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_BenomShock },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_BenomShock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // どく状態の相手に威力２倍
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CheckSick(bpp, WAZASICK_DOKU) ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * たたりめ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tatarime( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Tatarime },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Tatarime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ポケ系状態状態の相手に威力２倍
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_GetPokeSick(bpp) != POKESICK_NULL ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * アクロバット
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Acrobat( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Acrobat },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Acrobat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 自分がどうぐを装備していない時に威力２倍
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetItem(bpp) == ITEM_DUMMY_DATA ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * アシストパワー
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_AsistPower( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_AsistPower },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_AsistPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 自分の能力変化上昇値合計*20を威力に加算
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32  upSum = 0;
    int  rank;
    int i;

    for(i=BPP_RANKVALUE_START; i<=BPP_RANKVALUE_END; ++i)
    {
      rank = BPP_GetValue( bpp, i );
      if( rank > 0 ){
        upSum += rank;
      }
    }

    if( upSum )
    {
      u32 power = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      power += (upSum * 20);
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, power );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ヘビーボンバー
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HeavyBomber( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_HeavyBomber },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_HeavyBomber( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    int weight_diff = BPP_GetWeight(bpp) - BPP_GetWeight(target);
    u32 pow;

    // 相手より重いほど威力が高い
    if( weight_diff >= 200 ){
      pow = 120;
    }else if( weight_diff >= 150 ){
      pow = 100;
    }else if( weight_diff >= 100 ){
      pow = 80;
    }else if( weight_diff >= 50 ){
      pow = 60;
    }else{
      pow = 40;
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * エレクトボール
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_ElectBall( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_ElectBall },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_ElectBall( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    int quot = BPP_GetValue(bpp, BPP_AGILITY) / BPP_GetValue(target, BPP_AGILITY);
    u32 pow;

    // 相手より素早いほど威力が高い
    if( quot >= 4 ){
      pow = 150;
    }else if( quot >= 3 ){
      pow = 120;
    }else if( quot >= 2 ){
      pow = 80;
    }else if( quot >= 1 ){
      pow = 60;
    }else{
      pow = 40;
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * エコーボイス
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_EchoVoice( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_EchoVoice },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_EchoVoice( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_WAZAREC* rec = BTL_SVF_GetWazaRecord( flowWk );
    WazaID  wazaID = BTL_EVENT_FACTOR_GetSubID( myHandle );
    int turn_cnt = BTL_SVFLOW_GetTurnCount( flowWk ) - 1;
    int cont_cnt = 0;

    while( turn_cnt >= 0 )
    {
      if( BTL_WAZAREC_IsUsedWaza(rec, wazaID, turn_cnt--) ){
        ++cont_cnt;
      }
    }

    {
      u32 pow;
      switch( cont_cnt ){
      case 0:  pow =  40; break;
      case 1:  pow =  80; break;
      case 2:  pow = 120; break;
      case 3:  pow = 160; break;
      default:  pow = 200; break;
      }
      BTL_Printf("エコボ %d 回目につき pow=%d\n", cont_cnt, pow);
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * いかさま
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ikasama( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER_PREV,   handler_Ikasama },    // ワザ威力チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ikasama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 相手の攻撃力で計算させる
    u8 def_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SWAP_POKEID, def_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * みずびたし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Mizubitasi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Mizubitasi },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Mizubitasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 targetCnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    BTL_HANDEX_PARAM_CHANGE_TYPE* param;
    u32 i;
    for(i=0; i<targetCnt; ++i)
    {
      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
      param->next_type = PokeTypePair_MakePure( POKETYPE_MIZU );
      param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1+i );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * シンプルビーム
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SimpleBeem( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_SimpleBeem   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SimpleBeem( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 targetCnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    BTL_HANDEX_PARAM_CHANGE_TOKUSEI*  param;
    u32 i;
    // 対象ポケモンのとくせいを「たんじゅん」に書き換え
    for(i=0; i<targetCnt; ++i)
    {
      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
      param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 + i );
      param->tokuseiID = POKETOKUSEI_TANJUN;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TokuseiChange );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );
      HANDEX_STR_AddArg( &param->exStr, param->tokuseiID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * なかまづくり
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_NakamaDukuri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_NakamaDukuri   },  // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_NakamaDukuri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 targetCnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    BTL_HANDEX_PARAM_CHANGE_TOKUSEI*  param;
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 myTokusei = BPP_GetValue( bpp, BPP_TOKUSEI );
    u32 i;
    // 対象ポケモンのとくせいを自分と同じものに書き換え
    for(i=0; i<targetCnt; ++i)
    {
      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
      param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 + i );
      param->tokuseiID = myTokusei;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TokuseiChange );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );
      HANDEX_STR_AddArg( &param->exStr, param->tokuseiID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * クリアスモッグ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_ClearSmog( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,  handler_ClearSmog   },         // ダメージ直後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_ClearSmog( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    reset_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    reset_param->poke_cnt = 1;
    reset_param->pokeID[0] = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RankReset );
    HANDEX_STR_AddArg( &msg_param->str, reset_param->pokeID[0] );
  }
}
//----------------------------------------------------------------------------------
/**
 * やきつくす
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yakitukusu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,  handler_Yakitukusu   },         // ダメージ直後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Yakitukusu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 defPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, defPokeID );
    u16 itemID = BPP_GetItem( target );
    BTL_Printf("やきつくすハンドラ：防御ポケID=%d, itemID=%d\n", defPokeID, itemID);
    if( ITEM_CheckNuts(itemID) )
    {
      BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );

      param->pokeID = defPokeID;
      param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Yakitukusu );
      HANDEX_STR_AddArg( &param->exStr, defPokeID );
      HANDEX_STR_AddArg( &param->exStr, itemID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * からをやぶる
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KarawoYaburu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_KarawoYaburu },   // ダメージ処理後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_KarawoYaburu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->rankType = WAZA_RANKEFF_DEFENCE;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_SP_DEFENCE;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_ATTACK;
    param->rankVolume = 2;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_SP_ATTACK;
    param->rankVolume = 2;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_AGILITY;
    param->rankVolume = 2;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;
  }
}
//----------------------------------------------------------------------------------
/**
 * ミラータイプ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MirrorType( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_MirrorType },   // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MirrorType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    param->pokeID = pokeID;
    param->next_type = BPP_GetPokeType( target );

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_p = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_p->str, BTL_STRTYPE_SET, BTL_STRID_SET_MirrorType );
      HANDEX_STR_AddArg( &msg_p->str, pokeID );
      HANDEX_STR_AddArg( &msg_p->str, targetPokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ボディパージ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_BodyPurge( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_RANKEFF_FIXED,  handler_BodyPurge },   // ワザによるランク増減効果確定後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_BodyPurge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 weight;

    weight = BPP_GetWeight( bpp );

    if( weight > BTL_POKE_WEIGHT_MIN )
    {
      BTL_HANDEX_PARAM_SET_WEIGHT* param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_WEIGHT, pokeID );

      if( weight > (BTL_POKE_WEIGHT_MIN + 100) ){
        weight -= 100;
      }else{
        weight = BTL_POKE_WEIGHT_MIN;
      }

      param->pokeID = pokeID;
      param->weight = weight;

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_BodyPurge );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * サイコショック
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PsycoShock( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD_PREV,  handler_PsycoShock },   // 防御側能力値計算
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_PsycoShock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_VID) == BPP_SP_DEFENCE )
    {
      int swap_cnt = BTL_EVENTVAR_GetValue( BTL_EVAR_VID_SWAP_CNT );
      ++swap_cnt;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_VID_SWAP_CNT, swap_cnt );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * なしくずし
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_NasiKuzusi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD_PREV,  handler_NasiKuzusi },   // 防御側能力値計算
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_NasiKuzusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
//----------------------------------------------------------------------------------
/**
 * ワンダールーム
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_WonderRoom( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_WonderRoom },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_WonderRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_WONDERROOM) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_WONDERROOM;
      param->cont = BPP_SICKCONT_MakeTurn( 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_WonderRoom_Start );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;
      BTL_HANDEX_PARAM_MESSAGE*  msg_param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_REMOVE_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_WONDERROOM;

      msg_param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_WonderRoom_End );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * マジックルーム
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MagicRoom( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_MagicRoom },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MagicRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_MAGICROOM) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_MAGICROOM;
      param->cont = BPP_SICKCONT_MakeTurn( 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_MagicRoom_Start );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_REMOVE_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_MAGICROOM;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * はじけるほのお
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HajikeruHonoo( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END, handler_HajikeruHonoo },    // 未分類ワザハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_HajikeruHonoo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokeID( flowWk, pokeID );
    BtlExPos   exPos = EXPOS_MAKE( BTL_EXPOS_MYSIDE_ALL, myPos );
    u8  targetPokeID[ BTL_POSIDX_MAX ];
    u8  targetCnt;
    u8  i;

    targetCnt = BTL_SVFTOOL_ExpandPokeID( flowWk, exPos, targetPokeID );
    for(i=0; i<targetCnt; ++i)
    {
      if( targetPokeID[i] != pokeID )
      {
        const BTL_POKEPARAM* bpp;
        BTL_HANDEX_PARAM_DAMAGE* param;

        bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID[i] );
        param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
        param->pokeID = targetPokeID[ i ];
        param->damage = BTL_CALC_QuotMaxHP( bpp, 16 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HajikeruHonoo_Side );
        HANDEX_STR_AddArg( &param->exStr, param->pokeID );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * シンクロノイズ
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SyncroNoise( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_SyncroNoise },    // ワザ無効チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SyncroNoise( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    const BTL_POKEPARAM* user = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    PokeTypePair myType, targetType;

    myType = BPP_GetPokeType( user );
    targetType = BPP_GetPokeType( target );

    if( !PokeTypePair_IsMatchEither(myType, targetType) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ギフトパス
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_GiftPass( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GiftPass },   // 未分類ワザ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_GiftPass( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    const BTL_POKEPARAM* user = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 myItemID = BPP_GetItem( user );
    if( (myItemID != ITEM_DUMMY_DATA)
    &&  (BPP_GetItem(target) == ITEM_DUMMY_DATA)
    ){
      BTL_HANDEX_PARAM_SET_ITEM* param;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      param->pokeID = pokeID;
      param->itemID = ITEM_DUMMY_DATA;

      param = BTL_SVF_HANEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      param->pokeID = targetPokeID;
      param->itemID = myItemID;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_GiftPass );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, myItemID );
    }
  }
}

