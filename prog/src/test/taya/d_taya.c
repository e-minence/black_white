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
#include <assert_default.h>
#include <procsys.h>
#include <tcbl.h>
#include <msgdata.h>
#include <net.h>

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
#include "battle\battle.h"

// local includes ---------------------
#include "msg\msg_d_taya.h"

// archive includes -------------------
#include "arc_def.h"
#include "message.naix"
#include "test_graphic/d_taya.naix"
#include "font/font.naix"

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
	u16				subArg;

	V_MENU_CTRL		menuCtrl;

	void*			arignDmy;
	u8				genericWork[ GENERIC_WORK_SIZE ];

//	PRINT_STREAM_HANDLE	psHandle;

	GFLNetInitializeStruct	netInitWork;
	int											netTestSeq;
	GFL_NETHANDLE*					netHandle;
	BOOL										ImParent;

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
static void* btlBeaconGetFunc( void* pWork );
static int btlBeaconGetSizeFunc( void* pWork );
static BOOL btlBeaconCompFunc( GameServiceID myNo, GameServiceID beaconNo );
static void btlAutoConnectCallback( void* pWork );
static BOOL SUBPROC_CommBattle( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static BOOL SUBPROC_MultiBattle( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static void setup_party( HEAPID heapID, POKEPARTY* party, ... );
static BOOL SUBPROC_KanjiMode( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
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
	u32				strID;
	pSubProc	subProc;
	u16				subArg;
	u16				quickKey;
}MainMenuTbl[] = {
	{ DEBUG_TAYA_MENU1,		SUBPROC_GoBattle,				0, 0 },
	{ DEBUG_TAYA_MENU2,		SUBPROC_CommBattle,	 		0, 0 },
	{ DEBUG_TAYA_MENU3,		SUBPROC_MultiBattle,		1, PAD_BUTTON_X },
	{ DEBUG_TAYA_MENU4,		SUBPROC_MultiBattle,		0, PAD_BUTTON_Y },
	{ DEBUG_TAYA_MENU5,		SUBPROC_KanjiMode,			0, 0 },
	{ DEBUG_TAYA_MENU6,		SUBPROC_NetPrintTest,		0, 0 },
	{ DEBUG_TAYA_MENU7,		SUBPROC_BlendMagic,			0, 0 },
	{ DEBUG_TAYA_MENU8,		SUBPROC_PrintTest,			0, 0 },
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
	wk->subArg = 0;
	wk->subProc = NULL;


	return GFL_PROC_RES_FINISH;

}

static void initGraphicSystems( MAIN_WORK* wk )
{
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,						// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,	// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,					// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,				// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,						// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_0123_E,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,	// �T�uOBJ�}�b�s���O���[�h
	};

	GFL_DISP_SetBank( &vramBank );

	// �e����ʃ��W�X�^������
	G2_BlendNone();
	G2S_BlendNone();

	// �㉺��ʐݒ�
	GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

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
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0, 0, wk->heapID );
//		void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
		GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0xff, 1, 0 );
		GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 0, 0, wk->heapID );
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

	GFL_HEAP_CheckHeapSafe( wk->heapID );
	wk->fontHandle = GFL_FONT_Create( ARCID_FONT,
	#if 0
		NARC_d_taya_lc12_2bit_nftr,
	#else
		NARC_font_small_nftr,
	#endif
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

	if( !PRINT_UTIL_Trans( wk->printUtil, wk->printQue ) )
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
			wk->subArg = MainMenuTbl[p].subArg;
			wk->subSeq = 0;
			(*seq)++;
		}
		else
		{
			u16 key = GFL_UI_KEY_GetTrg();

			if( key & PAD_BUTTON_B ){ key = PAD_BUTTON_B; }
			else if( key & PAD_BUTTON_X ){ key = PAD_BUTTON_X; }
			else if( key & PAD_BUTTON_Y ){ key = PAD_BUTTON_Y; }
			else{ key = 0; }

			if( key )
			{
				int i;
				for(i=0; i<NELEMS(MainMenuTbl); ++i)
				{
					if( MainMenuTbl[i].quickKey == key )
					{
						wk->subProc = MainMenuTbl[i].subProc;
						wk->subArg = MainMenuTbl[i].subArg;
						wk->subSeq = 0;
						(*seq)++;
						break;
					}
				}
			}

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
// ���j���[�R���g���[��
//-------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * �c���j���[�R���g���[��������
 *
 * @param   ctrl			�R���g���[��
 * @param   maxLines		�\���\�ȍs��
 * @param   maxElems		�I���\�ȍ��ڐ�
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
 * �c���j���[�R���g���[���@�L�[�`�F�b�N
 *
 * @param   ctrl		�R���g���[��
 *
 * @retval  BOOL		�I�����ڂ̕ύX�������TRUE
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
 * ���j���[���ڕ`��
 *
 * @param   wk		
 *
 */
//--------------------------------------------------------------------------
static void print_menu( MAIN_WORK* wk, const V_MENU_CTRL* menuCtrl )
{
	u16 selPos, writePos;
	u16 ypos;

	u8 fontCol;

	selPos = VMENU_GetSelPos( menuCtrl );
	writePos = VMENU_GetWritePos( menuCtrl );

	GFL_BMP_Clear( wk->bmp, 0xff );

	ypos = MAINMENU_PRINT_OY;

	while( writePos < NELEMS(MainMenuTbl) )
	{
		GFL_MSG_GetString( wk->mm, MainMenuTbl[writePos].strID, wk->strbuf );

		if( writePos == selPos )
		{
			fontCol = 0x03;
		}
		else
		{
			switch( MainMenuTbl[writePos].quickKey ){
			case PAD_BUTTON_X: fontCol = 0x05; break;
			case PAD_BUTTON_Y: fontCol = 0x09; break;
			case PAD_BUTTON_B: fontCol = 0x0b; break;
			default: fontCol = 0x01; break;
			}
		}

		GFL_FONTSYS_SetColor( fontCol, 2, 0x0f );

		PRINTSYS_Print( wk->bmp, MAINMENU_PRINT_OX, ypos, wk->strbuf, wk->fontHandle );
		GFL_FONTSYS_SetDefaultColor();

		ypos += (GFL_FONT_GetLineHeight(wk->fontHandle)+2);
		writePos++;
	}
	GFL_FONTSYS_SetDefaultColor();

	GFL_BMPWIN_TransVramCharacter( wk->win );
}

//------------------------------------------------------------------------------------------------------
// ���C�����[�N�֐�
//------------------------------------------------------------------------------------------------------
static void* getGenericWork( MAIN_WORK* mainWork, u32 size )
{
	GF_ASSERT(size<GENERIC_WORK_SIZE);
	GFL_STD_MemClear( mainWork->genericWork, size );
	{
		u32 adrs = (u32)(&mainWork->genericWork[0]);
		GF_ASSERT(adrs%4==0);
	}
	return mainWork->genericWork;
}

//------------------------------------------------------------------------------------------------------
// �X�^���h�A������Ԃł̊���PrintTest
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

	if( !PRINT_UTIL_Trans( wk->printUtil, wk->printQue ) )
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
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
		{
			return TRUE;
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
	TEST_MULTI_MEMBER_MAX = 4,
	TEST_COMM_SEND_SIZE_MAX	= 100,
	TEST_COMM_BCON_MAX		= 1,

	TEST_MULTI_BCON_MAX = 4,

	TEST_TIMINGID_INIT		= 11,
	TEST_TIMINGID_PRINT,

};

static const NetRecvFuncTable  testPacketTbl[] = {
//    { testPacketFunc, GFL_NET_COMMAND_SIZE(sizeof(TEST_PACKET)), NULL },
    { testPacketFunc, NULL },
};

typedef struct{
    int gameNo;   ///< �Q�[�����
}TEST_BCON;

static TEST_BCON testBcon = { WB_NET_SERVICEID_DEBUG_TAYA };



static const GFLNetInitializeStruct testNetInitParam = {
	testPacketTbl,				// ��M�֐��e�[�u��
	NELEMS(testPacketTbl),		// ��M�e�[�u���v�f��
	NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
	NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
	NULL,						// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
	NULL,						// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
	testBeaconGetFunc,			// �r�[�R���f�[�^�擾�֐�
	testBeaconGetSizeFunc,		// �r�[�R���f�[�^�T�C�Y�擾�֐�
	testBeaconCompFunc,			// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
	NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
	FatalError_Disp,			// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	NULL,						// �ʐM�ؒf���ɌĂ΂��֐�
	NULL,						// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
	NULL,	///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
	NULL,	///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
	NULL,	///< wifi�t�����h���X�g�폜�R�[���o�b�N
	NULL,	///< DWC�`���̗F�B���X�g	
	NULL,	///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
	0,		///< DWC�ւ�HEAP�T�C�Y
	TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
	TEST_GGID,					// ggid  DP=0x333,RANGER=0x178,WII=0x346
	GFL_HEAPID_APP,				//���ɂȂ�heapid
	HEAPID_NETWORK,				//�ʐM�p��create�����HEAPID
	HEAPID_WIFI,				//wifi�p��create�����HEAPID
	HEAPID_NETWORK,				//IRC�p��create�����HEAPID
	GFL_WICON_POSX,				// �ʐM�A�C�R��XY�ʒu
	GFL_WICON_POSY,
	TEST_COMM_MEMBER_MAX,		// �ő�ڑ��l��
	TEST_COMM_SEND_SIZE_MAX,	// �P�t���[��������̍ő呗�M�o�C�g��
	TEST_COMM_BCON_MAX,			// �ő�r�[�R�����W��
	TRUE,						// CRC�v�Z
	FALSE,						// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
	FALSE,						// wifi�ʐM���s�����ǂ���
	TRUE,						// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	WB_NET_SERVICEID_DEBUG_TAYA,//GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
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

///< �r�[�R���f�[�^�擾�֐�
static void* testBeaconGetFunc( void* pWork )
{
	return &testBcon;
}
///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int testBeaconGetSizeFunc( void* pWork )
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
	MAIN_WORK* wk = pWork;
	wk->netTestSeq = 1;
}

//------------------------------------------------------------------------------------------------------
// �o�g����ʂ�                  
//------------------------------------------------------------------------------------------------------

FS_EXTERN_OVERLAY(battle);

#include "battle\battle.h"
#include "poke_tool\monsno_def.h"
#include "waza_tool\wazano_def.h"
#include "test\performance.h"

//----------------------------------
// �X�^���h�A����
//----------------------------------
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
			para->rule = BTL_RULE_DOUBLE;
			if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
			{
				para->rule = BTL_RULE_SINGLE;
			}
			para->competitor = BTL_COMPETITOR_WILD;

			para->netHandle = NULL;
			para->commMode = BTL_COMM_NONE;
			para->commPos = 0;
			para->netID = 0;


			para->partyPlayer = PokeParty_AllocPartyWork( HEAPID_CORE );	///< �v���C���[�̃p�[�e�B
			para->partyEnemy1 = PokeParty_AllocPartyWork( HEAPID_CORE );	///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
			para->partyPartner = NULL;	///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
			para->partyEnemy2 = NULL;	///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j

		#ifdef DEBUG_ONLY_FOR_taya
			setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_GYARADOSU, MONSNO_PIKATYUU, MONSNO_RIZAADON, 0 );
			setup_party( HEAPID_CORE, para->partyEnemy1, MONSNO_HITOKAGE, MONSNO_BAKUUDA, MONSNO_SUTAAMII, 0 );
		#else
			setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_AUSU + 2, MONSNO_AUSU + 1, 0 );
			setup_party( HEAPID_CORE, para->partyEnemy1, MONSNO_AUSU + 1, MONSNO_AUSU + 2, 0 );
		#endif

			GFL_PROC_SysCallProc( FS_OVERLAY_ID(battle), &BtlProcData, para );
			(*seq)++;
		}
		break;
	case 2:
		GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TEMP,   0xb0000 );
		initGraphicSystems( wk );
		createTemporaryModules( wk );
		startView( wk );
		return TRUE;
	}

	return FALSE;

}

