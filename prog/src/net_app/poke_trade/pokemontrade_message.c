//=============================================================================
/**
 * @file	  pokemontrade_message.c
 * @brief	  �|�P�����������ĒʐM���I������ ���b�Z�[�W�֌W
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/11/21
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

#include "net_app/pokemontrade.h"
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
#include "gamesystem\msgspeed.h"

#include "msg/msg_poke_trade.h"
#include "msg/msg_chr.h"
#include "msg/msg_trade_head.h"
#include "ircbattle.naix"
#include "trade.naix"
#include "net_app/connect_anm.h"
#include "net/dwc_rapfriend.h"
#include "savedata/wifilist.h"

#include "system/touch_subwindow.h"

#include "item/item.h"
#include "app/app_menu_common.h"
#include "box_m_obj_NANR_LBLDEFS.h"
#include "p_st_obj_d_NANR_LBLDEFS.h"

#include "pokemontrade_local.h"




static void _select6keywait(POKEMON_TRADE_WORK* pWork);
static void _pokeNickNameMsgDisp(POKEMON_PARAM* pp,GFL_BMP_DATA* pWin,int x,int y,BOOL bEgg,POKEMON_TRADE_WORK* pWork);
static void _pokeLvMsgDisp(POKEMON_PARAM* pp,GFL_BMP_DATA* pWin,int x,int y,POKEMON_TRADE_WORK* pWork);
static void _pokeSexMsgDisp(POKEMON_PARAM* pp,GFL_BMP_DATA* pWin,int x,int y,POKEMON_TRADE_WORK* pWork);



//------���b�Z�[�W�֘A


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
  UI_EASY_CLWK_RES_PARAM prm;
  PokeType type = PP_Get(pp, ID_PARA_type1+side, NULL);


  UITemplate_TYPEICON_DeleteCLWK(wk);

  prm.draw_type = draw_type;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetPokeTypePltArcIdx();
  prm.ncg_id    = APP_COMMON_GetPokeTypeCharArcIdx(type);
  prm.cell_id   = APP_COMMON_GetPokeTypeCellArcIdx( APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K );
  prm.pltt_line = PLTID_OBJ_TYPEICON_M;
  prm.pltt_src_ofs  = 0;
  prm.pltt_src_num = 3;

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
                                            int x ,int y, int drawtype, HEAPID heapID,int plt )
{
  UI_EASY_CLWK_RES_PARAM prm;
  BALL_ID ballID;

  // @todo �A�C�e�����{�[���ϊ����[�`���҂�
  ballID = BALLID_MASUTAABOORU;  // num=	PP_Get(pp, ID_PARA_item, NULL);

  UITemplate_BALLICON_DeleteCLWK(wk);

  prm.draw_type = drawtype;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetBallPltArcIdx( ballID );
  prm.ncg_id    = APP_COMMON_GetBallCharArcIdx( ballID );
  prm.cell_id   = APP_COMMON_GetBallCellArcIdx( ballID, APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetBallAnimeArcIdx( ballID, APP_COMMON_MAPPING_128K );
  prm.pltt_line = plt;
  prm.pltt_src_ofs  = 0;
  prm.pltt_src_num = 1;

  // ���\�[�X�ǂݍ���
  UI_EASY_CLWK_LoadResource( &wk->clres_ball, &prm, unit, heapID );
  // CLWK����
  wk->clwk_ball = UI_EASY_CLWK_CreateCLWK( &wk->clres_ball, unit, x, y, 0, heapID );
}


//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�E�C���h�E�J�� �����
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------


void POKETRADE_MESSAGE_WindowOpenXY(POKEMON_TRADE_WORK* pWork,BOOL bFast,int x,int y,int xm,int ym)
{
  GFL_BMPWIN* pwin;

  POKETRADE_MESSAGE_WindowClose(pWork);


  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);
  pWork->mesWin = GFL_BMPWIN_Create(GFL_BG_FRAME2_S , x , y, xm ,ym ,  _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  pwin = pWork->mesWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 15);

  GFL_FONTSYS_SetColor(FBMP_COL_BLACK, FBMP_COL_BLK_SDW, 15);
  if(!bFast){
    pWork->pStream = PRINTSYS_PrintStream(pwin ,0,0, pWork->pMessageStrBuf, pWork->pFontHandle,
                                          MSGSPEED_GetWait(), pWork->pMsgTcblSys, 2, pWork->heapID, 15 );
    APP_PRINTSYS_COMMON_PrintStreamInit(&pWork->trgWork, APP_PRINTSYS_COMMON_TYPE_KEY );
  }
  else{
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 0, 0, pWork->pMessageStrBuf, pWork->pFontHandle);
  }
  BmpWinFrame_Write( pwin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

  GFL_BMPWIN_TransVramCharacter(pwin);
  GFL_BMPWIN_MakeScreen(pwin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);

}


void POKETRADE_MESSAGE_WindowOpenCustom(POKEMON_TRADE_WORK* pWork,BOOL bFast,BOOL bUnderPos)
{
  if(bUnderPos){
    POKETRADE_MESSAGE_WindowOpenXY(pWork,bFast, 1,18,30,4);
  }
  else{
    POKETRADE_MESSAGE_WindowOpenXY(pWork,bFast, 1,1,30,4);
  }
}


void POKETRADE_MESSAGE_ChangeStreamType(POKEMON_TRADE_WORK* pWork,int type)
{
  APP_PRINTSYS_COMMON_PrintStreamInit(&pWork->trgWork, APP_PRINTSYS_COMMON_TYPE_THROUGH );
}


//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�E�C���h�E�J�� �����
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------


void POKETRADE_MESSAGE_WindowOpen(POKEMON_TRADE_WORK* pWork)
{
  POKETRADE_MESSAGE_WindowOpenCustom(pWork,FALSE,FALSE);
}


//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�E�C���h�E����
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKETRADE_MESSAGE_WindowClose(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->mesWin){
		GFL_BMPWIN_Delete(pWork->mesWin);
    pWork->mesWin=NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�E�C���h�E��ʏ����{����
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKETRADE_MESSAGE_WindowClear(POKEMON_TRADE_WORK* pWork)
{

	if(pWork->mesWin){
		GFL_BMPWIN_ClearScreen(pWork->mesWin);
		GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);
		BmpWinFrame_Clear(pWork->mesWin, WINDOW_TRANS_OFF);
		GFL_BMPWIN_Delete(pWork->mesWin);
		pWork->mesWin=NULL;
	}
}


//------------------------------------------------------------------------------
/**
 * @brief   ������֘A�̏������ƊJ��
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKETRADE_MESSAGE_HeapInit(POKEMON_TRADE_WORK* pWork)
{
  //�����n������
  pWork->pWordSet    = WORDSET_Create( pWork->heapID );
  pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_poke_trade_dat, pWork->heapID );
  pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
  pWork->pStrBuf = GFL_STR_CreateBuffer( 128, pWork->heapID );
  pWork->pExStrBuf = GFL_STR_CreateBuffer( 128, pWork->heapID );
  pWork->pMessageStrBuf = GFL_STR_CreateBuffer( 128, pWork->heapID );
  pWork->pMessageStrBufEx = GFL_STR_CreateBuffer( 128, pWork->heapID );
  pWork->SysMsgQue = PRINTSYS_QUE_Create( pWork->heapID );
  GFL_FONTSYS_SetColor(1, 2, 15);
  pWork->pMsgTcblSys = GFL_TCBL_Init( pWork->heapID , pWork->heapID , 2 , 0 );

	pWork->pAppTaskRes	= APP_TASKMENU_RES_Create( GFL_BG_FRAME2_S, _SUBLIST_NORMAL_PAL,
			pWork->pFontHandle, pWork->SysMsgQue, pWork->heapID );
}

//------------------------------------------------------------------------------
/**
 * @brief   ������֘A�̏������ƊJ��
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKETRADE_MESSAGE_HeapEnd(POKEMON_TRADE_WORK* pWork)
{
  int i;

  if(pWork->pMsgTcblSys==NULL){
    return;
  }
  

  if(pWork->pAppTaskRes){
    APP_TASKMENU_RES_Delete( pWork->pAppTaskRes );
    pWork->pAppTaskRes=NULL;
  }

  if(pWork->MyInfoWin){
    GFL_BMPWIN_Delete(pWork->MyInfoWin);
    pWork->MyInfoWin=NULL;
  }

  if(pWork->mesWin){
    GFL_BMPWIN_Delete(pWork->mesWin);
    pWork->mesWin=NULL;
  }

  for(i=0;i<2;i++){
    if(pWork->StatusWin[i]){
      GFL_BMPWIN_Delete(pWork->StatusWin[i]);
      pWork->StatusWin[i]=NULL;
    }
  }

  WORDSET_Delete(pWork->pWordSet);
  GFL_MSG_Delete(pWork->pMsgData);
  GFL_FONT_Delete(	pWork->pFontHandle );
  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  GFL_STR_DeleteBuffer(pWork->pExStrBuf);
  GFL_STR_DeleteBuffer(pWork->pMessageStrBuf);
  GFL_STR_DeleteBuffer(pWork->pMessageStrBufEx);
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);
  GFL_TCBL_Exit(pWork->pMsgTcblSys);
  pWork->pMsgTcblSys=NULL;
}


//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�̏I���҂�  APP_PRINTSYS_COMMON_TYPE_THROUGH
 * @retval  none
 */
