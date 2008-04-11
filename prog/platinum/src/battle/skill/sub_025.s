
//============================================================================
/**
 *
 *@file		sub_025.s
 *@brief	戦闘シーケンス
 *			やけどの追加効果シーケンス
 *@author	HisashiSogabe
 *@data		2005.12.05
 *
 */
//============================================================================
	.text

	.include	"waza_seq_def.h"

SUB_025:
	//装備アイテムでの追加の時は専用のチェックをする
	IF				IF_FLAG_NE,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_SOUBIITEM,NormalCheck
SoubiItemCheck:
	//特性みずのベールを持っているときは、失敗する（かたやぶりチェックのために最上位）
	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_MIZUNOBEERU,SEQ_025_END
	//天候無効系の特性は、天候チェックを無視
	NOOTENKI_CHECK	SoubiItemCheckNext
	//天候がはれじゃないときは、特性リーフガードチェックをしない
	IF				IF_FLAG_NBIT,BUF_PARA_FIELD_CONDITION,FIELD_CONDITION_HARE_ALL,SoubiItemCheckNext
	//特性リーフガードを持っているときは、失敗する（かたやぶりチェックのために最上位）
	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_RIIHUGAADO,SEQ_025_END
SoubiItemCheckNext:
	//特性りんぷんを持っているときは、失敗する（かたやぶりチェックのために最上位）
//	TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_RINPUN,SEQ_025_END
//すでにやけどを負っている場合は、失敗する
	IF_PSP			IF_FLAG_BIT,SIDE_TSUIKA,ID_PSP_condition,CONDITION_YAKEDO,SEQ_025_END
//ほのおタイプには効果がない
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type1,FIRE_TYPE,SEQ_025_END
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type2,FIRE_TYPE,SEQ_025_END
//すでに状態異常になっている場合は、失敗する
	IF_PSP			IF_FLAG_NE,SIDE_TSUIKA,ID_PSP_condition,0,SEQ_025_END
//しんぴのまもりで守られている場合は、失敗する
	IF				IF_FLAG_BIT,BUF_PARA_SIDE_CONDITION_TSUIKA,SIDE_CONDITION_SHINPI,SEQ_025_END
	BRANCH			NoWazaEffect
NormalCheck:
	//特性みずのベールを持っているときは、失敗する（かたやぶりチェックのために最上位）
	KATAYABURI_TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_MIZUNOBEERU,TokuseiNoYakedo
	//天候無効系の特性は、天候チェックを無視
	NOOTENKI_CHECK	SUB_025_NEXT
	//天候がはれじゃないときは、特性リーフガードチェックをしない
	IF							IF_FLAG_NBIT,BUF_PARA_FIELD_CONDITION,FIELD_CONDITION_HARE_ALL,SUB_025_NEXT
	//特性リーフガードを持っているときは、失敗する（かたやぶりチェックのために最上位）
	KATAYABURI_TOKUSEI_CHECK	TOKUSEI_HAVE,SIDE_TSUIKA,TOKUSYU_RIIHUGAADO,TokuseiNoYakedo
SUB_025_NEXT:
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

//特性追加の場合、みがわりチェックなし
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI,NoMigawari
//みがわりを出されているときは、失敗する
	MIGAWARI_CHECK	SIDE_TSUIKA,Umakukimaran

NoMigawari:
//すでにやけどを負っている場合は、失敗する
	IF_PSP			IF_FLAG_BIT,SIDE_TSUIKA,ID_PSP_condition,CONDITION_YAKEDO,AlreadyYakedo

//ほのおタイプには効果がない
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type1,FIRE_TYPE,Koukanai
	IF_PSP			IF_FLAG_EQ,SIDE_TSUIKA,ID_PSP_type2,FIRE_TYPE,Koukanai

//すでに状態異常になっている場合は、失敗する
	IF_PSP			IF_FLAG_NE,SIDE_TSUIKA,ID_PSP_condition,0,Umakukimaran

//特性追加の場合、以下のチェックは必要なし
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI,NoWazaEffect

//わざがはずれているときは、うまくきまらんにする
	IF				IF_FLAG_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_NOHIT_CHG,Umakukimaran

//しんぴのまもりで守られている場合は、失敗する
	IF				IF_FLAG_BIT,BUF_PARA_SIDE_CONDITION_TSUIKA,SIDE_CONDITION_SHINPI,ShinpiNoYakedo

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
	STATUS_EFFECT	SIDE_TSUIKA,STATUS_YAKEDO
	SERVER_WAIT
	//やけどフラグを立てる
	PSP_VALUE		VAL_BIT,SIDE_TSUIKA,ID_PSP_condition,CONDITION_YAKEDO
