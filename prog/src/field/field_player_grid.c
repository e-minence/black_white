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

#include "fldeff_kemuri.h"

//======================================================================
//  define
//======================================================================
//#define PLAYER_MOVE_TRACK_CHANGE //��`�Ŏ��@���쎞�Ƀg���b�N�ύX

///���@��p�q�b�g�`�F�b�N�r�b�g���o��
#define EX_MOVEHITBIT(bit_no) (1<<(MMDL_MOVEHITBIT_BITMAX+bit_no))

///�����������E�t���[��
#define OZE_YURE_MAXFRAME (30*2)

//--------------------------------------------------------------
/// JIKI_MOVEBIT
//--------------------------------------------------------------
typedef enum
{
  ///���ɖ���
  JIKI_MOVEBIT_NON = (0),
  ///�����ړ���
  JIKI_MOVEBIT_FORCE       = (1<<0),
  ///��������
  JIKI_MOVEBIT_UNDER_OFF   = (1<<1),
  ///����ړ�
  JIKI_MOVEBIT_STEP        = (1<<2),
  ///���������
  JIKI_MOVEBIT_OZE_YURE    = (1<<3),
  ///����������
  JIKI_MOVEBIT_OZE_FALLOUT = (1<<4),
  ///�ő�
  JIKI_MOVEBIT_BITMAX      = (1<<5),
}JIKI_MOVEBIT;

//--------------------------------------------------------------
///  JIKI_MOVEORDER
//--------------------------------------------------------------
typedef enum
{
  //��{�ړ�
  JIKI_MOVEORDER_NON = 0, ///<�Z�b�g����
  JIKI_MOVEORDER_STOP,    ///<��~�Z�b�g
  JIKI_MOVEORDER_WALK,    ///<�����Z�b�g
  JIKI_MOVEORDER_TURN,    ///<�U������Z�b�g
  JIKI_MOVEORDER_HITCH,   ///<��Q���q�b�g�Z�b�g
  JIKI_MOVEORDER_JUMP,    ///<�W�����v�Z�b�g
  
  //�ʏ�ړ��@����
  JIKI_MOVEORDER_OZE_YURE, ///<�������
  JIKI_MOVEORDER_OZE_FALLOUT_JUMP_START, ///<���������Z�b�g
  JIKI_MOVEORDER_OZE_FALLOUT_JUMP_WAIT, ///<���������J�n
  JIKI_MOVEORDER_OZE_FALLOUT, ///<����������
}JIKI_MOVEORDER;


//--------------------------------------------------------------
///  JIKI_ACTION
//--------------------------------------------------------------
typedef enum
{
  //��{�ړ�
  JIKI_ACTION_STOP = 0,
  JIKI_ACTION_WALK,
  JIKI_ACTION_TURN,
  JIKI_ACTION_HITCH,
  
  //�ʏ�ړ��@����
  JIKI_ACTION_OZE_YURE,
  JIKI_ACTION_OZE_FALLOUT_JUMP,
  JIKI_ACTION_OZE_FALLOUT_JUMP_WAIT,
  JIKI_ACTION_OZE_FALLOUT,
}JIKI_ACTION;

//--------------------------------------------------------------
/// JIKI_MOVEHITBIT 
//--------------------------------------------------------------
typedef enum
{
  JIKI_MOVEHITBIT_EXIT  =  EX_MOVEHITBIT(0), ///<�o������q�b�g
  JIKI_MOVEHITBIT_WATER =  EX_MOVEHITBIT(1), ///<���A�g���r���[�g�q�b�g
  JIKI_MOVEHITBIT_JUMP  =  EX_MOVEHITBIT(2), ///<�W�����v�A�g���r���[�g�q�b�g
  JIKI_MOVEHITBIT_OZE   =  EX_MOVEHITBIT(3), ///<�����A�g���r���[�g�q�b�g
  
  JIKI_MOVEHITBIT_CHECK_OVER = (MMDL_MOVEHITBIT_BITMAX+4),
  
#if 0
  JIKI_MOVE_HIT_BIT_TAKE_OFF  (1<<4)          ///<�W�����v��q�b�g
  JIKI_MOVE_HIT_BIT_BRIDGEHV  (1<<6)          ///<��{���q�b�g
  JIKI_MOVE_HIT_BIT_JUMP_3G  (1<<7)          ///<3G�W�����v
#endif
}JIKI_MOVEHITBIT;

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
  JIKI_ACTION move_action;
  JIKI_MOVEORDER move_order;
  
  JIKI_MOVEBIT move_bit;
  FIELD_PLAYER_REQBIT req_bit;
  
  FIELD_PLAYER *fld_player;
  FIELDMAP_WORK *fieldWork;
  FLDEFF_TASK *fldeff_joint;
  
  u16 input_key_dir_x; //�L�[���͉�����
  u16 input_key_dir_z; //�L�[���͏c����
  
  u16 oze_yure_frame;
  u16 oze_anime_reset_flag;
};

//======================================================================
//  proto
//======================================================================
static void gjiki_InitMoveStartCommon(
    FIELD_PLAYER_GRID *gjiki, u16 key_prs, JIKI_MOVEORDER set );
static BOOL gjiki_CheckMoveStart( FIELD_PLAYER_GRID *gjiki, u16 dir );
static void gjiki_PlaySE( FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, u16 dir );

//�L�[���͏���
static u16 gjiki_GetInputKeyDir(
    FIELD_PLAYER_GRID *gjiki, u16 key_prs );
