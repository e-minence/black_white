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

/* @todo �V���U���V���{�����ꎞ�I�Ɏ�ł����Ă��� */
enum {
  WAZANO_TUMETOGI = 468,
  WAZANO_WAIDOGAADO,
  WAZANO_GAADOSHEA,
  WAZANO_PAWAASHEA,
  WAZANO_WANDAARUUMU,
  WAZANO_SAIKOSYOKKU,
  WAZANO_BENOMUSHOKKU,
  WAZANO_BODYPAAZI,
  WAZANO_IKARINOKONA,
  WAZANO_TEREKINESISU,
  WAZANO_MAZIKKURUUMU,
  WAZANO_UTIOTOSU,
  WAZANO_GORINKUDAKI,
  WAZANO_HAZIKERUHONOO,
  WAZANO_HEDOROWHEEBU,
  WAZANO_TYOUNOMAI,
  WAZANO_HEBIIBONBAA,
  WAZANO_SINKURONOIZU,
  WAZANO_EREKUTOBOORU,
  WAZANO_MIZUBITASI,
  WAZANO_NITOROTYAAZI,
  WAZANO_TOGUROWOMAKU,
  WAZANO_ROOKIKKU,
  WAZANO_ASSIDOBOMU,
  WAZANO_IKASAMA,
  WAZANO_SINPURUBIIMU,
  WAZANO_NAKAMADUKURI,
  WAZANO_OSAKINIDOUZO,
  WAZANO_RINSYOU,
  WAZANO_EKOOBOISU,
  WAZANO_NASIKUZUSI,
  WAZANO_KURIASUMOGGU,
  WAZANO_ASISUTOPAWAA,
  WAZANO_FASTOGAADO,
  WAZANO_SAIDOCHENZI,
  WAZANO_NETTOU,
  WAZANO_KARAWOYABURU,
  WAZANO_IYASINOHADOU,
  WAZANO_TATARIME,
  WAZANO_FURIIFOORU,
  WAZANO_HURIIFOORU,
  WAZANO_GIACHENZI,
  WAZANO_TOMOENAGE,
  WAZANO_YAKITUKUSU,
  WAZANO_SAKIOKURI,
  WAZANO_AKUROBATTO,
  WAZANO_MIRAATAIPU,
  WAZANO_KATAKIUTI,
  WAZANO_INOTIGAKE,
  WAZANO_GIHUTOPASU,
  WAZANO_RENGOKU,
  WAZANO_MIZUNOTIKAI,
  WAZANO_HONOONOTIKAI,
  WAZANO_KUSANOTIKAI,
  WAZANO_BORUTOCHENZI,
  WAZANO_MUSINOTEIKOU,
  WAZANO_ZINARASI,
  WAZANO_KOORINOIBUKI,
  WAZANO_DORAGONTEERU,
  WAZANO_HURUITATERU,
  WAZANO_EREKINETTO,
  WAZANO_WAIRUDOBORUTO,
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
static BTL_EVENT_FACTOR*  ADD_Mizubitasi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Mizubitasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SimpleBeem( u16 pri, WazaID waza, u8 pokeID );
static void handler_SimpleBeem( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_NakamaDukuri( u16 pri, WazaID waza, u8 pokeID );
static void handler_NakamaDukuri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_ClearSmog( u16 pri, WazaID waza, u8 pokeID );
static void handler_ClearSmog( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_AsistPower( u16 pri, WazaID waza, u8 pokeID );
static void handler_AsistPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KarawoYaburu( u16 pri, WazaID waza, u8 pokeID );
static void handler_KarawoYaburu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Tatarime( u16 pri, WazaID waza, u8 pokeID );
static void handler_Tatarime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Acrobat( u16 pri, WazaID waza, u8 pokeID );
static void handler_Acrobat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );


//=============================================================================================
/**
 * �|�P�����́u���U�v�n���h�����V�X�e���ɒǉ�
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
    { WAZANO_NAITOHEDDO,      ADD_TikyuuNage    },  // �i�C�g�w�b�h=�����イ�Ȃ��Ɠ���
    { WAZANO_SAIKOWHEEBU,     ADD_PsycoWave     },
    { WAZANO_IBIKI,           ADD_Ibiki         },
    { WAZANO_TOTTEOKI,        ADD_Totteoki      },
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
    { WAZANO_NOROI,           ADD_Noroi         },
    { WAZANO_ABARERU,         ADD_Abareru       },
    { WAZANO_SAWAGU,          ADD_Sawagu        },
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
    { WAZANO_HUIUTI,          ADD_Fuiuti        },
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
    { WAZANO_YUKINADARE,      ADD_Revenge       },  // �䂫�Ȃ���=���x���W�Ɠ���
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
    { WAZANO_HOSIGARU,        ADD_Dorobou       },  // �ق�����=�ǂ�ڂ� �Ɠ���
    { WAZANO_TORIKKU,         ADD_Trick         },
    { WAZANO_SURIKAE,         ADD_Trick         },  // ���肩��=�g���b�N�Ɠ���
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
    { WAZANO_KUSAMUSUBI,      ADD_Ketaguri      },  // �����ނ���=��������Ɠ���
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
    { WAZANO_MUSIKUI,         ADD_Tuibamu       },  // �ނ�����=���΂� �Ɠ���
    { WAZANO_NAGETUKERU,      ADD_Nagetukeru    },
    { WAZANO_MAKITUKU,        ADD_Makituku      },
    { WAZANO_SIMETUKERU,      ADD_Makituku      },
    { WAZANO_HONOONOUZU,      ADD_Makituku      },
    { WAZANO_KARADEHASAMU,    ADD_Makituku      },
    { WAZANO_SUNAZIGOKU,      ADD_Makituku      },
    { WAZANO_MAGUMASUTOOMU,   ADD_Makituku      },
    { WAZANO_UZUSIO,          ADD_Uzusio        },
    { WAZANO_HUKITOBASI,      ADD_KousokuSpin   },

    // �ȉ��A�V���U
    { WAZANO_BENOMUSHOKKU,    ADD_BenomShock    },
    { WAZANO_IKARINOKONA,     ADD_KonoyubiTomare},  // ������̂���=���̂�тƂ܂�
    { WAZANO_MIZUBITASI,      ADD_Mizubitasi    },
    { WAZANO_SINPURUBIIMU,    ADD_SimpleBeem    },
    { WAZANO_NAKAMADUKURI,    ADD_NakamaDukuri  },
    { WAZANO_KURIASUMOGGU,    ADD_ClearSmog     },
    { WAZANO_ASISUTOPAWAA,    ADD_AsistPower    },
    { WAZANO_KARAWOYABURU,    ADD_KarawoYaburu  },
    { WAZANO_TATARIME,        ADD_Tatarime      },
    { WAZANO_AKUROBATTO,      ADD_Acrobat       },
    { WAZANO_BORUTOCHENZI,    ADD_TonboGaeri    },  // �{���g�`�F���W=�Ƃ�ڂ�����
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
        BTL_Printf("���U�n���h��[%d]�o�^\n", waza);
        if( funcTbl[i].func( agi, waza, pokeID ) ){
          return TRUE;
        }
      }
      else if( fRegistered ){
        BTL_Printf("���U�n���h��[%d]�͂��łɓo�^�ς�\n", waza);
        return TRUE;
      }
      return FALSE;
    }
  }

  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * �o�^�ł�������𖞂����Ă��邩����
 * �� �����|�P���������U�̃n���h���͂P�܂�
 * �� �|�P�����P�̂ɂ� EVENT_WAZA_STICK_MAX �܂� �o�^�ł���
 *
 * @param   pokeID
 * @param   waza
 *
 * @retval  BOOL    �����𖞂����Ă�����TRUE
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
      BTL_Printf("���ɓ������U���o�^����Ă���");
      *fRegistered = TRUE;
      return FALSE;
    }
    if( ++cnt > EVENT_WAZA_STICK_MAX ){
      BTL_Printf("�����|�P����[%d]�̓\��t�����U��%d���z����\n", pokeID, EVENT_WAZA_STICK_MAX);
      return FALSE;
    }
    factor = BTL_EVENT_GetNextFactor( factor );
  }
  return TRUE;
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
  u8 pokeID = BPP_GetID( pp );

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
 * ����|�P�����̃��U�n���h�����V�X�e������S�ċ����폜�i�\��t���Ă�����̂������I�ɍ폜�j
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
 * �e�N�X�`���[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Texture( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Texture },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Texture( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
      BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
      param->next_type = next_type;
      param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ق����傭
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Hogosyoku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Hogosyoku },    // �����ރ��U�n���h��
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
    case BTL_LANDFORM_GRASS:   type = POKETYPE_KUSA;   break;  ///< ���ނ�
    case BTL_LANDFORM_SAND:    type = POKETYPE_JIMEN;  break;  ///< ���n
    case BTL_LANDFORM_SEA:     type = POKETYPE_MIZU;   break;  ///< �C
    case BTL_LANDFORM_RIVER:   type = POKETYPE_MIZU;   break;  ///< ��
    case BTL_LANDFORM_SNOW:    type = POKETYPE_KOORI;  break;  ///< �ጴ
    case BTL_LANDFORM_CAVE:    type = POKETYPE_IWA;    break;  ///< ���A
    case BTL_LANDFORM_ROCK:    type = POKETYPE_IWA;    break;  ///< ���
    case BTL_LANDFORM_ROOM:    type = POKETYPE_NORMAL; break;  ///< ����
    }

    {
      PokeTypePair  pairType = PokeTypePair_MakePure( type );
      const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

      if( BPP_GetPokeType(bpp) != pairType )
      {
        BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
        param->next_type = pairType;
        param->pokeID = pokeID;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �g���b�N���[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TrickRoom( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET, handler_TrickRoom },    // �����ރ��U�n���h��
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

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_TRICKROOM;
      param->cont = BPP_SICKCONT_MakeTurn( 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TrickRoom );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;
      BTL_HANDEX_PARAM_MESSAGE*  msg_param;

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_REMOVE_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_TRICKROOM;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TrickRoomOff );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );

    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���イ��傭
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Juryoku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,     handler_Juryoku },          // �����ރ��U�n���h��
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

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
    param->effect = BTL_FLDEFF_JURYOKU;
    param->cont = BPP_SICKCONT_MakeTurn( 5 );

    param->exStr.type = BTL_STRTYPE_STD;
    param->exStr.ID = BTL_STRID_STD_Jyuryoku;
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂�������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MizuAsobi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET, handler_MizuAsobi },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MizuAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
    param->effect = BTL_FLDEFF_MIZUASOBI;
    param->cont = BPP_SICKCONT_MakePoke( pokeID );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Mizuasobi );
  }
}
//----------------------------------------------------------------------------------
/**
 * �ǂ날����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_DoroAsobi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET, handler_DoroAsobi },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_DoroAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
    param->effect = BTL_FLDEFF_MIZUASOBI;
    param->cont = BPP_SICKCONT_MakePoke( pokeID );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Doroasobi );
  }
}
//----------------------------------------------------------------------------------
/**
 * ����΂炢
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kiribarai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Kiribarai },    // �����ރ��U�n���h��
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

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->rankType = WAZA_RANKEFF_AVOID;
    rank_param->rankVolume = -1;
    rank_param->fAlmost = TRUE;
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = targetPokeID;

    remove_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
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
 * �������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kawarawari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_DETERMINE, handler_Kawarawari },    // �����ރ��U�n���h��
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

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
      param->side = side;
      BTL_CALC_BITFLG_Construction( param->flags, sizeof(param->flags) );
      BTL_CALC_BITFLG_Set( param->flags, BTL_SIDEEFF_REFRECTOR );
      BTL_CALC_BITFLG_Set( param->flags, BTL_SIDEEFF_HIKARINOKABE );

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Kawarawari_Break );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ƃт���E�ƂтЂ�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tobigeri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_AVOID, handler_Tobigeri },    // ���U�͂��ꂽ
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
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );
    u32 maxHP = BPP_GetValue( target, BPP_MAX_HP );

    damage /= 2;
    maxHP /= 2;
    if( damage > maxHP ){
      damage = maxHP;
    }

    {
      BTL_HANDEX_PARAM_DAMAGE* param;

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->damage[0] = damage;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HazureJibaku );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���̂܂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Monomane( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Monomane },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Monomane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    WazaID waza = BPP_GetPrevWazaID( target );
    if( waza != WAZANO_NULL)
    {
      u8 wazaIdx = BPP_WAZA_SearchIdx( self, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        BTL_HANDEX_PARAM_UPDATE_WAZA*  param;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_UPDATE_WAZA, pokeID );
        param->pokeID = pokeID;
        param->wazaIdx = wazaIdx;
        param->waza = waza;
        param->ppMax = 5;
        param->fPermanent = FALSE;

        msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Monomane );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
        HANDEX_STR_AddArg( &msg_param->str, waza );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �X�P�b�`
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Sketch( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Sketch },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Sketch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    WazaID waza = BPP_GetPrevWazaID( target );
    if( waza != WAZANO_NULL)
    {
      u8 wazaIdx = BPP_WAZA_SearchIdx( self, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        BTL_HANDEX_PARAM_UPDATE_WAZA*  param;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_UPDATE_WAZA, pokeID );
        param->pokeID = pokeID;
        param->wazaIdx = wazaIdx;
        param->waza = waza;
        param->ppMax = 0;
        param->fPermanent = TRUE;

        msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Monomane );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
        HANDEX_STR_AddArg( &msg_param->str, waza );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ���̂�тƂ܂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KonoyubiTomare( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_KonoyubiTomare_ExeCheck },    // ���U�o�����ۃ`�F�b�N
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_KonoyubiTomare_Exe   },     // �����ރ��U���s
    { BTL_EVENT_DECIDE_TARGET,      handler_KonoyubiTomare_Target },      // �^�[�Q�b�g����
    { BTL_EVENT_TAME_START,         handler_KonoyubiTomare_TurnCheck },   // �^�[���`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
// ���U�o�����ۃ`�F�b�N�F�V���O���Ȃ玸�s����
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
// ���U�o�����ۊm��F���b�Z�[�W�\��
static void handler_KonoyubiTomare_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
 * ���t���b�V��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Refresh( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Refresh },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Refresh( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    PokeSick sick = BPP_GetPokeSick( bpp );

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
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_KumoNoSu },    // �����ރ��U�n���h��
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

    if( !BPP_CheckSick(bpp, WAZASICK_TOOSENBOU) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
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
 * ���낢����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KuroiKiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_KuroiKiri },    // �����ރ��U�n���h��
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
    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    {
      BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
      BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_ALL, myPos );

      reset_param->poke_cnt = BTL_SERVERFLOW_RECEPT_GetTargetPokeID( flowWk, expos, reset_param->pokeID );
    }

    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_RankReset );
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
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Haneru },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Haneru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_NotHappen );
  }
}
//----------------------------------------------------------------------------------
/**
 * �̂낢
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Noroi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Noroi },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Noroi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

    if( BPP_IsMatchType(bpp, POKETYPE_GHOST) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param;
      u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
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
      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_AGILITY;
      param->rankVolume = -1;

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_ATTACK;
      param->rankVolume = 1;

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_DEFENCE;
      param->rankVolume = 1;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ȃ�݂̃^�l
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_NayamiNoTane( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_NayamiNoTane_NoEff },    // �������`�F�b�N���x���Q�n���h��
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_NayamiNoTane       },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_NayamiNoTane_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    // �Ȃ܂��C�ӂ݂񎝂��ɂ͌����Ȃ�
    if( (BPP_GetValue(target, BPP_TOKUSEI) == POKETOKUSEI_NAMAKE)
    ||  (BPP_GetValue(target, BPP_TOKUSEI) == POKETOKUSEI_FUMIN)
    ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
static void handler_NayamiNoTane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �Ώۂ̂Ƃ��������u�ӂ݂�v�ɏ�������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );

    param->pokeID = targetPokeID;
    param->tokuseiID = POKETOKUSEI_FUMIN;
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

    if( !BPP_CheckSick(target, POKESICK_NEMURI) )
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
    cont = BTL_CALC_MakeDefaultPokeSickCont( sick );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 20 );
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ђ݂̂�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HimituNoTikara( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_HimituNoTikara_Sick },    // �ǉ����ʂɂ���Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_HimituNoTikara_Sick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BtlLandForm  land = BTL_SVFLOW_GetLandForm( flowWk );
    WazaSick  sick;
    BPP_SICK_CONT  cont;
    WazaID  waza = BTL_EVENT_FACTOR_GetSubID( myHandle );

    // @@@ �{���͒n�`�ɉ����Ă����ƃt�N�U�c��
    if( land == BTL_LANDFORM_SNOW ){
      sick = WAZASICK_KOORI;
    }else{
      sick = WAZASICK_MAHI;
    }

    BTL_CALC_MakeDefaultWazaSickCont( sick, bpp, &cont );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, cont.raw );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, WAZADATA_GetSickPer(waza) );
  }
}
//----------------------------------------------------------------------------------
/**
 * ������ׂ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Osyaberi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_Osyaberi },    // �ǉ����ʂɂ���Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Osyaberi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
 * ���߂���E�܂����E�ق̂��̂������X
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Makituku( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_Makituku },    // �ǉ����ʂɂ���Ԉُ�`�F�b�N�n���h��
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
    BTL_Printf("  �����t���n : poketurn, pokeID=%d, turn=%d\n", cont.poketurn.pokeID, cont.poketurn.count);
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Uzusio( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_Makituku },    // �ǉ����ʂɂ���Ԉُ�`�F�b�N�n���h�� = �܂����ƈꏏ
    { BTL_EVENT_WAZA_DMG_PROC2, handler_Uzusio_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Uzusio_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_DIVING) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
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

    u16 damage = BPP_GetValue(target, BPP_HP) / 2;

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
    diff = BPP_GetValue(defender, BPP_HP) - BPP_GetValue(attacker, BPP_HP);
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
    u8 level = BPP_GetValue( bpp, BPP_LEVEL );
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
 * �����킦��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Takuwaeru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Takuwaeru_CheckExe   },    // ���U�o����������
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Takuwaeru            },    // �����ރ��U����
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
// ���ۃ`�F�b�N�i�J�E���^���R���܂��Ă����玸�s����j
static void handler_Takuwaeru_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU) >= 3 ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
static void handler_Takuwaeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 takuwae_cnt = BPP_COUNTER_Get( bpp, BPP_COUNTER_TAKUWAERU );
    if( takuwae_cnt < 3 )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;
      BTL_HANDEX_PARAM_COUNTER*     counter_param;
      BTL_HANDEX_PARAM_MESSAGE*     msg_param;

      counter_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
      counter_param->pokeID = pokeID;
      counter_param->counterID = BPP_COUNTER_TAKUWAERU;
      counter_param->value = takuwae_cnt + 1;
      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Takuwaeru );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, takuwae_cnt+1 );


      rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->poke_cnt = 1;
      rank_param->pokeID[0] = pokeID;
      rank_param->rankType = BPP_DEFENCE;
      rank_param->rankVolume = 1;
      counter_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
      counter_param->header.failSkipFlag = TRUE;
      counter_param->pokeID = pokeID;
      counter_param->counterID = BPP_COUNTER_TAKUWAERU_DEF;
      counter_param->value = BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU_DEF) + 1;


      rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->poke_cnt = 1;
      rank_param->pokeID[0] = pokeID;
      rank_param->rankType = BPP_SP_DEFENCE;
      rank_param->rankVolume = 1;
      counter_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
      counter_param->header.failSkipFlag = TRUE;
      counter_param->pokeID = pokeID;
      counter_param->counterID = BPP_COUNTER_TAKUWAERU_SPDEF;
      counter_param->value = BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU_SPDEF) + 1;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �͂�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Hakidasu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Hakidasu_CheckExe   },    // ���U�o����������
    { BTL_EVENT_WAZA_POWER,          handler_Hakidasu_Pow        },    // ���U�З͌���
    { BTL_EVENT_WAZA_EXECUTE_DONE,   handler_Hakidasu_Done       },    // ���U�o���I��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
// ���ۃ`�F�b�N�i�J�E���^���O�������玸�s����j
static void handler_Hakidasu_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU) == 0 ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
static void handler_Hakidasu_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 pow = BPP_COUNTER_Get( bpp, BPP_COUNTER_TAKUWAERU ) * 100;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
static void handler_Hakidasu_Done( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID     = pokeID;
    rank_param->attack     = BPP_GetValue( bpp, BPP_ATTACK_RANK );
    rank_param->defence    = def_rank;
    rank_param->sp_attack  = BPP_GetValue( bpp, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence = spdef_rank;
    rank_param->agility    = BPP_GetValue( bpp, BPP_AGILITY_RANK );

    counter_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
    counter_param->pokeID = pokeID;
    counter_param->counterID = BPP_COUNTER_TAKUWAERU;
    counter_param->value = 0;

    counter_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
    counter_param->pokeID = pokeID;
    counter_param->counterID = BPP_COUNTER_TAKUWAERU_DEF;
    counter_param->value = 0;

    counter_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
    counter_param->pokeID = pokeID;
    counter_param->counterID = BPP_COUNTER_TAKUWAERU_SPDEF;
    counter_param->value = 0;

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TakuwaeOff );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂̂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nomikomu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Hakidasu_CheckExe   },    // ���U�o����������
    { BTL_EVENT_RECOVER_HP_RATIO,    handler_Nomikomu_Ratio      },    // HP�񕜗�����
    { BTL_EVENT_WAZA_EXECUTE_DONE,   handler_Hakidasu_Done       },    // ���U�o���I��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Nomikomu_Ratio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
 * �J�E���^�[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Counter( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Counter_CheckExe   },    // ���U�o����������
    { BTL_EVENT_DECIDE_TARGET,       handler_Counter_Target     },    // �^�[�Q�b�g����
    { BTL_EVENT_WAZA_DMG_PROC1,      handler_Counter_CalcDamage },    // �_���[�W�v�Z�ŏI�n���h��

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
 * �~���[�R�[�g
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MilerCoat( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_MilerCoat_CheckExe   },    // ���U�o����������
    { BTL_EVENT_DECIDE_TARGET,       handler_MilerCoat_Target     },    // �^�[�Q�b�g����
    { BTL_EVENT_WAZA_DMG_PROC1,      handler_MilerCoat_CalcDamage },    // �_���[�W�v�Z�ŏI�n���h��

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
 * ���^���o�[�X�g
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MetalBurst( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_MetalBurst_CheckExe   },    // ���U�o����������
    { BTL_EVENT_DECIDE_TARGET,       handler_MetalBurst_Target     },    // �^�[�Q�b�g����
    { BTL_EVENT_WAZA_DMG_PROC1,      handler_MetalBurst_CalcDamage },    // �_���[�W�v�Z�ŏI�n���h��

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
// �J�E���^�[�n���ʁF���U�����`�F�b�N
//
//----------------------------------------------------------------------------------
static void common_Counter_ExeCheck( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BPP_WAZADMG_REC  rec;

    if( !common_Counter_GetRec(bpp, dmgType, &rec) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
//
// �J�E���^�[�n���ʁF�^�[�Q�b�g����
//
//----------------------------------------------------------------------------------
static void common_Counter_SetTarget( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BPP_WAZADMG_REC  rec;

    if( common_Counter_GetRec(bpp, dmgType, &rec) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, rec.pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
//
// �J�E���^�[�n���ʁF�_���[�W�v�Z
//
//----------------------------------------------------------------------------------
static void common_Counter_CalcDamage( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BPP_WAZADMG_REC  rec;

    if( common_Counter_GetRec(bpp, dmgType, &rec) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, rec.damage*2 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �J�E���^�[�n���ʉ������F�ΏۂƂȂ�_���[�W���R�[�h���擾
 *
 * @param   bpp
 * @param   dmgType   �Ώۃ_���[�W�^�C�v�i����or����FWAZADATA_DMG_NONE�Ȃ痼�Ή��j
 * @param   rec       [out]�擾��\���̃A�h���X
 *
 * @retval  BOOL      �擾�ł�����TRUE
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
 * �Ƃ��Ă���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Totteoki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_Totteoki },    // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Totteoki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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

    if( !BPP_CheckSick(bpp, POKESICK_NEMURI) ){
      BTL_Printf("����[%d]���Q�ĂȂ����玸�s\n", targetPokeID);
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂ�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Fuiuti( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Fuiuti_NoEff },    // ���U�����`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Fuiuti_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );
    BTL_ACTION_PARAM  action;
    WazaID waza;
    BOOL fSuccess = FALSE;

    do {
      // ���肪���ɍs�����Ă����玸�s
      if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_WAZA_EXE) ){ break; }

      // ���^�[���̃A�N�V�������擾�ł��Ȃ������玸�s
      if( !BTL_SVFLOW_GetThisTurnAction( flowWk, targetPokeID, &action ) ){ break; }

      // ���肪�_���[�W���U��I�����Ă��Ȃ��ꍇ�����s
      waza = BTL_ACTION_GetWazaID( &action );
      if( (waza == WAZANO_NULL)
      ||  (!WAZADATA_IsDamage(waza))
      ){
        break;
      }
      // �����܂ŗ����琬��
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
 * ��������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Oiuti( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MENBERCHANGE_INTR, handler_Oiuti_Intr },    // ����ւ����荞�݃n���h��
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Oiuti_Dmg  },    // �_���[�W�v�Z�ŏI�`�F�b�N
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
    BTL_Printf("���V[%d]��������_���|�P=%d, ����ւ�낤�Ƃ��Ă�|�P=%d�i�ʒu:%d)\n",
      pokeID, targetPokeID, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1), action.fight.targetPos);
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) == targetPokeID )
    {
      BTL_Printf("���荞�݂܂�\n");
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
        BTL_Printf("����ւ�낤�Ƃ��Ă郄�c�ɂ͂Q�{�ł�\n");
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �����΂��͂E���΂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Daibakuhatsu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD,   handler_Daibakuhatsu },         // �h��̓`�F�b�N�n���h��
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Daibakuhatsu_ExeFix },  // �킴�o���m��n���h��
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
    BTL_HANDEX_PARAM_KILL* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    param->pokeID = pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * ���イ�ł�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Juden( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Juden_Exe },    // ���U�o���m��n���h��
    { BTL_EVENT_WAZA_POWER,       handler_Juden_Pow },    // ���U�З͌v�Z�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Juden_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) == BTL_EVENT_FACTOR_GetSubID(myHandle) ){
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
 * �ق�т̂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HorobiNoUta( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_HorobiNoUta_Exe },    // ���U�o���m��n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_HorobiNoUta_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_HorobiNoUta );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��ǂ肬�̂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_YadorigiNoTane( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM, handler_YadorigiNoTane_Param },    // ���U�o���m��n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_YadorigiNoTane_Param( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BPP_SICK_CONT cont;
    BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
    cont = BPP_SICKCONT_MakePermanentParam( myPos );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}
//----------------------------------------------------------------------------------
/**
 * �l�R�ɂ��΂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_NekoNiKoban( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER, handler_NekoNiKoban },    // �_���[�W�����n���h��
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
      const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

      u32 volume = BPP_GetValue( bpp, BPP_LEVEL ) * 20;
      BTL_SVFLOW_AddBonusMoney( flowWk, volume );

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_NekoNiKoban );
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
    if( !BPP_IsDead(bpp) )
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
 * �A�N�A�����O
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_AquaRing( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_AquaRing },            // ���U�o���m��n���h��
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_AquaRing_turnCheck },  // �^�[���`�F�b�N�J�n�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_AquaRing( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_AquaRing );
    HANDEX_STR_AddArg( &param->str, pokeID );
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_AquaRing_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] != 0)
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( !BPP_IsHPFull(bpp)
    &&  !BPP_IsDead(bpp)
    ){
      BTL_HANDEX_PARAM_RECOVER_HP* param;
      const BTL_POKEPARAM* bpp;

      bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = pokeID;
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 16 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_AquaRingRecover );
    }
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
    if( (!BPP_CheckSick(bpp, WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      u8 turns;

      turns = 3 + GFL_STD_MtRand(2);

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( !BPP_CheckSick(bpp, WAZASICK_WAZALOCK)
    &&  !BPP_CheckSick(bpp, WAZASICK_KONRAN)
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
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
 * ���킮
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Sawagu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Sawagu },            // ���U�o���m��n���h��
    { BTL_EVENT_TURNCHECK_END,     handler_Sawagu_turnCheck },  // �^�[���`�F�b�N�I���n���h��
    { BTL_EVENT_ADDSICK_CHECKFAIL, handler_Sawagu_CheckSickFail },  // ��Ԉُ편�s�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Sawagu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���������U���b�N��ԂɂȂ��Ă��Ȃ��Ȃ�A���U���b�N��Ԃɂ���
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( (!BPP_CheckSick(bpp, WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      u8 turns;

      turns = BTL_CALC_RandRange( 2, 5 );
      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK;
      param->sickCont = BPP_SICKCONT_MakeTurn( turns );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Sawagu );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );

      {
        BTL_HANDEX_PARAM_CURE_SICK* cure_param;
        BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
        BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_MYSIDE_ALL, myPos );

        cure_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        cure_param->sickCode = WAZASICK_NEMURI;
        cure_param->poke_cnt = BTL_SERVERFLOW_RECEPT_GetTargetPokeID( flowWk, expos, cure_param->pokeID );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( !BPP_CheckSick(bpp, WAZASICK_WAZALOCK)
    &&  !BPP_IsDead(bpp)
    ){
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
  enum {
    KOROGARU_TURNS = 5,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���������U���b�N��ԂɂȂ��Ă��Ȃ��Ȃ�A���U���b�N��Ԃɂ���
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( (!BPP_CheckSick(bpp, WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK;
      param->sickCont = BPP_SICKCONT_MakeTurn( KOROGARU_TURNS );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      work[0] = 0;
      work[ WORKIDX_STICK ] = 1;
      BTL_Printf("�|�P(%d)�����낪��B���U�n���h���\��t��\n", pokeID);
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
    param->sickCode = WAZASICK_WAZALOCK;

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
 * �g���v���L�b�N
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TripleKick( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_TripleKick },    // ���U�З̓`�F�b�N�n���h��
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
 * �W���C���{�[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_GyroBall( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_GyroBall },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_GyroBall( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

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
 * ���x���W
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Revenge( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Revenge },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Revenge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BPP_WAZADMG_REC  rec;
    u8  idx = 0;

    // �_�����肩�猻�^�[���Ƀ_���[�W�󂯂Ă���З͂Q�{
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
    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_WAZA_EXE) )
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
    fx32 r = BPP_GetHPRatio( bpp );
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
    fx32 r = BPP_GetHPRatio( bpp );
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
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
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
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
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
    { BTL_EVENT_DAMAGEPROC_END, handler_MezamasiBinta_AfterDamage },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MezamasiBinta( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
    if( BPP_CheckSick(bpp, WAZASICK_NEMURI) ){
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
    { BTL_EVENT_DAMAGEPROC_END, handler_Kituke_AfterDamage },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �}�q��Ԃ̑���ɈЗ͂Q�{
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
    if( BPP_CheckSick(bpp, WAZASICK_MAHI) ){
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

    if( !BPP_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP*  param = BTL_SVFLOW_HANDLERWORK_Push( flowWk ,BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = target_pokeID;
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 4 );

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    }
    else
    {
      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
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
 * ��񂼂�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_RenzokuGiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_RenzokuGiri },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_RenzokuGiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
 * �_������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Dameosi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Dameosi },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Dameosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_DAMAGED) ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ��������E�����ނ���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ketaguri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Ketaguri },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ketaguri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    u32 heavy = BPP_GetValue( bpp, BPP_HEAVY );
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
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_SORAWOTOBU) )
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
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Kaminari_checkHide },      // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_EXCUSE_CALC_HIT,   handler_Kaminari_excuseHitCalc },  // �q�b�g�m���v�Z�X�L�b�v
    { BTL_EVENT_WAZA_HIT_RATIO,    handler_Kaminari_hitRatio },       // �������v�Z
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
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_ANAWOHORU) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���΂��̂Ԃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SabakiNoTubute( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_SabakiNoTubute },  // �킴�p�����[�^�`�F�b�N
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
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
 * ������̂߂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SizenNoMegumi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_SizenNoMegumi_ExeCheck },    // �킴�o�����ۃ`�F�b�N
    { BTL_EVENT_DAMAGEPROC_END,        handler_SizenNoMegumi_AfterDamage }, // �킴�o���m��
    { BTL_EVENT_WAZA_PARAM,   handler_SizenNoMegumi_Type },               // �킴�p�����[�^�`�F�b�N
    { BTL_EVENT_WAZA_POWER,   handler_SizenNoMegumi_Pow  },               // �킴�З̓`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_SizenNoMegumi_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u16 item = BPP_GetItem( bpp );
    s32 type = BTL_CALC_ITEM_GetParam( item, ITEM_PRM_SIZENNOMEGUMI_TYPE );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}
static void handler_SizenNoMegumi_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u16 item = BPP_GetItem( bpp );
    s32 pow = BTL_CALC_ITEM_GetParam( item, ITEM_PRM_SIZENNOMEGUMI_ATC );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
static void handler_SizenNoMegumi_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
    param->pokeID = pokeID;
    param->itemID = ITEM_DUMMY_DATA;
  }
}
//----------------------------------------------------------------------------------
/**
 * �͂������Ƃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Hatakiotosu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END,      handler_Hatakiotosu }, // �_���[�W��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Hatakiotosu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
    u16 itemID = BPP_GetItem( bpp );
    if( itemID != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
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
 * �}�W�b�N�R�[�g
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MagicCoat( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_MagicCoat },           // �����ރ��U����
    { BTL_EVENT_CHECK_WAZA_ROB,               handler_MagicCoat_Rob },       // ���U������
    { BTL_EVENT_TURNCHECK_BEGIN,              handler_MagicCoat_TurnCheck }, // �^�[���`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MagicCoat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
    WazaCategory category = WAZADATA_GetCategory( waza );
    if( category == WAZADATA_CATEGORY_SIMPLE_SICK )
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
  // �^�[���`�F�b�N�Ŏ��E
  BTL_EVENT_FACTOR_Remove( myHandle );
}
//----------------------------------------------------------------------------------
/**
 * �悱�ǂ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yokodori( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Yokodori },        // �����ރ��U����
    { BTL_EVENT_CHECK_WAZA_ROB,               handler_Yokodori_Rob },    // ����蔻��
    { BTL_EVENT_TURNCHECK_BEGIN,              handler_Yokodori_TurnCheck }, // �^�[���`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Yokodori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
    WazaCategory category = WAZADATA_GetCategory( waza );
    BOOL fEffective = FALSE;

    if( category == WAZADATA_CATEGORY_SIMPLE_EFFECT ){
      int volume;
      WAZADATA_GetRankEffect( waza, 0, &volume );
      if( volume > 0 ){
        fEffective = TRUE;
      }
    }else if( category == WAZADATA_CATEGORY_SIMPLE_RECOVER ){
      fEffective = TRUE;
    }

    if( fEffective )
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
static void handler_Yokodori_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_EVENT_FACTOR_Remove( myHandle );
}
//----------------------------------------------------------------------------------
/**
 * �ǂ�ڂ��E�ق�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Dorobou( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END,      handler_Dorobou }, // �_���[�W��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Dorobou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BPP_GetItem(self) == ITEM_DUMMY_DATA )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
      if( BPP_GetItem(target) != ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_SWAP_ITEM* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SWAP_ITEM, pokeID );
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
 * �g���b�N�E���肩��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Trick( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END,      handler_Trick }, // �_���[�W��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Trick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BPP_GetItem(self) != ITEM_DUMMY_DATA )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
      if( BPP_GetItem(target) != ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_SWAP_ITEM* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SWAP_ITEM, pokeID );
        param->pokeID = target_pokeID;
        param->fSucceedMsg = TRUE;
        param->succeedStrID = BTL_STRID_SET_Trick;
      }
    }
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
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Magnitude1 + work[0] );
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
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_DIVING) )
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
    if( BPP_CONTFLAG_Get(target, BPP_CONTFLG_TIISAKUNARU) )
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
    u8 counter = BPP_GetWazaContCounter( bpp );

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
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Mamoru },          // �����ރ��U�n���h��
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
    u8 counter = BPP_GetWazaContCounter( bpp );

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
 * ���܂�
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
      // �P�^�[���ځF���������U���b�N��Ԃɂ��āA�����𔲂���B
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
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
      // �Q�^�[���ځF�������������𔲂���
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
        work[0] = GAMAN_STATE_3RD;
      }
      break;
    case GAMAN_STATE_3RD:
      // �R�^�[���ځF�\��t�������U���b�N��ԉ���
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 dmg_sum = gaman_getTotalDamage( bpp );
    if( dmg_sum ){
      dmg_sum *= 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, dmg_sum );
      BTL_Printf("���܂�R�^�[���ځF�_���[�W�f���o��=%d\n", dmg_sum);
    }
  }
}
static void handler_Gamen_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] == GAMAN_STATE_3RD)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 dmg_sum = gaman_getTotalDamage( bpp );
    if( dmg_sum == 0 ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
      BTL_Printf("���܂�R�^�[���ځF�_���[�W�󂯂ĂȂ��̂Ŏ��s\n");
    }
  }
}
static void handler_Gaman_Fail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ���s�����炷���ɓ\��t�������U���b�N��ԉ���
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    gaman_ReleaseStick( flowWk, pokeID, work );
  }
}
static void gaman_ReleaseStick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] ){
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
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
 * ���T�C�N��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Recycle( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Recycle },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Recycle( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u16 itemID = BPP_GetConsumedItem( bpp );
    if( itemID != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );

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
 * �T�C�R�V�t�g
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PsycoShift( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_PsycoShift },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_PsycoShift( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    WazaSick sick = BPP_GetPokeSick( bpp );
    // �������|�P�����n�̏�Ԉُ�ŁA���肪�����łȂ��ꍇ�̂݌��ʂ�����
    if( sick != WAZASICK_NULL )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
      if( BPP_GetPokeSick(target) == WAZASICK_NULL )
      {
        BTL_HANDEX_PARAM_ADD_SICK    *sick_param;
        BTL_HANDEX_PARAM_CURE_SICK   *cure_param;

        sick_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        sick_param->sickID = sick;
        sick_param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( sick );
        sick_param->poke_cnt = 1;
        sick_param->pokeID[0] = target_pokeID;
        sick_param->fAlmost = TRUE;

        cure_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        cure_param->sickCode = sick;
        cure_param->poke_cnt = 1;
        cure_param->pokeID[0] = pokeID;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �����݂킯
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Itamiwake( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Itamiwake },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Itamiwake( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp_me = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* bpp_target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );

    // ���҂̕��ϒl��HP�����킹��
    u32 hp_me     = BPP_GetValue( bpp_me, BPP_HP );
    u32 hp_target = BPP_GetValue( bpp_target, BPP_HP );
    u32 hp_avrg = (hp_me + hp_target) / 2;

    BTL_HANDEX_PARAM_SHIFT_HP* param;
    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
    param->poke_cnt = 2;
    param->pokeID[0] = pokeID;
    param->volume[0] = hp_avrg - hp_me;
    param->pokeID[1] = target_pokeID;
    param->pokeID[1] = hp_avrg - hp_target;

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Itamiwake );
    }
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Haradaiko },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Haradaiko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 downHP = BPP_GetValue( bpp, BPP_MAX_HP ) / 2;
    u32 upVolume = BPP_RankEffectUpLimit( bpp, BPP_ATTACK );
    if( (BPP_GetValue(bpp, BPP_HP) > downHP) && (upVolume != 0) )
    {
      BTL_HANDEX_PARAM_SHIFT_HP     *hp_param;
      BTL_HANDEX_PARAM_RANK_EFFECT  *rank_param;
      BTL_HANDEX_PARAM_MESSAGE      *msg_param;

      hp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
      hp_param->poke_cnt = 1;
      hp_param->pokeID[0] = pokeID;
      hp_param->volume[0] = -downHP;

      rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->rankType = BPP_ATTACK;
      rank_param->rankVolume = upVolume;
      rank_param->poke_cnt = 0;
      rank_param->pokeID[0] = pokeID;
      rank_param->fStdMsgDisable = TRUE;
      rank_param->fAlmost = TRUE;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Haradaiko );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
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
    { BTL_EVENT_DAMAGEPROC_END,       handler_Feint_AfterDamage },   // �_���[�W������
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Feint_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );

    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_MAMORU) )
    {
      BTL_HANDEX_PARAM_TURNFLAG* flg_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      flg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
      flg_param->pokeID = target_pokeID;
      flg_param->flag = BPP_TURNFLG_MAMORU;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Feint );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_TuboWoTuku },          // �����ރ��U�n���h��
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Nemuru },          // �����ރ��U�n���h��
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( !BPP_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP    *hp_param;
      BTL_HANDEX_PARAM_ADD_SICK      *sick_param;

      hp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      hp_param->pokeID = pokeID;
      hp_param->recoverHP = (BPP_GetValue(bpp, BPP_MAX_HP) - BPP_GetValue(bpp, BPP_HP));

      if( BPP_GetPokeSick(bpp) != POKESICK_NULL )
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
      sick_param->exStr.ID = BTL_STRID_SET_Nemuru;
      sick_param->exStr.argCnt = 1;
      sick_param->exStr.args[0] = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ��������
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
    const BTL_POKEPARAM* self = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

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
 * �e�N�X�`���[�Q
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Texture2( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,   handler_Texture2_CheckNoEffect },
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Texture2 },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Texture2_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_GetPrevWazaID(bpp) == WAZANO_NULL ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
static void handler_Texture2( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    WazaID  waza = BPP_GetPrevWazaID( bpp );
    if( waza != WAZANO_NULL )
    {
      BTL_HANDEX_PARAM_CHANGE_TYPE* param;
      PokeType type = WAZADATA_GetType( waza );
      PokeType next_type = BTL_CALC_RandomResistType( type );

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
      param->next_type = PokeTypePair_MakePure( next_type );
      BTL_Printf("�Ώۃ|�P=%d, ���̃��U=%d, ���̃^�C�v=%d, ��R�^�C�v=%d\n",
        BPP_GetID(bpp), waza, type, param->next_type);
      param->pokeID = pokeID;
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Encore },          // �����ރ��U�n���h��
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

    if( !BPP_CheckSick(target, WAZASICK_ENCORE)
    &&  (BPP_GetPrevWazaID(target) != WAZANO_NULL)
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
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
 * ���傤�͂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Chouhatu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Chouhatu },          // �����ރ��U�n���h��
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

    if( !BPP_CheckSick(target, WAZASICK_TYOUHATSU) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
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
 * ���������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ichamon( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Ichamon },          // �����ރ��U�n���h��
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

    if( !BPP_CheckSick(target, WAZASICK_ICHAMON) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
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
 * ���Ȃ��΂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Kanasibari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Kanasibari },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };

  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Kanasibari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, WAZASICK_KANASIBARI) )
    {
      WazaID  prevWaza = BPP_GetPrevWazaID( target );
      if( prevWaza != WAZANO_NULL )
      {
        u8 prevWazaIdx = BPP_WAZA_SearchIdx( target, prevWaza );
        BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
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
 * �ӂ�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Fuuin( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Fuuin },          // �����ރ��U�n���h��
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
      BTL_HANDEX_PARAM_ADD_FLDEFF* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );

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
 * �A���}�Z���s�[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Alomatherapy( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Alomatherapy   },  // ���U�З͌���
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_IyasiNoSuzu   },  // ���U�З͌���
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

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, strID );

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
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Okimiyage   },  // ���U�З͌���
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Urami   },  // ���U�З͌���
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

        decpp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DECREMENT_PP, pokeID );
        decpp_param->pokeID = targetPokeID;
        decpp_param->volume = volume;
        decpp_param->wazaIdx = wazaIdx;

        msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
 * ��������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_JikoAnji( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_JikoAnji   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_JikoAnji( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = pokeID;
    rank_param->attack     = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_attack  = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( target, BPP_AGILITY_RANK );


    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_JikoAnji );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
    HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �n�[�g�X���b�v
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HeartSwap( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_HeartSwap   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_HeartSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
    const BTL_POKEPARAM* self = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = pokeID;
    rank_param->attack     = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_attack  = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( target, BPP_AGILITY_RANK );

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = target_pokeID;
    rank_param->attack     = BPP_GetValue( self, BPP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( self, BPP_DEFENCE_RANK );
    rank_param->sp_attack  = BPP_GetValue( self, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence = BPP_GetValue( self, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( self, BPP_AGILITY_RANK );

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HeartSwap );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �p���[�X���b�v
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PowerSwap( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerSwap   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_PowerSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
    const BTL_POKEPARAM* self = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = pokeID;
    rank_param->attack     = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->sp_attack  = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( self,   BPP_DEFENCE_RANK );
    rank_param->sp_defence = BPP_GetValue( self,   BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( self,   BPP_AGILITY_RANK );

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = target_pokeID;
    rank_param->attack     = BPP_GetValue( self,   BPP_ATTACK_RANK );
    rank_param->sp_attack  = BPP_GetValue( self,   BPP_SP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_defence = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( target, BPP_AGILITY_RANK );

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_PowerSwap );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �K�[�h�X���b�v
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_GuardSwap( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GuardSwap   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_GuardSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
    const BTL_POKEPARAM* self = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = pokeID;
    rank_param->attack     = BPP_GetValue( self,   BPP_ATTACK_RANK );
    rank_param->sp_attack  = BPP_GetValue( self,   BPP_SP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_defence = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( self,   BPP_AGILITY_RANK );

    rank_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = target_pokeID;
    rank_param->attack     = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->sp_attack  = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->defence    = BPP_GetValue( self,   BPP_DEFENCE_RANK );
    rank_param->sp_defence = BPP_GetValue( self,   BPP_SP_DEFENCE_RANK );
    rank_param->agility    = BPP_GetValue( target, BPP_AGILITY_RANK );

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_GuardSwap );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_LockON   },  // �����ރ��U
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
 * ���t���N�^�[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Refrector( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Refrector   },  // �����ރ��U
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
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_HikariNoKabe   },  // �����ރ��U
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
 * ����҂̂܂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SinpiNoMamori( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_SinpiNoMamori   },  // �����ރ��U
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
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_SiroiKiri   },  // �����ރ��U
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
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Oikaze   },  // �����ރ��U
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
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Omajinai   },  // �����ރ��U
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
 * �܂��т�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Makibisi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Makibisi   },  // �����ރ��U
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
 * �ǂ��т�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Dokubisi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Dokubisi   },  // �����ރ��U
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
 * �X�e���X���b�N
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_StealthRock( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_StealthRock   },  // �����ރ��U
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
static void common_SideEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, BPP_SICK_CONT cont, u16 strID )
{
  BTL_Printf("���V=%d, �g�����|�P=%d��\n", pokeID, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK));
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_HANDLER_SIDE_Add(side, effect, cont) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, strID );
      HANDEX_STR_AddArg( &param->str, side );
      BTL_Printf("�T�C�h�G�t�F�N�g�����@������=%d\n", strID);
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂��Â��̂܂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MikadukiNoMai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_MikadukiNoMai   },  // �����ރ��U
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

    eff_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_MIKADUKINOMAI;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, pokeID );

    kill_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    kill_param->pokeID = pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * ���₵�̂˂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_IyasiNoNegai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_IyasiNoNegai   },  // �����ރ��U
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

    eff_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_IYASINONEGAI;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, pokeID );

    kill_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    kill_param->pokeID = pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * �˂�������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Negaigoto( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Negaigoto   },  // �����ރ��U
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

    eff_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_NEGAIGOTO;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂炢�悿�E�͂߂̂˂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Miraiyoti( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Miraiyoti   },  // �����ރ��U
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

    eff_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_DELAY_ATTACK;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, targetPokeID );
    eff_param->param[0] = 3;
    eff_param->param[1] = BTL_EVENT_FACTOR_GetSubID( myHandle );
    eff_param->param_cnt = 2;

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Miraiyoti );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂炢�悿�E�͂߂̂˂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_HametuNoNegai( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_HametuNoNegai   },  // �����ރ��U
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

    eff_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_DELAY_ATTACK;
    eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, targetPokeID );
    eff_param->param[0] = 3; // delay turn
    eff_param->param[1] = BTL_EVENT_FACTOR_GetSubID( myHandle );
    eff_param->param_cnt = 2;

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HametuNoNegai );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Ieki   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_CHANGE_TOKUSEI *tok_param;
    BTL_HANDEX_PARAM_ADD_SICK       *sick_param;
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    sick_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    sick_param->poke_cnt = 1;
    sick_param->pokeID[0] = target_pokeID;
    sick_param->sickID = WAZASICK_IEKI;
    sick_param->fAlmost = TRUE;
    sick_param->sickCont = BPP_SICKCONT_MakePermanent();
    HANDEX_STR_Setup( &sick_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Ieki );
    HANDEX_STR_AddArg( &sick_param->exStr, target_pokeID );

    tok_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
    tok_param->pokeID = target_pokeID;
    tok_param->tokuseiID = POKETOKUSEI_NULL;
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Narikiri   },  // �����ރ��U
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
    BTL_HANDEX_PARAM_CHANGE_TOKUSEI*  tok_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    tok_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
    tok_param->pokeID = pokeID;
    tok_param->tokuseiID = BPP_GetValue( target, BPP_TOKUSEI );
    HANDEX_STR_Setup( &tok_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Narikiri );
    HANDEX_STR_AddArg( &tok_param->exStr, pokeID );
    HANDEX_STR_AddArg( &tok_param->exStr, target_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ƃ�ڂ�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TonboGaeri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,  handler_TonboGaeri   },         // �_���[�W����
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

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Tonbogaeri );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, clientID );

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_MEMBER, pokeID );
      param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���������X�s��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KousokuSpin( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,  handler_KousokuSpin   },         // �_���[�W����
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_KousokuSpin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_CURE_SICK* cure_param;
    BTL_HANDEX_PARAM_SIDEEFF_REMOVE* side_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    BtlSide  side;

    if( BPP_CheckSick(bpp, WAZASICK_YADORIGI) ){
      cure_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      cure_param->poke_cnt = 1;
      cure_param->pokeID[0] = pokeID;
      cure_param->sickCode = WAZASICK_YADORIGI;
    }
    if( BPP_CheckSick(bpp, WAZASICK_BIND) ){
      cure_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      cure_param->poke_cnt = 1;
      cure_param->pokeID[0] = pokeID;
      cure_param->sickCode = WAZASICK_BIND;
    }

    side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
    if( BTL_HANDER_SIDE_IsExist(side, BTL_SIDEEFF_MAKIBISI)
    ){
      side_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
      side_param->side = side;
      BTL_CALC_BITFLG_Construction( side_param->flags, sizeof(side_param->flags) );
      BTL_CALC_BITFLG_Set( side_param->flags, BTL_SIDEEFF_MAKIBISI );

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KousokuSpin );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, WAZANO_MAKIBISI );
    }
    if( BTL_HANDER_SIDE_IsExist(side, BTL_SIDEEFF_DOKUBISI)
    ){
      side_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
      side_param->side = side;
      BTL_CALC_BITFLG_Construction( side_param->flags, sizeof(side_param->flags) );
      BTL_CALC_BITFLG_Set( side_param->flags, BTL_SIDEEFF_DOKUBISI );

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KousokuSpin );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, WAZANO_DOKUBISI );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �o�g���^�b�`
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_BatonTouch( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_BatonTouch   },         // �_���[�W����
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

      eff_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
      eff_param->effect = BTL_POSEFF_BATONTOUCH;
      eff_param->pos = BTL_SVFLOW_PokeIDtoPokePos( flowWk, pokeID );
      eff_param->param[0] = pokeID;
      eff_param->param_cnt = 1;

      change_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_MEMBER, pokeID );
      change_param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �e���|�[�g
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Teleport( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Teleport   },         // �����ރ��U
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_Teleport_ExeCheck },  // ���s�`�F�b�N
    { BTL_EVENT_NIGERU_EXMSG,       handler_Teleport_ExMsg },     // ������Ƃ��̓��ꃁ�b�Z�[�W
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
    BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_QUIT_BATTLE, pokeID );
  }
}
static void handler_Teleport_ExMsg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, TRUE) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Teleport );
    HANDEX_STR_AddArg( &param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ȃ�����
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u16 itemID = BPP_GetItem( bpp );
    if( itemID != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_SET_ITEM* param;

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
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
         param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_EQUIP_ITEM, pokeID );
         param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
         param->itemID = itemID;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ł񂶂ӂ䂤
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_DenjiFuyuu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_DenjiFuyuu   },  // �����ރ��U
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_DenjiFuyuu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK*  param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

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
 * �Ă�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tedasuke( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,    handler_Tedasuke_Ready },       // �����ރ��U�n���h��
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

      if( !BPP_IsDead(bpp) )
      {
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

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
  BTL_EVENT_FACTOR_Remove( myHandle );  // �^�[���`�F�b�N�ŋ������E
}
//----------------------------------------------------------------------------------
/**
 * �ӂ��낾����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_FukuroDataki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_COUNT,  handler_FukuroDataki },    // �����ރ��U�n���h��
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
    if( BPP_GetTurnCount(bpp) )
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
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SoraWoTobu );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
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
    { BTL_EVENT_DAMAGEPROC_END,       handler_ShadowDive_AfterDamage },   // �_���[�W������
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
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_ShadowDive );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
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

    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_MAMORU) )
    {
      BTL_HANDEX_PARAM_TURNFLAG* flg_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      flg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
      flg_param->pokeID = target_pokeID;
      flg_param->flag = BPP_TURNFLG_MAMORU;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MamoruBreak );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
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
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Tobihaneru );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
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
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Diving );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
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
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_AnaWoHoru );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
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
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Kamaitati );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
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
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_GodBird );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
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

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RocketZutuki );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );

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
 *  ���΂ށE�ނ�����
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
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );
    u16 itemID = BPP_GetItem( target );
    if( ITEM_CheckNuts(itemID) )
    {
      BTL_HANDEX_PARAM_SET_ITEM* item_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      BTL_HANDEX_PARAM_EQUIP_ITEM* eq_param;

      item_param->pokeID = targetPokeID;
      item_param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &item_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Tuibamu );
      HANDEX_STR_AddArg( &item_param->exStr, pokeID );
      HANDEX_STR_AddArg( &item_param->exStr, itemID );

      eq_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_EQUIP_ITEM, pokeID );
      eq_param->header.failSkipFlag = TRUE;
      eq_param->itemID = itemID;
      eq_param->pokeID = pokeID;
    }
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
    { BTL_EVENT_DAMAGEPROC_END,  handler_Waruagaki },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Waruagaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_SHIFT_HP* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->volume[0] = -BTL_CALC_QuotMaxHP( bpp, 4 );
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
    { BTL_EVENT_DAMAGEPROC_END,       handler_Bakajikara },   // �_���[�W������
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
    { BTL_EVENT_UNCATEGORIZE_WAZA,    handler_Michidure_Ready },       // �����ރ��U�n���h��
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
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MichidureReady );
    HANDEX_STR_AddArg( &param->str, pokeID );

    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_Michidure_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BPP_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_KILL* kill_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      kill_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_KILL, pokeID );
      kill_param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MichidureDone );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );

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
    { BTL_EVENT_UNCATEGORIZE_WAZA,    handler_Onnen_Ready },       // �����ރ��U�n���h��
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
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_OnnenReady );
    HANDEX_STR_AddArg( &param->str, pokeID );

    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_Onnen_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BPP_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_PP* pp_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      WazaID  waza;

      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      const BTL_POKEPARAM* targetPoke = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );

      waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );

      pp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DECREMENT_PP, pokeID );
      pp_param->pokeID = target_pokeID;
      pp_param->wazaIdx = BPP_WAZA_SearchIdx( targetPoke, waza );
      pp_param->volume = BPP_WAZA_GetPP( targetPoke, pp_param->wazaIdx ) * -1;

      msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_OnnenDone );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
      HANDEX_STR_AddArg( &msg_param->str, waza );

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
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Tiisakunaru   },  // ���U�o���m�茈��
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
//----------------------------------------------------------------------------------
/**
 * �͂˂₷��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Haneyasume( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,  handler_Haneyasume   },  // ���U�o���m�茈��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Haneyasume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_TURNFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_TURNFLG_FLYING;
  }
}
//----------------------------------------------------------------------------------
/**
 * �������p���`
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KiaiPunch( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_BEFORE_FIGHT,  handler_KiaiPunch   },  // �^�[���ŏ��̃��U�V�[�P���X���O
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_KiaiPunch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_TURNFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_TURNFLAG, pokeID );
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    param->pokeID = pokeID;
    param->flag = BPP_TURNFLG_MUST_SHRINK;

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KiaiPunch );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}


//------------------------------------------------------------------------------------------------------
/**
 *  ���̃��U���Ăяo�����U
 */
//------------------------------------------------------------------------------------------------------

// ���O���郏�U�̃e�[�u��
// ���� �K�������Ƀ\�[�g���Ă������ƁI�I ����
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
 * ��т��ӂ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_YubiWoFuru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_YubiWoFuru     },  // �����U�Ăяo���i�����v�Z�O����j
    { BTL_EVENT_REQWAZA_MSG,            handler_YubiWoFuru_Msg },  // �����U���s�����b�Z�[�W
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
 * ������̂�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SizenNoTikara( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_SizenNoTikara     },  // �����U�Ăяo���i�����v�Z�O����j
    { BTL_EVENT_REQWAZA_MSG,            handler_SizenNoTikara_Msg },  // �����U���s�����b�Z�[�W
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

