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
#include "msg\msg_d_ohno.h"


typedef struct _G_SYNC_WORK G_SYNC_WORK;

typedef void (StateFunc)(G_SYNC_WORK* pState);

#define POSTURL "http://219.118.175.21:10610/cgi-bin/cgeartest/gsync.cgi"
#define GETURL1 "http://219.118.175.21:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data1&mac=1"
#define GETURL2 "http://219.118.175.21:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data2&mac=1"
#define GETURL3 "http://219.118.175.21:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data3&mac=1"
#define GETURL4 "http://219.118.175.21:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data4&mac=1"
#define GETURL5 "http://219.118.175.21:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data5&mac=1"
#define GETURL6 "http://219.118.175.21:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data6&mac=1"


static void _changeState(G_SYNC_WORK* pWork,StateFunc* state);
static void _changeStateDebug(G_SYNC_WORK* pWork,StateFunc* state, int line);

#define _NET_DEBUG (1)

#ifdef _NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pWork ,state)
#endif //_NET_DEBUG



struct _G_SYNC_WORK {
	GFL_FONT		  *fontHandle;
	GFL_MSGDATA		*mm;
	GFL_BMPWIN		*win;
	GFL_BMP_DATA	*bmp;
	HEAPID heapID;
	STRBUF			*strbuf;
	STRBUF			*strbufEx;
  WORDSET			*WordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[

	SAVE_CONTROL_WORK* pSaveData;
	StateFunc* state;      ///< �n���h���̃v���O�������
	vu32 count;
	int req;
	int getdataCount;
	BOOL bEnd;
	DWCGHTTPPost post;
	u8 mac[6];
	u8 buffer[0x102];
};


static void _ghttpGetting(G_SYNC_WORK* pWork);
static void _ghttpPost(G_SYNC_WORK* pWork);
static void _ghttpPosting(G_SYNC_WORK* pWork);
static void _ghttpGet(G_SYNC_WORK* pWork);
static void _ghttpKeyWait(G_SYNC_WORK* pWork);
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
  GFL_NET_DWC_HEAPSIZE + 0x80000,   ///< DWC�ւ�HEAP�T�C�Y
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


static void _msgPrint(G_SYNC_WORK* pWork,int msg)
{
	GFL_BMP_Clear( pWork->bmp, 0xff );
	GFL_MSG_GetString(pWork->mm, msg, pWork->strbuf);
	PRINTSYS_Print( pWork->bmp, 0, 0, pWork->strbuf, pWork->fontHandle);
	GFL_BMPWIN_TransVramCharacter( pWork->win );
}

static void _msgPrintNo(G_SYNC_WORK* pWork,int msg,int no)
{
	GFL_BMP_Clear( pWork->bmp, 0xff );
	GFL_MSG_GetString(pWork->mm, msg, pWork->strbufEx);

	WORDSET_RegisterNumber(pWork->WordSet, 0, no,
												 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
	WORDSET_ExpandStr( pWork->WordSet, pWork->strbuf,pWork->strbufEx );


	PRINTSYS_Print( pWork->bmp, 0, 0, pWork->strbuf, pWork->fontHandle);
	GFL_BMPWIN_TransVramCharacter( pWork->win );
}

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
	G_SYNC_WORK* pWork = param;

	pWork->bEnd = TRUE;
	OS_Printf("Recv Result : %d length %d\n", result,buflen);
	OS_Printf("Recv msg : %s \n", buf);
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

static void _ghttpConnecting(G_SYNC_WORK* pWork)
{
	int len;

	if(GFL_NET_StateGetWifiStatus()!=GFL_NET_STATE_MATCHING){
		return;
	}

	
	// ghttp�̏�����
	if(DWC_InitGHTTP(NULL) != TRUE){
		OS_Printf("error at SCInitialize()\n");
	}
	_msgPrint(pWork, DEBUG_OHNO_MSG0007);
	
	_CHANGE_STATE(_ghttpKeyWait);
}


static void _ghttpPost(G_SYNC_WORK* pWork)
{
	
	// send data
	//-------------------------------------------
	GFL_STD_MemClear(pWork->mac, 6);

	OS_GetMacAddress(pWork->mac);


	_msgPrint(pWork, DEBUG_OHNO_MSG0008);
	

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

	pWork->count = OS_GetVBlankCount();
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
		_msgPrint(pWork, DEBUG_OHNO_MSG0008);

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
	switch(GFL_UI_KEY_GetTrg())
	{
	case PAD_BUTTON_X:
		_CHANGE_STATE(_ghttpPost);
		break;
	case PAD_BUTTON_Y:

		pWork->count = OS_GetVBlankCount();
		OS_TPrintf("start %d  count %d\n",OS_GetVBlankCount(), pWork->getdataCount);
		pWork->getdataCount = 0;//�ǂݍ��ݎ��͕������Ȃ��ƃq�[�v������Ȃ�

		_CHANGE_STATE(_ghttpGet);
		break;
	case PAD_BUTTON_B:
		_CHANGE_STATE(NULL);
		DWC_ShutdownGHTTP();
		break;
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
		vu32 ans = OS_GetVBlankCount() - pWork->count;
		int ansint = ans / 60;
		
		OS_TPrintf("end %d\n",OS_GetVBlankCount());
		
		pWork->bEnd = FALSE;
		_msgPrintNo(pWork, DEBUG_OHNO_MSG0009, ansint);
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
	char* bufadd[]={GETURL1,GETURL2,GETURL3,GETURL4,GETURL4,GETURL6};


	pWork->bEnd = FALSE;

	if( 0 <= DWC_GetGHTTPData(bufadd[pWork->getdataCount], GETCallback, (void*)pWork)){


		_msgPrint(pWork, DEBUG_OHNO_MSG0010);
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
		pWork->getdataCount++;
		if(pWork->getdataCount < 6){   //710703�o�C�g�͈��131072�Ȃ̂� 6��
			_CHANGE_STATE(_ghttpGet);
		}
		else{
			vu32 ans = OS_GetVBlankCount() - pWork->count;
			int ansint = ans / 60;
			OS_TPrintf("end %d\n",OS_GetVBlankCount());
			_msgPrintNo(pWork, DEBUG_OHNO_MSG0011, ansint);
			_CHANGE_STATE(_ghttpKeyWait);
		}
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

	_msgPrint(pWork, DEBUG_OHNO_MSG0006);

	_CHANGE_STATE(_ghttpConnecting);

	
}

static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_0123_E,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_32K,	// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};


static GFL_PROC_RESULT GSYNCProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PROC, 0x70000 );//�e�X�g

	{
		G_SYNC_WORK* pWork = GFL_PROC_AllocWork( proc, sizeof(G_SYNC_WORK), HEAPID_PROC );

		pWork->pSaveData = SaveControl_GetPointer();  //�f�o�b�O
		pWork->heapID = HEAPID_PROC;

		GFL_DISP_SetBank( &vramBank );

		//�o�b�N�O���E���h�̐F�����Ă���
		GFL_STD_MemFill16((void*)HW_BG_PLTT, 0x5ad6, 2);
	
		// �e����ʃ��W�X�^������
		G2_BlendNone();

		// BGsystem������
		GFL_BG_Init( pWork->heapID );
		GFL_BMPWIN_Init( pWork->heapID );
		GFL_FONTSYS_Init();

			//�a�f���[�h�ݒ�
	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysHeader );
	}

	// �ʃt���[���ݒ�
	{
		static const GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};

		GFL_BG_SetBGControl( GFL_BG_FRAME0_M,   &bgcntText,   GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S,   &bgcntText,   GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

//		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0x22, wk->heapID );
		GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0xff, 1, 0 );

//		void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
		GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_FillCharacter( GFL_BG_FRAME0_S, 0x00, 1, 0 );
		GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	}

