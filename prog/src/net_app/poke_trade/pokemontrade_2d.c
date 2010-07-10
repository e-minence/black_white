//=============================================================================
/**
 * @file	  pokemontrade_2d.c
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
#include "system/main.h"
#include "system/palanm.h"

#include "net_app/pokemontrade.h"
#include "pokemontrade_local.h"
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
#include "tradeirdemo.naix"
#include "app_menu_common.naix"

#include "poke_tool/pokerus.h"    //for POKERUS_�`    20100610 add Satio



#define _SUCKEDCOUNT_NUM (21)  //�z�����݂܂łɂ������



static void IRC_POKETRADE_TrayInit(POKEMON_TRADE_WORK* pWork,int subchar);
static void IRC_POKETRADE_TrayExit(POKEMON_TRADE_WORK* pWork);
static void _PokeIconCgxLoad(POKEMON_TRADE_WORK* pWork );
static void _POKETRADE_PokeIconPosSet(POKEMON_TRADE_WORK* pWork);

static void SetInfectedPokerusMarkSub(POKEMON_TRADE_WORK* pWork,int side, POKEMON_PARAM* pp);



static void _iconAllDrawDisable(POKEMON_TRADE_WORK* pWork);


#define FIELD_CLSYS_RESOUCE_MAX		(128)

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

//--------------------------------------------------------------
///	�Z���A�N�^�[�@�������f�[�^
//--------------------------------------------------------------
static const GFL_CLSYS_INIT _CLSYS_Init =
{
  0, 0,
  0, 512,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL, //�ʐM�A�C�R������
  0,
  5,
  5,
  5,
  5,
  16, 16,
};


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


void IRC_POKETRADE_DEMOCLACT_Create(POKEMON_TRADE_WORK* pWork)
{
    GFL_CLACT_SYS_Create(	&_CLSYS_Init, &vramBank, pWork->heapID );
}

void IRC_POKETRADE_CLACT_Create(POKEMON_TRADE_WORK* pWork)
{
    GFL_CLACT_SYS_Create(	&fldmapdata_CLSYS_Init, &vramBank, pWork->heapID );
}

//------------------------------------------------------------------
/**
 * $brief   ���������̃O���t�B�b�N������
 * @param   POKEMON_TRADE_WORK   ���[�N�|�C���^
 * @retval  none
 */
//------------------------------------------------------------------

void IRC_POKETRADE_GraphicInitMainDisp(POKEMON_TRADE_WORK* pWork)
{
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
  int ncgr,nscr;

  if(POKEMONTRADEPROC_IsTriSelect(pWork)){
    nscr = NARC_trade_wb_gts_bg01_back_NSCR;
  }
  else{
    nscr = NARC_trade_wb_trade_bg01_back_NSCR;
  }
  

	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_trade_wb_trade_bg_NCLR,
																		PALTYPE_MAIN_BG, 0, 0,  pWork->heapID);

  if(pWork->subchar==0){
    pWork->subchar =
      GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_trade_wb_trade_bg01_NCGR,
                                                   GFL_BG_FRAME2_M, 0, 0, pWork->heapID);
  }

	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 nscr,
																				 GFL_BG_FRAME2_M, 0,
                                         GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                         pWork->heapID);

  GFL_ARC_CloseDataHandle( p_handle );

}

//------------------------------------------------------------------
/**
 * @brief   ���C����ʂ̊J������
 * @param   POKEMON_TRADE_WORK   ���[�N�|�C���^
 * @retval  none
 */
//------------------------------------------------------------------

void IRC_POKETRADE_MainGraphicExit(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->subchar){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_M,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar),
                             GFL_ARCUTIL_TRANSINFO_GetSize( pWork->subchar ));
  }
  pWork->subchar = 0;
}


//------------------------------------------------------------------
/**
 * @brief   �X�e�[�^�X�\�����̉���ʏ�����
 * @param   POKEMON_TRADE_WORK   ���[�N�|�C���^
 * @retval  none
 */
//------------------------------------------------------------------

void IRC_POKETRADE_GraphicInitSubDispStatusDisp(POKEMON_TRADE_WORK* pWork)
{
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );


	// �T�u���BG�L�����]��

	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_trade_bg01_touch_NSCR,
																				 GFL_BG_FRAME0_S, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1), 0, 0,
																				 pWork->heapID);

  
	GFL_ARC_CloseDataHandle( p_handle );

}

//------------------------------------------------------------------
/**
 * @brief   �X�e�[�^�X�\�����̉���ʊJ��
 * @param   POKEMON_TRADE_WORK   ���[�N�|�C���^
 * @retval  none
 */
//------------------------------------------------------------------

void IRC_POKETRADE_GraphicEndSubDispStatusDisp(POKEMON_TRADE_WORK* pWork)
{
//  if(pWork->subcharMain){
  //  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subcharMain),
    //                         GFL_ARCUTIL_TRANSINFO_GetSize( pWork->subcharMain ));
    
//  }

  //pWork->subcharMain = 0;
}



//------------------------------------------------------------------
/**
 * @brief   ���������̉���ʃO���t�B�b�N������
 * @param   POKEMON_TRADE_WORK   ���[�N�|�C���^
 * @retval  none
 */
//------------------------------------------------------------------

void IRC_POKETRADE_GraphicInitSubDisp(POKEMON_TRADE_WORK* pWork)
{
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );

	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_trade_wb_trade_bg_down_NCLR,
																		PALTYPE_SUB_BG, 0, 0x20*_TRADE_BG_PALLETE_NUM,  pWork->heapID);


	// �T�u���BG�L�����]��
	pWork->subchar2 = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_trade_wb_trade_bg02_NCGR,
																																GFL_BG_FRAME3_S, 0, 0, pWork->heapID);
	pWork->subchar1 = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_trade_wb_trade_bg02_NCGR,
																																GFL_BG_FRAME1_S, 0, 0, pWork->heapID);


	GFL_ARC_CloseDataHandle( p_handle );


//  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME2_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
  
  IRC_POKETRADE_TrayInit(pWork,pWork->subchar2);

//  _PokeIconCgxLoad( pWork );
 // IRC_POKETRADE_InitBoxIcon(pWork->pBox, pWork , FALSE );

//	pWork->pAppTaskRes	= APP_TASKMENU_RES_Create( GFL_BG_FRAME2_S, _SUBLIST_NORMAL_PAL,
	//		pWork->pFontHandle, pWork->SysMsgQue, pWork->heapID );

}


void IRC_POKETRADE_GraphicInitSubDispSys(POKEMON_TRADE_WORK* pWork)
{
  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME2_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
}


//------------------------------------------------------------------
/**
 * @brief   ����ʂ̃��\�[�X�̊J��
 * @param   POKEMON_TRADE_WORK   ���[�N�|�C���^
 * @retval  none
 */
//------------------------------------------------------------------

void IRC_POKETRADE_SubGraphicExit(POKEMON_TRADE_WORK* pWork)
{
  IRC_POKETRADE_TrayExit(pWork);
  if(pWork->subchar1){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1),
                             GFL_ARCUTIL_TRANSINFO_GetSize( pWork->subchar1 ));
    pWork->subchar1=0;
  }
  if(pWork->subchar2){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME3_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar2),
                             GFL_ARCUTIL_TRANSINFO_GetSize( pWork->subchar2 ));
    pWork->subchar2=0;
  }
  if(pWork->bgchar){
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
                             GFL_ARCUTIL_TRANSINFO_GetSize( pWork->bgchar ));
    pWork->bgchar=0;
  }
}


//------------------------------------------------------------------
/**
 * $brief   COMMON�Z��������
 * @param   POKEMON_TRADE_WORK   ���[�N�|�C���^
 * @retval  none
 */
//------------------------------------------------------------------

void IRC_POKETRADE_CommonCellInit(POKEMON_TRADE_WORK* pWork)
{

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



void IRC_POKETRADE_SubStatusInit(POKEMON_TRADE_WORK* pWork,int leftright, int bgtype)
{
  int nscr[]={NARC_trade_wb_trade_stbg01_NSCR,NARC_trade_wb_trade_stbg02_NSCR};
  int frame = GFL_BG_FRAME0_S;

	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 nscr[bgtype],
																				 frame, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1), 0, 0,
																				 pWork->heapID);

  if(leftright){
    GFL_BG_SetScroll( frame, GFL_BG_SCROLL_X_SET, 128 );
  }
  else{
    GFL_BG_SetScroll( frame, GFL_BG_SCROLL_X_SET, 0 );
  }
	//G2S_SetBlendAlpha( GFL_BG_FRAME3_S|GX_BLEND_PLANEMASK_OBJ, frame , 16, 3 );
  //G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_OBJ , -8 );
  GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], FALSE );

	GFL_ARC_CloseDataHandle( p_handle );
  GFL_BG_SetVisible( frame , TRUE );

}


void IRC_POKETRADE_SubStatusEnd(POKEMON_TRADE_WORK* pWork)
{
  int frame = GFL_BG_FRAME0_S;

  GFL_BG_SetVisible( frame , FALSE );

	GFL_BG_SetScroll( frame, GFL_BG_SCROLL_X_SET, 0 );
  POKEMONTRADE_2D_AlphaSet(pWork); //G2S_BlendNone();
  GFL_BG_ClearScreen(frame);  //�����X�e�[�^�X
  GFL_BG_LoadScreenV_Req( frame );

}



//------------------------------------------------------------------------------
/**
 * @brief   �X�N���[�����W����X�N���[���f�[�^�����o��
 * @retval  none
 */
//------------------------------------------------------------------------------

static u16 _GetScr(int x , int y, POKEMON_TRADE_WORK* pWork, BOOL bMark)
{
  int x2=x;

  if(x >= pWork->_SRCMAX){
    x2 = x - pWork->_SRCMAX;
  }
  if(x2 < _TEMOTITRAY_SCR_MAX){
    if(bMark){
      return pWork->scrTemotiMark[ 18+(y * 32) + x2 ];
    }
    else{
      return pWork->scrTemoti[ 18+(y * 32) + x2 ];
    }
  }
  x2 = x2 - _TEMOTITRAY_SCR_MAX;
  x2 = x2 % _BOXTRAY_SCR_MAX;
  
  if(bMark){
    return pWork->scrTrayMark[ 18+(y * 32) + x2 ];
  }
  else{
    return pWork->scrTray[ 18+(y * 32) + x2 ];
  }
}



