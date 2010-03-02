//=============================================================================
/**
 * @file    gsync_state.c
 * @brief   �Q�[���V���N
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date    09/04/30
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "arc_def.h"
//#include "libdpw/dwci_ghttp.h"
#include "net_app/gsync.h"
#include <nitroWiFi/nhttp.h>
#include "net/nitrowifidummy.h"

#include "system/main.h"  //HEAPID
#include "message.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"
#include "sound/pm_sndsys.h"
#include "system/wipe.h"
#include "net/network_define.h"
#include "savedata/wifilist.h"
#include "savedata/system_data.h"
#include "msg/msg_d_ohno.h"
#include "gsync_local.h"
#include "net/nhttp_rap.h"
#include "../../field/event_gsync.h"
#include "savedata/dreamworld_data.h"
#include "savedata/symbol_save.h"
#include "gsync_obj_NANR_LBLDEFS.h"
#include "gsync_poke.cdat"
#include "msg/msg_gsync.h"
#include "gsync.naix"

/*
��BGM��

SEQ_BGM_GAME_SYNC	�Q�[���V���NBGM

��SE��

SEQ_SE_SYS_24			�A�b�v�E�_�E�����[�h
SEQ_SE_SYS_25			�A�b�v���[�h������
SEQ_SE_SYS_26			�|�P��������
*/
#define _ZZZCOUNT (110)

typedef void (StateFunc)(G_SYNC_WORK* pState);



//
// PHP �Ƃ̃C���^�[�t�F�[�X�\����
//
typedef struct tag_EVENT_DATA
{
  unsigned long rom_code;
  unsigned short country_code;
  unsigned char id;
  unsigned char send_flag;
  unsigned long dec_code;
  unsigned short cat_id;
  unsigned short present;
  unsigned char status;
  unsigned char idname1[26];
  unsigned char idname2[26];
  unsigned char idname3[26];
  unsigned char idname4[26];
  unsigned char idname5[26];
}
  EVENT_DATA;



static void _changeState(G_SYNC_WORK* pWork,StateFunc* state);
static void _changeStateDebug(G_SYNC_WORK* pWork,StateFunc* state, int line);

#if DEBUG_ONLY_FOR_ohno
#define _NET_DEBUG (1)
#else
#define _NET_DEBUG (0)
#endif
#ifdef _NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pWork ,state)
#endif //_NET_DEBUG




struct _G_SYNC_WORK {
  HEAPID heapID;
  POKEMON_PARAM* pp;
  NHTTP_RAP_WORK* pNHTTPRap;
  GSYNC_DISP_WORK* pDispWork;  // �`��n
  GSYNC_MESSAGE_WORK* pMessageWork; //���b�Z�[�W�n
  APP_TASKMENU_WORK* pAppTask;
  EVENT_GSYNC_WORK* pParent;
  BOX_MANAGER * pBox;
  SAVE_CONTROL_WORK* pSaveData;
  GAMEDATA* pGameData;
  void* pTopAddr;
  int trayno;
  int indexno;
  StateFunc* state;      ///< �n���h���̃v���O�������
  vu32 count;
  int req;
  int getdataCount;
  int countTimer;
  BOOL bEnd;
  int zzzCount;
  int notNetEffect;  ///< �ʐM���Ė����ꍇ�̃G�t�F�N�g
  DREAM_WORLD_SERVER_ERROR_TYPE ErrorNo;   ///�G���[���������ꍇ�̔ԍ�
};


static void _ghttpKeyWait(G_SYNC_WORK* pWork);
static void _ghttpPokemonListDownload(G_SYNC_WORK* pWork);
static void _networkClose(G_SYNC_WORK* pWork);


