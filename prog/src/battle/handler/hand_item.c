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
#include "sound\pm_sndsys.h"

#include "..\btl_common.h"
#include "..\btl_calc.h"
#include "..\btl_field.h"
#include "..\btl_client.h"
#include "..\btl_event_factor.h"
#include "..\btlv\btlv_effect.h"


#include "hand_common.h"
#include "hand_item.h"


enum {
  WORKIDX_TMP_FLAG = EVENT_HANDLER_WORK_ELEMS-1,  ///< 一時利用アイテム
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR* AddItemEventCore( const BTL_POKEPARAM* bpp, u16 itemID );
static BOOL Item_IsExePer( BTL_SVFLOW_WORK* flowWk, u8 per );
static s32 common_GetItemParam( BTL_EVENT_FACTOR* myHandle, u16 paramID );
static fx32 Item_AttackValue_to_Ratio( BTL_EVENT_FACTOR* myHandle );
static void ItemPushRun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID );
static void handler_KuraboNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KuraboNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KuraboNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuraboNomi( u32* numElems );
static void handler_KagoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KagoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KagoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KagoNomi( u32* numElems );
static void handler_ChigoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ChigoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ChigoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_ChigoNomi( u32* numElems );
static void handler_NanasiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NanasiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NanasiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_NanasiNomi( u32* numElems );
static void handler_KiiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KiiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KiiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KiiNomi( u32* numElems );
static void handler_MomonNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MomonNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MomonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MomonNomi( u32* numElems );
static void handler_RamNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_RamNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_RamNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_RamNomi( u32* numElems );
static void common_sickReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
static void common_useForSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
static BOOL common_sickcode_match( BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HimeriNomi( u32* numElems );
static void handler_HimeriNomi_wazaEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HimeriNomi_reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HimeriNomi_get( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HimeriNomi_ppDec( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HimeriNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HimeriNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static u8 common_Himeri_LastWazaIdx( BTL_SVFLOW_WORK* flowWk, u8 pokeID );
static u8 common_Himeri_EnableWazaIdx( BTL_SVFLOW_WORK* flowWk, u8 pokeID );
static BOOL handler_HimeriNomi_common( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, BOOL fUseTmp );
static const BtlEventHandlerTable* HAND_ADD_ITEM_OrenNomi( u32* numElems );
static void handler_OrenNomi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_OrenNomi_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_OrenNomi_RotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_OrenNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KinomiJuice( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_ObonNomi( u32* numElems );
static void handler_ObonNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ObonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_FiraNomi( u32* numElems );
static void handler_FiraNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_FiraNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_UiNomi( u32* numElems );
static void handler_UiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_UiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MagoNomi( u32* numElems );
static void handler_MagoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MagoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_BanziNomi( u32* numElems );
static void handler_BanjiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_BanjiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_IaNomi( u32* numElems );
static void handler_IaNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_IaNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_common_KaifukuKonran_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_common_KaifukuKonran_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_common_KaifukuKonran_RotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_KaifukuKonran( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PtlTaste taste );
static const BtlEventHandlerTable* HAND_ADD_ITEM_TiiraNomi( u32* numElems );
static void handler_TiiraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_TiiraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_RyugaNomi( u32* numElems );
static void handler_RyugaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_RyugaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KamuraNomi( u32* numElems );
static void handler_KamuraNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KamuraNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_YatapiNomi( u32* numElems );
static void handler_YatapiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_YatapiNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_ZuaNomi( u32* numElems );
static void handler_ZuaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ZuaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SanNomi( u32* numElems );
static void handler_SanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SanNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SutaaNomi( u32* numElems );
static void handler_SutaaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SutaaNomi_Tmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_PinchRankup( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaRankEffect rankType, u8 volume );
static const BtlEventHandlerTable* HAND_ADD_ITEM_NazoNomi( u32* numElems );
static void handler_NazoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NazoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_OkkaNomi( u32* numElems );
static void handler_OkkaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_ItokeNomi( u32* numElems );
static void handler_ItokeNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SokunoNomi( u32* numElems );
static void handler_SokunoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_RindoNomi( u32* numElems );
static void handler_RindoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_YacheNomi( u32* numElems );
static void handler_YacheNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_YopuNomi( u32* numElems );
static void handler_YopuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_BiarNomi( u32* numElems );
static void handler_BiarNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SyukaNomi( u32* numElems );
static void handler_SyukaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_BakouNomi( u32* numElems );
static void handler_BakouNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_UtanNomi( u32* numElems );
static void handler_UtanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_TangaNomi( u32* numElems );
static void handler_TangaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_YorogiNomi( u32* numElems );
static void handler_YorogiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KasibuNomi( u32* numElems );
static void handler_KasibuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HabanNomi( u32* numElems );
static void handler_HabanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_NamoNomi( u32* numElems );
static void handler_NamoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_RiribaNomi( u32* numElems );
static void handler_RiribaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HozuNomi( u32* numElems );
static void handler_HozuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BOOL common_WeakAff_Relieve( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type, BOOL fIgnoreAffine );
static void handler_common_WeakAff_DmgAfter( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_PinchReactCommon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_PinchReactMemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_PinchReactRotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_DamageReact( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n, BOOL fCheckReactionType );
static BOOL common_DamageReactCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n );
static BOOL common_DamageReactCheckCore( BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n );
static const BtlEventHandlerTable* HAND_ADD_ITEM_IbanNomi( u32* numElems );
static void handler_IbanNomi_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_IbanNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MikuruNomi( u32* numElems );
static void handler_MikuruNomi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MikuruNomi_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MikuruNomi_ActProcEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MikuruNomi_RotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MikuruNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MikuruNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_JapoNomi( u32* numElems );
static void handler_JapoNomi_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_RenbuNomi( u32* numElems );
static void handler_RenbuNomi_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_JapoRenbu_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType );
static void handler_JapoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SiroiHerb( u32* numElems );
static void handler_SiroiHerb_ActCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SiroiHerb_MemberInComp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SiroiHerb_ActEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SiroiHerb_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SiroiHerb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SiroiHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MentalHerb( u32* numElems );
static void handler_MentalHerb_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MentalHerb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MentalHerb_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HikarinoKona( u32* numElems );
static void handler_HikarinoKona( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KyouseiGipusu( u32* numElems );
static void handler_KyouseiGipusu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SenseiNoTume( u32* numElems );
static void handler_SenseiNoTume_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SenseiNoTume_SpPriorityWorked( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SenseiNoTume_ActStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SenseiNoTume_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KoukouNoSippo( u32* numElems );
static void handler_KoukouNoSippo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_OujaNoSirusi( u32* numElems );
static void handler_OujaNoSirusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_OujaNoSirusi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SurudoiKiba( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KoukakuLens( u32* numElems );
static void handler_KoukakuLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_PintLens( u32* numElems );
static void handler_PintLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_LuckyPunch( u32* numElems );
static void handler_LuckyPunch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Naganegi( u32* numElems );
static void handler_Naganegi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_FocusLens( u32* numElems );
static void handler_FocusLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_NonkiNoOkou( u32* numElems );
static void handler_NonkiNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_ChikaraNoHachimaki( u32* numElems );
static void handler_ChikaraNoHachimaki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MonosiriMegane( u32* numElems );
static void handler_MonosiriMegane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SinkaiNoKiba( u32* numElems );
static void handler_SinkaiNoKiba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SinkaiNoUroko( u32* numElems );
static void handler_SinkaiNoUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MetalPowder( u32* numElems );
static void handler_MetalPowder( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SpeedPowder( u32* numElems );
static void handler_SpeedPowder( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KokoroNoSizuku( u32* numElems );
static void handler_KokoroNoSizuku_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KokoroNoSizuku_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_FutoiHone( u32* numElems );
static void handler_FutoiHone( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariHachimaki( u32* numElems );
static void handler_KodawariHachimaki_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariMegane( u32* numElems );
static void handler_KodawariMegane_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariScarf( u32* numElems );
static void handler_KodawariScarf( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kodawari_Common_WazaExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kodawari_Common_ItemChange( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoTasuki( u32* numElems );
static void handler_KiaiNoTasuki_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KiaiNoTasuki_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoHachimaki( u32* numElems );
static void handler_KiaiNoHachimaki_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KiaiNoHachimaki_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KiaiNoHachimaki_UseItem( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_TatsujinNoObi( u32* numElems );
static void handler_TatsujinNoObi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_InochiNoTama( u32* numElems );
static void handler_InochiNoTama_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_InochiNoTama_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MetroNome( u32* numElems );
static void handler_MetroNome( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_NebariNoKagidume( u32* numElems );
static void handler_NebariNoKagidume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KaigaraNoSuzu( u32* numElems );
static void handler_KaigaraNoSuzu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HikariNoNendo( u32* numElems );
static void handler_HikariNoNendo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowefulHarb( u32* numElems );
static void handler_PowefulHarb_CheckTameSkip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_PowefulHarb_FixTameSkip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_PowefulHarb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Tabenokosi( u32* numElems );
static void handler_Tabenokosi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tabenokosi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiHedoro( u32* numElems );
static void handler_KuroiHedoro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_AkaiIto( u32* numElems );
static void handler_AkaiIto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuttukiBari( u32* numElems );
static void handler_KuttukiBari_DamageReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KuttukiBari_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWrist( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBelt( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerLens( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBand( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerAnkle( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWeight( u32* numElems );
static void handler_PowerXXX_CalcAgility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HinotamaPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SizukuPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_IkazutiPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MirodinoPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_TuraranoPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KobusinoPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MoudokuPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_DaitinoPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_AozoraPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HusiginoPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_TamamusiPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_GansekiPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MononokePlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_RyuunoPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KowamotePlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KoutetsuPlate( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_OokinaNekko( u32* numElems );
static void handler_OokinaNekko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_ADD_ITEM_Kemuridama( u32* numElems );
static void handler_Kemuridama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kemuridama_Msg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static  const BtlEventHandlerTable*  HAND_ADD_ITEM_OmamoriKoban( u32* numElems );
static void handler_OmamoriKoban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_OmamoriKoban_RotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable*  HAND_ADD_ITEM_HakkinDama( u32* numElems );
static void handler_HakkinDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_TumetaiIwa( u32* numElems );
static void handler_TumetaiIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SarasaraIwa( u32* numElems );
static void handler_SarasaraIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_AtuiIwa( u32* numElems );
static void handler_AtuiIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SimettaIwa( u32* numElems );
static void handler_SimettaIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_WazaWeatherTurnUp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather );
static const BtlEventHandlerTable* HAND_ADD_ITEM_DenkiDama( u32* numElems );
static void handler_DenkiDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DenkiDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_DokudokuDama( u32* numElems );
static void handler_DokudokuDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DokudokuDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KaenDama( u32* numElems );
static void handler_KaenDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KaenDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_GinNoKona( u32* numElems );
static void handler_GinNoKona( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_YawarakaiSuna( u32* numElems );
static void handler_YawarakaiSuna( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KataiIsi( u32* numElems );
static void handler_KataiIsi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KisekiNoTane( u32* numElems );
static void handler_KisekiNoTane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiMegane( u32* numElems );
static void handler_KuroiMegane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Kuroobi( u32* numElems );
static void handler_Kuroobi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Zisyaku( u32* numElems );
static void handler_Zisyaku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MetalCoat( u32* numElems );
static void handler_MetalCoat( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SinpiNoSizuku( u32* numElems );
static void handler_SinpiNoSizuku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SurudoiKutibasi( u32* numElems );
static void handler_SurudoiKutibasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Dokubari( u32* numElems );
static void handler_Dokubari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Dokubari_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_TokenaiKoori( u32* numElems );
static void handler_TokenaiKoori( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_NoroiNoOfuda( u32* numElems );
static void handler_NoroiNoOfuda( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MagattaSupuun( u32* numElems );
static void handler_MagattaSupuun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Mokutan( u32* numElems );
static void handler_Mokutan( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_RyuunoKiba( u32* numElems );
static void handler_RyuunoKiba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SirukuNoSukaafu( u32* numElems );
static void handler_SirukuNoSukaafu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_AyasiiOkou( u32* numElems );
static void handler_AyasiiOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_GansekiOkou( u32* numElems );
static void handler_GansekiOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SazanamiNoOkou( u32* numElems );
static void handler_SazanamiNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_UsioNoOkou( u32* numElems );
static void handler_UsioNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_OhanaNoOkou( u32* numElems );
static void handler_OhanaNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_PowerUpSpecificType( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType type );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Siratama( u32* numElems );
static void handler_Siratama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Kongoudama( u32* numElems );
static void handler_Kongoudama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiTekkyuu( u32* numElems );
static void handler_KuroiTekkyuu_Agility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KuroiTekkyuu_CheckFly( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Karuisi( u32* numElems );
static void handler_Karuisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SinkanoKiseki( u32* numElems );
static void handler_SinkanoKiseki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_GotugotuMet( u32* numElems );
static void handler_GotugotuMet( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Huusen( u32* numElems );
static void handler_Huusen_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Huusen_CheckFlying( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Huusen_DamageReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Huusen_ItemSetFixed( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_RedCard( u32* numElems );
static void handler_RedCard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_NerainoMato( u32* numElems );
static void handler_NerainoMato( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SimetukeBand( u32* numElems );
static void handler_SimetukeBand( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Kyuukon( u32* numElems );
static void handler_Kyuukon_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kyuukon_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_Juudenti( u32* numElems );
static void handler_Juudenti_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Juudenti_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_DassyutuButton( u32* numElems );
static void handler_DassyutuButton_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DassyutuButton_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HonooNoJuel( u32* numElems );
static void handler_HonooNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HonooNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MizuNoJuel( u32* numElems );
static void handler_MizuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MizuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_DenkiNoJuel( u32* numElems );
static void handler_DenkiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DenkiNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KusaNoJuel( u32* numElems );
static void handler_KusaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KusaNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KooriNoJuel( u32* numElems );
static void handler_KooriNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KooriNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KakutouJuel( u32* numElems );
static void handler_KakutouJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_KakutouJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_DokuNoJuel( u32* numElems );
static void handler_DokuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DokuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_JimenNoJuel( u32* numElems );
static void handler_JimenNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_JimenNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HikouNoJuel( u32* numElems );
static void handler_HikouNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HikouNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_EsperJuel( u32* numElems );
static void handler_EsperJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_EsperJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MusiNoJuel( u32* numElems );
static void handler_MusiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MusiNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_IwaNoJuel( u32* numElems );
static void handler_IwaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_IwaNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_GhostJuel( u32* numElems );
static void handler_GhostJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_GhostJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_DragonJuel( u32* numElems );
static void handler_DragonNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_DragonNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_AkuNoJuel( u32* numElems );
static void handler_AkuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_AkuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_HaganeNoJuel( u32* numElems );
static void handler_HaganeNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HaganeNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_NormalJuel( u32* numElems );
static void handler_NormalJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_NormalJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_Juel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type );
static void common_Juel_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type );
static void handler_Juel_EndDmgSeq( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );



// イベントハンドラ登録関数の型定義
typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numHandlers );

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
  { ITEM_IBANNOMI,      HAND_ADD_ITEM_IbanNomi    },
  { ITEM_MIKURUNOMI,    HAND_ADD_ITEM_MikuruNomi  },
  { ITEM_ZYAPONOMI,     HAND_ADD_ITEM_JapoNomi    },
  { ITEM_RENBUNOMI,     HAND_ADD_ITEM_RenbuNomi   },
  { ITEM_SIROIHAABU,        HAND_ADD_ITEM_SiroiHerb         },
  { ITEM_MENTARUHAABU,      HAND_ADD_ITEM_MentalHerb        },
  { ITEM_HIKARINOKONA,      HAND_ADD_ITEM_HikarinoKona      },
  { ITEM_KYOUSEIGIPUSU,     HAND_ADD_ITEM_KyouseiGipusu     },
  { ITEM_SENSEINOTUME,      HAND_ADD_ITEM_SenseiNoTume      },
  { ITEM_KOUKOUNOSIPPO,     HAND_ADD_ITEM_KoukouNoSippo     },
  { ITEM_MANPUKUOKOU,       HAND_ADD_ITEM_KoukouNoSippo     },  // まんぷくおこう=こうこうのしっぽ等価
  { ITEM_OUZYANOSIRUSI,     HAND_ADD_ITEM_OujaNoSirusi      },
  { ITEM_SURUDOITUME,       HAND_ADD_ITEM_PintLens          },  // するどいツメ = ピントレンズと等価
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
  { ITEM_KUROIHEDORO,       HAND_ADD_ITEM_KuroiHedoro       },
  { ITEM_KODAWARIMEGANE,    HAND_ADD_ITEM_KodawariMegane    },
  { ITEM_KODAWARISUKAAHU,   HAND_ADD_ITEM_KodawariScarf     },
  { ITEM_GINNOKONA,         HAND_ADD_ITEM_GinNoKona         },
  { ITEM_YAWARAKAISUNA,     HAND_ADD_ITEM_YawarakaiSuna     },
  { ITEM_KATAIISI,          HAND_ADD_ITEM_KataiIsi          },
  { ITEM_KISEKINOTANE,      HAND_ADD_ITEM_KisekiNoTane      },
  { ITEM_KUROIMEGANE,       HAND_ADD_ITEM_KuroiMegane       },
  { ITEM_KUROOBI,           HAND_ADD_ITEM_Kuroobi           },
  { ITEM_ZISYAKU,           HAND_ADD_ITEM_Zisyaku           },
  { ITEM_METARUKOOTO,       HAND_ADD_ITEM_MetalCoat         },
  { ITEM_SINPINOSIZUKU,     HAND_ADD_ITEM_SinpiNoSizuku     },
  { ITEM_SURUDOIKUTIBASI,   HAND_ADD_ITEM_SurudoiKutibasi   },
  { ITEM_SURUDOIKIBA,       HAND_ADD_ITEM_SurudoiKiba       },
  { ITEM_DOKUBARI,          HAND_ADD_ITEM_Dokubari          },
  { ITEM_TOKENAIKOORI,      HAND_ADD_ITEM_TokenaiKoori      },
  { ITEM_NOROINOOHUDA,      HAND_ADD_ITEM_NoroiNoOfuda      },
  { ITEM_MAGATTASUPUUN,     HAND_ADD_ITEM_MagattaSupuun     },
  { ITEM_MOKUTAN,           HAND_ADD_ITEM_Mokutan           },
  { ITEM_RYUUNOKIBA,        HAND_ADD_ITEM_RyuunoKiba        },
  { ITEM_SIRUKUNOSUKAAHU,   HAND_ADD_ITEM_SirukuNoSukaafu   },
  { ITEM_AYASIIOKOU,        HAND_ADD_ITEM_AyasiiOkou        },
  { ITEM_GANSEKIOKOU,       HAND_ADD_ITEM_GansekiOkou       },
  { ITEM_SAZANAMINOOKOU,    HAND_ADD_ITEM_SazanamiNoOkou    },
  { ITEM_USIONOOKOU,        HAND_ADD_ITEM_UsioNoOkou        },
  { ITEM_OHANANOOKOU,       HAND_ADD_ITEM_OhanaNoOkou       },
  { ITEM_KIAINOTASUKI,      HAND_ADD_ITEM_KiaiNoTasuki      },
  { ITEM_KIAINOHATIMAKI,    HAND_ADD_ITEM_KiaiNoHachimaki   },
  { ITEM_TATUZINNOOBI,      HAND_ADD_ITEM_TatsujinNoObi     },
  { ITEM_INOTINOTAMA,       HAND_ADD_ITEM_InochiNoTama      },
  { ITEM_METORONOOMU,       HAND_ADD_ITEM_MetroNome         },
  { ITEM_NEBARINOKAGIDUME,  HAND_ADD_ITEM_NebariNoKagidume  },
  { ITEM_KAIGARANOSUZU,     HAND_ADD_ITEM_KaigaraNoSuzu     },
  { ITEM_HIKARINONENDO,     HAND_ADD_ITEM_HikariNoNendo     },
  { ITEM_PAWAHURUHAABU,     HAND_ADD_ITEM_PowefulHarb       },
  { ITEM_TABENOKOSI,        HAND_ADD_ITEM_Tabenokosi        },
  { ITEM_DOKUDOKUDAMA,      HAND_ADD_ITEM_DokudokuDama      },
  { ITEM_KAENDAMA,          HAND_ADD_ITEM_KaenDama          },
  { ITEM_SIRATAMA,          HAND_ADD_ITEM_Siratama          },
  { ITEM_KONGOUDAMA,        HAND_ADD_ITEM_Kongoudama        },
  { ITEM_KUROITEKKYUU,      HAND_ADD_ITEM_KuroiTekkyuu      },
  { ITEM_AKAIITO,           HAND_ADD_ITEM_AkaiIto           },
  { ITEM_TUMETAIIWA,        HAND_ADD_ITEM_TumetaiIwa        },
  { ITEM_SARASARAIWA,       HAND_ADD_ITEM_SarasaraIwa       },
  { ITEM_ATUIIWA,           HAND_ADD_ITEM_AtuiIwa           },
  { ITEM_SIMETTAIWA,        HAND_ADD_ITEM_SimettaIwa        },
  { ITEM_KUTTUKIBARI,       HAND_ADD_ITEM_KuttukiBari       },
  { ITEM_PAWAARISUTO,       HAND_ADD_ITEM_PowerWrist        },
  { ITEM_PAWAABERUTO,       HAND_ADD_ITEM_PowerBelt         },
  { ITEM_PAWAARENZU,        HAND_ADD_ITEM_PowerLens         },
  { ITEM_PAWAABANDO,        HAND_ADD_ITEM_PowerBand         },
  { ITEM_PAWAAANKURU,       HAND_ADD_ITEM_PowerAnkle        },
  { ITEM_PAWAAUEITO,        HAND_ADD_ITEM_PowerWeight       },
  { ITEM_HINOTAMAPUREETO,   HAND_ADD_ITEM_HinotamaPlate     },
  { ITEM_SIZUKUPUREETO,     HAND_ADD_ITEM_SizukuPlate       },
  { ITEM_IKAZUTIPUREETO,    HAND_ADD_ITEM_IkazutiPlate      },
  { ITEM_MIDORINOPUREETO,   HAND_ADD_ITEM_MirodinoPlate     },
  { ITEM_TURARANOPUREETO,   HAND_ADD_ITEM_TuraranoPlate     },
  { ITEM_KOBUSINOPUREETO,   HAND_ADD_ITEM_KobusinoPlate     },
  { ITEM_MOUDOKUPUREETO,    HAND_ADD_ITEM_MoudokuPlate      },
  { ITEM_DAITINOPUREETO,    HAND_ADD_ITEM_DaitinoPlate      },
  { ITEM_AOZORAPUREETO,     HAND_ADD_ITEM_AozoraPlate       },
  { ITEM_HUSIGINOPUREETO,   HAND_ADD_ITEM_HusiginoPlate     },
  { ITEM_TAMAMUSIPUREETO,   HAND_ADD_ITEM_TamamusiPlate     },
  { ITEM_GANSEKIPUREETO,    HAND_ADD_ITEM_GansekiPlate      },
  { ITEM_MONONOKEPUREETO,   HAND_ADD_ITEM_MononokePlate     },
  { ITEM_RYUUNOPUREETO,     HAND_ADD_ITEM_RyuunoPlate       },
  { ITEM_KOWAMOTEPUREETO,   HAND_ADD_ITEM_KowamotePlate     },
  { ITEM_KOUTETUPUREETO,    HAND_ADD_ITEM_KoutetsuPlate     },
  { ITEM_OOKINANEKKO,       HAND_ADD_ITEM_OokinaNekko       },
  { ITEM_KEMURIDAMA,        HAND_ADD_ITEM_Kemuridama        },
  { ITEM_OMAMORIKOBAN,      HAND_ADD_ITEM_OmamoriKoban      },
  { ITEM_KOUUNNOOKOU,       HAND_ADD_ITEM_OmamoriKoban      },
  { ITEM_HAKKINDAMA,        HAND_ADD_ITEM_HakkinDama        },


  { ITEM_KARUISI,           HAND_ADD_ITEM_Karuisi         },  // かるいし
  { ITEM_SINKANOKISEKI,     HAND_ADD_ITEM_SinkanoKiseki   },  // しんかのきせき
  { ITEM_GOTUGOTUMETTO,     HAND_ADD_ITEM_GotugotuMet     },  // ゴツゴツメット
  { ITEM_HUUSEN,            HAND_ADD_ITEM_Huusen          },  // ふうせん
  { ITEM_REDDOKAADO,        HAND_ADD_ITEM_RedCard         },  // レッドカード
  { ITEM_NERAINOMATO,       HAND_ADD_ITEM_NerainoMato     },  // ねらいのまと
  { ITEM_SIMETUKEBANDO,     HAND_ADD_ITEM_SimetukeBand    },  // しめつけバンド
  { ITEM_KYUUKON,           HAND_ADD_ITEM_Kyuukon         },  // きゅうこん
  { ITEM_ZYUUDENTI,         HAND_ADD_ITEM_Juudenti        },  // じゅうでんち
  { ITEM_DASSYUTUBOTAN,     HAND_ADD_ITEM_DassyutuButton  },  // だっしゅつボタン
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

//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   bpp
 * @param   itemID
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* AddItemEventCore( const BTL_POKEPARAM* bpp, u16 itemID )
{
  u32 i;
  for(i=0; i<NELEMS(EventAddFuncTbl); ++i)
  {
    if( EventAddFuncTbl[i].itemID == itemID )
    {
      u16 priority = BPP_GetValue_Base( bpp, BPP_AGILITY );
      u8 pokeID = BPP_GetID( bpp );
      u32 numHandlers;
      const BtlEventHandlerTable* handlerTable;

      handlerTable = EventAddFuncTbl[i].func( &numHandlers );
      return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID,
              BTL_EVPRI_ITEM_DEFAULT, priority, pokeID, handlerTable, numHandlers );
    }
  }
  return NULL;
}
//=============================================================================================
/**
 * 特定ポケモンの「アイテム」ハンドラをシステムから全て削除
 *
 * @param   pp
 *
 */
//=============================================================================================
void BTL_HANDLER_ITEM_Remove( const BTL_POKEPARAM* bpp )
{
  BTL_EVENT_FACTOR *factor, *next;
  u8 pokeID = BPP_GetID( bpp );

  factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_ITEM, pokeID );
  while( factor )
  {
    next = BTL_EVENT_GetNextFactor( factor );
    if( BTL_EVENT_FACTOR_GetWorkValue(factor, WORKIDX_TMP_FLAG) == 0 ){
      BTL_EVENT_FACTOR_Remove( factor );
    }
    factor = next;
  }
}
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
    return AddItemEventCore( bpp, itemID );
  }
  return NULL;
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
    BTL_EVENT_FACTOR* factor = AddItemEventCore( bpp, itemID );
    if( factor )
    {
      BTL_EVENT_FACTOR_SetTmpItemFlag( factor );
      BTL_EVENT_FACTOR_SetWorkValue( factor, WORKIDX_TMP_FLAG, TRUE );
      return factor;
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


//=============================================================================================
/**
 * 指定ポケモンのアイテムハンドラを一時的に休止（ローテーション用）
 *
 * @param   bpp
 */
//=============================================================================================
void BTL_HANDLER_ITEM_RotationSleep( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = BPP_GetID( bpp );
  BTL_EVENT_SleepFactorRotation( pokeID, BTL_EVENT_FACTOR_ITEM );
}
//=============================================================================================
/**
 * 指定ポケモンのアイテムハンドラを休止から復帰（ローテーション用）
 *
 * @param   bpp
 */
//=============================================================================================
void BTL_HANDLER_ITEM_RotationWake( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = BPP_GetID( bpp );
  if( BTL_EVENT_WakeFactorRotation(pokeID, BTL_EVENT_FACTOR_ITEM) == FALSE )
  {
    BTL_HANDLER_ITEM_Add( bpp );
  }
}

//----------------------------------------------------------------------------------
/**
 * ランダム発動する装備効果の共通チェックルーチン
 *
 * @param   flowWk
 * @param   per
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL Item_IsExePer( BTL_SVFLOW_WORK* flowWk, u8 per )
{
  if( BTL_CALC_IsOccurPer(per) ){
    return TRUE;
  }
  if( BTL_SVFTOOL_GetDebugFlag(flowWk, BTL_DEBUGFLAG_MUST_ITEM) ){
    return TRUE;
  }
  return FALSE;
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
static s32 common_GetItemParam( BTL_EVENT_FACTOR* myHandle, u16 paramID )
{
  u16     itemID = BTL_EVENT_FACTOR_GetSubID( myHandle );
  return BTL_CALC_ITEM_GetParam( itemID, paramID );
}

//----------------------------------------------------------------------------------
/**
 * アイテムパラメータの「威力値」を、威力・命中率などの倍率に変換する際の共通ルーチン
 *
 * @param   itemID    アイテムID
 *
 * @retval  fx32      「威力値」を倍率値に変換したもの。
 *                    例）威力値20 なら FX32_CONST(1.2）相当の値が返る
 */
//----------------------------------------------------------------------------------
static fx32 Item_AttackValue_to_Ratio( BTL_EVENT_FACTOR* myHandle )
{
  int pow = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
  return FX32_CONST(100+pow) / 100;
}

//----------------------------------------------------------------------------------
/**
 * 自分自身の再呼び出し許可後、アイテム使用イベント呼び出し
 *
 * @param   myHandle
 * @param   flowWk
 * @param   pokeID
 */
//----------------------------------------------------------------------------------
static void ItemPushRun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID )
{
  BTL_EVENT_FACTOR_SetRecallEnable( myHandle );
  BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
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
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_MAHI );
}

static const BtlEventHandlerTable* HAND_ADD_ITEM_KuraboNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_KuraboNomi     }, // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KuraboNomi     }, // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_KuraboNomi     },
    { BTL_EVENT_USE_ITEM,            handler_KuraboNomi_Use }, // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_KuraboNomi_Use }, // 強制一時使用（ついばむなど）
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_KagoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_KagoNomi     }, // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KagoNomi     }, // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_KagoNomi     },
    { BTL_EVENT_USE_ITEM,            handler_KagoNomi_Use }, // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_KagoNomi_Use }, // 強制一時使用（ついばむなど）
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_ChigoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_ChigoNomi     },  // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_ChigoNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_ChigoNomi     },
    { BTL_EVENT_USE_ITEM,            handler_ChigoNomi_Use },  // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_ChigoNomi_Use },  // 強制一時使用（ついばむなど）
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_NanasiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_NanasiNomi     }, // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_NanasiNomi     }, // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_NanasiNomi     },
    { BTL_EVENT_USE_ITEM,            handler_NanasiNomi_Use }, // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_NanasiNomi_Use }, // 強制一時使用（ついばむなど）
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_KiiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_KiiNomi     }, // メンバー入場チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KiiNomi     }, // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_KiiNomi     },
    { BTL_EVENT_USE_ITEM,            handler_KiiNomi_Use }, // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_KiiNomi_Use }, // 強制一時使用（ついばむなど）
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_MomonNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_MomonNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_MomonNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_MomonNomi     },
    { BTL_EVENT_USE_ITEM,            handler_MomonNomi_Use },  // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_MomonNomi_Use },  // 強制一時使用（ついばむなど）
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_RamNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_RamNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_RamNomi     },  // 状態異常チェックハンドラ
    { BTL_EVENT_ACTPROC_END,         handler_RamNomi     },
    { BTL_EVENT_USE_ITEM,            handler_RamNomi_Use },  // 通常道具使用
    { BTL_EVENT_USE_ITEM_TMP,        handler_RamNomi_Use },  // 強制一時使用（ついばむなど）
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
    // 該当する反応タイプでなければ何もしない
    int reactionType;
    if( BTL_EVENTVAR_GetValueIfExist(BTL_EVAR_ITEM_REACTION, &reactionType) )
    {
      if( (reactionType != BTL_ITEMREACTION_SICK)
      &&  (reactionType != BTL_ITEMREACTION_GEN)
      ){
        return;
      }
    }

    if( common_sickcode_match(flowWk, pokeID, sickCode) )
    {
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
static void common_useForSick( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_sickcode_match(flowWk, pokeID, sickCode) )
    {
      BTL_HANDEX_PARAM_CURE_SICK* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
        param->sickCode = sickCode;
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
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
  case WAZASICK_EX_MENTAL:
    {
      WazaSick sickID = BTL_CALC_CheckMentalSick( bpp );
      if( sickID != WAZASICK_NULL ){
        return TRUE;
      }
      return FALSE;
    }
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_HimeriNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASEQ_END,            handler_HimeriNomi_wazaEnd },
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_HimeriNomi_reaction},
    { BTL_EVENT_MEMBER_IN,              handler_HimeriNomi_get     },
    { BTL_EVENT_ITEMSET_FIXED,          handler_HimeriNomi_get     },
    { BTL_EVENT_DECREMENT_PP_DONE,      handler_HimeriNomi_ppDec   },
    { BTL_EVENT_USE_ITEM,               handler_HimeriNomi_Use     },
    { BTL_EVENT_USE_ITEM_TMP,           handler_HimeriNomi_UseTmp  },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ最後
static void handler_HimeriNomi_wazaEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_Himeri_LastWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
    {
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// アイテム反応汎用
static void handler_HimeriNomi_reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlItemReaction reactionType = BTL_EVENTVAR_GetValue( BTL_EVAR_ITEM_REACTION );
    if( (reactionType == BTL_ITEMREACTION_PP)
    ||  (reactionType == BTL_ITEMREACTION_GEN)
    ){
      if( common_Himeri_EnableWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
      {
        ItemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
// アイテム入手
static void handler_HimeriNomi_get( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  ){
    if( common_Himeri_EnableWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
    {
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// PPが削られた
static void handler_HimeriNomi_ppDec( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_Himeri_LastWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
    }
  }
}
static void handler_HimeriNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    handler_HimeriNomi_common( myHandle, flowWk, pokeID, work, FALSE );
  }
}
static void handler_HimeriNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] )
  {
    handler_HimeriNomi_common( myHandle, flowWk, pokeID, work, TRUE );
  }
}
/**
 *  ヒメリのみ用：直前に使ったワザのPPが0の場合、そのワザのIndexを返す／それ以外 PTL_WAZA_MAX を返す
 */
static u8 common_Himeri_LastWazaIdx( BTL_SVFLOW_WORK* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      // 直前に使ったワザのPPが0なら最優先
  WazaID waza = BPP_GetPrevOrgWazaID( bpp );
  u8  wazaIdx = BPP_WAZA_SearchIdx( bpp, waza );

  if( (wazaIdx != PTL_WAZA_MAX)
  &&  (BPP_WAZA_GetPP(bpp, wazaIdx) == 0)
  ){
    return wazaIdx;
  }

  return PTL_WAZA_MAX;
}
/**
 *  ヒメリのみ用：直前に使ったワザのPPが0の場合、そのワザのIndexを返す／それ以外 PTL_WAZA_MAX を返す
 */
static u8 common_Himeri_EnableWazaIdx( BTL_SVFLOW_WORK* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

  u32 waza_cnt = BPP_WAZA_GetCount( bpp );
  u32 i;

  for(i=0; i<waza_cnt; ++i)
  {
    if(BPP_WAZA_GetPP( bpp, i ) == 0){
      return i;
    }
  }

  return PTL_WAZA_MAX;
}
/**
 *  いろんな使われ方をするヒメリのみの共通処理
 */
static BOOL handler_HimeriNomi_common( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, BOOL fUseTmp )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  u8 wazaIdx = PTL_WAZA_MAX;

  do{
      // 直前に使ったワザのPPが0なら最優先
      wazaIdx = common_Himeri_LastWazaIdx( flowWk, pokeID );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        break;
      }

      // それ以外にPP=0のワザがあれば使う（0番から順サーチ）
      wazaIdx = common_Himeri_EnableWazaIdx( flowWk, pokeID );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        break;
      }

      // ついばむ・なげつける効果の時はPPが１つでも減っていたら使う（0番から順サーチ）
      if( fUseTmp )
      {
        u32 waza_cnt = BPP_WAZA_GetCount( bpp );
        u32 i;

        for(i=0; i<waza_cnt; ++i)
        {
          if(BPP_WAZA_GetPPShort(bpp, i) != 0){
            wazaIdx = i;
            break;
          }
        }
      }

  }while(0);

  if( wazaIdx != PTL_WAZA_MAX )
  {
    BTL_HANDEX_PARAM_PP* param;

    if( fUseTmp )
    {
      // 一時利用の場合はココでエフェクト出さないといけない…
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM_EFFECT, pokeID );
    }

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_PP, pokeID );
      param->volume = 10;
      param->pokeID = pokeID;
      param->wazaIdx = wazaIdx;
      param->fSurfacePP = TRUE;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverPP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      HANDEX_STR_AddArg( &param->exStr, BPP_WAZA_GetID(bpp, wazaIdx) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 *  オレンのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_OrenNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_OrenNomi_MemberIn   },
    { BTL_EVENT_ROTATION_IN,            handler_OrenNomi_RotationIn },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use        },
    { BTL_EVENT_USE_ITEM_TMP,           handler_OrenNomi_Use        },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_OrenNomi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  if( !BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) ){
    common_DamageReact( myHandle, flowWk, pokeID, 2, TRUE );
  }
}
static void handler_OrenNomi_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  if( !BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) ){
    common_DamageReact( myHandle, flowWk, pokeID, 2, FALSE );
  }
}
static void handler_OrenNomi_RotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  if( !BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) )
  {
    if( common_DamageReactCheckCore(flowWk, pokeID, 2) ){
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}


static void handler_OrenNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = pokeID;
      param->recoverHP = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverHP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  きのみジュース
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KinomiJuice( u32* numElems )
{
  // 投げつけ効果以外は“オレンのみ”と同じで良い
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction },
    { BTL_EVENT_MEMBER_IN,              handler_OrenNomi_MemberIn },
    { BTL_EVENT_ROTATION_IN,            handler_OrenNomi_RotationIn },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use      },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  オボンのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_ObonNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_OrenNomi_MemberIn   },
    { BTL_EVENT_ROTATION_IN,            handler_OrenNomi_RotationIn },
    { BTL_EVENT_USE_ITEM,               handler_ObonNomi_Use      },
    { BTL_EVENT_USE_ITEM_TMP,           handler_ObonNomi_Use      },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_ObonNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param;
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 maxHP = BPP_GetValue( bpp, BPP_MAX_HP );
    u8 ratio = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = pokeID;
      param->recoverHP = (maxHP * ratio) / 100;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverHP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_FiraNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn   },
    { BTL_EVENT_ROTATION_IN,            handler_common_KaifukuKonran_RotationIn },
    { BTL_EVENT_USE_ITEM,               handler_FiraNomi_Use                    },
    { BTL_EVENT_USE_ITEM_TMP,           handler_FiraNomi_UseTmp                 },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_FiraNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_KARAI );
}
static void handler_FiraNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_KARAI );
  }
}