static BOOL _checkMark(POKEMON_TRADE_WORK* pWork,int x,int y)
{
  int line,pos;
  int sx,sy;
  int ax,ay;

  for(line = 0; line < _LING_LINENO_MAX; line++){
    for(pos = 0; pos < BOX_VERTICAL_NUM; pos++){
      if(pWork->pokeIconMarkFlg[line][pos]){
        ax = pWork->pokeIconMark[line][pos].x-5;
        ay = pWork->pokeIconMark[line][pos].y-8;
        if(ax < 0){
          ax = ax - 8; //BTS6154 �}�C�i�X�͉��̊���Z�Ő������Ȃ��Ȃ�
        }
        sx = ax / 8;
        sy = ay / 8;
        if((sx <= x) && ((sx + 3) > x)){
          if((sy <= y) && ((sy + 3) > y)){
//            OS_TPrintf("%d %d %d %d  %d %d\n",x,y,ax,ay,sx,sy);
            return TRUE;
          }
        }
      }
    }
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �g���C�̕\��
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_TrayDisp(POKEMON_TRADE_WORK* pWork)
{
  u16 scr;
  int bgscr = pWork->BoxScrollNum / 8;  //�}�X�̒P�ʂ͉�ʃX�N���[�����g��
  int frame = GFL_BG_FRAME3_S;
  int x,y;
  int line = POKETRADE_boxScrollNum2Line(pWork);  //�|�P�����̒P�ʂ̓��C�����g��
  
  for(y = 0; y < 24 ; y++){
    for(x = 0; x < (_BOXTRAY_SCR_MAX*2+_TEMOTITRAY_SCR_MAX) ; x++){
      scr = _GetScr( bgscr + x , y ,pWork , _checkMark(pWork,x,y));
      GFL_BG_ScrSetDirect(frame, x, y, scr);
    }
  }

  pWork->bgscroll = pWork->BoxScrollNum % 8;  //�}�X�̒P�ʂ͉�ʃX�N���[�����g��
  pWork->bgscrollRenew = TRUE;

  
  GFL_BG_LoadScreenV_Req(frame);

}

//------------------------------------------------------------------------------
/**
 * @brief   �g���C�̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void IRC_POKETRADE_TrayInit(POKEMON_TRADE_WORK* pWork,int subchar)
{
  // �X�N���[����ǂݍ���ł���
  pWork->scrTray = GFL_ARC_LoadDataAlloc( ARCID_POKETRADE, NARC_trade_wb_trade_bg02_NSCR, pWork->heapID);
  pWork->scrTemoti = GFL_ARC_LoadDataAlloc( ARCID_POKETRADE, NARC_trade_wb_trade_bg03_NSCR, pWork->heapID);
  pWork->scrTrayMark = GFL_ARC_LoadDataAlloc( ARCID_POKETRADE, NARC_trade_wb_trade_search_NSCR, pWork->heapID);
  pWork->scrTemotiMark = GFL_ARC_LoadDataAlloc( ARCID_POKETRADE, NARC_trade_wb_trade_search_t_NSCR, pWork->heapID);


  {
    int i;
    for(i=0;i<(32*24);i++){
      pWork->scrTray[18+i] += GFL_ARCUTIL_TRANSINFO_GetPos(subchar);
      pWork->scrTemoti[18+i] += GFL_ARCUTIL_TRANSINFO_GetPos(subchar);
      pWork->scrTrayMark[18+i] += GFL_ARCUTIL_TRANSINFO_GetPos(subchar);
      pWork->scrTemotiMark[18+i] += GFL_ARCUTIL_TRANSINFO_GetPos(subchar);
    }
  }

  pWork->BoxScrollNum = pWork->_DOTMAX + _STARTDOT_OFFSET;

  IRC_POKETRADE_TrayDisp(pWork);
}


//------------------------------------------------------------------------------
/**
 * @brief   �g���C�̃��\�[�X�̊J��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void IRC_POKETRADE_TrayExit(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->scrTray){
    GFL_HEAP_FreeMemory( pWork->scrTray);
    pWork->scrTray=NULL;
    GFL_HEAP_FreeMemory( pWork->scrTemoti);
    pWork->scrTemoti=NULL;
    GFL_HEAP_FreeMemory( pWork->scrTrayMark);
    pWork->scrTrayMark=NULL;
    GFL_HEAP_FreeMemory( pWork->scrTemotiMark);
    pWork->scrTemotiMark=NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �{�b�N�X�����܂Ƃ߂�VRAM�ɑ���
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_SendVramBoxNameChar(POKEMON_TRADE_WORK* pWork)
{
  int i;
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

  for(i=0;i< pWork->BOX_TRAY_MAX+1;i++){
    if(pWork->BoxNameWin[i]==NULL){
  //    pWork->BoxNameWin[i] = GFL_BMPWIN_Create(GFL_BG_FRAME1_S, (i % 3) * 10 , (i / 3) * 2, 15, 2, _BUTTON_MSG_PAL,GFL_BMP_CHRAREA_GET_F);
      pWork->BoxNameWin[i] = GFL_BMPWIN_Create(GFL_BG_FRAME1_S, 0 , 0, 15, 2, _BUTTON_MSG_PAL,GFL_BMP_CHRAREA_GET_F);
    }
    if(i == pWork->BOX_TRAY_MAX){
      GFL_MSG_GetString(  pWork->pMsgData, POKETRADE_STR_19, pWork->pStrBuf );
    }
    else{
      BOXDAT_GetBoxName(pWork->pBox, i, pWork->pStrBuf);
    }
    GFL_FONTSYS_SetColor( 0xf, 0x2, 0 );
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->BoxNameWin[i]), 0 );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->BoxNameWin[i]), 0, 0, pWork->pStrBuf, pWork->pFontHandle);
    GFL_BMPWIN_TransVramCharacter(pWork->BoxNameWin[i]);

    GFL_BMPWIN_SetPosY( pWork->BoxNameWin[i], 0 );  //�O�ɕ␳
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �{�b�N�X���̃E�C���h�E�������J��
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_ResetBoxNameWindow(POKEMON_TRADE_WORK* pWork)
{
  int i;

  for(i=0;i<BOX_MAX_TRAY+1;i++){
    if(pWork->BoxNameWin[i]){
//      GFL_BMPWIN_ClearScreen(pWork->BoxNameWin[i] );
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

void IRC_POKETRADE_SendScreenBoxNameChar(POKEMON_TRADE_WORK* pWork)
{
  int bgscr,i,disp,x,y;
  int spos,pos = pWork->BoxScrollNum / 8;


  for(y=0;y<2;y++){
    for(x=0;x<64;x++){
      GFL_BG_ScrSetDirect(GFL_BG_FRAME1_S, x, y, 0);
    }
  }


  
  if(pWork->BoxScrollNum < _TEMOTITRAY_MAX){  //�莝����Ԃł���
    bgscr = pWork->BOX_TRAY_MAX;
  }
  else{
    bgscr = (pWork->BoxScrollNum - _TEMOTITRAY_MAX) / _BOXTRAY_MAX;
  }
  for(disp = 0; disp < 3; disp++){ //�O��BOX��\��
    i = bgscr+disp;
    if(i > pWork->BOX_TRAY_MAX){
      i = i - pWork->BOX_TRAY_MAX - 1;
    }
    else if(i < 0){
      i = i + pWork->BOX_TRAY_MAX + 1;
    }
    {
      //�����̊�l
      if(i == pWork->BOX_TRAY_MAX){
        if(pos > (pWork->_SRCMAX - pos)){
          spos = pWork->_SRCMAX - pos;
          pos = pos - pWork->_SRCMAX;
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
      GFL_BMPWIN_SetPosX( pWork->BoxNameWin[i], spos+1 );
      GFL_BMPWIN_MakeScreen(pWork->BoxNameWin[i]);
    }
  }
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R�����\�[�X�̍쐬
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------


void IRC_POKETRADE_CreatePokeIconResource(POKEMON_TRADE_WORK* pWork)
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
                                   _OBJPLT_POKEICON_OFFSET ,
                                   pWork->heapID  );

//    pWork->cellRes[PLT_POKEICON_GRAY] =
  //    GFL_CLGRP_PLTT_RegisterComp( arcHandlePoke ,
    //                               POKEICON_GetPalArcIndex() , CLSYS_DRAW_SUB ,
      //                             _OBJPLT_POKEICON_GRAY_OFFSET , pWork->heapID  );

#if 0
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
#endif
    
    
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
        cellInitData.softpri = _CLACT_SOFTPRI_POKELIST; // + _LING_LINENO_MAX - line;
        cellInitData.bgpri = 3;

        pWork->pokeIconNcgRes[line][i] = GFL_CLGRP_CGR_Register( arcHandlePoke ,NARC_poke_icon_poke_icon_egg_normal_m_NCGR , FALSE , CLSYS_DRAW_SUB , pWork->heapID );

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
//        cellInitData.anmseq = 10;
//        cellInitData.softpri = _CLACT_SOFTPRI_MARK;
//        pWork->searchIcon[line][i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
 //                                                       pWork->cellRes[CHAR_SCROLLBAR],
  //                                                      pWork->cellRes[PAL_SCROLLBAR],
   //                                                     pWork->cellRes[ANM_SCROLLBAR],
    //                                                    &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );


        

        GFL_CLACT_WK_SetAutoAnmFlag( pWork->pokeIcon[line][i] , FALSE );
        GFL_CLACT_WK_SetDrawEnable( pWork->pokeIcon[line][i], FALSE );
        GFL_CLACT_WK_SetDrawEnable( pWork->markIcon[line][i], FALSE );
        GFL_CLACT_WK_SetAutoAnmFlag( pWork->markIcon[line][i] , TRUE );
//        GFL_CLACT_WK_SetDrawEnable( pWork->searchIcon[line][i], FALSE );
//        GFL_CLACT_WK_SetAutoAnmFlag( pWork->searchIcon[line][i] , TRUE );
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

static void _deletePokeIconResource(POKEMON_TRADE_WORK* pWork, int line)
{
  int i;
  
	for( i = 0 ; i < BOX_VERTICAL_NUM ; i++ ){
		if(pWork->pokeIcon[line][i]){
			GFL_CLACT_WK_Remove(pWork->pokeIcon[line][i]);
			pWork->pokeIcon[line][i]=NULL;
		}
		if(pWork->markIcon[line][i]){
			GFL_CLACT_WK_Remove(pWork->markIcon[line][i]);
			pWork->markIcon[line][i]=NULL;
		}
//		if(pWork->searchIcon[line][i]){
//			GFL_CLACT_WK_Remove(pWork->searchIcon[line][i]);
//			pWork->searchIcon[line][i]=NULL;
//		}
		if(pWork->pokeIconNcgRes[line][i]){
			GFL_CLGRP_CGR_Release(pWork->pokeIconNcgRes[line][i]);
			pWork->pokeIconNcgRes[line][i] = NULL;
		}
	}
}

void IRC_POKETRADE_AllDeletePokeIconResource(POKEMON_TRADE_WORK* pWork)
{
  int i;
  
  POKEMONTRADE_RemovePokeSelectSixButton(pWork);
  for(i = 0 ; i < _LING_LINENO_MAX ; i++){
    _deletePokeIconResource(pWork,i);
  }

  if(pWork->cellRes[PLT_POKEICON]!=0){
    GFL_CLGRP_PLTT_Release(pWork->cellRes[PLT_POKEICON] );
    pWork->cellRes[PLT_POKEICON]=0;
  }
//  if(pWork->cellRes[PLT_POKEICON_GRAY]!=0){
  //  GFL_CLGRP_PLTT_Release(pWork->cellRes[PLT_POKEICON_GRAY]);
  //  pWork->cellRes[PLT_POKEICON_GRAY]=0;
//  }
  if(pWork->cellRes[ANM_POKEICON]!=0){
    GFL_CLGRP_CELLANIM_Release(pWork->cellRes[ANM_POKEICON] );
    pWork->cellRes[ANM_POKEICON]=0;
  }
  if(pWork->pCharMem){
    GFL_HEAP_FreeMemory(pWork->pCharMem);
  }
  pWork->pCharMem = NULL;
}

void IRC_POKETRADE_EndIconResource(POKEMON_TRADE_WORK* pWork)
{
  int i;

  POKEMONTRADE_EndMojiSelect(pWork);

  POKETRADE_MESSAGE_ResetPokemonBallIcon(pWork);
  
  for(i = 0;i< CELL_DISP_NUM;i++){
    if(pWork->curIcon[i]){
      GFL_CLACT_WK_Remove(pWork->curIcon[i]);
      pWork->curIcon[i] = NULL;
    }
  }

  
  for(i=0;i<PLT_RESOURCE_MAX;i++){
    if(pWork->cellRes[i]!=0){
      GFL_CLGRP_PLTT_Release(pWork->cellRes[i]);
      pWork->cellRes[i]=0;
    }
  }
  for( ;i<CHAR_RESOURCE_MAX;i++){
    if(pWork->cellRes[i]!=0){
      GFL_CLGRP_CGR_Release(pWork->cellRes[i]);
      pWork->cellRes[i]=0;
    }
  }
  for( ;i<ANM_RESOURCE_MAX;i++){
    if(pWork->cellRes[i]!=0){
      GFL_CLGRP_CELLANIM_Release(pWork->cellRes[i]);
      pWork->cellRes[i]=0;
    }
  }

}


static void _calcPokeIconPos(int line,int index, GFL_CLACTPOS* pos)
{
	static const u8	iconSize = 24;
	static const u8 iconTop = 72;
	static const u8 iconLeft = 24;

	pos->x = line * iconSize + iconLeft;
	pos->y = index * iconSize + iconTop;
}


static POKEMON_PASO_PARAM* _getPokeDataAddress(BOX_MANAGER* boxData , int lineno, int verticalindex , POKEMON_TRADE_WORK* pWork, BOOL* bTemoti)
{
  if(lineno >= HAND_HORIZONTAL_NUM){
    int tray = IRC_TRADE_LINE2TRAY(lineno,pWork);
    int index = IRC_TRADE_LINE2POKEINDEX(lineno, verticalindex);
    *bTemoti = FALSE;
    return IRCPOKEMONTRADE_GetPokeDataAddress(boxData, tray, index, pWork);
  }
	else{
    if(verticalindex <3){
      int index = lineno + verticalindex * 2;
      *bTemoti = TRUE;
      return IRCPOKEMONTRADE_GetPokeDataAddress(boxData, pWork->BOX_TRAY_MAX, index, pWork);
		}
	}
	return NULL;
}

static POKE_EASY_INFO* _getPokeInfo(int lineno, int verticalindex , POKEMON_TRADE_WORK* pWork)
{
  if(lineno >= HAND_HORIZONTAL_NUM){
    int tray = IRC_TRADE_LINE2TRAY(lineno,pWork);
    int index = IRC_TRADE_LINE2POKEINDEX(lineno, verticalindex);
    return IRCPOKEMONTRADE_GetPokeInfo(tray, index, pWork);
  }
	else{
    if(verticalindex <3){
      int index = lineno + verticalindex * 2;
      return IRCPOKEMONTRADE_GetPokeInfo(pWork->BOX_TRAY_MAX, index, pWork);
		}
	}
	return NULL;
}


//�J�[�\���̈ʒu���Z�b�g����
void IRC_POKETRADE_SetCursorXY(POKEMON_TRADE_WORK* pWork)
{
  int i,j;
  GFL_CLACTPOS apos;

  for(j = 0; j < BOX_VERTICAL_NUM;j++){
    for(i = 0; i < _LING_LINENO_MAX;i++){
      if(GFL_CLACT_WK_GetDrawEnable( pWork->markIcon[i][j])){
        GFL_CLACT_WK_GetPos( pWork->markIcon[i][j],&apos,CLSYS_DRAW_SUB);
        pWork->x = apos.x;
        pWork->y = apos.y;
      }
    }
  }
}

#if 1
#include "tradehead.cdat"

static BOOL _IsPokeLanguageMark(POKEMON_TRADE_WORK* pWork,int monsno)
{
  if(pWork->selectMoji){
    int i = pWork->selectMoji - 1;

    if(i==MonsterHeadLangTable[monsno]){
      return TRUE;
    }
  }
  return FALSE;
}


BOOL POKEMONTRADE_IsPokeLanguageMark(int monsno,int moji)
{
  GF_ASSERT(monsno < elementof(MonsterHeadLangTable));
  
  if(moji==MonsterHeadLangTable[monsno]){
    return TRUE;
  }
  return FALSE;
}

#endif


void POKMEONTRADE2D_IconGray(POKEMON_TRADE_WORK* pWork, GFL_CLWK* pCL ,BOOL bGray)
{
  NNSG2dImagePaletteProxy proxy;
  u32 res;
  GXOamMode mode;

  if(bGray){
    mode=GX_OAM_MODE_XLU;
  }
  else{
    mode=GX_OAM_MODE_NORMAL;
  }
  
  GFL_CLACT_WK_SetObjMode(pCL,mode);

}


static void _pokeIconPaletteGray(POKEMON_TRADE_WORK* pWork,int line, int i,POKEMON_PASO_PARAM* ppp,BOOL bTemoti,int k)
{
  BOOL bGray = FALSE;

  if( POKETRADE_NEGO_IsSelect(pWork,line,i) ){
//  if((bTemoti && _hedenWazaCheck(ppp))  || ( POKETRADE_NEGO_IsSelect(pWork,line,i)) ){
    bGray=TRUE;
  }
  POKMEONTRADE2D_IconGray(pWork, pWork->pokeIcon[k][i], bGray);
}


//------------------------------------------------------------------------------
/**
 * @brief   ���C���ɂ��킹���|�P�����\��
 * @param   POKEMON_TRADE_WORK ���[�N
 * @param   boxData  �{�b�N�X�̃��[�N
 * @param   line      �|�P�����\����
 * @param   k         ��ʏ�̗�
 * @retval  none
 */
//------------------------------------------------------------------------------
//
static void _createPokeIconResource(POKEMON_TRADE_WORK* pWork,BOX_MANAGER* boxData ,int line, int k , const BOOL isTransVBlank )
{
  int i,j;
  
  POKEMON_PASO_PARAM* ppp;
  POKE_EASY_INFO* info;
	void *obj_vram = G2S_GetOBJCharPtr();


  if(line == pWork->MainObjCursorLine){
 //   if(pWork->padMode==TRUE){
    if(GFL_UI_CheckTouchOrKey()==GFL_APP_END_KEY){
      GFL_CLACT_WK_SetDrawEnable( pWork->markIcon[k][pWork->MainObjCursorIndex], TRUE );
    }
  }


	for( i = 0 ; i < BOX_VERTICAL_NUM ; i++ )
	{
    {
      int	fileNo,monsno,formno,bEgg,bTemoti,sex;
      ppp = _getPokeDataAddress(boxData, line, i, pWork, &bTemoti);
#if 0      
      if(!ppp){
        pWork->pokeIconLine[k][i] = 0xff;
        continue;
      }
      if(PPP_Get( ppp, ID_PARA_poke_exist, NULL  ) == 0 ){
        pWork->pokeIconLine[k][i] = 0xff;
        continue;
      }
      pWork->pokeIconLine[k][i] = line;
      monsno = PPP_Get(ppp,ID_PARA_monsno_egg,NULL);
      if( monsno == 0 ){	//�|�P���������邩�̃`�F�b�N
        continue;
      }

      formno = PPP_Get(ppp,ID_PARA_form_no,NULL);
      sex = PPP_Get(ppp,ID_PARA_sex,NULL);
#else
      info = _getPokeInfo(line, i, pWork);
      if (!info){
        pWork->pokeIconLine[k][i] = 0xff;
        continue;
      }
      if (info->monsno == 0){
        pWork->pokeIconLine[k][i] = 0xff;
        continue;
      }
      pWork->pokeIconLine[k][i] = line;

      monsno = info->monsno;
      formno = info->formno;
      sex = info->sex;
#endif      
      if( (pWork->pokeIconNo[k][i] == monsno) &&
          (pWork->pokeIconForm[k][i] == formno) && 
          (pWork->pokeIconSex[k][i] == sex) ){
        GFL_CLACT_WK_SetDrawEnable( pWork->pokeIcon[k][i], TRUE );
        _pokeIconPaletteGray(pWork, line, i, ppp,bTemoti,k);
      }      
      else if((pWork->pokeIconNo[k][i] != monsno) ||
              ( (pWork->pokeIconNo[k][i] == monsno)&&(pWork->pokeIconForm[k][i] != formno) ) ||
              ( (pWork->pokeIconNo[k][i] == monsno)&&(pWork->pokeIconSex[k][i] != sex) ) ){
        u8 pltNum;
        GFL_CLACTPOS pos;
        NNSG2dImageProxy aproxy;

        pWork->pokeIconNo[k][i] = monsno;
        pWork->pokeIconForm[k][i] = formno;
        pWork->pokeIconSex[k][i] = sex;
        
//        pltNum = POKEICON_GetPalNumGetByPPP( ppp );
        _calcPokeIconPos(line, i, &pos);

        GFL_CLACT_WK_GetImgProxy( pWork->pokeIcon[k][i], &aproxy );

        {
          GFL_CLACTPOS apos;
          int tray = IRC_TRADE_LINE2TRAY(line, pWork);
          int index = IRC_TRADE_LINE2POKEINDEX(line, i);

          int cgxnum = tray*BOX_MAX_POS+index;
          GFL_CLACT_WK_GetPos(pWork->pokeIcon[k][i], &apos, CLSYS_DRAW_SUB);
          if( isTransVBlank == TRUE &&
              apos.x >= -16 && apos.x <= 256+16)
          {
            //�\�����̓p���b�g�Ƃ̍X�V�^�C�~���O�̊֌W��VBLank�]��
            const BOOL ret = NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_OBJ_CHAR_SUB , aproxy.vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DSUB], &pWork->pCharMem[4*8*4*4*cgxnum] , 4*8*4*4 );
            if( ret == FALSE )
            {
              //�Ԃɍ���Ȃ������̂Œʏ�]��(��{��ʊO�Ȃ̂Ŗ��Ȃ��͂�
              GFL_STD_MemCopy(&pWork->pCharMem[4*8*4*4*cgxnum] , (char*)((u32)obj_vram) + aproxy.vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DSUB], 4*8*4*4);
            }
          }
          else
          {
            //���������͊Ԃɍ���Ȃ�����ʏ�]��
            GFL_STD_MemCopy(&pWork->pCharMem[4*8*4*4*cgxnum] , (char*)((u32)obj_vram) + aproxy.vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DSUB], 4*8*4*4);
          }
          
        }
        _pokeIconPaletteGray(pWork, line, i, ppp,bTemoti,k);
        GFL_CLACT_WK_SetPlttOffs( pWork->pokeIcon[k][i] , POKEICON_GetPalNumGetByPPP( ppp ) , CLWK_PLTTOFFS_MODE_PLTT_TOP );
        GFL_CLACT_WK_SetAutoAnmFlag( pWork->pokeIcon[k][i] , FALSE );
        GFL_CLACT_WK_SetDrawEnable( pWork->pokeIcon[k][i], TRUE );
      }
      if(!PPP_Get(ppp,ID_PARA_tamago_flag,NULL) && _IsPokeLanguageMark(pWork,monsno)){
        pWork->pokeIconMarkFlg[k][i]=TRUE;        //�|�P��������
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   BoxScrollNum�̒l����Line�̊�{�l�𓾂�
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

int POKETRADE_boxScrollNum2Line(POKEMON_TRADE_WORK* pWork)
{
  int line,i;
  
  if(pWork->BoxScrollNum < (_TEMOTITRAY_MAX/2)){
    line = 0;
  }
  else if(pWork->BoxScrollNum < _TEMOTITRAY_MAX){
    line = 1;
  }
  else
  {
    const int boxNo = (pWork->BoxScrollNum - _TEMOTITRAY_MAX) / _BOXTRAY_MAX; //BOX��
    const int boxOfs = (pWork->BoxScrollNum - _TEMOTITRAY_MAX) - (boxNo*_BOXTRAY_MAX);
    const int baseLine = boxNo * 6 + 2;
    
    //Box�̑O��ɂ�8Dot�̗]��������̂ł��̃`�F�b�N
    if( boxOfs < 8 )
    {
      line = baseLine;
    }
    else
    if( boxOfs >= _BOXTRAY_MAX - 8 )
    {
      line = baseLine + 5;
    }
    else
    {
      line = baseLine + (boxOfs-8)/24;
    }
//    OS_TPrintf("LINE %d %d\n", pWork->BoxScrollNum, line);
  }
  return line;
}

//------------------------------------------------------------------------------
/**
 * @brief   OBJ�J�[�\������ʓ��ɂ��邩�ǂ������肷��
 * @param   POKEMON_TRADE_WORK ���[�N
 * @param   ���݂̉�ʂ��瓾����line
 * @retval  TRUE ��ʓ��ɂ���
 */
//------------------------------------------------------------------------------

BOOL POKETRADE_IsMainCursorDispIn(POKEMON_TRADE_WORK* pWork,int* line)
{
  int linest = POKETRADE_boxScrollNum2Line(pWork);
  int lineend = linest + 10;
  int maincur;

//  OS_TPrintf("line st %d %d %d %d\n",linest,lineend,pWork->TRADEBOX_LINEMAX,pWork->MainObjCursorLine);
  
  *line = linest + 2;
  if(*line >= pWork->TRADEBOX_LINEMAX){  //�Ă����ɂȂ�  20100625 mod Saito BTS6599
    *line = linest + 2 - pWork->TRADEBOX_LINEMAX;
  }

  if(lineend >= pWork->TRADEBOX_LINEMAX){ //�Ă����ɂȂ�  20100625 mod Saito BTS6599
    maincur = pWork->MainObjCursorLine;
    if(pWork->MainObjCursorLine < 20){  //���C���J�[�\����������Ă���ꍇ
      maincur = pWork->MainObjCursorLine + 1 + pWork->TRADEBOX_LINEMAX; //�v�Z�ׂ̈ɐL�΂�
    }
    if((linest <= maincur) && (maincur <= lineend)){
      return TRUE;
    }
 //   if(linest <= pWork->MainObjCursorLine && pWork->MainObjCursorLine <= pWork->TRADEBOX_LINEMAX){
  //    return TRUE;
  //  }
  //  else if(0 <= pWork->MainObjCursorLine && pWork->MainObjCursorLine <= lineend){
  //    return TRUE;
   // }
  }
  else{
    if(linest <= pWork->MainObjCursorLine && pWork->MainObjCursorLine <= lineend){
      return TRUE;
    }
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�R���̕`��
 * @param   boxData    �{�b�N�X�f�[�^
 * @param   POKEMON_TRADE_WORK  ���[�N
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_InitBoxIcon( BOX_MANAGER* boxData ,POKEMON_TRADE_WORK* pWork , const BOOL isTransVBlank )
{
  int j,k,i,line = POKETRADE_boxScrollNum2Line(pWork);

  if( pWork->oldLine != line ){
    pWork->oldLine = line;

    for(j=0;j<_LING_LINENO_MAX;j++){
      for(i=0;i<BOX_VERTICAL_NUM;i++){
        GFL_CLACT_WK_SetDrawEnable(pWork->markIcon[j][i],FALSE);
        pWork->pokeIconMarkFlg[j][i]=FALSE;
      }
    }


    pWork->ringLineNo = line;
    _iconAllDrawDisable(pWork);  // �A�C�R���\������U����

    for(i=0;i < _LING_LINENO_MAX;i++){
      k =  POKETRADE_Line2RingLineIconGet(pWork->oldLine+i);
      
      _createPokeIconResource(pWork, boxData,line, k , isTransVBlank );  //�A�C�R���\��
      line++;
      if(line >= pWork->TRADEBOX_LINEMAX){
        line=0;
      }
    }
  }
  _POKETRADE_PokeIconPosSet(pWork);
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�R���̏ꏊ��ݒ肷��
 * @param   POKEMON_TRADE_WORK  ���[�N
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _POKETRADE_PokeIconPosSet(POKEMON_TRADE_WORK* pWork)
{
  int line, i, no, m, subnum=0;
  int x,y;
  
  line = pWork->oldLine;
  for(m = 0; m < _LING_LINENO_MAX; m++,line++){
    no = POKETRADE_Line2RingLineIconGet(pWork->oldLine + m);

    if(line >= pWork->TRADEBOX_LINEMAX){
      line = line - pWork->TRADEBOX_LINEMAX;
      subnum = pWork->_DOTMAX;
    }

    for(i=0;i<BOX_VERTICAL_NUM;i++){
      GFL_CLACTPOS apos;
      GFL_CLACTPOS poke_icon_mark_pos;    //20100614 add Saito
      y = 32+i*24;
      if(line == 0){
        x = 16+8+4;
      }
      else if(line == 1){
        x = 56+4;
      }
      else{
        x = ((line - 2) / 6) * _BOXTRAY_MAX;
        x += ((line - 2) % 6) * 24 + 16 + 4;
        x += _TEMOTITRAY_MAX;
      }
      x = x - pWork->BoxScrollNum + subnum;
      apos.x = x;
      apos.y = y;
      poke_icon_mark_pos = apos;        //20100614 add Saito

      if(POKETRADE_NEGO_IsStatusLookAt(pWork,line,i) &&  (GFL_UI_CheckTouchOrKey()==GFL_APP_END_TOUCH)){
        apos.y -= 8;
        GFL_CLACT_WK_SetSoftPri(pWork->pokeIcon[no][i],_CLACT_SOFTPRI_POKESEL);
      }
      else{
        GFL_CLACT_WK_SetSoftPri(pWork->pokeIcon[no][i],_CLACT_SOFTPRI_POKELIST + _LING_LINENO_MAX - m);
      }
      
      GFL_CLACT_WK_SetPos(pWork->pokeIcon[no][i], &apos, CLSYS_DRAW_SUB);
      if(pWork->pokeIconMarkFlg[no][i]){
#if 0   //20100614 del Saito
        GFL_CLACT_WK_GetPos(pWork->pokeIcon[no][i], &pWork->pokeIconMark[no][i], CLSYS_DRAW_SUB);
#else
        pWork->pokeIconMark[no][i] = poke_icon_mark_pos;
#endif
      }
      apos.y = y + 3;
      GFL_CLACT_WK_SetPos(pWork->markIcon[no][i], &apos, CLSYS_DRAW_SUB);
//      GFL_CLACT_WK_SetPos(pWork->searchIcon[no][i], &apos, CLSYS_DRAW_SUB);
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �|�P�����A�C�R���L�����f�[�^���������ɓW�J
 * @param	POKEMON_TRADE_WORK ���[�N
 */
//--------------------------------------------------------------------------------------------
void  POKEMONTRADE_PokeIconCgxLoad(POKEMON_TRADE_WORK* pWork,int count )
{
  int i,j,id,k=0;
  NNSG2dCharacterData* pCharData;
  void* pMem;
  ARCHANDLE * pokeicon_ah;

  if(count == 0){
//    OS_TPrintf("�m��\n");
    pWork->pCharMem = GFL_HEAP_AllocMemory(pWork->heapID, 4*8*4*4*(BOX_POKESET_MAX+TEMOTI_POKEMAX) );
  }

  pokeicon_ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, pWork->heapID );

  for(i = count;i < BOX_MAX_TRAY+1;i++){
    k = i * BOX_MAX_POS;
    for( j = 0 ; j < BOX_MAX_POS ; j++ ){
      if(BOX_MAX_TRAY == i){
        if(j==TEMOTI_POKEMAX){
          break;
        }
      }
      {
        const POKEMON_PASO_PARAM* ppp =
          IRCPOKEMONTRADE_GetPokeDataAddress(pWork->pBox, i, j,  pWork);
        if(ppp){
          id = POKEICON_GetCgxArcIndex( ppp );
        }
        else{
          id = POKEICON_GetCgxArcIndexByMonsNumber( 0, 0, 0, 0 );
        }
        pMem = GFL_ARCHDL_UTIL_LoadBGCharacter(pokeicon_ah, id, FALSE, &pCharData, pWork->heapID);
        GFL_STD_MemCopy(pCharData->pRawData, &pWork->pCharMem[4*8*4*4*k] , 4*8*4*4);
        k++;
        GFL_HEAP_FreeMemory(pMem);
      }
    }
    break;
  }
  GFL_ARC_CloseDataHandle(pokeicon_ah);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �����O�o�b�t�@�̃��C���ԍ���Ԃ�
 * @param   line  ���C��
 * @return	ret   ���C������������
 */
//--------------------------------------------------------------------------------------------

int POKETRADE_Line2RingLineIconGet(int line)
{
  int ret;

  ret = line % _LING_LINENO_MAX;
  return ret;
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

static void _iconAllDrawDisable(POKEMON_TRADE_WORK* pWork)
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

//--------------------------------------------------------------------------------------------
/**
 * �A�C�R���\����S���O���[�ɂ���
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	ppp		POKEMON_PASO_PARAM
 * @param	chr		NNSG2dCharacterData
 *
 * @return	buf
 */
//--------------------------------------------------------------------------------------------


void POKETRADE2D_IconAllGray(POKEMON_TRADE_WORK* pWork,BOOL bGray)
{
  int line,i;
  
  for(line =0 ;line < _LING_LINENO_MAX; line++)
  {
    for(i = 0;i < BOX_VERTICAL_NUM; i++)
    {
//      GFL_CLACT_WK_SetDrawEnable( pWork->pokeIcon[line][i], FALSE );
      POKMEONTRADE2D_IconGray(pWork, pWork->pokeIcon[line][i], bGray);
    }
  }
}



GFL_CLWK* IRC_POKETRADE_GetCLACT( POKEMON_TRADE_WORK* pWork , int x, int y, int* trayno, int* pokeindex, int* outline, int* outindex, int* outRingLine)
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
            if(line2==0xff){
              return NULL;
            }
            if(-1 != IRC_TRADE_LINE2POKEINDEX(line2, i)){
              *trayno = IRC_TRADE_LINE2TRAY(line2,pWork);
              *pokeindex = IRC_TRADE_LINE2POKEINDEX(line2, i);
            }
            else{
              return NULL;
            }
          }
          if(outline != NULL){
            *outline =line2;
            *outindex = i;
            *outRingLine = line;
          }
          OS_TPrintf("pokeicon %d %d  %d\n",line,i,line2);
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
 * @param	   type   �f���̃^�C�v 0=NORMAL 1=�ԊO��
 * @return	 none
 */
//--------------------------------------------------------------------------------------------

void IRC_POKETRADE_SetSubdispGraphicDemo(POKEMON_TRADE_WORK* pWork,int type)
{
  int frame = GFL_BG_FRAME3_S;

  IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE_DEMO);

  pWork->DemoBGClearFlg=TRUE;

  {
    int frame = GFL_BG_FRAME2_S;
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

  {
    int frame = GFL_BG_FRAME3_S;
    {
      GFL_BG_BGCNT_HEADER TextBgCntDat = {
        0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
        GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01,
        2, 0, 0, FALSE
        };
      GFL_BG_SetBGControl( frame, &TextBgCntDat, GFL_BG_MODE_256X16 );
    }


    if(type==0){
      ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADEDEMO, pWork->heapID );
      
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_tradedemo_under01_NCLR,
                                        PALTYPE_SUB_BG_EX, 0x6000, 0,  pWork->heapID);
      
      
      GFL_ARCHDL_UTIL_TransVramBgCharacter(p_handle, NARC_tradedemo_under01_NCGR,
                                           frame,0,0,0,pWork->heapID);
      GFL_ARCHDL_UTIL_TransVramScreen(p_handle,NARC_tradedemo_under01_NSCR,frame, 0,0,0,pWork->heapID);
      
      GFL_BG_SetScroll(frame,GFL_BG_SCROLL_X_SET, 0);
      GFL_ARC_CloseDataHandle( p_handle );
      
    }
    else{
      ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADEDEMO_IR, pWork->heapID );
      
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_tradeirdemo_demo_backshot21_NCLR,
                                        PALTYPE_SUB_BG_EX, 0x6000, 0,  pWork->heapID);
      
      
      GFL_ARCHDL_UTIL_TransVramBgCharacter(p_handle, NARC_tradeirdemo_demo_backshot2_NCGR,
                                           frame,0,0,0,pWork->heapID);
      GFL_ARCHDL_UTIL_TransVramScreen(p_handle,NARC_tradeirdemo_under02_NSCR,frame, 0,0,0,pWork->heapID);
      
      GFL_BG_SetScroll(frame,GFL_BG_SCROLL_X_SET, 0);
      GFL_ARC_CloseDataHandle( p_handle );
      
    }
  }
}

void IRC_POKETRADE_ResetSubdispGraphicDemo(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->DemoBGClearFlg){
    GFL_BG_FillCharacterRelease(GFL_BG_FRAME2_S,1,0);
    GFL_BG_FreeBGControl(GFL_BG_FRAME2_S);
    GFL_BG_FreeBGControl(GFL_BG_FRAME3_S);
    pWork->DemoBGClearFlg=FALSE;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   BG�̈�ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_SetMainDispGraphic(POKEMON_TRADE_WORK* pWork)
{
  {
    GFL_DISP_SetBank( &vramBank );
    GFL_CLACT_SYS_Create(	&fldmapdata_CLSYS_Init, &vramBank, pWork->heapID );
  }

//  GFL_BG_DebugPrintCtrl(GFL_BG_SUB_DISP,TRUE);�f�o�b�O
  IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE_NORMAL);

  GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);

}

 //------------------------------------------------------------------------------
/**
 * @brief   MainBGVram�̈�ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_SetMainVram(POKEMON_TRADE_WORK* pWork)
{
 
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
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 24, GFL_BG_SCRWRT_PALIN );
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
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 24, GFL_BG_SCRWRT_PALIN );
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
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 24, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }

  GFL_BG_SetBGControl3D( 0 );
  GFL_BG_SetVisible( GFL_BG_FRAME0_M , TRUE );
  GFL_BG_SetPriority( GFL_BG_FRAME0_M , 0 );
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
  G3X_SetShading( GX_SHADING_HIGHLIGHT );
  G3X_AntiAlias( TRUE );
 // G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
  G3X_AlphaBlend( TRUE );		// �A���t�@�u�����hON
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
void IRC_POKETRADE_3DGraphicSetUp( POKEMON_TRADE_WORK* pWork )
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
                  0, GetHeapLowID(pWork->heapID), Graphic_3d_SetUp );

    pWork->pCamera   = GFL_G3D_CAMERA_CreateDefault( &cam_pos, &cam_target, GetHeapLowID(pWork->heapID) );

    GFL_G3D_CAMERA_Switching( pWork->pCamera );
    //�G�b�W�}�[�L���O�J���[�Z�b�g
   // G3X_SetEdgeColorTable( edge_color_table );
    G3X_EdgeMarking( FALSE );

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

void IRC_POKETRADE_SetSubVram(POKEMON_TRADE_WORK* pWork)
{

  {
    int frame = GFL_BG_FRAME0_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x10000, 0x10000,GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
 //   GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 24, GFL_BG_SCRWRT_PALIN );
    //		GFL_BG_LoadScreenReq( frame );
    //        GFL_BG_ClearFrame(frame);
  }
  {
    int frame = GFL_BG_FRAME1_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x10000, 0x10000,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 24, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
    //        GFL_BG_ClearFrame(frame);
  }
  {
    int frame = GFL_BG_FRAME3_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 64, 24, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME2_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 24, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
    pWork->BGClearFlg=TRUE;



}



//--------------------------------------------------------------------------------------------
/**
 * @brief    ����ʔj��
 * @param	   pWork  ���[�N
 * @return	 none
 */
//--------------------------------------------------------------------------------------------

void IRC_POKETRADE_ResetSubDispGraphic(POKEMON_TRADE_WORK* pWork)
{
  

  if(pWork->BGClearFlg==TRUE){
    GFL_BG_FillCharacterRelease(GFL_BG_FRAME0_S,1,0);
    GFL_BG_FillCharacterRelease(GFL_BG_FRAME2_S,1,0);
    GFL_BG_FreeBGControl(GFL_BG_FRAME0_S);
    GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
    GFL_BG_FreeBGControl(GFL_BG_FRAME2_S);
    GFL_BG_FreeBGControl(GFL_BG_FRAME3_S);
    pWork->BGClearFlg=FALSE;
  }
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
 * @brief   MAIN��ʂ�OBJ
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_InitMainObj(POKEMON_TRADE_WORK* pWork)
{
  u8 pltNum,i;
  GFL_CLACTPOS pos;

  //�f��
  if(pWork->cellRes[CHAR_SCROLLBAR_UP]==0){
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );

    pWork->cellRes[CHAR_SCROLLBAR_UP] =
      GFL_CLGRP_CGR_Register( p_handle , NARC_trade_wb_trade_obj01_NCGR ,
                              FALSE , CLSYS_DRAW_MAIN , pWork->heapID );
    pWork->cellRes[PAL_SCROLLBAR_UP] =
      GFL_CLGRP_PLTT_RegisterEx(
        p_handle ,NARC_trade_wb_trade_obj_NCLR , CLSYS_DRAW_MAIN ,
        _OBJPLT_BOX_OFFSET,0, _OBJPLT_BOX, pWork->heapID  );
    pWork->cellRes[ANM_SCROLLBAR_UP] =
      GFL_CLGRP_CELLANIM_Register(
        p_handle , NARC_trade_wb_trade_obj01_NCER, NARC_trade_wb_trade_obj01_NANR , pWork->heapID  );
    
    GFL_ARC_CloseDataHandle( p_handle );

  }

}

//------------------------------------------------------------------------------
/**
 * @brief   MAIN��ʂ�OBJ���\�[�X�J��
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_ReleaseMainObj(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->cellRes[CHAR_SCROLLBAR_UP]!=0){
    GFL_CLGRP_CGR_Release(pWork->cellRes[CHAR_SCROLLBAR_UP] );
    GFL_CLGRP_PLTT_Release(pWork->cellRes[PAL_SCROLLBAR_UP] );
    GFL_CLGRP_CELLANIM_Release(pWork->cellRes[ANM_SCROLLBAR_UP] );
    pWork->cellRes[CHAR_SCROLLBAR_UP] = 0;
    pWork->cellRes[PAL_SCROLLBAR_UP] = 0;
    pWork->cellRes[ANM_SCROLLBAR_UP] = 0;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   BOX�J�[�\���\��
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_InitBoxCursor(POKEMON_TRADE_WORK* pWork)
{
  u8 pltNum,i;
  GFL_CLACTPOS pos;

  //�f��
  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
    pWork->cellRes[CHAR_SCROLLBAR] =
      GFL_CLGRP_CGR_Register( p_handle , NARC_trade_wb_trade_obj02_NCGR ,
                              FALSE , CLSYS_DRAW_SUB , pWork->heapID );
    pWork->cellRes[PAL_SCROLLBAR] =
      GFL_CLGRP_PLTT_RegisterEx(
        p_handle ,NARC_trade_wb_trade_obj_NCLR , CLSYS_DRAW_SUB ,
        _OBJPLT_BOX_OFFSET,0, _OBJPLT_BOX, pWork->heapID  );
    pWork->cellRes[ANM_SCROLLBAR] =
      GFL_CLGRP_CELLANIM_Register(
        p_handle , NARC_trade_wb_trade_obj02_NCER, NARC_trade_wb_trade_obj02_NANR , pWork->heapID  );
    GFL_ARC_CloseDataHandle( p_handle );

  }

  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 128;
    cellInitData.pos_y = CONTROL_PANEL_Y;
    cellInitData.anmseq = 2;
    cellInitData.softpri = _CLACT_SOFTPRI_SCROLL_BAR;
    cellInitData.bgpri = 0;
    pWork->curIcon[CELL_CUR_SCROLLBAR] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                             pWork->cellRes[CHAR_SCROLLBAR],
                                             pWork->cellRes[PAL_SCROLLBAR],
                                             pWork->cellRes[ANM_SCROLLBAR],
                                             &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_SCROLLBAR] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], TRUE );
  }

}

//------------------------------------------------------------------------------
/**
 * @brief   ���ʂ̃X�e�[�^�X�\��
 * @param   POKEMON_TRADE_WORK work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_SetMainStatusBG(POKEMON_TRADE_WORK* pWork)
{

	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_trade_stbg03_NSCR,
																				 GFL_BG_FRAME2_M, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
																				 pWork->heapID);
	GFL_ARC_CloseDataHandle( p_handle );
}

//------------------------------------------------------------------------------
/**
 * @brief   ����ʂ̑I���A�C�R���\��
 * @param   POKEMON_TRADE_WORK work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_SetSubStatusIcon(POKEMON_TRADE_WORK* pWork)
{
  
#if 1
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 128;
    cellInitData.pos_y = _POKEMON_SELECT1_CELLY + 4;
    cellInitData.anmseq = 19;
    cellInitData.softpri = _CLACT_SOFTPRI_SCROLL_BAR;
    cellInitData.bgpri = 1;
    pWork->curIcon[CELL_CUR_POKE_BASEPANEL] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                                pWork->cellRes[CHAR_SCROLLBAR],
                                                                pWork->cellRes[PAL_SCROLLBAR],
                                                                pWork->cellRes[ANM_SCROLLBAR],
                                                                &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_POKE_BASEPANEL] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_POKE_BASEPANEL], TRUE );


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
    if(GFL_UI_CheckTouchOrKey()!=GFL_APP_END_KEY){
      GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_POKE_SELECT], FALSE );
    }
    
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
    GFL_CLACT_WK_SetPlttOffs( pWork->curIcon[CELL_CUR_POKE_PLAYER] , POKEICON_GetPalNumGetByPPP( ppp1 ) , CLWK_PLTTOFFS_MODE_PLTT_TOP );

  
    cellInitData.pos_x = _POKEMON_SELECT2_CELLX;
    pWork->curIcon[CELL_CUR_POKE_FRIEND] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                                pWork->cellRes[CHAR_SELECT_POKEICON2],
                                                                pWork->cellRes[PLT_POKEICON],
                                                                pWork->cellRes[ANM_POKEICON],
                                                                &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_POKE_FRIEND] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_POKE_FRIEND], TRUE );
    GFL_CLACT_WK_SetPlttOffs( pWork->curIcon[CELL_CUR_POKE_FRIEND] , POKEICON_GetPalNumGetByPPP( ppp2 ) , CLWK_PLTTOFFS_MODE_PLTT_TOP );
    GFL_ARC_CloseDataHandle(arcHandlePoke);
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ����ʂ̑I���A�C�R���\��
 * @param   POKEMON_TRADE_WORK work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_PosChangeSubStatusIcon(POKEMON_TRADE_WORK* pWork,int sel,BOOL bReset)
{
  GFL_CLACTPOS pos;

  if(bReset){
    pos.x = _POKEMON_SELECT1_CELLX;
    pos.y = _POKEMON_SELECT1_CELLY;
    GFL_CLACT_WK_SetPos( pWork->curIcon[CELL_CUR_POKE_PLAYER], &pos, CLSYS_DRAW_SUB );
    pos.x = _POKEMON_SELECT2_CELLX;
    pos.y = _POKEMON_SELECT1_CELLY;
    GFL_CLACT_WK_SetPos( pWork->curIcon[CELL_CUR_POKE_FRIEND], &pos, CLSYS_DRAW_SUB );
  }
  else{
    pos.x = _POKEMON_SELECT1_CELLX;
    pos.y =  !(sel) ?  _POKEMON_SELECT1_CELLY - 4 : _POKEMON_SELECT1_CELLY;
    GFL_CLACT_WK_SetPos( pWork->curIcon[CELL_CUR_POKE_PLAYER], &pos, CLSYS_DRAW_SUB );
    pos.x = _POKEMON_SELECT2_CELLX;
    pos.y =  (sel) ?  _POKEMON_SELECT1_CELLY - 4 : _POKEMON_SELECT1_CELLY;
    GFL_CLACT_WK_SetPos( pWork->curIcon[CELL_CUR_POKE_FRIEND], &pos, CLSYS_DRAW_SUB );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ���ʂ̃X�e�[�^�X�J�� & ����ʂ̑I���A�C�R���J��
 * @param   POKEMON_TRADE_WORK work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_ResetMainStatusBG(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->curIcon[CELL_CUR_POKE_BASEPANEL]){
    GFL_CLACT_WK_Remove(pWork->curIcon[CELL_CUR_POKE_BASEPANEL]);
  }
  if(pWork->curIcon[CELL_CUR_POKE_SELECT]){
    GFL_CLACT_WK_Remove(pWork->curIcon[CELL_CUR_POKE_SELECT]);
  }
  if(pWork->curIcon[CELL_CUR_POKE_PLAYER]){
    GFL_CLACT_WK_Remove(pWork->curIcon[CELL_CUR_POKE_PLAYER]);
  }
  if(pWork->curIcon[CELL_CUR_POKE_FRIEND]){
    GFL_CLACT_WK_Remove(pWork->curIcon[CELL_CUR_POKE_FRIEND]);
  }
  pWork->curIcon[CELL_CUR_POKE_BASEPANEL]=NULL;
  pWork->curIcon[CELL_CUR_POKE_SELECT]=NULL;
  pWork->curIcon[CELL_CUR_POKE_PLAYER]=NULL;
  pWork->curIcon[CELL_CUR_POKE_FRIEND]=NULL;
  if(pWork->cellRes[CHAR_SELECT_POKEICON1]){
    GFL_CLGRP_CGR_Release(pWork->cellRes[CHAR_SELECT_POKEICON1] );
  }
  if(pWork->cellRes[CHAR_SELECT_POKEICON2] ){
    GFL_CLGRP_CGR_Release(pWork->cellRes[CHAR_SELECT_POKEICON2] );
  }
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
 * @param   POKEMON_TRADE_WORK work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_InitSubMojiBG(POKEMON_TRADE_WORK* pWork)
{
  int i;
  GFL_BG_SetVisible( GFL_BG_FRAME0_S , FALSE );
  GFL_BG_SetVisible(GFL_BG_FRAME2_S, FALSE);

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
                                           NARC_trade_wb_trade_bg04_NSCR,
                                           GFL_BG_FRAME0_S, 0,
                                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1), 0, 0,
                                           pWork->heapID);
    GFL_BG_LoadScreenV_Req(GFL_BG_FRAME0_S);
    GFL_BG_SetScroll(GFL_BG_FRAME0_S,GFL_BG_SCROLL_X_SET, -4);
    GFL_BG_SetScroll(GFL_BG_FRAME2_S,GFL_BG_SCROLL_X_SET, -4);

    GFL_ARC_CloseDataHandle( p_handle );
  }
//  IRC_POKETRADE_GraphicInitSubDispStatusDisp(pWork);


  GFL_FONTSYS_SetColor( 1, 2, 15 );
  for(i=0;i<elementof(mojitbl);i++){
    pWork->SerchMojiWin[i] =
      GFL_BMPWIN_Create(GFL_BG_FRAME2_S, mojitbl[i].x, mojitbl[i].y, 2, 2, _BUTTON_MSG_PAL,  GFL_BMP_CHRAREA_GET_F );
    GF_ASSERT(pWork->SerchMojiWin[i]);
    GFL_MSG_GetString(  pWork->pMsgData, POKETRADE_STR_53+i, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->SerchMojiWin[i]), 0, 0, pWork->pStrBuf, pWork->pFontHandle);
    GFL_BMPWIN_MakeScreen(pWork->SerchMojiWin[i]);
    GFL_BMPWIN_TransVramCharacter(pWork->SerchMojiWin[i]);
  }
  
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);

  
//  if(pWork->curIcon[CELL_CUR_SCROLLBAR]){
//    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], FALSE );
//  }
//  if(pWork->curIcon[CHAR_LEFTPAGE_MARK]){
//    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CHAR_LEFTPAGE_MARK], FALSE );
//  }
  GFL_BG_SetVisible( GFL_BG_FRAME0_S , TRUE );
  GFL_BG_SetVisible(GFL_BG_FRAME2_S, TRUE);

}


//------------------------------------------------------------------------------
/**
 * @brief   ����ʂɕ������͔\���I��
 * @param   POKEMON_TRADE_WORK work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_EndSubMojiBG(POKEMON_TRADE_WORK* pWork)
{
  int i;
  
  GFL_BG_SetVisible( GFL_BG_FRAME0_S , FALSE );
  GFL_BG_SetScroll(GFL_BG_FRAME0_S,GFL_BG_SCROLL_X_SET, 0);
  GFL_BG_SetScroll(GFL_BG_FRAME2_S,GFL_BG_SCROLL_X_SET, 0);

  if(pWork->curIcon[CELL_CUR_SCROLLBAR]){
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_SCROLLBAR], TRUE );
  }
  if(pWork->curIcon[CHAR_LEFTPAGE_MARK]){
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CHAR_LEFTPAGE_MARK], TRUE );
  }

  for(i=0;i<elementof(mojitbl);i++){
    if(pWork->SerchMojiWin[i]){
      GFL_BMPWIN_ClearScreen(pWork->SerchMojiWin[i]);
      BmpWinFrame_Clear(pWork->SerchMojiWin[i], WINDOW_TRANS_OFF);
      GFL_BMPWIN_Delete(pWork->SerchMojiWin[i]);
      pWork->SerchMojiWin[i]=NULL;
    }
	}
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);
}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����X�e�[�^�X�ɃA�C�R���\��
 * @param   POKEMON_TRADE_WORK work
 * @param   side  ��ʕ\��  �E�Ȃ�P���Ȃ�O
 * @param   POKEMON_PARAM 
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_ItemIconDisp(POKEMON_TRADE_WORK* pWork,int side, POKEMON_PARAM* pp)
{
  // �|�P�A�C�R���p�A�C�e���A�C�R��
  int x,y;
  {
    _ITEMMARK_ICON_WORK* pIM = &pWork->aItemMark;
    UI_EASY_CLWK_RES_PARAM prm;
    int item = PP_Get( pp , ID_PARA_item ,NULL);
    int type = 0;

    if(ITEM_CheckMail(item)){
      type = 1;
    }

    if(pIM->clwk_poke_item){
      if(item == ITEM_DUMMY_ID){
        GFL_CLACT_WK_SetDrawEnable( pIM->clwk_poke_item, FALSE );
      }
      else{
        GFL_CLACT_WK_SetAnmSeq( pIM->clwk_poke_item, type );
        GFL_CLACT_WK_SetDrawEnable( pIM->clwk_poke_item, TRUE );
      }
      return;
    }
    
    if(item == ITEM_DUMMY_ID){
      return;
    }
    
    switch(side){
    case 0:
    case 1:
      x = 2 * 8 + side * 16 * 8;
      y = 17*8-4;
      prm.draw_type = CLSYS_DRAW_SUB;
      prm.pltt_line = PLTID_OBJ_POKEITEM_S;
      break;
    case 2:
      x = 32;
      y = 192-20;
      prm.draw_type = CLSYS_DRAW_MAIN;
      prm.pltt_line = PLTID_OBJ_POKEITEM_M;
      break;
    }

    prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
    prm.arc_id    = APP_COMMON_GetArcId();
    prm.pltt_id   = APP_COMMON_GetPokeItemIconPltArcIdx();
    prm.ncg_id    = APP_COMMON_GetPokeItemIconCharArcIdx();
    prm.cell_id   = APP_COMMON_GetPokeItemIconCellArcIdx( APP_COMMON_MAPPING_128K );
    prm.anm_id    = APP_COMMON_GetPokeItemIconAnimeArcIdx( APP_COMMON_MAPPING_128K );
   // prm.pltt_line = PLTID_OBJ_POKEITEM_S;
    prm.pltt_src_ofs = 0;
    prm.pltt_src_num = 1;
    
    UI_EASY_CLWK_LoadResource( &pIM->clres_poke_item, &prm, pWork->cellUnit, pWork->heapID );

    // �A�j���V�[�P���X�Ŏw��( 0=�ǂ���, 1=���[��, 2=�{�[�� )
    
    pIM->clwk_poke_item =
      UI_EASY_CLWK_CreateCLWK( &pIM->clres_poke_item, pWork->cellUnit,
                               x, y, type, pWork->heapID );
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
 * @brief   �|�P���X�A�C�R���\��
 * @param   POKEMON_TRADE_WORK work
 * @param   side  ��ʕ\��  �E�Ȃ�P���Ȃ�O
 * @param   POKEMON_PARAM 
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_PokerusIconDisp(POKEMON_TRADE_WORK* pWork,int side,int bMain, POKEMON_PARAM* pp)
{
  // �|�P�A�C�R���p�A�C�e���A�C�R��
//  IRC_POKETRADE_ItemIconReset(&pWork->aPokerusMark);
  
  {
    _ITEMMARK_ICON_WORK* pIM = &pWork->aPokerusMark;
    UI_EASY_CLWK_RES_PARAM prm;
    BOOL rus = POKERUS_CheckInfectPP(pp);   //20100610 mod Saito
    int type = 0;

    if(pIM->clwk_poke_item){  //�m�ۍς�
      if(!rus){
        GFL_CLACT_WK_SetDrawEnable( pIM->clwk_poke_item, FALSE );
      }
      else{
        GFL_CLACT_WK_SetDrawEnable( pIM->clwk_poke_item, TRUE );
      }
      return;
    }
    if(!rus){
      //�������Ă邩������Ȃ��̂Œ��ׂ�
      //�������A�T�u��ʂ̂Ƃ��̂݁i���C����IRC_POKETRADE_PokeStatusIconDisp�ŕ\�����Ă���j    20100611 add Saito
      if (!bMain)
      {
        SetInfectedPokerusMarkSub(pWork, side, pp);
      }
      return;
    }
    
    if(bMain){
      prm.draw_type = CLSYS_DRAW_MAIN;
      prm.pltt_line = PLTID_OBJ_POKERUS_M;
    }
    else{
      prm.draw_type = CLSYS_DRAW_SUB;
      prm.pltt_line = PLTID_OBJ_POKERUS_S;
    }
    prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
    prm.arc_id    = APP_COMMON_GetArcId();
    prm.pltt_id   = APP_COMMON_GetStatusIconPltArcIdx();
    prm.ncg_id    = APP_COMMON_GetStatusIconCharArcIdx();
    prm.cell_id   = APP_COMMON_GetStatusIconCellArcIdx( APP_COMMON_MAPPING_128K );
    prm.anm_id    = APP_COMMON_GetStatusIconAnimeArcIdx( APP_COMMON_MAPPING_128K );
    prm.pltt_src_ofs = 0;
    prm.pltt_src_num = 1;
    
    UI_EASY_CLWK_LoadResource( &pIM->clres_poke_item, &prm, pWork->cellUnit, pWork->heapID );

    if(bMain){
      pIM->clwk_poke_item =
        UI_EASY_CLWK_CreateCLWK( &pIM->clres_poke_item, pWork->cellUnit,(30 * 8)+4,13 * 8,type,pWork->heapID );
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
 * @brief   �|�P���X��A�C�R���\���i�T�u��ʁj
 * @param   POKEMON_TRADE_WORK work
 * @param   side  ��ʕ\��  �E�Ȃ�P���Ȃ�O
 * @param   POKEMON_PARAM 
 * @retval  none
 */
//------------------------------------------------------------------------------

static void SetInfectedPokerusMarkSub(POKEMON_TRADE_WORK* pWork,int side, POKEMON_PARAM* pp)
{
  _ITEMMARK_ICON_WORK* pIM = &pWork->aPokerusFaceMark;
  UI_EASY_CLWK_RES_PARAM prm;
  BOOL rus = POKERUS_CheckInfectedPP(pp);
  int type = 13; //��}�[�N

  if (!rus) return;

  if(pIM->clwk_poke_item){  //�m�ۍς�
    if(!rus){
      GFL_CLACT_WK_SetDrawEnable( pIM->clwk_poke_item, FALSE );
    }
    else{
      GFL_CLACT_WK_SetDrawEnable( pIM->clwk_poke_item, TRUE );
    }
    return;
  }

  prm.draw_type = CLSYS_DRAW_SUB;
  prm.pltt_line = PLTID_OBJ_POKERUS_FACE_S;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetPokeMarkPltArcIdx();
  prm.ncg_id    = APP_COMMON_GetPokeMarkCharArcIdx( APP_COMMON_MAPPING_128K );
  prm.cell_id   = APP_COMMON_GetPokeMarkCellArcIdx( APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetPokeMarkAnimeArcIdx( APP_COMMON_MAPPING_128K );
  prm.pltt_src_ofs = 0;
  prm.pltt_src_num = 1;
    
  UI_EASY_CLWK_LoadResource( &pIM->clres_poke_item, &prm, pWork->cellUnit, pWork->heapID );

  pIM->clwk_poke_item =
        UI_EASY_CLWK_CreateCLWK( &pIM->clres_poke_item, pWork->cellUnit,
                                 13 * 8 + side * 16 * 8 + 4, 2*8+4, type, pWork->heapID );
}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����X�e�[�^�X�ɃA�C�R���\��
 * @param   POKEMON_TRADE_WORK work
 * @param   POKEMON_PARAM  �\������|�P����
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_PokeStatusIconDisp(POKEMON_TRADE_WORK* pWork, POKEMON_PARAM* pp, BOOL bEgg)
{
  int i,type;
  UI_EASY_CLWK_RES_PARAM prm;
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
//  IRC_POKETRADE_PokeStatusIconReset(pWork);

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetPokeMarkPltArcIdx();
  prm.ncg_id    = APP_COMMON_GetPokeMarkCharArcIdx(APP_COMMON_MAPPING_128K);
  prm.cell_id   = APP_COMMON_GetPokeMarkCellArcIdx( APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetPokeMarkAnimeArcIdx( APP_COMMON_MAPPING_128K );
  prm.pltt_line = PLTID_OBJ_POKESTATE_M;
  prm.pltt_src_ofs = 0;
  prm.pltt_src_num = 1;

  //���A�}�X�N���쐬          20100610 add Saito
  if (!bEgg)
  {
    BOOL rare = PP_CheckRare( pp );
    if (rare) mark |= (1<<6);
  }
  //�|�P���X�����}�X�N���쐬    20100610 add Saito
  {
    BOOL infected = POKERUS_CheckInfectedPP(pp);
    if (infected) mark |= (1<<7);
  }

  if ( !(pWork->aPokeMark.load) )
  {
    UI_EASY_CLWK_LoadResource( &pWork->aPokeMark.res, &prm, pWork->cellUnit, pWork->heapID );
    pWork->aPokeMark.load = TRUE;
  }

  for(i = 0;i < _POKEMARK_MAX ; i++){
    GFL_CLWK  *clwk = pWork->aPokeMark.clwk[i];

    if(mark & (1 << i)){
      type = marktbl[i*2];
    }
    else{
      type = marktbl[i*2+1];
    }
    if(clwk){
      if(type==-1){
        GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );
      }
      else if(bEgg && i==6){
        GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );
      }
      else{
        GFL_CLACT_WK_SetAnmSeq( clwk, type );
        GFL_CLACT_WK_SetDrawEnable( clwk, TRUE );
      }
    }
    else{
#if 0      // 20100610 del Saito
      UI_EASY_CLWK_LoadResource( &pIM->clres_poke_item, &prm, pWork->cellUnit, pWork->heapID );
      pIM->clwk_poke_item =
        UI_EASY_CLWK_CreateCLWK( &pIM->clres_poke_item, pWork->cellUnit, markpos[i]*8 , (12 * 8)+5, type, pWork->heapID );
#else
      int make_type;
      if (type==-1) make_type = marktbl[i*2];
      else make_type = type;
      pWork->aPokeMark.clwk[i] =
        UI_EASY_CLWK_CreateCLWK( &pWork->aPokeMark.res, pWork->cellUnit, markpos[i]*8 , (12 * 8)+5, make_type, pWork->heapID );
      clwk = pWork->aPokeMark.clwk[i];
#endif
      if(type==-1){
        GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );
        continue;
      }
      if(bEgg && i==6){
        GFL_CLACT_WK_SetDrawEnable( clwk, FALSE );
        continue;
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����X�e�[�^�X�ɃA�C�R���̑S������
 * @param   POKEMON_TRADE_WORK work
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_PokeStatusIconReset(POKEMON_TRADE_WORK* pWork)
{
  int i;
  _STATE_ICON_WORK* pSIW = &pWork->aPokeMark;
  for(i = 0;i < _POKEMARK_MAX ; i++){
    if(pSIW->clwk[i]){
      GFL_CLACT_WK_Remove( pSIW->clwk[i] );
      pSIW->clwk[i] = NULL;
    }
  }
  if (pSIW->load)
  {
    UI_EASY_CLWK_UnLoadResource( &pSIW->res );
    pSIW->load = FALSE;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����X�e�[�^�X�E�C���h�E�p�̃p���b�g����������
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void IRC_POKETRADE_StatusWindowMessagePaletteTrans(POKEMON_TRADE_WORK* pWork, int palno, int palType)
{

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, palType,
                                0x20*palno, 0x20, pWork->heapID);
  GFL_ARC_UTIL_TransVramPaletteEx(APP_COMMON_GetArcId() ,
                                  NARC_app_menu_common_task_menu_NCLR , palType,
                                  0xe*2 ,0x20*palno+0xe*2, 2*2 ,pWork->heapID);
  
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�s�r�p�|�P�����A�C�R��������
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKETRADE_2D_GTSPokemonIconReset(POKEMON_TRADE_WORK* pWork,int side, int no)
{
  if(pWork->pokeIconGTS[side][no]){
    GFL_CLACT_WK_Remove( pWork->pokeIconGTS[side][no]);
    pWork->pokeIconGTS[side][no]=NULL;
    GFL_CLGRP_CGR_Release( pWork->pokeIconNcgResGTS[side][no] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�s�r�p�|�P�����A�C�R���\���ؑ�
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKETRADE_2D_GTSPokemonIconVisible(POKEMON_TRADE_WORK* pWork,int side, BOOL onoff)
{
  int i;
  for(i=0;i<GTS_NEGO_POKESLT_MAX;i++){
    if(pWork->pokeIconGTS[side][i]){
      GFL_CLACT_WK_SetDrawEnable( pWork->pokeIconGTS[side][i], onoff);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   GTS�G���g���[�Ƀ|�P�����A�C�R�����G���g���[����
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

const static GFL_POINT pokemonpos[]= {{32,96},{64,96},{96,96} , {36+128+4,96},{68+128+4,96},{100+128+4,96}};
const static GFL_POINT pokemonposl[]={{24,46},{24,94},{24,142}, {24+128,46},{24+128,94},{24+128,142} };


void POKETRADE_2D_GTSPokemonIconSet(POKEMON_TRADE_WORK* pWork, int side,int no, POKEMON_PARAM* pp, int hilow, BOOL bDisp)
{
  POKEMON_PASO_PARAM* ppp = PP_GetPPPPointer(pp);
  ARCHANDLE *arcHandlePoke = GFL_ARC_OpenDataHandle( ARCID_POKEICON , pWork->heapID );
  GFL_CLWK_DATA cellInitData;
  int drawn;

  POKETRADE_2D_GTSPokemonIconReset(pWork, side, no);
  
  if(hilow){
    cellInitData.pos_x = pokemonpos[no+side*GTS_NEGO_POKESLT_MAX].x;
    cellInitData.pos_y = pokemonpos[no+side*GTS_NEGO_POKESLT_MAX].y;
    drawn = CLSYS_DRAW_MAIN;
  }
  else{
    cellInitData.pos_x = pokemonposl[no+side*GTS_NEGO_POKESLT_MAX].x;
    cellInitData.pos_y = pokemonposl[no+side*GTS_NEGO_POKESLT_MAX].y;
    drawn = CLSYS_DRAW_SUB;
  }
  cellInitData.anmseq = POKEICON_ANM_HPMAX;
  cellInitData.softpri = _CLACT_SOFTPRI_POKELIST;
  cellInitData.bgpri = 1;

  pWork->pokeIconNcgResGTS[side][no] =
    GFL_CLGRP_CGR_Register( arcHandlePoke ,
                            POKEICON_GetCgxArcIndex(ppp) , FALSE , drawn , pWork->heapID );
  pWork->pokeIconGTS[side][no] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                  pWork->pokeIconNcgResGTS[side][no],
                                                  pWork->cellRes[PLT_GTS_POKEICON],
                                                  pWork->cellRes[ANM_GTS_POKEICON],
                                                  &cellInitData ,drawn , pWork->heapID );

  GFL_CLACT_WK_SetPlttOffs( pWork->pokeIconGTS[side][no] , POKEICON_GetPalNumGetByPPP( ppp ) , CLWK_PLTTOFFS_MODE_PLTT_TOP );
  GFL_CLACT_WK_SetAutoAnmFlag( pWork->pokeIconGTS[side][no] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( pWork->pokeIconGTS[side][no], bDisp );
  GFL_ARC_CloseDataHandle(arcHandlePoke);

}


//------------------------------------------------------------------------------
/**
 * @brief   �f�s�r�p�|�P�����A�C�R������̂�����ɏグ��
 * @param   POKEMON_TRADE_WORK
 * @param   side_up ��ɏグ�� side
 * @param   no_up  ��ɏグ�� no
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKETRADE_2D_GTSPokemonIconChangePosUp(POKEMON_TRADE_WORK* pWork,int side_up, int no_up)
{
  int i,side;
  for(side = 0;side < GTS_PLAYER_WORK_NUM;side++){
    for(i=0;i<GTS_NEGO_POKESLT_MAX;i++){
      if(pWork->pokeIconGTS[side][i]){
        GFL_CLACTPOS pos;
        pos.x = pokemonposl[i+side*GTS_NEGO_POKESLT_MAX].x;
        pos.y = pokemonposl[i+side*GTS_NEGO_POKESLT_MAX].y;
        if((side == side_up) && (i==no_up)){
          pos.y-=4;
        }
        GFL_CLACT_WK_SetPos(pWork->pokeIconGTS[side][i], &pos, CLSYS_DRAW_SUB);
      }
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���̑S������
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKETRADE_2D_GTSPokemonIconResetAll(POKEMON_TRADE_WORK* pWork)
{
  int i,side;

  for(side=0;side<GTS_PLAYER_WORK_NUM;side++){
    for(i=0;i<GTS_NEGO_POKESLT_MAX;i++){
      POKETRADE_2D_GTSPokemonIconReset(pWork,side,i);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �ۂ�����̂��ݏ����J�n
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_StartCatched(POKEMON_TRADE_WORK* pWork,int Ringline, int pos,int x,int y,POKEMON_PASO_PARAM* ppp)
{
  GFL_CLWK_DATA cellInitData;

  cellInitData.pos_x = x;
  cellInitData.pos_y = y;
  cellInitData.anmseq = POKEICON_ANM_HPMAX;
  cellInitData.softpri = 0;
  cellInitData.bgpri = 0;


  pWork->curIcon[CELL_CUR_POKE_KEY] =
    GFL_CLACT_WK_Create( pWork->cellUnit ,
                         pWork->pokeIconNcgRes[Ringline][pos],
                         pWork->cellRes[PLT_POKEICON],
                         pWork->cellRes[ANM_POKEICON],
                         &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );

  GFL_CLACT_WK_SetPlttOffs( pWork->curIcon[CELL_CUR_POKE_KEY] ,
                            POKEICON_GetPalNumGetByPPP( ppp ) , CLWK_PLTTOFFS_MODE_PLTT_TOP );

}

//------------------------------------------------------------------------------
/**
 * @brief   �ۂ�����̋z�����ݏ���
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------
#define DEF_SUCKED_X (28)
#define DEF_SUCKED_Y (0)

void POKEMONTRADE_StartSucked(POKEMON_TRADE_WORK* pWork)
{
  GFL_CLACTPOS pos;

  GFL_CLACT_WK_GetPos(pWork->curIcon[CELL_CUR_POKE_KEY], &pos, CLSYS_DRAW_SUB);


  {
    VecFx32 mullpos[4];
    fx32 x,y,scale,dotproduct;
    VecFx32 angle,normal;
    u16 rad;

    angle.x = (pos.x-pWork->aVecPos.x)*FX32_ONE;  //���������x�N�g��
    angle.y = (pos.y-pWork->aVecPos.y)*FX32_ONE;
    angle.z = 0;

    normal.x = (DEF_SUCKED_X - pos.x)*FX32_ONE;  //�Ō�̕��ւ̃x�N�g��
    normal.y = (DEF_SUCKED_Y - pos.y)*FX32_ONE;
    normal.z = 0;
    
    mullpos[0].x = pos.x*FX32_ONE;
    mullpos[0].y = pos.y*FX32_ONE;
    mullpos[0].z = 0;
    mullpos[1].x = (pos.x-(pWork->aVecPos.x-pos.x)*1)*FX32_ONE;
    mullpos[1].y = (pos.y-(pWork->aVecPos.y-pos.y)*1)*FX32_ONE;
    mullpos[1].z = 0;

    mullpos[3].x = 28*FX32_ONE;
    mullpos[3].y = 0*FX32_ONE;
    mullpos[3].z = 0;

    x = mullpos[0].x-mullpos[3].x;
    y = mullpos[0].y / 2;

    scale = FX_Sqrt(FX_Mul(x,x) + FX_Mul(y,y)) / 15;  //�x�N�g���̑傫��
//    VEC_Normalize(&angle, &normal);
    normal.x = FX_Mul( angle.x, scale);
    normal.y = FX_Mul( angle.y, scale);
    normal.z = FX_Mul( angle.z, scale);

    mullpos[2].x = x + normal.x;
    mullpos[2].y = y + normal.y;
    mullpos[2].z = 0;



    PROGVAL_PEZIER_Init(&pWork->aCutMullRom,
                         &mullpos[0],&mullpos[1],&mullpos[2],&mullpos[3],_SUCKEDCOUNT_NUM-2);

  }
  pWork->SuckedCount = _SUCKEDCOUNT_NUM;
}


//------------------------------------------------------------------------------
/**
 * @brief   �ۂ�����̋z�����ݏ������C��
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

BOOL POKEMONTRADE_SuckedMain(POKEMON_TRADE_WORK* pWork)
{
  double num;
  double ansx,ansy;
    GFL_CLACTPOS pos;
  
  if(pWork->SuckedCount){
    pWork->SuckedCount--;
    if(pWork->SuckedCount==0){
      return TRUE;
    }
    GFL_CLACT_WK_GetPos(pWork->curIcon[CELL_CUR_POKE_KEY], &pos, CLSYS_DRAW_SUB);
    if(pos.y < 0){
      pWork->SuckedCount=1;
    }
    if(pWork->SuckedCount==1){
      POKEMONTRADE_RemovePokemonCursor(pWork);
    }
    else{
      PROGVAL_PEZIER_Main( &pWork->aCutMullRom );
      pos.x = pWork->aCutMullRom.now_val.x/FX32_ONE;
      pos.y = pWork->aCutMullRom.now_val.y/FX32_ONE;
      GFL_CLACT_WK_SetPos(  pWork->curIcon[CELL_CUR_POKE_KEY], &pos, CLSYS_DRAW_SUB);
    }
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����J�[�\��������
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_RemovePokemonCursor(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->curIcon[CELL_CUR_POKE_KEY]){
    GFL_CLACT_WK_Remove(pWork->curIcon[CELL_CUR_POKE_KEY]);
    pWork->curIcon[CELL_CUR_POKE_KEY]=NULL;
  }
}




void POKEMONTRADE_2D_AlphaSet(POKEMON_TRADE_WORK* pWork)
{
  G2S_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE,
                   GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3,
                   8,8);
}



//------------------------------------------------------------------------------
/**
 * @brief   �^�b�`�A�C�R����\��
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_StartFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo)
{
  GFL_CLWK_DATA cellInitData;

  if(!pWork->faceButtonGTS[faceNo]){
    cellInitData.pos_x = GTSFACEICON_POSX+faceNo*GTSFACEICON_HEIGHT;
    cellInitData.pos_y = GTSFACEICON_POSY;
    cellInitData.anmseq = 11+faceNo;
    cellInitData.softpri = _CLACT_SOFTPRI_MARK;
    cellInitData.bgpri = 1;
    
    pWork->faceButtonGTS[faceNo] =
      GFL_CLACT_WK_Create( pWork->cellUnit ,
                           pWork->cellRes[CHAR_SCROLLBAR],
                           pWork->cellRes[PAL_SCROLLBAR],
                           pWork->cellRes[ANM_SCROLLBAR],
                           &cellInitData ,CLSYS_DRAW_SUB, pWork->heapID );
  }
  GFL_CLACT_WK_SetAutoAnmFlag(pWork->faceButtonGTS[faceNo] ,TRUE );
  GFL_CLACT_WK_SetDrawEnable( pWork->faceButtonGTS[faceNo], TRUE );
}

//------------------------------------------------------------------------------
/**
 * @brief   �^�b�`�A�C�R���������Ȃ�����
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_VisibleFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo, BOOL bVisible)
{
  GFL_CLACT_WK_SetDrawEnable( pWork->faceButtonGTS[faceNo], bVisible );
}



//------------------------------------------------------------------------------
/**
 * @brief   �^�b�`�A�C�R��������
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_RemoveFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo)
{
  if(pWork->faceButtonGTS[faceNo]){
    GFL_CLACT_WK_Remove(pWork->faceButtonGTS[faceNo] );
    pWork->faceButtonGTS[faceNo] = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �^�b�`�A�C�R���A�j���^�b�`
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_TouchFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo)
{
  GFL_CLACT_WK_SetAnmSeq(pWork->faceButtonGTS[faceNo] , 15+faceNo);
}

//------------------------------------------------------------------------------
/**
 * @brief   ���ʂɕ��o���{�A�C�R����\��
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_StartEruptedGTS(POKEMON_TRADE_WORK* pWork, int faceNo, int index)
{
  GFL_CLWK_DATA cellInitData;
 
  cellInitData.pos_x = 8 + index*(206);
  cellInitData.pos_y = 192-24-8;
  cellInitData.anmseq = 4;
  cellInitData.softpri = 1;
  cellInitData.bgpri = 0;
  
  

  if(!pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_BASE+index*2]){
    pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_BASE+index*2] =
      GFL_CLACT_WK_Create( pWork->cellUnit ,
                           pWork->cellRes[CHAR_SCROLLBAR_UP],
                           pWork->cellRes[PAL_SCROLLBAR_UP],
                           pWork->cellRes[ANM_SCROLLBAR_UP],
                           &cellInitData ,CLSYS_DRAW_MAIN, pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag(pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_BASE+index*2] ,TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_BASE+index*2], TRUE );
  }
  else{
    GFL_CLACT_WK_SetDrawEnable( pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_BASE+index*2],TRUE);
  }
  
  if(!pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_MARK+index*2]){
    cellInitData.softpri = 0;
    cellInitData.anmseq = faceNo;
    pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_MARK+index*2] =
      GFL_CLACT_WK_Create( pWork->cellUnit ,
                           pWork->cellRes[CHAR_SCROLLBAR_UP],
                           pWork->cellRes[PAL_SCROLLBAR_UP],
                           pWork->cellRes[ANM_SCROLLBAR_UP],
                           &cellInitData ,CLSYS_DRAW_MAIN, pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag(pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_MARK+index*2] ,TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_MARK+index*2], TRUE );
  }
  else{
    GFL_CLACT_WK_SetAnmSeq(pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_MARK+index*2] , faceNo);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ���ʂɕ��o���{�A�C�R�����폜
 * @param   POKEMON_TRADE_WORK
 * @param   palno      �p���b�g�𑗂�ԍ�
 * @param   palType   �p���b�g�]���^�C�v MAIN��SUB
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_RemoveEruptedGTS(POKEMON_TRADE_WORK* pWork,int index)
{
  if(pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_MARK+index*2]!=0){
    GFL_CLACT_WK_Remove(pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_MARK+index*2] );
    pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_MARK+index*2]=0;
  }
  if(pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_BASE+index*2]!=0){
    GFL_CLACT_WK_Remove(pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_BASE+index*2] );
    pWork->eruptedButtonGTS[GTS_ERUPTED_BUTTON_BASE+index*2]=0;
  }
  
}


//------------------------------------------------------------------------------
/**
 * @brief   �U�̑I���A�C�R����\��
 * @param   POKEMON_TRADE_WORK
 * @param   index �\���ꏊ
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_StartPokeSelectSixButton(POKEMON_TRADE_WORK* pWork,int index)
{
  GFL_CLWK_DATA cellInitData;
  GFL_CLACTPOS pos[6]={
    {64, 48},
    {64, 96},
    {64, 144},
    {128+64, 48},
    {128+64, 96},
    {128+64,144},
  };

  if(-1==index){
    if(pWork->pokeSelectSixCur ){
      GFL_CLACT_WK_SetDrawEnable( pWork->pokeSelectSixCur, FALSE );
    }
  }
  else{
    if(!pWork->pokeSelectSixCur ){
      cellInitData.pos_x = 64;
      cellInitData.pos_y = 48;
      cellInitData.anmseq = 23;
      cellInitData.softpri = 0;
      cellInitData.bgpri = 1;
      
      pWork->pokeSelectSixCur =
        GFL_CLACT_WK_Create( pWork->cellUnit ,
                             pWork->cellRes[CHAR_SCROLLBAR],
                             pWork->cellRes[PAL_SCROLLBAR],
                             pWork->cellRes[ANM_SCROLLBAR],
                             &cellInitData ,CLSYS_DRAW_SUB, pWork->heapID );
    }
    GFL_CLACT_WK_SetAutoAnmFlag(pWork->pokeSelectSixCur ,TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->pokeSelectSixCur, TRUE );

    GFL_CLACT_WK_SetPos(  pWork->pokeSelectSixCur, &pos[index], CLSYS_DRAW_SUB);
  }
}





//------------------------------------------------------------------------------
/**
 * @brief   �U�̑I���A�C�R��������
 * @param   POKEMON_TRADE_WORK
 * @param   index �\���ꏊ
 * @retval  none
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_RemovePokeSelectSixButton(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->pokeSelectSixCur){
    GFL_CLACT_WK_Remove(pWork->pokeSelectSixCur );
    pWork->pokeSelectSixCur = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �l�S�V�G�[�V����
 * @param   POKEMON_TRADE_WORK
 * @param   index �\���ꏊ
 * @retval  none
 */
//------------------------------------------------------------------------------

BOOL POKEMONTRADE_NEGOBG_Select6Scroll(POKEMON_TRADE_WORK* pWork)
{
  int i;
  const int scrollnum=4;

  if(pWork->countNEGOSlideScroll <128){
    pWork->countNEGOSlideScroll+=scrollnum;
  }
  else{
    return TRUE;
  }
//  if(GFL_UI_KEY_GetRepeat() == PAD_KEY_UP){
//    pWork->countNEGOSlideScroll++;
//  }
//  else if(GFL_UI_KEY_GetRepeat() == PAD_KEY_DOWN){
//    pWork->countNEGOSlideScroll--;
//  }
//  OS_Printf("Count %d\n",pWork->countNEGOSlideScroll);


  for(i=0;i<GTS_SEL6MSG_NUM;i++){
    GFL_CLACTPOS pos;
    GFL_CLACT_WK_GetPos(pWork->select6Msg[i], &pos,CLSYS_DRAW_SUB);
    if(i/4){
      pos.x += scrollnum;
    }
    else{
      pos.x -= scrollnum;
    }
    GFL_CLACT_WK_SetPos(pWork->select6Msg[i], &pos,CLSYS_DRAW_SUB);
  }


  for(i=0;i<GTS_NEGO_POKESLT_MAX;i++){
    if(pWork->pokeIconGTS[0][i]){
      GFL_CLACTPOS pos;
      GFL_CLACT_WK_GetPos(pWork->pokeIconGTS[0][i], &pos,CLSYS_DRAW_SUB);
      pos.x -= scrollnum;
      GFL_CLACT_WK_SetPos(pWork->pokeIconGTS[0][i], &pos,CLSYS_DRAW_SUB);
    }
  }
  for(i=0;i<GTS_NEGO_POKESLT_MAX;i++){
    if(pWork->pokeIconGTS[1][i]){
      GFL_CLACTPOS pos;
      GFL_CLACT_WK_GetPos(pWork->pokeIconGTS[1][i], &pos,CLSYS_DRAW_SUB);
      pos.x += scrollnum;
      GFL_CLACT_WK_SetPos(pWork->pokeIconGTS[1][i], &pos,CLSYS_DRAW_SUB);
    }
  }
  
  //pWork->pokeIconGTS[side][no]

  
  GFL_BG_SetScroll(GFL_BG_FRAME1_S,GFL_BG_SCROLL_X_SET, -pWork->countNEGOSlideScroll);
  GFL_BG_SetScroll(GFL_BG_FRAME0_S,GFL_BG_SCROLL_X_SET, -128+pWork->countNEGOSlideScroll);
  return FALSE;
}


// �ŏ��ʒu��ύX��  �X�N���[���Ō��ɖ߂�
static void _initreverce(POKEMON_TRADE_WORK* pWork)
{
  int i;
  
  for(i=0;i<GTS_SEL6MSG_NUM;i++){
    GFL_CLACTPOS pos;
    GFL_CLACT_WK_GetPos(pWork->select6Msg[i], &pos,CLSYS_DRAW_SUB);
    if(i/4){
      pos.x -= 128;
      GFL_CLACT_WK_SetBgPri(pWork->select6Msg[i],3);
      
    }
    else{
      pos.x += 128;
    }
    GFL_CLACT_WK_SetPos(pWork->select6Msg[i], &pos,CLSYS_DRAW_SUB);
  }

  for(i=0;i<GTS_NEGO_POKESLT_MAX;i++){
    if(pWork->pokeIconGTS[0][i]){
      GFL_CLACTPOS pos;
      GFL_CLACT_WK_GetPos(pWork->pokeIconGTS[0][i], &pos,CLSYS_DRAW_SUB);
      pos.x += 128;
      GFL_CLACT_WK_SetPos(pWork->pokeIconGTS[0][i], &pos,CLSYS_DRAW_SUB);
    }
  }
  for(i=0;i<GTS_NEGO_POKESLT_MAX;i++){
    if(pWork->pokeIconGTS[1][i]){
      GFL_CLACTPOS pos;
      GFL_CLACT_WK_GetPos(pWork->pokeIconGTS[1][i], &pos,CLSYS_DRAW_SUB);
      pos.x -= 128;
      GFL_CLACT_WK_SetPos(pWork->pokeIconGTS[1][i], &pos,CLSYS_DRAW_SUB);
      GFL_CLACT_WK_SetBgPri(pWork->pokeIconGTS[1][i],3);
    }
  }

  GFL_BG_SetPriority( GFL_BG_FRAME0_S , 2 );
  GFL_BG_SetPriority( GFL_BG_FRAME1_S , 3 );


}



void POKEMONTRADE_NEGOBG_Select6Create(POKEMON_TRADE_WORK* pWork)
{
  ARCHANDLE* p_handle;

  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME2_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
  p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );
	pWork->subchar1 = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_trade_wb_trade_bg02_NCGR,
																																GFL_BG_FRAME1_S, 0, 0, pWork->heapID);
	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_gts_bg05_NSCR,
																				 GFL_BG_FRAME0_S, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1), 0, 0,
																				 pWork->heapID);

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_gts_bg05_NSCR,
																				 GFL_BG_FRAME1_S, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1), 0, 0,
																				 pWork->heapID);

  {//1714�s�ڂƍ��킹��
    int frame = GFL_BG_FRAME3_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, 0x10000,GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };
    GFL_BG_FreeBGControl(frame);
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }
  
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_gts_bg06_NSCR,
																				 GFL_BG_FRAME3_S, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar1), 0, 0,
																				 pWork->heapID);
  GFL_BG_SetScroll(GFL_BG_FRAME0_S,GFL_BG_SCROLL_X_SET, -128);
  GFL_BG_SetScroll(GFL_BG_FRAME3_S,GFL_BG_SCROLL_X_SET, 0);
  
  GFL_BG_SetPriority( GFL_BG_FRAME0_S , 3 );
	GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
																				 NARC_trade_wb_trade_bg01_back_NSCR,
																				 GFL_BG_FRAME2_M, 0,
																				 GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
																				 pWork->heapID);
  GFL_ARC_CloseDataHandle( p_handle );
  pWork->countNEGOSlideScroll=0;
  _initreverce(pWork);

  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );

}





