//=============================================================================================
/**
 * @file  hand_item.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[�i�A�C�e���j�ǉ�����
 * @author  taya
 *
 * @date  2009.04.21  �쐬
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
#include "hand_item.h"


enum {
  WORKIDX_TMP_FLAG = EVENT_HANDLER_WORK_ELEMS-1,  ///< �ꎞ���p�A�C�e��
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
static void handler_OrenNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_KinomiJuice( u32* numElems );
static const BtlEventHandlerTable* HAND_ADD_ITEM_ObonNomi( u32* numElems );
static void handler_ObonNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_ObonNomi_UseTmp( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_FiraNomi( u32* numElems );
static void handler_FiraNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_UiNomi( u32* numElems );
static void handler_UiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MagoNomi( u32* numElems );
static void handler_MagoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_BanziNomi( u32* numElems );
static void handler_BanjiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_IaNomi( u32* numElems );
static void handler_IaNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_common_KaifukuKonran_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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
static void common_DamageReactCore( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n );
static BOOL common_DamageReactCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n );
static const BtlEventHandlerTable* HAND_ADD_ITEM_IbanNomi( u32* numElems );
static void handler_IbanNomi_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_IbanNomi_SpPriorityWorked( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_IbanNomi_ActStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_IbanNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_MikuruNomi( u32* numElems );
static void handler_MikuruNomi_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MikuruNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_JapoNomi( u32* numElems );
static void handler_JapoNomi_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_JapoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_RenbuNomi( u32* numElems );
static void handler_RenbuNomi_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static const BtlEventHandlerTable* HAND_ADD_ITEM_SiroiHerb( u32* numElems );
static void handler_SiroiHerb_ActCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
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



// �C�x���g�n���h���o�^�֐��̌^��`
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
  { ITEM_MANPUKUOKOU,       HAND_ADD_ITEM_KoukouNoSippo     },  // �܂�Ղ�������=���������̂����ۓ���
  { ITEM_OUZYANOSIRUSI,     HAND_ADD_ITEM_OujaNoSirusi      },
  { ITEM_SURUDOITUME,       HAND_ADD_ITEM_PintLens          },  // ����ǂ��c�� = �s���g�����Y�Ɠ���
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


  { ITEM_KARUISI,           HAND_ADD_ITEM_Karuisi         },  // ���邢��
  { ITEM_SINKANOKISEKI,     HAND_ADD_ITEM_SinkanoKiseki   },  // ���񂩂̂�����
  { ITEM_GOTUGOTUMETTO,     HAND_ADD_ITEM_GotugotuMet     },  // �S�c�S�c���b�g
  { ITEM_HUUSEN,            HAND_ADD_ITEM_Huusen          },  // �ӂ�����
  { ITEM_REDDOKAADO,        HAND_ADD_ITEM_RedCard         },  // ���b�h�J�[�h
  { ITEM_NERAINOMATO,       HAND_ADD_ITEM_NerainoMato     },  // �˂炢�̂܂�
  { ITEM_SIMETUKEBANDO,     HAND_ADD_ITEM_SimetukeBand    },  // ���߂��o���h
  { ITEM_KYUUKON,           HAND_ADD_ITEM_Kyuukon         },  // ���イ����
  { ITEM_ZYUUDENTI,         HAND_ADD_ITEM_Juudenti        },  // ���イ�ł�
  { ITEM_DASSYUTUBOTAN,     HAND_ADD_ITEM_DassyutuButton  },  // ��������{�^��
  { ITEM_HONOONOZYUERU,     HAND_ADD_ITEM_HonooNoJuel     },  // �ق̂��̃W���G��
  { ITEM_MIZUNOZYUERU,      HAND_ADD_ITEM_MizuNoJuel      },  // �݂��̃W���G��
  { ITEM_DENKINOZYUERU,     HAND_ADD_ITEM_DenkiNoJuel     },  // �ł񂫂̃W���G��
  { ITEM_KUSANOZYUERU,      HAND_ADD_ITEM_KusaNoJuel      },  // �����̃W���G��
  { ITEM_KOORINOZYUERU,     HAND_ADD_ITEM_KooriNoJuel     },  // ������̃W���G��
  { ITEM_KAKUTOUZYUERU,     HAND_ADD_ITEM_KakutouJuel     },  // �����Ƃ��W���G��
  { ITEM_DOKUNOZYUERU,      HAND_ADD_ITEM_DokuNoJuel      },  // �ǂ��̃W���G��
  { ITEM_ZIMENNOZYUERU,     HAND_ADD_ITEM_JimenNoJuel     },  // ���߂�̃W���G��
  { ITEM_HIKOUNOZYUERU,     HAND_ADD_ITEM_HikouNoJuel     },  // �Ђ����̃W���G��
  { ITEM_ESUPAAZYUERU,      HAND_ADD_ITEM_EsperJuel       },  // �G�X�p�[�W���G��
  { ITEM_MUSINOZYUERU,      HAND_ADD_ITEM_MusiNoJuel      },  // �ނ��̃W���G��
  { ITEM_IWANOZYUERU,       HAND_ADD_ITEM_IwaNoJuel       },  // ����̃W���G��
  { ITEM_GOOSUTOZYUERU,     HAND_ADD_ITEM_GhostJuel       },  // �S�[�X�g�W���G��
  { ITEM_DORAGONZYUERU,     HAND_ADD_ITEM_DragonJuel      },  // �h���S���W���G��
  { ITEM_AKUNOZYUERU,       HAND_ADD_ITEM_AkuNoJuel       },  // �����̃W���G��
  { ITEM_HAGANENOZYUERU,    HAND_ADD_ITEM_HaganeNoJuel    },  // �͂��˂̃W���G��
  { ITEM_NOOMARUZYUERU,     HAND_ADD_ITEM_NormalJuel      },  // �m�[�}���W���G��


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
 * ����|�P�����́u�A�C�e���v�n���h�����V�X�e������S�č폜
 *
 * @param   pp
 *
 */
