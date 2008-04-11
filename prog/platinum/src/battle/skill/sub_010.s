
//============================================================================
/**
 *
 *@file		sub_010.s
 *@brief	戦闘シーケンス
 *			気絶によるポケモン入れ替えシーケンス
 *@author	HisashiSogabe
 *@data		2005.07.20
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_010:
	POKEMON_LIST
	POKEMON_LIST_WAIT
POKE_RESHUFFLE:
	POKEMON_RESHUFFLE		SIDE_RESHUFFLE
	BALL_GAUGE_RESOURCE_LOAD
	BALL_GAUGE_SET			SIDE_RESHUFFLE
	SERVER_WAIT
	KURIDASU_MESSAGE		SIDE_RESHUFFLE
	SERVER_WAIT
	BALL_GAUGE_OUT			SIDE_RESHUFFLE
	SERVER_WAIT
	BALL_GAUGE_RESOURCE_DELETE
	POKEMON_APPEAR			SIDE_RESHUFFLE
	WAIT_NO_SKIP			ENCOUNT_WAIT_RESHUFFLE
	HP_GAUGE_IN				SIDE_RESHUFFLE
	SERVER_WAIT
	GOSUB					SUB_SEQ_MAKIBISI_CHECK
	//気絶していたら、気絶エフェクトへ
	IF						IF_FLAG_NBIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_KIZETSU,SUB_010_END
	GOSUB					SUB_SEQ_KIZETSU
SUB_010_END:
	POKEMON_RESHUFFLE_LOOP	POKE_RESHUFFLE
	SEQ_END
