//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
//�x�N�g���̃X�J���[��Z�ia * b�j
void	inline VEC_Mult( const fx32 a, const VecFx32* b, VecFx32* dst ) 
{
	dst->x = FX_Mul( a, b->x );
	dst->y = FX_Mul( a, b->y );
	dst->z = FX_Mul( a, b->z );
}

//�x�N�g���̃X�J���[���Z�ib / a�j
void	inline VEC_Divt( const fx32 a, const VecFx32* b, VecFx32* dst ) 
{
	GF_ASSERT( a );

	dst->x = FX_Div( b->x, a );
	dst->y = FX_Div( b->y, a );
	dst->z = FX_Div( b->z, a );
}

//�x�N�g���̓��e���Z�ia��b�֓��e�j
void	inline VEC_Proj( const VecFx32* a, const VecFx32* b, VecFx32* dst ) 
{
	// �a�͐��K���ς݂ł��邱��
	// �@���`���a�ɓ��e����̂́A�a�͐��K���ς݂ł��邱�Ƃ�O��� (�`�E�a)�a
	// �@�@���m�ɂ�(�`�E�a)�a�^(�a�̒���)
	fx32 scalar = VEC_DotProduct( a, b );
	VEC_Mult( scalar, b, dst );
}

//�x�N�g���̔��ˉ��Z�ia��b���ɑ΂��Ĕ��ˁj
void	inline VEC_Refrect( const VecFx32* a, const VecFx32* b, VecFx32* dst ) 
{
	// �a�͐��K���ς݂ł��邱��
	VecFx32 vecP;

	VEC_Proj( a, b, &vecP ); 
	VEC_Set( &vecP, -vecP.x, -vecP.y, -vecP.z );	//�������ɕϊ�
	VEC_MultAdd( 2 * FX32_ONE, &vecP, a, dst );
}

//------------------------------------------------------------------
/**
 * @brief		�}�b�v��ł̃x�N�g���ړ��i�d�͂���j
 */
//------------------------------------------------------------------
typedef struct _CALC_PH_MV CALC_PH_MV; 
typedef void	(GFL_PH_GROUND_VECN_FUNC)( const VecFx32* pos, VecFx32* vecN );
typedef void	(GFL_PH_GROUND_HEIGHT_FUNC)( const VecFx32* pos, fx32* height );

#define PHMV_FULL_ABSORB	(0xffffffff)
typedef struct {
	fx32	gravityMove;		//�d��:�ړ��p(�����\�ɂ��邽�ߋ󒆂Ƃ͕���)
	fx32	gravityFall;		//�d��:�󒆗p
	u16		planeMarginTheta;	//���ʂƂ݂Ȃ��X��
	fx32	absorbVal;			//�����͋z���l(PHMV_FULL_ABSORB	= �����Ȃ�)
	u16		enableMoveTheta;	//�ړ��\�Ȋp�x

	GFL_PH_GROUND_VECN_FUNC*	getGroundVecN_func;		//�n�`�̖@���x�N�g���擾�֐�
	GFL_PH_GROUND_HEIGHT_FUNC*	getGroundHeight_func;	//�n�`�̍����擾�֐�

}PHMV_SETUP;

extern CALC_PH_MV*	CreateCalcPhisicsMoving( HEAPID heapID, PHMV_SETUP* setup );
extern void			DeleteCalcPhisicsMoving( CALC_PH_MV* calcPHMV );
extern void			SetAbsorbPHMV( CALC_PH_MV* calcPHMV, fx32 absorbVal );
extern void			SetMoveVecPHMV( CALC_PH_MV* calcPHMV, VecFx32* dirXZ );
extern void			GetMoveDirPHMV( CALC_PH_MV* calcPHMV, VecFx32* vecDir );
extern void			GetMoveVecPHMV( CALC_PH_MV* calcPHMV, VecFx32* vecMove );
extern fx32			GetMoveSpeedPHMV( CALC_PH_MV* calcPHMV );
extern void			SetMoveSpeedPHMV( CALC_PH_MV* calcPHMV, fx32 speed );
extern void			AddMoveSpeedPHMV( CALC_PH_MV* calcPHMV, fx32 speed );
extern BOOL			CheckOnFloorPHMV( CALC_PH_MV* calcPHMV, VecFx32* pos );
extern BOOL			CheckGroundGravityPHMV( CALC_PH_MV* calcPHMV, VecFx32* pos );
extern void			StartMovePHMV( CALC_PH_MV* calcPHMV, VecFx32* dirXZ, fx32 speed, u16 theta );
extern void			ResetMovePHMV( CALC_PH_MV* calcPHMV );
extern BOOL			CalcMovePHMV( CALC_PH_MV* calcPHMV, VecFx32* posNow );




