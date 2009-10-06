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
#include "ircbattle.naix"
#include "trade.naix"
#include "net_app/connect_anm.h"
#include "net/dwc_rapfriend.h"
#include "savedata/wifilist.h"

#include "system/touch_subwindow.h"

#include "poke_tool/poke_tool_def.h"
#include "box_m_obj_NANR_LBLDEFS.h"
#include "p_st_obj_d_NANR_LBLDEFS.h"

#include "ircpokemontrade_local.h"


#include "poke_tool/status_rcv.h"




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

}

//--------------------------------------------------------------
//	�|�P����MCSS�쐬
//--------------------------------------------------------------
void IRCPOKETRADE_PokeCreateMcss( IRC_POKEMON_TRADE *pWork ,int no, int bFront, const POKEMON_PARAM *pp )
{
  MCSS_ADD_WORK addWork;
  VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};
  int xpos[] = { PSTATUS_MCSS_POS_X1 , PSTATUS_MCSS_POS_X2};

  GF_ASSERT( pWork->pokeMcss[no] == NULL );

  if(bFront){
    MCSS_TOOL_MakeMAWPP( pp , &addWork , MCSS_DIR_FRONT );
  }
  else{
    MCSS_TOOL_MakeMAWPP( pp , &addWork , MCSS_DIR_BACK );
  }
  pWork->pokeMcss[no] = MCSS_Add( pWork->mcssSys , xpos[no] , PSTATUS_MCSS_POS_Y , 1000 , &addWork );
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

//--------------------------------------------------------------
//	�|�P������ԕ��̓Z�b�g
//--------------------------------------------------------------

static void _setPokemonStatusMessage(IRC_POKEMON_TRADE *pWork, int side,const POKEMON_PARAM* pp)
{


  int i=0,lv;
  int frame = GFL_BG_FRAME3_M;
  int sidew = side*4;

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

  if(!pWork->StatusWin[sidew]){
    int sidex[] = {5, 21};
    pWork->StatusWin[sidew] = GFL_BMPWIN_Create(frame,	sidex[side], 1, 9, 2,	_BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
    pWork->StatusWin[sidew+1] = GFL_BMPWIN_Create(frame,	sidex[side], 3, 9, 2,	_BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
  }

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->StatusWin[sidew]), 0);
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->StatusWin[sidew+1]), 0);
  GFL_BMPWIN_MakeScreen(pWork->StatusWin[sidew]);
  GFL_BMPWIN_MakeScreen(pWork->StatusWin[sidew+1]);

  PP_Get(pp, ID_PARA_nickname, pWork->pStrBuf);

  GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->StatusWin[sidew]), 0, 1, pWork->pStrBuf, pWork->pFontHandle);


  lv=	PP_Get(pp, ID_PARA_level, NULL);

  GFL_MSG_GetString(  pWork->pMsgData, POKETRADE_STR_21, pWork->pExStrBuf );
  WORDSET_RegisterNumber(pWork->pWordSet, 0, lv,
                         3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);

  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->StatusWin[sidew+1]), 0, 1, pWork->pStrBuf, pWork->pFontHandle);



  GFL_BMPWIN_TransVramCharacter(pWork->StatusWin[sidew]);
  GFL_BMPWIN_TransVramCharacter(pWork->StatusWin[sidew+1]);

  GFL_BG_LoadScreenV_Req( frame );
  //OS_TPrintf("�|�P���������\��\n");

}


