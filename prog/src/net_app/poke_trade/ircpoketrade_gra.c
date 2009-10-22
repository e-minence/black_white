//=============================================================================
/**
 * @file	  ircpoketrade_gra.c
 * @bfief	  �|�P���������O���t�B�b�N����
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/08/28
 */
//=============================================================================


#include <gflib.h>
#include "arc_def.h"
#include "poke_tool/monsno_def.h"

#include "trade.naix"
#include "poke_icon.naix"

#include "ircpokemontrade.h"
#include "ircpokemontrade_local.h"
#include "msg/msg_poke_trade.h"
#include "gamesystem/msgspeed.h"  //MSGSPEED_GetWait
#include "font/font.naix"    // NARC_font_default_nclr

#include "system/bmp_winframe.h"
#include "savedata/box_savedata.h"
#include "pokeicon/pokeicon.h"
#include "app/app_menu_common.h"

#include "ircbattle.naix"
#include "trade.naix"
#include "tradedemo.naix"


#define _SUBLIST_NORMAL_PAL   (9)   //�T�u���j���[�̒ʏ�p���b�g




#define _CLACT_SOFTPRI_POKESEL  (2)
#define _CLACT_SOFTPRI_POKESEL_BAR  (3)
#define _CLACT_SOFTPRI_SCROLL_BAR  (4)
#define _CLACT_SOFTPRI_SELECT (5)
#define _CLACT_SOFTPRI_POKELIST  (6)



static void IRC_POKETRADE_TrayInit(IRC_POKEMON_TRADE* pWork,int subchar);
static void IRC_POKETRADE_TrayExit(IRC_POKEMON_TRADE* pWork);
static int _DotToLine(int pos);
static void PokeIconCgxLoad(IRC_POKEMON_TRADE* pWork );
static void IRC_POKETRADE_PokeIcomPosSet(IRC_POKEMON_TRADE* pWork);


static int _Line2RingLineIconGet(int line);
static void _Line2RingLineSet(IRC_POKEMON_TRADE* pWork,int add);
static void _iconAllDrawDisable(IRC_POKEMON_TRADE* pWork);


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



//------------------------------------------------------------------
/**
 * $brief   ���������̃O���t�B�b�N������
 * @param   IRC_POKEMON_TRADE   ���[�N�|�C���^
 * @retval  none
 */
//------------------------------------------------------------------

void IRC_POKETRADE_GraphicInitMainDisp(IRC_POKEMON_TRADE* pWork)
{
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );


	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_trade_wb_trade_bg_NCLR,
																		PALTYPE_MAIN_BG, 0, 0,  pWork->heapID);


	// �T�u���BG�L�����]��
  if(pWork->subchar == 0){
    pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_trade_wb_trade_bg01_NCGR,
                                                                  GFL_BG_FRAME2_M, 0, 0, pWork->heapID);
  }

	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_trade_bg01_NSCR,
																				 GFL_BG_FRAME1_M, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
																				 pWork->heapID);
	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_trade_bg01_back_NSCR,
																				 GFL_BG_FRAME2_M, 0,
                                         GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                         pWork->heapID);
	GFL_ARC_CloseDataHandle( p_handle );


  
}


//------------------------------------------------------------------
/**
 * $brief   ���������̃O���t�B�b�N������
 * @param   IRC_POKEMON_TRADE   ���[�N�|�C���^
 * @retval  none
 */
//------------------------------------------------------------------

void IRC_POKETRADE_GraphicInitSubDisp(IRC_POKEMON_TRADE* pWork)
{
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );

	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_trade_wb_trade_bg_NCLR,
																		PALTYPE_SUB_BG, 0, 0,  pWork->heapID);


	// �T�u���BG�L�����]��
	pWork->subchar2 = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_trade_wb_trade_bg02_NCGR,
																																GFL_BG_FRAME2_S, 0, 0, pWork->heapID);
	pWork->subchar1 = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_trade_wb_trade_bg02_NCGR,
																																GFL_BG_FRAME1_S, 0, 0, pWork->heapID);


	GFL_ARC_CloseDataHandle( p_handle );


  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME3_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
  
  IRC_POKETRADE_TrayInit(pWork,pWork->subchar2);

  PokeIconCgxLoad( pWork );
  IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork);

	pWork->pAppTaskRes	= APP_TASKMENU_RES_Create( GFL_BG_FRAME3_S, _SUBLIST_NORMAL_PAL,
			pWork->pFontHandle, pWork->SysMsgQue, pWork->heapID );



  if(pWork->cellRes[PLT_COMMON]==0){
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), pWork->heapID );
    pWork->cellRes[CHAR_COMMON] =
      GFL_CLGRP_CGR_Register( p_handle , APP_COMMON_GetBarIconCharArcIdx() ,
                              FALSE , CLSYS_DRAW_SUB , pWork->heapID );
    pWork->cellRes[PLT_COMMON] =
      GFL_CLGRP_PLTT_RegisterEx(
        p_handle ,APP_COMMON_GetBarIconPltArcIdx() , CLSYS_DRAW_SUB ,
        _OBJPLT_COMMON_OFFSET, 0, _OBJPLT_COMMON, pWork->heapID  );
    pWork->cellRes[ANM_COMMON] =
      GFL_CLGRP_CELLANIM_Register(
        p_handle , APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K),
        APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K) , pWork->heapID  );
    GFL_ARC_CloseDataHandle( p_handle );
  }

  
}

void IRC_POKETRADE_GraphicFreeVram(IRC_POKEMON_TRADE* pWork)
{

  if(pWork->subchar){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_M,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar),
                             GFL_ARCUTIL_TRANSINFO_GetSize( pWork->subchar ));
  }
  if(pWork->subchar1){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1),
                             GFL_ARCUTIL_TRANSINFO_GetSize( pWork->subchar1 ));
  }
  if(pWork->subchar2){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar2),
                             GFL_ARCUTIL_TRANSINFO_GetSize( pWork->subchar2 ));
  }
  if(pWork->bgchar){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME3_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
                             GFL_ARCUTIL_TRANSINFO_GetSize( pWork->bgchar ));
  }
  pWork->subchar = 0;
  pWork->subchar1 = 0;
  pWork->subchar2 = 0;
  pWork->bgchar = 0;
  
}



void IRC_POKETRADE_G3dDraw(IRC_POKEMON_TRADE* pWork)
{
//		G3X_EdgeMarking( FALSE );
//  GFL_G3D_CAMERA_Switching( pWork->p_camera );  
//  GFL_G3D_DRAW_DrawObject( pWork->pG3dObj, &pWork->status );
}



void IRC_POKETRADE_GraphicExit(IRC_POKEMON_TRADE* pWork)
{
	APP_TASKMENU_RES_Delete( pWork->pAppTaskRes );
  IRC_POKETRADE_TrayExit(pWork);
}


void IRC_POKETRADE_SubStatusInit(IRC_POKEMON_TRADE* pWork,int pokeposx, int bgtype)
{
  int nscr[]={NARC_trade_wb_trade_stbg01_NSCR,NARC_trade_wb_trade_stbg02_NSCR};
  int frame = GFL_BG_FRAME0_S;
  
  
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 nscr[bgtype],
																				 frame, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1), 0, 0,
																				 pWork->heapID);

  if(pokeposx < 128){
    GFL_BG_SetScroll( frame, GFL_BG_SCROLL_X_SET, 128 );
  }
  else{
    GFL_BG_SetScroll( frame, GFL_BG_SCROLL_X_SET, 0 );
  }
	G2S_SetBlendAlpha( GFL_BG_FRAME2_S|GX_BLEND_PLANEMASK_OBJ, frame , 16, 3 );
  //G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_OBJ , -8 );
  GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], FALSE );

	GFL_ARC_CloseDataHandle( p_handle );
  GFL_BG_SetVisible( frame , TRUE );

}


