//=============================================================================
/**
 * @file	  pokemontrade_demo.c
 * @bfief	  �|�P���������A�j���[�V����
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/09/27
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

#include "pokemontrade_local.h"
#include "app/mailtool.h"

#include "spahead.h"

#include "sound/pm_voice.h"

#include "pokemontrade_demo.cdat"

static void _changeDemo_ModelTrade2_1(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelT1(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelT2(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade0(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade1(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade2(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade3(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade20(POKEMON_TRADE_WORK* pWork);
static void _setFadeMask(_D2_PAL_FADE_WORK* pD2Fade);
static void	_FIELD_StartPaletteFade( _EFFTOOL_PAL_FADE_WORK* epfw, u8 start_evy, u8 end_evy, u8 wait, u16 rgb );
static _EFFTOOL_PAL_FADE_WORK* _createPaletteFade(GFL_G3D_RES* g3DRES,HEAPID heapID);
static void	_freePaletteFade( _EFFTOOL_PAL_FADE_WORK* pwk );
static void  _EFFTOOL_CalcPaletteFade( _EFFTOOL_PAL_FADE_WORK *epfw );
static void _pokeMoveRenew(_POKEMCSS_MOVE_WORK* pPoke,int time, const VecFx32* pPos);
static _POKEMCSS_MOVE_WORK* _pokeMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, HEAPID heapID);
static _POKEMCSS_MOVE_WORK* _pokeTblMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, VecFx32* pTbl, HEAPID heapID);



static void _pokeMoveFunc(_POKEMCSS_MOVE_WORK* pMove);

//���x�����֐�
static int ANMCNTC(int no)
{
  return no;
}


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
 * @brief   �|�P�����o��CALCT�f��INIT
 * @param   POKEMON_TRADE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------
static void _pokemonCreateCLACTInit(POKEMON_TRADE_WORK* pWork)
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKETRADEDEMO, pWork->heapID );

  pWork->cellRes[CHAR_POKECREATE] =
    GFL_CLGRP_CGR_Register( p_handle , NARC_tradedemo_trade_obj01_NCGR ,
                            FALSE , CLSYS_DRAW_MAIN , pWork->heapID );
  pWork->cellRes[PLT_POKECREATE] =
    GFL_CLGRP_PLTT_RegisterEx(
      p_handle ,NARC_tradedemo_trade_obj01_NCLR , CLSYS_DRAW_MAIN ,
      PLTID_OBJ_DEMO_M * 32 , 0, 1, pWork->heapID  );
  pWork->cellRes[ANM_POKECREATE] =
    GFL_CLGRP_CELLANIM_Register(
      p_handle , NARC_tradedemo_trade01_NCER,
      NARC_tradedemo_trade01_NANR , pWork->heapID  );
  GFL_ARC_CloseDataHandle( p_handle );
  pWork->pPokemonTradeDemo->pPokeCreateCLWK=NULL;



}
//------------------------------------------------------------------
/**
 * @brief   �|�P�����o��CALCT�f��Exit
 * @param   POKEMON_TRADE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------

static void _pokemonCreateCLACTExit(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->pPokemonTradeDemo->pPokeCreateCLWK){
    GFL_CLACT_WK_Remove( pWork->pPokemonTradeDemo->pPokeCreateCLWK);
  }
  GFL_CLGRP_CGR_Release(pWork->cellRes[CHAR_POKECREATE] );
  GFL_CLGRP_PLTT_Release(pWork->cellRes[PLT_POKECREATE] );
  GFL_CLGRP_CELLANIM_Release(pWork->cellRes[ANM_POKECREATE] );
  pWork->cellRes[CHAR_POKECREATE] = 0;
  pWork->cellRes[PLT_POKECREATE] = 0;
  pWork->cellRes[ANM_POKECREATE] = 0;
  pWork->pPokemonTradeDemo->pPokeCreateCLWK=NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �|�P�����o��CALCT������
 * @param   POKEMON_TRADE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------
static void _pokemonCreateCLACTAdd(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->pPokemonTradeDemo->pPokeCreateCLWK==NULL){
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 128;
    cellInitData.pos_y = 99;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    pWork->pPokemonTradeDemo->pPokeCreateCLWK = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                  pWork->cellRes[CHAR_POKECREATE],
                                                  pWork->cellRes[PLT_POKECREATE],
                                                  pWork->cellRes[ANM_POKECREATE],
                                                  &cellInitData ,CLSYS_DRAW_MAIN , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->pPokemonTradeDemo->pPokeCreateCLWK , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->pPokemonTradeDemo->pPokeCreateCLWK, TRUE );
  }
  else{
    GFL_CLACT_WK_ResetAnm(pWork->pPokemonTradeDemo->pPokeCreateCLWK);
  }
}


static void _WIPE_SYS_StartRap(int pattern, int wipe_m, int wipe_s, u16 color, int division, int piece_sync, int heap)
{
  WIPE_SYS_Start(pattern, wipe_m, wipe_s, color, division, piece_sync, heap);
}

//MCSS�I���ׂ̈̃R�[���o�b�N
static void _McssAnmStop( u32 data, fx32 currentFrame )
{
  POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)data;

  MCSS_SetAnmStopFlag( pWork->pokeMcss[0]);
  pWork->mcssStop[0] = TRUE;
}


static void _pEmitCBInFunc( GFL_EMIT_PTR pEmiter, unsigned int flag)
{
  if( flag == SPL_EMITTER_CALLBACK_FRONT  ){
    return;
  }
  if(pEmiter ){
    POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)GFL_PTC_GetUserData( pEmiter );
    if( pWork->pPokemonTradeDemo->icaBallin){
      VecFx32 pos={0,0,0};
      ICA_ANIME_GetTranslate( pWork->pPokemonTradeDemo->icaBallin, &pos );
      GFL_PTC_SetEmitterPosition(pEmiter, &pos);
    }
  }
}

static void _ballinEmitFunc(GFL_EMIT_PTR pEmiter)
{
  POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)GFL_PTC_GetTempPtr();

  GFL_PTC_SetUserData( pEmiter, pWork );
  GFL_PTC_SetCallbackFunc(pEmiter, &_pEmitCBInFunc);

}

static void _pEmitCBOutFunc( GFL_EMIT_PTR pEmiter, unsigned int flag)
{
  if( flag == SPL_EMITTER_CALLBACK_FRONT  ){
    return;
  }
  if(pEmiter  ){
    POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)GFL_PTC_GetUserData( pEmiter );
    if(pWork->pPokemonTradeDemo->icaBallout){
      VecFx32 pos={0,0,0};
      ICA_ANIME_GetTranslate( pWork->pPokemonTradeDemo->icaBallout, &pos );
      GFL_PTC_SetEmitterPosition(pEmiter, &pos);
    }
  }
}


static void _balloutEmitFunc(GFL_EMIT_PTR pEmiter)
{
  POKEMON_TRADE_WORK* pWork = (POKEMON_TRADE_WORK*)GFL_PTC_GetTempPtr();

  GFL_PTC_SetUserData( pEmiter, pWork );
  GFL_PTC_SetCallbackFunc(pEmiter, &_pEmitCBOutFunc);
}

//------------------------------------------------------------------
/**
 * @brief   �|�P�������ړ�����O�ɃA�j���I���҂�
 * @param   POKEMON_TRADE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------
void IRC_POKMEONTRADE_STEP_ChangeDemo_PokeMove(POKEMON_TRADE_WORK* pWork)
{
  VecFx32 apos;
  int i;

  PMSND_PushBGM();
  PMSND_PlayBGM(  SEQ_BGM_KOUKAN );
  PMSND_FadeInBGM( 8 );
  
#if 0
//  pWork->mcssStop[0] = FALSE;
//  MCSS_SetAnimCtrlCallBack(pWork->pokeMcss[0], (u32)pWork, _McssAnmStop, NNS_G2D_ANMCALLBACKTYPE_LAST_FRM);
#else
  //������~
  pWork->mcssStop[0] = TRUE;
  MCSS_SetAnimeIndex(pWork->pokeMcss[0], 0);
#endif  
  _CHANGE_STATE(pWork,_changeDemo_ModelT1);
}

//------------------------------------------------------------------
/**
 * @brief   �|�P�����o��CALCT������
 * @param   POKEMON_TRADE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------
static void _changeDemo_ModelT1(POKEMON_TRADE_WORK* pWork)
{
  if(pWork->mcssStop[0]!=TRUE){  //�A�j���I���҂�
    return;
  }

  if(POKEMONTRADEPROC_IsNetworkMode(pWork)){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_ANIMEEND,WB_NET_TRADE_SERVICEID);
    _CHANGE_STATE(pWork,_changeDemo_ModelT2);
  }
  else{
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade0);
  }


}

static void _changeDemo_ModelT2(POKEMON_TRADE_WORK* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_ANIMEEND, WB_NET_TRADE_SERVICEID)){
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade0);
  }

}

static void _changeDemo_ModelTrade0(POKEMON_TRADE_WORK* pWork)
{
  VecFx32 apos;
  int i;

  _setNextAnim(pWork, 0);

  pWork->pD2Fade = GFL_HEAP_AllocClearMemory(pWork->heapID, sizeof(_D2_PAL_FADE_WORK));
  pWork->pD2Fade->pal_fade_time = ANMCNTC(_POKEMON_CENTER_TIME/3);
  pWork->pD2Fade->pal_fade_nowcount = 0;
  pWork->pD2Fade->pal_start = 0;
  pWork->pD2Fade->pal_end = -16;

  if(pWork->modelno!=-1){
//    pWork->pModelFade = _createPaletteFade(GFL_G3D_UTIL_GetResHandle(pWork->g3dUtil,0), pWork->heapID);
//    _FIELD_StartPaletteFade( pWork->pModelFade, 0, 16, ANMCNTC(_POKEMON_CENTER_TIME/3)/16, 0 );
  }

  // �|�P���������Ɉړ��J�n
  {
    VecFx32 pos={_POKEMON_PLAYER_CENTER_POSX,_POKEMON_PLAYER_CENTER_POSY, _POKEMON_PLAYER_CENTER_POSZ};
    pWork->pMoveMcss[0] = _pokeMoveCreate(pWork->pokeMcss[0], ANMCNTC(_POKEMON_CENTER_TIME), &pos, pWork->heapID);
  }
  {
    VecFx32 pos={_POKEMON_FRIEND_CENTER_POSX,_POKEMON_FRIEND_CENTER_POSY, _POKEMON_FRIEND_CENTER_POSZ};
    pWork->pMoveMcss[1] = _pokeMoveCreate(pWork->pokeMcss[1], ANMCNTC(_POKEMON_CENTER_TIME*2), &pos, pWork->heapID);
  }

  MCSS_SetAnimCtrlCallBack(pWork->pokeMcss[0], (u32)pWork, _McssAnmStop, NNS_G2D_ANMCALLBACKTYPE_LAST_FRM);

  MCSS_SetPaletteFade( pWork->pokeMcss[1], 0, 16, ANMCNTC(_POKEMON_CENTER_TIME/3)/16, 0 );
  POKETRADE_MESSAGE_WindowClose(pWork);

  GFL_BG_SetBackGroundColor(GFL_BG_FRAME1_M ,0);
  GFL_BG_SetBackGroundColor(GFL_BG_FRAME1_S ,0);

  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade1);

}

static void _changeDemo_ModelTrade1(POKEMON_TRADE_WORK* pWork)
{
  int i;

  {  // �t�F�[�h��
    _setFadeMask(pWork->pD2Fade);
    if(pWork->pModelFade){
      _EFFTOOL_CalcPaletteFade(pWork->pModelFade);
    }
    _pokeMoveFunc(pWork->pMoveMcss[0]);
    _pokeMoveFunc(pWork->pMoveMcss[1]);
  }


  if(pWork->anmCount > ANMCNTC(_POKEMON_CENTER_TIME)){  //�t�F�[�h���� �|�P�����ړ�����
    GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_OBJ );
    GFL_DISP_GXS_SetVisibleControlDirect( 0 );

    POKMEONTRADE_RemoveCoreResource( pWork);
    
    pWork->cellUnit = GFL_CLACT_UNIT_Create( 3 , 0 , pWork->heapID );
    _freePaletteFade(pWork->pModelFade);
    pWork->pModelFade = NULL;

    _setNextAnim(pWork, 0);

    {
      POKEMON_PARAM* pp= IRC_POKEMONTRADE_GetRecvPP(pWork, 0);
      if(! PP_Get(pp,ID_PARA_tamago_flag,NULL) ){
        int monsno = PP_Get(pp,ID_PARA_monsno_egg,NULL);
        int formno = PP_Get(pp,ID_PARA_form_no,NULL);
        PMVOICE_Play( monsno, formno, 64, FALSE, 0, 0, FALSE, 0 );
      }
      if(pWork->pMoveMcss[0]){
        GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
        pWork->pMoveMcss[0]=NULL;
      }
      {
        VecFx32 apos;
        MCSS_GetPosition(pWork->pokeMcss[0], &apos);
        pWork->pMoveMcss[0] = _pokeTblMoveCreate(pWork->pokeMcss[0], 13*3, &apos, _triJumpTbl,  pWork->heapID);
      }
    }
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade2_1);
  }
  
}

static void _changeDemo_ModelTrade2_1(POKEMON_TRADE_WORK* pWork)
{

  if(pWork->anmCount > _POKEMON_VOICE_TIME){
    if(pWork->pMoveMcss[0]){
      GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
      pWork->pMoveMcss[0]=NULL;
    }
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade2);
  }

  _pokeMoveFunc(pWork->pMoveMcss[0]);
}


//3d�X�^�[�g
static void _changeDemo_ModelTrade2(POKEMON_TRADE_WORK* pWork)
{
  int i;

  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  GFL_CLACT_SYS_Delete();
  IRC_POKETRADE_DEMOCLACT_Create(pWork);
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 3 , 0 , pWork->heapID );

  pWork->pPokemonTradeDemo = GFL_HEAP_AllocClearMemory(pWork->heapID,sizeof(POKEMONTRADE_DEMO_WORK));
  pWork->pPokemonTradeDemo->heapID = pWork->heapID;

  IRC_POKETRADE_SetSubdispGraphicDemo(pWork,0);
  G2S_BlendNone();

  G2_BlendNone();
  IRC_POKETRADEDEMO_SetModel( pWork, TRADE01_OBJECT);
  
  IRCPOKETRADE_PokeCreateMcss(pWork, 1, 0, IRC_POKEMONTRADE_GetRecvPP(pWork,1),TRUE );  //���藠
  IRCPOKETRADE_PokeCreateMcss(pWork, 2, 0, IRC_POKEMONTRADE_GetRecvPP(pWork,0),TRUE );  //�݂�����
  IRCPOKETRADE_PokeCreateMcss(pWork, 3, 1, IRC_POKEMONTRADE_GetRecvPP(pWork,1),TRUE );  //����\
  MCSS_SetVanishFlag( pWork->pokeMcss[1] );
  MCSS_SetVanishFlag( pWork->pokeMcss[2] );
  MCSS_SetVanishFlag( pWork->pokeMcss[3] );
  POKEMONTRADE_DEMO_PTC_Init(pWork->pPokemonTradeDemo);
  POKEMONTRADE_DEMO_ICA_Init(pWork->pPokemonTradeDemo);


  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade3);


}




static void _changeDemo_ModelTrade3(POKEMON_TRADE_WORK* pWork)
{


  if(pWork->anmCount == ANMCNTC(_POKEMON_DELETE_TIME-3)){
    //�����𔒂�����
    MCSS_SetPaletteFade( pWork->pokeMcss[0], 8, 8, 0, 0xffff );
  }
  if(pWork->anmCount == ANMCNTC(_POKEMON_DELETE_TIME-2)){
    //�����𔒂�����
    MCSS_SetPaletteFade( pWork->pokeMcss[0], 16, 16, 0, 0xffff );
  }

  if(pWork->anmCount == ANMCNTC(_POKEMON_DELETE_TIME-1)){
    _pokemonCreateCLACTInit(pWork);
    _pokemonCreateCLACTAdd(pWork);
  }
  
  if(pWork->anmCount == ANMCNTC(_POKEMON_DELETE_TIME)){
    GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_OBJ );
    GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  }

  if(pWork->anmCount == ANMCNTC(_POKEMON_DELETE_TIME)){
    MCSS_SetVanishFlag( pWork->pokeMcss[0] );
//      IRCPOKETRADE_PokeDeleteMcss(pWork,0);
  }


  if(pWork->anmCount == ANMCNTC(_BALL_PARTICLE_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[PTC_KIND_DEMO1], DERMO_TEX001, NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_PARTICLE_DEMO2_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[PTC_KIND_DEMO2], DEMO_TEX002, NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_PARTICLE_DEMO3_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[PTC_KIND_DEMO3], DEMO_TEX003, NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_PARTICLE_DEMO3_START2)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[PTC_KIND_ORG], DEMO_TEX003, NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_PARTICLE_DEMO4_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[PTC_KIND_DEMO4], DEMO_TEX004,  NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_PARTICLE_DEMO4_START2)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[PTC_KIND_DEMO8], DEMO_TEX004,  NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_PARTICLE_DEMO5_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[PTC_KIND_DEMO5], DEMO_TEX005, _ballinEmitFunc, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_PARTICLE_DEMO6_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[PTC_KIND_DEMO6], DEMO_TEX006, NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_PARTICLE_DEMO7_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[PTC_KIND_DEMO7], DEMO_TEX007,_balloutEmitFunc, pWork);
  }

  if(pWork->anmCount == ANMCNTC(_OAM_POKECREATE_START2)){
    _pokemonCreateCLACTAdd(pWork);
  }
  if(pWork->anmCount == ANMCNTC(_POKEUP_WHITEOUT_START)){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE,
                       ANMCNTC(_POKEUP_WHITEOUT_TIMER), 1, pWork->heapID );

  }
  if(pWork->anmCount == ANMCNTC(_POKEUP_WHITEIN_START)){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_WHITE,
                       ANMCNTC(_POKEUP_WHITEIN_TIMER), 1, pWork->heapID );
  }

  if(pWork->anmCount == ANMCNTC(_POKE_APPEAR_START)){
    MCSS_ResetVanishFlag( pWork->pokeMcss[2] );
    //IRCPOKETRADE_PokeCreateMcss(pWork, 0, 0, IRC_POKEMONTRADE_GetRecvPP(pWork,0) );
    {  //�����ʒu�ݒ�
      VecFx32 apos;
      apos.x = _POKEMON_PLAYER_APPEAR_POSX;
      apos.y = _POKEMON_PLAYER_APPEAR_POSY;
      apos.z = _POKEMON_PLAYER_APPEAR_POSZ;
      MCSS_SetPosition( pWork->pokeMcss[2] ,&apos );
      apos.x = _POKEMON_PLAYER_UP_POSX;
      apos.y = _POKEMON_PLAYER_UP_POSY;
      apos.z = _POKEMON_PLAYER_UP_POSZ;
      pWork->pMoveMcss[0] = _pokeMoveCreate(pWork->pokeMcss[2], ANMCNTC(_POKE_APPEAR_TIME), &apos, pWork->heapID);
      MCSS_SetAnmStopFlag(pWork->pokeMcss[2]);

      MCSS_GetScale( pWork->pokeMcss[2], &apos );
      apos.x *= _FRONT_POKMEON_SCALE_SIZE;
      apos.y *= _FRONT_POKMEON_SCALE_SIZE;
      MCSS_SetScale( pWork->pokeMcss[2], &apos );
    }
  }

  if(pWork->anmCount == ANMCNTC(_POKE_APPEAR_START+1)){
    MCSS_ResetVanishFlag( pWork->pokeMcss[3] );
    
 //   IRCPOKETRADE_PokeCreateMcss(pWork, 1, 1, IRC_POKEMONTRADE_GetRecvPP(pWork,1) );
    {
      VecFx32 apos;
      apos.x = _POKEMON_FRIEND_APPEAR_POSX;
      apos.y = _POKEMON_FRIEND_APPEAR_POSY;
      apos.z = _POKEMON_FRIEND_APPEAR_POSZ;
      MCSS_SetPosition( pWork->pokeMcss[3] ,&apos );

      apos.x = _POKEMON_FRIEND_DOWN_POSX;
      apos.y = _POKEMON_FRIEND_DOWN_POSY;
      apos.z = _POKEMON_FRIEND_DOWN_POSZ;
      pWork->pMoveMcss[1] = _pokeMoveCreate(pWork->pokeMcss[3], ANMCNTC(_POKE_APPEAR_TIME), &apos , pWork->heapID);
      MCSS_SetAnmStopFlag(pWork->pokeMcss[3]);
      MCSS_GetScale( pWork->pokeMcss[3], &apos );
      apos.x *= _BACK_POKMEON_SCALE_SIZE;
      apos.y *= _BACK_POKMEON_SCALE_SIZE;
      MCSS_SetScale( pWork->pokeMcss[3], &apos );
    }
    //�ړ��ݒ�
  }

  if(pWork->anmCount == ANMCNTC(_POKE_APPEAR_START2)){
    VecFx32 apos;
    apos.x = _POKEMON_PLAYER_UP_POSX;
    apos.y = _POKEMON_PLAYER_UP_POSY;
    apos.z = _POKEMON_PLAYER_UP_POSZ;
    _pokeMoveRenew(pWork->pMoveMcss[0],ANMCNTC(_POKE_APPEAR_TIME2),&apos);
    apos.x = _POKEMON_FRIEND_DOWN_POSX;
    apos.y = _POKEMON_FRIEND_DOWN_POSY;
    apos.z = _POKEMON_FRIEND_DOWN_POSZ;
    _pokeMoveRenew(pWork->pMoveMcss[1],ANMCNTC(_POKE_APPEAR_TIME2),&apos);
  }
  
  if(ANMCNTC(_POKE_SIDEOUT_START) == pWork->anmCount){
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[1]);
    pWork->pMoveMcss[1]=NULL;

    {  //�����ʒu�ݒ�
      VecFx32 apos;
      apos.x = _POKEMON_PLAYER_SIDEOUT_POSX;
      apos.y = _POKEMON_PLAYER_SIDEOUT_POSY;
      apos.z = _POKEMON_PLAYER_SIDEOUT_POSZ;
      pWork->pMoveMcss[0] = _pokeMoveCreate(pWork->pokeMcss[2], ANMCNTC(_POKE_SIDEOUT_TIME), &apos, pWork->heapID);
      apos.x = _POKEMON_FRIEND_SIDEOUT_POSX;
      apos.y = _POKEMON_FRIEND_SIDEOUT_POSY;
      apos.z = _POKEMON_FRIEND_SIDEOUT_POSZ;
      pWork->pMoveMcss[1] = _pokeMoveCreate(pWork->pokeMcss[3], ANMCNTC(_POKE_SIDEOUT_TIME), &apos , pWork->heapID);
      pWork->pMoveMcss[0]->wave=1;
      pWork->pMoveMcss[1]->wave=1;
    }
  }

  if(ANMCNTC(_POKE_SIDEIN_START) == pWork->anmCount){
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    MCSS_ResetVanishFlag( pWork->pokeMcss[0] );
    MCSS_SetPaletteFade( pWork->pokeMcss[0], 0, 0, 0, 0x7fff  );
    {  //�����ʒu�ݒ�
      VecFx32 apos;
      apos.x = _POKEMON_PLAYER_SIDEST_POSX;
      apos.y = _POKEMON_PLAYER_SIDEST_POSY;
      apos.z = _POKEMON_PLAYER_SIDEST_POSZ;
      MCSS_SetPosition( pWork->pokeMcss[0] ,&apos );
      apos.x = _POKEMON_PLAYER_SIDEIN_POSX;
      apos.y = _POKEMON_PLAYER_SIDEIN_POSY;
      apos.z = _POKEMON_PLAYER_SIDEIN_POSZ;
      pWork->pMoveMcss[0] = _pokeMoveCreate(pWork->pokeMcss[0], ANMCNTC(_POKE_SIDEIN_TIME), &apos, pWork->heapID);
      MCSS_GetScale( pWork->pokeMcss[0], &apos );
      apos.x *= _BACK_POKMEON_SCALE_SIZE;
      apos.y *= _BACK_POKMEON_SCALE_SIZE;
      MCSS_SetScale( pWork->pokeMcss[0], &apos );
    }

    GFL_HEAP_FreeMemory(pWork->pMoveMcss[1]);
    pWork->pMoveMcss[1]=NULL;
    MCSS_ResetVanishFlag( pWork->pokeMcss[1] );
    {  //�����ʒu�ݒ�
      VecFx32 apos;
      apos.x = _POKEMON_FRIEND_SIDEST_POSX;
      apos.y = _POKEMON_FRIEND_SIDEST_POSY;
      apos.z = _POKEMON_FRIEND_SIDEST_POSZ;
      MCSS_SetPosition( pWork->pokeMcss[1] ,&apos );
      apos.x = _POKEMON_FRIEND_SIDEIN_POSX;
      apos.y = _POKEMON_FRIEND_SIDEIN_POSY;
      apos.z = _POKEMON_FRIEND_SIDEIN_POSZ;
      pWork->pMoveMcss[1] = _pokeMoveCreate(pWork->pokeMcss[1], ANMCNTC(_POKE_SIDEIN_TIME), &apos , pWork->heapID);
      MCSS_GetScale( pWork->pokeMcss[1], &apos );
      apos.x *= _FRONT_POKMEON_SCALE_SIZE;
      apos.y *= _FRONT_POKMEON_SCALE_SIZE;
      MCSS_SetScale( pWork->pokeMcss[1], &apos );
    }
    pWork->pMoveMcss[0]->wave=1;
    pWork->pMoveMcss[1]->wave=1;
  }

  if(ANMCNTC(_POKE_SIDEIN_START2) == pWork->anmCount){
    _pokeMoveRenew(pWork->pMoveMcss[0],ANMCNTC(_POKE_SIDEIN_TIME2),&pWork->pMoveMcss[0]->end);
    _pokeMoveRenew(pWork->pMoveMcss[1],ANMCNTC(_POKE_SIDEIN_TIME2),&pWork->pMoveMcss[1]->end);
  }

  
  if(ANMCNTC(_POKE_LEAVE_START) == pWork->anmCount){
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[1]);
    pWork->pMoveMcss[1]=NULL;


    {  //�����ʒu�ݒ�
      VecFx32 apos;
      apos.x = _POKEMON_PLAYER_LEAVE_POSX;
      apos.y = _POKEMON_PLAYER_LEAVE_POSY;
      apos.z = _POKEMON_PLAYER_LEAVE_POSZ;
      pWork->pMoveMcss[0] = _pokeMoveCreate(pWork->pokeMcss[0], ANMCNTC(_POKE_LEAVE_TIME), &apos, pWork->heapID);
      apos.x = _POKEMON_FRIEND_LEAVE_POSX;
      apos.y = _POKEMON_FRIEND_LEAVE_POSY;
      apos.z = _POKEMON_FRIEND_LEAVE_POSZ;
      pWork->pMoveMcss[1] = _pokeMoveCreate(pWork->pokeMcss[1], ANMCNTC(_POKE_LEAVE_TIME), &apos , pWork->heapID);
    }
  }
  if(ANMCNTC(_POKEMON_CREATE_TIME-3) == pWork->anmCount){
//    IRCPOKETRADE_PokeDeleteMcss(pWork, 1);
//    IRCPOKETRADE_PokeCreateMcss(pWork, 1, 1, IRC_POKEMONTRADE_GetRecvPP(pWork,1) );
//    MCSS_ResetVanishFlag( pWork->pokeMcss[3] );


  }

#if 1
  if(ANMCNTC(_POKEMON_CREATE_TIME) == pWork->anmCount){
      VecFx32 apos;
    MCSS_ResetVanishFlag(pWork->pokeMcss[3]);
    apos.x = _POKEMON_PLAYER_CENTER_POSX;
    apos.y = _POKEMON_PLAYER_CENTER_POSY;
    apos.z = _POKEMON_PLAYER_CENTER_POSZ;
    MCSS_SetPosition( pWork->pokeMcss[3] ,&apos );
    MCSS_SetAnmStopFlag(pWork->pokeMcss[3]);

    MCSS_SetPaletteFade( pWork->pokeMcss[3], 16, 16, 0, 0x7fff );
  }
  if(ANMCNTC(_POKEMON_CREATE_TIME+1) == pWork->anmCount){
    MCSS_SetPaletteFade( pWork->pokeMcss[3], 8, 8, 0, 0x7fff );
  }
  if(ANMCNTC(_POKEMON_CREATE_TIME+2) == pWork->anmCount){
    MCSS_SetPaletteFade( pWork->pokeMcss[3], 0, 0, 0, 0x7fff  );
  }
#endif


  if(pWork->anmCount == ANMCNTC(_POKECHANGE_WHITEOUT_START)){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE,
                       ANMCNTC(_POKECHANGE_WHITEOUT_TIMER), 1, pWork->heapID );
  }
  if(pWork->anmCount == ANMCNTC(_POKECHANGE_WHITEIN_START)){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_WHITE,
                       ANMCNTC(_POKECHANGE_WHITEIN_TIMER), 1, pWork->heapID );
  }
  if(pWork->anmCount == ANMCNTC(_POKESPLASH_WHITEOUT_START)){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE,
                       ANMCNTC(_POKESPLASH_WHITEOUT_TIMER), 1, pWork->heapID );
  }
  if(pWork->anmCount == ANMCNTC(_POKESPLASH_WHITEIN_START)){
    _WIPE_SYS_StartRap(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_WHITE,
                       ANMCNTC(_POKESPLASH_WHITEIN_TIMER), 1, pWork->heapID );
  }
  //OS_TPrintf("C %d\n",pWork->anmCount);

  if(pWork->anmCount == ANMCNTC(_DEMO_END-1)){
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[1]);
    pWork->pMoveMcss[1]=NULL;
    IRC_POKETRADEDEMO_RemoveModel( pWork);
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade20);
  }
  _pokeMoveFunc(pWork->pMoveMcss[0]);
  _pokeMoveFunc(pWork->pMoveMcss[1]);
}


static void _changeDemo_ModelTrade20(POKEMON_TRADE_WORK* pWork)
{
  _pokemonCreateCLACTExit(pWork);
  POKEMONTRADE_DEMO_PTC_End(pWork->pPokemonTradeDemo,PTC_KIND_NUM_MAX);
  POKEMONTRADE_DEMO_ICA_Delete(pWork->pPokemonTradeDemo);
  GFL_HEAP_FreeMemory(pWork->pPokemonTradeDemo);
  pWork->pPokemonTradeDemo = NULL;

  _CHANGE_STATE(pWork,POKMEONTRADE_SAVE_Init);

}

//-----------------------------------------------------------------------------------------
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
  _EFFTOOL_PAL_FADE_WORK* pwk = GFL_HEAP_AllocClearMemory(heapID,sizeof(_EFFTOOL_PAL_FADE_WORK));

  pwk->g3DRES = g3DRES;
  pwk->pData_dst  = GFL_HEAP_AllocClearMemory( heapID, size );
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
  if(pwk){
    GFL_HEAP_FreeMemory( pwk->pData_dst );
    GFL_HEAP_FreeMemory( pwk );
  }
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
 *	@brief      MCSS�ړ�����
 *	@param[in,out]	_POKEMCSS_MOVE_WORK   �ړ��|�C���^
 *	@param[in]	time   �ړ�����
 *	@param[in]	pPos    �ŏI�ړ���
 */