//--------------------------------------------------------------
//	�|�P�����\���Z�b�g
//--------------------------------------------------------------
static void _Pokemonset(IRC_POKEMON_TRADE *pWork, int side, const POKEMON_PARAM* pp )
{
  IRCPOKETRADE_PokeDeleteMcss(pWork, side);

  IRCPOKETRADE_PokeCreateMcss(pWork, side, 1-side, pp);
  
  pWork->bPokemonSet[side]=TRUE;

  _setPokemonStatusMessage(pWork,side ,pp);
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
  //OS_TPrintf("���b�Z�[�W\n");

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


BOOL IRCPOKEMONTRADE_GetPokeDataAddress(BOX_DATA* boxData , int trayNo, int index,IRC_POKEMON_TRADE* pWork,POKEMON_PARAM* pp)
{
  POKEMON_PASO_PARAM* ppp;
  if(trayNo!=BOX_MAX_TRAY){
    OS_TPrintf("traynum %d \n",trayNo);
    ppp = BOXDAT_GetPokeDataAddress(boxData,trayNo,index);
    if(ppp){
      POKEMON_PARAM* crpp = PP_CreateByPPP(ppp,pWork->heapID );
      PP_Renew(crpp);
      GFL_STD_MemCopy(crpp,pp,POKETOOL_GetWorkSize());
      GFL_HEAP_FreeMemory(crpp);
      return TRUE;
    }
  }
  else{
    POKEPARTY* party = pWork->pMyParty;
    if(index < PokeParty_GetPokeCount(party)){
      GFL_STD_MemCopy( PokeParty_GetMemberPointer( party , index ), pp, POKETOOL_GetWorkSize());
      return TRUE;
    }
  }
  return FALSE;

}



//------------------------------------------------------------------------------
/**
 * @brief   ����ʃ|�P�����\��
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _InitBoxName( BOX_DATA* boxData , u8 trayNo ,IRC_POKEMON_TRADE* pWork )
{


  {//�{�b�N�X���\��
    if(pWork->MyInfoWin){
      GFL_BMPWIN_Delete(pWork->MyInfoWin);
    }

    pWork->MyInfoWin = GFL_BMPWIN_Create(GFL_BG_FRAME1_S, 0x07, 0x01, 10, 2, _BUTTON_MSG_PAL,  GFL_BMP_CHRAREA_GET_B );

    if(trayNo == BOX_MAX_TRAY){
      GFL_MSG_GetString(  pWork->pMsgData, POKETRADE_STR_19, pWork->pStrBuf );
    }
    else{
      BOXDAT_GetBoxName(boxData, trayNo, pWork->pStrBuf);
    }
    GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 0 );
    GFL_BMPWIN_MakeScreen(pWork->MyInfoWin);
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 1, 0, pWork->pStrBuf, pWork->pFontHandle);
    GFL_BMPWIN_TransVramCharacter(pWork->MyInfoWin);
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );

  }

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

  //�|�P�����Z�b�g���R�[��
  pWork->pokemonsetCall = netID+1;

  //	_Pokemonset(pWork, 1, pWork->recvPoke[netID]);

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

  _msgWindowCreate(pWork, POKETRADE_STR_20);

  TouchYesNoStart(pWork->TouchSubWindowSys);
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
    TOUCH_SW_Reset( pWork->TouchSubWindowSys );
    pWork->bTouchReset=FALSE;
  }
  _messageDelete(pWork);

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
  _msgWindowCreate(pWork, POKETRADE_STR_22);
  TouchYesNoStart(pWork->TouchSubWindowSys);
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


//�����I��ʐM���M
static void _changePokemonSendDataNetwork(IRC_POKEMON_TRADE* pWork)
{
  // �ʐM���ĂȂ���ΐi��
  if( !GFL_NET_IsInit() || GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_CHANGE_POKEMON,0,NULL)){

    //���肩��̕Ԏ���҂̂Ń��b�Z�[�W�͑ҋ@��
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_09, pWork->pMessageStrBuf );
    IRC_POKETRADE_MessageWindowOpen(pWork,  POKETRADE_STR_09);
    _CHANGE_STATE(pWork,_changeWaitState);
  }
}



// �|�P�����̃X�e�[�^�X�\���҂�
static void _pokemonStatusWait(IRC_POKEMON_TRADE* pWork)
{

  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
    IRCPOKETRADE_PokeDeleteMcss(pWork,0);
    IRCPOKETRADE_PokeDeleteMcss(pWork,1);
    GFL_BMPWIN_ClearScreen(pWork->MyInfoWin);
    GFL_BMPWIN_Delete(pWork->MyInfoWin);
    pWork->MyInfoWin=NULL;
  	_Pokemonset(pWork, 0, pWork->recvPoke[0]);
  	_Pokemonset(pWork, 1, pWork->recvPoke[1]);
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );
    _CHANGE_STATE(pWork, _networkFriendsStandbyWait2);
  }
}


// �|�P�����̃X�e�[�^�X�\��
static void _pokemonStatusStart(IRC_POKEMON_TRADE* pWork)
{
  int i,num,num2;
  POKEMON_PARAM* pp = pWork->recvPoke[0];

  IRCPOKETRADE_PokeDeleteMcss(pWork,1);  //����̂�����

  if(1){//�����̈ʒu����
    VecFx32 apos;
    MCSS_GetPosition( pWork->pokeMcss[0] ,&apos );
    apos.x = _MCSS_POS_X(190);
    apos.y = _MCSS_POS_Y(100);
    apos.z = 1000;
    MCSS_SetPosition( pWork->pokeMcss[0] ,&apos );
  }
  if(pWork->MyInfoWin){
    GFL_BMPWIN_Delete(pWork->MyInfoWin);
  }
  GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );
  pWork->MyInfoWin =
    GFL_BMPWIN_Create(GFL_BG_FRAME3_M,
                      1, 1, 30 , 24, _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);

  {
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_30, pWork->pExStrBuf );
    WORDSET_RegisterPokeNickName( pWork->pWordSet, 0,  pp );
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 16, 0, pWork->pStrBuf, pWork->pFontHandle);
  }
  {
    int lv;
  
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_21, pWork->pExStrBuf );
    lv=	PP_Get(pWork->recvPoke[0], ID_PARA_level, NULL);

    WORDSET_RegisterNumber(pWork->pWordSet, 0, lv, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 12*8, 0, pWork->pStrBuf, pWork->pFontHandle);
  }


  for(i=0;i<6;i++){//HP�|���΂₳
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_31+i, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 0, 3*8+16*i, pWork->pStrBuf, pWork->pFontHandle);
  }

  for(i=0;i<3;i++){//��������-��������
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_37+i, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 0, 16*8+16*i, pWork->pStrBuf, pWork->pFontHandle);
  }

  //HP HPMax
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_40, pWork->pExStrBuf );
  num=	PP_Get(pWork->recvPoke[0], ID_PARA_hp, NULL);
  num2=	PP_Get(pWork->recvPoke[0], ID_PARA_hpmax, NULL);
  WORDSET_RegisterNumber(pWork->pWordSet, 0, num, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
  WORDSET_RegisterNumber(pWork->pWordSet, 1, num2, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf);
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 8*7, 3*8, pWork->pStrBuf, pWork->pFontHandle);

  
  for(i=0;i<5;i++){//���������|���΂₳
    int paras[]={
      ID_PARA_pow,                //�U����
      ID_PARA_def,                //�h���
      ID_PARA_spepow,               //���U
      ID_PARA_spedef,               //���h
      ID_PARA_agi,                //�f����
    };
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_41, pWork->pExStrBuf );
    num=	PP_Get(pWork->recvPoke[0], paras[i], NULL);
    WORDSET_RegisterNumber(pWork->pWordSet, 0, num, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 8*9, 5*8+16*i, pWork->pStrBuf, pWork->pFontHandle);
  }
#if 1
  { //��������
    GFL_MSGDATA *pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,  NARC_message_chr_dat, pWork->heapID );
    num=	PP_GetSeikaku(pWork->recvPoke[0]);
    GFL_MSG_GetString( pMsgData, seikaku00_msg+num, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 9*8, 16*8, pWork->pStrBuf, pWork->pFontHandle);
    GFL_MSG_Delete(pMsgData);
  }
#endif
  { //�Ƃ�����
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_43, pWork->pExStrBuf );
    num=	PP_Get(pWork->recvPoke[0], ID_PARA_speabino, NULL);
    WORDSET_RegisterTokuseiName( pWork->pWordSet, 0,  num );
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 9*8, 18*8, pWork->pStrBuf, pWork->pFontHandle);
  }
  { //��������
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_44, pWork->pExStrBuf );
    num=	PP_Get(pWork->recvPoke[0], ID_PARA_item, NULL);
    WORDSET_RegisterItemName( pWork->pWordSet, 0,  num );
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 9*8, 20*8, pWork->pStrBuf, pWork->pFontHandle);
  }
  { //�킴
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_45, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 20*8, 13*8, pWork->pStrBuf, pWork->pFontHandle);
  }

  

  for(i=0;i<4;i++){//�킴
    num=	PP_Get(pWork->recvPoke[0], ID_PARA_waza1+i, NULL);
    if(num==0){
      continue;
    }
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_46, pWork->pExStrBuf );
    WORDSET_RegisterWazaName(pWork->pWordSet, 0,  num);
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 20*8, 15*8+i*16, pWork->pStrBuf, pWork->pFontHandle);
  }

  
  
  GFL_BMPWIN_MakeScreen(pWork->MyInfoWin);
  GFL_BMPWIN_TransVramCharacter(pWork->MyInfoWin);
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );


  _CHANGE_STATE(pWork, _pokemonStatusWait);

}


//�����I��҂�
static void _changePokemonSendData(IRC_POKEMON_TRADE* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
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
        POKEMON_PARAM* pp = pWork->recvPoke[i];
        WORDSET_RegisterPokeNickName( pWork->pWordSet, i,  pp );
      }
      WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx  );


      IRC_POKETRADE_MessageWindowOpen(pWork,  POKETRADE_STR_20);

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
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;

    if(selectno==0){
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_09, pWork->pMessageStrBuf );

      IRC_POKETRADE_MessageWindowOpen(pWork,  POKETRADE_STR_09);

      _CHANGE_STATE(pWork, _pokeSendDataState);

    }
    else{
      _CHANGE_STATE(pWork, _touchState);
    }
  }
}


//��������ɂ����A���j���[�\��
static void _changeMenuOpen(IRC_POKEMON_TRADE* pWork)
{

  if( _PokemonsetAndSendData(pWork) )
  {
    int msg[]={POKETRADE_STR_01,POKETRADE_STR_02};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
    _CHANGE_STATE(pWork, _changeMenuWait);
  }
}



//������ʂɃX�e�[�^�X�\�� ����Ɍ����邩�ǂ����҂�
static void _dispSubStateWait(IRC_POKEMON_TRADE* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      //����Ɍ�����
      pWork->selectIndex = pWork->underSelectIndex;
      pWork->selectBoxno = pWork->underSelectBoxno;
      _CHANGE_STATE(pWork, _changeMenuOpen);
    }
    else{
      _CHANGE_STATE(pWork, _touchState);
    }
    IRC_POKETRADE_SubStatusEnd(pWork);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
  }


}


// �����̉�ʂɃX�e�[�^�X��\��

static void _dispSubState(IRC_POKEMON_TRADE* pWork)
{
  {
    int msg[]={POKETRADE_STR_03,POKETRADE_STR_02};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
  }
  IRC_POKETRADE_SubStatusInit(pWork,pWork->x);
  _CHANGE_STATE(pWork,_dispSubStateWait);
}


static void _messageWaitState(IRC_POKEMON_TRADE* pWork)
{
  if(GFL_UI_TP_GetTrg()){
    _messageDelete(pWork);
    _CHANGE_STATE(pWork, _touchState);
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
  _CHANGE_STATE(pWork,IRC_POKMEONTRADE_STEP_ChangeDemo_PokeMove);
}

void IRC_POKMEONTRADE_ChangeFinish(IRC_POKEMON_TRADE* pWork)
{
  int id = 1-GFL_NET_SystemGetCurrentID();
  //��������
  // ����̃|�P�������̑I��ł����ꏊ�ɓ����
  if(pWork->selectBoxno == BOX_MAX_TRAY){ //�������̂̌����̏ꍇ
    POKEPARTY* party = pWork->pMyParty;

    PokeParam_RecoverAll(pWork->recvPoke[id]);

    PokeParty_SetMemberData(party, pWork->selectIndex, pWork->recvPoke[id]);
  }
  else{
    // @todo  ���[������������{�b�N�X��
    // @todo  ���[���{�b�N�X�����^���Ȃ�폜������
    
    BOXDAT_PutPokemonPos(pWork->pBox, pWork->selectBoxno, pWork->selectIndex, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pWork->recvPoke[id]));
  }
  pWork->selectBoxno = 0;
  pWork->selectIndex = -1;
  pWork->pCatchCLWK = NULL;
  IRCPOKETRADE_PokeDeleteMcss(pWork, 0);
  IRCPOKETRADE_PokeDeleteMcss(pWork, 1);

  {
    int i;
    for(i = 0;i< CUR_NUM;i++){
      if(pWork->curIcon[i]){
        GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[i], TRUE );
      }
    }
  }
  
  _InitBoxName(pWork->pBox,pWork->nowBoxno,pWork);  //�ĕ`��

  IRC_POKETRADE_MessageWindowClear(pWork);
  _msgWindowCreate(pWork, POKETRADE_STR_23);

  pWork->bPokemonSet[0]=FALSE;
  pWork->bPokemonSet[1]=FALSE;

  _CHANGE_STATE(pWork, _messageWaitState);
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
}



static BOOL _PokemonsetAndSendData(IRC_POKEMON_TRADE* pWork)
{ //�I���|�P�����\��
  POKEMON_PARAM* pp = GFL_HEAP_AllocClearMemory(pWork->heapID,POKETOOL_GetWorkSize());
  BOOL bRet = FALSE;

  GF_ASSERT(IRCPOKEMONTRADE_GetPokeDataAddress(pWork->pBox, pWork->selectBoxno, pWork->selectIndex,pWork, pp));

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
  if(GFL_NET_IsInit()){
    if( GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_END_REQ, 0, NULL)){
      _CHANGE_STATE(pWork,_noneState);
    }
  }
  else{
    _CHANGE_STATE(pWork,_recvEndReqFunc);
  }
}

static void _touchState(IRC_POKEMON_TRADE* pWork)
{
  u32 x,y,i;
  GFL_CLACTPOS pos;

  int backBoxNo = pWork->nowBoxno;

  for(i=0;i<2;i++){
    pWork->bChangeOK[i]=FALSE;
  }


  if(GFL_UI_TP_GetPointCont(&x,&y)){   //�x�N�g�����Ď�
    // ���������
    pWork->bUpVec = FALSE;
    if(pWork->y > y){
      pWork->bUpVec = TRUE;
    }
    // �p�l���X�N���[��
    if((x >=  64) && ((192) > x)){
      if((y >=  152) && ((176) > y)){
        if(pWork->touckON){
          pWork->BoxScrollNum -= (x - pWork->x)*2;
          if(0 > pWork->BoxScrollNum){
            pWork->BoxScrollNum+=TOTAL_DOT_MAX;
          }
          if(TOTAL_DOT_MAX <= pWork->BoxScrollNum){
            pWork->BoxScrollNum-=TOTAL_DOT_MAX;
          }
          IRC_POKETRADE_TrayDisp(pWork);
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
    pWork->touckON = TRUE;
  }
  else{
    pWork->touckON = FALSE;
  }


#if 1  //  ��ʃ^�b�`�g���K���̏���
  if(GFL_UI_TP_GetPointTrg(&x, &y)==TRUE){
    if((x >=(256-24)) && (256 > x)){  //�I���{�^��������ꍇ
      if((y >= (192-24)) && (192 > y)){
        _CHANGE_STATE(pWork,_endRequestState);
        return;
      }
    }

    //		if(backBoxNo !=  pWork->nowBoxno){
    //		IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);
    //	_InitBoxName(pWork->pBox,pWork->nowBoxno,pWork);
    //		}
    pWork->workBoxno=-1;
    pWork->workPokeIndex=-1;
    pWork->pCatchCLWK = IRC_POKETRADE_GetCLACT(pWork,x,y, &pWork->workBoxno, &pWork->workPokeIndex);
    if(pWork->pCatchCLWK){
      OS_TPrintf("GET %d %d\n",pWork->workBoxno,pWork->workPokeIndex);
    }
  }
#endif


  if(GFL_UI_TP_GetCont()==FALSE){ //�^�b�`�p�l���𗣂���

    if((pWork->pCatchCLWK != NULL) && pWork->bUpVec){ //�|�P��������ɓo�^
      GFL_CLACT_WK_SetDrawEnable( pWork->pCatchCLWK, FALSE);

      pWork->selectIndex = pWork->workPokeIndex;
      pWork->selectBoxno = pWork->workBoxno;
      pWork->workPokeIndex = 0;
      pWork->workBoxno = 0;

      _CHANGE_STATE(pWork,_changeMenuOpen);

      
    }
    else if(pWork->pCatchCLWK){  //�L���b�`���Ă邪��x�N�g���͓����Ȃ������ꍇ �����̉�ʂɃX�e�[�^�X��\������
      pWork->underSelectBoxno  = pWork->workBoxno;
      pWork->underSelectIndex = pWork->workPokeIndex;
      _CHANGE_STATE(pWork,_dispSubState);
    }
    pWork->workPokeIndex = 0;
    pWork->workBoxno = 0;
    pWork->pCatchCLWK = NULL;
  }

  if(pWork->pCatchCLWK != NULL){  //�^�b�`�p�l�����쒆�̃A�C�R���ړ�
    if(GFL_UI_TP_GetPointCont(&x,&y)){
      pos.x = x;
      pos.y = y;
      GFL_CLACT_WK_SetPos( pWork->pCatchCLWK, &pos, CLSYS_DRAW_SUB);
    }
  }



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
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 120 , 0 , pWork->heapID );


  IRC_POKETRADE_InitBoxCursor(pWork);


  IRC_POKETRADE_CreatePokeIconResource(pWork);

  IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);  //�|�P�����̕\��

  _InitBoxName(pWork->pBox, 0, pWork);
  IRC_POKETRADE_GraphicInit(pWork);  //BG��


#if 1
  pWork->mcssSys = MCSS_Init( 2 , pWork->heapID );
  MCSS_SetTextureTransAdrs( pWork->mcssSys , 0x30000 );
  MCSS_SetOrthoMode( pWork->mcssSys );
#endif

  GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );

  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, _BRIGHTNESS_SYNC);

  GFL_NET_ReloadIcon();

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
    pWork->bgscrollRenew=FALSE;
  }
}



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
  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x110000 );
  GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
  {
    IRC_POKEMON_TRADE *pWork = GFL_PROC_AllocWork( proc, sizeof( IRC_POKEMON_TRADE ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(IRC_POKEMON_TRADE));
    pWork->heapID = HEAPID_IRCBATTLE;

    if(pParentWork){
      GAMEDATA* pGameData = GAMESYSTEM_GetGameData(IrcBattle_GetGAMESYS_WORK(pParentWork));
      pWork->pBox = SaveData_GetBoxData( GAMEDATA_GetSaveControlWork(pGameData)  );
      pWork->pMy = GAMEDATA_GetMyStatus( pGameData );
      pWork->pMyParty = GAMEDATA_GetMyPokemon(pGameData);
    }
#if PM_DEBUG
    else{
      pWork->pBox = SaveData_GetBoxData(SaveControl_GetPointer());
      pWork->pMy = MyStatus_AllocWork(pWork->heapID);
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

  for(i=0;i<8;i++){
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


  GFL_CLGRP_CGR_Release(pWork->cellRes[CHAR_BOX] );
  GFL_CLGRP_PLTT_Release(pWork->cellRes[PLT_BOX]);
  GFL_CLGRP_CELLANIM_Release(pWork->cellRes[ANM_BOX]);
  GFL_CLGRP_PLTT_Release(pWork->cellRes[PLT_POKEICON]);
  GFL_CLGRP_CELLANIM_Release(pWork->cellRes[ANM_POKEICON]);

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


  for(i = 0;i< CUR_NUM;i++){
    if(pWork->curIcon[i]){
      GFL_CLACT_WK_Remove(pWork->curIcon[i]);
    }
  }
  GFL_CLACT_UNIT_Delete(pWork->cellUnit);

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
    GFL_HEAP_FreeMemory(pWork->pMy);
    GFL_HEAP_FreeMemory(pWork->pMyParty);
  }
#endif


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


