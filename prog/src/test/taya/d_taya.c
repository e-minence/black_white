//============================================================================================
/**
 * @file	d_taya.c
 * @brief	デバッグ用関数
 * @author	taya
 * @date	2008.08.01
 */
//============================================================================================

// lib includes -----------------------
#include <gflib.h>
#include <assert_default.h>
#include <procsys.h>
#include <tcbl.h>
#include <msgdata.h>

#include <npBphImporter.h>
#include <npBpcImporter.h>
#include <nplibc.h>


// global includes --------------------
#include "system\main.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "poke_tool\pokeparty.h"
#include "poke_tool\poke_tool.h"
#include "net\network_define.h"

// local includes ---------------------
#include "msg\msg_d_taya.h"

// archive includes -------------------
#include "arc_def.h"
#include "message.naix"
#include "test_graphic\d_taya.naix"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	GENERIC_WORK_SIZE = 1024,

	HEAPID_CORE = HEAPID_TAYA_DEBUG,
	HEAPID_TEMP = HEAPID_TAYA_DEBUG_SUB,
};

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef BOOL (*pSubProc)( GFL_PROC*, int*, void*, void* );

typedef struct _V_MENU_CTRL		V_MENU_CTRL;

struct _V_MENU_CTRL {
	s16   selPos;
	s16   writePos;
	s16   maxLines;
	s16   maxElems;
};

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

	GFL_FONT*				fontHandle;
	PRINT_STREAM*			printStream;
	PRINT_QUE*				printQue;
	PRINT_UTIL				printUtil[1];

	pSubProc		subProc;
	int				subSeq;

	V_MENU_CTRL		menuCtrl;

	u8				genericWork[ GENERIC_WORK_SIZE ];

//	PRINT_STREAM_HANDLE	psHandle;

	GFLNetInitializeStruct		netInitWork;
	int							netTestSeq;
	GFL_NETHANDLE*				netHandle;
	BOOL						ImParent;

	TEST_PACKET			packet;

}MAIN_WORK;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static GFL_PROC_RESULT DebugTayaMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void initGraphicSystems( MAIN_WORK* wk );
static void quitGraphicSystems( MAIN_WORK* wk );
static void startView( MAIN_WORK* wk );
static void createTemporaryModules( MAIN_WORK* wk );
static void deleteTemporaryModules( MAIN_WORK* wk );
static GFL_PROC_RESULT DebugTayaMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void VMENU_Init( V_MENU_CTRL* ctrl, u8 maxLines, u8 maxElems );
static BOOL VMENU_Ctrl( V_MENU_CTRL* ctrl );
static u8 VMENU_GetSelPos( const V_MENU_CTRL* ctrl );
static u8 VMENU_GetWritePos( const V_MENU_CTRL* ctrl );
static void print_menu( MAIN_WORK* wk, const V_MENU_CTRL* menuCtrl );
static void* getGenericWork( MAIN_WORK* mainWork, u32 size );
static BOOL SUBPROC_PrintTest( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT DebugTayaMainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void testPacketFunc( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static void* testBeaconGetFunc( void* pWork );
static int testBeaconGetSizeFunc( void* pWork );
static BOOL testBeaconCompFunc( GameServiceID myNo, GameServiceID beaconNo );
static void testCallBack(void* pWork);
static void autoConnectCallBack( void* pWork );
static BOOL SUBPROC_GoBattle( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static void setup_party( HEAPID heapID, POKEPARTY* party, ... );
static BOOL SUBPROC_NetPrintTest( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static void* bmt_alloc( HEAPID heapID, u32 size );
static void bmt_free( void* adrs );
static BOOL SUBPROC_BlendMagic( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static void Bmt_initSystems( void );
static BOOL Bmt_initEffect( npCONTEXT **pCtx, npSYSTEM **pSys, void **ppvBuf);
static void Bmt_releaseEffect( npCONTEXT *pCtx, npSYSTEM *pSys, void **ppvBuf);
static int Bmt_loadTextures( npCONTEXT* pCtx, npTEXTURE** pTex, npPARTICLEEMITCYCLE** ppEmit, npPARTICLECOMPOSITE** ppComp );
static void bmt_loadBpc( const char *pszBpc, npPARTICLEEMITCYCLE **ppEmit );
static void bmt_loadTex(const char *pszTex, const char *pszPlt, npTEXTURE *pTex, npU32* texSize, npU32* palSize );
static void Bmt_releaseTextures( npPARTICLECOMPOSITE* pComp, npCONTEXT* pCtx, npTEXTURE** pTex, int textureCount );
static void Bmt_update( npCONTEXT* ctx, npPARTICLECOMPOSITE* pComp, npTEXTURE** pTex );
static void bmt_Mtx43ToMtx44(MtxFx44 *pDst, MtxFx43 *pTrc);
static void bmt_updateEffect( npPARTICLECOMPOSITE* pComp,npU32 renew );



/*--------------------------------------------------------------------------*/
/* Menu Table                                                               */
/*--------------------------------------------------------------------------*/
static const struct {
	u32			strID;
	pSubProc	subProc;
}MainMenuTbl[] = {
	{ DEBUG_TAYA_MENU1,		SUBPROC_GoBattle },
	{ DEBUG_TAYA_MENU2,		SUBPROC_NetPrintTest },
	{ DEBUG_TAYA_MENU3,		SUBPROC_BlendMagic   },
	{ DEBUG_TAYA_MENU4,		SUBPROC_PrintTest    },
};

enum {
	MAINMENU_DISP_MAX = 12,
	MAINMENU_PRINT_OX = 0,
	MAINMENU_PRINT_OY = 0,
};


//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugTayaMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	MAIN_WORK* wk;

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_CORE,    0x4000 );
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TEMP,   0xb0000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(MAIN_WORK), HEAPID_CORE );
	wk->heapID = HEAPID_TEMP;

	initGraphicSystems( wk );
	createTemporaryModules( wk );
	startView( wk );

	VMENU_Init( &wk->menuCtrl, MAINMENU_DISP_MAX, NELEMS(MainMenuTbl) );
	wk->seq = 0;
	wk->subProc = NULL;

	return GFL_PROC_RES_FINISH;

}

static void initGraphicSystems( MAIN_WORK* wk )
{
	static const GFL_BG_DISPVRAM vramBank = {
		GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_128_B,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_0123_E			// テクスチャパレットスロット
	};

	GFL_DISP_SetBank( &vramBank );

	// 各種効果レジスタ初期化
	G2_BlendNone();
	G2S_BlendNone();

	// 上下画面設定
	GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

	GFL_BG_Init( wk->heapID );
	GFL_BMPWIN_Init( wk->heapID );
	GFL_FONTSYS_Init();

	//ＢＧモード設定
	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysHeader );
	}

	// 個別フレーム設定
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
}

