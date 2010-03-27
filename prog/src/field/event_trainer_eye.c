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
#include "effect_encount.h"

#include "script_trainer.h"
//SCRID_TRAINER_MOVE_BATTLE 
#include "../../../resource/fldmapdata/script/trainer_def.h"

#include "trainer_eye_data.h"  //TRAINER_HITDATA

#include "script_def.h"
//======================================================================
//  define
//======================================================================
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
/**
 * @brief �����q�b�g�ړ����䃏�[�N
 */
//--------------------------------------------------------------
struct _EV_EYEMEET_MOVE_WORK
{
  int seq_no;        ///<�����ԍ�
  BOOL end_flag;      ///<�I���t���O
   
  TRAINER_HITDATA hitdata;

  int gyoe_type;    ///<!�^�C�v
  u32 sisen_no;      ///<�����ԍ�
  int count;        ///<�ړ��J�E���g
   
  FLDEFF_TASK *task_gyoe;   ////<�M���G�[�^�X�N
  FIELDMAP_WORK *fieldMap;  ///<FILEDMAP_WORK*
  FIELD_PLAYER *fieldPlayer; ///<FIELD_PLAYER*
};

//--------------------------------------------------------------
/**
 * @brief �����q�b�g�ړ�����C�x���g�p���[�N
 */
//--------------------------------------------------------------
typedef struct {
  EV_EYEMEET_MOVE_WORK * eye0;  ///<�g���[�i�[�O�p
  EV_EYEMEET_MOVE_WORK * eye1;  ///<�g���[�i�[�P�p
}TRAINER_MOVE_EVENT_WORK;

//======================================================================
//  proto
//======================================================================
static BOOL treye_CheckEyeMeet(
    FIELDMAP_WORK *fieldMap, MMDL *non_mmdl, TRAINER_HITDATA *hit );
static int treye_CheckEyeRange(
    const FIELD_ENCOUNT* enc, const MMDL *mmdl, const MMDL *jiki, u16 *hitDir );

static int treye_CheckEyeLine(
    u16 dir, s16 range, s16 tx, s16 ty, s16 tz, s16 gx, s16 gy, s16 gz );


static u16 tr_GetTrainerEventType( const MMDL *mmdl );
static BOOL tr_HitCheckEyeLine( const MMDL *mmdl, u16 dir, int range, const FIELD_ENCOUNT* enc );
static BOOL tr_CheckEventFlag( FIELDMAP_WORK *fieldMap, const MMDL *mmdl );
static void makeHitData( TRAINER_HITDATA *data, MMDL *mmdl, int range, u16 dir );
static MMDL * tr_CheckPairTrainer( const MMDL *tr_mmdl, u16 tr_id );
static GMEVENT * createTrainerScript( FIELDMAP_WORK *fieldMap, MMDL *mmdl );

static int (* const data_treye_CheckEyeLineTbl[DIR_MAX4])(
    s16,s16,s16,s16,s16,s16,s16);


