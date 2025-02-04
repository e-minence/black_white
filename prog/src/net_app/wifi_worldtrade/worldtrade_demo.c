//============================================================================================
/**
 * @file	worldtrade_demo.c
 * @brief	世界交換ポケモン交換デモ呼び出し
 * @author	Akito Mori
 * @date	06.05.10
 */
//============================================================================================
#ifdef PM_DEBUG
//#define DEBUG_DEMO_NONE   //デモを飛ばす
#endif


#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include <dwc.h>
#include <dpw_tr.h>
#include "print/wordset.h"
#include "message.naix"
#include "system/wipe.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "savedata/wifilist.h"
#include "savedata/worldtrade_data.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "savedata/box_savedata.h"
#include "item/itemequip.h"

#include "item/itemsym.h"

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "net_app/pokemontrade.h"
#include "demo/shinka_demo.h"
#include "poke_tool/shinka_check.h"

FS_EXTERN_OVERLAY(pokemon_trade);
FS_EXTERN_OVERLAY( ui_common );

//============================================================================================
//	プロトタイプ宣言
//============================================================================================
/*** 関数プロトタイプ ***/
static POKEMON_PARAM *RecvPokemonParamPointerGet( WORLDTRADE_WORK *wk, int mode );
static void EvoPokemonUpdate( WORLDTRADE_WORK *wk );




//============================================================================================
//	GFL_PROC定義
//============================================================================================

//============================================================================================
//	定義
//============================================================================================
enum{
	DEMO_MODE_DEMO=0,
	DEMO_MODE_SHINKA,
};

enum
{ 
  DEMO_DIV_NONE,
  DEMO_DIV_MID,
  DEMO_DIV_RECV,
};

//============================================================================================
//	プロセス関数
//============================================================================================

//==============================================================================
/**
 * @brief   世界交換入り口画面初期化
 *
 * @param   wk		GTS画面ワーク
 * @param   seq		（未使用）
 *
 * @retval  int		アプリシーケンス
 */
