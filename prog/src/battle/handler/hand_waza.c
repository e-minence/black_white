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
#include "..\btlv\btlv_effect.h"

#include "hand_common.h"
#include "hand_side.h"
#include "hand_field.h"
#include "hand_waza.h"



/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_STICK = EVENT_HANDLER_WORK_ELEMS - 1,
};

/**
 *  ���̃��U����
 */
typedef enum {

  COMBI_EFFECT_NULL = 0,
  COMBI_EFFECT_RAINBOW,   ///< ��      = ���E��
  COMBI_EFFECT_BURNING,   ///< �΂̊C  = ���E��
  COMBI_EFFECT_MOOR,      ///< ��      = ���E��

}CombiEffectType;


/* @todo �V���U���V���{�����ꎞ�I�Ɏ�ł����Ă��� */
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
  WAZANO_KARI_TELEKINESISU,
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

  WAZANO_KARI_DORIRURAINAA,
  WAZANO_KARI_DABURUTYOPPU,
  WAZANO_KARI_HAATOSUTANPU,
  WAZANO_KARI_UDDOHOON,
  WAZANO_KARI_SEINARUTURUGI,
  WAZANO_KARI_SHERUBUREEDO,
  WAZANO_KARI_HIITOSUTANPU,
  WAZANO_KARI_GURASUMIKISAA,
  WAZANO_KARI_HAADOROORAA,
  WAZANO_KARI_KOTTONGAADO,
  WAZANO_KARI_NAITOBAASUTO,
  WAZANO_KARI_SAIKOBUREIKU,
  WAZANO_KARI_SUIIBUBINTA,
  WAZANO_KARI_BOUHUU,
  WAZANO_KARI_AHUROBUREIKU,
  WAZANO_KARI_GIASOOSAA,
  WAZANO_KARI_KAENDAN,
  WAZANO_KARI_TEKUNOBASUTAA,
  WAZANO_KARI_INISIENOUTA,
  WAZANO_KARI_KISEKINOTURUGI,
  WAZANO_KARI_KOGOERUSEKAI,
  WAZANO_KARI_RAIGEKI,
  WAZANO_KARI_AOIHONOO,
  WAZANO_KARI_HONOONOMAI,
  WAZANO_KARI_HURIIZUBORUTO,
  WAZANO_KARI_KOORUDOHUREA,
  WAZANO_KARI_DONARITUKERU,
  WAZANO_KARI_TURARAOTOSI,
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL is_registable( u8 pokeID, WazaID waza, u8* fRegistered );
static void removeHandlerForce( u8 pokeID, WazaID waza );
static const BtlEventHandlerTable*  ADD_Texture( u32* numElems );
static void handler_Texture( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Hogosyoku( u32* numElems );
static void handler_Hogosyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_TrickRoom( u32* numElems );
static void handler_TrickRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Juryoku( u32* numElems );
static void handler_Juryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MizuAsobi( u32* numElems );
static void handler_MizuAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_DoroAsobi( u32* numElems );
static void handler_DoroAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Kiribarai( u32* numElems );
static void handler_Kiribarai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Kawarawari( u32* numElems );
static void handler_Kawarawari_DmgProc1( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kawarawari_DmgProcEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kawarawari_DmgDetermine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL handler_Kawarawari_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 dependID );
static const BtlEventHandlerTable*  ADD_Tobigeri( u32* numElems );
static void handler_Tobigeri_Avoid( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tobigeri_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void  common_TobigeriReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Monomane( u32* numElems );
static void handler_Monomane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Sketch( u32* numElems );
static void handler_Sketch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_KonoyubiTomare( u32* numElems );
static void handler_KonoyubiTomare_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KonoyubiTomare_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KonoyubiTomare_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KonoyubiTomare_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Refresh( u32* numElems );
static void handler_Refresh( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_KumoNoSu( u32* numElems );
static void handler_KumoNoSu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_KuroiKiri( u32* numElems );
static void handler_KuroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Haneru( u32* numElems );
static void handler_Haneru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Noroi( u32* numElems );
static void handler_Noroi_WazaParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Noroi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Denjiha( u32* numElems );
static void handler_Denjiha_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_NayamiNoTane( u32* numElems );
static void handler_NayamiNoTane_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NayamiNoTane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Yumekui( u32* numElems );
static void handler_Yumekui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Yuwaku( u32* numElems );
static void handler_Yuwaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_TriAttack( u32* numElems );
static void handler_TriAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_HimituNoTikara( u32* numElems );
static void handler_HimituNoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Osyaberi( u32* numElems );
static void handler_Osyaberi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Makituku( u32* numElems );
static void handler_Makituku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Uzusio( u32* numElems );
static void handler_Uzusio_CheckHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Uzusio_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_IkariNoMaeba( u32* numElems );
static void handler_IkariNoMaeba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_RyuuNoIkari( u32* numElems );
static void handler_RyuuNoIkari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SonicBoom( u32* numElems );
static void handler_SonicBoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Gamusyara( u32* numElems );
static void handler_Gamusyara_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gamusyara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_TikyuuNage( u32* numElems );
static void handler_TikyuuNage_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_TikyuuNage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_PsycoWave( u32* numElems );
static void handler_PsycoWave( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Takuwaeru( u32* numElems );
static void handler_Takuwaeru_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Takuwaeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Hakidasu( u32* numElems );
static void handler_Hakidasu_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hakidasu_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Hakidasu_Done( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Nomikomu( u32* numElems );
static void handler_Nomikomu_Ratio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Counter( u32* numElems );
static void handler_Counter_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Counter_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Counter_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MilerCoat( u32* numElems );
static void handler_MilerCoat_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MilerCoat_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MilerCoat_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MetalBurst( u32* numElems );
static void handler_MetalBurst_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MetalBurst_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MetalBurst_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_Counter_ExeCheck( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType );
static void common_Counter_SetTarget( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType );
static void common_Counter_CalcDamage( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType, fx32 ratio );
static BOOL common_Counter_GetRec( const BTL_POKEPARAM* bpp, WazaDamageType dmgType, BPP_WAZADMG_REC* rec );
static const BtlEventHandlerTable*  ADD_Totteoki( u32* numElems );
static void handler_Totteoki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Ibiki( u32* numElems );
static void handler_Ibiki_CheckFail_1( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Ibiki_CheckFail_2( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Akumu( u32* numElems );
static void handler_Akumu_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Fuiuti( u32* numElems );
static void handler_Fuiuti_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Oiuti( u32* numElems );
static void handler_Oiuti_Intr( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Oiuti_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Daibakuhatsu( u32* numElems );
static void handler_Daibakuhatsu_DmgDetermine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Daibakuhatsu_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Kiaidame( u32* numElems );
static void handler_Kiaidame( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Juden( u32* numElems );
static void handler_Juden_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Juden_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Juden_WazaStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Juden_WazaEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_HorobiNoUta( u32* numElems );
static void handler_HorobiNoUta_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_YadorigiNoTane( u32* numElems );
static void handler_YadorigiNoTane_Param( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Miyaburu( u32* numElems );
static void handler_Miyaburu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_NekoNiKoban( u32* numElems );
static void handler_NekoNiKoban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Ikari( u32* numElems );
static void handler_Ikari_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Ikari_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_AquaRing( u32* numElems );
static void handler_AquaRing( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_AquaRing_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Abareru( u32* numElems );
static void handler_Abareru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Sawagu( u32* numElems );
static void handler_Sawagu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sawagu_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sawagu_CheckSickFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Korogaru( u32* numElems );
static void handler_Korogaru_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Korugaru_Avoid( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Korogaru_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void  common_Korogaru_Unlock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Korogaru_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_TripleKick( u32* numElems );
static void handler_TripleKick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_TripleKick_HitCount( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_GyroBall( u32* numElems );
static void handler_GyroBall( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Revenge( u32* numElems );
static void handler_Revenge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Jitabata( u32* numElems );
static void handler_Jitabata( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Karagenki( u32* numElems );
static void handler_Karagenki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Sippegaesi( u32* numElems );
static void handler_Sippegaesi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Funka( u32* numElems );
static void handler_Funka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Siboritoru( u32* numElems );
static void handler_Siboritoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Siomizu( u32* numElems );
static void handler_Siomizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Ongaesi( u32* numElems );
static void handler_Ongaesi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Yatuatari( u32* numElems );
static void handler_Yatuatari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MezamasiBinta( u32* numElems );
static void handler_MezamasiBinta( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MezamasiBinta_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Kituke( u32* numElems );
static void handler_Kituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kituke_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Present( u32* numElems );
static void handler_Present_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Present_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Present_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Kirifuda( u32* numElems );
static void handler_Kirifuda( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Osioki( u32* numElems );
static void handler_Osioki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_RenzokuGiri( u32* numElems );
static void handler_RenzokuGiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Dameosi( u32* numElems );
static void handler_Dameosi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Ketaguri( u32* numElems );
static void handler_Ketaguri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_WeatherBall( u32* numElems );
static void handler_WeatherBall_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_WeatherBall_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Tatumaki( u32* numElems );
static void handler_Tatumaki_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tatumaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SkyUpper( u32* numElems );
static void handler_SkyUpper( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Kaminari( u32* numElems );
static void handler_Kaminari_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kaminari_excuseHitCalc( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kaminari_hitRatio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Fubuki( u32* numElems );
static void handler_Fubuki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Jisin( u32* numElems );
static void handler_Jisin_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Jisin_damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SabakiNoTubute( u32* numElems );
static void handler_SabakiNoTubute( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MezameruPower( u32* numElems );
static void handler_MezameruPower_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MezameruPower_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SizenNoMegumi( u32* numElems );
static void handler_SizenNoMegumi_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenNoMegumi_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenNoMegumi_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenNoMegumi_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Hatakiotosu( u32* numElems );
static void handler_Hatakiotosu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MagicCoat( u32* numElems );
static void handler_MagicCoat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagicCoat_CheckRob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagicCoat_Reflect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagicCoat_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Yokodori( u32* numElems );
static void handler_Yokodori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Yokodori_CheckRob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Yokodori_Rob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Yokodori_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Dorobou( u32* numElems );
static void handler_Dorobou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Trick( u32* numElems );
static void handler_Trick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Magnitude( u32* numElems );
static void handler_Magnitude_effect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Magnitude_pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Naminori( u32* numElems );
static void handler_Naminori_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Naminori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Fumituke( u32* numElems );
static void handler_Fumituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Mineuti( u32* numElems );
static void handler_Mineuti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Koraeru( u32* numElems );
static void handler_Koraeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Koraeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Koraeru_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Koraeru_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Mamoru( u32* numElems );
static void handler_Mamoru_StartSeq( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Mamoru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Mamoru_ExeFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Mamoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void IncrementMamoruCounter( BTL_SVFLOW_WORK* flowWk, u8 pokeID );
static const BtlEventHandlerTable*  ADD_Gaman( u32* numElems );
static void handler_Gaman( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gaman_WazaMsg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gaman_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gaman_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gaman_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gaman_Fail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void gaman_ReleaseStick( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static u32 gaman_getTotalDamage( const BTL_POKEPARAM* bpp );
static const BtlEventHandlerTable*  ADD_Recycle( u32* numElems );
static void handler_Recycle( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_PsycoShift( u32* numElems );
static void handler_PsycoShift( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Itamiwake( u32* numElems );
static void handler_Itamiwake( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Haradaiko( u32* numElems );
static void handler_Haradaiko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Feint( u32* numElems );
static void handler_Feint_MamoruBreak( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Feint_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Feint_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Feint_Start( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Feint_End( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL handler_Feint_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );
static const BtlEventHandlerTable*  ADD_TuboWoTuku( u32* numElems );
static void handler_TuboWoTuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Nemuru( u32* numElems );
static void handler_Nemuru_exeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nemuru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Meromero( u32* numElems );
static void handler_Meromero_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Texture2( u32* numElems );
static void handler_Texture2( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Encore( u32* numElems );
static void handler_Encore( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Chouhatu( u32* numElems );
static void handler_Chouhatu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Ichamon( u32* numElems );
static void handler_Ichamon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Kanasibari( u32* numElems );
static void handler_Kanasibari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Fuuin( u32* numElems );
static void handler_Fuuin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Alomatherapy( u32* numElems );
static void handler_Alomatherapy( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_IyasiNoSuzu( u32* numElems );
static void handler_IyasiNoSuzu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_CureFriendPokeSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, u16 strID );
static const BtlEventHandlerTable*  ADD_Okimiyage( u32* numElems );
static void handler_Okimiyage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Urami( u32* numElems );
static void handler_Urami( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_JikoAnji( u32* numElems );
static void handler_JikoAnji( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_HeartSwap( u32* numElems );
static void handler_HeartSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_PowerSwap( u32* numElems );
static void handler_PowerSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_GuardSwap( u32* numElems );
static void handler_GuardSwap( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_PowerTrick( u32* numElems );
static void handler_PowerTrick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_PowerShare( u32* numElems );
static void handler_PowerShare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_GuardShare( u32* numElems );
static void handler_GuardShare( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_LockON( u32* numElems );
static void handler_LockON( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Refrector( u32* numElems );
static void handler_Refrector( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_HikariNoKabe( u32* numElems );
static void handler_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SinpiNoMamori( u32* numElems );
static void handler_SinpiNoMamori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SiroiKiri( u32* numElems );
static void handler_SiroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Oikaze( u32* numElems );
static void handler_Oikaze( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Omajinai( u32* numElems );
static void handler_Omajinai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Makibisi( u32* numElems );
static void handler_Makibisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Dokubisi( u32* numElems );
static void handler_Dokubisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_StealthRock( u32* numElems );
static void handler_StealthRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_WideGuard( u32* numElems );
static void handler_WideGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_SideEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, BPP_SICK_CONT cont, u16 strID );
static const BtlEventHandlerTable*  ADD_Hensin( u32* numElems );
static void handler_Hensin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MikadukiNoMai( u32* numElems );
static void handler_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_IyasiNoNegai( u32* numElems );
static void handler_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Negaigoto( u32* numElems );
static void handler_Negaigoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Miraiyoti( u32* numElems );
static void handler_Miraiyoti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Miraiyoti_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_delayAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u8 targetPos );
static const BtlEventHandlerTable*  ADD_HametuNoNegai( u32* numElems );
static void handler_HametuNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HametuNoNegai_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Ieki( u32* numElems );
static void handler_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Narikiri( u32* numElems );
static void handler_Narikiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_TonboGaeri( u32* numElems );
static void handler_TonboGaeri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_KousokuSpin( u32* numElems );
static void handler_KousokuSpin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_BatonTouch( u32* numElems );
static void handler_BatonTouch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Teleport( u32* numElems );
static void handler_Teleport_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Teleport( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Teleport_ExMsg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Nagetukeru( u32* numElems );
static void handler_Nagetukeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nagetukeru_WazaPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nagetukeru_DmgAfter( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_DenjiFuyuu( u32* numElems );
static void handler_DenjiFuyuu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Tedasuke( u32* numElems );
static void handler_Tedasuke_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tedasuke_WazaPow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tedasuke_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_FukuroDataki( u32* numElems );
static void handler_FukuroDataki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Nekodamasi( u32* numElems );
static void handler_Nekodamasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_AsaNoHizasi( u32* numElems );
static void handler_AsaNoHizasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SoraWoTobu( u32* numElems );
static void handler_SoraWoTobu_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SoraWoTobu_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_ShadowDive( u32* numElems );
static void handler_ShadowDive_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ShadowDive_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ShadowDive_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Tobihaneru( u32* numElems );
static void handler_Tobihaneru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tobihaneru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Diving( u32* numElems );
static void handler_Diving_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Diving_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_AnaWoHoru( u32* numElems );
static void handler_AnaWoHoru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_AnaWoHoru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SolarBeam( u32* numElems );
static void handler_SolarBeam_TameSkip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SolarBeam_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SolarBeam_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Kamaitati( u32* numElems );
static void handler_Kamaitati_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_GodBird( u32* numElems );
static void handler_GodBird_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_RocketZutuki( u32* numElems );
static void handler_RocketZutuki_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* ADD_Tuibamu( u32* numElems );
static void handler_Tuibamu_DmgDetermine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tuibamu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* ADD_Waruagaki( u32* numElems );
static void handler_Waruagaki_WazaParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Waruagaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Michidure( u32* numElems );
static void handler_Michidure_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Michidure_WazaSeqStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Michidure_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Michidure_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Onnen( u32* numElems );
static void handler_Onnen_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Onnen_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Onnen_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Tiisakunaru( u32* numElems );
static void handler_Tiisakunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Marukunaru( u32* numElems );
static void handler_Marukunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Haneyasume( u32* numElems );
static void handler_Haneyasume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_KiaiPunch( u32* numElems );
static void handler_KiaiPunch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL isWazaExclude( const u16* wazaTbl, u32 tblElems, WazaID waza );
static BOOL isReqWazaForbid( WazaID waza, const u16* exTbl, u32 exTblElems );
static const BtlEventHandlerTable*  ADD_YubiWoFuru( u32* numElems );
static void handler_YubiWoFuru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_YubiWoFuru_Msg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SizenNoTikara( u32* numElems );
static void handler_SizenNoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SizenNoTikara_Msg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Nekonote( u32* numElems );
static void handler_Nekonote( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Nekonote_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Negoto( u32* numElems );
static void handler_Negoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Negoto_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_OumuGaesi( u32* numElems );
static void handler_OumuGaesi_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Sakidori( u32* numElems );
static void handler_Sakidori_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sakidori_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Manekko( u32* numElems );
static void handler_Manekko_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_GensiNoTikara( u32* numElems );
static void handler_GensiNoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_BenomShock( u32* numElems );
static void handler_BenomShock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Tatarime( u32* numElems );
static void handler_Tatarime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Acrobat( u32* numElems );
static void handler_Acrobat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_AsistPower( u32* numElems );
static void handler_AsistPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_HeavyBomber( u32* numElems );
static void handler_HeavyBomber( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_ElectBall( u32* numElems );
static void handler_ElectBall( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_EchoVoice( u32* numElems );
static void handler_EchoVoice( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Katakiuti( u32* numElems );
static void handler_Katakiuti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Ikasama( u32* numElems );
static void handler_Ikasama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Mizubitasi( u32* numElems );
static void handler_Mizubitasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SimpleBeem( u32* numElems );
static void handler_SimpleBeem( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_NakamaDukuri( u32* numElems );
static void handler_NakamaDukuri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_ClearSmog( u32* numElems );
static void handler_ClearSmog( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Yakitukusu( u32* numElems );
static void handler_Yakitukusu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_TomoeNage( u32* numElems );
static void handler_TomoeNage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Utiotosu( u32* numElems );
static void handler_Utiotosu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_KarawoYaburu( u32* numElems );
static void handler_KarawoYaburu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MirrorType( u32* numElems );
static void handler_MirrorType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_BodyPurge( u32* numElems );
static void handler_BodyPurge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_PsycoShock( u32* numElems );
static void handler_PsycoShock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_NasiKuzusi( u32* numElems );
static void handler_NasiKuzusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_WonderRoom( u32* numElems );
static void handler_WonderRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MagicRoom( u32* numElems );
static void handler_MagicRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_HajikeruHonoo( u32* numElems );
static void handler_HajikeruHonoo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SyncroNoise( u32* numElems );
static void handler_SyncroNoise( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_GiftPass( u32* numElems );
static void handler_GiftPass( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Inotigake( u32* numElems );
static void handler_Inotigake_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Inotigake_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_OsakiniDouzo( u32* numElems );
static void handler_OsakiniDouzo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Sakiokuri( u32* numElems );
static void handler_Sakiokuri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Rinsyou( u32* numElems );
static void handler_Rinsyou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Rinsyou_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_FastGuard( u32* numElems );
static void handler_FastGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_SideChange( u32* numElems );
static void handler_SideChange( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Telekinesis( u32* numElems );
static void handler_Telekinesis( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_FreeFall( u32* numElems );
static void handler_FreeFall_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_FreeFall_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_FreeFall_TypeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_InisieNoUta( u32* numElems );
static void handler_InisieNoUta( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_MiracleEye( u32* numElems );
static void handler_MiracleEye( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_Seityou( u32* numElems );
static void handler_Seityou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static CombiEffectType GetCombiWazaType( WazaID waza1, WazaID waza2 );
static const BtlEventHandlerTable*  ADD_MizuNoTikai( u32* numElems );
static const BtlEventHandlerTable*  ADD_HonooNoTikai( u32* numElems );
static const BtlEventHandlerTable*  ADD_KusaNoTikai( u32* numElems );
static void handler_CombiWaza_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_CombiWaza_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_CombiWaza_WazaParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_CombiWaza_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_CombiWaza_AfterDmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  ADD_TechnoBaster( u32* numElems );
static void handler_TechnoBaster( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );

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
//  typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, WazaID wazaID, u8 pokeID );
  typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numElems );


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
//    { WAZANO_BAKADIKARA,      ADD_Bakajikara    },
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
    { WAZANO_DENZIHA,         ADD_Denjiha       },
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
    { WAZANO_KOUSOKUSUPIN,    ADD_KousokuSpin   },
    { WAZANO_HUKITOBASI,      ADD_KousokuSpin   },
    { WAZANO_PAWAATORIKKU,    ADD_PowerTrick    },
    { WAZANO_HENSIN,          ADD_Hensin        },
    { WAZANO_DAIBAKUHATU,     ADD_Daibakuhatsu  },
    { WAZANO_ZIBAKU,          ADD_Daibakuhatsu  },
    { WAZANO_KIAIDAME,        ADD_Kiaidame      },
    { WAZANO_IKARI,           ADD_Ikari         },
    { WAZANO_GENSINOTIKARA,   ADD_GensiNoTikara },
    { WAZANO_AYASIIKAZE,      ADD_GensiNoTikara },  // ���₵������=���񂵂̂�����
    { WAZANO_GINIRONOKAZE,    ADD_GensiNoTikara },  // ���񂢂�̂���=���񂵂̂�����
    { WAZANO_KAMINARI,        ADD_Kaminari      },
    { WAZANO_HUBUKI,          ADD_Fubuki        },
    { WAZANO_NEGAIGOTO,       ADD_Negaigoto     },
    { WAZANO_OMAZINAI,        ADD_Omajinai      },
    { WAZANO_MIYABURU,        ADD_Miyaburu      },
    { WAZANO_KAGIWAKERU,      ADD_Miyaburu      },  // �����킯��=�݂�Ԃ�
    { WAZANO_MIRAKURUAI,      ADD_MiracleEye    },
    { WAZANO_SEITYOU,         ADD_Seityou       },

    // �ȉ��A�V���U
    { WAZANO_KARI_BENOMUSHOKKU,     ADD_BenomShock      },
    { WAZANO_KARI_IKARINOKONA,      ADD_KonoyubiTomare  },  // ������̂���=���̂�тƂ܂�
    { WAZANO_KARI_MIZUBITASI,       ADD_Mizubitasi      },
    { WAZANO_KARI_SINPURUBIIMU,     ADD_SimpleBeem      },
    { WAZANO_KARI_NAKAMADUKURI,     ADD_NakamaDukuri    },
    { WAZANO_KARI_KURIASUMOGGU,     ADD_ClearSmog       },
    { WAZANO_KARI_ASISUTOPAWAA,     ADD_AsistPower      },
    { WAZANO_KARI_KARAWOYABURU,     ADD_KarawoYaburu    },
    { WAZANO_KARI_TATARIME,         ADD_Tatarime        },
    { WAZANO_KARI_AKUROBATTO,       ADD_Acrobat         },
    { WAZANO_KARI_BORUTOCHENZI,     ADD_TonboGaeri      },  // �{���g�`�F���W=�Ƃ�ڂ�����
    { WAZANO_KARI_WAIDOGAADO,       ADD_WideGuard       },
    { WAZANO_KARI_MIRAATAIPU,       ADD_MirrorType      },
    { WAZANO_KARI_PAWAASHEA,        ADD_PowerShare      },
    { WAZANO_KARI_GAADOSHEA,        ADD_GuardShare      },
    { WAZANO_KARI_BODYPAAZI,        ADD_BodyPurge       },
    { WAZANO_KARI_HEBIIBONBAA,      ADD_HeavyBomber     },
    { WAZANO_KARI_HIITOSUTANPU,     ADD_HeavyBomber     },  // �q�[�g�X�^���v=�w�r�[�{���o�[
    { WAZANO_KARI_WANDAARUUMU,      ADD_WonderRoom      },
    { WAZANO_KARI_MAZIKKURUUMU,     ADD_MagicRoom       },
    { WAZANO_KARI_SAIKOSYOKKU,      ADD_PsycoShock      },
    { WAZANO_KARI_SAIKOBUREIKU,     ADD_PsycoShock      },  // �T�C�R�u���C�N=�T�C�R�V���b�N
    { WAZANO_KARI_HAZIKERUHONOO,    ADD_HajikeruHonoo   },
    { WAZANO_KARI_EREKUTOBOORU,     ADD_ElectBall       },
    { WAZANO_KARI_SINKURONOIZU,     ADD_SyncroNoise     },
    { WAZANO_KARI_NASIKUZUSI,       ADD_NasiKuzusi      },
    { WAZANO_KARI_SEINARUTURUGI,    ADD_NasiKuzusi      },  // �����Ȃ�邬=�Ȃ�������
    { WAZANO_KARI_EKOOBOISU,        ADD_EchoVoice       },
    { WAZANO_KARI_YAKITUKUSU,       ADD_Yakitukusu      },
    { WAZANO_KARI_GIHUTOPASU,       ADD_GiftPass        },
    { WAZANO_KARI_TOMOENAGE,        ADD_TomoeNage       },
    { WAZANO_KARI_DORAGONTEERU,     ADD_TomoeNage       },  // �h���S���e�[��=�Ƃ����Ȃ�
    { WAZANO_KARI_KATAKIUTI,        ADD_Katakiuti       },
    { WAZANO_KARI_IKASAMA,          ADD_Ikasama         },
    { WAZANO_KARI_UTIOTOSU,         ADD_Utiotosu        },
    { WAZANO_KARI_INOTIGAKE,        ADD_Inotigake       },
    { WAZANO_KARI_OSAKINIDOUZO,     ADD_OsakiniDouzo    },
    { WAZANO_KARI_SAKIOKURI,        ADD_Sakiokuri       },
    { WAZANO_KARI_RINSYOU,          ADD_Rinsyou         },
    { WAZANO_KARI_FASTOGAADO,       ADD_FastGuard       },
    { WAZANO_KARI_SAIDOCHENZI,      ADD_SideChange      },
    { WAZANO_KARI_TELEKINESISU,     ADD_Telekinesis     },
    { WAZANO_KARI_FURIIFOORU,       ADD_FreeFall        },
    { WAZANO_KARI_HAADOROORAA,      ADD_Fumituke        },  // �n�[�h���[���[=�ӂ݂�
    { WAZANO_KARI_BOUHUU,           ADD_Kaminari        },  // �ڂ��ӂ�=���݂Ȃ�
    { WAZANO_KARI_KISEKINOTURUGI,   ADD_PsycoShock      },  // �������̂邬=�T�C�R�V���b�N
    { WAZANO_KARI_INISIENOUTA,      ADD_InisieNoUta     },
    { WAZANO_KARI_TEKUNOBASUTAA,    ADD_TechnoBaster    },  // �e�N�m�o�X�^�[=���΂��̂Ԃ�

    { WAZANO_KARI_MIZUNOTIKAI,      ADD_MizuNoTikai     },
    { WAZANO_KARI_HONOONOTIKAI,     ADD_HonooNoTikai    },
    { WAZANO_KARI_KUSANOTIKAI,      ADD_KusaNoTikai     },
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
        u32 numHandlers;
        const BtlEventHandlerTable* handlerTable = funcTbl[i].func( &numHandlers );
        if( handlerTable )
        {
          BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, agi, pokeID, handlerTable, numHandlers );
          return TRUE;
        }
      }
      else if( fRegistered ){
  //      BTL_Printf("���U�n���h��[%d]�͂��łɓo�^�ς�\n", waza);
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
  //    BTL_Printf("���ɓ������U���o�^����Ă���\n");
      *fRegistered = TRUE;
      return FALSE;
    }
    if( ++cnt > EVENT_WAZA_STICK_MAX ){
//      BTL_Printf("�����|�P����[%d]�̓\��t�����U��%d���z����\n", pokeID, EVENT_WAZA_STICK_MAX);
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
  BTL_EVENT_FACTOR* factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    if( BTL_EVENT_FACTOR_GetSubID(factor) == waza){
      BTL_EVENT_FACTOR_Remove( factor );
      factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
    }else{
      factor = BTL_EVENT_GetNextFactor( factor );
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
static const BtlEventHandlerTable*  ADD_Texture( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Texture },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Texture( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 waza_cnt = BPP_WAZA_GetCount( bpp );
    PokeTypePair next_type;
    {
      PokeType type[ PTL_WAZA_MAX ];
      u16 i;
      for(i=0; i<waza_cnt; ++i){
        type[i] = WAZADATA_GetType( BPP_WAZA_GetID(bpp, i) );
      }
      i = BTL_CALC_GetRand( waza_cnt );
      next_type = PokeTypePair_MakePure( type[i] );
    }

    {
      BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
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
static const BtlEventHandlerTable*  ADD_Hogosyoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Hogosyoku },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Hogosyoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlBgAttr  bg = BTL_SVFTOOL_GetLandForm( flowWk );
    PokeType   type = POKETYPE_NORMAL;

    switch( bg ){

    case BATTLE_BG_ATTR_GROUND:     //�ʏ�n��
    case BATTLE_BG_ATTR_GROUND_S1:  //�l�G����n�ʂP
    case BATTLE_BG_ATTR_GROUND_S2:  //�l�G����n�ʂQ
    case BATTLE_BG_ATTR_PALACE:     //�p���X�ł̑ΐ��p
    case BATTLE_BG_ATTR_SAND:       //���n
    case BATTLE_BG_ATTR_MARSH:      //�󂢎���
      type = POKETYPE_JIMEN;
      break;

    case BATTLE_BG_ATTR_E_INDOOR:   //�G���J�E���g����
    default:
      type = POKETYPE_NORMAL;
      break;

    case BATTLE_BG_ATTR_CAVE:           //���A
      type = POKETYPE_IWA;
      break;

    case BATTLE_BG_ATTR_LAWN:           //�Ő�
    case BATTLE_BG_ATTR_E_GRASS:        //�G���J�E���g��
      type = POKETYPE_KUSA;
      break;

    case BATTLE_BG_ATTR_WATER:          //����
    case BATTLE_BG_ATTR_POOL:           //�����܂�
    case BATTLE_BG_ATTR_SHOAL:          //��
      type = POKETYPE_MIZU;
      break;

    case BATTLE_BG_ATTR_SNOW:           //�ጴ
    case BATTLE_BG_ATTR_ICE:            //�X��
      type = POKETYPE_KOORI;
      break;
    }


    {
      PokeTypePair  pairType = PokeTypePair_MakePure( type );
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

      if( BPP_GetPokeType(bpp) != pairType )
      {
        BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
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
static const BtlEventHandlerTable*  ADD_TrickRoom( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_TrickRoom },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_TrickRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_TRICKROOM) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_TRICKROOM;
      param->cont = BPP_SICKCONT_MakeTurn( 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TrickRoom );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_REMOVE_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_TRICKROOM;

    #if 0
      {
        BTL_HANDEX_PARAM_MESSAGE*  msg_param;
        msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TrickRoomOff );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
      }
    #endif

    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���イ��傭
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Juryoku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL,     handler_Juryoku },          // �����ރ��U�n���h��
  };

  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Juryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU) == FALSE)
  ){
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;
    BTL_HANDEX_PARAM_MESSAGE*  msg_param;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
    param->effect = BTL_FLDEFF_JURYOKU;
    param->cont = BPP_SICKCONT_MakeTurn( 5 );
    param->exStr.type = BTL_STRTYPE_STD;
    param->exStr.ID = BTL_STRID_STD_Jyuryoku;

    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_JURYOKU_CHECK, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂�������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MizuAsobi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_MizuAsobi },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MizuAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
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
static const BtlEventHandlerTable*  ADD_DoroAsobi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_DoroAsobi },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_DoroAsobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_FLDEFF* param;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
    param->effect = BTL_FLDEFF_DOROASOBI;
    param->cont = BPP_SICKCONT_MakePoke( pokeID );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Doroasobi );
  }
}
//----------------------------------------------------------------------------------
/**
 * ����΂炢
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Kiribarai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Kiribarai },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Kiribarai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;
    BTL_HANDEX_PARAM_SIDEEFF_REMOVE* remove_param;

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->rankType = WAZA_RANKEFF_AVOID;
    rank_param->rankVolume = -1;
    rank_param->fAlmost = TRUE;
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = targetPokeID;

    remove_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
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
static const BtlEventHandlerTable*  ADD_Kawarawari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1,     handler_Kawarawari_DmgProc1     },  // �_���[�W�v�Z�n���h��
    { BTL_EVENT_WAZA_DMG_PROC_END,  handler_Kawarawari_DmgProcEnd   },  // �_���[�W�v�Z�I���n���h��
    { BTL_EVENT_WAZA_DMG_DETERMINE, handler_Kawarawari_DmgDetermine },  // �_���[�W�m��n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�v�Z�n���h��
static void handler_Kawarawari_DmgProc1( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    BTL_EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Kawarawari_SkipCheck );
  }
}
// �_���[�W�v�Z�I���n���h��
static void handler_Kawarawari_DmgProcEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    BTL_EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
  }
}
// �_���[�W�m��n���h��
static void handler_Kawarawari_DmgDetermine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BtlSide side = BTL_MAINUTIL_PokeIDtoSide( targetPokeID );

    if( BTL_HANDER_SIDE_IsExist(side, BTL_SIDEEFF_REFRECTOR)
    ||  BTL_HANDER_SIDE_IsExist(side, BTL_SIDEEFF_HIKARINOKABE)
    ){
      BTL_HANDEX_PARAM_SIDEEFF_REMOVE* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
      param->side = side;
      BTL_CALC_BITFLG_Construction( param->flags, sizeof(param->flags) );
      BTL_CALC_BITFLG_Set( param->flags, BTL_SIDEEFF_REFRECTOR );
      BTL_CALC_BITFLG_Set( param->flags, BTL_SIDEEFF_HIKARINOKABE );
    }
  }
}
// �X�L�b�v�`�F�b�N�n���h��
static BOOL handler_Kawarawari_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 dependID )
{
  if( factorType == BTL_EVENT_FACTOR_SIDE )
  {
    if( (subID == BTL_SIDEEFF_REFRECTOR)
    ||  (subID == BTL_SIDEEFF_HIKARINOKABE)
    ){
      u8 myPokeID = BTL_EVENT_FACTOR_GetPokeID( myHandle );
      BtlSide mySide = BTL_MAINUTIL_PokeIDtoSide( myPokeID );

      if( dependID != mySide ){
        return TRUE;
      }
    }
  }
  return FALSE;
}



//----------------------------------------------------------------------------------
/**
 * �Ƃт���E�ƂтЂ�����
 *
 * ���U���O�ꂽ���A�܂��͌��ʂ��Ȃ��������ɁA������HP��1/3�_���[�W���󂯂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Tobigeri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_NO_EFFECT, handler_Tobigeri_NoEffect  }, // ���ʂȂ������n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Tobigeri_Avoid( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    common_TobigeriReaction( myHandle, flowWk, pokeID, work );
  }
}
static void handler_Tobigeri_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    common_TobigeriReaction( myHandle, flowWk, pokeID, work );
  }
}
/**
 *  �Ƃт��蔽���_���[�W��������
 */
static void  common_TobigeriReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  u32 damage = BTL_CALC_QuotMaxHP( bpp, 2 );

  BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
  param->pokeID = pokeID;
  param->damage = damage;
  HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HazureJibaku );
  HANDEX_STR_AddArg( &param->exStr, pokeID );
}
//----------------------------------------------------------------------------------
/**
 * ���̂܂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Monomane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Monomane },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_Monomane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    WazaID waza = BPP_GetPrevWazaID( target );
    if( (waza != WAZANO_NULL)
    &&  (BPP_WAZA_SearchIdx(self, waza) == PTL_WAZA_MAX)
    ){
      u8 wazaIdx = BPP_WAZA_SearchIdx( self, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        BTL_HANDEX_PARAM_UPDATE_WAZA*  param;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_UPDATE_WAZA, pokeID );
        param->pokeID = pokeID;
        param->wazaIdx = wazaIdx;
        param->waza = waza;
        param->ppMax = 0;
        param->fPermanent = FALSE;

        msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
static const BtlEventHandlerTable*  ADD_Sketch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Sketch },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_UPDATE_WAZA, pokeID );
        param->pokeID = pokeID;
        param->wazaIdx = wazaIdx;
        param->waza = waza;
        param->ppMax = 0;
        param->fPermanent = TRUE;

        msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
static const BtlEventHandlerTable*  ADD_KonoyubiTomare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND, handler_KonoyubiTomare_ExeCheck  },  // ���U�o�����ۃ`�F�b�N
    { BTL_EVENT_UNCATEGORIZE_WAZA,      handler_KonoyubiTomare_Exe       },  // �����ރ��U���s
    { BTL_EVENT_DECIDE_TARGET,          handler_KonoyubiTomare_Target    },  // �^�[�Q�b�g����
    { BTL_EVENT_TURNCHECK_BEGIN,        handler_KonoyubiTomare_TurnCheck },  // �^�[���`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�o�����ۃ`�F�b�N�F�V���O���Ȃ玸�s����
static void handler_KonoyubiTomare_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_SVFTOOL_GetRule(flowWk) == BTL_RULE_SINGLE )
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
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Konoyubi );
    HANDEX_STR_AddArg( &param->str, pokeID );
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_KonoyubiTomare_Target( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_SVFTOOL_GetRule(flowWk) != BTL_RULE_SINGLE )
  {
    u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if (!BTL_MAINUTIL_IsFriendPokeID(pokeID, atkPokeID) ){
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
static const BtlEventHandlerTable*  ADD_Refresh( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Refresh },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
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
static const BtlEventHandlerTable*  ADD_KumoNoSu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_KumoNoSu },    // �����ރ��U�n���h��
//    { BTL_EVENT_USE_ITEM,      handler_KuraboNomi_Exe },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_KumoNoSu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(bpp, WAZASICK_TOOSENBOU) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->sickID = WAZASICK_TOOSENBOU;
      param->sickCont = BTL_CALC_MakeWazaSickCont_Poke( targetPokeID );

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_CantEscWaza );
      HANDEX_STR_AddArg( &param->exStr,targetPokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���낢����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_KuroiKiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_KuroiKiri },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_KuroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    reset_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    {
      BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokePos( flowWk, pokeID );
      BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_AREA_ALL, myPos );

      reset_param->poke_cnt = BTL_SVFTOOL_ExpandPokeID( flowWk, expos, reset_param->pokeID );
    }

    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_RankReset );
  }
}
//----------------------------------------------------------------------------------
/**
 * �͂˂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Haneru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Haneru },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_Haneru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_NotHappen );
  }
}
//----------------------------------------------------------------------------------
/**
 * �̂낢
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Noroi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,        handler_Noroi_WazaParam }, // ���U�p�����[�^����n���h��
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Noroi           }, // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�p�����[�^����n���h��
static void handler_Noroi_WazaParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    WazaTarget targetType = BTL_CALC_GetNoroiTargetType( bpp );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_TARGET_TYPE, targetType );
  }
}
// �����ރ��U�n���h��
static void handler_Noroi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 effIndex = BTLV_WAZAEFF_NOROI_NORMAL;

    if( BPP_IsMatchType(bpp, POKETYPE_GHOST) )
    {
      int restHP = BPP_GetValue( bpp, BPP_HP );
      if( restHP > 0 )
      {
        u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
        if( targetPokeID != BTL_POKEID_NULL )
        {
          BTL_HANDEX_PARAM_ADD_SICK* param;
          BTL_HANDEX_PARAM_SHIFT_HP* hp_param;

          param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
          param->poke_cnt = 1;
          param->pokeID[0] = targetPokeID;
          param->sickID = WAZASICK_NOROI;
          param->sickCont = BPP_SICKCONT_MakePermanent();
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Noroi );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, targetPokeID );

          hp_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
          hp_param->pokeID[0] = pokeID;
          hp_param->volume[0] = -(BTL_CALC_QuotMaxHP( bpp, 2 ));
          hp_param->poke_cnt = 1;
          hp_param->header.failSkipFlag = TRUE;
        }
      }
      effIndex = BTLV_WAZAEFF_NOROI_GHOST;
    }
    else
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param;
      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_AGILITY_RANK;
      param->rankVolume = -1;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_ATTACK_RANK;
      param->rankVolume = 1;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = BPP_DEFENCE_RANK;
      param->rankVolume = 1;

    }

    BTL_SVFRET_SetWazaEffectIndex( flowWk, effIndex );
  }
}

//----------------------------------------------------------------------------------
/**
 * �ł񂶂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Denjiha( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,   handler_Denjiha_NoEff },    // �������`�F�b�N���x���Q�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Denjiha_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    // �u���߂�v�^�C�v�ɂ͌����Ȃ�
    if( BPP_IsMatchType(target, POKETYPE_JIMEN) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �Ȃ�݂̃^�l
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_NayamiNoTane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_NayamiNoTane_NoEff },    // �������`�F�b�N���x���Q�n���h��
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_NayamiNoTane       },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_NayamiNoTane_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    // �Ȃ܂��C�ӂ݂񎝂��ɂ͌����Ȃ�
    if( (BPP_GetValue(target, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_NAMAKE)
    ||  (BPP_GetValue(target, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_FUMIN)
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

    BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );

    param->pokeID = targetPokeID;
    param->tokuseiID = POKETOKUSEI_FUMIN;
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_TokuseiChange );
    HANDEX_STR_AddArg( &param->exStr, param->pokeID );
    HANDEX_STR_AddArg( &param->exStr, param->tokuseiID );
  }
}

//----------------------------------------------------------------------------------
/**
 * ��߂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Yumekui( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Yumekui },    // �������`�F�b�N���x���Q�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �䂤�킭
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Yuwaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Yuwaku },    // �������`�F�b�N���x���Q�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �g���C�A�^�b�N
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_TriAttack( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_TriAttack },    // �ǉ����ʂɂ���Ԉُ�`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
    u8 rand = BTL_CALC_GetRand( NELEMS(sick_tbl) );

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
static const BtlEventHandlerTable*  ADD_HimituNoTikara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_HimituNoTikara },    // �_���[�W�����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_HimituNoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 rnd = BTL_CALC_GetRand( 100 );

    #ifdef PM_DEBUG
    if( BTL_SVFTOOL_GetDebugFlag(flowWk, BTL_DEBUGFLAG_MUST_TUIKA) ){
      rnd = 0;
    }
    #endif

    if( rnd < 30 )
    {
      enum {
        METHOD_SICK,    ///< ��Ԉُ�
        METHOD_RANK,    ///< �\�̓����N�_�E��
        METHOD_SHRINK,  ///< �Ђ�܂���
      };

      BtlBgAttr  bg = BTL_SVFTOOL_GetLandForm( flowWk );
      const BTL_POKEPARAM* attacker = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      u8   targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
      u16        method, method_arg;

      switch( bg ){
      case BATTLE_BG_ATTR_GROUND:         //�ʏ�n��
      case BATTLE_BG_ATTR_GROUND_S1:        //�l�G����n�ʂP
      case BATTLE_BG_ATTR_GROUND_S2:        //�l�G����n�ʂQ
      case BATTLE_BG_ATTR_PALACE:         //�p���X�ł̑ΐ��p
      case BATTLE_BG_ATTR_SAND:           //���n
        method = METHOD_RANK;
        method_arg = BPP_HIT_RATIO;
        break;

      case BATTLE_BG_ATTR_E_INDOOR:         //����
      default:
        method = METHOD_SICK;
        method_arg = WAZASICK_MAHI;
        break;

      case BATTLE_BG_ATTR_CAVE:           //���A
        method = METHOD_SHRINK;
        method_arg = 0;
        break;

      case BATTLE_BG_ATTR_LAWN:           //�Ő�
      case BATTLE_BG_ATTR_E_GRASS:        //�G���J�E���g��
        method = METHOD_SICK;
        method_arg = WAZASICK_NEMURI;
        break;

      case BATTLE_BG_ATTR_WATER:          //����
      case BATTLE_BG_ATTR_POOL:           //�����܂�
      case BATTLE_BG_ATTR_SHOAL:          //��
        method = METHOD_RANK;
        method_arg = BPP_ATTACK_RANK;
        break;

      case BATTLE_BG_ATTR_MARSH:          //�󂢎���
        method = METHOD_RANK;
        method_arg = BPP_AGILITY_RANK;
        break;

      case BATTLE_BG_ATTR_SNOW:           //�ጴ
      case BATTLE_BG_ATTR_ICE:            //�X��
        method = METHOD_SICK;
        method_arg = WAZASICK_KOORI;
        break;
      }/* switch( bg ) */

      switch( method ){
      case METHOD_SICK:
        {
          BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
          param->poke_cnt = 1;
          param->pokeID[0] = targetPokeID;
          param->sickID = method_arg;
          BTL_CALC_MakeDefaultWazaSickCont( param->sickID, attacker, &param->sickCont );
        }
        break;

      case METHOD_RANK:
        {
          BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

          param->poke_cnt = 1;
          param->pokeID[0] = targetPokeID;
          param->rankType = method_arg;
        }
        break;

      case METHOD_SHRINK:
        {
          BTL_HANDEX_PARAM_ADD_SHRINK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SHRINK, pokeID );

          param->pokeID = targetPokeID;
          param->per = 100;
        }
        break;
      }
    }/* if( rand < 30) */
  }
}

//----------------------------------------------------------------------------------
/**
 * ������ׂ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Osyaberi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_Osyaberi },    // �ǉ����ʂɂ���Ԉُ�`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Osyaberi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetMonsNo(bpp) == MONSNO_PERAPPU )
    {
      PokeSick sick = WAZASICK_KONRAN;
      BPP_SICK_CONT cont;
      const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
      u32 rnd = PP_Get( pp, ID_PARA_personal_rnd, NULL );
      u8 per = (rnd&1)? 10 : 30;

      BTL_CALC_MakeDefaultWazaSickCont( sick, bpp, &cont );

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
static const BtlEventHandlerTable*  ADD_Makituku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK, handler_Makituku },    // �ǉ����ʂɂ���Ԉُ�`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_Uzusio( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,        handler_Makituku           }, // �ǉ����ʂɂ���Ԉُ�`�F�b�N�n���h�� = �܂����ƈꏏ
    { BTL_EVENT_CHECK_POKE_HIDE, handler_Uzusio_CheckHide   }, // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_WAZA_DMG_PROC2,  handler_Uzusio_Dmg         }, // �_���[�W�v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����|�P�q�b�g�`�F�b�N�n���h��
static void handler_Uzusio_CheckHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BPP_CONTFLG_DIVING ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, FALSE );
    }
  }
}
// �_���[�W�v�Z�n���h��
static void handler_Uzusio_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �_�C�r���O���̑���ɂQ�{
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
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
static const BtlEventHandlerTable*  ADD_IkariNoMaeba( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_IkariNoMaeba },    // �_���[�W�v�Z�ŏI�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�v�Z�ŏI�n���h��
static void handler_IkariNoMaeba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    u16 damage = BPP_GetValue(target, BPP_HP) / 2;
    if( damage == 0 ){
      damage = 1;
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, damage );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��イ�̂�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_RyuuNoIkari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_RyuuNoIkari },    // �_���[�W�v�Z�ŏI�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_RyuuNoIkari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, 40 );
  }
}
//----------------------------------------------------------------------------------
/**
 * �\�j�b�N�u�[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_SonicBoom( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_SonicBoom },    // �_���[�W�v�Z�ŏI�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_SonicBoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, 20 );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���ނ����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Gamusyara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,  handler_Gamusyara_CheckNoEffect },
    { BTL_EVENT_WAZA_DMG_PROC1, handler_Gamusyara },    // �_���[�W�v�Z�ŏI�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����`�F�b�N
static void handler_Gamusyara_CheckNoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    const BTL_POKEPARAM* user = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    int diff = BPP_GetValue(target, BPP_HP) - BPP_GetValue(user, BPP_HP);
    if( diff <= 0 ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
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
      diff = 1;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, diff );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����イ�Ȃ��A�i�C�g�w�b�h
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_TikyuuNage( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_TikyuuNage },    // �_���[�W�v�Z�ŏI�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����v�Z�F�����I�ɓ��{�ɂ���i���ʂ̓o�c�O���\���������Ȃ��j
static void handler_TikyuuNage_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 level = BPP_GetValue( bpp, BPP_LEVEL );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, level );
  }
}
// �_���[�W�v�Z�F�Œ�l
static void handler_TikyuuNage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 level = BPP_GetValue( bpp, BPP_LEVEL );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, level );
  }
}
//----------------------------------------------------------------------------------
/**
 * �T�C�R�E�F�[�u
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_PsycoWave( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1, handler_PsycoWave },    // �_���[�W�v�Z�ŏI�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_PsycoWave( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 level = BPP_GetValue( bpp, BPP_LEVEL );
    u16 rand = 50 + BTL_CALC_GetRand( 101 );
    u16 damage = level * rand / 100;
    if( damage == 0 ){
      damage = 1;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, damage );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����킦��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Takuwaeru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_Takuwaeru_CheckExe   },    // ���U�o����������
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Takuwaeru            },    // �����ރ��U����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���ۃ`�F�b�N�i�J�E���^���R���܂��Ă����玸�s����j
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

      counter_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
      counter_param->pokeID = pokeID;
      counter_param->counterID = BPP_COUNTER_TAKUWAERU;
      counter_param->value = takuwae_cnt + 1;
      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Takuwaeru );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, takuwae_cnt+1 );


      rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->poke_cnt = 1;
      rank_param->pokeID[0] = pokeID;
      rank_param->rankType = BPP_DEFENCE_RANK;
      rank_param->rankVolume = 1;
      counter_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
      counter_param->header.failSkipFlag = TRUE;
      counter_param->pokeID = pokeID;
      counter_param->counterID = BPP_COUNTER_TAKUWAERU_DEF;
      counter_param->value = BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU_DEF) + 1;


      rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->poke_cnt = 1;
      rank_param->pokeID[0] = pokeID;
      rank_param->rankType = BPP_SP_DEFENCE_RANK;
      rank_param->rankVolume = 1;
      counter_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
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
static const BtlEventHandlerTable*  ADD_Hakidasu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_Hakidasu_CheckExe   },    // ���U�o����������
    { BTL_EVENT_WAZA_POWER,          handler_Hakidasu_Pow        },    // ���U�З͌���
    { BTL_EVENT_WAZA_EXECUTE_DONE,   handler_Hakidasu_Done       },    // ���U�o���I��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���ۃ`�F�b�N�i�J�E���^���O�������玸�s����j
static void handler_Hakidasu_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_COUNTER_Get(bpp, BPP_COUNTER_TAKUWAERU) == 0 ){
      BTL_Printf("�����킦�ĂȂ����玸�s���܂�\n");
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }else{
      BTL_Printf("�����킦�Ă邩�琬�����܂�\n");
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

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID      = pokeID;
    rank_param->attack      = BPP_GetValue( bpp, BPP_ATTACK_RANK );
    rank_param->defence     = def_rank;
    rank_param->sp_attack   = BPP_GetValue( bpp, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence  = spdef_rank;
    rank_param->agility     = BPP_GetValue( bpp, BPP_AGILITY_RANK );
    rank_param->hit_ratio   = BPP_GetValue( bpp, BPP_HIT_RATIO );
    rank_param->avoid_ratio = BPP_GetValue( bpp, BPP_AVOID_RATIO );

    counter_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
    counter_param->pokeID = pokeID;
    counter_param->counterID = BPP_COUNTER_TAKUWAERU;
    counter_param->value = 0;

    counter_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
    counter_param->pokeID = pokeID;
    counter_param->counterID = BPP_COUNTER_TAKUWAERU_DEF;
    counter_param->value = 0;

    counter_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
    counter_param->pokeID = pokeID;
    counter_param->counterID = BPP_COUNTER_TAKUWAERU_SPDEF;
    counter_param->value = 0;

    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TakuwaeOff );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂̂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Nomikomu( u32* numElems )
{
  // @todo �悱�ǂ肳�ꂽ���A�����킦���[�N����Ȃ̂Ƀn���h�����Ăяo����邱�Ƃ�����
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_Hakidasu_CheckExe   },    // ���U�o����������
    { BTL_EVENT_RECOVER_HP_RATIO,    handler_Nomikomu_Ratio      },    // HP�񕜗�����
    { BTL_EVENT_WAZA_EXECUTE_DONE,   handler_Hakidasu_Done       },    // ���U�o���I��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Nomikomu_Ratio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 cnt = BPP_COUNTER_Get( bpp, BPP_COUNTER_TAKUWAERU );
    fx32 ratio;
    switch( cnt ){
    case 1:
    default:
      ratio = FX32_CONST( 0.25 );
      break;
    case 2:   ratio = FX32_CONST( 0.5 ); break;
    case 3:   ratio = FX32_CONST( 1 ); break;
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, ratio );
  }
}

//----------------------------------------------------------------------------------
/**
 * �J�E���^�[
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Counter( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_Counter_CheckExe   },    // ���U�o����������
    { BTL_EVENT_DECIDE_TARGET,           handler_Counter_Target     },    // �^�[�Q�b�g����
    { BTL_EVENT_WAZA_DMG_PROC1,          handler_Counter_CalcDamage },    // �_���[�W�v�Z�ŏI�n���h��

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
  common_Counter_CalcDamage( flowWk, pokeID, work, WAZADATA_DMG_PHYSIC, FX32_CONST(2) );
}
//----------------------------------------------------------------------------------
/**
 * �~���[�R�[�g
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MilerCoat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_MilerCoat_CheckExe   },    // ���U�o����������
    { BTL_EVENT_DECIDE_TARGET,       handler_MilerCoat_Target     },    // �^�[�Q�b�g����
    { BTL_EVENT_WAZA_DMG_PROC1,      handler_MilerCoat_CalcDamage },    // �_���[�W�v�Z�ŏI�n���h��

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
  common_Counter_CalcDamage( flowWk, pokeID, work, WAZADATA_DMG_SPECIAL, FX32_CONST(2) );
}
//----------------------------------------------------------------------------------
/**
 * ���^���o�[�X�g
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MetalBurst( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_MetalBurst_CheckExe   },    // ���U�o����������
    { BTL_EVENT_DECIDE_TARGET,       handler_MetalBurst_Target     },    // �^�[�Q�b�g����
    { BTL_EVENT_WAZA_DMG_PROC1,      handler_MetalBurst_CalcDamage },    // �_���[�W�v�Z�ŏI�n���h��

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
  common_Counter_CalcDamage( flowWk, pokeID, work, WAZADATA_DMG_NONE, FX32_CONST(1.5f) );
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BPP_WAZADMG_REC  rec;

    // �w��^�C�v�_���[�W���󂯂Ă��Ȃ��Ȃ玸�s
    if( !common_Counter_GetRec(bpp, dmgType, &rec) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
    else
    {
      u8 targetPokeID = rec.pokeID;
      // �Ώۂ����ɏ�ɂ��Ȃ������̏�ɑ��̃|�P�����Ȃ��Ȃ玸�s
      if( BTL_SVFTOOL_GetExistFrontPokePos(flowWk, rec.pokeID) == BTL_POS_MAX )
      {
        if( (rec.pokePos == BTL_POS_MAX)
        ||  (BTL_SVFTOOL_GetExistPokeID(flowWk, rec.pokePos) == BTL_POKEID_NULL)
        ){
          BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
        }
      }
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
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BPP_WAZADMG_REC  rec;

    if( common_Counter_GetRec(bpp, dmgType, &rec) )
    {
      u8 targetPokeID = rec.pokeID;

      // �Ώۂ����ɏ�ɂ��Ȃ��ꍇ�i�Ƃ�ڂ�����Ȃǁj�A���̈ʒu�Ɍ��݂���|�P��Ώۂɂ���
      if( BTL_SVFTOOL_GetExistFrontPokePos(flowWk, rec.pokeID) == BTL_POS_NULL )
      {
        if( rec.pokePos != BTL_POS_MAX ){
          targetPokeID = BTL_SVFTOOL_GetExistPokeID( flowWk, rec.pokePos );
        }else{
          targetPokeID = BTL_POKEID_NULL;
        }
      }
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, targetPokeID );
    }
  }
}
//----------------------------------------------------------------------------------
//
// �J�E���^�[�n���ʁF�_���[�W�v�Z
//
//----------------------------------------------------------------------------------
static void common_Counter_CalcDamage( BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType, fx32 ratio )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BPP_WAZADMG_REC  rec;

    if( common_Counter_GetRec(bpp, dmgType, &rec) )
    {
      u32 damage = BTL_CALC_MulRatio( rec.damage, ratio );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, damage );
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
static const BtlEventHandlerTable*  ADD_Totteoki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND, handler_Totteoki },    // ���U�o�����ۃ`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_Totteoki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 wazaCnt = BPP_WAZA_GetCount( bpp );
    u8 usedCnt, i;
    for(usedCnt=0, i=0; i<wazaCnt; ++i)
    {
      if( BPP_WAZA_GetID(bpp, i) == WAZANO_TOTTEOKI ){
        continue;
      }
      if( BPP_WAZA_CheckUsedInAlive(bpp,i) ){
        ++usedCnt;
      }
    }
    if( (wazaCnt < 2)
    ||  (usedCnt < (wazaCnt-1))
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
static const BtlEventHandlerTable*  ADD_Ibiki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_1ST, handler_Ibiki_CheckFail_1 },  // ���U�o�����ۃ`�F�b�N�n���h���P
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND, handler_Ibiki_CheckFail_2 },  // ���U�o�����ۃ`�F�b�N�n���h���Q
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�o�����ۃ`�F�b�N�n���h���P
static void handler_Ibiki_CheckFail_1( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    SV_WazaFailCause cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );

    if( cause == SV_WAZAFAIL_NEMURI ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
    }

    /*
    �u�˂ނ�v��ԂłȂ��̂��R�R�Ŏ��s�����ɂ����Ⴄ�Ƃ܂����B
    �i���U���b�Z�[�W���\�������܂��Ɂu���܂����܂�Ȃ������v���\������Ă��܂��j
    else if( cause == SV_WAZAFAIL_NULL )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( !BPP_CheckSick(bpp, POKESICK_NEMURI) ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
      }
    }
    */
  }
}
// ���U�o�����ۃ`�F�b�N�n���h���Q
static void handler_Ibiki_CheckFail_2( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
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
 * ������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Akumu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Akumu_NoEff },    // ���U�����`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Akumu_NoEff( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF);
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(bpp, POKESICK_NEMURI) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂ�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Fuiuti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Fuiuti_NoEff },    // ���U�����`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      // ���肪���ɍs�����Ă����玸�s
      if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_WAZAPROC_DONE) ){ break; }

      // ���^�[���̃A�N�V�������擾�ł��Ȃ������玸�s
      if( !BTL_SVFTOOL_GetThisTurnAction( flowWk, targetPokeID, &action ) ){ break; }

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
static const BtlEventHandlerTable*  ADD_Oiuti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MENBERCHANGE_INTR, handler_Oiuti_Intr },    // ����ւ����荞�݃n���h��
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Oiuti_Dmg  },    // �_���[�W�v�Z�ŏI�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Oiuti_Intr( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_ACTION_PARAM  action;

  if( BTL_SVFTOOL_GetThisTurnAction(flowWk, pokeID, &action) )
  {
    BOOL fIntr = FALSE;

    // �V���O����̏ꍇ�A�^�[�Q�b�g�ʒu��NULL�ɂȂ��Ă���̂ŕK���L���ɂ��Ă��܂�
    if( BTL_SVFTOOL_GetRule(flowWk) == BTL_RULE_SINGLE ){
      fIntr = TRUE;
    }else{
      u8 targetPokeID = BTL_SVFTOOL_PokePosToPokeID( flowWk, action.fight.targetPos );
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) == targetPokeID ){
        fIntr = TRUE;
      }
    }
    if( fIntr )
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
    if( BTL_SVFTOOL_GetThisTurnAction(flowWk, targetPokeID, &action) )
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
static const BtlEventHandlerTable*  ADD_Daibakuhatsu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_DETERMINE,   handler_Daibakuhatsu_DmgDetermine },  // �_���[�W�m��n���h��
    { BTL_EVENT_WAZA_EXECUTE_DONE,    handler_Daibakuhatsu_ExeFix       },  // ���U�����I���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�m��n���h��
static void handler_Daibakuhatsu_DmgDetermine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    param->pokeID = pokeID;
  }
}
// ���U�����I���n���h��
static void handler_Daibakuhatsu_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    param->pokeID = pokeID;
  }
}

//----------------------------------------------------------------------------------
/**
 * ����������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Kiaidame( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Kiaidame },         // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Kiaidame( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_KIAIDAME) )
    {
      BTL_HANDEX_PARAM_SET_CONTFLAG* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      param = (BTL_HANDEX_PARAM_SET_CONTFLAG*)BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
      param->pokeID = pokeID;
      param->flag = BPP_CONTFLG_KIAIDAME;

      msg_param = (BTL_HANDEX_PARAM_MESSAGE*)BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KiaiDame );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * ���イ�ł�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Juden( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,       handler_Juden_WazaStart }, // ���U�����J�n�n���h��
    { BTL_EVENT_WAZA_EXE_START,      handler_Juden_Exe       }, // ���U�o���m��n���h��
    { BTL_EVENT_WAZA_POWER,          handler_Juden_Pow       }, // ���U�З͌v�Z�n���h��
    { BTL_EVENT_WAZASEQ_END,         handler_Juden_WazaEnd   }, // ���U�����I���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�o���m��n���h��
static void handler_Juden_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) == BTL_EVENT_FACTOR_GetSubID(myHandle) ){
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Juuden );
      HANDEX_STR_AddArg( &param->str, pokeID );

      work[ WORKIDX_STICK ] = 1;
      work[0] = 1;  // ���イ�ł�V�[�P���X���P�ɂ���i�\��t���J�n�j
    }
  }
}
// ���U�З͌v�Z�n���h��
static void handler_Juden_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 2 )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_DENKI ){
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
      }
      BTL_EVENT_FACTOR_Remove( myHandle );
    }
  }
}
// ���U�����J�n�n���h��
static void handler_Juden_WazaStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �u���イ�ł�v�ȊO�̃��U���o���Ȃ�A�V�[�P���X���Q�ɂ���
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) != BTL_EVENT_FACTOR_GetSubID(myHandle))
  ){
     work[0] = 2;
  }
}
// ���U�����I���n���h��
static void handler_Juden_WazaEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[0] == 2)
  ){
    // �V�[�P���X=2 �Ń��U�����I��������\��t������
    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}

//----------------------------------------------------------------------------------
/**
 * �ق�т̂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_HorobiNoUta( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START, handler_HorobiNoUta_Exe },    // ���U�o���m��n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_HorobiNoUta_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_HorobiNoUta );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��ǂ肬�̂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_YadorigiNoTane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM, handler_YadorigiNoTane_Param },    // ��Ԉُ�p�����[�^�����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ��Ԉُ�p�����[�^�����n���h��
static void handler_YadorigiNoTane_Param( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BPP_SICK_CONT cont;
    BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokePos( flowWk, pokeID );
    cont = BPP_SICKCONT_MakePermanentParam( myPos );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂�Ԃ�E�����킯��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Miyaburu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM, handler_Miyaburu },    // ��Ԉُ�p�����[�^�����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ��Ԉُ�p�����[�^�����n���h��
static void handler_Miyaburu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BPP_SICK_CONT cont;
    cont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
    BPP_SICKCONT_AddParam( &cont, POKETYPE_GHOST );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}

//----------------------------------------------------------------------------------
/**
 * �l�R�ɂ��΂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_NekoNiKoban( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZADMG_SIDE_AFTER, handler_NekoNiKoban },    // �_���[�W�����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_NekoNiKoban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 volume = BPP_GetValue( bpp, BPP_LEVEL ) * 20;
    if( BTL_SVFRET_AddBonusMoney(flowWk, volume, pokeID) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_NekoNiKoban );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Ikari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,      handler_Ikari_Exe },    // ���U�o���m��n���h��
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_Ikari_React },  // �_���[�W�����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

      param->rankType = WAZA_RANKEFF_ATTACK;
      param->rankVolume = 1;
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fStdMsgDisable = TRUE;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Ikari );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
    BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
  }
}
//----------------------------------------------------------------------------------
/**
 * �A�N�A�����O
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_AquaRing( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_AquaRing },            // �����ރ��U�n���h��
//    { BTL_EVENT_TURNCHECK_BEGIN,   handler_AquaRing_turnCheck },  // �^�[���`�F�b�N�J�n�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����ރ��U�n���h��
static void handler_AquaRing( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_CheckSick(bpp, WAZASICK_AQUARING) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

      param->sickID = WAZASICK_AQUARING;
      param->sickCont = BPP_SICKCONT_MakePermanent();
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_AquaRing );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
  }
}
// �^�[���`�F�b�N�J�n�n���h��
static void handler_AquaRing_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WORKIDX_STICK] != 0 )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( !BPP_IsHPFull(bpp)
      &&  !BPP_IsDead(bpp)
      ){
        BTL_HANDEX_PARAM_RECOVER_HP* param;
        const BTL_POKEPARAM* bpp;

        bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
        param->pokeID = pokeID;
        param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 16 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_AquaRingRecover );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ���΂��E�͂Ȃт�̂܂��E�������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Abareru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,     handler_Abareru },            // ���U�o���m��n���h��
    { BTL_EVENT_TURNCHECK_END,      handler_Abareru_turnCheck },  // �^�[���`�F�b�N�I���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_Abareru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���������U���b�N��ԂɂȂ��Ă��Ȃ��Ȃ�A���U���b�N��Ԃɂ���
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (!BPP_CheckSick(bpp, WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      u8 turns;

      turns = 2 + BTL_CALC_GetRand( 2 );

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK;
      param->sickCont = BPP_SICKCONT_MakeTurnParam( turns, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      work[ WORKIDX_STICK ] = 1;
    }
  }
}
static void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WORKIDX_STICK] != 0)
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( !BPP_CheckSick(bpp, WAZASICK_WAZALOCK)
      &&  !BPP_CheckSick(bpp, WAZASICK_KONRAN)
      ){
        BTL_HANDEX_PARAM_ADD_SICK* param;

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        param->sickID = WAZASICK_KONRAN;
        BTL_CALC_MakeDefaultWazaSickCont( param->sickID, bpp, &param->sickCont );
        param->fAlmost = FALSE;
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->poke_cnt = 1;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_KonranAbare );
        HANDEX_STR_AddArg( &param->exStr, pokeID );

        BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���킮
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Sawagu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,       handler_Sawagu },               // ���U�o���m��n���h��
    { BTL_EVENT_TURNCHECK_END,        handler_Sawagu_turnCheck },     // �^�[���`�F�b�N�I���n���h��
    { BTL_EVENT_ADDSICK_CHECKFAIL,    handler_Sawagu_CheckSickFail }, // ��Ԉُ편�s�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_Sawagu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���������U���b�N��ԂɂȂ��Ă��Ȃ��Ȃ�A���U���b�N��Ԃɂ���
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (!BPP_CheckSick(bpp, WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      u8 turns;

      turns = BTL_CALC_RandRange( 2, 5 );
      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK;
      param->sickCont = BPP_SICKCONT_MakeTurnParam( turns, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Sawagu );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );

      {
        BTL_HANDEX_PARAM_CURE_SICK* cure_param;
        BtlPokePos myPos = BTL_SVFTOOL_GetExistFrontPokePos( flowWk, pokeID );
        BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_AREA_ALL, myPos );

        HANDWORK_POKEID* idwk = BTL_SVFTOOL_GetTmpWork( flowWk, sizeof(HANDWORK_POKEID) );
        u32 i;

        idwk->pokeCnt = BTL_SVFTOOL_ExpandPokeID( flowWk, expos, idwk->pokeID );
        for(i=0; i<idwk->pokeCnt; ++i)
        {
          cure_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
          cure_param->sickCode = WAZASICK_NEMURI;
          cure_param->poke_cnt = 1;
          cure_param->pokeID[0] = idwk->pokeID[i];
          HANDEX_STR_Setup( &cure_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_SawaguWake );
          HANDEX_STR_AddArg( &cure_param->exStr, idwk->pokeID[i] );
        }
      }

      work[ WORKIDX_STICK ] = 1;
    }
  }
}
static void handler_Sawagu_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[WORKIDX_STICK] )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( !BPP_CheckSick(bpp, WAZASICK_WAZALOCK)
      &&  !BPP_IsDead(bpp)
      ){
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SawaguCure );
        HANDEX_STR_AddArg( &param->str, pokeID );

        BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      }
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
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
static const BtlEventHandlerTable*  ADD_Korogaru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,          handler_Korogaru_ExeFix    },  // ���U�o���m��n���h��
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Korogaru_NoEffect  },  // ���U�o�����s�m��n���h��
    { BTL_EVENT_WAZA_AVOID,              handler_Korugaru_Avoid     },  // ���U�͂��ꂽ�n���h��
    { BTL_EVENT_WAZA_EXECUTE_NO_EFFECT,  handler_Korogaru_NoEffect  },   // ���U�����n���h��
    { BTL_EVENT_WAZA_POWER,              handler_Korogaru_Pow       },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Korogaru_ExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  enum {
    KOROGARU_TURNS = 5,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���������U���b�N��ԂɂȂ��Ă��Ȃ��Ȃ�A���U���b�N��Ԃɂ���
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (!BPP_CheckSick(bpp, WAZASICK_WAZALOCK))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK;
      param->sickCont = BPP_SICKCONT_MakeTurnParam( KOROGARU_TURNS, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      work[0] = 0;
      work[1] = KOROGARU_TURNS;
      work[ WORKIDX_STICK ] = 1;
    }
  }
}
// ���U�͂��ꂽ
static void handler_Korugaru_Avoid( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    common_Korogaru_Unlock( myHandle, flowWk, pokeID, work );
  }
}
// ���U�����������ʂȂ�
static void handler_Korogaru_NoEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    common_Korogaru_Unlock( myHandle, flowWk, pokeID, work );
  }
}
/**
 *  ���낪�郍�b�N�������\��t����������
 */
static void  common_Korogaru_Unlock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );

  param->pokeID[0] = pokeID;
  param->poke_cnt = 1;
  param->sickCode = WAZASICK_WAZALOCK;

  work[ WORKIDX_STICK ] = 0;
  removeHandlerForce( pokeID, BTL_EVENT_FACTOR_GetSubID(myHandle) );
}

// �З͌���
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
    if( work[0] >= work[1] ){
      removeHandlerForce( pokeID, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �g���v���L�b�N
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_TripleKick( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,      handler_TripleKick },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_WAZA_HIT_COUNT,  handler_TripleKick_HitCount }
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_TripleKick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    work[0]++;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, work[0]*10 );
  }
}
static void handler_TripleKick_HitCount( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, TRUE );
  }
}


//----------------------------------------------------------------------------------
/**
 * �W���C���{�[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_GyroBall( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_GyroBall },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_GyroBall( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bppSelf = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* bppTarget = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    u16 selfAgi = BTL_SVFTOOL_CalcAgility( flowWk, bppSelf, FALSE );
    u16 targetAgi = BTL_SVFTOOL_CalcAgility( flowWk, bppTarget, FALSE );

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
static const BtlEventHandlerTable*  ADD_Revenge( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Revenge },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Revenge( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
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
static const BtlEventHandlerTable*  ADD_Jitabata( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Jitabata },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      if( r <= tbl[i].ratio ){ break; }
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, tbl[i].pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���炰��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Karagenki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Karagenki },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �����؂�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Sippegaesi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Sippegaesi },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Sippegaesi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_WAZAPROC_DONE) )
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
static const BtlEventHandlerTable*  ADD_Funka( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Funka },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Funka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    fx32 r = BPP_GetHPRatio( bpp );
    r /= 100;
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, r );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���ڂ�Ƃ�E�ɂ���Ԃ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Siboritoru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Siboritoru },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Siboritoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    fx32 r = BPP_GetHPRatio( bpp );
    u32 pow = BTL_CALC_MulRatio_OverZero( 120, r ) / 100;
    if( pow == 0 ){
      pow = 1;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����݂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Siomizu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Siomizu },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * ���񂪂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Ongaesi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Ongaesi },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Yatuatari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Yatuatari },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �߂��܂��r���^
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MezamasiBinta( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,          handler_MezamasiBinta             }, // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_DAMAGEPROC_END_INFO, handler_MezamasiBinta_AfterDamage }, // �_���[�W�v���Z�X�I���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�З̓`�F�b�N�n���h��
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
// �_���[�W�v���Z�X�I���n���h��
static void handler_MezamasiBinta_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
    if( BPP_CheckSick(bpp, WAZASICK_NEMURI) ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
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
static const BtlEventHandlerTable*  ADD_Kituke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,          handler_Kituke             },  // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_DAMAGEPROC_END_INFO, handler_Kituke_AfterDamage },  // �_���[�W�v���Z�X�I���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�З̓`�F�b�N�n���h��
static void handler_Kituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �}�q��Ԃ̑���ɈЗ͂Q�{
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_CheckSick(bpp, WAZASICK_MAHI) ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
// �_���[�W�v���Z�X�I���n���h��
static void handler_Kituke_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    if( target_pokeID != BTL_POKEID_NULL )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
      if( BPP_CheckSick(bpp, WAZASICK_MAHI) ){
        BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        param->pokeID[0] = target_pokeID;
        param->poke_cnt = 1;
        param->sickCode = WAZASICK_MAHI;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �v���[���g
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Present( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DMG_TO_RECOVER_CHECK,  handler_Present_Check }, // �_���[�W���U�񕜉��`�F�b�N�n���h��
    { BTL_EVENT_DMG_TO_RECOVER_FIX,    handler_Present_Fix },   // �_���[�W���U�񕜉��m��n���h��
    { BTL_EVENT_WAZA_POWER,            handler_Present_Pow },   // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W���U�񕜉��`�F�b�N�n���h��
static void handler_Present_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 20���̊m���ŉ񕜉�
    if( BTL_CALC_IsOccurPer(20) )
    {
      work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }

    {
      u8 effIndex = (work[0])? BTLV_WAZAEFF_PRESENT_RECOVER : BTLV_WAZAEFF_PRESENT_DAMAGE;
      BTL_SVFRET_SetWazaEffectIndex( flowWk, effIndex );
    }
  }
}
// �_���[�W���U�񕜉��m��n���h��
static void handler_Present_Fix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������񕜉������ꍇ�A�񕜗ʂ��v�Z
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[0])
  ){
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    if( !BPP_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP*  param = BTL_SVF_HANDEX_Push( flowWk ,BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = target_pokeID;
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 4 );

      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    }
    else
    {
      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NoEffect );
      HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
    }
  }
}
// ���U�З̓`�F�b�N�n���h��
static void handler_Present_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 rand = BTL_CALC_GetRand( 80 );
    u32 pow = 0;

    if( rand < 40 ){
      pow = 40;
    }else if( rand < 70 ){
      pow = 80;
    }else{
      pow = 120;
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ����ӂ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Kirifuda( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Kirifuda },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Kirifuda( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u16 powTbl[] = {
    0, 200, 80, 60, 50, 40,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 pow;
    u8 wazaIdx;

    wazaIdx = BPP_WAZA_SearchIdx( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    if( wazaIdx != PTL_WAZA_MAX ){
      u8 pp = BPP_WAZA_GetPP( bpp, wazaIdx );
      if( pp >= NELEMS(powTbl) ){
        pp = NELEMS(powTbl) - 1;
      }
      pow = powTbl[ pp ];
    }else{
      GF_ASSERT(0);
      pow = powTbl[ (NELEMS(powTbl)-1) ];
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Osioki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Osioki },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Osioki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const BppValueID  statusTbl[] = {
    BPP_ATTACK_RANK, BPP_DEFENCE_RANK, BPP_SP_ATTACK_RANK, BPP_SP_DEFENCE_RANK, BPP_AGILITY_RANK,
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
      if( rank > BPP_RANK_STATUS_DEFAULT ){
        rankSum += (rank - BPP_RANK_STATUS_DEFAULT);
      }
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
static const BtlEventHandlerTable*  ADD_RenzokuGiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_RenzokuGiri },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_RenzokuGiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  enum {
    POW_MAX = 160,    // ���̃��U�̈З͍ő�l
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetPrevWazaID(bpp) == BTL_EVENT_FACTOR_GetSubID(myHandle) )
    {
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      u32 cnt = BPP_GetWazaContCounter( bpp );
      u32 i;

      for(i=0; i<cnt; ++i)
      {
        pow *= 2;
        if( pow > POW_MAX ){
          pow = POW_MAX;
          break;
        }
      }
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �_������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Dameosi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Dameosi },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * ��������E�����ނ���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Ketaguri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Ketaguri },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �E�F�U�[�{�[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_WeatherBall( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM, handler_WeatherBall_Type },  // ���U�p�����[�^�`�F�b�N�n���h��
    { BTL_EVENT_WAZA_POWER, handler_WeatherBall_Pow },   // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_WeatherBall_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlWeather weather = BTL_SVFTOOL_GetWeather(flowWk);
    PokeType  type  = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    u8  effIndex = BTLV_WAZAEFF_WEATHERBALL_NORMAL;

    switch( weather ){
    case BTL_WEATHER_SHINE:
      type = POKETYPE_HONOO;
      effIndex = BTLV_WAZAEFF_WEATHERBALL_SHINE;
      break;
    case BTL_WEATHER_RAIN:
      type = POKETYPE_MIZU;
      effIndex = BTLV_WAZAEFF_WEATHERBALL_RAIN;
      break;
    case BTL_WEATHER_SAND:
      type = POKETYPE_IWA;
      effIndex = BTLV_WAZAEFF_WEATHERBALL_SAND;
      break;
    case BTL_WEATHER_SNOW:
      type = POKETYPE_KOORI;
      effIndex = BTLV_WAZAEFF_WEATHERBALL_SNOW;
      break;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
    BTL_SVFRET_SetWazaEffectIndex( flowWk, effIndex );
  }
}
static void handler_WeatherBall_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) != POKETYPE_NORMAL)
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���܂��E����������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Tatumaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Tatumaki_checkHide },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Tatumaki },     // �_���[�W�ŏI�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �X�J�C�A�b�p�[
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_SkyUpper( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_SkyUpper },  // �����|�P�q�b�g�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * ���݂Ȃ�E�ڂ��ӂ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Kaminari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Kaminari_checkHide     },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_EXCUSE_CALC_HIT,   handler_Kaminari_excuseHitCalc },  // �q�b�g�m���v�Z�X�L�b�v
    { BTL_EVENT_WAZA_HIT_RATIO,    handler_Kaminari_hitRatio      },  // �������v�Z
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����|�P�q�b�g�`�F�b�N�n���h��
static void handler_Kaminari_checkHide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKE_HIDE) == BPP_CONTFLG_SORAWOTOBU ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_AVOID_FLAG, FALSE );
    }
  }
}
// �q�b�g�m���v�Z�X�L�b�v�n���h��
static void handler_Kaminari_excuseHitCalc( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_RAIN ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }
  }
}
// �������v�Z�n���h��
static void handler_Kaminari_hitRatio( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SHINE ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_HIT_PER, 50 );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂԂ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Fubuki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_EXCUSE_CALC_HIT,   handler_Fubuki },      // �q�b�g�m���v�Z�X�L�b�v�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �q�b�g�m���v�Z�X�L�b�v�n���h��
static void handler_Fubuki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SNOW ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Jisin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Jisin_checkHide },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Jisin_damage },     // �_���[�W�ŏI�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
    // ���Ȃ��ق��Ԃ̑���ɂQ�{
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
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
static const BtlEventHandlerTable*  ADD_SabakiNoTubute( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_SabakiNoTubute },  // �킴�p�����[�^�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SabakiNoTubute( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
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
 * �e�N�m�o�X�^�[
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_TechnoBaster( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_TechnoBaster },  // �킴�p�����[�^�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_TechnoBaster( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 item = BPP_GetItem( bpp );
    u8 type = POKETYPE_NORMAL;
    u8 effIndex = BTLV_WAZAEFF_TECKNOBASTER_NORMAL;

    switch( item ){
    case ITEM_AKUAKASETTO:    // �A�N�A�J�Z�b�g
      type = POKETYPE_MIZU;
      effIndex = BTLV_WAZAEFF_TECKNOBASTER_MIZU;
      break;
    case ITEM_INAZUMAKASETTO: // �C�i�Y�}�J�Z�b�g
      type = POKETYPE_DENKI;
      effIndex = BTLV_WAZAEFF_TECKNOBASTER_DENKI;
      break;
    case ITEM_BUREIZUKASETTO: // �u���C�Y�J�Z�b�g
      type = POKETYPE_HONOO;
      effIndex = BTLV_WAZAEFF_TECKNOBASTER_HONOO;
      break;
    case ITEM_HURIIZUKASETTO: // �t���[�Y�J�Z�b�g
      type = POKETYPE_KOORI;
      effIndex = BTLV_WAZAEFF_TECKNOBASTER_KOORI;
      break;
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
    BTL_SVFRET_SetWazaEffectIndex( flowWk, effIndex );
  }
}

//----------------------------------------------------------------------------------
/**
 * �߂��߂�p���[
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MezameruPower( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,   handler_MezameruPower_Type },  // �킴�p�����[�^�`�F�b�N
    { BTL_EVENT_WAZA_POWER,   handler_MezameruPower_Pow  },  // �킴�З̓`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MezameruPower_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
    PokeType type = POKETOOL_GetMezaPa_Type( pp );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}
static void handler_MezameruPower_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
    u32 pow = POKETOOL_GetMezaPa_Power( pp );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }

}
//----------------------------------------------------------------------------------
/**
 * ������̂߂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_SizenNoMegumi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND, handler_SizenNoMegumi_ExeCheck    }, // �킴�o�����ۃ`�F�b�N
    { BTL_EVENT_WAZA_PARAM,             handler_SizenNoMegumi_Type        }, // �킴�p�����[�^�`�F�b�N
    { BTL_EVENT_WAZA_POWER,             handler_SizenNoMegumi_Pow         }, // �킴�З̓`�F�b�N
    { BTL_EVENT_DAMAGEPROC_END,         handler_SizenNoMegumi_AfterDamage }, // ���U�_���[�W�V�[�P���X�I��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �킴�o�����ۃ`�F�b�N
static void handler_SizenNoMegumi_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 item = BPP_GetItem( bpp );
    int prm = BTL_CALC_ITEM_GetParam(item, ITEM_PRM_SIZENNOMEGUMI_ATC);
    // �����A�C�e���̂�����̂߂��݃p�����[�^�������l�i0�j�Ȃ玸�s����
    if( (item == ITEM_DUMMY_DATA)
    ||  (BTL_CALC_ITEM_GetParam(item, ITEM_PRM_SIZENNOMEGUMI_ATC) == 0)
    ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
// �킴�p�����[�^�`�F�b�N
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
// �킴�З̓`�F�b�N
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
// ���U�_���[�W�V�[�P���X�I��
static void handler_SizenNoMegumi_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
    param->pokeID = pokeID;
    param->itemID = ITEM_DUMMY_DATA;
  }
}

//----------------------------------------------------------------------------------
/**
 * �͂������Ƃ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Hatakiotosu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_INFO,      handler_Hatakiotosu },  // �_���[�W�v���Z�X�I���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�v���Z�X�I���n���h��
static void handler_Hatakiotosu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    if( target_pokeID != BTL_POKEID_NULL )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
      u16 itemID = BPP_GetItem( bpp );
      if( itemID != ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
        param->pokeID = target_pokeID;
        param->itemID = ITEM_DUMMY_DATA;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Hatakiotosu );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, target_pokeID );
        HANDEX_STR_AddArg( &param->exStr, itemID );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �}�W�b�N�R�[�g
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MagicCoat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_MagicCoat           }, // �����ރ��U����
    { BTL_EVENT_CHECK_WAZA_ROB,               handler_MagicCoat_CheckRob  }, // ���U�����蔻��
    { BTL_EVENT_WAZASEQ_REFRECT,              handler_MagicCoat_Reflect   }, // ���U������m��
    { BTL_EVENT_TURNCHECK_BEGIN,              handler_MagicCoat_TurnCheck }, // �^�[���`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MagicCoat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MagicCoatReady );
    HANDEX_STR_AddArg( &param->str, pokeID );
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_MagicCoat_CheckRob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  if( atkPokeID != pokeID )
  {
    u8 targetCnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TARGET_POKECNT );
    u8 i;
    for( i=0; i<targetCnt; ++i)
    {
      // �������^�[�Q�b�g���}�W�b�N�R�[�g�Ώۃ��U�������璵�˕Ԃ�
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1+i) == pokeID )
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
            // ���˕Ԃ��t���O
            work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
            break;
          }
        }
      }
    }
  }
}
static void handler_MagicCoat_Reflect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0])
  ){
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MagicCoatExe );
    HANDEX_STR_AddArg( &param->str, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) );
    HANDEX_STR_AddArg( &param->str, BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
    work[0] = 0;
  }
}
static void handler_MagicCoat_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // �^�[���`�F�b�N�Ŏ��E
    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}
//----------------------------------------------------------------------------------
/**
 * �悱�ǂ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Yokodori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,     handler_Yokodori }, // �����ރ��U����
    { BTL_EVENT_CHECK_WAZA_ROB,        handler_Yokodori_CheckRob },  // �����蔻��
    { BTL_EVENT_WAZASEQ_ROB,           handler_Yokodori_Rob      },  // ������m��
    { BTL_EVENT_TURNCHECK_BEGIN,       handler_Yokodori_TurnCheck }, // �^�[���`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Yokodori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_YokodoriReady );
    HANDEX_STR_AddArg( &param->str, pokeID );
    BTL_Printf("�����\��t���܂���\n");
    work[ WORKIDX_STICK ] = 1;
  }
}
static void handler_Yokodori_CheckRob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  BTL_Printf("�悱�ǂ�`�F�b�N atkPoke=%d, selfPokeID=%d\n", atkPokeID, pokeID);
  if( atkPokeID != pokeID )
  {
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );

    if( WAZADATA_GetFlag(waza, WAZAFLAG_Yokodori) )
    {
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_POKEID, pokeID) )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEID_DEF, pokeID );
        work[ 0 ] = 1;
      }
    }
  }
}
static void handler_Yokodori_Rob( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0])
  ){
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_YokodoriExe );
    HANDEX_STR_AddArg( &param->str, pokeID );
    HANDEX_STR_AddArg( &param->str, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) );
  }
}
static void handler_Yokodori_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}
//----------------------------------------------------------------------------------
/**
 * �ǂ�ڂ��E�ق�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Dorobou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_INFO,      handler_Dorobou }, // �_���[�W�v���Z�X�I���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�v���Z�X�I���n���h��
static void handler_Dorobou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetItem(self) == ITEM_DUMMY_DATA )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      if( target_pokeID != BTL_POKEID_NULL )
      {
        const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
        if( BPP_GetItem(target) != ITEM_DUMMY_DATA )
        {
          BTL_HANDEX_PARAM_SWAP_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SWAP_ITEM, pokeID );
          BTL_HANDEX_PARAM_SET_EFFECT_IDX* effParam = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_EFFECT_IDX, pokeID );

          param->pokeID = target_pokeID;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Dorobou );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, target_pokeID );
          HANDEX_STR_AddArg( &param->exStr, BPP_GetItem(target) );

          effParam->header.failSkipFlag = TRUE;
          effParam->effIndex = BTLV_WAZAEFF_DOROBOU_STEAL;
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �g���b�N�E���肩��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Trick( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,      handler_Trick }, // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Trick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* self = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    u16 selfItemID = BPP_GetItem( self );
    u16 targetItemID = BPP_GetItem( target );

    if( (selfItemID != ITEM_DUMMY_DATA) || (targetItemID != ITEM_DUMMY_DATA) )
    {
      if( !ITEM_CheckMail(selfItemID) && !ITEM_CheckMail(targetItemID) )
      {
        BTL_HANDEX_PARAM_SWAP_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SWAP_ITEM, pokeID );
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        param->pokeID = target_pokeID;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Trick );
        HANDEX_STR_AddArg( &param->exStr, pokeID );

        if( selfItemID != ITEM_DUMMY_DATA )
        {
          msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
          msg_param->header.failSkipFlag = TRUE;
          HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TrickGetItem );
          HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
          HANDEX_STR_AddArg( &msg_param->str, selfItemID );
        }
        if( targetItemID != ITEM_DUMMY_DATA )
        {
          msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
          msg_param->header.failSkipFlag = TRUE;
          HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_TrickGetItem );
          HANDEX_STR_AddArg( &msg_param->str, pokeID );
          HANDEX_STR_AddArg( &msg_param->str, targetItemID );
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �}�O�j�`���[�h
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Magnitude( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,     handler_Jisin_checkHide  },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_WAZA_DMG_PROC2,      handler_Jisin_damage     },  // �_���[�W�ŏI�`�F�b�N
    { BTL_EVENT_WAZA_EXE_START,      handler_Magnitude_effect },  // ���U�o���m��
    { BTL_EVENT_WAZA_POWER,          handler_Magnitude_pow    },  // ���U�З͌v�Z
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_Magnitude_effect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
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

    u8 i, per = BTL_CALC_GetRand( 100 );

    for(i=0; i<NELEMS(powTbl); ++i)
    {
      if( per < powTbl[i].per ){
        break;
      }
    }

    // work[0] �ɈЗ͒l�����Ă���
    work[0] = powTbl[i].pow;

    {
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Magnitude1 + i );
    }
  }
}
static void handler_Magnitude_pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �L�蓾�Ȃ��n�Y�����˂�̂���
    if( work[0] == 0 ){
      work[0] = 10;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, work[0] );
  }
}

//----------------------------------------------------------------------------------
/**
 * �Ȃ݂̂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Naminori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_POKE_HIDE,   handler_Naminori_checkHide },  // �����|�P�q�b�g�`�F�b�N
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Naminori },     // �_���[�W�ŏI�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
    // �_�C�r���O��Ԃ̑���ɂQ�{�_���[�W
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
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
static const BtlEventHandlerTable*  ADD_Fumituke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,    handler_Fumituke },     // �_���[�W�ŏI�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Fumituke( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ���������Ȃ��Ԃ̑���ɂQ�{�_���[�W
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
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
static const BtlEventHandlerTable*  ADD_Mineuti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK, handler_Mineuti },    // ���炦��`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_Koraeru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,           handler_Mamoru_StartSeq }, // ���U�����J�n
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_Mamoru_ExeCheck }, // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Mamoru_ExeFail  }, // ���U�������s�m��

//    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_Koraeru_ExeCheck },  // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_Koraeru },           // �����ރ��U�n���h��
    { BTL_EVENT_KORAERU_CHECK,           handler_Koraeru_Check },     // ���炦��`�F�b�N�n���h��
    { BTL_EVENT_TURNCHECK_BEGIN,         handler_Koraeru_TurnCheck }, // �^�[���`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�o�����ۃ`�F�b�N�n���h��
static void handler_Koraeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �����������񂾂񉺂���e�[�u�� 1/2, 1/4, 1/8, ---
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
      if( BTL_CALC_GetRand( randRange[counter] ) != 0 )
      {
        // �A�����p�ɂ�鎸�s�B���s������n���h�����Ə��ł��邱�ƂŁA���炦��`�F�b�N�ɔ������Ȃ��B
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
        BTL_EVENT_FACTOR_Remove( myHandle );
      }
    }
  }
}
// �����ރ��U�n���h��
static void handler_Koraeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ���U���o����\��t���āu���炦��v�̐��ɂȂ�
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Koraeru_Ready );
    HANDEX_STR_AddArg( &param->str, pokeID );
    work[ WORKIDX_STICK ] = 1;

    IncrementMamoruCounter( flowWk, pokeID );
  }
}
// ���炦��`�F�b�N�n���h��
static void handler_Koraeru_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �\��t�����Ȃ�u���炦��v����
    if( work[ WORKIDX_STICK] )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BPP_KORAE_WAZA_DEFENDER );
//      BTL_EVENT_FACTOR_Remove( myHandle );  // �P�^�[���Q��ȏケ�炦�邽�߂ɃR�����g�A�E�g
    }
  }
}
// �^�[���`�F�b�N�n���h��
static void handler_Koraeru_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // �^�[���`�F�b�N�܂œ\��t���Ă����玩�����g���폜
    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}


//----------------------------------------------------------------------------------
/**
 * �܂���E�݂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Mamoru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,           handler_Mamoru_StartSeq }, // ���U�����J�n
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_Mamoru_ExeCheck }, // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Mamoru_ExeFail  }, // ���U�������s�m��
    { BTL_EVENT_UNCATEGORIZE_WAZA,       handler_Mamoru          }, // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�����J�n�n���h��
static void handler_Mamoru_StartSeq( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const WazaID checkWazaTbl[] = {
    WAZANO_MAMORU, WAZANO_MIKIRI, WAZANO_KORAERU,
    WAZANO_KARI_WAIDOGAADO, WAZANO_KARI_FASTOGAADO,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    WazaID  waza = BPP_GetPrevWazaID( bpp );
    u32 i;
    for(i=0; i<NELEMS(checkWazaTbl); ++i){
      if( checkWazaTbl[i] == waza ){
        return;
      }
    }

    // ���U���o or �J�E���^���L���U�ȊO�̃��U�𒼑O�ɏo���Ă����烊�Z�b�g
    {

      BTL_HANDEX_PARAM_COUNTER*  counterParam = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );

      counterParam->pokeID = pokeID;
      counterParam->counterID = BPP_COUNTER_MAMORU;
      counterParam->value = 0;
    }
  }
}
// ���U�o�����ۃ`�F�b�N�n���h��
static void handler_Mamoru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �����������񂾂񉺂���e�[�u�� 1/1, 1/2, 1/4, 1/8, ---
  static const u8 randRange[] = {
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 counter = BPP_COUNTER_Get( bpp, BPP_COUNTER_MAMORU );

    // �A�����p�ɂ�鎸�s�`�F�b�N
    if( counter )
    {
      if( counter >= NELEMS(randRange) ){
        counter = NELEMS(randRange) - 1;
      }
      if( BTL_CALC_GetRand( randRange[counter] ) != 0 )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
        return;
      }
    }

    // ���^�[���Ō�̍s���Ȃ玸�s
    {
      u8 myOrder, maxOrder;
      if( BTL_SVFTOOL_GetMyActionOrder(flowWk, pokeID, &myOrder, &maxOrder) )
      {
        if( (myOrder+1) == maxOrder ){
          BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
          return;
        }
      }
    }
  }
}
// ���U�o�����s�m��
static void handler_Mamoru_ExeFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���s������J�E���^�����Z�b�g���n���h�����Ə��ł��邱�ƂŁA���̌�̃C�x���g�ɔ������Ȃ��B
    BTL_HANDEX_PARAM_COUNTER*  counterParam = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );

    counterParam->pokeID = pokeID;
    counterParam->counterID = BPP_COUNTER_MAMORU;
    counterParam->value = 0;

    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}
// �����ރ��U�n���h��
static void handler_Mamoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_TURNFLAG* flagParam;

    flagParam = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_TURNFLAG, pokeID );
    flagParam->pokeID = pokeID;
    flagParam->flag = BPP_TURNFLG_MAMORU;

    // �����ʔ���������J�E���^���C���N�������g
    IncrementMamoruCounter( flowWk, pokeID );
  }
}
/**
 *  �u�܂���v�J�E���^���C���N�������g
 */
static void IncrementMamoruCounter( BTL_SVFLOW_WORK* flowWk, u8 pokeID )
{
  BTL_HANDEX_PARAM_COUNTER*  param;
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

  param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_COUNTER, pokeID );
  param->value = BPP_COUNTER_Get( bpp, BPP_COUNTER_MAMORU ) + 1;
  param->pokeID = pokeID;
  param->counterID = BPP_COUNTER_MAMORU;


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
  GAMAN_STATE_END,
};

static const BtlEventHandlerTable*  ADD_Gaman( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,            handler_Gaman          }, //
    { BTL_EVENT_REQWAZA_MSG,               handler_Gaman_WazaMsg  },
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,    handler_Gaman_ExeCheck },
    { BTL_EVENT_DECIDE_TARGET,             handler_Gaman_Target   },
    { BTL_EVENT_WAZA_DMG_PROC1,            handler_Gaman_Damage   },
    { BTL_EVENT_WAZA_EXECUTE_FAIL,         handler_Gaman_Fail     },
//  { BTL_EVENT_WAZA_DMG_DETERMINE,        handler_Gaman_DamageDetermine }
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Gaman( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    switch( work[0] ){
    case GAMAN_STATE_1ST:
      // �P�^�[���ځF���������U���b�N��Ԃɂ��āA�����𔲂���B
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        param->sickID = WAZASICK_WAZALOCK;
        param->sickCont = BPP_SICKCONT_MakePermanentParam( BTL_EVENT_FACTOR_GetSubID(myHandle) );
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
    default:
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
  &&  (work[0] >= GAMAN_STATE_3RD)
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
  &&  (work[0] >= GAMAN_STATE_3RD)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 dmg_sum = gaman_getTotalDamage( bpp );
    if( dmg_sum ){
      dmg_sum *= 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, dmg_sum );
    }
    work[0] = GAMAN_STATE_END;
//    gaman_ReleaseStick( flowWk, pokeID, work );
  }
}
static void handler_Gaman_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] >= GAMAN_STATE_3RD)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 dmg_sum = gaman_getTotalDamage( bpp );
    if( dmg_sum == 0 ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
    work[0] = GAMAN_STATE_END;
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
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
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
static const BtlEventHandlerTable*  ADD_Recycle( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Recycle },          // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Recycle( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetItem(bpp) == ITEM_DUMMY_DATA )
    {
      u16 itemID = BPP_GetConsumedItem( bpp );
      if( itemID != ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
        BTL_HANDEX_PARAM_CLEAR_CONSUMED_ITEM* clear_param;


        param->itemID = itemID;
        param->pokeID = pokeID;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Recycle );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, itemID );

        clear_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CLEAR_CONSUMED_ITEM, pokeID );
        clear_param->pokeID = pokeID;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �T�C�R�V�t�g
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_PsycoShift( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_PsycoShift },          // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_PsycoShift( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    WazaSick sick = BPP_GetPokeSick( bpp );
    // �������|�P�����n�̏�Ԉُ�ŁA���肪�����łȂ��ꍇ�̂݌��ʂ�����
    if( sick != WAZASICK_NULL )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
      if( BPP_GetPokeSick(target) == WAZASICK_NULL )
      {
        BTL_HANDEX_PARAM_ADD_SICK    *sick_param;
        BTL_HANDEX_PARAM_CURE_SICK   *cure_param;
        BPP_SICK_CONT defaultCont = BPP_GetSickCont( bpp, sick );

        sick_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        sick_param->sickID = sick;
        if( (sick == POKESICK_DOKU) && BPP_SICKCONT_IsMoudokuCont(defaultCont) ){
          defaultCont = BPP_SICKCONT_MakeMoudokuCont();
        }else{
          defaultCont = BTL_CALC_MakeDefaultPokeSickCont( sick );
        }
        sick_param->sickCont = defaultCont;
        sick_param->poke_cnt = 1;
        sick_param->pokeID[0] = target_pokeID;
        sick_param->fAlmost = TRUE;

        cure_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
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
static const BtlEventHandlerTable*  ADD_Itamiwake( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Itamiwake },          // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Itamiwake( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp_me = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* bpp_target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    // ���҂̕��ϒl��HP�����킹��
    u32 hp_me     = BPP_GetValue( bpp_me, BPP_HP );
    u32 hp_target = BPP_GetValue( bpp_target, BPP_HP );
    u32 hp_avrg = (hp_me + hp_target) / 2;


    BTL_HANDEX_PARAM_SHIFT_HP* param;
    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
    param->poke_cnt = 2;
    param->pokeID[0] = pokeID;
    param->volume[0] = hp_avrg - hp_me;
    param->pokeID[1] = target_pokeID;
    param->volume[1] = hp_avrg - hp_target;


    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, BTL_STRID_STD_Itamiwake );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �͂炾����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Haradaiko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Haradaiko },          // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Haradaiko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 downHP = BPP_GetValue( bpp, BPP_MAX_HP ) / 2;
    u32 upVolume = BPP_RankEffectUpLimit( bpp, BPP_ATTACK_RANK );
    if( (BPP_GetValue(bpp, BPP_HP) > downHP) && (upVolume != 0) )
    {
      BTL_HANDEX_PARAM_SHIFT_HP     *hp_param;
      BTL_HANDEX_PARAM_RANK_EFFECT  *rank_param;
      BTL_HANDEX_PARAM_MESSAGE      *msg_param;

      hp_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
      hp_param->poke_cnt = 1;
      hp_param->pokeID[0] = pokeID;
      hp_param->volume[0] = -downHP;

      rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
      rank_param->rankType = BPP_ATTACK_RANK;
      rank_param->rankVolume = upVolume;
      rank_param->poke_cnt = 1;
      rank_param->pokeID[0] = pokeID;
      rank_param->fStdMsgDisable = TRUE;
      rank_param->fAlmost = TRUE;

      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
static const BtlEventHandlerTable*  ADD_Feint( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_MAMORU_BREAK,  handler_Feint_MamoruBreak },  // �܂��閳�����`�F�b�N
    { BTL_EVENT_DAMAGEPROC_END_INFO, handler_Feint_AfterDamage },  // �_���[�W������

    { BTL_EVENT_WAZA_EXE_DECIDE,     handler_Feint_Decide },  // ���U�o���m��
//    { BTL_EVENT_WAZASEQ_START,       handler_Feint_Start },  // ���U�����J�n
//    { BTL_EVENT_WAZASEQ_END,         handler_Feint_End   },  // ���U�����I��

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �܂��閳�����`�F�b�N
static void handler_Feint_MamoruBreak( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
// �_���[�W������
static void handler_Feint_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_TARGET_POKECNT) )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

      if( (BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_MAMORU))
      ||  (work[0])
      ) {
        BTL_HANDEX_PARAM_TURNFLAG* flg_param;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        flg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
        flg_param->pokeID = target_pokeID;
        flg_param->flag = BPP_TURNFLG_MAMORU;

        msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Feint );
        HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
      }
    }
  }
}
// ���U�o���m��
static void handler_Feint_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����Ȃ瑊��̃��C�h�K�[�h�E�t�@�X�g�K�[�h��j��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
    side = BTL_MAINUTIL_GetOpponentSide( side );

    if( BTL_SVFTOOL_IsExistSideEffect(flowWk, side, BTL_SIDEEFF_WIDEGUARD)
    ||  BTL_SVFTOOL_IsExistSideEffect(flowWk, side, BTL_SIDEEFF_FASTGUARD)
    ){
      BTL_HANDEX_PARAM_SIDEEFF_REMOVE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );

      param->side = side;
      BTL_CALC_BITFLG_Construction( param->flags, sizeof(param->flags) );
      BTL_CALC_BITFLG_Set( param->flags, BTL_SIDEEFF_WIDEGUARD );
      BTL_CALC_BITFLG_Set( param->flags, BTL_SIDEEFF_FASTGUARD );
      work[0] = 1;  // work[0] = �K�[�h�j��t���O�Ƃ���
    }
  }
}
// ���U�����J�n
static void handler_Feint_Start( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����Ȃ�X�L�b�v�`�F�b�N�n���h�����A�^�b�`����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENT_FACTOR_AttachSkipCheckHandler( myHandle, handler_Feint_SkipCheck );
  }
}
// ���U�����I��
static void handler_Feint_End( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����Ȃ�X�L�b�v�`�F�b�N�n���h�����f�^�b�`����
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENT_FACTOR_DettachSkipCheckHandler( myHandle );
  }
}
// �u�t�F�C���g�v�X�L�b�v�`�F�b�N�n���h��
static BOOL handler_Feint_SkipCheck( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID )
{
  if( factorType == BTL_EVENT_FACTOR_SIDE )
  {
    if( (subID == BTL_SIDEEFF_WIDEGUARD)      // ���C�h�K�[�h����
    ||  (subID == BTL_SIDEEFF_FASTGUARD)      // �t�@�X�g�K�[�h����
    ){
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * �ڂ���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_TuboWoTuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_TuboWoTuku },          // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_TuboWoTuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    static const BppValueID rankType[] = {
      BPP_ATTACK_RANK, BPP_DEFENCE_RANK, BPP_AGILITY_RANK, BPP_SP_ATTACK_RANK, BPP_SP_DEFENCE_RANK,
      BPP_HIT_RATIO, BPP_AVOID_RATIO,
    };
    u8  target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
    u8 valid_cnt, i;

    for(i=0, valid_cnt=0; i<NELEMS(rankType); ++i){
      if( BPP_IsRankEffectValid(bpp, rankType[i], 2) ){
        ++valid_cnt;
      }
    }

    if( valid_cnt )
    {
      u8 idx = BTL_CALC_GetRand( valid_cnt );
      for(i=0; i<NELEMS(rankType); ++i)
      {
        if( BPP_IsRankEffectValid(bpp, rankType[i], 2) )
        {
          if( idx == 0 )
          {
            BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
            param->pokeID[0] = target_pokeID;
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
static const BtlEventHandlerTable*  ADD_Nemuru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,   handler_Nemuru_exeCheck },
    { BTL_EVENT_UNCATEGORIZE_WAZA,        handler_Nemuru },          // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Nemuru_exeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    SV_WazaFailCause  cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );
    if( cause == SV_WAZAFAIL_NULL )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

      if( BPP_CheckSick(bpp, WAZASICK_NEMURI) )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
      }
      else if( BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_KAIHUKUHUUJI );
      }
      else if( BPP_IsHPFull(bpp) )
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_HPFULL );
      }
      else {
        u16 tok = BPP_GetValue( bpp, BPP_TOKUSEI_EFFECTIVE );
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

    // ��{��Ԉُ�͑S�Ď���
      if( BPP_GetPokeSick(bpp) != POKESICK_NULL )
      {
        BTL_HANDEX_PARAM_CURE_SICK     *cure_param;
        cure_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        cure_param->sickCode = WAZASICK_EX_POKEFULL;
        cure_param->pokeID[0] = pokeID;
        cure_param->poke_cnt = 1;
        cure_param->fStdMsgDisable = TRUE;
      }

      sick_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      sick_param->poke_cnt = 1;
      sick_param->pokeID[0] = pokeID;
      sick_param->sickID = WAZASICK_NEMURI;
      sick_param->sickCont = BTL_CALC_MakeWazaSickCont_Turn( 3 );

      HANDEX_STR_Setup( &sick_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Nemuru );
      HANDEX_STR_AddArg( &sick_param->exStr, pokeID );

      hp_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      hp_param->pokeID = pokeID;
      hp_param->recoverHP = (BPP_GetValue(bpp, BPP_MAX_HP) - BPP_GetValue(bpp, BPP_HP));
      hp_param->header.failSkipFlag = TRUE;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ��������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Meromero( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,   handler_Meromero_CheckNoEffect },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �e�N�X�`���[�Q
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Texture2( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Texture2 },          // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Texture2( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_TARGET1) );
    WazaID  waza = BPP_GetPrevWazaID( bpp );
    if( waza != WAZANO_NULL )
    {
      // ��R�������
      PokeType type = WAZADATA_GetType( waza );
      PokeType *typeArray = BTL_SVFTOOL_GetTmpWork( flowWk, sizeof(PokeType)* POKETYPE_NUMS );
      u32 cnt = BTL_CALC_GetResistTypes( type, typeArray );
      if( cnt )
      {
        const BTL_POKEPARAM* bppSelf = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
        u32 i;
        // �����Ɠ����^�C�v�͏��O
        for(i=0; i<cnt; ++i)
        {
          if( BPP_IsMatchType(bppSelf, typeArray[i])
          &&  PokeTypePair_IsPure( BPP_GetPokeType(bppSelf) )
          ){
            u32 j = i+1;
            while( j < cnt ){
              typeArray[j-1] = typeArray[j++];
            }
            --cnt;
            break;
          }
        }
        if( cnt )
        {
          BTL_HANDEX_PARAM_CHANGE_TYPE* param;
          u32 idx = BTL_CALC_GetRand( cnt );
          PokeType next_type = typeArray[ idx ];

          param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
          param->next_type = PokeTypePair_MakePure( next_type );
          param->pokeID = pokeID;
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �A���R�[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Encore( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Encore },          // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Encore( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    WazaID  prevWaza;

    prevWaza = BPP_GetPrevOrgWazaID( target );

    if( !BPP_CheckSick(target, WAZASICK_ENCORE)
    &&  (prevWaza != WAZANO_NULL)
    &&  (BPP_WAZA_IsUsable(target, prevWaza))
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
//      u8 turns = BTL_CALC_RandRange( 3, 7 );
      param->sickID = WAZASICK_ENCORE;
      param->sickCont = BTL_CALC_MakeWazaSickCont_Turn( 3 );
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->fAlmost = FALSE;

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Encore );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID );

    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���傤�͂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Chouhatu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Chouhatu },          // �����ރ��U�n���h��
  };

  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Chouhatu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, WAZASICK_TYOUHATSU) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

     // ���^�[���ɍs���ς݂̃|�P�����ɂ͌��ʃ^�[������+1����
      u8 turns = BTL_TYOUHATSU_EFFECTIVE_TURN;
      if( BPP_TURNFLAG_Get(target, BPP_TURNFLG_ACTION_DONE) ){
        ++turns;
      }

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
static const BtlEventHandlerTable*  ADD_Ichamon( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Ichamon },          // �����ރ��U�n���h��
  };

  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Ichamon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, WAZASICK_ICHAMON) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_ICHAMON;
      param->sickCont = BPP_SICKCONT_MakePermanent();
      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���Ȃ��΂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Kanasibari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Kanasibari },          // �����ރ��U�n���h��
  };

  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Kanasibari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_CheckSick(target, WAZASICK_KANASIBARI) )
    {
      WazaID  prevWaza = BPP_GetPrevOrgWazaID( target );

      if( (prevWaza != WAZANO_NULL) && (prevWaza != WAZANO_WARUAGAKI) )
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

        // ���^�[���ɍs���ς݂̃|�P�����ɂ͌��ʃ^�[������+1����
        u8 turns = BTL_KANASIBARI_EFFECTIVE_TURN;
        if( BPP_TURNFLAG_Get(target, BPP_TURNFLG_ACTION_DONE) ){
          ++turns;
        }

        BTL_Printf("�ӂ������߂郏�U�� %d\n", prevWaza);
        param->sickID = WAZASICK_KANASIBARI;
        param->sickCont = BPP_SICKCONT_MakeTurnParam( turns, prevWaza );
        param->poke_cnt = 1;
        param->pokeID[0] = targetPokeID;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂ�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Fuuin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,     handler_Fuuin },          // �����ރ��U�n���h��
  };

  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Fuuin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_FUIN) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );

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
static const BtlEventHandlerTable*  ADD_Alomatherapy( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Alomatherapy   }, // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_IyasiNoSuzu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_IyasiNoSuzu   },  // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
    const BTL_PARTY* party = BTL_SVFTOOL_GetPartyData( flowWk, pokeID );
    u8 i;

    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_STD, strID );

    cure_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );

    // �ꏊ���ő�i6�j���i�[�ł���z��Ƀp�[�e�B�����o�[�i�ő�6�j���i�[����̂ő��v�ȃn�Y�����A
    // �O�̂��߁A�I�[�o�[�������͊i�[���Ȃ��悤�ȏ��������Ă���
    cure_param->poke_cnt = BTL_PARTY_GetMemberCount( party );
    if( cure_param->poke_cnt > NELEMS(cure_param->pokeID) ){
      cure_param->poke_cnt = NELEMS(cure_param->pokeID);
    }

    cure_param->sickCode = WAZASICK_EX_POKEFULL;
    for(i=0; i<cure_param->poke_cnt; ++i){
      {
        const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( party, i );
        cure_param->pokeID[ i ] = BPP_GetID( bpp );
      }
    }

  }
}
//----------------------------------------------------------------------------------
/**
 * �����݂₰
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Okimiyage( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Okimiyage   },  // ���U�З͌���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Okimiyage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* kill_param;
    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    kill_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    kill_param->pokeID = pokeID;

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = targetPokeID;
    rank_param->rankType = BPP_ATTACK_RANK;
    rank_param->rankVolume = -2;
    rank_param->fAlmost = TRUE;

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    rank_param->poke_cnt = 1;
    rank_param->pokeID[0] = targetPokeID;
    rank_param->rankType = BPP_SP_ATTACK_RANK;
    rank_param->rankVolume = -2;
    rank_param->fAlmost = TRUE;
  }
}
//----------------------------------------------------------------------------------
/**
 * �����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Urami( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Urami   },  // ���U�З͌���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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

        decpp_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DECREMENT_PP, pokeID );
        decpp_param->pokeID = targetPokeID;
        decpp_param->volume = volume;
        decpp_param->wazaIdx = wazaIdx;

        msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
static const BtlEventHandlerTable*  ADD_JikoAnji( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_JikoAnji   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_JikoAnji( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

    BTL_HANDEX_PARAM_SET_RANK* rank_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID      = pokeID;
    rank_param->attack      = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->defence     = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_attack   = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence  = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility     = BPP_GetValue( target, BPP_AGILITY_RANK );
    rank_param->hit_ratio   = BPP_GetValue( target, BPP_HIT_RATIO );
    rank_param->avoid_ratio = BPP_GetValue( target, BPP_AVOID_RATIO );


    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
static const BtlEventHandlerTable*  ADD_HeartSwap( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_HeartSwap   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID      = pokeID;
    rank_param->attack      = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->defence     = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_attack   = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence  = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility     = BPP_GetValue( target, BPP_AGILITY_RANK );
    rank_param->hit_ratio   = BPP_GetValue( target, BPP_HIT_RATIO );
    rank_param->avoid_ratio = BPP_GetValue( target, BPP_AVOID_RATIO );

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID      = target_pokeID;
    rank_param->attack      = BPP_GetValue( self, BPP_ATTACK_RANK );
    rank_param->defence     = BPP_GetValue( self, BPP_DEFENCE_RANK );
    rank_param->sp_attack   = BPP_GetValue( self, BPP_SP_ATTACK_RANK );
    rank_param->sp_defence  = BPP_GetValue( self, BPP_SP_DEFENCE_RANK );
    rank_param->agility     = BPP_GetValue( self, BPP_AGILITY_RANK );
    rank_param->hit_ratio   = BPP_GetValue( self, BPP_HIT_RATIO );
    rank_param->avoid_ratio = BPP_GetValue( self, BPP_AVOID_RATIO );

    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HeartSwap );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �p���[�X���b�v
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_PowerSwap( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerSwap   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID = pokeID;
    rank_param->attack      = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->sp_attack   = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->defence     = BPP_GetValue( self,   BPP_DEFENCE_RANK );
    rank_param->sp_defence  = BPP_GetValue( self,   BPP_SP_DEFENCE_RANK );
    rank_param->agility     = BPP_GetValue( self,   BPP_AGILITY_RANK );
    rank_param->hit_ratio   = BPP_GetValue( self,   BPP_HIT_RATIO );
    rank_param->avoid_ratio = BPP_GetValue( self,   BPP_AVOID_RATIO );

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID      = target_pokeID;
    rank_param->attack      = BPP_GetValue( self,   BPP_ATTACK_RANK );
    rank_param->sp_attack   = BPP_GetValue( self,   BPP_SP_ATTACK_RANK );
    rank_param->defence     = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_defence  = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility     = BPP_GetValue( target, BPP_AGILITY_RANK );
    rank_param->hit_ratio   = BPP_GetValue( target, BPP_HIT_RATIO );
    rank_param->avoid_ratio = BPP_GetValue( target, BPP_AVOID_RATIO );

    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_PowerSwap );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �K�[�h�X���b�v
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_GuardSwap( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GuardSwap   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID      = pokeID;
    rank_param->attack      = BPP_GetValue( self,   BPP_ATTACK_RANK );
    rank_param->sp_attack   = BPP_GetValue( self,   BPP_SP_ATTACK_RANK );
    rank_param->defence     = BPP_GetValue( target, BPP_DEFENCE_RANK );
    rank_param->sp_defence  = BPP_GetValue( target, BPP_SP_DEFENCE_RANK );
    rank_param->agility     = BPP_GetValue( self,   BPP_AGILITY_RANK );
    rank_param->hit_ratio   = BPP_GetValue( self,   BPP_HIT_RATIO );
    rank_param->avoid_ratio = BPP_GetValue( self,   BPP_AVOID_RATIO );

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_RANK, pokeID );
    rank_param->pokeID      = target_pokeID;
    rank_param->attack      = BPP_GetValue( target, BPP_ATTACK_RANK );
    rank_param->sp_attack   = BPP_GetValue( target, BPP_SP_ATTACK_RANK );
    rank_param->defence     = BPP_GetValue( self,   BPP_DEFENCE_RANK );
    rank_param->sp_defence  = BPP_GetValue( self,   BPP_SP_DEFENCE_RANK );
    rank_param->agility     = BPP_GetValue( target, BPP_AGILITY_RANK );
    rank_param->hit_ratio   = BPP_GetValue( target, BPP_HIT_RATIO );
    rank_param->avoid_ratio = BPP_GetValue( target, BPP_AVOID_RATIO );

    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_GuardSwap );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �p���[�g���b�N
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_PowerTrick( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerTrick   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_PowerTrick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_STATUS* param;
    BTL_HANDEX_PARAM_SET_CONTFLAG*  flag_param;
    const BTL_POKEPARAM* bpp;

    bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );

    param->pokeID  = pokeID;
    param->attack  = BPP_GetValue_Base( bpp, BPP_DEFENCE_RANK );
    param->defence = BPP_GetValue_Base( bpp, BPP_ATTACK_RANK );
    param->fAttackEnable  = TRUE;
    param->fDefenceEnable = TRUE;

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_PowerTrick );
    HANDEX_STR_AddArg( &param->exStr, pokeID );

    flag_param = (BTL_HANDEX_PARAM_SET_CONTFLAG*)BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    flag_param->pokeID = pokeID;
    flag_param->flag = BPP_CONTFLG_POWERTRICK;
  }
}
//----------------------------------------------------------------------------------
/**
 * �p���[�V�F�A
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_PowerShare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_PowerShare   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );
    param->pokeID = target_pokeID;
    param->attack = avrg;
    param->sp_attack = sp_avrg;
    param->fAttackEnable = TRUE;
    param->fSpAttackEnable = TRUE;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );
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
 * �K�[�h�V�F�A
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_GuardShare( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GuardShare   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );
    param->pokeID = target_pokeID;
    param->defence = avrg;
    param->sp_defence = sp_avrg;
    param->fDefenceEnable = TRUE;
    param->fSpDefenceEnable = TRUE;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_STATUS, pokeID );
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
 * ���b�N�I��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_LockON( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_LockON   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_LockON( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->sickID = WAZASICK_MUSTHIT_TARGET;
    param->sickCont = BPP_SICKCONT_MakeTurnParam( 2, targetPokeID );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_LockOn );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
    HANDEX_STR_AddArg( &param->exStr, targetPokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���t���N�^�[
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Refrector( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Refrector   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_HikariNoKabe( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_HikariNoKabe   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_SinpiNoMamori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_SinpiNoMamori   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SinpiNoMamori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );

  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 5 );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_SINPINOMAMORI, cont, BTL_STRID_STD_SinpiNoMamori );

}
//----------------------------------------------------------------------------------
/**
 * ���낢����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_SiroiKiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_SiroiKiri   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_Oikaze( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Oikaze   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_Omajinai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Omajinai   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_Makibisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Makibisi   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_Dokubisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_Dokubisi   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable*  ADD_StealthRock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_StealthRock   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_StealthRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakePermanent();
  BtlSide side = BTL_MAINUTIL_PokeIDtoOpponentSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_STEALTHROCK, cont, BTL_STRID_STD_StealthRock );
}
//----------------------------------------------------------------------------------
/**
 * ���C�h�K�[�h
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_WideGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,            handler_Mamoru_StartSeq }, // ���U�����J�n
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,   handler_Mamoru_ExeCheck }, // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_WAZA_EXECUTE_FAIL,        handler_Mamoru_ExeFail  }, // ���U�������s�m��
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_WideGuard   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_WideGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 1 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_WIDEGUARD, cont, BTL_STRID_STD_WideGuard );

  IncrementMamoruCounter( flowWk, pokeID );
}

//-------------------------------------
/**
 *  �T�C�h�G�t�F�N�g���U����
 */
//-------------------------------------
static void common_SideEffect( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work,
  BtlSide side, BtlSideEffect effect, BPP_SICK_CONT cont, u16 strID )
{
//  BTL_Printf("���V=%d, �g�����|�P=%d, �G�t�F�N�g=%d\n", pokeID, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK), effect);
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SIDEEFF_ADD* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SIDEEFF_ADD, pokeID );
    param->effect = effect;
    param->side = side;
    param->cont = cont;
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, strID );
    HANDEX_STR_AddArg( &param->exStr, side );
  }
}
//----------------------------------------------------------------------------------
/**
 * �ւ񂵂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Hensin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Hensin   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Hensin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_HENSIN* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_HENSIN, pokeID );
    param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Hensin );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
    HANDEX_STR_AddArg( &param->exStr, param->pokeID );
  }
}

//----------------------------------------------------------------------------------
/**
 * �݂��Â��̂܂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MikadukiNoMai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_MikadukiNoMai   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_SVFTOOL_IsExistBenchPoke(flowWk, pokeID) )
    {
      BTL_HANDEX_PARAM_KILL* kill_param;
      BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

      eff_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
      eff_param->effect = BTL_POSEFF_MIKADUKINOMAI;
      eff_param->pos = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );

      kill_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
      kill_param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���₵�̂˂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_IyasiNoNegai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_IyasiNoNegai   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_SVFTOOL_IsExistBenchPoke(flowWk, pokeID) )
    {
      BTL_HANDEX_PARAM_KILL* kill_param;
      BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

      eff_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
      eff_param->effect = BTL_POSEFF_IYASINONEGAI;
      eff_param->pos = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );

      kill_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
      kill_param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �˂�������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Negaigoto( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Negaigoto   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Negaigoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* kill_param;
    BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

    eff_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
    eff_param->effect = BTL_POSEFF_NEGAIGOTO;
    eff_param->pos = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂炢�悿
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Miraiyoti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_DELAY_WAZA,  handler_Miraiyoti        },  // �x�����U�Z�b�g�`�F�b�N
    { BTL_EVENT_DECIDE_DELAY_WAZA, handler_Miraiyoti_Decide },  // �x�����U�Z�b�g����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �x�����U�Z�b�g�`�F�b�N
static void handler_Miraiyoti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlPokePos  targetPos = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    common_delayAttack( myHandle, flowWk, pokeID, targetPos );
  }
}
// �x�����U�Z�b�g����
static void handler_Miraiyoti_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Miraiyoti );
    HANDEX_STR_AddArg( &param->str, pokeID );
  }
}
/**
 *  �x�����U�Z�b�g�`�F�b�N���ʏ���
 */
static void common_delayAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u8 targetPos )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, TRUE) )
    {
      BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;

      if( targetPos == BTL_POS_NULL )
      {
        BtlPokePos myPos = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
        BtlRule  rule = BTL_SVFTOOL_GetRule( flowWk );
        targetPos = BTL_MAINUTIL_GetOpponentPokePos( rule, myPos, 0 );
      }

      eff_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
      eff_param->effect = BTL_POSEFF_DELAY_ATTACK;
      eff_param->pos = targetPos;
      eff_param->param[0] = BTL_SVFTOOL_GetTurnCount( flowWk ) + 2;
      eff_param->param[1] = BTL_EVENT_FACTOR_GetSubID( myHandle );
      eff_param->param_cnt = 2;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �͂߂̂˂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_HametuNoNegai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_DELAY_WAZA,  handler_HametuNoNegai        },  // �x�����U�Z�b�g�`�F�b�N
    { BTL_EVENT_DECIDE_DELAY_WAZA, handler_HametuNoNegai_Decide },  // �x�����U�Z�b�g����

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �x�����U�Z�b�g�`�F�b�N
static void handler_HametuNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BtlPokePos  targetPos = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    common_delayAttack( myHandle, flowWk, pokeID, targetPos );
  }
}
// �x�����U�Z�b�g����
static void handler_HametuNoNegai_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HametuNoNegai );
    HANDEX_STR_AddArg( &param->str, pokeID );
  }
}

//----------------------------------------------------------------------------------
/**
 * ������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Ieki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Ieki   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Ieki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK       *sick_param;
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    sick_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    sick_param->poke_cnt = 1;
    sick_param->pokeID[0] = target_pokeID;
    sick_param->sickID = WAZASICK_IEKI;
    sick_param->sickCont = BPP_SICKCONT_MakePermanent();
    HANDEX_STR_Setup( &sick_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Ieki );
    HANDEX_STR_AddArg( &sick_param->exStr, target_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ȃ肫��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Narikiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Narikiri   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Narikiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );
    BTL_HANDEX_PARAM_CHANGE_TOKUSEI*  tok_param;

    tok_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
    tok_param->pokeID = pokeID;
    tok_param->tokuseiID = BPP_GetValue( target, BPP_TOKUSEI );
    tok_param->fSameTokEffective = TRUE;
    HANDEX_STR_Setup( &tok_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Narikiri );
    HANDEX_STR_AddArg( &tok_param->exStr, pokeID );
    HANDEX_STR_AddArg( &tok_param->exStr, target_pokeID );
    HANDEX_STR_AddArg( &tok_param->exStr, tok_param->tokuseiID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ƃ�ڂ�����E�{���g�`�F���W
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_TonboGaeri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZADMG_SIDE_AFTER,  handler_TonboGaeri   },         // �_���[�W����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_TonboGaeri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_EFFECT_IDX* effIdx_param;

    if( BTL_SVFTOOL_IsExistBenchPoke(flowWk, pokeID) )
    {
      u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );

      BTL_HANDEX_PARAM_CHANGE_MEMBER* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Tonbogaeri );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, clientID );

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_MEMBER, pokeID );
      param->pokeID = pokeID;

      effIdx_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_EFFECT_IDX, pokeID );
      effIdx_param->effIndex = BTLV_WAZAEFF_POKECHANGE_ON;
      effIdx_param->header.failSkipFlag = TRUE;
    }
    else
    {
      effIdx_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_EFFECT_IDX, pokeID );
      effIdx_param->effIndex = BTLV_WAZAEFF_POKECHANGE_OFF;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���������X�s��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_KousokuSpin( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZADMG_SIDE_AFTER,  handler_KousokuSpin   },         // �_���[�W����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KousokuSpin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_CURE_SICK* cure_param;
    BtlSide  side;
    u8  fMakibisi, fDokubisi, fStealth;

    if( BPP_CheckSick(bpp, WAZASICK_YADORIGI) ){
      cure_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      cure_param->poke_cnt = 1;
      cure_param->pokeID[0] = pokeID;
      cure_param->sickCode = WAZASICK_YADORIGI;
    }
    if( BPP_CheckSick(bpp, WAZASICK_BIND) ){
      cure_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      cure_param->poke_cnt = 1;
      cure_param->pokeID[0] = pokeID;
      cure_param->sickCode = WAZASICK_BIND;
    }

    side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
    fMakibisi = BTL_HANDER_SIDE_IsExist( side, BTL_SIDEEFF_MAKIBISI );
    fDokubisi = BTL_HANDER_SIDE_IsExist( side, BTL_SIDEEFF_DOKUBISI );
    fStealth = BTL_HANDER_SIDE_IsExist( side, BTL_SIDEEFF_STEALTHROCK );

    if( fMakibisi || fDokubisi || fStealth )
    {
      BTL_HANDEX_PARAM_SIDEEFF_REMOVE* side_param;

      side_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
      side_param->side = side;
      BTL_CALC_BITFLG_Construction( side_param->flags, sizeof(side_param->flags) );

      if( fMakibisi ){
        BTL_CALC_BITFLG_Set( side_param->flags, BTL_SIDEEFF_MAKIBISI );
      }
      if( fDokubisi ){
        BTL_CALC_BITFLG_Set( side_param->flags, BTL_SIDEEFF_DOKUBISI );
      }
      if( fStealth ){
        BTL_CALC_BITFLG_Set( side_param->flags, BTL_SIDEEFF_STEALTHROCK );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �o�g���^�b�`
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_BatonTouch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_BatonTouch   },         // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����ރ��U�n���h��
static void handler_BatonTouch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_SVFTOOL_IsExistBenchPoke(flowWk, pokeID) )
    {
      BTL_HANDEX_PARAM_POSEFF_ADD* eff_param;
      BTL_HANDEX_PARAM_CHANGE_MEMBER* change_param;

      eff_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_POSEFF_ADD, pokeID );
      eff_param->effect = BTL_POSEFF_BATONTOUCH;
      eff_param->pos = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
      eff_param->param[0] = pokeID;
      eff_param->param_cnt = 1;

      change_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_MEMBER, pokeID );
      change_param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �e���|�[�g
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Teleport( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Teleport   },         // �����ރ��U
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND, handler_Teleport_ExeCheck },  // ���s�`�F�b�N
    { BTL_EVENT_NIGERU_EXMSG,       handler_Teleport_ExMsg },     // ������Ƃ��̓��ꃁ�b�Z�[�W
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Teleport_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_SVFTOOL_GetCompetitor(flowWk) != BTL_COMPETITOR_WILD ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
  }
}
static void handler_Teleport( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_QUIT_BATTLE, pokeID );
  }
}
static void handler_Teleport_ExMsg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, TRUE) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Teleport );
    HANDEX_STR_AddArg( &param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ȃ�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Nagetukeru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND, handler_Nagetukeru_ExeCheck },
    { BTL_EVENT_WAZA_POWER,             handler_Nagetukeru_WazaPower},
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_Nagetukeru_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static void handler_Nagetukeru_WazaPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 itemID = BPP_GetItem( bpp );
    u32 pow = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_NAGETUKERU_ATC );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
