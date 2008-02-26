//============================================================================================
/**
 * @file	calc_ph.c
 * @brief	�������Z�n�֐�
 */
//============================================================================================
#include "gflib.h"

#include "calc_ph.h"

//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
 *
 * @brief		�}�b�v��ł̃x�N�g���ړ��i�d�͂���j
 *
 */
//------------------------------------------------------------------
struct _CALC_PH_MV {
	GFL_PH_GROUND_VECN_FUNC*	getGroundVecN_func;		//�n�`�̖@���x�N�g���擾�֐�
	GFL_PH_GROUND_HEIGHT_FUNC*	getGroundHeight_func;	//�n�`�̍����擾�֐�

	fx32	gravityMove;
	fx32	gravityFall;
	fx32	planeMargin;
	fx32	absorbVal;
	u16		enableMoveTheta;

	VecFx32	vecDir;
	fx32	speed;
	u16		theta;

	VecFx32	vecSpeed;
	VecFx32	vecMove;
};

static void	inline VEC_Mult( const fx32 a, const VecFx32* b, VecFx32* dst ) 
{
	dst->x = FX_Mul( a, b->x );
	dst->y = FX_Mul( a, b->y );
	dst->z = FX_Mul( a, b->z );
}

static void	inline VEC_Divt( const fx32 a, const VecFx32* b, VecFx32* dst ) 
{
	GF_ASSERT( a );

	dst->x = FX_Div( b->x, a );
	dst->y = FX_Div( b->y, a );
	dst->z = FX_Div( b->z, a );
}

