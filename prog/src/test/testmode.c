//===================================================================
/**
 * @file	testmode.c
 */
//===================================================================
#include <wchar.h>
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"

#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_menuwork.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"
#include "msg/msg_debugname.h"

#include "test/testmode.h"
#include "title/title.h"
#include "title/game_start.h"
#include "gamesystem/game_init.h"
#include "savedata/mystatus.h"
#include "app/config_panel.h"		//ConfigPanelProcData�Q��
#include "infowin/infowin.h"

//======================================================================
//	define
//======================================================================

#define BG_PLANE_MENU GFL_BG_FRAME1_M
#define ITEM_NAME_MAX 32
#define TESTMODE_PLT_FONT	15
#define LIST_DISP_MAX 5
//======================================================================
//	enum
//======================================================================
typedef enum
{
	TMS_INIT_MENU,
	TMS_SELECT_ITEM,
	TMS_TERM_MENU,
	TMS_CHECK_NEXT,
	
	TMS_MAX,
}TESTMODE_STATE;

typedef enum
{
	TMN_CHANGE_PROC,
	TMN_CHANGE_MENU,
	TMN_GAME_START,
	
	TMN_CANCEL_MENU,	//B�{�^������
	TMN_EXIT_PROC,		//��������Proc�𔲂���

	TMN_MAX,
	
}TESTMODE_NEXTACT;

typedef enum
{
	TMI_NEWGAME,
	TMI_CONTINUE,
	TMI_CONTINUE_NET_OFF,
	TMI_CONTINUE_NET,
	TMI_DEBUG_START,
	TMI_DEBUG_START_NET,
	TMI_DEBUG_SELECT_NAME,
	
	TMI_MAX,
	
}TESTMODE_INITGAME_TYPE;

//======================================================================
//	typedef struct
//======================================================================
typedef struct _TESTMODE_MENU_LIST TESTMODE_MENU_LIST;

typedef struct
{
	HEAPID heapId_;
	void	*parentWork_;
	TESTMODE_STATE state_;
	TESTMODE_NEXTACT nextAction_;
	u8	refreshCount_;
	RTCDate	rtcDate_;
	RTCTime	rtcTime_;
	
	TESTMODE_MENU_LIST *currentMenu_;
	u16	currentItemNum_;
	TESTMODE_MENU_LIST *nextMenu_;
	u16	nextItemNum_;
	
	BMP_MENULIST_DATA	*menuList_;
	BMPMENULIST_WORK	*menuWork_;
	PRINT_UTIL			printUtil_;
	PRINT_UTIL			printUtilSub_;
	PRINT_QUE			*printQue_;
	PRINT_QUE			*printQueSub_;
	GFL_FONT 			*fontHandle_;
	GFL_BMPWIN			*bmpWin_;
	GFL_BMPWIN			*bmpWinSub_;	//���̏����
	
	//Proc�؂�ւ��p
	FSOverlayID overlayId_;
	const GFL_PROC_DATA *procData_;
	void *procWork_;
	
	//�V�K�J�n�p���������
	TESTMODE_INITGAME_TYPE	gameType_;

}TESTMODE_WORK;


//���j���[���莞�̏���
//	@value  ���[�N�E�I�����ꂽIndex
//	@return ���j���[�𔲂��邩�H
typedef BOOL (*TestMode_SelectFunc)( TESTMODE_WORK *work , const int idx );
struct _TESTMODE_MENU_LIST
{
	u16 str_[ITEM_NAME_MAX];
	TestMode_SelectFunc selectFunc_;
};

//======================================================================
//	proto
//======================================================================
static void	TESTMODE_InitGraphic( TESTMODE_WORK *work );
static void	TESTMODE_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane );

static void TESTMODE_CreateMenu( TESTMODE_WORK *work , TESTMODE_MENU_LIST *menuList , const u16 itemNum );
static BOOL TESTMODE_UpdateMenu( TESTMODE_WORK *work );

static void TESTMODE_RefreshSubWindow( TESTMODE_WORK *work );

//���j���[���莞�̊e�폈���ݒ�֐�
static void TESTMODE_COMMAND_ChangeProc( TESTMODE_WORK *work ,FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void *pwork );
static void TESTMODE_COMMAND_ChangeMenu( TESTMODE_WORK *work , TESTMODE_MENU_LIST *menuList , const u16 itemNum );
static void TESTMODE_COMMAND_StartGame( TESTMODE_WORK *work , TESTMODE_INITGAME_TYPE type );

