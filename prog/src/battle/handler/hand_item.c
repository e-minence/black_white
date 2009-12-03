//=============================================================================================
/**
 * @file  hand_item.c
 * @brief ポケモンWB バトルシステム イベントファクター（アイテム）追加処理
 * @author  taya
 *
 * @date  2009.04.21  作成
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

#include "hand_item.h"


enum {
  WORKIDX_TMP_FLAG = EVENT_HANDLER_WORK_ELEMS-1,  ///< 一時利用アイテム
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static s32 common_GetItemParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u16 paramID );
static void handler_KuraboNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KuraboNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KuraboNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KuraboNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KagoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KagoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KagoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KagoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_ChigoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ChigoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ChigoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ChigoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_NanasiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NanasiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NanasiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NanasiNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KiiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KiiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KiiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiiNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_MomonNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MomonNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MomonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MomonNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_RamNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_RamNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_RamNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RamNomi( u16 pri, u16 itemID, u8 pokeID );
static void common_sickReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
static void common_useForSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
static BOOL common_sickcode_match( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HimeriNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_HimeriNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HimeriNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HimeriNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OrenNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_OrenNomi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_OrenNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KinomiJuice( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ObonNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_ObonNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ObonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FiraNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UiNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MagoNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BanziNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_IaNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TiiraNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_TiiraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_TiiraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RyugaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_RyugaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_RyugaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KamuraNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KamuraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KamuraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YatapiNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_YatapiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_YatapiNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ZuaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_ZuaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ZuaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SanNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_SanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SanNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SutaaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_SutaaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SutaaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_PinchRankup( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaRankEffect rankType, u8 volume );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NazoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_NazoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NazoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OkkaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_OkkaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ItokeNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_ItokeNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SokunoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_SokunoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RindoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_RindoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YacheNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_YacheNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YopuNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_YopuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BiarNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_BiarNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SyukaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_SyukaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BakouNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_BakouNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UtanNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_UtanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TangaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_TangaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YorogiNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_YorogiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KasibuNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KasibuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HabanNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_HabanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NamoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_NamoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RiribaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_RiribaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HozuNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_HozuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL common_WeakAff_Relieve( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType type );
static void handler_common_WeakAff_DmgAfter( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_PinchReactCommon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_DamageReactCore( BTL_SVFLOW_WORK* flowWk, u8 pokeID, u8 n );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SiroiHerb( u16 pri, u16 itemID, u8 pokeID );
static void handler_SiroiHerb_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SiroiHerb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SiroiHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MentalHerb( u16 pri, u16 itemID, u8 pokeID );
static void handler_MentalHerb_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MentalHerb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MentalHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HikarinoKona( u16 pri, u16 itemID, u8 pokeID );
static void handler_HikarinoKona( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KyouseiGipusu( u16 pri, u16 itemID, u8 pokeID );
static void handler_KyouseiGipusu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SenseiNoTume( u16 pri, u16 itemID, u8 pokeID );
static void handler_SenseiNoTume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KoukouNoSippo( u16 pri, u16 itemID, u8 pokeID );
static void handler_KoukouNoSippo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OujaNoSirusi( u16 pri, u16 itemID, u8 pokeID );
static void handler_OujaNoSirusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_OujaNoSirusi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SurudoiTume( u16 pri, u16 itemID, u8 pokeID );
static void handler_SurudoiTume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SurudoiTume_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KoukakuLens( u16 pri, u16 itemID, u8 pokeID );
static void handler_KoukakuLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_PintLens( u16 pri, u16 itemID, u8 pokeID );
static void handler_PintLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_LuckyPunch( u16 pri, u16 itemID, u8 pokeID );
static void handler_LuckyPunch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Naganegi( u16 pri, u16 itemID, u8 pokeID );
static void handler_Naganegi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FocusLens( u16 pri, u16 itemID, u8 pokeID );
static void handler_FocusLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NonkiNoOkou( u16 pri, u16 itemID, u8 pokeID );
static void handler_NonkiNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KemuriDama( u16 pri, u16 itemID, u8 pokeID );
static void handler_KemuriDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ChikaraNoHachimaki( u16 pri, u16 itemID, u8 pokeID );
static void handler_ChikaraNoHachimaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MonosiriMegane( u16 pri, u16 itemID, u8 pokeID );
static void handler_MonosiriMegane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SinkaiNoKiba( u16 pri, u16 itemID, u8 pokeID );
static void handler_SinkaiNoKiba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SinkaiNoUroko( u16 pri, u16 itemID, u8 pokeID );
static void handler_SinkaiNoUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MetalPowder( u16 pri, u16 itemID, u8 pokeID );
static void handler_MetalPowder( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SpeedPowder( u16 pri, u16 itemID, u8 pokeID );
static void handler_SpeedPowder( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DenkiDama( u16 pri, u16 itemID, u8 pokeID );
static void handler_DenkiDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DenkiDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KokoroNoSizuku( u16 pri, u16 itemID, u8 pokeID );
static void handler_KokoroNoSizuku_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KokoroNoSizuku_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FutoiHone( u16 pri, u16 itemID, u8 pokeID );
static void handler_FutoiHone( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KodawariHachimaki( u16 pri, u16 itemID, u8 pokeID );
static void handler_KodawariHachimaki_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KodawariMegane( u16 pri, u16 itemID, u8 pokeID );
static void handler_KodawariMegane_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KodawariScarf( u16 pri, u16 itemID, u8 pokeID );
static void handler_KodawariScarf( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kodawari_Common( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiaiNoTasuki( u16 pri, u16 itemID, u8 pokeID );
static void handler_KiaiNoTasuki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiaiNoHachimaki( u16 pri, u16 itemID, u8 pokeID );
static void handler_KiaiNoHachimaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TatsujinNoObi( u16 pri, u16 itemID, u8 pokeID );
static void handler_TatsujinNoObi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_InochiNoTama( u16 pri, u16 itemID, u8 pokeID );
static void handler_InochiNoTama_KickBack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_InochiNoTama_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MetroNome( u16 pri, u16 itemID, u8 pokeID );
static void handler_MetroNome( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NebariNoKagidume( u16 pri, u16 itemID, u8 pokeID );
static void handler_NebariNoKagidume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KaigaraNoSuzu( u16 pri, u16 itemID, u8 pokeID );
static void handler_KaigaraNoSuzu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Tabenokosi( u16 pri, u16 itemID, u8 pokeID );
static void handler_Tabenokosi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tabenokosi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DokudokuDama( u16 pri, u16 itemID, u8 pokeID );
static void handler_DokudokuDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DokudokuDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KaenDama( u16 pri, u16 itemID, u8 pokeID );
static void handler_KaenDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KaenDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_GinNoKona( u16 pri, u16 itemID, u8 pokeID );
static void handler_GinNoKona( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YawarakaiSuna( u16 pri, u16 itemID, u8 pokeID );
static void handler_YawarakaiSuna( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KataiIsi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KataiIsi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KisekiNoTane( u16 pri, u16 itemID, u8 pokeID );
static void handler_KisekiNoTane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KuroiMegane( u16 pri, u16 itemID, u8 pokeID );
static void handler_KuroiMegane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Kuroobi( u16 pri, u16 itemID, u8 pokeID );
static void handler_Kuroobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Zisyaku( u16 pri, u16 itemID, u8 pokeID );
static void handler_Zisyaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MetalCoat( u16 pri, u16 itemID, u8 pokeID );
static void handler_MetalCoat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SinpiNoSizuku( u16 pri, u16 itemID, u8 pokeID );
static void handler_SinpiNoSizuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SurudoiKutibasi( u16 pri, u16 itemID, u8 pokeID );
static void handler_SurudoiKutibasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Dokubari( u16 pri, u16 itemID, u8 pokeID );
static void handler_Dokubari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Dokubari_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TokenaiKoori( u16 pri, u16 itemID, u8 pokeID );
static void handler_TokenaiKoori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NoroiNoOfuda( u16 pri, u16 itemID, u8 pokeID );
static void handler_NoroiNoOfuda( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MagattaSupuun( u16 pri, u16 itemID, u8 pokeID );
static void handler_MagattaSupuun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Mokutan( u16 pri, u16 itemID, u8 pokeID );
static void handler_Mokutan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RyuunoKiba( u16 pri, u16 itemID, u8 pokeID );
static void handler_RyuunoKiba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SirukuNoSukaafu( u16 pri, u16 itemID, u8 pokeID );
static void handler_SirukuNoSukaafu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_AyasiiOkou( u16 pri, u16 itemID, u8 pokeID );
static void handler_AyasiiOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_GansekiOkou( u16 pri, u16 itemID, u8 pokeID );
static void handler_GansekiOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SazanamiNoOkou( u16 pri, u16 itemID, u8 pokeID );
static void handler_SazanamiNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UsioNoOkou( u16 pri, u16 itemID, u8 pokeID );
static void handler_UsioNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OhanaNoOkou( u16 pri, u16 itemID, u8 pokeID );
static void handler_OhanaNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_PowerUpSpecificType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType type );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_GotugotuMet( u16 pri, u16 itemID, u8 pokeID );
static void handler_GotugotuMet( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Huusen( u16 pri, u16 itemID, u8 pokeID );
static void handler_Huusen_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Huusen_CheckFlying( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Huusen_DamageReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Huusen_ItemSetFixed( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RedCard( u16 pri, u16 itemID, u8 pokeID );
static void handler_RedCard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NerainoMato( u16 pri, u16 itemID, u8 pokeID );
static void handler_NerainoMato( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SimetukeBand( u16 pri, u16 itemID, u8 pokeID );
static void handler_SimetukeBand( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Kyuukon( u16 pri, u16 itemID, u8 pokeID );
static void handler_Kyuukon_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kyuukon_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Juudenti( u16 pri, u16 itemID, u8 pokeID );
static void handler_Juudenti_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Juudenti_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DassyutuPod( u16 pri, u16 itemID, u8 pokeID );
static void handler_DassyutuPod_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DassyutuPod_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_Juel_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type );
static void common_Juel_DmgDetermine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HonooNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_HonooNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HonooNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MizuNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_MizuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MizuNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DenkiNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_DenkiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DenkiNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KusaNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_KusaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KusaNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KooriNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_KooriNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KooriNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KakutouJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_KakutouJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KakutouJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DokuNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_DokuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DokuNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_JimenNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_JimenNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_JimenNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HikouNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_HikouNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HikouNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_EsperJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_EsperJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_EsperJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MusiNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_MusiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MusiNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_IwaNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_IwaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_IwaNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_GhostJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_GhostJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_GhostJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DragonJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_DragonNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DragonNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_AkuNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_AkuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_AkuNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HaganeNoJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_HaganeNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HaganeNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NormalJuel( u16 pri, u16 itemID, u8 pokeID );
static void handler_NormalJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NormalJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );



// イベントハンドラ登録関数の型定義
typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, u16 itemID, u8 pokeID );

static const struct {
  u32             itemID;
  pEventAddFunc   func;
}EventAddFuncTbl[] = {
  { ITEM_KURABONOMI,    HAND_ADD_ITEM_KuraboNomi  },
  { ITEM_KAGONOMI,      HAND_ADD_ITEM_KagoNomi    },
  { ITEM_TIIGONOMI,     HAND_ADD_ITEM_ChigoNomi   },
  { ITEM_NANASINOMI,    HAND_ADD_ITEM_NanasiNomi  },
  { ITEM_KIINOMI,       HAND_ADD_ITEM_KiiNomi     },
  { ITEM_MOMONNOMI,     HAND_ADD_ITEM_MomonNomi   },
  { ITEM_RAMUNOMI,      HAND_ADD_ITEM_RamNomi     },
  { ITEM_HIMERINOMI,    HAND_ADD_ITEM_HimeriNomi  },
  { ITEM_ORENNOMI,      HAND_ADD_ITEM_OrenNomi    },
  { ITEM_KINOMIZYUUSU,  HAND_ADD_ITEM_KinomiJuice },
  { ITEM_OBONNOMI,      HAND_ADD_ITEM_ObonNomi    },
  { ITEM_FIRANOMI,      HAND_ADD_ITEM_FiraNomi    },
  { ITEM_UINOMI,        HAND_ADD_ITEM_UiNomi      },
  { ITEM_MAGONOMI,      HAND_ADD_ITEM_MagoNomi    },
  { ITEM_BANZINOMI,     HAND_ADD_ITEM_BanziNomi   },
  { ITEM_IANOMI,        HAND_ADD_ITEM_IaNomi      },
  { ITEM_TIIRANOMI,     HAND_ADD_ITEM_TiiraNomi   },
  { ITEM_RYUGANOMI,     HAND_ADD_ITEM_RyugaNomi   },
  { ITEM_KAMURANOMI,    HAND_ADD_ITEM_KamuraNomi  },
  { ITEM_YATAPINOMI,    HAND_ADD_ITEM_YatapiNomi  },
  { ITEM_ZUANOMI,       HAND_ADD_ITEM_ZuaNomi     },
  { ITEM_SANNOMI,       HAND_ADD_ITEM_SanNomi     },
  { ITEM_SUTAANOMI,     HAND_ADD_ITEM_SutaaNomi   },
  { ITEM_NAZONOMI,      HAND_ADD_ITEM_NazoNomi    },
  { ITEM_OKKANOMI,      HAND_ADD_ITEM_OkkaNomi    },
  { ITEM_ITOKENOMI,     HAND_ADD_ITEM_ItokeNomi   },
  { ITEM_SOKUNONOMI,    HAND_ADD_ITEM_SokunoNomi  },
  { ITEM_RINDONOMI,     HAND_ADD_ITEM_RindoNomi   },
  { ITEM_YATHENOMI,     HAND_ADD_ITEM_YacheNomi   },
  { ITEM_YOPUNOMI,      HAND_ADD_ITEM_YopuNomi    },
  { ITEM_BIAANOMI,      HAND_ADD_ITEM_BiarNomi    },
  { ITEM_SYUKANOMI,     HAND_ADD_ITEM_SyukaNomi   },
  { ITEM_BAKOUNOMI,     HAND_ADD_ITEM_BakouNomi   },
  { ITEM_UTANNOMI,      HAND_ADD_ITEM_UtanNomi    },
  { ITEM_TANGANOMI,     HAND_ADD_ITEM_TangaNomi   },
  { ITEM_YOROGINOMI,    HAND_ADD_ITEM_YorogiNomi  },
  { ITEM_KASIBUNOMI,    HAND_ADD_ITEM_KasibuNomi  },
  { ITEM_HABANNOMI,     HAND_ADD_ITEM_HabanNomi   },
  { ITEM_NAMONOMI,      HAND_ADD_ITEM_NamoNomi    },
  { ITEM_RIRIBANOMI,    HAND_ADD_ITEM_RiribaNomi  },
  { ITEM_HOZUNOMI,      HAND_ADD_ITEM_HozuNomi    },
  { ITEM_SIROIHAABU,        HAND_ADD_ITEM_SiroiHerb         },
  { ITEM_MENTARUHAABU,      HAND_ADD_ITEM_MentalHerb        },
  { ITEM_KEMURIDAMA,        HAND_ADD_ITEM_KemuriDama        },
  { ITEM_HIKARINOKONA,      HAND_ADD_ITEM_HikarinoKona      },
  { ITEM_KYOUSEIGIPUSU,     HAND_ADD_ITEM_KyouseiGipusu     },
  { ITEM_SENSEINOTUME,      HAND_ADD_ITEM_SenseiNoTume      },
  { ITEM_KOUKOUNOSIPPO,     HAND_ADD_ITEM_KoukouNoSippo     },
  { ITEM_MANPUKUOKOU,       HAND_ADD_ITEM_KoukouNoSippo     },  // まんぷくおこう=こうこうのしっぽ等価
  { ITEM_OUZYANOSIRUSI,     HAND_ADD_ITEM_OujaNoSirusi      },
  { ITEM_SURUDOITUME,       HAND_ADD_ITEM_SurudoiTume       },
  { ITEM_KOUKAKURENZU,      HAND_ADD_ITEM_KoukakuLens       },
  { ITEM_PINTORENZU,        HAND_ADD_ITEM_PintLens          },
  { ITEM_FOOKASURENZU,      HAND_ADD_ITEM_FocusLens         },
  { ITEM_NONKINOOKOU,       HAND_ADD_ITEM_NonkiNoOkou       },
  { ITEM_TIKARANOHATIMAKI,  HAND_ADD_ITEM_ChikaraNoHachimaki},
  { ITEM_MONOSIRIMEGANE,    HAND_ADD_ITEM_MonosiriMegane    },
  { ITEM_SINKAINOKIBA,      HAND_ADD_ITEM_SinkaiNoKiba      },
  { ITEM_SINKAINOUROKO,     HAND_ADD_ITEM_SinkaiNoUroko     },
  { ITEM_METARUPAUDAA,      HAND_ADD_ITEM_MetalPowder       },
  { ITEM_SUPIIDOPAUDAA,     HAND_ADD_ITEM_SpeedPowder       },
  { ITEM_DENKIDAMA,         HAND_ADD_ITEM_DenkiDama         },
  { ITEM_RAKKIIPANTI,       HAND_ADD_ITEM_LuckyPunch        },
  { ITEM_NAGANEGI,          HAND_ADD_ITEM_Naganegi          },
  { ITEM_KOKORONOSIZUKU,    HAND_ADD_ITEM_KokoroNoSizuku    },
  { ITEM_HUTOIHONE,         HAND_ADD_ITEM_FutoiHone         },
  { ITEM_KODAWARIHATIMAKI,  HAND_ADD_ITEM_KodawariHachimaki },
  { ITEM_KODAWARIMEGANE,    HAND_ADD_ITEM_KodawariMegane },
  { ITEM_KODAWARISUKAAHU,   HAND_ADD_ITEM_KodawariScarf },
  { ITEM_GINNOKONA,         HAND_ADD_ITEM_GinNoKona       },
  { ITEM_YAWARAKAISUNA,     HAND_ADD_ITEM_YawarakaiSuna   },
  { ITEM_KATAIISI,          HAND_ADD_ITEM_KataiIsi        },
  { ITEM_KISEKINOTANE,      HAND_ADD_ITEM_KisekiNoTane    },
  { ITEM_KUROIMEGANE,       HAND_ADD_ITEM_KuroiMegane     },
  { ITEM_KUROOBI,           HAND_ADD_ITEM_Kuroobi         },
  { ITEM_ZISYAKU,           HAND_ADD_ITEM_Zisyaku         },
  { ITEM_METARUKOOTO,       HAND_ADD_ITEM_MetalCoat       },
  { ITEM_SINPINOSIZUKU,     HAND_ADD_ITEM_SinpiNoSizuku   },
  { ITEM_SURUDOIKUTIBASI,   HAND_ADD_ITEM_SurudoiKutibasi },
  { ITEM_DOKUBARI,          HAND_ADD_ITEM_Dokubari        },
  { ITEM_TOKENAIKOORI,      HAND_ADD_ITEM_TokenaiKoori    },
  { ITEM_NOROINOOHUDA,      HAND_ADD_ITEM_NoroiNoOfuda    },
  { ITEM_MAGATTASUPUUN,     HAND_ADD_ITEM_MagattaSupuun   },
  { ITEM_MOKUTAN,           HAND_ADD_ITEM_Mokutan         },
  { ITEM_RYUUNOKIBA,        HAND_ADD_ITEM_RyuunoKiba      },
  { ITEM_SIRUKUNOSUKAAHU,   HAND_ADD_ITEM_SirukuNoSukaafu },
  { ITEM_AYASIIOKOU,        HAND_ADD_ITEM_AyasiiOkou      },
  { ITEM_GANSEKIOKOU,       HAND_ADD_ITEM_GansekiOkou     },
  { ITEM_SAZANAMINOOKOU,    HAND_ADD_ITEM_SazanamiNoOkou  },
  { ITEM_OHANANOOKOU,       HAND_ADD_ITEM_OhanaNoOkou     },
  { ITEM_KIAINOTASUKI,      HAND_ADD_ITEM_KiaiNoTasuki    },
  { ITEM_KIAINOHATIMAKI,    HAND_ADD_ITEM_KiaiNoHachimaki },
  { ITEM_TATUZINNOOBI,      HAND_ADD_ITEM_TatsujinNoObi   },
  { ITEM_INOTINOTAMA,       HAND_ADD_ITEM_InochiNoTama    },
  { ITEM_METORONOOMU,       HAND_ADD_ITEM_MetroNome       },
  { ITEM_NEBARINOKAGIDUME,  HAND_ADD_ITEM_NebariNoKagidume},
  { ITEM_KAIGARANOSUZU,     HAND_ADD_ITEM_KaigaraNoSuzu   },
  { ITEM_TABENOKOSI,        HAND_ADD_ITEM_Tabenokosi      },
  { ITEM_DOKUDOKUDAMA,      HAND_ADD_ITEM_DokudokuDama    },
  { ITEM_KAENDAMA,          HAND_ADD_ITEM_KaenDama        },

  { ITEM_GOTUGOTUMETTO,     HAND_ADD_ITEM_GotugotuMet     },  // ゴツゴツメット
  { ITEM_HUUSEN,            HAND_ADD_ITEM_Huusen          },  // ふうせん
  { ITEM_REDDOKAADO,        HAND_ADD_ITEM_RedCard         },  // レッドカード
  { ITEM_NERAINOMATO,       HAND_ADD_ITEM_NerainoMato     },  // ねらいのまと
  { ITEM_SIMETUKEBANDO,     HAND_ADD_ITEM_SimetukeBand    },  // しめつけバンド
  { ITEM_KYUUKON,           HAND_ADD_ITEM_Kyuukon         },  // きゅうこん
  { ITEM_ZYUUDENTI,         HAND_ADD_ITEM_Juudenti        },  // じゅうでんち
  { ITEM_DASYUTUPODDO,      HAND_ADD_ITEM_DassyutuPod     },  // だしゅつポッド
  { ITEM_HONOONOZYUERU,     HAND_ADD_ITEM_HonooNoJuel     },  // ほのおのジュエル
  { ITEM_MIZUNOZYUERU,      HAND_ADD_ITEM_MizuNoJuel      },  // みずのジュエル
  { ITEM_DENKINOZYUERU,     HAND_ADD_ITEM_DenkiNoJuel     },  // でんきのジュエル
  { ITEM_KUSANOZYUERU,      HAND_ADD_ITEM_KusaNoJuel      },  // くさのジュエル
  { ITEM_KOORINOZYUERU,     HAND_ADD_ITEM_KooriNoJuel     },  // こおりのジュエル
  { ITEM_KAKUTOUZYUERU,     HAND_ADD_ITEM_KakutouJuel     },  // かくとうジュエル
  { ITEM_DOKUNOZYUERU,      HAND_ADD_ITEM_DokuNoJuel      },  // どくのジュエル
  { ITEM_ZIMENNOZYUERU,     HAND_ADD_ITEM_JimenNoJuel     },  // じめんのジュエル
  { ITEM_HIKOUNOZYUERU,     HAND_ADD_ITEM_HikouNoJuel     },  // ひこうのジュエル
  { ITEM_ESUPAAZYUERU,      HAND_ADD_ITEM_EsperJuel       },  // エスパージュエル
  { ITEM_MUSINOZYUERU,      HAND_ADD_ITEM_MusiNoJuel      },  // むしのジュエル
  { ITEM_IWANOZYUERU,       HAND_ADD_ITEM_IwaNoJuel       },  // いわのジュエル
  { ITEM_GOOSUTOZYUERU,     HAND_ADD_ITEM_GhostJuel       },  // ゴーストジュエル
  { ITEM_DORAGONZYUERU,     HAND_ADD_ITEM_DragonJuel      },  // ドラゴンジュエル
  { ITEM_AKUNOZYUERU,       HAND_ADD_ITEM_AkuNoJuel       },  // あくのジュエル
  { ITEM_HAGANENOZYUERU,    HAND_ADD_ITEM_HaganeNoJuel    },  // はがねのジュエル
  { ITEM_NOOMARUZYUERU,     HAND_ADD_ITEM_NormalJuel      },  // ノーマルジュエル


  { 0, NULL },
};

//=============================================================================================
/**
 * ポケモンの装備しているアイテムのハンドラを追加
 *
 * @param   bpp
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  BTL_HANDLER_ITEM_Add( const BTL_POKEPARAM* bpp )
{
  u32 itemID = BPP_GetItem( bpp );
  if( itemID != ITEM_DUMMY_DATA )
  {
    u32 i;
    for(i=0; i<NELEMS(EventAddFuncTbl); ++i)
    {
      if( EventAddFuncTbl[i].itemID == itemID )
      {
        u16 priority = BPP_GetValue_Base( bpp, BPP_AGILITY );
        u8 pokeID = BPP_GetID( bpp );
        return EventAddFuncTbl[i].func( priority, itemID, pokeID );
      }
    }
  }

  return NULL;
}
//=============================================================================================
/**
 * ポケモンの「アイテム」ハンドラをシステムから全て削除
 *
 * @param   pp
 *
 */
