//======================================================================
/**
 *
 * @file	ari_field_name.c
 * @brief	�t�B�[���h�f�o�b�O
 * @author	ariizumi
 * @data	08.10.07
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"
#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"
#include "message.naix"

#include "test_graphic/d_taya.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_d_comm_menu.h"

#include "ari_field_menu.h"
#include "ari_comm_func.h"
#include "ari_comm_def.h"

//======================================================================
//	define
//======================================================================
#define DEBUG_BGFRAME_MENU (GFL_BG_FRAME1_M)	//�g�p�t���[��
#define DEBUG_BGFRAME_MSG  (GFL_BG_FRAME2_M)	//�g�p�t���[��
#define DEBUG_BGFRAME_SUB  (GFL_BG_FRAME3_M)	//�g�p�t���[��
#define DEBUG_MENU_PANO	(14)
#define DEBUG_FONT_PANO	(15)

#define D_MENU_CHARSIZE_X (14)		//���j���[����
#define D_MENU_CHARSIZE_Y (18)		//���j���[�c��

#define D_MSG_CHAR_TOP	 ( 1)
#define D_MSG_CHAR_LEFT	 (19)

#define D_MSG_CHARSIZE_X (30)		//���b�Z�[�W�E�B���h�E����
#define D_MSG_CHARSIZE_Y ( 4)		//���b�Z�[�W�E�B���h�E�c��

#define D_MSG_STR_TOP	 (0)
#define D_MSG_STR_LEFT   (0)

#define D_CONNECTTYPE_MENU_CHARSIZE_X (16)		//���j���[����
#define D_CONNECTTYPE_MENU_CHARSIZE_Y ( 6)		//���j���[�c��

#define D_SEARCHPARENT_MENU_CHARSIZE_X (16)		//���j���[����
#define D_SEARCHPARENT_MENU_CHARSIZE_Y ((FIELD_COMM_SEARCH_PARENT_NUM+1)*2)		//���j���[�c��

#define D_WAITCHILD_MENU_CHARSIZE_X (16)		//���j���[����
#define D_WAITCHILD_MENU_CHARSIZE_Y ((FIELD_COMM_MEMBER_MAX+2)*2)		//���j���[�c��

//======================================================================
//	enum
//======================================================================

//���C������p
enum ARI_COMMMENU_MAIN_STATE
{
	CMS_COMM_START_MENU,
	CMS_COMM_MAIN_MENU,
	
	CMS_MAX,
};

//�ʐM���j���[�p(���p)
enum ARI_COMMMENU_SUB_STATE
{
	//�ʐM�O���j���[�p	
	FMS_INIT_TOPMENU,	//�ΐ���@�I��
	FMS_LOOP_TOPMENU,
	FMS_TERM_TOPMENU,
	FMS_INIT_REPORT_FIRST,	//�ŏ��̃��|�[�g
	FMS_LOOP_REPORT_FIRST,
	FMS_TERM_REPORT_FIRST,
	FMS_INIT_REPORT_SECOND,	//���̃��|�[�g
	FMS_LOOP_REPORT_SECOND,
	FMS_TERM_REPORT_SECOND,
	FMS_SAVE_WAIT1,
	FMS_SAVE_WAIT2,
	
	FMS_GO_COMM_MAIN,		//�ʐM���C�����j���[�֐؂�ւ�
		
	//�ʐM���C�����j���[�p
	FMS_INIT_SELECT_TYPE,	//�e�E�q�I��
	FMS_MAIN_SELECT_TYPE,
	FMS_TERM_SELECT_TYPE,
	FMS_START_COMM_COMMON,	//�ʐM���ʏ�����
	FMS_START_COMM_PARENT,	//�e�@�p������	
	FMS_START_COMM_CHILD,	//�q�@�p������
	FMS_UPDATE_SEARCH_PARENT,   //�e�@�T����
	FMS_TERM_SEARCH_PARENT,
	FMS_UPDATE_WAIT_CHILD,	    //�q�@�ڑ��҂�
	
	FMS_INIT_WAIT_PARENT,	    //�e�@�J�n�҂�
	FMS_LOOP_WAIT_PARENT,

	FMS_RETURN_CONNECT_TYPE,    //�ڑ���ޑI���֖߂�
	
	//�ėp
	FMS_END,			//�I�����ɗ^����X�e�[�g

	FMS_MAX,
};

//�͂��E�������I�����p�Ԃ�l
enum ARI_COMMMENU_YES_NO_RETURN
{
	YNR_WAIT,	//�I��
	YNR_YES,	//�͂�
	YNR_NO,		//������

	YNR_MAX,
};

//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	���j���[�Ăяo���֐�
///	�Ăяo���ہA����void*��DEBUG_COMMMENU���w�肳��܂�
//--------------------------------------------------------------
typedef void (* D_MENU_CALLPROC)(void*);

//--------------------------------------------------------------
///	DEBUG_MENU_LIST
//--------------------------------------------------------------
typedef struct
{
	u32 str_id;				//�\�����镶����ID
	D_MENU_CALLPROC callp;	//�I�����ꂽ�ۂɌĂяo���֐� NULL=�Ăяo������
}DEBUG_MENU_LIST;

//--------------------------------------------------------------
///	DEBUG_COMMMENU
//--------------------------------------------------------------
struct _DEBUG_COMMMENU
{
	int seq_no;
	int main_seq_no;
	u8  selectItemNo;
	
	u32 bgFrame;
	u32 msgFrame;
	u32 subFrame;
	HEAPID heapID;
	//���C���E�B���h�E�p
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;
	//���b�Z�[�W�E�B���h�E�p
	GFL_BMP_DATA *bmp_msg;
	GFL_BMPWIN *bmpwin_msg;
	//�I�����p
	GFL_BMP_DATA *bmp_sub;
	GFL_BMPWIN *bmpwin_sub;
	
	GFL_MSGDATA *msgdata;
	GFL_FONT *fontHandle;
	PRINT_QUE *printQue;
	PRINT_UTIL printUtil[1];
	PRINT_QUE *printQueMsg;
	PRINT_UTIL printUtilMsg[1];
	PRINT_QUE *printQueSub;
	PRINT_UTIL printUtilSub[1];
	ARCHANDLE *arcHandleMsg;
	
	BMP_MENULIST_DATA *menulistdata;
	BMPMENU_WORK *bmpmenu;
	BMP_MENULIST_DATA *menulistdata_sub;
	BMPMENU_WORK *bmpmenu_sub;
	
	D_MENU_CALLPROC call_proc;
};

//======================================================================
//	proto
//======================================================================
static void AriFldMenuCallProc_Test( void *wk );
static void AriCommMenuCallProc_ToSelectRule( void *wk );
static void AriCommMenuCallProc_CloseMenu( void *wk );

void	AriCommMenu_SetDefault_BmpMenuHeader( DEBUG_COMMMENU *d_menu, BMPMENU_HEADER *head ,u8 itemNum );

void	AriCommMenu_OpenTopMenu( DEBUG_COMMMENU *d_menu );
void	AriCommMenu_CloseTopMenu( DEBUG_COMMMENU *d_menu );
void	AriCommMenu_OpenMsgWindow( DEBUG_COMMMENU *d_menu , u32 msgID );

void	AriCommMenu_CloseMainMenu( DEBUG_COMMMENU *d_menu );
void	AriCommMenu_CloseSubMenu( DEBUG_COMMMENU *d_menu );
void	AriCommMenu_CloseMsgWindow( DEBUG_COMMMENU *d_menu );

void	AriCommMenu_OpenYesNoMenu( DEBUG_COMMMENU *d_menu );
u8	AriCommMenu_UpdateYesNoMenu( DEBUG_COMMMENU *d_menu );
void	AriCommMenu_ResetYesNoMenu( DEBUG_COMMMENU *d_menu );

void	AriCommMenu_OpenConnectTypeMenu( DEBUG_COMMMENU *d_menu );
void	AriCommMenu_OpenSearchParentMenu( DEBUG_COMMMENU *d_menu );
void	AriCommMenu_UpdateSearchParentMenu( const u8 findNum , DEBUG_COMMMENU *d_menu );
void	AriCommMenu_OpenWaitChildMenu( DEBUG_COMMMENU *d_menu );
void	AriCommMenu_UpdateWaitChildMenu( DEBUG_COMMMENU *d_menu );

void	AriCommMenu_ChangeExplanation( DEBUG_COMMMENU *d_menu , u32 msgID);
void	AriCommMenu_ExplanationMain( DEBUG_COMMMENU *d_menu );

BOOL	AriCommMenu_CommStartMenu( DEBUG_COMMMENU *d_menu );
BOOL	AriCommMenu_CommMainMenu( DEBUG_COMMMENU *d_menu );
//======================================================================
//	���j���[���X�g�ꗗ
//======================================================================
//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g�ꗗ
//--------------------------------------------------------------
static const DEBUG_MENU_LIST DATA_DebugMenuList[] =
{
	{ DEB_COMM_MENU_CAPTION1, NULL },
	{ DEB_COMM_MENU_2MODE_1	, AriCommMenuCallProc_ToSelectRule },
	{ DEB_COMM_MENU_2MODE_2 , AriCommMenuCallProc_ToSelectRule },
	{ DEB_COMM_MENU_2MODE_3 , AriCommMenuCallProc_ToSelectRule },
	{ DEB_COMM_MENU_CAPTION2, NULL },
	{ DEB_COMM_MENU_4MODE_1	, AriCommMenuCallProc_ToSelectRule },
	{ DEB_COMM_MENU_4MODE_2	, AriCommMenuCallProc_ToSelectRule },
	{ DEB_COMM_MENU_MANUAL	, AriFldMenuCallProc_Test },
	{ DEB_COMM_MENU_END     , AriCommMenuCallProc_CloseMenu },
};

//���j���[�ő吔
#define D_MENULIST_MAX (NELEMS(DATA_DebugMenuList))

//���[���g�p�I��p
static const DEBUG_MENU_LIST DATA_CommYesNoMenuList[] =
{
	{ DEB_COMM_YES , NULL },
	{ DEB_COMM_NO  , NULL },
};

//���j���[�ő吔
#define D_YESNO_MENULIST_MAX (NELEMS(DATA_CommYesNoMenuList))

//�ڑ����@�I��
static const DEBUG_MENU_LIST DATA_CommConnectTypeList[] =
{
	{ DEB_COMM_MENU_TYPE_CHILD	, NULL },
	{ DEB_COMM_MENU_TYPE_PARENT	, NULL },
	{ DEB_COMM_MENU_END			, NULL },
};

//���j���[�ő吔
#define D_CONNECTTYPE_MENULIST_MAX (NELEMS(DATA_CommConnectTypeList))

//======================================================================
//	�t�B�[���h�f�o�b�O���j���[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�@������
 * @param	heapID	�q�[�vID
 * @retval	DEBUG_COMMMENU
 */
