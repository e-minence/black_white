//=============================================================================
/**
 * @file	  ircpokemontrade.c
 * @brief	  �|�P�����������ĒʐM���I������
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/07/09
 */
//=============================================================================

#include <gflib.h>

#if PM_DEBUG
#include "debug/debugwin_sys.h"
#include "test/debug_pause.h"
#include "poke_tool/monsno_def.h"
#endif

#include "arc_def.h"
#include "net/network_define.h"

#include "ircpokemontrade.h"
#include "system/main.h"

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
#include "msg/msg_chr.h"
#include "msg/msg_trade_head.h"
#include "ircbattle.naix"
#include "trade.naix"
#include "net_app/connect_anm.h"
#include "net/dwc_rapfriend.h"
#include "savedata/wifilist.h"

#include "system/touch_subwindow.h"

//#include "poke_tool/poke_tool_def.h"
#include "item/item.h"
#include "app/app_menu_common.h"
#include "box_m_obj_NANR_LBLDEFS.h"
#include "p_st_obj_d_NANR_LBLDEFS.h"

#include "ircpokemontrade_local.h"


//#define _ENDTABLE  {192-32, 192, 256-32, 256}     //�I��
//#define _SEARCHTABLE  {192-32, 192, 0, 32}        //�����{�^��


///�ʐM�R�}���h
typedef enum {
  _NETCMD_SELECT_POKEMON = GFL_NET_CMD_IRCTRADE,
  _NETCMD_CHANGE_POKEMON,
  _NETCMD_CHANGE_YESNO,
  _NETCMD_CHANGE_CANCEL,
  _NETCMD_END_REQ,
  _NETCMD_END,
  _NETCMD_SCROLLBAR,
} _BATTLEIRC_SENDCMD;


static void _recvSelectPokemon(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _recvChangePokemon(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _recvChangeYesNo(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _recvChangeCancel(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);

static void _recvEndReq(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _recvEnd(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);

static void _touchState(IRC_POKEMON_TRADE* pWork);
static u8* _setChangePokemonBuffer(int netID, void* pWork, int size);
static void _changeWaitState(IRC_POKEMON_TRADE* pWork);
static void _changeYesNoWaitState(IRC_POKEMON_TRADE* pWork);
static void _endWaitState(IRC_POKEMON_TRADE* pWork);
static BOOL _PokemonsetAndSendData(IRC_POKEMON_TRADE* pWork);
static void _recvFriendScrollBar(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _changeFinish(IRC_POKEMON_TRADE* pWork);
static void _networkFriendsStandbyWait2(IRC_POKEMON_TRADE* pWork);
static void _touchStateCommon(IRC_POKEMON_TRADE* pWork);






///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _PacketTbl[] = {
  {_recvSelectPokemon,   _setChangePokemonBuffer},    ///_NETCMD_SELECT_POKEMON
  {_recvChangePokemon,   NULL},    ///_NETCMD_CHANGE_POKEMON
  {_recvChangeYesNo,   NULL},    ///_NETCMD_CHANGE_YESNO
  {_recvChangeCancel,   NULL},    ///
  {_recvEndReq, NULL},
  {_recvEnd, NULL},
  {_recvFriendScrollBar, NULL}, //_NETCMD_SCROLLBAR

};



#if 0
static void _messageDelete(IRC_POKEMON_TRADE *pWork)
{
  if(pWork->MessageWin){
    GFL_BMPWIN_ClearScreen(pWork->MessageWin);
    GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);
    BmpWinFrame_Clear(pWork->MessageWin, WINDOW_TRANS_OFF);
    GFL_BMPWIN_Delete(pWork->MessageWin);
    pWork->MessageWin=NULL;
  }

}
#endif

//------------------------------------------------------------------
/**
 * @brief   YESNO�E�C���h�E�V�X�e���X�^�[�g
 * @param   bgl
 * @param   touch_sub_window_sys
 * @retval  static
 */
//------------------------------------------------------------------
static void TouchYesNoStart( TOUCH_SW_SYS* touch_sub_window_sys )
{
  
#if 0

  TOUCH_SW_PARAM param;


  // YES NO �E�B���h�E�{�^���̕\��
  param.bg_frame	= GFL_BG_FRAME2_S;
  param.char_offs	= YESNO_CHARA_OFFSET+YESNO_CHARA_W*YESNO_CHARA_H;
  param.pltt_offs = 8;
  param.x			= 25;
  param.y			= 6;

  param.key_pos = 0;
  param.type = TOUCH_SW_TYPE_S;

  TOUCH_SW_Init( touch_sub_window_sys, &param );
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );
#endif
}

//--------------------------------------------------------------
//	�|�P����MCSS�쐬
//--------------------------------------------------------------
void IRCPOKETRADE_PokeCreateMcss( IRC_POKEMON_TRADE *pWork ,int no, int bFront, const POKEMON_PARAM *pp )
{
  MCSS_ADD_WORK addWork;
  VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};
  int xpos[] = { PSTATUS_MCSS_POS_X1 , PSTATUS_MCSS_POS_X2};
  int z;

  GF_ASSERT( pWork->pokeMcss[no] == NULL );

  if(bFront){
    MCSS_TOOL_MakeMAWPP( pp , &addWork , MCSS_DIR_FRONT );
    z=PSTATUS_MCSS_POS_MYZ;
  }
  else{
    MCSS_TOOL_MakeMAWPP( pp , &addWork , MCSS_DIR_BACK );
    z=PSTATUS_MCSS_POS_YOUZ;
  }
  pWork->pokeMcss[no] = MCSS_Add( pWork->mcssSys , xpos[no] , PSTATUS_MCSS_POS_Y , z , &addWork );
  MCSS_SetScale( pWork->pokeMcss[no] , &scale );

}

//--------------------------------------------------------------
//	�|�P����MCSS�폜
//--------------------------------------------------------------
void IRCPOKETRADE_PokeDeleteMcss( IRC_POKEMON_TRADE *pWork,int no  )
{
  if( pWork->pokeMcss[no] == NULL ){
    return;
  }

  MCSS_SetVanishFlag( pWork->pokeMcss[no] );
  MCSS_Del(pWork->mcssSys,pWork->pokeMcss[no]);
  pWork->pokeMcss[no] = NULL;
}


static void _msgWindowCreate(IRC_POKEMON_TRADE* pWork,int strno)
{
  int i=0;
  int frame = GFL_BG_FRAME3_S;

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

  if(pWork->MessageWin){
    return;
  }
  GFL_FONTSYS_SetDefaultColor();

  pWork->MessageWin = GFL_BMPWIN_Create(
    frame,
    ((0x20-_BUTTON_WIN_WIDTH)/2), (0x18-(2+_BUTTON_WIN_HEIGHT)), _BUTTON_WIN_WIDTH,_BUTTON_WIN_HEIGHT,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->MessageWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(pWork->MessageWin);
  GFL_BMPWIN_TransVramCharacter(pWork->MessageWin);
  BmpWinFrame_Write( pWork->MessageWin, WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

  GFL_MSG_GetString(  pWork->pMsgData, strno, pWork->pStrBuf );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MessageWin), 4, 4, pWork->pStrBuf, pWork->pFontHandle);
  GFL_BMPWIN_TransVramCharacter(pWork->MessageWin);
  GFL_BG_LoadScreenV_Req( frame );

}




//------------------------------------------------------------------------------
/**
 * @brief   ����ʃ|�P�����ʒu���v�Z
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

#if 1

static void _getPokeIconPos(int index, GFL_CLACTPOS* pos)
{
  static const u8	iconSize = 24;
  static const u8 iconTop = 72-24;
  static const u8 iconLeft = 72-48;

  pos->x = (index % 6) * iconSize + iconLeft;
  pos->y = (index / 6) * iconSize + iconTop;
}


POKEMON_PASO_PARAM* IRCPOKEMONTRADE_GetPokeDataAddress(BOX_MANAGER* boxData , int trayNo, int index,IRC_POKEMON_TRADE* pWork)
{
  POKEMON_PASO_PARAM* ppp;
  if(trayNo!=BOX_MAX_TRAY){
    return BOXDAT_GetPokeDataAddress(boxData,trayNo,index);
  }
  else{
    POKEPARTY* party = pWork->pMyParty;
    if(index < PokeParty_GetPokeCount(party)){
      return (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst( PokeParty_GetMemberPointer( party , index ) );
    }
  }
  return NULL;
}

#endif

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void IRC_POKMEONTRADE_changeState(IRC_POKEMON_TRADE* pWork,StateFunc state)
{
  pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#if _TRADE_DEBUG
void IRC_POKMEONTRADE_changeStateDebug(IRC_POKEMON_TRADE* pWork,StateFunc state, int line)
{
  OS_TPrintf("trade: %d\n",line);
  IRC_POKMEONTRADE_changeState(pWork, state);
}
#endif

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����̎�M�o�b�t�@��Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
u8* _setChangePokemonBuffer(int netID, void* pWk, int size)
{
  IRC_POKEMON_TRADE *pWork = pWk;
  if((netID >= 0) && (netID < 2)){
    return (u8*)pWork->recvPoke[netID];
  }
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @brief   �����Ƒ����index�w��Ńo�b�t�@��Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
POKEMON_PARAM* IRC_POKEMONTRADE_GetRecvPP(IRC_POKEMON_TRADE *pWork, int index)
{
  GF_ASSERT(index < 2);
  
  if(!GFL_NET_IsInit() || (0==GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )  ){
    return (POKEMON_PARAM*)pWork->recvPoke[index];
  }
  return (POKEMON_PARAM*)pWork->recvPoke[1-index];
}




//_NETCMD_SELECT_POKEMON
static void _recvSelectPokemon(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_POKEMON_TRADE *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }

  if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return;//�����͍̂��͎󂯎��Ȃ�
  }

  //�|�P�����Z�b�g���R�[��  �������d�����߂����ł͂��Ȃ�
  pWork->pokemonsetCall = netID+1;
}


//
//------------------------------------------------------------------------------
/**
 * @brief  �͂��������E�C���h�E������
 * _NETCMD_CHANGE_YESNO
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _recvChangeYesNo(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_POKEMON_TRADE *pWork = pWk;
  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  //�͂��������E�C���h�E

  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_20, pWork->pMessageStrBuf );
  IRC_POKETRADE_MessageWindowOpen(pWork);
  
  {
    int msg[]={POKETRADE_STR_27, POKETRADE_STR_28};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
  }
  
//  TouchYesNoStart(pWork->TouchSubWindowSys);
  pWork->bTouchReset=TRUE;

  _CHANGE_STATE(pWork,_changeYesNoWaitState);


}

//------------------------------------------------------------------------------
/**
 * @brief   ����̃X�N���[���o�[�̈ړ��ʒu�������Ă���
 *          _NETCMD_SCROLLBAR
 * @retval  none
 */
//------------------------------------------------------------------------------

//
static void _recvFriendScrollBar(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_POKEMON_TRADE *pWork = pWk;
  short* pRecvData = (short*)pData;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//�����̃f�[�^�͗v��Ȃ�
  }
  pWork->FriendBoxScrollNum = pRecvData[0];
}

