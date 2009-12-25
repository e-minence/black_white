//============================================================================
/**
 *@file		tr_ai_move.s
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
	IF_WAZASICK		CHECK_ATTACK,WAZASICK_BIND,MvPokeAISeq_end	// ���ߋZ
	IF_WAZASICK		CHECK_ATTACK,WAZASICK_TOOSENBOU,MvPokeAISeq_end	// ���낢�܂Ȃ���
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