//------------------------------------------------------------------------------
/**
 *  ウイのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_UiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn   },
    { BTL_EVENT_ROTATION_IN,            handler_common_KaifukuKonran_RotationIn },
    { BTL_EVENT_USE_ITEM,               handler_UiNomi_Use                      },
    { BTL_EVENT_USE_ITEM_TMP,           handler_UiNomi_UseTmp                   },

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_UiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_SIBUI );
}
static void handler_UiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_SIBUI );
  }
}
//------------------------------------------------------------------------------
/**
 *  マゴのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MagoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn   },
    { BTL_EVENT_ROTATION_IN,            handler_common_KaifukuKonran_RotationIn },
    { BTL_EVENT_USE_ITEM,               handler_MagoNomi_Use                    },
    { BTL_EVENT_USE_ITEM_TMP,           handler_MagoNomi_UseTmp                 },

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MagoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_AMAI );
}
static void handler_MagoNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_AMAI );
  }
}

//------------------------------------------------------------------------------
/**
 *  バンジのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_BanziNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn },
    { BTL_EVENT_ROTATION_IN,            handler_common_KaifukuKonran_RotationIn },
    { BTL_EVENT_USE_ITEM,               handler_BanjiNomi_Use                 },
    { BTL_EVENT_USE_ITEM_TMP,           handler_BanjiNomi_UseTmp              },

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_BanjiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_NIGAI );
}
static void handler_BanjiNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_NIGAI );
  }
}

//------------------------------------------------------------------------------
/**
 *  イアのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_IaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction   },
    { BTL_EVENT_MEMBER_IN,              handler_common_KaifukuKonran_MemberIn   },
    { BTL_EVENT_ROTATION_IN,            handler_common_KaifukuKonran_RotationIn },
    { BTL_EVENT_USE_ITEM,               handler_IaNomi_Use                      },
    { BTL_EVENT_USE_ITEM_TMP,           handler_IaNomi_UseTmp                   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_IaNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_SUPPAI );
}
static void handler_IaNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_SUPPAI );
  }
}

/**
 *  回復こんらん木の実の共通リアクションハンドラ
 */
