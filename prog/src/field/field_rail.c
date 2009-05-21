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

#include "field/fieldmap_proc.h"

#include "field_rail.h"

#include "field_camera.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------


//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
typedef enum {  
  FIELD_RAIL_TYPE_POINT = 0,
  FIELD_RAIL_TYPE_LINE,

  FIELD_RAIL_TYPE_MAX
}FIELD_RAIL_TYPE;


//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
typedef struct {  
  FIELD_RAIL_TYPE type;
  union { 
    void * dummy;
    const RAIL_POINT * point;
    const RAIL_LINE * line;
  };
}FIELD_RAIL;


//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
struct _FIELD_RAIL_MAN{
  HEAPID heapID;

  FIELD_CAMERA * field_camera;

  BOOL active_flag;

  /// LINE�ɂ���Ԃ́ALINE��ł̃I�t�Z�b�g�ʒu
  u32 line_ofs;

  /// �ړ����N�����ŐV�̃L�[�o�����[
  RAIL_KEY last_active_key;

  FIELD_RAIL nowRail;

  //������K�v�ɂȂ邪���l����ƍ�������
  //u32 width_ofs;    ///<LINE�ɂ���Ԃ̕��̓����I�t�Z�b�g�ʒu

};

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY keyContToRailKey(int cont);
static RAIL_KEY getReverseKey(RAIL_KEY key);

