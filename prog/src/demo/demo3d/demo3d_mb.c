//=============================================================================
/**
 *
 *	@file		demo3d_mb.c
 *	@brief  ���[�V�����u���[
 *	@author	hosaka genya(HGSS����nakamura hiroyuki����̃\�[�X���ڐA)
 *	@data		2010.01.19
 *
 */
//=============================================================================
#include <gflib.h>
#include <tcbl.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "demo3d_mb.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	���[�V�����u���[�p���[�N
//==============================================================

// ���[�V�����u���[�������p�����[�^
typedef struct {
	// �O���t�B�b�N���[�h
	GXDispMode dispMode;	// �f�B�X�v���C���[�h
	GXBGMode bgMode;		// BG���[�h	
	GXBG0As bg0_2d3d;		// BG0��3d�Ɏg�p���邩

	// �L���v�`��
	GXCaptureSize sz;		// �L���v�`���T�C�Y
    GXCaptureMode mode;		// �L���v�`�����[�h
    GXCaptureSrcA a;		// �L���v�`����A
    GXCaptureSrcB b;		// �L���v�`����B
    GXCaptureDest dest;		// �L���v�`���f�[�^�]��Vram
    int eva;				// �u�����h�W��A
    int evb;				// �u�����h�W��B

	int heap_id;			// �g�p����q�[�vID
} DEMO3D_MBL_PARAM;

//--------------------------------------------------------------
///	���[�V�����u���[�^�X�N���[�N
//==============================================================
struct _DEMO3D_MBL_WORK {
	GXVRamLCDC			lcdc;		// ����LCDC
	DEMO3D_MBL_PARAM	data;		// ���[�V�����u���[�������p�����[�^
	BOOL				init_flg;
	GFL_TCBL*			tcb;	
};//DEMO3D_MBL_WORK;

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static DEMO3D_MBL_WORK * MotionBlInit( GFL_TCBLSYS* tcbsys, DEMO3D_MBL_PARAM * prm );
static void MotionBlDelete( DEMO3D_MBL_WORK * mb, GXDispMode dispMode, GXBGMode bgMode, GXBG0As bg0_2d3d );
static void MotionBlTask( GFL_TCBL* tcb, void * work );
static void VBlankLCDCChange( GFL_TCB* tcb, void* work );
static void MotionBlCapture( DEMO3D_MBL_PARAM * p_data );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//--------------------------------------------------------------------------------------------
/**
 * ���[�V�����u���[������
 *
 * @param	eva		�u�����h�W��A
 * @param	evb		�u�����h�W��B
 *
 * @return	FLD_MOTION_BL_PTR	���[�V�����u���[�|�C���^
 */