// �L�����Z���̃R�[���o�b�N�̏���
static void _recvChangeCancelFunc(IRC_POKEMON_TRADE *pWork)
{
  //���ǂ�
  if(pWork->bTouchReset){
//    TOUCH_SW_Reset( pWork->TouchSubWindowSys );
    pWork->bTouchReset=FALSE;
  }
  IRC_POKETRADE_MessageWindowClear(pWork);
//  _messageDelete(pWork);

  _CHANGE_STATE(pWork, _touchState);
}

//_NETCMD_CHANGE_CANCEL
static void _recvChangeCancel(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_POKEMON_TRADE *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  _recvChangeCancelFunc(pWork);
}

//�I���R�[���o�b�N����
static void _recvEndReqFunc(IRC_POKEMON_TRADE *pWork)
{
//  _msgWindowCreate(pWork, POKETRADE_STR_22);

  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_22, pWork->pMessageStrBuf );
  IRC_POKETRADE_MessageWindowOpen(pWork);

  {
    int msg[]={POKETRADE_STR_27, POKETRADE_STR_28};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
  }

//  TouchYesNoStart(pWork->TouchSubWindowSys);
  pWork->bTouchReset=TRUE;

  _CHANGE_STATE(pWork, _endWaitState);
}

//_NETCMD_END_REQ
static void _recvEndReq(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_POKEMON_TRADE *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  _recvEndReqFunc(pWork);
}

//_NETCMD_END
static void _recvEnd(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_POKEMON_TRADE *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  _CHANGE_STATE(pWork, NULL);
}


// _NETCMD_CHANGE_POKEMON
static void _recvChangePokemon(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_POKEMON_TRADE *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  pWork->bChangeOK[netID] = TRUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	�I��������������̂Ńv���Z�X�I��
 *	@param	IRC_POKEMON_TRADE		���[�N
 */
//-----------------------------------------------------------------------------

static void _sendTimingCheck(IRC_POKEMON_TRADE* pWork)
{
  if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_ENDNO)){
    _CHANGE_STATE(pWork,NULL);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�����������
 *	@param	IRC_POKEMON_TRADE		���[�N
 */
//-----------------------------------------------------------------------------

static void _sendTiming(IRC_POKEMON_TRADE* pWork)
{
  GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_ENDNO);
  _CHANGE_STATE(pWork,_sendTimingCheck);
}

static void _noneState(IRC_POKEMON_TRADE* pWork)
{
  //�Ȃɂ����Ȃ�
}

///�|�P��������WINDOW�ɕ\��
static void _pokeNickNameMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_30, pWork->pExStrBuf );
  WORDSET_RegisterPokeNickName( pWork->pWordSet, 0,  pp );
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
}

//�|�P�����k�u��\��
static void _pokeLvMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{
  int lv;
  
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_21, pWork->pExStrBuf );
  lv=	PP_Get(pp, ID_PARA_level, NULL);

  WORDSET_RegisterNumber(pWork->pWordSet, 0, lv, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
}

//HP�|���΂₳�������\��
static void _pokeHPSPEEDMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{
  int i;
  for(i=0;i<6;i++){//HP�|���΂₳
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_31+i, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y+16*i, pWork->pStrBuf, pWork->pFontHandle);
  }
}

static void _pokePocketItemMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{ //��������
  int num;
  
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_44, pWork->pExStrBuf );
  num=	PP_Get(pp, ID_PARA_item, NULL);
  WORDSET_RegisterItemName( pWork->pWordSet, 0,  num );
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
}

  //HP HPMax
static void _pokeHpHpmaxMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{
  int num,num2;
  
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_40, pWork->pExStrBuf );
  num=	PP_Get(pp, ID_PARA_hp, NULL);
  num2=	PP_Get(pp, ID_PARA_hpmax, NULL);
  WORDSET_RegisterNumber(pWork->pWordSet, 0, num, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
  WORDSET_RegisterNumber(pWork->pWordSet, 1, num2, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf);
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
}

static void _pokeATTACKSPEEDNumMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{
  int i,num;
  
  for(i=0;i<5;i++){//���������|���΂₳����
    int paras[]={
      ID_PARA_pow,                //�U����
      ID_PARA_def,                //�h���
      ID_PARA_spepow,               //���U
      ID_PARA_spedef,               //���h
      ID_PARA_agi,                //�f����
    };
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_41, pWork->pExStrBuf );
    num=	PP_Get(pp, paras[i], NULL);
    WORDSET_RegisterNumber(pWork->pWordSet, 0, num, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y+16*i, pWork->pStrBuf, pWork->pFontHandle);
  }
}

static void _pokeTechniqueMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{
  { //�킴
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_45, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
  }
}

static void _pokeTechniqueListMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{
  int i,num;
  
  for(i=0;i<4;i++){//�킴
    num=	PP_Get(pp, ID_PARA_waza1+i, NULL);
    if(num==0){
      continue;
    }
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_46, pWork->pExStrBuf );
    WORDSET_RegisterWazaName(pWork->pWordSet, 0,  num);
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y+i*16, pWork->pStrBuf, pWork->pFontHandle);
  }
}

static void _pokePersonalMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{ //��������
  int num;
  GFL_MSGDATA *pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,  NARC_message_chr_dat, pWork->heapID );
  num=	PP_GetSeikaku(pp);
  GFL_MSG_GetString( pMsgData, seikaku00_msg+num, pWork->pStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
  GFL_MSG_Delete(pMsgData);
}