//=============================================================================================
void BTL_HANDLER_ITEM_Remove( const BTL_POKEPARAM* pp )
{
  BTL_EVENT_FACTOR* factor;
  u8 pokeID = BPP_GetID( pp );

  while( (factor = BTL_EVENT_SeekFactor(BTL_EVENT_FACTOR_ITEM, pokeID)) != NULL )
  {
    BTL_EVENT_FACTOR_Remove( factor );
  }
}
//=============================================================================================
/**
 * 装備アイテムとは別に一時的なハンドラ追加
 *
 * @param   bpp
 * @param   itemID
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  BTL_HANDLER_ITEM_TMP_Add( const BTL_POKEPARAM* bpp, u16 itemID )
{
  if( itemID != ITEM_DUMMY_DATA )
  {
    u32 i;
    for(i=0; i<NELEMS(EventAddFuncTbl); ++i)
    {
      if( EventAddFuncTbl[i].itemID == itemID )
      {
        u16 priority = BPP_GetValue_Base( bpp, BPP_AGILITY );
        u8 pokeID = BPP_GetID( bpp );
        BTL_EVENT_FACTOR* factor = EventAddFuncTbl[i].func( priority, itemID, pokeID );
        if( factor ){
          BTL_EVENT_FACTOR_SetWorkValue( factor, WORKIDX_TMP_FLAG, TRUE );
          return factor;
        }
      }
    }
  }

  return NULL;
}
//=============================================================================================
/**
 * 一時的な追加ハンドラを削除する
 *
 * @param   bpp
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
void BTL_HANDLER_ITEM_TMP_Remove( BTL_EVENT_FACTOR* factor )
{
  BTL_EVENT_FACTOR_Remove( factor );
}



//----------------------------------------------------------------------------------
/**
 * アイテムパラメータ取得
 *
 * @param   myHandle
 * @param   paramID   パラメータID（item/item.h)
 *
 * @retval  s32
 */
