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
#include "../comm_wifihistory.h"



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
#include "savedata/perapvoice.h"

static void _changeDemo_ModelTrade20(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade21(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade22_1(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade22(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade23(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade24(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade25(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade31(POKEMON_TRADE_WORK* pWork);
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

static void _changeTimingSaveStartT1(POKEMON_TRADE_WORK* pWork);
static void _changeTimingSaveStartT2(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTradeMeWait(POKEMON_TRADE_WORK* pWork);



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
  if(pWork->pushSound != 0){
    PMSND_PopBGM();
    pWork->pushSound--;
    NET_PRINT("pWork->pushSound%d\n",pWork->pushSound);
    PMSND_PauseBGM( FALSE );
    PMSND_FadeInBGM( 60 );
  }
}

static void _endBGM_NoStart(POKEMON_TRADE_WORK* pWork)
{
  PMSND_PopBGM();
  pWork->pushSound--;
  NET_PRINT("pWork->pushSound%d\n",pWork->pushSound);
//  PMSND_PauseBGM( FALSE );
//  PMSND_FadeInBGM( 60 );
}





static void _endME(POKEMON_TRADE_WORK* pWork)
{
  PMSND_PopBGM();
  pWork->pushSound--;
  NET_PRINT("pWork->pushSound%d\n",pWork->pushSound);
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


  //GTS��GTSUP�ŏ�������BGTSUP�̓t�F�[�h���Ă���̂ł����Ńt�F�[�h���Ȃ�
  //Ariizumi100629
  if(pWork->type == POKEMONTRADE_TYPE_GTS)
  {
    _endBGM(pWork);
    pWork->pParentWork->ret = POKEMONTRADE_MOVE_END;
    _CHANGE_STATE(pWork,POKEMONTRADE_PROC_FadeoutStart);
    return;
  }
  if(pWork->type == POKEMONTRADE_TYPE_GTSUP)
  {
    _endBGM(pWork);
    pWork->pParentWork->ret = POKEMONTRADE_MOVE_END;
    _CHANGE_STATE(pWork, NULL);        // �I���
    return;
  }


  POKETRADE_MESSAGE_HeapInit(pWork);

  PMSND_PauseBGM( TRUE );
  PMSND_PushBGM();
  pWork->pushSound++;
  NET_PRINT("pWork->pushSound%d\n",pWork->pushSound);
  PMSND_FadeOutBGM( PMSND_FADE_FAST );

  pWork->anmCount = 0;
  _CHANGE_STATE(pWork,_changeDemo_ModelTradeMeWait);
}

//�������̂����
static void _changeDemo_ModelTradeMeWait(POKEMON_TRADE_WORK* pWork)
{
  POKEMON_PARAM* pp;
  
  if(pWork->anmCount < PMSND_FADE_FAST){
    return;
  }

  PMSND_PlayBGM( SEQ_ME_POKEGET );
  PMSND_FadeInBGM( PMSND_FADE_FAST );

  if(pWork->type == POKEMONTRADE_TYPE_GTSDOWN){
    GFL_MSG_GetString( pWork->pMsgData, gtsnego_info_20, pWork->pMessageStrBufEx );
  }
  else if(pWork->type == POKEMONTRADE_TYPE_GTSMID){
    GFL_MSG_GetString( pWork->pMsgData, gtsnego_info_23, pWork->pMessageStrBufEx );
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
  //POKETRADE_MESSAGE_WindowOpen(pWork);
  POKETRADE_MESSAGE_WindowOpenCustom(pWork, TRUE, FALSE);
  _setNextAnim(pWork, 0);

  GFL_BG_SetVisible( GFL_BG_FRAME2_S , TRUE );
  pWork->anmCount=0;
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade21);

}

//�����Ŏ��̃��b�Z�[�W���o��
static void _changeDemo_ModelTrade21(POKEMON_TRADE_WORK* pWork)
{
  POKEMON_PARAM* pp;
  if(pWork->anmCount < 200){
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
  {
    WORDSET_RegisterPlayerName( pWork->pWordSet, 0, pWork->pFriend  );
  }

  if(pWork->type == POKEMONTRADE_TYPE_GTSDOWN){
  }
  else if(pWork->type == POKEMONTRADE_TYPE_GTSMID){
  }
  else if(pWork->bEncountMessageEach){
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_34, pWork->pMessageStrBufEx );
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx);
    POKETRADE_MESSAGE_WindowOpenCustom(pWork, TRUE, FALSE);
  }
  else{
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_50, pWork->pMessageStrBufEx );
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx);
    POKETRADE_MESSAGE_WindowOpenCustom(pWork, TRUE, FALSE);
  }

  _CHANGE_STATE(pWork,_changeDemo_ModelTrade22_1);
}



static void _changeDemo_ModelTrade22_1(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->anmCount < 270){
    return;
  }
  _endME(pWork);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade22);
}

static void _changeDemo_ModelTrade22(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->anmCount < 420){
    return;
  }
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




//=============================================================================================
/**
 * @brief  ���[���f�[�^�u���b�N���o�b�N�A�b�v����  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  MAIL_BLOCK*   
 */
//=============================================================================================
inline MAIL_BLOCK* MAIL_BackupMailBlock( GAMEDATA *gamedata,HEAPID heapID )
{
  MAIL_BLOCK* pBk = GFL_HEAP_AllocMemory(heapID, MAIL_GetBlockWorkSize() );
  GFL_STD_MemCopy(GAMEDATA_GetMailBlock( gamedata ), pBk, MAIL_GetBlockWorkSize());
  return pBk;
}

//=============================================================================================
/**
 * @brief  ���[���f�[�^�u���b�N���o�b�N�A�b�v������ǂ�  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  MAIL_BLOCK*   
 */
//=============================================================================================
inline void MAIL_RestoreMailBlock( GAMEDATA *gamedata, MAIL_BLOCK* backup )
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork( gamedata );
  MAIL_BLOCK* pOrg = GAMEDATA_GetMailBlock( gamedata );
  GFL_STD_MemCopy(backup, pOrg, MAIL_GetBlockWorkSize());
  GFL_HEAP_FreeMemory(backup);
}



//=============================================================================================
/**
 * @brief  ���R�[�h�u���b�N���o�b�N�A�b�v����  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  MAIL_BLOCK*   
 */
//=============================================================================================
inline RECORD* RECORD_BackupBlock( GAMEDATA *gamedata,HEAPID heapID )
{
  RECORD* pBk = GFL_HEAP_AllocMemory(heapID, RECORD_GetWorkSize() );
  GFL_STD_MemCopy(GAMEDATA_GetRecordPtr( gamedata ), pBk, RECORD_GetWorkSize());
  return pBk;
}

//=============================================================================================
/**
 * @brief  ���R�[�h�u���b�N���o�b�N�A�b�v������ǂ�  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  MAIL_BLOCK*   
 */
//=============================================================================================
inline void RECORD_RestoreBlock( GAMEDATA *gamedata, RECORD* backup )
{
  RECORD* pOrg = GAMEDATA_GetRecordPtr( gamedata );
  GFL_STD_MemCopy(backup, pOrg, RECORD_GetWorkSize());
  GFL_HEAP_FreeMemory(backup);
}


//=============================================================================================
/**
 * @brief  WiFi�ʐM�������o�b�N�A�b�v����  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  WIFI_HISTORY*   
 */
//=============================================================================================
static WIFI_HISTORY* WIFIHISTORY_BackupBlock( GAMEDATA *gamedata,HEAPID heapID )
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork( gamedata );
  WIFI_HISTORY* pBk = GFL_HEAP_AllocMemory(heapID, WIFIHISTORY_GetWorkSize() );
  GFL_STD_MemCopy(SaveData_GetWifiHistory( sv ), pBk, WIFIHISTORY_GetWorkSize());
  return pBk;
}