static void _pokeAttributeMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{ //�Ƃ�����
  int num;
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_43, pWork->pExStrBuf );
  num=	PP_Get(pp, ID_PARA_speabino, NULL);
  WORDSET_RegisterTokuseiName( pWork->pWordSet, 0,  num );
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����A�^�C�v�A�C�R���j��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_DeleteCLWK( _TYPE_ICON_WORK *wk )
{
  if(wk->clwk_type_icon){
    //CLWK�j��
    GFL_CLACT_WK_Remove( wk->clwk_type_icon );
    wk->clwk_type_icon = NULL;
    //���\�[�X�j��
    UI_EASY_CLWK_UnLoadResource( &wk->clres_type_icon );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	�����A�^�C�v�A�C�R���쐬
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *	@param	PokeType					�^�C�v
 *	@param	GFL_CLUNIT *unit	CLUNIT  ���j�b�g
 *	@param	heapID						�q�[�vID
 *
 *	@return	void
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_CreateCLWK( _TYPE_ICON_WORK *wk, POKEMON_PARAM* pp, int side,GFL_CLUNIT *unit, int x,int y,int draw_type,  HEAPID heapID )
{	
  UI_EAYSY_CLWK_RES_PARAM prm;
  PokeType type = PP_Get(pp, ID_PARA_type1+side, NULL);
  

  UITemplate_TYPEICON_DeleteCLWK(wk);
  
  prm.draw_type = draw_type;
  prm.comp_flg  = UI_EAYSY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetPokeTypePltArcIdx();
  prm.ncg_id    = APP_COMMON_GetPokeTypeCharArcIdx(type);
  prm.cell_id   = APP_COMMON_GetPokeTypeCellArcIdx( APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K );
  prm.pltt_line = PLTID_OBJ_TYPEICON_M;
  
  UI_EASY_CLWK_LoadResource( &wk->clres_type_icon, &prm, unit, heapID );

  wk->clwk_type_icon = UI_EASY_CLWK_CreateCLWK( &wk->clres_type_icon, unit, x, y, 0, heapID );
		
  GFL_CLACT_WK_SetPlttOffs( wk->clwk_type_icon, APP_COMMON_GetPokeTypePltOffset(type),
                            CLWK_PLTTOFFS_MODE_PLTT_TOP );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�[���A�C�R���j��
 *
 *	@param	_BALL_ICON_WORK* wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UITemplate_BALLICON_DeleteCLWK( _BALL_ICON_WORK* wk )
{	
  if(wk->clwk_ball){
    //CLWK�j��
    GFL_CLACT_WK_Remove( wk->clwk_ball );
    wk->clwk_ball=NULL;
    //���\�[�X�J��
    UI_EASY_CLWK_UnLoadResource( &wk->clres_ball );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�[���A�C�R���쐬
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk	���[�N
 *	@param	ballID	�{�[��ID 
 *	@param	*unit		CLUNIT
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
//UI�e���v���[�g �{�[��
static void UITemplate_BALLICON_CreateCLWK( _BALL_ICON_WORK* wk,
                                            POKEMON_PARAM* pp, GFL_CLUNIT *unit,
                                            int x ,int y, int drawtype, HEAPID heapID )
{
  UI_EAYSY_CLWK_RES_PARAM prm;
  BALL_ID ballID;

  // @todo �A�C�e�����{�[���ϊ����[�`���҂�
  ballID = BALLID_MASUTAABOORU;  // num=	PP_Get(pp, ID_PARA_item, NULL);

  UITemplate_BALLICON_DeleteCLWK(wk);

  prm.draw_type = drawtype;
  prm.comp_flg  = UI_EAYSY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetBallPltArcIdx( ballID );
  prm.ncg_id    = APP_COMMON_GetBallCharArcIdx( ballID );
  prm.cell_id   = APP_COMMON_GetBallCellArcIdx( ballID, APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetBallAnimeArcIdx( ballID, APP_COMMON_MAPPING_128K );
  prm.pltt_line = PLTID_OBJ_BALLICON_M;

  // ���\�[�X�ǂݍ���
  UI_EASY_CLWK_LoadResource( &wk->clres_ball, &prm, unit, heapID );
  // CLWK����
  wk->clwk_ball = UI_EASY_CLWK_CreateCLWK( &wk->clres_ball, unit, x, y, 0, heapID );
}

static void _pokeSexMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,IRC_POKEMON_TRADE* pWork)
{ //�Ƃ�����
  int sex;
  PRINTSYS_LSB fontCol;

  sex=	PP_Get(pp, ID_PARA_sex, NULL);

  if(sex > PM_FEMALE){
    return;  //�\�����Ȃ�
  }
  if(sex == PM_FEMALE){
    fontCol = PRINTSYS_LSB_Make( _FONT_PARAM_LETTER_RED , _FONT_PARAM_SHADOW_RED , 0 );
  }
  else{
    fontCol = PRINTSYS_LSB_Make( _FONT_PARAM_LETTER_BLUE , _FONT_PARAM_SHADOW_BLUE , 0 );
  }
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_47 + sex, pWork->pStrBuf );
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle,fontCol);
}



//--------------------------------------------------------------
//	�|�P������ԕ��̓Z�b�g
//--------------------------------------------------------------

static void _setPokemonStatusMessage(IRC_POKEMON_TRADE *pWork, int side,POKEMON_PARAM* pp)
{
  int i=0,lv;
  int frame = GFL_BG_FRAME3_M;
  int sidex[] = {1, 17};
 int xdotpos[]={0,128};
  
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);
  if(pWork->StatusWin[side]){
    GFL_BMPWIN_Delete(pWork->StatusWin[side]);
  }
  pWork->StatusWin[side] = GFL_BMPWIN_Create(frame,	sidex[side], 1, 15, 20,	_BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);

  _pokeNickNameMsgDisp(pp, pWork->StatusWin[side], 2*8, 0, pWork);
  _pokeLvMsgDisp(pp, pWork->StatusWin[side], 16, 16, pWork);
  _pokeSexMsgDisp(pp, pWork->StatusWin[side], 10*8, 0, pWork);
  _pokePocketItemMsgDisp(pp, pWork->StatusWin[side], 16, 16*8, pWork);

  UITemplate_BALLICON_CreateCLWK( &pWork->aBallIcon[side+UI_BALL_MYSTATUS], pp, pWork->cellUnit,
                                  xdotpos[side]+16, 16, CLSYS_DRAW_MAIN, pWork->heapID );

  GFL_BMPWIN_TransVramCharacter(pWork->StatusWin[side]);
  GFL_BMPWIN_MakeScreen(pWork->StatusWin[side]);
  GFL_BG_LoadScreenV_Req( frame );

}

//--------------------------------------------------------------
//	�|�P������ԕ��͏���
//--------------------------------------------------------------

void IRCPOKEMONTRADE_ResetPokemonStatusMessage(IRC_POKEMON_TRADE *pWork, int side)
{
  
  if(pWork->StatusWin[side]){
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->StatusWin[side]), 0);
    GFL_BMPWIN_ClearScreen(pWork->StatusWin[side]);
    GFL_BMPWIN_Delete(pWork->StatusWin[side]);
    GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_M);
    pWork->StatusWin[side] = NULL;
    UITemplate_BALLICON_DeleteCLWK( &pWork->aBallIcon[side+UI_BALL_MYSTATUS]);
  }
}


//--------------------------------------------------------------
//	�|�P�����\���Z�b�g
//--------------------------------------------------------------
static void _Pokemonset(IRC_POKEMON_TRADE *pWork, int side, POKEMON_PARAM* pp )
{
  IRCPOKETRADE_PokeDeleteMcss(pWork, side);

  IRCPOKETRADE_PokeCreateMcss(pWork, side, 1-side, pp);
  
  pWork->bPokemonSet[side]=TRUE;

  _setPokemonStatusMessage(pWork,side ,pp);
}


//--------------------------------------------------------------
//	�|�P�����\�����Z�b�g
//--------------------------------------------------------------
static void _PokemonReset(IRC_POKEMON_TRADE *pWork, int side )
{
  IRCPOKETRADE_PokeDeleteMcss(pWork, side);
  pWork->bPokemonSet[side]=FALSE;

  IRCPOKEMONTRADE_ResetPokemonStatusMessage(pWork, side);
}





// ���\���̕ύX �P�܂��߂��Q�܂��߂�����
static void _changePokemonMyStDisp(IRC_POKEMON_TRADE* pWork,int pageno,int leftright,POKEMON_PARAM* pp)
{
  int i,num,num2;
  int xwinpos[]={0,16};
  int xdotpos[]={0,128};
  {
    GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);
    GFL_FONTSYS_SetColor( 0xf, 0xe, 0x0 );
    if(pWork->MyInfoWin){
      GFL_BMPWIN_Delete(pWork->MyInfoWin);
    }
    pWork->MyInfoWin =
      GFL_BMPWIN_Create(GFL_BG_FRAME3_S,
                        xwinpos[leftright], 0, 16 , 18, _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
  }

  if(pageno == 0){  //���������Ƃ�
    TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_CUR_R, TRUE);
    TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_CUR_L, FALSE);

    UITemplate_BALLICON_CreateCLWK( &pWork->aBallIcon[UI_BALL_SUBSTATUS], pp, pWork->cellUnit,
                                    xdotpos[leftright]+16, 16, CLSYS_DRAW_SUB, pWork->heapID );

    _pokeNickNameMsgDisp(pp,pWork->MyInfoWin, 32, 0,pWork);
    _pokeLvMsgDisp(pp,pWork->MyInfoWin, 32 ,16,pWork);
    _pokeHPSPEEDMsgDisp(pp,pWork->MyInfoWin, 8 ,32,pWork);
    _pokePocketItemMsgDisp(pp,pWork->MyInfoWin, 4*8 ,16*8,pWork);
    _pokeHpHpmaxMsgDisp(pp, pWork->MyInfoWin, 8*8,4*8, pWork);
    _pokeATTACKSPEEDNumMsgDisp(pp, pWork->MyInfoWin, 10*8,6*8, pWork);
    _pokeSexMsgDisp(pp, pWork->MyInfoWin, 12*8, 0, pWork);

    IRC_POKETRADE_ItemIconDisp(pWork, leftright, pp);
    IRC_POKETRADE_PokerusIconDisp(pWork, leftright,FALSE, pp);
    IRC_POKETRADE_LeftPageMarkDisp(pWork,APP_COMMON_BARICON_CURSOR_RIGHT);
    
  }
  else{
    IRC_POKETRADE_ItemIconReset(&pWork->aItemMark);
    IRC_POKETRADE_ItemIconReset(&pWork->aPokerusMark);
    TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_CUR_R, FALSE);
    TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_CUR_L, TRUE);

    UITemplate_BALLICON_DeleteCLWK(&pWork->aBallIcon[UI_BALL_SUBSTATUS]);
    _pokeTechniqueMsgDisp(pp, pWork->MyInfoWin, 2*8,0, pWork);
    _pokeTechniqueListMsgDisp(pp, pWork->MyInfoWin, 2*8,2*8, pWork);
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_37, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 2*8, 10*8, pWork->pStrBuf, pWork->pFontHandle);
    _pokePersonalMsgDisp(pp, pWork->MyInfoWin, 2*8, 12*8, pWork);
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_38, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 2*8, 14*8, pWork->pStrBuf, pWork->pFontHandle);
    _pokeAttributeMsgDisp(pp, pWork->MyInfoWin, 2*8, 16*8, pWork);

   IRC_POKETRADE_LeftPageMarkDisp(pWork,APP_COMMON_BARICON_CURSOR_LEFT);

  }

  IRC_POKETRADE_SubStatusInit(pWork,pWork->x, pageno);  //���BG�\��
    
  GFL_BMPWIN_MakeScreen(pWork->MyInfoWin);
  GFL_BMPWIN_TransVramCharacter(pWork->MyInfoWin);
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_S );
}

static void _resetPokemonMyStDisp(IRC_POKEMON_TRADE* pWork)
{
  UITemplate_BALLICON_DeleteCLWK(&pWork->aBallIcon[UI_BALL_SUBSTATUS]);
  if(pWork->MyInfoWin){
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 0 );
    GFL_BMPWIN_ClearScreen(pWork->MyInfoWin);
    GFL_BMPWIN_TransVramCharacter(pWork->MyInfoWin);
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_S );
    GFL_BMPWIN_Delete(pWork->MyInfoWin);
    pWork->MyInfoWin = NULL;
  }
}




//�����I��ʐM���M
static void _changePokemonSendDataNetwork(IRC_POKEMON_TRADE* pWork)
{
  // �ʐM���ĂȂ���ΐi��
  if( !GFL_NET_IsInit() || GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_CHANGE_POKEMON,0,NULL)){

    //���肩��̕Ԏ���҂̂Ń��b�Z�[�W�͑ҋ@��
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_09, pWork->pMessageStrBuf );
    IRC_POKETRADE_MessageWindowOpen(pWork);
    _CHANGE_STATE(pWork,_changeWaitState);
  }
}

// �X�e�[�^�X�\���̕ύX
static void _changePokemonStatusDisp(IRC_POKEMON_TRADE* pWork)
{
  int i,num,num2;
  POKEMON_PARAM* pp = IRC_POKEMONTRADE_GetRecvPP(pWork, pWork->pokemonselectno);
//  pWork->recvPoke[pWork->pokemonselectno];

  {//�����̈ʒu����
    VecFx32 apos;
    apos.x = _MCSS_POS_X(50);
    apos.y = _MCSS_POS_Y(8);
    apos.z = _MCSS_POS_Z(0);
    MCSS_SetPosition( pWork->pokeMcss[pWork->pokemonselectno] ,&apos );
    MCSS_ResetVanishFlag(pWork->pokeMcss[pWork->pokemonselectno]);

    //����̂�OFF
    MCSS_SetVanishFlag(pWork->pokeMcss[1-pWork->pokemonselectno]);

  }
  if(pWork->MyInfoWin){
    GFL_BMPWIN_Delete(pWork->MyInfoWin);
  }
  GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );
  pWork->MyInfoWin =
    GFL_BMPWIN_Create(GFL_BG_FRAME3_M,
                      1, 1, 31 , 24, _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);

  _pokeNickNameMsgDisp(pp,pWork->MyInfoWin, 16, 0,pWork);//�j�b�N�l�[��
  _pokeLvMsgDisp(pp,pWork->MyInfoWin, 8*12 , 0,pWork);
  _pokeSexMsgDisp(pp, pWork->MyInfoWin, 10*8, 0, pWork);

  _pokeHPSPEEDMsgDisp(pp,pWork->MyInfoWin, 0 ,3*8,pWork);

  for(i=0;i<3;i++){//��������-��������
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_37+i, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 0, 16*8+16*i, pWork->pStrBuf, pWork->pFontHandle);
  }
  _pokeHpHpmaxMsgDisp(pp, pWork->MyInfoWin, 8*7, 3*8, pWork);  //HP HPMax

  _pokeATTACKSPEEDNumMsgDisp(pp, pWork->MyInfoWin, 8*9, 5*8, pWork);


  _pokePersonalMsgDisp(pp, pWork->MyInfoWin, 9*8, 16*8, pWork);  //���i
  _pokeAttributeMsgDisp(pp, pWork->MyInfoWin, 9*8, 18*8, pWork);  //����
  _pokePocketItemMsgDisp(pp,pWork->MyInfoWin, 9*8, 20*8,pWork);  //��������
  _pokeTechniqueMsgDisp(pp, pWork->MyInfoWin, 20*8, 13*8, pWork);  //�킴
  _pokeTechniqueListMsgDisp(pp, pWork->MyInfoWin, 20*8,15*8, pWork); //�킴���X�g

  UITemplate_BALLICON_CreateCLWK( &pWork->aBallIcon[UI_BALL_SUBSTATUS], pp, pWork->cellUnit,
                                  16, 16, CLSYS_DRAW_MAIN, pWork->heapID );

  UITemplate_TYPEICON_CreateCLWK(&pWork->aTypeIcon[0], pp, 0, pWork->cellUnit,
                                  25*8, 12*8, CLSYS_DRAW_MAIN, pWork->heapID );

  if( PP_Get(pp, ID_PARA_type1, NULL) != PP_Get(pp, ID_PARA_type2, NULL)){
    UITemplate_TYPEICON_CreateCLWK(&pWork->aTypeIcon[1], pp, 1, pWork->cellUnit,
                                   29*8, 12*8, CLSYS_DRAW_MAIN, pWork->heapID );
  }
  IRC_POKETRADE_PokeStatusIconDisp(pWork,pp);
  IRC_POKETRADE_PokerusIconDisp(pWork, 0,TRUE, pp);

  
  GFL_BMPWIN_MakeScreen(pWork->MyInfoWin);
  GFL_BMPWIN_TransVramCharacter(pWork->MyInfoWin);
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );
}