void IRC_POKETRADE_SubStatusEnd(IRC_POKEMON_TRADE* pWork)
{
  int frame = GFL_BG_FRAME0_S;

  GFL_BG_SetVisible( frame , FALSE );

	GFL_BG_SetScroll( frame, GFL_BG_SCROLL_X_SET, 0 );
  G2S_BlendNone();
  GFL_BG_ClearScreen(frame);  //�����X�e�[�^�X
  GFL_BG_LoadScreenV_Req( frame );
  GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], TRUE );

}

void IRC_POKETRADE_AppMenuClose(IRC_POKEMON_TRADE* pWork)
{
  G2_BlendNone();
  APP_TASKMENU_CloseMenu(pWork->pAppTask);
}

void IRC_POKETRADE_AppMenuOpen(IRC_POKEMON_TRADE* pWork, int *menustr,int num)
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  num;
  appinit.itemWork =  &pWork->appitem[0];

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 24;
	appinit.w				 = APP_TASKMENU_PLATE_WIDTH;
	appinit.h				 = APP_TASKMENU_PLATE_HEIGHT;

  for(i=0;i<num;i++){
    pWork->appitem[i].str = GFL_STR_CreateBuffer(100, pWork->heapID);
    GFL_MSG_GetString(pWork->pMsgData, menustr[i], pWork->appitem[i].str);
    pWork->appitem[i].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  }
  pWork->pAppTask = APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
  for(i=0;i<num;i++){
    GFL_STR_DeleteBuffer(pWork->appitem[i].str);
  }
  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ , -8 );

}

void IRC_POKETRADE_MessageOpen(IRC_POKEMON_TRADE* pWork, int msgno)
{


  GFL_MSG_GetString( pWork->pMsgData, msgno, pWork->pMessageStrBufEx );

	WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBufEx, pWork->pMessageStrBuf  );

}

void IRC_POKETRADE_MessageWindowOpen(IRC_POKEMON_TRADE* pWork)
{
  GFL_BMPWIN* pwin;

  IRC_POKETRADE_MessageWindowClose(pWork);


  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);
  pWork->mesWin = GFL_BMPWIN_Create(GFL_BG_FRAME3_S , 1 , 1, 30 ,4 ,  _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  
  pwin = pWork->mesWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 15);


 pWork->pStream = PRINTSYS_PrintStream(pwin ,0,0, pWork->pMessageStrBuf, pWork->pFontHandle,
                                        MSGSPEED_GetWait(), pWork->pMsgTcblSys, 2, pWork->heapID, 15);

  
  BmpWinFrame_Write( pwin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

  GFL_BMPWIN_TransVramCharacter(pwin);
  GFL_BMPWIN_MakeScreen(pwin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);

}


void IRC_POKETRADE_MessageWindowClear(IRC_POKEMON_TRADE* pWork)
{

	if(pWork->mesWin){
		GFL_BMPWIN_ClearScreen(pWork->mesWin);
		GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);
		BmpWinFrame_Clear(pWork->mesWin, WINDOW_TRANS_OFF);
		GFL_BMPWIN_Delete(pWork->mesWin);
		pWork->mesWin=NULL;
	}
}



