
//==============================================================================
/**
 * @file	quaternion.c
 * @brief	�N�H�[�^�j�I��
 * @author	soga
 * @date	2008.03.26
 *
 * �����ɐF�X�ȉ�����������Ă��悢
 *
 */
//==============================================================================

#include	"gflib.h"

/* �N�H�[�^�j�I���\���� */
struct _GFL_QUATERNION{
	fx32 w;
	fx32 x;
	fx32 y;
	fx32 z;
};

static	void	MakeQuaternion(GFL_QUATERNION *qt,const GFL_QUATERNION *a,u16 rot);

//=============================================================================================
//=============================================================================================
//	�ݒ�֐�
//=============================================================================================
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���������i�������m�ہj
 *
 * @param[out]	qt		�N�H�[�^�j�I���\���̂ւ̃|�C���^
 * @param[in]	heapID	�������m�ۂ���q�[�vID
 */
//--------------------------------------------------------------------------------------------
GFL_QUATERNION *GFL_QUAT_Init(int heapID)
{
	GFL_QUATERNION *qt;

	qt=GFL_HEAP_AllocMemory(heapID,sizeof(GFL_QUATERNION));
	GFL_QUAT_Zero(qt);

	return qt;
}

//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���j��
 *
 * @param[out]	qt	�j������N�H�[�^�j�I���\���̂ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Free(GFL_QUATERNION *qt)
{
	GFL_HEAP_FreeMemory(qt);
	qt=NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���������i�O�j
 *
 * @param[out]	qt	����������N�H�[�^�j�I���\���̂ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Zero(GFL_QUATERNION *qt)
{
	qt->w = qt->x = qt->y = qt->z = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���������i�P�j
 *
 * @param[out]	qt	����������N�H�[�^�j�I���\���̂ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Identity(GFL_QUATERNION *qt)
{
	qt->w = FX32_ONE;
	qt->x = qt->y = qt->z = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���������i�w�肳�ꂽ�����j
 *
 * @param[out]	qt	����������N�H�[�^�j�I���\���̂ւ̃|�C���^
 * @param[in]	w	���������邗�l
 * @param[in]	x	���������邘�l
 * @param[in]	y	���������邙�l
 * @param[in]	z	���������邚�l
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Assign(GFL_QUATERNION *qt, fx32 w, fx32 x, fx32 y, fx32 z)
{
	qt->w = w;
	qt->x = x;
	qt->y = y;
	qt->z = z;
}

//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���̉��Z
 *
 * @param[out]	ans		�v�Z���ʂ�Ԃ��N�H�[�^�j�I���\���̂ւ̃|�C���^
 * @param[in]	qt_a	���Z�lA
 * @param[in]	qt_b	���Z�lB
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Add(GFL_QUATERNION *ans, const GFL_QUATERNION *qt_a, const GFL_QUATERNION *qt_b)
{
	ans->w = qt_a->w + qt_b->w;
	ans->x = qt_a->x + qt_b->x;
	ans->y = qt_a->y + qt_b->y;
	ans->z = qt_a->z + qt_b->z;
}

//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���̌��Z
 *
 * @param[out]	ans		�v�Z���ʂ�Ԃ��N�H�[�^�j�I���\���̂ւ̃|�C���^
 * @param[in]	qt_a	���Z�lA
 * @param[in]	qt_b	���Z�lB
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Sub(GFL_QUATERNION *ans, const GFL_QUATERNION *qt_a, const GFL_QUATERNION *qt_b)
{
	ans->w = qt_a->w - qt_b->w;
	ans->x = qt_a->x - qt_b->x;
	ans->y = qt_a->y - qt_b->y;
	ans->z = qt_a->z - qt_b->z;
}

//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���̏�Z
 *
 * @param[out]	ans		�v�Z���ʂ�Ԃ��N�H�[�^�j�I���\���̂ւ̃|�C���^
 * @param[in]	qt_a	��Z�lA
 * @param[in]	qt_b	��Z�lB
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Mul(GFL_QUATERNION *ans, const GFL_QUATERNION *qt_a, const GFL_QUATERNION *qt_b)
{
	GFL_QUATERNION	ans_work;

	ans_work.w = FX_Mul(qt_a->w , qt_b->w) - FX_Mul(qt_a->x , qt_b->x) - FX_Mul(qt_a->y , qt_b->y) - FX_Mul(qt_a->z , qt_b->z);
	ans_work.x = FX_Mul(qt_a->w , qt_b->x) + FX_Mul(qt_a->x , qt_b->w) - FX_Mul(qt_a->y , qt_b->z) + FX_Mul(qt_a->z , qt_b->y);
	ans_work.y = FX_Mul(qt_a->w , qt_b->y) + FX_Mul(qt_a->x , qt_b->z) + FX_Mul(qt_a->y , qt_b->w) - FX_Mul(qt_a->z , qt_b->x);
	ans_work.z = FX_Mul(qt_a->w , qt_b->z) - FX_Mul(qt_a->x , qt_b->y) + FX_Mul(qt_a->y , qt_b->x) + FX_Mul(qt_a->z , qt_b->w);

	ans->w=ans_work.w;
	ans->x=ans_work.x;
	ans->y=ans_work.y;
	ans->z=ans_work.z;
}

//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���Ǝ����̏�Z
 *
 * @param[out]	ans	�v�Z���ʂ�Ԃ��N�H�[�^�j�I���\���̂ւ̃|�C���^
 * @param[in]	s	��Z�l����
 * @param[in]	qt	��Z�l�N�H�[�^�j�I��
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_MulReal(GFL_QUATERNION *ans, fx32 s, const GFL_QUATERNION *qt)
{
	ans->w = FX_Mul(s , qt->w);
	ans->x = FX_Mul(s , qt->x);
	ans->y = FX_Mul(s , qt->y);
	ans->z = FX_Mul(s , qt->z);
}

//--------------------------------------------------------------------------------------------
/**
 * �N�H�[�^�j�I���Ǝ����̏��Z
 *
 * @param[out]	ans	�v�Z���ʂ�Ԃ��N�H�[�^�j�I���\���̂ւ̃|�C���^
 * @param[in]	qt	���Z�l�N�H�[�^�j�I��
 * @param[in]	s	���Z�l����
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_DivReal(GFL_QUATERNION *ans, const GFL_QUATERNION *qt, fx32 s)
{
	ans->w = FX_Div(qt->w , s);
	ans->x = FX_Div(qt->x , s);
	ans->y = FX_Div(qt->y , s);
	ans->z = FX_Div(qt->z , s);
}

//--------------------------------------------------------------------------------------------
/**
 *	�N�H�[�^�j�I���̃m�����i ||a||^2 �j
 *
 * @param[in]	qt	�v�Z����N�H�[�^�j�I���\���̂̃|�C���^
 */
//--------------------------------------------------------------------------------------------
fx32 GFL_QUAT_NormSqr(const GFL_QUATERNION *qt)
{
	return (FX_Mul(qt->w , qt->w) + FX_Mul(qt->x , qt->x) + FX_Mul(qt->y , qt->y) + FX_Mul(qt->z , qt->z));
}

//--------------------------------------------------------------------------------------------
/**
 *	�N�H�[�^�j�I���̃m�����i ||a|| �j
 *
 * @param[in]	qt	�v�Z����N�H�[�^�j�I���\���̂̃|�C���^
 */
//--------------------------------------------------------------------------------------------
fx32 GFL_QUAT_Norm(const GFL_QUATERNION *qt)
{
	return FX_Sqrt(GFL_QUAT_NormSqr(qt));
}

//--------------------------------------------------------------------------------------------
/**
 *	�e���̉�]�ʂ���N�H�[�^�j�I���𐶐�����
 *
 * @param[out]	qt		���������N�H�[�^�j�I�����i�[����\���̂ւ̃|�C���^
 * @param[in]	rot_x	X���̉�]��
 * @param[in]	rot_y	Y���̉�]��
 * @param[in]	rot_z	Z���̉�]��
 */
//--------------------------------------------------------------------------------------------
void	GFL_QUAT_MakeQuaternionXYZ(GFL_QUATERNION *qt,u16 rot_x,u16 rot_y,u16 rot_z)
{
	GFL_QUATERNION	a;
	GFL_QUATERNION	qt_x,qt_y,qt_z,qt_ans;
	fx32			norm;

	a.w=0;
	a.x=FX32_ONE;
	a.y=0;
	a.z=0;
	MakeQuaternion(&qt_x,&a,rot_x);

	a.w=0;
	a.x=0;
	a.y=FX32_ONE;
	a.z=0;
	MakeQuaternion(&qt_y,&a,rot_y);

	a.w=0;
	a.x=0;
	a.y=0;
	a.z=FX32_ONE;
	MakeQuaternion(&qt_z,&a,rot_z);

	GFL_QUAT_Mul(qt,&qt_x,&qt_y);
	GFL_QUAT_Mul(qt,qt,&qt_z);
}

static	void	MakeQuaternion(GFL_QUATERNION *qt,const GFL_QUATERNION *a,u16 rot)
{
	fx32	sss=FX_SinIdx(rot);

	qt->w = FX_CosIdx(rot);
	qt->x = FX_Mul(a->x,sss);
	qt->y = FX_Mul(a->y,sss);
	qt->z = FX_Mul(a->z,sss);
}

//--------------------------------------------------------------------------------------------
/**
 *	�N�H�[�^�j�I�������]�s��𐶐�����
 *
 * @param[out]	mtx	���������]�s��\���̂ւ̃|�C���^
 * @param[in]	qt	���ɂȂ�N�H�[�^�j�I���\���̂̃|�C���^
 */
//--------------------------------------------------------------------------------------------
void	GFL_QUAT_MakeRotateMatrix(MtxFx44 *mtx,GFL_QUATERNION *qt)
{
	fx32	xx;
	fx32	yy;
	fx32	zz;
	fx32	xy;
	fx32	yz;
	fx32	xz;
	fx32	wx;
	fx32	wy;
	fx32	wz;

	xx = FX_Mul(qt->x,qt->x);
	yy = FX_Mul(qt->y,qt->y);
	zz = FX_Mul(qt->z,qt->z);

	xy = FX_Mul(qt->x,qt->y);
	yz = FX_Mul(qt->y,qt->z);
	xz = FX_Mul(qt->x,qt->z);

	wx = FX_Mul(qt->w,qt->x);
	wy = FX_Mul(qt->w,qt->y);
	wz = FX_Mul(qt->w,qt->z);

	mtx->_00 = FX32_ONE - FX_Mul(FX32_ONE*2,(yy + zz));
	mtx->_01 =            FX_Mul(FX32_ONE*2,(xy - wz));
	mtx->_02 =            FX_Mul(FX32_ONE*2,(xz + wy));
	mtx->_03 = 0;
	mtx->_10 =            FX_Mul(FX32_ONE*2,(xy + wz));
	mtx->_11 = FX32_ONE - FX_Mul(FX32_ONE*2,(zz + xx));
	mtx->_12 =            FX_Mul(FX32_ONE*2,(yz - wx));
	mtx->_13 = 0;
	mtx->_20 =            FX_Mul(FX32_ONE*2,(xz - wy));
	mtx->_21 =            FX_Mul(FX32_ONE*2,(yz + wx));
	mtx->_22 = FX32_ONE - FX_Mul(FX32_ONE*2,(yy + xx));
	mtx->_23 = 0;
	mtx->_30 = 0;
	mtx->_31 = 0;
	mtx->_32 = 0;
	mtx->_33 = FX32_ONE;
}