//-------------------------------------------------

static void _pokeMoveRenew(_POKEMCSS_MOVE_WORK* pPoke,int time, const VecFx32* pPos)
{
  {
    VecFx32 apos;
    MCSS_GetPosition(pPoke->pMcss, &apos);
    pPoke->time = time;
    pPoke->nowcount=0;
    GFL_STD_MemCopy(pPos, &pPoke->end, sizeof(VecFx32));
    GFL_STD_MemCopy(&apos, &pPoke->start, sizeof(VecFx32));
  }
}

//-------------------------------------------------
/**
 *	@brief      MCSS�ړ����ߍ쐬
 *	@param[in]	pokeMcss   �Ώ�MCSS
 *	@param[in]	time   �ړ�����
 *	@param[in]	pPos    �ŏI�ړ���
 *	@param[in]	HEAPID  HEAPID
 */
//-------------------------------------------------

static _POKEMCSS_MOVE_WORK* _pokeMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, HEAPID heapID)
{
  _POKEMCSS_MOVE_WORK* pPoke = GFL_HEAP_AllocClearMemory(heapID, sizeof(_POKEMCSS_MOVE_WORK));

  pPoke->pMcss = pokeMcss;
  _pokeMoveRenew(pPoke,time,pPos);
  pPoke->percent=0.0f;
  return pPoke;

}



