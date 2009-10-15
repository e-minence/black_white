//=============================================================================
/**
 * @file	  ircpokemontrade_step.c
 * @bfief	  �|�P���������A�j���[�V����
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/09/27
 */
//=============================================================================

#include <gflib.h>


#include "arc_def.h"
#include "net/network_define.h"
#include "system\wipe.h"

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
#include "spahead.h"


static void _changeDemo_ModelTrade0(IRC_POKEMON_TRADE* pWork);
static void _changeDemo_ModelTrade1(IRC_POKEMON_TRADE* pWork);
static void _changeDemo_ModelTrade3(IRC_POKEMON_TRADE* pWork);
static void _changeDemo_ModelTrade20(IRC_POKEMON_TRADE* pWork);
static void _changeDemo_ModelTrade21(IRC_POKEMON_TRADE* pWork);
static void _changeDemo_ModelTrade22(IRC_POKEMON_TRADE* pWork);
static void _changeDemo_ModelTrade24(IRC_POKEMON_TRADE* pWork);
static void _setFadeMask(_D2_PAL_FADE_WORK* pD2Fade);
static void	_FIELD_StartPaletteFade( _EFFTOOL_PAL_FADE_WORK* epfw, u8 start_evy, u8 end_evy, u8 wait, u16 rgb );
static _EFFTOOL_PAL_FADE_WORK* _createPaletteFade(GFL_G3D_RES* g3DRES,HEAPID heapID);
static void	_freePaletteFade( _EFFTOOL_PAL_FADE_WORK* pwk );
static void  _EFFTOOL_CalcPaletteFade( _EFFTOOL_PAL_FADE_WORK *epfw );
static _POKEMCSS_MOVE_WORK* _pokeMoveCreate(MCSS_WORK* pokeMcss, int time, fx32 xend, fx32 zend, HEAPID heapID);
static void _pokeMoveFunc(_POKEMCSS_MOVE_WORK* pMove);





//------------------------------------------------------------------
/**
 * @brief   ���̃V�[���ɕK�v�Ȓl���Z�b�g
 * @param   IRC_POKEMON_TRADE ���[�N
 * @param   _TRADE_SCENE_NO_E no �V�[���Ǘ�enum
 * @retval  none
 */
//------------------------------------------------------------------

static void _setNextAnim(IRC_POKEMON_TRADE* pWork, int timer)
{
  pWork->anmCount = timer;
}


static void _WIPE_SYS_StartRap(int pattern, int wipe_m, int wipe_s, u16 color, int division, int piece_sync, int heap)
{
  WIPE_SYS_Start(pattern, wipe_m, wipe_s, color, division, piece_sync, heap);
}

//MCSS�I���ׂ̈̃R�[���o�b�N
static void _McssAnmStop( u32 data, fx32 currentFrame )
{
  IRC_POKEMON_TRADE* pWork = (IRC_POKEMON_TRADE*)data;

  MCSS_SetAnmStopFlag( pWork->pokeMcss[0]);
  pWork->mcssStop[0] = TRUE;
}


static void _ballinEmitFunc(GFL_EMIT_PTR pEmiter)
{
  IRC_POKEMON_TRADE* pWork = (IRC_POKEMON_TRADE*)GFL_PTC_GetTempPtr();
  
  pWork->pBallInPer = pEmiter;
  OS_TPrintf("PAR IN %x\n",pWork->pBallInPer);
}

static void _balloutEmitFunc(GFL_EMIT_PTR pEmiter)
{
  IRC_POKEMON_TRADE* pWork = (IRC_POKEMON_TRADE*)GFL_PTC_GetTempPtr();
  pWork->pBallOutPer = GFL_PTC_CreateEmitterCallback(pWork->ptc, 6, NULL, pWork);
  OS_TPrintf("PAR OUT %x\n",pWork->pBallOutPer);
}


