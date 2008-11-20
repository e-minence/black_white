//======================================================================
/**
 *
 * @file	event_debug_menu.c
 * @brief	�t�B�[���h�f�o�b�O���j���[�̐���C�x���g
 * @author	kagaya
 * @date	2008.11.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "arc_def.h"

#include "system/bmp_menu.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "message.naix"
#include "msg/msg_d_field.h"
#include "test_graphic/d_taya.naix"

#include "field/zonedata.h"

#include "event_debug_menu.h"

#include "field_comm/field_comm_main.h"

//======================================================================
//	define
//======================================================================
#define DEBUG_BGFRAME_MENU (GFL_BG_FRAME1_M)	//�g�p�t���[��
#define DEBUG_BGFRAME_MSG  (GFL_BG_FRAME2_M)	//�g�p�t���[��
#define DEBUG_MENU_PANO	(14)
#define DEBUG_FONT_PANO	(15)

#define D_MENU_CHARSIZE_X (8)		//���j���[����
#define D_MENU_CHARSIZE_Y (16)		//���j���[�c��

//--------------------------------------------------------------
///	DMENURET
//--------------------------------------------------------------
typedef enum
{
	DMENURET_CONTINUE,
	DMENURET_FINISH,
	DMENURET_NEXTEVENT,
}DMENURET;

//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	DEBUG_MENU_EVENT_WORK
//--------------------------------------------------------------
typedef struct {
	DEBUG_FLDMENU *d_menu;
	FIELD_MAIN_WORK * fieldWork;
	HEAPID heapID;
	u16 page_id;
}DEBUG_MENU_EVENT_WORK;

//--------------------------------------------------------------
///	���j���[�Ăяo���֐�
///	BOOL TRUE=�C�x���g�p�� FALSE==�f�o�b�O���j���[�C�x���g�I��
//--------------------------------------------------------------
typedef BOOL (* D_MENU_CALLPROC)(DEBUG_FLDMENU*);

//--------------------------------------------------------------
///	DEBUG_MENU_LIST
//--------------------------------------------------------------
typedef struct
{
	u32 str_id;				//�\�����镶����ID
	D_MENU_CALLPROC callp;	//�I�����ꂽ�ۂɌĂяo���֐� NULL=�Ăяo������
}DEBUG_MENU_LIST;

//--------------------------------------------------------------
///	DEBUG_MENU_LISTTBL
//--------------------------------------------------------------
typedef struct
{
	u16 charsize_x;
	u16 charsize_y;
	u32 max;
	const DEBUG_MENU_LIST *list;
}DEBUG_MENU_LISTTBL;

//--------------------------------------------------------------
///	DEBUG_FLDMENU
//--------------------------------------------------------------
struct _TAG_DEBUG_FLDMENU
{
	HEAPID heapID;			//�f�o�b�O�p�q�[�vID
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	
	u32 menu_num;

	int seq_no;
	u32 bgFrame;
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;
	
	STRBUF *strbuf;
	GFL_MSGDATA *msgdata;
	GFL_FONT *fontHandle;
	PRINT_QUE *printQue;
	PRINT_UTIL printUtil[1];
	
	BMP_MENULIST_DATA *menulistdata;
	BMPMENU_WORK *bmpmenu;
	D_MENU_CALLPROC call_proc;

	//�ʐM���j���[�p
	int commSeq;	//�ǉ� Ari1111
	FIELD_COMM_MAIN	*commSys;
	
	void *pUserWork;
};

//======================================================================
//	proto
//======================================================================
static DEBUG_FLDMENU * FldDebugMenu_Init(
 GMEVENT *gmEvent, FIELD_MAIN_WORK *fieldWork, u32 menu_num, u32 heapID );
static void FldDebugMenu_Delete( DEBUG_FLDMENU *d_menu );
static void FldDebugMenu_Create( DEBUG_FLDMENU *d_menu );
static DMENURET FldDebugMenu_Main( DEBUG_FLDMENU *d_menu );

static BOOL DMenuCallProc_GridCamera( DEBUG_FLDMENU *wk );
static BOOL DMenuCallProc_GridScaleSwitch( DEBUG_FLDMENU *wk );
static BOOL DMenuCallProc_GridScaleControl( DEBUG_FLDMENU *wk );

static BOOL DMenuCallProc_OpenStartComm( DEBUG_FLDMENU *wk );
static BOOL DMenuCallProc_OpenStartInvasion( DEBUG_FLDMENU *wk );

static BOOL DMenuCallProc_MapZoneSelect( DEBUG_FLDMENU *wk );

static void DEBUG_SetMenuWorkZoneIDName(
		BMP_MENULIST_DATA *list, HEAPID heapID );

//======================================================================
//	���j���[���X�g�ꗗ
//======================================================================
//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g�@�ėp
//--------------------------------------------------------------
static const DEBUG_MENU_LIST DATA_DebugMenuList[] =
{
	{ DEBUG_FIELD_C_CHOICE00, DMenuCallProc_OpenStartComm },
	{ DEBUG_FIELD_C_CHOICE01, DMenuCallProc_OpenStartInvasion },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
};

//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g	�O���b�h�����}�b�v�p
//--------------------------------------------------------------
static const DEBUG_MENU_LIST DATA_DebugMenuListGrid[] =
{
	{ DEBUG_FIELD_STR02, DMenuCallProc_GridCamera },
	{ DEBUG_FIELD_STR03, DMenuCallProc_GridScaleSwitch },
	{ DEBUG_FIELD_STR04, DMenuCallProc_GridScaleControl },
	{ DEBUG_FIELD_STR05, DMenuCallProc_MapZoneSelect },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_STR01, NULL },
	{ DEBUG_FIELD_C_CHOICE00, DMenuCallProc_OpenStartComm },
	{ DEBUG_FIELD_C_CHOICE01, DMenuCallProc_OpenStartInvasion },
//	{ DEBUG_FIELD_STR01, NULL },
//	{ DEBUG_FIELD_STR01, NULL },
};

//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g�e�[�u��
//--------------------------------------------------------------
static const DEBUG_MENU_LISTTBL DATA_DebugMenuListTbl[] =
{
	{	//�����}�b�v ��
		D_MENU_CHARSIZE_X,
		D_MENU_CHARSIZE_Y,
		NELEMS(DATA_DebugMenuList),
		DATA_DebugMenuList
	},
	{	//�����}�b�v�@�O���b�h�ړ�
		11,
		16,
		NELEMS(DATA_DebugMenuListGrid),
		DATA_DebugMenuListGrid
	},
	{	//�����}�b�v�@��
		D_MENU_CHARSIZE_X,
		D_MENU_CHARSIZE_Y,
		NELEMS(DATA_DebugMenuList),
		DATA_DebugMenuList
	},
	{	//�����}�b�v�@�O���b�h�ړ�
		11,
		16,
		NELEMS(DATA_DebugMenuListGrid),
		DATA_DebugMenuListGrid
	},
};

//���j���[�ő吔
#define D_MENULISTTBL_MAX (NELEMS(DATA_DebugMenuListTbl))

//======================================================================
//	�C�x���g�F�t�B�[���h�f�o�b�O���j���[
//======================================================================
//--------------------------------------------------------------
/**
 * �C�x���g�F�t�B�[���h�f�o�b�O���j���[
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	work	event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *work )
{
	DEBUG_MENU_EVENT_WORK *dmew = work;
	
	switch (*seq) {
	case 0:
		dmew->d_menu = FldDebugMenu_Init(
			event, dmew->fieldWork, dmew->page_id, dmew->heapID );
		++ *seq;
		break;
	case 1:
		FldDebugMenu_Create( dmew->d_menu );
		++ *seq;
		break;
	case 2:
		{
			DEBUG_FLDMENU *d_menu = dmew->d_menu;
			DMENURET ret =  FldDebugMenu_Main( d_menu );
			
			if( ret == DMENURET_CONTINUE ){
				break;
			}
			
			FldDebugMenu_Delete( d_menu );
			
			if( ret == DMENURET_FINISH ){
				return( GMEVENT_RES_FINISH );
			}
		}
		break;
	}

	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�C�x���g�N��
 * @param	gsys	GAMESYS_WORK
 * @param	fieldWork	FIELD_MAIN_WORK
 * @param	heapID	HEAPID
 * @param	page_id	�f�o�b�O���j���[�y�[�W
 * @retval	GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_DebugMenu(
	GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork,
	HEAPID heapID, u16 page_id )
{
	DEBUG_MENU_EVENT_WORK * dmew;
	GMEVENT * event;
	event = GMEVENT_Create(
		gsys, NULL, DebugMenuEvent, sizeof(DEBUG_MENU_EVENT_WORK));
	dmew = GMEVENT_GetEventWork(event);
	dmew->d_menu = NULL;
	dmew->fieldWork = fieldWork;
	dmew->heapID = heapID;
	dmew->page_id = page_id;
	return event;
}

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
static DEBUG_FLDMENU * FldDebugMenu_Init(
	GMEVENT *gmEvent, FIELD_MAIN_WORK *fieldWork, u32 menu_num, u32 heapID )
{
	DEBUG_FLDMENU *d_menu;
	
	d_menu = GFL_HEAP_AllocClearMemory( heapID, sizeof(DEBUG_FLDMENU) );
	
	d_menu->gmEvent = gmEvent;
	d_menu->heapID = heapID;
	d_menu->fieldWork = fieldWork;
	d_menu->bgFrame = DEBUG_BGFRAME_MENU;
	
	if( menu_num >= D_MENULISTTBL_MAX ){
		OS_Printf( "debug menu number error\n" );
		menu_num = 0;
	}
	
	d_menu->menu_num = menu_num;
	
	{	//BG������ �����ꃁ�C�����̂𗘗p����`��
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl(
			d_menu->bgFrame, &bgcntText, GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( d_menu->bgFrame, VISIBLE_ON );
		
		GFL_BG_SetPriority( d_menu->bgFrame, 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME0_M, 2 );
		
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_D_TAYA, NARC_d_taya_default_nclr,
			PALTYPE_MAIN_BG, DEBUG_FONT_PANO*32, 32, d_menu->heapID );
		
		GFL_BG_FillCharacter( d_menu->bgFrame, 0x00, 1, 0 );
		GFL_BG_FillScreen( d_menu->bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_LoadScreenReq( d_menu->bgFrame );
	}

	{
		//�����P�ʒǉ����܂� Ari1113
		GFL_BG_BGCNT_HEADER msgBgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		const u8 msgBgFrame = DEBUG_BGFRAME_MSG;
		
		GFL_BG_SetBGControl(
			msgBgFrame, &msgBgcntText, GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( msgBgFrame, VISIBLE_ON );
		
		GFL_BG_SetPriority( msgBgFrame, 1 );

		GFL_BG_FillCharacter( msgBgFrame, 0x00, 1, 0 );
		GFL_BG_FillScreen( msgBgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_LoadScreenReq( msgBgFrame );
	}

	d_menu->commSys = (FIELD_COMM_MAIN*)FieldMain_GetCommSys( fieldWork );
	
	return( d_menu );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O���j���[�@�폜
 * @param	d_menu	DEBUG_FLDMENU
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldDebugMenu_Delete( DEBUG_FLDMENU *d_menu )
{
	const u8 msgBgFrame = DEBUG_BGFRAME_MSG;
	
	{	//�Ƃ肠����������Ł@������̓��C����
		GFL_BG_FreeCharacterArea( d_menu->bgFrame, 0x00, 0x20 );
		GFL_BG_FreeBGControl( d_menu->bgFrame );
		GFL_BG_FreeCharacterArea( msgBgFrame, 0x00, 0x20 );
		GFL_BG_FreeBGControl( msgBgFrame );
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
static void FldDebugMenu_Create( DEBUG_FLDMENU *d_menu )
{
	{	//work
		d_menu->seq_no = 0;
	}
	
	{	//window frame
		BmpWinFrame_GraphicSet( d_menu->bgFrame, 1,
			DEBUG_MENU_PANO, MENU_TYPE_SYSTEM, d_menu->heapID );
	}
	
	{	//bmpwin
		const DEBUG_MENU_LISTTBL *d_menu_tbl;
		d_menu_tbl = &DATA_DebugMenuListTbl[d_menu->menu_num];

		d_menu->bmpwin = GFL_BMPWIN_Create( d_menu->bgFrame,
			1, 1, d_menu_tbl->charsize_x, d_menu_tbl->charsize_y,
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
			NARC_message_d_field_dat, d_menu->heapID );
		
		d_menu->strbuf = GFL_STR_CreateBuffer( 1024, d_menu->heapID );
		
		d_menu->fontHandle = GFL_FONT_Create(
			ARCID_D_TAYA,
			NARC_d_taya_lc12_2bit_nftr,
			GFL_FONT_LOADTYPE_FILE,
			FALSE,
			d_menu->heapID );

		d_menu->printQue = PRINTSYS_QUE_Create( d_menu->heapID );
		PRINT_UTIL_Setup( d_menu->printUtil, d_menu->bmpwin );	
	}
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�@���C��
 * @param	d_menu	DEBUG_FLDMENU
 * @retval	DMENURET	DMENURET
 */