//-------------------------------------------------
/**
 *	@brief      MCSS�e�[�u���ړ����ߍ쐬
 *	@param[in]	pokeMcss   �Ώ�MCSS
 *	@param[in]	time   �ړ�����
 *	@param[in]	pPos    �ŏI�ړ���
 *	@param[in]	HEAPID  HEAPID
 */
//-------------------------------------------------

static _POKEMCSS_MOVE_WORK* _pokeTblMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, VecFx32* pTbl, HEAPID heapID)
{
  _POKEMCSS_MOVE_WORK* pPoke = GFL_HEAP_AllocClearMemory(heapID, sizeof(_POKEMCSS_MOVE_WORK));

  pPoke->pMcss = pokeMcss;
  _pokeMoveRenew(pPoke,time,pPos);
  pPoke->percent=0.0f;
  pPoke->MoveTbl = pTbl;
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

static fx32 _movemath(fx32 st,fx32 en,_POKEMCSS_MOVE_WORK* pMove,BOOL bWave)
{
  fx32 re;
  re = en - st;
  re = re / pMove->time;
  

  re = st + re * pMove->nowcount;
  if(pMove->wave && bWave){
    re = en + FX_SinIdx(pMove->sins) * _WAVE_NUM;
  }
  return re;
}


//-------------------------------------------------
/**
 *	@brief      �e�[�u���ɂ��ړ��֐�
 *	@param[in]	pMove
 */
//-------------------------------------------------

static void _pokeMoveTblFunc(_POKEMCSS_MOVE_WORK* pMove)
{
  VecFx32 apos,aposold;
  VecFx32* pPMT = &pMove->MoveTbl[pMove->nowcount];
  
  apos.x = pPMT->x + pMove->start.x;
  apos.y = pPMT->y + pMove->start.y;
  apos.z = pPMT->z + pMove->start.z;

  MCSS_SetPosition( pMove->pMcss ,&apos );


}

//-------------------------------------------------
/**
 *	@brief      MCSS�ړ��֐�
 *	@param[in]	pMove   �ړ����[�N
 */
//-------------------------------------------------
static void _pokeMoveFunc(_POKEMCSS_MOVE_WORK* pMove)
{
  if(!pMove){
    return;
  }
  pMove->nowcount++;
  if(pMove->time < pMove->nowcount){
    return;
  }
  if(pMove->time != pMove->nowcount)
  {
    if(pMove->MoveTbl){
      _pokeMoveTblFunc(pMove);
      return;
    }
    else{
      VecFx32 apos,aposold;
      MCSS_GetPosition( pMove->pMcss ,&aposold );

      apos.x = _movemath(pMove->start.x, pMove->end.x ,pMove, FALSE);
      if(pMove->wave){
        float an = FX_FX32_TO_F32(aposold.x - apos.x);
        an = an * 374;
        pMove->sins -= (s32)an;
      }
      apos.y = _movemath(pMove->start.y, pMove->end.y ,pMove, TRUE);
      apos.z = _movemath(pMove->start.z, pMove->end.z ,pMove, FALSE);
      MCSS_SetPosition( pMove->pMcss ,&apos );
    }
  }
  else{
    MCSS_SetPosition( pMove->pMcss ,&pMove->end );
  }
}