// �|�P�����̃X�e�[�^�X�\���҂�
static void _pokemonStatusWait(IRC_POKEMON_TRADE* pWork)
{
  BOOL bReturn=FALSE;
  GFL_UI_TP_HITTBL tp_data[]={
    {_POKEMON_SELECT1_CELLY-8,_POKEMON_SELECT1_CELLY+16,_POKEMON_SELECT1_CELLX-8, _POKEMON_SELECT1_CELLX+16},
    {_POKEMON_SELECT2_CELLY-8,_POKEMON_SELECT2_CELLY+16,_POKEMON_SELECT2_CELLX-8, _POKEMON_SELECT2_CELLX+16},
    {GFL_UI_TP_HIT_END,0,0,0},
  };

  switch(GFL_UI_TP_HitTrg(tp_data)){
  case 0:
    if(pWork->pokemonselectno!=0){
      pWork->pokemonselectno = 0;
      {
        GFL_CLACTPOS pos = {_POKEMON_SELECT1_CELLX,_POKEMON_SELECT1_CELLY+4};
        GFL_CLACT_WK_SetPos( pWork->curIcon[CELL_CUR_POKE_SELECT], &pos, CLSYS_DRAW_SUB );
      }
      _changePokemonStatusDisp(pWork);
    }
    break;
  case 1:
    if(pWork->pokemonselectno!=1){
      pWork->pokemonselectno = 1;
      {
        GFL_CLACTPOS pos = {_POKEMON_SELECT2_CELLX,_POKEMON_SELECT2_CELLY+4};
        GFL_CLACT_WK_SetPos( pWork->curIcon[CELL_CUR_POKE_SELECT], &pos, CLSYS_DRAW_SUB );
      }
      _changePokemonStatusDisp(pWork);
    }
    break;
  default:
    break;
  }

  
  TOUCHBAR_Main(pWork->pTouchWork);
  switch( TOUCHBAR_GetTrg(pWork->pTouchWork )){
  case TOUCHBAR_ICON_RETURN:
    bReturn=TRUE;
    break;
  default:
    break;
  }
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL){ /// �߂鏈��
    bReturn=TRUE;
  }

  if(bReturn){
    IRC_POKETRADE_ItemIconReset(&pWork->aPokerusMark);
    IRC_POKETRADE_PokeStatusIconReset(pWork);
    IRC_POKETRADEDEMO_SetModel( pWork, REEL_PANEL_OBJECT);
    GXS_SetVisibleWnd( GX_WNDMASK_NONE );
    UITemplate_BALLICON_DeleteCLWK(&pWork->aBallIcon[UI_BALL_SUBSTATUS]);
    UITemplate_TYPEICON_DeleteCLWK(&pWork->aTypeIcon[0]);
    UITemplate_TYPEICON_DeleteCLWK(&pWork->aTypeIcon[1]);
    
    IRC_POKETRADE_ResetMainStatusBG(pWork);
    IRCPOKETRADE_PokeDeleteMcss(pWork,0);
    IRCPOKETRADE_PokeDeleteMcss(pWork,1);
    GFL_BMPWIN_ClearScreen(pWork->MyInfoWin);
    GFL_BMPWIN_Delete(pWork->MyInfoWin);
    pWork->MyInfoWin = NULL;
    _Pokemonset(pWork, 0, IRC_POKEMONTRADE_GetRecvPP(pWork, 0));
    _Pokemonset(pWork, 1, IRC_POKEMONTRADE_GetRecvPP(pWork, 1));
    IRC_POKETRADE_GraphicInitMainDisp(pWork);
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );
    G2_BlendNone();
    _CHANGE_STATE(pWork, _networkFriendsStandbyWait2);
  }
}




// �|�P�����̃X�e�[�^�X�\��
static void _pokemonStatusStart(IRC_POKEMON_TRADE* pWork)
{

  IRC_POKETRADEDEMO_RemoveModel( pWork);  //���[��������
  IRC_POKETRADE_MessageWindowClear(pWork);  //���̃��b�Z�[�W������

  IRCPOKEMONTRADE_ResetPokemonStatusMessage(pWork,0); //��̃X�e�[�^�X����+OAM������
  IRCPOKEMONTRADE_ResetPokemonStatusMessage(pWork,1); //��̃X�e�[�^�X����+OAM������

  pWork->pokemonselectno = 0;//��������\��
  _changePokemonStatusDisp(pWork);

  IRC_POKETRADE_SetMainStatusBG(pWork);  // �w�iBG��
  GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );

  {
    G2S_SetWnd0InsidePlane( 
                 GX_WND_PLANEMASK_OBJ,
                 FALSE );
    G2S_SetWndOutsidePlane(
    GX_WND_PLANEMASK_BG0|
    GX_WND_PLANEMASK_BG1|
    GX_WND_PLANEMASK_BG2|
    GX_WND_PLANEMASK_BG3|
    GX_WND_PLANEMASK_OBJ,
       TRUE );

    
    G2S_SetWnd0Position( 128-8*10, 0, 128+8*10, 33 );
    GXS_SetVisibleWnd( GX_WNDMASK_W0 );
  }


  _CHANGE_STATE(pWork, _pokemonStatusWait);

}


//�����I��҂�
static void _changePokemonSendData(IRC_POKEMON_TRADE* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    IRC_POKETRADE_AppMenuClose(pWork);
    IRC_POKETRADE_MessageWindowClear(pWork);  //���̃��b�Z�[�W������
    pWork->pAppTask=NULL;
    switch(selectno){
    case 0:  //��������
      _CHANGE_STATE(pWork,_changePokemonSendDataNetwork);
      break;
    case 1:  //�悳���݂�
      _CHANGE_STATE(pWork, _pokemonStatusStart);
      break;
    case 2:  //���ǂ�
      _CHANGE_STATE(pWork, _touchState);
      break;
    }
  }
}

//��������ɂ����@�ʐM����̏����҂�
static void _networkFriendsStandbyWait2(IRC_POKEMON_TRADE* pWork)
{
  if(IRC_POKETRADE_MessageEndCheck(pWork)){
    {
      int msg[]={POKETRADE_STR_05, POKETRADE_STR_04, POKETRADE_STR_06};
      IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
    }

    _CHANGE_STATE(pWork,_changePokemonSendData);
  }
}


//��������ɂ����@�ʐM����̏����҂�
static void _networkFriendsStandbyWait(IRC_POKEMON_TRADE* pWork)
{
  int i;

  if(IRC_POKETRADE_MessageEndCheck(pWork)){
    if(pWork->bPokemonSet[0] && pWork->bPokemonSet[1]){  //�����̃|�P����������ꍇ

      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_20, pWork->pMessageStrBufEx );
      for(i=0;i<2;i++){
        POKEMON_PARAM* pp = IRC_POKEMONTRADE_GetRecvPP(pWork, i); //pWork->recvPoke[i];
        WORDSET_RegisterPokeNickName( pWork->pWordSet, i,  pp );
      }
      WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx  );


      IRC_POKETRADE_MessageWindowOpen(pWork);

      _CHANGE_STATE(pWork,_networkFriendsStandbyWait2);
    }
  }
}


// �|�P�����f�[�^�𑊎�ɑ���
static void _pokeSendDataState(IRC_POKEMON_TRADE* pWork)
{

  if(_PokemonsetAndSendData(pWork)){
      _CHANGE_STATE(pWork, _networkFriendsStandbyWait);// �����Ă��I�Ԃ܂ő҂�
  }
}


//��������ɂ����A���j���[�҂�
static void _changeMenuWait(IRC_POKEMON_TRADE* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    IRC_POKETRADE_AppMenuClose(pWork);
    pWork->pAppTask=NULL;

    if(selectno==0){  //�����ɏo��
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_09, pWork->pMessageStrBuf );

      IRC_POKETRADE_MessageWindowOpen(pWork);

      _CHANGE_STATE(pWork, _pokeSendDataState);

    }
    else{  //�L�����Z��
      _PokemonReset(pWork,0);
      _CHANGE_STATE(pWork, _touchState);
    }
  }
}


//��������ɂ����A���j���[�\��
static void _changeMenuOpen(IRC_POKEMON_TRADE* pWork)
{
  TOUCHBAR_SetVisible( pWork->pTouchWork, TOUCHBAR_ICON_CUR_U, FALSE );
  TOUCHBAR_SetVisible( pWork->pTouchWork, TOUCHBAR_ICON_CUR_R, FALSE );
  TOUCHBAR_SetVisible( pWork->pTouchWork, TOUCHBAR_ICON_CUR_L, FALSE );
  TOUCHBAR_SetVisible( pWork->pTouchWork, TOUCHBAR_ICON_RETURN ,FALSE );
  GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], FALSE );

  
  if( _PokemonsetAndSendData(pWork) )
  {
    int msg[]={POKETRADE_STR_01,POKETRADE_STR_02};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));

    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ , 8 );

    _CHANGE_STATE(pWork, _changeMenuWait);
  }
}