void IRC_POKETRADE_MessageWindowClose(IRC_POKEMON_TRADE* pWork)
{
  if(pWork->mesWin){
		GFL_BMPWIN_Delete(pWork->mesWin);
    pWork->mesWin=NULL;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�̏I���҂�
 * @retval  none
 */
//------------------------------------------------------------------------------

BOOL IRC_POKETRADE_MessageEndCheck(IRC_POKEMON_TRADE* pWork)
{
  if(pWork->pStream){
    int state = PRINTSYS_PrintStreamGetState( pWork->pStream );
    switch(state){
    case PRINTSTREAM_STATE_DONE:
      PRINTSYS_PrintStreamDelete( pWork->pStream );
      pWork->pStream = NULL;
      break;
    case PRINTSTREAM_STATE_PAUSE:
      if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
        PRINTSYS_PrintStreamReleasePause( pWork->pStream );
      }
      break;
    default:
      break;
    }
    return FALSE;  //�܂��I����ĂȂ�
  }
  return TRUE;// �I����Ă���
}

//------------------------------------------------------------------------------
/**
 * @brief   �g���C�̕\��
 * @retval  none
 */
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
/**
 * @brief   �X�N���[�����W����X�N���[���f�[�^�����o��
 * @retval  none
 */
//------------------------------------------------------------------------------

static u16 _GetScr(int x , int y, IRC_POKEMON_TRADE* pWork)
{
  int x2=x;

  if(x >= _SRCMAX){
    x2 = x - _SRCMAX;
  }
  if(x2 < _TEMOTITRAY_SCR_MAX){
    return pWork->scrTemoti[ 18+(y * 32) + x2 ];
  }
  x2 = x2 - _TEMOTITRAY_SCR_MAX;
  x2 = x2 % _BOXTRAY_SCR_MAX;
  return pWork->scrTray[ 18+(y * 32) + x2 ];
}


void IRC_POKETRADE_TrayDisp(IRC_POKEMON_TRADE* pWork)
{
  int bgscr = pWork->BoxScrollNum / 8;  //�}�X�̒P�ʂ͉�ʃX�N���[�����g��
  int frame = GFL_BG_FRAME2_S;
  int x,y;

  for(y = 0; y < 24 ; y++){
    for(x = 0; x < (_BOXTRAY_SCR_MAX*2+_TEMOTITRAY_SCR_MAX) ; x++){
      u16 scr = _GetScr( bgscr + x , y ,pWork);
//      GFL_BG_WriteScreen(frame, &scr, x, y, 1, 1 );
      GFL_BG_ScrSetDirect(frame, x, y, scr);
    }
  }

  pWork->bgscroll = pWork->BoxScrollNum % 8;  //�}�X�̒P�ʂ͉�ʃX�N���[�����g��
  pWork->bgscrollRenew = TRUE;

  
  GFL_BG_LoadScreenV_Req(frame);

}


static void IRC_POKETRADE_TrayInit(IRC_POKEMON_TRADE* pWork,int subchar)
{
  // �X�N���[����ǂݍ���ł���
  pWork->scrTray = GFL_ARC_LoadDataAlloc( ARCID_POKETRADE, NARC_trade_wb_trade_bg02_NSCR, pWork->heapID);
  pWork->scrTemoti = GFL_ARC_LoadDataAlloc( ARCID_POKETRADE, NARC_trade_wb_trade_bg03_NSCR, pWork->heapID);


  {
    int i;
    for(i=0;i<(32*24);i++){
      pWork->scrTray[18+i] += GFL_ARCUTIL_TRANSINFO_GetPos(subchar);
      pWork->scrTemoti[18+i] += GFL_ARCUTIL_TRANSINFO_GetPos(subchar);
    }
  }

  pWork->BoxScrollNum = _DOTMAX - 80;

  IRC_POKETRADE_TrayDisp(pWork);
//  IRC_POKETRADE_TrayDisp(pWork);
}

static void IRC_POKETRADE_TrayExit(IRC_POKEMON_TRADE* pWork)
{
  GFL_HEAP_FreeMemory( pWork->scrTray);
  GFL_HEAP_FreeMemory( pWork->scrTemoti);
}

//------------------------------------------------------------------------------
/**
 * @brief   �{�b�N�X�����܂Ƃ߂�VRAM�ɑ���
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_SendVramBoxNameChar(IRC_POKEMON_TRADE* pWork)
{
  int i;
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

  for(i=0;i<BOX_MAX_TRAY+1;i++){
    if(pWork->BoxNameWin[i]==NULL){
      pWork->BoxNameWin[i] = GFL_BMPWIN_Create(GFL_BG_FRAME1_S, (i % 3) * 10 , (i / 3) * 2, 10, 2, _BUTTON_MSG_PAL,GFL_BMP_CHRAREA_GET_B);
    }
    if(i == BOX_MAX_TRAY){
      GFL_MSG_GetString(  pWork->pMsgData, POKETRADE_STR_19, pWork->pStrBuf );
    }
    else{
      BOXDAT_GetBoxName(pWork->pBox, i, pWork->pStrBuf);
    }
    GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->BoxNameWin[i]), 0 );
//   GFL_BMPWIN_MakeScreen(pWork->BoxNameWin[i]);
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->BoxNameWin[i]), 0, 0, pWork->pStrBuf, pWork->pFontHandle);
    GFL_BMPWIN_TransVramCharacter(pWork->BoxNameWin[i]);

    GFL_BMPWIN_SetPosY( pWork->BoxNameWin[i], 0 );  //�O�ɕ␳
  }
//  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );
}

//------------------------------------------------------------------------------
/**
 * @brief   �{�b�N�X���̃E�C���h�E�������J��
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_ResetBoxNameWindow(IRC_POKEMON_TRADE* pWork)
{
  int i;

  for(i=0;i<BOX_MAX_TRAY+1;i++){
    if(pWork->BoxNameWin[i]){
      GFL_BMPWIN_Delete(pWork->BoxNameWin[i] );
      pWork->BoxNameWin[i]=NULL;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �{�b�N�X�����X�N���[���ɓ]��
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_SendScreenBoxNameChar(IRC_POKEMON_TRADE* pWork)
{
  int bgscr,i,disp,x,y;
  int spos,pos = pWork->BoxScrollNum / 8;


  for(y=0;y<2;y++){
    for(x=0;x<64;x++){
      GFL_BG_ScrSetDirect(GFL_BG_FRAME1_S, x, y, 0);
    }
  }


  
  if(pWork->BoxScrollNum < _TEMOTITRAY_MAX){  //�莝����Ԃł���
    bgscr = BOX_MAX_TRAY;
  }
  else{
    bgscr = (pWork->BoxScrollNum - _TEMOTITRAY_MAX) / _BOXTRAY_MAX;
  }
  for(disp = 0; disp < 3; disp++){ //�O��BOX��\��
    i = bgscr+disp;
    if(i > BOX_MAX_TRAY){
      i = i-BOX_MAX_TRAY-1;
    }
    else if(i < 0){
      i = i + BOX_MAX_TRAY + 1;
    }
    {
      //�����̊�l
      if(i==BOX_MAX_TRAY){
        if(pos > (_SRCMAX - pos)){
          spos = _SRCMAX - pos;
          pos = pos - _SRCMAX;
        }
        else{
          spos = 0 - pos;
        }
      }
      else{
        spos = (_TEMOTITRAY_SCR_MAX + _BOXTRAY_SCR_MAX * i) - pos;  //
      }
      if(spos < -10){
        continue;
      }
      if(spos > 33){
        continue;
      }
      GFL_BMPWIN_SetPosX( pWork->BoxNameWin[i], spos );
      GFL_BMPWIN_MakeScreen(pWork->BoxNameWin[i]);
    }
  }
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R�����\�[�X�̍쐬
 * @param   IRC_POKEMON_TRADE
 * @retval  none
 */
//------------------------------------------------------------------------------


void IRC_POKETRADE_CreatePokeIconResource(IRC_POKEMON_TRADE* pWork)
{
  //�|�P�A�C�R���p���\�[�X
  //�L�����N�^�͊e�v���[�g��
#if 1
  {
    int line,i;
    ARCHANDLE *arcHandlePoke = GFL_ARC_OpenDataHandle( ARCID_POKEICON , pWork->heapID );
    pWork->cellRes[PLT_POKEICON] =
      GFL_CLGRP_PLTT_RegisterComp( arcHandlePoke ,
                                   POKEICON_GetPalArcIndex() , CLSYS_DRAW_SUB ,
                                   _OBJPLT_POKEICON_OFFSET , pWork->heapID  );

    pWork->cellRes[PLT_POKEICON_GRAY] =
      GFL_CLGRP_PLTT_RegisterComp( arcHandlePoke ,
                                   POKEICON_GetPalArcIndex() , CLSYS_DRAW_SUB ,
                                   _OBJPLT_POKEICON_GRAY_OFFSET , pWork->heapID  );

    {
      PALETTE_FADE_PTR pP = PaletteFadeInit(pWork->heapID);

      PaletteFadeWorkAllocSet(pP, FADE_SUB_OBJ, 16*32, pWork->heapID);
      PaletteWorkSetEx_ArcHandle(pP,arcHandlePoke ,POKEICON_GetPalArcIndex() ,pWork->heapID,
                                 FADE_SUB_OBJ, _OBJPLT_POKEICON_GRAY*32, _OBJPLT_POKEICON_GRAY_OFFSET/2, 0);
      SoftFadePfd(pP, FADE_SUB_OBJ, _OBJPLT_POKEICON_GRAY_OFFSET/2, _OBJPLT_POKEICON_GRAY*16, 0, 0x7b1a);
      PaletteFadeTrans(pP);
      PaletteFadeWorkAllocFree(pP,FADE_SUB_OBJ);
      PaletteFadeFree(pP);
    }

    
    
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
        cellInitData.softpri = _CLACT_SOFTPRI_POKELIST;
        cellInitData.bgpri = 3;

        pWork->pokeIconNcgRes[line][i] = GFL_CLGRP_CGR_Register( arcHandlePoke ,NARC_poke_icon_poke_icon_tam_NCGR , FALSE , CLSYS_DRAW_SUB , pWork->heapID );
        pWork->pokeIcon[line][i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                        pWork->pokeIconNcgRes[line][i],
                                                        pWork->cellRes[PLT_POKEICON],
                                                        pWork->cellRes[ANM_POKEICON],
                                                        &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
        cellInitData.anmseq = 0;
        cellInitData.softpri = _CLACT_SOFTPRI_SELECT;
        pWork->markIcon[line][i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                        pWork->cellRes[CHAR_SCROLLBAR],
                                                        pWork->cellRes[PAL_SCROLLBAR],
                                                        pWork->cellRes[ANM_SCROLLBAR],
                                                        &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );

        GFL_CLACT_WK_SetAutoAnmFlag( pWork->pokeIcon[line][i] , FALSE );
        GFL_CLACT_WK_SetDrawEnable( pWork->pokeIcon[line][i], FALSE );
        GFL_CLACT_WK_SetDrawEnable( pWork->markIcon[line][i], FALSE );
      }
    }
    GFL_ARC_CloseDataHandle(arcHandlePoke);
  }
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   �K�v�Ȃ��Ȃ������C���̃��\�[�X�J��
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _deletePokeIconResource(IRC_POKEMON_TRADE* pWork, int line)
{
  int i;
  
	for( i = 0 ; i < BOX_VERTICAL_NUM ; i++ ){
		if(pWork->pokeIconNcgRes[line][i]){
			GFL_CLGRP_CGR_Release(pWork->pokeIconNcgRes[line][i]);
			pWork->pokeIconNcgRes[line][i] = NULL;
		}
		if(pWork->pokeIcon[line][i]){
			GFL_CLACT_WK_Remove(pWork->pokeIcon[line][i]);
			pWork->pokeIcon[line][i]=NULL;
		}
	}
}

void IRC_POKETRADE_AllDeletePokeIconResource(IRC_POKEMON_TRADE* pWork)
{
  int i;
  
  for(i = 0 ; i < _LING_LINENO_MAX ; i++){
    _deletePokeIconResource(pWork,i);
  }

  GFL_HEAP_FreeMemory(pWork->pCharMem);
  pWork->pCharMem = NULL;
}

static void _calcPokeIconPos(int line,int index, GFL_CLACTPOS* pos)
{
	static const u8	iconSize = 24;
	static const u8 iconTop = 72;
	static const u8 iconLeft = 24;

	pos->x = line * iconSize + iconLeft;
	pos->y = index * iconSize + iconTop;
}


static POKEMON_PASO_PARAM* _getPokeDataAddress(BOX_MANAGER* boxData , int lineno, int verticalindex , IRC_POKEMON_TRADE* pWork, BOOL* bTemoti)
{
  if(lineno >= HAND_HORIZONTAL_NUM){
    int tray = LINE2TRAY(lineno);
    int index = LINE2POKEINDEX(lineno, verticalindex);
    *bTemoti = FALSE;
    return IRCPOKEMONTRADE_GetPokeDataAddress(boxData, tray, index, pWork);
  }
	else{
    if(verticalindex <3){
      int index = lineno * 3 + verticalindex;
      *bTemoti = TRUE;
      return IRCPOKEMONTRADE_GetPokeDataAddress(boxData, BOX_MAX_TRAY, index, pWork);
		}
	}
	return NULL;
}


//�J�[�\���̈ʒu���Z�b�g����
void IRC_POKETRADE_SetCursorXY(IRC_POKEMON_TRADE* pWork)
{
  int k;
  GFL_CLACTPOS apos;

  k =  _Line2RingLineIconGet(pWork->MainObjCursorLine);
  GFL_CLACT_WK_GetPos( pWork->markIcon[k][pWork->MainObjCursorIndex],&apos,CLSYS_DRAW_SUB);
  pWork->x = apos.x;
  pWork->y = apos.y;
}

//���C���C���f�b�N�X�̈ʒu�ɃJ�[�\���\��
void IRC_POKETRADE_CursorEnable(IRC_POKEMON_TRADE* pWork,int line,int index)
{
  int i,j;

  for(j=0;j<_LING_LINENO_MAX;j++){
    for(i=0;i<BOX_VERTICAL_NUM;i++){
      GFL_CLACT_WK_SetDrawEnable(pWork->markIcon[j][i],FALSE);
    }
  }
  if(_LING_LINENO_MAX <= line){
    return;
  }
  if(BOX_VERTICAL_NUM <= index){
    return;
  }
  
  GFL_CLACT_WK_SetDrawEnable( pWork->markIcon[line ][index], TRUE );
}



#if 1
#include "tradehead.cdat"

static BOOL _IsPokeLanguageMark(IRC_POKEMON_TRADE* pWork,int monsno)
{
  if(pWork->selectMoji){
    int i = pWork->selectMoji - 1;

    if(i==MonsterHeadLangTable[monsno]){
      return TRUE;
    }
  }
  return FALSE;
}
#endif

//���C���ɂ��킹���|�P�����\��
static void _createPokeIconResource(IRC_POKEMON_TRADE* pWork,BOX_MANAGER* boxData ,int line, int k)
{
  int i,j;
  
  POKEMON_PASO_PARAM* ppp;
	void *obj_vram = G2S_GetOBJCharPtr();
	ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_POKEICON , pWork->heapID );


  if(line == pWork->MainObjCursorLine){
    GFL_CLACT_WK_SetDrawEnable( pWork->markIcon[k][pWork->MainObjCursorIndex], TRUE );
  }
  

	for( i = 0 ; i < BOX_VERTICAL_NUM ; i++ )
	{
    {
      int	fileNo,monsno,formno,bEgg,bTemoti;
      ppp = _getPokeDataAddress(boxData, line, i, pWork, &bTemoti);
      if(!ppp){
        continue;
      }
      pWork->pokeIconLine[k][i] = line;
      monsno = PPP_Get(ppp,ID_PARA_monsno,NULL);
      if( monsno == 0 ){	//�|�P���������邩�̃`�F�b�N
        continue;
      }
      else if(pWork->pokeIconNo[k][i] == monsno){
        GFL_CLACT_WK_SetDrawEnable( pWork->pokeIcon[k][i], TRUE );
      }
      else if(pWork->pokeIconNo[k][i] != monsno){
        u8 pltNum;
        GFL_CLACTPOS pos;
        NNSG2dImageProxy aproxy;
          
        pWork->pokeIconNo[k][i] = monsno;
        
        pltNum = POKEICON_GetPalNumGetByPPP( ppp );
        _calcPokeIconPos(line, i, &pos);

        GFL_CLACT_WK_GetImgProxy( pWork->pokeIcon[k][i], &aproxy );
        
        GFL_STD_MemCopy(&pWork->pCharMem[4*8*4*4*monsno] , (char*)((u32)obj_vram) + aproxy.vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DSUB], 4*8*4*4);

        if(bTemoti){
          for(i=0;i<4;i++){
            int machine = PPP_Get(ppp,POKEPER_ID_machine1+i,NULL);
            if(ITEM_CheckHidenWaza(machine)){
              pltNum-=_OBJPLT_POKEICON_GRAY; //
              break;
            }
          }
        }
        
        GFL_CLACT_WK_SetPlttOffs( pWork->pokeIcon[k][i] , pltNum , CLWK_PLTTOFFS_MODE_PLTT_TOP );


        GFL_CLACT_WK_SetAutoAnmFlag( pWork->pokeIcon[k][i] , FALSE );
        GFL_CLACT_WK_SetDrawEnable( pWork->pokeIcon[k][i], TRUE );
      }
      if(_IsPokeLanguageMark(pWork,monsno)){
        GFL_CLACT_WK_SetDrawEnable( pWork->markIcon[k][i], TRUE );
      }
    }
  }
  GFL_ARC_CloseDataHandle( arcHandle );
}