static void handler_Nagetukeru_DmgAfter( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 itemID = BPP_GetItem( bpp );
    if( itemID != ITEM_DUMMY_DATA )
    {
      BTL_HANDEX_PARAM_SET_ITEM* item_param;

      item_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      item_param->pokeID = pokeID;
      item_param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &item_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Nagetukeru );
      HANDEX_STR_AddArg( &item_param->exStr, pokeID );
      HANDEX_STR_AddArg( &item_param->exStr, itemID );

      {
        int equip = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_NAGETUKERU_EFF );
        if( equip )
        {
           BTL_HANDEX_PARAM_ITEM_SP*  param;
           param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ITEM_SP, pokeID );
           param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
           param->itemID = itemID;
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �ł񂶂ӂ䂤
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_DenjiFuyuu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_DenjiFuyuu   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_DenjiFuyuu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK*  param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->sickID = WAZASICK_FLYING;
    param->sickCont = BPP_SICKCONT_MakeTurn( 5 );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_DenjiFuyu );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ă�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Tedasuke( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_Tedasuke_Ready },       // �����ރ��U�n���h��
    { BTL_EVENT_WAZA_POWER,         handler_Tedasuke_WazaPow },     // ���U�З̓`�F�b�N
    { BTL_EVENT_TURNCHECK_BEGIN,    handler_Tedasuke_TurnCheck },   // �^�[���`�F�b�N�J�n�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Tedasuke_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_SVFTOOL_GetFrontPosNum(flowWk) > 1 )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
    {
      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

      if( !BPP_IsDead(bpp)
      &&  !BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_ACTION_DONE)
      ){
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

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
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    BTL_EVENT_FACTOR_Remove( myHandle );  // �^�[���`�F�b�N�ŋ������E
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂ��낾����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_FukuroDataki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_COUNT,  handler_FukuroDataki },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_FukuroDataki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_PARTY* party = BTL_SVFTOOL_GetPartyData( flowWk, pokeID );
    const BTL_POKEPARAM* bpp;
    u32 cnt_max, cnt, i;

    cnt_max = cnt = BTL_PARTY_GetMemberCount( party );
    for(i=0; i<cnt_max; ++i)
    {
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( BPP_IsDead(bpp)
      ||  (BPP_GetPokeSick(bpp) != POKESICK_NULL)
      ){
        --cnt;
      }
    }
    BTL_Printf("�����Ăď�Ԉُ킶��Ȃ����Ԃ̐�=%d\n", cnt);
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
static const BtlEventHandlerTable*  ADD_Nekodamasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,       handler_Nekodamasi },     // ���U�o�����ۃ`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Nekodamasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_ACTION_DONE) )
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
static const BtlEventHandlerTable*  ADD_AsaNoHizasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RECOVER_HP_RATIO,       handler_AsaNoHizasi },     // HP�񕜗��v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_AsaNoHizasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlWeather w = BTL_SVFTOOL_GetWeather( flowWk );
    fx32 ratio;
    switch( w ){
    case BTL_WEATHER_SHINE:
      ratio = FX32_CONST( 0.667 );
      break;
    case BTL_WEATHER_RAIN:
    case BTL_WEATHER_SAND:
    case BTL_WEATHER_SNOW:
      ratio = FX32_CONST( 0.25 );
      break;
    default:
      ratio = FX32_CONST( 0.5 );
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, ratio );
  }
}
//----------------------------------------------------------------------------------
/**
 * ������Ƃ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_SoraWoTobu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_SoraWoTobu_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,     handler_SoraWoTobu_TameRelease },   // ���߉��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SoraWoTobu_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SoraWoTobu );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_SoraWoTobu_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �V���h�[�_�C�u
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_ShadowDive( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,          handler_ShadowDive_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,        handler_ShadowDive_TameRelease },   // ���߉��
    { BTL_EVENT_CHECK_MAMORU_BREAK,  handler_Feint_MamoruBreak },        // �܂��閳�����`�F�b�N
    { BTL_EVENT_DAMAGEPROC_END_INFO, handler_ShadowDive_AfterDamage },   // �_���[�W������
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_ShadowDive_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_ShadowDive );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_ShadowDive_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
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

      flg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_TURNFLAG, pokeID );
      flg_param->pokeID = target_pokeID;
      flg_param->flag = BPP_TURNFLG_MAMORU;

      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
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
static const BtlEventHandlerTable*  ADD_Tobihaneru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Tobihaneru_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,     handler_Tobihaneru_TameRelease },   // ���߉��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Tobihaneru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Tobihaneru );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_Tobihaneru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �_�C�r���O
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Diving( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Diving_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,     handler_Diving_TameRelease },   // ���߉��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Diving_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_DIVING;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Diving );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_Diving_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_DIVING;
  }
}
//----------------------------------------------------------------------------------
/**
 * ���Ȃ��ق�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_AnaWoHoru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_AnaWoHoru_TameStart },     // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,     handler_AnaWoHoru_TameRelease },   // ���߉��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_AnaWoHoru_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_ANAWOHORU;
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_AnaWoHoru );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
    }
  }
}
static void handler_AnaWoHoru_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_ANAWOHORU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �\�[���[�r�[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_SolarBeam( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_TAMETURN_SKIP,  handler_SolarBeam_TameSkip },  // ���߃X�L�b�v����
    { BTL_EVENT_TAME_START,           handler_SolarBeam_TameStart }, // ���ߊJ�n
    { BTL_EVENT_WAZA_POWER,           handler_SolarBeam_Power     }, // ���U�З͌���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SolarBeam_TameSkip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SHINE )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }
  }
}
static void handler_SolarBeam_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SolarBeam );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
static void handler_SolarBeam_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  ( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_RAIN )
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(0.5) );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���܂�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Kamaitati( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_Kamaitati_TameStart },     // ���ߊJ�n
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Kamaitati_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Kamaitati );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �S�b�h�o�[�h
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_GodBird( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_GodBird_TameStart },     // ���ߊJ�n
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_GodBird_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_GodBird );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���P�b�g����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_RocketZutuki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,       handler_RocketZutuki_TameStart },     // ���ߊJ�n
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_RocketZutuki_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    BTL_HANDEX_PARAM_RANK_EFFECT* rank_param;

    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RocketZutuki );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );

    rank_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
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
static const BtlEventHandlerTable* ADD_Tuibamu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_Tuibamu              },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Tuibamu_DmgDetermine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{

}
static void handler_Tuibamu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
    u16 itemID = BPP_GetItem( target );
    if( ITEM_CheckNuts(itemID) )
    {
      BTL_HANDEX_PARAM_SET_ITEM* item_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      BTL_HANDEX_PARAM_ITEM_SP* eq_param;

      item_param->pokeID = targetPokeID;
      item_param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &item_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Tuibamu );
      HANDEX_STR_AddArg( &item_param->exStr, pokeID );
      HANDEX_STR_AddArg( &item_param->exStr, itemID );

      eq_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ITEM_SP, pokeID );
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
static const BtlEventHandlerTable* ADD_Waruagaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM,      handler_Waruagaki_WazaParam   }, // ���U�p�����[�^�擾
    { BTL_EVENT_DAMAGEPROC_END,  handler_Waruagaki             }, // �_���[�W�v���Z�X�I���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�p�����[�^�擾
static void handler_Waruagaki_WazaParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, TRUE );
  }
}
// �_���[�W�v���Z�X�I���n���h��
static void handler_Waruagaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_SHIFT_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SHIFT_HP, pokeID );
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->volume[0] = -BTL_CALC_QuotMaxHP( bpp, 4 );
  }
}

//----------------------------------------------------------------------------------
/**
 * �݂��Â�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Michidure( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,   handler_Michidure_Ready        },  // �����ރ��U�n���h��
    { BTL_EVENT_WAZASEQ_START,       handler_Michidure_WazaSeqStart },  // ���U�����J�n�n���h��
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_Michidure_WazaDamage   },  // ���U�_���[�W������
//    { BTL_EVENT_TURNCHECK_BEGIN,     handler_Michidure_TurnCheck    },  // �^�[���`�F�b�N�J�n�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����ރ��U�n���h��
static void handler_Michidure_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ���A�ꏀ�����b�Z�[�W�\�����n���h���\��t��
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MichidureReady );
    HANDEX_STR_AddArg( &param->str, pokeID );

    work[ WORKIDX_STICK ] = 1;
  }
}
// ���U�����J�n�n���h��
static void handler_Michidure_WazaSeqStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �u�݂��Â�v�ȊO�̃��U���o���Ȃ�A�\��t������
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) != BTL_EVENT_FACTOR_GetSubID(myHandle))
  ){
      BTL_EVENT_FACTOR_Remove( myHandle );
  }
}
// ���U�_���[�W������
static void handler_Michidure_WazaDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF)==pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_KILL* kill_param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      kill_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
      kill_param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MichidureDone );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );

      BTL_EVENT_FACTOR_Remove( myHandle );  // ���A�ꐬ���Ŏ��E
    }
  }
}
// �^�[���`�F�b�N�J�n�n���h��
static void handler_Michidure_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    BTL_EVENT_FACTOR_Remove( myHandle );  // �^�[���`�F�b�N�ŋ������E
  }
}
//----------------------------------------------------------------------------------
/**
 * ����˂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Onnen( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,           handler_Onnen_Ready      }, // �����ރ��U�n���h��
    { BTL_EVENT_WAZA_DMG_REACTION,           handler_Onnen_WazaDamage }, // ���U�_���[�W������
    { BTL_EVENT_TURNCHECK_BEGIN,             handler_Onnen_TurnCheck  }, // �^�[���`�F�b�N�J�n�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Onnen_Ready( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

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
      BTL_HANDEX_PARAM_MESSAGE* msg_param;
      WazaID  waza;
      u16     wazaIdx;

      u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      const BTL_POKEPARAM* targetPoke = BTL_SVFTOOL_GetPokeParam( flowWk, target_pokeID );

      waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      wazaIdx = BPP_WAZA_SearchIdx( targetPoke, waza );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        BTL_HANDEX_PARAM_PP* pp_param;

        pp_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DECREMENT_PP, pokeID );
        pp_param->pokeID = target_pokeID;
        pp_param->wazaIdx = wazaIdx;
        pp_param->volume = BPP_WAZA_GetPP( targetPoke, wazaIdx ) * -1;

        msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_OnnenDone );
        HANDEX_STR_AddArg( &msg_param->str, target_pokeID );
        HANDEX_STR_AddArg( &msg_param->str, waza );
      }

      BTL_EVENT_FACTOR_Remove( myHandle );  // ���A�ꐬ���Ŏ��E
    }
  }
}
static void handler_Onnen_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    BTL_EVENT_FACTOR_Remove( myHandle );  // �^�[���`�F�b�N�ŋ������E
  }
}

//----------------------------------------------------------------------------------
/**
 * ���������Ȃ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Tiisakunaru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,  handler_Tiisakunaru   },  // ���U�o���m�茈��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

static void handler_Tiisakunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_TIISAKUNARU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �܂邭�Ȃ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Marukunaru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,  handler_Marukunaru   },  // ���U�З͌���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Marukunaru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_MARUKUNARU;
  }
}
//----------------------------------------------------------------------------------
/**
 * �͂˂₷��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Haneyasume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_EFFECTIVE,  handler_Haneyasume   },  // ���U�o���L���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Haneyasume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* sick_param;

    sick_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    sick_param->poke_cnt = 1;
    sick_param->pokeID[0] = pokeID;
    sick_param->fStdMsgDisable = TRUE;
    sick_param->sickID = WAZASICK_HANEYASUME;
    sick_param->sickCont = BPP_SICKCONT_MakeTurnParam( 1, POKETYPE_HIKOU );
    BPP_SICKCONT_SetFlag( &sick_param->sickCont, TRUE );
  }
}
//----------------------------------------------------------------------------------
/**
 * �������p���`
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_KiaiPunch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_BEFORE_FIGHT,  handler_KiaiPunch   },  // �^�[���ŏ��̃��U�V�[�P���X���O
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KiaiPunch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_TURNFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_TURNFLAG, pokeID );
    BTL_HANDEX_PARAM_EFFECT_BY_POS* eff_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;


    param->pokeID = pokeID;
    param->flag = BPP_TURNFLG_MUST_SHRINK;

    eff_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_EFFECT_BY_POS, pokeID );
    eff_param->effectNo = BTLEFF_KIAIPUNCH_TAME;
    eff_param->pos_from = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
    eff_param->pos_to = BTL_POS_NULL;

    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KiaiPunch );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );


  }
}


//------------------------------------------------------------------------------------------------------
/**
 *  �h�����U�i���̃��U���Ăяo�����U�j�֘A
 */