static void handler_common_KaifukuKonran_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) ){
      common_DamageReact( myHandle, flowWk, pokeID, 2, TRUE );
    }
  }
}
/**
 *  回復こんらん木の実の共通メンバー入場時ハンドラ
 */
static void handler_common_KaifukuKonran_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) ){
      common_DamageReact( myHandle, flowWk, pokeID, 2, FALSE );
    }
  }
}
/**
 *  回復こんらん木の実の共通ローテーションインハンドラ
 */
static void handler_common_KaifukuKonran_RotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) )
    {
      if( common_DamageReactCheckCore(flowWk, pokeID, 2) ){
        ItemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
/**
 *  回復こんらん木の実の共通どうぐ使用ハンドラ
 */
static void common_KaifukuKonran( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PtlTaste taste )
{
  if( work[0] == 0 )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
    {
      BTL_HANDEX_PARAM_RECOVER_HP* param;
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
        param->pokeID = pokeID;
        param->recoverHP = BTL_CALC_QuotMaxHP( bpp, common_GetItemParam(myHandle, ITEM_PRM_ATTACK) );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverHP );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      BTL_SVF_HANDEX_Pop( flowWk, param );

      if( PP_CheckDesiredTaste(BPP_GetSrcData(bpp), taste) == PTL_TASTE_DISLIKE )
      {
        BTL_HANDEX_PARAM_ADD_SICK* sick_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
          sick_param->pokeID = pokeID;
          sick_param->sickID = WAZASICK_KONRAN;
          BTL_CALC_MakeDefaultWazaSickCont( sick_param->sickID, bpp, &(sick_param->sickCont) );
        BTL_SVF_HANDEX_Pop( flowWk, sick_param );
      }

      work[0] = 1;
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  チイラのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TiiraNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,              handler_PinchReactMemberIn   },
    { BTL_EVENT_ROTATION_IN,            handler_PinchReactRotationIn },
    { BTL_EVENT_USE_ITEM,               handler_TiiraNomi            },
    { BTL_EVENT_USE_ITEM_TMP,           handler_TiiraNomi            },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_RyugaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,              handler_PinchReactMemberIn   },
    { BTL_EVENT_ROTATION_IN,            handler_PinchReactRotationIn },
    { BTL_EVENT_USE_ITEM,               handler_RyugaNomi            },
    { BTL_EVENT_USE_ITEM_TMP,           handler_RyugaNomi            },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_KamuraNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,              handler_PinchReactMemberIn   },
    { BTL_EVENT_ROTATION_IN,            handler_PinchReactRotationIn },
    { BTL_EVENT_USE_ITEM,               handler_KamuraNomi           },
    { BTL_EVENT_USE_ITEM_TMP,           handler_KamuraNomi           },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_YatapiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,     handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,               handler_PinchReactMemberIn   },
    { BTL_EVENT_ROTATION_IN,             handler_PinchReactRotationIn },
    { BTL_EVENT_USE_ITEM,                handler_YatapiNomi           },
    { BTL_EVENT_USE_ITEM_TMP,            handler_YatapiNomi           },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_ZuaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,     handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,               handler_PinchReactMemberIn   },
    { BTL_EVENT_ROTATION_IN,             handler_PinchReactRotationIn },
    { BTL_EVENT_USE_ITEM,                handler_ZuaNomi              },
    { BTL_EVENT_USE_ITEM_TMP,            handler_ZuaNomi              },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_SanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,     handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,               handler_PinchReactMemberIn   },
    { BTL_EVENT_ROTATION_IN,             handler_PinchReactRotationIn },
    { BTL_EVENT_USE_ITEM,                handler_SanNomi              },
    { BTL_EVENT_USE_ITEM_TMP,            handler_SanNomi              },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_KIAIDAME) )
    {
      BTL_HANDEX_PARAM_SET_CONTFLAG* param;
      BTL_HANDEX_PARAM_MESSAGE* msg_param;

      param = (BTL_HANDEX_PARAM_SET_CONTFLAG*)BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SET_CONTFLAG, pokeID );
        param->pokeID = pokeID;
        param->flag = BPP_CONTFLG_KIAIDAME;
      BTL_SVF_HANDEX_Pop( flowWk, param );

      msg_param = (BTL_HANDEX_PARAM_MESSAGE*)BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Rankup_Critical );
        HANDEX_STR_AddArg( &msg_param->str, pokeID );
        HANDEX_STR_AddArg( &msg_param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      BTL_SVF_HANDEX_Pop( flowWk, msg_param );
    }
  }
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_SutaaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,     handler_PinchReactCommon     },
    { BTL_EVENT_MEMBER_IN,               handler_PinchReactMemberIn   },
    { BTL_EVENT_ROTATION_IN,             handler_PinchReactRotationIn },
    { BTL_EVENT_USE_ITEM,                handler_SutaaNomi            },
    { BTL_EVENT_USE_ITEM_TMP,            handler_SutaaNomi            },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SutaaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u8 rankTypeTbl[] = {
    WAZA_RANKEFF_ATTACK, WAZA_RANKEFF_DEFENCE,
    WAZA_RANKEFF_SP_ATTACK, WAZA_RANKEFF_SP_DEFENCE,
    WAZA_RANKEFF_AGILITY,
  };

  u32 cnt, i;
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  u8* typeAry = BTL_SVFTOOL_GetTmpWork( flowWk, sizeof(rankTypeTbl) );

  for(i=0, cnt=0; i<NELEMS(rankTypeTbl); ++i)
  {
    if( BPP_IsRankEffectValid(bpp, rankTypeTbl[i], 1) )
    {
      typeAry[ cnt++ ] = rankTypeTbl[ i ];
    }
  }

  if( cnt )
  {
    i = BTL_CALC_GetRand( cnt );
    common_PinchRankup( flowWk, pokeID, typeAry[i], 2 );
  }
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
//      param->fAlmost = TRUE;
      param->pokeID[0] = pokeID;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  ナゾのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_NazoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_NazoNomi     },
    { BTL_EVENT_USE_ITEM,              handler_NazoNomi_Use },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_NazoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == FALSE)
  ){
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > BTL_TYPEAFF_100 )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( !BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) ){
        ItemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
static void handler_NazoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 item_pow = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );

    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      param->pokeID = pokeID;
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, item_pow );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  オッカのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_OkkaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,       handler_OkkaNomi                },
    { BTL_EVENT_PREV_WAZA_DMG,        handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_OkkaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_HONOO, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  イトケのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_ItokeNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,     handler_ItokeNomi               },
    { BTL_EVENT_PREV_WAZA_DMG,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_ItokeNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_MIZU, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  ソクノのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SokunoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,       handler_SokunoNomi },
    { BTL_EVENT_PREV_WAZA_DMG,        handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SokunoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_DENKI, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  リンドのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_RindoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,       handler_RindoNomi },
    { BTL_EVENT_PREV_WAZA_DMG,        handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_RindoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_KUSA, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  ヤチェのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_YacheNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,          handler_YacheNomi },
    { BTL_EVENT_PREV_WAZA_DMG,       handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_YacheNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_KOORI, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  ヨプのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_YopuNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_YopuNomi },
    { BTL_EVENT_PREV_WAZA_DMG,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_YopuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_KAKUTOU, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  ビアーのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_BiarNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_BiarNomi },
    { BTL_EVENT_PREV_WAZA_DMG,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_BiarNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_DOKU, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  シュカのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SyukaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_SyukaNomi },
    { BTL_EVENT_PREV_WAZA_DMG,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SyukaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_JIMEN, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  バコウのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_BakouNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_BakouNomi },
    { BTL_EVENT_PREV_WAZA_DMG,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_BakouNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  ウタンのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_UtanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_UtanNomi },
    { BTL_EVENT_PREV_WAZA_DMG,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_UtanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_ESPER, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  タンガのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TangaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_TangaNomi },
    { BTL_EVENT_PREV_WAZA_DMG,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_TangaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_MUSHI, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  ヨロギのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_YorogiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_YorogiNomi },
    { BTL_EVENT_PREV_WAZA_DMG,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_YorogiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_IWA, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  カシブのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KasibuNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_KasibuNomi },
    { BTL_EVENT_PREV_WAZA_DMG,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KasibuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_GHOST, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  ハバンのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_HabanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_HabanNomi },
    { BTL_EVENT_PREV_WAZA_DMG,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_HabanNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_DRAGON, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  ナモのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_NamoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,         handler_NamoNomi },
    { BTL_EVENT_PREV_WAZA_DMG,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_NamoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_AKU, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  リリバのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_RiribaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_RiribaNomi },
    { BTL_EVENT_PREV_WAZA_DMG,     handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_RiribaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_HAGANE, FALSE );
}
//------------------------------------------------------------------------------
/**
 *  ホズのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_HozuNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,     handler_HozuNomi },
    { BTL_EVENT_PREV_WAZA_DMG,      handler_common_WeakAff_DmgAfter },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_HozuNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WeakAff_Relieve( myHandle, flowWk, pokeID, work, POKETYPE_NORMAL, TRUE );
}
/**
 *  威力半減させたらTRUEを返す
 */
