//=============================================================================
/**
 * @file	  ircpokemontrade.c
 * @bfief	  �|�P�����������ĒʐM���I������
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/07/09
 */
//=============================================================================

#include <gflib.h>

#if PM_DEBUG
#define _TRADE_DEBUG (1)
#else
#define _TRADE_DEBUG (0)
#endif

#include "arc_def.h"
#include "net/network_define.h"

#include "ircpokemontrade.h"
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

#include "ircpokemontrade_local.h"





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


static void _changeState(IRC_POKEMON_TRADE* pWork,StateFunc* state);
static void _changeStateDebug(IRC_POKEMON_TRADE* pWork,StateFunc* state, int line);
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
static void _PokemonsetAndSendData(IRC_POKEMON_TRADE* pWork);
static void _recvFriendScrollBar(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);



#if _TRADE_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG



///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _PacketTbl[] = {
	{_recvSelectPokemon,   _setChangePokemonBuffer},    ///_NETCMD_SELECT_POKEMON
	{_recvChangePokemon,   NULL},    ///_NETCMD_CHANGE_POKEMON
	{_recvChangeYesNo,   NULL},    ///_NETCMD_CHANGE_YESNO
	{_recvChangeCancel,   NULL},    ///_NETCMD_SELECT_POKEMON
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
 * $brief   YESNO�E�C���h�E�V�X�e���X�^�[�g
 *
 * @param   bgl		
 * @param   touch_sub_window_sys		
 *
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
static void PSTATUS_SUB_PokeCreateMcss( IRC_POKEMON_TRADE *pWork ,int no, const POKEMON_PASO_PARAM *ppp )
{
	MCSS_ADD_WORK addWork;
	VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};
	int xpos[] = { PSTATUS_MCSS_POS_X1 , PSTATUS_MCSS_POS_X2};

	GF_ASSERT( pWork->pokeMcss[no] == NULL );

	MCSS_TOOL_MakeMAWPPP( ppp , &addWork , MCSS_DIR_FRONT );
	pWork->pokeMcss[no] = MCSS_Add( pWork->mcssSys , xpos[no] , PSTATUS_MCSS_POS_Y ,0 , &addWork );
	MCSS_SetScale( pWork->pokeMcss[no] , &scale );
}

//--------------------------------------------------------------
//	�|�P����MCSS�폜
//--------------------------------------------------------------
static void PSTATUS_SUB_PokeDeleteMcss( IRC_POKEMON_TRADE *pWork,int no  )
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

