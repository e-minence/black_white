//============================================================================================
/**
 *	@file	field_camera.h
 *	@brief
 */
//============================================================================================
#pragma once

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _FIELD_CAMERA	FIELD_CAMERA;


//------------------------------------------------------------------
//	�����E���C���E����
//------------------------------------------------------------------
extern FIELD_CAMERA* FIELD_CAMERA_Create(FIELD_MAIN_WORK * fieldWork,
		GFL_G3D_CAMERA * cam, HEAPID heapID);
extern void				FIELD_CAMERA_Delete( FIELD_CAMERA* camera );
extern void				FIELD_CAMERA_Main( FIELD_CAMERA* camera);

extern void FIELD_CAMERA_DEBUG_Control( FIELD_CAMERA * camera, int key);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_CAMERA_SetNear(FIELD_CAMERA * camera, fx32 near);
extern fx32 FIELD_CAMERA_GetNear(const FIELD_CAMERA * camera);
extern void FIELD_CAMERA_SetFar(FIELD_CAMERA * camera, fx32 far);
extern fx32 FIELD_CAMERA_GetFar(const FIELD_CAMERA * camera);

//------------------------------------------------------------------
/**
 * @brief	�J�����ʒu�̎擾
 * @param	camera		FIELD�J��������|�C���^
 * @param	pos			�J�����ʒu���󂯎��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void	FIELD_CAMERA_GetPos( const FIELD_CAMERA* camera, VecFx32* pos );

//------------------------------------------------------------------
/**
 * @brief	�J�����ʒu�̎擾
 * @param	camera		FIELD�J��������|�C���^
 * @param	pos			�J�����ʒu��n��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void	FIELD_CAMERA_SetPos( FIELD_CAMERA* camera, const VecFx32* pos );

//------------------------------------------------------------------
//	�ȉ��̓��l�[�������\��̊֐��Q
//------------------------------------------------------------------
extern void				FLD_SetCameraDirection( FIELD_CAMERA* camera, u16* direction );
extern void				FLD_GetCameraDirection( FIELD_CAMERA* camera, u16* direction );

extern void	FLD_SetCameraLength( FIELD_CAMERA *camera, u16 leng );
extern void	FLD_GetCameraLength( FIELD_CAMERA *camera, u16 *leng );
extern void	FLD_SetCameraHeight( FIELD_CAMERA *camera, fx32 height );
extern void	FLD_GetCameraHeight( FIELD_CAMERA *camera, fx32 *height );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern const GFL_G3D_CAMERA * FIELD_CAMERA_GetCameraPtr(const FIELD_CAMERA * camera);

extern void FIELD_CAMERA_SetNormalCameraMode(FIELD_CAMERA * camera, const VecFx32 * watch_pos);
extern const void FIELD_CAMERA_InitPositionWatcher(FIELD_CAMERA * camera, const VecFx32 * watch_pos);
extern const void FIELD_CAMERA_InitOffsetWatcher(FIELD_CAMERA * camera, const VecFx32 * watch_ofs);

