//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
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




