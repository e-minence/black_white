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

#include "savedata/save_tbl.h"
//#include "savedata/wifilist.h"
#include "savedata/system_data.h"
#include "savedata/gametime.h"
#include "savedata/dreamworld_data.h"
#include "savedata/symbol_save.h"
#include "savedata/c_gear_data.h"
#include "savedata/zukan_wp_savedata.h"
#include "savedata/musical_dist_save.h"

#include "msg/msg_d_ohno.h"
#include "gsync_local.h"
#include "gsync_download.h"
#include "net/nhttp_rap.h"
#include "../../field/event_gsync.h"
#include "gsync_obj_NANR_LBLDEFS.h"
#include "gsync_poke.cdat"
#include "msg/msg_gsync.h"
#include "gsync.naix"
#include "savedata/c_gear_picture.h"
#include "field/tpoke_data.h"

/*
��BGM��

SEQ_BGM_GAME_SYNC	�Q�[���V���NBGM

��SE��

SEQ_SE_SYS_24			�A�b�v�E�_�E�����[�h
SEQ_SE_SYS_25			�A�b�v���[�h������
SEQ_SE_SYS_26			�|�P��������
*/
#define _ZZZCOUNT (110)

#define _UPLOAD_PROCESS_PERCENT  (50)
#define _UPSAVE_PROCESS_PERCENT  (100)


#define _DOWNLOAD_PROCESS_PERCENT (30)
#define _DOWNLOAD_EXSAVE_PERCENT (60)
#define _DOWNLOAD_SAVE_PERCENT (100)


typedef void (StateFunc)(G_SYNC_WORK* pState);

#define _DOWNLOAD_ERROR  (GSYNC_ERR008-GSYNC_ERR001+1)

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


typedef enum{
  _DOWNLOAD_CGEAR,
  _DOWNLOAD_MUSICAL,
  _DOWNLOAD_ZUKAN,
} _DOWNLOAD_CONT_TYPE;



struct _G_SYNC_WORK {
  HEAPID heapID;
  DREAM_WORLD_SERVER_DOWNLOAD_FINISH_DATA aDownFinish;
  DREAM_WORLD_SERVER_STATUS_DATA aDreamStatus;
  GSYNC_DOWNLOAD_WORK* pDownload;
  POKEMON_PARAM* pp;
  NHTTP_RAP_WORK* pNHTTPRap;
  GSYNC_DISP_WORK* pDispWork;  // �`��n
  GSYNC_MESSAGE_WORK* pMessageWork; //���b�Z�[�W�n
  APP_TASKMENU_WORK* pAppTask;
  EVENT_GSYNC_WORK* pParent;
  BOX_MANAGER * pBox;
  SAVE_CONTROL_WORK* pSaveData;
  GAMEDATA* pGameData;
  MUSICAL_DIST_SAVE* pMusical;
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
  int lvup;
  int percent;
  DREAM_WORLD_SERVER_ERROR_TYPE ErrorNo;   ///�G���[���������ꍇ�̔ԍ�
  char tempbuffer[30];
  u8 musicalNo;      ///< web�őI�������ԍ�  �����ꍇ 0
  u8 cgearNo;        ///< web�őI�������ԍ�  �����ꍇ 0
  u8 zukanNo;        ///< web�őI�������ԍ�  �����ꍇ 0
  u8 downloadType;
  u8 msgBit;
  u8 dummy;
};


static void _ghttpKeyWait(G_SYNC_WORK* pWork);
static void _ghttpPokemonListDownload(G_SYNC_WORK* pWork);
static void _networkClose(G_SYNC_WORK* pWork);
static void _downloadcheck(G_SYNC_WORK* pWork);
static void _downloadcgearend(G_SYNC_WORK* pWork);


//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _IsNetworkMode(G_SYNC_WORK* pWork)
{
  if(GSYNC_CALLTYPE_BOXSET_NOACC == pWork->pParent->selectType){
    return FALSE;
  }
  if(GFL_NET_IsInit()){
    return TRUE;
  }
  return FALSE;
}



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
  GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
  
  GSYNC_MESSAGE_SetNormalMessage(pWork->pMessageWork,gmm);
  GSYNC_MESSAGE_SystemMessageDisp(pWork->pMessageWork);
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


static void _wakeupActio10(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  if(GFL_UI_KEY_GetTrg()){
    _CHANGE_STATE(_networkClose);
  }
}



static void _wakeupActio9(G_SYNC_WORK* pWork)
{
  int msgbuff[]={
    0,
    GSYNC_017,   //CG
    GSYNC_018,     //MU
    GSYNC_020,     //CGMU
    GSYNC_019,     //ZK
    GSYNC_022,  //zkcg
    GSYNC_021,
    GSYNC_016
    };
  
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  if(GFL_UI_KEY_GetTrg()){
    GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    GSYNC_MESSAGE_SetNormalMessage(pWork->pMessageWork, msgbuff[pWork->msgBit]);
    GSYNC_MESSAGE_SystemMessageDisp(pWork->pMessageWork);
    _CHANGE_STATE(_wakeupActio10);
  }
}


