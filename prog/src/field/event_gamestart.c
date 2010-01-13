//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_gamestart.c
 *	@brief  �C�x���g�@�Q�[���J�n
 *	@author	tomoya takahashi
 *	@date		2010.01.12
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "event_gamestart.h"

#include "event_mapchange.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

// OVERLAY FIELD_INIT
FS_EXTERN_OVERLAY(field_init);


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�Q�[���I���҂��C�x���g
//=====================================
typedef struct {
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
}GAME_END_WORK;
//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static GMEVENT_RESULT GameEndEvent(GMEVENT * event, int *seq, void *work);


//------------------------------------------------------------------
/**
 * @brief �Q�[���J�n�C�x���g
 */
//------------------------------------------------------------------
GMEVENT * EVENT_GameStart(GAMESYS_WORK * gsys, GAME_INIT_WORK * game_init_work)
{
  // �t�B�[���h�������p�I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(field_init) );
  return EVENT_CallGameStart( gsys, game_init_work );
}


//------------------------------------------------------------------
/**
 * @brief �f�o�b�O�p�F�Q�[���I��
 */
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_GameEnd( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap)
{
  GMEVENT * event = GMEVENT_Create(gsys, NULL, GameEndEvent, sizeof(GAME_END_WORK));
  GAME_END_WORK * gew = GMEVENT_GetEventWork(event);

  // �Q�[���I���C�x���g����
  gew->gsys = gsys;
  gew->fieldmap = fieldmap;
  return event;
}


//--------------------------------------------------------------
/**
 * @brief �S�Ŏ��̃}�b�v�J�ڏ����i�t�B�[���h�񐶐����j
 */
//--------------------------------------------------------------
GMEVENT * EVENT_GameOver( GAMESYS_WORK * gsys )
{
  // �t�B�[���h�������p�I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(field_init) );
  return EVENT_CallGameOver( gsys );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�o�[���C�̓ǂݍ���
 */
//-----------------------------------------------------------------------------
void GAMESTART_OVERLAY_FIELD_INIT_Load( void )
{
  // �t�B�[���h�������p�I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(field_init) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�[�o�[���C�̔j��
 */
//-----------------------------------------------------------------------------
void GAMESTART_OVERLAY_FIELD_INIT_UnLoad( void )
{
  // �t�B�[���h�������p�I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(field_init) );
}





//-----------------------------------------------------------------------------
/**
 *    private�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�I���҂��C�x���g
 *
 *	@retval GMEVENT_RES_CONTINUE = 0, ///<�C�x���g�p����
 *	@retval GMEVENT_RES_FINISH,       ///<�C�x���g�I��
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GameEndEvent(GMEVENT * event, int *seq, void *work)
{
  GAME_END_WORK * gew = work;
  
  switch (*seq) {
  case 0:
    //�I���C�x���g���R�[��
    {
      GMEVENT* end_event;
      end_event = DEBUG_EVENT_CallGameEnd(gew->gsys, gew->fieldmap);
      GMEVENT_CallEvent(event, end_event);
    }
    (*seq)++;
    break;
  case 1:
    //�@�I�[�o�[���C�̃A�����[�h
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(field_init) );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

