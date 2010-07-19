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

// 【マルチバトルを行なった際の前回の記録でのポケモンが（味方・相手）VS（自分・相手）という表示になる】不具合修正
#define BUGFIX_BTS7753_100713

//手持ち不正のときの通信エラーをとじた後、３０秒間ボタン操作が効かなくなるバグを対処
#define BUGFIX_BTS7760_20100713

// BTS7754:対戦結果の「WIN」のパーティクルが崩れることがある。
// demo/comm_btl_demo/comm_btl_demo.c
#define BUGFIX_BTS7754_20100713

//Wi-Fi大会見せ合い〜結果画面までにLANケーブルを抜き、差し戻した場合、切断回数が増加しないバグを修正
#define BUGFIX_BTS7762_20100713

// [ BTS7778 ] タマゴの中身が図鑑登録される
#define BUGFIX_BTS7778_20100713

//通信エラー後、画面を切り替えてもリングのSEが鳴り続けることがあったバグを修正
#define BUGFIX_BTS7775_20100713

//Wi-Fi大会で内部的にマッチングしている両者がサーチを中断するとエラー移行後に選択肢が残る問題を修正
#define BUGFIX_BTS7769_20100713

//VCHATで親機子機交互に変更するとつながらない不具合
#define BUGFIX_BTS7785_20100713

// [ BTS7790 ] ボックス 「にがす」実行後に不必要なメッセージウィンドウが表示されている
#define BUGFIX_BTS7790_20100714

// [ BTS7793 ] ボックス 道具を持っていないのに「メールは〜」と表示される
#define BUGFIX_BTS7793_20100714

// [ BTS7796 ] 図鑑 LRを同時押しするとリストが崩れる
#define BUGFIX_BTS7796_20100714

//GTSにて選択したアイコンと、実際の挙動が異なってしまうバグの修正
#define BUGFIX_BTS7795_20100714

//GTSにて【選ぶ相手がいないにも拘らず、相手を選ぶよう促すメッセージが表示される】の対処
#define BUGFIX_BTS7794_20100714

//GTSにて受信強度アイコンにマスクがかかっているバグを修正
#define BUGFIX_BTS7802_20100714

// 【検索結果が12個だった場合、画面のスクロールを行えない状況でスクロールバーが表示される】不具合修正
#define BUGFIX_BTS7812_20100714

//DWCLIB割り込み内でヒープが足りなくなった為 OS_Panicを入れました 保留案件
#define BUGFIX_BTS7819_20100715

// [ BTS7820 ] バッグ ポケットのＹ登録のチェックボックスが反応しないのに表示されている
#define BUGFIX_BTS7820_20100714

//大会データアクセス時にはまってしまうことがあるのバグを対処
#define BUGFIX_BTS7821_20100714

///BTS7823  【全滅した際の画面に受信強度アイコンの表示がありません】の対処
#define BUGFIX_BTS7823_20100715

// 通信対戦時に両者ひん死入れ替えが発生した際、ポケモン選択が終わった側に「通信待機中」のメッセージを表示する。
#define BUGFIX_BTS7825_20100715

//タッチとキーが同時に入力された場合のリストの不具合の対処
#define BUGFIX_BTS7830_20100715

//ランダムマッチにて放置していたところエラーメッセージでアサートが表示されることがあったのバグを修正
#define BUGFIX_BTS7842_20100715

//ライブ大会にて、バトル終了後ROM同士を引き離すと片方が暗転状態ではまるバグの修正
#define BUGFIX_BTS7837_20100715

//フリーモードにて不正操作を行っても子機のみ録画可能になっている件を修正
#define BUGFIX_BTS7849_20100715

//自機、【十字ボタンの同時入力で、入力している方向と違う方向に進む】を修正
#define BUGFIX_BTS7846_20100715

//ダウンロードライブラリにタイムアウトを追加
#define BUGFIX_BTS7852_20100715

//タイムアウトを追加
#define BUGFIX_BTS7868_20100716

//ミッション受注後、通信遅延が発生すると受注したのに参加していない、という現象が発生する
#define BUGFIX_BTS7874_20100716

//LV1ご褒美の時にはFinishを送らない  エラー処理には行かないように修正しました
#define BUGFIX_BTS7876_20100716

//ランダムマッチにてBキャンセル中にWiFiから切断されるとはまるバグの修正
#define BUGFIX_BTS7867_20100716

// 【戦闘終了後、進化したと同時に新しい技を覚えさせようとした際、受信強度アイコンが表示されていない】不具合修正
#define BUGFIX_BTS7893_100719


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

//バトルレコーダーのポケモンリストの並びがおかしいの対処
#define BUGFIX_GFBTS1974_20100713

//【一本橋からエフェクトの発生グリッドに落ちても、特殊エンカウントイベントが起動しない】を修正
#define BUGFIX_GFBTS1967_100713

//PGLサーバにプロファイルIDを送る必要があるの対処
#define BUGFIX_GFBTS1976_20100714

//【自機がグリッド間移動中に通信エラー画面が起動するとPOSイベントをすり抜けるバグ】を修正
#define BUGFIX_GFBTS1975_100713

//【自機がグリッド間移動中に侵入相手から場取るミッションによる話しかけが起動するとPOSイベントをすり抜けるバグ】を修正
#define BUGFIX_GFBTS1979_100714

//GFL_PROC_FreeWorkの場所を適切な位置に移動
#define BUGFIX_GFGTS1978_20100714

// ポケモン交換をいったんとりやめてもう一回ポケモンを交換するとCheckSumErrorになる不具合を修正
#define BUGFIX_GFBTS1981_20100714

// レベルアップ時、クライアント側データのタイプをデフォルト値に書き戻してしまう不具合を修正
#define BUGFIX_GFBTS1990_20100716

//  通信のクロスチェックを行った結果の修正箇所です
#define BUGFIX_GFBTS1989_20100716

//  WiFi大会の成績データが初期化されている箇所がある
#define BUGFIX_GFBTS1987_20100716

//  全滅後、フィールドに戻ってくると、c-gearのダウンロードスキンにアルファがかかっていない。 
#define BUGFIX_GFBTS1994_20100719

//  イベントのライブキャスターで通信アイコンが出ていない
#define BUGFIX_GFBTS1995_20100719

//*****************************************************************************
//--ガイドライン掲示板 GLBTSXXXX
//*****************************************************************************



//*****************************************************************************
//--PGL掲示板
//*****************************************************************************

//【セーブデータのアップロードの管理画面について、寝たポケモンが表記されない】
#define BUGFIX_PGLBTS78_100712

// レスポンスエラーを増やす対応
#define BUGFIX_PGLBTS85_20100715


//*****************************************************************************
//--外部配信ROM掲示板
//*****************************************************************************

//タマゴを配信したときに孵化歩数が正しく反映されない
#define BUGFIX_OUTROMBTS32_20100716