//=============================================================================================
/**
 * @brief  WiFi�ʐM�������o�b�N�A�b�v������ǂ�  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  none
 */
//=============================================================================================
static void WIFIHISTORY_RestoreBlock( GAMEDATA *gamedata, WIFI_HISTORY* backup )
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork( gamedata );
  WIFI_HISTORY* pOrg = SaveData_GetWifiHistory( sv );
  GFL_STD_MemCopy(backup, pOrg, WIFIHISTORY_GetWorkSize());
  GFL_HEAP_FreeMemory(backup);
}

//=============================================================================================
/**
 * @brief  �}�ӃZ�[�u�f�[�^���o�b�N�A�b�v����  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  ZUKAN_SAVEDATA*   
 */
//=============================================================================================
static ZUKAN_SAVEDATA* ZUKAN_SAVEDATA_BackupBlock( GAMEDATA *gamedata,HEAPID heapID )
{
  ZUKAN_SAVEDATA* pBk = GFL_HEAP_AllocMemory(heapID, ZUKANSAVE_GetWorkSize() );
  GFL_STD_MemCopy(GAMEDATA_GetZukanSave( gamedata ), pBk, ZUKANSAVE_GetWorkSize());
  return pBk;
}

//=============================================================================================
/**
 * @brief  �}�ӃZ�[�u�f�[�^���o�b�N�A�b�v������ǂ�  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  none
 */
