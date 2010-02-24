//======================================================================
/**
 *
 * @file	fieldobj_acmd.c
 * @brief	�t�B�[���h���샂�f���A�j���[�V�����R�}���h
 * @author	kagaya
 * @date	05.08.05
 *
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procacmd.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "fieldmap.h"
#include "fldeff_gyoe.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	�����Q�� �����_2�� u16
//--------------------------------------------------------------
#define UX16_ONE (0x0100)
#define UX16_NUM(a) ((a)/UX16_ONE)
#define NUM_UX16(a) (UX16_ONE*(a))

//--------------------------------------------------------------
///	�A�j���[�V�����R�}���h���X�g�V�[�P���X
//--------------------------------------------------------------
enum
{
	SEQNO_AL_ACMD_COUNT_INIT = 0,///<�񐔏�����
	SEQNO_AL_ACMD_SET_CHECK,	///<�Z�b�g�\�`�F�b�N
	SEQNO_AL_ACMD_SET,			///<�Z�b�g
	SEQNO_AL_ACMD_END_CHECK,	///<�A�j���R�}���h�I���`�F�b�N
	SEQNO_AL_ACMD_SET_COUNT,	///<�A�j���R�}���h�Z�b�g�J�E���g
	SEQNO_AL_END,				///<�I��
};

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

//--------------------------------------------------------------
///	�j�ꂽ���E�W�����vSE
//--------------------------------------------------------------
#ifdef SE_YABURETA_JUMP2_STOP
#define SE_TWORLD_JUMP (0)
#else
#define SE_TWORLD_JUMP (SE_YABURETA_JUMP2)
#endif

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	ACMD_LIST_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	int seq_no;					///<�����ԍ�
	BOOL end_flag;				///<�I���t���O
	int acmd_count;				///<�A�j�����s�񐔂��L�^
	MMDL * mmdl;			///<�A�j���Ώ�MMDL *
	const MMDL_ACMD_LIST *list;	///<���s����R�}���h���X�g
}ACMD_LIST_WORK;

#define ACMD_LIST_WORK_SIZE (sizeof(ACMD_LIST_WORK)) ///<ACMD_LIST_WORK�T�C�Y

//--------------------------------------------------------------
///	AC_WALK_WORK�\���́B�����n�ŋ��ʎg�p���郏�[�N
//--------------------------------------------------------------
typedef struct
{
	u16 draw_state;			///<�`��X�e�[�^�X
	s16 wait;				///<�E�F�C�g
	int dir;				///<�ړ�����
	fx32 val;				///<�ړ���
}AC_WALK_WORK;

#define AC_WALK_WORK_SIZE (sizeof(AC_WALK_WORK))///<AC_WALK_WORK�T�C�Y

//--------------------------------------------------------------
///	AC_STAY_WALK_WORK�\���́B���̏�����ŋ��ʎg�p����
//--------------------------------------------------------------
typedef struct
{
	u16 draw_state;			///<�`��X�e�[�^�X
	s16 wait;				///<�E�F�C�g
}AC_STAY_WALK_WORK;

#define AC_STAY_WALK_WORK_SIZE (sizeof(AC_STAY_WALK_WORK))///<AC_STAY_WALK�T�C�Y

//--------------------------------------------------------------
///	AC_JUMP_WORK�\���́B�W�����v�Ŏg�p����
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
}AC_JUMP_WORK;

#define AC_JUMP_WORK_SIZE (sizeof(AC_JUMP_WORK))///<AC_JUMP_WORK�T�C�Y

//--------------------------------------------------------------
///	AC_WAIT�\���� �ҋ@�Ŏg�p����
//--------------------------------------------------------------
typedef struct
{
	int wait;
}AC_WAIT_WORK;

#define AC_WAIT_WORK_SIZE (sizeof(AC_WAIT_WORK))///<AC_WAIT_WORK�T�C�Y

//--------------------------------------------------------------
///	AC_WARP_WORK�\���� �㉺���[�v�Ŏg�p����
//--------------------------------------------------------------
typedef struct
{
	fx32 total_offset;
	fx32 value;
}AC_WARP_WORK;

#define AC_WARP_WORK_SIZE (sizeof(AC_WARP_WORK))

//--------------------------------------------------------------
///	AC_MARK_WORK�\���� !�}�[�N���Ŏg�p����
//--------------------------------------------------------------
typedef struct
{
	int type;
	FLDEFF_TASK *task_mark;
}AC_MARK_WORK;

#define AC_MARK_WORK_SIZE (sizeof(AC_MARK_WORK))

//--------------------------------------------------------------
///	AC_WALK_ODD_WORK�\���́B�����n�ŋ��ʎg�p���郏�[�N
//--------------------------------------------------------------
typedef struct
{
	s16 dir;			///<�ړ�����
	u16 draw_state;		///<�`��X�e�[�^�X
	s16 max_frame;		///<�ő�t���[��
	s16 frame;			///<�E�F�C�g
}AC_WALK_ODD_WORK;

#define AC_WALK_ODD_WORK_SIZE (sizeof(AC_WALK_ODD_WORK))
#define AC_WALK_7F_FRAME (7)
#define AC_WALK_6F_FRAME (6)
#define AC_WALK_3F_FRAME (3)
#define AC_WALK_12F_FRAME (12)

//--------------------------------------------------------------
///	AC_WALKVEC_WORK�\���́B�ړ��x�N�g���g�p�ɂ��ړ������Ŏg�p���郏�[�N
//--------------------------------------------------------------
typedef struct
{
	int wait;
	VecFx32 vec;
}AC_WALKVEC_WORK;

#define AC_WALKVEC_WORK_SIZE (sizeof(AC_WALKVEC_WORK))

//--------------------------------------------------------------
///	AC_JUMPVEC_WORK�\���́B�ړ��x�N�g���g�p�ɂ��ړ������Ŏg�p���郏�[�N
//--------------------------------------------------------------
typedef struct
{
	u8 wait;
	u8 jump_vec_type;
	u8 vec_type;
	u8 jump_flip;
	
	fx32 val;
	fx32 count;
	
	u16 jump_frame;
	u16 jump_frame_val;
}AC_JUMPVEC_WORK;

#define AC_JUMPVEC_WORK_SIZE (sizeof(AC_JUMPVEC_WORK))

enum
{
	VEC_X,
	VEC_Y,
	VEC_Z,
};

//======================================================================
//	proto
//======================================================================
static void mmdl_AcmdListProcTCB( GFL_TCB * tcb, void *wk );
int (* const DATA_AcmdListProcTbl[])( ACMD_LIST_WORK *work );

static int mmdl_AcmdAction( MMDL * mmdl, int code, int seq );

static int AC_End( MMDL * mmdl );

static void AcDirSet( MMDL * mmdl, int dir );

static void AcWalkWorkInit(
	MMDL * mmdl, int dir, fx32 val, s16 wait, u16 draw );
static int AC_Walk_1( MMDL * mmdl );
static int AC_End( MMDL *mmdl );

static void AcStayWalkWorkInit(
	MMDL * mmdl, int dir, s16 wait, u16 draw );
static int AC_StayWalk_1( MMDL * mmdl );

#ifndef MMDL_PL_NULL
static void AcMarkWorkInit(
	MMDL * mmdl, GYOE_TYPE type, int trans );
#else
static void AcMarkWorkInit(
	MMDL * mmdl, int type, int trans );
#endif
static int AC_Mark_1( MMDL * mmdl );

const fx32 * DATA_AcJumpOffsetTbl[];

static const fx32 DATA_AcWalk7FMoveValueTbl[AC_WALK_7F_FRAME];
static const fx32 DATA_AcWalk6FMoveValueTbl[AC_WALK_6F_FRAME];
static const fx32 DATA_AcWalk3FMoveValueTbl[AC_WALK_3F_FRAME];
static const fx32 DATA_AcWalk12FMoveValueTbl[AC_WALK_12F_FRAME];

//======================================================================
//	�A�j���[�V�����R�}���h���X�g
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���A�j���[�V�����R�}���h���X�g�Z�b�g
 * @param	mmdl		�A�j�����s��MMDL *
 * @param	list		�R�}���h���܂Ƃ߂�ꂽMMDL_ACMD_LIST *
 * @retval	GFL_TCB *		�A�j���[�V�����R�}���h�����s����GFL_TCB *
 */
