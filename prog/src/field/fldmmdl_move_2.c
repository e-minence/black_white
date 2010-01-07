//======================================================================
/**
 *
 * @file	fieldobj_move_2.c
 * @brief	�t�B�[���h���샂�f���@��{����n����2
 * @author	kagaya
 * @date	05.07.21
 *
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procmove.h"

//======================================================================
//	define
//======================================================================
///�g���[�i�[�y�A�@�e�ړ���~
#define PAIR_TR_OYA_MOVEBIT_STOP \
	(MMDL_MOVEBIT_ATTR_GET_ERROR		| \
	 MMDL_MOVEBIT_HEIGHT_GET_ERROR	| \
	 MMDL_MOVEBIT_PAUSE_MOVE)

//--------------------------------------------------------------
///	�R�s�[����ԍ�
//--------------------------------------------------------------
enum
{
	SEQNO_COPYMOVE_FIRST_INIT,
	SEQNO_COPYMOVE_FIRST_WAIT,
	SEQNO_COPYMOVE_INIT,
	SEQNO_COPYMOVE_CMD_SET,
	SEQNO_COPYMOVE_CMD_WAIT,
	SEQNO_COPYMOVE_MAX,
};

//--------------------------------------------------------------
///	�ǈړ��@������
//--------------------------------------------------------------
typedef enum
{
	ALONG_L = 0,		///<��
	ALONG_R,			///<�E
	ALONG_LR,			///<���E
}ALONG_DIR;

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
//	MV_PAIR_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;				///<����ԍ�
	u8 jiki_init;			///<���@��񏉊�������
	s16 jiki_gx;			///<���@�O���b�h���WX
	s16 jiki_gz;			///<���@�O���b�h���WZ
	u16 jiki_ac;			///<���@�A�j���[�V�����R�[�h
}MV_PAIR_WORK;

#define MV_PAIR_WORK_SIZE (sizeof(MV_PAIR_WORK))

//--------------------------------------------------------------
//	MV_TR_PAIR_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;				///<����ԍ�
	u8 oya_init;			///<�e��񏉊�������
	s16 oya_gx;				///<�e�O���b�h���WX
	s16 oya_gz;				///<�e�O���b�h���WZ
	u16 oya_ac;				///<���@�A�j���[�V�����R�[�h
	MMDL * oyaobj;		///<�e�ƂȂ�OBJ
}MV_TR_PAIR_WORK;

#define MV_TR_PAIR_WORK_SIZE (sizeof(MV_TR_PAIR_WORK))

//--------------------------------------------------------------
///	MV_HIDE_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;
	u8 hide_type;
	u8 pulloff_flag;
	u8 dmy;
	#ifndef MMDL_PL_NULL
	EOA_PTR eoa_hide;
	#endif
}MV_HIDE_WORK;

#define MV_HIDE_WORK_SIZE (sizeof(MV_HIDE_WORK))

//--------------------------------------------------------------
///	MV_COPY_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;
	s8 dir_jiki;
	u8 lgrass_on;
	u8 dmy;
}MV_COPY_WORK;

#define MV_COPY_WORK_SIZE (sizeof(MV_COPY_WORK))

//--------------------------------------------------------------
///	MV_ALONGW_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u32 seq_no;
	ALONG_DIR dir_hand_init;
	ALONG_DIR dir_hand;
}MV_ALONGW_WORK;

#define MV_ALONGW_WORK_SIZE (sizeof(MV_ALONGW_WORK))

//======================================================================
//	�v���g�^�C�v
//======================================================================
int (* const DATA_PairMoveTbl[])( MMDL * fmmdl, MV_PAIR_WORK *work );

static int Pair_WorkSetJikiSearch( MMDL * fmmdl, MV_PAIR_WORK *work );
static void Pair_WorkInit( MMDL * fmmdl, MV_PAIR_WORK *work );
static int Pair_JikiPosUpdateCheck( MMDL * fmmdl, MV_PAIR_WORK *work );
static void Pair_JikiPosSet( MMDL * fmmdl, MV_PAIR_WORK *work );
static u32 Pair_JikiAcmdCodeGet( MMDL * fmmdl );
static int Pair_JikiCheckAcmdSet( MMDL * fmmdl );

int (* const DATA_PairTrMoveTbl[])( MMDL * fmmdl, MV_TR_PAIR_WORK *work );

static int PairTr_WorkSetOyaSearch( MMDL * fmmdl, MV_TR_PAIR_WORK *work );
static void PairTr_WorkInit( MMDL * fmmdl, MV_TR_PAIR_WORK *work, MMDL * oyaobj);
static int PairTr_OyaPosUpdateCheck( MMDL * fmmdl, MV_TR_PAIR_WORK *work );
static void PairTr_OyaPosSet( MMDL * fmmdl, MV_TR_PAIR_WORK *work );
static int PairTr_OyaCheckAcmdSet( MMDL * fmmdl, MV_TR_PAIR_WORK *work );

int (* const DATA_HideMoveTbl[])( MMDL * fmmdl, MV_HIDE_WORK *work );

//======================================================================
//	MV_PAIR	���@�A�����
//======================================================================
//--------------------------------------------------------------
/**
 * MV_PAIR�@������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MovePair_Init( MMDL * fmmdl )
{
	MV_PAIR_WORK *work = MMDL_InitMoveProcWork( fmmdl, MV_PAIR_WORK_SIZE );
	Pair_WorkSetJikiSearch( fmmdl, work );
	MMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );
	MMDL_OffMoveBitMove( fmmdl );
	MMDL_SetStatusBitFellowHit( fmmdl, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_PAIR�@����
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MovePair_Move( MMDL * fmmdl )
{
	MV_PAIR_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	
	if( Pair_WorkSetJikiSearch(fmmdl,work) == FALSE ){
		return;
	}
	
	MMDL_SetStatusBitFellowHit( fmmdl, FALSE );
	
	while( DATA_PairMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_PAIR �폜
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MovePair_Delete( MMDL * fmmdl )
{
}

//======================================================================
//	MV_PAIR ����
//======================================================================
//--------------------------------------------------------------
/**
 * Pair 0
 * @param	fmmdl	MMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairMove_Init( MMDL * fmmdl, MV_PAIR_WORK *work )
{
	MMDL_OffMoveBitMove( fmmdl );
	MMDL_OffMoveBitMoveEnd( fmmdl );
		
	if( Pair_JikiPosUpdateCheck(fmmdl,work) == TRUE ){
		Pair_JikiPosSet( fmmdl, work );
		
		if( Pair_JikiCheckAcmdSet(fmmdl) == TRUE ){
			MMDL_OnMoveBitMove( fmmdl );
			work->seq_no++;
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * Pair 1
 * @param	fmmdl	MMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairMove_Move( MMDL * fmmdl, MV_PAIR_WORK *work )
{
	if( MMDL_ActionLocalAcmd(fmmdl) == TRUE ){
		MMDL_OffMoveBitMove( fmmdl );
		work->seq_no = 0;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
///	�ꂠ�邫����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_PairMoveTbl[])( MMDL * fmmdl, MV_PAIR_WORK *work ) =
{
	PairMove_Init,
	PairMove_Move,
};

//======================================================================
//	MV_PAIR�@�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�����݂��邩�`�F�b�N�@���݂���̂ł���Ώ�񏉊��� 
 * @param	fmmdl	MMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		FALSE=���݂��Ă��Ȃ�
 */
