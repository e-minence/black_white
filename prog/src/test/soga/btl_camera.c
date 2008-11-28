
//============================================================================================
/**
 * @file	btl_camera.c
 * @brief	戦闘画面カメラ制御
 * @author	soga
 * @date	2008.11.20
 */
//============================================================================================

#include <gflib.h>

#include "btl_efftool.h"
#include "btl_camera.h"
#include "btl_camera_def.h"

//============================================================================================
/**
 *	構造体宣言
 */
//============================================================================================

struct _BTL_CAMERA_WORK
{
	GFL_TCBSYS			*tcb_sys;
	GFL_G3D_CAMERA		*camera;
	u16					phi;
	u16					theta;
	fx32				radius;
	int					move_flag;
	VecFx32				move_pos;
	VecFx32				move_target;
	VecFx32				vec_pos;
	VecFx32				vec_target;
	int					brake_frame;
	HEAPID				heapID;
};

//============================================================================================
/**
 *	プロトタイプ宣言
 */
//============================================================================================

BTL_CAMERA_WORK	*BTL_CAMERA_Init( GFL_TCBSYS *tcb_sys, HEAPID heapID );
void			BTL_CAMERA_Exit( BTL_CAMERA_WORK *bcw );
void			BTL_CAMERA_Main( BTL_CAMERA_WORK *bcw );
void			BTL_CAMERA_MoveCameraPosition( BTL_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target );
void			BTL_CAMERA_MoveCameraAngle( BTL_CAMERA_WORK *bcw, int phi, int theta );
void			BTL_CAMERA_MoveCameraInterpolation( BTL_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target, int flame, int brake );
void			BTL_CAMERA_GetCameraPosition( BTL_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target );
void			BTL_CAMERA_GetDefaultCameraPosition( VecFx32 *pos, VecFx32 *target );
BOOL			BTL_CAMERA_CheckExecute( BTL_CAMERA_WORK *bcw );

static	void	BTL_CAMERA_Move( BTL_CAMERA_WORK *bcw );
static	void	BTL_CAMERA_UpdateCameraAngle( BTL_CAMERA_WORK *bcw );

//============================================================================================
/**
 *	カメラ初期位置
 */
//============================================================================================

static const VecFx32 cam_pos = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 7.8f ), FX_F32_TO_FX32( 21.0f ) };
static const VecFx32 cam_target = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 2.6f ), FX_F32_TO_FX32( 0.0f ) };
static const VecFx32 cam_up = { 0, FX32_ONE, 0 };

//============================================================================================
/**
 *	システム初期化
 *
 * @param[in]	tcb_sys	システム内で使用するTCBSYS構造体へのポインタ
 * @param[in]	heapID	ヒープID
 */
//============================================================================================
BTL_CAMERA_WORK	*BTL_CAMERA_Init( GFL_TCBSYS *tcb_sys, HEAPID heapID )
{
	BTL_CAMERA_WORK *bcw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTL_CAMERA_WORK ) );

	bcw->heapID = heapID;
	bcw->tcb_sys = tcb_sys;

	bcw->camera = GFL_G3D_CAMERA_Create( GFL_G3D_PRJPERS, 
										 FX32_SIN13,
										 FX32_COS13,
										 FX_F32_TO_FX32( 1.33f ),
										 NULL,
										 0,
										 FX32_ONE * 180,
										 NULL,
										 &cam_pos,
										 &cam_up,
										 &cam_target,
										 heapID );

	BTL_CAMERA_UpdateCameraAngle( bcw );

	GFL_G3D_CAMERA_Switching( bcw->camera );

	return bcw;
}

//============================================================================================
/**
 *	システム終了
 *
 * @param[in]	bcw	BTL_CAMERA管理ワークへのポインタ
 */
//============================================================================================
void	BTL_CAMERA_Exit( BTL_CAMERA_WORK *bcw )
{
	GFL_G3D_CAMERA_Delete( bcw->camera );
	GFL_HEAP_FreeMemory( bcw );
}

//============================================================================================
/**
 *	システムメイン
 *
 * @param[in]	bcw	BTL_CAMERA管理ワークへのポインタ
 */
//============================================================================================
void	BTL_CAMERA_Main( BTL_CAMERA_WORK *bcw )
{
	BTL_CAMERA_Move( bcw );
}

//============================================================================================
/**
 *	カメラ移動（位置、ターゲット指定）
 *
 * @param[in]	bcw		BTL_CAMERA管理ワークへのポインタ
 * @param[in]	pos		移動先カメラ位置
 * @param[in]	target	移動先カメラターゲット
 */
