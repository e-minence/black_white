//=============================================================================
/**
 * @file	  gsync_state.c
 * @brief	  �Q�[���V���N
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/04/30
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "arc_def.h"

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

typedef struct _G_SYNC_WORK G_SYNC_WORK;

typedef void (StateFunc)(G_SYNC_WORK* pState);

#define POSTURL "http://219.118.175.21:10610/cgi-bin/cgeartest/gsync.cgi"
#define GETURL "http://219.118.175.21:10610/cgi-bin/cgeartest/getsync.cgi"


static void _changeState(G_SYNC_WORK* pWork,StateFunc* state);
static void _changeStateDebug(G_SYNC_WORK* pWork,StateFunc* state, int line);

#define _NET_DEBUG (1)

#ifdef _NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pWork ,state)
#endif //_NET_DEBUG



struct _G_SYNC_WORK {
	SAVE_CONTROL_WORK* pSaveData;
	StateFunc* state;      ///< �n���h���̃v���O�������
	int req;
	BOOL bEnd;
	DWCGHTTPPost post;
	u8 mac[6];
	u8 buffer[0x102];
};


static void _ghttpGetting(G_SYNC_WORK* pWork);
static void _ghttpPost(G_SYNC_WORK* pWork);
static void _ghttpPosting(G_SYNC_WORK* pWork);
static void _ghttpGet(G_SYNC_WORK* pWork);
static void GETCallback(const char* buf, int buflen, DWCGHTTPResult result, void* param);


static DWCUserData* _getMyUserData(void* pWork)
{
  G_SYNC_WORK *wk = (G_SYNC_WORK*)pWork;
  return WifiList_GetMyUserInfo(SaveData_GetWifiListData(wk->pSaveData));
}

static DWCFriendData* _getFriendData(void* pWork)
{
  G_SYNC_WORK *wk = (G_SYNC_WORK*)pWork;
  //    NET_PRINT("Friend %d\n",WifiList_GetFriendDataNum(SaveData_GetWifiListData(wk->pSaveData)));
  return WifiList_GetDwcDataPtr(SaveData_GetWifiListData(wk->pSaveData),0);
}



static GFLNetInitializeStruct aGFLNetInit = {
  NULL,  // ��M�֐��e�[�u��
  0, // ��M�e�[�u���v�f��
  NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  NULL,  // �r�[�R���f�[�^�擾�֐�
  NULL,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
  NULL,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
  NULL,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  _getFriendData,   ///< DWC�`���̗F�B���X�g
  _getMyUserData,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  GFL_NET_DWC_HEAPSIZE,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
  0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_NETWORK,  //IRC�p��create�����HEAPID
  GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
  1,     // �ő�ڑ��l��
  48,    // �ő呗�M�o�C�g��
  32,    // �ő�r�[�R�����W��
  TRUE,     // CRC�v�Z
  FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIFI,  //�ʐM���
  TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_WIFICLUB,  //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
};



//=============================================================================
/*!
 *	@brief	�f�[�^��M�������̃R�[���o�b�N
 *
 *	@param	buf		[in] �f�[�^�o�b�t�@
 *	@param	buflen	[in] buf�̃T�C�Y
 *  @param  param   [in] �R�[���o�b�N�p�p�����[�^
 *
 *	@retval	TRUE	buf�J��
 *	@retval	FALSE   buf�J������
 */
//=============================================================================
static void GETCallback(const char* buf, int buflen, DWCGHTTPResult result, void* param)
{
	char*	g_data;	//! ��M�f�[�^�̃o�b�t�@
#if 0
	g_data = param;

	if(buf){
		MI_CpuCopy8(buf, g_data, (u32)buflen);
	}

	g_pending = 0;
#endif
	OS_Printf("Recv Result : %d\n", result);

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
	OS_TPrintf("ghttp: %d\n",line);
#endif
	_changeState(pWork, state);
}
#endif


