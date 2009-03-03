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

#include "field/field_msgbg.h"

#include "message.naix"
#include "msg/msg_d_field.h"

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

static DMenuCallProc_MapSeasonSelect( DEBUG_MENU_EVENT_WORK *wk );

static void DEBUG_SetMenuWorkZoneIDNameAll(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID );
static void DEBUG_SetMenuWorkZoneIDName(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID, u32 zoneID );

static BOOL DMenuCallProc_ControlCamera( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_CameraList( DEBUG_MENU_EVENT_WORK *wk );

static BOOL DMenuCallProc_FldMMdlList( DEBUG_MENU_EVENT_WORK *wk );

//--------------------------------------------------------------
///	�f�o�b�O���j���[���X�g�@�ėp
///	�f�[�^��ǉ����鎖�Ń��j���[�̍��ڂ������܂��B
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuList[] =
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
///	�f�o�b�O���j���[���X�g	�O���b�h�����}�b�v�p�B
///	�f�[�^��ǉ����鎖�Ń��j���[�̍��ڂ������܂��B
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuListGrid[] =
{
	{ DEBUG_FIELD_STR02, DMenuCallProc_ControlCamera },
	{ DEBUG_FIELD_STR03, DMenuCallProc_GridScaleSwitch },
	{ DEBUG_FIELD_STR04, DMenuCallProc_GridScaleControl },
	{ DEBUG_FIELD_STR05, DMenuCallProc_MapZoneSelect },
	{ DEBUG_FIELD_STR06, DMenuCallProc_MapSeasonSelect},
	{ DEBUG_FIELD_STR07, DMenuCallProc_CameraList },
	{ DEBUG_FIELD_STR01, DMenuCallProc_FldMMdlList },
	{ DEBUG_FIELD_C_CHOICE00, DMenuCallProc_OpenCommDebugMenu },
	{ DEBUG_FIELD_STR12, DMenuCallProc_OpenIRCBTLMenu },
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
		12,
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
		12,
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
			
			msgBG = FIELDMAP_GetFLDMSGBG( work->fieldWork );
			
			work->msgData = FLDMSGBG_CreateMSGDATA(
						msgBG, NARC_message_d_field_dat );
			
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
	const HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	GAMESYS_WORK	*gameSys	= wk->gmSys;
	IRC_BATTLE_MENU *work;
	
//	GMEVENT_Change( event,
//		IRCBATTLE_MENU_Main, IRCBATTLE_MENU_GetWorkSize() );
	
//	work = GMEVENT_GetEventWork( event );
	//IRCBATTLE_MENU_InitWork( heapID , gameSys , fieldWork , event , work );
//    GAMESYSTEM_SetEvent(gameSys, EVENT_IrcBattle(gameSys, fieldWork));


 //   GMEVENT_CallEvent(event, EVENT_IrcBattle(gameSys, fieldWork));

    EVENT_IrcBattle(gameSys, fieldWork, event);

#if 0
	GMEVENT *event = wk->gmEvent;
	const HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	GAMESYS_WORK	*gameSys	= wk->gmSys;
	FIELD_COMM_DEBUG_WORK *work;
	
	GMEVENT_Change( event,FIELD_COMM_DEBUG_CommDebugMenu, FIELD_COMM_DEBUG_GetWorkSize() );
	
	work = GMEVENT_GetEventWork( event );
	FIELD_COMM_DEBUG_InitWork( heapID , gameSys , fieldWork , event , work );

#endif


    
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
			
			msgBG = FIELDMAP_GetFLDMSGBG( work->fieldWork );
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
	
	if( zone_id == ZONE_ID_MAPSPRING || zone_id == ZONE_ID_MAPSUMMER ||
		zone_id == ZONE_ID_MAPAUTUMN || zone_id == ZONE_ID_MAPWINTER ){
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
	
	return( FALSE );
}

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
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_ZoneSel;
			FLDMENUFUNC_LISTDATA *pMenuListData;
			u32 i,tbl[4] = { ZONE_ID_MAPSPRING,ZONE_ID_MAPSUMMER,
				ZONE_ID_MAPAUTUMN,ZONE_ID_MAPWINTER };
			
			msgBG = FIELDMAP_GetFLDMSGBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
					msgBG, NARC_message_d_field_dat );
			
			pMenuListData = FLDMENUFUNC_CreateListData( 4, work->heapID );
			
			for( i = 0; i < 4; i++ ){
				DEBUG_SetMenuWorkZoneIDName(
					pMenuListData, work->heapID, tbl[i] );
			}
			
			FLDMENUFUNC_InputHeaderListSize( &menuH, 4, 1, 1, 11, 7 );
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
			}else{
				GMEVENT *event;
				GAMEDATA *gdata = GAMESYSTEM_GetGameData( work->gmSys );
				PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
				const VecFx32 *pos = PLAYERWORK_getPosition( player );
				u16 dir = PLAYERWORK_getDirection( player );
				
				if( (dir>0x2000) && (dir<0x6000) ){
					dir = EXIT_TYPE_LEFT;
				}else if( (dir >= 0x6000) && (dir <= 0xa000) ){
					dir = EXIT_TYPE_DOWN;
				}else if( (dir > 0xa000) && (dir < 0xe000) ){
					dir = EXIT_TYPE_RIGHT;
				}else{
					dir = EXIT_TYPE_UP;
				}

				event = DEBUG_EVENT_ChangeMapPos(
					work->gmSys, work->fieldWork, ret, pos, dir );
				GMEVENT_ChangeEvent( work->gmEvent, event );
				OS_Printf( "x = %xH, z = %xH\n", pos->x, pos->z );
			}
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
	work->pMsgBG = FIELDMAP_GetFLDMSGBG( work->fieldWork );
	work->pStrBuf = GFL_STR_CreateBuffer( 128, work->heapID );
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
	int update = FALSE;
	DEBUG_CTLCAMERA_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		work->pMsgWin = FLDMSGWIN_Add( work->pMsgBG, NULL, 21, 1, 10, 5 );
		update = TRUE;
		(*seq)++;
	case 1:
		{
			fx32 height;
			u16 dir,length;
			int trg = GFL_UI_KEY_GetTrg();
			int cont = GFL_UI_KEY_GetCont();
			FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( work->fieldWork );
			
			if( trg & PAD_BUTTON_B ){
				(*seq)++;
				break;
			}
			
			FLD_GetCameraDirection( camera, &dir );
			FLD_GetCameraLength( camera, &length );
			FLD_GetCameraHeight( camera, &height );

			if( cont & PAD_BUTTON_R ){
				dir -= CM_RT_SPEED;
				update = TRUE;
			}
	
			if( cont & PAD_BUTTON_L ){
				dir += CM_RT_SPEED;
				update = TRUE;
			}
	
			if( cont & PAD_KEY_LEFT ){
				if( length > 8 ){
					length -= 8;
					update = TRUE;
				}
			}

			if( cont & PAD_KEY_RIGHT ){
				if( length < 4096 ){
					length += 8;
					update = TRUE;
				}
			}
		
			if( cont & PAD_KEY_UP ){
				height -= CM_HEIGHT_MV;
				update = TRUE;
			}
	
			if( cont & PAD_KEY_DOWN ){
				height += CM_HEIGHT_MV;
				update = TRUE;
			}	

			FLD_SetCameraDirection( camera, &dir );
			FLD_SetCameraLength( camera, length );
			FLD_SetCameraHeight( camera, height );
			
			if( update == TRUE && work->vanish == FALSE ){
				int len = 128;
				char sjis[128];
				u16 ucode[128];
				
				FLDMSGWIN_ClearWindow( work->pMsgWin );

				sprintf( sjis, "LENGTH:%xH \0", length );
				STD_ConvertStringSjisToUnicode(
						ucode, &len, sjis, NULL, NULL );
				GFL_STR_SetStringCodeOrderLength( 
						work->pStrBuf, ucode, len );
				FLDMSGWIN_PrintStrBuf( work->pMsgWin, 1, 1, work->pStrBuf );
				
				len = 128;
				sprintf( sjis, "HEIGHT:%xH \0", height );
				STD_ConvertStringSjisToUnicode(
						ucode, &len, sjis, NULL, NULL );
				GFL_STR_SetStringCodeOrderLength(
						work->pStrBuf, ucode, len );
				FLDMSGWIN_PrintStrBuf( work->pMsgWin, 1, 12, work->pStrBuf );
				
				len = 128;
				sprintf( sjis, "DIR:%xH \0", dir );
				STD_ConvertStringSjisToUnicode(
						ucode, &len, sjis, NULL, NULL );
				GFL_STR_SetStringCodeOrderLength(
						work->pStrBuf, ucode, len );
				FLDMSGWIN_PrintStrBuf( work->pMsgWin, 1, 23, work->pStrBuf );
				
				FLD_MainCamera( camera, 0 );
			}
		}
		break;
	case 2:
		FLDMSGWIN_Delete( work->pMsgWin );
		GFL_STR_DeleteBuffer( work->pStrBuf );
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
			
			msgBG = FIELDMAP_GetFLDMSGBG( work->fieldWork );
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
				u16 length[TESTCAMERALISTMAX] =
					{ 0x0090, 0x0078, 0x0080, 0x0078 };
				fx32 height[TESTCAMERALISTMAX] =
					{ 0xae000, 0xa0000, 0xab000, 0xd8000 };
				u16 dir[TESTCAMERALISTMAX] =
					{ 0x0000, 0x0000, 0x0000, 0x0000 };
				FIELD_CAMERA *camera =
					FIELDMAP_GetFieldCamera( work->fieldWork );
				FLD_SetCameraDirection( camera, &dir[ret] );
				FLD_SetCameraLength( camera, length[ret] );
				FLD_SetCameraHeight( camera, height[ret] );
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
///	DEBUG_FLDMMDL_LIST_EVENT_WORK ���샂�f�����X�g�����p���[�N
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
	FLDMMDLSYS *fldmmdlsys;

	u16 obj_code;
	u16 res_add;
	FLDMMDL *fmmdl;
}DEBUG_FLDMMDLLIST_EVENT_WORK;