void IRC_POKMEONTRADE_STEP_ChangeDemo_PokeMove(IRC_POKEMON_TRADE* pWork)
{
  VecFx32 apos;
  int i;

  _CHANGE_STATE(pWork,_changeDemo_ModelTrade0);

}





void _changeDemo_ModelTrade0(IRC_POKEMON_TRADE* pWork)
{
  VecFx32 apos;
  int i;

    //�ҋ@�A�j�����~�܂�̂�҂�
  MCSS_SetAnimCtrlCallBack(pWork->pokeMcss[0], (u32)pWork, _McssAnmStop, NNS_G2D_ANMCALLBACKTYPE_LAST_FRM);

  pWork->mcssStop[0] = FALSE;
  _setNextAnim(pWork, 0);
  
  pWork->pD2Fade = GFL_HEAP_AllocClearMemory(pWork->heapID, sizeof(_D2_PAL_FADE_WORK));
  pWork->pD2Fade->pal_fade_time = _POKEMON_CENTER_TIME;
  pWork->pD2Fade->pal_fade_nowcount = 0;
  pWork->pD2Fade->pal_start = 0;
  pWork->pD2Fade->pal_end = -16;

  pWork->pModelFade = _createPaletteFade(GFL_G3D_UTIL_GetResHandle(pWork->g3dUtil,0), pWork->heapID);
  _FIELD_StartPaletteFade( pWork->pModelFade, 0, 16, _POKEMON_CENTER_TIME/16, 0 );


  pWork->pMoveMcss[0] =
    _pokeMoveCreate(pWork->pokeMcss[0], _POKEMON_CENTER_TIME,
                    _POKE_PRJORTH_RIGHT/2, FX32_ONE*4.5f, pWork->heapID);

  MCSS_SetAnimCtrlCallBack(pWork->pokeMcss[0], (u32)pWork, _McssAnmStop, NNS_G2D_ANMCALLBACKTYPE_LAST_FRM);

  MCSS_SetPaletteFade( pWork->pokeMcss[1], 0, 16, _POKEMON_CENTER_TIME/16, 0 );

  GFL_BG_SetBackGroundColor(GFL_BG_FRAME1_M ,0);
  GFL_BG_SetBackGroundColor(GFL_BG_FRAME1_S ,0);
  
  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade1);

  pWork->pBallInPer = NULL;
  pWork->pBallOutPer = NULL;
  

}

static void _changeDemo_ModelTrade1(IRC_POKEMON_TRADE* pWork)
{
  int i;

  {  // �t�F�[�h��
    _setFadeMask(pWork->pD2Fade);
    _EFFTOOL_CalcPaletteFade(pWork->pModelFade);
    _pokeMoveFunc(pWork->pMoveMcss[0]);
  }
  if(pWork->mcssStop[0]!=TRUE){  //�A�j���I���҂�
    return;
  }
 
  if(pWork->anmCount > _POKEMON_CENTER_TIME){  //�t�F�[�h����

    GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0 );
    GFL_DISP_GXS_SetVisibleControlDirect( 0 );
    IRC_POKETRADEDEMO_RemoveModel( pWork);
    IRC_POKETRADE_AllDeletePokeIconResource(pWork);
    for(i = 0;i< CELL_DISP_NUM;i++){
      if(pWork->curIcon[i]){
        GFL_CLACT_WK_Remove(pWork->curIcon[i]);
      }
    }
    for(i=1;i<2;i++){
      if( pWork->pokeMcss[i] ){
        IRCPOKETRADE_PokeDeleteMcss(pWork,i);
      }
    }

    IRC_POKETRADE_GraphicFreeVram(pWork);
    IRC_POKETRADE_ResetSubDispGraphic(pWork);
    IRCPOKEMONTRADE_ResetPokemonStatusMessage(pWork);

    IRC_POKETRADEDEMO_SetModel( pWork, TRADE01_OBJECT);

    IRC_POKETRADE_SetSubdispGraphicDemo(pWork);

    GFL_HEAP_FreeMemory(pWork->pD2Fade);
    pWork->pD2Fade=NULL;
    _freePaletteFade(pWork->pModelFade);
    pWork->pModelFade = NULL;
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    
    _setNextAnim(pWork, 0);

//    GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_OBJ );

    _CHANGE_STATE(pWork,_changeDemo_ModelTrade3);
  }

}






