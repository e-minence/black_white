//======================================================================
/**
 *
 * @file  fieldobj_move_2.c
 * @brief  �t�B�[���h���샂�f���@��{����n����2
 * @author  kagaya
 * @date  05.07.21
 *
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procmove.h"

#include "field_player.h"   //event_trainer_eye.h
#include "field_encount.h"  //event_trainer_eye.h
#include "event_trainer_eye.h"

#include "fieldmap.h"
#include "field_effect.h"

#include "fldeff_hide.h"

//======================================================================
//  define
//======================================================================
///�g���[�i�[�y�A�@�e�ړ���~
#define PAIR_TR_OYA_MOVEBIT_STOP \
  (MMDL_MOVEBIT_ATTR_GET_ERROR    | \
   MMDL_MOVEBIT_HEIGHT_GET_ERROR  | \
   MMDL_MOVEBIT_PAUSE_MOVE)

//--------------------------------------------------------------
///  �R�s�[����ԍ�
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
///  �ǈړ��@������
//--------------------------------------------------------------
typedef enum
{
  ALONG_L = 0,    ///<��
  ALONG_R,      ///<�E
  ALONG_LR,      ///<���E
}ALONG_DIR;

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
//  MV_PAIR_WORK�\����
//--------------------------------------------------------------
typedef struct
{
  u8 seq_no;        ///<����ԍ�
  u8 jiki_init;      ///<���@��񏉊�������
  s16 jiki_gx;      ///<���@�O���b�h���WX
  s16 jiki_gz;      ///<���@�O���b�h���WZ
  u16 jiki_ac;      ///<���@�A�j���[�V�����R�[�h
}MV_PAIR_WORK;

#define MV_PAIR_WORK_SIZE (sizeof(MV_PAIR_WORK))

//--------------------------------------------------------------
//  MV_TR_PAIR_WORK�\����
//--------------------------------------------------------------
typedef struct
{
  u8 seq_no;        ///<����ԍ�
  u8 oya_init;      ///<�e��񏉊�������
  s16 oya_gx;        ///<�e�O���b�h���WX
  s16 oya_gz;        ///<�e�O���b�h���WZ
  u16 oya_ac;        ///<���@�A�j���[�V�����R�[�h
  MMDL * oyaobj;    ///<�e�ƂȂ�OBJ
}MV_TR_PAIR_WORK;

#define MV_TR_PAIR_WORK_SIZE (sizeof(MV_TR_PAIR_WORK))

//--------------------------------------------------------------
///  MV_HIDE_WORK�\����
//--------------------------------------------------------------
typedef struct
{
  u8 seq_no;
  u8 hide_type;
  u8 pulloff_flag;
  u8 dmy;
  FLDEFF_TASK *task_hide;
}MV_HIDE_WORK;

#define MV_HIDE_WORK_SIZE (sizeof(MV_HIDE_WORK))

//--------------------------------------------------------------
///  MV_COPY_WORK�\����
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
///  MV_ALONGW_WORK�\����
//--------------------------------------------------------------
typedef struct
{
  u32 seq_no;
  ALONG_DIR dir_hand_init;
  ALONG_DIR dir_hand;
}MV_ALONGW_WORK;

#define MV_ALONGW_WORK_SIZE (sizeof(MV_ALONGW_WORK))

//======================================================================
//  �v���g�^�C�v
//======================================================================
int (* const DATA_PairMoveTbl[])( MMDL * mmdl, MV_PAIR_WORK *work );

static int Pair_WorkSetJikiSearch( MMDL * mmdl, MV_PAIR_WORK *work );
static void Pair_WorkInit( MMDL * mmdl, MV_PAIR_WORK *work );
static int Pair_JikiPosUpdateCheck( MMDL * mmdl, MV_PAIR_WORK *work );
static void Pair_JikiPosSet( MMDL * mmdl, MV_PAIR_WORK *work );
static u32 Pair_JikiAcmdCodeGet( MMDL * mmdl );
static int Pair_JikiCheckAcmdSet( MMDL * mmdl );

int (* const DATA_PairTrMoveTbl[])( MMDL * mmdl, MV_TR_PAIR_WORK *work );

static int PairTr_WorkSetOyaSearch( MMDL * mmdl, MV_TR_PAIR_WORK *work );
static void PairTr_WorkInit( MMDL * mmdl, MV_TR_PAIR_WORK *work, MMDL * oyaobj);
static int PairTr_OyaPosUpdateCheck( MMDL * mmdl, MV_TR_PAIR_WORK *work );
static void PairTr_OyaPosSet( MMDL * mmdl, MV_TR_PAIR_WORK *work );
static int PairTr_OyaCheckAcmdSet( MMDL * mmdl, MV_TR_PAIR_WORK *work );

int (* const DATA_HideMoveTbl[])( MMDL * mmdl, MV_HIDE_WORK *work );

//======================================================================
//  MV_PAIR  ���@�A�����
//======================================================================
//--------------------------------------------------------------
/**
 * MV_PAIR�@������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MovePair_Init( MMDL * mmdl )
{
  MV_PAIR_WORK *work = MMDL_InitMoveProcWork( mmdl, MV_PAIR_WORK_SIZE );
  Pair_WorkSetJikiSearch( mmdl, work );
  MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
  MMDL_OffMoveBitMove( mmdl );
  MMDL_SetStatusBitFellowHit( mmdl, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_PAIR�@����
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MovePair_Move( MMDL * mmdl )
{
  MV_PAIR_WORK *work = MMDL_GetMoveProcWork( mmdl );
  
  if( Pair_WorkSetJikiSearch(mmdl,work) == FALSE ){
    return;
  }
  
  MMDL_SetStatusBitFellowHit( mmdl, FALSE );
  
  while( DATA_PairMoveTbl[work->seq_no](mmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_PAIR �폜
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MovePair_Delete( MMDL * mmdl )
{
}

//======================================================================
//  MV_PAIR ����
//======================================================================
//--------------------------------------------------------------
/**
 * Pair 0
 * @param  mmdl  MMDL *
 * @param  work  MV_PAIR_WORK
 * @retval  int    TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairMove_Init( MMDL * mmdl, MV_PAIR_WORK *work )
{
  MMDL_OffMoveBitMove( mmdl );
  MMDL_OffMoveBitMoveEnd( mmdl );
    
  if( Pair_JikiPosUpdateCheck(mmdl,work) == TRUE ){
    Pair_JikiPosSet( mmdl, work );
    
    if( Pair_JikiCheckAcmdSet(mmdl) == TRUE ){
      MMDL_OnMoveBitMove( mmdl );
      work->seq_no++;
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * Pair 1
 * @param  mmdl  MMDL *
 * @param  work  MV_PAIR_WORK
 * @retval  int    TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairMove_Move( MMDL * mmdl, MV_PAIR_WORK *work )
{
  if( MMDL_ActionLocalAcmd(mmdl) == TRUE ){
    MMDL_OffMoveBitMove( mmdl );
    work->seq_no = 0;
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
///  �ꂠ�邫����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_PairMoveTbl[])( MMDL * mmdl, MV_PAIR_WORK *work ) =
{
  PairMove_Init,
  PairMove_Move,
};

//======================================================================
//  MV_PAIR�@�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�����݂��邩�`�F�b�N�@���݂���̂ł���Ώ�񏉊��� 
 * @param  mmdl  MMDL *
 * @param  work  MV_PAIR_WORK
 * @retval  int    FALSE=���݂��Ă��Ȃ�
 */
