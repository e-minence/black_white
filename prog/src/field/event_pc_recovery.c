////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �|�P�Z���񕜃A�j���[�V�����C�x���g
 * @file   event_pc_recovery.c
 * @author obata_toshihiro
 * @date   2009.09.18
 */
//////////////////////////////////////////////////////////////////////////////// 
#include "include/gamesystem/game_event.h"
#include "event_pc_recovery.h"


//==============================================================================
// �C�x���g���[�N
//==============================================================================
typedef struct
{
  int temp;
} RECOVERY_WORK;


//==============================================================================
// �v���g�^�C�v�錾
//==============================================================================
GMEVENT_RESULT EVENT_FUNC_PcRecoveryAnime( GMEVENT* event, int* seq, void* work );


//------------------------------------------------------------------------------
/**
 * @brief �|�P�Z���񕜃A�j���[�V�����C�x���g���쐬����
 *
 * @param gsys   �Q�[���V�X�e��
 * @param parent �e�C�x���g
 *
 * @return �쐬�����񕜃A�j���C�x���g
 */
//------------------------------------------------------------------------------
GMEVENT* EVENT_PcRecoveryAnime( GAMESYS_WORK* gsys, GMEVENT* parent )
{
  GMEVENT* event;
  RECOVERY_WORK* work;

  // �C�x���g�쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_PcRecoveryAnime, sizeof(RECOVERY_WORK) );

  // �C�x���g���[�N������
  work = (RECOVERY_WORK*)GMEVENT_GetEventWork( event );

  return event;
}


//==============================================================================
/**
 * @brief �C�x���g����֐�
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X
 * @param work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//==============================================================================
GMEVENT_RESULT EVENT_FUNC_PcRecoveryAnime( GMEVENT* event, int* seq, void* work )
{
  return GMEVENT_RES_CONTINUE;
}