//���j���[���莞�̊֐��Q
static BOOL TESTMODE_ITEM_SelectFuncDebugStart( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncDebugStartComm( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncContinue( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncContinueComm( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncChangeSelectName( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncWatanabe( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncTamada( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncSogabe( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncOhno( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncTaya( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncSample1( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncMatsuda( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncSave( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncSound( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncKagaya( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncAri( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncDlPlay( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectFuncNagi( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_BackTopMenu( TESTMODE_WORK *work , const int idx );

static BOOL TESTMODE_ITEM_SelectFuncRTCEdit( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_ChangeRTC( TESTMODE_WORK *work , const int idx );

static BOOL TESTMODE_ITEM_ChangeMusicalMenu( TESTMODE_WORK *work , const int idx );

static BOOL TESTMODE_ITEM_SelectFuncSelectName( TESTMODE_WORK *work , const int idx );

//------------------------------------------------------------------------
/*
 *	QUICKSTART -> START�{�^���ꔭ�ŔC�ӂ̃��j���[���N�����邽�߂̊֐��ݒ�
 */
//------------------------------------------------------------------------
#if defined DEBUG_ONLY_FOR_taya
	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncTaya
#elif defined DEBUG_ONLY_FOR_watanabe
	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncWatanabe
#elif defined DEBUG_ONLY_FOR_tamada
	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncTamada
#elif defined DEBUG_ONLY_FOR_sogabe
	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncSogabe
#elif defined DEBUG_ONLY_FOR_ohno
	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncOhno
#elif defined DEBUG_ONLY_FOR_matsuda
	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncMatsuda
#elif defined DEBUG_ONLY_FOR_kagaya
	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncKagaya
#elif defined DEBUG_ONLY_FOR_ariizumi_nobuhiko
//	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncDressUp
	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncAri
#elif defined DEBUG_ONLY_FOR_nagihashi_toru
	#define QuickSelectFunc		TESTMODE_ITEM_SelectFuncNagi
#endif


static TESTMODE_MENU_LIST topMenu[] = 
{
	//�ėp
	{L"�f�o�b�O�J�n"		,TESTMODE_ITEM_SelectFuncDebugStart },
	{L"�f�o�b�O�J�n(�ʐM)"		,TESTMODE_ITEM_SelectFuncDebugStartComm },
	{L"��������"			,TESTMODE_ITEM_SelectFuncContinue },
	{L"��������(�ʐM)"		,TESTMODE_ITEM_SelectFuncContinueComm },
	{L"���O��I��ŊJ�n"	,TESTMODE_ITEM_SelectFuncChangeSelectName },
	{L"RTC����"				,TESTMODE_ITEM_SelectFuncRTCEdit },
	{L"�Z�[�u�j����"		,TESTMODE_ITEM_SelectFuncSave },
	{L"SOUND"               ,TESTMODE_ITEM_SelectFuncSound },
	{L"�~���[�W�J��"		,TESTMODE_ITEM_ChangeMusicalMenu },

	//�l
	{L"�킽�Ȃׁ@�Ă�"	,TESTMODE_ITEM_SelectFuncWatanabe },
	{L"���܂��@��������"	,TESTMODE_ITEM_SelectFuncTamada },
	{L"�����ׁ@�Ђ���"		,TESTMODE_ITEM_SelectFuncSogabe },
	{L"�����́@����"		,TESTMODE_ITEM_SelectFuncOhno },
	{L"����@�܂���"		,TESTMODE_ITEM_SelectFuncTaya },
//	{L"Sample1" 			,TESTMODE_ITEM_SelectFuncSample1 },
	{L"�܂��@�悵�̂�"	,TESTMODE_ITEM_SelectFuncMatsuda },
	{L"������@������"		,TESTMODE_ITEM_SelectFuncKagaya  },
	{L"���肢���݁@�̂ԂЂ�",TESTMODE_ITEM_SelectFuncAri },
	{L"DlPlay Sample"		,TESTMODE_ITEM_SelectFuncDlPlay },
	{L"�Ȃ��͂��@�Ƃ���"	, TESTMODE_ITEM_SelectFuncNagi	}
};

static TESTMODE_MENU_LIST menuRTCEdit[] = 
{
	{L"���@�ӂ₷"			,TESTMODE_ITEM_ChangeRTC },
	{L"���@�ւ炷"			,TESTMODE_ITEM_ChangeRTC },
	{L"���@�ӂ₷"			,TESTMODE_ITEM_ChangeRTC },
	{L"���@�ւ炷"			,TESTMODE_ITEM_ChangeRTC },
	{L"���@�ӂ₷"			,TESTMODE_ITEM_ChangeRTC },
	{L"���@�ւ炷"			,TESTMODE_ITEM_ChangeRTC },
	{L"���@�ӂ₷"			,TESTMODE_ITEM_ChangeRTC },
	{L"���@�ւ炷"			,TESTMODE_ITEM_ChangeRTC },
	{L"�b�@�ӂ₷"			,TESTMODE_ITEM_ChangeRTC },
	{L"�b�@�ւ炷"			,TESTMODE_ITEM_ChangeRTC },
	
	{L"���ǂ�"				,TESTMODE_ITEM_BackTopMenu },
};

static TESTMODE_MENU_LIST nameListMax[DEBUG_NAME_MAX];

//--------------------------------------------------------------------------
//	�`����菉����
//--------------------------------------------------------------------------
static void	TESTMODE_InitGraphic( TESTMODE_WORK *work )
{
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_32K,		// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};

	static const GFL_BG_SYS_HEADER sysHeader = {
		GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
	};
	
	static const GFL_BG_BGCNT_HEADER bgCont_Menu = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	};

	GX_SetMasterBrightness(0);	
	GXS_SetMasterBrightness(-16);
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);

	GFL_DISP_SetBank( &vramBank );
	GFL_BG_Init( work->heapId_ );
	GFL_BG_SetBGMode( &sysHeader );	

	TESTMODE_InitBgFunc( &bgCont_Menu , BG_PLANE_MENU );
	
	GFL_BMPWIN_Init( work->heapId_ );
	

}

//--------------------------------------------------------------------------
//	Bg������ �@�\��
//--------------------------------------------------------------------------
static void	TESTMODE_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane )
{
	GFL_BG_SetBGControl( bgPlane, bgCont, GFL_BG_MODE_TEXT );
	GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
	GFL_BG_ClearFrame( bgPlane );
	GFL_BG_LoadScreenReq( bgPlane );
}


//--------------------------------------------------------------------------
//	���j���[�쐬
//--------------------------------------------------------------------------
static void TESTMODE_CreateMenu( TESTMODE_WORK *work , TESTMODE_MENU_LIST *menuList  , const u16 itemNum )
{
	{	//menu create
		u32 i;
		BMPMENULIST_HEADER head;
		STRCODE	workStr[128];	//��������
	
		work->menuList_ = BmpMenuWork_ListCreate( itemNum , work->heapId_ );
		//���X�g�쐬
		for( i=0;i<itemNum;i++ )
		{
			STRBUF *strbuf;
			const u16 strLen = wcslen(menuList[i].str_);
			//�I�[�R�[�h��ǉ����Ă���STRBUF�ɕϊ�
			GFL_STD_MemCopy( menuList[i].str_ , workStr , strLen*2 );
			workStr[strLen] = GFL_STR_GetEOMCode();
			
			strbuf = GFL_STR_CreateBuffer( strLen+1 , work->heapId_ );
			GFL_STR_SetStringCode( strbuf , workStr );
			
			BmpMenuWork_ListAddString( work->menuList_ , strbuf , NULL , work->heapId_ );
			
			GFL_STR_DeleteBuffer( strbuf );
		}
		{
			BMPMENULIST_HEADER menuHead = 
			{
				NULL,			//�\�������f�[�^�|�C���^
				NULL,			/* �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐� */
				NULL,			/* ���\�����Ƃ̃R�[���o�b�N�֐� */
				NULL,			/* BMP�E�B���h�E�f�[�^ */
				0,				/* ���X�g���ڐ� */
				LIST_DISP_MAX,	/* �\���ő區�ڐ� */
				0,				/* ���x���\���w���W */
				16,				/* ���ڕ\���w���W */
				0,				/* �J�[�\���\���w���W */
				2,				/* �\���x���W */
				1,				/*�����F */
				15,				/*�w�i�F */
				2,				/*�����e�F */
				0,				/* �����Ԋu�w */
				0,				/* �����Ԋu�x */
				BMPMENULIST_LRKEY_SKIP,		/*�y�[�W�X�L�b�v�^�C�v */
				0,				/* �����w��(�{���� u8 �����ǁA����Ȃɍ��Ȃ��Ǝv���̂�) */
				0,				/* �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF) */
				NULL,			//���[�N
				16,				//�����T�C�YX
				16,				//�����T�C�YY
				NULL,			//���b�Z�[�W�o�b�t�@
				NULL,			//�v�����g���[�e�B���e�B
				NULL,			//�v�����g�L���[
				NULL,			//�t�H���g�n���h��
			};
			menuHead.list = work->menuList_;
			menuHead.count = itemNum;
			menuHead.win = work->bmpWin_;
			menuHead.print_que = work->printQue_;
			menuHead.print_util = &work->printUtil_;
			menuHead.font_handle = work->fontHandle_;
			work->menuWork_ = BmpMenuList_Set( &menuHead, 0, 0, work->heapId_ );
			BmpMenuList_SetCursorBmp( work->menuWork_ , work->heapId_ );
		}
	}	
}

//--------------------------------------------------------------------------
//	���j���[�폜
//--------------------------------------------------------------------------
static void TESTMODE_DeleteMenu( TESTMODE_WORK *work )
{
	BmpMenuList_Exit( work->menuWork_ , NULL , NULL );
	BmpMenuWork_ListDelete( work->menuList_ );
}

//--------------------------------------------------------------------------
//	���j���[�X�V
//--------------------------------------------------------------------------
static BOOL TESTMODE_UpdateMenu( TESTMODE_WORK *work )
{
	u32 ret;
	
	ret = BmpMenuList_Main(work->menuWork_);
	switch(ret)
	{
	case BMPMENULIST_NULL:	/* �����I������Ă��Ȃ� */
		#ifdef QuickSelectFunc
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
		{
			return QuickSelectFunc( work, 0 );
		}
		#endif
		break;
	case BMPMENULIST_CANCEL:	/* �L�����Z�����ꂽ */
		if( work->currentMenu_ != topMenu && work->parentWork_ == NULL )
		{
			//Proc���\����TopMenu�ł͂Ȃ��Ƃ���TopMenu�ɖ߂�
			TESTMODE_ITEM_BackTopMenu(work,0);
		}
		else
		{
			work->nextAction_ = TMN_CANCEL_MENU;
		}
		return TRUE;
		break;
	default:		/* ���������肳�ꂽ */
		{
			u16 topPos,curPos;
			BmpMenuList_PosGet( work->menuWork_ , &topPos , &curPos );
			return work->currentMenu_[topPos+curPos].selectFunc_( work , topPos+curPos );
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------------------
//	���̃E�B���h�E�̎��ԍX�V
//--------------------------------------------------------------------------
static const char BuildDate[]=__DATE__;
static const char BuildTime[]=__TIME__;
static void TESTMODE_RefreshSubWindow( TESTMODE_WORK *work )
{
	//������擾�p
	const u16 strLen = 64;
	STRBUF *strbuf;
	u16		workStr[strLen];
	
	GFL_RTC_GetDateTime( &work->rtcDate_ , &work->rtcTime_ );
	
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->bmpWinSub_), 15 );
	
	strbuf = GFL_STR_CreateBuffer( strLen , work->heapId_ );
	
//	swprintf( workStr , "%2d����%2d�ɂ� %2��%2�ӂ�",
	swprintf( workStr ,strLen, L"ROM�쐬�@�@�@%s�@�@�@%s",BuildDate,BuildTime );
	workStr[wcslen(workStr)] = GFL_STR_GetEOMCode();
	GFL_STR_SetStringCode( strbuf , workStr );
	
	PRINT_UTIL_Print( &work->printUtilSub_ , work->printQueSub_ ,
				2 , 2 , strbuf , work->fontHandle_ );
	GFL_STR_ClearBuffer( strbuf );
	
	swprintf( workStr ,strLen, L"RTC�����@�@�@%4d/%02d/%02d�@�@�@%2d:%2d:%2d"
		,work->rtcDate_.year+2000 , work->rtcDate_.month , work->rtcDate_.day 
		,work->rtcTime_.hour , work->rtcTime_.minute , work->rtcTime_.second );

	workStr[wcslen(workStr)] = GFL_STR_GetEOMCode();
	GFL_STR_SetStringCode( strbuf , workStr );
	
	PRINT_UTIL_Print( &work->printUtilSub_ , work->printQueSub_ ,
				2 , 18 , strbuf , work->fontHandle_ );
	
	GFL_STR_DeleteBuffer( strbuf );
	
	GFL_BG_LoadScreenReq( BG_PLANE_MENU );
	work->refreshCount_ = 0;
}


//--------------------------------------------------------------------------
//	Proc�̐؂�ւ��\��
//	@value  �ŏ���TESTMODE_WORK������ȊO��GFL_PROC_SysSetNextProc �Ɠ����ł�
//--------------------------------------------------------------------------
static void TESTMODE_COMMAND_ChangeProc( TESTMODE_WORK *work ,FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void *pwork )
{
	work->overlayId_= ov_id;
	work->procData_	= procdata;
	work->procWork_	= pwork;
	
	work->nextAction_ = TMN_CHANGE_PROC;
}

//--------------------------------------------------------------------------
//	�Ⴄ���j���[�ֈړ�
//	@value  �ړ���̃��j���[���X�g�ƃ��X�g�̌�
//--------------------------------------------------------------------------
static void TESTMODE_COMMAND_ChangeMenu( TESTMODE_WORK *work , TESTMODE_MENU_LIST *menuList , const u16 itemNum )
{
	work->nextMenu_ = menuList;
	work->nextItemNum_ = itemNum;
	
	work->nextAction_ = TMN_CHANGE_MENU;
}

//--------------------------------------------------------------------------
//	�Q�[�����J�n����
//	@value  �ړ���̃��j���[���X�g�ƃ��X�g�̌�
//--------------------------------------------------------------------------
static void TESTMODE_COMMAND_StartGame( TESTMODE_WORK *work , TESTMODE_INITGAME_TYPE type )
{
	work->gameType_ = type;
	
	work->nextAction_ = TMN_GAME_START;
}




//============================================================================================
//
//
//	�v���Z�X�R���g���[��
//
//
//============================================================================================
const	GFL_PROC_DATA TestMainProcData;
FS_EXTERN_OVERLAY(testmode);

//------------------------------------------------------------------
/**
 * @brief		������
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	TESTMODE_WORK *work;
	HEAPID	heapID = HEAPID_TITLE;

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, heapID, 0x30000 );

	work = GFL_PROC_AllocWork( proc, sizeof(TESTMODE_WORK), heapID );
	GFL_STD_MemClear(work, sizeof(TESTMODE_WORK));
	work->heapId_	= heapID;
	work->parentWork_ = pwk;
	work->nextMenu_ = NULL;
	work->state_	= TMS_INIT_MENU;
	work->nextAction_ = TMN_MAX;
	work->refreshCount_ = 15;

	TESTMODE_InitGraphic( work );

	//�t�H���g�p�p���b�g
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , TESTMODE_PLT_FONT * 32, 16*2, work->heapId_ );
	work->fontHandle_ = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId_ );

	work->bmpWin_ = GFL_BMPWIN_Create( BG_PLANE_MENU , 1,1,30,LIST_DISP_MAX*2,TESTMODE_PLT_FONT,GFL_BMP_CHRAREA_GET_B );
	GFL_BMPWIN_MakeScreen( work->bmpWin_ );
	GFL_BMPWIN_TransVramCharacter( work->bmpWin_ );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->bmpWin_), 15 );
	GFL_BG_LoadScreenReq( BG_PLANE_MENU );
	work->printQue_ = PRINTSYS_QUE_Create( work->heapId_ );
	PRINT_UTIL_Setup( &work->printUtil_ , work->bmpWin_ );

	work->bmpWinSub_ = GFL_BMPWIN_Create( BG_PLANE_MENU , 1,18,30,4,TESTMODE_PLT_FONT,GFL_BMP_CHRAREA_GET_B );
	GFL_BMPWIN_MakeScreen( work->bmpWinSub_ );
	GFL_BMPWIN_TransVramCharacter( work->bmpWinSub_ );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->bmpWinSub_), 15 );
	GFL_BG_LoadScreenReq( BG_PLANE_MENU );
	work->printQueSub_ = PRINTSYS_QUE_Create( work->heapId_ );
	PRINT_UTIL_Setup( &work->printUtilSub_ , work->bmpWinSub_ );

	//�w�i�F
	*((u16 *)HW_BG_PLTT) = 0x7d8c;//RGB(12, 12, 31);

	//���[�N��NULL���^�C�g������ProcSet�ŌĂ΂ꂽ
	if( pwk == NULL )
	{
		work->currentMenu_ = topMenu;
		work->currentItemNum_ = NELEMS(topMenu);
	}
	else
	{
		TESTMODE_PROC_WORK *parentWork = pwk;

		switch( parentWork->startMode_ )
		{
		case TESTMODE_NAMESELECT:	//�l���I��
			{
				u16 i;
				GFL_MSGDATA *msgMng = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_debugname_dat, work->heapId_);

				for( i=0;i<DEBUG_NAME_MAX;i++ )
				{
					STRBUF  *str = GFL_STR_CreateBuffer( ITEM_NAME_MAX , work->heapId_ );
					GFL_MSG_GetString( msgMng , i , str );
					GFL_STR_GetStringCode( str , nameListMax[i].str_ , ITEM_NAME_MAX );
					GFL_STR_DeleteBuffer( str );

					nameListMax[i].selectFunc_ = TESTMODE_ITEM_SelectFuncSelectName;
				}
				TESTMODE_COMMAND_ChangeMenu( work , nameListMax , DEBUG_NAME_MAX );
				
				GFL_MSG_Delete( msgMng );

				work->currentMenu_ = nameListMax;
				work->currentItemNum_ = DEBUG_NAME_MAX;
			}
			break;
		}
	}
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief		���C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	TESTMODE_WORK *work = mywk;

	work->refreshCount_++;
	if( work->refreshCount_ > 15 )TESTMODE_RefreshSubWindow(work);
	
	switch( work->state_ )
	{
	case TMS_INIT_MENU:
		TESTMODE_CreateMenu( work , work->currentMenu_ ,work->currentItemNum_);
		work->state_ = TMS_SELECT_ITEM;
		break;

	case TMS_SELECT_ITEM:
		if( TESTMODE_UpdateMenu( work ) == TRUE )
		{
			work->state_ = TMS_TERM_MENU;
		}
		break;

	case TMS_TERM_MENU:
		TESTMODE_DeleteMenu( work );
		work->currentMenu_ = NULL;
		work->state_ = TMS_CHECK_NEXT;
		break;

	case TMS_CHECK_NEXT:
		//���̏����ւ̕���
		switch( work->nextAction_ )
		{
		case TMN_CHANGE_PROC:
			return GFL_PROC_RES_FINISH;
			break;

		case TMN_CHANGE_MENU:
			work->currentMenu_ = work->nextMenu_;
			work->currentItemNum_ = work->nextItemNum_;

			work->nextMenu_ = NULL;
			work->state_ = TMS_INIT_MENU;
			break;

		case TMN_CANCEL_MENU:
		case TMN_EXIT_PROC:
		default:
			return GFL_PROC_RES_FINISH;
			break;
		}
		break;
	}
	
	PRINTSYS_QUE_Main( work->printQue_ );
	PRINT_UTIL_Trans( &work->printUtil_ ,work->printQue_ );
	PRINTSYS_QUE_Main( work->printQueSub_ );
	PRINT_UTIL_Trans( &work->printUtilSub_ ,work->printQueSub_ );

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief		�I��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT TestModeProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	TESTMODE_WORK *work = mywk;
	
	//��������
	switch( work->nextAction_ )
	{
	case TMN_CHANGE_PROC:
		GFL_PROC_SysSetNextProc(work->overlayId_, work->procData_, work->procWork_);
		break;

	case TMN_GAME_START:
		{
			FS_EXTERN_OVERLAY(title);
			GFL_OVERLAY_Load(FS_OVERLAY_ID(title));
			switch( work->gameType_ )
			{
			case TMI_NEWGAME:
				GameStart_Beginning();
				break;
			case TMI_CONTINUE:
				GameStart_Continue();
				break;
			case TMI_CONTINUE_NET_OFF:
				GameStart_ContinueNetOff();
				break;
			case TMI_CONTINUE_NET:
				GameStart_ContinueNet();
				break;
			case TMI_DEBUG_START:
				GameStart_Debug();
				break;
			case TMI_DEBUG_START_NET:
				GameStart_DebugNet();
				break;
			case TMI_DEBUG_SELECT_NAME:
				GameStart_Debug_SelectName();
				break;
			}
			GFL_OVERLAY_Unload(FS_OVERLAY_ID(title));
		}
		break;

	case TMN_EXIT_PROC:
		//NoAction
		break;
		
	case TMN_CANCEL_MENU:
	default:
		//���[�N��NULL���^�C�g������ProcSet�ŌĂ΂ꂽ
		if( pwk == NULL )
		{
			GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
		}
		else
		{
			TESTMODE_PROC_WORK *parentWork = pwk;
			switch( parentWork->startMode_ )
			{
			case TESTMODE_NAMESELECT:	//�l���I��
				parentWork->work_ = (void*)1;
				break;
			}		
		}
		break;
	}


	//�J������
	PRINTSYS_QUE_Clear( work->printQue_ );
	PRINTSYS_QUE_Delete( work->printQue_ );
	PRINTSYS_QUE_Clear( work->printQueSub_ );
	PRINTSYS_QUE_Delete( work->printQueSub_ );

	GFL_BMPWIN_Delete( work->bmpWin_ );
	GFL_BMPWIN_Delete( work->bmpWinSub_ );

	GFL_FONT_Delete( work->fontHandle_ );
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap( HEAPID_TITLE );

	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��e�[�u��
 */
//------------------------------------------------------------------
const GFL_PROC_DATA TestMainProcData = {
	TestModeProcInit,
	TestModeProcMain,
	TestModeProcEnd,
};

//--------------------------------------------------------------------------
//	���ڌ��莞�R�[���o�b�N
//	@value  ���[�N
//			�I�����ꂽIndex
//	@return ���j���[�𔲂��邩�H(���j���[�؂�ւ����܂�
//--------------------------------------------------------------------------
static BOOL TESTMODE_ITEM_SelectFuncDummy( TESTMODE_WORK *work , const int idx )
{
	return FALSE;
}

//------------------------------------------------------------------
//	�g�b�v���j���[�p
//------------------------------------------------------------------

//�f�o�b�O�X�^�[�g
static BOOL TESTMODE_ITEM_SelectFuncDebugStart( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_StartGame( work , TMI_DEBUG_START );
	return TRUE;
}
//�f�o�b�O�X�^�[�g
static BOOL TESTMODE_ITEM_SelectFuncDebugStartComm( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_StartGame( work , TMI_DEBUG_START_NET );
	return TRUE;
}

//��������
static BOOL TESTMODE_ITEM_SelectFuncContinue( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_StartGame( work , TMI_CONTINUE_NET_OFF );
	return TRUE;
}
//��������
static BOOL TESTMODE_ITEM_SelectFuncContinueComm( TESTMODE_WORK *work , const int idx )
{
//	INFOWIN_InitComm( GFL_HEAPID_APP|HEAPDIR_MASK );
	TESTMODE_COMMAND_StartGame( work , TMI_CONTINUE_NET );
	return TRUE;
}

//�l���I��
static BOOL TESTMODE_ITEM_SelectFuncChangeSelectName( TESTMODE_WORK *work , const int idx )
{
	/*
	u16 i;
	GFL_MSGDATA *msgMng = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_debugname_dat, work->heapId_);

	for( i=0;i<DEBUG_NAME_MAX;i++ )
	{
		STRBUF  *str = GFL_STR_CreateBuffer( ITEM_NAME_MAX , work->heapId_ );
		GFL_MSG_GetString( msgMng , i , str );
		GFL_STR_GetStringCode( str , nameListMax[i].str_ , ITEM_NAME_MAX );
		GFL_STR_DeleteBuffer( str );
	}
	TESTMODE_COMMAND_ChangeMenu( work , nameListMax , DEBUG_NAME_MAX );
	
	GFL_MSG_Delete( msgMng );
	*/
	TESTMODE_COMMAND_StartGame( work , TMI_DEBUG_SELECT_NAME );
	return TRUE;
}

FS_EXTERN_OVERLAY(watanabe_sample);
extern const GFL_PROC_DATA DebugWatanabeMainProcData;
static BOOL TESTMODE_ITEM_SelectFuncWatanabe( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(watanabe_sample), &DebugWatanabeMainProcData, NULL);
	return TRUE;
}

static BOOL TESTMODE_ITEM_SelectFuncTamada( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(config_panel), &ConfigPanelProcData, NULL);
	//GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(debug_tamada), &DebugTamadaMainProcData, NULL);
#if 0
	{
		GAME_INIT_WORK * init_param = DEBUG_GetGameInitWork(GAMEINIT_MODE_DEBUG, 1);
		TESTMODE_COMMAND_ChangeProc(work,
			NO_OVERLAY_ID, &GameMainProcData, init_param);
	}
#endif
	return TRUE;
}

extern const GFL_PROC_DATA DebugSogabeMainProcData;
FS_EXTERN_OVERLAY(sogabe_debug);
static BOOL TESTMODE_ITEM_SelectFuncSogabe( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(sogabe_debug), &DebugSogabeMainProcData, NULL);
	return TRUE;
}

FS_EXTERN_OVERLAY(ohno_debug);
extern const GFL_PROC_DATA DebugOhnoListProcData;
static BOOL TESTMODE_ITEM_SelectFuncOhno( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(ohno_debug) , &DebugOhnoListProcData, NULL);
	return TRUE;
}

FS_EXTERN_OVERLAY(taya_debug);
extern const GFL_PROC_DATA DebugTayaMainProcData;
static BOOL TESTMODE_ITEM_SelectFuncTaya( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(taya_debug), &DebugTayaMainProcData, NULL);
	return TRUE;
}

#if 0
extern const GFL_PROC_DATA TestProg1MainProcData;
static BOOL TESTMODE_ITEM_SelectFuncSample1( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,NO_OVERLAY_ID, &TestProg1MainProcData, NULL);
	return TRUE;
}
#endif

FS_EXTERN_OVERLAY(matsuda_debug);
extern const GFL_PROC_DATA DebugMatsudaListProcData;
static BOOL TESTMODE_ITEM_SelectFuncMatsuda( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(matsuda_debug), &DebugMatsudaListProcData, NULL);
	return TRUE;
}

//�Z�[�u�j��
extern const GFL_PROC_DATA DebugSaveProcData;
static BOOL TESTMODE_ITEM_SelectFuncSave( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(matsuda_debug), &DebugSaveProcData, NULL);
	return TRUE;
}

//SOUND
FS_EXTERN_OVERLAY(sound_debug);
extern const GFL_PROC_DATA SoundTest2ProcData;
static BOOL TESTMODE_ITEM_SelectFuncSound( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(sound_debug), &SoundTest2ProcData, NULL);
	return TRUE;
}

static BOOL TESTMODE_ITEM_SelectFuncKagaya( TESTMODE_WORK *work , const int idx )
{
	//������
	{
		TESTMODE_COMMAND_StartGame( work , TMI_DEBUG_START );
		/*
		FS_EXTERN_OVERLAY(title);
		GFL_OVERLAY_Load(FS_OVERLAY_ID(title));
		GameStart_Debug();
		GFL_OVERLAY_Unload(FS_OVERLAY_ID(title));
		*/
	}
	return TRUE;
}

#include "ariizumi/ari_testmode.cdat"
static BOOL TESTMODE_ITEM_SelectFuncAri( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeMenu( work , menuAriizumi , NELEMS(menuAriizumi) );
	return TRUE;
}

FS_EXTERN_OVERLAY(download_play);
extern const GFL_PROC_DATA DebugDLPlayMainProcData;
static BOOL TESTMODE_ITEM_SelectFuncDlPlay( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(download_play), &DebugDLPlayMainProcData, NULL);
	return TRUE;
}

FS_EXTERN_OVERLAY(nagihashi_debug);
extern const GFL_PROC_DATA DebugNagiMainProcData;
static BOOL TESTMODE_ITEM_SelectFuncNagi( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(nagihashi_debug), &DebugNagiMainProcData, NULL);
	return TRUE;
}

static BOOL TESTMODE_ITEM_BackTopMenu( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeMenu( work , topMenu , NELEMS(topMenu) );
	return TRUE;
}

//RTC����
static BOOL TESTMODE_ITEM_SelectFuncRTCEdit( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeMenu( work , menuRTCEdit , NELEMS(menuRTCEdit) );
	return TRUE;
}


//-----------------------------------------------------------------------

//�~���[�W�J�����j���[
//#include "ariizumi/ari_testmode.cdat"	���̒��ɂ���
static BOOL TESTMODE_ITEM_ChangeMusicalMenu( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeMenu( work , menuMusical , NELEMS(menuMusical) );
	return TRUE;
}

//
static BOOL TESTMODE_ITEM_ChangeRTC( TESTMODE_WORK *work , const int idx )
{
	const int addValue = (idx%2==0 ? 1 : -1 );
	
	switch( idx )
	{
	case 0:	//��
	case 1:
		work->rtcDate_.month += addValue;
		RTC_SetDate( &work->rtcDate_ );
		break;
	case 2:	//��
	case 3:
		work->rtcDate_.day += addValue;
		RTC_SetDate( &work->rtcDate_ );
		break;
	case 4:	//��
	case 5:
		work->rtcTime_.hour += addValue;
		RTC_SetTime( &work->rtcTime_ );
		break;
	case 6:	//��
	case 7:
		work->rtcTime_.minute += addValue;
		RTC_SetTime( &work->rtcTime_ );
		break;
	case 8:	//�b
	case 9:
		work->rtcTime_.second += addValue;
		RTC_SetTime( &work->rtcTime_ );
		break;
	}
	TESTMODE_RefreshSubWindow(work);
	return FALSE;
}

//�l���I�����莞
static BOOL TESTMODE_ITEM_SelectFuncSelectName( TESTMODE_WORK *work , const int idx )
{
	GFL_MSGDATA *msgMng = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_debugname_dat, work->heapId_);

	STRBUF  *str = GFL_STR_CreateBuffer( ITEM_NAME_MAX , work->heapId_ );
	MYSTATUS		*myStatus;
	

	GFL_MSG_GetString( msgMng , idx , str );
	//���O�̃Z�b�g
	myStatus = SaveData_GetMyStatus( SaveControl_GetPointer() );
	MyStatus_SetMyNameFromString( myStatus , str );

	GFL_STR_DeleteBuffer( str );
	
	GFL_MSG_Delete( msgMng );

	work->nextAction_ = TMN_EXIT_PROC;
	return TRUE;
}