//------------------------------------------------------------------------------

BOOL POKETRADE_MESSAGE_EndCheck(POKEMON_TRADE_WORK* pWork)
{
  BOOL ret = APP_PRINTSYS_COMMON_PrintStreamFunc(&pWork->trgWork,pWork->pStream);

  if(ret){
    if(pWork->pStream){
      PRINTSYS_PrintStreamDelete( pWork->pStream );
      pWork->pStream = NULL;
    }
  }
  return ret;
  
#if 0



  if(pWork->pStream){
    int state = PRINTSYS_PrintStreamGetState( pWork->pStream );
    switch(state){
    case PRINTSTREAM_STATE_DONE:
      PRINTSYS_PrintStreamDelete( pWork->pStream );
      pWork->pStream = NULL;
      break;
    case PRINTSTREAM_STATE_PAUSE:
 //     if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
        PRINTSYS_PrintStreamReleasePause( pWork->pStream );
   //   }
      break;
    default:
      break;
    }
    return FALSE;  //�܂��I����ĂȂ�
  }
  return TRUE;// �I����Ă���
#endif
}


void POKETRADE_MESSAGE_AppMenuClose(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->pAppTask){
    G2_BlendNone();
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
  }
}

void POKETRADE_MESSAGE_AppMenuOpenCustom(POKEMON_TRADE_WORK* pWork, int *menustr,int num, int x,int y)
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  num;
  appinit.itemWork =  &pWork->appitem[0];

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = x;
  appinit.charPosY = y;
	appinit.w				 = APP_TASKMENU_PLATE_WIDTH;
	appinit.h				 = APP_TASKMENU_PLATE_HEIGHT;

  for(i=0;i<num;i++){
    pWork->appitem[i].str = GFL_STR_CreateBuffer(100, pWork->heapID);
    GFL_MSG_GetString(pWork->pMsgData, menustr[i], pWork->appitem[i].str);
    pWork->appitem[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
    if(menustr[i] == POKETRADE_STR_06){
      pWork->appitem[i].type = APP_TASKMENU_WIN_TYPE_RETURN;
    }
    else{
      pWork->appitem[i].type = APP_TASKMENU_WIN_TYPE_NORMAL;
    }
  }
  pWork->pAppTask = APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
  for(i=0;i<num;i++){
    GFL_STR_DeleteBuffer(pWork->appitem[i].str);
  }

}

