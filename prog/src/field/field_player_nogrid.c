//======================================================================
/**
 * @file	field_player_nogrid.c
 * @brief	�m���O���b�h�ړ��@�t�B�[���h�v���C���[����
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "field_g3d_mapper.h"
#include "fldmmdl.h"
#include "field_player.h"
#include "field_camera.h"
#include "fldeff_shadow.h"

#include "field_player_nogrid.h"

//======================================================================
//	define
//======================================================================
#define MV_SPEED (2*FX32_ONE) ///<�ړ����x

//======================================================================
//	struct
//======================================================================

//======================================================================
//	proto
//======================================================================
static void nogridPC_Move_SetValue( FIELD_PLAYER *fld_player,
		FIELDMAP_WORK *fieldWork, u16 key, VecFx32 *vec );
static BOOL nogridPC_Move_CalcSetGroundMove(
		const FLDMAPPER *g3Dmapper,
		FLDMAPPER_GRIDINFODATA* gridInfoData,
		VecFx32* pos, VecFx32* vecMove, fx32 speed );




//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�@�m�[�O���b�h�@���[������
 *
 *	@param	fld_player		�t�B�[���h�v���C���[
 *	@param	key						�L�[
 *	@param	cp_camera			�J�������
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_Rail_Move( FIELD_PLAYER *fld_player, FLDEFF_CTRL *fectrl, const VecFx32* cp_playerway, int key, const FIELD_CAMERA* cp_camera )
{
	VecFx32 target;
	VecFx32 camerapos;
	VecFx32 pl_way;
	VecFx32 camera_way;
	u16 rotate_y;
	fx32 cos;
	VecFx32 cross;
	MMDL *fmmdl;

	fmmdl = FIELD_PLAYER_GetMMdl( fld_player );

	FIELD_CAMERA_GetTargetPos( cp_camera, &target );
	FIELD_CAMERA_GetCameraPos( cp_camera, &camerapos );
	VEC_Subtract( &target, &camerapos, &camera_way );	// �J���������̌v�Z
	camera_way.y = 0;	// ���ʂōl����
	VEC_Normalize( &camera_way, &camera_way );

	// ��l������
	pl_way = *cp_playerway;
	pl_way.y = 0;	// ���ʂōl����
	VEC_Normalize( &pl_way, &pl_way );


	// �J������������A�A�̕�����ݒ�
	rotate_y = FX_Atan2Idx( -camera_way.x, -camera_way.z );
	FLDEFF_SHADOW_SetGlobalRotate( fectrl, 0,rotate_y,0 );
	
	// �J�����̕����Ǝ�l���̕�������A����������
	if( VEC_Mag( cp_playerway ) == 0 )
	{
		MMDL_SetDirDisp(fmmdl,DIR_UP);
	}
	else
	{
		cos =  VEC_DotProduct( &pl_way, &camera_way );
		VEC_CrossProduct( &pl_way, &camera_way, &cross );

			

		if( (MATH_ABS(cos) < FX32_HALF) )
		{
			if( cross.y < 0 )
			{
				// left
				MMDL_SetDirDisp(fmmdl,DIR_LEFT);
			}
			else
			{
				// right
				MMDL_SetDirDisp(fmmdl,DIR_RIGHT);
			}
		}
		else if( cos > 0 )
		{
			// up
			MMDL_SetDirDisp(fmmdl,DIR_UP);
		}
		else
		{
			// down
			MMDL_SetDirDisp(fmmdl,DIR_DOWN);
		}


		// �v���C���[�����̐ݒ�
		FIELD_PLAYER_SetDir( fld_player, FX_Atan2Idx( pl_way.x, pl_way.z ) );
	}

	if( key & (PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT) )
	{
		if( key & PAD_BUTTON_B )
		{	
			MMDL_SetDrawStatus(fmmdl,DRAW_STA_WALK_8F);
		}
		else
		{
			MMDL_SetDrawStatus(fmmdl,DRAW_STA_WALK_8F);
		}
	}
	else
	{
		MMDL_SetDrawStatus(fmmdl,DRAW_STA_STOP);
	}
}


//======================================================================
//	�m���O���b�h�ړ��@�t�B�[���h�v���C���[����
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�@�m���O���b�h�ړ�
 * @param fld_player FIELD_PLAYER
 * @param key �L�[���
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_NOGRID_Free_Move( FIELD_PLAYER *fld_player, int key, fx32 onedist )
{
	VecFx32 pos;
	FIELDMAP_WORK *fieldWork;
	VecFx32	vecMove = { 0, 0, 0 };
	
	FIELD_PLAYER_GetPos( fld_player, &pos );
	fieldWork = FIELD_PLAYER_GetFieldMapWork( fld_player );
	
	nogridPC_Move_SetValue( fld_player, fieldWork, key, &vecMove );
	vecMove.x = FX_Mul( vecMove.x, onedist );	// �ړ�������ݒ�
	vecMove.z = FX_Mul( vecMove.z, onedist );
	
	if (key & PAD_BUTTON_B) {
		VEC_Add( &pos, &vecMove, &pos );
		FIELD_PLAYER_SetPos( fld_player, &pos );
	} else {
		fx32 diff;
		nogridPC_Move_CalcSetGroundMove(
				FIELDMAP_GetFieldG3Dmapper(fieldWork),
				FIELD_PLAYER_GetGridInfoData(fld_player),
				&pos, &vecMove, MV_SPEED );
		FIELD_PLAYER_SetPos( fld_player, &pos );
	}
}

//--------------------------------------------------------------
/**
 * ���@�@�m���O���b�h�ړ� C3�p
 * @param fld_player FIELD_PLAYER
 * @param key �L�[���
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_C3_Move( FIELD_PLAYER *fld_player, int key, u16 angle )
{
	VecFx32	vecMove = { 0, 0, 0 };
	FIELDMAP_WORK *fieldWork = FIELD_PLAYER_GetFieldMapWork( fld_player );
	nogridPC_Move_SetValue( fld_player, fieldWork, key, &vecMove );
}


//======================================================================
//	�m���O���b�h���@�@�ړ�����
//======================================================================
//--------------------------------------------------------------
/**
 * �m���O���b�h���@�@�ړ�����@�ړ��ʁA�������Z�b�g
 * @param	fld_player FIELD_PLAYER
 * @param	fieldWork FIELDMAP_WORK
 * @param key �L�[���
 * @param vec �ړ��ʊi�[��
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogridPC_Move_SetValue( FIELD_PLAYER *fld_player,
		FIELDMAP_WORK *fieldWork, u16 key, VecFx32 *vec )
{
	u16		dir;
	MMDL *fmmdl;
	BOOL	mvFlag = FALSE;
	
	fmmdl = FIELD_PLAYER_GetMMdl( fld_player );
	dir = FIELD_CAMERA_GetAngleYaw( FIELDMAP_GetFieldCamera(fieldWork) );
	
	if( key & PAD_KEY_UP )
	{
		mvFlag = TRUE;
		vec->x = FX_SinIdx( (u16)(dir + 0x8000) );
		vec->z = FX_CosIdx( (u16)(dir + 0x8000) );
		FIELD_PLAYER_SetDir( fld_player, dir );
		MMDL_SetDirDisp(fmmdl,DIR_UP);
	}

	if( key & PAD_KEY_DOWN )
	{
		mvFlag = TRUE;
		vec->x = FX_SinIdx( (u16)(dir + 0x0000) );
		vec->z = FX_CosIdx( (u16)(dir + 0x0000) );
		FIELD_PLAYER_SetDir( fld_player, dir + 0x8000 );
		MMDL_SetDirDisp(fmmdl, DIR_DOWN);
	}

	if( key & PAD_KEY_LEFT )
	{
		mvFlag = TRUE;
		vec->x = FX_SinIdx( (u16)(dir + 0xc000) );
		vec->z = FX_CosIdx( (u16)(dir + 0xc000) );
		FIELD_PLAYER_SetDir( fld_player, dir + 0x4000 );
		MMDL_SetDirDisp(fmmdl,DIR_LEFT);
	}

	if( key & PAD_KEY_RIGHT )
	{
		mvFlag = TRUE;
		vec->x = FX_SinIdx( (u16)(dir + 0x4000) );
		vec->z = FX_CosIdx( (u16)(dir + 0x4000) );
		FIELD_PLAYER_SetDir( fld_player, dir + 0xc000 );
		MMDL_SetDirDisp(fmmdl,DIR_RIGHT);
	}

	if (key & PAD_BUTTON_Y) {
		vec->y = -2 * FX32_ONE;
	}
	if (key & PAD_BUTTON_X) {
		vec->y = +2 * FX32_ONE;
	}
	
	if( mvFlag == TRUE ){
		if( key & PAD_BUTTON_B )
		{	
			//MMDL_SetDrawStatus(fmmdl,DRAW_STA_DASH_4F);
			MMDL_SetDrawStatus(fmmdl,DRAW_STA_WALK_8F);
		}
		else
		{
			MMDL_SetDrawStatus(fmmdl,DRAW_STA_WALK_8F);
		}
	} else {
		MMDL_SetDrawStatus(fmmdl,DRAW_STA_STOP);
	}
}

//--------------------------------------------------------------
/**
 * �ړ������̒n�`�ɉ������x�N�g���擾
 * @param	vecN �n�`�x�N�g��
 * @param	pos ���W
 * @param	vecMove �ړ��x�N�g��
 * @param	result ���ʊi�[��
 * @retval nothing
 */
