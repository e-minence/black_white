//============================================================================
/**
 *@file		tr_ai_safari.s
 *@brief	トレーナーAI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	サファリゾーンAI
//========================================================

SafariAISeq:
	SAFARI_ESCAPE_JUMP	SafariAIEscape
	SAFARI
SafariAIEscape:
	ESCAPE