static void _setPokemonStatusMessage(IRC_POKEMON_TRADE *pWork, int side,const POKEMON_PASO_PARAM* ppp)
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

	PPP_Get(ppp, ID_PARA_nickname, pWork->pStrBuf);

	GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->StatusWin[sidew]), 0, 1, pWork->pStrBuf, pWork->pFontHandle);


	lv=	PPP_Get(ppp, ID_PARA_level, NULL);

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
static void _Pokemonset(IRC_POKEMON_TRADE *pWork, int side, const POKEMON_PASO_PARAM* ppp )
{
	PSTATUS_SUB_PokeDeleteMcss(pWork, side);
	PSTATUS_SUB_PokeCreateMcss(pWork, side, ppp );

	pWork->bPokemonSet[side]=TRUE;

	_setPokemonStatusMessage(pWork,side ,ppp);
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


static const POKEMON_PASO_PARAM* _getPokeDataAddress(BOX_DATA* boxData , int trayNo, int index,IRC_POKEMON_TRADE* pWork)
{
	if(trayNo!=BOX_MAX_TRAY){
		return BOXDAT_GetPokeDataAddress(boxData,trayNo,index);
  }
	{
		POKEPARTY* party = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(pWork->pGameSys));

		if(index < PokeParty_GetPokeCount(party)){
			const POKEMON_PARAM *pp = PokeParty_GetMemberPointer( party , index );
			return PP_GetPPPPointerConst( pp );
		}
	}
	return NULL;

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

static void _changeState(IRC_POKEMON_TRADE* pWork,StateFunc state)
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
static void _changeStateDebug(IRC_POKEMON_TRADE* pWork,StateFunc state, int line)
{
	OS_TPrintf("trade: %d\n",line);
	_changeState(pWork, state);
}
#endif

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����̎�M�o�b�t�@��Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static u8* _setChangePokemonBuffer(int netID, void* pWk, int size)
{
	IRC_POKEMON_TRADE *pWork = pWk;
	if((netID >= 0) && (netID < 2)){
		return (u8*)pWork->recvPoke[netID];
	}
	return NULL;
}



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


//_NETCMD_CHANGE_CANCEL	

static void _recvChangeCancel(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
	IRC_POKEMON_TRADE *pWork = pWk;

	if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	//���ǂ�
	if(pWork->bTouchReset){
		TOUCH_SW_Reset( pWork->TouchSubWindowSys );
		pWork->bTouchReset=FALSE;
	}
	_messageDelete(pWork);
	
	_CHANGE_STATE(pWork, _touchState);
	
}

//_NETCMD_END_REQ
static void _recvEndReq(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
	IRC_POKEMON_TRADE *pWork = pWk;

	if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}

	_msgWindowCreate(pWork, POKETRADE_STR_22);
	TouchYesNoStart(pWork->TouchSubWindowSys);
	pWork->bTouchReset=TRUE;

	_CHANGE_STATE(pWork, _endWaitState);
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
	//OS_TPrintf("�������� %d\n", netID);
	pWork->bChangeOK[netID] = TRUE;


//	_CHANGE_STATE(pWork, _changeWaitState);
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

//�����I��҂�
static void _changePokemonSendData(IRC_POKEMON_TRADE* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    switch(selectno){
    case 0:  //��������
      GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_CHANGE_POKEMON,0,NULL);
      //���肩��̕Ԏ���҂̂Ń��b�Z�[�W�͑ҋ@��
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_09, pWork->pMessageStrBuf );
      IRC_POKETRADE_MessageWindowOpen(pWork,  POKETRADE_STR_09);

      _CHANGE_STATE(pWork,_changeWaitState);
      break;
    case 1:
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
        POKEMON_PARAM* pp = PP_CreateByPPP( pWork->recvPoke[i], pWork->heapID );
        WORDSET_RegisterPokeNickName( pWork->pWordSet, i,  pp );
        GFL_HEAP_FreeMemory(pp);
      }
      WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx  );

      
      IRC_POKETRADE_MessageWindowOpen(pWork,  POKETRADE_STR_20);
      
      _CHANGE_STATE(pWork,_networkFriendsStandbyWait2);
    }
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
			pWork->selectIndex = pWork->underSelectIndex;
			pWork->selectBoxno = pWork->underSelectBoxno;
      _PokemonsetAndSendData(pWork); //�����|�P���M
      // �����Ă��I�Ԃ܂ő҂�
      _CHANGE_STATE(pWork, _networkFriendsStandbyWait);
    }
    else{
      _CHANGE_STATE(pWork, _touchState);
    }
  }
}


//��������ɂ����A���j���[�\��
static void _changeMenuOpen(IRC_POKEMON_TRADE* pWork)
{
  {
    int msg[]={POKETRADE_STR_01,POKETRADE_STR_02};
    IRC_POKETRADE_AppMenuOpen(pWork,msg,elementof(msg));
  }

  _CHANGE_STATE(pWork, _changeMenuWait);

}



//������ʂɃX�e�[�^�X�\�� ����Ɍ����邩�ǂ����҂�
static void _dispSubStateWait(IRC_POKEMON_TRADE* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      //����Ɍ�����
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

static void _changeWaitState(IRC_POKEMON_TRADE* pWork)
{
	int i;
	int id = 1-GFL_NET_SystemGetCurrentID();


  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }

  
	for(i=0;i<2;i++){
		if(pWork->bChangeOK[i]==FALSE){
			return;
		}
	}

	//��������
	// ����̃|�P�������̑I��ł����ꏊ�ɓ����
	if(pWork->selectBoxno == BOX_MAX_TRAY){ //�������̂̌����̏ꍇ
		POKEPARTY* party = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(pWork->pGameSys));
    POKEMON_PARAM* pp = PP_CreateByPPP( pWork->recvPoke[id], pWork->heapID );
		
		PokeParty_SetMemberData(party, pWork->selectIndex, pp);
		GFL_HEAP_FreeMemory(pp);
	}
	else{
		BOXDAT_PutPokemonPos(pWork->pBox, pWork->selectBoxno, pWork->selectIndex,  pWork->recvPoke[id]);
	}
	pWork->selectBoxno = 0;
	pWork->selectIndex = -1;
	pWork->pCatchCLWK = NULL;
	PSTATUS_SUB_PokeDeleteMcss(pWork, 0);
	PSTATUS_SUB_PokeDeleteMcss(pWork, 1);

  IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);
	_InitBoxName(pWork->pBox,pWork->nowBoxno,pWork);  //�ĕ`��

  IRC_POKETRADE_MessageWindowClear(pWork);
	_msgWindowCreate(pWork, POKETRADE_STR_23);

	pWork->bPokemonSet[0]=FALSE;
	pWork->bPokemonSet[1]=FALSE;
  
	_CHANGE_STATE(pWork, _messageWaitState);
}


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
		GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_CHANGE_POKEMON,0,NULL);
    //���肩��̕Ԏ���҂̂Ń��b�Z�[�W�͑ҋ@��
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_09, pWork->pMessageStrBuf );
    IRC_POKETRADE_MessageWindowOpen(pWork,  POKETRADE_STR_09);
		_CHANGE_STATE(pWork,_changeWaitState);
		break;
	case TOUCH_SW_RET_NO:						//������

		GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_CHANGE_CANCEL,0,NULL);
		
		_CHANGE_STATE(pWork,_noneState);
		break;
	}


}



