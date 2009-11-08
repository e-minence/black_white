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

#include "msg/msg_poke_trade.h"
#include "ircbattle.naix"
#include "trade.naix"
#include "net_app/connect_anm.h"
#include "net/dwc_rapfriend.h"
#include "savedata/wifilist.h"

#include "system/touch_subwindow.h"

#include "poke_tool/poke_tool_def.h"
#include "box_m_obj_NANR_LBLDEFS.h"
#include "p_st_obj_d_NANR_LBLDEFS.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/status_rcv.h"
#include "tradedemo.naix"

#include "ircpokemontrade_local.h"
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
static void _changeTimingSaveStart(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart2(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart3(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart4(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStart5(POKEMON_TRADE_WORK* pWork);
static void _saveStart(POKEMON_TRADE_WORK* pWork);
static void _mailBoxStart(POKEMON_TRADE_WORK* pWork);

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

//------------------------------------------------------------------
/**
 * @brief   �|�P���������I���B
 * @param   POKEMON_TRADE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------

void POKMEONTRADE_SAVE_Init(POKEMON_TRADE_WORK* pWork)
{
  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  GFL_CLACT_SYS_Delete();
  IRC_POKETRADE_CLACT_Create(pWork);
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 340 , 0 , pWork->heapID );
  POKETRADE_MessageHeapInit(pWork);

  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_49, pWork->pMessageStrBufEx );
  {
    POKEMON_PARAM* pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
    WORDSET_RegisterPokeNickName( pWork->pWordSet, 1,  pp );
  }
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx);

  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME2_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
  IRC_POKETRADE_MessageWindowOpen(pWork);
  _setNextAnim(pWork, 0);

  GFL_BG_SetVisible( GFL_BG_FRAME2_S , TRUE );
  
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade21);

}

static void _changeDemo_ModelTrade21(POKEMON_TRADE_WORK* pWork)
{
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  if(pWork->anmCount < 100){
    return;
  }

  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_50, pWork->pMessageStrBufEx );
  {
    POKEMON_PARAM* pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
    WORDSET_RegisterPokeNickName( pWork->pWordSet, 1,  pp );
  }
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx);
  IRC_POKETRADE_MessageWindowOpen(pWork);
  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade22);

}

static void _changeDemo_ModelTrade22(POKEMON_TRADE_WORK* pWork)
{
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  if(pWork->anmCount < 100){
    return;
  }
  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade23);
}





static void _changeDemo_ModelTrade23(POKEMON_TRADE_WORK* pWork)
{
  IRCPOKETRADE_PokeDeleteMcss(pWork, 0);
  IRCPOKETRADE_PokeDeleteMcss(pWork, 1);
  IRCPOKETRADE_PokeDeleteMcss(pWork, 2);
  IRCPOKETRADE_PokeDeleteMcss(pWork, 3);

//  _pokemonCreateCLACTExit(pWork);
//  POKEMONTRADE_DEMO_PTC_End(pWork->pPokemonTradeDemo,PTC_KIND_NUM_MAX);
//  POKEMONTRADE_DEMO_ICA_Delete(pWork->pPokemonTradeDemo);
//  GFL_HEAP_FreeMemory(pWork->pPokemonTradeDemo);
//  pWork->pPokemonTradeDemo = NULL;

  //@todo �i���f���ɂƂԂ��A���[������


  {
    int id = 1-GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
    STATUS_RCV_PokeParam_RecoverAll(pWork->recvPoke[id]);
    //��������
    // ����̃|�P�������̑I��ł����ꏊ�ɓ����
    if(pWork->selectBoxno == BOXDAT_GetTrayMax(pWork->pBox)){ //�Ă����̌����̏ꍇ
      POKEPARTY* party = pWork->pMyParty;
      PokeParty_SetMemberData(party, pWork->selectIndex, pWork->recvPoke[id]);
    }
    else{
      // ���[������������{�b�N�X��
      _ITEMMARK_ICON_WORK* pIM = &pWork->aItemMark;
      int item = PP_Get( pWork->recvPoke[id] , ID_PARA_item ,NULL);
      if(ITEM_CheckMail(item)){
        GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_08, pWork->pMessageStrBuf );
        IRC_POKETRADE_MessageWindowOpen(pWork);
        _CHANGE_STATE(pWork,_mailBoxStart);
        return;
      }
      else{
        BOXDAT_PutPokemonPos(pWork->pBox, pWork->selectBoxno,
                             pWork->selectIndex, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pWork->recvPoke[id]));
      }
    }
  }
  _CHANGE_STATE(pWork,_saveStart);
}



static void _saveStart(POKEMON_TRADE_WORK* pWork)
{
  if(!pWork->pGameData){
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade30); //�Q�[���f�[�^�̈��n���������ꍇ�Z�[�u�ɍs���Ȃ�
    return;
  }
  else{
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_51, pWork->pMessageStrBuf );
    IRC_POKETRADE_MessageWindowOpen(pWork);
    _CHANGE_STATE(pWork, _changeTimingSaveStart);
  }
}


static void _changeTimingSaveStart(POKEMON_TRADE_WORK* pWork)
{
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  if(GFL_NET_IsInit()){
    GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVEST);
    _CHANGE_STATE(pWork, _changeTimingSaveStart2);
  }
  else{
    GAMEDATA_SaveAsyncStart(pWork->pGameData);
    _CHANGE_STATE(pWork, _changeTimingSaveStart3);
  }

}
static void _changeTimingSaveStart2(POKEMON_TRADE_WORK* pWork)
{
  if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVEST)){
    GAMEDATA_SaveAsyncStart(pWork->pGameData);
    _CHANGE_STATE(pWork,_changeTimingSaveStart3);
  }
}

static void _changeTimingSaveStart3(POKEMON_TRADE_WORK* pWork)
{
  if(GAMEDATA_SaveAsyncMain(pWork->pGameData) == SAVE_RESULT_LAST){
    if(GFL_NET_IsInit()){
      pWork->saveStep = GFUser_GetPublicRand(_CHANGERAND);
      _CHANGE_STATE(pWork,_changeTimingSaveStart4);
    }
    else{
      _CHANGE_STATE(pWork,_changeDemo_ModelTrade25);
    }
  }
}

static void _changeTimingSaveStart4(POKEMON_TRADE_WORK* pWork)
{
  pWork->saveStep--;
  if(pWork->saveStep<0){
    if(GFL_NET_IsInit()){
      GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVELAST);
      _CHANGE_STATE(pWork, _changeTimingSaveStart5);
    }
  }
}

static void _changeTimingSaveStart5(POKEMON_TRADE_WORK* pWork)
{
  if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVELAST)){
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade25);
  }
}

static void _changeDemo_ModelTrade25(POKEMON_TRADE_WORK* pWork)
{
  if(GAMEDATA_SaveAsyncMain(pWork->pGameData) == SAVE_RESULT_OK){
    GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVEEND);
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade26);
  }
}

static void _changeDemo_ModelTrade26(POKEMON_TRADE_WORK* pWork)
{
  if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVEEND)){
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade30);
  }
}

static void _changeDemo_ModelTrade30(POKEMON_TRADE_WORK* pWork)
{
  GFL_DISP_GX_SetVisibleControlDirect( 0 );
  GFL_DISP_GXS_SetVisibleControlDirect( 0 );


  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 340 , 0 , pWork->heapID );

  IRC_POKETRADE_MessageWindowClose(pWork);

//  IRC_POKETRADE_GraphicFreeVram(pWork);

  GFL_BG_FillCharacterRelease(GFL_BG_FRAME2_S,1,0);
  GFL_BG_FreeBGControl(GFL_BG_FRAME2_S);
  GFL_BG_FreeBGControl(GFL_BG_FRAME3_S);

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
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  if(GFL_UI_KEY_GetTrg() || GFL_UI_TP_GetTrg()){
    //�|�P�������i�[
    int id = 1-GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
    BOXDAT_PutPokemonPos(pWork->pBox, pWork->selectBoxno,
                         pWork->selectIndex, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pWork->recvPoke[id]));
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
    IRC_POKETRADE_AppMenuClose(pWork);
    IRC_POKETRADE_MessageWindowClear(pWork);
    pWork->pAppTask=NULL;

    switch(selectno){
    case 0:  //�͂�
      {
        int id = 1-GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
        PP_Put(pWork->recvPoke[id], ID_PARA_item, ITEM_DUMMY_ID);
      }
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_11, pWork->pMessageStrBuf );
      IRC_POKETRADE_MessageWindowOpen(pWork);
      _CHANGE_STATE(pWork,_mailSeqEnd);
      break;
    default: //������
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_08, pWork->pMessageStrBuf );
      IRC_POKETRADE_MessageWindowOpen(pWork);
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
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  {
    int msg[]={POKETRADE_STR_27, POKETRADE_STR_28};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
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
    IRC_POKETRADE_AppMenuClose(pWork);
    IRC_POKETRADE_MessageWindowClear(pWork);
    pWork->pAppTask=NULL;

    switch(selectno){
    case 0:  //�͂�
      //@todo PROC�Ăяo��
      break;
    default: //������
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_09, pWork->pMessageStrBuf );
      IRC_POKETRADE_MessageWindowOpen(pWork);
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
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  {
    int msg[]={POKETRADE_STR_27, POKETRADE_STR_28};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
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
    IRC_POKETRADE_AppMenuClose(pWork);
    IRC_POKETRADE_MessageWindowClear(pWork);
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
          IRC_POKETRADE_MessageWindowOpen(pWork);
          _CHANGE_STATE(pWork,_mailPCArrangement);
        }
        else{
          GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_12, pWork->pMessageStrBuf );
          IRC_POKETRADE_MessageWindowOpen(pWork);
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
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  {
    int msg[]={POKETRADE_STR_27, POKETRADE_STR_28};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
  }
  _CHANGE_STATE(pWork,_mailPCSendYesOrNo);

}





