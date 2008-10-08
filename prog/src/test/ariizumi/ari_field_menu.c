//======================================================================
/**
 *
 * @file	field_debug.c
 * @brief	�t�B�[���h�f�o�b�O
 * @author	kagaya
 * @data	08.09.29
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"
#include "system/bmp_menu.h"
#include "system/bmp_winframe.h"
#include "message.naix"

#include "test_graphic/d_taya.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_d_comm_menu.h"

#include "ari_field_menu.h"

//======================================================================
//	define
//======================================================================
#define DEBUG_BGFRAME_MENU (GFL_BG_FRAME1_M)	//�g�p�t���[��
#define DEBUG_BGFRAME_MSG  (GFL_BG_FRAME2_M)	//�g�p�t���[��
#define DEBUG_MENU_PANO	(14)
#define DEBUG_FONT_PANO	(15)

#define D_MENU_CHARSIZE_X (14)		//���j���[����
#define D_MENU_CHARSIZE_Y (18)		//���j���[�c��

#define D_MSG_CHAR_TOP	 ( 1)
#define D_MSG_CHAR_LEFT	 (20)

#define D_MSG_CHARSIZE_X (30)		//���b�Z�[�W�E�B���h�E����
#define D_MSG_CHARSIZE_Y ( 3)		//���b�Z�[�W�E�B���h�E�c��

#define D_MSG_STR_TOP	 (192)
#define D_MSG_STR_LEFT   (  8)

//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	���j���[�Ăяo���֐�
///	�Ăяo���ہA����void*��DEBUG_FLDMENU���w�肳��܂�
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
///	DEBUG_FLDMENU
//--------------------------------------------------------------
struct _DEBUG_FLDMENU
{
	int seq_no;
	
	u32 bgFrame;
	u32 msgFrame;
	HEAPID heapID;
	
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;
	
	GFL_BMP_DATA *msg_bmp;
	GFL_BMPWIN *msg_bmpwin;
	
	STRBUF *strbuf;
	GFL_MSGDATA *msgdata;
	GFL_FONT *fontHandle;
	PRINT_QUE *printQue;
	PRINT_UTIL printUtil[1];
	PRINT_QUE *printQueMsg;
	PRINT_UTIL printUtilMsg[1];
	ARCHANDLE *arcHandleMsg;
	
	BMP_MENULIST_DATA *menulistdata;
	BMPMENU_WORK *bmpmenu;
	
	D_MENU_CALLPROC call_proc;
};


//======================================================================
//	proto
//======================================================================
static void AriFldMenuCallProc_Test( void *wk );

//======================================================================
//	���j���[���X�g�ꗗ
//======================================================================
//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g�ꗗ
//--------------------------------------------------------------
static const DEBUG_MENU_LIST DATA_DebugMenuList[] =
{
	{ DEB_COMM_MENU_CAPTION1, AriFldMenuCallProc_Test },
	{ DEB_COMM_MENU_2MODE_1	, AriFldMenuCallProc_Test },
	{ DEB_COMM_MENU_2MODE_2 , AriFldMenuCallProc_Test },
	{ DEB_COMM_MENU_2MODE_3 , AriFldMenuCallProc_Test },
	{ DEB_COMM_MENU_CAPTION2, AriFldMenuCallProc_Test },
	{ DEB_COMM_MENU_4MODE_1	, AriFldMenuCallProc_Test },
	{ DEB_COMM_MENU_4MODE_2	, AriFldMenuCallProc_Test },
	{ DEB_COMM_MENU_MANUAL	, AriFldMenuCallProc_Test },
	{ DEB_COMM_MENU_END     , AriFldMenuCallProc_Test },
};

//���j���[�ő吔
#define D_MENULIST_MAX (NELEMS(DATA_DebugMenuList))

//======================================================================
//	�t�B�[���h�f�o�b�O���j���[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�@������
 * @param	heapID	�q�[�vID
 * @retval	DEBUG_FLDMENU
 */