static void _wakeupActio8(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  if(GFL_UI_KEY_GetTrg()){
    GSYNC_MESSAGE_SystemMessageEnd(pWork->pMessageWork);
    GSYNC_MESSAGE_NickNameMessageDisp(pWork->pMessageWork,GSYNC_015, pWork->lvup, pWork->pp);
    GSYNC_MESSAGE_MessageDisp(pWork->pMessageWork);
    
    _CHANGE_STATE(_wakeupActio9);
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

    if(pWork->lvup!=0){  //���x���X�V
      int level = PP_Get( pp, ID_PARA_level, 0);
      level += pWork->lvup;
      if(level > 100){
        level = 100;
      }
      POKETOOL_MakeLevelRevise( pp, level);
    }
    
    BOXDAT_PutPokemon(pWork->pBox, PP_GetPPPPointerConst(pp));
    
    pWork->pp = pp;
    DREAMWORLD_SV_SetSleepPokemonFlg(pDream,FALSE);
  }


  GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
  GSYNC_MESSAGE_NickNameMessageDisp(pWork->pMessageWork,GSYNC_005,0, pWork->pp);
  GSYNC_MESSAGE_SystemMessageDisp(pWork->pMessageWork);

  GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_rug_ani3,NANR_gsync_obj_rug_ani4);

  GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_bed,NANR_gsync_obj_bed_ani);
  GSYNC_DISP_BlendSmokeStart(pWork->pDispWork,FALSE);
  
  
  GSYNC_DISP_PokemonIconCreate(pWork->pDispWork, PP_GetPPPPointer(pWork->pp),CLSYS_DRAW_MAIN);
  GSYNC_DISP_PokemonIconJump(pWork->pDispWork);

  PMSND_PlaySE(SEQ_SE_SYS_25);

  _CHANGE_STATE(_wakeupActio8);
}


static void _wakeupAction71(G_SYNC_WORK* pWork)
{

  if(GFL_NET_IsInit()){
    if(NHTTP_ERROR_NONE== NHTTP_RAP_Process(pWork->pNHTTPRap)){
      NET_PRINT("�I��\n");
      {
        u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        DREAMWORLD_SAVEDATA* pDreamSave = DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData);
        gs_response* pRep = (gs_response*)pEvent;
        NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);
        _CHANGE_STATE(_wakeupAction7);
      }
    }
  }
  else{
    _CHANGE_STATE(_wakeupAction7);
  }


}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P������Ԍ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wakeupAction_save2(G_SYNC_WORK* pWork)
{
  if(pWork->percent < _DOWNLOAD_SAVE_PERCENT){
    pWork->percent++;
  }
  GSYNC_DISP_SetPerfomance(pWork->pDispWork, pWork->percent);

  if(GAMEDATA_SaveAsyncMain(pWork->pGameData) != SAVE_RESULT_OK){
    return;
  }


  
  if(GFL_NET_IsInit()){
    if(NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_DOWNLOAD_FINISH, pWork->pNHTTPRap)){
      pWork->aDownFinish.bGet=TRUE;
      pWork->aDownFinish.dummy=0;
      NHTTP_AddPostDataRaw(NHTTP_RAP_GetHandle(pWork->pNHTTPRap), &pWork->aDownFinish, sizeof(NHTTPRAP_URL_DOWNLOAD_FINISH) );
      if(NHTTP_RAP_StartConnect(pWork->pNHTTPRap)){
        _CHANGE_STATE(_wakeupAction71);
      }
    }
  }
  else{
    if(GFL_UI_KEY_GetTrg()){
      _CHANGE_STATE(_wakeupAction7);
    }
  }
}

//�e�X�g 

static void _wakeupAction_1(G_SYNC_WORK* pWork)
{

  GAMEDATA_SaveAsyncStart(pWork->pGameData);

  
  _CHANGE_STATE(_wakeupAction_save2);
}

//------------------------------------------------------------------

static void _exsaveEnd(G_SYNC_WORK* pWork)
{
  if(GSYNC_DOWNLOAD_ExitAsync(pWork->pDownload)){
    _CHANGE_STATE(_downloadcgearend);
  }
  else{
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
  }
}


