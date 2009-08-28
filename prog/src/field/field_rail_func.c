//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_rail_func.c
 *	@brief	�t�B�[���h���[���@��{�֐��S
 *	@author	tomoya takahashi
 *	@date		2009.07.06
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "field/fieldmap_proc.h"

#include "field_rail_access.h"
#include "field_rail_func.h"


#ifdef PM_DEBUG


#endif

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static void updateCircleCamera( const FIELD_RAIL_MAN * man, u16 pitch, fx32 len, const VecFx32* cp_target );


//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintWholeVector(const char * before, const VecFx32 * vec, const char * after)
{
  if (before)
  {
    OS_TPrintf("%s",before);
  }
  OS_TPrintf("( %d, %d, %d)", FX_Whole(vec->x), FX_Whole(vec->y), FX_Whole(vec->z));
  if (after)
  {
    OS_TPrintf("%s",after);
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintHexVector(const VecFx32 * vec)
{
  OS_TPrintf("(%08x, %08x, %08x)\n", (vec->x), (vec->y), (vec->z));
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintPoint(const char * before, const RAIL_POINT * point, const char * after)
{
  if (point != NULL)
  {
    debugPrintWholeVector(before, &point->pos, after);
  }
  else
  {
    if (before) OS_TPrintf("%s", before);
    OS_TPrintf("( NULL )");
    if (after) OS_TPrintf("%s", after);
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void debugPrintDegree(const VecFx32 * p0, const VecFx32 * p1)
{
    VecFx32 s,e;
    fx32 angle;
    VEC_Normalize( p0, &s );
    VEC_Normalize( p1, &e );
    angle = FX_AcosIdx( VEC_DotProduct( &s, &e ) );
    OS_TPrintf("Degree: %08x\n",angle);
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


//------------------------------------------------------------------
/**
 * @brief �x�N�g���ƃx�N�g���̊Ԃ̋��ʐ��`�⊮�������Ȃ�
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
	
	if( (sinTheta == 0) )
	{
		// sinTheta��0�@(�ω�����p�x�����Ȃ�����)�ꍇ�́A�I���ɂ����ɂ���
		*out = e;
	}
	else
	{
		Ps = FX_SinIdx( FX_Mul(angle<<FX32_SHIFT, FX32_ONE - t)>>FX32_SHIFT );
		Pe = FX_SinIdx( FX_Mul(angle<<FX32_SHIFT, t)>>FX32_SHIFT );

		// out = ( Ps * s + Pe * e ) / sinTheta
		GFL_CALC3D_VEC_MulScalar(&s, FX_Div( Ps, sinTheta), &s);
		GFL_CALC3D_VEC_MulScalar(&e, FX_Div( Pe, sinTheta), &e);
		//GFL_CALC3D_VEC_MulScalar(&s, FX32_ONE * Ps / 0x10000, &s);
		//GFL_CALC3D_VEC_MulScalar(&e, FX32_ONE * Pe / 0x10000, &e);
		VEC_Add(&s, &e, out);
		//GFL_CALC3D_VEC_DivScalar(out, FX32_ONE * sinTheta / 0x10000, out);
		VEC_Normalize(out, out);
	}

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

  // out = (|start| + (|end| - |start|) * t) * out
  l_s = VEC_Mag(start);
  l_e = VEC_Mag(end);
  l_i = l_s + FX_Mul((l_e - l_s) , t);
  VEC_MultAdd(l_i, out, &zeroVec, out);
}

//------------------------------------------------------------------
/**
 * @brief ���[���ړ��F�_�Ɠ_�̊Ԃ̒����⊮
 */
//------------------------------------------------------------------
void FIELD_RAIL_POSFUNC_StraitLine(const FIELD_RAIL_WORK * work, VecFx32 * pos)
{
  //LINE�ɂ���Ƃ���POINT��POINT�̊Ԃ̍��W
  //pos = p0 + (p1 - p0) * (ofs / div )
  const RAIL_LINE * nLine = FIELD_RAIL_GetLine( work );
	s32 line_ofs = FIELD_RAIL_GetLineOfs( work );
	s32 line_ofs_max = FIELD_RAIL_GetLineOfsMax( work );
  fx32 ofs = FX_Div( (line_ofs * FX32_ONE), line_ofs_max<<FX32_SHIFT);
  VecFx32 val;
	const RAIL_POINT* point_s = FIELD_RAIL_GetPointStart( work );
	const RAIL_POINT* point_e = FIELD_RAIL_GetPointEnd( work );
  const VecFx32 * p0 = &point_s->pos;
  const VecFx32 * p1 = &point_e->pos;
  VEC_Subtract(p1, p0, &val);
  VEC_MultAdd(ofs, &val, p0, pos);

  {
    VecFx32 xzNormal = {0,FX32_ONE, 0}; //XZ���ʂɐ����ȒP�ʃx�N�g��
    VecFx32 width;
		s32 width_ofs = FIELD_RAIL_GetWidthOfs( work );
		fx32 ofs_unit = FIELD_RAIL_GetOfsUnit( work );
    fx32 w_ofs = FX_Mul( width_ofs<<FX32_SHIFT, ofs_unit );
    VEC_CrossProduct(&val, &xzNormal, &width);
    VEC_Normalize(&width, &width);
    VEC_MultAdd(w_ofs, &width, pos, pos);

		if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_B)
		{
			debugPrintWholeVector("start ", p0, "\n");
			//debugPrintHexVector(p0);
			debugPrintWholeVector("now   ", pos, "\n");
			//debugPrintHexVector(pos);
			debugPrintWholeVector("end   ", p1, "\n");
			//debugPrintHexVector(p1);
			TOMOYA_Printf( "w_ofs = 0x%x\n", w_ofs );
			debugPrintHexVector(&width);
		}
  }

}

//------------------------------------------------------------------
/**
 *
 * @brief ���[���ړ��F�_�Ɠ_�̊Ԃ��Ȑ��⊮
 *
 * �i���S�_-->�X�^�[�g�ʒu�j�A�i���S�_-->�I���ʒu�j�̓�̃x�N�g����
 * ���ʐ��`�⊮���Ă���B���S�_�̈ʒu�̓p�����[�^�Ŏ󂯎��
 */
//------------------------------------------------------------------
void FIELD_RAIL_POSFUNC_CurveLine(const FIELD_RAIL_WORK * work, VecFx32 * pos)
{
  const RAIL_LINE * nLine = FIELD_RAIL_GetLine(work);
	const RAIL_LINEPOS_SET* line_pos_set;
	const RAIL_POINT* point_s = FIELD_RAIL_GetPointStart( work );
	const RAIL_POINT* point_e = FIELD_RAIL_GetPointEnd( work );
  const VecFx32 * p_s = &point_s->pos;
  const VecFx32 * p_e = &point_e->pos;
	const RAIL_POSFUNC_CURVE_WORK* cp_wk;
  VecFx32 center, vec_s, vec_e, vec_i;
	s32 line_ofs			= FIELD_RAIL_GetLineOfs( work );
	s32 line_ofs_max	= FIELD_RAIL_GetLineOfsMax( work );
  fx32 ofs = FX_Div( (line_ofs * FX32_ONE), line_ofs_max*FX32_ONE );
  VecFx32 pos_sub;


	// Y���W�́A���`�ŋ��߂�B
	// XZ�́A��]�ł��Ƃ߂�
	pos->y = p_e->y - p_s->y;
	pos->y = FX_Div( FX_Mul( pos->y, line_ofs<<FX32_SHIFT ), line_ofs_max<<FX32_SHIFT );
	pos->y += p_s->y;
	

	line_pos_set = FIELD_RAIL_GetLinePosSet( work );
	cp_wk = (const RAIL_POSFUNC_CURVE_WORK*)line_pos_set->work;
	center.x = cp_wk->x;
	center.y = pos->y;
	center.z = cp_wk->z;

	VEC_Subtract(p_s, &center, &vec_s);
	VEC_Subtract(p_e, &center, &vec_e);
	vec_s.y = 0;
	vec_e.y = 0;

  VEC_Subtract( &vec_e, &vec_s, &pos_sub );
  pos_sub.y = 0;
  VEC_Normalize( &pos_sub, &pos_sub );

	
	getIntermediateVector(&vec_i, &vec_s, &vec_e, ofs);
	VEC_Add(&center, &vec_i, pos);

	{
		VecFx32 w_vec;
    VecFx32 normal;
		s32 width_ofs = FIELD_RAIL_GetWidthOfs( work );
		fx32 ofs_unit = FIELD_RAIL_GetOfsUnit( work );
    fx32 ofs_w = FX_Mul( width_ofs<<FX32_SHIFT, ofs_unit );
    
		VEC_Normalize(&vec_i, &w_vec);
    VEC_CrossProduct( &w_vec, &pos_sub, &normal );
    if( normal.y < 0 )
    {
  		VEC_MultAdd(-(ofs_w), &w_vec, pos, pos);
    }
    else
    {
  		VEC_MultAdd(ofs_w, &w_vec, pos, pos);
    }
	}

  if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_B)
  {
    debugPrintWholeVector("Center:", &center, "\n");
    debugPrintWholeVector("start: ", p_s, "\n");
    debugPrintWholeVector("end:   ", p_e, "\n");
    debugPrintWholeVector("v_s:   ", &vec_s, "\n");
    OS_TPrintf("v_s:len=%08x\n",VEC_Mag(&vec_s));
    debugPrintWholeVector("v_e:   ", &vec_e, "\n");
    OS_TPrintf("v_e:len=%08x\n",VEC_Mag(&vec_e));
    debugPrintWholeVector("v_i:   ", &vec_i, "\n");
    debugPrintHexVector(&vec_i);
    OS_TPrintf("v_i:len=%08x\n",VEC_Mag(&vec_i));
    debugPrintWholeVector("pos:   ", pos, "\n");
  }
}


//============================================================================================
//
//
//      �Q�_�Ԃ̋��������߂�
//
//
//============================================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	����
 */
//-----------------------------------------------------------------------------
fx32 FIELD_RAIL_LINE_DIST_FUNC_StraitLine( const RAIL_POINT * point_s, const RAIL_POINT * point_e, const RAIL_LINEPOS_SET * line_pos_set )
{
	return VEC_Distance( &point_s->pos, &point_e->pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�~
 */
//-----------------------------------------------------------------------------
fx32 FIELD_RAIL_LINE_DIST_FUNC_CircleLine( const RAIL_POINT * point_s, const RAIL_POINT * point_e, const RAIL_LINEPOS_SET * line_pos_set )
{
	u16     rotate;
	VecFx32 center;
	fx32    r;
	fx32    circle_par;
  VecFx32 p0;
  VecFx32 p1;
  fx32 dist;
	const RAIL_POSFUNC_CURVE_WORK* cp_wk;

  p0 = point_s->pos;
  p1 = point_e->pos;

	cp_wk = (const RAIL_POSFUNC_CURVE_WORK*)line_pos_set->work;
	center.x = cp_wk->x;
	center.y = cp_wk->y;
	center.z = cp_wk->z;

	// ���S����̋����ɕύX
	VEC_Subtract( &p0, &center, &p0 );
	VEC_Subtract( &p1, &center, &p1 );
  // ���ʂōl����
  p0.y = 0;
  p1.y = 0;

	// ���S����̋����́A���ς��g��
	// ���a�����߂����̂ŁA�Q�ł��Ȃ�
	r = VEC_Mag( &p0 );
	r += VEC_Mag( &p1 );

	// �p�x�����߂�
	VEC_Normalize( &p0, &p0 );
	VEC_Normalize( &p1, &p1 );
	rotate = FX_AcosIdx( VEC_DotProduct( &p0, &p1 ) );  // 180�ȉ��̊p�x���߂��Ă���
	
	
	// �p�x�̔�����߂�
	circle_par = FX_Div( rotate<<FX32_SHIFT, 0x10000<<FX32_SHIFT );
/*    OS_TPrintf( "circle_par 0x%x ", circle_par );
	OS_TPrintf( "rotate=%d ", rotate / 182 );
	OS_TPrintf( "r=0x%x\n", r );//*/

	// �~������A���������߂�
	dist = FX_Mul( r, FX32_CONST(3.140f) );
	dist = FX_Mul( dist, circle_par );

	return dist;
}




//------------------------------------------------------------------
/**
 * @brief �J�����w��F�p�����[�^��Fx32�R�Ƃ��ĒP���ɑ�������
 */
//------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_FixPosCamera(const FIELD_RAIL_MAN* man)
{
  VecFx32 pos;
	const FIELD_RAIL_WORK* work = FIELD_RAIL_MAN_GetBindWork( man );
  const RAIL_CAMERA_SET * cam_set = FIELD_RAIL_GetCameraSet(work);
	const RAIL_CAMERAFUNC_FIXPOS_WORK* cp_wk;
	
  FIELD_RAIL_MAN_GetBindWorkPos(man, &pos);

	// ���W���w�胂�[�h
	FIELD_CAMERA_SetMode( FIELD_RAIL_MAN_GetCamera(man), FIELD_CAMERA_MODE_DIRECT_POS );

	cp_wk = (const RAIL_CAMERAFUNC_FIXPOS_WORK*)cam_set->work;
  pos.x += cp_wk->x;
  pos.y += cp_wk->y;
  pos.z += cp_wk->z;
  FIELD_CAMERA_SetCameraPos( FIELD_RAIL_MAN_GetCamera(man), &pos );

	// �f�t�H���g�^�[�Q�b�g���Q��
	FIELD_CAMERA_BindDefaultTarget( FIELD_RAIL_MAN_GetCamera(man) );
}

//------------------------------------------------------------------
/**
 * @brief �J�����w��F�p�����[�^��Angle�w��Ɏg�p����
 */
//------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_FixAngleCamera(const FIELD_RAIL_MAN* man)
{
  FIELD_CAMERA * cam = FIELD_RAIL_MAN_GetCamera(man);
  VecFx32 pos;
	const FIELD_RAIL_WORK* work = FIELD_RAIL_MAN_GetBindWork( man );
  const RAIL_CAMERA_SET * cam_set = FIELD_RAIL_GetCameraSet(work);
	const RAIL_CAMERAFUNC_FIXANGLE_WORK* cp_wk;

	cp_wk = (const RAIL_CAMERAFUNC_FIXANGLE_WORK*)cam_set->work;

	// ���W���w�胂�[�h
	FIELD_CAMERA_SetMode( cam, FIELD_CAMERA_MODE_DIRECT_POS );

	// �f�t�H���g�^�[�Q�b�g���Q��
	FIELD_CAMERA_BindDefaultTarget( FIELD_RAIL_MAN_GetCamera(man) );

  FIELD_RAIL_MAN_GetBindWorkPos( man, &pos );
  {
    VecFx32 camPos;
    u16 yaw = (u16)cp_wk->yaw;
    u16 pitch = (u16)cp_wk->pitch;
    fx32 len = cp_wk->len;

    getVectorFromAngleValue(&camPos, yaw, pitch, len);
    VEC_Add(&camPos, &pos, &camPos);
    FIELD_CAMERA_SetCameraPos(cam, &camPos);

    if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y)
    {
      debugPrintWholeVector("CamTgt:", &pos, "\n");
      debugPrintWholeVector("CamPos:", &camPos, "\n");
      OS_TPrintf("yaw:%04x pitch:%04x len:%08x\n",yaw, pitch, len);
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief �J�����w��F�ێ�������POINT��angle�o�����[��⊮����
 */
//------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_OfsAngleCamera(const FIELD_RAIL_MAN* man)
{
  VecFx32 c_s, c_e, c_now, target;
  const FIELD_RAIL_WORK * work = FIELD_RAIL_MAN_GetBindWork( man );
	const RAIL_POINT* point_s = FIELD_RAIL_GetPointStart( work );
	const RAIL_POINT* point_e = FIELD_RAIL_GetPointEnd( work );
  const RAIL_CAMERA_SET * cs;
  const RAIL_CAMERA_SET * ce;
	FIELD_CAMERA* cam = FIELD_RAIL_MAN_GetCamera( man );
	const RAIL_CAMERAFUNC_FIXANGLE_WORK* cs_work;
	const RAIL_CAMERAFUNC_FIXANGLE_WORK* ce_work;
	
  GF_ASSERT(FIELD_RAIL_GetType(work) == FIELD_RAIL_TYPE_LINE);

	// ���W���w�胂�[�h
	FIELD_CAMERA_SetMode( cam, FIELD_CAMERA_MODE_DIRECT_POS );

	// �f�t�H���g�^�[�Q�b�g���Q��
	FIELD_CAMERA_BindDefaultTarget( FIELD_RAIL_MAN_GetCamera(man) );

  cs = FIELD_RAIL_POINT_GetCameraSet( work, point_s );
//  GF_ASSERT(getRailDatCameraFunc( &man->rail_dat, cs->func_index ) == FIELD_RAIL_CAMERAFUNC_FixAngleCamera);
  ce = FIELD_RAIL_POINT_GetCameraSet( work, point_e );
//  GF_ASSERT(getRailDatCameraFunc( &man->rail_dat, ce->func_index ) == FIELD_RAIL_CAMERAFUNC_FixAngleCamera);

	cs_work = (const RAIL_CAMERAFUNC_FIXANGLE_WORK*)cs->work;
	ce_work = (const RAIL_CAMERAFUNC_FIXANGLE_WORK*)ce->work;
	
  {
    u32 div = FIELD_RAIL_GetLineOfsMax(work);
		s32 line_ofs = FIELD_RAIL_GetLineOfs(work);
    fx32 t = FX_Div( FX32_ONE * line_ofs, div * FX32_ONE );
    u16 pitch_s = cs_work->pitch;
    fx32 len_s = cs_work->len;
    u16 pitch_e = ce_work->pitch;
    fx32 len_e = ce_work->len;


    getVectorFromAngleValue(&c_s, cs_work->yaw, pitch_s, len_s);
    getVectorFromAngleValue(&c_e, ce_work->yaw, pitch_e, len_e);

		getIntermediateVector(&c_now, &c_s, &c_e, t);

		FIELD_RAIL_MAN_GetBindWorkPos( man, &target );
		VEC_Add(&c_now, &target, &c_now);
		FIELD_CAMERA_SetCameraPos(cam, &c_now);
  }

  if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y)
  {
    debugPrintWholeVector("CamTgt:", &target, "\n");
    debugPrintWholeVector("CamPos:", &c_now, "\n");
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���ׂČ��߂����̃J����
 *
 *	@param	man
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_FixAllCamera(const FIELD_RAIL_MAN* man)
{
  VecFx32 pos;
  VecFx32 target;
	const FIELD_RAIL_WORK* work = FIELD_RAIL_MAN_GetBindWork( man );
  const RAIL_CAMERA_SET * cam_set = FIELD_RAIL_GetCameraSet(work);
	FIELD_CAMERA* cam = FIELD_RAIL_MAN_GetCamera( man );
	const RAIL_CAMERAFUNC_FIXALL_WORK* cp_work;

	cp_work = (const RAIL_CAMERAFUNC_FIXALL_WORK*)cam_set->work;

	// ���W���w�胂�[�h
	FIELD_CAMERA_SetMode( cam, FIELD_CAMERA_MODE_DIRECT_POS );

	// �Ǐ]�^�[�Q�b�gNULL
	FIELD_CAMERA_FreeTarget( FIELD_RAIL_MAN_GetCamera(man) );

  pos.x = cp_work->pos_x;
  pos.y = cp_work->pos_y;
  pos.z = cp_work->pos_z;
  target.x = cp_work->target_x;
  target.y = cp_work->target_y;
  target.z = cp_work->target_z;
  FIELD_CAMERA_SetCameraPos(cam, &pos);
  FIELD_CAMERA_SetTargetPos(cam, &target);
}



//----------------------------------------------------------------------------
/**
 *	@brief  �~�J��������
 *
 *	@param	man 
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_CircleCamera( const FIELD_RAIL_MAN * man )
{
	const FIELD_RAIL_WORK* work = FIELD_RAIL_MAN_GetBindWork( man );
	const FIELD_CAMERA* cam = FIELD_RAIL_MAN_GetCamera( man );
  const RAIL_CAMERA_SET * cs;
  const RAIL_CAMERA_SET * ce;
  const RAIL_POINT * point_s;
  const RAIL_POINT * point_e;
  u16 pitch;
  fx32 len;
  s32 div;
  s32 ofs;
  VecFx32 target;
	u32 type;
	const RAIL_CAMERAFUNC_CIRCLE_WORK* cs_work;
	const RAIL_CAMERAFUNC_CIRCLE_WORK* ce_work;

	type = FIELD_RAIL_GetType( work );


  if( type == FIELD_RAIL_TYPE_LINE )
  {
		point_s = FIELD_RAIL_GetPointStart( work );
		point_e = FIELD_RAIL_GetPointEnd( work );
    cs = FIELD_RAIL_POINT_GetCameraSet( work, point_s );
    ce = FIELD_RAIL_POINT_GetCameraSet( work, point_e );
    div = FIELD_RAIL_GetLineOfsMax(work);
    ofs = FIELD_RAIL_GetLineOfs(work);
  }
  else
  {
    cs = FIELD_RAIL_GetCameraSet( work );
    ce = FIELD_RAIL_GetCameraSet( work );
    div = 1;
    ofs = 0;
  }

	cs_work = (const RAIL_CAMERAFUNC_CIRCLE_WORK*)cs->work;
	ce_work = (const RAIL_CAMERAFUNC_CIRCLE_WORK*)ce->work; 

  // pitch len �̌v�Z
  pitch = cs_work->pitch + ((((int)ce_work->pitch - (int)cs_work->pitch) * ofs) / div);
  len   = cs_work->len + FX_Div( FX_Mul(ce_work->len - cs_work->len, ofs<<FX32_SHIFT), div<<FX32_SHIFT );

  target.x = cs_work->target_x;
  target.y = cs_work->target_y;
  target.z = cs_work->target_z;

  updateCircleCamera( man, pitch, len, &target );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����Œ�@�~�^���J����
 *
 *	@param	man   
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_FixLenCircleCamera( const FIELD_RAIL_MAN * man )
{
	const FIELD_RAIL_WORK* work = FIELD_RAIL_MAN_GetBindWork( man );
  const RAIL_CAMERA_SET * cline;
  u16 pitch;
  fx32 len;
  VecFx32 target;
	const RAIL_CAMERAFUNC_FIXLEN_CIRCLE_WORK* cp_work;



	cline = FIELD_RAIL_GetCameraSet( work );

	cp_work = (const RAIL_CAMERAFUNC_FIXLEN_CIRCLE_WORK*)cline->work;

  // pitch len �̌v�Z
  pitch = cp_work->pitch;
  len   = cp_work->len;


  target.x = cp_work->target_x;
  target.y = cp_work->target_y;
  target.z = cp_work->target_z;

  updateCircleCamera( man, pitch, len, &target );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �~�^���J�����̍X�V
 *
 *	@param	man
 *	@param	pitch
 *	@param	len 
 */
//-----------------------------------------------------------------------------
static void updateCircleCamera( const FIELD_RAIL_MAN * man, u16 pitch, fx32 len, const VecFx32* cp_target )
{
  VecFx32 pos, target, n0, camera_pos;
  FIELD_CAMERA* p_camera;
  fx32 xz_dist;
  fx32 target_y;

//	TOMOYA_Printf( "target x[0x%x] y[0x%x] z[0x%x]\n", cp_target->x, cp_target->y, cp_target->z );

  p_camera = FIELD_RAIL_MAN_GetCamera( man );

	// ���W���w�胂�[�h
	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_DIRECT_POS );

	// �Ǐ]�^�[�Q�b�gNULL
	FIELD_CAMERA_FreeTarget( p_camera );
	
  target    = *cp_target;
  target_y  = target.y;
  target.y  = 0;
  
  FIELD_RAIL_MAN_GetBindWorkPos( man, &pos );
  pos.y   = 0;

  VEC_Subtract( &pos, &target, &pos );
  VEC_Normalize( &pos, &n0 );

  // �����x�N�g������A�J����angle�����߂�
  camera_pos.y = FX_Mul( FX_SinIdx( pitch ), len );
  xz_dist      = FX_Mul( FX_CosIdx( pitch ), len );
  camera_pos.x = FX_Mul( n0.x, xz_dist );
  camera_pos.z = FX_Mul( n0.z, xz_dist );
  camera_pos.x += target.x;
  camera_pos.y += target_y;
  camera_pos.z += target.z;
  
	FIELD_CAMERA_SetTargetPos( p_camera, cp_target );
	FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���C���A�^�[�Q�b�g�ύX �~����J����
 */
//-----------------------------------------------------------------------------
void FIELD_RAIL_CAMERAFUNC_TargetChangeCircleCamera( const FIELD_RAIL_MAN * man )
{
	const FIELD_RAIL_WORK* work = FIELD_RAIL_MAN_GetBindWork( man );
  const RAIL_CAMERA_SET * cline;
  u16 pitch;
  fx32 len;
  VecFx32 target;
	const RAIL_CAMERAFUNC_TARGETCHANGE_CIRCLE_WORK* cp_work;
	u32 type;
  s32 ofs, ofs_max;

	type = FIELD_RAIL_GetType( work );
  // ���[���Ŏg���Ă�
  GF_ASSERT( type == FIELD_RAIL_TYPE_LINE );

  ofs_max = FIELD_RAIL_GetLineOfsMax(work); 
  ofs = FIELD_RAIL_GetLineOfs(work);


	cline = FIELD_RAIL_GetCameraSet( work );

	cp_work = (const RAIL_CAMERAFUNC_TARGETCHANGE_CIRCLE_WORK*)cline->work;

  // pitch len �̌v�Z
  pitch = cp_work->pitch;
  len   = cp_work->len;

  target.x = cp_work->target_sx + FX_Div( FX_Mul( cp_work->target_ex - cp_work->target_sx, ofs<<FX32_SHIFT ), ofs_max<<FX32_SHIFT );
  target.y = cp_work->target_sy + FX_Div( FX_Mul( cp_work->target_ey - cp_work->target_sy, ofs<<FX32_SHIFT ), ofs_max<<FX32_SHIFT );
  target.z = cp_work->target_sz + FX_Div( FX_Mul( cp_work->target_ez - cp_work->target_sz, ofs<<FX32_SHIFT ), ofs_max<<FX32_SHIFT );

  updateCircleCamera( man, pitch, len, &target );
}