static void quitGraphicSystems( MAIN_WORK* wk )
{
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );

	GFL_BMPWIN_Exit();
	GFL_BG_Exit();
}

static void startView( MAIN_WORK* wk )
{
	GFL_BMP_Clear( wk->bmp, 0xff );
	GFL_BMPWIN_MakeScreen( wk->win );

	GFL_BMPWIN_TransVramCharacter( wk->win );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
}

static void createTemporaryModules( MAIN_WORK* wk )
{
	wk->win = GFL_BMPWIN_Create( GFL_BG_FRAME0_S, 0, 0, 32, 24, 0, GFL_BMP_CHRAREA_GET_F );
	wk->bmp = GFL_BMPWIN_GetBmp(wk->win);

	wk->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_taya_dat, wk->heapID );
	wk->strbuf = GFL_STR_CreateBuffer( 1024, wk->heapID );
	wk->tcbl = GFL_TCBL_Init( wk->heapID, wk->heapID, 4, 32 );

	wk->fontHandle = GFL_FONT_Create( ARCID_D_TAYA, NARC_d_taya_lc12_2bit_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	wk->printQue = PRINTSYS_QUE_Create( wk->heapID );

	PRINT_UTIL_Setup( wk->printUtil, wk->win );
}

static void deleteTemporaryModules( MAIN_WORK* wk )
{
	PRINTSYS_QUE_Delete( wk->printQue );
	GFL_FONT_Delete( wk->fontHandle );

	GFL_TCBL_Exit( wk->tcbl );
	GFL_STR_DeleteBuffer( wk->strbuf );
	GFL_MSG_Delete( wk->mm );

	GFL_BMPWIN_Delete( wk->win );
}



//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugTayaMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	MAIN_WORK* wk = mywk;

	PRINTSYS_QUE_Main( wk->printQue );

	if( PRINT_UTIL_Trans( wk->printUtil, wk->printQue ) )
	{
		return FALSE;
	}

	switch( *seq ){
	case 0:
		print_menu( wk, &wk->menuCtrl );
		(*seq)++;
		break;

	case 1:
		if( VMENU_Ctrl(&wk->menuCtrl) )
		{
			print_menu( wk, &wk->menuCtrl );
			break;
		}

		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
		{
			GFL_MSGSYS_SetLangID( !GFL_MSGSYS_GetLangID() );
			print_menu( wk, &wk->menuCtrl );
			break;
		}

		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
		{
			u16 p = VMENU_GetSelPos( &wk->menuCtrl );
			wk->subProc = MainMenuTbl[p].subProc;
			wk->subSeq = 0;
			(*seq)++;
		}
		break;

	case 2:
		if( wk->subProc( proc, &(wk->subSeq), pwk, mywk ) )
		{
			wk->subProc = NULL;
			*seq = 0;
		}
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}

//-------------------------------------------------------------------------------------------------------
// メニューコントローラ
//-------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * 縦メニューコントローラ初期化
 *
 * @param   ctrl			コントローラ
 * @param   maxLines		表示可能な行数
 * @param   maxElems		選択可能な項目数
 *
 */
//--------------------------------------------------------------------------
static void VMENU_Init( V_MENU_CTRL* ctrl, u8 maxLines, u8 maxElems )
{
	ctrl->selPos = 0;
	ctrl->writePos = 0;
	ctrl->maxElems = maxElems;
	ctrl->maxLines = maxLines;
}
//--------------------------------------------------------------------------
/**
 * 縦メニューコントローラ　キーチェック
 *
 * @param   ctrl		コントローラ
 *
 * @retval  BOOL		選択項目の変更があればTRUE
 */
//--------------------------------------------------------------------------
static BOOL VMENU_Ctrl( V_MENU_CTRL* ctrl )
{
	u16 key = GFL_UI_KEY_GetRepeat();

	do {

		if( key & PAD_KEY_DOWN )
		{
			if( ctrl->selPos < (ctrl->maxElems-1) )
			{
				if( ++(ctrl->selPos) >= (ctrl->writePos + ctrl->maxLines) )
				{
					ctrl->writePos++;
				}
				break;
			}
		}

		if( key & PAD_KEY_UP )
		{
			if( ctrl->selPos )
			{
				if( --(ctrl->selPos) < ctrl->writePos )
				{
					--(ctrl->writePos);
				}
				break;
			}
		}

		return FALSE;

	}while(0);

	return TRUE;
}

