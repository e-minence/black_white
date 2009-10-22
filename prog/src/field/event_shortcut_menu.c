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

//�t�B�[���h
#include "field/fieldmap.h"
#include "field/field_msgbg.h"

//�A�[�J�C�u
#include "arc_def.h"

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
	GMEVENT						*p_event;		//�����̃C�x���g
	GAMESYS_WORK			*p_gamesys;	//�Q�[���V�X�e��
	FIELDMAP_WORK			*p_fieldmap;//�t�B�[���h���[�N
	SHORTCUTMENU_WORK *p_menu;		//�V���[�g�J�b�g���j���[
	HEAPID						heapID;			//�q�[�vID
} SHORCUTMENU_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
 */
//=============================================================================
static GMEVENT_RESULT ShortCutMenu_MainEvent( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
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
  SHORCUTMENU_WORK *p_wk;
  GMEVENT *p_event;
  
	//�C�x���g�쐬
  p_event = GMEVENT_Create(
    p_gamesys, NULL, ShortCutMenu_MainEvent, sizeof(SHORCUTMENU_WORK));
  
	//���[�N�擾
  p_wk = GMEVENT_GetEventWork(p_event);
  GFL_STD_MemClear( p_wk, sizeof(SHORCUTMENU_WORK) );
  
	//���[�N������
  p_wk->p_gamesys		= p_gamesys;
  p_wk->p_event			= p_event;
  p_wk->p_fieldmap	= p_fieldmap;
  p_wk->heapID = heapID;
  
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
	};

	SHORCUTMENU_WORK	*p_wk	= p_wk_adrs;

	switch( *p_seq )
	{	
	case SEQ_INIT:
		{	//MSGBG��j����SHORTCUTMENU���쐬
			FLDMSGBG *p_msgbg;
			p_msgbg	= FIELDMAP_GetFldMsgBG( p_wk->p_fieldmap );
			FLDMSGBG_ReleaseBGResouce( p_msgbg );

			p_wk->p_menu	= SHORTCUTMENU_Init( p_wk->heapID );
		}
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
			if( input != SHORTCUTMENU_INPUT_NONE )
			{	
				//@todoPROC�ړ��⏈��
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
		{	//SHORTCUT��j�����AMSGBG���쐬
			FLDMSGBG *p_msgbg;
			SHORTCUTMENU_Exit( p_wk->p_menu );

			p_msgbg	= FIELDMAP_GetFldMsgBG( p_wk->p_fieldmap );
			FLDMSGBG_ResetBGResource( p_msgbg );

			return GMEVENT_RES_FINISH;
		}
		break;
	}

  return GMEVENT_RES_CONTINUE;
}