//==============================================================================
int WorldTrade_Demo_Init(WORLDTRADE_WORK *wk, int seq)
{
  POKEMONTRADE_DEMO_PARAM *p_param;
  const GFL_PROC_DATA *call_proc;

  WorldTrade_ExitSystem( wk );

  wk->sub_proc_wk = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, sizeof(POKEMONTRADE_DEMO_PARAM) );
  GFL_STD_MemClear( wk->sub_proc_wk, sizeof(POKEMONTRADE_DEMO_PARAM) );
  p_param = wk->sub_proc_wk;
	wk->demoPokePara = PokemonParam_AllocWork( HEAPID_WORLDTRADE );

  p_param->gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
  p_param->pMy      = wk->param->mystatus;

	// モードごとにパラメータを格納する
	switch(wk->sub_process_mode){
	// 預ける
	case MODE_UPLOAD:
    //アップロードするポケモンは名前が置換されている可能性があるので、
    //セーブデータのポケモンを使う
		p_param->pMyPoke = WorldTradeData_GetPokemonDataPtr( wk->param->worldtrade_data );
;
//      (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;
		p_param->pNPCPoke = p_param->pMyPoke;
		wk->pNPCStatus = MakePartnerStatusData( &wk->UploadPokemonData );
		p_param->pNPC  = wk->pNPCStatus;

    call_proc = &PokemonTradeGTSSendProcData;
		break;

	// 受け取る
	case MODE_DOWNLOAD:
    //受け取ったポケモンは名前が置換されている可能性があるので、
    //交換されていないばあいセーブデータのポケモンを使う
    if( wk->UploadPokemonData.isTrade )
    {
      p_param->pNPCPoke = (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;
    }
    else
    {
      p_param->pNPCPoke = WorldTradeData_GetPokemonDataPtr( wk->param->worldtrade_data );
    }
		p_param->pMyPoke = p_param->pNPCPoke;
		// 相手のMYSTATUSが無いので、できる限りでっちあげる
		wk->pNPCStatus = MakePartnerStatusData( &wk->UploadPokemonData );
		p_param->pNPC  = wk->pNPCStatus;

    call_proc = &PokemonTradeGTSRecvProcData;
		break;

	// 受け取るポケモンが交換されていた
	case MODE_DOWNLOAD_EX:
    //受け取ったポケモンは名前が置換されている可能性があるので、
    //交換されていないばあいセーブデータのポケモンを使う
    if( wk->UploadPokemonData.isTrade )
    {
      p_param->pNPCPoke = (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;
    }
    else
    {
      p_param->pNPCPoke = WorldTradeData_GetPokemonDataPtr( wk->param->worldtrade_data );
    }

    WorldTradeData_GetPokemonData( wk->param->worldtrade_data, wk->demoPokePara );
    p_param->pMyPoke = wk->demoPokePara;

    // 相手のMYSTATUSが無いので、できる限りでっちあげる
    wk->pNPCStatus 			= MakePartnerStatusData( &wk->UploadPokemonData );
    p_param->pNPC  = wk->pNPCStatus;

    call_proc = &PokemonTradeGTSMidProcData;
		break;

	// 交換する
	case MODE_EXCHANGE:

    POKETOOL_CopyPPtoPP( wk->evilcheck_dummy_poke, wk->demoPokePara );
		p_param->pMyPoke = wk->evilcheck_dummy_poke;
		p_param->pNPCPoke = (POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data;
		// 相手のMYSTATUSが無いので、できる限りでっちあげる
		wk->pNPCStatus = MakePartnerStatusData( &wk->DownloadPokemonData[wk->TouchTrainerPos] );
		p_param->pNPC  = wk->pNPCStatus;

    OS_Printf( "my monsno %d\n", PP_Get( p_param->pMyPoke, ID_PARA_monsno, NULL ) );
    OS_Printf( "npc monsno %d\n", PP_Get( p_param->pNPCPoke, ID_PARA_monsno, NULL ) );

    call_proc = &PokemonTradeGTSMidProcData;
		break;
  default:
    GF_ASSERT_HEAVY(0);
	}
#ifndef DEBUG_DEMO_NONE
  GFL_PROC_LOCAL_CallProc( wk->local_proc,
		FS_OVERLAY_ID(pokemon_trade), call_proc, wk->sub_proc_wk );
#endif

	wk->subprocflag = 1;
  wk->sub_display_continue  = FALSE;

	return SEQ_FADEIN;
}
//==============================================================================
/**
 * @brief   世界交換入り口画面メイン
 *
 * @param   wk		GTS画面ワーク
 * @param   seq		
 *
 * @retval  int		アプリシーケンス
 */
//==============================================================================
int WorldTrade_Demo_Main(WORLDTRADE_WORK *wk, int seq)
{
  POKEMONTRADE_DEMO_PARAM *p_param  = wk->sub_proc_wk;
	int ret=SEQ_MAIN;

	switch(wk->subprocess_seq){
  case DEMO_MODE_DEMO:
    if( wk->local_proc_status == GFL_PROC_MAIN_NULL )
    {

			// 進化チェック
			if(wk->sub_process_mode==MODE_EXCHANGE){
				// 自分で交換した
				POKEMON_PARAM *pp = RecvPokemonParamPointerGet( wk, wk->sub_process_mode );
				int item     = PP_Get( pp, ID_PARA_item, NULL );
				int shinkano;
				SHINKA_COND shinka_cond;
				OS_Printf( "進化チェック %d\n",PP_Get(pp,ID_PARA_monsno,NULL));


				shinkano=SHINKA_Check( NULL, pp, SHINKA_TYPE_TUUSHIN, 
            (u32)wk->demoPokePara,
            GAMEDATA_GetSeasonID( GAMESYSTEM_GetGameData( wk->param->gamesys ) ), &shinka_cond, HEAPID_WORLDTRADE );

				if(shinkano!=0){

          SHINKA_DEMO_PARAM *p_param;

          //進化チェックのためもう一度PROCを作るため一旦削除
          if( wk->sub_proc_wk )
          { 
            GFL_HEAP_FreeMemory(wk->sub_proc_wk);
            wk->sub_proc_wk = NULL;
          }

          //進化チェックのためもう一度作成
          wk->sub_proc_wk = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, sizeof(SHINKA_DEMO_PARAM) );
          GFL_STD_MemClear( wk->sub_proc_wk, sizeof(SHINKA_DEMO_PARAM) );
          p_param = wk->sub_proc_wk;
          p_param->gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
          p_param->ppt      = PokeParty_AllocPartyWork(HEAPID_WORLDTRADE);
          PokeParty_Add( (POKEPARTY *)p_param->ppt, pp );
          p_param->after_mons_no  = shinkano;
          p_param->shinka_pos     = 0;
          p_param->shinka_cond    = shinka_cond;
          p_param->b_field        = TRUE;
          p_param->b_enable_cancel= FALSE;

          //進化デモ
#ifndef DEBUG_DEMO_NONE
          GFL_OVERLAY_Unload( FS_OVERLAY_ID( ui_common ));

          GFL_PROC_LOCAL_CallProc( wk->local_proc,
              FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, wk->sub_proc_wk );
#endif

					wk->subprocess_seq = DEMO_MODE_SHINKA;

				}else{
					// 進化無しならそのまま終了

					WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
					ret = SEQ_FADEOUT;
				}
			}else if(wk->sub_process_mode==MODE_DOWNLOAD || wk->sub_process_mode==MODE_DOWNLOAD_EX){
				// 自分のポケモンを引き取ったら交換されてた?
				POKEMON_PARAM *pp     = RecvPokemonParamPointerGet( wk, wk->sub_process_mode );
        u16 other_poke  = wk->UploadPokemonData.wantSimple.characterNo;
				POKEMON_PARAM *backup = PokemonParam_AllocWork(HEAPID_WORLDTRADE);
				WorldTradeData_GetPokemonData( wk->param->worldtrade_data, backup );
				if(PP_Get(pp,ID_PARA_monsno,NULL)!=PP_Get(backup,ID_PARA_monsno,NULL)
					|| PP_Get(pp,ID_PARA_personal_rnd,NULL)!=PP_Get(backup,ID_PARA_personal_rnd,NULL)){

					int item     = PP_Get( pp, ID_PARA_item, NULL );
					int shinkano = 0;
					SHINKA_COND shinka_cond;
          u32 param = 0;
          BOOL is_check  = TRUE;


          if( wk->is_trade_download )
          {
            OS_Printf( "進化チェック %d\n",PP_Get(pp,ID_PARA_monsno,NULL));
            wk->is_trade_download = FALSE;
            shinkano=SHINKA_Check( NULL, pp, SHINKA_TYPE_TUUSHIN,
              (u32)backup,
              GAMEDATA_GetSeasonID( GAMESYSTEM_GetGameData( wk->param->gamesys ) ),
              &shinka_cond, HEAPID_WORLDTRADE );
          }
          else
          {
            OS_Printf( "交換していないので、進化しない\n" );
          }

					if(shinkano!=0){
            SHINKA_DEMO_PARAM *p_param;

            //進化チェックのためもう一度PROCを作るため一旦削除
            if( wk->sub_proc_wk )
            { 
              GFL_HEAP_FreeMemory(wk->sub_proc_wk);
              wk->sub_proc_wk = NULL;
            }

            //進化チェックのためもう一度作成
            wk->sub_proc_wk = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, sizeof(SHINKA_DEMO_PARAM) );
            GFL_STD_MemClear( wk->sub_proc_wk, sizeof(SHINKA_DEMO_PARAM) );
            p_param = wk->sub_proc_wk;
            p_param->gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
            p_param->ppt      = PokeParty_AllocPartyWork(HEAPID_WORLDTRADE);
            PokeParty_Add( (POKEPARTY *)p_param->ppt, pp );
            p_param->after_mons_no  = shinkano;
            p_param->shinka_pos     = 0;
            p_param->shinka_cond    = shinka_cond;
            p_param->b_field        = TRUE;
            p_param->b_enable_cancel= FALSE;

            //進化デモ
#ifndef DEBUG_DEMO_NONE
            GFL_OVERLAY_Unload( FS_OVERLAY_ID( ui_common ));

            GFL_PROC_LOCAL_CallProc( wk->local_proc,
                FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, wk->sub_proc_wk );
#endif
            wk->subprocess_seq = DEMO_MODE_SHINKA;

          }else{
						// 進化無しならそのまま終了
						WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
						ret = SEQ_FADEOUT;
				
					}
					// 比較用に使ったポケモンデータを解放
				}else{
					// 自分のポケモンを引き取ったのであればそのまま
					WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
					ret = SEQ_FADEOUT;
				}
				GFL_HEAP_FreeMemory(backup);
							
			}else{
				// 進化無しならそのまま終了
				WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
				ret = SEQ_FADEOUT;
			}
		}

		break;
	case DEMO_MODE_SHINKA:
    if( wk->local_proc_status == GFL_PROC_MAIN_NULL )
    {
      if( wk->sub_proc_wk )
      {
        SHINKA_DEMO_PARAM *p_param  = wk->sub_proc_wk;
				POKEMON_PARAM *pp     = RecvPokemonParamPointerGet( wk, wk->sub_process_mode );
        POKEMON_PARAM *shinka_pp   = PokeParty_GetMemberPointer( p_param->ppt, 0 );  // 0番に格納したので0番から取得
        // ppを進化後のもので上書きする
        POKETOOL_CopyPPtoPP( shinka_pp, pp );
        GFL_HEAP_FreeMemory( (void*)p_param->ppt );
        //パラメータクリア
        GFL_HEAP_FreeMemory(wk->sub_proc_wk);
        wk->sub_proc_wk = NULL;
      }

			// 進化後のPOKEMON_PARAMをさっき格納した場所に反映させる
			EvoPokemonUpdate( wk );

			// ハードウェアウインドウを解除
			GX_SetVisibleWnd(GX_WNDMASK_NONE);

      GFL_OVERLAY_Load( FS_OVERLAY_ID( ui_common ));

			// セーブへ
//			WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
			WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_POKEMON_EVO_SAVE );

			ret = SEQ_FADEOUT;
		}
		break;
	}

	return ret;
}


