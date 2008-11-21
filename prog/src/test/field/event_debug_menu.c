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
#include "message.naix"
#include "msg/msg_d_field.h"
#include "test_graphic/d_taya.naix"

#include "field/zonedata.h"
#include "field_comm/field_comm_main.h"

#include "event_debug_menu.h"
#include "event_mapchange.h"

#include "field_comm/field_comm_main.h"
#include "field_comm/field_comm_debug.h"

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
///	���j���[�Ăяo���֐�
///	BOOL TRUE=�C�x���g�p�� FALSE==�f�o�b�O���j���[�C�x���g�I��
//--------------------------------------------------------------
typedef BOOL (* D_MENU_CALLPROC)(DEBUG_MENU_EVENT_WORK*);

//--------------------------------------------------------------
///	DEBUG_MENU_LIST
//--------------------------------------------------------------
struct _TAG_DEBUG_MENU_LIST
{
	u32 str_id;		//�\�����镶����ID
	void *callp;	//�I�����ꂽ�ۂɕԂ��p�����^
};

//--------------------------------------------------------------
///	DEBUG_MENU_LISTDATA
//--------------------------------------------------------------
typedef struct
{
	u16 charsize_x;
	u16 charsize_y;
	u32 max;
	const DEBUG_MENU_LIST *list;
}DEBUG_MENU_LISTDATA;

//--------------------------------------------------------------
///	DMENU_COMMON_WORK
//--------------------------------------------------------------
struct _TAG_DMENU_COMMON_WORK
{
	u32 bgFrame;
	u32 heapID;
	
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;
	
	STRBUF *strbuf;
	GFL_MSGDATA *msgdata;
	GFL_FONT *fontHandle;
	PRINT_QUE *printQue;
	PRINT_UTIL printUtil[1];
	
	const DEBUG_MENU_LIST *pMenuList;
	BMP_MENULIST_DATA *pMenuListData;
	BMPMENULIST_WORK *pMenuListWork;
};

//--------------------------------------------------------------
///	DEBUG_MENU_EVENT_WORK
//--------------------------------------------------------------
struct _TAG_DEBUG_MENU_EVENT_WORK
{
	u32 page_id;
	HEAPID heapID;
	GMEVENT *gmEvent;
	GAMESYS_WORK *gmSys;
	FIELD_MAIN_WORK * fieldWork;
	
	D_MENU_CALLPROC call_proc;
	DMENU_COMMON_WORK menuCommonWork;
};

//--------------------------------------------------------------
///	DEBUG_FLDMENU
//--------------------------------------------------------------
#if 0
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
};
#endif

//======================================================================
//	proto
//======================================================================
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk );

#if 0
static DEBUG_FLDMENU * FldDebugMenu_Init(
 GMEVENT *gmEvent, FIELD_MAIN_WORK *fieldWork, u32 menu_num, u32 heapID );
static void FldDebugMenu_Delete( DEBUG_FLDMENU *d_menu );
static void FldDebugMenu_Create( DEBUG_FLDMENU *d_menu );
static DMENURET FldDebugMenu_Main( DEBUG_FLDMENU *d_menu );
#endif

