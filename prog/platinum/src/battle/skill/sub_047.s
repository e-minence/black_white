
//============================================================================
/**
 *
 *@file		sub_047.s
 *@brief	戦闘シーケンス
 *			どくどくの追加効果シーケンス
 *@author	HisashiSogabe
 *@data		2006.01.17
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_047:
	//どくびしでの追加の時は専用のチェックをする
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DOKUBISI,SoubiItemCheckStart

Dokubisi:
	//特性めんえきを持っているときは、失敗する
	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_MENEKI,TokuseiNoDoku
	//天候無効系の特性は、天候チェックを無視
	NOOTENKI_CHECK	NoRinpun
	//天候がはれじゃないときは、特性リーフガードチェックをしない
	IF				IF_FLAG_NBIT,BUF_PARA_FIELD_CONDITION,FIELD_CONDITION_HARE_ALL,NoRinpun
	//特性リーフガードを持っているときは、失敗する
	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_RIIHUGAADO,TokuseiNoDoku
	BRANCH			NoRinpun

SoubiItemCheckStart:
	//装備アイテムでの追加の時は専用のチェックをする
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_SOUBIITEM,NomalCheck
	//装備アイテムでは、防がれた系のメッセージは表示しない（暫定）
SoubiItemCheck:
	//特性めんえきを持っているときは、失敗する
	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_MENEKI,SEQ_047_END
	//天候無効系の特性は、天候チェックを無視
	NOOTENKI_CHECK	SoubiItemCheckNext
	//天候がはれじゃないときは、特性リーフガードチェックをしない
	IF				IF_FLAG_NBIT,BUF_PARA_FIELD_CONDITION,FIELD_CONDITION_HARE_ALL,SoubiItemCheckNext
	//特性リーフガードを持っているときは、失敗する（かたやぶりチェックのために最上位）
	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_RIIHUGAADO,SEQ_047_END
SoubiItemCheckNext:
	//特性りんぷんを持っているときは、失敗する（かたやぶりチェックのために最上位）とりあえず無し
//	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_RINPUN,Umakukimaran
//すでにどくを受けている場合は、失敗する
	IF_PSP			IF_FLAG_BIT,SIDE_TSUIKA,ID_PSP_condition,CONDITION_DOKU,SEQ_047_END
	IF_PSP			IF_FLAG_BIT,SIDE_TSUIKA,ID_PSP_condition,CONDITION_DOKUDOKU,SEQ_047_END
//どくタイプ、はがねタイプには効果がない
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type1,POISON_TYPE,SEQ_047_END
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type2,POISON_TYPE,SEQ_047_END
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type1,METAL_TYPE,SEQ_047_END
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type2,METAL_TYPE,SEQ_047_END
//すでに状態異常になっている場合は、失敗する
	IF_PSP			IF_FLAG_NE,SIDE_TSUIKA,ID_PSP_condition,0,SEQ_047_END
//しんぴのまもりで守られている場合は、失敗する
	IF				IF_FLAG_BIT,BUF_PARA_SIDE_CONDITION_TSUIKA,SIDE_CONDITION_SHINPI,SEQ_047_END
	BRANCH			NoWazaEffect
NomalCheck:
	//特性めんえきを持っているときは、失敗する（かたやぶりチェックのために最上位）
	KATAYABURI_TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_MENEKI,TokuseiNoDoku
	//天候無効系の特性は、天候チェックを無視
	NOOTENKI_CHECK				SUB_047_NEXT
	//天候がはれじゃないときは、特性リーフガードチェックをしない
	IF							IF_FLAG_NBIT,BUF_PARA_FIELD_CONDITION,FIELD_CONDITION_HARE_ALL,SUB_047_NEXT
	//特性リーフガードを持っているときは、失敗する（かたやぶりチェックのために最上位）
	KATAYABURI_TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_RIIHUGAADO,TokuseiNoDoku
SUB_047_NEXT:
	//間接追加の場合、りんぷんチェックをする
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,NoRinpun
	//特性りんぷんを持っているときは、失敗する（かたやぶりチェックのために最上位）
	KATAYABURI_TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_RINPUN,Umakukimaran
NoRinpun:
//直接追加の場合、WAZAOUTシーケンスでメッセージを出さないので、ここで出す
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DIRECT,NoAttackMsg
	ATTACK_MESSAGE
	SERVER_WAIT
NoAttackMsg:

//みがわりを出されているときは、失敗する
	MIGAWARI_CHECK	SIDE_TSUIKA,Umakukimaran

//すでにどくを受けている場合は、失敗する
	IF_PSP			IF_FLAG_BIT,SIDE_TSUIKA,ID_PSP_condition,CONDITION_DOKU,AlreadyDoku
	IF_PSP			IF_FLAG_BIT,SIDE_TSUIKA,ID_PSP_condition,CONDITION_DOKUDOKU,AlreadyDoku

//どくタイプ、はがねタイプには効果がない
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type1,POISON_TYPE,Koukanai
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type2,POISON_TYPE,Koukanai
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type1,METAL_TYPE,Koukanai
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type2,METAL_TYPE,Koukanai

//すでに状態異常になっている場合は、失敗する
	IF_PSP			IF_FLAG_NE,SIDE_TSUIKA,ID_PSP_condition,0,Umakukimaran

//わざがはずれているときは、うまくきまらんにする
	IF				IF_FLAG_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_NOHIT_CHG,Umakukimaran

//しんぴのまもりで守られている場合は、失敗する
	IF				IF_FLAG_BIT,BUF_PARA_SIDE_CONDITION_TSUIKA,SIDE_CONDITION_SHINPI,ShinpiNoDoku

//直接追加の場合、WAZAOUTシーケンスで技エフェクトを出さないので、ここで出す
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DIRECT,NoWazaEffect
	WAZA_EFFECT		SIDE_ATTACK
	SERVER_WAIT
NoWazaEffect:
//装備アイテム効果の時は、アイテム発動エフェクトを出す
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_SOUBIITEM,NoItemEffect
	STATUS_EFFECT	SIDE_TSUIKA,STATUS_ITEM_POKE
	SERVER_WAIT
	WAIT			MSG_WAIT/2
NoItemEffect:
	STATUS_EFFECT	SIDE_TSUIKA,STATUS_DOKU
	SERVER_WAIT
	//どくどくフラグを立てる
	PSP_VALUE		VAL_BIT,SIDE_TSUIKA,ID_PSP_condition,CONDITION_DOKUDOKU
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_SOUBIITEM,SoubiItemDokuDokuMsg
NormalDokuDokuMsg:
	MESSAGE			DokuDokuMineMsg,TAG_NICK,SIDE_TSUIKA
	BRANCH			MessageNext
SoubiItemDokuDokuMsg:
	MESSAGE			ItemDokuDokuMineMsg,TAG_NICK_ITEM,SIDE_TSUIKA,SIDE_WORK
MessageNext:
	SERVER_WAIT
	STATUS_SET		SIDE_TSUIKA,STATUS_DOKU
	WAIT			MSG_WAIT
	//特性シンクロのために追加効果があったフラグを立てる
	IF				IF_FLAG_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_SINKURO_TSUIKA,FlagOff
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_SINKURO_TSUIKA
	BRANCH			SEQ_047_END
FlagOff:
	VALUE			VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_SINKURO_TSUIKA
SEQ_047_END:
	SEQ_END

//特性でどくを防ぐ
TokuseiNoDoku:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_047_RET
//技効果での追加の時はAttackMessageの必要なし
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_WAZA_KOUKA,TokuseiNoAttackMsg
//どくびしでの追加の時はAttackMessageの必要なし
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DOKUBISI,TokuseiNoAttackMsg
	ATTACK_MESSAGE
	SERVER_WAIT
TokuseiNoAttackMsg:
	WAIT			MSG_WAIT
	MESSAGE			MenekiMineMsg,TAG_NICK_TOKU,SIDE_TSUIKA,SIDE_TSUIKA
	BRANCH			SUB_047_END

//うまくきまらないとき
Umakukimaran:
//間接追加、技効果での追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_047_RET
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_WAZA_KOUKA,SUB_047_RET
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DOKUBISI,SUB_047_RET
	WAIT			MSG_WAIT
	GOSUB			SUB_SEQ_UMAKUKIMARAN
	BRANCH			SUB_047_RET

//すでにどくを受けているとき
AlreadyDoku:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_047_RET
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DOKUBISI,SUB_047_RET
	WAIT			MSG_WAIT
	MESSAGE			AlreadyDokuMineMsg,TAG_NICK,SIDE_TSUIKA
	BRANCH			SUB_047_END

//こうかがないとき
Koukanai:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_047_RET
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DOKUBISI,SUB_047_RET
	WAIT			MSG_WAIT
	MESSAGE			KoukanaiMineMsg,TAG_NICK,SIDE_TSUIKA
	BRANCH			SUB_047_END

//しんぴのまもりで防がれたとき
ShinpiNoDoku:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_047_RET
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_DOKUBISI,SUB_047_RET
	WAIT			MSG_WAIT
	MESSAGE			ShinpiGuardMineMsg,TAG_NICK,SIDE_TSUIKA
SUB_047_END:
	SERVER_WAIT
	WAIT			MSG_WAIT
	//WazaStatusMessageを無効にするためにこのフラグを立てる
	//VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI_RENZOKU_CHECK
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI
SUB_047_RET:
	SEQ_END