static BOOL common_WeakAff_Relieve( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type, BOOL fIgnoreAffine )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  &&  ((fIgnoreAffine) || (BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > BTL_TYPEAFF_100))
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_MIGAWARI_IsExist(bpp) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
      if( !BTL_SVFTOOL_IsSimulationMode(flowWk) ){
        work[0] = 1;
      }
      return TRUE;
    }
  }
  return FALSE;
}
static void handler_common_WeakAff_DmgAfter( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] )
  {
    if( HandCommon_CheckTargetPokeID(pokeID) )
    {
      BTL_HANDEX_PARAM_CONSUME_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CONSUME_ITEM, pokeID );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Item_DamageShrink );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ピンチ時（HP <= 1/アイテム威力値）に反応する木の実の共通処理（ダメージ反応）
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
    s32 item_pow = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
    common_DamageReact( myHandle, flowWk, pokeID, item_pow, TRUE );
  }
}
//----------------------------------------------------------------------------------
/**
 * ピンチ時（HP <= 1/アイテム威力値）に反応する木の実の共通処理（メンバー入場）
 *
 * @param   myHandle
 * @param   flowWk
 * @param   pokeID
 * @param   work
 */
//----------------------------------------------------------------------------------
static void handler_PinchReactMemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    s32 item_pow = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
    common_DamageReact( myHandle, flowWk, pokeID, item_pow, FALSE );
  }
}
//----------------------------------------------------------------------------------
/**
 * ピンチ時（HP <= 1/アイテム威力値）に反応する木の実の共通処理（ローテーションイン）
 *
 * @param   myHandle
 * @param   flowWk
 * @param   pokeID
 * @param   work
 */
