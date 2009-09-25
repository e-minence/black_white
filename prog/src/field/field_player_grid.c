//======================================================================
/**
 * @file  field_player_grid.c
 * @brief �O���b�h��p �t�B�[���h�v���C���[����
 * @author kagaya
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "fldmmdl.h"
#include "field_player.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "fieldmap_ctrl_grid.h"
#include "field_player_grid.h"
#include "map_attr.h"

#include "fldeff_namipoke.h"

#include "field_sound.h"

//======================================================================
//  define
//======================================================================
//#define PLAYER_MOVE_TRACK_CHANGE //��`�Ŏ��@���쎞�Ƀg���b�N�ύX

//--------------------------------------------------------------
/// FIELD_PLAYER_GRID_MOVEBIT
//--------------------------------------------------------------
typedef enum
{
  ///���ɖ���
  FIELD_PLAYER_GRID_MOVEBIT_NON = (0),
  ///�����ړ���
  FIELD_PLAYER_GRID_MOVEBIT_FORCE       = (1<<0),
  ///��������
  FIELD_PLAYER_GRID_MOVEBIT_UNDER_OFF   = (1<<1),
  ///����ړ�
  FIELD_PLAYER_GRID_MOVEBIT_STEP        = (1<<2),
  ///�ő�
  FIELD_PLAYER_GRID_MOVEBIT_BITMAX      = (1<<3),
}FIELD_PLAYER_GRID_MOVEBIT;

#define FIELD_PLAYER_GRID_MOVEBIT_MAX (3)

//--------------------------------------------------------------
///  PLAYER_MOVE
//--------------------------------------------------------------
typedef enum
{
  PLAYER_MOVE_STOP = 0,
  PLAYER_MOVE_WALK,
  PLAYER_MOVE_TURN,
  PLAYER_MOVE_HITCH,
}PLAYER_MOVE;

//--------------------------------------------------------------
///  PLAYER_SET
//--------------------------------------------------------------
typedef enum
{
  PLAYER_SET_NON = 0, ///<�Z�b�g����
  PLAYER_SET_STOP,    ///<��~�Z�b�g
  PLAYER_SET_WALK,    ///<�����Z�b�g
  PLAYER_SET_TURN,    ///<�U������Z�b�g
  PLAYER_SET_HITCH,   ///<��Q���q�b�g�Z�b�g
  PLAYER_SET_JUMP,    ///<�W�����v�Z�b�g
}PLAYER_SET;

//--------------------------------------------------------------
/// PLAYER_MOVEHITBIT
//--------------------------------------------------------------
typedef enum
{
  PLAYER_MOVEHITBIT_NONE   =        0, ///<��Q������
  PLAYER_MOVEHITBIT_ATTR   =   (1<<0), ///<�A�g���r���[�g�q�b�g
  PLAYER_MOVEHITBIT_OBJ    =   (1<<1), ///<OBJ���m�̏Փ�
  PLAYER_MOVEHITBIT_JUMP   =   (1<<2), ///<�W�����v�A�g���r���[�g�q�b�g
  PLAYER_MOVEHITBIT_EXIT   =   (1<<3), ///<�o������q�b�g
  PLAYER_MOVEHITBIT_WATER  =   (1<<4), ///<���A�g���r���[�g�q�b�g
#if 0
  JIKI_MOVE_HIT_BIT_TAKE_OFF  (1<<4)          ///<�W�����v��q�b�g
  JIKI_MOVE_HIT_BIT_BRIDGEHV  (1<<6)          ///<��{���q�b�g
  JIKI_MOVE_HIT_BIT_JUMP_3G  (1<<7)          ///<3G�W�����v
#endif
}PLAYER_MOVEHITBIT;

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// FIELD_PLAYER_GRID
//--------------------------------------------------------------
struct _TAG_FIELD_PLAYER_GRID
{
  int move_state;
  
  FIELD_PLAYER_GRID_MOVEBIT move_bit;
  FIELD_PLAYER_GRID_REQBIT req_bit;
  
  FIELD_PLAYER *fld_player;
  FIELDMAP_WORK *fieldWork;
  FLDEFF_TASK *fldeff_joint;
  
  u16 input_key_dir_x; //�L�[���͉�����
  u16 input_key_dir_z; //�L�[���͏c����
};

//======================================================================
//  proto
//======================================================================
//�ʏ�ړ�
static void jikiMove_Normal(
    FIELD_PLAYER_GRID *gjiki, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );

static PLAYER_SET gjiki_CheckMoveStart_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjiki_CheckMoveStart_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjiki_CheckMoveStart_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjiki_CheckMoveStart_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void gjiki_SetMove_Non(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjiki_SetMove_Jump(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

//���]�Ԉړ�
static void jikiMove_Cycle(
    FIELD_PLAYER_GRID *gjiki, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );

static PLAYER_SET gjikiCycle_CheckMoveStart_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjikiCycle_CheckMoveStart_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjikiCycle_CheckMoveStart_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjikiCycle_CheckMoveStart_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void gjikiCycle_SetMove_Non(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiCycle_SetMove_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiCycle_SetMove_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiCycle_SetMove_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiCycle_SetMove_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiCycle_SetMove_Jump(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

//�g���ړ�
static void jikiMove_Swim(
    FIELD_PLAYER_GRID *gjiki, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag );

static PLAYER_SET gjikiSwim_CheckMoveStart_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjikiSwim_CheckMoveStart_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjikiSwim_CheckMoveStart_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static PLAYER_SET gjikiSwim_CheckMoveStart_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void gjikiSwim_SetMove_Non(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiSwim_SetMove_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiSwim_SetMove_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiSwim_SetMove_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiSwim_SetMove_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static u32 gjiki_HitCheckMove(
    FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, u16 dir, MAPATTR *attr );

//�T�E���h
#if 0
static void gjiki_Sound_MoveStop( void );
static void gjiki_Sound_Move( void );
#endif

//�p�[�c
static void gjiki_OnMoveBit(
    FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_MOVEBIT bit );
static void gjiki_OffMoveBit(
    FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_MOVEBIT bit );
static void gjiki_OnMoveBitForce( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OffMoveBitForce( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OnMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OffMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OnMoveBitStep( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OffMoveBitStep( FIELD_PLAYER_GRID *gjiki );

//======================================================================
//  �L�[����
//======================================================================
//�L�[�v���X���牟����Ă���X�������擾
static u16 getKeyDirX( u16 key_prs )
{
  if( (key_prs & PAD_KEY_LEFT) ){
    return( DIR_LEFT );
  }
  
  if( (key_prs & PAD_KEY_RIGHT) ){
    return( DIR_RIGHT );
  }
  
  return( DIR_NOT );
}

//�L�[�v���X���牟����Ă���Z�������擾
static u16 getKeyDirZ( u16 key_prs )
{
  if( (key_prs & PAD_KEY_UP) ){
    return( DIR_UP );
  }
  
  if( (key_prs & PAD_KEY_DOWN) ){
    return( DIR_DOWN );
  }
  
  return( DIR_NOT );
}

//�L�[�����擾
static u16 gjiki_GetInputKeyDir(
    FIELD_PLAYER_GRID *gjiki, u16 key_prs )
{
  int key_dir_x = getKeyDirX( key_prs );
  int key_dir_z = getKeyDirZ( key_prs );
  
  if( key_dir_x == DIR_NOT ){   //����������̏ꍇ��Z�D��
    return( key_dir_z );
  }
  
  if( key_dir_z == DIR_NOT ){   //Z�L�[���� X�L�[�Ԃ�
    return( key_dir_x );
  }
  
  { //�΂߉���
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
    u16 move_dir = MMDL_GetDirMove( mmdl );
    u16 input_x = gjiki->input_key_dir_x;
    u16 input_z = gjiki->input_key_dir_z;
    
    //�ړ������ƈ�v���Ȃ�������Z�D��ŕԂ�
    if( move_dir != DIR_NOT ){
      //�ߋ��ɉ������������p��
      if( key_dir_x == input_x && key_dir_z == input_z ){
        return( move_dir ); //�ړ����ƈ�v���������Ԃ�
      }
      
      if( key_dir_z != input_z ){  //�V�K�΂߉�����Z�D��ŕԂ�
        return( key_dir_z );
      }
      
      return( key_dir_x );
    }
    
    return( key_dir_z ); //Z�D��ŕԂ�
  }
  
  return( DIR_NOT );
}

//�L�[���͕����Z�b�g
static void gjiki_SetInputKeyDir( FIELD_PLAYER_GRID *gjiki, u16 key_prs )
{
  gjiki->input_key_dir_x = getKeyDirX( key_prs );
  gjiki->input_key_dir_z = getKeyDirZ( key_prs );
}

//����J�n���̋��ʏ���������
static void gjiki_InitMoveStartCommon( FIELD_PLAYER_GRID *gjiki, u16 key_prs )
{
  gjiki_SetInputKeyDir( gjiki, key_prs );
  gjiki_OffMoveBitStep( gjiki );
}

//======================================================================
//  �O���b�h�ړ� �t�B�[���h�v���C���[����
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@����
 * @param  fld_player FIELD_PLAYER
 * @param heapID HEAPID
 * @retval FIELD_PLAYER_GRID
 */
