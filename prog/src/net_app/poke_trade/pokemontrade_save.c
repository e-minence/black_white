//=============================================================================
/**
 * @file	  pokemontrade_save.c
 * @bfief	  �|�P���������Z�[�u����
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/11/08
 */
//=============================================================================

#include <gflib.h>


#include "arc_def.h"
#include "net/network_define.h"
#include "net/net_save.h"
#include "system\wipe.h"

#include "net_app/pokemontrade.h"
#include "system/main.h"

#include "poke_icon.naix"
#include "message.naix"
#include "print/printsys.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"
#include "pokeicon/pokeicon.h"

#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"

#include "demo/shinka_demo.h"
#include "poke_tool/shinka_check.h"

#include "msg/msg_poke_trade.h"
#include "ircbattle.naix"
#include "trade.naix"
#include "net_app/connect_anm.h"
#include "net/dwc_rapfriend.h"
#include "savedata/wifilist.h"
#include "savedata/wifihistory.h"
#include "savedata/wifihistory_local.h"
#include "savedata/undata_update.h"



#include "poke_tool/poke_tool_def.h"
#include "box_m_obj_NANR_LBLDEFS.h"
#include "p_st_obj_d_NANR_LBLDEFS.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/status_rcv.h"
#include "poke_tool/poke_memo.h"
#include "tradedemo.naix"
#include "gamesystem/game_data.h"
#include "savedata/record.h"

#include "pokemontrade_local.h"
#include "app/mailtool.h"

#include "spahead.h"


