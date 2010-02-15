//======================================================================
/**
 *
 * @file  fieldobj_move_1.c
 * @brief  �t�B�[���h���샂�f���@��{����n����1�@�T�u����
 * @author  kagaya
 * @date  05.07.21
 *
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procmove.h"

//======================================================================
//  define
//======================================================================
//#define DEBUG_IDO_ASSERT

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static void MoveSub_InitProcCall( MMDL * mmdl );
static int MoveSub_StartCheckProcCall( MMDL * mmdl );
static int MoveSub_MoveProcCall( MMDL * mmdl );

static int MoveSub_PosUpdateStartCheck( MMDL * mmdl );
static int MoveSub_PosUpdateEndCheck( MMDL * mmdl );

static void SubMoveNon_Init( MMDL * mmdl );
static int SubMoveNon_StartCheck( MMDL * mmdl );
static int SubMoveNon_Move( MMDL * mmdl );

static void SubMoveKyoro_Init( MMDL * mmdl );
static int SubMoveKyoro_StartCheck( MMDL * mmdl );
static int SubMoveKyoro_Move( MMDL * mmdl );

static void SubMoveSpinStop_Init( MMDL * mmdl );
static int SubMoveSpinStop_StartCheck( MMDL * mmdl );
static int SubMoveSpinStop_Move( MMDL * mmdl );

static void (* const DATA_MMdl_EventTypeInitProcTbl[EV_TYPE_MAX])( MMDL * );
static int (* const DATA_MMdl_EventTypeStartCheckProcTbl[EV_TYPE_MAX])( MMDL * );
static int (* const DATA_MMdl_EventTypeMoveProcTbl[EV_TYPE_MAX])( MMDL * );

//======================================================================
//  �T�u����
//======================================================================
//--------------------------------------------------------------
/**
 * �T�u���쏉����
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveSubProcInit( MMDL * mmdl )
{
  MoveSub_InitProcCall( mmdl );
}

//--------------------------------------------------------------
/**
 * �T�u����
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=�T�u����ɂ��ړ��A��
 */
//--------------------------------------------------------------
int MMDL_MoveSub( MMDL * mmdl )
{
  if( MoveSub_StartCheckProcCall(mmdl) == FALSE ){
    return( FALSE );
  }
  
  if( MoveSub_MoveProcCall(mmdl) == FALSE ){
    return( FALSE );
  }
  
  return( TRUE );
}

//======================================================================
//  �T�u���쏈���@�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �������֐��Ăяo��
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MoveSub_InitProcCall( MMDL * mmdl )
{
  int type = MMDL_GetEventType( mmdl );
  DATA_MMdl_EventTypeInitProcTbl[type]( mmdl );
}

//--------------------------------------------------------------
/**
 * �`�F�b�N�֐��Ăяo��
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=�`�F�b�N�Ɉ�����������
 */
//--------------------------------------------------------------
static int MoveSub_StartCheckProcCall( MMDL * mmdl )
{
  int type = MMDL_GetEventType( mmdl );
  return( DATA_MMdl_EventTypeStartCheckProcTbl[type](mmdl) );
}

//--------------------------------------------------------------
/**
 * ����֐��Ăяo��
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=���쒆
 */
//--------------------------------------------------------------
static int MoveSub_MoveProcCall( MMDL * mmdl )
{
  int type = MMDL_GetEventType( mmdl );
  return( DATA_MMdl_EventTypeMoveProcTbl[type](mmdl) );
}

//======================================================================
//  �T�u����p�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * ���W�X�V�J�n���`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=�X�V
 */