static GMEVENT_RESULT controlTrainerEyeMoveEvent( GMEVENT * ev, int *seq, void * wk );
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
  TRAINER_HITDATA hit0;
  GMEVENT *event = NULL;
  
  if( treye_CheckEyeMeet(fieldMap,NULL,&hit0) == TRUE )
  {
    TRAINER_HITDATA hit1;
    BtlRule btl_rule = SCRIPT_GetTrainerBtlRule( hit0.tr_id );
    
    if( btl_rule == BTL_RULE_SINGLE ) //�V���O��
    {

      if( vs2 == FALSE || //�V���O����`�F�b�N
          treye_CheckEyeMeet(fieldMap,hit0.mmdl,&hit1) == FALSE )
      { //�X�N���v�g�p�C�x���g�f�[�^�Z�b�g �V���O��
        event = createTrainerScript( fieldMap, hit0.mmdl ); //�X�N���v�g�N��
        hit0.move_type = SCR_EYE_TR_TYPE_SINGLE;
        SCRIPT_TRAINER_SetHitData( event, SCR_EYE_TR_0, &hit0 );
        KAGAYA_Printf( "TRAINER EYE HIT SINGLE\n" );
      }
      else
      { //�X�N���v�g�p�C�x���g�f�[�^�Z�b�g �^�b�O
        event = createTrainerScript( fieldMap, hit0.mmdl ); //�X�N���v�g�N��
        hit0.move_type = SCR_EYE_TR_TYPE_TAG;
        SCRIPT_TRAINER_SetHitData( event, SCR_EYE_TR_0, &hit0 );
        hit1.move_type = SCR_EYE_TR_TYPE_TAG;
        SCRIPT_TRAINER_SetHitData( event, SCR_EYE_TR_1, &hit1 );
        KAGAYA_Printf( "TRAINER EYE HIT TAG DOUBLE\n" );
      }
    }
    else if( btl_rule == BTL_RULE_DOUBLE )
    {
      if( vs2 == TRUE ) //�_�u���\
      {
        MMDL *mmdl = tr_CheckPairTrainer( hit0.mmdl, hit0.tr_id );
        makeHitData( &hit1, mmdl, hit0.range, hit0.dir );
        
        event = createTrainerScript( fieldMap, hit0.mmdl ); //�X�N���v�g�N��
        //�X�N���v�g�p�C�x���g�f�[�^�Z�b�g �_�u��
        hit0.move_type = SCR_EYE_TR_TYPE_DOUBLE;
        SCRIPT_TRAINER_SetHitData( event, SCR_EYE_TR_0, &hit0 );
        hit1.move_type = SCR_EYE_TR_TYPE_DOUBLE;
        SCRIPT_TRAINER_SetHitData( event, SCR_EYE_TR_1, &hit1 );
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

//--------------------------------------------------------------
/**
 * �g���[�i�[�����͈̓`�F�b�N�@�O���[�o���@�����w��
 * @param  mmdl FIELD_OBJ_PTR
 * @param  jiki    PLAYER_STATE_PTR
 * @param  eye_dir    ��������
 * @param  eye_range  ��������
 * @param enc FIELD_ENCOUNT NULL=�G�t�F�N�g�G���J�E���g�`�F�b�N����
 * @retval  int �����q�b�g���̎��@�܂ł̃O���b�h�����BEYE_CHECK_NOHIT=�G���[
 */
//--------------------------------------------------------------
int TRAINER_MMDL_CheckEyeRange(
    const MMDL *mmdl, u16 eye_dir, int eye_range, const FIELD_ENCOUNT *enc )
{
  int ret;
  MMDL_GRIDPOS pt,pj;
  const MMDL *jiki;
  const MMDLSYS *mmdlsys;
  
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  jiki = MMDLSYS_SearchOBJID( mmdlsys, MMDL_ID_PLAYER );
  
  if( jiki == NULL ){
    GF_ASSERT( 0 );
    return( EYE_CHECK_NOHIT );
  }
  
  MMDL_GetGridPos( jiki, &pj );
  MMDL_GetGridPos( mmdl, &pt );
  
  ret = treye_CheckEyeLine( eye_dir, eye_range, 
      pt.gx, pt.gy, pt.gz, pj.gx, pj.gy, pj.gz );
  
  if( ret != EYE_CHECK_NOHIT ){
    if( tr_HitCheckEyeLine(mmdl,eye_dir,ret,enc) == TRUE ){
      ret = EYE_CHECK_NOHIT;
    }
  }
  
  return( ret );
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
    FIELDMAP_WORK *fieldMap, MMDL *non_mmdl, TRAINER_HITDATA *hit )
{
  u16 dir;
  u32 no=0;
  int range=EYE_CHECK_NOHIT;
  MMDL *mmdl = NULL;
  MMDL *jiki = FIELD_PLAYER_GetMMdl( FIELDMAP_GetFieldPlayer(fieldMap) );
  MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldMap );
  const FIELD_ENCOUNT * enc = FIELDMAP_GetEncount( fieldMap );

  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE )
  {
    if( non_mmdl == NULL || (non_mmdl != mmdl) )
    {
      range = treye_CheckEyeRange( enc, mmdl, jiki, &dir );
      
      if( range != EYE_CHECK_NOHIT )
      {
        if( tr_CheckEventFlag(fieldMap,mmdl) == FALSE )
        {
#ifdef DEBUG_ONLY_FOR_kagaya
          KAGAYA_Printf( "�����q�b�g �q�b�g�����g���[�i�[ID =%d\n",
              TRAINER_MMDL_GetTrainerID(mmdl) );
#endif
          makeHitData( hit, mmdl, range, dir );
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
    const FIELD_ENCOUNT* enc, const MMDL *mmdl, const MMDL *jiki, u16 *hitDir )
{
  u16 type;
  
  type = tr_GetTrainerEventType( mmdl );
  
  if( type == EV_TYPE_TRAINER || type == EV_TYPE_TRAINER_EYEALL )
  {
    int ret;
    u16 dir;
    MMDL_GRIDPOS pt,pj;
    s16 range = MMDL_GetParam( mmdl, MMDL_PARAM_0 );

    MMDL_GetGridPos( mmdl, &pt);
    MMDL_GetGridPos( jiki, &pj);
    
    if( type == EV_TYPE_TRAINER )
    {
      dir = MMDL_GetDirDisp( mmdl );
      ret = treye_CheckEyeLine( dir, range, pt.gx, pt.gy, pt.gz, pj.gx, pj.gy, pj.gz );
      
      if( ret != EYE_CHECK_NOHIT )
      {
        if( tr_HitCheckEyeLine(mmdl,dir,ret,enc) == FALSE ){
          *hitDir = dir;
          return( ret );
        }
      }
    }
    else //EV_TYPE_TRAINER_EYEALL
    {
      dir = DIR_UP;
      
      do{
        ret = treye_CheckEyeLine( dir, range, pt.gx, pt.gy, pt.gz, pj.gx, pj.gy, pj.gz );
        
        if( ret != EYE_CHECK_NOHIT )
        {
          if( tr_HitCheckEyeLine(mmdl,dir,ret,enc) == FALSE ){
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
//
//
//  �����q�b�g�ɂ��ړ��C�x���g
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �����q�b�g�ړ�����C�x���g�̐���
 * @param gsys
 * @param eye0  EV_EYEMEET_MOVE_WORK
 * @param eye1  EV_EYEMEET_MOVE_WORK
 * @return  GMEVENT ���������C�x���g
 */
//--------------------------------------------------------------
GMEVENT * EVENT_TrainerEyeMoveControl(
    GAMESYS_WORK * gsys, EV_EYEMEET_MOVE_WORK * eye0, EV_EYEMEET_MOVE_WORK * eye1 )
{
  GMEVENT * event;
  TRAINER_MOVE_EVENT_WORK * tmew;
  event = GMEVENT_Create( gsys, NULL, controlTrainerEyeMoveEvent, sizeof(TRAINER_MOVE_EVENT_WORK) );
  tmew = GMEVENT_GetEventWork( event );
  tmew->eye0 = eye0;
  tmew->eye1 = eye1;

  return event;
}
//--------------------------------------------------------------
/**
 * �����q�b�g�ړ�����p���[�N�̐���
 * @param   heapID
 * @param   fieldMap
 * @param   hitdata
 * @param   work_pos �����q�b�g���[�N�v�f�� 0=������ 1=�y�A
 * @retval  GMEVENT �ړ��������s��GMEVENT
 */
//--------------------------------------------------------------
EV_EYEMEET_MOVE_WORK * TRAINER_EYEMOVE_Create( HEAPID heapID,
    FIELDMAP_WORK *fieldMap, const TRAINER_HITDATA * hitdata, u32 work_pos )
{
  EV_EYEMEET_MOVE_WORK * work;
  work = GFL_HEAP_AllocClearMemory( heapID, sizeof(EV_EYEMEET_MOVE_WORK) );
  
  work->hitdata = *hitdata;
  work->gyoe_type = 0;      //���ݖ��g�p
  work->sisen_no = work_pos;
  work->fieldMap = fieldMap;
  work->fieldPlayer = FIELDMAP_GetFieldPlayer( fieldMap );
  
  return  work;
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
  if( MMDL_CheckMoveBitMove(work->hitdata.mmdl) == TRUE ){ //���쒆
    MMDL_OffMoveBitMoveProcPause( work->hitdata.mmdl ); //����|�[�Y����
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
  if( MMDL_CheckMoveBitMove(work->hitdata.mmdl) == TRUE ){
    return( FALSE ); //���쒆
  }
  
  MMDL_SetDirDisp( work->hitdata.mmdl, work->hitdata.dir );
  MMDL_OnMoveBitMoveProcPause( work->hitdata.mmdl );
  
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
    u32 code = MMDL_GetMoveCode( work->hitdata.mmdl );
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
  
  if( MMDL_CheckPossibleAcmd(work->hitdata.mmdl) == TRUE ){
    u16 code;
    GF_ASSERT( work->hitdata.dir < DIR_MAX4 );
    code = MMDL_ChangeDirAcmdCode( work->hitdata.dir, AC_DIR_U );
    MMDL_SetAcmd( work->hitdata.mmdl, code );
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
  if( MMDL_CheckEndAcmd(work->hitdata.mmdl) == FALSE ){
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
      fectrl, work->hitdata.mmdl, FLDEFF_GYOETYPE_GYOE, FALSE ); //SE�͖炳�Ȃ�
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
  MMDL_SetAcmd( work->hitdata.mmdl, AC_HIDE_PULLOFF );
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
  if( MMDL_CheckEndAcmd(work->hitdata.mmdl) == TRUE ){
    work->seq_no = SEQNO_TRMOVE_GYOE_END_WAIT;
  }
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
  if( work->hitdata.range <= 1 ){                  //���@�ڂ̑O �ړ�����K�v�Ȃ�
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
  if( MMDL_CheckPossibleAcmd(work->hitdata.mmdl) == TRUE ){
    u16 code = MMDL_ChangeDirAcmdCode( work->hitdata.dir, AC_WALK_U_8F );
    MMDL_SetAcmd( work->hitdata.mmdl, code );
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
  if( MMDL_CheckEndAcmd(work->hitdata.mmdl) == FALSE ){
    return( FALSE );
  }
  
  work->hitdata.range--;                      //������炷
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
      MMDL_GetGridPosX(work->hitdata.mmdl), MMDL_GetGridPosZ(work->hitdata.mmdl) );
  
  if( MMDL_GetDirDisp(j_mmdl) != turn_dir &&
    (work->sisen_no == 0 || work->hitdata.move_type == SCR_EYE_TR_TYPE_TAG) )
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
  MMDL_EndAcmd( work->hitdata.mmdl );

  //�퓬�I���ォ��͈ړ����Ȃ��悤�ɁA����R�[�h��MV_DMY�ɂ���
  MMDL_ChangeMoveCode( work->hitdata.mmdl, MV_DMY );
 
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
static BOOL trainerMoveTask( EV_EYEMEET_MOVE_WORK * work )
{
  while( data_EyeMeetMoveFuncTbl[work->seq_no](work) == TRUE ){};
  
  return( work->end_flag == TRUE );
}

//--------------------------------------------------------------
/**
 * @brief �����q�b�g�ړ�����C�x���g
 */
//--------------------------------------------------------------
static GMEVENT_RESULT controlTrainerEyeMoveEvent( GMEVENT * ev, int *seq, void * wk )
{
  TRAINER_MOVE_EVENT_WORK * tmew = wk;

  if ( tmew->eye0 && trainerMoveTask( tmew->eye0 ) == TRUE )
  {
    GFL_HEAP_FreeMemory( tmew->eye0 );
    tmew->eye0 = NULL;
  }

  if ( tmew->eye1 && trainerMoveTask( tmew->eye1 ) == TRUE )
  {
    GFL_HEAP_FreeMemory( tmew->eye1 );
    tmew->eye1 = NULL;
  }
  if ( tmew->eye0 == NULL && tmew->eye1 == NULL )
  {
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
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
  case EV_TYPE_TRAINER_DASH_ALTER:
  case EV_TYPE_TRAINER_DASH_REACT:
  case EV_TYPE_TRAINER_DASH_ACCEL:
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
static BOOL tr_HitCheckEyeLine( const MMDL *mmdl, u16 dir, int range, const FIELD_ENCOUNT* enc )
{
  if( range ) //�����W0=�ڂ̑O
  {
    int i = 0;
    u32 hit;
    s16 dx,dz;
    MMDL_GRIDPOS pos;
    
    MMDL_GetGridPos( mmdl, &pos );
    dx = MMDL_TOOL_GetDirAddValueGridX(dir);
    dz = MMDL_TOOL_GetDirAddValueGridZ(dir);
    pos.gx += dx;
    pos.gz += dz;
    
    for( ; i < (range-1); i++ ){
      hit = MMDL_HitCheckMoveCurrent( mmdl, pos.gx, pos.gy, pos.gz, dir );
      hit &= ~MMDL_MOVEHITBIT_LIM; //�ړ������𖳎�����
      
      if( hit ){ //�ړ������ȊO�Ńq�b�g
        return( TRUE );
      }
      
      if( enc != NULL ){ //�G�t�F�N�g�G���J�E���g���W�`�F�b�N
        if( EFFECT_ENC_CheckEffectPos( enc, &pos )){
          return( TRUE );
        }
      }
      
      pos.gx += dx;
      pos.gz += dz;
    }
    
    hit = MMDL_HitCheckMoveCurrent( //�Ō�
        mmdl, pos.gx, pos.gy, pos.gz, dir );
    
    hit &= ~MMDL_MOVEHITBIT_LIM; //�ړ������𖳎�����
    
    if( hit != MMDL_MOVEHITBIT_OBJ ){ //OBJ�Փ�(���@)�̂�
      return( TRUE );
    }
    
    if( enc != NULL ){ //�G�t�F�N�g�G���J�E���g���W�`�F�b�N
      if( EFFECT_ENC_CheckEffectPos(enc,&pos) ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * @brief   ���샂�f������g���[�i�[ID�擾
 * @param   mmdl MMDL*
 * @retval  u16 �g���[�i�[ID
 */
//--------------------------------------------------------------
u16 TRAINER_MMDL_GetTrainerID( const MMDL *mmdl )
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
  u16 id = TRAINER_MMDL_GetTrainerID( mmdl );
  return( SCRIPT_CheckEventFlagTrainer(evwork,id) );
}

//--------------------------------------------------------------
/**
 * TRAINER_HITDATA������
 * @param data TRAINER_HITDATA
 * @param mmdl MMDL
 * @param range ��������
 * @param dir �ړ�����
 * @retval nothing
 */
//--------------------------------------------------------------
static void makeHitData(
    TRAINER_HITDATA *data, MMDL *mmdl, int range, u16 dir )
{
  data->range = range;
  data->dir = dir;
  data->scr_id = MMDL_GetEventID( mmdl );
  data->tr_id = TRAINER_MMDL_GetTrainerID( mmdl );
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
        if( tr_id == TRAINER_MMDL_GetTrainerID(mmdl) ){
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
static GMEVENT * createTrainerScript( FIELDMAP_WORK *fieldMap, MMDL *mmdl )
{
  GMEVENT *event;
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldMap );
  event = SCRIPT_SetEventScript(
      gsys, SCRID_TRAINER_MOVE_BATTLE, mmdl, GFL_HEAPID_APP );
  return( event );
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
