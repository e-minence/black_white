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

//�t�B�[���h
#include "field/fieldmap.h"

//�A�[�J�C�u
#include "arc_def.h"

//�O���Q��
#include "event_shortcut_menu.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

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
	GMEVENT				*p_event;		//�����̃C�x���g
	GAMESYS_WORK	*p_gamesys;	//�Q�[���V�X�e��
	FIELDMAP_WORK	*p_fieldmap;//�t�B�[���h���[�N
	HEAPID				heapID;			//�q�[�vID
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
  return GMEVENT_RES_FINISH;
}