//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(G_SYNC_WORK* pWork,StateFunc state)
{
  pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef _NET_DEBUG
static void _changeStateDebug(G_SYNC_WORK* pWork,StateFunc state, int line)
{
#ifdef DEBUG_ONLY_FOR_ohno
  OS_TPrintf("gsync: %d\n",line);
#endif
  _changeState(pWork, state);
}
#endif

//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�A�E�g����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeout(G_SYNC_WORK* pWork)
{
	if(WIPE_SYS_EndCheck()){
		_CHANGE_STATE( NULL);        // �I���
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�A�E�g����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeStart(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }

 
  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
  _CHANGE_STATE(_modeFadeout);        // �I���
}

//------------------------------------------------------------------------------
/**
 * @brief   �G���[�\������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ErrorDisp2(G_SYNC_WORK* pWork)
{

  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL)){
    _CHANGE_STATE(_networkClose);
  }
}


static void _ErrorDisp(G_SYNC_WORK* pWork)
{
  int gmm = GSYNC_ERR001 + pWork->ErrorNo - 1;

  if((pWork->ErrorNo <= 0) || (pWork->ErrorNo >= DREAM_WORLD_SERVER_ERROR_MAX)){
    gmm = DREAM_WORLD_SERVER_ERROR_ETC - 1 + GSYNC_ERR001;
  }

  GSYNC_MESSAGE_SystemMessageDisp(pWork->pMessageWork,gmm);
  _CHANGE_STATE(_ErrorDisp2);
}

//�ʐM�ؒf

static void _networkClose1(G_SYNC_WORK* pWork)
{
  if(!GFL_NET_IsInit()){
    _CHANGE_STATE(_modeFadeStart);
  }
}

static void _networkClose(G_SYNC_WORK* pWork)
{
  pWork->pParent->selectType = GAMESYNC_RETURNMODE_EXIT;
  if(!GFL_NET_IsInit()){
    _CHANGE_STATE(_modeFadeStart);
  }
  else{
    GFL_NET_Exit(NULL);
    _CHANGE_STATE(_networkClose1);
  }
}


static void _wakeupActio8(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  if(GFL_UI_KEY_GetTrg()){
    _CHANGE_STATE(_networkClose);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �Q�[���V���N����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wakeupAction7(G_SYNC_WORK* pWork)
{
  DREAMWORLD_SAVEDATA* pDream = DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData);

  if(pWork->pp){
    GFL_HEAP_FreeMemory(pWork->pp);
    pWork->pp=NULL;
  }

  if(BOXDAT_GetEmptyTrayNumberAndPos( pWork->pBox, &pWork->trayno, &pWork->indexno )){
    POKEMON_PARAM* pp = GFL_HEAP_AllocClearMemory( pWork->heapID, POKETOOL_GetWorkSize()) ;

    GFL_STD_MemCopy(DREAMWORLD_SV_GetSleepPokemon(pDream), pp, POKETOOL_GetWorkSize());

    BOXDAT_PutPokemon(pWork->pBox, PP_GetPPPPointerConst(pp));
    
    pWork->pp = pp;
    DREAMWORLD_SV_SetSleepPokemonFlg(pDream,FALSE);
  }


  
  GSYNC_MESSAGE_NickNameMessageDisp(pWork->pMessageWork,GSYNC_005, pWork->pp);
  GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_rug_ani3,NANR_gsync_obj_rug_ani4);

  GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_bed,NANR_gsync_obj_bed_ani);
  GSYNC_DISP_BlendSmokeStart(pWork->pDispWork,FALSE);
  
  
  GSYNC_DISP_PokemonIconCreate(pWork->pDispWork, PP_GetPPPPointer(pWork->pp),CLSYS_DRAW_MAIN);
  GSYNC_DISP_PokemonIconJump(pWork->pDispWork);

  PMSND_PlaySE(SEQ_SE_SYS_25);

  _CHANGE_STATE(_wakeupActio8);
}








//------------------------------------------------------------------------------
/**
 * @brief   �|�P������Ԍ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wakeupAction_save2(G_SYNC_WORK* pWork)
{
  if(GAMEDATA_SaveAsyncMain(pWork->pGameData) == SAVE_RESULT_OK){
    _CHANGE_STATE(_wakeupAction7);
  }
  //@todo �����R�}���h�̑��M���s������
  
}

//�e�X�g 

static void _wakeupAction_1(G_SYNC_WORK* pWork)
{

  GAMEDATA_SaveAsyncStart(pWork->pGameData);

  
  _CHANGE_STATE(_wakeupAction_save2);
}


//   �Ƌ���Z�[�u�G���A�Ɉړ�
static void _furnitureInSaveArea(DREAMWORLD_SAVEDATA* pDreamSave,DREAM_WORLD_SERVER_DOWNLOAD_DATA* pDream)
{
  int i;
  
  for(i=0;i<DREAM_WORLD_DATA_MAX_FURNITURE;i++){
    DREAMWORLD_SV_SetFurnitureData(pDreamSave, i,   &pDream->Furniture[i]);
  }
}


//   �A�C�e�����Z�[�u�G���A�Ɉړ�
static void _itemInSaveArea(DREAMWORLD_SAVEDATA* pDreamSave,DREAM_WORLD_SERVER_DOWNLOAD_DATA* pDream)
{
  int i;
  
  for(i=0;i<DREAM_WORLD_DATA_MAX_ITEMBOX;i++){
    DREAMWORLD_SV_SetItem(pDreamSave, i,   pDream->itemID[i], pDream->itemNum[i]);
  }
}


static void _datacheck(G_SYNC_WORK* pWork, DREAMWORLD_SAVEDATA* pDreamSave,DREAM_WORLD_SERVER_DOWNLOAD_DATA* pDream,gs_response* pRep )
{


  if(pRep->ret_cd != DREAM_WORLD_SERVER_ERROR_NONE){
    pWork->ErrorNo = pRep->ret_cd;
    GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    _CHANGE_STATE(_ErrorDisp);
    return;
  }
#if 0  //@todo BFS�Ƃ̘b���I����Ă���L��
    if(!pDream->bGet && (DREAMWORLD_SV_GetUploadCount() != pDream->uploadCount))
#else
    if(!pDream->bGet)
#endif
    {
      //pDreamSave->sleepPokemonState;  ///< �Q�Ă���|�P�����̏�Ԃ͂��܂肩�񂯂��Ȃ�

      //@todo�_�E�����[�h�ɍs��
      //      /u8 musicalNo;      ///< web�őI�������ԍ�  �����ꍇ 0xff
      //  u8 cgearNo;        ///< web�őI�������ԍ�  �����ꍇ 0xff
      //  u8 zukanNo;        ///< web�őI�������ԍ�  �����ꍇ 0xff
      
      //�|�P�����V���{���G���J�E���g
      SymbolSave_Set(SymbolSave_GetSymbolData(pWork->pSaveData), pDream->findPokemon,
                     pDream->findPokemonTecnique, pDream->findPokemonSex, pDream->findPokemonForm);
      //�T�C���C��
      DREAMWORLD_SV_SetSignin(pDreamSave,pDream->signin);
      // �Ƌ�
      _furnitureInSaveArea(pDreamSave, pDream);
      //�A�C�e��
      _itemInSaveArea(pDreamSave, pDream);
      //          _CHANGE_STATE();  //�Z�[�u�ɍs��?
    }
  _CHANGE_STATE(_wakeupAction_1);
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P������Ԍ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wakeupAction6(G_SYNC_WORK* pWork)
{
  if(GFL_NET_IsInit()){
    if(NHTTP_ERROR_NONE== NHTTP_RAP_Process(pWork->pNHTTPRap)){
      NET_PRINT("�I��\n");
      {
        u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        DREAMWORLD_SAVEDATA* pDreamSave = DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData);
        DREAM_WORLD_SERVER_DOWNLOAD_DATA* pDream = (DREAM_WORLD_SERVER_DOWNLOAD_DATA*)&pEvent[sizeof(gs_response)];
        gs_response* pRep = (gs_response*)pEvent;

        NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);

        _datacheck(pWork, pDreamSave, pDream, pRep);
      }
    }
  }
  else{
    _CHANGE_STATE(_wakeupAction7);
  }
}

static void _wakeupAction5(G_SYNC_WORK* pWork)
{
  if(GFL_NET_IsInit()){
    if(NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_DOWNLOAD, pWork->pNHTTPRap)){
      if(NHTTP_RAP_StartConnect(pWork->pNHTTPRap)){
        _CHANGE_STATE(_wakeupAction6);
      }
    }
  }
  else{
    if(GFL_UI_KEY_GetTrg()){
      _CHANGE_STATE(_wakeupAction6);
    }
  }
}

static void _wakeupActio4_2(G_SYNC_WORK* pWork)
{
 if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  _CHANGE_STATE(_wakeupAction5);
}


static void _wakeupAction4(G_SYNC_WORK* pWork)
{
  GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_004);
  GSYNC_DISP_ObjEnd(pWork->pDispWork, NANR_gsync_obj_zzz_ani);
  GSYNC_DISP_DreamSmokeBgStart(pWork->pDispWork);
  GSYNC_DISP_BlendSmokeStart(pWork->pDispWork,TRUE);
  GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_rug_ani3,NANR_gsync_obj_rug_ani2);
  PMSND_PlaySE(SEQ_SE_SYS_24);

  _CHANGE_STATE(_wakeupActio4_2);

}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����N��������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wakeupAction3(G_SYNC_WORK* pWork)
{

  if(pWork->zzzCount%_ZZZCOUNT==0){
    PMSND_PlaySE(SEQ_SE_SYS_26);
  }
  pWork->zzzCount++;

  
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      _CHANGE_STATE(_wakeupAction4);
    }
    else{
      _CHANGE_STATE(_networkClose);
    }
    GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����N��������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wakeupAction2(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  pWork->pAppTask = GSYNC_MESSAGE_YesNoStart(pWork->pMessageWork,GSYNC_YESNOTYPE_INFO);

  _CHANGE_STATE(_wakeupAction3);
}



//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����N��������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wakeupAction1(G_SYNC_WORK* pWork)
{

  GSYNC_DISP_ObjInit(pWork->pDispWork, NANR_gsync_obj_rug_ani3);
  GSYNC_DISP_ObjInit(pWork->pDispWork, NANR_gsync_obj_zzz_ani);

  GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_003);
  PMSND_PlaySE(SEQ_SE_SYS_26);

  _CHANGE_STATE(_wakeupAction2);

}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����N��������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _playStatusCheck(G_SYNC_WORK* pWork, int status , gs_response* pRep)
{
  switch(pRep->ret_cd){
  case DREAM_WORLD_SERVER_ERROR_NONE:    //����
    switch(pWork->pParent->selectType){
    case GSYNC_CALLTYPE_INFO:   //�Z�[�u�f�[�^��ł͐Q�����Ă���
      if(DREAM_WORLD_SLEEP_TYPE_SLEEP == status){
        _CHANGE_STATE(_wakeupAction1);
      }
      else if(DREAM_WORLD_SLEEP_TYPE_PLAYED == status){
        _CHANGE_STATE(_wakeupAction1);
      }
      else{
        //�����łȂ������ꍇ���N�����Ă悢
        NET_PRINT("!!!!!NOTSERVERSTATE %d %d\n",status, pWork->pParent->selectType);
        _CHANGE_STATE(_wakeupAction1);
      }
      break;
    case GSYNC_CALLTYPE_POKELIST:          //�Z�[�u�f�[�^��ł͖��点��|�P������I��
      //
      _CHANGE_STATE(_ghttpPokemonListDownload);
      break;
    }
    break;
  case DREAM_WORLD_SERVER_NO_DATA:   //�A�J�E���g���o�^�̏ꍇ
    switch(pWork->pParent->selectType){
    case GSYNC_CALLTYPE_INFO:
/*
      {          //���Ԃ��m�F����
        GFDATE gd = DREAMWORLD_SV_GetTime(DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData));
        RTCDate date;
        GFDATE_GFDate2RTCDate(gd, &date);
        if(GFL_RTC_GetDaysOffset(&date) > 0){
          _CHANGE_STATE(_wakeupAction1);   //�N��������
        }
        else{
          
        }
      }
   */
      _CHANGE_STATE(_wakeupAction1);   //�N��������
      break;
    case GSYNC_CALLTYPE_POKELIST:          //���点��|�P������I��
      _CHANGE_STATE(_ghttpPokemonListDownload);
      break;
    }
    break;
  default: //���ʂ̃G���[����
    pWork->ErrorNo = pRep->ret_cd;
    GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    _CHANGE_STATE(_ErrorDisp);
    break;
  }


}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P������Ԍ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpInfoWait1(G_SYNC_WORK* pWork)
{
  if(GFL_NET_IsInit()){
    if(NHTTP_ERROR_NONE== NHTTP_RAP_Process(pWork->pNHTTPRap)){
      NET_PRINT("�I��\n");
      {
        u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        gs_response* pRep = (gs_response*)pEvent;
        DREAM_WORLD_SERVER_STATUS_DATA* pDream = (DREAM_WORLD_SERVER_STATUS_DATA*)&pEvent[sizeof(gs_response)];

        NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);
        NET_PRINT("PlayStatus %d\n",pDream->PlayStatus);

        _playStatusCheck(pWork, pDream->PlayStatus, pRep);  //���򂷂�

      }
    }
  }
  else{
    _CHANGE_STATE(_wakeupAction1);
  }
    
}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P������Ԍ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpInfoWait0(G_SYNC_WORK* pWork)
{

  GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_006);
  
  if(GFL_NET_IsInit()){
    if(NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_ACCOUNTINFO, pWork->pNHTTPRap)){
      if(NHTTP_RAP_StartConnect(pWork->pNHTTPRap)){
        _CHANGE_STATE(_ghttpInfoWait1);
      }
    }
  }
  else{
    if(GFL_UI_KEY_GetTrg()){
      _CHANGE_STATE(_ghttpInfoWait1);
    }
  }
}




