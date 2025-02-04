
//============================================================================================
/**
 * @file	poke_tool_def.h
 * @bfief	ポケモンパラメータツール郡（WB改訂版）
 * @author	HisashiSogabe
 * @date	08.11.12
 */
//============================================================================================

#include  "item/itemsym.h"
#include  "savedata/mail.h"
#define	EOM_				( 0xff )

#ifndef __POKE_TOOL_DEF_H_
#define __POKE_TOOL_DEF_H_

#include "pm_define.h"

#define	POKE_PARA_BLOCK_MAX		( 4 )		//POKEMON_PARAM構造体の分割ブロック数
#define	GROW_TBL_SIZE			( 101 )		//成長テーブルのサイズ
#define MAIL_DATA_SIZE    ( 56 )

struct pokemon_paso_param1
{
	u16	monsno;								//02h	モンスターナンバー
	u16 item;								//04h	所持アイテムナンバー
	u32	id_no;								//08h	IDNo
	u32	exp;								//0ch	経験値
	u8	friend;								//0dh	なつき度
	u8	speabino;							//0eh	特殊能力
	u8	mark;								//0fh	ポケモンにつけるマーク（ボックス）
	u8	country_code;						//10h	国コード
	u8	hp_exp;								//11h	HP努力値
	u8	pow_exp;							//12h	攻撃力努力値
	u8	def_exp;							//13h	防御力努力値
	u8	agi_exp;							//14h	素早さ努力値
	u8	spepow_exp;							//15h	特攻努力値
	u8	spedef_exp;							//16h	特防努力値
	u8	style;								//17h	かっこよさ
	u8	beautiful;							//18h	うつくしさ
	u8	cute;								//19h	かわいさ
	u8	clever;								//1ah	かしこさ
	u8	strong;								//1bh	たくましさ
	u8	fur;								//1ch	毛艶
	u32	sinou_ribbon;						//20h	シンオウ系リボン（ビットフィールドでアクセスするとプログラムメモリを消費するので、ビットシフトに変更）
//以下内訳
#if 0
	u32	sinou_champ_ribbon					:1;	//シンオウチャンプリボン
	u32	sinou_battle_tower_ttwin_first		:1;	//シンオウバトルタワータワータイクーン勝利1回目
	u32	sinou_battle_tower_ttwin_second		:1;	//シンオウバトルタワータワータイクーン勝利1回目
	u32	sinou_battle_tower_2vs2_win50		:1;	//シンオウバトルタワータワーダブル50連勝
	u32	sinou_battle_tower_aimulti_win50	:1;	//シンオウバトルタワータワーAIマルチ50連勝
	u32	sinou_battle_tower_siomulti_win50	:1;	//シンオウバトルタワータワー通信マルチ50連勝
	u32	sinou_battle_tower_wifi_rank5		:1;	//シンオウバトルタワーWifiランク５入り
	u32	sinou_syakki_ribbon					:1;	//シンオウしゃっきリボン
	u32	sinou_dokki_ribbon					:1;	//シンオウどっきリボン
	u32	sinou_syonbo_ribbon					:1;	//シンオウしょんぼリボン

	u32	sinou_ukka_ribbon					:1;	//シンオウうっかリボン
	u32	sinou_sukki_ribbon					:1;	//シンオウすっきリボン
	u32	sinou_gussu_ribbon					:1;	//シンオウぐっすリボン
	u32	sinou_nikko_ribbon					:1;	//シンオウにっこリボン
	u32	sinou_gorgeous_ribbon				:1;	//シンオウゴージャスリボン
	u32	sinou_royal_ribbon					:1;	//シンオウロイヤルリボン
	u32	sinou_gorgeousroyal_ribbon			:1;	//シンオウゴージャスロイヤルリボン
	u32	sinou_ashiato_ribbon				:1;	//シンオウあしあとリボン
	u32	sinou_record_ribbon					:1;	//シンオウレコードリボン
	u32	sinou_history_ribbon				:1;	//シンオウヒストリーリボン