//----------------------------------------------------------------------------------
static void handler_PinchReactRotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  s32 item_pow = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
  if( common_DamageReactCheckCore(flowWk, pokeID, item_pow) )
  {
    ItemPushRun( myHandle, flowWk, pokeID );
  }
}


//----------------------------------------------------------------------------------
/**
 * HP 1/n 以下で反応するアイテムの共通処理 - 条件を満たしていたらアイテム使用ハンドラ呼び出し
 *
 * @param   flowWk
 * @param   pokeID
 * @param   n
 */
//----------------------------------------------------------------------------------
static void common_DamageReact( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n, BOOL fCheckReactionType )
{

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BOOL fReaction;

    if( fCheckReactionType )
    {
      fReaction = common_DamageReactCheck( myHandle, flowWk, pokeID, n );
    }
    else
    {
      fReaction = common_DamageReactCheckCore( flowWk, pokeID, n );
    }

//    if( BTL_EVENT_FACTOR_GetPokeID

    if( fReaction )
    {
      BTL_N_PrintfSimple( DBGSTR_Item_PinchReactOn );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
//--------------------------------------------------------------
/**
 * HP 1/n 以下で反応するアイテムの共通処理 - 条件を満たしてるかチェック
 */
//--------------------------------------------------------------
static BOOL common_DamageReactCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 reactionType = BTL_EVENTVAR_GetValue( BTL_EVAR_ITEM_REACTION );
    if( (reactionType == BTL_ITEMREACTION_HP)
    ||  (reactionType == BTL_ITEMREACTION_GEN)
    ){
    return common_DamageReactCheckCore( flowWk, pokeID, n );
    }
  }
  return FALSE;
}
static BOOL common_DamageReactCheckCore( BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  u32 maxHP;

  if( BPP_GetValue(bpp, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_KUISINBOU )
  {
    if( n > 2 ){
      n /= 2;
    }
  }
  if( n == 0 ){
    GF_ASSERT(0);
    n = 1;
  }

  maxHP = BPP_GetValue( bpp, BPP_MAX_HP );
  if( maxHP > 1 )
  {
    u32 hp = BPP_GetValue( bpp, BPP_HP );
    if( hp <= BTL_CALC_QuotMaxHP_Zero(bpp, n) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 *  イバンのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_IbanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,    handler_IbanNomi_SpPriorityCheck  },  // 特殊行動プライオリティチェック
//    { BTL_EVENT_WORKED_SP_PRIORITY,   handler_IbanNomi_SpPriorityWorked },  // 特殊行動プライオリティ効果発生
//    { BTL_EVENT_ACTPROC_START,        handler_IbanNomi_ActStart         },  // アクション開始ハンドラ
    { BTL_EVENT_USE_ITEM,             handler_IbanNomi_Use              },  // アイテム使用ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 特殊行動プライオリティチェック
static void handler_IbanNomi_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_DamageReactCheckCore(flowWk, pokeID, 4) )
    {
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_SP_PRIORITY, BTL_SPPRI_HIGH) )
      {
        ItemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}

// アイテム使用ハンドラ
static void handler_IbanNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_PriorityUpOnce );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );

    work[0] = 0;
  }
}

//------------------------------------------------------------------------------
/**
 *  ミクルのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MikuruNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,   handler_MikuruNomi_Reaction   },
    { BTL_EVENT_MEMBER_IN,             handler_MikuruNomi_MemberIn   },
    { BTL_EVENT_ACTPROC_END,           handler_MikuruNomi_ActProcEnd },
    { BTL_EVENT_ROTATION_IN,           handler_MikuruNomi_RotationIn },
    { BTL_EVENT_USE_ITEM,              handler_MikuruNomi_Use        }, // アイテム使用ハンドラ
    { BTL_EVENT_USE_ITEM_TMP,          handler_MikuruNomi_UseTmp     }, // アイテム使用ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// アイテム反応チェック
static void handler_MikuruNomi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u8 quot = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
    if( common_DamageReactCheck(myHandle, flowWk, pokeID, quot) )
    {
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// メンバー入場
static void handler_MikuruNomi_MemberIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    handler_MikuruNomi_RotationIn( myHandle, flowWk, pokeID, work );
  }
}
// アクション終了
static void handler_MikuruNomi_ActProcEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_ACTION) != BTL_ACTION_ROTATION )
  {
    handler_MikuruNomi_MemberIn( myHandle, flowWk, pokeID, work );
  }
}
// ローテーション入場
static void handler_MikuruNomi_RotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  u8 quot = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
  if( common_DamageReactCheckCore(flowWk, pokeID, quot) )
  {
    ItemPushRun( myHandle, flowWk, pokeID );
  }
}
// アイテム使用ハンドラ
static void handler_MikuruNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->pokeID = pokeID;
      param->sickID = WAZASICK_HITRATIO_UP;
      param->sickCont = BPP_SICKCONT_MakePermanentParam( 120 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_HitRatioUpOnce );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
static void handler_MikuruNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_TMP_FLAG] ){
    handler_MikuruNomi_Use( myHandle, flowWk, pokeID, work );
  }
}


//------------------------------------------------------------------------------
/**
 *  ジャポのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_JapoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_JapoNomi_Damage     }, // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,          handler_JapoNomi_Use        }, // アイテム使用ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
static void handler_JapoNomi_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_JapoRenbu_Reaction( myHandle, flowWk, pokeID, work, WAZADATA_DMG_PHYSIC );
}
//------------------------------------------------------------------------------
/**
 *  レンブのみ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_RenbuNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_RenbuNomi_Damage     }, // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,          handler_JapoNomi_Use         }, // アイテム使用ハンドラ（ジャポのみと同じ）
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
static void handler_RenbuNomi_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_JapoRenbu_Reaction( myHandle, flowWk, pokeID, work, WAZADATA_DMG_SPECIAL );
}

/**
 *  ジャポのみ・レンブのみ共用：ダメージ反応処理
 */
static void common_JapoRenbu_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, WazaDamageType dmgType )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == FALSE)
  ){
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( (!BTL_TABLES_IsDelayAttackWaza(waza))
    &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == dmgType)
    ){
      BTL_HANDEX_PARAM_USE_ITEM* param;

      BTL_EVENT_FACTOR_SetRecallEnable( myHandle );
      work[0] = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK ) + 1;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
        param->fUseDead = TRUE;
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
/**
 *  ジャポのみ・レンブのみ共用：アイテム使用ハンドラ
 */
static void handler_JapoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[0] != 0 )
    {
      u8 targetPokeID = work[0] - 1;
      const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, targetPokeID );
      if( !BPP_IsDead(target) )
      {
        u8 quot = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );

        BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
          param->pokeID = targetPokeID;
          param->damage = BTL_CALC_QuotMaxHP( target, quot );
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_DamageOpponent );
          HANDEX_STR_AddArg( &param->exStr, param->pokeID );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
        BTL_SVF_HANDEX_Pop( flowWk, param );
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 *  しろいハーブ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SiroiHerb( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
//    { BTL_EVENT_CHECK_ITEM_REACTION, handler_SiroiHerb_TurnCheck },
//    { BTL_EVENT_RANKEFF_FIXED,       handler_SiroiHerb_TurnCheck },
    { BTL_EVENT_MEMBER_IN_COMP,      handler_SiroiHerb_MemberInComp },
    { BTL_EVENT_ACTPROC_END,         handler_SiroiHerb_ActEnd       },
    { BTL_EVENT_ROTATION_IN,         handler_SiroiHerb_MemberInComp },
    { BTL_EVENT_TURNCHECK_END,       handler_SiroiHerb_TurnCheck    },
    { BTL_EVENT_USE_ITEM,            handler_SiroiHerb_Use          },
    { BTL_EVENT_USE_ITEM_TMP,        handler_SiroiHerb_UseTmp       },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SiroiHerb_ActCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  if( BPP_IsRankEffectDowned(bpp) )
  {
    ItemPushRun( myHandle, flowWk, pokeID );
  }
}
static void handler_SiroiHerb_MemberInComp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  if( BPP_IsRankEffectDowned(bpp) )
  {
    ItemPushRun( myHandle, flowWk, pokeID );
  }
}
static void handler_SiroiHerb_ActEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_ACTION) != BTL_ACTION_ROTATION)
//  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  ){
    handler_SiroiHerb_MemberInComp( myHandle, flowWk, pokeID, work );
  }
}