// �o�g���p��M�֐��e�[�u��
extern const NetRecvFuncTable BtlRecvFuncTable[];

typedef struct{
    int gameNo;   ///< �Q�[�����
}BTL_BCON;

static BTL_BCON btlBcon = { WB_NET_BATTLE_SERVICEID };
static BTL_BCON MultiBcon = { WB_NET_BATTLE_SERVICEID };

///< �r�[�R���f�[�^�擾�֐�
static void* btlBeaconGetFunc( void* pWork )
{
	return &btlBcon;
}
///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int btlBeaconGetSizeFunc( void* pWork )
{
	return sizeof(btlBcon);
}

///< �r�[�R���f�[�^��r�֐�
static BOOL btlBeaconCompFunc( GameServiceID myNo, GameServiceID beaconNo )
{
    if( myNo != beaconNo ){
        return FALSE;
    }
    return TRUE;
}

static const GFLNetInitializeStruct btlNetInitParam = {
	BtlRecvFuncTable,			// ��M�֐��e�[�u��
	5,										// ��M�e�[�u���v�f��
	NULL,									// �n�[�h�Őڑ��������ɌĂ΂��
	NULL,									// �l�S�V�G�[�V�����������ɃR�[��
	NULL,									// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
	NULL,									// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
	btlBeaconGetFunc,			// �r�[�R���f�[�^�擾�֐�
	btlBeaconGetSizeFunc,	// �r�[�R���f�[�^�T�C�Y�擾�֐�
	btlBeaconCompFunc,		// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
	NULL,									// ���ʂ̃G���[���N�������ꍇ �ʐM�I��
	FatalError_Disp,			// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	NULL,									// �ʐM�ؒf���ɌĂ΂��֐�
	NULL,									// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
	NULL,	///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
	NULL,	///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
	NULL,	///< wifi�t�����h���X�g�폜�R�[���o�b�N
	NULL,	///< DWC�`���̗F�B���X�g	
	NULL,	///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
	0,		///< DWC�ւ�HEAP�T�C�Y
	TRUE,	///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif
	TEST_GGID,						// ggid  DP=0x333,RANGER=0x178,WII=0x346
	GFL_HEAPID_APP,				//���ɂȂ�heapid
	HEAPID_NETWORK,				//�ʐM�p��create�����HEAPID
	HEAPID_WIFI,					//wifi�p��create�����HEAPID
	HEAPID_NETWORK,				//IRC�p��create�����HEAPID
	GFL_WICON_POSX,				// �ʐM�A�C�R��XY�ʒu
	GFL_WICON_POSY,
	TEST_COMM_MEMBER_MAX,		// �ő�ڑ��l��
	TEST_COMM_SEND_SIZE_MAX,// �ő呗�M�o�C�g��
	TEST_COMM_BCON_MAX,			// �ő�r�[�R�����W��
	TRUE,										// CRC�v�Z
	FALSE,									// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
	GFL_NET_TYPE_WIRELESS,	/// �g�p����ʐM���w��
	TRUE,										// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	WB_NET_BATTLE_SERVICEID,//GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
};