//=============================================================================================
static void ZUKAN_SAVEDATA_RestoreBlock( GAMEDATA *gamedata, ZUKAN_SAVEDATA* backup )
{
  ZUKAN_SAVEDATA* pOrg = GAMEDATA_GetZukanSave( gamedata );
  GFL_STD_MemCopy(backup, pOrg, ZUKANSAVE_GetWorkSize());
  GFL_HEAP_FreeMemory(backup);
}


//=============================================================================================
/**
 * @brief  �l�S�V�G�[�V�����Z�[�u�f�[�^���o�b�N�A�b�v����  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  ZUKAN_SAVEDATA*   
 */
//=============================================================================================
static WIFI_NEGOTIATION_SAVEDATA* WIFI_NEGOTIATION_SV_BackupBlock( GAMEDATA *gamedata,HEAPID heapID )
{
  WIFI_NEGOTIATION_SAVEDATA* pBk = GFL_HEAP_AllocMemory(heapID, WIFI_NEGOTIATION_SV_GetWorkSize() );
  GFL_STD_MemCopy(GAMEDATA_GetWifiNegotiation( gamedata ), pBk, WIFI_NEGOTIATION_SV_GetWorkSize());
  return pBk;
}

//=============================================================================================
/**
 * @brief  �l�S�V�G�[�V�����Z�[�u�f�[�^���o�b�N�A�b�v������ǂ�  �|�P�������������߂��p
 * @param   gamedata    
 * @retval  none
 */
//=============================================================================================
static void WIFI_NEGOTIATION_SV_RestoreBlock( GAMEDATA *gamedata, WIFI_NEGOTIATION_SAVEDATA* backup )
{
  WIFI_NEGOTIATION_SAVEDATA* pOrg = GAMEDATA_GetWifiNegotiation( gamedata );
  GFL_STD_MemCopy(backup, pOrg, WIFI_NEGOTIATION_SV_GetWorkSize());
  GFL_HEAP_FreeMemory(backup);
}

//�Ō�Z�[�u�ʉߎ�O�Ŕj��
static void _lastCallback(void* pW)
{
  POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)pW;

  if(pWork->aBackup.pMail){
    GFL_HEAP_FreeMemory(pWork->aBackup.pMail);
  }
  if(pWork->aBackup.pRecord){
    GFL_HEAP_FreeMemory(pWork->aBackup.pRecord);
  }
  if(pWork->aBackup.pWifiHis){
    GFL_HEAP_FreeMemory(pWork->aBackup.pWifiHis);
  }
  if(pWork->aBackup.pZukan){
    GFL_HEAP_FreeMemory(pWork->aBackup.pZukan);
  }
  if(pWork->aBackup.pNego){
    GFL_HEAP_FreeMemory(pWork->aBackup.pNego);
  }
  if(pWork->aBackup.pPokeParty){
    GFL_HEAP_FreeMemory(pWork->aBackup.pPokeParty);
  }
  if(pWork->aBackup.pBoxTray){
    GFL_HEAP_FreeMemory(pWork->aBackup.pBoxTray);
  }
  GFL_STD_MemClear(&pWork->aBackup,sizeof(SAVEREV_BACKUP));

  pWork->bBackupStart = FALSE;
  //  OS_TPrintf("�j��callback\n");
}

