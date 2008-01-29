//=============================================================================================
/**
 * @file	display.c                                                  
 * @brief	�\���V�X�e���ݒ�
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�v���g�^�C�v�錾
//=============================================================================================
//=============================================================================================
//	���[�J���ϐ��錾
//=============================================================================================
static	int				MainVisiblePlane;
static	int				SubVisiblePlane;

//=============================================================================================
//	�ݒ�֐�
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �`��ɂ������VRAM�̃I�[���N���A
 *
 * @brief	������VRAM���蓖�Ă�LCDC�ɕύX���ăN���A����̂ŁA
 *			���̊֐����Ă񂾌�ɁAGFL_DISP_SetBank��VRAM�̍Ċ��蓖�Ă������Ȃ��K�v������܂�
 *
 * @param	withoutVRAM	��N���A�ɂ�����VRAM���w��iNULL�Ŏw��Ȃ��AGX_VRAM_A�`GX_VRAM_I�A������OR�Ŏw��\�j
 */
//--------------------------------------------------------------------------------------------
void	GFL_DISP_ClearVRAM( u16	withoutVRAM )
{
	//VRAM�N���A
	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL&(withoutVRAM^0xffff));
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	(void)GX_DisableBankForLCDC();
	MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);			// clear OAM
	MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);				// clear the standard palette
	MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);		// clear OAM
	MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);		// clear the standard palette
}

//--------------------------------------------------------------------------------------------
/**
 * �u�q�`�l�o���N�S�̐ݒ�
 *
 * @param	data	�ݒ�f�[�^
 */
