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
#include "system/main.h"
#include "arc_def.h"

#include "field/field_msgbg.h"

#include "message.naix"
#include "msg/msg_d_field.h"

#include "fieldmap.h"
#include "field/zonedata.h"
#include "field_comm/field_comm_main.h"
#include "arc/fieldmap/zone_id.h"
#include "field/eventdata_sxy.h"

#include "event_debug_menu.h"
#include "event_mapchange.h"

#include "field_comm/field_comm_main.h"
#include "field_comm/field_comm_debug.h"
#include "ircbattle/ircbattlemenu.h"
#include "event_ircbattle.h"
#include "event_wificlub.h"
#include "field_subscreen.h"
#include "sound/pm_sndsys.h"

#include "font/font.naix"

#include  "field/weather_no.h"
#include  "weather.h"
#include  "msg/msg_d_tomoya.h"

#include "field_debug.h"

#include "field_event_check.h"
#include "event_debug_item.h" //EVENT_DebugItemMake
#include "savedata/box_savedata.h"  //�f�o�b�O�A�C�e�������p
#include "app/townmap.h"


//======================================================================
//	define
//======================================================================
#define DEBUG_BGFRAME_MENU (GFL_BG_FRAME1_M)	//�g�p�t���[��
#define DEBUG_BGFRAME_MSG  (GFL_BG_FRAME2_M)	//�g�p�t���[��
#define DEBUG_MENU_PANO	(14)
#define DEBUG_FONT_PANO	(15)

#define D_MENU_CHARSIZE_X (12)		//���j���[����
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
typedef struct _TAG_DEBUG_MENU_EVENT_WORK DEBUG_MENU_EVENT_WORK;

//--------------------------------------------------------------
///	���j���[�Ăяo���֐�
///	BOOL TRUE=�C�x���g�p�� FALSE==�f�o�b�O���j���[�C�x���g�I��
//--------------------------------------------------------------
typedef BOOL (* D_MENU_CALLPROC)(DEBUG_MENU_EVENT_WORK*);

//--------------------------------------------------------------
///	DEBUG_MENU_LISTDATA
//--------------------------------------------------------------
typedef struct
{
	u16 charsize_x;
	u16 charsize_y;
	u32 max;
	const FLDMENUFUNC_LIST *list;
}DEBUG_MENU_LISTDATA;

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
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
};

//======================================================================
//	proto
//======================================================================
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk );

static BOOL DMenuCallProc_GridCamera( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_GridScaleSwitch( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_GridScaleControl( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_OpenStartComm( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenStartInvasion( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_MapZoneSelect( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenCommDebugMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenIRCBTLMenu( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_OpenClubMenu( DEBUG_MENU_EVENT_WORK *wk );

static DMenuCallProc_MapSeasonSelect( DEBUG_MENU_EVENT_WORK *wk );
static DMenuCallProc_SubscreenSelect( DEBUG_MENU_EVENT_WORK *wk );
static DMenuCallProc_MusicalSelect( DEBUG_MENU_EVENT_WORK *wk );

static void DEBUG_SetMenuWorkZoneIDNameAll(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID );
static void DEBUG_SetMenuWorkZoneIDName(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID, u32 zoneID );

static BOOL DMenuCallProc_ControlCamera( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_ControlTarget( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_CameraList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_MMdlList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_ControlLight( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_ControlFog( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_WeatherList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_FieldPosData( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_ControlRtcList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_Naminori( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_DebugItem( DEBUG_MENU_EVENT_WORK *wk );
static BOOL DMenuCallProc_BoxMax( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_DebugSkyJump( DEBUG_MENU_EVENT_WORK *p_wk );

//======================================================================
//  �f�o�b�O���j���[���X�g
//======================================================================
//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g�@�ėp
///	�f�[�^��ǉ����鎖�Ń��j���[�̍��ڂ������܂��B
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuList[] =
{
	{	DEBUG_FIELD_STR38, DMenuCallProc_DebugSkyJump },
	{ DEBUG_FIELD_STR17, DMenuCallProc_FieldPosData },
	{ DEBUG_FIELD_STR02, DMenuCallProc_ControlCamera },
	{ DEBUG_FIELD_STR20, DMenuCallProc_ControlTarget },
	{ DEBUG_FIELD_STR03, DMenuCallProc_GridScaleSwitch },
	{ DEBUG_FIELD_STR04, DMenuCallProc_GridScaleControl },
	{ DEBUG_FIELD_STR05, DMenuCallProc_MapZoneSelect },
	{ DEBUG_FIELD_STR06, DMenuCallProc_MapSeasonSelect},
	{ DEBUG_FIELD_STR07, DMenuCallProc_CameraList },
	{ DEBUG_FIELD_STR13, DMenuCallProc_MMdlList },
	{ DEBUG_FIELD_C_CHOICE00, DMenuCallProc_OpenCommDebugMenu },
	{ DEBUG_FIELD_STR12, DMenuCallProc_OpenIRCBTLMenu },
	{ DEBUG_FIELD_STR19, DMenuCallProc_OpenClubMenu },
	{ DEBUG_FIELD_STR22, DMenuCallProc_ControlRtcList },
	{ DEBUG_FIELD_STR15, DMenuCallProc_ControlLight },
	{ DEBUG_FIELD_STR16, DMenuCallProc_WeatherList },
  { DEBUG_FIELD_STR_SUBSCRN, DMenuCallProc_SubscreenSelect },
  { DEBUG_FIELD_STR21 , DMenuCallProc_MusicalSelect },
  { DEBUG_FIELD_STR31, DMenuCallProc_Naminori },
  { DEBUG_FIELD_STR32, DMenuCallProc_DebugItem },
  { DEBUG_FIELD_STR37, DMenuCallProc_BoxMax },
  { DEBUG_FIELD_STR36, DMenuCallProc_ControlFog },
	{ DEBUG_FIELD_STR01, NULL },
};

//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g	�O���b�h�����}�b�v�p�B
///	�f�[�^��ǉ����鎖�Ń��j���[�̍��ڂ������܂��B
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuListGrid[] =
{
	{	DEBUG_FIELD_STR38, DMenuCallProc_DebugSkyJump },
	{ DEBUG_FIELD_STR17, DMenuCallProc_FieldPosData },
	{ DEBUG_FIELD_STR02, DMenuCallProc_ControlCamera },
	{ DEBUG_FIELD_STR20, DMenuCallProc_ControlTarget },
	{ DEBUG_FIELD_STR03, DMenuCallProc_GridScaleSwitch },
	{ DEBUG_FIELD_STR04, DMenuCallProc_GridScaleControl },
	{ DEBUG_FIELD_STR05, DMenuCallProc_MapZoneSelect },
	{ DEBUG_FIELD_STR06, DMenuCallProc_MapSeasonSelect},
	{ DEBUG_FIELD_STR07, DMenuCallProc_CameraList },
	{ DEBUG_FIELD_STR13, DMenuCallProc_MMdlList },
	{ DEBUG_FIELD_C_CHOICE00, DMenuCallProc_OpenCommDebugMenu },
	{ DEBUG_FIELD_STR12, DMenuCallProc_OpenIRCBTLMenu },
	{ DEBUG_FIELD_STR19, DMenuCallProc_OpenClubMenu },
	{ DEBUG_FIELD_STR22, DMenuCallProc_ControlRtcList },
	{ DEBUG_FIELD_STR15, DMenuCallProc_ControlLight },
	{ DEBUG_FIELD_STR16, DMenuCallProc_WeatherList },
  { DEBUG_FIELD_STR_SUBSCRN, DMenuCallProc_SubscreenSelect },
  { DEBUG_FIELD_STR21 , DMenuCallProc_MusicalSelect },
  { DEBUG_FIELD_STR31, DMenuCallProc_Naminori },
  { DEBUG_FIELD_STR32, DMenuCallProc_DebugItem },
	{ DEBUG_FIELD_STR01, NULL },
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
		D_MENU_CHARSIZE_X,
		D_MENU_CHARSIZE_Y,
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
		D_MENU_CHARSIZE_X,
		D_MENU_CHARSIZE_Y,
		NELEMS(DATA_DebugMenuListGrid),
		DATA_DebugMenuListGrid
	},
};

//���j���[�ő吔
#define D_MENULISTTBL_MAX (NELEMS(DATA_DebugMenuListTbl))

//--------------------------------------------------------------
///	���j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_DebugMenuListHeader =
{
	1,		//���X�g���ڐ�
	9,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	2,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	0,		//�\�����WX �L�����P��
	0,		//�\�����WY �L�����P��
	0,		//�\���T�C�YX �L�����P��
	0,		//�\���T�C�YY �L�����P��
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
			FLDMSGBG *msgBG;
			FLDMENUFUNC_HEADER head;
			FLDMENUFUNC_LISTDATA *listdata;
			const FLDMENUFUNC_LIST *menulist;
			const DEBUG_MENU_LISTDATA *d_menu_listdata;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			
			work->msgData = FLDMSGBG_CreateMSGDATA(
						msgBG, NARC_message_d_field_dat );
			
      {
        FLDMSGWIN *msgWin = FLDMSGWIN_AddTalkWin( msgBG, work->msgData );
        FLDMSGWIN_Print( msgWin, 0, 0, 0 );
        FLDMSGWIN_Delete( msgWin );
      }

			d_menu_listdata = &DATA_DebugMenuListTbl[work->page_id];
			menulist = d_menu_listdata->list;
			
			listdata = FLDMENUFUNC_CreateMakeListData(
				menulist, d_menu_listdata->max, work->msgData, work->heapID );
			
			head = DATA_DebugMenuListHeader;
			FLDMENUFUNC_InputHeaderListSize(
				&head, d_menu_listdata->max, 1, 1,
				d_menu_listdata->charsize_x, d_menu_listdata->charsize_y );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &head, listdata );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//���얳��
				break;
			}else if( ret == FLDMENUFUNC_CANCEL ){	//�L�����Z��
				(*seq)++;
			}else{							//����
				work->call_proc = (D_MENU_CALLPROC)ret;
				(*seq)++;
			}
		}
		break;
	case 2:
		{
			FLDMENUFUNC_DeleteMenu( work->menuFunc );
			
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
//	�f�o�b�O���j���[�Ăяo��
//======================================================================
#if 0
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
#endif

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�O���b�h�p�X�P�[���؂�ւ�
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_GridScaleSwitch( DEBUG_MENU_EVENT_WORK *wk )
{
#if 0
	DEBUG_MENU_EVENT_WORK *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_ScaleChange( fieldWork );
#endif
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
#if 0
	DEBUG_MENU_EVENT_WORK *d_menu = wk;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	HEAPID DebugHeapID = d_menu->heapID;
	
	DEBUG_FldGridProc_ScaleControl( fieldWork );
#endif
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
	GAMESYS_WORK	*gameSys	= wk->gmSys;
	FIELD_COMM_DEBUG_WORK *work;
	
	GMEVENT_Change( event,
		FIELD_COMM_DEBUG_CommDebugMenu, FIELD_COMM_DEBUG_GetWorkSize() );
	
	work = GMEVENT_GetEventWork( event );
	FIELD_COMM_DEBUG_InitWork( heapID , gameSys , fieldWork , event , work );

	return( TRUE );
}

//--------------------------------------------------------------
/**
 * IRCBATTLE���j���[�Ăт���
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_OpenIRCBTLMenu( DEBUG_MENU_EVENT_WORK *wk )
{
	GMEVENT *event = wk->gmEvent;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	GAMESYS_WORK	*gameSys	= wk->gmSys;

	FIELD_SUBSCREEN_SetAction(FIELDMAP_GetFieldSubscreenWork(fieldWork), FIELD_SUBSCREEN_ACTION_DEBUGIRC);
	
//    EVENT_IrcBattle(gameSys, fieldWork, event,FALSE);
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * WiFiClub���j���[�Ăт���
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_OpenClubMenu( DEBUG_MENU_EVENT_WORK *wk )
{
	GMEVENT *event = wk->gmEvent;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	GAMESYS_WORK	*gameSys	= wk->gmSys;

    EVENT_WiFiClub(gameSys, fieldWork, event);
	return( TRUE );
}





//======================================================================
//	�f�o�b�O���j���[ �ǂ��ł��W�����v
//======================================================================
//--------------------------------------------------------------
///	DEBUG_ZONESEL_EVENT_WORK �ǂ��ł��W�����v�����p���[�N
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
}DEBUG_ZONESEL_EVENT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuZoneSelectEvent(
		GMEVENT *event, int *seq, void *work );

///	�ǂ��ł��W�����v�@���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_ZoneSel =
{
	1,		//���X�g���ڐ�
	10,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	0,		//�\�����WX �L�����P��
	0,		//�\�����WY �L�����P��
	0,		//�\���T�C�YX �L�����P��
	0,		//�\���T�C�YY �L�����P��
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
	DEBUG_ZONESEL_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuZoneSelectEvent, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
	
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
	DEBUG_ZONESEL_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			u32 max = ZONEDATA_GetZoneIDMax();
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_ZoneSel;
			FLDMENUFUNC_LISTDATA *pMenuListData;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
					msgBG, NARC_message_d_field_dat );
			pMenuListData = FLDMENUFUNC_CreateListData( max, work->heapID );
			DEBUG_SetMenuWorkZoneIDNameAll( pMenuListData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 11, 16 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu(
					msgBG, &menuH, pMenuListData );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//���얳��
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_CANCEL ){	//�L�����Z��
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
//	�f�o�b�O���j���[�@�l�G�W�����v
//======================================================================
//--------------------------------------------------------------
//	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuSeasonSelectEvent(
		GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�l�G�}�b�v�Ԉړ�
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static DMenuCallProc_MapSeasonSelect( DEBUG_MENU_EVENT_WORK *wk )
{
	HEAPID heapID = wk->heapID;
	GMEVENT *event = wk->gmEvent;
	GAMESYS_WORK *gsys = wk->gmSys;
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
	PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
	ZONEID zone_id = PLAYERWORK_getZoneID( player );
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_ZONESEL_EVENT_WORK *work;
	
		GMEVENT_Change( event,
			DMenuSeasonSelectEvent, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
		work = GMEVENT_GetEventWork( event );
		MI_CpuClear8( work, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
	
		work->gmSys = gsys;
		work->gmEvent = event;
		work->heapID = heapID;
		work->fieldWork = fieldWork;
		return( TRUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
#include "gamesystem/pm_season.h"
static const FLDMENUFUNC_LIST DATA_SeasonMenuList[PMSEASON_TOTAL] =
{
	{ DEBUG_FIELD_STR_SPRING, (void*)PMSEASON_SPRING },
	{ DEBUG_FIELD_STR_SUMMER, (void*)PMSEASON_SUMMER },
	{ DEBUG_FIELD_STR_AUTUMN, (void*)PMSEASON_AUTUMN },
	{ DEBUG_FIELD_STR_WINTER, (void*)PMSEASON_WINTER },
};

//--------------------------------------------------------------
/**
 * �C�x���g�F�l�G�W�����v
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuSeasonSelectEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_ZONESEL_EVENT_WORK *work = wk;

	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = NELEMS(DATA_SeasonMenuList);
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_ZoneSel;  //���p
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
				msgBG, NARC_message_d_field_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_SeasonMenuList, max, work->msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 16, 7 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( work->msgData );
		}
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_NULL ){	//���얳��
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_CANCEL ){	//�L�����Z��
				return( GMEVENT_RES_FINISH );
			}else{
				GMEVENT *event;
				GAMEDATA *gdata = GAMESYSTEM_GetGameData( work->gmSys );
				PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
				const VecFx32 *pos = PLAYERWORK_getPosition( player );
				u16 dir = PLAYERWORK_getDirection( player );
				ZONEID zone_id = PLAYERWORK_getZoneID(player);
				
				if( (dir>0x2000) && (dir<0x6000) ){
					dir = EXIT_DIR_LEFT;
				}else if( (dir >= 0x6000) && (dir <= 0xa000) ){
					dir = EXIT_DIR_DOWN;
				}else if( (dir > 0xa000) && (dir < 0xe000) ){
					dir = EXIT_DIR_RIGHT;
				}else{
					dir = EXIT_DIR_UP;
				}

				GAMEDATA_SetSeasonID(gdata, ret);
				event = DEBUG_EVENT_ChangeMapPos(
					work->gmSys, work->fieldWork, zone_id, pos, dir );
				GMEVENT_ChangeEvent( work->gmEvent, event );
				OS_Printf( "x = %xH, z = %xH\n", pos->x, pos->z );
			}
		}
		break;
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
typedef struct {  
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	FLDMENUFUNC *menuFunc;
  FIELD_SUBSCREEN_WORK * subscreen;
}DEBUG_MENU_EVENT_SUBSCRN_SELECT_WORK, DMESSWORK;

//--------------------------------------------------------------
//	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuSubscreenSelectEvent(
		GMEVENT *event, int *seq, void *wk );
static void setupTouchCameraSubscreen(DMESSWORK * dmess);
static void setupSoundViewerSubscreen(DMESSWORK * dmess);
static void setupNormalSubscreen(DMESSWORK * dmess);
static void setupTopMenuSubscreen(DMESSWORK * dmess);
static void setupUnionSubscreen(DMESSWORK * dmess);
static void setupDebugLightSubscreen(DMESSWORK * dmess);


//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�l�G�}�b�v�Ԉړ�
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static DMenuCallProc_SubscreenSelect( DEBUG_MENU_EVENT_WORK *wk )
{
	HEAPID heapID = wk->heapID;
	GMEVENT *event = wk->gmEvent;
	GAMESYS_WORK *gsys = wk->gmSys;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DMESSWORK *work;
	
		GMEVENT_Change( event,
			DMenuSubscreenSelectEvent, sizeof(DMESSWORK) );
		work = GMEVENT_GetEventWork( event );
		MI_CpuClear8( work, sizeof(DMESSWORK) );
	
		work->gmSys = gsys;
		work->gmEvent = event;
		work->heapID = heapID;
		work->fieldWork = fieldWork;
    work->subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
		return( TRUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_SubcreenMenuList[FIELD_SUBSCREEN_MODE_MAX] =
{
	{ DEBUG_FIELD_STR_SUBSCRN01, (void*)setupTouchCameraSubscreen },
	{ DEBUG_FIELD_STR_SUBSCRN02, (void*)setupSoundViewerSubscreen },
	{ DEBUG_FIELD_STR_SUBSCRN03, (void*)setupNormalSubscreen },
	{ DEBUG_FIELD_STR_SUBSCRN04, (void*)setupTopMenuSubscreen },
	{ DEBUG_FIELD_STR_SUBSCRN01, (void*)setupDebugLightSubscreen },
	{ DEBUG_FIELD_STR_SUBSCRN05, (void*)setupUnionSubscreen },
};

//--------------------------------------------------------------
/**
 * �C�x���g�F������ʃW�����v
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuSubscreenSelectEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DMESSWORK *work = wk;

	switch( *seq )
  {
	case 0:
		{
			FLDMSGBG *msgBG;
	    GFL_MSGDATA *msgData;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = NELEMS(DATA_SubcreenMenuList);
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_ZoneSel;  //���p
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_d_field_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_SubcreenMenuList, max, msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 16, max*2 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( msgData );
		}
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_NULL ){	//���얳��
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_CANCEL ){	//�L�����Z��
				return( GMEVENT_RES_FINISH );
			}else{
        typedef void (* CHANGE_FUNC)(DMESSWORK*);
        CHANGE_FUNC func = (CHANGE_FUNC)ret;
        func(work);
        //FIELD_SUBSCREEN_ChangeForce(FIELDMAP_GetFieldSubscreenWork(work->fieldWork), ret);
				return( GMEVENT_RES_FINISH );
			}
		}
		break;
	}
	
	return GMEVENT_RES_CONTINUE ;
}

//--------------------------------------------------------------
/**
 * @brief �T�u�X�N���[���̃J��������ɓ���������
 *
 * @todo  �J�������o�C���h���Ă���A�ǂ̂悤�ɐ؂藣�����H���m�肳����
 */
//--------------------------------------------------------------
static void setupTouchCameraSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
  { 
    void * inner_work;
    FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(dmess->fieldWork);
    inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(dmess->subscreen);
    FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_CAMERA_POS);
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupSoundViewerSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupNormalSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_NORMAL);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupTopMenuSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_TOPMENU);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupUnionSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_UNION);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupDebugLightSubscreen(DMESSWORK * dmess)
{ 
  FIELD_SUBSCREEN_ChangeForce(dmess->subscreen, FIELD_SUBSCREEN_DEBUG_LIGHT);
}


//--------------------------------------------------------------
//  �~���[�W�J���n
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(musical);
#include "musical/musical_local.h"
#include "musical/musical_define.h"
#include "musical/musical_system.h"
#include "musical/musical_dressup_sys.h"
#include "musical/musical_stage_sys.h"
#include "poke_tool/poke_tool.h"  //�h���X�A�b�v���f�[�^�p
#include "poke_tool/monsno_def.h" //�h���X�A�b�v���f�[�^�p
#include "event_fieldmap_control.h"
typedef struct {  
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	GMEVENT *newEvent;
	FIELD_MAIN_WORK *fieldWork;
	FLDMENUFUNC *menuFunc;
	
	POKEMON_PARAM *pokePara;
	MUSICAL_POKE_PARAM *musPoke;
	MUSICAL_INIT_WORK *musInitWork;
	DRESSUP_INIT_WORK *dupInitWork;
	u8  menuRet;
}DEBUG_MENU_EVENT_MUSICAL_SELECT_WORK, DEB_MENU_MUS_WORK;
static GMEVENT_RESULT DMenuMusicalSelectEvent( GMEVENT *event, int *seq, void *wk );

static void setupMusicalDressup(DEB_MENU_MUS_WORK * work);
static void setupMusicarShowPart(DEB_MENU_MUS_WORK * work);
static void setupMusicarAll(DEB_MENU_MUS_WORK * work);

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�~���[�W�J���Ăяo��
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static DMenuCallProc_MusicalSelect( DEBUG_MENU_EVENT_WORK *wk )
{
	HEAPID heapID = wk->heapID;
	GMEVENT *event = wk->gmEvent;
	GAMESYS_WORK *gsys = wk->gmSys;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEB_MENU_MUS_WORK *work;
	
		GMEVENT_Change( event,
			DMenuMusicalSelectEvent, sizeof(DEB_MENU_MUS_WORK) );
		work = GMEVENT_GetEventWork( event );
		MI_CpuClear8( work, sizeof(DEB_MENU_MUS_WORK) );
	
		work->gmSys = gsys;
		work->gmEvent = event;
		work->heapID = heapID;
		work->fieldWork = fieldWork;
		work->musInitWork = NULL;
		work->dupInitWork = NULL;
		work->musPoke = NULL;
		work->pokePara = NULL;
		return( TRUE );
}

//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_MusicalMenuList[3] =
{
	{ DEBUG_FIELD_STR_MUSICAL3, (void*)setupMusicarAll },
	{ DEBUG_FIELD_STR_MUSICAL1, (void*)setupMusicalDressup },
	{ DEBUG_FIELD_STR_MUSICAL2, (void*)setupMusicarShowPart },
};

//--------------------------------------------------------------
/**
 * �C�x���g�F�~���[�W�J���f�o�b�O���j���[
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuMusicalSelectEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEB_MENU_MUS_WORK *work = wk;

	switch( *seq )
  {
	case 0:
		{
			FLDMSGBG *msgBG;
	    GFL_MSGDATA *msgData;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = NELEMS(DATA_MusicalMenuList);
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_ZoneSel;  //���p
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_d_field_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_MusicalMenuList, max, msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 16, max*2 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( msgData );
		}
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_NULL ){	//���얳��
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );

			if( ret == FLDMENUFUNC_CANCEL ){	//�L�����Z��
				return( GMEVENT_RES_FINISH );
			}else{
        typedef void (* CHANGE_FUNC)(DEB_MENU_MUS_WORK*);
        CHANGE_FUNC func = (CHANGE_FUNC)ret;
        func(work);
        PMSND_PauseBGM(TRUE);
        PMSND_PushBGM();
        GMEVENT_CallEvent(work->gmEvent, work->newEvent);
    		(*seq)++;
    		return( GMEVENT_RES_CONTINUE );
			}
		}
		break;
  case 2:
    if( work->musInitWork != NULL )
    {
      GFL_HEAP_FreeMemory( work->musInitWork->pokePara );
      GFL_HEAP_FreeMemory( work->musInitWork );
      work->musInitWork = NULL;
    }
    if( work->dupInitWork != NULL )
    {
      MUSICAL_DRESSUP_DeleteInitWork( work->dupInitWork );
      work->dupInitWork = NULL;
    }
    if( work->musPoke != NULL )
    {
      GFL_HEAP_FreeMemory( work->musPoke );
      work->musPoke = NULL;
    }
    if( work->pokePara != NULL )
    {
      GFL_HEAP_FreeMemory( work->pokePara );
      work->pokePara = NULL;
    }
    PMSND_PopBGM();
    PMSND_PauseBGM(FALSE);
    PMSND_FadeInBGM(60);
		return( GMEVENT_RES_FINISH );
    
    break;
	}
	
	return GMEVENT_RES_CONTINUE ;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupMusicalDressup(DEB_MENU_MUS_WORK * work)
{
  work->pokePara = PP_Create( MONSNO_PIKUSII , 20 , PTL_SETUP_POW_AUTO , GFL_HEAPID_APP|HEAPDIR_MASK );
  work->musPoke = MUSICAL_SYSTEM_InitMusPoke( work->pokePara , GFL_HEAPID_APP|HEAPDIR_MASK );
  work->dupInitWork = MUSICAL_DRESSUP_CreateInitWork( GFL_HEAPID_APP|HEAPDIR_MASK , work->musPoke , SaveControl_GetPointer() );

  work->newEvent = EVENT_FieldSubProc(work->gmSys, work->fieldWork,
        FS_OVERLAY_ID(musical), &DressUp_ProcData, work->dupInitWork );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupMusicarShowPart(DEB_MENU_MUS_WORK * work)
{ 
  work->newEvent = EVENT_FieldSubProc(work->gmSys, work->fieldWork,
        FS_OVERLAY_ID(musical), &MusicalStage_ProcData, NULL );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setupMusicarAll(DEB_MENU_MUS_WORK * work)
{
	work->musInitWork = GFL_HEAP_AllocMemory( HEAPID_PROC , sizeof(MUSICAL_INIT_WORK));
	work->musInitWork->saveCtrl = GAMEDATA_GetSaveControlWork( GAMESYSTEM_GetGameData(work->gmSys) ); 
	work->musInitWork->pokePara = PP_Create( MONSNO_PIKUSII , 20 , PTL_SETUP_POW_AUTO , HEAPID_PROC );
	work->musInitWork->isComm = FALSE;
	work->musInitWork->isDebug = FALSE;
	work->musInitWork->gameComm = GAMESYSTEM_GetGameCommSysPtr(work->gmSys);
  work->newEvent = EVENT_FieldSubProc(work->gmSys, work->fieldWork,
        NO_OVERLAY_ID, &Musical_ProcData, work->musInitWork );
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
  
  //���Ή�����
#if 0	
	GF_ASSERT( 0 );
#else
  OS_Printf( "���Ή����� CODE=%d\n", code );
	return( GFL_STR_GetEOMCode() );
#endif
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
	ZONEDATA_DEBUG_GetZoneName( name8, zoneID );
	utf16_eom = GFL_STR_GetEOMCode();
//	OS_Printf( "�ϊ� %s\n", name8 );
	
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
static void DEBUG_SetMenuWorkZoneIDNameAll(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID )
{
	int id,max = ZONEDATA_GetZoneIDMax();
	STRBUF *strBuf = GFL_STR_CreateBuffer( 128, heapID );
	
	for( id = 0; id < max; id++ ){
		GFL_STR_ClearBuffer( strBuf );
		DEBUG_SetSTRBUF_ZoneIDName( heapID, id, strBuf );
		FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
	}
	
	GFL_HEAP_FreeMemory( strBuf );
}

//--------------------------------------------------------------
/**
 * ZONE_ID�������BMP_MENULIST_DATA�ɃZ�b�g
 * @param	list	�Z�b�g��BMP_MENULIST_DATA
 * @param	heapID	������o�b�t�@�m�ۗpHEAPID
 * @param	zoneID	�Z�b�g����ZONE_ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkZoneIDName(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID, u32 zoneID )
{
	int max = ZONEDATA_GetZoneIDMax();
	STRBUF *strBuf = GFL_STR_CreateBuffer( 128, heapID );
	
	GFL_STR_ClearBuffer( strBuf );
	DEBUG_SetSTRBUF_ZoneIDName( heapID, zoneID, strBuf );
	FLDMENUFUNC_AddStringListData( list, strBuf, zoneID, heapID );
	GFL_HEAP_FreeMemory( strBuf );
}

//======================================================================
//	�f�o�b�O���j���[�@�J��������
//======================================================================
#define CM_RT_SPEED (FX32_ONE/8)
#define CM_HEIGHT_MV (FX32_ONE*2)
#define CM_NEARFAR_MV (FX32_ONE)

//--------------------------------------------------------------
///	DEBUG_CTLCAMERA_WORK �J�������샏�[�N1
//--------------------------------------------------------------
typedef struct
{
	int vanish;
	GAMESYS_WORK *gsys;
	GMEVENT *event;
	HEAPID heapID;
	FIELD_MAIN_WORK *fieldWork;
	FLDMSGBG *pMsgBG;
	FLDMSGWIN *pMsgWin;
	STRBUF *pStrBuf;
}DEBUG_CTLCAMERA_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlCamera(
		GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�J��������
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_ControlCamera( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_CTLCAMERA_WORK *work;
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	
	GMEVENT_Change( event, DMenuControlCamera, sizeof(DEBUG_CTLCAMERA_WORK) );
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_CTLCAMERA_WORK) );
	
	work->gsys = gsys;
	work->event = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;

  {

    FIELD_SUBSCREEN_WORK * subscreen;
    

    // �J��������͉���ʂōs��
    subscreen = FIELDMAP_GetFieldSubscreenWork(work->fieldWork);
    FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
    { 
      void * inner_work;
      FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(work->fieldWork);
      inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(subscreen);
      FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_CAMERA_POS);
    }
  }
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�J��������
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_ControlTarget( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_CTLCAMERA_WORK *work;
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	
	GMEVENT_Change( event, DMenuControlCamera, sizeof(DEBUG_CTLCAMERA_WORK) );
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_CTLCAMERA_WORK) );
	
	work->gsys = gsys;
	work->event = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;

  {
    FIELD_SUBSCREEN_WORK * subscreen;

    // �J��������͉���ʂōs��
    subscreen = FIELDMAP_GetFieldSubscreenWork(work->fieldWork);
    FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA);
    { 
      void * inner_work;
      FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(work->fieldWork);
      inner_work = FIELD_SUBSCREEN_DEBUG_GetControl(subscreen);
      FIELD_CAMERA_DEBUG_BindSubScreen(cam, inner_work, FIELD_CAMERA_DEBUG_BIND_TARGET_POS);
    }
  }
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�J��������
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlCamera(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_CTLCAMERA_WORK *work = wk;
  FIELD_CAMERA * cam = FIELDMAP_GetFieldCamera(work->fieldWork);


  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
    FIELD_CAMERA_DEBUG_ReleaseSubScreen( cam );
	  return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//	�f�o�b�O���j���[�@�e�X�g�J�������X�g
//======================================================================
//--------------------------------------------------------------
///	DEBUG_TESTCAMERALIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
}DEBUG_TESTCAMERALIST_EVENT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuTestCameraListEvent(
		GMEVENT *event, int *seq, void *work );

///�J�������X�g�ő�
#define TESTCAMERALISTMAX (4)

///�e�X�g�J�������X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_TestCameraList =
{
	1,		//���X�g���ڐ�
	4,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	0,		//�\�����WX �L�����P��
	0,		//�\�����WY �L�����P��
	0,		//�\���T�C�YX �L�����P��
	0,		//�\���T�C�YY �L�����P��
};

///�e�X�g�J�������j���[���X�g
static const FLDMENUFUNC_LIST DATA_TestCameraMenuList[TESTCAMERALISTMAX] =
{
	{ DEBUG_FIELD_STR09, (void*)0 },
	{ DEBUG_FIELD_STR10, (void*)1 },
	{ DEBUG_FIELD_STR11, (void*)2 },
	{ DEBUG_FIELD_STR08, (void*)3 },
};

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�e�X�g�J�������X�g
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_CameraList( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_TESTCAMERALIST_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuTestCameraListEvent, sizeof(DEBUG_TESTCAMERALIST_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_TESTCAMERALIST_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�e�X�g�J�������X�g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuTestCameraListEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_ZONESEL_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = TESTCAMERALISTMAX;
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_TestCameraList;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
				msgBG, NARC_message_d_field_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_TestCameraMenuList, max, work->msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 8, 7 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//���얳��
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );
			
			if( ret != FLDMENUFUNC_CANCEL ){	//����
				static const u16 length[TESTCAMERALISTMAX] =
					{ 0x0090, 0x0078, 0x0080, 0x0078 };
				static const fx32 height[TESTCAMERALISTMAX] =
					{ 0xae000, 0xa0000, 0xab000, 0xd8000 };
				FIELD_CAMERA *camera =
					FIELDMAP_GetFieldCamera( work->fieldWork );
				VecFx32 vec0, vec1;
				fx32 cos, len;

				// XZ���ʂ̋�����Y�����̍�����񂩂�
				// angle�p�x�@���������߂�
				//
				// FIELD_CAMERA�V�X�e���̏�����angle�ł̓���ɕύX��������
				// �C��
				len = FX_Mul( length[ret]<<FX32_SHIFT, length[ret]<<FX32_SHIFT ) + FX_Mul( height[ret], height[ret] );
				len = FX_Sqrt( len );

				VEC_Set( &vec0, 0,0,FX32_ONE );
				VEC_Set( &vec1, 0,height[ret],length[ret]<<FX32_SHIFT );
				VEC_Normalize( &vec0, &vec0 );
				VEC_Normalize( &vec1, &vec1 );
				cos = VEC_DotProduct( &vec0, &vec1 );
				
				FIELD_CAMERA_SetAnglePitch( camera, FX_AcosIdx( cos ) );
				FIELD_CAMERA_SetAngleLen( camera, len );
			}
			
			return( GMEVENT_RES_FINISH );
		}
		break;
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//	�f�o�b�O���j���[�@���샂�f���ꗗ
//======================================================================
//--------------------------------------------------------------
///	DEBUG_MMDL_LIST_EVENT_WORK ���샂�f�����X�g�����p���[�N
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
	MMDLSYS *fldmmdlsys;

	u16 obj_code;
	u16 res_add;
	MMDL *fmmdl;
}DEBUG_MMDLLIST_EVENT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuMMdlListEvent(
		GMEVENT *event, int *seq, void *wk );
static u16 * DEBUG_GetOBJCodeStrBuf( HEAPID heapID, u16 code );
static void DEBUG_SetMenuWorkMMdlList(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID );

///	���샂�f�����X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_MMdlList =
{
	1,		//���X�g���ڐ�
	10,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	0,		//�\�����WX �L�����P��
	0,		//�\�����WY �L�����P��
	0,		//�\���T�C�YX �L�����P��
	0,		//�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@���샂�f���ꗗ
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_MMdlList( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_MMDLLIST_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuMMdlListEvent, sizeof(DEBUG_MMDLLIST_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_MMDLLIST_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	
	{
		GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
		work->fldmmdlsys = GAMEDATA_GetMMdlSys( gdata );
	}

	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���샂�f���ꗗ
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuMMdlListEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_MMDLLIST_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			u32 max = OBJCODEMAX;
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_MMdlList;
			FLDMENUFUNC_LISTDATA *pMenuListData;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
					msgBG, NARC_message_d_field_dat );
			pMenuListData = FLDMENUFUNC_CreateListData( max, work->heapID );
			DEBUG_SetMenuWorkMMdlList( pMenuListData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 11, 16 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu(
					msgBG, &menuH, pMenuListData );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//���얳��
				break;
			}
			
			if( ret == FLDMENUFUNC_CANCEL || ret == NONDRAW ){ //�L�����Z��
				FLDMENUFUNC_DeleteMenu( work->menuFunc );
				return( GMEVENT_RES_FINISH );
			}
			
			work->obj_code = ret;
#if 0 //�v��Ȃ�
			work->res_add = MMDL_BLACTCONT_AddOBJCodeRes(
					work->fldmmdlsys, work->obj_code );
#endif			
			{
				//VecFx32 pos;
				MMDL *jiki;
				MMDL_HEADER head = {
					0,	///<����ID
					0,	///<�\������OBJ�R�[�h
					MV_RND,	///<����R�[�h
					0,	///<�C�x���g�^�C�v
					0,	///<�C�x���g�t���O
					0,	///<�C�x���gID
					0,	///<�w�����
					0,	///<�w��p�����^ 0
					0,	///<�w��p�����^ 1
					0,	///<�w��p�����^ 2
					4,	///<X�����ړ�����
					4,	///<Z�����ړ�����
					0,	///<�O���b�hX
					0,	///<�O���b�hZ
					0,	///<Y�l fx32�^
				};
				
				jiki = MMDLSYS_SearchOBJID(
					work->fldmmdlsys, MMDL_ID_PLAYER );
				
				head.id = 250;
				head.gx = MMDL_GetGridPosX( jiki ) + 2;
				head.gz = MMDL_GetGridPosZ( jiki );
				head.y = MMDL_GetVectorPosY( jiki );
				head.obj_code = work->obj_code;
				work->fmmdl = MMDLSYS_AddMMdl(
					work->fldmmdlsys, &head, 0 );
			}
			
			(*seq)++;
			break;
		case 2:
			{
				int key_trg = GFL_UI_KEY_GetTrg();
				MMDL_UpdateMoveProc( work->fmmdl );

				if( (key_trg & PAD_BUTTON_B) ){
					MMDL_Delete( work->fmmdl );
					
					if( work->res_add == TRUE ){
						MMDL_BLACTCONT_DeleteOBJCodeRes(
								work->fldmmdlsys, work->obj_code );
					}
					
					(*seq) = 1;
				}
			}
			break;
		}
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * ���샂�f��OBJ�R�[�h->STRCODE
 * @param	heapID	������o�b�t�@�m�ۗp�q�[�vID
 * @param	code	��������擾������OBJ�R�[�h
 * @retval	u16*	�����񂪊i�[���ꂽu16*(�J�����K�v
 */
//--------------------------------------------------------------
static u16 * DEBUG_GetOBJCodeStrBuf( HEAPID heapID, u16 code )
{
	int i;
	u16 utf16,utf16_eom;
	u16 *pStrBuf;
	u8 *name8;
	
	pStrBuf = GFL_HEAP_AllocClearMemory( heapID,
			sizeof(u16)*DEBUG_OBJCODE_STR_LENGTH );
	name8 = DEBUG_MMDL_GetOBJCodeString( code, heapID );
	utf16_eom = GFL_STR_GetEOMCode();
//	OS_Printf( "�ϊ� %s\n", name8 );
	
	for( i = 0; i < DEBUG_OBJCODE_STR_LENGTH; i++ ){
		utf16 = DEBUG_ASCIICODE_UTF16( name8[i] );
		pStrBuf[i] = utf16;
		if( utf16 == utf16_eom ){
			break;
		}
	}
	
	GFL_HEAP_FreeMemory( name8 );
	
	if( i >= DEBUG_OBJCODE_STR_LENGTH ){ //�������I�[�o�[
		GF_ASSERT( 0 );
		pStrBuf[DEBUG_OBJCODE_STR_LENGTH-1] = utf16_eom;
	}
	
	return( pStrBuf );
}

//--------------------------------------------------------------
/**
 * ���샂�f�����X�g�pBMP_MENULIST_DATA�Z�b�g
 * @param	list	�Z�b�g��BMP_MENULIST_DATA
 * @param	heapID	������o�b�t�@�m�ۗpHEAPID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkMMdlList(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID )
{
	u16 *str;
	int id,max = OBJCODEMAX;
	STRBUF *strBuf = GFL_STR_CreateBuffer( DEBUG_OBJCODE_STR_LENGTH, heapID );
	
	for( id = 0; id < max; id++ ){
		GFL_STR_ClearBuffer( strBuf );
		str = DEBUG_GetOBJCodeStrBuf( heapID, id );
		GFL_STR_SetStringCode( strBuf, str );
		GFL_HEAP_FreeMemory( str );
		FLDMENUFUNC_AddStringListData( list, strBuf, id, heapID );
	}
	
	GFL_HEAP_FreeMemory( strBuf );
}




//======================================================================
//	�f�o�b�O���j���[�@���C�g����
//======================================================================

//--------------------------------------------------------------
///	DEBUG_CTLIGHT_WORK ���C�g���샏�[�N
//--------------------------------------------------------------
typedef struct
{
	GAMESYS_WORK *gsys;
	GMEVENT *event;
	HEAPID heapID;
	FIELD_MAIN_WORK *fieldWork;

	GFL_BMPWIN* p_win;
}DEBUG_CTLLIGHT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlLight(
		GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�J��������
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_ControlLight( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_CTLLIGHT_WORK *work;
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	
	GMEVENT_Change( event, DMenuControlLight, sizeof(DEBUG_CTLLIGHT_WORK) );
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_CTLLIGHT_WORK) );
	
	work->gsys = gsys;
	work->event = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	return( TRUE );
}


//--------------------------------------------------------------
/**
 * �C�x���g�F�J��������
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlLight(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_CTLLIGHT_WORK *work = wk;
	FIELD_LIGHT* p_light;

	// ���C�g�擾
	p_light = FIELDMAP_GetFieldLight( work->fieldWork );
	
	switch( (*seq) ){
	case 0:
		// ���C�g�Ǘ��J�n
		FIELD_LIGHT_DEBUG_Init( p_light, work->heapID );

		// �C���t�H�[�o�[�̔�\��
		FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(work->fieldWork));
		GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

		// �r�b�g�}�b�v�E�B���h�E������
		{
			static const GFL_BG_BGCNT_HEADER header_sub3 = {
				0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
				GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
			};

			GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
			GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
			GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

			// �p���b�g����]��
			GFL_ARC_UTIL_TransVramPalette(
				ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, work->heapID );
			
			// �r�b�g�}�b�v�E�B���h�E���쐬
			work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
				0, 0, 32, 24,
				FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_F );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
			GFL_BMPWIN_MakeScreen( work->p_win );
			GFL_BMPWIN_TransVramCharacter( work->p_win );
			GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );
		}

		FIELD_LIGHT_DEBUG_PrintData( p_light, work->p_win );

		(*seq)++;
	case 1:
		// ���C�g�Ǘ����C��
		FIELD_LIGHT_DEBUG_Control( p_light );
		FIELD_LIGHT_DEBUG_PrintData( p_light, work->p_win );

		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
			(*seq)++;
		}
		break;
	case 2:
		// ���C�g�Ǘ��I��
		FIELD_LIGHT_DEBUG_Exit( p_light );

		// �r�b�g�}�b�v�E�B���h�E�j��
		{
			GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
			GFL_BMPWIN_Delete( work->p_win );
		}

		// �C���t�H�[�o�[�̕\��
    FIELDMAP_SetFieldSubscreenWork(work->fieldWork,
        FIELD_SUBSCREEN_Init( work->heapID, work->fieldWork, FIELD_SUBSCREEN_NORMAL ));

		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}


//======================================================================
//	�f�o�b�O���j���[�@�t�H�O����
//======================================================================

//--------------------------------------------------------------
///	DEBUG_CTLFOG_WORK �t�H�O���샏�[�N
//--------------------------------------------------------------
typedef struct
{
	GAMESYS_WORK *gsys;
	GMEVENT *event;
	HEAPID heapID;
	FIELD_MAIN_WORK *fieldWork;

	GFL_BMPWIN* p_win;

}DEBUG_CTLFOG_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlFog(
		GMEVENT *event, int *seq, void *wk );

static BOOL DMenuCallProc_ControlFog( DEBUG_MENU_EVENT_WORK *wk )
{
	DEBUG_CTLFOG_WORK *work;
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	
	GMEVENT_Change( event, DMenuControlFog, sizeof(DEBUG_CTLFOG_WORK) );
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_CTLFOG_WORK) );
	
	work->gsys = gsys;
	work->event = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	return( TRUE );
}


//--------------------------------------------------------------
/**
 * �C�x���g�F�ӂ�������
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlFog(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_CTLFOG_WORK *work = wk;
	FIELD_FOG_WORK* p_fog;

	// �t�H�O�擾
	p_fog = FIELDMAP_GetFieldFog( work->fieldWork );
	
	switch( (*seq) ){
	case 0:

		// �C���t�H�[�o�[�̔�\��
		FIELD_SUBSCREEN_Exit(FIELDMAP_GetFieldSubscreenWork(work->fieldWork));
		GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_OFF );

		// �r�b�g�}�b�v�E�B���h�E������
		{
			static const GFL_BG_BGCNT_HEADER header_sub3 = {
				0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
				GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
			};

			GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
			GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
			GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );

			// �p���b�g����]��
			GFL_ARC_UTIL_TransVramPalette(
				ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_SUB_BG, FIELD_SUBSCREEN_PALLET*32, 32, work->heapID );
			
			// �r�b�g�}�b�v�E�B���h�E���쐬
			work->p_win = GFL_BMPWIN_Create( FIELD_SUBSCREEN_BGPLANE,
				0, 0, 32, 24,
				FIELD_SUBSCREEN_PALLET, GFL_BMP_CHRAREA_GET_F );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
			GFL_BMPWIN_MakeScreen( work->p_win );
			GFL_BMPWIN_TransVramCharacter( work->p_win );
			GFL_BG_LoadScreenReq( FIELD_SUBSCREEN_BGPLANE );
		}

		FIELD_FOG_DEBUG_Init( p_fog, work->heapID );


		(*seq)++;
	case 1:
		// �t�H�O�Ǘ����C��
		FIELD_FOG_DEBUG_Control( p_fog );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->p_win ), 0xf );
		FIELD_FOG_DEBUG_PrintData( p_fog, work->p_win );
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
			(*seq)++;
		}
		break;
	case 2:

		// �r�b�g�}�b�v�E�B���h�E�j��
		{
			GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
			GFL_BMPWIN_Delete( work->p_win );
		}

		FIELD_FOG_DEBUG_Exit( p_fog );


		// �C���t�H�[�o�[�̕\��
    FIELDMAP_SetFieldSubscreenWork(work->fieldWork,
        FIELD_SUBSCREEN_Init( work->heapID, work->fieldWork, FIELD_SUBSCREEN_NORMAL ));

		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}


//======================================================================
//	�f�o�b�O���j���[�@�Ă񂫃��X�g
//======================================================================
//--------------------------------------------------------------
///	DEBUG_WEATERLIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
}DEBUG_WEATERLIST_EVENT_WORK;


//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuWeatherListEvent(
		GMEVENT *event, int *seq, void *work );

///�e�X�g�J�������X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_WeatherList =
{
	1,		//���X�g���ڐ�
	6,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	0,		//�\�����WX �L�����P��
	0,		//�\�����WY �L�����P��
	0,		//�\���T�C�YX �L�����P��
	0,		//�\���T�C�YY �L�����P��
};

///�Ă񂫃��j���[���X�g
static const FLDMENUFUNC_LIST DATA_WeatherMenuList[] =
{
	{ D_TOMOYA_WEATEHR00, (void*)WEATHER_NO_SUNNY },
	{ D_TOMOYA_WEATEHR01, (void*)WEATHER_NO_SNOW },
	{ D_TOMOYA_WEATEHR02, (void*)WEATHER_NO_RAIN },
	{ D_TOMOYA_WEATEHR03, (void*)WEATHER_NO_STORM },
	{ D_TOMOYA_WEATEHR05, (void*)WEATHER_NO_SPARK },
	{ D_TOMOYA_WEATEHR06, (void*)WEATHER_NO_SNOWSTORM },
	{ D_TOMOYA_WEATEHR07, (void*)WEATHER_NO_ARARE },
	{ D_TOMOYA_WEATEHR08, (void*)WEATHER_NO_MIRAGE },
};
#define DEBUG_WEATHERLIST_LIST_MAX	( NELEMS(DATA_WeatherMenuList) )

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�V�C���X�g
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_WeatherList( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_WEATERLIST_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuWeatherListEvent, sizeof(DEBUG_WEATERLIST_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_WEATERLIST_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�Ă񂫃��X�g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuWeatherListEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_ZONESEL_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = DEBUG_WEATHERLIST_LIST_MAX;
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_WeatherList;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
				msgBG, NARC_message_d_tomoya_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_WeatherMenuList, max, work->msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 8, 11 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//���얳��
				break;
			}
			
			FLDMENUFUNC_DeleteMenu( work->menuFunc );
			
			if( ret != FLDMENUFUNC_CANCEL ){	//����
				FIELD_WEATHER_Change( FIELDMAP_GetFieldWeather( work->fieldWork ), ret );
			}
			
			return( GMEVENT_RES_FINISH );
		}
		break;
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//	�f�o�b�O���j���[�@�ʒu���
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�t�B�[���h�ʒu���
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_FieldPosData( DEBUG_MENU_EVENT_WORK *wk )
{
	FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( wk->fieldWork );
	FIELD_DEBUG_SetPosPrint( debug );
	return( FALSE );
}

//======================================================================
///	�f�o�b�N���j���[�@����
//======================================================================
//--------------------------------------------------------------
///	DEBUG_CONTROL_TIME_LIST_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	HEAPID heapID;
	GAMESYS_WORK *gmSys;
	GMEVENT *gmEvent;
	FIELD_MAIN_WORK *fieldWork;
	GFL_MSGDATA *msgData;
	FLDMENUFUNC *menuFunc;
}DEBUG_CONTROL_TIME_LIST_EVENT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlTimeListEvent(
		GMEVENT *event, int *seq, void *work );


//--------------------------------------------------------------
///	data
//--------------------------------------------------------------
/// ���ԊǗ��^�C�v��
enum{
  CONT_TIME_TYPE_NORMAL,
  CONT_TIME_TYPE_8HOUR,
  CONT_TIME_TYPE_12HOUR,
  CONT_TIME_TYPE_18HOUR,
  CONT_TIME_TYPE_22HOUR,
  CONT_TIME_TYPE_2HOUR,
  CONT_TIME_TYPE_600RATE,
  CONT_TIME_TYPE_60RATE,

  CONT_TIME_TYPE_NUM,
} ;


///�e�X�g�J�������X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_ContTimeList =
{
	1,		//���X�g���ڐ�
	CONT_TIME_TYPE_NUM,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	0,		//�\�����WX �L�����P��
	0,		//�\�����WY �L�����P��
	0,		//�\���T�C�YX �L�����P��
	0,		//�\���T�C�YY �L�����P��
};

///�e�X�g�J�������j���[���X�g
static const FLDMENUFUNC_LIST DATA_ControlTimeMenuList[CONT_TIME_TYPE_NUM] =
{
	{ DEBUG_FIELD_STR30, (void*)CONT_TIME_TYPE_NORMAL,  },
	{ DEBUG_FIELD_STR23, (void*)CONT_TIME_TYPE_8HOUR,   },
	{ DEBUG_FIELD_STR24, (void*)CONT_TIME_TYPE_12HOUR,  },
	{ DEBUG_FIELD_STR25, (void*)CONT_TIME_TYPE_18HOUR,  },
	{ DEBUG_FIELD_STR26, (void*)CONT_TIME_TYPE_22HOUR,  },
	{ DEBUG_FIELD_STR27, (void*)CONT_TIME_TYPE_2HOUR,   },
	{ DEBUG_FIELD_STR28, (void*)CONT_TIME_TYPE_600RATE, },
	{ DEBUG_FIELD_STR29, (void*)CONT_TIME_TYPE_60RATE,  },
};

//--------------------------------------------------------------
/**
 * �f�o�b�O���j���[�Ăяo���@�e�X�g�J�������X�g
 * @param	wk	DEBUG_MENU_EVENT_WORK*
 * @retval	BOOL	TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_ControlRtcList( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_CONTROL_TIME_LIST_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuControlTimeListEvent, sizeof(DEBUG_CONTROL_TIME_LIST_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_CONTROL_TIME_LIST_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���ԑ��샊�X�g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuControlTimeListEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_CONTROL_TIME_LIST_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			FLDMENUFUNC_LISTDATA *listdata;
			u32 max = CONT_TIME_TYPE_NUM;
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_ContTimeList;
			
			msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
				msgBG, NARC_message_d_field_dat );
			listdata = FLDMENUFUNC_CreateMakeListData(
				DATA_ControlTimeMenuList, max, work->msgData, work->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, max, 1, 1, 9, 13 );
			
			work->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &menuH, listdata );
			GFL_MSG_Delete( work->msgData );
		}
		
		(*seq)++;
		break;
	case 1:
		{
			u32 ret;
			ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
			
			if( ret == FLDMENUFUNC_NULL ){	//���얳��
				break;
			}
			
			if( ret != FLDMENUFUNC_CANCEL ){	//����
        switch( ret ){
        case CONT_TIME_TYPE_NORMAL:
          GFL_RTC_DEBUG_StopFakeTime();
          break;
        case CONT_TIME_TYPE_8HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 8,0 );
        case CONT_TIME_TYPE_12HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 12,0 );
          break;
        case CONT_TIME_TYPE_18HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 18,0 );
          break;
        case CONT_TIME_TYPE_22HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 22,0 );
          break;
        case CONT_TIME_TYPE_2HOUR:
          GFL_RTC_DEBUG_StartFakeFixTime( 2,0 );
          break;
        case CONT_TIME_TYPE_600RATE:
          GFL_RTC_DEBUG_StartFakeTime( 600 );
          break;
        case CONT_TIME_TYPE_60RATE:
          GFL_RTC_DEBUG_StartFakeTime( 60 );
          break;
        default:
          break;
        }
			}else{

			  FLDMENUFUNC_DeleteMenu( work->menuFunc );
			
			
        // �I����
  			return( GMEVENT_RES_FINISH );
      }
		}
		break;
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  �f�o�b�O���j���[�@�g���
//======================================================================
//--------------------------------------------------------------
/**
 * �g���C�x���g�Ăяo��
 * @param wk DEBUG_MENU_EVENT_WORK*
 * @retval BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_Naminori( DEBUG_MENU_EVENT_WORK *wk )
{
  if( FIELDMAP_GetMapControlType(wk->fieldWork) != FLDMAP_CTRLTYPE_GRID ){
    return( FALSE );
  }
  
  FIELD_EVENT_ChangeNaminoriStart( wk->gmEvent, wk->gmSys, wk->fieldWork );
  return( TRUE );
}

//======================================================================
//  �f�o�b�O���j���[ �A�C�e��
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�A�C�e���쐬
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
static BOOL DMenuCallProc_DebugItem( DEBUG_MENU_EVENT_WORK *wk )
{
	GMEVENT *event = wk->gmEvent;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	GAMESYS_WORK	*gameSys	= wk->gmSys;

	EVENT_DebugItemMake(gameSys, fieldWork, event, wk->heapID);

	return( TRUE );
}

//--------------------------------------------------------------
/**
 * @brief   �|�P�����{�b�N�X�Ƀ|�P�����ł����ς��ɂ���
 * @param   wk DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------

static BOOL DMenuCallProc_BoxMax( DEBUG_MENU_EVENT_WORK *wk )
{
#if 1
	GAMESYS_WORK	*gameSys	= wk->gmSys;
  MYSTATUS *myStatus;
	POKEMON_PARAM *pp;
	const STRCODE *name;
	
  myStatus = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gameSys));
  name = MyStatus_GetMyName( myStatus );
  
	pp = PP_Create(MONSNO_WANIBAAN, 100, 123456, GFL_HEAPID_APP);

	{
		int i,j;
		BOX_DATA* pBox = SaveData_GetBoxData(GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gameSys)));

		for(i=0;i < 18;i++){
			for(j=0;j < 30;j++){
				int monsno =GFUser_GetPublicRand(MONSNO_END-1)+1;
				OS_TPrintf("%d  %d %d�쐬\n",monsno, i, j);
				PP_Setup(pp,  monsno , 30, 123456);
				PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
				PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );
				BOXDAT_PutPokemonBox(pBox, i, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pp));
			}
		}
	}
	
	GFL_HEAP_FreeMemory(pp);
#endif
	return( FALSE );
}

//======================================================================
//  �f�o�b�O���j���[ �������
//======================================================================
FS_EXTERN_OVERLAY(townmap);
//-------------------------------------
///	�f�o�b�O����ԗp���[�N	
//=====================================
typedef struct 
{
	GAMESYS_WORK		*p_gamesys;
	GMEVENT					*p_event;
	FIELD_MAIN_WORK *p_field;
	TOWNMAP_PARAM		*p_param;
} DEBUG_SKYJUMP_EVENT_WORK;
//-------------------------------------
///		PROTOTYPE
//=====================================
static GMEVENT_RESULT DMenuSkyJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�����
 *
 *	@param	DEBUG_MENU_EVENT_WORK *wk		���[�N
 *
 *	@return	TRUE�C�x���g�p��	FALSE�I��
 */
//-----------------------------------------------------------------------------
static BOOL DMenuCallProc_DebugSkyJump( DEBUG_MENU_EVENT_WORK *p_wk )
{	
	GAMESYS_WORK	*p_gamesys	= p_wk->gmSys;
	GMEVENT				*p_event		= p_wk->gmEvent;
	FIELD_MAIN_WORK *p_field	= p_wk->fieldWork;
	DEBUG_SKYJUMP_EVENT_WORK	*p_sky;

	//�C���F���g
	GMEVENT_Change( p_event, DMenuSkyJump, sizeof(DEBUG_SKYJUMP_EVENT_WORK) );
	p_sky = GMEVENT_GetEventWork( p_event );
	GFL_STD_MemClear( p_sky, sizeof(DEBUG_SKYJUMP_EVENT_WORK) );
	
	//���[�N�ݒ�
	p_sky->p_gamesys	= p_gamesys;
	p_sky->p_event		= p_event;
	p_sky->p_field		= p_field;

	//�^�E���}�b�v���[�N�ݒ�
	p_sky->p_param	= GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(TOWNMAP_PARAM) );
	GFL_STD_MemClear( p_sky->p_param, sizeof(TOWNMAP_PARAM) );
	p_sky->p_param->mode			= TOWNMAP_MODE_SKY;
	p_sky->p_param->is_debug	= TRUE;

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O����ԃC�x���g
 *
 *	@param	GMEVENT *event	GMEVENT
 *	@param	*seq						�V�[�P���X
 *	@param	*work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT DMenuSkyJump( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_FLD_FADEOUT,
		SEQ_FLD_CLOSE,
		SEQ_CALL_PROC,
		SEQ_PROC_MAIN,
		SEQ_END_PROC,
		SEQ_FLD_OPEN,
		SEQ_FLD_FADEIN,
		SEQ_PLACE_NAME,
		SEQ_EXIT,
	};

	DEBUG_SKYJUMP_EVENT_WORK	*p_wk	= p_wk_adrs;

	switch(*p_seq )
	{	
	case SEQ_INIT:

		PMSND_PauseBGM(TRUE);
		PMSND_PushBGM();
		*p_seq	= SEQ_FLD_FADEOUT;
		break;

	case SEQ_FLD_FADEOUT:
		GMEVENT_CallEvent(p_wk->p_event, EVENT_FieldFadeOut(p_wk->p_gamesys, p_wk->p_field, 0));
		*p_seq	= SEQ_FLD_CLOSE;
		break;

	case SEQ_FLD_CLOSE:
		GMEVENT_CallEvent(p_wk->p_event, EVENT_FieldClose(p_wk->p_gamesys, p_wk->p_field));
		*p_seq	= SEQ_CALL_PROC;
		break;

	case SEQ_CALL_PROC:
		GAMESYSTEM_CallProc(p_wk->p_gamesys, FS_OVERLAY_ID(townmap), &TownMap_ProcData, p_wk->p_param);
		*p_seq	= SEQ_PROC_MAIN;
		break;

	case SEQ_PROC_MAIN:
		if( !GAMESYSTEM_IsProcExists(p_wk->p_gamesys) )
		{	
			*p_seq	= SEQ_END_PROC;
		}
		break;

	case SEQ_END_PROC:
		if( p_wk->p_param->select == TOWNMAP_SELECT_SKY )
		{	
			GAMEDATA		*p_gamedata;
			PLAYER_WORK *p_player;
			LOCATION		location;

			VecFx32	pos;
			u32 zoneID;
			
			p_gamedata	= GAMESYSTEM_GetGameData( p_wk->p_gamesys );
			p_player		= GAMEDATA_GetMyPlayerWork( p_gamedata );

			LOCATION_Init( &location );
#if 0
			LOCATION_SetDirect( &location, p_wk->p_param->zoneID, 0, 
					p_wk->p_param->grid.x, 0, p_wk->p_param->grid.y );
#else
			LOCATION_DEBUG_SetDefaultPos(&location, p_wk->p_param->zoneID);
#endif

			PLAYERWORK_setZoneID( p_player, location.zone_id );
			PLAYERWORK_setPosition( p_player, &location.pos );
			GAMEDATA_SetStartLocation( p_gamedata, &location );
		}
		*p_seq	= SEQ_FLD_OPEN;
		break;

	case SEQ_FLD_OPEN:
		GMEVENT_CallEvent(p_wk->p_event, EVENT_FieldOpen(p_wk->p_gamesys));
		*p_seq	= SEQ_FLD_FADEIN;
		break;

	case SEQ_FLD_FADEIN:
		GMEVENT_CallEvent(p_wk->p_event, EVENT_FieldFadeIn(p_wk->p_gamesys, p_wk->p_field, 0));
		*p_seq	= SEQ_PLACE_NAME;
		break;

	case SEQ_PLACE_NAME:
		if( p_wk->p_param->select == TOWNMAP_SELECT_SKY )
		{		
			FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork(p_wk->p_gamesys);
			FIELD_PLACE_NAME_ZoneChange(FIELDMAP_GetPlaceNameSys(fieldmap), p_wk->p_param->zoneID);
		}
		*p_seq	= SEQ_EXIT;
		break;

	case SEQ_EXIT:
		if( p_wk->p_param )
		{	
			GFL_HEAP_FreeMemory(p_wk->p_param);
		}
		return GMEVENT_RES_FINISH;
	}

	return GMEVENT_RES_CONTINUE ;
}
