//============================================================================
/**
 *@file		tr_ai_gamble.s
 *@brief	�g���[�i�[AI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	�M�����u��AI ���߁@�Â��`�h�@�ŏ��̃o�g���p�@2010/4/28
//========================================================

GambleAISeq:

#if 0

	IF_MIKATA_ATTACK	GambleAI_end			//�Ώۂ������Ȃ�I��

	CHECK_WAZASEQNO
	IFN_TABLE_JUMP	GambleAI_Table,GambleAI_end

	IF_RND_UNDER	128,GambleAI_end

	INCDEC		2
GambleAI_end:
	AIEND

#else
	COMP_POWER	LOSS_CALC_OFF
	IFN_EQUAL	COMP_POWER_NONE,AmaiAI_damage

  IF_HP_OVER CHECK_DEFENCE,50,AmaiAI_end
	
	IF_RND_UNDER	80,AmaiAI_2
	INCDEC		-1

AmaiAI_2:
  IF_HP_OVER CHECK_DEFENCE,25,AmaiAI_end

	IF_RND_UNDER	50,AmaiAI_end
	INCDEC		-1
  JUMP  AmaiAI_end

AmaiAI_damage:  
	CHECK_TURN
	IFN_EQUAL	0,AmaiAI_end

	INCDEC		1
AmaiAI_end:
	AIEND




#endif


GambleAI_Table:
	.long	1,7,9,38,43,49,83,88,89,98
	.long	118,120,122,140,142,144,170,185,199
	//2006.6.13
	.long	219,226,227,230,241,248
	.long	0xffffffff