//----------------------------------------------------------------------------------
static s32 common_GetItemParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u16 paramID )
{
  u16     itemID = BTL_EVENT_FACTOR_GetSubID( myHandle );
  return BTL_CALC_ITEM_GetParam( itemID, paramID );
}
//------------------------------------------------------------------------------
/**
 *  クラボのみ
 */
//------------------------------------------------------------------------------
static void handler_KuraboNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_sickReaction( myHandle, flowWk, pokeID, WAZASICK_MAHI );
}
static void handler_KuraboNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_MAHI );
}
static void handler_KuraboNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, WAZASICK_MAHI );
  }
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KuraboNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KuraboNomi },       // 状態異常チェックハンドラ
    { BTL_EVENT_USE_ITEM,            handler_KuraboNomi_Use },   // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_KuraboNomi_UseTmp },// 強制一時使用（ついばむなど）
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  カゴのみ
 */
//------------------------------------------------------------------------------
static void handler_KagoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_sickReaction( myHandle, flowWk, pokeID, WAZASICK_NEMURI );
}
static void handler_KagoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_NEMURI );
}
static void handler_KagoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, WAZASICK_NEMURI );
  }
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KagoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KagoNomi },       // 状態異常チェックハンドラ
    { BTL_EVENT_USE_ITEM,            handler_KagoNomi_Use },   // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_KagoNomi_UseTmp },// 強制一時使用（ついばむなど）
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  チーゴのみ
 */