//============================================================================================
void	BTL_CAMERA_MoveCameraPosition( BTL_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target )
{
	if( pos != NULL ){
		GFL_G3D_CAMERA_SetPos( bcw->camera, pos );
	}
	if( target != NULL ){
		GFL_G3D_CAMERA_SetTarget( bcw->camera, target );
	}
	GFL_G3D_CAMERA_Switching( bcw->camera );
	BTL_CAMERA_UpdateCameraAngle( bcw );
}

//============================================================================================
/**
 *	カメラ移動（角度指定）
 *
 * @param[in]	bcw		BTL_CAMERA管理ワークへのポインタ
 * @param[in]	phi		X軸方向の移動量
 * @param[in]	theta	Y軸方向の移動量
 */
//============================================================================================
void	BTL_CAMERA_MoveCameraAngle( BTL_CAMERA_WORK *bcw, int phi, int theta )
{
	VecFx32	pos, target;

	GFL_G3D_CAMERA_GetTarget( bcw->camera, &target );

	bcw->phi += phi;
	bcw->theta += theta;

//	if( now_phi <= -PHI_MAX ){
//		now_phi = -PHI_MAX;
//	}
//	if( now_phi >= PHI_MAX ){
//		now_phi = PHI_MAX;
//	}

	//phiとthetaとscaleからcamPosを計算
	pos.x = FX_MUL( FX_CosIdx( bcw->theta ), FX_CosIdx( bcw->phi ) );
	pos.y = FX_SinIdx( bcw->phi );
	pos.z = FX_MUL( FX_SinIdx( bcw->theta ), FX_CosIdx( bcw->phi ) );
	pos.x = FX_MUL( pos.x, bcw->radius );
	pos.y = FX_MUL( pos.y, bcw->radius );
	pos.z = FX_MUL( pos.z, bcw->radius );

	pos.x += target.x;
	pos.y += target.y;
	pos.z += target.z;

	GFL_G3D_CAMERA_SetPos( bcw->camera, &pos );
	GFL_G3D_CAMERA_Switching( bcw->camera );
}

//============================================================================================
/**
 *	カメラ移動（移動先を指定して、間を補間する）
 *
 * @param[in]	bcw		BTL_CAMERA管理ワークへのポインタ
 * @param[in]	pos		移動先カメラ位置
 * @param[in]	target	移動先カメラターゲット
 * @param[in]	flame	移動フレーム数
 * @param[in]	brake	移動にブレーキをかけるフレーム数
 */
//============================================================================================
void	BTL_CAMERA_MoveCameraInterpolation( BTL_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target, int flame, int brake )
{
	VecFx32	now_pos, now_target;

	GFL_G3D_CAMERA_GetPos( bcw->camera, &now_pos );
	GFL_G3D_CAMERA_GetTarget( bcw->camera, &now_target );

	bcw->brake_frame = brake;

	if( pos != NULL ){
		bcw->move_pos.x = pos->x;
		bcw->move_pos.y = pos->y;
		bcw->move_pos.z = pos->z;
		BTL_EFFTOOL_CalcMoveVector( &now_pos, pos, &bcw->vec_pos, FX32_CONST( flame ) );
		bcw->move_flag |= CAMERA_POS_MOVE_FLAG;
	}
	if( target != NULL ){
		bcw->move_target.x = target->x;
		bcw->move_target.y = target->y;
		bcw->move_target.z = target->z;
		BTL_EFFTOOL_CalcMoveVector( &now_target, target, &bcw->vec_target, FX32_CONST( flame ) );
		bcw->move_flag |= CAMERA_TARGET_MOVE_FLAG;
	}
}

//============================================================================================
/**
 *	カメラ位置を取得
 *
 * @param[in]	bcw		BTL_CAMERA管理ワークへのポインタ
 * @param[in]	pos		取得したカメラ位置を格納するワークへのポインタ
 * @param[in]	target	取得したカメラターゲットを格納するワークへのポインタ
 */
//============================================================================================
void	BTL_CAMERA_GetCameraPosition( BTL_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target )
{
	GFL_G3D_CAMERA_GetPos( bcw->camera, pos );
	GFL_G3D_CAMERA_GetTarget( bcw->camera, target );
}

//============================================================================================
/**
 *	カメラのデフォルト位置を取得
 *
 * @param[in]	pos		取得したカメラ位置を格納するワークへのポインタ
 * @param[in]	target	取得したカメラターゲットを格納するワークへのポインタ
 */