static void _cgearsave(G_SYNC_WORK* pWork)
{
  {
    SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(pWork->pGameData);
    u8* pCGearWork = GFL_HEAP_AllocMemory(pWork->heapID,SAVESIZE_EXTRA_CGEAR_PICTURE);
    CGEAR_PICTURE_SAVEDATA* pPic = (CGEAR_PICTURE_SAVEDATA*)pCGearWork;
    u16* pCRC = GSYNC_DOWNLOAD_GetData(pWork->pDownload);
    int size = CGEAR_PICTURTE_CHAR_SIZE+CGEAR_PICTURTE_PAL_SIZE+CGEAR_PICTURTE_SCR_SIZE;
    int sizeh = size/2;
    u16 crc;
    
    //crc���� pCRC
    crc = GFL_STD_CrcCalc( pCRC, size );
    OS_TPrintf("crc%x crc%x",crc,pCRC[sizeh]);
    GF_ASSERT(crc == pCRC[sizeh]);

    SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE, pWork->heapID,
                               pCGearWork,SAVESIZE_EXTRA_CGEAR_PICTURE);

    GFL_STD_MemCopy(pCRC, pPic->picture, size);
    
    SaveControl_Extra_SaveAsyncInit(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE);
    while(1){
      if(SAVE_RESULT_OK==SaveControl_Extra_SaveAsyncMain(pSave,SAVE_EXTRA_ID_CGEAR_PICUTRE)){
        break;
      }
      OS_WaitIrq(TRUE, OS_IE_V_BLANK);
    }
    SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_CGEAR_PICUTRE);
    pWork->cgearNo=DREAM_WORLD_NOPICTURE;

    CGEAR_SV_SetCGearPictureONOFF(CGEAR_SV_GetCGearSaveData(pSave),TRUE);  //CGEAR�f�J�[���L��
    CGEAR_SV_SetCGearPictureCRC(CGEAR_SV_GetCGearSaveData(pSave),crc);  //CGEAR�f�J�[���L��
    GFL_HEAP_FreeMemory(pCGearWork);
  }
}

static void _zukansave(G_SYNC_WORK* pWork)
{

  {
    SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(pWork->pGameData);
    u8* pZknWork = GFL_HEAP_AllocMemory(pWork->heapID, SAVESIZE_EXTRA_ZUKAN_WALLPAPER);
    u16* pCRC = GSYNC_DOWNLOAD_GetData(pWork->pDownload);
    int size = ZUKANWP_SAVEDATA_RESOURCE_SIZE;
    int sizeh = size/2;
    u16 crc;
    
    //crc���� pCRC
    crc = GFL_STD_CrcCalc( pCRC, size );
    OS_TPrintf("crc%x crc%x",crc,pCRC[sizeh]);
    GF_ASSERT(crc == pCRC[sizeh]);

    SaveControl_Extra_LoadWork(pSave, SAVE_EXTRA_ID_ZUKAN_WALLPAPER, pWork->heapID,
                               pZknWork,SAVESIZE_EXTRA_ZUKAN_WALLPAPER);

    GFL_STD_MemCopy(pCRC, pZknWork, size);
    ZUKANWP_SAVEDATA_SetDataCheckFlag( (ZUKANWP_SAVEDATA *) pZknWork,TRUE );
    
    SaveControl_Extra_SaveAsyncInit(pSave, SAVE_EXTRA_ID_ZUKAN_WALLPAPER);
    while(1){
      if(SAVE_RESULT_OK==SaveControl_Extra_SaveAsyncMain(pSave,SAVE_EXTRA_ID_ZUKAN_WALLPAPER)){
        break;
      }
      OS_WaitIrq(TRUE, OS_IE_V_BLANK);
    }
    SaveControl_Extra_UnloadWork(pSave, SAVE_EXTRA_ID_ZUKAN_WALLPAPER);
    pWork->zukanNo=DREAM_WORLD_NOPICTURE;

    GFL_HEAP_FreeMemory(pZknWork);
  }


}


static void _musicalsaveMain(G_SYNC_WORK* pWork)
{

  if( MUSICAL_DIST_SAVE_SaveMusicalArchive_Main( pWork->pMusical )){
    pWork->pMusical=NULL;
    _CHANGE_STATE(_exsaveEnd);
  }
} 

static void _musicalsave(G_SYNC_WORK* pWork,int size)
{
  pWork->musicalNo=DREAM_WORLD_NOPICTURE;

  {
    SAVE_CONTROL_WORK* pSave = GAMEDATA_GetSaveControlWork(pWork->pGameData);

    u16* pCRC = GSYNC_DOWNLOAD_GetData(pWork->pDownload);
    int sizeh = size/2;
    u16 crc;
    
    //crc���� pCRC
    crc = GFL_STD_CrcCalc( pCRC, size );
    OS_TPrintf("crc%x crc%x",crc,pCRC[sizeh]);
    GF_ASSERT(crc == pCRC[sizeh]);

    //�~���[�W�J���Z�[�u
    pWork->pMusical = MUSICAL_DIST_SAVE_SaveMusicalArchive_Init( pSave , pCRC, size, pWork->heapID);
    _CHANGE_STATE(_musicalsaveMain);
  }
}


static void _downloadcgearend(G_SYNC_WORK* pWork)
{
  if(!GSYNC_DOWNLOAD_ResultEnd(pWork->pDownload)){
    return;
  }
  if(GSYNC_DOWNLOAD_ResultError(pWork->pDownload)){
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
  }
  GSYNC_DOWNLOAD_Exit(pWork->pDownload);
  pWork->pDownload=NULL;

  if(pWork->percent < 100){
    pWork->percent+=10;
  }
  GSYNC_DISP_SetPerfomance(pWork->pDispWork, pWork->percent);

  _CHANGE_STATE(_downloadcheck);
}