//--------------------------------------------------------------
FIELD_PLAYER_GRID * FIELD_PLAYER_GRID_Init(
    FIELD_PLAYER *fld_player, HEAPID heapID )
{
  FIELD_PLAYER_GRID *gjiki;
  
  gjiki = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLAYER_GRID) );
  gjiki->fld_player = fld_player;
  gjiki->fieldWork = FIELD_PLAYER_GetFieldMapWork( fld_player );
  gjiki->input_key_dir_x = DIR_NOT;
  gjiki->input_key_dir_z = DIR_NOT;

  //���A
  {
    PLAYER_MOVE_FORM form;
    form = FIELD_PLAYER_GetMoveForm( gjiki->fld_player );
    
    switch( form ){
    case PLAYER_MOVE_FORM_SWIM:
      FIELD_PLAYER_GRID_SetRequest(
          gjiki, FIELD_PLAYER_GRID_REQBIT_SWIM );
      FIELD_PLAYER_GRID_UpdateRequest( gjiki );
      break;
    }
  }
  
//SetGridPlayerActTrans( gjiki->pActCont, &gjiki->vec_pos );
  return( gjiki );
}

//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@�폜
 * @param  gjiki FIELD_PLAYER_GRID
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_Delete( FIELD_PLAYER_GRID *gjiki )
{
  GFL_HEAP_FreeMemory( gjiki );
}