static u8 VMENU_GetSelPos( const V_MENU_CTRL* ctrl )
{
	return ctrl->selPos;
}
static u8 VMENU_GetWritePos( const V_MENU_CTRL* ctrl )
{
	return ctrl->writePos;
}


//--------------------------------------------------------------------------
/**
 * メニュー項目描画
 *
 * @param   wk		
 *
 */
//--------------------------------------------------------------------------
static void print_menu( MAIN_WORK* wk, const V_MENU_CTRL* menuCtrl )
{
	u16 selPos, writePos;
	u16 ypos;

	selPos = VMENU_GetSelPos( menuCtrl );
	writePos = VMENU_GetWritePos( menuCtrl );

	GFL_BMP_Clear( wk->bmp, 0xff );

	ypos = MAINMENU_PRINT_OY;

	while( writePos < NELEMS(MainMenuTbl) )
	{
		GFL_MSG_GetString( wk->mm, MainMenuTbl[writePos].strID, wk->strbuf );

		if( writePos == selPos ){ GFL_FONTSYS_SetColor( 4, 5, 0 ); }

		PRINTSYS_Print( wk->bmp, MAINMENU_PRINT_OX, ypos, wk->strbuf, wk->fontHandle );

		if( writePos == selPos ){ GFL_FONTSYS_SetDefaultColor(); }

		ypos += 16;
		writePos++;
	}

	GFL_BMPWIN_TransVramCharacter( wk->win );
}

//------------------------------------------------------------------------------------------------------
// メインワーク関数
//------------------------------------------------------------------------------------------------------
static void* getGenericWork( MAIN_WORK* mainWork, u32 size )
{
	GF_ASSERT(size<GENERIC_WORK_SIZE);
	return mainWork->genericWork;
}

//------------------------------------------------------------------------------------------------------
// スタンドアロン状態での漢字PrintTest
//------------------------------------------------------------------------------------------------------
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


	enum {
		RAND_MAX_NUM = 100,
		RAND_TEST_UNIT = 10000,
		RAND_TEST_COUNT = RAND_MAX_NUM * RAND_TEST_UNIT,
		RAND_TEST_DOWN_VAL = RAND_MAX_NUM/10,
		RAND_TEST_UP_VAL = RAND_MAX_NUM - RAND_TEST_DOWN_VAL,
	};

	typedef struct {

		u16  count[RAND_MAX_NUM];

	}SUBWORK;

	MAIN_WORK* wk = mywk;
	SUBWORK* sub = getGenericWork( wk, sizeof(SUBWORK) );

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
			GF_ASSERT(0);
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
				GFL_MSGSYS_SetLangID( wk->kanjiMode );
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
//    { testPacketFunc, GFL_NET_COMMAND_SIZE(sizeof(TEST_PACKET)), NULL },
    { testPacketFunc, NULL },
};

typedef struct{
    int gameNo;   ///< ゲーム種類
}TEST_BCON;

static TEST_BCON testBcon = { WB_NET_SERVICEID_DEBUG_TAYA };



static const GFLNetInitializeStruct testNetInitParam = {
	testPacketTbl,				// 受信関数テーブル
	NELEMS(testPacketTbl),		// 受信テーブル要素数
    NULL,    ///< ハードで接続した時に呼ばれる
    NULL,    ///< ネゴシエーション完了時にコール
	NULL,						// ユーザー同士が交換するデータのポインタ取得関数
	NULL,						// ユーザー同士が交換するデータのサイズ取得関数
	testBeaconGetFunc,			// ビーコンデータ取得関数
	testBeaconGetSizeFunc,		// ビーコンデータサイズ取得関数
	testBeaconCompFunc,			// ビーコンのサービスを比較して繋いで良いかどうか判断する
	FatalError_Disp,			// 通信不能なエラーが起こった場合呼ばれる 切断するしかない
	NULL,						// 通信切断時に呼ばれる関数
	NULL,						// オート接続で親になった場合
	TEST_GGID,					// ggid  DP=0x333,RANGER=0x178,WII=0x346
	GFL_HEAPID_APP,				//元になるheapid
	HEAPID_NETWORK,				//通信用にcreateされるHEAPID
	HEAPID_WIFI,				//wifi用にcreateされるHEAPID
	GFL_WICON_POSX,				// 通信アイコンXY位置
	GFL_WICON_POSY,
	TEST_COMM_MEMBER_MAX,		// 最大接続人数
	TEST_COMM_SEND_SIZE_MAX,	//最大送信バイト数
	TEST_COMM_BCON_MAX,			// 最大ビーコン収集数
	TRUE,						// CRC計算
	FALSE,						// MP通信＝親子型通信モードかどうか
	FALSE,						// wifi通信を行うかどうか
	TRUE,						// 親が再度初期化した場合、つながらないようにする場合TRUE
	WB_NET_SERVICEID_DEBUG_TAYA,//GameServiceID
};

static void testPacketFunc( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	MAIN_WORK* wk = pWork;
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

///< ビーコンデータ取得関数
static void* testBeaconGetFunc( void* pWork )
{
	return &testBcon;
}
///< ビーコンデータサイズ取得関数
static int testBeaconGetSizeFunc( void* pWork )
{
	return sizeof(testBcon);
}

///< ビーコンデータ比較関数
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
	MAIN_WORK* wk = pWork;
	wk->netTestSeq = 1;
}

//------------------------------------------------------------------------------------------------------
// バトル画面へ                  
//------------------------------------------------------------------------------------------------------

#include "battle/battle.h"
#include "poke_tool/monsno_def.h"