//--------------------------------------------------------------
static int Pair_WorkSetJikiSearch( MMDL * fmmdl, MV_PAIR_WORK *work )
{
#ifndef MMDL_PL_NULL
	const MMDLSYS *fos = MMDL_GetMMdlSys( fmmdl );
	MMDL * jikiobj = Player_FieldOBJSearch( fos );
	
	if( jikiobj == NULL ){
		work->jiki_init = FALSE;
		return( FALSE );
	}
	
	if( work->jiki_init == FALSE ){
		Pair_WorkInit( fmmdl, work );
	}
	
	return( TRUE );
#else
	return( FALSE );
#endif
}

//--------------------------------------------------------------
/**
 * MV_PAIR_WORK�������@���@�����鎖�O��
 * @param	fmmdl	MMDL *	
 * @param	work	MV_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Pair_WorkInit( MMDL * fmmdl, MV_PAIR_WORK *work )
{
#ifndef MMDL_PL_NULL
	FIELDSYS_WORK *fsys = MMDL_FieldSysWorkGet( fmmdl );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	
	work->jiki_init = TRUE;
	work->jiki_gx = Player_NowGPosXGet( jiki );
	work->jiki_gz = Player_NowGPosZGet( jiki );
	work->jiki_ac = ACMD_NOT;
#endif
}

//--------------------------------------------------------------
/**
 * ���@���W�X�V�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�X�V����
 */
