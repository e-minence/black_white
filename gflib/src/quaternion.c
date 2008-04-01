
//==============================================================================
/**
 * @file	quaternion.c
 * @brief	クォータニオン
 * @author	soga
 * @date	2008.03.26
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#include	"gflib.h"

/* クォータニオン構造体 */
struct _GFL_QUATERNION{
	fx32 w;
	fx32 x;
	fx32 y;
	fx32 z;
};

static	void	MakeQuaternion(GFL_QUATERNION *qt,const GFL_QUATERNION *a,u16 rot);
static	fx32	CalcPosZ(fx32 x,fx32 y,fx32 radius);

#define	SCREEN_WIDTH	(256<<FX32_SHIFT)
#define	SCREEN_HEIGHT	(192<<FX32_SHIFT)

//=============================================================================================
//=============================================================================================
//	設定関数
//=============================================================================================
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * クォータニオン初期化（メモリ確保）
 *
 * @param[out]	qt		クォータニオン構造体へのポインタ
 * @param[in]	heapID	メモリ確保するヒープID
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
 * クォータニオン破棄
 *
 * @param[out]	qt	破棄するクォータニオン構造体へのポインタ
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Free(GFL_QUATERNION *qt)
{
	GFL_HEAP_FreeMemory(qt);
	qt=NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * クォータニオン初期化（０）
 *
 * @param[out]	qt	初期化するクォータニオン構造体へのポインタ
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Zero(GFL_QUATERNION *qt)
{
	qt->w = qt->x = qt->y = qt->z = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * クォータニオン初期化（１）
 *
 * @param[out]	qt	初期化するクォータニオン構造体へのポインタ
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Identity(GFL_QUATERNION *qt)
{
	qt->w = FX32_ONE;
	qt->x = qt->y = qt->z = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * クォータニオン初期化（指定された引数）
 *
 * @param[out]	qt	初期化するクォータニオン構造体へのポインタ
 * @param[in]	w	初期化するｗ値
 * @param[in]	x	初期化するｘ値
 * @param[in]	y	初期化するｙ値
 * @param[in]	z	初期化するｚ値
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
 * クォータニオンの加算
 *
 * @param[out]	ans		計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	qt_a	加算値A
 * @param[in]	qt_b	加算値B
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
 * クォータニオンの減算
 *
 * @param[out]	ans		計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	qt_a	減算値A
 * @param[in]	qt_b	減算値B
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
 * クォータニオンの乗算
 *
 * @param[out]	ans		計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	qt_a	乗算値A
 * @param[in]	qt_b	乗算値B
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
 * クォータニオンと実数の乗算
 *
 * @param[out]	ans	計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	s	乗算値実数
 * @param[in]	qt	乗算値クォータニオン
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
 * クォータニオンと実数の除算
 *
 * @param[out]	ans	計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	qt	除算値クォータニオン
 * @param[in]	s	除算値実数
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
 *	クォータニオンのノルム（ ||a||^2 ）
 *
 * @param[in]	qt	計算するクォータニオン構造体のポインタ
 */
//--------------------------------------------------------------------------------------------
fx32 GFL_QUAT_NormSqr(const GFL_QUATERNION *qt)
{
	return (FX_Mul(qt->w , qt->w) + FX_Mul(qt->x , qt->x) + FX_Mul(qt->y , qt->y) + FX_Mul(qt->z , qt->z));
}

//--------------------------------------------------------------------------------------------
/**
 *	クォータニオンのノルム（ ||a|| ）
 *
 * @param[in]	qt	計算するクォータニオン構造体のポインタ
 */
//--------------------------------------------------------------------------------------------
fx32 GFL_QUAT_Norm(const GFL_QUATERNION *qt)
{
	return FX_Sqrt(GFL_QUAT_NormSqr(qt));
}

//--------------------------------------------------------------------------------------------
/**
 *	クォータニオンを正規化する
 *
 * @param[in/out]	qt	正規化するクォータニオン構造体のポインタ
 */
//--------------------------------------------------------------------------------------------
void GFL_QUAT_Normarize(GFL_QUATERNION *qt)
{
	fx32	norm;

	norm=GFL_QUAT_Norm(qt);
	GFL_QUAT_DivReal(qt,qt,norm);
}

//--------------------------------------------------------------------------------------------
/**
 *	各軸の回転量からクォータニオンを生成する
 *
 * @param[out]	qt		生成したクォータニオンを格納する構造体へのポインタ
 * @param[in]	rot_x	X軸の回転量
 * @param[in]	rot_y	Y軸の回転量
 * @param[in]	rot_z	Z軸の回転量
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
	fx32	sss=FX_SinIdx(rot/2);

	qt->w = FX_CosIdx(rot/2);
	qt->x = FX_Mul(a->x,sss);
	qt->y = FX_Mul(a->y,sss);
	qt->z = FX_Mul(a->z,sss);
}

//--------------------------------------------------------------------------------------------
/**
 *	指定された2点に直行するベクトルを回転軸とするクォータニオンを生成する
 *
 * @param[out]	qt		生成したクォータニオンを格納する構造体へのポインタ
 * @param[in]	pos1_x	指定点１のX座標（0〜256）
 * @param[in]	pos1_y	指定点１のY座標（0〜192）
 * @param[in]	pos2_x	指定点２のX座標（0〜256）
 * @param[in]	pos2_y	指定点２のY座標（0〜192）
 * @param[in]	radius	回転軸の半径
 */
//--------------------------------------------------------------------------------------------
void	GFL_QUAT_MakeQuaternionPos(GFL_QUATERNION *qt,u32 pos1_x,u32 pos1_y,u32 pos2_x,u32 pos2_y,fx32 radius)
{
	GFL_QUATERNION	p1,p2,a,d;
	fx32			s,t,tdiv;

	if((pos1_x==pos2_x)&&(pos1_y==pos2_y)){
		GFL_QUAT_Identity(qt);
		return;
	}

	p1.w=0;
	p1.x=FX_Mul(FX32_ONE*2,(pos1_x<<FX32_SHIFT));
	p1.x-=SCREEN_WIDTH;
	p1.x=FX_Div(p1.x,SCREEN_WIDTH);
	p1.y=FX_Mul(FX32_ONE*2,(pos1_y<<FX32_SHIFT));
	p1.y=SCREEN_HEIGHT-p1.y;
	p1.y=FX_Div(p1.y,SCREEN_HEIGHT);
	p1.z=CalcPosZ(p1.x,p1.y,radius);
	p2.w=0;
	p2.x=FX_Mul(FX32_ONE*2,(pos2_x<<FX32_SHIFT));
	p2.x-=SCREEN_WIDTH;
	p2.x=FX_Div(p2.x,SCREEN_WIDTH);
	p2.y=FX_Mul(FX32_ONE*2,(pos2_y<<FX32_SHIFT));
	p2.y=SCREEN_HEIGHT-p2.y;
	p2.y=FX_Div(p2.y,SCREEN_HEIGHT);
	p2.z=CalcPosZ(p2.x,p2.y,radius);

	GFL_QUAT_Mul(&a,&p1,&p2);
	a.w=0;
	s=GFL_QUAT_Norm(&a);
	GFL_QUAT_DivReal(&a,&a,s);

	GFL_QUAT_Sub(&d,&p1,&p2);
	t=GFL_QUAT_Norm(&d);
	tdiv=FX_Mul(FX32_ONE*2,radius);
	tdiv=FX_Mul(tdiv,FX_InvSqrt(FX32_ONE*2));
	t=FX_Div(t,tdiv);
	if(t>FX32_ONE){
		t=FX32_ONE;
	}

	qt->w=FX_CosIdx(FX_AsinIdx(t));
	qt->x=FX_Mul(a.x,t);
	qt->y=FX_Mul(a.y,t);
	qt->z=FX_Mul(a.z,t);
}

static	fx32	CalcPosZ(fx32 x,fx32 y,fx32 radius)
{
	fx32	posz;
	fx32	d_sqr,d;

	d_sqr=FX_Sqrt(x)+FX_Sqrt(y);
	d=FX_Sqrt(d_sqr);
	if(d<radius){
		posz=FX_Sqrt(FX_Mul(FX32_ONE*2,FX_Sqrt(radius))-d_sqr);
	}
	else{
		posz=FX_Div(FX_Sqrt(radius),d);
	}
	return posz;
}

//--------------------------------------------------------------------------------------------
/**
 *	クォータニオンから回転行列を生成する
 *
 * @param[out]	mtx	生成する回転行列構造体へのポインタ
 * @param[in]	qt	元になるクォータニオン構造体のポインタ
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

