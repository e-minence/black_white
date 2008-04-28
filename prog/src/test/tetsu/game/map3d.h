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

extern void	Draw3Dmap( SCENE_MAP* sceneMap, GFL_G3D_CAMERA* g3Dcamera );

enum {
	EXMAPOBJ_WALL = 0,
	EXMAPOBJ_TOWER_RED,
	EXMAPOBJ_TOWER_BLUE,
	EXMAPOBJ_TOWER_GREEN,
	EXMAPOBJ_TOWER_YELLOW,
};

extern BOOL	CheckGroundOutRange( const VecFx32* pos );
extern BOOL	GetGroundPlaneData( const VecFx32* pos, VecFx32* vecN, fx32* valD );
extern void	GetGroundPlaneVecN( const VecFx32* pos, VecFx32* vecN );
extern void	GetGroundPlaneHeight( const VecFx32* pos, fx32* height );
extern void	GetGroundMoveVec( const VecFx32* pos, const VecFx32* vecDir, VecFx32* vecMove );
extern void	GetGroundMovePos( const VecFx32* posNow, const VecFx32* vecMoveXZ, VecFx32* posNext );

extern void	SetMapGroundUp( VecFx32* pos );
extern void	SetMapGroundDown( VecFx32* pos );

extern BOOL GetRayPosOnMap( const VecFx32* posRay, const VecFx32* vecRay, VecFx32* dst );


