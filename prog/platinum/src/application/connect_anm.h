//==============================================================================
/**
 * @file	connect_anm.h
 * @brief	Wifi�ڑ���ʂ�BG�A�j����inline�֐�
 * @author	matsuda
 * @date	2007.12.26(��)
 *
 * Wifi�ڑ�BG��ʂ��e�A�v���P�[�V�����X�ō쐬����Ă���̂ŁA�A�j���[�V����������ׂ̊֐���
 * �����ɑS��inline�֐��Ƃ��ėp�ӁB(�����̃I�[�o�[���C�ɂ܂������Ă����)
 */
//==============================================================================
#ifndef __CONNECT_ANM_H__
#define __CONNECT_ANM_H__

#include "application/connect_anm_types.h"


//==============================================================================
//
//	inline�֐�
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   Wifi�ڑ�BG�A�j���F������
 *
 * @param   cbp				Wifi�ڑ�BG�̃p���b�g�A�j������\���̂ւ̃|�C���^
 * @param   p_hdl			BG�p���b�g�������Ă���A�[�J�C�u�̃n���h��
 * @param   pal_index		BG�p���b�g��Index
 * @param   heap_id			�q�[�vID(���̏����̃e���|�����Ŏg�p)
 *
 * ���[�N�̊m�ۂ�TCB�̐������͍s���Ă��Ȃ��ׁA�I���֐��͂���܂���B
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_Init(CONNECT_BG_PALANM *cbp, ARCHANDLE *p_hdl, int pal_index, int heap_id)
{
	NNSG2dPaletteData *palData;
	void *p_work;
	
	MI_CpuClear8(cbp, sizeof(CONNECT_BG_PALANM));

	p_work = ArcUtil_HDL_PalDataGet(p_hdl, pal_index, &palData, heap_id);
	
	//�o�b�t�@�Ƀp���b�g�f�[�^���R�s�[
	MI_CpuCopy16(&((u16*)(palData->pRawData))[CBP_PAL_START_NUMBER * 16], 
		cbp->src_color, CBP_PAL_NUM * 0x20);
	MI_CpuCopy16(&((u16*)(palData->pRawData))[CBP_PAL_START_NUMBER * 16], 
		cbp->dest_color, CBP_PAL_NUM * 0x20);
	
	sys_FreeMemoryEz(p_work);
	
	//�L���t���OON
	cbp->occ = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   Wifi�ڑ�BG�A�j���F���C������
 *
 * @param   cbp		Wifi�ڑ�BG�̃p���b�g�A�j������\���̂ւ̃|�C���^
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_Main(CONNECT_BG_PALANM *cbp)
{
	int i, s;
	int anm_pos, evy, next_color_pos;
	
	if(cbp->occ == FALSE){
		return;
	}

	anm_pos = cbp->anm_pos;
	cbp->evy += CBP_ADD_EVY;
	
	if(cbp->evy >= (16 << 8)){
		evy = 16;
		cbp->evy = 0;
		if(cbp->evy_dir == 0){
			cbp->anm_pos++;
			if(cbp->anm_pos >= CBP_PAL_NUM){
				cbp->anm_pos = CBP_PAL_NUM - 2;
				cbp->evy_dir ^= 1;
			}
		}
		else{
			cbp->anm_pos--;
			if(cbp->anm_pos < 0){
				cbp->anm_pos = 1;
				cbp->evy_dir ^= 1;
			}
		}
	}
	else{
		evy = cbp->evy >> 8;
	}

	if(cbp->evy_dir == 0){
		next_color_pos = anm_pos + 1;
		if(next_color_pos >= CBP_PAL_NUM){
			next_color_pos = anm_pos - 1;
		}
	}
	else{
		next_color_pos = anm_pos - 1;
		if(next_color_pos < 0){
			next_color_pos = 1;
		}
	}
	
	for(s = CBP_PAL_COLOR_START; s < CBP_PAL_COLOR_START + CBP_PAL_COLOR_NUM; s++){
		SoftFade(&cbp->src_color[anm_pos][s], &cbp->dest_color[0][s], 
			1, evy, cbp->src_color[next_color_pos][s]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   Wifi�ڑ�BG�A�j���F�]��
 *
 * @param   cbp		Wifi�ڑ�BG�̃p���b�g�A�j������\���̂ւ̃|�C���^
 *
 * VBlank���ɌĂяo���悤�ɂ��Ă�������
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_VBlank(CONNECT_BG_PALANM *cbp)
{
	if(cbp->occ == FALSE){
		return;
	}
	
	DC_FlushRange(cbp->dest_color, CBP_TRANS_PAL_NUM * 0x20);
	GX_LoadBGPltt((const void *)cbp->dest_color, 
		CBP_PAL_START_NUMBER * 0x20, CBP_TRANS_PAL_NUM * 0x20);
	GXS_LoadBGPltt((const void *)cbp->dest_color, 
		CBP_PAL_START_NUMBER * 0x20, CBP_TRANS_PAL_NUM * 0x20);
}

//--------------------------------------------------------------
/**
 * @brief   Wifi�ڑ�BG�A�j���F�L���A�����t���O���Z�b�g
 *
 * @param   cbp		Wifi�ڑ�BG�̃p���b�g�A�j������\���̂ւ̃|�C���^
 * @param   occ		TRUE:�L�����B�@FALSE�F������
 */
//--------------------------------------------------------------
static inline void ConnectBGPalAnm_OccSet(CONNECT_BG_PALANM *cbp, BOOL occ)
{
	cbp->occ = occ;
}


#endif	//__CONNECT_ANM_H__
