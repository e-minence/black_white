
//============================================================================================
/**
 * @file	btlv_camera.c
 * @brief	�퓬��ʃJ��������
 * @author	soga
 * @date	2008.11.20
 */
//============================================================================================

#include <gflib.h>

#include "btlv_effect.h"

//============================================================================================
/**
 *	�萔�錾
 */
//============================================================================================

#define	PHI_MAX				( ( 65536 / 360 ) * 89 )

#define	CAMERA_POS_MOVE_FLAG		( 0x00000001 )
#define	CAMERA_TARGET_MOVE_FLAG		( 0x00000002 )

#define	CAMERA_POS_MOVE_FLAG_OFF	( 0x00000001 ^ 0xffffffff )
#define	CAMERA_TARGET_MOVE_FLAG_OFF	( 0x00000002 ^ 0xffffffff )

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTLV_CAMERA_WORK
{
	GFL_TCBSYS			*tcb_sys;
	GFL_G3D_CAMERA	*camera;
	int							phi;
	int							theta;
	fx32						radius;
	int							move_flag;
	VecFx32					move_pos;
	VecFx32					move_target;
	VecFx32					vec_pos;
	VecFx32					vec_target;
	int							wait;
	int							wait_tmp;
	int							brake_frame;
	HEAPID					heapID;
};

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================
static	void		BTLV_CAMERA_Move( BTLV_CAMERA_WORK *bcw );
static	void		BTLV_CAMERA_UpdateCameraAngle( BTLV_CAMERA_WORK *bcw );

//============================================================================================
/**
 *	�J���������ʒu
 */
//============================================================================================
static const VecFx32 cam_pos	= { FX_F32_TO_FX32( 6.7f ), FX_F32_TO_FX32( 6.7f ), FX_F32_TO_FX32( 17.3f ) };
static const VecFx32 cam_target = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 2.6f ), FX_F32_TO_FX32( 0.0f ) };
static const VecFx32 cam_up		= { 0,						FX32_ONE,				0 };

//============================================================================================
/**
 *	�V�X�e��������
 *
 * @param[in]	tcb_sys	�V�X�e�����Ŏg�p����TCBSYS�\���̂ւ̃|�C���^
 * @param[in]	heapID	�q�[�vID
 */
//============================================================================================
BTLV_CAMERA_WORK	*BTLV_CAMERA_Init( GFL_TCBSYS *tcb_sys, HEAPID heapID )
{
	BTLV_CAMERA_WORK *bcw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_CAMERA_WORK ) );

	bcw->heapID = heapID;
	bcw->tcb_sys = tcb_sys;

	bcw->camera = GFL_G3D_CAMERA_Create( GFL_G3D_PRJPERS, 
										 FX32_SIN13,
										 FX32_COS13,
										 FX_F32_TO_FX32( 1.35f ),
										 NULL,
										 FX32_ONE,
										 FX32_ONE * 512, //FX32_ONE * 256,
										 NULL,
										 &cam_pos,
										 &cam_up,
										 &cam_target,
										 heapID );

	BTLV_CAMERA_UpdateCameraAngle( bcw );

	GFL_G3D_CAMERA_Switching( bcw->camera );

	return bcw;
}

//============================================================================================
/**
 *	�V�X�e���I��
 *
 * @param[in]	bcw	BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_CAMERA_Exit( BTLV_CAMERA_WORK *bcw )
{
	GFL_G3D_CAMERA_Delete( bcw->camera );
	GFL_HEAP_FreeMemory( bcw );
}

//============================================================================================
/**
 *	�V�X�e�����C��
 *
 * @param[in]	bcw	BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_CAMERA_Main( BTLV_CAMERA_WORK *bcw )
{
	BTLV_CAMERA_Move( bcw );
}

//============================================================================================
/**
 *	�J�����ړ��i�ʒu�A�^�[�Q�b�g�w��j
 *
 * @param[in]	bcw		BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	pos		�ړ���J�����ʒu
 * @param[in]	target	�ړ���J�����^�[�Q�b�g
 */
//============================================================================================
void	BTLV_CAMERA_MoveCameraPosition( BTLV_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target )
{
	if( pos != NULL ){
		GFL_G3D_CAMERA_SetPos( bcw->camera, pos );
	}
	if( target != NULL ){
		GFL_G3D_CAMERA_SetTarget( bcw->camera, target );
	}
	GFL_G3D_CAMERA_Switching( bcw->camera );
	BTLV_CAMERA_UpdateCameraAngle( bcw );
}