static void handler_SiroiHerb_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_IsRankEffectDowned(bpp) )
    {
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
static void handler_SiroiHerb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_RECOVER_RANK* recover_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    TAYA_Printf("pokeID=%d, しろいハーブで回復するよ\n", pokeID);
    recover_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_RANK, pokeID );
      recover_param->pokeID = pokeID;
    BTL_SVF_HANDEX_Pop( flowWk, recover_param );

    TAYA_Printf("pokeID=%d, しろいハーブメッセージ出すよ\n", pokeID);

    msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_RankRecoverItem );
      HANDEX_STR_AddArg( &msg_param->str, pokeID );
      HANDEX_STR_AddArg( &msg_param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, msg_param );
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_MentalHerb( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,  handler_MentalHerb_React  }, // 状態異常チェックハンドラ
    { BTL_EVENT_MEMBER_IN,            handler_MentalHerb_React  },
    { BTL_EVENT_USE_ITEM,             handler_MentalHerb_Use    },
    { BTL_EVENT_USE_ITEM_TMP,         handler_MentalHerb_UseTmp },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MentalHerb_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BTL_CALC_CheckMentalSick(bpp) != WAZASICK_NULL )
    {
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
static void handler_MentalHerb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_useForSick( myHandle, flowWk, pokeID, WAZASICK_EX_MENTAL );
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_HikarinoKona( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_HikarinoKona },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_HikarinoKona( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    u32 per = 100 - common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
    fx32 r = FX32_CONST(per) / 100;
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, r );
  }
}
//------------------------------------------------------------------------------
/**
 *  きょうせいギプス
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KyouseiGipusu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,     handler_KyouseiGipusu },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_SenseiNoTume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,    handler_SenseiNoTume_SpPriorityCheck  },  // 特殊行動プライオリティチェック
//    { BTL_EVENT_WORKED_SP_PRIORITY,   handler_SenseiNoTume_SpPriorityWorked },  // 特殊行動プライオリティ効果発生
//    { BTL_EVENT_ACTPROC_START,        handler_SenseiNoTume_ActStart         },  // アクション開始ハンドラ
    { BTL_EVENT_USE_ITEM,             handler_SenseiNoTume_Use              },  // アイテム使用ハンドラ

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 特殊行動プライオリティチェック
static void handler_SenseiNoTume_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 per = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
    if( Item_IsExePer(flowWk, per) )
    {
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_SP_PRIORITY, BTL_SPPRI_HIGH) ){
        ItemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
// 特殊行動プライオリティ効果発生
static void handler_SenseiNoTume_SpPriorityWorked( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    work[0] = 1;
  }
}
// アクション開始ハンドラ
static void handler_SenseiNoTume_ActStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] == 1)
  ){
//    BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
    ItemPushRun( myHandle, flowWk, pokeID );
  }
}
// アイテム使用ハンドラ
static void handler_SenseiNoTume_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );

      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_PriorityUpOnce );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );

    BTL_SVF_HANDEX_Pop( flowWk, param );

    work[0] = 0;
  }
}

//------------------------------------------------------------------------------
/**
 *  こうこうのしっぽ、まんぷくおこう
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KoukouNoSippo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,      handler_KoukouNoSippo },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KoukouNoSippo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SP_PRIORITY, BTL_SPPRI_LOW );
  }
}
//------------------------------------------------------------------------------
/**
 *  おうじゃのしるし
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_OujaNoSirusi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_OujaNoSirusi        },
    { BTL_EVENT_USE_ITEM_TMP,        handler_OujaNoSirusi_UseTmp },   // なげつける等
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_OujaNoSirusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ひるみ確率０なら、アイテム威力値に書き換え
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_ADD_DEFAULT_PER) == 0 )
    {
      u8 per = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
      #ifdef PM_DEBUG
      if( BTL_SVFTOOL_GetDebugFlag(flowWk, BTL_DEBUGFLAG_MUST_ITEM) ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 100 );
        return;
      }
      #endif

      BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
      BTL_N_Printf( DBGSTR_HANDITEM_OujaEffective, per );
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
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  するどいキバ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SurudoiKiba( u32* numElems )
{
  // どちらも「おうじゃのしるし」と等価
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_OujaNoSirusi        },
    { BTL_EVENT_USE_ITEM_TMP,        handler_OujaNoSirusi_UseTmp },   // なげつける等
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  こうかくレンズ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KoukakuLens( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_KoukakuLens },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KoukakuLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側のとき
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 命中率上昇
    fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  ピントレンズ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PintLens( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,     handler_PintLens },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_LuckyPunch( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK, handler_LuckyPunch   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_Naganegi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK, handler_Naganegi   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_FocusLens( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_FocusLens },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_FocusLens( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) )
  {
    u8 def_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* defender = BTL_SVFTOOL_GetPokeParam( flowWk, def_pokeID );

    if( BPP_TURNFLAG_Get(defender, BPP_TURNFLG_ACTION_DONE) )
    {
      fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  のんきのおこう
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_NonkiNoOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_HIT_RATIO,     handler_NonkiNoOkou },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_NonkiNoOkou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
  {
    u16 ratio = 100 - common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
    fx32 fx_ratio = FX32_CONST(ratio) / 100;
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, fx_ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  ちからのハチマキ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_ChikaraNoHachimaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_ChikaraNoHachimaki },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ものしりメガネ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MonosiriMegane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MonosiriMegane },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  しんかいのキバ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SinkaiNoKiba( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_SinkaiNoKiba   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_SinkaiNoUroko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_SinkaiNoUroko   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_SPECIAL )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  メタルパウダー
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MetalPowder( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD, handler_MetalPowder   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MetalPowder( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が防御側で
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // メタモン（へんしん前）なら
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (BPP_GetMonsNo(bpp) == MONSNO_METAMON)
    &&  (!BPP_HENSIN_Check(bpp))
    ){
      // ぼうぎょ２倍
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_PHYSIC )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  スピードパウダー
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SpeedPowder( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY, handler_SpeedPowder   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SpeedPowder( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 自分がメタモンで変身前なら素早さ２倍
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( (BPP_GetMonsNo(bpp) == MONSNO_METAMON)
    &&  (!BPP_HENSIN_Check(bpp))
    ){
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  こころのしずく
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KokoroNoSizuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_KokoroNoSizuku_Pow },
    { BTL_EVENT_DEFENDER_GUARD, handler_KokoroNoSizuku_Guard },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
//      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
//      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_SPECIAL )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
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
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_SPECIAL )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ふといほね
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_FutoiHone( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER,     handler_FutoiHone },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariHachimaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,   handler_Kodawari_Common_WazaExe    }, // ワザ名出力確定ハンドラ
    { BTL_EVENT_ITEMSET_DECIDE,     handler_Kodawari_Common_ItemChange }, // アイテム変更確定ハンドラ
    { BTL_EVENT_ATTACKER_POWER,     handler_KodawariHachimaki_Pow      }, // 攻撃力決定ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 攻撃力決定ハンドラ
static void handler_KodawariHachimaki_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側なら
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 物理こうげき上昇
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こだわりメガネ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariMegane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,    handler_Kodawari_Common_WazaExe    }, // ワザ名出力確定ハンドラ
    { BTL_EVENT_ITEMSET_DECIDE,      handler_Kodawari_Common_ItemChange }, // アイテム変更確定ハンドラ
    { BTL_EVENT_ATTACKER_POWER,      handler_KodawariMegane_Pow         }, // 攻撃力決定ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 攻撃力決定ハンドラ
static void handler_KodawariMegane_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が攻撃側なら
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // 特殊こうげき上昇
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こだわりスカーフ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariScarf( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,    handler_Kodawari_Common_WazaExe      }, // ワザ名出力確定ハンドラ
    { BTL_EVENT_ITEMSET_DECIDE,      handler_Kodawari_Common_ItemChange   }, // アイテム変更確定ハンドラ
    { BTL_EVENT_CALC_AGILITY,        handler_KodawariScarf                }, // 素早さ計算ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 素早さ計算ハンドラ
static void handler_KodawariScarf( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    // すばやさ上昇
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
  }
}

// ワザ出し確定ハンドラ（こだわり系共通）
static void handler_Kodawari_Common_WazaExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  enum {
    KODAWARI_SICKID = WAZASICK_KODAWARI,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );

    if( waza != WAZANO_WARUAGAKI )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        param->pokeID = pokeID;
        param->sickID = KODAWARI_SICKID;
        param->sickCont = BPP_SICKCONT_MakePermanentParam( waza );
        param->overWriteMode = BTL_SICK_OW_STRONG;
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
// アイテム変更確定ハンドラ
static void handler_Kodawari_Common_ItemChange( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->sickCode = WAZASICK_KODAWARI;
      param->pokeID[0] = pokeID;
      param->poke_cnt = 1;
      param->fStdMsgDisable = TRUE;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}



//------------------------------------------------------------------------------
/**
 *  きあいのタスキ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoTasuki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK,  handler_KiaiNoTasuki_Check },    // こらえるチェックハンドラ
    { BTL_EVENT_KORAERU_EXE,    handler_KiaiNoTasuki_Exe      }, // こらえる発動ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// こらえるチェックハンドラ
static void handler_KiaiNoTasuki_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_IsHPFull(bpp) )
    {
      work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BPP_KORAE_ITEM );
    }
    else{
      work[0] = 0;
    }
  }
}
// こらえる発動ハンドラ
static void handler_KiaiNoTasuki_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( work[0] )
    {
      BTL_HANDEX_PARAM_CONSUME_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CONSUME_ITEM, pokeID );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_KoraeItem );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      BTL_SVF_HANDEX_Pop( flowWk, param );

      work[0] = 0;
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  きあいのハチマキ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoHachimaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK,  handler_KiaiNoHachimaki_Check   }, // こらえるチェックハンドラ
    { BTL_EVENT_KORAERU_EXE,    handler_KiaiNoHachimaki_Exe     }, // こらえる発動ハンドラ
    { BTL_EVENT_USE_ITEM,       handler_KiaiNoHachimaki_UseItem }, // アイテム使用ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// こらえるチェックハンドラ
static void handler_KiaiNoHachimaki_Check( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    u8 per = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
    if( Item_IsExePer(flowWk, per) )
    {
      work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BPP_KORAE_ITEM );
    }
    else{
      work[0] = 0;
    }
  }
}
// こらえる発動ハンドラ
static void handler_KiaiNoHachimaki_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] != 0)
  ){
//    BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
    ItemPushRun( myHandle, flowWk, pokeID );
    work[0] = 0;
  }
}
// アイテム使用ハンドラ
static void handler_KiaiNoHachimaki_UseItem( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_KoraeItem );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}

//------------------------------------------------------------------------------
/**
 *  たつじんのおび
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TatsujinNoObi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_TatsujinNoObi },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_TatsujinNoObi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) > BTL_TYPEAFF_100)
  ){
    fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  いのちのたま
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_InochiNoTama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L2,   handler_InochiNoTama_Reaction },   // ダメージ処理後ハンドラ
    { BTL_EVENT_WAZA_DMG_PROC2,          handler_InochiNoTama_Damage   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ処理後ハンドラ
static void handler_InochiNoTama_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
      param->pokeID = pokeID;
      param->damage = BTL_CALC_QuotMaxHP( bpp, 10 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_InochiNoTama );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
static void handler_InochiNoTama_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  メトロノーム
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MetroNome( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_MetroNome},
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MetroNome( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (!BTL_TABLES_IsDelayAttackWaza(BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID)))
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 counter = BPP_GetWazaContCounter( bpp );
    if( counter >= 1 )
    {
      WazaID  currentWaza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      WazaID  prevWaza = BPP_GetPrevWazaID( bpp );

      if( currentWaza == prevWaza )
      {
        fx32 ratio;
        u16  pow, damage;

        pow = 100 + (common_GetItemParam(myHandle, ITEM_PRM_ATTACK) * (counter));
        if( pow > 200 ){ pow = 200; }
        ratio = (FX32_CONST(pow) / 100);

//        TAYA_Printf("カウンタ=%d, pow=%d, ratio=%08x\n", counter, pow, ratio );
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ねばりのかぎづめ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_NebariNoKagidume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM, handler_NebariNoKagidume },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_NebariNoKagidume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) == WAZASICK_BIND)
  ){
    BPP_SICK_CONT cont;
    cont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
    BPP_SICKCONT_SetTurn( &cont, 8 );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}

//------------------------------------------------------------------------------
/**
 *  かいがらのすず
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KaigaraNoSuzu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_L2, handler_KaigaraNoSuzu },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KaigaraNoSuzu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_DELAY_ATTACK_FLAG) == FALSE)
  ){
    u32 damage_sum = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    if( damage_sum )
    {
      damage_sum /= common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
      if( damage_sum == 0 ){
        damage_sum = 1;
      }

      {
        BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
          param->pokeID = pokeID;
          param->recoverHP = damage_sum;
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverLittle );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
          HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
        BTL_SVF_HANDEX_Pop( flowWk, param );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ひかりのねんど
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_HikariNoNendo( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SIDEEFF_PARAM, handler_HikariNoNendo },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_HikariNoNendo( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlSideEffect sideEff = BTL_EVENTVAR_GetValue( BTL_EVAR_SIDE_EFFECT );
    if( (sideEff == BTL_SIDEEFF_HIKARINOKABE) || (sideEff == BTL_SIDEEFF_REFRECTOR) )
    {
      BPP_SICK_CONT cont;
      u8 inc_turns;
      cont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
      inc_turns = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
//      BTL_Printf("サイドエフェクトの継続ターン数を %d ターン増加\n", inc_turns);
      BPP_SICKCONT_IncTurn( &cont, inc_turns );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  パワフルハーブ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowefulHarb( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_TAMETURN_SKIP,   handler_PowefulHarb_CheckTameSkip }, // 溜めスキップチェック
    { BTL_EVENT_TAME_SKIP,             handler_PowefulHarb_FixTameSkip   }, // 溜めスキップ確定
    { BTL_EVENT_USE_ITEM,              handler_PowefulHarb_Use           }, // アイテム使用
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 溜めスキップチェック
static void handler_PowefulHarb_CheckTameSkip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) != WAZANO_HURIIFOORU)
  ){
    if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, TRUE) ){
      work[0] = 1;
    }
  }
}
// 溜めスキップ確定
static void handler_PowefulHarb_FixTameSkip( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( work[0] )
    {
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// アイテム使用
static void handler_PowefulHarb_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_TameSkip );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}

//------------------------------------------------------------------------------
/**
 *  たべのこし
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Tabenokosi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN, handler_Tabenokosi_Reaction },
    { BTL_EVENT_USE_ITEM,        handler_Tabenokosi_Use      },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Tabenokosi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_USE_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
        param->fSkipHPFull = TRUE;
        BTL_EVENT_FACTOR_SetRecallEnable( myHandle );
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
static void handler_Tabenokosi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( !BPP_IsHPFull(bpp) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
        param->pokeID = pokeID;
        param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 16 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverLittle );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  くろいヘドロ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiHedoro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN, handler_KuroiHedoro  },  // ターンチェックハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ターンチェックハンドラ
static void handler_KuroiHedoro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (!BTL_SVFTOOL_CheckShowDown(flowWk))
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    if( PokeTypePair_IsMatch(BPP_GetPokeType(bpp), POKETYPE_DOKU) )
    {
      BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
        param->pokeID = pokeID;
        param->recoverHP = BTL_CALC_QuotMaxHP( bpp, common_GetItemParam(myHandle, ITEM_PRM_ATTACK) );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_RecoverLittle );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
    else
    {
      BTL_HANDEX_PARAM_DAMAGE* param;

      param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
        param->pokeID = pokeID;
        param->damage = BTL_CALC_QuotMaxHP( bpp, 8 );
        param->fExEffect = TRUE;
        param->effectNo = BTLEFF_SOUBI_ITEM;
        param->pos_from = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
        param->pos_to = BTL_POS_NULL;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Damage );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  あかいいと
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_AkaiIto( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_FIXED, handler_AkaiIto  },  // ワザ系状態異常確定
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザ系状態異常確定
static void handler_AkaiIto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) == WAZASICK_MEROMERO)
  ){
    u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( atkPokeID != BTL_POKEID_NULL )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        param->pokeID = atkPokeID;
        param->sickID = WAZASICK_MEROMERO;
        param->sickCont = BPP_SICKCONT_MakePoke( pokeID );
        param->fStdMsgDisable = TRUE;

        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_MeromeroGetItem );
        HANDEX_STR_AddArg( &param->exStr, atkPokeID );
        HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );

      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  くっつきばり
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuttukiBari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_KuttukiBari_DamageReaction  },  // ワザダメージ反応ハンドラ
    { BTL_EVENT_TURNCHECK_END,     handler_KuttukiBari_TurnCheck       },  // ターンチェックハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ワザダメージ反応ハンドラ
static void handler_KuttukiBari_DamageReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == FALSE)
  ){
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag(waza, WAZAFLAG_Touch) )
    {
      u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
      const BTL_POKEPARAM* attacker = BTL_SVFTOOL_GetPokeParam( flowWk, atkPokeID );
      if( BPP_GetItem(attacker) == ITEM_DUMMY_DATA )
      {
        BTL_HANDEX_PARAM_SWAP_ITEM* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SWAP_ITEM, pokeID );
          param->pokeID = atkPokeID;
        BTL_SVF_HANDEX_Pop( flowWk, param );
      }
    }
  }
}
// ターンチェックハンドラ
static void handler_KuttukiBari_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    BTL_HANDEX_PARAM_DAMAGE* param;

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
      param->pokeID = pokeID;
      param->damage = BTL_CALC_QuotMaxHP( bpp, common_GetItemParam(myHandle, ITEM_PRM_ATTACK) );
      param->fExEffect = TRUE;
      param->effectNo = BTLEFF_SOUBI_ITEM;
      param->pos_from = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
      param->pos_to = BTL_POS_NULL;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Damage );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}

//------------------------------------------------------------------------------
/**
 *  パワーリスト
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWrist( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーベルト
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBelt( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーレンズ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerLens( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーバンド
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBand( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーアンクル
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerAnkle( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  パワーウェイト
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWeight( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // すばやさ計算ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 *  パワー○○○（努力値アップアイテム）共通：すばやさ計算ハンドラ
 */