//=============================================================================================
void BTL_HANDLER_ITEM_Remove( const BTL_POKEPARAM* bpp )
{
  BTL_EVENT_FACTOR* factor;
  u8 pokeID = BPP_GetID( bpp );

  while( (factor = BTL_EVENT_SeekFactor(BTL_EVENT_FACTOR_ITEM, pokeID)) != NULL )
  {
    BTL_EVENT_FACTOR_Remove( factor );
  }
}
//=============================================================================================
/**
 * �|�P�����̑������Ă���A�C�e���̃n���h����ǉ�
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
 * �����A�C�e���Ƃ͕ʂɈꎞ�I�ȃn���h���ǉ�
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
 * �ꎞ�I�Ȓǉ��n���h�����폜����
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
 * �����_���������鑕�����ʂ̋��ʃ`�F�b�N���[�`��
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
 * �A�C�e���p�����[�^�擾
 *
 * @param   myHandle
 * @param   paramID   �p�����[�^ID�iitem/item.h)
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
 * �A�C�e���p�����[�^�́u�З͒l�v���A�З́E�������Ȃǂ̔{���ɕϊ�����ۂ̋��ʃ��[�`��
 *
 * @param   itemID    �A�C�e��ID
 *
 * @retval  fx32      �u�З͒l�v��{���l�ɕϊ��������́B
 *                    ��j�З͒l20 �Ȃ� FX32_CONST(1.2�j�����̒l���Ԃ�
 */
//----------------------------------------------------------------------------------
static fx32 Item_AttackValue_to_Ratio( BTL_EVENT_FACTOR* myHandle )
{
  int pow = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
  return FX32_CONST(100+pow) / 100;
}

//----------------------------------------------------------------------------------
/**
 * �������g�̍ČĂяo������A�A�C�e���g�p�C�x���g�Ăяo��
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
 *  �N���{�̂�
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
    { BTL_EVENT_MEMBER_IN,           handler_KuraboNomi     }, // �����o�[����`�F�b�N�n���h��
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KuraboNomi     }, // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_KuraboNomi_Use }, // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_KuraboNomi_Use }, // �����ꎞ�g�p�i���΂ނȂǁj
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �J�S�̂�
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
    { BTL_EVENT_MEMBER_IN,           handler_KagoNomi     }, // �����o�[����`�F�b�N�n���h��
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KagoNomi     }, // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_KagoNomi_Use }, // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_KagoNomi_Use }, // �����ꎞ�g�p�i���΂ނȂǁj
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �`�[�S�̂�
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
    { BTL_EVENT_MEMBER_IN,           handler_ChigoNomi     },  // �����o�[����`�F�b�N�n���h��
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_ChigoNomi     },  // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_ChigoNomi_Use },  // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_ChigoNomi_Use },  // �����ꎞ�g�p�i���΂ނȂǁj
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �i�i�V�̂�
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
    { BTL_EVENT_MEMBER_IN,           handler_NanasiNomi     }, // �����o�[����`�F�b�N�n���h��
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_NanasiNomi     }, // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_NanasiNomi_Use }, // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_NanasiNomi_Use }, // �����ꎞ�g�p�i���΂ނȂǁj
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �L�[�̂�
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
    { BTL_EVENT_MEMBER_IN,           handler_KiiNomi     }, // �����o�[����`�F�b�N�n���h��
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KiiNomi     }, // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_KiiNomi_Use }, // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_KiiNomi_Use }, // �����ꎞ�g�p�i���΂ނȂǁj
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �������̂�
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
    { BTL_EVENT_MEMBER_IN,           handler_MomonNomi     },  // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_MomonNomi     },  // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_MomonNomi_Use },  // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_MomonNomi_Use },  // �����ꎞ�g�p�i���΂ނȂǁj
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �����̂�
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
    { BTL_EVENT_MEMBER_IN,           handler_RamNomi     },  // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_RamNomi     },  // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_RamNomi_Use },  // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_RamNomi_Use },  // �����ꎞ�g�p�i���΂ނȂǁj
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//--------------------------------------------------------------------------
/**
 * ��Ԉُ�ɔ�������A�C�e���Q�̋��ʏ���
 *
 * @param   flowWk
 * @param   pokeID
 * @param   sickID    ���������Ԉُ�
 *
 */