static void gjiki_SetInputKeyDir( FIELD_PLAYER_GRID *gjiki, u16 key_prs );
static u16 getKeyDirX( u16 key_prs );
static u16 getKeyDirZ( u16 key_prs );

//�ړ��J�n���
static JIKI_MOVEORDER gjiki_GetMoveOrder( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag );

static JIKI_MOVEORDER gjiki_GetMoveOrder_Normal(
    FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static JIKI_MOVEORDER gjiki_GetMoveOrder_Oze( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_Yure(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_WaitFallOutJump(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_FallOut(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static JIKI_MOVEORDER gjiki_GetMoveOrder_Cycle( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static JIKI_MOVEORDER gjiki_GetMoveOrder_Swim( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

//�ړ��Z�b�g
static void gjiki_SetMove( FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void gjiki_SetMove_Normal( FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set,
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
static void gjikiOze_SetMove_Yure(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiOze_SetMove_FallOutJumpStart(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiOze_SetMove_FallOutJumpWait(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static void gjikiOze_SetMove_FallOut(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );

static void gjiki_SetMove_Cycle( FIELD_PLAYER_GRID *gjiki,
    JIKI_MOVEORDER set,
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

static void gjiki_SetMove_Swim( FIELD_PLAYER_GRID *gjiki,
    JIKI_MOVEORDER set,
    u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Hitch(
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
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEBIT bit );
static void gjiki_OffMoveBit(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEBIT bit );
static BOOL gjiki_CheckMoveBit(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEBIT bit );
static void gjiki_OnMoveBitForce( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OffMoveBitForce( FIELD_PLAYER_GRID *gjiki );
static BOOL gjiki_CheckMoveBitForce( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OnMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OffMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OnMoveBitStep( FIELD_PLAYER_GRID *gjiki );
static void gjiki_OffMoveBitStep( FIELD_PLAYER_GRID *gjiki );
static void gjiki_SetMoveBitOzeYure( FIELD_PLAYER_GRID *gjiki, BOOL on );
static BOOL gjiki_CheckMoveBitOzeYure( FIELD_PLAYER_GRID *gjiki );
static void gjiki_SetMoveBitOzeFallOut( FIELD_PLAYER_GRID *gjiki, BOOL on );
static BOOL gjiki_CheckMoveBitOzeFallOut( FIELD_PLAYER_GRID *gjiki );

static UNDER gjiki_CheckUnder( FIELD_PLAYER_GRID *gjiki, u16 dir );

static BOOL gjiki_ControlUnder(
    FIELD_PLAYER_GRID *gjiki, u16 dir, BOOL debug );

//����
//static void oze_InitParam( FIELD_PLAYER_GRID *gjiki );
static BOOL oze_IsOzeAttr( const MAPATTR_VALUE val );
static BOOL oze_CheckAttrOzeMove( FIELD_PLAYER_GRID *gjiki, u16 dir );
static BOOL oze_CheckAttrUnderOze( FIELD_PLAYER_GRID *gjiki );
static u16 oze_CheckAttrOzeFallOut( FIELD_PLAYER_GRID *gjiki );
static u32 oze_HitCheckMove(
    FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, u16 dir, MAPATTR *attr );
static BOOL oze_CheckFallOutEnd( FIELD_PLAYER_GRID *gjiki );
static void oze_StartYureJiki( FIELD_PLAYER_GRID *gjiki );
static void oze_EndYureJiki( FIELD_PLAYER_GRID *gjiki );

//parts
static BOOL gjiki_GetAttr( FIELD_PLAYER_GRID *gjiki, u16 dir, MAPATTR *attr );

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
  
  { //��p�̃q�b�g�`�F�b�N�p�r�b�g��32bit�ȓ���
    GF_ASSERT( JIKI_MOVEHITBIT_CHECK_OVER < 32 );
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
  JIKI_MOVEORDER order;
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
  
  order = gjiki_GetMoveOrder( gjiki, key_trg, key_cont, dir, debug_flag );
  
  gjiki_InitMoveStartCommon( gjiki, key_cont, order );
  gjiki_SetMove( gjiki, order, key_trg, key_cont, dir, debug_flag );
  gjiki_PlaySE( gjiki, order, dir );
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
    FIELD_PLAYER_GRID *gjiki, u16 key_prs, JIKI_MOVEORDER set )
{
  gjiki->move_order = set;
  gjiki_SetInputKeyDir( gjiki, key_prs );
  gjiki_OffMoveBitStep( gjiki );
  
  if( set == JIKI_MOVEORDER_WALK ){
    gjiki_OffMoveBitUnderOff( gjiki );
  }
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N
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
    if( gjiki->move_action == JIKI_ACTION_HITCH ){
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
static void gjiki_PlaySE( FIELD_PLAYER_GRID *gjiki,
    JIKI_MOVEORDER set, u16 dir )
{
  if( set == JIKI_MOVEORDER_WALK ){
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
static u16 gjiki_GetInputKeyDir( FIELD_PLAYER_GRID *gjiki, u16 key_prs )
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
// �L�[���͂ɂ�鎩�@�ړ��I�[�_�[���擾
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��I�[�_�[���擾
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �L�[���͕���
 * @param debug_flag �f�o�b�O�ړ��t���O TRUE=�f�o�b�O�ړ�
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjiki_GetMoveOrder( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag )
{
  JIKI_MOVEORDER set;
  PLAYER_MOVE_FORM form;
  
  set = JIKI_MOVEORDER_NON;
  form = FIELD_PLAYER_GetMoveForm( gjiki->fld_player );
  
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    set = gjiki_GetMoveOrder_Normal(
        gjiki, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_FORM_CYCLE:
    set = gjiki_GetMoveOrder_Cycle(
        gjiki, key_trg, key_cont, dir, debug_flag );
    break;
  case PLAYER_MOVE_FORM_SWIM:
    set = gjiki_GetMoveOrder_Swim(
        gjiki, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  return( set );
}

//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��I�[�_�[���擾
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �L�[���͕���
 * @retval JIKI_MOVEORDER
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
  
  if( dir != m_dir && gjiki->move_action != JIKI_ACTION_WALK ){
    return( PLAYER_MOVE_VALUE_TURN );
  }
  
  return( PLAYER_MOVE_VALUE_WALK );
}

//======================================================================
// �L�[���͂ɂ�鎩�@�ړ��I�[�_�[���擾�@�ʏ�ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��I�[�_�[���擾 �ʏ�ړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �L�[���͕���
 * @param debug_flag �f�o�b�O�ړ��t���O TRUE=�f�o�b�O�ړ�
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjiki_GetMoveOrder_Normal( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag )
{
  JIKI_MOVEORDER set;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  
  set = JIKI_MOVEORDER_NON;
  
  if( oze_CheckAttrUnderOze(gjiki) == TRUE ||
      gjiki_CheckMoveBitOzeFallOut(gjiki) == TRUE ){
    set = gjiki_GetMoveOrder_Oze(
        gjiki, key_trg, key_cont, dir, debug_flag );
    return( set );
  }
  
  switch( gjiki->move_action ){
  case JIKI_ACTION_STOP:
    set = gjiki_CheckMoveOrder_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_WALK:
    set = gjiki_CheckMoveOrder_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_TURN:
    set = gjiki_CheckMoveOrder_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_HITCH:
    set = gjiki_CheckMoveOrder_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  return( set );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@��~��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( dir != old_dir && debug_flag == FALSE ){
        return( JIKI_MOVEORDER_TURN );
      }
      
      return( gjiki_CheckMoveOrder_Walk(
        gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
    }
    
    return( JIKI_MOVEORDER_STOP );
  }
  
  return( JIKI_MOVEORDER_NON );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@�ړ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( gjiki_CheckMoveOrder_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  {
    MAPATTR attr;
    u32 hit = gjiki_HitCheckMove( gjiki, mmdl, dir, &attr );
    
    if( debug_flag == TRUE )
    {
      if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit & MMDL_MOVEHITBIT_OUTRANGE) )
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
          return( JIKI_MOVEORDER_JUMP );
        }
      }
      
      if( hit == MMDL_MOVEHITBIT_NON )
      {
        if( (flag & MAPATTR_FLAGBIT_WATER) == 0 ){ //���n�`
          return( JIKI_MOVEORDER_WALK );
        }
        
        if( debug_flag == TRUE ){ //�f�o�b�O�ړ�
          return( JIKI_MOVEORDER_WALK );
        }
      }
    }
  }
  
  return( JIKI_MOVEORDER_HITCH );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@�U�������
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjiki_CheckMoveOrder_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjiki_CheckMoveOrder_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@��Q���q�b�g��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( gjiki_CheckMoveOrder_Walk(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjiki_CheckMoveOrder_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjiki_CheckMoveOrder_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
// �L�[���͂ɂ�鎩�@�ړ��I�[�_�[���擾�@����
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��I�[�_�[���擾 ����
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �L�[���͕���
 * @param debug_flag �f�o�b�O�ړ��t���O TRUE=�f�o�b�O�ړ�
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjiki_GetMoveOrder_Oze( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag )
{
  JIKI_MOVEORDER set;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  
  set = JIKI_MOVEORDER_NON;
  
  switch( gjiki->move_action ){
  case JIKI_ACTION_STOP:
    set = gjikiOze_CheckMoveOrder_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_WALK:
    set = gjikiOze_CheckMoveOrder_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_TURN:
    set = gjiki_CheckMoveOrder_Turn( //���p
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_HITCH:
    set = gjiki_CheckMoveOrder_Hitch( //���p
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_OZE_YURE:
    set = gjikiOze_CheckMoveOrder_Yure(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_OZE_FALLOUT_JUMP:
  case JIKI_ACTION_OZE_FALLOUT_JUMP_WAIT:
    set = gjikiOze_CheckMoveOrder_WaitFallOutJump(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_OZE_FALLOUT:
    set = gjikiOze_CheckMoveOrder_FallOut(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  return( set );
}

//--------------------------------------------------------------
/**
 * �����@�I�[�_�[�`�F�b�N�@��~��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( dir != old_dir && debug_flag == FALSE ){
        return( JIKI_MOVEORDER_TURN );
      }
      
      return( gjikiOze_CheckMoveOrder_Walk(
        gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
    }
    
    return( gjikiOze_CheckMoveOrder_Yure(
          gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( JIKI_MOVEORDER_NON );
}

//--------------------------------------------------------------
/**
 * �����@�I�[�_�[�`�F�b�N�@�ړ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( gjikiOze_CheckMoveOrder_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  {
    MAPATTR attr;
    u32 hit = oze_HitCheckMove( gjiki, mmdl, dir, &attr );
    
    if( attr != MAPATTR_ERROR )
    {
      if( (hit & JIKI_MOVEHITBIT_OZE) ) //�ړ�������A�g���r���[�g
      {
        hit &= ~JIKI_MOVEHITBIT_OZE;
        
        if( hit == MMDL_MOVEHITBIT_NON ) //��Q���Ȃ�
        {
          return( JIKI_MOVEORDER_WALK );
        }
      }
      else //�����ł͂Ȃ�
      {
        if( MAPATTR_GetHitchFlag(attr) == FALSE ) //�ړ��\
        {
          gjiki_SetMoveBitOzeFallOut( gjiki, TRUE ); //�����t���OON
          return( JIKI_MOVEORDER_OZE_FALLOUT_JUMP_START ); //����
        }
      }
    }
  }
  
  return( JIKI_MOVEORDER_HITCH );
}

//--------------------------------------------------------------
/**
 * �����@�I�[�_�[�`�F�b�N�@�h�ꒆ
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_Yure(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( dir == DIR_NOT ) //���������Ă��Ȃ�
  {
    if( gjiki_CheckMoveBitOzeYure(gjiki) == FALSE ){
      oze_StartYureJiki( gjiki );
    }
    
    if( gjiki->oze_anime_reset_flag == TRUE ){
      if( MMDL_DrawYureHero_SetAnimeFrame(mmdl,gjiki->oze_yure_frame) ){
        gjiki->oze_anime_reset_flag = FALSE;
      }
    }

    gjiki->oze_yure_frame++;
    
    if( gjiki->oze_yure_frame == OZE_YURE_MAXFRAME )
    { //�����J�n
      oze_EndYureJiki( gjiki );
      gjiki_SetMoveBitOzeFallOut( gjiki, TRUE );
      return( JIKI_MOVEORDER_OZE_FALLOUT_JUMP_START );
    }
  }
  else //����������
  { 
    oze_EndYureJiki( gjiki );
    return( gjikiOze_CheckMoveOrder_Walk(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( JIKI_MOVEORDER_OZE_YURE );
}

//--------------------------------------------------------------
/**
 * �����@�I�[�_�[�`�F�b�N�@�����J�n�@�W�����v�I��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 * @attention �����ɗ���Ƃ������͗����ړ����Ƃ������ł���
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_WaitFallOutJump(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ) //���������J�n�A�j���I���҂�
  {
    return( JIKI_MOVEORDER_OZE_FALLOUT );
  }
  
  return( JIKI_MOVEORDER_OZE_FALLOUT_JUMP_WAIT );
}

//--------------------------------------------------------------
/**
 * �����@�I�[�_�[�`�F�b�N�@������
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 * @attention �����ɗ���Ƃ������͗����ړ����Ƃ������ł���
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_FallOut(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( oze_CheckFallOutEnd(gjiki) == TRUE ){
    VecFx32 pos;
    //�X�e�[�^�X�����ɖ߂�
    //OBJ�R�[�h�����ɖ߂��K�v�A��
    MMDL_SetStatusBitHeightGetOFF( mmdl, FALSE ); //�����擾�J�n
	  MMDL_OnStatusBit( mmdl, //�W�����v����I�����Z�b�g
        MMDL_STABIT_MOVE_END|MMDL_STABIT_JUMP_END );
    MMDL_UpdateCurrentHeight( mmdl );
    MMDL_GetVectorPos( mmdl, &pos );
    MMDL_SetGridPosY( mmdl, SIZE_GRID_FX32(pos.y) );
    MMDL_UpdateCurrentMapAttr( mmdl );
    MMDL_UpdateGridPosCurrent( mmdl );
    gjiki_SetMoveBitOzeFallOut( gjiki, FALSE );
    return( JIKI_MOVEORDER_STOP );
  }
  
  return( JIKI_MOVEORDER_OZE_FALLOUT );
}

//======================================================================
// �L�[���͂ɂ�鎩�@��ԕω����擾�@���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��I�[�_�[���擾�@���]�Ԉړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �L�[���͕���
 * @param debug_flag �f�o�b�O�ړ��t���O TRUE=�f�o�b�O�ړ�
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjiki_GetMoveOrder_Cycle( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag )
{
  JIKI_MOVEORDER set;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
   
  set = JIKI_MOVEORDER_NON;
  switch( gjiki->move_action ){
  case JIKI_ACTION_STOP:
    set = gjikiCycle_CheckMoveOrder_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_WALK:
    set = gjikiCycle_CheckMoveOrder_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_TURN:
    set = gjikiCycle_CheckMoveOrder_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_HITCH:
    set = gjikiCycle_CheckMoveOrder_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  return( set );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@��~��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( dir != old_dir && debug_flag == FALSE ){
        return( JIKI_MOVEORDER_TURN );
      }
      
      return( gjikiCycle_CheckMoveOrder_Walk(
        gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
    }
    return( JIKI_MOVEORDER_STOP );
  }
  
  return( JIKI_MOVEORDER_NON );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@�ړ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( gjikiCycle_CheckMoveOrder_Stop(
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
      return( JIKI_MOVEORDER_WALK );
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
          return( JIKI_MOVEORDER_JUMP );
        }
      }
    }
  }
  
  return( JIKI_MOVEORDER_HITCH );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@�U�������
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjikiCycle_CheckMoveOrder_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjikiCycle_CheckMoveOrder_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@��Q���q�b�g��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( gjikiCycle_CheckMoveOrder_Walk(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjikiCycle_CheckMoveOrder_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjikiCycle_CheckMoveOrder_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
// �L�[���͂ɂ�鎩�@��ԕω����擾�@�g���ړ�
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[���͂ɂ�鎩�@�ړ��I�[�_�[���擾�@���]�Ԉړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �L�[���͕���
 * @param debug_flag �f�o�b�O�ړ��t���O TRUE=�f�o�b�O�ړ�
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjiki_GetMoveOrder_Swim( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, u16 dir, BOOL debug_flag )
{
  JIKI_MOVEORDER set;
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
   
  set = JIKI_MOVEORDER_NON;
  switch( gjiki->move_action ){
  case JIKI_ACTION_STOP:
    set = gjikiSwim_CheckMoveOrder_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_WALK:
    set = gjikiSwim_CheckMoveOrder_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_TURN:
    set = gjikiSwim_CheckMoveOrder_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_ACTION_HITCH:
    set = gjikiSwim_CheckMoveOrder_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  return( set );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@��~��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( dir != old_dir && debug_flag == FALSE ){
        return( JIKI_MOVEORDER_TURN );
      }
      
      return( gjikiSwim_CheckMoveOrder_Walk(
        gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
    }
    
    return( JIKI_MOVEORDER_STOP );
  }
  
  return( JIKI_MOVEORDER_NON );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@�ړ���
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT )
  {
    return( gjikiSwim_CheckMoveOrder_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  {
    MAPATTR attr;
    u32 hit = gjiki_HitCheckMove( gjiki, mmdl, dir, &attr );
    
    if( debug_flag == TRUE )
    {
      if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit & MMDL_MOVEHITBIT_OUTRANGE) )
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
          return( JIKI_MOVEORDER_WALK );
        }
        
        if( debug_flag == TRUE ){
          return( JIKI_MOVEORDER_WALK );
        }
      }
    }
  }
  
  return( JIKI_MOVEORDER_HITCH );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@�U�������
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjikiSwim_CheckMoveOrder_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjikiSwim_CheckMoveOrder_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//--------------------------------------------------------------
/**
 * �I�[�_�[�`�F�b�N�@��Q���q�b�g��
 * @param  gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param dir �ړ�����
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval JIKI_MOVEORDER
 */
//--------------------------------------------------------------
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( dir != DIR_NOT && dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( gjikiSwim_CheckMoveOrder_Walk(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( dir == DIR_NOT ){
    return( gjikiSwim_CheckMoveOrder_Stop(
      gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
  }
  
  return( gjikiSwim_CheckMoveOrder_Walk(
    gjiki,mmdl,key_trg,key_cont,dir,debug_flag) );
}

//======================================================================
//  ���@�ړ��Z�b�g
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ړ��Z�b�g
 * @param gjiki FIELD_PLAYER_GRID
 * @param set JIKI_MOVEORDER
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove( FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set,
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
 * @param set JIKI_MOVEORDER
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Normal( FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  
  switch( set ){
  case JIKI_MOVEORDER_NON:
    gjiki_SetMove_Non(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_STOP:
    gjiki_SetMove_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_WALK:
    gjiki_SetMove_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_TURN:
    gjiki_SetMove_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_HITCH:
    gjiki_SetMove_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_JUMP:
    gjiki_SetMove_Jump(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_OZE_YURE:
    gjikiOze_SetMove_Yure(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_OZE_FALLOUT_JUMP_START:
    gjikiOze_SetMove_FallOutJumpStart(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_OZE_FALLOUT_JUMP_WAIT:
    gjikiOze_SetMove_FallOutJumpWait(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_OZE_FALLOUT:
    gjikiOze_SetMove_FallOut(
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
  gjiki->move_action = JIKI_ACTION_STOP;
  
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
  gjiki->move_action = JIKI_ACTION_WALK;

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
  gjiki->move_action = JIKI_ACTION_TURN;
  
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
  gjiki->move_action = JIKI_ACTION_HITCH;
  
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
  gjiki->move_action = JIKI_ACTION_WALK;
  
  FIELD_PLAYER_SetMoveValue( gjiki->fld_player, PLAYER_MOVE_VALUE_WALK );
  PMSND_PlaySE( SEQ_SE_DANSA );
  
#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackAction();
#endif
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g�@�����@���
 * @param gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiOze_SetMove_Yure(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  //���ɂȂɂ����Ȃ��H
  gjiki->move_action = JIKI_ACTION_OZE_YURE;
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g�@�����@�����W�����v�Z�b�g
 * @param gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiOze_SetMove_FallOutJumpStart(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  u16 code;

  if( dir == DIR_NOT ){ //�h����E�ɂ�藎����
    dir = oze_CheckAttrOzeFallOut( gjiki );
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_1G_8F );
  MMDL_SetAcmd( mmdl, code );
  MMDL_SetStatusBitHeightGetOFF( mmdl, TRUE ); //�����擾���֎~��
  MMDL_OnStatusBit( mmdl, MMDL_STABIT_SHADOW_VANISH );

  gjiki->move_action = JIKI_ACTION_OZE_FALLOUT_JUMP;
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g�@�����@�����W�����v�I���҂�
 * @param gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiOze_SetMove_FallOutJumpWait(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  gjiki->move_action = JIKI_ACTION_OZE_FALLOUT_JUMP_WAIT;
}

//--------------------------------------------------------------
/**
 * �ړ��Z�b�g�@�����@����
 * @param gjiki FIELD_PLAYER_GRID
 * @param mmdl MMDL*
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjikiOze_SetMove_FallOut(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  fx32 height;
  VecFx32 pos;
  
  mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  MMDL_GetVectorPos( mmdl, &pos );
  MMDL_GetMapPosHeight( mmdl, &pos, &height );
  
  if( pos.y > height ){
    pos.y += NUM_FX32( -8 );
  
    if( pos.y < height ){
      pos.y = height;
      MMDL_OffStatusBit( mmdl, MMDL_STABIT_SHADOW_VANISH );
      MMDL_SetStatusBitHeightGetOFF( mmdl, FALSE ); //�����擾�֎~����
    }
  
    MMDL_SetVectorPos( mmdl, &pos );
    gjiki->move_action = JIKI_ACTION_OZE_FALLOUT;
  }
}

//======================================================================
//  ���@�ړ��Z�b�g  ���]�Ԉړ�
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ړ��Z�b�g�@���]�Ԉړ�
 * @param gjiki FIELD_PLAYER_GRID
 * @param set JIKI_MOVEORDER
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Cycle( FIELD_PLAYER_GRID *gjiki,
    JIKI_MOVEORDER set,
    u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );

  switch( set ){
  case JIKI_MOVEORDER_NON:
    gjikiCycle_SetMove_Non(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_STOP:
    gjikiCycle_SetMove_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_WALK:
    gjikiCycle_SetMove_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_TURN:
    gjikiCycle_SetMove_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_HITCH:
    gjikiCycle_SetMove_Hitch(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_JUMP:
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
  gjiki->move_action = JIKI_ACTION_STOP;
  
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
    MAPATTR attr;
    code = AC_WALK_U_2F;
    
    if( gjiki_GetAttr(gjiki,DIR_NOT,&attr) == TRUE ){ //�[�������Ȃ瑬�x�ቺ
      MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
      
      if( MAPATTR_VALUE_CheckDesertDeep(val) == TRUE ){
        FLDEFF_CTRL *fectrl;
        code = AC_WALK_U_16F; //��p
        
        fectrl = FIELDMAP_GetFldEffCtrl( gjiki->fieldWork );
        FLDEFF_KEMURI_SetMMdl( mmdl, fectrl ); //���o��
      }
    }
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, code );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_WALK;
  
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
  gjiki->move_action = JIKI_ACTION_TURN;
  
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
  gjiki->move_action = JIKI_ACTION_HITCH;
  
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
  gjiki->move_action = JIKI_ACTION_WALK;
  
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
 * @param set JIKI_MOVEORDER
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @param debug_flag �f�o�b�O�ړ��\�t���O TRUE=�\
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMove_Swim(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set,
  u32 key_trg, u32 key_cont, u16 dir, BOOL debug_flag )
{
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );

  switch( set ){
  case JIKI_MOVEORDER_NON:
    gjikiSwim_SetMove_Non(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_STOP:
    gjikiSwim_SetMove_Stop(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_WALK:
    gjikiSwim_SetMove_Walk(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_TURN:
    gjikiSwim_SetMove_Turn(
      gjiki, mmdl, key_trg, key_cont, dir, debug_flag );
    break;
  case JIKI_MOVEORDER_HITCH:
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
  gjiki->move_action = JIKI_ACTION_STOP;
  
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
  gjiki->move_action = JIKI_ACTION_WALK;

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
  gjiki->move_action = JIKI_ACTION_TURN;
  
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
  gjiki->move_action = JIKI_ACTION_HITCH;
  
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
  JIKI_MOVEHITBIT ret = MMDL_MOVEHITBIT_NON;
  
  gx += MMDL_TOOL_GetDirAddValueGridX( dir );
  gz += MMDL_TOOL_GetDirAddValueGridZ( dir );
  
  hit = MMDL_HitCheckMoveCurrent( mmdl, gx, gy, gz, dir );
  
  MMDL_GetVectorPos( mmdl, &pos );
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  
  if( MMDL_GetMapPosAttr(mmdl,&pos,attr) == FALSE )
  {
    *attr = MAPATTR_ERROR;
  }
  
  if( (hit&MMDL_MOVEHITBIT_ATTR) ) //�ꕔ�����Ƃ��锻����`�F�b�N
  {
    MAPATTR_VALUE attr_val;
    attr_val = MAPATTR_GetAttrValue( *attr );
    
    if( MAPATTR_VALUE_CheckKairikiAna(attr_val) == TRUE )
    {
      const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
      
      KAGAYA_Printf( "���`�F�b�N���܂���\n" );
      
      if( MMDLSYS_ROCKPOS_CheckRockFalled(mmdlsys,&pos) == TRUE )
      {
        hit &= ~MMDL_MOVEHITBIT_ATTR;
      }
    }
  }
  
  return( hit );
}

//--------------------------------------------------------------
/**
 * ���@�I�[�_�[�`�F�b�N
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
 * JIKI_MOVEBIT ON
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit JIKI_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OnMoveBit(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEBIT bit )
{
  GF_ASSERT( bit < JIKI_MOVEBIT_BITMAX );
  gjiki->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT OFF
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit JIKI_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OffMoveBit(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEBIT bit )
{
  GF_ASSERT( bit < JIKI_MOVEBIT_BITMAX );
  gjiki->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT Check
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit JIKI_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static BOOL gjiki_CheckMoveBit(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEBIT bit )
{
  GF_ASSERT( bit < JIKI_MOVEBIT_BITMAX );
  if( (gjiki->move_bit & bit) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT �����ړ��r�b�gON
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit JIKI_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OnMoveBitForce( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OnMoveBit( gjiki, JIKI_MOVEBIT_FORCE );
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT �����ړ��r�b�gOFF
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit JIKI_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OffMoveBitForce( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OffMoveBit( gjiki, JIKI_MOVEBIT_FORCE );
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT �����ړ��r�b�g�`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @retval BOOL 
 */
//--------------------------------------------------------------
static BOOL gjiki_CheckMoveBitForce( FIELD_PLAYER_GRID *gjiki )
{
  if( gjiki_CheckMoveBit(gjiki,JIKI_MOVEBIT_FORCE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT ��������ON
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit JIKI_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OnMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OnMoveBit( gjiki, JIKI_MOVEBIT_UNDER_OFF );
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT ��������OFF
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit JIKI_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OffMoveBitUnderOff( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OffMoveBit( gjiki, JIKI_MOVEBIT_UNDER_OFF );
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT ����ړ� ON
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit JIKI_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OnMoveBitStep( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OnMoveBit( gjiki, JIKI_MOVEBIT_STEP );
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT ����ړ� OFF
 * @param gjiki FIELD_PLAYER_GRID
 * @param bit JIKI_MOVEBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_OffMoveBitStep( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_OffMoveBit( gjiki, JIKI_MOVEBIT_STEP );
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT ��������� �Z�b�g
 * @param gjiki FIELD_PLAYER_GRID
 * @param on TRUE=������� FALSE=�����t���O�N���A
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMoveBitOzeYure(
    FIELD_PLAYER_GRID *gjiki, BOOL on )
{
  if( on == TRUE ){
    gjiki_OnMoveBit( gjiki, JIKI_MOVEBIT_OZE_YURE );
  }else{
    gjiki_OffMoveBit( gjiki, JIKI_MOVEBIT_OZE_YURE );
  }
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT ��������� �`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param on TRUE=������� FALSE=�����t���O�N���A
 * @retval nothing
 */
//--------------------------------------------------------------
static BOOL gjiki_CheckMoveBitOzeYure( FIELD_PLAYER_GRID *gjiki )
{
  if( gjiki_CheckMoveBit(gjiki,JIKI_MOVEBIT_OZE_YURE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT �������� �Z�b�g
 * @param gjiki FIELD_PLAYER_GRID
 * @param on TRUE=������� FALSE=�����t���O�N���A
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_SetMoveBitOzeFallOut( FIELD_PLAYER_GRID *gjiki, BOOL on )
{
  if( on == TRUE ){
    gjiki_OnMoveBit( gjiki, JIKI_MOVEBIT_OZE_FALLOUT );
  }else{
    gjiki_OffMoveBit( gjiki, JIKI_MOVEBIT_OZE_FALLOUT );
  }
}

//--------------------------------------------------------------
/**
 * JIKI_MOVEBIT ������� �Z�b�g
 * @param gjiki FIELD_PLAYER_GRID
 * @param on TRUE=������� FALSE=�����t���O�N���A
 * @retval nothing
 */
//--------------------------------------------------------------
static BOOL gjiki_CheckMoveBitOzeFallOut( FIELD_PLAYER_GRID *gjiki )
{
  if( gjiki_CheckMoveBit(gjiki,JIKI_MOVEBIT_OZE_FALLOUT) ){
    return( TRUE );
  }
  return( FALSE );
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

  if( gjiki->move_action == JIKI_ACTION_HITCH ){
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
  switch( gjiki->move_action ){
  case JIKI_ACTION_STOP:
    break;
  case JIKI_ACTION_WALK:
    return( FALSE );
  case JIKI_ACTION_TURN:
  case JIKI_ACTION_HITCH:
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
  
  if( gjiki_CheckMoveBit(gjiki,JIKI_MOVEBIT_UNDER_OFF) == FALSE ){
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

//--------------------------------------------------------------
/**
 * ���@���������������ǂ���
 * @param
 * @retval
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_CheckOzeFallOut( FIELD_PLAYER *fld_player )
{
  FIELDMAP_WORK *fieldmap = FIELD_PLAYER_GetFieldMapWork( fld_player );
  FIELD_PLAYER_GRID *gjiki = FIELDMAP_GetPlayerGrid( fieldmap );
  
  if( gjiki_CheckMoveBitOzeFallOut(gjiki) == TRUE ){
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
    
    gjiki->move_action = JIKI_ACTION_WALK;
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
//  ����
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * ������Ԃɂ���
 * @param gjiki FIELD_PLAYER_GRID
 * @param init_frame TRUE=���t���[��������
 * @retval nothing
 */
//--------------------------------------------------------------
static void oze_InitParam( FIELD_PLAYER_GRID *gjiki )
{
}
#endif

//--------------------------------------------------------------
/**
 * �����n�A�g���r���[�g��
 * @param val MAPATTR_VALUE
 * @retval BOOL TRUE=�����n
 */
//--------------------------------------------------------------
static BOOL oze_IsOzeAttr( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckOze01(val) == TRUE ||
      MAPATTR_VALUE_CheckOzeStairs(val) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����A�g���r���[�g���@�����K�i�͍l�����Ȃ�
 * @param val MAPATTR_VALUE
 * @retval BOOL TRUE=����
 */
//--------------------------------------------------------------
static BOOL oze_IsOnlyOzeAttr( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckOze01(val) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����@���@�̈ړ��悪�ړ��\���ǂ���
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �ړ�����
 * @retval BOOL TRUE=�ړ��\
 */
//--------------------------------------------------------------
static BOOL oze_CheckAttrOzeMove( FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  BOOL ret;
  MAPATTR attr;
  
  GF_ASSERT( dir < DIR_MAX4 );
  ret = gjiki_GetAttr( gjiki, dir, &attr );
  
  if( ret == TRUE ){
    MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
    
    if( oze_IsOzeAttr(val) == TRUE ){
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����@���@�̑��ꂪ�������ǂ���
 * @param gjiki FIELD_PLAYER_GRID
 * @retval BOOL TRUE=����
 */
//--------------------------------------------------------------
static BOOL oze_CheckAttrUnderOze( FIELD_PLAYER_GRID *gjiki )
{
  BOOL ret;
  MAPATTR attr;
  MAPATTR_VALUE val;

  gjiki_GetAttr( gjiki, DIR_NOT, &attr );
  val = MAPATTR_GetAttrValue( attr );
  
  if( oze_IsOnlyOzeAttr(val) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����@�����A�g���r���[�g�ȊO�ŗ����\�ȕ�����T���B
 * @param gjiki FIELD_PLAYER_GRID
 * @retval u16 �����\�ȕ��� DIR_NOT=�Ȃ�
 */
//--------------------------------------------------------------
static u16 oze_CheckAttrOzeFallOut( FIELD_PLAYER_GRID *gjiki )
{
  u16 i,dir;
  BOOL ret;
  MAPATTR attr;
  MAPATTR_VALUE val;
  MAPATTR_FLAGBIT flag;
  //���v���Ŋe�������`�F�b�N
  u16 next_dir[DIR_MAX4] = { DIR_RIGHT, DIR_LEFT, DIR_UP, DIR_DOWN };
  
  dir = FIELD_PLAYER_GetDir( gjiki->fld_player );
  
  for( i = 0; i < DIR_MAX4; i++, dir = next_dir[dir] ){
    ret = gjiki_GetAttr( gjiki, dir, &attr );
    
    if( ret == TRUE ){
      val = MAPATTR_GetAttrValue( attr );
      
      if( oze_IsOzeAttr(val) == FALSE ){
        if( MAPATTR_GetHitchFlag(attr) == FALSE ){
          return( dir );
        }
      }
    }
  }
  
  GF_ASSERT( 0 );
  return( DIR_NOT );
}

//--------------------------------------------------------------
/**
 * �����@�ړ��q�b�g�`�F�b�N
 * @param gjiki
 * @retval
 */
//--------------------------------------------------------------
static u32 oze_HitCheckMove(
    FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, u16 dir, MAPATTR *attr )
{
  u32 hit = gjiki_HitCheckMove( gjiki, mmdl, dir, attr );
  MAPATTR_VALUE val = MAPATTR_GetAttrValue( *attr );
  
  if( oze_IsOzeAttr(val) == TRUE ){
    hit |= JIKI_MOVEHITBIT_OZE;
  }
  
  return( hit );
}

//--------------------------------------------------------------
/**
 * �����@�����ɂ�钅�n�`�F�b�N
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL oze_CheckFallOutEnd( FIELD_PLAYER_GRID *gjiki )
{
  fx32 height;
  VecFx32 pos;
  MMDL *mmdl;
  
  mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  MMDL_GetVectorPos( mmdl, &pos );
  MMDL_GetMapPosHeight( mmdl, &pos, &height );
  
  if( pos.y > height ){
    return( FALSE );
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ��������Ԃ�
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void oze_StartYureJiki( FIELD_PLAYER_GRID *gjiki )
{
  gjiki_SetMoveBitOzeYure( gjiki, TRUE );
  FIELD_PLAYER_ChangeDrawForm(
    gjiki->fld_player, PLAYER_DRAW_FORM_YURE );
}

//--------------------------------------------------------------
/**
 * ��������Ԃ��猳�ɖ߂�
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void oze_EndYureJiki( FIELD_PLAYER_GRID *gjiki )
{
  gjiki->oze_yure_frame = 0;
  gjiki->oze_anime_reset_flag = FALSE;
  gjiki_SetMoveBitOzeFallOut( gjiki, FALSE );
      
  gjiki_SetMoveBitOzeYure( gjiki, FALSE );
  FIELD_PLAYER_ResetMoveForm( gjiki->fld_player );
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


//--------------------------------------------------------------
/**
 * ���@������\���̏ꍇ�͌��ɖ߂��B
 * @param fld_player
 * @retval nothing
 * @note ���̏��A�Y���ӏ��͔�������Ԃ̂�
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_CheckSpecialDrawForm(
    FIELD_PLAYER *fld_player, BOOL menu_open_flag )
{
  FIELDMAP_WORK *fieldWork = FIELD_PLAYER_GetFieldMapWork( fld_player );
  FIELD_PLAYER_GRID *gjiki = FIELDMAP_GetPlayerGrid( fieldWork );
  
  if( gjiki_CheckMoveBitOzeYure(gjiki) == TRUE ){
    u32 oze_yure_frame = gjiki->oze_yure_frame;
    oze_EndYureJiki( gjiki );
    //���j���[�I�[�v�����͗h��t���[�����ێ�
    //�I�[�v����A�o�b�O���A��ʑJ�ڂ̏ꍇ�͎��@���[�N�̓N���A�����
    if( menu_open_flag == TRUE ){
      gjiki->oze_yure_frame = oze_yure_frame;
      gjiki->oze_anime_reset_flag = TRUE;
    }
  }
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�̎w�������̃A�g���r���[�g���擾����
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �擾�� DIR_NOT=���̏�`�F�b�N
 * @pparam attr MNAPATTR�i�[��
 * @retval BOOL FALSE=�A�g���r���[�g�����݂��Ȃ�
 */
//--------------------------------------------------------------
static BOOL gjiki_GetAttr(
    FIELD_PLAYER_GRID *gjiki, u16 dir, MAPATTR *attr )
{
  BOOL ret;
  MMDL *mmdl;
  VecFx32 pos;
  
  FIELD_PLAYER_GetPos( gjiki->fld_player, &pos );
  
  if( dir != DIR_NOT ){
    MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  }
  
  mmdl = FIELD_PLAYER_GetMMdl( gjiki->fld_player );
  ret = MMDL_GetMapPosAttr( mmdl, &pos, attr );
  return( ret );
}
