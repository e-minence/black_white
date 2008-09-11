//============================================================================================
/**
 * @file	d_taya.c
 * @brief	�f�o�b�O�p�֐�
 * @author	taya
 * @date	2008.08.01
 */
//============================================================================================

// lib includes -----------------------
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include <msgdata.h>

// global includes --------------------
#include "system\main.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "net\network_define.h"

// local includes ---------------------
#include "msg\msg_d_taya.h"

// archive includes -------------------
#include "arc_def.h"
#include "message.naix"
#include "test_graphic\d_taya.naix"



typedef BOOL (*pSubProc)( GFL_PROC*, int*, void*, void* );

typedef struct {

	u16		yofs;
	u8		strNum;
	u8		kanjiMode;

}TEST_PACKET;

typedef struct {

	u16		seq;
	HEAPID	heapID;
	u16		strNum;
	u16		kanjiMode;
	u32		yofs;

	GFL_BMPWIN*			win;
	GFL_BMP_DATA*		bmp;
	GFL_MSGDATA*		mm;
	STRBUF*				strbuf;
	GFL_TCBLSYS*		tcbl;

	ARCHANDLE*				arcHandle;
	GFL_FONT*				fontHandle;
	PRINT_STREAM*			printStream;
	PRINT_QUE*				printQue;
	PRINT_UTIL				printUtil[1];

	pSubProc		subProc;
	int				subSeq;

//	PRINT_STREAM_HANDLE	psHandle;

	GFLNetInitializeStruct		netInitWork;
	int							netTestSeq;
	GFL_NETHANDLE*				netHandle;
	BOOL						ImParent;

	TEST_PACKET			packet;

}KANJI_WORK;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static GFL_PROC_RESULT DebugTayaMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT DebugTayaMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static BOOL SUBPROC_PrintTest( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT DebugTayaMainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void testPacketFunc( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static void* testBeaconGetFunc( void );
static int testBeaconGetSizeFunc( void );
static BOOL testBeaconCompFunc( GameServiceID myNo, GameServiceID beaconNo );
static void testCallBack(void* pWork);
static BOOL SUBPROC_NetPrintTest( GFL_PROC* proc, int* seq, void* pwk, void* mywk );




//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugTayaMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	static const GFL_BG_DISPVRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_0123_E			// �e�N�X�`���p���b�g�X���b�g
	};

#if 0
	static const char* arcFiles[] = {
		"test_graphic/d_taya.narc",
	};
	enum {
		ARCID_D_TAYA,
	};
#endif


	KANJI_WORK* wk;

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TAYA_DEBUG, 0x70000 );
	wk = GFL_PROC_AllocWork( proc, sizeof(KANJI_WORK), HEAPID_TAYA_DEBUG );
	wk->heapID = HEAPID_TAYA_DEBUG;

//	GFL_ARC_Init( arcFiles, NELEMS(arcFiles) );	gfl_use.c��1�񂾂��������ɕύX

	GFL_DISP_SetBank( &vramBank );

	// �e����ʃ��W�X�^������
	G2_BlendNone();

	// BGsystem������
	GFL_BG_Init( wk->heapID );
	GFL_BMPWIN_Init( wk->heapID );

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

		GFL_BG_SetVisible( GFL_BG_FRAME0_S,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME1_S,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME2_S,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_S,   VISIBLE_OFF );

//		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0x22, wk->heapID );
		GFL_ARC_UTIL_TransVramPalette( ARCID_D_TAYA, NARC_d_taya_default_nclr, PALTYPE_MAIN_BG, 0, 0, wk->heapID );
//		void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
		GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0xff, 1, 0 );
		GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		GFL_ARC_UTIL_TransVramPalette( ARCID_D_TAYA, NARC_d_taya_default_nclr, PALTYPE_SUB_BG, 0, 0, wk->heapID );
		GFL_BG_FillCharacter( GFL_BG_FRAME0_S, 0xff, 1, 0 );
		GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	}

	// �㉺��ʐݒ�
	GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

	wk->win = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 0, 0, 32, 24, 0, GFL_BMP_CHRAREA_GET_F );
	wk->bmp = GFL_BMPWIN_GetBmp(wk->win);
	GFL_BMP_Clear( wk->bmp, 0xff );
	GFL_BMPWIN_MakeScreen( wk->win );

	wk->subProc = NULL;

	GFL_BMPWIN_TransVramCharacter( wk->win );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

	wk->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_taya_dat, wk->heapID );
	wk->strbuf = GFL_STR_CreateBuffer( 1024, wk->heapID );
	wk->seq = 0;

	wk->tcbl = GFL_TCBL_Init( wk->heapID, wk->heapID, 4, 32 );