void POKETRADE_MESSAGE_AppMenuOpen(POKEMON_TRADE_WORK* pWork, int *menustr,int num)
{
  if(num==1){
    POKETRADE_MESSAGE_AppMenuOpenCustom(pWork, menustr, num, 32,21);
  }
  else{
    POKETRADE_MESSAGE_AppMenuOpenCustom(pWork, menustr, num, 32,24);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P��������WINDOW�ɕ\��
 * @retval  none
 */
//------------------------------------------------------------------------------

//static void _pokeNickNameMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,BOOL bEgg,POKEMON_TRADE_WORK* pWork)
static void _pokeNickNameMsgDisp(POKEMON_PARAM* pp,GFL_BMP_DATA* pWin,int x,int y,BOOL bEgg,POKEMON_TRADE_WORK* pWork)
{
  if(!bEgg){
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_30, pWork->pExStrBuf );
    WORDSET_RegisterPokeNickName( pWork->pWordSet, 0,  pp );
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
  }
  else{
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_07, pWork->pStrBuf );
  }
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
  PRINTSYS_Print( pWin, x, y, pWork->pStrBuf, pWork->pFontHandle);
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����k�u��WINDOW�ɕ\��
 * @retval  none
 */
//------------------------------------------------------------------------------

//static void _pokeLvMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
static void _pokeLvMsgDisp(POKEMON_PARAM* pp,GFL_BMP_DATA* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
{
  int lv;

  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_21, pWork->pExStrBuf );
  lv=	PP_Get(pp, ID_PARA_level, NULL);

  WORDSET_RegisterNumber(pWork->pWordSet, 0, lv, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );

  PRINTSYS_Print( pWin, x, y, pWork->pStrBuf, pWork->pFontHandle);
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P����SEX��WINDOW�ɕ\��
 * @retval  none
 */
//------------------------------------------------------------------------------

//static void _pokeSexMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
static void _pokeSexMsgDisp(POKEMON_PARAM* pp,GFL_BMP_DATA* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
{
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
 // PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle,fontCol);
  PRINTSYS_PrintColor( pWin, x, y, pWork->pStrBuf, pWork->pFontHandle,fontCol);
}



//HP�|���΂₳�������\��
static void _pokeHPSPEEDMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
{
  int i;
  for(i=0;i<6;i++){//HP�|���΂₳
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_31+i, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y+16*i, pWork->pStrBuf, pWork->pFontHandle);
  }
}

static void _pokePocketItemMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
{ //��������
  int num;

  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_44, pWork->pExStrBuf );
  num=	PP_Get(pp, ID_PARA_item, NULL);
  if(num!=0){
    WORDSET_RegisterItemName( pWork->pWordSet, 0,  num );
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
  }
}

//HP HPMax
static void _pokeHpHpmaxMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
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

static void _pokeATTACKSPEEDNumMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
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

static void _pokeTechniqueMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
{
  { //�킴
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_45, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
  }
}

static void _pokeTechniqueListMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
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

static void _pokePersonalMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
{ //��������
  int num;
  GFL_MSGDATA *pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,  NARC_message_chr_dat, pWork->heapID );
  num=	PP_GetSeikaku(pp);
  GFL_MSG_GetString( pMsgData, seikaku00_msg+num, pWork->pStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
  GFL_MSG_Delete(pMsgData);
}

static void _pokeAttributeMsgDisp(POKEMON_PARAM* pp,GFL_BMPWIN* pWin,int x,int y,POKEMON_TRADE_WORK* pWork)
{ //�Ƃ�����
  int num;
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_43, pWork->pExStrBuf );
  num=	PP_Get(pp, ID_PARA_speabino, NULL);
  WORDSET_RegisterTokuseiName( pWork->pWordSet, 0,  num );
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWin), x, y, pWork->pStrBuf, pWork->pFontHandle);
}