	u32	sinou_legend_ribbon					:1;	//シンオウレジェンドリボン
	u32	sinou_red_ribbon					:1;	//シンオウレッドリボン
	u32	sinou_green_ribbon					:1;	//シンオウグリーンリボン
	u32	sinou_blue_ribbon					:1;	//シンオウブルーリボン
	u32	sinou_festival_ribbon				:1;	//シンオウフェスティバルリボン
	u32	sinou_carnival_ribbon				:1;	//シンオウカーニバルリボン
	u32	sinou_classic_ribbon				:1;	//シンオウクラシックリボン
	u32	sinou_premiere_ribbon				:1;	//シンオウプレミアリボン

	u32	sinou_amari_ribbon					:4;	//20h	あまり
#endif
};
	
struct pokemon_paso_param2
{
	u16	waza[PTL_WAZA_MAX];					//08h	所持技
	u8	pp[PTL_WAZA_MAX];					//0ch	所持技PP
	u8	pp_count[PTL_WAZA_MAX];				//10h	所持技PP_COUNT
	u32	hp_rnd			:5;					//		HP乱数
	u32	pow_rnd			:5;					//		攻撃力乱数
	u32	def_rnd			:5;					//		防御力乱数
	u32	agi_rnd			:5;					//		素早さ乱数
	u32	spepow_rnd		:5;					//		特攻乱数
	u32	spedef_rnd		:5;					//		特防乱数
	u32	tamago_flag		:1;					//		タマゴフラグ（0:タマゴじゃない　1:タマゴだよ）
	u32	nickname_flag	:1;					//14h	ニックネームをつけたかどうかフラグ（0:つけていない　1:つけた）

	u32	old_ribbon;							//18h	過去作のリボン系（ビットフィールドでアクセスするとプログラムメモリを消費するので、ビットシフトに変更）
//以下内訳
#if 0
	u32	stylemedal_normal		:1;			//	かっこよさ勲章(ノーマル)(AGBコンテスト)
	u32	stylemedal_super		:1;			//	かっこよさ勲章(スーパー)(AGBコンテスト)
	u32	stylemedal_hyper		:1;			//	かっこよさ勲章(ハイパー)(AGBコンテスト)
	u32	stylemedal_master		:1;			//	かっこよさ勲章(マスター)(AGBコンテスト)
	u32	beautifulmedal_normal	:1;			//	うつくしさ勲章(ノーマル)(AGBコンテスト)
	u32	beautifulmedal_super	:1;			//	うつくしさ勲章(スーパー)(AGBコンテスト)
	u32	beautifulmedal_hyper	:1;			//	うつくしさ勲章(ハイパー)(AGBコンテスト)
	u32	beautifulmedal_master	:1;			//	うつくしさ勲章(マスター)(AGBコンテスト)
	u32	cutemedal_normal		:1;			//	かわいさ勲章(ノーマル)(AGBコンテスト)
	u32	cutemedal_super			:1;			//	かわいさ勲章(スーパー)(AGBコンテスト)
	u32	cutemedal_hyper			:1;			//	かわいさ勲章(ハイパー)(AGBコンテスト)
	u32	cutemedal_master		:1;			//	かわいさ勲章(マスター)(AGBコンテスト)
	u32	clevermedal_normal		:1;			//	かしこさ勲章(ノーマル)(AGBコンテスト)
	u32	clevermedal_super		:1;			//	かしこさ勲章(スーパー)(AGBコンテスト)
	u32	clevermedal_hyper		:1;			//	かしこさ勲章(ハイパー)(AGBコンテスト)
	u32	clevermedal_master		:1;			//	かしこさ勲章(マスター)(AGBコンテスト)
	u32	strongmedal_normal		:1;			//	たくましさ勲章(ノーマル)(AGBコンテスト)
	u32	strongmedal_super		:1;			//	たくましさ勲章(スーパー)(AGBコンテスト)
	u32	strongmedal_hyper		:1;			//	たくましさ勲章(ハイパー)(AGBコンテスト)
	u32	strongmedal_master		:1;			//	たくましさ勲章(マスター)(AGBコンテスト)