// �����f�[�^���c���Ă���ꍇ�G���[�I�������犪���߂�
static void _removeCallback(void* pW)
{
  POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)pW;

  if(pWork->aBackup.pMail){
    MAIL_RestoreMailBlock(pWork->pGameData, pWork->aBackup.pMail);
  }
  if(pWork->aBackup.pRecord){
    RECORD_RestoreBlock(pWork->pGameData, pWork->aBackup.pRecord);
  }
  if(pWork->aBackup.pWifiHis){
    WIFIHISTORY_RestoreBlock(pWork->pGameData, pWork->aBackup.pWifiHis);
  }
  if(pWork->aBackup.pZukan){
    ZUKAN_SAVEDATA_RestoreBlock(pWork->pGameData, pWork->aBackup.pZukan);
  }
  if(pWork->aBackup.pNego){
    WIFI_NEGOTIATION_SV_RestoreBlock(pWork->pGameData, pWork->aBackup.pNego);
  }
  if(pWork->aBackup.pPokeParty){
    PokeParty_Copy(pWork->aBackup.pPokeParty, pWork->pMyParty );
    //�v���C�^�C�����Ԏ؂�
    PLAYTIME_SetSaveDate( GAMEDATA_GetPlayTimeWork(pWork->pGameData), &pWork->aBackup.date );
    GFL_HEAP_FreeMemory(pWork->aBackup.pPokeParty);
    pWork->aBackup.pPokeParty=NULL;
  }
  if(pWork->aBackup.pBoxTray){
    GFL_STD_MemCopy(pWork->aBackup.pBoxTray,
                    BOXTRAYDAT_GetTrayData(pWork->pBox, pWork->pParentWork->selectBoxno),
                    BOXTRAYDAT_GetTotalSize());
  }
  if(pWork->aBackup.bVoice){
    PERAPVOICE_SetExistFlag(GAMEDATA_GetPerapVoice(pWork->pGameData));
  }
  GFL_STD_MemClear(&pWork->aBackup,sizeof(SAVEREV_BACKUP));


  // OS_TPrintf("�����߂�\n");
}


//������p�؃��b�v�{�C�X����
static BOOL _deletePerapVoice(POKEMON_TRADE_WORK* pWork, const POKEPARTY * party,int changepos)
{
  int num, count=0,i;
  PERAPVOICE* pVoice = GAMEDATA_GetPerapVoice(pWork->pGameData);

  if(!PERAPVOICE_GetExistFlag(pVoice)){
    NET_PRINT("�����Ȃ�\n");
    return FALSE;
  }
  // �莝���p�[�e�B�Ƀy���b�v�����邩�`�F�b�N����
  num = PokeParty_GetPokeCount(party);
  for(i=0;i<num;i++){
    if(PP_Get( PokeParty_GetMemberPointer(party,i), ID_PARA_monsno, NULL)==MONSNO_PERAPPU){
      count++;
    }
  }
  if(count != 1){  //��C�łȂ���Ε��C
    NET_PRINT("��C�łȂ���Ε��C\n");
    return FALSE;
  }
  //��������ꏊ���؃��b�v���ǂ���
  if(PP_Get( PokeParty_GetMemberPointer(party,changepos), ID_PARA_monsno, NULL)==MONSNO_PERAPPU){
    PERAPVOICE_ClearExistFlag(pVoice); //����
    NET_PRINT("������\n");
    return TRUE;
  }
  NET_PRINT("�؃��b�v�̌����Ŗ���\n");
  return FALSE;
}



