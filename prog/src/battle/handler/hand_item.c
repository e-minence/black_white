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



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static s32 common_GetItemParam( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u16 paramID );
static void handler_KuraboNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KuraboNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KagoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KagoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_ChigoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ChigoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_NanashiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NanashiNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KiiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiiNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_MomonNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MomonNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_RamNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RamNomi( u16 pri, u16 itemID, u8 pokeID );
static void common_sickReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HimeriNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_HimeriNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HimeriNomi_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OrenNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_OrenNomi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_OrenNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KinomiJuice( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ObonNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_ObonNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FiraNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UiNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MagoNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BanziNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_IaNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TiiraNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_TiiraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RyugaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_RyugaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KamuraNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KamuraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YatapiNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_YatapiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ZuaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_ZuaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SanNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_SanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SutaaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_SutaaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static void common_WeakAff_Relieve( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType type );
static void handler_PinchReactCommon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_DamageReactCore( BTL_SVFLOW_WORK* flowWk, u8 pokeID, u8 n );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SiroiHerb( u16 pri, u16 itemID, u8 pokeID );
static void handler_SiroiHerb_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SiroiHerb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MentalHerb( u16 pri, u16 itemID, u8 pokeID );
static void handler_MentalHerb_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KaenDama( u16 pri, u16 itemID, u8 pokeID );
static void handler_KaenDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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




BTL_EVENT_FACTOR*  BTL_HANDLER_ITEM_Add( const BTL_POKEPARAM* pp )
{
  // イベントハンドラ登録関数の型定義
  typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, u16 itemID, u8 pokeID );

  static const struct {
    u32             itemID;
    pEventAddFunc   func;
  }funcTbl[] = {
    { ITEM_KURABONOMI,    HAND_ADD_ITEM_KuraboNomi  },
    { ITEM_KAGONOMI,      HAND_ADD_ITEM_KagoNomi    },
    { ITEM_TIIGONOMI,     HAND_ADD_ITEM_ChigoNomi   },
    { ITEM_NANASINOMI,    HAND_ADD_ITEM_NanashiNomi },
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
    { ITEM_KIINOMI,       HAND_ADD_ITEM_KiiNomi     },

    { ITEM_SIROIHAABU,        HAND_ADD_ITEM_SiroiHerb         },
    { ITEM_MENTARUHAABU,      HAND_ADD_ITEM_MentalHerb        },
    { ITEM_KEMURIDAMA,        HAND_ADD_ITEM_KemuriDama        },
    { ITEM_HIKARINOKONA,      HAND_ADD_ITEM_HikarinoKona      },
    { ITEM_KYOUSEIGIPUSU,     HAND_ADD_ITEM_KyouseiGipusu     },
    { ITEM_SENSEINOTUME,      HAND_ADD_ITEM_SenseiNoTume      },
    { ITEM_KOUKOUNOSIPPO,     HAND_ADD_ITEM_KoukouNoSippo     },
    { ITEM_MANPUKUOKOU,       HAND_ADD_ITEM_KoukouNoSippo     },  // まんぷくおこう=こうこうのしっぽ等価
    { ITEM_OUZYANOSIRUSI,     HAND_ADD_ITEM_OujaNoSirusi      },
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

    { 0, NULL },
  };

  u32 itemID = BTL_POKEPARAM_GetItem( pp );
  if( itemID != ITEM_DUMMY_DATA )
  {
    u32 i;
    for(i=0; i<NELEMS(funcTbl); ++i)
    {
      if( funcTbl[i].itemID == itemID )
      {
        u16 priority = BTL_POKEPARAM_GetValue_Base( pp, BPP_AGILITY );
        u8 pokeID = BTL_POKEPARAM_GetID( pp );
        return funcTbl[i].func( priority, itemID, pokeID );
      }
    }
  }

  return NULL;
}

//=============================================================================================
/**
 * ポケモンの「アイテム」ハンドラをシステムから削除
 *
 * @param   pp
 *
 */
//=============================================================================================
void BTL_HANDLER_ITEM_Remove( const BTL_POKEPARAM* pp )
{
  BTL_EVENT_FACTOR* factor;
  u8 pokeID = BTL_POKEPARAM_GetID( pp );

  while( (factor = BTL_EVENT_SeekFactor(BTL_EVENT_FACTOR_ITEM, pokeID)) != NULL )
  {
    BTL_EVENT_FACTOR_Remove( factor );
  }
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KuraboNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_FIX,    handler_KuraboNomi },
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KagoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_FIX, handler_KagoNomi },    // 状態異常チェックハンドラ
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ChigoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_FIX, handler_ChigoNomi },   // 状態異常チェックハンドラ
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  ナナシのみ
 */
