//==============================================================================
/**
 * @file	bmp_oam.c
 * @brief	BMP��OAM�ɕ`��
 * @author	matsuda
 * @date	2009.02.25(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/bmp_oam.h"
#include "bmpoam.naix"
#include "arc_def.h"


//==============================================================================
//	�萔��`
//==============================================================================
///�_�~�[�Z���̃T�C�YX(dot�P��)
#define DUMMY_CELL_SIZE_X		(32)
///�_�~�[�Z���̃T�C�YY(dot�P��)
#define DUMMY_CELL_SIZE_Y		(16)
///�_�~�[�Z���̍��v�f�[�^�T�C�Y(byte�P��)
#define DUMMY_CELL_SIZE_TOTAL		((DUMMY_CELL_SIZE_X/8) * (DUMMY_CELL_SIZE_Y/8) * 0x20)

///1�̃t�H���gOAM�ŊǗ��o����ő�A�N�^�[��
#define FACT_ONE_ACT_NUM			(8)


//==============================================================================
//	�\���̒�`
//==============================================================================
///BMPOAM�A�N�^�[�\����
typedef struct _BMPOAM_ACT{
	GFL_BMP_DATA *bmp;
	GFL_CLWK *cap[FACT_ONE_ACT_NUM];
	CLSYS_DRAW_TYPE draw_type;
	u32 cgr_id[FACT_ONE_ACT_NUM];
	u16 setSerface;		///<CLSYS_DEFREND_MAIN,SUB or CLWK_SETSF_NONE
	u8 act_num_x;
	u8 act_num_y;
}BMPOAM_ACT;

///BMPOAM�Ǘ��\����
typedef struct _BMPOAM_SYS{
	GFL_CLUNIT *clunit;
	HEAPID heap_id;
	u32 cell_id;			///<���ʃZ���̓o�^index
}BMPOAM_SYS;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void _FOLocal_CommonResourceSet(BMPOAM_SYS_PTR bsp);
static void _FOLocal_CommonResourceFree(BMPOAM_SYS_PTR bsp);


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   BMPOAM�V�X�e���쐬
 *
 * @param   heap_id		BMPOAM�ŏ�Ɏg�p����q�[�vID
 * @param   p_unit		CLUNIT�ւ̃|�C���^
 *
 * @retval  �쐬����BMPOAM�V�X�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
BMPOAM_SYS_PTR BmpOam_Init(HEAPID heap_id, GFL_CLUNIT* p_unit)
{
	BMPOAM_SYS_PTR bsp;
	
	bsp = GFL_HEAP_AllocClearMemory(heap_id, sizeof(BMPOAM_SYS));
	bsp->heap_id = heap_id;
	bsp->clunit = p_unit;
	
	//���ʃ��\�[�X�o�^
	_FOLocal_CommonResourceSet(bsp);
	
	return bsp;
}

//--------------------------------------------------------------
/**
 * @brief   BMPOAM�V�X�e���j��
 *
 * @param   bsp		BMPOAM�V�X�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
void BmpOam_Exit(BMPOAM_SYS_PTR bsp)
{
	_FOLocal_CommonResourceFree(bsp);
	GFL_HEAP_FreeMemory(bsp);
}

//--------------------------------------------------------------
/**
 * @brief   BMPOAM�A�N�^�[�쐬
 *
 * @param   bsp			BMPOAM�V�X�e���ւ̃|�C���^
 * @param   head		BMPOAM�A�N�^�[�w�b�_�ւ̃|�C���^
 *
 * @retval  �쐬����BMPOAM�A�N�^�[�ւ̃|�C���^
 */