//------------------------------------------------------------------------------
static void handler_ChigoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_sickReaction( myHandle, flowWk, pokeID, WAZASICK_YAKEDO );
}
static void handler_ChigoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_YAKEDO );
}
static void handler_ChigoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, WAZASICK_YAKEDO );
  }
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ChigoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_ChigoNomi },       // 状態異常チェックハンドラ
    { BTL_EVENT_USE_ITEM,            handler_ChigoNomi_Use },   // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_ChigoNomi_UseTmp },// 強制一時使用（ついばむなど）
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  ナナシのみ
 */
//------------------------------------------------------------------------------
static void handler_NanasiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_sickReaction( myHandle, flowWk, pokeID, WAZASICK_KOORI );
}
static void handler_NanasiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_KOORI );
}
static void handler_NanasiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, WAZASICK_KOORI );
  }
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NanasiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_NanasiNomi },       // 状態異常チェックハンドラ
    { BTL_EVENT_USE_ITEM,            handler_NanasiNomi_Use },   // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_NanasiNomi_UseTmp },// 強制一時使用（ついばむなど）
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  キーのみ
 */
//------------------------------------------------------------------------------
static void handler_KiiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_sickReaction( myHandle, flowWk, pokeID, WAZASICK_KONRAN );
}
static void handler_KiiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_KONRAN );
}
static void handler_KiiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, WAZASICK_KONRAN );
  }
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KiiNomi },       // 状態異常チェックハンドラ
    { BTL_EVENT_USE_ITEM,            handler_KiiNomi_Use },   // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_KiiNomi_UseTmp },// 強制一時使用（ついばむなど）
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  モモンのみ
 */
//------------------------------------------------------------------------------
static void handler_MomonNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_sickReaction( myHandle, flowWk, pokeID, WAZASICK_DOKU );
}
static void handler_MomonNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_DOKU );
}
static void handler_MomonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, WAZASICK_DOKU );
  }
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MomonNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_MomonNomi },       // 状態異常チェックハンドラ
    { BTL_EVENT_USE_ITEM,            handler_MomonNomi_Use },   // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_MomonNomi_UseTmp },// 強制一時使用（ついばむなど）
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  ラムのみ
 */
//------------------------------------------------------------------------------
static void handler_RamNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_sickReaction( myHandle, flowWk, pokeID, WAZASICK_EX_POKEFULL_PLUS );
}
static void handler_RamNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_EX_POKEFULL_PLUS );
}
static void handler_RamNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, WAZASICK_EX_POKEFULL_PLUS );
  }
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RamNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_RamNomi },       // 状態異常チェックハンドラ
    { BTL_EVENT_USE_ITEM,            handler_RamNomi_Use },   // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_RamNomi_UseTmp },// 強制一時使用（ついばむなど）
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//--------------------------------------------------------------------------
/**
 * 状態異常に反応するアイテム群の共通処理
 *
 * @param   flowWk
 * @param   pokeID
 * @param   sickID    反応する状態異常
 *
 */
//--------------------------------------------------------------------------
static void common_sickReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_sickcode_match(flowWk, pokeID, sickCode) ){
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
    }
  }
}
static void common_useForSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode )
{
  if( common_sickcode_match(flowWk, pokeID, sickCode) )
  {
    BTL_HANDEX_PARAM_CURE_SICK* param;
    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
    param->sickCode = sickCode;
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
  }
}
static BOOL common_sickcode_match( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

  switch( sickCode ){
  case WAZASICK_EX_POKEFULL_PLUS:
    if( (BPP_GetPokeSick(bpp) != POKESICK_NULL)
    ||  (BPP_CheckSick(bpp, WAZASICK_KONRAN))
    ){
      return TRUE;
    }
    break;
  case WAZASICK_EX_POKEFULL:
    return  (BPP_GetPokeSick(bpp) != POKESICK_NULL);
    break;
  default:
    return BPP_CheckSick( bpp, sickCode );
  }
  return FALSE;
}
//------------------------------------------------------------------------------
/**
 *  ヒメリのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HimeriNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DECREMENT_PP_DONE,      handler_HimeriNomi },
    { BTL_EVENT_USE_ITEM,               handler_HimeriNomi_Use },
    { BTL_EVENT_USE_ITEM_TMP,           handler_HimeriNomi_UseTmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HimeriNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 waza_idx = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_IDX );
    if( BPP_WAZA_GetPP(bpp, waza_idx) == 0 )
    {
      // work[0] = 対象ワザインデックスの受け渡し用に使う
      work[0] = waza_idx;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }
  }
}
static void handler_HimeriNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_PP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_PP, pokeID );
    param->volume = 10;
    param->pokeID = pokeID;
    param->wazaIdx = work[0];
  }
}
static void handler_HimeriNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 waza_cnt, PP, PPMax, diffMin, minIdx, i;

    waza_cnt = BPP_WAZA_GetCount( bpp );
    for(i=0; i<waza_cnt; ++i)
    {
      BPP_WAZA_GetParticular( bpp, i, &PP, &PPMax );
      if( PP == 0 ){  // 残りPP=0 のワザは優先する
        minIdx = i;
        break;
      }
      if( i == 0 ){
        minIdx = 0;
        diffMin = PPMax - PP;
      }else if( (PPMax - PP) < diffMin ){
        diffMin = PPMax - PP;
        minIdx = i;
      }
    }
    work[0] = minIdx;
    handler_HimeriNomi_Use( myHandle, flowWk, pokeID, work );
  }
}

//------------------------------------------------------------------------------
/**
 *  オレンのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OrenNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use      },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_OrenNomi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DamageReactCore( flowWk, pokeID, 2 );
}
static void handler_OrenNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    param->pokeID = pokeID;
    param->recoverHP = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverHP );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
    HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
  }
}
//------------------------------------------------------------------------------
/**
 *  きのみジュース
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KinomiJuice( u16 pri, u16 itemID, u8 pokeID )
{
  // 全部“オレンのみ”と同じで良い
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  オボンのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ObonNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_ObonNomi_Use },
    { BTL_EVENT_USE_ITEM_TMP,           handler_ObonNomi_UseTmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_ObonNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 maxHP = BPP_GetValue( bpp, BPP_MAX_HP );
    u8 ratio = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );

    param->pokeID = pokeID;
    param->recoverHP = (maxHP * ratio) / 100;
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverHP );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
    HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
  }
}
static void handler_ObonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    handler_ObonNomi_Use( myHandle, flowWk, pokeID, work );
  }
}

//------------------------------------------------------------------------------
/**
 *  フィラのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FiraNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ とりあえずオレンと一緒で
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  ウイのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ とりあえずオレンと一緒で
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  マゴのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MagoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ とりあえずオレンと一緒で
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  バンジのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BanziNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ とりあえずオレンと一緒で
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  イアのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_IaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ とりあえずオレンと一緒で
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *  チイラのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TiiraNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_TiiraNomi },
    { BTL_EVENT_USE_ITEM_TMP,           handler_TiiraNomi_Tmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_TiiraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_ATTACK, 1 );
}
static void handler_TiiraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_TiiraNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  リュガのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RyugaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_RyugaNomi },
    { BTL_EVENT_USE_ITEM_TMP,           handler_RyugaNomi_Tmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_RyugaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_DEFENCE, 1 );
}
static void handler_RyugaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_RyugaNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  カムラのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KamuraNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_KamuraNomi },
    { BTL_EVENT_USE_ITEM_TMP,           handler_KamuraNomi_Tmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KamuraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_AGILITY, 1 );
}
static void handler_KamuraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_KamuraNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  ヤタピのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YatapiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,                    handler_YatapiNomi },
    { BTL_EVENT_USE_ITEM_TMP,                handler_YatapiNomi_Tmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_YatapiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_SP_ATTACK, 1 );
}
static void handler_YatapiNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_YatapiNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  ズアのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ZuaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,                    handler_ZuaNomi },
    { BTL_EVENT_USE_ITEM_TMP,                handler_ZuaNomi_Tmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_ZuaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_SP_DEFENCE, 1 );
}
static void handler_ZuaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_ZuaNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  サンのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SanNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,                    handler_SanNomi },
    { BTL_EVENT_USE_ITEM_TMP,                handler_SanNomi_Tmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_CRITICAL_RATIO, 1 );
}
static void handler_SanNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_SanNomi( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  スターのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SutaaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,                    handler_SutaaNomi },
    { BTL_EVENT_USE_ITEM_TMP,                handler_SutaaNomi_Tmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SutaaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u8 rankType[] = {
    WAZA_RANKEFF_ATTACK, WAZA_RANKEFF_DEFENCE,
    WAZA_RANKEFF_SP_ATTACK, WAZA_RANKEFF_SP_DEFENCE,
    WAZA_RANKEFF_AGILITY,
  };
  u8 idx = GFL_STD_MtRand( NELEMS(rankType) );

  common_PinchRankup( flowWk, pokeID, rankType[idx], 2 );
}
static void handler_SutaaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG ] ){
    handler_SutaaNomi( myHandle, flowWk, pokeID, work );
  }
}

//----------------------------------------------------------------------------------
/**
 * ピンチによって能力ランクアップするどうぐの共通処理
 *
 * @param   flowWk
 * @param   pokeID
 * @param   rankType
 * @param   volume
 */
