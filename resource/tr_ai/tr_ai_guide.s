//============================================================================
/**
 *@file		tr_ai_guide.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	�K�C�h�o�g��AI
//========================================================

GuideAISeq:
	IF_HP_EQUAL	CHECK_DEFENCE,20,GuideAIHPHalf
	IF_HP_UNDER	CHECK_DEFENCE,20,GuideAIHPHalf
	AIEND

//�����HP�������ȉ��Ȃ�u�ɂ���v
GuideAIHPHalf:
	ESCAPE
	AIEND