//--------------------------------------------------------------------------
static void common_sickReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, BtlWazaSickEx sickCode )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
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
 *  �q�����̂�
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
// ���U�Ō�
static void handler_HimeriNomi_wazaEnd( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    if( common_Himeri_LastWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
    {
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// �A�C�e�������ėp
static void handler_HimeriNomi_reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_ITEM_REACTION) != BTL_ITEMREACTION_HP)
  ){
    if( common_Himeri_EnableWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
    {
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// �A�C�e������
static void handler_HimeriNomi_get( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  ){
    TAYA_Printf("�q�����Ńo�g���C�� pke=%d\n", pokeID);
    if( common_Himeri_EnableWazaIdx(flowWk, pokeID) != PTL_WAZA_MAX )
    {
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// PP�����ꂽ
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
 *  �q�����̂ݗp�F���O�Ɏg�������U��PP��0�̏ꍇ�A���̃��U��Index��Ԃ��^����ȊO PTL_WAZA_MAX ��Ԃ�
 */
static u8 common_Himeri_LastWazaIdx( BTL_SVFLOW_WORK* flowWk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      // ���O�Ɏg�������U��PP��0�Ȃ�ŗD��
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
 *  �q�����̂ݗp�F���O�Ɏg�������U��PP��0�̏ꍇ�A���̃��U��Index��Ԃ��^����ȊO PTL_WAZA_MAX ��Ԃ�
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
 *  �����Ȏg����������q�����݂̂̋��ʏ���
 */
static BOOL handler_HimeriNomi_common( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, BOOL fUseTmp )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  u8 wazaIdx = PTL_WAZA_MAX;

  do{
      // ���O�Ɏg�������U��PP��0�Ȃ�ŗD��
      wazaIdx = common_Himeri_LastWazaIdx( flowWk, pokeID );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        break;
      }

      // ����ȊO��PP=0�̃��U������Ύg���i0�Ԃ��珇�T�[�`�j
      wazaIdx = common_Himeri_EnableWazaIdx( flowWk, pokeID );
      if( wazaIdx != PTL_WAZA_MAX )
      {
        break;
      }

      // ���΂ށE�Ȃ�������ʂ̎���PP���P�ł������Ă�����g���i0�Ԃ��珇�T�[�`�j
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
      // �ꎞ���p�̏ꍇ�̓R�R�ŃG�t�F�N�g�o���Ȃ��Ƃ����Ȃ��c
      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM_EFFECT, pokeID );
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
 *  �I�����̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_OrenNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use      },
    { BTL_EVENT_USE_ITEM_TMP,           handler_OrenNomi_Use      },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_OrenNomi_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_DamageReactCore( myHandle, flowWk, pokeID, 2 );
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
 *  ���̂݃W���[�X
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KinomiJuice( u32* numElems )
{
  // ���������ʈȊO�́g�I�����̂݁h�Ɠ����ŗǂ�
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �I�{���̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_ObonNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction },
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
 *  �t�B���̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_FiraNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_FiraNomi_Use },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_FiraNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_KARAI );
}
//------------------------------------------------------------------------------
/**
 *  �E�C�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_UiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_UiNomi_Use },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_UiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_SIBUI );
}
//------------------------------------------------------------------------------
/**
 *  �}�S�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MagoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,      handler_common_KaifukuKonran_Reaction },
    { BTL_EVENT_USE_ITEM,                 handler_MagoNomi_Use },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_MagoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_AMAI );
}
//------------------------------------------------------------------------------
/**
 *  �o���W�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_BanziNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_BanjiNomi_Use },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_BanjiNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_NIGAI );
}
//------------------------------------------------------------------------------
/**
 *  �C�A�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_IaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_common_KaifukuKonran_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_IaNomi_Use },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_IaNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_KaifukuKonran( myHandle, flowWk, pokeID, work, PTL_TASTE_SUPPAI );
}

/**
 *  �񕜂�����؂̎��̋��ʃ��A�N�V�����n���h��
 */
static void handler_common_KaifukuKonran_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[0] == 0 )
  {
    common_DamageReactCore( myHandle, flowWk, pokeID, 2 );
  }
}
/**
 *  �񕜂�����؂̎��̋��ʂǂ����g�p�n���h��
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
 *  �`�C���̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TiiraNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_TiiraNomi },
    { BTL_EVENT_USE_ITEM_TMP,           handler_TiiraNomi },
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
 *  �����K�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_RyugaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_RyugaNomi },
    { BTL_EVENT_USE_ITEM_TMP,           handler_RyugaNomi },
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
 *  �J�����̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KamuraNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_KamuraNomi },
    { BTL_EVENT_USE_ITEM_TMP,           handler_KamuraNomi },
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
 *  ���^�s�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_YatapiNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,                    handler_YatapiNomi },
    { BTL_EVENT_USE_ITEM_TMP,                handler_YatapiNomi },
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
 *  �Y�A�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_ZuaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,                    handler_ZuaNomi },
    { BTL_EVENT_USE_ITEM_TMP,                handler_ZuaNomi },
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
 *  �T���̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,                    handler_SanNomi },
    { BTL_EVENT_USE_ITEM_TMP,                handler_SanNomi },
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
 *  �X�^�[�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SutaaNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,                    handler_SutaaNomi },
    { BTL_EVENT_USE_ITEM_TMP,                handler_SutaaNomi },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SutaaNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const u8 rankType[] = {
    WAZA_RANKEFF_ATTACK, WAZA_RANKEFF_DEFENCE,
    WAZA_RANKEFF_SP_ATTACK, WAZA_RANKEFF_SP_DEFENCE,
    WAZA_RANKEFF_AGILITY,
  };
  u8 idx = BTL_CALC_GetRand( NELEMS(rankType) );

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
 * �s���`�ɂ���Ĕ\�̓����N�A�b�v����ǂ����̋��ʏ���
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
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  �i�]�̂�
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
      ItemPushRun( myHandle, flowWk, pokeID );
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
 *  �I�b�J�̂�
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
 *  �C�g�P�̂�
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
 *  �\�N�m�̂�
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
 *  �����h�̂�
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
 *  ���`�F�̂�
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
 *  ���v�̂�
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
 *  �r�A�[�̂�
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
 *  �V���J�̂�
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
 *  �o�R�E�̂�
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
 *  �E�^���̂�
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
 *  �^���K�̂�
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
 *  �����M�̂�
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
 *  �J�V�u�̂�
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
 *  �n�o���̂�
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
 *  �i���̂�
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
 *  �����o�̂�
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
 *  �z�Y�̂�
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
 *  �З͔�����������TRUE��Ԃ�
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
 * �s���`���iHP <= 1/�A�C�e���З͒l�j�ɔ�������؂̎��̋��ʏ���
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
    common_DamageReactCore( myHandle, flowWk, pokeID, item_pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * HP 1/n �ȉ��Ŕ�������A�C�e���̋��ʏ��� - �����𖞂����Ă�����A�C�e���g�p�n���h���Ăяo��
 *
 * @param   flowWk
 * @param   pokeID
 * @param   n
 */
//----------------------------------------------------------------------------------
static void common_DamageReactCore( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_DamageReactCheck( myHandle, flowWk, pokeID, n) )
    {
      BTL_N_PrintfSimple( DBGSTR_Item_PinchReactOn );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
//--------------------------------------------------------------
/**
 * HP 1/n �ȉ��Ŕ�������A�C�e���̋��ʏ��� - �����𖞂����Ă邩�`�F�b�N
 */
//--------------------------------------------------------------
static BOOL common_DamageReactCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, u32 n )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 maxHP;

    if( BPP_GetValue(bpp, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_KUISINBOU ){
      n /= 2;
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
  }
  return FALSE;
}
//------------------------------------------------------------------------------
/**
 *  �C�o���̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_IbanNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,    handler_IbanNomi_SpPriorityCheck  },  // ����s���v���C�I���e�B�`�F�b�N
//    { BTL_EVENT_WORKED_SP_PRIORITY,   handler_IbanNomi_SpPriorityWorked },  // ����s���v���C�I���e�B���ʔ���
//    { BTL_EVENT_ACTPROC_START,        handler_IbanNomi_ActStart         },  // �A�N�V�����J�n�n���h��
    { BTL_EVENT_USE_ITEM,             handler_IbanNomi_Use              },  // �A�C�e���g�p�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ����s���v���C�I���e�B�`�F�b�N
static void handler_IbanNomi_SpPriorityCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( common_DamageReactCheck(myHandle, flowWk, pokeID, 4) )
    {
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_SP_PRIORITY, BTL_SPPRI_HIGH) )
      {
        ItemPushRun( myHandle, flowWk, pokeID );
      }
    }
  }
}
// ����s���v���C�I���e�B���ʔ���
static void handler_IbanNomi_SpPriorityWorked( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    work[0] = 1;
  }
}
// �A�N�V�����J�n�n���h��
static void handler_IbanNomi_ActStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] == 1)
  ){
//    BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
    ItemPushRun( myHandle, flowWk, pokeID );
  }
}

// �A�C�e���g�p�n���h��
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
 *  �~�N���̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MikuruNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_END,     handler_MikuruNomi_TurnCheck  }, // �^�[���`�F�b�N
    { BTL_EVENT_USE_ITEM,          handler_MikuruNomi_Use        }, // �A�C�e���g�p�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �^�[���`�F�b�N
static void handler_MikuruNomi_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
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
// �A�C�e���g�p�n���h��
static void handler_MikuruNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->pokeID = pokeID;
      param->sickID = WAZASICK_HITRATIO_UP;
      param->sickCont = BPP_SICKCONT_MakeTurnParam( 1, 120 );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_HitRatioUpOnce );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}

//------------------------------------------------------------------------------
/**
 *  �W���|�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_JapoNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_JapoNomi_Damage     }, // �_���[�W�����n���h��
    { BTL_EVENT_USE_ITEM,          handler_JapoNomi_Use        }, // �A�C�e���g�p�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_JapoNomi_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_PHYSIC )
    {
//      BTL_EVENT_FACTOR_ConvertForIsolate( myHandle );
      work[0] = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// �A�C�e���g�p�n���h��
static void handler_JapoNomi_Use( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
    u8 quot = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
    const BTL_POKEPARAM* target;

      param->pokeID = work[0];
      target = BTL_SVFTOOL_GetPokeParam( flowWk, param->pokeID );
      param->damage = BTL_CALC_QuotMaxHP( target, quot );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_DamageOpponent );
      HANDEX_STR_AddArg( &param->exStr, param->pokeID );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
//------------------------------------------------------------------------------
/**
 *  �����u�̂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_RenbuNomi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_RenbuNomi_Damage     }, // �_���[�W�����n���h��
    { BTL_EVENT_USE_ITEM,          handler_JapoNomi_Use         }, // �A�C�e���g�p�n���h���i�W���|�݂̂Ɠ����j
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_RenbuNomi_Damage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_SPECIAL )
    {
      BTL_EVENT_FACTOR_ConvertForIsolate( myHandle );
      work[0] = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}


//------------------------------------------------------------------------------
/**
 *  ���낢�n�[�u
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SiroiHerb( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ACTPROC_END,   handler_SiroiHerb_ActCheck  },
    { BTL_EVENT_TURNCHECK_END, handler_SiroiHerb_TurnCheck },
    { BTL_EVENT_USE_ITEM,      handler_SiroiHerb_Use   },
    { BTL_EVENT_USE_ITEM_TMP,  handler_SiroiHerb_Use   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_SiroiHerb_ActCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
  if( BPP_IsRankEffectDowned(bpp) )
  {
//    BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      ItemPushRun( myHandle, flowWk, pokeID );
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

    recover_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_RECOVER_RANK, pokeID );
      recover_param->pokeID = pokeID;
    BTL_SVF_HANDEX_Pop( flowWk, recover_param );

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
 *  �����^���n�[�u
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MentalHerb( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,  handler_MentalHerb_React  }, // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,             handler_MentalHerb_Use    },
    { BTL_EVENT_USE_ITEM_TMP,         handler_MentalHerb_Use    },
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
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
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
 *  �Ђ���̂���
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
 *  ���傤�����M�v�X
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
 *  ���񂹂��̃c��
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SenseiNoTume( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_SP_PRIORITY,    handler_SenseiNoTume_SpPriorityCheck  },  // ����s���v���C�I���e�B�`�F�b�N
//    { BTL_EVENT_WORKED_SP_PRIORITY,   handler_SenseiNoTume_SpPriorityWorked },  // ����s���v���C�I���e�B���ʔ���
//    { BTL_EVENT_ACTPROC_START,        handler_SenseiNoTume_ActStart         },  // �A�N�V�����J�n�n���h��
    { BTL_EVENT_USE_ITEM,             handler_SenseiNoTume_Use              },  // �A�C�e���g�p�n���h��

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ����s���v���C�I���e�B�`�F�b�N
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
// ����s���v���C�I���e�B���ʔ���
static void handler_SenseiNoTume_SpPriorityWorked( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    work[0] = 1;
  }
}
// �A�N�V�����J�n�n���h��
static void handler_SenseiNoTume_ActStart( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
  &&  (work[0] == 1)
  ){
//    BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
    ItemPushRun( myHandle, flowWk, pokeID );
  }
}
// �A�C�e���g�p�n���h��
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
 *  ���������̂����ہA�܂�Ղ�������
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
 *  ��������̂��邵
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_OujaNoSirusi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_OujaNoSirusi        },
    { BTL_EVENT_USE_ITEM_TMP,        handler_OujaNoSirusi_UseTmp },   // �Ȃ����铙
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_OujaNoSirusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �Ђ�݊m���O�Ȃ�A�A�C�e���З͒l�ɏ�������
    u8 per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    BTL_N_Printf( DBGSTR_HANDITEM_OujaCheck, per );
    if( per == 0 )
    {
      per = common_GetItemParam( myHandle, ITEM_PRM_ATTACK );
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
 *  ����ǂ��L�o
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SurudoiKiba( u32* numElems )
{
  // �ǂ�����u��������̂��邵�v�Ɠ���
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_OujaNoSirusi        },
    { BTL_EVENT_USE_ITEM_TMP,        handler_OujaNoSirusi_UseTmp },   // �Ȃ����铙
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

//------------------------------------------------------------------------------
/**
 *  �������������Y
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
  // �������U�����̂Ƃ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �������㏸
    fx32 ratio = Item_AttackValue_to_Ratio( myHandle );
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  �s���g�����Y
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
  // �������U�����̂Ƃ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 rank = BTL_EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK );
    rank += 1;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
  }
}
//------------------------------------------------------------------------------
/**
 *  ���b�L�[�p���`
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
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ���b�L�[�Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_RAKKII )
    {
      // �N���e�B�J���Q�i�K�㏸
      u8 rank = BTL_EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK );
      rank += 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �Ȃ��˂�
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
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �J���l�M�Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_KAMONEGI )
    {
      // �N���e�B�J���Q�i�K�㏸
      u8 rank = BTL_EVENTVAR_GetValue( BTL_EVAR_CRITICAL_RANK );
      rank += 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_RANK, rank );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �t�H�[�J�X�����Y
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
 *  �̂񂫂̂�����
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
 *  ������̃n�`�}�L
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
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �Ԃ�U���̂Ƃ��A�З͏㏸
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
 *  ���̂��胁�K�l
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
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �Ƃ�����U���̂Ƃ��A�З͏㏸
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
 *  ���񂩂��̃L�o
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
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �p�[�����Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_PAARURU )
    {
      // �Ƃ������Q�{
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
 *  ���񂩂��̃E���R
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
  // �������h�䑤��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // �p�[�����Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_PAARURU )
    {
      // �Ƃ��ڂ��Q�{
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
 *  ���^���p�E�_�[
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
  // �������h�䑤��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // ���^�����Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_METAMON )
    {
      // �ڂ�����Q�{
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
 *  �X�s�[�h�p�E�_�[
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
    // ���������^�����Ȃ�f�����Q�{
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    if( BPP_GetMonsNo(bpp) == MONSNO_METAMON )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  ������̂�����
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
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ���e�B�A�X�E���e�B�I�X�Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( (monsNo == MONSNO_RATHIASU) || (monsNo == MONSNO_RATHIOSU) )
    {
      // �Ƃ������㏸
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );

      }
    }
  }
}
static void handler_KokoroNoSizuku_Guard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    // ���e�B�A�X�E���e�B�I�X�Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( (monsNo == MONSNO_RATHIASU) || (monsNo == MONSNO_RATHIOSU) )
    {
      // �Ƃ��ڂ��㏸
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
      {
        BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �ӂƂ��ق�
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
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �J���J���E�K���K���Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( (monsNo == MONSNO_KARAKARA) || (monsNo == MONSNO_GARAGARA) )
    {
      // ���������㏸
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
 *  �������n�`�}�L
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariHachimaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,   handler_Kodawari_Common_WazaExe    }, // ���U���o�͊m��n���h��
    { BTL_EVENT_ITEMSET_DECIDE,     handler_Kodawari_Common_ItemChange }, // �A�C�e���ύX�m��n���h��
    { BTL_EVENT_ATTACKER_POWER,     handler_KodawariHachimaki_Pow      }, // �U���͌���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �U���͌���n���h��
static void handler_KodawariHachimaki_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����Ȃ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �������������㏸
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ������胁�K�l
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariMegane( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,    handler_Kodawari_Common_WazaExe    }, // ���U���o�͊m��n���h��
    { BTL_EVENT_ITEMSET_DECIDE,      handler_Kodawari_Common_ItemChange }, // �A�C�e���ύX�m��n���h��
    { BTL_EVENT_ATTACKER_POWER,      handler_KodawariMegane_Pow         }, // �U���͌���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �U���͌���n���h��
static void handler_KodawariMegane_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U�����Ȃ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ���ꂱ�������㏸
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �������X�J�[�t
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KodawariScarf( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_CALL_DECIDE,    handler_Kodawari_Common_WazaExe      }, // ���U���o�͊m��n���h��
    { BTL_EVENT_ITEMSET_DECIDE,      handler_Kodawari_Common_ItemChange   }, // �A�C�e���ύX�m��n���h��
    { BTL_EVENT_CALC_AGILITY,        handler_KodawariScarf                }, // �f�����v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �f�����v�Z�n���h��
static void handler_KodawariScarf( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID ){
    // ���΂₳�㏸
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.5) );
  }
}

// ���U�o���m��n���h���i�������n���ʁj
static void handler_Kodawari_Common_WazaExe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  enum {
    KODAWARI_SICKID = WAZASICK_KODAWARI,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    WazaID  waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
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
// �A�C�e���ύX�m��n���h��
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
 *  �������̃^�X�L
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoTasuki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK,  handler_KiaiNoTasuki_Check },    // ���炦��`�F�b�N�n���h��
    { BTL_EVENT_KORAERU_EXE,    handler_KiaiNoTasuki_Exe      }, // ���炦�锭���n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���炦��`�F�b�N�n���h��
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
// ���炦�锭���n���h��
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
 *  �������̃n�`�}�L
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KiaiNoHachimaki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK,  handler_KiaiNoHachimaki_Check   }, // ���炦��`�F�b�N�n���h��
    { BTL_EVENT_KORAERU_EXE,    handler_KiaiNoHachimaki_Exe     }, // ���炦�锭���n���h��
    { BTL_EVENT_USE_ITEM,       handler_KiaiNoHachimaki_UseItem }, // �A�C�e���g�p�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���炦��`�F�b�N�n���h��
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
// ���炦�锭���n���h��
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
// �A�C�e���g�p�n���h��
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
 *  ������̂���
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
 *  ���̂��̂���
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_InochiNoTama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT,    handler_InochiNoTama_Reaction },   // �_���[�W������n���h��
    { BTL_EVENT_WAZA_DMG_PROC2,        handler_InochiNoTama_Damage   },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W������n���h��
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
 *  ���g���m�[��
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
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u16 counter = BPP_GetWazaContCounter( bpp );
    TAYA_Printf("���g���m�[�� %d count\n", counter );
    if( counter )
    {
      fx32 ratio;
      u16  pow, damage;
      pow = 100 + (common_GetItemParam(myHandle, ITEM_PRM_ATTACK) * (counter));
      TAYA_Printf("���g���m�[�� %d pow\n", pow );
      if( pow > 200 ){ pow = 200; }
      ratio = (FX32_CONST(pow) / 100);
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �˂΂�̂����Â�
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
 *  ��������̂���
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KaigaraNoSuzu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT, handler_KaigaraNoSuzu },
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_KaigaraNoSuzu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u32 damage_sum = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
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
//------------------------------------------------------------------------------
/**
 *  �Ђ���̂˂��
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
//      BTL_Printf("�T�C�h�G�t�F�N�g�̌p���^�[������ %d �^�[������\n", inc_turns);
      BPP_SICKCONT_IncTurn( &cont, inc_turns );
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �p���t���n�[�u
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowefulHarb( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_TAMETURN_SKIP,   handler_PowefulHarb_CheckTameSkip }, // ���߃X�L�b�v�`�F�b�N
    { BTL_EVENT_TAME_SKIP,             handler_PowefulHarb_FixTameSkip   }, // ���߃X�L�b�v�m��
    { BTL_EVENT_USE_ITEM,              handler_PowefulHarb_Use           }, // �A�C�e���g�p
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���߃X�L�b�v�`�F�b�N
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
// ���߃X�L�b�v�m��
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
// �A�C�e���g�p
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
 *  ���ׂ̂���
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
 *  ���낢�w�h��
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiHedoro( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN, handler_KuroiHedoro  },  // �^�[���`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �^�[���`�F�b�N�n���h��
static void handler_KuroiHedoro( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
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
      BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
        param->pokeID = pokeID;
        param->damage = BTL_CALC_QuotMaxHP( bpp, 8 );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Damage );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
        HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ����������
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_AkaiIto( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_FIXED, handler_AkaiIto  },  // ���U�n��Ԉُ�m��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�n��Ԉُ�m��
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
      BTL_SVF_HANDEX_Pop( flowWk, param );

      // ���܂���������A�C�e���G�t�F�N�g���������b�Z�[�W
      {
        BTL_HANDEX_PARAM_HEADER* header;
        BTL_HANDEX_PARAM_MESSAGE* msg_param;

        header = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ITEM_EFFECT, pokeID );
          header->failSkipFlag = TRUE;
        BTL_SVF_HANDEX_Pop( flowWk, header );

        msg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
          msg_param->header.failSkipFlag = TRUE;
          HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MeromeroGetItem );
          HANDEX_STR_AddArg( &msg_param->str, atkPokeID );
          HANDEX_STR_AddArg( &msg_param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );
        BTL_SVF_HANDEX_Pop( flowWk, msg_param );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �������΂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuttukiBari( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION, handler_KuttukiBari_DamageReaction  },  // ���U�_���[�W�����n���h��
    { BTL_EVENT_TURNCHECK_END,     handler_KuttukiBari_TurnCheck       },  // �^�[���`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���U�_���[�W�����n���h��
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
// �^�[���`�F�b�N�n���h��
static void handler_KuttukiBari_TurnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

    BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
      param->pokeID = pokeID;
      param->damage = BTL_CALC_QuotMaxHP( bpp, common_GetItemParam(myHandle, ITEM_PRM_ATTACK) );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Damage );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}

//------------------------------------------------------------------------------
/**
 *  �p���[���X�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWrist( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �p���[�x���g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBelt( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �p���[�����Y
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerLens( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �p���[�o���h
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerBand( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �p���[�A���N��
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerAnkle( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �p���[�E�F�C�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_PowerWeight( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,      handler_PowerXXX_CalcAgility  },  // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}

/**
 *  �p���[�������i�w�͒l�A�b�v�A�C�e���j���ʁF���΂₳�v�Z�n���h��
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
 *  �Ђ̂��܃v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_HinotamaPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Mokutan },  // ���U�З̓n���h���F�u��������v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �������v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SizukuPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SinpiNoSizuku },  // ���U�З̓n���h���F�u����҂̂������v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ���������v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_IkazutiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Zisyaku },  // ���U�З̓n���h���F�u�����Ⴍ�v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �݂ǂ�̃v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MirodinoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KisekiNoTane },  // ���U�З̓n���h���F�u�������̂��ˁv�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ���̃v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TuraranoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_TokenaiKoori },  // ���U�З̓n���h���F�u�Ƃ��Ȃ�������v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ���Ԃ��̃v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KobusinoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Kuroobi },  // ���U�З̓n���h���F�u���남�сv�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �����ǂ��v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MoudokuPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_Dokubari },  // ���U�З̓n���h���F�u�ǂ��΂�v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �������̃v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_DaitinoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_YawarakaiSuna },  // ���U�З̓n���h���F�u���炩�����ȁv�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ��������v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_AozoraPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_SurudoiKutibasi },  // ���U�З̓n���h���F�u����ǂ������΂��v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �ӂ����̃v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_HusiginoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MagattaSupuun },  // ���U�З̓n���h���F�u�܂������X�v�[���v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ���܂ނ��v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TamamusiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_GinNoKona },  // ���U�З̓n���h���F�u����̂��ȁv�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ���񂹂��v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_GansekiPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KataiIsi },  // ���U�З̓n���h���F�u�����������v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ���̂̂��v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_MononokePlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_NoroiNoOfuda },  // ���U�З̓n���h���F�u�̂낢�̂��ӂ��v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ��イ�̃v���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_RyuunoPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_RyuunoKiba },  // ���U�З̓n���h���F�u��イ�̃L�o�v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  ������ăv���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KowamotePlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_KuroiMegane },  // ���U�З̓n���h���F�u���낢�߂��ˁv�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �����Ăv���[�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KoutetsuPlate( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,     handler_MetalCoat },  // ���U�З̓n���h���F�u���^���R�[�g�v�Ɠ���
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
//------------------------------------------------------------------------------
/**
 *  �������Ȃ˂���
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_OokinaNekko( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_DRAIN,     handler_OokinaNekko },  // �h���C���ʌv�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �h���C���ʌv�Z�n���h��
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
 *  ���ނ肾��
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_ADD_ITEM_Kemuridama( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_SKIP_NIGERU_CALC,       handler_Kemuridama },  // �ɂ���`�F�b�N�n���h��
    { BTL_EVENT_NIGERU_EXMSG,           handler_Kemuridama_Msg }, // �������ꃁ�b�Z�[�W�n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �ɂ���`�F�b�N�n���h��
static void handler_Kemuridama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
// �������ꃁ�b�Z�[�W�n���h��
static void handler_Kemuridama_Msg( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_GEN_FLAG, TRUE) )
    {
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_Kemuridama );
        HANDEX_STR_AddArg( &param->str, pokeID );
        HANDEX_STR_AddArg( &param->str, BTL_EVENT_FACTOR_GetSubID(myHandle) );
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ���܂��肱�΂�
 */
//------------------------------------------------------------------------------
static  const BtlEventHandlerTable*  HAND_ADD_ITEM_OmamoriKoban( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,       handler_OmamoriKoban },  // �����o�[����n���h��
  };
  *numElems = NELEMS(HandlerTable);
  return HandlerTable;
}
// �����o�[����n���h��
static void handler_OmamoriKoban( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_SVFRET_SetMoneyDblUp( flowWk, pokeID );
  }
}



