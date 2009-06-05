//======================================================================
/**
 * @file	fieldobj_move_0.c
 * @brief	�t�B�[���h���샂�f���@��{����n����0
 * @author	kagaya
 * @data	05.07.21
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procmove.h"

//======================================================================
//	define
//======================================================================
#define WAIT_END (-1)										///<�E�F�C�g�I�[

//--------------------------------------------------------------
//	MV_RND�n�@�ړ��`�F�b�N��������
//--------------------------------------------------------------
enum
{
	MV_RND_MOVE_CHECK_NORMAL = 0,				///<�ʏ�`�F�b�N
	MV_RND_MOVE_CHECK_RECT,						///<��`�`�F�b�N�A��
	MV_RND_MOVE_CHECK_LIMIT_ONLY,				///<�ړ������`�F�b�N�̂�
};

//--------------------------------------------------------------
///	MV_SPIN_DIR����ԍ�
//--------------------------------------------------------------
enum
{
	SEQNO_MV_SPIN_DIR_CMD_SET = 0,
	SEQNO_MV_SPIN_DIR_CMD_ACT,
	SEQNO_MV_SPIN_DIR_WAIT,
	SEQNO_MV_SPIN_DIR_NEXT_DIR_SET,
};

#define MV_SPIN_WAIT_FRAME (24)	//AGB 48 frame

//--------------------------------------------------------------
///	MV_REWAR����ԍ�
//--------------------------------------------------------------
enum
{
	SEQNO_MV_REWAR_DIR_CMD_SET = 0,
	SEQNO_MV_REWAR_DIR_CMD_ACT,
	SEQNO_MV_REWAR_DIR_WAIT,
	SEQNO_MV_REWAR_DIR_NEXT_DIR_SET,
};

//--------------------------------------------------------------
///	MV_RT2����ԍ�
//--------------------------------------------------------------
enum
{
	SEQNO_MV_RT2_DIR_CHECK = 0,
	SEQNO_MV_RT2_MOVE_SET,
	SEQNO_MV_RT2_MOVE,
};

//--------------------------------------------------------------
///	MV_RT3����ԍ�
//--------------------------------------------------------------
enum
{
	SEQNO_MV_RT3_MOVE_DIR_SET = 0,
	SEQNO_MV_RT3_MOVE,
};

#define MV_RT3_TURN_END_NO (3)	///<3�_����I���l

enum
{
	MV_RT3_CHECK_TYPE_X = 0,
	MV_RT3_CHECK_TYPE_Z,
};

//--------------------------------------------------------------
///	MV_RT4����ԍ�
//--------------------------------------------------------------
enum
{
	SEQNO_MV_RT4_JIKI_CHECK = 0,
	SEQNO_MV_RT4_DISCOVERY_JUMP,
	SEQNO_MV_RT4_MOVE_DIR_SET,
	SEQNO_MV_RT4_MOVE,
};

//--------------------------------------------------------------
///	�T�u����@�N���N���ړ��@��]����
//--------------------------------------------------------------
enum
{
	RT_KURU2_L = 0,
	RT_KURU2_R,
	RT_KURU2_MAX,
};

#define RT_KURU2_DIR_MAX (DIR_MAX4)		///<�N���N���ړ��ő��]��

//--------------------------------------------------------------
///	�����e�[�u��ID
//--------------------------------------------------------------
enum
{
	DIRID_MvDirRndDirTbl = 0,
	DIRID_MvDirRndDirTbl_UL,
	DIRID_MvDirRndDirTbl_UR,
	DIRID_MvDirRndDirTbl_DL,
	DIRID_MvDirRndDirTbl_DR,
	DIRID_MvDirRndDirTbl_UDL,
	DIRID_MvDirRndDirTbl_UDR,
	DIRID_MvDirRndDirTbl_ULR,
	DIRID_MvDirRndDirTbl_DLR,
	DIRID_MvDirRndDirTbl_UD,
	DIRID_MvDirRndDirTbl_LR,
	DIRID_MV_RND_DirTbl,
	DIRID_MV_RND_V_DirTbl,
	DIRID_MV_RND_H_DirTbl,
	DIRID_MvRtURLD_DirTbl,
	DIRID_MvRtRLDU_DirTbl,
	DIRID_MvRtDURL_DirTbl,
	DIRID_MvRtLDUR_DirTbl,
	DIRID_MvRtULRD_DirTbl,
	DIRID_MvRtLRDU_DirTbl,
	DIRID_MvRtDULR_DirTbl,
	DIRID_MvRtRDUL_DirTbl,
	DIRID_MvRtLUDR_DirTbl,
	DIRID_MvRtUDRL_DirTbl,
	DIRID_MvRtRLUD_DirTbl,
	DIRID_MvRtDRLU_DirTbl,
	DIRID_MvRtRUDL_DirTbl,
	DIRID_MvRtUDLR_DirTbl,
	DIRID_MvRtLRUD_DirTbl,
	DIRID_MvRtDLRU_DirTbl,
	DIRID_MvRtUL_DirTbl,
	DIRID_MvRtDR_DirTbl,
	DIRID_MvRtLD_DirTbl,
	DIRID_MvRtRU_DirTbl,
	DIRID_MvRtUR_DirTbl,
	DIRID_MvRtDL_DirTbl,
	DIRID_MvRtLU_DirTbl,
	DIRID_MvRtRD_DirTbl,
	DIRID_MvDirSpin_DirTbl,
	DIRID_END,
	DIRID_MAX,
};

//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
//#ifdef PM_DEBUG
//#define DEBUG_SEARCH	//�L���ŕ����؂�ւ��^����S�Ď��@�T�[�`
//#endif

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
//	DIR_TBL�\����
//--------------------------------------------------------------
typedef struct
{
	int id;
	const int *tbl;
}DIR_TBL;

//--------------------------------------------------------------
///	RECT�\����
//--------------------------------------------------------------
typedef struct
{
	int left;
	int top;
	int right;
	int bottom;
}RECT;

//--------------------------------------------------------------
///	RT_KURUKURU_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	s8 origin_dir;
	s8 spin_type;
	u8 dir_pause;
	u8 dmy;
}RT_KURUKURU_WORK;

#define RT_KURUKURU_WORK_SIZE (sizeof(RT_KURUKURU_WORK))

//--------------------------------------------------------------
///	MV_DIR_RND_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u16 seq_no;												///<�V�[�P���X�i���o�[
	s16 wait;												///<�\���E�F�C�g
	int tbl_id;
}MV_DIR_RND_WORK;

#define MV_DIR_RND_WORK_SIZE (sizeof(MV_DIR_RND_WORK))		///<MV_DIR_RND_WORK�T�C�Y

//--------------------------------------------------------------
///	MV_RND_WORK �\����
//--------------------------------------------------------------
typedef struct
{
	s16 seq_no;
	s16 wait;
	int move_check_type;
	int acmd_code;
	int tbl_id;
}MV_RND_WORK;

#define MV_RND_WORK_SIZE (sizeof(MV_RND_WORK))

//--------------------------------------------------------------
///	MV_DIR_WORK �\����
//--------------------------------------------------------------
typedef struct
{
	int dir;
	int seq_no;
}MV_DIR_WORK;

#define MV_DIR_WORK_SIZE (sizeof(MV_DIR_WORK))

//--------------------------------------------------------------
///	MV_SPIN_DIR_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	s8 spin_dir;
	s8 set_dir;
	s8 seq_no;
	s8 tbl_no;
	int wait;
}MV_SPIN_DIR_WORK;

#define MV_SPIN_DIR_WORK_SIZE (sizeof(MV_SPIN_DIR_WORK))

//--------------------------------------------------------------
///	MV_RT2_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	s16 seq_no;
	s16 turn_flag;
	RT_KURUKURU_WORK kurukuru;
}MV_RT2_WORK;

#define MV_RT2_WORK_SIZE (sizeof(MV_RT2_WORK))

//--------------------------------------------------------------
///	MV_RT3_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;
	u8 turn_no;
	u8 turn_check_no;
	u8 turn_check_type;
	int tbl_id;
	RT_KURUKURU_WORK kurukuru;
}MV_RT3_WORK;

#define MV_RT3_WORK_SIZE (sizeof(MV_RT3_WORK))

//--------------------------------------------------------------
///	MV_RT4_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;
	s8 turn_no;
	u8 turn_check_no;
	u8 turn_check_type;
	u8 tbl_id;
	u8 rev_flag;
	u8 dmy0;
	u8 dmy1;
	RT_KURUKURU_WORK kurukuru;
}MV_RT4_WORK;

#define MV_RT4_WORK_SIZE (sizeof(MV_RT4_WORK))

//======================================================================
//	proto
//======================================================================
static void DirRndWorkInit( FLDMMDL * fmmdl, int id );

static void MvRndWorkInit( FLDMMDL * fmmdl, int ac, int id, int check );

static int TblNumGet( const int *tbl, int end );
static int TblRndGet( const int *tbl, int end );
static int TblIDRndGet( int id, int end );
static const int * MoveDirTblIDSearch( int id );
static int TrJikiDashSearch( FLDMMDL * fmmdl );
static int TrJikiDashSearchTbl( FLDMMDL * fmmdl, int id, int end );

static void MvRndRectMake( FLDMMDL * fmmdl, RECT *rect );
static int MvRndRectMoveLimitCheck( FLDMMDL * fmmdl, int dir );

static void MvDirWorkInit( FLDMMDL * fmmdl, int dir );

static void MvSpinDirWorkInit( FLDMMDL * fmmdl, int dir );
int (* const DATA_MvSpinMoveTbl[])( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work );

int (* const DATA_MvRewarMoveTbl[])( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work );

int (* const DATA_MvRt2MoveTbl[])( FLDMMDL * fmmdl, MV_RT2_WORK *work );

static void MvRt3WorkInit( FLDMMDL * fmmdl, int check_no, int check_type, int id );
int (* const DATA_MvRt3MoveTbl[])( FLDMMDL * fmmdl, MV_RT3_WORK *work );

static void MvRt4WorkInit( FLDMMDL * fmmdl, int check_no, int check_type, int tbl_id );
int (* const DATA_MvRt4MoveTbl[])( FLDMMDL * fmmdl, MV_RT4_WORK *work );

static const int DATA_KuruKuruTbl[RT_KURU2_MAX][RT_KURU2_DIR_MAX];
static int MoveSub_KuruKuruCheck( FLDMMDL * fmmdl );
static void MoveSub_KuruKuruInit( FLDMMDL * fmmdl, RT_KURUKURU_WORK *work );
static void MoveSub_KuruKuruSet( FLDMMDL * fmmdl, RT_KURUKURU_WORK *work );
static void MoveSub_KuruKuruEnd( FLDMMDL * fmmdl, RT_KURUKURU_WORK *work );

//data
const int DATA_MvDirRndWaitTbl[];
const int DATA_MvDirRndDirTbl[];
const int DATA_MvDirRndDirTbl_UL[];
const int DATA_MvDirRndDirTbl_UR[];
const int DATA_MvDirRndDirTbl_DL[];
const int DATA_MvDirRndDirTbl_DR[];
const int DATA_MvDirRndDirTbl_UDL[];
const int DATA_MvDirRndDirTbl_UDR[];
const int DATA_MvDirRndDirTbl_ULR[];
const int DATA_MvDirRndDirTbl_DLR[];
const int DATA_MvDirRndDirTbl_UD[];
const int DATA_MvDirRndDirTbl_LR[];

const int DATA_MV_RND_DirTbl[];
const int DATA_MV_RND_V_DirTbl[];
const int DATA_MV_RND_H_DirTbl[];

const int DATA_MvRtURLD_DirTbl[];
const int DATA_MvRtRLDU_DirTbl[];
const int DATA_MvRtDURL_DirTbl[];
const int DATA_MvRtLDUR_DirTbl[];
const int DATA_MvRtULRD_DirTbl[];
const int DATA_MvRtLRDU_DirTbl[];
const int DATA_MvRtDULR_DirTbl[];
const int DATA_MvRtRDUL_DirTbl[];
const int DATA_MvRtLUDR_DirTbl[];
const int DATA_MvRtUDRL_DirTbl[];
const int DATA_MvRtRLUD_DirTbl[];
const int DATA_MvRtDRLU_DirTbl[];
const int DATA_MvRtRUDL_DirTbl[];
const int DATA_MvRtUDLR_DirTbl[];
const int DATA_MvRtLRUD_DirTbl[];
const int DATA_MvRtDLRU_DirTbl[];

const int DATA_MvRtUL_DirTbl[];
const int DATA_MvRtDR_DirTbl[];
const int DATA_MvRtLD_DirTbl[];
const int DATA_MvRtRU_DirTbl[];
const int DATA_MvRtUR_DirTbl[];
const int DATA_MvRtDL_DirTbl[];
const int DATA_MvRtLU_DirTbl[];
const int DATA_MvRtRD_DirTbl[];

static const DIR_TBL DATA_MoveDirTbl[DIRID_MAX];

const int JikiDashSensorMoveCodeTbl[];

//======================================================================
//	�f�o�b�O
//======================================================================

//======================================================================
//	MV_DIR_RND	�����_���ɕ����؂�ւ�
//======================================================================
//--------------------------------------------------------------
/**
 * MV_DIR_RND_WORK������
 * @param	fmmdl	FLDMMDL *
 * @param	id		�����e�[�u��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DirRndWorkInit( FLDMMDL * fmmdl, int id )
{
	MV_DIR_RND_WORK *work;
	
	work = FLDMMDL_InitMoveProcWork( fmmdl, MV_DIR_RND_WORK_SIZE );	//���샏�[�N������
	work->wait = TblRndGet( DATA_MvDirRndWaitTbl, WAIT_END );			//�҂����ԃZ�b�g
	work->tbl_id = id;
	
	FLDMMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );					//�`��X�e�[�^�X�@��~
	FLDMMDL_OffStatusBitMove( fmmdl );								//��ɒ�~��
}

//--------------------------------------------------------------
/**
 * MV_DIR_RND ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRnd_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RND_UL ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndUL_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UL );
}

//--------------------------------------------------------------
/**
 * MV_RND_UR ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndUR_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UR );
}

//--------------------------------------------------------------
/**
 * MV_RND_DL ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndDL_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_DL );
}

//--------------------------------------------------------------
/**
 * MV_RND_DR ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndDR_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_DR );
}

//--------------------------------------------------------------
/**
 * MV_RND_UDL ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndUDL_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UDL );
}

//--------------------------------------------------------------
/**
 * MV_RND_UDR ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndUDR_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UDR );
}

//--------------------------------------------------------------
/**
 * MV_RND_ULR ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndULR_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_ULR );
}

//--------------------------------------------------------------
/**
 * MV_RND_DLR ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndDLR_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_DLR );
}

//--------------------------------------------------------------
/**
 * MV_RND_UD ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndUD_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_UD );
}

//--------------------------------------------------------------
/**
 * MV_RND_LR ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRndLR_Init( FLDMMDL * fmmdl )
{
	DirRndWorkInit( fmmdl, DIRID_MvDirRndDirTbl_LR );
}

//--------------------------------------------------------------
/**
 * MV_DIR_RND ����
 * @param	fmmdl		FLDMMDL * 
 * @retval	int			TRUE=����������
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRnd_Move( FLDMMDL * fmmdl )
{
	MV_DIR_RND_WORK *work = FLDMMDL_GetMoveProcWork( fmmdl );
	int dir = TrJikiDashSearchTbl( fmmdl, work->tbl_id, DIR_NOT );
	
	if( dir != DIR_NOT ){
		FLDMMDL_SetDirDisp( fmmdl, dir );
	}else{
		switch( work->seq_no ){
		case 0:
			work->wait--;
		
			if( work->wait <= 0 ){									//�҂����� 0
				work->wait = TblRndGet( DATA_MvDirRndWaitTbl, WAIT_END );
				FLDMMDL_SetDirDisp( fmmdl, TblIDRndGet(work->tbl_id,DIR_NOT) );
			}
		}
	}
	
	FLDMMDL_UpdateGridPosCurrent( fmmdl );
}

//--------------------------------------------------------------
/**
 * MV_DIR_RND �폜�֐�
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDirRnd_Delete( FLDMMDL * fmmdl )
{
}

//======================================================================
//	MV_RND�n	�����_���Ɉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * MV_RND ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRnd_Init( FLDMMDL * fmmdl )
{
	MvRndWorkInit( fmmdl, AC_WALK_U_8F, DIRID_MV_RND_DirTbl, MV_RND_MOVE_CHECK_NORMAL );
}

//--------------------------------------------------------------
/**
 * MV_RND_V ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRndV_Init( FLDMMDL * fmmdl )
{
	MvRndWorkInit( fmmdl, AC_WALK_U_8F, DIRID_MV_RND_V_DirTbl, MV_RND_MOVE_CHECK_NORMAL );
}

//--------------------------------------------------------------
/**
 * MV_RND_H ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRndH_Init( FLDMMDL * fmmdl )
{
	MvRndWorkInit( fmmdl, AC_WALK_U_8F, DIRID_MV_RND_H_DirTbl, MV_RND_MOVE_CHECK_NORMAL );
}

//--------------------------------------------------------------
/**
 * MV_RND_H_LIM ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRndHLim_Init( FLDMMDL * fmmdl )
{
	MvRndWorkInit( fmmdl, AC_WALK_U_8F, DIRID_MV_RND_H_DirTbl, MV_RND_MOVE_CHECK_LIMIT_ONLY );
}

//--------------------------------------------------------------
/**
 * MV_RND_UL�n ������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRndRect_Init( FLDMMDL * fmmdl )
{
	MvRndWorkInit( fmmdl, AC_WALK_U_8F, DIRID_MV_RND_DirTbl, MV_RND_MOVE_CHECK_RECT );
}

//======================================================================
//	MV_RND�n�@�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * MV_RND_WORK������
 * @param	fmmdl		FLDMMDL *
 * @param	ac			�ړ�����ۂɎg�p����A�j���[�V�����R�}���h�B
 * FLDMMDL_ChangeDirAcmdCode()�ŕϊ��ΏۂƂȂ�BAC_WALK_U_32F��
 * @param	tbl_id		�����e�[�u��ID
 * @param	check		MV_RND_MOVE_CHECK_NORMAL��
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MvRndWorkInit( FLDMMDL * fmmdl, int ac, int tbl_id, int check )
{
	MV_RND_WORK *work;
	
	work = FLDMMDL_InitMoveProcWork( fmmdl, MV_RND_WORK_SIZE );
	work->move_check_type = check;
	work->acmd_code = ac;
	work->tbl_id = tbl_id;
	
	FLDMMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );
	FLDMMDL_OffStatusBitMove( fmmdl );
}

//--------------------------------------------------------------
/**
 * MV_RND�n�@����
 * @param	fmmdl		FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MvRnd_Move( FLDMMDL * fmmdl )
{
	int ret;
	MV_RND_WORK *work;
	
	work = FLDMMDL_GetMoveProcWork( fmmdl );
	
	switch( work->seq_no ){
	case 0:
		FLDMMDL_OffStatusBitMove( fmmdl );
		FLDMMDL_OffStatusBitMoveEnd( fmmdl );
		
		ret = FLDMMDL_GetDirDisp( fmmdl );
		ret = FLDMMDL_ChangeDirAcmdCode( ret, AC_DIR_U );
		FLDMMDL_SetLocalAcmd( fmmdl, ret );
		
		work->seq_no++;
		break;
	case 1:
		if( FLDMMDL_ActionLocalAcmd(fmmdl) == FALSE ){
			break;
		}
		
		work->wait = TblRndGet( DATA_MvDirRndWaitTbl, WAIT_END );
		work->seq_no++;
	case 2:
		work->wait--;
		
		if( work->wait ){
			break;
		}
		
		work->seq_no++;
	case 3:
		ret = TblIDRndGet( work->tbl_id, DIR_NOT );
		FLDMMDL_SetDirAll( fmmdl, ret );
		
		if( work->move_check_type == MV_RND_MOVE_CHECK_RECT ){
			if( MvRndRectMoveLimitCheck(fmmdl,ret) == FALSE ){
				work->seq_no = 0;
				break;
			}
		}
		
		{
			u32 hit = FLDMMDL_HitCheckMoveDir( fmmdl, ret );
			
			if( hit != FLDMMDL_MOVEHITBIT_NON ){
				if( work->move_check_type == MV_RND_MOVE_CHECK_LIMIT_ONLY ){
					if( (hit&FLDMMDL_MOVEHITBIT_LIM) ){ //�ړ������̂݌���
						work->seq_no = 0;
						break;
					}
				}else{
					work->seq_no = 0;
					break;
				}
			}
		}
		
		ret = FLDMMDL_ChangeDirAcmdCode( ret, work->acmd_code );
		FLDMMDL_SetLocalAcmd( fmmdl, ret );
			
		FLDMMDL_OnStatusBitMove( fmmdl );
		work->seq_no++;
	case 4:
		if( FLDMMDL_ActionLocalAcmd(fmmdl) == FALSE ){
			break;
		}
		
		FLDMMDL_OffStatusBitMove( fmmdl );
		work->seq_no = 0;
	}
}

//======================================================================
//	MV_RND_UL���p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��\�͈͋�`�쐬
 * @param	fmmdl	FLDMMDL *
 * @param	rect	��`�i�[��
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MvRndRectMake( FLDMMDL * fmmdl, RECT *rect )
{
	int code,ix,iz,lx,lz;
	
	ix = FLDMMDL_GetInitGridPosX( fmmdl );
	iz = FLDMMDL_GetInitGridPosZ( fmmdl );
	lx = FLDMMDL_GetMoveLimitX( fmmdl );
	lz = FLDMMDL_GetMoveLimitZ( fmmdl );
	
	code = FLDMMDL_GetMoveCode( fmmdl );
	
	switch( code ){
	case MV_RND_UL:
		rect->left = ix - lx; rect->right = ix;
		rect->top = iz - lz; rect->bottom = iz;
		break;
	case MV_RND_UR:
		rect->left = ix; rect->right = ix + lx;
		rect->top = iz - lz; rect->bottom = iz;
		break;
	case MV_RND_DL:
		rect->left = ix - lx; rect->right = ix;
		rect->top = iz; rect->bottom = iz + lz;
		break;
	case MV_RND_DR:
		rect->left = ix; rect->right = ix + lx;
		rect->top = iz; rect->bottom = iz + lz;
		break;
	case MV_RND_UDL:
		rect->left = ix - lx; rect->right = ix;
		rect->top = iz - lz; rect->bottom = iz + lz;
		break;
	case MV_RND_UDR:
		rect->left = ix; rect->right = ix + lx;
		rect->top = iz - lz; rect->bottom = iz + lz;
		break;
	case MV_RND_ULR:
		rect->left = ix - lx; rect->right = ix + lx;
		rect->top = iz - lz; rect->bottom = iz;
		break;
	case MV_RND_DLR:
		rect->left = ix - lx; rect->right = ix + lx;
		rect->top = iz; rect->bottom = iz + lz;
		break;
	default:
		GF_ASSERT( 0 && "MvRndRectMake()���Ή�����R�[" );
	}
}

//--------------------------------------------------------------
/**
 * �ړ��\�͈̓`�F�b�N�@�����w��
 * @param	fmmdl	FLDMMDL *
 * @param	dir		�ړ�����
 * @retval	int		TRUE=�\
 */