//	GFL_FONT* GFL_FONT_CreateHandle( ARCHANDLE* arcHandle, u32 datID, GFL_FONT_LOADTYPE loadType, BOOL fixedFontFlag, u32 heapID )
	wk->arcHandle = GFL_ARC_OpenDataHandle( ARCID_D_TAYA, wk->heapID );
	wk->fontHandle = GFL_FONT_CreateHandle( wk->arcHandle, NARC_d_taya_lc12_2bit_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	PRINTSYS_Init( wk->heapID );
	wk->printQue = PRINTSYS_QUE_Create( wk->heapID );

	PRINT_UTIL_Setup( wk->printUtil, wk->win );

	return GFL_PROC_RES_FINISH;
}
//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugTayaMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	KANJI_WORK* wk = mywk;

	PRINTSYS_QUE_Main( wk->printQue );

	switch( *seq ){
	case 0:
		{
			u16 key = GFL_UI_KEY_GetTrg();

			do {
				if( key & PAD_BUTTON_A )
				{
					wk->subProc = SUBPROC_PrintTest; break;
				}

				if( key & PAD_BUTTON_B )
				{
					wk->subProc = SUBPROC_NetPrintTest; break;
				}

			}while(0);

			if( wk->subProc != NULL )
			{
				wk->subSeq = 0;
				(*seq)++;
			}
		}
		break;

	case 1:
		if( wk->subProc( proc, &(wk->subSeq), pwk, mywk ) )
		{
			wk->subProc = NULL;
			*seq = 0;
		}
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------
// �X�^���h�A������Ԃł̊���PrintTest
//------------------------------------------------------
static BOOL SUBPROC_PrintTest( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	static const u16 strID[] = {
		DEBUG_TAYA_STR01,
		DEBUG_TAYA_STR02,
		DEBUG_TAYA_STR03,
		DEBUG_TAYA_STR04,
		DEBUG_TAYA_STR05,
		DEBUG_TAYA_STR06,
		DEBUG_TAYA_STR07,
		DEBUG_TAYA_STR08,
		DEBUG_TAYA_STR09,
	};

	KANJI_WORK* wk = mywk;

	GFL_TCBL_Main( wk->tcbl );

	if( PRINT_UTIL_Trans( wk->printUtil, wk->printQue ) )
	{
		return FALSE;
	}

	switch( *seq ){
	case 0:
		GFL_MSG_GetString( wk->mm, DEBUG_TAYA_STR00, wk->strbuf );
		GFL_BMP_Clear( wk->bmp, 0xff );
		PRINT_UTIL_Print( wk->printUtil, wk->printQue, 0, 0, wk->strbuf, wk->fontHandle );
		(*seq)++;
		break;

	case 1:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
		{
			GFL_MSG_GetString( wk->mm, DEBUG_TAYA_STR10, wk->strbuf );
			GFL_BMP_Clear( wk->bmp, 0xff );
			PRINT_UTIL_Print( wk->printUtil, wk->printQue, 0, 0, wk->strbuf, wk->fontHandle );
//			GFL_BMPWIN_TransVramCharacter( wk->win );
			(*seq) = 10;
			break;
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
		{
			wk->strNum = 0;
			wk->yofs = 30;
			(*seq)++;
			break;
		}
		break;

	case 2:
		GFL_MSG_GetString( wk->mm, strID[wk->strNum], wk->strbuf );
		wk->printStream = PRINTSYS_PrintStream( wk->win, 0, wk->yofs,
						wk->strbuf, wk->fontHandle, 0, wk->tcbl, 0, wk->heapID, 0xff );
		(*seq)++;
		break;

	case 3:
		if( PRINTSYS_PrintStreamGetState(wk->printStream) == PRINTSTREAM_STATE_DONE )
		{
			PRINTSYS_PrintStreamDelete( wk->printStream );
			(*seq)++;
		}
		break;

	case 4:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
		{
			if( ++(wk->strNum) < NELEMS(strID) )
			{
				wk->yofs += 16;
				if( strID[wk->strNum] == DEBUG_TAYA_STR09 )
				{
					wk->yofs += 16;
				}
				(*seq) = 2;
			}
			else
			{
				wk->kanjiMode = !(wk->kanjiMode);
				GFL_MSG_SetLangID( wk->kanjiMode );
				(*seq) = 0;
			}
		}
		break;

	case 10:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
		{
			(*seq) = 0;
		}
		break;

	}

	return FALSE;
}
//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugTayaMainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	return GFL_PROC_RES_FINISH;
}




//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA		DebugTayaMainProcData = {
	DebugTayaMainProcInit,
	DebugTayaMainProcMain,
	DebugTayaMainProcEnd,
};


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