//--------------------------------------------------------------
//	�|�P������ԕ��̓Z�b�g
//--------------------------------------------------------------

void POKETRADE_MESSAGE_SetPokemonStatusMessage(POKEMON_TRADE_WORK *pWork, int side,POKEMON_PARAM* pp)
{
  int i=0,lv;
  int frame = GFL_BG_FRAME3_M;
  int sidex[] = {1, 17};
  int xdotpos[]={0,128};
  BOOL bEgg = PP_Get(pp,ID_PARA_tamago_flag,NULL);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);
  if(pWork->StatusWin[side]){
    GFL_BMPWIN_Delete(pWork->StatusWin[side]);
  }
  pWork->StatusWin[side] = GFL_BMPWIN_Create(frame,	sidex[side], 1, 15, 20,	_BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);

  GFL_FONTSYS_SetColor( FBMP_COL_WHITE, FBMP_COL_BLK_SDW, 0x0 );

  _pokeNickNameMsgDisp(pp, GFL_BMPWIN_GetBmp(pWork->StatusWin[side]), 2*8, 0, bEgg, pWork);
  if(!bEgg){
//    _pokeSexMsgDisp(pp, pWork->StatusWin[side], 10*8, 0, pWork);
    _pokeSexMsgDisp(pp, GFL_BMPWIN_GetBmp(pWork->StatusWin[side]), 10*8, 0, pWork);
    GFL_FONTSYS_SetColor( FBMP_COL_BLACK, FBMP_COL_BLK_SDW, 0x0 );
    _pokeLvMsgDisp(pp, GFL_BMPWIN_GetBmp(pWork->StatusWin[side]), 16, 19, pWork);
    _pokePocketItemMsgDisp(pp, pWork->StatusWin[side], 16, 16*8, pWork);
    UITemplate_BALLICON_CreateCLWK( &pWork->aBallIcon[side+UI_BALL_MYSTATUS], pp, pWork->cellUnit,
                                    xdotpos[side]+16, 16, CLSYS_DRAW_MAIN, pWork->heapID,PLTID_OBJ_BALLICON_M );
  }
  GFL_BMPWIN_TransVramCharacter(pWork->StatusWin[side]);
  GFL_BMPWIN_MakeScreen(pWork->StatusWin[side]);
  GFL_BG_LoadScreenV_Req( frame );

}


//--------------------------------------------------------------
//	�|�P�����X�e�[�^�X�\���̏���
//--------------------------------------------------------------



void POKETRADE_MESSAGE_ResetPokemonStatusMessage(POKEMON_TRADE_WORK *pWork)
{
  IRC_POKETRADE_ItemIconReset(&pWork->aPokerusMark);
  IRC_POKETRADE_PokeStatusIconReset(pWork);
  GXS_SetVisibleWnd( GX_WNDMASK_NONE );
  UITemplate_BALLICON_DeleteCLWK(&pWork->aBallIcon[UI_BALL_SUBSTATUS]);
  UITemplate_TYPEICON_DeleteCLWK(&pWork->aTypeIcon[0]);
  UITemplate_TYPEICON_DeleteCLWK(&pWork->aTypeIcon[1]);
  IRC_POKETRADE_ResetMainStatusBG(pWork); //CLACT�폜�܂�
  IRCPOKETRADE_PokeDeleteMcss(pWork,0);
  IRCPOKETRADE_PokeDeleteMcss(pWork,1);
  GFL_BMPWIN_ClearScreen(pWork->MyInfoWin);
  GFL_BMPWIN_Delete(pWork->MyInfoWin);
  pWork->MyInfoWin = NULL;

  IRC_POKETRADE_GraphicEndSubDispStatusDisp(pWork);


}

//--------------------------------------------------------------
//	�|�P�����X�e�[�^�X�\���̏�����
//--------------------------------------------------------------