static const GFLNetInitializeStruct btlMultiNetInitParam = {
	BtlRecvFuncTable,		// ��M�֐��e�[�u��
	5,									// ��M�e�[�u���v�f��
	NULL,									///< �n�[�h�Őڑ��������ɌĂ΂��
	NULL,									///< �l�S�V�G�[�V�����������ɃR�[��
	NULL,									// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
	NULL,									// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
	btlBeaconGetFunc,			// �r�[�R���f�[�^�擾�֐�
	btlBeaconGetSizeFunc,	// �r�[�R���f�[�^�T�C�Y�擾�֐�
	btlBeaconCompFunc,		// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
	NULL,									// ���ʂ̃G���[���N�������ꍇ �ʐM�I��
	FatalError_Disp,			// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	NULL,						// �ʐM�ؒf���ɌĂ΂��֐�
	NULL,						// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g	
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif
	TEST_GGID,						// ggid  DP=0x333,RANGER=0x178,WII=0x346
	GFL_HEAPID_APP,				//���ɂȂ�heapid
	HEAPID_NETWORK,				//�ʐM�p��create�����HEAPID
	HEAPID_WIFI,					//wifi�p��create�����HEAPID
	HEAPID_NETWORK,				//IRC�p��create�����HEAPID
	GFL_WICON_POSX,				// �ʐM�A�C�R��XY�ʒu
	GFL_WICON_POSY,
	TEST_MULTI_MEMBER_MAX,		// �ő�ڑ��l��
	TEST_COMM_SEND_SIZE_MAX,	// �ő呗�M�o�C�g��
	TEST_MULTI_BCON_MAX,			// �ő�r�[�R�����W��
	TRUE,											// CRC�v�Z
	FALSE,										// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
	GFL_NET_TYPE_WIRELESS,		// �g�p����ʐM���w��
	TRUE,											// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	WB_NET_BATTLE_SERVICEID,	//GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
};

