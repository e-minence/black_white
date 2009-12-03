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

#include "hand_item.h"


enum {
  WORKIDX_TMP_FLAG = EVENT_HANDLER_WORK_ELEMS-1,  ///< �ꎞ���p�A�C�e��
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



// �C�x���g�n���h���o�^�֐��̌^��`
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
  { ITEM_MANPUKUOKOU,       HAND_ADD_ITEM_KoukouNoSippo     },  // �܂�Ղ�������=���������̂����ۓ���
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

  { ITEM_GOTUGOTUMETTO,     HAND_ADD_ITEM_GotugotuMet     },  // �S�c�S�c���b�g
  { ITEM_HUUSEN,            HAND_ADD_ITEM_Huusen          },  // �ӂ�����
  { ITEM_REDDOKAADO,        HAND_ADD_ITEM_RedCard         },  // ���b�h�J�[�h
  { ITEM_NERAINOMATO,       HAND_ADD_ITEM_NerainoMato     },  // �˂炢�̂܂�
  { ITEM_SIMETUKEBANDO,     HAND_ADD_ITEM_SimetukeBand    },  // ���߂��o���h
  { ITEM_KYUUKON,           HAND_ADD_ITEM_Kyuukon         },  // ���イ����
  { ITEM_ZYUUDENTI,         HAND_ADD_ITEM_Juudenti        },  // ���イ�ł�
  { ITEM_DASYUTUPODDO,      HAND_ADD_ITEM_DassyutuPod     },  // ������|�b�h
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
 * �|�P�����́u�A�C�e���v�n���h�����V�X�e������S�č폜
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
 * �A�C�e���p�����[�^�擾
 *
 * @param   myHandle
 * @param   paramID   �p�����[�^ID�iitem/item.h)
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
  if( work[WORKIDX_TMP_FLAG] ){
    common_useForSick( myHandle, flowWk, pokeID, WAZASICK_MAHI );
  }
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KuraboNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KuraboNomi },       // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_KuraboNomi_Use },   // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_KuraboNomi_UseTmp },// �����ꎞ�g�p�i���΂ނȂǁj
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KagoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KagoNomi },       // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_KagoNomi_Use },   // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_KagoNomi_UseTmp },// �����ꎞ�g�p�i���΂ނȂǁj
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ChigoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_ChigoNomi },       // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_ChigoNomi_Use },   // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_ChigoNomi_UseTmp },// �����ꎞ�g�p�i���΂ނȂǁj
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NanasiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_NanasiNomi },       // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_NanasiNomi_Use },   // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_NanasiNomi_UseTmp },// �����ꎞ�g�p�i���΂ނȂǁj
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_KiiNomi },       // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_KiiNomi_Use },   // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_KiiNomi_UseTmp },// �����ꎞ�g�p�i���΂ނȂǁj
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MomonNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_MomonNomi },       // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_MomonNomi_Use },   // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_MomonNomi_UseTmp },// �����ꎞ�g�p�i���΂ނȂǁj
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RamNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION, handler_RamNomi },       // ��Ԉُ�`�F�b�N�n���h��
    { BTL_EVENT_USE_ITEM,            handler_RamNomi_Use },   // �ʏ퓹��g�p
    { BTL_EVENT_USE_ITEM_TMP,        handler_RamNomi_UseTmp },// �����ꎞ�g�p�i���΂ނȂǁj
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
 *  �q�����̂�
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
      // work[0] = �Ώۃ��U�C���f�b�N�X�̎󂯓n���p�Ɏg��
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
      if( PP == 0 ){  // �c��PP=0 �̃��U�͗D�悷��
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
 *  �I�����̂�
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
 *  ���̂݃W���[�X
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KinomiJuice( u16 pri, u16 itemID, u8 pokeID )
{
  // �S���g�I�����̂݁h�Ɠ����ŗǂ�
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_OrenNomi_Reaction },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  �I�{���̂�
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
 *  �t�B���̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FiraNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ �Ƃ肠�����I�����ƈꏏ��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  �E�C�̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UiNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ �Ƃ肠�����I�����ƈꏏ��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  �}�S�̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MagoNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ �Ƃ肠�����I�����ƈꏏ��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  �o���W�̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BanziNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,         handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ �Ƃ肠�����I�����ƈꏏ��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *  �C�A�̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_IaNomi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,    handler_PinchReactCommon },
    { BTL_EVENT_USE_ITEM,               handler_OrenNomi_Use }, // @@@ �Ƃ肠�����I�����ƈꏏ��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *  �`�C���̂�
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
 *  �����K�̂�
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
 *  �J�����̂�
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
 *  ���^�s�̂�
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
 *  �Y�A�̂�
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
 *  �T���̂�
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
 *  �X�^�[�̂�
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
  }
}
//------------------------------------------------------------------------------
/**
 *  �i�]�̂�
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
 *  �I�b�J�̂�
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
 *  �C�g�P�̂�
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
 *  �\�N�m�̂�
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
 *  �����h�̂�
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
 *  ���`�F�̂�
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
 *  ���v�̂�
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
 *  �r�A�[�̂�
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
 *  �V���J�̂�
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
 *  �o�R�E�̂�
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
 *  �E�^���̂�
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
 *  �^���K�̂�
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
 *  �����M�̂�
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
 *  �J�V�u�̂�
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
 *  �n�o���̂�
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
 *  �i���̂�
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
 *  �����o�̂�
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
 *  �z�Y�̂�
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
 *  �З͔�����������TRUE��Ԃ�
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
    s32 item_pow = common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    BTL_Printf("�s���`���A�N�V�����A�C�e��: n=%d\n", item_pow );
    common_DamageReactCore( flowWk, pokeID, item_pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * HP 1/n �ȉ��Ŕ�������A�C�e���̋��ʏ���
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
      BTL_Printf("�ő�HP=%d, ��HP=%d, n=%d ... ��������\n", BPP_GetValue(bpp,BPP_MAX_HP), hp, n);
      BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ���낢�n�[�u
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
 *  �����^���n�[�u
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MentalHerb( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CHECK_ITEM_REACTION,  handler_MentalHerb_React  }, // ��Ԉُ�`�F�b�N�n���h��
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
    work[0] = 1;  // �퓬���A�P�x�����g���Ȃ�
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
 *  �Ђ���̂���
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
 *  ���傤�����M�v�X
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
 *  ���񂹂��̃c��
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
 *  ���������̂����ہA�܂�Ղ�������
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
 *  ��������̂��邵
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OujaNoSirusi( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_OujaNoSirusi },
    { BTL_EVENT_USE_ITEM_TMP,        handler_OujaNoSirusi_UseTmp },   // �Ȃ����铙
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_OujaNoSirusi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �Ђ�݊m���O�Ȃ�A�A�C�e���З͒l�ɏ�������
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
 *  ����ǂ��c��
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SurudoiTume( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_SHRINK_PER,     handler_SurudoiTume },
    { BTL_EVENT_USE_ITEM_TMP,        handler_SurudoiTume_UseTmp },   // �Ȃ����铙
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
static void handler_SurudoiTume( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �Ђ�݊m���O�Ȃ�A�A�C�e���З͒l�ɏ�������
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
 *  �������������Y
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
  // �������U�����̂Ƃ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �������㏸
    u32 r = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
    fx32 fx_ratio = FX32_CONST(r) / 100;
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, fx_ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  �s���g�����Y
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
 *  �̂񂫂̂�����
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
 *  ���ނ肾��
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
 *  ������̃n�`�}�L
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
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �Ԃ�U���̂Ƃ��A�З͏㏸
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
 *  ���̂��胁�K�l
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
  // �������U������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �Ƃ�����U���̂Ƃ��A�З͏㏸
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
 *  ���񂩂��̃L�o
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
        u32 guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
        guard *= 2;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GUARD, guard );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ���^���p�E�_�[
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
        u32 guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
        guard *= 2;
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_GUARD, guard );
      }
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �X�s�[�h�p�E�_�[
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
 *  �ł񂫂���
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
 *  ������̂�����
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
 *  �ӂƂ��ق�
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
  // �������U�����Ȃ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // �������������㏸
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ������胁�K�l
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
  // �������U�����Ȃ�
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    // ���ꂱ�������㏸
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �������X�J�[�t
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
    // ���΂₳�㏸
    BTL_EVENTVAR_MulValue( BTL_EVAR_AGILITY, FX32_CONST(2) );
  }
}
// �������n����
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
 *  �������̃^�X�L
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
 *  �������̃n�`�}�L
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
 *  ������̂���
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
    BTL_Printf("������̂��сFitemAtk=%d, ratio=%08x\n", itemAtk, ratio);
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  ���̂��̂���
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
    BTL_Printf("���̂��̂��܁FitemAtk=%d, ratio=%08x\n", itemAtk, ratio);
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
}
//------------------------------------------------------------------------------
/**
 *  ���g���m�[��
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
 *  �˂΂�̂����Â�
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
 *  ��������̂���
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
 *  ���ׂ̂���
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
 *  �ǂ��ǂ�����
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
// �Ȃ�������
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
 *  �����񂾂�
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
// �Ȃ�������
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
 *  ����̂���
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
 *  ���炩������
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
 *  ����������
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
 *  �������̂���
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
 *  ���낢�߂���
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
 *  ���남��
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
 *  �����Ⴍ
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
 *  ���^���R�[�g
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
 *  ����҂̂�����
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
 *  ����ǂ������΂�
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
 *  �ǂ��΂�
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
// �Ȃ�������
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
 *  �Ƃ��Ȃ�������
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
 *  �̂낢�̂��ӂ�
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
 *  �܂������X�v�[��
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
 *  ��������
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
 *  ��イ�̃L�o
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
 *  �V���N�̃X�J�[�t
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
 *  ���₵��������
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
 *  ���񂹂�������
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
 *  �����Ȃ݂̂�����
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
 *  �������̂�����
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
 *  ���͂Ȃ̂�����
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
      u32 waza_pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      u32 ratio = 100 + common_GetItemParam( myHandle, flowWk, ITEM_PRM_ATTACK );
      waza_pow = (waza_pow * ratio) / 100;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, waza_pow );
    }
  }
}


//------------------------------------------------------------------------------
/**
 *  �S�c�S�c���b�g
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_GotugotuMet( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_GotugotuMet },   // �_���[�W�����n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// �_���[�W�����n���h��
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
      // @todo �����Ƀ��b�Z�[�W�͗v��Ȃ��̂��ȁH
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  �ӂ�����
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Huusen( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,           handler_Huusen_MemberIn       },   // �����o�[����n���h��
    { BTL_EVENT_CHECK_FLYING,        handler_Huusen_CheckFlying    },   // ��s�t���O�`�F�b�N�n���h��
    { BTL_EVENT_WAZA_DMG_REACTION,   handler_Huusen_DamageReaction },   // �_���[�W�����n���h��
    { BTL_EVENT_ITEMSET_FIXED,       handler_Huusen_ItemSetFixed   },   // �A�C�e�����������m���
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
  }
}
// �A�C�e�����������m���n���h��
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
 *  ���b�h�J�[�h
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RedCard( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_RedCard },   /// �_���[�W�����n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
/// �_���[�W�����n���h��
static void handler_RedCard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    BTL_HANDEX_PARAM_PUSHOUT* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_PUSHOUT, pokeID );
    param->pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_RedCard );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
    HANDEX_STR_AddArg( &param->exStr, param->pokeID );

    // ����𐁂���΂�����A�A�C�e���������
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
 *  �˂炢�̂܂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NerainoMato( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_TYPE_CHECK,     handler_NerainoMato },   // �^�C�v�����ɂ�閳�����`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// �^�C�v�����ɂ�閳�����`�F�b�N
static void handler_NerainoMato( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������h�䑤�̎��A�����ɂ�薳��������Ă���ΗL���ɏ�������
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_NOEFFECT_FLAG) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, FALSE );
    }
  }
}
//------------------------------------------------------------------------------
/**
 *  ���߂��o���h
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_SimetukeBand( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,     handler_SimetukeBand },   // ��Ԉُ�p�����[�^�`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// ��Ԉُ�p�����[�^�`�F�b�N�n���h��
static void handler_SimetukeBand( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BPP_SICK_CONT cont;

    cont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
    BPP_SICKCONT_SetFlag( &cont, TRUE );
    BTL_Printf("���߂����\n");
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
  }
}
//------------------------------------------------------------------------------
/**
 *  ���イ����
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Kyuukon( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Kyuukon_DmgReaction },   // �_���[�W�����n���h��
    { BTL_EVENT_USE_ITEM,              handler_Kyuukon_Use   },         // �ǂ����g�p�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
      if( BPP_IsRankEffectValid(bpp, BPP_SP_ATTACK, 1) )
      {
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
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
    param->rankType = BPP_SP_ATTACK;
    param->rankVolume = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ���イ�ł�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_Juudenti( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_Juudenti_DmgReaction },   // �_���[�W�����n���h��
    { BTL_EVENT_USE_ITEM,              handler_Juudenti_Use   },         // �ǂ����g�p�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
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
      if( BPP_IsRankEffectValid(bpp, BPP_ATTACK, 1) )
      {
        BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
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
    param->rankType = BPP_ATTACK;
    param->rankVolume = 1;
  }
}
//------------------------------------------------------------------------------
/**
 *  ��������|�b�h
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_DassyutuPod( u16 pri, u16 itemID, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_REACTION,     handler_DassyutuPod_Reaction },  // �_���[�W�����n���h��
    { BTL_EVENT_USE_ITEM,              handler_DassyutuPod_Use      },  // �ǂ����g�p�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_ITEM, itemID, pri, pokeID, HandlerTable );
}
// �_���[�W�����n���h��
static void handler_DassyutuPod_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �������_���[�W���󂯂���A�ǂ����g�p�n���h���Ăяo��
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_USE_ITEM, pokeID );
  }
}
// �ǂ����g�p�n���h��
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
 *  �������W���G��
 */
//------------------------------------------------------------------------------

// �_���[�W�m��n���h������
static void common_Juel_DmgDetermine( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type )
{
  // �������U�������^�C�v���}�b�`���Ă����烁�b�Z�[�W�\��
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

// �З͌v�Z�n���h������
static void common_Juel_Power( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work, PokeType type )
{
  // �������U�������^�C�v���}�b�`���Ă�����З͂𑝉����A�C�e���폜
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == type)
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.5f) );
    BTL_SVFRET_RemoveItem( flowWk, pokeID );
  }
}
/**
 *  �ق̂��̃W���G��
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
 *  �݂��̃W���G��
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
 *  �ł񂫂̃W���G��
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
 *  �����̃W���G��
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
 *  ������̃W���G��
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
 *  �����Ƃ��W���G��
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
 *  �ǂ��̃W���G��
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
 *  ���߂�̃W���G��
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
 *  �Ђ����̃W���G��
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
 *  �G�X�p�[�W���G��
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
 *  �ނ��̃W���G��
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
 *  ����̃W���G��
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
 *  �S�[�X�g�W���G��
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
 *  �h���S���W���G��
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
 *  �����̃W���G��
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
 *  �͂��˂̃W���G��
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
 *  �m�[�}���W���G��
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


