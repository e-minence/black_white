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

//【きねんリボン画面のリストにて十字ボタンでカーソル移動させた際のSEについて】
#define BUGFIX_AF_BTS7734_100806

//【ミュージカルのメンバー募集で、リーダー側に表示される「あと○人まで」の数字表記が重なってしまう事があった】
#define BUGFIX_AF_BTS7835_20100806

//通信切断エラー後、画面を送っても対戦相手を募集している階層へもどることがあるの対処
#define BUGFIX_BTS7738_20100712

//  [ BTS7745 ] 図鑑　検索後のリストから再検索して戻るとリストのレイアウトが検索リストになっている
#define BUGFIX_BTS7745_20100712

// BTS7747:バッジがくすむスピードが２日ではなく0.75日になってしまっている（２日で３下がる）
// app/trainercard/badge_view.c
#define BUGFIX_BTS7747_20100712

//【身代わり状態で複数回攻撃技を使用すると攻撃時のテキストが一瞬表示される】
//battle/btlv/btlv_effvm.c
#define BUGFIX_BTS7710_20100712

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

//Wi-Fi大会で大会終了時のセーブ中にエラーが起こるとバグを修正
#define BUGFIX_BTS7895_20100719

// 募集時にエラーが発生した時にタイミングよく募集を取りやめると、エラーが消えてしまう
#define BUGFIX_BTS7918_20100726  //こちらはいれバグになったため現状GetVersionがダミーで呼ばれている 必要ないのでコメントにしてよい8/12 k.ohno

// 募集時にエラーが発生した時にタイミングよく募集を取りやめると、エラーが消えてしまう
// NetErr_MainをNET_DEV_Mainの直後で呼ぶことで修正
//#define BUGFIX_BTS7918_20100812


//wifiランダムマッチでマッチング時のメモリーの断片化による不具合対処の為
// 1.本番用ROMではメモリーを100K多めに割り当てる
// 2.無理やりエラー画面を出す対処追加
#define BUGFIX_BTS7931_20100726

//ライブキャスターのお絵描きでフリーズ
//下の定義はロットが分かれた場合に有効にすること
#define BUGFIX_BTS7934_100726
//#define BUGFIX_BTS7934_100726_2ND

// 【レアカラーのポケモンを捕まえた際の図鑑にある「みつけた　すがたの　かず」の加算について】不具合修正
#define BUGFIX_BTS7936_20100726

//【ホワイトフォレストの草むら、水上でエンカウントしなくなることがあった】修正
#define BUGFIX_BTS7926_20100726

//ユニオンルームのチャットに友達じゃないのに「友達」と表示される
#define BUGFIX_BTS7903_20100726

//バトルビデオアップロードで5文字を超えたニックネームの不正ポケモンを送信すると自分のローカルビデオでのニックネームが文字化けする
#define BUGFIX_BTS7939_20100726

//Wi-Fi大会で選手証セーブ時にエラーや電源切断が起こるとセーブとサーバーの開催ワークが同期しなくなるバグ修正
#define BUGFIX_BTS7898_20100726

//【ユナイテッドタワーに配置されるNPCの初回テキストに表示される交換したポケモンが状況に合っていない】バグ修正
#define BUGFIX_BTS7927_20100726

//ライブ大会にて、設定した対戦回数を超えてバトルを行なう事が可能なバグ修正
#define BUGFIX_BTS7901_20100726

//【大会終了告知受領後のサーバ開催ワーク書き換えで通信エラーになると、以降再接続時にサーバ開催ワークが書き換えられない】
#define BUGFIX_BTS7890_20100726

//【進化後に特性が変化するポケモンがいる】
#define BUGFIX_BTS7955_20100728


//【海底遺跡および深海マップからマップ遷移した時の季節変化について】
//field/event_mapchange.c
//#define BUGFIX_AF_BTS7986_20100806