static void _setPokemonData(POKEMON_TRADE_WORK* pWork)
{
  // �o�b�N�A�b�v�J�n
//���[����߂�
//���R�[�h��߂�
//���A�f�[�^��߂�
//�}�Ӄf�[�^��߂�
//�l�S�V�G�[�V�������������ǂ�
  //�|�P������߂�

  if(!POKEMONTRADEPROC_IsTriSelect(pWork)){
    ///---�o�b�N�A�b�v
    pWork->aBackup.pMail = MAIL_BackupMailBlock(pWork->pGameData, pWork->heapID);
    pWork->aBackup.pRecord = RECORD_BackupBlock(pWork->pGameData, pWork->heapID);
    pWork->aBackup.pWifiHis = WIFIHISTORY_BackupBlock(pWork->pGameData, pWork->heapID);
    pWork->aBackup.pZukan = ZUKAN_SAVEDATA_BackupBlock(pWork->pGameData, pWork->heapID);
    pWork->aBackup.pNego = WIFI_NEGOTIATION_SV_BackupBlock(pWork->pGameData, pWork->heapID);
    PLAYTIME_GetSaveDate( GAMEDATA_GetPlayTimeWork(pWork->pGameData), &pWork->aBackup.date );
    if(pWork->pParentWork->selectBoxno == BOXDAT_GetTrayMax(pWork->pBox)){ //�Ă����̌����̏ꍇ
      pWork->aBackup.pPokeParty = PokeParty_AllocPartyWork( pWork->heapID );
      PokeParty_Copy(pWork->pMyParty, pWork->aBackup.pPokeParty);
      pWork->aBackup.bVoice = _deletePerapVoice(pWork, pWork->pMyParty,pWork->pParentWork->selectIndex);
    }
    else{
      pWork->aBackup.pBoxTray = GFL_HEAP_AllocMemory(pWork->heapID,BOXDAT_GetOneBoxDataSize());
      GFL_STD_MemCopy(BOXTRAYDAT_GetTrayData(pWork->pBox, pWork->pParentWork->selectBoxno),
                      pWork->aBackup.pBoxTray,BOXDAT_GetOneBoxDataSize());
    }
  }
  else{
    if(pWork->pParentWork->selectBoxno == BOXDAT_GetTrayMax(pWork->pBox)){ //�Ă����̌����̏ꍇ
      _deletePerapVoice(pWork, pWork->pMyParty,pWork->pParentWork->selectIndex);
    }
    pWork->bBackupStart=TRUE;  //�㕔�ŃG���[�ɂ���ׂɕK�v�ȃt���OON
  }
  //---
    // ���[���o�^
  if(pWork->pParentWork->selectBoxno != BOXDAT_GetTrayMax(pWork->pBox)) //�Ă����̌����̏ꍇ
  {
    POKEMON_PARAM* pp=PokeParty_GetMemberPointer( pWork->pParentWork->pParty, 0 );
    _ITEMMARK_ICON_WORK* pIM = &pWork->aItemMark;
    int item = PP_Get( pp , ID_PARA_item ,NULL);
    if(ITEM_CheckMail(item)){
      MAIL_BLOCK* pMailBlock = GAMEDATA_GetMailBlock(pWork->pGameData);


#if 0 //�f�o�b�O�p�r

      {
        int id,j;
        int itemno = ITEM_DUMMY_DATA;
        MAIL_DATA* src = NULL;
        //���[�N�쐬
        src = MailData_CreateWork(pWork->heapID);
        PP_Get(pp,ID_PARA_mail_data,src);

        for(j = 0;j < 100;j++){
          id = MAIL_SearchNullID(pMailBlock,MAILBLOCK_PASOCOM);
          if(id != MAILDATA_NULLID){
            //�p�\�R���̈�Ƀf�[�^�R�s�[
            MAIL_AddMailFormWork(pMailBlock,MAILBLOCK_PASOCOM,id,src);
          }
        }
        MailData_Clear(src);
        PP_Put(pp,ID_PARA_mail_data,(u32)src);
        PP_Put(pp,ID_PARA_item,ITEM_DUMMY_DATA);
        //�̈���
        GFL_HEAP_FreeMemory(src);
      }
#else
      {
        int id;
        int itemno = ITEM_DUMMY_DATA;
        MAIL_DATA* src = NULL;
        //�󂫗̈挟��
        id = MAIL_SearchNullID(pMailBlock,MAILBLOCK_PASOCOM);
        if(id != MAILDATA_NULLID){
          //���[�N�쐬
          src = MailData_CreateWork(pWork->heapID);
          //�|�P�������烁�[�����擾
          PP_Get(pp,ID_PARA_mail_data,src);
          //�p�\�R���̈�Ƀf�[�^�R�s�[
          MAIL_AddMailFormWork(pMailBlock,MAILBLOCK_PASOCOM,id,src);
          //�|�P�������烁�[�����O��
          MailData_Clear(src);
          PP_Put(pp,ID_PARA_mail_data,(u32)src);
          PP_Put(pp,ID_PARA_item,itemno);
          //�̈���
          GFL_HEAP_FreeMemory(src);
        }
      }
#endif
    }
  }

  WIFI_NEGOTIATION_SV_SetFriend(GAMEDATA_GetWifiNegotiation(pWork->pGameData),
                                pWork->pFriend);

  {
    WIFI_HISTORY* pWH = SaveData_GetWifiHistory(GAMEDATA_GetSaveControlWork(pWork->pGameData));
    const MYSTATUS* pBuff[2];
    pBuff[0] = pWork->pMy;
    pBuff[1] = pWork->pFriend;
    Comm_WifiHistoryCheck(pWH, pBuff, 2);
  }
  {
    POKEMON_PARAM* pp=PokeParty_GetMemberPointer( pWork->pParentWork->pParty, 0 );
    RECORD* pRec = GAMEDATA_GetRecordPtr(pWork->pGameData);

    STATUS_RCV_PokeParam_RecoverAll(pp); //��
    if(!PP_Get(pp,ID_PARA_tamago_flag,NULL)){
      // �������ꂽ�|�P�����ɓ����Ȃ��x
      u8 friend = FIRST_NATUKIDO;
      PP_Put(pp, ID_PARA_friend, friend);
    }
    
    //�|�P����
    if(PP_Get(pp,ID_PARA_tamago_flag,NULL)){
      POKE_MEMO_SetTrainerMemoPP( pp, POKE_MEMO_EGG_TRADE,
                                  pWork->pMy, 
                                  POKE_MEMO_PLACE_HUMAN_TRADE, pWork->heapID );
    }
    
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

    
    //������
    ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave( pWork->pGameData ), pp );
    //�������������
    if(pWork->type == POKEMONTRADE_TYPE_GTSNEGO){
      WIFI_NEGOTIATION_SV_AddChangeCount(GAMEDATA_GetWifiNegotiation(pWork->pGameData));
    }

    //�X�J�C�t�H����>�����h�t�H����
    if(MONSNO_492 == PP_Get( pp , ID_PARA_monsno ,NULL)){
      if(FORMNO_492_SKY == PP_Get( pp , ID_PARA_form_no ,NULL)){
        PP_ChangeFormNo(pp,FORMNO_SHEIMI_LAND);
        ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave( pWork->pGameData ), pp );
      }
    }
    
    //��������   // ����̃|�P�������̑I��ł����ꏊ�ɓ����
    if(pWork->pParentWork->selectBoxno == BOXDAT_GetTrayMax(pWork->pBox)){ //�Ă����̌����̏ꍇ
      POKEPARTY* party = pWork->pMyParty;
      PokeParty_SetMemberData(party, pWork->pParentWork->selectIndex, pp);
    }
    else{
      BOXDAT_PutPokemonPos(pWork->pBox, pWork->pParentWork->selectBoxno,
                           pWork->pParentWork->selectIndex,
                           (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pp));
    }
  }

  switch(pWork->type){
  case POKEMONTRADE_TYPE_WIFICLUB:
    // �����̋L�^
    GF_ASSERT((pWork->pParentWork->friendNo-1) >= 0);
    WifiList_SetResult(GAMEDATA_GetWiFiList(pWork->pGameData), pWork->pParentWork->friendNo-1,0,0,1);
    break;
  case POKEMONTRADE_TYPE_UNION:
  case POKEMONTRADE_TYPE_IRC:
    {
      int index;
      if(WifiList_CheckFriendMystatus(GAMEDATA_GetWiFiList(pWork->pGameData),pWork->pFriend,&index )) {
        WifiList_SetResult(GAMEDATA_GetWiFiList(pWork->pGameData), index,0,0,1);
      }
    }
    break;
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

  if(
    (pWork->type == POKEMONTRADE_TYPE_GTSNEGO)||
    (pWork->type == POKEMONTRADE_TYPE_WIFICLUB)||
    (pWork->type == POKEMONTRADE_TYPE_UNION)
    )
  {
    _CHANGE_STATE(pWork, _UnDataSend);
  }
  else{
    _CHANGE_STATE(pWork, _changeDemo_ModelTrade27);
  }
}