//----------------------------------------------------------------------------------
static void common_PinchRankup( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaRankEffect rankType, u8 volume )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->rankType = rankType;
    param->rankVolume = volume;
    param->poke_cnt = 1;
    param->fAlmost = TRUE;
    param->pokeID[0] = pokeID;
  }
}
//------------------------------------------------------------------------------
/**
 *  ナゾのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NazoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,   handler_NazoNomi     },
    { BTL_EVENT_USE_ITEM,              handler_NazoNomi_Use },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_NazoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > BTL_TYPEAFF_100 )
    {
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
    }
  }
}
static void handler_NazoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 item_pow = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );

    param->pokeID = pokeID;
    param->recoverHP = BTL_CALC_QuotMaxHP( bpp, item_pow );
  }
}
//------------------------------------------------------------------------------
/**
 *  オッカのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OkkaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_OkkaNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_OkkaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_HONOO ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  イトケのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ItokeNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_ItokeNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_ItokeNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_MIZU ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ソクノのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SokunoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_SokunoNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SokunoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_DENKI ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  リンドのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RindoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_RindoNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_RindoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_KUSA ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ヤチェのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YacheNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,          handler_YacheNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,       handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_YacheNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_KOORI ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ヨプのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YopuNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_YopuNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_YopuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_KAKUTOU ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ビアーのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BiarNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_BiarNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_BiarNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_DOKU ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  シュカのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SyukaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_SyukaNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SyukaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_JIMEN ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  バコウのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BakouNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_BakouNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_BakouNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_HIKOU ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ウタンのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UtanNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_UtanNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_UtanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_ESPER ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  タンガのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TangaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_TangaNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_TangaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_MUSHI ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ヨロギのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YorogiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_YorogiNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_YorogiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_IWA ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  カシブのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KasibuNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_KasibuNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KasibuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_GHOST ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ハバンのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HabanNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_HabanNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HabanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_DRAGON ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ナモのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NamoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_NamoNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_NamoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_AKU ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  リリバのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RiribaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_RiribaNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_RiribaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_HAGANE ) ){
    work[0] = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ホズのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HozuNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_HozuNomi },
    { BTL_EVENT_WAZA_DMG_REACTION,      handler_common_WeakAff_DmgAfter },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HozuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_NORMAL )
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
    work[0] = 1;
  }
}
/**
 *  威力半減させたらTRUEを返す
 */
static BOOL common_WeakAff_Relieve( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType type )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > BTL_TYPEAFF_100)
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
    return TRUE;
  }
  return FALSE;
}
static void handler_common_WeakAff_DmgAfter( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
    {
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Item_DamageShrink );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );

      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CONSUME_ITEM, pokeID );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ピンチ時（HP <= 1/アイテム威力値）に反応する木の実の共通処理
 *
 * @param   myHandle
 * @param   flowWk
 * @param   pokeID
 * @param   work
 */
