//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		event_shortcut_menu.c
 *	@brief	����Y�{�^���֗����j���[�C�x���g
 *	@author	Toru=Nagihashi
 *	@data		2009.10.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"
#include "gamesystem/gamesystem.h"

//���W���[��
#include "field/shortcut_menu.h"
#include "field/event_field_proclink.h"

//�t�B�[���h
#include "field/fieldmap.h"
#include "field/field_msgbg.h"
#include "itemuse_event.h"

//�A�[�J�C�u
#include "arc_def.h"

//�Z�[�u�f�[�^
#include "savedata/shortcut.h"

//�O���Q��
#include "event_shortcut_menu.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	BG�t���[��
//=====================================
enum
{
	BG_FRAME_WINDOW_M	= GFL_BG_FRAME1_M
} ;

//-------------------------------------
///	
//=====================================
typedef enum
{
	CALLTYPE_PROC,
	CALLTYPE_ITEM,
} CALLTYPE;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct 
{
	GMEVENT								*p_event;		//�����̃C�x���g
	GAMESYS_WORK					*p_gamesys;	//�Q�[���V�X�e��
	FIELDMAP_WORK					*p_fieldmap;//�t�B�[���h���[�N
	SHORTCUTMENU_WORK			*p_menu;		//�V���[�g�J�b�g���j���[
	EVENT_PROCLINK_PARAM	*p_link;		//�����N�C�x���g�p�����[�^
	HEAPID								heapID;			//�q�[�vID
} EVENT_SHORTCUTMENU_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
 */
//=============================================================================
//�C�x���g
static GMEVENT_RESULT ShortCutMenu_MainEvent( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
static GMEVENT_RESULT ShortCutMenu_OneEvent( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//�V���[�g�J�b�gID�ƃv���Z�X�̑Ή�
static CALLTYPE ShortCutMenu_SetCallType( EVENT_PROCLINK_PARAM *p_param, SHORTCUT_ID shortcutID );

//���j���[�쐬
static void ShortCutMenu_Init( SHORTCUTMENU_MODE mode, EVENT_SHORTCUTMENU_WORK *p_wk );
static void ShortCutMenu_Exit( EVENT_SHORTCUTMENU_WORK *p_wk );

//�R�[���o�b�N
static void ShortCutMenu_Open_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs );
static void ShortCutMenu_Close_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs );

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	GAMESYS_WORK *p_gamesys 
 *
 *	@return	TRUE�ŃV���[�g�J�b�g���j���[���J����	FALSE�ŊJ���Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL EVENT_ShortCutMenu_IsOpen( GAMESYS_WORK *p_gamesys )
{	
	GAMEDATA					*p_gdata;
	SAVE_CONTROL_WORK	*p_sv;
	const SHORTCUT		*cp_shortcut;

	p_gdata	= GAMESYSTEM_GetGameData( p_gamesys );
	p_sv		= GAMEDATA_GetSaveControlWork( p_gdata );
	cp_shortcut	= SaveData_GetShortCutConst( p_sv );

	return SHORTCUT_GetMax( cp_shortcut ) != 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief	Y�{�^���֗����j���[�C�x���g�N��
 *
 *	@param	GAMESYS_WORK *p_gamesys	�Q�[���V�X�e��
 *	@param	*p_fieldmap							�t�B�[���h
 *	@param	heapID									�q�[�vID
 *
 *	@return	�C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_ShortCutMenu( GAMESYS_WORK *p_gamesys, FIELDMAP_WORK *p_fieldmap, HEAPID heapID )
{
  EVENT_SHORTCUTMENU_WORK *p_wk;
  GMEVENT *p_event;
	
	GAMEDATA					*p_gdata;
	SAVE_CONTROL_WORK	*p_sv;
	const SHORTCUT		*cp_shortcut;

	p_gdata	= GAMESYSTEM_GetGameData( p_gamesys );
	p_sv		= GAMEDATA_GetSaveControlWork( p_gdata );
	cp_shortcut	= SaveData_GetShortCutConst( p_sv );

	if( SHORTCUT_GetMax( cp_shortcut ) == 1 )
	{	
		//�P�������s�C�x���g�쐬
		p_event = GMEVENT_Create(
				p_gamesys, NULL, ShortCutMenu_OneEvent, sizeof(EVENT_SHORTCUTMENU_WORK));
	}
	else
	{	
		//���j���[���J���C�x���g�쐬
		p_event = GMEVENT_Create(
				p_gamesys, NULL, ShortCutMenu_MainEvent, sizeof(EVENT_SHORTCUTMENU_WORK));
	}
  
	//���[�N�擾
  p_wk = GMEVENT_GetEventWork(p_event);
  GFL_STD_MemClear( p_wk, sizeof(EVENT_SHORTCUTMENU_WORK) );
  
	//���[�N������
  p_wk->p_gamesys		= p_gamesys;
  p_wk->p_event			= p_event;
  p_wk->p_fieldmap	= p_fieldmap;
  p_wk->heapID = heapID;

	//�����N�p�p�����[�^
	p_wk->p_link	= GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(EVENT_PROCLINK_PARAM) );
	GFL_STD_MemClear( p_wk->p_link, sizeof(EVENT_PROCLINK_PARAM) );
	p_wk->p_link->gsys				= p_wk->p_gamesys;
	p_wk->p_link->field_wk		= p_wk->p_fieldmap;
	p_wk->p_link->event				= p_event;


	//��������Ƃ��Ƃ����ƃ��j���[���J���悤�ɂ���
	if( SHORTCUT_GetMax( cp_shortcut ) != 1 )
	{	
		p_wk->p_link->open_func		= ShortCutMenu_Open_Callback;
		p_wk->p_link->close_func	= ShortCutMenu_Close_Callback;
		p_wk->p_link->wk_adrs			= p_wk;
	}

	//�RD���f���̃|�[�Y
	{	
		MMDLSYS *p_fldmdl = FIELDMAP_GetMMdlSys( p_wk->p_fieldmap );
		MMDLSYS_PauseMoveProc( p_fldmdl );
	}

  
  return p_event;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�C�x���g�t�B�[���h�}�b�v���j���[
 *
 *	@param	GMEVENT	*p_event	�C�x���g
 *	@param	*p_seq			�V�[�P���X
 *	@param	*p_wk_adrs	�C�x���g�p���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT ShortCutMenu_MainEvent( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_OPEN_START,
		SEQ_OPEN_WAIT,
		SEQ_MAIN,
		SEQ_CLOSE_START,
		SEQ_CLOSE_WAIT,
		SEQ_EXIT,
		SEQ_RESTART,

		SEQ_EVENT_CALL,
		SEQ_EVENT_RETURN,

		SEQ_ITEM_CALL,
		SEQ_ITEM_RETURN,
	};

	EVENT_SHORTCUTMENU_WORK	*p_wk	= p_wk_adrs;

	switch( *p_seq )
	{	
		//-------------------------------------
		///	���j���[����
		//=====================================
	case SEQ_INIT:
		ShortCutMenu_Init( SHORTCUTMENU_MODE_POPUP, p_wk );
		*p_seq	= SEQ_OPEN_START;
		break;

	case SEQ_OPEN_START:
		SHORTCUTMENU_Open( p_wk->p_menu );
		*p_seq	= SEQ_OPEN_WAIT;
		break;

	case SEQ_OPEN_WAIT:
		SHORTCUTMENU_Main( p_wk->p_menu );
		if( !SHORTCUTMENU_IsMove( p_wk->p_menu ) )
		{	
			*p_seq	= SEQ_MAIN;
		}
		break;

	case SEQ_MAIN:
		{	
			SHORTCUT_ID	shortcutID;
			SHORTCUTMENU_INPUT	input;
			SHORTCUTMENU_Main( p_wk->p_menu );
			input	= SHORTCUTMENU_GetInput( p_wk->p_menu, &shortcutID );
			if( input == SHORTCUTMENU_INPUT_SELECT )
			{	
				switch( ShortCutMenu_SetCallType( p_wk->p_link, shortcutID ) )
				{	
				case CALLTYPE_PROC:
					*p_seq	= SEQ_EVENT_CALL;
					break;
				case CALLTYPE_ITEM:
					*p_seq	= SEQ_ITEM_CALL;
					break;
				}
			}
			else if( input == SHORTCUTMENU_INPUT_CANCEL )
			{	
				*p_seq	= SEQ_CLOSE_START;
			}
		}
		break;

	case SEQ_CLOSE_START:
		SHORTCUTMENU_Close( p_wk->p_menu );
		*p_seq	= SEQ_CLOSE_WAIT;
		break;

	case SEQ_CLOSE_WAIT:
		SHORTCUTMENU_Main( p_wk->p_menu );
		if( !SHORTCUTMENU_IsMove( p_wk->p_menu ) )
		{	
			*p_seq	= SEQ_EXIT;
		}

		break;

	case SEQ_EXIT:
		ShortCutMenu_Exit( p_wk );

		//�I���Ȃ̂ŁA�������j��
		GFL_HEAP_FreeMemory( p_wk->p_link );
		{
   		MMDLSYS *p_fldmdl = FIELDMAP_GetMMdlSys( p_wk->p_fieldmap );
      MMDLSYS_ClearPauseMoveProc( p_fldmdl );
		}
		return GMEVENT_RES_FINISH;	

	case SEQ_RESTART:
		ShortCutMenu_Init( SHORTCUTMENU_MODE_STAY, p_wk );
		*p_seq	= SEQ_MAIN;
		break;


		//-------------------------------------
		///	�C�x���g�J��
		//=====================================
	case SEQ_EVENT_CALL:
		{	
			GMEVENT_CallEvent( p_event, EVENT_ProcLink( p_wk->p_link, p_wk->heapID ) );
			*p_seq	= SEQ_EVENT_RETURN;
		}
		break;

	case SEQ_EVENT_RETURN:

		if( p_wk->p_link->result == EVENT_PROCLINK_RESULT_RETURN )
		{
			*p_seq	= SEQ_RESTART;
		}
		else if( p_wk->p_link->result == EVENT_PROCLINK_RESULT_EXIT )
		{	
			*p_seq	= SEQ_EXIT;
		}
		else if( p_wk->p_link->result == EVENT_PROCLINK_RESULT_ITEM )
		{	
			*p_seq	= SEQ_ITEM_CALL;
		}
		break;

		//-------------------------------------
		///	�A�C�e���g�p
		//=====================================
	case SEQ_ITEM_CALL:
		{	
			GMEVENT *p_item_event;
			u32 item;

			//MENU�j��
			ShortCutMenu_Exit( p_wk );

			//����������܂��ɏ����󂯎��
			item	= p_wk->p_link->select_param;
			GFL_HEAP_FreeMemory( p_wk->p_link );
			p_wk->p_link	= NULL;

			//�A�C�e���R�[��
			GFL_OVERLAY_Load( FS_OVERLAY_ID(itemuse) );
			p_item_event	= EVENT_FieldItemUse( item, p_wk->p_gamesys, p_wk->p_fieldmap );
			GMEVENT_CallEvent(p_event, p_item_event );

			*p_seq	= SEQ_ITEM_RETURN;
		}
		break;

	case SEQ_ITEM_RETURN:
		GFL_OVERLAY_Unload( FS_OVERLAY_ID(itemuse) );

		{
   		MMDLSYS *p_fldmdl = FIELDMAP_GetMMdlSys( p_wk->p_fieldmap );
      MMDLSYS_ClearPauseMoveProc( p_fldmdl );
		}
		return GMEVENT_RES_FINISH;
	}

  return GMEVENT_RES_CONTINUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�^�����P�̂Ƃ����j���[���J�����C�x���g�����s����
 *
 *	@param	GMEVENT	*p_event	�C�x���g
 *	@param	*p_seq			�V�[�P���X
 *	@param	*p_wk_adrs	�C�x���g�p���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT ShortCutMenu_OneEvent( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_EXIT,

		SEQ_EVENT_CALL,
		SEQ_EVENT_RETURN,

		SEQ_ITEM_CALL,
		SEQ_ITEM_RETURN,
	};

	EVENT_SHORTCUTMENU_WORK	*p_wk	= p_wk_adrs;

	switch( *p_seq )
	{	
	case SEQ_INIT:
		{	
			SHORTCUT_ID shortcutID;

			GAMEDATA					*p_gdata;
			SAVE_CONTROL_WORK	*p_sv;
			const SHORTCUT		*cp_shortcut;

			p_gdata	= GAMESYSTEM_GetGameData( p_wk->p_gamesys );
			p_sv		= GAMEDATA_GetSaveControlWork( p_gdata );
			cp_shortcut	= SaveData_GetShortCutConst( p_sv );
	
			shortcutID	= SHORTCUT_GetType( cp_shortcut, 0 );
			switch( ShortCutMenu_SetCallType( p_wk->p_link, shortcutID ) )
			{	
			case CALLTYPE_PROC:
				*p_seq	= SEQ_EVENT_CALL;
				break;
			case CALLTYPE_ITEM:
				*p_seq	= SEQ_ITEM_CALL;
				break;
			}
		}
		break;

	case SEQ_EXIT:

		//�I���Ȃ̂ŁA�������j��
		GFL_HEAP_FreeMemory( p_wk->p_link );
		{
   		MMDLSYS *p_fldmdl = FIELDMAP_GetMMdlSys( p_wk->p_fieldmap );
      MMDLSYS_ClearPauseMoveProc( p_fldmdl );
		}
		return GMEVENT_RES_FINISH;	

		//-------------------------------------
		///	�C�x���g�J��
		//=====================================
	case SEQ_EVENT_CALL:
		{	
			ShortCutMenu_Exit( p_wk );

			GMEVENT_CallEvent( p_event, EVENT_ProcLink( p_wk->p_link, p_wk->heapID ) );
			*p_seq	= SEQ_EVENT_RETURN;
		}
		break;

	case SEQ_EVENT_RETURN:

		if( p_wk->p_link->result == EVENT_PROCLINK_RESULT_RETURN )
		{
			*p_seq	= SEQ_EXIT;
		}
		else if( p_wk->p_link->result == EVENT_PROCLINK_RESULT_EXIT )
		{	
			*p_seq	= SEQ_EXIT;
		}
		else if( p_wk->p_link->result == EVENT_PROCLINK_RESULT_ITEM )
		{	
			*p_seq	= SEQ_ITEM_CALL;
		}
		break;

		//-------------------------------------
		///	�A�C�e���g�p
		//=====================================
	case SEQ_ITEM_CALL:
		{	
			GMEVENT *p_item_event;
			u32 item;

			//����������܂��ɏ����󂯎��
			item	= p_wk->p_link->select_param;
			GFL_HEAP_FreeMemory( p_wk->p_link );
			p_wk->p_link	= NULL;

			//�A�C�e���R�[��
			GFL_OVERLAY_Load( FS_OVERLAY_ID(itemuse) );
			p_item_event	= EVENT_FieldItemUse( item, p_wk->p_gamesys, p_wk->p_fieldmap );
			GMEVENT_CallEvent(p_event, p_item_event );

			*p_seq	= SEQ_ITEM_RETURN;
		}
		break;

	case SEQ_ITEM_RETURN:
		GFL_OVERLAY_Unload( FS_OVERLAY_ID(itemuse) );
		{
   		MMDLSYS *p_fldmdl = FIELDMAP_GetMMdlSys( p_wk->p_fieldmap );
      MMDLSYS_ClearPauseMoveProc( p_fldmdl );
		}
		return GMEVENT_RES_FINISH;
	}

  return GMEVENT_RES_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�V���[�g�J�b�gID���R�[���^�C�v��
 *
 *  @param	param
 *	@param	SHORTCUT_ID shortcutID	�V���[�g�J�b�gID
 *
 *	@return	�R�[���^�C�v
 */
//-----------------------------------------------------------------------------
static CALLTYPE ShortCutMenu_SetCallType( EVENT_PROCLINK_PARAM *p_param, SHORTCUT_ID shortcutID )
{	
	switch( shortcutID )
	{	
	case SHORTCUT_ID_ZITENSYA:			//���]��
		p_param->select_param	= EVENT_ITEMUSE_CALL_CYCLE;
		return CALLTYPE_ITEM;
	case SHORTCUT_ID_TOWNMAP:			//�^�E���}�b�v
		p_param->call = EVENT_PROCLINK_CALL_TOWNMAP;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_PALACEGO:			//�p���X�փS�[
		p_param->select_param	= EVENT_ITEMUSE_CALL_PALACEJUMP;
		return CALLTYPE_ITEM;
	case SHORTCUT_ID_BTLRECORDER:	//�o�g�����R�[�_�[
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_FRIENDNOTE:		//�F�B���
		p_param->call = EVENT_PROCLINK_CALL_WIFINOTE;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_TURIZAO:			//�肴��
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_PSTATUS_STATUS:		//�|�P������񁄃X�e�[�^�X
		p_param->call = EVENT_PROCLINK_CALL_STATUS;
		p_param->data	= PPT_INFO;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_PSTATUS_ABILITY:	//�|�P������񁄂̂���傭
		p_param->call = EVENT_PROCLINK_CALL_STATUS;
		p_param->data	= PPT_SKILL;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_PSTATUS_RIBBON:		//�|�P������񁄂��˂񃊃{��
		p_param->call = EVENT_PROCLINK_CALL_STATUS;
		p_param->data	= PPT_RIBBON;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_BAG_ITEM:					//�o�b�O���ǂ���
		p_param->call = EVENT_PROCLINK_CALL_BAG;
		p_param->data	= ITEMPOCKET_NORMAL;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_BAG_RECOVERY:			//�o�b�O�������ӂ�
		p_param->call = EVENT_PROCLINK_CALL_BAG;
		p_param->data	= ITEMPOCKET_DRUG;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_BAG_WAZAMACHINE:	//�o�b�O���Z�}�V��
		p_param->call = EVENT_PROCLINK_CALL_BAG;
		p_param->data	= ITEMPOCKET_WAZA;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_BAG_NUTS:					//�o�b�O�����̂�
		p_param->call = EVENT_PROCLINK_CALL_BAG;
		p_param->data	= ITEMPOCKET_NUTS;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_BAG_IMPORTANT:		//�o�b�O���������Ȃ���
		p_param->call = EVENT_PROCLINK_CALL_BAG;
		p_param->data	= ITEMPOCKET_EVENT;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_ZUKAN_MENU:				//�����񁄃��j���[
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_ZUKAN_BUNPU:			//�����񁄕��z
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_ZUKAN_CRY:				//�����񁄂Ȃ�����
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_TRCARD_FRONT:			//�g���[�i�[�J�[�h������߂�
		p_param->call = EVENT_PROCLINK_CALL_TRAINERCARD;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_TRCARD_BACK:			//�g���[�i�[�J�[�h������߂�
		p_param->call = EVENT_PROCLINK_CALL_TRAINERCARD;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_CONFIG:						//�����Ă�
		p_param->call = EVENT_PROCLINK_CALL_CONFIG;
		return CALLTYPE_PROC;
	default:
		GF_ASSERT(0);
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		return CALLTYPE_PROC;
	}

}
//=============================================================================
/**
 *	���j���[�쐬�j��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	FLDMSGBG�̊��蓖�Ă��������AMENU��������
 *
 *	@param	mode�N�����[�h
 *	@param	EVENT_SHORTCUTMENU_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void ShortCutMenu_Init( SHORTCUTMENU_MODE mode, EVENT_SHORTCUTMENU_WORK *p_wk )
{	
	if( p_wk->p_menu == NULL )
	{	
		{	
			FLDMSGBG *p_msgbg;

			p_msgbg	= FIELDMAP_GetFldMsgBG( p_wk->p_fieldmap );
			FLDMSGBG_ReleaseBGResouce( p_msgbg );
		}

		{	
			GAMEDATA	*p_gdata	= GAMESYSTEM_GetGameData( p_wk->p_gamesys );
			SHORTCUT_CURSOR	*p_cursor	= GAMEDATA_GetShortCutCursor( p_gdata );
			p_wk->p_menu	= SHORTCUTMENU_Init( mode, p_cursor, p_wk->heapID );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	MENU��j�����AFLDMSGBG�����蓖��
 *
 *	@param	EVENT_SHORTCUTMENU_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void ShortCutMenu_Exit( EVENT_SHORTCUTMENU_WORK *p_wk )
{	
	if( p_wk->p_menu != NULL )
	{
		//SHORTCUT��j�����AMSGBG���쐬
		FLDMSGBG *p_msgbg;

		SHORTCUTMENU_Exit( p_wk->p_menu );
		p_wk->p_menu	= NULL;

		p_msgbg	= FIELDMAP_GetFldMsgBG( p_wk->p_fieldmap );
		FLDMSGBG_ResetBGResource( p_msgbg );
	}
}
//=============================================================================
/**
 *	�R�[���o�b�N
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�����N�ɓn���R�[���o�b�N
 *
 *	@param	const EVENT_PROCLINK_PARAM *param	����
 *	@param	*wk_adrs													�����̃��[�N
 */
//-----------------------------------------------------------------------------
static void ShortCutMenu_Open_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs )
{	
	EVENT_SHORTCUTMENU_WORK *p_wk	= wk_adrs;
	ShortCutMenu_Init( SHORTCUTMENU_MODE_STAY, p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�����N�ɓn���R�[���o�b�N
 *
 *	@param	const EVENT_PROCLINK_PARAM *param	����
 *	@param	*wk_adrs													�����̃��[�N
 */
//-----------------------------------------------------------------------------
static void ShortCutMenu_Close_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs )
{	

	EVENT_SHORTCUTMENU_WORK *p_wk	= wk_adrs;
	ShortCutMenu_Exit( p_wk );
}