//------------------------------------------------------------------------------
/**
 * @brief   BoxScrollNum�̒l����Line�̊�{�l�𓾂�
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static int _boxScrollNum2Line(IRC_POKEMON_TRADE* pWork)
{
  int line,i;
  
  if(pWork->BoxScrollNum < (_TEMOTITRAY_MAX/2)){
    line = 0;
  }
  else if(pWork->BoxScrollNum < _TEMOTITRAY_MAX){
    line = 1;
  }
  else{
    i = (pWork->BoxScrollNum - _TEMOTITRAY_MAX) / _BOXTRAY_MAX; //BOX��
    line = i * 6 + 2;
    line += ((pWork->BoxScrollNum - _TEMOTITRAY_MAX) - (i*_BOXTRAY_MAX)) / 27 ;
    //OS_TPrintf("LINE %d %d\n", pWork->BoxScrollNum, line);
  }
  return line;
}





void IRC_POKETRADE_InitBoxIcon( BOX_MANAGER* boxData ,IRC_POKEMON_TRADE* pWork )
{
  int j,k,i,line = _boxScrollNum2Line(pWork);

  if( pWork->oldLine != line ){
    pWork->oldLine = line;

    for(j=0;j<_LING_LINENO_MAX;j++){
      for(i=0;i<BOX_VERTICAL_NUM;i++){
        GFL_CLACT_WK_SetDrawEnable(pWork->markIcon[j][i],FALSE);
      }
    }

    
    _Line2RingLineSet(pWork,line);
    _iconAllDrawDisable(pWork);  // �A�C�R���\������U����
  
    for(i=0;i < _LING_LINENO_MAX;i++){
      k =  _Line2RingLineIconGet(pWork->oldLine+i);

      
      _createPokeIconResource(pWork, boxData,line, k );  //�A�C�R���\��
      line++;
      if(line >= TRADEBOX_LINEMAX){
        line=0;
      }
    }
  }
  IRC_POKETRADE_PokeIcomPosSet(pWork);
  
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���ꏊ�v�Z
 * @param   line           ��
 * @param   m              �\�����C���Ƃ̍���
 * @param   vertical       �c
 * @param   boxscrollnum   �X�N���[���h�b�g
 * @param   GFL_CLACTPOS   �v�Z�����ʒu
 * @retval  none
 */
