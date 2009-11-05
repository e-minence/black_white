//======================================================================
/**
 * @file  event_trainer_eye.c
 * @brief  �g���[�i�[�����C�x���g
 * @author  kagaya
 * @date  05.10.03
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"

#include "fieldmap.h"
#include "fldmmdl.h"
#include "script.h"
#include "fldeff_gyoe.h"
#include "event_trainer_eye.h"

//SCRID_TRAINER_MOVE_BATTLE 
#include "../../../resource/fldmapdata/script/trainer_def.h"

#include "script_def.h"
//======================================================================
//  define
//======================================================================
#define EYE_CHECK_NOHIT (-1) ///<�����͈̓`�F�b�N �q�b�g����

#define EYE_MEET_GYOE_END_WAIT (30) ///<!�}�[�N�o����̊ԁ@�t���[���P��
#define EYE_MEET_MOVE_END_WAIT (8) ///<�����ړ���̊ԁ@�t���[���P��

//--------------------------------------------------------------
///  �����ړ��C�x���g�����ԍ�
//--------------------------------------------------------------
enum
{
  SEQNO_TRMOVE_INIT = 0,
  SEQNO_TRMOVE_OBJMOVE_WAIT,     ///<�����~�҂�
  SEQNO_TRMOVE_JIKIMOVE_WAIT,    ///<���@����I���҂�
  SEQNO_TRMOVE_DIR_CHANGE,       ///<�����ύX
  SEQNO_TRMOVE_DIR_CHANGE_WAIT,  ///<�����ύX�҂�
  SEQNO_TRMOVE_GYOE_SET,         ///<!�Z�b�g
  SEQNO_TRMOVE_GYOE_WAIT,        ///<!�҂�
  SEQNO_TRMOVE_HIDE_PULLOFF_SET, ///<�B�ꖪ�E���Z�b�g
  SEQNO_TRMOVE_HIDE_PULLOFF_WAIT,///<�B�ꖪ�I���҂�
  SEQNO_TRMOVE_GYOE_END_WAIT,    ///<!�I����҂�
  SEQNO_TRMOVE_MOVE_RANGE_CHECK, ///<�ړ������`�F�b�N
  SEQNO_TRMOVE_MOVE_START,       ///<�ړ��J�n
  SEQNO_TRMOVE_MOVE,             ///<�ړ���
  SEQNO_TRMOVE_MOVE_END_WAIT,    ///<�ړ��I����҂�
  SEQNO_TRMOVE_JIKI_TURN_SET,    ///<���@�U������Z�b�g
  SEQNO_TRMOVE_JIKI_TURN,        ///<���@�U�����
  SEQNO_TRMOVE_ACMD_END,         ///<�A�j���I��
  SEQNO_TRMOVE_END,              ///<�I��
};

//======================================================================
//  strcut
//======================================================================
//--------------------------------------------------------------
///  �����q�b�g�i�[
//--------------------------------------------------------------
typedef struct
{
  int range;
  int dir;
  int scr_id;
  int tr_id;
  int tr_type;
  MMDL *mmdl;
}EYEMEET_HITDATA;

//--------------------------------------------------------------
//	�����q�b�g�ړ������p���[�N
//--------------------------------------------------------------
typedef struct
{
	int seq_no;				///<�����ԍ�
	BOOL end_flag;			///<�I���t���O
   
	int dir;					///<�ړ�����
	int range;				///<�ړ�����
	int gyoe_type;		///<!�^�C�v
	int tr_type;			///<�g���[�i�[�^�C�v
	int sisen_no;			///<�����ԍ�
	int count;				///<�ړ��J�E���g
   
  FLDEFF_TASK *task_gyoe;   ////<�M���G�[�^�X�N
	MMDL *mmdl;								///<�ړ����s��MMDL*
  FIELDMAP_WORK *fieldMap;  ///<FILEDMAP_WORK*
  FIELD_PLAYER *fieldPlayer; ///<FIELD_PLAYER*
}EV_EYEMEET_MOVE_WORK;

//======================================================================
//  proto
//======================================================================
static BOOL treye_CheckEyeMeet(
    FIELDMAP_WORK *fieldMap, MMDL *non_mmdl, EYEMEET_HITDATA *hit );
static int treye_CheckEyeRange(
    const MMDL *mmdl, const MMDL *jiki, u16 *hitDir );

static int treye_CheckEyeLine(
    u16 dir, s16 range, s16 tx, s16 ty, s16 tz, s16 gx, s16 gy, s16 gz );

static GMEVENT_RESULT treyeEvent_EyeMeetMove(
    GMEVENT *ev, int *seq, void *wk );

static u16 tr_GetTrainerEventType( const MMDL *mmdl );
static BOOL tr_HitCheckEyeLine( const MMDL *mmdl, u16 dir, int range );
static u16 tr_GetTrainerID( const MMDL *mmdl );
static BOOL tr_CheckEventFlag( FIELDMAP_WORK *fieldMap, const MMDL *mmdl );
static void tr_InitEyeMeetHitData(
    EYEMEET_HITDATA *data, MMDL *mmdl, int range, u16 dir );
static MMDL * tr_CheckPairTrainer( const MMDL *tr_mmdl, u16 tr_id );
static GMEVENT * tr_SetEventScript( FIELDMAP_WORK *fieldMap, MMDL *mmdl );
static void trEventScript_SetTrainerEyeData(
    GMEVENT *event, const EYEMEET_HITDATA *data, int tr_type, int tr_num );

static int (* const data_treye_CheckEyeLineTbl[DIR_MAX4])(
    s16,s16,s16,s16,s16,s16,s16);

//======================================================================
//  �g���[�i�[�����C�x���g
//======================================================================
//--------------------------------------------------------------
/**
 * �g���[�i�[�����C�x���g�`�F�b�N
 * @param fieldMap FIELDMAP_WORK*
 * @param vs2 �_�u���o�g���\�� TRUE=�\ FALSE=�s��
 * @retval GMEVENT NULL=�q�b�g����
 */