//============================================================================================
/**
 *	�J�����ړ��i�p�x�w��j
 *
 * @param[in]	bcw		BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	phi		X�������̈ړ���
 * @param[in]	theta	Y�������̈ړ���
 */
//============================================================================================
void	BTLV_CAMERA_MoveCameraAngle( BTLV_CAMERA_WORK *bcw, int phi, int theta )
{
	VecFx32	pos, target;

	BTLV_CAMERA_GetCameraPositionAngle( bcw, phi, theta, &pos, &target );

	GFL_G3D_CAMERA_SetPos( bcw->camera, &pos );
	GFL_G3D_CAMERA_Switching( bcw->camera );
}

//============================================================================================
/**
 *	�J�����ړ��i�ړ�����w�肵�āA�Ԃ��Ԃ���j
 *
 * @param[in]	bcw		BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	pos		�ړ���J�����ʒu
 * @param[in]	target	�ړ���J�����^�[�Q�b�g
 * @param[in]	flame	�ړ��t���[����
 * @param[in]	wait	�ړ��E�G�C�g
 * @param[in]	brake	�ړ��Ƀu���[�L��������t���[����
 */
//============================================================================================
void	BTLV_CAMERA_MoveCameraInterpolation( BTLV_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target, int flame, int wait, int brake )
{
	VecFx32	now_pos, now_target;

	GFL_G3D_CAMERA_GetPos( bcw->camera, &now_pos );
	GFL_G3D_CAMERA_GetTarget( bcw->camera, &now_target );

	bcw->brake_frame = brake;
	bcw->wait = wait;
	bcw->wait_tmp = wait;

	if( pos != NULL ){
		bcw->move_pos.x = pos->x;
		bcw->move_pos.y = pos->y;
		bcw->move_pos.z = pos->z;
		BTLV_EFFTOOL_CalcMoveVector( &now_pos, pos, &bcw->vec_pos, FX32_CONST( flame ) );
		bcw->move_flag |= CAMERA_POS_MOVE_FLAG;
	}
	if( target != NULL ){
		bcw->move_target.x = target->x;
		bcw->move_target.y = target->y;
		bcw->move_target.z = target->z;
		BTLV_EFFTOOL_CalcMoveVector( &now_target, target, &bcw->vec_target, FX32_CONST( flame ) );
		bcw->move_flag |= CAMERA_TARGET_MOVE_FLAG;
	}
}

//============================================================================================
/**
 *	�J�����ʒu���擾
 *
 * @param[in]	bcw		BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	pos		�擾�����J�����ʒu���i�[���郏�[�N�ւ̃|�C���^
 * @param[in]	target	�擾�����J�����^�[�Q�b�g���i�[���郏�[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_CAMERA_GetCameraPosition( BTLV_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target )
{
	GFL_G3D_CAMERA_GetPos( bcw->camera, pos );
	GFL_G3D_CAMERA_GetTarget( bcw->camera, target );
}

//============================================================================================
/**
 *	�w�肳�ꂽ�p�x�̃J�����ʒu���擾
 *
 * @param[in]		bcw			BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 * @param[in]		phi			�擾�������J�����p�x
 * @param[in]		theta		�擾�������J�����p�x
 * @param[out]	pos			�擾�����J�����ʒu���i�[���郏�[�N�ւ̃|�C���^
 * @param[out]	target	�擾�����J�����^�[�Q�b�g���i�[���郏�[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_CAMERA_GetCameraPositionAngle( BTLV_CAMERA_WORK *bcw, int phi, int theta, VecFx32 *pos, VecFx32 *target )
{
	GFL_G3D_CAMERA_GetTarget( bcw->camera, target );

	bcw->phi += phi;
	bcw->theta += theta;

//	if( now_phi <= -PHI_MAX ){
//		now_phi = -PHI_MAX;
//	}
//	if( now_phi >= PHI_MAX ){
//		now_phi = PHI_MAX;
//	}

	//phi��theta��scale����camPos���v�Z
	pos->x = FX_MUL( FX_CosIdx( bcw->theta ), FX_CosIdx( bcw->phi ) );
	pos->y = FX_SinIdx( bcw->phi );
	pos->z = FX_MUL( FX_SinIdx( bcw->theta ), FX_CosIdx( bcw->phi ) );
	pos->x = FX_MUL( pos->x, bcw->radius );
	pos->y = FX_MUL( pos->y, bcw->radius );
	pos->z = FX_MUL( pos->z, bcw->radius );

	pos->x += target->x;
	pos->y += target->y;
	pos->z += target->z;
}

//============================================================================================
/**
 *	�J�����̃f�t�H���g�ʒu���擾
 *
 * @param[in]	pos		�擾�����J�����ʒu���i�[���郏�[�N�ւ̃|�C���^
 * @param[in]	target	�擾�����J�����^�[�Q�b�g���i�[���郏�[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_CAMERA_GetDefaultCameraPosition( VecFx32 *pos, VecFx32 *target )
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
 *	�J�����ړ������s����Ă��邩�`�F�b�N
 *
 * @param[in]	bcw		BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 *
 * @retval	FALSE:�ړ����Ă��Ȃ��@TRUE:�ړ���
 */