static void _endWaitState(IRC_POKEMON_TRADE* pWork)
{

	int result;
	result = TOUCH_SW_Main( pWork->TouchSubWindowSys );
	switch(result){				//�����
	case TOUCH_SW_RET_YES:						//�͂�

		GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_END,0,NULL);
		
		break;
	case TOUCH_SW_RET_NO:						//������

		GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_CHANGE_CANCEL,0,NULL);
		
		_CHANGE_STATE(pWork,_noneState);
		break;
	}


}



static void _PokemonsetAndSendData(IRC_POKEMON_TRADE* pWork)
{ //�I���|�P�����\��
  
  const POKEMON_PASO_PARAM* ppp = _getPokeDataAddress(pWork->pBox, pWork->selectBoxno, pWork->selectIndex,pWork);

  GF_ASSERT(ppp);
  
  if(ppp!=NULL){
    _Pokemonset(pWork,0,ppp);
    //@@OO POKEMON_PASO_PARAM�T�C�Y�擾�֐��˗���
    //@@OO �߂�l�����Ȃ��Ƃ΂���
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_SELECT_POKEMON, sizeof( POKEMON_PASO_PARAM ),ppp);
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
            pWork->BoxScrollNum+=2976;
          }
          if(2976 <= pWork->BoxScrollNum){
            pWork->BoxScrollNum-=2976;
          }
          IRC_POKETRADE_TrayDisp(pWork);
          IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);
          GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_SCROLLBAR,4,&pWork->BoxScrollNum);
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
				GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_END_REQ, 0, NULL);
				_CHANGE_STATE(pWork,_noneState);
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
      _PokemonsetAndSendData(pWork);

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

//----------------------------------------------------------------------------
/**
 *	@brief	ARC����ǂݍ���
 *	@param	IRC_POKEMON_TRADE		���[�N
 */
//-----------------------------------------------------------------------------

static void _subBgMake(IRC_POKEMON_TRADE* pWork)
{
	//ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_IRCBATTLE, pWork->heapID );
	MYSTATUS* pMy = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(pWork->pGameSys) );
	u32 sex = MyStatus_GetMySex(pMy);

#if 0

	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircbattle_DsTradeList_NCLR,
																		PALTYPE_MAIN_BG, 0, 0,  pWork->heapID);


	// �T�u���BG�L�����]��
	pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_ircbattle_DsTradeList_Sub_NCGR,
																																GFL_BG_FRAME2_M, 0, 0, pWork->heapID);

	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_ircbattle_DsTradeList_Sub2_NSCR,
																				 GFL_BG_FRAME1_M, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
																				 pWork->heapID);
	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_ircbattle_DsTradeList_Sub3_NSCR,
																				 GFL_BG_FRAME2_M, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
																				 pWork->heapID);




	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircbattle_box_wp01_NCLR,
																		PALTYPE_SUB_BG, 0, 0,  pWork->heapID);








	GFL_ARC_CloseDataHandle( p_handle );
#endif

//  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME1_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);



}

#define FIELD_CLSYS_RESOUCE_MAX		(100)