//--------------------------------------------------------------
GMEVENT * EVENT_CheckTrainerEye( FIELDMAP_WORK *fieldMap, BOOL vs2 )
{
  EYEMEET_HITDATA hit0;
  GMEVENT *event = NULL;
  
  if( treye_CheckEyeMeet(fieldMap,NULL,&hit0) == TRUE )
  {
    MMDL *mmdl;
    EYEMEET_HITDATA hit1;
    
    if( hit0.tr_type == SCR_EYE_TR_TYPE_SINGLE ) //�V���O��
    {
      event = tr_SetEventScript( fieldMap, hit0.mmdl ); //�X�N���v�g�N��

      if( vs2 == FALSE || //�V���O����`�F�b�N
          treye_CheckEyeMeet(fieldMap,hit0.mmdl,&hit1) == FALSE )
      { //�X�N���v�g�p�C�x���g�f�[�^�Z�b�g �V���O��
        trEventScript_SetTrainerEyeData(
            event, &hit0, SCR_EYE_TR_TYPE_SINGLE, SCR_EYE_TR_0 );
        KAGAYA_Printf( "TRAINER EYE HIT SINGLE\n" );
      }
      else
      { //�X�N���v�g�p�C�x���g�f�[�^�Z�b�g �^�b�O
        trEventScript_SetTrainerEyeData(
            event, &hit0, SCR_EYE_TR_TYPE_TAG, SCR_EYE_TR_0 );
        trEventScript_SetTrainerEyeData(
            event, &hit1, SCR_EYE_TR_TYPE_TAG, SCR_EYE_TR_1 );
        KAGAYA_Printf( "TRAINER EYE HIT TAG DOUBLE\n" );
      }
    }
    else if( hit0.tr_type == SCR_EYE_TR_TYPE_DOUBLE )
    {
      if( vs2 == TRUE ) //�_�u���\
      {
        event = tr_SetEventScript( fieldMap, hit0.mmdl ); //�X�N���v�g�N��
        
        mmdl = tr_CheckPairTrainer( hit0.mmdl, hit0.tr_id );
        tr_InitEyeMeetHitData( &hit1, mmdl, hit0.range, hit0.dir );
        
        //�X�N���v�g�p�C�x���g�f�[�^�Z�b�g �_�u��
        trEventScript_SetTrainerEyeData(
            event, &hit0, SCR_EYE_TR_TYPE_DOUBLE, SCR_EYE_TR_0 );
        trEventScript_SetTrainerEyeData(
            event, &hit1, SCR_EYE_TR_TYPE_DOUBLE, SCR_EYE_TR_1 );
        KAGAYA_Printf( "TRAINER EYE HIT DOUBLE\n" );
      }
    }
    else //�Ή����Ă��Ȃ��g���[�i�[�^�C�v
    {
      GF_ASSERT( 0 && "TRAINER EYE TYPE ERROR" );
    }
  }
  
  return( event );
}

