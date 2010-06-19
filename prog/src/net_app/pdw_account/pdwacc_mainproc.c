//============================================================================================
/**
 * @file    pdwacc_mainproc.c
 * @brief   PROC�F�A�J�E���g�쐬
 * @author  k.ohno
 * @date    2009.12.08
 */
//============================================================================================


#include <gflib.h>
#include "net/network_define.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "savedata/dreamworld_data.h"
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "system/main.h"      //GFL_HEAPID_APP�Q��
#include "title/title.h"

#include "net_app/pdwacc.h"


typedef struct {
  WIFILOGIN_PARAM     login;
  WIFILOGOUT_PARAM   logout;
  GAMEDATA      *gameData;
  PDWACC_PROCWORK pwdaccWork;
  DWCSvlResult aSVL;
  HEAPID heapID;
  int state;
} PDWACCMAIN_WORK;




typedef enum{
  _WIFI_LOGIN,
  _WIFI_ACCOUNT,
  _WIFI_ERROR,
  _WIFI_LOGOUT,
  _WIFI_END,
} _WIFI_STATE_LABEL;



//------------------------------------------------------------------------------
/**
 * @brief   PROC������
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT PDWACCProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
//  EVENT_PDWACC_WORK* pParent = pwk;
  PDWACCMAIN_WORK* pWork;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PDWACC, 0x78000 );
  pWork = GFL_PROC_AllocWork( proc, sizeof( PDWACCMAIN_WORK ), HEAPID_PDWACC );
  GFL_STD_MemClear(pWork, sizeof(PDWACCMAIN_WORK));
  pWork->heapID = HEAPID_PDWACC;
  pWork->gameData = GAMEDATA_Create( HEAPID_PDWACC );

  {
    BOOL bIn = DREAMWORLD_SV_GetAccount(DREAMWORLD_SV_GetDreamWorldSaveData(GAMEDATA_GetSaveControlWork(pWork->gameData)));

    if(bIn){
      pWork->state = _WIFI_ACCOUNT;  //�J�[�h�\��
      pWork->pwdaccWork.type = PDWACC_DISPPASS;
    }
    else{  //�ڑ��ɂ���
      pWork->state = _WIFI_LOGIN;
      pWork->pwdaccWork.type = PDWACC_GETACC;
    }
  }

  return GFL_PROC_RES_FINISH;
}


FS_EXTERN_OVERLAY(wifi_login);
FS_EXTERN_OVERLAY(gamesync);
extern const GFL_PROC_DATA WiFiLogin_ProcData;
extern const GFL_PROC_DATA PDW_ACC_ProcData;

static GFL_PROC_RESULT PDWACCProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  PDWACCMAIN_WORK* pWork = mywk;

  switch (pWork->state) {
  case _WIFI_LOGIN:
    pWork->login.gamedata = pWork->gameData;
    pWork->login.bg       = WIFILOGIN_BG_NORMAL;
    pWork->login.display  = WIFILOGIN_DISPLAY_UP;
    pWork->login.nsid = WB_NET_PDW_ACC;
    pWork->login.pSvl = &pWork->aSVL;
    GFL_PROC_SysCallProc(FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &pWork->login);
    pWork->state++;
    break;
  case _WIFI_ACCOUNT:
    if(pWork->login.result == WIFILOGIN_RESULT_CANCEL){
      pWork->state = _WIFI_END;
    }
    else{
      pWork->pwdaccWork.gameData = pWork->gameData;
      pWork->pwdaccWork.heapID = pWork->heapID;
      pWork->pwdaccWork.pSvl = &pWork->aSVL;
      GFL_PROC_SysCallProc(FS_OVERLAY_ID(gamesync), &PDW_ACC_ProcData, &pWork->pwdaccWork);
      pWork->state++;
    }
    break;
  case _WIFI_ERROR:
    NetErr_DispCall(FALSE);
    pWork->state++;
    break;
  case _WIFI_LOGOUT:
    if(!GFL_NET_IsInit() || (pWork->pwdaccWork.returnCode == PDWACC_RETURNMODE_ERROR)){
      pWork->login.mode = WIFILOGIN_MODE_ERROR;
      pWork->state = _WIFI_LOGIN;
    }
    else{
      pWork->logout.gamedata = pWork->gameData;
      pWork->logout.bg       = WIFILOGIN_BG_NORMAL;
      pWork->logout.display  = WIFILOGIN_DISPLAY_UP;
      //    pWork->logout.nsid = WB_NET_PDW_ACC;
      GFL_PROC_SysCallProc(FS_OVERLAY_ID(wifi_login), &WiFiLogout_ProcData, &pWork->logout);
      pWork->state++;
    }
    break;
  default:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

static GFL_PROC_RESULT PDWACCProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  PDWACCMAIN_WORK* pWork = mywk;


  GAMEDATA_Delete(pWork->gameData);

  GFL_PROC_FreeWork(proc);

  GFL_HEAP_DeleteHeap(HEAPID_PDWACC);

  //�^�C�g���ɖ߂�
//  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
  OS_ResetSystem(0);

  
  return GFL_PROC_RES_FINISH;
}

// �v���Z�X��`�f�[�^
const GFL_PROC_DATA PDW_ACC_MainProcData = {
  PDWACCProc_Init,
  PDWACCProc_Main,
  PDWACCProc_End,
};