//--------------------------------------------------------------
static int MvRndRectMoveLimitCheck( FLDMMDL * fmmdl, int dir )
{
	int gx,gz;
	RECT rect;
	
	MvRndRectMake( fmmdl, &rect );
	gx = FLDMMDL_GetGridPosX( fmmdl ) + FLDMMDL_TOOL_GetDirAddValueGridX( dir );
	gz = FLDMMDL_GetGridPosZ( fmmdl ) + FLDMMDL_TOOL_GetDirAddValueGridZ( dir );
	
	if( rect.left > gx || rect.right < gx ){
		return( FALSE );
	}
	
	if( rect.top > gz || rect.bottom < gz ){
		return( FALSE );
	}
	
	return( TRUE );
}

//======================================================================
//	MV_UP�n�@��Ɏw�����������
//======================================================================
//--------------------------------------------------------------
/**
 * MV_UP�n�@������
 * @param	fmmdl	FLDMMDL *
 * @param	dir		��������
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MvDirWorkInit( FLDMMDL * fmmdl, int dir )
{
	MV_DIR_WORK *work;
	
	work = FLDMMDL_InitMoveProcWork( fmmdl, MV_DIR_WORK_SIZE );
	work->dir = dir;
	
	FLDMMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );
	FLDMMDL_OffStatusBitMove( fmmdl );
	FLDMMDL_UpdateGridPosCurrent( fmmdl );
}

//--------------------------------------------------------------
/**
 * MV_UP�n�@����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDir_Move( FLDMMDL * fmmdl )
{
	MV_DIR_WORK *work;
	
	work = FLDMMDL_GetMoveProcWork( fmmdl );
	
	switch( work->seq_no ){
	case 0:
		FLDMMDL_SetDirDisp( fmmdl, work->dir );
		work->seq_no++;
		break;
	case 1:
		break;
	}
}

//--------------------------------------------------------------
/**
 * MV_UP ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveUp_Init( FLDMMDL * fmmdl )
{
	MvDirWorkInit( fmmdl, DIR_UP );
}

//--------------------------------------------------------------
/**
 * MV_DOWN ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDown_Init( FLDMMDL * fmmdl )
{
	MvDirWorkInit( fmmdl, DIR_DOWN );
}

//--------------------------------------------------------------
/**
 * MV_LEFT ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveLeft_Init( FLDMMDL * fmmdl )
{
	MvDirWorkInit( fmmdl, DIR_LEFT );
}

//--------------------------------------------------------------
/**
 * MV_RIGHT ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRight_Init( FLDMMDL * fmmdl )
{
	MvDirWorkInit( fmmdl, DIR_RIGHT );
}

//======================================================================
//	MV_SPIN�n�@������]
//======================================================================
//--------------------------------------------------------------
/**
 * MV_SPIN_DIR_WORK������
 * @param	fmmdl	FLDMMDL *
 * @param	dir		DIR_LEFT=������ADIR_RIGHT=�E����
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MvSpinDirWorkInit( FLDMMDL * fmmdl, int dir )
{
	MV_SPIN_DIR_WORK *work;
	
	work = FLDMMDL_InitMoveProcWork( fmmdl, MV_SPIN_DIR_WORK_SIZE );
	work->spin_dir = dir;
	
	FLDMMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );
	FLDMMDL_OffStatusBitMove( fmmdl );
	FLDMMDL_UpdateGridPosCurrent( fmmdl );
}

//--------------------------------------------------------------
/**
 * MV_SPIN_L�@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveSpinLeft_Init( FLDMMDL * fmmdl )
{
	MvSpinDirWorkInit( fmmdl, DIR_LEFT );
}

//--------------------------------------------------------------
/**
 * MV_SPIN_R�@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveSpinRight_Init( FLDMMDL * fmmdl )
{
	MvSpinDirWorkInit( fmmdl, DIR_RIGHT );
}

//--------------------------------------------------------------
/**
 * MV_SPIN�n�@����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveSpin_Move( FLDMMDL * fmmdl )
{
	MV_SPIN_DIR_WORK *work;
	
	work = FLDMMDL_GetMoveProcWork( fmmdl );
	while( DATA_MvSpinMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_SPIN�n�@����@���ݕ����R�}���h�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_SPIN_DIR_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvSpinMove_DirCmdSet( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work )
{
	int ret = TrJikiDashSearchTbl( fmmdl, DIRID_MvDirSpin_DirTbl, DIR_NOT );
	
	if( ret == DIR_NOT ){
		ret = FLDMMDL_GetDirDisp( fmmdl );
	}
	
	ret = FLDMMDL_ChangeDirAcmdCode( ret, AC_DIR_U );
	FLDMMDL_SetLocalAcmd( fmmdl, ret );
	
	work->seq_no = SEQNO_MV_SPIN_DIR_CMD_ACT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_SPIN�n�@����@�R�}���h�A�N�V����
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_SPIN_DIR_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvSpinMove_CmdAction( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work )
{
	if( FLDMMDL_ActionLocalAcmd(fmmdl) == FALSE ){
		return( FALSE );
	}
	
	work->wait = 0;
	work->seq_no = SEQNO_MV_SPIN_DIR_WAIT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_SPIN�n�@����@�E�F�C�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_SPIN_DIR_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvSpinMove_Wait( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work )
{
	if( work->wait ){
		if( TrJikiDashSearchTbl(fmmdl,DIRID_MvDirSpin_DirTbl,DIR_NOT) != DIR_NOT ){
			work->seq_no = SEQNO_MV_SPIN_DIR_CMD_SET;
			return( TRUE );
		}
	}
	
	work->wait++;
	
	if( work->wait < MV_SPIN_WAIT_FRAME ){
		return( FALSE );
	}
	
	work->seq_no = SEQNO_MV_SPIN_DIR_NEXT_DIR_SET;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_SPIN�n�@����@�������Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_SPIN_DIR_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvSpinMove_NextDirSet( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work )
{
	int i,ret,*tbl;
	int left[5] = { DIR_UP, DIR_LEFT, DIR_DOWN, DIR_RIGHT, DIR_NOT };
	int right[5] = { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT, DIR_NOT };
	
	if( work->spin_dir == DIR_LEFT ){
		tbl = left;
	}else{
		tbl = right;
	}
	
	ret = FLDMMDL_GetDirDisp( fmmdl );
	
	for( i = 0; tbl[i] != DIR_NOT; i++ ){
		if( ret == tbl[i] ){
			break;
		}
	}
	
	GF_ASSERT( tbl[i] != DIR_NOT && "FieldOBJ MV_SPIN���������G���[" );
	
	i++;
	
	if( tbl[i] == DIR_NOT ){
		i = 0;
	}
	
	ret = tbl[i];
	
	FLDMMDL_SetDirDisp( fmmdl, ret );
	
	work->seq_no = SEQNO_MV_SPIN_DIR_CMD_SET;
	return( TRUE );
}

//--------------------------------------------------------------
///	MV_SPIN����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_MvSpinMoveTbl[])( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work ) =
{
	MvSpinMove_DirCmdSet,
	MvSpinMove_CmdAction,
	MvSpinMove_Wait,
	MvSpinMove_NextDirSet,
};

//======================================================================
//	MV_REWAR�@�E��]<->����]
//======================================================================
//--------------------------------------------------------------
/**
 * MV_REWAR ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRewar_Init( FLDMMDL * fmmdl )
{
	MvSpinDirWorkInit( fmmdl, DIR_RIGHT );
}

//--------------------------------------------------------------
/**
 * MV_REWAR�n�@����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRewar_Move( FLDMMDL * fmmdl )
{
	MV_SPIN_DIR_WORK *work;
	
	work = FLDMMDL_GetMoveProcWork( fmmdl );
	while( DATA_MvRewarMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_REWAR�@����@���ݕ����R�}���h�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_SPIN_DIR_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRewarMove_DirCmdSet( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work )
{
	int ret = FLDMMDL_GetDirDisp( fmmdl );
	ret = FLDMMDL_ChangeDirAcmdCode( ret, AC_DIR_U );
	FLDMMDL_SetLocalAcmd( fmmdl, ret );
	
	work->seq_no = SEQNO_MV_REWAR_DIR_CMD_ACT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_REWAR�@����@�R�}���h�A�N�V����
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_SPIN_DIR_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRewarMove_CmdAction( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work )
{
	if( FLDMMDL_ActionLocalAcmd(fmmdl) == FALSE ){
		return( FALSE );
	}
	
	work->wait = 0;
	work->seq_no = SEQNO_MV_REWAR_DIR_WAIT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_REWAR�n�@����@�E�F�C�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_SPIN_DIR_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRewarMove_Wait( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work )
{
	work->wait++;
	
	if( work->wait < MV_SPIN_WAIT_FRAME ){
		return( FALSE );
	}
	
	work->seq_no = SEQNO_MV_REWAR_DIR_NEXT_DIR_SET;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_REWAR�@����@�������Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_SPIN_DIR_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRewarMove_NextDirSet( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work )
{
	int i,ret,*tbl;
	int left[5] = { DIR_UP, DIR_LEFT, DIR_DOWN, DIR_RIGHT, DIR_NOT };
	int right[5] = { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT, DIR_NOT };
	
	if( work->spin_dir == DIR_LEFT ){
		tbl = left;
	}else{
		tbl = right;
	}
	
	ret = FLDMMDL_GetDirDisp( fmmdl );
	
	for( i = 0; tbl[i] != DIR_NOT; i++ ){
		if( ret == tbl[i] ){
			break;
		}
	}
	
	GF_ASSERT( tbl[i] != DIR_NOT && "FieldOBJ MV_REWAR���������G���[" );
	
	i++;
	
	if( tbl[i] == DIR_NOT ){
		i = 0;
	}
	
	ret = tbl[i];
	
	FLDMMDL_SetDirDisp( fmmdl, ret );
	
	{
		int dir = FLDMMDL_GetDirDisp( fmmdl );
		int sdir = FLDMMDL_GetDirHeader( fmmdl );
		
		if( dir == sdir ){
			work->spin_dir = FLDMMDL_TOOL_FlipDir( work->spin_dir );
		}
	}
	
	work->seq_no = SEQNO_MV_REWAR_DIR_CMD_SET;
	return( TRUE );
}

//--------------------------------------------------------------
///	MV_SPIN����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_MvRewarMoveTbl[])( FLDMMDL * fmmdl, MV_SPIN_DIR_WORK *work ) =
{
	MvRewarMove_DirCmdSet,
	MvRewarMove_CmdAction,
	MvRewarMove_Wait,
	MvRewarMove_NextDirSet,
};

//======================================================================
//	MV_RT2�@�����������	
//======================================================================
//--------------------------------------------------------------
/**
 * MV_RT2 ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRoute2_Init( FLDMMDL * fmmdl )
{
	MV_RT2_WORK *work;
	
	work = FLDMMDL_InitMoveProcWork( fmmdl, MV_RT2_WORK_SIZE );
	
	if( MoveSub_KuruKuruCheck(fmmdl) == TRUE ){
		MoveSub_KuruKuruInit( fmmdl, &work->kurukuru );
	}
}

//--------------------------------------------------------------
/**
 * MV_RT2 ����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRoute2_Move( FLDMMDL * fmmdl )
{
	MV_RT2_WORK *work = FLDMMDL_GetMoveProcWork( fmmdl );
	
	while( DATA_MvRt2MoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_RT2 ����@�����`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_RT2_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRt2_DirCheck( FLDMMDL * fmmdl, MV_RT2_WORK *work )
{
	int dir;
	
	dir = FLDMMDL_GetDirHeader( fmmdl );						//�w�b�_�w�����
	
	if( work->turn_flag == TRUE ){
		dir = FLDMMDL_TOOL_FlipDir( dir );
	}
	
	FLDMMDL_SetDirMove( fmmdl, dir );
	
	if( MoveSub_KuruKuruCheck(fmmdl) == FALSE ){
		FLDMMDL_SetDirDisp( fmmdl, dir );
	}
	
	work->seq_no = SEQNO_MV_RT2_MOVE_SET;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_RT2 ����@����Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_RT2_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRt2_MoveSet( FLDMMDL * fmmdl, MV_RT2_WORK *work )
{
	if( work->turn_flag ){
		int ix,iz,gx,gz;
		
		ix = FLDMMDL_GetInitGridPosX( fmmdl );
		iz = FLDMMDL_GetInitGridPosZ( fmmdl );
		gx = FLDMMDL_GetGridPosX( fmmdl );
		gz = FLDMMDL_GetGridPosZ( fmmdl );
		
		if( ix == gx && iz == gz ){
			int dir = FLDMMDL_TOOL_FlipDir( FLDMMDL_GetDirMove(fmmdl) );
			
			FLDMMDL_SetDirMove( fmmdl, dir );
			
			if( MoveSub_KuruKuruCheck(fmmdl) == FALSE ){
				FLDMMDL_SetDirDisp( fmmdl, dir );
			}
			
			work->turn_flag = FALSE;
		}
	}
	
	{
		int dir,ac;
		u32 ret;
		
		dir = FLDMMDL_GetDirMove( fmmdl );
		ret = FLDMMDL_HitCheckMoveDir( fmmdl, dir );
		
		if( (ret & FLDMMDL_MOVEHITBIT_LIM) ){
			work->turn_flag = TRUE;
			dir = FLDMMDL_TOOL_FlipDir( dir );
			ret = FLDMMDL_HitCheckMoveDir( fmmdl, dir );
		}
		
		ac = AC_WALK_U_8F;
		
		if( ret != FLDMMDL_MOVEHITBIT_NON ){
			ac = AC_STAY_WALK_U_8F;
		}
		
		ac = FLDMMDL_ChangeDirAcmdCode( dir, ac );
		FLDMMDL_SetLocalAcmd( fmmdl, ac );
		
		if( MoveSub_KuruKuruCheck(fmmdl) == TRUE ){
			MoveSub_KuruKuruSet( fmmdl, &work->kurukuru );
		}
	}
	
	FLDMMDL_OnStatusBitMove( fmmdl );
	work->seq_no = SEQNO_MV_RT2_MOVE;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_RT2 ����@���쒆
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_RT2_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRt2_Move( FLDMMDL * fmmdl, MV_RT2_WORK *work )
{
	if( FLDMMDL_ActionLocalAcmd(fmmdl) == TRUE ){
		FLDMMDL_OffStatusBitMove( fmmdl );
		
		if( MoveSub_KuruKuruCheck(fmmdl) == TRUE ){
			MoveSub_KuruKuruEnd( fmmdl, &work->kurukuru );
		}
		
		work->seq_no = SEQNO_MV_RT2_DIR_CHECK;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
///	MV_RT2����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_MvRt2MoveTbl[])( FLDMMDL * fmmdl, MV_RT2_WORK *work ) =
{
	MvRt2_DirCheck,
	MvRt2_MoveSet,
	MvRt2_Move,
};

//======================================================================
//	MV_RTURLD���@3�ӏ�����
//======================================================================
//--------------------------------------------------------------
/**
 * MV_RT3_WORK������
 * @param	fmmdl		FLDMMDL *
 * @param	check_no	3�_�ړ��œ��蕪��Ƃ���_
 * @param	check_type	MV_RT3_CHECK_TYPE_X��
 * @param	tbl_id		�e�[�u��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MvRt3WorkInit( FLDMMDL * fmmdl, int check_no, int check_type, int tbl_id )
{
	MV_RT3_WORK *work;
	
	work = FLDMMDL_InitMoveProcWork( fmmdl, MV_RT3_WORK_SIZE );
	work->turn_check_no = check_no;
	work->turn_check_type = check_type;
	work->tbl_id = tbl_id;
	
	if( MoveSub_KuruKuruCheck(fmmdl) == TRUE ){
		MoveSub_KuruKuruInit( fmmdl, &work->kurukuru );
	}
}

//--------------------------------------------------------------
/**
 * MV_RTURLD ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteURLD_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtURLD_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTRLDU ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteRLDU_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtRLDU_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTDURL������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteDURL_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtDURL_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTLDUR������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteLDUR_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtLDUR_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTULRD������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteULRD_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtULRD_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTLRDU������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteLRDU_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtLRDU_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTDULR������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteDULR_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtDULR_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTRDUL������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteRDUL_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtRDUL_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTLUDR������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteLUDR_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtLUDR_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTUDRL������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteUDRL_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtUDRL_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTRLUD������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteRLUD_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtRLUD_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTDRLU������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteDRLU_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtDRLU_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTRUDL������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteRUDL_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtRUDL_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTUDLR������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteUDLR_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtUDLR_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTLRUD������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteLRUD_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtLRUD_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTDLRU������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteDLRU_Init( FLDMMDL * fmmdl )
{
	MvRt3WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtDLRU_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RT3����@
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRoute3_Move( FLDMMDL * fmmdl )
{
	MV_RT3_WORK *work;
	
	work = FLDMMDL_GetMoveProcWork( fmmdl );
	while( DATA_MvRt3MoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_RT3����@�ړ������Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_RT3_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRt3Move_MoveDirSet( FLDMMDL * fmmdl, MV_RT3_WORK *work )
{
	if( work->turn_no == work->turn_check_no  ){
		if( work->turn_check_type == MV_RT3_CHECK_TYPE_X ){
			int ix = FLDMMDL_GetInitGridPosX( fmmdl );
			int gx = FLDMMDL_GetGridPosX( fmmdl );
			
			if( ix == gx ){
				work->turn_no++;
			}
		}else{
			int iz = FLDMMDL_GetInitGridPosZ( fmmdl );
			int gz = FLDMMDL_GetGridPosZ( fmmdl );
			
			if( iz == gz ){
				work->turn_no++;
			}
		}
	}
	
	if( work->turn_no == MV_RT3_TURN_END_NO ){
		int ix = FLDMMDL_GetInitGridPosX( fmmdl );
		int iz = FLDMMDL_GetInitGridPosZ( fmmdl );
		int gx = FLDMMDL_GetGridPosX( fmmdl );
		int gz = FLDMMDL_GetGridPosZ( fmmdl );
		
		if( ix == gx && iz == gz ){
			work->turn_no = 0;
		}
	}
	
	{
		const int *tbl;
		int dir,ac;
		u32 ret;
		
		tbl = MoveDirTblIDSearch( work->tbl_id );
		dir = tbl[work->turn_no];
		
		FLDMMDL_SetDirMove( fmmdl, dir );
		
		if( MoveSub_KuruKuruCheck(fmmdl) == FALSE ){
			FLDMMDL_SetDirDisp( fmmdl, dir );
		}
		
		ret = FLDMMDL_HitCheckMoveDir( fmmdl, dir );
		
		if( (ret & FLDMMDL_MOVEHITBIT_LIM) ){
			work->turn_no++;
			dir = tbl[work->turn_no];
			
			FLDMMDL_SetDirMove( fmmdl, dir );
			
			if( MoveSub_KuruKuruCheck(fmmdl) == FALSE ){
				FLDMMDL_SetDirDisp( fmmdl, dir );
			}
			
			ret = FLDMMDL_HitCheckMoveDir( fmmdl, dir );
		}
		
		ac = AC_WALK_U_8F;
		
		if( (ret != FLDMMDL_MOVEHITBIT_NON) ){
			ac = AC_STAY_WALK_U_8F;
		}
		
		ac = FLDMMDL_ChangeDirAcmdCode( dir, ac );
		FLDMMDL_SetLocalAcmd( fmmdl, ac );
		
		if( MoveSub_KuruKuruCheck(fmmdl) == TRUE ){
			MoveSub_KuruKuruSet( fmmdl, &work->kurukuru );
		}
	}
	
	FLDMMDL_OnStatusBitMove( fmmdl );
	work->seq_no = SEQNO_MV_RT3_MOVE;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_RT3����@�ړ�
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_RT3_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRt3Move_MoveDir( FLDMMDL * fmmdl, MV_RT3_WORK *work )
{
	if( FLDMMDL_ActionLocalAcmd(fmmdl) == TRUE ){
		FLDMMDL_OffStatusBitMove( fmmdl );
		
		if( MoveSub_KuruKuruCheck(fmmdl) == TRUE ){
			MoveSub_KuruKuruEnd( fmmdl, &work->kurukuru );
		}
		
		work->seq_no = SEQNO_MV_RT3_MOVE_DIR_SET;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
///	MV_RT3����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_MvRt3MoveTbl[])( FLDMMDL * fmmdl, MV_RT3_WORK *work ) =
{
	MvRt3Move_MoveDirSet,
	MvRt3Move_MoveDir,
};

//======================================================================
//	MV_RTRL���@�S�_�ړ�����
//======================================================================
//--------------------------------------------------------------
/**
 * MV_RT4_WORK������
 * @param	fmmdl		FLDMMDL *
 * @param	check_no	4�_�ړ��œ��蕪��Ƃ���_
 * @param	check_type	MV_RT3_CHECK_TYPE_X��
 * @param	tbl_id		�e�[�u��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MvRt4WorkInit( FLDMMDL * fmmdl, int check_no, int check_type, int tbl_id )
{
	MV_RT4_WORK *work;
	
	work = FLDMMDL_InitMoveProcWork( fmmdl, MV_RT4_WORK_SIZE );
	work->turn_check_no = check_no;
	work->turn_check_type = check_type;
	work->tbl_id = tbl_id;
	
	if( MoveSub_KuruKuruCheck(fmmdl) == TRUE ){
		MoveSub_KuruKuruInit( fmmdl, &work->kurukuru );
	}
}

//--------------------------------------------------------------
/**
 * MV_RTUL������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteUL_Init( FLDMMDL * fmmdl )
{
	MvRt4WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtUL_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTDR������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteDR_Init( FLDMMDL * fmmdl )
{
	MvRt4WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtDR_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTLD������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteLD_Init( FLDMMDL * fmmdl )
{
	MvRt4WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtLD_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTRU������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteRU_Init( FLDMMDL * fmmdl )
{
	MvRt4WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtRU_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTUR������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteUR_Init( FLDMMDL * fmmdl )
{
	MvRt4WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtUR_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTDL������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteDL_Init( FLDMMDL * fmmdl )
{
	MvRt4WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_Z, DIRID_MvRtDL_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTLU������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteLU_Init( FLDMMDL * fmmdl )
{
	MvRt4WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtLU_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RTRD������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRouteRD_Init( FLDMMDL * fmmdl )
{
	MvRt4WorkInit( fmmdl, 2, MV_RT3_CHECK_TYPE_X, DIRID_MvRtRD_DirTbl );
}

//--------------------------------------------------------------
/**
 * MV_RT4�n����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveRoute4_Move( FLDMMDL * fmmdl )
{
	MV_RT4_WORK *work = FLDMMDL_GetMoveProcWork( fmmdl );
	while( DATA_MvRt4MoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_RT4����@�^�[���񐔑���
 * @param	work	MV_RT4_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MvRt4Move_TurnNoInc( MV_RT4_WORK *work )
{
	if( work->rev_flag == TRUE ){
		work->turn_no--;
		if( work->turn_no < 0 ){
			work->turn_no = 3;
		}
	}else{
		work->turn_no++;
	}
}

//--------------------------------------------------------------
/**
 * MV_RT4����@���@�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_RT4_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRt4Move_JikiCheck( FLDMMDL * fmmdl, MV_RT4_WORK *work )
{
#if 0
	if( FLDMMDL_GetEventType(fmmdl) == EV_TYPE_TRAINER_ESCAPE ){
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
		int eye_dir = FLDMMDL_GetDirDisp( fmmdl );
		int eye_range = FLDMMDL_GetParam( fmmdl, FLDMMDL_PARAM_0 );
		int ret = EvTrainer_EyeRangeHitCheck( fmmdl, jiki, eye_dir, eye_range );
		
		if( ret != EYE_CHECK_NOHIT ){
			int ac = FLDMMDL_ChangeDirAcmdCode( eye_dir, AC_STAY_JUMP_U_8F );
			FLDMMDL_SetLocalAcmd( fmmdl, ret );
			FLDMMDL_OnStatusBitMove( fmmdl );
			work->seq_no = SEQNO_MV_RT4_DISCOVERY_JUMP;
			return( TRUE );
		}
	}
#endif	
	work->seq_no = SEQNO_MV_RT4_MOVE_DIR_SET;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_RT4����@���@�𔭌��@��яオ�蓮��
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_RT4_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRt4Move_DiscoveryJump( FLDMMDL * fmmdl, MV_RT4_WORK *work )
{
	if( FLDMMDL_ActionLocalAcmd(fmmdl) == TRUE ){
		FLDMMDL_OffStatusBitMove( fmmdl );
		work->seq_no = SEQNO_MV_RT4_MOVE_DIR_SET;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MV_RT4����@�ړ������Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_RT4_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRt4Move_MoveDirSet( FLDMMDL * fmmdl, MV_RT4_WORK *work )
{
	if( work->turn_no == work->turn_check_no  ){
		if( work->turn_check_type == MV_RT3_CHECK_TYPE_X ){
			int ix = FLDMMDL_GetInitGridPosX( fmmdl );
			int gx = FLDMMDL_GetGridPosX( fmmdl );
			
			if( ix == gx ){
				MvRt4Move_TurnNoInc( work );
			}
		}else{
			int iz = FLDMMDL_GetInitGridPosZ( fmmdl );
			int gz = FLDMMDL_GetGridPosZ( fmmdl );
			
			if( iz == gz ){
				MvRt4Move_TurnNoInc( work );
			}
		}
	}
	
	if( work->turn_no == MV_RT3_TURN_END_NO ){
		int ix = FLDMMDL_GetInitGridPosX( fmmdl );
		int iz = FLDMMDL_GetInitGridPosZ( fmmdl );
		int gx = FLDMMDL_GetGridPosX( fmmdl );
		int gz = FLDMMDL_GetGridPosZ( fmmdl );
		
		if( ix == gx && iz == gz ){
			work->turn_no = 0;
		}
	}
	
	{
		const int *tbl;
		int dir,ac;
		u32 ret;
		
		tbl = MoveDirTblIDSearch( work->tbl_id );
		dir = tbl[work->turn_no];
		
		FLDMMDL_SetDirMove( fmmdl, dir );
		
		if( MoveSub_KuruKuruCheck(fmmdl) == FALSE ){
			FLDMMDL_SetDirDisp( fmmdl, dir );
		}
		
		ret = FLDMMDL_HitCheckMoveDir( fmmdl, dir );
		
		if( (ret & FLDMMDL_MOVEHITBIT_LIM) ){
			MvRt4Move_TurnNoInc( work );
			dir = tbl[work->turn_no];
			
			FLDMMDL_SetDirMove( fmmdl, dir );
			
			if( MoveSub_KuruKuruCheck(fmmdl) == FALSE ){
				FLDMMDL_SetDirDisp( fmmdl, dir );
			}
			
			ret = FLDMMDL_HitCheckMoveDir( fmmdl, dir );
		}
		
		ac = AC_WALK_U_8F;
		
		if( (ret != FLDMMDL_MOVEHITBIT_NON) ){
			ac = AC_STAY_WALK_U_8F;
		}
		
		ac = FLDMMDL_ChangeDirAcmdCode( dir, ac );
		FLDMMDL_SetLocalAcmd( fmmdl, ac );
		
		if( MoveSub_KuruKuruCheck(fmmdl) == TRUE ){
			MoveSub_KuruKuruSet( fmmdl, &work->kurukuru );
		}
	}
	
	FLDMMDL_OnStatusBitMove( fmmdl );
	work->seq_no = SEQNO_MV_RT4_MOVE;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_RT4����@�ړ�
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_RT4_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int MvRt4Move_MoveDir( FLDMMDL * fmmdl, MV_RT4_WORK *work )
{
	if( FLDMMDL_ActionLocalAcmd(fmmdl) == TRUE ){
		FLDMMDL_OffStatusBitMove( fmmdl );
		
		if( MoveSub_KuruKuruCheck(fmmdl) == TRUE ){
			MoveSub_KuruKuruEnd( fmmdl, &work->kurukuru );
		}
		
		work->seq_no = SEQNO_MV_RT4_JIKI_CHECK;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
///	MV_RT4����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_MvRt4MoveTbl[])( FLDMMDL * fmmdl, MV_RT4_WORK *work ) =
{
	MvRt4Move_JikiCheck,
	MvRt4Move_DiscoveryJump,
	MvRt4Move_MoveDirSet,
	MvRt4Move_MoveDir,
};

//======================================================================
//	�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �����e�[�u���v�f���擾
 * @param	tbl		�f�[�^�e�[�u���B�I�[��end
 * @param	end		�e�[�u���I�[�Ƃ���l
 * @retval	int		tbl�v�f��
 */
