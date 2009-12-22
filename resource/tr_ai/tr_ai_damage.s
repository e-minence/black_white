//============================================================================
/**
 *@file		tr_ai_damage.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	�_���[�W�Z�d��AI
//========================================================
DamageAISeq:
	IF_MIKATA_ATTACK	DamageAI_end			//�Ώۂ������Ȃ�I��

	COMP_POWER	LOSS_CALC_OFF
	IFN_EQUAL	COMP_POWER_NONE,DamageAI_end	// �З͌v�Z����Z
	IF_RND_UNDER	100,DamageAI_end
	
	INCDEC		2

DamageAI_end:
	AIEND