// ライブ通信のメイン画面でお礼アイコンのON/OFFを切り替えていると、合計人数の数字が重なって表示されることがある
#define BUGFIX_AF_BTS8022_20100806

//【IDイベントにて別IDのタマゴがボックスにあると、タマゴがカウントされる】
#define BUGFIX_AF_BTS7899_20100806

//親機探索メニューで特定の手順で子機が抜けれなくなる
//#define BUGFIX_AF_BTS7875_20100806

//申し込みを取りやめる「はい」「いいえ」ウィンドウをキャンセルすると下のウィンドウが欠ける
#define BUGFIX_BTS7887_20100806

//親機しかいないのに子機が参加している時のテキストが表示される
#define BUGFIX_BTS7886_20100806

//通信募集で誰かと接続した後にBでやめようとすると、相手がいなくても「解散しますか？」が出る
#define BUGFIX_BTS7889_20100806

//通信エラー画面で、蓋を閉じてバックライトOFFの状態でもAorBを押すとボタン操作が効いてしまう
#define BUGFIX_BTS7870_20100806

//ハイリンクで2番目に橋の上を行き来すると2番目に接続した相手の名前が一瞬表示される
#define BUGFIX_BTS7869_20100806

//ぐるぐる交換で募集を締め切ったのにアピールジャンプする
//#define BUGFIX_BTS7943_20100806

//ハイリンクで接続したターゲットが自分のハイリンクに表示される事がある
#define BUGFIX_AF_BTS8033_20100902

//ハイリンク専用画面に一瞬、侵入者とターゲットの表示が混在して表示される
//#define BUGFIX_AF_BTS7221_20100903

//NTR-CAPTUREですれ違いデータを受信しなくなる
//※未対処です。その為、現状報告をログとして残す意味でここに記します
//  NTR-CAPTURE限定で発生しているバグです。
//  しかしバグ報告があったバージョンであっても社内で再現が出来ていません。
//　社内のNTR-CAPTURE、IS-DEBUGGER、双方で何時間も再現確認を企画にしてもらいましたが、
//  再現は出来ておらず、マリオクラブ様の方でも再現が出来なくなっています。
//  ソースで怪しい部分を探りましたが、こちらも特におかしな所は見つかっていません。
//    2010.09.06(月) matsuda
//#define BUGFIX_AF_BTS3936_20100906

//※未対処です。その為、現状報告をログとして残す意味でここに記します
//このバグを対処するには「侵入する側」が、「受け取ったビーコンの主は調査レーダー画面を開いている」
//というのを判別する必要があります。
//現在の送信ビーコンデータの中にはそれを判別するデータは含まれていませんので、
//修正を行う場合、GBS_BEACON構造体のpaddingから1bit新たに用意し、
//調査レーダー画面を開いた時点でそのビットをON(出るときにはOFF)、
//対象のビーコンを受け取った「侵入する側」でフラグを参照し、
//対象に含めないようにする事で対処になります。
//(intrude_comm.cの_SetScanBeaconData関数内で捨てます)
//  2010.09.14(火) matsuda
// ※但し、既に市場に出回っている何百万本というシャチがそのフラグを持っていないため、
//   修正はほぼ意味がないものだと思います。
//#define BUGFIX_AF_BTS7622_100914

//※未対処です。その為、現状報告をログとして残す意味でここに記します
//再現性も無い事からソースレベルで調査を行いましたが原因特定にいたりませんでした。
//次期バージョンに備え関連箇所を述べます。
//・対象ファイル　intrude_subdisp.c
//・症状　背景BGの色、街アイコンの色、この両方が通常であれば同じ色になっているはずが
//　　　　背景BGは侵入先の色、街アイコンは自分の色(報告では侵入者なのでNetID 0番)
//        になっている事が問題です。
//・見解　症状発生後、即エラーが発生していますので、
//        背景BG=侵入先のエリア番号(NetID 1番)
//        街アイコン=エラーが発生している為、0番
//        をそれぞれ参照しているのではないかと考えています。
//・ソース箇所
//        背景BGの更新は_IntSub_BGColorUpdate関数です。
//        街アイコンの更新は_IntSub_ActorUpdate_TouchTown関数です。
//        どちらもcomm->now_palace_areaで自分がいるエリアを判定しているので、
//        ずれた表示になるのはありえない、、、はず。という所です。
//        comm->now_palace_areaの更新は_IntSub_CommParamUpdate関数で行っています。
//        ※エラー時は全体初期化するため、now_palace_areaは0になります。
//#define BUGFIX_AF_BTS7998_100914