static void btlAutoConnectCallback( void* pWork )
{
	MAIN_WORK* wk = pWork;

	wk->netTestSeq = 1;
	TAYA_Printf("GFL_NET AutoConnCallBack\n");
}


//----------------------------------
// �ʐM�i�ʏ�j
//----------------------------------
static BOOL SUBPROC_CommBattle( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
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
		GFL_NET_Init( &btlNetInitParam, testCallBack, (void*)wk );
		(*seq)++;
		break;
	case 2:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
		{
			TAYA_Printf("GFL_NET Initi wait ...\n");
		}
		if( GFL_NET_IsInit() )
		{
			GFL_NET_ChangeoverConnect( btlAutoConnectCallback ); // �����ڑ�
			(*seq)++;
		}
		break;
	case 3:
		if( wk->netTestSeq )
		{
			GFL_NET_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), 0 );
			(*seq)++;
		}
		break;
	case 4:
		if( GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), 0) )
		{
			BATTLE_SETUP_PARAM* para = getGenericWork( wk, sizeof(BATTLE_SETUP_PARAM) );

			para->engine = BTL_ENGINE_ALONE;
			para->rule = BTL_RULE_DOUBLE;
			para->competitor = BTL_COMPETITOR_COMM;

			para->netHandle = GFL_NET_HANDLE_GetCurrentHandle();
			para->netID = GFL_NET_GetNetID( para->netHandle );
			para->commMode = BTL_COMM_DS;
			para->commPos = para->netID;
			para->multiMode = 0;

			para->partyPlayer = PokeParty_AllocPartyWork( HEAPID_CORE );	///< �v���C���[�̃p�[�e�B
			para->partyEnemy1 = NULL;		///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
			para->partyPartner = NULL;	///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
			para->partyEnemy2 = NULL;		///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j

			if( para->netID == 0 )
			{
				setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_GYARADOSU, MONSNO_PIKATYUU, MONSNO_RIZAADON, 0 );
			}
			else
			{
				setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_YADOKINGU, MONSNO_METAGUROSU, MONSNO_SUTAAMII, 0 );
			}

			DEBUG_PerformanceSetActive( FALSE );
			GFL_PROC_SysCallProc( FS_OVERLAY_ID(battle), &BtlProcData, para );
			(*seq)++;
		}
		break;
	case 5:
		GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TEMP,   0xb0000 );
		initGraphicSystems( wk );
		createTemporaryModules( wk );
		startView( wk );
		return TRUE;
	}

	return FALSE;
}
//----------------------------------
// �ʐM�i�}���`�j
//----------------------------------
static BOOL SUBPROC_MultiBattle( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	enum {
		MULTI_TIMING_NUMBER = 24,
	};
	MAIN_WORK* wk = mywk;

	switch( *seq ){
	case 0:
		deleteTemporaryModules( wk );
		quitGraphicSystems( wk );
		GFL_HEAP_DeleteHeap( HEAPID_TEMP );
		(*seq)++;
		break;
	case 1:
		GFL_NET_Init( &btlMultiNetInitParam, testCallBack, (void*)wk );
		(*seq)++;
		break;
	case 2:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
		{
			TAYA_Printf("GFL_NET Initi wait ...\n");
		}
		if( GFL_NET_IsInit() )
		{
			(*seq)++;
		}
		break;
	case 3:
		// subArg!=0 -> �e�@��
		if( wk->subArg )
		{
			TAYA_Printf("[D_TAYA] �}���`�e�@�ɂȂ�܂�\n");
			GFL_NET_InitServer();
			(*seq)++;
		}
		// �q�@�Ȃ�r�[�R�����Ђ낢�܂���
		else
		{
			GFL_NET_StartBeaconScan();
			(*seq)++;
		}
		break;
	case 4:
		if( wk->subArg )
		{
			(*seq)++;
		}
		else
		{
			int i;
			BTL_BCON* bcon;

			TAYA_Printf("[D_TAYA] �q�@�Ȃ̂Ńr�[�R���Ђ낢�܂�\n");
			for(i=0; i<TEST_MULTI_BCON_MAX; ++i)
			{
				bcon = GFL_NET_GetBeaconData(i);
				if( bcon )
				{
					if( bcon->gameNo == MultiBcon.gameNo )
					{
						break;
					}
				}
			}
			if( i != TEST_MULTI_BCON_MAX )
			{
				u8* macAdrs = GFL_NET_GetBeaconMacAddress( i );
				if( macAdrs != NULL )
				{
					TAYA_Printf("[D_TAYA] �q�@���}���`�e�@��������\n");
					GFL_NET_ConnectToParent( macAdrs );
					(*seq)++;
				}
			}
		}
		break;

	case 5:
		if( GFL_NET_HANDLE_RequestNegotiation() )
		{
			TAYA_Printf("[D_TAYA] �l�S�V�G�[�V�������������̂Ŏ��� \n");
			(*seq)++;
		}
		break;

	// �S��Ȃ�������V���N���J�n
	case 6:
		if( GFL_NET_GetConnectNum() == TEST_MULTI_MEMBER_MAX )
		{
			GFL_NET_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), MULTI_TIMING_NUMBER );
			TAYA_Printf("[D_TAYA] �}���`�V���N���J�n���܂� ... \n");
			(*seq)++;
		}
		else
		{
			if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )
			{
				u8 n = GFL_NET_GetConnectNum();
				TAYA_Printf("SubArg=%d, conNum=%d\n", wk->subArg, n);
			}
		}
		break;

	case 7:
		if( GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), MULTI_TIMING_NUMBER) )
		{
			BATTLE_SETUP_PARAM* para = getGenericWork( wk, sizeof(BATTLE_SETUP_PARAM) );

			para->engine = BTL_ENGINE_ALONE;
			para->rule = BTL_RULE_DOUBLE;
			para->competitor = BTL_COMPETITOR_COMM;

			para->netHandle = GFL_NET_HANDLE_GetCurrentHandle();
			TAYA_Printf("[DTAYA] Multi Timing Sync Finish! NetHandle=%p\n", para->netHandle);
			para->netID = GFL_NET_GetNetID( para->netHandle );
			para->commMode = BTL_COMM_DS;
			para->commPos = para->netID;
			para->multiMode = 1;

			para->partyPlayer = PokeParty_AllocPartyWork( HEAPID_CORE );	///< �v���C���[�̃p�[�e�B
			para->partyEnemy1 = NULL;		///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
			para->partyPartner = NULL;	///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
			para->partyEnemy2 = NULL;		///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j

			switch( para->netID ){
			case 0:
				setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_GYARADOSU, MONSNO_PIKATYUU, MONSNO_RIZAADON, 0 );
				break;
			case 1:
				setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_YADOKINGU, MONSNO_METAGUROSU, MONSNO_SUTAAMII, 0 );
				break;
			case 2:
				setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_BAKUUDA, MONSNO_DONKARASU, MONSNO_SANDAASU, 0 );
				break;
			case 3:
				setup_party( HEAPID_CORE, para->partyPlayer, MONSNO_HERAKUROSU, MONSNO_GENGAA, MONSNO_EAAMUDO, 0 );
				break;
			}

			DEBUG_PerformanceSetActive( FALSE );
			GFL_PROC_SysCallProc( FS_OVERLAY_ID(battle), &BtlProcData, para );
			(*seq)++;
		}
		break;

	case 8:
		GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TEMP,   0xb0000 );
		initGraphicSystems( wk );
		createTemporaryModules( wk );
		startView( wk );
		return TRUE;
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
			if( monsno == MONSNO_GYARADOSU )
			{
				PP_SetWazaPush( pp, WAZANO_NAMINORI );
			}
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
// �������[�h�؂�ւ�
//------------------------------------------------------------------------------------------------------
static BOOL SUBPROC_KanjiMode( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	MAIN_WORK* wk = mywk;

	GFL_MSGSYS_SetLangID( !GFL_MSGSYS_GetLangID() );
	print_menu( wk, &wk->menuCtrl );
	return TRUE;
}

