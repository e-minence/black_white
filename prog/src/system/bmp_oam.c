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
#define FACT_ONE_ACT_NUM			(24)


///�f�[�^�̃}�b�s���O���[�h���̃A�N�Z�Xindex
enum{
	DATAID_128K,
	DATAID_64K,
	DATAID_32K,
	
	DATAID_MAX,
	DATAID_NULL = 0xffffffff,	//���g�p
};

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
	u32 cell_id[DATAID_MAX];			///<���ʃZ���̓o�^index
}BMPOAM_SYS;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static u32 _FOLocal_CellResourceSet(BMPOAM_SYS_PTR bsp, CLSYS_DRAW_TYPE draw_type, ARCHANDLE *hdl);
static void _FOLocal_CommonResourceFree(BMPOAM_SYS_PTR bsp);


//==============================================================================
//	�f�[�^
//==============================================================================
///���\�[�X�f�[�^�̃e�[�u���@�@��DATAID_???�ƕ��т𓯂��ɂ��Ă������ƁI
static const struct{
	u32 ncgr;
	u32 ncer;
	u32 nanr;
}OamResourceTbl[] = {
	{
		NARC_bmpoam_bmpoam_dummy32x16_NCGR,
		NARC_bmpoam_bmpoam_dummy32x16_NCER,
		NARC_bmpoam_bmpoam_dummy32x16_NANR,
	},
	{
		NARC_bmpoam_bmpoam_dummy32x16_64k_NCGR,
		NARC_bmpoam_bmpoam_dummy32x16_64k_NCER,
		NARC_bmpoam_bmpoam_dummy32x16_64k_NANR,
	},
	{
		NARC_bmpoam_bmpoam_dummy32x16_32k_NCGR,
		NARC_bmpoam_bmpoam_dummy32x16_32k_NCER,
		NARC_bmpoam_bmpoam_dummy32x16_32k_NANR,
	},
};

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
	int i;
	
	bsp = GFL_HEAP_AllocClearMemory(heap_id, sizeof(BMPOAM_SYS));
	bsp->heap_id = heap_id;
	bsp->clunit = p_unit;
	for(i = 0; i < DATAID_MAX; i++){
		bsp->cell_id[i] = DATAID_NULL;
	}
	
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
	u32 data_index;
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
	bact->draw_type = head->draw_type;
	
	//�L�����o�^
	hdl = GFL_ARC_OpenDataHandle(ARCID_BMPOAM, GetHeapLowID(bsp->heap_id));
	for(y = 0; y < num_y; y++){
		for(x = 0; x < num_x; x++){
			//���ʃZ�����\�[�X�o�^
			data_index = _FOLocal_CellResourceSet(bsp, head->draw_type, hdl);
			//�L�����o�^(�̈�m��)
			bact->cgr_id[x + y*num_x] = GFL_CLGRP_CGR_Register(hdl, 
				OamResourceTbl[data_index].ncgr, FALSE, head->draw_type, bsp->heap_id);
			//�A�N�^�[����
			clwkdata.pos_x = head->x + x * DUMMY_CELL_SIZE_X;
			clwkdata.pos_y = head->y + y * DUMMY_CELL_SIZE_Y;
			clwkdata.softpri = head->soft_pri;
			clwkdata.bgpri = head->bg_pri;
			bact->cap[x + y*num_x] = GFL_CLACT_WK_Create(bsp->clunit, 
				bact->cgr_id[x + y*num_x], head->pltt_index, bsp->cell_id[data_index], &clwkdata, 
				head->setSerface, bsp->heap_id);
			GFL_CLACT_WK_SetPlttOffs(bact->cap[x + y*num_x], head->pal_offset, CLWK_PLTTOFFS_MODE_OAM_COLOR);

      // SRT�A�j��OFF
      GFL_CLACT_WK_SetSRTAnimeFlag( bact->cap[x + y*num_x], FALSE );
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

//----------------------------------------------------------------------------
/**
 *	@brief  BMPOAM�A�N�^�[��OBJ���[�h���Z�b�g
 *
 *	@param	BMPOAM_ACT_PTR bact BMPOAM�A�N�^�[�ւ̃|�C���^
 *	@param	mode                ���[�h
 */
//-----------------------------------------------------------------------------
void BmpOam_ActorSetObjMode( BMPOAM_ACT_PTR bact, GXOamMode mode )
{ 
	int num_y, num_x;
	for(num_y = 0; num_y < bact->act_num_y; num_y++){
		for(num_x = 0; num_x < bact->act_num_x; num_x++){
      GFL_CLACT_WK_SetObjMode( bact->cap[num_x + num_y*bact->act_num_x], mode );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPOAM�A�N�^�[��OBJ�v���C�I���e�B���Z�b�g
 *
 *	@param	BMPOAM_ACT_PTR bact
 *	@param	soft_pri 
 */
//-----------------------------------------------------------------------------
void BmpOam_ActorSetSoftPriprity( BMPOAM_ACT_PTR bact, u8 soft_pri )
{ 
	int num_y, num_x;
	for(num_y = 0; num_y < bact->act_num_y; num_y++){
		for(num_x = 0; num_x < bact->act_num_x; num_x++){
      GFL_CLACT_WK_SetSoftPri( bact->cap[num_x + num_y*bact->act_num_x], soft_pri );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPOAM�A�N�^�[��BG�v���C�I���e�B���Z�b�g
 *
 *	@param	BMPOAM_ACT_PTR bact
 *	@param	bg_pri
 */
//-----------------------------------------------------------------------------
void BmpOam_ActorSetBgPriority( BMPOAM_ACT_PTR bact, u8 bg_pri )
{ 
	int num_y, num_x;
	for(num_y = 0; num_y < bact->act_num_y; num_y++){
		for(num_x = 0; num_x < bact->act_num_x; num_x++){
      GFL_CLACT_WK_SetBgPri( bact->cap[num_x + num_y*bact->act_num_x], bg_pri );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPOAM�A�N�^�[�̃p���b�g�I�t�Z�b�g���Z�b�g
 *
 *	@param	BMPOAM_ACT_PTR bact
 *	@param	pal_offset �p���b�g�I�t�Z�b�g(0�`15)
 */
//-----------------------------------------------------------------------------
void BmpOam_ActorSetPaletteOffset( BMPOAM_ACT_PTR bact, u8 pal_offset )
{ 
	int num_y, num_x;
	for(num_y = 0; num_y < bact->act_num_y; num_y++){
		for(num_x = 0; num_x < bact->act_num_x; num_x++){
			GFL_CLACT_WK_SetPlttOffs(
          bact->cap[num_x + num_y*bact->act_num_x], pal_offset, CLWK_PLTTOFFS_MODE_OAM_COLOR);
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
 * @param   draw_type
 * @retval	�f�[�^Index
 *
 * ���ɓ����}�b�s���O���[�h�̃Z�����o�^����Ă���ꍇ�́A����Index��Ԃ��܂�
 */
//--------------------------------------------------------------
static u32 _FOLocal_CellResourceSet(BMPOAM_SYS_PTR bsp, CLSYS_DRAW_TYPE draw_type, ARCHANDLE *hdl)
{
	GXOBJVRamModeChar obj_vram_mode;
	int index;
	
	//�}�b�s���O���[�h���擾���A�o�^�ς݂��`�F�b�N
	if(draw_type == CLSYS_DRAW_MAIN){
		obj_vram_mode = GX_GetOBJVRamModeChar();
	}
	else{
		obj_vram_mode = GXS_GetOBJVRamModeChar();
	}
	switch(obj_vram_mode){
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		index = DATAID_128K;
		break;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		index = DATAID_64K;
		break;
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		index = DATAID_32K;
		break;
	default:
		GF_ASSERT(0);	//�Ή����Ă��Ȃ��}�b�s���O���[�h�ł�
		index = DATAID_128K;
		break;
	}
	if(bsp->cell_id[index] != DATAID_NULL){
		return index;		//���̃}�b�s���O���[�h�̃Z���͊��ɓo�^�ς�
	}

	//�Z�����Z���A�j��
	bsp->cell_id[index] = GFL_CLGRP_CELLANIM_Register(hdl, 
		OamResourceTbl[index].ncer, OamResourceTbl[index].nanr, bsp->heap_id);

	return index;
}

//--------------------------------------------------------------
/**
 * @brief   ���ʃ��\�[�X���
 * @param   bsp		�t�H���gOAM�V�X�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
static void _FOLocal_CommonResourceFree(BMPOAM_SYS_PTR bsp)
{
	int i;
	
	for(i = 0; i < DATAID_MAX; i++){
		if(bsp->cell_id[i] != DATAID_NULL){
			GFL_CLGRP_CELLANIM_Release(bsp->cell_id[i]);
			bsp->cell_id[i] = DATAID_NULL;
		}
	}
}
