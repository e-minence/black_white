/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ���ʒ�`
 * @file   research_common.c
 * @author obata
 * @date   2010.02.13
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>


//===============================================================================
// ��VRAM-Bank �ݒ�
//===============================================================================
const GFL_DISP_VRAM VRAMBankSettings =
{
  GX_VRAM_BG_128_A,            // MAIN-BG
  GX_VRAM_BGEXTPLTT_NONE,      // MAIN-BG-EXP-PLT
  GX_VRAM_SUB_BG_128_C,        // SUB--BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,  // SUB--BG-EXP-PLT
  GX_VRAM_OBJ_64_E,            // MAIN-OBJ
  GX_VRAM_OBJEXTPLTT_NONE,     // MAIN-OBJ-EXP-PLT
  GX_VRAM_SUB_OBJ_16_I,        // SUB--OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE, // SUB--OBJ-EXP-PLT
  GX_VRAM_TEX_0_B,             // TEX-IMG
  GX_VRAM_TEXPLTT_0_G,         // TEX-PLT
  GX_OBJVRAMMODE_CHAR_1D_32K,  // MAIN-OBJ-MAPPING-MODE
  GX_OBJVRAMMODE_CHAR_1D_32K,  // SUB--OBJ-MAPPING-MODE
}; 

//===============================================================================
// ��BG���[�h�ݒ�
//===============================================================================
// 2D
const GFL_BG_SYS_HEADER BGSysHeader2D = 
{
  GX_DISPMODE_GRAPHICS,   // �\�����[�h�w��
  GX_BGMODE_0,            // �a�f���[�h�w��(���C���X�N���[��)
  GX_BGMODE_0,            // �a�f���[�h�w��(�T�u�X�N���[��)
  GX_BG0_AS_2D            // �a�f�O�̂Q�c�A�R�c���[�h�I��
}; 
// 3D
const GFL_BG_SYS_HEADER BGSysHeader3D = 
{
  GX_DISPMODE_GRAPHICS,   // �\�����[�h�w��
  GX_BGMODE_0,            // �a�f���[�h�w��(���C���X�N���[��)
  GX_BGMODE_0,            // �a�f���[�h�w��(�T�u�X�N���[��)
  GX_BG0_AS_3D            // �a�f�O�̂Q�c�A�R�c���[�h�I��
};