//--------------------------------------------------------------
///	�Z���A�N�^�[�@�������f�[�^
//--------------------------------------------------------------
static const GFL_CLSYS_INIT fldmapdata_CLSYS_Init =
{
	0, 0,
	0, 512,
	GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL,
	GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL, //�ʐM�A�C�R������
	0,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
	16, 16,
};

//----------------------------------------------------------------------------
/**
 *	@brief	���Z�b�g�A�b�v�R�[���o�b�N�֐�
 */
//-----------------------------------------------------------------------------
static void Graphic_3d_SetUp( void )
{
	// �R�c�g�p�ʂ̐ݒ�(�\�����v���C�I���e�B�[)
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );

	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_TOON); //GX_SHADING_HIGHLIGHT );
	G3X_AntiAlias( FALSE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( FALSE );		// �A���t�@�u�����h�@�I��
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);

	// ���C�g�ݒ�
	{
		static const GFL_G3D_LIGHT sc_GFL_G3D_LIGHT[] = 
		{
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
		};
		int i;
		
		for( i=0; i<NELEMS(sc_GFL_G3D_LIGHT); i++ ){
			GFL_G3D_SetSystemLight( i, &sc_GFL_G3D_LIGHT[i] );
		}
	}

	//�����_�����O�X���b�v�o�b�t�@
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
	G2_SetBG0Priority(2);
}


