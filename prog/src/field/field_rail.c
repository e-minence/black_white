//============================================================================================
/**
 * @file  field_rail.c
 * @brief �m�[�O���b�h�ł̎��@�E�J��������\��
 * @author  tamada, tomoya
 * @date  2009.05.18
 *
 */
//============================================================================================

#include <gflib.h>

#include "field_rail.h"


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
struct _FIELD_RAIL_MAN{
  HEAPID heapID;
  const FIELD_RAIL * railData;

  const FIELD_RAIL nowRail;
  u32 line_ofs;     ///<LINE�ɂ���Ԃ́ALINE��ł̃I�t�Z�b�g�ʒu

  //������K�v�ɂȂ邪���l����ƍ�������
  //u32 width_ofs;    ///<LINE�ɂ���Ԃ̕��̓����I�t�Z�b�g�ʒu

};

//------------------------------------------------------------------
//------------------------------------------------------------------
//typedef struct {
//}NOGRID_POS;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------


static RAIL_KEY keyContToRailKey(int cont);
static RAIL_KEY getReverseKey(RAIL_KEY key);

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ���[������V�X�e���̐���
 */
//------------------------------------------------------------------
FIELD_RAIL_MAN * FIELD_RAIL_MAN_Create(HEAPID heapID)
{ 
  FIELD_RAIL_MAN * man = GFL_HEAP_AllocMemory(heapID, sizeof(FIELD_RAIL_MAN));

  man->heapID = heapID;
  man->railData = NULL;

  return man;
}

//------------------------------------------------------------------
/**
 * @brief ���[������V�X�e���̍폜
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Delete(FIELD_RAIL_MAN * man)
{ 
  GFL_HEAP_FreeMemory(man);
}

//------------------------------------------------------------------
/**
 * @brief ���[���f�[�^�̓ǂݍ���
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Load(FIELD_RAIL_MAN * man, const FIELD_RAIL * railData)
{ 
  man->railData = railData;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Update(FIELD_RAIL_MAN * man, int key_cont)
{ 
  RAIL_KEY key = keyContToRailKey(key_cont);

  if (man->railData == NULL)
  {
    return;
  }

  if(man->nowRail.type == FIELD_RAIL_TYPE_POINT)
  {
    int i;
    const RAIL_POINT * nPoint = &man->nowRail.point;
    for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
    {
      if (nPoint->keys[i] == key)
      {
        // POINT --> LINE[i] �ւ̈ڍs����
      }
    }
    
  }
  else if(man->nowRail.type == FIELD_RAIL_TYPE_LINE)
  {
    const RAIL_LINE * nLine = &man->nowRail.line;
    if (key == nLine->key)
    {
      man->line_ofs ++;
      if (man->line_ofs == nLine->line_divider)
      {
        // LINE --> POINT[1] �ւ̈ڍs����
      }
    }
    else if (key == getReverseKey(nLine->key))
    {
      man->line_ofs --;
      if (man->line_ofs == 0)
      {
        // LINE --> POINT[0] �ւ̈ڍs����
      }
    }
  }
}


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Get3DPos(const FIELD_RAIL_MAN * man, VecFx32 * pos)
{
  if(man->nowRail.type == FIELD_RAIL_TYPE_POINT)
  {
    //POINT�ɂ���Ƃ���POINT�̍��W
    *pos = man->nowRail.point.pos;
  }
  if(man->nowRail.type == FIELD_RAIL_TYPE_LINE)
  {
    //LINE�ɂ���Ƃ���POINT��POINT�̊Ԃ̍��W
    //pos = p0 + (p1 - p0) * (ofs / div )
    const RAIL_LINE * nLine = &man->nowRail.line;
    fx32 ofs = (man->line_ofs * FX32_ONE) / nLine->line_divider;
    VecFx32 val;
    const VecFx32 * p0 = &nLine->point[0]->pos;
    const VecFx32 * p1 = &nLine->point[1]->pos;
    VEC_Subtract(p1, p0, &val);
    VEC_MultAdd(ofs, &val, p0, pos);
  }
}


//============================================================================================
/**
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
static RAIL_KEY keyContToRailKey(int cont)
{
  if (cont & PAD_KEY_UP) return RAIL_KEY_UP;
  if (cont & PAD_KEY_LEFT) return RAIL_KEY_LEFT;
  if (cont & PAD_KEY_RIGHT) return RAIL_KEY_RIGHT;
  if (cont & PAD_KEY_DOWN) return RAIL_KEY_DOWN;
  return RAIL_KEY_NULL;
}

static RAIL_KEY getReverseKey(RAIL_KEY key)
{
  switch (key) {
  case RAIL_KEY_UP: 
    return RAIL_KEY_DOWN;
  case RAIL_KEY_DOWN:
    return RAIL_KEY_UP;
  case RAIL_KEY_LEFT:
    return RAIL_KEY_RIGHT;
  case RAIL_KEY_RIGHT:
    return RAIL_KEY_LEFT;
  }
  return RAIL_KEY_NULL;
}





