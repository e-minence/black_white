//==============================================================================
/**
 * @file    intrude_secret_item.c
 * @brief   �N���B���A�C�e��
 * @author  matsuda
 * @date    2010.02.01(��)
 */
//==============================================================================
#include <gflib.h>
#include "field/intrude_secret_item.h"
#include "fieldmap/zone_id.h"
#include "gamesystem/game_comm.h"
#include "field\intrude_common.h"
#include "field_comm/intrude_work.h"
#include "field/intrude_secret_item_def.h"


//==============================================================================
//  �O���f�[�^
//==============================================================================
#include "intrude_secret_item.cdat"


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �B���A�C�e���z�u�e�[�u���ɍڂ��Ă�����W���`�F�b�N
 *
 * @param   zone_id		�]�[��ID
 * @param   grid_x		�O���b�h���WX
 * @param   grid_y		�O���b�h���WY
 * @param   grid_z		�O���b�h���WZ
 *
 * @retval  int		    �q�b�g�����ꍇ�F�e�[�u���ʒu
 * @retval  int		    �q�b�g���Ȃ������ꍇ�FISC_SEARCH_NONE
 */
//==================================================================
int ISC_TblSearch(u16 zone_id, u16 grid_x, u16 grid_y, u16 grid_z)
{
  int i;
  const INTRUDE_SECRET_ITEM_POSDATA *postbl = IntrudeSecretItemPosDataTbl;
  
  for(i = 0; i < SECRET_ITEM_DATA_TBL_MAX; i++){
    if(postbl->zone_id == zone_id && postbl->grid_x == grid_x && postbl->grid_y == grid_y
        && postbl->grid_z == grid_z){
      return i;
    }
    postbl++;
  }
  return ISC_SEARCH_NONE;
}

