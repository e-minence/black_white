//============================================================================
/**
 *
 *	@file		demo3d_restbl.h
 *	@brief
 *	@author		hosaka genya
 *	@data		2009.11.27
 *
 */
//============================================================================
#pragma once

//@TODO ���resource�t�H���_���玩������������\��B

#include "demo3d_restbl.h"
#include "arc/demo3d.naix"

enum
{ 
  RESID_01_IMD = 0,
  RESID_01_ICA,
};

// �f��
static const GFL_G3D_UTIL_RES res_table_unit01[] = 
{
  { ARCID_DEMO3D_GRA, NARC_demo3d_cdemo_01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_DEMO3D_GRA, NARC_demo3d_cdemo_01_nsbca, GFL_G3D_UTIL_RESARC },
};

static const GFL_G3D_UTIL_RES res_table_unit02[] = 
{
  { ARCID_DEMO3D_GRA, NARC_demo3d_cdemo_02_nsbmd, GFL_G3D_UTIL_RESARC },
};

static const GFL_G3D_UTIL_RES res_table_unit03[] = 
{
  { ARCID_DEMO3D_GRA, NARC_demo3d_cdemo_03_nsbmd, GFL_G3D_UTIL_RESARC },
};


// �A�j���̎w��
static const GFL_G3D_UTIL_ANM anm_table_unit01[] = 
{
  { RESID_01_ICA, 0 }, // ICA
};

static const GFL_G3D_UTIL_OBJ obj_table_unit01[] = 
{
  {
    RESID_01_IMD,         // ���f�����\�[�XID
    0,                        // ���f���f�[�^ID(���\�[�X����INDEX)
    RESID_01_IMD,         // �e�N�X�`�����\�[�XID
    anm_table_unit01,         // �A�j���e�[�u��(�����w��̂���)
    NELEMS(anm_table_unit01), // �A�j�����\�[�X��
  },
};

static const GFL_G3D_UTIL_OBJ obj_table_unit02[] = 
{
  {
    0, // ���f�����\�[�XID
    0,                // ���f���f�[�^ID(���\�[�X����INDEX)
    0, // �e�N�X�`�����\�[�XID
    NULL,
    0,
  },
};

static const GFL_G3D_UTIL_OBJ obj_table_unit03[] = 
{
  {
    0,
    0,
    0,
    NULL,
    0,
  },
}; 


// �Z�b�g�A�b�v�ԍ�
enum
{
  SETUP_INDEX_unit01,
  SETUP_INDEX_unit02,
  SETUP_INDEX_unit03,
  SETUP_INDEX_MAX
};

// �Z�b�g�A�b�v�f�[�^
static const GFL_G3D_UTIL_SETUP setup[] =
{
  { res_table_unit01, NELEMS(res_table_unit01), obj_table_unit01, NELEMS(obj_table_unit01) },
  { res_table_unit02, NELEMS(res_table_unit02), obj_table_unit02, NELEMS(obj_table_unit02) },
  { res_table_unit03, NELEMS(res_table_unit03), obj_table_unit03, NELEMS(obj_table_unit03) },
};

u8 Demo3D_RESTBL_GetUnitMax( void )
{
  return SETUP_INDEX_MAX;
}

const GFL_G3D_UTIL_SETUP* Demo3D_RESTBL_GetG3DUtilSetup( u8 idx )
{
  GF_ASSERT( idx < SETUP_INDEX_MAX );
  return &setup[ idx ];
}


// �J�����f�[�^
static const int c_ica_tbl[2] = 
{ 
  ARCID_DEMO3D_GRA, NARC_demo3d_c_camera_bin,
};

ICA_ANIME* Demo3D_RESTBL_CreatICACamera(  HEAPID heapID, int buf_interval )
{
  return ICA_ANIME_CreateStreamingAlloc( heapID, c_ica_tbl[0], c_ica_tbl[1], buf_interval );
}