//------------------------------------------------------------------------------------------------------

// u16 �z��� �w��ID�̃��U���܂܂�Ă��邩�`�F�b�N
static BOOL isWazaExclude( const u16* wazaTbl, u32 tblElems, WazaID waza )
{
  u32 i;
  for(i=0; i<tblElems; ++i)
  {
    if( wazaTbl[i] == waza ){
      return TRUE;
    }
  }
  return FALSE;
}
// �w�肳�ꂽID�̃��U���A�h�����U����Ăяo���Ă͂����Ȃ����U���ǂ�������
static BOOL isReqWazaForbid( WazaID waza, const u16* exTbl, u32 exTblElems )
{
  // �h�����U�����΂ɌĂяo���Ȃ����U���X�g
  static const u16 ReqWazaExcludeTbl[] = {
    WAZANO_YUBIWOHURU,    WAZANO_NEGOTO,      WAZANO_NEKONOTE,
    WAZANO_MANEKKO,       WAZANO_SAKIDORI,    WAZANO_OUMUGAESI,
    WAZANO_SIZENNOTIKARA, WAZANO_OSYABERI,    WAZANO_WARUAGAKI,
    WAZANO_SUKETTI,       WAZANO_MONOMANE,
  };

  if( isWazaExclude( ReqWazaExcludeTbl, NELEMS(ReqWazaExcludeTbl), waza ) ){
    return TRUE;
  }

  if( exTbl != NULL ){
    if( isWazaExclude( exTbl, exTblElems, waza ) ){
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
static const BtlEventHandlerTable*  ADD_YubiWoFuru( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
//    { BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  handler_YubiWoFuru     },  // �����U�Ăяo���i�����v�Z�O����j
    { BTL_EVENT_REQWAZA_PARAM,          handler_YubiWoFuru     },  // �����U�p�����[�^�`�F�b�N
    { BTL_EVENT_REQWAZA_MSG,            handler_YubiWoFuru_Msg },  // �����U���s�����b�Z�[�W
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_YubiWoFuru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // �u��т��ӂ�v�ŏo�Ȃ����U�ꗗ
    static const u16 forbidWazaTbl[] = {
      WAZANO_YUBIWOHURU,    WAZANO_NEGOTO,      WAZANO_NEKONOTE,
      WAZANO_MANEKKO,       WAZANO_SAKIDORI,    WAZANO_OUMUGAESI,
      WAZANO_SIZENNOTIKARA, WAZANO_OSYABERI,    WAZANO_WARUAGAKI,

      WAZANO_SUKETTI,       WAZANO_MONOMANE,    WAZANO_KAUNTAA,
      WAZANO_MIRAAKOOTO,    WAZANO_MAMORU,      WAZANO_MIKIRI,
      WAZANO_KORAERU,       WAZANO_MITIDURE,    WAZANO_KONOYUBITOMARE,
      WAZANO_YOKODORI,      WAZANO_TEDASUKE,    WAZANO_DOROBOU,
      WAZANO_HOSIGARU,      WAZANO_TORIKKU,     WAZANO_KIAIPANTI,
      WAZANO_FEINTO,
    };

    WazaID     waza = BTL_CALC_RandWaza( forbidWazaTbl, NELEMS(forbidWazaTbl) );
    BtlPokePos pos  = BTL_SVFTOOL_ReqWazaTargetAuto( flowWk, pokeID, waza );

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
static const BtlEventHandlerTable*  ADD_SizenNoTikara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,        handler_SizenNoTikara     },  // �h�����U�p�����[�^�`�F�b�N
    { BTL_EVENT_REQWAZA_MSG,          handler_SizenNoTikara_Msg },  // �h�����U���s�����b�Z�[�W
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����U�Ăяo���p�����[�^�`�F�b�N
static void handler_SizenNoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlBgAttr  bg = BTL_SVFTOOL_GetLandForm( flowWk );
    WazaID  waza;
    BtlPokePos pos;

    switch( bg ){

    case BATTLE_BG_ATTR_GROUND:         //�ʏ�n��
    case BATTLE_BG_ATTR_GROUND_S1:      //�l�G����n�ʂP
    case BATTLE_BG_ATTR_GROUND_S2:      //�l�G����n�ʂQ
    case BATTLE_BG_ATTR_PALACE:         //�p���X�ł̑ΐ��p
    case BATTLE_BG_ATTR_SAND:           //���n
      waza = WAZANO_ZISIN;
      break;

    case BATTLE_BG_ATTR_E_INDOOR:       //�����G���J�E���g
    default:
      waza = WAZANO_TORAIATAKKU;
      break;

    case BATTLE_BG_ATTR_CAVE:           //���A
      waza = WAZANO_IWANADARE;
      break;

    case BATTLE_BG_ATTR_LAWN:           //�Ő�
    case BATTLE_BG_ATTR_E_GRASS:        //�G���J�E���g��
      waza = WAZANO_TANEBAKUDAN;
      break;

    case BATTLE_BG_ATTR_WATER:          //����
    case BATTLE_BG_ATTR_POOL:           //�����܂�
    case BATTLE_BG_ATTR_SHOAL:          //��
      waza = WAZANO_HAIDOROPONPU;
      break;

    case BATTLE_BG_ATTR_MARSH:          //�󂢎���
      waza = WAZANO_DOROBAKUDAN;
      break;

    case BATTLE_BG_ATTR_SNOW:           //�ጴ
      waza = WAZANO_HUBUKI;
      break;

    case BATTLE_BG_ATTR_ICE:            //�X��
      waza = WAZANO_REITOUBIIMU;
      break;
    }

    pos  = BTL_SVFTOOL_ReqWazaTargetAuto( flowWk, pokeID, waza );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  waza );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
  }
}
// �h�����U���s�����b�Z�[�W
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
static const BtlEventHandlerTable*  ADD_Nekonote( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,      handler_Nekonote },  // �����U�p�����[�^�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Nekonote( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u16 forbidTbl[] = {
      WAZANO_KAUNTAA,       WAZANO_MIRAAKOOTO,  WAZANO_MAMORU,      WAZANO_MIKIRI,
      WAZANO_KORAERU,       WAZANO_MITIDURE,    WAZANO_KONOYUBITOMARE,
      WAZANO_YOKODORI,      WAZANO_TEDASUKE,    WAZANO_DOROBOU,
      WAZANO_HOSIGARU,      WAZANO_TORIKKU,     WAZANO_KIAIPANTI,
      WAZANO_FEINTO,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_PARTY* party = BTL_SVFTOOL_GetPartyData( flowWk, pokeID );
    u32  arySize = PTL_WAZA_MAX * BTL_PARTY_MEMBER_MAX * sizeof(u16);
    u16* wazaAry = BTL_SVFTOOL_GetTmpWork( flowWk, arySize );
    const BTL_POKEPARAM* bpp;

    u8 memberCnt, wazaCnt, i;

    wazaCnt = 0;
    memberCnt = BTL_PARTY_GetMemberCount( party );
    for(i=0; i<memberCnt; ++i)
    {
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( BPP_GetID(bpp) != pokeID )
      {
        u8 pokeWazaMax = BPP_WAZA_GetCount( bpp );
        u8 j;
        u16 waza;
        for(j=0; j<pokeWazaMax; ++j)
        {
          waza = BPP_WAZA_GetID( bpp, j );
          if( !isReqWazaForbid(waza, forbidTbl, NELEMS(forbidTbl)) ){
            wazaAry[wazaCnt++] = waza;
          }
        }
      }
    }

    if( wazaCnt )
    {
      u8 idx = BTL_CALC_GetRand( wazaCnt );
      BtlPokePos  pos= BTL_SVFTOOL_ReqWazaTargetAuto( flowWk, pokeID, wazaAry[idx] );

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
static const BtlEventHandlerTable*  ADD_Negoto( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK_1ST, handler_Ibiki_CheckFail_1 },  // ���U�o�����ۃ`�F�b�N�n���h���P
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND, handler_Ibiki_CheckFail_2 },  // ���U�o�����ۃ`�F�b�N�n���h���Q
    { BTL_EVENT_REQWAZA_PARAM,          handler_Negoto            },  // �h���킴�p�����[�^�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Negoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u16 forbidWazaTbl[] = {
    WAZANO_SAWAGU,      WAZANO_ROKETTOZUTUKI, WAZANO_GODDOBAADO,
    WAZANO_SOORAABIIMU, WAZANO_GAMAN,         WAZANO_KAMAITATI,
    WAZANO_SORAWOTOBU,  WAZANO_TOBIHANERU,    WAZANO_SYADOODAIBU,
  };

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
        if( (isReqWazaForbid(waza, forbidWazaTbl, NELEMS(forbidWazaTbl)) == FALSE)
        &&  (WAZADATA_GetFlag(waza, WAZAFLAG_Tame) == FALSE)
        ){
          work[cnt++] = waza;
        }
      }
      if( cnt )
      {
        u8 idx = BTL_CALC_GetRand( cnt );
        WazaID waza = work[ idx ];
        BtlPokePos  pos= BTL_SVFTOOL_ReqWazaTargetAuto( flowWk, pokeID, waza );

        BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID,  waza );
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
      }
      else
      {
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
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
static const BtlEventHandlerTable*  ADD_OumuGaesi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,          handler_OumuGaesi_CheckParam },  // �����U�p�����[�^�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      BtlPokePos  myPos = BTL_SVFTOOL_GetExistFrontPokePos( flowWk, pokeID );
      BtlRule  rule = BTL_SVFTOOL_GetRule( flowWk );
      targetPos = BTL_MAINUTIL_GetOpponentPokePos( rule, myPos, 0 );
    }


    if( BTL_SVFTOOL_ExpandPokeID(flowWk, targetPos, targetPokeIDAry) )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeIDAry[0] );
      WazaID  waza = BPP_GetPrevWazaID( bpp );
      {
        BTL_Printf( "�I�E���F�_�����ʒu�Ƀ|�P���������B�ʒu=%d, ID=%d�ł�\n", targetPos, targetPokeIDAry[0]);
      }
      if( (waza == WAZANO_NULL)
      ||  (WAZADATA_GetFlag(waza, WAZAFLAG_Oumu) == FALSE)
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
static const BtlEventHandlerTable*  ADD_Sakidori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,        handler_Sakidori_CheckParam },  // �����U�p�����[�^�`�F�b�N
    { BTL_EVENT_WAZA_POWER,           handler_Sakidori_Power      },  // ���U�З͌���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      BtlPokePos  myPos = BTL_SVFTOOL_GetExistFrontPokePos( flowWk, pokeID );
      BtlRule  rule = BTL_SVFTOOL_GetRule( flowWk );
      targetPos = BTL_MAINUTIL_GetOpponentPokePos( rule, myPos, 0 );
    }

    if( BTL_SVFTOOL_ExpandPokeID(flowWk, targetPos, targetPokeIDAry) )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeIDAry[0] );
      BTL_ACTION_PARAM action;
      WazaID waza;
      u8 fSucceess = FALSE;

      do{

        // ���肪���ɍs�����Ă����玸�s
        if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_WAZAPROC_DONE) ){ break; }

        // ���^�[���̃A�N�V�������擾�ł��Ȃ������玸�s
        if( !BTL_SVFTOOL_GetThisTurnAction( flowWk, targetPokeIDAry[0], &action ) ){ break; }

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
static const BtlEventHandlerTable*  ADD_Manekko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_REQWAZA_PARAM,         handler_Manekko_CheckParam },  // �����U�p�����[�^�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Manekko_CheckParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u16 forbidTbl[] = {
      WAZANO_KAUNTAA,       WAZANO_MIRAAKOOTO,  WAZANO_MAMORU,      WAZANO_MIKIRI,
      WAZANO_KORAERU,       WAZANO_MITIDURE,    WAZANO_KONOYUBITOMARE,
      WAZANO_YOKODORI,      WAZANO_TEDASUKE,    WAZANO_DOROBOU,
      WAZANO_HOSIGARU,      WAZANO_TORIKKU,     WAZANO_KIAIPANTI,
      WAZANO_FEINTO,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    WazaID waza = BTL_SVFTOOL_GetPrevExeWaza( flowWk );
    if( (waza != WAZANO_NULL)
    &&  (!isReqWazaForbid(waza, forbidTbl, NELEMS(forbidTbl)))
    ){
      BtlPokePos  pos = BTL_SVFTOOL_ReqWazaTargetAuto( flowWk, pokeID, waza );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZAID, waza );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POKEPOS, pos );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���񂵂̂�����E���₵������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_GensiNoTikara( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_RANKEFF_VALUE,   handler_GensiNoTikara  },  // ���U�����N�������ʃ`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�����N�������ʃ`�F�b�N�n���h��
static void handler_GensiNoTikara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ���������U�g�p�ҁ��Ώۂ̃P�[�X��
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_STATUS_TYPE, WAZA_RANKEFF_MULTI5 );
  }
}

//----------------------------------------------------------------------------------
/**
 * �x�m���V���b�N
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_BenomShock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_BenomShock },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_BenomShock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �ǂ���Ԃ̑���ɈЗ͂Q�{
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
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
static const BtlEventHandlerTable*  ADD_Tatarime( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Tatarime },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Tatarime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �|�P�n��ԏ�Ԃ̑���ɈЗ͂Q�{
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
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
static const BtlEventHandlerTable*  ADD_Acrobat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Acrobat },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Acrobat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �������ǂ����𑕔����Ă��Ȃ����ɈЗ͂Q�{
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetItem(bpp) == ITEM_DUMMY_DATA ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �A�V�X�g�p���[
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_AsistPower( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_AsistPower },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_AsistPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u8 RankValue[] = {
    BPP_ATTACK_RANK, BPP_DEFENCE_RANK, BPP_AGILITY_RANK, BPP_SP_ATTACK_RANK, BPP_SP_DEFENCE_RANK,
    BPP_HIT_RATIO, BPP_AVOID_RATIO,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �����̔\�͕ω��㏸�l���v*20���З͂ɉ��Z
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32  upSum = 0;
    int  rank;
    int i;

    for(i=0; i<NELEMS(RankValue); ++i)
    {
      rank = BPP_GetValue( bpp, RankValue[i] ) - BPP_RANK_STATUS_DEFAULT;
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
 * �w�r�[�{���o�[
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_HeavyBomber( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_HeavyBomber },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_HeavyBomber( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    int weight_ratio = BPP_GetWeight(bpp) / BPP_GetWeight(target);
    u32 pow;

    // ������d���قǈЗ͂�����
    if( weight_ratio >= 5 ){
      pow = 120;
    }else if( weight_ratio == 4 ){
      pow = 100;
    }else if( weight_ratio == 3 ){
      pow = 80;
    }else if( weight_ratio == 2 ){
      pow = 60;
    }else{
      pow = 40;
    }

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * �G���N�g�{�[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_ElectBall( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_ElectBall },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_ElectBall( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

    int quot = BPP_GetValue(bpp, BPP_AGILITY) / BPP_GetValue(target, BPP_AGILITY);
    u32 pow;

    // ������f�����قǈЗ͂�����
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
 * �G�R�[�{�C�X
 *
 * ���^�[���A��ɂ���N�����g�����тɃJ�E���^���C���N�������g����A
 * ���̓s�x�A���U�̈З͂��オ���Ă����B�i�N���g��Ȃ��^�[��������΃J�E���^�͂O�ɖ߂�j
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_EchoVoice( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_EchoVoice },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_EchoVoice( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_WAZAREC* rec = BTL_SVFTOOL_GetWazaRecord( flowWk );
    WazaID  wazaID = BTL_EVENT_FACTOR_GetSubID( myHandle );
    int turn_cnt = BTL_SVFTOOL_GetTurnCount( flowWk ) - 1;
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
      case 0:   pow =  40; break;
      case 1:   pow =  80; break;
      case 2:   pow = 120; break;
      case 3:   pow = 160; break;
      default:  pow = 200; break;
      }
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ����������
 *
 * �O�̃^�[���ɖ������Ђ񎀂ɂȂ��Ă�����З͂��{�ɂȂ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Katakiuti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_Katakiuti },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Katakiuti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_DEADREC* rec = BTL_SVFTOOL_GetDeadRecord( flowWk );
    u8 deadPokeCnt, deadPokeID, i;

    // �O�̃^�[���ɖ���������ł�����З͂�{
    deadPokeCnt = BTL_DEADREC_GetCount( rec, 1 );
    BTL_Printf("�O�^�[���Ɏ��񂾃|�P��=%d\n", deadPokeCnt);
    for(i=0; i<deadPokeCnt; ++i)
    {
      deadPokeID = BTL_DEADREC_GetPokeID( rec, 1, i );
      BTL_Printf("�O�^�[���Ɏ��񂾃|�PID=%d\n", deadPokeID);
      if( BTL_MAINUTIL_IsFriendPokeID( pokeID, deadPokeID ) )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
        break;
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �C�J�T�}
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Ikasama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER_PREV,   handler_Ikasama },    // ���U�З̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Ikasama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ����̍U���͂Ōv�Z������
    u8 def_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SWAP_POKEID, def_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂��т���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Mizubitasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Mizubitasi },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
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
static const BtlEventHandlerTable*  ADD_SimpleBeem( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_SimpleBeem   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
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
static const BtlEventHandlerTable*  ADD_NakamaDukuri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_NakamaDukuri   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
    // �Ώۃ|�P�����̂Ƃ������������Ɠ������̂ɏ�������
    for(i=0; i<targetCnt; ++i)
    {
      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TOKUSEI, pokeID );
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
static const BtlEventHandlerTable*  ADD_ClearSmog( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,  handler_ClearSmog   },         // �_���[�W����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_ClearSmog( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    reset_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

    reset_param->poke_cnt = 1;
    reset_param->pokeID[0] = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RankReset );
    HANDEX_STR_AddArg( &msg_param->str, reset_param->pokeID[0] );
  }
}
//----------------------------------------------------------------------------------
/**
 * �₫����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Yakitukusu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,  handler_Yakitukusu   },         // �_���[�W����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Yakitukusu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 defPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, defPokeID );
    u16 itemID = BPP_GetItem( target );
    if( ITEM_CheckNuts(itemID) )
    {
      BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );

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
 * �Ƃ����Ȃ��E�h���S���e�[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_TomoeNage( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,  handler_TomoeNage   },         // �_���[�W����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_TomoeNage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_PUSHOUT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_PUSHOUT, pokeID );

    param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  }
}
//----------------------------------------------------------------------------------
/**
 * �������Ƃ�
 */