static BOOL IsTouchCLACTPosition(IRC_POKEMON_TRADE* pWork)
{
  BOOL bChange=FALSE;;
  {
    u32 x,y;
    int line,index;
    if(GFL_UI_TP_GetPointTrg(&x, &y)==TRUE){
      GFL_CLWK* pCL = IRC_POKETRADE_GetCLACT(pWork,x+12,y+12, &pWork->workBoxno, &pWork->workPokeIndex, &line, &index);
      if(pCL){
        if(pWork->pCatchCLWK){
          //���̂���ł镨�����̈ʒu�ɖ߂�
          GFL_CLACT_WK_SetPos(pWork->pCatchCLWK, &pWork->aCatchOldPos, CLSYS_DRAW_SUB);
        }
        pWork->pCatchCLWK = pCL;
        GFL_CLACT_WK_GetPos(pWork->pCatchCLWK, &pWork->aCatchOldPos, CLSYS_DRAW_SUB);
        IRC_POKETRADE_CursorEnable(pWork,line, index);  //OAM�J�[�\���ړ�
        OS_TPrintf("GET %d %d\n",pWork->workBoxno,pWork->workPokeIndex);
        pWork->underSelectBoxno  = pWork->workBoxno;
        pWork->underSelectIndex = pWork->workPokeIndex;
        pWork->x = x;
        pWork->y = y;
        pWork->workPokeIndex = 0;
        pWork->workBoxno = 0;
        bChange = TRUE;
      }
    }
  }
  return bChange;
}



//������ʂɃX�e�[�^�X�\�� ����Ɍ����邩�ǂ����҂�
static void _dispSubStateWait(IRC_POKEMON_TRADE* pWork)
{
  BOOL bExit=FALSE;
  BOOL bChange=FALSE;
  int selectno=0;

  TOUCHBAR_Main(pWork->pTouchWork);
  switch( TOUCHBAR_GetTrg(pWork->pTouchWork )){
  case TOUCHBAR_ICON_CUR_L:
  case TOUCHBAR_ICON_CUR_R:
    {
      pWork->pokemonselectno = 1 - pWork->pokemonselectno;
      bChange=TRUE;
    }
    break;
  default:
    if(GFL_UI_KEY_GetTrg()==PAD_KEY_LEFT){
      if(pWork->pokemonselectno == 1){
        pWork->pokemonselectno = 0;
        bChange=TRUE;
      }
    }
    else if(GFL_UI_KEY_GetTrg()==PAD_KEY_RIGHT){
      if(pWork->pokemonselectno == 0){
        pWork->pokemonselectno = 1;
        bChange=TRUE;
      }
    }
    break;
  }

  if(IsTouchCLACTPosition(pWork)){
    bChange = TRUE;
  }

  if(bChange){
    POKEMON_PASO_PARAM* ppp =
      IRCPOKEMONTRADE_GetPokeDataAddress(pWork->pBox,
                                         pWork->underSelectBoxno, pWork->underSelectIndex,pWork);
    POKEMON_PARAM* pp = PP_CreateByPPP(ppp,pWork->heapID);
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 0 );
    _resetPokemonMyStDisp(pWork);
    _changePokemonMyStDisp(pWork, pWork->pokemonselectno , (pWork->x < 128), pp);
    GFL_HEAP_FreeMemory(pp);
    return;
  }

  // ���Ă���
  if(pWork->pCatchCLWK != NULL){  //�^�b�`�p�l�����쒆�̃A�C�R���ړ�
    u32 x,y;
    GFL_CLACTPOS pos;
    if(GFL_UI_TP_GetPointCont(&x,&y)){
      pos.x = x;
      pos.y = y;
      GFL_CLACT_WK_SetPos( pWork->pCatchCLWK, &pos, CLSYS_DRAW_SUB);
    }
  }
  {
    u32 x,y;

    if(GFL_UI_TP_GetCont()==FALSE){ //�^�b�`�p�l���𗣂���
      if((pWork->pCatchCLWK != NULL) && pWork->bUpVec){ //�|�P��������ɓo�^
        selectno = 0;  //����Ɠ���
        bExit=TRUE;  
      }
    }
    pWork->bUpVec = FALSE;
    if(GFL_UI_TP_GetPointCont(&x,&y)){   //�x�N�g�����Ď�  // ���������
      if(pWork->y > (y + 10)){   // ��Ɉړ������ꍇ���ˏグ�鏈��
        pWork->bUpVec = TRUE;
      }
      pWork->x = x;
      pWork->y = y;
    }
  }

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    bExit=TRUE;
  }
  if(bExit){
    if(selectno==0){
      //����Ɍ�����
      pWork->selectIndex = pWork->underSelectIndex;
      pWork->selectBoxno = pWork->underSelectBoxno;
      GFL_CLACT_WK_SetDrawEnable( pWork->pCatchCLWK, FALSE);
      _CHANGE_STATE(pWork, _changeMenuOpen);
    }
    else{
      _CHANGE_STATE(pWork, _touchState);
      GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], TRUE );
    }
    IRC_POKETRADE_ItemIconReset(&pWork->aItemMark);
    IRC_POKETRADE_ItemIconReset(&pWork->aPokerusMark);
    _resetPokemonMyStDisp(pWork);
    IRC_POKETRADE_SubStatusEnd(pWork);
    IRC_POKETRADE_AppMenuClose(pWork);
    pWork->pAppTask=NULL;
  }
}


// �����̉�ʂɃX�e�[�^�X��\��

static void _dispSubState(IRC_POKEMON_TRADE* pWork)
{

  TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_RETURN, FALSE);
  TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_CUR_U, FALSE);

  {
    int msg[]={POKETRADE_STR_03,POKETRADE_STR_02};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
  }

  {
    POKEMON_PASO_PARAM* ppp = IRCPOKEMONTRADE_GetPokeDataAddress(pWork->pBox,
                                                                 pWork->underSelectBoxno, pWork->underSelectIndex,pWork);
    POKEMON_PARAM* pp = PP_CreateByPPP(ppp,pWork->heapID);
    pWork->pokemonselectno = 0;
    _changePokemonMyStDisp(pWork, pWork->pokemonselectno , (pWork->x < 128), pp);
    GFL_HEAP_FreeMemory(pp);
  }

  GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], FALSE );
  
  _CHANGE_STATE(pWork,_dispSubStateWait);
}

//�ʐM���������
static void _changeTimingDemoStart(IRC_POKEMON_TRADE* pWork)
{
  if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_TRADEDEMO_START)){
    _CHANGE_STATE(pWork,IRC_POKMEONTRADE_STEP_ChangeDemo_PokeMove);
  }
}

// �����̕Ԏ���҂�
static void _changeWaitState(IRC_POKEMON_TRADE* pWork)
{
  int i;

  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  if(GFL_NET_IsInit()){
    for(i=0;i<2;i++){
      if(pWork->bChangeOK[i]==FALSE){  // �ʐM�Ԏ��҂�
        return;
      }
    }
  }
  if(GFL_NET_IsInit()){
    GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_TRADEDEMO_START);
    _CHANGE_STATE(pWork, _changeTimingDemoStart);
  }
  else{
    _CHANGE_STATE(pWork,IRC_POKMEONTRADE_STEP_ChangeDemo_PokeMove);
  }
}

void IRC_POKMEONTRADE_ChangeFinish(IRC_POKEMON_TRADE* pWork)
{
  int id = 1-GFL_NET_SystemGetCurrentID();

  pWork->selectBoxno = 0;
  pWork->selectIndex = -1;
  pWork->pCatchCLWK = NULL;
  IRCPOKETRADE_PokeDeleteMcss(pWork, 0);
  IRCPOKETRADE_PokeDeleteMcss(pWork, 1);

  {
    int i;
    for(i = 0;i< CELL_DISP_NUM;i++){
      if(pWork->curIcon[i]){
        GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[i], TRUE );
      }
    }
  }
  

  IRC_POKETRADE_MessageWindowClear(pWork);

  pWork->bPokemonSet[0]=FALSE;
  pWork->bPokemonSet[1]=FALSE;

  _CHANGE_STATE(pWork, _touchState);
}


//�L�����Z���܂�
static void _cancelPokemonSendDataNetwork(IRC_POKEMON_TRADE* pWork)
{
  if(GFL_NET_IsInit()){
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_CHANGE_CANCEL,0,NULL)){
      _CHANGE_STATE(pWork,_noneState);
    }
  }
  else{
    _recvChangeCancelFunc(pWork);
  }
}


//�͂��������̊m�F
static void _changeYesNoWaitState(IRC_POKEMON_TRADE* pWork)
{
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    IRC_POKETRADE_AppMenuClose(pWork);
    pWork->pAppTask=NULL;
    switch(selectno){
    case 0:
      if(pWork->bTouchReset){
        //TOUCH_SW_Reset( pWork->TouchSubWindowSys );
        pWork->bTouchReset=FALSE;
      }
//      _messageDelete(pWork);
      _CHANGE_STATE(pWork,_changePokemonSendDataNetwork);
      break;
    case 1:
      _CHANGE_STATE(pWork,_cancelPokemonSendDataNetwork);
      break;
    }
  }

#if 0
  
  int result;
  result = TOUCH_SW_Main( pWork->TouchSubWindowSys );
  switch(result){				//��������
  case TOUCH_SW_RET_YES:						//�͂�
    if(pWork->bTouchReset){
      TOUCH_SW_Reset( pWork->TouchSubWindowSys );
      pWork->bTouchReset=FALSE;
    }
    _messageDelete(pWork);
    _CHANGE_STATE(pWork,_changePokemonSendDataNetwork);
    break;
  case TOUCH_SW_RET_NO:						//������
    _CHANGE_STATE(pWork,_cancelPokemonSendDataNetwork);
    break;
  }
#endif

}


//�I���̂𑊎�ɓ`���Ă���
static void _endWaitStateNetwork(IRC_POKEMON_TRADE* pWork)
{
  if(GFL_NET_IsInit()){
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_END,0,NULL)){
      _CHANGE_STATE(pWork,_noneState);
    }
  }
  else{
    _CHANGE_STATE(pWork, NULL);
  }
}

// �����܂�
static void _endWaitState(IRC_POKEMON_TRADE* pWork)
{
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    IRC_POKETRADE_AppMenuClose(pWork);
    pWork->pAppTask=NULL;
    switch(selectno){
    case 0:
      _CHANGE_STATE(pWork,_endWaitStateNetwork);
      break;
    case 1:
      _CHANGE_STATE(pWork,_cancelPokemonSendDataNetwork);
      break;
    }
  }

#if 0
  int result;
  result = TOUCH_SW_Main( pWork->TouchSubWindowSys );
  switch(result){				//�����
  case TOUCH_SW_RET_YES:						//�͂�
    _CHANGE_STATE(pWork,_endWaitStateNetwork);
    break;
  case TOUCH_SW_RET_NO:						//������
    _CHANGE_STATE(pWork,_cancelPokemonSendDataNetwork);
    break;
  }
