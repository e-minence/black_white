//=============================================================================
/**
 * @file	  pokemontrade_anim.h
 * @bfief	  ポケモン交換 アニメーション間隔定義
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/08/28
 */
//=============================================================================

#pragma once

#include "pokemontrade_local.h"


//時間に関する事
#define _POKEMON_CENTER_TIME   (42)  ///ポケモンが真ん中による時間


#define _POKEMON_VOICE_TIME   (200)  ///ポケモンが声を出している時間


//フェードに関する時間
#define _POKEUP_WHITEOUT_START (266)       //絵コンテ６番モンスターボールが打ちあがった際のホワイトアウト
#define _POKEUP_WHITEOUT_TIMER (20)       //絵コンテ６番モンスターボールが打ちあがった際のホワイトアウト時間 1/
#define _POKEUP_WHITEIN_START  (301)      //絵コンテ７番ポケモン交換シーン前のフェードイン
#define _POKEUP_WHITEIN_TIMER (25)        //絵コンテ７番ポケモン交換シーン前のフェードイン時間

#define _POKECHANGE_WHITEOUT_START (651)  //絵コンテ１４番ポケモン交換シーンのホワイトアウト
#define _POKECHANGE_WHITEOUT_TIMER (10)      //絵コンテ１４番ポケモン交換シーンのホワイトアウト時間
#define _POKECHANGE_WHITEIN_START  (693)  //絵コンテ１５番ボールおち前のフェードイン
#define _POKECHANGE_WHITEIN_TIMER (25)        //絵コンテ１５番ボールおち前のフェードイン時間

#define _POKESPLASH_WHITEOUT_START (896)  //絵コンテ２０番ボールおちシーンのホワイトアウト
#define _POKESPLASH_WHITEOUT_TIMER (30)       //絵コンテ２０番ボールおちシーンのホワイトアウト時間
#define _POKESPLASH_WHITEIN_START  (970)  //絵コンテ２１番もう一回ボール落ちのフェードイン
#define _POKESPLASH_WHITEIN_TIMER (30)        //絵コンテ２１番もう一回ボール落ちのフェードイン時間

#define _DEMO_END (1218)   //デモが終わる最後のフレーム


//ポケモンの動きに関する時間
#define _POKEMON_DELETE_TIME   (4)     // 絵コンテ３番  ポケモンを消す

#define _POKE_APPEAR_START   (330)   //絵コンテ８番  ポケモン登場 自分のは裏
#define _POKE_APPEAR_TIME    (15)       //絵コンテ８番  ポケモン登場移動時間
#define _POKE_APPEAR_START2   (340)   //絵コンテ９番     さらにゆっくり移動
#define _POKE_APPEAR_TIME2   (28)       //絵コンテ９番  さらにゆっくり移動
#define _POKE_SIDEOUT_START   (368)  //絵コンテ１０番  ポケモン左右に移動していく
#define _POKE_SIDEOUT_TIME    (46)      //絵コンテ１０番  ポケモン登場移動時間


#define _POKE_SIDEIN_START   (434)   //絵コンテ１１番  ポケモン左右に移動して戻ってくる
#define _POKE_SIDEIN_TIME    (103)     //絵コンテ１１番 ポケモン登場移動時間

#define _POKE_SIDEIN_START2   (504)   //絵コンテ１１番  さらにポケモン左右に移動して戻ってくる
#define _POKE_SIDEIN_TIME2    (128)     //絵コンテ１１番 さらにポケモン登場移動時間

#define _POKE_LEAVE_START   (637)  //絵コンテ１３番  ポケモン退場




#define _POKE_LEAVE_TIME    (7)      //絵コンテ１３番  ポケモン退場移動時間

#define _POKEMON_CREATE_TIME   (1166)  ///絵コンテ２４番 交換終了してポケモン登場 


