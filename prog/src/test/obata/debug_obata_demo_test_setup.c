#include <gflib.h>
#include "debug_obata_demo_test_setup.h"


#define cameraPerspway  ( 0x0b60 )
#define cameraAspect    ( FX32_ONE * 4 / 3 )
#define cameraNear	    ( 1 << FX32_SHIFT )
#define cameraFar       ( 1024 << FX32_SHIFT )


//============================================================================================
/**
 * @brief BG�f�[�^
 */
//============================================================================================
// �D�揇��
enum
{
	PRIORITY_MAIN_BG1,
	PRIORITY_MAIN_BG0,
};

// VRAM�o���N�̊��蓖��
static const GFL_DISP_VRAM sc_VRAM_param = 
{ 
	GX_VRAM_BG_128_B,				      // ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,		  	// ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_128_C,		  	  // �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,  	// �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_80_EF,				    // ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		  // ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_16_I,			    // �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_0_A,				      // �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_0_G,			    // �e�N�X�`���p���b�g�X���b�g
	GX_OBJVRAMMODE_CHAR_1D_32K,
	GX_OBJVRAMMODE_CHAR_1D_32K,
};


// BG���[�h�ݒ�
static const GFL_BG_SYS_HEADER sc_BG_mode = 
{
	GX_DISPMODE_GRAPHICS,	// �\�����[�h
	GX_BGMODE_0,		   	  // BG���[�h(���C���X�N���[��)
	GX_BGMODE_0,		      // BG���[�h(�T�u�X�N���[��) 
	GX_BG0_AS_3D,		      // BG0��2D or 3D���[�h�I��
};

// BG�R���g���[���ݒ�
static const GFL_BG_BGCNT_HEADER sc_BGCNT1_M = 
{
	0, 0,				              // �����\���ʒu
	0x800,					          // �X�N���[���o�b�t�@�T�C�Y
	0,						            // �X�N���[���o�b�t�@�I�t�Z�b�g
	GFL_BG_SCRSIZ_256x256,	  // �X�N���[���T�C�Y
	GX_BG_COLORMODE_16,		    // �J���[���[�h
	GX_BG_SCRBASE_0x0000,	    // �X�N���[���x�[�X�u���b�N
	GX_BG_CHARBASE_0x04000,	  // �L�����N�^�x�[�X�u���b�N
	GFL_BG_CHRSIZ_256x256,	  // �L�����N�^�G���A�T�C�Y
	GX_BG_EXTPLTT_01,		      // BG�g���p���b�g�X���b�g�I��
	PRIORITY_MAIN_BG1,		    // �\���v���C�I���e�B�[
	0,						            // �G���A�I�[�o�[�t���O
	0,						            // DUMMY
	FALSE,					          // ���U�C�N�ݒ�
};

static const GFL_BG_BGCNT_HEADER sc_BGCNT1_S = 
{
	0, 0,				            // �����\���ʒu
	0x800,					        // �X�N���[���o�b�t�@�T�C�Y
	0,						          // �X�N���[���o�b�t�@�I�t�Z�b�g
	GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
	GX_BG_COLORMODE_16,		  // �J���[���[�h
	GX_BG_SCRBASE_0x0000,	  // �X�N���[���x�[�X�u���b�N
	GX_BG_CHARBASE_0x04000,	// �L�����N�^�x�[�X�u���b�N
	GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
	GX_BG_EXTPLTT_01,		    // BG�g���p���b�g�X���b�g�I��
	0,						          // �\���v���C�I���e�B�[
	0,						          // �G���A�I�[�o�[�t���O
	0,						          // DUMMY
	FALSE,					        // ���U�C�N�ݒ�
};


//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light_data[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light_setup = { light_data, NELEMS(light_data) };
GFL_G3D_LIGHTSET* g_lightSet;


//-------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 */
//-------------------------------------------------------------------------------------------
void DEBUG_OBATA_DEMO_TEST_Init( HEAPID heap_id )
{
	//VRAM�ݒ�
	GFL_DISP_SetBank(&sc_VRAM_param );

  // BGL������
	GFL_BG_Init( heap_id );	// �V�X�e��������
	GFL_BG_SetBGMode( &sc_BG_mode );	  // BG���[�h�ݒ�
	GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &sc_BGCNT1_M, GFL_BG_MODE_TEXT );	// MAIN_BG1�̐ݒ� 
	//GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &sc_BGCNT1_S, GFL_BG_MODE_TEXT );	// SUB_BG1�̐ݒ� 
	GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );						// MAIN_BG0��\��ON
	//GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );						// MAIN_BG1��\��ON
	GFL_BG_SetBGControl3D( PRIORITY_MAIN_BG0 );		              // 3D�ʂ̕\���D�揇�ʂ�ݒ�

  // 3D�V�X�e����������
  GFL_G3D_Init( 
      GFL_G3D_VMANLNK, GFL_G3D_TEX128K, 
      GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0x1000, heap_id, NULL );

  // ���C�g�쐬
  g_lightSet = GFL_G3D_LIGHT_Create( &light_setup, heap_id );
  GFL_G3D_LIGHT_Switching( g_lightSet );

  // �J���������ݒ�
  {
    GFL_G3D_PROJECTION proj = { GFL_G3D_PRJPERS, 0, 0, cameraAspect, 0, cameraNear, cameraFar, 0 }; 
    proj.param1 = FX_SinIdx( cameraPerspway ); 
    proj.param2 = FX_CosIdx( cameraPerspway ); 
    GFL_G3D_SetSystemProjection( &proj );	
  }
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �I������
 */
//-------------------------------------------------------------------------------------------
void DEBUG_OBATA_DEMO_TEST_Exit()
{
  // ���C�g�Z�b�g�j��
  GFL_G3D_LIGHT_Delete( g_lightSet );

  // 3D�V�X�e���I��
  GFL_G3D_Exit();

  // BGL�j��
  GFL_BG_Exit();
}