void POKETRADE_MESSAGE_CreatePokemonParamDisp(POKEMON_TRADE_WORK* pWork,POKEMON_PARAM* pp)
{
  GFL_BG_SetVisible( GFL_BG_FRAME3_M , FALSE );

  IRC_POKETRADE_MainGraphicExit(pWork);  //����VRAM�J��

  IRC_POKETRADEDEMO_RemoveModel( pWork);  //���[��������
  POKETRADE_MESSAGE_WindowClear(pWork);  //���̃��b�Z�[�W������

  IRCPOKEMONTRADE_ResetPokemonStatusMessage(pWork,0); //��̃X�e�[�^�X����+OAM������
  IRCPOKEMONTRADE_ResetPokemonStatusMessage(pWork,1); //��̃X�e�[�^�X����+OAM������

  pWork->pokemonselectno = 0;//��������\��
  POKETRADE_MESSAGE_ChangePokemonStatusDisp(pWork, pp, 0, FALSE);
  IRC_POKETRADE_SetMainStatusBG(pWork);  // �w�iBG��

  if(!POKEMONTRADEPROC_IsTriSelect(pWork)){
    IRC_POKETRADE_SetSubStatusIcon(pWork);  //�I���A�C�R��
    IRC_POKETRADE_GraphicInitSubDispStatusDisp(pWork);
    GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
    GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );

    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ, -8 );

//    POKETRADE2D_IconAllGray(pWork,TRUE);

    {
      G2S_SetWnd1InsidePlane(
        GX_WND_PLANEMASK_OBJ,
        FALSE );
      G2S_SetWnd1Position( 128-8*10, 0, 128+8*10, 32 );
      G2S_SetWnd0InsidePlane(
        GX_WND_PLANEMASK_BG0|
        GX_WND_PLANEMASK_OBJ,
        FALSE );
      G2S_SetWnd0Position( 224, 169, 255, 192 );
      G2S_SetWndOutsidePlane(
        GX_WND_PLANEMASK_BG0|
        GX_WND_PLANEMASK_BG1|
        GX_WND_PLANEMASK_BG2|
        GX_WND_PLANEMASK_BG3|
        GX_WND_PLANEMASK_OBJ,
        TRUE );
      GXS_SetVisibleWnd( GX_WNDMASK_W0|GX_WNDMASK_W1 );
     // GXS_SetVisibleWnd( GX_WNDMASK_W1 );
    }
  }
  //G2S_BlendNone();
  TOUCHBAR_SetVisible( pWork->pTouchWork, TOUCHBAR_ICON_RETURN ,TRUE );
}

// ���\���̕ύX �P�܂��߂��Q�܂��߂�����
void POKETRADE_MESSAGE_ChangePokemonMyStDisp(POKEMON_TRADE_WORK* pWork,int pageno,int leftright,POKEMON_PARAM* pp)
{
  int i,num,num2,bEgg;
  int xwinpos[]={0,16};
  int xdotpos[]={0,128};
  {
 //   GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
   //                               0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

    IRC_POKETRADE_StatusWindowMessagePaletteTrans(pWork,_STATUS_MSG_PAL,PALTYPE_SUB_BG);

    if(pWork->MyInfoWin){
      GFL_BMPWIN_Delete(pWork->MyInfoWin);
    }
    pWork->MyInfoWin =
      GFL_BMPWIN_Create(GFL_BG_FRAME2_S,
                        xwinpos[leftright], 0, 16 , 18, _STATUS_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
  }
  GFL_FONTSYS_SetColor( 0xe, 0xf, 0x0 );
  bEgg = PP_Get(pp,ID_PARA_tamago_flag,NULL);

  if(pageno == 0){  //���������Ƃ�

    _pokeNickNameMsgDisp(pp,GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 32, 0, bEgg,pWork);
    if(bEgg==FALSE){
      _pokeLvMsgDisp(pp,GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 32 ,16,pWork);
      _pokeHPSPEEDMsgDisp(pp,pWork->MyInfoWin, 8 ,32,pWork);
      _pokePocketItemMsgDisp(pp,pWork->MyInfoWin, 4*8 ,16*8,pWork);
      _pokeHpHpmaxMsgDisp(pp, pWork->MyInfoWin, 8*8,4*8, pWork);
      _pokeATTACKSPEEDNumMsgDisp(pp, pWork->MyInfoWin, 10*8,6*8, pWork);
      _pokeSexMsgDisp(pp, GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 12*8, 0, pWork);

      IRC_POKETRADE_ItemIconDisp(pWork, leftright, pp);
      IRC_POKETRADE_PokerusIconDisp(pWork, leftright,FALSE, pp);
      UITemplate_BALLICON_CreateCLWK( &pWork->aBallIcon[UI_BALL_SUBSTATUS], pp, pWork->cellUnit,
                                      xdotpos[leftright]+16, 16, CLSYS_DRAW_SUB, pWork->heapID,PLTID_OBJ_BALLICON_S );
    }
  }
  else{
    UITemplate_BALLICON_DeleteCLWK(&pWork->aBallIcon[UI_BALL_SUBSTATUS]);
    IRC_POKETRADE_ItemIconReset(&pWork->aItemMark);
    IRC_POKETRADE_ItemIconReset(&pWork->aPokerusMark);
    if(bEgg==FALSE){
      _pokeTechniqueMsgDisp(pp, pWork->MyInfoWin, 8,0, pWork);

      _pokeTechniqueListMsgDisp(pp, pWork->MyInfoWin, 2*8,2*8, pWork);
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_37, pWork->pStrBuf );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 8, 10*8, pWork->pStrBuf, pWork->pFontHandle);
      _pokePersonalMsgDisp(pp, pWork->MyInfoWin, 2*8, 12*8, pWork);
      GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_38, pWork->pStrBuf );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 8, 14*8, pWork->pStrBuf, pWork->pFontHandle);
      _pokeAttributeMsgDisp(pp, pWork->MyInfoWin, 2*8, 16*8, pWork);
    }
  }

  IRC_POKETRADE_SubStatusInit(pWork,pWork->x, pageno);  //���BG�\��

  GFL_BMPWIN_MakeScreen(pWork->MyInfoWin);
  GFL_BMPWIN_TransVramCharacter(pWork->MyInfoWin);
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
}