//----------------------------------------------------------------------------------
static void handler_PinchReactCommon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    s32 item_pow = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    BTL_Printf("ピンチリアクションアイテム: n=%d\n", item_pow );
    common_DamageReactCore( flowWk, pokeID, item_pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * HP 1/n 以下で反応するアイテムの共通処理
 *
 * @param   flowWk
 * @param   pokeID
 * @param   n
 */
//----------------------------------------------------------------------------------
static void common_DamageReactCore( BTL_SVFLOW_WORK* flowWk, u8 pokeID, u8 n )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

  if( BPP_GetValue(bpp, BPP_TOKUSEI) == POKETOKUSEI_KUISINBOU ){
    n /= 2;
  }
  if( n == 0 ){
    GF_ASSERT(0);
    n = 1;
  }

  {
    u32 hp = BPP_GetValue( bpp, BPP_HP );
    if( hp <= BTL_CALC_QuotMaxHP(bpp, n) ){
      BTL_Printf("最大HP=%d, 現HP=%d, n=%d ... 反応あり\n", BPP_GetValue(bpp,BPP_MAX_HP), hp, n);
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  しろいハーブ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SiroiHerb( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_FIXED, handler_SiroiHerb_React },
    { BTL_EVENT_USE_ITEM,      handler_SiroiHerb_Use   },
    { BTL_EVENT_USE_ITEM_TMP,  handler_SiroiHerb_UseTmp},
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SiroiHerb_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
    {
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      work[0] = 1;
    }
  }
}
static void handler_SiroiHerb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_RANK* recover_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    recover_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_RANK, pokeID );
    recover_param->pokeID = pokeID;

    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    msg_param->str.type = BTL_STRTYPE_SET;
    msg_param->str.ID = BTL_STRID_SET_RankRecoverItem;
    msg_param->str.args[0] = pokeID;
    msg_param->str.args[1] = BTL_EVENT_FACTOR_GetSubID( myHandle );
    msg_param->str.argCnt = 2;
  }
}
static void handler_SiroiHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    handler_SiroiHerb_Use( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  メンタルハーブ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MentalHerb( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,  handler_MentalHerb_React  }, // 状態異常チェックハンドラ
    { BTL_EVENT_USE_ITEM,             handler_MentalHerb_Use    },
    { BTL_EVENT_USE_ITEM_TMP,         handler_MentalHerb_UseTmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_MentalHerb_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( BPP_CheckSick(bpp, WAZASICK_MEROMERO) )
      {
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      }
    }
    work[0] = 1;  // 戦闘中、１度しか使えない
  }
}
static void handler_MentalHerb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_MEROMERO );
}
static void handler_MentalHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    handler_MentalHerb_Use( myHandle, flowWk, pokeID, work );
  }
}
//------------------------------------------------------------------------------
/**
 *  ひかりのこな
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HikarinoKona( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_HikarinoKona },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HikarinoKona( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    u32 per = 100 - common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    fx32 r = FX32_CONST(per) / 100;
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, r );
  }
}
//------------------------------------------------------------------------------
/**
 *  きょうせいギプス
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KyouseiGipusu( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,     handler_KyouseiGipusu },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KyouseiGipusu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
  }
}
//------------------------------------------------------------------------------
/**
 *  せんせいのツメ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SenseiNoTume( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,      handler_SenseiNoTume },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SenseiNoTume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 per = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    if( BTL_CALC_IsOccurPer(per) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_SP_PRIORITY_A, BTL_SPPRI_A_HIGH );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こうこうのしっぽ、まんぷくおこう
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KoukouNoSippo( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,      handler_KoukouNoSippo },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KoukouNoSippo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SP_PRIORITY_B, BTL_SPPRI_B_LOW );
  }
}
//------------------------------------------------------------------------------
/**
 *  おうじゃのしるし
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OujaNoSirusi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_OujaNoSirusi },
    { BTL_EVENT_USE_ITEM_TMP,        handler_OujaNoSirusi_UseTmp },   // なげつける等
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_OujaNoSirusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ひるみ確率０なら、アイテム威力値に書き換え
    u8 per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    if( per == 0 ){
      per = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
    }
  }
}
static void handler_OujaNoSirusi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SHRINK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SHRINK, pokeID );
    param->pokeID = pokeID;
    param->per = 100;
  }
}
//------------------------------------------------------------------------------
/**
 *  するどいツメ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SurudoiTume( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_SurudoiTume },
    { BTL_EVENT_USE_ITEM_TMP,        handler_SurudoiTume_UseTmp },   // なげつける等
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SurudoiTume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ひるみ確率０なら、アイテム威力値に書き換え
    u8 per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    if( per == 0 ){
      per = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
    }
  }
}
static void handler_SurudoiTume_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SHRINK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SHRINK, pokeID );
    param->pokeID = pokeID;
    param->per = 100;
  }
}
//------------------------------------------------------------------------------
/**
 *  こうかくレンズ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KoukakuLens( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_KoukakuLens },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KoukakuLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側のとき
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 命中率上昇
    u32 r = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    fx32 fx_ratio = FX32_CONST(r) / 100;
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, fx_ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  ピントレンズ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_PintLens( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,     handler_PintLens },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_PintLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側のとき
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 rank = BTL_EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK );
    rank += 1;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
  }
}
//------------------------------------------------------------------------------
/**
 *  ラッキーパンチ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_LuckyPunch( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK, handler_LuckyPunch   },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_LuckyPunch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ラッキーなら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_RAKKII )
    {
      // クリティカル２段階上昇
      u8 rank = BTL_EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK );
      rank += 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ながねぎ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Naganegi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK, handler_Naganegi   },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_Naganegi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // カモネギなら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_KAMONEGI )
    {
      // クリティカル２段階上昇
      u8 rank = BTL_EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK );
      rank += 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  フォーカスレンズ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FocusLens( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_FocusLens },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_FocusLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    u8 def_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* defender = BTL_SVFTOOL_GetPokeParam( flowWk, def_pokeID );

    if( BPP_TURNFLAG_Get(defender, BPP_TURNFLG_ACTION_DONE) )
    {
      u32 ratio = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
      fx32 fx_ratio = FX32_CONST(ratio) / 100;
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, fx_ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  のんきのおこう
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NonkiNoOkou( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_NonkiNoOkou },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_NonkiNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
  {
    u16 ratio = 100 - common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    fx32 fx_ratio = FX32_CONST(ratio) / 100;
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, fx_ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  けむりだま
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KemuriDama( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SKIP_NIGERU_CALC,    handler_KemuriDama },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KemuriDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}


//------------------------------------------------------------------------------
/**
 *  ちからのハチマキ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ChikaraNoHachimaki( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_ChikaraNoHachimaki },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_ChikaraNoHachimaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ぶつり攻撃のとき、威力上昇
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
    {
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      u32 ratio = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
      pow = (pow * ratio) / 100;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ものしりメガネ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MonosiriMegane( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MonosiriMegane },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_MonosiriMegane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // とくしゅ攻撃のとき、威力上昇
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
    {
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      u32 ratio = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
      pow = (pow * ratio) / 100;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  しんかいのキバ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SinkaiNoKiba( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_SinkaiNoKiba   },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SinkaiNoKiba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // パールルなら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_PAARURU )
    {
      // とくこう２倍
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  しんかいのウロコ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SinkaiNoUroko( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_SinkaiNoUroko   },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SinkaiNoUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が防御側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // パールルなら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_PAARURU )
    {
      // とくぼう２倍
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        u32 guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
        guard *= 2;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GUARD, guard );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  メタルパウダー
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MetalPowder( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_MetalPowder   },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_MetalPowder( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が防御側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // メタモンなら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_METAMON )
    {
      // ぼうぎょ２倍
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
      {
        u32 guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
        guard *= 2;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GUARD, guard );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  スピードパウダー
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SpeedPowder( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_SpeedPowder   },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SpeedPowder( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 自分がメタモンなら素早さ２倍
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetMonsNo(bpp) == MONSNO_METAMON )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  でんきだま
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DenkiDama( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_DenkiDama   },
    { BTL_EVENT_USE_ITEM_TMP,   handler_DenkiDama_UseTmp},
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// 装備効果
static void handler_DenkiDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ピカチュウなら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_PIKATYUU )
    {
      // こうげき・とくこう２倍
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
// なげつけ効果
static void handler_DenkiDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    param->pokeID[0] = pokeID;
    param->poke_cnt = 1;
    param->sickID = WAZASICK_MAHI;
    param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( WAZASICK_MAHI );
    param->fAlmost = TRUE;
  }
}

//------------------------------------------------------------------------------
/**
 *  こころのしずく
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KokoroNoSizuku( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_KokoroNoSizuku_Pow },
    { BTL_EVENT_DEFENDER_GUARD, handler_KokoroNoSizuku_Guard },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KokoroNoSizuku_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ラティアス・ラティオスなら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( (monsNo == MONSNO_RATHIASU) || (monsNo == MONSNO_RATHIOSU) )
    {
      // とくこう上昇
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        u32 item_pow = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
        fx32 ratio = FX32_CONST(item_pow) / 100;
//        BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );

      }
    }
  }
}
static void handler_KokoroNoSizuku_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が防御側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // ラティアス・ラティオスなら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( (monsNo == MONSNO_RATHIASU) || (monsNo == MONSNO_RATHIOSU) )
    {
      // とくぼう上昇
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        u32 guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
        u32 ratio = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
        guard = (guard * ratio) / 100;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GUARD, guard );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ふといほね
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FutoiHone( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER,     handler_FutoiHone },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_FutoiHone( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // カラカラ・ガラガラなら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( (monsNo == MONSNO_KARAKARA) || (monsNo == MONSNO_GARAGARA) )
    {
      // こうげき上昇
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  こだわりハチマキ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KodawariHachimaki( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,   handler_Kodawari_Common },
    { BTL_EVENT_ATTACKER_POWER,     handler_KodawariHachimaki_Pow },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KodawariHachimaki_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側なら
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 物理こうげき上昇
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こだわりメガネ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KodawariMegane( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX,   handler_Kodawari_Common },
    { BTL_EVENT_ATTACKER_POWER,     handler_KodawariMegane_Pow },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KodawariMegane_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側なら
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 特殊こうげき上昇
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こだわりスカーフ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KodawariScarf( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Kodawari_Common },
    { BTL_EVENT_CALC_AGILITY,     handler_KodawariScarf },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KodawariScarf( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // すばやさ上昇
    BTL_EVENTVAR_MulValue( BTL_EVAR_AGILITY, FX32_CONST(2) );
  }
}
// こだわり系共通
static void handler_Kodawari_Common( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
    param->pokeID = pokeID;
    param->flag = BPP_CONTFLG_KODAWARI_LOCK;
  }
}
//------------------------------------------------------------------------------
/**
 *  きあいのタスキ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiaiNoTasuki( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK,  handler_KiaiNoTasuki },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KiaiNoTasuki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( BPP_GetValue(bpp, BPP_HP) == BPP_GetValue(bpp, BPP_MAX_HP) ){
        work[0] = 1;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BPP_KORAE_ITEM );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  きあいのハチマキ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiaiNoHachimaki( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK,  handler_KiaiNoHachimaki },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KiaiNoHachimaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    u8 per = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    if( BTL_CALC_IsOccurPer(per) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BPP_KORAE_ITEM );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  たつじんのおび
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TatsujinNoObi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_TatsujinNoObi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_TatsujinNoObi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8  itemAtk = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );

    fx32 ratio = FX32_CONST(100 + itemAtk) / 100;
    BTL_Printf("たつじんのおび：itemAtk=%d, ratio=%08x\n", itemAtk, ratio);
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  いのちのたま
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_InochiNoTama( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_KICKBACK,  handler_InochiNoTama_KickBack },
    { BTL_EVENT_WAZA_DMG_PROC2, handler_InochiNoTama_Damage },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_InochiNoTama_KickBack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO_EX );
    ratio += 10;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO_EX, ratio );
  }
}
static void handler_InochiNoTama_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8  itemAtk = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );

    fx32 ratio = FX32_CONST(100 + itemAtk) / 100;
    BTL_Printf("いのちのたま：itemAtk=%d, ratio=%08x\n", itemAtk, ratio);
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  メトロノーム
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MetroNome( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_MetroNome},
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_MetroNome( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 counter = BPP_GetWazaContCounter( bpp );
    if( counter )
    {
      fx32 ratio;
      u16  pow, damage;
      pow = 100 + (common_GetItemParam(myHandle, flowWk, ITEM_PRM_ATTACK) * (counter));
      if( pow > 200 ){ pow = 200; }
      ratio = (FX32_CONST(pow) / 100);
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ねばりのかぎづめ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NebariNoKagidume( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_COUNT, handler_NebariNoKagidume },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_NebariNoKagidume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u16 hit_max = BTL_EVENTVAR_GetValue( BTL_EVAR_HITCOUNT_MAX );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_HITCOUNT, hit_max );
  }
}
//------------------------------------------------------------------------------
/**
 *  かいがらのすず
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KaigaraNoSuzu( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_SPECIAL_DRAIN, handler_KaigaraNoSuzu },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KaigaraNoSuzu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u16 volume, item_pow;

    item_pow = common_GetItemParam(myHandle, flowWk, ITEM_PRM_ATTACK);
    if( item_pow )
    {
      u16 volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME ) / item_pow;
      if( volume )
      {
        BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
        param->pokeID = pokeID;
        param->recoverHP = volume;
        {
          BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

          HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverLittle );
          HANDEX_STR_AddArg( &msg_param->str, pokeID );
          HANDEX_STR_AddArg( &msg_param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );
        }
      }
    }
    else
    {
      GF_ASSERT(0);
    }

  }
}

//------------------------------------------------------------------------------
/**
 *  たべのこし
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Tabenokosi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END, handler_Tabenokosi_Reaction },
    { BTL_EVENT_USE_ITEM,      handler_Tabenokosi_Use },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_Tabenokosi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
}
static void handler_Tabenokosi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    param->pokeID = pokeID;
    param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 16 );
  }
}
//------------------------------------------------------------------------------
/**
 *  どくどくだま
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DokudokuDama( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_DokudokuDama },
    { BTL_EVENT_USE_ITEM_TMP,     handler_DokudokuDama_UseTmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_DokudokuDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

  param->sickID = WAZASICK_DOKUDOKU;
  param->sickCont = BPP_SICKCONT_MakeMoudokuCont();
  param->fAlmost = FALSE;
  param->poke_cnt = 1;
  param->pokeID[0] = pokeID;

  param->fExMsg = TRUE;
  param->exStr.type = BTL_STRTYPE_SET;
  param->exStr.ID = BTL_STRID_SET_MoudokuGetSP;
  param->exStr.argCnt = 2;
  param->exStr.args[0] = pokeID;
  param->exStr.args[1] = BTL_EVENT_FACTOR_GetSubID( myHandle );;
}
// なげつけ効果
static void handler_DokudokuDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    param->pokeID[0] = pokeID;
    param->poke_cnt = 1;
    param->sickID = WAZASICK_DOKU;
    param->sickCont = BPP_SICKCONT_MakeMoudokuCont();
    param->fAlmost = TRUE;
  }
}
//------------------------------------------------------------------------------
/**
 *  かえんだま
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KaenDama( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_KaenDama },
    { BTL_EVENT_USE_ITEM_TMP,   handler_KaenDama_UseTmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KaenDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

  param->sickID = WAZASICK_YAKEDO;
  param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( WAZASICK_YAKEDO );
  param->fAlmost = FALSE;
  param->poke_cnt = 1;
  param->pokeID[0] = pokeID;
  param->fExMsg = TRUE;

  param->exStr.type = BTL_STRTYPE_SET;
  param->exStr.ID = BTL_STRID_SET_YakedoGetSP;
  param->exStr.argCnt = 2;
  param->exStr.args[0] = pokeID;
  param->exStr.args[1] = BTL_EVENT_FACTOR_GetSubID( myHandle );;
}
// なげつけ効果
static void handler_KaenDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    param->pokeID[0] = pokeID;
    param->poke_cnt = 1;
    param->sickID = WAZASICK_YAKEDO;
    param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( param->sickID );
    param->fAlmost = TRUE;
  }
}

//------------------------------------------------------------------------------
/**
 *  ぎんのこな
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_GinNoKona( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GinNoKona },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_GinNoKona( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_MUSHI );
}
//------------------------------------------------------------------------------
/**
 *  やわらかいすな
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YawarakaiSuna( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_YawarakaiSuna },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_YawarakaiSuna( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_JIMEN );
}
//------------------------------------------------------------------------------
/**
 *  かたいいし
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KataiIsi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KataiIsi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KataiIsi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_IWA );
}
//------------------------------------------------------------------------------
/**
 *  きせきのたね
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KisekiNoTane( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KisekiNoTane },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KisekiNoTane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_KUSA );
}
//------------------------------------------------------------------------------
/**
 *  くろいめがね
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KuroiMegane( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KuroiMegane },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KuroiMegane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_AKU );
}
//------------------------------------------------------------------------------
/**
 *  くろおび
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Kuroobi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Kuroobi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_Kuroobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_KAKUTOU );
}
//------------------------------------------------------------------------------
/**
 *  じしゃく
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Zisyaku( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Zisyaku },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_Zisyaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_DENKI );
}
//------------------------------------------------------------------------------
/**
 *  メタルコート
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MetalCoat( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MetalCoat },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_MetalCoat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_HAGANE );
}
//------------------------------------------------------------------------------
/**
 *  しんぴのしずく
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SinpiNoSizuku( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SinpiNoSizuku },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SinpiNoSizuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_MIZU );
}
//------------------------------------------------------------------------------
/**
 *  するどいくちばし
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SurudoiKutibasi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SurudoiKutibasi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SurudoiKutibasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_HIKOU );
}
//------------------------------------------------------------------------------
/**
 *  どくばり
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Dokubari( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Dokubari },
    { BTL_EVENT_USE_ITEM_TMP,   handler_Dokubari_UseTmp },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_Dokubari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_DOKU );
}
// なげつけ効果
static void handler_Dokubari_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    param->pokeID[0] = pokeID;
    param->poke_cnt = 1;
    param->sickID = WAZASICK_DOKU;
    param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( WAZASICK_DOKU );
    param->fAlmost = TRUE;
  }
}
//------------------------------------------------------------------------------
/**
 *  とけないこおり
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TokenaiKoori( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_TokenaiKoori },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_TokenaiKoori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_KOORI );
}
//------------------------------------------------------------------------------
/**
 *  のろいのおふだ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NoroiNoOfuda( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_NoroiNoOfuda },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_NoroiNoOfuda( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_GHOST );
}
//------------------------------------------------------------------------------
/**
 *  まがったスプーン
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MagattaSupuun( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MagattaSupuun },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_MagattaSupuun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_ESPER );
}
//------------------------------------------------------------------------------
/**
 *  もくたん
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Mokutan( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Mokutan },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_Mokutan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_HONOO );
}
//------------------------------------------------------------------------------
/**
 *  りゅうのキバ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RyuunoKiba( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_RyuunoKiba },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_RyuunoKiba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_DRAGON );
}
//------------------------------------------------------------------------------
/**
 *  シルクのスカーフ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SirukuNoSukaafu( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SirukuNoSukaafu },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SirukuNoSukaafu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_NORMAL );
}
//------------------------------------------------------------------------------
/**
 *  あやしいおこう
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_AyasiiOkou( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_AyasiiOkou },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_AyasiiOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_ESPER );
}
//------------------------------------------------------------------------------
/**
 *  がんせきおこう
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_GansekiOkou( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GansekiOkou },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_GansekiOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_IWA );
}
//------------------------------------------------------------------------------
/**
 *  さざなみのおこう
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SazanamiNoOkou( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SazanamiNoOkou },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SazanamiNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_MIZU );
}
//------------------------------------------------------------------------------
/**
 *  うしおのおこう
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UsioNoOkou( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_UsioNoOkou },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_UsioNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_MIZU );
}
//------------------------------------------------------------------------------
/**
 *  おはなのおこう
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OhanaNoOkou( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_OhanaNoOkou },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_OhanaNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_KUSA );
}
//----------------------------------------------------------------------------------
/**
 * とくていタイプのワザを威力アップさせる道具の共通処理
 *
 * @param   myHandle
 * @param   flowWk
 * @param   pokeID
 * @param   type
 */
