//=============================================================================
/**
 * @file  bugfix.h
 * @date  2010.07.12
 * @brief バグ対処用定義
 * 定義の有効・無効で決まります  反映させない不具合はコメントにしてください
 */
//=============================================================================

#pragma once

//*****************************************************************************
//--バグ掲示板 BTSXXXX
//*****************************************************************************

#define BUGFIX_BTS7711_20100712   //WiFi大会の選手証でトレーナー名が切れて表示されるバグの対処

/*
  [ BTS7714 ] ボックス
  メニュー「もちもの」でバッグに戻すときの「はい・いいえ」時に
  トレイ切り替えの矢印が表示されたままになっている
*/
#define BUGFIX_BTS7714_20100712

// 【フォルムNo.が「0」以外のガーメイルが、通信交換に出せない】不具合修正 
#define BUGFIX_BTS7718_100712

// 【ユンゲラーの通信進化デモ中に額の星マークが崩れている】不具合修正
#define BUGFIX_BTS7721_100712

//ふしぎなおくりものでカードを消したときBを押すと送りアイコンのみ消えるバグの修正
#define BUGFIX_BTS7724_20100712

//WIFIクラブ 募集開始にラグが存在する不具合修正
#define BUGFIX_BTS7736_20100712

// BTS7737:「メールをよむ」を選択すると、「U」マークも暗くなってしまうの修正
#define BUGFIX_BTS7737_20100712

//赤外線交換 たまごの交換の場合秘伝技検査を行わない
#define BUGFIX_BTS7742_20100712

//GDSのバトルビデオのマルチランキングで相手のポケモンが表示されているの修正
#define BUGFIX_BTS7719_20100712

//VCHATにノイズがのることで不具合が生じる事に対しての対処
#define BUGFIX_BTS7722_20100713

//ランダムマッチフリーモードにて、エラー画面で切断された後シーケンス移動するとはまりの修正
#define BUGFIX_BTS7733_20100712

//通信切断エラー後、画面を送っても対戦相手を募集している階層へもどることがあるの対処
#define BUGFIX_BTS7738_20100712

//  [ BTS7745 ] 図鑑　検索後のリストから再検索して戻るとリストのレイアウトが検索リストになっている
#define BUGFIX_BTS7745_20100712

// BTS7747:バッジがくすむスピードが２日ではなく0.75日になってしまっている（２日で３下がる）
// app/trainercard/badge_view.c
#define BUGFIX_BTS7747_20100712

//wifiクラブ話しかけ時にキャンセルすると、相手側のBUSYがのこってしまう不具合修正
#define BUGFIX_BTS7749_20100713

//ポケモン交換時に増殖してしまう、表示がかわってしまう不具合修正 BTS7752かねる
#define BUGFIX_BTS7750_20100713

//ドレスアップ確認画面で進むとはまる
#define BUGFIX_BTS7755_20100713

//*****************************************************************************
//--社内バグ掲示板 GFBTSXXXX
//*****************************************************************************

// WIFI交換の時にタイマーアイコンの更新処理と削除処理が重なりフリーズした不具合修正
#define BUGFIX_GFBTS1957_100712

#define BUGFIX_GFBTS1958_20100712 //WiFi大会でマッチング中に、はまるバグの対処

//ポケモン交換時にアイテム表示が出ていない不具合修正
#define BUGFIX_GFBTS1963_100712

#define BUGFIX_GFBTS1965_20100712  //仮素材削除対応

//WIFI大会でマッチング中に、対戦相手以外の選手証が表示される場合があるの対処
#define BUGFIX_GFBTS1961_20100712

// 【図鑑登録：有り得ないフォルムNo.のポケモンを登録したときにINDEXがずれる】不具合修正
#define BUGFIX_GFBTS1964_100712


//*****************************************************************************
//--ガイドライン掲示板 GLBTSXXXX
//*****************************************************************************



//*****************************************************************************
//--PGL掲示板
//*****************************************************************************

//【セーブデータのアップロードの管理画面について、寝たポケモンが表記されない】
#define BUGFIX_PGLBTS78_100712

