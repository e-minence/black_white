//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fldmmdl_railacmd.c
 *	@brief  �t�B�[���h���샂�f���@���[���A�N�V�����R�}���h
 *	@author	tomoya takahashi
 *	@date		2009.08.26
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "fldmmdl.h"
#include "fldmmdl_procacmd.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
///	�����Q�� �����_2�� u16
//--------------------------------------------------------------
#define UX16_ONE (0x0100)
#define UX16_NUM(a) ((a)/UX16_ONE)
#define NUM_UX16(a) (UX16_ONE*(a))
//--------------------------------------------------------------
///	AC_JUMP_WORK�Ŏw�肷�鍂���B
///	DATA_AcJumpOffsetTbl[]�̗v�f���Ɉ�v
//--------------------------------------------------------------
enum
{
	AC_JUMP_HEIGHT_12 = 0,		///<����12
	AC_JUMP_HEIGHT_8,			///<����8
	AC_JUMP_HEIGHT_6,			///<����6
};

#define AC_JUMP_H_TBL_MAX (16)///<�����e�[�u���v�f��
#define AC_JUMP_H_TBL_FRAME_MAX (AC_JUMP_H_TBL_MAX-1)///<�����e�[�u���v�f��
#define AC_JUMP_H_TBL_FRAME_MAX_UX16 (NUM_UX16(AC_JUMP_H_TBL_FRAME_MAX))

//--------------------------------------------------------------
///	AC_JUMP_WORK�Ŏw�肷��㏸���x
//--------------------------------------------------------------
#define AC_JUMP_SPEED_UX16_1 (NUM_UX16(1))///<���x1
#define	AC_JUMP_SPEED_UX16_2 (NUM_UX16(2))///<���x2
#define AC_JUMP_SPEED_UX16_4 (NUM_UX16(4))///<���x4
#define AC_JUMP_SPEED_UX16_TBL(a) (AC_JUMP_H_TBL_FRAME_MAX_UX16/a)


//======================================================================
//	data
//======================================================================
//--------------------------------------------------------------
///	AC_JUMP�n�@����Y�I�t�Z�b�g ����0
//--------------------------------------------------------------
static const fx32 DATA_AcJumpOffsTblType0[AC_JUMP_H_TBL_MAX] =	// 12
{
	4*FX32_ONE,6*FX32_ONE,8*FX32_ONE,10*FX32_ONE,
	11*FX32_ONE,12*FX32_ONE,12*FX32_ONE,12*FX32_ONE,
	11*FX32_ONE,10*FX32_ONE,9*FX32_ONE,8*FX32_ONE,
	6*FX32_ONE,4*FX32_ONE,0*FX32_ONE,0*FX32_ONE
};

//--------------------------------------------------------------
///	AC_JUMP�n�@����Y�I�t�Z�b�g ����1
//--------------------------------------------------------------
static const fx32 DATA_AcJumpOffsTblType1[AC_JUMP_H_TBL_MAX] =	// 6
{
	 0*FX32_ONE,2*FX32_ONE,3*FX32_ONE,4*FX32_ONE,
	 5*FX32_ONE,6*FX32_ONE,6*FX32_ONE,6*FX32_ONE,
	 5*FX32_ONE,5*FX32_ONE,4*FX32_ONE,3*FX32_ONE,
	 2*FX32_ONE,0*FX32_ONE,0*FX32_ONE,0*FX32_ONE,
};

//--------------------------------------------------------------
///	AC_JUMP�n�@����Y�I�t�Z�b�g ����2
//--------------------------------------------------------------
static const fx32 DATA_AcJumpOffsTblType2[AC_JUMP_H_TBL_MAX] =	// 8
{
	 2*FX32_ONE,4*FX32_ONE,6*FX32_ONE,8*FX32_ONE,
	 9*FX32_ONE,10*FX32_ONE,10*FX32_ONE,10*FX32_ONE,
	 9*FX32_ONE,8*FX32_ONE,6*FX32_ONE,5*FX32_ONE,
	 3*FX32_ONE,2*FX32_ONE,0*FX32_ONE,0*FX32_ONE,
};

//--------------------------------------------------------------
///	AC_JUMP�n�@����Y�I�t�Z�b�g ����3
//--------------------------------------------------------------
static const fx32 DATA_AcJumpOffsTblType3[AC_JUMP_H_TBL_MAX] =	// 12
{
	4*FX32_ONE,6*FX32_ONE,8*FX32_ONE,10*FX32_ONE,
	11*FX32_ONE,12*FX32_ONE,12*FX32_ONE,12*FX32_ONE,
	11*FX32_ONE,10*FX32_ONE,9*FX32_ONE,8*FX32_ONE,
	6*FX32_ONE,4*FX32_ONE,0*FX32_ONE,0*FX32_ONE
};

//--------------------------------------------------------------
///	AC_JUMP�n�@��ޕʍ���Y�I�t�Z�b�g�e�[�u��
//--------------------------------------------------------------
static const fx32 * DATA_AcJumpOffsetTbl[] =
{
	DATA_AcJumpOffsTblType0,
	DATA_AcJumpOffsTblType1,
	DATA_AcJumpOffsTblType2,
};

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_WORK�\���́B���̏�����ŋ��ʎg�p����
//--------------------------------------------------------------
typedef struct
{
	u16 draw_state;			///<�`��X�e�[�^�X
	s16 wait;				///<�E�F�C�g
}AC_RAIL_STAY_WALK_WORK;

#define AC_RAIL_STAY_WALK_WORK_SIZE (sizeof(AC_RAIL_STAY_WALK_WORK))///<AC_STAY_WALK�T�C�Y


//--------------------------------------------------------------
///	AC_RAIL_JUMP_WORK�\���́B�W�����v�Ŏg�p����
//--------------------------------------------------------------
typedef struct
{
	fx32 val;			///<�ړ���
	fx32 dest_val;		///<�ړ�
	u16 h_speed;
	u16 h_frame;
	s8 dir;				///<�ړ�����
	s8 wait;			///<�E�F�C�g
	u8 draw_state;		///<�`��X�e�[�^�X
	s8 h_type;			///<����
}AC_RAIL_JUMP_WORK;

#define AC_RAIL_JUMP_WORK_SIZE (sizeof(AC_RAIL_JUMP_WORK))///<AC_RAIL_JUMP_WORK�T�C�Y




//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//======================================================================
//	�A�j���[�V�����R�}���h�@���ʃp�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * AC�n�@�R�}���h�I��
 * ���MMDL_STABIT_ACMD_END���Z�b�g
 * @param	mmdl	MMDL * 
 * @retval	int		FALSE
 */
//--------------------------------------------------------------
static int AC_RailEnd( MMDL * mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_ACMD_END );
	
	return( FALSE );
}