#endif

}



static BOOL _PokemonsetAndSendData(IRC_POKEMON_TRADE* pWork)
{ //�I���|�P�����\��
  BOOL bRet = FALSE;
  POKEMON_PASO_PARAM* ppp = IRCPOKEMONTRADE_GetPokeDataAddress(pWork->pBox, pWork->selectBoxno, pWork->selectIndex,pWork);
  POKEMON_PARAM* pp = PP_CreateByPPP(ppp,pWork->heapID);
  
  if(!GFL_NET_IsInit() || GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_SELECT_POKEMON, POKETOOL_GetWorkSize(), pp)){
    _Pokemonset(pWork,0,pp);
#if PM_DEBUG
    if(!GFL_NET_IsInit()){
      _Pokemonset(pWork,1,pp);  //���������Z�b�g

      GFL_STD_MemCopy(pp,pWork->recvPoke[0],POKETOOL_GetWorkSize());
      GFL_STD_MemCopy(pp,pWork->recvPoke[1],POKETOOL_GetWorkSize());
      
    }
#endif
    bRet= TRUE;
  }
  GFL_HEAP_FreeMemory(pp);
  return bRet;
}


// �I�����܂����H�̖₢������������
static void _endRequestState(IRC_POKEMON_TRADE* pWork)
{
  TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_RETURN, FALSE);
  TOUCHBAR_SetActive( pWork->pTouchWork, TOUCHBAR_ICON_CUR_U, FALSE );

  if(GFL_NET_IsInit()){
    if( GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_END_REQ, 0, NULL)){
      _CHANGE_STATE(pWork,_noneState);
    }
  }
  else{
    _CHANGE_STATE(pWork,_recvEndReqFunc);
  }
}


#define _MOJITOP   (44)
#define _MOJILEFT  (4)


static GFL_UI_TP_HITTBL tp_mojidata[]={
  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT , _MOJILEFT+24},  //��
  { _MOJITOP+24  , _MOJITOP+24*2 , _MOJILEFT , _MOJILEFT+24},
  { _MOJITOP+24*2, _MOJITOP+24*3 , _MOJILEFT , _MOJILEFT+24},
  { _MOJITOP+24*3, _MOJITOP+24*4 , _MOJILEFT , _MOJILEFT+24},
  { _MOJITOP+24*4, _MOJITOP+24*5 , _MOJILEFT , _MOJILEFT+24},

  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT+24 , _MOJILEFT+24*2},  //��
  { _MOJITOP+24  , _MOJITOP+24*2 , _MOJILEFT+24 , _MOJILEFT+24*2},
  { _MOJITOP+24*2, _MOJITOP+24*3 , _MOJILEFT+24 , _MOJILEFT+24*2},
  { _MOJITOP+24*3, _MOJITOP+24*4 , _MOJILEFT+24 , _MOJILEFT+24*2},
  { _MOJITOP+24*4, _MOJITOP+24*5 , _MOJILEFT+24 , _MOJILEFT+24*2},

  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT+24*2 , _MOJILEFT+24*3},  //��
  { _MOJITOP+24  , _MOJITOP+24*2 , _MOJILEFT+24*2 , _MOJILEFT+24*3},
  { _MOJITOP+24*2, _MOJITOP+24*3 , _MOJILEFT+24*2 , _MOJILEFT+24*3},
  { _MOJITOP+24*3, _MOJITOP+24*4 , _MOJILEFT+24*2 , _MOJILEFT+24*3},
  { _MOJITOP+24*4, _MOJITOP+24*5 , _MOJILEFT+24*2 , _MOJILEFT+24*3},

  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT+24*3 , _MOJILEFT+24*4},  //��
  { _MOJITOP+24  , _MOJITOP+24*2 , _MOJILEFT+24*3 , _MOJILEFT+24*4},
  { _MOJITOP+24*2, _MOJITOP+24*3 , _MOJILEFT+24*3 , _MOJILEFT+24*4},
  { _MOJITOP+24*3, _MOJITOP+24*4 , _MOJILEFT+24*3 , _MOJILEFT+24*4},
  { _MOJITOP+24*4, _MOJITOP+24*5 , _MOJILEFT+24*3 , _MOJILEFT+24*4},
  
  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT+24*4 , _MOJILEFT+24*5},  //��
  { _MOJITOP+24  , _MOJITOP+24*2 , _MOJILEFT+24*4 , _MOJILEFT+24*5},
  { _MOJITOP+24*2, _MOJITOP+24*3 , _MOJILEFT+24*4 , _MOJILEFT+24*5},
  { _MOJITOP+24*3, _MOJITOP+24*4 , _MOJILEFT+24*4 , _MOJILEFT+24*5},
  { _MOJITOP+24*4, _MOJITOP+24*5 , _MOJILEFT+24*4 , _MOJILEFT+24*5},

  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT+24*5 , _MOJILEFT+24*6},  //��
  { _MOJITOP+24  , _MOJITOP+24*2 , _MOJILEFT+24*5 , _MOJILEFT+24*6},
  { _MOJITOP+24*2, _MOJITOP+24*3 , _MOJILEFT+24*5 , _MOJILEFT+24*6},
  { _MOJITOP+24*3, _MOJITOP+24*4 , _MOJILEFT+24*5 , _MOJILEFT+24*6},
  { _MOJITOP+24*4, _MOJITOP+24*5 , _MOJILEFT+24*5 , _MOJILEFT+24*6},

  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT+24*6 , _MOJILEFT+24*7},  //��
  { _MOJITOP+24  , _MOJITOP+24*2 , _MOJILEFT+24*6 , _MOJILEFT+24*7},
  { _MOJITOP+24*2, _MOJITOP+24*3 , _MOJILEFT+24*6 , _MOJILEFT+24*7},
  { _MOJITOP+24*3, _MOJITOP+24*4 , _MOJILEFT+24*6 , _MOJILEFT+24*7},
  { _MOJITOP+24*4, _MOJITOP+24*5 , _MOJILEFT+24*6 , _MOJILEFT+24*7},

  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT+24*7 , _MOJILEFT+24*8},  //��
  { _MOJITOP+24*2, _MOJITOP+24*3 , _MOJILEFT+24*7 , _MOJILEFT+24*8},
  { _MOJITOP+24*4, _MOJITOP+24*5 , _MOJILEFT+24*7 , _MOJILEFT+24*8},

  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT+24*8 , _MOJILEFT+24*9},  //��
  { _MOJITOP+24  , _MOJITOP+24*2 , _MOJILEFT+24*8 , _MOJILEFT+24*9},
  { _MOJITOP+24*2, _MOJITOP+24*3 , _MOJILEFT+24*8 , _MOJILEFT+24*9},
  { _MOJITOP+24*3, _MOJITOP+24*4 , _MOJILEFT+24*8 , _MOJILEFT+24*9},
  { _MOJITOP+24*4, _MOJITOP+24*5 , _MOJILEFT+24*8 , _MOJILEFT+24*9},

  { _MOJITOP     , _MOJITOP+24   , _MOJILEFT+24*9 , _MOJILEFT+24*10},  //��

//  _ENDTABLE,
  
  {GFL_UI_TP_HIT_END,0,0,0},
};


// ���������p�l���\����
static void _loopSearchMojiState(IRC_POKEMON_TRADE* pWork)
{
  BOOL endflg=FALSE;
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  {
    int ans = GFL_UI_TP_HitTrg(tp_mojidata);
    if(ans != GFL_UI_TP_HIT_NONE){
      pWork->selectMoji = ans + 1;
      endflg = TRUE;
    }
  }

  TOUCHBAR_Main(pWork->pTouchWork);
  switch( TOUCHBAR_GetTrg(pWork->pTouchWork )){
  case TOUCHBAR_ICON_RETURN:
    pWork->selectMoji = 0;
    endflg=TRUE;
    break;
  default:
    break;
  }
  if(GFL_UI_KEY_GetTrg()==PAD_BUTTON_CANCEL ){
    pWork->selectMoji = 0;
    endflg = TRUE;
  }

  if(endflg){
    OS_TPrintf("SELECTMOJI %d\n", pWork->selectMoji);
    IRC_POKETRADE_EndSubMojiBG(pWork);
    IRC_POKETRADE_MessageWindowClear(pWork);
    _CHANGE_STATE(pWork,_touchState);
  }

}



// ���������p�l���J�n
static void _startSearchMojiState(IRC_POKEMON_TRADE* pWork)
{
  IRC_POKETRADE_InitSubMojiBG(pWork);

  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_52, pWork->pMessageStrBuf );
  IRC_POKETRADE_MessageWindowOpen(pWork);

  TOUCHBAR_SetActive( pWork->pTouchWork, TOUCHBAR_ICON_CUR_U, FALSE );

  _CHANGE_STATE(pWork,_loopSearchMojiState);
}

//------------------------------------------------------------------------------
/**
 * @brief   Line�̒l����h�b�g�s�b�`�𓾂�
 * @param   line
 * @retval  none
 */
//------------------------------------------------------------------------------

static int _boxScrollLine2Num(int line)
{
  int num=0, pos, page;
  
  if(line==0){
    num = 0;
  }
  else if(line==1){
    num = _TEMOTITRAY_MAX/2;
  }
  else{
    pos = line - 2;
    page = pos / 6;
    pos = pos % 6;
    num = _TEMOTITRAY_MAX + _BOXTRAY_MAX*page + (_BOXTRAY_MAX/6)*pos;
  }
  return num;
}

//------------------------------------------------------------------------------
/**
 * @brief   ��ʂ̊O�ɃJ�[�\�������邩�ǂ���
 * @param   line
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _isCursorInScreen(IRC_POKEMON_TRADE* pWork, int line)
{
  int i=_boxScrollLine2Num(line);


  if((pWork->BoxScrollNum < i) && ((pWork->BoxScrollNum + 224) > i)){
    return TRUE;  //�͈͓�
  }
  i += _DOTMAX;
  if((pWork->BoxScrollNum < i) && ((pWork->BoxScrollNum + 224) > i)){
    return TRUE;
  }
  return FALSE;
}




//--------------------------------------------------------------------------------------------
/**
 * @brief ��{�̏\���L�[����
 * @param IRC_POKEMON_TRADE* ���[�N 
 */
