
//============================================================================================
/**
 * @file	btl_camera.c
 * @brief	�퓬��ʃJ��������
 * @author	soga
 * @date	2008.11.20
 */
//============================================================================================

#include <gflib.h>

#include "btl_efftool.h"
#include "btl_camera.h"

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
 *	�v���g�^�C�v�錾
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
 *	�J���������ʒu
 */
//============================================================================================

//static const VecFx32 cam_pos = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 7.8f ), FX_F32_TO_FX32( 21.0f ) };
//static const VecFx32 cam_target = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 2.6f ), FX_F32_TO_FX32( 0.0f ) };
//static const VecFx32 cam_up = { 0, FX32_ONE, 0 };

static const VecFx32 cam_pos = { FX_F32_TO_FX32( 6.7f ), FX_F32_TO_FX32( 6.7f ), FX_F32_TO_FX32( 17.3f ) };
static const VecFx32 cam_target = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 2.6f ), FX_F32_TO_FX32( 0.0f ) };
static const VecFx32 cam_up = { 0, FX32_ONE, 0 };

//============================================================================================
/**
 *	�V�X�e��������
 *
 * @param[in]	tcb_sys	�V�X�e�����Ŏg�p����TCBSYS�\���̂ւ̃|�C���^
 * @param[in]	heapID	�q�[�vID
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
										 FX_F32_TO_FX32( 1.35f ),
										 NULL,
										 FX32_ONE,
										 FX32_ONE * 200,
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
 *	�V�X�e���I��
 *
 * @param[in]	bcw	BTL_CAMERA�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_CAMERA_Exit( BTL_CAMERA_WORK *bcw )
{
	GFL_G3D_CAMERA_Delete( bcw->camera );
	GFL_HEAP_FreeMemory( bcw );
}

//============================================================================================
/**
 *	�V�X�e�����C��
 *
 * @param[in]	bcw	BTL_CAMERA�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_CAMERA_Main( BTL_CAMERA_WORK *bcw )
{
	BTL_CAMERA_Move( bcw );
}

//============================================================================================
/**
 *	�J�����ړ��i�ʒu�A�^�[�Q�b�g�w��j
 *
 * @param[in]	bcw		BTL_CAMERA�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	pos		�ړ���J�����ʒu
 * @param[in]	target	�ړ���J�����^�[�Q�b�g
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
 *	�J�����ړ��i�p�x�w��j
 *
 * @param[in]	bcw		BTL_CAMERA�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	phi		X�������̈ړ���
 * @param[in]	theta	Y�������̈ړ���
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

	//phi��theta��scale����camPos���v�Z
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
 *	�J�����ړ��i�ړ�����w�肵�āA�Ԃ��Ԃ���j
 *
 * @param[in]	bcw		BTL_CAMERA�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	pos		�ړ���J�����ʒu
 * @param[in]	target	�ړ���J�����^�[�Q�b�g
 * @param[in]	flame	�ړ��t���[����
 * @param[in]	brake	�ړ��Ƀu���[�L��������t���[����
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
 *	�J�����ʒu���擾
 *
 * @param[in]	bcw		BTL_CAMERA�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	pos		�擾�����J�����ʒu���i�[���郏�[�N�ւ̃|�C���^
 * @param[in]	target	�擾�����J�����^�[�Q�b�g���i�[���郏�[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_CAMERA_GetCameraPosition( BTL_CAMERA_WORK *bcw, VecFx32 *pos, VecFx32 *target )
{
	GFL_G3D_CAMERA_GetPos( bcw->camera, pos );
	GFL_G3D_CAMERA_GetTarget( bcw->camera, target );
}

//============================================================================================
/**
 *	�J�����̃f�t�H���g�ʒu���擾
 *
 * @param[in]	pos		�擾�����J�����ʒu���i�[���郏�[�N�ւ̃|�C���^
 * @param[in]	target	�擾�����J�����^�[�Q�b�g���i�[���郏�[�N�ւ̃|�C���^
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
 *	�J�����ړ������s����Ă��邩�`�F�b�N
 *
 * @param[in]	bcw		BTL_CAMERA�Ǘ����[�N�ւ̃|�C���^
 *
 * @retval	FALSE:�ړ����Ă��Ȃ��@TRUE:�ړ���
 */
//============================================================================================
BOOL	BTL_CAMERA_CheckExecute( BTL_CAMERA_WORK *bcw )
{
	return ( bcw->move_flag != 0 );
}

//============================================================================================
/**
 *	�J�����ړ�����
 *
 * @param[in]	bcw		BTL_CAMERA�Ǘ����[�N�ւ̃|�C���^
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
 *	���݂̃J�����ʒu�ƃ^�[�Q�b�g�ʒu����J�����p�x���v�Z���čX�V
 *
 * @param[in]	bcw		BTL_CAMERA�Ǘ����[�N�ւ̃|�C���^
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