static void handler_PowerXXX_CalcAgility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
  }
}

//------------------------------------------------------------------------------
/**
 *  ひのたまプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_HinotamaPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Mokutan },  // ワザ威力ハンドラ：「もくたん」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  しずくプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SizukuPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SinpiNoSizuku },  // ワザ威力ハンドラ：「しんぴのしずく」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  いかずちプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_IkazutiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Zisyaku },  // ワザ威力ハンドラ：「じしゃく」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  みどりのプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MirodinoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KisekiNoTane },  // ワザ威力ハンドラ：「きせきのたね」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  つららのプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TuraranoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_TokenaiKoori },  // ワザ威力ハンドラ：「とけないこおり」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  こぶしのプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KobusinoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Kuroobi },  // ワザ威力ハンドラ：「くろおび」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  もうどくプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MoudokuPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Dokubari },  // ワザ威力ハンドラ：「どくばり」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  だいちのプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_DaitinoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_YawarakaiSuna },  // ワザ威力ハンドラ：「やわらかいすな」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  あおぞらプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_AozoraPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SurudoiKutibasi },  // ワザ威力ハンドラ：「するどいくちばし」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ふしぎのプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_HusiginoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MagattaSupuun },  // ワザ威力ハンドラ：「まがったスプーン」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  たまむしプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TamamusiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GinNoKona },  // ワザ威力ハンドラ：「ぎんのこな」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  がんせきプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_GansekiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KataiIsi },  // ワザ威力ハンドラ：「かたいいし」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  もののけプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MononokePlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_NoroiNoOfuda },  // ワザ威力ハンドラ：「のろいのおふだ」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  りゅうのプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_RyuunoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_RyuunoKiba },  // ワザ威力ハンドラ：「りゅうのキバ」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  こわもてプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KowamotePlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KuroiMegane },  // ワザ威力ハンドラ：「くろいめがね」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  こうてつプレート
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KoutetsuPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MetalCoat },  // ワザ威力ハンドラ：「メタルコート」と等価
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  おおきなねっこ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_OokinaNekko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_DRAIN,     handler_OokinaNekko },  // ドレイン量計算ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ドレイン量計算ハンドラ
static void handler_OokinaNekko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  けむりだま
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_ADD_ITEM_Kemuridama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SKIP_NIGERU_CALC,       handler_Kemuridama     }, // にげるチェックハンドラ
    { BTL_EVENT_NIGERU_EXMSG,           handler_Kemuridama_Msg }, // 逃げ特殊メッセージハンドラ
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// にげるチェックハンドラ
static void handler_Kemuridama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  HandCommon_NigeruCalcSkip( myHandle, flowWk, pokeID, work );
}
// 逃げ特殊メッセージハンドラ
static void handler_Kemuridama_Msg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_CheckNigeruExMsg(myHandle, flowWk, pokeID) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Kemuridama );
      HANDEX_STR_AddArg( &param->str, pokeID );
      HANDEX_STR_AddArg( &param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      HANDEX_STR_AddSE( &param->str, SEQ_SE_NIGERU );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  おまもりこばん
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_ADD_ITEM_OmamoriKoban( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,       handler_OmamoriKoban },  // メンバー入場ハンドラ
    { BTL_EVENT_ROTATION_IN,     handler_OmamoriKoban_RotationIn },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// メンバー入場ハンドラ
static void handler_OmamoriKoban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_SVFRET_SetMoneyDblUp( flowWk, pokeID );
  }
}
static void handler_OmamoriKoban_RotationIn( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  BTL_SVFRET_SetMoneyDblUp( flowWk, pokeID );
}

//------------------------------------------------------------------------------
/**
 *  はっきんだま
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable*  HAND_ADD_ITEM_HakkinDama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_HakkinDama },
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// ワザ威力計算ハンドラ
static void handler_HakkinDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetMonsNo(bpp) == MONSNO_GIRATHINA )
    {
      PokeType wazaType = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      if( (wazaType == POKETYPE_DRAGON) || (wazaType == POKETYPE_GHOST) )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.2) );
      }
    }
  }
}




//------------------------------------------------------------------------------
/**
 *  つめたいいわ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TumetaiIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_TumetaiIwa  },  // 天候変化ワザのターン数チェック
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 天候変化ワザのターン数チェック
static void handler_TumetaiIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_SNOW );
}
//------------------------------------------------------------------------------
/**
 *  さらさらいわ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SarasaraIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_SarasaraIwa  },  // 天候変化ワザのターン数チェック
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 天候変化ワザのターン数チェック
static void handler_SarasaraIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_SAND );
}
//------------------------------------------------------------------------------
/**
 *  あついいわ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_AtuiIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_AtuiIwa  },  // 天候変化ワザのターン数チェック
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 天候変化ワザのターン数チェック
static void handler_AtuiIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_SHINE );
}
//------------------------------------------------------------------------------
/**
 *  しめったいわ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SimettaIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_SimettaIwa  },  // 天候変化ワザのターン数チェック
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 天候変化ワザのターン数チェック
static void handler_SimettaIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_RAIN );
}

/*
 *  天候変化ワザターン数増加アイテムの共通ルーチン
 */
static void common_WazaWeatherTurnUp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWeather weather )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WEATHER) == weather)
  ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_TURN_COUNT, common_GetItemParam(myHandle, ITEM_PRM_ATTACK) );
  }
}

