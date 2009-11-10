//==============================================================================
/**
 * @file	gds_data_conv.c
 * @brief	�{�b�N�X��h���X�A�b�v����GDS�T�[�o�[�Ŏg�p����`���ɕϊ�����c�[��
 * @author	matsuda
 * @date	2007.12.10(��)
 */
//==============================================================================
#include <gflib.h>
#include "savedata\save_control.h"
#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "poke_tool/monsno_def.h"
//#include "gflib/strbuf_family.h"

#include "savedata/gds_profile.h"
#include "savedata/gds_profile_types.h"
#include "gds_data_conv.h"




#if GDS_FIX
//==============================================================================
//	�h���X�A�b�v
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �h���X�A�b�v�f�[�^��GT�h���X�A�b�v�`���ɕϊ�����
 *
 * @param   dress			���f�[�^�ƂȂ�h���X�A�b�v�f�[�^
 * @param   gt_dress		�ϊ���̃f�[�^�Z�b�g��
 */
//--------------------------------------------------------------
void GDS_CONV_Dress_to_GTDress(SAVE_CONTROL_WORK *sv, const IMC_TELEVISION_SAVEDATA * dress, GT_GDS_DRESS * gt_dress)
{
	ImcTelevision_to_GTDress(dress, gt_dress);
	
	//�`�F�b�N�T���쐬
	gt_dress->crc.crc16ccitt_hash = 
		GFL_STD_CrcCalc(gt_dress, sizeof(GT_GDS_DRESS) - GT_GDS_CRC_SIZE);
}

//--------------------------------------------------------------
/**
 * @brief   GT�h���X�A�b�v���h���X�A�b�v�`���ɕϊ�����
 *
 * @param   gt_dress		���f�[�^�ƂȂ�GT�h���X�A�b�v�f�[�^
 * @param   dress			�ϊ���̃f�[�^�Z�b�g��
 */
//--------------------------------------------------------------
void GDS_CONV_GTDress_to_Dress(const GT_GDS_DRESS * gt_dress, IMC_TELEVISION_SAVEDATA * dress)
{
	GTDress_to_ImcTelevision(gt_dress, dress);
}