//--------------------------------------------------------------------------------------------
DEMO3D_MBL_WORK * DEMO3D_MotionBlInit( GFL_TCBLSYS* tcbsys, int eva, int evb )
{
	DEMO3D_MBL_WORK * wk;

//	MI_CpuClear32(ret, sizeof(FLD_MOTION_BL_DATA));

	// BG�ʂ̕`���p�~
  GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_BG1,VISIBLE_OFF);
  GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_BG2,VISIBLE_OFF);
  GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_BG3,VISIBLE_OFF);
	GX_ResetBankForBG();
	
	// ���[�V�����u���[�ݒ�
	{
		DEMO3D_MBL_PARAM mb = {
			GX_DISPMODE_VRAM_C,
			GX_BGMODE_0,
			GX_BG0_AS_3D,
			
			GX_CAPTURE_SIZE_256x192,
			GX_CAPTURE_MODE_AB,
			GX_CAPTURE_SRCA_2D3D,
			GX_CAPTURE_SRCB_VRAM_0x00000,
			GX_CAPTURE_DEST_VRAM_C_0x00000,
			0,
			0,
			HEAPID_DEMO3D
		};
		mb.eva = eva;
		mb.evb = evb;

		wk = MotionBlInit( tcbsys, &mb );
	}	

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * ���[�V�����u���[�폜
 *
 * @param	mb		���[�V�����u���[�|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DEMO3D_MotionBlExit( DEMO3D_MBL_WORK * mb )
{
	// ���ɖ߂�
	MotionBlDelete( mb, GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D );

	GX_SetBankForBG(GX_VRAM_BG_128_C);

	// BG�ʕ`��
  GFL_DISP_GX_SetVisibleControl(
//	GF_Disp_GX_VisibleControl(
			GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3,
			VISIBLE_ON );
}



//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//============================================================================================
//	���[�V�����u���[
//============================================================================================

//----------------------------------------------------------------------------
/**
 * ���[�V�����u���[������
 *
 * @param	prm		�������f�[�^
 *
 *@return	���[�V�����u���[���[�N
 */
//-----------------------------------------------------------------------------
static DEMO3D_MBL_WORK * MotionBlInit( GFL_TCBLSYS* tcbsys, DEMO3D_MBL_PARAM * prm )
{
	DEMO3D_MBL_WORK * mb;
	GFL_TCBL* task;
	
	// ���[�V�����u���[�^�X�N���Z�b�g
  task = GFL_TCBL_Create( tcbsys, MotionBlTask, sizeof(DEMO3D_MBL_WORK), 5 );

	mb = GFL_TCBL_GetWork( task );
	mb->data = *prm;
	mb->tcb = task;
	mb->init_flg = FALSE;

	mb->lcdc = GX_GetBankForLCDC();

	MotionBlCapture( &mb->data );

	// LCDC�`�F���W
  GFUser_VIntr_CreateTCB( VBlankLCDCChange, mb, 0 );

	return mb;
}

//----------------------------------------------------------------------------
/**
 * ���[�V�����u���[�폜
 *
 * @param	data		���[�V�����u���[�I�u�W�F
 * @param	dispMode	������̃f�B�X�v���C���[�h
 * @param	bgMode		�������BG���[�h
 * @param	bg0_2d3d	�������BG�O���RD�Ɏg�p���邩
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
static void MotionBlDelete( DEMO3D_MBL_WORK * mb, GXDispMode dispMode, GXBGMode bgMode, GXBG0As bg0_2d3d )
{
	// �O���t�B�b�N���[�h��߂�
	GX_SetGraphicsMode(dispMode, bgMode,bg0_2d3d);

	GX_SetBankForLCDC( mb->lcdc);
	
	switch( mb->data.dispMode ){
	case GX_DISPMODE_VRAM_A:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_A, HW_VRAM_A_SIZE );
		break;
	case GX_DISPMODE_VRAM_B:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_B, HW_VRAM_B_SIZE );
		break;
	case GX_DISPMODE_VRAM_C:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_C, HW_VRAM_C_SIZE );
		break;
	case GX_DISPMODE_VRAM_D:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE );
		break;
	default:
		GX_SetBankForLCDC(GX_VRAM_LCDC_NONE);
		break;
	}

	// �^�X�N�j��
	GFL_TCBL_Delete( mb->tcb );

  // ���[�N�j��
  GFL_HEAP_FreeMemory( mb );
}

//----------------------------------------------------------------------------
/**
 * ���[�V�����u���[�@�L���v�`���^�X�N
 *
 * @param	tcb		�^�X�N�|�C���^
 * @param	work	���[�V�����u���[�f�[�^
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
static void MotionBlTask( GFL_TCBL* tcb, void * work )
{
	DEMO3D_MBL_WORK * mb = work;
	
	//�����������҂�
	if( mb->init_flg ){
		GX_SetCapture(
				mb->data.sz,			// �L���v�`���T�C�Y
				mb->data.mode,			// �L���v�`�����[�h
				mb->data.a,				// �L���v�`���u�����hA
				mb->data.b,				// �L���v�`���u�����hB
				mb->data.dest,			// �]��Vram
				mb->data.eva,			// �u�����h�W��A
				mb->data.evb);			// �u�����h�W��B
	}
}

//----------------------------------------------------------------------------
/**
 * LCDC�̏�Ԃ�ݒ肷��^�X�N
 *
 * @param	tcb		�^�X�N�|�C���^
 * @param	work	�ݒ肷��l�������Ă���
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
static void VBlankLCDCChange( GFL_TCB* tcb, void* work )
{
	DEMO3D_MBL_WORK * mb = work;

	// �`��Vram�ݒ�
	switch(mb->data.dispMode){
	case GX_DISPMODE_VRAM_A:
		GX_SetBankForLCDC(GX_VRAM_LCDC_A);
		break;
	case GX_DISPMODE_VRAM_B:
		GX_SetBankForLCDC(GX_VRAM_LCDC_B);
		break;
	case GX_DISPMODE_VRAM_C:
		GX_SetBankForLCDC(GX_VRAM_LCDC_C);
		break;
	case GX_DISPMODE_VRAM_D:
		GX_SetBankForLCDC(GX_VRAM_LCDC_D);
		break;
	default:
		GX_SetBankForLCDC(GX_VRAM_LCDC_NONE);
		break;
	}

	// �O���t�B�b�N���[�h�ݒ�
	GX_SetGraphicsMode(mb->data.dispMode, mb->data.bgMode,mb->data.bg0_2d3d);	

	// ����������
	mb->init_flg = TRUE;

	// �^�X�N�j��
  GFL_TCB_DeleteTask( tcb );
}

//----------------------------------------------------------------------------
/**
 * Capture�֐�
 *
 * @param	p_data 
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
static void MotionBlCapture( DEMO3D_MBL_PARAM * p_data )
{
	// �`��Vram������
	switch(p_data->dispMode){
	case GX_DISPMODE_VRAM_A:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_A, HW_VRAM_A_SIZE );
		break;
	case GX_DISPMODE_VRAM_B:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_B, HW_VRAM_B_SIZE );
		break;
	case GX_DISPMODE_VRAM_C:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_C, HW_VRAM_C_SIZE );
		break;
	case GX_DISPMODE_VRAM_D:
		MI_CpuClearFast( (void*)HW_LCDC_VRAM_D, HW_VRAM_D_SIZE );
		break;
	default:
		break;
	}

	GX_SetCapture(
			p_data->sz,			// �L���v�`���T�C�Y
			p_data->mode,			// �L���v�`�����[�h
			p_data->a,				// �L���v�`���u�����hA
			p_data->b,				// �L���v�`���u�����hB
			p_data->dest,			// �]��Vram
			16,						// �u�����h�W��A
			0);						// �u�����h�W��B	
}