//============================================================================================
BOOL	BTLV_CAMERA_CheckExecute( BTLV_CAMERA_WORK *bcw )
{
	return ( bcw->move_flag != 0 );
}

//============================================================================================
/**
 *	�J�����ړ�����
 *
 * @param[in]	bcw		BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
static	void	BTLV_CAMERA_Move( BTLV_CAMERA_WORK *bcw )
{
	BOOL ret = TRUE;
	VecFx32	pos, target;

	if( bcw->move_flag == 0 ) return;

	if( bcw->wait == 0 ){
		bcw->wait = bcw->wait_tmp;
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
			BTLV_EFFTOOL_CheckMove( &pos.x, &bcw->vec_pos.x, &bcw->move_pos.x, &ret );
			BTLV_EFFTOOL_CheckMove( &pos.y, &bcw->vec_pos.y, &bcw->move_pos.y, &ret );
			BTLV_EFFTOOL_CheckMove( &pos.z, &bcw->vec_pos.z, &bcw->move_pos.z, &ret );
			if( ret == TRUE ){
				bcw->move_flag &= CAMERA_POS_MOVE_FLAG_OFF;
			}
		}
		if( bcw->move_flag & CAMERA_TARGET_MOVE_FLAG ){
			BTLV_EFFTOOL_CheckMove( &target.x, &bcw->vec_target.x, &bcw->move_target.x, &ret );
			BTLV_EFFTOOL_CheckMove( &target.y, &bcw->vec_target.y, &bcw->move_target.y, &ret );
			BTLV_EFFTOOL_CheckMove( &target.z, &bcw->vec_target.z, &bcw->move_target.z, &ret );
			if( ret == TRUE ){
				bcw->move_flag &= CAMERA_TARGET_MOVE_FLAG_OFF;
			}
		}
		GFL_G3D_CAMERA_SetPos( bcw->camera, &pos );
		GFL_G3D_CAMERA_SetTarget( bcw->camera, &target );
		GFL_G3D_CAMERA_Switching( bcw->camera );
		BTLV_CAMERA_UpdateCameraAngle( bcw );
	}
	else{
		bcw->wait--;
	}
}

//============================================================================================
/**
 *	���݂̃J�����ʒu�ƃ^�[�Q�b�g�ʒu����J�����p�x���v�Z���čX�V
 *
 * @param[in]	bcw		BTLV_CAMERA�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
static	void	BTLV_CAMERA_UpdateCameraAngle( BTLV_CAMERA_WORK *bcw )
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

//�J�����̃|�W�V��������A�ӁA�Ƃ����߂鎮�i�Q�l�̂��߂ɃR�����g�Ŏc���Ă����j
#if 0
	cw->phi		= FX_Atan2Idx( cam_pos.y, cam_pos.z );
	cw->theta	= FX_Atan2Idx( cam_pos.y, cam_pos.x );
	cw->scale	= VEC_Mag( &cam_pos );
//======================================================================
//	�J�����̐���
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
//	�J�����ʒu�̌v�Z
//======================================================================
static	void	CalcCamera( CAMERA_WORK *cw )
{
	MtxFx43	scale,trans;
	VecFx32	pos;

	//phi��theta��scale����camPos���v�Z
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
