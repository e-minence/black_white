#include <gflib.h>
#include "debug_obata_menu_setup.h"


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

// �p���b�g�f�[�^
static const u16 palette[] = 
{
  0x0000,
  0xffff,
  0x7c00,
  0x001f,
};

//-------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 */
//-------------------------------------------------------------------------------------------
void DEBUG_OBATA_MENU_SETUP_Init( HEAPID heap_id )
{
	//VRAM�ݒ�
	GFL_DISP_SetBank( &sc_VRAM_param );

  // BGL������
	GFL_BG_Init( heap_id );	// �V�X�e��������
	GFL_BG_SetBGMode( &sc_BG_mode );	  // BG���[�h�ݒ�
	GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &sc_BGCNT1_M, GFL_BG_MODE_TEXT );	// MAIN_BG1�̐ݒ� 
	GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );						// MAIN_BG1��\��ON

  // �p���b�g�]��
  GFL_BG_LoadPalette( GFL_BG_FRAME1_M, (void*)palette, NELEMS(palette)*sizeof(u16), 0 );

  // �r�b�g�}�b�v�Ǘ��V�X�e���̏�����
  GFL_BMPWIN_Init( heap_id );
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �I������
 */
//-------------------------------------------------------------------------------------------
void DEBUG_OBATA_MENU_SETUP_Exit()
{ 
  // BGL�j��
  GFL_BG_Exit();

  // �r�b�g�}�b�v�Ǘ��V�X�e���̏I��
  GFL_BMPWIN_Exit();
}
