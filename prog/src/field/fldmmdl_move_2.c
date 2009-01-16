//******************************************************************************
/**
 *
 * @file	fieldobj_move_2.c
 * @brief	�t�B�[���h���샂�f���@��{����n����2
 * @author	kagaya
 * @data	05.07.21
 *
 */
//******************************************************************************
#include "fldmmdl.h"

//==============================================================================
//	define
//==============================================================================
///�g���[�i�[�y�A�@�e�ړ���~
#define PAIR_TR_OYA_STA_BIT_STOP \
	(FLDMMDL_STABIT_ATTR_GET_ERROR		| \
	 FLDMMDL_STABIT_HEIGHT_GET_ERROR	| \
	 FLDMMDL_STABIT_PAUSE_MOVE)

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

//==============================================================================
//	typedef
//==============================================================================
//--------------------------------------------------------------
//	MV_PAIR_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;												///<����ԍ�
	u8 jiki_init;											///<���@��񏉊�������
	s16 jiki_gx;											///<���@�O���b�h���WX
	s16 jiki_gz;											///<���@�O���b�h���WZ
	u16 jiki_ac;											///<���@�A�j���[�V�����R�[�h
}MV_PAIR_WORK;

#define MV_PAIR_WORK_SIZE (sizeof(MV_PAIR_WORK))

//--------------------------------------------------------------
//	MV_TR_PAIR_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	u8 seq_no;											///<����ԍ�
	u8 oya_init;										///<�e��񏉊�������
	s16 oya_gx;											///<�e�O���b�h���WX
	s16 oya_gz;											///<�e�O���b�h���WZ
	u16 oya_ac;											///<���@�A�j���[�V�����R�[�h
	FLDMMDL * oyaobj;								///<�e�ƂȂ�OBJ
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
#if 0
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

//==============================================================================
//	�v���g�^�C�v
//==============================================================================
int (* const DATA_PairMoveTbl[])( FLDMMDL * fmmdl, MV_PAIR_WORK *work );

static int Pair_WorkSetJikiSearch( FLDMMDL * fmmdl, MV_PAIR_WORK *work );
static void Pair_WorkInit( FLDMMDL * fmmdl, MV_PAIR_WORK *work );
static int Pair_JikiPosUpdateCheck( FLDMMDL * fmmdl, MV_PAIR_WORK *work );
static void Pair_JikiPosSet( FLDMMDL * fmmdl, MV_PAIR_WORK *work );
static u32 Pair_JikiAcmdCodeGet( FLDMMDL * fmmdl );
static int Pair_JikiCheckAcmdSet( FLDMMDL * fmmdl );

int (* const DATA_PairTrMoveTbl[])( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work );

static int PairTr_WorkSetOyaSearch( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work );
static void PairTr_WorkInit( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work, FLDMMDL * oyaobj);
static int PairTr_OyaPosUpdateCheck( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work );
static void PairTr_OyaPosSet( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work );
static int PairTr_OyaCheckAcmdSet( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work );

int (* const DATA_HideMoveTbl[])( FLDMMDL * fmmdl, MV_HIDE_WORK *work );

//==============================================================================
//	MV_PAIR	���@�A�����
//==============================================================================
//--------------------------------------------------------------
/**
 * MV_PAIR�@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MovePair_Init( FLDMMDL * fmmdl )
{
	MV_PAIR_WORK *work = FLDMMDL_MoveProcWorkInit( fmmdl, MV_PAIR_WORK_SIZE );
	Pair_WorkSetJikiSearch( fmmdl, work );
	FLDMMDL_DrawStatusSet( fmmdl, DRAW_STA_STOP );
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	FLDMMDL_StatusBitSet_FellowHit( fmmdl, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_PAIR�@����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MovePair_Move( FLDMMDL * fmmdl )
{
	MV_PAIR_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	
	if( Pair_WorkSetJikiSearch(fmmdl,work) == FALSE ){
		return;
	}
	
	FLDMMDL_StatusBitSet_FellowHit( fmmdl, FALSE );
	
	while( DATA_PairMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_PAIR �폜
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MovePair_Delete( FLDMMDL * fmmdl )
{
}

//==============================================================================
//	MV_PAIR ����
//==============================================================================
//--------------------------------------------------------------
/**
 * Pair 0
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairMove_Init( FLDMMDL * fmmdl, MV_PAIR_WORK *work )
{
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	FLDMMDL_StatusBitOFF_MoveEnd( fmmdl );
		
	if( Pair_JikiPosUpdateCheck(fmmdl,work) == TRUE ){
		Pair_JikiPosSet( fmmdl, work );
		
		if( Pair_JikiCheckAcmdSet(fmmdl) == TRUE ){
			FLDMMDL_StatusBitON_Move( fmmdl );
			work->seq_no++;
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * Pair 1
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairMove_Move( FLDMMDL * fmmdl, MV_PAIR_WORK *work )
{
	if( FLDMMDL_CmdAction(fmmdl) == TRUE ){
		FLDMMDL_StatusBitOFF_Move( fmmdl );
		work->seq_no = 0;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
///	�ꂠ�邫����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_PairMoveTbl[])( FLDMMDL * fmmdl, MV_PAIR_WORK *work ) =
{
	PairMove_Init,
	PairMove_Move,
};

//==============================================================================
//	MV_PAIR�@�p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * ���@�����݂��邩�`�F�b�N�@���݂���̂ł���Ώ�񏉊��� 
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		FALSE=���݂��Ă��Ȃ�
 */