static void _changeDemo_ModelTrade3(IRC_POKEMON_TRADE* pWork)
{
  _pokeMoveFunc(pWork->pMoveMcss[0]);
  _pokeMoveFunc(pWork->pMoveMcss[1]);

  if(pWork->anmCount == 2){
    GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_OBJ );
    GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  }

  if(pWork->anmCount == _POKEMON_DELETE_TIME){
    if( pWork->pokeMcss[0] ){
      IRCPOKETRADE_PokeDeleteMcss(pWork,0);
    }
  }


  
  if(pWork->anmCount == _BALL_PARTICLE_START){
    GFL_PTC_CreateEmitterCallback(pWork->ptc, DERMO_TEX001, NULL, pWork);
  }
  if(pWork->anmCount == _PARTICLE_DEMO2_START){
    GFL_PTC_CreateEmitterCallback(pWork->ptc, DEMO_TEX002, NULL, pWork);
  }
  if(pWork->anmCount == _PARTICLE_DEMO3_START){
    GFL_PTC_CreateEmitterCallback(pWork->ptc, DEMO_TEX003, NULL, pWork);
  }
  if(pWork->anmCount == _PARTICLE_DEMO3_START2){
    GFL_PTC_CreateEmitterCallback(pWork->ptc, DEMO_TEX003, NULL, pWork);
  }
  if(pWork->anmCount == _PARTICLE_DEMO4_START){
    GFL_PTC_CreateEmitterCallback(pWork->ptc, DEMO_TEX004, NULL, pWork);
  }
  if(pWork->anmCount == _PARTICLE_DEMO4_START2){
    GFL_PTC_CreateEmitterCallback(pWork->ptc, DEMO_TEX004, NULL, pWork);
  }
  if(pWork->anmCount == _PARTICLE_DEMO5_START){
    GFL_PTC_CreateEmitterCallback(pWork->ptc, DEMO_TEX005, _ballinEmitFunc, pWork);
  }
  if(pWork->anmCount == _PARTICLE_DEMO6_START){
    GFL_PTC_CreateEmitterCallback(pWork->ptc, DEMO_TEX006, NULL, pWork);
  }
  if(pWork->anmCount == _PARTICLE_DEMO7_START){
    GFL_PTC_CreateEmitterCallback(pWork->ptc, DEMO_TEX007, NULL, pWork);
  }
  
  if(pWork->anmCount == _POKEUP_WHITEOUT_START){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE,
                   _POKEUP_WHITEOUT_TIMER, 1, pWork->heapID );

  }
  if(pWork->anmCount == _POKEUP_WHITEIN_START){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_WHITE,
                   _POKEUP_WHITEIN_TIMER, 1, pWork->heapID );
  }

  if(pWork->anmCount == _POKE_APPEAR_START){
    IRCPOKETRADE_PokeCreateMcss(pWork, 0, 0, IRC_POKEMONTRADE_GetRecvPP(pWork,0) );
    IRCPOKETRADE_PokeCreateMcss(pWork, 1, 1, IRC_POKEMONTRADE_GetRecvPP(pWork,1) );
    {  //�����ʒu�ݒ�
        VecFx32 apos;
        apos.x = PSTATUS_MCSS_POS_X1;
        apos.y = _MCSS_POS_Y(240);
        MCSS_SetPosition( pWork->pokeMcss[0] ,&apos );
        apos.x = PSTATUS_MCSS_POS_X2;
        apos.y = _MCSS_POS_Y(-50);
        MCSS_SetPosition( pWork->pokeMcss[1] ,&apos );
    }
    //�ړ��ݒ�
    pWork->pMoveMcss[0] =
      _pokeMoveCreate(pWork->pokeMcss[0], _POKE_APPEAR_TIME,
                      _MCSS_POS_X(60), _MCSS_POS_Y(140), pWork->heapID);
    pWork->pMoveMcss[1] =
      _pokeMoveCreate(pWork->pokeMcss[1], _POKE_APPEAR_TIME,
                      _MCSS_POS_X(190), _MCSS_POS_Y(120), pWork->heapID);
  }
  

  if(_POKE_SIDEOUT_START == pWork->anmCount){
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[1]);
    pWork->pMoveMcss[1]=NULL;
    pWork->pMoveMcss[0] =
      _pokeMoveCreate(pWork->pokeMcss[0], _POKE_SIDEOUT_TIME,
                      _POKEMON_PLAYER_SIDEOUT_POSX, _POKEMON_PLAYER_SIDEOUT_POSY, pWork->heapID);
    pWork->pMoveMcss[1] =
      _pokeMoveCreate(pWork->pokeMcss[1], _POKE_SIDEOUT_TIME,
                      _POKEMON_FRIEND_SIDEOUT_POSX, _POKEMON_FRIEND_SIDEOUT_POSY, pWork->heapID);
  }

  if(_POKE_SIDEIN_START == pWork->anmCount){
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[1]);
    pWork->pMoveMcss[1]=NULL;

    IRCPOKETRADE_PokeDeleteMcss(pWork, 0);
    IRCPOKETRADE_PokeDeleteMcss(pWork, 1);
    IRCPOKETRADE_PokeCreateMcss(pWork, 0, 1, IRC_POKEMONTRADE_GetRecvPP(pWork,0) );
    IRCPOKETRADE_PokeCreateMcss(pWork, 1, 0, IRC_POKEMONTRADE_GetRecvPP(pWork,1) );

    
    {  //�����ʒu�ݒ�
        VecFx32 apos;
        apos.x = _POKEMON_PLAYER_SIDEOUT_POSX;
        apos.y = _POKEMON_PLAYER_SIDEIN_POSY;
        MCSS_SetPosition( pWork->pokeMcss[0] ,&apos );
        apos.x = _POKEMON_FRIEND_SIDEOUT_POSX;
        apos.y = _POKEMON_FRIEND_SIDEIN_POSY;
        MCSS_SetPosition( pWork->pokeMcss[1] ,&apos );
      
    }

    pWork->pMoveMcss[0] =
      _pokeMoveCreate(pWork->pokeMcss[0], _POKE_SIDEIN_TIME,
                      _POKEMON_PLAYER_SIDEIN_POSX, _POKEMON_PLAYER_SIDEIN_POSY, pWork->heapID);
    pWork->pMoveMcss[1] =
      _pokeMoveCreate(pWork->pokeMcss[1], _POKE_SIDEIN_TIME,
                      _POKEMON_FRIEND_SIDEIN_POSX, _POKEMON_FRIEND_SIDEIN_POSY, pWork->heapID);
  }
  if(_POKE_LEAVE_START == pWork->anmCount){
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[1]);
    pWork->pMoveMcss[1]=NULL;
    pWork->pMoveMcss[0] =
      _pokeMoveCreate(pWork->pokeMcss[0], _POKE_LEAVE_TIME,
                      _POKEMON_PLAYER_LEAVE_POSX, _POKEMON_PLAYER_LEAVE_POSY, pWork->heapID);
    pWork->pMoveMcss[1] =
      _pokeMoveCreate(pWork->pokeMcss[1], _POKE_LEAVE_TIME,
                      _POKEMON_FRIEND_LEAVE_POSX, _POKEMON_FRIEND_LEAVE_POSY, pWork->heapID);
  }
  if(_POKEMON_CREATE_TIME == pWork->anmCount){

//    IRCPOKETRADE_PokeCreateMcss(pWork, 1, 0, IRC_POKEMONTRADE_GetRecvPP(pWork,1) );
    {
      VecFx32 apos;
      apos.x = _MCSS_POS_X(120);
      apos.y = _MCSS_POS_Y(120);
      MCSS_SetPosition( pWork->pokeMcss[1] ,&apos );
      MCSS_SetAnmStopFlag(pWork->pokeMcss[1]);
    }

  }
  if(pWork->anmCount == _POKECHANGE_WHITEOUT_START){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE,
                   _POKECHANGE_WHITEOUT_TIMER, 1, pWork->heapID );
  }
  if(pWork->anmCount == _POKECHANGE_WHITEIN_START){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_WHITE,
                   _POKECHANGE_WHITEIN_TIMER, 1, pWork->heapID );
  }
  if(pWork->anmCount == _POKESPLASH_WHITEOUT_START){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE,
                   _POKESPLASH_WHITEOUT_TIMER, 1, pWork->heapID );
  }
  if(pWork->anmCount == _POKESPLASH_WHITEIN_START){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_WHITE,
                   _POKESPLASH_WHITEIN_TIMER, 1, pWork->heapID );
  }
  OS_TPrintf("C %d\n",pWork->anmCount);
  
  if(pWork->anmCount == (_DEMO_END-1)){
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[1]);
    pWork->pMoveMcss[1]=NULL;
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade20);
  }
}