//=============================================================================
/*!
 *	@brief	�f�[�^���M�������̃R�[���o�b�N
 *
 *	@param	buf		[in] �f�[�^�o�b�t�@
 *	@param	buflen	[in] buf�̃T�C�Y
 *
 *	@retval	TRUE	buf�J��
 *	@retval	FALSE   buf�J������
*/
//=============================================================================
static void POSTCallback(const char* buf, int buflen, DWCGHTTPResult result, void* param)
{
	G_SYNC_WORK* pWork = param;

	OS_TPrintf("GHTTP�̃f�[�^���M����\n");
	pWork->bEnd = TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpPost(G_SYNC_WORK* pWork)
{
	int len;

	if(GFL_NET_StateGetWifiStatus()!=GFL_NET_STATE_MATCHING){
		return;
	}

	
	// ghttp�̏�����
	if(DWC_InitGHTTP(NULL) != TRUE){
		OS_Printf("error at SCInitialize()\n");
	}

	// send data
	//-------------------------------------------
	GFL_STD_MemClear(pWork->mac, 6);

	OS_GetMacAddress(pWork->mac);


	

	// post data
	//-------------------------------------------
	{
		DWC_GHTTPNewPost(&pWork->post);
//		DWC_GHTTPPostAddString(&pWork->post, "name", "sogabe");
//		DWC_GHTTPPostAddString(&pWork->post, "mac", "123456");
//		DWC_GHTTPPostAddString(&pWork->post, "save", "datatest");

		DWC_GHTTPPostAddFileFromMemory(&pWork->post, "name1", (const char*)"sogabe",6,
																	 "name.txt","application/octet-stream");

		DWC_GHTTPPostAddFileFromMemory(&pWork->post, "file1", (const char*)SaveControl_GetPointer(),0x80000,
																	 "test.bin","application/octet-stream");

//		DWC_GHTTPPostAddFileFromMemory(&pWork->post, "mac", (const char*)pWork->mac,6,
	//																 "mac.bin","application/octet-stream");

	}
	
	OS_TPrintf("start %d\n",OS_GetVBlankCount());

	OS_TPrintf("DWC_GHTTP_HOST_LOOKUP       %d\n",DWC_GHTTP_HOST_LOOKUP       );
	OS_TPrintf("DWC_GHTTP_CONNECTING        %d\n",DWC_GHTTP_CONNECTING        );
	OS_TPrintf("DWC_GHTTP_SECURING_SESSION  %d\n",DWC_GHTTP_SECURING_SESSION  );
	OS_TPrintf("DWC_GHTTP_SENDING_REQUEST   %d\n",DWC_GHTTP_SENDING_REQUEST   );
	OS_TPrintf("DWC_GHTTP_POSTING           %d\n",DWC_GHTTP_POSTING           );
	OS_TPrintf("DWC_GHTTP_WAITING           %d\n",DWC_GHTTP_WAITING           );
	OS_TPrintf("DWC_GHTTP_RECEIVING_STATUS  %d\n",DWC_GHTTP_RECEIVING_STATUS  );
	OS_TPrintf("DWC_GHTTP_RECEIVING_HEADERS %d\n",DWC_GHTTP_RECEIVING_HEADERS );
	OS_TPrintf("DWC_GHTTP_RECEIVING_FILE    %d\n",DWC_GHTTP_RECEIVING_FILE    );
	OS_TPrintf("DWC_GHTTP_SOCKET_INIT       %d\n",DWC_GHTTP_SOCKET_INIT       );
	OS_TPrintf("DWC_GHTTP_LOOKUP_PENDING    %d\n",DWC_GHTTP_LOOKUP_PENDING    );

	pWork->bEnd = FALSE;

	
	pWork->req = DWC_PostGHTTPData(POSTURL, &pWork->post, POSTCallback, pWork);
	
	if( 0 <= pWork->req){
		_CHANGE_STATE(_ghttpPosting);
	}
}



//------------------------------------------------------------------------------
/**
 * @brief   �L�[���͂œ�����ς���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpKeyWait(G_SYNC_WORK* pWork)
{
	if(GFL_UI_KEY_GetTrg()==PAD_BUTTON_X){
		_CHANGE_STATE(_ghttpPost);
	}
	else if(GFL_UI_KEY_GetTrg()==PAD_BUTTON_Y){
		_CHANGE_STATE(_ghttpGet);
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpPosting(G_SYNC_WORK* pWork)
{

	DWC_ProcessGHTTP();
	//OS_TPrintf("state %d\n",DWC_GetGHTTPState(pWork->req));
	
	if(	pWork->bEnd ){
		OS_TPrintf("end %d\n",OS_GetVBlankCount());
		pWork->bEnd = FALSE;
		_CHANGE_STATE(_ghttpKeyWait);
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpGet(G_SYNC_WORK* pWork)
{
	pWork->bEnd = FALSE;
	if( 0 <= DWC_GetGHTTPData("URL", GETCallback, (void*)pWork->buffer)){
		_CHANGE_STATE(_ghttpGetting);
	}

}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpGetting(G_SYNC_WORK* pWork)
{

	DWC_ProcessGHTTP();
	if(	pWork->bEnd ){

		DWC_ShutdownGHTTP();
		_CHANGE_STATE(NULL);
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM������
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ghttpInit(G_SYNC_WORK* pWork)
{

	GFL_NET_Init(&aGFLNetInit, NULL, pWork);
	GFL_NET_StateWifiEnterLogin();
	_CHANGE_STATE(_ghttpPost);

	
}



static GFL_PROC_RESULT GSYNCProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PROC, 0x70000 );//�e�X�g

	{
		G_SYNC_WORK* pWork = GFL_PROC_AllocWork( proc, sizeof(G_SYNC_WORK), HEAPID_PROC );
		pWork->pSaveData = SaveControl_GetPointer();  //�f�o�b�O
		_CHANGE_STATE(_ghttpInit);
	}
  return GFL_PROC_RES_FINISH;
}
static GFL_PROC_RESULT GSYNCProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	G_SYNC_WORK* pWork = mywk;
	StateFunc* state = pWork->state;

	if( state ){
		state( pWork );
		return GFL_PROC_RES_CONTINUE;
	}
	return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT GSYNCProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork(mywk);
	GFL_HEAP_DeleteHeap(HEAPID_PROC);  //�e�X�g
  return GFL_PROC_RES_FINISH;
}

// �v���Z�X��`�f�[�^
const GFL_PROC_DATA G_SYNC_ProcData = {
  GSYNCProc_Init,
  GSYNCProc_Main,
  GSYNCProc_End,
};