//======================================================================
//  �O���b�h�ړ� �t�B�[���h�v���C���[����@�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ� �t�B�[���h�v���C���[����@�ړ�
 * @param  gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_Move(
    FIELD_PLAYER_GRID *gjiki, int key_trg, int key_cont )
{
  u16 dir;
  BOOL debug_flag;
  PLAYER_MOVE_FORM form;
  
  dir = gjiki_GetInputKeyDir( gjiki, key_cont );
  
  debug_flag = FALSE;
  
  #ifdef PM_DEBUG
  if( key_cont & PAD_BUTTON_R ){
    debug_flag = TRUE;
  }
  #endif
  
  FIELD_PLAYER_GRID_UpdateRequest( gjiki );
  
  form = FIELD_PLAYER_GetMoveForm( gjiki->fld_player );
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    jikiMove_Normal( gjiki, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_FORM_CYCLE:
    jikiMove_Cycle( gjiki, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_FORM_SWIM:
    jikiMove_Swim( gjiki, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
}

//======================================================================
//  �O���b�h�ړ��@�t�B�[���h�v���C���[����@�ʏ�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@�ʏ�ړ�
 * @param  gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval nothing
 */
//--------------------------------------------------------------
static void jikiMove_Normal(
    FIELD_PLAYER_GRID *gjiki, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
  PLAYER_SET set;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
   
  set = PLAYER_SET_NON;
  switch( gjiki->move_state ){
  case PLAYER_MOVE_STOP:
    set = gjiki_CheckMoveStart_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_WALK:
    set = gjiki_CheckMoveStart_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_TURN:
    set = gjiki_CheckMoveStart_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_HITCH:
    set = gjiki_CheckMoveStart_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  if( set != PLAYER_SET_NON ){
    gjiki_InitMoveStartCommon( gjiki, key_cont );
  }

  switch( set ){
  case PLAYER_SET_NON:
    gjiki_SetMove_Non(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_STOP:
    gjiki_SetMove_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_WALK:
    gjiki_SetMove_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_TURN:
    gjiki_SetMove_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_HITCH:
    gjiki_SetMove_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_JUMP:
    gjiki_SetMove_Jump(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  }
}

//======================================================================
//  �ړ��J�n�`�F�b�N�@�ʏ�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��~��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_CheckMoveStart_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( dir != old_dir && debug_flag == FALSE ){
        return( PLAYER_SET_TURN );
      }
      
      return( gjiki_CheckMoveStart_Walk(
        gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
    }
    
    return( PLAYER_SET_STOP );
  }
  
  return( PLAYER_SET_NON );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�ړ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_CheckMoveStart_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( gjiki_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  {
    MAPATTR attr;
    u32 hit = gjiki_HitCheckMove( gjiki, mmdl, dir, &attr );
    
    if( debug_flag == TRUE )
    {
      if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit&MMDL_MOVEHITBIT_OUTRANGE) )
      {
        hit = MMDL_MOVEHITBIT_NON;
      }
    }
    
    if( attr != MAPATTR_ERROR )
    {
      MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
      
      if( (hit & MMDL_MOVEHITBIT_ATTR) )
      {
        u16 attr_dir = DIR_NOT;
        
        if( MAPATTR_VALUE_CheckJumpUp(val) ){
          attr_dir = DIR_UP;
        }else if( MAPATTR_VALUE_CheckJumpDown(val) ){
          attr_dir = DIR_DOWN;
        }else if( MAPATTR_VALUE_CheckumpLeft(val) ){
          attr_dir = DIR_LEFT;
        }else if( MAPATTR_VALUE_CheckJumpRight(val) ){
          attr_dir = DIR_RIGHT;
        }
        
        if( attr_dir != DIR_NOT && attr_dir == dir ){ //�W�����v������v
          return( PLAYER_SET_JUMP );
        }
      }
      
      if( hit == MMDL_MOVEHITBIT_NON )
      {
        if( (flag & MAPATTR_FLAGBIT_WATER) == 0 ){ //���n�`
          return( PLAYER_SET_WALK );
        }
        
        if( debug_flag == TRUE ){ //�f�o�b�O�ړ�
          return( PLAYER_SET_WALK );
        }
      }
    }
  }
  
  return( PLAYER_SET_HITCH );
}

#if 0
static PLAYER_SET gjiki_CheckMoveStart_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( gjiki_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  {
    u32 hit = MMDL_HitCheckMoveDir( mmdl, dir );
    
    if( debug_flag == TRUE )
    {
      if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit&MMDL_MOVEHITBIT_OUTRANGE) )
      {
        hit = MMDL_MOVEHITBIT_NON;
      }
    }
    
    if( hit == MMDL_MOVEHITBIT_NON )
    {
      return( PLAYER_SET_WALK );
    }
    
    if( (hit & MMDL_MOVEHITBIT_ATTR) )
    {
      BOOL ret;
      u32 attr;
      VecFx32 pos;
      
      MMDL_GetVectorPos( mmdl, &pos );
      MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
      ret = MMDL_GetMapPosAttr( mmdl, &pos, &attr );
      
      if( ret == TRUE )
      {
        u16 attr_dir = DIR_NOT;
        MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
        MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
        
        if( MAPATTR_VALUE_CheckJumpUp(val) ){
          attr_dir = DIR_UP;
        }else if( MAPATTR_VALUE_CheckJumpDown(val) ){
          attr_dir = DIR_DOWN;
        }else if( MAPATTR_VALUE_CheckumpLeft(val) ){
          attr_dir = DIR_LEFT;
        }else if( MAPATTR_VALUE_CheckJumpRight(val) ){
          attr_dir = DIR_RIGHT;
        }
        
        if( attr_dir != DIR_NOT && attr_dir == dir ) //�W�����v������v
        {
          return( PLAYER_SET_JUMP );
        }
      }
    }
  }
  
  return( PLAYER_SET_HITCH );
}
#endif

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�U�������
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_CheckMoveStart_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjiki_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjiki_CheckMoveStart_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��Q���q�b�g��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_CheckMoveStart_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( gjiki_CheckMoveStart_Walk(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjiki_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjiki_CheckMoveStart_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
//  �ړ��Z�b�g�@�ʏ�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�Z�b�g ���ɖ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Non(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��~
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  if( dir == DIR_NOT ){
    dir = MMDL_GetDirDisp( mmdl );
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_STOP;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_STOP );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �ړ�
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  
  if( debug_flag == TRUE ){
    code = AC_WALK_U_2F;
  }else if( key_cont & PAD_BUTTON_B ){
    code = AC_DASH_U_4F;
  }else{
    code = AC_WALK_U_8F;
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, code );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_WALK;

  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_WALK );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackAction();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �U�����
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_2F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_TURN;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_TURN );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��Q���q�b�g
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_HITCH;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_STOP );
  PMSND_PlaySE( SEQ_SE_WALL_HIT );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �W�����v
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Jump(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_WALK;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_WALK );
  PMSND_PlaySE( SEQ_SE_DANSA );