//--------------------------------------------------------------
static void nogridPC_Move_GetGroundMoveVec(
	const VecFx16* vecN, const VecFx32* pos,
	const VecFx32* vecMove, VecFx32* result )
{
	VecFx32	vecN32, posNext;
	fx32	by, valD;
	
	VEC_Add( pos, vecMove, &posNext );

	VEC_Set( &vecN32, vecN->x, vecN->y, vecN->z );
	valD = -( FX_Mul(vecN32.x,pos->x) + FX_Mul(vecN32.y,pos->y) + FX_Mul(vecN32.z,pos->z) ); 
	by = -( FX_Mul( vecN32.x, posNext.x ) + FX_Mul( vecN32.z, posNext.z ) + valD );
	posNext.y = FX_Div( by, vecN32.y );

	VEC_Subtract( &posNext, pos, result );
	VEC_Normalize( result, result );
}

//--------------------------------------------------------------
/**
 * �m���O���b�h�ړ��@�ړ��v�Z
 * @param	g3Dmapper FLDMAPPER*
 * @param gridInfoData FLDMAPPER_GRIDINFODATA
 * @param pos ���W
 * @param vecMove �ړ���
 * @param speed �ړ����x
 * @retval BOOL FALSE=�ړ��s��
 */
//--------------------------------------------------------------
static BOOL nogridPC_Move_CalcSetGroundMove(
		const FLDMAPPER *g3Dmapper,
		FLDMAPPER_GRIDINFODATA* gridInfoData,
		VecFx32* pos, VecFx32* vecMove, fx32 speed )
{
	FLDMAPPER_GRIDINFO gridInfo;
	VecFx32	posNext, vecGround;
	fx32	height = 0;
	BOOL	initSw = FALSE;

	//VEC_Normalize( &vecMove, &vecMove );
	//
	if( (gridInfoData->vecN.x|gridInfoData->vecN.y|gridInfoData->vecN.z) == 0 ){
		//vecN = {0,0,0}�̏ꍇ�͏������
		VecFx16	vecNinit = { 0, FX16_ONE, 0 };

		nogridPC_Move_GetGroundMoveVec( &vecNinit, pos, vecMove, &vecGround );
		initSw = TRUE;
	} else {
		nogridPC_Move_GetGroundMoveVec( &gridInfoData->vecN, pos, vecMove, &vecGround );
	}
	VEC_MultAdd( speed, &vecGround, pos, &posNext );
	if( posNext.y < 0 ){
		posNext.y = 0;	//�x�[�X���C��
	}
	if( FLDMAPPER_CheckOutRange( g3Dmapper, &posNext ) == TRUE ){
	//	OS_Printf("�}�b�v�͈͊O�ňړ��s��\n");
		return FALSE;
	}

	//�v���[���[�p����B���̈ʒu���S�ɍ����f�[�^�����݂��邽�߁A���ׂĎ擾���Đݒ�
	if( FLDMAPPER_GetGridInfo( g3Dmapper, &posNext, &gridInfo ) == FALSE ){
		return FALSE;
	}

	if( gridInfo.count ){
		int		i = 0, p;
		fx32	h_tmp, diff1, diff2;

		height = gridInfo.gridData[0].height;
		p = 0;
		i++;
		for( ; i<gridInfo.count; i++ ){
			h_tmp = gridInfo.gridData[i].height;

			diff1 = height - pos->y;
			diff2 = h_tmp - pos->y;

			if( FX_Mul( diff2, diff2 ) < FX_Mul( diff1, diff1 ) ){
				height = h_tmp;
				p = i;
			}
		}
		*gridInfoData = gridInfo.gridData[p];	//�O���b�h�f�[�^�X�V
		VEC_Set( pos, posNext.x, gridInfoData->height, posNext.z );		//�ʒu���X�V
	}
	return TRUE;
}