static void _downloadcgear6(G_SYNC_WORK* pWork)
{
  if(!GSYNC_DOWNLOAD_ResultEnd(pWork->pDownload)){
    return;
  }
  if(GSYNC_DOWNLOAD_ResultError(pWork->pDownload)){
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
  }
  else{
    switch(pWork->downloadType){
    case _DOWNLOAD_CGEAR:
      _cgearsave(pWork);
      _CHANGE_STATE(_exsaveEnd);
      break;
    case _DOWNLOAD_MUSICAL:
      _musicalsave(pWork,GSYNC_DOWNLOAD_GetSize(pWork->pDownload)-2);
      break;
    case _DOWNLOAD_ZUKAN:
      _zukansave(pWork);
      _CHANGE_STATE(_exsaveEnd);
      break;
    }
  }
}



static void _downloadcgear5(G_SYNC_WORK* pWork)
{
  if(!GSYNC_DOWNLOAD_ResultEnd(pWork->pDownload)){
    return;
  }
  if(GSYNC_DOWNLOAD_ResultError(pWork->pDownload)){
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
  }
  else if(GSYNC_DOWNLOAD_FileAsync(pWork->pDownload)){
    _CHANGE_STATE(_downloadcgear6);
  }
  else{
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
  }
}


static void _downloadcgear4(G_SYNC_WORK* pWork)
{
  if(!GSYNC_DOWNLOAD_ResultEnd(pWork->pDownload)){
    return;
  }
  if(GSYNC_DOWNLOAD_ResultError(pWork->pDownload)){
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
  }
  else if(GSYNC_DOWNLOAD_FileListAsync(pWork->pDownload)){
    _CHANGE_STATE(_downloadcgear5);
  }
  else{
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
  }
}

static void _downloadcgear3(G_SYNC_WORK* pWork)
{
  if(GSYNC_DOWNLOAD_FileListNumAsync(pWork->pDownload)){
    _CHANGE_STATE(_downloadcgear4);
  }
  else{
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
  }
}


static void _downloadcgear2(G_SYNC_WORK* pWork)
{
  if(!GSYNC_DOWNLOAD_ResultEnd(pWork->pDownload)){
    return;
  }
  if(GSYNC_DOWNLOAD_ResultError(pWork->pDownload)){
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
    return;
  }

  switch(pWork->downloadType){
  case _DOWNLOAD_CGEAR:
    if(GSYNC_DOWNLOAD_SetAttr(pWork->pDownload, "CGEAR", pWork->cgearNo)){
      _CHANGE_STATE(_downloadcgear3);
    }
    else{
      pWork->ErrorNo = _DOWNLOAD_ERROR;
      _CHANGE_STATE(_ErrorDisp);
    }
    break;
  case _DOWNLOAD_MUSICAL:
    if(GSYNC_DOWNLOAD_SetAttr(pWork->pDownload, "MUSICAL", pWork->musicalNo)){
      _CHANGE_STATE(_downloadcgear3);
    }
    else{
      pWork->ErrorNo = _DOWNLOAD_ERROR;
      _CHANGE_STATE(_ErrorDisp);
    }
    break;
  case _DOWNLOAD_ZUKAN:
    if(GSYNC_DOWNLOAD_SetAttr(pWork->pDownload, "ZUKAN", pWork->zukanNo)){
      _CHANGE_STATE(_downloadcgear3);
    }
    else{
      pWork->ErrorNo = _DOWNLOAD_ERROR;
      _CHANGE_STATE(_ErrorDisp);
    }
    break;
  }
}


// �_�E�����[�hCGEAR
static void _downloadcgear(G_SYNC_WORK* pWork)
{
  switch(pWork->downloadType){
  case _DOWNLOAD_CGEAR:
    pWork->pDownload = GSYNC_DOWNLOAD_Create(pWork->heapID, CGEAR_PICTURE_SAVE_GetWorkSize() + 2); //CRC
    break;
  case _DOWNLOAD_MUSICAL:
    pWork->pDownload = GSYNC_DOWNLOAD_Create(pWork->heapID, 128*1024 ); //CRC���݂�128K
    break;
  case _DOWNLOAD_ZUKAN:
    pWork->pDownload = GSYNC_DOWNLOAD_Create(pWork->heapID, ZUKANWP_SAVEDATA_GetWorkSize() + 2); //CRC
    break;
  }

  if(GSYNC_DOWNLOAD_InitAsync(pWork->pDownload)){
    _CHANGE_STATE(_downloadcgear2);
  }
  else{
    pWork->ErrorNo = _DOWNLOAD_ERROR;
    _CHANGE_STATE(_ErrorDisp);
  }
}


