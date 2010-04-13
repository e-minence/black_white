//=============================================================================
/**
 * @file	  net_save.c
 * @bfief	  �ʐM�Z�[�u
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  10/02/15
 */
//=============================================================================

#include <gflib.h>


#include "net/network_define.h"

#include "system/main.h"
#include "system/gfl_use.h"
#include "system/net_err.h"

#include "net/net_save.h"

#if DEBUG_ONLY_FOR_ohno
#define _NETSAVE_DEBUG (1)
#else
#define _NETSAVE_DEBUG (0)
#endif

typedef enum
{
  _CHANGERAND = 60,    ///< �|�P���������^�C�~���O�h�炬��
  _TIMING_ENDNO=112,
  _TIMING_SAVEST,
  _TIMING_SAVECHK,
  _TIMING_SAVELAST,
  _TIMING_SAVEEND,
  _TIMING_ANIMEEND,

} NET_TIMING_ENUM;



typedef void (StateFunc)(NET_SAVE_WORK* pState);

struct _NET_SAVE_WORK{
  SAVE_LASTCALLBACK* lastCallback;
  void* pWorkOrg;
  StateFunc* state;
  GAMEDATA* pGameData;
  int saveStep;
};


#if _TRADE_DEBUG

static void _changeStateDebug(NET_SAVE_WORK* pWork,StateFunc* state, int line);

#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)

#else  //_NET_DEBUG

static void _changeState(NET_SAVE_WORK* pWork,StateFunc* state);

#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)

#endif //_NET_DEBUG


static void _changeTimingSaveStart(NET_SAVE_WORK* pWork);
static void _changeTimingSaveStart2(NET_SAVE_WORK* pWork);
static void _changeTimingSaveStart3(NET_SAVE_WORK* pWork);
static void _changeTimingSaveStart32(NET_SAVE_WORK* pWork);
static void _changeTimingSaveStart4(NET_SAVE_WORK* pWork);
static void _changeTimingSaveStart5(NET_SAVE_WORK* pWork);
static void _changeDemo_ModelTrade25(NET_SAVE_WORK* pWork);
static void _changeDemo_ModelTrade26(NET_SAVE_WORK* pWork);



//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(NET_SAVE_WORK* pWork,StateFunc state)
{
  pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#if _TRADE_DEBUG
void _changeStateDebug(NET_SAVE_WORK* pWork,StateFunc state, int line)
{
  OS_TPrintf("ns: %d\n",line);
  _changeState(pWork, state);
}
#endif


//------------------------------------------------------------------
/**
 * @brief   �|�P���������J�n
 * @param   NET_SAVE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------

NET_SAVE_WORK* NET_SAVE_Init(HEAPID heapID, GAMEDATA* pGameData)
{
  NET_SAVE_WORK* pWork = GFL_HEAP_AllocClearMemory(heapID,sizeof(NET_SAVE_WORK));

  pWork->pGameData = pGameData;
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_SAVEST, WB_NET_TRADE_SERVICEID);
  _CHANGE_STATE(pWork, _changeTimingSaveStart2);
  return pWork;
}

//------------------------------------------------------------------
/**
 * @brief   �|�P���������J�n  ���[�N��
 * @param   NET_SAVE_WORK ���[�N
 * @retval  none
 */
//------------------------------------------------------------------

NET_SAVE_WORK* NET_SAVE_InitCallback(HEAPID heapID, GAMEDATA* pGameData, SAVE_LASTCALLBACK* pCallback, void* pWorkOrg)
{
  NET_SAVE_WORK* pWork = GFL_HEAP_AllocClearMemory(heapID,sizeof(NET_SAVE_WORK));

  pWork->lastCallback = pCallback;
  pWork->pWorkOrg = pWorkOrg;
  pWork->pGameData = pGameData;
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_SAVEST, WB_NET_TRADE_SERVICEID);
  _CHANGE_STATE(pWork, _changeTimingSaveStart2);
  return pWork;
}


//------------------------------------------------------------------
/**
 * @brief   �ʐM�Z�[�u
 * @param   pNetSaveWork  ���[�N
 * @retval  �I��������TRUE
 */
//------------------------------------------------------------------