//------------------------------------------------------------------------------
/**
 * @brief   �����v���[�g���쐬 (�|�P�����R�{����)X2
 */
//------------------------------------------------------------------------------

void POKEMONTRADE_CreatePokeSelectMessage(POKEMON_TRADE_WORK* pWork)
{
  int i;
  GFL_CLWK_DATA cellInitData;
  GFL_CLACTPOS pos[GTS_SEL6MSG_NUM]={
    {64+32, 0+32},
    {64+32+8, 48+16},
    {64+32+8, 96+16},
    {64+32+8, 144+16},
    {128+64+32, 0+32},
    {128+64+32+8, 48+16},
    {128+64+32+8, 96+16},
    {128+64+32+8,144+16},
  };
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );

  GF_ASSERT(!pWork->select6CellRes[PAL_SEL6MSG]);
  
  pWork->select6CellRes[PAL_SEL6MSG] =
    GFL_CLGRP_PLTT_RegisterEx(
      p_handle ,NARC_trade_default_NCLR , CLSYS_DRAW_SUB ,  _PALETTE_OAMMSG*0x20, 0, 1, pWork->heapID  );
  pWork->select6CellRes[ANM_SEL6MSG] =
    GFL_CLGRP_CELLANIM_Register(
      p_handle , NARC_trade_plate_NCER, NARC_trade_plate_NANR , pWork->heapID  );

  
  for(i=0;i<GTS_SEL6MSG_NUM;i++){
    pWork->listBmp[i] = GFL_BMP_Create( 16, 6, GFL_BMP_16_COLOR, pWork->heapID );
    pWork->listRes[i] = GFL_CLGRP_CGR_Register(  p_handle , NARC_trade_plate_NCGR , FALSE , CLSYS_DRAW_SUB , pWork->heapID );
  }
  
  for(i=0;i<GTS_SEL6MSG_NUM;i++){
    cellInitData.pos_x = pos[i].x;
    cellInitData.pos_y = pos[i].y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 1;
    
    pWork->select6Msg[i] =
      GFL_CLACT_WK_Create( pWork->cellUnit ,
                           pWork->listRes[i],
                           pWork->select6CellRes[PAL_SEL6MSG],
                           pWork->select6CellRes[ANM_SEL6MSG],
                           &cellInitData ,CLSYS_DRAW_SUB, pWork->heapID );
    GFL_CLACT_WK_SetDrawEnable( pWork->select6Msg[i], TRUE );
  }


  GFL_ARC_CloseDataHandle( p_handle );
}