	u32	champ_ribbon	:1;					//		チャンプリボン
	u32	winning_ribbon	:1;					//		ウィニングリボン
	u32	victory_ribbon	:1;					//		ビクトリーリボン
	u32	bromide_ribbon	:1;					//		ブロマイドリボン
	u32	ganba_ribbon	:1;					//		がんばリボン
	u32	marine_ribbon	:1;					//		マリンリボン
	u32	land_ribbon		:1;					//		ランドリボン
	u32	sky_ribbon		:1;					//		スカイリボン
	u32	country_ribbon	:1;					//		カントリーリボン
	u32	national_ribbon	:1;					//		ナショナルリボン
	u32	earth_ribbon	:1;					//		アースリボン
	u32	world_ribbon	:1;					//		ワールドリボン
#endif

	u8	event_get_flag	:1;					//		イベントで配布したことを示すフラグ
	u8	sex				:2;					//   	ポケモンの性別
	u8	form_no			:5;					//19h	形状ナンバー（アンノーン、デオキシス、ミノメスなど用）
	u8	seikaku;							//1ah	性格
  u16 tokusei_3_flag  :1;     //1ch 第3特性フラグ
  u16                 :15;
	u32	dummy_p2_3;							//20h	あまり
};
	
struct pokemon_paso_param3
{
	STRCODE	nickname[MONS_NAME_SIZE+EOM_SIZE];	//16h	ニックネーム(MONS_NAME_SIZE=10)+(EOM_SIZE=1)=11
	u8	pref_code;								//18h	都道府県コード
	u8	get_cassette;							//		捕まえたカセットバージョン
	u64	new_ribbon;								//20h	新リボン系（ビットフィールドでアクセスするとプログラムメモリを消費するので、ビットシフトに変更）
//以下内訳
#if 0
	u32	trial_stylemedal_normal			:1;		//	かっこよさ勲章(ノーマル)(トライアル)
	u32	trial_stylemedal_super			:1;		//	かっこよさ勲章(スーパー)(トライアル)
	u32	trial_stylemedal_hyper			:1;		//	かっこよさ勲章(ハイパー)(トライアル)
	u32	trial_stylemedal_master			:1;		//	かっこよさ勲章(マスター)(トライアル)
	u32	trial_beautifulmedal_normal		:1;		//	うつくしさ勲章(ノーマル)(トライアル)
	u32	trial_beautifulmedal_super		:1;		//	うつくしさ勲章(スーパー)(トライアル)
	u32	trial_beautifulmedal_hyper		:1;		//	うつくしさ勲章(ハイパー)(トライアル)
	u32	trial_beautifulmedal_master		:1;		//	うつくしさ勲章(マスター)(トライアル)
	u32	trial_cutemedal_normal			:1;		//	かわいさ勲章(ノーマル)(トライアル)
	u32	trial_cutemedal_super			:1;		//	かわいさ勲章(スーパー)(トライアル)
	u32	trial_cutemedal_hyper			:1;		//	かわいさ勲章(ハイパー)(トライアル)
	u32	trial_cutemedal_master			:1;		//	かわいさ勲章(マスター)(トライアル)
	u32	trial_clevermedal_normal		:1;		//	かしこさ勲章(ノーマル)(トライアル)
	u32	trial_clevermedal_super			:1;		//	かしこさ勲章(スーパー)(トライアル)
	u32	trial_clevermedal_hyper			:1;		//	かしこさ勲章(ハイパー)(トライアル)
	u32	trial_clevermedal_master		:1;		//	かしこさ勲章(マスター)(トライアル)
	u32	trial_strongmedal_normal		:1;		//	たくましさ勲章(ノーマル)(トライアル)
	u32	trial_strongmedal_super			:1;		//	たくましさ勲章(スーパー)(トライアル)
	u32	trial_strongmedal_hyper			:1;		//	たくましさ勲章(ハイパー)(トライアル)
	u32	trial_strongmedal_master		:1;		//	たくましさ勲章(マスター)(トライアル)
	u32 amari_ribbon					:12;	//20h	あまり
	u32	amari;									//20h	あまり
#endif
};
	
struct pokemon_paso_param4
{
	STRCODE	oyaname[7+EOM_SIZE];				//10h	親の名前(PERSON_NAME_SIZE=7)+(EOM_SIZE_=1)=8*2(STRCODE=u16)

	u8	get_year;								//11h	捕まえた年
	u8	get_month;								//12h	捕まえた月
	u8	get_day;								//13h	捕まえた日
	u8	birth_year;								//14h	生まれた年