//------------------------------------------------------------------------------
/**
 *  �߂�������
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_TumetaiIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_TumetaiIwa  },  // �V��ω����U�̃^�[�����`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �V��ω����U�̃^�[�����`�F�b�N
static void handler_TumetaiIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_SNOW );
}
//------------------------------------------------------------------------------
/**
 *  ���炳�炢��
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SarasaraIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_SarasaraIwa  },  // �V��ω����U�̃^�[�����`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �V��ω����U�̃^�[�����`�F�b�N
static void handler_SarasaraIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_SAND );
}
//------------------------------------------------------------------------------
/**
 *  ��������
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_AtuiIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_AtuiIwa  },  // �V��ω����U�̃^�[�����`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �V��ω����U�̃^�[�����`�F�b�N
static void handler_AtuiIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_SHINE );
}
//------------------------------------------------------------------------------
/**
 *  ���߂�������
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SimettaIwa( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_WEATHER_TURNCNT, handler_SimettaIwa  },  // �V��ω����U�̃^�[�����`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �V��ω����U�̃^�[�����`�F�b�N
static void handler_SimettaIwa( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  common_WazaWeatherTurnUp( myHandle, flowWk, pokeID, BTL_WEATHER_RAIN );
}

/*
 *  �V��ω����U�^�[���������A�C�e���̋��ʃ��[�`��
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
 *  �ł񂫂���
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
// ��������
static void handler_DenkiDama( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �s�J�`���E�Ȃ�
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    u32 monsNo = BPP_GetMonsNo( bpp );
    if( monsNo == MONSNO_PIKATYUU )
    {
      // ���������E�Ƃ������Q�{
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
// �Ȃ�������
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
 *  �ǂ��ǂ�����
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
// �Ȃ�������
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
 *  �����񂾂�
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
// �Ȃ�������
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
 *  ����̂���
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
 *  ���炩������
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
 *  ����������
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
 *  �������̂���
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
 *  ���낢�߂���
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
 *  ���남��
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
 *  �����Ⴍ
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
 *  ���^���R�[�g
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
 *  ����҂̂�����
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
 *  ����ǂ������΂�
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
 *  �ǂ��΂�
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
// �Ȃ�������
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
 *  �Ƃ��Ȃ�������
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
 *  �̂낢�̂��ӂ�
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
 *  �܂������X�v�[��
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
 *  ��������
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
 *  ��イ�̃L�o
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
 *  �V���N�̃X�J�[�t
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
 *  ���₵��������
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
 *  ���񂹂�������
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
 *  �����Ȃ݂̂�����
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
 *  �������̂�����
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
 *  ���͂Ȃ̂�����
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
 * �Ƃ��Ă��^�C�v�̃��U���З̓A�b�v�����铹��̋��ʏ���
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
 *  ���炽��
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
 *  ���񂲂�����
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
 *  ���낢�Ă����イ
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_KuroiTekkyuu( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,     handler_KuroiTekkyuu_Agility  },  // ���΂₳�v�Z�n���h��
    { BTL_EVENT_CHECK_FLYING,     handler_KuroiTekkyuu_CheckFly },  // �ӂ䂤�`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���΂₳�v�Z�n���h��
static void handler_KuroiTekkyuu_Agility( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5) );
  }
}
// �ӂ䂤�`�F�b�N�n���h��
static void handler_KuroiTekkyuu_CheckFly( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}
//------------------------------------------------------------------------------
/**
 *  ���邢��
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Karuisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WEIGHT_RATIO,     handler_Karuisi },   // �̏d�{���`�F�b�N�n���h��
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
 *  ���񂩂̂�����
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SinkanoKiseki( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DEFENDER_GUARD,     handler_SinkanoKiseki },   // �h�䑤�K�[�h�̓`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �h�䑤�K�[�h�̓`�F�b�N�n���h��
static void handler_SinkanoKiseki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤�Ői���O��������h��E���h1.5�{
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
 *  �S�c�S�c���b�g
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_GotugotuMet( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_GotugotuMet },   // �_���[�W�����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�����n���h��
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

          HANDEX_STR_Setup( &(param->exStr), BTL_STRTYPE_SET, BTL_STRID_SET_UseItem_DamageOpponent );
          HANDEX_STR_AddArg( &(param->exStr), param->pokeID );
          HANDEX_STR_AddArg( &(param->exStr), pokeID );
          HANDEX_STR_AddArg( &(param->exStr), BTL_EVENT_FACTOR_GetSubID(myHandle) );
        }
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �ӂ�����
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Huusen( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_Huusen_MemberIn       },   // �����o�[����n���h��
    { BTL_EVENT_CHECK_FLYING,        handler_Huusen_CheckFlying    },   // ��s�t���O�`�F�b�N�n���h��
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_Huusen_DamageReaction },   // �_���[�W�����n���h��
    { BTL_EVENT_ITEMSET_FIXED,       handler_Huusen_ItemSetFixed   },   // �A�C�e���������������n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �����o�[����n���h��
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
// ��s�t���O�`�F�b�N�n���h��
static void handler_Huusen_CheckFlying( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
// �_���[�W�����n���h��
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
// �A�C�e���������������n���h��
static void handler_Huusen_ItemSetFixed( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // �u�ӂ�����v���������ꂽ���s�֎~��Ԃ�����
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
 *  ���b�h�J�[�h
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_RedCard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,     handler_RedCard },   /// �_���[�W�����I��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
/// �_���[�W�����I���n���h��
static void handler_RedCard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( HandCommon_CheckTargetPokeID(pokeID) )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
    if( BTL_SVFTOOL_GetExistFrontPokePos(flowWk, targetPokeID) != BTL_POS_NULL )
    {
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
 *  �˂炢�̂܂�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_NerainoMato( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_AFFINITY,     handler_NerainoMato },   // �^�C�v�����ɂ�閳�����`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �^�C�v�����ɂ�閳�����`�F�b�N
static void handler_NerainoMato( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤�̎��A�����ɂ�薳��������Ă���ΗL���ɏ�������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, TRUE );
  }
}
//------------------------------------------------------------------------------
/**
 *  ���߂��o���h
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_SimetukeBand( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZASICK_PARAM,     handler_SimetukeBand },   // ��Ԉُ�p�����[�^�`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ��Ԉُ�p�����[�^�`�F�b�N�n���h��
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
 *  ���イ����
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Kyuukon( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Kyuukon_DmgReaction },   // �_���[�W�����n���h��
    { BTL_EVENT_USE_ITEM,              handler_Kyuukon_Use   },         // �ǂ����g�p�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_Kyuukon_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // ���������^�C�v�̃��U�_���[�W���󂯂����A���U���܂��オ��Ȃ甽������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
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
// �ǂ����g�p�n���h��
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
 *  ���イ�ł�
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_Juudenti( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Juudenti_DmgReaction },   // �_���[�W�����n���h��
    { BTL_EVENT_USE_ITEM,              handler_Juudenti_Use   },         // �ǂ����g�p�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�����n���h��
static void handler_Juudenti_DmgReaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������d�C�^�C�v�̃��U�_���[�W���󂯂����A�U�����܂��オ��Ȃ甽������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
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
// �ǂ����g�p�n���h��
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
 *  ��������{�^��
 */