void POKEMONTRADE_RemovePokeSelectMessage(POKEMON_TRADE_WORK* pWork)
{
//  Resource pWork->select6CellRes[PAL_SEL6MSG]
  int i;

  for(i=0;i<GTS_SEL6MSG_NUM;i++){
    if( pWork->select6Msg[i] ){
      GFL_CLACT_WK_Remove( pWork->select6Msg[i]);
      pWork->select6Msg[i]=NULL;
      GFL_BMP_Delete(pWork->listBmp[i]);
      pWork->listBmp[i]=NULL;
      GFL_CLGRP_CGR_Release(pWork->listRes[i]);
      pWork->listRes[i]=0;
    }
  }
  if(pWork->select6CellRes[PLT_POKEICON]!=0){
    GFL_CLGRP_PLTT_Release(pWork->select6CellRes[PLT_POKEICON] );
    pWork->select6CellRes[PLT_POKEICON]=0;
  }
  if(pWork->select6CellRes[PLT_POKEICON]!=0){
    GFL_CLGRP_CELLANIM_Release(pWork->select6CellRes[ANM_POKEICON] );
    pWork->select6CellRes[PLT_POKEICON]=0;
  }
}


void POKEMONTRADE_MessageOAMWriteVram(POKEMON_TRADE_WORK* pWork)
{
  int i,x;
  u8 buff[]={0,2,4,6,1,3,5,7};
  
#if 1
  // ���X�gOAM�������`��
  for(i = 0; i< GTS_SEL6MSG_NUM ; i++)
  {
    u32 dest_adrs = GFL_CLGRP_CGR_GetAddr( pWork->listRes[i], CLSYS_DRAW_SUB);
    u8* charbuff = GFL_BMP_GetCharacterAdrs(pWork->listBmp[i]);
    u32 size = GFL_BMP_GetBmpDataSize(pWork->listBmp[i]);
    
    DC_FlushRange(charbuff, size);

    for(x = 0; x < 8; x++){
      GXS_LoadOBJ(&charbuff[ 8*32*buff[x] ], dest_adrs, (32*8));
      dest_adrs += 8*32;
    }
  }
#endif
}