//--------------------------------------------------------------
static int TblNumGet( const int *tbl, int end )
{
	int i=0; while(tbl[i]!=end){i++;} GF_ASSERT(i&&"TblNumGet()�v�f��0"); return(i);
}

//--------------------------------------------------------------
/**
 * �e�[�u���f�[�^���烉���_���Œl�擾
 * @param	tbl		�f�[�^�e�[�u���B�I�[��end
 * @param	end		�e�[�u���I�[�Ƃ���l
 * @retval	int		tbl�����̒l
 */
//--------------------------------------------------------------
static int TblRndGet( const int *tbl, int end )
{
#if 0
	return( tbl[gf_rand()%TblNumGet(tbl,end)] );
#else
	return( tbl[GFUser_GetPublicRand(TblNumGet(tbl,end))] );
#endif
}

//--------------------------------------------------------------
/**
 * ����ID����e�[�u���f�[�^���擾�������_���Œl�擾
 * @param	tbl		�f�[�^�e�[�u���B�I�[��end
 * @param	end		�e�[�u���I�[�Ƃ���l
 * @retval	int		tbl�����̒l
 */
//--------------------------------------------------------------
static int TblIDRndGet( int id, int end )
{
	const int *tbl = MoveDirTblIDSearch( id );
	return( tbl[GFUser_GetPublicRand(TblNumGet(tbl,end))] );
}