// �X�e�[�^�X�\���̕ύX
void POKETRADE_MESSAGE_ChangePokemonStatusDisp(POKEMON_TRADE_WORK* pWork,POKEMON_PARAM* pp, int mcssno,int change)
{
  int i,num,num2,bEgg;

  bEgg = PP_Get(pp,ID_PARA_tamago_flag,NULL);

  if(!pWork->pokeMcss[mcssno]){
    IRCPOKETRADE_PokeCreateMcss(pWork, mcssno, mcssno, pp , FALSE);
  }
  if(change){
    IRCPOKETRADE_PokeDeleteMcss(pWork, mcssno);
    IRCPOKETRADE_PokeCreateMcss(pWork, mcssno, mcssno, pp , FALSE);
  }

  {//�����̈ʒu����
    VecFx32 apos;
    apos.x = _MCSS_POS_X(51);
    apos.y = _MCSS_POS_Y(16);
    apos.z = _MCSS_POS_Z(0);
    MCSS_SetPosition( pWork->pokeMcss[mcssno] ,&apos );
    MCSS_ResetVanishFlag(pWork->pokeMcss[mcssno]);

    //����̂�OFF
    if(pWork->pokeMcss[ 1 - mcssno]){
      MCSS_SetVanishFlag(pWork->pokeMcss[1-mcssno]);
    }

  }
  if(pWork->MyInfoWin){
    GFL_BMPWIN_Delete(pWork->MyInfoWin);
  }
  GFL_FONTSYS_SetColor( 0xf, 0x2, 0 );
  pWork->MyInfoWin =
    GFL_BMPWIN_Create(GFL_BG_FRAME3_M,
                      1, 1, 31 , 24, _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);

  _pokeNickNameMsgDisp(pp,GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 16, 0,bEgg,pWork);//�j�b�N�l�[��
  _pokeLvMsgDisp(pp,GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 8*12 , 2,pWork);
  _pokeSexMsgDisp(pp, GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 10*8, 0, pWork);

  _pokeHPSPEEDMsgDisp(pp,pWork->MyInfoWin, 0 ,3*8,pWork);

  for(i=0;i<3;i++){//��������-��������
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_37+i, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 0, 16*8+16*i, pWork->pStrBuf, pWork->pFontHandle);
  }
  _pokeHpHpmaxMsgDisp(pp, pWork->MyInfoWin, 8*7, 3*8, pWork);  //HP HPMax

  _pokeATTACKSPEEDNumMsgDisp(pp, pWork->MyInfoWin, 8*9, 5*8, pWork);


  _pokePersonalMsgDisp(pp, pWork->MyInfoWin, 7*8, 16*8, pWork);  //���i
  _pokeAttributeMsgDisp(pp, pWork->MyInfoWin, 7*8, 18*8, pWork);  //����
  _pokePocketItemMsgDisp(pp,pWork->MyInfoWin, 7*8, 20*8,pWork);  //��������
  _pokeTechniqueMsgDisp(pp, pWork->MyInfoWin, 19*8, 12*8, pWork);  //�킴
  _pokeTechniqueListMsgDisp(pp, pWork->MyInfoWin, 19*8,14*8, pWork); //�킴���X�g

  UITemplate_BALLICON_CreateCLWK( &pWork->aBallIcon[UI_BALL_SUBSTATUS], pp, pWork->cellUnit,
                                  16, 16, CLSYS_DRAW_MAIN, pWork->heapID,PLTID_OBJ_BALLICON_M );

  UITemplate_TYPEICON_CreateCLWK(&pWork->aTypeIcon[0], pp, 0, pWork->cellUnit,
                                 25*8, 12*8, CLSYS_DRAW_MAIN, pWork->heapID );

  if( PP_Get(pp, ID_PARA_type1, NULL) != PP_Get(pp, ID_PARA_type2, NULL)){
    UITemplate_TYPEICON_CreateCLWK(&pWork->aTypeIcon[1], pp, 1, pWork->cellUnit,
                                   29*8, 12*8, CLSYS_DRAW_MAIN, pWork->heapID );
  }
  IRC_POKETRADE_PokeStatusIconDisp(pWork,pp);  //������
  IRC_POKETRADE_PokerusIconDisp(pWork, 0,TRUE, pp);


  GFL_BMPWIN_MakeScreen(pWork->MyInfoWin);
  GFL_BMPWIN_TransVramCharacter(pWork->MyInfoWin);
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );
}


