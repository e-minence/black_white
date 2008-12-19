//==============================================================================
/**
 * @file	actor_tool.c
 * @brief	�A�N�^�[����p�c�[��
 * @author	matsuda
 * @date	2008.12.18(��)
 */
//==============================================================================
#include <gflib.h>


///�u�q�`�l�o���N�ݒ�\����
static const GFL_DISP_VRAM dispVram = {
	GX_VRAM_BG_16_F,				//���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE,			//���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_128_C,			//�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE,		//�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E,				//���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE,		//���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_16_I,			//�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	//�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_01_AB,				//�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_0_G,			//�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
	GX_OBJVRAMMODE_CHAR_1D_64K,		// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};

static const GFL_CLSYS_INIT clactIni = {
	0,0,	// ���C���@�T�[�t�F�[�X�̍�����W
	0,1000,	// �T�u�@�T�[�t�F�[�X�̍�����W
	0,128,	// ���C�����OAM�Ǘ��J�n�ʒu�A�Ǘ���
	0,128,	// �T�u���OAM�Ǘ��J�n�ʒu�A�Ǘ���
	64,		// �Z��Vram�]���Ǘ���
};

static const GFL_OBJGRP_INIT_PARAM objgrpIni = {
	64,			///< �o�^�ł���L�����f�[�^��
	64,			///< �o�^�ł���p���b�g�f�[�^��
	64,			///< �o�^�ł���Z���A�j���p�^�[����
	16,			///< �o�^�ł���}���`�Z���A�j���p�^�[�����i�����󖢑Ή��j
};


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
	u32 data_id;			///<�o�^�f�[�^ID
	u32 entry_index;		///<�o�^INDEX
	

//==============================================================================
//
//	
//
//==============================================================================
void ACTORTOOL_Init(int heap_id, const GFL_CLSYS_INIT *clsysinit, const GFL_DISP_VRAM *dispvram, const GFL_OBJGRP_INIT_PARAM *objparam)
{
	GFL_CLACT_Init(clsysinit, heap_id);
	GFL_OBJGRP_sysStart(heap_id, dispvram, objparam);
}



//==============================================================================
//
//	�p���b�g�X���b�g�Ǘ�
//
//==============================================================================
//==============================================================================
//	�萔��`
//==============================================================================
///�p���b�g�o�^�ő�{��
#define PLTT_NUM_MAX		(16)
///�X���b�g�̋󂫂������l
#define PLTT_SLOT_FREE		(0xff)
///VRAMTBL�ő吔
#define VRAM_TBL_MAX		(2)


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
	u32 entry_index;	//GFL_OBJGRP_RegisterPltt�Ŏ擾�����o�^index
	u8 pltt_num_max;	///<�Ǘ�����p���b�g�ő�o�^�{��
	u8 slot[VRAM_TBL_MAX][16];
}PLTTSLOT_SYSTEM;


//==============================================================================
//	�}�N��
//==============================================================================
///GFL_VRAM_???��VRAMTBL�ɕϊ�����}�N��
#define VRAMTBL(vram_type)		((vram_type == GFL_VRAM_2D_MAIN) ? 0 : 1)


//--------------------------------------------------------------
/**
 * @brief   �p���b�g�X���b�g�Ǘ��V�X�e���쐬
 *
 * @param   heap_id				�q�[�vID
 * @param   pltt_num_max		�p���b�g�Ǘ��{��
 *
 * @retval  �p���b�g�X���b�g�ւ̃|�C���^
 */