//------------------------------------------------------------------------------------------------------
// �ʐM��Ԃł̊���PrintTest
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

	if( !PRINT_UTIL_Trans(wk->printUtil, wk->printQue) )
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
				GFL_NET_SendData( wk->netHandle, GFL_NET_CMD_DEBUG_TAYA, sizeof(TEST_PACKET), &(wk->packet) );
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
			GFL_NET_SendData( wk->netHandle, GFL_NET_CMD_DEBUG_TAYA,sizeof(TEST_PACKET), &(wk->packet) );
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
				GFL_NET_SendData( wk->netHandle, GFL_NET_CMD_DEBUG_TAYA, sizeof(TEST_PACKET), &(wk->packet) );
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
// BlendMagic �\���e�X�g
//------------------------------------------------------------------------------------------------------

#define DEF_BPCFILE "/BMSample/aura_02.bpc"

enum {
	DEF_NP_TEXTURE_MAX =	 16,
	DEF_ENTRYTEX_MAX =		  3,
};

/*
	BlendMagicSDKforDS �̎����ɂ���
	-----------------------------------------------------------

	�����̃X�e�b�v�Ƃ��ĂV�ɕ����邱�Ƃ��ł��܂��B
	�P�D���C�u�����̏������A�������m��
	�Q�D�t�@�C���̃��[�h
	�R�D�G�t�F�N�g�̐���
	�S�D�G�t�F�N�g�̃A�b�v�f�[�g
	�T�D�G�t�F�N�g�̕`��
	�U�D�G�t�F�N�g�̊J��
	�V�D���C�u�����̏I������

	�����Ŋe�X�e�b�v�̏ڍׁE���ӎ������L���܂��B
	���X�e�b�v�P�D���C�u�����̏������A�������m�ہi initSystem(),initEffect() �j����
	-----------------------------------------------------------
	�C���N���[�h�w�b�_�[�͈ȉ��̂��̂ł��B
	#include "npfx.h"
	#include "nplibc.h"

	��{�I�ɍŏ��Ɉ�񂾂��R�[�����܂��B
	����Ă��邱�Ƃ͕\��̂Ƃ��胉�C�u�����̏������ƃ������m�ۂł��B
	���̍ێ����Ă��ė~�����R�n���h��������܂��B(�ϐ����̓T���v���Ɠ���)
	void*		pvBuf	�G�t�F�N�g�̃������� malloc �����Ƃ��̃n���h���ł��B�Ō�ɊJ������K�v������܂�
	npSYSTEM	sys		�������A�I�������̂Ƃ��K�v�ł�
	npCONTEXT	ctx		�������A�I�������A���[�h�A�`��̂Ƃ��ɕK�v�ł�

	�ǂꂾ�����������K�v����
	npU32 npParticleGetUsingComponentSize()
	npU32 npParticleGetUsingSubjectSize()
	�łǂꂾ���g���Ă��邩�m�F���Ȃ��猈�߂Ă��������B
	�iNP_DEBUG ���v���v���Z�b�T�Őݒ肵�Ă��������B����f�o�b�O�o�[�W���������g���Ȃ����Ă��܂��B�j

	���ۂ̏������A�������m�ۂ̓T���v�����������������B

	���X�e�b�v�Q�D�t�@�C���̃��[�h�i npUtilParticleLoadBph(),loadBpc() �j ����
	-----------------------------------------------------------
	�C���N���[�h�w�b�_�[�Ƃ��Ĉȉ�������܂��B
	#include "npBphImporter.h"
	#include "npBpcImporter.h"

	�ǂݍ��ނׂ��t�@�C���́u.bph�v�u.bpc�v�ł��B
	���G�t�F�N�g�f�[�^���[�h
	�܂��u.bph�v�t�@�C����ǂݍ��݂܂��B���ꂩ�瓾��ꂽ�u.bpc�v�t�@�C��������u.bpc�v�t�@�C�������[�h���܂��B
	���̍ۈ�����
	npPARTICLEEMITCYCLE*	pEmit
	�Ƃ����\���̂������Ƃ��ēn���̂ł����A���ꂪ�G�t�F�N�g���\�[�X�̑f�ł��B
	���[�h�̓x��npPARTICLEEMITCYCLE*�����A�����ȃG�t�F�N�g�̃��\�[�X���m�ۂ��Ă������ƂɂȂ�܂��B

	�e�N�X�`���̃��[�h�ł�
	�u.bph�v�t�@�C��������ǂݍ��񂾃e�N�X�`���̖��O����e�N�X�`�������[�h���܂��B
	�ǂݍ��񂾃e�N�X�`����npTEXTURE*�Ŏ����Ă����A�e�G�t�F�N�g���ƂɎg���܂킵�܂��B

	���X�e�b�v�R�D�G�t�F�N�g�̐����i npParticleCreateComposite() �j����
	-----------------------------------------------------------
	�G�t�F�N�g�𐶐�����ɂ̓t�@�C�������[�h�����Ƃ��ɍ����
	npPARTICLEEMITCYCLE*	pEmit
	�Ƃ�������Ƃɕ`��̎��ɕK�v��
	npPARTICLECOMPOSITE*	pComp
	�Ƃ����\���̂��K�v�ɂȂ�܂��B������g����
	npParticleCreateComposite( pEmit, &pComp );
	�Ƃ����֐����R�[�����邾���ł��B
	���ӁF�����G�t�F�N�g�ł��Ⴄ npPARTICLECOMPOSITE* ���K�v�ɂȂ�܂��B���̃G�t�F�N�g�͋�ʂ��Ȃ���΂����܂���B

	���X�e�b�v�S�D�G�t�F�N�g�̃A�b�v�f�[�g�i�@effUpdate(npPARTICLECOMPOSITE*, npU32)�@�j����
	-----------------------------------------------------------
	���ӁF�ʒu�A�X�P�[���A��]�ɂ��ẴZ�b�g�̎d��
		�@�X�P�[���Ɋւ��Ă͕ʑ������p�ӂ���Ă��܂��B
		�@npParticleSetScaling( npPARTICLECOMPOSITE* ,npFVECTOR* );
		�@���s�ړ��A��]�̓}�g���b�N�X�Ƃ���
		�@npParticleSetGlobalFMATRIX( npPARTICLEOBJECT* ,npFMATRIX );

	�A�b�v�f�[�g�֐��͈ȉ��̂��̂ł����A�������Ƀt���[�����[�g���Z�b�g���Ă��������B
	�iNP_NTSC60 ,NP_NTSC30 �܂��͐��̐������Z�b�g���邱�ƂŃf���^�^�C���ɑΉ��ł��܂��j
	npParticleUpdateComposite( pComp, NP_NTSC60 );

	���X�e�b�v�T�D�G�t�F�N�g�̕`��i�@npParticleRenderComposite()�@�j����
	-----------------------------------------------------------
	�`��֐��͈ȉ��̂��̂ł��B���̍ہA�e�N�X�`�����X�g�Ƃ��̐����K�v�ɂȂ�܂��B
	npParticleRenderComposite( &ctx, pComp, &pTex, 1 );

	���X�e�b�v�U�D�G�t�F�N�g�̊J���i�@npParticleReleaseComposite()�@�j����
	-----------------------------------------------------------
	�g��Ȃ��Ȃ����G�t�F�N�g�͂����ƊJ�����܂��傤�B	�����Ă����ƃ�����������Ȃ��Ȃ�܂��B
	npParticleReleaseComposite( pComp );


	���X�e�b�v�V�D���C�u�����̏I�������i�@releaseEffect()�@�j����
	-----------------------------------------------------------
	�G�t�F�N�g�������̊J���ƃ��C�u�����̏I�������ł��B

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

// DS�`��n������
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
// BM�V�X�e��������:������TRUE
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

	OS_TPrintf("BM�K�v�o�b�t�@�T�C�Y=0x%x bytes\n", nSize);

	*ppvBuf = bmt_alloc( HEAPID_TEMP, nSize );
	pByte = (npBYTE*)(*ppvBuf);//�|�C���^��n���Ă���

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
// BM�V�X�e�����
//--------------------------------------------------
static void Bmt_releaseEffect( npCONTEXT *pCtx, npSYSTEM *pSys, void **ppvBuf)
{
	// subject memory �̊J��
	npParticleExitSubjectFactory();
	// key frame memory �̊J��
	npParticleExitFkeyFactory();
	// component memory �̊J��
	npParticleExitComponentFactory();
	// texture memory �̊J��
	npExitTextureLibrary( pCtx );
	// context �̊J��
	npReleaseContext( pCtx );
	// system �̊J��
	npExitSystem( pSys );
	OS_TPrintf(" pSys ... ExitSistem(%p)\n", pSys);

	bmt_free( *ppvBuf );
	*ppvBuf = NULL;
}
//--------------------------------------------------
// �e�N�X�`�����[�h : return = �ǂݍ��݃e�N�X�`����
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

	// �e�N�X�`���t�@�C���E�p���b�g�t�@�C���̃T�C�Y�ɒ���
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
// �e�N�X�`�����
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
// �`��X�V
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