///パーティクルのタイミング
#define _BALL_PARTICLE_START   (854)  ///モンスターボール激突 パーティクル開始
#define _PARTICLE_DEMO2_START   (101)  ///demo_tex002パーティクル開始
#define _PARTICLE_DEMO3_START   (154)  ///demo_tex003パーティクル開始
#define _PARTICLE_DEMO3_START2  (854)  ///パーティクル開始
#define _PARTICLE_DEMO4_START   (14)     ///ポケモン＞ボールエフェクト
#define _PARTICLE_DEMO4_START2   (1155)  ///ボール＞ポケモンエフェクト
#define _PARTICLE_DEMO5_START   (154)  ///モンスターボールが打ちあがるパーティクル開始
#define _PARTICLE_DEMO6_START   (338)  ///パーティクル開始
#define _PARTICLE_DEMO7_START   (700)  ///パーティクル開始
#define _PARTICLE_DEMO8_START   (210)  ///パーティクル開始
#define _PARTICLE_DEMO9_START   (280)  ///パーティクル開始
#define _PARTICLE_DEM10_START   (350)  ///パーティクル開始

///OAM登場のタイミング
#define _OAM_POKECREATE_START   (14)  ///ポケモン＞ボールエフェクト
#define _OAM_POKECREATE_START2  (1151)  ///ボール＞ポケモンエフェクト




// 座標に関する事
//絵コンテ２番の移動の中心
#define _POKEMON_PLAYER_CENTER_POSX _MCSS_POS_X(  0)   // 自分のポケモンが真ん中に移動する際のX
#define _POKEMON_PLAYER_CENTER_POSY _MCSS_POS_Y(-23)   // 自分のポケモンが真ん中に移動する際のY
#define _POKEMON_PLAYER_CENTER_POSZ _MCSS_POS_Z( 10)   // 自分のポケモンが真ん中に移動する際のZ

#define _POKEMON_FRIEND_CENTER_POSX _MCSS_POS_X(  0)   // 相手のポケモンが真ん中に移動する際のX
#define _POKEMON_FRIEND_CENTER_POSY _MCSS_POS_Y(-23)   // 相手のポケモンが真ん中に移動する際のY
#define _POKEMON_FRIEND_CENTER_POSZ _MCSS_POS_Z(-10)   // 相手のポケモンが真ん中に移動する際のZ


//絵コンテ８番の最初の位置
#define _POKEMON_PLAYER_APPEAR_POSX _MCSS_POS_X( -50)  // 自分のポケモンのX
#define _POKEMON_PLAYER_APPEAR_POSY _MCSS_POS_Y(-150)  // 自分のポケモンのY
#define _POKEMON_PLAYER_APPEAR_POSZ _MCSS_POS_Z(  10)  // 自分のポケモンのZ
#define _POKEMON_FRIEND_APPEAR_POSX _MCSS_POS_X( 60)  // 相手のポケモンのX
#define _POKEMON_FRIEND_APPEAR_POSY _MCSS_POS_Y(80)  // 相手のポケモンのY
#define _POKEMON_FRIEND_APPEAR_POSZ _MCSS_POS_Z(-10)  // 相手のポケモンのZ

//絵コンテ９番の移動後の位置
#define _POKEMON_PLAYER_UP_POSX   _POKEMON_PLAYER_APPEAR_POSX  // 自分のポケモンのX
#define _POKEMON_PLAYER_UP_POSY   _MCSS_POS_Y(-55)             // 自分のポケモンのY
#define _POKEMON_PLAYER_UP_POSZ   _POKEMON_PLAYER_APPEAR_POSZ  // 自分のポケモンのZ
#define _POKEMON_FRIEND_DOWN_POSX _POKEMON_FRIEND_APPEAR_POSX  // 相手のポケモンのX
#define _POKEMON_FRIEND_DOWN_POSY _MCSS_POS_Y( 5)              // 相手のポケモンのY
#define _POKEMON_FRIEND_DOWN_POSZ _POKEMON_FRIEND_APPEAR_POSZ  // 相手のポケモンのZ


//絵コンテ１０番
#define _POKEMON_PLAYER_SIDEOUT_POSX _MCSS_POS_X(-150)           // 自分のポケモンのX
#define _POKEMON_PLAYER_SIDEOUT_POSY _POKEMON_PLAYER_UP_POSY    // 自分のポケモンのY
#define _POKEMON_PLAYER_SIDEOUT_POSZ _MCSS_POS_Z(0)    // 自分のポケモンのZ
#define _POKEMON_FRIEND_SIDEOUT_POSX _MCSS_POS_X(150)           // 相手のポケモンのX
#define _POKEMON_FRIEND_SIDEOUT_POSY _POKEMON_FRIEND_DOWN_POSY  // 相手のポケモンのY
#define _POKEMON_FRIEND_SIDEOUT_POSZ _MCSS_POS_Z(0)  // 相手のポケモンのZ