//==============================================================================
//	�{�b�N�X�V���b�g
//		���{�b�N�X�͕`�敔����GT�\���̂��g�p���邽�߁A�����̃{�b�N�X�𑗐M���鎞��
//		  �ϊ����߂������K�v
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �{�b�N�X�f�[�^��GT�{�b�N�X�V���b�g�f�[�^�ɕϊ�
 *
 * @param   boxdata			�{�b�N�X�f�[�^�ւ̃|�C���^
 * @param   tray_number		�g���C�ԍ�
 * @param   gt_box			�f�[�^�����
 * @param   heap_id			�e���|�����Ƃ��Ďg�p����q�[�v��ID
 */
//--------------------------------------------------------------
void GDS_CONV_Box_to_GTBox(SAVE_CONTROL_WORK *sv, const BOX_DATA *boxdata, int tray_number, GT_BOX_SHOT *gt_box, int heap_id)
{
	STRBUF *box_name;
	int i;

	GFL_STD_MemClear( gt_box, sizeof( GT_BOX_SHOT ) );
	
	box_name = GFL_STR_CreateBuffer(GT_PLW_BOX_TRAYNAME_SIZE * 3, heap_id);	//�O�̂��ߑ��߂Ɋm��
	BOXDAT_GetBoxName(boxdata, tray_number, box_name);
	STRBUF_GetStringCode(box_name, gt_box->box_name, GT_PLW_BOX_TRAYNAME_SIZE);
	GFL_STR_DeleteBuffer(box_name);
	
	for(i = 0; i < GT_PLW_BOX_MAX_POS; i++){
		gt_box->monsno[i] = BOXDAT_PokeParaGet(boxdata, tray_number, i, ID_PARA_monsno, NULL);
		gt_box->personal_rnd[i] = 
			BOXDAT_PokeParaGet(boxdata, tray_number, i, ID_PARA_personal_rnd, NULL);
		gt_box->id_no[i] = 
			BOXDAT_PokeParaGet(boxdata, tray_number, i, ID_PARA_id_no, NULL);
		if(BOXDAT_PokeParaGet(boxdata, tray_number, i, ID_PARA_monsno_egg, NULL) == MONSNO_TAMAGO){
			gt_box->egg_bit |= 1 << i;
		}
		gt_box->form_no[i] = BOXDAT_PokeParaGet(boxdata, tray_number, i, ID_PARA_form_no, NULL);
	}
	
	gt_box->wallPaper = BOXDAT_GetWallPaperNumber(boxdata, tray_number);
	
	//�T�[�o�[���M���ɃZ�b�g����̂ł����ł�0������
	gt_box->category_no = 0;
	
	//�`�F�b�N�T���쐬
	gt_box->crc.crc16ccitt_hash = 
		GFL_STD_CrcCalc(gt_box, sizeof(GT_BOX_SHOT) - GT_GDS_CRC_SIZE);
}

//--------------------------------------------------------------
/**
 * @brief   GT�{�b�N�X�V���b�g�ɓo�^�J�e�S���[�ԍ�����
 *
 * @param   gt_box			GT�{�b�N�X�V���b�g�ւ̃|�C���^
 * @param   category_no		�o�^�J�e�S���[�ԍ�
 *
 * �`�F�b�N�T������蒼���ׁA�o�^�J�e�S���[�ԍ����Z�b�g����ꍇ�͕K�����̊֐����g�p���Ă�������
 */
//--------------------------------------------------------------
void GDS_GTBoxShot_SetCategoryNo(SAVE_CONTROL_WORK *sv, GT_BOX_SHOT *gt_box, int category_no)
{
	gt_box->category_no = category_no;
	//�`�F�b�N�T������蒼��
	gt_box->crc.crc16ccitt_hash = 
		GFL_STD_CrcCalc(gt_box, sizeof(GT_BOX_SHOT) - GT_GDS_CRC_SIZE);
}

#endif  //GDS_FIX

//==============================================================================
//
//	GDS�v���t�B�[��
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   GDS�v���t�B�[����GTGDS�v���t�B�[���`���ɕϊ�����
 *
 * @param   gpp			���f�[�^�ƂȂ�GDS�v���t�B�[��
 * @param   gt_gpp		�ϊ���̃f�[�^�Z�b�g��
 */
//--------------------------------------------------------------
void GDS_CONV_GDSProfile_to_GTGDSProfile(SAVE_CONTROL_WORK *sv, const GDS_PROFILE_PTR gpp, GT_GDS_PROFILE * gt_gpp)
{
	GF_ASSERT(sizeof(GT_GDS_PROFILE) == sizeof(GDS_PROFILE));
	
	GFL_STD_MemClear(gt_gpp, sizeof(GT_GDS_PROFILE));
	GFL_STD_MemCopy(gpp, gt_gpp, sizeof(GT_GDS_PROFILE));
}

//--------------------------------------------------------------
/**
 * @brief   GTGDS�v���t�B�[����GDS�v���t�B�[���`���ɕϊ�����
 *
 * @param   gt_dress		���f�[�^�ƂȂ�GT�h���X�A�b�v�f�[�^
 * @param   dress			�ϊ���̃f�[�^�Z�b�g��
 */
//--------------------------------------------------------------
void GDS_CONV_GTGDSProfile_to_GDSProfile(const GT_GDS_PROFILE * gt_gpp, GDS_PROFILE_PTR gpp)
{
	GF_ASSERT(sizeof(GT_GDS_PROFILE) == sizeof(GDS_PROFILE));

	GFL_STD_MemCopy(gt_gpp, gpp, sizeof(GT_GDS_PROFILE));
}

#if GDS_FIX
//--------------------------------------------------------------
/**
 * @brief   GDS�v���t�B�[���������L���O�v���t�B�[���`���ɕϊ�����
 *
 * @param   gpp			���f�[�^�ƂȂ�GDS�v���t�B�[��
 * @param   gt_rp		�ϊ���̃f�[�^�Z�b�g��
 */
//--------------------------------------------------------------
void GDS_CONV_GDSProfile_to_GTRankingProfile(SAVE_CONTROL_WORK *sv, const GDS_PROFILE_PTR gpp, GT_RANKING_PROFILE * gt_rp)
{
	GFL_STD_MemClear(gt_rp, sizeof(GT_RANKING_PROFILE));
	
	gt_rp->version_code = gpp->version_code;
	gt_rp->language = gpp->language;
	gt_rp->birthday_month = gpp->birthday_month;
	gt_rp->trainer_view = gpp->trainer_view;
	gt_rp->monsno = gpp->monsno;
	gt_rp->egg_flag = gpp->egg_flag;
}
#endif  //GDS_FIX