//======================================================================
//  �����`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �g���[�i�[�����`�F�b�N
 * @param fieldMap FIELDMAP_WORK
 * @param non_mmdl �����`�F�b�N�̍ۂɖ�������MMDL NULL=����
 * @param hit �����q�b�g���̏��i�[��
 * @retval BOOL TRUE=�q�b�g
 */
//--------------------------------------------------------------
static BOOL treye_CheckEyeMeet(
    FIELDMAP_WORK *fieldMap, MMDL *non_mmdl, EYEMEET_HITDATA *hit )
{
  u16 dir;
  u32 no=0;
  int range=EYE_CHECK_NOHIT;
  MMDL *mmdl = NULL;
  MMDL *jiki = FIELD_PLAYER_GetMMdl( FIELDMAP_GetFieldPlayer(fieldMap) );
  MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldMap );
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE )
  {
    if( non_mmdl == NULL || (non_mmdl != mmdl) )
    {
      range = treye_CheckEyeRange( mmdl, jiki, &dir );
      
      if( range != EYE_CHECK_NOHIT )
      {
        if( tr_CheckEventFlag(fieldMap,mmdl) == FALSE )
        {
#ifdef DEBUG_ONLY_FOR_kagaya
          KAGAYA_Printf( "�����q�b�g �q�b�g�����g���[�i�[ID =%d\n",
              tr_GetTrainerID(mmdl) );
#endif
          tr_InitEyeMeetHitData( hit, mmdl, range, dir );
          return( TRUE );
        }
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�����͈̓`�F�b�N
 * @param mmdl MMDL*
 * @param jiki ���@MMDL
 * @param hitDir 
 * @retval int �����q�b�g���̎��@�܂ł̃O���b�h���BEYE_CHECK_NOHIT=�O��B
 */
