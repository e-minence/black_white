//======================================================================
/**
 * @file	field_player_nogrid.c
 * @brief	�m���O���b�h�ړ��@�t�B�[���h�v���C���[����
 */
//======================================================================
#include <gflib.h>

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "field_sound.h"


#include "fieldmap.h"
#include "field_g3d_mapper.h"
#include "fldmmdl.h"
#include "field_player.h"
#include "field_camera.h"
#include "fldeff_shadow.h"
#include "field_rail.h"
#include "rail_attr.h"

#include "field_player_nogrid.h"

//======================================================================
//	define
//======================================================================
#define MV_SPEED (2*FX32_ONE) ///<�ړ����x



//--------------------------------------------------------------
///	PLAYER_MOVE
//--------------------------------------------------------------
typedef enum
{
	PLAYER_MOVE_STOP = 0,
	PLAYER_MOVE_WALK,
	PLAYER_MOVE_TURN,
	PLAYER_MOVE_HITCH,
} PLAYER_MOVE;

//--------------------------------------------------------------
///	PLAYER_SET
//--------------------------------------------------------------
typedef enum
{
	PLAYER_SET_NON = 0,
	PLAYER_SET_STOP,
	PLAYER_SET_WALK,
	PLAYER_SET_TURN,
	PLAYER_SET_HITCH,
//  PLAYER_SET_JUMP,
} PLAYER_SET;


//======================================================================
//	struct
//======================================================================
//-------------------------------------
///	���[������p�⏕���[�N
//=====================================
struct _FIELD_PLAYER_NOGRID
{
  int move_state;
  
  FIELD_PLAYER* p_player;
	FIELDMAP_WORK* p_fieldwork;
  
  FIELD_RAIL_WORK* p_railwork;
} ;

//======================================================================
//	proto
//======================================================================
static void nogridPC_Move_SetValue( FIELD_PLAYER *fld_player,
		FIELDMAP_WORK *fieldWork, u16 key, VecFx32 *vec );
static BOOL nogridPC_Move_CalcSetGroundMove(
		const FLDMAPPER *g3Dmapper,
		FLDMAPPER_GRIDINFODATA* gridInfoData,
		VecFx32* pos, VecFx32* vecMove, fx32 speed );




//======================================================================
//	proto�@for nogrid_player
//======================================================================
//�ʏ�ړ�
static void jikiMove_Normal(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );

static PLAYER_SET player_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET player_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET player_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET player_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void player_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void player_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void player_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void player_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void player_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
/*static void player_SetMove_Jump(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );//*/

//���]�Ԉړ�
static void jikiMove_Cycle(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );

static PLAYER_SET playerCycle_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET playerCycle_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET playerCycle_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET playerCycle_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void playerCycle_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerCycle_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerCycle_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerCycle_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void playerCycle_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
/*static void playerCycle_SetMove_Jump(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );//*/

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�m�[�O���b�h�v���C���[���[�N�̐���
 *
 *	@param	p_player    �t�B�[���h�v���C���[
 *	@param	heapID      �q�[�vID
 *
 *	@return �m�[�O���b�h�p���[�N
 */
//-----------------------------------------------------------------------------
FIELD_PLAYER_NOGRID* FIELD_PLAYER_NOGRID_Create( FIELD_PLAYER* p_player, HEAPID heapID )
{
  FIELD_PLAYER_NOGRID* p_wk;
  MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_player );

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLAYER_NOGRID) );

  // ���[�N�ɕۑ�
  p_wk->p_player      = p_player;
  p_wk->p_fieldwork   = FIELD_PLAYER_GetFieldMapWork( p_player );

  // ����R�[�h�����[������ɕύX
  MMDL_OnStatusBit( p_mmdl, MMDL_STABIT_RAIL_MOVE );
  MMDL_ChangeMoveCode( p_mmdl, MV_RAIL_DMY );

  // ���[�����[�N�̎擾
  p_wk->p_railwork = MMDL_GetRailWork( p_mmdl );

  // �ʒu��������
  {
    RAIL_LOCATION location = {0};
    FIELD_RAIL_WORK_SetLocation( p_wk->p_railwork, &location );
  }


  // ��Ԃ̕��A
  {
    /*
    PLAYER_MOVE_FORM form;
    form = FIELD_PLAYER_GetMoveForm( gjiki->fld_player );
    
    switch( form ){
    case PLAYER_MOVE_FORM_SWIM:
      FIELD_PLAYER_NOGRID_SetRequest(
          gjiki, FIELD_PLAYER_NOGRID_REQBIT_SWIM );
      FIELD_PLAYER_NOGRID_UpdateRequest( gjiki );
      break;
    }
    //*/
  }

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�m�[�O���b�h�v���C���[���[�N�@�j��
 *
 *	@param	p_player  �t�B�[���h�v���C���[
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_Delete( FIELD_PLAYER_NOGRID* p_player )
{
  MMDL* p_mmdl = FIELD_PLAYER_GetMMdl( p_player->p_player );

  GFL_HEAP_FreeMemory( p_player );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�m�[�O���b�h�v���C���[���[�N  ����
 *
 *	@param	p_player      �v���C���[
 *	@param	key_trg       �L�[�g���K
 *	@param	key_cont      �L�[�R���g
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_Move( FIELD_PLAYER_NOGRID* p_player, int key_trg, int key_cont )
{
	u16 dir;
	BOOL debug_flag;
  PLAYER_MOVE_FORM form;
  
	dir = DIR_NOT;
	if( (key_cont&PAD_KEY_UP) ){
		dir = DIR_UP;
	}else if( (key_cont&PAD_KEY_DOWN) ){
		dir = DIR_DOWN;
	}else if( (key_cont&PAD_KEY_LEFT) ){
		dir = DIR_LEFT;
	}else if( (key_cont&PAD_KEY_RIGHT) ){
		dir = DIR_RIGHT;
	}
	
	debug_flag = FALSE;
#ifdef PM_DEBUG
	if( key_cont & PAD_BUTTON_R ){
		debug_flag = TRUE;
	}
#endif

#if 0
  if( key_trg & PAD_BUTTON_SELECT ){
    VecFx32 pos;
    FLDEFF_CTRL *fectrl = FIELDMAP_GetFldEffCtrl( gjiki->fieldWork );
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
    u16 dir = MMDL_GetDirDisp( mmdl );
    MMDL_GetVectorPos( mmdl, &pos );
    MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
    
    gjiki->fldeff_joint = FLDEFF_NAMIPOKE_SetMMdl(
        fectrl, dir, &pos, mmdl, FALSE );
  }
#endif
  
//  FIELD_PLAYER_NOGRID_UpdateRequest( gjiki );
  
  form = FIELD_PLAYER_GetMoveForm( p_player->p_player );
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    jikiMove_Normal( p_player, key_trg, key_cont, dir, debug_flag );
	  break;
  case PLAYER_MOVE_FORM_CYCLE:
    jikiMove_Cycle( p_player, key_trg, key_cont, dir, debug_flag );
	  break;
  case PLAYER_MOVE_FORM_SWIM:
    GF_ASSERT( form == PLAYER_MOVE_FORM_SWIM );
    break;
  default:
    GF_ASSERT( 0 );
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�����̐ݒ�
 *
 *	@param	p_player      �v���C���[
 *	@param	cp_location   ���P�[�V����
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_SetLocation( FIELD_PLAYER_NOGRID* p_player, const RAIL_LOCATION* cp_location )
{
  FIELD_RAIL_WORK_SetLocation( p_player->p_railwork, cp_location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���P�[�V�����̎擾
 *
 *	@param	cp_player     �v���C���[
 *	@param	p_location    ���P�[�V�����i�[��
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_GetLocation( const FIELD_PLAYER_NOGRID* cp_player, RAIL_LOCATION* p_location )
{
  FIELD_RAIL_WORK_GetLocation( cp_player->p_railwork, p_location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h�}�b�v�v���C���[�@position�擾
 *
 *	@param	cp_player     �v���C���[���[�N
 *	@param	p_pos         position
 */
//-----------------------------------------------------------------------------
void FIELD_PLAYER_NOGRID_GetPos( const FIELD_PLAYER_NOGRID* cp_player, VecFx32* p_pos )
{
  FIELD_RAIL_WORK_GetPos( cp_player->p_railwork, p_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����[�N�̎擾
 *
 *	@param	cp_player   �v���C���[
 *
 *	@return ���[�����[�N
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_WORK* FIELD_PLAYER_NOGRID_GetRailWork( const FIELD_PLAYER_NOGRID* cp_player )
{
  return cp_player->p_railwork;
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










//======================================================================
//  �O���b�h�ړ��@�t�B�[���h�v���C���[����@�ʏ�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@�ʏ�ړ�
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval nothing
 */
//--------------------------------------------------------------
static void jikiMove_Normal(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
	PLAYER_SET set;
	MMDL *mmdl = FIELD_PLAYER_GetMMdl( p_player->p_player );
   
	set = PLAYER_SET_NON;
	switch( p_player->move_state ){
	case PLAYER_MOVE_STOP:
		set = player_CheckMoveStart_Stop(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_WALK:
		set = player_CheckMoveStart_Walk(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_TURN:
		set = player_CheckMoveStart_Turn(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_HITCH:
		set = player_CheckMoveStart_Hitch(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	default:
		GF_ASSERT( 0 );
	}
	
	switch( set ){
	case PLAYER_SET_NON:
		player_SetMove_Non(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_STOP:
		player_SetMove_Stop(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_WALK:
		player_SetMove_Walk(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_TURN:
		player_SetMove_Turn(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_HITCH:
		player_SetMove_Hitch(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
/*  case PLAYER_SET_JUMP:
		player_SetMove_Jump(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;//*/
	}
}

//======================================================================
//	�ړ��J�n�`�F�b�N�@�ʏ�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��~��
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET player_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
		if( dir != DIR_NOT ){
			u16 old_dir;
			old_dir = MMDL_GetDirDisp( mmdl );
			
			if( dir != old_dir && debug_flag == FALSE ){
				return( PLAYER_SET_TURN );
			}
			
			return( player_CheckMoveStart_Walk(
				p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
		}
		
		return( PLAYER_SET_STOP );
	}
	
	return( PLAYER_SET_NON );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�ړ���
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET player_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT )
  {
		return( player_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	{
		u32 hit = MMDL_HitCheckRailMoveDir( mmdl, dir );
		
		if( debug_flag == TRUE )
    {
			if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit&MMDL_MOVEHITBIT_OUTRANGE) )
      {
				hit = MMDL_MOVEHITBIT_NON;
			}
		}
		
		if( hit == MMDL_MOVEHITBIT_NON )
    {
			return( PLAYER_SET_WALK );
		}
    
/*
    if( (hit & MMDL_MOVEHITBIT_ATTR) )
    {
      BOOL ret;
      u32 attr;
      VecFx32 pos;
      
      MMDL_GetVectorPos( mmdl, &pos );
      MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
      ret = MMDL_GetMapPosAttr( mmdl, &pos, &attr );
      
      if( ret == TRUE )
      {
        u16 attr_dir = DIR_NOT;
        MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
        MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
        
        if( MAPATTR_VALUE_CheckJumpUp(val) ){
          attr_dir = DIR_UP;
        }else if( MAPATTR_VALUE_CheckJumpDown(val) ){
          attr_dir = DIR_DOWN;
        }else if( MAPATTR_VALUE_CheckumpLeft(val) ){
          attr_dir = DIR_LEFT;
        }else if( MAPATTR_VALUE_CheckJumpRight(val) ){
          attr_dir = DIR_RIGHT;
        }
        
        if( attr_dir != DIR_NOT && attr_dir == dir ) //�W�����v������v
        {
          return( PLAYER_SET_JUMP );
        }
      }
    }
//*/
	}
	
	return( PLAYER_SET_HITCH );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�U�������
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET player_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( player_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( player_CheckMoveStart_Walk(
		p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��Q���q�b�g��
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET player_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
	  return( player_CheckMoveStart_Walk(
		  p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( player_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( player_CheckMoveStart_Walk(
		p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
//	�ړ��Z�b�g�@�ʏ�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�Z�b�g ���ɖ���
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��~
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	if( dir == DIR_NOT ){
		dir = MMDL_GetDirDisp( mmdl );
	}
	
	code = MMDL_ChangeDirAcmdCode( dir, AC_RAIL_DIR_U );
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_STOP;
	
  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_STOP );
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �ړ�
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	if( debug_flag == TRUE ){
		code = AC_RAIL_WALK_U_2F;
	}else if( key_cont & PAD_BUTTON_B ){
		code = AC_RAIL_DASH_U_4F;
	}else{
		code = AC_RAIL_WALK_U_8F;
	}
  
	code = MMDL_ChangeDirAcmdCode( dir, code );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_WALK;

  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_WALK );
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �U�����
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_2F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_TURN;
	
  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_TURN );
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��Q���q�b�g
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_16F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_HITCH;
	
  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_STOP );
  PMSND_PlaySE( SEQ_SE_WALL_HIT );
}

#if 0
//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �W�����v
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void player_SetMove_Jump(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	code = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_WALK;
  
  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_WALK );
  PMSND_PlaySE( SEQ_SE_DANSA );

}
#endif




//======================================================================
//  �O���b�h�ړ��@�t�B�[���h�v���C���[����@���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@���]�Ԉړ�
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval nothing
 */
//--------------------------------------------------------------
static void jikiMove_Cycle(
		FIELD_PLAYER_NOGRID *p_player, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
	PLAYER_SET set;
	MMDL *mmdl = FIELD_PLAYER_GetMMdl( p_player->p_player );
   
	set = PLAYER_SET_NON;
	switch( p_player->move_state ){
	case PLAYER_MOVE_STOP:
		set = playerCycle_CheckMoveStart_Stop(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_WALK:
		set = playerCycle_CheckMoveStart_Walk(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_TURN:
		set = playerCycle_CheckMoveStart_Turn(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_MOVE_HITCH:
		set = playerCycle_CheckMoveStart_Hitch(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	default:
		GF_ASSERT( 0 );
	}
	
	switch( set ){
	case PLAYER_SET_NON:
		playerCycle_SetMove_Non(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_STOP:
		playerCycle_SetMove_Stop(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_WALK:
		playerCycle_SetMove_Walk(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_TURN:
		playerCycle_SetMove_Turn(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
	case PLAYER_SET_HITCH:
		playerCycle_SetMove_Hitch(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
		break;
/*  case PLAYER_SET_JUMP:
		playerCycle_SetMove_Jump(
			p_player, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
//*/
	}
}

//======================================================================
//	�ړ��J�n�`�F�b�N�@���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��~��
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET playerCycle_CheckMoveStart_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
		if( dir != DIR_NOT ){
			u16 old_dir;
			old_dir = MMDL_GetDirDisp( mmdl );
			
			if( dir != old_dir && debug_flag == FALSE ){
				return( PLAYER_SET_TURN );
			}
			
			return( playerCycle_CheckMoveStart_Walk(
				p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
		}
		
		return( PLAYER_SET_STOP );
	}
	
	return( PLAYER_SET_NON );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�ړ���
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET playerCycle_CheckMoveStart_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT )
  {
		return( playerCycle_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	{
		u32 hit = MMDL_HitCheckRailMoveDir( mmdl, dir );
		
		if( debug_flag == TRUE )
    {
			if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit&MMDL_MOVEHITBIT_OUTRANGE) )
      {
				hit = MMDL_MOVEHITBIT_NON;
			}
		}
		
		if( hit == MMDL_MOVEHITBIT_NON )
    {
			return( PLAYER_SET_WALK );
		}
    
/*
    if( (hit & MMDL_MOVEHITBIT_ATTR) )
    {
      BOOL ret;
      u32 attr;
      VecFx32 pos;
      
      MMDL_GetVectorPos( mmdl, &pos );
      MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
      ret = MMDL_GetMapPosAttr( mmdl, &pos, &attr );
      
      if( ret == TRUE )
      {
        u16 attr_dir = DIR_NOT;
        MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
        MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
        
        if( MAPATTR_VALUE_CheckJumpUp(val) ){
          attr_dir = DIR_UP;
        }else if( MAPATTR_VALUE_CheckJumpDown(val) ){
          attr_dir = DIR_DOWN;
        }else if( MAPATTR_VALUE_CheckumpLeft(val) ){
          attr_dir = DIR_LEFT;
        }else if( MAPATTR_VALUE_CheckJumpRight(val) ){
          attr_dir = DIR_RIGHT;
        }
        
        if( attr_dir != DIR_NOT && attr_dir == dir ) //�W�����v������v
        {
          return( PLAYER_SET_JUMP );
        }
      }
    }
//*/
	}
	
	return( PLAYER_SET_HITCH );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�U�������
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET playerCycle_CheckMoveStart_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( playerCycle_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( playerCycle_CheckMoveStart_Walk(
		p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��Q���q�b�g��
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET playerCycle_CheckMoveStart_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
	  return( playerCycle_CheckMoveStart_Walk(
		  p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
	if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
		return( PLAYER_SET_NON );
	}
	
	if( dir == DIR_NOT ){
		return( playerCycle_CheckMoveStart_Stop(
			p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
	}
	
	return( playerCycle_CheckMoveStart_Walk(
		p_player,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
//	�ړ��Z�b�g�@���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�Z�b�g ���ɖ���
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Non(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��~
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Stop(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	if( dir == DIR_NOT ){
		dir = MMDL_GetDirDisp( mmdl );
	}
	
	code = MMDL_ChangeDirAcmdCode( dir, AC_RAIL_DIR_U );
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_STOP;
	
  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_STOP );
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �ړ�
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Walk(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	if( debug_flag == TRUE ){
		code = AC_RAIL_WALK_U_2F;
	}else{
		code = AC_RAIL_WALK_U_4F;
	}
  
	code = MMDL_ChangeDirAcmdCode( dir, code );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_WALK;

  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_WALK );
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �U�����
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Turn(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_2F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_TURN;
	
  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_TURN );
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��Q���q�b�g
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Hitch(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_16F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_HITCH;
	
  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_STOP );
  PMSND_PlaySE( SEQ_SE_WALL_HIT );
}

#if 0
//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �W�����v
 * @param	p_player FIELD_PLAYER_NOGRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void playerCycle_SetMove_Jump(
	FIELD_PLAYER_NOGRID *p_player, MMDL *mmdl,
	u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
	u16 code;
	
	GF_ASSERT( dir != DIR_NOT );
	
	code = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
	
	MMDL_SetAcmd( mmdl, code );
	p_player->move_state = PLAYER_MOVE_WALK;
  
  FIELD_PLAYER_SetMoveValue( p_player->p_player, PLAYER_MOVE_VALUE_WALK );
  PMSND_PlaySE( SEQ_SE_DANSA );
}
#endif