static BOOL SUBPROC_GoBattle( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	MAIN_WORK* wk = mywk;

	switch( *seq ){
	case 0:
		deleteTemporaryModules( wk );
		quitGraphicSystems( wk );
		GFL_HEAP_DeleteHeap( HEAPID_TEMP );
		(*seq)++;
		break;
	case 1:
		{
			BATTLE_SETUP_PARAM* para = getGenericWork( wk, sizeof(BATTLE_SETUP_PARAM) );

			para->engine = BTL_ENGINE_ALONE;
			para->rule = BTL_RULE_SINGLE;
			para->competitor = BTL_COMPETITOR_WILD;

			para->netHandle = NULL;
			para->commMode = BTL_COMM_NONE;
			para->commPos = 0;
			para->netID = 0;


			para->partyPlayer = PokeParty_AllocPartyWork( HEAPID_CORE );	///< プレイヤーのパーティ
			para->partyEnemy1 = PokeParty_AllocPartyWork( HEAPID_CORE );	///< 1vs1時の敵AI, 2vs2時の１番目敵AI用
			para->partyPartner = NULL;	///< 2vs2時の味方AI（不要ならnull）
			para->partyEnemy2 = NULL;	///< 2vs2時の２番目敵AI用（不要ならnull）

			setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_GYARADOSU, MONSNO_PIKATYUU, MONSNO_RIZAADON, 0 );
			setup_party( HEAPID_CORE, para->partyEnemy1, MONSNO_YADOKINGU, MONSNO_METAGUROSU, MONSNO_SUTAAMII, 0 );

			GFL_PROC_SysCallProc( NO_OVERLAY_ID, &BtlProcData, para );
			(*seq)++;
		}
		break;
	case 2:
		break;
	}

	return FALSE;

}

static void setup_party( HEAPID heapID, POKEPARTY* party, ... )
{
	va_list  list;
	int monsno;
	POKEMON_PARAM* pp;

	va_start( list, party );
	while( 1 )
	{
		monsno = va_arg( list, int );
		if( monsno )
		{
			TAYA_Printf("Create MonsNo=%d\n", monsno);
			pp = PP_Create( monsno, 50, 3594, heapID );
			PokeParty_Add( party, pp );
		}
		else
		{
			break;
		}
	}
	va_end( list );
}