//--------------------------------------------------------------
//	�|�P������ԕ��͏���
//--------------------------------------------------------------

void IRCPOKEMONTRADE_ResetPokemonStatusMessage(POKEMON_TRADE_WORK *pWork, int side)
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

void POKETRADE_MESSAGE_ResetPokemonMyStDisp(POKEMON_TRADE_WORK* pWork)
{
  UITemplate_BALLICON_DeleteCLWK(&pWork->aBallIcon[UI_BALL_SUBSTATUS]);
  IRC_POKETRADE_ItemIconReset(&pWork->aItemMark);
  IRC_POKETRADE_ItemIconReset(&pWork->aPokerusMark);
  if(pWork->MyInfoWin){
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->MyInfoWin), 0 );
    GFL_BMPWIN_ClearScreen(pWork->MyInfoWin);
    GFL_BMPWIN_TransVramCharacter(pWork->MyInfoWin);
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
    GFL_BMPWIN_Delete(pWork->MyInfoWin);
    pWork->MyInfoWin = NULL;
  }
}




//--------------------------------------------------------------
//	�U�̃|�P�����X�e�[�^�X�\��
//--------------------------------------------------------------

void POKETRADE_MESSAGE_SixStateDisp(POKEMON_TRADE_WORK* pWork,int frame)
{
  GFL_BMPWIN* pWin;
  int side,poke;
  MYSTATUS* aStBuf[GTS_PLAYER_WORK_NUM];

  POKEMONTRADE_CreatePokeSelectMessage(pWork);
  //�����Ƒ���͂�������
  aStBuf[0] = pWork->pFriend;
  aStBuf[1] = pWork->pMy;

#if 1
  GFL_FONTSYS_SetColor( FBMP_COL_WHITE, FBMP_COL_BLK_SDW, 0x0 );
  for(side=0;side<GTS_PLAYER_WORK_NUM;side++){
    GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR2_27, pWork->pExStrBuf );
    WORDSET_RegisterPlayerName( pWork->pWordSet, 0,  aStBuf[side]  );
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
    PRINTSYS_Print( pWork->listBmp[side*4], 0, 0, pWork->pStrBuf, pWork->pFontHandle);
  }
#endif


  GFL_FONTSYS_SetColor( FBMP_COL_WHITE, FBMP_COL_BLK_SDW, 0x0 );


  for(side=0;side<GTS_PLAYER_WORK_NUM;side++){
//    if(!pWork->TriStatusWin[side+2]){
//      pWork->TriStatusWin[side+2] =
//        GFL_BMPWIN_Create(frame,
//                          side*16 + 5 , 3, 9, 18, _BUTTON_MSG_PAL,GFL_BMP_CHRAREA_GET_F);
//    }
//    pWin = pWork->TriStatusWin[side+2];
    GFL_FONTSYS_SetColor( FBMP_COL_BLACK, FBMP_COL_BLK_SDW, 0x0 );
    for(poke = 0;poke < GTS_NEGO_POKESLT_MAX;poke++){
      POKEMON_PARAM* pp = pWork->GTSSelectPP[1-side][poke];  //�������܂ɕύX����

      OS_TPrintf("SixDisp %d %d %d\n",1-side,poke,pWork->GTSSelectIndex[1-side][poke]);

      if(pWork->GTSSelectIndex[1-side][poke]!=-1){
        BOOL bEgg = PP_Get(pp,ID_PARA_tamago_flag,NULL);

        POKETRADE_2D_GTSPokemonIconSet(pWork, side, poke, pp,FALSE);
        
        _pokeNickNameMsgDisp(pp, pWork->listBmp[side*4+poke+1], 0,  0 , bEgg, pWork);
        if(!bEgg){
          _pokeSexMsgDisp(pp, pWork->listBmp[side*4+poke+1], 7*8, 2*8, pWork);
          _pokeLvMsgDisp(pp, pWork->listBmp[side*4+poke+1], 0,  2*8, pWork);
        }
      }
    }
    //GFL_BMPWIN_TransVramCharacter(pWin);
    //GFL_BMPWIN_MakeScreen(pWin);
  }
  POKEMONTRADE_MessageOAMWriteVram(pWork);
}


//--------------------------------------------------------------
//	�U�̃|�P�����X�e�[�^�X����
//--------------------------------------------------------------