void POKEMONTRADE_NEGOBG_SlideInit(POKEMON_TRADE_WORK* pWork,int side,POKEMON_PARAM* pp)
{
  ARCHANDLE* p_handle;
  void* pArc;
  NNSG2dScreenData* scrnData;
  u16*scr;
  int i;

  
  p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADE, pWork->heapID );

  if(side==0){
    pArc = GFL_ARCHDL_UTIL_LoadScreen(p_handle,NARC_trade_wb_gts_bg02_NSCR,FALSE,&scrnData,pWork->heapID);
  }
  else{
    pArc = GFL_ARCHDL_UTIL_LoadScreen(p_handle,NARC_trade_wb_gts_bg03_NSCR,FALSE,&scrnData,pWork->heapID);
  }

    
  scr=(u16 *)&scrnData->rawData;
  for(i=0;i<scrnData->szByte/2;i++){
    scr[i] += GFL_ARCUTIL_TRANSINFO_GetPos( pWork->subchar);
  }
  if(side==0){
    GFL_BG_WriteScreen(GFL_BG_FRAME1_M, scr, 0,0,16,24);
  }
  else{
    GFL_BG_WriteScreen(GFL_BG_FRAME1_M, scrnData->rawData, 16,0,16,24);
  }
  GFL_HEAP_FreeMemory(pArc);

  GFL_BG_SetPriority( GFL_BG_FRAME3_M , 2 );
  GFL_BG_SetPriority( GFL_BG_FRAME2_M , 3 );
  GFL_BG_SetPriority( GFL_BG_FRAME1_M , 1 );
  GFL_BG_SetPriority( GFL_BG_FRAME0_M , 0 );


  GFL_ARC_CloseDataHandle( p_handle );



  
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_M);


