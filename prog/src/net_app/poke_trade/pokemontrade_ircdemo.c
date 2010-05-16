//=============================================================================
/**
 * @file	  pokemontrade_ircdemo.c
 * @bfief	  �|�P���������ԊO���̃f���A�j���[�V����
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/11/05
 */
//=============================================================================

#include <gflib.h>


#include "arc_def.h"
#include "net/network_define.h"
#include "system/wipe.h"

#include "net_app/pokemontrade.h"
#include "system/main.h"

#include "poke_icon.naix"
#include "pokeicon/pokeicon.h"

#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"

#include "msg/msg_poke_trade.h"
#include "ircbattle.naix"
#include "trade.naix"



#include "poke_tool/poke_tool_def.h"
#include "box_m_obj_NANR_LBLDEFS.h"
#include "p_st_obj_d_NANR_LBLDEFS.h"
#include "poke_tool/status_rcv.h"
#include "tradedemo.naix"
#include "tradeirdemo.naix"

#include "pokemontrade_local.h"
//#include "app/mailtool.h"

#include "spaheadir.h"
#include "pokemontrade_demo.cdat"
#include "sound/pm_voice.h"


static void _changeDemo_ModelTrade2_1(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelT1(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelT2(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade0(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade1(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade2(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade3(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade20(POKEMON_TRADE_WORK* pWork);
static void _changeDemo_ModelTrade2_jump(POKEMON_TRADE_WORK* pWork);


//#include "pokemontrade_mcss.c"
#include "pokemontrade_fade.c"
#include "pokemontrade_demo_common.c"




//------------------------------------------------------------------
/**
 * @brief   �|�P�������ړ�����O�ɃA�j���I���҂�
 * @param   POKEMON_TRADE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------
void POKMEONTRADE_IRCDEMO_ChangeDemo(POKEMON_TRADE_WORK* pWork)
{
  VecFx32 apos;
  int i;

  _demoBGMChangeStart(pWork);
  
  pWork->mcssStop[0] = TRUE;
  MCSS_SetAnimeIndex(pWork->pokeMcss[0], 0);
  MCSS_SetAnmStopFlag( pWork->pokeMcss[0]);
  if(pWork->type==POKEMONTRADE_TYPE_EVENT){  //�f���̂Ƃ��͐^�񒆂���
    VecFx32 pos={_POKEMON_PLAYER_CENTER_POSX,_POKEMON_PLAYER_CENTER_POSY, _POKEMON_PLAYER_CENTER_POSZ};
    MCSS_SetPosition( pWork->pokeMcss[0] ,&pos );
  }
  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelT1);
}

static void _changeDemo_ModelTrade2(POKEMON_TRADE_WORK* pWork)
{
  int i;

  POKMEONTRADE_RemoveCoreResource( pWork);//@new
  GFL_CLACT_SYS_Delete();
  IRC_POKETRADE_DEMOCLACT_Create(pWork);
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 3 , 0 , pWork->heapID );

  pWork->pPokemonTradeDemo = GFL_HEAP_AllocClearMemory(pWork->heapID,sizeof(POKEMONTRADE_DEMO_WORK));
  pWork->pPokemonTradeDemo->heapID = pWork->heapID;

  IRC_POKETRADE_SetSubdispGraphicDemo(pWork,1);  //�ԊO���w�i
  G2S_BlendNone();

  G2_BlendNone();
  IRC_POKETRADEDEMO_SetModel( pWork, TRADEIR_OBJECT); //�ԊO�����f��

  IRCPOKETRADE_PokeCreateMcss(pWork, 1, 0, IRC_POKEMONTRADE_GetRecvPP(pWork,1),TRUE );  //���藠
  IRCPOKETRADE_PokeCreateMcss(pWork, 2, 0, IRC_POKEMONTRADE_GetRecvPP(pWork,0),TRUE );  //�݂�����
  IRCPOKETRADE_PokeCreateMcss(pWork, 3, 1, IRC_POKEMONTRADE_GetRecvPP(pWork,1),TRUE );  //����\
  MCSS_GetScale( pWork->pokeMcss[3], &pWork->pPokemonTradeDemo->ReturnPos );


  MCSS_SetVanishFlag( pWork->pokeMcss[1] );
  MCSS_SetVanishFlag( pWork->pokeMcss[2] );
  MCSS_SetVanishFlag( pWork->pokeMcss[3] );
  POKEMONTRADE_DEMO_IRPTC_Init(pWork->pPokemonTradeDemo);
  POKEMONTRADE_DEMO_IRICA_Init(pWork->pPokemonTradeDemo);


  _setNextAnim(pWork, 0);
  _CHANGE_STATE(pWork,_changeDemo_ModelTrade3);


  G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG0,GX_BLEND_PLANEMASK_BD,0,0);

}



static void _changeDemo_ModelTrade3(POKEMON_TRADE_WORK* pWork)
{


  if(pWork->anmCount == ANMCNTC(_IR_POKEMON_DELETE_TIME-3)){
    //�����𔒂�����
    MCSS_SetPaletteFade( pWork->pokeMcss[0], 8, 8, 0, 0xffff );
  }
  if(pWork->anmCount == ANMCNTC(_IR_POKEMON_DELETE_TIME-2)){
    //�����𔒂�����
    MCSS_SetPaletteFade( pWork->pokeMcss[0], 16, 16, 0, 0xffff );
  }
  if(pWork->anmCount == ANMCNTC(_IR_POKEMON_DELETE_TIME-1)){
    _pokemonCreateCLACTInit(pWork);
    _pokemonCreateCLACTAdd(pWork);
  }

  if(pWork->anmCount == ANMCNTC(_IR_POKEMON_DELETE_TIME)){
    GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_OBJ );
    GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  }

  if(pWork->anmCount == ANMCNTC(_IR_POKEMON_DELETE_TIME)){
    MCSS_SetVanishFlag( pWork->pokeMcss[0] );
  }


  if(pWork->anmCount == ANMCNTC(_IRTEX001_01_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[IRPTC_KIND_DEMO1], IR_TEX001, NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_IRTEX001_02_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[IRPTC_KIND_DEMO1_2], IR_TEX001, NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_IRTEX002_01_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[IRPTC_KIND_DEMO2], IR_TEX002, NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_IRTEX003_01_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[IRPTC_KIND_DEMO3], IR_TEX003, NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_IRTEX003_02_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[IRPTC_KIND_DEMO3_2], IR_TEX003,  NULL, pWork);
  }
  if(pWork->anmCount == ANMCNTC(_IRTEX004_01_START)){
    GFL_PTC_CreateEmitterCallback(pWork->pPokemonTradeDemo->ptc[IRPTC_KIND_DEMO4], IR_TEX004,  NULL, pWork);
  }

  if(pWork->anmCount == ANMCNTC(_IR_OAM_POKECREATE)){
    _pokemonCreateCLACTAdd(pWork);
  }
  if(pWork->anmCount == ANMCNTC(_IR_POKEUP_WHITEOUT_START)){
    WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE,
                       ANMCNTC(_IR_POKEUP_WHITEOUT_TIMER), 1, pWork->heapID );

  }
  if(pWork->anmCount == ANMCNTC(_IR_POKEUP_WHITEIN_START)){
    WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_WHITE,
                       ANMCNTC(_IR_POKEUP_WHITEIN_TIMER), 1, pWork->heapID );
  }

  if(pWork->anmCount == ANMCNTC(_IR_POKE_APPEAR_START)){
    MCSS_ResetVanishFlag( pWork->pokeMcss[2] );
    //IRCPOKETRADE_PokeCreateMcss(pWork, 0, 0, IRC_POKEMONTRADE_GetRecvPP(pWork,0) );
    {  //�����ʒu�ݒ�
      VecFx32 apos;
      apos.x = _IR_POKEMON_PLAYER_APPEAR_POSX;
      apos.y = _IR_POKEMON_PLAYER_APPEAR_POSY;
      apos.z = _IR_POKEMON_PLAYER_APPEAR_POSZ;
      MCSS_SetPosition( pWork->pokeMcss[2] ,&apos );
      apos.x = _IR_POKEMON_PLAYER_UP_POSX;
      apos.y = _IR_POKEMON_PLAYER_UP_POSY;
      apos.z = _IR_POKEMON_PLAYER_UP_POSZ;
      pWork->pMoveMcss[0] = POKEMONTRADE_pokeMoveCreate(pWork->pokeMcss[2], ANMCNTC(_IR_POKE_APPEAR_TIME), &apos, pWork->heapID);
      //MCSS_SetAnmStopFlag(pWork->pokeMcss[2]);

      MCSS_GetScale( pWork->pokeMcss[2], &apos );
      apos.x *= _IRC_FRONT_POKMEON_SCALE_SIZE;
      apos.y *= _IRC_FRONT_POKMEON_SCALE_SIZE;
      MCSS_SetScale( pWork->pokeMcss[2], &apos );

    }
  }

  if(pWork->anmCount == ANMCNTC(_IR_POKE_APPEAR_START+1)){
    MCSS_ResetVanishFlag( pWork->pokeMcss[3] );
    
 //   IRCPOKETRADE_PokeCreateMcss(pWork, 1, 1, IRC_POKEMONTRADE_GetRecvPP(pWork,1) );
    {
      VecFx32 apos;
      apos.x = _IR_POKEMON_FRIEND_APPEAR_POSX;
      apos.y = _IR_POKEMON_FRIEND_APPEAR_POSY;
      apos.z = _IR_POKEMON_FRIEND_APPEAR_POSZ;
      MCSS_SetPosition( pWork->pokeMcss[3] ,&apos );

      apos.x = _IR_POKEMON_FRIEND_DOWN_POSX;
      apos.y = _IR_POKEMON_FRIEND_DOWN_POSY;
      apos.z = _IR_POKEMON_FRIEND_DOWN_POSZ;
      pWork->pMoveMcss[1] = POKEMONTRADE_pokeMoveCreate(pWork->pokeMcss[3], ANMCNTC(_IR_POKE_APPEAR_TIME), &apos , pWork->heapID);
      //MCSS_SetAnmStopFlag(pWork->pokeMcss[3]);
      MCSS_SetScale( pWork->pokeMcss[3], &pWork->pPokemonTradeDemo->ReturnPos );
      MCSS_GetScale( pWork->pokeMcss[3], &apos );
      pWork->pPokemonTradeDemo->PushPos.x = apos.x;
      pWork->pPokemonTradeDemo->PushPos.y = apos.y;
      pWork->pPokemonTradeDemo->PushPos.z = apos.z;
//      apos.x *= _IRC_BACK_POKMEON_SCALE_SIZE;
//      apos.y *= _IRC_BACK_POKMEON_SCALE_SIZE;
//      MCSS_SetScale( pWork->pokeMcss[3], &pWork->pPokemonTradeDemo->ReturnPos );
      
    }

    {
      VecFx32 apos;
      MCSS_GetScale( pWork->pokeMcss[3], &apos );
      OS_TPrintf("��������2 %d %d\n",apos.x, apos.y);
    }


    //�ړ��ݒ�
  }
  if(pWork->anmCount == ANMCNTC(_IR_POKE_APPEAR_START+2)){
      MCSS_SetAlpha(pWork->pokeMcss[2], _IRC_FRONT_POKMEON_ALPHA);
      MCSS_SetAlpha(pWork->pokeMcss[3], _IRC_BACK_POKMEON_ALPHA);
  }


  
#if 1
  if(ANMCNTC(_IR_POKEMON_CREATE_TIME) == pWork->anmCount){

    _MCSS_ApperSet(pWork->pokeMcss[3],pWork);
    if(!pWork->bEncountMessageEach){     // �s��v�Ȃ̂ŃW�����v+��]
//      _MCSS_ApperSet(pWork->pokeMcss[3],pWork);
    }
    else{// ��v���Ă���̂ŃW�����v
  //    _MCSS_ApperSet(pWork->pokeMcss[1],pWork);
   //   MCSS_SetVanishFlag(pWork->pokeMcss[3]);
    }
    _pokemonApperLastCreate(pWork, 3);
#if 0    
    {  //�����o��
      POKEMON_PARAM* pp= IRC_POKEMONTRADE_GetRecvPP(pWork, 1);
      int id = PP_Get(pp,ID_PARA_id_no,NULL);
      if(! PP_Get(pp,ID_PARA_tamago_flag,NULL) ){
        int monsno = PP_Get(pp,ID_PARA_monsno_egg,NULL);
        int formno = PP_Get(pp,ID_PARA_form_no,NULL);
        PMVOICE_Play( monsno, formno, 64, FALSE, 0, 0, FALSE, 0 );
      }
    }
    _MCSS_ApperSet(pWork->pokeMcss[3],pWork);
    if(!pWork->bEncountMessageEach){     // �s��v�Ȃ̂ŃW�����v+��]
      VecFx32 apos;
      MCSS_GetPosition(pWork->pokeMcss[3], &apos);

      if(pWork->bEncountNoJump){
        pWork->pMoveMcss[3] = POKEMONTRADE_pokeTblMoveCreate(pWork->pokeMcss[3], elementof(_noJumpTbl), &apos, _noJumpTbl,  pWork->heapID);
      }
      else{
        pWork->pMoveMcss[3] = POKEMONTRADE_pokeTblMoveCreate(pWork->pokeMcss[3], elementof(_triJumpTbl), &apos, _triJumpTbl,  pWork->heapID);
      }
    }
    else{// ��v���Ă���̂ŃW�����v
      VecFx32 apos;

      _MCSS_ApperSet(pWork->pokeMcss[1],pWork);
      MCSS_GetPosition(pWork->pokeMcss[1], &apos);
      MCSS_SetVanishFlag(pWork->pokeMcss[3]);

      if(pWork->bEncountNoJump){
        pWork->pMoveMcss[3] = POKEMONTRADE_pokeTblMoveCreate(pWork->pokeMcss[1], elementof(_noJumpTbl), &apos, _noJumpTbl,  pWork->heapID);
      }
      else{
        pWork->pMoveMcss[3] = POKEMONTRADE_pokeTblMoveCreate(pWork->pokeMcss[1], elementof(_shortJumpTbl), &apos, _shortJumpTbl,  pWork->heapID);
      }
    }
#endif




  }
  if(ANMCNTC(_IR_POKEMON_CREATE_TIME+1) == pWork->anmCount){
    MCSS_SetPaletteFade( pWork->pokeMcss[3], 8, 8, 0, 0x7fff );
    MCSS_SetPaletteFade( pWork->pokeMcss[1], 8, 8, 0, 0x7fff );
  }
  if(ANMCNTC(_IR_POKEMON_CREATE_TIME+2) == pWork->anmCount){
    MCSS_SetPaletteFade( pWork->pokeMcss[3], 0, 0, 0, 0x7fff  );
    MCSS_SetPaletteFade( pWork->pokeMcss[1], 0, 0, 0, 0x7fff  );
  }

  if(pWork->bEncountMessageEach && (pWork->pMoveMcss[3]) &&
     (pWork->pMoveMcss[3]->nowcount == _JUMP_PEEK)){
    {
      VecFx32 apos;
      MCSS_GetPosition( pWork->pokeMcss[1] ,&apos );
      MCSS_SetPosition( pWork->pokeMcss[3] ,&apos );
    }
    MCSS_ResetVanishFlag( pWork->pokeMcss[3] );
    MCSS_SetVanishFlag( pWork->pokeMcss[1] );
    pWork->pMoveMcss[3]->pMcss = pWork->pokeMcss[3];
  }


#endif

  POKETRADE_SE_irSeStrat(pWork->anmCount);
  
  if(pWork->anmCount == ANMCNTC(_IR_POKECHANGE_WHITEOUT_START)){
    WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE,
                       ANMCNTC(_IR_POKECHANGE_WHITEOUT_TIMER), 1, pWork->heapID );
  }
  if(pWork->anmCount == ANMCNTC(_IR_POKECHANGE_WHITEIN_START)){
    WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_WHITE,
                       ANMCNTC(_IR_POKECHANGE_WHITEIN_TIMER), 1, pWork->heapID );
  }
  OS_TPrintf("C %d\n",pWork->anmCount);

  if(pWork->anmCount == ANMCNTC(_IR_DEMO_END-1)){
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[0]);
    pWork->pMoveMcss[0]=NULL;
    GFL_HEAP_FreeMemory(pWork->pMoveMcss[1]);
    pWork->pMoveMcss[1]=NULL;
    IRC_POKETRADEDEMO_RemoveModel( pWork);
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade20);
  }
  POKEMONTRADE_pokeMoveFunc(pWork->pMoveMcss[0],pWork);
  POKEMONTRADE_pokeMoveFunc(pWork->pMoveMcss[1],pWork);
  POKEMONTRADE_pokeMoveFunc(pWork->pMoveMcss[2],pWork);
  POKEMONTRADE_pokeMoveFunc(pWork->pMoveMcss[3],pWork);
}