//絵コンテ１１番
#define _POKEMON_PLAYER_SIDEST_POSX _POKEMON_PLAYER_SIDEOUT_POSX  // 自分のポケモンが左から登場する場合のX
#define _POKEMON_PLAYER_SIDEST_POSY _MCSS_POS_Y(5)  // 自分のポケモンが左から登場する場合のY
#define _POKEMON_PLAYER_SIDEST_POSZ _MCSS_POS_Z(-10)  // 自分のポケモンが左から登場する場合のY
#define _POKEMON_FRIEND_SIDEST_POSX _POKEMON_FRIEND_SIDEOUT_POSX  // 相手のポケモンが右から登場する場合のX
#define _POKEMON_FRIEND_SIDEST_POSY _MCSS_POS_Y(-55)  // 相手のポケモンが右から登場する場合のY
#define _POKEMON_FRIEND_SIDEST_POSZ _MCSS_POS_Z(10)  // 相手のポケモンが右から登場する場合のY

//絵コンテ１２番
#define _POKEMON_PLAYER_SIDEIN_POSX _MCSS_POS_X( 60)  // 自分のポケモンが左から登場する場合のX
#define _POKEMON_PLAYER_SIDEIN_POSY _POKEMON_PLAYER_SIDEST_POSY  // 自分のポケモンが左から登場する場合のY
#define _POKEMON_PLAYER_SIDEIN_POSZ _POKEMON_PLAYER_SIDEST_POSZ  // 自分のポケモンが左から登場する場合のY
#define _POKEMON_FRIEND_SIDEIN_POSX _MCSS_POS_X(-45)  // 相手のポケモンが右から登場する場合のX
#define _POKEMON_FRIEND_SIDEIN_POSY _POKEMON_FRIEND_SIDEST_POSY  // 相手のポケモンが右から登場する場合のY
#define _POKEMON_FRIEND_SIDEIN_POSZ _POKEMON_FRIEND_SIDEST_POSZ  // 相手のポケモンが右から登場する場合のY

//絵コンテ１３番
#define _POKEMON_PLAYER_LEAVE_POSX _POKEMON_PLAYER_SIDEIN_POSX  // 自分のポケモンが上に退場する場合のX
#define _POKEMON_PLAYER_LEAVE_POSY _MCSS_POS_Y(100)  // 自分のポケモンが上に退場する場合のY
#define _POKEMON_PLAYER_LEAVE_POSZ _POKEMON_PLAYER_SIDEIN_POSZ  // 自分のポケモンが上に退場する場合のY
#define _POKEMON_FRIEND_LEAVE_POSX _POKEMON_FRIEND_SIDEIN_POSX  // 相手のポケモンが下に退場する場合のX
#define _POKEMON_FRIEND_LEAVE_POSY _MCSS_POS_Y(-160)  // 相手のポケモンが下に退場する場合のY
#define _POKEMON_FRIEND_LEAVE_POSZ _POKEMON_FRIEND_SIDEIN_POSZ  // 相手のポケモンが下に退場する場合のY



//手前のポケモンのサイズ
#define _FRONT_POKMEON_SCALE_SIZE   (1.2)
//奥のポケモンサイズ
#define _BACK_POKMEON_SCALE_SIZE   (1.2)

//ゆれる上下幅  大きいほど大きく揺れる
#define _WAVE_NUM    (8)



//SEタイミング -1はここでは当てれないので手であててある