static void	inline VEC_Proj( const VecFx32* a, const VecFx32* b, VecFx32* dst ) 
{
	// �a�͐��K���ς݂ł��邱��
	// �@���`���a�ɓ��e����̂́A�a�͐��K���ς݂ł��邱�Ƃ�O��� (�`�E�a)�a
	// �@�@���m�ɂ�(�`�E�a)�a�^(�a�̒���)
	fx32 scalar = VEC_DotProduct( a, b );
	VEC_Mult( scalar, b, dst );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL	CheckOnGround( CALC_PH_MV* calcPHMV, VecFx32* pos, fx32* y )
{
	fx32 groundHeight;

	//GetGroundPlaneHeight( pos, &groundHeight );
	calcPHMV->getGroundHeight_func( pos, &groundHeight );
	
	if( pos->y <= groundHeight + (0.001f * FX32_ONE) ){	//0.1f�͌덷���e��
		if( y != NULL ){
			*y = groundHeight;
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

static void	GetGroundGravity( CALC_PH_MV* calcPHMV,
		const VecFx32* pos, VecFx32* vecG, const fx32 gravity )
{
	VecFx32 vecN, vecH, vecV;
	VecFx32 vecGravity = { 0, -FX32_ONE, 0 };

	//���ʂ̖@���x�N�g���ɂ��n�ʂɐ����ŎΖʂɕ��s�ȃx�N�g�����Z�o
	//GetGroundPlaneVecN( pos, &vecN );				//���ʂ̖@�����擾
	calcPHMV->getGroundVecN_func( pos, &vecN );		//���ʂ̖@�����擾
	VEC_CrossProduct( &vecN, &vecGravity, &vecH );	//���ʏ�̐����x�N�g���Z�o
	VEC_CrossProduct( &vecN, &vecH, &vecV );		//���ʏ�̎Ζʃx�N�g���Z�o
	if( vecV.y > 0 ){
		//�������i�V�����j�ւ̃x�N�g�����o�Ă��܂����ꍇ�������i�n�����j�֏C��
		VEC_Set( &vecV, -vecV.x, -vecV.y, -vecV.z );
	}
	VEC_Normalize( &vecV, &vecV );

	//�d�̓x�N�g���𓊉e���A������ׂ��d�̓x�N�g�����Z�o
	vecGravity.y = -gravity;
	VEC_Proj( &vecGravity, &vecV, vecG ); 
}

static void	GetGroundVec( CALC_PH_MV* calcPHMV,
		const VecFx32* pos, const VecFx32* vecDir, VecFx32* vecGround )
{
	VecFx32 vecN, vecH, vecV;

	//���ʂ̖@���x�N�g���ɂ��ړ��x�N�g���ɐ����ŎΖʂɕ��s�ȃx�N�g�����Z�o
	//GetGroundPlaneVecN( pos, &vecN );				//���ʂ̖@�����擾
	calcPHMV->getGroundVecN_func( pos, &vecN );		//���ʂ̖@�����擾
	VEC_CrossProduct( &vecN, vecDir, &vecH );		//���ʏ�̐����x�N�g���Z�o
	VEC_CrossProduct( &vecN, &vecH, &vecV );		//���ʏ�̎Ζʃx�N�g���Z�o

	if( VEC_DotProduct( &vecV, vecDir ) < 0 ){
		//�t�����ւ̃x�N�g�����o�Ă��܂����ꍇ�C��
		VEC_Set( vecGround, -vecV.x, -vecV.y, -vecV.z );
	} else {
		VEC_Set( vecGround, vecV.x, vecV.y, vecV.z );
	}
	VEC_Normalize( vecGround, vecGround );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
CALC_PH_MV*	CreateCalcPhisicsMoving( HEAPID heapID, PHMV_SETUP* setup )
{
	CALC_PH_MV*	calcPHMV = GFL_HEAP_AllocClearMemory( heapID, sizeof(CALC_PH_MV) );

	GF_ASSERT( setup->getGroundVecN_func );
	GF_ASSERT( setup->getGroundHeight_func );

	calcPHMV->getGroundVecN_func = setup->getGroundVecN_func;
	calcPHMV->getGroundHeight_func = setup->getGroundHeight_func;

	calcPHMV->gravityMove		= setup->gravityMove;
	calcPHMV->gravityFall		= setup->gravityFall;
	calcPHMV->planeMargin		= -FX_SinIdx( setup->planeMarginTheta );
	calcPHMV->absorbVal			= setup->absorbVal;
	calcPHMV->enableMoveTheta	= setup->enableMoveTheta;
	
	ResetMovePHMV( calcPHMV );
	return calcPHMV;
}

void DeleteCalcPhisicsMoving( CALC_PH_MV* calcPHMV )
{
	GFL_HEAP_FreeMemory( calcPHMV );
}

//------------------------------------------------------------------
void SetAbsorbPHMV( CALC_PH_MV* calcPHMV, fx32 absorbVal )
{
	calcPHMV->absorbVal = absorbVal;
}

//------------------------------------------------------------------
void SetMoveVecPHMV( CALC_PH_MV* calcPHMV, VecFx32* dirXZ )
{
	VecFx32 vecDir, vecMove;

	vecDir.x = FX_Mul( dirXZ->x, FX_CosIdx( calcPHMV->theta ) );
	vecDir.y = FX_SinIdx( calcPHMV->theta );
	vecDir.z = FX_Mul( dirXZ->z, FX_CosIdx( calcPHMV->theta ) );
	VEC_Normalize( &vecDir, &vecDir );
	VEC_Mult( calcPHMV->speed, &vecDir, &vecDir );

	VEC_Add( &vecDir, &calcPHMV->vecSpeed, &vecDir );
	VEC_Normalize( &vecDir, &vecDir );

	VEC_Mult( VEC_Mag( &calcPHMV->vecSpeed ), &vecDir, &calcPHMV->vecSpeed );
}

//------------------------------------------------------------------
void GetMoveDirPHMV( CALC_PH_MV* calcPHMV, VecFx32* vecDir )
{
	VEC_Normalize( &calcPHMV->vecSpeed, vecDir );
}

//------------------------------------------------------------------
void GetMoveVecPHMV( CALC_PH_MV* calcPHMV, VecFx32* vecMove )
{
	*vecMove = calcPHMV->vecMove;
}

//------------------------------------------------------------------
fx32 GetMoveSpeedPHMV( CALC_PH_MV* calcPHMV )
{
	return VEC_Mag( &calcPHMV->vecSpeed );
}

//------------------------------------------------------------------
void SetMoveSpeedPHMV( CALC_PH_MV* calcPHMV, fx32 speed )
{
	VecFx32 vecSpeed;

	VEC_Normalize( &calcPHMV->vecSpeed, &vecSpeed );
	VEC_Mult( speed, &vecSpeed, &calcPHMV->vecSpeed );
}
//------------------------------------------------------------------
void AddMoveSpeedPHMV( CALC_PH_MV* calcPHMV, fx32 speed )
{
	VecFx32 vecSpeed;
	fx32	valSpeed = VEC_Mag( &calcPHMV->vecSpeed ) + speed;

	VEC_Normalize( &calcPHMV->vecSpeed, &vecSpeed );
	VEC_Mult( valSpeed, &vecSpeed, &calcPHMV->vecSpeed );
}

//------------------------------------------------------------------
BOOL CheckOnFloorPHMV( CALC_PH_MV* calcPHMV, VecFx32* pos )
{
	return CheckOnGround( calcPHMV, pos, NULL );	//TRUE = �n��, FALSE = ��
}

//------------------------------------------------------------------
BOOL CheckGroundGravityPHMV( CALC_PH_MV* calcPHMV, VecFx32* pos )
{
	VecFx32 vecG;

	GetGroundGravity( calcPHMV, pos, &vecG, FX32_ONE );
	if( vecG.y < 0 ){
		return TRUE;	//��or�n��Ζ�(�d�͂ɂ��d�����������Ă�����)
	} else {
		return FALSE;	//�n�㕽��
	}
}

//------------------------------------------------------------------
void StartMovePHMV( CALC_PH_MV* calcPHMV, VecFx32* dirXZ, fx32 speed, u16 theta )
{
	calcPHMV->speed = speed;
	calcPHMV->theta = theta;

	calcPHMV->vecDir.x = FX_Mul( dirXZ->x, FX_CosIdx( calcPHMV->theta ) );
	calcPHMV->vecDir.y = FX_SinIdx( calcPHMV->theta );
	calcPHMV->vecDir.z = FX_Mul( dirXZ->z, FX_CosIdx( calcPHMV->theta ) );
	VEC_Normalize( &calcPHMV->vecDir, &calcPHMV->vecDir );

	VEC_Mult( calcPHMV->speed, &calcPHMV->vecDir, &calcPHMV->vecSpeed ); 
}

//------------------------------------------------------------------
void ResetMovePHMV( CALC_PH_MV* calcPHMV )
{
	calcPHMV->speed = 0;
	calcPHMV->theta = 0;

	calcPHMV->vecDir.x = 0;
	calcPHMV->vecDir.y = 0;
	calcPHMV->vecDir.z = 0;

	calcPHMV->vecSpeed.x = 0;
	calcPHMV->vecSpeed.y = 0;
	calcPHMV->vecSpeed.z = 0;
}

//------------------------------------------------------------------
BOOL	CalcMovePHMV( CALC_PH_MV* calcPHMV, VecFx32* posNow )
{
	VecFx32 vecMove, vecG, vecSpeed, posNext;
	VecFx32	vecSpeed_next;
	u16		thetaGround;

	if( !calcPHMV->vecDir.y ){
		//�����ړ�

		//�ړ������̒n�`�ɉ������P�ʃx�N�g���擾
		GetGroundVec( calcPHMV, posNow, &calcPHMV->vecSpeed, &vecMove );
		//�ړ��x�N�g���擾
		VEC_Mult( VEC_Mag( &calcPHMV->vecSpeed ), &vecMove, &calcPHMV->vecSpeed );
		//�\�z�ړ��ʒu�̌v�Z
		VEC_Add( posNow, &calcPHMV->vecSpeed, &posNext );
		GetGroundGravity( calcPHMV, &posNext, &vecG, calcPHMV->gravityMove );

		//���x�̕��ς��Z�o( V0+V1 / 2 )
		VEC_Add( &calcPHMV->vecSpeed, &vecG, &vecSpeed_next );
		VEC_Add( &calcPHMV->vecSpeed, &vecSpeed_next, &vecSpeed );
		VEC_Divt( 2 * FX32_ONE, &vecSpeed, &vecSpeed );

		calcPHMV->vecSpeed = vecSpeed_next;
		//OS_Printf("speed{ %d }\n ", VEC_Mag( &calcPHMV->vecSpeed ));

		//�ړ��ʒu�̌v�Z
		VEC_Add( posNow, &vecSpeed, &posNext );
		//GetGroundPlaneHeight( &posNext, &posNext.y );
		calcPHMV->getGroundHeight_func( &posNext, &posNext.y );
	} else {
		//�󒆈ړ�
		vecG.x = 0;
		vecG.y = -calcPHMV->gravityFall;
		vecG.z = 0;
		
		//���x�̕��ς��Z�o( V0+V1 / 2 )
		VEC_Add( &calcPHMV->vecSpeed, &vecG, &vecSpeed_next );
		VEC_Add( &calcPHMV->vecSpeed, &vecSpeed_next, &vecSpeed );
		VEC_Divt( 2 * FX32_ONE, &vecSpeed, &vecSpeed );

		calcPHMV->vecSpeed = vecSpeed_next;

		//�ړ��ʒu�̌v�Z
		VEC_Add( posNow, &vecSpeed, &posNext );
		CheckOnGround( calcPHMV, &posNext, &posNext.y );
	}
	VEC_Subtract( &posNext, posNow, &calcPHMV->vecMove );
	*posNow = posNext;

	return TRUE;
}