#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackAction();
#endif
}

//======================================================================
//  �O���b�h�ړ� �t�B�[���h�v���C���[����@�g���ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@�g���ړ�
 * @param  gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval nothing
 */
//--------------------------------------------------------------
static void jikiMove_Swim(
    FIELD_PLAYER_GRID *gjiki, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
  PLAYER_SET set;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
   
  set = PLAYER_SET_NON;
  switch( gjiki->move_state ){
  case PLAYER_MOVE_STOP:
    set = gjikiSwim_CheckMoveStart_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_WALK:
    set = gjikiSwim_CheckMoveStart_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_TURN:
    set = gjikiSwim_CheckMoveStart_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_HITCH:
    set = gjikiSwim_CheckMoveStart_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  if( set != PLAYER_SET_NON ){
    gjiki_InitMoveStartCommon( gjiki, key_cont );
  }

  switch( set ){
  case PLAYER_SET_NON:
    gjikiSwim_SetMove_Non(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_STOP:
    gjikiSwim_SetMove_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_WALK:
    gjikiSwim_SetMove_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_TURN:
    gjikiSwim_SetMove_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_HITCH:
    gjikiSwim_SetMove_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
    break;
  }
}

//======================================================================
//  �ړ��J�n�`�F�b�N�@�g���ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��~��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjikiSwim_CheckMoveStart_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( dir != old_dir && debug_flag == FALSE ){
        return( PLAYER_SET_TURN );
      }
      
      return( gjikiSwim_CheckMoveStart_Walk(
        gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
    }
    
    return( PLAYER_SET_STOP );
  }
  
  return( PLAYER_SET_NON );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�ړ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjikiSwim_CheckMoveStart_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( gjikiSwim_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  {
    MAPATTR attr;
    u32 hit = gjiki_HitCheckMove( gjiki, mmdl, dir, &attr );
    
    if( debug_flag == TRUE )
    {
      if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit&MMDL_MOVEHITBIT_OUTRANGE) )
      {
        hit = MMDL_MOVEHITBIT_NON;
      }
    }
    
    if( attr != MAPATTR_ERROR )
    {
      MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
      
      if( hit == MMDL_MOVEHITBIT_NON ){
        if( (flag & MAPATTR_FLAGBIT_WATER) ){
          return( PLAYER_SET_WALK );
        }
        
        if( debug_flag == TRUE ){
          return( PLAYER_SET_WALK );
        }
      }
    }
  }
  
  return( PLAYER_SET_HITCH );
}