//特性追加の場合、専用メッセージへ
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI,TokuseiYakedoMsg
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_SOUBIITEM,ItemYakedoMsg
NormalMsg:
	MESSAGE			YakedoMineMsg,TAG_NICK,SIDE_TSUIKA
	BRANCH			MsgAfter
TokuseiYakedoMsg:
	MESSAGE			TokuseiYakedoM2MMsg,TAG_NICK_TOKU_NICK,SIDE_WORK,SIDE_CLIENT_WORK,SIDE_TSUIKA
	BRANCH			MsgAfter
ItemYakedoMsg:
	MESSAGE			ItemYakedoMineMsg,TAG_NICK_ITEM,SIDE_TSUIKA,SIDE_WORK
MsgAfter:
	SERVER_WAIT
	STATUS_SET		SIDE_TSUIKA,STATUS_YAKEDO
	WAIT			MSG_WAIT
	//特性シンクロのために追加効果があったフラグを立てる
	IF				IF_FLAG_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_SINKURO_TSUIKA,FlagOff
	VALUE			VAL_BIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_SINKURO_TSUIKA
	BRANCH			SEQ_025_END
FlagOff:
	VALUE			VAL_NBIT,BUF_PARA_SERVER_STATUS_FLAG,SERVER_STATUS_FLAG_SINKURO_TSUIKA
SEQ_025_END:
	SEQ_END

//うまくきまらないとき
Umakukimaran:
//間接追加、特性追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_025_RET
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI,SUB_025_RET
	WAIT			MSG_WAIT
	GOSUB			SUB_SEQ_UMAKUKIMARAN
	BRANCH			SUB_025_RET

//すでにやけどを負っているとき
AlreadyYakedo:
//間接追加、特性追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_025_RET
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI,SUB_025_RET
	WAIT			MSG_WAIT
	MESSAGE			AlreadyYakedoMineMsg,TAG_NICK,SIDE_TSUIKA
	BRANCH			SUB_025_END

//こうかがないとき
Koukanai:
//間接追加、特性追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_025_RET
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI,SUB_025_RET
	WAIT			MSG_WAIT
//特性追加の場合、専用メッセージへ（この仕様はなくなりました）
//	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI,TokuseiKoukanai
	MESSAGE			KoukanaiMineMsg,TAG_NICK,SIDE_TSUIKA
	BRANCH			SUB_025_END
//TokuseiKoukanai:
//	MESSAGE			TokuseiNoTypeM2MMsg,TAG_NICK_TOKU_NICK,SIDE_WORK,SIDE_CLIENT_WORK,SIDE_TSUIKA
//	BRANCH			SUB_025_END

//特性でやけどを防ぐ
TokuseiNoYakedo:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_025_RET
//特性追加の場合、メッセージを出さない
//	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI,TokuseiNoYakedo2
//特性追加の場合、専用メッセージへ
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_TOKUSEI,SUB_025_RET
	ATTACK_MESSAGE
	SERVER_WAIT
	WAIT			MSG_WAIT
	MESSAGE			MizunobeeruMineMsg,TAG_NICK_TOKU,SIDE_TSUIKA,SIDE_TSUIKA
	BRANCH			SUB_025_END
TokuseiNoYakedo2:
	MESSAGE			TokuseiNoTokuseiM2MMsg,TAG_NICK_TOKU_NICK_TOKU,SIDE_TSUIKA,SIDE_TSUIKA,SIDE_WORK,SIDE_CLIENT_WORK
	BRANCH			SUB_025_END


//しんぴのまもりで防がれたとき
ShinpiNoYakedo:
//間接追加の場合、メッセージを出さない
	IF				IF_FLAG_EQ,BUF_PARA_TSUIKA_TYPE,ADD_STATUS_INDIRECT,SUB_025_RET
	WAIT			MSG_WAIT
	MESSAGE			ShinpiGuardMineMsg,TAG_NICK,SIDE_TSUIKA
SUB_025_END:
	SERVER_WAIT
	WAIT			MSG_WAIT
	//WazaStatusMessageを無効にするためにこのフラグを立てる
//	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI_RENZOKU_CHECK
	VALUE			VAL_BIT,BUF_PARA_WAZA_STATUS_FLAG,WAZA_STATUS_FLAG_SIPPAI
SUB_025_RET:
	SEQ_END