static void _downloadcheck(G_SYNC_WORK* pWork)
{
  if(pWork->cgearNo != DREAM_WORLD_NOPICTURE){
    pWork->downloadType=_DOWNLOAD_CGEAR;
    _CHANGE_STATE(_downloadcgear);
    return;
  }
  if(pWork->musicalNo != DREAM_WORLD_NOPICTURE){
    pWork->downloadType=_DOWNLOAD_MUSICAL;
    _CHANGE_STATE(_downloadcgear);
    return;
  }
  if(pWork->zukanNo != DREAM_WORLD_NOPICTURE){
    pWork->downloadType=_DOWNLOAD_ZUKAN;
    _CHANGE_STATE(_downloadcgear);
    return;
  }
  _CHANGE_STATE(_wakeupAction_1);  //�N���鉉�o��
}


//------------------------------------------------------------------

//   �Ƌ���Z�[�u�G���A�Ɉړ�
static void _furnitureInSaveArea(DREAMWORLD_SAVEDATA* pDreamSave,DREAM_WORLD_SERVER_DOWNLOAD_DATA* pDream)
{
  int i;

  //�����X�V
  for(i=0;i<DREAM_WORLD_DATA_MAX_FURNITURE;i++){
    DREAMWORLD_SV_SetFurnitureData(pDreamSave, i,   &pDream->Furniture[i]);
  }
  DREAMWORLD_SV_SetIsSyncFurniture(pDreamSave, TRUE);

  
}


//   �A�C�e�����Z�[�u�G���A�Ɉړ�
static void _itemInSaveArea(DREAMWORLD_SAVEDATA* pDreamSave,DREAM_WORLD_SERVER_DOWNLOAD_DATA* pDream)
{
  int i;
  
  for(i=0;i<DREAM_WORLD_DATA_MAX_ITEMBOX;i++){
    DREAMWORLD_SV_SetItem(pDreamSave, i,   pDream->itemID[i], pDream->itemNum[i]);
    NET_PRINT("DREAMWORLD_SV_SetItem %d %d %d\n",i,pDream->itemID[i], pDream->itemNum[i]);
  }
}

static void _datacheck(G_SYNC_WORK* pWork, DREAMWORLD_SAVEDATA* pDreamSave,DREAM_WORLD_SERVER_DOWNLOAD_DATA* pDream,gs_response* pRep )
{
  if(pRep->ret_cd != DREAM_WORLD_SERVER_ERROR_NONE){
    pWork->ErrorNo = pRep->ret_cd;
    _CHANGE_STATE(_ErrorDisp);
    return;
  }
  if(!pDream->bGet && (DREAMWORLD_SV_GetUploadCount(pDreamSave) != pDream->uploadCount))
  {
    DREAMWORLD_SV_SetUploadCount(pDreamSave, pDream->uploadCount);
    pWork->musicalNo = pDream->musicalNo;      ///< web�őI�������ԍ�  �����ꍇ 0
    pWork->cgearNo = pDream->cgearNo; //  u8 cgearNo;        ///< web�őI�������ԍ�  �����ꍇ 0
    pWork->zukanNo = pDream->zukanNo; //  u8 zukanNo;        ///< web�őI�������ԍ�  �����ꍇ 0
    pWork->lvup = pDream->sleepPokemonLv; //�Q�Ă��|�P�����̃��x���A�b�v�l

    if(pWork->cgearNo != DREAM_WORLD_NOPICTURE){
      pWork->msgBit = pWork->msgBit | 0x01;
    }
    if(pWork->musicalNo != DREAM_WORLD_NOPICTURE){
      pWork->msgBit = pWork->msgBit | 0x02;
    }
    if(pWork->zukanNo != DREAM_WORLD_NOPICTURE){
      pWork->msgBit = pWork->msgBit | 0x04;
    }


/*
    {// �f�[�^�ǂݍ��݁E�j��
       TPOKE_DATA* pTP =TPOKE_DATA_Create( pWork->heapID );
      int size = SYMBOL_ZONE_TYPE_FREE_SMALL;
      if(TPOKE_DATA_IsSizeBig( pTP, pDream->findPokemon, pDream->findPokemonSex,
                               pDream->findPokemonForm )){
        size = SYMBOL_ZONE_TYPE_FREE_LARGE;
      }
      //�|�P�����V���{���G���J�E���g�i�[
      SymbolSave_SetFreeZone(SymbolSave_GetSymbolData(pWork->pSaveData), pDream->findPokemon,
                             pDream->findPokemonTecnique, pDream->findPokemonSex, pDream->findPokemonForm,
                             size);
      TPOKE_DATA_Delete( pTP );
    }
   */
      
    //�T�C���C��
    DREAMWORLD_SV_SetSignin(pDreamSave,pDream->signin);
    // �Ƌ�
    _furnitureInSaveArea(pDreamSave, pDream);
    //�A�C�e��
    _itemInSaveArea(pDreamSave, pDream);
    //          _CHANGE_STATE();  //�Z�[�u�ɍs��?
  }
  else{
    NET_PRINT("�擾���Ȃ�����%d %d=%d\n",pDream->bGet, pDream->uploadCount,DREAMWORLD_SV_GetUploadCount(pDreamSave));
  }
  
  _downloadcheck(pWork);
}