static void _mailBoxStart(POKEMON_TRADE_WORK* pWork)
{
  if(!POKETRADE_MESSAGE_EndCheck(pWork)){
    return;
  }

  if(pWork->anmCount > 180){
    _CHANGE_STATE(pWork,_saveStart);
  }
}


//�f�[�^�̍����ւ�

static void _changeDemo_ModelTrade27(POKEMON_TRADE_WORK* pWork)
{
  {   //�������炢�ł�PROCCHANGE���Ă��ǂ��悤�ɐe���[�N�Ɍ��������i�[
    POKEMON_PARAM* pp;// = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);

    if(POKEMONTRADEPROC_IsTriSelect(pWork))
    {
      int no = pWork->pokemonselectnoGTS % 3;
      pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 0);  //���肪�I�񂾃|�P
      pWork->pParentWork->selectBoxno = pWork->GTSSelectBoxno[0][no]; //�����Ă��I�񂾏ꏊ
      pWork->pParentWork->selectIndex = pWork->GTSSelectIndex[0][no]; //
#if PM_DEBUG
      NET_PRINT("�|�P����NO%d BOX%d INDEX%d SEL%d\n",
                PP_Get(pp,ID_PARA_monsno,NULL),
                pWork->pParentWork->selectBoxno,
                pWork->pParentWork->selectIndex,pWork->pokemonselectno);
#endif
    }
    else{
      pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
      pWork->pParentWork->selectBoxno = pWork->selectBoxno;
      pWork->pParentWork->selectIndex = pWork->selectIndex;
    }

    GF_ASSERT(pWork->pParentWork->pParty);
    
    PokeParty_Init( pWork->pParentWork->pParty, TEMOTI_POKEMAX );
    PokeParty_Add( pWork->pParentWork->pParty, pp );
    //�i���O��o�^
    ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave( pWork->pGameData ), pp );
  }


  // ���[������������{�b�N�X��
  if(pWork->pParentWork->selectBoxno != BOXDAT_GetTrayMax(pWork->pBox)) //BOX�̌����̏ꍇ
  {
    POKEMON_PARAM* pp=PokeParty_GetMemberPointer( pWork->pParentWork->pParty, 0 );
    _ITEMMARK_ICON_WORK* pIM = &pWork->aItemMark;
    int item = PP_Get( pp , ID_PARA_item ,NULL);
    if(ITEM_CheckMail(item)){
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_08, pWork->pMessageStrBuf );
      POKETRADE_MESSAGE_WindowOpen(pWork);
      _CHANGE_STATE(pWork,_mailBoxStart);
      pWork->anmCount=0;
      return;
    }
  }

  _CHANGE_STATE(pWork,_saveStart);
}