//【全滅すると、戻り先のポケセンから出るまでは、すれ違った場所が正常に表示されない】
#define BUGFIX_AF_BTS7826_20100806

//【ふしぎなカードに関する選択肢を決定する際、Aボタンを押し続けると、再度選択肢が表示されます】
#define BUGFIX_AF_BTS7960_20100806

//【Wi-Fi接続の選択肢表示前に決定が入力出来てしまう事があります】
//#define BUGFIX_AF_BTS7810_20100806

//【ライブ大会デジタル選手証取得時の赤外線通信画面からエラー画面へ移行することがあった】
//#define BUGFIX_AF_BTS7976_20100806

//【ニックネームの入力画面でポケモンアイコンをタッチするとアイコンが右にずれる】
#define BUGFIX_AF_BTS7858_20100806

//【ハイリンクで侵入されている場合、文字入力パレット画面で2種類のテキストが重なる】
#define BUGFIX_AF_BTS8006_20100806

//【タウンやシティにいる状態でタウンマップを使用した時のアイコンの表示について】
//#define BUGFIX_AF_BTS8005_20100806

//【「そらをとぶ」画面から抜けようとした直後に決定すると「そらをとぶ」が実行される】
#define BUGFIX_AF_BTS8009_20100806

//【ライブ大会、設定したバトル回数の最後のバトル後、アッパーバージョンとのバトルビデオが再生できます】
#define BUGFIX_AF_BTS8030_20100806

//【アイテム「しんかいのウロコ」が配置されている浅瀬で、「なみのり」から浅瀬に上陸出来ない箇所がある】
//#define BUGFIX_AF_BTS7995_20100806

//【「なみのり」で移動中、沿岸にいるNPCに向かって移動すると、上陸できてしまいNPCと主人公が重なることがあった】
//#define BUGFIX_AF_BTS7935_20100724

// 【全国図鑑未入手の状態で全国図鑑のポケモンを捕獲した時の図鑑No.の表記について】zkn.gmmを修正してコミットしただけ。
#define BUGFIX_AF_BTS7908_20100903

//自己紹介を登録する時に単語入力のない定形文を選択するとNPCがちぐはぐなことをいうバグへの対処
#define BUGFIX_AF_BTS7862_20100902

//【調査報告画面の順位欄にタッチ判定のない部分が生まれる】
#define BUGFIX_AF_BTS7816_20100906

// 【子機側のテキストの自動送りの速度が速く途中で切れているように見える】
// 素材と子機ROMの更新のみになります。
//multiboot2/spec/main.sgn
//multiboot/spec/main.sgn
//prog/filetree/dl_rom/child.srl
//prog/filetree/dl_rom/child2.srl
//resource/message/src/mb_child_movie.gmm
#define BUGFIX_AF_BTS7717_20100906

//【ポケモン情報画面でポケモンをタッチした直後にタマゴの情報画面に切り替えるとタマゴが背面グラフィックになる】
#define BUGFIX_AF_BTS7848_20100913

//【ねむけ玉発動時にポケモンが眠らないことがあった】
//#define BUGFIX_AF_BTS8034_100913

//【「ならびかえ」のキャンセルをする際に「戻る」アイコンをタッチしても、アイコンの点滅演出がありません】
//#define BUGFIX_AF_BTS7984_100913