//----------------------------------------------------------------------------------
static void common_PowerUpSpecificType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType type )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type )
    {
      u32 waza_pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      u32 ratio = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
      waza_pow = (waza_pow * ratio) / 100;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, waza_pow );
    }
  }
}


//------------------------------------------------------------------------------
/**
 *  ゴツゴツメット
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_GotugotuMet( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_GotugotuMet },   // ダメージ反応ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// ダメージ反応ハンドラ
static void handler_GotugotuMet( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag(waza, WAZAFLAG_Touch) )
    {
      BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
      param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      {
        const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, param->pokeID );
        u8 itemAtk = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
        param->damage = BTL_CALC_QuotMaxHP( target, itemAtk );
      }
      // @todo ここにメッセージは要らないのかな？
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ふうせん
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Huusen( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_Huusen_MemberIn       },   // メンバー入場ハンドラ
    { BTL_EVENT_CHECK_FLYING,        handler_Huusen_CheckFlying    },   // 飛行フラグチェックハンドラ
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_Huusen_DamageReaction },   // ダメージ反応ハンドラ
    { BTL_EVENT_ITEMSET_FIXED,       handler_Huusen_ItemSetFixed   },   // アイテム書き換え確定後
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// メンバー入場ハンドラ
static void handler_Huusen_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_FuusenIn );
      HANDEX_STR_AddArg( &param->str, pokeID );
    }
  }
}
// 飛行フラグチェックハンドラ
static void handler_Huusen_CheckFlying( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
// ダメージ反応ハンドラ
static void handler_Huusen_DamageReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
    param->pokeID = pokeID;
    param->itemID = ITEM_DUMMY_DATA;
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_FuusenBreak );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  }
}
// アイテム書き換え確定後ハンドラ
static void handler_Huusen_ItemSetFixed( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );

    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->sickCode = WAZASICK_FLYING_CANCEL;
    param->fStdMsgDisable = TRUE;
  }
}
//------------------------------------------------------------------------------
/**
 *  レッドカード
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RedCard( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_RedCard },   /// ダメージ反応ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
/// ダメージ反応ハンドラ
static void handler_RedCard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    BTL_HANDEX_PARAM_PUSHOUT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_PUSHOUT, pokeID );
    param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_RedCard );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
    HANDEX_STR_AddArg( &param->exStr, param->pokeID );

    // 相手を吹き飛ばしたら、アイテムを消費する
    {
      BTL_HANDEX_PARAM_SET_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_ITEM, pokeID );
      param->header.failSkipFlag = TRUE;
      param->pokeID = pokeID;
      param->itemID = ITEM_DUMMY_DATA;
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ねらいのまと
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NerainoMato( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_TYPE_CHECK,     handler_NerainoMato },   // タイプ相性による無効化チェック
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// タイプ相性による無効化チェック
static void handler_NerainoMato( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が防御側の時、相性により無効化されていれば有効に書き換え
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_NOEFFECT_FLAG) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, FALSE );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  しめつけバンド
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SimetukeBand( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,     handler_SimetukeBand },   // 状態異常パラメータチェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// 状態異常パラメータチェックハンドラ
static void handler_SimetukeBand( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BPP_SICK_CONT cont;

    cont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
    BPP_SICKCONT_SetFlag( &cont, TRUE );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}
//------------------------------------------------------------------------------
/**
 *  きゅうこん
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Kyuukon( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Kyuukon_DmgReaction },   // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,              handler_Kyuukon_Use   },         // どうぐ使用ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// ダメージ反応ハンドラ
static void handler_Kyuukon_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が水タイプのワザダメージを受けた時、特攻がまだ上がるなら反応する
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_MIZU )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( BPP_IsRankEffectValid(bpp, BPP_SP_ATTACK, 1) )
      {
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      }
    }
  }
}
// どうぐ使用ハンドラ
static void handler_Kyuukon_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->rankType = BPP_SP_ATTACK;
    param->rankVolume = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  じゅうでんち
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Juudenti( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Juudenti_DmgReaction },   // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,              handler_Juudenti_Use   },         // どうぐ使用ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// ダメージ反応ハンドラ
static void handler_Juudenti_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が電気タイプのワザダメージを受けた時、攻撃がまだ上がるなら反応する
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_DENKI )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( BPP_IsRankEffectValid(bpp, BPP_ATTACK, 1) )
      {
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      }
    }
  }
}
// どうぐ使用ハンドラ
static void handler_Juudenti_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->rankType = BPP_ATTACK;
    param->rankVolume = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  だっしゅつポッド
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DassyutuPod( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_DassyutuPod_Reaction },  // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,              handler_DassyutuPod_Use      },  // どうぐ使用ハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// ダメージ反応ハンドラ
static void handler_DassyutuPod_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分がダメージを受けたら、どうぐ使用ハンドラ呼び出し
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
  }
}
// どうぐ使用ハンドラ
static void handler_DassyutuPod_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_PUSHOUT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_PUSHOUT, pokeID );
    param->pokeID = pokeID;

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_DassyutuPod );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  }
}

//------------------------------------------------------------------------------
/**
 *  ○○○ジュエル
 */
