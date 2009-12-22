//============================================================================
/**
 *@file		tr_ai_weather.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
// 	�ŏ��̃^�[���ɓV�C�Z������
//========================================================

WeatherAISeq:
	IF_MIKATA_ATTACK	WeatherAISeq_End					// �Ώۂ������Ȃ�I��2006.6.20

	CHECK_TURN
	IFN_EQUAL	0,WeatherAISeq_End						//�ŏ��̃^�[��
	
	IF_WAZA_SEQNO_JUMP	137,WeatherAI_Nihonbare		// �ɂق�΂�
	IF_WAZA_SEQNO_JUMP	136,WeatherAI_Amagoi		// ���܂���
	IF_WAZA_SEQNO_JUMP	115,WeatherAI_Sunaarashi	// ���Ȃ��炵
	IF_WAZA_SEQNO_JUMP	164,WeatherAI_Arare			// �����

WeatherAI_Nihonbare:// �ɂق�΂�	
	CHECK_WEATHER
	IF_EQUAL	WEATHER_HARE,WeatherAISeq_End
	JUMP	WeatherAI_Check_Ok

WeatherAI_Amagoi:// ���܂���	
	CHECK_WEATHER
	IF_EQUAL	WEATHER_AME,WeatherAISeq_End
	JUMP	WeatherAI_Check_Ok

WeatherAI_Sunaarashi:// ���Ȃ��炵	
	CHECK_WEATHER
	IF_EQUAL	WEATHER_SUNAARASHI,WeatherAISeq_End
	JUMP	WeatherAI_Check_Ok

WeatherAI_Arare:// �����	
	CHECK_WEATHER
	IF_EQUAL	WEATHER_ARARE,WeatherAISeq_End
	JUMP	WeatherAI_Check_Ok


WeatherAI_Check_Ok:	
	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_EQUAL	0,WeatherAISeq_End
	INCDEC		5

WeatherAISeq_End:	
	AIEND