//======================================================================
//	DUMMY�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_DUMMY 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailDummy_0( MMDL * mmdl )
{
  GF_ASSERT_MSG( 0, "���[���ړ��ł́A�Ή�����Ă��܂���BACMD[0x%x]\n", MMDL_GetAcmdCode(mmdl) );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//======================================================================
//	DIR�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_DIR�n���ʏ���
 * @param	mmdl	MMDL * 
 * @param	dir		�\�������BDIR_UP
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcRailDirSet( MMDL * mmdl, int dir )
{
	MMDL_SetDirDisp( mmdl, dir );
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_UpdateGridPosCurrent( mmdl );
	MMDL_IncAcmdSeq( mmdl );
}

//--------------------------------------------------------------
/**
 * AC_DIR_U 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailDirU_0( MMDL * mmdl )
{
	AcRailDirSet( mmdl, DIR_UP );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DIR_D 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailDirD_0( MMDL * mmdl )
{
	AcRailDirSet( mmdl, DIR_DOWN );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DIR_L 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailDirL_0( MMDL * mmdl )
{
	AcRailDirSet( mmdl, DIR_LEFT );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DIR_R 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailDirR_0( MMDL * mmdl )
{
	AcRailDirSet( mmdl, DIR_RIGHT );
	
	return( TRUE );
}


//======================================================================
//	WORK�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_WALK_WORK������
 * @param	mmdl	MMDL * 
 * @param	dir		�ړ�����
 * @param	val		�ړ���
 * @param	wait	�ړ��t���[��
 * @param	draw	�`��X�e�[�^�X
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcRailWalkInit( MMDL * mmdl, int dir, s16 wait, u16 draw )
{

  MMDL_ReqRailMove( mmdl, dir, wait );
	MMDL_Rail_UpdateGridPosDir( mmdl, dir );
  MMDL_SetDirAll( mmdl, dir );
	MMDL_SetDrawStatus( mmdl, draw );
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVE_START );
	MMDL_IncAcmdSeq( mmdl );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK�n�@�ړ�
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailWalk_1( MMDL * mmdl )
{
  VecFx32 pos;
  FIELD_RAIL_WORK* p_rail;

  // RAIL_WORK
  p_rail = MMDL_GetRailWork( mmdl );
  FIELD_RAIL_WORK_Update( p_rail );

  //FIELD_RAIL_WORK_DEBUG_PrintRailGrid( p_rail );

  // �V�|�W�V�����̐ݒ�
  FIELD_RAIL_WORK_GetPos( p_rail, &pos );
  MMDL_SetVectorPos( mmdl, &pos );
  
	if( FIELD_RAIL_WORK_IsAction( p_rail ) ){
		return( FALSE );
	}
	
	MMDL_OnStatusBit(
		mmdl, MMDL_STABIT_MOVE_END|MMDL_STABIT_ACMD_END );

	MMDL_IncAcmdSeq( mmdl );
	
	return( FALSE );
}


//--------------------------------------------------------------
/**
 * AC_RAIL_WALK�n �ړ�����
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalk_End( MMDL *mmdl )
{
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	return( FALSE );
}


//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_U_16F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkU16F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_UP, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_D_16F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkD16F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_DOWN, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_L_16F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkL16F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_LEFT, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_R_16F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkR16F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_RIGHT, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_U_8F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkU8F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_UP, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_D_8F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkD8F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_DOWN, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_L_8F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkL8F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_R_8F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkR8F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_RIGHT, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	return( TRUE );
}


//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_U_4F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkU4F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_UP, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_D_4F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkD4F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_DOWN, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_L_4F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkL4F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_LEFT, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_R_4F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkR4F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_RIGHT, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	return( TRUE );
}




//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_U_2F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkU2F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_UP, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_D_2F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkD2F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_DOWN, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_L_2F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkL2F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_LEFT, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_WALK_R_2F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailWalkR2F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_RIGHT, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	return( TRUE );
}


//--------------------------------------------------------------
/**
 * AC_RAIL_DASH_U_4F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailDashU4F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_UP, GRID_FRAME_4, DRAW_STA_DASH_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_DASH_D_4F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailDashD4F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_DOWN, GRID_FRAME_4, DRAW_STA_DASH_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_DASH_L_4F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailDashL4F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_LEFT, GRID_FRAME_4, DRAW_STA_DASH_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_DASH_R_4F 0
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_RailDashR4F_0( MMDL* mmdl )
{
	AcRailWalkInit( mmdl, DIR_RIGHT, GRID_FRAME_4, DRAW_STA_DASH_4F ); 
	return( TRUE );
}


//======================================================================
//	StayWalk�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_RAIL_STAY_WALK_WORK������
 * @param	mmdl	MMDL * 
 * @param	dir		�ړ�����
 * @param	wait	�\���t���[��
 * @param	draw	�`��X�e�[�^�X
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcRailStayWalkWorkInit( MMDL * mmdl, int dir, s16 wait, u16 draw )
{
	AC_RAIL_STAY_WALK_WORK *work;
	
	work = MMDL_InitMoveCmdWork( mmdl, AC_RAIL_STAY_WALK_WORK_SIZE );
	
	work->draw_state = draw;
	work->wait = wait + FRAME_1;	//FRAME_1=����->�A�j���ւ�1�t���[��
	
	MMDL_SetDirDisp( mmdl, dir );
	MMDL_SetDrawStatus( mmdl, draw );
	MMDL_UpdateGridPosCurrent( mmdl );
	MMDL_IncAcmdSeq( mmdl );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK�n�@����
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static int AC_RailStayWalk_1( MMDL * mmdl )
{
	AC_RAIL_STAY_WALK_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	
	work->wait--;
	
	if( work->wait > 0 ){
		return( FALSE );
	}
	
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_ACMD_END );
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}


//--------------------------------------------------------------
/**
 * AC_STAY_WALK_U_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkU16F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_UP, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_D_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkD16F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_DOWN, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_L_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkL16F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_LEFT, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_R_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkR16F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_RIGHT, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_U_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkU8F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_UP, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_D_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkD8F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_DOWN, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_L_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkL8F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_R_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkR8F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_RIGHT, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}


//--------------------------------------------------------------
/**
 * AC_STAY_WALK_U_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkU2F_0( MMDL * mmdl )
{
  AcRailStayWalkWorkInit( mmdl, DIR_UP, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_D_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkD2F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_DOWN, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_L_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkL2F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_LEFT, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_R_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_RailStayWalkR2F_0( MMDL * mmdl )
{
	AcRailStayWalkWorkInit( mmdl, DIR_RIGHT, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}


//======================================================================
//	�A�j���[�V�����R�}���h�@AC_STAY_RAIL_JUMP_U�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_RAIL_JUMP_WORK�������@���C��
 * @param	mmdl	MMDL *
 * @param	dir		�ړ����� DIR_UP��
 * @param	val		�ړ���
 * @param	wait	�ړ��t���[��
 * @param	draw	�`��X�e�[�^�X
 * @param	h_type	�����BAC_RAIL_JUMP_HEIGHT_12��
 * @param	h_speed	�㏸���x AC_RAIL_JUMP_SPEED_1��
 * @param	se		�Đ�SE 0=�炳�Ȃ�
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcJumpWorkInitMain(
	MMDL * mmdl, int dir, fx32 val,
	s16 wait, u16 draw, s16 h_type, u16 h_speed, u32 se )
{
	AC_RAIL_JUMP_WORK *work;
	
	work = MMDL_InitMoveCmdWork( mmdl, AC_RAIL_JUMP_WORK_SIZE );
	work->dir = dir;
	work->val = val;
	work->wait = wait;
	work->draw_state = draw;
	work->h_type = h_type;
	work->h_speed = h_speed;
	
	if( val == 0 ){												//���̏�
		MMDL_UpdateGridPosCurrent( mmdl );
	}else{
    MMDL_ReqRailMove( mmdl, dir, GRID_FRAME_8 );
		MMDL_Rail_UpdateGridPosDir( mmdl, dir );					//�ړ�
	}
	
	MMDL_OnStatusBit( mmdl,
			MMDL_STABIT_MOVE_START |
			MMDL_STABIT_JUMP_START );
	
	MMDL_SetDirAll( mmdl, dir );
	MMDL_SetDrawStatus( mmdl, draw );
	MMDL_IncAcmdSeq( mmdl );
	
	if( se ){
    PMSND_PlaySE( se );
	}
}

//--------------------------------------------------------------
/**
 * AC_RAIL_JUMP_WORK������ SE_JUMP�Œ�
 * @param	mmdl	MMDL *
 * @param	dir		�ړ����� DIR_UP��
 * @param	val		�ړ���
 * @param	wait	�ړ��t���[��
 * @param	draw	�`��X�e�[�^�X
 * @param	h_type	�����BAC_JUMP_HEIGHT_12��
 * @param	h_speed	�㏸���x AC_JUMP_SPEED_1��
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcJumpWorkInit(
		MMDL * mmdl, int dir, fx32 val, s16 wait, u16 draw, s16 h_type, u16 h_speed )
{
#if 0
	AcJumpWorkInitMain( mmdl, dir, val,
			wait, draw, h_type, h_speed, SE_RAIL_JUMP );
#else
	AcJumpWorkInitMain( mmdl, dir, val,
			wait, draw, h_type, h_speed, SEQ_SE_DANSA );
#endif
}

//--------------------------------------------------------------
/**
 * AC_RAIL_JUMP�n�@�ړ�
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Jump_1( MMDL * mmdl )
{
	AC_RAIL_JUMP_WORK *work;
  VecFx32 pos;
  FIELD_RAIL_WORK* p_rail;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	
  // �ړ��Ǘ�
	if( work->val ){
    // RAIL_WORK
    p_rail = MMDL_GetRailWork( mmdl );
    FIELD_RAIL_WORK_Update( p_rail );

    // �V�|�W�V�����̐ݒ�
    FIELD_RAIL_WORK_GetPos( p_rail, &pos );
    MMDL_SetVectorPos( mmdl, &pos );
			
		if( work->dest_val >= GRID_FX32 ){						//�P�O���b�h�ړ�
			work->dest_val = 0;
      MMDL_ReqRailMove( mmdl, work->dir, GRID_FRAME_8 );
			MMDL_Rail_UpdateGridPosDir( mmdl, work->dir );
			MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVE_START );
		}
			
		{
			fx32 val = work->val;
			if( val < 0 ){ val = -val; }
			work->dest_val += val;
		}
	}
	
	{
		
		work->h_frame += work->h_speed;
		
		if( work->h_frame > AC_JUMP_H_TBL_FRAME_MAX_UX16 ){
			work->h_frame = AC_JUMP_H_TBL_FRAME_MAX_UX16;
		}
		
		{
			VecFx32 vec;
			u16 frame = UX16_NUM( work->h_frame );
			const fx32 *tbl = DATA_AcJumpOffsetTbl[work->h_type];
//			OS_Printf( " RAIL_JUMP=%x ", work->h_frame );
//			OS_Printf( " SPEED=%x ", work->h_speed );
//			OS_Printf( " FRAME=%x \n", frame );
			vec.x = 0;
			vec.y = tbl[frame];
			vec.z = 0;
			MMDL_SetVectorDrawOffsetPos( mmdl, &vec );
		}
	}
	
	work->wait--;
	
	if( work->wait > 0 ){
		return( FALSE );
	}
	
	{
		VecFx32 vec = { 0, 0, 0 };								//�I�t�Z�b�g�N���A
		MMDL_SetVectorDrawOffsetPos( mmdl, &vec );
	}
	
	MMDL_OnStatusBit( mmdl,
			MMDL_STABIT_MOVE_END |
			MMDL_STABIT_JUMP_END |
			MMDL_STABIT_ACMD_END );
	
	MMDL_UpdateGridPosCurrent( mmdl );

#if 0	//091030 �i�߂Ȃ� �A�j���֐������ŏ���
	MMDL_CallDrawProc( mmdl );						//1�t���[���i�߂�
#endif
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
  
  PMSND_PlaySE( SEQ_SE_FLD_10 );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_RAIL_JUMP_U_1G_8F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpU1G8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_8,
		GRID_FRAME_8, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_2 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JDMP_D_1G_8F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpD1G8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_8,
		GRID_FRAME_8, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_2 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JDMP_L_1G_8F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpL1G8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_8,
		GRID_FRAME_8, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_2 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JDMP_R_1G_8F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpR1G8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_8,
		GRID_FRAME_8, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_2 );
	
	return( TRUE );
}


//--------------------------------------------------------------
/**
 * AC_JUMP_U_2G_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpU2G16F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_8,
		GRID_FRAME_16, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMP_D_2G_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpD2G16F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_8,
		GRID_FRAME_16, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMP_L_2G_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpL2G16F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_8,
		GRID_FRAME_16, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMP_R_2G_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpR2G16F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_8,
		GRID_FRAME_16, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}


//======================================================================
//	data	�A�j���[�V�����R�}���h�e�[�u��
//======================================================================
//--------------------------------------------------------------
///	AC_RAIL_DUMMY
//--------------------------------------------------------------
int (* const DATA_AC_RailDummy[])( MMDL * ) = 
{
  AC_RailDummy_0,
  AC_RailEnd,
};
//--------------------------------------------------------------
///	AC_RAIL_DIR_U
//--------------------------------------------------------------
int (* const DATA_AC_RailDirU_Tbl[])( MMDL * ) = 
{
  AC_RailDirU_0,
  AC_RailEnd,
};
//--------------------------------------------------------------
///	AC_RAIL_DIR_D
//--------------------------------------------------------------
int (* const DATA_AC_RailDirD_Tbl[])( MMDL * ) = 
{
  AC_RailDirD_0,
  AC_RailEnd,
};
//--------------------------------------------------------------
///	AC_RAIL_DIR_L
//--------------------------------------------------------------
int (* const DATA_AC_RailDirL_Tbl[])( MMDL * ) = 
{
  AC_RailDirL_0,
  AC_RailEnd,
};
//--------------------------------------------------------------
///	AC_RAIL_DIR_R
//--------------------------------------------------------------
int (* const DATA_AC_RailDirR_Tbl[])( MMDL * ) =
{
  AC_RailDirR_0,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_U_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkU_16F_Tbl[])( MMDL * ) =
{
	AC_RailWalkU16F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_D_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkD_16F_Tbl[])( MMDL * ) =
{
	AC_RailWalkD16F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_L_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkL_16F_Tbl[])( MMDL * ) =
{
	AC_RailWalkL16F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_R_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkR_16F_Tbl[])( MMDL * ) =
{
	AC_RailWalkR16F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};


//--------------------------------------------------------------
///	AC_RAIL_WALK_U_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkU_8F_Tbl[])( MMDL * ) =
{
	AC_RailWalkU8F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_D_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkD_8F_Tbl[])( MMDL * ) =
{
	AC_RailWalkD8F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_L_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkL_8F_Tbl[])( MMDL * ) =
{
	AC_RailWalkL8F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_R_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkR_8F_Tbl[])( MMDL * ) =
{
	AC_RailWalkR8F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};


//--------------------------------------------------------------
///	AC_RAIL_WALK_U_4F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkU_4F_Tbl[])( MMDL * ) =
{
	AC_RailWalkU4F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_D_4F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkD_4F_Tbl[])( MMDL * ) =
{
	AC_RailWalkD4F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_L_4F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkL_4F_Tbl[])( MMDL * ) =
{
	AC_RailWalkL4F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_R_4F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkR_4F_Tbl[])( MMDL * ) =
{
	AC_RailWalkR4F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};


//--------------------------------------------------------------
///	AC_RAIL_WALK_U_2F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkU_2F_Tbl[])( MMDL * ) =
{
	AC_RailWalkU2F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_D_2F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkD_2F_Tbl[])( MMDL * ) =
{
	AC_RailWalkD2F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_L_2F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkL_2F_Tbl[])( MMDL * ) =
{
	AC_RailWalkL2F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_WALK_R_2F
//--------------------------------------------------------------
int (* const DATA_AC_RailWalkR_2F_Tbl[])( MMDL * ) =
{
	AC_RailWalkR2F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};


//--------------------------------------------------------------
///	AC_RAIL_DASH_U_4F
//--------------------------------------------------------------
int (* const DATA_AC_RailDashU_4F_Tbl[])( MMDL * ) =
{
	AC_RailDashU4F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_DASH_D_4F
//--------------------------------------------------------------
int (* const DATA_AC_RailDashD_4F_Tbl[])( MMDL * ) =
{
	AC_RailDashD4F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_DASH_L_4F
//--------------------------------------------------------------
int (* const DATA_AC_RailDashL_4F_Tbl[])( MMDL * ) =
{
	AC_RailDashL4F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};

//--------------------------------------------------------------
///	AC_RAIL_DASH_R_4F
//--------------------------------------------------------------
int (* const DATA_AC_RailDashR_4F_Tbl[])( MMDL * ) =
{
	AC_RailDashR4F_0,
	AC_RailWalk_1,
	AC_RailWalk_End,
};


//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_U_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkU_16F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkU16F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_D_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkD_16F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkD16F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_L_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkL_16F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkL16F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_R_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkR_16F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkR16F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_U_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkU_8F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkU8F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_D_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkD_8F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkD8F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_L_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkL_8F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkL8F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_R_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkR_8F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkR8F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_U_2F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkU_2F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkU2F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_D_2F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkD_2F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkD2F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_L_2F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkL_2F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkL2F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_RAIL_STAY_WALK_R_2F
//--------------------------------------------------------------
int (* const DATA_AC_RailStayWalkR_2F_Tbl[])( MMDL * ) =
{
	AC_RailStayWalkR2F_0,
	AC_RailStayWalk_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_JUMP_U_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailJumpU_1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpU1G8F_0,
	AC_Jump_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_JUMP_D_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailJumpD_1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpD1G8F_0,
	AC_Jump_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_JUMP_L_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailJumpL_1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpL1G8F_0,
	AC_Jump_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_JUMP_R_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_RailJumpR_1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpR1G8F_0,
	AC_Jump_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_JUMP_U_2G_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailJumpU_2G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpU2G16F_0,
	AC_Jump_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_JUMP_D_2G_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailJumpD_2G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpD2G16F_0,
	AC_Jump_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_JUMP_L_2G_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailJumpL_2G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpL2G16F_0,
	AC_Jump_1,
  AC_RailEnd,
};

//--------------------------------------------------------------
///	AC_JUMP_R_2G_16F
//--------------------------------------------------------------
int (* const DATA_AC_RailJumpR_2G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpR2G16F_0,
	AC_Jump_1,
  AC_RailEnd,
};