//------------------------------------------------------------------------------
#if 0
static void _pokeIcomPosCalc(int line,int m , int vertical, int boxscrollnum,GFL_CLACTPOS* pPos)
{
  int line2, i, no, subnum=0;
  int x,y;

  line2 = line;

  no = _Line2RingLineIconGet(line + m);
  
  if(line2 >= TRADEBOX_LINEMAX){
    line2 = line2 - TRADEBOX_LINEMAX;
    subnum = _DOTMAX;
  }

  y = 32+vertical*24;

  if(line2 == 0){
    x = 16;
  }
  else if(line2 == 1){
    x = 56;
  }
  else{
    x = ((line2 - 2) / 6) * _BOXTRAY_MAX;
    x += ((line2 - 2) % 6) * 24 + 16 + 4;
    x += _TEMOTITRAY_MAX;
  }
  x = x - boxscrollnum + subnum;
  pPos->x = x;
  pPos->y = y;
}
#endif


static void IRC_POKETRADE_PokeIcomPosSet(IRC_POKEMON_TRADE* pWork)
{
  int line, i, no, m, subnum=0;
  int x,y;
  //int bgscr = pWork->BoxScrollNum;
  
  line = pWork->oldLine;
  for(m = 0; m < _LING_LINENO_MAX; m++,line++){
    no = _Line2RingLineIconGet(pWork->oldLine + m);

    if(line >= TRADEBOX_LINEMAX){
      line = line - TRADEBOX_LINEMAX;
      subnum = _DOTMAX;
    }

    for(i=0;i<BOX_VERTICAL_NUM;i++){
      GFL_CLACTPOS apos;
      y = 32+i*24;
      if(line == 0){
        x = 16;
      }
      else if(line == 1){
        x = 56;
      }
      else{
        x = ((line - 2) / 6) * _BOXTRAY_MAX;
        x += ((line - 2) % 6) * 24 + 16 + 4;
        x += _TEMOTITRAY_MAX;
      }
      x = x - pWork->BoxScrollNum + subnum;
      apos.x = x;
      apos.y = y;


      GFL_CLACT_WK_SetPos(pWork->pokeIcon[no][i], &apos, CLSYS_DRAW_SUB);
      apos.y = y + 3;
      GFL_CLACT_WK_SetPos(pWork->markIcon[no][i], &apos, CLSYS_DRAW_SUB);
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �h�b�g�̈ʒu
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static int _DotToLine(int pos)
{
  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���L�����f�[�^���������ɓW�J
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	ppp		POKEMON_PASO_PARAM
 * @param	chr		NNSG2dCharacterData
 *
 * @return	buf
 */
//--------------------------------------------------------------------------------------------
static void  PokeIconCgxLoad(IRC_POKEMON_TRADE* pWork )
{
  int i;
  void* pMem;
	u32	arcIndex;
  ARCHANDLE * pokeicon_ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, pWork->heapID );
  NNSG2dCharacterData* pCharData;


  GF_ASSERT(MONSNO_MAX < 650);
  pWork->pCharMem = GFL_HEAP_AllocMemory(pWork->heapID, 4*8*4*4*650 );
  
  for(i=0;i<MONSNO_MAX; i++){ //@todo �t�H�����Ⴂ�������Ă���K�v����
  
    arcIndex = POKEICON_GetCgxArcIndexByMonsNumber( i, 0, 0 );
    pMem = GFL_ARCHDL_UTIL_LoadBGCharacter(pokeicon_ah, arcIndex, FALSE, &pCharData, pWork->heapID);

    GFL_STD_MemCopy(pCharData->pRawData,&pWork->pCharMem[4*8*4*4*i] , 4*8*4*4);

    GFL_HEAP_FreeMemory(pMem);
    
  }

  GFL_ARC_CloseDataHandle(pokeicon_ah);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �����O�o�b�t�@�̃��C���ԍ���Ԃ�
 * @return	buf
 */
//--------------------------------------------------------------------------------------------

static int _Line2RingLineIconGet(int line)
{
  int ret;

  ret = line % _LING_LINENO_MAX;
  return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �����O�o�b�t�@��i�߂違���ǂ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	ppp		POKEMON_PASO_PARAM
 * @param	chr		NNSG2dCharacterData
 *
 * @return	buf
 */
//--------------------------------------------------------------------------------------------

static void _Line2RingLineSet(IRC_POKEMON_TRADE* pWork,int add)
{
  int ret;

  pWork->ringLineNo = add;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�R���\����S���֎~
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	ppp		POKEMON_PASO_PARAM
 * @param	chr		NNSG2dCharacterData
 *
 * @return	buf
 */
//--------------------------------------------------------------------------------------------

static void _iconAllDrawDisable(IRC_POKEMON_TRADE* pWork)
{
  int line,i;
  
  for(line =0 ;line < _LING_LINENO_MAX; line++)
  {
    for(i = 0;i < BOX_VERTICAL_NUM; i++)
    {
      GFL_CLACT_WK_SetDrawEnable( pWork->pokeIcon[line][i], FALSE );
    }
  }
}




GFL_CLWK* IRC_POKETRADE_GetCLACT( IRC_POKEMON_TRADE* pWork , int x, int y, int* trayno, int* pokeindex, int* outline, int* outindex)
{
  GFL_CLACTPOS apos;
  int line,i,line2;

  for(line =0 ;line < _LING_LINENO_MAX; line++)
  {
    for(i = 0;i < BOX_VERTICAL_NUM; i++)
    {
      GFL_CLACT_WK_GetPos( pWork->pokeIcon[line][i], &apos ,CLSYS_DRAW_SUB );
      if((apos.x <= x) && (x < (apos.x+24))){
        if((apos.y <= y) && (y < (apos.y+24))){

          if(trayno!=NULL){//�ʒu���𓾂����ꍇ�Ɍv�Z

            line2 = pWork->pokeIconLine[line][i];
            *trayno = LINE2TRAY(line2);
            *pokeindex = LINE2POKEINDEX(line2, i);
          }
          if(outline != NULL){
            *outline =line2;
            *outindex = i;
          }
          return pWork->pokeIcon[line][i];
          //          line += BoxScrollNum(pWork);
        }
      }
    }
  }
  return NULL;
}



//--------------------------------------------------------------------------------------------
/**
 * �f���p����ʕ\��
 * @param	   pWork  ���[�N
 * @return	 none
 */
//--------------------------------------------------------------------------------------------

void IRC_POKETRADE_SetSubdispGraphicDemo(IRC_POKEMON_TRADE* pWork)
{
  int frame = GFL_BG_FRAME3_S;

  IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE_DEMO);

  {
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GX_BG_SCRSIZE_256x16PLTT_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
			};
    GFL_BG_SetBGControl( frame, &TextBgCntDat, GFL_BG_MODE_256X16 );
  }

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADEDEMO, pWork->heapID );

    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_tradedemo_under01_NCLR,
                                      PALTYPE_SUB_BG_EX, 0x6000, 0,  pWork->heapID);


    GFL_ARCHDL_UTIL_TransVramBgCharacter(p_handle, NARC_tradedemo_under01_NCGR,
                                      frame,0,0,0,pWork->heapID);
    GFL_ARCHDL_UTIL_TransVramScreen(p_handle,NARC_tradedemo_under01_NSCR,frame, 0,0,0,pWork->heapID);
  
    GFL_BG_SetScroll(frame,GFL_BG_SCROLL_X_SET, 0);
    GFL_ARC_CloseDataHandle( p_handle );
    
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   BG�̈�ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_SetMainDispGraphic(IRC_POKEMON_TRADE* pWork)
{
  {
    static const GFL_DISP_VRAM vramBank = {
      GX_VRAM_BG_64_E,				// ���C��2D�G���W����BG
      GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
      GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
      GX_VRAM_SUB_BGEXTPLTT_32_H,		// �T�u2D�G���W����BG�g���p���b�g
      GX_VRAM_OBJ_16_G,				// ���C��2D�G���W����OBJ
      GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g
      GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
      GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
      GX_VRAM_TEX_01_AB,				// �e�N�X�`���C���[�W�X���b�g
      GX_VRAM_TEXPLTT_0_F,			// �e�N�X�`���p���b�g�X���b�g
      GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
      GX_OBJVRAMMODE_CHAR_1D_128K,		// �T�uOBJ�}�b�s���O���[�h
    };
    GFL_DISP_SetBank( &vramBank );
    GFL_CLACT_SYS_Create(	&fldmapdata_CLSYS_Init, &vramBank, pWork->heapID );
  }

  IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE_NORMAL);

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
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,
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
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x08000, 0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &bgcntText, GFL_BG_MODE_TEXT );
    //		GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }

  GFL_BG_SetBGControl3D( 0 );
  GFL_BG_SetVisible( GFL_BG_FRAME0_M , TRUE );

}



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
  G3X_AlphaBlend( FALSE );		// �A���t�@�u�����h�@�I�t
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
//  GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
  G2_SetBG0Priority(2);
}



//----------------------------------------------------------------------------
/**
 *	@brief	���Z�b�g�A�b�v�R�[���o�b�N�֐�
 */
//-----------------------------------------------------------------------------
void IRC_POKETRADE_3DGraphicSetUp( IRC_POKEMON_TRADE* pWork )
{
  //3D�n�̏�����
  { //3D�n�̐ݒ�
    static const VecFx32 cam_pos = {FX32_CONST(0.0f),FX32_CONST(0.0f),FX32_CONST(200.0f)};
    static const VecFx32 cam_target = {FX32_CONST(0.0f),FX32_CONST(0.0f),FX32_CONST(0.0f)};
    static const VecFx32 cam_up = {0,FX32_ONE,0};
    //�G�b�W�}�[�L���O�J���[
    static  const GXRgb edge_color_table[8]=
    { GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K,
                  0, pWork->heapID, Graphic_3d_SetUp );

    //GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
    
#if 1

    pWork->pCamera   = GFL_G3D_CAMERA_CreateDefault( &cam_pos, &cam_target, pWork->heapID );
#else    
    //�����ˉe
    pWork->pCamera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH,
                                             FX32_ONE*12.0f,
                                             FX32_ONE*-12.0f,
                                             FX32_ONE*-16.0f,
                                             FX32_ONE*16.0f,
                                             (FX32_ONE),
                                             (FX32_ONE*300),
                                             NULL,
                                             &cam_pos,
                                             &cam_up,
                                             &cam_target,
                                             pWork->heapID );
#endif

    GFL_G3D_CAMERA_Switching( pWork->pCamera );
    //�G�b�W�}�[�L���O�J���[�Z�b�g
    G3X_SetEdgeColorTable( edge_color_table );
    G3X_EdgeMarking( TRUE );

    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );

  }
}



