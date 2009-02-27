
//============================================================================================
/**
 * @file	btlv_camera.h
 * @brief	�퓬��ʃJ��������
 * @author	soga
 * @date	2008.11.20
 */
//============================================================================================

#ifndef	__BTLV_CAMERA_H_
#define	__BTLV_CAMERA_H_

typedef struct _BTLV_CAMERA_WORK BTLV_CAMERA_WORK;

extern	BTLV_CAMERA_WORK	*BTLV_CAMERA_Init( GFL_TCBSYS *tcb_sys, HEAPID heapID );
extern	void				BTLV_CAMERA_Exit( BTLV_CAMERA_WORK *bcw );
extern	void				BTLV_CAMERA_Main( BTLV_CAMERA_WORK *bcw );
extern	void				BTLV_CAMERA_MoveCameraPosition( BTLV_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target );
extern	void				BTLV_CAMERA_MoveCameraAngle( BTLV_CAMERA_WORK *bcw, int phi, int theta );
extern	void				BTLV_CAMERA_MoveCameraInterpolation( BTLV_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target, int flame, int brake );
extern	void				BTLV_CAMERA_GetCameraPosition( BTLV_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target );
extern	void				BTLV_CAMERA_GetDefaultCameraPosition( VecFx32 *pos, VecFx32 *target );
extern	BOOL				BTLV_CAMERA_CheckExecute( BTLV_CAMERA_WORK *bcw );

#endif	//__BTLV_CAMERA_H_