//------------------------------------------------------------------------------------------------------
// 通信状態での漢字PrintTest
//------------------------------------------------------------------------------------------------------
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

	MAIN_WORK* wk = mywk;

	GFL_TCBL_Main( wk->tcbl );

	if( PRINT_UTIL_Trans(wk->printUtil, wk->printQue) )
	{
		return FALSE;
	}

	switch( *seq ){
	case 0:
		GFL_MSG_GetString( wk->mm, DEBUG_TAYA_WAIT, wk->strbuf );
		GFL_BMP_Clear( wk->bmp, 0xff );
		PRINT_UTIL_Print( wk->printUtil, wk->printQue, 0, 0, wk->strbuf, wk->fontHandle );
		wk->netInitWork = testNetInitParam;
		wk->netTestSeq = 0;
		wk->kanjiMode = 0;
		wk->packet.kanjiMode = 0;
		GFL_NET_Init(&(wk->netInitWork), testCallBack, (void*)wk);
		(*seq)++;
	    break;

	case 1:
		if( GFL_NET_IsInit() )
		{
			GFL_NET_ChangeoverConnect( autoConnectCallBack ); // 自動接続
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
		GFL_BMP_Clear( wk->bmp, 0xff );
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
				GFL_NET_SendData( wk->netHandle, GFL_NET_CMD_COMMAND_MAX, sizeof(TEST_PACKET), &(wk->packet) );
				(*seq)++;
				break;
			}
			else
			{
				GFL_BMP_Fill( wk->bmp, 0, 0, 256, 16, 0xff );
				PRINT_UTIL_Print( wk->printUtil, wk->printQue, 0, 0, wk->strbuf, wk->fontHandle );
				PRINT_UTIL_Print( wk->printUtil, wk->printQue, 0, 16, wk->strbuf, wk->fontHandle );
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
		else
		{
			GFL_BMP_Fill( wk->bmp, 0, 0, 256, 16, 0xff );
			PRINT_UTIL_Print( wk->printUtil, wk->printQue, 0, 0, wk->strbuf, wk->fontHandle );
			PRINT_UTIL_Print( wk->printUtil, wk->printQue, 0, 16, wk->strbuf, wk->fontHandle );
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
			GFL_NET_SendData( wk->netHandle, GFL_NET_CMD_COMMAND_MAX,sizeof(TEST_PACKET), &(wk->packet) );
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
				GFL_MSGSYS_SetLangID( wk->kanjiMode );

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
				GFL_NET_SendData( wk->netHandle, GFL_NET_CMD_COMMAND_MAX, sizeof(TEST_PACKET), &(wk->packet) );
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

//------------------------------------------------------------------------------------------------------
// BlendMagic 表示テスト
//------------------------------------------------------------------------------------------------------

#define DEF_BPCFILE "/BMSample/aura_02.bpc"

enum {
	DEF_NP_TEXTURE_MAX =	 16,
	DEF_ENTRYTEX_MAX =		  3,
};

/*
	BlendMagicSDKforDS の実装について
	-----------------------------------------------------------

	実装のステップとして７つに分けることができます。
	１．ライブラリの初期化、メモリ確保
	２．ファイルのロード
	３．エフェクトの生成
	４．エフェクトのアップデート
	５．エフェクトの描画
	６．エフェクトの開放
	７．ライブラリの終了処理

	ここで各ステップの詳細・注意事項を記します。
	■ステップ１．ライブラリの初期化、メモリ確保（ initSystem(),initEffect() ）■■
	-----------------------------------------------------------
	インクルードヘッダーは以下のものです。
	#include "npfx.h"
	#include "nplibc.h"

	基本的に最初に一回だけコールします。
	やっていることは表題のとおりライブラリの初期化とメモリ確保です。
	その際持っていて欲しい３つハンドルがあります。(変数名はサンプルと同じ)
	void*		pvBuf	エフェクトのメモリを malloc したときのハンドルです。最後に開放する必要があります
	npSYSTEM	sys		初期化、終了処理のとき必要です
	npCONTEXT	ctx		初期化、終了処理、ロード、描画のときに必要です

	どれだけメモリが必要かは
	npU32 npParticleGetUsingComponentSize()
	npU32 npParticleGetUsingSubjectSize()
	でどれだけ使っているか確認しながら決めてください。
	（NP_DEBUG をプリプロセッサで設定してください。現状デバッグバージョンしか使えなくしています。）

	実際の初期化、メモリ確保はサンプルをご覧ください。

	■ステップ２．ファイルのロード（ npUtilParticleLoadBph(),loadBpc() ） ■■
	-----------------------------------------------------------
	インクルードヘッダーとして以下があります。
	#include "npBphImporter.h"
	#include "npBpcImporter.h"

	読み込むべきファイルは「.bph」「.bpc」です。
	●エフェクトデータロード
	まず「.bph」ファイルを読み込みます。これから得られた「.bpc」ファイル名から「.bpc」ファイルをロードします。
	この際引数に
	npPARTICLEEMITCYCLE*	pEmit
	という構造体を引数として渡すのですが、これがエフェクトリソースの素です。
	ロードの度にnpPARTICLEEMITCYCLE*を作り、いろんなエフェクトのリソースを確保しておくことになります。

	テクスチャのロードでは
	「.bph」ファイル名から読み込んだテクスチャの名前からテクスチャをロードします。
	読み込んだテクスチャはnpTEXTURE*で持っておき、各エフェクトごとに使いまわします。

	■ステップ３．エフェクトの生成（ npParticleCreateComposite() ）■■
	-----------------------------------------------------------
	エフェクトを生成するにはファイルをロードしたときに作った
	npPARTICLEEMITCYCLE*	pEmit
	とそれをもとに描画の時に必要な
	npPARTICLECOMPOSITE*	pComp
	という構造体が必要になります。これを使って
	npParticleCreateComposite( pEmit, &pComp );
	という関数をコールするだけです。
	注意：同じエフェクトでも違う npPARTICLECOMPOSITE* が必要になります。一つ一つのエフェクトは区別しなければいけません。

	■ステップ４．エフェクトのアップデート（　effUpdate(npPARTICLECOMPOSITE*, npU32)　）■■
	-----------------------------------------------------------
	注意：位置、スケール、回転についてのセットの仕方
		　スケールに関しては別窓口が用意されています。
		　npParticleSetScaling( npPARTICLECOMPOSITE* ,npFVECTOR* );
		　平行移動、回転はマトリックスとして
		　npParticleSetGlobalFMATRIX( npPARTICLEOBJECT* ,npFMATRIX );

	アップデート関数は以下のものですが、第二引数にフレームレートをセットしてください。
	（NP_NTSC60 ,NP_NTSC30 または正の整数をセットすることでデルタタイムに対応できます）
	npParticleUpdateComposite( pComp, NP_NTSC60 );

	■ステップ５．エフェクトの描画（　npParticleRenderComposite()　）■■
	-----------------------------------------------------------
	描画関数は以下のものです。この際、テクスチャリストとその数が必要になります。
	npParticleRenderComposite( &ctx, pComp, &pTex, 1 );

	■ステップ６．エフェクトの開放（　npParticleReleaseComposite()　）■■
	-----------------------------------------------------------
	使わなくなったエフェクトはちゃんと開放しましょう。	放っておくとメモリが足りなくなります。
	npParticleReleaseComposite( pComp );


	■ステップ７．ライブラリの終了処理（　releaseEffect()　）■■
	-----------------------------------------------------------
	エフェクトメモリの開放とライブラリの終了処理です。

*/

static void* bmt_alloc( HEAPID heapID, u32 size )
{
	return GFL_HEAP_AllocClearMemory( heapID, size );
}
static void bmt_free( void* adrs )
{
	GFL_HEAP_FreeMemory( adrs );
}


static BOOL SUBPROC_BlendMagic( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	typedef struct {
		npCONTEXT*	ctx;
		npCONTEXT	ctxBody;
		npSYSTEM*	sys;
		void		*pvBuf;
		int			textureCount;

	 	npPARTICLEEMITCYCLE *pEmit;
		npPARTICLECOMPOSITE *pComp;
		npTEXTURE*			pTex[ DEF_NP_TEXTURE_MAX ];

	}SUBWORK;

	MAIN_WORK* mainWork = mywk;
	SUBWORK* wk = getGenericWork( mainWork, sizeof(SUBWORK) );

	switch( *seq ){
	case 0:
		GFL_STD_MemClear( wk, sizeof(SUBWORK) );
		#if 0
		{
			GFL_STD_MemFill( &wk->sys, 0xc3, sizeof(npSYSTEM) );
			GFL_STD_MemFill( &wk->ctx, 0xc3, sizeof(npCONTEXT) );
		}
		#endif
		wk->ctx = &(wk->ctxBody);
		Bmt_initSystems();
		Bmt_initEffect( &wk->ctx, &wk->sys, &wk->pvBuf );
		OS_TPrintf("BM init effect OK.\n");
		wk->textureCount = Bmt_loadTextures( wk->ctx, &wk->pTex[0], &wk->pEmit, &wk->pComp );
		OS_TPrintf("BM load texture OK.\n");
		(*seq)++;
		break;

	case 1:
	    GX_DispOn();
	    GXS_DispOn();
	  	G3X_SetClearColor(GX_RGB(0, 0, 0), 31, 0x7fff, 63, 0);
		G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
		npSetPolygonIDMin( wk->ctx, 30 );
		npSetPolygonIDMax( wk->ctx, 50 );
		(*seq)++;
		break;

	case 2:
		Bmt_update( wk->ctx, wk->pComp, &wk->pTex[0] );
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
		{
			(*seq)++;
		}
		break;

	case 3:
		Bmt_releaseTextures( wk->pComp, wk->ctx, &wk->pTex[0], wk->textureCount );
		Bmt_releaseEffect( wk->ctx, wk->sys, &wk->pvBuf );
		(*seq)++;
		break;

	case 4:
		OS_TPrintf("BM initialize OK.\n");
		return TRUE;
	}
	return FALSE;
}

// DS描画系初期化
static void Bmt_initSystems( void )
{
    G3X_Init();                        // initialize the 3D graphics states
    G3X_InitMtxStack();                // initialize the matrix stack

    GX_SetBankForTex( GX_VRAM_TEX_0_A );			// VRAM-A for texture images
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);	// VRAM-E for texture palettes

	#if 1
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS,    // graphics mode
                       GX_BGMODE_4,    // BGMODE is 4
                       GX_BG0_AS_3D);  // BG #0 is for 3D

    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    G2_SetBG0Priority(0);
    #endif

    G3X_SetShading(GX_SHADING_TOON);   // shading mode is toon
    G3X_AntiAlias(TRUE);               // enable antialias(without additional computing costs)
	G3X_AlphaBlend(TRUE);
    G3_ViewPort(0, 0, 255, 191);       // Viewport

	#if 0
	{
		void *nstart;
		void *heapStart;
		static OSHeapHandle _hdl = NULL;

		nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 2);
		OS_SetMainArenaLo(nstart);
		
	    heapStart = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
		_hdl = OS_CreateHeap(OS_ARENA_MAIN, heapStart, (void *)((u32)heapStart + MAIN_HEAP_SIZE));
		OS_SetCurrentHeap(OS_ARENA_MAIN, _hdl);
	}
	#endif
}