enum {
	TEST_GGID				= 0x3594,
	TEST_COMM_MEMBER_MAX	= 2,
	TEST_COMM_SEND_SIZE_MAX	= 128,
	TEST_COMM_BCON_MAX		= 1,

	TEST_TIMINGID_INIT		= 11,
	TEST_TIMINGID_PRINT,

};

static const NetRecvFuncTable  testPacketTbl[] = {
    { testPacketFunc, GFL_NET_COMMAND_SIZE(sizeof(TEST_PACKET)), NULL },
};

typedef struct{
    int gameNo;   ///< �Q�[�����
}TEST_BCON;

static TEST_BCON testBcon = { WB_NET_SERVICEID_DEBUG_TAYA };



static const GFLNetInitializeStruct testNetInitParam = {
	testPacketTbl,				// ��M�֐��e�[�u��
	NELEMS(testPacketTbl),		// ��M�e�[�u���v�f��
	NULL,						// ���[�N�|�C���^
	NULL,						// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
	NULL,						// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
	testBeaconGetFunc,			// �r�[�R���f�[�^�擾�֐�
	testBeaconGetSizeFunc,		// �r�[�R���f�[�^�T�C�Y�擾�֐�
	testBeaconCompFunc,			// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
	FatalError_Disp,			// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	NULL,						// �ʐM�ؒf���ɌĂ΂��֐�
	NULL,						// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
	TEST_GGID,					// ggid  DP=0x333,RANGER=0x178,WII=0x346
	GFL_HEAPID_APP,				//���ɂȂ�heapid
	HEAPID_NETWORK,				//�ʐM�p��create�����HEAPID
	HEAPID_WIFI,				//wifi�p��create�����HEAPID
	GFL_WICON_POSX,				// �ʐM�A�C�R��XY�ʒu
	GFL_WICON_POSY,
	TEST_COMM_MEMBER_MAX,		// �ő�ڑ��l��
	TEST_COMM_SEND_SIZE_MAX,	//�ő呗�M�o�C�g��
	TEST_COMM_BCON_MAX,			// �ő�r�[�R�����W��
	TRUE,						// CRC�v�Z
	FALSE,						// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
	FALSE,						// wifi�ʐM���s�����ǂ���
	TRUE,						// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	WB_NET_SERVICEID_DEBUG_TAYA,//GameServiceID
};

static void testPacketFunc( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	KANJI_WORK* wk = pWork;
	const TEST_PACKET* packet = pData;

	wk->netTestSeq = 1;
	wk->packet = *packet;

	/*
	SAMPLE_WORK* psw = sampleWork;

	if(GetSampleNetID() != netID){
	    GFL_STD_MemCopy(pData, &psw->recvWork, sizeof(VecFx32));
	}
	*/
}

///< �r�[�R���f�[�^�擾�֐�
static void* testBeaconGetFunc( void )
{
	return &testBcon;
}
///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int testBeaconGetSizeFunc( void )
{
	return sizeof(testBcon);
}

///< �r�[�R���f�[�^��r�֐�
static BOOL testBeaconCompFunc( GameServiceID myNo, GameServiceID beaconNo )
{
    if(myNo != beaconNo ){
        return FALSE;
    }
    return TRUE;
}


static void testCallBack(void* pWork)
{
	
}
static void autoConnectCallBack( void* pWork )
{
	KANJI_WORK* wk = pWork;
	wk->netTestSeq = 1;
}