#define _GTSSTART_SEQ_SE_DANSA	    (-1)		//デモ	wireless交換デモ	ポケモンジャンプ	ぴゅう
#define _GTSSTART_SEQ_SE_SYS_83			(-1)   //デモ	wireless交換デモ	ポケモン着地	とん！
#define _GTSSTART_SEQ_SE_W554_PAKIN	(1)		//デモ	wireless交換デモ	ポケモン、光る	ピカーーー！
#define _GTSSTART_SEQ_SE_BOWA2			(4)   //デモ	wireless交換デモ	ボールに入る	ボワ！
#define _GTSSTART_SEQ_SE_KON			(29)    //デモ	wireless交換デモ	ボールはねる	コンコンコン…
#define _GTSSTART_SEQ_SE_KON1			(45)  //デモ	wireless交換デモ	ボールはねる	コンコンコン…
#define _GTSSTART_SEQ_SE_KON2			(56)  //デモ	wireless交換デモ	ボールはねる	コンコンコン…
#define _GTSSTART_SEQ_SE_KON3			(63)  //デモ	wireless交換デモ	ボールはねる	コンコンコン…
#define _GTSSTART_SEQ_SE_TDEMO_001	(17)		//デモ	wireless交換デモ	ボールからキラキラした光が出る	キラリラリラリラ…
#define _GTSSTART_SEQ_SE_TDEMO_002	(107)		//デモ	wireless交換デモ	青い光が走りアングルが変わる	ショショショショショショショ…
#define _GTSSTART_SEQ_SE_W028_02		(152)	//デモ	wireless交換デモ	ボール飛び上がる	シュワッ！
#define _GTSSTART_SEQ_SE_TDEMO_006	(266)		//デモ	wireless交換デモ	ボールが天井に到達、光ってホワイトアウト	ピシャーーーン＋しゅわーーー
#define _GTSSTART_SEQ_SE_TDEMO_007	(350)		//デモ	wireless交換デモ	空中に光の輪ができてゆっくりまわる	キラキラ…キラキラ…
#define _GTSSTART_SEQ_SE_TDEMO_008	(636)		//デモ	wireless交換デモ	ポケモン猛スピードで飛んでいく	しゅわ！
#define _GTSSTART_SEQ_SE_TDEMO_009	(650)		//デモ	wireless交換デモ	ホワイトアウト	しゅわわわわわわ
#define _GTSSTART_SEQ_SE_TDEMO_010	(694)		//デモ	wireless交換デモ	ボールが上空から落ちてくる（長	キラキラ＋シューーーーン
#define _GTSSTART_SEQ_SE_TDEMO_011	(857)		//デモ	wireless交換デモ	ボールが地上に到達	パキャーーン！キラキラキラ…
#define _GTSSTART2_SEQ_SE_TDEMO_001	(858)		//デモ	wireless交換デモ	ボールからキラキラした光が出る(上のSEQ_SE_TDEMO_011と同時に鳴らす)	キラリラリラリラ…
#define _GTSSTART2_SEQ_SE_TDEMO_009	(896)		//デモ	wireless交換デモ	再度ホワイトアウト	しゅわわわわわわ
#define _GTSSTART2_SEQ_SE_KON			  (1048)   //デモ	wireless交換デモ	ボールはねる	コンコンコン…
#define _GTSSTART2_SEQ_SE_KON1			  (1068)   //デモ	wireless交換デモ	ボールはねる	コンコンコン…
#define _GTSSTART2_SEQ_SE_KON2			  (1082)   //デモ	wireless交換デモ	ボールはねる	コンコンコン…
#define _GTSSTART2_SEQ_SE_KON3			  (1092)   //デモ	wireless交換デモ	ボールはねる	コンコンコン…
#define _GTSSTART2_SEQ_SE_KON4			  (1103)   //デモ	wireless交換デモ	ボールはねる	コンコンコン…
#define _GTSSTART_SEQ_SE_TDEMO_004	(1123)		//デモ	wireless交換デモ	ボール光る	ほわ！
#define _GTSSTART2_SEQ_SE_BOWA2			(1166)  //デモ	wireless交換デモ	ポケモンが現れる	ボワ！
#define _GTSSTART3_SEQ_SE_TDEMO_001	(1159)		//デモ	wireless交換デモ	ポケモン現れる（星とびちる	キラリラリラリラ…




//----------------------------------------------------------------------------------------------
//ここから赤外線デモ
/*

●画面用 ica
ir_sean.ica
ir_sean.imd
ir_sean.ita

●カメラ位置
ir_ichi.ica

●カメラ注視点
ir_tyushiten.ica

●パーティクル
波紋
ir_tex001.spr（2回出してください）

交換シーン
ir_tex002.spr
ir_tex004.spr

ポケモン→モン玉（2回出してください）
ir_tex003.spr

●下画面用
under02.nsc
demo_backshot21.ncl
demo_backshot2.ncg


全部で660フレームあります。

*/