//--------------------------------------------------------------
static DMENURET FldDebugMenu_Main( DEBUG_FLDMENU *d_menu )
{
	switch( d_menu->seq_no ){
	case 0:	//���j���[�쐬
		{	//window frame
			BmpWinFrame_Write( d_menu->bmpwin,
				WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
		}

		{	//menu create
			u32 i,lmax;
			BMPMENU_HEADER head;
			const DEBUG_MENU_LIST *d_menu_list;
			const DEBUG_MENU_LISTTBL *d_menu_tbl;
			
			d_menu_tbl = &DATA_DebugMenuListTbl[d_menu->menu_num];
			d_menu_list = d_menu_tbl->list;
			lmax = d_menu_tbl->max;
			
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
			
			for( i = 0; i < lmax; i++ ){
				BmpMenuWork_ListAddArchiveString(
					d_menu->menulistdata, d_menu->msgdata,
					d_menu_list[i].str_id, (u32)d_menu_list[i].callp,
					d_menu->heapID );
			}
			
			head.menu = d_menu->menulistdata;
			
			d_menu->bmpmenu = BmpMenu_Add( &head, 0, d_menu->heapID );
			BmpMenu_SetCursorString( d_menu->bmpmenu, DEBUG_FIELD_STR00 );
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
			
			switch( ret ){
			case BMPMENU_NULL:
				break;
			case BMPMENU_CANCEL:
				GFL_BMP_Clear( d_menu->bmp, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
				GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
				BmpWinFrame_Clear( d_menu->bmpwin, 0 );
				d_menu->seq_no++;
				break;
			default:
				GFL_BMP_Clear( d_menu->bmp, 0x00 );
				GFL_BMPWIN_MakeScreen( d_menu->bmpwin );
				GFL_BMPWIN_TransVramCharacter( d_menu->bmpwin );
				BmpWinFrame_Clear( d_menu->bmpwin, 0 );
				d_menu->call_proc = (D_MENU_CALLPROC)ret;
				d_menu->seq_no++;
				break;
			}
		}
		break;
	case 2:	//�폜
		{
			BOOL ret = FALSE;

			BmpMenu_Exit( d_menu->bmpmenu, NULL );
			BmpMenuWork_ListDelete( d_menu->menulistdata );	//freeheap
			
			PRINTSYS_QUE_Delete( d_menu->printQue );
			GFL_FONT_Delete( d_menu->fontHandle );
			
			GFL_STR_DeleteBuffer( d_menu->strbuf );
			GFL_MSG_Delete( d_menu->msgdata );
			GFL_BMPWIN_Delete( d_menu->bmpwin );
			
			if( d_menu->call_proc != NULL ){
				ret = d_menu->call_proc( d_menu );
			}
			
			d_menu->call_proc = NULL;
			
			//�ʐM�ɍs���Ƃ��͂��̂܂܂ɂ������̂�
			if( d_menu->seq_no == 2 ){
				if( ret == TRUE ){
					return( DMENURET_NEXTEVENT );
				}
				
				return( DMENURET_FINISH );
			}
		}
	case 10:	//�ʐM���j���[�p����
		switch( d_menu->commSeq )
		{
		case 0:
			FIELD_COMM_MAIN_InitStartCommMenu( d_menu->commSys );
			d_menu->commSeq++;
			break;
		case 1:
			if( FIELD_COMM_MAIN_LoopStartCommMenu( d_menu->commSys ) == TRUE ){
				d_menu->commSeq++;
			}
			break;
		case 2:
			FIELD_COMM_MAIN_TermStartCommMenu( d_menu->commSys );
			d_menu->commSeq++;
			return (DMENURET_FINISH);
			break;
		}
		break;
	case 11:
		switch( d_menu->commSeq )
		{
		case 0:
			FIELD_COMM_MAIN_InitStartInvasionMenu( d_menu->commSys );
			d_menu->commSeq++;
			break;
		case 1:
			if( FIELD_COMM_MAIN_LoopStartInvasionMenu( d_menu->commSys ) == TRUE ){
				d_menu->commSeq++;
			}
			break;
		case 2:
			FIELD_COMM_MAIN_TermStartInvasionMenu( d_menu->commSys );
			d_menu->commSeq++;
			return (DMENURET_FINISH);
			break;
		}
		break;
	}
	
	return( DMENURET_CONTINUE );
}

void FldDebugMenu_SetCommSystem( void *commSys , DEBUG_FLDMENU *d_menu )
{
	d_menu->commSys = (FIELD_COMM_MAIN*)commSys;
}

//======================================================================
//	�f�o�b�O���j���[�Ăяo��
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�O���b�h�p�J����
 * @param	wk	DEBUG_FLDMENU*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridCamera( DEBUG_FLDMENU *wk )
{
	DEBUG_FLDMENU *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_Camera( fieldWork );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�O���b�h�p�X�P�[���؂�ւ�
 * @param	wk	DEBUG_FLDMENU*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridScaleSwitch( DEBUG_FLDMENU *wk )
{
	DEBUG_FLDMENU *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_ScaleChange( fieldWork );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�O���b�h�p�X�P�[������
 * @param	wk	DEBUG_FLDMENU*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridScaleControl( DEBUG_FLDMENU *wk )
{
	DEBUG_FLDMENU *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_ScaleControl( fieldWork );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�ʐM�J�n���j���[
 * @param	wk	DEBUG_FLDMENU*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_OpenStartComm( DEBUG_FLDMENU *wk )
{
	wk->seq_no = 10;
	wk->commSeq = 0;
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�N���J�n���j���[
 * @param	wk	DEBUG_FLDMENU*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_OpenStartInvasion( DEBUG_FLDMENU *wk )
{
	wk->seq_no = 11;
	wk->commSeq = 0;
	return( FALSE );
}

//======================================================================
//	�f�o�b�O���j���[ �ǂ��ł��W�����v
//======================================================================
//--------------------------------------------------------------
///	DEBUG_ZONESEL �ǂ��ł��W�����v�����p���[�N
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	FIELD_MAIN_WORK *fieldWork;

	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;
	BMP_MENULIST_DATA *pMenuList;
	BMPMENULIST_WORK *pMenuListWork;

	STRBUF *strbuf;
	GFL_MSGDATA *msgdata;
	GFL_FONT *fontHandle;
	PRINT_QUE *printQue;
	PRINT_UTIL printUtil[1];
}DEBUG_ZONESEL;

//--------------------------------------------------------------
///	�ǂ��ł��W�����v�@���j���[�w�b�_�[
//--------------------------------------------------------------
static const BMPMENULIST_HEADER DATA_DebugMenuList_ZoneSel =
{
	NULL,	//�\�������f�[�^�|�C���^
	NULL,	//�J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
	NULL,	//���\�����Ƃ̃R�[���o�b�N�֐�
	NULL,	//GFL_BMPWIN

	1,		//���X�g���ڐ�
	12,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	11,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	BMPMENULIST_LRKEY_SKIP,	//�y�[�W�X�L�b�v�^�C�v
	0,		//�����w��(�{����u8�����ǂ���Ȃɍ��Ȃ��Ǝv���̂�)
	0,		//�a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
	
	NULL,	//���[�N�|�C���^
	
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	
	NULL,	//�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
	NULL,	//�\���Ɏg�p����v�����g���[�e�B���e�B
	NULL,	//�\���Ɏg�p����v�����g�L���[
	NULL,	//�\���Ɏg�p����t�H���g�n���h��
};

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuZoneSelectEvent(
		GMEVENT *event, int *seq, void *work );

static void DMenuZoneSelect_Init( DEBUG_ZONESEL *work );
static int DMenuZoneSelect_Main( DEBUG_ZONESEL *work );

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�ǂ��ł��W�����v
 * @param	wk	DEBUG_FLDMENU*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_MapZoneSelect( DEBUG_FLDMENU *wk )
{
	GMEVENT *event;
	DEBUG_ZONESEL *work;
	
	event = wk->gmEvent;
	GMEVENT_Change( event, DMenuZoneSelectEvent, sizeof(DEBUG_ZONESEL) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_ZONESEL) );

	work->heapID = wk->heapID;
	work->fieldWork = wk->fieldWork;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�ǂ��ł��W�����v
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuZoneSelectEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_ZONESEL *work = wk;
	
	switch( (*seq) ){
	case 0:
		DMenuZoneSelect_Init( work );
		(*seq)++;
		break;
	case 1:
		if( DMenuZoneSelect_Main(work) == TRUE ){
			return( GMEVENT_RES_FINISH );
		}
		break;
	}

	return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * �ǂ��ł��W�����v�@������
 */
//--------------------------------------------------------------
static void DMenuZoneSelect_Init( DEBUG_ZONESEL *work )
{
	{	//graphic init
		u32 bgFrame = DEBUG_BGFRAME_MENU;
		
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl(
			bgFrame, &bgcntText, GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( bgFrame, VISIBLE_ON );
		
		GFL_BG_SetPriority( bgFrame, 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME0_M, 2 );
		
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_D_TAYA, NARC_d_taya_default_nclr,
			PALTYPE_MAIN_BG, DEBUG_FONT_PANO*32, 32, work->heapID );
		
		GFL_BG_FillCharacter( bgFrame, 0x00, 1, 0 );
		GFL_BG_FillScreen( bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( bgFrame );
	}

	{	//window frame
		BmpWinFrame_GraphicSet( DEBUG_BGFRAME_MENU, 1,
				DEBUG_MENU_PANO, MENU_TYPE_SYSTEM, work->heapID );
	}
	
	{	//bmp window
		work->bmpwin = GFL_BMPWIN_Create(
			DEBUG_BGFRAME_MENU,
			1, 1, 11, 16,
			DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
		work->bmp = GFL_BMPWIN_GetBmp( work->bmpwin );
		
		GFL_BMP_Clear( work->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( work->bmpwin );
		
		GFL_BMPWIN_TransVramCharacter( work->bmpwin );
		GFL_BG_LoadScreenReq( DEBUG_BGFRAME_MENU );
	}
	
	{	//message
		work->msgdata = GFL_MSG_Create(
			GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
			NARC_message_d_field_dat, work->heapID );
		
		work->strbuf = GFL_STR_CreateBuffer( 1024, work->heapID );

		work->fontHandle = GFL_FONT_Create(
			ARCID_D_TAYA,
			NARC_d_taya_lc12_2bit_nftr,
			GFL_FONT_LOADTYPE_FILE,
			FALSE,
			work->heapID );

		work->printQue = PRINTSYS_QUE_Create( work->heapID );
		PRINT_UTIL_Setup( work->printUtil, work->bmpwin );
	}
}

//--------------------------------------------------------------
/**
 * �ǂ��ł��W�����v�@���C��
 */
//--------------------------------------------------------------
static int DMenuZoneSelect_Main( DEBUG_ZONESEL *work )
{
	switch( work->seq_no ){
	case 0:
		{	//Menu List
			int i;
			u32 max = ZONEDATA_GetZoneIDMax();
			BMPMENULIST_HEADER head = DATA_DebugMenuList_ZoneSel;
			BmpWinFrame_Write( work->bmpwin,
				WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
			work->pMenuList = BmpMenuWork_ListCreate( max, work->heapID );
			DEBUG_SetMenuWorkZoneIDName( work->pMenuList, work->heapID );
			
			head.count = max;
			head.msgdata = work->msgdata;
			head.print_util = work->printUtil;
			head.print_que = work->printQue;
			head.font_handle = work->fontHandle;
			head.win = work->bmpwin;
			head.list = work->pMenuList;
			
			work->pMenuListWork =
				BmpMenuList_Set( &head, 0, 0, work->heapID );
			BmpMenuList_SetCursorString( work->pMenuListWork, 0 );
		}
		
		work->seq_no++;
		break;
	case 1:
		{
			u32 ret;
			ret = BmpMenuList_Main( work->pMenuListWork );
			PRINTSYS_QUE_Main( work->printQue );
			
			if( PRINT_UTIL_Trans(work->printUtil,work->printQue) ){
			}

			switch( ret ){
			case BMPMENULIST_NULL:
				break;
			case BMPMENULIST_CANCEL:	//�L�����Z��
				work->seq_no++;
				break;
			default:					//����
				//ret == zoneID
				work->seq_no++;
				break;
			}
		}
		break;
	case 2:
		BmpWinFrame_Clear( work->bmpwin, 0 );

		BmpMenuList_Exit( work->pMenuListWork, NULL, NULL );
		BmpMenuWork_ListDelete( work->pMenuList );
		
		PRINTSYS_QUE_Delete( work->printQue );
		GFL_FONT_Delete( work->fontHandle );
		GFL_STR_DeleteBuffer( work->strbuf );
		GFL_MSG_Delete( work->msgdata );
		GFL_BMPWIN_Delete( work->bmpwin );
		
		{
			u32 bgFrame = DEBUG_BGFRAME_MENU;
			GFL_BG_FreeCharacterArea( bgFrame, 0x00, 0x20 );
			GFL_BG_FreeBGControl( bgFrame );
		}

		return( TRUE );
	}

	return( FALSE );
}

//======================================================================
//	�f�o�b�O�pZoneID������ϊ�
//======================================================================
//--------------------------------------------------------------
//	�����R�[�h
//--------------------------------------------------------------
#define ASCII_EOM (0x00)
#define ASCII_0 (0x30)
#define ASCII_9 (0x39)
#define ASCII_A (0x41)
#define ASCII_Z (0x5a)

//���p
#define UTF16_U8_0 (0x0030)
#define UTF16_U8_9 (0x0039)
#define UTF16_U8_A (0x0041)
#define UTF16_U8_Z (0x005a)

//�S�p
#define UTF16_0 (0xff10)
#define UTF16_9 (0xff19)
#define UTF16_A (0xff21)
#define UTF16_Z (0xff3a)

//--------------------------------------------------------------
/**
 * ASCII->UTF-16
 * @param	str	ASCII ���p�p����
 * @retval	u16 UTF-16 str
 */
//--------------------------------------------------------------
static u16 DEBUG_ASCIICODE_UTF16( u8 code )
{
	if( code == ASCII_EOM ){
		return( GFL_STR_GetEOMCode() );
	}
	
	if( code >= ASCII_0 && code <= ASCII_9 ){
		code -= ASCII_0;
		return( UTF16_U8_0 + code );
	}
	
	if( code >= ASCII_A && code <= ASCII_Z ){
		code -= ASCII_A;
		return( UTF16_U8_A + code );
	}
	
	GF_ASSERT( 0 ); 					//���Ή�����
	return( GFL_STR_GetEOMCode() );
}

//--------------------------------------------------------------
/**
 * ZONE_ID->STRCODE
 * @param	heapID	������o�b�t�@�m�ۗp�q�[�vID
 * @param	zoneID	��������擾������zoneID
 * @retval	u16*	zoneID�����񂪊i�[���ꂽu16*(�J�����K�v
 */
//--------------------------------------------------------------
static u16 * DEBUG_GetZoneNameUTF16( u32 heapID, u32 zoneID )
{
	int i;
	u16 utf16,utf16_eom;
	u16 *pStrBuf;
	char name8[128];
	
	pStrBuf = GFL_HEAP_AllocClearMemory( heapID, sizeof(u16)*128 );
	ZONEDATA_GetZoneName( heapID, name8, zoneID );
	utf16_eom = GFL_STR_GetEOMCode();
	OS_Printf( "�ϊ� %s\n", name8 );
	
	for( i = 0; i < 128; i++ ){
		utf16 = DEBUG_ASCIICODE_UTF16( name8[i] );
		pStrBuf[i] = utf16;
		
		if( utf16 == utf16_eom ){
			return( pStrBuf );
		}
	}
	
	pStrBuf[i-1] = utf16_eom;	//�������I�[�o�[
	GF_ASSERT( 0 );
	return( pStrBuf );
}

//--------------------------------------------------------------
/**
 * Zone ID Name -> BMP_MENULIST_DATA 
 * @param	heapID	�e���|����������m�ۗpheapID
 * @param	zoneID	��������擾������zoneID
 * @param	strBuf	������i�[��STRBUF
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetSTRBUF_ZoneIDName(
		u32 heapID, u32 zoneID, STRBUF *strBuf )
{
	u16 *str = DEBUG_GetZoneNameUTF16( heapID, zoneID );
	GFL_STR_SetStringCode( strBuf, str );
	GFL_HEAP_FreeMemory( str );
}

//--------------------------------------------------------------
/**
 * �����ł��W�����v�pBMP_MENULIST_DATA�Z�b�g
 * @param	list	�Z�b�g��BMP_MENULIST_DATA
 * @param	heapID	������o�b�t�@�m�ۗpHEAPID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkZoneIDName(
		BMP_MENULIST_DATA *list, HEAPID heapID )
{
	int id,max = ZONEDATA_GetZoneIDMax();
	STRBUF *strBuf = GFL_STR_CreateBuffer( 128, heapID );
	
	for( id = 0; id < max; id++ ){
		GFL_STR_ClearBuffer( strBuf );
		DEBUG_SetSTRBUF_ZoneIDName( heapID, id, strBuf );
		BmpMenuWork_ListAddString( list, strBuf, id, heapID );
	}
	
	GFL_HEAP_FreeMemory( strBuf );
}