//============================================================================================
void	BTL_CAMERA_GetDefaultCameraPosition( VecFx32 *pos, VecFx32 *target )
{
	pos->x = cam_pos.x;
	pos->y = cam_pos.y;
	pos->z = cam_pos.z;

	target->x = cam_target.x;
	target->y = cam_target.y;
	target->z = cam_target.z;
}

//============================================================================================
/**
 *	カメラ移動が実行されているかチェック
 *
 * @param[in]	bcw		BTL_CAMERA管理ワークへのポインタ
 *
 * @retval	FALSE:移動していない　TRUE:移動中
 */
//============================================================================================
BOOL	BTL_CAMERA_CheckExecute( BTL_CAMERA_WORK *bcw )
{
	return ( bcw->move_flag != 0 );
}

//============================================================================================
/**
 *	カメラ移動処理
 *
 * @param[in]	bcw		BTL_CAMERA管理ワークへのポインタ
 */
//============================================================================================
static	void	BTL_CAMERA_Move( BTL_CAMERA_WORK *bcw )
{
	BOOL ret = TRUE;
	VecFx32	pos, target;

	if( bcw->move_flag == 0 ) return;

	if( bcw->brake_frame ){
		bcw->brake_frame--;
		if( bcw->brake_frame == 0 ){
			bcw->vec_pos.x = bcw->vec_pos.x >> 1;
			bcw->vec_pos.y = bcw->vec_pos.y >> 1;
			bcw->vec_pos.z = bcw->vec_pos.z >> 1;
			bcw->vec_target.x = bcw->vec_target.x >> 1;
			bcw->vec_target.y = bcw->vec_target.y >> 1;
			bcw->vec_target.z = bcw->vec_target.z >> 1;
		}
	}

	GFL_G3D_CAMERA_GetPos( bcw->camera, &pos );
	GFL_G3D_CAMERA_GetTarget( bcw->camera, &target );

	if( bcw->move_flag & CAMERA_POS_MOVE_FLAG ){
		BTL_EFFTOOL_CheckMove( &pos.x, &bcw->vec_pos.x, &bcw->move_pos.x, &ret );
		BTL_EFFTOOL_CheckMove( &pos.y, &bcw->vec_pos.y, &bcw->move_pos.y, &ret );
		BTL_EFFTOOL_CheckMove( &pos.z, &bcw->vec_pos.z, &bcw->move_pos.z, &ret );
		if( ret == TRUE ){
			bcw->move_flag &= CAMERA_POS_MOVE_FLAG_OFF;
		}
	}
	if( bcw->move_flag & CAMERA_TARGET_MOVE_FLAG ){
		BTL_EFFTOOL_CheckMove( &target.x, &bcw->vec_target.x, &bcw->move_target.x, &ret );
		BTL_EFFTOOL_CheckMove( &target.y, &bcw->vec_target.y, &bcw->move_target.y, &ret );
		BTL_EFFTOOL_CheckMove( &target.z, &bcw->vec_target.z, &bcw->move_target.z, &ret );
		if( ret == TRUE ){
			bcw->move_flag &= CAMERA_TARGET_MOVE_FLAG_OFF;
		}
	}
	GFL_G3D_CAMERA_SetPos( bcw->camera, &pos );
	GFL_G3D_CAMERA_SetTarget( bcw->camera, &target );
	GFL_G3D_CAMERA_Switching( bcw->camera );
	BTL_CAMERA_UpdateCameraAngle( bcw );
}

//============================================================================================
/**
 *	現在のカメラ位置とターゲット位置からカメラ角度を計算して更新
 *
 * @param[in]	bcw		BTL_CAMERA管理ワークへのポインタ
 */
//============================================================================================
static	void	BTL_CAMERA_UpdateCameraAngle( BTL_CAMERA_WORK *bcw )
{
	VecFx32	pos, target;

	GFL_G3D_CAMERA_GetPos( bcw->camera, &pos );
	GFL_G3D_CAMERA_GetTarget( bcw->camera, &target );

	pos.x -= target.x;
	pos.y -= target.y;
	pos.z -= target.z;

	bcw->phi    = FX_Atan2Idx( pos.y, pos.z );
	bcw->theta  = FX_Atan2Idx( pos.z, pos.x );
	bcw->radius = VEC_Mag( &pos );
}

//カメラのポジションから、φ、θを求める式（参考のためにコメントで残しておく）
#if 0
	cw->phi		= FX_Atan2Idx( cam_pos.y, cam_pos.z );
	cw->theta	= FX_Atan2Idx( cam_pos.y, cam_pos.x );
	cw->scale	= VEC_Mag( &cam_pos );