	u8	birth_month;							//15h	生まれた月
	u8	birth_day;								//16h	生まれた日
	u16	get_place;								//18h	捕まえた場所

	u16	birth_place;							//1ah	生まれた場所
	u8	pokerus;								//1bh	ポケルス
	u8	get_ball;								//1ch	捕まえたボール

	u8	get_level		:7;						//1dh	捕まえたレベル
	u8	oyasex			:1;						//1dh	親の性別
	u8	get_ground_id;							//1eh	捕まえた場所の地形アトリビュート（ミノッチ用）
	u16	dummy_p4_1;								//20h	あまり
};

struct pokemon_para_data11
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data12
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data13
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data14
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data15
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data16
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data21
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data22
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data23
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data24
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data25
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data26
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data31
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data32
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data33
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data34
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data35
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data36
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data41
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data42
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data43
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data44
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data45
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data46
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
};

///<ボックスポケモン分の構造体宣言
struct pokemon_paso_param
{
	u32	personal_rnd;							//04h	個性乱数
	u16	pp_fast_mode		:1;					//06h	暗号／復号／チェックサム生成を後回しにして、処理を高速化モード
	u16	ppp_fast_mode		:1;					//06h	暗号／復号／チェックサム生成を後回しにして、処理を高速化モード
	u16	fusei_tamago_flag	:1;					//06h	ダメタマゴフラグ
	u16						:13;				//06h
	u16	checksum;								//08h	チェックサム

	u8	paradata[sizeof(POKEMON_PASO_PARAM1)+
				 sizeof(POKEMON_PASO_PARAM2)+
				 sizeof(POKEMON_PASO_PARAM3)+
				 sizeof(POKEMON_PASO_PARAM4)];	//88h
};

///<ボックスポケモン分以外の構造体宣言
struct pokemon_calc_param
{
	u32 condition;					          //04h	コンディション
	u8  level;							          //05h	レベル
	u8	cb_id;							          //06h	カスタムボールID
	u16	hp;								 	          //08h	HP
	u16	hpmax;							          //0ah	HPMAX
	u16	pow;								          //0ch	攻撃力
	u16	def;								          //0eh	防御力
	u16	agi;								          //10h	素早さ
	u16	spepow;							          //12h	特攻
	u16	spedef;							          //14h	特防
	u8	mail_data[ MAIL_DATA_SIZE ];  //4ch	ポケモンにもたせるメールデータ（56byte）
	u8	padding[ 8 ];                 //POKEMON_PARAM合計で1体220バイト
};

///<手持ちポケモンの構造体宣言
struct	pokemon_param
{
	POKEMON_PASO_PARAM	ppp;				//88h
	POKEMON_CALC_PARAM	pcp;				//dch 220
};

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * 対戦録画用にカスタマイズされたポケモンパラメータ
 */
//--------------------------------------------------------------
struct rec_pokepara{
	//--------------------------------------------------------------
	//	pokemon_paso_param
	//--------------------------------------------------------------
	u32	personal_rnd;							//04h	個性乱数
	u16	pp_fast_mode		:1;					//06h	暗号／復号／チェックサム生成を後回しにして、処理を高速化モード
	u16	ppp_fast_mode		:1;					//06h	暗号／復号／チェックサム生成を後回しにして、処理を高速化モード
	u16	fusei_tamago_flag	:1;					//06h	ダメタマゴフラグ
	u16 seikaku:8;                  // 性格
	u16           :5;        //06h
	
	//--------------------------------------------------------------
	//	pokemon_paso_param1
	//--------------------------------------------------------------
	u16	monsno;								//02h	モンスターナンバー
	u16 item;								//04h	所持アイテムナンバー
	u32	id_no;								//08h	IDNo
	u32	exp;								//0ch	経験値
	u8	friend;								//0dh	なつき度
	u8	speabino;							//0eh	特殊能力
//	u8	mark;								//0fh	ポケモンにつけるマーク（ボックス）
//	u8	country_code;						//10h	国コード
	u8	hp_exp;								//11h	HP努力値
	u8	pow_exp;							//12h	攻撃力努力値
	u8	def_exp;							//13h	防御力努力値
	u8	agi_exp;							//14h	素早さ努力値
	u8	spepow_exp;							//15h	特攻努力値
	u8	spedef_exp;							//16h	特防努力値
//	u8	style;								//17h	かっこよさ
//	u8	beautiful;							//18h	うつくしさ
//	u8	cute;								//19h	かわいさ
//	u8	clever;								//1ah	かしこさ
//	u8	strong;								//1bh	たくましさ
//	u8	fur;								//1ch	毛艶
//	u32	sinou_ribbon;						//20h	シンオウ系リボン

