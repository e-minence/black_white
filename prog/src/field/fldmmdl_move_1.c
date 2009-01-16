//******************************************************************************
/**
 *
 * @file	fieldobj_move_1.c
 * @brief	�t�B�[���h���샂�f���@��{����n����1�@�T�u����
 * @author	kagaya
 * @data	05.07.21
 *
 */
//******************************************************************************
#include "fldmmdl.h"

//==============================================================================
//	define
//==============================================================================
//#define DEBUG_IDO_ASSERT

//==============================================================================
//	struct
//==============================================================================

//==============================================================================
//	�v���g�^�C�v
//==============================================================================
static void MoveSub_InitProcCall( FLDMMDL * fmmdl );
static int MoveSub_StartCheckProcCall( FLDMMDL * fmmdl );
static int MoveSub_MoveProcCall( FLDMMDL * fmmdl );

static int MoveSub_PosUpdateStartCheck( FLDMMDL * fmmdl );
static int MoveSub_PosUpdateEndCheck( FLDMMDL * fmmdl );

static void SubMoveNon_Init( FLDMMDL * fmmdl );
static int SubMoveNon_StartCheck( FLDMMDL * fmmdl );
static int SubMoveNon_Move( FLDMMDL * fmmdl );

static void SubMoveKyoro_Init( FLDMMDL * fmmdl );
static int SubMoveKyoro_StartCheck( FLDMMDL * fmmdl );
static int SubMoveKyoro_Move( FLDMMDL * fmmdl );

static void SubMoveSpinStop_Init( FLDMMDL * fmmdl );
static int SubMoveSpinStop_StartCheck( FLDMMDL * fmmdl );
static int SubMoveSpinStop_Move( FLDMMDL * fmmdl );

void (* const DATA_fmmdl_EventTypeInitProcTbl[])( FLDMMDL * );
int (* const DATA_fmmdl_EventTypeStartCheckProcTbl[])( FLDMMDL * );
int (* const DATA_fmmdl_EventTypeMoveProcTbl[])( FLDMMDL * );

//==============================================================================
//	�T�u����
//==============================================================================
//--------------------------------------------------------------
/**
 * �T�u���쏉����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveSubProcInit( FLDMMDL * fmmdl )
{
	MoveSub_InitProcCall( fmmdl );
}

//--------------------------------------------------------------
/**
 * �T�u����
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�T�u����ɂ��ړ��A��
 */
//--------------------------------------------------------------
int FLDMMDL_MoveSub( FLDMMDL * fmmdl )
{
	if( MoveSub_StartCheckProcCall(fmmdl) == FALSE ){
		return( FALSE );
	}
	
	if( MoveSub_MoveProcCall(fmmdl) == FALSE ){
		return( FALSE );
	}
	
	return( TRUE );
}

//==============================================================================
//	�T�u���쏈���@�p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * �������֐��Ăяo��
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MoveSub_InitProcCall( FLDMMDL * fmmdl )
{
	int type = FLDMMDL_EventTypeGet( fmmdl );
	DATA_fmmdl_EventTypeInitProcTbl[type]( fmmdl );
}

//--------------------------------------------------------------
/**
 * �`�F�b�N�֐��Ăяo��
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�`�F�b�N�Ɉ�����������
 */
//--------------------------------------------------------------
static int MoveSub_StartCheckProcCall( FLDMMDL * fmmdl )
{
	int type = FLDMMDL_EventTypeGet( fmmdl );
	return( DATA_fmmdl_EventTypeStartCheckProcTbl[type](fmmdl) );
}

//--------------------------------------------------------------
/**
 * ����֐��Ăяo��
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=���쒆
 */
//--------------------------------------------------------------
static int MoveSub_MoveProcCall( FLDMMDL * fmmdl )
{
	int type = FLDMMDL_EventTypeGet( fmmdl );
	return( DATA_fmmdl_EventTypeMoveProcTbl[type](fmmdl) );
}

//==============================================================================
//	�T�u����p�p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * ���W�X�V�J�n���`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�X�V
 */