//======================================================================
//	カメラの制御
//======================================================================
static	void	MoveCamera( SOGA_WORK *wk )
{
	CAMERA_WORK	*cw = &wk->cw;
	int pad = GFL_UI_KEY_GetCont();
	VecFx32	pos,ofsx,ofsz;
	fx32	xofs=0,yofs=0,zofs=0;		

	if( pad & PAD_BUTTON_Y ){
		if( pad & PAD_KEY_LEFT ){
			cw->theta -= ROTATE_SPEED;
		}
		if( pad & PAD_KEY_RIGHT ){
			cw->theta += ROTATE_SPEED;
		}
		if( pad & PAD_KEY_UP ){
			cw->phi -= ROTATE_SPEED;
			if( cw->phi <= -PHI_MAX ){
				cw->phi = -PHI_MAX;
			}
		}
		if( pad & PAD_KEY_DOWN ){
			cw->phi += ROTATE_SPEED;
			if( cw->phi >= PHI_MAX ){
				cw->phi = PHI_MAX;
			}
		}
	}
	else{
		if( pad & PAD_KEY_LEFT ){
			xofs = -MOVE_SPEED;
		}
		if( pad & PAD_KEY_RIGHT ){
			xofs = MOVE_SPEED;
		}
		if( pad & PAD_KEY_UP ){
			yofs = MOVE_SPEED;
		}
		if( pad & PAD_KEY_DOWN ){
			yofs = -MOVE_SPEED;
		}
		if( pad & PAD_BUTTON_X ){
			zofs = MOVE_SPEED;
		}
		if( pad & PAD_BUTTON_B ){
			zofs = -MOVE_SPEED;
		}
		if( wk->mw.pos_x ){
			xofs = MOVE_SPEED * wk->mw.pos_x;
			wk->mw.pos_x = 0;
		}
		if( wk->mw.pos_y ){
			yofs = MOVE_SPEED * wk->mw.pos_y;
			wk->mw.pos_y = 0;
		}
		if( wk->mw.distance ){
			zofs = MOVE_SPEED * (wk->mw.distance / 50);
			wk->mw.distance = 0;
		}
		pos.x = cw->camPos.x - cw->target.x;
		pos.y = 0;
		pos.z = cw->camPos.z - cw->target.z;
		VEC_Normalize( &pos, &pos );

		ofsx.x = FX_MUL( pos.z, xofs );
		ofsx.y = 0;
		ofsx.z = -FX_MUL( pos.x, xofs );

		ofsz.x = -FX_MUL( pos.x, zofs );
		ofsz.y = yofs;
		ofsz.z = -FX_MUL( pos.z, zofs );

		cw->target.x += ofsx.x + ofsz.x;
		cw->target.y += ofsx.y + ofsz.y;
		cw->target.z += ofsx.z + ofsz.z;
	}
	if( pad & PAD_BUTTON_L ){
		if( cw->scale > 0 ){
			cw->scale -= SCALE_SPEED;
		}
	}
	if( pad & PAD_BUTTON_R ){
		if( cw->scale < SCALE_MAX ){
			cw->scale += SCALE_SPEED;
		}
	}
	if( wk->mw.theta ){
		cw->theta -= wk->mw.theta * 32 ;
		wk->mw.theta = 0;
	}

	if( wk->mw.phi ){
		cw->phi -= wk->mw.phi * 32 ;
		if( cw->phi <= -PHI_MAX ){
			cw->phi = -PHI_MAX;
		}
		if( cw->phi >= PHI_MAX ){
			cw->phi = PHI_MAX;
		}
		wk->mw.phi = 0;
	}

	CalcCamera( cw );
}

//======================================================================
//	カメラ位置の計算
//======================================================================
static	void	CalcCamera( CAMERA_WORK *cw )
{
	MtxFx43	scale,trans;
	VecFx32	pos;

	//phiとthetaとscaleからcamPosを計算
	pos.x = FX_MUL( FX_CosIdx( cw->theta & 0xffff ), FX_CosIdx( cw->phi & 0xffff ) );
	pos.y = FX_SinIdx( cw->phi & 0xffff );
	pos.z = FX_MUL( FX_SinIdx( cw->theta & 0xffff ), FX_CosIdx( cw->phi & 0xffff ) );
	MTX_Scale43( &scale, cw->scale, cw->scale, cw->scale );
	MTX_MultVec43( &pos, &scale, &pos );
	MTX_Identity43( &trans );
//	MTX_TransApply43( &trans, &trans, cw->target.x,	cw->target.y, cw->target.z );
	MTX_MultVec43( &pos, &trans, &cw->camPos );
}
#endif
