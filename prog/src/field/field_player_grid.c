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
#include "field_player_core.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "fieldmap_ctrl_grid.h"
#include "field_player_grid.h"
#include "map_attr.h"

#include "fldeff_namipoke.h"

#include "field_sound.h"

#include "fldeff_kemuri.h"

#include "include/gamesystem/pm_season.h"
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"

//======================================================================
//  define
//======================================================================
//#define PLAYER_MOVE_TRACK_CHANGE //��`�Ŏ��@���쎞�Ƀg���b�N�ύX

///���@��p�q�b�g�`�F�b�N�r�b�g���o��
#define EX_MOVEHITBIT(bit_no) (1<<(MMDL_MOVEHITBIT_BITMAX+bit_no))

///�����������E�t���[��
#define OZE_YURE_MAXFRAME (30*2)

#define JIKI_AC_TURN_U (AC_STAY_WALK_U_4F)

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
  ///�_�b�V���ړ���
  JIKI_MOVEBIT_DASH        = (1<<5),
  ///�ő�
  JIKI_MOVEBIT_BITMAX      = (1<<6),
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

//--------------------------------------------------------------
/// GROUND_HEIGHT
//--------------------------------------------------------------
typedef enum
{
  GROUND_HEIGHT_FLAT = 0, ///<����
  GROUND_HEIGHT_UP, ///<�o��
  GROUND_HEIGHT_DOWN, ///<����
}GROUND_HEIGHT;

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
  
  FIELD_PLAYER_CORE *player_core;
  FIELDMAP_WORK *fieldWork;
  
  u16 oze_yure_frame;
  u16 oze_anime_reset_flag;
  
  u16 dash_play_se;
  u16 dash_play_se_count;
  
  u32 naminori_end_flag;
};

//--------------------------------------------------------------
/// INPUTDATA�@���̓f�[�^
//--------------------------------------------------------------
typedef struct
{
  int key_trg;
  int key_cont;
  u16 dir;
  PLAYER_MOVEBIT mbit;
  BOOL debug_flag;
}INPUTDATA;

//======================================================================
//  proto
//======================================================================
static void gjiki_InitMoveStartCommon(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, const INPUTDATA *input );
static BOOL gjiki_CheckMoveStart( FIELD_PLAYER_GRID *gjiki, u16 dir );
static void gjiki_PlaySE(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, u16 dir );

//�L�[���͏���
static u16 gjiki_GetInputKeyDir(
    FIELD_PLAYER_GRID *gjiki, u16 key_prs );
static void gjiki_SetInputKeyDir( FIELD_PLAYER_GRID *gjiki, u16 key_prs );
static u16 getKeyDirX( u16 key_prs );
static u16 getKeyDirZ( u16 key_prs );

//�ړ��J�n���
static JIKI_MOVEORDER gjiki_GetMoveOrder(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input );

static JIKI_MOVEORDER gjiki_GetMoveOrder_Normal(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input );
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl,  const INPUTDATA *input );
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjiki_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );

static JIKI_MOVEORDER gjiki_GetMoveOrder_Oze(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_Yure(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_WaitFallOutJump(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiOze_CheckMoveOrder_FallOut(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );

static JIKI_MOVEORDER gjiki_GetMoveOrder_Cycle(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );

static JIKI_MOVEORDER gjiki_GetMoveOrder_Swim(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );

//�ړ��Z�b�g
static void gjiki_SetMove( FIELD_PLAYER_GRID *gjiki,
    JIKI_MOVEORDER set, const INPUTDATA *input );

static void gjiki_SetMove_Normal(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set,const INPUTDATA *input );
static void gjiki_SetMove_Non(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjiki_SetMove_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjiki_SetMove_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjiki_SetMove_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjiki_SetMove_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjiki_SetMove_Jump(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiOze_SetMove_Yure(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiOze_SetMove_FallOutJumpStart(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiOze_SetMove_FallOutJumpWait(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiOze_SetMove_FallOut(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );

static void gjiki_SetMove_Cycle(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, const INPUTDATA *input );
static void gjikiCycle_SetMove_Non(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiCycle_SetMove_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiCycle_SetMove_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiCycle_SetMove_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiCycle_SetMove_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiCycle_SetMove_Jump(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );

static void gjiki_SetMove_Swim(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static JIKI_MOVEORDER gjikiSwim_CheckMoveOrder_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );

static void gjikiSwim_SetMove_Non(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiSwim_SetMove_Stop(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiSwim_SetMove_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiSwim_SetMove_Turn(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );
static void gjikiSwim_SetMove_Hitch(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input );

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
static GROUND_HEIGHT gjiki_CheckNextHeight(
    FIELD_PLAYER_GRID *gjiki, u16 dir );
static BOOL gjiki_CheckDirDoor( FIELD_PLAYER_GRID *gjiki, u16 dir );

//======================================================================
//  �O���b�h�ړ� �t�B�[���h�v���C���[����
//======================================================================
//--------------------------------------------------------------
/**
 * �O���b�h�ړ��@�t�B�[���h�v���C���[����@����
 * @param  player_core FIELD_PLAYER_CORE
 * @param heapID HEAPID
 * @retval FIELD_PLAYER_GRID
 */
//--------------------------------------------------------------
FIELD_PLAYER_GRID * FIELD_PLAYER_GRID_Init(
    FIELD_PLAYER_CORE *player_core, HEAPID heapID )
{
  FIELD_PLAYER_GRID *gjiki;
  
  gjiki = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_PLAYER_GRID) );
  gjiki->player_core = player_core;
  gjiki->fieldWork = FIELD_PLAYER_CORE_GetFieldMapWork( player_core );

  gjiki->dash_play_se = SEQ_SE_DUMMY;

  gjiki_OnMoveBitUnderOff( gjiki );
  
  //���A
  {
    PLAYER_MOVE_FORM form;
    form = FIELD_PLAYER_CORE_GetMoveForm( gjiki->player_core );
    
    switch( form ){
    case PLAYER_MOVE_FORM_SWIM:
      FIELD_PLAYER_CORE_SetRequest(
          gjiki->player_core, FIELD_PLAYER_REQBIT_SWIM );
      FIELD_PLAYER_CORE_UpdateRequest( gjiki->player_core );
      break;
    case PLAYER_MOVE_FORM_DIVING:
      FIELD_PLAYER_CORE_SetRequest(
          gjiki->player_core, FIELD_PLAYER_REQBIT_DIVING );
      FIELD_PLAYER_CORE_UpdateRequest( gjiki->player_core );
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
void FIELD_PLAYER_GRID_Move( FIELD_PLAYER_GRID *gjiki,
    int key_trg, int key_cont, PLAYER_MOVEBIT mbit )
{
  u16 dir;
  INPUTDATA input;
  
  dir = FIELD_PLAYER_CORE_GetKeyDir( gjiki->player_core, key_cont );
  
  if( gjiki_CheckMoveStart(gjiki,dir) == TRUE )
  {
    BOOL debug_flag = FALSE;
    
    #ifdef PM_DEBUG
    if( key_cont & PLAYER_PAD_DEBUG){
      debug_flag = TRUE;
    }
    #endif
    
    FIELD_PLAYER_CORE_UpdateRequest( gjiki->player_core );
    
    if( gjiki_ControlUnder(gjiki,dir,debug_flag) == FALSE )
    {
      JIKI_MOVEORDER order;
      input.key_trg = key_trg;
      input.key_cont = key_cont;
      input.dir = dir;
      input.mbit = mbit;
      input.debug_flag = debug_flag;
      
      order = gjiki_GetMoveOrder( gjiki, &input );
      gjiki_InitMoveStartCommon( gjiki, order, &input );
      gjiki_SetMove( gjiki, order, &input );
      gjiki_PlaySE( gjiki, order, dir );
    }
  }
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɍs������������
 * @param gjiki FIELD_PLAYER_GRID
 * @param key_cont �L�[���͏��
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_InitMoveStartCommon(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, const INPUTDATA *input )
{
  gjiki->move_order = set;
  FIELD_PLAYER_CORE_SetMoveStartKeyDir(
      gjiki->player_core, input->key_cont );
  gjiki_OffMoveBitStep( gjiki );
  gjiki_OffMoveBit( gjiki, JIKI_MOVEBIT_DASH );
  
  if( set == JIKI_MOVEORDER_WALK ){
    gjiki_OffMoveBitUnderOff( gjiki );
  }
  
  gjiki->naminori_end_flag = FALSE;
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �ړ����� DIR_UP��
 * @retval BOOL TRUE=�ړ��J�n FALSE=�ړ��s��
 */
//--------------------------------------------------------------
static BOOL gjiki_CheckMoveStart( FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  
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
/// SE�Đ��`�F�b�N�p�A�g���r���[�g�f�[�^
//--------------------------------------------------------------
#define ATTRFLAG_NONE (MAPATTR_FLAGBIT_NONE)

typedef struct
{
  BOOL (*check)(const MAPATTR_VALUE);
  u16 se;
  s8 dash_count;
  s8 cycle_count;
  u8 season;
  u8 padding[3];
}ATTR_VALUE_SE;

typedef struct
{
  u32 flag;
  u32 se;
  s16 dash_count;
  s16 cycle_count;
}ATTR_FLAG_SE;

//now value
static const ATTR_VALUE_SE data_PlaySE_NowValue[] =
{
  {NULL,0,0,0,PMSEASON_TOTAL}, //end
};

//next value
static const ATTR_VALUE_SE data_PlaySE_NextValue[] =
{
  {MAPATTR_VALUE_CheckLongGrass,SEQ_SE_FLD_08,2,4,PMSEASON_TOTAL},
  {MAPATTR_VALUE_CheckSnow,SEQ_SE_FLD_11,2,4,PMSEASON_TOTAL},
  {MAPATTR_VALUE_CheckSnowNotCycle,SEQ_SE_FLD_11,2,4,PMSEASON_TOTAL},
  {MAPATTR_VALUE_CheckShoal,SEQ_SE_FLD_13,2,4,PMSEASON_TOTAL},
  {MAPATTR_VALUE_CheckPool,SEQ_SE_FLD_13,2,4,PMSEASON_TOTAL},
  {MAPATTR_VALUE_CheckMarsh,SEQ_SE_FLD_13,2,4,PMSEASON_TOTAL},
  {MAPATTR_VALUE_CheckDesertDeep,SEQ_SE_FLD_91,2,4,PMSEASON_TOTAL},
  {MAPATTR_VALUE_CheckSandType,SEQ_SE_FLD_14,2,4,PMSEASON_TOTAL},
  {MAPATTR_VALUE_CheckSeasonGround1,SEQ_SE_FLD_11,2,4,PMSEASON_WINTER},
  {MAPATTR_VALUE_CheckSeasonGround2,SEQ_SE_FLD_11,2,4,PMSEASON_WINTER},
  {NULL,0,0,0,PMSEASON_TOTAL}, //end
};

//now flag
static const ATTR_FLAG_SE data_PlaySE_NowFlag[] =
{
  {ATTRFLAG_NONE,SEQ_SE_DUMMY,0,0}, //end
};

//next flag
static const ATTR_FLAG_SE data_PlaySE_NextFlag[] =
{
  {MAPATTR_FLAGBIT_GRASS,SEQ_SE_FLD_09,2,4},
  {ATTRFLAG_NONE,SEQ_SE_DUMMY,0}, //end
};

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɖ炷SE ���蕔��
 * @param gjiki FIELD_PLAYER_GRID
 * @param set JIKI_MOVEORDER
 * @param pos �`�F�b�N������W
 * @param next TRUE=�ړ��� FALSE=�ړ���
 * @retval BOOL TRUE=SE�Đ�����
 */
//--------------------------------------------------------------
static BOOL gjiki_PlaySECore(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set,
    const VecFx32 *pos, BOOL next, u8 season )
{
  int dash_count;
  u32 se,dash_flag,init_flag;
  const ATTR_FLAG_SE *p0;
  const ATTR_VALUE_SE *p1;
  MMDL *mmdl;
  MAPATTR attr;
  MAPATTR_FLAG flag;
  MAPATTR_VALUE val;
  
  se = SEQ_SE_DUMMY;
  dash_count = 0;
  
  init_flag = TRUE;
  dash_flag = 0;
  
  if( FIELD_PLAYER_CORE_GetMoveForm(
        gjiki->player_core) == PLAYER_MOVE_FORM_CYCLE ){
    dash_flag = 2;
  }else if( gjiki_CheckMoveBit(gjiki,JIKI_MOVEBIT_DASH) ){
    dash_flag = 1;
  }
  
  if( dash_flag == 0 ){
    gjiki->dash_play_se = SEQ_SE_DUMMY;
    gjiki->dash_play_se_count = 0;
  }
  
  mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  
  if( MMDL_GetMapPosAttr(mmdl,pos,&attr) == TRUE ){
    flag = MAPATTR_GetAttrFlag( attr );
    
    if( next == FALSE ){
      p0 = data_PlaySE_NowFlag;
    }else{
      p0 = data_PlaySE_NextFlag;
    }
    
    for( ; p0->flag != ATTRFLAG_NONE; p0++ ){
      if( (p0->flag & flag) ){
        se = p0->se;
        
        if( dash_flag == 1 ){
          dash_count = p0->dash_count;
        }else if( dash_flag == 2 ){
          dash_count = p0->cycle_count;
        }
        break;
      }
    }
    
    if( se == SEQ_SE_DUMMY ){
      if( next == FALSE ){
        p1 = data_PlaySE_NowValue;
      }else{
        p1 = data_PlaySE_NextValue;
      }
      
      for( ; p1->check != NULL; p1++ ){
        if( p1->check(attr) == TRUE ){
          if( p1->season == PMSEASON_TOTAL || p1->season == season ){
            se = p1->se;
          
            if( dash_flag == 1 ){
              dash_count = p1->dash_count;
            }else if( dash_flag == 2 ){
              dash_count = p1->cycle_count;
            }
            break;
          }
        }
      }
    }
    
    if( se == SEQ_SE_DUMMY ){
      if( next == TRUE ){     //�ړ���SE�Đ�����
        gjiki->dash_play_se_count = 0;
      }
    }else{
      if( dash_flag != 0 ){ //dash�n����
        if( gjiki->dash_play_se != se ){
          gjiki->dash_play_se_count = 0;
        }
        
        if( gjiki->dash_play_se_count != 0 &&
            gjiki->dash_play_se_count < dash_count ){
          se = SEQ_SE_DUMMY;
          init_flag = FALSE; //�Đ�SE�͂��邪������������
        }
        
        if( gjiki->dash_play_se_count >= dash_count ){
          gjiki->dash_play_se_count = 0;
        }
        
        gjiki->dash_play_se = se;
        gjiki->dash_play_se_count++;
      }
    }
    
    if( se != SEQ_SE_DUMMY ){
      PMSND_PlaySE( se );
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɖ炷SE
 * @param gjiki FIELD_PLAYER_GRID
 * @param set JIKI_MOVEORDER
 * @param dir �ړ�����
 * @retval nothing
 */
//--------------------------------------------------------------
static void gjiki_PlaySE(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, u16 dir )
{
  if( set != JIKI_MOVEORDER_WALK ){
    gjiki->dash_play_se = SEQ_SE_DUMMY;
    gjiki->dash_play_se_count = 0;
  }else{
    VecFx32 pos;
    MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
    
    {
      GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( gjiki->fieldWork );
//      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      u8 season = PMSEASON_GetConsiderCommSeason( gsys );
      MMDL_GetVectorPos( mmdl, &pos );

      if( gjiki_PlaySECore(gjiki,set,&pos,FALSE,season) == FALSE ){
        MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
        gjiki_PlaySECore( gjiki, set, &pos, TRUE, season );
      }
    }
  }
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
static JIKI_MOVEORDER gjiki_GetMoveOrder(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input )
{
  JIKI_MOVEORDER set;
  PLAYER_MOVE_FORM form;
  
  set = JIKI_MOVEORDER_NON;
  form = FIELD_PLAYER_CORE_GetMoveForm( gjiki->player_core );
  
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    set = gjiki_GetMoveOrder_Normal( gjiki, input );
    break;
  case PLAYER_MOVE_FORM_CYCLE:
    set = gjiki_GetMoveOrder_Cycle( gjiki, input );
    break;
  case PLAYER_MOVE_FORM_SWIM:
  case PLAYER_MOVE_FORM_DIVING:
    set = gjiki_GetMoveOrder_Swim( gjiki, input );
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  return( set );
}

//--------------------------------------------------------------
/**
 * �w����������@�ɓn����PLAYER_MOVE_VALUE�͂ǂ��Ȃ邩�`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �L�[���͕���
 * @retval PLAYER_MOVE_VALUE
 */
//--------------------------------------------------------------
PLAYER_MOVE_VALUE FIELD_PLAYER_GRID_GetMoveValue(
    FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  PLAYER_MOVE_VALUE val = PLAYER_MOVE_VALUE_STOP;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  u16 m_dir = MMDL_GetDirDisp( mmdl );
  
  if( dir == DIR_NOT ){
    return( PLAYER_MOVE_VALUE_STOP );
  }
  
  if( dir != m_dir && (gjiki->move_action != JIKI_ACTION_WALK) &&
      (gjiki->move_action != JIKI_ACTION_HITCH) ){
    return( PLAYER_MOVE_VALUE_TURN );
  }
  
  return( PLAYER_MOVE_VALUE_WALK );
}

//--------------------------------------------------------------
/**
 * ���@�I�[�_�[�`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �ړ������BDIR_UP��
 * @retval BOOL TRUE=�ړ��\ FALSE=�ړ��s��
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_CheckStartMove(
    FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  return FIELD_PLAYER_CORE_CheckStartMove( gjiki->player_core, dir );
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
static JIKI_MOVEORDER gjiki_GetMoveOrder_Normal(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input )
{
  JIKI_MOVEORDER set;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  
  set = JIKI_MOVEORDER_NON;
  
  if( oze_CheckAttrUnderOze(gjiki) == TRUE ||
      gjiki_CheckMoveBitOzeFallOut(gjiki) == TRUE ){
    set = gjiki_GetMoveOrder_Oze( gjiki, input );
    return( set );
  }
  
  switch( gjiki->move_action ){
  case JIKI_ACTION_STOP:
    set = gjiki_CheckMoveOrder_Stop( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_WALK:
    set = gjiki_CheckMoveOrder_Walk( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_TURN:
    set = gjiki_CheckMoveOrder_Turn( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_HITCH:
    set = gjiki_CheckMoveOrder_Hitch( gjiki, mmdl, input );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( input->dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( input->dir != old_dir && input->debug_flag == FALSE ){
        //�_�b�V�����͒��͐U�����𖳌���
        if( (input->mbit & PLAYER_MOVEBIT_DASH) == 0 ){
          return( JIKI_MOVEORDER_TURN );
        }else if( (input->key_cont & PLAYER_PAD_DASH) == 0 ){
          return( JIKI_MOVEORDER_TURN );
        }
      }
      
      return( gjiki_CheckMoveOrder_Walk( gjiki, mmdl, input) );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT )
  {
    return( gjiki_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  {
    MAPATTR attr;
    u32 hit = gjiki_HitCheckMove( gjiki, mmdl, input->dir, &attr );
    
    if( input->debug_flag == TRUE )
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
        
        if( attr_dir != DIR_NOT && attr_dir == input->dir ){
          return( JIKI_MOVEORDER_JUMP ); //�W�����v������v
        }
      }
      
      if( hit == MMDL_MOVEHITBIT_NON )
      {
        if( (flag & MAPATTR_FLAGBIT_WATER) == 0 ){ //���n�`
          return( JIKI_MOVEORDER_WALK );
        }
        
        if( input->debug_flag == TRUE ){ //�f�o�b�O�ړ�
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT ){
    return( gjiki_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  return( gjiki_CheckMoveOrder_Walk( gjiki, mmdl, input) );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( input->dir != DIR_NOT && input->dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( gjiki_CheckMoveOrder_Walk( gjiki, mmdl, input) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT ){
    return( gjiki_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  return( gjiki_CheckMoveOrder_Walk( gjiki, mmdl, input) );
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
static JIKI_MOVEORDER gjiki_GetMoveOrder_Oze(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input )
{
  JIKI_MOVEORDER set;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  
  set = JIKI_MOVEORDER_NON;
  
  switch( gjiki->move_action ){
  case JIKI_ACTION_STOP:
    set = gjikiOze_CheckMoveOrder_Stop( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_WALK:
    set = gjikiOze_CheckMoveOrder_Walk( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_TURN:
    set = gjiki_CheckMoveOrder_Turn( gjiki, mmdl, input );//���p
    break;
  case JIKI_ACTION_HITCH:
    set = gjiki_CheckMoveOrder_Hitch( gjiki, mmdl, input ); //���p
    break;
  case JIKI_ACTION_OZE_YURE:
    set = gjikiOze_CheckMoveOrder_Yure( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_OZE_FALLOUT_JUMP:
  case JIKI_ACTION_OZE_FALLOUT_JUMP_WAIT:
    set = gjikiOze_CheckMoveOrder_WaitFallOutJump( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_OZE_FALLOUT:
    set = gjikiOze_CheckMoveOrder_FallOut( gjiki, mmdl, input );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( input->dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( input->dir != old_dir && input->debug_flag == FALSE ){
        return( JIKI_MOVEORDER_TURN );
      }
      
      return( gjikiOze_CheckMoveOrder_Walk( gjiki, mmdl, input) );
    }
    
    return( gjikiOze_CheckMoveOrder_Yure( gjiki, mmdl, input) );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT )
  {
    return( gjikiOze_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  {
    MAPATTR attr;
    u32 hit = oze_HitCheckMove( gjiki, mmdl, input->dir, &attr );
    
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( input->dir == DIR_NOT ) //���������Ă��Ȃ�
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
    return( gjikiOze_CheckMoveOrder_Walk( gjiki, mmdl, input) );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( oze_CheckFallOutEnd(gjiki) == TRUE ){
    VecFx32 pos;
    //�X�e�[�^�X�����ɖ߂�
    //OBJ�R�[�h�����ɖ߂��K�v�A��
    MMDL_SetStatusBitHeightGetOFF( mmdl, FALSE ); //�����擾�J�n
	  MMDL_OnMoveBit( mmdl, //�W�����v����I�����Z�b�g
        MMDL_MOVEBIT_MOVE_END|MMDL_MOVEBIT_JUMP_END );
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
static JIKI_MOVEORDER gjiki_GetMoveOrder_Cycle(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input )
{
  JIKI_MOVEORDER set;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
   
  set = JIKI_MOVEORDER_NON;
  switch( gjiki->move_action ){
  case JIKI_ACTION_STOP:
    set = gjikiCycle_CheckMoveOrder_Stop( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_WALK:
    set = gjikiCycle_CheckMoveOrder_Walk( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_TURN:
    set = gjikiCycle_CheckMoveOrder_Turn( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_HITCH:
    set = gjikiCycle_CheckMoveOrder_Hitch( gjiki, mmdl, input );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( input->dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( input->dir != old_dir && input->debug_flag == FALSE ){
        return( JIKI_MOVEORDER_TURN );
      }
      
      return( gjikiCycle_CheckMoveOrder_Walk( gjiki, mmdl, input) );
    }
    return( JIKI_MOVEORDER_STOP );
  }
  
  return( JIKI_MOVEORDER_NON );
}

//--------------------------------------------------------------
/**
 * ���]�Ԃ�����Ȃ��A�g���r���[�g���`�F�b�N
 * @param val MAPATTR_VALUE
 * @retval BOOL TRUE=�i���s�A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL gjikiCycle_CheckAttr( MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckLongGrass(val) ||
      MAPATTR_VALUE_CheckOze01(val) ||
      MAPATTR_VALUE_CheckOzeStairs(val) ||
      MAPATTR_VALUE_CheckSnowNotCycle(val) ){
    return( TRUE ); //hit
  }
  
  return( FALSE );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT )
  {
    return( gjikiCycle_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  {
    MAPATTR attr;
    u32 hit = gjiki_HitCheckMove( gjiki, mmdl, input->dir, &attr );
    
    if( input->debug_flag == TRUE )
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
        
        if( attr_dir != DIR_NOT && attr_dir == input->dir ){
          return( JIKI_MOVEORDER_JUMP ); //�W�����v������v
        }
      }
      
      if( hit == MMDL_MOVEHITBIT_NON )
      {
        if( (flag & MAPATTR_FLAGBIT_WATER) == 0 && //not���n�`
            gjikiCycle_CheckAttr(val) == FALSE ){
          return( JIKI_MOVEORDER_WALK );
        }
        
        if( input->debug_flag == TRUE ){ //�f�o�b�O�ړ�
          return( JIKI_MOVEORDER_WALK );
        }
      }
    }
  }
  
  return( JIKI_MOVEORDER_HITCH );
}

#if 0 //��
static JIKI_MOVEORDER gjikiCycle_CheckMoveOrder_Walk(
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT )
  {
    return( gjikiCycle_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  {
    BOOL ret;
    VecFx32 pos;
    MAPATTR attr;
    MAPATTR_VALUE val;
    MAPATTR_FLAG flag;
    u32 hit = MMDL_HitCheckMoveDir( mmdl, input->dir );
    
    if( input->debug_flag == TRUE )
    {
      if( hit != MMDL_MOVEHITBIT_NON &&
          !(hit&MMDL_MOVEHITBIT_OUTRANGE) )
      {
        return( JIKI_MOVEORDER_WALK );
      }
    }
    
    MMDL_GetVectorPos( mmdl, &pos );
    MMDL_TOOL_AddDirVector( input->dir, &pos, GRID_FX32 );
    ret = MMDL_GetMapPosAttr( mmdl, &pos, &attr );
    val = MAPATTR_GetAttrValue( attr );
    flag = MAPATTR_GetAttrFlag( attr );
    
    if( (hit & MMDL_MOVEHITBIT_ATTR) )
    {
      if( ret == TRUE )
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
        
        if( attr_dir != DIR_NOT && attr_dir == input->dir )
        {
          return( JIKI_MOVEORDER_JUMP ); //�W�����v������v
        }
      }
    }
    
    if( hit == MMDL_MOVEHITBIT_NON )
    {
      if( (flag & MAPATTR_FLAGBIT_WATER) ||
          MAPATTR_VALUE_CheckSnowNotCycle(val) == FALSE ){
        return( JIKI_MOVEORDER_WALK );
      }
    }
  }
  
  return( JIKI_MOVEORDER_HITCH );
}
#endif

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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT ){
    return( gjikiCycle_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  return( gjikiCycle_CheckMoveOrder_Walk( gjiki, mmdl, input) );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( input->dir != DIR_NOT && input->dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( gjikiCycle_CheckMoveOrder_Walk( gjiki, mmdl, input) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT ){
    return( gjikiCycle_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  return( gjikiCycle_CheckMoveOrder_Walk( gjiki, mmdl, input) );
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
static JIKI_MOVEORDER gjiki_GetMoveOrder_Swim(
    FIELD_PLAYER_GRID *gjiki, const INPUTDATA *input )
{
  JIKI_MOVEORDER set;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
   
  set = JIKI_MOVEORDER_NON;
  switch( gjiki->move_action ){
  case JIKI_ACTION_STOP:
    set = gjikiSwim_CheckMoveOrder_Stop( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_WALK:
    set = gjikiSwim_CheckMoveOrder_Walk( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_TURN:
    set = gjikiSwim_CheckMoveOrder_Turn( gjiki, mmdl, input );
    break;
  case JIKI_ACTION_HITCH:
    set = gjikiSwim_CheckMoveOrder_Hitch( gjiki, mmdl, input );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == TRUE ){
    if( input->dir != DIR_NOT ){
      u16 old_dir;
      old_dir = MMDL_GetDirDisp( mmdl );
      
      if( input->dir != old_dir && input->debug_flag == FALSE ){
        return( JIKI_MOVEORDER_TURN );
      }
      
      return( gjikiSwim_CheckMoveOrder_Walk( gjiki, mmdl, input) );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT )
  {
    return( gjikiSwim_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  {
    MAPATTR attr;
    u32 hit = gjiki_HitCheckMove( gjiki, mmdl, input->dir, &attr );
    
    if( input->debug_flag == TRUE )
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
        
        if( input->debug_flag == TRUE ){
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT ){
    return( gjikiSwim_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  return( gjikiSwim_CheckMoveOrder_Walk( gjiki, mmdl, input) );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 dir_now = MMDL_GetDirDisp( mmdl );
   
  if( input->dir != DIR_NOT && input->dir != dir_now ){
    MMDL_FreeAcmd( mmdl );
    return( gjikiSwim_CheckMoveOrder_Walk( gjiki, mmdl, input) );
  }
  
  if( MMDL_CheckPossibleAcmd(mmdl) == FALSE ){
    return( JIKI_MOVEORDER_NON );
  }
  
  if( input->dir == DIR_NOT ){
    return( gjikiSwim_CheckMoveOrder_Stop( gjiki, mmdl, input) );
  }
  
  return( gjikiSwim_CheckMoveOrder_Walk( gjiki, mmdl, input) );
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
static void gjiki_SetMove( FIELD_PLAYER_GRID *gjiki,
    JIKI_MOVEORDER set, const INPUTDATA *input )
{
  PLAYER_MOVE_FORM form;
  
  form = FIELD_PLAYER_CORE_GetMoveForm( gjiki->player_core );
  
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    gjiki_SetMove_Normal( gjiki, set, input );
    break;
  case PLAYER_MOVE_FORM_CYCLE:
    gjiki_SetMove_Cycle( gjiki, set, input );
    break;
  case PLAYER_MOVE_FORM_SWIM:
  case PLAYER_MOVE_FORM_DIVING:
    gjiki_SetMove_Swim( gjiki, set, input );
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
static void gjiki_SetMove_Normal(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, const INPUTDATA *input )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  
  switch( set ){
  case JIKI_MOVEORDER_NON:
    gjiki_SetMove_Non( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_STOP:
    gjiki_SetMove_Stop( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_WALK:
    gjiki_SetMove_Walk( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_TURN:
    gjiki_SetMove_Turn( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_HITCH:
    gjiki_SetMove_Hitch( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_JUMP:
    gjiki_SetMove_Jump( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_OZE_YURE:
    gjikiOze_SetMove_Yure( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_OZE_FALLOUT_JUMP_START:
    gjikiOze_SetMove_FallOutJumpStart( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_OZE_FALLOUT_JUMP_WAIT:
    gjikiOze_SetMove_FallOutJumpWait( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_OZE_FALLOUT:
    gjikiOze_SetMove_FallOut( gjiki, mmdl, input );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  u16 dir = input->dir;
  
  if( dir == DIR_NOT ){
    dir = MMDL_GetDirDisp( mmdl );
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_STOP;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_STOP );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  BOOL flat_check = TRUE;
  
  GF_ASSERT( input->dir != DIR_NOT );
  
  if( input->debug_flag == TRUE )
  {
    code = AC_WALK_U_2F;
    flat_check = FALSE;
  }
  else if( (input->mbit & PLAYER_MOVEBIT_DASH) &&
      (input->key_cont & PLAYER_PAD_DASH) )
  {
    if( FIELD_PLAYER_CORE_CheckIllegalOBJCode(
          gjiki->player_core) == FALSE )
    {
      code = AC_WALK_U_4F;
    }
    else
    {
      VecFx32 pos;
      MAPATTR attr;
      
      code = AC_DASH_U_4F;
      gjiki_OnMoveBit( gjiki, JIKI_MOVEBIT_DASH );
      FIELD_PLAYER_CORE_GetPos( gjiki->player_core, &pos );
      
      if( MMDL_GetMapPosAttr( mmdl, &pos, &attr ))
      {
        if( MAPATTR_VALUE_CheckUpDownFloor(MAPATTR_GetAttrValue(attr)) )
        {
          code = AC_WALK_U_8F; //�㉺�����ł̓_�b�V���Ȃ�
          flat_check = FALSE;
          gjiki_OffMoveBit( gjiki, JIKI_MOVEBIT_DASH );
        }
      }
    }
  }
  else
  {
    code = AC_WALK_U_8F;
  }
  
  if( flat_check == TRUE )
  {
    if( gjiki_CheckNextHeight(gjiki,input->dir) == GROUND_HEIGHT_UP )
    {
      switch( code ){
      case AC_WALK_U_4F:
        code = AC_WALK_U_6F;
        break;
      case AC_DASH_U_4F:
        code = AC_DASH_U_6F;
        break;
      case AC_WALK_U_8F:
        code = AC_WALK_U_12F;
        break;
      }
    }
  }
  
  code = MMDL_ChangeDirAcmdCode( input->dir, code );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_WALK;

  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_WALK );

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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  
  GF_ASSERT( input->dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( input->dir, JIKI_AC_TURN_U );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_TURN;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_TURN );

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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  
  GF_ASSERT( input->dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( input->dir, AC_STAY_WALK_U_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_HITCH;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_STOP );

  if( gjiki_CheckDirDoor(gjiki,input->dir) == FALSE ){
    PMSND_PlaySE( SEQ_SE_WALL_HIT );
  }
  
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  
  GF_ASSERT( input->dir != DIR_NOT );
  
  code = MMDL_ChangeDirAcmdCode( input->dir, AC_JUMP_U_2G_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_WALK;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_WALK );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  u16 dir = input->dir;

  if( dir == DIR_NOT ){ //�h����E�ɂ�藎����
    dir = oze_CheckAttrOzeFallOut( gjiki );
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_1G_8F );
  MMDL_SetAcmd( mmdl, code );
  MMDL_SetStatusBitHeightGetOFF( mmdl, TRUE ); //�����擾���֎~��
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH );

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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  fx32 height;
  VecFx32 pos;
  
  mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  MMDL_GetVectorPos( mmdl, &pos );
  MMDL_GetMapPosHeight( mmdl, &pos, &height );
  
  if( pos.y > height ){
    pos.y += NUM_FX32( -8 );
  
    if( pos.y < height ){
      pos.y = height;
      MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH );
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
static void gjiki_SetMove_Cycle(
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, const INPUTDATA *input )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );

  switch( set ){
  case JIKI_MOVEORDER_NON:
    gjikiCycle_SetMove_Non( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_STOP:
    gjikiCycle_SetMove_Stop( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_WALK:
    gjikiCycle_SetMove_Walk( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_TURN:
    gjikiCycle_SetMove_Turn( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_HITCH:
    gjikiCycle_SetMove_Hitch( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_JUMP:
    gjikiCycle_SetMove_Jump( gjiki, mmdl, input );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  u16 dir = input->dir;

  if( dir == DIR_NOT ){
    dir = MMDL_GetDirDisp( mmdl );
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_STOP;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_STOP );

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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  BOOL flat_check = TRUE;
  
  GF_ASSERT( input->dir != DIR_NOT );
  
  if( input->debug_flag == TRUE ){
    code = AC_WALK_U_2F;
    flat_check = FALSE;
  }else{
    MAPATTR attr;
    code = AC_WALK_U_2F;
    
    if( gjiki_GetAttr(gjiki,DIR_NOT,&attr) == TRUE ){ //�[�������Ȃ瑬�x�ቺ
      MAPATTR_VALUE val = MAPATTR_GetAttrValue( attr );
      
      if( MAPATTR_VALUE_CheckDesertDeep(val) == TRUE ){
        FLDEFF_CTRL *fectrl;
        code = AC_SANDWALK_U_16F; //��p
        
        fectrl = FIELDMAP_GetFldEffCtrl( gjiki->fieldWork );
        FLDEFF_KEMURI_SetMMdl( mmdl, fectrl ); //���o��
      }
    }
  }
  
  if( flat_check == TRUE ){
    if( gjiki_CheckNextHeight(gjiki,input->dir) == GROUND_HEIGHT_UP ){
      if( code == AC_WALK_U_2F ){
        code = AC_WALK_U_4F;
      }
    }
  }
  
  code = MMDL_ChangeDirAcmdCode( input->dir, code );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_WALK;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_WALK );

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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  
  GF_ASSERT( input->dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( input->dir, JIKI_AC_TURN_U );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_TURN;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_TURN );

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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  
  GF_ASSERT( input->dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( input->dir, AC_STAY_WALK_U_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_HITCH;
  
  FIELD_PLAYER_CORE_SetMoveValue( gjiki->player_core, PLAYER_MOVE_VALUE_STOP );
  
  if( gjiki_CheckDirDoor(gjiki,input->dir) == FALSE ){
    PMSND_PlaySE( SEQ_SE_WALL_HIT );
  }
  
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  
  GF_ASSERT( input->dir != DIR_NOT );
  
  code = MMDL_ChangeDirAcmdCode( input->dir, AC_JUMP_U_2G_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_WALK;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_WALK );
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
    FIELD_PLAYER_GRID *gjiki, JIKI_MOVEORDER set, const INPUTDATA *input )
{
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );

  switch( set ){
  case JIKI_MOVEORDER_NON:
    gjikiSwim_SetMove_Non( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_STOP:
    gjikiSwim_SetMove_Stop( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_WALK:
    gjikiSwim_SetMove_Walk( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_TURN:
    gjikiSwim_SetMove_Turn( gjiki, mmdl, input );
    break;
  case JIKI_MOVEORDER_HITCH:
    gjikiSwim_SetMove_Hitch( gjiki, mmdl, input );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  u16 dir = input->dir;

  if( dir == DIR_NOT ){
    dir = MMDL_GetDirDisp( mmdl );
  }
  
  code = MMDL_ChangeDirAcmdCode( dir, AC_DIR_U );
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_STOP;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_STOP );

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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  
  GF_ASSERT( input->dir != DIR_NOT );
  
  if( input->debug_flag == TRUE ){
    code = AC_WALK_U_2F;
  }else{
    code = AC_WALK_U_4F;
  }
  
  code = MMDL_ChangeDirAcmdCode( input->dir, code );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_WALK;

  FIELD_PLAYER_CORE_SetMoveValue( gjiki->player_core, PLAYER_MOVE_VALUE_WALK );
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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  
  GF_ASSERT( input->dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( input->dir, JIKI_AC_TURN_U );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_TURN;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_TURN );

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
  FIELD_PLAYER_GRID *gjiki, MMDL *mmdl, const INPUTDATA *input )
{
  u16 code;
  
  GF_ASSERT( input->dir != DIR_NOT );
  code = MMDL_ChangeDirAcmdCode( input->dir, AC_STAY_WALK_U_16F );
  
  MMDL_SetAcmd( mmdl, code );
  gjiki->move_action = JIKI_ACTION_HITCH;
  
  FIELD_PLAYER_CORE_SetMoveValue(
      gjiki->player_core, PLAYER_MOVE_VALUE_STOP );
  
  if( gjiki_CheckDirDoor(gjiki,input->dir) == FALSE ){
    PMSND_PlaySE( SEQ_SE_WALL_HIT );
  }

#ifdef PLAYER_MOVE_TRACK_CHANGE
  FIELD_SOUND_ChangeBGMTrackStill();
#endif
}

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
  s16 gy = MMDL_GetGridPosY( mmdl );
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
 * @param player_core FIELD_PLAYER
 * @retval nothing
 * @note ���@��������~�o���Ȃ��ꍇ�͂��̂܂�
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_ForceStop( FIELD_PLAYER_GRID *gjiki )
{
  if( gjiki->move_action == JIKI_ACTION_HITCH ){
    MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
    u16 dir = MMDL_GetDirDisp( mmdl );
    MMDL_FreeAcmd( mmdl );
    MMDL_SetDirDisp( mmdl, dir );
    MMDL_SetDrawStatus( mmdl, DRAW_STA_STOP );
    FIELD_PLAYER_CORE_SetMoveValue( gjiki->player_core, PLAYER_MOVE_VALUE_STOP );
  }
}

//--------------------------------------------------------------
/**
 * ���@�@�����~
 * @param gjiki
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
      FIELD_PLAYER_CORE_SetMoveValue( gjiki->player_core, PLAYER_MOVE_VALUE_STOP );
      break;
    }
  }
  
  return( TRUE );
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
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
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
BOOL FIELD_PLAYER_GRID_CheckUnderForceMove( FIELD_PLAYER_GRID * gjiki )
{
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
BOOL FIELD_PLAYER_GRID_CheckOzeFallOut( FIELD_PLAYER_GRID * gjiki )
{
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
    MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
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
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  u16 jiki_dir = MMDL_GetDirMove( mmdl );
  u32 hit = gjiki_HitCheckMove( gjiki, mmdl, jiki_dir, &attr );
	
  if( hit != MMDL_MOVEHITBIT_NON ){ //��Q���q�b�g
    gjiki_ClearUnderForceMove( gjiki );
    gjiki_OnMoveBitUnderOff( gjiki );
    FIELD_PLAYER_CORE_SetMoveValue( gjiki->player_core, PLAYER_MOVE_VALUE_STOP );
    return( FALSE );
  }
  
  gjiki_OnMoveBitForce( gjiki );
  
  {
	  u16 code = MMDL_ChangeDirAcmdCode( jiki_dir, AC_WALK_U_4F );
    
    MMDL_SetAcmd( mmdl, code );
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_PAUSE_DIR|MMDL_STABIT_PAUSE_ANM );
    
    gjiki->move_action = JIKI_ACTION_WALK;
    FIELD_PLAYER_CORE_SetMoveValue( gjiki->player_core, PLAYER_MOVE_VALUE_WALK );
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
  
  dir = FIELD_PLAYER_CORE_GetDir( gjiki->player_core );
  
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
  
  mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
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
  FIELD_PLAYER_CORE_ChangeDrawForm(
    gjiki->player_core, PLAYER_DRAW_FORM_YURE );
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
  FIELD_PLAYER_CORE_ResetMoveForm( gjiki->player_core );
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
FIELD_PLAYER_CORE * FIELD_PLAYER_GRID_GetFieldPlayerCore( FIELD_PLAYER_GRID *gjiki )
{
  return( gjiki->player_core );
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
  return( FIELD_PLAYER_CORE_GetMMdl(gjiki->player_core) );
}


//--------------------------------------------------------------
/**
 * ���@������\���̏ꍇ�͌��ɖ߂��B
 * @param gjiki
 * @retval nothing
 * @note ���̏��A�Y���ӏ��͔�������Ԃ̂�
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_CheckSpecialDrawForm(
    FIELD_PLAYER_GRID *gjiki, BOOL menu_open_flag )
{
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

//--------------------------------------------------------------
/**
 * ���@���g���C�x���g�I�����ォ
 * @param gjiki
 * @retval BOOL
 */
//--------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_CheckNaminoriEventEnd( FIELD_PLAYER_GRID *gjiki )
{
  return( gjiki->naminori_end_flag );
}

//--------------------------------------------------------------
/**
 * ���@���g���C�x���g�I������̃t���O���Z�b�g
 * @param gjiki
 * @retval BOOL
 */
//--------------------------------------------------------------
void FIELD_PLAYER_GRID_SetNaminoriEventEnd( FIELD_PLAYER_GRID *gjiki, BOOL flag )
{
  if( flag == TRUE ){
    gjiki->naminori_end_flag = TRUE;
  }else{
    gjiki->naminori_end_flag = FALSE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �q�b�`�`�F�b�N
 *
 *	@param	gjiki 
 *
 *	@retval TRUE  �ǃq�b�g���
 */
//-----------------------------------------------------------------------------
BOOL FIELD_PLAYER_GRID_CheckHitch( const FIELD_PLAYER_GRID *gjiki )
{
  if( gjiki->move_action == JIKI_ACTION_HITCH ){
    return TRUE;
  }
  return FALSE;
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
  
  FIELD_PLAYER_CORE_GetPos( gjiki->player_core, &pos );
  
  if( dir != DIR_NOT ){
    MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  }
  
  mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  ret = MMDL_GetMapPosAttr( mmdl, &pos, attr );
  return( ret );
}

//--------------------------------------------------------------
/**
 * �ړ���̍��፷���`�F�b�N
 * @param gjiki FIELD_PLAYER_GRID
 * @param dir �ړ�����
 * @retval  GROUND_HEIGHT
 */
//--------------------------------------------------------------
static GROUND_HEIGHT gjiki_CheckNextHeight(
    FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  VecFx32 pos;
  fx32 now,next;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  
  MMDL_GetVectorPos( mmdl, &pos );
  now = pos.y;
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  
  if( MMDL_GetMapPosHeight(mmdl,&pos,&next) == TRUE ){
    if( now < next ){
      return( GROUND_HEIGHT_UP );
    }else if( now > next ){
      return( GROUND_HEIGHT_DOWN );
    }
  }
  
  return( GROUND_HEIGHT_FLAT );
}

//--------------------------------------------------------------
/**
 * �w��������h�A���ǂ���
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL gjiki_CheckDirDoor( FIELD_PLAYER_GRID *gjiki, u16 dir )
{
  int idx;
  VecFx32 pos;
  MMDL *mmdl = FIELD_PLAYER_CORE_GetMMdl( gjiki->player_core );
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( gjiki->fieldWork );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
  
  MMDL_GetVectorPos( mmdl, &pos );
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
	idx = EVENTDATA_SearchConnectIDByPos( evdata, &pos );
  
  if( idx == EXIT_ID_NONE ){
    return( FALSE );
  }
  
  return( TRUE );
}
