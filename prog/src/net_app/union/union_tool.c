//==============================================================================
/**
 * @file    union_tool.c
 * @brief   ���j�I�����[���Ŏg�p����c�[���ށF�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2009.07.25(�y)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "field\field_player.h"
#include "field\fieldmap.h"
#include "union_types.h"
#include "union_tool.h"

//==============================================================================
//  �\���̒�`
//==============================================================================
///s16�̃|�C���g�^
typedef struct{
  s16 x;
  s16 z;
}POINT_S16;

//==============================================================================
//  �f�[�^
//==============================================================================
//--------------------------------------------------------------
//  �o�����W
//--------------------------------------------------------------
///1vs1�̎��̗����ʒu���W(�O���b�h���W)
static const POINT_S16 UnionWayOutPos[] = {
  {11, 16},
};


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ���j�I���̏o�����W�ɂ��邩�`�F�b�N
 *
 * @param   fieldWork		
 *
 * @retval  BOOL		TRUE:�o�����W�ɂ���
 */
//==================================================================
BOOL UnionTool_CheckWayOut(FIELDMAP_WORK *fieldWork)
{
  VecFx32 pos;
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork(fieldWork);
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork(gsys);
  int i;
  
  FIELD_PLAYER_GetPos(player, &pos);
  pos.x = SIZE_GRID_FX32(pos.x);
  pos.z = SIZE_GRID_FX32(pos.z);

  if(PLAYERWORK_getDirection( plWork ) == DIR_UP){
//    return FALSE;
  }
  
  for(i = 0; i < NELEMS(UnionWayOutPos); i++){
    if(pos.x == UnionWayOutPos[i].x && pos.z == UnionWayOutPos[i].z){
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * play_category���A�s�[���ԍ��ɕϊ�
 *
 * @param   situ		
 *
 * @retval  UNION_APPEAL		
 */
//==================================================================
UNION_APPEAL UnionTool_PlayCategory_to_AppealNo(UNION_PLAY_CATEGORY play_category)
{
  //�V��ł���Q�[��������Ȃ�A�������D�悵�ăA�s�[��������
  if(play_category >= UNION_PLAY_CATEGORY_COLOSSEUM_START && 
      play_category <= UNION_PLAY_CATEGORY_COLOSSEUM_END){
    return UNION_APPEAL_BATTLE;
  }
  else{
    switch(play_category){
    case UNION_PLAY_CATEGORY_PICTURE:        //���G����
      return UNION_APPEAL_PICTURE;
    case UNION_PLAY_CATEGORY_TRADE:          //�|�P��������
      return UNION_APPEAL_TRADE;
    case UNION_PLAY_CATEGORY_GURUGURU:       //���邮�����
      return UNION_APPEAL_GURUGURU;
    case UNION_PLAY_CATEGORY_RECORD:         //���R�[�h�R�[�i�[
      return UNION_APPEAL_RECORD;
    }
  }
  
  return UNION_APPEAL_NULL;
}