//--------------------------------------------------------------------------------------------
/**
 * ���ʂ̉���ʕ\��
 * @param	   pWork  ���[�N
 * @return	 none
 */
//--------------------------------------------------------------------------------------------

void IRC_POKETRADE_SetSubDispGraphic(IRC_POKEMON_TRADE* pWork)
{

  {
    int frame = GFL_BG_FRAME0_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
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
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
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
      3, 0, 0, FALSE
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
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 24, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }



}



//--------------------------------------------------------------------------------------------
/**
 * @brief    ����ʔj��
 * @param	   pWork  ���[�N
 * @return	 none
 */
//--------------------------------------------------------------------------------------------

void IRC_POKETRADE_ResetSubDispGraphic(IRC_POKEMON_TRADE* pWork)
{

  GFL_BG_FillCharacterRelease(GFL_BG_FRAME0_S,1,0);
  GFL_BG_FillCharacterRelease(GFL_BG_FRAME3_S,1,0);
  GFL_BG_FreeBGControl(GFL_BG_FRAME0_S);
  GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
  GFL_BG_FreeBGControl(GFL_BG_FRAME2_S);
  GFL_BG_FreeBGControl(GFL_BG_FRAME3_S);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    BG���[�h��ύX
 * @param	   type
 * @return	 none
 */
//--------------------------------------------------------------------------------------------

void IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE type)
{
  static const GFL_BG_SYS_HEADER sysHeader[] = {
    {GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D},
    {GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_3,GX_BG0_AS_3D},
  };
  GF_ASSERT(type < elementof(sysHeader));
  GFL_BG_SetBGMode( &sysHeader[type] );
}


//------------------------------------------------------------------------------
/**
 * @brief   BOX�J�[�\���\��
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_InitBoxCursor(IRC_POKEMON_TRADE* pWork)
{
  u8 pltNum,i;
  GFL_CLACTPOS pos;

  //�f��
  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
    pWork->cellRes[CHAR_SCROLLBAR] =
      GFL_CLGRP_CGR_Register( p_handle , NARC_trade_wb_trade_obj01_NCGR ,
                              FALSE , CLSYS_DRAW_SUB , pWork->heapID );
    pWork->cellRes[PAL_SCROLLBAR] =
      GFL_CLGRP_PLTT_RegisterEx(
        p_handle ,NARC_trade_wb_trade_obj_NCLR , CLSYS_DRAW_SUB ,
        _OBJPLT_BOX_OFFSET,0, _OBJPLT_BOX, pWork->heapID  );
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
    cellInitData.softpri = _CLACT_SOFTPRI_SCROLL_BAR;
    cellInitData.bgpri = 1;
    pWork->curIcon[CELL_CUR_SCROLLBAR] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                             pWork->cellRes[CHAR_SCROLLBAR],
                                             pWork->cellRes[PAL_SCROLLBAR],
                                             pWork->cellRes[ANM_SCROLLBAR],
                                             &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_SCROLLBAR] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], TRUE );
  }

}

//------------------------------------------------------------------------------
/**
 * @brief   ���ʂ̃X�e�[�^�X�\��&����ʂ̑I���A�C�R���\��
 * @param   IRC_POKEMON_TRADE work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_SetMainStatusBG(IRC_POKEMON_TRADE* pWork)
{

	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_trade_stbg03_NSCR,
																				 GFL_BG_FRAME2_M, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1), 0, 0,
																				 pWork->heapID);
	GFL_ARC_CloseDataHandle( p_handle );
#if 1
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 128;
    cellInitData.pos_y = _POKEMON_SELECT1_CELLY + 4;
    cellInitData.anmseq = 3;
    cellInitData.softpri = _CLACT_SOFTPRI_SCROLL_BAR;
    cellInitData.bgpri = 1;
    pWork->curIcon[CELL_CUR_SCROLLBAR] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                                pWork->cellRes[CHAR_SCROLLBAR],
                                                                pWork->cellRes[PAL_SCROLLBAR],
                                                                pWork->cellRes[ANM_SCROLLBAR],
                                                                &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_SCROLLBAR] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], TRUE );


    cellInitData.pos_x = _POKEMON_SELECT1_CELLX;
    cellInitData.pos_y = _POKEMON_SELECT1_CELLY + 4;
    cellInitData.anmseq = 0;
    cellInitData.softpri = _CLACT_SOFTPRI_POKESEL;
    cellInitData.bgpri = 1;
    pWork->curIcon[CELL_CUR_POKE_SELECT] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                                pWork->cellRes[CHAR_SCROLLBAR],
                                                                pWork->cellRes[PAL_SCROLLBAR],
                                                                pWork->cellRes[ANM_SCROLLBAR],
                                                                &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_POKE_SELECT] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_POKE_SELECT], TRUE );

  }





#endif
  {
    GFL_CLWK_DATA cellInitData;
    ARCHANDLE *arcHandlePoke = GFL_ARC_OpenDataHandle( ARCID_POKEICON , pWork->heapID );
    const POKEMON_PASO_PARAM* ppp1 = PP_GetPPPPointerConst(IRC_POKEMONTRADE_GetRecvPP(pWork,0));
    const POKEMON_PASO_PARAM* ppp2 = PP_GetPPPPointerConst(IRC_POKEMONTRADE_GetRecvPP(pWork,1));

    pWork->cellRes[CHAR_SELECT_POKEICON1] =
      GFL_CLGRP_CGR_Register( arcHandlePoke , POKEICON_GetCgxArcIndex(ppp1) , FALSE , CLSYS_DRAW_SUB , pWork->heapID );
    pWork->cellRes[CHAR_SELECT_POKEICON2] =
      GFL_CLGRP_CGR_Register( arcHandlePoke , POKEICON_GetCgxArcIndex(ppp2) , FALSE , CLSYS_DRAW_SUB , pWork->heapID );

    cellInitData.pos_x = _POKEMON_SELECT1_CELLX;
    cellInitData.pos_y = _POKEMON_SELECT1_CELLY;
    cellInitData.anmseq = 0;
    cellInitData.softpri = _CLACT_SOFTPRI_POKESEL_BAR;
    cellInitData.bgpri = 1;
    pWork->curIcon[CELL_CUR_POKE_PLAYER] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                                pWork->cellRes[CHAR_SELECT_POKEICON1],
                                                                pWork->cellRes[PLT_POKEICON],
                                                                pWork->cellRes[ANM_POKEICON],
                                                                &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_POKE_PLAYER] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_POKE_PLAYER], TRUE );
  
  
    cellInitData.pos_x = _POKEMON_SELECT2_CELLX;
    pWork->curIcon[CELL_CUR_POKE_FRIEND] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                                pWork->cellRes[CHAR_SELECT_POKEICON2],
                                                                pWork->cellRes[PLT_POKEICON],
                                                                pWork->cellRes[ANM_POKEICON],
                                                                &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_POKE_FRIEND] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_POKE_FRIEND], TRUE );
    GFL_ARC_CloseDataHandle(arcHandlePoke);
  }





  
}




//------------------------------------------------------------------------------
/**
 * @brief   ���ʂ̃X�e�[�^�X�J�� & ����ʂ̑I���A�C�R���J��
 * @param   IRC_POKEMON_TRADE work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_ResetMainStatusBG(IRC_POKEMON_TRADE* pWork)
{
  
//  GFL_CLACT_WK_Remove(pWork->curIcon[CELL_CUR_SCROLLBAR]);
  GFL_CLACT_WK_Remove(pWork->curIcon[CELL_CUR_POKE_SELECT]);
  GFL_CLACT_WK_Remove(pWork->curIcon[CELL_CUR_POKE_PLAYER]);
  GFL_CLACT_WK_Remove(pWork->curIcon[CELL_CUR_POKE_FRIEND]);
//  pWork->curIcon[CELL_CUR_SCROLLBAR]=NULL;
  pWork->curIcon[CELL_CUR_POKE_SELECT]=NULL;
  pWork->curIcon[CELL_CUR_POKE_PLAYER]=NULL;
  pWork->curIcon[CELL_CUR_POKE_FRIEND]=NULL;
  GFL_CLGRP_CGR_Release(pWork->cellRes[CHAR_SELECT_POKEICON1] );
  GFL_CLGRP_CGR_Release(pWork->cellRes[CHAR_SELECT_POKEICON2] );
  pWork->cellRes[CHAR_SELECT_POKEICON1]=0;
  pWork->cellRes[CHAR_SELECT_POKEICON2]=0;
}



typedef struct{
  int x;
  int y;
} MojiTableStruct;

static MojiTableStruct mojitbl[]={
  1 , 6,
  1 , 9,
  1 ,12,
  1 ,15,
  1 ,18,
  4 , 6,
  4 , 9,
  4 ,12,
  4 ,15,
  4 ,18,
  7 , 6,
  7 , 9,
  7 ,12,
  7 ,15,
  7 ,18,
 10 , 6,
 10 , 9,
 10 ,12,
 10 ,15,
 10 ,18,
 13 , 6,
 13 , 9,
 13 ,12,
 13 ,15,
 13 ,18,

 16 , 6,
 16 , 9,
 16 ,12,
 16 ,15,
 16 ,18,

 19 , 6,
 19 , 9,
 19 ,12,
 19 ,15,
 19 ,18,

 22 , 6,
 22 ,12,
 22 ,18,
  
 25 , 6,
 25 , 9,
 25 ,12,
 25 ,15,
 25 ,18,
  
 28 , 6,
  
};


//------------------------------------------------------------------------------
/**
 * @brief   ����ʂɕ������͔\��
 * @param   IRC_POKEMON_TRADE work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_InitSubMojiBG(IRC_POKEMON_TRADE* pWork)
{
int i;
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_trade_bg04_NSCR,
																				 GFL_BG_FRAME0_S, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1), 0, 0,
																				 pWork->heapID);
  GFL_BG_SetVisible( GFL_BG_FRAME0_S , TRUE );
	GFL_ARC_CloseDataHandle( p_handle );




  GFL_FONTSYS_SetColor( 1, 2, 15 );
  for(i=0;i<elementof(mojitbl);i++){
    pWork->SerchMojiWin[i] =
      GFL_BMPWIN_Create(GFL_BG_FRAME3_S, mojitbl[i].x, mojitbl[i].y, 2, 2, _BUTTON_MSG_PAL,  GFL_BMP_CHRAREA_GET_B );
    GF_ASSERT(pWork->SerchMojiWin[i]);
    GFL_MSG_GetString(  pWork->pMsgData, POKETRADE_STR_53+i, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->SerchMojiWin[i]), 0, 0, pWork->pStrBuf, pWork->pFontHandle);
    GFL_BMPWIN_MakeScreen(pWork->SerchMojiWin[i]);
    GFL_BMPWIN_TransVramCharacter(pWork->SerchMojiWin[i]);
  }
  
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);

  
  if(pWork->curIcon[CELL_CUR_SCROLLBAR]){
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], FALSE );
  }
  if(pWork->curIcon[CHAR_LEFTPAGE_MARK]){
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CHAR_LEFTPAGE_MARK], FALSE );
  }


}


//------------------------------------------------------------------------------
/**
 * @brief   ����ʂɕ������͔\���I��
 * @param   IRC_POKEMON_TRADE work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_EndSubMojiBG(IRC_POKEMON_TRADE* pWork)
{
  int i;
  
  GFL_BG_SetVisible( GFL_BG_FRAME0_S , FALSE );

  if(pWork->curIcon[CELL_CUR_SCROLLBAR]){
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], TRUE );
  }
  if(pWork->curIcon[CHAR_LEFTPAGE_MARK]){
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CHAR_LEFTPAGE_MARK], TRUE );
  }

  for(i=0;i<elementof(mojitbl);i++){
    if(pWork->SerchMojiWin[i]){
      GFL_BMPWIN_ClearScreen(pWork->SerchMojiWin[i]);
      GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);
      BmpWinFrame_Clear(pWork->SerchMojiWin[i], WINDOW_TRANS_OFF);
      GFL_BMPWIN_Delete(pWork->SerchMojiWin[i]);
      pWork->SerchMojiWin[i]=NULL;
    }
	}

  
}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����X�e�[�^�X�ɃA�C�R���\��
 * @param   IRC_POKEMON_TRADE work
 * @param   side  ��ʕ\��  �E�Ȃ�P���Ȃ�O
 * @param   POKEMON_PARAM 
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_ItemIconDisp(IRC_POKEMON_TRADE* pWork,int side, POKEMON_PARAM* pp)
{
  // �|�P�A�C�R���p�A�C�e���A�C�R��
  {
    _ITEMMARK_ICON_WORK* pIM = &pWork->aItemMark;
    UI_EAYSY_CLWK_RES_PARAM prm;
    int item = PP_Get( pp , ID_PARA_item ,NULL);
    int type = 0;

    if(item == ITEM_DUMMY_ID){
      return;
    }
    
    if(ITEM_CheckMail(item)){
      type = 1;
    }

    
    prm.draw_type = CLSYS_DRAW_SUB;
    prm.comp_flg  = UI_EAYSY_CLWK_RES_COMP_NONE;
    prm.arc_id    = APP_COMMON_GetArcId();
    prm.pltt_id   = APP_COMMON_GetPokeItemIconPltArcIdx();
    prm.ncg_id    = APP_COMMON_GetPokeItemIconCharArcIdx();
    prm.cell_id   = APP_COMMON_GetPokeItemIconCellArcIdx( APP_COMMON_MAPPING_128K );
    prm.anm_id    = APP_COMMON_GetPokeItemIconAnimeArcIdx( APP_COMMON_MAPPING_128K );
    prm.pltt_line = PLTID_OBJ_POKEITEM_S;
    
    UI_EASY_CLWK_LoadResource( &pIM->clres_poke_item, &prm, pWork->cellUnit, pWork->heapID );

    // �A�j���V�[�P���X�Ŏw��( 0=�ǂ���, 1=���[��, 2=�{�[�� )
    // ���ʒu�����͂Ƃ肠�����̒l�ł��B
    pIM->clwk_poke_item =
      UI_EASY_CLWK_CreateCLWK( &pIM->clres_poke_item, pWork->cellUnit,
                               2 * 8 + side * 16 * 8 + 4, 17*8, type, pWork->heapID );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�R���\���J��
 * @param   _ITEMMARK_ICON_WORK  �A�C�e���}�[�N
 * @param   side                 ��ʕ\��  �E�Ȃ�P���Ȃ�O
 * @param   POKEMON_PARAM 
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_ItemIconReset(_ITEMMARK_ICON_WORK* pIM)
{
  if(pIM->clwk_poke_item){
    GFL_CLACT_WK_Remove( pIM->clwk_poke_item );
    UI_EASY_CLWK_UnLoadResource( &pIM->clres_poke_item );
    pIM->clwk_poke_item = NULL;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����X�e�[�^�X�ɃA�C�R���\��
 * @param   IRC_POKEMON_TRADE work
 * @param   side  ��ʕ\��  �E�Ȃ�P���Ȃ�O
 * @param   POKEMON_PARAM 
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_PokerusIconDisp(IRC_POKEMON_TRADE* pWork,int side,int bMain, POKEMON_PARAM* pp)
{
  // �|�P�A�C�R���p�A�C�e���A�C�R��
  IRC_POKETRADE_ItemIconReset(&pWork->aPokerusMark);
  
  {
    _ITEMMARK_ICON_WORK* pIM = &pWork->aPokerusMark;
    UI_EAYSY_CLWK_RES_PARAM prm;
    int rus = PP_Get( pp , ID_PARA_pokerus ,NULL);
    int type = 0;

    if(rus == 0){
      return;
    }
    
    if(bMain){
      prm.draw_type = CLSYS_DRAW_MAIN;
    }
    else{
      prm.draw_type = CLSYS_DRAW_SUB;
    }
    prm.comp_flg  = UI_EAYSY_CLWK_RES_COMP_NONE;
    prm.arc_id    = APP_COMMON_GetArcId();
    prm.pltt_id   = APP_COMMON_GetStatusIconPltArcIdx();
    prm.ncg_id    = APP_COMMON_GetStatusIconCharArcIdx();
    prm.cell_id   = APP_COMMON_GetStatusIconCellArcIdx( APP_COMMON_MAPPING_128K );
    prm.anm_id    = APP_COMMON_GetStatusIconAnimeArcIdx( APP_COMMON_MAPPING_128K );
    prm.pltt_line = PLTID_OBJ_POKEITEM_S;
    
    UI_EASY_CLWK_LoadResource( &pIM->clres_poke_item, &prm, pWork->cellUnit, pWork->heapID );

    if(bMain){
      pIM->clwk_poke_item =
        UI_EASY_CLWK_CreateCLWK( &pIM->clres_poke_item, pWork->cellUnit,22 * 8,13 * 8,type,pWork->heapID );
    }
    else{
      pIM->clwk_poke_item =
        UI_EASY_CLWK_CreateCLWK( &pIM->clres_poke_item, pWork->cellUnit,
                                 13 * 8 + side * 16 * 8 + 4, 3 * 8, type, pWork->heapID );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����X�e�[�^�X�ɃA�C�R���\��
 * @param   IRC_POKEMON_TRADE work
 * @param   POKEMON_PARAM  �\������|�P����
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_PokeStatusIconDisp(IRC_POKEMON_TRADE* pWork, POKEMON_PARAM* pp)
{
  int i,type;
  UI_EAYSY_CLWK_RES_PARAM prm;
  int mark = PP_Get( pp , ID_PARA_mark ,NULL);
  const int markpos[]= { 25, 26, 27, 28, 29, 30, 20, 21};
  const int marktbl[]= {
    APP_COMMON_POKE_MARK_CIRCLE_WHITE,    //��
    APP_COMMON_POKE_MARK_CIRCLE_BLACK,    //��
    APP_COMMON_POKE_MARK_TRIANGLE_WHITE,  //��
    APP_COMMON_POKE_MARK_TRIANGLE_BLACK,  //�� 
    APP_COMMON_POKE_MARK_SQUARE_WHITE,    //��
    APP_COMMON_POKE_MARK_SQUARE_BLACK,    //��
    APP_COMMON_POKE_MARK_HEART_WHITE,     //�n�[�g(��)
    APP_COMMON_POKE_MARK_HEART_BLACK,     //�n�[�g(��)
    APP_COMMON_POKE_MARK_STAR_WHITE,      //��
    APP_COMMON_POKE_MARK_STAR_BLACK,      //��
    APP_COMMON_POKE_MARK_DIAMOND_WHITE,   //��
    APP_COMMON_POKE_MARK_DIAMOND_BLACK,   //��
    APP_COMMON_POKE_MARK_STAR_RED,        //��(�ԁE���A�p)
    -1,
    APP_COMMON_POKE_MARK_POKERUSU,        //��(�|�P���X�����}�[�N)
    -1,
  };
  IRC_POKETRADE_PokeStatusIconReset(pWork);

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = UI_EAYSY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetPokeMarkPltArcIdx();
  prm.ncg_id    = APP_COMMON_GetPokeMarkCharArcIdx(APP_COMMON_MAPPING_128K);
  prm.cell_id   = APP_COMMON_GetPokeMarkCellArcIdx( APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetPokeMarkAnimeArcIdx( APP_COMMON_MAPPING_128K );
  prm.pltt_line = PLTID_OBJ_POKESTATE_M;

  for(i = 0;i < _POKEMARK_MAX ; i++){
    _ITEMMARK_ICON_WORK* pIM = &pWork->aPokeMark[i];

    if(mark & (1 << i)){
      type = marktbl[i*2];
    }
    else{
      type = marktbl[i*2+1];
    }
    if(type==-1){
      continue;
    }

    UI_EASY_CLWK_LoadResource( &pIM->clres_poke_item, &prm, pWork->cellUnit, pWork->heapID );

    pIM->clwk_poke_item =
      UI_EASY_CLWK_CreateCLWK( &pIM->clres_poke_item, pWork->cellUnit, markpos[i]*8 , 13 * 8, type, pWork->heapID );
  }

}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����X�e�[�^�X�ɃA�C�R���̑S������
 * @param   IRC_POKEMON_TRADE work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_PokeStatusIconReset(IRC_POKEMON_TRADE* pWork)
{
  int i;
  for(i = 0;i < _POKEMARK_MAX ; i++){
    _ITEMMARK_ICON_WORK* pIM = &pWork->aPokeMark[i];
    IRC_POKETRADE_ItemIconReset(pIM);
  }
}