static void _saveStart(POKEMON_TRADE_WORK* pWork)
{
  if(!pWork->pGameData){
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade30); //�Q�[���f�[�^�̈��n���������ꍇ�Z�[�u�ɍs���Ȃ�
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
      
    
      after_mons_no = SHINKA_Check( NULL, pp, SHINKA_TYPE_TUUSHIN, (u32)pp2, GAMEDATA_GetSeasonID( pWork->pGameData),
                                    &cond, pWork->heapID );
      if( after_mons_no ){
        pWork->pParentWork->ret = POKEMONTRADE_MOVE_EVOLUTION;
        pWork->pParentWork->after_mons_no = after_mons_no;
        pWork->pParentWork->cond = cond;
        _CHANGE_STATE(pWork,POKEMONTRADE_PROC_FadeoutStart);   //�i�����ɍs��
        _endBGM_NoStart(pWork);
        return ;
      }
    }

    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_51, pWork->pMessageStrBuf );
    POKETRADE_MESSAGE_WindowOpen(pWork);
    POKETRADE_MESSAGE_WindowTimeIconStart(pWork);
    _CHANGE_STATE(pWork, _changeTimingSaveStart);
  }
  _endBGM(pWork);
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
  GFL_NET_ReloadIconTopOrBottom(TRUE, pWork->heapID);

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
  GFL_NET_ReloadIconTopOrBottom(TRUE, pWork->heapID);

  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, _BRIGHTNESS_SYNC);

  _CHANGE_STATE(pWork,_changeTimingSaveStart);
}



