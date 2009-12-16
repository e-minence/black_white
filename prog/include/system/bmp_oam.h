//==============================================================================
/**
 * @file	bmp_oam.h
 * @brief	BMPOAM�̃w�b�_
 * @author	matsuda
 * @date	2009.02.26(��)
 */
//==============================================================================
#ifndef __BMP_OAM_H__
#define __BMP_OAM_H__

//==============================================================================
//	�\���̒�`
//==============================================================================
///BMPOAM�V�X�e���̕s��`�|�C���^
typedef struct _BMPOAM_SYS * BMPOAM_SYS_PTR;

///BMPOAM�A�N�^�[�̕s��`�|�C���^
typedef struct _BMPOAM_ACT * BMPOAM_ACT_PTR;

///BMPOAM�A�N�^�[�w�b�_�f�[�^�\����
typedef struct{
	GFL_BMP_DATA *bmp;	///<OAM�Ƃ��ĕ\��������BMP�f�[�^�ւ̃|�C���^
	s16 x;				///<����X���W
	s16 y;				///<����Y���W
	u32 pltt_index;		///<�K�p����p���b�g��index(GFL_CLGRP_PLTT_Register�̖߂�l)
	u32 pal_offset;		///<pltt_index�̃p���b�g���ł̃I�t�Z�b�g
	u8 soft_pri;		///<�\�t�g�v���C�I���e�B
	u8 bg_pri;			///<BG�v���C�I���e�B
	u16 setSerface;		///<CLSYS_DEFREND_MAIN,SUB or CLWK_SETSF_NONE
	CLSYS_DRAW_TYPE draw_type;
}BMPOAM_ACT_DATA;


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern BMPOAM_SYS_PTR BmpOam_Init(HEAPID heap_id, GFL_CLUNIT* p_unit);
extern void BmpOam_Exit(BMPOAM_SYS_PTR bsp);
extern BMPOAM_ACT_PTR BmpOam_ActorAdd(BMPOAM_SYS_PTR bsp, const BMPOAM_ACT_DATA *head);
extern void BmpOam_ActorDel(BMPOAM_ACT_PTR bact);
extern void BmpOam_ActorSetDrawEnable(BMPOAM_ACT_PTR bact, BOOL on_off);
extern BOOL BmpOam_ActorGetDrawEnable(BMPOAM_ACT_PTR bact);
extern void BmpOam_ActorBmpTrans(BMPOAM_ACT_PTR bact);
extern void BmpOam_ActorGetPos(BMPOAM_ACT_PTR bact, s16 *x, s16 *y);
extern void BmpOam_ActorSetPos(BMPOAM_ACT_PTR bact, s16 x, s16 y);
extern void BmpOam_ActorSetObjMode( BMPOAM_ACT_PTR bact, GXOamMode mode );


#endif	//__BMP_OAM_H__