BOOL NET_SAVE_Main(NET_SAVE_WORK* pNetSaveWork)
{
  if(pNetSaveWork->state){
    pNetSaveWork->state(pNetSaveWork);
    if(pNetSaveWork->state){
      return FALSE;
    }
  }
  return TRUE;
}


//------------------------------------------------------------------
/**
 * @brief   �ʐM�Z�[�u�I������
 * @param   pNetSaveWork  ���[�N
 */
//------------------------------------------------------------------

void NET_SAVE_Exit(NET_SAVE_WORK* pNetSaveWork)
{
  GFL_HEAP_FreeMemory(pNetSaveWork);
}




static void _changeTimingSaveStart2(NET_SAVE_WORK* pWork)
{
  if(NET_ERR_CHECK_NONE!=NetErr_App_CheckError()){  //�G���[�Ȃ�I��
    _CHANGE_STATE(pWork,NULL);
  }
  else if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVEST,WB_NET_TRADE_SERVICEID)){
    GAMEDATA_SaveAsyncStart(pWork->pGameData);
    _CHANGE_STATE(pWork,_changeTimingSaveStart3);
  }
}

static void _changeTimingSaveStart3(NET_SAVE_WORK* pWork)
{
  if(GAMEDATA_SaveAsyncMain(pWork->pGameData) == SAVE_RESULT_LAST){
    if(NET_ERR_CHECK_NONE!=NetErr_App_CheckError()){  //�G���[�Ȃ�I��
      _CHANGE_STATE(pWork,NULL);
    }
    else{
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_SAVECHK, WB_NET_TRADE_SERVICEID);
      pWork->saveStep = GFUser_GetPublicRand(_CHANGERAND);
      _CHANGE_STATE(pWork,_changeTimingSaveStart32);
    }
  }
}

static void _changeTimingSaveStart32(NET_SAVE_WORK* pWork)
{
  if(NET_ERR_CHECK_NONE!=NetErr_App_CheckError()){  //�G���[�Ȃ�I��
    _CHANGE_STATE(pWork,NULL);
  }
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVECHK,WB_NET_TRADE_SERVICEID)){
    _CHANGE_STATE(pWork,_changeTimingSaveStart4);
  }
}


static void _changeTimingSaveStart4(NET_SAVE_WORK* pWork)
{
  if(NET_ERR_CHECK_NONE!=NetErr_App_CheckError()){  //�G���[�Ȃ�I��
    _CHANGE_STATE(pWork,NULL);
  }
  pWork->saveStep--;
  if(pWork->saveStep<0){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVELAST,WB_NET_TRADE_SERVICEID);
    _CHANGE_STATE(pWork, _changeTimingSaveStart5);
  }
}

static void _changeTimingSaveStart5(NET_SAVE_WORK* pWork)
{
  if(NET_ERR_CHECK_NONE!=NetErr_App_CheckError()){  //�G���[�Ȃ�I��
    _CHANGE_STATE(pWork,NULL);
  }
  else if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVELAST,WB_NET_TRADE_SERVICEID)){
    _CHANGE_STATE(pWork,_changeDemo_ModelTrade25);
  }
}

static void _changeDemo_ModelTrade25(NET_SAVE_WORK* pWork)
{
  if(pWork->lastCallback){
    pWork->lastCallback(pWork->pWorkOrg);
  }
  if(GAMEDATA_SaveAsyncMain(pWork->pGameData) == SAVE_RESULT_OK){
    if(NET_ERR_CHECK_NONE!=NetErr_App_CheckError()){  //�G���[�Ȃ�I��
      _CHANGE_STATE(pWork,NULL);
    }
    else{
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVEEND, WB_NET_TRADE_SERVICEID);
      _CHANGE_STATE(pWork,_changeDemo_ModelTrade26);
    }
  }
}

static void _changeDemo_ModelTrade26(NET_SAVE_WORK* pWork)
{
  if(NET_ERR_CHECK_NONE!=NetErr_App_CheckError()){  //�G���[�Ȃ�I��
    _CHANGE_STATE(pWork,NULL);
  }
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_SAVEEND, WB_NET_TRADE_SERVICEID)){
    _CHANGE_STATE(pWork, NULL);
  }
}