#if 0
static PLAYER_SET gjikiSwim_CheckMoveStart_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( gjikiSwim_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  {
    u32 hit = MMDL_HitCheckMoveDir( mmdl, dir );
    
    if( debug_flag == TRUE )
    {
      if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit&MMDL_MOVEHITBIT_OUTRANGE) )
      {
        hit = MMDL_MOVEHITBIT_NON;
      }
    }
    
    if( hit == MMDL_MOVEHITBIT_NON )
    {
      return( PLAYER_SET_WALK );
    }
    
    if( (hit & MMDL_MOVEHITBIT_ATTR) )
    {
      BOOL ret;
      u32 attr;
      VecFx32 pos;
      
      MMDL_GetVectorPos( mmdl, &pos );
      MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
      ret = MMDL_GetMapPosAttr( mmdl, &pos, &attr );
      
      if( ret == TRUE )
      {
        u16 attr_dir = DIR_NOT;
        MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
        MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
        
        if( (flag & MAPATTR_FLAGBIT_WATER) ){
          return( PLAYER_SET_WALK );
        }
      }
    }
  }
  
  return( PLAYER_SET_HITCH );
}
#endif

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�U�������
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjikiSwim_CheckMoveStart_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjikiSwim_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjikiSwim_CheckMoveStart_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��Q���q�b�g��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjikiSwim_CheckMoveStart_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( gjikiSwim_CheckMoveStart_Walk(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjikiSwim_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjikiSwim_CheckMoveStart_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
//  �ړ��Z�b�g�@�g���ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�Z�b�g ���ɖ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiSwim_SetMove_Non(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��~
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiSwim_SetMove_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  if( dir == DIR_NOT ){
    dir = MMDL_GetDirDisp( mmdl );
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_STOP;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_STOP );

#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �ړ�
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiSwim_SetMove_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  
  if( debug_flag == TRUE ){
    code = AC_WALK_U_2F;
  }else{
    code = AC_WALK_U_4F;
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, code );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_WALK;

  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_WALK );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackAction();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �U�����
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiSwim_SetMove_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_2F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_TURN;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_TURN );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��Q���q�b�g
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiSwim_SetMove_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_HITCH;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_STOP );
  PMSND_PlaySE( SEQ_SE_WALL_HIT );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

//======================================================================
//  �O���b�h�ړ��@�t�B�[���h�v���C���[����@���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@���]�Ԉړ�
 * @param  gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval nothing
 */