	// �㉺��ʐݒ�
		GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		pWork->win = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 4, 12, 24, 16, 0, GFL_BMP_CHRAREA_GET_F );
		pWork->bmp = GFL_BMPWIN_GetBmp( pWork->win);
		GFL_BMP_Clear( pWork->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( pWork->win );
		GFL_BMPWIN_TransVramCharacter( pWork->win );

		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		pWork->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr,
																			GFL_FONT_LOADTYPE_FILE, FALSE, pWork->heapID );

		pWork->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_ohno_dat, pWork->heapID );
		pWork->strbuf = GFL_STR_CreateBuffer(64, pWork->heapID);
		pWork->strbufEx = GFL_STR_CreateBuffer(64, pWork->heapID);
		pWork->WordSet    = WORDSET_Create( pWork->heapID );

	//�t�H���g�p���b�g�]��
		GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
																	0, 0x20, HEAPID_PROC);

		
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
	G_SYNC_WORK* pWork = mywk;

	GFL_STR_DeleteBuffer(pWork->strbuf);
	GFL_STR_DeleteBuffer(pWork->strbufEx);
	GFL_MSG_Delete(pWork->mm);
	GFL_FONT_Delete(pWork->fontHandle);

	WORDSET_Delete( pWork->WordSet );

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_PROC);  //�e�X�g
  return GFL_PROC_RES_FINISH;
}

// �v���Z�X��`�f�[�^
const GFL_PROC_DATA G_SYNC_ProcData = {
  GSYNCProc_Init,
  GSYNCProc_Main,
  GSYNCProc_End,
};