//------------------------------------------------------------------------------
static void handler_NanashiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_sickReaction( myHandle, flowWk, pokeID, WAZASICK_KOORI );
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NanashiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_FIX, handler_NanashiNomi },   // 状態異常チェックハンドラ
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_FIX, handler_KiiNomi },   // 状態異常チェックハンドラ
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MomonNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_FIX, handler_MomonNomi },   // 状態異常チェックハンドラ
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RamNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_FIX, handler_RamNomi },   // 状態異常チェックハンドラ
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
  WazaSick sickID = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    BOOL  fMatch = FALSE;
    switch( sickCode ){
    case WAZASICK_EX_POKEFULL_PLUS:
      fMatch = ( (sickID < POKESICK_MAX) || (sickID == WAZASICK_KONRAN) );
      break;
    case WAZASICK_EX_POKEFULL:
      fMatch = (sickID < POKESICK_MAX);
      break;
    default:
      fMatch = (sickID == sickCode);
      break;
    }
    if( fMatch )
    {
      BTL_HANDEX_PARAM_CURE_SICK* param;
      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->sickCode = sickCode;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;

      BTL_EVENTVAR_RewriteValue( BTL_EVAR_ITEM, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
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
    { BTL_EVENT_USE_ITEM,               handler_HimeriNomi_Exe },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HimeriNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 waza_idx = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_IDX );
    if( BTL_POKEPARAM_GetPP(bpp, waza_idx) == 0 )
    {
      // work[0] = 対象ワザインデックスの受け渡し用に使う
      work[0] = waza_idx;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }
  }
}
static void handler_HimeriNomi_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_PP* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_PP, pokeID );
    param->volume = 10;
    param->pokeID = pokeID;
    param->wazaIdx = work[0];
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
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_OrenNomi_Reaction },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_OrenNomi_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use },
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
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    param->pokeID = pokeID;
    param->recoverHP = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
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
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_OrenNomi_Reaction },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_OrenNomi_Reaction },
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
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_ObonNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_ObonNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 ratio = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );

    param->pokeID = pokeID;
    param->recoverHP = BTL_POKEPARAM_GetValue(bpp, BPP_MAX_HP) * 100 / ratio;
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
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
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
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
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
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
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
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
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
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
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
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_TiiraNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_TiiraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_ATTACK, 1 );
}
//------------------------------------------------------------------------------
/**
 *  リュガのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RyugaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_RyugaNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_RyugaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_DEFENCE, 1 );
}
//------------------------------------------------------------------------------
/**
 *  カムラのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KamuraNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_KamuraNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KamuraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_AGILITY, 1 );
}
//------------------------------------------------------------------------------
/**
 *  ヤタピのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YatapiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_YatapiNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_YatapiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_SP_ATTACK, 1 );
}
//------------------------------------------------------------------------------
/**
 *  ズアのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ZuaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_ZuaNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_ZuaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_SP_DEFENCE, 1 );
}
//------------------------------------------------------------------------------
/**
 *  サンのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SanNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_SanNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PinchRankup( flowWk, pokeID, WAZA_RANKEFF_CRITICAL_RATIO, 1 );
}
//------------------------------------------------------------------------------
/**
 *  スターのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SutaaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_AFTER,         handler_PinchReactCommon },
    { BTL_EVENT_SIMPLE_DAMAGE_REACTION, handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_SutaaNomi },
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
    BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );
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
    { BTL_EVENT_WAZA_DMG_AFTER,     handler_NazoNomi     },
    { BTL_EVENT_USE_ITEM,           handler_NazoNomi_Use },
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
      BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
    }
  }
}
static void handler_NazoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
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
    { BTL_EVENT_CHECK_AFFINITY,         handler_OkkaNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_OkkaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_HONOO );
}
//------------------------------------------------------------------------------
/**
 *  イトケのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ItokeNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,         handler_ItokeNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_ItokeNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_MIZU );
}
//------------------------------------------------------------------------------
/**
 *  ソクノのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SokunoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_SokunoNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SokunoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_DENKI );
}
//------------------------------------------------------------------------------
/**
 *  リンドのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RindoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_RindoNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_RindoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_KUSA );
}
//------------------------------------------------------------------------------
/**
 *  ヤチェのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YacheNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_YacheNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_YacheNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_KOORI );
}
//------------------------------------------------------------------------------
/**
 *  ヨプのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YopuNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_YopuNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_YopuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_KAKUTOU );
}
//------------------------------------------------------------------------------
/**
 *  ビアーのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BiarNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_BiarNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_BiarNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_DOKU );
}
//------------------------------------------------------------------------------
/**
 *  シュカのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SyukaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_SyukaNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SyukaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_JIMEN );
}
//------------------------------------------------------------------------------
/**
 *  バコウのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BakouNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_BakouNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_BakouNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_HIKOU );
}
//------------------------------------------------------------------------------
/**
 *  ウタンのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UtanNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_UtanNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_UtanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_ESPER );
}
//------------------------------------------------------------------------------
/**
 *  タンガのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_TangaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_TangaNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_TangaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_MUSHI );
}
//------------------------------------------------------------------------------
/**
 *  ヨロギのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_YorogiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_YorogiNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_YorogiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_IWA );
}
//------------------------------------------------------------------------------
/**
 *  カシブのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KasibuNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_KasibuNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KasibuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_GHOST );
}
//------------------------------------------------------------------------------
/**
 *  ハバンのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HabanNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_HabanNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HabanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_DRAGON );
}
//------------------------------------------------------------------------------
/**
 *  ナモのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NamoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_NamoNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_NamoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_AKU );
}
//------------------------------------------------------------------------------
/**
 *  リリバのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RiribaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_RiribaNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_RiribaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, POKETYPE_HAGANE );
}
//------------------------------------------------------------------------------
/**
 *  ホズのみ
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_HozuNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,               handler_HozuNomi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_HozuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_NORMAL )
  ){
    BTL_EVWK_CHECK_AFFINITY* evwk = (BTL_EVWK_CHECK_AFFINITY*) BTL_EVENTVAR_GetValue(BTL_EVAR_WORK_ADRS);
    evwk->weakReserveByItem = TRUE;
    evwk->weakedItemID = BTL_EVENT_FACTOR_GetSubID( myHandle );
  }
}

static void common_WeakAff_Relieve( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType type )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  ){
    BTL_EVWK_CHECK_AFFINITY* evwk = (BTL_EVWK_CHECK_AFFINITY*) BTL_EVENTVAR_GetValue(BTL_EVAR_WORK_ADRS);
    if( evwk->aff > BTL_TYPEAFF_100 )
    {
      evwk->aff--;
      evwk->weakedByItem = TRUE;
      evwk->weakedItemID = BTL_EVENT_FACTOR_GetSubID( myHandle );
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
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    s32 item_pow = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
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
  const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

  if( BTL_POKEPARAM_GetValue(bpp, BPP_TOKUSEI) == POKETOKUSEI_KUISINBOU ){
    n /= 2;
  }
  if( n == 0 ){
    GF_ASSERT(0);
    n = 1;
  }

  {
    u32 hp = BTL_POKEPARAM_GetValue( bpp, BPP_HP );
    if( hp <= BTL_CALC_QuotMaxHP(bpp, n) ){
      BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
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
      BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
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

    recover_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_RANK, pokeID );
    recover_param->pokeID = pokeID;

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
    msg_param->strID = BTL_STRID_SET_RankRecoverItem;
    msg_param->pokeID = pokeID;
    msg_param->args[0] = BTL_EVENT_FACTOR_GetSubID( myHandle );
    msg_param->arg_cnt = 1;
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
    { BTL_EVENT_ADDSICK_FIX,  handler_MentalHerb_React },
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
      WazaSick sickID = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
      if( sickID == WAZASICK_MEROMERO )
      {
        BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        param->pokeID[0] = pokeID;
        param->poke_cnt = 1;
        param->sickCode = WAZASICK_MEROMERO;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_ITEM, BTL_EVENT_FACTOR_GetSubID(myHandle) );
        work[0] = 1;
      }
    }
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
    u32 per = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    s32 r = 100 - common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    per = (per * r) / 100;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, per );
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
    u32 agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
    agi /= 2;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_AGILITY, agi );
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
    { BTL_EVENT_SHRINK_CHECK,     handler_OujaNoSirusi },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_OujaNoSirusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ダメージワザのとき
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_IsDamage(waza) )
    {
      // ひるみ確率０なら、アイテム威力値に書き換え
      u8 per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
      if( per == 0 ){
        per = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
      }
    }
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
    u32 per = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    u32 ratio = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    per = (ratio * per) / 100;
    if( per > 100 ){
      per = 100;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, per );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BTL_POKEPARAM_GetMonsNo( bpp );
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
    // ラッキーなら
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BTL_POKEPARAM_GetMonsNo( bpp );
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
    const BTL_POKEPARAM* defender = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, def_pokeID );

    if( BTL_POKEPARAM_GetTurnFlag(defender, BPP_TURNFLG_ACTION_DONE) )
    {
      u16 per = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      u16 ratio = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
      per = (per * ratio) / 100;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, per );
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
    u16 hit_per = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    u16 ratio = 100 - common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    hit_per = (hit_per * ratio) / 100;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, hit_per );
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
    { BTL_EVENT_CHECK_ESCAPE,    handler_KemuriDama },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KemuriDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) )
  {
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BTL_POKEPARAM_GetMonsNo( bpp );
    if( monsNo == MONSNO_PAARURU )
    {
      // とくこう２倍
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
        pow *= 2;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BTL_POKEPARAM_GetMonsNo( bpp );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BTL_POKEPARAM_GetMonsNo( bpp );
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
 *  メタルコート
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
    // すばやさ上昇
    u32 agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
    agi *= 2;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_AGILITY, agi );
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
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_DenkiDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ピカチュウなら
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BTL_POKEPARAM_GetMonsNo( bpp );
    if( monsNo == MONSNO_PIKATYUU )
    {
      // こうげき・とくこう２倍
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
      pow *= 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
    }
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BTL_POKEPARAM_GetMonsNo( bpp );
    if( (monsNo == MONSNO_RATHIASU) || (monsNo == MONSNO_RATHIOSU) )
    {
      // とくこう上昇
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
        u32 ratio = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
        pow = (pow * ratio) / 100;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_POWER, pow );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BTL_POKEPARAM_GetMonsNo( bpp );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BTL_POKEPARAM_GetMonsNo( bpp );
    if( (monsNo == MONSNO_KARAKARA) || (monsNo == MONSNO_GARAGARA) )
    {
      // こうげき上昇
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
      {
        u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
        pow *= 2;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
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
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      pow = BTL_CALC_MulRatio( pow, FX32_CONST(1.5f) );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
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
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      pow = BTL_CALC_MulRatio( pow, FX32_CONST(1.5f) );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
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
    u32 agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
    agi *= 2;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_AGILITY, agi );
  }
}
// こだわり系共通
static void handler_Kodawari_Common( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_CONTFLAG* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
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
      const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
      if( BTL_POKEPARAM_GetValue(bpp, BPP_HP) == BTL_POKEPARAM_GetValue(bpp, BPP_MAX_HP) ){
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
    u16 damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    u8  ratio = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );

    damage = damage * (100+ratio) / 100;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
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
    u16 damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    u8  ratio = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );

    damage = damage * (100+ratio) / 100;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
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
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u16 counter = BTL_POKEPARAM_GetSameWazaUsedCounter( bpp );
    if( counter )
    {
      u16 ratio, damage;
      ratio = 100 + (common_GetItemParam(myHandle, flowWk, ITEM_PRM_ATTACK) * (counter));
      if( ratio > 200 ){ ratio = 200; }
      damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
      damage = (damage * ratio) / 100;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
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
        BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
        param->pokeID = pokeID;
        param->recoverHP = volume;
        {
          BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_SET, pokeID );
          msg_param->strID = BTL_STRID_SET_UseItem_RecoverLittle;
          msg_param->pokeID = pokeID;
          msg_param->args[0] = BTL_EVENT_FACTOR_GetSubID( myHandle );
          msg_param->arg_cnt = 1;
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
  BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
}
static void handler_Tabenokosi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

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
    { BTL_EVENT_TURNCHECK_END, handler_DokudokuDama },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_DokudokuDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

  param->sickID = WAZASICK_DOKU;
  param->sickCont = BTL_CALC_MakeMoudokuSickCont();
  param->fAlmost = FALSE;
  param->poke_cnt = 1;
  param->pokeID[0] = pokeID;
  param->fExMsg = TRUE;
  param->exStrID = BTL_STRID_SET_MoudokuGetSP;
  param->exStrArgCnt = 1;
  param->exStrArgs[0] = BTL_EVENT_FACTOR_GetSubID( myHandle );
}
//------------------------------------------------------------------------------
/**
 *  かえんだま
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KaenDama( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END, handler_KaenDama },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_KaenDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );

  param->sickID = WAZASICK_YAKEDO;
  param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( WAZASICK_YAKEDO );
  param->fAlmost = FALSE;
  param->poke_cnt = 1;
  param->pokeID[0] = pokeID;
  param->fExMsg = TRUE;
  param->exStrID = BTL_STRID_SET_YakedoGetSP;
  param->exStrArgCnt = 1;
  param->exStrArgs[0] = BTL_EVENT_FACTOR_GetSubID( myHandle );

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
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_Dokubari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_PowerUpSpecificType( myHandle, flowWk, pokeID, POKETYPE_DOKU );
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



#if 0
テンプレ
//------------------------------------------------------------------------------
/**
 *
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_@@@( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_,     handler_@@@_React },
    { BTL_EVENT_USE_ITEM,       handler_@@@_Use },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}

static void handler_@@@_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {

  }
}

static void handler_@@@_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_###* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_###, pokeID );
    param->pokeID = pokeID;
  }
}

#endif

