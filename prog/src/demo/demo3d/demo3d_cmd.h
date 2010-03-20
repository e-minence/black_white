//=============================================================================
/**
 *	@file		demo3d_cmd.h
 *	@brief  3D�f���R�}���h���[�J���w�b�_
 *	@author Miyuki Iwasawa	
 *	@data		2010.03.17
 */
//=============================================================================

#pragma once

#include "demo3d_graphic.h"
#include "demo3d_mb.h" 

//�f�[�^
#include "demo3d_data.h"

#include "demo3d_engine.h"
#include "demo3d_cmdsys.h"
#include "demo3d_engine_local.h"

//�R�}���h���t���[�Ɏg����BG�t���[����`
#define DEMO3D_CMD_FREE_BG_M1 (GFL_BG_FRAME1_M)

//--------------------------------------------------------------
///	�R�}���h���[�N
//==============================================================
struct _DEMO3D_CMD_WORK {
  // [IN]
  HEAPID heapID;
  HEAPID tmpHeapID;
  DEMO3D_ID demo_id;
 
  // [PRIVATE]
  BOOL  is_cmd_end;
  BOOL  cmdsys_end_f; ///<�R�}���h�^�X�N�̎��E�p
  int   pre_frame; ///< 1sync=1
  int   cmd_idx;
  u8    msg_spd;
  u8    bgm_push_f;

  //�R�}���h����
  DEMO3D_ENGINE_WORK* core;

  GFL_TCBLSYS*  tcbsys;
  GFL_FONT *fontHandle;
  
  DEMO3D_MBL_WORK* mb;
};

extern void (*DATA_Demo3D_CmdTable[ DEMO3D_CMD_TYPE_MAX ])();

