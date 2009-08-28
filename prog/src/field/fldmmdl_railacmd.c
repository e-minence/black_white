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

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

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
//	MMDL_UpdateGridPosCurrent( mmdl );
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
//	MMDL_UpdateGridPosDir( mmdl, dir );
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

  // �V�|�W�V�����̐ݒ�
  FIELD_RAIL_WORK_GetPos( p_rail, &pos );
  MMDL_SetVectorPos( mmdl, &pos );
  
	if( FIELD_RAIL_WORK_GetLastAction( p_rail ) ){
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
//	data	�A�j���[�V�����R�}���h�e�[�u��
//======================================================================
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