//  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );

}




static void _mojiIconCallback(u32 param, fx32 currentFrame )
{
  POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)param;
  {
    pWork->mojiIconEnd = TRUE;
  }
}


void POKEMONTRADE_StartMojiSelect(POKEMON_TRADE_WORK* pWork,int x,int y)
{
  GFL_CLWK_DATA cellInitData;

  cellInitData.pos_x = x+12;
  cellInitData.pos_y = y+12;
  cellInitData.anmseq = 24;
  cellInitData.softpri = 0;
  cellInitData.bgpri = 0;

  POKEMONTRADE_EndMojiSelect(pWork);
  
    
  pWork->mojiIcon =
    GFL_CLACT_WK_Create( pWork->cellUnit ,
                         pWork->cellRes[CHAR_SCROLLBAR],
                         pWork->cellRes[PAL_SCROLLBAR],
                         pWork->cellRes[ANM_SCROLLBAR],
                         &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
  GFL_CLACT_WK_SetDrawEnable( pWork->mojiIcon, TRUE );
  GFL_CLACT_WK_SetAutoAnmFlag(pWork->mojiIcon ,TRUE );
//  GFL_CLACT_WK_StartAnm(pWork->mojiIcon  );
  pWork->mojiIconEnd=FALSE;

  {
    GFL_CLWK_ANM_CALLBACK cbwk;
    cbwk.callback_type = CLWK_ANM_CALLBACK_TYPE_LAST_FRM ;  // CLWK_ANM_CALLBACK_TYPE
    cbwk.param = (u32)pWork;          // �R�[���o�b�N���[�N
    cbwk.p_func = _mojiIconCallback; // �R�[���o�b�N�֐�
    GFL_CLACT_WK_StartAnmCallBack(pWork->mojiIcon , &cbwk);
  }
}

void POKEMONTRADE_EndMojiSelect(POKEMON_TRADE_WORK* pWork)
{
  
  if(pWork->mojiIcon){
    GFL_CLACT_WK_Remove( pWork->mojiIcon);
    pWork->mojiIcon=NULL;
    pWork->mojiIconEnd=FALSE;
  }

}

BOOL POKEMONTRADE_CheckMojiSelect(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->mojiIconEnd){
    POKEMONTRADE_EndMojiSelect(pWork);
    return FALSE;
  }
  return TRUE;
}



