//==============================================================================
/**
 * @file  p_sta_oam.c
 * @brief BMP��OAM�ɕ`��(�|�P�����X�e�[�^�X�p�J�X�^��
 * @author  ariizumi
 * @date  2009.07.08
 */
//==============================================================================
#pragma once

//==============================================================================
//	�\���̒�`
//==============================================================================
///BMPOAM�V�X�e���̕s��`�|�C���^
typedef struct _PSTA_OAM_SYS * PSTA_OAM_SYS_PTR;

///BMPOAM�A�N�^�[�̕s��`�|�C���^
typedef struct _PSTA_OAM_ACT * PSTA_OAM_ACT_PTR;

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
}PSTA_OAM_ACT_DATA;


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern PSTA_OAM_SYS_PTR PSTA_OAM_Init(HEAPID heap_id, GFL_CLUNIT* p_unit);
extern void PSTA_OAM_Exit(PSTA_OAM_SYS_PTR bsp);
extern PSTA_OAM_ACT_PTR PSTA_OAM_ActorAdd(PSTA_OAM_SYS_PTR bsp, const PSTA_OAM_ACT_DATA *head);
extern void PSTA_OAM_ActorDel(PSTA_OAM_ACT_PTR bact);
extern void PSTA_OAM_ActorSetDrawEnable(PSTA_OAM_ACT_PTR bact, BOOL on_off);
extern BOOL PSTA_OAM_ActorGetDrawEnable(PSTA_OAM_ACT_PTR bact);
extern void PSTA_OAM_ActorBmpTrans(PSTA_OAM_ACT_PTR bact);
extern void PSTA_OAM_ActorGetPos(PSTA_OAM_ACT_PTR bact, s16 *x, s16 *y);
extern void PSTA_OAM_ActorSetPos(PSTA_OAM_ACT_PTR bact, s16 x, s16 y);

//bmp �̓���ւ�(�����T�C�Y��bmp����I�I����ւ���vTrans
extern void PSTA_OAM_SwapBmp(PSTA_OAM_ACT_PTR act1, PSTA_OAM_ACT_PTR act2);