//--------------------------------------------------------------
static void jikiMove_Cycle(
    FIELD_PLAYER_GRID *gjiki, int key_trg, int key_cont,
    u16 dir, BOOL debug_flag )
{
  PLAYER_SET set;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
   
  set = PLAYER_SET_NON;
  switch( gjiki->move_state ){
  case PLAYER_MOVE_STOP:
    set = gjikiCycle_CheckMoveStart_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_WALK:
    set = gjikiCycle_CheckMoveStart_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_TURN:
    set = gjikiCycle_CheckMoveStart_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_HITCH:
    set = gjikiCycle_CheckMoveStart_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  if( set != PLAYER_SET_NON ){
    gjiki_InitMoveStartCommon( gjiki, key_cont );
  }

  switch( set ){
  case PLAYER_SET_NON:
    gjikiCycle_SetMove_Non(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_STOP:
    gjikiCycle_SetMove_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_WALK:
    gjikiCycle_SetMove_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_TURN:
    gjikiCycle_SetMove_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_HITCH:
    gjikiCycle_SetMove_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_SET_JUMP:
    gjikiCycle_SetMove_Jump(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  }
}

//======================================================================
//  �ړ��J�n�`�F�b�N�@���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��~��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjikiCycle_CheckMoveStart_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( dir != old_dir && debug_flag == FALSE ){
        return( PLAYER_SET_TURN );
      }
      
      return( gjikiCycle_CheckMoveStart_Walk(
        gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
    }
    
    return( PLAYER_SET_STOP );
  }
  
  return( PLAYER_SET_NON );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�ړ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjikiCycle_CheckMoveStart_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( gjikiCycle_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  {
    u32 hit = MMDL_HitCheckMoveDir( mmdl, dir );
    
    if( debug_flag == TRUE )
    {
      if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit&MMDL_MOVEHITBIT_OUTRANGE) )
      {
        hit = MMDL_MOVEHITBIT_NON;
      }
    }
    
    if( hit == MMDL_MOVEHITBIT_NON )
    {
      return( PLAYER_SET_WALK );
    }
    
    if( (hit & MMDL_MOVEHITBIT_ATTR) )
    {
      BOOL ret;
      u32 attr;
      VecFx32 pos;
      
      MMDL_GetVectorPos( mmdl, &pos );
      MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
      ret = MMDL_GetMapPosAttr( mmdl, &pos, &attr );
      
      if( ret == TRUE )
      {
        u16 attr_dir = DIR_NOT;
        MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
        MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
        
        if( MAPATTR_VALUE_CheckJumpUp(val) ){
          attr_dir = DIR_UP;
        }else if( MAPATTR_VALUE_CheckJumpDown(val) ){
          attr_dir = DIR_DOWN;
        }else if( MAPATTR_VALUE_CheckumpLeft(val) ){
          attr_dir = DIR_LEFT;
        }else if( MAPATTR_VALUE_CheckJumpRight(val) ){
          attr_dir = DIR_RIGHT;
        }
        
        if( attr_dir != DIR_NOT && attr_dir == dir ) //�W�����v������v
        {
          return( PLAYER_SET_JUMP );
        }
      }
    }
  }
  
  return( PLAYER_SET_HITCH );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@�U�������
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjikiCycle_CheckMoveStart_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjikiCycle_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjikiCycle_CheckMoveStart_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N�@��Q���q�b�g��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjikiCycle_CheckMoveStart_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( gjikiCycle_CheckMoveStart_Walk(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( PLAYER_SET_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjikiCycle_CheckMoveStart_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjikiCycle_CheckMoveStart_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
//  �ړ��Z�b�g�@���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �ړ��J�n�Z�b�g ���ɖ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiCycle_SetMove_Non(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��~
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiCycle_SetMove_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  if( dir == DIR_NOT ){
    dir = MMDL_GetDirDisp( mmdl );
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_STOP;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_STOP );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �ړ�
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiCycle_SetMove_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  
  if( debug_flag == TRUE ){
    code = AC_WALK_U_2F;
  }else{
    code = AC_WALK_U_4F;
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, code );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_WALK;

  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_WALK );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackAction();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �U�����
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiCycle_SetMove_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_2F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_TURN;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_TURN );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g ��Q���q�b�g
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiCycle_SetMove_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( dir, AC_STAY_WALK_U_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_HITCH;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_STOP );
  PMSND_PlaySE( SEQ_SE_WALL_HIT );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g �W�����v
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiCycle_SetMove_Jump(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;
  
  GF_ASSERT( dir != DIR_NOT );
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_state = PLAYER_MOVE_WALK;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_WALK );
  PMSND_PlaySE( SEQ_SE_DANSA );
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackAction();
#endif
}

//======================================================================
//  ���N�G�X�g
//======================================================================
static void (* const data_gjikiRequestProcTbl[FIELD_PLAYER_GRID_REQBIT_MAX])( FIELD_PLAYER_GRID *gjiki );

static void gjiki_PlayBGM( FIELD_PLAYER_GRID *gjiki )
{
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( gjiki->fieldWork );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( gjiki->fld_player );
  u32 zone_id = FIELDMAP_GetZoneID( gjiki->fieldWork );
  u32 no = FIELD_SOUND_GetFieldBGMNo( gdata, form, zone_id );
  FIELD_SOUND_PlayNextBGM( no );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g 
 * @param gjiki FIELD_PLAYER_GRID
 * @param reqbit FIELD_PLAYER_GRID_REQBIT
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_SetRequest(
  FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_REQBIT req_bit )
{
  gjiki->req_bit = req_bit;
}

//--------------------------------------------------------------
/**
 * ���N�G�X�g���X�V
 * @param gjiki FIELD_PLAYER_GRID
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_UpdateRequest( FIELD_PLAYER_GRID *gjiki )
{
  int i = 0;
  FIELD_PLAYER_GRID_REQBIT req_bit = gjiki->req_bit;
  
  while( i < FIELD_PLAYER_GRID_REQBIT_MAX ){
    if( (req_bit&0x01) ){
      data_gjikiRequestProcTbl[i]( gjiki );
    }
    req_bit >>= 1;
    i++;
  }
   
  gjiki->req_bit = 0;
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�ʏ�ړ��ɕύX
 * @param gjiki FIELD_PLAYER_GIRD
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetNormal( FIELD_PLAYER_GRID *gjiki )
{
  int sex;
  u16 code;
  MMDL *mmdl;
  
  sex = FIELD_PLAYER_GetSex( gjiki->fld_player );
  mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_NORMAL );
  
  if( MMDL_GetOBJCode(mmdl) != code ){
    MMDL_ChangeOBJCode( mmdl, code );
  }

  FIELD_PLAYER_SetMoveForm( gjiki->fld_player, PLAYER_MOVE_FORM_NORMAL );
  gjiki_PlayBGM( gjiki );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@���]�Ԉړ��ɕύX
 * @param gjiki FIELD_PLAYER_GIRD
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetCycle( FIELD_PLAYER_GRID *gjiki )
{
  int sex;
  u16 code;
  MMDL *mmdl;
  
  sex = FIELD_PLAYER_GetSex( gjiki->fld_player );
  mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_CYCLE );
  
  if( MMDL_GetOBJCode(mmdl) != code ){
    MMDL_ChangeOBJCode( mmdl, code );
  }
   
  FIELD_PLAYER_SetMoveForm( gjiki->fld_player, PLAYER_MOVE_FORM_CYCLE );
  gjiki_PlayBGM( gjiki );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�g���ړ��ɕύX
 * @param gjiki FIELD_PLAYER_GRID
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetSwim( FIELD_PLAYER_GRID *gjiki )
{
  int sex;
  u16 code;
  MMDL *mmdl;
  
  sex = FIELD_PLAYER_GetSex( gjiki->fld_player );
  mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_SWIM );

  if( MMDL_GetOBJCode(mmdl) != code ){
    MMDL_ChangeOBJCode( mmdl, code );
  }
  
  FIELD_PLAYER_SetMoveForm( gjiki->fld_player, PLAYER_MOVE_FORM_SWIM );
  gjiki_PlayBGM( gjiki );
  
  if( gjiki->fldeff_joint == NULL ){ //�g���|�P����
    u16 dir;
    VecFx32 pos;
    FLDEFF_CTRL *fectrl;
    
    fectrl = FIELDMAP_GetFldEffCtrl( gjiki->fieldWork );
    dir = MMDL_GetDirDisp( mmdl );
    MMDL_GetVectorPos( mmdl, &pos );
    
    gjiki->fldeff_joint = FLDEFF_NAMIPOKE_SetMMdl(
        fectrl, dir, &pos, mmdl, TRUE );
  }
}

//--------------------------------------------------------------
/// ���@���N�G�X�g�����e�[�u��
//--------------------------------------------------------------
static void (* const data_gjikiRequestProcTbl[FIELD_PLAYER_GRID_REQBIT_MAX])( FIELD_PLAYER_GRID *gjiki ) =
{
  gjikiReq_SetNormal, //FIELD_PLAYER_GRID_REQBIT_NORMAL
  gjikiReq_SetCycle, //FIELD_PLAYER_GRID_REQBIT_CYCLE
  gjikiReq_SetSwim, //FIELD_PLAYER_GRID_REQBIT_SWIM
};

//======================================================================
//  �ړ��`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ړ��`�F�b�N
 * @param
 * @retval
 */
//--------------------------------------------------------------
static u32 gjiki_HitCheckMove(
    FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, u16 dir, MAPATTR *attr )
{
  u32 hit;
  VecFx32 pos;
  s16 gx = MMDL_GetGridPosX( mmdl );
  s16 gy = MMDL_GetHeightGrid( mmdl );
  s16 gz = MMDL_GetGridPosZ( mmdl );
  PLAYER_MOVEHITBIT ret = PLAYER_MOVEHITBIT_NONE;
  
  gx += MMDL_TOOL_GetDirAddValueGridX( dir );
  gz += MMDL_TOOL_GetDirAddValueGridZ( dir );
  
  hit = MMDL_HitCheckMoveCurrent( mmdl, gx, gy, gz, dir );
  
  MMDL_GetVectorPos( mmdl, &pos );
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  
  if( MMDL_GetMapPosAttr(mmdl,&pos,attr) == FALSE ){
    *attr = MAPATTR_ERROR;
  }
  
  return( hit );
}

//======================================================================
//  �T�E���h
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * �T�E���h�e�X�g�@���@��~
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_Sound_MoveStop( void )
{
  //�T�E���h�e�X�g�i��~�j
  u16 trackBit = 0xfcff; // track 9,10 OFF
  PMSND_ChangeBGMtrack(trackBit);
}

//--------------------------------------------------------------
/**
 * �T�E���h�e�X�g�@���@�ړ�
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_Sound_Move( void )
{
  //�T�E���h�e�X�g�i�ړ��j
  u16 trackBit = 0xffff; // �Strack ON
  PMSND_ChangeBGMtrack(trackBit);
}
#endif

//======================================================================
//  �c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@��������~������B�O���b�h��p
 * @param fld_player FIELD_PLAYER
 * @retval nothing
 * @note ���@��������~�o���Ȃ��ꍇ�͂��̂܂�
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_ForceStop( FIELD_PLAYER *fld_player )
{
  FIELDMAP_WORK *fieldWork = FIELD_PLAYER_GetFieldMapWork( fld_player );
  FIELDMAP_CTRL_GRID *gridMap = FIELDMAP_GetMapCtrlWork( fieldWork );
  FIELD_PLAYER_GRID *gjiki = FIELDMAP_CTRL_GRID_GetFieldPlayerGrid( gridMap );

  if( gjiki->move_state == PLAYER_MOVE_HITCH ){
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
    u16 dir = MMDL_GetDirDisp( mmdl );
    MMDL_FreeAcmd( mmdl );
    MMDL_SetDirDisp( mmdl, dir );
    MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
    FIELD_PLAYER_SetMoveValue( fld_player, PLAYER_MOVE_VALUE_STOP );
  }
}

//--------------------------------------------------------------
/**
 * ���@�ɔg���|�P�����̃^�X�N�|�C���^���Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_SetEffectTaskWork(
    FIELD_PLAYER_GRID *gjiki, FLDEFF_TASK *task )
{
  gjiki->fldeff_joint = task;
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
FLDEFF_TASK * FIELD_PLAYER_GRID_GetEffectTaskWork(
    FIELD_PLAYER_GRID *gjiki )
{
  return( gjiki->fldeff_joint );
}


//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetNaminori( FIELD_PLAYER_GRID *gjiki )
{
  int sex;
  u16 code;
  MMDL *mmdl;
   
  sex = FIELD_PLAYER_GetSex( gjiki->fld_player );
  mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_SWIM );

  if( MMDL_GetOBJCode(mmdl) != code ){
    MMDL_ChangeOBJCode( mmdl, code );
  }
  
  FIELD_PLAYER_SetMoveForm( gjiki->fld_player, PLAYER_MOVE_FORM_SWIM );
  gjiki_PlayBGM( gjiki );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
void FIELD_PLAYER_SetNaminoriEnd( FIELD_PLAYER_GRID *gjiki )
{
  VecFx32 offs = {0,0,0};
  int sex;
  u16 code;
  MMDL *mmdl;
  
  sex = FIELD_PLAYER_GetSex( gjiki->fld_player );
  mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  code = FIELD_PLAYER_GetDrawFormToOBJCode( sex, PLAYER_DRAW_FORM_NORMAL );
  
  if( MMDL_GetOBJCode(mmdl) != code ){
    MMDL_ChangeOBJCode( mmdl, code );
  }
  
  FIELD_PLAYER_SetMoveForm( gjiki->fld_player, PLAYER_MOVE_FORM_NORMAL );
  gjiki_PlayBGM( gjiki );
  
  if( gjiki->fldeff_joint != NULL ){
    FLDEFF_TASK_CallDelete( gjiki->fldeff_joint );
    gjiki->fldeff_joint = NULL;
  }
  
  MMDL_SetVectorOuterDrawOffsetPos( mmdl, &offs );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
u16 FIELD_PLAYER_GRID_GetKeyDir( FIELD_PLAYER_GRID *gjiki, int key )
{
  u16 dir = DIR_NOT;
  if( (key&PAD_KEY_UP) ){
    dir = DIR_UP;
  }else if( (key&PAD_KEY_DOWN) ){
    dir = DIR_DOWN;
  }else if( (key&PAD_KEY_LEFT) ){
    dir = DIR_LEFT;
  }else if( (key&PAD_KEY_RIGHT) ){
    dir = DIR_RIGHT;
  }
  return( dir );
}


//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT ON
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void gjiki_OnMoveBit(
    FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_MOVEBIT bit )
{
  GF_ASSERT( bit < FIELD_PLAYER_GRID_MOVEBIT_BITMAX );
  gjiki->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT OFF
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void gjiki_OffMoveBit(
    FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_MOVEBIT bit )
{
  GF_ASSERT( bit < FIELD_PLAYER_GRID_MOVEBIT_BITMAX );
  gjiki->move_bit &= ~bit;
}

static void gjiki_OnMoveBitForce( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OnMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_FORCE );
}
static void gjiki_OffMoveBitForce( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OffMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_FORCE );
}

static void gjiki_OnMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OnMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_UNDER_OFF );
}
static void gjiki_OffMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OffMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_UNDER_OFF );
}

static void gjiki_OnMoveBitStep( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OnMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_STEP );
}
static void gjiki_OffMoveBitStep( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OffMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_STEP );
}