//--------------------------------------------------------------
static int Pair_WorkSetJikiSearch( MMDL * mmdl, MV_PAIR_WORK *work )
{
  MMDL *jiki = MMDLSYS_SearchMMdlPlayer( MMDL_GetMMdlSys(mmdl) );
  
  if( jiki == NULL ){
    work->jiki_init = FALSE;
    return( FALSE );
  }
  
  if( work->jiki_init == FALSE ){
    Pair_WorkInit( mmdl, work );
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_PAIR_WORK�������@���@�����鎖�O��
 * @param  mmdl  MMDL *  
 * @param  work  MV_PAIR_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Pair_WorkInit( MMDL * mmdl, MV_PAIR_WORK *work )
{
  MMDL *jiki = MMDLSYS_SearchMMdlPlayer( MMDL_GetMMdlSys(mmdl) );
  work->jiki_init = TRUE;
  work->jiki_gx = MMDL_GetGridPosX( jiki );
  work->jiki_gz = MMDL_GetGridPosZ( jiki );
  work->jiki_ac = ACMD_NOT;
}

//--------------------------------------------------------------
/**
 * ���@���W�X�V�`�F�b�N
 * @param  mmdl  MMDL *
 * @param  work  MV_PAIR_WORK
 * @retval  int    TRUE=�X�V����
 */
//--------------------------------------------------------------
static int Pair_JikiPosUpdateCheck( MMDL * mmdl, MV_PAIR_WORK *work )
{
  MMDL *jiki = MMDLSYS_SearchMMdlPlayer( MMDL_GetMMdlSys(mmdl) );
  
  if( jiki != NULL ){
    int gx = MMDL_GetGridPosX( jiki );
    int gz = MMDL_GetGridPosZ( jiki );
    
    if( gx != work->jiki_gx || gz != work->jiki_gz ){
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���W�Z�b�g
 * @param  mmdl  MMDL *
 * @param  work  MV_PAIR_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void Pair_JikiPosSet( MMDL * mmdl, MV_PAIR_WORK *work )
{
  MMDL *jiki = MMDLSYS_SearchMMdlPlayer( MMDL_GetMMdlSys(mmdl) );
  work->jiki_gx = MMDL_GetGridPosX( jiki );
  work->jiki_gz = MMDL_GetGridPosZ( jiki );
}

//--------------------------------------------------------------
/**
 * ���@�A�j���R�[�h�擾
 * @param  mmdl  MMDL *
 * @retval  u32    �A�j���[�V�����R�[�h
 */
//--------------------------------------------------------------
static u32 Pair_JikiAcmdCodeGet( MMDL * mmdl )
{
  u16 code;
  MMDL *jiki = MMDLSYS_SearchMMdlPlayer( MMDL_GetMMdlSys(mmdl) );
  code = MMDL_GetAcmdCode( mmdl );
  
  switch( code ){
  case AC_DASH_U_4F: code = AC_WALK_U_4F; break;
  case AC_DASH_D_4F: code = AC_WALK_D_4F; break;
  case AC_DASH_L_4F: code = AC_WALK_L_4F; break;
  case AC_DASH_R_4F: code = AC_WALK_R_4F; break;
  }
  
  return( code );
}

//--------------------------------------------------------------
/**
 * ���@�A�j���R�[�h�A�������擾���A�j���[�V�����R�}���h���Z�b�g
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=�Z�b�g�ł��� FALSE=�d�Ȃ�ɂ��Z�b�g�ł��Ȃ�
 */
//--------------------------------------------------------------
static int Pair_JikiCheckAcmdSet( MMDL * mmdl )
{
  MMDL *jiki = MMDLSYS_SearchMMdlPlayer( MMDL_GetMMdlSys(mmdl) );
  int gx = MMDL_GetGridPosX( mmdl );
  int gz = MMDL_GetGridPosZ( mmdl );
  int jx = MMDL_GetOldGridPosX( jiki );
  int jz = MMDL_GetOldGridPosZ( jiki );
  
  if( gx != jx || gz != jz ){
    u32 code = Pair_JikiAcmdCodeGet( mmdl );
    int dir = MMDL_TOOL_GetRangeDir( gx, gz, jx, jz );
    code = MMDL_ChangeDirAcmdCode( dir, code );
    MMDL_SetLocalAcmd( mmdl, code );
    return( TRUE );
  }
  return( FALSE );
}

//======================================================================
//  MV_TR_PAIR �g���[�i�[�A�����
//======================================================================
//--------------------------------------------------------------
/**
 * MV_TR_PAIR�@������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MovePairTr_Init( MMDL * mmdl )
{
  MV_TR_PAIR_WORK *work = MMDL_InitMoveProcWork( mmdl, MV_TR_PAIR_WORK_SIZE );
  PairTr_WorkSetOyaSearch( mmdl, work );
  MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
  MMDL_OffMoveBitMove( mmdl );
  
#if 0 //���蔻��̖������͖����B
  MMDL_SetStatusBitFellowHit( mmdl, FALSE );
#endif

  work->oya_init = FALSE;
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR�@����
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MovePairTr_Move( MMDL * mmdl )
{
  MV_TR_PAIR_WORK *work = MMDL_GetMoveProcWork( mmdl );
  
  if( PairTr_WorkSetOyaSearch(mmdl,work) == FALSE ){
    return;
  }

#if 0 //���蔻��̖������͖����B
  MMDL_SetStatusBitFellowHit( mmdl, FALSE );
#endif

  while( DATA_PairTrMoveTbl[work->seq_no](mmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR �폜
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MovePairTr_Delete( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR�@���A
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MovePairTr_Return( MMDL * mmdl )
{
  MV_TR_PAIR_WORK *work = MMDL_GetMoveProcWork( mmdl );
  work->oya_init = 0;
}

//======================================================================
//  MV_TR_PAIR ����
//======================================================================
//--------------------------------------------------------------
/**
 * Pair 0
 * @param  mmdl  MMDL *
 * @param  work  MV_TR_PAIR_WORK
 * @retval  int    TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairTrMove_Init( MMDL * mmdl, MV_TR_PAIR_WORK *work )
{
  MMDL_OffMoveBitMove( mmdl );
  MMDL_OffMoveBitMoveEnd( mmdl );
  
  if( PairTr_OyaPosUpdateCheck(mmdl,work) == TRUE ){
    if( PairTr_OyaCheckAcmdSet(mmdl,work) == TRUE ){
      MMDL_OnMoveBitMove( mmdl );
      work->seq_no++;
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * Pair 1
 * @param  mmdl  MMDL *
 * @param  work  MV_PAIR_WORK
 * @retval  int    TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int PairTrMove_Move( MMDL * mmdl, MV_TR_PAIR_WORK *work )
{
  if( MMDL_ActionLocalAcmd(mmdl) == FALSE ){
    return( FALSE );
  }
  
  MMDL_OffMoveBitMove( mmdl );
  work->seq_no = 0;
  return( FALSE );
}

//--------------------------------------------------------------
///  �ꂠ�邫����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_PairTrMoveTbl[])( MMDL * mmdl, MV_TR_PAIR_WORK *work ) =
{
  PairTrMove_Init,
  PairTrMove_Move,
};

//======================================================================
//  MV_TR_PAIR�@�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �Ώۂ��y�A���삩�ǂ����B�y�A����ł���Α�����T��
 * @param  mmdl  MMDL *
 * @retval  MMDL *�@������MMDL *�ANULL=�y�A����ł͂Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDL_MovePairSearch( MMDL * mmdl )
{
  MMDL * pair;
  u32 no = 0;
  int type = MMDL_GetEventType( mmdl );
  int zone_id = MMDL_GetZoneID( mmdl );
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  u32 trid = TRAINER_MMDL_GetTrainerID( mmdl );
  
  switch( type ){
  case EV_TYPE_TRAINER:
  case EV_TYPE_TRAINER_EYEALL:
  case EV_TYPE_ITEM:
  case EV_TYPE_TRAINER_KYORO:
  case EV_TYPE_TRAINER_SPIN_STOP_L:
  case EV_TYPE_TRAINER_SPIN_STOP_R:
  case EV_TYPE_TRAINER_SPIN_MOVE_L:
  case EV_TYPE_TRAINER_SPIN_MOVE_R:
    while( MMDLSYS_SearchUseMMdl(mmdlsys,&pair,&no) == TRUE )
    {
      if( mmdl != pair && MMDL_GetZoneID(pair) == zone_id )
      {
        if( TRAINER_MMDL_GetTrainerID(pair) == trid )
        {
          return( pair );
        }
      }
    }
  }
  return( NULL );
}

//--------------------------------------------------------------
/**
 * �e�����݂��邩�`�F�b�N�@���݂���̂ł���Ώ�񏉊��� 
 * @param  mmdl  MMDL *
 * @param  work  MV_TR_PAIR_WORK
 * @retval  int    FALSE=���݂��Ă��Ȃ�
 */
//--------------------------------------------------------------
static int PairTr_WorkSetOyaSearch( MMDL * mmdl, MV_TR_PAIR_WORK *work )
{
  int zone;
  u32 trid,no;
  MMDL *oyaobj;
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  no = 0;
  zone = MMDL_GetZoneID( mmdl );
  trid = TRAINER_MMDL_GetTrainerID( mmdl );
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&oyaobj,&no) == TRUE ){
    if( mmdl != oyaobj && MMDL_GetZoneID(oyaobj) == zone &&
      TRAINER_MMDL_GetTrainerID(oyaobj) == trid ){
      
      if( work->oya_init == FALSE ){
        PairTr_WorkInit( mmdl, work, oyaobj );
      }
      
      return( TRUE );
    }
  }
  
  work->oya_init = FALSE;
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MV_TR_PAIR_WORK�������@�e�����鎖�O��
 * @param  mmdl  MMDL *  
 * @param  work  MV_TR_PAIR_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void PairTr_WorkInit( MMDL * mmdl, MV_TR_PAIR_WORK *work, MMDL * oyaobj )
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
 * @param  mmdl  MMDL *
 * @param  work  MV_PAIR_WORK
 * @retval  int    TRUE=�X�V����
 */
//--------------------------------------------------------------
static int PairTr_OyaPosUpdateCheck( MMDL * mmdl, MV_TR_PAIR_WORK *work )
{
  MMDL * oyaobj = work->oyaobj;
  int gx = MMDL_GetGridPosX( mmdl );
  int gz = MMDL_GetGridPosZ( mmdl );
  int ngx = MMDL_GetOldGridPosX( oyaobj );
  int ngz = MMDL_GetOldGridPosZ( oyaobj );
  
  if( (gx != ngx || gz != ngz) &&
    (MMDL_CheckMoveBitMove(oyaobj) == TRUE ||
    MMDL_CheckMoveBit(oyaobj,PAIR_TR_OYA_MOVEBIT_STOP) == 0) ){
    return( TRUE );
  }
  
  return( FALSE );
}

#if 0  //�����̍������l�����Ă��Ȃ�
static int PairTr_OyaPosUpdateCheck( MMDL * mmdl, MV_TR_PAIR_WORK *work )
{
  MMDL * oyaobj = work->oyaobj;
  int gx = MMDL_GetGridPosX( mmdl );
  int gz = MMDL_GetGridPosZ( mmdl );
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
static int PairTr_OyaPosUpdateCheck( MMDL * mmdl, MV_TR_PAIR_WORK *work )
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
 * @param  mmdl  MMDL *
 * @param  work  MV_PAIR_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
static void PairTr_OyaPosSet( MMDL * mmdl, MV_TR_PAIR_WORK *work )
{
  work->oya_gx = MMDL_GetGridPosX( work->oyaobj );
  work->oya_gz = MMDL_GetGridPosZ( work->oyaobj );
}

//--------------------------------------------------------------
/**
 * �e�A�j���R�[�h�A�������擾���A�j���[�V�����R�}���h���Z�b�g
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=�Z�b�g�ł��� FALSE=�d�Ȃ�ɂ��Z�b�g�ł��Ȃ�
 */
//--------------------------------------------------------------
static int PairTr_OyaCheckAcmdSet( MMDL * mmdl, MV_TR_PAIR_WORK *work )
{
  int gx = MMDL_GetGridPosX( mmdl );
  int gz = MMDL_GetGridPosZ( mmdl );
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
    MMDL_SetLocalAcmd( mmdl, code );
    return( TRUE );
  }
  
  return( FALSE );
}

#if 0
static int PairTr_OyaCheckAcmdSet( MMDL * mmdl, MV_TR_PAIR_WORK *work )
{
  int gx = MMDL_GetGridPosX( mmdl );
  int gz = MMDL_GetGridPosZ( mmdl );
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
      MMDL_SetLocalAcmd( mmdl, code );
      return( TRUE );
    }
  }
  
  return( FALSE );
}
#endif

//======================================================================
//  MV_HIDE_SNOW�@�B�ꖪ�@��
//======================================================================
//--------------------------------------------------------------
/**
 * MV_HIDE ������
 * @param  mmdl  MMDL *
 * @param  type  HIDETYPE
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MoveHide_Init( MMDL * mmdl, HIDE_TYPE type )
{
  MV_HIDE_WORK *work = MMDL_InitMoveProcWork( mmdl, MV_HIDE_WORK_SIZE );
  work->hide_type = type;
  MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
  MMDL_OffMoveBitMove( mmdl );
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH );
  
  { //�������Ƃ�
    VecFx32 offs = { 0, NUM_FX32(-32), 0 };
    MMDL_SetVectorDrawOffsetPos( mmdl, &offs );
  }
}

//--------------------------------------------------------------
/**
 * MV_HIDE_SNOW ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHideSnow_Init( MMDL * mmdl )
{
  MMdl_MoveHide_Init( mmdl, HIDE_SNOW );
}

//--------------------------------------------------------------
/**
 * MV_HIDE_SAND ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHideSand_Init( MMDL * mmdl )
{
  MMdl_MoveHide_Init( mmdl, HIDE_SAND );
}

//--------------------------------------------------------------
/**
 * MV_HIDE_GRND ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHideGround_Init( MMDL * mmdl )
{
  MMdl_MoveHide_Init( mmdl, HIDE_GROUND );
}

//--------------------------------------------------------------
/**
 * MV_HIDE_KUSA ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHideKusa_Init( MMDL * mmdl )
{
  MMdl_MoveHide_Init( mmdl, HIDE_GRASS );
}

//--------------------------------------------------------------
/**
 * MV_HIDE ����
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHide_Move( MMDL * mmdl )
{
  MV_HIDE_WORK *work = MMDL_GetMoveProcWork( mmdl );
  while( DATA_HideMoveTbl[work->seq_no](mmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_HIDE �폜
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHide_Delete( MMDL * mmdl )
{
  FLDEFF_TASK *task = MMDL_GetMoveHideEffectTask( mmdl );
  if( task != NULL ){ FLDEFF_TASK_CallDelete( task ); }
}

//--------------------------------------------------------------
/**
 * MV_HIDE ���A
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHide_Return( MMDL * mmdl )
{
  MV_HIDE_WORK *work = MMDL_GetMoveProcWork( mmdl );
  
  work->seq_no = 0;
  
  MMDL_SetMoveHideEffectTask( mmdl, NULL );
  
  if( work->pulloff_flag == FALSE ){
    VecFx32 offs = { 0, NUM_FX32(-32), 0 };
    MMDL_SetVectorDrawOffsetPos( mmdl, &offs );
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH );
  }
}

//======================================================================
//  MV_HIDE ����
//======================================================================
//--------------------------------------------------------------
/**
 * �B�ꖪ�G�t�F�N�g�Z�b�g
 * @param mmdl  MMDL*
 * @param type HIDE_TYPE
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
static FLDEFF_TASK * set_HideEffect( MMDL *mmdl, HIDE_TYPE type )
{
  MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  FIELDMAP_WORK *fieldmap = MMDLSYS_GetFieldMapWork( mmdlsys );
  FLDEFF_CTRL *fectrl = FIELDMAP_GetFldEffCtrl( fieldmap );
  FLDEFF_TASK *eff_task = FLDEFF_HIDE_SetMMdl( fectrl, mmdl, type );
  return( eff_task );
}

//--------------------------------------------------------------
/**
 * hide 0
 * @param  mmdl  MMDL *
 * @param  work  MV_HIDE_WORK
 * @retval  int    TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int HideMove_Init( MMDL * mmdl, MV_HIDE_WORK *work )
{
  if( work->pulloff_flag == FALSE ){
    FLDEFF_TASK *eff_task = set_HideEffect( mmdl, work->hide_type );
    MMDL_SetMoveHideEffectTask( mmdl, eff_task );
  }
  
  MMDL_OffMoveBitMove( mmdl );
  MMDL_OffMoveBitMoveEnd( mmdl );
  
  work->seq_no++;
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * hide 1
 * @param  mmdl  MMDL *
 * @param  work  MV_HIDE_WORK
 * @retval  int    TRUE=�ċA�v��
 */
//--------------------------------------------------------------
static int HideMove_Move( MMDL * mmdl, MV_HIDE_WORK *work )
{
  if( work->pulloff_flag == FALSE ){
    FLDEFF_TASK *eff_task = MMDL_GetMoveHideEffectTask( mmdl );
    
    if( eff_task == NULL ){ 
      if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
        eff_task = set_HideEffect( mmdl, work->hide_type );
        MMDL_SetMoveHideEffectTask( mmdl, eff_task );
      }
    }
    
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH ); //��ɉe������
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
///  MV_HIDE ����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_HideMoveTbl[])( MMDL * mmdl, MV_HIDE_WORK *work ) =
{
  HideMove_Init,
  HideMove_Move,
};

//======================================================================
//  MV_HIDE �p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * MV_HIDE �B�ꖪFLDEFF_TASK�Z�b�g
 * @param  mmdl  MMDL *
 * @param  eoa    FLDEFF_TASK
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveHideEffectTask( MMDL * mmdl, FLDEFF_TASK *task )
{
  MV_HIDE_WORK *work = MMDL_GetMoveProcWork( mmdl );
  work->task_hide = task;
}

//--------------------------------------------------------------
/**
 * MV_HIDE �B�ꖪFLDEFF_TASK�擾
 * @param  mmdl  MMDL *
 * @param  eoa    FLDEFF_TASK
 * @retval  nothing
 */
//--------------------------------------------------------------
FLDEFF_TASK * MMDL_GetMoveHideEffectTask( MMDL * mmdl )
{
  MV_HIDE_WORK *work = MMDL_GetMoveProcWork( mmdl );
  return( work->task_hide );
}

//--------------------------------------------------------------
/**
 * MV_HIDE �B�ꖪ�A�E������Ԃ�
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveHidePullOffFlagSet( MMDL * mmdl )
{
  MV_HIDE_WORK *work = MMDL_GetMoveProcWork( mmdl );
  work->pulloff_flag = TRUE;
}

//======================================================================
//  MV_COPYU
//======================================================================
static int (* const DATA_CopyMoveTbl[SEQNO_COPYMOVE_MAX])( MMDL *, MV_COPY_WORK * );

//--------------------------------------------------------------
/**
 * ���̂܂ˁ@���[�N������
 * @param  mmdl  MMDL *
 * @param  dir    ��������
 * @param  lgrass  ������������̂܂� FALSE=Not
 * @retval
 */
//--------------------------------------------------------------
static void MoveCopy_WorkInit( MMDL * mmdl, int dir, u32 lgrass )
{
  MV_COPY_WORK *work = MMDL_InitMoveProcWork( mmdl, MV_COPY_WORK_SIZE );
  work->dir_jiki = DIR_NOT;
  work->lgrass_on = lgrass;
  MMDL_SetDirDisp( mmdl, DIR_UP );
}

//--------------------------------------------------------------
/**
 * MV_COPYU ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyU_Init( MMDL * mmdl )
{
  MoveCopy_WorkInit( mmdl, DIR_UP, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYD ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyD_Init( MMDL * mmdl )
{
  MoveCopy_WorkInit( mmdl, DIR_DOWN, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYL ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyL_Init( MMDL * mmdl )
{
  MoveCopy_WorkInit( mmdl, DIR_LEFT, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYR ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyR_Init( MMDL * mmdl )
{
  MoveCopy_WorkInit( mmdl, DIR_RIGHT, FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSU ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyLGrassU_Init( MMDL * mmdl )
{
  MoveCopy_WorkInit( mmdl, DIR_UP, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSD ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyLGrassD_Init( MMDL * mmdl )
{
  MoveCopy_WorkInit( mmdl, DIR_DOWN, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSL ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyLGrassL_Init( MMDL * mmdl )
{
  MoveCopy_WorkInit( mmdl, DIR_LEFT, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYLGRASSR ������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveCopyLGrassR_Init( MMDL * mmdl )
{
  MoveCopy_WorkInit( mmdl, DIR_RIGHT, TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPY ����
 * @param
 * @retval
 */
//--------------------------------------------------------------
void MMDL_MoveCopy_Move( MMDL * mmdl )
{
  MV_COPY_WORK *work = MMDL_GetMoveProcWork( mmdl );
  while( DATA_CopyMoveTbl[work->seq_no](mmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * MV_COPY ����@��������@����0
 * @param  mmdl  MMDL *
 * @param  work  MV_COPY_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_FirstInit( MMDL * mmdl, MV_COPY_WORK *work )
{
  int ret = MMDL_GetDirDisp( mmdl );
  ret = MMDL_ChangeDirAcmdCode( ret, AC_DIR_U );
  MMDL_SetLocalAcmd( mmdl, ret );
  MMDL_OffMoveBitMove( mmdl );
  MMDL_OffMoveBitMoveEnd( mmdl );
  work->seq_no = SEQNO_COPYMOVE_FIRST_WAIT;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPYU ����@��������@����1
 * @param  mmdl  MMDL *
 * @param  work  MV_COPY_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_FirstWait( MMDL * mmdl, MV_COPY_WORK *work )
{
  if( MMDL_ActionLocalAcmd(mmdl) == TRUE ){
    work->seq_no = SEQNO_COPYMOVE_INIT;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MV_COPY ���� 0
 * @param  mmdl  MMDL *
 * @param  work  MV_COPY_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_Init( MMDL * mmdl, MV_COPY_WORK *work )
{
  if( work->dir_jiki == DIR_NOT ){
    MMDL *jiki = MMDLSYS_SearchMMdlPlayer( MMDL_GetMMdlSys(mmdl) );
    if( jiki != NULL ){
      work->dir_jiki = MMDL_GetDirMove( jiki );
    }
  }
  
  MMDL_OffMoveBitMove( mmdl );
  MMDL_OffMoveBitMoveEnd( mmdl );
  work->seq_no = SEQNO_COPYMOVE_CMD_SET;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���̂܂ˑ��ړ��`�F�b�N
 * @param  mmdl  MMDL *
 * @param  dir    �ړ�����
 * @retval  u32    hit bit  
 */
//--------------------------------------------------------------
static u32 CopyMove_LongGrassHitCheck( MMDL * mmdl, int dir )
{
  u32 ret = MMDL_MOVEHITBIT_NON;
  MAPATTR attr = MMDL_GetMapDirAttr( mmdl, dir );
  MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
  
  if( MAPATTR_VALUE_CheckLongGrass(val) == FALSE ){
    ret = MMDL_MOVEHITBIT_ATTR;
  }
  
  ret |= MMDL_HitCheckMoveDir( mmdl, dir );
  return( ret );
}

//--------------------------------------------------------------
/**
 * �R�s�[���� �ړ��Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void CopyMove_MoveSet(
  MMDL * mmdl, int dir, int ac, u32 lgrass_on )
{
  u32 ret;
  
  if( lgrass_on == FALSE ){
    ret = MMDL_HitCheckMoveDir( mmdl, dir );
  }else{
    ret = CopyMove_LongGrassHitCheck( mmdl, dir );
  }
  
  if( ret != MMDL_MOVEHITBIT_NON ){
    ac = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
  }else{
    ac = MMDL_ChangeDirAcmdCode( dir, ac );
    MMDL_OnMoveBitMove( mmdl );
  }
  
  MMDL_SetLocalAcmd( mmdl, ac );
}

//--------------------------------------------------------------
/**
 * MV_COPY ���� 1
 * @param  mmdl  MMDL *
 * @param  work  MV_COPY_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_CmdSet( MMDL * mmdl, MV_COPY_WORK *work )
{
  int ret;
  MMDL *jiki = MMDLSYS_SearchMMdlPlayer( MMDL_GetMMdlSys(mmdl) );
  u16 dir = MMDL_GetDirMove( jiki );
#ifndef MMDL_PL_NULL //wb ���@���̑Ή����K�v
  u32 type = Player_AcmdTypeGet( fsys->player );
  
  switch( type ){
  case HEROACTYPE_NON:
  case HEROACTYPE_STOP:
    ret = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
    MMDL_SetLocalAcmd( mmdl, ret );
    break;
  case HEROACTYPE_WALK_32F:
    CopyMove_MoveSet( mmdl, dir, AC_WALK_U_32F, work->lgrass_on );
    break;
  case HEROACTYPE_WALK_16F:
    CopyMove_MoveSet( mmdl, dir, AC_WALK_U_16F, work->lgrass_on );
    break;
  case HEROACTYPE_WALK_8F:
    CopyMove_MoveSet( mmdl, dir, AC_WALK_U_8F, work->lgrass_on );
    break;
  case HEROACTYPE_WALK_4F:
    CopyMove_MoveSet( mmdl, dir, AC_WALK_U_4F, work->lgrass_on );
    break;
  case HEROACTYPE_WALK_2F:
    CopyMove_MoveSet( mmdl, dir, AC_WALK_U_2F, work->lgrass_on );
    break;
  }
#else
  ret = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
  MMDL_SetLocalAcmd( mmdl, ret );
#endif

  work->seq_no = SEQNO_COPYMOVE_CMD_WAIT;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * MV_COPY ���� 2
 * @param  mmdl  MMDL *
 * @param  work  MV_COPY_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int CopyMove_MoveWait( MMDL * mmdl, MV_COPY_WORK *work )
{
  if( MMDL_ActionLocalAcmd(mmdl) == FALSE ){
    return( FALSE );
  }
  
  MMDL_OffMoveBitMove( mmdl );
  MMDL_OffMoveBitMoveEnd( mmdl );
  work->seq_no = SEQNO_COPYMOVE_INIT;
  return( FALSE );
}

//--------------------------------------------------------------
///  �R�s�[����e�[�u��
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
//  �ǉ����ړ�
//======================================================================
static int AlongWall_WallMove(MMDL * mmdl,MV_ALONGW_WORK *work,int ac);
static int (* const DATA_AlongMoveTbl[3])(MMDL *, MV_ALONGW_WORK *);

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@���[�N������
 * @param  mmdl    MMDL *
 * @param  dir_h_init  ����������
 * @param  dir_h    ������
 * @retval  nothing
 */
//--------------------------------------------------------------
static void AlongWallWorkInit( MMDL * mmdl, ALONG_DIR dir_h_init, ALONG_DIR dir_h )
{
  MV_ALONGW_WORK *work;
  work = MMDL_InitMoveProcWork( mmdl, MV_ALONGW_WORK_SIZE );
  work->dir_hand_init = dir_h_init;
  work->dir_hand = dir_h;
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@����@������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWallL_Init( MMDL * mmdl )
{
  AlongWallWorkInit( mmdl, ALONG_L, ALONG_L );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@�E��@������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWallR_Init( MMDL * mmdl )
{
  AlongWallWorkInit( mmdl, ALONG_R, ALONG_R );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@���荶�@������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWallLRL_Init( MMDL * mmdl )
{
  AlongWallWorkInit( mmdl, ALONG_LR, ALONG_L );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@����E�@������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWallLRR_Init( MMDL * mmdl )
{
  AlongWallWorkInit( mmdl, ALONG_LR, ALONG_R );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@����
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AlongWall_Move( MMDL * mmdl )
{
  MV_ALONGW_WORK *work = MMDL_GetMoveProcWork( mmdl );
  while( DATA_AlongMoveTbl[work->seq_no](mmdl,work) == TRUE ){};
}

//--------------------------------------------------------------
/**
 * ���� ������
 * @param  mmdl  MMDL *
 * @param  work  MV_ALONGW_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int AlongWallMove_Init( MMDL * mmdl, MV_ALONGW_WORK *work )
{
  MMDL_OffMoveBitMove( mmdl );
  work->seq_no++;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���� ������
 * @param  mmdl  MMDL *
 * @param  work  MV_ALONGW_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int AlongWallMove_CmdSet( MMDL * mmdl, MV_ALONGW_WORK *work )
{
  AlongWall_WallMove( mmdl, work, AC_WALK_U_8F );
  work->seq_no++;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���� �R�}���h�I���҂�
 * @param  mmdl  MMDL *
 * @param  work  MV_ALONGW_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int AlongWallMove_CmdWait( MMDL * mmdl, MV_ALONGW_WORK *work )
{
  if( MMDL_ActionLocalAcmd(mmdl) == TRUE ){
    work->seq_no = 0;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
//  �Ǔ`���ړ��@����e�[�u��
//--------------------------------------------------------------
static int (* const DATA_AlongMoveTbl[3])( MMDL *, MV_ALONGW_WORK * ) =
{
  AlongWallMove_Init,
  AlongWallMove_CmdSet,
  AlongWallMove_CmdWait,
};

//--------------------------------------------------------------
///  ��̈ʒu
//--------------------------------------------------------------
static const int DATA_DirHandPosX[DIR_MAX4][2] = 
{
  { -1, 1 },  //up,left,right
  { 1, -1 },  //down,left,right
  { 0, 0 },  //left,left,right
  { 0, 0 },  //right,left,right
};

static const int DATA_DirHandPosZ[DIR_MAX4][2] =
{
  { 0, 0 },  //up,left,right
  { 0, 0 },  //down,left,right
  { 1, -1 },  //left,left,right
  { -1, 1 },  //right,left,right
};

//--------------------------------------------------------------
///  ���������ۂ̔���ʒu
//--------------------------------------------------------------
static const int DATA_DirHandLostPosX[DIR_MAX4][2] =
{
  { -1, 1 },  //up,left,right
  { 1, -1 },  //down,left,right
  { 1, 1  },  //left,left,right
  { -1, -1 },  //right,left,right
};

static const int DATA_DirHandLostPosZ[DIR_MAX4][2] =
{
  { 1, 1 },  //up,left,right
  { -1, -1 },  //down,left,right
  { 1, -1  },  //left,left,right
  { -1, 1 },  //right,left,right
};

//--------------------------------------------------------------
///  ���������ۂ̕����؂�ւ�
//--------------------------------------------------------------
static const int DATA_DirHandLostDir[DIR_MAX4][2] =
{
  { DIR_LEFT, DIR_RIGHT },  //up,left,right
  { DIR_RIGHT, DIR_LEFT },  //down,left,right
  { DIR_DOWN, DIR_UP  },  //left,left,right
  { DIR_UP, DIR_DOWN },  //right,left,right
};

//--------------------------------------------------------------
///  �ǏՓˎ��̕����؂�ւ�
//--------------------------------------------------------------
static const int DATA_DirHandAttrHitDir[DIR_MAX4][2] =
{
  { DIR_RIGHT, DIR_LEFT },  //up,left,right
  { DIR_LEFT, DIR_RIGHT },  //down,left,right
  { DIR_UP, DIR_DOWN  },  //left,left,right
  { DIR_DOWN, DIR_UP },  //right,left,right
};

//--------------------------------------------------------------
///  ��̂ނ����]
//--------------------------------------------------------------
static const ALONG_DIR DATA_DirHandFlip[ALONG_LR] =
{ ALONG_R, ALONG_L };
  
//--------------------------------------------------------------
/**
 * �ǉ����@�w������̕ǂ��E��
 * @param  fsys  FIELDSYS_WORK
 * @param  gx    �O���b�hX
 * @param  gz    �O���b�hZ
 * @param  dir    ����
 * @param  hdir  ������
 * @retval  BOOL  TRUE=�ǂ���
 */
//--------------------------------------------------------------
static BOOL AlongWall_HandHitGet(
  const MMDL *mmdl, const VecFx32 *pos, int dir, ALONG_DIR hdir )
{
  u32 attr;
  BOOL hit = FALSE;
  VecFx32 next = *pos;
  s16 gx = SIZE_GRID_FX32( pos->x );
  s16 gz = SIZE_GRID_FX32( pos->z );
  
  gx += DATA_DirHandPosX[dir][hdir];
  gz += DATA_DirHandPosZ[dir][hdir];
  next.x = GRID_SIZE_FX32( gx );
  next.z = GRID_SIZE_FX32( gz );
  
  if( MMDL_GetMapPosAttr(mmdl,&next,&attr) == TRUE ){
    if( MAPATTR_GetHitchFlag(attr) != FALSE ){
      hit = TRUE;
    }
  }
  
  return( hit );
}

//--------------------------------------------------------------
/**
 * �ǉ����@�Ǐ������̕ǌ���
 * @param  fsys  FIELDSYS_WORK
 * @param  gx    �O���b�hX
 * @param  gz    �O���b�hZ
 * @param  dir    ����
 * @param  hdir  ������
 * @retval  BOOL  TRUE=�ǂ���
 */
//--------------------------------------------------------------
static BOOL AlongWall_HandLostHitGet(
    const MMDL *mmdl, const VecFx32 *pos, int dir, ALONG_DIR hdir )
{
  u32 attr;
  BOOL hit = FALSE;
  VecFx32 next = *pos;
  s16 gx = SIZE_GRID_FX32( pos->x );
  s16 gz = SIZE_GRID_FX32( pos->z );
  gx += DATA_DirHandLostPosX[dir][hdir];
  gz += DATA_DirHandLostPosZ[dir][hdir];
  next.x = GRID_SIZE_FX32( gx );
  next.z = GRID_SIZE_FX32( gz );
  if( MMDL_GetMapPosAttr(mmdl,&next,&attr) == TRUE ){
    if( MAPATTR_GetHitchFlag(attr) != FALSE ){
      hit = TRUE;
    }
  }
  return( hit );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@��̈ʒu�ɕǂ͂��邩
 * @param  mmdl  MMDL *
 * @param  dir_hand  ALONG_DIR
 * @retval  int    TRUE=�ǃA��
 */
//--------------------------------------------------------------
static int AlongWall_HandWallCheck(
  const MMDL * mmdl, int dir, ALONG_DIR dir_hand )
{
  BOOL hit;
  VecFx32 pos;
  MMDL_GetVectorPos( mmdl, &pos );
  hit = AlongWall_HandHitGet( mmdl, &pos, dir, dir_hand );
  return( hit );
}

//--------------------------------------------------------------
/**
 * �ǉ����ړ��@�ǎ�T��
 * @param  mmdl  MMDL *
 * @param  dir_hand  ALONG_DIR
 * @retval  int    TRUE=�ǃA��
 */
//--------------------------------------------------------------
static int AlongWall_HandLostWallCheck(
  MMDL * mmdl, int dir, ALONG_DIR dir_hand )
{
  BOOL hit;
  VecFx32 pos;
  MMDL_GetVectorPos( mmdl, &pos );
  hit = AlongWall_HandLostHitGet( mmdl, &pos, dir, dir_hand );
  return( hit );
}

//--------------------------------------------------------------
/**
 * �ǈړ��`�F�b�N
 * @param  mmdl  MMDL *
 * @param  dir_hand  ALONG_DIR
 * @retval  int    �ړ�����ׂ�����
 */
//--------------------------------------------------------------
static int AlongWall_MoveHitCheck(
  MMDL * mmdl, int dir_move, ALONG_DIR dir_hand )
{
  if( AlongWall_HandWallCheck(mmdl,dir_move,dir_hand) == FALSE ){ //�ǂ�����
    if( AlongWall_HandLostWallCheck(mmdl,dir_move,dir_hand) == FALSE ){
      return( DIR_NOT ); //��T��ł��ǂ�����
    }
    
    //�ǔ��� �����؂�ւ�
    dir_move = DATA_DirHandLostDir[dir_move][dir_hand];
  }
  
  return( dir_move );
}

//--------------------------------------------------------------
/**
 * �Ǔ`���ړ��`�F�b�N
 * @param  mmdl  MMDL *
 * @param  dir_move  �ړ����� DIR_NOT=�ړ��s��
 * @param  dir_hand  �Ǒ��̎�̌���
 * @retval  u32    �q�b�g�r�b�g
 */
//--------------------------------------------------------------
static u32 AlongWall_WallMoveCheck(
  MMDL * mmdl, int *dir_move, ALONG_DIR dir_hand )
{
  u32 ret;
  
  *dir_move = AlongWall_MoveHitCheck( mmdl, *dir_move, dir_hand );
  
  if( *dir_move != DIR_NOT ){
    ret = MMDL_HitCheckMoveDir( mmdl, *dir_move );
    return( ret );
  }
  
  return( 0 );
}

//--------------------------------------------------------------
/**
 * �Ǔ`���ړ�
 * @param  mmdl  MMDL *
 * @param  work  MV_ALONGW_WORK
 * @retval  int    TRUE=�ړ�����
 */
//--------------------------------------------------------------
static int AlongWall_WallMove(
  MMDL * mmdl, MV_ALONGW_WORK *work, int ac )
{
  u32 ret;
  int dir_hand = work->dir_hand;
  int dir_move = MMDL_GetDirDisp( mmdl );
  
  ret = AlongWall_WallMoveCheck( mmdl, &dir_move, dir_hand );
  
  if( dir_move == DIR_NOT ){          //�ǂ��Ȃ�
    dir_move = MMDL_GetDirDisp( mmdl );
    ac = MMDL_ChangeDirAcmdCode( dir_move, AC_STAY_WALK_U_16F );
    MMDL_SetLocalAcmd( mmdl, ac );
    DEBUG_MMDL_PrintState( mmdl, "�ǂ��������Ă���", NULL );
    return( FALSE );
  }
  
  if( ret == MMDL_MOVEHITBIT_NON ){    //�ړ��\
    ac = MMDL_ChangeDirAcmdCode( dir_move, ac );
    MMDL_OnMoveBitMove( mmdl );
    MMDL_SetLocalAcmd( mmdl, ac );
    return( TRUE );
  }
  
  //�ړ������q�b�g&���藘���@���]�����݂�
  if( (ret & MMDL_MOVEHITBIT_LIM) && work->dir_hand_init == ALONG_LR ){
    dir_move = MMDL_TOOL_FlipDir( MMDL_GetDirDisp(mmdl) );
    dir_hand = DATA_DirHandFlip[dir_hand];
    work->dir_hand = dir_hand;
    
    ret = AlongWall_WallMoveCheck( mmdl, &dir_move, dir_hand );
    
    if( dir_move == DIR_NOT ){          //�ǂ��Ȃ�
      dir_move = MMDL_GetDirDisp( mmdl );
      ac = MMDL_ChangeDirAcmdCode( dir_move, AC_STAY_WALK_U_16F );
      MMDL_SetLocalAcmd( mmdl, ac );
      DEBUG_MMDL_PrintState( mmdl, "�ǂ��������Ă��� ���藘��", NULL );
      return( FALSE );
    }
    
    if( ret == MMDL_MOVEHITBIT_NON ){    //�ړ��\
      ac = MMDL_ChangeDirAcmdCode( dir_move, ac );
      MMDL_OnMoveBitMove( mmdl );
      MMDL_SetLocalAcmd( mmdl, ac );
      return( TRUE );
    }
  }
  
  //�ǃq�b�g�@�ړ������ύX
  if( (ret & MMDL_MOVEHITBIT_ATTR) ){
    dir_move = DATA_DirHandAttrHitDir[dir_move][dir_hand];
    
    ret = AlongWall_WallMoveCheck( mmdl, &dir_move, dir_hand );
    
    if( dir_move == DIR_NOT ){          //�ǂ�����
      dir_move = MMDL_GetDirDisp( mmdl );
      ac = MMDL_ChangeDirAcmdCode( dir_move, AC_STAY_WALK_U_16F );
      MMDL_SetLocalAcmd( mmdl, ac );
      DEBUG_MMDL_PrintState( mmdl, "�ǂ��������Ă��� �ǃq�b�g", NULL );
      return( FALSE );
    }
    
    if( ret == MMDL_MOVEHITBIT_NON ){    //�ړ��\
      ac = MMDL_ChangeDirAcmdCode( dir_move, ac );
      MMDL_OnMoveBitMove( mmdl );
      MMDL_SetLocalAcmd( mmdl, ac );
      return( TRUE );
    }
  }
  
  //�ړ��s��
  dir_move = MMDL_GetDirDisp( mmdl );  //������߂�
  ac = MMDL_ChangeDirAcmdCode( dir_move, AC_STAY_WALK_U_16F );
  MMDL_SetLocalAcmd( mmdl, ac );
  DEBUG_MMDL_PrintState( mmdl, "���S�Ɉړ��s��", NULL );
  return( FALSE );
}
