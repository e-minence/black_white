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

#include "fieldmap_ctrl.h"
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

//--------------------------------------------------------------
/// UNDER
//--------------------------------------------------------------
typedef enum
{
  UNDER_NONE = 0, //��������
  UNDER_ICE, //���鏰
  UNDER_MAX,
}UNDER;

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// FIELD_PLAYER_GRID
//--------------------------------------------------------------
struct _TAG_FIELD_PLAYER_GRID
{
  int move_state;
  PLAYER_SET move_set;
  
  FIELD_PLAYER_GRID_MOVEBIT move_bit;
  FIELD_PLAYER_REQBIT req_bit;
  
  FIELD_PLAYER *fld_player;
  FIELDMAP_WORK *fieldWork;
  FLDEFF_TASK *fldeff_joint;
  
  u16 input_key_dir_x; //�L�[���͉�����
  u16 input_key_dir_z; //�L�[���͏c����
};

//======================================================================
//  proto
//======================================================================
static void gjiki_InitMoveStartCommon(
    FIELD_PLAYER_GRID *gjiki, u16 key_prs, PLAYER_SET set );
static BOOL gjiki_CheckMoveStart( FIELD_PLAYER_GRID *gjiki, u16 dir );
static void gjiki_PlaySE( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set, u16 dir );

//�L�[���͏���
static u16 gjiki_GetInputKeyDir(
    FIELD_PLAYER_GRID *gjiki, u16 key_prs );
static void gjiki_SetInputKeyDir( FIELD_PLAYER_GRID *gjiki, u16 key_prs );
static u16 getKeyDirX( u16 key_prs );
static u16 getKeyDirZ( u16 key_prs );

//�ړ��J�n���
static PLAYER_SET gjiki_GetMoveStartSet( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag );

static PLAYER_SET gjiki_GetMoveStartSet_Normal( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag );
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

static PLAYER_SET gjiki_GetMoveStartSet_Cycle( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag );
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

static PLAYER_SET gjiki_GetMoveStartSet_Swim( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag );
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