//------------------------------------------------------------------
//------------------------------------------------------------------
static void initRail(FIELD_RAIL * rail);
static BOOL isValidRail(const FIELD_RAIL * rail);
static const char * getRailName(const FIELD_RAIL * rail);
static BOOL checkLine(const RAIL_LINE * line);
static BOOL checkPoint(const RAIL_POINT * point);


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ���[������V�X�e���̐���
 */
//------------------------------------------------------------------
FIELD_RAIL_MAN * FIELD_RAIL_MAN_Create(HEAPID heapID, FIELD_CAMERA * camera)
{ 
  FIELD_RAIL_MAN * man = GFL_HEAP_AllocMemory(heapID, sizeof(FIELD_RAIL_MAN));

  man->heapID = heapID;
  man->active_flag = FALSE;
  man->field_camera = camera;
  man->line_ofs = 0;
  man->last_active_key = RAIL_KEY_NULL;
  initRail(&man->nowRail);

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
 *
 * �Ƃ肠�����A�ŏ���POINT����n�܂�Ɖ��肷��
 *
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Load(FIELD_RAIL_MAN * man, const RAIL_POINT * railPointData)
{ 
  FIELD_RAIL * rail = &man->nowRail;
  rail->type = FIELD_RAIL_TYPE_POINT;
  rail->point = railPointData;
  man->active_flag = TRUE;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_SetActiveFlag(FIELD_RAIL_MAN * man, BOOL flag)
{
  man->active_flag = flag;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
BOOL FIELD_RAIL_MAN_GetActiveFlag(const FIELD_RAIL_MAN *man)
{
  return man->active_flag;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_Update(FIELD_RAIL_MAN * man, int key_cont)
{ 
  FIELD_RAIL_TYPE type = man->nowRail.type;
  RAIL_KEY set_key = RAIL_KEY_NULL;
  RAIL_KEY before_key = man->last_active_key;
  RAIL_KEY key = keyContToRailKey(key_cont);

  if (!man->active_flag)
  {
    return;
  }

  if (isValidRail(&man->nowRail) == FALSE)
  {
    return;
  }

  if (key == RAIL_KEY_NULL)
  {
    return;
  }

  if(man->nowRail.type == FIELD_RAIL_TYPE_POINT)
  { //POINT��̂Ƃ��̈ړ�����
    int i;
    const RAIL_POINT * nPoint = man->nowRail.point;
    for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
    {
      if (nPoint->keys[i] == key)
      { // POINT --> LINE[i] �ւ̈ڍs����
        const RAIL_LINE * nLine = nPoint->lines[i];
        if (nLine->point_s == nPoint)
        { 
          man->line_ofs = 1;  //�ŏ��͂P����I
        }
        else if (nLine->point_e == nPoint)
        {
          man->line_ofs = nLine->line_divider - 1; //�Ō�[1����I
        }
        //�Ȃ�����LINE������RAIL�Ƃ��ăZ�b�g
        man->nowRail.type = FIELD_RAIL_TYPE_LINE;
        man->nowRail.line = nLine;
        set_key = key;
        checkLine(man->nowRail.line);
        
      }
    }
  }
  else if(man->nowRail.type == FIELD_RAIL_TYPE_LINE)
  { //LINE��̂Ƃ��̈ړ�����
    const RAIL_LINE * nLine = man->nowRail.line;
    if (key == nLine->key)
    {
      set_key = key;
      man->line_ofs ++;
      if (man->line_ofs == nLine->line_divider)
      { // LINE --> point_e �ւ̈ڍs����
        man->nowRail.type = FIELD_RAIL_TYPE_POINT;
        man->nowRail.point = nLine->point_e;
        checkPoint(man->nowRail.point);
      }
    }
    else if (key == getReverseKey(nLine->key))
    {
      set_key = key;
      man->line_ofs --;
      if (man->line_ofs == 0)
      { // LINE --> POINT_S �ւ̈ڍs����
        man->nowRail.type = FIELD_RAIL_TYPE_POINT;
        man->nowRail.point = nLine->point_s;
        checkPoint(man->nowRail.point);
      }
    }
  }

  if (set_key != RAIL_KEY_NULL)
  {
    static const char * const names[] = {
      "RAIL_KEY_NULL","RAIL_KEY_UP","RAIL_KEY_RIGHT","RAIL_KEY_DOWN","RAIL_KEY_LEFT"
    };
    OS_TPrintf("RAIL:%s :ofs=%d\n", names[set_key], man->line_ofs);
    man->last_active_key = set_key;
  }

  if (type != man->nowRail.type)
  {
    OS_TPrintf("RAIL:change to %d(%s:%08x)\n",
        man->nowRail.type,
        getRailName(&man->nowRail),
        man->nowRail.dummy);
  }
}


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_GetPos(const FIELD_RAIL_MAN * man, VecFx32 * pos)
{
  RAIL_POS_FUNC * func = NULL;

  if(man->nowRail.type == FIELD_RAIL_TYPE_POINT)
  {
    //POINT�ɂ���Ƃ���POINT�̍��W
    *pos = man->nowRail.point->pos;
  }
  else if(man->nowRail.type == FIELD_RAIL_TYPE_LINE)
  {
    func = man->nowRail.line->line_pos_set->func;
    if (func == NULL)
    {
      func = FIELD_RAIL_POSFUNC_StraitLine;
    }
    func(man, pos);
  }
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void FIELD_RAIL_MAN_UpdateCamera(const FIELD_RAIL_MAN * man)
{
  const FIELD_RAIL * rail = &man->nowRail;
  RAIL_CAMERA_FUNC * func = NULL;
  switch (rail->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    if (rail->point->camera_set)
    {
      func = rail->point->camera_set->func;
    }
    break;
  case FIELD_RAIL_TYPE_LINE:
    if (rail->line->camera_set)
    {
      func = rail->line->camera_set->func;
    }
    break;
  default:
    GF_ASSERT(0); //NO TYPE;
  }
  if (func)
  {
    func(man);
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_RAIL_MAN_GetDirection(const FIELD_RAIL_MAN * man, VecFx32 * dir)
{
  const FIELD_RAIL * rail = &man->nowRail;
  switch (rail->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    break;
  case FIELD_RAIL_TYPE_LINE:
    break;
  default:
    break;
  }
}

//============================================================================================
/**
 */
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void initRail(FIELD_RAIL * rail)
{
  rail->type = FIELD_RAIL_TYPE_MAX;
  rail->dummy = NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL isValidRail(const FIELD_RAIL * rail)
{
  if (rail->type < FIELD_RAIL_TYPE_MAX && rail->dummy != NULL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static const RAIL_CAMERA_SET * getCameraSet(const FIELD_RAIL * rail)
{
  switch (rail->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    return rail->point->camera_set;
    break;
  case FIELD_RAIL_TYPE_LINE:
    return rail->line->camera_set;
    break;
  }
  GF_ASSERT(0);
  return &RAIL_CAMERA_SET_Default;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static const char * getRailName(const FIELD_RAIL * rail)
{
  switch (rail->type)
  {
  case FIELD_RAIL_TYPE_POINT:
    return rail->point->name;
  case FIELD_RAIL_TYPE_LINE:
    return rail->line->name;
  }
  return "NO ENTRY";
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static RAIL_KEY searchLineInPoint(const RAIL_POINT *point, const RAIL_LINE * line)
{
  int i;
  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
    if (point->lines[i] == line) return point->keys[i];
  }
  return RAIL_KEY_NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL checkLine(const RAIL_LINE * line)
{

  RAIL_KEY key;

  if(line->point_s == NULL || line->point_e == NULL)
  {
    OS_Printf("%s��POINT������������܂���\n", line->name);
    return FALSE;
  }
  key = searchLineInPoint(line->point_s, line);
  if (key == RAIL_KEY_NULL)
  {
    OS_Printf("%s��%s�Ɛ������ڑ��ł��Ă��܂���\n", line->name, line->point_s->name);
    return FALSE;
  }
  if (key != line->key)
  {
    OS_Printf("%s�͎n�[%s�ƃL�[�������Ă��܂���\n", line->name, line->point_s->name);
    return FALSE;
  }
  key = searchLineInPoint(line->point_e, line);
  if (key == RAIL_KEY_NULL)
  {
    OS_Printf("%s��%s�Ɛ������ڑ��ł��Ă��܂���\n", line->name, line->point_e->name);
    return FALSE;
  }
  if (key != getReverseKey(line->key))
  {
    OS_Printf("%s�͏I�[%s�ƃL�[�������Ă��܂���\n", line->name, line->point_e->name);
    return FALSE;
  }
  return TRUE;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL checkPoint(const RAIL_POINT * point)
{
  int i;
  for (i = 0; i < RAIL_CONNECT_LINE_MAX; i++)
  {
    if (point->keys[i] == RAIL_KEY_NULL)
    {
      if (point->lines[i] == NULL)
      {
        return TRUE;
      }
      OS_Printf("%s:LINE��RAIL_KEY�̎w��ɖ���������܂�\n", point->name);
    }
    if (point->lines[i]->point_s != point && point->lines[i]->point_e != point)
    {
      OS_Printf("%s��%s�Ɛڑ����Ă��܂���\n",point->name, point->lines[i]->name);
      return FALSE;
    }
  }
  return TRUE;
}

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

//------------------------------------------------------------------
//------------------------------------------------------------------
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

//============================================================================================
//
//
//
//    ���[���\�����������邽�߂̃c�[��
//
//
//
//============================================================================================

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_RAIL_POSFUNC_StraitLine(const FIELD_RAIL_MAN * man, VecFx32 * pos)
{
    //LINE�ɂ���Ƃ���POINT��POINT�̊Ԃ̍��W
    //pos = p0 + (p1 - p0) * (ofs / div )
    const RAIL_LINE * nLine = man->nowRail.line;
    fx32 ofs = (man->line_ofs * FX32_ONE) / nLine->line_divider;
    VecFx32 val;
    const VecFx32 * p0 = &nLine->point_s->pos;
    const VecFx32 * p1 = &nLine->point_e->pos;
    VEC_Subtract(p1, p0, &val);
    VEC_MultAdd(ofs, &val, p0, pos);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const RAIL_CAMERA_SET RAIL_CAMERA_SET_Default =
{
  NULL,
  0,
  0,
  0,
  0,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
const RAIL_LINEPOS_SET RAIL_LINEPOS_SET_Default =
{
  NULL,
  0,
  0,
  0,
  0,
};

//------------------------------------------------------------------
/**
 * @brief �J�����w��F�p�����[�^��Fx32�R�Ƃ��ĒP���ɑ�������
 */
//------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_FixPosCamera(const FIELD_RAIL_MAN* man)
{
  VecFx32 target;
  const RAIL_CAMERA_SET * cam_set = getCameraSet(&man->nowRail);
  FIELD_RAIL_MAN_GetPos(man, &target);

  target.x += cam_set->param0;
  target.y += cam_set->param1;
  target.z += cam_set->param2;
  //TODO: FIELD_CAMERA�ɒ��ڃJ�����ʒu�w���������̂�ǉ�����
  //*pos = target;
}

//------------------------------------------------------------------
/**
 * @brief �J�����w��F�p�����[�^��Angle�w��Ɏg�p����
 */
//------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_FixAngleCamera(const FIELD_RAIL_MAN* man)
{
  FIELD_CAMERA * cam = man->field_camera;
  const RAIL_CAMERA_SET * cam_set = getCameraSet(&man->nowRail);
  FIELD_CAMERA_SetAnglePitch(cam, (u16)cam_set->param0);
  FIELD_CAMERA_SetAngleYaw(cam, (u16)cam_set->param1);
  FIELD_CAMERA_SetAngleLen(cam, (fx32)cam_set->param2);
}

//------------------------------------------------------------------
/**
 * @brief �J�����w��F�ێ�������POINT��angle�o�����[��⊮����
 */
//------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_OfsAngleCamera(const FIELD_RAIL_MAN* man)
{
  const RAIL_CAMERA_SET * c0;
  const RAIL_CAMERA_SET * c1;
  GF_ASSERT(man->nowRail.type == FIELD_RAIL_TYPE_LINE);

  c0 = man->nowRail.line->point_s->camera_set;
  GF_ASSERT(c0->func == FIELD_RAIL_CAMERAFUNC_FixAngleCamera);
  c1 = man->nowRail.line->point_e->camera_set;
  GF_ASSERT(c1->func == FIELD_RAIL_CAMERAFUNC_FixAngleCamera);
  {
    u32 div = man->nowRail.line->line_divider;
    u16 pitch = c0->param0 + ( ((int)c1->param0 - (int)c0->param0) * man->line_ofs / div );
    /** TODO yaw�͑����Z�ł̓o�O��I�I�I */
    u16 yaw = c0->param1 + ( ((int)c1->param1 - (int)c0->param1) * man->line_ofs / div );
    fx32 len = c0->param2 + ( (c1->param2 - c0->param2) * man->line_ofs / div );
    FIELD_CAMERA * cam = man->field_camera;
    FIELD_CAMERA_SetAnglePitch(cam, pitch);
    FIELD_CAMERA_SetAngleYaw(cam, yaw);
    FIELD_CAMERA_SetAngleLen(cam, len);
  }
}







//----------------------------------------------------------------------------
/**
 *	@brief  �~�ړ�
 *
 *	@param	man     ���[���}�l�[�W��
 *	@param	pos     �ʒu�i�[��
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_POSFUNC_CircleLine( const FIELD_RAIL_MAN * man, VecFx32 * pos )
{
  const RAIL_LINE * nLine = man->nowRail.line;
  fx32 ofs = (man->line_ofs * FX32_ONE) / nLine->line_divider;
  VecFx32 p0 = nLine->point_s->pos;
  VecFx32 p1 = nLine->point_e->pos;
  VecFx32 n0, n1;
  VecFx32 def_vec = {0,0,FX32_ONE};
  VecFx32 normal;
  int rotate_dist, rotate_dist1;
  u16 rotate, rotate_s;
  fx32 dist0, dist1;
  FIELD_CAMERA * cam = man->field_camera;
  VecFx32 target;
  
  // �^�[�Q�b�g���W�ƁAp0 p1 �����]�p�x�Ƌ����̈ړ��l�����߂�B
  FIELD_CAMERA_GetTargetPos( cam, &target );

  // XZ���ʂōl����
  target.y  = 0;
  p0.y      = 0;
  p1.y      = 0;

  //  
  VEC_Subtract( &p0, &target, &p0 );
  VEC_Subtract( &p1, &target, &p1 );
  VEC_Normalize( &p0, &n0 );
  VEC_Normalize( &p1, &n1 );

  // �ړ��p�x�����߂�
  rotate_s  = FX_AcosIdx( VEC_DotProduct( &n0, &def_vec ) );
  VEC_CrossProduct( &n0, &def_vec, &normal );
  if( normal.y > 0 ){
    rotate_dist = -rotate_s;
    rotate_s    = 0xffff - rotate_s;
  }else{
    rotate_dist = rotate_s;
  }
  rotate_dist1 = FX_AcosIdx( VEC_DotProduct( &n1, &def_vec ) );
  VEC_CrossProduct( &n1, &def_vec, &normal );
  if( normal.y > 0 ){
    rotate_dist1 = -rotate_dist1;
  }
  rotate_dist = rotate_dist1 - rotate_dist;
  
  dist0   = VEC_Mag( &p0 );
  dist1   = VEC_Mag( &p1 );
  dist1   = dist1 - dist0;

  /*
  OS_TPrintf( "rotate dist=0x%x (%d)\n", rotate_dist, rotate_dist/182 );
  OS_TPrintf( "rotate_s dist=0x%x (%d)\n", rotate_s, rotate_s/182 );

  OS_TPrintf( "dist0=0x%x\n", dist0 );
  OS_TPrintf( "dist dist=0x%x\n", dist1 );
  //*/

  // ����line_ofs�̉�]�p�x�Ƌ��������߂�
  if( rotate_dist != 0 ){
    rotate  = rotate_s + (( rotate_dist * man->line_ofs ) / nLine->line_divider);
  }else{
    rotate  = rotate_s;
  }
  if( dist1 != 0 ){
    dist0   = dist0 + FX_Div( FX_Mul( dist1, man->line_ofs<<FX32_SHIFT ), nLine->line_divider<<FX32_SHIFT );
  }
    
  pos->y = 0;
  pos->x = FX_Mul( FX_SinIdx( rotate ), dist0 ) + target.x;
  pos->z = FX_Mul( FX_CosIdx( rotate ), dist0 ) + target.z;

  /*
  OS_TPrintf( "line_ofs %d\n", man->line_ofs );
  OS_TPrintf( "pos x=0x%x z=0x%x\n", pos->x, pos->z );
  //*/
}

//----------------------------------------------------------------------------
/**
 *	@brief  �~�J��������
 *
 *	@param	man 
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_POSFUNC_CircleCamera( const FIELD_RAIL_MAN * man )
{
  const RAIL_CAMERA_SET * c0;
  const RAIL_CAMERA_SET * c1;
  VecFx32 pos, target, npos, normal;
  fx32 len, dist;
  VecFx32 normal_vec = {0,0,FX32_ONE};
  u16 yaw, pitch;
  u32 div;
  
  // �^�[�Q�b�g���W�擾
  FIELD_CAMERA_GetTargetPos( man->field_camera, &target );
  target.y  = 0;

  // LINE��POINT�œ����ύX
  if(man->nowRail.type == FIELD_RAIL_TYPE_LINE)
  {
    c0 = man->nowRail.line->point_s->camera_set;
    GF_ASSERT(c0->func == FIELD_RAIL_POSFUNC_CircleCamera);
    c1 = man->nowRail.line->point_e->camera_set;
    GF_ASSERT(c1->func == FIELD_RAIL_POSFUNC_CircleCamera);

    // ���@�ʒu�擾
    FIELD_RAIL_MAN_GetPos( man, &pos ); 
    pos.y = 0;
    VEC_Subtract( &pos, &target, &pos );
    VEC_Normalize( &pos, &npos );

    div = man->nowRail.line->line_divider;

    yaw = FX_AcosIdx( VEC_DotProduct( &npos, &normal_vec ) );
    VEC_CrossProduct( &npos, &normal_vec, &normal );
    if( normal.y > 0 ){
      yaw = 0xffff-yaw;
    }

    // �s�b�`
    if( c0->param1 != c1->param1 ){
      pitch = c0->param1 + ( ((int)c1->param1 - (int)c0->param1) * man->line_ofs / div );
    }else{
      pitch = c0->param1;
    }

    // �����̌v�Z fx32�^
    dist = ((fx32)c1->param0 - (fx32)c0->param0);
    if( dist > 0 ){
      len = (fx32)c0->param0 + FX_Div( FX_Mul( dist, man->line_ofs<<FX32_SHIFT ), div<<FX32_SHIFT );
    }else{
      len = c0->param0;
    }
  }
  else
  {
    c0 = man->nowRail.point->camera_set;
    GF_ASSERT(c0->func == FIELD_RAIL_POSFUNC_CircleCamera);

    // ���@�ʒu�擾
    FIELD_RAIL_MAN_GetPos( man, &pos ); 
    pos.y = 0;
    VEC_Subtract( &pos, &target, &pos );
    VEC_Normalize( &pos, &npos );


    div = man->nowRail.line->line_divider;

    // ���[
    yaw = FX_AcosIdx( VEC_DotProduct( &npos, &normal_vec ) );
    VEC_CrossProduct( &npos, &normal_vec, &normal );
    if( normal.y > 0 ){
      yaw = 0xffff-yaw;
    }

    // �s�b�`
    pitch = c0->param1;

    // �����̌v�Z fx32�^
    len = c0->param0;
  }

  FIELD_CAMERA_SetAngleYaw(man->field_camera, yaw);
  FIELD_CAMERA_SetAnglePitch(man->field_camera, pitch);
  FIELD_CAMERA_SetAngleLen(man->field_camera, len);
}