//--------------------------------------------------------------
DEBUG_COMMMENU *AriFldMenu_Init( u32 heapID )
{
	DEBUG_COMMMENU *d_menu;
	d_menu = GFL_HEAP_AllocClearMemory( heapID, sizeof(DEBUG_COMMMENU) );
	
	d_menu->heapID = heapID;
	d_menu->bgFrame = DEBUG_BGFRAME_MENU;
	d_menu->msgFrame = DEBUG_BGFRAME_MSG;
	d_menu->subFrame = DEBUG_BGFRAME_SUB;

	{	//bmp font �����ꃁ�C������
		GFL_BMPWIN_Init( d_menu->heapID );
		GFL_FONTSYS_Init();
	}
	
	{	//BG������ �����ꃁ�C�����̂𗘗p����`��
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, 0x6000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_BGCNT_HEADER bgcntTextMsg = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
	
		GFL_BG_BGCNT_HEADER bgcntTextSub = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl( d_menu->bgFrame, &bgcntText   , GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( d_menu->msgFrame , &bgcntTextMsg, GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( d_menu->subFrame , &bgcntTextSub, GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( d_menu->bgFrame, VISIBLE_ON );
		GFL_BG_SetVisible( d_menu->msgFrame , VISIBLE_ON );
		GFL_BG_SetVisible( d_menu->subFrame , VISIBLE_ON );
		
		GFL_BG_SetPriority( d_menu->bgFrame, 2 );
		GFL_BG_SetPriority( d_menu->msgFrame , 1 );
		GFL_BG_SetPriority( d_menu->subFrame , 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME0_M, 3 );

		GFL_ARC_UTIL_TransVramPalette(
			ARCID_D_TAYA, NARC_d_taya_default_nclr,
			PALTYPE_MAIN_BG, DEBUG_FONT_PANO*32, 32, d_menu->heapID );

		GFL_BG_FillCharacter( d_menu->bgFrame, 0x00, 1, 0 );
		GFL_BG_FillCharacter( d_menu->msgFrame , 0x00, 1, 0 );
		GFL_BG_FillCharacter( d_menu->subFrame , 0x00, 1, 0 );
		GFL_BG_FillScreen( d_menu->bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_FillScreen( d_menu->msgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_FillScreen( d_menu->subFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_LoadScreenReq( d_menu->bgFrame );
		GFL_BG_LoadScreenReq( d_menu->msgFrame );
		GFL_BG_LoadScreenReq( d_menu->subFrame );
	}
	
	return( d_menu );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�@�J��
 * @param	heapID	�q�[�vID
 * @retval	DEBUG_COMMMENU
 */
//--------------------------------------------------------------
void AriFldMenu_Term( DEBUG_COMMMENU *d_menu )
{
	{	//�Ƃ肠����������Ł@������̓��C����
		GFL_BG_FreeCharacterArea( d_menu->bgFrame, 0x00, 0x20 );
		GFL_BG_FreeBGControl( d_menu->bgFrame );
		GFL_BG_FreeCharacterArea( d_menu->msgFrame, 0x00, 0x20 );
		GFL_BG_FreeBGControl( d_menu->msgFrame );
		GFL_BG_FreeCharacterArea( d_menu->subFrame, 0x00, 0x20 );
		GFL_BG_FreeBGControl( d_menu->subFrame );
	}
	GFL_BMPWIN_Exit();
	GFL_HEAP_FreeMemory( d_menu );

	//�O�̂��ߊJ��
	FieldComm_TermSystem();
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�@�폜
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	nMENU_TYPE_SYSTEMothing
 */
//--------------------------------------------------------------
void AriFldMenu_Delete( DEBUG_COMMMENU *d_menu )
{
	FontDataMan_Delete( d_menu->fontHandle );
	GFL_MSG_Delete( d_menu->msgdata );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�@���j���[�쐬
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	nothing
 */
//--------------------------------------------------------------
void AriFldMenu_Create( DEBUG_COMMMENU *d_menu )
{
	{	//work
		d_menu->seq_no = FMS_INIT_TOPMENU;
		d_menu->main_seq_no = CMS_COMM_START_MENU;
	}
	
	{	//window frame
		BmpWinFrame_GraphicSet( d_menu->bgFrame, 1,
			DEBUG_MENU_PANO, MENU_TYPE_SYSTEM, d_menu->heapID );
	}
		//�E�B���h�E�������ł���(���Ԃ�
	{	//bmpwin
		d_menu->bmpwin = GFL_BMPWIN_Create( d_menu->bgFrame,
			1, 1, D_MENU_CHARSIZE_X, D_MENU_CHARSIZE_Y,
			DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
		d_menu->bmp = GFL_BMPWIN_GetBmp( d_menu->bmpwin );

		GFL_BMP_Clear( d_menu->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
		
		GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
		GFL_BG_LoadScreenReq( d_menu->bgFrame );
	}
	
	{	//msg
		d_menu->msgdata = GFL_MSG_Create(
			GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
			NARC_message_d_comm_menu_dat, d_menu->heapID );
		
		
		d_menu->arcHandleMsg = GFL_ARC_OpenDataHandle(
				ARCID_D_TAYA, d_menu->heapID );
		
		d_menu->fontHandle = GFL_FONT_CreateHandle(
			d_menu->arcHandleMsg, NARC_d_taya_lc12_2bit_nftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, d_menu->heapID );
		
		PRINTSYS_Init( d_menu->heapID );
		d_menu->printQue = PRINTSYS_QUE_Create( d_menu->heapID );
		d_menu->printQueMsg = PRINTSYS_QUE_Create( d_menu->heapID );
		d_menu->printQueSub = PRINTSYS_QUE_Create( d_menu->heapID );
	
	}


	
	//�I�����E�B���h�E�p������
	{	//window frame
		BmpWinFrame_GraphicSet( d_menu->subFrame, 1,
			DEBUG_MENU_PANO, MENU_TYPE_SYSTEM, d_menu->heapID );
	}

	//�E�B���h�E�������ł��
	/*
	{	//bmpwin
		d_menu->bmpwin_sub = GFL_BMPWIN_Create( d_menu->subFrame,
			5, 5, 
			25, 5,
			DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
		d_menu->bmp_sub = GFL_BMPWIN_GetBmp( d_menu->bmpwin_sub );
		
		GFL_BMP_Clear( d_menu->bmp_sub, 0xff );
		GFL_BMPWIN_MakeScreen( d_menu->bmpwin_sub );
		
		GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin_sub );
		GFL_BG_LoadScreenReq( d_menu->subFrame );
	}

	{	//msg
		PRINT_UTIL_Setup( d_menu->printUtilSub, d_menu->bmpwin_sub );
	}
	*/
	AriCommMenu_OpenMsgWindow( d_menu , 0 );
}

//--------------------------------------------------------------
/**
 * ���X�g���j���[�p�w�b�_�W���ݒ�֐�
 * @param	d_menu	DEBUG_COMMMENU
 * @param	head	BMPMENU_HEADER �ݒ肵�����w�b�_
 * @param	itemNum	���j���[�̍��ڐ�
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_SetDefault_BmpMenuHeader( DEBUG_COMMMENU *d_menu, BMPMENU_HEADER *head ,u8 itemNum )
{
	head->x_max = 1;
	head->y_max = itemNum;
	head->line_spc = 4;
	head->c_disp_f = 0;
	head->loop_f = 1;
		
	head->font_size_x = 12;
	head->font_size_y = 12;
	head->msgdata = d_menu->msgdata;
	head->font_handle = d_menu->fontHandle;
}


//--------------------------------------------------------------
/**
 * ���C�����j���[�ėp����
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_CloseMainMenu( DEBUG_COMMMENU *d_menu )
{
	//�N���A����
	//�I���R�}���h�̎�t��ɌĂ�ł����ǁA�����Ɉړ�
	GFL_BMP_Clear( d_menu->bmp, 0x00 );
	GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
	GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
	BmpWinFrame_Clear( d_menu->bmpwin, 0 );
	
	//�J������
	GFL_BMPWIN_Delete( d_menu->bmpwin );
	BmpMenu_Exit( d_menu->bmpmenu, NULL );
	BmpMenuWork_ListSTRBUFDelete( d_menu->menulistdata );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�ėp�J��
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_OpenMsgWindow( DEBUG_COMMMENU *d_menu , u32 msgID )
{
	//���b�Z�[�W�E�B���h�E�p������
	{	//window frame
		BmpWinFrame_GraphicSet( d_menu->msgFrame, 1,
			DEBUG_MENU_PANO, MENU_TYPE_SYSTEM, d_menu->heapID );
	}
        //�E�B���h�E�������ł���(���Ԃ�
	{	//bmpwin
	    d_menu->bmpwin_msg = GFL_BMPWIN_Create( d_menu->msgFrame,
	    D_MSG_CHAR_TOP , D_MSG_CHAR_LEFT , 
	    D_MSG_CHARSIZE_X, D_MSG_CHARSIZE_Y,
	    DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
	    d_menu->bmp_msg = GFL_BMPWIN_GetBmp( d_menu->bmpwin_msg );

	    GFL_BMP_Clear( d_menu->bmp_msg, 0xff );
	    GFL_BMPWIN_MakeScreen( d_menu->bmpwin_msg );

	    GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin_msg );
	    GFL_BG_LoadScreenReq( d_menu->bgFrame );
	}
    
	{	//msg
	    PRINT_UTIL_Setup( d_menu->printUtilMsg, d_menu->bmpwin_msg );
	}

	BmpWinFrame_Write( d_menu->bmpwin_msg,
	    WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
	AriCommMenu_ChangeExplanation( d_menu , msgID );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�ėp����
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_CloseMsgWindow( DEBUG_COMMMENU *d_menu )
{
	//�N���A����
	GFL_BMP_Clear( d_menu->bmp_msg, 0x00 );
	GFL_BMPWIN_MakeScreen( d_menu->bmpwin_msg );
	GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin_msg );
	BmpWinFrame_Clear( d_menu->bmpwin_msg, 0 );

	//�J������
	GFL_BMPWIN_Delete( d_menu->bmpwin_msg );
}

//--------------------------------------------------------------
/**
 * �T�u���j���[�ėp����
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_CloseSubMenu( DEBUG_COMMMENU *d_menu )
{
	//�N���A����
	//�I���R�}���h�̎�t��ɌĂ�ł����ǁA�����Ɉړ�
	GFL_BMP_Clear( d_menu->bmp_sub, 0x00 );
	GFL_BMPWIN_MakeScreen( d_menu->bmpwin_sub );
	GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin_sub );
	BmpWinFrame_Clear( d_menu->bmpwin_sub, 0 );
	
	//�J������
	BmpMenu_Exit( d_menu->bmpmenu_sub, NULL );
	BmpMenuWork_ListSTRBUFDelete( d_menu->menulistdata_sub );
		
	//�v�����g���[�e�B���e�B�폜�@����Ȃ�
	PRINTSYS_QUE_Delete( d_menu->printQueSub );
	GFL_BMPWIN_Delete( d_menu->bmpwin_sub );
}


//--------------------------------------------------------------
/**
 * �g�b�v���j���[(�ΐ���@�I��)�J��
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_OpenTopMenu( DEBUG_COMMMENU *d_menu )
{
	{	//window frame
		BmpWinFrame_Write( d_menu->bmpwin,
			WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
		PRINT_UTIL_Setup( d_menu->printUtil, d_menu->bmpwin );	
	}
		
	{	//menu create
		u32 i,lmax;
		BMPMENU_HEADER head;
		const DEBUG_MENU_LIST *d_menu_list;
			
		lmax = D_MENULIST_MAX;
		AriCommMenu_SetDefault_BmpMenuHeader( d_menu , &head , lmax );
		head.print_util = d_menu->printUtil;
		head.print_que = d_menu->printQue;
		head.win = d_menu->bmpwin;
		d_menu->menulistdata =
			BmpMenuWork_ListCreate( lmax, d_menu->heapID );
			
		d_menu_list = DATA_DebugMenuList;
			
		for( i = 0; i < lmax; i++ ){
#if 0	//���o���̃X�L�b�v���I���ł��Ȃ��̂łƂ肠�����ۗ�
			if( d_menu_list[i].callp == NULL ){
			BMPMENULIST_RABEL	BmpMenuWork_ListAddArchiveString(
					d_menu->menulistdata, d_menu->msgdata,
			d_menu_list[i].str_id, BMPMENULIST_RABEL,
					d_menu->heapID );
			}else{
				BmpMenuWork_ListAddArchiveString(
					d_menu->menulistdata, d_menu->msgdata,
					d_menu_list[i].str_id, (u32)d_menu_list[i].callp,
					d_menu->heapID );
			}
#endif
				BmpMenuWork_ListAddArchiveString(
					d_menu->menulistdata, d_menu->msgdata,
					d_menu_list[i].str_id, (u32)d_menu_list[i].callp,
					d_menu->heapID );
		}
		
		head.menu = d_menu->menulistdata;
		
		d_menu->bmpmenu = BmpMenu_Add( &head, 0, d_menu->heapID );
		//�����J�[�\�����������
		BmpMenu_MainOutControl( d_menu->bmpmenu , 0 );
		//�������b�Z�[�W
		BmpMenu_SetCursorString( d_menu->bmpmenu, DEB_COMM_MENU_CURSOR );
	}
	
	d_menu->seq_no = FMS_LOOP_TOPMENU;
	AriCommMenu_ChangeExplanation( d_menu , DEB_COMM_MENU_STR_10 );
}

//--------------------------------------------------------------
/**
 * �g�b�v���j���[(�ΐ���@�I��)����
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_CloseTopMenu( DEBUG_COMMMENU *d_menu )
{
	AriCommMenu_CloseMainMenu( d_menu );
	AriCommMenu_CloseMsgWindow( d_menu );
}

//--------------------------------------------------------------
/**
 * �͂��E�������ėp�@�J��
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_OpenYesNoMenu( DEBUG_COMMMENU *d_menu )
{
	{	//bmpwin
		d_menu->bmpwin_sub = GFL_BMPWIN_Create( d_menu->subFrame,
			25, 13, 6, 4,
			DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
		d_menu->bmp_sub = GFL_BMPWIN_GetBmp( d_menu->bmpwin_sub );
		
		GFL_BMP_Clear( d_menu->bmp_sub, 0xff );
		GFL_BMPWIN_MakeScreen( d_menu->bmpwin_sub );
		
		GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin_sub );
		GFL_BG_LoadScreenReq( d_menu->subFrame );
	}
	PRINT_UTIL_Setup( d_menu->printUtilSub, d_menu->bmpwin_sub );

	{	//window frame
		BmpWinFrame_Write( d_menu->bmpwin_sub,
			WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
	}
		
	{	//menu create
		u32 i,lmax;
		BMPMENU_HEADER head;
		const DEBUG_MENU_LIST *d_menu_list;
			
		lmax = D_YESNO_MENULIST_MAX;

		AriCommMenu_SetDefault_BmpMenuHeader( d_menu , &head , lmax );
		head.print_util = d_menu->printUtilSub;
		head.print_que = d_menu->printQueSub;
		head.win = d_menu->bmpwin_sub;

		d_menu->menulistdata_sub =
			BmpMenuWork_ListCreate( lmax, d_menu->heapID );
			
		d_menu_list = DATA_CommYesNoMenuList;
			
		for( i = 0; i < lmax; i++ ){
				BmpMenuWork_ListAddArchiveString(
					d_menu->menulistdata_sub, d_menu->msgdata,
					d_menu_list[i].str_id, (u32)d_menu_list[i].callp,
					d_menu->heapID );
		}
		
		head.menu = d_menu->menulistdata_sub;
		
		d_menu->bmpmenu_sub = BmpMenu_Add( &head, 0, d_menu->heapID );
		BmpMenu_SetCursorString( d_menu->bmpmenu_sub, DEB_COMM_MENU_CURSOR );
	}
	
	d_menu->seq_no = FMS_LOOP_REPORT_FIRST;
}

//--------------------------------------------------------------
/**
 * �͂��E�������ėp�@�X�V
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	ARI_COMM_YESNO_RETURN(�͂��E�������E�I��)
 */
//--------------------------------------------------------------
u8		AriCommMenu_UpdateYesNoMenu( DEBUG_COMMMENU *d_menu )
{
	const u32 ret = BmpMenu_Main( d_menu->bmpmenu_sub );

	PRINTSYS_QUE_Main( d_menu->printQueSub );
	if( PRINT_UTIL_Trans(d_menu->printUtilSub,d_menu->printQueSub) ){
	}
	
	switch( ret ){
	case BMPMENU_NULL:
		break;
	case BMPMENU_CANCEL:
		return YNR_NO;
		break;
	default:
		switch( BmpMenu_CursorPosGet(d_menu->bmpmenu_sub) ){
		case 0:
			return YNR_YES;
			break;
		case 1:
			return YNR_NO;
			break;
		default:
			GF_ASSERT("Invalid return value!\n");
			break;
		}
	
	}
	
	return YNR_WAIT;
}

//--------------------------------------------------------------
/**
 * �͂��E�������ėp�@���Z�b�g
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	ARI_COMM_YESNO_RETURN(�͂��E�������E�I��)
 */
//--------------------------------------------------------------
void	AriCommMenu_ResetYesNoMenu( DEBUG_COMMMENU *d_menu )
{
	//�Ƃ肠�������āA�������Ȃ���
	//�������E�E�E
//	AriCommMenu_CloseSubMenu( d_menu );
//	AriCommMenu_OpenYesNoMenu( d_menu );
}

//--------------------------------------------------------------
/**
 * �ڑ����@�I���@�J�� 
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_OpenConnectTypeMenu( DEBUG_COMMMENU *d_menu )
{
	//�E�B���h�E�������ł���(���Ԃ�
	{	//bmpwin
		d_menu->bmpwin = GFL_BMPWIN_Create( d_menu->bgFrame,
			1, 1, D_CONNECTTYPE_MENU_CHARSIZE_X, D_CONNECTTYPE_MENU_CHARSIZE_Y,
			DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
		d_menu->bmp = GFL_BMPWIN_GetBmp( d_menu->bmpwin );

		GFL_BMP_Clear( d_menu->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
		
		GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
		GFL_BG_LoadScreenReq( d_menu->bgFrame );
	}

	{//window frame
		BmpWinFrame_Write( d_menu->bmpwin,
			WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );

		PRINT_UTIL_Setup( d_menu->printUtil, d_menu->bmpwin );	
	}
		
	{	//menu create
		u32 i,lmax;
		BMPMENU_HEADER head;
		const DEBUG_MENU_LIST *d_menu_list;
			
		lmax = D_CONNECTTYPE_MENULIST_MAX;
		AriCommMenu_SetDefault_BmpMenuHeader( d_menu , &head , lmax );
		head.print_util = d_menu->printUtil;
		head.print_que = d_menu->printQue;
		head.win = d_menu->bmpwin;
		d_menu->menulistdata =
			BmpMenuWork_ListCreate( lmax, d_menu->heapID );
			
		d_menu_list = DATA_CommConnectTypeList;
			
		for( i = 0; i < lmax; i++ ){
				BmpMenuWork_ListAddArchiveString(
					d_menu->menulistdata, d_menu->msgdata,
					d_menu_list[i].str_id, (u32)d_menu_list[i].callp,
					d_menu->heapID );
		}
		
		head.menu = d_menu->menulistdata;
		
		d_menu->bmpmenu = BmpMenu_Add( &head, 0, d_menu->heapID );
		BmpMenu_SetCursorString( d_menu->bmpmenu, DEB_COMM_MENU_CURSOR );
	}
}

//--------------------------------------------------------------
/**
 * (�q�@)�e�@�{����ʐ��� 
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_OpenSearchParentMenu( DEBUG_COMMMENU *d_menu )
{
	//�E�B���h�E�������ł���(���Ԃ�
	{	//bmpwin
		d_menu->bmpwin = GFL_BMPWIN_Create( d_menu->bgFrame,
			1, 1, D_SEARCHPARENT_MENU_CHARSIZE_X, D_SEARCHPARENT_MENU_CHARSIZE_Y,
			DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
		d_menu->bmp = GFL_BMPWIN_GetBmp( d_menu->bmpwin );

		GFL_BMP_Clear( d_menu->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
		
		GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
		GFL_BG_LoadScreenReq( d_menu->bgFrame );
	}

	{//window frame
		BmpWinFrame_Write( d_menu->bmpwin,
			WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
	PRINT_UTIL_Setup( d_menu->printUtil, d_menu->bmpwin );	
	}
		
	{	//menu create
		u32 i;
		const u32 lmax = FIELD_COMM_SEARCH_PARENT_NUM+1;
		BMPMENU_HEADER head;
		const DEBUG_MENU_LIST *d_menu_list;
			
		AriCommMenu_SetDefault_BmpMenuHeader( d_menu , &head , lmax );
		head.print_util = d_menu->printUtil;
		head.print_que = d_menu->printQue;
		head.win = d_menu->bmpwin;
		d_menu->menulistdata =
			BmpMenuWork_ListCreate( lmax, d_menu->heapID );
			
		for( i = 0; i < lmax; i++ ){
				BmpMenuWork_ListAddArchiveString(
					d_menu->menulistdata, d_menu->msgdata,
					( i==lmax-1 ? DEB_COMM_MENU_END : DEB_COMM_EMPTYNAME ), NULL ,
					d_menu->heapID );
		}
		
		head.menu = d_menu->menulistdata;
		
		d_menu->bmpmenu = BmpMenu_Add( &head, 0, d_menu->heapID );
		BmpMenu_SetCursorString( d_menu->bmpmenu, DEB_COMM_MENU_CURSOR );
	}
}

//--------------------------------------------------------------
/**
 * (�q�@)�e�@�{����ʍX�V 
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_UpdateSearchParentMenu( const u8 findNum , DEBUG_COMMMENU *d_menu )
{
    u8 i=0;
    //STRBUF *tempBuf;
    for( i=0 ; i<findNum ; i++ )
    {
	//FIXME: ID���������ꂽ��AID��r�Ŗ��O�̍X�V������
	//tempBuf = GFL_STR_CreateBuffer( FIELD_COMM_NAME_LENGTH , d_menu->heapID );
	//FieldComm_GetSearchParentData( i , tempBuf );
	FieldComm_GetSearchParentName( i , (STRBUF*)d_menu->menulistdata[i].str );
    }
    for( ; i< FIELD_COMM_SEARCH_PARENT_NUM ; i++ )
    {
	GFL_MSG_GetString( d_menu->msgdata , DEB_COMM_EMPTYNAME , (STRBUF*)d_menu->menulistdata[i].str );
    }
    BmpMenu_RedrawString( d_menu->bmpmenu );
}

//--------------------------------------------------------------
/**
 * (�e�@)�q�@�҂���ʐ��� 
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_OpenWaitChildMenu( DEBUG_COMMMENU *d_menu )
{
	//�E�B���h�E�������ł���(���Ԃ�
	{	//bmpwin
		d_menu->bmpwin = GFL_BMPWIN_Create( d_menu->bgFrame,
			1, 1, D_WAITCHILD_MENU_CHARSIZE_X , D_WAITCHILD_MENU_CHARSIZE_Y ,
			DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
		d_menu->bmp = GFL_BMPWIN_GetBmp( d_menu->bmpwin );

		GFL_BMP_Clear( d_menu->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
		
		GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
		GFL_BG_LoadScreenReq( d_menu->bgFrame );
	}

	{//window frame
		BmpWinFrame_Write( d_menu->bmpwin,
			WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
	PRINT_UTIL_Setup( d_menu->printUtil, d_menu->bmpwin );	
	}
		
	{	//menu create
		u32 i;
		const u32 lmax = FIELD_COMM_MEMBER_MAX+2;
		BMPMENU_HEADER head;
		const DEBUG_MENU_LIST *d_menu_list;
			
		AriCommMenu_SetDefault_BmpMenuHeader( d_menu , &head , lmax );
		head.print_util = d_menu->printUtil;
		head.print_que = d_menu->printQue;
		head.win = d_menu->bmpwin;
		d_menu->menulistdata =
			BmpMenuWork_ListCreate( lmax, d_menu->heapID );
			
		for( i = 0; i < lmax; i++ ){
		    if( i == lmax-1 ){
			BmpMenuWork_ListAddArchiveString(
				d_menu->menulistdata, d_menu->msgdata,
				DEB_COMM_MENU_END , NULL ,
				d_menu->heapID );
		    }
		    else if(i==lmax-2){
			BmpMenuWork_ListAddArchiveString(
				d_menu->menulistdata, d_menu->msgdata,
				DEB_COMM_START , NULL ,
				d_menu->heapID );
		    }
		    else{
		        BmpMenuWork_ListAddArchiveString(
			    d_menu->menulistdata, d_menu->msgdata,
			    DEB_COMM_EMPTYNAME , NULL ,
			    d_menu->heapID );
		    }
		}
		
		
		head.menu = d_menu->menulistdata;
		
		d_menu->bmpmenu = BmpMenu_Add( &head, 0, d_menu->heapID );
		BmpMenu_SetCursorString( d_menu->bmpmenu, DEB_COMM_MENU_CURSOR );
	}

}


//--------------------------------------------------------------
/**
 * (�e�@)�q�@�҂���ʍX�V 
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_UpdateWaitChildMenu( DEBUG_COMMMENU *d_menu )
{
        BmpMenu_RedrawString( d_menu->bmpmenu );
	return;
    if( FieldComm_IsDutyMemberData() == TRUE ){
	const u8 memberNum = FieldComm_GetMemberNum();
	u8 i=0;
        //STRBUF *tempBuf;
	for( i=0 ; i<memberNum ; i++ )
        {
	    //FIXME: ID���������ꂽ��AID��r�Ŗ��O�̍X�V������
	    //tempBuf = GFL_STR_CreateBuffer( FIELD_COMM_NAME_LENGTH , d_menu->heapID );
	    //FieldComm_GetSearchParentData( i , tempBuf );
	    const BOOL ret = FieldComm_GetMemberName( i , (STRBUF*)d_menu->menulistdata[i].str );
	    if( ret == FALSE ){
		//�f�[�^�擾���s
		GFL_MSG_GetString( d_menu->msgdata , DEB_COMM_EMPTYNAME , (STRBUF*)d_menu->menulistdata[i].str );
	    }
        }
	for( ; i< FIELD_COMM_MEMBER_MAX ; i++ )
	{
	    GFL_MSG_GetString( d_menu->msgdata , DEB_COMM_EMPTYNAME , (STRBUF*)d_menu->menulistdata[i].str );
	}
        BmpMenu_RedrawString( d_menu->bmpmenu );
	FieldComm_ResetDutyMemberData();
    }
}


//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�@�������ύX
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_ChangeExplanation( DEBUG_COMMMENU *d_menu , u32 msgID)
{
	//����������b�Z�[�W�f�[�^��������o�����߂̃o�b�t�@
	STRBUF *strTemp;

	//�O�񕶎���̏���
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(d_menu->bmpwin_msg),
				D_MSG_STR_LEFT, D_MSG_STR_TOP,
				D_MSG_CHARSIZE_X*8, D_MSG_CHARSIZE_Y*8, 0xff );
				
	strTemp = GFL_STR_CreateBuffer( 128, d_menu->heapID );
	GFL_MSG_GetString( d_menu->msgdata , msgID , strTemp );

	PRINT_UTIL_Print(
		d_menu->printUtilMsg, d_menu->printQueMsg,
		D_MSG_STR_LEFT, D_MSG_STR_TOP , (void*)strTemp, d_menu->fontHandle );
	GFL_STR_DeleteBuffer( strTemp );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�@���������C��GameServiceID GameServiceID1, GameServiceID GameServiceID2
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriCommMenu_ExplanationMain( DEBUG_COMMMENU *d_menu )
{
	return;
/*	
	u32 msgID;
	//���̕�����̐ݒ�
	switch( BmpMenu_CursorPosGet(d_menu->bmpmenu) ){
	case 0:
	case 1:
		AriCommMenu_ChangeExplanation( d_menu , DEB_COMM_MENU_STR_10 );
		break;
	default:
		AriCommMenu_ChangeExplanation( d_menu , DEB_COMM_MENU_STR_20 );
		break;
	}
*/
}

//--------------------------------------------------------------
/**
 * �ʐM�O���j���[
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	BOOL	TRUE=�I��
 */
//--------------------------------------------------------------
BOOL	AriCommMenu_CommStartMenu( DEBUG_COMMMENU *d_menu )
{
	PRINTSYS_QUE_Main( d_menu->printQueMsg );
	if( PRINT_UTIL_Trans(d_menu->printUtilMsg,d_menu->printQueMsg) ){
	}
			
	switch( d_menu->seq_no ){
	case FMS_INIT_TOPMENU:	//���j���[�쐬
		AriCommMenu_OpenTopMenu( d_menu );
		break;
	case FMS_LOOP_TOPMENU:	//���j���[����
		{
			u32 ret;
			
			ret = BmpMenu_Main( d_menu->bmpmenu );
			PRINTSYS_QUE_Main( d_menu->printQue );
			if( PRINT_UTIL_Trans(d_menu->printUtil,d_menu->printQue) ){
			}
			
			switch( ret ){
			case BMPMENU_NULL:
				break;
			case BMPMENU_CANCEL:
				/* //�J�������̂Ƃ���ֈړ�
				GFL_BMP_Clear( d_menu->bmp, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
				GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
				BmpWinFrame_Clear( d_menu->bmpwin, 0 );
	
				GFL_BMP_Clear( d_menu->bmp_msg, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->bmpwin_msg );
				GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin_msg );
				*/
				d_menu->seq_no = FMS_TERM_TOPMENU;
				break;
				
			default:
				/* //�J�������̂Ƃ���ֈړ�
				GFL_BMP_Clear( d_menu->bmp, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
				GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
				BmpWinFrame_Clear( d_menu->bmpwin, 0 );
				
				GFL_BMP_Clear( d_menu->bmp_msg, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->bmpwin_msg );
				GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin_msg );
				BmpWinFrame_Clear( d_menu->bmpwin_msg, 0 );
				*/
				
				d_menu->call_proc = (D_MENU_CALLPROC)ret;
				d_menu->seq_no = FMS_TERM_TOPMENU;
				break;
			}
			
			if(	BmpMenu_MoveSiteGet(d_menu->bmpmenu) == BMPMENU_MOVE_UP || 
				BmpMenu_MoveSiteGet(d_menu->bmpmenu) == BMPMENU_MOVE_DOWN ){

				AriCommMenu_ExplanationMain( d_menu );
			}
				
		
		}
		break;
	case FMS_TERM_TOPMENU:	//�폜
		
		if( d_menu->call_proc != NULL ){
			d_menu->call_proc( d_menu );
		}else{
			d_menu->seq_no = FMS_END;
		}
		
		d_menu->call_proc = NULL;
		
		if( d_menu->seq_no == FMS_END ){
			AriCommMenu_CloseTopMenu(d_menu);	
			return( TRUE );
		}
		break;

	//	���|�[�g�I�����P	
	case FMS_INIT_REPORT_FIRST:	//���j���[�쐬
		AriCommMenu_OpenYesNoMenu( d_menu );
		AriCommMenu_ChangeExplanation( d_menu , DEB_COMM_MENU_STR_20 );
		break;
	case FMS_LOOP_REPORT_FIRST:	//���j���[����
		{
			{
				const u32 ret = AriCommMenu_UpdateYesNoMenu( d_menu );
				switch( ret )
				{
				case YNR_YES:
					d_menu->seq_no = FMS_INIT_REPORT_SECOND;
					break;
					
				case YNR_NO:
					d_menu->seq_no = FMS_TERM_REPORT_FIRST;
					break;
				}
			}
		}
		break;

	case FMS_TERM_REPORT_FIRST:	//�폜
		d_menu->seq_no = FMS_END; 
		AriCommMenu_CloseSubMenu(d_menu);	
		AriCommMenu_CloseTopMenu(d_menu);	
		return( TRUE );

		break;	
		
	//	���|�[�g�I�����Q
	case FMS_INIT_REPORT_SECOND:	//���j���[�쐬
		AriCommMenu_ResetYesNoMenu( d_menu );
		AriCommMenu_ChangeExplanation( d_menu , DEB_COMM_MENU_STR_30 );
		d_menu->seq_no = FMS_LOOP_REPORT_SECOND;
		break;
	case FMS_LOOP_REPORT_SECOND:	//���j���[����
		{
			{
				const u32 ret = AriCommMenu_UpdateYesNoMenu( d_menu );
				switch( ret )
				{
				case YNR_YES:
					AriCommMenu_ChangeExplanation( d_menu , DEB_COMM_MENU_STR_40 );
					AriCommMenu_CloseMainMenu(d_menu);	
					AriCommMenu_CloseSubMenu(d_menu);	
					d_menu->seq_no = FMS_SAVE_WAIT1;
					break;
					
				case YNR_NO:
					d_menu->seq_no = FMS_TERM_REPORT_SECOND;
					break;
				}
			}
		}
		break;

	case FMS_TERM_REPORT_SECOND:	//�폜
		d_menu->seq_no = FMS_END; 
		AriCommMenu_CloseSubMenu(d_menu);	
		AriCommMenu_CloseTopMenu(d_menu);	
		return( TRUE );

		break;	
	case FMS_SAVE_WAIT1:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
		{
			AriCommMenu_ChangeExplanation( d_menu , DEB_COMM_MENU_STR_50 );
			d_menu->seq_no = FMS_SAVE_WAIT2;
		}
		break;
	case FMS_SAVE_WAIT2:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
		{
			d_menu->seq_no = FMS_GO_COMM_MAIN;
		}
		break;
	case FMS_GO_COMM_MAIN:
		AriCommMenu_CloseMsgWindow(d_menu);	
		d_menu->seq_no = FMS_INIT_SELECT_TYPE;
		d_menu->main_seq_no = CMS_COMM_MAIN_MENU;
		
		break;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�@���C��
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	BOOL	TRUE=�I��
 */
//--------------------------------------------------------------
BOOL	AriCommMenu_CommMainMenu( DEBUG_COMMMENU *d_menu )
{
    PRINTSYS_QUE_Main( d_menu->printQueMsg );
    if( PRINT_UTIL_Trans(d_menu->printUtilMsg,d_menu->printQueMsg) ){
    }
	switch( d_menu->seq_no )
	{
	case FMS_INIT_SELECT_TYPE:
		AriCommMenu_OpenConnectTypeMenu( d_menu );
		d_menu->seq_no = FMS_MAIN_SELECT_TYPE;
		break;
	
	case FMS_MAIN_SELECT_TYPE:
		{
			u32 ret;
		
			ret = BmpMenu_Main( d_menu->bmpmenu );
			PRINTSYS_QUE_Main( d_menu->printQue );
			if( PRINT_UTIL_Trans(d_menu->printUtil,d_menu->printQue) ){
			}
		
			switch( ret ){
			case BMPMENU_NULL:
				break;
			case BMPMENU_CANCEL:
				d_menu->seq_no = FMS_TERM_SELECT_TYPE;
				break;
				
			default:
				d_menu->seq_no = FMS_START_COMM_COMMON;
				d_menu->selectItemNo = BmpMenu_CursorPosGet(d_menu->bmpmenu);
				break;
			}
		}
		break;
	case FMS_TERM_SELECT_TYPE:
		d_menu->seq_no = FMS_END; 
		AriCommMenu_CloseMainMenu(d_menu);
		return(TRUE);
		break;

	case FMS_START_COMM_COMMON:	//�ʐM���ʏ�����
		AriCommMenu_CloseMainMenu(d_menu);
		FieldComm_InitData( d_menu->heapID );
		FieldComm_InitSystem();
		if( d_menu->selectItemNo == 1 ){ d_menu->seq_no = FMS_START_COMM_PARENT; }
		else			       { d_menu->seq_no = FMS_START_COMM_CHILD;  }
		break;

	case FMS_RETURN_CONNECT_TYPE:   //�ڑ���ʑI���֖߂�
		GFL_NET_Exit(NULL);
		d_menu->seq_no = FMS_INIT_SELECT_TYPE;
		break;

	case FMS_START_COMM_PARENT:	//�e�@�p������	
		if( FieldComm_IsFinish_InitSystem() )
		{
		    FieldComm_InitParent();
		    d_menu->seq_no = FMS_UPDATE_WAIT_CHILD;
		    AriCommMenu_OpenWaitChildMenu( d_menu );
		    AriCommMenu_OpenMsgWindow( d_menu , DEB_COMM_MENU_STR_70 );

		}
		break;

	case FMS_START_COMM_CHILD:	//�q�@�p������
		if( FieldComm_IsFinish_InitSystem() )
		{
		    FieldComm_InitChild();
		    d_menu->seq_no = FMS_UPDATE_SEARCH_PARENT;
		    AriCommMenu_OpenSearchParentMenu( d_menu );
		    AriCommMenu_OpenMsgWindow( d_menu , DEB_COMM_MENU_STR_80 ); 
		}
		break;

	case FMS_UPDATE_SEARCH_PARENT:   //�e�@�T����
		{
		    u32 ret;
		    ret = BmpMenu_Main( d_menu->bmpmenu );
		    PRINTSYS_QUE_Main( d_menu->printQue );
		    if( PRINT_UTIL_Trans(d_menu->printUtil,d_menu->printQue) ){
		    }
		    {
			const u8 findNum = FieldComm_UpdateSearchParent();
			AriCommMenu_UpdateSearchParentMenu( findNum , d_menu );
		    }
		    switch( ret ){
		    case BMPMENU_NULL:
			break;
		    case BMPMENU_CANCEL:
			d_menu->seq_no = FMS_TERM_SEARCH_PARENT;
			break;
				
		    default:
			{
			    const u8 selectPos = BmpMenu_CursorPosGet( d_menu->bmpmenu );
			    if( selectPos == FIELD_COMM_SEARCH_PARENT_NUM ){
				d_menu->seq_no = FMS_TERM_SEARCH_PARENT;
			    }
			    else{
				if( FieldComm_IsValidParentData( selectPos ) == TRUE ){
				    FieldComm_ConnectParent( selectPos );
				    d_menu->seq_no = FMS_INIT_WAIT_PARENT;
				}
				else{
				    //TODO:���s���ł�����
				}

			    }
			}
			break;
		    }
		}
		break;
	    case FMS_TERM_SEARCH_PARENT:
		AriCommMenu_CloseMainMenu(d_menu);
		d_menu->seq_no = FMS_RETURN_CONNECT_TYPE;
		break;

	    case FMS_UPDATE_WAIT_CHILD:	 //�q�@�ڑ��҂�
		{
		    const u32 ret = BmpMenu_Main( d_menu->bmpmenu );
		    AriCommMenu_UpdateWaitChildMenu( d_menu );
		    PRINTSYS_QUE_Main( d_menu->printQue );
		    if( PRINT_UTIL_Trans(d_menu->printUtil,d_menu->printQue) ){
		    }
		}
		break;

		
	    case FMS_INIT_WAIT_PARENT:	    //�e�@�J�n�҂�
		if( FieldComm_IsFinish_ConnectParent() == TRUE ){
		    d_menu->seq_no = FMS_LOOP_WAIT_PARENT;
		    FieldComm_SendSelfData();
		}
		break;
	    case FMS_LOOP_WAIT_PARENT:
		break;
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�@���C��
 * @param	d_menu	DEBUG_COMMMENU
 * @retval	BOOL	TRUE=�I��
 */
//--------------------------------------------------------------
BOOL AriFldMenu_Main( DEBUG_COMMMENU *d_menu )
{
	switch( d_menu->main_seq_no ){
	case CMS_COMM_START_MENU:
		return AriCommMenu_CommStartMenu( d_menu );
		break;
	case CMS_COMM_MAIN_MENU:
		return AriCommMenu_CommMainMenu( d_menu );
		break;
	}
	return( FALSE );
}
//======================================================================
//	�f�o�b�O���j���[�Ăяo���@�e�X�g
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�e�X�g
 * @param	wk	DEBUG_COMMMENU*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AriFldMenuCallProc_Test( void *wk )
{
//	DEBUG_COMMMENU *d_menu = wk;
//	HEAPID DebugHeapID = d_menu->heapID;
	OS_TPrintf("test outpur\n");
}


//======================================================================
//	���j���[���ڗp�R�[���o�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * TOP�����[���I��p
 * @param	wk	DEBUG_COMMMENU*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AriCommMenuCallProc_ToSelectRule( void *wk )
{
	DEBUG_COMMMENU *d_menu = wk;
	OS_TPrintf("go rule select\n");

	d_menu->seq_no = FMS_INIT_REPORT_FIRST;
}

//--------------------------------------------------------------
/**
 * TOP������
 * @param	wk	DEBUG_COMMMENU*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AriCommMenuCallProc_CloseMenu( void *wk )
{
	DEBUG_COMMMENU *d_menu = wk;
	d_menu->seq_no = FMS_END;
}


