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
#include "libdpw/dwci_ghttp.h"
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

#define _NET_DEBUG (1)

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
  BOX_MANAGER * pBox;
  SAVE_CONTROL_WORK* pSaveData;
  void* pTopAddr;
  u16 trayno;
  u16 indexno;
  StateFunc* state;      ///< �n���h���̃v���O�������
  vu32 count;
  int req;
  int getdataCount;
  int countTimer;
  BOOL bEnd;
};


static void _ghttpKeyWait(G_SYNC_WORK* pWork);



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


static void _networkClose1(G_SYNC_WORK* pWork)
{
  if(!GFL_NET_IsInit()){
    _CHANGE_STATE(NULL);
  }
}

static void _networkClose(G_SYNC_WORK* pWork)
{
  if(!GFL_NET_IsInit()){
    _CHANGE_STATE(NULL);
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

  GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_005);
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

static void _wakeupAction6(G_SYNC_WORK* pWork)
{
  if(GFL_NET_IsInit()){
    if(NHTTP_ERROR_NONE== NHTTP_RAP_Process(pWork->pNHTTPRap)){
      NET_PRINT("�I��\n");
      {
        EVENT_DATA* pEvent = (EVENT_DATA*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        

      }

      
      _CHANGE_STATE(_wakeupAction7);
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
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      _CHANGE_STATE(_wakeupAction4);
    }
    else{
      //@todo �ؒf������
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
        EVENT_DATA* pEvent = (EVENT_DATA*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        

      }
      _CHANGE_STATE(_wakeupAction1);
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
/**
 * @brief   �f�[�^�A�b�v���[�h����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _upeffectLoop8(G_SYNC_WORK* pWork)
{
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
  PMSND_PlaySE(SEQ_SE_SYS_26);
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
  }

  
  GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_002);
  GSYNC_DISP_ObjInit(pWork->pDispWork, NANR_gsync_obj_zzz_ani);
  GSYNC_DISP_BlendSmokeStart(pWork->pDispWork,FALSE);
    GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_rug_ani1,NANR_gsync_obj_rug_ani3);
  PMSND_PlaySE(SEQ_SE_SYS_25);

  _CHANGE_STATE(_upeffectLoop7);
}

static void _upeffectLoop5(G_SYNC_WORK* pWork)
{
  if(GFL_NET_IsInit()){
    if(NHTTP_ERROR_NONE== NHTTP_RAP_Process(pWork->pNHTTPRap)){
      NET_PRINT("�I��\n");
      {
        EVENT_DATA* pEvent = (EVENT_DATA*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        int d,j;
        u32 size;
        
        OS_TPrintf("%d \n",	pEvent->rom_code);
        OS_TPrintf("%d \n",	 pEvent->country_code);
        OS_TPrintf("%d \n",	pEvent->id);
        OS_TPrintf("%d \n",	pEvent->send_flag);
        OS_TPrintf("%d \n",	pEvent->dec_code);
        OS_TPrintf("%d \n",	 pEvent->cat_id);
        OS_TPrintf("%d \n",	 pEvent->present);
        OS_TPrintf("%d \n",	pEvent->status);

      }
      _CHANGE_STATE(_upeffectLoop6);
    }
  }
  else{
    _CHANGE_STATE(_upeffectLoop6);
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
  //test
    _CHANGE_STATE(_upeffectLoop6);
  return;

  if(GFL_NET_IsInit()){
    if(NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_UPLOAD, pWork->pNHTTPRap)){
      if(0){
        u32 size;
        u8* topAddr = (u8*)SaveControl_GetSaveWorkAdrs(pWork->pSaveData, &size);
        NHTTP_AddPostDataRaw(NHTTP_RAP_GetHandle(pWork->pNHTTPRap), topAddr, 0x80000 );
      }
      else{
        ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_GSYNC, pWork->heapID );
        u32 size;
        pWork->pTopAddr = GFL_ARCHDL_UTIL_LoadEx(p_handle,NARC_gsync_save3_bin,FALSE,pWork->heapID,&size );
        NHTTP_AddPostDataRaw(NHTTP_RAP_GetHandle(pWork->pNHTTPRap), pWork->pTopAddr, 0x80000 );
        GFL_ARC_CloseDataHandle(p_handle);
      }
      if(NHTTP_RAP_StartConnect(pWork->pNHTTPRap)){
        _CHANGE_STATE(_upeffectLoop5);
      }
    }
  }
  else{
    if(GFL_UI_KEY_GetTrg()){
      _CHANGE_STATE(_upeffectLoop5);
    }
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

//    test

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

  GF_ASSERT(DREAMWORLD_SV_GetSleepPokemonFlg(pDream)==FALSE);

  if(pWork->pp){
    GFL_HEAP_FreeMemory(pWork->pp);
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


#if PM_DEBUG
static  G_SYNC_WORK* _pWork;
#endif

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

  GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GAMESYNC, 0x78000 );
  pWork = GFL_PROC_AllocWork( proc, sizeof( G_SYNC_WORK ), HEAPID_GAMESYNC );
  GFL_STD_MemClear(pWork, sizeof(G_SYNC_WORK));

  pWork->heapID = HEAPID_GAMESYNC;

  if(pParent){
    pWork->pSaveData = GAMEDATA_GetSaveControlWork(pParent->gameData);
    pWork->pNHTTPRap = NHTTP_RAP_Init(HEAPID_GAMESYNC, SYSTEMDATA_GetDpwInfo(SaveData_GetSystemData(pWork->pSaveData)));
    pWork->pBox = GAMEDATA_GetBoxManager(GAMESYSTEM_GetGameData(pParent->gsys));
    pWork->trayno = pParent->boxNo;
    pWork->indexno = pParent->boxIndex;
    switch(pParent->selectType){
    case  GSYNC_CALLTYPE_INFO:
      _CHANGE_STATE(_ghttpInfoWait0);
      break;
    case GSYNC_CALLTYPE_BOXSET:
      _CHANGE_STATE(_BoxPokeMove);
      break;
    }
  }
  else{
#if PM_DEBUG
    pWork->pNHTTPRap = NHTTP_RAP_Init(HEAPID_GAMESYNC, 1234);
    _pWork=pWork;
    pWork->trayno=0;
    pWork->indexno=0;
    pWork->pSaveData = SaveControl_GetPointer();
    pWork->pBox = BOX_DAT_InitManager(pWork->heapID,SaveControl_GetPointer());

    {
      POKEMON_PARAM *pp;
      BOX_MANAGER* pBox = pWork->pBox;
      pp = PP_Create(MONSNO_MARIRU, 100, 123456, pWork->heapID);
      {
        u16 oyaName[5] = {L'�f',L'�o',L'�b',L'�O',0xFFFF};
        POKEMON_PERSONAL_DATA* ppd = POKE_PERSONAL_OpenHandle(MONSNO_MARIRU, 0, GFL_HEAPID_APP);
        u32 ret = POKE_PERSONAL_GetParam(ppd, POKEPER_ID_sex);
        PP_SetupEx(pp, MONSNO_MARIRU, 3, 123456,PTL_SETUP_POW_AUTO, ret);
        PP_Put( pp , ID_PARA_oyaname_raw , (u32)oyaName );

        BOXDAT_PutPokemonBox(pBox, 0, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pp));

        POKE_PERSONAL_CloseHandle(ppd);
      }
      GFL_HEAP_FreeMemory(pp);
    }
    _CHANGE_STATE(_BoxPokeMove);
#endif
  }

  pWork->pDispWork = GSYNC_DISP_Init(pWork->heapID);
  pWork->pMessageWork = GSYNC_MESSAGE_Init(pWork->heapID, NARC_message_gsync_dat);


  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );

  PMSND_PlayBGM(SEQ_BGM_GAME_SYNC);

  
  return GFL_PROC_RES_FINISH;
}

#if PM_DEBUG

static GFL_PROC_RESULT GSYNCProc_InitDbg( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GSYNCProc_Init( proc, seq, pwk,  mywk);
  {
    G_SYNC_WORK* pWork = _pWork;
    _BoxPokeMove(pWork);
    _CHANGE_STATE(_ghttpInfoWait0);
  }
  return GFL_PROC_RES_FINISH;
}

#endif


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

#if PM_DEBUG
  if(pwk==NULL){
    _pWork = NULL;
    BOX_DAT_ExitManager(pWork->pBox);
  }
#endif
  
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

#if PM_DEBUG
// �v���Z�X��`�f�[�^
const GFL_PROC_DATA G_SYNC_ProcData_Dbg = {
  GSYNCProc_InitDbg,
  GSYNCProc_Main,
  GSYNCProc_End,
};
#endif