static void _changeDemo_ModelTrade20(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade21(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade22(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade23(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade24(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade25(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade30(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade26(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade27(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart2(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart3(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart4(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart5(POKEMON_TRADE_WORK* pWork);
static void _saveStart(POKEMON_TRADE_WORK* pWork);
static void _mailBoxStart(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart(POKEMON_TRADE_WORK* pWork);

//------------------------------------------------------------------
/**
 * @brief   ���̃V�[���ɕK�v�Ȓl���Z�b�g
 * @param   POKEMON_TRADE_WORK ���[�N
 * @param   _TRADE_SCENE_NO_E no �V�[���Ǘ�enum
 * @retval  none
 */
//------------------------------------------------------------------

static void _setNextAnim(POKEMON_TRADE_WORK* pWork, int timer)
{
  pWork->anmCount = timer;
}


static void _endBGM(POKEMON_TRADE_WORK* pWork)
{
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );
  PMSND_FadeInBGM( 60 );
}

static void _endME(POKEMON_TRADE_WORK* pWork)
{
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );
  PMSND_FadeInBGM( 24 );
}


//------------------------------------------------------------------
/**
 * @brief   �|�P���������I���B
 * @param   POKEMON_TRADE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------

void POKMEONTRADE_SAVE_Init(POKEMON_TRADE_WORK* pWork)
{
  POKEMON_PARAM* pp;
  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  GFL_CLACT_SYS_Delete();
  IRC_POKETRADE_CLACT_Create(pWork);
  
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 340 , 0 , pWork->heapID );


  if(
    (pWork->type == POKEMONTRADE_TYPE_GTS) ||
    (pWork->type == POKEMONTRADE_TYPE_GTSUP)
    )
  {
    _endBGM(pWork);
    pWork->pParentWork->ret = POKEMONTRADE_MOVE_END;
    _CHANGE_STATE(pWork,POKEMONTRADE_PROC_FadeoutStart);
    return;
  }



  POKETRADE_MESSAGE_HeapInit(pWork);

  PMSND_PauseBGM( TRUE );
  PMSND_PushBGM();
  PMSND_PlayBGM( SEQ_ME_POKEGET );
  PMSND_FadeInBGM( 8 );


  
  if((pWork->type == POKEMONTRADE_TYPE_GTSDOWN) ||
     (pWork->type == POKEMONTRADE_TYPE_GTSMID)){
    GFL_MSG_GetString( pWork->pMsgData, gtsnego_info_20, pWork->pMessageStrBufEx );
  }
  else if(pWork->bEncountMessageEach){
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_30, pWork->pMessageStrBufEx );
  }
  else{
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_49, pWork->pMessageStrBufEx );
  }

  if(POKEMONTRADEPROC_IsTriSelect(pWork))
  {
    pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 0);
  }
  else{
    pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
  }

  WORDSET_RegisterPokeNickName( pWork->pWordSet, 1,  pp );

  {
    WORDSET_RegisterPlayerName( pWork->pWordSet, 0, pWork->pFriend  );
  }
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx);

  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME2_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
  POKETRADE_MESSAGE_WindowOpen(pWork);
  _setNextAnim(pWork, 0);
  POKETRADE_MESSAGE_ChangeStreamType(pWork,APP_PRINTSYS_COMMON_TYPE_THROUGH);

  GFL_BG_SetVisible( GFL_BG_FRAME2_S , TRUE );



  _CHANGE_STATE(pWork,_changeDemo_ModelTrade22);

}

static void _changeDemo_ModelTrade21(POKEMON_TRADE_WORK* pWork)
{
  POKEMON_PARAM* pp;
  
  if(!POKETRADE_MESSAGE_EndCheck(pWork)){
    return;
  }
  if(pWork->anmCount < 400){
    return;
  }

  if(POKEMONTRADEPROC_IsTriSelect(pWork))
  {
    pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 0);
  }
  else{
    pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
  }


  WORDSET_RegisterPokeNickName( pWork->pWordSet, 1,  pp );
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx);
  POKETRADE_MESSAGE_WindowOpen(pWork);
  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade22);

}

static void _changeDemo_ModelTrade22(POKEMON_TRADE_WORK* pWork)
{
  if(!POKETRADE_MESSAGE_EndCheck(pWork)){
    return;
  }
  if(pWork->anmCount < 400){
    return;
  }
  POKETRADE_MESSAGE_ChangeStreamType(pWork,APP_PRINTSYS_COMMON_TYPE_KEY);

  _endME(pWork);

  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade23);
}


static void _UnDataSend2(POKEMON_TRADE_WORK* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),
                               _TIMING_UNDATA,WB_NET_TRADE_SERVICEID)){
    _CHANGE_STATE(pWork, _changeDemo_ModelTrade27);

  }
}

static void _UnDataSend(POKEMON_TRADE_WORK* pWork)
{
  WIFI_HISTORY* pWH = SaveData_GetWifiHistory(GAMEDATA_GetSaveControlWork(pWork->pGameData));
  UNITEDNATIONS_SAVE add_data;
  POKEMON_PARAM* ppr;
  POKEMON_PARAM* pps;

  if(POKEMONTRADEPROC_IsTriSelect(pWork)){
    ppr = IRC_POKEMONTRADE_GetRecvPP(pWork, 0);
    pps = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
  }
  else{
    ppr = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
    pps = IRC_POKEMONTRADE_GetRecvPP(pWork, 0);
  }
  
  MyStatus_Copy(GAMEDATA_GetMyStatus(pWork->pGameData) , &add_data.aMyStatus);
  add_data.recvPokemon = PP_Get( ppr , ID_PARA_monsno ,NULL) ;  //������|�P����
  add_data.sendPokemon = PP_Get( pps , ID_PARA_monsno ,NULL);  //�������|�P����
  add_data.favorite = WIFIHISTORY_GetMyFavorite(pWH);   //�
  add_data.countryCount = WIFIHISTORY_GetMyCountryCount(pWH) ;  //�����������̉�
  add_data.nature =WIFIHISTORY_GetMyNature(pWH);   //���i


  if(POKEMONTRADEPROC_IsNetworkMode(pWork)){
    if( GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_UN, sizeof(add_data), &add_data)){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_UNDATA, WB_NET_TRADE_SERVICEID);
      _CHANGE_STATE(pWork, _UnDataSend2);
    }
  }
  else{
    _CHANGE_STATE(pWork, _changeDemo_ModelTrade30);
  }
}

static void _setPokemonData(POKEMON_TRADE_WORK* pWork)
{

  {
    POKEMON_PARAM* pp=PokeParty_GetMemberPointer( pWork->pParentWork->pParty, 0 );
    RECORD* pRec = GAMEDATA_GetRecordPtr(pWork->pGameData);
    
    STATUS_RCV_PokeParam_RecoverAll(pp); //��
    //�|�P����
    POKE_MEMO_SetTrainerMemoPP( pp, POKE_MEMO_EGG_TRADE,
                                pWork->pMy, 
                                POKE_MEMO_PLACE_HUMAN_TRADE, pWork->heapID );

    
    switch(pWork->type){
    case POKEMONTRADE_TYPE_GTSNEGO:
    case POKEMONTRADE_TYPE_WIFICLUB:
      RECORD_Inc(pRec, RECID_WIFI_TRADE);///< @WiFi�ʐM������������
      break;
    case POKEMONTRADE_TYPE_UNION:
      RECORD_Inc(pRec, RECID_COMM_TRADE); ///< @���C�����X�ʐM������������
      break;
    case POKEMONTRADE_TYPE_IRC:
      RECORD_Inc(pRec, RECID_IRC_TRADE);
      break;
    }

    //���A
    
    
    //������
    ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave( pWork->pGameData ), pp );
    //�������������
    if(pWork->type == POKEMONTRADE_TYPE_GTSNEGO){
      WIFI_NEGOTIATION_SV_AddChangeCount(GAMEDATA_GetWifiNegotiation(pWork->pGameData));
    }
    //��������   // ����̃|�P�������̑I��ł����ꏊ�ɓ����
    if(pWork->pParentWork->selectBoxno == BOXDAT_GetTrayMax(pWork->pBox)){ //�Ă����̌����̏ꍇ
      POKEPARTY* party = pWork->pMyParty;
      PokeParty_SetMemberData(party, pWork->pParentWork->selectIndex, pp);
    }
    else{
      if(MONSNO_492 == PP_Get( pp , ID_PARA_monsno ,NULL)){
        if(FORMNO_492_SKY == PP_Get( pp , ID_PARA_form_no ,NULL)){
          PP_ChangeFormNo(pp,FORMNO_SHEIMI_LAND);
        }
      }
      BOXDAT_PutPokemonPos(pWork->pBox, pWork->pParentWork->selectBoxno,
                           pWork->pParentWork->selectIndex,
                           (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pp));
    }
  }
}





static void _changeDemo_ModelTrade23(POKEMON_TRADE_WORK* pWork)
{
  IRCPOKETRADE_PokeDeleteMcss(pWork, 0);
  IRCPOKETRADE_PokeDeleteMcss(pWork, 1);
  IRCPOKETRADE_PokeDeleteMcss(pWork, 2);
  IRCPOKETRADE_PokeDeleteMcss(pWork, 3);



  if(
    (pWork->type == POKEMONTRADE_TYPE_EVENT)||
    (pWork->type == POKEMONTRADE_TYPE_GTSMID) ||
    (pWork->type == POKEMONTRADE_TYPE_GTSDOWN)
    )
  {
    pWork->pParentWork->ret = POKEMONTRADE_MOVE_END;
    
    _endBGM(pWork);
    _CHANGE_STATE(pWork,POKEMONTRADE_PROC_FadeoutStart);
    return;
  }

  if(pWork->type == POKEMONTRADE_TYPE_GTSNEGO){
    WIFI_NEGOTIATION_SV_SetFriend(GAMEDATA_GetWifiNegotiation(pWork->pGameData),
                                  pWork->pFriend);
  }
  if(
    (pWork->type == POKEMONTRADE_TYPE_GTSNEGO)||
    (pWork->type == POKEMONTRADE_TYPE_UNION)
    )
  {
    _CHANGE_STATE(pWork, _UnDataSend);
  }
  else{
    _CHANGE_STATE(pWork, _changeDemo_ModelTrade27);
  }
}

static void _changeDemo_ModelTrade27(POKEMON_TRADE_WORK* pWork)
{



  
  {   //�������炢�ł�PROCCHANGE���Ă��ǂ��悤�ɐe���[�N�Ɍ��������i�[
    POKEMON_PARAM* pp;// = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);

    if(POKEMONTRADEPROC_IsTriSelect(pWork))
    {
      int no = pWork->pokemonselectno % 3;
      pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 0);
      pWork->pParentWork->selectBoxno = pWork->GTSSelectBoxno[0][no];
      pWork->pParentWork->selectIndex = pWork->GTSSelectIndex[0][no];
      NET_PRINT("�|�P����NO%d BOX%d INDEX%d SEL%d\n",
                PP_Get(pp,ID_PARA_monsno,NULL),
                pWork->pParentWork->selectBoxno,
                pWork->pParentWork->selectIndex,pWork->pokemonselectno);
    }
    else{
      pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
      pWork->pParentWork->selectBoxno = pWork->selectBoxno;
      pWork->pParentWork->selectIndex = pWork->selectIndex;
    }

    GF_ASSERT(pWork->pParentWork->pParty);
    
    PokeParty_Init( pWork->pParentWork->pParty, TEMOTI_POKEMAX );
    PokeParty_Add( pWork->pParentWork->pParty, pp );
  }


  // ���[������������{�b�N�X��
  {
    POKEMON_PARAM* pp=PokeParty_GetMemberPointer( pWork->pParentWork->pParty, 0 );
    _ITEMMARK_ICON_WORK* pIM = &pWork->aItemMark;
    int item = PP_Get( pp , ID_PARA_item ,NULL);
    if(ITEM_CheckMail(item)){
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_08, pWork->pMessageStrBuf );
      POKETRADE_MESSAGE_WindowOpen(pWork);
      _CHANGE_STATE(pWork,_mailBoxStart);
      return;
    }
  }
  //�|�P�����Z�b�g
  _setPokemonData(pWork);


  _CHANGE_STATE(pWork,_saveStart);
}



static void _saveStart(POKEMON_TRADE_WORK* pWork)
{
  if(!pWork->pGameData){
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade30); //�Q�[���f�[�^�̈��n���������ꍇ�Z�[�u�ɍs���Ȃ�
    return;
  }
  else{
    if(pWork->pParentWork){
      SHINKA_COND cond;
      POKEMON_PARAM* pp;
      POKEMON_PARAM* pp2;
      u16 after_mons_no;

      if(POKEMONTRADEPROC_IsTriSelect(pWork))
      {
        pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 0);
        pp2 = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
      }
      else{
        pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
        pp2 = IRC_POKEMONTRADE_GetRecvPP(pWork, 0);
      }
      
   //   POKEMON_PARAM* pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
    
      after_mons_no = SHINKA_Check( NULL, pp, SHINKA_TYPE_TUUSHIN, (u32)pp2, &cond, pWork->heapID );
      if( after_mons_no ){
        pWork->pParentWork->ret = POKEMONTRADE_MOVE_EVOLUTION;
        pWork->pParentWork->after_mons_no = after_mons_no;
        pWork->pParentWork->cond = cond;

//        PokeParty_Init( pWork->pParentWork->pParty, TEMOTI_POKEMAX );
//        PokeParty_Add( pWork->pParentWork->pParty, pp );
        _CHANGE_STATE(pWork,POKEMONTRADE_PROC_FadeoutStart);   //
        return ;
      }
    }
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_51, pWork->pMessageStrBuf );
    POKETRADE_MESSAGE_WindowOpen(pWork);
    POKETRADE_MESSAGE_WindowTimeIconStart(pWork);
    _CHANGE_STATE(pWork, _changeTimingSaveStart);
  }
}



//-------------------------------------------------
/**
 *	@brief         �i���^�C�~���O�X�^�[�g  ���[����̖߂蕔��
 *	@param[inout]	 _POKEMCSS_MOVE_WORK ���[�N
 */
//-------------------------------------------------

void POKMEONTRADE_EVOLUTION_TimingStart(POKEMON_TRADE_WORK* pWork)
{

  POKETRADE_MESSAGE_HeapInit(pWork);
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_51, pWork->pMessageStrBuf );
  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME2_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
  POKETRADE_MESSAGE_WindowOpen(pWork);
  POKETRADE_MESSAGE_WindowTimeIconStart(pWork);
  _setNextAnim(pWork, 0);

  GFL_BG_SetVisible( GFL_BG_FRAME2_S , TRUE );
  GFL_NET_WirelessIconEasy_HoldLCD(TRUE,pWork->heapID); //�ʐM�A�C�R��
  GFL_NET_ReloadIcon();

  {
    POKEMON_PARAM* pp = PokeParty_GetMemberPointer( pWork->pParentWork->pParty, 0 );
    GF_ASSERT(MAILDATA_NULLID != MailSys_MoveMailPoke2Paso(pWork->pMailBlock, pp, pWork->heapID));
  }
  
  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, _BRIGHTNESS_SYNC);

  _CHANGE_STATE(pWork,_changeTimingSaveStart);
}



//-------------------------------------------------
/**
 *	@brief         �Z�[�u�^�C�~���O�X�^�[�g  �i����̖߂蕔��
 *	@param[inout]	 _POKEMCSS_MOVE_WORK ���[�N
 */
//-------------------------------------------------

void POKMEONTRADE_SAVE_TimingStart(POKEMON_TRADE_WORK* pWork)
{

//  pWork->cellUnit = GFL_CLACT_UNIT_Create( 340 , 0 , pWork->heapID );
  POKETRADE_MESSAGE_HeapInit(pWork);
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_51, pWork->pMessageStrBuf );
  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME2_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
  POKETRADE_MESSAGE_WindowOpen(pWork);
  POKETRADE_MESSAGE_WindowTimeIconStart(pWork);
  _setNextAnim(pWork, 0);

  GFL_BG_SetVisible( GFL_BG_FRAME2_S , TRUE );
  GFL_NET_WirelessIconEasy_HoldLCD(TRUE,pWork->heapID); //�ʐM�A�C�R��
  GFL_NET_ReloadIcon();

  //�߂��Ă����|�P�����Ɠ���ւ�
 // pWork->selectBoxno = pWork->pParentWork->selectBoxno;
//  pWork->selectIndex = pWork->pParentWork->selectIndex;

  _setPokemonData(pWork);

  
  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, _BRIGHTNESS_SYNC);

  _CHANGE_STATE(pWork,_changeTimingSaveStart);
}

static void _changeTimingSaveStart(POKEMON_TRADE_WORK* pWork)
{  
  if(!POKETRADE_MESSAGE_EndCheck(pWork)){
    return;
  }
  if(POKEMONTRADEPROC_IsNetworkMode(pWork)){
    pWork->pNetSave = NET_SAVE_Init(pWork->heapID, pWork->pGameData);
    _CHANGE_STATE(pWork, _changeTimingSaveStart2);
  }
  else{
    _CHANGE_STATE(pWork, _changeDemo_ModelTrade30);
  }

}
static void _changeTimingSaveStart2(POKEMON_TRADE_WORK* pWork)
{
  if(NET_SAVE_Main(pWork->pNetSave)){
    NET_SAVE_Exit(pWork->pNetSave);
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade30);
  }
}

static void _changeDemo_ModelTrade30(POKEMON_TRADE_WORK* pWork)
{
  _endBGM(pWork);

  GFL_DISP_GX_SetVisibleControlDirect( 0 );
  GFL_DISP_GXS_SetVisibleControlDirect( 0 );


  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 340 , 0 , pWork->heapID );

  POKETRADE_MESSAGE_WindowClose(pWork);

  IRC_POKETRADE_ResetSubDispGraphic(pWork);
  IRC_POKETRADE_ResetSubdispGraphicDemo(pWork);

  IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE_NORMAL);

  IRC_POKETRADE_SetSubVram(pWork);

  IRC_POKETRADE_InitBoxCursor(pWork);  // �^�X�N�o�[
  IRC_POKETRADE_CreatePokeIconResource(pWork);  // �|�P�����A�C�R��CLACT+���\�[�X�풓��

  IRC_POKETRADE_GraphicInitMainDisp(pWork);
  IRC_POKETRADE_GraphicInitSubDisp(pWork);

  IRC_POKETRADEDEMO_SetModel( pWork, REEL_PANEL_OBJECT);

  GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  POKETRADE_TOUCHBAR_Init(pWork);
  GFL_NET_ReloadIcon();
  _CHANGE_STATE(pWork,IRC_POKMEONTRADE_ChangeFinish);

}




//-------------------------------------------------
/**
 *	@brief      ���[�����p�\�R���ɓ��ꂽ > �Z�[�u�Ɍ�����
 *	@param[inout]	POKEMON_TRADE_WORK ���[�N
 */
//-------------------------------------------------

static void _mailSeqEnd(POKEMON_TRADE_WORK* pWork)
{
  if(!POKETRADE_MESSAGE_EndCheck(pWork)){
    return;
  }
  if(GFL_UI_KEY_GetTrg() || GFL_UI_TP_GetTrg()){


    //�|�P�����Z�b�g
    _setPokemonData(pWork);

    _CHANGE_STATE(pWork,_saveStart);  //�Z�[�u�����ɍs��
  }
}


//-------------------------------------------------
/**
 *	@brief    ���[�����̂Ă�m�F
 *	@param[inout]	_POKEMCSS_MOVE_WORK ���[�N
 */
//-------------------------------------------------

static void _mailTrashYesOrNo(POKEMON_TRADE_WORK* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    POKETRADE_MESSAGE_AppMenuClose(pWork);
    POKETRADE_MESSAGE_WindowClear(pWork);
    pWork->pAppTask=NULL;

    switch(selectno){
    case 0:  //�͂�
      {
        int id = 1-GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
        PP_Put(pWork->recvPoke[id], ID_PARA_item, ITEM_DUMMY_ID);
      }
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_11, pWork->pMessageStrBuf );
      POKETRADE_MESSAGE_WindowOpen(pWork);
      _CHANGE_STATE(pWork,_mailSeqEnd);
      break;
    default: //������
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_08, pWork->pMessageStrBuf );
      POKETRADE_MESSAGE_WindowOpen(pWork);
      _CHANGE_STATE(pWork,_mailBoxStart);
      break;
    }
  }
}


//-------------------------------------------------
/**
 *	@brief    ���[�����̂Ă�m�F
 *	@param[inout]	_POKEMCSS_MOVE_WORK ���[�N
 */
//-------------------------------------------------

static void _mailTrash(POKEMON_TRADE_WORK* pWork)
{
  if(!POKETRADE_MESSAGE_EndCheck(pWork)){
    return;
  }
  {
    int msg[]={POKETRADE_STR_27, POKETRADE_STR_28};
    POKETRADE_MESSAGE_AppMenuOpen(pWork,msg,elementof(msg));
  }
  _CHANGE_STATE(pWork,_mailTrashYesOrNo);
}


//-------------------------------------------------
/**
 *	@brief    �p�\�R���̐����m�F
 *	@param[inout]	_POKEMCSS_MOVE_WORK ���[�N
 */
//-------------------------------------------------

static void _mailPCArrangementYesOrNo(POKEMON_TRADE_WORK* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    POKETRADE_MESSAGE_AppMenuClose(pWork);
    POKETRADE_MESSAGE_WindowClear(pWork);
    pWork->pAppTask=NULL;

    switch(selectno){
    case 0:  //�͂�
      pWork->pParentWork->ret = POKEMONTRADE_MOVE_MAIL;
      _CHANGE_STATE(pWork,POKEMONTRADE_PROC_FadeoutStart);   //�����f���ɍs��
      break;
    default: //������
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_09, pWork->pMessageStrBuf );
      POKETRADE_MESSAGE_WindowOpen(pWork);
      _CHANGE_STATE(pWork,_mailTrash);
      break;
    }
  }
}