//【「たたかうわざ」画面で空スロットの1つ上の技をタッチで選択すると、移動できない方向に矢印アイコンが表示される】
//#define BUGFIX_AF_BTS7973_100913

//【2種類のテキストが重なることがあった】
//#define BUGFIX_AF_BTS7924_100914

//【ポケモン情報画面を消した後、ボックスがオートでスクロールしてしまう】
#define BUGFIX_AF_BTS7735_20100914

//【左手親指のガイドに指を乗せた際に画面上に表示されるガイドの一部がちらつく】
#define BUGFIX_AF_BTS7817_20100914

//【大会終了処理でバトルボックス解除メッセージが表示されない】
#define BUGFIX_AF_BTS7944_20100806

//【ボックスに空きがあると「ようすをみる」画面においてポケモンを下方向へ切り替えることができない】
#define BUGFIX_AF_BTS8044_20100914

//【ローテーションバトルで「たたかう」を選択した直後、「シューター」ボタンが「バッグ」ボタンに変化している】
//battle/btlv/btlv_input.c
//この定義を有効にする場合はresource/battle/battgra_wb.scrにbattle_w_bg0l.NSCRを追加しないとMAKEが通りません
//#define BUGFIX_AF_BTS7728_20100914

//【じゅくがえりの『アン』が所持しているミノマダム（虫・地面）が「リーフストーム」を使用する】
//tr_tool/tr_tool.c
//#define BUGFIX_AF_BTS7992_20100914

//【バトル中におけるトレーナーの影の表示についての質問】
//system/mcss.c
//#define BUGFIX_AF_BTS8038_20100914

//【ライブ大会、ポケモン選択画面の「決定」ボタンが高速で点滅します】
//#define BUGFIX_AF_BTS7843_100914

//【ポケモンの配信を受け取っている時、ウインドウの下部にポケモンのグラフィックが一瞬表示される】
#define BUGFIX_AF_BTS7881_20100914

//【ポケモン選択画面で暗転したままになり、選択時間切れでVS画面に移行後暗転(白転)したままはまることがあった】
//#define BUGFIX_AF_BTS7982_20100914

//【配信期間切れかつ大会終了後接続時に「データを　送っています…」で通信切断するとはまる】
//またこのBTS7947の対処でBTS7895についてもされます修正
#define BUGFIX_AF_BTS7947_20100914

//【通信エラー表示中に回転しているリンクのSEが鳴り続ける】
#define BUGFIX_AF_BTS7759_20100914

// BTS7983:ポケモンリストからタッチし続けているとメール閲覧画面で即終了してしまう
// src/app/mail/mailview.c
#define BUGFIX_AF_BTS7983_20100915

// BTS7916:右下の「×」のタッチ判定が広い（リストタッチした時の「もどる」と同じ範囲になっている）
// src/app/waza_oshie/wo_main.c
#define BUGFIX_AF_BTS7916_20100915

//【バトルビデオのランキング画面でスライドさせるとポケモンのアイコンが一瞬表示化けする】
//※未対処です。その為、現状報告をログとして残す意味でここに記します
//ポケモンアイコンを読み込む処理負荷が高く、一瞬アイコンがぱしっています。
//これを解決するためには、余分に上下に1OBJ（もしくはリソース）を呼んでおき、
//入れ替えて表示する必要があります。現在は直接見える場所に転送しているためパシリます。
//（VBLANKでも試しましたが転送が間に合いませんでした）
//ソースの変更箇所が多くなるため修正しておりません。アッパー開発時に対処の予定です。
//#define BUGFIX_AF_BTS7847_20100916

// BTS7892:ぐるぐる交換でポケモンリストからステータスを見に行き、
// 戻ってくるとカーソルが1匹目（左上）に移動してしまう
// src/net_app/guru2/guru2.c
#define BUGFIX_AF_BTS7892_20100915

