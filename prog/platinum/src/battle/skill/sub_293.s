
//============================================================================
/**
 *
 *@file		sub_293.s
 *@brief	�퓬�V�[�P���X
 *			��������ɂ���ăt�H�����`�F���W������|�P�����̃`�F�b�N
 *@author	HisashiSogabe
 *@data		2007.04.17
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_293:
	//�Z���󂯂��|�P�������V�F�C�~�ŁA�X�^�C�v�̋Z��������A�t�H�����`�F���W�i�P���O�j
	IF_PSP			IF_FLAG_NE,SIDE_DEFENCE,ID_PSP_monsno,MONSNO_EURISU,SUB_293_END
	//���łɃt�H�����i���o�[���O�������疳��
	IF_PSP			IF_FLAG_EQ,SIDE_DEFENCE,ID_PSP_form_no,0,SUB_293_END
	//�g���肪�o�Ă���Ƃ��͖���
	IF_PSP			IF_FLAG_BIT,SIDE_DEFENCE,ID_PSP_condition2,CONDITION2_MIGAWARI,SUB_293_END
	WAZA_TYPE_CHECK	KOORI_TYPE,SheimiFormChg
	IF				IF_FLAG_NE,BUF_PARA_WAZA_TYPE,KOORI_TYPE,SUB_293_END
SheimiFormChg:
	VALUE_WORK		VAL_SET,BUF_PARA_CLIENT_WORK,BUF_PARA_DEFENCE_CLIENT
	PSP_VALUE		VAL_SET,SIDE_DEFENCE,ID_PSP_form_no,0
	GOSUB			SUB_SEQ_TENKIYA
SUB_293_END:
	SEQ_END