//--------------------------------------------------------------
BMPOAM_ACT_PTR BmpOam_ActorAdd(BMPOAM_SYS_PTR bsp, const BMPOAM_ACT_DATA *head)
{
	BMPOAM_ACT_PTR bact;
	u8 num_x, num_y, x, y;
	ARCHANDLE *hdl;
	GFL_CLWK_DATA clwkdata = {
		0, 0,		//pos_x, pos_y
		0,			//anmseq
		0, 0,		//softpri, bgpri
	};
	
	bact = GFL_HEAP_AllocClearMemory(bsp->heap_id, sizeof(BMPOAM_ACT));
	bact->bmp = head->bmp;
	
	num_x = GFL_BMP_GetSizeX(head->bmp) / DUMMY_CELL_SIZE_X;
	num_y = GFL_BMP_GetSizeY(head->bmp) / DUMMY_CELL_SIZE_Y;
	if(GFL_BMP_GetSizeX(head->bmp) % DUMMY_CELL_SIZE_X != 0){
		num_x++;
	}
	if(GFL_BMP_GetSizeY(head->bmp) % DUMMY_CELL_SIZE_Y != 0){
		num_y++;
	}
	//����ASSERT�Ɉ�������������FACT_ONE_ACT_NUM�̐�����傫�����ĊǗ��o����
	//�A�N�^�[���𑝂₷�K�v������
	GF_ASSERT(num_y*num_x <= FACT_ONE_ACT_NUM);

	bact->act_num_x = num_x;
	bact->act_num_y = num_y;
	bact->setSerface = head->setSerface;
	
	//�L�����o�^
	hdl = GFL_ARC_OpenDataHandle(ARCID_BMPOAM, bsp->heap_id);
	for(y = 0; y < num_y; y++){
		for(x = 0; x < num_x; x++){
			//�L�����o�^(�̈�m��)
			bact->cgr_id[x + y*num_x] = GFL_CLGRP_CGR_Register(hdl, 
				NARC_bmpoam_bmpoam_dummy32x16_NCGR, FALSE, head->draw_type, bsp->heap_id);
			//�A�N�^�[����
			clwkdata.pos_x = head->x + x * DUMMY_CELL_SIZE_X;
			clwkdata.pos_y = head->y + y * DUMMY_CELL_SIZE_Y;
			clwkdata.softpri = head->soft_pri;
			clwkdata.bgpri = head->bg_pri;
			bact->cap[x + y*num_x] = GFL_CLACT_WK_Create(bsp->clunit, 
				bact->cgr_id[x + y*num_x], head->pltt_index, bsp->cell_id, &clwkdata, 
				head->setSerface, bsp->heap_id);
			GFL_CLACT_WK_SetPlttOffs(bact->cap[x], head->pal_offset, CLWK_PLTTOFFS_MODE_OAM_COLOR);
		}
	}
	GFL_ARC_CloseDataHandle(hdl);
	
	return bact;
}

//--------------------------------------------------------------
/**
 * @brief   BMPOAM�A�N�^�[�̍폜
 *
 * @param   bact		BMPOAM�A�N�^�[�ւ̃|�C���^
 */
//--------------------------------------------------------------
void BmpOam_ActorDel(BMPOAM_ACT_PTR bact)
{
	u8 x, y;
	
	for(y = 0; y < bact->act_num_y; y++){
		for(x = 0; x < bact->act_num_x; x++){
			GFL_CLGRP_CGR_Release(bact->cgr_id[x + y*bact->act_num_x]);
			GFL_CLACT_WK_Remove(bact->cap[x + y*bact->act_num_x]);
		}
	}
	GFL_HEAP_FreeMemory(bact);
}

//--------------------------------------------------------------
/**
 * @brief   BMPOAM�A�N�^�[�̕\���E��\���t���O�ݒ�
 *
 * @param   bact		BMPOAM�A�N�^�[�ւ̃|�C���^
 * @param   on_off		TRUE:�\���@FALSE:��\��
 */