//--------------------------------------------------------------
PLTTSLOT_SYS_PTR PLTTSLOT_Init(int heap_id, int pltt_num_max)
{
	PLTTSLOT_SYS_PTR pssp;
	int i;
	
	GF_ASSERT(pltt_num_max <= PLTT_NUM_MAX);
	
	pssp = GFL_HEAP_AllocClearMemory(heap_id, sizeof(PLTTSLOT_SYSTEM));
	pssp->pltt_num_max = pltt_num_max;
	for(tbl = 0; tbl < VRAM_TBL_MAX; tbl++){
		for(i = 0; i < PLTT_NUM_MAX; i++){
			pssp->slot[tb][i] = PLTT_SLOT_FREE;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   �p���b�g�X���b�g�Ǘ��V�X�e���폜
 *
 * @param   pssp		�p���b�g�X���b�g�ւ̃|�C���^
 */
//--------------------------------------------------------------
void PLTTSLOT_Exit(PLTTSLOT_SYS_PTR pssp)
{
	GFL_HEAP_FreeMemory(pssp);
}

//--------------------------------------------------------------
/**
 * @brief   �p���b�g�o�^�G���A���擾
 *
 * @param   pssp			�p���b�g�X���b�g�ւ̃|�C���^
 * @param   pltt_num		�p���b�g�{��
 * @param   vram_type		VRAM�^�C�v
 *
 * @retval  �p���b�g�ԍ�
 */
//--------------------------------------------------------------
static u32 PLTTSLOT_Get(PLTTSLOT_SYS_PTR pssp, int pltt_num, GFL_VRAM_TYPE vram_type)
{
	int i, s, tbl;
	
	tbl = VRAMTBL(vram_type);
	for(i = 0; i < pssp->pltt_num_max; i++){
		if(pssp->slot[tbl][i] == PLTT_SLOT_FREE){
			for(s = i; s < i+pltt_num; s++){
				if(pssp->slot[tbl][s] != PLTT_SLOT_FREE){
					i = s;
					break;	//�r���ɋ󂫃X���b�g�ł͂Ȃ��̈悪������
				}
			}
			if(s == i+pltt_num){
				//�m�ې���
				for(s = i; s < i+pltt_num; s++){
					pspp->slot[tbl][s] = i;
				}
				return i;
			}
		}
	}
	
	GF_ASSERT(0);	//�p���b�g�ɋ󂫂�����
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   �p���b�g�o�^�G���A�����
 *
 * @param   pssp			�p���b�g�X���b�g�ւ̃|�C���^
 * @param   pal_no			�p���b�g�ԍ�(PLTTSLOT_Get�֐��̖߂�l)
 * @param   vram_type		VRAM�^�C�v
 */
//--------------------------------------------------------------
static void PLTTSLOT_Free(PLTTSLOT_SYS_PTR pssp, u32 pal_no, GFL_VRAM_TYPE vram_type)
{
	int i, tbl;
	
	tbl = VRAMTBL(vram_type);
	for(i = pal_no; pssp->slot[tbl][i] == pal_no; i++){
		pssp->slot[tbl][i] = PLTT_SLOT_FREE;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �p���b�g���\�[�X�̓]��(�Ɨ̈�m��)
 *
 * @param   pssp			�p���b�g�X���b�g�ւ̃|�C���^
 * @param   handle			�A�[�J�C�u�n���h��
 * @param   data_id			���f�[�^�ւ̃f�[�^Index
 * @param   pltt_num		�]������p���b�g�{��
 * @param   vram_type		GFL_VRAM_2D_MAIN or GFL_VRAM_2D_SUB (��GFL_VRAM_2D_BOTH�͔�Ή�)
 * @param   heap_id			�e���|�����Ŏg�p����q�[�vID
 *
 * @retval  �o�^INDEX(�p���b�g�ԍ��ł͂���܂���B�폜���ɕK�v�ɂȂ�܂�)
 */
//--------------------------------------------------------------
u32 PLTTSLOT_ResourceSet(PLTTSLOT_SYS_PTR pssp, ARCHANDLE *handle, u32 data_id, int pltt_num, GFL_VRAM_TYPE vram_type, int heap_id)
{
	int pal_no;
	
	pal_no = PLTTSLOT_Get(pssp, pltt_num);
	index = GFL_OBJGRP_RegisterPlttEx(
		handle, data_id, vram_type, pal_no * 0x20, 0, pltt_num, heap_id);
	return index;
}

//--------------------------------------------------------------
/**
 * @brief   �G���g���[�ԍ�����p���b�g�ԍ����擾
 * @param   pssp		�p���b�g�X���b�g�ւ̃|�C���^
 * @param   index		�o�^INDEX (PLTTSLOT_ResourceSet�֐��̖߂�l)
 * @retval	�p���b�g�ԍ�
 */
//--------------------------------------------------------------
u32 PLTTSLOT_GetPalNo(PLTTSLOT_SYS_PTR pssp, u32 index, GFL_VRAM_TYPE vram_type)
{
	int offset, nitro_vramtype;
	NNSG2dImagePaletteProxy proxy;
	
	nitro_vramtype = ((vram_type == GFL_VRAM_2D_MAIN) ? NNS_G2D_VRAM_TYPE_2DMAIN : NNS_G2D_VRAM_TYPE_2DSUB);
	
	GFL_OBJGRP_GetPlttProxy(index, &proxy);
	offset =  NNS_G2dGetImagePaletteLocation(&proxy, nitro_vramtype);
	GF_ASSERT(offset != NNS_G2D_VRAM_ADDR_NOT_INITIALIZED);
	
	return offset / 0x20;
}

//--------------------------------------------------------------
/**
 * @brief   �p���b�g���\�[�X�̉��(�Ɨ̈���)
 *
 * @param   pssp		�p���b�g�X���b�g�ւ̃|�C���^
 * @param   index		�o�^INDEX (PLTTSLOT_ResourceSet�֐��̖߂�l)
 */
//--------------------------------------------------------------
void PLTTSLOT_ResourceFree(PLTTSLOT_SYS_PTR pssp, u32 index, GFL_VRAM_TYPE vram_type)
{
	int pal_no;
	
	pal_no = PLTTSLOT_GetPalNo(pssp, index, vram_type);
	PLTTSLOT_Free(pssp, pal_no, vram_type);
}