//�ړ��Z�b�g
static void gjiki_SetMove( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void gjiki_SetMove_Normal( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set,
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

static void gjiki_SetMove_Cycle( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set,
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

static void gjiki_SetMove_Swim( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
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

//���N�G�X�g
static void (* const data_gjikiRequestProcTbl[
    FIELD_PLAYER_REQBIT_MAX])( FIELD_PLAYER_GRID *gjiki );

//�q�b�g�`�F�b�N
static u32 gjiki_HitCheckMove(
    FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, u16 dir, MAPATTR *attr );

//�T�E���h
#if 0
static void gjiki_Sound_MoveStop( void );
static void gjiki_Sound_Move( void );
#endif

//���@����r�b�g
static void gjiki_OnMoveBit(
    FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_MOVEBIT bit );
static void gjiki_OffMoveBit(
    FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_MOVEBIT bit );
static BOOL gjiki_CheckMoveBit(
    FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_MOVEBIT bit );
static void gjiki_OnMoveBitForce( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OffMoveBitForce( FIELD_PLAYER_GRID *gjiki );
static BOOL gjiki_CheckMoveBitForce( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OnMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OffMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OnMoveBitStep( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OffMoveBitStep( FIELD_PLAYER_GRID *gjiki );

static UNDER gjiki_CheckUnder( FIELD_PLAYER_GRID *gjiki, u16 dir );

static BOOL gjiki_ControlUnder(
    FIELD_PLAYER_GRID *gjiki, u16 dir, BOOL debug );

//�p�[�c

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

  gjiki_OnMoveBitUnderOff( gjiki );
  
  //���A
  {
    PLAYER_MOVE_FORM form;
    form = FIELD_PLAYER_GetMoveForm( gjiki->fld_player );
    
    switch( form ){
    case PLAYER_MOVE_FORM_SWIM:
      FIELD_PLAYER_GRID_SetRequest(
          gjiki, FIELD_PLAYER_REQBIT_SWIM );
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
  PLAYER_SET set;
  BOOL debug_flag = FALSE;
  
  dir = gjiki_GetInputKeyDir( gjiki, key_cont );
  
  if( gjiki_CheckMoveStart(gjiki,dir) == FALSE ){
    return;
  }
  
#ifdef PM_DEBUG
  if( key_cont & PAD_BUTTON_R ){
    debug_flag = TRUE;
  }
#endif
  
  FIELD_PLAYER_GRID_UpdateRequest( gjiki );
  
  if( gjiki_ControlUnder(gjiki,dir,debug_flag) == TRUE ){
    return;
  }
  
  set = gjiki_GetMoveStartSet( gjiki, key_trg, key_cont, dir, debug_flag );
  
  gjiki_InitMoveStartCommon( gjiki, key_cont, set );
  gjiki_SetMove( gjiki, set, key_trg, key_cont, dir, debug_flag );
  gjiki_PlaySE( gjiki, set, dir );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɍs������������
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_prs �L�[���͏��
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_InitMoveStartCommon(
    FIELD_PLAYER_GRID *gjiki, u16 key_prs, PLAYER_SET set )
{
  gjiki->move_set = set;
  gjiki_SetInputKeyDir( gjiki, key_prs );
  gjiki_OffMoveBitStep( gjiki );
  
  if( set == PLAYER_SET_WALK ){
    gjiki_OffMoveBitUnderOff( gjiki );
  }
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL gjiki_CheckMoveStart( FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    return( TRUE ); //�Z�b�g�\
  }
  
  if( dir != DIR_NOT ){
    if( gjiki->move_state == PLAYER_MOVE_HITCH ){
      if( dir != MMDL_GetDirDisp(mmdl) ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɖ炷SE
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void gjiki_PlaySE( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set, u16 dir )
{
  if( set == PLAYER_SET_WALK ){
    VecFx32 pos;
    MAPATTR attr;
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
    
    MMDL_GetVectorPos( mmdl, &pos );
    MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
    
    if( MMDL_GetMapPosAttr(mmdl,&pos,&attr) == TRUE ){
      int se;
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
      MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
      
      if( (flag & MAPATTR_FLAGBIT_GRASS) ){
        se = SEQ_SE_FLD_09;
        if( MAPATTR_VALUE_CheckLongGrass(val) == TRUE ){
          se = SEQ_SE_FLD_08;
        }
        PMSND_PlaySE( se );
      }else{
        if( MAPATTR_VALUE_CheckShoal(val) == TRUE ||
            MAPATTR_VALUE_CheckPool(val) == TRUE  ||
            MAPATTR_VALUE_CheckMarsh(val) == TRUE ){
          PMSND_PlaySE( SEQ_SE_FLD_13 );
        }
      }
    }
  }
}

//======================================================================
//  �L�[���͏���
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͕����擾
 * @param gjiki
 * @param key_prs �L�[���͏��
 * @retval u16 �L�[���͕���
 */
//--------------------------------------------------------------
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

//--------------------------------------------------------------
/**
 * �L�[���͏��Z�b�g
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_prs �L�[���͏��
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetInputKeyDir( FIELD_PLAYER_GRID *gjiki, u16 key_prs )
{
  gjiki->input_key_dir_x = getKeyDirX( key_prs );
  gjiki->input_key_dir_z = getKeyDirZ( key_prs );
}

//--------------------------------------------------------------
/**
 * �L�[�v���X���牟����Ă���X�������擾
 * @param key_prs �L�[���͏��
 * @retval u16 ������Ă������ DIR_UP��
 */
//--------------------------------------------------------------
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

//--------------------------------------------------------------
/**
 * �L�[�v���X���牟����Ă���Z�������擾
 * @param key_prs �L�[���͏��
 * @retval u16 ������Ă������ DIR_UP��
 */
//--------------------------------------------------------------
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

//======================================================================
// �L�[���͂ɂ�鎩�@�ړ��J�n��Ԃ��擾
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��J�n��Ԃ��擾
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �L�[���͕���
 * @param debug_flag �f�o�b�O�ړ��t���O TRUE=�f�o�b�O�ړ�
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_GetMoveStartSet( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag )
{
  PLAYER_SET set;
  PLAYER_MOVE_FORM form;
  
  set = PLAYER_SET_NON;
  form = FIELD_PLAYER_GetMoveForm( gjiki->fld_player );
  
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    set = gjiki_GetMoveStartSet_Normal(
        gjiki, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_FORM_CYCLE:
    set = gjiki_GetMoveStartSet_Cycle(
        gjiki, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_FORM_SWIM:
    set = gjiki_GetMoveStartSet_Swim(
        gjiki, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  return( set );
}

//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��J�n��Ԃ��擾
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �L�[���͕���
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
PLAYER_MOVE_VALUE FIELD_PLAYER_GRID_GetMoveValue(
    FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  PLAYER_MOVE_VALUE val = PLAYER_MOVE_VALUE_STOP;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  u16 m_dir = MMDL_GetDirDisp( mmdl );
  
  if( dir == DIR_NOT ){
    return( PLAYER_MOVE_VALUE_STOP );
  }
  
  if( dir != m_dir && gjiki->move_state != PLAYER_MOVE_WALK ){
    return( PLAYER_MOVE_VALUE_TURN );
  }
  
  return( PLAYER_MOVE_VALUE_WALK );
}

//======================================================================
// �L�[���͂ɂ�鎩�@�ړ��J�n��Ԃ��擾�@�ʏ�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��J�n��Ԃ��擾 �ʏ�ړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �L�[���͕���
 * @param debug_flag �f�o�b�O�ړ��t���O TRUE=�f�o�b�O�ړ�
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_GetMoveStartSet_Normal( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag )
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
  
  return( set );
}

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
// �L�[���͂ɂ�鎩�@��ԕω����擾�@���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��J�n��Ԃ��擾�@���]�Ԉړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �L�[���͕���
 * @param debug_flag �f�o�b�O�ړ��t���O TRUE=�f�o�b�O�ړ�
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_GetMoveStartSet_Cycle( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag )
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
  
  return( set );
}

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
// �L�[���͂ɂ�鎩�@��ԕω����擾�@�g���ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��J�n��Ԃ��擾�@���]�Ԉړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �L�[���͕���
 * @param debug_flag �f�o�b�O�ړ��t���O TRUE=�f�o�b�O�ړ�
 * @retval PLAYER_SET
 */
//--------------------------------------------------------------
static PLAYER_SET gjiki_GetMoveStartSet_Swim( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag )
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
  
  return( set );
}

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
//  ���@�ړ��Z�b�g
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ړ��Z�b�g
 * @param gjiki FIELD_PLAYER_GRID
 * @param set PLAYER_SET
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  PLAYER_MOVE_FORM form;
  
  form = FIELD_PLAYER_GetMoveForm( gjiki->fld_player );
  
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    gjiki_SetMove_Normal( gjiki, set, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_FORM_CYCLE:
    gjiki_SetMove_Cycle( gjiki, set, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_FORM_SWIM:
    gjiki_SetMove_Swim( gjiki, set, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
}

//======================================================================
//  ���@�ړ��Z�b�g  �ʏ�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ړ��Z�b�g�@�ʏ�ړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param set PLAYER_SET
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Normal( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  
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
  default:
    GF_ASSERT( 0 );
  }
}

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
    
    if( FIELD_PLAYER_CheckIllegalOBJCode(gjiki->fld_player) == FALSE ){
      code = AC_WALK_U_4F;
    }
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
//  ���@�ړ��Z�b�g  ���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ړ��Z�b�g�@���]�Ԉړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param set PLAYER_SET
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Cycle( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );

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
  default:
    GF_ASSERT( 0 );
  }
}

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
    code = AC_WALK_U_2F;
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
//  ���@�ړ��Z�b�g  �g���ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ړ��Z�b�g�@�g���ړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param set PLAYER_SET
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Swim( FIELD_PLAYER_GRID *gjiki, PLAYER_SET set,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );

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
//  ���N�G�X�g
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�̌`�Ԃɍ��킹��BGM���Đ�
 * @param gjiki FIELD_PLAYER_GRID
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_PlayBGM( FIELD_PLAYER_GRID *gjiki )
{
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( gjiki->fieldWork );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gdata );
  PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( gjiki->fld_player );
  u32 zone_id = FIELDMAP_GetZoneID( gjiki->fieldWork );
  u32 no = FIELD_SOUND_GetFieldBGMNo( gdata, form, zone_id );
  FIELD_SOUND_PlayNextBGM( fsnd, no );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g 
 * @param gjiki FIELD_PLAYER_GRID
 * @param reqbit FIELD_PLAYER_REQBIT
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_SetRequest(
  FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_REQBIT req_bit )
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
  FIELD_PLAYER_REQBIT req_bit = gjiki->req_bit;
  
  while( i < FIELD_PLAYER_REQBIT_MAX ){
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
        fectrl, dir, &pos, mmdl, NAMIPOKE_JOINT_ON );
  }
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@����`�Ԃɂ��킹���\���ɂ���
 * @param gjiki FIELD_PLAYER_GRID
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetMoveFormToDrawForm( FIELD_PLAYER_GRID *gjiki )
{
  FIELD_PLAYER_ResetMoveForm( gjiki->fld_player );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@�A�C�e���Q�b�g���@�ɕύX
 * @param gjiki FIELD_PLYER
 * @retval  nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetItemGetHero( FIELD_PLAYER_GRID *gjiki )
{
  FIELD_PLAYER_ChangeDrawForm( gjiki->fld_player, PLAYER_DRAW_FORM_ITEMGET );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@PC�a�����@�ɕύX
 * @param gjiki FIELD_PLYER
 * @retval  nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetReportHero( FIELD_PLAYER_GRID *gjiki )
{
  FIELD_PLAYER_ChangeDrawForm( gjiki->fld_player, PLAYER_DRAW_FORM_SAVEHERO );
}

//--------------------------------------------------------------
/**
 * ���@���N�G�X�g�����@PC�a�����@�ɕύX
 * @param gjiki FIELD_PLYER
 * @retval  nothing
 */
//--------------------------------------------------------------
static void gjikiReq_SetPCAzukeHero( FIELD_PLAYER_GRID *gjiki )
{
  FIELD_PLAYER_ChangeDrawForm( gjiki->fld_player, PLAYER_DRAW_FORM_PCHERO );
}

//--------------------------------------------------------------
/// ���@���N�G�X�g�����e�[�u��
//--------------------------------------------------------------
static void (* const data_gjikiRequestProcTbl[FIELD_PLAYER_REQBIT_MAX])( FIELD_PLAYER_GRID *gjiki ) =
{
  gjikiReq_SetNormal, //FIELD_PLAYER_REQBIT_NORMAL
  gjikiReq_SetCycle, //FIELD_PLAYER_REQBIT_CYCLE
  gjikiReq_SetSwim, //FIELD_PLAYER_REQBIT_SWIM
  gjikiReq_SetMoveFormToDrawForm,
  gjikiReq_SetItemGetHero,
  gjikiReq_SetReportHero,
  gjikiReq_SetPCAzukeHero,
};

//======================================================================
//  �ړ��`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ړ��`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL
 * @param dir �ړ�����
 * @param attr �ړ���̃A�g���r���[�g���i�[
 * @retval u32 �q�b�g�r�b�g�BMMDL_MOVEHITBIT_NON��
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
  }else if( (hit&MMDL_MOVEHITBIT_ATTR) ){ //�ꕔ�����Ƃ��锻����`�F�b�N
    MAPATTR_VALUE attr_val;
    attr_val = MAPATTR_GetAttrValue( *attr );
    
    if( MAPATTR_VALUE_CheckKairikiAna(attr_val) == TRUE ){
      const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
      
      KAGAYA_Printf( "���`�F�b�N���܂���\n" );
      
      if( MMDLSYS_ROCKPOS_CheckRockFalled(mmdlsys,&pos) == TRUE ){
        hit &= ~MMDL_MOVEHITBIT_ATTR;
      }
    }
  }
  
  return( hit );
}

//--------------------------------------------------------------
/**
 * ���@�ړ��J�n�`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �ړ������BDIR_UP��
 * @retval BOOL TRUE=�ړ��\ FALSE=�ړ��s��
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_CheckStartMove( FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    return( TRUE );
  }
  
  if( dir == DIR_NOT ){
    return( FALSE );
  }
  
#if 0  
  if( Jiki_WallHitAcmdCodeCheck(code) == TRUE ){ //�ǃq�b�g�̓L�����Z���\
  }
#endif
  return( FALSE );
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
//  ���@����r�b�g
//======================================================================
//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT ON
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit FIELD_PLAYER_GRID_MOVEBIT
 * @retval nothing
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
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit FIELD_PLAYER_GRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OffMoveBit(
    FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_MOVEBIT bit )
{
  GF_ASSERT( bit < FIELD_PLAYER_GRID_MOVEBIT_BITMAX );
  gjiki->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT Check
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit FIELD_PLAYER_GRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static BOOL gjiki_CheckMoveBit(
    FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_MOVEBIT bit )
{
  GF_ASSERT( bit < FIELD_PLAYER_GRID_MOVEBIT_BITMAX );
  if( (gjiki->move_bit & bit) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT �����ړ��r�b�gON
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit FIELD_PLAYER_GRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OnMoveBitForce( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OnMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_FORCE );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT �����ړ��r�b�gOFF
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit FIELD_PLAYER_GRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OffMoveBitForce( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OffMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_FORCE );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT �����ړ��r�b�g�`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @retval BOOL 
 */
//--------------------------------------------------------------
static BOOL gjiki_CheckMoveBitForce( FIELD_PLAYER_GRID *gjiki )
{
  if( gjiki_CheckMoveBit(gjiki,FIELD_PLAYER_GRID_MOVEBIT_FORCE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT ��������ON
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit FIELD_PLAYER_GRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OnMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OnMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_UNDER_OFF );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT ��������OFF
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit FIELD_PLAYER_GRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OffMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OffMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_UNDER_OFF );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT ����ړ� ON
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit FIELD_PLAYER_GRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OnMoveBitStep( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OnMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_STEP );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID_MOVEBIT ����ړ� OFF
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit FIELD_PLAYER_GRID_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OffMoveBitStep( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OffMoveBit( gjiki, FIELD_PLAYER_GRID_MOVEBIT_STEP );
}

//======================================================================
//  parts
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
  FIELD_PLAYER_GRID *gjiki = FIELDMAP_GetPlayerGrid( fieldWork );

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
 * ���@�@�����~
 * @param fld_player
 * @retval BOOL TRUE=��~�����BFALSE=�ړ����ɂ���~�o���Ȃ��B
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_SetMoveStop( FIELD_PLAYER_GRID *gjiki )
{
  switch( gjiki->move_state ){
  case PLAYER_MOVE_STOP:
    break;
  case PLAYER_MOVE_WALK:
    return( FALSE );
  case PLAYER_MOVE_TURN:
  case PLAYER_MOVE_HITCH:
    {
      MMDL *mmdl = FIELD_PLAYER_GRID_GetMMdl( gjiki );
      u16 dir = MMDL_GetDirDisp( mmdl );
      MMDL_FreeAcmd( mmdl );
      MMDL_SetDirDisp( mmdl, dir );
      MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
      FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_STOP );
      break;
    }
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���@�ɔg���|�P�����̃^�X�N�|�C���^���Z�b�g
 * @param gjiki FIELD_PLAYER_GRID
 * @param task �Z�b�g����FLDEFF_TASK
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_SetEffectTaskWork(
    FIELD_PLAYER_GRID *gjiki, FLDEFF_TASK *task )
{
  gjiki->fldeff_joint = task;
}

//--------------------------------------------------------------
/**
 * ���@�������Ă���G�t�F�N�g�^�X�N�̃|�C���^���擾
 * @param   gjiki FIELD_PLAYER_GRID
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
FLDEFF_TASK * FIELD_PLAYER_GRID_GetEffectTaskWork(
    FIELD_PLAYER_GRID *gjiki )
{
  return( gjiki->fldeff_joint );
}

//--------------------------------------------------------------
/**
 * ���@��g����Ԃɂ���
 * @param gjiki FIELD_PLAYER_GRID
 * @retval nothing
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
 * ���@�g����Ԃ��I������
 * @param gjiki FIELD_PLAYER_GRID
 * @retval nothing
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
 * ���@�@���̓L�[����ړ����������Ԃ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param key �L�[���
 * @retval u16 �ړ����� DIR_UP��
 */
//--------------------------------------------------------------
u16 FIELD_PLAYER_GRID_GetKeyDir( FIELD_PLAYER_GRID *gjiki, int key )
{
  u16 dir = gjiki_GetInputKeyDir( gjiki, key );
  return( dir );
}

//--------------------------------------------------------------
/**
 * ���@�������`�F�b�N����
 * @param
 * @retval
 */
//--------------------------------------------------------------
static UNDER gjiki_CheckUnder( FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  MAPATTR attr = MMDL_GetMapAttr( mmdl );
  
  if( gjiki_CheckMoveBit(
        gjiki,FIELD_PLAYER_GRID_MOVEBIT_UNDER_OFF) == FALSE ){
    int i = 0;
    MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
    
    if( MAPATTR_VALUE_CheckIce(val) == TRUE ){
      return( UNDER_ICE );
    }
  }
  
  return( UNDER_NONE );
}

//--------------------------------------------------------------
/**
 * ���@�����������ړ����ǂ���
 * @param
 * @retval
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_CheckUnderForceMove( FIELD_PLAYER *fld_player )
{
  FIELDMAP_WORK *fieldmap = FIELD_PLAYER_GetFieldMapWork( fld_player );
  FIELD_PLAYER_GRID *gjiki = FIELDMAP_GetPlayerGrid( fieldmap );
  UNDER under = gjiki_CheckUnder( gjiki, DIR_NOT );
  
  if( under != UNDER_NONE ){
    return( TRUE );
  }
  return( FALSE );
}

//======================================================================
//  ���@��������
//======================================================================
//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
static void gjiki_ClearUnderForceMove( FIELD_PLAYER_GRID *gjiki )
{
  if( gjiki_CheckMoveBitForce(gjiki) == TRUE ){
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_PAUSE_DIR|MMDL_STABIT_PAUSE_ANM );
    gjiki_OffMoveBitForce( gjiki );
  }
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL gjikiUnder_None( FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  gjiki_ClearUnderForceMove( gjiki );
  return( FALSE );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL gjikiUnder_Ice( FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  MAPATTR attr;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  u16 jiki_dir = MMDL_GetDirMove( mmdl );
  u32 hit = gjiki_HitCheckMove( gjiki, mmdl, jiki_dir, &attr );
	
  if( hit != MMDL_MOVEHITBIT_NON ){ //��Q���q�b�g
    gjiki_ClearUnderForceMove( gjiki );
    gjiki_OnMoveBitUnderOff( gjiki );
    FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_STOP );
    return( FALSE );
  }
  
  gjiki_OnMoveBitForce( gjiki );
  
  {
	  u16 code = MMDL_ChangeDirAcmdCode( jiki_dir, AC_WALK_U_4F );
    
    MMDL_SetAcmd( mmdl, code );
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_PAUSE_DIR|MMDL_STABIT_PAUSE_ANM );
    
    gjiki->move_state = PLAYER_MOVE_WALK;
    FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_WALK );
	  return( TRUE );
  }
}

static BOOL (* const data_MoveUnderFuncTbl[UNDER_MAX])(FIELD_PLAYER_GRID*,u16) =
{
  gjikiUnder_None,
  gjikiUnder_Ice,
};

//--------------------------------------------------------------
/**
 * ���@��������
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL gjiki_ControlUnder(
    FIELD_PLAYER_GRID *gjiki, u16 dir, BOOL debug )
{
  if( debug == FALSE ){
    UNDER under = gjiki_CheckUnder( gjiki, dir );
    
    if( data_MoveUnderFuncTbl[under](gjiki,dir) == TRUE ){
      return( TRUE );
    }
  }
  
  gjiki_OffMoveBitForce( gjiki );
  return( FALSE );
}

//======================================================================
//  FIELD_PLAYER_GRID�Q��
//======================================================================
//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID FIELD_PLAYER�擾
 * @param gjiki FIELD_PLAYER_GRID
 * @retval FIELD_PLAYER*
 */
//--------------------------------------------------------------
FIELD_PLAYER * FIELD_PLAYER_GRID_GetFieldPlayer( FIELD_PLAYER_GRID *gjiki )
{
  return( gjiki->fld_player );
}

//--------------------------------------------------------------
/**
 * FIELD_PLAYER_GRID �g�p���Ă��铮�샂�f���擾
 * @param gjiki FIELD_PLAYER_GRID
 * @retval MMDL*
 */
//--------------------------------------------------------------
MMDL * FIELD_PLAYER_GRID_GetMMdl( FIELD_PLAYER_GRID *gjiki )
{
  return( FIELD_PLAYER_GetMMdl(gjiki->fld_player) );
}