//BTS7715【8文字のアイテム名のとき、右端が枠に重なってしまうものがある】のバグ修正
#define BUGFIX_AF_BTS7715_20100916

//【「ようすをみる」と「ポケモンを選ぶ」で、表示位置が異なるテキストがある】
#define BUGFIX_AF_BTS7723_20100916

//【フィーリングチェックで相手と接続できなくなることがある】のバグ修正
//タイムアウトを追加し、メニューのシーンのずれを消しました
//#define BUGFIX_AF_BTS7929_20100915

//【大会選手証所持状態からのWi-Fiユーザー情報更新フローについて】
#define BUGFIX_AF_BTS7878_20100916

//【「ポケモンを探す」で見つかった相手の表示が残り続ける】
#define BUGFIX_AF_BTS7863_20100916

//【残り1体のポケモンがノーダメージの状態でバトルに勝利し、デバッグモードから対戦相手の履歴を確認した時、残りのポケモン数が異なっている】
//#define BUGFIX_AF_BTS7957_20100916

//【調査報告の下画面に表示される「合計人数」と「今日の人数」のテキスト表示位置が若干ずれている】
#define BUGFIX_AF_BTS7803_20100916

//【アララギ博士の捕獲デモ中のモンスターボールのアイコンの表示について質問】
//battle/btlv/btlv_scd.c
#define BUGFIX_AF_BTS7891_20100917

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

//  バトルレコーダーでセーブ中エラーになるとはまるバグを修正
#define BUGFIX_GFBTS1996_20100719

//  イベントのライブキャスターにデバッグが残っている
#define BUGFIX_GFBTS1997_20100726

//フィールドに配置してあるモンスターボールのアイテムを取ると、ボールが一瞬化けます。を修正
#define BUGFIX_GFBTS2003_20100726

// 育て屋のポケモンがタマゴを産んだ時、手持ちがMAXでも呼び止められてしまうバグを修正
#define BUGFIX_GFBTS2008_20100726

// [GFBTS2013] 殿堂入りＰＣ画面の回収表示が２桁になっている
// pc_dendou.gmmも修正されてます
#define BUGFIX_AF_GFBTS2013_20100806

// ライブ通信メイン画面でメンバーリストのランク表示がスクロール時にバグる現象を修正
#define BUGFIX_AF_GFBTS2002_20100806

//【ふしぎなおくりもの：カードアルバムの矢印がチラつく】
#define BUGFIX_AF_GFBTS2004_20100806

//ランダムマッチのポケモン選択画面で片方がフリーズする
#define BUGFIX_AF_GFBTS2015_100806

//【Wi-Fi大会：バトルボックスの技を入れ替えるとマッチングしなくなる】
#define BUGFIX_AF_GFBTS2017_100806

//D09　チャンピオンロード外のBLOCK（１，１）　LOCAL(２７，６) RAIL INDEX　１８　FRONT 1　SIDE　２の見た目と合わない場所にエンカウントアトリビュートがあります。
//resource/fldmapdata/rail_data/rail/d09.atdat
//resource/fldmapdata/rail_data/rail/d09.attr
//#define BUGFIX_AF_GFBTS1999_100903

//【ヤグルマの森外マップ（D02/map18_19）のLOCAL30,22〜30,23にかけて、外観では弱草が敷かれていますが、設定されているアトリビュートが水溜りになっているため、エンカウントが起こりません。】
//resource/fldmapdata/landata/land_res/map18_19.bin
//#define  BUGFIX_AF_GFBTS1960_100903

//【ヤグルマの森外マップ（D02/map18_19）のLOCAL14,13にて、通常地面にも関わらず、水溜りのアトリビュートが設定されており、歩くと波紋が生じます。】
//resource/fldmapdata/landata/land_res/map18_19.bin
//#define  BUGFIX_AF_GFBTS1959_100903

//【P2ラボ（D13）のBGM指定が全ての季節で春のものになっています。】
//下記のとおり、バグ修正定義を追加してありますが実際にはバイナリ変更のため対応箇所には影響しません。
#define BUGFIX_AF_GFBTS1966_100906

