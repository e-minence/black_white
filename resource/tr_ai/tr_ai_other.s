//============================================================================
/**
 *@file		tr_ai_other.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	�ړ��|�P����AI
//========================================================
//�ړ��|�P�����͂P�O�O��������
MvPokeAISeq:
	IF_COND2		CHECK_ATTACK,CONDITION2_SHIME,MvPokeAISeq_end	// ���ߋZ
	IF_COND2		CHECK_ATTACK,CONDITION2_KUROIMANAZASHI,MvPokeAISeq_end	// ���낢�܂Ȃ���
	GET_TOKUSEI		CHECK_DEFENCE
	IF_EQUAL		TOKUSYU_KAGEHUMI,MvPokeAISeq_end	// �����ӂ�
	GET_TOKUSEI		CHECK_ATTACK
	IF_EQUAL		TOKUSYU_HUYUU,MvPokeAISeq_Escape	// �ӂ䂤�́A���肶�����������Ȃ�
	GET_TOKUSEI		CHECK_DEFENCE
	IF_EQUAL		TOKUSYU_ARIZIGOKU,MvPokeAISeq_end	// ���肶����
	
MvPokeAISeq_Escape:
	ESCAPE

MvPokeAISeq_end:
	AIEND

//========================================================
//	�T�t�@���]�[��AI
//========================================================

SafariAISeq:
	SAFARI_ESCAPE_JUMP	SafariAIEscape
	SAFARI
SafariAIEscape:
	ESCAPE

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

DummyAISeq:
	AIEND