//--------------------------------------------------------------
DEBUG_FLDMENU *AriFldMenu_Init( u32 heapID )
{
	DEBUG_FLDMENU *d_menu;
	d_menu = GFL_HEAP_AllocClearMemory( heapID, sizeof(DEBUG_FLDMENU) );
	
	d_menu->heapID = heapID;
	d_menu->bgFrame = DEBUG_BGFRAME_MENU;
	d_menu->msgFrame = DEBUG_BGFRAME_MSG;
	
	{	//bmp font �����ꃁ�C������
		GFL_BMPWIN_Init( d_menu->heapID );
		GFL_BMPWIN_Init( d_menu->heapID );
		GFL_FONTSYS_Init();
	}
	
	{	//BG������ �����ꃁ�C�����̂𗘗p����`��
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_BGCNT_HEADER bgcntTextMsg = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
	
		GFL_BG_SetBGControl( d_menu->bgFrame, &bgcntText   , GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( d_menu->msgFrame , &bgcntTextMsg, GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( d_menu->bgFrame, VISIBLE_ON );
		GFL_BG_SetVisible( d_menu->msgFrame , VISIBLE_ON );
		
		GFL_BG_SetPriority( d_menu->bgFrame, 1 );
		GFL_BG_SetPriority( d_menu->msgFrame , 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME0_M, 2 );

		GFL_ARC_UTIL_TransVramPalette(
			ARCID_D_TAYA, NARC_d_taya_default_nclr,
			PALTYPE_MAIN_BG, DEBUG_FONT_PANO*32, 32, d_menu->heapID );

		GFL_BG_FillCharacter( d_menu->bgFrame, 0x00, 1, 0 );
		GFL_BG_FillCharacter( d_menu->msgFrame , 0x00, 1, 0 );
		GFL_BG_FillScreen( d_menu->bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_FillScreen( d_menu->msgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_LoadScreenReq( d_menu->bgFrame );
		GFL_BG_LoadScreenReq( d_menu->msgFrame );
	}
	
	return( d_menu );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�@�폜
 * @param	d_menu	DEBUG_FLDMENU
 * @retval	nMENU_TYPE_SYSTEMothing
 */
//--------------------------------------------------------------
void AriFldMenu_Delete( DEBUG_FLDMENU *d_menu )
{
	{	//�Ƃ肠����������Ł@������̓��C����
		GFL_BMPWIN_Exit();
		GFL_BG_FreeCharacterArea( d_menu->bgFrame, 0x00, 0x20 );
		GFL_BG_FreeBGControl( d_menu->bgFrame );
		GFL_BG_FreeCharacterArea( d_menu->msgFrame, 0x00, 0x20 );
		GFL_BG_FreeBGControl( d_menu->msgFrame );
	}
	
	GFL_HEAP_FreeMemory( d_menu );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�@���j���[�쐬
 * @param	d_menu	DEBUG_FLDMENU
 * @retval	nothing
 */
//--------------------------------------------------------------
void AriFldMenu_Create( DEBUG_FLDMENU *d_menu )
{
	{	//work
		d_menu->seq_no = 0;
	}
	
	{	//window frame
		BmpWinFrame_GraphicSet( d_menu->bgFrame, 1,
			DEBUG_MENU_PANO, MENU_TYPE_SYSTEM, d_menu->heapID );
	}
	
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
		
		d_menu->strbuf = GFL_STR_CreateBuffer( 2048, d_menu->heapID );
		
		d_menu->arcHandleMsg = GFL_ARC_OpenDataHandle(
				ARCID_D_TAYA, d_menu->heapID );
		
		d_menu->fontHandle = GFL_FONT_CreateHandle(
			d_menu->arcHandleMsg, NARC_d_taya_lc12_2bit_nftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, d_menu->heapID );

		PRINTSYS_Init( d_menu->heapID );
		d_menu->printQue = PRINTSYS_QUE_Create( d_menu->heapID );
		d_menu->printQueMsg = PRINTSYS_QUE_Create( d_menu->heapID );
	
		PRINT_UTIL_Setup( d_menu->printUtil, d_menu->bmpwin );	
		//PRINT_UTIL_Setup( d_menu->printUtilMsg, d_menu->msg_bmpwin );
		d_menu->printUtilMsg->win = d_menu->msg_bmpwin;
		d_menu->printUtilMsg->transReq = FALSE;
	}

	//���b�Z�[�W�E�B���h�E�p������
	{	//window frame
		BmpWinFrame_GraphicSet( d_menu->msgFrame, 1,
			DEBUG_MENU_PANO, MENU_TYPE_SYSTEM, d_menu->heapID );
	}
	
	{	//bmpwin
		d_menu->msg_bmpwin = GFL_BMPWIN_Create( d_menu->msgFrame,
			D_MSG_CHAR_TOP , D_MSG_CHAR_LEFT , 
			D_MSG_CHARSIZE_X, D_MSG_CHARSIZE_Y,
			DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
		d_menu->msg_bmp = GFL_BMPWIN_GetBmp( d_menu->msg_bmpwin );
		
		GFL_BMP_Clear( d_menu->msg_bmp, 0xff );
		GFL_BMPWIN_MakeScreen( d_menu->msg_bmpwin );
		
		GFL_BMPWIN_TransVramCharacter( d_menu->msg_bmpwin );
		GFL_BG_LoadScreenReq( d_menu->bgFrame );
	}
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�@���������C��
 * @param	d_menu	DEBUG_FLDMENU
 * @retval	void	
 */
//--------------------------------------------------------------
void	AriFldMenu_ExplanationMain( DEBUG_FLDMENU *d_menu );
void	AriFldMenu_ExplanationMain( DEBUG_FLDMENU *d_menu )
{
	//����������b�Z�[�W�f�[�^��������o�����߂̃o�b�t�@
	STRBUF *strTemp;
				
	strTemp = GFL_STR_CreateBuffer( 64, d_menu->heapID );
	GFL_MSG_GetString( d_menu->msgdata , DEB_COMM_MENU_STR_10 , strTemp );

	PRINT_UTIL_Print(
		d_menu->printUtilMsg, d_menu->printQueMsg,
		D_MSG_STR_TOP, D_MSG_STR_LEFT , (void*)strTemp, d_menu->fontHandle );
	GFL_STR_DeleteBuffer( strTemp );
}


//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�@���C��
 * @param	d_menu	DEBUG_FLDMENU
 * @retval	BOOL	TRUE=�I��
 */
//--------------------------------------------------------------
BOOL AriFldMenu_Main( DEBUG_FLDMENU *d_menu )
{
	switch( d_menu->seq_no ){
	case 0:	//���j���[�쐬
		{	//window frame
			BmpWinFrame_Write( d_menu->bmpwin,
				WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
			BmpWinFrame_Write( d_menu->msg_bmpwin,
				WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
		}
		
		{	//menu create
			u32 i,lmax;
			BMPMENU_HEADER head;
			const DEBUG_MENU_LIST *d_menu_list;
			
			lmax = D_MENULIST_MAX;
			
			head.x_max = 1;
			head.y_max = lmax;
			head.line_spc = 4;
			head.c_disp_f = 0;
			head.loop_f = 1;
			
			head.font_size_x = 12;
			head.font_size_y = 12;
			head.msgdata = d_menu->msgdata;
			head.print_util = d_menu->printUtil;
			head.print_que = d_menu->printQue;
			head.font_handle = d_menu->fontHandle;
			head.win = d_menu->bmpwin;

			d_menu->menulistdata =
				BmpMenuWork_ListCreate( lmax, d_menu->heapID );
			
			d_menu_list = DATA_DebugMenuList;
			
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
		
		d_menu->seq_no++;
		break;
	case 1:	//���j���[����
		{
			u32 ret;
			
			ret = BmpMenu_Main( d_menu->bmpmenu );
			PRINTSYS_QUE_Main( d_menu->printQue );
			
			if( PRINT_UTIL_Trans(d_menu->printUtil,d_menu->printQue) ){
			}
			
			PRINTSYS_QUE_Main( d_menu->printQue );
			if( PRINT_UTIL_Trans(d_menu->printUtilMsg,d_menu->printQueMsg) ){
			}
			
			switch( ret ){
			case BMPMENU_NULL:
				break;
			case BMPMENU_CANCEL:
				GFL_BMP_Clear( d_menu->bmp, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
				GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
				BmpWinFrame_Clear( d_menu->bmpwin, 0 );
				
				GFL_BMP_Clear( d_menu->msg_bmp, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->msg_bmpwin );
				GFL_BMPWIN_TransVramCharacter( d_menu->msg_bmpwin );
				BmpWinFrame_Clear( d_menu->msg_bmpwin, 0 );
				
				d_menu->seq_no++;
				break;
			default:
				GFL_BMP_Clear( d_menu->bmp, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
				GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
				BmpWinFrame_Clear( d_menu->bmpwin, 0 );
				
				GFL_BMP_Clear( d_menu->msg_bmp, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->msg_bmpwin );
				GFL_BMPWIN_TransVramCharacter( d_menu->msg_bmpwin );
				BmpWinFrame_Clear( d_menu->msg_bmpwin, 0 );
				
				d_menu->call_proc = (D_MENU_CALLPROC)ret;
				d_menu->seq_no++;
				break;
			}
			
			if(	BmpMenu_MoveSiteGet(d_menu->bmpmenu) == BMPMENU_MOVE_UP || 
				BmpMenu_MoveSiteGet(d_menu->bmpmenu) == BMPMENU_MOVE_DOWN ){

				AriFldMenu_ExplanationMain( d_menu );
			}
				
		
		}
		break;
	case 2:	//�폜
		BmpMenu_Exit( d_menu->bmpmenu, NULL );
		BmpMenuWork_ListSTRBUFDelete( d_menu->menulistdata );
		
		//�v�����g���[�e�B���e�B�폜�@����Ȃ�
		PRINTSYS_QUE_Delete( d_menu->printQue );
		
		FontDataMan_Delete( d_menu->fontHandle );
//		GFL_ARC_CloseDataHandle( d_menu->arcHandleMsg ); //�}�l�[�W�����ōς�
		GFL_STR_DeleteBuffer( d_menu->strbuf );
		GFL_MSG_Delete( d_menu->msgdata );
		GFL_BMPWIN_Delete( d_menu->bmpwin );
		GFL_BMPWIN_Delete( d_menu->msg_bmpwin );
		
		if( d_menu->call_proc != NULL ){
			d_menu->call_proc( d_menu );
		}
		
		d_menu->call_proc = NULL;
		return( TRUE );
	}
	
	return( FALSE );
}

//======================================================================
//	�f�o�b�O���j���[�Ăяo���@�e�X�g
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�e�X�g
 * @param	wk	DEBUG_FLDMENU*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AriFldMenuCallProc_Test( void *wk )
{
//	DEBUG_FLDMENU *d_menu = wk;
//	HEAPID DebugHeapID = d_menu->heapID;
	OS_TPrintf("test outpur\n");
}