//------------------------------------------------------------------------------
static const BtlEventHandlerTable* HAND_ADD_ITEM_DassyutuButton( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_END_HIT_REAL,     handler_DassyutuButton_Reaction },  // �_���[�W�����I���n���h��
    { BTL_EVENT_USE_ITEM,                handler_DassyutuButton_Use      },  // �ǂ����g�p�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �_���[�W�����I���n���h��
static void handler_DassyutuButton_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������_���[�W���󂯂���

  if( (HandCommon_CheckTargetPokeID(pokeID))
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_DELAY_ATTACK_FLAG) == FALSE )
  ){
    // �T���Ɍ�։\�����o�[������Ȃ�A�ǂ����g�p�n���h���Ăяo��
    //�i���荞�݃A�N�V�����������łɃ����o�[����ւ��s������낤�Ƃ��Ă���|�P����������ꍇ�������j
    if( (!BTL_SVFTOOL_IsMemberOutIntr(flowWk))
    &&  (BTL_SVFTOOL_IsExistBenchPoke(flowWk, pokeID))
    &&  (BTL_SVFTOOL_ReserveMemberChangeAction(flowWk))
    ){
//      BTL_SVF_HANDEX_PushRun( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
      ItemPushRun( myHandle, flowWk, pokeID );
    }
  }
}
// �ǂ����g�p�n���h��
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
      BTL_HANDEX_PARAM_CHANGE_MEMBER* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHANGE_MEMBER, pokeID );
        param->pokeID = pokeID;
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_DassyutuPod );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      BTL_SVF_HANDEX_Pop( flowWk, param );
      #endif
    }
  }
}