//--------------------------------------------------------------
static int treye_CheckEyeRange(
    const MMDL *mmdl, const MMDL *jiki, u16 *hitDir )
{
  u16 type;
  
  type = tr_GetTrainerEventType( mmdl );
  
  if( type == EV_TYPE_TRAINER || type == EV_TYPE_TRAINER_EYEALL )
  {
    int ret;
    u16 dir;
    s16 tx = MMDL_GetGridPosX( mmdl );
    s16 ty = MMDL_GetGridPosY( mmdl );
    s16 tz = MMDL_GetGridPosZ( mmdl );
    s16 jx = MMDL_GetGridPosX( jiki );
    s16 jy = MMDL_GetGridPosY( jiki );
    s16 jz = MMDL_GetGridPosZ( jiki );
    s16 range = MMDL_GetParam( mmdl, MMDL_PARAM_0 );
    
    if( type == EV_TYPE_TRAINER )
    {
      dir = MMDL_GetDirDisp( mmdl );
      ret = treye_CheckEyeLine( dir, range, tx, ty, tz, jx, jy, jz );
      
      if( ret != EYE_CHECK_NOHIT )
      {
        if( tr_HitCheckEyeLine(mmdl,dir,ret) == FALSE ){
          *hitDir = dir;
          return( ret );
        }
      }
    }
    else //EV_TYPE_TRAINER_EYEALL
    {
      dir = DIR_UP;
      
      do{
        ret = treye_CheckEyeLine( dir, range, tx, ty, tz, jx, jy, jz );
        
        if( ret != EYE_CHECK_NOHIT )
        {
          if( tr_HitCheckEyeLine(mmdl,dir,ret) == FALSE ){
            *hitDir = dir;
            return( ret );
          }
        }
        
        dir++;
      }while( dir < DIR_MAX4 );
    }
  }

  return( EYE_CHECK_NOHIT );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�����`�F�b�N
 * @param mmdl MMDL
 * @param dir �������� DIR_UP��
 * @param range ��������
 * @param x ���@�O���b�hX���W
 * @param y ���@�O���b�hY���W
 * @param z ���@�O���b�hZ���W
 * @retval int ���@�ւ̃O���b�h�����BEYE_CHECK_NOHIT=�O��
 */
//--------------------------------------------------------------
static int treye_CheckEyeLine(
    u16 dir, s16 range, s16 tx, s16 ty, s16 tz, s16 gx, s16 gy, s16 gz )
{
  int hit;
  GF_ASSERT( dir < DIR_MAX4 );
  hit = data_treye_CheckEyeLineTbl[dir]( range, tx, ty, tz, gx, gy, gz );
  return( hit );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�����`�F�b�N ��
 * @param range ��������
 * @param tx �g���[�i�[�O���b�hX���W
 * @param ty �g���[�i�[�O���b�hY���W
 * @param tz �g���[�i�[�O���b�hZ���W
 * @param jx ���@�O���b�hX���W
 * @param jy ���@�O���b�hY���W
 * @param jz ���@�O���b�hZ���W
 * @retval int ���@�ւ̃O���b�h�����BEYE_CHECK_NOHIT=�O��
 */
//--------------------------------------------------------------
static int treye_CheckEyeLineUp(
    s16 range, s16 tx, s16 ty, s16 tz, s16 jx, s16 jy, s16 jz )
{
  if( tx == jx && ty == jy ){
    if( jz < tz && jz >= (tz-range) ){
      return( tz-jz );
    }
  }
  return( EYE_CHECK_NOHIT );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�����`�F�b�N ��
 * @param range ��������
 * @param tx �g���[�i�[�O���b�hX���W
 * @param ty �g���[�i�[�O���b�hY���W
 * @param tz �g���[�i�[�O���b�hZ���W
 * @param jx ���@�O���b�hX���W
 * @param jy ���@�O���b�hY���W
 * @param jz ���@�O���b�hZ���W
 * @retval int ���@�ւ̃O���b�h�����BEYE_CHECK_NOHIT=�O��
 */
//--------------------------------------------------------------
static int treye_CheckEyeLineDown(
    s16 range, s16 tx, s16 ty, s16 tz, s16 jx, s16 jy, s16 jz )
{
  if( tx == jx && ty == jy ){
    if( jz > tz && jz <= (tz+range) ){
      return( jz - tz );
    }
  }
  return( EYE_CHECK_NOHIT );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�����`�F�b�N ��
 * @param range ��������
 * @param tx �g���[�i�[�O���b�hX���W
 * @param ty �g���[�i�[�O���b�hY���W
 * @param tz �g���[�i�[�O���b�hZ���W
 * @param jx ���@�O���b�hX���W
 * @param jy ���@�O���b�hY���W
 * @param jz ���@�O���b�hZ���W
 * @retval int ���@�ւ̃O���b�h�����BEYE_CHECK_NOHIT=�O��
 */
//--------------------------------------------------------------
static int treye_CheckEyeLineLeft(
    s16 range, s16 tx, s16 ty, s16 tz, s16 jx, s16 jy, s16 jz )
{
  if( tz == jz && ty == jy ){
    if( jx < tx && jx >= (tx-range) ){
      return( tx - jx );
    }
  }
  return( EYE_CHECK_NOHIT );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�����`�F�b�N �E
 * @param range ��������
 * @param tx �g���[�i�[�O���b�hX���W
 * @param ty �g���[�i�[�O���b�hY���W
 * @param tz �g���[�i�[�O���b�hZ���W
 * @param jx ���@�O���b�hX���W
 * @param jy ���@�O���b�hY���W
 * @param jz ���@�O���b�hZ���W
 * @retval int ���@�ւ̃O���b�h�����BEYE_CHECK_NOHIT=�O��
 */
//--------------------------------------------------------------
static int treye_CheckEyeLineRight(
    s16 range, s16 tx, s16 ty, s16 tz, s16 jx, s16 jy, s16 jz )
{
  if( tz == jz ){
    if( jx > tx && jx <= (tx+range) ){
      return( jx - tx );
    }
  }
  return( EYE_CHECK_NOHIT );
}

//======================================================================
//  �����q�b�g�ɂ��ړ��C�x���g
//======================================================================
//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�Z�b�g
 * @param  fsys  FIELDSYS_WORK
 * @param  fldobj  FIELD_OBJ_PTR
 * @param  jiki  PLAYER_STATE_PTR
 * @param  dir    �ړ�����
 * @param  range  �ړ�����
 * @param  gyoe  !�}�[�N���� ���󖢎g�p
 * @param  tr_type  �g���[�i�[�^�C�v
 * @param  work_pos �����q�b�g���[�N�v�f�� 0=������ 1=�y�A
 * @retval  GMEVENT �ړ��������s��GMEVENT
 */
//--------------------------------------------------------------
GMEVENT * EVENT_SetTrainerEyeMove( FIELDMAP_WORK *fieldMap,
    MMDL *mmdl, FIELD_PLAYER *jiki,
    int dir, int range, int gyoe, int tr_type, int work_pos )
{
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldMap );
  GMEVENT *ev = GMEVENT_Create( gsys, NULL,
      treyeEvent_EyeMeetMove, sizeof(EV_EYEMEET_MOVE_WORK) );
  EV_EYEMEET_MOVE_WORK *work = GMEVENT_GetEventWork( ev );
  MI_CpuClear8( work, sizeof(EV_EYEMEET_MOVE_WORK) );
  
  work->dir = dir;
  work->range = range;
  work->gyoe_type = gyoe;
  work->tr_type = tr_type;
  work->sisen_no = work_pos;
  work->mmdl = mmdl;
  work->fieldMap = fieldMap;
  work->fieldPlayer = FIELDMAP_GetFieldPlayer( fieldMap );
  
  KAGAYA_Printf( "OBJ ID %d�̎����ړ����Z�b�g\n", MMDL_GetOBJID(mmdl) );
  return( ev );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@������
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_Init( EV_EYEMEET_MOVE_WORK *work )
{
  if( MMDL_CheckStatusBitMove(work->mmdl) == TRUE ){ //���쒆
    MMDL_OffStatusBitMoveProcPause( work->mmdl ); //����|�[�Y����
  }
  
  work->seq_no = SEQNO_TRMOVE_OBJMOVE_WAIT;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@OBJ����I���҂�
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_OBJMoveWait( EV_EYEMEET_MOVE_WORK *work )
{
  if( MMDL_CheckStatusBitMove(work->mmdl) == TRUE ){
    return( FALSE ); //���쒆
  }
  
  MMDL_SetDirDisp( work->mmdl, work->dir );
  MMDL_OnStatusBitMoveProcPause( work->mmdl );
  
  work->seq_no = SEQNO_TRMOVE_JIKIMOVE_WAIT;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@���@�ړ��I���҂�
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_JikiMoveWait( EV_EYEMEET_MOVE_WORK *work )
{
  MMDL *j_mmdl = FIELD_PLAYER_GetMMdl( work->fieldPlayer );
  
  if( MMDL_CheckEndAcmd(j_mmdl) == FALSE ){
    return( FALSE );
  }
  
  work->seq_no = SEQNO_TRMOVE_DIR_CHANGE;

  {
    u32 code = MMDL_GetMoveCode( work->mmdl );
    KAGAYA_Printf( "�g���[�i�[����R�[�h=0x%x\n", code );
    
    switch( code )
    {
    case MV_HIDE_SNOW:
    case MV_HIDE_SAND:
    case MV_HIDE_GRND:
    case MV_HIDE_KUSA:
      work->seq_no = SEQNO_TRMOVE_HIDE_PULLOFF_SET;
    }
  }
   
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�ړ���֌�����ς��� 
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_DirChange( EV_EYEMEET_MOVE_WORK *work )
{
  
  if( MMDL_CheckPossibleAcmd(work->mmdl) == TRUE ){
    u16 code;
    GF_ASSERT( work->dir < DIR_MAX4 );
    code = MMDL_ChangeDirAcmdCode( work->dir, AC_DIR_U );
    MMDL_SetAcmd( work->mmdl, code );
    work->seq_no = SEQNO_TRMOVE_DIR_CHANGE_WAIT;
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�ړ���֌�����ς���@�҂�
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_DirChangeWait( EV_EYEMEET_MOVE_WORK *work )
{
  if( MMDL_CheckEndAcmd(work->mmdl) == FALSE ){
    return( FALSE );
  }
  
  work->seq_no = SEQNO_TRMOVE_GYOE_SET;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�т�����}�[�N�o��
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_GyoeSet( EV_EYEMEET_MOVE_WORK *work )
{
  FLDEFF_CTRL *fectrl =  FIELDMAP_GetFldEffCtrl( work->fieldMap );
  work->task_gyoe = FLDEFF_GYOE_SetMMdl(
      fectrl, work->mmdl, FLDEFF_GYOETYPE_GYOE, FALSE ); //SE�͖炳�Ȃ�
  work->seq_no = SEQNO_TRMOVE_GYOE_WAIT;
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�т�����}�[�N�\���I���҂�
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_GyoeWait( EV_EYEMEET_MOVE_WORK *work )
{
  if( FLDEFF_GYOE_CheckEnd(work->task_gyoe) == TRUE ){
    FLDEFF_TASK_CallDelete( work->task_gyoe );
    work->seq_no = SEQNO_TRMOVE_GYOE_END_WAIT;
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�B�ꖪ�E��
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_HidePullOFFSet( EV_EYEMEET_MOVE_WORK *work )
{
#if 0 //pl null
  FieldOBJ_AcmdSet( work->fldobj, AC_HIDE_PULLOFF );
#else //wb kari
#endif
  work->seq_no = SEQNO_TRMOVE_HIDE_PULLOFF_WAIT;
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�B�ꖪ�E���I���҂�
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_HidePullOFFWait( EV_EYEMEET_MOVE_WORK *work )
{
#if 0 //pl null
  if( FieldOBJ_AcmdEndCheck(work->fldobj) == TRUE ){
    work->seq_no = SEQNO_TRMOVE_GYOE_END_WAIT;
  }
#else //wb kari
  work->seq_no = SEQNO_TRMOVE_GYOE_END_WAIT;
#endif
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�т�����}�[�N�\���I����̊�
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_GyoeEndWait( EV_EYEMEET_MOVE_WORK *work )
{
  work->count++;
  
  if( work->count >= EYE_MEET_GYOE_END_WAIT ){
    work->count = 0;
    work->seq_no = SEQNO_TRMOVE_MOVE_RANGE_CHECK;
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�ړ������`�F�b�N
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_MoveRangeCheck( EV_EYEMEET_MOVE_WORK *work )
{
  if( work->range <= 1 ){                  //���@�ڂ̑O �ړ�����K�v�Ȃ�
    work->seq_no = SEQNO_TRMOVE_MOVE_END_WAIT;
    return( TRUE );  
  }

  work->seq_no = SEQNO_TRMOVE_MOVE_START;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�ړ��J�n
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_MoveStart( EV_EYEMEET_MOVE_WORK *work )
{
  if( MMDL_CheckPossibleAcmd(work->mmdl) == TRUE ){
    u16 code = MMDL_ChangeDirAcmdCode( work->dir, AC_WALK_U_8F );
    MMDL_SetAcmd( work->mmdl, code );
    work->seq_no = SEQNO_TRMOVE_MOVE;
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�ړ���
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_Move( EV_EYEMEET_MOVE_WORK *work )
{
  if( MMDL_CheckEndAcmd(work->mmdl) == FALSE ){
    return( FALSE );
  }
  
  work->range--;                      //������炷
  work->seq_no = SEQNO_TRMOVE_MOVE_RANGE_CHECK;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�ړ��I����A���@�̕����֌�������
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_MoveEndWait( EV_EYEMEET_MOVE_WORK *work )
{
  work->count++;
  
  if( work->count < EYE_MEET_MOVE_END_WAIT ){
    return( FALSE );
  }
  
  work->count = 0;
  work->seq_no = SEQNO_TRMOVE_JIKI_TURN_SET;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@���@��U���������
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_JikiTurnSet( EV_EYEMEET_MOVE_WORK *work )
{
  u16 turn_dir;
  MMDL *j_mmdl;
  
  j_mmdl = FIELD_PLAYER_GetMMdl( work->fieldPlayer );
  turn_dir = MMDL_TOOL_GetRangeDir(
      MMDL_GetGridPosX(j_mmdl), MMDL_GetGridPosZ(j_mmdl),
      MMDL_GetGridPosX(work->mmdl), MMDL_GetGridPosZ(work->mmdl) );
  
  if( MMDL_GetDirDisp(j_mmdl) != turn_dir &&
    (work->sisen_no == 0 || work->tr_type == SCR_EYE_TR_TYPE_TAG) )
  {
    if( MMDL_CheckPossibleAcmd(j_mmdl) == TRUE )
    {
      u16 code = MMDL_ChangeDirAcmdCode( turn_dir, AC_DIR_U );
//    MMDL_OnStatusBit( j_mmdl, MMDL_STABIT_PAUSE_DIR );
      MMDL_SetAcmd( j_mmdl, code );
      work->seq_no = SEQNO_TRMOVE_JIKI_TURN;
    }
  }
  else
  {
    work->seq_no = SEQNO_TRMOVE_ACMD_END;
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@���@�U������I���҂�
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_JikiTurn( EV_EYEMEET_MOVE_WORK *work )
{
  MMDL *j_mmdl = FIELD_PLAYER_GetMMdl( work->fieldPlayer );
  
  if( MMDL_CheckEndAcmd(j_mmdl) == FALSE ){
    return( FALSE );
  }
  
  MMDL_EndAcmd( j_mmdl );
  work->seq_no = SEQNO_TRMOVE_ACMD_END;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�A�j���I��
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_AcmdEnd( EV_EYEMEET_MOVE_WORK *work )
{
  MMDL_EndAcmd( work->mmdl );

#if 0 //pl null �S�[�X�g�W���p���ꏈ��
  if( GYM_GimmickCodeCheck(work->fsys, FLD_GIMMICK_GHOST_GYM) == FALSE 
      || GYM_PlGhostGymTrainerMoveCodeChange(work->fsys,work->fldobj) == FALSE ){
    FieldOBJ_MoveCodeChange( work->fldobj, MV_DMY );
  }
#endif
  
  work->seq_no = SEQNO_TRMOVE_END;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����ړ��C�x���g�@�I��
 * @param  work  EV_EYEMEET_MOVE_WORK
 * @retval  int    TRUE=�ċA
 */
//--------------------------------------------------------------
static int eyeMeetMove_End( EV_EYEMEET_MOVE_WORK *work )
{
  work->end_flag = TRUE;
  return( FALSE );
}

//--------------------------------------------------------------
///  �����ړ��C�x���g�����e�[�u��
//--------------------------------------------------------------
static int (* const data_EyeMeetMoveFuncTbl[])( EV_EYEMEET_MOVE_WORK* ) =
{
  eyeMeetMove_Init,
  eyeMeetMove_OBJMoveWait,
  eyeMeetMove_JikiMoveWait,
  eyeMeetMove_DirChange,
  eyeMeetMove_DirChangeWait,
  eyeMeetMove_GyoeSet,
  eyeMeetMove_GyoeWait,
  eyeMeetMove_HidePullOFFSet,
  eyeMeetMove_HidePullOFFWait,
  eyeMeetMove_GyoeEndWait,
  eyeMeetMove_MoveRangeCheck,
  eyeMeetMove_MoveStart,
  eyeMeetMove_Move,
  eyeMeetMove_MoveEndWait,
  eyeMeetMove_JikiTurnSet,
  eyeMeetMove_JikiTurn,
  eyeMeetMove_AcmdEnd,
  eyeMeetMove_End,
};

//--------------------------------------------------------------
/**
 * �����ړ������@TCB
 * @param  tcb    TCB_PTR
 * @param  wk    TCB work
 * @retval  nothing
 */
//--------------------------------------------------------------
static GMEVENT_RESULT treyeEvent_EyeMeetMove(
    GMEVENT *ev, int *seq, void *wk )
{
  EV_EYEMEET_MOVE_WORK *work = wk;
  
  while( data_EyeMeetMoveFuncTbl[work->seq_no](work) == TRUE ){};
  
  if( work->end_flag == TRUE ){
    return( GMEVENT_RES_FINISH );
  }
  
  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  �p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �Ώۃg���[�i�[�^�C�v���擾 
 * @param mmdl �g���[�i�[MMDL
 * @retval u16 EV_TYPE_TRAINER
 */
//--------------------------------------------------------------
static u16 tr_GetTrainerEventType( const MMDL *mmdl )
{
  u16 type = MMDL_GetEventType( mmdl );
  
  switch( type ){
  case EV_TYPE_TRAINER_KYORO:
  case EV_TYPE_TRAINER_SPIN_STOP_L:
  case EV_TYPE_TRAINER_SPIN_STOP_R:
  case EV_TYPE_TRAINER_SPIN_MOVE_L:
  case EV_TYPE_TRAINER_SPIN_MOVE_R:
    type = EV_TYPE_TRAINER;
    break;
  }
  
  return( type );
}

//--------------------------------------------------------------
/**
 * �����͈͈ړ�����
 * @param  mmdl MMDL
 * @param  dir      �ړ�����
 * @param  range    �ړ�����
 * @retval  int     TRUE=�ړ��s��
 */
//--------------------------------------------------------------
static BOOL tr_HitCheckEyeLine( const MMDL *mmdl, u16 dir, int range )
{
  s16 gx,gy,gz;
  u32 ret;
  
  if( range ) //�����W0=�ڂ̑O
  {
    int i = 0;
    u32 hit;
    s16 gx = MMDL_GetGridPosX( mmdl );
    s16 gy = MMDL_GetGridPosY( mmdl );
    s16 gz = MMDL_GetGridPosZ( mmdl );
    
    gx += MMDL_TOOL_GetDirAddValueGridX( dir );
    gz += MMDL_TOOL_GetDirAddValueGridZ( dir );
    
    for( ; i < (range-1); i++ ){
      hit = MMDL_HitCheckMoveCurrent( mmdl, gx, gy, gz, dir );
      hit &= ~MMDL_MOVEHITBIT_LIM; //�ړ������𖳎�����
      
      if( hit ){ //�ړ������ȊO�Ńq�b�g
        return( TRUE );
      }

      gx += MMDL_TOOL_GetDirAddValueGridX( dir );
      gz += MMDL_TOOL_GetDirAddValueGridZ( dir );
    }
    
    hit = MMDL_HitCheckMoveCurrent( mmdl, gx, gy, gz, dir ); //�Ō�
    hit &= ~MMDL_MOVEHITBIT_LIM; //�ړ������𖳎�����
    
    if( hit != MMDL_MOVEHITBIT_OBJ ){ //OBJ�Փ�(���@)�̂�
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���샂�f������g���[�i�[ID�擾
 * @param mmdl MMDL*
 * @retval u16 �g���[�i�[ID
 */
//--------------------------------------------------------------
static u16 tr_GetTrainerID( const MMDL *mmdl )
{
  u16 scr_id = MMDL_GetEventID( mmdl );
  return( SCRIPT_GetTrainerID_ByScriptID(scr_id) );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�C�x���g�t���O�`�F�b�N
 * @param fieldMap FIELDMAP_WORK
 * @param mmdl �g���[�i�[MMDL
 * @retval BOOL TRUE=�t���OON FALSE=�t���OOFF
 */
//--------------------------------------------------------------
static BOOL tr_CheckEventFlag( FIELDMAP_WORK *fieldMap, const MMDL *mmdl )
{
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldMap );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  u16 id = tr_GetTrainerID( mmdl );
  return( SCRIPT_CheckEventFlagTrainer(evwork,id) );
}

//--------------------------------------------------------------
/**
 * EYEMEET_HITDATA������
 * @param data EYEMEET_HITDATA
 * @param mmdl MMDL
 * @param range ��������
 * @param dir �ړ�����
 * @retval nothing
 */
//--------------------------------------------------------------
static void tr_InitEyeMeetHitData(
    EYEMEET_HITDATA *data, MMDL *mmdl, int range, u16 dir )
{
  data->range = range;
  data->dir = dir;
  data->scr_id = MMDL_GetEventID( mmdl );
  data->tr_id = tr_GetTrainerID( mmdl );
  data->tr_type = SCRIPT_CheckTrainer2vs2Type( data->tr_id );
  data->mmdl = mmdl;
}

//--------------------------------------------------------------
/**
 * ����̃g���[�i�[ID����������T��
 * @param mmdl ������T���Ă���MMDL*
 * @param tr_id �g���[�i�[ID
 * @retval MMDL ����MMDL* ������Ȃ��ꍇ��ASSERT
 */
//--------------------------------------------------------------
static MMDL * tr_CheckPairTrainer( const MMDL *tr_mmdl, u16 tr_id )
{
  u16 type;
  MMDL *mmdl;
  u32 no = 0;
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( tr_mmdl );
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( mmdl != tr_mmdl ){
      type = tr_GetTrainerEventType( mmdl );

      if( type == EV_TYPE_TRAINER || type == EV_TYPE_TRAINER_EYEALL ){
        if( tr_id == tr_GetTrainerID(mmdl) ){
          return( mmdl );
        }
      }
    }
  }
  
  GF_ASSERT( 0 && "TRAINER EYE NON PAIR" );
  return( NULL );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�����C�x���g�X�N���v�g�Z�b�g
 * @param fieldMap FIELDMAP_WORK*
 * @param mmdl �g���[�i�[MMDL*
 * @param id �X�N���v�gID
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
static GMEVENT * tr_SetEventScript( FIELDMAP_WORK *fieldMap, MMDL *mmdl )
{
  GMEVENT *event;
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldMap );
  HEAPID heapID = FIELDMAP_GetHeapID( fieldMap );
  event = SCRIPT_SetEventScript(
      gsys, SCRID_TRAINER_MOVE_BATTLE, mmdl, GFL_HEAPID_APP );
  return( event );
}

//--------------------------------------------------------------
/**
 * ���������X�N���v�g�C�x���g�փZ�b�g
 * @param event �N������GMEVENT*
 * @param data EYEMEET_HITDATA
 * @param tr_type �g���[�i�[�����^�C�v�BSCR_EYE_TR_TYPE_SINGLE��
 * @param tr_num �g���[�i�[�ԍ��BSCR_EYE_TR_0��
 * @retval nothing
 */
//--------------------------------------------------------------
static void trEventScript_SetTrainerEyeData(
    GMEVENT *event, const EYEMEET_HITDATA *data, int tr_type, int tr_num )
{
  SCRIPT_SetTrainerEyeData( event, data->mmdl,
    data->range, data->dir, data->scr_id, data->tr_id,
    tr_type, tr_num );
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
/// �����`�F�b�N�e�[�u��
//--------------------------------------------------------------
static int (* const data_treye_CheckEyeLineTbl[DIR_MAX4])(
    s16,s16,s16,s16,s16,s16,s16) =
{
  treye_CheckEyeLineUp,
  treye_CheckEyeLineDown,
  treye_CheckEyeLineLeft,
  treye_CheckEyeLineRight,
};