static void _PaletteFadeSingle3(POKEMON_TRADE_WORK* pWork, int type, int paletteBIT)
{
  u32 addr;
  int i;
  PALETTE_FADE_PTR pP = PaletteFadeInit(pWork->heapID);
  PaletteFadeWorkAllocSet(pP, type, 16 * 32, pWork->heapID);
  PaletteWorkSet_VramCopy( pP, type, 0, 16*32);
  SoftFadePfd(pP, type,  0, 16 * 16, 6, 0);
  addr = (u32)PaletteWorkTransWorkGet( pP, type );

  for(i = 0; i < 16 ; i++){
    if(paletteBIT & (0x1 << i)){
      DC_FlushRange((void*)(addr+32*i), 32);
      switch(type){
      case FADE_SUB_OBJ:
        GXS_LoadOBJPltt((void*)(addr+32*i), i*16*2, 32);
        break;
      case FADE_SUB_BG:
        GXS_LoadBGPltt((void*)(addr+32*i),  i*16*2, 32);
        break;
      case FADE_MAIN_OBJ:
        GX_LoadOBJPltt((void*)(addr+32*i), i*16*2, 32);
        break;
      case FADE_MAIN_BG:
        GX_LoadBGPltt((void*)(addr+32*i),  i*16*2, 32);
        break;
      }
    }
  }
  PaletteFadeWorkAllocFree(pP,type);
  PaletteFadeFree(pP);
}