//------------------------------------------------------------------------------
/**
 * @brief   BG�̈�ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _createBg(IRC_POKEMON_TRADE* pWork)
{
	{
		static const GFL_DISP_VRAM vramBank = {
			GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
			GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
			GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
			GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
			GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g
			GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
			GX_VRAM_TEX_0_B,				// �e�N�X�`���C���[�W�X���b�g
			GX_VRAM_TEXPLTT_0_F,			// �e�N�X�`���p���b�g�X���b�g
			GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
			GX_OBJVRAMMODE_CHAR_1D_128K,		// �T�uOBJ�}�b�s���O���[�h
		};
		GFL_DISP_SetBank( &vramBank );


		GFL_CLACT_SYS_Create(	&fldmapdata_CLSYS_Init, &vramBank, pWork->heapID );


	}
	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
			};
		GFL_BG_SetBGMode( &sysHeader );
	}
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);

	{
		int frame = GFL_BG_FRAME3_M;
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			};

		GFL_BG_SetBGControl(
			frame, &bgcntText, GFL_BG_MODE_TEXT );
		GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
		GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( frame );
	}
	{
		int frame = GFL_BG_FRAME2_M;
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
			};

		GFL_BG_SetBGControl(
			frame, &bgcntText, GFL_BG_MODE_TEXT );
		GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
		GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( frame );
	}
	{
		int frame = GFL_BG_FRAME1_M;
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
			};

		GFL_BG_SetBGControl(
			frame, &bgcntText, GFL_BG_MODE_TEXT );
		//		GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
		GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( frame );
	}
	{
		int frame = GFL_BG_FRAME0_S;
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
			};
		GFL_BG_SetBGControl(
			frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
		GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		//		GFL_BG_LoadScreenReq( frame );
		//        GFL_BG_ClearFrame(frame);
	}
	{
		int frame = GFL_BG_FRAME1_S;
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
			};

		GFL_BG_SetBGControl(
			frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

		//		GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
		GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( frame );
		//        GFL_BG_ClearFrame(frame);
	}
	{
		int frame = GFL_BG_FRAME2_S;
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x10000,GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
			};

		GFL_BG_SetBGControl(
			frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

		GFL_BG_FillScreen( frame,	0x0000, 0, 0, 64, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( frame );
	}
	{
		int frame = GFL_BG_FRAME3_S;
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xc800, GX_BG_CHARBASE_0x10000, 0x8000,GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
			};

		GFL_BG_SetBGControl(
			frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
		GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 24, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( frame );
	}

	GFL_BG_SetBGControl3D( 0 );
	GFL_BG_SetVisible( GFL_BG_FRAME0_M , TRUE );
  

	//3D�n�̏�����
	{ //3D�n�̐ݒ�
		static const VecFx32 cam_pos = {FX32_CONST(0.0f),FX32_CONST(0.0f),FX32_CONST(101.0f)};
		static const VecFx32 cam_target = {FX32_CONST(0.0f),FX32_CONST(0.0f),FX32_CONST(-100.0f)};
		static const VecFx32 cam_up = {0,FX32_ONE,0};
		//�G�b�W�}�[�L���O�J���[
		static  const GXRgb edge_color_table[8]=
		{ GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };
		GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K,
									0, pWork->heapID, Graphic_3d_SetUp );
#if 0

		//���ˉe�J����
		pWork->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH,
																						FX32_ONE*12.0f,
																						0,
																						0,
																						FX32_ONE*16.0f,
																						(FX32_ONE),
																						(FX32_ONE*200),
																						NULL,
																						&cam_pos,
																						&cam_up,
																						&cam_target,
																						pWork->heapID );

		GFL_G3D_CAMERA_Switching( pWork->camera );
		//�G�b�W�}�[�L���O�J���[�Z�b�g
		G3X_SetEdgeColorTable( edge_color_table );
		G3X_EdgeMarking( TRUE );

		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );
#endif

	}


}



//------------------------------------------------------------------------------
/**
 * @brief   BOX�J�[�\���\��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _initBoxCursor(IRC_POKEMON_TRADE* pWork)
{
	GFL_CLWK_DATA cellInitData;
	u8 pltNum,i;
	GFL_CLACTPOS pos;

	//�f��
	{
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
		pWork->cellRes[CHAR_SCROLLBAR] =
			GFL_CLGRP_CGR_Register( p_handle , NARC_trade_wb_trade_obj01_NCGR ,
															FALSE , CLSYS_DRAW_SUB , pWork->heapID );
		pWork->cellRes[PAL_SCROLLBAR] =
			GFL_CLGRP_PLTT_Register(
				p_handle ,NARC_trade_wb_trade_obj_NCLR , CLSYS_DRAW_SUB ,
				_OBJPLT_BOX * 32 , pWork->heapID  );
		pWork->cellRes[ANM_SCROLLBAR] =
			GFL_CLGRP_CELLANIM_Register(
				p_handle , NARC_trade_wb_trade_obj01_NCER, NARC_trade_wb_trade_obj01_NANR , pWork->heapID  );

    GFL_ARC_CloseDataHandle( p_handle );

  }

  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 128;
    cellInitData.pos_y = CONTROL_PANEL_Y;
    cellInitData.anmseq = 2;
    cellInitData.bgpri = 1;
    pWork->curIcon[2] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                             pWork->cellRes[CHAR_SCROLLBAR],
                                             pWork->cellRes[PAL_SCROLLBAR],
                                             pWork->cellRes[ANM_SCROLLBAR],
                                             &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[2] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[2], TRUE );
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
	//	BOX_DATA* pBox = SaveData_GetBoxData(GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(pWork->pGameSys)));

	GFL_BG_Init( pWork->heapID );
	GFL_BMPWIN_Init( pWork->heapID );
	GFL_FONTSYS_Init();

	_createBg(pWork);
	_subBgMake(pWork);

	GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);

	//�����n������
	pWork->pWordSet    = WORDSET_Create( pWork->heapID );
	pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_poke_trade_dat, pWork->heapID );
	pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
	pWork->MyInfoWin = GFL_BMPWIN_Create(GFL_BG_FRAME1_S, 0x07, 0x01, 10, 2, _BUTTON_MSG_PAL,  GFL_BMP_CHRAREA_GET_B );
	pWork->pStrBuf = GFL_STR_CreateBuffer( 128, pWork->heapID );
	pWork->pExStrBuf = GFL_STR_CreateBuffer( 128, pWork->heapID );
	pWork->pMessageStrBuf = GFL_STR_CreateBuffer( 128, pWork->heapID );
	pWork->pMessageStrBufEx = GFL_STR_CreateBuffer( 128, pWork->heapID );


	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);
	
	//�Z���n�V�X�e���̍쐬
	pWork->cellUnit = GFL_CLACT_UNIT_Create( 120 , 0 , pWork->heapID );


	_initBoxCursor(pWork);

	//�|�P�A�C�R���p���\�[�X
	//�L�����N�^�͊e�v���[�g��
#if 1
	{
    int line,i;
		ARCHANDLE *arcHandlePoke = GFL_ARC_OpenDataHandle( ARCID_POKEICON , pWork->heapID );
		pWork->cellRes[PLT_POKEICON] =
      GFL_CLGRP_PLTT_RegisterComp( arcHandlePoke ,
                                   POKEICON_GetPalArcIndex() , CLSYS_DRAW_SUB ,
                                   _OBJPLT_POKEICON*32 , pWork->heapID  );
		pWork->cellRes[ANM_POKEICON] =
      GFL_CLGRP_CELLANIM_Register( arcHandlePoke ,
                                   POKEICON_GetCellSubArcIndex() , POKEICON_GetAnmArcIndex(), pWork->heapID  );

    for(line =0 ;line < _LING_LINENO_MAX; line++){
      for(i = 0;i < BOX_VERTICAL_NUM; i++){
        GFL_CLWK_DATA cellInitData;
          
 //         pWork->pokeIconNcgRes[line][i] =
   //         GFL_CLGRP_CGR_Register( arcHandle , POKEICON_GetCgxArcIndex(ppp) , FALSE , CLSYS_DRAW_SUB , pWork->heapID );
          
        cellInitData.pos_x = 0;
        cellInitData.pos_y = 0;
        cellInitData.anmseq = POKEICON_ANM_HPMAX;
        cellInitData.softpri = 0;
        cellInitData.bgpri = 1;

        pWork->pokeIconNcgRes[line][i] = GFL_CLGRP_CGR_Register( arcHandlePoke ,NARC_poke_icon_poke_icon_tam_NCGR , FALSE , CLSYS_DRAW_SUB , pWork->heapID );
        pWork->pokeIcon[line][i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                        pWork->pokeIconNcgRes[line][i],
                                                        pWork->cellRes[PLT_POKEICON],
                                                        pWork->cellRes[ANM_POKEICON],
                                                        &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
        GFL_CLACT_WK_SetAutoAnmFlag( pWork->pokeIcon[line][i] , FALSE );
        GFL_CLACT_WK_SetDrawEnable( pWork->pokeIcon[line][i], FALSE );
      }


    }

    GFL_ARC_CloseDataHandle(arcHandlePoke);


    
	}
#endif


	IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);  //�|�P�����̕\��

  _InitBoxName(pWork->pBox, 0, pWork);
  IRC_POKETRADE_GraphicInit(pWork);  //BG��

  
#if 1
	pWork->mcssSys = MCSS_Init( 2 , pWork->heapID );
	MCSS_SetTextureTransAdrs( pWork->mcssSys , 0 );
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
 * @brief   PROC�X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleFriendProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	int i;
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x38000+0x3CF00 );

	{
		IRC_POKEMON_TRADE *pWork = GFL_PROC_AllocWork( proc, sizeof( IRC_POKEMON_TRADE ), HEAPID_IRCBATTLE );
		GFL_STD_MemClear(pWork, sizeof(IRC_POKEMON_TRADE));
		pWork->heapID = HEAPID_IRCBATTLE;
		pWork->selectType =  EVENT_IrcBattleGetType((EVENT_IRCBATTLE_WORK*) pwk);
		pWork->pParentWork = pwk;
		pWork->pGameSys = IrcBattle_GetGAMESYS_WORK(pWork->pParentWork);
		// �ʐM�e�[�u���ǉ�
		GFL_NET_AddCommandTable(GFL_NET_CMD_IRCTRADE,_PacketTbl,NELEMS(_PacketTbl), pWork);
		pWork->g3dVintr = GFUser_VIntr_CreateTCB( _VBlank, (void*)pWork, 0 );

		pWork->pBox = SaveData_GetBoxData(GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(pWork->pGameSys)));

		pWork->pCatchCLWK = NULL;
		pWork->selectIndex = -1;


		pWork->recvPoke[0] = GFL_HEAP_AllocClearMemory(pWork->heapID, sizeof(POKEMON_PASO_PARAM));
		pWork->recvPoke[1] = GFL_HEAP_AllocClearMemory(pWork->heapID, sizeof(POKEMON_PASO_PARAM));


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
	if(state != NULL){
		state(pWork);
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
	GFL_G3D_DRAW_SetLookAt();
//  GFL_G3D_CAMERA_Switching( pWork->camera );  

//	MCSS_Main( pWork->mcssSys );
//	MCSS_Draw( pWork->mcssSys );
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

	GFL_NET_DelCommandTable(GFL_NET_CMD_IRCTRADE);
	
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


	GFL_G3D_CAMERA_Delete(pWork->camera);
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

	EVENT_IrcBattle_SetEnd(pParentWork);



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


