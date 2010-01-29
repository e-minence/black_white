/*
 *  @file   beacon_status.c
 *  @brief  �Q�[�����ێ�����r�[�R�����O�Ǘ�
 *  @author Miyuki Iwasawa
 *  @date   10.01.21
 */

#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_types.h"
#include "gamesystem/beacon_status.h"

struct _TAG_BEACON_STATUS{
  u8  view_top_ofs; ///<�r���[���X�g�̃g�b�v�̃f�[�^�I�t�Z�b�g
  GAMEBEACON_INFO_TBL view_log; 
};

/*
 *  @brief  �r�[�R���X�e�[�^�X���[�N����
 */
BEACON_STATUS* BEACON_STATUS_Create( HEAPID heapID )
{
  BEACON_STATUS* wk;

  wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BEACON_STATUS));

  return wk;
}

/*
 *  @brief  �r�[�R���X�e�[�^�X���[�N�j��
 */
void BEACON_STATUS_Delete( BEACON_STATUS* wk )
{
  MI_CpuClear8( wk, sizeof(BEACON_STATUS));
  GFL_HEAP_FreeMemory( wk );
}

/*
 *  @brief  InfoTbl���擾
 */
GAMEBEACON_INFO_TBL* BEACON_STATUS_GetInfoTbl( BEACON_STATUS* wk )
{
  return &wk->view_log;
}

/*
 *  @brief  �r���[���X�g�̃g�b�v�I�t�Z�b�g���擾
 */
u8  BEACON_STATUS_GetViewTopOffset( BEACON_STATUS* wk )
{
  return wk->view_top_ofs;
}

/*
 *  @brief  �r���[���X�g�̃g�b�v�I�t�Z�b�g���Z�b�g
 */
void BEACON_STATUS_SetViewTopOffset( BEACON_STATUS* wk, u8 ofs )
{
  wk->view_top_ofs = ofs;
}