void POKETRADE_MESSAGE_SixStateDelete(POKEMON_TRADE_WORK* pWork)
{
  int i;

  for(i=0;i<GTS_PLAYER_WORK_NUM*2 ;i++){
    if( pWork->TriStatusWin[i] ){
      GFL_BMPWIN_Delete(pWork->TriStatusWin[i]);
      pWork->TriStatusWin[i]=NULL;
    }
  }
  POKEMONTRADE_RemovePokeSelectMessage(pWork);
}



void POKEMON_TRADE_MaskCommon(POKEMON_TRADE_WORK* pWork)
{
  G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ, -8 );

  {
    G2S_SetWnd0InsidePlane(
      GX_WND_PLANEMASK_BG0|
      GX_WND_PLANEMASK_BG1|
      GX_WND_PLANEMASK_BG2|
      GX_WND_PLANEMASK_BG3|
      GX_WND_PLANEMASK_OBJ,
      TRUE );
    G2S_SetWnd0Position( 0, 0, 255, 192 );
    G2S_SetWnd1InsidePlane(
      GX_WND_PLANEMASK_BG0|
      GX_WND_PLANEMASK_BG1|
      GX_WND_PLANEMASK_BG2|
      GX_WND_PLANEMASK_BG3|
      GX_WND_PLANEMASK_OBJ,
      TRUE );
    G2S_SetWnd1Position( 255, 0, 0, 192 );
    G2S_SetWndOutsidePlane(
      GX_WND_PLANEMASK_BG0|
      GX_WND_PLANEMASK_BG1|
      GX_WND_PLANEMASK_BG2|
      GX_WND_PLANEMASK_BG3|
      GX_WND_PLANEMASK_OBJ,
      FALSE );
    GXS_SetVisibleWnd( GX_WNDMASK_W0|GX_WNDMASK_W1 );
  }
}


void POKEMONTRADE_NEGOBG_SlideMessage(POKEMON_TRADE_WORK *pWork, int side,POKEMON_PARAM* pp)
{
  int i=0,lv;
  int frame = GFL_BG_FRAME1_M;
  int sidex[] = {1, 17};
  int xdotpos[]={0,128};
  int palette[]={9,8,11,10 };
  
  BOOL bEgg = PP_Get(pp,ID_PARA_tamago_flag,NULL);

  if(pWork->StatusWin1[side]){
    GFL_BMPWIN_Delete(pWork->StatusWin1[side]);
    GFL_BMPWIN_Delete(pWork->StatusWin2[side]);
  }
  pWork->StatusWin1[side] = GFL_BMPWIN_Create(frame,	sidex[side],  0,14, 5,	_POKEMON_MAIN_FRIENDGIVEMSG_PAL, GFL_BMP_CHRAREA_GET_F);
  pWork->StatusWin2[side] = GFL_BMPWIN_Create(frame,	sidex[side], 17,13, 2,	_POKEMON_MAIN_FRIENDGIVEMSG_PAL, GFL_BMP_CHRAREA_GET_F);


  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(pWork->StatusWin1[side]), WINCLR_COL(palette[side*2]) );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(pWork->StatusWin2[side]), WINCLR_COL(palette[side*2+1]) );

  GFL_FONTSYS_SetColor( 1, 2, palette[side*2] );

  _pokeNickNameMsgDisp(pp, GFL_BMPWIN_GetBmp(pWork->StatusWin1[side]), 2*8, 4, bEgg, pWork);
  if(!bEgg){
    _pokeSexMsgDisp(pp, GFL_BMPWIN_GetBmp(pWork->StatusWin1[side]), 10*8, 4, pWork);
    _pokeLvMsgDisp(pp, GFL_BMPWIN_GetBmp(pWork->StatusWin1[side]), 16, 19, pWork);
    GFL_FONTSYS_SetColor( 1, 2, palette[side*2+1] );
    _pokePocketItemMsgDisp(pp, pWork->StatusWin2[side], 16, 16*8, pWork);
    UITemplate_BALLICON_CreateCLWK( &pWork->aBallIcon[side+UI_BALL_MYSTATUS], pp, pWork->cellUnit,
                                    xdotpos[side]+16, 16, CLSYS_DRAW_MAIN, pWork->heapID,PLTID_OBJ_BALLICON_M );
  }
  GFL_BMPWIN_TransVramCharacter(pWork->StatusWin1[side]);
  GFL_BMPWIN_TransVramCharacter(pWork->StatusWin2[side]);

  GFL_BMPWIN_MakeScreen(pWork->StatusWin1[side]);
  GFL_BMPWIN_MakeScreen(pWork->StatusWin2[side]);
  GFL_BG_LoadScreenV_Req( frame );

  pWork->SlideWindowTimer[side] = _SLIDE_END_TIME;
  

}

void POKEMONTRADE_NEGOBG_SlideMessageDel(POKEMON_TRADE_WORK *pWork,int side)
{
  GFL_BMPWIN_Delete(pWork->StatusWin1[side]);
  GFL_BMPWIN_Delete(pWork->StatusWin2[side]);
  
  pWork->StatusWin1[side]=NULL;
  pWork->StatusWin2[side]=NULL;
}

