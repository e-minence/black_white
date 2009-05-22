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
static BOOL debugCheckLineData(const RAIL_LINE * line);
static BOOL debugCheckPointData(const RAIL_POINT * point);
static void debugPrintPoint(const char * before, const RAIL_POINT * point, const char * after);


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
        debugCheckLineData(man->nowRail.line);
        
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
        debugCheckPointData(man->nowRail.point);
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
        debugCheckPointData(man->nowRail.point);
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
    if (type == FIELD_RAIL_TYPE_POINT)
    {
      debugPrintPoint(NULL, man->nowRail.point ,"\n");
    } else if (type == FIELD_RAIL_TYPE_LINE)
    {
      debugPrintPoint("start", man->nowRail.line->point_s, NULL);
      debugPrintPoint("-end", man->nowRail.line->point_e, "\n");
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief ���݈ʒu�̎擾
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
 * @brief �J������Ԃ̃A�b�v�f�[�g
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
/**
 *
 */
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

//============================================================================================
//
//
//    �f�o�b�O�p�֐�
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL debugCheckLineData(const RAIL_LINE * line)
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
static BOOL debugCheckPointData(const RAIL_POINT * point)
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
//------------------------------------------------------------------
static void debugPrintWholeVector(const char * before, const VecFx32 * vec, const char * after)
{
  if (before)
  {
    OS_TPrintf("%s",before);
  }
  OS_Printf("( %d, %d, %d)", FX_Whole(vec->x), FX_Whole(vec->y), FX_Whole(vec->z));
  if (after)
  {
    OS_TPrintf("%s",after);
  }
}
static void debugPrintHexVector(const VecFx32 * vec)
{
  OS_Printf("(%08x, %08x, %08x)\n", (vec->x), (vec->y), (vec->z));
}

static void debugPrintPoint(const char * before, const RAIL_POINT * point, const char * after)
{
  if (point != NULL)
  {
    debugPrintWholeVector(before, &point->pos, after);
  }
  else
  {
    if (before) OS_TPrintf("%s", before);
    OS_Printf("( NULL )");
    if (after) OS_TPrintf("%s", after);
  }
}

static void debugPrintDegree(const VecFx32 * p0, const VecFx32 * p1)
{
    VecFx32 s,e;
    fx32 angle;
    VEC_Normalize( p0, &s );
    VEC_Normalize( p1, &e );
    angle = FX_AcosIdx( VEC_DotProduct( &s, &e ) );
    OS_Printf("Degree: %08x\n",angle);
}

//============================================================================================
//
//    ���W���Z�֘A
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void getVectorFromAngleValue(VecFx32 * vec, u16 yaw, u16 pitch, fx32 len)
{
  enum {  PITCH_LIMIT = 0x200 };
  VecFx32 cameraPos;

	fx16 sinYaw = FX_SinIdx(yaw);
	fx16 cosYaw = FX_CosIdx(yaw);
	fx16 sinPitch = FX_SinIdx(pitch);
	fx16 cosPitch = FX_CosIdx(pitch);

  // �����肷��ƃJ������ʒu���ύX����K�v������I�I
  // ���܂�肵�Ȃ��悤�Ƀ}�C�i�X�l�̓v���X�l�ɕ␳
	if(cosPitch < 0){ cosPitch = -cosPitch; }
  // 0�l�ߕӂ͕s���\���ɂȂ邽�ߕ␳
	if(cosPitch < PITCH_LIMIT){ cosPitch = PITCH_LIMIT; }

	VEC_Set( &cameraPos, FX_Mul(sinYaw, cosPitch), sinPitch, FX_Mul(cosYaw, cosPitch) );
	VEC_Normalize(&cameraPos, &cameraPos);
  {
    const VecFx32 zeroVec = {0, 0, 0};
    VEC_MultAdd(len, &cameraPos, &zeroVec, &cameraPos);
  }
  *vec = cameraPos;
}

#if 0
static void getAngleFromVector(const VecFx32 * vec, u16 * yaw, u16 * pitch, fx32 * len)
{
  *len = VEC_Mag(vec);
}
#endif

//------------------------------------------------------------------
/**
 * @brief �x�N�g���ƃx�N�g���̊Ԃ̐��`�⊮�������Ȃ�
 *
 * @param out     ���������x�N�g��
 * @param start   �J�n�x�N�g��
 * @param end     �I���x�N�g��
 * @param t       0�`FX32_ONE
 *
 * ���������x�N�g���͐��K������Ă���̂Œ���
 */
//------------------------------------------------------------------
static void getInterNormalVector(VecFx32 * out, const VecFx32 * start, const VecFx32 * end, fx32 t)
{
  VecFx32 s, e;
  fx32 angle;   //-1�`1
  u16 sinTheta; //radian index
  u16 Ps, Pe;   //radian index

  VEC_Normalize( start, &s );
  VEC_Normalize( end, &e );
  angle = FX_AcosIdx( VEC_DotProduct( &s, &e ) );
  sinTheta = FX_SinIdx( angle );
  Ps = FX_SinIdx( FX_Mul(angle, FX32_ONE - t) );
  Pe = FX_SinIdx( FX_Mul(angle, t) );

  // out = ( Ps * s + Pe * e ) / sinTheta
  GFL_CALC3D_VEC_MulScalar(&s, FX32_ONE * Ps / sinTheta, &s);
  GFL_CALC3D_VEC_MulScalar(&e, FX32_ONE * Pe / sinTheta, &e);
  //GFL_CALC3D_VEC_MulScalar(&s, FX32_ONE * Ps / 0x10000, &s);
  //GFL_CALC3D_VEC_MulScalar(&e, FX32_ONE * Pe / 0x10000, &e);
  VEC_Add(&s, &e, out);
  //GFL_CALC3D_VEC_DivScalar(out, FX32_ONE * sinTheta / 0x10000, out);

  VEC_Normalize(out, out);
}

//------------------------------------------------------------------
/**
 * @brief �x�N�g���ƃx�N�g���̊Ԃ̐��`�⊮�i�������⊮�j
 *
 * @param out     ���������x�N�g��
 * @param start   �J�n�x�N�g��
 * @param end     �I���x�N�g��
 * @param t       0�`FX32_ONE
 */
//------------------------------------------------------------------
static void getIntermediateVector(VecFx32 * out, const VecFx32 * start, const VecFx32 * end, fx32 t)
{
  const VecFx32 zeroVec = {0, 0, 0};
  fx32 l_s, l_e, l_i;
  getInterNormalVector(out, start, end, t);

  l_s = VEC_Mag(start);
  l_e = VEC_Mag(end);
  l_i = l_s + FX_Mul((l_e - l_s) , t);
  VEC_MultAdd(l_i, out, &zeroVec, out);
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

  if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_B)
  {
    debugPrintWholeVector("start ", p0, "\n");
    debugPrintWholeVector("now   ", pos, "\n");
    debugPrintWholeVector("end   ", p1, "\n");
  }
}

//------------------------------------------------------------------
/**
 *
 * �i���S�_-->�X�^�[�g�ʒu�j�A�i���S�_-->�I���ʒu�j�̓�̃x�N�g����
 * ���ʐ��`�⊮���Ă���
 */
//------------------------------------------------------------------
void FIELD_RAIL_POSFUNC_CurveLine(const FIELD_RAIL_MAN * man, VecFx32 * pos)
{
  const RAIL_LINE * nLine = man->nowRail.line;
  const VecFx32 * p_s = &nLine->point_s->pos;
  const VecFx32 * p_e = &nLine->point_e->pos;
  VecFx32 center, vec_s, vec_e, vec_i;
  fx32 ofs = (man->line_ofs * FX32_ONE) / nLine->line_divider;

  center.x = nLine->line_pos_set->param0;
  center.y = nLine->line_pos_set->param1;
  center.z = nLine->line_pos_set->param2;

  VEC_Subtract(p_s, &center, &vec_s);
  VEC_Subtract(p_e, &center, &vec_e);
  
  getIntermediateVector(&vec_i, &vec_s, &vec_e, ofs);
  VEC_Add(&center, &vec_i, pos);

  if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_B)
  {
    debugPrintWholeVector("Center:", &center, "\n");
    debugPrintWholeVector("start: ", p_s, "\n");
    debugPrintWholeVector("end:   ", p_e, "\n");
    debugPrintWholeVector("v_s:   ", &vec_s, "\n");
    OS_Printf("v_s:len=%08x\n",VEC_Mag(&vec_s));
    debugPrintWholeVector("v_e:   ", &vec_e, "\n");
    OS_Printf("v_e:len=%08x\n",VEC_Mag(&vec_e));
    debugPrintWholeVector("v_i:   ", &vec_i, "\n");
    debugPrintHexVector(&vec_i);
    OS_Printf("v_i:len=%08x\n",VEC_Mag(&vec_i));
    debugPrintWholeVector("pos:   ", pos, "\n");
  }
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

//============================================================================================
//
//
//      �J�����w��̂��߂̃c�[��
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void calcAngleCamera(FIELD_CAMERA * field_camera)
{
  {
    VecFx32 camPos, target, before;
    u16 yaw = FIELD_CAMERA_GetAngleYaw(field_camera);
    u16 pitch = FIELD_CAMERA_GetAnglePitch(field_camera);
    fx32 len = FIELD_CAMERA_GetAngleLen(field_camera);

    FIELD_CAMERA_GetCameraPos(field_camera, &before);

    FIELD_CAMERA_GetTargetPos(field_camera, &target);
    getVectorFromAngleValue(&camPos, yaw, pitch, len);
    VEC_Add(&camPos, &target, &camPos);
    FIELD_CAMERA_SetCameraPos(field_camera, &camPos);

    if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y)
    {
      debugPrintWholeVector("CamTgt:", &target, "\n");
      debugPrintWholeVector("CamPos:", &camPos, "\n");
      OS_Printf("yaw:%04x pitch:%04x len:%08x\n",yaw, pitch, len);
    }
  }
}

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

  calcAngleCamera(man->field_camera);
}