//------------------------------------------------------------------------------
/**    /gs?p=account.playstatus&gsid=123456789012&rom=32&langcode=32&dreamw=32
 * @brief   �|�P������Ԍ���  �A�J�E���g���������ǂ����������ł킩��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpPokemonListDownload1(G_SYNC_WORK* pWork)
{
  if(GFL_NET_IsInit()){
    if(NHTTP_ERROR_NONE== NHTTP_RAP_Process(pWork->pNHTTPRap)){
      {
        u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        gs_response* pRep = (gs_response*)pEvent;

        NHTTP_DEBUG_GPF_HEADER_PRINT(pRep);

        if(pRep->ret_cd == DREAM_WORLD_SERVER_ERROR_NONE){  //����
          GFL_STD_MemCopy(&pEvent[sizeof(gs_response)],pWork->pParent->selectPokeList.pokemonList,
                          DREAM_WORLD_SERVER_POKMEONLIST_MAX/8);
          pWork->pParent->selectType = GAMESYNC_RETURNMODE_BOXJUMP;
          _CHANGE_STATE(_modeFadeStart);
        }
        else{
          GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
          pWork->ErrorNo = pRep->ret_cd;
          _CHANGE_STATE(_ErrorDisp);
        }
      }
    }
  }
  else{
      pWork->pParent->selectType = GAMESYNC_RETURNMODE_BOXJUMP;
    _CHANGE_STATE(_modeFadeStart);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P������Ԍ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpPokemonListDownload(G_SYNC_WORK* pWork)
{

  if(GFL_NET_IsInit()){
    if(NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_POKEMONLIST, pWork->pNHTTPRap)){
      if(NHTTP_RAP_StartConnect(pWork->pNHTTPRap)){
        _CHANGE_STATE(_ghttpPokemonListDownload1);
      }
    }
  }
  else{
    if(GFL_UI_KEY_GetTrg()){
      _CHANGE_STATE(_ghttpPokemonListDownload1);
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�A�b�v���[�h����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _upeffectLoop8(G_SYNC_WORK* pWork)
{

  if(pWork->zzzCount%_ZZZCOUNT==0){
    PMSND_PlaySE(SEQ_SE_SYS_26);
  }
  pWork->zzzCount++;

  if(GFL_UI_KEY_GetTrg()){
    _CHANGE_STATE(_networkClose);
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�A�b�v���[�h����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _upeffectLoop7(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  _CHANGE_STATE(_upeffectLoop8);

}

//------------------------------------------------------------------------------
/**
 * @brief   ���銮��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _upeffectLoop6(G_SYNC_WORK* pWork)
{
  if(pWork->pTopAddr){
    GFL_HEAP_FreeMemory(pWork->pTopAddr);
    pWork->pTopAddr = NULL;
  }

  GSYNC_MESSAGE_NickNameMessageDisp( pWork->pMessageWork, GSYNC_007, pWork->pp );

  GSYNC_DISP_ObjInit(pWork->pDispWork, NANR_gsync_obj_zzz_ani);
  GSYNC_DISP_BlendSmokeStart(pWork->pDispWork,FALSE);
  GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_rug_ani1,NANR_gsync_obj_rug_ani3);
  PMSND_PlaySE(SEQ_SE_SYS_25);

  _CHANGE_STATE(_upeffectLoop7);
}



static void _updateSave2(G_SYNC_WORK* pWork)
{
  if(GAMEDATA_SaveAsyncMain(pWork->pGameData) == SAVE_RESULT_OK){
    _CHANGE_STATE(_upeffectLoop6);
  }
}



static void _updateSave(G_SYNC_WORK* pWork)
{
  //��M��������
  {
    DREAMWORLD_SAVEDATA* pDream = DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData);
    RTCDate date;
    GFL_RTC_GetDate( &date );
    DREAMWORLD_SV_SetTime(pDream , GFDATE_Set(date.year,date.month,date.day,date.week));
  }

  GAMEDATA_SaveAsyncStart(pWork->pGameData);
  
  _CHANGE_STATE(_updateSave2);
}






//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�A�b�v���[�h���� �Z�[�u����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _upeffectLoop5(G_SYNC_WORK* pWork)
{
  int percent;
  
  if(GFL_NET_IsInit()){
    percent = NHTTP_RAP_ProcessPercent(pWork->pNHTTPRap);

    GSYNC_DISP_SetPerfomance(pWork->pDispWork, percent);

    if(NHTTP_ERROR_NONE== NHTTP_RAP_Process(pWork->pNHTTPRap)){
      NET_PRINT("�I��\n");
      {
        u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        gs_response* pRep = (gs_response*)pEvent;
        int d,j;
        u32 size;
        NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);
#if DEBUG_ONLY_FOR_ohno   //�Z�[�u�f�[�^���グ������
        _CHANGE_STATE(_updateSave);
#else
        if(pRep->ret_cd == DREAM_WORLD_SERVER_ERROR_NONE){  //����
          _CHANGE_STATE(_updateSave);
        }
        else{
          GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
          pWork->ErrorNo = pRep->ret_cd;
          _CHANGE_STATE(_ErrorDisp);
        }
#endif
      }
    }
  }
  else{
    GSYNC_DISP_SetPerfomance(pWork->pDispWork,pWork->notNetEffect);
    pWork->notNetEffect++;
    if(pWork->notNetEffect==100){
      _CHANGE_STATE(_upeffectLoop6);
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�A�b�v���[�h��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _upeffectLoop4(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }

  if(GFL_NET_IsInit()){
    if(NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_UPLOAD, pWork->pNHTTPRap)){
      u32 size;
      u8* topAddr = (u8*)SaveControl_GetSaveWorkAdrs(pWork->pSaveData, &size);
      NHTTP_AddPostDataRaw(NHTTP_RAP_GetHandle(pWork->pNHTTPRap), topAddr, 0x80000 );
      GSYNC_DISP_SetPerfomance(pWork->pDispWork,0);
      if(NHTTP_RAP_StartConnect(pWork->pNHTTPRap)){
        _CHANGE_STATE(_upeffectLoop5);
      }
    }
  }
  else{
    GSYNC_DISP_SetPerfomance(pWork->pDispWork,0);
    _CHANGE_STATE(_upeffectLoop5);
  }

}

//------------------------------------------------------------------------------
/**
 * @brief   ����G�t�F�N�g��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _upeffectLoop3(G_SYNC_WORK* pWork)
{

  pWork->countTimer--;
  if(pWork->countTimer==0){
    GSYNC_DISP_DreamSmokeBgStart(pWork->pDispWork);

    GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_001);

    GSYNC_DISP_BlendSmokeStart(pWork->pDispWork,TRUE);
    GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_rug_ani1,NANR_gsync_obj_rug_ani2);

    PMSND_PlaySE(SEQ_SE_SYS_24);

    _CHANGE_STATE(_upeffectLoop4);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ����G�t�F�N�g��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _upeffectLoop2(G_SYNC_WORK* pWork)
{
  pWork->countTimer--;
  if(pWork->countTimer==0){
    GSYNC_DISP_ObjInit(pWork->pDispWork, NANR_gsync_obj_rug_ani1);
    pWork->countTimer = _DREAMSMOKE_TIME;
    _CHANGE_STATE(_upeffectLoop3);
  }

}


//------------------------------------------------------------------------------
/**
 * @brief   ����G�t�F�N�g��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _upeffectLoop1(G_SYNC_WORK* pWork)
{
  pWork->countTimer--;
  if(pWork->countTimer==0){
    GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_bed,NANR_gsync_obj_bed_ani);

    GSYNC_DISP_ObjInit(pWork->pDispWork,NANR_gsync_obj_kemuri_r);
    GSYNC_DISP_ObjInit(pWork->pDispWork,NANR_gsync_obj_kemuri_l);
    
    pWork->countTimer = _HAND_UP_TIME;
    _CHANGE_STATE(_upeffectLoop2);
  }
}





//------------------------------------------------------------------------------
/**
 * @brief   ����G�t�F�N�g��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _upeffectLoop(G_SYNC_WORK* pWork)
{
  pWork->countTimer--;
  if(pWork->countTimer==0){
    GSYNC_DISP_HandInit(pWork->pDispWork);
    GSYNC_DISP_PokemonIconMove(pWork->pDispWork);
    pWork->countTimer = _BEDIN_TIME;
    _CHANGE_STATE(_upeffectLoop1);
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ����G�t�F�N�g�J�n
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _upeffectStart(G_SYNC_WORK* pWork)
{
  GSYNC_DISP_PokemonIconCreate(pWork->pDispWork, PP_GetPPPPointer(pWork->pp),CLSYS_DRAW_SUB);
  pWork->countTimer=10;

  _CHANGE_STATE(_upeffectLoop);
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����𖰂�G���A�Ɉړ�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _BoxPokeMove(G_SYNC_WORK* pWork)
{
  POKEMON_PASO_PARAM* ppp;
  DREAMWORLD_SAVEDATA* pDream = DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData);


  if(pWork->pp){
    GFL_HEAP_FreeMemory(pWork->pp);
    pWork->pp=NULL;
  }

  ppp = BOXDAT_GetPokeDataAddress( pWork->pBox, pWork->trayno, pWork->indexno );
  GF_ASSERT(ppp);
  if(ppp){
    POKEMON_PARAM* pp = PP_CreateByPPP( ppp, pWork->heapID );
    DREAMWORLD_SV_SetSleepPokemon(pDream, pp);
    pWork->pp = pp;
    BOXDAT_ClearPokemon(pWork->pBox, pWork->trayno, pWork->indexno );
    DREAMWORLD_SV_SetSleepPokemonFlg(pDream,TRUE);
  }
  _CHANGE_STATE(_upeffectStart);

}

FS_EXTERN_OVERLAY(dpw_common);



//------------------------------------------------------------------------------
/**
 * @brief   PROC������
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT GSYNCProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_GSYNC_WORK* pParent = pwk;
  G_SYNC_WORK* pWork;
  s32 profileID;

  GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GAMESYNC, 0x88000 );
  pWork = GFL_PROC_AllocWork( proc, sizeof( G_SYNC_WORK ), HEAPID_GAMESYNC );
  GFL_STD_MemClear(pWork, sizeof(G_SYNC_WORK));

  pWork->heapID = HEAPID_GAMESYNC;

  GF_ASSERT(pParent);
  if(pParent){
    pWork->pParent = pParent;
    pWork->pGameData = pParent->gameData;
    pWork->pSaveData = GAMEDATA_GetSaveControlWork(pParent->gameData);
    profileID = MyStatus_GetProfileID( GAMEDATA_GetMyStatus(pParent->gameData) );
    GF_ASSERT(profileID);
    pWork->pNHTTPRap = NHTTP_RAP_Init(HEAPID_GAMESYNC, profileID, NULL);
    pWork->pBox = GAMEDATA_GetBoxManager(GAMESYSTEM_GetGameData(pParent->gsys));
    pWork->trayno = pParent->boxNo;
    pWork->indexno = pParent->boxIndex;
    switch(pParent->selectType){
    case GSYNC_CALLTYPE_INFO:      //�N����
      _CHANGE_STATE(_ghttpInfoWait0);
      break;
    case GSYNC_CALLTYPE_BOXSET:   //BOX�|�P�����I����
      _CHANGE_STATE(_BoxPokeMove);
      break;
    case GSYNC_CALLTYPE_POKELIST:   //�A�J�E���g�̌����㖰�点��|�P������I��
      _CHANGE_STATE(_ghttpInfoWait0);
      break;
    }
  }

  pWork->pDispWork = GSYNC_DISP_Init(pWork->heapID);
  pWork->pMessageWork = GSYNC_MESSAGE_Init(pWork->heapID, NARC_message_gsync_dat);


  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );

  PMSND_PlayBGM(SEQ_BGM_GAME_SYNC);

  
  return GFL_PROC_RES_FINISH;
}


static GFL_PROC_RESULT GSYNCProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  G_SYNC_WORK* pWork = mywk;
  StateFunc* state = pWork->state;
  GFL_PROC_RESULT ret = GFL_PROC_RES_FINISH;

  if( state ){
    state( pWork );
    ret = GFL_PROC_RES_CONTINUE;
  }
  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }

  GSYNC_DISP_Main(pWork->pDispWork);
  GSYNC_MESSAGE_Main(pWork->pMessageWork);

  return ret;
}

static GFL_PROC_RESULT GSYNCProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  G_SYNC_WORK* pWork = mywk;

  GSYNC_MESSAGE_End(pWork->pMessageWork);
  GSYNC_DISP_End(pWork->pDispWork);
  if(pWork->pp){
    GFL_HEAP_FreeMemory(pWork->pp);
  }
  if(pWork->pTopAddr){
    GFL_HEAP_FreeMemory(pWork->pTopAddr);
  }

  NHTTP_RAP_End(pWork->pNHTTPRap);

  
  GFL_PROC_FreeWork(proc);
 
  GFL_HEAP_DeleteHeap(HEAPID_GAMESYNC);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(dpw_common));
  return GFL_PROC_RES_FINISH;
}

// �v���Z�X��`�f�[�^
const GFL_PROC_DATA G_SYNC_ProcData = {
  GSYNCProc_Init,
  GSYNCProc_Main,
  GSYNCProc_End,
};