//--------------------------------------------------
// BMシステム初期化:成功時TRUE
//--------------------------------------------------
static BOOL Bmt_initEffect( npCONTEXT **pCtx, npSYSTEM **pSys, void **ppvBuf)
{
	enum {
		TEXTURE_MAX = 			 32,
		SHADER_MAX = 			 16,
		EFF_COMPONENT_MAX = 	128,
		EFF_SUBJECT_MAX = 		128,
		EFF_FKEY_MAX = 			256,
	};

	npSYSTEMDESC	sysDesc;
	npSIZE			nSize	= 0;
	npBYTE*			pByte;
	npERROR			nResult;

	void *pvNativeWnd = NULL;
	void *pvNativeDc = NULL;
	void *pvNativeRc = NULL;

	/* initialize nplibc */
	MI_CpuClear8(&sysDesc, sizeof(npSYSTEMDESC));
	sysDesc.MaxContext = 1;

	nSize = npCheckSystemHeapSize( &sysDesc )			+
		npCheckTextureLibraryHeapSize( TEXTURE_MAX )	+
		npParticleCheckComponentHeapSize( EFF_COMPONENT_MAX ) +
		npParticleCheckFKeyHeapSize( EFF_FKEY_MAX )	+ 
		npParticleCheckSubjectHeapSize( EFF_SUBJECT_MAX );

	OS_TPrintf("BM必要バッファサイズ=0x%x bytes\n", nSize);

	*ppvBuf = bmt_alloc( HEAPID_TEMP, nSize );
	pByte = (npBYTE*)(*ppvBuf);//ポインタを渡しておく

	/* memory size */
	nSize = npCheckSystemHeapSize( &sysDesc );

	/* initialize system */
	OS_TPrintf("[Bmt_InitEffect] pSys Before Init: %p\n", *pSys );
	nResult = npInitSystem( NP_NULL, &sysDesc, pByte, nSize, pSys );
	OS_TPrintf("[Bmt_InitEffect] pSys After  Init: %p\n", *pSys );

	if(nResult != NP_SUCCESS) {
		OS_TPrintf("FAILED : npInitSystem\n");
		return FALSE;
	}

//	*pCtx = NULL;
	/* create rendering context	*/
	OS_TPrintf("[Bmt_InitEffect] pCtx Before Init: %p\n", *pCtx );
	nResult = npCreateContext( (*pSys), NP_NULL, pCtx );
	OS_TPrintf("[Bmt_InitEffect] pCtx After  Init: %p\n", *pCtx );

	if(nResult != NP_SUCCESS) {
		OS_TPrintf("FAILED : npCreateContext\n");
		return FALSE;
	}

	(*pCtx)->m_nWidth = 256;
	(*pCtx)->m_nHeight = 192;

	/* reset rendering context */
	nResult = npResetEnvironment( *pCtx );

	if ( nResult != NP_SUCCESS )
	{
		return FALSE;
	}

	// BackGround Color
	(void)npSetClearColor(*pCtx, 0xFFFF);

	/* set render state */
	npSetRenderState( *pCtx, NP_SHADE_CULLMODE, NP_CULL_NONE );

	pByte += nSize;

	/* initialize texture library */
	nSize = npCheckTextureLibraryHeapSize( TEXTURE_MAX );
	nResult = npInitTextureLibrary( *pCtx, pByte, nSize );
	if ( nResult != NP_SUCCESS )
	{
		OS_Printf("ERROR: TextureLibrary\n");
		return FALSE;
	}
	pByte += nSize;

	/* initialize particle component */
	nSize = npParticleCheckComponentHeapSize( EFF_COMPONENT_MAX );
	nResult = npParticleInitComponentFactory( pByte, nSize );
	if(nResult != NP_SUCCESS) {
		OS_Printf("ERROR: ParticleComponent\n");
		return FALSE;
	}
	pByte += nSize;

	/* initialize particle key frame  */
	nSize = npParticleCheckFKeyHeapSize( EFF_FKEY_MAX );
	nResult = npParticleInitFKeyFactory( pByte, nSize );
	if(nResult != NP_SUCCESS) {
		OS_Printf("ERROR: ParticleComponent\n");
		return FALSE;
	}
	pByte += nSize;

	/* initialize particle subject */
	nSize = npParticleCheckSubjectHeapSize( EFF_SUBJECT_MAX );
	nResult = npParticleInitSubjectFactory( pByte, nSize );
	if(nResult != NP_SUCCESS) {
		OS_Printf("ERROR: ParticleSubject\n");
		return FALSE;
	}
	pByte += nSize;

	return TRUE;
}
//--------------------------------------------------
// BMシステム解放
//--------------------------------------------------
static void Bmt_releaseEffect( npCONTEXT *pCtx, npSYSTEM *pSys, void **ppvBuf)
{
	// subject memory の開放
	npParticleExitSubjectFactory();
	// key frame memory の開放
	npParticleExitFkeyFactory();
	// component memory の開放
	npParticleExitComponentFactory();
	// texture memory の開放
	npExitTextureLibrary( pCtx );
	// context の開放
	npReleaseContext( pCtx );
	// system の開放
	npExitSystem( pSys );
	OS_TPrintf(" pSys ... ExitSistem(%p)\n", pSys);

	bmt_free( *ppvBuf );
	*ppvBuf = NULL;
}
//--------------------------------------------------
// テクスチャロード : return = 読み込みテクスチャ数
//--------------------------------------------------
static int Bmt_loadTextures( npCONTEXT* pCtx, npTEXTURE** pTex, npPARTICLEEMITCYCLE** ppEmit, npPARTICLECOMPOSITE** ppComp )
{
	typedef struct __TEX_LIST
	{
		char		cNtftName[32];
		char		cNtfpName[32];
		npU32			width;
		npU32			height;
		GXTexFmt	fomat;
	} TEX_LIST;

	static const TEX_LIST texList[] = 
	{
		{ "/BmSample/aura_01.ntft"			, "/BmSample/aura_01.ntfp"			, 32, 64,	GX_TEXFMT_A5I3} ,
		{ "/BmSample/sphere_06.ntft"		, "/BmSample/sphere_06.ntfp"		, 256, 256,	GX_TEXFMT_A5I3} ,
		{ "/BmSample/sphere_hole_01.ntft"	, "/BmSample/sphere_hole_01.ntfp"	, 32, 32,	GX_TEXFMT_A5I3} ,
	};

	npU32 ntftAddr = 0;
	npU32 ntfpAddr = 0;
	npU32 texSize, palSize;
	int i;

	bmt_loadBpc( DEF_BPCFILE, ppEmit );

	npParticleCreateComposite( *ppEmit, ppComp );
	if( *ppComp == NULL )
	{
		GF_ASSERT(0);
	}

	// テクスチャファイル・パレットファイルのサイズに注意
	for(i=0; i<DEF_ENTRYTEX_MAX; i++)
	{
		
		npCreateTextureFromRef( pCtx, NULL, &pTex[i] );
		npSetTextureParam(pTex[i], texList[i].width, texList[i].height, texList[i].fomat, ntftAddr, ntfpAddr);	
		bmt_loadTex( texList[i].cNtftName, texList[i].cNtfpName, pTex[i], &texSize, &palSize );
		ntftAddr += texSize;
		ntfpAddr += palSize;
	}
	return i;
}