//--------------------------------------------------------------
static int MoveSub_PosUpdateStartCheck( MMDL * mmdl )
{
  int now = MMDL_GetGridPosX( mmdl );
  int old = MMDL_GetOldGridPosX( mmdl );
  
  if( now != old ){
    return( TRUE );
  }
  
  now = MMDL_GetGridPosZ( mmdl );
  old = MMDL_GetOldGridPosZ( mmdl );
  
  if( now != old ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���W�X�V�I�����`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=�I��
 */
//--------------------------------------------------------------
static int MoveSub_PosUpdateEndCheck( MMDL * mmdl )
{
  int now = MMDL_GetGridPosX( mmdl );
  int old = MMDL_GetOldGridPosX( mmdl );
  
  if( now != old ){
    return( FALSE );
  }
  
  now = MMDL_GetGridPosZ( mmdl );
  old = MMDL_GetOldGridPosZ( mmdl );
  
  if( now != old ){
    return( FALSE );
  }
  
  return( TRUE );
}

//======================================================================
//  �T�u����@����
//======================================================================
//--------------------------------------------------------------
/**
 * �T�u���쏉�����@����
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void SubMoveNon_Init( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �T�u����X�^�[�g�`�F�b�N�@����
 * @param  mmdl  MMDL *
 * @retval  int    FALSE
 */
//--------------------------------------------------------------
static int SubMoveNon_StartCheck( MMDL * mmdl )
{
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �T�u����@����
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=���쒆
 */
//--------------------------------------------------------------
static int SubMoveNon_Move( MMDL * mmdl )
{
  return( FALSE );
}

//======================================================================
//  �T�u����@�~�܂��Ă���낫���
//======================================================================
//--------------------------------------------------------------
///  SUBWORK_KYORO�\����
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
///  ����낫������
//--------------------------------------------------------------
enum
{
  DIR_H_TYPE = 0,    ///<����������낫���
  DIR_V_TYPE,      ///<�c��������낫���
};

#define KYORO_WAIT_FRAME (8)    ///<����낫���̍���
#define KYORO_WAIT_FRAME_DASH (4)    ///<����낫���̍��� DASH������
#define KYORO_COUNT_MAX (4)      ///<����낫���񐔍ő�

//--------------------------------------------------------------
/**
 * �~�܂��Ă���낫���@������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void SubMoveKyoro_Init( MMDL * mmdl )
{
  SUBWORK_KYORO *work;
  
  work = MMDL_InitMoveSubProcWork( mmdl, SUBWORK_KYORO_SIZE );
  work->max_count = MMDL_GetParam( mmdl, MMDL_PARAM_1 );
}

//--------------------------------------------------------------
/**
 * �~�܂��Ă���낫���@�X�^�[�g�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=����A��
 */
//--------------------------------------------------------------
static int SubMoveKyoro_StartCheck( MMDL * mmdl )
{
  SUBWORK_KYORO *work;
  
  work = MMDL_GetMoveSubProcWork( mmdl );
  
  switch( work->check_seq_no ){
  case 0:                            //�ړ��J�n�Ď�
    if( MoveSub_PosUpdateStartCheck(mmdl) == TRUE ){
      work->check_seq_no++;
    }
    
    break;
  case 1:                            //�ړ��I���Ď�
    if( MoveSub_PosUpdateEndCheck(mmdl) == FALSE ){
      break;
    }
    
    work->walk_count++;
    
    if( work->walk_count < work->max_count ){
      work->check_seq_no = 0;
      break;
    }
    
    work->check_seq_no++;
  case 2:                            //�ړ������Ď�
    if( MMDL_CheckMoveBitMove(mmdl) == TRUE ){
      #ifdef DEBUG_IDO_ASSERT
      GF_ASSERT( MoveSub_PosUpdateStartCheck(mmdl) == TRUE &&
          "SubMoveKyoro_StartCheck()ERROR" );
      #endif
      break;
    }
    
    work->check_seq_no++;
    work->walk_count = 0;
    work->move_seq_no = 0;
  case 3:                            //�T�u���쒆
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �~�܂��Ă���낫���@����
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=���쒆
 */
//--------------------------------------------------------------
static int SubMoveKyoro_Move( MMDL * mmdl )
{
  SUBWORK_KYORO *work;
  
  work = MMDL_GetMoveSubProcWork( mmdl );
  
  switch( work->move_seq_no ){
  case 0:                            //������
    {
      int type_tbl[4] = {DIR_H_TYPE,DIR_H_TYPE,DIR_V_TYPE,DIR_V_TYPE};
      int dir = MMDL_GetDirDisp( mmdl );
      work->origin_dir = dir;
      work->dir_type = type_tbl[dir];
      work->move_seq_no++;
    }
  case 1:                            //�����Z�b�g  
    {
      int dir_tbl[2][2] = { {DIR_LEFT,DIR_RIGHT},{DIR_UP,DIR_DOWN} };
      int dir = dir_tbl[work->dir_type][work->dir_no];
      int code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
      MMDL_SetLocalAcmd( mmdl, code );
      work->move_seq_no++;
    }
  case 2:
    {
      if( MMDL_ActionLocalAcmd(mmdl) == FALSE ){
        return( TRUE );
      }
      
      work->move_seq_no++;
    }
  case 3:
    {
      u8 wait_max = KYORO_WAIT_FRAME;
      u16 ev_type = MMDL_GetEventType( mmdl );
      
      if( ev_type == EV_TYPE_TRAINER_DASH_ACCEL_KYORO ){
        if( MMDL_CheckPlayerDispSizeRect(mmdl) == TRUE ){
          wait_max = KYORO_WAIT_FRAME_DASH;
        }
      }
      
      work->wait++;
      
     
      if( work->wait < wait_max ){
        return( TRUE );
      }
      
      work->wait = 0;
      work->dir_count++;
      
      if( work->dir_count < KYORO_COUNT_MAX ){
        work->dir_no = (work->dir_no + 1) & 0x01;
        work->move_seq_no = 1;
        return( TRUE );
      }
      
      MMDL_SetDirDisp( mmdl, work->origin_dir );
      work->move_seq_no++;
      work->dir_count = 0;
      work->check_seq_no = 0;
    }
  }
  
  return( FALSE );
}

//======================================================================
//  �T�u����@�~�܂��ăN���N��
//======================================================================
//--------------------------------------------------------------
///  SUBWORK_SPIN_STOP�\����
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
///  ��]����
//--------------------------------------------------------------
enum
{
  SPIN_STOP_L_TYPE = 0,    ///<����]
  SPIN_STOP_R_TYPE,      ///<�E��]
};

#define SPIN_STOP_WAIT_FRAME (8)  ///<��]�̊�
#define SPIN_STOP_WAIT_FRAME_DASH (4) ///<��]�̊� �_�b�V��������
#define SPIN_STOP_COUNT_MAX (4)    ///<��]�ő吔

//--------------------------------------------------------------
/**
 * �~�܂��ăN���N���@������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void SubMoveSpinStop_Init( MMDL * mmdl )
{
  int type;
  SUBWORK_SPIN_STOP *work;
  
  work = MMDL_InitMoveSubProcWork( mmdl, SUBWORK_SPIN_STOP_SIZE );
  work->max_count = MMDL_GetParam( mmdl, MMDL_PARAM_1 );
  
  type = MMDL_GetEventType( mmdl );
  
  if( type == EV_TYPE_TRAINER_SPIN_STOP_L ||
      type == EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_L ){
    type = SPIN_STOP_L_TYPE;
  }else{
    type = SPIN_STOP_R_TYPE;
  }
  
  work->dir_type = type;
}

//--------------------------------------------------------------
/**
 * �~�܂��ăN���N���@�X�^�[�g�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=����A��
 */
//--------------------------------------------------------------
static int SubMoveSpinStop_StartCheck( MMDL * mmdl )
{
  SUBWORK_SPIN_STOP *work;
  
  work = MMDL_GetMoveSubProcWork( mmdl );
  
  switch( work->check_seq_no ){
  case 0:                            //�ړ��J�n�Ď�
    if( MoveSub_PosUpdateStartCheck(mmdl) == TRUE ){
      work->check_seq_no++;
    }
    
    break;
  case 1:                            //�ړ��I���Ď�
    if( MoveSub_PosUpdateEndCheck(mmdl) == FALSE ){
      break;
    }
    
    work->walk_count++;
    
    if( work->walk_count < work->max_count ){
      work->check_seq_no = 0;
      break;
    }
    
    work->check_seq_no++;
  case 2:                            //�ړ������Ď�
    if( MMDL_CheckMoveBitMove(mmdl) == TRUE ){
      #ifdef DEBUG_IDO_ASSERT
      GF_ASSERT( MoveSub_PosUpdateStartCheck(mmdl) == TRUE &&
        "SubMoveKyoro_StartCheck()�Ώۂ̈ړ����@���ُ�" );
      #endif
      break;
    }
    
    work->check_seq_no++;
    work->walk_count = 0;
    work->move_seq_no = 0;
  case 3:                            //�T�u���쒆
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �~�܂��ăN���N���@����
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=���쒆
 */
//--------------------------------------------------------------
static int SubMoveSpinStop_Move( MMDL * mmdl )
{
  SUBWORK_KYORO *work;
  int spin_tbl[2][4] =
  { {DIR_UP,DIR_LEFT,DIR_DOWN,DIR_RIGHT},{DIR_UP,DIR_RIGHT,DIR_DOWN,DIR_LEFT} };
  
  work = MMDL_GetMoveSubProcWork( mmdl );
  
  switch( work->move_seq_no ){
  case 0:                            //������
    {
      int i,dir = MMDL_GetDirDisp( mmdl );
      
      for( i = 0; (i < DIR_MAX4 && dir != spin_tbl[work->dir_type][i]); i++ ){}
      GF_ASSERT( i < DIR_MAX4 && "SubMoveKyoro_Move()�����ُ�" );
      
      work->origin_dir = dir;
      work->dir_no = (i + 1) % SPIN_STOP_COUNT_MAX;    //+1=1��
      work->move_seq_no++;
    }
  case 1:                            //�����Z�b�g  
    {
      int dir = spin_tbl[work->dir_type][work->dir_no];
      int code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
      MMDL_SetLocalAcmd( mmdl, code );
      work->move_seq_no++;
    }
  case 2:
    {
      if( MMDL_ActionLocalAcmd(mmdl) == FALSE ){
        return( TRUE );
      }
      
      work->move_seq_no++;
    }
  case 3:
    {
      u8 wait_max = SPIN_STOP_WAIT_FRAME;
      u16 ev_type = MMDL_GetEventType( mmdl );
      
      if( ev_type == EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_L ||
          ev_type == EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_R ){
        if( MMDL_CheckPlayerDispSizeRect(mmdl) == TRUE ){
          wait_max = SPIN_STOP_WAIT_FRAME_DASH;
        }
      }
      
      work->wait++;
      
      if( work->wait < wait_max ){
        return( TRUE );
      }
      
      work->wait = 0;
      work->dir_count++;
      
      if( work->dir_count < SPIN_STOP_COUNT_MAX ){
        work->dir_no = (work->dir_no + 1) % SPIN_STOP_COUNT_MAX;
        work->move_seq_no = 1;
        return( TRUE );
      }
      
      MMDL_SetDirDisp( mmdl, work->origin_dir );
      work->move_seq_no++;
      work->dir_count = 0;
      work->check_seq_no = 0;
    }
  }
  
  return( FALSE );
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
//  �C�x���g�^�C�v�ʏ������֐� EV_TYPE_NORMAL���̒l�Ɉ�v
//--------------------------------------------------------------
static void (* const DATA_MMdl_EventTypeInitProcTbl[EV_TYPE_MAX])( MMDL * ) =
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
  SubMoveNon_Init, //EV_TYPE_TRAINER_DASH_ALTER
  SubMoveNon_Init, //EV_TYPE_TRAINER_DASH_REACT
  SubMoveNon_Init, //EV_TYPE_TRAINER_DASH_ACCEL
  SubMoveKyoro_Init, //EV_TYPE_TRAINER_DASH_ACCEL_KYORO
  SubMoveSpinStop_Init, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_L
  SubMoveSpinStop_Init, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_R
  SubMoveNon_Init, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_L
  SubMoveNon_Init, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_R
};

//--------------------------------------------------------------
//  �C�x���g�^�C�v�ʃX�^�[�g�`�F�b�N�֐��@EV_TYPE_NORMAL���̒l�Ɉ�v
//--------------------------------------------------------------
static int (* const DATA_MMdl_EventTypeStartCheckProcTbl[EV_TYPE_MAX])( MMDL * ) =
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
  SubMoveNon_StartCheck, //EV_TYPE_TRAINER_DASH_ALTER
  SubMoveNon_StartCheck, //EV_TYPE_TRAINER_DASH_REACT
  SubMoveNon_StartCheck, //EV_TYPE_TRAINER_DASH_ACCEL
  SubMoveKyoro_StartCheck, //EV_TYPE_TRAINER_DASH_ACCEL_KYORO
  SubMoveSpinStop_StartCheck, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_L
  SubMoveSpinStop_StartCheck, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_R
  SubMoveNon_StartCheck, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_L
  SubMoveNon_StartCheck, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_R
};

//--------------------------------------------------------------
//  �C�x���g�^�C�v�ʓ���֐� EV_TYPE_NORMAL���̒l�Ɉ�v
//--------------------------------------------------------------
static int (* const DATA_MMdl_EventTypeMoveProcTbl[EV_TYPE_MAX])( MMDL * ) =
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
  SubMoveNon_Move, //EV_TYPE_TRAINER_DASH_ALTER
  SubMoveNon_Move, //EV_TYPE_TRAINER_DASH_REACT
  SubMoveNon_Move, //EV_TYPE_TRAINER_DASH_ACCEL
  SubMoveKyoro_Move, //EV_TYPE_TRAINER_DASH_ACCEL_KYORO
  SubMoveSpinStop_Move, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_L
  SubMoveSpinStop_Move, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_R
  SubMoveNon_Move, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_L
  SubMoveNon_Move, //EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_R
};