#define _IR_POKEUP_WHITEOUT_START (130)    //最初のホワイトアウト
#define _IR_POKEUP_WHITEOUT_TIMER (20)     //
#define _IR_POKEUP_WHITEIN_START  (180)    //
#define _IR_POKEUP_WHITEIN_TIMER (25)      //

#define _IR_POKECHANGE_WHITEOUT_START (409)  //交換後のホワイトアウト
#define _IR_POKECHANGE_WHITEOUT_TIMER (10)      //
#define _IR_POKECHANGE_WHITEIN_START  (420)  //
#define _IR_POKECHANGE_WHITEIN_TIMER (25)        //


#define _IR_DEMO_END (660)   //デモが終わる最後のフレーム

///パーティクルのタイミング
#define _IRTEX001_01_START   (-1)   ///波紋
#define _IRTEX001_02_START   (350)   ///波紋
#define _IRTEX002_01_START   (200)   ///交換シーン
#define _IRTEX003_01_START   (20)    ///ポケモン→モン玉
#define _IRTEX003_02_START   (600)    ///ポケモン→モン玉
#define _IRTEX004_01_START   (-1) ///交換シーン

//ポケモンの動きに関する時間
#define _IR_POKEMON_DELETE_TIME   (4)     //  ポケモンを消す

#define _IR_POKE_APPEAR_START   (250)   //    ポケモン登場 自分のは裏
#define _IR_POKE_APPEAR_TIME    (150)       //  ポケモン登場移動時間

#define _IR_OAM_POKECREATE      (598)  //最後にポケモンを出す時
#define _IR_POKEMON_CREATE_TIME  (615)  //ポケモンを出す時間

//ポケモンの位置
#define _IR_POKEMON_PLAYER_APPEAR_POSX _MCSS_POS_X( -50)  // 自分のポケモンのX
#define _IR_POKEMON_PLAYER_APPEAR_POSY _MCSS_POS_Y(-500)  // 自分のポケモンのY
#define _IR_POKEMON_PLAYER_APPEAR_POSZ _MCSS_POS_Z(10)  // 自分のポケモンのZ
#define _IR_POKEMON_FRIEND_APPEAR_POSX _MCSS_POS_X( 60)  // 相手のポケモンのX
#define _IR_POKEMON_FRIEND_APPEAR_POSY _MCSS_POS_Y(300)  // 相手のポケモンのY
#define _IR_POKEMON_FRIEND_APPEAR_POSZ _MCSS_POS_Z(-10)  // 相手のポケモンのZ

#define _IR_POKEMON_PLAYER_UP_POSX   _IR_POKEMON_PLAYER_APPEAR_POSX  // 自分のポケモンのX
#define _IR_POKEMON_PLAYER_UP_POSY   _MCSS_POS_Y(300)             // 自分のポケモンのY
#define _IR_POKEMON_PLAYER_UP_POSZ   _IR_POKEMON_PLAYER_APPEAR_POSZ  // 自分のポケモンのZ
#define _IR_POKEMON_FRIEND_DOWN_POSX _IR_POKEMON_FRIEND_APPEAR_POSX  // 相手のポケモンのX
#define _IR_POKEMON_FRIEND_DOWN_POSY _MCSS_POS_Y(-370)              // 相手のポケモンのY
#define _IR_POKEMON_FRIEND_DOWN_POSZ _IR_POKEMON_FRIEND_APPEAR_POSZ  // 相手のポケモンのZ

//手前のポケモンのサイズ
#define _IRC_FRONT_POKMEON_SCALE_SIZE   (2)
//手前のポケモンのα 0-31まで
#define _IRC_FRONT_POKMEON_ALPHA  (15)
//奥のポケモンサイズ
#define _IRC_BACK_POKMEON_SCALE_SIZE   (2)
//奥のポケモンのα 0-31まで
#define _IRC_BACK_POKMEON_ALPHA  (15)


//飛び上がっていくSEのスタートフレーム
#define _IRC_PMSND_START  (50)