//--------------------------------------------------------------
void BmpOam_ActorSetDrawEnable(BMPOAM_ACT_PTR bact, BOOL on_off)
{
	u8 x, y;
	
	for(y = 0; y < bact->act_num_y; y++){
		for(x = 0; x < bact->act_num_x; x++){
			GFL_CLACT_WK_SetDrawEnable(bact->cap[x + y*bact->act_num_x], on_off);
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   BMPOAM�A�N�^�[�̕\���E��\���t���O�擾
 *
 * @param   bact		BMPOAM�A�N�^�[�ւ̃|�C���^
 * 
 * @retval   TRUE:�\���@FALSE:��\��
 */
//--------------------------------------------------------------
BOOL BmpOam_ActorGetDrawEnable(BMPOAM_ACT_PTR bact)
{
	return GFL_CLACT_WK_GetDrawEnable( bact->cap[0] );
}

//--------------------------------------------------------------
/**
 * @brief   BMPOAM�A�N�^�[�Ɋ֘A�t������Ă���BMP��VRAM�]��
 *
 * @param   bact		BMPOAM�A�N�^�[�ւ̃|�C���^
 */
//--------------------------------------------------------------
void BmpOam_ActorBmpTrans(BMPOAM_ACT_PTR bact)
{
	u8 *bmp_adrs;
	u32 total_size;
	void (*load_func)(const void *pSrc, u32 offset, u32 szByte);
	u32 dest_adrs;
	int x, y, bmp_char_x, bmp_char_y;
	u32 src_u, src_d, trans_size, last_trans_size;
	
	bmp_adrs = GFL_BMP_GetCharacterAdrs(bact->bmp);
	total_size = GFL_BMP_GetBmpDataSize(bact->bmp);
	
	DC_FlushRange(bmp_adrs, total_size);
	if(bact->draw_type == CLSYS_DRAW_MAIN){
		load_func = GX_LoadOBJ;
	}
	else{
		load_func = GXS_LoadOBJ;
	}

	bmp_char_x = GFL_BMP_GetSizeX(bact->bmp) / 8;
	bmp_char_y = GFL_BMP_GetSizeY(bact->bmp) / 8;
	last_trans_size = (GFL_BMP_GetSizeX(bact->bmp) % DUMMY_CELL_SIZE_X) / 8 * 0x20;
	if(last_trans_size == 0){
		last_trans_size = DUMMY_CELL_SIZE_X / 8 * 0x20;
	}
	
	src_u = 0;
	src_d = bmp_char_x * 0x20;
	
	for(y = 0; y < bact->act_num_y; y++){
		for(x = 0; x < bact->act_num_x; x++){
			if(x == bact->act_num_x-1){
				trans_size = last_trans_size;
			}
			else{
				trans_size = DUMMY_CELL_SIZE_X / 8 * 0x20;
			}
			
			//��i
			dest_adrs = GFL_CLGRP_CGR_GetAddr(
				bact->cgr_id[x + y*bact->act_num_x], bact->draw_type);
			load_func(&bmp_adrs[src_u], dest_adrs, trans_size);
			
			//���i
			if(y == bact->act_num_y-1 && (bmp_char_y & 1)){
				;	//BMP_Y����L�����̏ꍇ�͍Ō�̉��i�͖���
			}
			else{
				dest_adrs += DUMMY_CELL_SIZE_X / 8 * 0x20;
				load_func(&bmp_adrs[src_d], dest_adrs, trans_size);
			}

			src_u += trans_size;
			src_d += trans_size;
		}
		src_u += bmp_char_x * 0x20;
		src_d += bmp_char_x * 0x20;
	}
}

//--------------------------------------------------------------
/**
 * @brief   BMPOAM�A�N�^�[�̍��W�擾(�擾���W�͍���)
 *
 * @param   bact		
 * @param   x			X���W�����
 * @param   y			Y���W�����
 */
//--------------------------------------------------------------
void BmpOam_ActorGetPos(BMPOAM_ACT_PTR bact, s16 *x, s16 *y)
{
	GFL_CLACTPOS pos;
	
	GFL_CLACT_WK_GetPos(bact->cap[0], &pos, bact->setSerface);
	*x = pos.x;
	*y = pos.y;
}

//--------------------------------------------------------------
/**
 * @brief   BMPOAM�A�N�^�[�̍��W���Z�b�g(�Z�b�g���W�͍���)
 *
 * @param   bact		BMPOAM�A�N�^�[�ւ̃|�C���^
 * @param   x			X���W
 * @param   y			Y���W
 */
//--------------------------------------------------------------
void BmpOam_ActorSetPos(BMPOAM_ACT_PTR bact, s16 x, s16 y)
{
	int num_y, num_x;
	GFL_CLACTPOS pos;
	
	for(num_y = 0; num_y < bact->act_num_y; num_y++){
		for(num_x = 0; num_x < bact->act_num_x; num_x++){
			pos.x = x + num_x * DUMMY_CELL_SIZE_X;
			pos.y = y + num_y * DUMMY_CELL_SIZE_Y;
			GFL_CLACT_WK_SetPos(bact->cap[num_x + num_y*bact->act_num_x], &pos, bact->setSerface);
		}
	}
}

//==============================================================================
//	���[�J���֐�
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ���ʃ��\�[�X�ǂݍ���
 * @param   bsp		�t�H���gOAM�V�X�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
static void _FOLocal_CommonResourceSet(BMPOAM_SYS_PTR bsp)
{
	ARCHANDLE *hdl;
	
	hdl = GFL_ARC_OpenDataHandle(ARCID_BMPOAM, bsp->heap_id);
	
	//�Z�����Z���A�j��
	bsp->cell_id = GFL_CLGRP_CELLANIM_Register(hdl, 
		NARC_bmpoam_bmpoam_dummy32x16_NCER, NARC_bmpoam_bmpoam_dummy32x16_NANR, bsp->heap_id);

	GFL_ARC_CloseDataHandle(hdl);
}

//--------------------------------------------------------------
/**
 * @brief   ���ʃ��\�[�X���
 * @param   bsp		�t�H���gOAM�V�X�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
static void _FOLocal_CommonResourceFree(BMPOAM_SYS_PTR bsp)
{
	GFL_CLGRP_CELLANIM_Release(bsp->cell_id);
}