//--------------------------------------------------------------
///	proto
//--------------------------------------------------------------
static GMEVENT_RESULT DMenuFldMMdlListEvent(
		GMEVENT *event, int *seq, void *wk );
static u16 * DEBUG_GetOBJCodeStrBuf( HEAPID heapID, u16 code );
static void DEBUG_SetMenuWorkFldMMdlList(
		FLDMENUFUNC_LISTDATA *list, HEAPID heapID );

///	���샂�f�����X�g ���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DebugMenuList_FldMMdlList =
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
static BOOL DMenuCallProc_FldMMdlList( DEBUG_MENU_EVENT_WORK *wk )
{
	GAMESYS_WORK *gsys = wk->gmSys;
	GMEVENT *event = wk->gmEvent;
	HEAPID heapID = wk->heapID;
	FIELD_MAIN_WORK *fieldWork = wk->fieldWork;
	DEBUG_FLDMMDLLIST_EVENT_WORK *work;
	
	GMEVENT_Change( event,
		DMenuFldMMdlListEvent, sizeof(DEBUG_FLDMMDLLIST_EVENT_WORK) );
	
	work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(DEBUG_FLDMMDLLIST_EVENT_WORK) );
	
	work->gmSys = gsys;
	work->gmEvent = event;
	work->heapID = heapID;
	work->fieldWork = fieldWork;
	
	{
		GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
		work->fldmmdlsys = GAMEDATA_GetFldMMdlSys( gdata );
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
static GMEVENT_RESULT DMenuFldMMdlListEvent(
		GMEVENT *event, int *seq, void *wk )
{
	DEBUG_FLDMMDLLIST_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:
		{
			FLDMSGBG *msgBG;
			u32 max = OBJCODEMAX;
			FLDMENUFUNC_HEADER menuH = DATA_DebugMenuList_FldMMdlList;
			FLDMENUFUNC_LISTDATA *pMenuListData;
			
			msgBG = FIELDMAP_GetFLDMSGBG( work->fieldWork );
			work->msgData = FLDMSGBG_CreateMSGDATA(
					msgBG, NARC_message_d_field_dat );
			pMenuListData = FLDMENUFUNC_CreateListData( max, work->heapID );
			DEBUG_SetMenuWorkFldMMdlList( pMenuListData, work->heapID );
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
			work->res_add = FLDMMDL_BLACTCONT_AddOBJCodeRes(
					work->fldmmdlsys, work->obj_code );
			
			{
				VecFx32 pos;
				FLDMMDL *jiki;
				FLDMMDL_HEADER head = {
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
				
				jiki = FLDMMDLSYS_SearchOBJID(
					work->fldmmdlsys, FLDMMDL_ID_PLAYER );
				
				head.id = 250;
				head.gx = FLDMMDL_GetGridPosX( jiki ) + 2;
				head.gz = FLDMMDL_GetGridPosZ( jiki );
				head.y = FLDMMDL_GetVectorPosY( jiki );
				head.obj_code = work->obj_code;
				work->fmmdl = FLDMMDLSYS_AddFldMMdl(
					work->fldmmdlsys, &head, 0 );
			}
			
			(*seq)++;
			break;
		case 2:
			{
				int key_trg = GFL_UI_KEY_GetTrg();
				FLDMMDL_UpdateMoveProc( work->fmmdl );

				if( (key_trg & PAD_BUTTON_B) ){
					FLDMMDL_Delete( work->fmmdl );
					
					if( work->res_add == TRUE ){
						FLDMMDL_BLACTCONT_DeleteOBJCodeRes(
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
	name8 = DEBUG_FLDMMDL_GetOBJCodeString( code, heapID );
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
static void DEBUG_SetMenuWorkFldMMdlList(
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
