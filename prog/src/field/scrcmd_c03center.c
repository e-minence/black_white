//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_c03center.c
 *	@brief  �X�N���v�g�R�}���h�@C03�����@�̏o������C�x���g
 *	@author	tomoya takahshi
 *	@date		2010.01.13
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "script.h"
#include "script_def.h"
#include "script_local.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "fieldmap.h"

#include "scrcmd_c03center.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�Ǘ������̃V�[�P���X
//=====================================
enum {
  SEQ_MAIN,
} ;

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���@�����Ǘ������̃��[�N
//=====================================
typedef struct {
 
  s32 start_rot;
  s32 div_rot;

  s32 frame;
  s32 frame_max;
  
} PLAYERWAY_CONTORL_WK;


// MMDLSYS_SetUp�ɓn���p�x
static u16 s_ControlPlayerWay = 0;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static void ControlPlayerWayTCB( GFL_TCB* p_tcb, void* p_work );

//----------------------------------------------------------------------------
/**
 *	@brief  C03�Z���^�[���@�����̒����V�X�e��
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdC03Center_ControlPlayerWay( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELDMAP_WORK* fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  GFL_TCBSYS* tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldWork );
  PLAYERWAY_CONTORL_WK* local_wk;
  HEAPID heapID = FIELDMAP_GetHeapID( fieldWork );
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldWork );
  MMDLSYS * mmdlsys = FIELDMAP_GetMMdlSys( fieldWork );
  const u16* now_angle;
  FLD_CAM_MV_PARAM_CORE* camera_move;
  u16          frame = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����
  u32 amari;

  // ���[�N����
  local_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(PLAYERWAY_CONTORL_WK) );

  now_angle = FIELD_CAMERA_GetAngleYawAddress( camera );
  s_ControlPlayerWay = *now_angle;

  // ����TCB���R�[�����AFLDMMDL�ɓn���Ă���Yaw�̒l�𒲐�
  MMDLSYS_SetupDrawProc( mmdlsys, &s_ControlPlayerWay );

  OS_TPrintf( "MMDL_SYS�ɐݒ肷�� DrawAngle��EvCmdC03Center_ControlPlayerWay�ŕύX\n" );

  // �ړ�������A���O�������߂�
  camera_move = FIELD_CAMERA_GetMoveDstPrmPtr( camera );

  // 
  local_wk->start_rot = s_ControlPlayerWay;
  local_wk->div_rot   = camera_move->AngleYaw;

  // div_rot��90�x�P�ʂɂ���
  amari               = local_wk->div_rot % 0x4000;
  if( amari >= 0x2000 ){ 
    local_wk->div_rot += 0x4000 - amari;
  }else{
    local_wk->div_rot -= amari;
  }

  // ��]����l�����߂�
  local_wk->div_rot   = local_wk->div_rot - local_wk->start_rot;
  if( MATH_ABS( local_wk->div_rot ) >= 0x8000 ) // 180�x�ȏ�Ȃ�t��]
  {
    if( local_wk->div_rot > 0 ){
      local_wk->div_rot = local_wk->div_rot - 0x10000;
    }else{
      local_wk->div_rot = local_wk->div_rot + 0x10000;
    }
  }
  local_wk->frame     = 0;
  local_wk->frame_max = frame;


  // TCB�N��
  GFL_TCB_AddTask( tcbsys, ControlPlayerWayTCB, local_wk, 0 );

  return VMCMD_RESULT_CONTINUE;
}






//-----------------------------------------------------------------------------
/**
 *  private�֐��S
 */
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief    ���@�̕������Ǘ��@�i�ŏI�I�ɁA�O�������悤�ɊǗ��j
 */
//-----------------------------------------------------------------------------
static void ControlPlayerWayTCB( GFL_TCB* p_tcb, void* p_work )
{
  PLAYERWAY_CONTORL_WK* p_wk = p_work;

  p_wk->frame ++;

  s_ControlPlayerWay = (p_wk->div_rot * p_wk->frame) / p_wk->frame_max;
  s_ControlPlayerWay += p_wk->start_rot;
  
  if( p_wk->frame >= p_wk->frame_max )
  {
    GFL_HEAP_FreeMemory( p_work );
    // ����
    GFL_TCB_DeleteTask( p_tcb );
  }
}