//--------------------------------------------------------------
static int Pair_JikiPosUpdateCheck( MMDL * fmmdl, MV_PAIR_WORK *work )
{
#ifndef MMDL_PL_NULL
	FIELDSYS_WORK *fsys = MMDL_FieldSysWorkGet( fmmdl );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	
	if( jiki != NULL ){
		int gx = Player_NowGPosXGet( jiki );
		int gz = Player_NowGPosZGet( jiki );
		
		if( gx != work->jiki_gx || gz != work->jiki_gz ){
			return( TRUE );
		}
	}
#endif	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���W�Z�b�g
 * @param	fmmdl	MMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Pair_JikiPosSet( MMDL * fmmdl, MV_PAIR_WORK *work )
{
#ifndef MMDL_PL_NULL
	FIELDSYS_WORK *fsys = MMDL_FieldSysWorkGet( fmmdl );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	
	work->jiki_gx = Player_NowGPosXGet( jiki );
	work->jiki_gz = Player_NowGPosZGet( jiki );
#endif
}

//--------------------------------------------------------------
/**
 * ���@�A�j���R�[�h�擾
 * @param	fmmdl	MMDL *
 * @retval	u32		�A�j���[�V�����R�[�h
 */
//--------------------------------------------------------------
static u32 Pair_JikiAcmdCodeGet( MMDL * fmmdl )
{
#ifndef MMDL_PL_NULL
	u32 code;
	FIELDSYS_WORK *fsys = MMDL_FieldSysWorkGet( fmmdl );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	code = Player_AcmdCodeGet( jiki );
	
	switch( code ){
	case AC_DASH_U_4F: code = AC_WALK_U_4F; break;
	case AC_DASH_D_4F: code = AC_WALK_D_4F; break;
	case AC_DASH_L_4F: code = AC_WALK_L_4F; break;
	case AC_DASH_R_4F: code = AC_WALK_R_4F; break;
	}
	
	return( code );
#else
	return( 0 );
#endif
}

//--------------------------------------------------------------
/**
 * ���@�A�j���R�[�h�A�������擾���A�j���[�V�����R�}���h���Z�b�g
 * @param	fmmdl	MMDL *
 * @retval	int		TRUE=�Z�b�g�ł��� FALSE=�d�Ȃ�ɂ��Z�b�g�ł��Ȃ�
 */
//--------------------------------------------------------------
static int Pair_JikiCheckAcmdSet( MMDL * fmmdl )
{
#ifndef MMDL_PL_NULL
	FIELDSYS_WORK *fsys = MMDL_FieldSysWorkGet( fmmdl );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	int gx = MMDL_GetGridPosX( fmmdl );
	int gz = MMDL_GetGridPosZ( fmmdl );
	int jx = Player_OldGPosXGet( jiki );
	int jz = Player_OldGPosZGet( jiki );
	
	if( gx != jx || gz != jz ){
		u32 code = Pair_JikiAcmdCodeGet( fmmdl );
		int dir = MMDL_TOOL_GetRangeDir( gx, gz, jx, jz );
		code = MMDL_ChangeDirAcmdCode( dir, code );
		MMDL_SetLocalAcmd( fmmdl, code );
		
		return( TRUE );
	}
#else	
	return( FALSE );
#endif
}

//======================================================================
//	MV_TR_PAIR �g���[�i�[�A�����
//======================================================================
//--------------------------------------------------------------
/**
 * MV_TR_PAIR�@������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MovePairTr_Init( MMDL * fmmdl )
{
	MV_TR_PAIR_WORK *work = MMDL_InitMoveProcWork( fmmdl, MV_TR_PAIR_WORK_SIZE );
	PairTr_WorkSetOyaSearch( fmmdl, work );
	MMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );
	MMDL_OffMoveBitMove( fmmdl );
	
//	MMDL_SetStatusBitFellowHit( fmmdl, FALSE );
	work->oya_init = FALSE;
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR�@����
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MovePairTr_Move( MMDL * fmmdl )
{
	MV_TR_PAIR_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	
	if( PairTr_WorkSetOyaSearch(fmmdl,work) == FALSE ){
		return;
	}
	
//	MMDL_SetStatusBitFellowHit( fmmdl, FALSE );
	while( DATA_PairTrMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR �폜
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MovePairTr_Delete( MMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR�@���A
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MovePairTr_Return( MMDL * fmmdl )
{
	MV_TR_PAIR_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	work->oya_init = 0;
//	PairTr_WorkSetOyaSearch( fmmdl, work );
}

//======================================================================
//	MV_TR_PAIR ����
//======================================================================
//--------------------------------------------------------------
/**
 * Pair 0
 * @param	fmmdl	MMDL *
 * @param	work	MV_TR_PAIR_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairTrMove_Init( MMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	MMDL_OffMoveBitMove( fmmdl );
	MMDL_OffMoveBitMoveEnd( fmmdl );
		
	if( PairTr_OyaPosUpdateCheck(fmmdl,work) == TRUE ){
		if( PairTr_OyaCheckAcmdSet(fmmdl,work) == TRUE ){
//			PairTr_OyaPosSet( fmmdl, work );
			MMDL_OnMoveBitMove( fmmdl );
			work->seq_no++;
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * Pair 1
 * @param	fmmdl	MMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairTrMove_Move( MMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	if( MMDL_ActionLocalAcmd(fmmdl) == FALSE ){
		return( FALSE );
	}
	
	MMDL_OffMoveBitMove( fmmdl );
	work->seq_no = 0;
	return( FALSE );
}

//--------------------------------------------------------------
///	�ꂠ�邫����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_PairTrMoveTbl[])( MMDL * fmmdl, MV_TR_PAIR_WORK *work ) =
{
	PairTrMove_Init,
	PairTrMove_Move,
};

//======================================================================
//	MV_TR_PAIR�@�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �Ώۂ��y�A���삩�ǂ����B�y�A����ł���Α�����T��
 * @param	fmmdl	MMDL *
 * @retval	MMDL *�@������MMDL *�ANULL=�y�A����ł͂Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDL_MovePairSearch( MMDL * fmmdl )
{
#ifndef MMDL_PL_NULL
	u32 no = 0;
	int type = MMDL_GetEventType( fmmdl );
	int zone_id = MMDL_GetZoneID( fmmdl );
	u32 trid = EvTrainerfmmdlTrainerIDGet( fmmdl );
	const MMDLSYS *fos = MMDL_GetMMdlSys( fmmdl );
	MMDL * pair;
	
	switch( type ){
	case EV_TYPE_TRAINER:
	case EV_TYPE_TRAINER_EYEALL:
	case EV_TYPE_ITEM:
	case EV_TYPE_TRAINER_KYORO:
	case EV_TYPE_TRAINER_SPIN_STOP_L:
	case EV_TYPE_TRAINER_SPIN_STOP_R:
	case EV_TYPE_TRAINER_SPIN_MOVE_L:
	case EV_TYPE_TRAINER_SPIN_MOVE_R:
		while( MMDLSYS_SearchUseMMdl(fos,&pair,&no,MMDL_STABIT_USE) == TRUE ){
			if( fmmdl != pair && MMDL_GetZoneID(pair) == zone_id ){
				if( EvTrainerfmmdlTrainerIDGet(pair) == trid ){
					return( pair );
				}
			}
		}
	}
#endif	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * �e�����݂��邩�`�F�b�N�@���݂���̂ł���Ώ�񏉊��� 
 * @param	fmmdl	MMDL *
 * @param	work	MV_TR_PAIR_WORK
 * @retval	int		FALSE=���݂��Ă��Ȃ�
 */
//--------------------------------------------------------------
static int PairTr_WorkSetOyaSearch( MMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
#ifndef MMDL_PL_NULL
	int zone;
	u32 trid,no;
	MMDL *oyaobj;
	const MMDLSYS *fos = MMDL_GetMMdlSys( fmmdl );
	
	no = 0;
	zone = MMDL_GetZoneID( fmmdl );
	trid = EvTrainerfmmdlTrainerIDGet( fmmdl );
	
	while( MMDLSYS_SearchUseMMdl(
		fos,&oyaobj,&no,MMDL_STABIT_USE) == TRUE ){
		if( fmmdl != oyaobj &&
			MMDL_GetZoneID(oyaobj) == zone &&
			EvTrainerfmmdlTrainerIDGet(oyaobj) == trid ){
			
			if( work->oya_init == FALSE ){
				PairTr_WorkInit( fmmdl, work, oyaobj );
			}
			
			return( TRUE );
		}
	}
	
	work->oya_init = FALSE;
#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR_WORK�������@�e�����鎖�O��
 * @param	fmmdl	MMDL *	
 * @param	work	MV_TR_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PairTr_WorkInit( MMDL * fmmdl, MV_TR_PAIR_WORK *work, MMDL * oyaobj )
{
	work->oya_init = TRUE;
	work->oya_gx = MMDL_GetGridPosX( oyaobj );
	work->oya_gz = MMDL_GetGridPosZ( oyaobj );
	work->oya_ac = ACMD_NOT;
	work->oyaobj = oyaobj;
}

//--------------------------------------------------------------
/**
 * �e���W�X�V�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�X�V����
 */
//--------------------------------------------------------------
static int PairTr_OyaPosUpdateCheck( MMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	MMDL * oyaobj = work->oyaobj;
	int gx = MMDL_GetGridPosX( fmmdl );
	int gz = MMDL_GetGridPosZ( fmmdl );
	int ngx = MMDL_GetOldGridPosX( oyaobj );
	int ngz = MMDL_GetOldGridPosZ( oyaobj );
	
	if( (gx != ngx || gz != ngz) &&
		(MMDL_CheckMoveBitMove(oyaobj) == TRUE ||
		MMDL_CheckMoveBit(oyaobj,PAIR_TR_OYA_MOVEBIT_STOP) == 0) ){
		return( TRUE );
	}
	
	return( FALSE );
}

#if 0	//�����̍������l�����Ă��Ȃ�
static int PairTr_OyaPosUpdateCheck( MMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	MMDL * oyaobj = work->oyaobj;
	int gx = MMDL_GetGridPosX( fmmdl );
	int gz = MMDL_GetGridPosZ( fmmdl );
	int ngx = MMDL_GetOldGridPosX( oyaobj );
	int ngz = MMDL_GetOldGridPosZ( oyaobj );
	
	if( (gx != ngx || gz != ngz) &&
		MMDL_CheckStatusBit(oyaobj,MMDL_STABIT_ATTR_GET_ERROR) == 0 &&
		MMDL_CheckStatusBit(oyaobj,MMDL_STABIT_PAUSE_MOVE) == 0 ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

#if 0
static int PairTr_OyaPosUpdateCheck( MMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	MMDL * oyaobj = work->oyaobj;
	int gx = MMDL_GetGridPosX( oyaobj );
	int gz = MMDL_GetGridPosZ( oyaobj );
	
	if( (gx != work->oya_gx || gz != work->oya_gz) &&
		MMDL_CheckStatusBit(oyaobj,MMDL_STABIT_ATTR_GET_ERROR) == 0 &&
		MMDL_CheckStatusBit(oyaobj,MMDL_STABIT_PAUSE_MOVE) == 0 ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * �e���W�Z�b�g
 * @param	fmmdl	MMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PairTr_OyaPosSet( MMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	work->oya_gx = MMDL_GetGridPosX( work->oyaobj );
	work->oya_gz = MMDL_GetGridPosZ( work->oyaobj );
}

//--------------------------------------------------------------
/**
 * �e�A�j���R�[�h�A�������擾���A�j���[�V�����R�}���h���Z�b�g
 * @param	fmmdl	MMDL *
 * @retval	int		TRUE=�Z�b�g�ł��� FALSE=�d�Ȃ�ɂ��Z�b�g�ł��Ȃ�
 */
//--------------------------------------------------------------
static int PairTr_OyaCheckAcmdSet( MMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	int gx = MMDL_GetGridPosX( fmmdl );
	int gz = MMDL_GetGridPosZ( fmmdl );
	int ngx = MMDL_GetGridPosX( work->oyaobj );
	int ngz = MMDL_GetGridPosZ( work->oyaobj );
	int ogx = MMDL_GetOldGridPosX( work->oyaobj );
	int ogz = MMDL_GetOldGridPosZ( work->oyaobj );
	int dir;
	
	if( gx == ngx && gz == ngz ){
		return( FALSE );
	}
	
	dir = MMDL_TOOL_GetRangeDir( gx, gz, ogx, ogz );
	gx += MMDL_TOOL_GetDirAddValueGridX( dir );
	gz += MMDL_TOOL_GetDirAddValueGridZ( dir );
		
	if( gx != ngx || gz != ngz ){
		u32 code = AC_WALK_U_8F;
		code = MMDL_ChangeDirAcmdCode( dir, code );
		MMDL_SetLocalAcmd( fmmdl, code );
		return( TRUE );
	}
	
	return( FALSE );
}

#if 0
static int PairTr_OyaCheckAcmdSet( MMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	int gx = MMDL_GetGridPosX( fmmdl );
	int gz = MMDL_GetGridPosZ( fmmdl );
	int ngx = MMDL_GetGridPosX( work->oyaobj );
	int ngz = MMDL_GetGridPosZ( work->oyaobj );
	int ogx = MMDL_GetOldGridPosX( work->oyaobj );
	int ogz = MMDL_GetOldGridPosZ( work->oyaobj );
	int sx,sz;
	
	if( gx == ngx && gz == ngz ){
		return( FALSE );
	}
	
	sx = gx - ogx;
	if( sx < 0 ){ sx = -sx; }
	sz = gz - ogz;
	if( sz < 0 ){ sz = -sz; }
	
	if( sx || sz ){
		int dir = MMDL_TOOL_GetRangeDir( gx, gz, ogx, ogz );
		gx += MMDL_TOOL_GetDirAddValueGridX( dir );
		gz += MMDL_TOOL_GetDirAddValueGridZ( dir );
		
		if( gx != ngx && gz != ngz ){
			u32 code = AC_WALK_U_8F;
			code = MMDL_ChangeDirAcmdCode( dir, code );
			MMDL_SetLocalAcmd( fmmdl, code );
			return( TRUE );
		}
	}
	
	return( FALSE );
}
#endif

//======================================================================
//	MV_HIDE_SNOW�@�B�ꖪ�@��
//======================================================================
//--------------------------------------------------------------
/**
 * MV_HIDE ������
 * @param	fmmdl	MMDL *
 * @param	type	HIDETYPE
 * @retval	nothing
 */
//--------------------------------------------------------------
#ifndef MMDL_PL_NULL
static void MMdl_MoveHide_Init( MMDL * fmmdl, HIDETYPE type )
{
	MV_HIDE_WORK *work = MMDL_InitMoveProcWork( fmmdl, MV_HIDE_WORK_SIZE );
	work->hide_type = type;
	MMDL_SetDrawStatus( fmmdl, DRAW_STA_STOP );
	MMDL_OffMoveBitMove( fmmdl );
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_SHADOW_VANISH );
	
	{															//�������Ƃ�
		VecFx32 offs = { 0, NUM_FX32(-32), 0 };
		MMDL_SetVectorDrawOffsetPos( fmmdl, &offs );
	}
}
#endif

//--------------------------------------------------------------
/**
 * MV_HIDE_SNOW ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHideSnow_Init( MMDL * fmmdl )
{
#ifndef MMDL_PL_NULL
	MMdl_MoveHide_Init( fmmdl, HIDE_SNOW );
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE_SAND ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHideSand_Init( MMDL * fmmdl )
{
#ifndef MMDL_PL_NULL
	MMdl_MoveHide_Init( fmmdl, HIDE_SAND );
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE_GRND ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHideGround_Init( MMDL * fmmdl )
{
#ifndef MMDL_PL_NULL
	MMdl_MoveHide_Init( fmmdl, HIDE_GROUND );
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE_KUSA ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHideKusa_Init( MMDL * fmmdl )
{
#ifndef MMDL_PL_NULL
	MMdl_MoveHide_Init( fmmdl, HIDE_GRASS );
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE ����
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHide_Move( MMDL * fmmdl )
{
	MV_HIDE_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	while( DATA_HideMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_HIDE �폜
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHide_Delete( MMDL * fmmdl )
{
#ifndef MMDL_PL_NULL
	EOA_PTR eoa = MMDL_MoveHideEoaPtrGet( fmmdl );
	if( eoa != NULL ){ FE_EoaDelete( eoa ); }
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE ���A
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHide_Return( MMDL * fmmdl )
{
#ifndef MMDL_PL_NULL
	MV_HIDE_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	
	work->seq_no = 0;
	
	MMDL_MoveHideEoaPtrSet( fmmdl, NULL );
	
	if( work->pulloff_flag == FALSE ){
		VecFx32 offs = { 0, NUM_FX32(-32), 0 };
		MMDL_SetVectorDrawOffsetPos( fmmdl, &offs );
		//add pl 
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_SHADOW_VANISH );
	}
#endif
}

//======================================================================
//	MV_HIDE ����
//======================================================================
//--------------------------------------------------------------
/**
 * hide 0
 * @param	fmmdl	MMDL *
 * @param	work	MV_HIDE_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int HideMove_Init( MMDL * fmmdl, MV_HIDE_WORK *work )
{
#ifndef MMDL_PL_NULL
	if( work->pulloff_flag == FALSE ){
		EOA_PTR eoa = FE_fmmdlHide_Add( fmmdl, work->hide_type );
		MMDL_MoveHideEoaPtrSet( fmmdl, eoa );
	}
	
	MMDL_OffMoveBitMove( fmmdl );
	MMDL_OffMoveBitMoveEnd( fmmdl );
	
	work->seq_no++;
#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * hide 1
 * @param	fmmdl	MMDL *
 * @param	work	MV_HIDE_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int HideMove_Move( MMDL * fmmdl, MV_HIDE_WORK *work )
{
#ifndef MMDL_PL_NULL
	if( work->pulloff_flag == FALSE ){
		EOA_PTR eoa = MMDL_MoveHideEoaPtrGet( fmmdl );
	
		if( eoa == NULL ){ 
			if( MMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
				eoa = FE_fmmdlHide_Add( fmmdl, work->hide_type );
				MMDL_MoveHideEoaPtrSet( fmmdl, eoa );
			}
		}
		
		//add pl ��ɉe������
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_SHADOW_VANISH );
	}
#endif
	return( FALSE );
}

//--------------------------------------------------------------
///	MV_HIDE ����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_HideMoveTbl[])( MMDL * fmmdl, MV_HIDE_WORK *work ) =
{
	HideMove_Init,
	HideMove_Move,
};

//======================================================================
//	MV_HIDE �p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * MV_HIDE �B�ꖪEOA_PTR�Z�b�g
 * @param	fmmdl	MMDL *
 * @param	eoa		EOA_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
#ifndef MMDL_PL_NULL
void MMDL_MoveHideEoaPtrSet( MMDL * fmmdl, EOA_PTR eoa )
{
	MV_HIDE_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	work->eoa_hide = eoa;
}
#endif

//--------------------------------------------------------------
/**
 * MV_HIDE �B�ꖪEOA_PTR�擾
 * @param	fmmdl	MMDL *
 * @param	eoa		EOA_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
#ifndef MMDL_PL_NULL
EOA_PTR MMDL_MoveHideEoaPtrGet( MMDL * fmmdl )
{
	MV_HIDE_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	return( work->eoa_hide );
}
#endif

//--------------------------------------------------------------
/**
 * MV_HIDE �B�ꖪ�A�E������Ԃ�
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHidePullOffFlagSet( MMDL * fmmdl )
{
	MV_HIDE_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	work->pulloff_flag = TRUE;
}

//======================================================================
//	MV_COPYU
//======================================================================
static int (* const DATA_CopyMoveTbl[SEQNO_COPYMOVE_MAX])( MMDL *, MV_COPY_WORK * );

//--------------------------------------------------------------
/**
 * ���̂܂ˁ@���[�N������
 * @param	fmmdl	MMDL *
 * @param	dir		��������
 * @param	lgrass	������������̂܂� FALSE=Not
 * @retval
 */
//--------------------------------------------------------------
static void MoveCopy_WorkInit( MMDL * fmmdl, int dir, u32 lgrass )
{
	MV_COPY_WORK *work = MMDL_InitMoveProcWork( fmmdl, MV_COPY_WORK_SIZE );
	work->dir_jiki = DIR_NOT;
	work->lgrass_on = lgrass;
	MMDL_SetDirDisp( fmmdl, DIR_UP );
}

//--------------------------------------------------------------
/**
 * MV_COPYU ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyU_Init( MMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_UP, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYD ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyD_Init( MMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_DOWN, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYL ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyL_Init( MMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_LEFT, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYR ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyR_Init( MMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_RIGHT, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSU ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyLGrassU_Init( MMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_UP, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSD ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyLGrassD_Init( MMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_DOWN, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSL ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyLGrassL_Init( MMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_LEFT, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSR ������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyLGrassR_Init( MMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_RIGHT, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPY ����
 * @param
 * @retval
 */
//--------------------------------------------------------------
void MMDL_MoveCopy_Move( MMDL * fmmdl )
{
	MV_COPY_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	while( DATA_CopyMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_COPY ����@��������@����0
 * @param	fmmdl	MMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_FirstInit( MMDL * fmmdl, MV_COPY_WORK *work )
{
	int ret = MMDL_GetDirDisp( fmmdl );
	ret = MMDL_ChangeDirAcmdCode( ret, AC_DIR_U );
	MMDL_SetLocalAcmd( fmmdl, ret );
	MMDL_OffMoveBitMove( fmmdl );
	MMDL_OffMoveBitMoveEnd( fmmdl );
	work->seq_no = SEQNO_COPYMOVE_FIRST_WAIT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYU ����@��������@����1
 * @param	fmmdl	MMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_FirstWait( MMDL * fmmdl, MV_COPY_WORK *work )
{
	if( MMDL_ActionLocalAcmd(fmmdl) == TRUE ){
		work->seq_no = SEQNO_COPYMOVE_INIT;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPY ���� 0
 * @param	fmmdl	MMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_Init( MMDL * fmmdl, MV_COPY_WORK *work )
{
#ifndef MMDL_PL_NULL
	if( work->dir_jiki == DIR_NOT ){
		FIELDSYS_WORK *fsys = MMDL_FieldSysWorkGet( fmmdl );
		work->dir_jiki = Player_DirGet( fsys->player );
	}
	
	MMDL_OffMoveBitMove( fmmdl );
	MMDL_OffMoveBitMoveEnd( fmmdl );
	work->seq_no = SEQNO_COPYMOVE_CMD_SET;
#endif
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���̂܂ˑ��ړ��`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	dir		�ړ�����
 * @retval	u32		hit bit	
 */
//--------------------------------------------------------------
static u32 CopyMove_LongGrassHitCheck( MMDL * fmmdl, int dir )
{
#ifndef MMDL_PL_NULL
	u32 ret = MMDL_GetMapDirAttr( fmmdl, dir );
	
	if( MATR_IsLongGrass(ret) == FALSE ){
		ret = MMDL_MOVEHITBIT_ATTR;
	}
	
	ret |= MMDL_HitCheckMoveDir( fmmdl, dir );
	return( ret );
#else
	return( 0 );
#endif
}

//--------------------------------------------------------------
/**
 * �R�s�[���� �ړ��Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void CopyMove_MoveSet(
	MMDL * fmmdl, int dir, int ac, u32 lgrass_on )
{
	u32 ret;
	
	if( lgrass_on == FALSE ){
		ret = MMDL_HitCheckMoveDir( fmmdl, dir );
	}else{
		ret = CopyMove_LongGrassHitCheck( fmmdl, dir );
	}
	
	if( ret != MMDL_MOVEHITBIT_NON ){
		ac = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
	}else{
		ac = MMDL_ChangeDirAcmdCode( dir, ac );
		MMDL_OnMoveBitMove( fmmdl );
	}
	
	MMDL_SetLocalAcmd( fmmdl, ac );
}

//--------------------------------------------------------------
/**
 * MV_COPY ���� 1
 * @param	fmmdl	MMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_CmdSet( MMDL * fmmdl, MV_COPY_WORK *work )
{
#ifndef MMDL_PL_NULL
	int ret;
	FIELDSYS_WORK *fsys = MMDL_FieldSysWorkGet( fmmdl );
	int dir = Player_DirGet( fsys->player );
	u32 type = Player_AcmdTypeGet( fsys->player );
	
	switch( type ){
	case HEROACTYPE_NON:
	case HEROACTYPE_STOP:
		ret = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
		MMDL_SetLocalAcmd( fmmdl, ret );
		break;
	case HEROACTYPE_WALK_32F:
		CopyMove_MoveSet( fmmdl, dir, AC_WALK_U_32F, work->lgrass_on );
		break;
	case HEROACTYPE_WALK_16F:
		CopyMove_MoveSet( fmmdl, dir, AC_WALK_U_16F, work->lgrass_on );
		break;
	case HEROACTYPE_WALK_8F:
		CopyMove_MoveSet( fmmdl, dir, AC_WALK_U_8F, work->lgrass_on );
		break;
	case HEROACTYPE_WALK_4F:
		CopyMove_MoveSet( fmmdl, dir, AC_WALK_U_4F, work->lgrass_on );
		break;
	case HEROACTYPE_WALK_2F:
		CopyMove_MoveSet( fmmdl, dir, AC_WALK_U_2F, work->lgrass_on );
		break;
	}
#endif
	work->seq_no = SEQNO_COPYMOVE_CMD_WAIT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPY ���� 2
 * @param	fmmdl	MMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_MoveWait( MMDL * fmmdl, MV_COPY_WORK *work )
{
	if( MMDL_ActionLocalAcmd(fmmdl) == FALSE ){
		return( FALSE );
	}
	
	MMDL_OffMoveBitMove( fmmdl );
	MMDL_OffMoveBitMoveEnd( fmmdl );
	work->seq_no = SEQNO_COPYMOVE_INIT;
	return( FALSE );
}

//--------------------------------------------------------------
///	�R�s�[����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_CopyMoveTbl[SEQNO_COPYMOVE_MAX])( MMDL *, MV_COPY_WORK * ) =
{
	CopyMove_FirstInit,
	CopyMove_FirstWait,
	CopyMove_Init,
	CopyMove_CmdSet,
	CopyMove_MoveWait,
};

//======================================================================
//	�ǉ����ړ�
//======================================================================
static int AlongWall_WallMove(MMDL * fmmdl,MV_ALONGW_WORK *work,int ac);
static int (* const DATA_AlongMoveTbl[3])(MMDL *, MV_ALONGW_WORK *);

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@���[�N������
 * @param	fmmdl		MMDL *
 * @param	dir_h_init	����������
 * @param	dir_h		������
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AlongWallWorkInit( MMDL * fmmdl, ALONG_DIR dir_h_init, ALONG_DIR dir_h )
{
	MV_ALONGW_WORK *work;
	work = MMDL_InitMoveProcWork( fmmdl, MV_ALONGW_WORK_SIZE );
	work->dir_hand_init = dir_h_init;
	work->dir_hand = dir_h;
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@����@������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWallL_Init( MMDL * fmmdl )
{
	AlongWallWorkInit( fmmdl, ALONG_L, ALONG_L );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@�E��@������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWallR_Init( MMDL * fmmdl )
{
	AlongWallWorkInit( fmmdl, ALONG_R, ALONG_R );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@���荶�@������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWallLRL_Init( MMDL * fmmdl )
{
	AlongWallWorkInit( fmmdl, ALONG_LR, ALONG_L );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@����E�@������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWallLRR_Init( MMDL * fmmdl )
{
	AlongWallWorkInit( fmmdl, ALONG_LR, ALONG_R );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@����
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWall_Move( MMDL * fmmdl )
{
	MV_ALONGW_WORK *work = MMDL_GetMoveProcWork( fmmdl );
	while( DATA_AlongMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * ���� ������
 * @param	fmmdl	MMDL *
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AlongWallMove_Init( MMDL * fmmdl, MV_ALONGW_WORK *work )
{
	MMDL_OffMoveBitMove( fmmdl );
	work->seq_no++;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���� ������
 * @param	fmmdl	MMDL *
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AlongWallMove_CmdSet( MMDL * fmmdl, MV_ALONGW_WORK *work )
{
	AlongWall_WallMove( fmmdl, work, AC_WALK_U_8F );
	work->seq_no++;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���� �R�}���h�I���҂�
 * @param	fmmdl	MMDL *
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AlongWallMove_CmdWait( MMDL * fmmdl, MV_ALONGW_WORK *work )
{
	if( MMDL_ActionLocalAcmd(fmmdl) == TRUE ){
		work->seq_no = 0;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	�Ǔ`���ړ��@����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_AlongMoveTbl[3])( MMDL *, MV_ALONGW_WORK * ) =
{
	AlongWallMove_Init,
	AlongWallMove_CmdSet,
	AlongWallMove_CmdWait,
};

//--------------------------------------------------------------
///	��̈ʒu
//--------------------------------------------------------------
static const int DATA_DirHandPosX[DIR_MAX4][2] = 
{
	{ -1, 1 },	//up,left,right
	{ 1, -1 },	//down,left,right
	{ 0, 0 },	//left,left,right
	{ 0, 0 },	//right,left,right
};

static const int DATA_DirHandPosZ[DIR_MAX4][2] =
{
	{ 0, 0 },	//up,left,right
	{ 0, 0 },	//down,left,right
	{ 1, -1 },	//left,left,right
	{ -1, 1 },	//right,left,right
};

//--------------------------------------------------------------
///	���������ۂ̔���ʒu
//--------------------------------------------------------------
static const int DATA_DirHandLostPosX[DIR_MAX4][2] =
{
	{ -1, 1 },	//up,left,right
	{ 1, -1 },	//down,left,right
	{ 1, 1  },	//left,left,right
	{ -1, -1 },	//right,left,right
};

static const int DATA_DirHandLostPosZ[DIR_MAX4][2] =
{
	{ 1, 1 },	//up,left,right
	{ -1, -1 },	//down,left,right
	{ 1, -1  },	//left,left,right
	{ -1, 1 },	//right,left,right
};

//--------------------------------------------------------------
///	���������ۂ̕����؂�ւ�
//--------------------------------------------------------------
static const int DATA_DirHandLostDir[DIR_MAX4][2] =
{
	{ DIR_LEFT, DIR_RIGHT },	//up,left,right
	{ DIR_RIGHT, DIR_LEFT },	//down,left,right
	{ DIR_DOWN, DIR_UP  },	//left,left,right
	{ DIR_UP, DIR_DOWN },	//right,left,right
};

//--------------------------------------------------------------
///	�ǏՓˎ��̕����؂�ւ�
//--------------------------------------------------------------
static const int DATA_DirHandAttrHitDir[DIR_MAX4][2] =
{
	{ DIR_RIGHT, DIR_LEFT },	//up,left,right
	{ DIR_LEFT, DIR_RIGHT },	//down,left,right
	{ DIR_UP, DIR_DOWN  },	//left,left,right
	{ DIR_DOWN, DIR_UP },	//right,left,right
};

//--------------------------------------------------------------
///	��̂ނ����]
//--------------------------------------------------------------
static const ALONG_DIR DATA_DirHandFlip[ALONG_LR] =
{ ALONG_R, ALONG_L };
	
//--------------------------------------------------------------
/**
 * �ǉ����@�w������̕ǂ��E��
 * @param	fsys	FIELDSYS_WORK
 * @param	gx		�O���b�hX
 * @param	gz		�O���b�hZ
 * @param	dir		����
 * @param	hdir	������
 * @retval	BOOL	TRUE=�ǂ���
 */
//--------------------------------------------------------------
#ifndef MMDL_PL_NULL
static BOOL AlongWall_HandHitGet(
	FIELDSYS_WORK *fsys, int gx, int gz, int dir, ALONG_DIR hdir )
{
	BOOL hit;
	gx += DATA_DirHandPosX[dir][hdir];
	gz += DATA_DirHandPosZ[dir][hdir];
	hit = GetHitAttr( fsys, gx, gz );
	return( hit );
}
#endif

//--------------------------------------------------------------
/**
 * �ǉ����@�Ǐ������̕ǌ���
 * @param	fsys	FIELDSYS_WORK
 * @param	gx		�O���b�hX
 * @param	gz		�O���b�hZ
 * @param	dir		����
 * @param	hdir	������
 * @retval	BOOL	TRUE=�ǂ���
 */
//--------------------------------------------------------------
#ifndef MMDL_PL_NULL
static BOOL AlongWall_HandLostHitGet(
		FIELDSYS_WORK *fsys, int gx, int gz, int dir, ALONG_DIR hdir )
{
	BOOL hit;
	gx += DATA_DirHandLostPosX[dir][hdir];
	gz += DATA_DirHandLostPosZ[dir][hdir];
	hit = GetHitAttr( fsys, gx, gz );
	return( hit );
}
#endif

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@��̈ʒu�ɕǂ͂��邩
 * @param	fmmdl	MMDL *
 * @param	dir_hand	ALONG_DIR
 * @retval	int		TRUE=�ǃA��
 */
//--------------------------------------------------------------
static int AlongWall_HandWallCheck(
	MMDL * fmmdl, int dir, ALONG_DIR dir_hand )
{
#ifndef MMDL_PL_NULL
	FIELDSYS_WORK *fsys = MMDL_FieldSysWorkGet( fmmdl );
	int gx = MMDL_GetGridPosX( fmmdl );
	int gz = MMDL_GetGridPosZ( fmmdl );
	BOOL hit = AlongWall_HandHitGet( fsys, gx, gz, dir, dir_hand );
	return( hit );
#else
	return( FALSE );
#endif
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@�ǎ�T��
 * @param	fmmdl	MMDL *
 * @param	dir_hand	ALONG_DIR
 * @retval	int		TRUE=�ǃA��
 */
//--------------------------------------------------------------
static int AlongWall_HandLostWallCheck(
	MMDL * fmmdl, int dir, ALONG_DIR dir_hand )
{
#ifndef MMDL_PL_NULL
	FIELDSYS_WORK *fsys = MMDL_FieldSysWorkGet( fmmdl );
	int gx = MMDL_GetGridPosX( fmmdl );
	int gz = MMDL_GetGridPosZ( fmmdl );
	BOOL hit = AlongWall_HandLostHitGet( fsys, gx, gz, dir, dir_hand );
	return( hit );
#else
	return( FALSE );
#endif
}

//--------------------------------------------------------------
/**
 * �ǈړ��`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	dir_hand	ALONG_DIR
 * @retval	int		�ړ�����ׂ�����
 */
//--------------------------------------------------------------
static int AlongWall_MoveHitCheck(
	MMDL * fmmdl, int dir_move, ALONG_DIR dir_hand )
{
#ifndef MMDL_PL_NULL
	if( AlongWall_HandWallCheck(
		fmmdl,dir_move,dir_hand) == FALSE ){	//�ǂ�����
		if( AlongWall_HandLostWallCheck(fmmdl,dir_move,dir_hand) == FALSE ){
			return( DIR_NOT );					//��T��ł��ǂ�����
		}
		
		//�ǔ��� �����؂�ւ�
		dir_move = DATA_DirHandLostDir[dir_move][dir_hand];
	}
	
	return( dir_move );
#else
	return( 0 );
#endif
}

//--------------------------------------------------------------
/**
 * �Ǔ`���ړ��`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	dir_move	�ړ����� DIR_NOT=�ړ��s��
 * @param	dir_hand	�Ǒ��̎�̌���
 * @retval	u32		�q�b�g�r�b�g
 */
//--------------------------------------------------------------
static u32 AlongWall_WallMoveCheck(
	MMDL * fmmdl, int *dir_move, ALONG_DIR dir_hand )
{
	u32 ret;
	
	*dir_move = AlongWall_MoveHitCheck( fmmdl, *dir_move, dir_hand );
	
	if( *dir_move != DIR_NOT ){
		ret = MMDL_HitCheckMoveDir( fmmdl, *dir_move );
		return( ret );
	}
	
	return( 0 );
}

//--------------------------------------------------------------
/**
 * �Ǔ`���ړ�
 * @param	fmmdl	MMDL *
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=�ړ�����
 */
//--------------------------------------------------------------
static int AlongWall_WallMove(
	MMDL * fmmdl, MV_ALONGW_WORK *work, int ac )
{
	u32 ret;
	int dir_hand = work->dir_hand;
	int dir_move = MMDL_GetDirDisp( fmmdl );
	
	ret = AlongWall_WallMoveCheck( fmmdl, &dir_move, dir_hand );
	
	if( dir_move == DIR_NOT ){					//�ǂ��Ȃ�
		dir_move = MMDL_GetDirDisp( fmmdl );
		ac = MMDL_ChangeDirAcmdCode( dir_move, AC_STAY_WALK_U_16F );
		MMDL_SetLocalAcmd( fmmdl, ac );
		return( FALSE );
	}
	
	if( ret == MMDL_MOVEHITBIT_NON ){		//�ړ��\
		ac = MMDL_ChangeDirAcmdCode( dir_move, ac );
		MMDL_OnMoveBitMove( fmmdl );
		MMDL_SetLocalAcmd( fmmdl, ac );
		return( TRUE );
	}
	
	//�ړ������q�b�g&���藘���@���]�����݂�
	if( (ret & MMDL_MOVEHITBIT_LIM) && work->dir_hand_init == ALONG_LR ){
		dir_move = MMDL_TOOL_FlipDir( MMDL_GetDirDisp(fmmdl) );
		dir_hand = DATA_DirHandFlip[dir_hand];
		work->dir_hand = dir_hand;
		
		ret = AlongWall_WallMoveCheck( fmmdl, &dir_move, dir_hand );
		
		if( dir_move == DIR_NOT ){					//�ǂ��Ȃ�
			dir_move = MMDL_GetDirDisp( fmmdl );
			ac = MMDL_ChangeDirAcmdCode( dir_move, AC_STAY_WALK_U_16F );
			MMDL_SetLocalAcmd( fmmdl, ac );
			return( FALSE );
		}
		
		if( ret == MMDL_MOVEHITBIT_NON ){		//�ړ��\
			ac = MMDL_ChangeDirAcmdCode( dir_move, ac );
			MMDL_OnMoveBitMove( fmmdl );
			MMDL_SetLocalAcmd( fmmdl, ac );
			return( TRUE );
		}
	}
	
	//�ǃq�b�g�@�ړ������ύX
	if( (ret & MMDL_MOVEHITBIT_ATTR) ){
		dir_move = DATA_DirHandAttrHitDir[dir_move][dir_hand];
		
		ret = AlongWall_WallMoveCheck( fmmdl, &dir_move, dir_hand );
		
		if( dir_move == DIR_NOT ){					//�ǂ�����
			dir_move = MMDL_GetDirDisp( fmmdl );
			ac = MMDL_ChangeDirAcmdCode( dir_move, AC_STAY_WALK_U_16F );
			MMDL_SetLocalAcmd( fmmdl, ac );
			return( FALSE );
		}
		
		if( ret == MMDL_MOVEHITBIT_NON ){		//�ړ��\
			ac = MMDL_ChangeDirAcmdCode( dir_move, ac );
			MMDL_OnMoveBitMove( fmmdl );
			MMDL_SetLocalAcmd( fmmdl, ac );
			return( TRUE );
		}
	}
	
	//�ړ��s��
	dir_move = MMDL_GetDirDisp( fmmdl );	//������߂�
	ac = MMDL_ChangeDirAcmdCode( dir_move, AC_STAY_WALK_U_16F );
	MMDL_SetLocalAcmd( fmmdl, ac );
	return( FALSE );
}