//--------------------------------------------------------------------------------------------
static void _padUDLRFunc(IRC_POKEMON_TRADE* pWork)
{
  BOOL bChange=FALSE;
  int num;
  
  if(GFL_UI_KEY_GetRepeat()==PAD_KEY_UP ){   //�x�N�g�����Ď�
    pWork->MainObjCursorIndex--;
    if(pWork->MainObjCursorIndex<0){
      pWork->MainObjCursorIndex=0;
    }
    pWork->oldLine--;
    bChange=TRUE;
  }
  if(GFL_UI_KEY_GetRepeat()==PAD_KEY_DOWN ){   //�x�N�g�����Ď�
    pWork->MainObjCursorIndex++;
    if(pWork->MainObjCursorIndex >= BOX_VERTICAL_NUM){
      pWork->MainObjCursorIndex = BOX_VERTICAL_NUM-1;
    }
    pWork->oldLine--;
    bChange=TRUE;
  }
  if(GFL_UI_KEY_GetRepeat()==PAD_KEY_RIGHT ){   //�x�N�g�����Ď�
    pWork->MainObjCursorLine++;
    if(FALSE == _isCursorInScreen(pWork,pWork->MainObjCursorLine)){
      pWork->BoxScrollNum = _boxScrollLine2Num(pWork->MainObjCursorLine+1)-256;
    }
    if(pWork->MainObjCursorLine >= TRADEBOX_LINEMAX){
      pWork->MainObjCursorLine = 0;
    }
    pWork->oldLine--;
    bChange=TRUE;
  }
  if(GFL_UI_KEY_GetRepeat()==PAD_KEY_LEFT ){   //�x�N�g�����Ď�
    pWork->MainObjCursorLine--;
    if(FALSE == _isCursorInScreen(pWork,pWork->MainObjCursorLine)){
      pWork->BoxScrollNum = _boxScrollLine2Num(pWork->MainObjCursorLine);
    }
    if(pWork->MainObjCursorLine < 0){
      pWork->MainObjCursorLine = TRADEBOX_LINEMAX-1;
    }
    pWork->oldLine++;
    bChange=TRUE;
  }

  if(bChange){
    pWork->bgscrollRenew = TRUE;

    IRC_POKETRADE_TrayDisp(pWork);
    IRC_POKETRADE_SendScreenBoxNameChar(pWork);
    IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);

    
    //IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);
  }

  
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ��{�X�e�[�g������
 * @param IRC_POKEMON_TRADE* ���[�N 
 */
//--------------------------------------------------------------------------------------------
static void _touchState(IRC_POKEMON_TRADE* pWork)
{
  int i;


  if(pWork->pCatchCLWK){
    GFL_CLACT_WK_SetPos(pWork->pCatchCLWK, &pWork->aCatchOldPos, CLSYS_DRAW_SUB);
    GFL_CLACT_WK_SetDrawEnable( pWork->pCatchCLWK, TRUE);
    pWork->pCatchCLWK=NULL;
  }

  
  TOUCHBAR_SetVisible( pWork->pTouchWork, TOUCHBAR_ICON_CUR_U, TRUE );
  TOUCHBAR_SetActive( pWork->pTouchWork, TOUCHBAR_ICON_CUR_U, TRUE );
  TOUCHBAR_SetVisible( pWork->pTouchWork, TOUCHBAR_ICON_CUR_R, FALSE );
  TOUCHBAR_SetVisible( pWork->pTouchWork, TOUCHBAR_ICON_CUR_L, FALSE );
  TOUCHBAR_SetVisible( pWork->pTouchWork, TOUCHBAR_ICON_RETURN ,TRUE );
  GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], TRUE );

  IRC_POKETRADE_SendVramBoxNameChar(pWork); // �{�b�N�X��������
  IRC_POKETRADE_ReturnPageMarkDisp(pWork,APP_COMMON_BARICON_RETURN); //�}�[�N�\��
  IRC_POKETRADE_LeftPageMarkDisp(pWork,APP_COMMON_BARICON_CURSOR_UP); //�}�[�N�\��
  G2S_BlendNone();
  for(i=0;i<2;i++){
    pWork->bChangeOK[i]=FALSE;
  }
  _CHANGE_STATE(pWork,_touchStateCommon);

}

//--------------------------------------------------------------------------------------------
/**
 * @brief �^�b�`����ő��̃A�N�V�����ɔ�Ԋ�{�X�e�[�g
 * @param IRC_POKEMON_TRADE* ���[�N
 */
//--------------------------------------------------------------------------------------------
static void _touchStateCommon(IRC_POKEMON_TRADE* pWork)
{
  u32 x,y,i;
  GFL_CLACTPOS pos;

  int backBoxNo = pWork->nowBoxno;

  //�L�[����
  _padUDLRFunc(pWork);
  if(GFL_UI_KEY_GetTrg()== PAD_BUTTON_DECIDE){
    pWork->underSelectBoxno = LINE2TRAY(pWork->MainObjCursorLine);
    pWork->underSelectIndex = LINE2POKEINDEX(pWork->MainObjCursorLine,pWork->MainObjCursorIndex);
    IRC_POKETRADE_SetCursorXY(pWork);
    _CHANGE_STATE(pWork,_dispSubState);
    return;
  }

  
  if(GFL_UI_TP_GetPointCont(&x,&y)){ 
    // �p�l���X�N���[��
    if((x >=  64) && ((192) > x)){
      if((y >=  152+12) && ((176+12) > y)){
        if(pWork->touchON){
          pWork->BoxScrollNum -= (x - pWork->x)*2;
          if(0 > pWork->BoxScrollNum){
            pWork->BoxScrollNum+=TOTAL_DOT_MAX;
          }
          if(TOTAL_DOT_MAX <= pWork->BoxScrollNum){
            pWork->BoxScrollNum-=TOTAL_DOT_MAX;
          }
          IRC_POKETRADE_TrayDisp(pWork);
          IRC_POKETRADE_SendScreenBoxNameChar(pWork);
          IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);
          if(GFL_NET_IsInit()){
            // ���Ɏ��s���Ă����܂�Ȃ��ʐM ����Ɉʒu��m�点�Ă��邾��
            GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_SCROLLBAR,4,&pWork->BoxScrollNum);
          }
        }
      }
    }
    pWork->x = x;
    pWork->y = y;
    pWork->touchON = TRUE;
  }
  else{
    pWork->touchON = FALSE;
  }

  TOUCHBAR_Main(pWork->pTouchWork);
  switch( TOUCHBAR_GetTrg(pWork->pTouchWork )){
  case TOUCHBAR_ICON_RETURN:

    _CHANGE_STATE(pWork,_endRequestState);
    return;
  case TOUCHBAR_ICON_CUR_U:  //@todo �����\��
    _CHANGE_STATE(pWork,_startSearchMojiState);
    return;
  default:
    break;
  }

  if(IsTouchCLACTPosition(pWork)){
    _CHANGE_STATE(pWork,_dispSubState);
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR������
 *	@param	IRC_POKEMON_TRADE
 *	@return	none
 */
//-----------------------------------------------------------------------------

static void _ToolBarInit(IRC_POKEMON_TRADE* pWork)
{
	//�A�C�R���̐ݒ�
	//�������
	TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
	{	
		{	
			TOUCHBAR_ICON_RETURN,
			{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
		},
		{
			TOUCHBAR_ICON_CUR_L,
			{	TOUCHBAR_ICON_X_00, TOUCHBAR_ICON_Y },
		},
		{
			TOUCHBAR_ICON_CUR_R,
			{	TOUCHBAR_ICON_X_00, TOUCHBAR_ICON_Y },
		},
		{	//@todo �����\��
			TOUCHBAR_ICON_CUR_U,
			{	TOUCHBAR_ICON_X_00, TOUCHBAR_ICON_Y },
		},
	};

	TOUCHBAR_SETUP	touchbar_setup;
	GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

	touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
	touchbar_setup.p_unit		= pWork->cellUnit;										//OBJ�ǂݍ��݂̂��߂�CLUNIT
  touchbar_setup.is_notload_bg =TRUE;  //BG�͂Ȃ�
  touchbar_setup.bar_frm	= GFL_BG_FRAME0_S;						//BG�ǂݍ��݂̂��߂�BG�ʏ㉺��ʔ���ɂ��K�v
	touchbar_setup.bg_plt		= 0;			//BG��گ�
	touchbar_setup.obj_plt	= _OBJPLT_COMMON;			//OBJ��گ�
	touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;	//�}�b�s���O���[�h

  
  pWork->pTouchWork = TOUCHBAR_Init(&touchbar_setup, pWork->heapID);
  TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_CUR_L, FALSE);
  TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_CUR_R, FALSE);

}


//------------------------------------------------------------------------------
/**
 * @brief   ��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _dispInit(IRC_POKEMON_TRADE* pWork)
{

  GFL_BG_Init( pWork->heapID );
  GFL_BMPWIN_Init( pWork->heapID );
  GFL_FONTSYS_Init();

  IRC_POKETRADE_SetMainDispGraphic(pWork);
  IRC_POKETRADE_SetSubDispGraphic(pWork);
  IRC_POKETRADE_3DGraphicSetUp( pWork );


  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);

  //�����n������
  pWork->pWordSet    = WORDSET_Create( pWork->heapID );
  pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_poke_trade_dat, pWork->heapID );
  pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
  pWork->pStrBuf = GFL_STR_CreateBuffer( 128, pWork->heapID );
  pWork->pExStrBuf = GFL_STR_CreateBuffer( 128, pWork->heapID );
  pWork->pMessageStrBuf = GFL_STR_CreateBuffer( 128, pWork->heapID );
  pWork->pMessageStrBufEx = GFL_STR_CreateBuffer( 128, pWork->heapID );


  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

  //�Z���n�V�X�e���̍쐬
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 240 , 0 , pWork->heapID );


  IRC_POKETRADE_InitBoxCursor(pWork);


  IRC_POKETRADE_CreatePokeIconResource(pWork);

  IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);  //�|�P�����̕\��
  IRC_POKETRADE_SendVramBoxNameChar(pWork); // �{�b�N�X��������

  IRC_POKETRADE_GraphicInitSubDisp(pWork);  //BG��
  IRC_POKETRADE_GraphicInitMainDisp(pWork);


#if 1
  pWork->mcssSys = MCSS_Init( 2 , pWork->heapID );
  MCSS_SetTextureTransAdrs( pWork->mcssSys , 0x30000 );
  MCSS_SetOrthoMode( pWork->mcssSys );
#endif


  GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );

  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, _BRIGHTNESS_SYNC);

  GFL_NET_ReloadIcon();
  _ToolBarInit(pWork);

  _CHANGE_STATE(pWork, _touchState);
}


//--------------------------------------------------------------
/**
 * G3D VBlank����
 * @param TCB GFL_TCB
 * @param work tcb work
 * @retval nothing
 */
