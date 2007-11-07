//=============================================================================================
/**
 * @file	display.h                                                  
 * @brief	�\���V�X�e���ݒ�
 */
//=============================================================================================

#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <nitro.h>
#include <nnsys.h>

//�a�f�E�n�a�i�\���p��`
#define	VISIBLE_OFF		(0)		// ��\����
#define	VISIBLE_ON		(1)		// �\����


//GFL_DISP_DispSelect( int value )�p��`
#define	GFL_DISP_3D_TO_MAIN	(GX_DISP_SELECT_MAIN_SUB)	//���ʂ����C��
#define	GFL_DISP_3D_TO_SUB	(GX_DISP_SELECT_SUB_MAIN)	//����ʂ����C��

//=============================================================================================
//	�^�錾
//=============================================================================================
///�u�q�`�l�o���N�ݒ�\����
typedef	struct	{
	GXVRamBG			main_bg;			//���C��2D�G���W����BG�Ɋ��蓖�� 
	GXVRamBGExtPltt		main_bg_expltt;		//���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GXVRamSubBG			sub_bg;				//�T�u2D�G���W����BG�Ɋ��蓖��
	GXVRamSubBGExtPltt	sub_bg_expltt;		//�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GXVRamOBJ			main_obj;			//���C��2D�G���W����OBJ�Ɋ��蓖��
	GXVRamOBJExtPltt	main_obj_expltt;	//���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GXVRamSubOBJ		sub_obj;			//�T�u2D�G���W����OBJ�Ɋ��蓖��
	GXVRamSubOBJExtPltt	sub_obj_expltt;		//�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GXVRamTex			teximage;			//�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GXVRamTexPltt		texpltt;			//�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
}GFL_BG_DISPVRAM;

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
extern	void	GFL_DISP_ClearVRAM( u16	withoutVRAM );

//--------------------------------------------------------------------------------------------
/**
 * �u�q�`�l�o���N�S�̐ݒ�
 *
 * @param	data	�ݒ�f�[�^
 */
//--------------------------------------------------------------------------------------------
extern void	GFL_DISP_SetBank( const GFL_BG_DISPVRAM* dispvram );

//--------------------------------------------------------------------------------------------
/**
 * ���C����ʂ̊e�ʂ̕\���R���g���[��������
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_DISP_GX_InitVisibleControl( void );
extern void GFL_DISP_GXS_InitVisibleControl( void );

//--------------------------------------------------------------------------------------------
/**
 * ���C�����T�u��ʂ̊e�ʂ̕\���R���g���[��
 *
 * @param	gxplanemask		�ݒ��
 * @param	flg				�R���g���[���t���O
 *
 * @li	flg = VISIBLE_ON	: �\��
 * @li	flg = VISIBLE_OFF	: ��\��
 */
//--------------------------------------------------------------------------------------------
extern void GFL_DISP_GX_SetVisibleControl( u8 gxplanemask, u8 flg );
extern void GFL_DISP_GXS_SetVisibleControl( u8 gxplanemask, u8 flg );

//--------------------------------------------------------------------------------------------
/**
 * ���C�����T�u��ʂ̊e�ʂ̕\���R���g���[���i���A�Ƃ��j
 *
 * @param	prm		�\���p�����[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void GFL_DISP_GX_SetVisibleControlDirect( int prm );
extern void GFL_DISP_GXS_SetVisibleControlDirect( int prm );

//--------------------------------------------------------------------------------------------
/**
 * �\���L���ݒ�
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void	GFL_DISP_SetDispOn( void );

//--------------------------------------------------------------------------------------------
/**
 * �\�������ݒ�
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void	GFL_DISP_SetDispOff( void );

//--------------------------------------------------------------------------------------------
/**
 * ��ʂ̕\���T�C�h�ݒ�
 *
 * @param	value	�㉺�ǂ�������C����ʂɂ��邩�iDISP_3D_TO_MAIN:���ʂ����C���@DISP_3D_TO_SUB:����ʂ����C���j
 */
//--------------------------------------------------------------------------------------------
extern void GFL_DISP_SetDispSelect( int value );

//--------------------------------------------------------------------------------------------
/**
 * ���C����ʂ̕\���󋵎擾
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern int GFL_DISP_GetMainVisible(void);

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʂ̕\���󋵎擾
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern int GFL_DISP_GetSubVisible(void);

#endif __DISPLAY_H_