static void _changeDemo_ModelTrade20(IRC_POKEMON_TRADE* pWork)
{
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_49, pWork->pMessageStrBufEx );

  {
    POKEMON_PARAM* pp = IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
    WORDSET_RegisterPokeNickName( pWork->pWordSet, 1,  pp );
  }
  WORDSET_ExpandStr( pWork->pWordSet, pWork->pMessageStrBuf, pWork->pMessageStrBufEx  );


  IRC_POKETRADE_MessageWindowOpen(pWork,  POKETRADE_STR_49);
  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade21);

}

static void _changeDemo_ModelTrade21(IRC_POKEMON_TRADE* pWork)
{
  if(!IRC_POKETRADE_MessageEndCheck(pWork)){
    return;
  }
  if(pWork->anmCount < 100){
    return;
  }
  GFL_MSG_GetString( pWork->pMsgData, POKETRADE_STR_50, pWork->pMessageStrBuf );
  IRC_POKETRADE_MessageWindowOpen(pWork,  POKETRADE_STR_50);
  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade24);

}


static void _changeDemo_ModelTrade24(IRC_POKEMON_TRADE* pWork)
{
  
  IRC_POKETRADEDEMO_RemoveModel( pWork);

  IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE_NORMAL);
  GFL_BG_FreeBGControl(GFL_BG_FRAME3_S);


  IRC_POKETRADE_InitBoxCursor(pWork);  // �^�X�N�o�[
  IRC_POKETRADE_CreatePokeIconResource(pWork);  // �|�P�����A�C�R��CLACT+���\�[�X�풓��
  
  IRC_POKETRADE_SetSubDispGraphic(pWork);

  IRC_POKETRADE_GraphicInitMainDisp(pWork);
  IRC_POKETRADE_GraphicInitSubDisp(pWork);

  IRC_POKETRADEDEMO_SetModel( pWork, REEL_PANEL_OBJECT);
    
  IRCPOKETRADE_PokeDeleteMcss(pWork, 0);
  IRCPOKETRADE_PokeDeleteMcss(pWork, 1);
    
  GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3 );
  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  _CHANGE_STATE(pWork,IRC_POKMEONTRADE_ChangeFinish);
  
}