//==============================================================================
/**
 * @brief   世界交換入り口画面終了
 *
 * @param   wk		GTS画面ワーク
 * @param   seq		（未使用）
 *
 * @retval  int		アプリシーケンス
 */
//==============================================================================
int WorldTrade_Demo_End(WORLDTRADE_WORK *wk, int seq)
{
  if( wk->pNPCStatus )
  { 
    GFL_HEAP_FreeMemory( wk->pNPCStatus );
    wk->pNPCStatus  = NULL;
  }

	GFL_HEAP_FreeMemory(wk->demoPokePara);
  if( wk->sub_proc_wk )
  { 
    GFL_HEAP_FreeMemory(wk->sub_proc_wk);
    wk->sub_proc_wk = NULL;
  }

  wk->is_trade_download = FALSE;

  WorldTrade_InitSystem( wk );

	// ボックス画面に戻る
	WorldTrade_SubProcessUpdate( wk );

  G2_BlendNone();

	return SEQ_INIT;
}


//------------------------------------------------------------------
/**
 * @brief   wifiの相手からはMYSTATUSを貰わないので、情報を設定する
 *
 * @param   dtd			サーバーから貰う交換データ
 *
 * @retval  MYSTATUS *	ステータス情報Allocして返す
 */
//------------------------------------------------------------------
MYSTATUS *MakePartnerStatusData( Dpw_Tr_Data *dtd )
{
	MYSTATUS *status = MyStatus_AllocWork( HEAPID_WORLDTRADE );
	
	MyStatus_Init( status );
	
	// トレーナー名格納
	MyStatus_SetMyName(status, dtd->name );

	// ROM・リージョンコード格納
	MyStatus_SetRomCode( status,    dtd->versionCode );
	MyStatus_SetRegionCode( status, dtd->langCode );
	
  MyStatus_SetMySex( status, dtd->gender );
  MyStatus_SetTrainerView( status, dtd->trainerType );

  MyStatus_SetMyNationArea( status, dtd->countryCode, dtd->localCode );

  //プレイヤーＩＤ
  status->id        = dtd->trainerID;
  status->profileID = dtd->id;
	
	return status;
}


