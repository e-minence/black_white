//==============================================================================
/**
 * @file	actor_tool.c
 * @brief	�A�N�^�[����p�c�[��
 * @author	matsuda
 * @date	2008.12.18(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/actor_tool.h"


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



#if 1
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
#define VRAMTBL(draw_type)		((draw_type == CLSYS_DRAW_MAIN) ? 0 : 1)


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
 * @param   draw_type		VRAM�^�C�v
 *
 * @retval  �p���b�g�ԍ�
 */
//--------------------------------------------------------------
static u32 PLTTSLOT_Get(PLTTSLOT_SYS_PTR pssp, int pltt_num, CLSYS_DRAW_TYPE draw_type)
{
	int i, s, tbl;
	
	tbl = VRAMTBL(draw_type);
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
 * @param   draw_type		VRAM�^�C�v
 */
//--------------------------------------------------------------
static void PLTTSLOT_Free(PLTTSLOT_SYS_PTR pssp, u32 pal_no, CLSYS_DRAW_TYPE draw_type)
{
	int i, tbl;
	
	tbl = VRAMTBL(draw_type);
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
 * @param   draw_type		CLSYS_DRAW_MAIN or CLSYS_DRAW_SUB
 * @param   heap_id			�e���|�����Ŏg�p����q�[�vID
 *
 * @retval  �o�^INDEX(�p���b�g�ԍ��ł͂���܂���B�폜���ɕK�v�ɂȂ�܂�)
 */
//--------------------------------------------------------------
u32 PLTTSLOT_ResourceSet(PLTTSLOT_SYS_PTR pssp, ARCHANDLE *handle, u32 data_id, CLSYS_DRAW_TYPE draw_type, int pltt_num, int heap_id)
{
	int pal_no, index;
	
	pal_no = PLTTSLOT_Get(pssp, pltt_num, draw_type);
	index = GFL_CLGRP_PLTT_RegisterEx(
		handle, data_id, draw_type, pal_no * 0x20, 0, pltt_num, heap_id);
	return index;
}

//--------------------------------------------------------------
/**
 * @brief   ���k�p���b�g���\�[�X�̓]��(�Ɨ̈�m��) ��-pcm�I�v�V���������Ă���p���b�g���Ώ�
 *
 * @param   pssp			�p���b�g�X���b�g�ւ̃|�C���^
 * @param   handle			�A�[�J�C�u�n���h��
 * @param   data_id			���f�[�^�ւ̃f�[�^Index
 * @param   pltt_num		�]������p���b�g�{��
 * @param   draw_type		CLSYS_DRAW_MAIN or CLSYS_DRAW_SUB
 * @param   heap_id			�e���|�����Ŏg�p����q�[�vID
 *
 * @retval  �o�^INDEX(�p���b�g�ԍ��ł͂���܂���B�폜���ɕK�v�ɂȂ�܂�)
 */
//--------------------------------------------------------------
u32 PLTTSLOT_ResourceCompSet(PLTTSLOT_SYS_PTR pssp, ARCHANDLE *handle, u32 data_id, CLSYS_DRAW_TYPE draw_type, int pltt_num, int heap_id)
{
	int pal_no, index;

#ifdef PM_DEBUG
  {//���k�p���b�g�o�^�ɖ{���w�肪�����ׁApltt_num�ƓW�J�{������v���Ă��邩���ׂ�
    BOOL cmpFlag;
    NNSG2dPaletteCompressInfo*  cmpData;
    NNSG2dPaletteData*  palData;
    void* loadPtr;
    
    loadPtr = GFL_ARC_LoadDataAllocByHandle(handle, data_id, GFL_HEAP_LOWID(heap_id));
    
    cmpFlag = NNS_G2dGetUnpackedPaletteCompressInfo( loadPtr, &cmpData );
    GF_ASSERT(cmpFlag); //-pcm�I�v�V�����������p���b�g�ł͂Ȃ�
    
    if( NNS_G2dGetUnpackedPaletteData( loadPtr, &palData ) ){
      //�W�J�T�C�Y��pltt_num���s��v�B
      //����GFL_CLGRP_PLTT_RegisterComp�ɂ͖{���w�肪�����ׁA����ASSERT�����Ă���
      OS_TPrintf("pcm palData->szByte = 0x%x, pltt_num = 0x%x\n", palData->szByte, pltt_num*0x20);
      GF_ASSERT(palData->szByte == pltt_num*0x20);
    }
    else{
      GF_ASSERT("unpacked faile");
    }
    
    GFL_HEAP_FreeMemory(loadPtr);
  }
#endif

	pal_no = PLTTSLOT_Get(pssp, pltt_num, draw_type);
	index = GFL_CLGRP_PLTT_RegisterComp( handle, data_id, draw_type, pal_no * 0x20, heap_id );
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
u32 PLTTSLOT_GetPalNo(PLTTSLOT_SYS_PTR pssp, u32 index, CLSYS_DRAW_TYPE draw_type)
{
	return GFL_CLGRP_PLTT_GetAddr(index, draw_type) / 0x20;
}

//--------------------------------------------------------------
/**
 * @brief   �p���b�g���\�[�X�̉��(�Ɨ̈���)
 *
 * @param   pssp		�p���b�g�X���b�g�ւ̃|�C���^
 * @param   index		�o�^INDEX (PLTTSLOT_ResourceSet�֐��̖߂�l)
 */
//--------------------------------------------------------------
void PLTTSLOT_ResourceFree(PLTTSLOT_SYS_PTR pssp, u32 index, CLSYS_DRAW_TYPE draw_type)
{
	int pal_no;
	
	pal_no = PLTTSLOT_GetPalNo(pssp, index, draw_type);
	PLTTSLOT_Free(pssp, pal_no, draw_type);
	GFL_CLGRP_PLTT_Release(index);
}

#endif