void POKEMON_TRADE_PaletteFade(POKEMON_TRADE_WORK* pWork, BOOL bFade, int type, int paletteBIT)
{
  u32 addr;

  if(bFade){
    {
      if( type == FADE_SUB_OBJ){
        GFL_STD_MemCopy((void*)HW_DB_OBJ_PLTT, pWork->pVramOBJ, 16*2*16);
      }
      else if( type == FADE_SUB_BG){
        GFL_STD_MemCopy((void*)HW_DB_BG_PLTT, pWork->pVramBG, 16*2*16);
      }
      _PaletteFadeSingle3( pWork,  type, paletteBIT);
    }
  }
  else{
    if( type == FADE_SUB_OBJ){
      DC_FlushRange((void*)pWork->pVramOBJ, 16*32);
      GXS_LoadOBJPltt((void*)pWork->pVramOBJ, 0, 16 * 32);
    }
    else if( type == FADE_SUB_BG){
      DC_FlushRange((void*)pWork->pVramBG, 16*32);
      GXS_LoadBGPltt((void*)pWork->pVramBG, 0, 16 * 32);
    }
  }
}





void POKEMONTRADE2D_ChangePokemonPalette(POKEMON_TRADE_WORK* pWork, BOOL bGray)
{

  POKEMON_TRADE_PaletteFade(pWork, bGray,FADE_SUB_OBJ,0xfffe);

}

static void POKEMONTRADE2D_SetVisibleMainVBlank_Func( GFL_TCB *tcb , void *work );
static void POKEMONTRADE2D_SetVisibleSubVBlank_Func( GFL_TCB *tcb , void *work );
void POKEMONTRADE2D_SetVisibleMainVBlank( POKEMON_TRADE_WORK* pWork, u32 frame )
{
  GFL_TCBSYS *tcbSys = GFUser_VIntr_GetTCBSYS();
  GFL_TCB_AddTask( tcbSys , POKEMONTRADE2D_SetVisibleMainVBlank_Func , (void*)frame , 10 );
}

void POKEMONTRADE2D_SetVisibleSubVBlank( POKEMON_TRADE_WORK* pWork, u32 frame )
{
  GFL_TCBSYS *tcbSys = GFUser_VIntr_GetTCBSYS();
  GFL_TCB_AddTask( tcbSys , POKEMONTRADE2D_SetVisibleSubVBlank_Func , (void*)frame , 10 );
}

static void POKEMONTRADE2D_SetVisibleMainVBlank_Func( GFL_TCB *tcb , void *work )
{
  u32 frame = (u32)work;
  GFL_DISP_GX_SetVisibleControlDirect( frame );
  GFL_TCB_DeleteTask( tcb );
}

static void POKEMONTRADE2D_SetVisibleSubVBlank_Func( GFL_TCB *tcb , void *work )
{
  u32 frame = (u32)work;
  GFL_DISP_GXS_SetVisibleControlDirect( frame );
  GFL_TCB_DeleteTask( tcb );
}