//------------------------------------------------------------------
/**
 * @brief   交換したポケモンのポインタを返す関数
 *
 * @param   wk		
 * @param   mode	
 *
 * @retval  POKEMON_PARAM *		
 */
//------------------------------------------------------------------
static POKEMON_PARAM *RecvPokemonParamPointerGet( WORLDTRADE_WORK *wk, int mode )
{
	// 能動的交換と、受動的交換のポケモンのポインタ
	if( mode == MODE_EXCHANGE){
		return (POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data;
	}else if( mode == MODE_DOWNLOAD_EX ){
		return (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;
	}else if( mode == MODE_DOWNLOAD ){
		return (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;
	}

	GF_ASSERT_MSG_HEAVY(0,"渡すモードを間違っている");
	return NULL;
}

//------------------------------------------------------------------
/**
 * @brief   進化ポケモンをもう一度さっきいれた場所に格納しなおす
 *
 * @param   wk		GTS画面ワーク
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void EvoPokemonUpdate( WORLDTRADE_WORK *wk )
{
	POKEMON_PARAM *pp = RecvPokemonParamPointerGet( wk, wk->sub_process_mode );
	
	if(wk->EvoPokeInfo.boxno==WORLDTRADE_BOX_TEMOTI){
		// てもちの時
		WT_PokeCopyPPtoPP( pp, PokeParty_GetMemberPointer( wk->param->myparty, wk->EvoPokeInfo.pos ) );
	}else{
		int boxno=0, boxpos=0;
		// ボックスの時
		// ボックスに格納したポケモンを一旦消す
		BOXDAT_ClearPokemon( wk->param->mybox, wk->EvoPokeInfo.boxno, wk->EvoPokeInfo.pos );

		// 格納しなおす
		BOXDAT_GetEmptyTrayNumberAndPos( wk->param->mybox, &boxno, &boxpos );
		BOXDAT_PutPokemonBox( wk->param->mybox, boxno, (POKEMON_PASO_PARAM *)PP_GetPPPPointerConst(pp) );
		
	}
}
