//==============================================================================
/**
 * @file	actor_tool.c
 * @brief	�A�N�^�[����p�c�[��
 * @author	matsuda
 * @date	2008.12.18(��)
 */
//==============================================================================
#include <gflib.h>
//#include "system/actor_tool.h"


//==============================================================================
//
//	
//
//==============================================================================
#if 0
void ACTORTOOL_Init(int heap_id, const GFL_CLSYS_INIT *clsysinit, const GFL_DISP_VRAM *dispvram, const GFL_OBJGRP_INIT_PARAM *objparam)
{
	GFL_CLACT_Init(clsysinit, heap_id);
	GFL_OBJGRP_sysStart(heap_id, dispvram, objparam);
}
#endif



#if 0
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
typedef struct _PLTTSLOT_SYSTEM{
	u32 entry_index;	//GFL_OBJGRP_RegisterPltt�Ŏ擾�����o�^index
	u8 pltt_num_max[2];	///<�Ǘ�����p���b�g�ő�o�^�{��
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
 * @param   main_num			�p���b�g�Ǘ��{��:���C�����
 * @param   sub_num				�p���b�g�Ǘ��{��:���C�����
 *
 * @retval  �p���b�g�X���b�g�ւ̃|�C���^
 */
//--------------------------------------------------------------
PLTTSLOT_SYS_PTR PLTTSLOT_Init(int heap_id, int main_num, int sub_num)
{
	PLTTSLOT_SYS_PTR pssp;
	int tbl, i;
	
	GF_ASSERT(main_num <= PLTT_NUM_MAX);
	GF_ASSERT(sub_num <= PLTT_NUM_MAX);
	
	pssp = GFL_HEAP_AllocClearMemory(heap_id, sizeof(PLTTSLOT_SYSTEM));
	pssp->pltt_num_max[0] = main_num;
	pssp->pltt_num_max[1] = sub_num;
	for(tbl = 0; tbl < VRAM_TBL_MAX; tbl++){
		for(i = 0; i < PLTT_NUM_MAX; i++){
			pssp->slot[tbl][i] = PLTT_SLOT_FREE;
		}
	}
	
	return pssp;
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
	for(i = 0; i < pssp->pltt_num_max[tbl]; i++){
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
					pssp->slot[tbl][s] = i;
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
	int pal_no, index;
	
	pal_no = PLTTSLOT_Get(pssp, pltt_num, vram_type);
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


#endif