//------------------------------------------------------------------------------
/**
 *  �������W���G��
 */
//------------------------------------------------------------------------------

/**
 *  �ق̂��̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_HonooNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_HonooNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_HonooNoJuel_Pow },
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
 *  �݂��̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_MizuNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_MizuNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_MizuNoJuel_Pow },
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
 *  �ł񂫂̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_DenkiNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_DenkiNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_DenkiNoJuel_Pow },
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
 *  �����̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_KusaNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_KusaNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_KusaNoJuel_Pow },
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
 *  ������̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_KooriNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_KooriNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_KooriNoJuel_Pow },
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
 *  �����Ƃ��W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_KakutouJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_KakutouJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_KakutouJuel_Pow },
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
 *  �ǂ��̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_DokuNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_DokuNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_DokuNoJuel_Pow },
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
 *  ���߂�̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_JimenNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_JimenNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_JimenNoJuel_Pow },
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
 *  �Ђ����̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_HikouNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_HikouNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_HikouNoJuel_Pow },
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
 *  �G�X�p�[�W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_EsperJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_EsperJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_EsperJuel_Pow },
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
 *  �ނ��̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_MusiNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_MusiNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_MusiNoJuel_Pow },
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
 *  ����̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_IwaNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_IwaNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_IwaNoJuel_Pow },
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
 *  �S�[�X�g�W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_GhostJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_GhostJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_GhostJuel_Pow },
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
 *  �h���S���W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_DragonJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_DragonNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_DragonNoJuel_Pow },
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
 *  �����̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_AkuNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_AkuNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_AkuNoJuel_Pow },
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
 *  �͂��˂̃W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_HaganeNoJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_HaganeNoJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_HaganeNoJuel_Pow },
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
 *  �m�[�}���W���G��
 */