static BOOL DMenuCallProc_GridCamera( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_GridScaleSwitch( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_GridScaleControl( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_OpenStartComm( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenStartInvasion( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_MapZoneSelect( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenCommDebugMenu( DEBUG_MENU_EVENT_WORK *wk );

static void DEBUG_SetMenuWorkZoneIDName(
		BMP_MENULIST_DATA *list, HEAPID heapID );

void DebugMenu_InitCommonMenu(
	DMENU_COMMON_WORK *work,
	const BMPMENULIST_HEADER *pMenuHead,
	BMP_MENULIST_DATA *pMenuListData,
	const DEBUG_MENU_LIST *pMenuList, int menuCount,
	int bmpsize_x, int bmpsize_y,
	int arcDatIDmsg, HEAPID heapID );
void DebugMenu_DeleteCommonMenu( DMENU_COMMON_WORK *work );
u32 DebugMenu_ProcCommonMenu( DMENU_COMMON_WORK *work );


//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g�@�ėp
//--------------------------------------------------------------
static const DEBUG_MENU_LIST DATA_DebugMenuList[] =
{
	{ DEBUG_FIELD_C_CHOICE00, DMenuCallProc_OpenCommDebugMenu },
	{ DEBUG_FIELD_STR01, NULL },
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
	{ DEBUG_FIELD_C_CHOICE00, DMenuCallProc_OpenCommDebugMenu },
//	{ DEBUG_FIELD_STR01, NULL },
//	{ DEBUG_FIELD_STR01, NULL },
};

//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g�e�[�u��
//--------------------------------------------------------------
static const DEBUG_MENU_LISTDATA DATA_DebugMenuListTbl[] =
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

//--------------------------------------------------------------
///	���j���[�w�b�_�[
//--------------------------------------------------------------
static const BMPMENULIST_HEADER DATA_DebugMenuListHeader =
{
	NULL,	//�\�������f�[�^�|�C���^
	NULL,	//�J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
	NULL,	//���\�����Ƃ̃R�[���o�b�N�֐�
	NULL,	//GFL_BMPWIN

	1,		//���X�g���ڐ�
	12,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	2,		//�����Ԋu�x
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

//======================================================================
//	�C�x���g�F�t�B�[���h�f�o�b�O���j���[
//======================================================================
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
	MI_CpuClear8( dmew, sizeof(DEBUG_MENU_EVENT_WORK) );
	
	dmew->gmSys = gsys;
	dmew->gmEvent = event;
	dmew->fieldWork = fieldWork;
	dmew->heapID = heapID;
	dmew->page_id = page_id;
	
	if( dmew->page_id >= D_MENULISTTBL_MAX ){
		OS_Printf( "debug menu number error\n" );
		dmew->page_id = 0;
	}
	
	return event;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�t�B�[���h�f�o�b�O���j���[
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk )
{
	DEBUG_MENU_EVENT_WORK *work = wk;
	
	switch (*seq) {
	case 0:
		{
			u32 i,max;
			BMPMENULIST_HEADER menuH;
			const DEBUG_MENU_LIST *d_menu_list;
			const DEBUG_MENU_LISTDATA *d_menu_listdata;
			
			menuH = DATA_DebugMenuListHeader;
			d_menu_listdata = &DATA_DebugMenuListTbl[work->page_id];
			d_menu_list = d_menu_listdata->list;
			max = d_menu_listdata->max;
			
			DebugMenu_InitCommonMenu(
				&work->menuCommonWork,
				&menuH, NULL, d_menu_list, max,
				d_menu_listdata->charsize_x, d_menu_listdata->charsize_y,
				NARC_message_d_field_dat, work->heapID );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = DebugMenu_ProcCommonMenu( &work->menuCommonWork );
			
			if( ret == BMPMENULIST_NULL ){	//���얳��
				break;
			}else if( ret == BMPMENULIST_CANCEL ){	//�L�����Z��
				(*seq)++;
			}else{							//����
				work->call_proc = (D_MENU_CALLPROC)ret;
				(*seq)++;
			}
		}
		break;
	case 2:
		{
			DebugMenu_DeleteCommonMenu( &work->menuCommonWork );
			
			if( work->call_proc != NULL ){
				if( work->call_proc(work) == TRUE ){
					return( GMEVENT_RES_CONTINUE );
				}
			}
			
			return( GMEVENT_RES_FINISH );
		}
		break;
	}

	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//	�t�B�[���h�f�o�b�O���j���[	�Â�
//======================================================================
#if 0
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
		const DEBUG_MENU_LISTDATA *d_menu_tbl;
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
#endif

//======================================================================
//	�f�o�b�O���j���[�Ăяo��
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�O���b�h�p�J����
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridCamera( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_MENU_EVENT_WORK *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_Camera( fieldWork );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�O���b�h�p�X�P�[���؂�ւ�
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridScaleSwitch( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_MENU_EVENT_WORK *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_ScaleChange( fieldWork );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�O���b�h�p�X�P�[������
 * @param	wk	DEBUG_MENU_EVENT_WORK *
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridScaleControl( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_MENU_EVENT_WORK *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_ScaleControl( fieldWork );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�ʐM�f�o�b�O�q���j���[
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_OpenCommDebugMenu( DEBUG_MENU_EVENT_WORK *wk )
{
	GMEVENT *event = wk->gmEvent;
	const HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	FIELD_COMM_DEBUG_WORK *work;
	
	GMEVENT_Change( event,
		FIELD_COMM_DEBUG_CommDebugMenu, FIELD_COMM_DEBUG_GetWorkSize() );
	
	work = GMEVENT_GetEventWork( event );
	FIELD_COMM_DEBUG_InitWork( heapID , fieldWork , event , work );

	return( TRUE );
}

//======================================================================
//	�f�o�b�O���j���[ �ǂ��ł��W�����v
//======================================================================
//--------------------------------------------------------------
///	DEBUG_ZONSEL_EVENT_WORK �ǂ��ł��W�����v�����p���[�N
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	
	BMP_MENULIST_DATA *pMenuListData;
	DMENU_COMMON_WORK menuCommonWork;
}DEBUG_ZONSEL_EVENT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuZoneSelectEvent(
		GMEVENT *event, int *seq, void *work );

///	�ǂ��ł��W�����v�@���j���[�w�b�_�[
static const BMPMENULIST_HEADER DATA_DebugMenuList_ZoneSel =
{
	NULL,	//�\�������f�[�^�|�C���^
	NULL,	//�J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
	NULL,	//���\�����Ƃ̃R�[���o�b�N�֐�
	NULL,	//GFL_BMPWIN

	1,		//���X�g���ڐ�
	12,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
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
/**
 * �f�o�b�O���j���[�Ăяo���@�ǂ��ł��W�����v
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_MapZoneSelect( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_ZONSEL_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuZoneSelectEvent, sizeof(DEBUG_ZONSEL_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_ZONSEL_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
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
	DEBUG_ZONSEL_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			u32 max = ZONEDATA_GetZoneIDMax();
			BMPMENULIST_HEADER menuH = DATA_DebugMenuList_ZoneSel;
			
			work->pMenuListData =
				BmpMenuWork_ListCreate( max, work->heapID );
			DEBUG_SetMenuWorkZoneIDName(
				work->pMenuListData, work->heapID );
			
			DebugMenu_InitCommonMenu(
				&work->menuCommonWork,
				&menuH, work->pMenuListData, NULL, max,
				11, 16, NARC_message_d_field_dat, work->heapID );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = DebugMenu_ProcCommonMenu( &work->menuCommonWork );
			
			if( ret == BMPMENULIST_NULL ){	//���얳��
				break;
			}
			
			DebugMenu_DeleteCommonMenu( &work->menuCommonWork );
			BmpMenuWork_ListDelete( work->pMenuListData );
			
			if( ret == BMPMENULIST_CANCEL ){	//�L�����Z��
				return( GMEVENT_RES_FINISH );
			}
			
			DEBUG_EVENT_ChangeEventMapChange(	//����
				work->gmSys, work->gmEvent, work->fieldWork, ret );
		}
		break;
	}

	return( GMEVENT_RES_CONTINUE );
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
#define UTF16H_0 (0x0030)
#define UTF16H_9 (0x0039)
#define UTF16H_A (0x0041)
#define UTF16H_Z (0x005a)

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
		return( UTF16H_0 + code );
	}
	
	if( code >= ASCII_A && code <= ASCII_Z ){
		code -= ASCII_A;
		return( UTF16H_A + code );
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

//======================================================================
//	�f�o�b�O���j���[��p���ʃ��j���[
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�p�@���ʃ��j���[������
 * @param	work	DMENU_COMMON_WORK
 * @param	pMenuHead BMPMENULIST_HEADER
 * @param	pMenuListData �\�����ځBpMenuList���玩����������ꍇ��NULL
 * @param	pMenuList pMenuList��������������ꍇ�Ɏw��B����ȊO��NULL
 * @param	menuCount	���j���[���ڐ�
 * @param	bmpsize_x	�r�b�g�}�b�v�T�C�YX�@�L�����P��
 * @param	bmpsize_y	�r�b�g�}�b�v�T�C�YY�@�L�����P��
 * @param	arcDatIDMsg	���b�Z�[�W�A�[�J�C�u�f�[�^�C���f�b�N�XID
 * @param	heapID		�g�p����HEAPID
 * @retval	nothing
 */
//--------------------------------------------------------------
void DebugMenu_InitCommonMenu(
	DMENU_COMMON_WORK *work,
	const BMPMENULIST_HEADER *pMenuHead,
	BMP_MENULIST_DATA *pMenuListData,
	const DEBUG_MENU_LIST *pMenuList, int menuCount,
	int bmpsize_x, int bmpsize_y,
	int arcDatIDmsg, HEAPID heapID )
{
	BMPMENULIST_HEADER menuH = *pMenuHead;
	
	work->heapID = heapID;
	work->bgFrame = DEBUG_BGFRAME_MENU;
	work->pMenuListData = pMenuListData;
	work->pMenuList = pMenuList;
	
	{	//BG������ �����ꃁ�C�����̂𗘗p����`��
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl( work->bgFrame, &bgcntText, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( work->bgFrame, VISIBLE_ON );
		
		GFL_BG_SetPriority( work->bgFrame, 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME0_M, 2 );
		
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_D_TAYA, NARC_d_taya_default_nclr,
			PALTYPE_MAIN_BG, DEBUG_FONT_PANO*32, 32, work->heapID );
		
		GFL_BG_FillCharacter( work->bgFrame, 0x00, 1, 0 );
		GFL_BG_FillScreen( work->bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	
		GFL_BG_LoadScreenReq( work->bgFrame );
	}
	
	{	//window frame
		BmpWinFrame_GraphicSet( work->bgFrame, 1,
			DEBUG_MENU_PANO, MENU_TYPE_SYSTEM, heapID );
	}
	
	{	//bmpwin
		work->bmpwin = GFL_BMPWIN_Create(
			work->bgFrame, 1, 1, bmpsize_x, bmpsize_y,
			DEBUG_FONT_PANO, GFL_BMP_CHRAREA_GET_B );
		work->bmp = GFL_BMPWIN_GetBmp( work->bmpwin );
		
		GFL_BMP_Clear( work->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( work->bmpwin );
		
		GFL_BMPWIN_TransVramCharacter( work->bmpwin );
		GFL_BG_LoadScreenReq( work->bgFrame );
	}
	
	{	//message
		work->msgdata = GFL_MSG_Create(
			GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, arcDatIDmsg, work->heapID );
		
		work->strbuf = GFL_STR_CreateBuffer( 512, work->heapID );
		
		work->fontHandle = GFL_FONT_Create(
			ARCID_D_TAYA,
			NARC_d_taya_lc12_2bit_nftr,
			GFL_FONT_LOADTYPE_FILE,
			FALSE,
			work->heapID );
		
		work->printQue = PRINTSYS_QUE_Create( work->heapID );
		PRINT_UTIL_Setup( work->printUtil, work->bmpwin );
	}
	
	{	//	menu window
		BmpWinFrame_Write( work->bmpwin,
			WINDOW_TRANS_ON, 1, DEBUG_MENU_PANO );
	}
	
	{	//menu list
		if( work->pMenuList != NULL ){	//���������ΏۗL��
			int i;
			GF_ASSERT( work->pMenuListData == NULL );
			work->pMenuListData = BmpMenuWork_ListCreate(
					menuCount, work->heapID );
			for( i = 0; i < menuCount; i++ ){
				BmpMenuWork_ListAddArchiveString(
					work->pMenuListData, work->msgdata,
					pMenuList[i].str_id, (u32)pMenuList[i].callp,
					work->heapID );
			}
		}
	}

	{	//menu
		menuH.count = menuCount;
		menuH.msgdata = work->msgdata;
		menuH.print_util = work->printUtil;
		menuH.print_que = work->printQue;
		menuH.font_handle = work->fontHandle;
		menuH.win = work->bmpwin;
		menuH.list = work->pMenuListData;
		
		work->pMenuListWork =
			BmpMenuList_Set( &menuH, 0, 0, work->heapID );
		BmpMenuList_SetCursorString( work->pMenuListWork, 0 );
	}
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�p�@���ʃ��j���[�폜
 * @param	work	DMENU_COMMON_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void DebugMenu_DeleteCommonMenu( DMENU_COMMON_WORK *work )
{
	BmpWinFrame_Clear( work->bmpwin, 0 );
	BmpMenuList_Exit( work->pMenuListWork, NULL, NULL );
	
	if( work->pMenuList != NULL ){	//��������
		BmpMenuWork_ListDelete( work->pMenuListData );
	}
	
	PRINTSYS_QUE_Delete( work->printQue );
	GFL_FONT_Delete( work->fontHandle );
	GFL_STR_DeleteBuffer( work->strbuf );
	GFL_MSG_Delete( work->msgdata );
	GFL_BMPWIN_Delete( work->bmpwin );
	
	{	//�Ƃ肠����������Ł@������̓��C����
		GFL_BG_FreeCharacterArea( work->bgFrame, 0x00, 0x20 );
		GFL_BG_FreeBGControl( work->bgFrame );
	}
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�p�@���ʃ��j���[����
 * @param	work	DEBUG_COMMON_WORK
 * @retval	u32		���j���[���X�g�I���ʒu�̃p�����[�^
 * BMPMENULIST_NULL = �I�� BMPMENULIST_CANCEL	= �L�����Z��
 */
//--------------------------------------------------------------
u32 DebugMenu_ProcCommonMenu( DMENU_COMMON_WORK *work )
{
	u32 ret = BmpMenuList_Main( work->pMenuListWork );
	
	PRINTSYS_QUE_Main( work->printQue );
	
	if( PRINT_UTIL_Trans(work->printUtil,work->printQue) ){
	}

	return( ret );
}

//--------------------------------------------------------------
//	���[�N�T�C�Y�̎擾
//--------------------------------------------------------------
const int DebugMenu_GetWorkSize(void)
{ 
	return sizeof(DMENU_COMMON_WORK);
}
