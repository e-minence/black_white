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

//�v���Z�X�i��ɋN�������[�h�̂��߁j
#include "app/zukan.h"
#include "app/pokelist.h"
#include "app/trainer_card.h"

//�X�N���v�g�C�x���g�Ăяo���̂���
#include "field/script.h"
#include "field/script_local.h"
#include "../../../resource/fldmapdata/script/common_scr_def.h" //SCRID_�`

//�Z�[�u�f�[�^
#include "savedata/shortcut.h"

//�O���Q��
#include "event_shortcut_menu.h"

//-------------------------------------
///	
//=====================================
#ifdef PM_DEBUG

static int s_cnt  = 0;
#endif //PM_DEBUG

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
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
	HEAPID								heapID;			//�풓�q�[�vID
	BOOL									is_empty;		//���j���[����ɂ����t���O
  u32                   exit_seq;
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
static BOOL GetItemCheckEnable( SHORTCUT_ID shortcutID, ITEMCHECK_ENABLE * enable, BOOL *reverse_use );

//���j���[�쐬
static void ShortCutMenu_Init( SHORTCUTMENU_MODE mode, EVENT_SHORTCUTMENU_WORK *p_wk );
static void ShortCutMenu_Exit( EVENT_SHORTCUTMENU_WORK *p_wk );
static BOOL ShortCutMenu_ExitSeq( EVENT_SHORTCUTMENU_WORK *p_wk );

//�R�[���o�b�N
static void ShortCutMenu_Open_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs );
static void ShortCutMenu_Close_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs );

//�A�C�e���g�pNG���b�Z�[�W�\���C�x���g����
static GMEVENT* EVENT_ItemuseNGMsgCall(GAMESYS_WORK * gsys, u8 type );

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
  p_wk->heapID			= heapID;
	p_wk->is_empty		= FALSE;

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
		SEQ_ITEM_ERROR,
		SEQ_ITEM_RETURN,
    
    SEQ_RIBBON_ERROR,
    SEQ_RIBBON_RETURN,
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
			{	// �N���`�F�b�N
				ITEMCHECK_ENABLE	enable;
				BOOL	item_use_err = FALSE;
        BOOL  ribbon_status_err = FALSE;
				CALLTYPE	call_type;

				BOOL reverse_use, check_item;
				check_item = GetItemCheckEnable( shortcutID, &enable, &reverse_use );
				call_type = ShortCutMenu_SetCallType( p_wk->p_link, shortcutID );

				if(reverse_use == FALSE 
				    && GAMEDATA_GetIntrudeReverseArea( GAMESYSTEM_GetGameData(p_wk->p_gamesys) ) == TRUE)
        {
          item_use_err = TRUE;
        }
				else if(check_item == TRUE)
        {
					ITEMCHECK_WORK	icwk;
					ITEMUSE_InitCheckWork( &icwk, p_wk->p_gamesys, p_wk->p_fieldmap );
					if( ITEMUSE_GetItemUseCheck( &icwk, enable ) == FALSE ){
						item_use_err = TRUE;
					}
				}

        if( shortcutID == SHORTCUT_ID_PSTATUS_RIBBON )
        { 
          //���{���̏ꍇ�́A�莝�����Ƀ��{�������Ă���|�P����������Δ�Ԃ�
          //���Ȃ���΃G���[���b�Z�[�W
          GAMEDATA  *p_gamedata = GAMESYSTEM_GetGameData(p_wk->p_gamesys);
          POKEPARTY *p_party  =  GAMEDATA_GetMyPokemon(p_gamedata);
          BOOL is_ribbon  = FALSE;
          POKEMON_PARAM *p_pp;
          int i;
          for( i = 0; i < PokeParty_GetPokeCount(p_party); i++ )
          { 
            p_pp  = PokeParty_GetMemberPointer( p_party, i );
            if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) && PP_CheckRibbon( p_pp ) )
            { 
              is_ribbon = TRUE;
            }
          }

          if( is_ribbon == FALSE )
          { 
            ribbon_status_err = TRUE;
          }
        }

				if( item_use_err == TRUE )
        {
          // �N���G���[
					*p_seq = SEQ_ITEM_ERROR;
				}
        else if( ribbon_status_err == TRUE )
        { 
          //���{����ʃG���[
          *p_seq  = SEQ_RIBBON_ERROR;
        }
        else
        {
          // �N��
					switch( call_type )
					{	
					case CALLTYPE_PROC:
						*p_seq	= SEQ_EVENT_CALL;
						break;
					case CALLTYPE_ITEM:
						*p_seq	= SEQ_ITEM_CALL;
						break;
					}
				}

        //�A�v���̋N���`�F�b�N

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
		if( ShortCutMenu_ExitSeq( p_wk ) )
    {
      //�I���Ȃ̂ŁA�������j��
      GFL_HEAP_FreeMemory( p_wk->p_link );
      {
        MMDLSYS *p_fldmdl = FIELDMAP_GetMMdlSys( p_wk->p_fieldmap );
        MMDLSYS_ClearPauseMoveProc( p_fldmdl );
      }
      return GMEVENT_RES_FINISH;	
    }
    break;

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
			if( p_wk->is_empty )
			{	
				*p_seq	= SEQ_EXIT;
			}
			else
			{	
				*p_seq	= SEQ_RESTART;
			}
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
			if( ShortCutMenu_ExitSeq( p_wk ) )
      {

        //����������܂��ɏ����󂯎��
        item	= p_wk->p_link->select_param;
        GFL_HEAP_FreeMemory( p_wk->p_link );
        p_wk->p_link	= NULL;

        // �A�C�e���R�[��
        p_item_event	= EVENT_FieldItemUse( item, p_wk->p_gamesys, p_wk->p_fieldmap );
        GMEVENT_CallEvent(p_event, p_item_event );

        *p_seq	= SEQ_ITEM_RETURN;
      }
		}
		break;

	case SEQ_ITEM_ERROR:
		{	
			GMEVENT *p_item_event;
			u32 item;

			//MENU�j��
			if( ShortCutMenu_ExitSeq( p_wk ) )
      {
        //����������܂��ɏ����󂯎��
        item	= p_wk->p_link->select_param;
        GFL_HEAP_FreeMemory( p_wk->p_link );
        p_wk->p_link	= NULL;

        // �A�C�e���R�[��
        if( item == EVENT_ITEMUSE_CALL_CYCLE ){
          PLAYER_WORK * plwk = GAMEDATA_GetMyPlayerWork( GAMESYSTEM_GetGameData(p_wk->p_gamesys) );
          if( PLAYERWORK_GetMoveForm( plwk ) == PLAYER_MOVE_FORM_CYCLE ){
            p_item_event = EVENT_ItemuseNGMsgCall( p_wk->p_gamesys, 1 );
          }else{
            p_item_event = EVENT_ItemuseNGMsgCall( p_wk->p_gamesys, 0 );
          }
        }else{
          p_item_event = EVENT_ItemuseNGMsgCall( p_wk->p_gamesys, 0 );
        }
        GMEVENT_CallEvent(p_event, p_item_event );

        *p_seq	= SEQ_ITEM_RETURN;
      }
		}
		break;

	case SEQ_ITEM_RETURN:
		//GFL_OVERLAY_Unload( FS_OVERLAY_ID(itemuse) );

		{
   		MMDLSYS *p_fldmdl = FIELDMAP_GetMMdlSys( p_wk->p_fieldmap );
      MMDLSYS_ClearPauseMoveProc( p_fldmdl );
		}
		return GMEVENT_RES_FINISH;


  case SEQ_RIBBON_ERROR:
		{	
			//MENU�j��
			if( ShortCutMenu_ExitSeq( p_wk ) )
      {
        //NG���b�Z�[�W�\��
        GMEVENT_CallEvent(p_event, EVENT_ItemuseNGMsgCall( p_wk->p_gamesys, 2 ) );

        *p_seq	= SEQ_RIBBON_RETURN;
      }
		}
    break;

  case SEQ_RIBBON_RETURN:
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
		SEQ_ITEM_ERROR,
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
			CALLTYPE	call_type;

			p_gdata	= GAMESYSTEM_GetGameData( p_wk->p_gamesys );
			p_sv		= GAMEDATA_GetSaveControlWork( p_gdata );
			cp_shortcut	= SaveData_GetShortCutConst( p_sv );
			shortcutID	= SHORTCUT_GetType( cp_shortcut, 0 );
			call_type   = ShortCutMenu_SetCallType( p_wk->p_link, shortcutID );

			{	// �N���`�F�b�N
				ITEMCHECK_ENABLE	enable;
				BOOL	err = FALSE;
				BOOL reverse_use, check_item;
				check_item = GetItemCheckEnable( shortcutID, &enable, &reverse_use );
				if(reverse_use == FALSE
				    && GAMEDATA_GetIntrudeReverseArea( GAMESYSTEM_GetGameData(p_wk->p_gamesys) ) == TRUE){
          err = TRUE;
        }
				else if(check_item == TRUE){
					ITEMCHECK_WORK	icwk;
					ITEMUSE_InitCheckWork( &icwk, p_wk->p_gamesys, p_wk->p_fieldmap );
					if( ITEMUSE_GetItemUseCheck( &icwk, enable ) == FALSE ){
						err = TRUE;
					}
				}
				// �N���G���[
				if( err == TRUE ){
					*p_seq = SEQ_ITEM_ERROR;
				// �N��
				}else{
					switch( call_type )
					{	
					case CALLTYPE_PROC:
						*p_seq	= SEQ_EVENT_CALL;
						break;
					case CALLTYPE_ITEM:
						*p_seq	= SEQ_ITEM_CALL;
						break;
					}
				}
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

      GFL_BG_SetVisible( FLDBG_MFRM_MSG, VISIBLE_OFF );
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

			// �A�C�e���R�[��
			p_item_event	= EVENT_FieldItemUse( item, p_wk->p_gamesys, p_wk->p_fieldmap );
			GMEVENT_CallEvent(p_event, p_item_event );

			*p_seq	= SEQ_ITEM_RETURN;
		}
		break;

	case SEQ_ITEM_ERROR:
		{	
			GMEVENT *p_item_event;
			u32 item;

			//����������܂��ɏ����󂯎��
			item	= p_wk->p_link->select_param;
			GFL_HEAP_FreeMemory( p_wk->p_link );
			p_wk->p_link	= NULL;

			// �A�C�e���R�[��
			if( item == EVENT_ITEMUSE_CALL_CYCLE ){
				PLAYER_WORK * plwk = GAMEDATA_GetMyPlayerWork( GAMESYSTEM_GetGameData(p_wk->p_gamesys) );
				if( PLAYERWORK_GetMoveForm( plwk ) == PLAYER_MOVE_FORM_CYCLE ){
					p_item_event = EVENT_ItemuseNGMsgCall( p_wk->p_gamesys, 1 );
				}else{
					p_item_event = EVENT_ItemuseNGMsgCall( p_wk->p_gamesys, 0 );
				}
			}else{
				p_item_event = EVENT_ItemuseNGMsgCall( p_wk->p_gamesys, 0 );
			}
			GMEVENT_CallEvent(p_event, p_item_event );

			*p_seq	= SEQ_ITEM_RETURN;
		}
		break;

	case SEQ_ITEM_RETURN:
		//GFL_OVERLAY_Unload( FS_OVERLAY_ID(itemuse) );
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
	case SHORTCUT_ID_BTLRECORDER:	//�o�g�����R�[�_�[
		p_param->call = EVENT_PROCLINK_CALL_BTLRECORDER;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_FRIENDNOTE:		//�F�B���
		p_param->call = EVENT_PROCLINK_CALL_WIFINOTE;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_TURIZAO:			//�肴��
		p_param->select_param	= EVENT_ITEMUSE_CALL_TURIZAO;
		return CALLTYPE_ITEM;
	case SHORTCUT_ID_DOWSINGMACHINE:		//�_�E�W���O�}�V��
		p_param->select_param	= EVENT_ITEMUSE_CALL_DOWSINGMACHINE;
		return CALLTYPE_ITEM;
	case SHORTCUT_ID_GURASHIDEA:		//�O���V�f�A�̉�
		p_param->select_param	= EVENT_PROCLINK_CALL_POKELIST;
		p_param->data	= PL_MODE_ITEMUSE;
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
  case SHORTCUT_ID_ZUKAN_LIST:				//�����񁄃��X�g
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		p_param->data	= ZUKAN_MODE_LIST;
		return CALLTYPE_PROC;
  case SHORTCUT_ID_ZUKAN_SEARCH:			//�����񁄌���
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		p_param->data	= ZUKAN_MODE_SEARCH;
		return CALLTYPE_PROC;
  case SHORTCUT_ID_ZUKAN_INFO:		  	//�����񁄏ڍׁE����
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		p_param->data	= ZUKAN_MODE_INFO;
		return CALLTYPE_PROC;
  case SHORTCUT_ID_ZUKAN_MAP:			//�����񁄏ڍׁE���z
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		p_param->data	= ZUKAN_MODE_MAP;
		return CALLTYPE_PROC;
  case SHORTCUT_ID_ZUKAN_VOICE:				//�����񁄏ڍׁE����
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		p_param->data	= ZUKAN_MODE_VOICE;
		return CALLTYPE_PROC;
  case SHORTCUT_ID_ZUKAN_FORM:				//�����񁄏ڍׁE�p
		p_param->call = EVENT_PROCLINK_CALL_ZUKAN;
		p_param->data	= ZUKAN_MODE_FORM;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_TRCARD_FRONT:			//�g���[�i�[�J�[�h������߂�
		p_param->call = EVENT_PROCLINK_CALL_TRAINERCARD;
    p_param->data = TRCARD_SHORTCUT_FRONT;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_TRCARD_BACK:			//�g���[�i�[�J�[�h������߂�
		p_param->call = EVENT_PROCLINK_CALL_TRAINERCARD;
    p_param->data = TRCARD_SHORTCUT_BACK;
		return CALLTYPE_PROC;
	case SHORTCUT_ID_TRCARD_BADGE:			//�g���[�i�[�J�[�h���o�b�W
		p_param->call = EVENT_PROCLINK_CALL_TRAINERCARD;
    p_param->data = TRCARD_SHORTCUT_BADGE;
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