//------------------------------------------------------------------------------
/**
 *  でんきだま
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_DenkiDama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ATTACKER_POWER, handler_DenkiDama   },
    { BTL_EVENT_USE_ITEM_TMP,   handler_DenkiDama_UseTmp},
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
    u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, atkPokeID );
      param->pokeID = pokeID;
      param->sickID = WAZASICK_MAHI;
      param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( WAZASICK_MAHI );
    BTL_SVF_HANDEX_Pop( flowWk, param );
//    param->fAlmost = TRUE;
  }
}
//------------------------------------------------------------------------------
/**
 *  どくどくだま
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_DokudokuDama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,    handler_DokudokuDama        },
    { BTL_EVENT_USE_ITEM_TMP,     handler_DokudokuDama_UseTmp },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_DokudokuDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_DOKU;
      param->sickCont = BPP_SICKCONT_MakeMoudokuCont();
      param->fAlmost = FALSE;
      param->pokeID = pokeID;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_MoudokuGetSP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
// なげつけ効果
static void handler_DokudokuDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, atkPokeID );
      param->pokeID = pokeID;
      param->sickID = WAZASICK_DOKU;
      param->sickCont = BPP_SICKCONT_MakeMoudokuCont();
//    param->fAlmost = TRUE;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  かえんだま
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KaenDama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,  handler_KaenDama        },
    { BTL_EVENT_USE_ITEM_TMP,   handler_KaenDama_UseTmp },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KaenDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_YAKEDO;
      param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( WAZASICK_YAKEDO );
      param->fAlmost = FALSE;
      param->pokeID = pokeID;
      param->exStr.args[1] = BTL_EVENT_FACTOR_GetSubID( myHandle );;

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_YakedoGetSP );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
// なげつけ効果
static void handler_KaenDama_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, atkPokeID );
      param->pokeID = pokeID;
      param->sickID = WAZASICK_YAKEDO;
      param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( param->sickID );
//    param->fAlmost = TRUE;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}


//------------------------------------------------------------------------------
/**
 *  ぎんのこな
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_GinNoKona( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GinNoKona },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_YawarakaiSuna( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_YawarakaiSuna },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_KataiIsi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KataiIsi },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_KisekiNoTane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KisekiNoTane },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiMegane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KuroiMegane },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_Kuroobi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Kuroobi },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_Zisyaku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Zisyaku },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_MetalCoat( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MetalCoat },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_SinpiNoSizuku( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SinpiNoSizuku },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_SurudoiKutibasi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SurudoiKutibasi },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_Dokubari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Dokubari },
    { BTL_EVENT_USE_ITEM_TMP,   handler_Dokubari_UseTmp },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
    u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, atkPokeID );
      param->pokeID = pokeID;
      param->sickID = WAZASICK_DOKU;
      param->sickCont = BTL_CALC_MakeDefaultPokeSickCont( WAZASICK_DOKU );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  とけないこおり
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TokenaiKoori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_TokenaiKoori },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_NoroiNoOfuda( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_NoroiNoOfuda },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_MagattaSupuun( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MagattaSupuun },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_Mokutan( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Mokutan },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_RyuunoKiba( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_RyuunoKiba },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_SirukuNoSukaafu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SirukuNoSukaafu },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_AyasiiOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_AyasiiOkou },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_GansekiOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GansekiOkou },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_SazanamiNoOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SazanamiNoOkou },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_UsioNoOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_UsioNoOkou },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_OhanaNoOkou( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_OhanaNoOkou },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
      BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  しらたま
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Siratama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Siratama },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Siratama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetMonsNo(bpp) == MONSNO_PARUKIA )
    {
      PokeType  waza_type =  BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      if( (waza_type == POKETYPE_DRAGON) || (waza_type == POKETYPE_MIZU) )
      {
        fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  こんごうだま
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Kongoudama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Kongoudama },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Kongoudama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetMonsNo(bpp) == MONSNO_DHIARUGA )
    {
      PokeType  waza_type =  BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
      if( (waza_type == POKETYPE_DRAGON) || (waza_type == POKETYPE_HAGANE) )
      {
        fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
        BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, ratio );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  くろいてっきゅう
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiTekkyuu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,     handler_KuroiTekkyuu_Agility  },  // すばやさ計算ハンドラ
    { BTL_EVENT_CHECK_FLYING,     handler_KuroiTekkyuu_CheckFly },  // ふゆうチェックハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// すばやさ計算ハンドラ
static void handler_KuroiTekkyuu_Agility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
  }
}
// ふゆうチェックハンドラ
static void handler_KuroiTekkyuu_CheckFly( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}
//------------------------------------------------------------------------------
/**
 *  かるいし
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Karuisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEIGHT_RATIO,     handler_Karuisi },   // 体重倍率チェックハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Karuisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
  }
}

//------------------------------------------------------------------------------
/**
 *  しんかのきせき
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SinkanoKiseki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD,     handler_SinkanoKiseki },   // 防御側ガード力チェックハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// 防御側ガード力チェックハンドラ
static void handler_SinkanoKiseki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が防御側で進化前だったら防御・特防1.5倍
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_SVFTOOL_CheckSinkaMae(flowWk, pokeID) )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ゴツゴツメット
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_GotugotuMet( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_GotugotuMet },   // ダメージ反応ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
static void handler_GotugotuMet( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == FALSE)
  ){
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetFlag(waza, WAZAFLAG_Touch) )
    {
      BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
        param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
        {
          const BTL_POKEPARAM* target = BTL_SVFTOOL_GetPokeParam( flowWk, param->pokeID );
          u8 itemAtk = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
          param->damage = BTL_CALC_QuotMaxHP( target, itemAtk );

          HANDEX_STR_Setup( &(param->exStr), BTL_STRTYPE_SET, BTL_STRID_SET_GotuGotuMet );
          HANDEX_STR_AddArg( &(param->exStr), param->pokeID );
        }
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ふうせん
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Huusen( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_Huusen_MemberIn       },   // メンバー入場ハンドラ
    { BTL_EVENT_CHECK_FLYING,        handler_Huusen_CheckFlying    },   // 飛行フラグチェックハンドラ
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_Huusen_DamageReaction },   // ダメージ反応ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
      BTL_SVF_HANDEX_Pop( flowWk, param );
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
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
// アイテム書き換え完了ハンドラ
static void handler_Huusen_ItemSetFixed( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // 「ふうせん」を持たされたら飛行禁止状態を解除
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->sickCode = WAZASICK_FLYING_CANCEL;
      param->fStdMsgDisable = TRUE;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}

//------------------------------------------------------------------------------
/**
 *  レッドカード
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_RedCard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,     handler_RedCard },   /// ダメージ処理終了
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
/// ダメージ処理終了ハンドラ
static void handler_RedCard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_CheckTargetPokeID(pokeID) )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( (BTL_SVFTOOL_GetExistFrontPokePos(flowWk, targetPokeID) != BTL_POS_NULL)
    &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_DELAY_ATTACK_FLAG) == FALSE )
    &&  (BTL_SVFTOOL_IsExistBenchPoke(flowWk, targetPokeID))
    &&  (!BTL_SVFTOOL_CheckShowDown(flowWk))
    ){
      BTL_HANDEX_PARAM_CONSUME_ITEM* consume_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CONSUME_ITEM, pokeID );
        HANDEX_STR_Setup( &consume_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_RedCard );
        HANDEX_STR_AddArg( &consume_param->exStr, pokeID );
        HANDEX_STR_AddArg( &consume_param->exStr, targetPokeID );
      BTL_SVF_HANDEX_Pop( flowWk, consume_param );

      {
        BTL_HANDEX_PARAM_PUSHOUT* push_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_PUSHOUT, pokeID );
          push_param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
          push_param->fIgnoreLevel = TRUE;
          push_param->effectNo = BTLEFF_DRAGONTAIL_RETURN;
        BTL_SVF_HANDEX_Pop( flowWk, push_param );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  ねらいのまと
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_NerainoMato( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,     handler_NerainoMato },   // タイプ相性による無効化チェック
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// タイプ相性による無効化チェック
static void handler_NerainoMato( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // シミュレーション時にはなにもしない
  if( !BTL_SVFTOOL_IsSimulationMode(flowWk) )
  {
    // 自分が防御側の時、相性により無効化されていれば有効に書き換え
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, TRUE );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  しめつけバンド
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SimetukeBand( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM,     handler_SimetukeBand },   // 状態異常パラメータチェックハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
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
static const BtlEventHandlerTable* HAND_ADD_ITEM_Kyuukon( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Kyuukon_DmgReaction },   // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,              handler_Kyuukon_Use   },         // どうぐ使用ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
static void handler_Kyuukon_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が水タイプのワザダメージを受けた時、特攻がまだ上がるなら反応する
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == FALSE)
  ){
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_MIZU )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( BPP_IsRankEffectValid(bpp, BPP_SP_ATTACK_RANK, 1) )
      {
//        BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
        ItemPushRun( myHandle, flowWk, pokeID );
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
      param->rankType = BPP_SP_ATTACK_RANK;
      param->rankVolume = 1;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  じゅうでんち
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Juudenti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Juudenti_DmgReaction },   // ダメージ反応ハンドラ
    { BTL_EVENT_USE_ITEM,              handler_Juudenti_Use   },         // どうぐ使用ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ反応ハンドラ
static void handler_Juudenti_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分が電気タイプのワザダメージを受けた時、攻撃がまだ上がるなら反応する
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_MIGAWARI_FLAG) == FALSE)
  ){
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_DENKI )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( BPP_IsRankEffectValid(bpp, BPP_ATTACK_RANK, 1) )
      {
//        BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
        ItemPushRun( myHandle, flowWk, pokeID );
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
      param->rankType = BPP_ATTACK_RANK;
      param->rankVolume = 1;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  だっしゅつボタン
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_DassyutuButton( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,     handler_DassyutuButton_Reaction },  // ダメージ処理終了ハンドラ
    { BTL_EVENT_USE_ITEM,                handler_DassyutuButton_Use      },  // どうぐ使用ハンドラ
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ダメージ処理終了ハンドラ
static void handler_DassyutuButton_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // 自分がダメージを受けた側

  if( (HandCommon_CheckTargetPokeID(pokeID))
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_DELAY_ATTACK_FLAG) == FALSE )
  &&  (BTL_SVFTOOL_IsFreeFallPoke(flowWk, pokeID) == FALSE)
  &&  (!BTL_SVFTOOL_CheckShowDown(flowWk))
  ){
    // 控えに交替可能メンバーがいるなら、どうぐ使用ハンドラ呼び出し
    //（割り込みアクション時＆すでにメンバー入れ替え行動を取ろうとしているポケモンがいる場合を除く）
    if( (!BTL_SVFTOOL_IsMemberOutIntr(flowWk))
    &&  (BTL_SVFTOOL_IsExistBenchPoke(flowWk, pokeID))
    &&  (BTL_SVFTOOL_ReserveMemberChangeAction(flowWk))
    ){
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// どうぐ使用ハンドラ
static void handler_DassyutuButton_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_SVFTOOL_IsExistBenchPoke(flowWk, pokeID) )
    {
      #if 0
      BTL_HANDEX_PARAM_PUSHOUT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_PUSHOUT, pokeID );
        param->pokeID = pokeID;
        param->fForceChange = TRUE;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_DassyutuPod );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      BTL_SVF_HANDEX_Pop( flowWk, param );
      #else
      {
        // おいうちを受けるタイミングを作るためにメッセージだけをまずは出す
        BTL_HANDEX_PARAM_MESSAGE* msgParam = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
          HANDEX_STR_Setup( &msgParam->str, BTL_STRTYPE_SET, BTL_STRID_SET_DassyutuPod );
          HANDEX_STR_AddArg( &msgParam->str, pokeID );
        BTL_SVF_HANDEX_Pop( flowWk, msgParam );
      }
      {
        BTL_HANDEX_PARAM_CHANGE_MEMBER* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_MEMBER, pokeID );
          param->pokeID = pokeID;
//        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_DassyutuPod );
//        HANDEX_STR_AddArg( &param->exStr, pokeID );
        BTL_SVF_HANDEX_Pop( flowWk, param );
      }
      #endif
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  ○○○ジュエル
 */
//------------------------------------------------------------------------------

/**
 *  ほのおのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_HonooNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_HonooNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_HonooNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_HonooNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_HONOO );
}
static void handler_HonooNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_HONOO );
}
/**
 *  みずのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_MizuNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_MizuNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_MizuNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MizuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_MIZU );
}
static void handler_MizuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_MIZU );
}
/**
 *  でんきのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_DenkiNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_DenkiNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_DenkiNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_DenkiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_DENKI );
}
static void handler_DenkiNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_DENKI );
}
/**
 *  くさのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_KusaNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_KusaNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_KusaNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KusaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_KUSA );
}
static void handler_KusaNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_KUSA );
}
/**
 *  こおりのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_KooriNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_KooriNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_KooriNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KooriNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_KOORI );
}
static void handler_KooriNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_KOORI );
}
/**
 *  かくとうジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_KakutouJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_KakutouJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_KakutouJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KakutouJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_KAKUTOU );
}
static void handler_KakutouJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_KAKUTOU );
}
/**
 *  どくのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_DokuNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_DokuNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_DokuNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_DokuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_DOKU );
}
static void handler_DokuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_DOKU );
}
/**
 *  じめんのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_JimenNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_JimenNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_JimenNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_JimenNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_JIMEN );
}
static void handler_JimenNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_JIMEN );
}
/**
 *  ひこうのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_HikouNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_HikouNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_HikouNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq     },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_HikouNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU );
}
static void handler_HikouNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_HIKOU );
}
/**
 *  エスパージュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_EsperJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_EsperJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_EsperJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_EsperJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_ESPER );
}
static void handler_EsperJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_ESPER );
}
/**
 *  むしのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_MusiNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_MusiNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_MusiNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MusiNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_MUSHI );
}
static void handler_MusiNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_MUSHI );
}
/**
 *  いわのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_IwaNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_IwaNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_IwaNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_IwaNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_IWA );
}
static void handler_IwaNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_IWA );
}
/**
 *  ゴーストジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_GhostJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_GhostJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_GhostJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_GhostJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_GHOST );
}
static void handler_GhostJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_GHOST );
}
/**
 *  ドラゴンジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_DragonJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_DragonNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_DragonNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq      },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_DragonNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_DRAGON );
}
static void handler_DragonNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_DRAGON );
}
/**
 *  あくのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_AkuNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_AkuNoJuel_Decide  },
    { BTL_EVENT_WAZA_POWER,             handler_AkuNoJuel_Pow     },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_AkuNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_AKU );
}
static void handler_AkuNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_AKU );
}
/**
 *  はがねのジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_HaganeNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_HaganeNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_HaganeNoJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq      },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_HaganeNoJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_HAGANE );
}
static void handler_HaganeNoJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_HAGANE );
}
/**
 *  ノーマルジュエル
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_NormalJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_NormalJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_NormalJuel_Pow    },
    { BTL_EVENT_DAMAGEPROC_END,         handler_Juel_EndDmgSeq    },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_NormalJuel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Decide( myHandle, flowWk, pokeID, work, POKETYPE_NORMAL );
}
static void handler_NormalJuel_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_Juel_Power( myHandle, flowWk, pokeID, work, POKETYPE_NORMAL );
}


enum {
  JUEL_SEQ_NULL = 0,
  JUEL_SEQ_STANDBY,
  JUEL_SEQ_END,
};

// ジュエル：ダメージ処理プロセス開始ハンドラ共通
static void common_Juel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  ){
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( (WAZADATA_IsDamage(waza))
    &&  (!BTL_TABLES_IsCombiWaza(waza) )
    &&  (!BTL_EVENT_FACTOR_IsIsolateMode(myHandle))
    ){
      if( !BTL_SVFTOOL_IsSimulationMode(flowWk) )
      {
        BTL_HANDEX_PARAM_CONSUME_ITEM* param;

        BTL_EVENT_FACTOR_ConvertForIsolate( myHandle );

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CONSUME_ITEM, pokeID );
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Juel );
          HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
          HANDEX_STR_AddArg( &param->exStr, BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
        BTL_SVF_HANDEX_Pop( flowWk, param );

        work[0] = JUEL_SEQ_STANDBY;
      }
    }
  }
}
// ジュエル：威力計算ハンドラ共通
static void common_Juel_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type )
{
  // 自分が攻撃側＆タイプがマッチしていたら威力を増加
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  &&  ((work[0] == JUEL_SEQ_STANDBY) || (BTL_SVFTOOL_IsSimulationMode(flowWk)))
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5f) );
  }
}
// ジュエル：ダメージ処理プロセス終了ハンドラ共通
static void handler_Juel_EndDmgSeq( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (work[0] == JUEL_SEQ_STANDBY)
  ){
    work[0] = JUEL_SEQ_END;
  }
}

