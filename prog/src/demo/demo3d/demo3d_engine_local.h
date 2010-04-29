//==========================================================================
/*
 *  @file   demo3d_engine_local.h
 *  @brief  Demo3D �G���W���@���[�J���w�b�_
 *  @author Miyuki Iwasawa
 *  @date   10.03.17
 */
//==========================================================================

#pragma once

struct _DEMO3D_ENGINE_WORK {
  // [IN]
  DEMO3D_GRAPHIC_WORK*    graphic;
  DEMO3D_ID               demo_id;
  u32                     end_result;

  // [PRIVATE]
  HEAPID        heapID;
  HEAPID        tmpHeapID;
  BOOL          is_double;
  fx32          anime_speed;  ///< �A�j���[�V�����X�s�[�h
  ICA_ANIME*    ica_anime;
  GFL_G3D_UTIL*   g3d_util;
  GFL_G3D_CAMERA* camera;
  fx32  def_top;
  fx32  def_bottom;
  u32   scene_unit_max;
  GFL_G3D_OBJSTATUS obj_status;

  DEMO3D_CMD_WORK*  cmd;

  u16* unit_idx; // unit_idx�ێ��iALLOC)

  //�V�[���p�����[�^�ێ�
  const DEMO3D_SCENE_DATA* scene;
  DEMO3D_SCENE_ENV        env;
};