static const BtlEventHandlerTable* HAND_ADD_ITEM_NormalJuel( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_DAMAGEPROC_START,       handler_NormalJuel_Decide },
    { BTL_EVENT_WAZA_POWER,             handler_NormalJuel_Pow    },
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


// �W���G���F���U�o���m��n���h������
static void common_Juel_Decide( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  ){
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_IsDamage(waza) )
    {
      if( !BTL_SVFTOOL_IsSimulationMode(flowWk) )
      {
        BTL_HANDEX_PARAM_CONSUME_ITEM* param;

        BTL_EVENT_FACTOR_ConvertForIsolate( myHandle );

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CONSUME_ITEM, pokeID );
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_STD, BTL_STRID_STD_Juel );
          HANDEX_STR_AddArg( &param->exStr, BTL_EVENT_FACTOR_GetSubID(myHandle) );
          HANDEX_STR_AddArg( &param->exStr, BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) );
        BTL_SVF_HANDEX_Pop( flowWk, param );

      }
      work[0] = 1;
    }
  }
}
// �W���G���F�З͌v�Z�n���h������
static void common_Juel_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type )
{
  // �������U�������^�C�v���}�b�`���Ă�����З͂𑝉�
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  &&  (work[0] == 1)
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5f) );

    if( BTL_SVFTOOL_IsSimulationMode(flowWk) ){
      work[0] = 0;
    }
  }
}