// すれ違い調査隊本部：ポスターへの話しかけ時にSEが当たっていない。
#define BUGFIX_AF_GFBTS1915_20100906

// すれ違い調査隊隊長：依頼の確認項において、不要なLAST_KEYWAIT()とメッセージクローズが入っている。
#define BUGFIX_AF_GFBTS1998_20100906

//チャンピオンロード
//レール座標（４８，１，４）の位置、（ポケモンリーグとの接続から入って左にある段々の上から２つめ）
//から左に移動している途中に、
//下を押して滑り降りると移動不可能な場所に滑り降りてしまう。
#define BUGFIX_AF_GFBTS2021_20100923

// 【レールマップ】波乗り問題
//．レールマップであり、
//．レール座標がfront=0 side=0であり、”アトリビュート数が特定の値”のある位置で、
//．sideの値が少なくなる方向に向いている状態で
//．ポケモンリストから波乗りを使ったとき
//
//波乗りが出来てしまい、移動が不可能な状態となります。
//さらに、そのままセーブしてしまうとユーザーが自力で復帰することが不可能になります。
// prog/src/field/rail_attr.c
#define BUGFIX_AF_GFBTS2025_20100928

// 【トレーナーカード】ホワイトバージョンのトレーナーカードにて、カードカラーが
// 最高ランクになった際、カード裏のスコアボックスを囲っている枠線の右下にドット欠けがあります。
// ※BG素材のみコミットのためソースコードは変更ありません
#define BUGFIX_AF_GFBTS1936_20100914

// リターンマークが入っているのにSEが決定音である不具合修正
// BTS7987も同じバグでした
//#define BUGFIX_AF_GFBTS1806_20100914

//【ふしぎなおくりもの：カードいっぱい時の整理シーケンスでテキストに改行コードがない】
//このバグを修正するためにはGMMに改行コードを追加する必要があるため、
//mystery.gmmも同時にコミットしています。
//上記のバグはgmmだけで修正可能なのですが、gmmに改行コードを追加すると、テキスト送りが2回になるというバグがでてしまうため、以下の定義で修正しています。
//ですので以下の定義のみ外してしまうと、テキスト送りが2回でるというバグがでてしまいます！コメントアウトしてはいけません！
//また、BTS7971もこのGMMのコミットで修正されております。BTS7971はプログラム的な変更はございません
#define BUGFIX_AF_GFBTS2005_20100915

//【天気中にYボタンメニューを開くと後ろの雪等のOBJが透過されず黒く見えてしまう】
//#define BUGFIX_AF_GFBTS1918_20100915

// [ GFBTS2020 ] ボックス：ポケモンアイコンが消える
#define BUGFIX_AF_GFBTS2020_20100924


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


//*****************************************************************************
//--ガイドライン掲示板\\\ GLBTSXXXX
//*****************************************************************************
//バトルレコーダーでビデオ削除中のセーブでスリープが効いてしまう
#define BUGFIX_LTC0003_20100726

//*****************************************************************************
//--質問掲示板
//*****************************************************************************

//タマゴ生成の抽選カウンタがフィールド破棄の度にリセットされるため、タマゴが発生しにくいバグを修正
#define BUGFIX_QABTS692_20100726


//*****************************************************************************
//--スクリプト不具合修正コメント　ここから↓
//
// スクリプトの不具合修正に関しては、修正内容にgmmが絡むものが多いため
// タグ定義による修正反映切替を行っていません。
// bugfix.hへはsvnのログ管理のためにコメントを残しています
//
// 定義をコメント化しても、修正が未反映にはならないのでご注意ください
//
// ログ管理のためプログラマがコミットしていますが担当者は別です。
// 担当者については、各タグのコメントを参照してください。
//*****************************************************************************