static void _changeTimingSaveStart(POKEMON_TRADE_WORK* pWork)
{
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_SHINKA_AFTER, WB_NET_TRADE_SERVICEID);
  _CHANGE_STATE(pWork,_changeTimingSaveStartT1);
}


static void _changeTimingSaveStartT1(POKEMON_TRADE_WORK* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_SHINKA_AFTER, WB_NET_TRADE_SERVICEID)){
    _CHANGE_STATE(pWork,_changeTimingSaveStartT2);
  }
}


static void _changeTimingSaveStartT2(POKEMON_TRADE_WORK* pWork)
{  
  if(!POKETRADE_MESSAGE_EndCheck(pWork)){
    return;
  }
  if(POKEMONTRADEPROC_IsNetworkMode(pWork)){
    _setPokemonData(pWork);  //�f�[�^�����ւ�
    pWork->pNetSave = NET_SAVE_InitCallback(pWork->heapID, pWork->pGameData, &_lastCallback , pWork);
    _CHANGE_STATE(pWork, _changeTimingSaveStart2);
  }
  else{
    _CHANGE_STATE(pWork, _changeDemo_ModelTrade30);
  }

}
static void _changeTimingSaveStart2(POKEMON_TRADE_WORK* pWork)
{
  if(NET_SAVE_Main(pWork->pNetSave)){
    _removeCallback(pWork);
    NET_SAVE_Exit(pWork->pNetSave);
    pWork->pNetSave=NULL;
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade30);
  }
}


static void _changeDemo_ModelTrade30(POKEMON_TRADE_WORK* pWork)
{
  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );

//  PMSND_FadeOutBGM( PMSND_FADE_FAST );
  pWork->anmCount=0;
  pWork->bBackupStart = FALSE;
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade31);
}


static void _changeDemo_ModelTrade31(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->anmCount < PMSND_FADE_FAST){
    return;
  }
  if(!WIPE_SYS_EndCheck()){
    return;
  }

  {   //20100615 add Satio
    if(pWork->bgchar){
      GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
                               GFL_ARCUTIL_TRANSINFO_GetSize( pWork->bgchar ));
    }
    pWork->bgchar = 0;
  }

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

  IRC_POKETRADE_SendVramBoxNameChar(pWork);
  IRC_POKETRADE_GraphicInitMainDisp(pWork);
  IRC_POKETRADE_GraphicInitSubDispSys(pWork);
  IRC_POKETRADE_GraphicInitSubDisp(pWork);

  IRC_POKETRADEDEMO_SetModel( pWork, REEL_PANEL_OBJECT);

  GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  POKETRADE_TOUCHBAR_Init(pWork);

  GFL_NET_ReloadIconTopOrBottom(TRUE, pWork->heapID);
  pWork->triCancel=0;
  _CHANGE_STATE(pWork,IRC_POKMEONTRADE_ChangeFinish);

}