//--------------------------------------------------------------------------------------------
void	GFL_DISP_SetBank( const GFL_BG_DISPVRAM* dispvram )
{
	// VRAM�ݒ胊�Z�b�g
	GX_ResetBankForBG();			// ���C��2D�G���W����BG
	GX_ResetBankForBGExtPltt();		// ���C��2D�G���W����BG�g���p���b�g
	GX_ResetBankForSubBG();			// �T�u2D�G���W����BG
	GX_ResetBankForSubBGExtPltt();	// �T�u2D�G���W����BG�g���p���b�g
	GX_ResetBankForOBJ();			// ���C��2D�G���W����OBJ
	GX_ResetBankForOBJExtPltt();	// ���C��2D�G���W����OBJ�g���p���b�g
	GX_ResetBankForSubOBJ();		// �T�u2D�G���W����OBJ
	GX_ResetBankForSubOBJExtPltt();	// �T�u2D�G���W����OBJ�g���p���b�g
	GX_ResetBankForTex();			// �e�N�X�`���C���[�W
	GX_ResetBankForTexPltt();		// �e�N�X�`���p���b�g

	GX_SetBankForBG( dispvram->main_bg );
	GX_SetBankForBGExtPltt( dispvram->main_bg_expltt );

	GX_SetBankForSubBG( dispvram->sub_bg );
	GX_SetBankForSubBGExtPltt( dispvram->sub_bg_expltt );

	GX_SetBankForOBJ( dispvram->main_obj );
	GX_SetBankForOBJExtPltt( dispvram->main_obj_expltt );

	GX_SetBankForSubOBJ( dispvram->sub_obj );
	GX_SetBankForSubOBJExtPltt( dispvram->sub_obj_expltt );

	GX_SetBankForTex( dispvram->teximage );
	GX_SetBankForTexPltt( dispvram->texpltt );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C����ʂ̊e�ʂ̕\���R���g���[���������i�a�f�̂݁j
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GX_InitVisibleControlBG( void )
{
	MainVisiblePlane &= GX_PLANEMASK_OBJ ;
	//GX_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C����ʂ̊e�ʂ̕\���R���g���[���������i�n�a�i�̂݁j
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GX_InitVisibleControlOBJ( void )
{
	MainVisiblePlane &= ( GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3 );
	//GX_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C����ʂ̊e�ʂ̕\���R���g���[��������
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GX_InitVisibleControl( void )
{
	GFL_DISP_GX_InitVisibleControlBG();
	GFL_DISP_GX_InitVisibleControlOBJ();
}

//--------------------------------------------------------------------------------------------
/**
 * ���C����ʂ̊e�ʂ̕\���R���g���[��
 *
 * @param	gxplanemask		�ݒ��
 * @param	flg				�R���g���[���t���O
 *
 * @li	flg = VISIBLE_ON	: �\��
 * @li	flg = VISIBLE_OFF	: ��\��
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GX_SetVisibleControl( u8 gxplanemask, u8 flg )
{
	if( flg == VISIBLE_ON ){
		if( MainVisiblePlane & gxplanemask ){ return; }
	}else{
		if( !(MainVisiblePlane & gxplanemask) ){ return; }
	}
	MainVisiblePlane ^= gxplanemask;
	GX_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C����ʂ̊e�ʂ̕\���R���g���[���i���A�Ƃ��j
 *
 * @param	prm		�\���p�����[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GX_SetVisibleControlDirect( int prm )
{
	MainVisiblePlane = prm;
	GX_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʂ̊e�ʂ̕\���R���g���[���������i�a�f�̂݁j
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GXS_InitVisibleControlBG( void )
{
	SubVisiblePlane &= GX_PLANEMASK_OBJ ;
	//GXS_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʂ̊e�ʂ̕\���R���g���[��������
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GXS_InitVisibleControlOBJ( void )
{
	SubVisiblePlane &= ( GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3 );
	//GXS_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʂ̊e�ʂ̕\���R���g���[���������i�n�a�i�̂݁j
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GXS_InitVisibleControl( void )
{
	GFL_DISP_GXS_InitVisibleControlBG();
	GFL_DISP_GXS_InitVisibleControlOBJ();
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʂ̊e�ʂ̕\���R���g���[��
 *
 * @param	gxplanemask		�ݒ��
 * @param	flg				�R���g���[���t���O
 *
 * @li	flg = VISIBLE_ON	: �\��
 * @li	flg = VISIBLE_OFF	: ��\��
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GXS_SetVisibleControl( u8 gxplanemask, u8 flg )
{
	if( flg == VISIBLE_ON ){
		if( SubVisiblePlane & gxplanemask ){ return; }
	}else{
		if( !(SubVisiblePlane & gxplanemask) ){ return; }
	}
	SubVisiblePlane ^= gxplanemask;
	GXS_SetVisiblePlane( SubVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʂ̊e�ʂ̕\���R���g���[���i���A�Ƃ��j
 *
 * @param	prm		�\���p�����[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_GXS_SetVisibleControlDirect( int prm )
{
	SubVisiblePlane = prm;
	GXS_SetVisiblePlane( SubVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * �\���L���ݒ�
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void	GFL_DISP_SetDispOn( void )
{
	GX_DispOn();
	GXS_DispOn();
}

//--------------------------------------------------------------------------------------------
/**
 * �\�������ݒ�
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void	GFL_DISP_SetDispOff( void )
{
	GX_DispOff();
	GXS_DispOff();
}


//--------------------------------------------------------------------------------------------
/**
 * ��ʂ̕\���T�C�h�ݒ�
 *
 * @param	value	�㉺�ǂ�������C����ʂɂ��邩�iGFL_DISP_3D_TO_MAIN:���ʂ����C���@GFL_DISP_3D_TO_SUB:����ʂ����C���j
 */
//--------------------------------------------------------------------------------------------
void GFL_DISP_SetDispSelect( int value )
{
	GX_SetDispSelect(value);
}


//--------------------------------------------------------------------------------------------
/**
 * ���C����ʂ̕\���󋵎擾
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
int GFL_DISP_GetMainVisible(void)
{
	return MainVisiblePlane;
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʂ̕\���󋵎擾
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
int GFL_DISP_GetSubVisible(void)
{
	return SubVisiblePlane;
}