//--------------------------------------------------------------
static int MoveSub_PosUpdateStartCheck( FLDMMDL * fmmdl )
{
	int now = FLDMMDL_NowPosGX_Get( fmmdl );
	int old = FLDMMDL_OldPosGX_Get( fmmdl );
	
	if( now != old ){
		return( TRUE );
	}
	
	now = FLDMMDL_NowPosGZ_Get( fmmdl );
	old = FLDMMDL_OldPosGZ_Get( fmmdl );
	
	if( now != old ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���W�X�V�I�����`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�I��
 */
//--------------------------------------------------------------
static int MoveSub_PosUpdateEndCheck( FLDMMDL * fmmdl )
{
	int now = FLDMMDL_NowPosGX_Get( fmmdl );
	int old = FLDMMDL_OldPosGX_Get( fmmdl );
	
	if( now != old ){
		return( FALSE );
	}
	
	now = FLDMMDL_NowPosGZ_Get( fmmdl );
	old = FLDMMDL_OldPosGZ_Get( fmmdl );
	
	if( now != old ){
		return( FALSE );
	}
	
	return( TRUE );
}

//==============================================================================
//	�T�u����@����
//==============================================================================
//--------------------------------------------------------------
/**
 * �T�u���쏉�����@����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void SubMoveNon_Init( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �T�u����X�^�[�g�`�F�b�N�@����
 * @param	fmmdl	FLDMMDL *
 * @retval	int		FALSE
 */
//--------------------------------------------------------------
static int SubMoveNon_StartCheck( FLDMMDL * fmmdl )
{
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �T�u����@����
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=���쒆
 */
//--------------------------------------------------------------
static int SubMoveNon_Move( FLDMMDL * fmmdl )
{
	return( FALSE );
}

//==============================================================================
//	�T�u����@�~�܂��Ă���낫���
//==============================================================================
//--------------------------------------------------------------
///	SUBWORK_KYORO�\����
//--------------------------------------------------------------
typedef struct
{
	u8 check_seq_no;
	u8 move_seq_no;
	s8 walk_count;
	s8 max_count;
	
	s8 origin_dir;
	s8 dir_type;
	s8 dir_no;
	s8 dir_count;
	s8 wait;
}SUBWORK_KYORO;

#define SUBWORK_KYORO_SIZE (sizeof(SUBWORK_KYORO))

//--------------------------------------------------------------
///	����낫������
//--------------------------------------------------------------
enum
{
	DIR_H_TYPE = 0,		///<����������낫���
	DIR_V_TYPE,			///<�c��������낫���
};

#define KYORO_WAIT_FRAME (8)		///<����낫���̍���
#define KYORO_COUNT_MAX (4)			///<����낫���񐔍ő�

//--------------------------------------------------------------
/**
 * �~�܂��Ă���낫���@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void SubMoveKyoro_Init( FLDMMDL * fmmdl )
{
	SUBWORK_KYORO *work;
	
	work = FLDMMDL_MoveSubProcWorkInit( fmmdl, SUBWORK_KYORO_SIZE );
	work->max_count = FLDMMDL_ParamGet( fmmdl, FLDMMDL_PARAM_1 );
}

//--------------------------------------------------------------
/**
 * �~�܂��Ă���낫���@�X�^�[�g�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=����A��
 */
//--------------------------------------------------------------
static int SubMoveKyoro_StartCheck( FLDMMDL * fmmdl )
{
	SUBWORK_KYORO *work;
	
	work = FLDMMDL_MoveSubProcWorkGet( fmmdl );
	
	switch( work->check_seq_no ){
	case 0:														//�ړ��J�n�Ď�
		if( MoveSub_PosUpdateStartCheck(fmmdl) == TRUE ){
			work->check_seq_no++;
		}
		
		break;
	case 1:														//�ړ��I���Ď�
		if( MoveSub_PosUpdateEndCheck(fmmdl) == FALSE ){
			break;
		}
		
		work->walk_count++;
		
		if( work->walk_count < work->max_count ){
			work->check_seq_no = 0;
			break;
		}
		
		work->check_seq_no++;
	case 2:														//�ړ������Ď�
		if( FLDMMDL_StatusBitCheck_Move(fmmdl) == TRUE ){
#ifdef DEBUG_IDO_ASSERT
			GF_ASSERT( MoveSub_PosUpdateStartCheck(fmmdl) == TRUE &&
					"SubMoveKyoro_StartCheck()ERROR" );
#endif
			break;
		}
		
		work->check_seq_no++;
		work->walk_count = 0;
		work->move_seq_no = 0;
	case 3:														//�T�u���쒆
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �~�܂��Ă���낫���@����
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=���쒆
 */
//--------------------------------------------------------------
static int SubMoveKyoro_Move( FLDMMDL * fmmdl )
{
	SUBWORK_KYORO *work;
	
	work = FLDMMDL_MoveSubProcWorkGet( fmmdl );
	
	switch( work->move_seq_no ){
	case 0:														//������
		{
			int type_tbl[4] = {DIR_H_TYPE,DIR_H_TYPE,DIR_V_TYPE,DIR_V_TYPE};
			int dir = FLDMMDL_DirDispGet( fmmdl );
			work->origin_dir = dir;
			work->dir_type = type_tbl[dir];
			work->move_seq_no++;
		}
	case 1:														//�����Z�b�g	
		{
			int dir_tbl[2][2] = { {DIR_LEFT,DIR_RIGHT},{DIR_UP,DIR_DOWN} };
			int dir = dir_tbl[work->dir_type][work->dir_no];
			int code = FLDMMDL_AcmdCodeDirChange( dir, AC_DIR_U );
			FLDMMDL_CmdSet( fmmdl, code );
			work->move_seq_no++;
		}
	case 2:
		{
			if( FLDMMDL_CmdAction(fmmdl) == FALSE ){
				return( TRUE );
			}
			
			work->move_seq_no++;
		}
	case 3:
		{
			work->wait++;
			
			if( work->wait < KYORO_WAIT_FRAME ){
				return( TRUE );
			}
			
			work->wait = 0;
			work->dir_count++;
			
			if( work->dir_count < KYORO_COUNT_MAX ){
				work->dir_no = (work->dir_no + 1) & 0x01;
				work->move_seq_no = 1;
				return( TRUE );
			}
			
			FLDMMDL_DirDispCheckSet( fmmdl, work->origin_dir );
			work->move_seq_no++;
			work->dir_count = 0;
			work->check_seq_no = 0;
		}
	}
	
	return( FALSE );
}

//==============================================================================
//	�T�u����@�~�܂��ăN���N��
//==============================================================================
//--------------------------------------------------------------
///	SUBWORK_SPIN_STOP�\����
//--------------------------------------------------------------
typedef struct
{
	u8 check_seq_no;
	u8 move_seq_no;
	s8 walk_count;
	s8 max_count;
	
	s8 origin_dir;
	s8 dir_type;
	s8 dir_no;
	s8 dir_count;
	s8 wait;
}SUBWORK_SPIN_STOP;

#define SUBWORK_SPIN_STOP_SIZE (sizeof(SUBWORK_SPIN_STOP))

//--------------------------------------------------------------
///	��]����
//--------------------------------------------------------------
enum
{
	SPIN_STOP_L_TYPE = 0,		///<����]
	SPIN_STOP_R_TYPE,			///<�E��]
};

#define SPIN_STOP_WAIT_FRAME (8)	///<��]�̊�
#define SPIN_STOP_COUNT_MAX (4)		///<��]�ő吔

//--------------------------------------------------------------
/**
 * �~�܂��ăN���N���@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void SubMoveSpinStop_Init( FLDMMDL * fmmdl )
{
	int type;
	SUBWORK_SPIN_STOP *work;
	
	work = FLDMMDL_MoveSubProcWorkInit( fmmdl, SUBWORK_SPIN_STOP_SIZE );
	work->max_count = FLDMMDL_ParamGet( fmmdl, FLDMMDL_PARAM_1 );
	
	type = FLDMMDL_EventTypeGet( fmmdl );
	
	if( type == EV_TYPE_TRAINER_SPIN_STOP_L ){
		type = SPIN_STOP_L_TYPE;
	}else{
		type = SPIN_STOP_R_TYPE;
	}
	
	work->dir_type = type;
}

//--------------------------------------------------------------
/**
 * �~�܂��ăN���N���@�X�^�[�g�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=����A��
 */
//--------------------------------------------------------------
static int SubMoveSpinStop_StartCheck( FLDMMDL * fmmdl )
{
	SUBWORK_SPIN_STOP *work;
	
	work = FLDMMDL_MoveSubProcWorkGet( fmmdl );
	
	switch( work->check_seq_no ){
	case 0:														//�ړ��J�n�Ď�
		if( MoveSub_PosUpdateStartCheck(fmmdl) == TRUE ){
			work->check_seq_no++;
		}
		
		break;
	case 1:														//�ړ��I���Ď�
		if( MoveSub_PosUpdateEndCheck(fmmdl) == FALSE ){
			break;
		}
		
		work->walk_count++;
		
		if( work->walk_count < work->max_count ){
			work->check_seq_no = 0;
			break;
		}
		
		work->check_seq_no++;
	case 2:														//�ړ������Ď�
		if( FLDMMDL_StatusBitCheck_Move(fmmdl) == TRUE ){
#ifdef DEBUG_IDO_ASSERT
			GF_ASSERT( MoveSub_PosUpdateStartCheck(fmmdl) == TRUE &&
				"SubMoveKyoro_StartCheck()�Ώۂ̈ړ����@���ُ�" );
#endif
			break;
		}
		
		work->check_seq_no++;
		work->walk_count = 0;
		work->move_seq_no = 0;
	case 3:														//�T�u���쒆
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �~�܂��ăN���N���@����
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=���쒆
 */
//--------------------------------------------------------------
static int SubMoveSpinStop_Move( FLDMMDL * fmmdl )
{
	SUBWORK_KYORO *work;
	int spin_tbl[2][4] =
	{ {DIR_UP,DIR_LEFT,DIR_DOWN,DIR_RIGHT},{DIR_UP,DIR_RIGHT,DIR_DOWN,DIR_LEFT} };
	
	work = FLDMMDL_MoveSubProcWorkGet( fmmdl );
	
	switch( work->move_seq_no ){
	case 0:														//������
		{
			int i,dir = FLDMMDL_DirDispGet( fmmdl );
			
			for( i = 0; (i < DIR_4_MAX && dir != spin_tbl[work->dir_type][i]); i++ ){}
			GF_ASSERT( i < DIR_4_MAX && "SubMoveKyoro_Move()�����ُ�" );
			
			work->origin_dir = dir;
			work->dir_no = (i + 1) % SPIN_STOP_COUNT_MAX;		//+1=1��
			work->move_seq_no++;
		}
	case 1:														//�����Z�b�g	
		{
			int dir = spin_tbl[work->dir_type][work->dir_no];
			int code = FLDMMDL_AcmdCodeDirChange( dir, AC_DIR_U );
			FLDMMDL_CmdSet( fmmdl, code );
			work->move_seq_no++;
		}
	case 2:
		{
			if( FLDMMDL_CmdAction(fmmdl) == FALSE ){
				return( TRUE );
			}
			
			work->move_seq_no++;
		}
	case 3:
		{
			work->wait++;
			
			if( work->wait < SPIN_STOP_WAIT_FRAME ){
				return( TRUE );
			}
			
			work->wait = 0;
			work->dir_count++;
			
			if( work->dir_count < SPIN_STOP_COUNT_MAX ){
				work->dir_no = (work->dir_no + 1) % SPIN_STOP_COUNT_MAX;
				work->move_seq_no = 1;
				return( TRUE );
			}
			
			FLDMMDL_DirDispCheckSet( fmmdl, work->origin_dir );
			work->move_seq_no++;
			work->dir_count = 0;
			work->check_seq_no = 0;
		}
	}
	
	return( FALSE );
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
//	�C�x���g�^�C�v�ʏ������֐� EV_TYPE_NORMAL���̒l�Ɉ�v
//--------------------------------------------------------------
static void (* const DATA_fmmdl_EventTypeInitProcTbl[])( FLDMMDL * ) =
{
	SubMoveNon_Init, //EV_TYPE_NORMAL
	SubMoveNon_Init, //EV_TYPE_TRAINER
	SubMoveNon_Init, //EV_TYPE_TRAINER_EYEALL
	SubMoveNon_Init, //EV_TYPE_ITEM
	SubMoveKyoro_Init, //EV_TYPE_TRAINER_KYORO
	SubMoveSpinStop_Init, //EV_TYPE_TRAINER_SPIN_STOP_L
	SubMoveSpinStop_Init, //EV_TYPE_TRAINER_SPIN_STOP_R
	SubMoveNon_Init, //EV_TYPE_TRAINER_SPIN_MOVE_L
	SubMoveNon_Init, //EV_TYPE_TRAINER_SPIN_MOVE_R
	SubMoveNon_Init, //EV_TYPE_MSG
	
	SubMoveNon_Init, //EV_TYPE_TRAINER_ESCAPE
	SubMoveNon_Init, //EV_TYPE_TRAINER_SWIM_H
};

//--------------------------------------------------------------
//	�C�x���g�^�C�v�ʃX�^�[�g�`�F�b�N�֐��@EV_TYPE_NORMAL���̒l�Ɉ�v
//--------------------------------------------------------------
static int (* const DATA_fmmdl_EventTypeStartCheckProcTbl[])( FLDMMDL * ) =
{
	SubMoveNon_StartCheck, //EV_TYPE_NORMAL
	SubMoveNon_StartCheck, //EV_TYPE_TRAINER
	SubMoveNon_StartCheck, //EV_TYPE_TRAINER_EYEALL
	SubMoveNon_StartCheck, //EV_TYPE_ITEM
	SubMoveKyoro_StartCheck, //EV_TYPE_TRAINER_KYORO
	SubMoveSpinStop_StartCheck, //EV_TYPE_TRAINER_SPIN_STOP_L
	SubMoveSpinStop_StartCheck, //EV_TYPE_TRAINER_SPIN_STOP_R
	SubMoveNon_StartCheck, //EV_TYPE_TRAINER_SPIN_MOVE_L
	SubMoveNon_StartCheck, //EV_TYPE_TRAINER_SPIN_MOVE_R
	SubMoveNon_StartCheck, //EV_TYPE_MSG
	
	SubMoveNon_StartCheck, //EV_TYPE_TRAINER_ESCAPE
	SubMoveNon_StartCheck, //EV_TYPE_TRAINER_SWIM_H
};

//--------------------------------------------------------------
//	�C�x���g�^�C�v�ʓ���֐� EV_TYPE_NORMAL���̒l�Ɉ�v
//--------------------------------------------------------------
static int (* const DATA_fmmdl_EventTypeMoveProcTbl[])( FLDMMDL * ) =
{
	SubMoveNon_Move, //EV_TYPE_NORMAL
	SubMoveNon_Move, //EV_TYPE_TRAINER
	SubMoveNon_Move, //EV_TYPE_TRAINER_EYEALL
	SubMoveNon_Move, //EV_TYPE_ITEM
	SubMoveKyoro_Move, //EV_TYPE_TRAINER_KYORO
	SubMoveSpinStop_Move, //EV_TYPE_TRAINER_SPIN_STOP_L
	SubMoveSpinStop_Move, //EV_TYPE_TRAINER_SPIN_STOP_R
	SubMoveNon_Move, //EV_TYPE_TRAINER_SPIN_MOVE_L
	SubMoveNon_Move, //EV_TYPE_TRAINER_SPIN_MOVE_R
	SubMoveNon_Move, //EV_TYPE_MSG
	
	SubMoveNon_Move, //EV_TYPE_TRAINER_ESCAPE
	SubMoveNon_Move, //EV_TYPE_TRAINER_SWIM_H
};