//--------------------------------------------------------------
/**
 * 
 *
 * @param   shortcutID		
 * @param   enable		
 * @param   reverse_use		TRUE:���t�B�[���h�ł��g����@FALSE:���t�B�[���h�ł͎g���Ȃ�
 *
 * @retval  BOOL		TRUE:�A�C�e���g�p�`�F�b�N�̕K�v�L
 */
//--------------------------------------------------------------
static BOOL GetItemCheckEnable( SHORTCUT_ID shortcutID, ITEMCHECK_ENABLE * enable, BOOL *reverse_use )
{
  *reverse_use = FALSE;
  
	switch( shortcutID ){
	case SHORTCUT_ID_ZITENSYA:				// ���]��
		*enable = ITEMCHECK_CYCLE;
		return TRUE;

	case SHORTCUT_ID_TOWNMAP:					// �^�E���}�b�v
		*enable = ITEMCHECK_TOWNMAP;
		return TRUE;

	case SHORTCUT_ID_BTLRECORDER:			// �o�g�����R�[�_�[
		*enable = ITEMCHECK_BATTLE_RECORDER;
		return TRUE;

	case SHORTCUT_ID_FRIENDNOTE:			// �F�B���
		*enable = ITEMCHECK_WIFINOTE;
		return TRUE;

	case SHORTCUT_ID_TURIZAO:					// �肴��
		*enable = ITEMCHECK_TURIZAO;
		return TRUE;

	case SHORTCUT_ID_DOWSINGMACHINE:	// �_�E�W���O�}�V��
		*enable = ITEMCHECK_DOWSINGMACHINE;
		return TRUE;

	case SHORTCUT_ID_GURASHIDEA:			// �O���V�f�A�̉�
		return FALSE;
	}

  *reverse_use = TRUE;
	return FALSE;
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
	if( p_wk->p_menu == NULL && p_wk->is_empty == FALSE )
	{	
		//��u�p�V��̂œǂݑւ�����BG�ʂ�����
		GFL_BG_SetVisible( FLDBG_MFRM_MSG, VISIBLE_OFF );
		{	
			FLDMSGBG *p_msgbg;

			p_msgbg	= FIELDMAP_GetFldMsgBG( p_wk->p_fieldmap );
      if( p_msgbg )
      { 
        FLDMSGBG_ReleaseBGResouce( p_msgbg );
      }
		}

		{	
			GAMEDATA	*p_gdata	= GAMESYSTEM_GetGameData( p_wk->p_gamesys );
			SHORTCUT_CURSOR	*p_cursor	= GAMEDATA_GetShortCutCursor( p_gdata );

      NAGI_Printf( "�V���[�g�J�b�g���j���[�m�ۑO�@PROC=0x%x FIELD=0x%x CUTIN=0x%x\n",
			GFL_HEAP_GetHeapFreeSize( HEAPID_PROC ), GFL_HEAP_GetHeapFreeSize(HEAPID_FIELDMAP), GFL_HEAP_GetHeapFreeSize(HEAPID_FLD3DCUTIN));

			p_wk->p_menu	= SHORTCUTMENU_Init( p_gdata, mode, p_cursor, HEAPID_FLD3DCUTIN, HEAPID_FIELDMAP );
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
	if( p_wk->p_menu != NULL && p_wk->is_empty == FALSE )
	{
		//SHORTCUT��j�����AMSGBG���쐬
		FLDMSGBG *p_msgbg;

		SHORTCUTMENU_Exit( p_wk->p_menu );
		p_wk->p_menu	= NULL;

		p_msgbg	= FIELDMAP_GetFldMsgBG( p_wk->p_fieldmap );
		FLDMSGBG_ResetBGResource( p_msgbg );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	MENU��j�����AFLDMSGBG�����蓖��
 *	        ShortCutMenu_Exit���d�����߁A�t�B�[���h��Ŕj������Ƃ��͂����������
 *
 *	@param	EVENT_SHORTCUTMENU_WORK *p_wk ���[�N
 *
 *	@return TRUE�����I��  FALSE������
 */
//-----------------------------------------------------------------------------
static BOOL ShortCutMenu_ExitSeq( EVENT_SHORTCUTMENU_WORK *p_wk )
{
  enum
  {
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_EXIT,
  };

	if( p_wk->is_empty == FALSE )
	{
    switch( p_wk->exit_seq )
    {
    case SEQ_INIT:
      if( p_wk->p_menu )
      {
        GFL_BG_SetVisible( FLDBG_MFRM_MSG, VISIBLE_OFF );
        SHORTCUTMENU_Exit( p_wk->p_menu );
        p_wk->p_menu	= NULL;
        p_wk->exit_seq  = SEQ_MAIN;
      }
      else
      {
        p_wk->exit_seq  = SEQ_EXIT;
      }
      break;

    case SEQ_MAIN:
      {
        FLDMSGBG *p_msgbg	= FIELDMAP_GetFldMsgBG( p_wk->p_fieldmap );
        FLDMSGBG_ResetBGResource( p_msgbg );
      }
      p_wk->exit_seq  = SEQ_EXIT;
      break;

    case SEQ_EXIT:
      p_wk->exit_seq  = 0;
      return TRUE;
    }

    return FALSE;
  }
  else
  {
    return TRUE;
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

	GAMEDATA					*p_gdata;
	SAVE_CONTROL_WORK	*p_sv;
	const SHORTCUT		*cp_shortcut;

	p_gdata	= GAMESYSTEM_GetGameData( p_wk->p_gamesys );
	p_sv		= GAMEDATA_GetSaveControlWork( p_gdata );
	cp_shortcut	= SaveData_GetShortCutConst( p_sv );

	if( SHORTCUT_GetMax( cp_shortcut ) == 0 )
	{	
		p_wk->is_empty		=	TRUE;
	}
	else
	{	
		//�t�B�[���h�ɖ߂�ꍇ
		if( param->result == EVENT_PROCLINK_RESULT_EXIT ||
				param->result == EVENT_PROCLINK_RESULT_ITEM ||
				param->result == EVENT_PROCLINK_RESULT_SKILL )
		{	
			/* �Ȃɂ����Ȃ� */
		}
		else
		{	
			//���j���[�ɖ߂�ꍇ
			ShortCutMenu_Init( SHORTCUTMENU_MODE_STAY, p_wk );
		}
	}
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

/*
 *  @brief  �V���[�g�J�b�g�A�C�e���g�pNG���b�Z�[�W�C�x���g
 *
 *  @param  type    0:�ʏ�,1:���]�Ԃ��~����Ȃ�,2���{����ʂ��J���Ȃ�
 */
static GMEVENT* EVENT_ItemuseNGMsgCall(GAMESYS_WORK * gsys, u8 type )
{
  GMEVENT* event;
  SCRIPT_WORK* sc;

  event = SCRIPT_SetEventScript( gsys, SCRID_ITEMUSE_NG_MSG, NULL, HEAPID_FIELDMAP );
  sc = SCRIPT_GetScriptWorkFromEvent( event );
  SCRIPT_SetScriptWorkParam( sc, type, 0, 0, 0 );
  
  return event;
}