//--------------------------------------------------------------
static void	_VBlank( GFL_TCB *tcb, void *work )
{
  IRC_POKEMON_TRADE *pWork=work;

  GFL_CLACT_SYS_VBlankFunc();	//�Z���A�N�^�[VBlank

  if(pWork->bgscrollRenew){

    GFL_BG_SetScroll(GFL_BG_FRAME2_S,GFL_BG_SCROLL_X_SET, pWork->bgscroll);
    GFL_BG_SetScroll(GFL_BG_FRAME1_S,GFL_BG_SCROLL_X_SET, pWork->bgscroll);
    pWork->bgscrollRenew=FALSE;
  }
}

FS_EXTERN_OVERLAY(ui_common);


//------------------------------------------------------------------------------
/**
 * @brief        PROC�X�^�[�g
 * @param pwk    EVENT_IRCBATTLE_WORK���K�v
 * @retval       none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleFriendProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  int i;
  EVENT_IRCBATTLE_WORK* pParentWork = pwk;
  
	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x110000 );
  GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
  {
    IRC_POKEMON_TRADE *pWork = GFL_PROC_AllocWork( proc, sizeof( IRC_POKEMON_TRADE ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(IRC_POKEMON_TRADE));
    pWork->heapID = HEAPID_IRCBATTLE;

    if(pParentWork){
      GAMEDATA* pGameData = GAMESYSTEM_GetGameData(IrcBattle_GetGAMESYS_WORK(pParentWork));
      pWork->pGameData=pGameData;
      pWork->pBox = GAMEDATA_GetBoxManager(pGameData);
      pWork->pMy = GAMEDATA_GetMyStatus( pGameData );
      pWork->pMyParty = GAMEDATA_GetMyPokemon(pGameData);
    }
#if PM_DEBUG
    else{
      pWork->pBox = BOX_DAT_InitManager(pWork->heapID,SaveControl_GetPointer());
      pWork->pMy = MyStatus_AllocWork(pWork->heapID);

      {
        POKEMON_PARAM *pp;
        int i,j;
        BOX_MANAGER* pBox = pWork->pBox;
        pp = PP_Create(MONSNO_ONOKKUSU, 100, 123456, HEAPID_IRCBATTLE);

        for(i=0;i<24;i++){
          for(j=0;j < 2;j++){
            u16 oyaName[5] = {L'�f',L'�o',L'�b',L'�O',0xFFFF};
            POKEMON_PERSONAL_DATA* ppd = POKE_PERSONAL_OpenHandle(MONSNO_MARIRU+i, 0, GFL_HEAPID_APP);
            u32 ret = POKE_PERSONAL_GetParam(ppd,POKEPER_ID_sex);

            PP_SetupEx(pp, MONSNO_RIZAADON, i+j, 123456,PTL_SETUP_POW_AUTO, ret);
            PP_Put( pp , ID_PARA_oyaname_raw , (u32)oyaName );
            PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex(  pWork->pMy ) );
            PP_Put( pp , ID_PARA_item , 18 );
            PP_Put( pp , ID_PARA_pokerus , 1 );
            
            BOXDAT_PutPokemonBox(pBox, i, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pp));
            
            POKE_PERSONAL_CloseHandle(ppd);
          }
        }
        GFL_HEAP_FreeMemory(pp);
      }

      pWork->pMyParty = PokeParty_AllocPartyWork(pWork->heapID);
    }
#endif

    // �ʐM�e�[�u���ǉ�
    if(GFL_NET_IsInit()){
      GFL_NET_AddCommandTable(GFL_NET_CMD_IRCTRADE,_PacketTbl,NELEMS(_PacketTbl), pWork);
    }
    pWork->g3dVintr = GFUser_VIntr_CreateTCB( _VBlank, (void*)pWork, 0 );

    pWork->pCatchCLWK = NULL;
    pWork->selectIndex = -1;


    pWork->recvPoke[0] = GFL_HEAP_AllocClearMemory(pWork->heapID, POKETOOL_GetWorkSize());
    pWork->recvPoke[1] = GFL_HEAP_AllocClearMemory(pWork->heapID, POKETOOL_GetWorkSize());


    // ����ʃE�C���h�E�V�X�e��������
    pWork->TouchSubWindowSys = TOUCH_SW_AllocWork( pWork->heapID );

    for(i=0;i<2;i++){
      pWork->bChangeOK[i]=FALSE;
    }

    pWork->SysMsgQue = PRINTSYS_QUE_Create( pWork->heapID );
    //pWork->SysMsgQue���F������ɋL������̂ŁA�ŏ��ɋL�����������F�����Z�b�g
    GFL_FONTSYS_SetColor(1, 2, 15);

    pWork->pMsgTcblSys = GFL_TCBL_Init( pWork->heapID , pWork->heapID , 2 , 0 );

    _dispInit(pWork);

    IRC_POKETRADEDEMO_Init(pWork);
    DEBUGWIN_InitProc( GFL_BG_FRAME3_M , pWork->pFontHandle );
    DEBUG_PAUSE_SetEnable( TRUE );
  }
  
  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT IrcBattleFriendProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  IRC_POKEMON_TRADE* pWork = mywk;
  GFL_PROC_RESULT retCode = GFL_PROC_RES_FINISH;
  StateFunc* state = pWork->state;

  if(DEBUGWIN_IsActive()){
    return GFL_PROC_RES_CONTINUE;
  }
    
  
  if(state != NULL){
    state(pWork);
    pWork->anmCount++;
    retCode = GFL_PROC_RES_CONTINUE;
  }
  //�|�P�����Z�b�g���R�[��
  if(pWork->pokemonsetCall != 0){
    _Pokemonset(pWork, 1, pWork->recvPoke[pWork->pokemonsetCall-1]);
    pWork->pokemonsetCall=0;
  }

  GFL_CLACT_SYS_Main(); // CLSYS���C��
  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }
  if(pWork->mesWin){
    GFL_FONTSYS_SetColor(1, 2, 15);
  }
  GFL_TCBL_Main( pWork->pMsgTcblSys );

  
  PRINTSYS_QUE_Main(pWork->SysMsgQue);


  GFL_G3D_DRAW_Start();
  GFL_G3D_CAMERA_Switching( pWork->pCamera );

  GFL_G3D_DRAW_SetLookAt();

  MCSS_Main( pWork->mcssSys );
  MCSS_Draw( pWork->mcssSys );
  //IRC_POKETRADE_G3dDraw(pWork);

  IRC_POKETRADEDEMO_Main(pWork);

  GFL_G3D_DRAW_End();


  //	ConnectBGPalAnm_Main(&pWork->cbp);

  return retCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleFriendProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  int i;
  IRC_POKEMON_TRADE* pWork = mywk;
  EVENT_IRCBATTLE_WORK* pParentWork = pwk;

  DEBUGWIN_ExitProc();
  IRC_POKETRADE_ItemIconReset(&pWork->aItemMark);
  IRC_POKETRADE_ItemIconReset(&pWork->aPokerusMark);
  TOUCHBAR_Exit(pWork->pTouchWork);

  if(GFL_NET_IsInit()){
    GFL_NET_DelCommandTable(GFL_NET_CMD_IRCTRADE);
  }
  TOUCH_SW_FreeWork( pWork->TouchSubWindowSys );

  if(pWork->MessageWin){
    GFL_BMPWIN_Delete(pWork->MessageWin);
  }
  if(pWork->MyInfoWin){
    GFL_BMPWIN_Delete(pWork->MyInfoWin);
  }

  if(pWork->mesWin){
    GFL_BMPWIN_Delete(pWork->mesWin);
  }

  for(i=0;i<2;i++){
    if(pWork->StatusWin[i]){
      GFL_BMPWIN_Delete(pWork->StatusWin[i]);
    }
  }
  if(pWork->pMsgData){
    GFL_MSG_Delete(pWork->pMsgData);
  }
  WORDSET_Delete(pWork->pWordSet);
  GFL_FONT_Delete(	pWork->pFontHandle );
  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  GFL_STR_DeleteBuffer(pWork->pExStrBuf);
  GFL_STR_DeleteBuffer(pWork->pMessageStrBuf);
  GFL_STR_DeleteBuffer(pWork->pMessageStrBufEx);


  GFL_G3D_CAMERA_Delete(pWork->pCamera);
  GFL_TCBL_Exit(pWork->pMsgTcblSys);

  for(i=0;i<PLT_RESOURCE_MAX;i++){
    if(pWork->cellRes[i]!=0){
      GFL_CLGRP_PLTT_Release(pWork->cellRes[i]);
    }
  }
  for( ;i<CHAR_RESOURCE_MAX;i++){
    if(pWork->cellRes[i]!=0){
      GFL_CLGRP_CGR_Release(pWork->cellRes[i]);
    }
  }
  for( ;i<ANM_RESOURCE_MAX;i++){
    if(pWork->cellRes[i]!=0){
      GFL_CLGRP_CELLANIM_Release(pWork->cellRes[i]);
    }
  }

  IRC_POKETRADE_GraphicExit(pWork);

  for(i=0;i<2;i++){
    if( pWork->pokeMcss[i] ){
      MCSS_Del(pWork->mcssSys,pWork->pokeMcss[i]);
      pWork->pokeMcss[i] = NULL;
    }
  }
  MCSS_Exit(pWork->mcssSys);
  IRC_POKETRADEDEMO_End(pWork);

  IRC_POKETRADE_AllDeletePokeIconResource(pWork);


  for(i = 0;i< CELL_DISP_NUM;i++){
    if(pWork->curIcon[i]){
      GFL_CLACT_WK_Remove(pWork->curIcon[i]);
    }
  }
  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  IRC_POKETRADE_ResetBoxNameWindow(pWork);

  
  //GFL_NET_Exit(NULL);  //@@OO �{���͂����ŌĂ΂Ȃ�
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);

  GFL_CLACT_SYS_Delete();
  GFL_G3D_Exit();
  GFL_HEAP_FreeMemory(pWork->recvPoke[0]);
  GFL_HEAP_FreeMemory(pWork->recvPoke[1]);

  GFL_TCB_DeleteTask( pWork->g3dVintr );
  GFL_BG_Exit();
  GFL_BMPWIN_Exit();

  GFL_PROC_FreeWork(proc);
  GFL_HEAP_DeleteHeap(HEAPID_IRCBATTLE);

  if(pParentWork){
    EVENT_IrcBattle_SetEnd(pParentWork);
  }
#if PM_DEBUG
  else{
    BOX_DAT_ExitManager(pWork->pBox);
    GFL_HEAP_FreeMemory(pWork->pMy);
    GFL_HEAP_FreeMemory(pWork->pMyParty);
  }
#endif

	//�I�[�o�[���C�j��
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA IrcPokemonTradeProcData = {
  IrcBattleFriendProcInit,
  IrcBattleFriendProcMain,
  IrcBattleFriendProcEnd,
};