//--------------------------------------------------------------
/**
 * DATA_MoveDirTbl����
 * @param	id		����ID
 * @retval	int		�q�b�g�����e�[�u��*
 */
//--------------------------------------------------------------
static const int * MoveDirTblIDSearch( int id )
{
	const DIR_TBL *tbl = DATA_MoveDirTbl;
	
	while( tbl->id != DIRID_END ){
		if( tbl->id == id ){ return( tbl->tbl ); }
		tbl++;
	}
	
	GF_ASSERT( 0 && "MoveDirTblIDSearch()ID�ُ�" );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * ���@���w��͈͓��Ń_�b�V�����Ă��邩
 * @param	fmmdl	FLDMMDL *
 * @retval	int		DIR_NOT=�͈͓��A�����؂�ւ��^�ł͖����B���̑�DIR_UP��
 */
//--------------------------------------------------------------
static int TrJikiDashSearch( FLDMMDL * fmmdl )
{
	int ret = FLDMMDL_GetEventType( fmmdl );
	
#ifndef DEBUG_SEARCH	
	if( ret != EV_TYPE_TRAINER && ret != EV_TYPE_TRAINER_EYEALL ){
		return( DIR_NOT );
	}
#endif

#if 0	
	{
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
		
#ifndef DEBUG_SEARCH		
		if( Player_AcmdCodeDashCheck(jiki) == FALSE ){
			return( DIR_NOT );
		}
#endif		
		{
			int code,i = 0;
			ret = FLDMMDL_GetMoveCode( fmmdl );
			
			do{
				code = JikiDashSensorMoveCodeTbl[i++];
				if( code == ret ){ break; }
			}while( code != MV_CODE_NOT );
			
			if( ret != code ){
				return( DIR_NOT );
			}
		}
		
		{
			const FLDMMDL * jikiobj = Player_FieldOBJGet( jiki );
			int jy = FLDMMDL_GetHeightGrid( jikiobj );
			int y = FLDMMDL_GetHeightGrid( fmmdl );
			
			if( jy != y ){
				return( DIR_NOT );
			}
		}
		
		{
			int jx = Player_NowGPosXGet( jiki );
			int jz = Player_NowGPosZGet( jiki );
#ifndef DEBUG_SEARCH
			int range = FLDMMDL_GetParam( fmmdl, FLDMMDL_PARAM_0 );
#else
			int range = 4;
#endif
			int x = FLDMMDL_GetGridPosX( fmmdl );
			int z = FLDMMDL_GetGridPosZ( fmmdl );
			int sx = x - range;
			int ex = x + range;
			int sz = z - range;
			int ez = z + range;
			
			if( sz <= jz && ez >= jz ){
				if( sx <= jx && ex >= jx ){
					return( FLDMMDL_TOOL_GetRangeDir(x,z,jx,jz) );
				}
			}
		}
	}
#endif	
	return( DIR_NOT );
}

//--------------------------------------------------------------
/**
 * ���@���w��͈͓��Ń_�b�V�����Ă���΂��̕�����Ԃ��B�����e�[�u���A��
 * @param	fmmdl	FLDMMDL *
 * @retval	int		DIR_NOT=�͈͓��A�����؂�ւ��^�ł͖����B���̑�DIR_UP��
 */
//--------------------------------------------------------------
static int TrJikiDashSearchTbl( FLDMMDL * fmmdl, int id, int end )
{
	const int *tbl = MoveDirTblIDSearch( id );
	int num = TblNumGet( tbl, end );
	
	if( num == 1 ){			//������̂�
		return( DIR_NOT );
	}
	
	{
		int dir;
		dir = TrJikiDashSearch( fmmdl );
		
		if( dir == DIR_NOT ){
			return( dir );
		}
#if 0		
		{
			int i = 0;
			
			do{ if(tbl[i]==dir){return(dir);} i++; }while( i < num );
			
			{
				int dirx = DIR_NOT,dirz = DIR_NOT;
				int x = FLDMMDL_GetGridPosX( fmmdl );
				int z = FLDMMDL_GetGridPosZ( fmmdl );
				FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
				PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
				int jx = Player_NowGPosXGet( jiki );
				int jz = Player_NowGPosZGet( jiki );
				
				if( x > jx ){ dirx = DIR_LEFT; }
				else if( x < jx ){ dirx = DIR_RIGHT; }
				
				if( z > jz ){ dirz = DIR_UP; }
				else if( z < jz ){ dirz = DIR_DOWN; }
				
				i = 0;
				
				if( dirx == DIR_NOT ){
					do{ if(tbl[i]==dirz){return(dirz);} i++; }while( i < num );
				}else if( dirz == DIR_NOT ){ 
					do{ if(tbl[i]==dirx){return(dirx);} i++; }while( i < num );
				}else{
					do{ if(tbl[i]==dirx){return(dirx);}
						if(tbl[i]==dirz){return(dirz);} i++; }while( i < num );
				}
			}	
		}
#endif
	}
	
	return( DIR_NOT );
}

//======================================================================
//	���[�g�^�@�N���N���ړ��p�[�c
//======================================================================
//--------------------------------------------------------------
///	�N���N���e�[�u��
//--------------------------------------------------------------
static const int DATA_KuruKuruTbl[RT_KURU2_MAX][RT_KURU2_DIR_MAX] =
{
	{DIR_UP,DIR_LEFT,DIR_DOWN,DIR_RIGHT},
	{DIR_UP,DIR_RIGHT,DIR_DOWN,DIR_LEFT},
};

//--------------------------------------------------------------
/**
 * �N���N���^�C�v�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�N���N�����
 */
//--------------------------------------------------------------
static int MoveSub_KuruKuruCheck( FLDMMDL * fmmdl )
{
	int type = FLDMMDL_GetEventType( fmmdl );
	
	if( type == EV_TYPE_TRAINER_SPIN_MOVE_L	||
		type == EV_TYPE_TRAINER_SPIN_MOVE_R	){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���邭��^�C�v������
 * @param	fmmdl	FLDMMDL *
 * @param	work	RT_KURUKURU_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MoveSub_KuruKuruInit( FLDMMDL * fmmdl, RT_KURUKURU_WORK *work )
{
	if( FLDMMDL_GetEventType(fmmdl) == EV_TYPE_TRAINER_SPIN_MOVE_L ){
		work->spin_type = RT_KURU2_L;
	}else{
		work->spin_type = RT_KURU2_R;
	}
}

//--------------------------------------------------------------
/**
 * ���邭��^�C�v�����Z�b�g�B
 * ���ӁF�������Œ肳���
 * @param	fmmdl	FLDMMDL *
 * @param	work	RT_KURUKURU_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MoveSub_KuruKuruSet( FLDMMDL * fmmdl, RT_KURUKURU_WORK *work )
{
	int i,dir = FLDMMDL_GetDirDisp( fmmdl );
	
#if 0	
	{
		switch( dir ){
		case DIR_UP: KAGAYA_Printf( "���邭��ړ� 0 �����͏�@" ); break;
		case DIR_DOWN: KAGAYA_Printf( "���邭��ړ� 0 �����͉��@" ); break;
		case DIR_LEFT: KAGAYA_Printf( "���邭��ړ� 0 �����͍��@" ); break;
		case DIR_RIGHT: KAGAYA_Printf( "���邭��ړ� 0 �����͉E�@" ); break;
		}
	}
#endif
	
	for( i = 0; (i < RT_KURU2_DIR_MAX && dir != DATA_KuruKuruTbl[work->spin_type][i]); i++ ){}
	GF_ASSERT( i < RT_KURU2_DIR_MAX && "MoveSub_KuruKuruInit()�����ُ�" );
	
	work->origin_dir = dir;
	i = (i + 1) % RT_KURU2_DIR_MAX;								//+1=1��
	dir = DATA_KuruKuruTbl[work->spin_type][i];
	
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_PAUSE_DIR) ){
		work->dir_pause = TRUE;
	}else{
		work->dir_pause = FALSE;
	}
	
	FLDMMDL_SetDirDisp( fmmdl, dir );
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_PAUSE_DIR );
#if 0	
	{
		switch( dir ){
		case DIR_UP: KAGAYA_Printf( "1�����͏�\n" ); break;
		case DIR_DOWN: KAGAYA_Printf( "1�����͉�\n" ); break;
		case DIR_LEFT: KAGAYA_Printf( "1�����͍�\n" ); break;
		case DIR_RIGHT: KAGAYA_Printf(  " 1�����͉E\n" ); break;
		}
	}
#endif
}

//--------------------------------------------------------------
/**
 * ���邭��^�C�v�����I��
 * @param	fmmdl	FLDMMDL *
 * @param	work	RT_KURUKURU_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MoveSub_KuruKuruEnd( FLDMMDL * fmmdl, RT_KURUKURU_WORK *work )
{
	if( work->dir_pause == FALSE ){
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_PAUSE_DIR );
	}
}

//======================================================================
//	data
//======================================================================
//--------------------------------------------------------------
///	DIR_RND �E�F�C�g�e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndWaitTbl[] =
{ 16, 32, 48, 64, WAIT_END };

//--------------------------------------------------------------
///	DIR_RND �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl[] =
{ DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UL �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UL[] =
{ DIR_UP, DIR_LEFT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UR[] =
{ DIR_UP, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_DL �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_DL[] =
{ DIR_DOWN, DIR_LEFT, DIR_NOT };

//--------------------------------------------------------------
/// RND_DR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_DR[] =
{ DIR_DOWN, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UDL �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UDL[] =
{ DIR_UP, DIR_DOWN, DIR_LEFT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UDR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UDR[] =
{ DIR_UP, DIR_DOWN, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_ULR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_ULR[] =
{ DIR_UP, DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_DLR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_DLR[] =
{ DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
/// RND_UD �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_UD[] =
{ DIR_UP, DIR_DOWN, DIR_NOT };

//--------------------------------------------------------------
/// RND_LR �����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirRndDirTbl_LR[] =
{ DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
///	MV_RND �ړ��\����
//--------------------------------------------------------------
static const int DATA_MV_RND_DirTbl[] =
{ DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
///	MV_RND_V �ړ��\����
//--------------------------------------------------------------
static const int DATA_MV_RND_V_DirTbl[] =
{ DIR_UP, DIR_DOWN, DIR_NOT };

//--------------------------------------------------------------
///	MV_RND_H �ړ��\����
//--------------------------------------------------------------
static const int DATA_MV_RND_H_DirTbl[] =
{ DIR_LEFT, DIR_RIGHT, DIR_NOT };

//--------------------------------------------------------------
///	MV_RTURLD�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtURLD_DirTbl[] = {DIR_UP,DIR_RIGHT,DIR_LEFT,DIR_DOWN};

//--------------------------------------------------------------
///	MV_RTRLDU�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtRLDU_DirTbl[] = {DIR_RIGHT,DIR_LEFT,DIR_DOWN,DIR_UP};

//--------------------------------------------------------------
///	MV_RTDURL�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtDURL_DirTbl[] = {DIR_DOWN,DIR_UP,DIR_RIGHT,DIR_LEFT};

//--------------------------------------------------------------
///	MV_RTLDUR�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtLDUR_DirTbl[] = {DIR_LEFT,DIR_DOWN,DIR_UP,DIR_RIGHT};

//--------------------------------------------------------------
///	MV_RTULRD�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtULRD_DirTbl[] = {DIR_LEFT,DIR_RIGHT,DIR_DOWN,DIR_UP};

//--------------------------------------------------------------
///	MV_RTLRDU�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtLRDU_DirTbl[] = {DIR_LEFT,DIR_RIGHT,DIR_DOWN,DIR_UP};

//--------------------------------------------------------------
///	MV_RTDULR�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtDULR_DirTbl[] = {DIR_DOWN,DIR_UP,DIR_LEFT,DIR_RIGHT};

//--------------------------------------------------------------
///	MV_RTRDUL�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtRDUL_DirTbl[] = {DIR_RIGHT,DIR_DOWN,DIR_UP,DIR_LEFT};

//--------------------------------------------------------------
///	MV_RTLUDR�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtLUDR_DirTbl[] = {DIR_LEFT,DIR_UP,DIR_DOWN,DIR_RIGHT};

//--------------------------------------------------------------
///	MV_RTUDRL�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtUDRL_DirTbl[] = {DIR_UP,DIR_DOWN,DIR_RIGHT,DIR_LEFT};

//--------------------------------------------------------------
///	MV_RTRLUD�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtRLUD_DirTbl[] = {DIR_RIGHT,DIR_LEFT,DIR_UP,DIR_DOWN};

//--------------------------------------------------------------
///	MV_RTDRLU�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtDRLU_DirTbl[] = {DIR_DOWN,DIR_RIGHT,DIR_LEFT,DIR_UP};

//--------------------------------------------------------------
///	MV_RTRUDL�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtRUDL_DirTbl[] = {DIR_RIGHT,DIR_UP,DIR_DOWN,DIR_LEFT};

//--------------------------------------------------------------
///	MV_RTUDLR�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtUDLR_DirTbl[] = {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT};

//--------------------------------------------------------------
///	MV_RTLRUD�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtLRUD_DirTbl[] = {DIR_LEFT,DIR_RIGHT,DIR_UP,DIR_DOWN};

//--------------------------------------------------------------
///	MV_RTDLRU�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtDLRU_DirTbl[] = {DIR_DOWN,DIR_LEFT,DIR_RIGHT,DIR_UP};

//--------------------------------------------------------------
///	MV_RTUL�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtUL_DirTbl[] = {DIR_UP,DIR_LEFT,DIR_DOWN,DIR_RIGHT};

//--------------------------------------------------------------
///	MV_RTDR�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtDR_DirTbl[] = {DIR_DOWN,DIR_RIGHT,DIR_UP,DIR_LEFT};

//--------------------------------------------------------------
///	MV_RTLD�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtLD_DirTbl[] = {DIR_LEFT,DIR_DOWN,DIR_RIGHT,DIR_UP};

//--------------------------------------------------------------
///	MV_RTRU�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtRU_DirTbl[] = {DIR_RIGHT,DIR_UP,DIR_LEFT,DIR_DOWN};

//--------------------------------------------------------------
///	MV_RTUR�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtUR_DirTbl[] = {DIR_UP,DIR_RIGHT,DIR_DOWN,DIR_LEFT};

//--------------------------------------------------------------
///	MV_RTDL�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtDL_DirTbl[] = {DIR_DOWN,DIR_LEFT,DIR_UP,DIR_RIGHT};

//--------------------------------------------------------------
///	MV_RTLU�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtLU_DirTbl[] = {DIR_LEFT,DIR_UP,DIR_RIGHT,DIR_DOWN};

//--------------------------------------------------------------
///	MV_RTRD�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvRtRD_DirTbl[] = {DIR_RIGHT,DIR_DOWN,DIR_LEFT,DIR_UP};

//--------------------------------------------------------------
///	MV_SPIN�n�����e�[�u��
//--------------------------------------------------------------
static const int DATA_MvDirSpin_DirTbl[] = {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT,DIR_NOT};

//--------------------------------------------------------------
///	�e�[�u���f�[�^
//--------------------------------------------------------------
static const DIR_TBL DATA_MoveDirTbl[DIRID_MAX] =
{
	{DIRID_MvDirRndDirTbl,DATA_MvDirRndDirTbl},
	{DIRID_MvDirRndDirTbl_UL,DATA_MvDirRndDirTbl_UL},
	{DIRID_MvDirRndDirTbl_UR,DATA_MvDirRndDirTbl_UR},
	{DIRID_MvDirRndDirTbl_DL,DATA_MvDirRndDirTbl_DL},
	{DIRID_MvDirRndDirTbl_DR,DATA_MvDirRndDirTbl_DR},
	{DIRID_MvDirRndDirTbl_UDL,DATA_MvDirRndDirTbl_UDL},
	{DIRID_MvDirRndDirTbl_UDR,DATA_MvDirRndDirTbl_UDR},
	{DIRID_MvDirRndDirTbl_ULR,DATA_MvDirRndDirTbl_ULR},
	{DIRID_MvDirRndDirTbl_DLR,DATA_MvDirRndDirTbl_DLR},
	{DIRID_MvDirRndDirTbl_UD,DATA_MvDirRndDirTbl_UD},
	{DIRID_MvDirRndDirTbl_LR,DATA_MvDirRndDirTbl_LR},
	{DIRID_MV_RND_DirTbl,DATA_MV_RND_DirTbl},
	{DIRID_MV_RND_V_DirTbl,DATA_MV_RND_V_DirTbl},
	{DIRID_MV_RND_H_DirTbl,DATA_MV_RND_H_DirTbl},
	{DIRID_MvRtURLD_DirTbl,DATA_MvRtURLD_DirTbl},
	{DIRID_MvRtRLDU_DirTbl,DATA_MvRtRLDU_DirTbl},
	{DIRID_MvRtDURL_DirTbl,DATA_MvRtDURL_DirTbl},
	{DIRID_MvRtLDUR_DirTbl,DATA_MvRtLDUR_DirTbl},
	{DIRID_MvRtULRD_DirTbl,DATA_MvRtULRD_DirTbl},
	{DIRID_MvRtLRDU_DirTbl,DATA_MvRtLRDU_DirTbl},
	{DIRID_MvRtDULR_DirTbl,DATA_MvRtDULR_DirTbl},
	{DIRID_MvRtRDUL_DirTbl,DATA_MvRtRDUL_DirTbl},
	{DIRID_MvRtLUDR_DirTbl,DATA_MvRtLUDR_DirTbl},
	{DIRID_MvRtUDRL_DirTbl,DATA_MvRtUDRL_DirTbl},
	{DIRID_MvRtRLUD_DirTbl,DATA_MvRtRLUD_DirTbl},
	{DIRID_MvRtDRLU_DirTbl,DATA_MvRtDRLU_DirTbl},
	{DIRID_MvRtRUDL_DirTbl,DATA_MvRtRUDL_DirTbl},
	{DIRID_MvRtUDLR_DirTbl,DATA_MvRtUDLR_DirTbl},
	{DIRID_MvRtLRUD_DirTbl,DATA_MvRtLRUD_DirTbl},
	{DIRID_MvRtDLRU_DirTbl,DATA_MvRtDLRU_DirTbl},
	{DIRID_MvRtUL_DirTbl,DATA_MvRtUL_DirTbl},
	{DIRID_MvRtDR_DirTbl,DATA_MvRtDR_DirTbl},
	{DIRID_MvRtLD_DirTbl,DATA_MvRtLD_DirTbl},
	{DIRID_MvRtRU_DirTbl,DATA_MvRtRU_DirTbl},
	{DIRID_MvRtUR_DirTbl,DATA_MvRtUR_DirTbl},
	{DIRID_MvRtDL_DirTbl,DATA_MvRtDL_DirTbl},
	{DIRID_MvRtLU_DirTbl,DATA_MvRtLU_DirTbl},
	{DIRID_MvRtRD_DirTbl,DATA_MvRtRD_DirTbl},
	{DIRID_MvDirSpin_DirTbl,DATA_MvDirSpin_DirTbl},
	{DIRID_END,NULL},
};

//--------------------------------------------------------------
///	���@�_�b�V�����m����R�[�h
//--------------------------------------------------------------
static const int JikiDashSensorMoveCodeTbl[] =
{
	MV_DIR_RND,
	MV_RND_UL,MV_RND_UR,
	MV_RND_DL,MV_RND_DR,MV_RND_UDL,MV_RND_UDR,MV_RND_ULR,MV_RND_DLR,
	MV_RND_UD,MV_RND_LR,
	MV_SPIN_L,MV_SPIN_R,
	MV_CODE_NOT
};
	