	//--------------------------------------------------------------
	//	pokemon_paso_param2
	//--------------------------------------------------------------
	u16	waza[PTL_WAZA_MAX];				//08h	所持技
	u8	pp[PTL_WAZA_MAX];				//0ch	所持技PP
	u8	pp_count[PTL_WAZA_MAX];			//10h	所持技PP_COUNT
	u32	hp_rnd			:5;					//		HP乱数
	u32	pow_rnd			:5;					//		攻撃力乱数
	u32	def_rnd			:5;					//		防御力乱数
	u32	agi_rnd			:5;					//		素早さ乱数
	u32	spepow_rnd		:5;					//		特攻乱数
	u32	spedef_rnd		:5;					//		特防乱数
	u32	tamago_flag		:1;					//		タマゴフラグ（0:タマゴじゃない　1:タマゴだよ）
	u32	nickname_flag	:1;					//14h	ニックネームをつけたかどうかフラグ
//	u32	old_ribbon;							//18h	過去作のリボン系
	u8	event_get_flag	:1;					//		イベントで配布したことを示すフラグ
	u8	sex				:2;					//   	ポケモンの性別
	u8	form_no			:5;					//19h	形状ナンバー
//	u8	dummy_p2_1;							//1ah	あまり
//	u16	dummy_p2_2;							//1ch	あまり
//	u32	dummy_p2_3;							//20h	あまり
	
	//--------------------------------------------------------------
	//	pokemon_paso_param3
	//--------------------------------------------------------------
	STRCODE	nickname[MONS_NAME_SIZE+EOM_SIZE];	//16h	ニックネーム
//	u8	pref_code;								//18h	都道府県コード
//	u8	get_cassette;							//		捕まえたカセットバージョン
//	u64	new_ribbon;								//20h	新リボン系
	
	//--------------------------------------------------------------
	//	pokemon_paso_param4
	//--------------------------------------------------------------
	STRCODE	oyaname[PERSON_NAME_SIZE+EOM_SIZE];	//10h	親の名前
//	u8	get_year;								//11h	捕まえた年
//	u8	get_month;								//12h	捕まえた月
//	u8	get_day;								//13h	捕まえた日
//	u8	birth_year;								//14h	生まれた年
//	u8	birth_month;							//15h	生まれた月
//	u8	birth_day;								//16h	生まれた日
//	u16	get_place;								//18h	捕まえた場所
//	u16	birth_place;							//1ah	生まれた場所
//	u8	pokerus;								//1bh	ポケルス
	u8	get_ball;								//1ch	捕まえたボール
//	u8	get_level		:7;						//1dh	捕まえたレベル
//	u8	oyasex			:1;						//1dh	親の性別
//	u8	get_ground_id;							//1eh	捕まえた場所の地形アトリビュート
//	u16	dummy_p4_1;								//20h	あまり
	u8  language;								//言語コード
	
	//--------------------------------------------------------------
	//	pokemon_calc_param
	//--------------------------------------------------------------
	u32			condition;							//04h	コンディション
	u8			level;								//05h	レベル
	u8			cb_id;								//06h	カスタムボールID
	u16			hp;									//08h	HP
	u16			hpmax;								//0ah	HPMAX
	u16			pow;								//0ch	攻撃力
	u16			def;								//0eh	防御力
	u16			agi;								//10h	素早さ
	u16			spepow;								//12h	特攻
	u16			spedef;								//14h	特防
//	_MAIL_DATA	mail_data;							//3ch	ポケモンにもたせるメールデータ
//	CB_CORE		cb_core;							//54h	カスタムボール
	
};

#endif __POKE_TOOL_DEF_H_