static void bmt_loadBpc( const char *pszBpc, npPARTICLEEMITCYCLE **ppEmit )
{
	FSFile file;
	unsigned int uiSize;
	void *pv;

	NP_BPC_DESC desc;
	desc.coordinateSystem = NP_BPC_RIGHT_HANDED;

	FS_InitFile(&file);
	if(!FS_OpenFile(&file, pszBpc)) {
		OS_Printf("Cannot Open File %s\n", pszBpc);
		GF_ASSERT(0);
		return;
	}

	uiSize = FS_GetLength(&file);
	pv = bmt_alloc( GFL_HEAP_LOWID(HEAPID_TEMP), uiSize );

	FS_ReadFile( &file, pv, (long)uiSize );
	FS_CloseFile(&file);

	if( !npUtilParticleLoadBpc(pv, uiSize, &desc, ppEmit))
	{
		OS_TPrintf("FAILED: npUtilParticleLoadBpc\n");
		GF_ASSERT(0);
		return;
	}

	GFL_HEAP_FreeMemory( pv );
}

static void bmt_loadTex(const char *pszTex, const char *pszPlt, npTEXTURE *pTex, npU32* texSize, npU32* palSize )
{
	FSFile file;
	int hasPalette = 0;

	*texSize = *palSize = 0;

	// Open Texture File
	FS_InitFile(&file);
	if(FS_OpenFile(&file, pszTex)) {
		u32 uReadSize = FS_GetLength(&file);
		u32 uTexSize = ((uReadSize+3)/4)*4;
		void *pvTex = bmt_alloc( GFL_HEAP_LOWID(HEAPID_TEMP), uTexSize );
		*texSize = uTexSize;
		if(pvTex)
		{
			FS_ReadFile( &file, pvTex, (long)uReadSize );
			FS_CloseFile( &file );

			DC_FlushRange(pvTex, uTexSize);
		    GX_BeginLoadTex();
			GX_LoadTex(pvTex, pTex->m_TexAddr, uTexSize);
		    GX_EndLoadTex();

		    GFL_HEAP_FreeMemory( pvTex );
		    pvTex = NULL;
		}
		else
		{
    		OS_TPrintf("CANNOT ALLOCATE MEMORY\n");
    		GF_ASSERT(0);
		}
	}
	else {
		OS_TPrintf("CANNOT OPEN FILE %s\n", pszTex);
		GF_ASSERT(0);
		return;
	}

	switch(pTex->m_TexFmt) {
	case GX_TEXFMT_PLTT4:
	case GX_TEXFMT_PLTT16:
	case GX_TEXFMT_PLTT256:
	case GX_TEXFMT_A3I5:
	case GX_TEXFMT_A5I3:
		hasPalette = 1;
		break;
	default:
		hasPalette = 0;
		break;
	}
	
	// Open Palette File
	if(hasPalette) {
		FS_InitFile(&file);
		if(FS_OpenFile(&file, pszPlt)) {
			u32 uReadSize = FS_GetLength(&file);
			u32 uPltSize = ((uReadSize+3)/4)*4;
			void *pvPlt = bmt_alloc( GFL_HEAP_LOWID(HEAPID_TEMP), uPltSize );
			*palSize = uPltSize;
			if(pvPlt) {
				FS_ReadFile(&file, pvPlt, (long)uReadSize);
				FS_CloseFile(&file);

				// LoadPalette
				DC_FlushRange(pvPlt, uPltSize);
				GX_BeginLoadTexPltt();
			    GX_LoadTexPltt(pvPlt, pTex->m_PltAddr, uPltSize);
			    GX_EndLoadTexPltt();

			    GFL_HEAP_FreeMemory( pvPlt );
			    pvPlt = NULL;
			}
			else
			{
	    		OS_Printf("CANNOT ALLOCATE MEMORY\n");
	    		GF_ASSERT(0);
			}
		}
		else {
			OS_Printf("CANNOT OPEN FILE %s\n", pszPlt);
			GF_ASSERT(0);
			return;
		}
	}
}
//--------------------------------------------------
// テクスチャ解放
//--------------------------------------------------
static void Bmt_releaseTextures( npPARTICLECOMPOSITE* pComp, npCONTEXT* pCtx, npTEXTURE** pTex, int textureCount )
{
	int i;

	npParticleReleaseComposite( pComp );

	for(i=0; i<textureCount; i++)
	{
		npReleaseTexture( pCtx, pTex[i] );
	}
}
//--------------------------------------------------
// 描画更新
//--------------------------------------------------
static void Bmt_update( npCONTEXT* ctx, npPARTICLECOMPOSITE* pComp, npTEXTURE** pTex )
{
	MtxFx44 matProj, matView;
	MtxFx43 matView43;
	npSIZE	nPolyMax = 63;
	npSIZE	nPolyMin = 0;

	G3X_Reset();

	//---------------------------------------------------------------------------
	// Set up a camera matrix
	//---------------------------------------------------------------------------
	{
		VecFx32 Eye = { 0*FX32_ONE, 0, 10*FX32_ONE };   // Eye position
		VecFx32 at = { 0, 0, 0 };  // Viewpoint
		VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

		fx32 fovSin = FX_SinIdx(182 * 45);
		fx32 fovCos = FX_CosIdx(182 * 45);
		fx32 aspect = FX_F32_TO_FX32(1.33333333f);
		fx32 n = FX32_ONE;
		fx32 f = 500 * FX32_ONE;

		G3_Perspective(fovSin, fovCos, aspect, n, f, &matProj);
		G3_LookAt(&Eye, &vUp, &at, &matView43);

		MTX_Identity44(&matView);
		bmt_Mtx43ToMtx44(&matView, &matView43);
	}

    G3_PushMtx();
    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_Identity();
    // Use an identity matrix for the texture matrix for simplicity
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

	npSetTransformFMATRIX( ctx, NP_PROJECTION, (npFMATRIX *)&matProj );
	npSetTransformFMATRIX( ctx, NP_VIEW, (npFMATRIX *)&matView );

	// UPDATE & RENDER
	{
		bmt_updateEffect( pComp, NP_NTSC60 );
		npParticleRenderComposite( ctx, pComp, pTex, DEF_ENTRYTEX_MAX );
	}

	G3_PopMtx(1);

	// swapping the polygon list RAM, the vertex RAM, etc.
	G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
}

static void bmt_Mtx43ToMtx44(MtxFx44 *pDst, MtxFx43 *pTrc)
{
	int i, j;
	for(i=0; i<4; i++) {
		for(j=0; j<3; j++) {
			pDst->m[i][j] = pTrc->m[i][j];
		}
	}
}


static void bmt_updateEffect( npPARTICLECOMPOSITE* pComp,npU32 renew )
{
	// scalingEffect
	npFMATRIX effMat;
	npUnitFMATRIX( &effMat );
	(void)npParticleSetGlobalFMATRIX((npPARTICLEOBJECT *)pComp, &effMat);
	(void)npParticleUpdateComposite( pComp, renew );
}