//-------------------------------------------------
/**
 *	@brief    �p�\�R���̐����m�F
 *	@param[inout]	_POKEMCSS_MOVE_WORK ���[�N
 */
//-------------------------------------------------

static void _mailPCArrangement(POKEMON_TRADE_WORK* pWork)
{
  if(!POKETRADE_MESSAGE_EndCheck(pWork)){
    return;
  }
  {
    int msg[]={POKETRADE_STR_27, POKETRADE_STR_28};
    POKETRADE_MESSAGE_AppMenuOpen(pWork,msg,elementof(msg));
  }
  _CHANGE_STATE(pWork,_mailPCArrangementYesOrNo);
}

//-------------------------------------------------
/**
 *	@brief    �p�\�R���ɂ����邩�ǂ��� �p�\�R���������ς��Ȃ炳��ɕ���
 *	@param[inout]	_POKEMCSS_MOVE_WORK ���[�N
 */
//-------------------------------------------------

static void _mailPCSendYesOrNo(POKEMON_TRADE_WORK* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    POKETRADE_MESSAGE_AppMenuClose(pWork);
    POKETRADE_MESSAGE_WindowClear(pWork);
    pWork->pAppTask=NULL;

    switch(selectno){
    case 0:  //�͂�
      {
        int friendID = 1-GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
        int ret = MailSys_MoveMailPoke2Paso(pWork->pMailBlock, pWork->recvPoke[friendID],pWork->heapID);
#if DEBUG_ONLY_FOR_ohno
        if(1){ //����Ȃ�����
#else
        if(MAILDATA_NULLID==ret){ //����Ȃ�����
#endif
          GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_10, pWork->pMessageStrBuf );
          POKETRADE_MESSAGE_WindowOpen(pWork);
          _CHANGE_STATE(pWork,_mailPCArrangement);
        }
        else{
          GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_12, pWork->pMessageStrBuf );
          POKETRADE_MESSAGE_WindowOpen(pWork);
          _CHANGE_STATE(pWork,_mailSeqEnd);
        }
      }
      break;
    default: //������
      //@todo
      break;
    }
  }
}

//-------------------------------------------------
/**
 *	@brief      ���[���̊J�n
 *	@param[inout]	POKEMON_TRADE_WORK ���[�N
 */
//-------------------------------------------------

static void _mailBoxStart(POKEMON_TRADE_WORK* pWork)
{
  if(!POKETRADE_MESSAGE_EndCheck(pWork)){
    return;
  }
  {
    int msg[]={POKETRADE_STR_27, POKETRADE_STR_28};
    POKETRADE_MESSAGE_AppMenuOpen(pWork,msg,elementof(msg));
  }
  _CHANGE_STATE(pWork,_mailPCSendYesOrNo);

}