//--------------------------------------------------------------
GFL_TCB * MMDL_SetAcmdList(
	MMDL *mmdl, const MMDL_ACMD_LIST *list )
{
	GFL_TCB *tcb;
	ACMD_LIST_WORK *work;
	MMDLSYS *mmdlsys;
	
	mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
	work = GFL_HEAP_AllocClearMemoryLo(
		MMDLSYS_GetHeapID(mmdlsys), ACMD_LIST_WORK_SIZE );
	
	{
		int pri = MMDLSYS_GetTCBPriority(
			MMDL_GetMMdlSys(mmdl) ) - 1;
		tcb = GFL_TCB_AddTask( MMDLSYS_GetTCBSYS(mmdlsys),
			mmdl_AcmdListProcTCB, work, pri );
		GF_ASSERT( tcb != NULL );
	}
	
	work->mmdl = mmdl;
	work->list = list;
	return( tcb );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h���X�g�I���`�F�b�N
 * @param	tcb	 �`�F�b�N����A�j�����Z�b�g����MMDL_SetAcmdList()�̖߂�lGFL_TCB *
 * @retval	BOOL TRUE=�I�� FALSE=���s��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckEndAcmdList( GFL_TCB * tcb )
{
	ACMD_LIST_WORK *work;
	GF_ASSERT( tcb != NULL );
	work = GFL_TCB_GetWork( tcb );
	return( work->end_flag );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h���X�g�I��
 * @param	tcb		�I������A�j�����Z�b�g����MMDL_SetAcmdList()�̖߂�lGFL_TCB *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_EndAcmdList( GFL_TCB * tcb )
{
	ACMD_LIST_WORK *work;
	work = GFL_TCB_GetWork( tcb );
	GF_ASSERT( MMDL_CheckEndAcmd(work->mmdl) == TRUE );
	MMDL_EndAcmd( work->mmdl );
	GFL_HEAP_FreeMemory( work );
	GFL_TCB_DeleteTask( tcb );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h���X�g����
 * @param	tcb		GFL_TCB *
 * @param	wk		TCB work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void mmdl_AcmdListProcTCB( GFL_TCB * tcb, void *wk )
{
	ACMD_LIST_WORK *work;
	work = wk;
	while( DATA_AcmdListProcTbl[work->seq_no](work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * ���X�g�����@�񐔏�����
 * @param	work	ACMD_LIST_WORK *
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int AcmdListProc_Init( ACMD_LIST_WORK *work )
{
	work->acmd_count = 0;
	work->seq_no = SEQNO_AL_ACMD_SET_CHECK;
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���X�g�����@�R�}���h�Z�b�g�\�`�F�b�N
 * @param	work	ACMD_LIST_WORK *
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int AcmdListProc_AcmdSetCheck( ACMD_LIST_WORK *work )
{
	if( MMDL_CheckPossibleAcmd(work->mmdl) == FALSE ){
		return( FALSE );
	}
	
	work->seq_no = SEQNO_AL_ACMD_SET;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���X�g�����@�R�}���h�Z�b�g
 * @param	work	ACMD_LIST_WORK *
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int AcmdListProc_AcmdSet( ACMD_LIST_WORK *work )
{
	const MMDL_ACMD_LIST *list;
		
	list = work->list;
	MMDL_SetAcmd( work->mmdl, list->code );
		
	work->seq_no = SEQNO_AL_ACMD_END_CHECK;
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���X�g�����@�R�}���h�I���`�F�b�N
 * @param	work	ACMD_LIST_WORK *
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int AcmdListProc_AcmdEndCheck( ACMD_LIST_WORK *work )
{
	if( MMDL_CheckEndAcmd(work->mmdl) == FALSE ){
		return( FALSE );
	}
	
	work->seq_no = SEQNO_AL_ACMD_SET_COUNT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���X�g�����@�R�}���h�J�E���g
 * @param	work	ACMD_LIST_WORK *
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int AcmdListProc_AcmdSetCount( ACMD_LIST_WORK *work )
{
	const MMDL_ACMD_LIST *list;
	
	list = work->list;
	work->acmd_count++;
		
	if( work->acmd_count < list->num ){
		work->seq_no = SEQNO_AL_ACMD_SET_CHECK;
		return( TRUE );
	}
		
	list++;
	work->list = list;
		
	if( list->code != ACMD_END ){
		work->seq_no = SEQNO_AL_ACMD_COUNT_INIT;
		return( TRUE );
	}
		
	work->end_flag = TRUE;
	work->seq_no = SEQNO_AL_END;
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���X�g�����@�I��
 * @param	work	ACMD_LIST_WORK *
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int AcmdListProc_End( ACMD_LIST_WORK *work )
{
	return( FALSE );
}

//--------------------------------------------------------------
///	�A�j���[�V�����R�}���h���X�g�����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_AcmdListProcTbl[])( ACMD_LIST_WORK *work ) =
{
	AcmdListProc_Init,
	AcmdListProc_AcmdSetCheck,
	AcmdListProc_AcmdSet,
	AcmdListProc_AcmdEndCheck,
	AcmdListProc_AcmdSetCount,
	AcmdListProc_End,
};

//======================================================================
//	�A�j���[�V�����R�}���h�@�R�[�h�擾
//======================================================================
//--------------------------------------------------------------
/**
 * �����ɂ��A�j���[�V�����R�}���h�R�[�h�ύX
 * @param	dir		����code��ϊ���������BDIR_UP��
 * @param	code	�ύX�������R�[�h�BAC_DIR_U��
 * @retval	int		dir�ʂɕύX���ꂽcode�@��Fdir=DIR_LEFT,code=AC_DIR_U = return(AC_DIR_L)
 */
//--------------------------------------------------------------
u16 MMDL_ChangeDirAcmdCode( u16 dir, u16 code )
{
	int i;
	const int * const *tbl;
	const int *dir_tbl;
	
	GF_ASSERT( dir < DIR_MAX4 );
	tbl = DATA_AcmdCodeDirChangeTbl;
	
	do{
		i = 0;
		dir_tbl = *tbl;
		
		do{
			if( dir_tbl[i] == code ){ return( dir_tbl[dir] ); }
			i++;
		}while( i < DIR_MAX4 );
		
		tbl++;
	}while( (*tbl) != NULL );
	
	GF_ASSERT( 0 && "MMDL_ChangeDirAcmdCode(���Ή��R�[�h)" );
	return( code );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�R�[�h�ɂ������擾
 * @param	code	�������擾�������R�[�h�BAC_DIR_U��
 * @retval	int		code���瓾��ꂽ����
 */
//--------------------------------------------------------------
u16 MMDL_GetAcmdDir( u16 code )
{
	int dir;
	const int * const *tbl;
	const int *dir_tbl;
	
	tbl = DATA_AcmdCodeDirChangeTbl;
	
	while( (*tbl) != NULL ){
		dir = DIR_UP;
		dir_tbl = *tbl;
		
		do{
			if( dir_tbl[dir] == code ){ return( dir ); }
			dir++;
		}while( dir < DIR_MAX4 );
		
		tbl++;
	}
	
	return( DIR_NOT );
}

//======================================================================
//	�A�j���[�V�����R�}���h�@����
//======================================================================
//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�A�N�V����
 * @param	mmdl		MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_ActionAcmd( MMDL * mmdl )
{
	int code,seq;
	
	do{
		code = MMDL_GetAcmdCode( mmdl );
		
		if( code == ACMD_NOT ){
			break;
		}
		
		seq = MMDL_GetAcmdSeq( mmdl );
	}while( mmdl_AcmdAction(mmdl,code,seq) );
}

//--------------------------------------------------------------
/**
 * �R�}���h�A�N�V�����B
 * �t�B�[���h���샂�f����������p�R�}���h����B
 * AcmdAction()�Ƃ̈Ⴂ�̓R�}���h�I������
 * �X�e�[�^�X�r�b�g�A�R�}���h���[�N�̏����������鎖��
 * �߂�l�ŏI�����肪�Ԃ鎖�B
 * @param	mmdl		MMDL * 
 * @retval	int			TRUE=�I��
 */
//--------------------------------------------------------------
BOOL MMDL_ActionLocalAcmd( MMDL * mmdl )
{
	MMDL_ActionAcmd( mmdl );
	
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ACMD_END );
	MMDL_SetAcmdCode( mmdl, ACMD_NOT );
	MMDL_SetAcmdSeq( mmdl, 0 );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�A�N�V����
 * @param	mmdl		MMDL * 
 * @retval	int			TRUE=�ċA
 */
//--------------------------------------------------------------
static int mmdl_AcmdAction( MMDL * mmdl, int code, int seq )
{
  if( !MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) )
  {
    // GRID�p�A�N�V�����R�}���h
  	return( DATA_AcmdActionTbl[code][seq](mmdl) );
  }
  else
  {
    // RAIL�p�A�N�V�����R�}���h
  	return( DATA_RailAcmdActionTbl[code][seq](mmdl) );
  }
}

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
static int AC_End( MMDL * mmdl )
{
	MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_ACMD_END );
	return( FALSE );
}

//======================================================================
//	�A�j���[�V�����R�}���h AC_DIR_U-AC_DIR_R
//======================================================================
//--------------------------------------------------------------
/**
 * AC_DIR�n���ʏ���
 * @param	mmdl	MMDL * 
 * @param	dir		�\�������BDIR_UP
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcDirSet( MMDL * mmdl, int dir )
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
static int AC_DirU_0( MMDL * mmdl )
{
	AcDirSet( mmdl, DIR_UP );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DIR_D 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DirD_0( MMDL * mmdl )
{
	AcDirSet( mmdl, DIR_DOWN );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DIR_L 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DirL_0( MMDL * mmdl )
{
	AcDirSet( mmdl, DIR_LEFT );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DIR_R 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DirR_0( MMDL * mmdl )
{
	AcDirSet( mmdl, DIR_RIGHT );
	
	return( TRUE );
}

//======================================================================
//	AC_WALK�n�@����
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
static void AcWalkWorkInit( MMDL * mmdl, int dir, fx32 val, s16 wait, u16 draw )
{
	AC_WALK_WORK *work;
	
	work = MMDL_InitMoveCmdWork( mmdl, AC_WALK_WORK_SIZE );
	work->draw_state = draw;
	work->wait = wait;
	work->dir = dir;
	work->val = val;
	
	MMDL_UpdateGridPosDir( mmdl, dir );
	MMDL_SetDirAll( mmdl, dir );
	MMDL_SetDrawStatus( mmdl, draw );
	MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
	MMDL_IncAcmdSeq( mmdl );
}

//--------------------------------------------------------------
/**
 * AC_WALK�n�@�ړ�
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Walk_1( MMDL * mmdl )
{
	AC_WALK_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	MMDL_AddVectorPosDir( mmdl, work->dir, work->val );
	MMDL_UpdateCurrentHeight( mmdl );
	
	work->wait--;
	
	if( work->wait > 0 ){
		return( FALSE );
	}
	
	MMDL_OnMoveBit(
		mmdl, MMDL_MOVEBIT_MOVE_END|MMDL_MOVEBIT_ACMD_END );
	MMDL_UpdateGridPosCurrent( mmdl );

#if 0	//091030 �i�߂Ȃ� �A�j���֐������ŏ���
  MMDL_CallDrawProc( mmdl );						//1�t���[���i�߂�
#endif
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
	
	return( FALSE );
}

#if 0
//--------------------------------------------------------------
/**
 * AC_WALK�n�@�ړ��@�_�b�V���p
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Dash_1( MMDL * mmdl )
{
	AC_WALK_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	MMDL_AddVectorPosDir( mmdl, work->dir, work->val );
	MMDL_UpdateCurrentHeight( mmdl );
	
	work->wait--;
	
	if( work->wait > 0 ){
		return( FALSE );
	}
	
	MMDL_OnMoveBit(
		mmdl, MMDL_MOVEBIT_MOVE_END|MMDL_MOVEBIT_ACMD_END );
	MMDL_UpdateGridPosCurrent( mmdl );
	
  MMDL_CallDrawProc( mmdl );						//1�t���[���i�߂�
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_WALK�n �_�b�V���ړ�����
 * @param	mmdl	MMDL *
 * @retval	int	TRUE
 */
//--------------------------------------------------------------
static int AC_Dash_End( MMDL *mmdl )
{
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	return( FALSE );
}
#endif

//======================================================================
//	AC_WALK_�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_WALK_U_32F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU32F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_32, GRID_FRAME_32, DRAW_STA_WALK_32F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_32F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD32F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_32, GRID_FRAME_32, DRAW_STA_WALK_32F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_32F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL32F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_32, GRID_FRAME_32, DRAW_STA_WALK_32F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_32F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR32F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_32, GRID_FRAME_32, DRAW_STA_WALK_32F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_U_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU16F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_16, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD16F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_16, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL16F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_16, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR16F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_16, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_U_8F 0
 *|| @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU8F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_8, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD8F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_8, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL8F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_8, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR8F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_8, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_U_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU4F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_4, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD4F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_4, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL4F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_4, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR4F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_4, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_U_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU2F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_2, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD2F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_2, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL2F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_2, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR2F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_2, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_U_1F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU1F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_1, GRID_FRAME_1, DRAW_STA_STOP ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_1F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD1F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_1, GRID_FRAME_1, DRAW_STA_STOP ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_1F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL1F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_1, GRID_FRAME_1, DRAW_STA_STOP );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_1F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR1F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_1, GRID_FRAME_1, DRAW_STA_STOP ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASH_U_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashU4F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_4, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASH_D_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashD4F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_4, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASH_L_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashL4F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_4, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASH_R_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashR4F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_4, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F ); 
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_SANDWALK_U_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_SandWalkU16F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_16, GRID_FRAME_16, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_SANDWALK_D_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_SandWalkD16F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_DOWN, GRID_VALUE_SPEED_16, GRID_FRAME_16, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_SANDWALK_L_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_SandWalkL16F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_16, GRID_FRAME_16, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_SANDWALK_R_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_SandWalkR16F_0( MMDL * mmdl )
{
	AcWalkWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_16, GRID_FRAME_16, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//======================================================================
//	AC_STAY_WALK�n	����
//======================================================================
//--------------------------------------------------------------
/**
 * AC_STAY_WALK_WORK������
 * @param	mmdl	MMDL * 
 * @param	dir		�ړ�����
 * @param	wait	�\���t���[��
 * @param	draw	�`��X�e�[�^�X
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcStayWalkWorkInit( MMDL * mmdl, int dir, s16 wait, u16 draw )
{
	AC_STAY_WALK_WORK *work;
	
	work = MMDL_InitMoveCmdWork( mmdl, AC_WALK_WORK_SIZE );
	
	work->draw_state = draw;
#if 0 //old
	work->wait = wait + FRAME_1;	//FRAME_1=����->�A�j���ւ�1�t���[��
#else //wb 091030 �A�j���֐������ŏ���
	work->wait = wait;
#endif

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
static int AC_StayWalk_1( MMDL * mmdl )
{
	AC_STAY_WALK_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	
	work->wait--;
	
	if( work->wait > 0 ){
		return( FALSE );
	}
	
	MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_ACMD_END );
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
	return( FALSE );
}

//======================================================================
//	AC_STAY_WALK�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_STAY_WALK_U_32F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkU32F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_UP, GRID_FRAME_32, DRAW_STA_WALK_32F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_D_32F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkD32F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_DOWN, GRID_FRAME_32, DRAW_STA_WALK_32F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_L_32F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkL32F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_LEFT, GRID_FRAME_32, DRAW_STA_WALK_32F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_R_32F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkR32F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_RIGHT, GRID_FRAME_32, DRAW_STA_WALK_32F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_U_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkU16F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_UP, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_D_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkD16F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_DOWN, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_L_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkL16F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_LEFT, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_R_16F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkR16F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_RIGHT, GRID_FRAME_16, DRAW_STA_WALK_16F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_U_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkU8F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_UP, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_D_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkD8F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_DOWN, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_L_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkL8F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_R_8F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkR8F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_RIGHT, GRID_FRAME_8, DRAW_STA_WALK_8F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_U_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkU4F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_UP, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_D_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkD4F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_DOWN, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_L_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkL4F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_LEFT, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_R_4F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkR4F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_RIGHT, GRID_FRAME_4, DRAW_STA_WALK_4F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_U_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkU2F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_UP, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_D_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkD2F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_DOWN, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_L_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkL2F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_LEFT, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_WALK_R_2F 0
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayWalkR2F_0( MMDL * mmdl )
{
	AcStayWalkWorkInit( mmdl, DIR_RIGHT, GRID_FRAME_2, DRAW_STA_WALK_2F ); 
	
	return( TRUE );
}

//======================================================================
//	�A�j���[�V�����R�}���h�@AC_STAY_JUMP_U�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_JUMP_WORK�������@���C��
 * @param	mmdl	MMDL *
 * @param	dir		�ړ����� DIR_UP��
 * @param	val		�ړ���
 * @param	wait	�ړ��t���[��
 * @param	draw	�`��X�e�[�^�X
 * @param	h_type	�����BAC_JUMP_HEIGHT_12��
 * @param	h_speed	�㏸���x AC_JUMP_SPEED_1��
 * @param	se		�Đ�SE 0=�炳�Ȃ�
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcJumpWorkInitMain(
	MMDL * mmdl, int dir, fx32 val,
	s16 wait, u16 draw, s16 h_type, u16 h_speed, u32 se )
{
	AC_JUMP_WORK *work;
	
	work = MMDL_InitMoveCmdWork( mmdl, AC_JUMP_WORK_SIZE );
	work->dir = dir;
	work->val = val;
	work->wait = wait;
	work->draw_state = draw;
	work->h_type = h_type;
	work->h_speed = h_speed;
	
	if( val == 0 ){												//���̏�
		MMDL_UpdateGridPosCurrent( mmdl );
	}else{
		MMDL_UpdateGridPosDir( mmdl, dir );					//�ړ�
	}
	
	MMDL_OnMoveBit( mmdl,
			MMDL_MOVEBIT_MOVE_START |
			MMDL_MOVEBIT_JUMP_START );
	
	MMDL_SetDirAll( mmdl, dir );
	MMDL_SetDrawStatus( mmdl, draw );
	MMDL_IncAcmdSeq( mmdl );
	
	if( se ){
    PMSND_PlaySE( se );
	}
}

//--------------------------------------------------------------
/**
 * AC_JUMP_WORK������ SE_JUMP�Œ�
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
			wait, draw, h_type, h_speed, SE_JUMP );
#else
	AcJumpWorkInitMain( mmdl, dir, val,
			wait, draw, h_type, h_speed, SEQ_SE_DANSA );
#endif
}

//--------------------------------------------------------------
/**
 * AC_JUMP�n�@�ړ�
 * @param	mmdl	MMDL * 
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Jump_1( MMDL * mmdl )
{
	AC_JUMP_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	
	if( work->val ){
		MMDL_AddVectorPosDir( mmdl, work->dir, work->val );
		MMDL_UpdateCurrentHeight( mmdl );
			
		if( work->dest_val >= GRID_FX32 ){						//�P�O���b�h�ړ�
			work->dest_val = 0;
			MMDL_UpdateGridPosDir( mmdl, work->dir );
			MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
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
//			OS_Printf( " JUMP=%x ", work->h_frame );
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
	
	MMDL_OnMoveBit( mmdl,
			MMDL_MOVEBIT_MOVE_END |
			MMDL_MOVEBIT_JUMP_END |
			MMDL_MOVEBIT_ACMD_END );
	
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
 * AC_STAY_JUMP_U_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayJumpU16F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_UP, 0,
		GRID_FRAME_16, DRAW_STA_WALK_16F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_JUMP_D_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayJumpD16F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_DOWN, 0,
		GRID_FRAME_16, DRAW_STA_WALK_16F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_JUMP_L_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayJumpL16F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_LEFT, 0,
		GRID_FRAME_16, DRAW_STA_WALK_16F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_JUMP_R_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayJumpR16F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_RIGHT, 0,
		GRID_FRAME_16, DRAW_STA_WALK_16F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_JUMP_U_8F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayJumpU8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_UP, 0,
		GRID_FRAME_8, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_2 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_JUMP_D_8F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayJumpD8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_DOWN, 0,
		GRID_FRAME_8, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_2 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_JUMP_L_8F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayJumpL8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_LEFT, 0,
		GRID_FRAME_8, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_2 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_STAY_JUMP_R_8F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_StayJumpR8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_RIGHT, 0,
		GRID_FRAME_8, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_2 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMP_U_1G_8F 0
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
static int AC_JumpU2G8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_UP, GRID_VALUE_SPEED_8,
		GRID_FRAME_8*2, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMP_D_2G_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpD2G8F_0( MMDL * mmdl )
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
static int AC_JumpL2G8F_0( MMDL * mmdl )
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
static int AC_JumpR2G8F_0( MMDL * mmdl )
{
	AcJumpWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_8,
		GRID_FRAME_16, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12, AC_JUMP_SPEED_UX16_1 );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPHI_L_1G_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpHiL1G16F_0( MMDL * mmdl )
{
#if 0
	AcJumpWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_16,
		GRID_FRAME_16, DRAW_STA_TAKE_OFF_16F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(GRID_FRAME_16) );
#else
	AcJumpWorkInit( mmdl, DIR_LEFT, GRID_VALUE_SPEED_16,
		GRID_FRAME_16, DRAW_STA_WALK_16F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(GRID_FRAME_16) );
#endif
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPHI_R_1G_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpHiR1G16F_0( MMDL * mmdl )
{
#if 0
	AcJumpWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_16,
		GRID_FRAME_16, DRAW_STA_TAKE_OFF_16F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(GRID_FRAME_16) );
#else
	AcJumpWorkInit( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_16,
		GRID_FRAME_16, DRAW_STA_WALK_16F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(GRID_FRAME_16) );
#endif
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPHI_L_3G_32F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpHiL3G32F_0( MMDL * mmdl )
{
#if 0
	AcJumpWorkInit( mmdl, DIR_LEFT,
		GRID_VALUE_SPEED_4,
		12, DRAW_STA_TAKE_OFF_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(12) );
#else	
	AcJumpWorkInit( mmdl, DIR_LEFT,
		GRID_VALUE_SPEED_4,
		12, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(12) );
#endif
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPHI_R_3G_32F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpHiR3G32F_0( MMDL * mmdl )
{
#if 0
	AcJumpWorkInit( mmdl, DIR_RIGHT,
		GRID_VALUE_SPEED_4,
		12, DRAW_STA_TAKE_OFF_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(12) );
#else
	AcJumpWorkInit( mmdl, DIR_RIGHT,
		GRID_VALUE_SPEED_4,
		12, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(12) );
#endif
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMP_U_3G_24F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpU3G24F_0( MMDL * mmdl )
{
#if 0
	AcJumpWorkInitMain( mmdl, DIR_UP, GRID_VALUE_SPEED_8,
		GRID_FRAME_8*3, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(24), SE_TWORLD_JUMP );
#else
	AcJumpWorkInitMain( mmdl, DIR_UP, GRID_VALUE_SPEED_8,
		GRID_FRAME_8*3, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(24), 0 );
#endif
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMP_D_3G_24F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpD3G24F_0( MMDL * mmdl )
{
#if 0
	AcJumpWorkInitMain( mmdl, DIR_DOWN, GRID_VALUE_SPEED_8,
		GRID_FRAME_8*3, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(24), SE_TWORLD_JUMP );
#else
	AcJumpWorkInitMain( mmdl, DIR_DOWN, GRID_VALUE_SPEED_8,
		GRID_FRAME_8*3, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(24), 0 );
#endif
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMP_L_3G_24F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpL3G24F_0( MMDL * mmdl )
{
#if 0
	AcJumpWorkInitMain( mmdl, DIR_LEFT, GRID_VALUE_SPEED_8,
		GRID_FRAME_8*3, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(24), SE_TWORLD_JUMP );
#else
	AcJumpWorkInitMain( mmdl, DIR_LEFT, GRID_VALUE_SPEED_8,
		GRID_FRAME_8*3, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(24), 0 );
#endif
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMP_R_3G_24F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpR3G24F_0( MMDL * mmdl )
{
#if 0
	AcJumpWorkInitMain( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_8,
		GRID_FRAME_8*3, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(24), SE_TWORLD_JUMP );
#else
	AcJumpWorkInitMain( mmdl, DIR_RIGHT, GRID_VALUE_SPEED_8,
		GRID_FRAME_8*3, DRAW_STA_WALK_8F, AC_JUMP_HEIGHT_12,
		AC_JUMP_SPEED_UX16_TBL(24), 0 );
#endif
	
	return( TRUE );
}

//======================================================================
//	�A�j���[�V�����R�}���h�@AC_WAIT�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_WAIT_WORK������
 * @param	mmdl	MMDL *
 * @param	wait	�E�F�C�g�t���[��
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcWaitWorkInit( MMDL * mmdl, int wait )
{
	AC_WAIT_WORK *work;
	
	work = MMDL_InitMoveCmdWork( mmdl, AC_WAIT_WORK_SIZE );
	work->wait = wait;
	
	MMDL_IncAcmdSeq( mmdl );
}

//--------------------------------------------------------------
/**
 * AC_WAIT�n�@�҂�
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Wait_1( MMDL * mmdl )
{
	AC_WAIT_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	
	if( work->wait ){
		work->wait--;
		return( FALSE );
	}
	
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WAIT_1F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Wait1F_0( MMDL * mmdl )
{
	AcWaitWorkInit( mmdl, 1 );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WAIT_2F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Wait2F_0( MMDL * mmdl )
{
	AcWaitWorkInit( mmdl, 2 );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WAIT_4F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Wait4F_0( MMDL * mmdl )
{
	AcWaitWorkInit( mmdl, 4 );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WAIT_8F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Wait8F_0( MMDL * mmdl )
{
	AcWaitWorkInit( mmdl, 8 );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WAIT_15F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Wait15F_0( MMDL * mmdl )
{
	AcWaitWorkInit( mmdl, 15 );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WAIT_16F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Wait16F_0( MMDL * mmdl )
{
	AcWaitWorkInit( mmdl, 16 );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WAIT_32F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Wait32F_0( MMDL * mmdl )
{
	AcWaitWorkInit( mmdl, 32 );
	return( TRUE );
}

//======================================================================
//	�A�j���[�V�����R�}���h�@AC_WARP_UP DOWN
//======================================================================
//--------------------------------------------------------------
/**
 * AC_WARP_UP 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WarpUp_0( MMDL * mmdl )
{
	AC_WARP_WORK *work;
	
	work = MMDL_InitMoveCmdWork( mmdl, AC_WARP_WORK_SIZE );
	work->value = FX32_ONE * 16;
	
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WARP_UP 1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WarpUp_1( MMDL * mmdl )
{
	int grid;
	AC_WARP_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	
	work->total_offset += work->value;
	
	{
		VecFx32 vec = {0,0,0};
		vec.y = work->total_offset;
		MMDL_SetVectorDrawOffsetPos( mmdl, &vec );
	}
	
	grid = work->total_offset / GRID_HALF_FX32;
	
	if( grid < 40 ){
		return( FALSE );
	}
	
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WARP_DOWN 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WarpDown_0( MMDL * mmdl )
{
	AC_WARP_WORK *work;
	
	work = MMDL_InitMoveCmdWork( mmdl, AC_WARP_WORK_SIZE );
	work->total_offset = GRID_HALF_FX32 * 40;
	work->value = -(FX32_ONE * 16);
	
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WARP_DOWN 1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WarpDown_1( MMDL * mmdl )
{
	AC_WARP_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	
	work->total_offset += work->value;
	
	if( work->total_offset < 0 ){
		work->total_offset = 0;
	}
	
	{
		VecFx32 vec = {0,0,0};
		vec.y = work->total_offset;
		
		MMDL_SetVectorDrawOffsetPos( mmdl, &vec );
	}
	
	if( work->total_offset > 0 ){
		return( FALSE );
	}
	
	
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//======================================================================
//	�A�j���[�V�����R�}���h�@AC_VANISH_ON OFF
//======================================================================
//--------------------------------------------------------------
/**
 * AC_VANISH_ON 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_VanishON_0( MMDL * mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_VANISH );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_VANISH_OFF 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_VanishOFF_0( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_VANISH );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//======================================================================
//
//	�A�j���[�V�����R�}���h�@AC_DIR_PAUSE_ON OFF
//======================================================================
//--------------------------------------------------------------
/**
 * AC_DIR_PAUSE_ON 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DirPauseON_0( MMDL * mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_PAUSE_DIR );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DIR_PAUSE_OFF 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DirPauseOFF_0( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_PAUSE_DIR );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//======================================================================
//	�A�j���[�V�����R�}���h�@AC_ANM_PAUSE_ON OFF
//======================================================================
//--------------------------------------------------------------
/**
 * AC_ANM_PAUSE_ON 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_AnmPauseON_0( MMDL * mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_PAUSE_ANM );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_ANM_PAUSE_OFF 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_AnmPauseOFF_0( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_PAUSE_ANM );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//======================================================================
//	�A�j���[�V�����R�}���h�@�}�[�N
//======================================================================
//--------------------------------------------------------------
/**
 * AC_MARK_WORK������
 * @param	mmdl	MMDL *
 * @param	type	�}�[�N���
 * @param	trans	TRUE=�}�[�N�O���t�B�b�N�]���҂���
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcMarkWorkInit(
	MMDL * mmdl, int type, int trans )
{
	AC_MARK_WORK *work;

	work = MMDL_InitMoveCmdWork( mmdl, AC_MARK_WORK_SIZE );
	work->type = type;
	
  {
    MMDLSYS *mmdlsys =  (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
    FIELDMAP_WORK *fieldMap = MMDLSYS_GetFieldMapWork( mmdlsys );
	  FLDEFF_CTRL *fectrl = FIELDMAP_GetFldEffCtrl( fieldMap );
    work->task_mark = FLDEFF_GYOE_SetMMdl( fectrl, mmdl, type, TRUE );
  }

	MMDL_IncAcmdSeq( mmdl );
}

//--------------------------------------------------------------
/**
 * AC_MARK�n�@����
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Mark_1( MMDL * mmdl )
{
	AC_MARK_WORK *work;
	work = MMDL_GetMoveCmdWork( mmdl );

	if( FLDEFF_GYOE_CheckEnd(work->task_mark) == TRUE ){
    FLDEFF_TASK_CallDelete( work->task_mark );
		MMDL_IncAcmdSeq( mmdl );
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_MARK_GYOE 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_MarkGyoe_0( MMDL * mmdl )
{
	AcMarkWorkInit( mmdl, FLDEFF_GYOETYPE_GYOE, FALSE );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_MARK_SAISEN 1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_MarkSaisen_0( MMDL * mmdl )
{
//	AcMarkWorkInit( mmdl, FLDEFF_GYOETYOE_SAISEN, FALSE );
	AcMarkWorkInit( mmdl, FLDEFF_GYOETYPE_GYOE, FALSE );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_MARK_GYOE_TWAIT 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_MarkGyoeTWait_0( MMDL * mmdl )
{
	AcMarkWorkInit( mmdl, FLDEFF_GYOETYPE_GYOE, FALSE );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_MARK_HANETA 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_MarkHatena_0( MMDL * mmdl )
{
	AcMarkWorkInit( mmdl, FLDEFF_GYOETYPE_HATE, FALSE );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_MARK_ONPU 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_MarkOnpu_0( MMDL * mmdl )
{
	AcMarkWorkInit( mmdl, FLDEFF_GYOETYPE_ONPU, FALSE );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_MARK_TENTEN 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_MarkTenTen_0( MMDL * mmdl )
{
	AcMarkWorkInit( mmdl, FLDEFF_GYOETYPE_TEN, FALSE );
	return( FALSE );
}

//======================================================================
//	AC_WALK_ODD�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_WALK_ODD_WORK������
 * @param	mmdl	MMDL *
 * @param	dir			�ړ�����
 * @param	max_frame	�ړ��ő�t���[��
 * @param	draw		�`��X�e�[�^�X
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcWalkOddWorkInit( MMDL * mmdl, int dir, s16 max_frame, u16 draw )
{
	AC_WALK_ODD_WORK *work;
	
	work = MMDL_InitMoveCmdWork( mmdl, AC_WALK_ODD_WORK_SIZE );
	work->dir = dir;
	work->draw_state = draw;
	work->max_frame = max_frame;
	
	MMDL_UpdateGridPosDir( mmdl, dir );
	MMDL_SetDirAll( mmdl, dir );
	MMDL_SetDrawStatus( mmdl, draw );
	MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
	MMDL_IncAcmdSeq( mmdl );
}

//--------------------------------------------------------------
/**
 * AC_WALK_ODD_WORK�n�@�ړ�
 * @param	mmdl	MMDL *
 * @param	tbl		�ړ��ʂ��i�[����Ă���e�[�u��
 * @retval	int		TRUE=�I��
 */
//--------------------------------------------------------------
static int AC_WalkOdd_Walk( MMDL * mmdl, const fx32 *tbl )
{
	AC_WALK_ODD_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	MMDL_AddVectorPosDir( mmdl, work->dir, tbl[work->frame] );
	MMDL_UpdateCurrentHeight( mmdl );
	
	work->frame++;
	
	if( work->frame < work->max_frame ){
		return( FALSE );
	}
	
	MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_END|MMDL_MOVEBIT_ACMD_END );
	MMDL_UpdateGridPosCurrent( mmdl );
#if 0	//091030 �i�߂Ȃ� �A�j���֐������ŏ���
	MMDL_CallDrawProc( mmdl );						//1�t���[���i�߂�
#endif
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
	
	return( TRUE );
}

//======================================================================
//	AC_WALK �t���[����n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_WALK_U_6F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU6F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_UP, AC_WALK_6F_FRAME, DRAW_STA_WALK_6F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_6F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD6F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_DOWN, AC_WALK_6F_FRAME, DRAW_STA_WALK_6F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_6F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL6F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_LEFT, AC_WALK_6F_FRAME, DRAW_STA_WALK_6F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_6F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR6F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_RIGHT, AC_WALK_6F_FRAME, DRAW_STA_WALK_6F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASH_U_6F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashU6F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_UP, AC_WALK_6F_FRAME, DRAW_STA_HERO_DASH_6F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASH_D_6F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashD6F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_DOWN, AC_WALK_6F_FRAME, DRAW_STA_HERO_DASH_6F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASH_L_6F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashL6F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_LEFT, AC_WALK_6F_FRAME, DRAW_STA_HERO_DASH_6F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASH_R_6F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashR6F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_RIGHT, AC_WALK_6F_FRAME, DRAW_STA_HERO_DASH_6F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_*_6F 1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Walk6F_1( MMDL * mmdl )
{
	if( AC_WalkOdd_Walk(mmdl,DATA_AcWalk6FMoveValueTbl) == TRUE ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_U_3F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU3F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_UP, AC_WALK_3F_FRAME, DRAW_STA_WALK_3F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_3F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD3F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_DOWN, AC_WALK_3F_FRAME, DRAW_STA_WALK_3F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_3F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL3F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_LEFT, AC_WALK_3F_FRAME, DRAW_STA_WALK_3F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_3F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR3F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_RIGHT, AC_WALK_3F_FRAME, DRAW_STA_WALK_3F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_*_3F 1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Walk3F_1( MMDL * mmdl )
{
	if( AC_WalkOdd_Walk(mmdl,DATA_AcWalk3FMoveValueTbl) == TRUE ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_U_7F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU7F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_UP, AC_WALK_7F_FRAME, DRAW_STA_WALK_7F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_7F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD7F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_DOWN, AC_WALK_7F_FRAME, DRAW_STA_WALK_7F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_7F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL7F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_LEFT, AC_WALK_7F_FRAME, DRAW_STA_WALK_7F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_7F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR7F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_RIGHT, AC_WALK_7F_FRAME, DRAW_STA_WALK_7F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_*_7F 1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Walk7F_1( MMDL * mmdl )
{
	if( AC_WalkOdd_Walk(mmdl,DATA_AcWalk7FMoveValueTbl) == TRUE ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_U_12F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkU12F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_UP, AC_WALK_12F_FRAME, DRAW_STA_WALK_12F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_D_12F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkD12F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_DOWN, AC_WALK_12F_FRAME, DRAW_STA_WALK_12F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_L_12F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkL12F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_LEFT, AC_WALK_12F_FRAME, DRAW_STA_WALK_12F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_R_12F 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkR12F_0( MMDL * mmdl )
{
	AcWalkOddWorkInit( mmdl, DIR_RIGHT, AC_WALK_12F_FRAME, DRAW_STA_WALK_12F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALK_*_12F 1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_Walk12F_1( MMDL * mmdl )
{
	if( AC_WalkOdd_Walk(mmdl,DATA_AcWalk12FMoveValueTbl) == TRUE ){
		return( TRUE );
	}
	
	return( FALSE );
}

//======================================================================
//	AC_PC_BOW PCWOMAN�����V
//======================================================================
//--------------------------------------------------------------
///	AC_PC_BOW_WORK
//--------------------------------------------------------------
typedef struct
{
	int frame;
}AC_PC_BOW_WORK;

#define AC_PC_BOW_WORK_SIZE (sizeof(AC_PC_BOW_WORK))

//--------------------------------------------------------------
/**
 * AC_PC_BOW	0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċN
 */
//--------------------------------------------------------------
static int AC_PcBow_0( MMDL * mmdl )
{
	AC_PC_BOW_WORK *work = MMDL_InitMoveCmdWork( mmdl, AC_PC_BOW_WORK_SIZE );
	MMDL_SetDrawStatus( mmdl, DRAW_STA_PC_BOW );
	MMDL_IncAcmdSeq( mmdl );
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_PC_BOW	1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċN
 */
//--------------------------------------------------------------
static int AC_PcBow_1( MMDL * mmdl )
{
	AC_PC_BOW_WORK *work = MMDL_GetMoveCmdWork( mmdl );
	
	work->frame++;
	
	if( work->frame >= 18 ){
		MMDL_SetDirDisp( mmdl, DIR_DOWN );
		MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
		MMDL_IncAcmdSeq( mmdl );
	}
	
	return( FALSE );
}

//======================================================================
//	AC_HIDE_PULLOFF �B�ꖪ�E��
//======================================================================
//--------------------------------------------------------------
///	AC_HIDE_PULLOFF
//--------------------------------------------------------------
typedef struct
{
	u32 frame;
}AC_HIDE_PULLOFF_WORK;

#define AC_HIDE_PULLOFF_WORK_SIZE (sizeof(AC_HIDE_PULLOFF_WORK))

//--------------------------------------------------------------
/**
 * AC_HIDE_PULLOFF 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_HidePullOFF_0( MMDL * mmdl )
{
	AC_HIDE_PULLOFF_WORK *work;

  work = MMDL_InitMoveCmdWork( mmdl, AC_HIDE_PULLOFF_WORK_SIZE );
	
	{
		FLDEFF_TASK *task = MMDL_GetMoveHideEffectTask( mmdl );
		if( task != NULL ){ FLDEFF_TASK_CallDelete( task ); }
	}
	
	{
		VecFx32 offs = { 0, 0, 0 };
		MMDL_SetVectorDrawOffsetPos( mmdl, &offs );
	}
	
#if 0 //�B�ꖪ�y��
	FE_mmdlHKemuri_Add( mmdl );
#endif

	MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START|MMDL_MOVEBIT_JUMP_START );
	MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH );
	
	MMDL_IncAcmdSeq( mmdl );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_HIDE_PULLOFF 1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_HidePullOFF_1( MMDL * mmdl )
{
	AC_HIDE_PULLOFF_WORK *work = MMDL_GetMoveCmdWork( mmdl );
	const fx32 *tbl = DATA_AcJumpOffsetTbl[AC_JUMP_HEIGHT_12];
	VecFx32 offs = { 0, 0, 0 };
	
	offs.y = tbl[work->frame];
	MMDL_SetVectorDrawOffsetPos( mmdl, &offs );
	
	work->frame += 2;
 	
	if( work->frame < AC_JUMP_H_TBL_MAX ){
		return( FALSE );
	}
	
	offs.y = 0;
	MMDL_SetVectorDrawOffsetPos( mmdl, &offs );
	
	MMDL_OnMoveBit( mmdl,
			MMDL_MOVEBIT_MOVE_END | 
      MMDL_MOVEBIT_JUMP_END | 
      MMDL_MOVEBIT_ACMD_END );
	
	MMDL_MoveHidePullOffFlagSet( mmdl );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//======================================================================
//	AC_HERO_BANZAI ���@����
//======================================================================
//--------------------------------------------------------------
///	AC_HERO_BANZAI
//--------------------------------------------------------------
typedef struct
{
	u16 frame;
  u16 frame_max;
}AC_HERO_ANM_WORK;

#define AC_HERO_ANM_WORK_SIZE (sizeof(AC_HERO_ANM_WORK))

//--------------------------------------------------------------
/**
 * AC_HERO_ANM_WORK�@������
 * @param mmdl  MMDL*
 * @param d_st �`��X�e�[�^�X
 * @param frame �A�j���t���[��
 * @retval nothing
 */
//--------------------------------------------------------------
static void AC_InitHeroWaitAnmWork( MMDL *mmdl, u16 d_st, u16 frame )
{
	AC_HERO_ANM_WORK *work;
  work = MMDL_InitMoveCmdWork( mmdl, AC_HERO_ANM_WORK_SIZE );
  work->frame_max = frame;
	MMDL_SetDrawStatus( mmdl, d_st);
}

//--------------------------------------------------------------
/**
 * AC_HERO_ANM_WORK�n�@�A�j���t���[���҂�
 * @param mmdl  MMDL*
 * @retval  int TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_HeroWaitAnmFrame_1( MMDL *mmdl )
{
	AC_HERO_ANM_WORK *work = MMDL_GetMoveCmdWork( mmdl );
  work->frame++;
  if( work->frame >= work->frame_max ){
	  MMDL_IncAcmdSeq( mmdl );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_HERO_BANZAI	0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċN
 */
//--------------------------------------------------------------
static int AC_HeroBanzai_0( MMDL * mmdl )
{
  AC_InitHeroWaitAnmWork( mmdl, DRAW_STA_PCAZUKE_ANM0, 40 );
	MMDL_IncAcmdSeq( mmdl );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_HERO_BANZAI_UKE	0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċN
 */
//--------------------------------------------------------------
static int AC_HeroBanzaiUke_0( MMDL * mmdl )
{
  AC_InitHeroWaitAnmWork( mmdl, DRAW_STA_PCAZUKE_ANM1, 40 );
	MMDL_IncAcmdSeq( mmdl );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_HERO_CUTIN 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċN
 */
//--------------------------------------------------------------
static int AC_HeroCutIn_0( MMDL * mmdl )
{
  AC_InitHeroWaitAnmWork( mmdl, 0, 12 );
	MMDL_IncAcmdSeq( mmdl );
	return( FALSE );
}

//======================================================================
//  ���@�A�C�e���Q�b�g
//======================================================================
//--------------------------------------------------------------
/// ITEMGET
//--------------------------------------------------------------
typedef struct
{
  int frame;
}AC_HERO_ITEMGET_WORK;

//--------------------------------------------------------------
/**
 * AC_HERO_ITEM_GET 0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċN
 */
//--------------------------------------------------------------
static int AC_HeroItemGet_0( MMDL * mmdl )
{
  AC_HERO_ITEMGET_WORK *work;
  work = MMDL_InitMoveCmdWork( mmdl, sizeof(AC_HERO_ITEMGET_WORK) );
	MMDL_SetDrawStatus( mmdl, DRAW_STA_ITEMGET_ANM );
	MMDL_IncAcmdSeq( mmdl );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_HERO_ITEM_GET 1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċN
 */
//--------------------------------------------------------------
static int AC_HeroItemGet_1( MMDL * mmdl )
{
  int no;
  AC_HERO_ITEMGET_WORK *work;
  VecFx32 offs = {0,0,0};
  fx32 y[10] = {
    NUM_FX32(0), NUM_FX32(0), NUM_FX32(0), NUM_FX32(0), NUM_FX32(0),
    NUM_FX32(4), NUM_FX32(4), NUM_FX32(3), NUM_FX32(1), 0 };
  
  work = MMDL_GetMoveCmdWork( mmdl );

  no = work->frame;
  if( no >= 10 ){
    no = 9;
  }
  
  offs.y = y[no];
  MMDL_SetVectorDrawOffsetPos( mmdl, &offs );
  
  work->frame++;

  if( work->frame >= 20 ){
    MMDL_IncAcmdSeq( mmdl );
  }
  
	return( FALSE );
}

//======================================================================
//	AC_WALKGL�n
//======================================================================
//--------------------------------------------------------------
/*
 * AC_WALKVEC_WORK������ 1G����
 * @param	mmdl	MMDL *
 * @param	vec		�ړ��x�N�g��*
 * @param	d_dir	�\������
 * @param	m_dir	�ړ�����
 * @param	wait	�ړ��t���[��
 * @param	draw	�`��X�e�[�^�X
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcWalkVecWorkInit( MMDL * mmdl,
	const VecFx32 *vec, int d_dir, int m_dir, int wait, u8 draw )
{
	AC_WALKVEC_WORK *work;
	work = MMDL_InitMoveCmdWork( mmdl, AC_WALKVEC_WORK_SIZE );
	work->wait = wait;
	work->vec = *vec;
	
	MMDL_SetDirDisp( mmdl, d_dir );
	MMDL_SetDirMove( mmdl, m_dir );
	MMDL_SetDrawStatus( mmdl, draw );
	MMDL_OnMoveBitMoveStart( mmdl );
	
	MMDL_SetOldGridPosX( mmdl, MMDL_GetGridPosX(mmdl) );
	MMDL_SetOldGridPosY( mmdl, MMDL_GetGridPosY(mmdl) );
	MMDL_SetOldGridPosZ( mmdl, MMDL_GetGridPosZ(mmdl) );
	if( vec->x < 0 ){ MMDL_AddGridPosX( mmdl, -GRID_ONE ); }
	else if( vec->x > 0 ){ MMDL_AddGridPosX( mmdl, GRID_ONE ); }
	if( vec->y < 0 ){ MMDL_AddGridPosY( mmdl, -GRID_ONE ); }
	else if( vec->y > 0 ){ MMDL_AddGridPosY( mmdl, GRID_ONE ); }
	if( vec->z < 0 ){ MMDL_AddGridPosZ( mmdl, -GRID_ONE ); }
	else if( vec->z > 0 ){ MMDL_AddGridPosZ( mmdl, GRID_ONE ); }
	
	MMDL_IncAcmdSeq( mmdl );
}

//--------------------------------------------------------------
/**
 * AC_WALKVEC�@�ړ��@�����擾�͍s��Ȃ�
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkVec_1( MMDL * mmdl )
{
	AC_WALKVEC_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	MMDL_AddVectorPos( mmdl, &work->vec );
	
	work->wait--;
	
	if( work->wait > 0 ){
		return( FALSE );
	}
	
	MMDL_OnMoveBit(
		mmdl, MMDL_MOVEBIT_MOVE_END|MMDL_MOVEBIT_ACMD_END );
	
	MMDL_UpdateGridPosCurrent( mmdl );
#if 0	//091030 �i�߂Ȃ� �A�j���֐������ŏ���
	MMDL_CallDrawProc( mmdl );
#endif
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGL_U_8F ���ǁ@��ړ�(y+) ���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGLU8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, GRID_VALUE_SPEED_8, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_LEFT, DIR_UP, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGL_D_8F ���ǁ@���ړ�(y-)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGLD8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, -GRID_VALUE_SPEED_8, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_RIGHT, DIR_DOWN, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGL_L_8F ���ǁ@���ړ�(z+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGLL8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, GRID_VALUE_SPEED_8 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_DOWN, DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGL_R_8F ���ǁ@�E�ړ�(z-)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGLR8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, -GRID_VALUE_SPEED_8 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_UP, DIR_RIGHT, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGR_U_8F �E�ǁ@��ړ�(y+)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGRU8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, GRID_VALUE_SPEED_8, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_RIGHT, DIR_UP, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGR_D_8F �E�ǁ@���ړ�(y-)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGRD8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, -GRID_VALUE_SPEED_8, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_LEFT, DIR_DOWN, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGR_L_8F �E�ǁ@���ړ�(z-)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGRL8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, -GRID_VALUE_SPEED_8 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_UP, DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGR_R_8F �E�ǁ@�E�ړ�(z+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGRR8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, GRID_VALUE_SPEED_8 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_DOWN, DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_8F ��ǁ@��ړ�(z+)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUU8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, GRID_VALUE_SPEED_8 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_DOWN, DIR_UP, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_8F ��ǁ@���ړ�(z-)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUD8F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, -GRID_VALUE_SPEED_8 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_UP, DIR_DOWN, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_8F ��ǁ@���ړ�(x-)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUL8F_0( MMDL * mmdl )
{
	VecFx32 vec = { -GRID_VALUE_SPEED_8, 0, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_RIGHT, DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_8F ��ǁ@�E�ړ�(x+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUR8F_0( MMDL * mmdl )
{
	VecFx32 vec = { GRID_VALUE_SPEED_8, 0, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_LEFT, DIR_RIGHT, GRID_FRAME_8, DRAW_STA_WALK_8F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_4F ��ǁ@��ړ�(z+)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUU4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, GRID_VALUE_SPEED_4 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_DOWN, DIR_UP, GRID_FRAME_4, DRAW_STA_WALK_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_4F ��ǁ@���ړ�(z-)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUD4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, -GRID_VALUE_SPEED_4 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_UP, DIR_DOWN, GRID_FRAME_4, DRAW_STA_WALK_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_4F ��ǁ@���ړ�(x-)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUL4F_0( MMDL * mmdl )
{
	VecFx32 vec = { -GRID_VALUE_SPEED_4, 0, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_RIGHT, DIR_LEFT, GRID_FRAME_4, DRAW_STA_WALK_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_4F ��ǁ@�E�ړ�(x+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUR4F_0( MMDL * mmdl )
{
	VecFx32 vec = { GRID_VALUE_SPEED_4, 0, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_LEFT, DIR_RIGHT, GRID_FRAME_4, DRAW_STA_WALK_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_2F ��ǁ@��ړ�(z+)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUU2F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, GRID_VALUE_SPEED_2 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_DOWN, DIR_UP, GRID_FRAME_2, DRAW_STA_WALK_2F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_2F ��ǁ@���ړ�(z-)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUD2F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, -GRID_VALUE_SPEED_2 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_UP, DIR_DOWN, GRID_FRAME_2, DRAW_STA_WALK_2F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_2F ��ǁ@���ړ�(x-)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUL2F_0( MMDL * mmdl )
{
	VecFx32 vec = { -GRID_VALUE_SPEED_2, 0, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_RIGHT, DIR_LEFT, GRID_FRAME_2, DRAW_STA_WALK_2F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_WALKGU_L_2F ��ǁ@�E�ړ�(x+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_WalkGUR2F_0( MMDL * mmdl )
{
	VecFx32 vec = { GRID_VALUE_SPEED_2, 0, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_LEFT, DIR_RIGHT, GRID_FRAME_2, DRAW_STA_WALK_2F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGL_U_4F ���ǁ@��ړ�(y+) ���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGLU4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, GRID_VALUE_SPEED_4, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_LEFT, DIR_UP, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGL_D_4F ���ǁ@���ړ�(y-)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGLD4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, -GRID_VALUE_SPEED_4, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_RIGHT, DIR_DOWN, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGL_L_4F ���ǁ@���ړ�(z+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGLL4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, GRID_VALUE_SPEED_4 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_DOWN, DIR_LEFT, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGL_R_4F ���ǁ@�E�ړ�(z-)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGLR4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, -GRID_VALUE_SPEED_4 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_UP, DIR_RIGHT, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGR_U_4F �E�ǁ@��ړ�(y+)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGRU4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, GRID_VALUE_SPEED_4, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_RIGHT, DIR_UP, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGR_D_4F �E�ǁ@���ړ�(y-)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGRD4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, -GRID_VALUE_SPEED_4, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_LEFT, DIR_DOWN, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGR_L_4F �E�ǁ@���ړ�(z-)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGRL4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, -GRID_VALUE_SPEED_4 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_UP, DIR_LEFT, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGR_L_4F �E�ǁ@�E�ړ�(z+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGRR4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, GRID_VALUE_SPEED_4 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_DOWN, DIR_LEFT, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGU_L_4F ��ǁ@��ړ�(z+)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGUU4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, GRID_VALUE_SPEED_4 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_DOWN, DIR_UP, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGU_L_4F ��ǁ@���ړ�(z-)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGUD4F_0( MMDL * mmdl )
{
	VecFx32 vec = { 0, 0, -GRID_VALUE_SPEED_4 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_UP, DIR_DOWN, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGU_L_4F ��ǁ@���ړ�(x-)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGUL4F_0( MMDL * mmdl )
{
	VecFx32 vec = { -GRID_VALUE_SPEED_4, 0, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_RIGHT, DIR_LEFT, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_DASHGU_L_4F ��ǁ@�E�ړ�(x+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_DashGUR4F_0( MMDL * mmdl )
{
	VecFx32 vec = { GRID_VALUE_SPEED_4, 0, 0 };
	AcWalkVecWorkInit( mmdl, &vec,
		DIR_LEFT, DIR_RIGHT, GRID_FRAME_4, DRAW_STA_HERO_DASH_4F );
	return( TRUE );
}

//======================================================================
//	AC_JUMPGL�n
//======================================================================
//--------------------------------------------------------------
/**
 * AC_JUMPVEC_WORK������
 * @param	mmdl		MMDL *
 * @param	val			�ړ���
 * @param	d_dir		�\������
 * @param	m_dir		�ړ�����
 * @param	wait		�ړ��t���[��
 * @param	draw		�`��X�e�[�^�X
 * @param	vec_type	�x�N�g���^�C�v VEC_X��
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AcJumpVecWorkInit( MMDL * mmdl,
	fx32 val, int d_dir, int m_dir, u8 wait, u8 draw,
	u8 vec_type, u8 jump_vec_type, u8 jump_flip )
{
	int grid = GRID_ONE;
	AC_JUMPVEC_WORK *work;
	work = MMDL_InitMoveCmdWork( mmdl, AC_JUMPVEC_WORK_SIZE );
	
	work->wait = wait;
	work->val = val;
	work->vec_type = vec_type;
	work->jump_vec_type = jump_vec_type;
	work->jump_flip = jump_flip;
	work->jump_frame_val = NUM_UX16(16) / work->wait;
	
	MMDL_SetDirDisp( mmdl, d_dir );
	MMDL_SetDirMove( mmdl, m_dir );
	MMDL_SetDrawStatus( mmdl, draw );
	MMDL_OnMoveBitMoveStart( mmdl );
	
	MMDL_SetOldGridPosX( mmdl, MMDL_GetGridPosX(mmdl) );
	MMDL_SetOldGridPosY( mmdl, MMDL_GetGridPosY(mmdl) );
	MMDL_SetOldGridPosZ( mmdl, MMDL_GetGridPosZ(mmdl) );
	
	GF_ASSERT( vec_type <= VEC_Z );
	
	if( val ){		//�ړ��ʂ����݂���
		switch( vec_type ){
		case VEC_X:
			if( val < 0 ){ grid = -grid; }
			MMDL_AddGridPosX( mmdl, grid );
			break;
		case VEC_Y:
			if( val < 0 ){ grid = -grid; }
			MMDL_AddGridPosY( mmdl, grid*2 );
			break;
		case VEC_Z:
			if( val < 0 ){ grid = -grid; }
			MMDL_AddGridPosZ( mmdl, grid );
			break;
		}
	}
	
	MMDL_IncAcmdSeq( mmdl );
}

//--------------------------------------------------------------
/**
 * AC_JUMPVEC_WORK�@�ړ��@�����擾�͍s��Ȃ�
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpVec_1( MMDL * mmdl )
{
	fx32 val;
	VecFx32 pos;
	AC_JUMPVEC_WORK *work;
	
	work = MMDL_GetMoveCmdWork( mmdl );
	MMDL_GetVectorPos( mmdl, &pos );
	
	switch( work->vec_type ){
	case VEC_X:
		pos.x += work->val; 
		break;
	case VEC_Y:
		pos.y += work->val; 
		break;
	case VEC_Z:
		pos.z += work->val; 
		break;
	}
	
	MMDL_SetVectorPos( mmdl, &pos );
	
	val = work->val;
	if( val < 0 ){ val = -val; }
	work->count += val;
	
	work->jump_frame += work->jump_frame_val;
	
	if( work->jump_frame > AC_JUMP_H_TBL_FRAME_MAX_UX16 ){
		work->jump_frame = AC_JUMP_H_TBL_FRAME_MAX_UX16;
	}
	
	{
		VecFx32 offs = {0,0,0};
		const fx32 *tbl = DATA_AcJumpOffsetTbl[AC_JUMP_HEIGHT_8];
		u16 frame = UX16_NUM( work->jump_frame );
		
		val = tbl[frame];
		if( work->jump_flip == TRUE ){
			val = -val;
		}
		
		switch( work->jump_vec_type ){
		case VEC_X: offs.x = val; break;
		case VEC_Y: offs.y = val; break;
		case VEC_Z: offs.z = val; break;
		}
		
		MMDL_SetVectorDrawOffsetPos( mmdl, &offs );
	}
	
	work->wait--;
	
	//�O���b�h�z��&�E�F�C�g�c��
	if( work->count >= GRID_FX32 && work->wait ){
		int grid = GRID_ONE;
		work->count -= GRID_FX32;
		val = work->val;
		
		MMDL_SetOldGridPosX( mmdl, MMDL_GetGridPosX(mmdl) );
		MMDL_SetOldGridPosY( mmdl, MMDL_GetGridPosY(mmdl) );
		MMDL_SetOldGridPosZ( mmdl, MMDL_GetGridPosZ(mmdl) );
		
		switch( work->vec_type ){
		case VEC_X:
			if( val < 0 ){ grid = -grid; }
			MMDL_AddGridPosX( mmdl, grid );
			break;
		case VEC_Y:
			if( val < 0 ){ grid = -grid; }
			MMDL_AddGridPosY( mmdl, grid*2 );
			break;
		case VEC_Z:
			if( val < 0 ){ grid = -grid; }
			MMDL_AddGridPosZ( mmdl, grid );
			break;
		}
	}
	
	if( work->wait > 0 ){
		return( FALSE );
	}
	
	{
		VecFx32 offs = {0,0,0};
		MMDL_SetVectorDrawOffsetPos( mmdl, &offs );
	}
	
	MMDL_OnMoveBit( mmdl,
			MMDL_MOVEBIT_MOVE_END |
			MMDL_MOVEBIT_JUMP_END |
			MMDL_MOVEBIT_ACMD_END );
	
	MMDL_UpdateGridPosCurrent( mmdl );
#if 0	//091030 �i�߂Ȃ� �A�j���֐������ŏ���
	MMDL_CallDrawProc( mmdl );				//1�t���[���i�߂�
#endif
	MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
	MMDL_IncAcmdSeq( mmdl );
	
#if 0
	Snd_SePlay( SE_SUTYA2 );
#endif
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGL_U_1G_8F ���ǁ@��ړ�(y+) ���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGLU1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, GRID_VALUE_SPEED_8,
		DIR_LEFT,DIR_UP, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Y, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGL_D_1G_8F ���ǁ@���ړ�(y-)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGLD1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, -GRID_VALUE_SPEED_8,
		DIR_RIGHT,DIR_DOWN, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Y, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGL_L_1G_8F ���ǁ@���ړ�(z+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGLL1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, GRID_VALUE_SPEED_8,
		DIR_DOWN,DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Z, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGL_R_1G_8F ���ǁ@�E�ړ�(z-)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGLR1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, -GRID_VALUE_SPEED_8,
		DIR_UP,DIR_RIGHT, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Z, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGR_U_1G_8F �E�ǁ@��ړ�(y+)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGRU1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, GRID_VALUE_SPEED_8,
		DIR_RIGHT,DIR_UP, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Y, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGR_D_1G_8F �E�ǁ@���ړ�(y-)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGRD1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, -GRID_VALUE_SPEED_8,
		DIR_LEFT,DIR_DOWN, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Y, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGR_L_1G_8F �E�ǁ@���ړ�(z-)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGRL1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, -GRID_VALUE_SPEED_8,
		DIR_UP,DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Z, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGR_R_1G_8F �E�ǁ@�E�ړ�(z+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGRR1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, GRID_VALUE_SPEED_8,
		DIR_DOWN,DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Z, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGU_U_1G_8F ��ǁ@��ړ�(z+)�@��\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGUU1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, GRID_VALUE_SPEED_8,
		DIR_DOWN,DIR_UP, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Z, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGU_D_1G_8F ��ǁ@���ړ�(z-)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGUD1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, -GRID_VALUE_SPEED_8,
		DIR_UP,DIR_DOWN, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_Z, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGU_L_1G_8F ��ǁ@���ړ�(x-)�@�E�\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGUL1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, -GRID_VALUE_SPEED_8,
		DIR_RIGHT,DIR_LEFT, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_X, VEC_Y, FALSE );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * AC_JUMPGU_R_1G_8F ��ǁ@�E�ړ�(x+)�@���\��
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AC_JumpGUR1G_8F_0( MMDL * mmdl )
{
	AcJumpVecWorkInit( mmdl, GRID_VALUE_SPEED_8,
		DIR_LEFT,DIR_RIGHT, GRID_FRAME_8, DRAW_STA_WALK_8F,
		VEC_X, VEC_Y, FALSE );
	return( TRUE );
}

//======================================================================
//	AC_SPIDER_REMOVE �����̑������A�j��
//======================================================================
//--------------------------------------------------------------
///	AC_SPIDER_REMOVE_WORK
//--------------------------------------------------------------
typedef struct
{
	int frame;
}AC_SPIDER_REMOVE_WORK;

#define AC_SPIDER_REMOVE_WORK_SIZE (sizeof(AC_SPIDER_REMOVE_WORK))

//--------------------------------------------------------------
/**
 * AC_SPIDER_REMOVE	0
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċN
 */
//--------------------------------------------------------------
static int AC_SpiderRemove_0( MMDL * mmdl )
{
	AC_SPIDER_REMOVE_WORK *work =
    MMDL_InitMoveCmdWork( mmdl, AC_SPIDER_REMOVE_WORK_SIZE );
	MMDL_SetDrawStatus( mmdl, DRAW_STA_SPIDER_REMOVE );
	MMDL_IncAcmdSeq( mmdl );
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * AC_SPIDER_REMOVE	1
 * @param	mmdl	MMDL *
 * @retval	int		TRUE=�ċN
 */
//--------------------------------------------------------------
static int AC_SpiderRemove_1( MMDL * mmdl )
{
	AC_SPIDER_REMOVE_WORK *work = MMDL_GetMoveCmdWork( mmdl );
	
	work->frame++;
	
	if( work->frame >= 22 ){
		MMDL_IncAcmdSeq( mmdl );
	}
	
	return( FALSE );
}

//======================================================================
//	data	�A�j���[�V�����R�}���h�e�[�u��
//======================================================================
//--------------------------------------------------------------
///	AC_DIR_U
//--------------------------------------------------------------
int (* const DATA_AC_DirU_Tbl[])( MMDL * ) =
{
	AC_DirU_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DIR_D
//--------------------------------------------------------------
int (* const DATA_AC_DirD_Tbl[])( MMDL * ) =
{
	AC_DirD_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DIR_L
//--------------------------------------------------------------
int (* const DATA_AC_DirL_Tbl[])( MMDL * ) =
{
	AC_DirL_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DIR_R
//--------------------------------------------------------------
int (* const DATA_AC_DirR_Tbl[])( MMDL * ) =
{
	AC_DirR_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_32F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU_32F_Tbl[])( MMDL * ) =
{
	AC_WalkU32F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_32F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD_32F_Tbl[])( MMDL * ) =
{
	AC_WalkD32F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_32F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL_32F_Tbl[])( MMDL * ) =
{
	AC_WalkL32F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_32F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR_32F_Tbl[])( MMDL * ) =
{
	AC_WalkR32F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_16F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU_16F_Tbl[])( MMDL * ) =
{
	AC_WalkU16F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_16F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD_16F_Tbl[])( MMDL * ) =
{
	AC_WalkD16F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_16F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL_16F_Tbl[])( MMDL * ) =
{
	AC_WalkL16F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_16F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR_16F_Tbl[])( MMDL * ) =
{
	AC_WalkR16F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU_8F_Tbl[])( MMDL * ) =
{
	AC_WalkU8F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD_8F_Tbl[])( MMDL * ) =
{
	AC_WalkD8F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL_8F_Tbl[])( MMDL * ) =
{
	AC_WalkL8F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR_8F_Tbl[])( MMDL * ) =
{
	AC_WalkR8F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_4F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU_4F_Tbl[])( MMDL * ) =
{
	AC_WalkU4F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_4F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD_4F_Tbl[])( MMDL * ) =
{
	AC_WalkD4F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_4F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL_4F_Tbl[])( MMDL * ) =
{
	AC_WalkL4F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_4F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR_4F_Tbl[])( MMDL * ) =
{
	AC_WalkR4F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_2F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU_2F_Tbl[])( MMDL * ) =
{
	AC_WalkU2F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_2F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD_2F_Tbl[])( MMDL * ) =
{
	AC_WalkD2F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_2F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL_2F_Tbl[])( MMDL * ) =
{
	AC_WalkL2F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_2F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR_2F_Tbl[])( MMDL * ) =
{
	AC_WalkR2F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_1F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU_1F_Tbl[])( MMDL * ) =
{
	AC_WalkU1F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_1F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD_1F_Tbl[])( MMDL * ) =
{
	AC_WalkD1F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_1F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL_1F_Tbl[])( MMDL * ) =
{
	AC_WalkL1F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_1F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR_1F_Tbl[])( MMDL * ) =
{
	AC_WalkR1F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_U_32F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkU_32F_Tbl[])( MMDL * ) =
{
	AC_StayWalkU32F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_D_32F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkD_32F_Tbl[])( MMDL * ) =
{
	AC_StayWalkD32F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_L_32F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkL_32F_Tbl[])( MMDL * ) =
{
	AC_StayWalkL32F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_R_32F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkR_32F_Tbl[])( MMDL * ) =
{
	AC_StayWalkR32F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_U_16F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkU_16F_Tbl[])( MMDL * ) =
{
	AC_StayWalkU16F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_D_16F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkD_16F_Tbl[])( MMDL * ) =
{
	AC_StayWalkD16F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_L_16F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkL_16F_Tbl[])( MMDL * ) =
{
	AC_StayWalkL16F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_R_16F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkR_16F_Tbl[])( MMDL * ) =
{
	AC_StayWalkR16F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_U_8F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkU_8F_Tbl[])( MMDL * ) =
{
	AC_StayWalkU8F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_D_8F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkD_8F_Tbl[])( MMDL * ) =
{
	AC_StayWalkD8F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_L_8F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkL_8F_Tbl[])( MMDL * ) =
{
	AC_StayWalkL8F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_R_8F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkR_8F_Tbl[])( MMDL * ) =
{
	AC_StayWalkR8F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_U_4F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkU_4F_Tbl[])( MMDL * ) =
{
	AC_StayWalkU4F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_D_4F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkD_4F_Tbl[])( MMDL * ) =
{
	AC_StayWalkD4F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_L_4F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkL_4F_Tbl[])( MMDL * ) =
{
	AC_StayWalkL4F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_R_4F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkR_4F_Tbl[])( MMDL * ) =
{
	AC_StayWalkR4F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_U_2F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkU_2F_Tbl[])( MMDL * ) =
{
	AC_StayWalkU2F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_D_2F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkD_2F_Tbl[])( MMDL * ) =
{
	AC_StayWalkD2F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_L_2F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkL_2F_Tbl[])( MMDL * ) =
{
	AC_StayWalkL2F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_WALK_R_2F
//--------------------------------------------------------------
int (* const DATA_AC_StayWalkR_2F_Tbl[])( MMDL * ) =
{
	AC_StayWalkR2F_0,
	AC_StayWalk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_JUMP_U_16F
//--------------------------------------------------------------
int (* const DATA_AC_StayJumpU_16F_Tbl[])( MMDL * ) =
{
	AC_StayJumpU16F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_JUMP_D_16F
//--------------------------------------------------------------
int (* const DATA_AC_StayJumpD_16F_Tbl[])( MMDL * ) =
{
	AC_StayJumpD16F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_JUMP_L_16F
//--------------------------------------------------------------
int (* const DATA_AC_StayJumpL_16F_Tbl[])( MMDL * ) =
{
	AC_StayJumpL16F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_JUMP_R_16F
//--------------------------------------------------------------
int (* const DATA_AC_StayJumpR_16F_Tbl[])( MMDL * ) =
{
	AC_StayJumpR16F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_JUMP_U_8F
//--------------------------------------------------------------
int (* const DATA_AC_StayJumpU_8F_Tbl[])( MMDL * ) =
{
	AC_StayJumpU8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_JUMP_D_8F
//--------------------------------------------------------------
int (* const DATA_AC_StayJumpD_8F_Tbl[])( MMDL * ) =
{
	AC_StayJumpD8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_JUMP_L_8F
//--------------------------------------------------------------
int (* const DATA_AC_StayJumpL_8F_Tbl[])( MMDL * ) =
{
	AC_StayJumpL8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_STAY_JUMP_R_8F
//--------------------------------------------------------------
int (* const DATA_AC_StayJumpR_8F_Tbl[])( MMDL * ) =
{
	AC_StayJumpR8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_U_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpU_1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpU1G8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_D_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpD_1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpD1G8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_L_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpL_1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpL1G8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_R_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpR_1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpR1G8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_U_2G_16F
//--------------------------------------------------------------
int (* const DATA_AC_JumpU_2G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpU2G8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_D_2G_16F
//--------------------------------------------------------------
int (* const DATA_AC_JumpD_2G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpD2G8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_L_2G_16F
//--------------------------------------------------------------
int (* const DATA_AC_JumpL_2G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpL2G8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_R_2G_16F
//--------------------------------------------------------------
int (* const DATA_AC_JumpR_2G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpR2G8F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPHI_L_1G_16F
//--------------------------------------------------------------
int (* const DATA_AC_JumpHiL_1G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpHiL1G16F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPHI_R_1G_16F
//--------------------------------------------------------------
int (* const DATA_AC_JumpHiR_1G_16F_Tbl[])( MMDL * ) =
{
	AC_JumpHiR1G16F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPHI_L_3G_32F
//--------------------------------------------------------------
int (* const DATA_AC_JumpHiL_3G_32F_Tbl[])( MMDL * ) =
{
	AC_JumpHiL3G32F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPHI_R_3G_32F
//--------------------------------------------------------------
int (* const DATA_AC_JumpHiR_3G_32F_Tbl[])( MMDL * ) =
{
	AC_JumpHiR3G32F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WAIT_1F
//--------------------------------------------------------------
int (* const DATA_AC_Wait_1F_Tbl[])( MMDL * ) =
{
	AC_Wait1F_0,
	AC_Wait_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WAIT_2F
//--------------------------------------------------------------
int (* const DATA_AC_Wait_2F_Tbl[])( MMDL * ) =
{
	AC_Wait2F_0,
	AC_Wait_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WAIT_4F
//--------------------------------------------------------------
int (* const DATA_AC_Wait_4F_Tbl[])( MMDL * ) =
{
	AC_Wait4F_0,
	AC_Wait_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WAIT_8F
//--------------------------------------------------------------
int (* const DATA_AC_Wait_8F_Tbl[])( MMDL * ) =
{
	AC_Wait8F_0,
	AC_Wait_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WAIT_15F
//--------------------------------------------------------------
int (* const DATA_AC_Wait_15F_Tbl[])( MMDL * ) =
{
	AC_Wait15F_0,
	AC_Wait_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WAIT_16F
//--------------------------------------------------------------
int (* const DATA_AC_Wait_16F_Tbl[])( MMDL * ) =
{
	AC_Wait16F_0,
	AC_Wait_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WAIT_32F
//--------------------------------------------------------------
int (* const DATA_AC_Wait_32F_Tbl[])( MMDL * ) =
{
	AC_Wait32F_0,
	AC_Wait_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WARP_UP
//--------------------------------------------------------------
int (* const DATA_AC_WarpUp_Tbl[])( MMDL * ) =
{
	AC_WarpUp_0,
	AC_WarpUp_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WARP_DOWN
//--------------------------------------------------------------
int (* const DATA_AC_WarpDown_Tbl[])( MMDL * ) =
{
	AC_WarpDown_0,
	AC_WarpDown_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_VANISH_ON
//--------------------------------------------------------------
int (* const DATA_AC_VanishON_Tbl[])( MMDL * ) =
{
	AC_VanishON_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_VANISH_OFF
//--------------------------------------------------------------
int (* const DATA_AC_VanishOFF_Tbl[])( MMDL * ) =
{
	AC_VanishOFF_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DIR_PAUSE_ON
//--------------------------------------------------------------
int (* const DATA_AC_DirPauseON_Tbl[])( MMDL * ) =
{
	AC_DirPauseON_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DIR_PAUSE_OFF
//--------------------------------------------------------------
int (* const DATA_AC_DirPauseOFF_Tbl[])( MMDL * ) =
{
	AC_DirPauseOFF_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_ANM_PAUSE_ON
//--------------------------------------------------------------
int (* const DATA_AC_AnmPauseON_Tbl[])( MMDL * ) =
{
	AC_AnmPauseON_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_ANM_PAUSE_OFF
//--------------------------------------------------------------
int (* const DATA_AC_AnmPauseOFF_Tbl[])( MMDL * ) =
{
	AC_AnmPauseOFF_0,
	AC_End,
};

//--------------------------------------------------------------
///	AC_MARK_GYOE 0
//--------------------------------------------------------------
int (* const DATA_AC_MarkGyoe_Tbl[])( MMDL * ) =
{
	AC_MarkGyoe_0,
	AC_Mark_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_MARK_SAISEN 0
//--------------------------------------------------------------
int (* const DATA_AC_MarkSaisen_Tbl[])( MMDL * ) =
{
	AC_MarkSaisen_0,
	AC_Mark_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_MARK_HATENA_0
//--------------------------------------------------------------
int (* const DATA_AC_MarkHatena_Tbl[])( MMDL * ) =
{
	AC_MarkHatena_0,
	AC_Mark_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_MARK_ONPU_0
//--------------------------------------------------------------
int (* const DATA_AC_MarkOnpu_Tbl[])( MMDL * ) =
{
	AC_MarkOnpu_0,
	AC_Mark_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_MARK_TENTEN_0
//--------------------------------------------------------------
int (* const DATA_AC_MarkTenTen_Tbl[])( MMDL * ) =
{
	AC_MarkTenTen_0,
	AC_Mark_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_6F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU6F_Tbl[])( MMDL * ) =
{
	AC_WalkU6F_0,
	AC_Walk6F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_6F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD6F_Tbl[])( MMDL * ) =
{
	AC_WalkD6F_0,
	AC_Walk6F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_6F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL6F_Tbl[])( MMDL * ) =
{
	AC_WalkL6F_0,
	AC_Walk6F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_6F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR6F_Tbl[])( MMDL * ) =
{
	AC_WalkR6F_0,
	AC_Walk6F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASH_U_6F
//--------------------------------------------------------------
int (* const DATA_AC_DashU6F_Tbl[])( MMDL * ) =
{
	AC_DashU6F_0,
	AC_Walk6F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASH_D_6F
//--------------------------------------------------------------
int (* const DATA_AC_DashD6F_Tbl[])( MMDL * ) =
{
	AC_DashD6F_0,
	AC_Walk6F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASH_L_6F
//--------------------------------------------------------------
int (* const DATA_AC_DashL6F_Tbl[])( MMDL * ) =
{
	AC_DashL6F_0,
	AC_Walk6F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASH_R_6F
//--------------------------------------------------------------
int (* const DATA_AC_DashR6F_Tbl[])( MMDL * ) =
{
	AC_DashR6F_0,
	AC_Walk6F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_3F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU3F_Tbl[])( MMDL * ) =
{
	AC_WalkU3F_0,
	AC_Walk3F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_3F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD3F_Tbl[])( MMDL * ) =
{
	AC_WalkD3F_0,
	AC_Walk3F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_3F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL3F_Tbl[])( MMDL * ) =
{
	AC_WalkL3F_0,
	AC_Walk3F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_3F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR3F_Tbl[])( MMDL * ) =
{
	AC_WalkR3F_0,
	AC_Walk3F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASH_U_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashU_4F_Tbl[])( MMDL * ) =
{
	AC_DashU4F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASH_D_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashD_4F_Tbl[])( MMDL * ) =
{
	AC_DashD4F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASH_L_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashL_4F_Tbl[])( MMDL * ) =
{
	AC_DashL4F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASH_R_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashR_4F_Tbl[])( MMDL * ) =
{
	AC_DashR4F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_7F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU7F_Tbl[])( MMDL * ) =
{
	AC_WalkU7F_0,
	AC_Walk7F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_7F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD7F_Tbl[])( MMDL * ) =
{
	AC_WalkD7F_0,
	AC_Walk7F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_7F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL7F_Tbl[])( MMDL * ) =
{
	AC_WalkL7F_0,
	AC_Walk7F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_7F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR7F_Tbl[])( MMDL * ) =
{
	AC_WalkR7F_0,
	AC_Walk7F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_U_12F
//--------------------------------------------------------------
int (* const DATA_AC_WalkU12F_Tbl[])( MMDL * ) =
{
	AC_WalkU12F_0,
	AC_Walk12F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_D_12F
//--------------------------------------------------------------
int (* const DATA_AC_WalkD12F_Tbl[])( MMDL * ) =
{
	AC_WalkD12F_0,
	AC_Walk12F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_L_12F
//--------------------------------------------------------------
int (* const DATA_AC_WalkL12F_Tbl[])( MMDL * ) =
{
	AC_WalkL12F_0,
	AC_Walk12F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALK_R_12F
//--------------------------------------------------------------
int (* const DATA_AC_WalkR12F_Tbl[])( MMDL * ) =
{
	AC_WalkR12F_0,
	AC_Walk12F_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_PC_BOW
//--------------------------------------------------------------
int (* const DATA_AC_PcBow_Tbl[])( MMDL * ) =
{
	AC_PcBow_0,
	AC_PcBow_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_HIDE_PULLOFF
//--------------------------------------------------------------
int (* const DATA_AC_HidePullOFF_Tbl[])( MMDL * ) =
{
	AC_HidePullOFF_0,
	AC_HidePullOFF_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_HERO_BANZAI
//--------------------------------------------------------------
int (* const DATA_AC_HeroBanzai_Tbl[])( MMDL * ) =
{
	AC_HeroBanzai_0,
	AC_HeroWaitAnmFrame_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_HERO_BANZAI_UKE
//--------------------------------------------------------------
int (* const DATA_AC_HeroBanzaiUke_Tbl[])( MMDL * ) =
{
	AC_HeroBanzaiUke_0,
	AC_HeroWaitAnmFrame_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_HERO_CUTIN
//--------------------------------------------------------------
int (* const DATA_AC_HeroCutin_Tbl[])( MMDL * ) =
{
  AC_HeroCutIn_0,
	AC_HeroWaitAnmFrame_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGL_U_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGLU8F_Tbl[])( MMDL * ) =
{
	AC_WalkGLU8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGL_D_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGLD8F_Tbl[])( MMDL * ) =
{
	AC_WalkGLD8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGL_L_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGLL8F_Tbl[])( MMDL * ) =
{
	AC_WalkGLL8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGL_R_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGLR8F_Tbl[])( MMDL * ) =
{
	AC_WalkGLR8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGR_U_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGRU8F_Tbl[])( MMDL * ) =
{
	AC_WalkGRU8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGL_D_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGRD8F_Tbl[])( MMDL * ) =
{
	AC_WalkGRD8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGR_L_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGRL8F_Tbl[])( MMDL * ) =
{
	AC_WalkGRL8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGR_R_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGRR8F_Tbl[])( MMDL * ) =
{
	AC_WalkGRR8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_U_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUU8F_Tbl[])( MMDL * ) =
{
	AC_WalkGUU8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_D_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUD8F_Tbl[])( MMDL * ) =
{
	AC_WalkGUD8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_L_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUL8F_Tbl[])( MMDL * ) =
{
	AC_WalkGUL8F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_R_8F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUR8F_Tbl[])( MMDL * ) =
{
	AC_WalkGUR8F_0,
	AC_WalkVec_1,
	AC_End,
};
//--------------------------------------------------------------
///	AC_WALKGU_U_4F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUU4F_Tbl[])( MMDL * ) =
{
	AC_WalkGUU4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_D_4F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUD4F_Tbl[])( MMDL * ) =
{
	AC_WalkGUD4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_L_4F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUL4F_Tbl[])( MMDL * ) =
{
	AC_WalkGUL4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_R_4F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUR4F_Tbl[])( MMDL * ) =
{
	AC_WalkGUR4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_U_2F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUU2F_Tbl[])( MMDL * ) =
{
	AC_WalkGUU2F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_D_2F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUD2F_Tbl[])( MMDL * ) =
{
	AC_WalkGUD2F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_L_2F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUL2F_Tbl[])( MMDL * ) =
{
	AC_WalkGUL2F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_WALKGU_R_2F
//--------------------------------------------------------------
int (* const DATA_AC_WalkGUR2F_Tbl[])( MMDL * ) =
{
	AC_WalkGUR2F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_U_3G_24F
//--------------------------------------------------------------
int (* const DATA_AC_JumpU_3G_24F_Tbl[])( MMDL * ) =
{
	AC_JumpU3G24F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_D_3G_24F
//--------------------------------------------------------------
int (* const DATA_AC_JumpD_3G_24F_Tbl[])( MMDL * ) =
{
	AC_JumpD3G24F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_L_3G_24F
//--------------------------------------------------------------
int (* const DATA_AC_JumpL_3G_24F_Tbl[])( MMDL * ) =
{
	AC_JumpL3G24F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMP_R_3G_24F
//--------------------------------------------------------------
int (* const DATA_AC_JumpR_3G_24F_Tbl[])( MMDL * ) =
{
	AC_JumpR3G24F_0,
	AC_Jump_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGL_U_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGLU4F_Tbl[])( MMDL * ) =
{
	AC_DashGLU4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGL_D_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGLD4F_Tbl[])( MMDL * ) =
{
	AC_DashGLD4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGL_L_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGLL4F_Tbl[])( MMDL * ) =
{
	AC_DashGLL4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGL_R_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGLR4F_Tbl[])( MMDL * ) =
{
	AC_DashGLR4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGR_U_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGRU4F_Tbl[])( MMDL * ) =
{
	AC_DashGRU4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGL_D_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGRD4F_Tbl[])( MMDL * ) =
{
	AC_DashGRD4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGR_L_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGRL4F_Tbl[])( MMDL * ) =
{
	AC_DashGRL4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGR_R_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGRR4F_Tbl[])( MMDL * ) =
{
	AC_DashGRR4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGU_U_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGUU4F_Tbl[])( MMDL * ) =
{
	AC_DashGUU4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGU_D_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGUD4F_Tbl[])( MMDL * ) =
{
	AC_DashGUD4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGU_L_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGUL4F_Tbl[])( MMDL * ) =
{
	AC_DashGUL4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_DASHGU_R_4F
//--------------------------------------------------------------
int (* const DATA_AC_DashGUR4F_Tbl[])( MMDL * ) =
{
	AC_DashGUR4F_0,
	AC_WalkVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGL_U_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGLU1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGLU1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGL_D_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGLD1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGLD1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGL_L_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGLL1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGLL1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGL_R_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGLR1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGLR1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGR_U_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGRU1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGRU1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGL_D_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGRD1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGRD1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGR_L_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGRL1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGRL1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGR_R_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGRR1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGRR1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGU_U_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGUU1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGUU1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGU_D_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGUD1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGUD1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGU_L_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGUL1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGUL1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_JUMPGU_R_1G_8F
//--------------------------------------------------------------
int (* const DATA_AC_JumpGUR1G_8F_Tbl[])( MMDL * ) =
{
	AC_JumpGUR1G_8F_0,
	AC_JumpVec_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_MARK_GYOE_TWAIT 0
//--------------------------------------------------------------
int (* const DATA_AC_MarkGyoeTWait_Tbl[])( MMDL * ) =
{
	AC_MarkGyoeTWait_0,
	AC_Mark_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_HERO_ITEMGET
//--------------------------------------------------------------
int (* const DATA_AC_HeroItemGet_Tbl[])( MMDL * ) =
{
	AC_HeroItemGet_0,
  AC_HeroItemGet_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_SANDWALK_U_16F
//--------------------------------------------------------------
int (* const DATA_AC_SandWalkU_16F_Tbl[])( MMDL * ) =
{
	AC_SandWalkU16F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_SANDWALK_D_16F
//--------------------------------------------------------------
int (* const DATA_AC_SandWalkD_16F_Tbl[])( MMDL * ) =
{
	AC_SandWalkD16F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_SANDWALK_L_16F
//--------------------------------------------------------------
int (* const DATA_AC_SandWalkL_16F_Tbl[])( MMDL * ) =
{
	AC_SandWalkL16F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_SANDWALK_R_16F
//--------------------------------------------------------------
int (* const DATA_AC_SandWalkR_16F_Tbl[])( MMDL * ) =
{
	AC_SandWalkR16F_0,
	AC_Walk_1,
	AC_End,
};

//--------------------------------------------------------------
///	AC_SPIDER_REMOVE
//--------------------------------------------------------------
int (* const DATA_AC_SpiderRemove_Tbl[])( MMDL * ) =
{
	AC_SpiderRemove_0,
	AC_SpiderRemove_1,
	AC_End,
};

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

//--------------------------------------------------------------
///	AC_WALK_*_7F�@�ړ��e�[�u��
//--------------------------------------------------------------
static const fx32 DATA_AcWalk7FMoveValueTbl[AC_WALK_7F_FRAME] =
{
	NUM_FX32(2)+0x500, NUM_FX32(2)+0x480, NUM_FX32(2)+0x480,NUM_FX32(2)+0x480,
	NUM_FX32(2)+0x480, NUM_FX32(2)+0x480, NUM_FX32(2)+0x480
};

#if 0
static const fx32 DATA_AcWalk7FMoveValueTbl[AC_WALK_7F_FRAME] =
{
	FX32_ONE*3, FX32_ONE*2, FX32_ONE*2,FX32_ONE*2,
	FX32_ONE*3, FX32_ONE*2, FX32_ONE*2
};
#endif

//--------------------------------------------------------------
///	AC_WALK_*_6F�@�ړ��e�[�u��
//--------------------------------------------------------------
static const fx32 DATA_AcWalk6FMoveValueTbl[AC_WALK_6F_FRAME] =
{
	FX32_ONE*2, FX32_ONE*3, FX32_ONE*3,
	FX32_ONE*2, FX32_ONE*3, FX32_ONE*3
};

//--------------------------------------------------------------
///	AC_WALK_*_3F�@�ړ��e�[�u��
//--------------------------------------------------------------
static const fx32 DATA_AcWalk3FMoveValueTbl[AC_WALK_3F_FRAME] =
{
	FX32_ONE*5, FX32_ONE*6, FX32_ONE*5,
};

//--------------------------------------------------------------
///	AC_WALK_*_12F�@�ړ��e�[�u��
//--------------------------------------------------------------
static const fx32 DATA_AcWalk12FMoveValueTbl[AC_WALK_12F_FRAME] =
{
  NUM_FX32(1),NUM_FX32(1),NUM_FX32(2),NUM_FX32(1),
  NUM_FX32(1),NUM_FX32(2),NUM_FX32(1),NUM_FX32(1),
  NUM_FX32(2),NUM_FX32(1),NUM_FX32(1),NUM_FX32(2),
};