static void _setFadeMask(_D2_PAL_FADE_WORK* pD2Fade)
{
  if(!pD2Fade){
    return;
  }
  if(pD2Fade->pal_fade_time < pD2Fade->pal_fade_nowcount){
    return;
  }
  pD2Fade->pal_fade_nowcount++;
  {
    int upper = pD2Fade->pal_end - pD2Fade->pal_start;
    upper *= 4096;
    upper = upper / pD2Fade->pal_fade_time;
    upper = upper * pD2Fade->pal_fade_nowcount;
    upper /= 4096;
    
    G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|
                           GX_BLEND_PLANEMASK_OBJ, upper);
    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|
                            GX_BLEND_PLANEMASK_OBJ, upper);
  }

}

//--------------------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�X�^�[�g
 * @param[in]	bfw	      _EFFTOOL_PAL_FADE_WORK�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	start_evy	�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 * @param[in]	end_evy		�Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���I������16�i�K�j
 * @param[in]	wait			�Z�b�g����p�����[�^�i�E�F�C�g�j
 * @param[in]	rgb				�Z�b�g����p�����[�^�i�t�F�[�h������F�j
 */
//--------------------------------------------------
static void	_FIELD_StartPaletteFade( _EFFTOOL_PAL_FADE_WORK* epfw, u8 start_evy, u8 end_evy, u8 wait, u16 rgb )
{ 

	epfw->pal_fade_flag      = 1;
	epfw->pal_fade_start_evy = start_evy;
	epfw->pal_fade_end_evy   = end_evy;
	epfw->pal_fade_wait      = 0;
	epfw->pal_fade_wait_tmp  = wait;
	epfw->pal_fade_rgb       = rgb;
}


//----------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�\���̍쐬
 * @param[in]	g3DRES	  GFL_G3D_RES
 * @param[in]	heapID	  HEAPID
 * @retturn   �p���b�g�t�F�[�h�\����
 */
//----------------------------------------

static _EFFTOOL_PAL_FADE_WORK* _createPaletteFade(GFL_G3D_RES* g3DRES,HEAPID heapID)
{
  //�p���b�g�t�F�[�h�p���[�N����
  NNSG3dResFileHeader*	header = GFL_G3D_GetResourceFileHeader( g3DRES );
  NNSG3dResTex*		    	pTex = NNS_G3dGetTex( header ); 
  u32                   size = (u32)pTex->plttInfo.sizePltt << 3;
  _EFFTOOL_PAL_FADE_WORK* pwk = GFL_HEAP_AllocMemory(heapID,sizeof(_EFFTOOL_PAL_FADE_WORK));
    
  pwk->g3DRES = g3DRES;
  pwk->pData_dst  = GFL_HEAP_AllocMemory( heapID, size );
  return pwk;
}

//----------------------------------------
/**
 * @brief �p���b�g�t�F�[�h�\���̊J��
 * @param[in]	pwk	_EFFTOOL_PAL_FADE_WORK�|�C���^
 */
//----------------------------------------
static void	_freePaletteFade( _EFFTOOL_PAL_FADE_WORK* pwk )
{
  GFL_HEAP_FreeMemory( pwk->pData_dst );
	GFL_HEAP_FreeMemory( pwk );
}



//-------------------------------------------------
/**
 *	@brief      3D���f���̃p���b�g�t�F�[�h�A�j�����s����   BTLV����]�p  
 *	@param[in]	epfw  �p���b�g�t�F�[�h�Ǘ��\���̂ւ̃|�C���^
 */
//-------------------------------------------------
static void  _EFFTOOL_CalcPaletteFade( _EFFTOOL_PAL_FADE_WORK *epfw )
{ 

	if(	(epfw == NULL) || (epfw->pal_fade_flag == 0) )
  { 
    return;
  }
	if( epfw->pal_fade_wait == 0 )
  { 
  	NNSG3dResFileHeader*	header;
  	NNSG3dResTex*		    	pTex;
    u32                   size;
    const void*           pData_src;
    u32                   from;

	  epfw->pal_fade_wait = epfw->pal_fade_wait_tmp;

    { 
    	//�e�N�X�`�����\�[�X�|�C���^�̎擾
      header = GFL_G3D_GetResourceFileHeader( epfw->g3DRES );
      pTex = NNS_G3dGetTex( header ); 
  
      size = ( u32 )pTex->plttInfo.sizePltt << 3;
      pData_src = NNS_G3dGetPlttData( pTex );
      from = NNS_GfdGetTexKeyAddr( pTex->plttInfo.vramKey );

      SoftFade( pData_src, epfw->pData_dst, ( size / 2 ), epfw->pal_fade_start_evy, epfw->pal_fade_rgb );

      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_PLTT, from, epfw->pData_dst, size );
    }
  	if( epfw->pal_fade_start_evy == epfw->pal_fade_end_evy )
 		{	
 			epfw->pal_fade_flag = 0;
 		}
 		else if( epfw->pal_fade_start_evy > epfw->pal_fade_end_evy )
 		{	
 			epfw->pal_fade_start_evy--;
 		}
 		else
 		{	
 			epfw->pal_fade_start_evy++;
 		}
  }
  else
  { 
 	  epfw->pal_fade_wait--;
  }
}

//-------------------------------------------------
/**
 *	@brief      MCSS�ړ����ߍ쐬
 *	@param[in]	pokeMcss   �Ώ�MCSS
 *	@param[in]	time   �ړ�����
 *	@param[in]	xend    X�̈ړ���
 *	@param[in]	zend    Z�̈ړ���
 *	@param[in]	HEAPID  HEAPID
 */
//-------------------------------------------------

static _POKEMCSS_MOVE_WORK* _pokeMoveCreate(MCSS_WORK* pokeMcss, int time, fx32 xend, fx32 zend, HEAPID heapID)
{
  _POKEMCSS_MOVE_WORK* pPoke = GFL_HEAP_AllocClearMemory(heapID, sizeof(_POKEMCSS_MOVE_WORK));

  pPoke->pMcss = pokeMcss;
 
  {
    VecFx32 apos;
    MCSS_GetPosition(pPoke->pMcss, &apos);

    pPoke->time = time;
    pPoke->xend = xend;
    pPoke->zend = zend;
    pPoke->xstart =apos.x;
    pPoke->zstart =apos.y;
  }
  return pPoke;
  
}
//-------------------------------------------------
/**
 *	@brief      MCSS�ړ����ߎ��s
 *	@param[in]	pokeMcss   �Ώ�MCSS
 *	@param[in]	time   �ړ�����
 *	@param[in]	xend    X�̈ړ���
 *	@param[in]	zend    Z�̈ړ���
 *	@param[in]	HEAPID  HEAPID
 */
//-------------------------------------------------

static void _pokeMoveFunc(_POKEMCSS_MOVE_WORK* pMove)
{
  if(!pMove){
    return;
  }
  if(pMove->time < pMove->nowcount){
    return;
  }
  pMove->nowcount++;
  if(pMove->time != pMove->nowcount)
  {
    VecFx32 apos;
    MCSS_GetPosition(pMove->pMcss, &apos);
    
    apos.x = pMove->xend - pMove->xstart;
    apos.x = apos.x / pMove->time;
    apos.x = pMove->xstart + apos.x * pMove->nowcount;
    apos.y = pMove->zend - pMove->zstart;
    apos.y = apos.y / pMove->time;
    apos.y = pMove->zstart + apos.y * pMove->nowcount;
    MCSS_SetPosition( pMove->pMcss ,&apos );
  }
  else{
    VecFx32 apos;
    MCSS_GetPosition(pMove->pMcss, &apos);
    apos.x = pMove->xend;
    apos.y = pMove->zend;
    MCSS_SetPosition( pMove->pMcss ,&apos );
  }
}