//--------------------------------------------------------------
static int Pair_WorkSetJikiSearch( FLDMMDL * fmmdl, MV_PAIR_WORK *work )
{
#if 0
	const FLDMMDLSYS *fos = FLDMMDL_FieldOBJSysGet( fmmdl );
	FLDMMDL * jikiobj = Player_FieldOBJSearch( fos );
	
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
 * @param	fmmdl	FLDMMDL *	
 * @param	work	MV_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Pair_WorkInit( FLDMMDL * fmmdl, MV_PAIR_WORK *work )
{
#if 0
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
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
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�X�V����
 */
//--------------------------------------------------------------
static int Pair_JikiPosUpdateCheck( FLDMMDL * fmmdl, MV_PAIR_WORK *work )
{
#if 0
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
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
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Pair_JikiPosSet( FLDMMDL * fmmdl, MV_PAIR_WORK *work )
{
#if 0
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	
	work->jiki_gx = Player_NowGPosXGet( jiki );
	work->jiki_gz = Player_NowGPosZGet( jiki );
#endif
}

//--------------------------------------------------------------
/**
 * ���@�A�j���R�[�h�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		�A�j���[�V�����R�[�h
 */
//--------------------------------------------------------------
static u32 Pair_JikiAcmdCodeGet( FLDMMDL * fmmdl )
{
#if 0
	u32 code;
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
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
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�Z�b�g�ł��� FALSE=�d�Ȃ�ɂ��Z�b�g�ł��Ȃ�
 */
//--------------------------------------------------------------
static int Pair_JikiCheckAcmdSet( FLDMMDL * fmmdl )
{
#if 0
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
	PLAYER_STATE_PTR jiki = Player_FieldSysWorkPlayerGet( fsys );
	int gx = FLDMMDL_NowPosGX_Get( fmmdl );
	int gz = FLDMMDL_NowPosGZ_Get( fmmdl );
	int jx = Player_OldGPosXGet( jiki );
	int jz = Player_OldGPosZGet( jiki );
	
	if( gx != jx || gz != jz ){
		u32 code = Pair_JikiAcmdCodeGet( fmmdl );
		int dir = FieldOBJTool_DirRange( gx, gz, jx, jz );
		code = FLDMMDL_AcmdCodeDirChange( dir, code );
		FLDMMDL_CmdSet( fmmdl, code );
		
		return( TRUE );
	}
#else	
	return( FALSE );
#endif
}

//==============================================================================
//	MV_TR_PAIR �g���[�i�[�A�����
//==============================================================================
//--------------------------------------------------------------
/**
 * MV_TR_PAIR�@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MovePairTr_Init( FLDMMDL * fmmdl )
{
	MV_TR_PAIR_WORK *work = FLDMMDL_MoveProcWorkInit( fmmdl, MV_TR_PAIR_WORK_SIZE );
	PairTr_WorkSetOyaSearch( fmmdl, work );
	FLDMMDL_DrawStatusSet( fmmdl, DRAW_STA_STOP );
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	
//	FLDMMDL_StatusBitSet_FellowHit( fmmdl, FALSE );
	work->oya_init = FALSE;
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR�@����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MovePairTr_Move( FLDMMDL * fmmdl )
{
	MV_TR_PAIR_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	
	if( PairTr_WorkSetOyaSearch(fmmdl,work) == FALSE ){
		return;
	}
	
//	FLDMMDL_StatusBitSet_FellowHit( fmmdl, FALSE );
	while( DATA_PairTrMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR �폜
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MovePairTr_Delete( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR�@���A
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MovePairTr_Return( FLDMMDL * fmmdl )
{
	MV_TR_PAIR_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	work->oya_init = 0;
//	PairTr_WorkSetOyaSearch( fmmdl, work );
}

//==============================================================================
//	MV_TR_PAIR ����
//==============================================================================
//--------------------------------------------------------------
/**
 * Pair 0
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_TR_PAIR_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairTrMove_Init( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	FLDMMDL_StatusBitOFF_MoveEnd( fmmdl );
		
	if( PairTr_OyaPosUpdateCheck(fmmdl,work) == TRUE ){
		if( PairTr_OyaCheckAcmdSet(fmmdl,work) == TRUE ){
//			PairTr_OyaPosSet( fmmdl, work );
			FLDMMDL_StatusBitON_Move( fmmdl );
			work->seq_no++;
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * Pair 1
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairTrMove_Move( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	if( FLDMMDL_CmdAction(fmmdl) == FALSE ){
		return( FALSE );
	}
	
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	work->seq_no = 0;
	return( FALSE );
}

//--------------------------------------------------------------
///	�ꂠ�邫����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_PairTrMoveTbl[])( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work ) =
{
	PairTrMove_Init,
	PairTrMove_Move,
};

//==============================================================================
//	MV_TR_PAIR�@�p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * �Ώۂ��y�A���삩�ǂ����B�y�A����ł���Α�����T��
 * @param	fmmdl	FLDMMDL *
 * @retval	FLDMMDL *�@������FLDMMDL *�ANULL=�y�A����ł͂Ȃ�
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDL_MovePairSearch( FLDMMDL * fmmdl )
{
#if 0
	int no = 0;
	int type = FLDMMDL_EventTypeGet( fmmdl );
	int zone_id = FLDMMDL_ZoneIDGet( fmmdl );
	u32 trid = EvTrainerfmmdlTrainerIDGet( fmmdl );
	const FLDMMDLSYS *fos = FLDMMDL_FieldOBJSysGet( fmmdl );
	FLDMMDL * pair;
	
	switch( type ){
	case EV_TYPE_TRAINER:
	case EV_TYPE_TRAINER_EYEALL:
	case EV_TYPE_ITEM:
	case EV_TYPE_TRAINER_KYORO:
	case EV_TYPE_TRAINER_SPIN_STOP_L:
	case EV_TYPE_TRAINER_SPIN_STOP_R:
	case EV_TYPE_TRAINER_SPIN_MOVE_L:
	case EV_TYPE_TRAINER_SPIN_MOVE_R:
		while( FLDMMDLSYS_FieldOBJSearch(fos,&pair,&no,FLDMMDL_STABIT_USE) == TRUE ){
			if( fmmdl != pair && FLDMMDL_ZoneIDGet(pair) == zone_id ){
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
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_TR_PAIR_WORK
 * @retval	int		FALSE=���݂��Ă��Ȃ�
 */
//--------------------------------------------------------------
static int PairTr_WorkSetOyaSearch( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
#if 0
	int no,zone;
	u32 trid;
	FLDMMDL * oyaobj;
	const FLDMMDLSYS *fos = FLDMMDL_FieldOBJSysGet( fmmdl );
	
	no = 0;
	zone = FLDMMDL_ZoneIDGet( fmmdl );
	trid = EvTrainerfmmdlTrainerIDGet( fmmdl );
	
	while( FLDMMDLSYS_FieldOBJSearch(fos,&oyaobj,&no,FLDMMDL_STABIT_USE) == TRUE ){
		if( fmmdl != oyaobj &&
			FLDMMDL_ZoneIDGet(oyaobj) == zone &&
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
 * @param	fmmdl	FLDMMDL *	
 * @param	work	MV_TR_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PairTr_WorkInit( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work, FLDMMDL * oyaobj )
{
	work->oya_init = TRUE;
	work->oya_gx = FLDMMDL_NowPosGX_Get( oyaobj );
	work->oya_gz = FLDMMDL_NowPosGZ_Get( oyaobj );
	work->oya_ac = ACMD_NOT;
	work->oyaobj = oyaobj;
}

//--------------------------------------------------------------
/**
 * �e���W�X�V�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	int		TRUE=�X�V����
 */
//--------------------------------------------------------------
static int PairTr_OyaPosUpdateCheck( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	FLDMMDL * oyaobj = work->oyaobj;
	int gx = FLDMMDL_NowPosGX_Get( fmmdl );
	int gz = FLDMMDL_NowPosGZ_Get( fmmdl );
	int ngx = FLDMMDL_OldPosGX_Get( oyaobj );
	int ngz = FLDMMDL_OldPosGZ_Get( oyaobj );
	
	if( (gx != ngx || gz != ngz) &&
		(FLDMMDL_StatusBitCheck_Move(oyaobj) == TRUE ||
		FLDMMDL_StatusBit_Check(oyaobj,PAIR_TR_OYA_STA_BIT_STOP) == 0) ){
		return( TRUE );
	}
	
	return( FALSE );
}

#if 0	//�����̍������l�����Ă��Ȃ�
static int PairTr_OyaPosUpdateCheck( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	FLDMMDL * oyaobj = work->oyaobj;
	int gx = FLDMMDL_NowPosGX_Get( fmmdl );
	int gz = FLDMMDL_NowPosGZ_Get( fmmdl );
	int ngx = FLDMMDL_OldPosGX_Get( oyaobj );
	int ngz = FLDMMDL_OldPosGZ_Get( oyaobj );
	
	if( (gx != ngx || gz != ngz) &&
		FLDMMDL_StatusBit_CheckEasy(oyaobj,FLDMMDL_STABIT_ATTR_GET_ERROR) == FALSE &&
		FLDMMDL_StatusBit_CheckEasy(oyaobj,FLDMMDL_STABIT_PAUSE_MOVE) == FALSE ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

#if 0
static int PairTr_OyaPosUpdateCheck( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	FLDMMDL * oyaobj = work->oyaobj;
	int gx = FLDMMDL_NowPosGX_Get( oyaobj );
	int gz = FLDMMDL_NowPosGZ_Get( oyaobj );
	
	if( (gx != work->oya_gx || gz != work->oya_gz) &&
		FLDMMDL_StatusBit_CheckEasy(oyaobj,FLDMMDL_STABIT_ATTR_GET_ERROR) == FALSE &&
		FLDMMDL_StatusBit_CheckEasy(oyaobj,FLDMMDL_STABIT_PAUSE_MOVE) == FALSE ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * �e���W�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_PAIR_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void PairTr_OyaPosSet( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	work->oya_gx = FLDMMDL_NowPosGX_Get( work->oyaobj );
	work->oya_gz = FLDMMDL_NowPosGZ_Get( work->oyaobj );
}

//--------------------------------------------------------------
/**
 * �e�A�j���R�[�h�A�������擾���A�j���[�V�����R�}���h���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�Z�b�g�ł��� FALSE=�d�Ȃ�ɂ��Z�b�g�ł��Ȃ�
 */
//--------------------------------------------------------------
static int PairTr_OyaCheckAcmdSet( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	int gx = FLDMMDL_NowPosGX_Get( fmmdl );
	int gz = FLDMMDL_NowPosGZ_Get( fmmdl );
	int ngx = FLDMMDL_NowPosGX_Get( work->oyaobj );
	int ngz = FLDMMDL_NowPosGZ_Get( work->oyaobj );
	int ogx = FLDMMDL_OldPosGX_Get( work->oyaobj );
	int ogz = FLDMMDL_OldPosGZ_Get( work->oyaobj );
	int dir;
	
	if( gx == ngx && gz == ngz ){
		return( FALSE );
	}
	
	dir = FieldOBJTool_DirRange( gx, gz, ogx, ogz );
	gx += FLDMMDL_DirAddValueGX( dir );
	gz += FLDMMDL_DirAddValueGZ( dir );
		
	if( gx != ngx || gz != ngz ){
		u32 code = AC_WALK_U_8F;
		code = FLDMMDL_AcmdCodeDirChange( dir, code );
		FLDMMDL_CmdSet( fmmdl, code );
		return( TRUE );
	}
	
	return( FALSE );
}

#if 0
static int PairTr_OyaCheckAcmdSet( FLDMMDL * fmmdl, MV_TR_PAIR_WORK *work )
{
	int gx = FLDMMDL_NowPosGX_Get( fmmdl );
	int gz = FLDMMDL_NowPosGZ_Get( fmmdl );
	int ngx = FLDMMDL_NowPosGX_Get( work->oyaobj );
	int ngz = FLDMMDL_NowPosGZ_Get( work->oyaobj );
	int ogx = FLDMMDL_OldPosGX_Get( work->oyaobj );
	int ogz = FLDMMDL_OldPosGZ_Get( work->oyaobj );
	int sx,sz;
	
	if( gx == ngx && gz == ngz ){
		return( FALSE );
	}
	
	sx = gx - ogx;
	if( sx < 0 ){ sx = -sx; }
	sz = gz - ogz;
	if( sz < 0 ){ sz = -sz; }
	
	if( sx || sz ){
		int dir = FieldOBJTool_DirRange( gx, gz, ogx, ogz );
		gx += FLDMMDL_DirAddValueGX( dir );
		gz += FLDMMDL_DirAddValueGZ( dir );
		
		if( gx != ngx && gz != ngz ){
			u32 code = AC_WALK_U_8F;
			code = FLDMMDL_AcmdCodeDirChange( dir, code );
			FLDMMDL_CmdSet( fmmdl, code );
			return( TRUE );
		}
	}
	
	return( FALSE );
}
#endif

//==============================================================================
//	MV_HIDE_SNOW�@�B�ꖪ�@��
//==============================================================================
//--------------------------------------------------------------
/**
 * MV_HIDE ������
 * @param	fmmdl	FLDMMDL *
 * @param	type	HIDETYPE
 * @retval	nothing
 */
//--------------------------------------------------------------
#if 0
static void fmmdl_MoveHide_Init( FLDMMDL * fmmdl, HIDETYPE type )
{
	MV_HIDE_WORK *work = FLDMMDL_MoveProcWorkInit( fmmdl, MV_HIDE_WORK_SIZE );
	work->hide_type = type;
	FLDMMDL_DrawStatusSet( fmmdl, DRAW_STA_STOP );
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
	
	{															//�������Ƃ�
		VecFx32 offs = { 0, NUM_FX32(-32), 0 };
		FLDMMDL_VecDrawOffsSet( fmmdl, &offs );
	}
}
#endif

//--------------------------------------------------------------
/**
 * MV_HIDE_SNOW ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveHideSnow_Init( FLDMMDL * fmmdl )
{
#if 0
	fmmdl_MoveHide_Init( fmmdl, HIDE_SNOW );
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE_SAND ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveHideSand_Init( FLDMMDL * fmmdl )
{
#if 0
	fmmdl_MoveHide_Init( fmmdl, HIDE_SAND );
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE_GRND ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveHideGround_Init( FLDMMDL * fmmdl )
{
#if 0
	fmmdl_MoveHide_Init( fmmdl, HIDE_GROUND );
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE_KUSA ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveHideKusa_Init( FLDMMDL * fmmdl )
{
#if 0
	fmmdl_MoveHide_Init( fmmdl, HIDE_GRASS );
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE ����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveHide_Move( FLDMMDL * fmmdl )
{
	MV_HIDE_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	while( DATA_HideMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_HIDE �폜
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveHide_Delete( FLDMMDL * fmmdl )
{
#if 0
	EOA_PTR eoa = FLDMMDL_MoveHideEoaPtrGet( fmmdl );
	if( eoa != NULL ){ FE_EoaDelete( eoa ); }
#endif
}

//--------------------------------------------------------------
/**
 * MV_HIDE ���A
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveHide_Return( FLDMMDL * fmmdl )
{
#if 0
	MV_HIDE_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	
	work->seq_no = 0;
	
	FLDMMDL_MoveHideEoaPtrSet( fmmdl, NULL );
	
	if( work->pulloff_flag == FALSE ){
		VecFx32 offs = { 0, NUM_FX32(-32), 0 };
		FLDMMDL_VecDrawOffsSet( fmmdl, &offs );
		//add pl 
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
	}
#endif
}

//==============================================================================
//	MV_HIDE ����
//==============================================================================
//--------------------------------------------------------------
/**
 * hide 0
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_HIDE_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int HideMove_Init( FLDMMDL * fmmdl, MV_HIDE_WORK *work )
{
#if 0
	if( work->pulloff_flag == FALSE ){
		EOA_PTR eoa = FE_fmmdlHide_Add( fmmdl, work->hide_type );
		FLDMMDL_MoveHideEoaPtrSet( fmmdl, eoa );
	}
	
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	FLDMMDL_StatusBitOFF_MoveEnd( fmmdl );
	
	work->seq_no++;
#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * hide 1
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_HIDE_WORK
 * @retval	int		TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int HideMove_Move( FLDMMDL * fmmdl, MV_HIDE_WORK *work )
{
	if( work->pulloff_flag == FALSE ){
#if 0
		EOA_PTR eoa = FLDMMDL_MoveHideEoaPtrGet( fmmdl );
	
		if( eoa == NULL ){ 
			if( FLDMMDL_StatusBit_DrawInitCompCheck(fmmdl) == TRUE ){
				eoa = FE_fmmdlHide_Add( fmmdl, work->hide_type );
				FLDMMDL_MoveHideEoaPtrSet( fmmdl, eoa );
			}
		}
		
		//add pl ��ɉe������
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
///	MV_HIDE ����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_HideMoveTbl[])( FLDMMDL * fmmdl, MV_HIDE_WORK *work ) =
{
	HideMove_Init,
	HideMove_Move,
};

//==============================================================================
//	MV_HIDE �p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * MV_HIDE �B�ꖪEOA_PTR�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	eoa		EOA_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
#if 0
void FLDMMDL_MoveHideEoaPtrSet( FLDMMDL * fmmdl, EOA_PTR eoa )
{
	MV_HIDE_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	work->eoa_hide = eoa;
}

//--------------------------------------------------------------
/**
 * MV_HIDE �B�ꖪEOA_PTR�擾
 * @param	fmmdl	FLDMMDL *
 * @param	eoa		EOA_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
EOA_PTR FLDMMDL_MoveHideEoaPtrGet( FLDMMDL * fmmdl )
{
	MV_HIDE_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	return( work->eoa_hide );
}
#endif

//--------------------------------------------------------------
/**
 * MV_HIDE �B�ꖪ�A�E������Ԃ�
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveHidePullOffFlagSet( FLDMMDL * fmmdl )
{
	MV_HIDE_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	work->pulloff_flag = TRUE;
}

//==============================================================================
//	MV_COPYU
//==============================================================================
static int (* const DATA_CopyMoveTbl[SEQNO_COPYMOVE_MAX])( FLDMMDL *, MV_COPY_WORK * );

//--------------------------------------------------------------
/**
 * ���̂܂ˁ@���[�N������
 * @param	fmmdl	FLDMMDL *
 * @param	dir		��������
 * @param	lgrass	������������̂܂� FALSE=Not
 * @retval
 */
//--------------------------------------------------------------
static void MoveCopy_WorkInit( FLDMMDL * fmmdl, int dir, u32 lgrass )
{
	MV_COPY_WORK *work = FLDMMDL_MoveProcWorkInit( fmmdl, MV_COPY_WORK_SIZE );
	work->dir_jiki = DIR_NOT;
	work->lgrass_on = lgrass;
	FLDMMDL_DirDispCheckSet( fmmdl, DIR_UP );
}

//--------------------------------------------------------------
/**
 * MV_COPYU ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCopyU_Init( FLDMMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_UP, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYD ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCopyD_Init( FLDMMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_DOWN, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYL ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCopyL_Init( FLDMMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_LEFT, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYR ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCopyR_Init( FLDMMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_RIGHT, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSU ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCopyLGrassU_Init( FLDMMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_UP, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSD ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCopyLGrassD_Init( FLDMMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_DOWN, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSL ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCopyLGrassL_Init( FLDMMDL * fmmdl )
{
	MoveCopy_WorkInit( fmmdl, DIR_LEFT, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSR ������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCopyLGrassR_Init( FLDMMDL * fmmdl )
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
void FLDMMDL_MoveCopy_Move( FLDMMDL * fmmdl )
{
	MV_COPY_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	while( DATA_CopyMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_COPY ����@��������@����0
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_FirstInit( FLDMMDL * fmmdl, MV_COPY_WORK *work )
{
	int ret = FLDMMDL_DirDispGet( fmmdl );
	ret = FLDMMDL_AcmdCodeDirChange( ret, AC_DIR_U );
	FLDMMDL_CmdSet( fmmdl, ret );
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	FLDMMDL_StatusBitOFF_MoveEnd( fmmdl );
	work->seq_no = SEQNO_COPYMOVE_FIRST_WAIT;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYU ����@��������@����1
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_FirstWait( FLDMMDL * fmmdl, MV_COPY_WORK *work )
{
	if( FLDMMDL_CmdAction(fmmdl) == TRUE ){
		work->seq_no = SEQNO_COPYMOVE_INIT;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPY ���� 0
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_Init( FLDMMDL * fmmdl, MV_COPY_WORK *work )
{
#if 0
	if( work->dir_jiki == DIR_NOT ){
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		work->dir_jiki = Player_DirGet( fsys->player );
	}
	
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	FLDMMDL_StatusBitOFF_MoveEnd( fmmdl );
	work->seq_no = SEQNO_COPYMOVE_CMD_SET;
#endif
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���̂܂ˑ��ړ��`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	dir		�ړ�����
 * @retval	u32		hit bit	
 */
//--------------------------------------------------------------
static u32 CopyMove_LongGrassHitCheck( FLDMMDL * fmmdl, int dir )
{
#if 0
	u32 ret = FLDMMDL_NextDirAttrGet( fmmdl, dir );
	
	if( MATR_IsLongGrass(ret) == FALSE ){
		ret = FLDMMDL_MOVE_HIT_BIT_ATTR;
	}
	
	ret |= FLDMMDL_MoveHitCheckDir( fmmdl, dir );
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
static void CopyMove_MoveSet( FLDMMDL * fmmdl, int dir, int ac, u32 lgrass_on )
{
	u32 ret;
	
	if( lgrass_on == FALSE ){
		ret = FLDMMDL_MoveHitCheckDir( fmmdl, dir );
	}else{
		ret = CopyMove_LongGrassHitCheck( fmmdl, dir );
	}
	
	if( ret != FLDMMDL_MOVE_HIT_BIT_NON ){
		ac = FLDMMDL_AcmdCodeDirChange( dir, AC_DIR_U );
	}else{
		ac = FLDMMDL_AcmdCodeDirChange( dir, ac );
		FLDMMDL_StatusBitON_Move( fmmdl );
	}
	
	FLDMMDL_CmdSet( fmmdl, ac );
}

//--------------------------------------------------------------
/**
 * MV_COPY ���� 1
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_CmdSet( FLDMMDL * fmmdl, MV_COPY_WORK *work )
{
#if 0
	int ret;
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
	int dir = Player_DirGet( fsys->player );
	u32 type = Player_AcmdTypeGet( fsys->player );
	
	switch( type ){
	case HEROACTYPE_NON:
	case HEROACTYPE_STOP:
		ret = FLDMMDL_AcmdCodeDirChange( dir, AC_DIR_U );
		FLDMMDL_CmdSet( fmmdl, ret );
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
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_COPY_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_MoveWait( FLDMMDL * fmmdl, MV_COPY_WORK *work )
{
	if( FLDMMDL_CmdAction(fmmdl) == FALSE ){
		return( FALSE );
	}
	
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	FLDMMDL_StatusBitOFF_MoveEnd( fmmdl );
	work->seq_no = SEQNO_COPYMOVE_INIT;
	return( FALSE );
}

//--------------------------------------------------------------
///	�R�s�[����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_CopyMoveTbl[SEQNO_COPYMOVE_MAX])( FLDMMDL *, MV_COPY_WORK * ) =
{
	CopyMove_FirstInit,
	CopyMove_FirstWait,
	CopyMove_Init,
	CopyMove_CmdSet,
	CopyMove_MoveWait,
};

//==============================================================================
//	�ǉ����ړ�
//==============================================================================
static int AlongWall_WallMove( FLDMMDL * fmmdl, MV_ALONGW_WORK *work, int ac );
static int (* const DATA_AlongMoveTbl[3])( FLDMMDL *, MV_ALONGW_WORK * );

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@���[�N������
 * @param	fmmdl		FLDMMDL *
 * @param	dir_h_init	����������
 * @param	dir_h		������
 * @retval	nothing
 */
//--------------------------------------------------------------
static void AlongWallWorkInit( FLDMMDL * fmmdl, ALONG_DIR dir_h_init, ALONG_DIR dir_h )
{
	MV_ALONGW_WORK *work = FLDMMDL_MoveProcWorkInit( fmmdl, MV_ALONGW_WORK_SIZE );
	work->dir_hand_init = dir_h_init;
	work->dir_hand = dir_h;
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@����@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AlongWallL_Init( FLDMMDL * fmmdl )
{
	AlongWallWorkInit( fmmdl, ALONG_L, ALONG_L );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@�E��@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AlongWallR_Init( FLDMMDL * fmmdl )
{
	AlongWallWorkInit( fmmdl, ALONG_R, ALONG_R );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@���荶�@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AlongWallLRL_Init( FLDMMDL * fmmdl )
{
	AlongWallWorkInit( fmmdl, ALONG_LR, ALONG_L );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@����E�@������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AlongWallLRR_Init( FLDMMDL * fmmdl )
{
	AlongWallWorkInit( fmmdl, ALONG_LR, ALONG_R );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AlongWall_Move( FLDMMDL * fmmdl )
{
	MV_ALONGW_WORK *work = FLDMMDL_MoveProcWorkGet( fmmdl );
	while( DATA_AlongMoveTbl[work->seq_no](fmmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * ���� ������
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AlongWallMove_Init( FLDMMDL * fmmdl, MV_ALONGW_WORK *work )
{
	FLDMMDL_StatusBitOFF_Move( fmmdl );
	work->seq_no++;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���� ������
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AlongWallMove_CmdSet( FLDMMDL * fmmdl, MV_ALONGW_WORK *work )
{
	AlongWall_WallMove( fmmdl, work, AC_WALK_U_8F );
	work->seq_no++;
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���� �R�}���h�I���҂�
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=�ċA
 */
//--------------------------------------------------------------
static int AlongWallMove_CmdWait( FLDMMDL * fmmdl, MV_ALONGW_WORK *work )
{
	if( FLDMMDL_CmdAction(fmmdl) == TRUE ){
		work->seq_no = 0;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
//	�Ǔ`���ړ��@����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_AlongMoveTbl[3])( FLDMMDL *, MV_ALONGW_WORK * ) =
{
	AlongWallMove_Init,
	AlongWallMove_CmdSet,
	AlongWallMove_CmdWait,
};

//--------------------------------------------------------------
///	��̈ʒu
//--------------------------------------------------------------
static const int DATA_DirHandPosX[DIR_4_MAX][2] = 
{
	{ -1, 1 },	//up,left,right
	{ 1, -1 },	//down,left,right
	{ 0, 0 },	//left,left,right
	{ 0, 0 },	//right,left,right
};

static const int DATA_DirHandPosZ[DIR_4_MAX][2] =
{
	{ 0, 0 },	//up,left,right
	{ 0, 0 },	//down,left,right
	{ 1, -1 },	//left,left,right
	{ -1, 1 },	//right,left,right
};

//--------------------------------------------------------------
///	���������ۂ̔���ʒu
//--------------------------------------------------------------
static const int DATA_DirHandLostPosX[DIR_4_MAX][2] =
{
	{ -1, 1 },	//up,left,right
	{ 1, -1 },	//down,left,right
	{ 1, 1  },	//left,left,right
	{ -1, -1 },	//right,left,right
};

static const int DATA_DirHandLostPosZ[DIR_4_MAX][2] =
{
	{ 1, 1 },	//up,left,right
	{ -1, -1 },	//down,left,right
	{ 1, -1  },	//left,left,right
	{ -1, 1 },	//right,left,right
};

//--------------------------------------------------------------
///	���������ۂ̕����؂�ւ�
//--------------------------------------------------------------
static const int DATA_DirHandLostDir[DIR_4_MAX][2] =
{
	{ DIR_LEFT, DIR_RIGHT },	//up,left,right
	{ DIR_RIGHT, DIR_LEFT },	//down,left,right
	{ DIR_DOWN, DIR_UP  },	//left,left,right
	{ DIR_UP, DIR_DOWN },	//right,left,right
};

//--------------------------------------------------------------
///	�ǏՓˎ��̕����؂�ւ�
//--------------------------------------------------------------
static const int DATA_DirHandAttrHitDir[DIR_4_MAX][2] =
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
#if 0
static BOOL AlongWall_HandHitGet( FIELDSYS_WORK *fsys, int gx, int gz, int dir, ALONG_DIR hdir )
{
#if 0
	BOOL hit;
	gx += DATA_DirHandPosX[dir][hdir];
	gz += DATA_DirHandPosZ[dir][hdir];
	hit = GetHitAttr( fsys, gx, gz );
	return( hit );
#else
	return( FALSE );
#endif
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
#if 0
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
 * @param	fmmdl	FLDMMDL *
 * @param	dir_hand	ALONG_DIR
 * @retval	int		TRUE=�ǃA��
 */
//--------------------------------------------------------------
static int AlongWall_HandWallCheck( FLDMMDL * fmmdl, int dir, ALONG_DIR dir_hand )
{
#if 0
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
	int gx = FLDMMDL_NowPosGX_Get( fmmdl );
	int gz = FLDMMDL_NowPosGZ_Get( fmmdl );
	BOOL hit = AlongWall_HandHitGet( fsys, gx, gz, dir, dir_hand );
	return( hit );
#else
	return( FALSE );
#endif
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@�ǎ�T��
 * @param	fmmdl	FLDMMDL *
 * @param	dir_hand	ALONG_DIR
 * @retval	int		TRUE=�ǃA��
 */
//--------------------------------------------------------------
static int AlongWall_HandLostWallCheck( FLDMMDL * fmmdl, int dir, ALONG_DIR dir_hand )
{
#if 0
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
	int gx = FLDMMDL_NowPosGX_Get( fmmdl );
	int gz = FLDMMDL_NowPosGZ_Get( fmmdl );
	BOOL hit = AlongWall_HandLostHitGet( fsys, gx, gz, dir, dir_hand );
	return( hit );
#else
	return( FALSE );
#endif
}

//--------------------------------------------------------------
/**
 * �ǈړ��`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	dir_hand	ALONG_DIR
 * @retval	int		�ړ�����ׂ�����
 */
//--------------------------------------------------------------
static int AlongWall_MoveHitCheck( FLDMMDL * fmmdl, int dir_move, ALONG_DIR dir_hand )
{
#if 0
	if( AlongWall_HandWallCheck(fmmdl,dir_move,dir_hand) == FALSE ){	//�ǂ�����
		if( AlongWall_HandLostWallCheck(fmmdl,dir_move,dir_hand) == FALSE ){
			return( DIR_NOT );									//��T��ł��ǂ�����
		}
		
		dir_move = DATA_DirHandLostDir[dir_move][dir_hand];		//�ǔ��� �����؂�ւ�
	}
	
	return( dir_move );
#else
	return( 0 );
#endif
}

//--------------------------------------------------------------
/**
 * �Ǔ`���ړ��`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	dir_move	�ړ����� DIR_NOT=�ړ��s��
 * @param	dir_hand	�Ǒ��̎�̌���
 * @retval	u32		�q�b�g�r�b�g
 */
//--------------------------------------------------------------
static u32 AlongWall_WallMoveCheck( FLDMMDL * fmmdl, int *dir_move, ALONG_DIR dir_hand )
{
	u32 ret;
	
	*dir_move = AlongWall_MoveHitCheck( fmmdl, *dir_move, dir_hand );
	
	if( *dir_move != DIR_NOT ){
		ret = FLDMMDL_MoveHitCheckDir( fmmdl, *dir_move );
		return( ret );
	}
	
	return( 0 );
}

//--------------------------------------------------------------
/**
 * �Ǔ`���ړ�
 * @param	fmmdl	FLDMMDL *
 * @param	work	MV_ALONGW_WORK
 * @retval	int		TRUE=�ړ�����
 */
//--------------------------------------------------------------
static int AlongWall_WallMove( FLDMMDL * fmmdl, MV_ALONGW_WORK *work, int ac )
{
	u32 ret;
	int dir_hand = work->dir_hand;
	int dir_move = FLDMMDL_DirDispGet( fmmdl );
	
	ret = AlongWall_WallMoveCheck( fmmdl, &dir_move, dir_hand );
	
	if( dir_move == DIR_NOT ){					//�ǂ��Ȃ�
		dir_move = FLDMMDL_DirDispGet( fmmdl );
		ac = FLDMMDL_AcmdCodeDirChange( dir_move, AC_STAY_WALK_U_16F );
		FLDMMDL_CmdSet( fmmdl, ac );
		return( FALSE );
	}
	
	if( ret == FLDMMDL_MOVE_HIT_BIT_NON ){		//�ړ��\
		ac = FLDMMDL_AcmdCodeDirChange( dir_move, ac );
		FLDMMDL_StatusBitON_Move( fmmdl );
		FLDMMDL_CmdSet( fmmdl, ac );
		return( TRUE );
	}
	
	//�ړ������q�b�g&���藘���@���]�����݂�
	if( (ret & FLDMMDL_MOVE_HIT_BIT_LIM) && work->dir_hand_init == ALONG_LR ){
		dir_move = FieldOBJTool_DirFlip( FLDMMDL_DirDispGet(fmmdl) );
		dir_hand = DATA_DirHandFlip[dir_hand];
		work->dir_hand = dir_hand;
		
		ret = AlongWall_WallMoveCheck( fmmdl, &dir_move, dir_hand );
		
		if( dir_move == DIR_NOT ){					//�ǂ��Ȃ�
			dir_move = FLDMMDL_DirDispGet( fmmdl );
			ac = FLDMMDL_AcmdCodeDirChange( dir_move, AC_STAY_WALK_U_16F );
			FLDMMDL_CmdSet( fmmdl, ac );
			return( FALSE );
		}
		
		if( ret == FLDMMDL_MOVE_HIT_BIT_NON ){		//�ړ��\
			ac = FLDMMDL_AcmdCodeDirChange( dir_move, ac );
			FLDMMDL_StatusBitON_Move( fmmdl );
			FLDMMDL_CmdSet( fmmdl, ac );
			return( TRUE );
		}
	}
	
	//�ǃq�b�g�@�ړ������ύX
	if( (ret & FLDMMDL_MOVE_HIT_BIT_ATTR) ){
		dir_move = DATA_DirHandAttrHitDir[dir_move][dir_hand];
		
		ret = AlongWall_WallMoveCheck( fmmdl, &dir_move, dir_hand );
		
		if( dir_move == DIR_NOT ){					//�ǂ�����
			dir_move = FLDMMDL_DirDispGet( fmmdl );
			ac = FLDMMDL_AcmdCodeDirChange( dir_move, AC_STAY_WALK_U_16F );
			FLDMMDL_CmdSet( fmmdl, ac );
			return( FALSE );
		}
		
		if( ret == FLDMMDL_MOVE_HIT_BIT_NON ){		//�ړ��\
			ac = FLDMMDL_AcmdCodeDirChange( dir_move, ac );
			FLDMMDL_StatusBitON_Move( fmmdl );
			FLDMMDL_CmdSet( fmmdl, ac );
			return( TRUE );
		}
	}
	
	//�ړ��s��
	dir_move = FLDMMDL_DirDispGet( fmmdl );	//������߂�
	ac = FLDMMDL_AcmdCodeDirChange( dir_move, AC_STAY_WALK_U_16F );
	FLDMMDL_CmdSet( fmmdl, ac );
	return( FALSE );
}