    // @@@ �{���͒n�`�ɉ����Ă����ƃt�N�U�c��
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
 * �˂��̂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nekonote( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_Nekonote     },         // �����U�Ăяo���i�����v�Z�O����j
    { BTL_EVENT_REQWAZA_PARAM,          handler_Nekonote_CheckParam },  // �����U�p�����[�^�`�F�b�N
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
 * �˂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Negoto( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_Negoto     },         // �����U�Ăяo���i�����v�Z�O����j
    { BTL_EVENT_REQWAZA_PARAM,          handler_Negoto_CheckParam },  // �����U�p�����[�^�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Negoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
 * �I�E��������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_OumuGaesi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,          handler_OumuGaesi_CheckParam },  // �����U�p�����[�^�`�F�b�N
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

    // �V���O����̏ꍇ�A����I���̕K�v���Ȃ��̂�NULL������
    if( targetPos == BTL_POS_NULL ){
      BtlPokePos  myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
      BtlRule  rule = BTL_SVFLOW_GetRule( flowWk );
      targetPos = BTL_MAINUTIL_GetOpponentPokePos( rule, myPos, 0 );
    }


    if( BTL_SERVERFLOW_RECEPT_GetTargetPokeID(flowWk, targetPos, targetPokeIDAry) )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeIDAry[0] );
      WazaID  waza = BPP_GetPrevWazaID( bpp );
      {
        BTL_Printf( "�I�E���F�_�����ʒu�Ƀ|�P���������B�ʒu=%d, ID=%d�ł�\n", targetPos, targetPokeIDAry[0]);
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
      BTL_Printf( "�I�E���F�_�����ʒu�Ƀ|�P�������Ȃ�\n");
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �����ǂ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Sakidori( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,        handler_Sakidori_CheckParam },  // �����U�p�����[�^�`�F�b�N
    { BTL_EVENT_WAZA_POWER,           handler_Sakidori_Power      },  // ���U�З͌���
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

    // �V���O����̏ꍇ�A����I���̕K�v���Ȃ��̂�NULL������
    if( targetPos == BTL_POS_NULL ){
      BtlPokePos  myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
      BtlRule  rule = BTL_SVFLOW_GetRule( flowWk );
      targetPos = BTL_MAINUTIL_GetOpponentPokePos( rule, myPos, 0 );
    }

    if( BTL_SERVERFLOW_RECEPT_GetTargetPokeID(flowWk, targetPos, targetPokeIDAry) )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeIDAry[0] );
      BTL_ACTION_PARAM action;
      WazaID waza;
      u8 fSucceess = FALSE;

      do{

        // ���肪���ɍs�����Ă����玸�s
        if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_WAZA_EXE) ){ break; }

        // ���^�[���̃A�N�V�������擾�ł��Ȃ������玸�s
        if( !BTL_SVFLOW_GetThisTurnAction( flowWk, targetPokeIDAry[0], &action ) ){ break; }

        // ���肪�_���[�W���U��I�����Ă��Ȃ��ꍇ�����s
        waza = BTL_ACTION_GetWazaID( &action );
        if( (waza == WAZANO_NULL)
        ||  (!WAZADATA_IsDamage(waza))
        ){
          break;
        }
        // �����܂ŗ����琬��
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
 * �܂˂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Manekko( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,         handler_Manekko_CheckParam },  // �����U�p�����[�^�`�F�b�N
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
 * �x�m���V���b�N
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_BenomShock( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_BenomShock },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_BenomShock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �ǂ���Ԃ̑���ɈЗ͂Q�{
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CheckSick(bpp, WAZASICK_DOKU) ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Tatarime( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Tatarime },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Tatarime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �|�P�n��ԏ�Ԃ̑���ɈЗ͂Q�{
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_GetPokeSick(bpp) != POKESICK_NULL ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �A�N���o�b�g
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Acrobat( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Acrobat },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Acrobat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �������ǂ����𑕔����Ă��Ȃ����ɈЗ͂Q�{
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BPP_GetItem(bpp) == ITEM_DUMMY_DATA )
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �A�V�X�g�p���[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_AsistPower( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_AsistPower },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_AsistPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �����̔\�͕ω��㏸�l���v*20���З͂ɉ��Z
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
 * �݂��т���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Mizubitasi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Mizubitasi },    // �����ރ��U�n���h��
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
      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
      param->next_type = PokeTypePair_MakePure( POKETYPE_MIZU );
      param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1+i );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �V���v���r�[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SimpleBeem( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_SimpleBeem   },  // �����ރ��U
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
    // �Ώۃ|�P�����̂Ƃ��������u���񂶂��v�ɏ�������
    for(i=0; i<targetCnt; ++i)
    {
      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
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
 * �Ȃ��܂Â���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_NakamaDukuri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_NakamaDukuri   },  // �����ރ��U
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u16 myTokusei = BPP_GetValue( bpp, BPP_TOKUSEI );
    u32 i;
    // �Ώۃ|�P�����̂Ƃ������������Ɠ������̂ɏ�������
    for(i=0; i<targetCnt; ++i)
    {
      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
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
 * �N���A�X���b�O
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_ClearSmog( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,  handler_ClearSmog   },         // �_���[�W����
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

    reset_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    reset_param->poke_cnt = 1;
    reset_param->pokeID[0] = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RankReset );
    HANDEX_STR_AddArg( &msg_param->str, reset_param->pokeID[0] );
  }
}

//----------------------------------------------------------------------------------
/**
 * �������Ԃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KarawoYaburu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_KarawoYaburu },   // �_���[�W������
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_KarawoYaburu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->rankType = WAZA_RANKEFF_DEFENCE;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_SP_DEFENCE;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_ATTACK;
    param->rankVolume = 2;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_SP_ATTACK;
    param->rankVolume = 2;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_AGILITY;
    param->rankVolume = 2;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;
  }
}