//----------------------------------------------------------------------------------

static const BtlEventHandlerTable*  ADD_Utiotosu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,  handler_Utiotosu               },   // �_���[�W����
    { BTL_EVENT_CHECK_POKE_HIDE,    handler_Kaminari_checkHide     },  // �����|�P�q�b�g�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W����n���h��
static void handler_Utiotosu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    u8 msgFlag = FALSE;

    if( BTL_SVFTOOL_IsFlyingPoke(flowWk, targetPokeID) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

      param->poke_cnt = 1;
      param->pokeID[0] = targetPokeID;
      param->sickID = WAZASICK_FLYING_CANCEL;
      param->sickCont = BPP_SICKCONT_MakePermanent();
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Utiotosu );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID );
      msgFlag = TRUE;
    }

    {
      const BTL_POKEPARAM* bppTarget = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
      if( BPP_CONTFLAG_Get(bppTarget, BPP_CONTFLG_SORAWOTOBU) )
      {
        BTL_HANDEX_PARAM_TAMEHIDE_CANCEL* cancel_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_TAMEHIDE_CANCEL, pokeID );
        cancel_param->pokeID = targetPokeID;
        cancel_param->flag = BPP_CONTFLG_SORAWOTOBU;
        if( !msgFlag ){
          HANDEX_STR_Setup( &cancel_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Utiotosu );
          HANDEX_STR_AddArg( &cancel_param->exStr, targetPokeID );
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �������Ԃ�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_KarawoYaburu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_KarawoYaburu },   // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����ރ��U�n���h��
static void handler_KarawoYaburu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

    param->rankType = WAZA_RANKEFF_DEFENCE;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_SP_DEFENCE;
    param->rankVolume = -1;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_ATTACK;
    param->rankVolume = 2;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_SP_ATTACK;
    param->rankVolume = 2;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = WAZA_RANKEFF_AGILITY;
    param->rankVolume = 2;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->fAlmost = TRUE;
  }
}
//----------------------------------------------------------------------------------
/**
 * �~���[�^�C�v
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MirrorType( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_MirrorType },   // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MirrorType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );

    param->pokeID = pokeID;
    param->next_type = BPP_GetPokeType( target );
    param->fStdMsgDisable = TRUE;

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_p = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_p->str, BTL_STRTYPE_SET, BTL_STRID_SET_MirrorType );
      HANDEX_STR_AddArg( &msg_p->str, pokeID );
      HANDEX_STR_AddArg( &msg_p->str, targetPokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �{�f�B�p�[�W
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_BodyPurge( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_RANKEFF_FIXED,  handler_BodyPurge },   // ���U�ɂ�郉���N�������ʊm���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      BTL_HANDEX_PARAM_SET_WEIGHT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_WEIGHT, pokeID );

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
 * �T�C�R�V���b�N
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_PsycoShock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD_PREV,  handler_PsycoShock },   // �h�䑤�\�͒l�v�Z
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �Ȃ��������E�����Ȃ�邬
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_NasiKuzusi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD_PREV,  handler_NasiKuzusi },   // �h�䑤�\�͒l�v�Z
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �����_�[���[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_WonderRoom( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_WonderRoom },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_WonderRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_WONDERROOM) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_WONDERROOM;
      param->cont = BPP_SICKCONT_MakeTurn( 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_WonderRoom_Start );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_REMOVE_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_WONDERROOM;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �}�W�b�N���[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MagicRoom( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_FIELD_EFFECT_CALL, handler_MagicRoom },    // �����ރ��U�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MagicRoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_MAGICROOM) )
    {
      BTL_HANDEX_PARAM_ADD_FLDEFF* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_MAGICROOM;
      param->cont = BPP_SICKCONT_MakeTurn( 5 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_MagicRoom_Start );
    }
    else
    {
      BTL_HANDEX_PARAM_REMOVE_FLDEFF* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_REMOVE_FLDEFF, pokeID );
      param->effect = BTL_FLDEFF_MAGICROOM;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �͂�����ق̂�
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_HajikeruHonoo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_INFO, handler_HajikeruHonoo },    // �_���[�W�����ŏI�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_HajikeruHonoo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 damagedPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    BtlPokePos pos = BTL_SVFTOOL_GetExistFrontPokePos( flowWk, damagedPokeID );
    BtlExPos   exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, pos );
    u8  targetPokeID[ BTL_POSIDX_MAX ];
    u8  targetCnt;
    u8  i;

    targetCnt = BTL_SVFTOOL_ExpandPokeID( flowWk, exPos, targetPokeID );
    for(i=0; i<targetCnt; ++i)
    {
      if( targetPokeID[i] != damagedPokeID )
      {
        const BTL_POKEPARAM* bpp;
        BTL_HANDEX_PARAM_DAMAGE* param;

        bpp = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID[i] );
        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
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
 * �V���N���m�C�Y
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_SyncroNoise( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_SyncroNoise },    // ���U�����`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
 * �M�t�g�p�X
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_GiftPass( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_GiftPass },   // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      param->pokeID = pokeID;
      param->itemID = ITEM_DUMMY_DATA;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      param->pokeID = targetPokeID;
      param->itemID = myItemID;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_GiftPass );
      HANDEX_STR_AddArg( &param->exStr, targetPokeID );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, myItemID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���̂�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Inotigake( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC1,    handler_Inotigake_CalcDamage  },     // �_���[�W�`�F�b�N
    { BTL_EVENT_WAZADMG_SIDE_AFTER,    handler_Inotigake_AfterDamage },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Inotigake_CalcDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 hp = BPP_GetValue( bpp, BPP_HP );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FIX_DAMAGE, hp );
  }
}
static void handler_Inotigake_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_KILL* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    param->pokeID = pokeID;
  }
}
//----------------------------------------------------------------------------------
/**
 * �������ɂǂ���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_OsakiniDouzo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_OsakiniDouzo  },     // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_OsakiniDouzo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_INTR_POKE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_INTR_POKE, pokeID );
    param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_OsakiniDouzo );
    HANDEX_STR_AddArg( &param->exStr, param->pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ����������
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Sakiokuri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA, handler_Sakiokuri  },     // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Sakiokuri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SEND_LAST* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SEND_LAST, pokeID );
    param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Sakiokuri );
    HANDEX_STR_AddArg( &param->exStr, param->pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��񂵂傤
 *
 * ����^�[���ɂ��̃��U���o�����|�P�����̍s���́A�S�ĘA�����ď��������B
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Rinsyou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXE_START,    handler_Rinsyou  },     // ���U�����m��
    { BTL_EVENT_WAZA_POWER,        handler_Rinsyou_Pow },  // �З͌v�Z
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Rinsyou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ���̃^�[���ɂ܂��N���������U���g���Ă��Ȃ���΁A�������U���g���\��̃|�P���J��グ��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_WAZAREC* rec = BTL_SVFTOOL_GetWazaRecord( flowWk );
    WazaID  wazaID = BTL_EVENT_FACTOR_GetSubID( myHandle );
    u32 thisTurn = BTL_SVFTOOL_GetTurnCount( flowWk );
    if( BTL_WAZAREC_GetUsedWazaCount(rec, wazaID, thisTurn) == 1 )
    {
      BTL_HANDEX_PARAM_INTR_WAZA* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_INTR_WAZA, pokeID );
      param->waza = wazaID;
    }
  }
}
static void handler_Rinsyou_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ���̃^�[���ɓ������U���g���Ă���ΈЗ͂��{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_WAZAREC* rec = BTL_SVFTOOL_GetWazaRecord( flowWk );
    WazaID  wazaID = BTL_EVENT_FACTOR_GetSubID( myHandle );
    u32 thisTurn = BTL_SVFTOOL_GetTurnCount( flowWk );
    // ���Ɏ����̃��U�����R�[�h����Ă���̂łP�����z���Ă���Q�Ԗڈȍ~�Ɣ���
    if( BTL_WAZAREC_GetUsedWazaCount(rec, wazaID, thisTurn) > 1)
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(2) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �t�@�X�g�K�[�h
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_FastGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_START,           handler_Mamoru_StartSeq }, // ���U�����J�n
    { BTL_EVENT_WAZA_EXECUTE_CHECK_2ND,  handler_Mamoru_ExeCheck }, // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_WAZA_EXECUTE_FAIL,       handler_Mamoru_ExeFail  }, // ���U�������s�m��
    { BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  handler_FastGuard   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_FastGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BPP_SICK_CONT  cont = BPP_SICKCONT_MakeTurn( 1 );
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
  common_SideEffect( myHandle, flowWk, pokeID, work, side, BTL_SIDEEFF_FASTGUARD, cont, BTL_STRID_STD_FastGuard );
  IncrementMamoruCounter( flowWk, pokeID );
}
//----------------------------------------------------------------------------------
/**
 * �T�C�h�`�F���W
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_SideChange( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORIZE_WAZA,  handler_SideChange   },  // �����ރ��U
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SideChange( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BtlPokePos myPos = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
  u8 target_pokeID = pokeID;
  BtlPokePos targetPos;

  switch( BTL_SVFTOOL_GetRule(flowWk) ){
  case BTL_RULE_DOUBLE:
  case BTL_RULE_ROTATION:
    if( !BTL_SVFTOOL_IsMultiMode(flowWk) )
    {
      targetPos = BTL_MAINUTIL_GetFriendPokePos( myPos, 0 );
      if( targetPos == myPos ){
        targetPos = BTL_MAINUTIL_GetFriendPokePos( myPos, 1 );
      }
      target_pokeID = BTL_SVFTOOL_PokePosToPokeID( flowWk, targetPos );
    }
    break;
  case BTL_RULE_TRIPLE:
    if( !BTL_MAINUTIL_IsTripleCenterPos(myPos) )
    {
      targetPos = BTL_MAINUTIL_GetFriendPokePos( myPos, 0 );
      if( targetPos == myPos ){
        targetPos = BTL_MAINUTIL_GetFriendPokePos( myPos, 2 );
      }
      target_pokeID = BTL_SVFTOOL_PokePosToPokeID( flowWk, targetPos );
    }
    break;
  }

  if( (target_pokeID != pokeID) )
  {
    BTL_HANDEX_PARAM_SWAP_POKE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SWAP_POKE, pokeID );
    param->pokeID1 = pokeID;
    param->pokeID2 = target_pokeID;
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_SideChange );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
    HANDEX_STR_AddArg( &param->exStr, target_pokeID );
  }
}
//----------------------------------------------------------------------------------
/**
 * �e���L�l�V�X
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Telekinesis( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_SPECIAL,  handler_Telekinesis   },  // �����Ԉُ�
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Telekinesis( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_STR_PARAMS* str = (BTL_HANDEX_STR_PARAMS*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, WAZASICK_TELEKINESIS );
    HANDEX_STR_Setup( str, BTL_STRTYPE_SET, BTL_STRID_SET_Telekinesis );
    HANDEX_STR_AddArg( str, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
  }
}
//----------------------------------------------------------------------------------
/**
 * �t���[�t�H�[��
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_FreeFall( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TAME_START,           handler_FreeFall_TameStart   },   // ���ߊJ�n
    { BTL_EVENT_TAME_RELEASE,         handler_FreeFall_TameRelease },   // ���߉��
    { BTL_EVENT_NOEFFECT_CHECK_L2,    handler_FreeFall_TypeCheck   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_FreeFall_TameStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param;
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = targetPokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;

    {
      BTL_HANDEX_PARAM_ADD_SICK* sick_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      sick_param->poke_cnt = 1;
      sick_param->pokeID[0] = targetPokeID;
      sick_param->sickID = WAZASICK_FREEFALL;
      sick_param->sickCont = BPP_SICKCONT_MakePoke( pokeID );
    }

    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_FreeFall );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, targetPokeID );
    }
  }
}
static void handler_FreeFall_TameRelease( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param;
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RESET_CONTFLAG, pokeID );
    param->pokeID = targetPokeID;
    param->flag = BPP_CONTFLG_SORAWOTOBU;

    {
      BTL_HANDEX_PARAM_CURE_SICK* cure_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      cure_param->poke_cnt = 1;
      cure_param->pokeID[0] = targetPokeID;
      cure_param->sickCode = WAZASICK_FREEFALL;
    }
  }
}
static void handler_FreeFall_TypeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ��s�^�C�v�ւ͍U����������Ȃ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BPP_IsMatchType(target, POKETYPE_HIKOU) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���ɂ����̂���
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_InisieNoUta( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,    handler_InisieNoUta   },   // �_���[�W�����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_InisieNoUta( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

  if( BPP_GetMonsNo(bpp) == MONSNO_MERODHIA )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK)==pokeID )
    {
      if( BPP_GetValue(bpp, BPP_FORM) == FORMNO_MERODHIA_VOICE )
      {
        BTL_HANDEX_PARAM_CHANGE_FORM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_FORM, pokeID );
        param->pokeID = pokeID;
        param->formNo = FORMNO_MERODHIA_STEP;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_ChangeForm );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      }
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * �~���N���A�C
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MiracleEye( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM, handler_MiracleEye },    // ��Ԉُ�p�����[�^�����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ��Ԉُ�p�����[�^�����n���h��
static void handler_MiracleEye( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BPP_SICK_CONT cont;
    cont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
    BPP_SICKCONT_AddParam( &cont, POKETYPE_AKU );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}
//----------------------------------------------------------------------------------
/**
 * �������傤
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_Seityou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_GET_RANKEFF_VALUE, handler_Seityou },    // �����N�������ʒ����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����N�������ʒ����n���h��
static void handler_Seityou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_SVFTOOL_GetWeather(flowWk) == BTL_WEATHER_SHINE )
    {
      int volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
      if( volume == 1 ){
        ++volume;
      }
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, volume );
    }
  }
}

//-----------------------------------------------------------------------------------------------
// ���̃��U�֘A�n���h�����T�u���[�`��
//-----------------------------------------------------------------------------------------------


static CombiEffectType GetCombiWazaType( WazaID waza1, WazaID waza2 )
{
  static const struct {
    WazaID  waza1;
    WazaID  waza2;
    CombiEffectType  effect;
  }CombiTbl[] = {
    { WAZANO_KARI_MIZUNOTIKAI, WAZANO_KARI_HONOONOTIKAI,    COMBI_EFFECT_RAINBOW },
    { WAZANO_KARI_KUSANOTIKAI, WAZANO_KARI_HONOONOTIKAI,    COMBI_EFFECT_BURNING },
    { WAZANO_KARI_MIZUNOTIKAI, WAZANO_KARI_KUSANOTIKAI,     COMBI_EFFECT_MOOR    },
  };
  u32 i;

  for(i=0; i<NELEMS(CombiTbl); ++i)
  {
    if( ((CombiTbl[i].waza1 == waza1) && (CombiTbl[i].waza2 == waza2))
    ||  ((CombiTbl[i].waza1 == waza2) && (CombiTbl[i].waza2 == waza1))
    ){
      return CombiTbl[ i ].effect;
    }
  }
  return COMBI_EFFECT_NULL;
}
//----------------------------------------------------------------------------------
/**
 * �݂��̂�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_MizuNoTikai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_COMBIWAZA_CHECK, handler_CombiWaza_CheckExe  },    // ���̃��U�����`�F�b�N
    { BTL_EVENT_WAZA_EXE_DECIDE, handler_CombiWaza_Decide    },    // ���U�o���m��
    { BTL_EVENT_WAZA_PARAM,      handler_CombiWaza_WazaParam },    // ���U�p�����[�^�`�F�b�N
    { BTL_EVENT_WAZA_POWER,      handler_CombiWaza_Pow       },    // �З͌v�Z
    { BTL_EVENT_WAZADMG_SIDE_AFTER,  handler_CombiWaza_AfterDmg  },    // �_���[�W�����I����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//----------------------------------------------------------------------------------
/**
 * �ق̂��̂�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_HonooNoTikai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_COMBIWAZA_CHECK, handler_CombiWaza_CheckExe  },    // ���̃��U�����`�F�b�N
    { BTL_EVENT_WAZA_EXE_DECIDE, handler_CombiWaza_Decide    },    // ���U�o���m��
    { BTL_EVENT_WAZA_PARAM,      handler_CombiWaza_WazaParam },    // ���U�p�����[�^�`�F�b�N
    { BTL_EVENT_WAZA_POWER,      handler_CombiWaza_Pow       },    // �З͌v�Z
    { BTL_EVENT_WAZADMG_SIDE_AFTER,  handler_CombiWaza_AfterDmg  },    // �_���[�W�����I����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

//----------------------------------------------------------------------------------
/**
 * �����̂�����
 */
