//=============================================================================
/**
 *
 *  @file   d_mori.c
 *  @brief  森デバッグメニュー
 *  @author mori 
 *  @data   2009.11.21
 *
 *  このソースはtestmode.cにincludeして使用される。
 *  testmode.cはデバッグ用アプリなのでメモリに積まれにくい（はず）
 *
 */
//=============================================================================
#include "app/waza_oshie.h"

//=============================================================================
/**
 *                定数定義
 */
//=============================================================================

//=============================================================================
/**
 *                構造体定義
 */
//=============================================================================

//=============================================================================
/**
 *              プロトタイプ宣言
 */
//=============================================================================
static BOOL TESTMODE_ITEM_SelectWazaOshie( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMailMake( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMailView( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMailbox( TESTMODE_WORK *work , const int idx );

static TESTMODE_MENU_LIST menuMori[] = 
{
  {L"わざおしえ",TESTMODE_ITEM_SelectWazaOshie },
  {L"メール作成",TESTMODE_ITEM_SelectMailMake },
  {L"メール閲覧",TESTMODE_ITEM_SelectMailView },
  {L"メールボックス",TESTMODE_ITEM_SelectMailbox },
  
  {L"もどる"        ,TESTMODE_ITEM_BackTopMenu },
};

//=============================================================================
/**
 *                外部公開関数
 */
//=============================================================================




//=============================================================================
/**
 *                static関数
 */
//=============================================================================


FS_EXTERN_OVERLAY(waza_oshie);
//----------------------------------------------------------------------------------
/**
 * @brief 技教え呼び出し
 *
 * @param   work    
 * @param   idx   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL TESTMODE_ITEM_SelectWazaOshie( TESTMODE_WORK *work , const int idx )
{
  GAMEDATA *gamedata =  GAMEDATA_Create( GFL_HEAPID_APP );
  SAVE_CONTROL_WORK *sv;
  POKEPARTY *party;
  WAZAOSHIE_DATA *param;
  POKEMON_PARAM *pp = PP_Create( 3, 50, 0, GFL_HEAPID_APP );

  param = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(WAZAOSHIE_DATA) );
  party = GAMEDATA_GetMyPokemon( gamedata );
  sv = GAMEDATA_GetSaveControlWork(gamedata);
  
//  PokeParty_Add(party, pp);

  param->gsys = NULL;
  param->pp   = PokeParty_GetMemberPointer( party, 0 );
  param->myst = GAMEDATA_GetMyStatus( gamedata );   // 自分データ
  param->cfg  = SaveData_GetConfig(sv);     // コンフィグデータ
  param->waza_tbl   = WAZAOSHIE_GetRemaindWaza( param->pp, GFL_HEAPID_APP );


  TESTMODE_COMMAND_ChangeProc( work,FS_OVERLAY_ID(waza_oshie), &WazaOshieProcData, param );

  return TRUE;
}


#include "savedata/mail_util.h"
#include "app/mailtool.h"
//#include "app/mailsys.h"
//オーバーレイID定義
FS_EXTERN_OVERLAY(app_mail);
FS_EXTERN_OVERLAY(pmsinput);
FS_EXTERN_OVERLAY(ui_common);

static void MailSys_SetProc(TESTMODE_WORK *work,MAIL_PARAM* param )
{

  TESTMODE_COMMAND_ChangeProc( work, NO_OVERLAY_ID, &MailSysProcData, param );
}


//----------------------------------------------------------------------------------
/**
 * @brief メール作成画面呼び出し
 *
 * @param   work    
 * @param   idx   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL TESTMODE_ITEM_SelectMailMake( TESTMODE_WORK *work , const int idx )
{
  MAIL_PARAM *param; 
  POKEPARTY *party;
  POKEMON_PARAM *pp = PP_Create( 3, 50, 0, GFL_HEAPID_APP );
  GAMEDATA *gamedata =  GAMEDATA_Create( GFL_HEAPID_APP );
  party = GAMEDATA_GetMyPokemon( gamedata );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(app_mail));

  PokeParty_Add(party, pp);


  
  param = MAILSYS_GetWorkCreate( gamedata, 0,0,0, GFL_HEAPID_APP);


  MailSys_SetProc( work, param );
  return TRUE;
  
}


//----------------------------------------------------------------------------------
/**
 * @brief メール閲覧画面呼び出し
 *
 * @param   work    
 * @param   idx   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL TESTMODE_ITEM_SelectMailView( TESTMODE_WORK *work , const int idx )
{
  MAIL_PARAM *param; 
  GAMEDATA *gamedata =  GAMEDATA_Create( GFL_HEAPID_APP );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(app_mail));

  param = MailSys_GetWorkViewPrev( gamedata,1, GFL_HEAPID_APP);

  MailSys_SetProc( work, param );
  return TRUE;

}

#include "app/mailbox.h"
//----------------------------------------------------------------------------------
/**
 * @brief メールボックス呼び出し
 *
 * @param   work    
 * @param   idx   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL TESTMODE_ITEM_SelectMailbox( TESTMODE_WORK *work , const int idx )
{
  // オーバーレイID宣言
//  FS_EXTERN_OVERLAY(overlay_mailbox);

  GAMEDATA *gamedata  = GAMEDATA_Create( GFL_HEAPID_APP );

  MAILBOX_PARAM * prm = GFL_HEAP_AllocMemory( GFL_HEAPID_APP, sizeof(MAILBOX_PARAM) );
  prm->gamedata  = gamedata;
  
  TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(app_mail), &MailBoxProcData, prm );
  
  return TRUE;

}

