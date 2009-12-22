//============================================================================
/**
 *@file		tr_ai_expert.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	�З͂̍����Z��I������AI
//========================================================
StrongAISeq:
	IF_MIKATA_ATTACK	StrongAI_end			//�Ώۂ������Ȃ�I��

	IF_WAZA_HINSHI	LOSS_CALC_OFF,StrongAI_hinshi

	COMP_POWER	LOSS_CALC_OFF
	IF_EQUAL	COMP_POWER_NOTOP,AI_DEC1			//�З͂��P�ԂłȂ��Z

StrongAI_wazacheck://COMP_POWER��ʂ�Ȃ��̂Ō��Z����Ȃ����X�N�𔺂�����ȋZ�itr_ai.dat�Q�Ɓj
	IF_WAZA_SEQNO_JUMP	7,StrongAI_wazacheck_1			// ����			2006.7.20
	IF_WAZA_SEQNO_JUMP	170,StrongAI_wazacheck_1		// �������p���`	2006.7.20
	IF_WAZA_SEQNO_JUMP	248,StrongAI_wazacheck_1		// �ӂ�����		2006.7.20
	JUMP	StrongAI_aisyou

StrongAI_wazacheck_1:
	IF_RND_UNDER	51,StrongAI_aisyou
	INCDEC		-2

StrongAI_aisyou:
//-----------------------------------------------------2004/06/16 sige ------
	CHECK_WAZA_AISYOU	AISYOU_4BAI,StrongAI_10
	AIEND
StrongAI_10:
	IF_RND_UNDER	80,StrongAI_end
	INCDEC		2
//-----------------------------------------------------2004/06/16 sige ------
	AIEND

StrongAI_hinshi:
	IF_WAZA_SEQNO_JUMP	7,StrongAI_end					// ����
	IF_WAZA_SEQNO_JUMP	170,StrongAI_hinshi_kiai		// �������p���`	2006.7.20
	IF_WAZA_SEQNO_JUMP	248,StrongAI_hinshi_kiai		// �ӂ�����		2006.7.20
	IF_WAZA_SEQNO_JUMP	148,StrongAI_hinshi_kiai		// �݂炢�悿	2006.7.20
	IF_WAZA_SEQNO_JUMP	103,StrongAI_hinshi_sensei		// �搧
	JUMP	StrongAI_hinshi2				

StrongAI_hinshi_kiai:
	IF_RND_UNDER	170,StrongAI_end				//1/3�̃����_��
	JUMP	StrongAI_hinshi2

StrongAI_hinshi_sensei:
	INCDEC		2
StrongAI_hinshi2:
	INCDEC		4
StrongAI_end:
	AIEND

AI_DEC1:
	INCDEC		-1	
	AIEND
AI_DEC2:
	INCDEC		-2	
	AIEND
AI_DEC3:
	INCDEC		-3	
	AIEND
AI_DEC5:
	INCDEC		-5	
	AIEND
AI_DEC6:
	INCDEC		-6	
	AIEND
AI_DEC8:
	INCDEC		-8	
	AIEND
AI_DEC10:
	INCDEC		-10
	AIEND
AI_DEC12:
	INCDEC		-12
	AIEND
AI_DEC30:
	INCDEC		-30
	AIEND
AI_INC1:
	INCDEC		1	
	AIEND
AI_INC2:
	INCDEC		2	
	AIEND
AI_INC3:
	INCDEC		3	
	AIEND
AI_INC5:
	INCDEC		5	
	AIEND
AI_INC10:
	INCDEC		10	
	AIEND