//----------------------------------------------------------------------------------
static const BtlEventHandlerTable*  ADD_KusaNoTikai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_COMBIWAZA_CHECK, handler_CombiWaza_CheckExe  },    // ���̃��U�����`�F�b�N
    { BTL_EVENT_WAZA_EXE_DECIDE, handler_CombiWaza_Decide    },    // ���U�o���m��
    { BTL_EVENT_WAZA_PARAM,      handler_CombiWaza_WazaParam },    // ���U�p�����[�^�`�F�b�N
    { BTL_EVENT_WAZA_POWER,      handler_CombiWaza_Pow       },    // �З͌v�Z
    { BTL_EVENT_WAZADMG_SIDE_AFTER,  handler_CombiWaza_AfterDmg  },    // �_���[�W�����I����
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}


// �����`�F�b�N�n���h��
static void handler_CombiWaza_CheckExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    WazaID  combiWazaID;
    u8 combiPokeID;

    if( BPP_CombiWaza_GetParam(bpp, &combiPokeID, &combiWazaID) )
    {
      CombiEffectType  effectType = GetCombiWazaType( BTL_EVENT_FACTOR_GetSubID(myHandle), combiWazaID );
      BTL_N_Printf( DBGSTR_HANDWAZA_CombiWazaExe, pokeID, combiPokeID, combiWazaID, effectType );
      if( effectType != COMBI_EFFECT_NULL )
      {
        work[ 0 ] = 1;  // ���̔����t���O
        work[ 1 ] = effectType;
        work[ 2 ] = combiPokeID;
      }
    }
  }
}
// ���U�o���m��n���h��
static void handler_CombiWaza_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_CombiWazaExe );
//      HANDEX_STR_AddArg( &param->str, work[2] );
    }
  }
}
// ���U�p�����[�^�`�F�b�N�n���h��
static void handler_CombiWaza_WazaParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  {
    if( work[0] )
    {
      PokeType  type = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      switch( work[1] ){
      case COMBI_EFFECT_RAINBOW:   type = POKETYPE_MIZU; break;
      case COMBI_EFFECT_BURNING:   type = POKETYPE_HONOO; break;
      case COMBI_EFFECT_MOOR:      type = POKETYPE_KUSA; break;
      }
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
    }
  }
}
// ���U�З͌v�Z�n���h��
static void handler_CombiWaza_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ���̔������Ă�����З�=150
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, 150 );
    }
  }
}
// �_���[�W�����I����n���h��
static void handler_CombiWaza_AfterDmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] )
    {
      BtlSideEffect eff = BTL_SIDEEFF_NULL;
      BtlSide side = BTL_MAINUTIL_PokeIDtoSide( pokeID );
      u16 strID = 0;

      switch( work[1] ){
      case COMBI_EFFECT_RAINBOW:
          eff = BTL_SIDEEFF_RAINBOW;
          strID = BTL_STRID_STD_Rainbow;
          break;
      case COMBI_EFFECT_BURNING:
          eff = BTL_SIDEEFF_BURNING;
          strID = BTL_STRID_STD_Burning;
          side = BTL_MAINUTIL_GetOpponentSide( side );
          break;
      case COMBI_EFFECT_MOOR:
          eff = BTL_SIDEEFF_MOOR;
          strID = BTL_STRID_STD_Moor;
          side = BTL_MAINUTIL_GetOpponentSide( side );
          break;
      }

      if( eff != BTL_SIDEEFF_NULL )
      {
        BTL_HANDEX_PARAM_SIDEEFF_ADD* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SIDEEFF_ADD, pokeID );
        param->effect = eff;
        param->side = side;
        param->cont = BPP_SICKCONT_MakeTurn( 3 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, strID );
        HANDEX_STR_AddArg( &param->exStr, side );
      }
    }
  }
}


