//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
typedef struct _SCENE_MAP		SCENE_MAP;

extern SCENE_MAP*	Create3Dmap( GFL_G3D_SCENE* g3Dscene, HEAPID heapID );
extern void			Delete3Dmap( SCENE_MAP* sceneMap );
extern int			AddObject3Dmap( SCENE_MAP* sceneMap, int objID, VecFx32* trans );
extern void			RemoveObject3Dmap( SCENE_MAP* sceneMap, int mapobjID );
extern void			Set3DmapDrawSw( SCENE_MAP* sceneMap, int mapobjID, BOOL* sw );
extern void			Get3DmapDrawSw( SCENE_MAP* sceneMap, int mapobjID, BOOL* sw );
extern BOOL			Get3DmapAttr( SCENE_MAP* sceneMap, VecFx32* pos, u16* attr );

enum {
	EXMAPOBJ_WALL = 0,
	EXMAPOBJ_WALL1,
	EXMAPOBJ_WALL2,
	EXMAPOBJ_WALL3,
	EXMAPOBJ_WALL4,
};

//------------------------------------------------------------------
/**
 * @brief		�}�b�v��ł̃x�N�g���ړ��i�d�͂���j
 */
//------------------------------------------------------------------
typedef struct _CALC_PH_MV CALC_PH_MV; 

#define PHMV_FULL_ABSORB	(0xffffffff)
typedef struct {
	fx32	gravityMove;		//�d��:�ړ��p(�����\�ɂ��邽�ߋ󒆂Ƃ͕���)
	fx32	gravityFall;		//�d��:�󒆗p
	u16		planeMarginTheta;	//���ʂƂ݂Ȃ��X��
	fx32	absorbVal;			//�����͋z���l(PHMV_FULL_ABSORB	= �����Ȃ�)

}PHMV_SETUP;

extern CALC_PH_MV*	CreateCalcPhisicsMoving( HEAPID heapID, PHMV_SETUP* setup );
extern void			DeleteCalcPhisicsMoving( CALC_PH_MV* calcPHMV );
extern void			SetAbsorbPHMV( CALC_PH_MV* calcPHMV, fx32 absorbVal );
extern BOOL			CheckMoveEndPHMV( CALC_PH_MV* calcPHMV );
extern BOOL			CheckMoveSpeedPHMV( CALC_PH_MV* calcPHMV );
extern BOOL			CheckOnFloorPHMV( CALC_PH_MV* calcPHMV, VecFx32* pos );
extern BOOL			CheckGravitySpeedPHMV( CALC_PH_MV* calcPHMV );
extern void			StartMovePHMV( CALC_PH_MV* calcPHMV, 
						VecFx32* posStart, VecFx32* dirXZ, fx32 speed, u16 theta );
extern void			ResetMovePHMV( CALC_PH_MV* calcPHMV );
extern BOOL			CalcMovePHMV( CALC_PH_MV* calcPHMV, VecFx32* posNow );