/*
 * GFBTS1983
 * サンドイッチ屋さんバイトイベントでの送り記号表示修正
 *
 * -h04.ev
 * -h04.gmm
 *
 *  担当：村上
 */
#define BUGFIX_AF_GFBTS1983_100914

/*
 * GFBTS1973
 * H03 暴走族総長との戦闘イベントで、「はい／いいえ」の表示前にキー待ち記号が表示されている
 *
 * -h03.gmm
 *
 *  担当：村上
 */
#define BUGFIX_AF_GFBTS1973_100914

/*
 * GFBTS1941
 * プラズマ団の隠れ家前、アーティVSプラズマ団イベントで送り記号が足りない部分がある
 *
 * -c03p07.ev
 *
 *  担当：村上
 */
#define BUGFIX_AF_GFBTS1941_100914

/*
 * GFBTS1940
 * アトリエ、指定タイプポケモンをみせるイベントでの送り記号不具合修正
 *
 * -c03r0201.gmm
 *
 *  担当：村上
 */
#define BUGFIX_AF_GFBTS1940_100914

/*
 * GFBTS1776
 * C03R1101：ポケモン盗難後、ゲーチスイベントで
 * アーティが部屋から出て行くときに、マットを越えた場所で消えている不具合修正
 *
 * -c03r1101.ev
 *
 *  担当：村上
 */
#define BUGFIX_AF_GFBTS1776_100914

/*
 * GFBTS1737
 * C03R0601：ヒウンゲートのサポート戦闘イベント：
 * サポートの登場位置が進入不可の場所から出てくる不具合修正
 *
 * -c03r0601.ev
 *
 *  担当：村上
 */
#define BUGFIX_AF_GFBTS1737_100914

/*
 * GFBTS1968
 * 博士のヒヒダルマ説明イベント：不要なメッセージクローズを削除
 *
 * -d03r0101.ev
 *
 *  担当：杉中
 */
#define BUGFIX_AF_GFBTS1968_100915

/*
 * GFBTS1902
 * 博士のポケモンセンター案内イベント：不要なキー待ち記号を削除対処、ラストキーウェイトを追加
 *
 * -t02pc0101.ev
 * -t02pc0101.gmm
 *
 *  担当：杉中
 */
#define BUGFIX_AF_GFBTS1902_100915

/*
 * BTS8023
 * ワンダーブリッジのミネズミイベント・二重キー待ち修正（GMM側で送りマーク削除・スクリプトで文字送り追加）
 *
 * -h05.ev
 * -h05.gmm
 *
 *  担当：杉中
 */
#define BUGFIX_AF_BTS8023_100917

/*
 * BTS7904
 * 認定トレーナーメッセージ表示条件修正
 *
 * -c08gym0101.ev
 *
 *  担当：水口
 */
#define BUGFIX_AF_BTS7904_100917

/*
 * BTS7910
 * ショウロのバトルボックス説明が実挙動と異なる。メッセージ修正
 *
 * -c01r0102.gmm
 *
 *  担当：松宮
 */
#define BUGFIX_AF_BTS7910_100916

/*
 * BTS7951
 * マルチトレインの説明が設定と異なる。メッセージ修正
 *
 * -t05.gmm
 *
 *  担当：松宮
 */
#define BUGFIX_AF_BTS7951_100916

/*
 * BTS7965
 * TV番組：「わざと暮らす-ちょうのまい」の内容が不適切。メッセージ修正
 *
 * -tv01_01_scr.gmm
 *
 *  担当：松宮
 */
#define BUGFIX_AF_BTS7965_100916

//*****************************************************************************
//--スクリプト不具合修正コメント　ここまで↑
// 定義をコメント化しても、修正が未反映にはならないのでご注意ください
//*****************************************************************************

//*****************************************************************************
//--BTSにアップされていない不具合
//*****************************************************************************