//------------------------------------------------------------------------------
/**
 * @brief   ���x���P����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _lv1check(G_SYNC_WORK* pWork)
{
  int i;
  DREAMWORLD_SAVEDATA* pDreamSave = DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData);

  //�|�P�����V���{���G���J�E���g
  SymbolSave_SetFreeZone(SymbolSave_GetSymbolData(pWork->pSaveData), pWork->aDreamStatus.findPokemon,
                         pWork->aDreamStatus.findPokemonTecnique,
                         pWork->aDreamStatus.findPokemonSex,
                         pWork->aDreamStatus.findPokemonForm,
                         SYMBOL_ZONE_TYPE_FREE_SMALL);  //@todo ��check �|�P����NO����SMALL,LARGE���ʂ�������


  for(i=0;i<DREAM_WORLD_DATA_MAX_ITEMBOX;i++){
    DREAMWORLD_SV_SetItem(pDreamSave, i, pWork->aDreamStatus.itemID[i], pWork->aDreamStatus.itemNum[i]);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P������Ԍ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wakeupAction6(G_SYNC_WORK* pWork)
{
  if(_IsNetworkMode(pWork)){

    if(pWork->percent < _DOWNLOAD_PROCESS_PERCENT){
      pWork->percent++;
    }
    GSYNC_DISP_SetPerfomance(pWork->pDispWork, pWork->percent);

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
    if(pWork->percent < _DOWNLOAD_PROCESS_PERCENT){
      pWork->percent++;
    }
    else{

      _lv1check(pWork);
      
      _CHANGE_STATE(_wakeupAction7);
    }
    GSYNC_DISP_SetPerfomance(pWork->pDispWork, pWork->percent);
  }
}

static void _wakeupAction5(G_SYNC_WORK* pWork)
{
  if(_IsNetworkMode(pWork)){
    if(NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_DOWNLOAD, pWork->pNHTTPRap)){
      if(NHTTP_RAP_StartConnect(pWork->pNHTTPRap)){
        _CHANGE_STATE(_wakeupAction6);
      }
    }
  }
  else{
    _CHANGE_STATE(_wakeupAction6);
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




static void _wakeupActionFailed1(G_SYNC_WORK* pWork)
{
 if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  _CHANGE_STATE(_networkClose);
}





static void _wakeupActionFailed(G_SYNC_WORK* pWork)
{
 if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_009);
  _CHANGE_STATE(_wakeupActionFailed1);
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


static void _wakeupAction12(G_SYNC_WORK* pWork)
{
  GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);

  GSYNC_DISP_ObjInit(pWork->pDispWork, NANR_gsync_obj_rug_ani3);
  GSYNC_DISP_ObjInit(pWork->pDispWork, NANR_gsync_obj_zzz_ani);

  GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_003);
  PMSND_PlaySE(SEQ_SE_SYS_26);

  _CHANGE_STATE(_wakeupAction2);

}



static void _itemcheck2(G_SYNC_WORK* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      _CHANGE_STATE(_wakeupAction12);
    }
    else{
      _CHANGE_STATE(_networkClose);
    }
    GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
  }
}



static void _itemcheck(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }

  pWork->pAppTask = GSYNC_MESSAGE_YesNoStart(pWork->pMessageWork,GSYNC_YESNOTYPE_INFO);

  _CHANGE_STATE(_itemcheck2);

}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����N��������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wakeupAction1(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }

  {
    DREAMWORLD_SAVEDATA* pDreamSave = DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData);
    if( DREAMWORLD_SV_GetItemRestNum(pDreamSave) ){

      GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_014);

      _CHANGE_STATE(_itemcheck);

    }
    else{
      _CHANGE_STATE(_wakeupAction12);
    }
  }
  
}



//------------------------------------------------------------------------------
/**
 * @brief   �|�P�������A�J�E���g�쐬
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _createAccount2(G_SYNC_WORK* pWork)
{
  if(GFL_NET_IsInit()){
    if(NHTTP_ERROR_NONE== NHTTP_RAP_Process(pWork->pNHTTPRap)){
      {
        gs_response* pEvent = (gs_response*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);
        if(pEvent->ret_cd==DREAM_WORLD_SERVER_ALREADY_EXISTS){ //�A�J�E���g�͂��łɂ���
           _CHANGE_STATE(_ghttpPokemonListDownload);
        }
        else if(pEvent->ret_cd==DREAM_WORLD_SERVER_ERROR_NONE){  //�A�J�E���g�쐬����
          DREAMWORLD_SV_SetAccount(DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData),TRUE);
           _CHANGE_STATE(_ghttpPokemonListDownload);
        }
        else{
          pWork->ErrorNo = pEvent->ret_cd;
          _CHANGE_STATE(_ErrorDisp);
        }
      }
    }
  }
  else{
    _CHANGE_STATE(_ghttpPokemonListDownload);
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �|�P�������A�J�E���g�쐬
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _createAccount(G_SYNC_WORK* pWork)
{

  if(GFL_NET_IsInit()){
    if(NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_ACCOUNT_CREATE, pWork->pNHTTPRap)){
      s32 proid = MyStatus_GetProfileID( GAMEDATA_GetMyStatus(pWork->pGameData) );

      GFL_STD_MemClear(pWork->tempbuffer, sizeof(pWork->tempbuffer));
      STD_TSNPrintf(pWork->tempbuffer, sizeof(pWork->tempbuffer), "%d\0\0\0\0\0\0\0\0\0\0\0\0", proid);

      OS_TPrintf("NHTTP_AddPostDataRaw byte %d %d %s\n",proid,STD_StrLen(pWork->tempbuffer),pWork->tempbuffer);
      NHTTP_AddPostDataRaw( NHTTP_RAP_GetHandle(pWork->pNHTTPRap),
                            pWork->tempbuffer, 12 );

      if(NHTTP_RAP_StartConnect(pWork->pNHTTPRap)){
        _CHANGE_STATE(_createAccount2);
      }
    }
  }
  else{
    if(GFL_UI_KEY_GetTrg()){
      _CHANGE_STATE(_createAccount2);
    }
  }
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
  case DREAM_WORLD_SERVER_NO_DATA:   //�T�[�o�[�Ƀf�[�^�������ꍇ
    switch(pWork->pParent->selectType){
    case GSYNC_CALLTYPE_POKELIST:          //�Z�[�u�f�[�^��ł͖��点��|�P������I��
      _CHANGE_STATE(_ghttpPokemonListDownload);
      break;
    default:
      pWork->ErrorNo = pRep->ret_cd;
      _CHANGE_STATE(_ErrorDisp);
      break;
    }
    break;
  case DREAM_WORLD_SERVER_NO_ACCOUNT:   //�A�J�E���g���o�^�̏ꍇ
    switch(pWork->pParent->selectType){
    case GSYNC_CALLTYPE_INFO:
      _CHANGE_STATE(_wakeupAction1);   //�N��������
      break;
    case GSYNC_CALLTYPE_POKELIST:          //���点��|�P������I��
      //����Ȃ炠�肤��
      _CHANGE_STATE(_createAccount);  //�A�J�E���g�����ɕύX
//      _CHANGE_STATE(_ghttpPokemonListDownload);
      break;
    }
    break;
  default: //���ʂ̃G���[����
    pWork->ErrorNo = pRep->ret_cd;
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
        GFL_STD_MemCopy(pDream, &pWork->aDreamStatus, sizeof(DREAM_WORLD_SERVER_STATUS_DATA));
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
/**
 * @brief   �{�b�N�X������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _BoxNullMsg2(G_SYNC_WORK* pWork)
{
  if(!GSYNC_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL)){
    pWork->pParent->selectType = GAMESYNC_RETURNMODE_EXIT;
    GSYNC_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    _CHANGE_STATE(_modeFadeStart);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �{�b�N�X������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _BoxNullMsg(G_SYNC_WORK* pWork)
{
  GSYNC_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GSYNC_012);
  _CHANGE_STATE(_BoxNullMsg2);
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

        if((pRep->ret_cd == DREAM_WORLD_SERVER_ERROR_NONE) || (DREAM_WORLD_SERVER_NO_ACCOUNT ==pRep->ret_cd)){  //����
          GFL_STD_MemCopy(&pEvent[sizeof(gs_response)],pWork->pParent->selectPokeList.pokemonList,
                          DREAM_WORLD_SERVER_POKMEONLIST_MAX/8);
          pWork->pParent->selectType = GAMESYNC_RETURNMODE_BOXJUMP;
          _CHANGE_STATE(_modeFadeStart);
        }
        else if (DREAM_WORLD_SERVER_NO_ACCOUNT ==pRep->ret_cd){
          
          GFL_STD_MemCopy(&pEvent[sizeof(gs_response)],pWork->pParent->selectPokeList.pokemonList,
                          DREAM_WORLD_SERVER_POKMEONLIST_MAX/8);
          pWork->pParent->selectType = GAMESYNC_RETURNMODE_BOXJUMP_NOACC;
          _CHANGE_STATE(_modeFadeStart);

        }
        else{
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
 * @brief   �|�P�����̑I�����_�E�����[�h
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpPokemonListDownload(G_SYNC_WORK* pWork)
{
  GMTIME* pGMT = SaveData_GetGameTime(pWork->pSaveData);
  if(GMTIME_IsPenaltyMode(pGMT)){  //�y�i���e�B�[���͖��鎖���ł��Ȃ�
    _CHANGE_STATE(_wakeupActionFailed);
    return;
  }
  else{          //���Ԃ��m�F����
    GFDATE gd = DREAMWORLD_SV_GetTime(DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData));
    RTCDate date;
    GFDATE_GFDate2RTCDate(gd, &date);
    
    if(GFL_RTC_GetDaysOffset(&date) > 0){
    }
    else{
      _CHANGE_STATE(_wakeupActionFailed);
      return;
    }
  }
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
  pWork->percent = _UPSAVE_PROCESS_PERCENT;
  GSYNC_DISP_SetPerfomance(pWork->pDispWork, pWork->percent);

  GSYNC_MESSAGE_NickNameMessageDisp( pWork->pMessageWork, GSYNC_007,0, pWork->pp );
  GSYNC_MESSAGE_MessageDisp(pWork->pMessageWork);
  
  GSYNC_DISP_ObjInit(pWork->pDispWork, NANR_gsync_obj_zzz_ani);
  GSYNC_DISP_BlendSmokeStart(pWork->pDispWork,FALSE);
  GSYNC_DISP_ObjChange(pWork->pDispWork,NANR_gsync_obj_rug_ani1,NANR_gsync_obj_rug_ani3);
  PMSND_PlaySE(SEQ_SE_SYS_25);

  _CHANGE_STATE(_upeffectLoop7);
}



static void _updateSave2(G_SYNC_WORK* pWork)
{
  if(pWork->percent < _UPSAVE_PROCESS_PERCENT){
    pWork->percent++;
  }
  GSYNC_DISP_SetPerfomance(pWork->pDispWork, pWork->percent);

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
  pWork->percent=_UPLOAD_PROCESS_PERCENT;

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
  
  if(_IsNetworkMode(pWork)){
    if(pWork->percent < _UPLOAD_PROCESS_PERCENT){
      pWork->percent++;
    }

    GSYNC_DISP_SetPerfomance(pWork->pDispWork, pWork->percent);

    if(NHTTP_ERROR_NONE== NHTTP_RAP_Process(pWork->pNHTTPRap)){
      NET_PRINT("�I��\n");
      {
        u8* pEvent = (u8*)NHTTP_RAP_GetRecvBuffer(pWork->pNHTTPRap);
        gs_response* pRep = (gs_response*)pEvent;
        int d,j;
        u32 size;
        NHTTP_DEBUG_GPF_HEADER_PRINT((gs_response*)pEvent);
        if(pRep->ret_cd == DREAM_WORLD_SERVER_ERROR_NONE){  //����
          _CHANGE_STATE(_updateSave);
        }
        else{
          pWork->ErrorNo = pRep->ret_cd;
          _CHANGE_STATE(_ErrorDisp);
        }
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

  if(_IsNetworkMode(pWork)){
    if(NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_UPLOAD, pWork->pNHTTPRap)){
      u32 size;
      u8* topAddr = (u8*)SaveControl_GetSaveWorkAdrs(pWork->pSaveData, &size);
      NHTTP_AddPostDataRaw(NHTTP_RAP_GetHandle(pWork->pNHTTPRap), topAddr, 0x80000 );
      GSYNC_DISP_SetPerfomance(pWork->pDispWork,0);
      pWork->percent=0;
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
    pWork->pNHTTPRap = NHTTP_RAP_Init(HEAPID_GAMESYNC, profileID, &pParent->aSVL);
    pWork->pBox = GAMEDATA_GetBoxManager(GAMESYSTEM_GetGameData(pParent->gsys));
    pWork->trayno = pParent->boxNo;
    pWork->indexno = pParent->boxIndex;
    switch(pParent->selectType){
    case GSYNC_CALLTYPE_INFO:      //�N����
      GF_ASSERT(profileID);
      _CHANGE_STATE(_ghttpInfoWait0);
      break;
    case GSYNC_CALLTYPE_BOXSET:   //BOX�|�P�����I����
      GF_ASSERT(profileID);
      _CHANGE_STATE(_BoxPokeMove);
      break;
    case GSYNC_CALLTYPE_BOXSET_NOACC:   //BOX�|�P�����I���� �A�J�E���g�Ȃ�
      GF_ASSERT(profileID);
      _CHANGE_STATE(_BoxPokeMove);
      break;
    case GSYNC_CALLTYPE_POKELIST:   //�A�J�E���g�̌����㖰�点��|�P������I��
      GF_ASSERT(profileID);
      _CHANGE_STATE(_ghttpInfoWait0);
      break;
    case GSYNC_CALLTYPE_BOXNULL:  //�|�P���������Ȃ�
      _CHANGE_STATE(_BoxNullMsg);
      break;
    }
  }

  pWork->pDispWork = GSYNC_DISP_Init(pWork->heapID);
  pWork->pMessageWork = GSYNC_MESSAGE_Init(pWork->heapID, NARC_message_gsync_dat);


  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );

  //PMSND_PlayBGM(SEQ_BGM_GAME_SYNC);
  GFL_NET_ReloadIcon();

  
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
  if(pWork->pDownload){
    GSYNC_DOWNLOAD_Main(pWork->pDownload);
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