//------------------------------------------------------------------------------

// ダメージ確定ハンドラ共通
static void common_Juel_DmgDetermine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type )
{
  // 自分が攻撃側＆タイプがマッチしていたらメッセージ表示
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  ){
    BTL_HANDEX_PARAM_MESSAGE* param;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Juel );
    HANDEX_STR_AddArg( &param->str, pokeID );
    HANDEX_STR_AddArg( &param->str, BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
  }
}

// 威力計算ハンドラ共通
static void common_Juel_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type )
{
  // 自分が攻撃側＆タイプがマッチしていたら威力を増加＆アイテム削除
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5f) );
    BTL_SVFRET_RemoveItem( flowWk, pokeID );
  }
}
/**
 *  ほのおのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HonooNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_HonooNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_HonooNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HonooNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_HONOO );
}
static void handler_HonooNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_HONOO );
}
/**
 *  みずのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MizuNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_MizuNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_MizuNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_MizuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_MIZU );
}
static void handler_MizuNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_MIZU );
}
/**
 *  でんきのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DenkiNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_DenkiNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_DenkiNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_DenkiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_DENKI );
}
static void handler_DenkiNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_DENKI );
}
/**
 *  くさのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KusaNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_KusaNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_KusaNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KusaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_KUSA );
}
static void handler_KusaNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_KUSA );
}
/**
 *  こおりのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KooriNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_KooriNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_KooriNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KooriNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_KOORI );
}
static void handler_KooriNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_KOORI );
}
/**
 *  かくとうジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KakutouJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_KakutouJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_KakutouJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KakutouJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_KAKUTOU );
}
static void handler_KakutouJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_KAKUTOU );
}
/**
 *  どくのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DokuNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_DokuNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_DokuNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_DokuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_DOKU );
}
static void handler_DokuNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_DOKU );
}
/**
 *  じめんのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_JimenNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_JimenNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_JimenNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_JimenNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_JIMEN );
}
static void handler_JimenNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_JIMEN );
}
/**
 *  ひこうのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HikouNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_HikouNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_HikouNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HikouNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU );
}
static void handler_HikouNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU );
}
/**
 *  エスパージュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_EsperJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_EsperJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_EsperJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_EsperJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_ESPER );
}
static void handler_EsperJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_ESPER );
}
/**
 *  むしのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MusiNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_MusiNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_MusiNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_MusiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_MUSHI );
}
static void handler_MusiNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_MUSHI );
}
/**
 *  いわのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_IwaNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_IwaNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_IwaNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_IwaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_IWA );
}
static void handler_IwaNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_IWA );
}
/**
 *  ゴーストジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_GhostJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_GhostJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_GhostJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_GhostJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_GHOST );
}
static void handler_GhostJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_GHOST );
}
/**
 *  ドラゴンジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DragonJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_DragonNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_DragonNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_DragonNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_DRAGON );
}
static void handler_DragonNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_DRAGON );
}
/**
 *  あくのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_AkuNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_AkuNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_AkuNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_AkuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_AKU );
}
static void handler_AkuNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_AKU );
}
/**
 *  はがねのジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HaganeNoJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_HaganeNoJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_HaganeNoJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HaganeNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_HAGANE );
}
static void handler_HaganeNoJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_HAGANE );
}
/**
 *  ノーマルジュエル
 */
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NormalJuel( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,             handler_NormalJuel_Pow },
    { BTL_EVENT_WAZA_DMG_DETERMINE,     handler_NormalJuel_Dmg },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_NormalJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_NORMAL );
}
static void handler_NormalJuel_Dmg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_DmgDetermine( myHandle, flowWk, pokeID, work, POKETYPE_NORMAL );
}