/*
  ボックスからステータス画面を呼び出すときのワーク初期化修正
  詳細：
    ステータス画面の呼び出しに使用するワークに、一部初期化していないメンバーが
    あるため、ステータス画面を呼び出した際、すぐに抜けてしまうなどの不具合が
    発生する場合があります。
*/
//#define BUGFIX_AF_BOX00_20100806

/*
  レポート画面のRTC取得関連をGFLIBの処理に変更
  詳細：
    RTC取得関連で、SDKの関数を直接使用にているため、
    取得できなかった場合に表示がおかしくなるので、GFLIBの処理に変更
*/
//#define BUGFIX_AF_REPORT00_20100806


/*
  ロットチェック違反防止
  詳細：
    TGID変更しない処理は地下の為にあったが、現状は必要ないので封鎖した
*/

//#define BUGFIX_AF_GF_TGID_20100806

/*
  ロットチェック違反修正
  詳細：
    WIFIクラブログアウト時にはセーブが必須
*/

//#define BUGFIX_AF_GF_CLUBSAVE_20100806


/*
  dwc_initの最初のエラーは初期化時しか出さないように修正
  詳細： DS本体情報が壊れた時に出すエラーをその時だけに抑制しないと、画面が乱れる
*/
//#define BUGFIX_AF_GF_DWCINITERR_20100812

/*
     ゲームシンクアップロード時にはNoerrをたてる必要がある
     これにより通信エラーが起きてもセーブを続行する為
*/
//#define BUGFIX_AF_GF_GSYNC_NOERROR_20100812

/*
  ロットチェック
  ２０１０８のエラーはWIFILoginIDがきえる
  ログインを繰り返す場合、DWC_CheckHasProfileを調べなおす修正が必要である
*/
//#define BUGFIX_AF_GF_LOGIN_20108_20100812

/*
  レシーブバッファのアライメント修正
  本来３２ビットアライメントが必要
  GFLIBの修正も必要  net_system.c 230
*/

//#define BUGFIX_AF_GF_BUFF32_20100812

/*
  親機の割り込みでハンドルを消さないように修正
*/
//#define BUGFIX_AF_GF_HANDLE_20100812


/*
  日韓バージョン以外で、メッセージスピード設定を「はやい」以上にした場合
  観覧車デモのメッセージ描画速度が異常にゆっくりになってしまうバグの修正
*/
#define BUGFIX_AF_MSGSPEED_DEMO3D_20100806

/**海外版に向け、PM_DEBUGでくくったところ**/
/*
gym_dragon.c      738行目
gym_ground_ent.c  177行目
gum_insect.c      1016行目
以上のファイルでデバッグ処理が有効になっていた箇所がありましたので、PM_DEBUGでくくりました。
日本版では、デバッグ機能が有効な状態ですが、実害が無かったためそのままにしてありました。
*/

/*

  GFBTS1996の状態でノイズを１００％発生させると、フリーズします。

  GFBTS1996はLANケーブルやAP電源抜きのには対処されておりますが、
  ノイズ１００％のときは対処されております。
  下記定義をはずしますと、ノイズ１００％のときも対処いたします

*/
//#define BUGFIX_AF_BTLVIDEO00_20100806

//国、地域の不正チェックが足りていない
//#define BUGFIX_COUNTRY_NGCHECK_20100806

/**海外版に向け、ソースにコメント追記した所**/
/*
Signedで取得する必要がある MSGSPEED_GetWait()の戻り値を、
Unsignedの変数に取得してしまっているが、現状変数が未使用のために
実害がない箇所に関して、注意喚起のためのコメントを残した

src/field/field_comm/beacon_view.c _sub_DataSetup()内 710行目付近
src/app/beacon_detail/beacon_detail.c _sub_DataSetup()内 453行目付近
*/


/*
 * 文字列比較ルーチンのミス（途中まで一致しているだけで完全一致とみなしてしまう場合がある）修正
 *
 * 担当：田谷
*/
#define BUGFIX_AF_GFBTS2022_100929