//
#define _LONGSTART_SEQ_SE_DANSA (1)			//デモ	IR交換デモ	ポケモンジャンプ	ぴゅう
#define _LONGSTART_SEQ_SE_SYS_83	 (13)		//デモ	IR交換デモ	ポケモン着地	とん！

#define _SHORTSTART1_SEQ_SE_DANSA (1)			//デモ	IR交換デモ	ポケモンジャンプ	ぴゅう
#define _SHORTSTART1_SEQ_SE_SYS_83	 (13)		//デモ	IR交換デモ	ポケモン着地	とん！
#define _SHORTSTART2_SEQ_SE_DANSA (15)			//デモ	IR交換デモ	ポケモンジャンプ	ぴゅう
#define _SHORTSTART2_SEQ_SE_SYS_83	 (25)		//デモ	IR交換デモ	ポケモン着地	とん！
#define _SHORTSTART3_SEQ_SE_DANSA (27)			//デモ	IR交換デモ	ポケモンジャンプ	ぴゅう
#define _SHORTSTART3_SEQ_SE_SYS_83	 (39)		//デモ	IR交換デモ	ポケモン着地	とん！




#define _IRCSTART_SEQ_SE_W554_PAKIN (32)			//デモ	IR交換デモ	ポケモン、光る	ピカーーー！
#define _IRCSTART_SEQ_SE_BOWA2 (33)			//デモ	IR交換デモ	ボールに入る	ボワ！
#define _IRCSTART_SEQ_SE_KON	 (38)		//デモ	IR交換デモ	ボールはねる	コンコンコン…
//はねるぶんあてる
#define _IRCSTART_SEQ_SE_KON1	 (45)
#define _IRCSTART_SEQ_SE_KON2	 (56)
#define _IRCSTART_SEQ_SE_KON3	 (64)

#define _IRCSTART_SEQ_SE_TDEMO_001 (30)			//デモ	IR交換デモ	ボールからキラキラした光が出る	キラリラリラリラ…
#define _IRCSTART_SEQ_SE_W028_02 (110)			//デモ	IR交換デモ	ボール飛び上がる	シュワッ！
#define _IRCSTART_SEQ_SE_TDEMO_009 (130)			//デモ	IR交換デモ	一度目のホワイトアウト	しゅわわわわわわ
#define _IRCSTART_SEQ_SE_TDEMO_002 (241)			//デモ	IR交換デモ	ボールが空中移動（４秒分くらい）	ショショショショショショショ…
#define _IRCSTART_SEQ_SE_TDEMO_003 (320)			//デモ	IR交換デモ	ポケモン交換、すれちがい	フォアッ
#define _IRCSTART_SEQ_SE_W054_01 (364)			//デモ	IR交換デモ	波紋が広がり、ボールが飛ぶ	わわわわわわ…
#define _IRCSTART_SEQ_SE_W179_02 (409)			//デモ	IR交換デモ	中間部ホワイトアウト	キーン
#define _IRCSTART_SEQ_SE_W307_03 (484)			//デモ	IR交換デモ	ボール上空から落下	シュウウウウウ
#define _IRCSTART2_SEQ_SE_KON	 (494)	//	デモ	IR交換デモ	ボールはねる	コンコンコン…

#define _IRCSTART2_SEQ_SE_KON1	 (514)	//	デモ	IR交換デモ	ボールはねる	コンコンコン…
#define _IRCSTART2_SEQ_SE_KON2	 (528)	//	デモ	IR交換デモ	ボールはねる	コンコンコン…
#define _IRCSTART2_SEQ_SE_KON3	 (538)	//	デモ	IR交換デモ	ボールはねる	コンコンコン…
#define _IRCSTART2_SEQ_SE_KON4	 (547)	//	デモ	IR交換デモ	ボールはねる	コンコンコン…


#define _IRCSTART_SEQ_SE_TDEMO_004	 (569)		//デモ	IR交換デモ	ボール光る	ほわ！
#define _IRCSTART_SEQ_SE_TDEMO_005 (615)			//デモ	IR交換デモ	ボールからポケモン飛び出す	パキーーーン
#define _IRCSTART2_SEQ_SE_TDEMO_001 (605)			//デモ	IR交換デモ	飛び出したポケモンからキラキラした星が飛び出す	キラリラリラ…