//------------------------------------------------------
// �ʐM��Ԃł̊���PrintTest
//------------------------------------------------------
static BOOL SUBPROC_NetPrintTest( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	static const u16 strID[] = {
		DEBUG_TAYA_STR01,
		DEBUG_TAYA_STR02,
		DEBUG_TAYA_STR03,
		DEBUG_TAYA_STR04,
		DEBUG_TAYA_STR05,
		DEBUG_TAYA_STR06,
		DEBUG_TAYA_STR07,
		DEBUG_TAYA_STR08,
		DEBUG_TAYA_STR09,
	};

	KANJI_WORK* wk = mywk;

	GFL_TCBL_Main( wk->tcbl );

	if( PRINT_UTIL_Trans( wk->printUtil, wk->printQue ) )
	{
		return FALSE;
	}

	switch( *seq ){
	case 0:
		wk->netInitWork = testNetInitParam;
		wk->netInitWork.pWork = (void*)wk;
		wk->netTestSeq = 0;
		wk->kanjiMode = 0;
		wk->packet.kanjiMode = 0;
	    GFL_NET_Init(&(wk->netInitWork), testCallBack);
	    (*seq)++;
	    break;

	case 1:
		if( GFL_NET_IsInit() )
		{
			GFL_NET_ChangeoverConnect( autoConnectCallBack ); // �����ڑ�
			(*seq)++;
		}
		break;

	case 2:
		if( wk->netTestSeq )
		{
			wk->netTestSeq = 0;
			wk->netHandle = GFL_NET_HANDLE_GetCurrentHandle();
			GFL_NET_TimingSyncStart( wk->netHandle, TEST_TIMINGID_INIT );
			(*seq)++;
		}
		break;

	case 3:
		if( GFL_NET_IsTimingSync(wk->netHandle, TEST_TIMINGID_INIT) )
		{
			wk->ImParent = GFL_NET_IsParentMachine();
			(*seq)++;
		}
		break;

	case 4:
		GFL_MSG_GetString(	wk->mm, (wk->ImParent)? DEBUG_TAYA_STR_PARENT : DEBUG_TAYA_STR_CHILD, wk->strbuf );
		PRINT_UTIL_Print( wk->printUtil, wk->printQue, 0, 0, wk->strbuf, wk->fontHandle );
//		GFL_BMPWIN_TransVramCharacter( wk->win );
		wk->packet.strNum = 0;
		wk->packet.yofs = 30;
		(*seq)++;
		break;

	case 5:
		if( wk->ImParent )
		{
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
			{
				GFL_NET_SendData( wk->netHandle, GFL_NET_CMD_COMMAND_MAX, &(wk->packet) );
				(*seq)++;
				break;
			}
			#if 0
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
			{
				wk->netTestSeq = 0;
				GFL_NET_Exit( autoConnectCallBack );
				(*seq) = 100;
				break;
			}
			#endif
		}
		else
		{
			(*seq)++;
		}
		break;

	case 6:
		if( wk->netTestSeq )
		{
			wk->netTestSeq = 0;
			GFL_NET_TimingSyncStart( wk->netHandle, TEST_TIMINGID_PRINT );
			(*seq)++;
		}
		break;

	case 7:
		if( GFL_NET_IsTimingSync(wk->netHandle, TEST_TIMINGID_PRINT) )
		{
			GFL_MSG_GetString( wk->mm, strID[wk->packet.strNum], wk->strbuf );
			wk->printStream = PRINTSYS_PrintStream( wk->win, 0, wk->packet.yofs,
							wk->strbuf, wk->fontHandle, 0, wk->tcbl, 0, wk->heapID, 0xff );
			(*seq)++;
		}
		break;

	case 8:
		if( PRINTSYS_PrintStreamGetState(wk->printStream) == PRINTSTREAM_STATE_DONE )
		{
			PRINTSYS_PrintStreamDelete( wk->printStream );
			(*seq)++;
		}
		break;

	case 9:
		if( wk->ImParent )
		{
			if( ++(wk->packet.strNum) <= NELEMS(strID) )
			{
				wk->packet.yofs += 16;
				if( strID[wk->packet.strNum] == DEBUG_TAYA_STR09 )
				{
					wk->packet.yofs += 16;
				}
			}
			else
			{
				wk->packet.kanjiMode = !(wk->packet.kanjiMode);
			}
			GFL_NET_SendData( wk->netHandle, GFL_NET_CMD_COMMAND_MAX, &(wk->packet) );
		}
		(*seq)++;
		break;

	case 10:
		if( wk->netTestSeq )
		{
			wk->netTestSeq = 0;

			if( wk->kanjiMode != wk->packet.kanjiMode )
			{
				wk->kanjiMode = wk->packet.kanjiMode;
				GFL_MSG_SetLangID( wk->kanjiMode );

				GFL_BMP_Clear( wk->bmp, 0xff );
				GFL_BMPWIN_TransVramCharacter( wk->win );

				(*seq)++;
			}
			else
			{
				(*seq) = 5;
			}
		}
		break;

	case 11:
		GFL_MSG_GetString( wk->mm, DEBUG_TAYA_STR10, wk->strbuf );
		PRINT_UTIL_Print( wk->printUtil, wk->printQue, 0, 0, wk->strbuf, wk->fontHandle );
		(*seq)++;
		break;

	case 12:
		if( wk->ImParent )
		{
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
			{
				GFL_NET_SendData( wk->netHandle, GFL_NET_CMD_COMMAND_MAX, &(wk->packet) );
				(*seq)++;
			}
			break;
		}
		(*seq)++;
		break;

	case 13:
		if( wk->netTestSeq )
		{
			wk->netTestSeq = 0;
			GFL_BMP_Clear( wk->bmp, 0xff );
			(*seq)=4;
		}
		break;

	case 100:
		if( wk->netTestSeq )
		{
			wk->netTestSeq = 0;
			(*seq)++;
		}
		break;

	case 101:
		return TRUE;

	}

    return FALSE;
}