//------------------------------------------------------------------
/**
 * @brief �J�����w��F�ێ�������POINT��angle�o�����[��⊮����
 */
//------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_OfsAngleCamera(const FIELD_RAIL_MAN* man)
{
  VecFx32 before;
  VecFx32 c_s, c_e, c_now, target;

  const RAIL_CAMERA_SET * cs;
  const RAIL_CAMERA_SET * ce;
  GF_ASSERT(man->nowRail.type == FIELD_RAIL_TYPE_LINE);

  cs = man->nowRail.line->point_s->camera_set;
  GF_ASSERT(cs->func == FIELD_RAIL_CAMERAFUNC_FixAngleCamera);
  ce = man->nowRail.line->point_e->camera_set;
  GF_ASSERT(ce->func == FIELD_RAIL_CAMERAFUNC_FixAngleCamera);

  FIELD_CAMERA_GetCameraPos(man->field_camera, &before);

  {
    u32 div = man->nowRail.line->line_divider;
    fx32 t = FX32_ONE * man->line_ofs / div;

    getVectorFromAngleValue(&c_s, cs->param1, cs->param0, cs->param2);
    getVectorFromAngleValue(&c_e, ce->param1, ce->param0, ce->param2);
    getIntermediateVector(&c_now, &c_s, &c_e, t);

    FIELD_CAMERA_GetTargetPos(man->field_camera, &target);
    VEC_Add(&c_now, &target, &c_now);
    FIELD_CAMERA_SetCameraPos(man->field_camera, &c_now);
  }

  if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y)
  {
    debugPrintWholeVector("CamTgt:", &target, "\n");
    debugPrintWholeVector("CamPos:", &c_now, "\n");
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

